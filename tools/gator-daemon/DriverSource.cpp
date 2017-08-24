/**
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

//                                            
#define __STDC_FORMAT_MACROS

#include "DriverSource.h"

#include <fcntl.h>
#include <inttypes.h>
#include <sys/prctl.h>
#include <unistd.h>

#include "Buffer.h"
#include "Child.h"
#include "DynBuf.h"
#include "Fifo.h"
#include "Logging.h"
#include "Proc.h"
#include "Sender.h"
#include "SessionData.h"

extern Child *child;

DriverSource::DriverSource(sem_t *senderSem, sem_t *startProfile) : mBuffer(NULL), mFifo(NULL), mSenderSem(senderSem), mStartProfile(startProfile), mBufferSize(0), mBufferFD(0), mLength(1) {
	int driver_version = 0;

	mBuffer = new Buffer(0, FRAME_PERF_ATTRS, 4*1024*1024, senderSem);
	if (readIntDriver("/dev/gator/version", &driver_version) == -1) {
		logg->logError(__FILE__, __LINE__, "Error reading gator driver version");
		handleException();
	}

	//                                                     
	if (driver_version != PROTOCOL_VERSION) {
		if ((driver_version > PROTOCOL_DEV) || (PROTOCOL_VERSION > PROTOCOL_DEV)) {
			//                                                      
			logg->logError(__FILE__, __LINE__,
				"DEVELOPMENT BUILD MISMATCH: gator driver version \"%d\" is not in sync with gator daemon version \"%d\".\n"
				">> The following must be synchronized from engineering repository:\n"
				">> * gator driver\n"
				">> * gator daemon\n"
				">> * Streamline", driver_version, PROTOCOL_VERSION);
			handleException();
		} else {
			//                         
			logg->logError(__FILE__, __LINE__,
				"gator driver version \"%d\" is different than gator daemon version \"%d\".\n"
				">> Please upgrade the driver and daemon to the latest versions.", driver_version, PROTOCOL_VERSION);
			handleException();
		}
	}

	int enable = -1;
	if (readIntDriver("/dev/gator/enable", &enable) != 0 || enable != 0) {
		logg->logError(__FILE__, __LINE__, "Driver already enabled, possibly a session is already in progress.");
		handleException();
	}

	readIntDriver("/dev/gator/cpu_cores", &gSessionData->mCores);
	if (gSessionData->mCores == 0) {
		gSessionData->mCores = 1;
	}

	if (readIntDriver("/dev/gator/buffer_size", &mBufferSize) || mBufferSize <= 0) {
		logg->logError(__FILE__, __LINE__, "Unable to read the driver buffer size");
		handleException();
	}
}

DriverSource::~DriverSource() {
	delete mFifo;

	//                                                                  
	writeDriver("/dev/gator/enable", "0");

	//                                         
	if (mBufferFD) {
		close(mBufferFD);
	}
}

bool DriverSource::prepare() {
	//                                                                                              
	logg->logMessage("Created %d MB collector buffer with a %d-byte ragged end", gSessionData->mTotalBufferSize, mBufferSize);
	mFifo = new Fifo(mBufferSize + 5, gSessionData->mTotalBufferSize*1024*1024, mSenderSem);

	return true;
}

void DriverSource::bootstrapThread() {
	prctl(PR_SET_NAME, (unsigned long)&"gatord-proc", 0, 0, 0);

	DynBuf printb;
	DynBuf b1;
	DynBuf b2;
	const uint64_t currTime = getTime();

	if (!readProcComms(currTime, mBuffer, &printb, &b1, &b2)) {
		logg->logError(__FILE__, __LINE__, "readProcComms failed");
		handleException();
	}

	mBuffer->commit(currTime);
	mBuffer->setDone();
}

void *DriverSource::bootstrapThreadStatic(void *arg) {
	static_cast<DriverSource *>(arg)->bootstrapThread();
	return NULL;
}

void DriverSource::run() {
	//                                              
	char *collectBuffer = mFifo->start();
	int bytesCollected = 0;

	logg->logMessage("********** Profiling started **********");

	//                                
	if (writeReadDriver("/dev/gator/backtrace_depth", &gSessionData->mBacktraceDepth)) {
		logg->logError(__FILE__, __LINE__, "Unable to set the driver backtrace depth");
		handleException();
	}

	//                                                                  
	mBufferFD = open("/dev/gator/buffer", O_RDONLY | O_CLOEXEC);
	if (mBufferFD < 0) {
		logg->logError(__FILE__, __LINE__, "The gator driver did not set up properly. Please view the linux console or dmesg log for more information on the failure.");
		handleException();
	}

	//                                         
	if (writeReadDriver("/dev/gator/tick", &gSessionData->mSampleRate) != 0) {
		logg->logError(__FILE__, __LINE__, "Unable to set the driver tick");
		handleException();
	}

	//                                       
	int response_type = gSessionData->mLocalCapture ? 0 : RESPONSE_APC_DATA;
	if (writeDriver("/dev/gator/response_type", response_type)) {
		logg->logError(__FILE__, __LINE__, "Unable to write the response type");
		handleException();
	}

	//                  
	if (writeReadDriver("/dev/gator/live_rate", &gSessionData->mLiveRate)) {
		logg->logError(__FILE__, __LINE__, "Unable to set the driver live rate");
		handleException();
	}

	logg->logMessage("Start the driver");

	//                                                                                        
	if (writeDriver("/dev/gator/enable", "1") != 0) {
		logg->logError(__FILE__, __LINE__, "The gator driver did not start properly. Please view the linux console or dmesg log for more information on the failure.");
		handleException();
	}

	lseek(mBufferFD, 0, SEEK_SET);

	sem_post(mStartProfile);

	pthread_t bootstrapThreadID;
	if (pthread_create(&bootstrapThreadID, NULL, bootstrapThreadStatic, this) != 0) {
		logg->logError(__FILE__, __LINE__, "Unable to start the gator_bootstrap thread");
		handleException();
	}

	//             
	do {
		//                                                               
		//                                      
		errno = 0;
		bytesCollected = read(mBufferFD, collectBuffer, mBufferSize);

		//                                                                                                
		if (bytesCollected == -1 && errno == EINTR) {
			bytesCollected = read(mBufferFD, collectBuffer, mBufferSize);
		}

		//                               
		logg->logMessage("Driver read of %d bytes", bytesCollected);

		//                                                                  
		if (gSessionData->mOneShot && gSessionData->mSessionIsActive) {
			if (bytesCollected == -1 || mFifo->willFill(bytesCollected)) {
				logg->logMessage("One shot");
				child->endSession();
			}
		}
		collectBuffer = mFifo->write(bytesCollected);
	} while (bytesCollected > 0);

	logg->logMessage("Exit collect data loop");

	pthread_join(bootstrapThreadID, NULL);
}

void DriverSource::interrupt() {
	//                                                                                                        
	if (writeDriver("/dev/gator/enable", "0") != 0) {
		logg->logMessage("Stopping kernel failed");
	}
}

bool DriverSource::isDone() {
	return mLength <= 0 && (mBuffer == NULL || mBuffer->isDone());
}

void DriverSource::write(Sender *sender) {
	char *data = mFifo->read(&mLength);
	if (data != NULL) {
		sender->writeData(data, mLength, RESPONSE_APC_DATA);
		mFifo->release();
		//                                                                         
		gSessionData->mSentSummary = true;
	}
	if (mBuffer != NULL && !mBuffer->isDone()) {
		mBuffer->write(sender);
		if (mBuffer->isDone()) {
			Buffer *buf = mBuffer;
			mBuffer = NULL;
			delete buf;
		}
	}
}

int DriverSource::readIntDriver(const char *fullpath, int *value) {
	char data[40]; //                                       
	const int fd = open(fullpath, O_RDONLY | O_CLOEXEC);
	if (fd < 0) {
		return -1;
	}

	const ssize_t bytes = read(fd, data, sizeof(data) - 1);
	close(fd);
	if (bytes < 0) {
		return -1;
	}
	data[bytes] = '\0';

	char *endptr;
	errno = 0;
	*value = strtol(data, &endptr, 10);
	if (errno != 0 || *endptr != '\n') {
		logg->logMessage("Invalid value in file %s", fullpath);
		return -1;
	}

	return 0;
}

int DriverSource::readInt64Driver(const char *fullpath, int64_t *value) {
	char data[40]; //                                       
	const int fd = open(fullpath, O_RDONLY | O_CLOEXEC);
	if (fd < 0) {
		return -1;
	}

	const ssize_t bytes = read(fd, data, sizeof(data) - 1);
	close(fd);
	if (bytes < 0) {
		return -1;
	}
	data[bytes] = '\0';

	char *endptr;
	errno = 0;
	*value = strtoll(data, &endptr, 10);
	if (errno != 0 || (*endptr != '\n' && *endptr != '\0')) {
		logg->logMessage("Invalid value in file %s", fullpath);
		return -1;
	}

	return 0;
}

int DriverSource::writeDriver(const char *fullpath, const char *data) {
	int fd = open(fullpath, O_WRONLY | O_CLOEXEC);
	if (fd < 0) {
		return -1;
	}
	if (::write(fd, data, strlen(data)) < 0) {
		close(fd);
		logg->logMessage("Opened but could not write to %s", fullpath);
		return -1;
	}
	close(fd);
	return 0;
}

int DriverSource::writeDriver(const char *path, int value) {
	char data[40]; //                                       
	snprintf(data, sizeof(data), "%d", value);
	return writeDriver(path, data);
}

int DriverSource::writeDriver(const char *path, int64_t value) {
	char data[40]; //                                       
	snprintf(data, sizeof(data), "%" PRIi64, value);
	return writeDriver(path, data);
}

int DriverSource::writeReadDriver(const char *path, int *value) {
	if (writeDriver(path, *value) || readIntDriver(path, value)) {
		return -1;
	}
	return 0;
}

int DriverSource::writeReadDriver(const char *path, int64_t *value) {
	if (writeDriver(path, *value) || readInt64Driver(path, value)) {
		return -1;
	}
	return 0;
}
