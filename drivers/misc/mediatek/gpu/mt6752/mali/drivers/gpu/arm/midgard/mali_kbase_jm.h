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

#ifndef _KBASE_JM_H_
#define _KBASE_JM_H_

#include <mali_kbase_hw.h>
#include <mali_kbase_debug.h>
#include <linux/atomic.h>

/* 
                       
     
 */

/* 
                             
     
 */

/* 
                                                  
     
  
 */

static INLINE int kbasep_jm_is_js_free(struct kbase_device *kbdev, int js, struct kbase_context *kctx)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(0 <= js && js < kbdev->gpu_props.num_job_slots);

	return !kbase_reg_read(kbdev, JOB_SLOT_REG(js, JS_COMMAND_NEXT), kctx);
}

/* 
                    
                                                                                                      
                                                                                                         
                                                                                   
 */
static INLINE mali_bool kbasep_jm_is_submit_slots_free(struct kbase_device *kbdev, int js, struct kbase_context *kctx)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(0 <= js && js < kbdev->gpu_props.num_job_slots);

	if (atomic_read(&kbdev->reset_gpu) != KBASE_RESET_GPU_NOT_PENDING) {
		/*                                                */
		return MALI_FALSE;
	}

	return (mali_bool) (kbasep_jm_is_js_free(kbdev, js, kctx)
			    && kbdev->jm_slots[js].submitted_nr < (BASE_JM_SUBMIT_SLOTS - 2));
}

/* 
                           
 */
static INLINE void kbasep_jm_init_submit_slot(struct kbase_jm_slot *slot)
{
	slot->submitted_nr = 0;
	slot->submitted_head = 0;
}

/* 
                                                                                                          
 */
static INLINE struct kbase_jd_atom *kbasep_jm_peek_idx_submit_slot(struct kbase_jm_slot *slot, u8 idx)
{
	u8 pos;
	struct kbase_jd_atom *katom;

	KBASE_DEBUG_ASSERT(idx < BASE_JM_SUBMIT_SLOTS);

	pos = (slot->submitted_head + idx) & BASE_JM_SUBMIT_SLOTS_MASK;
	katom = slot->submitted[pos];

	return katom;
}

/* 
                             
 */
static INLINE struct kbase_jd_atom *kbasep_jm_dequeue_submit_slot(struct kbase_jm_slot *slot)
{
	u8 pos;
	struct kbase_jd_atom *katom;

	pos = slot->submitted_head & BASE_JM_SUBMIT_SLOTS_MASK;
	katom = slot->submitted[pos];
	slot->submitted[pos] = NULL;	/*                       */
	KBASE_DEBUG_ASSERT(katom);

	/*                    */
	slot->submitted_head = (slot->submitted_head + 1) & BASE_JM_SUBMIT_SLOTS_MASK;
	slot->submitted_nr--;

	dev_dbg(katom->kctx->kbdev->dev, "katom %p new head %u", (void *)katom, (unsigned int)slot->submitted_head);

	return katom;
}

/*                                                 
 */
static INLINE struct kbase_jd_atom *kbasep_jm_dequeue_tail_submit_slot(struct kbase_jm_slot *slot)
{
	u8 pos;

	slot->submitted_nr--;

	pos = (slot->submitted_head + slot->submitted_nr) & BASE_JM_SUBMIT_SLOTS_MASK;

	return slot->submitted[pos];
}

static INLINE u8 kbasep_jm_nr_jobs_submitted(struct kbase_jm_slot *slot)
{
	return slot->submitted_nr;
}

/* 
                             
 */
static INLINE void kbasep_jm_enqueue_submit_slot(struct kbase_jm_slot *slot, struct kbase_jd_atom *katom)
{
	u8 nr;
	u8 pos;
	nr = slot->submitted_nr++;
	KBASE_DEBUG_ASSERT(nr < BASE_JM_SUBMIT_SLOTS);

	pos = (slot->submitted_head + nr) & BASE_JM_SUBMIT_SLOTS_MASK;
	slot->submitted[pos] = katom;
}

/* 
                                                                        
                                                             
  
                                                                          
                       
  
                                                                               
                                                                          
                                                               
  
                                        
                                    
  
                                                                                 
                                
                                                               
 */
static INLINE mali_bool kbasep_jm_is_dummy_workaround_job(struct kbase_device *kbdev, struct kbase_jd_atom *atom)
{
	/*                                       */
	/*                   */
	return MALI_FALSE;
}

/* 
                                            
  
                                                                                            
  
                                                           
                                                               
 */
void kbase_job_submit_nolock(struct kbase_device *kbdev, struct kbase_jd_atom *katom, int js);

/* 
                                                        
 */
void kbase_job_done_slot(struct kbase_device *kbdev, int s, u32 completion_code, u64 job_tail, ktime_t *end_timestamp);

	  /*     *//*                    */
	  /*     *//*                          */
	  /*     *//*                    */

#endif				/*              */
