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

#ifndef _KBASE_PM_H_
#define _KBASE_PM_H_

#include <mali_midg_regmap.h>
#include <linux/atomic.h>

/*                                       */
struct kbase_device;

#include "mali_kbase_pm_ca.h"
#include "mali_kbase_pm_policy.h"

#include "mali_kbase_pm_ca_fixed.h"
#if !MALI_CUSTOMER_RELEASE
#include "mali_kbase_pm_ca_random.h"
#endif

#include "mali_kbase_pm_always_on.h"
#include "mali_kbase_pm_coarse_demand.h"
#include "mali_kbase_pm_demand.h"
#if !MALI_CUSTOMER_RELEASE
#include "mali_kbase_pm_demand_always_powered.h"
#include "mali_kbase_pm_fast_start.h"
#endif

/*                             
  
                                                
                                    
                                   
                                  
                                   
  
                                                                               
                                                                             
             
 */
enum kbase_pm_core_type {
	KBASE_PM_CORE_L3 = L3_PRESENT_LO,	    /*                */
	KBASE_PM_CORE_L2 = L2_PRESENT_LO,	    /*                */
	KBASE_PM_CORE_SHADER = SHADER_PRESENT_LO,   /*                */
	KBASE_PM_CORE_TILER = TILER_PRESENT_LO	    /*               */
};
/*          */
#define MTK_MT6752_GPU_LIMIT_COUNT 3

typedef struct _mtk_gpu_freq_limit_data{
    const int actual_freq_index_count;
    const int virtual_freq_index_count;
    const int *virtual_freq_index;
}mtk_gpu_freq_limit_data;

extern mtk_gpu_freq_limit_data mt6752_gpu_freq_limit_data[];


/*                                            
  
                                                            
  
                                                                                       
  
                                                                                          
 */
mali_error kbase_pm_init(struct kbase_device *kbdev);

/*                                                                                        
  
                                                                                       
  
                                                     
 */
mali_error kbase_pm_powerup(struct kbase_device *kbdev);

/* 
                                       
                                                      
                                                                               
                                                                         
                                                                             
  
                                                                                       
 */
void kbase_pm_halt(struct kbase_device *kbdev);

/*                                           
  
                                                         
                              
  
                                                                                       
 */
void kbase_pm_term(struct kbase_device *kbdev);

/*                                                                   
  
 */
struct kbasep_pm_metrics_data {
	int vsync_hit;
	int utilisation;
	int util_gl_share;
	int util_cl_share[2]; /*                                              */
	ktime_t time_period_start;
	u32 time_busy;
	u32 time_idle;
	mali_bool gpu_active;
	u32 busy_cl[2];
	u32 busy_gl;
	u32 active_cl_ctx[2];
	u32 active_gl_ctx;

	spinlock_t lock;

#ifdef CONFIG_MALI_MIDGARD_DVFS
	struct hrtimer timer;
	mali_bool timer_active;
#endif

	void *platform_data;
	struct kbase_device *kbdev;
};

/*                   
  
                                                                         
                                                        
 */
enum kbase_pm_dvfs_action {
	KBASE_PM_DVFS_NOP,	    /*                                             */
	KBASE_PM_DVFS_CLOCK_UP,	    /*                                                       */
	KBASE_PM_DVFS_CLOCK_DOWN,    /*                                                       */
	KBASE_PM_DVFS_NONSENSE       /*                                              */
};

union kbase_pm_policy_data {
	struct kbasep_pm_policy_always_on always_on;
	struct kbasep_pm_policy_coarse_demand coarse_demand;
	struct kbasep_pm_policy_demand demand;
#if !MALI_CUSTOMER_RELEASE
	struct kbasep_pm_policy_demand_always_powered demand_always_powered;
	struct kbasep_pm_policy_fast_start fast_start;
#endif
};

union kbase_pm_ca_policy_data {
	struct kbasep_pm_ca_policy_fixed fixed;
#if !MALI_CUSTOMER_RELEASE
	struct kbasep_pm_ca_policy_random random;
#endif
};

/*                                              
  
                                                                                                                      
                 
 */
struct kbase_pm_device_data {
	/*                                                          
                   
   
                                                                            
  */
	struct mutex lock;

	/*                                                                      
   
                                                                           
                                                                
                                                                       
                                                        
                                                                             
                        
  */
	const struct kbase_pm_ca_policy *ca_current_policy;

	/*                                                                    
   
                                                                           
                                                             
                                                                       
                                                        
                                                                          
                        
  */
	const struct kbase_pm_policy *pm_current_policy;

	/*                                     */
	union kbase_pm_ca_policy_data ca_policy_data;

	/*                                     */
	union kbase_pm_policy_data pm_policy_data;

	/*                                                                       
                                                                                 
                
   
                                                        */
	mali_bool ca_in_transition;

	/*                                                    */
	mali_bool reset_done;
	wait_queue_head_t reset_done_wait;

	/*                                                                    */
	wait_queue_head_t l2_powered_wait;
	/*                                                         
                                            
                                                      */
	int l2_powered;

	/*                                                         */
	int active_count;
	/*                                       */
	mali_bool suspending;
	/*                                       */
	wait_queue_head_t zero_active_count_wait;

	/*                                                        */
	int gpu_cycle_counter_requests;
	/*                                             */
	spinlock_t gpu_cycle_counter_requests_lock;

	/*                                                                                    
                                                                                                          
                                                                                                     
  */
	u64 desired_shader_state;
	/*                                                                                */
	u64 powering_on_shader_state;
	/*                                                                                   
                                                   */
	u64 desired_tiler_state;
	/*                                                                              */
	u64 powering_on_tiler_state;

	/*                                                                             */
	u64 powering_on_l2_state;
	/*                                                                             */
	u64 powering_on_l3_state;

	/*                                                
   
                                                                                                                   
                                                                                                                           
                                                                                                                     
                                                                                              
  */
	spinlock_t power_change_lock;

	/*                                                             
                                */
	mali_bool gpu_in_desired_state;
	/*                                               */
	wait_queue_head_t gpu_in_desired_state_wait;

	/*                                                                                          */
	mali_bool gpu_powered;

	/*                                                                                 */
	u64 debug_core_mask;

	/*                                                               */
	mali_bool instr_enabled;

	mali_bool cg1_disabled;

#ifdef CONFIG_MALI_DEBUG
	/*                                                                        
                                */
	mali_bool driver_ready_for_irqs;
#endif /*                   */

	/*                                  
                         
                                                                    */
	spinlock_t gpu_powered_lock;

	/*                                                */

	u32 platform_dvfs_frequency;

	/*                                        */

	struct kbasep_pm_metrics_data metrics;

	/*                                                                         */
	int gpu_poweroff_pending;

	/*                                                                          */
	int shader_poweroff_pending_time;

	/*                             */
	struct hrtimer gpu_poweroff_timer;

	struct workqueue_struct *gpu_poweroff_wq;

	struct work_struct gpu_poweroff_work;

	/*                               */
	ktime_t gpu_poweroff_time;

	/*                                                               */
	u64 shader_poweroff_pending;

	/*                                                                                    */
	mali_bool poweroff_timer_running;

	int poweroff_shader_ticks;

	int poweroff_gpu_ticks;

	/*                                                                              
   
                                         
   
                                                
  */
	int (*callback_power_on)(struct kbase_device *kbdev);

	/*                                                                          
   
                                         
  */
	void (*callback_power_off)(struct kbase_device *kbdev);

	/*                                                                    
                                    
   
                                         
  */
	void (*callback_power_suspend)(struct kbase_device *kbdev);

	/*                                                                  
                                    
   
                                         
  */
	void (*callback_power_resume)(struct kbase_device *kbdev);

	/*                                                         
   
                                         
   
                                                       
  */
	 mali_error (*callback_power_runtime_init)(struct kbase_device *kbdev);

	/*                                                        
   
                                         
  */
	void (*callback_power_runtime_term)(struct kbase_device *kbdev);

	/*                                                                              
   
                                         
   
                                                
  */
	int (*callback_power_runtime_on)(struct kbase_device *kbdev);

	/*                                                                          
   
                                         
  */
	void (*callback_power_runtime_off)(struct kbase_device *kbdev);

};

/*                  
  
                                             
  
                                                                                       
 */
void kbase_pm_dev_idle(struct kbase_device *kbdev);

/*                    
  
                                                                               
  
                                                                                       
 */
void kbase_pm_dev_activate(struct kbase_device *kbdev);

/*                                                          
  
                                                                                                                
                                                                     
  
                                                                                       
                                                                                       
  
                                                 
 */
u64 kbase_pm_get_present_cores(struct kbase_device *kbdev, enum kbase_pm_core_type type);

/*                                                                   
  
                                                                                                                     
                                                            
  
                                                                                       
                                                                                       
  
                                                
 */
u64 kbase_pm_get_active_cores(struct kbase_device *kbdev, enum kbase_pm_core_type type);

/*                                                                                 
  
                                                                                                                     
                                                    
  
                                                                                       
                                                                                       
  
                                                       
 */
u64 kbase_pm_get_trans_cores(struct kbase_device *kbdev, enum kbase_pm_core_type type);

/*                                                                         
  
                                                                                                                     
                                                                                    
  
                                                                                       
                                                                                       
  
                                               
 */
u64 kbase_pm_get_ready_cores(struct kbase_device *kbdev, enum kbase_pm_core_type type);

/*                                                                 
  
                                                                                                             
                                                                                                    
  
                                                                                         
                                                                        
                                          
 */
void kbase_pm_clock_on(struct kbase_device *kbdev, mali_bool is_resume);

/*                                                                   
  
                                                                                                              
                                                                                                               
  
                                                                                         
                                                                                 
 */
void kbase_pm_clock_off(struct kbase_device *kbdev, mali_bool is_suspend);

/*                                  
  
                                                                   
  
                                                                                       
 */
void kbase_pm_enable_interrupts(struct kbase_device *kbdev);

/*                                   
  
                                                                           
                                                                              
                                                                          
  
                                                                     
  
                                                                                       
 */
void kbase_pm_disable_interrupts(struct kbase_device *kbdev);

/*                         
  
                                                                                                                     
                                                                       
  
                                                                                          
                                                                                           
                                                  
  
                                                                             
 */
mali_error kbase_pm_init_hw(struct kbase_device *kbdev, mali_bool enable_irqs);

/*                                      
  
                                                                                                                   
                                                                                 
  
                                                                                       
 */
void kbase_pm_reset_done(struct kbase_device *kbdev);

/*                                         
  
                                                                                                                      
                                                                                            
  
                                                       
  
                                                                             
                                                                   
  
                                                                             
                                                                               
          
  
                                                                                       
 */
void kbase_pm_context_active(struct kbase_device *kbdev);


/*                                                                   */
enum kbase_pm_suspend_handler {
	/*                                                              
                              */
	KBASE_PM_SUSPEND_HANDLER_NOT_POSSIBLE,
	/*                                                                */
	KBASE_PM_SUSPEND_HANDLER_DONT_INCREASE,
	/*                                                                         
                                                                 
   
                                                                           
                                                            */
	KBASE_PM_SUSPEND_HANDLER_DONT_REACTIVATE
};

/*                                                     
  
                                                                         
                                                                                 
  
                                                                            
                                                                               
                                                                       
                                                                        
  
                                                                                       
                                                                                       
                                     
                                                                                   
 */
int kbase_pm_context_active_handle_suspend(struct kbase_device *kbdev, enum kbase_pm_suspend_handler suspend_handler);

/*                                                   
  
                                                                                                                     
                                                                                        
  
                                                                                       
 */
void kbase_pm_context_idle(struct kbase_device *kbdev);

/*                                                                         
  
                                                                                                                    
                                                                                                                   
                               
  
                                                                          
                                                                           
                   
  
                                                                      
                                                                              
                                                                               
                                                                            
                                                                           
            
  
                                                           
  
                                                                                       
                                                                           
                                                                   
                                        
 */
mali_bool kbase_pm_check_transitions_nolock(struct kbase_device *kbdev);

/*                                                                        
  
                                                                              
  
                                                                         
                                                                         
                                          
                                                                     
  
                                                                              
               
  
                                                                       
                                                                            
               
  
                                                                                       
 */
void kbase_pm_check_transitions_sync(struct kbase_device *kbdev);

/*                                                                     
                                      
  
                                                                                
                              
 */
void kbase_pm_update_cores_state_nolock(struct kbase_device *kbdev);

/*                                                                           
                         
  
                                                            
                                                                                     
                                                                               
         
  
                                                                                
                              
 */
void kbase_pm_update_cores_state(struct kbase_device *kbdev);

/*                                                                       
  
                                                                           
  
                                                                                
                              
 */
void kbase_pm_cancel_deferred_poweroff(struct kbase_device *kbdev);

/*                                     
  
                                                                                                             
  
                                                                                       
 */
void kbasep_pm_read_present_cores(struct kbase_device *kbdev);

/*                                             
  
                                                                     
  
                                                                                       
  
                                                                          
 */
mali_error kbasep_pm_metrics_init(struct kbase_device *kbdev);

/*                                            
  
                                                                                                                
                                                                            
  
                                                                                       
 */
void kbasep_pm_metrics_term(struct kbase_device *kbdev);

/*                                               
  
                                                                     
                                           
  
                                                             
                                             
 */
void kbasep_pm_record_job_status(struct kbase_device *kbdev);

/*                                
  
                                                                                                                   
                                     
  
                                                                                       
 */
void kbasep_pm_record_gpu_active(struct kbase_device *kbdev);

/*                              
  
                                                                                                                   
                                     
  
                                                                                       
 */
void kbasep_pm_record_gpu_idle(struct kbase_device *kbdev);

/*                                                                              
  
                                                                                                                     
                                                                                                                 
                                                                                                                   
                                
  
                                                                                               
                                                                                              
 */
void kbase_pm_report_vsync(struct kbase_device *kbdev, int buffer_updated);

/*                                                              
  
                                                                                                             
                        
  
                                                                           
  
                                                                                       
 */
void kbase_pm_register_vsync_callback(struct kbase_device *kbdev);

/*                                                                     
  
                                                                                                       
                                                               
  
                                                                           
  
                                                                                       
 */
void kbase_pm_unregister_vsync_callback(struct kbase_device *kbdev);

/*                                                                             
  
                                                                                                                
                                                                                   
  
                                                                                                       
                                                                    
                                                                                        
                                                                                        
 */
enum kbase_pm_dvfs_action kbase_pm_get_dvfs_action(struct kbase_device *kbdev);

/*                                                                              
                                                                       
  
                                                                                                                
  
                                                                                      
 */
void kbase_pm_request_gpu_cycle_counter(struct kbase_device *kbdev);

/*                                                                                                       
                                                                                                 
                                                                        
                                                               
  
                                                                                                                 
                                        
                                                                                       
 */
void kbase_pm_request_gpu_cycle_counter_l2_is_on(struct kbase_device *kbdev);

/*                                                                                
                                                                                      
                          
  
                                                                                      
 */

void kbase_pm_release_gpu_cycle_counter(struct kbase_device *kbdev);

/*                                                                                    
                            
  
                                                                                     
                               
  
                                                                                       
                                                            
                                    
  
                                                                                         
  
                                                                                      
 */
void kbase_pm_register_access_enable(struct kbase_device *kbdev);

/*                                                                   
                                      
  
                                                                                       
                                                             
                                   
  
                                                                                         
  
                                                                                      
 */
void kbase_pm_register_access_disable(struct kbase_device *kbdev);

/* 
                                                                              
           
  
                                                                                
                                            
  
                                                                             
                                                                              
                                    
  
                                                                                       
 */
void kbase_pm_suspend(struct kbase_device *kbdev);

/* 
                                                                             
           
  
                                                                               
                                           
  
                                                                                       
 */
void kbase_pm_resume(struct kbase_device *kbdev);

/*                                                                                   */

/* 
                                                              
  
                                                                        
                                                                                
                                            
  
                                                                          
  
                                                                                       
                                                                               
 */

mali_bool kbase_pm_metrics_is_active(struct kbase_device *kbdev);

/* 
                                                      
  
                                                                                          
                                                                         
                                           
 */
void kbase_pm_do_poweron(struct kbase_device *kbdev, mali_bool is_resume);

/* 
                                                             
  
                                                                                          
                                                             
                                           
 */
void kbase_pm_do_poweroff(struct kbase_device *kbdev, mali_bool is_suspend);

#ifdef CONFIG_PM_DEVFREQ
void kbase_pm_get_dvfs_utilisation(struct kbase_device *kbdev,
		unsigned long *total, unsigned long *busy, bool reset);
#endif

#ifdef CONFIG_MALI_MIDGARD_DVFS

/* 
                                                                            
                                                             
  
                                                                                             
                                                                                   
                                                                         
                                                                                        
                                                                       
 */

int kbase_platform_dvfs_event(struct kbase_device *kbdev, u32 utilisation,
	u32 util_gl_share, u32 util_cl_share[2]);
#endif

void proc_mali_register(void);
void proc_mali_unregister(void);

u32 kbasep_get_gl_utilization(void);
u32 kbasep_get_cl_js0_utilization(void);
u32 kbasep_get_cl_js1_utilization(void);

enum kbase_pm_dvfs_action mtk_get_dvfs_action(void);

void mtk_clear_dvfs_action(void);
int mtk_get_dvfs_enabled(void);
unsigned int mtk_get_current_gpu_platform_id(void);

void mtk_gpu_input_boost_CB(unsigned int ui32BoostFreqID);
void mtk_gpu_power_limit_CB(unsigned int ui32LimitFreqID);

int mtk_get_input_boost_enabled(void);

void mtk_kbase_boost_gpu_freq(void);

void mtk_kbase_custom_boost_gpu_freq(unsigned int ui32FreqLevel);
void mtk_kbase_ged_bottom_gpu_freq(unsigned int ui32FreqLevel);
unsigned int mtk_kbase_custom_get_gpu_freq_level_count(void);

int mtk_get_dvfs_freq(void);
int mtk_get_dvfs_threshold_max(void);
int mtk_get_dvfs_threshold_min(void);
int mtk_get_dvfs_deferred_count(void);

void mtk_get_touch_boost_flag(int *touch_boost_flag, int *touch_boost_id);
void mtk_set_touch_boost_flag(int boost_id);
void mtk_clear_touch_boost_flag(void);

#endif				/*              */
