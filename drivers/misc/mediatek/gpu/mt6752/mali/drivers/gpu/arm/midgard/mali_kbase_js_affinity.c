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
#include "mali_kbase_js_affinity.h"


STATIC INLINE mali_bool affinity_job_uses_high_cores(struct kbase_device *kbdev, struct kbase_jd_atom *katom)
{
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8987)) {
		struct kbase_context *kctx;
		kbase_context_flags ctx_flags;

		kctx = katom->kctx;
		ctx_flags = kctx->jctx.sched_info.ctx.flags;

		/*                                                                     
                                                                */
		return (mali_bool) ((katom->core_req & BASE_JD_REQ_ONLY_COMPUTE) != 0 || (ctx_flags & KBASE_CTX_FLAG_HINT_ONLY_COMPUTE) != 0);
	}
	return MALI_FALSE;
}

/* 
                                                                              
  
                                                           
                                                          
  
                                                        
                                                     
                                                     
 */
STATIC INLINE mali_bool kbase_affinity_requires_split(struct kbase_device *kbdev)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	lockdep_assert_held(&kbdev->js_data.runpool_irq.lock);

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8987)) {
		s8 nr_compute_ctxs = kbasep_js_ctx_attr_count_on_runpool(kbdev, KBASEP_JS_CTX_ATTR_COMPUTE);
		s8 nr_noncompute_ctxs = kbasep_js_ctx_attr_count_on_runpool(kbdev, KBASEP_JS_CTX_ATTR_NON_COMPUTE);

		/*                                                                
                                                                          
                              
    
                                                                          
              
    
                                                                           
                                                     */

		return (mali_bool) (nr_compute_ctxs > 0 && nr_noncompute_ctxs > 0);
	}
	return MALI_FALSE;
}

mali_bool kbase_js_can_run_job_on_slot_no_lock(struct kbase_device *kbdev, int js)
{
	/*
                                              
                                                                  
                                    
                                            
                                                
                                                                        
                                                                         
              
                                                                          
                                                                         
                                                                        
                                                                    
                 
  */
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8987))
		return MALI_TRUE;

	if (js != 2)
		return MALI_TRUE;

	/*                           */
	if (kbdev->gpu_props.num_core_groups > 1) {
		/*                                          */
		if (kbasep_js_ctx_attr_is_attr_on_runpool(kbdev, KBASEP_JS_CTX_ATTR_COMPUTE_ALL_CORES) == MALI_FALSE) {
			/*                                                              */

			/*                                                                                        */
			return MALI_TRUE;
		}
	}

	/*                                                  */
	return MALI_FALSE;
}

/*
                                                                  
                                                              
                                                             
           
                                                              
                                                    
                                                                
                                                                
                                                                 
                                                          
                     
                                                             
                                                                
                                                      
*/
mali_bool kbase_js_choose_affinity(u64 * const affinity, struct kbase_device *kbdev, struct kbase_jd_atom *katom, int js)
{
	base_jd_core_req core_req = katom->core_req;
	unsigned int num_core_groups = kbdev->gpu_props.num_core_groups;
	u64 core_availability_mask;
	unsigned long flags;

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	core_availability_mask = kbase_pm_ca_get_core_mask(kbdev);

	/*
                                                                    
                             
  */
	if (0 == core_availability_mask) {
		spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
		*affinity = 0;
		return MALI_FALSE;
	}

	KBASE_DEBUG_ASSERT(js >= 0);

	if ((core_req & (BASE_JD_REQ_FS | BASE_JD_REQ_CS | BASE_JD_REQ_T)) == BASE_JD_REQ_T) {
		spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
		/*                                                                           */
		*affinity = 1;
		return MALI_TRUE;
	}

	if (1 == kbdev->gpu_props.num_cores) {
		/*                                           */
		*affinity = core_availability_mask;
	} else if (kbase_affinity_requires_split(kbdev) == MALI_FALSE) {
		if ((core_req & (BASE_JD_REQ_COHERENT_GROUP | BASE_JD_REQ_SPECIFIC_COHERENT_GROUP))) {
			if (js == 0 || num_core_groups == 1) {
				/*                                                                   */
				*affinity = kbdev->gpu_props.props.coherency_info.group[0].core_mask & core_availability_mask;
			} else {
				/*                                                               */
				u32 core_group_idx = ((u32) js) - 1;

				KBASE_DEBUG_ASSERT(core_group_idx < num_core_groups);
				*affinity = kbdev->gpu_props.props.coherency_info.group[core_group_idx].core_mask & core_availability_mask;

				/*                                                               
                                                                     */
				if (*affinity == 0 && core_group_idx == 1 && kbdev->pm.cg1_disabled == MALI_TRUE)
					katom->event_code = BASE_JD_EVENT_PM_EVENT;
			}
		} else {
			/*                                                        */
			*affinity = core_availability_mask;
		}
	} else {
		/*                                                                  */
		u64 low_bitmap, high_bitmap;
		int n_high_cores = kbdev->gpu_props.num_cores >> 1;
		KBASE_DEBUG_ASSERT(1 == num_core_groups);
		KBASE_DEBUG_ASSERT(0 != n_high_cores);

		/*                                        */
		high_bitmap = ~0;
		/*                                                     */
		while (n_high_cores != hweight32(high_bitmap & kbdev->shader_present_bitmap))
			high_bitmap = high_bitmap << 1;

		high_bitmap &= core_availability_mask;
		low_bitmap = core_availability_mask ^ high_bitmap;

		if (affinity_job_uses_high_cores(kbdev, katom))
			*affinity = high_bitmap;
		else
			*affinity = low_bitmap;
	}

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	/*
                                                                    
                                                          
  */
	if (*affinity == 0)
		return MALI_FALSE;

	/*                                 */
	if (core_req & BASE_JD_REQ_T)
		*affinity = *affinity | 1;

	return MALI_TRUE;
}

STATIC INLINE mali_bool kbase_js_affinity_is_violating(struct kbase_device *kbdev, u64 *affinities)
{
	/*                                                                                     
                                                                                        
                                                                                      
                                                   
   
                                                              
                                                                       
                                                                  
  */
	u64 affinity_set_left;
	u64 affinity_set_right;
	u64 intersection;

	KBASE_DEBUG_ASSERT(affinities != NULL);

	affinity_set_left = affinities[1];

	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8987)) {
		/*                                                           
                                                           */
		affinity_set_left |= affinities[0];
	}

	affinity_set_right = affinities[2];

	/*                                                                          */
	intersection = affinity_set_left & affinity_set_right;

	return (mali_bool) (intersection != (u64) 0u);
}

mali_bool kbase_js_affinity_would_violate(struct kbase_device *kbdev, int js, u64 affinity)
{
	struct kbasep_js_device_data *js_devdata;
	u64 new_affinities[BASE_JM_MAX_NR_SLOTS];

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(js < BASE_JM_MAX_NR_SLOTS);
	js_devdata = &kbdev->js_data;

	memcpy(new_affinities, js_devdata->runpool_irq.slot_affinities, sizeof(js_devdata->runpool_irq.slot_affinities));

	new_affinities[js] |= affinity;

	return kbase_js_affinity_is_violating(kbdev, new_affinities);
}

void kbase_js_affinity_retain_slot_cores(struct kbase_device *kbdev, int js, u64 affinity)
{
	struct kbasep_js_device_data *js_devdata;
	u64 cores;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(js < BASE_JM_MAX_NR_SLOTS);
	js_devdata = &kbdev->js_data;

	KBASE_DEBUG_ASSERT(kbase_js_affinity_would_violate(kbdev, js, affinity) == MALI_FALSE);

	cores = affinity;
	while (cores) {
		int bitnum = fls64(cores) - 1;
		u64 bit = 1ULL << bitnum;
		s8 cnt;

		KBASE_DEBUG_ASSERT(js_devdata->runpool_irq.slot_affinity_refcount[js][bitnum] < BASE_JM_SUBMIT_SLOTS);

		cnt = ++(js_devdata->runpool_irq.slot_affinity_refcount[js][bitnum]);

		if (cnt == 1)
			js_devdata->runpool_irq.slot_affinities[js] |= bit;

		cores &= ~bit;
	}
}

void kbase_js_affinity_release_slot_cores(struct kbase_device *kbdev, int js, u64 affinity)
{
	struct kbasep_js_device_data *js_devdata;
	u64 cores;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(js < BASE_JM_MAX_NR_SLOTS);
	js_devdata = &kbdev->js_data;

	cores = affinity;
	while (cores) {
		int bitnum = fls64(cores) - 1;
		u64 bit = 1ULL << bitnum;
		s8 cnt;

		KBASE_DEBUG_ASSERT(js_devdata->runpool_irq.slot_affinity_refcount[js][bitnum] > 0);

		cnt = --(js_devdata->runpool_irq.slot_affinity_refcount[js][bitnum]);

		if (0 == cnt)
			js_devdata->runpool_irq.slot_affinities[js] &= ~bit;

		cores &= ~bit;
	}

}

void kbase_js_affinity_slot_blocked_an_atom(struct kbase_device *kbdev, int js)
{
	struct kbasep_js_device_data *js_devdata;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(js < BASE_JM_MAX_NR_SLOTS);
	js_devdata = &kbdev->js_data;

	js_devdata->runpool_irq.slots_blocked_on_affinity |= 1u << js;
}

void kbase_js_affinity_submit_to_blocked_slots(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	u16 slots;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;

	KBASE_DEBUG_ASSERT(js_devdata->nr_user_contexts_running != 0);

	/*                                                                   */
	slots = js_devdata->runpool_irq.slots_blocked_on_affinity;

	while (slots) {
		int bitnum = fls(slots) - 1;

		u16 bit = 1u << bitnum;
		slots &= ~bit;

		KBASE_TRACE_ADD_SLOT(kbdev, JS_AFFINITY_SUBMIT_TO_BLOCKED, NULL, NULL, 0u, bitnum);

		/*                                                          */
		js_devdata->runpool_irq.slots_blocked_on_affinity &= ~bit;

		kbasep_js_try_run_next_job_on_slot_nolock(kbdev, bitnum);

		/*                                                                                    */
	}
}

#if KBASE_TRACE_ENABLE
void kbase_js_debug_log_current_affinities(struct kbase_device *kbdev)
{
	struct kbasep_js_device_data *js_devdata;
	int slot_nr;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	js_devdata = &kbdev->js_data;

	for (slot_nr = 0; slot_nr < 3; ++slot_nr)
		KBASE_TRACE_ADD_SLOT_INFO(kbdev, JS_AFFINITY_CURRENT, NULL, NULL, 0u, slot_nr, (u32) js_devdata->runpool_irq.slot_affinities[slot_nr]);
}
#endif				/*                     */
