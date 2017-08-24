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
#include <mali_kbase_js.h>
#include <mali_kbase_js_affinity.h>
#include <mali_kbase_gator.h>
#include <mali_kbase_hw.h>

#include "mali_kbase_jm.h"
#include <mali_kbase_defs.h>
#include <mali_kbase_config_defaults.h>

/*
                
 */

/*                                                          */
enum {
	/*                                                                         
                             */
	KBASEP_JS_RELEASE_RESULT_WAS_DESCHEDULED = (1u << 0),
};

typedef u32 kbasep_js_release_result;

/*
                              
 */
STATIC INLINE void kbasep_js_deref_permon_check_and_disable_cycle_counter(struct kbase_device *kbdev, struct kbase_jd_atom *katom);

STATIC INLINE void kbasep_js_ref_permon_check_and_enable_cycle_counter(struct kbase_device *kbdev, struct kbase_jd_atom *katom);

STATIC kbasep_js_release_result kbasep_js_runpool_release_ctx_internal(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbasep_js_atom_retained_state *katom_retained_state);

/*                            */
#if KBASE_TRACE_ENABLE
STATIC int kbasep_js_trace_get_refcnt(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;
	int as_nr;
	int refcnt = 0;

	js_devdata = &kbdev->js_data;

	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
	as_nr = kctx->as_nr;
	if (as_nr != KBASEP_AS_NR_INVALID) {
		struct kbasep_js_per_as_data *js_per_as_data;

		js_per_as_data = &js_devdata->runpool_irq.per_as_data[as_nr];

		refcnt = js_per_as_data->as_busy_refcount;
	}
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	return refcnt;
}
#else				/*                     */
STATIC int kbasep_js_trace_get_refcnt(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	CSTD_UNUSED(kbdev);
	CSTD_UNUSED(kctx);
	return 0;
}
#endif				/*                     */

/*
                
 */
enum {
	JS_DEVDATA_INIT_NONE = 0,
	JS_DEVDATA_INIT_CONSTANTS = (1 << 0),
	JS_DEVDATA_INIT_POLICY = (1 << 1),
	JS_DEVDATA_INIT_ALL = ((1 << 2) - 1)
};

enum {
	JS_KCTX_INIT_NONE = 0,
	JS_KCTX_INIT_CONSTANTS = (1 << 0),
	JS_KCTX_INIT_POLICY = (1 << 1),
	JS_KCTX_INIT_ALL = ((1 << 2) - 1)
};

/*
                    
 */

/* 
                                                                                                   
                                                                               
  
                                                             
  
                                
 */

STATIC INLINE void kbasep_js_deref_permon_check_and_disable_cycle_counter(struct kbase_device *kbdev, struct kbase_jd_atom *katom)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(katom != NULL);

	if (katom->core_req & BASE_JD_REQ_PERMON)
		kbase_pm_release_gpu_cycle_counter(kbdev);
}

/* 
                                                                                               
                                                                                           
  
                                                             
  
                                                       
  
                                
 */

STATIC INLINE void kbasep_js_ref_permon_check_and_enable_cycle_counter(struct kbase_device *kbdev, struct kbase_jd_atom *katom)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(katom != NULL);

	if (katom->core_req & BASE_JD_REQ_PERMON)
		kbase_pm_request_gpu_cycle_counter_l2_is_on(kbdev);
}

/*
                                                           
                                                                    
                                                                  
 */
STATIC INLINE void runpool_inc_context_count(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_kctx_info *js_kctx_info;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	js_devdata = &kbdev->js_data;
	js_kctx_info = &kctx->jctx.sched_info;

	BUG_ON(!mutex_is_locked(&js_kctx_info->ctx.jsctx_mutex));
	BUG_ON(!mutex_is_locked(&js_devdata->runpool_mutex));

	/*                      */
	KBASE_DEBUG_ASSERT(js_devdata->nr_all_contexts_running < S8_MAX);
	++(js_devdata->nr_all_contexts_running);

	if ((js_kctx_info->ctx.flags & KBASE_CTX_FLAG_SUBMIT_DISABLED) == 0) {
		/*                                     */
		KBASE_DEBUG_ASSERT(js_devdata->nr_user_contexts_running < S8_MAX);
		++(js_devdata->nr_user_contexts_running);
	}
}

/*
                                                           
                                                                    
                                                                  
 */
STATIC INLINE void runpool_dec_context_count(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_kctx_info *js_kctx_info;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	js_devdata = &kbdev->js_data;
	js_kctx_info = &kctx->jctx.sched_info;

	BUG_ON(!mutex_is_locked(&js_kctx_info->ctx.jsctx_mutex));
	BUG_ON(!mutex_is_locked(&js_devdata->runpool_mutex));

	/*                      */
	--(js_devdata->nr_all_contexts_running);
	KBASE_DEBUG_ASSERT(js_devdata->nr_all_contexts_running >= 0);

	if ((js_kctx_info->ctx.flags & KBASE_CTX_FLAG_SUBMIT_DISABLED) == 0) {
		/*                                     */
		--(js_devdata->nr_user_contexts_running);
		KBASE_DEBUG_ASSERT(js_devdata->nr_user_contexts_running >= 0);
	}
}

/* 
                                                                   
  
                                                                      
                                                                            
                                             
  
                                                                          
                                                               
  
                                                           
                                                                            
                                                                                      
                                                                                                                  
 */
STATIC mali_bool check_is_runpool_full(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_device_data *js_devdata;
	mali_bool is_runpool_full;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	js_devdata = &kbdev->js_data;
	BUG_ON(!mutex_is_locked(&js_devdata->runpool_mutex));

	/*                                                                                 
                          */
	is_runpool_full = (mali_bool) (js_devdata->nr_all_contexts_running >= kbdev->nr_hw_address_spaces);

	if (kctx != NULL && (kctx->jctx.sched_info.ctx.flags & KBASE_CTX_FLAG_SUBMIT_DISABLED) == 0) {
		BUG_ON(!mutex_is_locked(&kctx->jctx.sched_info.ctx.jsctx_mutex));
		/*                                                                               
                                                                             
                                                                          
    
                                                                                       
                                                                                       */
		is_runpool_full |= (mali_bool) (js_devdata->nr_user_contexts_running >= kbdev->nr_user_address_spaces);
	}

	return is_runpool_full;
}

STATIC base_jd_core_req core_reqs_from_jsn_features(u16 features) /*                              */
{
	base_jd_core_req core_req = 0u;

	if ((features & JS_FEATURE_SET_VALUE_JOB) != 0)
		core_req |= BASE_JD_REQ_V;

	if ((features & JS_FEATURE_CACHE_FLUSH_JOB) != 0)
		core_req |= BASE_JD_REQ_CF;

	if ((features & JS_FEATURE_COMPUTE_JOB) != 0)
		core_req |= BASE_JD_REQ_CS;

	if ((features & JS_FEATURE_TILER_JOB) != 0)
		core_req |= BASE_JD_REQ_T;

	if ((features & JS_FEATURE_FRAGMENT_JOB) != 0)
		core_req |= BASE_JD_REQ_FS;

	return core_req;
}

/* 
                                       
  
                                                                         
                                                             
  
                                                                               
  
                                                           
                                                      
  
                                                                                          
 */
STATIC struct kbase_as *pick_free_addr_space(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbase_as *current_as;
	long ffs_result;

	js_devdata = &kbdev->js_data;

	lockdep_assert_held(&js_devdata->runpool_mutex);

	/*                             */
	ffs_result = ffs(js_devdata->as_free) - 1;

	/*                                           */
	KBASE_DEBUG_ASSERT(0 <= ffs_result && ffs_result < kbdev->nr_hw_address_spaces);
	/*                                   */
	js_devdata->as_free &= ~((u16) (1u << ffs_result));

	current_as = &kbdev->as[ffs_result];

	return current_as;
}

/* 
                                                                        
  
                                                           
                                                      
 */
STATIC INLINE void release_addr_space(struct kbase_device *kbdev, int kctx_as_nr)
{
	struct kbasep_js_device_data *js_devdata;
	u16 as_bit = (1u << kctx_as_nr);

	js_devdata = &kbdev->js_data;
	lockdep_assert_held(&js_devdata->runpool_mutex);

	/*                                            */
	KBASE_DEBUG_ASSERT(!(js_devdata->as_free & as_bit));

	js_devdata->as_free |= as_bit;
}

/* 
                                                                                
  
                 
                                                                          
                                 
                                            
  
                      
                                                          
                                                              
                                          
                                      
 */
STATIC void assign_and_activate_kctx_addr_space(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_as *current_as)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_per_as_data *js_per_as_data;
	int as_nr;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(current_as != NULL);

	js_devdata = &kbdev->js_data;
	as_nr = current_as->number;

	lockdep_assert_held(&kctx->jctx.sched_info.ctx.jsctx_mutex);
	lockdep_assert_held(&js_devdata->runpool_mutex);
	lockdep_assert_held(&current_as->transaction_mutex);
	lockdep_assert_held(&js_devdata->runpool_irq.lock);

	js_per_as_data = &js_devdata->runpool_irq.per_as_data[as_nr];

	/*                    */
	kbasep_js_ctx_attr_runpool_retain_ctx(kbdev, kctx);

	/*                   */
	kctx->as_nr = as_nr;
#ifdef CONFIG_MALI_GATOR_SUPPORT
	kbase_trace_mali_mmu_as_in_use(kctx->as_nr);
#endif				/*                           */
	/*                                        */
	kbase_mmu_update(kctx);

	/*                      */
	kbasep_js_set_submit_allowed(js_devdata, kctx);

	/*              */
	js_per_as_data->kctx = kctx;
	js_per_as_data->as_busy_refcount = 0;

	/*                                                                                     */
	kbasep_js_policy_runpool_add_ctx(&js_devdata->policy, kctx);
}

void kbasep_js_try_run_next_job_nolock(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	int js;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;
	lockdep_assert_held(&js_devdata->runpool_mutex);
	lockdep_assert_held(&js_devdata->runpool_irq.lock);

	/*                                                                    
                                                                           
              */
	if (js_devdata->nr_user_contexts_running == 0)
		return; /*                                                                    */

	for (js = 0; js < kbdev->gpu_props.num_job_slots; ++js)
		kbasep_js_try_run_next_job_on_slot_nolock(kbdev, js);
}

/*                                                             */
mali_bool kbasep_js_runpool_retain_ctx_nolock(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_per_as_data *js_per_as_data;
	mali_bool result = MALI_FALSE;
	int as_nr;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	js_devdata = &kbdev->js_data;

	as_nr = kctx->as_nr;
	if (as_nr != KBASEP_AS_NR_INVALID) {
		int new_refcnt;

		KBASE_DEBUG_ASSERT(as_nr >= 0);
		js_per_as_data = &js_devdata->runpool_irq.per_as_data[as_nr];

		KBASE_DEBUG_ASSERT(js_per_as_data->kctx != NULL);

		new_refcnt = ++(js_per_as_data->as_busy_refcount);
		KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_RETAIN_CTX_NOLOCK, kctx, NULL, 0u, new_refcnt);
		result = MALI_TRUE;
	}

	return result;
}

/*
                                                     
 */
void kbase_js_try_run_jobs(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	unsigned long flags;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;

	mutex_lock(&js_devdata->runpool_mutex);
	if (js_devdata->nr_user_contexts_running != 0) {
		/*                                                                                               */
		spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

		kbasep_js_try_run_next_job_nolock(kbdev);

		spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
	}
	mutex_unlock(&js_devdata->runpool_mutex);
}

void kbase_js_try_run_jobs_on_slot(struct kbase_device *kbdev, int js)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;

	mutex_lock(&js_devdata->runpool_mutex);
	if (js_devdata->nr_user_contexts_running != 0) {
		/*                                                                                               */
		spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

		kbasep_js_try_run_next_job_on_slot_nolock(kbdev, js);

		spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
	}
	mutex_unlock(&js_devdata->runpool_mutex);
}

mali_error kbasep_js_devdata_init(struct kbase_device * const kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	mali_error err;
	int i;
	u16 as_present;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	js_devdata = &kbdev->js_data;

	KBASE_DEBUG_ASSERT(js_devdata->init_status == JS_DEVDATA_INIT_NONE);

	/*                                                                                          */
	as_present = (1U << kbdev->nr_hw_address_spaces) - 1;
	kbdev->nr_user_address_spaces = kbdev->nr_hw_address_spaces;
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8987)) {
		mali_bool use_workaround_for_security;

		use_workaround_for_security = DEFAULT_SECURE_BUT_LOSS_OF_PERFORMANCE;
		if (use_workaround_for_security != MALI_FALSE) {
			dev_dbg(kbdev->dev, "GPU has HW ISSUE 8987, and driver configured for security workaround: 1 address space only");
			kbdev->nr_user_address_spaces = 1;
		}
	}
#ifdef CONFIG_MALI_DEBUG
	/*                                                                                         */
	js_devdata->softstop_always = MALI_FALSE;
#endif				/*                   */
	js_devdata->nr_all_contexts_running = 0;
	js_devdata->nr_user_contexts_running = 0;
	js_devdata->as_free = as_present;	/*                        */
	js_devdata->runpool_irq.submit_allowed = 0u;	/*                          */
	memset(js_devdata->runpool_irq.ctx_attr_ref_count, 0, sizeof(js_devdata->runpool_irq.ctx_attr_ref_count));
	memset(js_devdata->runpool_irq.slot_affinities, 0, sizeof(js_devdata->runpool_irq.slot_affinities));
	js_devdata->runpool_irq.slots_blocked_on_affinity = 0u;
	memset(js_devdata->runpool_irq.slot_affinity_refcount, 0, sizeof(js_devdata->runpool_irq.slot_affinity_refcount));
	INIT_LIST_HEAD(&js_devdata->suspended_soft_jobs_list);

	/*                   */
	js_devdata->scheduling_tick_ns = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_SCHEDULING_TICK_NS);
	js_devdata->soft_stop_ticks = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS);
	js_devdata->soft_stop_ticks_cl = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS_CL);
	js_devdata->hard_stop_ticks_ss = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_SS);
	js_devdata->hard_stop_ticks_cl = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_CL);
	js_devdata->hard_stop_ticks_nss = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_NSS);
	js_devdata->gpu_reset_ticks_ss = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_RESET_TICKS_SS);
	js_devdata->gpu_reset_ticks_cl = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_RESET_TICKS_CL);
	js_devdata->gpu_reset_ticks_nss = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_RESET_TICKS_NSS);
	js_devdata->ctx_timeslice_ns = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_JS_CTX_TIMESLICE_NS);
	js_devdata->cfs_ctx_runtime_init_slices = DEFAULT_JS_CFS_CTX_RUNTIME_INIT_SLICES;
	js_devdata->cfs_ctx_runtime_min_slices = DEFAULT_JS_CFS_CTX_RUNTIME_MIN_SLICES;

	dev_dbg(kbdev->dev, "JS Config Attribs: ");
	dev_dbg(kbdev->dev, "\tscheduling_tick_ns:%u", js_devdata->scheduling_tick_ns);
	dev_dbg(kbdev->dev, "\tsoft_stop_ticks:%u", js_devdata->soft_stop_ticks);
	dev_dbg(kbdev->dev, "\tsoft_stop_ticks_cl:%u", js_devdata->soft_stop_ticks_cl);
	dev_dbg(kbdev->dev, "\thard_stop_ticks_ss:%u", js_devdata->hard_stop_ticks_ss);
	dev_dbg(kbdev->dev, "\thard_stop_ticks_cl:%u", js_devdata->hard_stop_ticks_cl);
	dev_dbg(kbdev->dev, "\thard_stop_ticks_nss:%u", js_devdata->hard_stop_ticks_nss);
	dev_dbg(kbdev->dev, "\tgpu_reset_ticks_ss:%u", js_devdata->gpu_reset_ticks_ss);
	dev_dbg(kbdev->dev, "\tgpu_reset_ticks_cl:%u", js_devdata->gpu_reset_ticks_cl);
	dev_dbg(kbdev->dev, "\tgpu_reset_ticks_nss:%u", js_devdata->gpu_reset_ticks_nss);
	dev_dbg(kbdev->dev, "\tctx_timeslice_ns:%u", js_devdata->ctx_timeslice_ns);
	dev_dbg(kbdev->dev, "\tcfs_ctx_runtime_init_slices:%u", js_devdata->cfs_ctx_runtime_init_slices);
	dev_dbg(kbdev->dev, "\tcfs_ctx_runtime_min_slices:%u", js_devdata->cfs_ctx_runtime_min_slices);

#if KBASE_DISABLE_SCHEDULING_SOFT_STOPS
	dev_dbg(kbdev->dev, "Job Scheduling Policy Soft-stops disabled, ignoring value for soft_stop_ticks==%u at %uns per tick. Other soft-stops may still occur.", js_devdata->soft_stop_ticks, js_devdata->scheduling_tick_ns);
#endif
#if KBASE_DISABLE_SCHEDULING_HARD_STOPS
	dev_dbg(kbdev->dev, "Job Scheduling Policy Hard-stops disabled, ignoring values for hard_stop_ticks_ss==%d and hard_stop_ticks_nss==%u at %uns per tick. Other hard-stops may still occur.", js_devdata->hard_stop_ticks_ss, js_devdata->hard_stop_ticks_nss, js_devdata->scheduling_tick_ns);
#endif
#if KBASE_DISABLE_SCHEDULING_SOFT_STOPS && KBASE_DISABLE_SCHEDULING_HARD_STOPS
	dev_dbg(kbdev->dev, "Note: The JS policy's tick timer (if coded) will still be run, but do nothing.");
#endif

	/*                                                            */
	{
		int irq_throttle_time_us = kbdev->gpu_props.irq_throttle_time_us;
		int irq_throttle_cycles = kbasep_js_convert_us_to_gpu_ticks_max_freq(kbdev, irq_throttle_time_us);

		atomic_set(&kbdev->irq_throttle_cycles, irq_throttle_cycles);
	}

	/*                                                    */
	memset(&js_devdata->runpool_irq.per_as_data[0], 0, sizeof(js_devdata->runpool_irq.per_as_data));

	for (i = 0; i < kbdev->gpu_props.num_job_slots; ++i)
		js_devdata->js_reqs[i] = core_reqs_from_jsn_features(kbdev->gpu_props.props.raw_props.js_features[i]);

	js_devdata->init_status |= JS_DEVDATA_INIT_CONSTANTS;

	/*                                                                      
                           */

	mutex_init(&js_devdata->runpool_mutex);
	mutex_init(&js_devdata->queue_mutex);
	spin_lock_init(&js_devdata->runpool_irq.lock);

	err = kbasep_js_policy_init(kbdev);
	if (err == MALI_ERROR_NONE)
		js_devdata->init_status |= JS_DEVDATA_INIT_POLICY;

	/*                                                                      
                                                                      */
	if (js_devdata->init_status != JS_DEVDATA_INIT_ALL)
		return MALI_ERROR_FUNCTION_FAILED;

	return MALI_ERROR_NONE;
}

void kbasep_js_devdata_halt(struct kbase_device *kbdev)
{
	CSTD_UNUSED(kbdev);
}

void kbasep_js_devdata_term(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	js_devdata = &kbdev->js_data;

	if ((js_devdata->init_status & JS_DEVDATA_INIT_CONSTANTS)) {
		s8 zero_ctx_attr_ref_count[KBASEP_JS_CTX_ATTR_COUNT] = { 0, };
		/*                                                              */
		KBASE_DEBUG_ASSERT(js_devdata->nr_all_contexts_running == 0);
		KBASE_DEBUG_ASSERT(memcmp(js_devdata->runpool_irq.ctx_attr_ref_count, zero_ctx_attr_ref_count, sizeof(js_devdata->runpool_irq.ctx_attr_ref_count)) == 0);
		CSTD_UNUSED(zero_ctx_attr_ref_count);
	}
	if ((js_devdata->init_status & JS_DEVDATA_INIT_POLICY))
		kbasep_js_policy_term(&js_devdata->policy);

	js_devdata->init_status = JS_DEVDATA_INIT_NONE;
}

mali_error kbasep_js_kctx_init(struct kbase_context * const kctx)
{
	struct kbase_device *kbdev;
	struct kbasep_js_kctx_info *js_kctx_info;
	mali_error err;

	KBASE_DEBUG_ASSERT(kctx != NULL);

	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(kbdev != NULL);

	js_kctx_info = &kctx->jctx.sched_info;
	KBASE_DEBUG_ASSERT(js_kctx_info->init_status == JS_KCTX_INIT_NONE);

	js_kctx_info->ctx.nr_jobs = 0;
	js_kctx_info->ctx.is_scheduled = MALI_FALSE;
	js_kctx_info->ctx.is_dying = MALI_FALSE;
	memset(js_kctx_info->ctx.ctx_attr_ref_count, 0, sizeof(js_kctx_info->ctx.ctx_attr_ref_count));

	/*                                                                                   */
	js_kctx_info->ctx.flags = KBASE_CTX_FLAG_SUBMIT_DISABLED;

	js_kctx_info->init_status |= JS_KCTX_INIT_CONSTANTS;

	/*                                                                      
                           */
	mutex_init(&js_kctx_info->ctx.jsctx_mutex);

	init_waitqueue_head(&js_kctx_info->ctx.is_scheduled_wait);

	err = kbasep_js_policy_init_ctx(kbdev, kctx);
	if (err == MALI_ERROR_NONE)
		js_kctx_info->init_status |= JS_KCTX_INIT_POLICY;

	/*                                                                      
                                                                      */
	if (js_kctx_info->init_status != JS_KCTX_INIT_ALL)
		return MALI_ERROR_FUNCTION_FAILED;

	return MALI_ERROR_NONE;
}

void kbasep_js_kctx_term(struct kbase_context *kctx)
{
	struct kbase_device *kbdev;
	struct kbasep_js_kctx_info *js_kctx_info;
	union kbasep_js_policy *js_policy;

	KBASE_DEBUG_ASSERT(kctx != NULL);

	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(kbdev != NULL);

	js_policy = &kbdev->js_data.policy;
	js_kctx_info = &kctx->jctx.sched_info;

	if ((js_kctx_info->init_status & JS_KCTX_INIT_CONSTANTS)) {
		/*                                                          */
		KBASE_DEBUG_ASSERT(js_kctx_info->ctx.is_scheduled == MALI_FALSE);
		KBASE_DEBUG_ASSERT(js_kctx_info->ctx.nr_jobs == 0);
	}

	if ((js_kctx_info->init_status & JS_KCTX_INIT_POLICY))
		kbasep_js_policy_term_ctx(js_policy, kctx);

	js_kctx_info->init_status = JS_KCTX_INIT_NONE;
}

/*                                   
  
                        
                                          
                                         
 */
STATIC void kbasep_js_runpool_evict_next_jobs(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	unsigned long flags;
	int js;
	struct kbasep_js_device_data *js_devdata;

	js_devdata = &kbdev->js_data;

	BUG_ON(!mutex_is_locked(&kctx->jctx.sched_info.ctx.jsctx_mutex));
	BUG_ON(!mutex_is_locked(&js_devdata->runpool_mutex));

	/*                                                      */
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

	/*                                                                         
                                                                 
                     */

	/*                                    */
	for (js = 0; js < kbdev->gpu_props.num_job_slots; js++) {
		struct kbase_jm_slot *slot;
		struct kbase_jd_atom *tail;

		if (!kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_COMMAND_NEXT), NULL)) {
			/*                             */
			continue;
		}

		slot = &kbdev->jm_slots[js];
		tail = kbasep_jm_peek_idx_submit_slot(slot, slot->submitted_nr - 1);

		KBASE_TIMELINE_TRY_SOFT_STOP(kctx, js, 1);
		/*                                  */
		kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_COMMAND_NEXT), JS_COMMAND_NOP, NULL);

		/*                                                             */
		if (kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_LO), NULL) != 0 || kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_HI), NULL) != 0) {
			/*                                                                      */
			struct kbase_jd_atom *dequeued_katom = kbasep_jm_dequeue_tail_submit_slot(slot);

			KBASE_DEBUG_ASSERT(dequeued_katom == tail);

			/*                                */
			kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_LO), 0, NULL);
			kbase_reg_write(kbdev, JOB_SLOT_REG(js, JS_HEAD_NEXT_HI), 0, NULL);

			KBASE_TRACE_ADD_SLOT(kbdev, JM_SLOT_EVICT, dequeued_katom->kctx, dequeued_katom, dequeued_katom->jc, js);

			/*                                                                 
                */
			kbase_jd_done(dequeued_katom, js, NULL, KBASE_JS_ATOM_DONE_EVICTED_FROM_NEXT);
		}
		KBASE_TIMELINE_TRY_SOFT_STOP(kctx, js, 0);
	}
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
}

/* 
                                   
                                                                           
                                   
  
                                                                                 
  
                                                           
                                              
                                                                         
                                                                          
                                                           
                                                                                   
                       
                                                                               
                       
                                                                              
               
 */
STATIC void kbasep_js_runpool_attempt_fast_start_ctx(struct kbase_device *kbdev, struct kbase_context *kctx_new)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_kctx_info *js_kctx_new;
	union kbasep_js_policy *js_policy;
	struct kbasep_js_per_as_data *js_per_as_data;
	int evict_as_nr;
	struct kbasep_js_atom_retained_state katom_retained_state;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	js_devdata = &kbdev->js_data;
	js_policy = &kbdev->js_data.policy;

	if (kctx_new != NULL) {
		js_kctx_new = &kctx_new->jctx.sched_info;
		mutex_lock(&js_kctx_new->ctx.jsctx_mutex);
	} else {
		js_kctx_new = NULL;
		CSTD_UNUSED(js_kctx_new);
	}

	/*                                    */
	kbasep_js_atom_retained_state_init_invalid(&katom_retained_state);

	mutex_lock(&js_devdata->runpool_mutex);

	/*                                                                                                            
                                                                                                 */
	if (check_is_runpool_full(kbdev, kctx_new) &&
			(!js_kctx_new || (js_kctx_new &&
			!js_kctx_new->ctx.is_dying))) {
		/*                                                                              */
		spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
		for (evict_as_nr = 0; evict_as_nr < kbdev->nr_hw_address_spaces; evict_as_nr++) {
			struct kbase_context *kctx_evict;

			js_per_as_data = &js_devdata->runpool_irq.per_as_data[evict_as_nr];
			kctx_evict = js_per_as_data->kctx;

			/*                                                */
			if (0 == js_per_as_data->as_busy_refcount && kctx_evict != NULL) {
				/*                                                                                         
                                                                                         
                                                              
     */
				if ((kctx_new == NULL) || kbasep_js_policy_ctx_has_priority(js_policy, kctx_evict, kctx_new)) {
					mali_bool retain_result;
					kbasep_js_release_result release_result;

					KBASE_TRACE_ADD(kbdev, JS_FAST_START_EVICTS_CTX, kctx_evict, NULL, 0u, (uintptr_t)kctx_new);

					/*                                                               */
					retain_result = kbasep_js_runpool_retain_ctx_nolock(kbdev, kctx_evict);
					KBASE_DEBUG_ASSERT(retain_result != MALI_FALSE);
					CSTD_UNUSED(retain_result);

					/*                                                                                   
                                              */
					kbasep_js_clear_submit_allowed(js_devdata, kctx_evict);

					spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
					mutex_unlock(&js_devdata->runpool_mutex);
					if (kctx_new != NULL)
						mutex_unlock(&js_kctx_new->ctx.jsctx_mutex);

					/*                                                                             */

					mutex_lock(&kctx_evict->jctx.sched_info.ctx.jsctx_mutex);
					mutex_lock(&js_devdata->runpool_mutex);
					release_result = kbasep_js_runpool_release_ctx_internal(kbdev, kctx_evict, &katom_retained_state);
					mutex_unlock(&js_devdata->runpool_mutex);
					/*                                                                   
                                                                         
                                                  */
					if ((release_result & KBASEP_JS_RELEASE_RESULT_WAS_DESCHEDULED) != 0u)
						kbasep_js_runpool_requeue_or_kill_ctx(kbdev, kctx_evict, MALI_TRUE);

					mutex_unlock(&kctx_evict->jctx.sched_info.ctx.jsctx_mutex);

					/*                                         
                                                           
                                                */

					/*                                */
					return;
				}
			}
		}
		spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
	}

	/*                                    */
	mutex_unlock(&js_devdata->runpool_mutex);
	if (kctx_new != NULL)
		mutex_unlock(&js_kctx_new->ctx.jsctx_mutex);
}

mali_bool kbasep_js_add_job(struct kbase_context *kctx, struct kbase_jd_atom *atom)
{
	unsigned long flags;
	struct kbasep_js_kctx_info *js_kctx_info;
	struct kbase_device *kbdev;
	struct kbasep_js_device_data *js_devdata;
	union kbasep_js_policy *js_policy;

	mali_bool policy_queue_updated = MALI_FALSE;

	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(atom != NULL);
	lockdep_assert_held(&kctx->jctx.lock);

	kbdev = kctx->kbdev;
	js_devdata = &kbdev->js_data;
	js_policy = &kbdev->js_data.policy;
	js_kctx_info = &kctx->jctx.sched_info;

	KBASE_TIMELINE_ATOM_READY(kctx, kbase_jd_atom_id(kctx, atom));

	mutex_lock(&js_kctx_info->ctx.jsctx_mutex);
	/*                                                                           
                                                                           */
	kbasep_js_policy_register_job(js_policy, kctx, atom);

	/*
                             
  */
	mutex_lock(&js_devdata->runpool_mutex);
	KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_ADD_JOB, kctx, atom, atom->jc, kbasep_js_trace_get_refcnt(kbdev, kctx));

	/*                      */
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.nr_jobs < U32_MAX);
	++(js_kctx_info->ctx.nr_jobs);

	/*                                  */
	kbasep_js_clear_job_retry_submit(atom);

	/*                                     */
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

	/*                               */
	kbasep_js_ctx_attr_ctx_retain_atom(kbdev, kctx, atom);

	/*                                                                 
                                  */
	kbasep_js_policy_enqueue_job(js_policy, atom);

	if (js_kctx_info->ctx.is_scheduled != MALI_FALSE) {
		/*                                                                       
                                                                         
          */
		kbasep_js_try_run_next_job_nolock(kbdev);
	}
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
	mutex_unlock(&js_devdata->runpool_mutex);
	/*                         */

	if (js_kctx_info->ctx.is_scheduled == MALI_FALSE) {
		if (js_kctx_info->ctx.is_dying) {
			/*                                                               
                                                                     
                                       */
			kbasep_js_runpool_requeue_or_kill_ctx(kbdev, kctx, MALI_FALSE);
		} else if (js_kctx_info->ctx.nr_jobs == 1) {
			/*                                                                             */
			KBASE_DEBUG_ASSERT(js_kctx_info->ctx.is_scheduled == MALI_FALSE);
			dev_dbg(kbdev->dev, "JS: Enqueue Context %p", kctx);

			mutex_lock(&js_devdata->queue_mutex);
			kbasep_js_policy_enqueue_ctx(js_policy, kctx);
			mutex_unlock(&js_devdata->queue_mutex);

			/*                                                                        
                                                       */
			policy_queue_updated = MALI_TRUE;
		}
	}
	mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);

	/*                                                                   
                                                                         
                                                                         
                                     
   
                                                                           
                                                                           
                                     */
	if (policy_queue_updated)
		kbasep_js_runpool_attempt_fast_start_ctx(kbdev, kctx);

	return policy_queue_updated;
}

void kbasep_js_remove_job(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_jd_atom *atom)
{
	struct kbasep_js_kctx_info *js_kctx_info;
	struct kbasep_js_device_data *js_devdata;
	union kbasep_js_policy *js_policy;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(atom != NULL);

	js_devdata = &kbdev->js_data;
	js_policy = &kbdev->js_data.policy;
	js_kctx_info = &kctx->jctx.sched_info;

	KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_REMOVE_JOB, kctx, atom, atom->jc, kbasep_js_trace_get_refcnt(kbdev, kctx));

	/*                         */
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.nr_jobs > 0);
	--(js_kctx_info->ctx.nr_jobs);

	/*                                     */
	kbasep_js_policy_deregister_job(js_policy, kctx, atom);
}

void kbasep_js_remove_cancelled_job(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_jd_atom *katom)
{
	unsigned long flags;
	struct kbasep_js_atom_retained_state katom_retained_state;
	struct kbasep_js_device_data *js_devdata;
	mali_bool attr_state_changed;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(katom != NULL);

	js_devdata = &kbdev->js_data;

	kbasep_js_atom_retained_state_copy(&katom_retained_state, katom);
	kbasep_js_remove_job(kbdev, kctx, katom);

	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

	/*                                                                 
                                  
   
                                                                        
                                                                         
                                       */
	attr_state_changed = kbasep_js_ctx_attr_ctx_release_atom(kbdev, kctx, &katom_retained_state);

	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	if (attr_state_changed != MALI_FALSE) {
		/*                                                                   
                  */
		kbase_js_try_run_jobs(kbdev);
	}
}

mali_bool kbasep_js_runpool_retain_ctx(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;
	mali_bool result;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;

	/*                                                               
                                              */
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
	result = kbasep_js_runpool_retain_ctx_nolock(kbdev, kctx);
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	return result;
}

struct kbase_context *kbasep_js_runpool_lookup_ctx(struct kbase_device *kbdev, int as_nr)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;
	struct kbase_context *found_kctx = NULL;
	struct kbasep_js_per_as_data *js_per_as_data;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(0 <= as_nr && as_nr < BASE_MAX_NR_AS);
	js_devdata = &kbdev->js_data;
	js_per_as_data = &js_devdata->runpool_irq.per_as_data[as_nr];

	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

	found_kctx = js_per_as_data->kctx;

	if (found_kctx != NULL)
		++(js_per_as_data->as_busy_refcount);

	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	return found_kctx;
}

/* 
                                                                     
  
                                                         
                                                    
  
                                        
                                                               
                                                    
                                                               
                                          
 */
STATIC void kbasep_js_run_jobs_after_ctx_and_atom_release(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbasep_js_atom_retained_state *katom_retained_state, mali_bool runpool_ctx_attr_change)
{
	struct kbasep_js_device_data *js_devdata;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(katom_retained_state != NULL);
	js_devdata = &kbdev->js_data;

	lockdep_assert_held(&kctx->jctx.sched_info.ctx.jsctx_mutex);
	lockdep_assert_held(&js_devdata->runpool_mutex);
	lockdep_assert_held(&js_devdata->runpool_irq.lock);

	if (js_devdata->nr_user_contexts_running != 0) {
		mali_bool retry_submit;
		int retry_jobslot;

		retry_submit = kbasep_js_get_atom_retry_submit_slot(katom_retained_state, &retry_jobslot);

		if (runpool_ctx_attr_change != MALI_FALSE) {
			/*                                                                   
                   */
			kbasep_js_try_run_next_job_nolock(kbdev);

			/*                                                                            */
			retry_submit = MALI_FALSE;
		}

		/*                                                                       
                      
    
                                                              */
		kbase_js_affinity_submit_to_blocked_slots(kbdev);

		/*                                                                       
                            
                                                                        */
		if (retry_submit != MALI_FALSE) {
			KBASE_TRACE_ADD_SLOT(kbdev, JD_DONE_TRY_RUN_NEXT_JOB, kctx, NULL, 0u, retry_jobslot);
			kbasep_js_try_run_next_job_on_slot_nolock(kbdev, retry_jobslot);
		}
	}
}

/* 
                                                                              
                                                             
  
                                                                                  
  
                                                         
                                          
                                                 
  
                                                                     
  
            
                                                                
                                    
                                               
                                           
 */
STATIC kbasep_js_release_result kbasep_js_runpool_release_ctx_internal(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbasep_js_atom_retained_state *katom_retained_state)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_kctx_info *js_kctx_info;
	union kbasep_js_policy *js_policy;
	struct kbasep_js_per_as_data *js_per_as_data;

	kbasep_js_release_result release_result = 0u;
	mali_bool runpool_ctx_attr_change = MALI_FALSE;
	int kctx_as_nr;
	struct kbase_as *current_as;
	int new_ref_count;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	js_kctx_info = &kctx->jctx.sched_info;
	js_devdata = &kbdev->js_data;
	js_policy = &kbdev->js_data.policy;

	/*                                       */
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.is_scheduled != MALI_FALSE);

	/*                                                                     
                                                                     
   
                                                                        
                                                  
  */
	kctx_as_nr = kctx->as_nr;
	KBASE_DEBUG_ASSERT(kctx_as_nr != KBASEP_AS_NR_INVALID);
	js_per_as_data = &js_devdata->runpool_irq.per_as_data[kctx_as_nr];
	KBASE_DEBUG_ASSERT(js_per_as_data->as_busy_refcount > 0);

	/*
                                            
   
                                     
  */
	current_as = &kbdev->as[kctx_as_nr];
	mutex_lock(&current_as->transaction_mutex);
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
	KBASE_DEBUG_ASSERT(kctx_as_nr == kctx->as_nr);
	KBASE_DEBUG_ASSERT(js_per_as_data->as_busy_refcount > 0);

	/*                 */
	new_ref_count = --(js_per_as_data->as_busy_refcount);

	/*                                                            */
	if (kbasep_js_has_atom_finished(katom_retained_state) != MALI_FALSE)
		runpool_ctx_attr_change |= kbasep_js_ctx_attr_ctx_release_atom(kbdev, kctx, katom_retained_state);

	KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_RELEASE_CTX, kctx, NULL, 0u, new_ref_count);

	if (new_ref_count == 1 && kctx->jctx.sched_info.ctx.flags & KBASE_CTX_FLAG_PRIVILEGED &&
			!kbase_pm_is_suspending(kbdev)) {
		/*                                                                                                  
                                                                                                      
    
                                                                               
   */
		kbasep_js_set_submit_allowed(js_devdata, kctx);
	}

	/*                                                                    */
	if (new_ref_count == 0 && (kctx->jctx.sched_info.ctx.nr_jobs == 0 || kbasep_js_is_submit_allowed(js_devdata, kctx) == MALI_FALSE)) {
		/*                                                                              */
		dev_dbg(kbdev->dev, "JS: RunPool Remove Context %p because as_busy_refcount=%d, jobs=%d, allowed=%d", kctx, new_ref_count, js_kctx_info->ctx.nr_jobs, kbasep_js_is_submit_allowed(js_devdata, kctx));

		kbasep_js_policy_runpool_remove_ctx(js_policy, kctx);

		/*                                                 */
		js_per_as_data->kctx = NULL;

		/*                                                           
                                                                */
		kbasep_js_clear_submit_allowed(js_devdata, kctx);

		/*                                                                          */
		kbase_mmu_disable(kctx);

#ifdef CONFIG_MALI_GATOR_SUPPORT
		kbase_trace_mali_mmu_as_released(kctx->as_nr);
#endif				/*                           */

		kctx->as_nr = KBASEP_AS_NR_INVALID;

		/*                       
    
                                                                        
                                                                          */
		runpool_ctx_attr_change |= kbasep_js_ctx_attr_runpool_release_ctx(kbdev, kctx);

		/*                                               
                                                          */
		runpool_dec_context_count(kbdev, kctx);

		/*                                                                           */
		kbasep_js_run_jobs_after_ctx_and_atom_release(kbdev, kctx, katom_retained_state, runpool_ctx_attr_change);

		/*
                                            
    
                                                                          
    
                                                                                    
                                                                                   
                       
   */
		spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
		mutex_unlock(&current_as->transaction_mutex);

		/*                           */
		release_addr_space(kbdev, kctx_as_nr);
		/*                                  */

		/*                                    */
		kbasep_js_policy_runpool_timers_sync(js_policy);

		/*                          */
		js_kctx_info->ctx.is_scheduled = MALI_FALSE;
		/*                                                                    
                                                                      
               */
		wake_up(&js_kctx_info->ctx.is_scheduled_wait);

		/*                                                       */
		release_result |= KBASEP_JS_RELEASE_RESULT_WAS_DESCHEDULED;

	} else {
		kbasep_js_run_jobs_after_ctx_and_atom_release(kbdev, kctx, katom_retained_state, runpool_ctx_attr_change);

		spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
		mutex_unlock(&current_as->transaction_mutex);
	}

	return release_result;
}

void kbasep_js_runpool_requeue_or_kill_ctx(struct kbase_device *kbdev, struct kbase_context *kctx, mali_bool has_pm_ref)
{
	struct kbasep_js_device_data *js_devdata;
	union kbasep_js_policy *js_policy;
	struct kbasep_js_kctx_info *js_kctx_info;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	js_kctx_info = &kctx->jctx.sched_info;
	js_policy = &kbdev->js_data.policy;
	js_devdata = &kbdev->js_data;

	/*                                                                      
                                                                          */
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.is_scheduled == MALI_FALSE);

	if (js_kctx_info->ctx.is_dying != MALI_FALSE) {
		/*                                                                     
                    */
		dev_dbg(kbdev->dev, "JS: ** Killing Context %p on RunPool Remove **", kctx);
		kbasep_js_policy_foreach_ctx_job(js_policy, kctx, &kbase_jd_cancel, MALI_TRUE);
	} else if (js_kctx_info->ctx.nr_jobs > 0) {
		/*                                                                     
                       */
		kbasep_js_policy_foreach_ctx_job(js_policy, kctx, &kbasep_js_job_check_deref_cores, MALI_FALSE);

		dev_dbg(kbdev->dev, "JS: Requeue Context %p", kctx);
		mutex_lock(&js_devdata->queue_mutex);
		kbasep_js_policy_enqueue_ctx(js_policy, kctx);
		mutex_unlock(&js_devdata->queue_mutex);
	} else {
		/*                                                 */
		dev_dbg(kbdev->dev, "JS: Idling Context %p (not requeued)", kctx);
	}

	if (has_pm_ref) {
		/*                                                            */
		kbase_pm_context_idle(kbdev);
	}
}

void kbasep_js_runpool_release_ctx_and_katom_retained_state(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbasep_js_atom_retained_state *katom_retained_state)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_kctx_info *js_kctx_info;
	kbasep_js_release_result release_result;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	js_kctx_info = &kctx->jctx.sched_info;
	js_devdata = &kbdev->js_data;

	mutex_lock(&js_kctx_info->ctx.jsctx_mutex);
	mutex_lock(&js_devdata->runpool_mutex);
	release_result = kbasep_js_runpool_release_ctx_internal(kbdev, kctx, katom_retained_state);

	/*                                               */
	mutex_unlock(&js_devdata->runpool_mutex);
	if ((release_result & KBASEP_JS_RELEASE_RESULT_WAS_DESCHEDULED) != 0u)
		kbasep_js_runpool_requeue_or_kill_ctx(kbdev, kctx, MALI_TRUE);

	/*                                                           */
	mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);
	if ((release_result & KBASEP_JS_RELEASE_RESULT_WAS_DESCHEDULED) != 0u) {
		/*                                                                     
            
    
                                                                         
                                                                          
                                                   */
		kbasep_js_try_schedule_head_ctx(kbdev);
	}
}

void kbasep_js_runpool_release_ctx(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_atom_retained_state katom_retained_state;

	kbasep_js_atom_retained_state_init_invalid(&katom_retained_state);

	kbasep_js_runpool_release_ctx_and_katom_retained_state(kbdev, kctx, &katom_retained_state);
}

/*                                                                   
                                     */
STATIC void kbasep_js_runpool_release_ctx_no_schedule(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_kctx_info *js_kctx_info;
	kbasep_js_release_result release_result;
	struct kbasep_js_atom_retained_state katom_retained_state_struct;
	struct kbasep_js_atom_retained_state *katom_retained_state = &katom_retained_state_struct;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	js_kctx_info = &kctx->jctx.sched_info;
	js_devdata = &kbdev->js_data;
	kbasep_js_atom_retained_state_init_invalid(katom_retained_state);

	mutex_lock(&js_kctx_info->ctx.jsctx_mutex);
	mutex_lock(&js_devdata->runpool_mutex);
	release_result = kbasep_js_runpool_release_ctx_internal(kbdev, kctx, katom_retained_state);

	/*                                               */
	mutex_unlock(&js_devdata->runpool_mutex);
	if ((release_result & KBASEP_JS_RELEASE_RESULT_WAS_DESCHEDULED) != 0u)
		kbasep_js_runpool_requeue_or_kill_ctx(kbdev, kctx, MALI_TRUE);

	/*                                                           */
	mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);

	/*                                                                   
                                                                          */
}


/* 
                                                                              
                                                                              
  
                                                                                   
  
                        
                                 
                      
  
                                 
                                                      
                                                                                       
                                                                                     
                                                               
  
                        
                                             
  
                                                                                                 
                                
                                                                                                 
                                 
  
 */
STATIC mali_bool kbasep_js_job_check_ref_cores(struct kbase_device *kbdev, int js, struct kbase_jd_atom *katom)
{
	/*                                                                        
                                                                        */
	u64 recently_chosen_affinity = 0;
	mali_bool chosen_affinity = MALI_FALSE;
	mali_bool retry;

	do {
		retry = MALI_FALSE;

		/*                                                                  
                                                                              */
		switch (katom->coreref_state) {
			/*                                             */
		case KBASE_ATOM_COREREF_STATE_NO_CORES_REQUESTED:
			KBASE_DEBUG_ASSERT(katom->affinity == 0);

			/*                  */
			if (MALI_FALSE == kbase_js_choose_affinity(&recently_chosen_affinity, kbdev, katom, js)) {
				/*                                  */
				/*                                        */
				break;
			}

			chosen_affinity = MALI_TRUE;

			/*                   */
			kbase_pm_request_cores(kbdev, katom->core_req & BASE_JD_REQ_T, recently_chosen_affinity);

			katom->affinity = recently_chosen_affinity;

			/*                       */
			katom->coreref_state = KBASE_ATOM_COREREF_STATE_WAITING_FOR_REQUESTED_CORES;

			/*                                               */

		case KBASE_ATOM_COREREF_STATE_WAITING_FOR_REQUESTED_CORES:
			{
				enum kbase_pm_cores_ready cores_ready;
				KBASE_DEBUG_ASSERT(katom->affinity != 0 || (katom->core_req & BASE_JD_REQ_T));

				cores_ready = kbase_pm_register_inuse_cores(kbdev, katom->core_req & BASE_JD_REQ_T, katom->affinity);
				if (cores_ready == KBASE_NEW_AFFINITY) {
					/*                                                     */
					kbasep_js_job_check_deref_cores(kbdev, katom);
					KBASE_TRACE_ADD_SLOT_INFO(kbdev, JS_CORE_REF_REGISTER_INUSE_FAILED, katom->kctx, katom, katom->jc, js, (u32) katom->affinity);
					/*                                                    */
					retry = MALI_TRUE;
					break;
				}
				if (cores_ready == KBASE_CORES_NOT_READY) {
					/*                                                             */
					KBASE_TRACE_ADD_SLOT_INFO(kbdev, JS_CORE_REF_REGISTER_INUSE_FAILED, katom->kctx, katom, katom->jc, js, (u32) katom->affinity);
					/*                                        */
					break;
				}
				/*                       */
				katom->coreref_state = KBASE_ATOM_COREREF_STATE_RECHECK_AFFINITY;
			}

			/*                                               */

		case KBASE_ATOM_COREREF_STATE_RECHECK_AFFINITY:
			KBASE_DEBUG_ASSERT(katom->affinity != 0 || (katom->core_req & BASE_JD_REQ_T));

			/*                                                                    */
			if (chosen_affinity == MALI_FALSE) {
				/*                                                    */
				if (MALI_FALSE == kbase_js_choose_affinity(&recently_chosen_affinity, kbdev, katom, js)) {
					/*                                  */
					kbasep_js_job_check_deref_cores(kbdev, katom);
					KBASE_TRACE_ADD_SLOT_INFO(kbdev, JS_CORE_REF_REQUEST_ON_RECHECK_FAILED, katom->kctx, katom, katom->jc, js, (u32) recently_chosen_affinity);
					/*                                              */
					break;
				}
				chosen_affinity = MALI_TRUE;
			}

			/*                                                   */
			if (recently_chosen_affinity != katom->affinity) {
				enum kbase_pm_cores_ready cores_ready;

				kbase_pm_request_cores(kbdev, katom->core_req & BASE_JD_REQ_T, recently_chosen_affinity);

				/*                                                                                     */
				cores_ready = kbase_pm_register_inuse_cores(kbdev, katom->core_req & BASE_JD_REQ_T, recently_chosen_affinity);
				kbasep_js_job_check_deref_cores(kbdev, katom);

				/*                                                  */
				katom->coreref_state = KBASE_ATOM_COREREF_STATE_RECHECK_AFFINITY;
				katom->affinity = recently_chosen_affinity;
				if (cores_ready == KBASE_NEW_AFFINITY) {
					/*                                                     */
					katom->coreref_state = KBASE_ATOM_COREREF_STATE_WAITING_FOR_REQUESTED_CORES;
					kbasep_js_job_check_deref_cores(kbdev, katom);
					KBASE_TRACE_ADD_SLOT_INFO(kbdev, JS_CORE_REF_REGISTER_INUSE_FAILED, katom->kctx, katom, katom->jc, js, (u32) katom->affinity);
					/*                                                    */
					retry = MALI_TRUE;
					break;
				}
				/*                                                             */
				if (cores_ready == KBASE_CORES_NOT_READY) {
					/*                          */
					katom->coreref_state = KBASE_ATOM_COREREF_STATE_WAITING_FOR_REQUESTED_CORES;
					KBASE_TRACE_ADD_SLOT_INFO(kbdev, JS_CORE_REF_REGISTER_ON_RECHECK_FAILED, katom->kctx, katom, katom->jc, js, (u32) katom->affinity);
					/*                                              */
					break;
				}
			}
			/*                       */
			katom->coreref_state = KBASE_ATOM_COREREF_STATE_CHECK_AFFINITY_VIOLATIONS;

			/*                                               */
		case KBASE_ATOM_COREREF_STATE_CHECK_AFFINITY_VIOLATIONS:
			KBASE_DEBUG_ASSERT(katom->affinity != 0 || (katom->core_req & BASE_JD_REQ_T));
			KBASE_DEBUG_ASSERT(katom->affinity == recently_chosen_affinity);

			/*                                                                   */

			/*                                                                   
                                                */
			if (kbase_js_affinity_would_violate(kbdev, js, katom->affinity) != MALI_FALSE) {
				/*                                                                      */
				kbase_js_affinity_slot_blocked_an_atom(kbdev, js);
				/*                          */
				katom->coreref_state = KBASE_ATOM_COREREF_STATE_RECHECK_AFFINITY;
				/*                                              */
				KBASE_TRACE_ADD_SLOT_INFO(kbdev, JS_CORE_REF_AFFINITY_WOULD_VIOLATE, katom->kctx, katom, katom->jc, js, (u32) katom->affinity);
				break;
			}

			/*                                                             */
			katom->coreref_state = KBASE_ATOM_COREREF_STATE_READY;
			/*                                */
			break;

		default:
			KBASE_DEBUG_ASSERT_MSG(MALI_FALSE, "Unhandled kbase_atom_coreref_state %d", katom->coreref_state);
			break;
		}
	} while (retry != MALI_FALSE);

	return (katom->coreref_state == KBASE_ATOM_COREREF_STATE_READY);
}

void kbasep_js_job_check_deref_cores(struct kbase_device *kbdev, struct kbase_jd_atom *katom)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(katom != NULL);

	switch (katom->coreref_state) {
	case KBASE_ATOM_COREREF_STATE_READY:
		/*                                                                       */
		KBASE_DEBUG_ASSERT(katom->affinity != 0 || (katom->core_req & BASE_JD_REQ_T));

		/*                     */

	case KBASE_ATOM_COREREF_STATE_RECHECK_AFFINITY:
		/*                                   */
		KBASE_DEBUG_ASSERT(katom->affinity != 0 || (katom->core_req & BASE_JD_REQ_T));
		kbase_pm_release_cores(kbdev, katom->core_req & BASE_JD_REQ_T, katom->affinity);

		/*                                                                              
                                                                    
                                                                     
                                                         */

		break;

	case KBASE_ATOM_COREREF_STATE_WAITING_FOR_REQUESTED_CORES:
		/*                                                      */
		KBASE_DEBUG_ASSERT(katom->affinity != 0 || (katom->core_req & BASE_JD_REQ_T));
		kbase_pm_unrequest_cores(kbdev, katom->core_req & BASE_JD_REQ_T, katom->affinity);
		break;

	case KBASE_ATOM_COREREF_STATE_NO_CORES_REQUESTED:
		/*                                  */
		KBASE_DEBUG_ASSERT(katom->affinity == 0);
		break;

	default:
		KBASE_DEBUG_ASSERT_MSG(MALI_FALSE, "Unhandled coreref_state: %d", katom->coreref_state);
		break;
	}

	katom->affinity = 0;
	katom->coreref_state = KBASE_ATOM_COREREF_STATE_NO_CORES_REQUESTED;
}

/*
                                                                               
 */
mali_bool kbasep_js_try_run_next_job_on_slot_irq_nolock(struct kbase_device *kbdev, int js, s8 *submit_count)
{
	struct kbasep_js_device_data *js_devdata;
	mali_bool cores_ready;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	js_devdata = &kbdev->js_data;

	/*                                                                                  
                                                                               
  */
	if (kbase_js_can_run_job_on_slot_no_lock(kbdev, js)) {
		/*                                                                   
                                                                       
                      */
		while (*submit_count < KBASE_JS_MAX_JOB_SUBMIT_PER_SLOT_PER_IRQ && kbasep_jm_is_submit_slots_free(kbdev, js, NULL) != MALI_FALSE) {
			struct kbase_jd_atom *dequeued_atom;
			mali_bool has_job = MALI_FALSE;

			/*                                             */
			has_job = kbasep_js_policy_dequeue_job(kbdev, js, &dequeued_atom);

			if (has_job != MALI_FALSE) {
				/*                                                                   
                                                                      
                                                                      
               */
				struct kbase_context *parent_ctx = dequeued_atom->kctx;
				mali_bool retain_success;

				/*                                                              */
				cores_ready = kbasep_js_job_check_ref_cores(kbdev, js, dequeued_atom);

				if (dequeued_atom->event_code == BASE_JD_EVENT_PM_EVENT || cores_ready != MALI_TRUE) {
					/*                                                                
                                                                            
                                                                         
                                                  */
					kbasep_js_policy_enqueue_job(&kbdev->js_data.policy, dequeued_atom);
					return MALI_TRUE;
				}

				/*                                                             */
				KBASE_DEBUG_ASSERT(kbasep_js_is_submit_allowed(js_devdata, parent_ctx));

				/*                                                       
                                              */
				retain_success = kbasep_js_runpool_retain_ctx_nolock(kbdev, parent_ctx);
				KBASE_DEBUG_ASSERT(retain_success != MALI_FALSE);
				CSTD_UNUSED(retain_success);

				/*                                 */
				kbase_js_affinity_retain_slot_cores(kbdev, js, dequeued_atom->affinity);

				/*                                                                     */
				kbasep_js_ref_permon_check_and_enable_cycle_counter(kbdev, dequeued_atom);

				/*                */
				kbase_job_submit_nolock(kbdev, dequeued_atom, js);

				++(*submit_count);
			} else {
				/*                                              */
				break;
			}
		}
	}

	/*                                                                      
                                                                 
                                                                        
                                                               
   
                                                                               
   
                                                                    
                                                                       
                                                                           
                                                                          
                                                                   
                               
                                                                           
                                                                             
                                             
                                                                         
                                                                           
                                                                                        
  */
	return (mali_bool) (*submit_count >= KBASE_JS_MAX_JOB_SUBMIT_PER_SLOT_PER_IRQ);
}

void kbasep_js_try_run_next_job_on_slot_nolock(struct kbase_device *kbdev, int js)
{
	struct kbasep_js_device_data *js_devdata;
	mali_bool has_job;
	mali_bool cores_ready;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	js_devdata = &kbdev->js_data;

	KBASE_DEBUG_ASSERT(js_devdata->nr_user_contexts_running > 0);

	/*                                                                   
                                                                      
                     */
	if (kbasep_jm_is_submit_slots_free(kbdev, js, NULL) != MALI_FALSE) {
		/*                                                                                  
                                                                                
   */
		if (kbase_js_can_run_job_on_slot_no_lock(kbdev, js)) {
			do {
				struct kbase_jd_atom *dequeued_atom;

				/*                                             */
				has_job = kbasep_js_policy_dequeue_job(kbdev, js, &dequeued_atom);

				if (has_job != MALI_FALSE) {
					/*                                                                   
                                                                       
                                                                       
                */
					struct kbase_context *parent_ctx = dequeued_atom->kctx;
					mali_bool retain_success;

					/*                                                              */
					cores_ready = kbasep_js_job_check_ref_cores(kbdev, js, dequeued_atom);

					if (cores_ready != MALI_TRUE && dequeued_atom->event_code != BASE_JD_EVENT_PM_EVENT) {
						/*                                                                       */
						kbasep_js_policy_enqueue_job(&kbdev->js_data.policy, dequeued_atom);
						break;
					}
					/*                                                             */
					KBASE_DEBUG_ASSERT(kbasep_js_is_submit_allowed(js_devdata, parent_ctx));

					/*                                                       
                                               */
					retain_success = kbasep_js_runpool_retain_ctx_nolock(kbdev, parent_ctx);
					KBASE_DEBUG_ASSERT(retain_success != MALI_FALSE);
					CSTD_UNUSED(retain_success);

					/*                                 */
					kbase_js_affinity_retain_slot_cores(kbdev, js, dequeued_atom->affinity);

					/*                                                                     */
					kbasep_js_ref_permon_check_and_enable_cycle_counter(kbdev, dequeued_atom);

					if (dequeued_atom->event_code == BASE_JD_EVENT_PM_EVENT) {
						dev_warn(kbdev->dev, "Rejecting atom due to BASE_JD_EVENT_PM_EVENT\n");
						/*                                                                      */
						kbase_jd_done(dequeued_atom, js, NULL, 0);
					} else {
						/*                */
						kbase_job_submit_nolock(kbdev, dequeued_atom, js);
					}
				}

			} while (kbasep_jm_is_submit_slots_free(kbdev, js, NULL) != MALI_FALSE && has_job != MALI_FALSE);
		}
	}
}

void kbasep_js_try_schedule_head_ctx(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	mali_bool has_kctx;
	struct kbase_context *head_kctx;
	struct kbasep_js_kctx_info *js_kctx_info;
	mali_bool is_runpool_full;
	struct kbase_as *new_address_space;
	unsigned long flags;
	mali_bool head_kctx_suspended = MALI_FALSE;
	int pm_active_err;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	js_devdata = &kbdev->js_data;

	/*                                                                           
                                                                          
                                                       
   
                                                                        */

	/*                                                      */
	mutex_lock(&js_devdata->queue_mutex);
	has_kctx = kbasep_js_policy_dequeue_head_ctx(&js_devdata->policy, &head_kctx);
	mutex_unlock(&js_devdata->queue_mutex);

	if (has_kctx == MALI_FALSE) {
		/*                                */
		return;
	}
	js_kctx_info = &head_kctx->jctx.sched_info;

	dev_dbg(kbdev->dev, "JS: Dequeue Context %p", head_kctx);

	pm_active_err = kbase_pm_context_active_handle_suspend(kbdev, KBASE_PM_SUSPEND_HANDLER_DONT_INCREASE);

	/*
                                                         
  */
	mutex_lock(&js_kctx_info->ctx.jsctx_mutex);
	mutex_lock(&js_devdata->runpool_mutex);

	/*                                                              
                                                                        
                                                                           
                                                               
                               
                                                          */
	is_runpool_full = check_is_runpool_full(kbdev, head_kctx);
	if (is_runpool_full || pm_active_err || js_kctx_info->ctx.is_dying) {
		/*                                             */
		mutex_unlock(&js_devdata->runpool_mutex);

		/*                                                                         
           */
		kbasep_js_runpool_requeue_or_kill_ctx(kbdev, head_kctx, !pm_active_err);

		mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);
		return;
	}

	/*                                                                     
                                                                            
                     */

	KBASE_TRACE_ADD_REFCOUNT(kbdev, JS_TRY_SCHEDULE_HEAD_CTX, head_kctx, NULL, 0u, kbasep_js_trace_get_refcnt(kbdev, head_kctx));

#if !MALI_CUSTOMER_RELEASE
	if (js_devdata->nr_user_contexts_running == 0) {
		/*                                                       
                                                                                           */
		if (kbdev->js_soft_stop_ticks != 0)
			js_devdata->soft_stop_ticks = kbdev->js_soft_stop_ticks;

		if (kbdev->js_soft_stop_ticks_cl != 0)
			js_devdata->soft_stop_ticks_cl = kbdev->js_soft_stop_ticks_cl;

		if (kbdev->js_hard_stop_ticks_ss != 0)
			js_devdata->hard_stop_ticks_ss = kbdev->js_hard_stop_ticks_ss;

		if (kbdev->js_hard_stop_ticks_cl != 0)
			js_devdata->hard_stop_ticks_cl = kbdev->js_hard_stop_ticks_cl;

		if (kbdev->js_hard_stop_ticks_nss != 0)
			js_devdata->hard_stop_ticks_nss = kbdev->js_hard_stop_ticks_nss;

		if (kbdev->js_reset_ticks_ss != 0)
			js_devdata->gpu_reset_ticks_ss = kbdev->js_reset_ticks_ss;

		if (kbdev->js_reset_ticks_cl != 0)
			js_devdata->gpu_reset_ticks_cl = kbdev->js_reset_ticks_cl;

		if (kbdev->js_reset_ticks_nss != 0)
			js_devdata->gpu_reset_ticks_nss = kbdev->js_reset_ticks_nss;
	}
#endif

	runpool_inc_context_count(kbdev, head_kctx);
	/*                                                                    
                */
	js_kctx_info->ctx.is_scheduled = MALI_TRUE;
	wake_up(&js_kctx_info->ctx.is_scheduled_wait);

	/*                                                                           */
	new_address_space = pick_free_addr_space(kbdev);

	/*                                             */
	mutex_lock(&new_address_space->transaction_mutex);
	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

	/*                                                                                   
                                                              */
	assign_and_activate_kctx_addr_space(kbdev, head_kctx, new_address_space);

	/*                                                                                       */

	if ((js_kctx_info->ctx.flags & KBASE_CTX_FLAG_PRIVILEGED) != 0) {
		/*                                                              */
		kbasep_js_runpool_retain_ctx_nolock(kbdev, head_kctx);
	}

	/*                                                              
                                                                            
                                                                          
                                                              
   
                                                                            
                                                                            
                                                                            
                                                                            
                                                                        */
	if (kbase_pm_is_suspending(kbdev)) {
		/*                                       */
		mali_bool retained;

		retained = kbasep_js_runpool_retain_ctx_nolock(kbdev, head_kctx);
		KBASE_DEBUG_ASSERT(retained);
		kbasep_js_clear_submit_allowed(js_devdata, head_kctx);
		head_kctx_suspended = MALI_TRUE;
	}

	/*                                                                      
                                                                           
       */
	kbasep_js_try_run_next_job_nolock(kbdev);

	/*                      */
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);
	mutex_unlock(&new_address_space->transaction_mutex);
	mutex_unlock(&js_devdata->runpool_mutex);
	mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);
	/*                                                                                     */

	if (head_kctx_suspended) {
		/*                                                                     
                                                                         
                                                            */
		kbasep_js_runpool_release_ctx_no_schedule(kbdev, head_kctx);
	}
}

void kbasep_js_schedule_privileged_ctx(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_kctx_info *js_kctx_info;
	struct kbasep_js_device_data *js_devdata;
	mali_bool is_scheduled;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);

	js_devdata = &kbdev->js_data;
	js_kctx_info = &kctx->jctx.sched_info;

	/*                                                                     
                                                             */
	BUG_ON(kbase_pm_is_suspending(kbdev));

	kbase_pm_request_l2_caches(kbdev);

	mutex_lock(&js_kctx_info->ctx.jsctx_mutex);
	/*                                */
	js_kctx_info->ctx.flags |= KBASE_CTX_FLAG_PRIVILEGED;

	is_scheduled = js_kctx_info->ctx.is_scheduled;
	if (is_scheduled == MALI_FALSE) {
		mali_bool is_runpool_full;

		/*                                */
		mutex_lock(&js_devdata->queue_mutex);
		kbasep_js_policy_enqueue_ctx(&js_devdata->policy, kctx);
		mutex_unlock(&js_devdata->queue_mutex);

		mutex_lock(&js_devdata->runpool_mutex);
		{
			is_runpool_full = check_is_runpool_full(kbdev, kctx);
			if (is_runpool_full != MALI_FALSE) {
				/*                                                       */
				kbasep_js_runpool_evict_next_jobs(kbdev, kctx);
			}
		}
		mutex_unlock(&js_devdata->runpool_mutex);
		mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);
		/*                                                                                         */

		if (is_runpool_full != MALI_FALSE) {
			/*                                             */
			kbasep_js_runpool_attempt_fast_start_ctx(kbdev, NULL);
		}
		/*                                */
		kbasep_js_try_schedule_head_ctx(kbdev);

		/*                                         */
		wait_event(kctx->jctx.sched_info.ctx.is_scheduled_wait, kctx->jctx.sched_info.ctx.is_scheduled == MALI_TRUE);
	} else {
		/*                                                                                     */
		kbasep_js_runpool_retain_ctx(kbdev, kctx);
		mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);
	}
}

void kbasep_js_release_privileged_ctx(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_kctx_info *js_kctx_info;

	KBASE_DEBUG_ASSERT(kctx != NULL);
	js_kctx_info = &kctx->jctx.sched_info;

	/*                                                */
	mutex_lock(&js_kctx_info->ctx.jsctx_mutex);
	js_kctx_info->ctx.flags &= (~KBASE_CTX_FLAG_PRIVILEGED);
	mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);

	kbase_pm_release_l2_caches(kbdev);

	/*                                                                           */
	kbasep_js_runpool_release_ctx(kbdev, kctx);
}

void kbasep_js_job_done_slot_irq(struct kbase_jd_atom *katom, int slot_nr,
		ktime_t *end_timestamp, kbasep_js_atom_done_code done_code)
{
	struct kbase_device *kbdev;
	union kbasep_js_policy *js_policy;
	struct kbasep_js_device_data *js_devdata;
	mali_bool submit_retry_needed = MALI_TRUE;	/*                                                            */
	ktime_t tick_diff;
	u64 microseconds_spent = 0u;
	struct kbase_context *parent_ctx;

	KBASE_DEBUG_ASSERT(katom);
	parent_ctx = katom->kctx;
	KBASE_DEBUG_ASSERT(parent_ctx);
	kbdev = parent_ctx->kbdev;
	KBASE_DEBUG_ASSERT(kbdev);

	js_devdata = &kbdev->js_data;
	js_policy = &kbdev->js_data.policy;

	lockdep_assert_held(&js_devdata->runpool_irq.lock);

	/*
                                                                        
                                         
  */
#ifdef CONFIG_MALI_GATOR_SUPPORT
	kbase_trace_mali_job_slots_event(GATOR_MAKE_EVENT(GATOR_JOB_SLOT_STOP, slot_nr), NULL, 0);
#endif				/*                           */

	/*                                                                           */
	kbasep_js_deref_permon_check_and_disable_cycle_counter(kbdev, katom);

	/*                                                                                      */
	kbase_js_affinity_release_slot_cores(kbdev, slot_nr, katom->affinity);
	kbase_js_debug_log_current_affinities(kbdev);
	/*                               */
	if (end_timestamp != NULL) {
		/*                                                                     
                                                                      */
		tick_diff = ktime_sub(*end_timestamp, katom->start_timestamp);

		microseconds_spent = ktime_to_ns(tick_diff);
		do_div(microseconds_spent, 1000);

		/*                                                     */
		if (microseconds_spent < KBASEP_JS_TICK_RESOLUTION_US)
			microseconds_spent = KBASEP_JS_TICK_RESOLUTION_US;
	}

	/*                                                                */
	kbasep_js_policy_log_job_result(js_policy, katom, microseconds_spent);
	/*                                                        */
	if (kbasep_js_policy_should_remove_ctx(js_policy, parent_ctx) != MALI_FALSE)
		kbasep_js_clear_submit_allowed(js_devdata, parent_ctx);

	if (done_code & KBASE_JS_ATOM_DONE_START_NEW_ATOMS) {
		/*                                                                                        */
		KBASE_TRACE_ADD_SLOT(kbdev, JS_JOB_DONE_TRY_RUN_NEXT_JOB, parent_ctx, katom, katom->jc, slot_nr);

		submit_retry_needed = kbasep_js_try_run_next_job_on_slot_irq_nolock(kbdev, slot_nr, &kbdev->slot_submit_count_irq[slot_nr]);
	}

	if (submit_retry_needed != MALI_FALSE || katom->event_code == BASE_JD_EVENT_STOPPED) {
		/*                                                                  
                                                                      
                                                                       
                                                                         
                                                                 
                                                                    
                                                     
    
                                                                        
                     */

		KBASE_TRACE_ADD_SLOT(kbdev, JS_JOB_DONE_RETRY_NEEDED, parent_ctx, katom, katom->jc, slot_nr);
		kbasep_js_set_job_retry_submit_slot(katom, slot_nr);
	}
}

void kbasep_js_suspend(struct kbase_device *kbdev)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;
	int i;
	u16 retained = 0u;
	int nr_privileged_ctx = 0;
	KBASE_DEBUG_ASSERT(kbdev);
	KBASE_DEBUG_ASSERT(kbase_pm_is_suspending(kbdev));
	js_devdata = &kbdev->js_data;

	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

	/*                                      */
	js_devdata->runpool_irq.submit_allowed = 0;

	/*                                                                        
                              */
	for (i = BASE_MAX_NR_AS - 1; i >= 0; --i) {
		struct kbasep_js_per_as_data *js_per_as_data = &js_devdata->runpool_irq.per_as_data[i];
		struct kbase_context *kctx = js_per_as_data->kctx;

		retained = retained << 1;

		if (kctx) {
			++(js_per_as_data->as_busy_refcount);
			retained |= 1u;
			/*                                                       
                                                           
                      */
			if (kctx->jctx.sched_info.ctx.flags & KBASE_CTX_FLAG_PRIVILEGED)
				KBASE_DEBUG_ASSERT(++nr_privileged_ctx == 1);
		}
	}
	CSTD_UNUSED(nr_privileged_ctx);
	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	/*                                                                  
                    */
	for (i = 0;
		 i < BASE_MAX_NR_AS;
		 ++i, retained = retained >> 1) {
		struct kbasep_js_per_as_data *js_per_as_data = &js_devdata->runpool_irq.per_as_data[i];
		struct kbase_context *kctx = js_per_as_data->kctx;

		if (retained & 1u)
			kbasep_js_runpool_release_ctx(kbdev, kctx);
	}

	/*                                                                        */
}

void kbasep_js_resume(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	int i;

	KBASE_DEBUG_ASSERT(kbdev);
	js_devdata = &kbdev->js_data;

	KBASE_DEBUG_ASSERT(!kbase_pm_is_suspending(kbdev));

	/*                                                                         */
	for (i = 0 ; i < kbdev->nr_hw_address_spaces; ++i)
		kbasep_js_try_schedule_head_ctx(kbdev);

	/*                    */
}
