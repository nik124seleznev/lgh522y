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

#ifndef _KBASE_JS_POLICY_H_
#define _KBASE_JS_POLICY_H_

/* 
                                                     
                                                           
                                           
                                            
  
                                  
                  
                  
                         
 */

/* 
                                                                    
  
                                    
                                                                    
                                                                              
                             
                                                                               
                
                                                           
  
                                                          
                                                                           
                             
                                                                                     
                                               
  
                                                                                 
                                                                              
           
  
                                                                              
                                                                                                                                                                                                   
  
                                        
                              
                                            
                                                                           
                                                                               
          
  
                                                                          
                                                                            
           
  
                                                                           
                                                                           
                                                                               
                   
  
                                                                            
                                                                           
                                                                           
                                                                           
                                                                          
        
 */

/* 
                                                        
  
                                                                             
                       
                 
                            
                                          
  
                                                                         
  
                                                                             
                                                                         
                                                                            
                                                                           
                                                                       
                                                                      
                                                                             
                                                                           
                                                                
  
                                                                              
                                                                                                                                                                          
  
                                                                    
  
                                                                          
                                                                               
                                                                               
                                                                            
                                                                             
                                                                              
                                                                            
                                                                               
                                                                             
                                                                            
                                         
  
                                                                            
                                                                         
                                                                              
                                                                             
                                                                              
                                                                             
                                                                   
  
  
                                                      
  
                                                                     
                                             
                                                                         
           
                                                                          
                
                                                                              
                                                                                 
                                    
                                                       
                                                             
                                                                         
                                                 
                                                   
                                                                             
                
                                                     
                                                                           
        
                                                  
                                                                               
                                  
                                                       
                                                
                                                         
                                                    
                                                                                   
                                                                                         
                                                                  
                                                                
                                                                       
                                                                   
                                                                                    
                                                                           
                                                               
                                                     
  
                                                                            
                                                                         
                                                                               
                                                                              
                                                          
  
                                                                               
                                                                             
                                                                            
                                                                                  
                                                                             
                                                                               
                                                                           
                                                       
  
                                                                            
                                                                               
                                                             
                                                        
                                                                           
                                                                              
                                                                  
                                                                               
                                              
                                                                             
                                                                                           
                                                                  
                                                                        
                                                                      
                                                                              
                          
                                                                           
                                                                              
                                                                      
                                                                            
                                               
  
                                                                            
                             
                                      
                                                                               
                                                                                    
                                                                          
                                                                             
                                                                           
                                                                               
                                     
                                                                               
                                                                
                                                                               
                                                               
                                          
                                                                             
                 
  
                                                                               
                                                                 
  
  
  
                                                                
  
                                                                              
                                                                        
            
                                                                                   
                    
                                                                       
                                   
                                         
                                                                           
                                                                          
                                                              
                                         
                                                                              
  
                                                                              
                                                
                                                                             
                                                                            
                                                                
                                                                               
                                                                               
                                                     
  
                                            
                                                     
                                              
                                                                             
                               
                                      
                                             
                                    
                                                                         
                
                                                                              
                                                                             
                                                                              
                                                              
                                                                             
                 
                                                                             
                                                                           
       
                                                
                                                                    
                                                                                    
  
                                                                                  
                                                                               
                   
                                                                             
                                            
                                                                             
                                                                              
                                                                               
        
                                                                              
                                                      
  
  
  
                                                                                                 
  
                                                                               
                                                                               
                                                                              
                                        
  
                                                                              
                                                                              
                                
                                                                             
                         
                                                                           
                                                                
                                                                             
                                                                      
                                                                           
                                                                            
                                         
                                                                             
                                                                                
                                                                      
                                                                          
                                                
                                                                             
                             
                                                                           
                                                                           
        
                                                                        
                                                        
                                                                                 
                                                                               
                                                                    
                                               
                                                                          
                                                                              
                                                          
                                                                             
                                                                                 
                                 
                                                                               
         
                                                                              
                                                                             
                                                                               
             
                                                                             
                                                     
                                                                             
           
  
                                                                           
                                                                               
                                                                             
                                                                               
                                                                                      
  
  
  
                                                     
  
                                                                              
                                                                              
                                                                 
                                                                          
                                                    
  
  
  
 */

/* 
                       
     
 */

/* 
                             
     
 */

/* 
                                                        
     
  
                                                                                  
                                                                        
 */

/* 
                                        
 */
union kbasep_js_policy;

/* 
                                             
 */
mali_error kbasep_js_policy_init(struct kbase_device *kbdev);

/* 
                                            
 */
void kbasep_js_policy_term(union kbasep_js_policy *js_policy);

/* 
                                                                               
     
  
                                                                                  
                                                                        
 */

/* 
                                                 
  
                                                                                   
                                                                               
                          
                                                                                     
                                                                           
                                            
                                                                            
                                                                             
           
 */
union kbasep_js_policy_ctx_info;

/* 
                                                                
  
                                                                                    
                                                                                        
 */
mali_error kbasep_js_policy_init_ctx(struct kbase_device *kbdev, struct kbase_context *kctx);

/* 
                                                                              
          
 */
void kbasep_js_policy_term_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx);

/* 
                                                               
  
                                                                               
                                                                            
                                                                              
                
  
                                                                            
                     
  
                                                                    
                                                                 
 */
void kbasep_js_policy_enqueue_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx);

/* 
                                                                           
  
                                                                 
  
                                                                        
                     
                                                    
 */
mali_bool kbasep_js_policy_dequeue_head_ctx(union kbasep_js_policy *js_policy, struct kbase_context ** const kctx_ptr);

/* 
                                                             
  
                                                             
  
                                                                      
                                                                             
                                                                              
                                                               
                                                                             
              
  
                                                                              
                                                                          
  
                                                                 
  
                                                                     
                                                                      
 */
mali_bool kbasep_js_policy_try_evict_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx);

/* 
                                                                            
                                                                      
  
                                                                         
                                                                             
                                                                            
                           
                                 
                                          
  
                                                                       
                                       
                                   
  
                                                                            
                                                                           
                   
  
                                                                             
                           
  
                                                       
                                                              
 */
void kbasep_js_policy_foreach_ctx_job(union kbasep_js_policy *js_policy, struct kbase_context *kctx,
	kbasep_js_policy_ctx_job_cb callback, mali_bool detach_jobs);

/* 
                                                              
  
                                                                               
                                                                          
                                                       
  
                                                                             
                            
  
                                                                            
             
                                
                                                
                                                                          
                                                          
                                                              
  
                                                       
                                                              
                                                             
                                                                             
  
                                                                                 
 */
void kbasep_js_policy_runpool_add_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx);

/* 
                                                                   
  
                                                                              
                                        
                                                                       
                                                               
  
                                                       
                                                              
                                                             
                                                                             
  
                                                                                 
 */
void kbasep_js_policy_runpool_remove_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx);

/* 
                                                                        
                             
  
                                                                           
  
                                             
 */
mali_bool kbasep_js_policy_should_remove_ctx(union kbasep_js_policy *js_policy, struct kbase_context *kctx);

/* 
                                                             
  
                                                                           
                                                                              
                                             
  
                                                                                
                                                                             
  
                                                       
                                                              
                                                             
 */
void kbasep_js_policy_runpool_timers_sync(union kbasep_js_policy *js_policy);


/* 
                                                                                         
  
  
                                                      
                                                                      
  
                                                                             
                  
  
                                                                         
                                                                               
                                                                               
                                                                        
 */
mali_bool kbasep_js_policy_ctx_has_priority(union kbasep_js_policy *js_policy, struct kbase_context *current_ctx, struct kbase_context *new_ctx);

	  /*     *//*                               */

/* 
                                                                                 
     
  
                                                                                  
                                                                        
 */

/* 
                                                 
  
                                                                                   
                                                                           
                          
                                                                                  
                                                                           
                                        
                                                                            
                                                        
 */
union kbasep_js_policy_job_info;

/* 
                                                                
  
                                                                                     
                                                                              
              
  
                                                 
                                                   
                                   
  
                                                                                         
                                                                                                 
                                                      
  
                                                                      
                                                    
  
                                                                          
                                                     
  
                                                         
 */
mali_error kbasep_js_policy_init_job(const union kbasep_js_policy *js_policy, const struct kbase_context *kctx, struct kbase_jd_atom *katom);

/* 
                                                                                         
  
                                                                             
                                                                              
                                                                             
                                                                       
  
                                                                     
                                                                          
                                              
  
                                                                    
                                                                               
                    
  
                                                      
                                                        
 */
void kbasep_js_policy_register_job(union kbasep_js_policy *js_policy, struct kbase_context *kctx, struct kbase_jd_atom *katom);

/* 
                                                                                        
  
                                                                             
                                                                               
                                                                            
  
                                                      
                                                        
 */
void kbasep_js_policy_deregister_job(union kbasep_js_policy *js_policy, struct kbase_context *kctx, struct kbase_jd_atom *katom);

/* 
                                                                             
  
                                                                            
                                                                            
                                                    
  
                                                                              
                                                                               
                  
  
                                                                               
                                                                             
                                                                       
                           
  
                                                                    
                     
                                                                               
  
                                                                        
  
                                                      
                                                           
                                                       
                                                        
 */
mali_bool kbasep_js_policy_dequeue_job(struct kbase_device *kbdev, int job_slot_idx, struct kbase_jd_atom ** const katom_ptr);

/* 
                                                                       
  
                                                                            
                                                                              
                                                                   
  
                                                      
                                                                        
                                                       
                                                        
 */
void kbasep_js_policy_enqueue_job(union kbasep_js_policy *js_policy, struct kbase_jd_atom *katom);

/* 
                                                                               
                         
  
                                                                                         
                                                                        
                      
  
                                                                                       
  
                                                                          
                                                
                                                                          
                                                                           
                                                                       
           
                                                                              
                                                                             
           
  
                                                                       
  
                                             
  
                                                      
                                                           
  
                                            
                                         
                                                                                   
 */
void kbasep_js_policy_log_job_result(union kbasep_js_policy *js_policy, struct kbase_jd_atom *katom, u64 time_spent_us);

	  /*     *//*                               */

	  /*     *//*                           */
	  /*     *//*                          */
	  /*     *//*                    */

#endif				/*                     */
