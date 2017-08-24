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

#ifndef _KBASE_JS_DEFS_H_
#define _KBASE_JS_DEFS_H_

/* 
                       
     
 */

/* 
                             
     
 */

/* 
                       
     
 */
/*               */
struct kbase_device;
struct kbase_jd_atom;


/*                                         */
enum {
	/*                                   */
	KBASE_CTX_FLAG_SUBMIT_DISABLED = (1u << 0),

	/*                                                                           */
	KBASE_CTX_FLAG_PRIVILEGED = (1u << 1),

	/*                                                                                                 */
	KBASE_CTX_FLAG_HINT_ONLY_COMPUTE = (1u << 2)

	    /*                                                                          */
};

typedef u32 kbase_context_flags;

struct kbasep_atom_req {
	base_jd_core_req core_req;
	kbase_context_flags ctx_req;
	u32 device_nr;
};

#include "mali_kbase_js_policy_cfs.h"

/*                                          */
union kbasep_js_policy {
#ifdef KBASE_JS_POLICY_AVAILABLE_FCFS
	struct kbasep_js_policy_fcfs fcfs;
#endif
#ifdef KBASE_JS_POLICY_AVAILABLE_CFS
	struct kbasep_js_policy_cfs cfs;
#endif
};

/*                                          */
union kbasep_js_policy_ctx_info {
#ifdef KBASE_JS_POLICY_AVAILABLE_FCFS
	struct kbasep_js_policy_fcfs_ctx fcfs;
#endif
#ifdef KBASE_JS_POLICY_AVAILABLE_CFS
	struct kbasep_js_policy_cfs_ctx cfs;
#endif
};

/*                                          */
union kbasep_js_policy_job_info {
#ifdef KBASE_JS_POLICY_AVAILABLE_FCFS
	struct kbasep_js_policy_fcfs_job fcfs;
#endif
#ifdef KBASE_JS_POLICY_AVAILABLE_CFS
	struct kbasep_js_policy_cfs_job cfs;
#endif
};


/*                                                                          
             */
typedef void (*kbasep_js_policy_ctx_job_cb)(struct kbase_device *kbdev, struct kbase_jd_atom *katom);

/* 
                                                                           
                          
  
                                                                              
                                                                               
                                                                            
 */
#define KBASE_JS_MAX_JOB_SUBMIT_PER_SLOT_PER_IRQ 2

/* 
                                               
  
                                                                           
  
                                                                       
                                    
 */
#define KBASE_JS_IRQ_THROTTLE_TIME_US 20

/* 
                            
  
                                                                               
                                                              
                                                                           
                                 
                                                                          
                             
  
                                                                      
                                                             
  
                                                                              
                                                                             
                                                                               
             
                                                                        
  
                   
                                                                            
                                                                    
 */
enum kbasep_js_ctx_attr {
	/*                                                                     
                                                                                     
                                 
   
                                                                          
                       
  */
	KBASEP_JS_CTX_ATTR_COMPUTE,

	/*                                                                         
                                                          
                                                     
                                                                  
                                                    
   
                                                                          
                       
  */
	KBASEP_JS_CTX_ATTR_NON_COMPUTE,

	/*                                                                     
                                                                    
   
                                                              
                                                                                                         
                                                                                         
   
                                                                          
                                                                         
                                         
   
                                                                        
                                                                      
                                                                           
                                                                        
                                                
   
                                                       
                                                                         
                                                                           
                            
  */
	KBASEP_JS_CTX_ATTR_COMPUTE_ALL_CORES,

	/*                               */
	KBASEP_JS_CTX_ATTR_COUNT
};

enum {
	/*                                                                             */
	KBASE_JS_ATOM_DONE_START_NEW_ATOMS = (1u << 0),
	/*                                                                      */
	KBASE_JS_ATOM_DONE_EVICTED_FROM_NEXT = (1u << 1)
};

/*                                               */
typedef u32 kbasep_js_atom_done_code;

/* 
                                                                    
  
                                                                            
                                                                      
                                
 */
struct kbasep_js_per_as_data {
	/* 
                                                                       
   
                                                                           
                                                                        
                                                                      
  */
	int as_busy_refcount;

	/*                                                                               */
	struct kbase_context *kctx;
};

/**
 * @brief KBase Device Data Job Scheduler sub-structure
 *
 * This encapsulates the current context of the Job Scheduler on a particular
 * device. This context is global to the device, and is not tied to any
 * particular struct kbase_context running on the device.
 *
 * nr_contexts_running and as_free are optimized for packing together (by making
 * them smaller types than u32). The operations on them should rarely involve
 * masking. The use of signed types for arithmetic indicates to the compiler that
 * the value will not rollover (which would be undefined behavior), and so under
 * the Total License model, it is free to make optimizations based on that (i.e.
 * to remove masking).
 */
struct kbasep_js_device_data {
	/*                                                                            */
	struct runpool_irq {
		/* 
                                                               
    
                                                                    
                                                                        
                                                                        
                                                                       
                   
    
                                               
                           
                             
                                                                       
                                                 
                                                                            
                                                 
   */
		spinlock_t lock;

		/*                                                                                       
                                                                                         
                                                         
    
                                                                                          
                                                      */
		u16 submit_allowed;

		/*                     
                                                                      
                                                                    
    
                                                                      
                                                              
                                                                     
                                                          
    
                                                                    
                                      
                                                                         
                                                                         
                                           */
		s8 ctx_attr_ref_count[KBASEP_JS_CTX_ATTR_COUNT];

		/*                                  */
		struct kbasep_js_per_as_data per_as_data[BASE_MAX_NR_AS];

		/*
                                     
   */
		/*                                                                   
                                                                           */
		u64 slot_affinities[BASE_JM_MAX_NR_SLOTS];
		/*                                                                  
                                                                 */
		u16 slots_blocked_on_affinity;
		/*                                                                 
                                        
    
                                                                    
                                                                       
                                                                      
              */
		s8 slot_affinity_refcount[BASE_JM_MAX_NR_SLOTS][64];
	} runpool_irq;

	/* 
                                                             
                                                                            
                       
   
                                        
                                                   
  */
	struct mutex runpool_mutex;

	/* 
                                                                           
                    
   
                                                               
  */
	struct mutex queue_mutex;

	u16 as_free;				/*                                     */

	/*                                                                                            */
	s8 nr_user_contexts_running;
	/*                                                                                       */
	s8 nr_all_contexts_running;

	/* 
                                
   
                                                                           
                                           
  */
	union kbasep_js_policy policy;

	/*                                                                    
                                                                             */
	base_jd_core_req js_reqs[BASE_JM_MAX_NR_SLOTS];

	u32 scheduling_tick_ns;		 /*                                                     */
	u32 soft_stop_ticks;		 /*                                                  */
	u32 soft_stop_ticks_cl;		 /*                                                     */
	u32 hard_stop_ticks_ss;		 /*                                                     */
	u32 hard_stop_ticks_cl;		 /*                                                     */
	u32 hard_stop_ticks_nss;	 /*                                                      */
	u32 gpu_reset_ticks_ss;		 /*                                                 */
	u32 gpu_reset_ticks_cl;		 /*                                                 */
	u32 gpu_reset_ticks_nss;	 /*                                                  */
	u32 ctx_timeslice_ns;		 /*                                                   */
	u32 cfs_ctx_runtime_init_slices; /*                                                    */
	u32 cfs_ctx_runtime_min_slices;	 /*                                                    */

	/*                              */
	struct list_head suspended_soft_jobs_list;

#ifdef CONFIG_MALI_DEBUG
	/*                                       */
	mali_bool softstop_always;
#endif				/*                   */
	/*                                                                               
                                               
                                                                             */
	int init_status;
};

/* 
                                                            
  
                                                                               
                          
 */
struct kbasep_js_kctx_info {
	/* 
                                                                        
                                                           
   
                                                                            
                                 
   
                                                                                  
                                    
  */
	struct {
		union kbasep_js_policy_ctx_info policy_ctx;	/*                           */
	} runpool;

	/* 
                                                                      
                                                  
                              
                           
                                     
   
                                                                         
                      
   
                                                             
  */
	struct {
		struct mutex jsctx_mutex;		    /*                              */

		/*                                                                               
                                                                           
                  */
		u32 nr_jobs;

		/*                     
                                                                      
                   */
		u32 ctx_attr_ref_count[KBASEP_JS_CTX_ATTR_COUNT];

		kbase_context_flags flags;
		/*                                                          */
		/* 
                                              
    
                                                              
   */
		mali_bool is_scheduled;
		/* 
                                                       
     */
		wait_queue_head_t is_scheduled_wait;

		mali_bool is_dying;			/*                                                   */
	} ctx;

	/*                                                                              
                                               */
	int init_status;
};

/*                                                                             
                                                                               
                                              */
struct kbasep_js_atom_retained_state {
	/*                                                          */
	enum base_jd_event_code event_code;
	/*                    */
	base_jd_core_req core_req;
	/*                                                                        */
	int retry_submit_on_slot;
	/*                                 */
	u32 device_nr;

};

/* 
                                                      
                                                       
                                        
 */
#define KBASEP_JS_RETRY_SUBMIT_SLOT_INVALID (-1)

/* 
                                                                                  
  
                                            
 */
#define KBASEP_JS_ATOM_RETAINED_STATE_CORE_REQ_INVALID BASE_JD_REQ_DEP

/* 
                                                  
  
                                                              
 */
#define KBASEP_JS_TICK_RESOLUTION_US 1

#endif				/*                   */

	  /*     *//*                    */
	  /*     *//*                          */
	  /*     *//*                    */
