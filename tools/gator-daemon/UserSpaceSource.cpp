/**
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "UserSpaceSource.h"

#include <sys/prctl.h>
#include <unistd.h>

#include "Child.h"
#include "DriverSource.h"
#include "Logging.h"
#include "SessionData.h"

extern Child *child;

UserSpaceSource::UserSpaceSource(sem_t *senderSem) : mBuffer(0, FRAME_BLOCK_COUNTER, gSessionData->mTotalBufferSize*1024*1024, senderSem) {
}

UserSpaceSource::~UserSpaceSource() {
}

bool UserSpaceSource::prepare() {
	return true;
}

void UserSpaceSource::run() {
	prctl(PR_SET_NAME, (unsigned long)&"gatord-counters", 0, 0, 0);

	for (int i = 0; i < ARRAY_LENGTH(gSessionData->usDrivers); ++i) {
		gSessionData->usDrivers[i]->start();
	}

	int64_t monotonic_started = 0;
	while (monotonic_started <= 0) {
		usleep(10);

		if (gSessionData->perf.isSetup()) {
			monotonic_started = gSessionData->mMonotonicStarted;
		} else {
			if (DriverSource::readInt64Driver("/dev/gator/started", &monotonic_started) == -1) {
				logg->logError(__FILE__, __LINE__, "Error reading gator driver start time");
				handleException();
			}
			gSessionData->mMonotonicStarted = monotonic_started;
		}
	}

	uint64_t next_time = 0;
	while (gSessionData->mSessionIsActive) {
		const uint64_t curr_time = getTime() - monotonic_started;
		//                                                             
		next_time += NS_PER_S/10;//                          
		if (next_time < curr_time) {
			logg->logMessage("Too slow, curr_time: %lli next_time: %lli", curr_time, next_time);
			next_time = curr_time;
		}

		if (mBuffer.eventHeader(curr_time)) {
			for (int i = 0; i < ARRAY_LENGTH(gSessionData->usDrivers); ++i) {
				gSessionData->usDrivers[i]->read(&mBuffer);
			}
			//                                                                                                       
			mBuffer.check(curr_time);
		}

		if (mBuffer.bytesAvailable() <= 0) {
			logg->logMessage("One shot (counters)");
			child->endSession();
		}

		usleep((next_time - curr_time)/NS_PER_US);
	}

	mBuffer.setDone();
}

void UserSpaceSource::interrupt() {
	//           
}

bool UserSpaceSource::isDone() {
	return mBuffer.isDone();
}

void UserSpaceSource::write(Sender *sender) {
	if (!mBuffer.isDone()) {
		mBuffer.write(sender);
	}
}
