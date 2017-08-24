/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





#ifndef _UMP_IMPORT_H_
#define _UMP_IMPORT_H_

#include <linux/ump.h>
#include <linux/module.h>

/* 
                          
                                                                           
                                                                            
                                                             
 */
struct ump_import_handler
{
	/* 
                                      
  */
	struct module * linux_module;

	/* 
                            
   
                                                            
                                                                         
                                                                                         
                                                         
   
                                                                    
                                                                     
   
                                                                                             
                                                                           
  */
	int (*session_begin)(void ** custom_session_data);

	/* 
                          
   
                                                             
                                                
   
                                                                             
  */
	void (*session_end)(void * custom_session_data);

	/* 
                   
   
                                                                                                        
                                                
   
                                                                                     
                                                     
                                                                                                    
   
                                                                             
                                                      
                                                                               
                                                                                            
  */
	ump_dd_handle (*import)(void * custom_session_data, void * phandle, ump_alloc_flags flags);
};

/* 
                              
                                                              
                                                                  
                                                 
                                                      
 */
int ump_import_module_register(enum ump_external_memory_type type, struct ump_import_handler * handler);

/* 
                                
                                                    
                                                               
 */
void ump_import_module_unregister(enum ump_external_memory_type type);

#endif /*                */
