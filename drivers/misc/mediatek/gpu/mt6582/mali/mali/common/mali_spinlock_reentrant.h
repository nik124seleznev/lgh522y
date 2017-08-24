/*
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_SPINLOCK_REENTRANT_H__
#define __MALI_SPINLOCK_REENTRANT_H__

#include "mali_osk.h"
#include "mali_kernel_common.h"

/* 
                      
 */
struct mali_spinlock_reentrant {
	_mali_osk_spinlock_irq_t *lock;
	u32               owner;
	u32               counter;
};

/* 
                                   
  
                                
                                  
 */
struct mali_spinlock_reentrant *mali_spinlock_reentrant_init(_mali_osk_lock_order_t lock_order);

/* 
                                                                  
  
                                                   
 */
void mali_spinlock_reentrant_term(struct mali_spinlock_reentrant *spinlock);

/* 
                                              
  
                                      
                        
 */
void mali_spinlock_reentrant_wait(struct mali_spinlock_reentrant *spinlock, u32 tid);

/* 
                             
  
                                      
                        
 */
void mali_spinlock_reentrant_signal(struct mali_spinlock_reentrant *spinlock, u32 tid);

/* 
                                                 
  
                                      
                        
                                                                      
 */
MALI_STATIC_INLINE mali_bool mali_spinlock_reentrant_is_held(struct mali_spinlock_reentrant *spinlock, u32 tid)
{
	MALI_DEBUG_ASSERT_POINTER(spinlock->lock);
	return (tid == spinlock->owner && 0 < spinlock->counter);
}

#endif /*                               */
