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

#ifndef _KBASE_JS_POLICY_CFS_H_
#define _KBASE_JS_POLICY_CFS_H_

#define KBASE_JS_POLICY_AVAILABLE_CFS

/*                      
      */
/*                            
      */
/*                             
      */

/* 
                                                                             
                                                                          
                       
  
                                                      
 */
#define KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS_LOG2 3
#define KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS      (1u << KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS_LOG2)

/*                                                 */
#define KBASEP_JS_VARIANT_LOOKUP_BITS_NEEDED (BASE_JM_MAX_NR_SLOTS * KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS)
/*                                                                 */
#define KBASEP_JS_VARIANT_LOOKUP_WORDS_NEEDED ((KBASEP_JS_VARIANT_LOOKUP_BITS_NEEDED + 31) / 32)

#define KBASEP_JS_RUNTIME_EMPTY 	((u64)-1)

typedef struct kbasep_js_policy_cfs {
	/*                                                 
                                                         */
	struct list_head ctx_queue_head;

	/*                                                                */
	struct list_head ctx_rt_queue_head;

	/*                                                                           
                                          */
	struct list_head scheduled_ctxs_head;

	/*                                                                   
                                                 */
	u32 num_core_req_variants;

	/*                                    */
	struct kbasep_atom_req core_req_variants[KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS];

	/*                                                               */
	u32 slot_to_variant_lookup_ss_state[KBASEP_JS_VARIANT_LOOKUP_WORDS_NEEDED];
	u32 slot_to_variant_lookup_ss_allcore_state[KBASEP_JS_VARIANT_LOOKUP_WORDS_NEEDED];

	/*                                           */
	struct hrtimer scheduling_timer;

	/*                      
   
                                                                                  */
	mali_bool timer_running;

	/*                                                        
   
                                                                            
                                                                           
                                                 */
	u64 head_runtime_us;

	/*                                                                              
                                  */
	atomic64_t least_runtime_us;

	/*                                                                            
                                                                 */
	atomic64_t rt_least_runtime_us;
} kbasep_js_policy_cfs;

/* 
                                                             
 */
typedef struct kbasep_js_policy_cfs_ctx {
	/*                                                                     */
	struct list_head list;

	/*                                                     
                                                                           
                                                                           
                                                                          
                           
   
                                                                          
                                                 
   
                                                                         
                                                                            
                                */
	struct list_head job_list_head[KBASEP_JS_MAX_NR_CORE_REQ_VARIANTS];

	/*                                                
   
                                                                          
                                                                     
                                                                       
                                                                        
   
                                                                              
                                                */
	u64 runtime_us;

	/*                                                                                      
                               */
	mali_bool process_rt_policy;
	/*                               */
	int process_priority;
	/*                                           
                                                                   */
	int bag_priority;
	/*                                        
                                                                   */
	int bag_total_priority;
	/*                                 
                                                                   */
	int bag_total_nr_atoms;

} kbasep_js_policy_cfs_ctx;

/* 
                                                                           
 */
typedef struct kbasep_js_policy_cfs_job {
	struct list_head list;	    /*                                                             */
	u32 cached_variant_idx;	  /*                                                                    */

	/*                                                      
   
                                                                              */
	u32 ticks;
} kbasep_js_policy_cfs_job;

	  /*     *//*                           */
	  /*     *//*                          */
	  /*     *//*                    */

#endif
