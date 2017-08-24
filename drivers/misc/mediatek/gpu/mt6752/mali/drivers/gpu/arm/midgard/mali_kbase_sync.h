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

#ifndef MALI_KBASE_SYNC_H
#define MALI_KBASE_SYNC_H

#include "sync.h"
#include <malisw/mali_malisw.h>

/*
                          
                                   
                                
                                                
                     
                          
 */
mali_error kbase_stream_create(const char *name, int *const out_fd);

/*
                                    
 */
int kbase_stream_create_fence(int tl_fd);

/*
                                    
                         
  
                                                                         
                                                                               
 */
mali_error kbase_fence_validate(int fd);

/*                                                             */
int kbase_sync_timeline_is_ours(struct sync_timeline *timeline);

/*                              
  
                                                    
 */
struct sync_timeline *kbase_sync_timeline_alloc(const char *name);

/*                                            
  
                                                                      
  
                                                                                   
 */
struct sync_pt *kbase_sync_pt_alloc(struct sync_timeline *parent);

/*                                
  
                                                                                   
  
                                                                                   
                                                                          
  
                                                                                       
 */
void kbase_sync_signal_pt(struct sync_pt *pt, int result);

#endif
