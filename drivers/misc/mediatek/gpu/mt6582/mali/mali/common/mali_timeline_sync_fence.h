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

#ifndef __MALI_TIMELINE_SYNC_FENCE_H__
#define __MALI_TIMELINE_SYNC_FENCE_H__

#include "mali_timeline.h"

#if defined(CONFIG_SYNC)

/* 
                      
 */
struct mali_timeline_sync_fence_tracker {
	struct mali_sync_flag        *flag;    /*                                                     */
	struct mali_timeline_tracker  tracker; /*                     */
};

/* 
                                                                         
  
                                 
                                                
                                                              
 */
s32 mali_timeline_sync_fence_create(struct mali_timeline_system *system, struct mali_timeline_fence *fence);

/* 
                                                                
  
                                                
  
 */
void mali_timeline_sync_fence_activate(struct mali_timeline_sync_fence_tracker *sync_fence_tracker);

#endif /*                      */

#endif /*                                */
