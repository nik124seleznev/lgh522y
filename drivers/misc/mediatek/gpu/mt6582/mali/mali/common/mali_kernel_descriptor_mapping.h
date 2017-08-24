/*
 * Copyright (C) 2010, 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                                         
 */

#ifndef __MALI_KERNEL_DESCRIPTOR_MAPPING_H__
#define __MALI_KERNEL_DESCRIPTOR_MAPPING_H__

#include "mali_osk.h"

/* 
                                                                          
 */
typedef struct mali_descriptor_table {
	u32 * usage; /*                                                                         */
	void** mappings; /*                                                */
} mali_descriptor_table;

/* 
                                
                                                                         
 */
typedef struct mali_descriptor_mapping {
	_mali_osk_mutex_rw_t *lock; /*                                                */
	int max_nr_mappings_allowed; /*                                                       */
	int current_nr_mappings; /*                                       */
	mali_descriptor_table * table; /*                                        */
} mali_descriptor_mapping;

/* 
                                     
                                                                                      
                                                                  
                                                      
                                                                  
 */
mali_descriptor_mapping * mali_descriptor_mapping_create(int init_entries, int max_entries);

/* 
                                      
                             
 */
void mali_descriptor_mapping_destroy(mali_descriptor_mapping * map);

/* 
                                               
                                    
                                                
                                    
                                                              
 */
_mali_osk_errcode_t mali_descriptor_mapping_allocate_mapping(mali_descriptor_mapping * map, void * target, int *descriptor);

/* 
                                             
                                                    
                                                
                                                                         
                                                    
 */
_mali_osk_errcode_t mali_descriptor_mapping_get(mali_descriptor_mapping * map, int descriptor, void** target);

/* 
                                             
                                                    
                                                
                                                          
                                                    
 */
_mali_osk_errcode_t mali_descriptor_mapping_set(mali_descriptor_mapping * map, int descriptor, void * target);

/* 
                                                                   
                                      
                                         
                                                                                
 */
void mali_descriptor_mapping_call_for_each(mali_descriptor_mapping * map, void (*callback)(int, void*));

/* 
                         
                                                     
                                                 
                                              
  
                                          
 */
void *mali_descriptor_mapping_free(mali_descriptor_mapping * map, int descriptor);

#endif /*                                      */
