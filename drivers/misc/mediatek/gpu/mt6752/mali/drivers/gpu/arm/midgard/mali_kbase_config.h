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

#ifndef _KBASE_CONFIG_H_
#define _KBASE_CONFIG_H_

#include <malisw/mali_stdtypes.h>

/* 
                       
     
 */

/* 
                             
     
 */

/* 
                                                            
     
 */

#if !MALI_CUSTOMER_RELEASE
/*                                                                               */
#define KBASE_HWCNT_DUMP_BYPASS_ROOT 1
#else
#define KBASE_HWCNT_DUMP_BYPASS_ROOT 0
#endif

/*                                                                        */
#define KBASE_PM_EN 1
/*                                                                        */
#define KBASE_GPU_RESET_EN 1
/*                                                                           */
#define KBASE_MMU_HW_BACKEND 1


#include <linux/rbtree.h>


/* 
                            
 */
enum {
	/* 
                                     
   
                           
                     
    */
	KBASE_CONFIG_ATTR_INVALID,

	/*                                  */
	/* 
                                                                       
                           
   
                                                         
                                                                         
                                                                 
                               
                                                                         
                          
   
                                                            
                                               
                                                  
                                                   
   
                                                                         
                                                                          
                                                                           
                                                                          
             
   
                                                                 
                                            
   
                                                                              
                                                                        
                       
                                                     
   
                                                  
                                                                                 
                                                                          
                                                                        
                  
  */
	KBASE_CONFIG_ATTR_JS_SCHEDULING_TICK_NS,

	/* 
                                                                       
                     
   
                                                                        
                                                         
   
                                                                             
                                                       
                                                
   
                                                                 
                                            
   
                                                                              
                                                  
   
                                                                       
                                                          
   
                                                  
                                               
                                                                        
                                                                        
                                                                     
                              
   
                                                
  */
	KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS,

	/* 
                                                                   
                     
   
                                                                        
                                                         
   
                                                                    
                                                                
                                                
   
                                                                 
                                            
   
                                   
                                                                         
                                                     
   
                                                                       
                                                          
   
                                                  
                                                              
                                                                      
                                                                      
                                        
   
                                                
  */
	KBASE_CONFIG_ATTR_JS_SOFT_STOP_TICKS_CL,

	/* 
                                                                       
                     
   
                                                                                  
                                                    
   
                                                                 
                                            
   
                                                                                 
                                                     
   
                                                                       
                                                          
   
                                                
  */
	KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_SS,

	/* 
                                                                                     
   
                                                                                  
                                                    
   
                                                                 
                                            
   
                                                                                 
                                                     
   
                                                                       
                                                          
   
                                                
  */
	KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_CL,

	/* 
                                                                                 
                 
   
                                                                                  
                                                    
   
                                                                 
                                            
   
                                                                                  
                                                      
   
                                                                       
                                                          
   
                                                
  */
	KBASE_CONFIG_ATTR_JS_HARD_STOP_TICKS_NSS,

	/* 
                                                                               
   
                                                                         
                                     
   
                                                                            
                         
   
                                                                        
             
                                                 
                                                
   
                                                                 
                                            
   
                                                                            
                                                           
                                                   
   
                                                                       
                                                        
                                           
                                                        
                                                                       
                                                   
  */
	KBASE_CONFIG_ATTR_JS_CTX_TIMESLICE_NS,

	/* 
                                                                       
                              
   
                                                                                  
                                                                                    
                                                                                   
                                                                    
   
                                                                 
                                            
   
                                                                                  
                                                 
   
                                                
  */
	KBASE_CONFIG_ATTR_JS_RESET_TICKS_SS,

	/* 
                                                                   
                              
   
                                                                                   
                                                                                    
                                                                                   
                                                                    
   
                                                                 
                                            
   
                                                                                 
                                                 
   
                                                
  */
	KBASE_CONFIG_ATTR_JS_RESET_TICKS_CL,

	/* 
                                                                                    
                       
   
                                                                                  
                                                                                    
                                                                                   
                                                                    
   
                                                                 
                                            
   
                                                                                  
                                                  
   
                                                
  */
	KBASE_CONFIG_ATTR_JS_RESET_TICKS_NSS,

	/* 
                                                                
                                                
   
                                          
                                                   
  */
	KBASE_CONFIG_ATTR_JS_RESET_TIMEOUT_MS,
	/*                                */

	/*                                
   
                                                          
                                                  
  */
	KBASE_CONFIG_ATTR_POWER_MANAGEMENT_CALLBACKS,

	/* 
                                                         
                                      
                                                             
              
   
                                                               
                                                            
                                                     
  */
	KBASE_CONFIG_ATTR_CPU_SPEED_FUNC,

	/* 
                                                         
                                      
                                                             
              
   
                                                               
                                                                  
                                                             
  */
	KBASE_CONFIG_ATTR_GPU_SPEED_FUNC,

	/* 
                                             
   
                                                             
                                                     
  */
	KBASE_CONFIG_ATTR_PLATFORM_FUNCS,

	/* 
                                                
   
                             
                                   
  */
	KBASE_CONFIG_ATTR_POWER_MANAGEMENT_DVFS_FREQ,

	/* 
                                                                        
                           
   
                                                                  
                                         
                               
   
                                         
                                                        
  */
	KBASE_CONFIG_ATTR_PM_GPU_POWEROFF_TICK_NS,

	/* 
                                                                     
   
                                                                                    
                                                       
   
                                                  
  */
	KBASE_CONFIG_ATTR_PM_POWEROFF_TICK_SHADER,

	/* 
                                                           
   
                                                                                 
                                                    
   
                                                  
  */
	KBASE_CONFIG_ATTR_PM_POWEROFF_TICK_GPU,

	/*                     
   
                                                     
  */
	KBASE_CONFIG_ATTR_POWER_MODEL_CALLBACKS,

	/* 
                                    
                                                                   
                                      
   
                     
  */
	KBASE_CONFIG_ATTR_END = 0x1FFFUL
};

/*
                                      
  
                                                                                                              
 */
typedef struct kbase_attribute {
	int id;
	uintptr_t data;
} kbase_attribute;

/*                                            */
struct kbase_device;

/*
                                                                                      
  
                                                                                              
 */
typedef struct kbase_platform_funcs_conf {
	/* 
                                                                                                            
                                                                                                                    
                                
                                                                                                                            
  */
	mali_bool(*platform_init_func) (struct kbase_device *kbdev);
	/* 
                                                                                                      
                                                                                                                   
                                
                                                                                                                           
  */
	void (*platform_term_func)(struct kbase_device *kbdev);

} kbase_platform_funcs_conf;

/*
                                                      
  
                                                                            
 */
typedef struct kbase_pm_callback_conf {
	/*                                                                            
   
                                                                                  
                                                               
                                                                                                                        
                                                                                                                               
  */
	void (*power_off_callback)(struct kbase_device *kbdev);

	/*                                                                                 
   
                                                                                                          
                                                                                                               
                                                                                                  
                                                                                                                        
                                                                                                                               
   
                                                                   
   
                                                               
  */
	int (*power_on_callback)(struct kbase_device *kbdev);

	/*                                                                    
                         
   
                                                                  
                                                                  
                                                                        
                          
   
                                                                        
                                                                
                                                                        
                                              
  */
	void (*power_suspend_callback)(struct kbase_device *kbdev);

	/*                                                                 
                              
   
                                                                  
                                                                 
                                                                        
                         
   
                                                                        
                                                                
                                                                        
                                              
  */
	void (*power_resume_callback)(struct kbase_device *kbdev);

	/*                                                                
   
                                                                                                             
                                                                           
                                                                                                        
                                                                                       
   
                                                                  
  */
	 mali_error(*power_runtime_init_callback) (struct kbase_device *kbdev);

	/*                                                             
   
                                                                                                             
                                                                        
                                                                                       
  */
	void (*power_runtime_term_callback)(struct kbase_device *kbdev);

	/*                                                          
   
                                                                                  
                                                                                       
   
                                             
  */
	void (*power_runtime_off_callback)(struct kbase_device *kbdev);

	/*                                                         
   
                                                                                 
                                                                                       
  */
	int (*power_runtime_on_callback)(struct kbase_device *kbdev);

} kbase_pm_callback_conf;

/* 
                                                                          
  
                                                                         
                   
  
                                                                             
                    
 */
int kbase_cpuprops_get_default_clock_speed(u32 * const clock_speed);

/* 
                                                                     
  
                                                                                             
                                                                                        
  
                                    
 */
typedef int (*kbase_cpuprops_clock_speed_function) (u32 *clock_speed);

/* 
                                                                     
  
                                                                                             
                                                                                               
                                                                                                
  
                                                                                         
                                        
 */
typedef int (*kbase_gpuprops_clock_speed_function) (u32 *clock_speed);

#ifdef CONFIG_OF
typedef struct kbase_platform_config {
	const struct kbase_attribute *attributes;
	u32 midgard_type;
} kbase_platform_config;
#else

/*
                                                       
 */
typedef struct kbase_io_memory_region {
	u64 start;
	u64 end;
} kbase_io_memory_region;

/*
                                                                                         
 */
typedef struct kbase_io_resources {

	u32                      job_irq_number;
	u32                      mmu_irq_number;
	u32                      gpu_irq_number;
	struct kbase_io_memory_region io_memory_region;
} kbase_io_resources;

typedef struct kbase_platform_config {
	const struct kbase_attribute *attributes;
	const struct kbase_io_resources *io_resources;
	u32 midgard_type;
} kbase_platform_config;

#endif /*           */

/* 
                                                                         
  
                                                
   
                                                                                             
 */
const char *kbasep_midgard_type_to_string(u32 midgard_type);

/* 
                                                                                            
  
                                                                                                                      
                    
  
                                                                              
                                             
  
                                                                                
 */
const struct kbase_attribute *kbasep_get_next_attribute(const struct kbase_attribute *attributes, int attribute_id);

/* 
                                                      
  
                                                                                                             
                                     
  
                                                  
                                                                              
                                             
  
                                               
 */
uintptr_t kbasep_get_config_value(struct kbase_device *kbdev, const struct kbase_attribute *attributes, int attribute_id);

/* 
                                            
  
                                                                                                                       
                                                                         
  
                                               
                                                          
  
                                                                                        
 */
mali_bool kbasep_validate_configuration_attributes(struct kbase_device *kbdev, const struct kbase_attribute *attributes);

/* 
                                              
  
                                         
 */
struct kbase_platform_config *kbase_get_platform_config(void);

/* 
                                               
  
                                                                                                    
  
                                                 
  
                                                                              
 */
int kbasep_get_config_attribute_count(const struct kbase_attribute *attributes);

/* 
                                                       
  
                                                                                          
                                                                                                            
  
                                               
  
                                                                                        
 */
mali_bool kbasep_platform_device_init(struct kbase_device *kbdev);

/* 
                                                      
  
                                                                                          
                                                                                                                
                                             
  
                                               
  
 */
void kbasep_platform_device_term(struct kbase_device *kbdev);

	  /*     *//*                        */
	  /*     *//*                          */
	  /*     *//*                    */

#endif				/*                  */
