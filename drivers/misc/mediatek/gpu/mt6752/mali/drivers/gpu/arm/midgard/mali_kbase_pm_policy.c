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
#include <mali_kbase_gator.h>
#include <mali_kbase_pm.h>

#if KBASE_PM_EN

extern const struct kbase_pm_policy kbase_pm_always_on_policy_ops;
extern const struct kbase_pm_policy kbase_pm_coarse_demand_policy_ops;
extern const struct kbase_pm_policy kbase_pm_demand_policy_ops;

#if !MALI_CUSTOMER_RELEASE
extern const struct kbase_pm_policy kbase_pm_fast_start_policy_ops;
extern const struct kbase_pm_policy kbase_pm_demand_always_powered_policy_ops;
#endif

static const struct kbase_pm_policy *const policy_list[] = {
#ifdef CONFIG_MALI_NO_MALI
	&kbase_pm_always_on_policy_ops,
	&kbase_pm_demand_policy_ops,
	&kbase_pm_coarse_demand_policy_ops,
#if !MALI_CUSTOMER_RELEASE
	&kbase_pm_demand_always_powered_policy_ops,
	&kbase_pm_fast_start_policy_ops,
#endif
#else				/*                     */
	&kbase_pm_demand_policy_ops,
	&kbase_pm_always_on_policy_ops,
	&kbase_pm_coarse_demand_policy_ops,
#if !MALI_CUSTOMER_RELEASE
	&kbase_pm_demand_always_powered_policy_ops,
	&kbase_pm_fast_start_policy_ops,
#endif
#endif				/*                     */
};

/*                                                 
                                                                               
 */
#define POLICY_COUNT (sizeof(policy_list)/sizeof(*policy_list))


/*                                                                                      */
enum kbase_pm_func_id {
	KBASE_PM_FUNC_ID_REQUEST_CORES_START,
	KBASE_PM_FUNC_ID_REQUEST_CORES_END,
	KBASE_PM_FUNC_ID_RELEASE_CORES_START,
	KBASE_PM_FUNC_ID_RELEASE_CORES_END,
	/*                                                                     
                                                                       
                                                                
                                                   */

	/*                  */
	KBASE_PM_FUNC_ID_COUNT
};


/*                                                          */
enum {
	KBASE_PM_CHANGE_STATE_SHADER = (1u << 0),
	KBASE_PM_CHANGE_STATE_TILER  = (1u << 1),

	/*                        */
	KBASE_PM_CHANGE_STATE_MASK = (KBASE_PM_CHANGE_STATE_TILER|KBASE_PM_CHANGE_STATE_SHADER),
	KBASE_PM_CHANGE_STATE_COUNT = KBASE_PM_CHANGE_STATE_MASK + 1
};
typedef u32 kbase_pm_change_state;


#ifdef CONFIG_MALI_TRACE_TIMELINE
/*                                               */
static u32 kbase_pm_change_state_trace_code[KBASE_PM_FUNC_ID_COUNT][KBASE_PM_CHANGE_STATE_COUNT] = {
	/*                        */
	[KBASE_PM_FUNC_ID_REQUEST_CORES_START][0] = 0,
	[KBASE_PM_FUNC_ID_REQUEST_CORES_START][KBASE_PM_CHANGE_STATE_SHADER] =
		SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_SHADER_START,
	[KBASE_PM_FUNC_ID_REQUEST_CORES_START][KBASE_PM_CHANGE_STATE_TILER] =
		SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_TILER_START,
	[KBASE_PM_FUNC_ID_REQUEST_CORES_START][KBASE_PM_CHANGE_STATE_SHADER|KBASE_PM_CHANGE_STATE_TILER] =
		SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_SHADER_TILER_START,

	[KBASE_PM_FUNC_ID_REQUEST_CORES_END][0] = 0,
	[KBASE_PM_FUNC_ID_REQUEST_CORES_END][KBASE_PM_CHANGE_STATE_SHADER] =
		SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_SHADER_END,
	[KBASE_PM_FUNC_ID_REQUEST_CORES_END][KBASE_PM_CHANGE_STATE_TILER] =
		SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_TILER_END,
	[KBASE_PM_FUNC_ID_REQUEST_CORES_END][KBASE_PM_CHANGE_STATE_SHADER|KBASE_PM_CHANGE_STATE_TILER] =
		SW_FLOW_PM_CHECKTRANS_PM_REQUEST_CORES_SHADER_TILER_END,

	/*                        */
	[KBASE_PM_FUNC_ID_RELEASE_CORES_START][0] = 0,
	[KBASE_PM_FUNC_ID_RELEASE_CORES_START][KBASE_PM_CHANGE_STATE_SHADER] =
		SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_SHADER_START,
	[KBASE_PM_FUNC_ID_RELEASE_CORES_START][KBASE_PM_CHANGE_STATE_TILER] =
		SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_TILER_START,
	[KBASE_PM_FUNC_ID_RELEASE_CORES_START][KBASE_PM_CHANGE_STATE_SHADER|KBASE_PM_CHANGE_STATE_TILER] =
		SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_SHADER_TILER_START,

	[KBASE_PM_FUNC_ID_RELEASE_CORES_END][0] = 0,
	[KBASE_PM_FUNC_ID_RELEASE_CORES_END][KBASE_PM_CHANGE_STATE_SHADER] =
		SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_SHADER_END,
	[KBASE_PM_FUNC_ID_RELEASE_CORES_END][KBASE_PM_CHANGE_STATE_TILER] =
		SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_TILER_END,
	[KBASE_PM_FUNC_ID_RELEASE_CORES_END][KBASE_PM_CHANGE_STATE_SHADER|KBASE_PM_CHANGE_STATE_TILER] =
		SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_SHADER_TILER_END
};

STATIC INLINE void kbase_timeline_pm_cores_func(struct kbase_device *kbdev,
		enum kbase_pm_func_id func_id,
		kbase_pm_change_state state)
{
	int trace_code;

	KBASE_DEBUG_ASSERT(func_id >= 0 && func_id < KBASE_PM_FUNC_ID_COUNT);
	KBASE_DEBUG_ASSERT(state != 0 && (state & KBASE_PM_CHANGE_STATE_MASK) == state);

	trace_code = kbase_pm_change_state_trace_code[func_id][state];
	KBASE_TIMELINE_PM_CHECKTRANS(kbdev, trace_code);
}

#else /*                            */
STATIC INLINE void kbase_timeline_pm_cores_func(struct kbase_device *kbdev,
		enum kbase_pm_func_id func_id, kbase_pm_change_state state)
{
}

#endif /*                            */

static enum hrtimer_restart kbasep_pm_do_gpu_poweroff_callback(struct hrtimer *timer)
{
	struct kbase_device *kbdev;

	kbdev = container_of(timer, struct kbase_device, pm.gpu_poweroff_timer);

	/*                                                                           
                                                                                             
               
   
                                                                          
                                                                        
                
  */
	if (kbdev->pm.gpu_poweroff_pending)
		queue_work(kbdev->pm.gpu_poweroff_wq, &kbdev->pm.gpu_poweroff_work);

	if (kbdev->pm.shader_poweroff_pending) {
		unsigned long flags;

		spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

		if (kbdev->pm.shader_poweroff_pending) {
			kbdev->pm.shader_poweroff_pending_time--;

			KBASE_DEBUG_ASSERT(kbdev->pm.shader_poweroff_pending_time >= 0);

			if (kbdev->pm.shader_poweroff_pending_time == 0) {
				u64 prev_shader_state = kbdev->pm.desired_shader_state;

				kbdev->pm.desired_shader_state &= ~kbdev->pm.shader_poweroff_pending;
				kbdev->pm.shader_poweroff_pending = 0;

				if (prev_shader_state != kbdev->pm.desired_shader_state ||
			    	    kbdev->pm.ca_in_transition != MALI_FALSE) {
					mali_bool cores_are_available;

					KBASE_TIMELINE_PM_CHECKTRANS(kbdev, SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_DEFERRED_START);
					cores_are_available = kbase_pm_check_transitions_nolock(kbdev);
					KBASE_TIMELINE_PM_CHECKTRANS(kbdev, SW_FLOW_PM_CHECKTRANS_PM_RELEASE_CORES_DEFERRED_END);

					/*                                                                    */
					CSTD_UNUSED(cores_are_available);
				}
			}
		}

		spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
	}

	hrtimer_add_expires(timer, kbdev->pm.gpu_poweroff_time);
	return HRTIMER_RESTART;
}

static void kbasep_pm_do_gpu_poweroff_wq(struct work_struct *data)
{
	unsigned long flags;
	struct kbase_device *kbdev;
	mali_bool do_poweroff = MALI_FALSE;

	kbdev = container_of(data, struct kbase_device, pm.gpu_poweroff_work);

	mutex_lock(&kbdev->pm.lock);

	if (kbdev->pm.gpu_poweroff_pending == 0) {
		mutex_unlock(&kbdev->pm.lock);
		return;
	}

	kbdev->pm.gpu_poweroff_pending--;

	if (kbdev->pm.gpu_poweroff_pending > 0) {
		mutex_unlock(&kbdev->pm.lock);
		return;
	}

	KBASE_DEBUG_ASSERT(kbdev->pm.gpu_poweroff_pending == 0);

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	/*                                                      */
	if (kbdev->pm.pm_current_policy->get_core_active(kbdev) == MALI_FALSE)
		do_poweroff = MALI_TRUE;

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	if (do_poweroff != MALI_FALSE) {
		kbdev->pm.poweroff_timer_running = MALI_FALSE;
		/*                   */
		kbase_pm_do_poweroff(kbdev, MALI_FALSE);
		hrtimer_cancel(&kbdev->pm.gpu_poweroff_timer);
	}

	mutex_unlock(&kbdev->pm.lock);
}

mali_error kbase_pm_policy_init(struct kbase_device *kbdev)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);

	kbdev->pm.gpu_poweroff_wq = alloc_workqueue("kbase_pm_do_poweroff", WQ_HIGHPRI | WQ_UNBOUND, 1);
	if (NULL == kbdev->pm.gpu_poweroff_wq)
		return MALI_ERROR_OUT_OF_MEMORY;
	INIT_WORK(&kbdev->pm.gpu_poweroff_work, kbasep_pm_do_gpu_poweroff_wq);

	hrtimer_init(&kbdev->pm.gpu_poweroff_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	kbdev->pm.gpu_poweroff_timer.function = kbasep_pm_do_gpu_poweroff_callback;

	kbdev->pm.pm_current_policy = policy_list[0];

	kbdev->pm.pm_current_policy->init(kbdev);

	kbdev->pm.gpu_poweroff_time = HR_TIMER_DELAY_NSEC(kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_PM_GPU_POWEROFF_TICK_NS));

	kbdev->pm.poweroff_shader_ticks = kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_PM_POWEROFF_TICK_SHADER);
	kbdev->pm.poweroff_gpu_ticks = kbasep_get_config_value(kbdev, kbdev->config_attributes, KBASE_CONFIG_ATTR_PM_POWEROFF_TICK_GPU);

	return MALI_ERROR_NONE;
}

void kbase_pm_policy_term(struct kbase_device *kbdev)
{
	kbdev->pm.pm_current_policy->term(kbdev);
}

void kbase_pm_cancel_deferred_poweroff(struct kbase_device *kbdev)
{
	unsigned long flags;

	lockdep_assert_held(&kbdev->pm.lock);

	hrtimer_cancel(&kbdev->pm.gpu_poweroff_timer);

	/*                                                                                 */
	kbdev->pm.gpu_poweroff_pending = 0;

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	kbdev->pm.shader_poweroff_pending = 0;
	kbdev->pm.shader_poweroff_pending_time = 0;

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
}

void kbase_pm_update_active(struct kbase_device *kbdev)
{
	unsigned long flags;
	mali_bool active;

	lockdep_assert_held(&kbdev->pm.lock);

	/*                                                            */
	KBASE_DEBUG_ASSERT(kbdev->pm.pm_current_policy);

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	active = kbdev->pm.pm_current_policy->get_core_active(kbdev);

	if (active != MALI_FALSE) {
		spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

		if (kbdev->pm.gpu_poweroff_pending) {
			/*                                      */
			kbdev->pm.gpu_poweroff_pending = 0;

			/*                                                                                 */
			return;
		}

		if (!kbdev->pm.poweroff_timer_running && !kbdev->pm.gpu_powered) {
			kbdev->pm.poweroff_timer_running = MALI_TRUE;
			hrtimer_start(&kbdev->pm.gpu_poweroff_timer, kbdev->pm.gpu_poweroff_time, HRTIMER_MODE_REL);
		}

		/*                                                        */
		kbase_pm_do_poweron(kbdev, MALI_FALSE);
	} else {
		/*                                                         
                                    */
		KBASE_DEBUG_ASSERT(kbdev->pm.active_count == 0);

		if (kbdev->pm.shader_poweroff_pending) {
			kbdev->pm.shader_poweroff_pending = 0;
			kbdev->pm.shader_poweroff_pending_time = 0;
		}

		spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);


		/*                   */
		if (kbdev->pm.gpu_powered) {
			kbdev->pm.gpu_poweroff_pending = kbdev->pm.poweroff_gpu_ticks;
			if (!kbdev->pm.poweroff_timer_running) {
				/*                                                                               
                                                                            */
				kbdev->pm.poweroff_timer_running = MALI_TRUE;
				hrtimer_start(&kbdev->pm.gpu_poweroff_timer, kbdev->pm.gpu_poweroff_time, HRTIMER_MODE_REL);
			}
		}
	}
}

void kbase_pm_update_cores_state_nolock(struct kbase_device *kbdev)
{
	u64 desired_bitmap;
	mali_bool cores_are_available;

	lockdep_assert_held(&kbdev->pm.power_change_lock);

	if (kbdev->pm.pm_current_policy == NULL)
		return;

	desired_bitmap = kbdev->pm.pm_current_policy->get_core_mask(kbdev); 
	desired_bitmap &= kbase_pm_ca_get_core_mask(kbdev);

	/*                                                                  */
	if (kbdev->tiler_needed_cnt > 0 || kbdev->tiler_inuse_cnt > 0)
		desired_bitmap |= 1;

	if (kbdev->pm.desired_shader_state != desired_bitmap)
		KBASE_TRACE_ADD(kbdev, PM_CORES_CHANGE_DESIRED, NULL, NULL, 0u, (u32)desired_bitmap);

	/*                                 */
	if (~kbdev->pm.desired_shader_state & desired_bitmap ||
	    kbdev->pm.ca_in_transition != MALI_FALSE) {
		/*                                                                     */
		if (kbdev->pm.desired_shader_state & ~desired_bitmap) {
			/*                                */
			kbdev->pm.shader_poweroff_pending |= (kbdev->pm.desired_shader_state & ~desired_bitmap);
			kbdev->pm.shader_poweroff_pending_time = kbdev->pm.poweroff_shader_ticks;
		}

		kbdev->pm.desired_shader_state = desired_bitmap;

		/*                                                           */
		cores_are_available = kbase_pm_check_transitions_nolock(kbdev);
	} else if (kbdev->pm.desired_shader_state & ~desired_bitmap) {
		/*                                */
		kbdev->pm.shader_poweroff_pending |= (kbdev->pm.desired_shader_state & ~desired_bitmap);
		kbdev->pm.shader_poweroff_pending_time = kbdev->pm.poweroff_shader_ticks;
	} else if (kbdev->pm.active_count == 0 && desired_bitmap != 0 && kbdev->pm.poweroff_timer_running) {
		/*                                                                           
                                                      */
		kbdev->pm.poweroff_timer_running = MALI_FALSE;
		hrtimer_cancel(&kbdev->pm.gpu_poweroff_timer);
	}

	/*                                                                                        */
	if (kbdev->pm.shader_poweroff_pending != 0) {
		kbdev->pm.shader_poweroff_pending &= ~(kbdev->pm.desired_shader_state & desired_bitmap);
		if (kbdev->pm.shader_poweroff_pending == 0)
			kbdev->pm.shader_poweroff_pending_time = 0;
	}

	/*                                                                    */
	CSTD_UNUSED(cores_are_available);
}

void kbase_pm_update_cores_state(struct kbase_device *kbdev)
{
	unsigned long flags;

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	kbase_pm_update_cores_state_nolock(kbdev);

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
}

int kbase_pm_list_policies(const struct kbase_pm_policy * const **list)
{
	if (!list)
		return POLICY_COUNT;

	*list = policy_list;

	return POLICY_COUNT;
}

KBASE_EXPORT_TEST_API(kbase_pm_list_policies)

const struct kbase_pm_policy *kbase_pm_get_policy(struct kbase_device *kbdev)
{
	KBASE_DEBUG_ASSERT(kbdev != NULL);

	return kbdev->pm.pm_current_policy;
}

KBASE_EXPORT_TEST_API(kbase_pm_get_policy)

void kbase_pm_set_policy(struct kbase_device *kbdev, const struct kbase_pm_policy *new_policy)
{
	const struct kbase_pm_policy *old_policy;
	unsigned long flags;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(new_policy != NULL);

	KBASE_TRACE_ADD(kbdev, PM_SET_POLICY, NULL, NULL, 0u, new_policy->id);

	/*                                                     */
	/*                                                                                 */
	kbase_pm_context_active(kbdev);

	mutex_lock(&kbdev->pm.lock);

	/*                                                              */
	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);
	old_policy = kbdev->pm.pm_current_policy;
	kbdev->pm.pm_current_policy = NULL;
	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	KBASE_TRACE_ADD(kbdev, PM_CURRENT_POLICY_TERM, NULL, NULL, 0u, old_policy->id);
	if (old_policy->term)
		old_policy->term(kbdev);

	KBASE_TRACE_ADD(kbdev, PM_CURRENT_POLICY_INIT, NULL, NULL, 0u, new_policy->id);
	if (new_policy->init)
		new_policy->init(kbdev);

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);
	kbdev->pm.pm_current_policy = new_policy;
	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	/*                                                                        
                                                                           
                      */
	kbase_pm_update_active(kbdev);
	kbase_pm_update_cores_state(kbdev);

	mutex_unlock(&kbdev->pm.lock);

	/*                                                                                 */
	kbase_pm_context_idle(kbdev);
}

KBASE_EXPORT_TEST_API(kbase_pm_set_policy)

/*                                                                       */
STATIC void kbase_pm_trace_check_and_finish_state_change(struct kbase_device *kbdev)
{
	if ((kbdev->shader_available_bitmap & kbdev->pm.desired_shader_state) == kbdev->pm.desired_shader_state &&
		(kbdev->tiler_available_bitmap & kbdev->pm.desired_tiler_state) == kbdev->pm.desired_tiler_state)
		kbase_timeline_pm_check_handle_event(kbdev, KBASE_TIMELINE_PM_EVENT_GPU_STATE_CHANGED);
}

void kbase_pm_request_cores(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores)
{
	unsigned long flags;
	u64 cores;

	kbase_pm_change_state change_gpu_state = 0u;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	cores = shader_cores;
	while (cores) {
		int bitnum = fls64(cores) - 1;
		u64 bit = 1ULL << bitnum;

		/*                                                                                 
                                                                                    
                                                                                
                                            */
		int cnt = ++kbdev->shader_needed_cnt[bitnum];

		if (1 == cnt) {
			kbdev->shader_needed_bitmap |= bit;
			change_gpu_state |= KBASE_PM_CHANGE_STATE_SHADER;
		}

		cores &= ~bit;
	}

	if (tiler_required != MALI_FALSE) {
		++kbdev->tiler_needed_cnt;

		KBASE_DEBUG_ASSERT(kbdev->tiler_needed_cnt != 0);

		/*                                                                                    
                                                                                        
                                                                                  
                                                                                            
    
                                                                                          
                                                                                     
                                                       
    
                                                                                    
                                                                                       
                                                                   */
	}

	if (change_gpu_state) {
		KBASE_TRACE_ADD(kbdev, PM_REQUEST_CHANGE_SHADER_NEEDED, NULL, NULL, 0u, (u32) kbdev->shader_needed_bitmap);

		kbase_timeline_pm_cores_func(kbdev, KBASE_PM_FUNC_ID_REQUEST_CORES_START, change_gpu_state);
		kbase_pm_update_cores_state_nolock(kbdev);
		kbase_timeline_pm_cores_func(kbdev, KBASE_PM_FUNC_ID_REQUEST_CORES_END, change_gpu_state);
	}

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
}

KBASE_EXPORT_TEST_API(kbase_pm_request_cores)

void kbase_pm_unrequest_cores(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores)
{
	unsigned long flags;

	kbase_pm_change_state change_gpu_state = 0u;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	while (shader_cores) {
		int bitnum = fls64(shader_cores) - 1;
		u64 bit = 1ULL << bitnum;
		int cnt;

		KBASE_DEBUG_ASSERT(kbdev->shader_needed_cnt[bitnum] > 0);

		cnt = --kbdev->shader_needed_cnt[bitnum];

		if (0 == cnt) {
			kbdev->shader_needed_bitmap &= ~bit;

			change_gpu_state |= KBASE_PM_CHANGE_STATE_SHADER;
		}

		shader_cores &= ~bit;
	}

	if (tiler_required != MALI_FALSE) {
		KBASE_DEBUG_ASSERT(kbdev->tiler_needed_cnt > 0);

		--kbdev->tiler_needed_cnt;

		/*                                                                                   
                                                                                      
                                                                                        
                                                                                        
                                                                                      */
	}

	if (change_gpu_state) {
		KBASE_TRACE_ADD(kbdev, PM_UNREQUEST_CHANGE_SHADER_NEEDED, NULL, NULL, 0u, (u32) kbdev->shader_needed_bitmap);

		kbase_pm_update_cores_state_nolock(kbdev);

		/*                                                                
                                          */
		kbase_pm_trace_check_and_finish_state_change(kbdev);
	}

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
}

KBASE_EXPORT_TEST_API(kbase_pm_unrequest_cores)

enum kbase_pm_cores_ready kbase_pm_register_inuse_cores(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores)
{
	unsigned long flags;
	u64 prev_shader_needed;	/*                  */
	u64 prev_shader_inuse;	/*                  */

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	prev_shader_needed = kbdev->shader_needed_bitmap;
	prev_shader_inuse = kbdev->shader_inuse_bitmap;

	/*                                                                         
                                                                     
                                                                   
           */
	if ((kbdev->pm.desired_shader_state & shader_cores) != shader_cores) {
		spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

		return KBASE_NEW_AFFINITY;
	}

	if ((kbdev->shader_available_bitmap & shader_cores) != shader_cores ||
	    (tiler_required != MALI_FALSE && !kbdev->tiler_available_bitmap)) {
		/*                               */
		kbase_timeline_pm_l2_transition_start(kbdev);
		spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
		return KBASE_CORES_NOT_READY;
	}

	/*                                                                        
                               */
	kbase_pm_trace_check_and_finish_state_change(kbdev);
	/*                            */
	kbase_timeline_pm_l2_transition_done(kbdev);

	while (shader_cores) {
		int bitnum = fls64(shader_cores) - 1;
		u64 bit = 1ULL << bitnum;
		int cnt;

		KBASE_DEBUG_ASSERT(kbdev->shader_needed_cnt[bitnum] > 0);

		cnt = --kbdev->shader_needed_cnt[bitnum];

		if (0 == cnt)
			kbdev->shader_needed_bitmap &= ~bit;

		/*                                                                 
                                                        */

		kbdev->shader_inuse_cnt[bitnum]++;
		kbdev->shader_inuse_bitmap |= bit;

		shader_cores &= ~bit;
	}

	if (tiler_required != MALI_FALSE) {
		KBASE_DEBUG_ASSERT(kbdev->tiler_needed_cnt > 0);

		--kbdev->tiler_needed_cnt;

		kbdev->tiler_inuse_cnt++;

		KBASE_DEBUG_ASSERT(kbdev->tiler_inuse_cnt != 0);
	}

	if (prev_shader_needed != kbdev->shader_needed_bitmap)
		KBASE_TRACE_ADD(kbdev, PM_REGISTER_CHANGE_SHADER_NEEDED, NULL, NULL, 0u, (u32) kbdev->shader_needed_bitmap);

	if (prev_shader_inuse != kbdev->shader_inuse_bitmap)
		KBASE_TRACE_ADD(kbdev, PM_REGISTER_CHANGE_SHADER_INUSE, NULL, NULL, 0u, (u32) kbdev->shader_inuse_bitmap);

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);

	return KBASE_CORES_READY;
}

KBASE_EXPORT_TEST_API(kbase_pm_register_inuse_cores)

void kbase_pm_release_cores(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores)
{
	unsigned long flags;
	kbase_pm_change_state change_gpu_state = 0u;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	while (shader_cores) {
		int bitnum = fls64(shader_cores) - 1;
		u64 bit = 1ULL << bitnum;
		int cnt;

		KBASE_DEBUG_ASSERT(kbdev->shader_inuse_cnt[bitnum] > 0);

		cnt = --kbdev->shader_inuse_cnt[bitnum];

		if (0 == cnt) {
			kbdev->shader_inuse_bitmap &= ~bit;
			change_gpu_state |= KBASE_PM_CHANGE_STATE_SHADER;
		}

		shader_cores &= ~bit;
	}

	if (tiler_required != MALI_FALSE) {
		KBASE_DEBUG_ASSERT(kbdev->tiler_inuse_cnt > 0);

		--kbdev->tiler_inuse_cnt;

		/*                                                                                   
                                                                                      
                                                                                    
                                                                                    
                                                                                     */
	}

	if (change_gpu_state) {
		KBASE_TRACE_ADD(kbdev, PM_RELEASE_CHANGE_SHADER_INUSE, NULL, NULL, 0u, (u32) kbdev->shader_inuse_bitmap);

		kbase_timeline_pm_cores_func(kbdev, KBASE_PM_FUNC_ID_RELEASE_CORES_START, change_gpu_state);
		kbase_pm_update_cores_state_nolock(kbdev);
		kbase_timeline_pm_cores_func(kbdev, KBASE_PM_FUNC_ID_RELEASE_CORES_END, change_gpu_state);

		/*                                                   */
		kbase_pm_trace_check_and_finish_state_change(kbdev);
	}

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
}

KBASE_EXPORT_TEST_API(kbase_pm_release_cores)

void kbase_pm_request_cores_sync(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores)
{
	kbase_pm_request_cores(kbdev, tiler_required, shader_cores);

	kbase_pm_check_transitions_sync(kbdev);
}

KBASE_EXPORT_TEST_API(kbase_pm_request_cores_sync)

void kbase_pm_request_l2_caches(struct kbase_device *kbdev)
{
	unsigned long flags;
	u32 prior_l2_users_count;

	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	prior_l2_users_count = kbdev->l2_users_count++;

	KBASE_DEBUG_ASSERT(kbdev->l2_users_count != 0);

	/*                                                                                            
                                                                           */
	if (!prior_l2_users_count || !kbdev->l2_available_bitmap)
		kbase_pm_check_transitions_nolock(kbdev);

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
	wait_event(kbdev->pm.l2_powered_wait, kbdev->pm.l2_powered == 1);

	/*                                                   */
	kbase_pm_trace_check_and_finish_state_change(kbdev);
}

KBASE_EXPORT_TEST_API(kbase_pm_request_l2_caches)

void kbase_pm_request_l2_caches_l2_is_on(struct kbase_device *kbdev)
{
	unsigned long flags;
	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	kbdev->l2_users_count++;

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
}

KBASE_EXPORT_TEST_API(kbase_pm_request_l2_caches_l2_is_on)

void kbase_pm_release_l2_caches(struct kbase_device *kbdev)
{
	unsigned long flags;
	spin_lock_irqsave(&kbdev->pm.power_change_lock, flags);

	KBASE_DEBUG_ASSERT(kbdev->l2_users_count > 0);

	--kbdev->l2_users_count;

	if (!kbdev->l2_users_count) {
		kbase_pm_check_transitions_nolock(kbdev);
		/*                                                   */
		kbase_pm_trace_check_and_finish_state_change(kbdev);
	}

	spin_unlock_irqrestore(&kbdev->pm.power_change_lock, flags);
}

KBASE_EXPORT_TEST_API(kbase_pm_release_l2_caches)
#endif /*             */
