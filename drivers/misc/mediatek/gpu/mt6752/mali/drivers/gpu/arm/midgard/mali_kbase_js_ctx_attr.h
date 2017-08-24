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

#ifndef _KBASE_JS_CTX_ATTR_H_
#define _KBASE_JS_CTX_ATTR_H_

/* 
                       
     
 */

/* 
                             
     
 */

/* 
                       
     
 */

/* 
                                                                              
  
            
                         
 */
void kbasep_js_ctx_attr_set_initial_attrs(struct kbase_device *kbdev, struct kbase_context *kctx);

/* 
                                     
  
                                                                     
                       
  
            
                
                         
                              
 */
void kbasep_js_ctx_attr_runpool_retain_ctx(struct kbase_device *kbdev, struct kbase_context *kctx);

/* 
                                      
  
                                                                         
                           
  
            
                
                         
                              
  
                                                                               
                                                                      
                                                                                  
                                       
                                                                                 
 */
mali_bool kbasep_js_ctx_attr_runpool_release_ctx(struct kbase_device *kbdev, struct kbase_context *kctx);

/* 
                                   
  
                                             
  
            
                
                                                                             
 */
void kbasep_js_ctx_attr_ctx_retain_atom(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbase_jd_atom *katom);

/* 
                                                               
  
                                                                  
  
            
                
                                                                             
  
                                                           
  
                                                                               
                                                                      
                                                                                  
                                       
                                                                                 
 */
mali_bool kbasep_js_ctx_attr_ctx_release_atom(struct kbase_device *kbdev, struct kbase_context *kctx, struct kbasep_js_atom_retained_state *katom_retained_state);

/* 
            
                         
 */
static INLINE s8 kbasep_js_ctx_attr_count_on_runpool(struct kbase_device *kbdev, enum kbasep_js_ctx_attr attribute)
{
	struct kbasep_js_device_data *js_devdata;

	KBASE_DEBUG_ASSERT(kbdev != NULL);
	KBASE_DEBUG_ASSERT(attribute < KBASEP_JS_CTX_ATTR_COUNT);
	js_devdata = &kbdev->js_data;

	return js_devdata->runpool_irq.ctx_attr_ref_count[attribute];
}

/* 
            
                         
 */
static INLINE mali_bool kbasep_js_ctx_attr_is_attr_on_runpool(struct kbase_device *kbdev, enum kbasep_js_ctx_attr attribute)
{
	/*                                                                                                           */
	return (mali_bool) kbasep_js_ctx_attr_count_on_runpool(kbdev, attribute);
}

/* 
            
                
 */
static INLINE mali_bool kbasep_js_ctx_attr_is_attr_on_ctx(struct kbase_context *kctx, enum kbasep_js_ctx_attr attribute)
{
	struct kbasep_js_kctx_info *js_kctx_info;

	KBASE_DEBUG_ASSERT(kctx != NULL);
	KBASE_DEBUG_ASSERT(attribute < KBASEP_JS_CTX_ATTR_COUNT);
	js_kctx_info = &kctx->jctx.sched_info;

	/*                                                                                       */
	return (mali_bool) (js_kctx_info->ctx.ctx_attr_ref_count[attribute]);
}

	  /*     *//*                    */
	  /*     *//*                          */
	  /*     *//*                    */

#endif				/*                   */
