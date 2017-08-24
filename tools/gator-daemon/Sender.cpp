/**
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "Sender.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Buffer.h"
#include "Logging.h"
#include "OlySocket.h"
#include "SessionData.h"

Sender::Sender(OlySocket* socket) {
	mDataFile = NULL;
	mDataSocket = NULL;

	//                             
	if (socket) {
		char streamline[64] = {0};
		mDataSocket = socket;

		//                                          
		//                                                                                                          
		while (strcmp("STREAMLINE", streamline) != 0) {
			if (mDataSocket->receiveString(streamline, sizeof(streamline)) == -1) {
				logg->logError(__FILE__, __LINE__, "Socket disconnected");
				handleException();
			}
		}

		//                                                                                 
		char magic[32];
		snprintf(magic, 32, "GATOR %i\n", PROTOCOL_VERSION);
		mDataSocket->send(magic, strlen(magic));

		gSessionData->mWaitingOnCommand = true;
		logg->logMessage("Completed magic sequence");
	}

	pthread_mutex_init(&mSendMutex, NULL);
}

Sender::~Sender() {
	//                                                   
	if (mDataSocket != NULL) {
		mDataSocket->closeSocket();
		mDataSocket = NULL;
	}
	if (mDataFile != NULL) {
		fclose(mDataFile);
	}
}

void Sender::createDataFile(char* apcDir) {
	if (apcDir == NULL) {
		return;
	}

	mDataFileName = (char*)malloc(strlen(apcDir) + 12);
	sprintf(mDataFileName, "%s/0000000000", apcDir);
	mDataFile = fopen_cloexec(mDataFileName, "wb");
	if (!mDataFile) {
		logg->logError(__FILE__, __LINE__, "Failed to open binary file: %s", mDataFileName);
		handleException();
	}
}

void Sender::writeData(const char* data, int length, int type) {
	if (length < 0 || (data == NULL && length > 0)) {
		return;
	}

	//                                  
	pthread_mutex_lock(&mSendMutex);

	//                                     
	if (mDataSocket) {
		//            
		const int alarmDuration = 8;
		alarm(alarmDuration);

		//                                                           
		logg->logMessage("Sending data with length %d", length);
		if (type != RESPONSE_APC_DATA) {
			//                                                            
			unsigned char header[5];
			header[0] = type;
			Buffer::writeLEInt(header + 1, length);
			mDataSocket->send((char*)&header, sizeof(header));
		}

		//                                               
		const int chunkSize = 100*1000 * alarmDuration / 8;
		int pos = 0;
		while (true) {
			mDataSocket->send((const char*)data + pos, min(length - pos, chunkSize));
			pos += chunkSize;
			if (pos >= length) {
				break;
			}

			//                
			alarm(alarmDuration);
			logg->logMessage("Resetting the alarm");
		}

		//           
		alarm(0);
	}

	//                                                  
	if (mDataFile && type == RESPONSE_APC_DATA) {
		logg->logMessage("Writing data with length %d", length);
		//                           
		if (fwrite(data, 1, length, mDataFile) != (unsigned int)length) {
			logg->logError(__FILE__, __LINE__, "Failed writing binary file %s", mDataFileName);
			handleException();
		}
	}

	pthread_mutex_unlock(&mSendMutex);
}
