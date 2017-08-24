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

#ifndef _KBASE_JS_H_
#define _KBASE_JS_H_

#include <malisw/mali_malisw.h>

#include "mali_kbase_js_defs.h"
#include "mali_kbase_js_policy.h"
#include "mali_kbase_defs.h"
#include "mali_kbase_debug.h"

#include "mali_kbase_js_ctx_attr.h"

/* 
                       
     
 */

/* 
                             
     
 */

/* 
                                                   
     
  
                                                                    
                                                   
 */

/* 
                                      
  
                                                                          
                                                                               
                                     
 */
mali_error kbasep_js_devdata_init(struct kbase_device * const kbdev);

/* 
                                 
  
                                                                           
                                                                               
                   
  
                                                                                    
                                                                     
                                                       
  
                                                                              
                                             
  
 */
void kbasep_js_devdata_halt(struct kbase_device *kbdev);

/* 
                                     
  
                                                                           
                                                                               
                   
  
                                                                                    
                                                                     
                                                       
  
                                                                              
                                             
 */
void kbasep_js_devdata_term(struct kbase_device *kbdev);

/* 
                                                                                             
  
                                                                          
  
                                                                                      
                                                              
  
                                                                           
                                                                       
 */
mali_error kbasep_js_kctx_init(struct kbase_context * const kctx);

/* 
                                                                                           
  
                                                                              
  
                                                                                 
                                                                             
        
  
                                                                                      
                                   
  
                                                                     
                                
 */
void kbasep_js_kctx_term(struct kbase_context *kctx);

/* 
                                                                             
                                    
  
                                      
                                           
                                                                
  
                                                          
                                                                               
                  
                                                                              
                   
  
                                                                   
                                                                
                                                                          
                                                                          
                                   
  
                                                                              
                                                                      
                                                                            
                                                   
  
                                                                                
  
                                                           
                                                                
                                                                                   
                       
                                                                               
                       
                                                                                           
  
                                                                            
                                                                  
                                                                              
                                                                               
                                           
 */
mali_bool kbasep_js_add_job(struct kbase_context *kctx, struct kbase_jd_atom *atom);

/* 
                                                                                     
  
                                                    
                                                                             
             
                                                                                
                                                                             
                                                             
  
                                                                                                                                                                           
  
                                                                                                                                   
  
                                               
                                            
                                                             
                                     
                                                                 
                                                                         
  
                                                                            
                                            
  
                                                           
                                                        
  
 */
void kbasep_js_remove_job(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_jd_atom *atom);

/* 
                                                                           
                                     
  
                                                                              
                                                                           
                                               
  
                                               
                                            
                                                             
                                     
                                                    
                                                                     
                                                                         
  
                                                           
                                                        
                                                                                        
                       
                                                                                
                       
 */
void kbasep_js_remove_cancelled_job(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_jd_atom *katom);

/* 
                                                                              
       
  
                                                             
  
                                                           
                                                                               
                              
  
                                                                                                  
                                                                                                 
 */
mali_bool kbasep_js_runpool_retain_ctx(struct kbase_device *kbdev, struct kbase_context *kctx);

/* 
                                                                              
       
  
                                                             
  
                                                  
                                             
  
                                                                                                  
                                                                                                 
 */
mali_bool kbasep_js_runpool_retain_ctx_nolock(struct kbase_device *kbdev, struct kbase_context *kctx);

/* 
                                                                               
                                         
  
                                                                        
                                                                              
                                        
  
                                                             
  
                                                           
                                                                                
                              
  
                                                                                            
                                                                            
 */
struct kbase_context *kbasep_js_runpool_lookup_ctx(struct kbase_device *kbdev, int as_nr);

/* 
                                                                               
                           
  
                                                                           
                                        
                                                                             
        
                               
  
                                                                           
  
                                                               
                                                                           
                                                                             
                                                                           
                                                                            
                                                           
  
                                                           
                                                        
                                                                             
                       
 */
void kbasep_js_runpool_requeue_or_kill_ctx(struct kbase_device *kbdev, struct kbase_context *kctx, mali_bool has_pm_ref);

/* 
                                                                       
                 
  
                                                                            
                                                                                 
                
  
                                                                            
                                          
                                       
                                                                          
                                                                               
           
                                                                             
                                                        
                                                                                
                                                         
                                                                             
                                                                                   
  
                                                                          
                              
                                                                             
                                                                           
                                             
                                                                             
                                                                        
                                                                             
                                                                                 
                                       
  
                                                                               
                    
  
                                                                            
                                                  
  
                                                           
                                                                               
                              
                                                                
                                                                               
                       
                                                                                                         
                                                                             
                       
  
 */
void kbasep_js_runpool_release_ctx(struct kbase_device *kbdev, struct kbase_context *kctx);

/* 
                                                                            
                                   
  
                                                                         
                                            
  
                                                                               
                               
                                            
                                                                             
                                    
  
                                                                    
                                  
 */
void kbasep_js_runpool_release_ctx_and_katom_retained_state(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbasep_js_atom_retained_state *katom_retained_state);

/* 
                                                                          
                                                                         
  
                                           
                                                                             
                                                                              
                                                                               
                                                                                    
                                                            
  
                                                  
                                             
  
                                                                             
                                                                          
                       
                                                                               
                                                                              
                                                                             
                                                                    
 */
mali_bool kbasep_js_try_run_next_job_on_slot_irq_nolock(struct kbase_device *kbdev, int js, s8 *submit_count);

/* 
                                                                                 
  
                                                                    
  
                                                                               
                                                                         
                                                                               
                                                                            
                                                                     
  
                                                                             
                                                                              
                                                                 
  
                                                           
                                                      
                                                          
  
                                                                         
                                              
  
                                                   
                                               
  
 */
void kbasep_js_try_run_next_job_on_slot_nolock(struct kbase_device *kbdev, int js);

/* 
                                                                                        
  
                                                                                    
                                                 
  
                                                           
                                                      
                                                          
  
                                                   
                                               
  
 */
void kbasep_js_try_run_next_job_nolock(struct kbase_device *kbdev);

/* 
                                                            
  
                                                                         
                                                                              
                               
  
                                                           
                                                                        
                                                                               
                 
  
                                                                        
                                                                     
                                                                             
                                                                               
                                                                        
                            
  
                                                                               
                                                           
                                                                    
                                                
                                         
                                                
  
                                                                             
                                                                               
                                               
  
                                                           
                                                                               
                              
                                                                               
                       
                                                                                                         
                                                                                           
                                                                                
                      
  
 */
void kbasep_js_try_schedule_head_ctx(struct kbase_device *kbdev);

/* 
                                          
  
                                                                  
                                                                                                 
                                          
                                                                                             
                                               
  
                                                           
                                                                               
                              
                                                                               
                       
                                                                                                         
                                                                                           
                                                                                
                      
  
 */
void kbasep_js_schedule_privileged_ctx(struct kbase_device *kbdev, struct kbase_context *kctx);

/* 
                                                                        
  
                                                                
  
                                                           
                                                                               
                              
                                                                
                                                                               
                       
                                                                                                         
  
 */
void kbasep_js_release_privileged_ctx(struct kbase_device *kbdev, struct kbase_context *kctx);

/* 
                                                                           
  
                           
                                         
                                                                                              
                                                                      
                                                                                
                                         
  
                                                                         
                                                                          
                                                                               
  
                                                                        
                                                                          
                                                                                         
                                                                      
  
                                                                                
                                                                               
                                                                        
                                                                       
                                                               
  
                                                           
                                                           
 */
void kbasep_js_job_done_slot_irq(struct kbase_jd_atom *katom, int slot_nr,
                                 ktime_t *end_timestamp,
                                 kbasep_js_atom_done_code done_code);

/* 
                                                 
  
                                   
                                         
                                             
 */
void kbase_js_try_run_jobs(struct kbase_device *kbdev);

/* 
                                                      
  
                                                           
  
                                                                               
                       
                                                                           
                               
  
 */
void kbase_js_try_run_jobs_on_slot(struct kbase_device *kbdev, int js);

/* 
                                                                              
                                                                         
  
                                                                             
                                                                     
  
                                                                                   
                       
                                
                     
          
  
                                                                               
                                                    
  
                                                                            
                                                                            
                                                                           
  
                                                                            
                                                                        
 */
void kbasep_js_job_check_deref_cores(struct kbase_device *kbdev, struct kbase_jd_atom *katom);

/* 
                                                                            
  
                                                                       
                                          
  
                                                                              
                                                                       
  
                                                                            
                                                                          
  
                                                                              
                                      
  
                                                                               
                                                                              
                                                                           
                                                                       
                                                                         
 */
void kbasep_js_suspend(struct kbase_device *kbdev);

/* 
                                                                         
  
                                                      
                                             
                                     
 */
void kbasep_js_resume(struct kbase_device *kbdev);


/*
                 
 */

/* 
                                                                       
  
                                                                                   
                                  
  
                                                                     
  
                                                                 
 */
static INLINE mali_bool kbasep_js_is_submit_allowed(struct kbasep_js_device_data *js_devdata, struct kbase_context *kctx)
{
	u16 test_bit;

	/*                                       */
	KBASE_DEBUG_ASSERT(kctx->as_nr != KBASEP_AS_NR_INVALID);
	KBASE_DEBUG_ASSERT(kctx->jctx.sched_info.ctx.is_scheduled != MALI_FALSE);

	test_bit = (u16) (1u << kctx->as_nr);

	return (mali_bool) (js_devdata->runpool_irq.submit_allowed & test_bit);
}

/* 
                                                       
  
                                                                                   
                                  
  
                                                                 
 */
static INLINE void kbasep_js_set_submit_allowed(struct kbasep_js_device_data *js_devdata, struct kbase_context *kctx)
{
	u16 set_bit;

	/*                                       */
	KBASE_DEBUG_ASSERT(kctx->as_nr != KBASEP_AS_NR_INVALID);
	KBASE_DEBUG_ASSERT(kctx->jctx.sched_info.ctx.is_scheduled != MALI_FALSE);

	set_bit = (u16) (1u << kctx->as_nr);

	dev_dbg(kctx->kbdev->dev, "JS: Setting Submit Allowed on %p (as=%d)", kctx, kctx->as_nr);

	js_devdata->runpool_irq.submit_allowed |= set_bit;
}

/* 
                                                                    
  
                                                                                   
                                  
  
                                                                 
 */
static INLINE void kbasep_js_clear_submit_allowed(struct kbasep_js_device_data *js_devdata, struct kbase_context *kctx)
{
	u16 clear_bit;
	u16 clear_mask;

	/*                                       */
	KBASE_DEBUG_ASSERT(kctx->as_nr != KBASEP_AS_NR_INVALID);
	KBASE_DEBUG_ASSERT(kctx->jctx.sched_info.ctx.is_scheduled != MALI_FALSE);

	clear_bit = (u16) (1u << kctx->as_nr);
	clear_mask = ~clear_bit;

	dev_dbg(kctx->kbdev->dev, "JS: Clearing Submit Allowed on %p (as=%d)", kctx, kctx->as_nr);

	js_devdata->runpool_irq.submit_allowed &= clear_mask;
}

/* 
                                                                   
 */
static INLINE void kbasep_js_clear_job_retry_submit(struct kbase_jd_atom *atom)
{
	atom->retry_submit_on_slot = KBASEP_JS_RETRY_SUBMIT_SLOT_INVALID;
}

/* 
                                                                          
                   
  
                                                                           
                                                                              
                                                                          
                                                
  
                                                                          
                                                                             
                               
 */
static INLINE void kbasep_js_set_job_retry_submit_slot(struct kbase_jd_atom *atom, int js)
{
	KBASE_DEBUG_ASSERT(0 <= js && js <= BASE_JM_MAX_NR_SLOTS);
	KBASE_DEBUG_ASSERT(atom->retry_submit_on_slot == KBASEP_JS_RETRY_SUBMIT_SLOT_INVALID
	                   || atom->retry_submit_on_slot == js);

	atom->retry_submit_on_slot = js;
}

/* 
                                                                    
                                                 
                                                           
 */
static INLINE void kbasep_js_atom_retained_state_init_invalid(struct kbasep_js_atom_retained_state *retained_state)
{
	retained_state->event_code = BASE_JD_EVENT_NOT_STARTED;
	retained_state->core_req = KBASEP_JS_ATOM_RETAINED_STATE_CORE_REQ_INVALID;
	retained_state->retry_submit_on_slot = KBASEP_JS_RETRY_SUBMIT_SLOT_INVALID;
}

/* 
                                                                              
                
 */
static INLINE void kbasep_js_atom_retained_state_copy(struct kbasep_js_atom_retained_state *retained_state, const struct kbase_jd_atom *katom)
{
	retained_state->event_code = katom->event_code;
	retained_state->core_req = katom->core_req;
	retained_state->retry_submit_on_slot = katom->retry_submit_on_slot;
	retained_state->device_nr = katom->device_nr;
}

/* 
                                                                            
                                                                    
  
                                             
                                                                               
  
                                                                          
                                                     
                                                   
 */
static INLINE mali_bool kbasep_js_has_atom_finished(const struct kbasep_js_atom_retained_state *katom_retained_state)
{
	return (mali_bool) (katom_retained_state->event_code != BASE_JD_EVENT_STOPPED && katom_retained_state->event_code != BASE_JD_EVENT_REMOVED_FROM_NEXT);
}

/* 
                                                                           
  
                                                                                     
                              
  
                                                                     
                                                                             
                                                         
 */
static INLINE mali_bool kbasep_js_atom_retained_state_is_valid(const struct kbasep_js_atom_retained_state *katom_retained_state)
{
	return (mali_bool) (katom_retained_state->core_req != KBASEP_JS_ATOM_RETAINED_STATE_CORE_REQ_INVALID);
}

static INLINE mali_bool kbasep_js_get_atom_retry_submit_slot(const struct kbasep_js_atom_retained_state *katom_retained_state, int *res)
{
	int js = katom_retained_state->retry_submit_on_slot;
	*res = js;
	return (mali_bool) (js >= 0);
}

#if KBASE_DEBUG_DISABLE_ASSERTS == 0
/* 
                                                                 
  
                                                   
  
                                                            
                                                                              
                                         
 */
static INLINE int kbasep_js_debug_check_ctx_refcount(struct kbase_device *kbdev, struct kbase_context *kctx)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;
	int result = -1;
	int as_nr;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kctx != NULL);
	js_devdata = &kbdev->js_data;

	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);
	as_nr = kctx->as_nr;
	if (as_nr != KBASEP_AS_NR_INVALID)
		result = js_devdata->runpool_irq.per_as_data[as_nr].as_busy_refcount;

	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	return result;
}
#endif				/*                                  */

/* 
                                                                             
                                                           
  
                                                                             
                                                                              
                                                    
  
                                                           
                                                                                
                              
  
                                                                                      
                                                  
                                                                            
 */
static INLINE struct kbase_context *kbasep_js_runpool_lookup_ctx_noretain(struct kbase_device *kbdev, int as_nr)
{
	unsigned long flags;
	struct kbasep_js_device_data *js_devdata;
	struct kbase_context *found_kctx;
	struct kbasep_js_per_as_data *js_per_as_data;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(0 <= as_nr && as_nr < BASE_MAX_NR_AS);
	js_devdata = &kbdev->js_data;
	js_per_as_data = &js_devdata->runpool_irq.per_as_data[as_nr];

	spin_lock_irqsave(&js_devdata->runpool_irq.lock, flags);

	found_kctx = js_per_as_data->kctx;
	KBASE_DEBUG_ASSERT(found_kctx == NULL || js_per_as_data->as_busy_refcount > 0);

	spin_unlock_irqrestore(&js_devdata->runpool_irq.lock, flags);

	return found_kctx;
}

/* 
                                                                          
                                                
                                                                            
                                       
                                                                           
                                                         
 */
static INLINE u32 kbasep_js_convert_us_to_gpu_ticks_min_freq(struct kbase_device *kbdev, u32 us)
{
	u32 gpu_freq = kbdev->gpu_props.props.core_props.gpu_freq_khz_min;
	KBASE_DEBUG_ASSERT(0 != gpu_freq);
	return us * (gpu_freq / 1000);
}

/* 
                                                                          
                                                
                                                                            
                                       
                                                                             
                                                   
 */
static INLINE u32 kbasep_js_convert_us_to_gpu_ticks_max_freq(struct kbase_device *kbdev, u32 us)
{
	u32 gpu_freq = kbdev->gpu_props.props.core_props.gpu_freq_khz_max;
	KBASE_DEBUG_ASSERT(0 != gpu_freq);
	return us * (u32) (gpu_freq / 1000);
}

/* 
                                                                          
                                                
                                                                            
                                       
                                                                           
                                                                           
               
 */
static INLINE u32 kbasep_js_convert_gpu_ticks_to_us_min_freq(struct kbase_device *kbdev, u32 ticks)
{
	u32 gpu_freq = kbdev->gpu_props.props.core_props.gpu_freq_khz_min;
	KBASE_DEBUG_ASSERT(0 != gpu_freq);
	return ticks / gpu_freq * 1000;
}

/* 
                                                                          
                                                
                                                                            
                                       
                                                                        
                                                                         
 */
static INLINE u32 kbasep_js_convert_gpu_ticks_to_us_max_freq(struct kbase_device *kbdev, u32 ticks)
{
	u32 gpu_freq = kbdev->gpu_props.props.core_props.gpu_freq_khz_max;
	KBASE_DEBUG_ASSERT(0 != gpu_freq);
	return ticks / gpu_freq * 1000;
}

	  /*     *//*                    */
	  /*     *//*                          */
	  /*     *//*                    */

#endif				/*              */
