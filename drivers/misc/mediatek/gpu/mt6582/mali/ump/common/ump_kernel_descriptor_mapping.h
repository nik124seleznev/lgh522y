/*
 * Copyright (C) 2010-2011, 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                                        
 */

#ifndef __UMP_KERNEL_DESCRIPTOR_MAPPING_H__
#define __UMP_KERNEL_DESCRIPTOR_MAPPING_H__

#include "mali_osk.h"

/* 
                                                                          
 */
typedef struct ump_descriptor_table {
	u32 * usage; /*                                                                         */
	void** mappings; /*                                                */
} ump_descriptor_table;

/* 
                                
                                                                         
 */
typedef struct ump_descriptor_mapping {
	_mali_osk_mutex_rw_t *lock; /*                                                */
	int max_nr_mappings_allowed; /*                                                       */
	int current_nr_mappings; /*                                       */
	ump_descriptor_table * table; /*                                        */
} ump_descriptor_mapping;

/* 
                                     
                                                                                      
                                                                  
                                                      
                                                                  
 */
ump_descriptor_mapping * ump_descriptor_mapping_create(int init_entries, int max_entries);

/* 
                                      
                             
 */
void ump_descriptor_mapping_destroy(ump_descriptor_mapping * map);

/* 
                                               
                                    
                                                
                                    
                                                              
 */
int ump_descriptor_mapping_allocate_mapping(ump_descriptor_mapping * map, void * target);

/* 
                                             
                                                    
                                                
                                                                         
                                                    
 */
int ump_descriptor_mapping_get(ump_descriptor_mapping * map, int descriptor, void** target);

/* 
                                             
                                                    
                                                
                                                          
                                                    
 */
int ump_descriptor_mapping_set(ump_descriptor_mapping * map, int descriptor, void * target);

/* 
                         
                                                     
                                                 
                                              
 */
void ump_descriptor_mapping_free(ump_descriptor_mapping * map, int descriptor);

#endif /*                                     */
