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
#include <mali_kbase_config_defaults.h>
#include <mali_midg_regmap.h>
#include <mali_kbase_gator.h>
#include <mali_kbase_pm.h>
#include <mali_kbase_config_defaults.h>

#if KBASE_PM_EN

#if MALI_MOCK_TEST
#define MOCKABLE(function) function##_original
#else
#define MOCKABLE(function) function
#endif				/*                */

/*                                          
  
                                                                                                       
                                                                    
 */
enum kbasep_pm_action {
	ACTION_PRESENT = 0,
	ACTION_READY = (SHADER_READY_LO - SHADER_PRESENT_LO),
	ACTION_PWRON = (SHADER_PWRON_LO - SHADER_PRESENT_LO),
	ACTION_PWROFF = (SHADER_PWROFF_LO - SHADER_PRESENT_LO),
	ACTION_PWRTRANS = (SHADER_PWRTRANS_LO - SHADER_PRESENT_LO),
	ACTION_PWRACTIVE = (SHADER_PWRACTIVE_LO - SHADER_PRESENT_LO)
};

/*                                              
  
                                                                                                               
                                                                                                                   
                                                                                
  
                                    
                                      
  
                                                                                                                    
             
 */
static u32 core_type_to_reg(enum kbase_pm_core_type core_type, enum kbasep_pm_action action)
{
	return (u32)core_type + (u32)action;
}

/*                                 
  
                                                                                                                  
                                                             
  
                                                            
                                                                           
                                                                              
                                                      
 */
STATIC void kbase_pm_invoke(struct kbase_device *kbdev, enum kbase_pm_core_type core_type, u64 cores, enum kbasep_pm_action action)
{
	u32 reg;
	u32 lo = cores & 0xFFFFFFFF;
	u32 hi = (cores >> 32) & 0xFFFFFFFF;

	lockdep_assert_held(&kbdev->pm.power_change_lock);

	reg = core_type_to_reg(core_type, action);

	KBASE_DEBUG_ASSERT(reg);
#ifdef CONFIG_MALI_GATOR_SUPPORT
	if (cores) {
		if (action == ACTION_PWRON)
			kbase_trace_mali_pm_power_on(core_type, cores);
		else if (action == ACTION_PWROFF)
			kbase_trace_mali_pm_power_off(core_type, cores);
	}
#endif				/*                           */
	/*         */
	if (cores) {
		if (action == ACTION_PWRON)
			switch (core_type) {
			case KBASE_PM_CORE_SHADER:
				KBASE_TRACE_ADD(kbdev, PM_PWRON, NULL, NULL, 0u, lo);
				break;
			case KBASE_PM_CORE_TILER:
				KBASE_TRACE_ADD(kbdev, PM_PWRON_TILER, NULL, NULL, 0u, lo);
				break;
			case KBASE_PM_CORE_L2:
				KBASE_TRACE_ADD(kbdev, PM_PWRON_L2, NULL, NULL, 0u, lo);
				break;
			default:
				/*                */
				break;
			}
		else if (action == ACTION_PWROFF)
			switch (core_type) {
			case KBASE_PM_CORE_SHADER:
				KBASE_TRACE_ADD(kbdev, PM_PWROFF, NULL, NULL, 0u, lo);
				break;
			case KBASE_PM_CORE_TILER:
				KBASE_TRACE_ADD(kbdev, PM_PWROFF_TILER, NULL, NULL, 0u, lo);
				break;
			case KBASE_PM_CORE_L2:
				KBASE_TRACE_ADD(kbdev, PM_PWROFF_L2, NULL, NULL, 0u, lo);
				break;
			default:
				/*                */
				break;
			}
	}

	if (lo != 0)
		kbase_reg_write(kbdev, GPU_CONTROL_REG(reg), lo, NULL);

	if (hi != 0)
		kbase_reg_write(kbdev, GPU_CONTROL_REG(reg + 4), hi, NULL);
}

/*                                  
  
                                                                                                                     
                                                                                                
                                                              
  
                                                            
                                                               
                                                      
  
                                                       
 */
static u64 kbase_pm_get_state(struct kbase_device *kbdev, enum kbase_pm_core_type core_type, enum kbasep_pm_action action)
{
	u32 reg;
	u32 lo, hi;

	reg = core_type_to_reg(core_type, action);

	KBASE_DEBUG_ASSERT(reg);

	lo = kbase_reg_read(kbdev, GPU_CONTROL_REG(reg), NULL);
	hi = kbase_reg_read(kbdev, GPU_CONTROL_REG(reg + 4), NULL);

	return (((u64) hi) << 32) | ((u64) lo);
}

void kbasep_pm_read_present_cores(struct kbase_device *kbdev)
{
	kbdev->shader_present_bitmap = kbase_pm_get_state(kbdev, KBASE_PM_CORE_SHADER, ACTION_PRESENT);
	kbdev->tiler_present_bitmap = kbase_pm_get_state(kbdev, KBASE_PM_CORE_TILER, ACTION_PRESENT);
	kbdev->l2_present_bitmap = kbase_pm_get_state(kbdev, KBASE_PM_CORE_L2, ACTION_PRESENT);
	kbdev->l3_present_bitmap = kbase_pm_get_state(kbdev, KBASE_PM_CORE_L3, ACTION_PRESENT);

	kbdev->shader_inuse_bitmap = 0;
	kbdev->shader_needed_bitmap = 0;
	kbdev->shader_available_bitmap = 0;
	kbdev->tiler_available_bitmap = 0;
	kbdev->l2_users_count = 0;
	kbdev->l2_available_bitmap = 0;
	kbdev->tiler_needed_cnt = 0;
	kbdev->tiler_inuse_cnt = 0;

	memset(kbdev->shader_needed_cnt, 0, sizeof(kbdev->shader_needed_cnt));
}

KBASE_EXPORT_TEST_API(kbasep_pm_read_present_cores)

/*                                
 */
u64 kbase_pm_get_present_cores(struct kbase_device *kbdev, enum kbase_pm_core_type type)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);

	switch (type) {
	case KBASE_PM_CORE_L3:
		return kbdev->l3_present_bitmap;
		break;
	case KBASE_PM_CORE_L2:
		return kbdev->l2_present_bitmap;
		break;
	case KBASE_PM_CORE_SHADER:
		return kbdev->shader_present_bitmap;
		break;
	case KBASE_PM_CORE_TILER:
		return kbdev->tiler_present_bitmap;
		break;
	}
	KBASE_DEBUG_ASSERT(0);
	return 0;
}

KBASE_EXPORT_TEST_API(kbase_pm_get_present_cores)

/*                                                        
 */
u64 kbase_pm_get_active_cores(struct kbase_device *kbdev, enum kbase_pm_core_type type)
{
	return kbase_pm_get_state(kbdev, type, ACTION_PWRACTIVE);
}

KBASE_EXPORT_TEST_API(kbase_pm_get_active_cores)

/*                                                           
 */
u64 kbase_pm_get_trans_cores(struct kbase_device *kbdev, enum kbase_pm_core_type type)
{
	return kbase_pm_get_state(kbdev, type, ACTION_PWRTRANS);
}

KBASE_EXPORT_TEST_API(kbase_pm_get_trans_cores)
/*                                   
 */
u64 kbase_pm_get_ready_cores(struct kbase_device *kbdev, enum kbase_pm_core_type type)
{
	u64 result;

	result = kbase_pm_get_state(kbdev, type, ACTION_READY);

	switch (type) {
	case KBASE_PM_CORE_SHADER:
		KBASE_TRACE_ADD(kbdev, PM_CORES_POWERED, NULL, NULL, 0u, (u32) result);
		break;
	case KBASE_PM_CORE_TILER:
		KBASE_TRACE_ADD(kbdev, PM_CORES_POWERED_TILER, NULL, NULL, 0u, (u32) result);
		break;
	case KBASE_PM_CORE_L2:
		KBASE_TRACE_ADD(kbdev, PM_CORES_POWERED_L2, NULL, NULL, 0u, (u32) result);
		break;
	default:
		/*                             */
		break;
	}

	return result;
}

KBASE_EXPORT_TEST_API(kbase_pm_get_ready_cores)

/*                                                       
  
                                                                                                                     
                                                                                                                    
                                                                                                                
                                                                                 
  
                                            
                                                                    
                                                                        
                                                                                    
                                                                                         
                                                              
                                                                                                              
                                                              
                                                                                                       
  
                                                                                
 */
STATIC mali_bool kbase_pm_transition_core_type(struct kbase_device *kbdev, enum kbase_pm_core_type type, u64 desired_state,
					       u64 in_use, u64 * const available, u64 *powering_on)
{
	u64 present;
	u64 ready;
	u64 trans;
	u64 powerup;
	u64 powerdown;
	u64 powering_on_trans;
	u64 desired_state_in_use;

	lockdep_assert_held(&kbdev->pm.power_change_lock);

	/*                   */
	present = kbase_pm_get_present_cores(kbdev, type);
	trans = kbase_pm_get_trans_cores(kbdev, type);
	ready = kbase_pm_get_ready_cores(kbdev, type);
	/*                                                                                   */
	trans &= ~ready;

	powering_on_trans = trans & *powering_on;
	*powering_on = powering_on_trans;

	if (available != NULL)
		*available = (ready | powering_on_trans) & desired_state;

	/*                                                                                                          
                                          
  */
	desired_state_in_use = desired_state | in_use;

	/*                                               */
	if (type == KBASE_PM_CORE_L2) {
		if ((ready == present) && (desired_state_in_use == ready) && (trans == 0)) {
			/*                                                                    */
			kbdev->pm.l2_powered = 1;
			if (kbdev->l2_users_count > 0) {
				/*                                                                            */
				wake_up(&kbdev->pm.l2_powered_wait);
			}
		} else {
			kbdev->pm.l2_powered = 0;
		}
	}

	if (desired_state_in_use == ready && (trans == 0))
		return MALI_TRUE;

	/*                                                       */
	powerup = desired_state_in_use & present;
	powerdown = (~desired_state_in_use) & present;

	/*                                                             */
	powerup &= ~ready;
	powerdown &= ready;

	/*                                                                      
                                               
   
                                                                       
                                                                     
                                                                         
                                                                     
                                                               
  */
	powerdown &= ~trans;

	if (kbase_hw_has_feature(kbdev, BASE_HW_FEATURE_PWRON_DURING_PWROFF_TRANS))
		if (KBASE_PM_CORE_SHADER == type || KBASE_PM_CORE_TILER == type)
			trans = powering_on_trans; /*                                                                  */

	powerup &= ~trans;

	/*                            */
	kbase_pm_invoke(kbdev, type, powerup, ACTION_PWRON);
	kbase_pm_invoke(kbdev, type, powerdown, ACTION_PWROFF);

	/*                                                               */
	powering_on_trans |= powerup;
	*powering_on = powering_on_trans;
	if (available != NULL)
		*available = (ready | powering_on_trans) & desired_state;

	return MALI_FALSE;
}

KBASE_EXPORT_TEST_API(kbase_pm_transition_core_type)

/*                                                                  
  
                                                                                                                      
                                                                                                                    
  
                                                      
                                                                                         
  
                                                            
 */
STATIC u64 get_desired_cache_status(u64 present, u64 cores_powered)
{
	u64 desired = 0;

	while (present) {
		/*                                       */
		u64 bit = fls64(present) - 1;
		u64 bit_mask = 1ull << bit;
		/*                                                         */

		u64 mask = ~(bit_mask - 1);

		/*                                                                                                          
                  */
		if (cores_powered & mask)
			desired |= bit_mask;

		/*                                            */
		cores_powered &= ~mask;
		present &= ~bit_mask;
	}

	return desired;
}

KBASE_EXPORT_TEST_API(get_desired_cache_status)

mali_bool MOCKABLE(kbase_pm_check_transitions_nolock) (struct kbase_device *kbdev)
{
	mali_bool cores_are_available = MALI_FALSE;
	mali_bool in_desired_state = MALI_TRUE;
	u64 desired_l2_state;
	u64 desired_l3_state;
	u64 cores_powered;
	u64 tiler_available_bitmap;
	u64 shader_available_bitmap;
	u64 shader_ready_bitmap;
	u64 shader_transitioning_bitmap;
	u64 l2_available_bitmap;
	u64 prev_l2_available_bitmap;

	KBASE_DEBUG_ASSERT(NULL != kbdev);
	lockdep_assert_held(&kbdev->pm.power_change_lock);

	spin_lock(&kbdev->pm.gpu_powered_lock);
	if (kbdev->pm.gpu_powered == MALI_FALSE) {
		spin_unlock(&kbdev->pm.gpu_powered_lock);
		if (kbdev->pm.desired_shader_state == 0 && kbdev->pm.desired_tiler_state == 0)
			return MALI_TRUE;
		return MALI_FALSE;
	}

	/*                                                               
                                                                           
                                                                     
                              */
	kbase_timeline_pm_send_event(kbdev, KBASE_TIMELINE_PM_EVENT_CHANGE_GPU_STATE);
	kbase_timeline_pm_handle_event(kbdev, KBASE_TIMELINE_PM_EVENT_CHANGE_GPU_STATE);

	/*                                                                   */
	cores_powered = kbase_pm_get_ready_cores(kbdev, KBASE_PM_CORE_SHADER);

	cores_powered |= kbdev->pm.desired_shader_state;

	/*                                                                                               */
	if (kbdev->l2_users_count > 0)
		cores_powered |= kbdev->l2_present_bitmap;

	desired_l2_state = get_desired_cache_status(kbdev->l2_present_bitmap, cores_powered);

	/*                                                                  */
	if (0 != desired_l2_state) {
		desired_l2_state |= 1;
		/*                                          */
		kbdev->pm.desired_tiler_state = kbdev->tiler_present_bitmap;
	} else {
		kbdev->pm.desired_tiler_state = 0;
	}

	desired_l3_state = get_desired_cache_status(kbdev->l3_present_bitmap, desired_l2_state);
	prev_l2_available_bitmap = l2_available_bitmap;
	in_desired_state &= kbase_pm_transition_core_type(kbdev, KBASE_PM_CORE_L3, desired_l3_state, 0, NULL, &kbdev->pm.powering_on_l3_state);
	in_desired_state &= kbase_pm_transition_core_type(kbdev, KBASE_PM_CORE_L2, desired_l2_state, 0, &l2_available_bitmap, &kbdev->pm.powering_on_l2_state);

	if (kbdev->l2_available_bitmap != l2_available_bitmap) {
		KBASE_TIMELINE_POWER_L2(kbdev, l2_available_bitmap);
	}

	kbdev->l2_available_bitmap = l2_available_bitmap;

	if (in_desired_state) {
		in_desired_state &= kbase_pm_transition_core_type(kbdev, KBASE_PM_CORE_TILER, kbdev->pm.desired_tiler_state, 0, &tiler_available_bitmap, &kbdev->pm.powering_on_tiler_state);
		in_desired_state &= kbase_pm_transition_core_type(kbdev, KBASE_PM_CORE_SHADER, kbdev->pm.desired_shader_state, kbdev->shader_inuse_bitmap, &shader_available_bitmap, &kbdev->pm.powering_on_shader_state);

		if (kbdev->shader_available_bitmap != shader_available_bitmap) {
			KBASE_TRACE_ADD(kbdev, PM_CORES_CHANGE_AVAILABLE, NULL, NULL, 0u, (u32) shader_available_bitmap);
			KBASE_TIMELINE_POWER_SHADER(kbdev, shader_available_bitmap);
		}

		kbdev->shader_available_bitmap = shader_available_bitmap;

		if (kbdev->tiler_available_bitmap != tiler_available_bitmap) {
			KBASE_TRACE_ADD(kbdev, PM_CORES_CHANGE_AVAILABLE_TILER, NULL, NULL, 0u, (u32) tiler_available_bitmap);
			KBASE_TIMELINE_POWER_TILER(kbdev, tiler_available_bitmap);
		}

		kbdev->tiler_available_bitmap = tiler_available_bitmap;

	} else if ((l2_available_bitmap & kbdev->tiler_present_bitmap) != kbdev->tiler_present_bitmap) {
		tiler_available_bitmap = 0;

		if (kbdev->tiler_available_bitmap != tiler_available_bitmap) {
			KBASE_TIMELINE_POWER_TILER(kbdev, tiler_available_bitmap);
		}

		kbdev->tiler_available_bitmap = tiler_available_bitmap;
	}

	/*                                     */
	kbdev->pm.gpu_in_desired_state = in_desired_state;

	shader_ready_bitmap = kbase_pm_get_ready_cores(kbdev, KBASE_PM_CORE_SHADER);
	shader_transitioning_bitmap = kbase_pm_get_trans_cores(kbdev, KBASE_PM_CORE_SHADER);

	/*                                                                  
                                                                            
                                                    */
	if ((kbdev->shader_available_bitmap & kbdev->pm.desired_shader_state) == kbdev->pm.desired_shader_state &&
			(kbdev->tiler_available_bitmap & kbdev->pm.desired_tiler_state) == kbdev->pm.desired_tiler_state) {
		cores_are_available = MALI_TRUE;

		KBASE_TRACE_ADD(kbdev, PM_CORES_AVAILABLE, NULL, NULL, 0u, (u32)(kbdev->shader_available_bitmap & kbdev->pm.desired_shader_state));
		KBASE_TRACE_ADD(kbdev, PM_CORES_AVAILABLE_TILER, NULL, NULL, 0u, (u32)(kbdev->tiler_available_bitmap & kbdev->pm.desired_tiler_state));

		/*                                                               
                                                                       
                                             */
		if (!in_desired_state)
			kbase_timeline_pm_send_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_STATE_CHANGED);
	}

	if (in_desired_state) {
		KBASE_DEBUG_ASSERT(cores_are_available);

#ifdef CONFIG_MALI_GATOR_SUPPORT
		kbase_trace_mali_pm_status(KBASE_PM_CORE_L3, kbase_pm_get_ready_cores(kbdev, KBASE_PM_CORE_L3));
		kbase_trace_mali_pm_status(KBASE_PM_CORE_L2, kbase_pm_get_ready_cores(kbdev, KBASE_PM_CORE_L2));
		kbase_trace_mali_pm_status(KBASE_PM_CORE_SHADER, kbase_pm_get_ready_cores(kbdev, KBASE_PM_CORE_SHADER));
		kbase_trace_mali_pm_status(KBASE_PM_CORE_TILER, kbase_pm_get_ready_cores(kbdev, KBASE_PM_CORE_TILER));
#endif				/*                           */

		KBASE_TRACE_ADD(kbdev, PM_DESIRED_REACHED, NULL, NULL, kbdev->pm.gpu_in_desired_state, (u32)kbdev->pm.desired_shader_state);
		KBASE_TRACE_ADD(kbdev, PM_DESIRED_REACHED_TILER, NULL, NULL, 0u, (u32)kbdev->pm.desired_tiler_state);

		/*                                                    */
		kbase_timeline_pm_send_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_STATE_CHANGED);
		/*                                                          */
		KBASE_TRACE_ADD(kbdev, PM_WAKE_WAITERS, NULL, NULL, 0u, 0);
		wake_up(&kbdev->pm.gpu_in_desired_state_wait);
	}

	spin_unlock(&kbdev->pm.gpu_powered_lock);

	/*                                                                  
                                                                      
                                                                
             */
	if (kbdev->shader_ready_bitmap != shader_ready_bitmap ||
	    kbdev->shader_transitioning_bitmap != shader_transitioning_bitmap) {
		kbdev->shader_ready_bitmap = shader_ready_bitmap;
		kbdev->shader_transitioning_bitmap = shader_transitioning_bitmap;

		kbase_pm_ca_update_core_status(kbdev, shader_ready_bitmap, shader_transitioning_bitmap);
	}

	/*                                                                                                                         */
	if (!((shader_ready_bitmap | shader_transitioning_bitmap) & kbdev->gpu_props.props.coherency_info.group[0].core_mask) &&
			(prev_l2_available_bitmap == desired_l2_state) &&
			!(kbase_pm_ca_get_core_mask(kbdev) & kbdev->gpu_props.props.coherency_info.group[0].core_mask))
		BUG();

	/*                                                                  
                                                      */
	if (!((shader_ready_bitmap | shader_transitioning_bitmap) & kbdev->gpu_props.props.coherency_info.group[1].core_mask) &&
		!(kbase_pm_ca_get_core_mask(kbdev) & kbdev->gpu_props.props.coherency_info.group[1].core_mask))
		kbdev->pm.cg1_disabled = MALI_TRUE;
	else
		kbdev->pm.cg1_disabled = MALI_FALSE;

	return cores_are_available;
}
KBASE_EXPORT_TEST_API(kbase_pm_check_transitions_nolock)

void kbase_pm_check_transitions_sync(struct kbase_device *kbdev)
{
	unsigned long flags;
	mali_bool cores_are_available;
	/*                                                                   
                                                               */
	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);
	cores_are_available = kbase_pm_check_transitions_nolock(kbdev);
	/*                                                                    */
	CSTD_UNUSED(cores_are_available);
	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	/*                */
	wait_event(kbdev->pm.gpu_in_desired_state_wait, kbdev->pm.gpu_in_desired_state);

	/*                                                                 */
	kbase_timeline_pm_handle_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_STATE_CHANGED);
}
KBASE_EXPORT_TEST_API(kbase_pm_check_transitions_sync)

void kbase_pm_enable_interrupts(struct kbase_device *kbdev)
{
	unsigned long flags;

	KBASE_DEBUG_ASSERT(NULL != kbdev);
	/*
                         
                        
  */
	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_CLEAR), GPU_IRQ_REG_ALL, NULL);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), GPU_IRQ_REG_ALL, NULL);
	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	kbase_reg_write(kbdev, JOB_CONTROL_REG(JOB_IRQ_CLEAR), 0xFFFFFFFF, NULL);
	kbase_reg_write(kbdev, JOB_CONTROL_REG(JOB_IRQ_MASK), 0xFFFFFFFF, NULL);

	kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_CLEAR), 0xFFFFFFFF, NULL);
	kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_MASK), 0xFFFFFFFF, NULL);
}

KBASE_EXPORT_TEST_API(kbase_pm_enable_interrupts)

void kbase_pm_disable_interrupts(struct kbase_device *kbdev)
{
	unsigned long flags;

	KBASE_DEBUG_ASSERT(NULL != kbdev);
	/*
                        
                       
  */
	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), 0, NULL);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_CLEAR), GPU_IRQ_REG_ALL, NULL);
	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	kbase_reg_write(kbdev, JOB_CONTROL_REG(JOB_IRQ_MASK), 0, NULL);
	kbase_reg_write(kbdev, JOB_CONTROL_REG(JOB_IRQ_CLEAR), 0xFFFFFFFF, NULL);

	kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_MASK), 0, NULL);
	kbase_reg_write(kbdev, MMU_REG(MMU_IRQ_CLEAR), 0xFFFFFFFF, NULL);
}

KBASE_EXPORT_TEST_API(kbase_pm_disable_interrupts)

/*
              
                                    
                                           
                                                      
 */
void kbase_pm_clock_on(struct kbase_device *kbdev, mali_bool is_resume)
{
	mali_bool reset_required = is_resume;
	unsigned long flags;

	KBASE_DEBUG_ASSERT(NULL != kbdev);
	lockdep_assert_held(&kbdev->pm.lock);

	if (kbdev->pm.gpu_powered) {
		/*                   */
		KBASE_DEBUG_ASSERT(!is_resume);
		return;
	}

	KBASE_TRACE_ADD(kbdev, PM_GPU_ON, NULL, NULL, 0u, 0u);

	if (is_resume && kbdev->pm.callback_power_resume) {
		kbdev->pm.callback_power_resume(kbdev);
	} else if (kbdev->pm.callback_power_on) {
		kbdev->pm.callback_power_on(kbdev);
		/*                                                                     
                                                                       
                                                                        
          */
		reset_required = MALI_TRUE;
	}

	spin_lock_irqsave(&kbdev->pm.gpu_powered_lock, flags);
	kbdev->pm.gpu_powered = MALI_TRUE;
	spin_unlock_irqrestore(&kbdev->pm.gpu_powered_lock, flags);

	if (reset_required) {
		/*                                                   
                      */
		kbase_pm_init_hw(kbdev, MALI_TRUE);
	}

	/*                               */
	kbase_pm_enable_interrupts(kbdev);
}

KBASE_EXPORT_TEST_API(kbase_pm_clock_on)

void kbase_pm_clock_off(struct kbase_device *kbdev, mali_bool is_suspend)
{
	unsigned long flags;

	KBASE_DEBUG_ASSERT(NULL != kbdev);
	lockdep_assert_held(&kbdev->pm.lock);

	/*                                                                  */
	KBASE_DEBUG_ASSERT(kbdev->shader_available_bitmap == 0u);

	if (!kbdev->pm.gpu_powered) {
		/*                    */
		if (is_suspend && kbdev->pm.callback_power_suspend)
			kbdev->pm.callback_power_suspend(kbdev);
		return;
	}

	KBASE_TRACE_ADD(kbdev, PM_GPU_OFF, NULL, NULL, 0u, 0u);

	/*                                                                 */
	kbase_pm_disable_interrupts(kbdev);
	/*                                            */
	kbase_synchronize_irqs(kbdev);

	/*                                                 */
	spin_lock_irqsave(&kbdev->pm.gpu_powered_lock, flags);
	kbdev->pm.gpu_powered = MALI_FALSE;
	spin_unlock_irqrestore(&kbdev->pm.gpu_powered_lock, flags);

	if (is_suspend && kbdev->pm.callback_power_suspend)
		kbdev->pm.callback_power_suspend(kbdev);
	else if (kbdev->pm.callback_power_off)
		kbdev->pm.callback_power_off(kbdev);
}

KBASE_EXPORT_TEST_API(kbase_pm_clock_off)

struct kbasep_reset_timeout_data {
	struct hrtimer timer;
	mali_bool timed_out;
	struct kbase_device *kbdev;
};

void kbase_pm_reset_done(struct kbase_device *kbdev)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);
	kbdev->pm.reset_done = MALI_TRUE;
	wake_up(&kbdev->pm.reset_done_wait);
}

/* 
                                                                           
 */
STATIC void kbase_pm_wait_for_reset(struct kbase_device *kbdev)
{
	lockdep_assert_held(&kbdev->pm.lock);

	wait_event(kbdev->pm.reset_done_wait, (kbdev->pm.reset_done));
	kbdev->pm.reset_done = MALI_FALSE;
}

KBASE_EXPORT_TEST_API(kbase_pm_reset_done)

static enum hrtimer_restart kbasep_reset_timeout(struct hrtimer *timer)
{
	struct kbasep_reset_timeout_data *rtdata = container_of(timer, struct kbasep_reset_timeout_data, timer);

	rtdata->timed_out = 1;

	/*                                                                                       */
	kbase_pm_reset_done(rtdata->kbdev);

	return HRTIMER_NORESTART;
}

static void kbase_pm_hw_issues(struct kbase_device *kbdev)
{
	u32 value = 0;

	/*                                                                    
       
                                                */
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8443) ||
			kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_11035))
		value |= SC_LS_PAUSEBUFFER_DISABLE;

	/*                                                                      */
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_10327))
		value |= SC_SDC_DISABLE_OQ_DISCARD;

	/*                                                              */
	if (DEFAULT_ALTERNATIVE_HWC)
		value |= SC_ALT_COUNTERS;

	/*                                                                         */
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_T76X_2121))
		value |= SC_OVERRIDE_FWD_PIXEL_KILL;

	/*                                                                   */
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_10797))
		value |= SC_ENABLE_TEXGRD_FLAGS;

	if (value != 0)
		kbase_reg_write(kbdev, GPU_CONTROL_REG(SHADER_CONFIG), value, NULL);

	/*                                           */
	if (kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_T76X_3953)) {
		value = kbase_reg_read(kbdev, GPU_CONTROL_REG(TILER_CONFIG), NULL);
		value |= TC_CLOCK_GATE_OVERRIDE;
		kbase_reg_write(kbdev, GPU_CONTROL_REG(TILER_CONFIG), value, NULL);
	}

	/*                                                         */
	value = kbase_reg_read(kbdev, GPU_CONTROL_REG(L2_MMU_CONFIG), NULL);

	/*                             */
	value &= ~(L2_MMU_CONFIG_LIMIT_EXTERNAL_READS);
	value |= (DEFAULT_ARID_LIMIT & 0x3) << L2_MMU_CONFIG_LIMIT_EXTERNAL_READS_SHIFT;

	/*                              */
	value &= ~(L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES);
	value |= (DEFAULT_AWID_LIMIT & 0x3) << L2_MMU_CONFIG_LIMIT_EXTERNAL_WRITES_SHIFT;

	kbase_reg_write(kbdev, GPU_CONTROL_REG(L2_MMU_CONFIG), value, NULL);
}

mali_error kbase_pm_init_hw(struct kbase_device *kbdev, mali_bool enable_irqs)
{
	unsigned long flags;
	struct kbasep_reset_timeout_data rtdata;

	KBASE_DEBUG_ASSERT(NULL != kbdev);
	lockdep_assert_held(&kbdev->pm.lock);

	/*                                                                 */
	if (!kbdev->pm.gpu_powered) {
		if (kbdev->pm.callback_power_on)
			kbdev->pm.callback_power_on(kbdev);

		spin_lock_irqsave(&kbdev->pm.gpu_powered_lock, flags);
		kbdev->pm.gpu_powered = MALI_TRUE;
		spin_unlock_irqrestore(&kbdev->pm.gpu_powered_lock, flags);
	}

	/*                                                                                      */
	kbase_pm_disable_interrupts(kbdev);

	/*                            */
	kbdev->pm.reset_done = MALI_FALSE;

	/*                                                       */
	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);
	if (kbdev->shader_available_bitmap != 0u)
			KBASE_TRACE_ADD(kbdev, PM_CORES_CHANGE_AVAILABLE, NULL, NULL, 0u, (u32)0u);
	if (kbdev->tiler_available_bitmap != 0u)
			KBASE_TRACE_ADD(kbdev, PM_CORES_CHANGE_AVAILABLE_TILER, NULL, NULL, 0u, (u32)0u);
	kbdev->shader_available_bitmap = 0u;
	kbdev->tiler_available_bitmap = 0u;
	kbdev->l2_available_bitmap = 0u;
	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	/*                    */
	KBASE_TRACE_ADD(kbdev, CORE_GPU_SOFT_RESET, NULL, NULL, 0u, 0);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_SOFT_RESET, NULL);

	/*                                          */
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_IRQ_MASK), RESET_COMPLETED, NULL);

	/*                                                             */
	rtdata.kbdev = kbdev;
	rtdata.timed_out = 0;

	/*                                                 */
	hrtimer_init_on_stack(&rtdata.timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	rtdata.timer.function = kbasep_reset_timeout;

	hrtimer_start(&rtdata.timer, HR_TIMER_DELAY_MSEC(RESET_TIMEOUT), HRTIMER_MODE_REL);

	/*                                                     */
	kbase_pm_wait_for_reset(kbdev);

	if (rtdata.timed_out == 0) {
		/*                    */
		hrtimer_cancel(&rtdata.timer);
		destroy_hrtimer_on_stack(&rtdata.timer);
		goto out;
	}

	/*                                                                                             */
	if (kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_IRQ_RAWSTAT), NULL) & RESET_COMPLETED) {
		/*                                                                                                   */
		dev_warn(kbdev->dev, "Reset interrupt didn't reach CPU. Check interrupt assignments.\n");
		/*                                                 */
		destroy_hrtimer_on_stack(&rtdata.timer);
		goto out;
	}

	/*                                                                        */
	dev_err(kbdev->dev, "Failed to soft-reset GPU (timed out after %d ms), now attempting a hard reset\n", RESET_TIMEOUT);
	KBASE_TRACE_ADD(kbdev, CORE_GPU_HARD_RESET, NULL, NULL, 0u, 0);
	kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_HARD_RESET, NULL);

	/*                                                          */
	rtdata.timed_out = 0;

	hrtimer_start(&rtdata.timer, HR_TIMER_DELAY_MSEC(RESET_TIMEOUT), HRTIMER_MODE_REL);

	/*                                                     */
	kbase_pm_wait_for_reset(kbdev);

	if (rtdata.timed_out == 0) {
		/*                    */
		hrtimer_cancel(&rtdata.timer);
		destroy_hrtimer_on_stack(&rtdata.timer);
		goto out;
	}

	destroy_hrtimer_on_stack(&rtdata.timer);

	dev_err(kbdev->dev, "Failed to hard-reset the GPU (timed out after %d ms)\n", RESET_TIMEOUT);

	/*                                     */
	return MALI_ERROR_FUNCTION_FAILED;

 out:

	/*                                                                                                           */
	if (kbdev->pm.gpu_cycle_counter_requests && enable_irqs) {
		/*                                                       */
		kbase_pm_enable_interrupts(kbdev);
		kbase_pm_request_l2_caches(kbdev);

		/*                                      */
		spin_lock_irqsave(&kbdev->pm.gpu_cycle_counter_requests_lock, flags);
		if (kbdev->pm.gpu_cycle_counter_requests)
			kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_CYCLE_COUNT_START, NULL);
		spin_unlock_irqrestore(&kbdev->pm.gpu_cycle_counter_requests_lock, flags);

		kbase_pm_release_l2_caches(kbdev);
		kbase_pm_disable_interrupts(kbdev);
	}

	if (enable_irqs)
		kbase_pm_enable_interrupts(kbdev);

	kbase_pm_hw_issues(kbdev);

	return MALI_ERROR_NONE;
}

KBASE_EXPORT_TEST_API(kbase_pm_init_hw)

/*                                                                                                      
  
                                                                                       
                                                        
  
                                                                                         
                                                                                                                
  
                                                            
  
 */
static void kbase_pm_request_gpu_cycle_counter_do_request(struct kbase_device *kbdev)
{
	unsigned long flags;

	spin_lock_irqsave(&kbdev->pm.gpu_cycle_counter_requests_lock, flags);

	++kbdev->pm.gpu_cycle_counter_requests;

	if (1 == kbdev->pm.gpu_cycle_counter_requests)
		kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_CYCLE_COUNT_START, NULL);

	spin_unlock_irqrestore(&kbdev->pm.gpu_cycle_counter_requests_lock, flags);
}

void kbase_pm_request_gpu_cycle_counter(struct kbase_device *kbdev)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);

	KBASE_DEBUG_ASSERT(kbdev->pm.gpu_powered);

	KBASE_DEBUG_ASSERT(kbdev->pm.gpu_cycle_counter_requests < INT_MAX);

	kbase_pm_request_l2_caches(kbdev);

	kbase_pm_request_gpu_cycle_counter_do_request(kbdev);
}

KBASE_EXPORT_TEST_API(kbase_pm_request_gpu_cycle_counter)

void kbase_pm_request_gpu_cycle_counter_l2_is_on(struct kbase_device *kbdev)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);

	KBASE_DEBUG_ASSERT(kbdev->pm.gpu_powered);

	KBASE_DEBUG_ASSERT(kbdev->pm.gpu_cycle_counter_requests < INT_MAX);

	kbase_pm_request_l2_caches_l2_is_on(kbdev);

	kbase_pm_request_gpu_cycle_counter_do_request(kbdev);
}

KBASE_EXPORT_TEST_API(kbase_pm_request_gpu_cycle_counter_l2_is_on)

void kbase_pm_release_gpu_cycle_counter(struct kbase_device *kbdev)
{
	unsigned long flags;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	spin_lock_irqsave(&kbdev->pm.gpu_cycle_counter_requests_lock, flags);

	KBASE_DEBUG_ASSERT(kbdev->pm.gpu_cycle_counter_requests > 0);

	--kbdev->pm.gpu_cycle_counter_requests;

	if (0 == kbdev->pm.gpu_cycle_counter_requests)
		kbase_reg_write(kbdev, GPU_CONTROL_REG(GPU_COMMAND), GPU_COMMAND_CYCLE_COUNT_STOP, NULL);

	spin_unlock_irqrestore(&kbdev->pm.gpu_cycle_counter_requests_lock, flags);

	kbase_pm_release_l2_caches(kbdev);
}

KBASE_EXPORT_TEST_API(kbase_pm_release_gpu_cycle_counter)
#endif /*             */
