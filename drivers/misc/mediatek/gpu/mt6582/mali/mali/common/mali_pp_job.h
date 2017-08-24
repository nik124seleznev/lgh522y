/*
 * Copyright (C) 2011-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_PP_JOB_H__
#define __MALI_PP_JOB_H__

#include "mali_osk.h"
#include "mali_osk_list.h"
#include "mali_uk_types.h"
#include "mali_session.h"
#include "mali_kernel_common.h"
#include "regs/mali_200_regs.h"
#include "mali_kernel_core.h"
#include "mali_dma.h"
#include "mali_dlbu.h"
#include "mali_timeline.h"
#if defined(CONFIG_DMA_SHARED_BUFFER) && !defined(CONFIG_MALI_DMA_BUF_MAP_ON_ATTACH)
#include "linux/mali_memory_dma_buf.h"
#endif

/* 
                                                            
                                                                                    
                   
 */
struct mali_pp_job {
	_mali_osk_list_t list;                             /*                                                     */
	struct mali_session_data *session;                 /*                                    */
	_mali_osk_list_t session_list;                     /*                                                      */
	_mali_osk_list_t session_fb_lookup_list;           /*                                                                         */
	_mali_uk_pp_start_job_s uargs;                     /*                             */
	mali_dma_cmd_buf dma_cmd_buf;                      /*                                                           */
	u32 id;                                            /*                                                                  */
	u32 cache_order;                                   /*                                                                 */
	u32 perf_counter_value0[_MALI_PP_MAX_SUB_JOBS];    /*                                                                                       */
	u32 perf_counter_value1[_MALI_PP_MAX_SUB_JOBS];    /*                                                                                       */
	u32 sub_jobs_num;                                  /*                                                                                                 */
	u32 sub_jobs_started;                              /*                                                                        */
	u32 sub_jobs_completed;                            /*                                                 */
	u32 sub_job_errors;                                /*                                                                           */
	u32 pid;                                           /*                                    */
	u32 tid;                                           /*                                  */
	_mali_osk_notification_t *finished_notification;   /*                                                       */
	u32 num_memory_cookies;                            /*                                            */
	u32 *memory_cookies;                               /*                                  */
#if defined(CONFIG_DMA_SHARED_BUFFER) && !defined(CONFIG_MALI_DMA_BUF_MAP_ON_ATTACH)
	struct mali_dma_buf_attachment **dma_bufs;         /*                                 */
	u32 num_dma_bufs;                                  /*                                  */
#endif
	struct mali_timeline_tracker tracker;              /*                                 */
	u32 perf_counter_per_sub_job_count;                /*                                                                          */
	u32 perf_counter_per_sub_job_src0[_MALI_PP_MAX_SUB_JOBS]; /*                             */
	u32 perf_counter_per_sub_job_src1[_MALI_PP_MAX_SUB_JOBS]; /*                             */
};

void mali_pp_job_initialize(void);
void mali_pp_job_terminate(void);

struct mali_pp_job *mali_pp_job_create(struct mali_session_data *session, _mali_uk_pp_start_job_s *uargs, u32 id);
void mali_pp_job_delete(struct mali_pp_job *job);

u32 mali_pp_job_get_perf_counter_src0(struct mali_pp_job *job, u32 sub_job);
u32 mali_pp_job_get_perf_counter_src1(struct mali_pp_job *job, u32 sub_job);

void mali_pp_job_set_pp_counter_global_src0(u32 counter);
void mali_pp_job_set_pp_counter_global_src1(u32 counter);
void mali_pp_job_set_pp_counter_sub_job_src0(u32 sub_job, u32 counter);
void mali_pp_job_set_pp_counter_sub_job_src1(u32 sub_job, u32 counter);

u32 mali_pp_job_get_pp_counter_global_src0(void);
u32 mali_pp_job_get_pp_counter_global_src1(void);
u32 mali_pp_job_get_pp_counter_sub_job_src0(u32 sub_job);
u32 mali_pp_job_get_pp_counter_sub_job_src1(u32 sub_job);

MALI_STATIC_INLINE u32 mali_pp_job_get_id(struct mali_pp_job *job)
{
	return (NULL == job) ? 0 : job->id;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_cache_order(struct mali_pp_job *job)
{
	return (NULL == job) ? 0 : job->cache_order;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_user_id(struct mali_pp_job *job)
{
	return job->uargs.user_job_ptr;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_frame_builder_id(struct mali_pp_job *job)
{
	return job->uargs.frame_builder_id;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_flush_id(struct mali_pp_job *job)
{
	return job->uargs.flush_id;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_pid(struct mali_pp_job *job)
{
	return job->pid;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_tid(struct mali_pp_job *job)
{
	return job->tid;
}

MALI_STATIC_INLINE u32* mali_pp_job_get_frame_registers(struct mali_pp_job *job)
{
	return job->uargs.frame_registers;
}

MALI_STATIC_INLINE u32* mali_pp_job_get_dlbu_registers(struct mali_pp_job *job)
{
	return job->uargs.dlbu_registers;
}

MALI_STATIC_INLINE mali_bool mali_pp_job_is_virtual(struct mali_pp_job *job)
{
#if defined(CONFIG_MALI450)
	return 0 == job->uargs.num_cores;
#else
	return MALI_FALSE;
#endif
}

MALI_STATIC_INLINE u32 mali_pp_job_get_addr_frame(struct mali_pp_job *job, u32 sub_job)
{
	if (mali_pp_job_is_virtual(job)) {
		return MALI_DLBU_VIRT_ADDR;
	} else if (0 == sub_job) {
		return job->uargs.frame_registers[MALI200_REG_ADDR_FRAME / sizeof(u32)];
	} else if (sub_job < _MALI_PP_MAX_SUB_JOBS) {
		return job->uargs.frame_registers_addr_frame[sub_job - 1];
	}

	return 0;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_addr_stack(struct mali_pp_job *job, u32 sub_job)
{
	if (0 == sub_job) {
		return job->uargs.frame_registers[MALI200_REG_ADDR_STACK / sizeof(u32)];
	} else if (sub_job < _MALI_PP_MAX_SUB_JOBS) {
		return job->uargs.frame_registers_addr_stack[sub_job - 1];
	}

	return 0;
}

MALI_STATIC_INLINE u32* mali_pp_job_get_wb0_registers(struct mali_pp_job *job)
{
	return job->uargs.wb0_registers;
}

MALI_STATIC_INLINE u32* mali_pp_job_get_wb1_registers(struct mali_pp_job *job)
{
	return job->uargs.wb1_registers;
}

MALI_STATIC_INLINE u32* mali_pp_job_get_wb2_registers(struct mali_pp_job *job)
{
	return job->uargs.wb2_registers;
}

MALI_STATIC_INLINE void mali_pp_job_disable_wb0(struct mali_pp_job *job)
{
	job->uargs.wb0_registers[MALI200_REG_ADDR_WB_SOURCE_SELECT] = 0;
}

MALI_STATIC_INLINE void mali_pp_job_disable_wb1(struct mali_pp_job *job)
{
	job->uargs.wb1_registers[MALI200_REG_ADDR_WB_SOURCE_SELECT] = 0;
}

MALI_STATIC_INLINE void mali_pp_job_disable_wb2(struct mali_pp_job *job)
{
	job->uargs.wb2_registers[MALI200_REG_ADDR_WB_SOURCE_SELECT] = 0;
}

MALI_STATIC_INLINE mali_bool mali_pp_job_all_writeback_unit_disabled(struct mali_pp_job *job)
{
	MALI_DEBUG_ASSERT_POINTER(job);

	if ( job->uargs.wb0_registers[MALI200_REG_ADDR_WB_SOURCE_SELECT] ||
	     job->uargs.wb1_registers[MALI200_REG_ADDR_WB_SOURCE_SELECT] ||
	     job->uargs.wb2_registers[MALI200_REG_ADDR_WB_SOURCE_SELECT]
	   ) {
		/*                                 */
		return MALI_FALSE;
	}

	/*                                                 */
	return MALI_TRUE;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_fb_lookup_id(struct mali_pp_job *job)
{
	MALI_DEBUG_ASSERT_POINTER(job);

	return MALI_PP_JOB_FB_LOOKUP_LIST_MASK & job->uargs.frame_builder_id;
}

MALI_STATIC_INLINE struct mali_session_data *mali_pp_job_get_session(struct mali_pp_job *job)
{
	return job->session;
}

MALI_STATIC_INLINE mali_bool mali_pp_job_has_unstarted_sub_jobs(struct mali_pp_job *job)
{
	return (job->sub_jobs_started < job->sub_jobs_num) ? MALI_TRUE : MALI_FALSE;
}

/*                                                                                                  
                                               */
MALI_STATIC_INLINE void mali_pp_job_mark_unstarted_failed(struct mali_pp_job *job)
{
	u32 jobs_remaining = job->sub_jobs_num - job->sub_jobs_started;
	job->sub_jobs_started   += jobs_remaining;
	job->sub_jobs_completed += jobs_remaining;
	job->sub_job_errors     += jobs_remaining;
}

MALI_STATIC_INLINE void mali_pp_job_mark_unstarted_success(struct mali_pp_job *job)
{
	u32 jobs_remaining = job->sub_jobs_num - job->sub_jobs_started;
	job->sub_jobs_started   += jobs_remaining;
	job->sub_jobs_completed += jobs_remaining;
}

MALI_STATIC_INLINE mali_bool mali_pp_job_is_complete(struct mali_pp_job *job)
{
	return (job->sub_jobs_num == job->sub_jobs_completed) ? MALI_TRUE : MALI_FALSE;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_first_unstarted_sub_job(struct mali_pp_job *job)
{
	return job->sub_jobs_started;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_sub_job_count(struct mali_pp_job *job)
{
	return job->sub_jobs_num;
}

MALI_STATIC_INLINE mali_bool mali_pp_job_needs_dma_buf_mapping(struct mali_pp_job *job)
{
	MALI_DEBUG_ASSERT(job);

	if (0 != job->num_memory_cookies) {
		return MALI_TRUE;
	}

	return MALI_FALSE;
}

MALI_STATIC_INLINE void mali_pp_job_mark_sub_job_started(struct mali_pp_job *job, u32 sub_job)
{
	MALI_DEBUG_ASSERT_POINTER(job);

	/*                                                                     */
	MALI_DEBUG_ASSERT(job->sub_jobs_started == sub_job);

	job->sub_jobs_started++;
}

MALI_STATIC_INLINE void mali_pp_job_mark_sub_job_completed(struct mali_pp_job *job, mali_bool success)
{
	job->sub_jobs_completed++;
	if ( MALI_FALSE == success ) {
		job->sub_job_errors++;
	}
}

MALI_STATIC_INLINE mali_bool mali_pp_job_was_success(struct mali_pp_job *job)
{
	if ( 0 == job->sub_job_errors ) {
		return MALI_TRUE;
	}
	return MALI_FALSE;
}

MALI_STATIC_INLINE mali_bool mali_pp_job_use_no_notification(struct mali_pp_job *job)
{
	return job->uargs.flags & _MALI_PP_JOB_FLAG_NO_NOTIFICATION ? MALI_TRUE : MALI_FALSE;
}

MALI_STATIC_INLINE u32 mali_pp_job_get_perf_counter_flag(struct mali_pp_job *job)
{
	return job->uargs.perf_counter_flag;
}


MALI_STATIC_INLINE u32 mali_pp_job_get_perf_counter_value0(struct mali_pp_job *job, u32 sub_job)
{
	return job->perf_counter_value0[sub_job];
}

MALI_STATIC_INLINE u32 mali_pp_job_get_perf_counter_value1(struct mali_pp_job *job, u32 sub_job)
{
	return job->perf_counter_value1[sub_job];
}

MALI_STATIC_INLINE void mali_pp_job_set_perf_counter_value0(struct mali_pp_job *job, u32 sub_job, u32 value)
{
	job->perf_counter_value0[sub_job] = value;
}

MALI_STATIC_INLINE void mali_pp_job_set_perf_counter_value1(struct mali_pp_job *job, u32 sub_job, u32 value)
{
	job->perf_counter_value1[sub_job] = value;
}

MALI_STATIC_INLINE _mali_osk_errcode_t mali_pp_job_check(struct mali_pp_job *job)
{
	if (mali_pp_job_is_virtual(job) && job->sub_jobs_num != 1) {
		return _MALI_OSK_ERR_FAULT;
	}
	return _MALI_OSK_ERR_OK;
}

/* 
                                                                     
  
                          
                            
                                                                                        
 */
MALI_STATIC_INLINE mali_bool mali_pp_job_should_start_after(struct mali_pp_job *first, struct mali_pp_job *second)
{
	MALI_DEBUG_ASSERT_POINTER(first);
	MALI_DEBUG_ASSERT_POINTER(second);

	/*                                                                            */
	if (0 < second->sub_jobs_started) {
		return MALI_TRUE;
	}

	/*                                                                                          
                                    */
	if ((mali_pp_job_get_id(first) - mali_pp_job_get_id(second)) < MALI_SCHEDULER_JOB_ID_SPAN) {
		return MALI_TRUE;
	}

	/*                                               */
	return MALI_FALSE;
}

/* 
                                                                                           
  
                           
                                                                                                         
 */
MALI_STATIC_INLINE mali_bool mali_pp_job_is_large_and_unstarted(struct mali_pp_job *job)
{
	MALI_DEBUG_ASSERT_POINTER(job);
	MALI_DEBUG_ASSERT(!mali_pp_job_is_virtual(job));

	return (0 == job->sub_jobs_started && 2 < job->sub_jobs_num);
}

/* 
                                 
  
                     
                                                   
 */
MALI_STATIC_INLINE struct mali_timeline_tracker *mali_pp_job_get_tracker(struct mali_pp_job *job)
{
	MALI_DEBUG_ASSERT_POINTER(job);
	return &(job->tracker);
}

#endif /*                   */
