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

#ifndef _KBASE_PM_POLICY_H_
#define _KBASE_PM_POLICY_H_

/*                     */
enum kbase_pm_policy_id {
	KBASE_PM_POLICY_ID_DEMAND = 1,
	KBASE_PM_POLICY_ID_ALWAYS_ON,
	KBASE_PM_POLICY_ID_COARSE_DEMAND,
#if !MALI_CUSTOMER_RELEASE
	KBASE_PM_POLICY_ID_DEMAND_ALWAYS_POWERED,
	KBASE_PM_POLICY_ID_FAST_START
#endif
};

typedef u32 kbase_pm_policy_flags;

/*                         
  
                                                                                                          
                    
 */
typedef struct kbase_pm_policy {
	/*                          */
	char *name;

	/*                                             
   
                                                                                        
                                          
   
                                                                            
   
                                                                                        
  */
	void (*init)(struct kbase_device *kbdev);

	/*                                                
   
                                                                                        
  */
	void (*term)(struct kbase_device *kbdev);

	/*                                                     
   
                                                                                                       
   
                                                                                        
   
                                                       */
	u64 (*get_core_mask)(struct kbase_device *kbdev);

	/*                                                            
   
                                                                                                           
                                                                                                         
                                                                                  
   
                                                                                        
   
                                                                             */
	mali_bool (*get_core_active) (struct kbase_device *kbdev);

	/*                                         */
	kbase_pm_policy_flags flags;

	/*                                                                     
                                                                       
                                     */
	enum kbase_pm_policy_id id;
} kbase_pm_policy;

/*                                   
  
                                                          
  
                                                                                       
  
                                                                                      
 */
mali_error kbase_pm_policy_init(struct kbase_device *kbdev);

/*                                  
  
                                                                                       
 */
void kbase_pm_policy_term(struct kbase_device *kbdev);

/*                                          
                                      
  
                                                                                       
 */
void kbase_pm_update_active(struct kbase_device *kbdev);

/*                                          
                                      
  
                                                                                       
 */
void kbase_pm_update_cores(struct kbase_device *kbdev);

/*                         
                                               
  
                                                                                       
  
                             
 */
const struct kbase_pm_policy *kbase_pm_get_policy(struct kbase_device *kbdev);

/*                                         
  
                                                                                       
                                                                                                     
 */
void kbase_pm_set_policy(struct kbase_device *kbdev, const struct kbase_pm_policy *policy);

/*                                                   
                                                                                            
                                                                            
  
                                 
 */
int kbase_pm_list_policies(const struct kbase_pm_policy * const **policies);


enum kbase_pm_cores_ready {
	KBASE_CORES_NOT_READY = 0,
	KBASE_NEW_AFFINITY = 1,
	KBASE_CORES_READY = 2
};


/*                                                 
  
                                                                              
  
                                                                            
                                                                            
                                                                               
           
  
                                                                   
                                                                                  
                                                                                   
 */

void kbase_pm_request_cores_sync(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores);

/*                                                                    
  
                                                                         
                                                          
  
                                                                                                           
                                                                                           
  
                                                                      
                                                                           
                                                                            
                                
  
                                                                   
                                                                                  
                                                                                   
  
                                                                    
 */
void kbase_pm_request_cores(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores);

/*                                                                      
  
                                                                                                      
                                                                                           
  
                                                                      
                                                                           
                                                                         
  
                                                                         
  
                                                                   
                                                                                  
                                                                                             
 */
void kbase_pm_unrequest_cores(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores);

/*                                             
  
                                                                                                            
                                                                                                               
                                                                  
  
                                                                                                                  
  
                                                                   
                                                                                  
                                                                                             
  
                                                                                                              
 */
enum kbase_pm_cores_ready kbase_pm_register_inuse_cores(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores);

/*                                    
  
                                                                                                 
                                                                                                                   
                                                                                                                 
                                     
  
                                                                 
                                                                                  
                                                                                                  
 */
void kbase_pm_release_cores(struct kbase_device *kbdev, mali_bool tiler_required, u64 shader_cores);

/*                                                                                                     
                                
  
                                                                                  
                                                                                   
                                              
  
                                                                                  
                                                             
  
                                                             
  
                                                                                      
 */
void kbase_pm_request_l2_caches(struct kbase_device *kbdev);

/*                                                                       
                                                                                
                                                            
  
                                                                                      
 */
void kbase_pm_request_l2_caches_l2_is_on(struct kbase_device *kbdev);

/*                                                                                 
                                   
  
                                                                                                  
                                 
  
                                                                                        
  
                                                             
  
                                                                                      
 */
void kbase_pm_release_l2_caches(struct kbase_device *kbdev);

#endif				/*                     */
