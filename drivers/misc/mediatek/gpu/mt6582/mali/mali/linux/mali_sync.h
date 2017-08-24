/*
 * Copyright (C) 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                    
  
                                         
 */

#ifndef _MALI_SYNC_H_
#define _MALI_SYNC_H_

#if defined(CONFIG_SYNC)

#include <linux/seq_file.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
#include "sync.h"
#else
#include <linux/sync.h>
#endif

#include "mali_osk.h"

struct mali_sync_flag;

/* 
                          
  
                                         
                                                            
 */
struct sync_timeline *mali_sync_timeline_create(const char *name);

/* 
                                          
  
                                         
                                                                         
 */
mali_bool mali_sync_timeline_is_ours(struct sync_timeline *sync_tl);

/* 
                                                                                                   
                         
  
                                
                                                                               
 */
s32 mali_sync_fence_fd_alloc(struct sync_fence *sync_fence);

/* 
                                                                    
  
                                       
                                        
                                                                                         
 */
struct sync_fence *mali_sync_fence_merge(struct sync_fence *sync_fence1, struct sync_fence *sync_fence2);

/* 
                                                
  
                           
                                                              
 */
struct sync_fence *mali_sync_timeline_create_signaled_fence(struct sync_timeline *sync_tl);

/* 
                      
  
                                
                                       
                                                    
 */
struct mali_sync_flag *mali_sync_flag_create(struct sync_timeline *sync_tl, u32 point);

/* 
                            
  
                         
 */
void mali_sync_flag_get(struct mali_sync_flag *flag);

/* 
                                                                                             
  
                         
 */
void mali_sync_flag_put(struct mali_sync_flag *flag);

/* 
                                                                              
  
                                   
                                                      
 */
void mali_sync_flag_signal(struct mali_sync_flag *flag, int error);

/* 
                                                   
  
                         
                                                     
 */
struct sync_fence *mali_sync_flag_create_fence(struct mali_sync_flag *flag);

#endif /*                      */

#endif /*               */
