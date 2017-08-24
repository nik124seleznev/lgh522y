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
#include <mali_midg_regmap.h>

/* 
                                                            
 */
static void kbasep_instr_hwcnt_cacheclean(struct kbase_device *kbdev)
{
	unsigned long flags;
	unsigned long pm_flags;
	u32 irq_mask;

	KBASE_DEBUG_ASSERT(NULL != kbdev);

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	/*                                */
	while (kbdev->hwcnt.state == KBASE_INSTR_STATE_RESETTING) {
		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
		wait_event(kbdev->hwcnt.cache_clean_wait,
				kbdev->hwcnt.state != KBASE_INSTR_STATE_RESETTING);
		spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	}
	KBASE_DEBUG_ASSERT(kbdev->hwcnt.state == KBASE_INSTR_STATE_REQUEST_CLEAN);

	/*                  */
	spin_lock_irqsave(&kbdev->pm.power_change_lock, pm_flags);
	irq_mask = kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), NULL);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), irq_mask | CLEAN_CACHES_COMPLETED, NULL);
	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, pm_flags);

	/*                                                                                       */
	KBASE_TRACE_ADD(kbdev, CORE_GPU_CLEAN_INV_CACHES, NULL, NULL, 0u, 0);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_CLEAN_INV_CACHES, NULL);
	kbdev->hwcnt.state = KBASE_INSTR_STATE_CLEANING;

	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
}

STATIC mali_error kbase_instr_hwcnt_enable_internal(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_uk_hwcnt_setup *setup)
{
	unsigned long flags, pm_flags;
	mali_error err = MALI_ERROR_FUNCTION_FAILED;
	struct kbasep_js_device_data *js_devdata;
	u32 irq_mask;
	int ret;
	u64 shader_cores_needed;

	KBASE_DEBUG_ASSERT(NULL != kctx);
	KBASE_DEBUG_ASSERT(NULL != kbdev);
	KBASE_DEBUG_ASSERT(NULL != setup);
	KBASE_DEBUG_ASSERT(NULL == kbdev->hwcnt.suspended_kctx);

	shader_cores_needed = kbase_pm_get_present_cores(kbdev, KBASE_PM_CORE_SHADER);

	js_devdata = &kbdev->js_data;

	/*                   */
	if ((setup->dump_buffer == 0ULL) || (setup->dump_buffer & (2048 - 1)))
		goto out_err;

	/*                                                                     */
	kbase_pm_ca_instr_enable(kbdev);

	/*                                                         */
	/*                                                                         
            */
	kbase_pm_context_active(kbdev);

	/*                                                                            
                                          */
	kbase_pm_request_cores_sync(kbdev, MALI_TRUE, shader_cores_needed);

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);

	if (kbdev->hwcnt.state == KBASE_INSTR_STATE_RESETTING) {
		/*                    */
		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
		wait_event(kbdev->hwcnt.wait, kbdev->hwcnt.triggered != 0);
		spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	}

	if (kbdev->hwcnt.state != KBASE_INSTR_STATE_DISABLED) {
		/*                                    */
		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
		goto out_unrequest_cores;
	}

	/*                  */
	spin_lock_irqsave(&kbdev->pm.power_change_lock, pm_flags);
	irq_mask = kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), NULL);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), irq_mask | PRFCNT_SAMPLE_COMPLETED, NULL);
	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, pm_flags);

	/*                                   */
	kbdev->hwcnt.kctx = kctx;
	/*                                                        */
	kbdev->hwcnt.addr = setup->dump_buffer;
	/*                                              */
	if (&kbdev->hwcnt.suspended_state != setup)
		memcpy(&kbdev->hwcnt.suspended_state, setup, sizeof(kbdev->hwcnt.suspended_state));

	/*                   */
	kbdev->hwcnt.state = KBASE_INSTR_STATE_REQUEST_CLEAN;
	kbdev->hwcnt.triggered = 0;
	/*                                                                                       */
	ret = queue_work(kbdev->hwcnt.cache_clean_wq, &kbdev->hwcnt.cache_clean_work);
	KBASE_DEBUG_ASSERT(ret);

	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);

	/*                                 */
	wait_event(kbdev->hwcnt.wait, kbdev->hwcnt.triggered != 0);

	KBASE_DEBUG_ASSERT(kbdev->hwcnt.state == KBASE_INSTR_STATE_IDLE);

	/*                         */
	kbasep_js_schedule_privileged_ctx(kbdev, kctx);

	/*           */
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_CONFIG), (kctx->as_nr << PRFCNT_CONFIG_AS_SHIFT) | PRFCNT_CONFIG_MODE_OFF, kctx);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_BASE_LO),     setup->dump_buffer & 0xFFFFFFFF, kctx);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_BASE_HI),     setup->dump_buffer >> 32,        kctx);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_JM_EN),       setup->jm_bm,                    kctx);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_SHADER_EN),   setup->shader_bm,                kctx);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_L3_CACHE_EN), setup->l3_cache_bm,              kctx);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_MMU_L2_EN),   setup->mmu_l2_bm,                kctx);
	/*                                                                                      */
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8186))
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_TILER_EN), 0, kctx);
	else
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_TILER_EN), setup->tiler_bm, kctx);

	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_CONFIG), (kctx->as_nr << PRFCNT_CONFIG_AS_SHIFT) | PRFCNT_CONFIG_MODE_MANUAL, kctx);

	/*                                                                      */
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8186))
		kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_TILER_EN), setup->tiler_bm, kctx);

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);

	if (kbdev->hwcnt.state == KBASE_INSTR_STATE_RESETTING) {
		/*                    */
		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
		wait_event(kbdev->hwcnt.wait, kbdev->hwcnt.triggered != 0);
		spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	}

	kbdev->hwcnt.state = KBASE_INSTR_STATE_IDLE;
	kbdev->hwcnt.triggered = 1;
	wake_up(&kbdev->hwcnt.wait);

	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);

	err = MALI_ERROR_NONE;

	dev_dbg(kbdev->dev, "HW counters dumping set-up for context %p", kctx);
	return err;
 out_unrequest_cores:
	kbase_pm_unrequest_cores(kbdev, MALI_TRUE, shader_cores_needed);
	kbase_pm_context_idle(kbdev);
 out_err:
	return err;
}

/* 
                                       
  
                                                
 */
mali_error kbase_instr_hwcnt_enable(struct kbase_context *kctx, struct kbase_uk_hwcnt_setup *setup)
{
	struct kbase_device *kbdev;
	mali_bool access_allowed;

	kbdev = kctx->kbdev;

	KBASE_DEBUG_ASSERT(NULL != kctx);
	/*                                                             */
	access_allowed = kbase_security_has_capability(kctx, KBASE_SEC_INSTR_HW_COUNTERS_COLLECT, KBASE_SEC_FLAG_NOAUDIT);
	if (MALI_FALSE == access_allowed)
		return MALI_ERROR_FUNCTION_FAILED;

	return kbase_instr_hwcnt_enable_internal(kbdev, kctx, setup);
}
KBASE_EXPORT_SYMBOL(kbase_instr_hwcnt_enable)

/* 
                                        
  
                                                             
 */
mali_error kbase_instr_hwcnt_disable(struct kbase_context *kctx)
{
	unsigned long flags, pm_flags;
	mali_error err = MALI_ERROR_FUNCTION_FAILED;
	u32 irq_mask;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(NULL != kctx);
	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(NULL != kbdev);

	while (1) {
		spin_lock_irqsave(&kbdev->hwcnt.lock, flags);

		if (kbdev->hwcnt.state == KBASE_INSTR_STATE_DISABLED) {
			/*                                */
			spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
			goto out;
		}

		if (kbdev->hwcnt.kctx != kctx) {
			/*                                                    */
			spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
			goto out;
		}

		if (kbdev->hwcnt.state == KBASE_INSTR_STATE_IDLE)
			break;

		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);

		/*                                              */
		wait_event(kbdev->hwcnt.wait, kbdev->hwcnt.triggered != 0);
	}

	kbdev->hwcnt.state = KBASE_INSTR_STATE_DISABLED;
	kbdev->hwcnt.triggered = 0;

	/*                   */
	spin_lock_irqsave(&kbdev->pm.power_change_lock, pm_flags);
	irq_mask = kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), NULL);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), irq_mask & ~PRFCNT_SAMPLE_COMPLETED, NULL);
	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, pm_flags);

	/*                      */
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_CONFIG), 0, kctx);

	kbdev->hwcnt.kctx = NULL;
	kbdev->hwcnt.addr = 0ULL;

	kbase_pm_ca_instr_disable(kbdev);

	kbase_pm_unrequest_cores(kbdev, MALI_TRUE, kbase_pm_get_present_cores(kbdev, KBASE_PM_CORE_SHADER));

	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);

	/*                                                                      */
	kbasep_js_release_privileged_ctx(kbdev, kctx);

	/*                                                 */
	kbase_pm_context_idle(kbdev);

	dev_dbg(kbdev->dev, "HW counters dumping disabled for context %p", kctx);

	err = MALI_ERROR_NONE;

 out:
	return err;
}
KBASE_EXPORT_SYMBOL(kbase_instr_hwcnt_disable)

/* 
                                          
 */
mali_error kbase_instr_hwcnt_setup(struct kbase_context *kctx, struct kbase_uk_hwcnt_setup *setup)
{
	mali_error err = MALI_ERROR_FUNCTION_FAILED;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(NULL != kctx);

	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(NULL != kbdev);

	if (NULL == setup) {
		/*                       */
		goto out;
	}

	if (setup->dump_buffer != 0ULL) {
		/*                    */
		err = kbase_instr_hwcnt_enable(kctx, setup);
	} else {
		/*                     */
		err = kbase_instr_hwcnt_disable(kctx);
	}

 out:
	return err;
}

/* 
                                        
  
         
                   
 */
mali_error kbase_instr_hwcnt_dump_irq(struct kbase_context *kctx)
{
	unsigned long flags;
	mali_error err = MALI_ERROR_FUNCTION_FAILED;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(NULL != kctx);
	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(NULL != kbdev);

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);

	if (kbdev->hwcnt.kctx != kctx) {
		/*                                                        */
		goto unlock;
	}

	if (kbdev->hwcnt.state != KBASE_INSTR_STATE_IDLE) {
		/*                                                                         */
		goto unlock;
	}

	kbdev->hwcnt.triggered = 0;

	/*                                                                     */
	kbdev->hwcnt.state = KBASE_INSTR_STATE_DUMPING;

	/*                              */
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_BASE_LO), kbdev->hwcnt.addr & 0xFFFFFFFF, NULL);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(PRFCNT_BASE_HI), kbdev->hwcnt.addr >> 32, NULL);

	/*               */
	KBASE_TRACE_ADD(kbdev, CORE_GPU_PRFCNT_SAMPLE, NULL, NULL, kbdev->hwcnt.addr, 0);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_PRFCNT_SAMPLE, kctx);

	dev_dbg(kbdev->dev, "HW counters dumping done for context %p", kctx);

	err = MALI_ERROR_NONE;

 unlock:
	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
	return err;
}
KBASE_EXPORT_SYMBOL(kbase_instr_hwcnt_dump_irq)

/* 
                                                         
  
         
                   
                                                              
                          
 */
mali_bool kbase_instr_hwcnt_dump_complete(struct kbase_context *kctx, mali_bool * const success)
{
	unsigned long flags;
	mali_bool complete = MALI_FALSE;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(NULL != kctx);
	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(NULL != kbdev);
	KBASE_DEBUG_ASSERT(NULL != success);

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);

	if (kbdev->hwcnt.state == KBASE_INSTR_STATE_IDLE) {
		*success = MALI_TRUE;
		complete = MALI_TRUE;
	} else if (kbdev->hwcnt.state == KBASE_INSTR_STATE_FAULT) {
		*success = MALI_FALSE;
		complete = MALI_TRUE;
		kbdev->hwcnt.state = KBASE_INSTR_STATE_IDLE;
	}

	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);

	return complete;
}
KBASE_EXPORT_SYMBOL(kbase_instr_hwcnt_dump_complete)

/* 
                                                                
 */
mali_error kbase_instr_hwcnt_dump(struct kbase_context *kctx)
{
	unsigned long flags;
	mali_error err = MALI_ERROR_FUNCTION_FAILED;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(NULL != kctx);
	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(NULL != kbdev);

	err = kbase_instr_hwcnt_dump_irq(kctx);
	if (MALI_ERROR_NONE != err) {
		/*                        */
		goto out;
	}

	/*                                        */
	wait_event(kbdev->hwcnt.wait, kbdev->hwcnt.triggered != 0);

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);

	if (kbdev->hwcnt.state == KBASE_INSTR_STATE_RESETTING) {
		/*                    */
		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
		wait_event(kbdev->hwcnt.wait, kbdev->hwcnt.triggered != 0);
		spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	}

	if (kbdev->hwcnt.state == KBASE_INSTR_STATE_FAULT) {
		err = MALI_ERROR_FUNCTION_FAILED;
		kbdev->hwcnt.state = KBASE_INSTR_STATE_IDLE;
	} else {
		/*           */
		KBASE_DEBUG_ASSERT(kbdev->hwcnt.state == KBASE_INSTR_STATE_IDLE);
		err = MALI_ERROR_NONE;
	}

	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
 out:
	return err;
}
KBASE_EXPORT_SYMBOL(kbase_instr_hwcnt_dump)

/* 
                               
 */
mali_error kbase_instr_hwcnt_clear(struct kbase_context *kctx)
{
	unsigned long flags;
	mali_error err = MALI_ERROR_FUNCTION_FAILED;
	struct kbase_device *kbdev;

	KBASE_DEBUG_ASSERT(NULL != kctx);
	kbdev = kctx->kbdev;
	KBASE_DEBUG_ASSERT(NULL != kbdev);

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);

	if (kbdev->hwcnt.state == KBASE_INSTR_STATE_RESETTING) {
		/*                    */
		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
		wait_event(kbdev->hwcnt.wait, kbdev->hwcnt.triggered != 0);
		spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	}

	/*                                                                        */
	if (kbdev->hwcnt.kctx != kctx || kbdev->hwcnt.state != KBASE_INSTR_STATE_IDLE)
		goto out;

	/*                    */
	KBASE_TRACE_ADD(kbdev, CORE_GPU_PRFCNT_CLEAR, NULL, NULL, 0u, 0);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_PRFCNT_CLEAR, kctx);

	err = MALI_ERROR_NONE;

 out:
	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
	return err;
}
KBASE_EXPORT_SYMBOL(kbase_instr_hwcnt_clear)

/* 
                                        
 */
void kbasep_cache_clean_worker(struct work_struct *data)
{
	struct kbase_device *kbdev;
	unsigned long flags;

	kbdev = container_of(data, struct kbase_device, hwcnt.cache_clean_work);

	mutex_lock(&kbdev->cacheclean_lock);
	kbasep_instr_hwcnt_cacheclean(kbdev);

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	/*                                                   */
	while (kbdev->hwcnt.state == KBASE_INSTR_STATE_RESETTING ||
			kbdev->hwcnt.state == KBASE_INSTR_STATE_CLEANING) {
		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
		wait_event(kbdev->hwcnt.cache_clean_wait,
		           (kbdev->hwcnt.state != KBASE_INSTR_STATE_RESETTING
		            && kbdev->hwcnt.state != KBASE_INSTR_STATE_CLEANING));
		spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
	}
	KBASE_DEBUG_ASSERT(kbdev->hwcnt.state == KBASE_INSTR_STATE_CLEANED);

	/*                       */
	kbdev->hwcnt.state = KBASE_INSTR_STATE_IDLE;
	kbdev->hwcnt.triggered = 1;
	wake_up(&kbdev->hwcnt.wait);

	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
	mutex_unlock(&kbdev->cacheclean_lock);
}

/* 
                                          
 */
void kbase_instr_hwcnt_sample_done(struct kbase_device *kbdev)
{
	unsigned long flags;

	spin_lock_irqsave(&kbdev->hwcnt.lock, flags);

	if (kbdev->hwcnt.state == KBASE_INSTR_STATE_FAULT) {
		kbdev->hwcnt.triggered = 1;
		wake_up(&kbdev->hwcnt.wait);
	} else if (kbdev->hwcnt.state == KBASE_INSTR_STATE_DUMPING) {
		int ret;
		/*                                                               */
		kbdev->hwcnt.state = KBASE_INSTR_STATE_REQUEST_CLEAN;
		ret = queue_work(kbdev->hwcnt.cache_clean_wq, &kbdev->hwcnt.cache_clean_work);
		KBASE_DEBUG_ASSERT(ret);
	}
	/*                                                                  
                                                            
                                                               */

	spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
}

/* 
                                        
 */
void kbase_clean_caches_done(struct kbase_device *kbdev)
{
	u32 irq_mask;

	if (kbdev->hwcnt.state != KBASE_INSTR_STATE_DISABLED) {
		unsigned long flags;
		unsigned long pm_flags;
		spin_lock_irqsave(&kbdev->hwcnt.lock, flags);
		/*                   */
		spin_lock_irqsave(&kbdev->pm.power_change_lock, pm_flags);
		irq_mask = kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), NULL);
		kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), irq_mask & ~CLEAN_CACHES_COMPLETED, NULL);
		spin_unlock_irqrestore(&kbdev->pm.power_change_lock, pm_flags);

		/*           */
		if (kbdev->hwcnt.state == KBASE_INSTR_STATE_CLEANING) {
			/*                                   */
			kbdev->hwcnt.state = KBASE_INSTR_STATE_CLEANED;
			wake_up(&kbdev->hwcnt.cache_clean_wait);
		}
		/*                                                                  
                                                             
                                                                */

		spin_unlock_irqrestore(&kbdev->hwcnt.lock, flags);
	}
}


/*                                                                   
                                                        
                                                                         
                                            */
void kbase_instr_hwcnt_suspend(struct kbase_device *kbdev)
{
	struct kbase_context *kctx;

	KBASE_DEBUG_ASSERT(kbdev);
	KBASE_DEBUG_ASSERT(!kbdev->hwcnt.suspended_kctx);

	kctx = kbdev->hwcnt.kctx;
	kbdev->hwcnt.suspended_kctx = kctx;

	/*                                                                      
             */

	if (kctx) {
		KBASE_DEBUG_ASSERT(kctx->jctx.sched_info.ctx.flags & KBASE_CTX_FLAG_PRIVILEGED);
		kbase_instr_hwcnt_disable(kctx);
	}
}

void kbase_instr_hwcnt_resume(struct kbase_device *kbdev)
{
	struct kbase_context *kctx;

	KBASE_DEBUG_ASSERT(kbdev);

	kctx = kbdev->hwcnt.suspended_kctx;
	kbdev->hwcnt.suspended_kctx = NULL;

	if (kctx) {
		mali_error err;
		err = kbase_instr_hwcnt_enable_internal(kbdev, kctx, &kbdev->hwcnt.suspended_state);
		WARN(err != MALI_ERROR_NONE,
		     "Failed to restore instrumented hardware counters on resume\n");
	}
}

