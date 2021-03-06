/*
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "mali_soft_job.h"
#include "mali_osk.h"
#include "mali_osk_mali.h"
#include "mali_timeline.h"
#include "mali_session.h"
#include "mali_kernel_common.h"
#include "mali_uk_types.h"
#include "mali_scheduler.h"

MALI_STATIC_INLINE void mali_soft_job_system_lock(struct mali_soft_job_system *system)
{
	MALI_DEBUG_ASSERT_POINTER(system);
	_mali_osk_spinlock_irq_lock(system->lock);
	MALI_DEBUG_PRINT(5, ("Mali Soft Job: soft system %p lock taken\n", system));
	MALI_DEBUG_ASSERT(0 == system->lock_owner);
	MALI_DEBUG_CODE(system->lock_owner = _mali_osk_get_tid());
}

MALI_STATIC_INLINE void mali_soft_job_system_unlock(struct mali_soft_job_system *system)
{
	MALI_DEBUG_ASSERT_POINTER(system);
	MALI_DEBUG_PRINT(5, ("Mali Soft Job: releasing soft system %p lock\n", system));
	MALI_DEBUG_ASSERT(_mali_osk_get_tid() == system->lock_owner);
	MALI_DEBUG_CODE(system->lock_owner = 0);
	_mali_osk_spinlock_irq_unlock(system->lock);
}

#if defined(DEBUG)
MALI_STATIC_INLINE void mali_soft_job_system_assert_locked(struct mali_soft_job_system *system)
{
	MALI_DEBUG_ASSERT_POINTER(system);
	MALI_DEBUG_ASSERT(_mali_osk_get_tid() == system->lock_owner);
}
#define MALI_ASSERT_SOFT_JOB_SYSTEM_LOCKED(system) mali_soft_job_system_assert_locked(system)
#else
#define MALI_ASSERT_SOFT_JOB_SYSTEM_LOCKED(system)
#endif /*                */

struct mali_soft_job_system *mali_soft_job_system_create(struct mali_session_data *session)
{
	u32 i;
	struct mali_soft_job_system *system;
	struct mali_soft_job *job;

	MALI_DEBUG_ASSERT_POINTER(session);

	system = (struct mali_soft_job_system *) _mali_osk_calloc(1, sizeof(struct mali_soft_job_system));
	if (NULL == system) {
		return NULL;
	}

	system->session = session;

	system->lock = _mali_osk_spinlock_irq_init(_MALI_OSK_LOCKFLAG_ORDERED, _MALI_OSK_LOCK_ORDER_SCHEDULER);
	if (NULL == system->lock) {
		mali_soft_job_system_destroy(system);
		return NULL;
	}
	system->lock_owner = 0;

	_MALI_OSK_INIT_LIST_HEAD(&(system->jobs_free));
	_MALI_OSK_INIT_LIST_HEAD(&(system->jobs_used));

	for (i = 0; i < MALI_MAX_NUM_SOFT_JOBS; ++i) {
		job = &(system->jobs[i]);
		_mali_osk_list_add(&(job->system_list), &(system->jobs_free));
		job->system = system;
		job->state = MALI_SOFT_JOB_STATE_FREE;
		job->id = i;
	}

	return system;
}

void mali_soft_job_system_destroy(struct mali_soft_job_system *system)
{
	MALI_DEBUG_ASSERT_POINTER(system);

	/*                                        */
	MALI_DEBUG_CODE( {
		u32 i;
		struct mali_soft_job *job;

		for (i = 0; i < MALI_MAX_NUM_SOFT_JOBS; ++i)
		{
			job = &(system->jobs[i]);
			MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_FREE == job->state);
		}
	});

	if (NULL != system) {
		if (NULL != system->lock) {
			_mali_osk_spinlock_irq_term(system->lock);
		}
		_mali_osk_free(system);
	}
}

static struct mali_soft_job *mali_soft_job_system_alloc_job(struct mali_soft_job_system *system)
{
	struct mali_soft_job *job;

	MALI_DEBUG_ASSERT_POINTER(system);
	MALI_ASSERT_SOFT_JOB_SYSTEM_LOCKED(system);

	if (_mali_osk_list_empty(&(system->jobs_free))) {
		/*                    */
		return NULL;
	}

	/*                                              */
	job = _MALI_OSK_LIST_ENTRY(system->jobs_free.next, struct mali_soft_job, system_list);
	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_FREE == job->state);

	_mali_osk_list_move(&(job->system_list), &(system->jobs_used));
	job->state = MALI_SOFT_JOB_STATE_ALLOCATED;

	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_INVALID_ID != job->id);
	MALI_DEBUG_ASSERT(system == job->system);

	return job;
}

static void mali_soft_job_system_free_job(struct mali_soft_job_system *system, struct mali_soft_job *job)
{
	MALI_DEBUG_ASSERT_POINTER(job);
	MALI_DEBUG_ASSERT_POINTER(system);

	mali_soft_job_system_lock(job->system);

	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_FREE != job->state);
	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_INVALID_ID != job->id);
	MALI_DEBUG_ASSERT(system == job->system);

	job->state = MALI_SOFT_JOB_STATE_FREE;
	_mali_osk_list_move(&(job->system_list), &(system->jobs_free));

	mali_soft_job_system_unlock(job->system);
}

MALI_STATIC_INLINE struct mali_soft_job *mali_soft_job_system_lookup_job(struct mali_soft_job_system *system, u32 job_id)
{
	MALI_DEBUG_ASSERT_POINTER(system);
	MALI_ASSERT_SOFT_JOB_SYSTEM_LOCKED(system);

	if (job_id < MALI_MAX_NUM_SOFT_JOBS) {
		return &system->jobs[job_id];
	}

	return NULL;
}

void mali_soft_job_destroy(struct mali_soft_job *job)
{
	MALI_DEBUG_ASSERT_POINTER(job);
	MALI_DEBUG_ASSERT_POINTER(job->system);

	MALI_DEBUG_PRINT(4, ("Mali Soft Job: destroying soft job %u (0x%08X)\n", job->id, job));

	if (NULL != job) {
		if (0 < _mali_osk_atomic_dec_return(&job->refcount)) return;

		_mali_osk_atomic_term(&job->refcount);

		if (NULL != job->activated_notification) {
			_mali_osk_notification_delete(job->activated_notification);
			job->activated_notification = NULL;
		}

		mali_soft_job_system_free_job(job->system, job);
	}
}

struct mali_soft_job *mali_soft_job_create(struct mali_soft_job_system *system, mali_soft_job_type type, u32 user_job)
{
	struct mali_soft_job *job;
	_mali_osk_notification_t *notification = NULL;

	MALI_DEBUG_ASSERT_POINTER(system);
	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_TYPE_USER_SIGNALED >= type);

	if (MALI_SOFT_JOB_TYPE_USER_SIGNALED == type) {
		notification = _mali_osk_notification_create(_MALI_NOTIFICATION_SOFT_ACTIVATED, sizeof(_mali_uk_soft_job_activated_s));
		if (unlikely(NULL == notification)) {
			MALI_PRINT_ERROR(("Mali Soft Job: failed to allocate notification"));
			return NULL;
		}
	}

	mali_soft_job_system_lock(system);

	job = mali_soft_job_system_alloc_job(system);
	if (NULL == job) {
		mali_soft_job_system_unlock(system);
		MALI_PRINT_ERROR(("Mali Soft Job: failed to allocate job"));
		_mali_osk_notification_delete(notification);
		return NULL;
	}

	job->type = type;
	job->user_job = user_job;
	job->activated = MALI_FALSE;

	if (MALI_SOFT_JOB_TYPE_USER_SIGNALED == type) {
		job->activated_notification = notification;
	}

	_mali_osk_atomic_init(&job->refcount, 1);

	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_ALLOCATED == job->state);
	MALI_DEBUG_ASSERT(system == job->system);
	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_INVALID_ID != job->id);

	mali_soft_job_system_unlock(system);

	return job;
}

mali_timeline_point mali_soft_job_start(struct mali_soft_job *job, struct mali_timeline_fence *fence)
{
	mali_timeline_point point;
	struct mali_soft_job_system *system;

	MALI_DEBUG_ASSERT_POINTER(job);
	MALI_DEBUG_ASSERT_POINTER(fence);

	MALI_DEBUG_ASSERT_POINTER(job->system);
	system = job->system;

	MALI_DEBUG_ASSERT_POINTER(system->session);
	MALI_DEBUG_ASSERT_POINTER(system->session->timeline_system);

	mali_soft_job_system_lock(system);

	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_ALLOCATED == job->state);
	job->state = MALI_SOFT_JOB_STATE_STARTED;

	mali_soft_job_system_unlock(system);

	MALI_DEBUG_PRINT(4, ("Mali Soft Job: starting soft job %u (0x%08X)\n", job->id, job));

	mali_timeline_tracker_init(&job->tracker, MALI_TIMELINE_TRACKER_SOFT, fence, job);
	point = mali_timeline_system_add_tracker(system->session->timeline_system, &job->tracker, MALI_TIMELINE_SOFT);

	return point;
}

static mali_bool mali_soft_job_is_activated(void *data)
{
	struct mali_soft_job *job;

	job = (struct mali_soft_job *) data;
	MALI_DEBUG_ASSERT_POINTER(job);

	return job->activated;
}

_mali_osk_errcode_t mali_soft_job_system_signal_job(struct mali_soft_job_system *system, u32 job_id)
{
	struct mali_soft_job *job;
	struct mali_timeline_system *timeline_system;
	mali_scheduler_mask schedule_mask;

	MALI_DEBUG_ASSERT_POINTER(system);

	mali_soft_job_system_lock(system);

	job = mali_soft_job_system_lookup_job(system, job_id);

	if (NULL == job || !(MALI_SOFT_JOB_STATE_STARTED == job->state || MALI_SOFT_JOB_STATE_TIMED_OUT == job->state)) {
		mali_soft_job_system_unlock(system);
		MALI_PRINT_ERROR(("Mali Soft Job: invalid soft job id %u", job_id));
		return _MALI_OSK_ERR_ITEM_NOT_FOUND;
	}

	if (MALI_SOFT_JOB_STATE_TIMED_OUT == job->state) {
		job->state = MALI_SOFT_JOB_STATE_SIGNALED;
		mali_soft_job_system_unlock(system);

		MALI_DEBUG_ASSERT(MALI_TRUE == job->activated);
		MALI_DEBUG_PRINT(4, ("Mali Soft Job: soft job %u (0x%08X) was timed out\n", job->id, job));
		mali_soft_job_destroy(job);

		return _MALI_OSK_ERR_TIMEOUT;
	}

	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_STARTED == job->state);

	job->state = MALI_SOFT_JOB_STATE_SIGNALED;
	mali_soft_job_system_unlock(system);

	/*                                                                                  
                                                              */

	timeline_system = system->session->timeline_system;
	MALI_DEBUG_ASSERT_POINTER(timeline_system);

	/*                       */
	_mali_osk_wait_queue_wait_event(timeline_system->wait_queue, mali_soft_job_is_activated, (void *) job);

	MALI_DEBUG_PRINT(4, ("Mali Soft Job: signaling soft job %u (0x%08X)\n", job->id, job));

	schedule_mask = mali_timeline_tracker_release(&job->tracker);
	mali_scheduler_schedule_from_mask(schedule_mask, MALI_FALSE);

	mali_soft_job_destroy(job);

	return _MALI_OSK_ERR_OK;
}

static void mali_soft_job_send_activated_notification(struct mali_soft_job *job)
{
	if (NULL != job->activated_notification) {
		_mali_uk_soft_job_activated_s *res = job->activated_notification->result_buffer;
		res->user_job = job->user_job;
		mali_session_send_notification(job->system->session, job->activated_notification);
	}
	job->activated_notification = NULL;
}

void mali_soft_job_system_activate_job(struct mali_soft_job *job)
{
	MALI_DEBUG_ASSERT_POINTER(job);
	MALI_DEBUG_ASSERT_POINTER(job->system);
	MALI_DEBUG_ASSERT_POINTER(job->system->session);

	MALI_DEBUG_PRINT(4, ("Mali Soft Job: Timeline activation for soft job %u (0x%08X).\n", job->id, job));

	mali_soft_job_system_lock(job->system);

	if (unlikely(job->system->session->is_aborting)) {
		MALI_DEBUG_PRINT(3, ("Mali Soft Job: Soft job %u (0x%08X) activated while session is aborting.\n", job->id, job));

		mali_soft_job_system_unlock(job->system);

		/*                                                                 */
		mali_timeline_tracker_release(&job->tracker);
		mali_soft_job_destroy(job);
		return;
	}

	/*                              */
	mali_soft_job_send_activated_notification(job);

	/*                            */
	job->activated = MALI_TRUE;
	_mali_osk_wait_queue_wake_up(job->tracker.system->wait_queue);

	mali_soft_job_system_unlock(job->system);
}

mali_scheduler_mask mali_soft_job_system_timeout_job(struct mali_soft_job *job)
{
	mali_scheduler_mask schedule_mask = MALI_SCHEDULER_MASK_EMPTY;

	MALI_DEBUG_ASSERT_POINTER(job);
	MALI_DEBUG_ASSERT_POINTER(job->system);
	MALI_DEBUG_ASSERT_POINTER(job->system->session);
	MALI_DEBUG_ASSERT(MALI_TRUE == job->activated);

	MALI_DEBUG_PRINT(4, ("Mali Soft Job: Timeline timeout for soft job %u (0x%08X).\n", job->id, job));

	mali_soft_job_system_lock(job->system);

	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_STARTED  == job->state ||
	                  MALI_SOFT_JOB_STATE_SIGNALED == job->state);

	if (unlikely(job->system->session->is_aborting)) {
		/*                                                                          
                                   */
		mali_soft_job_system_unlock(job->system);

		return MALI_SCHEDULER_MASK_EMPTY;
	}

	if (MALI_SOFT_JOB_STATE_STARTED != job->state) {
		MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_SIGNALED == job->state);

		/*                                                  */
		MALI_DEBUG_PRINT(4, ("Mali Soft Job: Timeout on soft job %u (0x%08X) in signaled state.\n", job->id, job));
		mali_soft_job_system_unlock(job->system);
		return schedule_mask;
	}

	MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_STARTED == job->state);

	job->state = MALI_SOFT_JOB_STATE_TIMED_OUT;
	_mali_osk_atomic_inc(&job->refcount);

	mali_soft_job_system_unlock(job->system);

	schedule_mask = mali_timeline_tracker_release(&job->tracker);

	mali_soft_job_destroy(job);

	return schedule_mask;
}

void mali_soft_job_system_abort(struct mali_soft_job_system *system)
{
	u32 i;
	struct mali_soft_job *job, *tmp;
	_MALI_OSK_LIST_HEAD_STATIC_INIT(jobs);

	MALI_DEBUG_ASSERT_POINTER(system);
	MALI_DEBUG_ASSERT_POINTER(system->session);
	MALI_DEBUG_ASSERT(system->session->is_aborting);

	MALI_DEBUG_PRINT(3, ("Mali Soft Job: Aborting soft job system for session 0x%08X.\n", system->session));

	mali_soft_job_system_lock(system);

	for (i = 0; i < MALI_MAX_NUM_SOFT_JOBS; ++i) {
		job = &(system->jobs[i]);

		MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_FREE      == job->state ||
		                  MALI_SOFT_JOB_STATE_STARTED   == job->state ||
		                  MALI_SOFT_JOB_STATE_TIMED_OUT == job->state);

		if (MALI_SOFT_JOB_STATE_STARTED == job->state) {
			/*                                                                          
                                                                               
                       */
			if (MALI_TRUE == job->activated) {
				MALI_DEBUG_PRINT(3, ("Mali Soft Job: Aborting unsignaled soft job %u (0x%08X).\n", job->id, job));

				job->state = MALI_SOFT_JOB_STATE_SIGNALED;
				_mali_osk_list_move(&job->system_list, &jobs);
			}
		} else if (MALI_SOFT_JOB_STATE_TIMED_OUT == job->state) {
			MALI_DEBUG_PRINT(3, ("Mali Soft Job: Aborting timed out soft job %u (0x%08X).\n", job->id, job));

			/*                                   */
			_mali_osk_list_move(&job->system_list, &jobs);
		}
	}

	mali_soft_job_system_unlock(system);

	/*                           */
	_MALI_OSK_LIST_FOREACHENTRY(job, tmp, &jobs, struct mali_soft_job, system_list) {
		MALI_DEBUG_ASSERT(MALI_SOFT_JOB_STATE_SIGNALED  == job->state ||
		                  MALI_SOFT_JOB_STATE_TIMED_OUT == job->state);

		if (MALI_SOFT_JOB_STATE_SIGNALED == job->state) {
			mali_timeline_tracker_release(&job->tracker);
		}

		/*                                               */
		_mali_osk_list_move(&job->system_list, &system->jobs_used);

		mali_soft_job_destroy(job);
	}
}
