/**
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "ExternalSource.h"

#include <fcntl.h>
#include <sys/prctl.h>
#include <unistd.h>

#include "Logging.h"
#include "OlySocket.h"
#include "SessionData.h"

static const char MALI_VIDEO[] = "\0mali-video";
static const char MALI_VIDEO_STARTUP[] = "\0mali-video-startup";
static const char MALI_VIDEO_V1[] = "MALI_VIDEO 1\n";
static const char MALI_GRAPHICS[] = "\0mali_thirdparty_server";
static const char MALI_GRAPHICS_STARTUP[] = "\0mali_thirdparty_client";
static const char MALI_GRAPHICS_V1[] = "MALI_GRAPHICS 1\n";

static bool setNonblock(const int fd) {
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0) {
		logg->logMessage("fcntl getfl failed");
		return false;
	}

	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
		logg->logMessage("fcntl setfl failed");
		return false;
	}

	return true;
}

ExternalSource::ExternalSource(sem_t *senderSem) : mBuffer(0, FRAME_EXTERNAL, 128*1024, senderSem), mMonitor(), mMveStartupUds(MALI_VIDEO_STARTUP, sizeof(MALI_VIDEO_STARTUP)), mMaliStartupUds(MALI_GRAPHICS_STARTUP, sizeof(MALI_GRAPHICS_STARTUP)), mAnnotate(8083), mInterruptFd(-1), mMaliUds(-1), mMveUds(-1) {
	sem_init(&mBufferSem, 0, 0);
}

ExternalSource::~ExternalSource() {
}

void ExternalSource::waitFor(const int bytes) {
	while (mBuffer.bytesAvailable() <= bytes) {
		sem_wait(&mBufferSem);
	}
}

void ExternalSource::configureConnection(const int fd, const char *const handshake, size_t size) {
	if (!setNonblock(fd)) {
		logg->logError(__FILE__, __LINE__, "Unable to set nonblock on fh");
		handleException();
	}

	if (!mMonitor.add(fd)) {
		logg->logError(__FILE__, __LINE__, "Unable to add fh to monitor");
		handleException();
	}

	//                                           
	waitFor(Buffer::MAXSIZE_PACK32 + size - 1);
	mBuffer.packInt(fd);
	mBuffer.writeBytes(handshake, size - 1);
	mBuffer.commit(1);
}

bool ExternalSource::connectMali() {
	mMaliUds = OlySocket::connect(MALI_GRAPHICS, sizeof(MALI_GRAPHICS));
	if (mMaliUds < 0) {
		return false;
	}

	configureConnection(mMaliUds, MALI_GRAPHICS_V1, sizeof(MALI_GRAPHICS_V1));

	return true;
}

bool ExternalSource::connectMve() {
	if (!gSessionData->maliVideo.countersEnabled()) {
		return true;
	}

	mMveUds = OlySocket::connect(MALI_VIDEO, sizeof(MALI_VIDEO));
	if (mMveUds < 0) {
		return false;
	}

	if (!gSessionData->maliVideo.start(mMveUds)) {
		return false;
	}

	configureConnection(mMveUds, MALI_VIDEO_V1, sizeof(MALI_VIDEO_V1));

	return true;
}

bool ExternalSource::prepare() {
	if (!mMonitor.init() ||
			!setNonblock(mMveStartupUds.getFd()) || !mMonitor.add(mMveStartupUds.getFd()) ||
			!setNonblock(mMaliStartupUds.getFd()) || !mMonitor.add(mMaliStartupUds.getFd()) ||
			!setNonblock(mAnnotate.getFd()) || !mMonitor.add(mAnnotate.getFd()) ||
			false) {
		return false;
	}

	connectMali();
	connectMve();

	return true;
}

void ExternalSource::run() {
	int pipefd[2];

	prctl(PR_SET_NAME, (unsigned long)&"gatord-external", 0, 0, 0);

	if (pipe_cloexec(pipefd) != 0) {
		logg->logError(__FILE__, __LINE__, "pipe failed");
		handleException();
	}
	mInterruptFd = pipefd[1];

	if (!mMonitor.add(pipefd[0])) {
		logg->logError(__FILE__, __LINE__, "Monitor::add failed");
		handleException();
	}

	//                                                      
	gSessionData->annotateListener.signal();

	while (gSessionData->mSessionIsActive) {
		struct epoll_event events[16];
		//                            
		while (sem_trywait(&mBufferSem) == 0);
		int ready = mMonitor.wait(events, ARRAY_LENGTH(events), -1);
		if (ready < 0) {
			logg->logError(__FILE__, __LINE__, "Monitor::wait failed");
			handleException();
		}

		const uint64_t currTime = getTime();

		for (int i = 0; i < ready; ++i) {
			const int fd = events[i].data.fd;
			if (fd == mMveStartupUds.getFd()) {
				//                                  
				int client = mMveStartupUds.acceptConnection();
				//                                                                         
				close(client);
				if (!connectMve()) {
					logg->logError(__FILE__, __LINE__, "Unable to configure incoming Mali video connection");
					handleException();
				}
			} else if (fd == mMaliStartupUds.getFd()) {
				//                              
				int client = mMaliStartupUds.acceptConnection();
				//                                                                             
				close(client);
				if (!connectMali()) {
					logg->logError(__FILE__, __LINE__, "Unable to configure incoming Mali graphics connection");
					handleException();
				}
			} else if (fd == mAnnotate.getFd()) {
				int client = mAnnotate.acceptConnection();
				if (!setNonblock(client) || !mMonitor.add(client)) {
					logg->logError(__FILE__, __LINE__, "Unable to set socket options on incoming annotation connection");
					handleException();
				}
			} else if (fd == pipefd[0]) {
				//                                                                      
			} else {
				/*                                                         
                                                          
                                                                 
                                 
     */
				while (gSessionData->mSessionIsActive) {
					//                                                                     
					waitFor(7*Buffer::MAXSIZE_PACK32 + 2*sizeof(uint32_t));
					mBuffer.packInt(fd);
					const int contiguous = mBuffer.contiguousSpaceAvailable();
					const int bytes = read(fd, mBuffer.getWritePos(), contiguous);
					if (bytes < 0) {
						if (errno == EAGAIN) {
							//                     
							mBuffer.commit(currTime);
							break;
						}
						//                                        
						mBuffer.commit(currTime);
						mBuffer.packInt(-1);
						mBuffer.packInt(fd);
						mBuffer.commit(currTime);
						close(fd);
						break;
					} else if (bytes == 0) {
						//                         
						mBuffer.commit(currTime);
						mBuffer.packInt(-1);
						mBuffer.packInt(fd);
						mBuffer.commit(currTime);
						close(fd);
						break;
					}

					mBuffer.advanceWrite(bytes);
					mBuffer.commit(currTime);

					//                                              
					if (bytes < contiguous) {
						break;
					}
				}
			}
		}
	}

	mBuffer.setDone();

	if (mMveUds >= 0) {
		gSessionData->maliVideo.stop(mMveUds);
	}

	mInterruptFd = -1;
	close(pipefd[0]);
	close(pipefd[1]);
}

void ExternalSource::interrupt() {
	if (mInterruptFd >= 0) {
		int8_t c = 0;
		//                                                                                     
		if (::write(mInterruptFd, &c, sizeof(c)) != sizeof(c)) {
			logg->logError(__FILE__, __LINE__, "write failed");
			handleException();
		}
	}
}

bool ExternalSource::isDone() {
	return mBuffer.isDone();
}

void ExternalSource::write(Sender *sender) {
	//                                                                                               
	if (!gSessionData->mSentSummary) {
		return;
	}
	if (!mBuffer.isDone()) {
		mBuffer.write(sender);
		sem_post(&mBufferSem);
	}
}
