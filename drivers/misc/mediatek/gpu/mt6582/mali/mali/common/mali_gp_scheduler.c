/*
 * Copyright (C) 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "mali_gp_scheduler.h"
#include "mali_kernel_common.h"
#include "mali_osk.h"
#include "mali_osk_list.h"
#include "mali_scheduler.h"
#include "mali_gp.h"
#include "mali_gp_job.h"
#include "mali_group.h"
#include "mali_timeline.h"
#include "mali_osk_profiling.h"
#include "mali_kernel_utilization.h"
#if defined(CONFIG_GPU_TRACEPOINTS) && defined(CONFIG_TRACEPOINTS)
#include <linux/sched.h>
#include <trace/events/gpu.h>
#endif

enum mali_gp_slot_state {
	MALI_GP_SLOT_STATE_IDLE,
	MALI_GP_SLOT_STATE_WORKING,
	MALI_GP_SLOT_STATE_DISABLED,
};

/*                                                             */
struct mali_gp_slot {
	struct mali_group *group;
	/*
                                                                  
                                                                                                   
  */
	enum mali_gp_slot_state state;
	u32 returned_cookie;
};

static u32 gp_version = 0;
static _MALI_OSK_LIST_HEAD_STATIC_INIT(job_queue);      /*                           */
static _MALI_OSK_LIST_HEAD_STATIC_INIT(job_queue_high); /*                                         */
static struct mali_gp_slot slot;

/*                                                  */
static _mali_osk_wait_queue_t *gp_scheduler_working_wait_queue = NULL;
static u32 pause_count = 0;

static mali_bool mali_gp_scheduler_is_suspended(void *data);
static void mali_gp_scheduler_job_queued(void);
static void mali_gp_scheduler_job_completed(void);

#if defined(MALI_UPPER_HALF_SCHEDULING)
static _mali_osk_spinlock_irq_t *gp_scheduler_lock = NULL;
#else
static _mali_osk_spinlock_t *gp_scheduler_lock = NULL;
#endif /*                                     */

_mali_osk_errcode_t mali_gp_scheduler_initialize(void)
{
	u32 num_groups;
	u32 i;
	_mali_osk_errcode_t ret = _MALI_OSK_ERR_OK;

#if defined(MALI_UPPER_HALF_SCHEDULING)
	gp_scheduler_lock = _mali_osk_spinlock_irq_init(_MALI_OSK_LOCKFLAG_ORDERED, _MALI_OSK_LOCK_ORDER_SCHEDULER);
#else
	gp_scheduler_lock = _mali_osk_spinlock_init(_MALI_OSK_LOCKFLAG_ORDERED, _MALI_OSK_LOCK_ORDER_SCHEDULER);
#endif /*                                     */
	if (NULL == gp_scheduler_lock) {
		ret = _MALI_OSK_ERR_NOMEM;
		goto cleanup;
	}

	gp_scheduler_working_wait_queue = _mali_osk_wait_queue_init();
	if (NULL == gp_scheduler_working_wait_queue) {
		ret = _MALI_OSK_ERR_NOMEM;
		goto cleanup;
	}

	/*                                 */
	num_groups = mali_group_get_glob_num_groups();
	for (i = 0; i < num_groups; i++) {
		struct mali_group *group = mali_group_get_glob_group(i);
		MALI_DEBUG_ASSERT(NULL != group);
		if (NULL != group) {
			struct mali_gp_core *gp_core = mali_group_get_gp_core(group);
			if (NULL != gp_core) {
				if (0 == gp_version) {
					/*                     */
					gp_version = mali_gp_core_get_version(gp_core);
				}
				slot.group = group;
				slot.state = MALI_GP_SLOT_STATE_IDLE;
				break; /*                                                    */
			}
		} else {
			ret = _MALI_OSK_ERR_ITEM_NOT_FOUND;
			goto cleanup;
		}
	}

	return _MALI_OSK_ERR_OK;

cleanup:
	if (NULL != gp_scheduler_working_wait_queue) {
		_mali_osk_wait_queue_term(gp_scheduler_working_wait_queue);
		gp_scheduler_working_wait_queue = NULL;
	}

	if (NULL != gp_scheduler_lock) {
#if defined(MALI_UPPER_HALF_SCHEDULING)
		_mali_osk_spinlock_irq_term(gp_scheduler_lock);
#else
		_mali_osk_spinlock_term(gp_scheduler_lock);
#endif /*                                     */
		gp_scheduler_lock = NULL;
	}

	return ret;
}

void mali_gp_scheduler_terminate(void)
{
	MALI_DEBUG_ASSERT(   MALI_GP_SLOT_STATE_IDLE     == slot.state
	                     || MALI_GP_SLOT_STATE_DISABLED == slot.state);
	MALI_DEBUG_ASSERT_POINTER(slot.group);
	mali_group_delete(slot.group);

	_mali_osk_wait_queue_term(gp_scheduler_working_wait_queue);

#if defined(MALI_UPPER_HALF_SCHEDULING)
	_mali_osk_spinlock_irq_term(gp_scheduler_lock);
#else
	_mali_osk_spinlock_term(gp_scheduler_lock);
#endif /*                                     */
}

MALI_STATIC_INLINE void mali_gp_scheduler_lock(void)
{
#if defined(MALI_UPPER_HALF_SCHEDULING)
	_mali_osk_spinlock_irq_lock(gp_scheduler_lock);
#else
	_mali_osk_spinlock_lock(gp_scheduler_lock);
#endif /*                                     */
	MALI_DEBUG_PRINT(5, ("Mali GP scheduler: GP scheduler lock taken\n"));
}

MALI_STATIC_INLINE void mali_gp_scheduler_unlock(void)
{
	MALI_DEBUG_PRINT(5, ("Mali GP scheduler: Releasing GP scheduler lock\n"));
#if defined(MALI_UPPER_HALF_SCHEDULING)
	_mali_osk_spinlock_irq_unlock(gp_scheduler_lock);
#else
	_mali_osk_spinlock_unlock(gp_scheduler_lock);
#endif /*                                     */
}

#if defined(DEBUG)
#define MALI_ASSERT_GP_SCHEDULER_LOCKED() MALI_DEBUG_ASSERT_LOCK_HELD(gp_scheduler_lock)
#else
#define MALI_ASSERT_GP_SCHEDULER_LOCKED() do {} while (0)
#endif /*                */

/*                                                                                              
            */
static void mali_gp_scheduler_schedule_internal_and_unlock(void)
{
	struct mali_gp_job *job = NULL;

	MALI_DEBUG_ASSERT_LOCK_HELD(slot.group->lock);
	MALI_DEBUG_ASSERT_LOCK_HELD(gp_scheduler_lock);

	if (0 < pause_count || MALI_GP_SLOT_STATE_IDLE != slot.state ||
	    (_mali_osk_list_empty(&job_queue) && _mali_osk_list_empty(&job_queue_high))) {
		mali_gp_scheduler_unlock();
		mali_group_unlock(slot.group);
		MALI_DEBUG_PRINT(4, ("Mali GP scheduler: Nothing to schedule (paused=%u, idle slots=%u)\n",
		                     pause_count, MALI_GP_SLOT_STATE_IDLE == slot.state ? 1 : 0));
#if defined(CONFIG_GPU_TRACEPOINTS) && defined(CONFIG_TRACEPOINTS)
		trace_gpu_sched_switch(mali_gp_get_hw_core_desc(group->gp_core), sched_clock(), 0, 0, 0);
#endif
		return; /*                             */
	}

	/*                       */
	if (!_mali_osk_list_empty(&job_queue_high)) {
		job = _MALI_OSK_LIST_ENTRY(job_queue_high.next, struct mali_gp_job, list);
	} else {
		MALI_DEBUG_ASSERT(!_mali_osk_list_empty(&job_queue));
		job = _MALI_OSK_LIST_ENTRY(job_queue.next, struct mali_gp_job, list);
	}

	MALI_DEBUG_ASSERT_POINTER(job);

	/*                           */
	_mali_osk_list_del(&job->list);

	/*                   */
	slot.state = MALI_GP_SLOT_STATE_WORKING;

	mali_gp_scheduler_unlock();

	MALI_DEBUG_PRINT(3, ("Mali GP scheduler: Starting job %u (0x%08X)\n", mali_gp_job_get_id(job), job));

	mali_group_start_gp_job(slot.group, job);
	mali_group_unlock(slot.group);
}

void mali_gp_scheduler_schedule(void)
{
	mali_group_lock(slot.group);
	mali_gp_scheduler_lock();

	mali_gp_scheduler_schedule_internal_and_unlock();
}

static void mali_gp_scheduler_return_job_to_user(struct mali_gp_job *job, mali_bool success)
{
	_mali_uk_gp_job_finished_s *jobres = job->finished_notification->result_buffer;
	_mali_osk_memset(jobres, 0, sizeof(_mali_uk_gp_job_finished_s)); /*                                                                   */
	jobres->user_job_ptr = mali_gp_job_get_user_id(job);
	if (MALI_TRUE == success) {
		jobres->status = _MALI_UK_JOB_STATUS_END_SUCCESS;
	} else {
		jobres->status = _MALI_UK_JOB_STATUS_END_UNKNOWN_ERR;
	}

	jobres->heap_current_addr = mali_gp_job_get_current_heap_addr(job);
	jobres->perf_counter0 = mali_gp_job_get_perf_counter_value0(job);
	jobres->perf_counter1 = mali_gp_job_get_perf_counter_value1(job);

	mali_session_send_notification(mali_gp_job_get_session(job), job->finished_notification);
	job->finished_notification = NULL;

	mali_gp_job_delete(job);
	mali_gp_scheduler_job_completed();
}

/*                                                                                     */
void mali_gp_scheduler_job_done(struct mali_group *group, struct mali_gp_job *job, mali_bool success)
{
	mali_scheduler_mask schedule_mask = MALI_SCHEDULER_MASK_EMPTY;

	MALI_DEBUG_ASSERT_POINTER(group);
	MALI_DEBUG_ASSERT_POINTER(job);

	MALI_DEBUG_ASSERT_LOCK_HELD(group->lock);
	MALI_DEBUG_ASSERT(slot.group == group);

	MALI_DEBUG_PRINT(3, ("Mali GP scheduler: Job %u (0x%08X) completed (%s)\n", mali_gp_job_get_id(job), job, success ? "success" : "failure"));

	/*                  */
	schedule_mask |= mali_timeline_tracker_release(&job->tracker);

	/*                */
	schedule_mask |= mali_gp_job_signal_pp_tracker(job, success);

	mali_gp_scheduler_lock();

	/*                         */
	slot.state = MALI_GP_SLOT_STATE_IDLE;

	/*                                                                    */
	if (pause_count > 0) {
		_mali_osk_wait_queue_wake_up(gp_scheduler_working_wait_queue);
	}

	/*                                            */
	mali_gp_scheduler_schedule_internal_and_unlock();

	/*                                                 */
	schedule_mask &= ~MALI_SCHEDULER_MASK_GP;

	if (MALI_SCHEDULER_MASK_EMPTY != schedule_mask) {
		/*                                                                  */
		mali_scheduler_schedule_from_mask(schedule_mask, MALI_FALSE);
	}

	/*                                                                 */
	mali_gp_scheduler_return_job_to_user(job, success);
}

void mali_gp_scheduler_oom(struct mali_group *group, struct mali_gp_job *job)
{
	_mali_uk_gp_job_suspended_s * jobres;
	_mali_osk_notification_t * notification;

	mali_gp_scheduler_lock();

	notification = job->oom_notification;
	job->oom_notification = NULL;
	slot.returned_cookie = mali_gp_job_get_id(job);

	jobres = (_mali_uk_gp_job_suspended_s *)notification->result_buffer;
	jobres->user_job_ptr = mali_gp_job_get_user_id(job);
	jobres->cookie = mali_gp_job_get_id(job);

	mali_gp_scheduler_unlock();

	mali_session_send_notification(mali_gp_job_get_session(job), notification);

	/*
                                                                                  
                                                                
                                            
 */
}

void mali_gp_scheduler_suspend(void)
{
	mali_gp_scheduler_lock();
	pause_count++; /*                                                                  */
	mali_gp_scheduler_unlock();

	_mali_osk_wait_queue_wait_event(gp_scheduler_working_wait_queue, mali_gp_scheduler_is_suspended, NULL);
}

void mali_gp_scheduler_resume(void)
{
	mali_gp_scheduler_lock();
	pause_count--; /*                                                                   */
	mali_gp_scheduler_unlock();
	if (0 == pause_count) {
		mali_gp_scheduler_schedule();
	}
}

mali_timeline_point mali_gp_scheduler_submit_job(struct mali_session_data *session, struct mali_gp_job *job)
{
	mali_timeline_point point;

	MALI_DEBUG_ASSERT_POINTER(session);
	MALI_DEBUG_ASSERT_POINTER(job);

	mali_gp_scheduler_job_queued();

	/*                             */
	point = mali_timeline_system_add_tracker(session->timeline_system, &job->tracker, MALI_TIMELINE_GP);

	return point;
}

_mali_osk_errcode_t _mali_ukk_gp_start_job(void *ctx, _mali_uk_gp_start_job_s *uargs)
{
	struct mali_session_data *session;
	struct mali_gp_job *job;
	mali_timeline_point point;
	u32 __user *timeline_point_ptr = NULL;

	MALI_DEBUG_ASSERT_POINTER(uargs);
	MALI_DEBUG_ASSERT_POINTER(ctx);

	session = (struct mali_session_data*)ctx;

	job = mali_gp_job_create(session, uargs, mali_scheduler_get_new_id(), NULL);
	if (NULL == job) {
		MALI_PRINT_ERROR(("Failed to create GP job.\n"));
		return _MALI_OSK_ERR_NOMEM;
	}

	timeline_point_ptr = (u32 __user *) job->uargs.timeline_point_ptr;

	point = mali_gp_scheduler_submit_job(session, job);

	if (0 != _mali_osk_put_user(((u32) point), timeline_point_ptr)) {
		/*                                                                      */
		return _MALI_OSK_ERR_ITEM_NOT_FOUND;
	}

	return _MALI_OSK_ERR_OK;
}

_mali_osk_errcode_t _mali_ukk_get_gp_number_of_cores(_mali_uk_get_gp_number_of_cores_s *args)
{
	MALI_DEBUG_ASSERT_POINTER(args);
	MALI_CHECK_NON_NULL(args->ctx, _MALI_OSK_ERR_INVALID_ARGS);
	args->number_of_cores = 1;
	return _MALI_OSK_ERR_OK;
}

_mali_osk_errcode_t _mali_ukk_get_gp_core_version(_mali_uk_get_gp_core_version_s *args)
{
	MALI_DEBUG_ASSERT_POINTER(args);
	MALI_CHECK_NON_NULL(args->ctx, _MALI_OSK_ERR_INVALID_ARGS);
	args->version = gp_version;
	return _MALI_OSK_ERR_OK;
}

_mali_osk_errcode_t _mali_ukk_gp_suspend_response(_mali_uk_gp_suspend_response_s *args)
{
	struct mali_session_data *session;
	struct mali_gp_job *resumed_job;
	_mali_osk_notification_t *new_notification = 0;

	MALI_DEBUG_ASSERT_POINTER(args);

	if (NULL == args->ctx) {
		return _MALI_OSK_ERR_INVALID_ARGS;
	}

	session = (struct mali_session_data*)args->ctx;
	if (NULL == session) {
		return _MALI_OSK_ERR_FAULT;
	}

	if (_MALIGP_JOB_RESUME_WITH_NEW_HEAP == args->code) {
		new_notification = _mali_osk_notification_create(_MALI_NOTIFICATION_GP_STALLED, sizeof(_mali_uk_gp_job_suspended_s));

		if (NULL == new_notification) {
			MALI_PRINT_ERROR(("Mali GP scheduler: Failed to allocate notification object. Will abort GP job.\n"));
			mali_group_lock(slot.group);
			mali_group_abort_gp_job(slot.group, args->cookie);
			mali_group_unlock(slot.group);
			return _MALI_OSK_ERR_FAULT;
		}
	}

	mali_group_lock(slot.group);

	if (_MALIGP_JOB_RESUME_WITH_NEW_HEAP == args->code) {
		MALI_DEBUG_PRINT(3, ("Mali GP scheduler: Resuming job %u with new heap; 0x%08X - 0x%08X\n", args->cookie, args->arguments[0], args->arguments[1]));

		resumed_job = mali_group_resume_gp_with_new_heap(slot.group, args->cookie, args->arguments[0], args->arguments[1]);
		if (NULL != resumed_job) {
			resumed_job->oom_notification = new_notification;
			mali_group_unlock(slot.group);
			return _MALI_OSK_ERR_OK;
		} else {
			mali_group_unlock(slot.group);
			_mali_osk_notification_delete(new_notification);
			return _MALI_OSK_ERR_FAULT;
		}
	}

	MALI_DEBUG_PRINT(2, ("Mali GP scheduler: Aborting job %u, no new heap provided\n", args->cookie));
	mali_group_abort_gp_job(slot.group, args->cookie);
	mali_group_unlock(slot.group);
	return _MALI_OSK_ERR_OK;
}

void mali_gp_scheduler_abort_session(struct mali_session_data *session)
{
	struct mali_gp_job *job, *tmp;
	_MALI_OSK_LIST_HEAD_STATIC_INIT(removed_jobs);

	MALI_DEBUG_ASSERT_POINTER(session);
	MALI_DEBUG_ASSERT(session->is_aborting);

	MALI_DEBUG_PRINT(3, ("Mali GP scheduler: Aborting all jobs from session 0x%08X.\n", session));

	mali_gp_scheduler_lock();

	/*                                          */
	_MALI_OSK_LIST_FOREACHENTRY(job, tmp, &job_queue, struct mali_gp_job, list) {
		if (job->session == session) {
			MALI_DEBUG_PRINT(3, ("Mali GP scheduler: Removing job %u (0x%08X) from queue.\n", mali_gp_job_get_id(job), job));
			_mali_osk_list_move(&job->list, &removed_jobs);
		}
	}

	/*                                                        */
	_MALI_OSK_LIST_FOREACHENTRY(job, tmp, &job_queue_high, struct mali_gp_job, list) {
		if (job->session == session) {
			MALI_DEBUG_PRINT(3, ("Mali GP scheduler: Removing job %u (0x%08X) from queue.\n", mali_gp_job_get_id(job), job));
			_mali_osk_list_move(&job->list, &removed_jobs);
		}
	}

	mali_gp_scheduler_unlock();

	/*                                                              */
	_MALI_OSK_LIST_FOREACHENTRY(job, tmp, &removed_jobs, struct mali_gp_job, list) {
		mali_timeline_tracker_release(&job->tracker);
		mali_gp_job_signal_pp_tracker(job, MALI_FALSE);
		mali_gp_job_delete(job);
		mali_gp_scheduler_job_completed();
	}

	/*                                          */
	mali_group_abort_session(slot.group, session);
}

static mali_bool mali_gp_scheduler_is_suspended(void *data)
{
	mali_bool ret;

	/*                                              */
	MALI_IGNORE(data);

	mali_gp_scheduler_lock();
	ret = pause_count > 0 && (slot.state == MALI_GP_SLOT_STATE_IDLE || slot.state == MALI_GP_SLOT_STATE_DISABLED);
	mali_gp_scheduler_unlock();

	return ret;
}


#if MALI_STATE_TRACKING
u32 mali_gp_scheduler_dump_state(char *buf, u32 size)
{
	int n = 0;

	n += _mali_osk_snprintf(buf + n, size - n, "GP\n");
	n += _mali_osk_snprintf(buf + n, size - n, "\tQueue is %s\n", _mali_osk_list_empty(&job_queue) ? "empty" : "not empty");
	n += _mali_osk_snprintf(buf + n, size - n, "\tHigh priority queue is %s\n", _mali_osk_list_empty(&job_queue_high) ? "empty" : "not empty");

	n += mali_group_dump_state(slot.group, buf + n, size - n);
	n += _mali_osk_snprintf(buf + n, size - n, "\n");

	return n;
}
#endif

void mali_gp_scheduler_reset_all_groups(void)
{
	if (NULL != slot.group) {
		mali_group_lock(slot.group);
		mali_group_reset(slot.group);
		mali_group_unlock(slot.group);
	}
}

void mali_gp_scheduler_zap_all_active(struct mali_session_data *session)
{
	if (NULL != slot.group) {
		mali_group_zap_session(slot.group, session);
	}
}

void mali_gp_scheduler_enable_group(struct mali_group *group)
{
	MALI_DEBUG_ASSERT_POINTER(group);
	MALI_DEBUG_ASSERT(slot.group == group);
	MALI_DEBUG_PRINT(2, ("Mali GP scheduler: enabling gp group %p\n", group));

	mali_group_lock(group);

	if (MALI_GROUP_STATE_DISABLED != group->state) {
		mali_group_unlock(group);
		MALI_DEBUG_PRINT(2, ("Mali GP scheduler: gp group %p already enabled\n", group));
		return;
	}

	mali_gp_scheduler_lock();

	MALI_DEBUG_ASSERT(MALI_GROUP_STATE_DISABLED == group->state);
	MALI_DEBUG_ASSERT(MALI_GP_SLOT_STATE_DISABLED == slot.state);
	slot.state = MALI_GP_SLOT_STATE_IDLE;
	group->state = MALI_GROUP_STATE_IDLE;

	mali_group_power_on_group(group);
	mali_group_reset(group);

	/*                                                                               */
	mali_gp_scheduler_schedule_internal_and_unlock();
}

void mali_gp_scheduler_disable_group(struct mali_group *group)
{
	MALI_DEBUG_ASSERT_POINTER(group);
	MALI_DEBUG_ASSERT(slot.group == group);
	MALI_DEBUG_PRINT(2, ("Mali GP scheduler: disabling gp group %p\n", group));

	mali_gp_scheduler_suspend();
	mali_group_lock(group);
	mali_gp_scheduler_lock();

	MALI_DEBUG_ASSERT(   MALI_GROUP_STATE_IDLE     == group->state
	                     || MALI_GROUP_STATE_DISABLED == group->state);

	if (MALI_GROUP_STATE_DISABLED == group->state) {
		MALI_DEBUG_ASSERT(MALI_GP_SLOT_STATE_DISABLED == slot.state);
		MALI_DEBUG_PRINT(2, ("Mali GP scheduler: gp group %p already disabled\n", group));
	} else {
		MALI_DEBUG_ASSERT(MALI_GP_SLOT_STATE_IDLE == slot.state);
		slot.state = MALI_GP_SLOT_STATE_DISABLED;
		group->state = MALI_GROUP_STATE_DISABLED;

		mali_group_power_off_group(group, MALI_TRUE);
	}

	mali_gp_scheduler_unlock();
	mali_group_unlock(group);
	mali_gp_scheduler_resume();
}

static mali_scheduler_mask mali_gp_scheduler_queue_job(struct mali_gp_job *job)
{
	_mali_osk_list_t *queue = NULL;
	mali_scheduler_mask schedule_mask = MALI_SCHEDULER_MASK_EMPTY;
	struct mali_gp_job *iter, *tmp;

	MALI_DEBUG_ASSERT_POINTER(job);
	MALI_DEBUG_ASSERT_POINTER(job->session);

	MALI_DEBUG_ASSERT_LOCK_HELD(gp_scheduler_lock);

	_mali_osk_profiling_add_event(MALI_PROFILING_EVENT_TYPE_SINGLE | MALI_PROFILING_EVENT_CHANNEL_SOFTWARE | MALI_PROFILING_EVENT_REASON_SINGLE_SW_GP_ENQUEUE, job->pid, job->tid, job->uargs.frame_builder_id, job->uargs.flush_id, 0);

	job->cache_order = mali_scheduler_get_new_cache_order();

	/*                                                   */
	if (job->session->use_high_priority_job_queue) {
		queue = &job_queue_high;
	} else {
		queue = &job_queue;
	}

	/*                                                   */
	_MALI_OSK_LIST_FOREACHENTRY_REVERSE(iter, tmp, queue, struct mali_gp_job, list) {
		if (mali_gp_job_is_after(job, iter)) {
			break;
		}
	}

	/*                   */
	_mali_osk_list_add(&job->list, &iter->list);

	/*                                              */
	if (MALI_GP_SLOT_STATE_IDLE == slot.state) {
		schedule_mask |= MALI_SCHEDULER_MASK_GP;
	}

#if defined(CONFIG_GPU_TRACEPOINTS) && defined(CONFIG_TRACEPOINTS)
	trace_gpu_job_enqueue(mali_gp_job_get_tid(job), mali_gp_job_get_id(job), "GP");
#endif

	MALI_DEBUG_PRINT(3, ("Mali GP scheduler: Job %u (0x%08X) queued\n", mali_gp_job_get_id(job), job));

	return schedule_mask;
}

mali_scheduler_mask mali_gp_scheduler_activate_job(struct mali_gp_job *job)
{
	mali_scheduler_mask schedule_mask = MALI_SCHEDULER_MASK_EMPTY;

	MALI_DEBUG_ASSERT_POINTER(job);
	MALI_DEBUG_ASSERT_POINTER(job->session);

	MALI_DEBUG_PRINT(4, ("Mali GP scheduler: Timeline activation for job %u (0x%08X).\n", mali_gp_job_get_id(job), job));

	mali_gp_scheduler_lock();

	if (unlikely(job->session->is_aborting)) {
		/*                                                                           */
		MALI_DEBUG_ASSERT_LOCK_HELD(gp_scheduler_lock);

		MALI_DEBUG_PRINT(3, ("Mali GP scheduler: Job %u (0x%08X) activated while session is aborting.\n", mali_gp_job_get_id(job), job));

		/*                                     */
		MALI_DEBUG_ASSERT(_mali_osk_list_empty(&job->list));

		mali_gp_scheduler_unlock();

		/*                                 */
		mali_timeline_tracker_release(&job->tracker);
		mali_gp_job_signal_pp_tracker(job, MALI_FALSE);
		mali_gp_job_delete(job);
		mali_gp_scheduler_job_completed();

		/*                                                     */
		return MALI_SCHEDULER_MASK_EMPTY;
	}

	/*                                   */
	schedule_mask = mali_gp_scheduler_queue_job(job);

	mali_gp_scheduler_unlock();

	return schedule_mask;
}

static void mali_gp_scheduler_job_queued(void)
{
	/*                                                                   */
	_mali_osk_pm_dev_ref_add();

	if (mali_utilization_enabled()) {
		/*
                                                                                       
                                                                                
                                                                       
   */
		mali_utilization_gp_start();
	}
}

static void mali_gp_scheduler_job_completed(void)
{
	/*                                                                                */
	_mali_osk_pm_dev_ref_dec();

	if (mali_utilization_enabled()) {
		mali_utilization_gp_end();
	}
}
