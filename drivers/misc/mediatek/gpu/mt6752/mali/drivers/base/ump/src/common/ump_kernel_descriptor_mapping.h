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





/* 
                                        
 */

#ifndef _UMP_KERNEL_DESCRIPTOR_MAPPING_H_
#define _UMP_KERNEL_DESCRIPTOR_MAPPING_H_

#include <linux/rwsem.h>
#include <linux/slab.h>
/* 
                                                                          
 */
typedef struct umpp_descriptor_table
{
	/*                                                            */
	unsigned long * usage; /*                                                                               */
	void** mappings; /*                                                */
} umpp_descriptor_table;

/* 
                                
                                                                         
 */
typedef struct umpp_descriptor_mapping
{
	struct rw_semaphore lock; /*                                                */
	unsigned int max_nr_mappings_allowed; /*                                                       */
	unsigned int current_nr_mappings; /*                                       */
	umpp_descriptor_table * table; /*                                        */
} umpp_descriptor_mapping;

/* 
                                      
                                                                                       
                                                                       
                                                                  
                                                      
                                                                  
 */
umpp_descriptor_mapping * umpp_descriptor_mapping_create(unsigned int init_entries, unsigned int max_entries);

/* 
                                      
                                 
 */
void umpp_descriptor_mapping_destroy(umpp_descriptor_mapping * map);

/* 
                                               
                                    
                                                    
                                        
                                                     
 */
unsigned int umpp_descriptor_mapping_allocate(umpp_descriptor_mapping * map, void * target);

/* 
                                             
                                                        
                                                    
                                                                              
  
                                                          
 */
int umpp_descriptor_mapping_lookup(umpp_descriptor_mapping * map, unsigned int descriptor, void** const target);

/* 
                         
                                                     
                                                     
                                              
 */
void umpp_descriptor_mapping_remove(umpp_descriptor_mapping * map, unsigned int descriptor);

#endif /*                                   */
