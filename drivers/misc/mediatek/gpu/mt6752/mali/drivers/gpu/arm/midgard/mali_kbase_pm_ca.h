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

#ifndef _KBASE_PM_CA_H_
#define _KBASE_PM_CA_H_

enum kbase_pm_ca_policy_id {
	KBASE_PM_CA_POLICY_ID_FIXED = 1,
	KBASE_PM_CA_POLICY_ID_RANDOM
};

typedef u32 kbase_pm_ca_policy_flags;

/*                                     
  
                                                                                                                      
                    
 */
struct kbase_pm_ca_policy {
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
	void (*update_core_status)(struct kbase_device *kbdev, u64 cores_ready, u64 cores_transitioning);

	/*                                         */
	kbase_pm_ca_policy_flags flags;

	/*                                                                     
                                                                       
                                     */
	enum kbase_pm_ca_policy_id id;
};

/*                                        
  
                                                                     
  
                                                                                       
  
                                                                                           
 */
mali_error kbase_pm_ca_init(struct kbase_device *kbdev);

/*                                       
  
                                                                                       
 */
void kbase_pm_ca_term(struct kbase_device *kbdev);

/*                                                  
                                          
  
                                                                                       
  
                                                   
 */
u64 kbase_pm_ca_get_core_mask(struct kbase_device *kbdev);

/*                                                                
                                          
  
                                                                                                  
                                                                             
                                                                                       
 */
void kbase_pm_ca_update_core_status(struct kbase_device *kbdev, u64 cores_ready, u64 cores_transitioning);

/*                                     
  
                                                                                                   
  
                                                                                                  
 */
void kbase_pm_ca_instr_enable(struct kbase_device *kbdev);

/*                                      
  
                                                                                         
  
                                                                                                  
 */
void kbase_pm_ca_instr_disable(struct kbase_device *kbdev);

/*                         
                                               
  
                                                                                       
  
                             
 */
const struct kbase_pm_ca_policy *kbase_pm_ca_get_policy(struct kbase_device *kbdev);

/*                                         
  
                                                                                       
                                                                                                        
 */
void kbase_pm_ca_set_policy(struct kbase_device *kbdev, const struct kbase_pm_ca_policy *policy);

/*                                                   
                                                                                            
                                                                            
  
                                 
 */
int kbase_pm_ca_list_policies(const struct kbase_pm_ca_policy * const **policies);

#endif				/*                 */
