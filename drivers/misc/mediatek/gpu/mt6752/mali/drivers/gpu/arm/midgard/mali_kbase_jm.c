/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





/* 
                        
                               
 */

#include <mali_kbase.h>
#include <mali_kbase_config.h>
#include <mali_midg_regmap.h>
#include <mali_kbase_gator.h>
#include <mali_kbase_js_affinity.h>
#include <mali_kbase_hw.h>

#include "mali_kbase_jm.h"

#define beenthere(kctx, f, a...)  dev_dbg(kctx->kbdev->dev, "%s:" f, __func__, ##a)

#ifdef CONFIG_MALI_DEBUG_SHADER_SPLIT_FS
u64 mali_js0_affinity_mask = 0xFFFFFFFFFFFFFFFFULL;
u64 mali_js1_affinity_mask = 0xFFFFFFFFFFFFFFFFULL;
u64 mali_js2_affinity_mask = 0xFFFFFFFFFFFFFFFFULL;
#endif

#if KBASE_GPU_RESET_EN
static void kbasep_try_reset_gpu_early(struct kbase_device *kbdev);
#endif /*                    */

#ifdef CONFIG_GPU_TRACEPOINTS
static char *kbasep_make_job_slot_string(int js, char *js_string)
{
	sprintf(js_string, "job_slot_%i", js);
	return js_string;
}
#endif

static void kbase_job_hw_submit(struct kbase_device *kbdev, struct kbase_jd_atom *katom, int js)
{
	struct kbase_context *kctx;
	u32 cfg;
	u64 jc_head = katom->jc;

	KBASE_DEBUG_ASSERT(kbdev);
	KBASE_DEBUG_ASSERT(katom);

	kctx = katom->kctx;

	/*                                    */
	KBASE_DEBUG_ASSERT(kbasep_jm_is_js_free(kbdev, js, kctx));
	/*                           */
	kbase_js_debug_log_current_affinities(kbdev);
	KBASE_DEBUG_ASSERT(!kbase_js_affinity_would_violate(kbdev, js, katom->affinity));

	kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_LO), jc_head & 0xFFFFFFFF, kctx);
	kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_HI), jc_head >> 32, kctx);

#ifdef CONFIG_MALI_DEBUG_SHADER_SPLIT_FS
	{
		u64 mask;
		u32 value;

		if (0 == js)
		{
			mask = mali_js0_affinity_mask;
		}
		else if (1 == js)
		{
			mask = mali_js1_affinity_mask;
		}
		else
		{
			mask = mali_js2_affinity_mask;
		}

		value = katom->affinity & (mask & 0xFFFFFFFF);

		kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_AFFINITY_NEXT_LO), value, kctx);

		value = (katom->affinity >> 32) & ((mask>>32) & 0xFFFFFFFF);
		kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_AFFINITY_NEXT_HI), value, kctx);
	}
#else
	kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_AFFINITY_NEXT_LO), katom->affinity & 0xFFFFFFFF, kctx);
	kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_AFFINITY_NEXT_HI), katom->affinity >> 32, kctx);
#endif

	/*                                                                            */
	cfg = kctx->as_nr | JS_CONFIG_END_FLUSH_CLEAN_INVALIDATE | JS_CONFIG_START_MMU | JS_CONFIG_START_FLUSH_CLEAN_INVALIDATE | JS_CONFIG_THREAD_PRI(8);

	if (kbase_hw_has_feature(kbdev, BASE_HW_FEATURE_JOBCHAIN_DISAMBIGUATION)) {
		if (!kbdev->jm_slots[js].job_chain_flag) {
			cfg |= JS_CONFIG_JOB_CHAIN_FLAG;
			katom->atom_flags |= KBASE_KATOM_FLAGS_JOBCHAIN;
			kbdev->jm_slots[js].job_chain_flag = MALI_TRUE;
		} else {
			katom->atom_flags &= ~KBASE_KATOM_FLAGS_JOBCHAIN;
			kbdev->jm_slots[js].job_chain_flag = MALI_FALSE;
		}
	}

	kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_CONFIG_NEXT), cfg, kctx);

	/*                                      
                                                                          
                                                                           
                                               */
	katom->start_timestamp = ktime_get();

	/*      */
	dev_dbg(kbdev->dev, "JS: Submitting atom %p from ctx %p to js[%d] with head=0x%llx, affinity=0x%llx", katom, kctx, js, jc_head, katom->affinity);

	KBASE_TRACE_ADD_SLOT_INFO(kbdev, JM_SUBMIT, kctx, katom, jc_head, js, (u32) katom->affinity);

#ifdef CONFIG_MALI_GATOR_SUPPORT
	kbase_trace_mali_job_slots_event(GATOR_MAKE_EVENT(GATOR_JOB_SLOT_START, js), kctx, kbase_jd_atom_id(kctx, katom)); 
#endif				/*                           */
#ifdef CONFIG_GPU_TRACEPOINTS
	if (kbasep_jm_nr_jobs_submitted(&kbdev->jm_slots[js]) == 1)
	{
		/*                                                           */
		char js_string[16];
		trace_gpu_sched_switch(kbasep_make_job_slot_string(js, js_string), ktime_to_ns(katom->start_timestamp), (u32)katom->kctx, 0, katom->work_id);
		kbdev->jm_slots[js].last_context = katom->kctx;
	}
#endif
	kbase_timeline_job_slot_submit(kbdev, kctx, katom, js);

	kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_COMMAND_NEXT), JS_COMMAND_START, katom->kctx);
}

void kbase_job_submit_nolock(struct kbase_device *kbdev, struct kbase_jd_atom *katom, int js)
{
	struct kbase_jm_slot *jm_slots;
#if KBASE_PM_EN
	base_jd_core_req core_req;
#endif
	KBASE_DEBUG_ASSERT(kbdev);
	KBASE_DEBUG_ASSERT(katom);

	jm_slots = kbdev->jm_slots;

#if KBASE_PM_EN
	core_req = katom->core_req;
	if (core_req & BASE_JD_REQ_ONLY_COMPUTE) {
		unsigned long flags;
		int device_nr = (core_req & BASE_JD_REQ_SPECIFIC_COHERENT_GROUP) ? katom->device_nr : 0;
		KBASE_DEBUG_ASSERT(device_nr < 2);
		spin_lock_irqsave(&kbdev->pm.metrics.lock, flags);
		kbasep_pm_record_job_status(kbdev);
		kbdev->pm.metrics.active_cl_ctx[device_nr]++;
		spin_unlock_irqrestore(&kbdev->pm.metrics.lock, flags);
	} else {
		unsigned long flags;

		spin_lock_irqsave(&kbdev->pm.metrics.lock, flags);
		kbasep_pm_record_job_status(kbdev);
		kbdev->pm.metrics.active_gl_ctx++;
		spin_unlock_irqrestore(&kbdev->pm.metrics.lock, flags);
	}
#endif

	/*
                
                                               
                  
                          
                                                       
                                         
  */
	kbasep_jm_enqueue_submit_slot(&jm_slots[js], katom);
	kbase_job_hw_submit(kbdev, katom, js);
}

void kbase_job_done_slot(struct kbase_device *kbdev, int s, u32 completion_code, u64 job_tail, ktime_t *end_timestamp)
{
	struct kbase_jm_slot *slot;
	struct kbase_jd_atom *katom;
	mali_addr64 jc_head;
	struct kbase_context *kctx;

	KBASE_DEBUG_ASSERT(kbdev);

	if (completion_code != BASE_JD_EVENT_DONE && completion_code != BASE_JD_EVENT_STOPPED)
		dev_err(kbdev->dev, "GPU fault 0x%02lx from job slot %d\n", (unsigned long)completion_code, s);

	/*                                                                        
                                                             
                                                                           
                                                           */

	slot = &kbdev->jm_slots[s];
	katom = kbasep_jm_dequeue_submit_slot(slot);

	/*                                                                                                    */
	if (kbasep_jm_is_dummy_workaround_job(kbdev, katom)) {
		KBASE_TRACE_ADD_SLOT_INFO(kbdev, JM_JOB_DONE, NULL, NULL, 0, s, completion_code);
		return;
	}

	jc_head = katom->jc;
	kctx = katom->kctx;

	KBASE_TRACE_ADD_SLOT_INFO(kbdev, JM_JOB_DONE, kctx, katom, jc_head, s, completion_code);

	if (completion_code != BASE_JD_EVENT_DONE && completion_code != BASE_JD_EVENT_STOPPED) {

#if KBASE_TRACE_DUMP_ON_JOB_SLOT_ERROR != 0
		KBASE_TRACE_DUMP(kbdev);
#endif
	}
	if (job_tail != 0) {
		mali_bool was_updated = (job_tail != jc_head);
		/*                                                                                                      */
		katom->jc = job_tail;
		if (was_updated)
			KBASE_TRACE_ADD_SLOT(kbdev, JM_UPDATE_HEAD, kctx, katom, job_tail, s);
	}

	/*                                                            */
	if (katom->event_code != BASE_JD_EVENT_JOB_CANCELLED)
		katom->event_code = (base_jd_event_code) completion_code;

	kbase_device_trace_register_access(kctx, REG_WRITE, JOB_CONTROL_REG(JOB_IRQ_CLEAR), 1 << s);

	/*                                     
   
                                                 
                                
                                                                       
                                                                              
  */
#ifdef CONFIG_GPU_TRACEPOINTS
	if (kbasep_jm_nr_jobs_submitted(slot) != 0) {
		struct kbase_jd_atom *katom;
		char js_string[16];
		katom = kbasep_jm_peek_idx_submit_slot(slot, 0);        /*                      */
		trace_gpu_sched_switch(kbasep_make_job_slot_string(s, js_string), ktime_to_ns(*end_timestamp), (u32)katom->kctx, 0, katom->work_id);
		slot->last_context = katom->kctx;
	} else {
		char js_string[16];
		trace_gpu_sched_switch(kbasep_make_job_slot_string(s, js_string), ktime_to_ns(ktime_get()), 0, 0, 0);
		slot->last_context = 0;
	}
#endif
	kbase_jd_done(katom, s, end_timestamp, KBASE_JS_ATOM_DONE_START_NEW_ATOMS);
}

/* 
                                                                            
                                                                  
  
                                                                             
                                                                             
                                                                 
 */
STATIC void kbasep_job_slot_update_head_start_timestamp(struct kbase_device *kbdev, struct kbase_jm_slot *slot, ktime_t end_timestamp)
{
	KBASE_DEBUG_ASSERT(slot);

	if (kbasep_jm_nr_jobs_submitted(slot) > 0) {
		struct kbase_jd_atom *katom;
		ktime_t new_timestamp;
		ktime_t timestamp_diff;
		katom = kbasep_jm_peek_idx_submit_slot(slot, 0);	/*                      */

		KBASE_DEBUG_ASSERT(katom != NULL);

		if (kbasep_jm_is_dummy_workaround_job(kbdev, katom) != MALI_FALSE) {
			/*                                                        */
			return;
		}

		/*                                                                                        */
		new_timestamp = ktime_sub_ns(end_timestamp, KBASE_JS_IRQ_THROTTLE_TIME_US * 1000);
		timestamp_diff = ktime_sub(new_timestamp, katom->start_timestamp);
		if (ktime_to_ns(timestamp_diff) >= 0) {
			/*                                                                                  
                                                                                      
                         */
			katom->start_timestamp = new_timestamp;
		}
	}
}

void kbase_job_done(struct kbase_device *kbdev, u32 done)
{
	unsigned long flags;
	int i;
	u32 count = 0;
	ktime_t end_timestamp = ktime_get();
	struct kbasep_js_device_data *js_devdata;

	KBASE_DEBUG_ASSERT(kbdev);
	js_devdata = &kbdev->js_data;

	KBASE_TRACE_ADD(kbdev, JM_IRQ, NULL, NULL, 0, done);

	memset(&kbdev->slot_submit_count_irq[0], 0, sizeof(kbdev->slot_submit_count_irq));

	/*                                                                         
                                                     */
	{
		int irq_throttle_cycles = atomic_read(&kbdev->irq_throttle_cycles);
		kbase_reg_write(kbdev, JOB_CONTROL_REG(JOB_IRQ_THROTTLE), irq_throttle_cycles, NULL);
	}

	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

	while (done) {
		struct kbase_jm_slot *slot;
		u32 failed = done >> 16;

		/*                                      */
		u32 finished = (done & 0xFFFF) | failed;

		/*                                                    
                                                    
                   */
		i = ffs(finished) - 1;
		KBASE_DEBUG_ASSERT(i >= 0);

		slot = &kbdev->jm_slots[i];

		do {
			int nr_done;
			u32 active;
			u32 completion_code = BASE_JD_EVENT_DONE;	/*           */
			u64 job_tail = 0;

			if (failed & (1u << i)) {
				/*                                                                    */
				completion_code = kbase_reg_read(kbdev, JOB_SLOT_REG(i, JS_STATUS), NULL);

				switch (completion_code) {
				case BASE_JD_EVENT_STOPPED:
#ifdef CONFIG_MALI_GATOR_SUPPORT
					kbase_trace_mali_job_slots_event(GATOR_MAKE_EVENT(GATOR_JOB_SLOT_SOFT_STOPPED, i), NULL, 0);
#endif				/*                           */
					/*                                                                                      */
					job_tail = (u64) kbase_reg_read(kbdev, JOB_SLOT_REG(i, JS_TAIL_LO), NULL) | ((u64) kbase_reg_read(kbdev, JOB_SLOT_REG(i, JS_TAIL_HI), NULL) << 32);
					break;
				case BASE_JD_EVENT_NOT_STARTED:
					/*                                                                                                                */
					completion_code = BASE_JD_EVENT_TERMINATED;
					/*               */
				default:
					dev_warn(kbdev->dev, "error detected from slot %d, job status 0x%08x (%s)", i, completion_code, kbase_exception_name(completion_code));
				}
			}

			kbase_reg_write(kbdev, JOB_CONTROL_REG(JOB_IRQ_CLEAR), done & ((1 << i) | (1 << (i + 16))), NULL);
			active = kbase_reg_read(kbdev, JOB_CONTROL_REG(JOB_IRQ_JS_STATE), NULL);

			if (((active >> i) & 1) == 0 && (((done >> (i + 16)) & 1) == 0)) {
				/*                                               
      
                                                                  
                                                                                   
                                                                      
                                                                           
                                                                                  
                                                                                         
                                                                                         
                                     
      
                                                                                        
                                                                                      
      
                                                                                                 
                                                                                             
                                                                                               
                                          
      
                                                                                                    
                                                                                                
                                                                                  
     */
				u32 rawstat = kbase_reg_read(kbdev, JOB_CONTROL_REG(JOB_IRQ_RAWSTAT), NULL);

				if ((rawstat >> (i + 16)) & 1) {
					/*                                                                 */
					active |= (1u << i);
				}
			}

			dev_dbg(kbdev->dev, "Job ended with status 0x%08X\n", completion_code);

			nr_done = kbasep_jm_nr_jobs_submitted(slot);
			nr_done -= (active >> i) & 1;
			nr_done -= (active >> (i + 16)) & 1;

			if (nr_done <= 0) {
				dev_warn(kbdev->dev, "Spurious interrupt on slot %d", i);
				goto spurious;
			}

			count += nr_done;

			while (nr_done) {
				if (nr_done == 1) {
					kbase_job_done_slot(kbdev, i, completion_code, job_tail, &end_timestamp);
				} else {
					/*                                                                                            
                                                                                                   
                                                                    
      */
					kbase_job_done_slot(kbdev, i, BASE_JD_EVENT_DONE, 0, &end_timestamp);
				}
				nr_done--;
			}

 spurious:
			done = kbase_reg_read(kbdev, JOB_CONTROL_REG(JOB_IRQ_RAWSTAT), NULL);

			if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_10883)) {
				/*                                                        */
				if (((active >> i) & 1) && (0 == kbase_reg_read(kbdev, JOB_SLOT_REG(i, JS_STATUS), NULL))) {
					/*                                      */
					done |= (1u << i);
				}
			}

			failed = done >> 16;
			finished = (done & 0xFFFF) | failed;
		} while (finished & (1 << i));

		kbasep_job_slot_update_head_start_timestamp(kbdev, slot, end_timestamp);
	}
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
#if KBASE_GPU_RESET_EN
	if (atomic_read(&kbdev->reset_gpu) == KBASE_RESET_GPU_COMMITTED) {
		/*                                                                      
                                                                     
   */
		kbasep_try_reset_gpu_early(kbdev);
	}
#endif /*                    */
	KBASE_TRACE_ADD(kbdev, JM_IRQ_END, NULL, NULL, 0, count);
}
KBASE_EXPORT_TEST_API(kbase_job_done)

static mali_bool kbasep_soft_stop_allowed(struct kbase_device *kbdev, u16 core_reqs)
{
	mali_bool soft_stops_allowed = MALI_TRUE;

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8408)) {
		if ((core_reqs & BASE_JD_REQ_T) != 0)
			soft_stops_allowed = MALI_FALSE;
	}
	return soft_stops_allowed;
}

static mali_bool kbasep_hard_stop_allowed(struct kbase_device *kbdev, u16 core_reqs)
{
	mali_bool hard_stops_allowed = MALI_TRUE;

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8394)) {
		if ((core_reqs & BASE_JD_REQ_T) != 0)
			hard_stops_allowed = MALI_FALSE;
	}
	return hard_stops_allowed;
}

static void kbasep_job_slot_soft_or_hard_stop_do_action(struct kbase_device *kbdev, int js, u32 action, u16 core_reqs, struct kbase_jd_atom *target_katom)
{
	struct kbase_context *kctx = target_katom->kctx;
#if KBASE_TRACE_ENABLE
	u32 status_reg_before;
	u64 job_in_head_before;
	u32 status_reg_after;

	KBASE_DEBUG_ASSERT(!(action & (~JS_COMMAND_MASK)));

	/*                        */
	job_in_head_before = ((u64) kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_HEAD_LO), NULL))
	    | (((u64) kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_HEAD_HI), NULL)) << 32);
	status_reg_before = kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_STATUS), NULL);
#endif

	if (action == JS_COMMAND_SOFT_STOP) {
		mali_bool soft_stop_allowed = kbasep_soft_stop_allowed(kbdev, core_reqs);
		if (!soft_stop_allowed) {
#ifdef CONFIG_MALI_DEBUG
			dev_dbg(kbdev->dev, "Attempt made to soft-stop a job that cannot be soft-stopped. core_reqs = 0x%X", (unsigned int)core_reqs);
#endif				/*                   */
			return;
		}

		/*                                                                                 */
		target_katom->atom_flags |= KBASE_KATOM_FLAG_BEEN_SOFT_STOPPPED;
	}

	if (action == JS_COMMAND_HARD_STOP) {
		mali_bool hard_stop_allowed = kbasep_hard_stop_allowed(kbdev, core_reqs);
		if (!hard_stop_allowed) {
			/*                                                    
                                                                                        
                                                                                 
     
                                                                                                  
                                                                                                   
     
                                                                                         
                               
     
                                                                           
                                                                    
    */
			dev_warn(kbdev->dev, "Attempt made to hard-stop a job that cannot be hard-stopped. core_reqs = 0x%X", (unsigned int)core_reqs);
			return;
		}
		target_katom->atom_flags |= KBASE_KATOM_FLAG_BEEN_HARD_STOPPED;
	}

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8316) && action == JS_COMMAND_SOFT_STOP) {
		int i;
		struct kbase_jm_slot *slot;
		slot = &kbdev->jm_slots[js];

		for (i = 0; i < kbasep_jm_nr_jobs_submitted(slot); i++) {
			struct kbase_jd_atom *katom;

			katom = kbasep_jm_peek_idx_submit_slot(slot, i);

			KBASE_DEBUG_ASSERT(katom);

			if (kbasep_jm_is_dummy_workaround_job(kbdev, katom) != MALI_FALSE) {
				/*                                                       
      
                                                                                       
                                                                                           */
				continue;
			}

			/*                                                            
                                                                   
                                                                      
                              
     
                                                               
                                                                 
                                                                     
                                                                      
                                                                  
                                                          */
			kbase_as_poking_timer_retain_atom(kbdev, katom->kctx, katom);
		}
	}

	if (kbase_hw_has_feature(kbdev, BASE_HW_FEATURE_JOBCHAIN_DISAMBIGUATION)) {
		if (action == JS_COMMAND_SOFT_STOP)
			action = (target_katom->atom_flags & KBASE_KATOM_FLAGS_JOBCHAIN) ?
				 JS_COMMAND_SOFT_STOP_1 :
		         JS_COMMAND_SOFT_STOP_0;
		else
			action = (target_katom->atom_flags & KBASE_KATOM_FLAGS_JOBCHAIN) ?
				 JS_COMMAND_HARD_STOP_1 :
		         JS_COMMAND_HARD_STOP_0;
	}

	kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_COMMAND), action, kctx);

#if KBASE_TRACE_ENABLE
	status_reg_after = kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_STATUS), NULL);
	if (status_reg_after == BASE_JD_EVENT_ACTIVE) {
		struct kbase_jm_slot *slot;
		struct kbase_jd_atom *head;
		struct kbase_context *head_kctx;

		slot = &kbdev->jm_slots[js];
		head = kbasep_jm_peek_idx_submit_slot(slot, slot->submitted_nr - 1);
		head_kctx = head->kctx;

		/*                                                                       
                                         
                                          
   */
		if (status_reg_before == BASE_JD_EVENT_ACTIVE)
			KBASE_TRACE_ADD_SLOT(kbdev, JM_CHECK_HEAD, head_kctx, head, job_in_head_before, js);
		else
			KBASE_TRACE_ADD_SLOT(kbdev, JM_CHECK_HEAD, NULL, NULL, 0, js);

		switch (action) {
		case JS_COMMAND_SOFT_STOP:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_SOFTSTOP, head_kctx, head, head->jc, js);
			break;
		case JS_COMMAND_SOFT_STOP_0:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_SOFTSTOP_0, head_kctx, head, head->jc, js);
			break;
		case JS_COMMAND_SOFT_STOP_1:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_SOFTSTOP_1, head_kctx, head, head->jc, js);
			break;
		case JS_COMMAND_HARD_STOP:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_HARDSTOP, head_kctx, head, head->jc, js);
			break;
		case JS_COMMAND_HARD_STOP_0:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_HARDSTOP_0, head_kctx, head, head->jc, js);
			break;
		case JS_COMMAND_HARD_STOP_1:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_HARDSTOP_1, head_kctx, head, head->jc, js);
			break;
		default:
			BUG();
			break;
		}
	} else {
		if (status_reg_before == BASE_JD_EVENT_ACTIVE)
			KBASE_TRACE_ADD_SLOT(kbdev, JM_CHECK_HEAD, NULL, NULL, job_in_head_before, js);
		else
			KBASE_TRACE_ADD_SLOT(kbdev, JM_CHECK_HEAD, NULL, NULL, 0, js);

		switch (action) {
		case JS_COMMAND_SOFT_STOP:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_SOFTSTOP, NULL, NULL, 0, js);
			break;
		case JS_COMMAND_SOFT_STOP_0:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_SOFTSTOP_0, NULL, NULL, 0, js);
			break;
		case JS_COMMAND_SOFT_STOP_1:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_SOFTSTOP_1, NULL, NULL, 0, js);
			break;
		case JS_COMMAND_HARD_STOP:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_HARDSTOP, NULL, NULL, 0, js);
			break;
		case JS_COMMAND_HARD_STOP_0:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_HARDSTOP_0, NULL, NULL, 0, js);
			break;
		case JS_COMMAND_HARD_STOP_1:
			KBASE_TRACE_ADD_SLOT(kbdev, JM_HARDSTOP_1, NULL, NULL, 0, js);
			break;
		default:
			BUG();
			break;
		}
	}
#endif
}

/*                                                         */
#define JM_SLOT_MAX_JOB_SUBMIT_REGS    2
#define JM_JOB_IS_CURRENT_JOB_INDEX(n) (1 == n)	/*                                  */
#define JM_JOB_IS_NEXT_JOB_INDEX(n)    (2 == n)	/*                                           */

/*                          
  
                                                                                    
                                                                         
  
                                                                                        
  
                                        
                                                                                                          
                                                       
                                                                                                                  
                                                                                                  
 */
static void kbasep_job_slot_soft_or_hard_stop(struct kbase_device *kbdev, struct kbase_context *kctx, int js, struct kbase_jd_atom *target_katom, u32 action)
{
	struct kbase_jd_atom *katom;
	u8 i;
	u8 jobs_submitted;
	struct kbase_jm_slot *slot;
	u16 core_reqs;
	struct kbasep_js_device_data *js_devdata;
	mali_bool can_safely_stop = kbase_hw_has_feature(kbdev, BASE_HW_FEATURE_JOBCHAIN_DISAMBIGUATION);
	u32 hw_action = action & JS_COMMAND_MASK;

	KBASE_DEBUG_ASSERT(hw_action == JS_COMMAND_HARD_STOP ||
			hw_action == JS_COMMAND_SOFT_STOP);
	KBASE_DEBUG_ASSERT(kbdev);
	js_devdata = &kbdev->js_data;

	slot = &kbdev->jm_slots[js];
	KBASE_DEBUG_ASSERT(slot);
	lockdep_assert_held(&js_devdata->runpool_irq.lock);

	jobs_submitted = kbasep_jm_nr_jobs_submitted(slot);

	KBASE_TIMELINE_TRY_SOFT_STOP(kctx, js, 1);
	KBASE_TRACE_ADD_SLOT_INFO(kbdev, JM_SLOT_SOFT_OR_HARD_STOP, kctx, NULL, 0u, js, jobs_submitted);

	if (jobs_submitted > JM_SLOT_MAX_JOB_SUBMIT_REGS)
		i = jobs_submitted - JM_SLOT_MAX_JOB_SUBMIT_REGS;
	else
		i = 0;

	/*                                                                                  */
	for (; i < jobs_submitted; i++) {
		katom = kbasep_jm_peek_idx_submit_slot(slot, i);

		if (kctx && katom->kctx != kctx)
			continue;

		if (target_katom && katom != target_katom)
			continue;

		if (kbasep_jm_is_dummy_workaround_job(kbdev, katom))
			continue;

		core_reqs = katom->core_req;

		/*                                                         
                                                                 
                                                            
                                                   
   */
		kbase_job_check_enter_disjoint(kbdev, action, core_reqs, katom);

		if (JM_JOB_IS_CURRENT_JOB_INDEX(jobs_submitted - i)) {
			/*                                                                        */
			if (kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_COMMAND_NEXT), NULL) == 0) {
				kbasep_job_slot_soft_or_hard_stop_do_action(kbdev,
						js, hw_action, core_reqs, katom);
			} else {
				/*                                  */
				beenthere(kctx, "clearing job from next registers on slot %d", js);
				kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_COMMAND_NEXT), JS_COMMAND_NOP, NULL);
				/*                                                             */
				if (kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_LO), NULL) != 0 || kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_HI), NULL) != 0) {
					/*                                                                      */
					struct kbase_jd_atom *dequeued_katom = kbasep_jm_dequeue_tail_submit_slot(slot);
					KBASE_DEBUG_ASSERT(dequeued_katom == katom);
					jobs_submitted--;

					/*                                */
					kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_LO), 0, NULL);
					kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_HI), 0, NULL);

					/*                                                                     
                                                       */
					if (can_safely_stop)
						slot->job_chain_flag = !slot->job_chain_flag;

					KBASE_TRACE_ADD_SLOT(kbdev, JM_SLOT_EVICT, dequeued_katom->kctx, dequeued_katom, dequeued_katom->jc, js);

					kbase_job_check_enter_disjoint(kbdev, action, 0u, dequeued_katom);
					/*                                                                                     */
					kbase_jd_done(dequeued_katom, js, NULL, KBASE_JS_ATOM_DONE_EVICTED_FROM_NEXT);
				} else {
					/*                                                                               
                                                                */
					beenthere(kctx, "missed job in next register, soft/hard-stopping slot %d", js);
					kbasep_job_slot_soft_or_hard_stop_do_action(kbdev,
							js, hw_action, core_reqs, katom);
				}
			}
		} else if (JM_JOB_IS_NEXT_JOB_INDEX(jobs_submitted - i)) {
			/*                                                   
                                                          
                                                    
                                                        
                                                    
                                                     
                                                         
                                                        
                                                       
                                                         
                     
    */
			if (kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_COMMAND_NEXT), NULL) != 0) {
				struct kbase_jd_atom *check_next_atom;
				/*                                                               */

				/*                                                                 
      
                                                     
                                                            
                                                        
                                                       
                                                                               
                                                                      
                                          
      
                                                                             
                                                                               
                                                    
      
                                                                                  
                               */
				check_next_atom = kbasep_jm_peek_idx_submit_slot(slot, i + 1);
				if (kbasep_jm_is_dummy_workaround_job(kbdev, check_next_atom) != MALI_FALSE)
					continue;

				if (!can_safely_stop) {
					beenthere(kctx, "clearing job from next registers on slot %d", js);
					kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_COMMAND_NEXT), JS_COMMAND_NOP, NULL);

					/*                                                             */
					if (kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_LO), NULL) != 0 || kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_HI), NULL) != 0) {
						/*                                                         */
						struct kbase_jd_atom *dequeued_katom = kbasep_jm_dequeue_tail_submit_slot(slot);
						KBASE_DEBUG_ASSERT(dequeued_katom != NULL);
						jobs_submitted--;

						/*                                */
						kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_LO), 0, NULL);
						kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_HI), 0, NULL);

						KBASE_TRACE_ADD_SLOT(kbdev, JM_SLOT_EVICT, dequeued_katom->kctx, dequeued_katom, dequeued_katom->jc, js);

						kbase_job_check_enter_disjoint(kbdev, action, 0u, dequeued_katom);
						/*                                                                                     */
						kbase_jd_done(dequeued_katom, js, NULL, KBASE_JS_ATOM_DONE_EVICTED_FROM_NEXT);
					} else {
						/*                                                                                   
                                                                      */
						continue;
					}
				}

				/*                                                     */
				beenthere(kctx, "soft/hard-stopped slot %d (there was a job in next which was successfully cleared)\n", js);
				kbasep_job_slot_soft_or_hard_stop_do_action(kbdev,
						js, hw_action, core_reqs, katom);
			}
			/*                                                                
                                                                
    */
		}
	}

	KBASE_TIMELINE_TRY_SOFT_STOP(kctx, js, 0);
}

void kbase_job_kill_jobs_from_context(struct kbase_context *kctx)
{
	unsigned long flags;
	struct kbase_device *kbdev;
	struct kbasep_js_device_data *js_devdata;
	int i;

	KBASE_DEBUG_ASSERT(kctx != NULL);
	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;

	/*                                                  */
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

	/*                                                          */
	for (i = 0; i < BASE_JD_ATOM_COUNT; i++)
		kctx->jctx.atoms[i].event_code = BASE_JD_EVENT_JOB_CANCELLED;

	for (i = 0; i < kbdev->gpu_props.num_job_slots; i++)
		kbase_job_slot_hardstop(kctx, i, NULL);

	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
}

void kbase_job_zap_context(struct kbase_context *kctx)
{
	struct kbase_device *kbdev;
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_kctx_info *js_kctx_info;
	int i;
	mali_bool evict_success;

	KBASE_DEBUG_ASSERT(kctx != NULL);
	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;
	js_kctx_info = &kctx->jctx.sched_info;

	/*
                                                                      
                                                                       
                                                   
  */

	/*                                    
                               
                                            */

	mutex_lock(&js_kctx_info->ctx.jsctx_mutex);
	js_kctx_info->ctx.is_dying = MALI_TRUE;

	dev_dbg(kbdev->dev, "Zap: Try Evict Ctx %p", kctx);
	mutex_lock(&js_devdata->queue_mutex);
	evict_success = kbasep_js_policy_try_evict_ctx(&js_devdata->policy, kctx);
	mutex_unlock(&js_devdata->queue_mutex);

	/*
                          
                                                                             
                                                                         
            
                                                                         
                            
                                                                          
                  
                                                                           
                            
                                                                            
                         
                                                                         
                                                                            
                                                                           
                                                                          
                                                                        
                                                                          
                                                                         
                      
                                                                            
                             
                                                             
                                                                          
                                              
                                                                        
                                                                          
                                                                            
                                          
                                                                         
                                                                         
                               
  */

	if (evict_success != MALI_FALSE || js_kctx_info->ctx.is_scheduled == MALI_FALSE) {
		/*                                                               
                            
                                                                            
    
                                                            
                                                                  
                                                                         
                              
   */

		KBASE_TRACE_ADD(kbdev, JM_ZAP_NON_SCHEDULED, kctx, NULL, 0u, js_kctx_info->ctx.is_scheduled);

		dev_dbg(kbdev->dev, "Zap: Ctx %p evict_success=%d, scheduled=%d", kctx, evict_success, js_kctx_info->ctx.is_scheduled);

		if (evict_success != MALI_FALSE) {
			/*                                                                 
                                        
     
                                                                       */
			kbasep_js_runpool_requeue_or_kill_ctx(kbdev, kctx, MALI_FALSE);
		}
		mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);
	} else {
		unsigned long flags;
		mali_bool was_retained;
		/*                                                                  */
		KBASE_TRACE_ADD(kbdev, JM_ZAP_SCHEDULED, kctx, NULL, 0u, js_kctx_info->ctx.is_scheduled);
		dev_dbg(kbdev->dev, "Zap: Ctx %p is in RunPool", kctx);

		/*                                               */
		spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
		kbasep_js_clear_submit_allowed(js_devdata, kctx);

		/*                                                                                      
                                                                                 */
		was_retained = kbasep_js_runpool_retain_ctx_nolock(kbdev, kctx);

		/*                                                                                    */
		KBASE_DEBUG_ASSERT(was_retained != MALI_FALSE);

		dev_dbg(kbdev->dev, "Zap: Ctx %p Kill Any Running jobs", kctx);
		/*                                                                               
                                                                                        */
		for (i = 0; i < kbdev->gpu_props.num_job_slots; i++)
			kbase_job_slot_hardstop(kctx, i, NULL);

		spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
		mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);

		dev_dbg(kbdev->dev, "Zap: Ctx %p Release (may or may not schedule out immediately)", kctx);
		kbasep_js_runpool_release_ctx(kbdev, kctx);
	}
	KBASE_TRACE_ADD(kbdev, JM_ZAP_DONE, kctx, NULL, 0u, 0u);

	/*                                                                       
                                                                          
                                                                          
                 
   
                                         */
}
KBASE_EXPORT_TEST_API(kbase_job_zap_context)

mali_error kbase_job_slot_init(struct kbase_device *kbdev)
{
	int i;
	KBASE_DEBUG_ASSERT(kbdev);

	for (i = 0; i < kbdev->gpu_props.num_job_slots; i++)
		kbasep_jm_init_submit_slot(&kbdev->jm_slots[i]);

	return MALI_ERROR_NONE;
}
KBASE_EXPORT_TEST_API(kbase_job_slot_init)

void kbase_job_slot_halt(struct kbase_device *kbdev)
{
	CSTD_UNUSED(kbdev);
}

void kbase_job_slot_term(struct kbase_device *kbdev)
{
	KBASE_DEBUG_ASSERT(kbdev);
}
KBASE_EXPORT_TEST_API(kbase_job_slot_term)

/* 
                                                                          
  
                                                             
                                                                         
  
                                                                               
  
                                        
                                                 
                                                                                 
                                                            
 */
void kbase_job_slot_softstop_swflags(struct kbase_device *kbdev, int js,
			struct kbase_jd_atom *target_katom, u32 sw_flags)
{
	KBASE_DEBUG_ASSERT(!(sw_flags & JS_COMMAND_MASK));
	kbasep_job_slot_soft_or_hard_stop(kbdev, NULL, js, target_katom,
			JS_COMMAND_SOFT_STOP | sw_flags);
}

/* 
                                   
  
                                                             
                                                                         
  
                                                                               
  
                                        
                                                 
                                                                                 
 */
void kbase_job_slot_softstop(struct kbase_device *kbdev, int js, struct kbase_jd_atom *target_katom)
{
	kbase_job_slot_softstop_swflags(kbdev, js, target_katom, 0u);
}

/* 
                                   
  
                                                             
  
                                                                      
                    
                                       
                                                                           
                           
 */
void kbase_job_slot_hardstop(struct kbase_context *kctx, int js,
				struct kbase_jd_atom *target_katom)
{
	struct kbase_device *kbdev = kctx->kbdev;

	kbasep_job_slot_soft_or_hard_stop(kbdev, kctx, js, target_katom,
						JS_COMMAND_HARD_STOP);
#if KBASE_GPU_RESET_EN
	if (kbase_hw_has_issue(kctx->kbdev, BASE_HW_ISSUE_8401) ||
		kbase_hw_has_issue(kctx->kbdev, BASE_HW_ISSUE_9510) ||
		(kbase_hw_has_issue(kctx->kbdev, BASE_HW_ISSUE_T76X_3542) &&
		(target_katom == NULL || target_katom->core_req & BASE_JD_REQ_FS_AFBC))) {
		/*                                                     
                                                            
                          
                                                       
                                                                
                        */
		if (kbase_prepare_to_reset_gpu_locked(kbdev)) {
			dev_err(kbdev->dev, "Issueing GPU\
			soft-reset after hard stopping due to hardware issue");
			kbase_reset_gpu_locked(kbdev);
		}
	}
#endif
}

/* 
                                                                          
         
  
                                                                          
                            
  
                                                                             
                                                                             
                                    
  
                                                                      
                                                   
 */
void kbase_job_check_enter_disjoint(struct kbase_device *kbdev, u32 action,
		u16 core_reqs, struct kbase_jd_atom *target_katom)
{
	u32 hw_action = action & JS_COMMAND_MASK;

	/*                                                     */
	if (hw_action == JS_COMMAND_HARD_STOP &&
			!kbasep_hard_stop_allowed(kbdev, core_reqs))
		return;

	/*                                                              
                     */
	if (hw_action == JS_COMMAND_SOFT_STOP &&
			!(kbasep_soft_stop_allowed(kbdev, core_reqs) &&
			  (action & JS_COMMAND_SW_CAUSES_DISJOINT)))
		return;

	/*                                                             */
	if (target_katom->atom_flags & KBASE_KATOM_FLAG_IN_DISJOINT)
		return;

	target_katom->atom_flags |= KBASE_KATOM_FLAG_IN_DISJOINT;
	kbase_disjoint_state_up(kbdev);
}

/* 
                                                                           
                                                  
 */
void kbase_job_check_leave_disjoint(struct kbase_device *kbdev,
		struct kbase_jd_atom *target_katom)
{
	if (target_katom->atom_flags & KBASE_KATOM_FLAG_IN_DISJOINT) {
		target_katom->atom_flags &= ~KBASE_KATOM_FLAG_IN_DISJOINT;
		kbase_disjoint_state_down(kbdev);
	}
}


#if KBASE_GPU_RESET_EN
static void kbase_debug_dump_registers(struct kbase_device *kbdev)
{
	int i;
	dev_err(kbdev->dev, "Register state:");
	dev_err(kbdev->dev, "  GPU_IRQ_RAWSTAT=0x%08x GPU_STATUS=0x%08x",
		kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_IRQ_RAWSTAT), NULL),
		kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_STATUS), NULL));
	dev_err(kbdev->dev, "  JOB_IRQ_RAWSTAT=0x%08x JOB_IRQ_JS_STATE=0x%08x JOB_IRQ_THROTTLE=0x%08x",
		kbase_reg_read(kbdev, JOB_CONTROL_REG(JOB_IRQ_RAWSTAT), NULL),
		kbase_reg_read(kbdev, JOB_CONTROL_REG(JOB_IRQ_JS_STATE), NULL),
		kbase_reg_read(kbdev, JOB_CONTROL_REG(JOB_IRQ_THROTTLE), NULL));
	for (i = 0; i < 3; i++) {
		dev_err(kbdev->dev, "  JS%d_STATUS=0x%08x      JS%d_HEAD_LO=0x%08x",
			i, kbase_reg_read(kbdev, JOB_SLOT_REG(i, JS_STATUS),
					NULL),
			i, kbase_reg_read(kbdev, JOB_SLOT_REG(i, JS_HEAD_LO),
					NULL));
	}
	dev_err(kbdev->dev, "  MMU_IRQ_RAWSTAT=0x%08x GPU_FAULTSTATUS=0x%08x",
		kbase_reg_read(kbdev, MMU_REG(MMU_IRQ_RAWSTAT), NULL),
		kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_FAULTSTATUS), NULL));
	dev_err(kbdev->dev, "  GPU_IRQ_MASK=0x%08x    JOB_IRQ_MASK=0x%08x     MMU_IRQ_MASK=0x%08x",
		kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), NULL),
		kbase_reg_read(kbdev, JOB_CONTROL_REG(JOB_IRQ_MASK), NULL),
		kbase_reg_read(kbdev, MMU_REG(MMU_IRQ_MASK), NULL));
	dev_err(kbdev->dev, "  PWR_OVERRIDE0=0x%08x   PWR_OVERRIDE1=0x%08x",
		kbase_reg_read(kbdev, GPU_CONTROL_REG(PWR_OVERRIDE0), NULL),
		kbase_reg_read(kbdev, GPU_CONTROL_REG(PWR_OVERRIDE1), NULL));
	dev_err(kbdev->dev, "  SHADER_CONFIG=0x%08x   L2_MMU_CONFIG=0x%08x",
		kbase_reg_read(kbdev, GPU_CONTROL_REG(SHADER_CONFIG), NULL),
		kbase_reg_read(kbdev, GPU_CONTROL_REG(L2_MMU_CONFIG), NULL));
}

void kbasep_reset_timeout_worker(struct work_struct *data)
{
	unsigned long flags;
	struct kbase_device *kbdev;
	int i;
	ktime_t end_timestamp = ktime_get();
	struct kbasep_js_device_data *js_devdata;
	struct kbase_uk_hwcnt_setup hwcnt_setup = { {0} };
	enum kbase_instr_state bckp_state;

	KBASE_DEBUG_ASSERT(data);

	kbdev = container_of(data, struct kbase_device, reset_work);

	KBASE_DEBUG_ASSERT(kbdev);
	js_devdata = &kbdev->js_data;

	KBASE_TRACE_ADD(kbdev, JM_BEGIN_RESET_WORKER, NULL, NULL, 0u, 0);

	/*                                                                                
                                                              */
	hrtimer_cancel(&kbdev->reset_timer);

	if (kbase_pm_context_active_handle_suspend(kbdev, KBASE_PM_SUSPEND_HANDLER_DONT_REACTIVATE)) {
		/*                                                                    
                      */
		atomic_set(&kbdev->reset_gpu, KBASE_RESET_GPU_NOT_PENDING);
		kbase_disjoint_state_down(kbdev);
		wake_up(&kbdev->reset_wait);
		return;
	}

	mutex_lock(&kbdev->pm.lock);
	/*                                                            */
	KBASE_DEBUG_ASSERT(kbdev->pm.pm_current_policy);

	/*                                                                                                               
                                                                                                                 */

	dev_err(kbdev->dev, "Resetting GPU (allowing up to %d ms)", RESET_TIMEOUT);

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);

	if (kbdev->hwcnt.state == KBASE_INSTR_STATE_RESETTING) {	/*                                            */
		/*                    */
		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
		wait_event(kbdev->hwcnt.wait, kbdev->hwcnt.triggered != 0);
		spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	}
	/*                            */
	if (kbdev->hwcnt.kctx != NULL) {
		struct kbase_context *kctx = kbdev->hwcnt.kctx;
		hwcnt_setup.dump_buffer = kbase_reg_read(kbdev, GPU_CONTROL_REG(PRFCNT_BASE_LO), kctx) & 0xffffffff;
		hwcnt_setup.dump_buffer |= (mali_addr64) kbase_reg_read(kbdev, GPU_CONTROL_REG(PRFCNT_BASE_HI), kctx) << 32;
		hwcnt_setup.jm_bm = kbase_reg_read(kbdev, GPU_CONTROL_REG(PRFCNT_JM_EN), kctx);
		hwcnt_setup.shader_bm = kbase_reg_read(kbdev, GPU_CONTROL_REG(PRFCNT_SHADER_EN), kctx);
		hwcnt_setup.tiler_bm = kbase_reg_read(kbdev, GPU_CONTROL_REG(PRFCNT_TILER_EN), kctx);
		hwcnt_setup.l3_cache_bm = kbase_reg_read(kbdev, GPU_CONTROL_REG(PRFCNT_L3_CACHE_EN), kctx);
		hwcnt_setup.mmu_l2_bm = kbase_reg_read(kbdev, GPU_CONTROL_REG(PRFCNT_MMU_L2_EN), kctx);
	}

	/*                                                              
                            */
	kbase_debug_dump_registers(kbdev);

	bckp_state = kbdev->hwcnt.state;
	kbdev->hwcnt.state = KBASE_INSTR_STATE_RESETTING;
	kbdev->hwcnt.triggered = 0;
	/*                                                                            
                                                */
	kbase_pm_disable_interrupts(kbdev);
	/*                                            */
	kbase_synchronize_irqs(kbdev);
	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);

	/*               */
	kbase_pm_init_hw(kbdev, MALI_TRUE);
	/*                                                                    */

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	/*                               */
	if (kbdev->hwcnt.kctx != NULL) {
		struct kbase_context *kctx = kbdev->hwcnt.kctx;
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_CONFIG), (kctx->as_nr << PRFCNT_CONFIG_AS_SHIFT) | PRFCNT_CONFIG_MODE_OFF, kctx);
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_BASE_LO),     hwcnt_setup.dump_buffer & 0xFFFFFFFF, kctx);
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_BASE_HI),     hwcnt_setup.dump_buffer >> 32,        kctx);
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_JM_EN),       hwcnt_setup.jm_bm,                    kctx);
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_SHADER_EN),   hwcnt_setup.shader_bm,                kctx);
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_L3_CACHE_EN), hwcnt_setup.l3_cache_bm,              kctx);
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_MMU_L2_EN),   hwcnt_setup.mmu_l2_bm,                kctx);

		/*                                                                                      */
		if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8186))
			kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_TILER_EN), 0, kctx);
		else
			kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_TILER_EN), hwcnt_setup.tiler_bm, kctx);

		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_CONFIG), (kctx->as_nr << PRFCNT_CONFIG_AS_SHIFT) | PRFCNT_CONFIG_MODE_MANUAL, kctx);

		/*                                                                      */
		if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8186))
			kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_TILER_EN), hwcnt_setup.tiler_bm, kctx);
	}
	kbdev->hwcnt.state = bckp_state;
	switch (kbdev->hwcnt.state) {
	/*                                                   */
	case KBASE_INSTR_STATE_CLEANED:
		/*                                        
                                            */
	case KBASE_INSTR_STATE_REQUEST_CLEAN:
		/*                                                               
                                            */
		wake_up(&kbdev->hwcnt.cache_clean_wait);
		break;
	case KBASE_INSTR_STATE_CLEANING:
		/*        
                                                               
                                                           
    
                                            */
		kbdev->hwcnt.state = KBASE_INSTR_STATE_CLEANED;
		wake_up(&kbdev->hwcnt.cache_clean_wait);
		break;

	/*                              */
	case KBASE_INSTR_STATE_DUMPING:
		/*                                                            */
		kbdev->hwcnt.state = KBASE_INSTR_STATE_IDLE;
		kbdev->hwcnt.triggered = 1;
		wake_up(&kbdev->hwcnt.wait);
		break;
	case KBASE_INSTR_STATE_DISABLED:
	case KBASE_INSTR_STATE_IDLE:
	case KBASE_INSTR_STATE_FAULT:
		/*                                          */
		kbdev->hwcnt.triggered = 1;
		wake_up(&kbdev->hwcnt.wait);
		break;

	/*                 */
	case KBASE_INSTR_STATE_RESETTING:
	default:
		BUG();
		break;
	}
	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);

	/*                                              */
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
	for (i = 0; i < kbdev->gpu_props.num_job_slots; i++) {
		int nr_done;
		struct kbase_jm_slot *slot = &kbdev->jm_slots[i];

		nr_done = kbasep_jm_nr_jobs_submitted(slot);
		while (nr_done) {
			dev_err(kbdev->dev, "Job stuck in slot %d on the GPU was cancelled", i);
			kbase_job_done_slot(kbdev, i, BASE_JD_EVENT_JOB_CANCELLED, 0, &end_timestamp);
			nr_done--;
		}
	}
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	mutex_lock(&js_devdata->runpool_mutex);

	/*                         */
	for (i = 0; i < BASE_MAX_NR_AS; i++) {
		if (js_devdata->runpool_irq.per_as_data[i].kctx) {
			struct kbase_as *as = &kbdev->as[i];
			mutex_lock(&as->transaction_mutex);
			kbase_mmu_update(js_devdata->runpool_irq.per_as_data[i].kctx);
			mutex_unlock(&as->transaction_mutex);
		}
	}

	atomic_set(&kbdev->reset_gpu, KBASE_RESET_GPU_NOT_PENDING);

	kbase_disjoint_state_down(kbdev);
	wake_up(&kbdev->reset_wait);
	dev_err(kbdev->dev, "Reset complete");

	/*                                      */
	kbase_pm_update_cores_state(kbdev);

	/*                                                           
                                                               */
	kbase_pm_check_transitions_sync(kbdev);

	/*                                                */
	if (js_devdata->nr_user_contexts_running > 0) {
		KBASE_TRACE_ADD(kbdev, JM_SUBMIT_AFTER_RESET, NULL, NULL, 0u, 0);

		spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
		kbasep_js_try_run_next_job_nolock(kbdev);
		spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
	}

	mutex_unlock(&js_devdata->runpool_mutex);
	mutex_unlock(&kbdev->pm.lock);

	kbase_pm_context_idle(kbdev);
	KBASE_TRACE_ADD(kbdev, JM_END_RESET_WORKER, NULL, NULL, 0u, 0);
}

enum hrtimer_restart kbasep_reset_timer_callback(struct hrtimer *timer)
{
	struct kbase_device *kbdev = container_of(timer, struct kbase_device, reset_timer);

	KBASE_DEBUG_ASSERT(kbdev);

	/*                      */
	if (atomic_cmpxchg(&kbdev->reset_gpu, KBASE_RESET_GPU_COMMITTED, KBASE_RESET_GPU_HAPPENING) == KBASE_RESET_GPU_COMMITTED)
		queue_work(kbdev->reset_workq, &kbdev->reset_work);

	return HRTIMER_NORESTART;
}

/*
                                                                 
                                                           
 */

static void kbasep_try_reset_gpu_early_locked(struct kbase_device *kbdev)
{
	int i;
	int pending_jobs = 0;

	KBASE_DEBUG_ASSERT(kbdev);

	/*                          */
	for (i = 0; i < kbdev->gpu_props.num_job_slots; i++) {
		struct kbase_jm_slot *slot = &kbdev->jm_slots[i];
		pending_jobs += kbasep_jm_nr_jobs_submitted(slot);
	}

	if (pending_jobs > 0) {
		/*                                        */
		return;
	}

	/*                                                                                                     
                                                  */
	if (atomic_cmpxchg(&kbdev->reset_gpu, KBASE_RESET_GPU_COMMITTED, KBASE_RESET_GPU_HAPPENING) != KBASE_RESET_GPU_COMMITTED) {
		/*                            */
		return;
	}
	queue_work(kbdev->reset_workq, &kbdev->reset_work);
}

static void kbasep_try_reset_gpu_early(struct kbase_device *kbdev)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;

	js_devdata = &kbdev->js_data;
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
	kbasep_try_reset_gpu_early_locked(kbdev);
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
}

/*
                                 
                                                                                                 
  
                                                                         
                                                                      
                                                                                             
  
                          
 */
mali_bool kbase_prepare_to_reset_gpu_locked(struct kbase_device *kbdev)
{
	int i;

	KBASE_DEBUG_ASSERT(kbdev);

	if (atomic_cmpxchg(&kbdev->reset_gpu, KBASE_RESET_GPU_NOT_PENDING, KBASE_RESET_GPU_PREPARED) != KBASE_RESET_GPU_NOT_PENDING) {
		/*                                                */
		return MALI_FALSE;
	}

	kbase_disjoint_state_up(kbdev);

	for (i = 0; i < kbdev->gpu_props.num_job_slots; i++)
		kbase_job_slot_softstop(kbdev, i, NULL);

	return MALI_TRUE;
}

mali_bool kbase_prepare_to_reset_gpu(struct kbase_device *kbdev)
{
	unsigned long flags;
	mali_bool ret;
	struct kbasep_js_device_data *js_devdata;

	js_devdata = &kbdev->js_data;
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
	ret = kbase_prepare_to_reset_gpu_locked(kbdev);
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	return ret;
}
KBASE_EXPORT_TEST_API(kbase_prepare_to_reset_gpu)

/*
                                                                                            
                                                                                        
  
                                                                                                   
                                                                                                      
 */
void kbase_reset_gpu(struct kbase_device *kbdev)
{
	u32 timeout_ms;

	KBASE_DEBUG_ASSERT(kbdev);

	/*                                                                                                 */
	KBASE_DEBUG_ASSERT(atomic_read(&kbdev->reset_gpu) == KBASE_RESET_GPU_PREPARED);
	atomic_set(&kbdev->reset_gpu, KBASE_RESET_GPU_COMMITTED);

	timeout_ms = kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_RESET_TIMEOUT_MS);
	dev_err(kbdev->dev, "Preparing to soft-reset GPU: Waiting (upto %d ms) for all jobs to complete soft-stop\n", timeout_ms);
	hrtimer_start(&kbdev->reset_timer, HR_TIMER_DELAY_MSEC(timeout_ms), HRTIMER_MODE_REL);

	/*                     */
	kbasep_try_reset_gpu_early(kbdev);
}
KBASE_EXPORT_TEST_API(kbase_reset_gpu)

void kbase_reset_gpu_locked(struct kbase_device *kbdev)
{
	u32 timeout_ms;

	KBASE_DEBUG_ASSERT(kbdev);

	/*                                                                                                 */
	KBASE_DEBUG_ASSERT(atomic_read(&kbdev->reset_gpu) == KBASE_RESET_GPU_PREPARED);
	atomic_set(&kbdev->reset_gpu, KBASE_RESET_GPU_COMMITTED);

	timeout_ms = kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_RESET_TIMEOUT_MS);
	dev_err(kbdev->dev, "Preparing to soft-reset GPU: Waiting (upto %d ms) for all jobs to complete soft-stop\n", timeout_ms);
	hrtimer_start(&kbdev->reset_timer, HR_TIMER_DELAY_MSEC(timeout_ms), HRTIMER_MODE_REL);

	/*                     */
	kbasep_try_reset_gpu_early_locked(kbdev);
}
#endif /*                    */
