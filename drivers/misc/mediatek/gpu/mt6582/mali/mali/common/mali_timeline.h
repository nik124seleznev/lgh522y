/*
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_TIMELINE_H__
#define __MALI_TIMELINE_H__

#include "mali_osk.h"
#include "mali_ukk.h"
#include "mali_session.h"
#include "mali_kernel_common.h"
#include "mali_spinlock_reentrant.h"
#include "mali_sync.h"
#include "mali_scheduler_types.h"

/* 
                    
  
                                                                                                    
                                                                                    
 */
#define MALI_TIMELINE_TIMEOUT_HZ ((u32) (HZ * 3 / 2)) /*          */

/* 
                 
 */
typedef enum mali_timeline_id {
	MALI_TIMELINE_GP   = MALI_UK_TIMELINE_GP,   /*                    */
	MALI_TIMELINE_PP   = MALI_UK_TIMELINE_PP,   /*                    */
	MALI_TIMELINE_SOFT = MALI_UK_TIMELINE_SOFT, /*                      */
	MALI_TIMELINE_MAX  = MALI_UK_TIMELINE_MAX
} mali_timeline_id;

/* 
                                                                                                   
 */
#define MALI_TIMELINE_NONE MALI_TIMELINE_MAX

/* 
                
 */
typedef enum mali_timeline_tracker_type {
	MALI_TIMELINE_TRACKER_GP   = 0, /*                            */
	MALI_TIMELINE_TRACKER_PP   = 1, /*                            */
	MALI_TIMELINE_TRACKER_SOFT = 2, /*                              */
	MALI_TIMELINE_TRACKER_WAIT = 3, /*                                */
	MALI_TIMELINE_TRACKER_SYNC = 4, /*                                */
	MALI_TIMELINE_TRACKER_MAX  = 5,
} mali_timeline_tracker_type;

/* 
                            
 */
typedef u32 mali_timeline_activation_error;
#define MALI_TIMELINE_ACTIVATION_ERROR_NONE      0
#define MALI_TIMELINE_ACTIVATION_ERROR_SYNC_BIT  (1<<1)
#define MALI_TIMELINE_ACTIVATION_ERROR_FATAL_BIT (1<<0)

/* 
                                                
 */
typedef u32 mali_timeline_point;

/* 
                                                 
 */
#define MALI_TIMELINE_NO_POINT ((mali_timeline_point) 0)

/* 
                                                                                                  
                                                                         
 */
#define MALI_TIMELINE_MAX_POINT_SPAN 65536

/* 
                                                      
 */
#define MALI_TIMELINE_TRACKER_MAGIC 0xabcdabcd

struct mali_timeline;
struct mali_timeline_waiter;
struct mali_timeline_tracker;

/* 
                  
 */
struct mali_timeline_fence {
	mali_timeline_point points[MALI_TIMELINE_MAX]; /*                                                         */
	s32                 sync_fd;                   /*                                                       */
};

/* 
                   
  
                                                                        
 */
struct mali_timeline_system {
	struct mali_spinlock_reentrant *spinlock;   /*                                            */
	struct mali_timeline           *timelines[MALI_TIMELINE_MAX]; /*                                */

	/*                                                                                       */
	struct mali_timeline_waiter    *waiter_empty_list;

	struct mali_session_data       *session;    /*                                  */

	mali_bool                       timer_enabled; /*                                                                            */

	_mali_osk_wait_queue_t         *wait_queue; /*               */

#if defined(CONFIG_SYNC)
	struct sync_timeline           *signaled_sync_tl; /*                                                                 */
#endif /*                      */
};

/* 
                                                                         
 */
struct mali_timeline {
	mali_timeline_point           point_next;   /*                             */
	mali_timeline_point           point_oldest; /*                                  */

	/*                                                                                        
                                                       */
	struct mali_timeline_tracker *tracker_head;
	struct mali_timeline_tracker *tracker_tail;

	/*                                                                                      
                                                            */
	struct mali_timeline_waiter  *waiter_head;
	struct mali_timeline_waiter  *waiter_tail;

	struct mali_timeline_system  *system;       /*                                             */
	enum mali_timeline_id         id;           /*                  */

#if defined(CONFIG_SYNC)
	struct sync_timeline         *sync_tl;      /*                                                    */
#endif /*                      */

	/*                                                              */
	_mali_osk_wq_delayed_work_t  *delayed_work;
	mali_bool                     timer_active;
};

/* 
                   
 */
struct mali_timeline_waiter {
	mali_timeline_point           point;         /*                                                        */
	struct mali_timeline_tracker *tracker;       /*                            */

	struct mali_timeline_waiter  *timeline_next; /*                                          */
	struct mali_timeline_waiter  *timeline_prev; /*                                              */

	struct mali_timeline_waiter  *tracker_next;  /*                                         */
};

/* 
                    
 */
struct mali_timeline_tracker {
	MALI_DEBUG_CODE(u32            magic); /*                                                                     */

	mali_timeline_point            point; /*                                      */

	struct mali_timeline_tracker  *timeline_next; /*                                           */
	struct mali_timeline_tracker  *timeline_prev; /*                                               */

	u32                            trigger_ref_count; /*                                       */
	mali_timeline_activation_error activation_error;  /*                     */
	struct mali_timeline_fence     fence;             /*                                     */

	/*                                                                   
                                   */
	struct mali_timeline_waiter   *waiter_head;
	struct mali_timeline_waiter   *waiter_tail;

#if defined(CONFIG_SYNC)
	/*                                                                */
	struct mali_timeline_waiter   *waiter_sync; /*                                                                */
	struct sync_fence_waiter       sync_fence_waiter; /*                                                                       */
	struct sync_fence             *sync_fence;   /*                                              */
	_mali_osk_list_t               sync_fence_cancel_list; /*                                                */
#endif /*                      */

	struct mali_timeline_system   *system;       /*                    */
	struct mali_timeline          *timeline;     /*                                           */
	enum mali_timeline_tracker_type type;        /*                    */
	void                          *job;          /*                     */

	/*                                                              */
	u32                           os_tick_create;
	u32                           os_tick_activate;
	mali_bool                     timer_active;
};

/* 
                                                                                                  
                                                                                                   
                                                                                            
                     
  
                                                                                                    
                                                   
  
  
  
                                                               
                                                    
                                                              
  
                                                                                   
  
                                                                                           
                                  
                                  
                                  
                                  
                                                        
 */

/* 
                              
  
                                                                    
  
                                                                               
                                                                          
  
                             
                             
                                    
 */
MALI_STATIC_INLINE mali_bool mali_timeline_point_after(mali_timeline_point a, mali_timeline_point b)
{
	return 0 > ((s32)b) - ((s32)a);
}

/* 
                                                                                                 
                                                  
  
                            
                                  
                                                                
 */
MALI_STATIC_INLINE mali_bool mali_timeline_is_point_on(struct mali_timeline *timeline, mali_timeline_point point)
{
	MALI_DEBUG_ASSERT_POINTER(timeline);
	MALI_DEBUG_ASSERT(MALI_TIMELINE_NO_POINT != point);

	return (point - timeline->point_oldest) < (timeline->point_next - timeline->point_oldest);
}

/* 
                                                                                                   
                                                                               
  
                            
                                  
                                                                  
 */
MALI_STATIC_INLINE mali_bool mali_timeline_is_point_released(struct mali_timeline *timeline, mali_timeline_point point)
{
	mali_timeline_point point_normalized;
	mali_timeline_point next_normalized;

	MALI_DEBUG_ASSERT_POINTER(timeline);
	MALI_DEBUG_ASSERT(MALI_TIMELINE_NO_POINT != point);

	point_normalized = point - timeline->point_oldest;
	next_normalized = timeline->point_next - timeline->point_oldest;

	return point_normalized > (next_normalized + MALI_TIMELINE_MAX_POINT_SPAN);
}

/* 
                                                                                           
  
                            
                                  
                                                          
 */
MALI_STATIC_INLINE mali_bool mali_timeline_is_point_valid(struct mali_timeline *timeline, mali_timeline_point point)
{
	MALI_DEBUG_ASSERT_POINTER(timeline);
	return mali_timeline_is_point_on(timeline, point) || mali_timeline_is_point_released(timeline, point);
}

/* 
                                                                                            
  
                            
                                                             
 */
MALI_STATIC_INLINE mali_bool mali_timeline_is_empty(struct mali_timeline *timeline)
{
	MALI_DEBUG_ASSERT_POINTER(timeline);
	return timeline->point_next == timeline->point_oldest;
}

/* 
                                                                                      
                                 
  
                            
                                                            
 */
MALI_STATIC_INLINE mali_bool mali_timeline_is_full(struct mali_timeline *timeline)
{
	MALI_DEBUG_ASSERT_POINTER(timeline);
	return MALI_TIMELINE_MAX_POINT_SPAN <= (timeline->point_next - timeline->point_oldest);
}

/* 
                                
  
                                                                  
                               
 */
struct mali_timeline_system *mali_timeline_system_create(struct mali_session_data *session);

/* 
                         
  
                                                                                          
             
  
                                                        
 */
void mali_timeline_system_abort(struct mali_timeline_system *system);

/* 
                                    
  
                                                                                   
  
                                            
 */
void mali_timeline_system_destroy(struct mali_timeline_system *system);

/* 
                           
  
                                
 */
void mali_timeline_system_stop_timer(struct mali_timeline_system *system);

/* 
                                                                        
  
                                                                                                    
                                                                                                  
                                                               
  
                                                                                                
                   
  
                                                              
                                          
                                                                         
                                                                                 
                                                                                                    
 */
mali_timeline_point mali_timeline_system_add_tracker(struct mali_timeline_system *system,
        struct mali_timeline_tracker *tracker,
        enum mali_timeline_id timeline_id);

/* 
                                
  
                                 
                                                              
                                                                                        
 */
mali_timeline_point mali_timeline_system_get_latest_point(struct mali_timeline_system *system,
        enum mali_timeline_id timeline_id);

/* 
                      
  
                                                                                                     
  
                                        
                               
                                                              
                                                                    
 */
void mali_timeline_tracker_init(struct mali_timeline_tracker *tracker,
                                mali_timeline_tracker_type type,
                                struct mali_timeline_fence *fence,
                                void *job);

/* 
                                     
  
                                                                                           
  
                                                                              
  
                                 
                          
 */
void mali_timeline_system_tracker_get(struct mali_timeline_system *system, struct mali_timeline_tracker *tracker);

/* 
                                        
  
                                                                        
  
                                 
                          
                                                                                                                     
                              
 */
mali_scheduler_mask mali_timeline_system_tracker_put(struct mali_timeline_system *system, struct mali_timeline_tracker *tracker, mali_timeline_activation_error activation_error);

/* 
                                              
  
                                                                                                    
                                            
  
                                                                                             
                                                                                                    
                                                                                               
  
                                                                
                                                                                                   
                 
  
                                         
                              
 */
mali_scheduler_mask mali_timeline_tracker_release(struct mali_timeline_tracker *tracker);

/* 
                                                 
  
                               
                            
 */
void mali_timeline_fence_copy_uk_fence(struct mali_timeline_fence *fence, _mali_uk_fence_t *uk_fence);

#define MALI_TIMELINE_DEBUG_FUNCTIONS
#if defined(MALI_TIMELINE_DEBUG_FUNCTIONS)

/* 
                                           
 */
typedef enum mali_timeline_tracker_state {
	MALI_TIMELINE_TS_INIT    = 0,
	MALI_TIMELINE_TS_WAITING = 1,
	MALI_TIMELINE_TS_ACTIVE  = 2,
	MALI_TIMELINE_TS_FINISH  = 3,
} mali_timeline_tracker_state;

/* 
                     
  
                                   
                            
 */
mali_timeline_tracker_state mali_timeline_debug_get_tracker_state(struct mali_timeline_tracker *tracker);

/* 
                                         
  
                                   
 */
void mali_timeline_debug_print_tracker(struct mali_timeline_tracker *tracker);

/* 
                                          
  
                                     
 */
void mali_timeline_debug_print_timeline(struct mali_timeline *timeline);

/* 
                                                 
  
                                          
 */
void mali_timeline_debug_print_system(struct mali_timeline_system *system);

#endif /*                                        */

#endif /*                     */
