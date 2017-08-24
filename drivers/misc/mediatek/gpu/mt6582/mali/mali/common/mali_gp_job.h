/*
 * Copyright (C) 2011-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_GP_JOB_H__
#define __MALI_GP_JOB_H__

#include "mali_osk.h"
#include "mali_osk_list.h"
#include "mali_uk_types.h"
#include "mali_session.h"
#include "mali_timeline.h"
#include "mali_scheduler_types.h"

/* 
                                                            
                                                                                    
                   
 */
struct mali_gp_job {
	_mali_osk_list_t list;                             /*                                                     */
	struct mali_session_data *session;                 /*                                    */
	_mali_uk_gp_start_job_s uargs;                     /*                             */
	u32 id;                                            /*                                                                  */
	u32 cache_order;                                   /*                                                                 */
	u32 heap_current_addr;                             /*                                                             */
	u32 perf_counter_value0;                           /*                                                                 */
	u32 perf_counter_value1;                           /*                                                                 */
	u32 pid;                                           /*                                    */
	u32 tid;                                           /*                                  */
	_mali_osk_notification_t *finished_notification;   /*                                                       */
	_mali_osk_notification_t *oom_notification;        /*                                              */
	struct mali_timeline_tracker tracker;              /*                                 */
	struct mali_timeline_tracker *pp_tracker;          /*                                                                    */
};

struct mali_gp_job *mali_gp_job_create(struct mali_session_data *session, _mali_uk_gp_start_job_s *uargs, u32 id, struct mali_timeline_tracker *pp_tracker);
void mali_gp_job_delete(struct mali_gp_job *job);

u32 mali_gp_job_get_gp_counter_src0(void);
void mali_gp_job_set_gp_counter_src0(u32 counter);
u32 mali_gp_job_get_gp_counter_src1(void);
void mali_gp_job_set_gp_counter_src1(u32 counter);

MALI_STATIC_INLINE u32 mali_gp_job_get_id(struct mali_gp_job *job)
{
	return (NULL == job) ? 0 : job->id;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_cache_order(struct mali_gp_job *job)
{
	return (NULL == job) ? 0 : job->cache_order;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_user_id(struct mali_gp_job *job)
{
	return job->uargs.user_job_ptr;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_frame_builder_id(struct mali_gp_job *job)
{
	return job->uargs.frame_builder_id;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_flush_id(struct mali_gp_job *job)
{
	return job->uargs.flush_id;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_pid(struct mali_gp_job *job)
{
	return job->pid;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_tid(struct mali_gp_job *job)
{
	return job->tid;
}

MALI_STATIC_INLINE u32* mali_gp_job_get_frame_registers(struct mali_gp_job *job)
{
	return job->uargs.frame_registers;
}

MALI_STATIC_INLINE struct mali_session_data *mali_gp_job_get_session(struct mali_gp_job *job)
{
	return job->session;
}

MALI_STATIC_INLINE mali_bool mali_gp_job_has_vs_job(struct mali_gp_job *job)
{
	return (job->uargs.frame_registers[0] != job->uargs.frame_registers[1]) ? MALI_TRUE : MALI_FALSE;
}

MALI_STATIC_INLINE mali_bool mali_gp_job_has_plbu_job(struct mali_gp_job *job)
{
	return (job->uargs.frame_registers[2] != job->uargs.frame_registers[3]) ? MALI_TRUE : MALI_FALSE;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_current_heap_addr(struct mali_gp_job *job)
{
	return job->heap_current_addr;
}

MALI_STATIC_INLINE void mali_gp_job_set_current_heap_addr(struct mali_gp_job *job, u32 heap_addr)
{
	job->heap_current_addr = heap_addr;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_perf_counter_flag(struct mali_gp_job *job)
{
	return job->uargs.perf_counter_flag;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_perf_counter_src0(struct mali_gp_job *job)
{
	return job->uargs.perf_counter_src0;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_perf_counter_src1(struct mali_gp_job *job)
{
	return job->uargs.perf_counter_src1;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_perf_counter_value0(struct mali_gp_job *job)
{
	return job->perf_counter_value0;
}

MALI_STATIC_INLINE u32 mali_gp_job_get_perf_counter_value1(struct mali_gp_job *job)
{
	return job->perf_counter_value1;
}

MALI_STATIC_INLINE void mali_gp_job_set_perf_counter_src0(struct mali_gp_job *job, u32 src)
{
	job->uargs.perf_counter_src0 = src;
}

MALI_STATIC_INLINE void mali_gp_job_set_perf_counter_src1(struct mali_gp_job *job, u32 src)
{
	job->uargs.perf_counter_src1 = src;
}

MALI_STATIC_INLINE void mali_gp_job_set_perf_counter_value0(struct mali_gp_job *job, u32 value)
{
	job->perf_counter_value0 = value;
}

MALI_STATIC_INLINE void mali_gp_job_set_perf_counter_value1(struct mali_gp_job *job, u32 value)
{
	job->perf_counter_value1 = value;
}

/* 
                                                                             
  
                          
                            
                                                                                            
 */
MALI_STATIC_INLINE mali_bool mali_gp_job_is_after(struct mali_gp_job *first, struct mali_gp_job *second)
{
	/*                                           */
	return (mali_gp_job_get_id(first) - mali_gp_job_get_id(second)) < MALI_SCHEDULER_JOB_ID_SPAN;
}

/* 
                                                                       
  
                                                                                                
           
  
                                  
                                                                             
                                                                               
 */
mali_scheduler_mask mali_gp_job_signal_pp_tracker(struct mali_gp_job *job, mali_bool success);

#endif /*                   */
