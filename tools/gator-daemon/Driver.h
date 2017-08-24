/**
 * Copyright (C) ARM Limited 2013-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>

#include "mxml/mxml.h"

class Buffer;
class Counter;

class DriverCounter {
public:
	DriverCounter(DriverCounter *const next, const char *const name);
	virtual ~DriverCounter();

	DriverCounter *getNext() const { return mNext; }
	const char *getName() const { return mName; }
	int getKey() const { return mKey; }
	bool isEnabled() const { return mEnabled; }
	void setEnabled(const bool enabled) { mEnabled = enabled; }
	virtual int64_t read() { return -1; }

private:
	DriverCounter *const mNext;
	const char *const mName;
	const int mKey;
	bool mEnabled;

	//                            
	DriverCounter(const DriverCounter &);
	DriverCounter &operator=(const DriverCounter &);
};

class Driver {
public:
	static Driver *getHead() { return head; }

	virtual ~Driver() {}

	//                                                   
	virtual bool claimCounter(const Counter &counter) const = 0;
	//                                 
	virtual void resetCounters() = 0;
	//                                             
	virtual void setupCounter(Counter &counter) = 0;

	//                                                            
	virtual void readEvents(mxml_node_t *const) {}
	//                         
	virtual int writeCounters(mxml_node_t *const root) const = 0;
	//                                                     
	virtual void writeEvents(mxml_node_t *const) const {}

	Driver *getNext() const { return next; }

protected:
	Driver();

private:
	static Driver *head;
	Driver *next;

	//                            
	Driver(const Driver &);
	Driver &operator=(const Driver &);
};

class SimpleDriver : public Driver {
public:
	virtual ~SimpleDriver();

	bool claimCounter(const Counter &counter) const;
	bool countersEnabled() const;
	void resetCounters();
	void setupCounter(Counter &counter);
	int writeCounters(mxml_node_t *root) const;

protected:
	SimpleDriver() : mCounters(NULL) {}

	DriverCounter *getCounters() const { return mCounters; }
	void setCounters(DriverCounter *const counter) { mCounters = counter; }

	DriverCounter *findCounter(const Counter &counter) const;

private:
	DriverCounter *mCounters;

	//                            
	SimpleDriver(const SimpleDriver &);
	SimpleDriver &operator=(const SimpleDriver &);
};

class PolledDriver : public SimpleDriver {
public:
	virtual ~PolledDriver();

	virtual void start() {}
	virtual void read(Buffer *const buffer);

protected:
	PolledDriver() {}

private:
	//                            
	PolledDriver(const PolledDriver &);
	PolledDriver &operator=(const PolledDriver &);
};

#endif //         
