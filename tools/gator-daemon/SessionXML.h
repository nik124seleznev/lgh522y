/**
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef SESSION_XML_H
#define SESSION_XML_H

#include "mxml/mxml.h"

struct ImageLinkList;

struct ConfigParameters {
	//                                                                                  
	char buffer_mode[64];
	//                                         
	char sample_rate[64];
	//                                     
	bool call_stack_unwinding;
	int live_rate;
};

class SessionXML {
public:
	SessionXML(const char *str);
	~SessionXML();
	void parse();
	ConfigParameters parameters;
private:
	const char *mSessionXML;
	void sessionTag(mxml_node_t *tree, mxml_node_t *node);
	void sessionImage(mxml_node_t *node);

	//                            
	SessionXML(const SessionXML &);
	SessionXML &operator=(const SessionXML &);
};

#endif //              
