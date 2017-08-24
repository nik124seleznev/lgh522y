/*
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_SOFT_JOB_H__
#define __MALI_SOFT_JOB_H__

#include "mali_osk.h"

#include "mali_timeline.h"

struct mali_timeline_fence;
struct mali_session_data;
struct mali_soft_job;
struct mali_soft_job_system;

/* 
                  
  
                                                                                                   
                                                                                                    
                          
 */
typedef enum mali_soft_job_type {
	MALI_SOFT_JOB_TYPE_USER_SIGNALED,
} mali_soft_job_type;

/* 
                  
  
                                                                                                   
                                                                                
                                                                                                  
                                                                                                  
  
                                                                                 
                                                                                                
                                                                               
  
                                                                                                 
                                                                                                 
                               
  
                                                                                            
                            
 */
typedef enum mali_soft_job_state {
	MALI_SOFT_JOB_STATE_FREE,
	MALI_SOFT_JOB_STATE_ALLOCATED,
	MALI_SOFT_JOB_STATE_STARTED,
	MALI_SOFT_JOB_STATE_SIGNALED,
	MALI_SOFT_JOB_STATE_TIMED_OUT,
} mali_soft_job_state;

#define MALI_SOFT_JOB_INVALID_ID ((u32) -1)

/*                                              */
#define MALI_MAX_NUM_SOFT_JOBS 20

/* 
                   
  
                                                                               
 */
typedef struct mali_soft_job {
	mali_soft_job_type            type;                   /*                                                        */
	u32                           user_job;               /*                                          */
	_mali_osk_atomic_t            refcount;               /*                                                                  */
	struct mali_timeline_tracker  tracker;                /*                                  */
	mali_bool                     activated;              /*                                                               */
	_mali_osk_notification_t     *activated_notification; /*                                                                 */

	/*                                    */
	u32                           id;                     /*                                                                      */
	mali_soft_job_state           state;                  /*                                                            */
	struct mali_soft_job_system  *system;                 /*                                       */
	_mali_osk_list_t              system_list;            /*                                         */
} mali_soft_job;

/* 
                               
  
                                                                                 
 */
typedef struct mali_soft_job_system {
	struct mali_session_data *session;                    /*                                                */

	struct mali_soft_job jobs[MALI_MAX_NUM_SOFT_JOBS];    /*                                          */
	_MALI_OSK_LIST_HEAD(jobs_free);                       /*                               */
	_MALI_OSK_LIST_HEAD(jobs_used);                       /*                                    */

	_mali_osk_spinlock_irq_t *lock;                       /*                                                           */
	u32 lock_owner;                                       /*                                                                      */
} mali_soft_job_system;

/* 
                            
  
                                                                  
                                                            
 */
struct mali_soft_job_system *mali_soft_job_system_create(struct mali_session_data *session);

/* 
                             
  
                                                                             
                                    
  
                                                       
 */
void mali_soft_job_system_destroy(struct mali_soft_job_system *system);

/* 
                     
  
                                                         
                                    
                                                         
                                                   
 */
struct mali_soft_job *mali_soft_job_create(struct mali_soft_job_system *system, mali_soft_job_type type, u32 user_job);

/* 
                    
  
                                  
 */
void mali_soft_job_destroy(struct mali_soft_job *job);

/* 
                    
  
                                                                                          
                                   
  
                                                                        
  
                                
                                                                  
                                      
 */
mali_timeline_point mali_soft_job_start(struct mali_soft_job *job, struct mali_timeline_fence *fence);

/* 
                                                             
  
                                                                             
  
                                                                                                   
  
                                                                                                   
                                                          
  
                                                                        
  
                                                            
                                                 
  
                                                                                                    
                                                                                       
 */
_mali_osk_errcode_t mali_soft_job_system_signal_job(struct mali_soft_job_system *system, u32 job_id);

/* 
                                                      
  
                                                   
 */
void mali_soft_job_system_activate_job(struct mali_soft_job *job);

/* 
                                                     
  
                                                                                                   
              
  
                                                   
                                
 */
mali_scheduler_mask mali_soft_job_system_timeout_job(struct mali_soft_job *job);

/* 
                                                                               
  
                                                           
 */
void mali_soft_job_system_abort(struct mali_soft_job_system *system);

#endif /*                     */
