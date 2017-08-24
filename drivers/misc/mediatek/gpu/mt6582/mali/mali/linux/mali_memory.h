/*
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_MEMORY_H__
#define __MALI_MEMORY_H__

#include "mali_osk.h"
#include "mali_session.h"

#include <linux/list.h>
#include <linux/mm.h>

#include "mali_memory_types.h"
#include "mali_memory_os_alloc.h"

_mali_osk_errcode_t mali_memory_initialize(void);
void mali_memory_terminate(void);

/*                                   
  
                                                                         
                            
  
                                                               
                                                      
                                                                  
 */
MALI_STATIC_INLINE _mali_osk_errcode_t mali_mmu_get_table_page(u32 *table_page, mali_io_address *mapping)
{
	return mali_mem_os_get_table_page(table_page, mapping);
}

/*                                  
  
                                                                         
  
                                                   
 */
MALI_STATIC_INLINE void mali_mmu_release_table_page(u32 phys, void *virt)
{
	mali_mem_os_release_table_page(phys, virt);
}

/*                      
  
                                                          
  
                                                                   
 */
int mali_mmap(struct file *filp, struct vm_area_struct *vma);

/*                                                         
  
                                                                  
                                                           
 */
mali_mem_allocation *mali_mem_descriptor_create(struct mali_session_data *session, mali_mem_type type);

/*                                         
  
                                                                            
              
  
                                                         
 */
void mali_mem_descriptor_destroy(mali_mem_allocation *descriptor);

/*                                   
  
                                                    
  
                                                  
 */
_mali_osk_errcode_t mali_memory_session_begin(struct mali_session_data *session);

/*                               
  
                                                     
  
                                                
  
                                                     
 */
void mali_memory_session_end(struct mali_session_data *session);

/*                                             
  
                                                                         
                              
  
                                                                           
  
                                                                    
 */
_mali_osk_errcode_t mali_mem_mali_map_prepare(mali_mem_allocation *descriptor);

/*                                          
  
                                                                           
                       
  
                                                                                                            
  
                                                              
 */
void mali_mem_mali_map_free(mali_mem_allocation *descriptor);

/*                                                           
  
                                                     
                                                          
 */
_mali_osk_errcode_t mali_memory_core_resource_os_memory(u32 size);

/*                                                                  
  
                                                                    
                                                 
                                                          
 */
_mali_osk_errcode_t mali_memory_core_resource_dedicated_memory(u32 start, u32 size);


void mali_mem_ump_release(mali_mem_allocation *descriptor);
void mali_mem_external_release(mali_mem_allocation *descriptor);

#endif /*                   */
