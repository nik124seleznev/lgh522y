/**
 * Copyright (C) ARM Limited 2013-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "KMod.h"

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include "ConfigurationXML.h"
#include "Counter.h"
#include "DriverSource.h"
#include "Logging.h"
#include "SessionData.h"

//                                            
bool KMod::claimCounter(const Counter &counter) const {
	char text[128];
	snprintf(text, sizeof(text), "/dev/gator/events/%s", counter.getType());
	return access(text, F_OK) == 0;
}

void KMod::resetCounters() {
	char base[128];
	char text[128];

	//                                                                     
	struct dirent *ent;
	DIR* dir = opendir("/dev/gator/events");
	if (dir) {
		while ((ent = readdir(dir)) != NULL) {
			//                                               
			if (ent->d_name[0] == '.')
				continue;
			snprintf(base, sizeof(base), "/dev/gator/events/%s", ent->d_name);
			snprintf(text, sizeof(text), "%s/enabled", base);
			DriverSource::writeDriver(text, 0);
			snprintf(text, sizeof(text), "%s/count", base);
			DriverSource::writeDriver(text, 0);
		}
		closedir(dir);
	}
}

static const char ARM_MALI_MIDGARD[] = "ARM_Mali-Midgard_";
static const char ARM_MALI_T[] = "ARM_Mali-T";

void KMod::setupCounter(Counter &counter) {
	char base[128];
	char text[128];
	snprintf(base, sizeof(base), "/dev/gator/events/%s", counter.getType());

	if ((strncmp(counter.getType(), ARM_MALI_MIDGARD, sizeof(ARM_MALI_MIDGARD) - 1) == 0 ||
	     strncmp(counter.getType(), ARM_MALI_T, sizeof(ARM_MALI_T) - 1) == 0)) {
		mIsMaliCapture = true;
	}

	snprintf(text, sizeof(text), "%s/enabled", base);
	int enabled = true;
	if (DriverSource::writeReadDriver(text, &enabled) || !enabled) {
		counter.setEnabled(false);
		return;
	}

	int value = 0;
	snprintf(text, sizeof(text), "%s/key", base);
	DriverSource::readIntDriver(text, &value);
	counter.setKey(value);

	snprintf(text, sizeof(text), "%s/cores", base);
	if (DriverSource::readIntDriver(text, &value) == 0) {
		counter.setCores(value);
	}

	snprintf(text, sizeof(text), "%s/event", base);
	DriverSource::writeDriver(text, counter.getEvent());
	snprintf(text, sizeof(text), "%s/count", base);
	if (access(text, F_OK) == 0) {
		int count = counter.getCount();
		if (DriverSource::writeReadDriver(text, &count) && counter.getCount() > 0) {
			logg->logError(__FILE__, __LINE__, "Cannot enable EBS for %s:%i with a count of %d\n", counter.getType(), counter.getEvent(), counter.getCount());
			handleException();
		}
		counter.setCount(count);
	} else if (counter.getCount() > 0) {
		ConfigurationXML::remove();
		logg->logError(__FILE__, __LINE__, "Event Based Sampling is only supported with kernel versions 3.0.0 and higher with CONFIG_PERF_EVENTS=y, and CONFIG_HW_PERF_EVENTS=y. The invalid configuration.xml has been removed.\n");
		handleException();
	}
}

int KMod::writeCounters(mxml_node_t *root) const {
	struct dirent *ent;
	mxml_node_t *counter;

	//                                                           
	DIR* dir = opendir("/dev/gator/events");
	if (dir == NULL) {
		return 0;
	}

	int count = 0;
	while ((ent = readdir(dir)) != NULL) {
		//                                               
		if (ent->d_name[0] == '.')
			continue;
		counter = mxmlNewElement(root, "counter");
		mxmlElementSetAttr(counter, "name", ent->d_name);
		++count;
	}
	closedir(dir);

	return count;
}