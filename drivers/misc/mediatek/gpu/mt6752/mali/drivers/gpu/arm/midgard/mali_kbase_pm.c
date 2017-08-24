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

#include <mali_kbase_pm.h>

#if KBASE_PM_EN

/*              */
#include "mach/mt_gpufreq.h"

unsigned int g_current_gpu_platform_id = 0;

mtk_gpu_freq_limit_data mt6752_gpu_freq_limit_data[MTK_MT6752_GPU_LIMIT_COUNT]=
{ {6, 3, (const int[]){0,3,5}}, //                 
  {4, 3, (const int[]){0,1,3}}, //                                      
  {3, 2, (const int[]){0,2}}    //                              
};

extern unsigned int (*mtk_get_gpu_loading_fp)(void);
extern void (*mtk_boost_gpu_freq_fp)(void);
extern void (*mtk_custom_boost_gpu_freq_fp)(unsigned int ui32FreqLevel);
extern void (*mtk_set_bottom_gpu_freq_fp)(unsigned int ui32FreqLevel);
extern unsigned int (*mtk_custom_get_gpu_freq_level_count_fp)(void);

/*                                   */
extern void mt_gpufreq_input_boost_notify_registerCB(gpufreq_input_boost_notify pCB);
extern void mt_gpufreq_power_limit_notify_registerCB(gpufreq_power_limit_notify pCB);

unsigned int mtk_get_current_gpu_platform_id()
{
    return g_current_gpu_platform_id;
}

void _mtk_gpu_dvfs_init()
{
    int i;
    unsigned int iCurrentFreqCount;
    printk(KERN_EMERG "[MALI] _mtk_gpu_dvfs_init\n");
    
    iCurrentFreqCount = mt_gpufreq_get_dvfs_table_num();
    
    //                          
    for(i=0 ; i<MTK_MT6752_GPU_LIMIT_COUNT ; i++)
    {
        if(iCurrentFreqCount == mt6752_gpu_freq_limit_data[i].actual_freq_index_count)
        {
            g_current_gpu_platform_id = i;
            break;
        }
    }

    //                                                       
    mtk_kbase_custom_boost_gpu_freq(0);
    mtk_kbase_ged_bottom_gpu_freq(0);

}


void kbase_pm_register_access_enable(struct kbase_device *kbdev)
{
	struct kbase_pm_callback_conf *callbacks;

	callbacks = (struct kbase_pm_callback_conf *)kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_POWER_MANAGEMENT_CALLBACKS);

	if (callbacks)
		callbacks->power_on_callback(kbdev);
}

void kbase_pm_register_access_disable(struct kbase_device *kbdev)
{
	struct kbase_pm_callback_conf *callbacks;

	callbacks = (struct kbase_pm_callback_conf *)kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_POWER_MANAGEMENT_CALLBACKS);

	if (callbacks)
		callbacks->power_off_callback(kbdev);
}

mali_error kbase_pm_init(struct kbase_device *kbdev)
{
	mali_error ret = MALI_ERROR_NONE;
	struct kbase_pm_callback_conf *callbacks;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	mutex_init(&kbdev->pm.lock);

	kbdev->pm.gpu_powered = MALI_FALSE;
	kbdev->pm.suspending = MALI_FALSE;
#ifdef CONFIG_MALI_DEBUG
	kbdev->pm.driver_ready_for_irqs = MALI_FALSE;
#endif /*                   */
	kbdev->pm.gpu_in_desired_state = MALI_TRUE;
	init_waitqueue_head(&kbdev->pm.gpu_in_desired_state_wait);

	callbacks = (struct kbase_pm_callback_conf *)kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_POWER_MANAGEMENT_CALLBACKS);
	if (callbacks) {
		kbdev->pm.callback_power_on = callbacks->power_on_callback;
		kbdev->pm.callback_power_off = callbacks->power_off_callback;
		kbdev->pm.callback_power_suspend =
					callbacks->power_suspend_callback;
		kbdev->pm.callback_power_resume =
					callbacks->power_resume_callback;
		kbdev->pm.callback_power_runtime_init = callbacks->power_runtime_init_callback;
		kbdev->pm.callback_power_runtime_term = callbacks->power_runtime_term_callback;
		kbdev->pm.callback_power_runtime_on = callbacks->power_runtime_on_callback;
		kbdev->pm.callback_power_runtime_off = callbacks->power_runtime_off_callback;
	} else {
		kbdev->pm.callback_power_on = NULL;
		kbdev->pm.callback_power_off = NULL;
		kbdev->pm.callback_power_suspend = NULL;
		kbdev->pm.callback_power_resume = NULL;
		kbdev->pm.callback_power_runtime_init = NULL;
		kbdev->pm.callback_power_runtime_term = NULL;
		kbdev->pm.callback_power_runtime_on = NULL;
		kbdev->pm.callback_power_runtime_off = NULL;
	}

	/*                   */
	_mtk_gpu_dvfs_init();

	/*                                                             */
	mt_gpufreq_input_boost_notify_registerCB(mtk_gpu_input_boost_CB);
	mt_gpufreq_power_limit_notify_registerCB(mtk_gpu_power_limit_CB);

	/*                                        */
	mtk_boost_gpu_freq_fp = mtk_kbase_boost_gpu_freq;
	mtk_custom_boost_gpu_freq_fp = mtk_kbase_custom_boost_gpu_freq; /*                                        */
    mtk_set_bottom_gpu_freq_fp = mtk_kbase_ged_bottom_gpu_freq; /*                    */
	mtk_custom_get_gpu_freq_level_count_fp = mtk_kbase_custom_get_gpu_freq_level_count;

	/*             */
	mtk_get_gpu_loading_fp = kbasep_get_gl_utilization;

	kbdev->pm.platform_dvfs_frequency = (u32) kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_POWER_MANAGEMENT_DVFS_FREQ);

	/*                                  */
	ret = kbasep_pm_metrics_init(kbdev);
	if (MALI_ERROR_NONE != ret)
		return ret;

	init_waitqueue_head(&kbdev->pm.l2_powered_wait);
	kbdev->pm.l2_powered = 0;

	init_waitqueue_head(&kbdev->pm.reset_done_wait);
	kbdev->pm.reset_done = MALI_FALSE;

	init_waitqueue_head(&kbdev->pm.zero_active_count_wait);
	kbdev->pm.active_count = 0;

	spin_lock_init(&kbdev->pm.power_change_lock);
	spin_lock_init(&kbdev->pm.gpu_cycle_counter_requests_lock);
	spin_lock_init(&kbdev->pm.gpu_powered_lock);

	if (MALI_ERROR_NONE != kbase_pm_ca_init(kbdev))
		goto workq_fail;

	if (MALI_ERROR_NONE != kbase_pm_policy_init(kbdev))
		goto pm_policy_fail;

	return MALI_ERROR_NONE;

pm_policy_fail:
	kbase_pm_ca_term(kbdev);
workq_fail:
	kbasep_pm_metrics_term(kbdev);
	return MALI_ERROR_FUNCTION_FAILED;
}

KBASE_EXPORT_TEST_API(kbase_pm_init)

void kbase_pm_do_poweron(struct kbase_device *kbdev, mali_bool is_resume)
{
	lockdep_assert_held(&kbdev->pm.lock);

	/*                                                                    
                         */
	kbase_pm_clock_on(kbdev, is_resume);

	/*                                              */
	KBASE_TIMELINE_PM_CHECKTRANS(kbdev, SW_FLOW_PM_CHECKTRANS_PM_DO_POWERON_START);
	kbase_pm_update_cores_state(kbdev);
	KBASE_TIMELINE_PM_CHECKTRANS(kbdev, SW_FLOW_PM_CHECKTRANS_PM_DO_POWERON_END);

	/*                                                                    
                                                  */
}

void kbase_pm_do_poweroff(struct kbase_device *kbdev, mali_bool is_suspend)
{
	unsigned long flags;
	mali_bool cores_are_available;

	lockdep_assert_held(&kbdev->pm.lock);

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	/*                     */
	kbdev->pm.desired_shader_state = 0;

	/*                                                          
                                                              
                                                             
                        */
	kbdev->shader_available_bitmap = 0;
	kbdev->tiler_available_bitmap = 0;
	kbdev->l2_available_bitmap = 0;

	KBASE_TIMELINE_PM_CHECKTRANS(kbdev, SW_FLOW_PM_CHECKTRANS_PM_DO_POWEROFF_START);
	cores_are_available = kbase_pm_check_transitions_nolock(kbdev);
	KBASE_TIMELINE_PM_CHECKTRANS(kbdev, SW_FLOW_PM_CHECKTRANS_PM_DO_POWEROFF_END);
	/*                                                                    */
	CSTD_UNUSED(cores_are_available);

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	/*                                                                     
                                                                      
                                                                          
                                               */

	/*                                 */
	kbase_timeline_pm_check_handle_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_STATE_CHANGED);
	/*                                           */
	kbase_pm_clock_off(kbdev, is_suspend);
}

mali_error kbase_pm_powerup(struct kbase_device *kbdev)
{
	unsigned long flags;
	mali_error ret;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	mutex_lock(&kbdev->pm.lock);

	/*                                              */
	KBASE_DEBUG_ASSERT(!kbase_pm_is_suspending(kbdev));

	/*                                                                          */
	ret = kbase_pm_init_hw(kbdev, MALI_FALSE);
	if (ret != MALI_ERROR_NONE) {
		mutex_unlock(&kbdev->pm.lock);
		return ret;
	}

	kbasep_pm_read_present_cores(kbdev);

	kbdev->pm.debug_core_mask = kbdev->shader_present_bitmap;

	/*                                                                               */
	kbdev->pm.active_count = 1;

	spin_lock_irqsave(&kbdev->pm.gpu_cycle_counter_requests_lock, flags);
	/*                             */
	kbdev->pm.gpu_cycle_counter_requests = 0;
	spin_unlock_irqrestore(&kbdev->pm.gpu_cycle_counter_requests_lock, flags);

	/*                                                                                  */
#ifdef CONFIG_MALI_DEBUG
	spin_lock_irqsave(&kbdev->pm.gpu_powered_lock, flags);
	kbdev->pm.driver_ready_for_irqs = MALI_TRUE;
	spin_unlock_irqrestore(&kbdev->pm.gpu_powered_lock, flags);
#endif
	kbase_pm_enable_interrupts(kbdev);

	/*                                                    */
	kbase_pm_do_poweron(kbdev, MALI_FALSE);
	mutex_unlock(&kbdev->pm.lock);

	/*                                                      */
	kbase_pm_context_idle(kbdev);

	return MALI_ERROR_NONE;
}

KBASE_EXPORT_TEST_API(kbase_pm_powerup)

void kbase_pm_context_active(struct kbase_device *kbdev)
{
	(void)kbase_pm_context_active_handle_suspend(kbdev, KBASE_PM_SUSPEND_HANDLER_NOT_POSSIBLE);
}

int kbase_pm_context_active_handle_suspend(struct kbase_device *kbdev, enum kbase_pm_suspend_handler suspend_handler)
{
	int c;
	int old_count;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	/*                                                                         
                                                                            
                                                                   
             */
	old_count = kbdev->pm.active_count;
	if (old_count == 0)
		kbase_timeline_pm_send_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_ACTIVE);

	mutex_lock(&kbdev->pm.lock);
	if (kbase_pm_is_suspending(kbdev)) {
		switch (suspend_handler) {
		case KBASE_PM_SUSPEND_HANDLER_DONT_REACTIVATE:
			if (kbdev->pm.active_count != 0)
				break;
			/*             */
		case KBASE_PM_SUSPEND_HANDLER_DONT_INCREASE:
			mutex_unlock(&kbdev->pm.lock);
			if (old_count == 0)
				kbase_timeline_pm_handle_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_ACTIVE);
			return 1;

		case KBASE_PM_SUSPEND_HANDLER_NOT_POSSIBLE:
			/*             */
		default:
			KBASE_DEBUG_ASSERT_MSG(MALI_FALSE, "unreachable");
			break;
		}
	}
	c = ++kbdev->pm.active_count;
	KBASE_TIMELINE_CONTEXT_ACTIVE(kbdev, c);

	KBASE_TRACE_ADD_REFCOUNT(kbdev, PM_CONTEXT_ACTIVE, NULL, NULL, 0u, c);

	/*                               */
	if (old_count == 0)
		kbase_timeline_pm_handle_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_ACTIVE);

	if (c == 1) {
		/*                                                                  
                */
		kbase_pm_update_active(kbdev);

		kbasep_pm_record_gpu_active(kbdev);
	}

	mutex_unlock(&kbdev->pm.lock);

	return 0;
}

KBASE_EXPORT_TEST_API(kbase_pm_context_active)

void kbase_pm_context_idle(struct kbase_device *kbdev)
{
	int c;
	int old_count;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	/*                                                                         
                                                                        
                                                                         
             */
	old_count = kbdev->pm.active_count;
	if (old_count == 0)
		kbase_timeline_pm_send_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_IDLE);

	mutex_lock(&kbdev->pm.lock);

	c = --kbdev->pm.active_count;
	KBASE_TIMELINE_CONTEXT_ACTIVE(kbdev, c);

	KBASE_TRACE_ADD_REFCOUNT(kbdev, PM_CONTEXT_IDLE, NULL, NULL, 0u, c);

	KBASE_DEBUG_ASSERT(c >= 0);

	/*                               */
	if (old_count == 0)
		kbase_timeline_pm_handle_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_IDLE);

	if (c == 0) {
		/*                            */
		kbase_pm_update_active(kbdev);

		kbasep_pm_record_gpu_idle(kbdev);

		/*                                                                
                                                                  
             */
		wake_up(&kbdev->pm.zero_active_count_wait);
	}

	mutex_unlock(&kbdev->pm.lock);
}

KBASE_EXPORT_TEST_API(kbase_pm_context_idle)

void kbase_pm_halt(struct kbase_device *kbdev)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);

	mutex_lock(&kbdev->pm.lock);
	kbase_pm_cancel_deferred_poweroff(kbdev);
	kbase_pm_do_poweroff(kbdev, MALI_FALSE);
	mutex_unlock(&kbdev->pm.lock);
}

KBASE_EXPORT_TEST_API(kbase_pm_halt)

void kbase_pm_term(struct kbase_device *kbdev)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(kbdev->pm.active_count == 0);
	KBASE_DEBUG_ASSERT(kbdev->pm.gpu_cycle_counter_requests == 0);

	/*                                         */
	kbase_pm_policy_term(kbdev);
	kbase_pm_ca_term(kbdev);

	/*                                 */
	kbasep_pm_metrics_term(kbdev);
}

KBASE_EXPORT_TEST_API(kbase_pm_term)

void kbase_pm_suspend(struct kbase_device *kbdev)
{
	int nr_keep_gpu_powered_ctxs;

	KBASE_DEBUG_ASSERT(kbdev);

	mutex_lock(&kbdev->pm.lock);
	KBASE_DEBUG_ASSERT(!kbase_pm_is_suspending(kbdev));
	kbdev->pm.suspending = MALI_TRUE;
	mutex_unlock(&kbdev->pm.lock);

	/*                                                                       
                                                                           
                                                                      */

	/*                                                                         
                                   */
	kbasep_js_suspend(kbdev);

	/*                                                        */
	kbase_instr_hwcnt_suspend(kbdev);

	/*                                   */
	for (nr_keep_gpu_powered_ctxs = atomic_read(&kbdev->keep_gpu_powered_count);
		 nr_keep_gpu_powered_ctxs > 0;
		 --nr_keep_gpu_powered_ctxs) {
		kbase_pm_context_idle(kbdev);
	}

	/*                                                                 
                                                                         
                  */
	wait_event(kbdev->pm.zero_active_count_wait, kbdev->pm.active_count == 0);

	/*                                                             
                                                              */

	/*                                                                         
                                                                       
                 */
	mutex_lock(&kbdev->pm.lock);
	kbase_pm_cancel_deferred_poweroff(kbdev);
	kbase_pm_do_poweroff(kbdev, MALI_TRUE);
	mutex_unlock(&kbdev->pm.lock);
}

void kbase_pm_resume(struct kbase_device *kbdev)
{
	int nr_keep_gpu_powered_ctxs;

	/*                                                      */
	mutex_lock(&kbdev->pm.lock);
	kbdev->pm.suspending = MALI_FALSE;
	kbase_pm_do_poweron(kbdev, MALI_TRUE);
	mutex_unlock(&kbdev->pm.lock);

	/*                                                          */
	kbase_pm_context_active(kbdev);

	/*                                    */
	for (nr_keep_gpu_powered_ctxs = atomic_read(&kbdev->keep_gpu_powered_count);
		 nr_keep_gpu_powered_ctxs > 0;
		 --nr_keep_gpu_powered_ctxs) {
		kbase_pm_context_active(kbdev);
	}

	/*                                                          */
	kbase_instr_hwcnt_resume(kbdev);

	/*                                                                      
                                */
	kbase_resume_suspended_soft_jobs(kbdev);

	/*                                                                      
          */
	kbasep_js_resume(kbdev);

	/*                                                                     
                                              */
	kbase_pm_context_idle(kbdev);
}
#endif /*             */
