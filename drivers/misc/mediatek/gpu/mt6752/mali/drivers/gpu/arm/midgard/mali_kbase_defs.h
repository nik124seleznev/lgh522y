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

#ifndef _KBASE_DEFS_H_
#define _KBASE_DEFS_H_

#include <mali_kbase_config.h>
#include <mali_base_hwconfig.h>
#include <mali_kbase_mem_lowlevel.h>
#include <mali_kbase_mem_alloc.h>
#include <mali_kbase_mmu_hw.h>


#include <linux/atomic.h>
#include <linux/mempool.h>
#include <linux/slab.h>

#ifdef CONFIG_KDS
#include <linux/kds.h>
#endif				/*            */

#ifdef CONFIG_SYNC
#include "sync.h"
#endif				/*             */

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#endif				/*                 */

#ifdef CONFIG_PM_DEVFREQ
#include <linux/devfreq.h>
#endif /*                */

/*                             */
#ifdef CONFIG_MALI_MIDGARD_ENABLE_TRACE
#define KBASE_TRACE_ENABLE 1
#endif

#ifndef KBASE_TRACE_ENABLE
#ifdef CONFIG_MALI_DEBUG
#define KBASE_TRACE_ENABLE 1
#else
#define KBASE_TRACE_ENABLE 0
#endif				/*                   */
#endif				/*                    */

/*                                                                        */
#define KBASE_TRACE_DUMP_ON_JOB_SLOT_ERROR 1

/* 
                                                                                                   
                                                                                                                  
                                                                                                                   
                    
 */
#define ZAP_TIMEOUT             1000

/*                                                                     */
#define RESET_TIMEOUT           500

/* 
                                                            
  
                                                                                       
  
                                                               
  
                                                                                                  
  
                                                                        
 */
#define KBASE_DISABLE_SCHEDULING_SOFT_STOPS 0
/* 
                                                            
  
                                                                                       
  
                                                                                                  
  
                                                                        
 */
#define KBASE_DISABLE_SCHEDULING_HARD_STOPS 0

/* 
                                                              
  
                                                                             
                             
 */
#define BASE_JM_MAX_NR_SLOTS        16

/* 
                                                                   
  
                                                                             
                                 
 */
#define BASE_MAX_NR_AS              16

/*     */
#define ENTRY_IS_ATE        1ULL
#define ENTRY_IS_INVAL      2ULL
#define ENTRY_IS_PTE        3ULL

#define MIDGARD_MMU_VA_BITS 48

#define ENTRY_ATTR_BITS (7ULL << 2)	/*          */
#define ENTRY_RD_BIT (1ULL << 6)
#define ENTRY_WR_BIT (1ULL << 7)
#define ENTRY_SHARE_BITS (3ULL << 8)	/*          */
#define ENTRY_ACCESS_BIT (1ULL << 10)
#define ENTRY_NX_BIT (1ULL << 54)

#define ENTRY_FLAGS_MASK (ENTRY_ATTR_BITS | ENTRY_RD_BIT | ENTRY_WR_BIT | ENTRY_SHARE_BITS | ENTRY_ACCESS_BIT | ENTRY_NX_BIT)

#if MIDGARD_MMU_VA_BITS > 39
#define MIDGARD_MMU_TOPLEVEL    0
#else
#define MIDGARD_MMU_TOPLEVEL    1
#endif

#define GROWABLE_FLAGS_REQUIRED (KBASE_REG_PF_GROW)
#define GROWABLE_FLAGS_MASK     (GROWABLE_FLAGS_REQUIRED | KBASE_REG_FREE)

/*                                                              */
#define KBASEP_AS_NR_INVALID     (-1)

#define KBASE_LOCK_REGION_MAX_SIZE (63)
#define KBASE_LOCK_REGION_MIN_SIZE (11)

#define KBASE_TRACE_SIZE_LOG2 8	/*             */
#define KBASE_TRACE_SIZE (1 << KBASE_TRACE_SIZE_LOG2)
#define KBASE_TRACE_MASK ((1 << KBASE_TRACE_SIZE_LOG2)-1)

#include "mali_kbase_js_defs.h"

#define KBASEP_FORCE_REPLAY_DISABLED 0

/*                                                          */
#define KBASEP_FORCE_REPLAY_RANDOM_LIMIT 16

/*                                         */
#define KBASE_KATOM_FLAG_BEEN_SOFT_STOPPPED (1<<1)
/*                                              */
#define KBASE_KATOM_FLAGS_RERUN (1<<2)
#define KBASE_KATOM_FLAGS_JOBCHAIN (1<<3)
/*                                         */
#define KBASE_KATOM_FLAG_BEEN_HARD_STOPPED (1<<4)
/*                                             */
#define KBASE_KATOM_FLAG_IN_DISJOINT (1<<5)

/*                                                  
                                                     */

/*                                       */
#define JS_COMMAND_SW_CAUSES_DISJOINT 0x100

/*                                  */
#define JS_COMMAND_SW_BITS  (JS_COMMAND_SW_CAUSES_DISJOINT)

#if (JS_COMMAND_SW_BITS & JS_COMMAND_MASK)
#error JS_COMMAND_SW_BITS not masked off by JS_COMMAND_MASK. Must update JS_COMMAND_SW_<..> bitmasks
#endif

/*                                                                      
                                           */
#define JS_COMMAND_SOFT_STOP_WITH_SW_DISJOINT \
		(JS_COMMAND_SW_CAUSES_DISJOINT | JS_COMMAND_SOFT_STOP)


struct kbase_jd_atom_dependency
{
	struct kbase_jd_atom *atom;
	u8 dep_type;
};

/* 
                                                                              
                                          
  
                                           
  
                                                
 */
static INLINE const struct kbase_jd_atom* const kbase_jd_katom_dep_atom(const struct kbase_jd_atom_dependency* dep)
{
	LOCAL_ASSERT(dep != NULL);
	
	return (const struct kbase_jd_atom* const )(dep->atom);
}
 
/* 
                                                                                         
                                          
  
                                           
  
                                   
 */
static INLINE const u8 kbase_jd_katom_dep_type(const struct kbase_jd_atom_dependency* dep)
{
	LOCAL_ASSERT(dep != NULL);

	return dep->dep_type;
}

/* 
                                                                
  
                                                  
                                                        
                                                        
  
 */
static INLINE void kbase_jd_katom_dep_set(const struct kbase_jd_atom_dependency* const_dep, 
	struct kbase_jd_atom * a,
	u8 type)
{
	struct kbase_jd_atom_dependency* dep;
	
	LOCAL_ASSERT(const_dep != NULL);

	dep = (REINTERPRET_CAST(struct kbase_jd_atom_dependency* )const_dep);

	dep->atom = a;
	dep->dep_type = type; 
}

/* 
                                                                
  
                                                                
  
 */
static INLINE void kbase_jd_katom_dep_clear(const struct kbase_jd_atom_dependency* const_dep)
{
	struct kbase_jd_atom_dependency* dep;

	LOCAL_ASSERT(const_dep != NULL);

	dep = (REINTERPRET_CAST(struct kbase_jd_atom_dependency* )const_dep);

	dep->atom = NULL;
	dep->dep_type = BASE_JD_DEP_TYPE_INVALID; 
}

struct kbase_ext_res
{
	mali_addr64 gpu_address;
	struct kbase_mem_phy_alloc * alloc;
};

struct kbase_jd_atom {
	struct work_struct work;
	ktime_t start_timestamp;
	u64 time_spent_us; /*                                               */

	struct base_jd_udata udata;
	struct kbase_context *kctx;

	struct list_head dep_head[2];
	struct list_head dep_item[2];
	const struct kbase_jd_atom_dependency dep[2];

	u16 nr_extres;
	struct kbase_ext_res * extres;

	u32 device_nr;
	u64 affinity;
	u64 jc;
	enum kbase_atom_coreref_state coreref_state;
#ifdef CONFIG_KDS
	struct list_head node;
	struct kds_resource_set *kds_rset;
	mali_bool kds_dep_satisfied;
#endif				/*            */
#ifdef CONFIG_SYNC
	struct sync_fence *fence;
	struct sync_fence_waiter sync_waiter;
#endif				/*             */

	/*                                                                                                       */
	enum base_jd_event_code event_code;
	base_jd_core_req core_req;	    /*                     */
	/*                                                                       
   
                                                                            */
	int retry_submit_on_slot;

	union kbasep_js_policy_job_info sched_info;
	/*                                                          */
	int nice_prio;

	int poking;		/*                    */

	wait_queue_head_t completed;
	enum kbase_jd_atom_state status;
#ifdef CONFIG_GPU_TRACEPOINTS
	int work_id;
#endif
	/*                                                                                                  */
	int slot_nr;

	u32 atom_flags;

	/*                                                                     
  */
	int retry_count;
};

/*
                        
  
                                                                      
  
                                                                                                       
 */

#define KBASE_JD_DEP_QUEUE_SIZE 256

struct kbase_jd_context {
	struct mutex lock;
	struct kbasep_js_kctx_info sched_info;
	struct kbase_jd_atom atoms[BASE_JD_ATOM_COUNT];

	/*                                                                   
                                                                  */
	u32 job_nr;

	/*                                                                  
                                                                      
                                  
   
                                                                       
                                                                          
                                
   
                                                                           
                                                                 
                                                                            
                                                                          
                                                            
                                            
   
                                                             
                                                                            
                                                                         
   
                                         
                               */
	wait_queue_head_t zero_jobs_wait;

	/*                      */
	struct workqueue_struct *job_done_wq;

	spinlock_t tb_lock;
	u32 *tb;
	size_t tb_wrap_offset;

#ifdef CONFIG_KDS
	struct kds_callback kds_cb;
#endif				/*            */
#ifdef CONFIG_GPU_TRACEPOINTS
	atomic_t work_id;
#endif
};

struct kbase_jm_slot {
	/*                                            */
#define BASE_JM_SUBMIT_SLOTS        16
#define BASE_JM_SUBMIT_SLOTS_MASK   (BASE_JM_SUBMIT_SLOTS - 1)

	struct kbase_jd_atom *submitted[BASE_JM_SUBMIT_SLOTS];

	struct kbase_context *last_context;

	u8 submitted_head;
	u8 submitted_nr;
	u8 job_chain_flag;

};

struct kbase_device_info {
	u32 features;
};

/*                                       */
enum {
	KBASE_AS_POKE_STATE_IN_FLIGHT     = 1<<0,
	KBASE_AS_POKE_STATE_KILLING_POKE  = 1<<1
};

/*                                       */
typedef u32 kbase_as_poke_state;

struct kbase_mmu_setup {
	u64	transtab;
	u64	memattr;
};

/* 
                                                                                      
                                                                   
                                                        
  
                                                                             
 */
struct kbase_as {
	int number;

	struct workqueue_struct *pf_wq;
	struct work_struct work_pagefault;
	struct work_struct work_busfault;
	enum kbase_mmu_fault_type fault_type;
	u32 fault_status;
	mali_addr64 fault_addr;
	struct mutex transaction_mutex;

	struct kbase_mmu_setup current_setup;

	/*                     */
	struct workqueue_struct *poke_wq;
	struct work_struct poke_work;
	/*                                                        */
	int poke_refcount;
	/*                                                        */
	kbase_as_poke_state poke_state;
	struct hrtimer poke_timer;
};

static inline int kbase_as_has_bus_fault(struct kbase_as *as)
{
	return as->fault_type == KBASE_MMU_FAULT_TYPE_BUS;
}

static inline int kbase_as_has_page_fault(struct kbase_as *as)
{
	return as->fault_type == KBASE_MMU_FAULT_TYPE_PAGE;
}

/* 
                                       
 */
enum kbase_instr_state {
	/*                                            */
	KBASE_INSTR_STATE_DISABLED = 0,
	/*                                                   */
	KBASE_INSTR_STATE_IDLE,
	/*                                         */
	KBASE_INSTR_STATE_DUMPING,
	/*                                                  */
	KBASE_INSTR_STATE_REQUEST_CLEAN,
	/*                                                          */
	KBASE_INSTR_STATE_CLEANING,
	/*                                                             
                                         */
	KBASE_INSTR_STATE_CLEANED,
	/*                                                                 
                                                                             
                                                          */
	KBASE_INSTR_STATE_RESETTING,
	/*                                                    */
	KBASE_INSTR_STATE_FAULT
};

void kbasep_reset_timeout_worker(struct work_struct *data);
enum hrtimer_restart kbasep_reset_timer_callback(struct hrtimer *data);

struct kbasep_mem_device {
	atomic_t used_pages;   /*                                          
                                          */

};



#define KBASE_TRACE_CODE(X) KBASE_TRACE_CODE_ ## X

enum kbase_trace_code {
	/*                                                               
                                               */
#define KBASE_TRACE_CODE_MAKE_CODE(X) KBASE_TRACE_CODE(X)
#include "mali_kbase_trace_defs.h"
#undef  KBASE_TRACE_CODE_MAKE_CODE
	/*                                      */
	,
	/*                              */
	KBASE_TRACE_CODE_COUNT
};

#define KBASE_TRACE_FLAG_REFCOUNT (((u8)1) << 0)
#define KBASE_TRACE_FLAG_JOBSLOT  (((u8)1) << 1)

struct kbase_trace {
	struct timespec timestamp;
	u32 thread_id;
	u32 cpu;
	void *ctx;
	mali_bool katom;
	int atom_number;
	u64 atom_udata[2];
	u64 gpu_addr;
	unsigned long info_val;
	u8 code;
	u8 jobslot;
	u8 refcount;
	u8 flags;
};

/*                                               
  
                                                                            
                                                                
                                                                      
                                                                     */
enum kbase_timeline_pm_event {
	/*                  */
	KBASEP_TIMELINE_PM_EVENT_FIRST,

	/*                                                                */
	KBASE_TIMELINE_PM_EVENT_RESERVED_0 = KBASEP_TIMELINE_PM_EVENT_FIRST,

	/*                                            
   
                                                                      
  */
	KBASE_TIMELINE_PM_EVENT_GPU_STATE_CHANGED,

	/*                             
   
                                                                      
  */
	KBASE_TIMELINE_PM_EVENT_GPU_ACTIVE,

	/*                           
   
                                                                        
  */
	KBASE_TIMELINE_PM_EVENT_GPU_IDLE,

	/*                                                                 
           */
	KBASE_TIMELINE_PM_EVENT_RESERVED_4,

	/*                                                                  
           */
	KBASE_TIMELINE_PM_EVENT_RESERVED_5,

	/*                                                                 
           */
	KBASE_TIMELINE_PM_EVENT_RESERVED_6,

	/*                                                         
   
                            
                                                     
                                             
  */
	KBASE_TIMELINE_PM_EVENT_CHANGE_GPU_STATE,

	KBASEP_TIMELINE_PM_EVENT_LAST = KBASE_TIMELINE_PM_EVENT_CHANGE_GPU_STATE,
};

#ifdef CONFIG_MALI_TRACE_TIMELINE
struct kbase_trace_kctx_timeline {
	atomic_t jd_atoms_in_flight;
	u32 owner_tgid;
};

struct kbase_trace_kbdev_timeline {
	/*                */
	struct dentry *dentry;

	/*                                                               
                                          
   
                                                                      
               
   
                                                                      
                   */
	u8 slot_atoms_submitted[BASE_JM_SUBMIT_SLOTS];

	/*                            */
	atomic_t pm_event_uid[KBASEP_TIMELINE_PM_EVENT_LAST+1];
	/*                                      */
	atomic_t pm_event_uid_counter;
	/*
                                                                            
                                                     */
	mali_bool l2_transitioning;
};
#endif /*                            */


struct kbasep_kctx_list_element {
	struct list_head link;
	struct kbase_context    *kctx;
};

#define DEVNAME_SIZE	16

struct kbase_device {
	/*                                                                    */
	struct kbase_jm_slot jm_slots[BASE_JM_MAX_NR_SLOTS];
	s8 slot_submit_count_irq[BASE_JM_MAX_NR_SLOTS];

	struct list_head entry;
	struct device *dev;
	struct miscdevice mdev;
	u64 reg_start;
	size_t reg_size;
	void __iomem *reg;
	struct resource *reg_res;
	struct {
		int irq;
		int flags;
	} irqs[3];
#ifdef CONFIG_HAVE_CLK
	struct clk *clock;
#endif
	char devname[DEVNAME_SIZE];

#ifdef CONFIG_MALI_NO_MALI
	void *model;
	struct kmem_cache *irq_slab;
	struct workqueue_struct *irq_workq;
	atomic_t serving_job_irq;
	atomic_t serving_gpu_irq;
	atomic_t serving_mmu_irq;
	spinlock_t reg_op_lock;
#endif				/*                     */

	struct kbase_pm_device_data pm;
	struct kbasep_js_device_data js_data;
	struct kbasep_mem_device memdev;
	struct kbase_as as[BASE_MAX_NR_AS];

	spinlock_t              mmu_mask_change;

	kbase_gpu_props gpu_props;

	/*                                       */
	unsigned long hw_issues_mask[(BASE_HW_ISSUE_END + BITS_PER_LONG - 1) / BITS_PER_LONG];
	/*                             */
	unsigned long hw_features_mask[(BASE_HW_FEATURE_END + BITS_PER_LONG - 1) / BITS_PER_LONG];

	/*                                                                                     */
	u64 shader_present_bitmap;
	u64 tiler_present_bitmap;
	u64 l2_present_bitmap;
	u64 l3_present_bitmap;

	/*                                                           
                                                         
   
                                                                     
   
                                                                      
                                                                          
           */
	u64 shader_inuse_bitmap;

	/*                           */
	u32 shader_inuse_cnt[64];

	/*                                                     */
	u64 shader_needed_bitmap;

	/*                           */
	u32 shader_needed_cnt[64];

	u32 tiler_inuse_cnt;

	u32 tiler_needed_cnt;

	/*                                                     
   
                                                                    
                                                                            
  */
	struct {
		atomic_t count;
		atomic_t state;
	} disjoint_event;

	/*                                                                                                */
	u32 l2_users_count;

	/*                                                                                                       
                                                                                                            
                                                                      
   
                                                                     
  */
	u64 shader_available_bitmap;
	u64 tiler_available_bitmap;
	u64 l2_available_bitmap;

	u64 shader_ready_bitmap;
	u64 shader_transitioning_bitmap;

	s8 nr_hw_address_spaces;			  /*                                                                              */
	s8 nr_user_address_spaces;			  /*                                                       */

	/*                                                            */
	struct {
		/*                                                                     */
		spinlock_t lock;

		struct kbase_context *kctx;
		u64 addr;
		wait_queue_head_t wait;
		int triggered;
		enum kbase_instr_state state;
		wait_queue_head_t   cache_clean_wait;
		struct workqueue_struct *cache_clean_wq;
		struct work_struct  cache_clean_work;

		struct kbase_context *suspended_kctx;
		struct kbase_uk_hwcnt_setup suspended_state;
	} hwcnt;

	/*                                       */
	atomic_t reset_gpu;
#define KBASE_RESET_GPU_NOT_PENDING     0	/*                             */
#define KBASE_RESET_GPU_PREPARED        1	/*                                            */
#define KBASE_RESET_GPU_COMMITTED       2	/*                                                                       
                                   */
#define KBASE_RESET_GPU_HAPPENING       3	/*                                                                    
                                                */

	/*                                                      */
	struct workqueue_struct *reset_workq;
	struct work_struct reset_work;
	wait_queue_head_t reset_wait;
	struct hrtimer reset_timer;

	/*                                                                            */
	atomic_t irq_throttle_cycles;

	const struct kbase_attribute *config_attributes;

#if KBASE_TRACE_ENABLE
	spinlock_t              trace_lock;
	u16                     trace_first_out;
	u16                     trace_next_in;
	struct kbase_trace            *trace_rbuf;
#endif

#if !MALI_CUSTOMER_RELEASE
	/*                                                              
                                           
                                           
                                           
                                           
                                            
                                       
                                       
                                         
   
                                                        
  */
	u32 js_soft_stop_ticks;
	u32 js_soft_stop_ticks_cl;
	u32 js_hard_stop_ticks_ss;
	u32 js_hard_stop_ticks_cl;
	u32 js_hard_stop_ticks_nss;
	u32 js_reset_ticks_ss;
	u32 js_reset_ticks_cl;
	u32 js_reset_ticks_nss;
#endif

	struct mutex cacheclean_lock;

	/*                                                                               */
	void *platform_context;

	/*                                            */
	atomic_t keep_gpu_powered_count;

	/*                                */
	struct list_head        kctx_list;
	struct mutex            kctx_list_lock;

#ifdef CONFIG_MALI_MIDGARD_RT_PM
	struct delayed_work runtime_pm_workqueue;
#endif

#ifdef CONFIG_PM_DEVFREQ
	struct devfreq_dev_profile devfreq_profile;
	struct devfreq *devfreq;
	unsigned long freq;
#ifdef CONFIG_DEVFREQ_THERMAL
	struct devfreq_cooling_device *devfreq_cooling;
#ifdef CONFIG_MALI_POWER_ACTOR
	struct power_actor *power_actor;
#endif
#endif
#endif

#ifdef CONFIG_MALI_TRACE_TIMELINE
	struct kbase_trace_kbdev_timeline timeline;
#endif

#ifdef CONFIG_DEBUG_FS
	/*                               */
	struct dentry *mali_debugfs_directory;
	/*                              */
	struct dentry *gpu_memory_dentry;
	/*                         */
	struct dentry *trace_dentry;
	/*                                             */
	struct dentry *memory_profile_directory;
	/*                                        */
	struct dentry *jd_directory;
#endif /*                 */

	/*                                        */
	u32 kbase_profiling_controls[FBDUMP_CONTROL_MAX];


#if MALI_CUSTOMER_RELEASE == 0
	/*                                                               
                                                                  
              */
	int force_replay_limit;
	/*                                                                  
                                                               
                        */
	int force_replay_count;
	/*                                                                   */
	base_jd_core_req force_replay_core_req;
	/*                                                                 
                                                                       
  */
	mali_bool force_replay_random;
#endif

	/*                                  */
	atomic_t ctx_num;
};

struct kbase_context {
	struct kbase_device *kbdev;
	int id; /*                       */
	phys_addr_t pgd;
	struct list_head event_list;
	struct mutex event_mutex;
	mali_bool event_closed;
	struct workqueue_struct *event_workq;

	u64 mem_attrs;
	bool is_compat;

	atomic_t                setup_complete;
	atomic_t                setup_in_progress;

	mali_bool keep_gpu_powered;

	u64 *mmu_teardown_pages;

	phys_addr_t aliasing_sink_page;

	struct mutex            reg_lock; /*                              */
	struct rb_root          reg_rbtree; /*                                              */

	unsigned long    cookies;
	struct kbase_va_region *pending_regions[BITS_PER_LONG];
	
	wait_queue_head_t event_queue;
	pid_t tgid;
	pid_t pid;

	struct kbase_jd_context jctx;
	atomic_t used_pages;
	atomic_t         nonmapped_pages;

	struct kbase_mem_allocator osalloc;
	struct kbase_mem_allocator * pgd_allocator;

	struct list_head waiting_soft_jobs;
#ifdef CONFIG_KDS
	struct list_head waiting_kds_resource;
#endif
	/*                                                                       
                                                                       
   
                                                                              
         
   
                                                                   
                                                                               
                                                                                                 
                                                                                          
                                    */
	int as_nr;

	/*      
   
                                          
                                                                          
   
                                        */
	spinlock_t         mm_update_lock;
	struct mm_struct * process_mm;

#ifdef CONFIG_MALI_TRACE_TIMELINE
	struct kbase_trace_kctx_timeline timeline;
#endif
#ifdef CONFIG_DEBUG_FS
	/*                                  */
	struct dentry *mem_dentry;
	/*                             */
	char *mem_profile_data;
	/*                             */
	size_t mem_profile_size;
	/*                        */
	spinlock_t mem_profile_lock;
	/*                                   */
	struct dentry *jd_ctx_dir;
#endif /*                 */
};

enum kbase_reg_access_type {
	REG_READ,
	REG_WRITE
};

enum kbase_share_attr_bits {
	/*                             */
	SHARE_BOTH_BITS = (2ULL << 8),	/*                                     */
	SHARE_INNER_BITS = (3ULL << 8)	/*                           */
};

/*                                                          */
#define HR_TIMER_DELAY_MSEC(x) (ns_to_ktime((x)*1000000U))
#define HR_TIMER_DELAY_NSEC(x) (ns_to_ktime(x))

/*                                                                                                   */
#define KBASE_CLEAN_CACHE_MAX_LOOPS     100000
/*                                                                                                         */
#define KBASE_AS_INACTIVE_MAX_LOOPS     100000

/*                                               */
#define BASEP_JD_REPLAY_LIMIT 15

#endif				/*                */
