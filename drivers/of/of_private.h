#ifndef _LINUX_OF_PRIVATE_H
#define _LINUX_OF_PRIVATE_H
/*
 * Private symbols used by OF support code
 *
 * Paul Mackerras	August 1996.
 * Copyright (C) 1996-2005 Paul Mackerras.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

/* 
                                                       
                                                                
                              
                                                         
                                           
                                        
  
                                                                   
                                   
 */
struct alias_prop {
	struct list_head link;
	const char *alias;
	struct device_node *np;
	int id;
	char stem[0];
};

extern struct mutex of_aliases_mutex;
extern struct list_head aliases_lookup;
#endif /*                     */
