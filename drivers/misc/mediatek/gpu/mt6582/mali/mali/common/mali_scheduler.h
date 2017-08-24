/*
 * Copyright (C) 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_SCHEDULER_H__
#define __MALI_SCHEDULER_H__

#include "mali_osk.h"
#include "mali_scheduler_types.h"
#include "mali_gp_scheduler.h"
#include "mali_pp_scheduler.h"

_mali_osk_errcode_t mali_scheduler_initialize(void);
void mali_scheduler_terminate(void);

u32 mali_scheduler_get_new_id(void);
u32 mali_scheduler_get_new_cache_order(void);

/* 
                          
  
                                                                              
                                                                              
          
 */
MALI_STATIC_INLINE void mali_scheduler_reset_all_groups(void)
{
	mali_gp_scheduler_reset_all_groups();
	mali_pp_scheduler_reset_all_groups();
}

/* 
                                                         
  
                                               
 */
MALI_STATIC_INLINE void mali_scheduler_zap_all_active(struct mali_session_data *session)
{
	mali_gp_scheduler_zap_all_active(session);
	mali_pp_scheduler_zap_all_active(session);
}

/* 
                                         
  
                                       
                           
                                                                        
 */
MALI_STATIC_INLINE mali_bool mali_scheduler_mask_is_set(mali_scheduler_mask mask, mali_scheduler_mask bit)
{
	return MALI_SCHEDULER_MASK_EMPTY != (bit & mask);
}

/* 
                                           
  
                                    
                                                                                        
 */
void mali_scheduler_schedule_from_mask(mali_scheduler_mask mask, mali_bool deferred_schedule);

/*                                   */
extern mali_bool mali_scheduler_hints[MALI_SCHEDULER_HINT_MAX];

MALI_STATIC_INLINE void mali_scheduler_hint_enable(mali_scheduler_hint hint)
{
	MALI_DEBUG_ASSERT(hint < MALI_SCHEDULER_HINT_MAX);
	mali_scheduler_hints[hint] = MALI_TRUE;
}

MALI_STATIC_INLINE void mali_scheduler_hint_disable(mali_scheduler_hint hint)
{
	MALI_DEBUG_ASSERT(hint < MALI_SCHEDULER_HINT_MAX);
	mali_scheduler_hints[hint] = MALI_FALSE;
}

MALI_STATIC_INLINE mali_bool mali_scheduler_hint_is_enabled(mali_scheduler_hint hint)
{
	MALI_DEBUG_ASSERT(hint < MALI_SCHEDULER_HINT_MAX);
	return mali_scheduler_hints[hint];
}

#endif /*                      */
