/**
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "Fifo.h"

#include <stdlib.h>

#include "Logging.h"

//                                              
//                                                                              
//                                                  
Fifo::Fifo(int singleBufferSize, int bufferSize, sem_t* readerSem) {
  mWrite = mRead = mReadCommit = mRaggedEnd = 0;
  mWrapThreshold = bufferSize;
  mSingleBufferSize = singleBufferSize;
  mReaderSem = readerSem;
  mBuffer = (char*)malloc(bufferSize + singleBufferSize);
  mEnd = false;

  if (mBuffer == NULL) {
    logg->logError(__FILE__, __LINE__, "failed to allocate %d bytes", bufferSize + singleBufferSize);
    handleException();
  }

  if (sem_init(&mWaitForSpaceSem, 0, 0)) {
    logg->logError(__FILE__, __LINE__, "sem_init() failed");
    handleException();
  }
}

Fifo::~Fifo() {
  free(mBuffer);
  sem_destroy(&mWaitForSpaceSem);
}

int Fifo::numBytesFilled() const {
  return mWrite - mRead + mRaggedEnd;
}

char* Fifo::start() const {
  return mBuffer;
}

bool Fifo::isEmpty() const {
  return mRead == mWrite && mRaggedEnd == 0;
}

bool Fifo::isFull() const {
  return willFill(0);
}

//                                                                                           
//                                                                                                                               
bool Fifo::willFill(int additional) const {
  if (mWrite > mRead) {
    if (numBytesFilled() + additional < mWrapThreshold) {
      return false;
    }
  } else {
    if (numBytesFilled() + additional < mWrapThreshold - mSingleBufferSize) {
      return false;
    }
  }
  return true;
}

//                                                                               
char* Fifo::write(int length) {
  if (length <= 0) {
    length = 0;
    mEnd = true;
  }

  //                         
  mWrite += length;

  //                       
  if (mWrite >= mWrapThreshold) {
    mRaggedEnd = mWrite;
    mWrite = 0;
  }

  //                                       
  sem_post(mReaderSem);

  //               
  while (isFull()) {
    sem_wait(&mWaitForSpaceSem);
  }

  return &mBuffer[mWrite];
}

void Fifo::release() {
  //                                                           
  mRead = mReadCommit;

  //                       
  if (mRead >= mWrapThreshold) {
    mRaggedEnd = mRead = mReadCommit = 0;
  }

  //                                                           
  sem_post(&mWaitForSpaceSem);
}

//                                                       
char* Fifo::read(int *const length) {
  //              
  if (isEmpty() && !mEnd) {
    return NULL;
  }

  //                  
  do {
    mReadCommit = mRaggedEnd ? mRaggedEnd : mWrite;
    *length = mReadCommit - mRead;
  } while (*length < 0); //                                              

  return &mBuffer[mRead];
}
