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

#ifndef _KBASE_JS_AFFINITY_H_
#define _KBASE_JS_AFFINITY_H_

/* 
                       
     
 */

/* 
                             
     
 */

/* 
                                                                
     
  
 */

/* 
                                                                                                      
  
                                                                           
                                                                               
                                                                           
                                                                               
            
  
                                                           
                                                          
  
                                                        
                                                      
 */
mali_bool kbase_js_can_run_job_on_slot_no_lock(struct kbase_device *kbdev, int js);

/* 
                                           
  
                                                               
                                                                
  
                                                                  
                           
  
                                                      
                                                        
                                                                
                                                     

 */
mali_bool kbase_js_choose_affinity(u64 * const affinity, struct kbase_device *kbdev, struct kbase_jd_atom *katom, int js);

/* 
                                                                          
                                              
  
                                                  
                                             
 */
mali_bool kbase_js_affinity_would_violate(struct kbase_device *kbdev, int js, u64 affinity);

/* 
                                                        
  
                                                  
                                             
 */
void kbase_js_affinity_retain_slot_cores(struct kbase_device *kbdev, int js, u64 affinity);

/* 
                                                         
  
                                                                               
                                                                             
                                                                  
                        
  
                                                  
                                             
 */
void kbase_js_affinity_release_slot_cores(struct kbase_device *kbdev, int js, u64 affinity);

/* 
                                                                      
  
                                                                             
                                                                 
                                     
                                                                              
                                                              
  
                                                  
                                             
 */
void kbase_js_affinity_slot_blocked_an_atom(struct kbase_device *kbdev, int js);

/* 
                                                                              
                                                  
  
                                          
                                                                              
                                                               
                                                                           
                                                     
  
                                                                         
                                              
  
                                                           
                                                      
                                                          
 */
void kbase_js_affinity_submit_to_blocked_slots(struct kbase_device *kbdev);

/* 
                                                                             
 */
#if KBASE_TRACE_ENABLE
void kbase_js_debug_log_current_affinities(struct kbase_device *kbdev);
#else				/*                      */
static INLINE void kbase_js_debug_log_current_affinities(struct kbase_device *kbdev)
{
}
#endif				/*                      */

	  /*     *//*                             */
	  /*     *//*                          */
	  /*     *//*                    */


#endif				/*                       */
