/*
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                                   
  
                                                                                
 */

#ifndef __MALI_TIMELINE_FENCE_WAIT_H__
#define __MALI_TIMELINE_FENCE_WAIT_H__

#include "mali_osk.h"
#include "mali_timeline.h"

/* 
                                                                                                
                                                    
 */
#define MALI_TIMELINE_FENCE_WAIT_TIMEOUT_NEVER ((u32) -1)

/* 
                                                                                             
                                                   
 */
#define MALI_TIMELINE_FENCE_WAIT_TIMEOUT_IMMEDIATELY 0

/* 
                      
  
                                                                                               
                                                                                                   
                                                       
 */
struct mali_timeline_fence_wait_tracker {
	mali_bool activated;                  /*                                                                   */
	_mali_osk_atomic_t refcount;          /*                    */
	struct mali_timeline_system *system;  /*                    */
	struct mali_timeline_tracker tracker; /*                     */
};

/* 
                                                          
  
                                 
                                 
                                                                             
                                                
                                                          
 */
mali_bool mali_timeline_fence_wait(struct mali_timeline_system *system, struct mali_timeline_fence *fence, u32 timeout);

/* 
                                                                
  
                                                  
 */
void mali_timeline_fence_wait_activate(struct mali_timeline_fence_wait_tracker *fence_wait_tracker);

#endif /*                                */
