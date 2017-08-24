/**
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "Child.h"

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/prctl.h>

#include "CapturedXML.h"
#include "Command.h"
#include "ConfigurationXML.h"
#include "Driver.h"
#include "DriverSource.h"
#include "ExternalSource.h"
#include "FtraceSource.h"
#include "LocalCapture.h"
#include "Logging.h"
#include "OlySocket.h"
#include "OlyUtility.h"
#include "PerfSource.h"
#include "Sender.h"
#include "SessionData.h"
#include "StreamlineSetup.h"
#include "UserSpaceSource.h"

static sem_t haltPipeline, senderThreadStarted, startProfile, senderSem; //                                    
static Source *primarySource = NULL;
static Source *externalSource = NULL;
static Source *userSpaceSource = NULL;
static Source *ftraceSource = NULL;
static Sender* sender = NULL;        //                                        
Child* child = NULL;                 //                                 

extern void cleanUp();
void handleException() {
	if (child && child->numExceptions++ > 0) {
		//                                                                                                       
		logg->logMessage("Received multiple exceptions, terminating the child");
		exit(1);
	}
	fprintf(stderr, "%s", logg->getLastError());

	if (child && child->socket) {
		if (sender) {
			//                                                             
			sender->writeData(logg->getLastError(), strlen(logg->getLastError()), RESPONSE_ERROR);

			//                                                                                                     
			if (gSessionData->mWaitingOnCommand) {
				char discard;
				child->socket->receiveNBytes(&discard, 1);
			}

			//                           
			child->socket->shutdownConnection();

			//                                                                            
			delete sender;
		}
	}

	if (gSessionData->mLocalCapture)
		cleanUp();

	exit(1);
}

//                                        
static void child_handler(int signum) {
	static bool beenHere = false;
	if (beenHere == true) {
		logg->logMessage("Gator is being forced to shut down.");
		exit(1);
	}
	beenHere = true;
	logg->logMessage("Gator is shutting down.");
	if (signum == SIGALRM || !primarySource) {
		exit(1);
	} else {
		child->endSession();
		alarm(5); //                                                                 
	}
}

static void *durationThread(void *) {
	prctl(PR_SET_NAME, (unsigned long)&"gatord-duration", 0, 0, 0);
	sem_wait(&startProfile);
	if (gSessionData->mSessionIsActive) {
		//                                
		//                                     
		sleep(gSessionData->mDuration + 1);
		if (gSessionData->mSessionIsActive) {
			logg->logMessage("Duration expired.");
			child->endSession();
		}
	}
	logg->logMessage("Exit duration thread");
	return 0;
}

static void *stopThread(void *) {
	OlySocket* socket = child->socket;

	prctl(PR_SET_NAME, (unsigned long)&"gatord-stopper", 0, 0, 0);
	while (gSessionData->mSessionIsActive) {
		//                                                                                                                    
		unsigned char header[5];
		const int result = socket->receiveNBytes((char*)&header, sizeof(header));
		const char type = header[0];
		const int length = (header[1] << 0) | (header[2] << 8) | (header[3] << 16) | (header[4] << 24);
		if (result == -1) {
			child->endSession();
		} else if (result > 0) {
			if ((type != COMMAND_APC_STOP) && (type != COMMAND_PING)) {
				logg->logMessage("INVESTIGATE: Received unknown command type %d", type);
			} else {
				//                        
				if (length == 0) {
					if (type == COMMAND_APC_STOP) {
						logg->logMessage("Stop command received.");
						child->endSession();
					} else {
						//                                                                             
						logg->logMessage("Ping command received.");
						sender->writeData(NULL, 0, RESPONSE_ACK);
					}
				} else {
					logg->logMessage("INVESTIGATE: Received stop command but with length = %d", length);
				}
			}
		}
	}

	logg->logMessage("Exit stop thread");
	return 0;
}

static void *senderThread(void *) {
	char end_sequence[] = {RESPONSE_APC_DATA, 0, 0, 0, 0};

	sem_post(&senderThreadStarted);
	prctl(PR_SET_NAME, (unsigned long)&"gatord-sender", 0, 0, 0);
	sem_wait(&haltPipeline);

	while (!primarySource->isDone() ||
	       !externalSource->isDone() ||
	       (userSpaceSource != NULL && !userSpaceSource->isDone()) ||
	       (ftraceSource != NULL && !ftraceSource->isDone())) {
		sem_wait(&senderSem);

		primarySource->write(sender);
		externalSource->write(sender);
		if (userSpaceSource != NULL) {
			userSpaceSource->write(sender);
		}
		if (ftraceSource != NULL) {
			ftraceSource->write(sender);
		}
	}

	//                              
	if (!gSessionData->mLocalCapture) {
		sender->writeData(end_sequence, sizeof(end_sequence), RESPONSE_APC_DATA);
	}

	logg->logMessage("Exit sender thread");
	return 0;
}

Child::Child() {
	initialization();
	gSessionData->mLocalCapture = true;
}

Child::Child(OlySocket* sock, int conn) {
	initialization();
	socket = sock;
	mNumConnections = conn;
}

Child::~Child() {
}

void Child::initialization() {
	//                                      
	gSessionData->mSessionIsActive = true;
	signal(SIGINT, child_handler);
	signal(SIGTERM, child_handler);
	signal(SIGABRT, child_handler);
	signal(SIGALRM, child_handler);
	socket = NULL;
	numExceptions = 0;
	mNumConnections = 0;

	//                      
	sem_init(&senderThreadStarted, 0, 0);
	sem_init(&startProfile, 0, 0);
	sem_init(&senderSem, 0, 0);
}

void Child::endSession() {
	gSessionData->mSessionIsActive = false;
	primarySource->interrupt();
	externalSource->interrupt();
	if (userSpaceSource != NULL) {
		userSpaceSource->interrupt();
	}
	if (ftraceSource != NULL) {
		ftraceSource->interrupt();
	}
	sem_post(&haltPipeline);
}

void Child::run() {
	LocalCapture* localCapture = NULL;
	pthread_t durationThreadID, stopThreadID, senderThreadID;

	prctl(PR_SET_NAME, (unsigned long)&"gatord-child", 0, 0, 0);

	//                                                                                                       
	mxmlSetWrapMargin(0);

	//                                                                                    
	sender = new Sender(socket);

	if (mNumConnections > 1) {
		logg->logError(__FILE__, __LINE__, "Session already in progress");
		handleException();
	}

	//                                             
	{ ConfigurationXML configuration; }

	//                                                                                    
	if (!gSessionData->perf.isSetup()) {
		primarySource = new DriverSource(&senderSem, &startProfile);
	} else {
		primarySource = new PerfSource(&senderSem, &startProfile);
	}

	//                       
	for (Driver *driver = Driver::getHead(); driver != NULL; driver = driver->getNext()) {
		driver->resetCounters();
	}

	//                                                             
	for (int i = 0; i < MAX_PERFORMANCE_COUNTERS; i++) {
		Counter & counter = gSessionData->mCounters[i];
		if (counter.isEnabled()) {
			counter.getDriver()->setupCounter(counter);
		}
	}

	//                               
	if (socket) {
		//                               
		StreamlineSetup ss(socket);
	} else {
		char* xmlString;
		xmlString = util->readFromDisk(gSessionData->mSessionXMLPath);
		if (xmlString == 0) {
			logg->logError(__FILE__, __LINE__, "Unable to read session xml file: %s", gSessionData->mSessionXMLPath);
			handleException();
		}
		gSessionData->parseSessionXML(xmlString);
		localCapture = new LocalCapture();
		localCapture->createAPCDirectory(gSessionData->mTargetPath);
		localCapture->copyImages(gSessionData->mImages);
		localCapture->write(xmlString);
		sender->createDataFile(gSessionData->mAPCDir);
		free(xmlString);
	}

	if (gSessionData->kmod.isMaliCapture() && (gSessionData->mSampleRate == 0)) {
		logg->logError(__FILE__, __LINE__, "Mali counters are not supported with Sample Rate: None.");
		handleException();
	}

	//                                    
	if (!primarySource->prepare()) {
		if (gSessionData->perf.isSetup()) {
			logg->logError(__FILE__, __LINE__, "Unable to prepare gator driver for capture");
		} else {
			logg->logError(__FILE__, __LINE__, "Unable to communicate with the perf API, please ensure that CONFIG_TRACING and CONFIG_CONTEXT_SWITCH_TRACER are enabled. Please refer to README_Streamline.txt for more information.");
		}
		handleException();
	}

	//                                                                     
	sem_init(&haltPipeline, 0, gSessionData->mOneShot ? 0 : 2);

	//                                                                                         
	externalSource = new ExternalSource(&senderSem);
	if (!externalSource->prepare()) {
		logg->logError(__FILE__, __LINE__, "Unable to prepare external source for capture");
		handleException();
	}
	externalSource->start();

	//                                              
	bool thread_creation_success = true;
	if (gSessionData->mDuration > 0 && pthread_create(&durationThreadID, NULL, durationThread, NULL)) {
		thread_creation_success = false;
	} else if (socket && pthread_create(&stopThreadID, NULL, stopThread, NULL)) {
		thread_creation_success = false;
	} else if (pthread_create(&senderThreadID, NULL, senderThread, NULL)) {
		thread_creation_success = false;
	}

	bool startUSSource = false;
	for (int i = 0; i < ARRAY_LENGTH(gSessionData->usDrivers); ++i) {
		if (gSessionData->usDrivers[i]->countersEnabled()) {
			startUSSource = true;
		}
	}
	if (startUSSource) {
		userSpaceSource = new UserSpaceSource(&senderSem);
		if (!userSpaceSource->prepare()) {
			logg->logError(__FILE__, __LINE__, "Unable to prepare userspace source for capture");
			handleException();
		}
		userSpaceSource->start();
	}

	if (gSessionData->ftraceDriver.countersEnabled()) {
		ftraceSource = new FtraceSource(&senderSem);
		if (!ftraceSource->prepare()) {
			logg->logError(__FILE__, __LINE__, "Unable to prepare userspace source for capture");
			handleException();
		}
		ftraceSource->start();
	}

	if (gSessionData->mAllowCommands && (gSessionData->mCaptureCommand != NULL)) {
		pthread_t thread;
		if (pthread_create(&thread, NULL, commandThread, NULL)) {
			thread_creation_success = false;
		}
	}

	if (!thread_creation_success) {
		logg->logError(__FILE__, __LINE__, "Failed to create gator threads");
		handleException();
	}

	//                              
	sem_wait(&senderThreadStarted);

	//                
	primarySource->run();

	if (ftraceSource != NULL) {
		ftraceSource->join();
	}
	if (userSpaceSource != NULL) {
		userSpaceSource->join();
	}
	externalSource->join();

	//                                   
	pthread_join(senderThreadID, NULL);

	//                                                                                                          
	if (socket) {
		logg->logMessage("Waiting on stop thread");
		socket->shutdownConnection();
		pthread_join(stopThreadID, NULL);
	}

	//                            
	if (gSessionData->mLocalCapture) {
		CapturedXML capturedXML;
		capturedXML.write(gSessionData->mAPCDir);
	}

	logg->logMessage("Profiling ended.");

	delete ftraceSource;
	delete userSpaceSource;
	delete externalSource;
	delete primarySource;
	delete sender;
	delete localCapture;
}
