/**
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "CapturedXML.h"

#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "SessionData.h"
#include "Logging.h"
#include "OlyUtility.h"

CapturedXML::CapturedXML() {
}

CapturedXML::~CapturedXML() {
}

mxml_node_t* CapturedXML::getTree(bool includeTime) {
	mxml_node_t *xml;
	mxml_node_t *captured;
	mxml_node_t *target;
	int x;

	xml = mxmlNewXML("1.0");

	captured = mxmlNewElement(xml, "captured");
	mxmlElementSetAttr(captured, "version", "1");
	if (gSessionData->perf.isSetup()) {
		mxmlElementSetAttr(captured, "type", "Perf");
		mxmlElementSetAttr(captured, "perf_beta", "yes");
	}
	mxmlElementSetAttrf(captured, "protocol", "%d", PROTOCOL_VERSION);
	if (includeTime) { //                                                      
		if (time(NULL) > 1267000000) { //                                               
			mxmlElementSetAttrf(captured, "created", "%lu", time(NULL)); //                          
		}
	}

	target = mxmlNewElement(captured, "target");
	mxmlElementSetAttr(target, "name", gSessionData->mCoreName);
	mxmlElementSetAttrf(target, "sample_rate", "%d", gSessionData->mSampleRate);
	mxmlElementSetAttrf(target, "cores", "%d", gSessionData->mCores);
	mxmlElementSetAttrf(target, "cpuid", "0x%x", gSessionData->mMaxCpuId);

	if (!gSessionData->mOneShot && (gSessionData->mSampleRate > 0)) {
		mxmlElementSetAttr(target, "supports_live", "yes");
	}

	if (gSessionData->mLocalCapture) {
		mxmlElementSetAttr(target, "local_capture", "yes");
	}

	mxml_node_t *counters = NULL;
	for (x = 0; x < MAX_PERFORMANCE_COUNTERS; x++) {
		const Counter & counter = gSessionData->mCounters[x];
		if (counter.isEnabled()) {
			if (counters == NULL) {
				counters = mxmlNewElement(captured, "counters");
			}
			mxml_node_t *const node = mxmlNewElement(counters, "counter");
			mxmlElementSetAttrf(node, "key", "0x%x", counter.getKey());
			mxmlElementSetAttr(node, "type", counter.getType());
			if (counter.getEvent() != -1) {
				mxmlElementSetAttrf(node, "event", "0x%x", counter.getEvent());
			}
			if (counter.getCount() > 0) {
				mxmlElementSetAttrf(node, "count", "%d", counter.getCount());
			}
			if (counter.getCores() > 0) {
				mxmlElementSetAttrf(node, "cores", "%d", counter.getCores());
			}
		}
	}

	return xml;
}

char* CapturedXML::getXML(bool includeTime) {
	char* xml_string;
	mxml_node_t *xml = getTree(includeTime);
	xml_string = mxmlSaveAllocString(xml, mxmlWhitespaceCB);
	mxmlDelete(xml);
	return xml_string;
}

void CapturedXML::write(char* path) {
	char file[PATH_MAX];

	//              
	snprintf(file, PATH_MAX, "%s/captured.xml", path);

	char* xml = getXML(true);
	if (util->writeToDisk(file, xml) < 0) {
		logg->logError(__FILE__, __LINE__, "Error writing %s\nPlease verify the path.", file);
		handleException();
	}

	free(xml);
}

//                                                        
const char * mxmlWhitespaceCB(mxml_node_t *node, int loc) {
	const char *name;

	name = mxmlGetElement(node);

	if (loc == MXML_WS_BEFORE_OPEN) {
		//                   
		if (!strcmp(name, "target") || !strcmp(name, "counters"))
			return "\n  ";

		//                   
		if (!strcmp(name, "counter"))
			return "\n    ";

		//                                                          
		if (!strncmp(name, "?xml", 4))
			return NULL;

		//                         
		return "\n";
	}

	if (loc == MXML_WS_BEFORE_CLOSE) {
		//               
		if (!strcmp(name, "captured"))
			return "\n";

		//                   
		if (!strcmp(name, "counters"))
			return "\n  ";

		//                             
		return NULL;
	}

	return NULL;
}
