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




#include <mali_kbase.h>
#include <mali_kbase_config.h>

/*
                           
 */

/* 
                                                                             
                            
  
            
                
                         
                                    
  
                                                                               
                                                                      
                                                                                  
                                       
                                                                                 
 */
STATIC mali_bool kbasep_js_ctx_attr_runpool_retain_attr(struct kbase_device *kbdev, struct kbase_context *kctx, enum kbasep_js_ctx_attr attribute)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_kctx_info *js_kctx_info;
	mali_bool runpool_state_changed = MALI_FALSE;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(attribute < KBASEP_JS_CTX_ATTR_COUNT);
	js_devdata = &kbdev->js_data;
	js_kctx_info = &kctx->jctx.sched_info;

	BUG_ON(!mutex_is_locked(&js_kctx_info->ctx.jsctx_mutex));
	lockdep_assert_held(&kbdev->js_data.runpool_irq.lock);

	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.is_scheduled != MALI_FALSE);

	if (kbasep_js_ctx_attr_is_attr_on_ctx(kctx, attribute) != MALI_FALSE) {
		KBASE_DEBUG_ASSERT(js_devdata->runpool_irq.ctx_attr_ref_count[attribute] < S8_MAX);
		++(js_devdata->runpool_irq.ctx_attr_ref_count[attribute]);

		if (js_devdata->runpool_irq.ctx_attr_ref_count[attribute] == 1) {
			/*                                         */
			runpool_state_changed = MALI_TRUE;
			KBASE_TRACE_ADD(kbdev, JS_CTX_ATTR_NOW_ON_RUNPOOL, kctx, NULL, 0u, attribute);
		}
	}

	return runpool_state_changed;
}

/* 
                                                                              
                            
  
            
                
                         
                                    
  
                                                                               
                                                                      
                                                                                  
                                       
                                                                                 
 */
STATIC mali_bool kbasep_js_ctx_attr_runpool_release_attr(struct kbase_device *kbdev, struct kbase_context *kctx, enum kbasep_js_ctx_attr attribute)
{
	struct kbasep_js_device_data *js_devdata;
	struct kbasep_js_kctx_info *js_kctx_info;
	mali_bool runpool_state_changed = MALI_FALSE;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(attribute < KBASEP_JS_CTX_ATTR_COUNT);
	js_devdata = &kbdev->js_data;
	js_kctx_info = &kctx->jctx.sched_info;

	BUG_ON(!mutex_is_locked(&js_kctx_info->ctx.jsctx_mutex));
	lockdep_assert_held(&kbdev->js_data.runpool_irq.lock);
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.is_scheduled != MALI_FALSE);

	if (kbasep_js_ctx_attr_is_attr_on_ctx(kctx, attribute) != MALI_FALSE) {
		KBASE_DEBUG_ASSERT(js_devdata->runpool_irq.ctx_attr_ref_count[attribute] > 0);
		--(js_devdata->runpool_irq.ctx_attr_ref_count[attribute]);

		if (js_devdata->runpool_irq.ctx_attr_ref_count[attribute] == 0) {
			/*                                           */
			runpool_state_changed = MALI_TRUE;
			KBASE_TRACE_ADD(kbdev, JS_CTX_ATTR_NOW_OFF_RUNPOOL, kctx, NULL, 0u, attribute);
		}
	}

	return runpool_state_changed;
}

/* 
                                                                               
                               
  
            
                
                                                                             
  
                                                                               
                                                                    
                                                                                 
 */
STATIC mali_bool kbasep_js_ctx_attr_ctx_retain_attr(struct kbase_device *kbdev, struct kbase_context *kctx, enum kbasep_js_ctx_attr attribute)
{
	struct kbasep_js_kctx_info *js_kctx_info;
	mali_bool runpool_state_changed = MALI_FALSE;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(attribute < KBASEP_JS_CTX_ATTR_COUNT);
	js_kctx_info = &kctx->jctx.sched_info;

	BUG_ON(!mutex_is_locked(&js_kctx_info->ctx.jsctx_mutex));
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.ctx_attr_ref_count[attribute] < U32_MAX);

	++(js_kctx_info->ctx.ctx_attr_ref_count[attribute]);

	if (js_kctx_info->ctx.is_scheduled != MALI_FALSE && js_kctx_info->ctx.ctx_attr_ref_count[attribute] == 1) {
		lockdep_assert_held(&kbdev->js_data.runpool_irq.lock);
		/*                                                                                                  */
		KBASE_TRACE_ADD(kbdev, JS_CTX_ATTR_NOW_ON_CTX, kctx, NULL, 0u, attribute);
		runpool_state_changed = kbasep_js_ctx_attr_runpool_retain_attr(kbdev, kctx, attribute);
	}

	return runpool_state_changed;
}

/* 
                                                                                  
                               
  
            
                
                                                                             
  
                                                                               
                                                                    
                                                                                 
 */
STATIC mali_bool kbasep_js_ctx_attr_ctx_release_attr(struct kbase_device *kbdev, struct kbase_context *kctx, enum kbasep_js_ctx_attr attribute)
{
	struct kbasep_js_kctx_info *js_kctx_info;
	mali_bool runpool_state_changed = MALI_FALSE;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(attribute < KBASEP_JS_CTX_ATTR_COUNT);
	js_kctx_info = &kctx->jctx.sched_info;

	BUG_ON(!mutex_is_locked(&js_kctx_info->ctx.jsctx_mutex));
	KBASE_DEBUG_ASSERT(js_kctx_info->ctx.ctx_attr_ref_count[attribute] > 0);

	if (js_kctx_info->ctx.is_scheduled != MALI_FALSE && js_kctx_info->ctx.ctx_attr_ref_count[attribute] == 1) {
		lockdep_assert_held(&kbdev->js_data.runpool_irq.lock);
		/*                                                                                          */
		runpool_state_changed = kbasep_js_ctx_attr_runpool_release_attr(kbdev, kctx, attribute);
		KBASE_TRACE_ADD(kbdev, JS_CTX_ATTR_NOW_OFF_CTX, kctx, NULL, 0u, attribute);
	}

	/*                                                                                                   */
	--(js_kctx_info->ctx.ctx_attr_ref_count[attribute]);

	return runpool_state_changed;
}

/*
                                      
 */

void kbasep_js_ctx_attr_set_initial_attrs(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	struct kbasep_js_kctx_info *js_kctx_info;
	mali_bool runpool_state_changed = MALI_FALSE;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	js_kctx_info = &kctx->jctx.sched_info;

	if ((js_kctx_info->ctx.flags & KBASE_CTX_FLAG_SUBMIT_DISABLED) != MALI_FALSE) {
		/*                                                                      */
		return;
	}

	/*                                                                                     */

	if ((js_kctx_info->ctx.flags & KBASE_CTX_FLAG_HINT_ONLY_COMPUTE) != MALI_FALSE) {
		/*                 */
		runpool_state_changed |= kbasep_js_ctx_attr_ctx_retain_attr(kbdev, kctx, KBASEP_JS_CTX_ATTR_COMPUTE);
	}
	/*                                                                      
                                                                        */

	/*                                           */

	/*                                                                         
                                                                           
                                                          */
	KBASE_DEBUG_ASSERT(runpool_state_changed == MALI_FALSE);
	CSTD_UNUSED(runpool_state_changed);
}

void kbasep_js_ctx_attr_runpool_retain_ctx(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	mali_bool runpool_state_changed;
	int i;

	/*                                */
	for (i = 0; i < KBASEP_JS_CTX_ATTR_COUNT; ++i) {
		if (kbasep_js_ctx_attr_is_attr_on_ctx(kctx, (enum kbasep_js_ctx_attr) i) != MALI_FALSE) {
			/*                                                                                  */
			runpool_state_changed = kbasep_js_ctx_attr_runpool_retain_attr(kbdev, kctx, (enum kbasep_js_ctx_attr) i);

			/*                                                               
                                                                    
                       */
			CSTD_UNUSED(runpool_state_changed);
		}
	}
}

mali_bool kbasep_js_ctx_attr_runpool_release_ctx(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	mali_bool runpool_state_changed = MALI_FALSE;
	int i;

	/*                                 */
	for (i = 0; i < KBASEP_JS_CTX_ATTR_COUNT; ++i) {
		if (kbasep_js_ctx_attr_is_attr_on_ctx(kctx, (enum kbasep_js_ctx_attr) i) != MALI_FALSE) {
			/*                                                                                     */
			runpool_state_changed |= kbasep_js_ctx_attr_runpool_release_attr(kbdev, kctx, (enum kbasep_js_ctx_attr) i);
		}
	}

	return runpool_state_changed;
}

void kbasep_js_ctx_attr_ctx_retain_atom(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_jd_atom *katom)
{
	mali_bool runpool_state_changed = MALI_FALSE;
	base_jd_core_req core_req;

	KBASE_DEBUG_ASSERT(katom);
	core_req = katom->core_req;

	if (core_req & BASE_JD_REQ_ONLY_COMPUTE)
		runpool_state_changed |= kbasep_js_ctx_attr_ctx_retain_attr(kbdev, kctx, KBASEP_JS_CTX_ATTR_COMPUTE);
	else
		runpool_state_changed |= kbasep_js_ctx_attr_ctx_retain_attr(kbdev, kctx, KBASEP_JS_CTX_ATTR_NON_COMPUTE);

	if ((core_req & (BASE_JD_REQ_CS | BASE_JD_REQ_ONLY_COMPUTE | BASE_JD_REQ_T)) != 0 && (core_req & (BASE_JD_REQ_COHERENT_GROUP | BASE_JD_REQ_SPECIFIC_COHERENT_GROUP)) == 0) {
		/*                                                            */
		runpool_state_changed |= kbasep_js_ctx_attr_ctx_retain_attr(kbdev, kctx, KBASEP_JS_CTX_ATTR_COMPUTE_ALL_CORES);
	}

	/*                                                                
                                                                  
              */
	CSTD_UNUSED(runpool_state_changed);
}

mali_bool kbasep_js_ctx_attr_ctx_release_atom(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbasep_js_atom_retained_state *katom_retained_state)
{
	mali_bool runpool_state_changed = MALI_FALSE;
	base_jd_core_req core_req;

	KBASE_DEBUG_ASSERT(katom_retained_state);
	core_req = katom_retained_state->core_req;

	/*                         */
	if (kbasep_js_atom_retained_state_is_valid(katom_retained_state) == MALI_FALSE)
		return MALI_FALSE;

	if (core_req & BASE_JD_REQ_ONLY_COMPUTE) {
#if KBASE_PM_EN
		unsigned long flags;
		int device_nr = (core_req & BASE_JD_REQ_SPECIFIC_COHERENT_GROUP) ? katom_retained_state->device_nr : 0;
		KBASE_DEBUG_ASSERT(device_nr < 2);

		spin_lock_irqsave(&kbdev->pm.metrics.lock, flags);
		kbasep_pm_record_job_status(kbdev);
		kbdev->pm.metrics.active_cl_ctx[device_nr]--;
		spin_unlock_irqrestore(&kbdev->pm.metrics.lock, flags);
#endif
		runpool_state_changed |= kbasep_js_ctx_attr_ctx_release_attr(kbdev, kctx, KBASEP_JS_CTX_ATTR_COMPUTE);
	} else {
#if KBASE_PM_EN
		unsigned long flags;

		spin_lock_irqsave(&kbdev->pm.metrics.lock, flags);
		kbasep_pm_record_job_status(kbdev);
		kbdev->pm.metrics.active_gl_ctx--;
		spin_unlock_irqrestore(&kbdev->pm.metrics.lock, flags);
#endif
		runpool_state_changed |= kbasep_js_ctx_attr_ctx_release_attr(kbdev, kctx, KBASEP_JS_CTX_ATTR_NON_COMPUTE);
	}

	if ((core_req & (BASE_JD_REQ_CS | BASE_JD_REQ_ONLY_COMPUTE | BASE_JD_REQ_T)) != 0 && (core_req & (BASE_JD_REQ_COHERENT_GROUP | BASE_JD_REQ_SPECIFIC_COHERENT_GROUP)) == 0) {
		/*                                                            */
		runpool_state_changed |= kbasep_js_ctx_attr_ctx_release_attr(kbdev, kctx, KBASEP_JS_CTX_ATTR_COMPUTE_ALL_CORES);
	}

	return runpool_state_changed;
}
