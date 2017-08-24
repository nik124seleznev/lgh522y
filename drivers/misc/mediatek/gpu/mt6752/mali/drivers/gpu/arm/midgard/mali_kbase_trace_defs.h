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

/*
                                                                                      
  
                                                                                              
  
                                                                                                               
  
                                                                                 
  
                                                                           
                                                                         
  
        
                                                       
               
    
                                                                
                                     
                                     
                      
  
                                                                                   
  
  
                                
                                    
  
                  
                                    
                      
  
  
               
                                                              
                                                   
                                      
                                     
                                                                           
 */

#if 0 /*                                            */
int dummy_array[] = {
#endif

/*
              
 */
	/*                                   */
	KBASE_TRACE_CODE_MAKE_CODE(CORE_CTX_DESTROY),
	/*                                   */
	KBASE_TRACE_CODE_MAKE_CODE(CORE_CTX_HWINSTR_TERM),
	/*                                     */
	KBASE_TRACE_CODE_MAKE_CODE(CORE_GPU_IRQ),
	/*                          */
	KBASE_TRACE_CODE_MAKE_CODE(CORE_GPU_IRQ_CLEAR),
	/*                                     */
	KBASE_TRACE_CODE_MAKE_CODE(CORE_GPU_IRQ_DONE),
	KBASE_TRACE_CODE_MAKE_CODE(CORE_GPU_SOFT_RESET),
	KBASE_TRACE_CODE_MAKE_CODE(CORE_GPU_HARD_RESET),
	KBASE_TRACE_CODE_MAKE_CODE(CORE_GPU_PRFCNT_CLEAR),
	/*                        */
	KBASE_TRACE_CODE_MAKE_CODE(CORE_GPU_PRFCNT_SAMPLE),
	KBASE_TRACE_CODE_MAKE_CODE(CORE_GPU_CLEAN_INV_CACHES),
/*
                             
 */
	/*                                */
	KBASE_TRACE_CODE_MAKE_CODE(JM_IRQ),
	/*                          */
	KBASE_TRACE_CODE_MAKE_CODE(JM_IRQ_END),
/*                  
  
                                                                                        
                                                                             
 */
	/*                                              */
	KBASE_TRACE_CODE_MAKE_CODE(JM_JOB_DONE),
	/*                                                                     */
	KBASE_TRACE_CODE_MAKE_CODE(JM_SUBMIT),
	/*                        
                                                                   
                       
                            */
	KBASE_TRACE_CODE_MAKE_CODE(JM_SOFTSTOP),
	KBASE_TRACE_CODE_MAKE_CODE(JM_SOFTSTOP_0),
	KBASE_TRACE_CODE_MAKE_CODE(JM_SOFTSTOP_1),
	/*                        */
	KBASE_TRACE_CODE_MAKE_CODE(JM_HARDSTOP),
	/*                        */
	KBASE_TRACE_CODE_MAKE_CODE(JM_HARDSTOP_0),
	/*                        */
	KBASE_TRACE_CODE_MAKE_CODE(JM_HARDSTOP_1),
	/*                        */
	KBASE_TRACE_CODE_MAKE_CODE(JM_UPDATE_HEAD),
/*                        
                                                       
                          
 */
	/*                        */
	KBASE_TRACE_CODE_MAKE_CODE(JM_CHECK_HEAD),
	KBASE_TRACE_CODE_MAKE_CODE(JM_FLUSH_WORKQS),
	KBASE_TRACE_CODE_MAKE_CODE(JM_FLUSH_WORKQS_DONE),
	/*                          */
	KBASE_TRACE_CODE_MAKE_CODE(JM_ZAP_NON_SCHEDULED),
	/*                          */
	KBASE_TRACE_CODE_MAKE_CODE(JM_ZAP_SCHEDULED),
	KBASE_TRACE_CODE_MAKE_CODE(JM_ZAP_DONE),
	/*                               */
	KBASE_TRACE_CODE_MAKE_CODE(JM_SLOT_SOFT_OR_HARD_STOP),
	/*                                     */
	KBASE_TRACE_CODE_MAKE_CODE(JM_SLOT_EVICT),
	KBASE_TRACE_CODE_MAKE_CODE(JM_SUBMIT_AFTER_RESET),
	KBASE_TRACE_CODE_MAKE_CODE(JM_BEGIN_RESET_WORKER),
	KBASE_TRACE_CODE_MAKE_CODE(JM_END_RESET_WORKER),
/*
                      
 */
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JD_DONE),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JD_DONE_WORKER),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JD_DONE_WORKER_END),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JD_DONE_TRY_RUN_NEXT_JOB),
	/*                                    */
	KBASE_TRACE_CODE_MAKE_CODE(JD_ZAP_CONTEXT),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JD_CANCEL),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JD_CANCEL_WORKER),
/*
                        
 */
	KBASE_TRACE_CODE_MAKE_CODE(JS_RETAIN_CTX_NOLOCK),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JS_ADD_JOB),
	/*                                                          */
	KBASE_TRACE_CODE_MAKE_CODE(JS_REMOVE_JOB),
	KBASE_TRACE_CODE_MAKE_CODE(JS_RETAIN_CTX),
	KBASE_TRACE_CODE_MAKE_CODE(JS_RELEASE_CTX),
	KBASE_TRACE_CODE_MAKE_CODE(JS_TRY_SCHEDULE_HEAD_CTX),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JS_JOB_DONE_TRY_RUN_NEXT_JOB),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JS_JOB_DONE_RETRY_NEEDED),
	/*                                                            */
	KBASE_TRACE_CODE_MAKE_CODE(JS_FAST_START_EVICTS_CTX),
	KBASE_TRACE_CODE_MAKE_CODE(JS_AFFINITY_SUBMIT_TO_BLOCKED),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JS_AFFINITY_CURRENT),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JS_CORE_REF_REQUEST_CORES_FAILED),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JS_CORE_REF_REGISTER_INUSE_FAILED),
	/*                                                 */
	KBASE_TRACE_CODE_MAKE_CODE(JS_CORE_REF_REQUEST_ON_RECHECK_FAILED),
	/*                                                 */
	KBASE_TRACE_CODE_MAKE_CODE(JS_CORE_REF_REGISTER_ON_RECHECK_FAILED),
	/*                                       */
	KBASE_TRACE_CODE_MAKE_CODE(JS_CORE_REF_AFFINITY_WOULD_VIOLATE),
	/*                                          */
	KBASE_TRACE_CODE_MAKE_CODE(JS_CTX_ATTR_NOW_ON_CTX),
	/*                                              */
	KBASE_TRACE_CODE_MAKE_CODE(JS_CTX_ATTR_NOW_ON_RUNPOOL),
	/*                                           */
	KBASE_TRACE_CODE_MAKE_CODE(JS_CTX_ATTR_NOW_OFF_CTX),
	/*                                               */
	KBASE_TRACE_CODE_MAKE_CODE(JS_CTX_ATTR_NOW_OFF_RUNPOOL),
/*
                          
 */
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_INIT_CTX),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_TERM_CTX),
	/*                                    */
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_TRY_EVICT_CTX),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_FOREACH_CTX_JOBS),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_ENQUEUE_CTX),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_DEQUEUE_HEAD_CTX),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_RUNPOOL_ADD_CTX),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_RUNPOOL_REMOVE_CTX),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_DEQUEUE_JOB),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_DEQUEUE_JOB_IRQ),
	/*                                                */
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_ENQUEUE_JOB),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_TIMER_START),
	KBASE_TRACE_CODE_MAKE_CODE(JS_POLICY_TIMER_END),
/*
                          
 */
	KBASE_TRACE_CODE_MAKE_CODE(PM_JOB_SUBMIT_AFTER_POWERING_UP),
	KBASE_TRACE_CODE_MAKE_CODE(PM_JOB_SUBMIT_AFTER_POWERED_UP),
	KBASE_TRACE_CODE_MAKE_CODE(PM_PWRON),
	KBASE_TRACE_CODE_MAKE_CODE(PM_PWRON_TILER),
	KBASE_TRACE_CODE_MAKE_CODE(PM_PWRON_L2),
	KBASE_TRACE_CODE_MAKE_CODE(PM_PWROFF),
	KBASE_TRACE_CODE_MAKE_CODE(PM_PWROFF_TILER),
	KBASE_TRACE_CODE_MAKE_CODE(PM_PWROFF_L2),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CORES_POWERED),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CORES_POWERED_TILER),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CORES_POWERED_L2),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CORES_CHANGE_DESIRED),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CORES_CHANGE_DESIRED_TILER),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CORES_CHANGE_AVAILABLE),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CORES_CHANGE_AVAILABLE_TILER),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CORES_AVAILABLE),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CORES_AVAILABLE_TILER),
	/*                                                         */
	KBASE_TRACE_CODE_MAKE_CODE(PM_DESIRED_REACHED),
	KBASE_TRACE_CODE_MAKE_CODE(PM_DESIRED_REACHED_TILER),
	KBASE_TRACE_CODE_MAKE_CODE(PM_REGISTER_CHANGE_SHADER_INUSE),
	KBASE_TRACE_CODE_MAKE_CODE(PM_REGISTER_CHANGE_TILER_INUSE),
	KBASE_TRACE_CODE_MAKE_CODE(PM_REGISTER_CHANGE_SHADER_NEEDED),
	KBASE_TRACE_CODE_MAKE_CODE(PM_REGISTER_CHANGE_TILER_NEEDED),
	KBASE_TRACE_CODE_MAKE_CODE(PM_RELEASE_CHANGE_SHADER_INUSE),
	KBASE_TRACE_CODE_MAKE_CODE(PM_RELEASE_CHANGE_TILER_INUSE),
	KBASE_TRACE_CODE_MAKE_CODE(PM_UNREQUEST_CHANGE_SHADER_NEEDED),
	KBASE_TRACE_CODE_MAKE_CODE(PM_UNREQUEST_CHANGE_TILER_NEEDED),
	KBASE_TRACE_CODE_MAKE_CODE(PM_REQUEST_CHANGE_SHADER_NEEDED),
	KBASE_TRACE_CODE_MAKE_CODE(PM_REQUEST_CHANGE_TILER_NEEDED),
	KBASE_TRACE_CODE_MAKE_CODE(PM_WAKE_WAITERS),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CONTEXT_ACTIVE),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CONTEXT_IDLE),
	KBASE_TRACE_CODE_MAKE_CODE(PM_GPU_ON),
	KBASE_TRACE_CODE_MAKE_CODE(PM_GPU_OFF),
	/*                                                         */
	KBASE_TRACE_CODE_MAKE_CODE(PM_SET_POLICY),
	KBASE_TRACE_CODE_MAKE_CODE(PM_CA_SET_POLICY),
	/*                           */
	KBASE_TRACE_CODE_MAKE_CODE(PM_CURRENT_POLICY_INIT),
	/*                           */
	KBASE_TRACE_CODE_MAKE_CODE(PM_CURRENT_POLICY_TERM),
/*                                                                   
                                         */
	KBASE_TRACE_CODE_MAKE_CODE(DUMMY)

#if 0 /*                                            */
};
#endif

/*                                                      */
