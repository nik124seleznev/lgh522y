/*
 * Performance events:
 *
 *    Copyright (C) 2008-2009, Thomas Gleixner <tglx@linutronix.de>
 *    Copyright (C) 2008-2011, Red Hat, Inc., Ingo Molnar
 *    Copyright (C) 2008-2011, Red Hat, Inc., Peter Zijlstra
 *
 * Data type definitions, declarations, prototypes.
 *
 *    Started by: Thomas Gleixner and Ingo Molnar
 *
 * For licencing details see kernel-base/COPYING
 */
#ifndef _LINUX_PERF_EVENT_H
#define _LINUX_PERF_EVENT_H

#include <uapi/linux/perf_event.h>

/*
                                              
 */

#ifdef CONFIG_PERF_EVENTS
# include <asm/perf_event.h>
# include <asm/local64.h>
#endif

struct perf_guest_info_callbacks {
	int				(*is_in_guest)(void);
	int				(*is_user_mode)(void);
	unsigned long			(*get_guest_ip)(void);
};

#ifdef CONFIG_HAVE_HW_BREAKPOINT
#include <asm/hw_breakpoint.h>
#endif

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <linux/spinlock.h>
#include <linux/hrtimer.h>
#include <linux/fs.h>
#include <linux/pid_namespace.h>
#include <linux/workqueue.h>
#include <linux/ftrace.h>
#include <linux/cpu.h>
#include <linux/irq_work.h>
#include <linux/static_key.h>
#include <linux/atomic.h>
#include <linux/sysfs.h>
#include <linux/perf_regs.h>
#include <asm/local.h>

struct perf_callchain_entry {
	__u64				nr;
	__u64				ip[PERF_MAX_STACK_DEPTH];
};

struct perf_raw_record {
	u32				size;
	void				*data;
};

/*
                                     
  
                                                                  
                           
                                            
                                         
  
                                                         
                                            
 */
struct perf_branch_entry {
	__u64	from;
	__u64	to;
	__u64	mispred:1,  /*                     */
		predicted:1,/*                  */
		reserved:62;
};

/*
                       
                                                    
  
                                              
                                                  
                                                      
                 
 */
struct perf_branch_stack {
	__u64				nr;
	struct perf_branch_entry	entries[0];
};

struct perf_regs_user {
	__u64		abi;
	struct pt_regs	*regs;
};

struct task_struct;

/*
                                              
 */
struct hw_perf_event_extra {
	u64		config;	/*                */
	unsigned int	reg;	/*                           */
	int		alloc;	/*                                  */
	int		idx;	/*                              */
};

/* 
                                                             
 */
struct hw_perf_event {
#ifdef CONFIG_PERF_EVENTS
	union {
		struct { /*          */
			u64		config;
			u64		last_tag;
			unsigned long	config_base;
			unsigned long	event_base;
			int		event_base_rdpmc;
			int		idx;
			int		last_cpu;
			int		flags;

			struct hw_perf_event_extra extra_reg;
			struct hw_perf_event_extra branch_reg;
		};
		struct { /*          */
			struct hrtimer	hrtimer;
		};
		struct { /*            */
			struct task_struct	*tp_target;
			/*                     */
			struct list_head	tp_list;
		};
#ifdef CONFIG_HAVE_HW_BREAKPOINT
		struct { /*            */
			/*
                                              
                                            
                                       
    */
			struct task_struct		*bp_target;
			struct arch_hw_breakpoint	info;
			struct list_head		bp_list;
		};
#endif
	};
	int				state;
	local64_t			prev_count;
	u64				sample_period;
	u64				last_period;
	local64_t			period_left;
	u64                             interrupts_seq;
	u64				interrupts;

	u64				freq_time_stamp;
	u64				freq_count_stamp;
#endif
};

/*
                             
 */
#define PERF_HES_STOPPED	0x01 /*                        */
#define PERF_HES_UPTODATE	0x02 /*                         */
#define PERF_HES_ARCH		0x04

struct perf_event;

/*
                                                                 
 */
#define PERF_EVENT_TXN 0x1

/* 
                                                   
 */
struct pmu {
	struct list_head		entry;

	struct device			*dev;
	const struct attribute_group	**attr_groups;
	char				*name;
	int				type;

	int * __percpu			pmu_disable_count;
	struct perf_cpu_context * __percpu pmu_cpu_context;
	int				task_ctx_nr;

	/*
                                                                      
                                                  
  */
	void (*pmu_enable)		(struct pmu *pmu); /*          */
	void (*pmu_disable)		(struct pmu *pmu); /*          */

	/*
                                              
                                                                 
  */
	int (*event_init)		(struct perf_event *event);

#define PERF_EF_START	0x01		/*                                  */
#define PERF_EF_RELOAD	0x02		/*                                  */
#define PERF_EF_UPDATE	0x04		/*                                  */

	/*
                                                              
                                             
  */
	int  (*add)			(struct perf_event *event, int flags);
	void (*del)			(struct perf_event *event, int flags);

	/*
                                                              
                                                              
                                           
  */
	void (*start)			(struct perf_event *event, int flags);
	void (*stop)			(struct perf_event *event, int flags);

	/*
                                           
  */
	void (*read)			(struct perf_event *event);

	/*
                                                            
                                                                
                                                
   
                                                             
                            
  */
	void (*start_txn)		(struct pmu *pmu); /*          */
	/*
                                                             
                                                              
                                                               
                                        
  */
	int  (*commit_txn)		(struct pmu *pmu); /*          */
	/*
                                                          
                                                       
  */
	void (*cancel_txn)		(struct pmu *pmu); /*          */

	/*
                                                                         
                                                                           
  */
	int (*event_idx)		(struct perf_event *event); /*         */

	/*
                                                                    
  */
	void (*flush_branch_stack)	(void);
};

/* 
                                                       
 */
enum perf_event_active_state {
	PERF_EVENT_STATE_ERROR		= -2,
	PERF_EVENT_STATE_OFF		= -1,
	PERF_EVENT_STATE_INACTIVE	=  0,
	PERF_EVENT_STATE_ACTIVE		=  1,
};

struct file;
struct perf_sample_data;

typedef void (*perf_overflow_handler_t)(struct perf_event *,
					struct perf_sample_data *,
					struct pt_regs *regs);

enum perf_group_flag {
	PERF_GROUP_SOFTWARE		= 0x1,
};

#define SWEVENT_HLIST_BITS		8
#define SWEVENT_HLIST_SIZE		(1 << SWEVENT_HLIST_BITS)

struct swevent_hlist {
	struct hlist_head		heads[SWEVENT_HLIST_SIZE];
	struct rcu_head			rcu_head;
};

#define PERF_ATTACH_CONTEXT	0x01
#define PERF_ATTACH_GROUP	0x02
#define PERF_ATTACH_TASK	0x04

struct perf_cgroup;
struct ring_buffer;

/* 
                                                               
 */
struct perf_event {
#ifdef CONFIG_PERF_EVENTS
	struct list_head		group_entry;
	struct list_head		event_entry;
	struct list_head		sibling_list;
	struct hlist_node		hlist_entry;
	int				nr_siblings;
	int				group_flags;
	struct perf_event		*group_leader;
	struct pmu			*pmu;

	enum perf_event_active_state	state;
	unsigned int			attach_state;
	local64_t			count;
	atomic64_t			child_count;

	/*
                                                          
                                                            
                                                   
                                                       
   
                                                             
                                                                 
  */
	u64				total_time_enabled;
	u64				total_time_running;

	/*
                                                              
                                                           
                                                                 
            
                                                                
                                                                     
                                                                 
                            
  */
	u64				tstamp_enabled;
	u64				tstamp_running;
	u64				tstamp_stopped;

	/*
                                                          
                                                            
                                                                
   
                                                              
                                                           
  */
	u64				shadow_ctx_time;

	struct perf_event_attr		attr;
	u16				header_size;
	u16				id_header_size;
	u16				read_size;
	struct hw_perf_event		hw;

	struct perf_event_context	*ctx;
	atomic_long_t			refcount;

	/*
                                                              
                                                       
  */
	atomic64_t			child_total_time_enabled;
	atomic64_t			child_total_time_running;

	/*
                                         
  */
	struct mutex			child_mutex;
	struct list_head		child_list;
	struct perf_event		*parent;

	int				oncpu;
	int				cpu;

	struct list_head		owner_entry;
	struct task_struct		*owner;

	/*           */
	struct mutex			mmap_mutex;
	atomic_t			mmap_count;

	struct ring_buffer		*rb;
	struct list_head		rb_entry;

	/*              */
	wait_queue_head_t		waitq;
	struct fasync_struct		*fasync;

	/*                                */
	int				pending_wakeup;
	int				pending_kill;
	int				pending_disable;
	struct irq_work			pending;

	atomic_t			event_limit;

	void (*destroy)(struct perf_event *);
	struct rcu_head			rcu_head;

	struct pid_namespace		*ns;
	u64				id;

	perf_overflow_handler_t		overflow_handler;
	void				*overflow_handler_context;

#ifdef CONFIG_EVENT_TRACING
	struct ftrace_event_call	*tp_event;
	struct event_filter		*filter;
#ifdef CONFIG_FUNCTION_TRACER
	struct ftrace_ops               ftrace_ops;
#endif
#endif

#ifdef CONFIG_CGROUP_PERF
	struct perf_cgroup		*cgrp; /*                           */
	int				cgrp_defer_enabled;
#endif

#endif /*                    */
};

enum perf_event_context_type {
	task_context,
	cpu_context,
};

/* 
                                                      
  
                                                              
 */
struct perf_event_context {
	struct pmu			*pmu;
	enum perf_event_context_type	type;
	/*
                                                 
                            
  */
	raw_spinlock_t			lock;
	/*
                                                             
                                                              
                                                              
  */
	struct mutex			mutex;

	struct list_head		pinned_groups;
	struct list_head		flexible_groups;
	struct list_head		event_list;
	int				nr_events;
	int				nr_active;
	int				is_active;
	int				nr_stat;
	int				nr_freq;
	int				rotate_disable;
	atomic_t			refcount;
	struct task_struct		*task;

	/*
                                             
  */
	u64				time;
	u64				timestamp;

	/*
                                                          
                                                   
  */
	struct perf_event_context	*parent_ctx;
	u64				parent_gen;
	u64				generation;
	int				pin_count;
	int				nr_cgroups;	 /*             */
	int				nr_branch_stack; /*                  */
	struct rcu_head			rcu_head;
};

/*
                                                 
                               
 */
#define PERF_NR_CONTEXTS	4

/* 
                                                                  
 */
struct perf_cpu_context {
	struct perf_event_context	ctx;
	struct perf_event_context	*task_ctx;
	int				active_oncpu;
	int				exclusive;
	struct list_head		rotation_list;
	int				jiffies_interval;
	struct pmu			*unique_pmu;
	struct perf_cgroup		*cgrp;
};

struct perf_output_handle {
	struct perf_event		*event;
	struct ring_buffer		*rb;
	unsigned long			wakeup;
	unsigned long			size;
	void				*addr;
	int				page;
};

#ifdef CONFIG_PERF_EVENTS

extern int perf_pmu_register(struct pmu *pmu, char *name, int type);
extern void perf_pmu_unregister(struct pmu *pmu);

extern int perf_num_counters(void);
extern const char *perf_pmu_name(void);
extern void __perf_event_task_sched_in(struct task_struct *prev,
				       struct task_struct *task);
extern void __perf_event_task_sched_out(struct task_struct *prev,
					struct task_struct *next);
extern int perf_event_init_task(struct task_struct *child);
extern void perf_event_exit_task(struct task_struct *child);
extern void perf_event_free_task(struct task_struct *task);
extern void perf_event_delayed_put(struct task_struct *task);
extern void perf_event_print_debug(void);
extern void perf_pmu_disable(struct pmu *pmu);
extern void perf_pmu_enable(struct pmu *pmu);
extern int perf_event_task_disable(void);
extern int perf_event_task_enable(void);
extern int perf_event_refresh(struct perf_event *event, int refresh);
extern void perf_event_update_userpage(struct perf_event *event);
extern int perf_event_release_kernel(struct perf_event *event);
extern struct perf_event *
perf_event_create_kernel_counter(struct perf_event_attr *attr,
				int cpu,
				struct task_struct *task,
				perf_overflow_handler_t callback,
				void *context);
extern void perf_pmu_migrate_context(struct pmu *pmu,
				int src_cpu, int dst_cpu);
extern u64 perf_event_read_value(struct perf_event *event,
				 u64 *enabled, u64 *running);


struct perf_sample_data {
	u64				type;

	u64				ip;
	struct {
		u32	pid;
		u32	tid;
	}				tid_entry;
	u64				time;
	u64				addr;
	u64				id;
	u64				stream_id;
	struct {
		u32	cpu;
		u32	reserved;
	}				cpu_entry;
	u64				period;
	union  perf_mem_data_src	data_src;
	struct perf_callchain_entry	*callchain;
	struct perf_raw_record		*raw;
	struct perf_branch_stack	*br_stack;
	struct perf_regs_user		regs_user;
	u64				stack_user_size;
	u64				weight;
};

static inline void perf_sample_data_init(struct perf_sample_data *data,
					 u64 addr, u64 period)
{
	/*                                                               */
	data->addr = addr;
	data->raw  = NULL;
	data->br_stack = NULL;
	data->period = period;
	data->regs_user.abi = PERF_SAMPLE_REGS_ABI_NONE;
	data->regs_user.regs = NULL;
	data->stack_user_size = 0;
	data->weight = 0;
	data->data_src.val = 0;
}

extern void perf_output_sample(struct perf_output_handle *handle,
			       struct perf_event_header *header,
			       struct perf_sample_data *data,
			       struct perf_event *event);
extern void perf_prepare_sample(struct perf_event_header *header,
				struct perf_sample_data *data,
				struct perf_event *event,
				struct pt_regs *regs);

extern int perf_event_overflow(struct perf_event *event,
				 struct perf_sample_data *data,
				 struct pt_regs *regs);

static inline bool is_sampling_event(struct perf_event *event)
{
	return event->attr.sample_period != 0;
}

/*
                                                        
 */
static inline int is_software_event(struct perf_event *event)
{
	return event->pmu->task_ctx_nr == perf_sw_context;
}

extern struct static_key perf_swevent_enabled[PERF_COUNT_SW_MAX];

extern void __perf_sw_event(u32, u64, struct pt_regs *, u64);

#ifndef perf_arch_fetch_caller_regs
static inline void perf_arch_fetch_caller_regs(struct pt_regs *regs, unsigned long ip) { }
#endif

/*
                                                            
                                                  
                          
                             
                      
                                              
 */
static inline void perf_fetch_caller_regs(struct pt_regs *regs)
{
	memset(regs, 0, sizeof(*regs));

	perf_arch_fetch_caller_regs(regs, CALLER_ADDR0);
}

static __always_inline void
perf_sw_event(u32 event_id, u64 nr, struct pt_regs *regs, u64 addr)
{
	struct pt_regs hot_regs;

	if (static_key_false(&perf_swevent_enabled[event_id])) {
		if (!regs) {
			perf_fetch_caller_regs(&hot_regs);
			regs = &hot_regs;
		}
		__perf_sw_event(event_id, nr, regs, addr);
	}
}

extern struct static_key_deferred perf_sched_events;

static inline void perf_event_task_sched_in(struct task_struct *prev,
					    struct task_struct *task)
{
	if (static_key_false(&perf_sched_events.key))
		__perf_event_task_sched_in(prev, task);
}

static inline void perf_event_task_sched_out(struct task_struct *prev,
					     struct task_struct *next)
{
	perf_sw_event(PERF_COUNT_SW_CONTEXT_SWITCHES, 1, NULL, 0);

	if (static_key_false(&perf_sched_events.key))
		__perf_event_task_sched_out(prev, next);
}

extern void perf_event_mmap(struct vm_area_struct *vma);
extern struct perf_guest_info_callbacks *perf_guest_cbs;
extern int perf_register_guest_info_callbacks(struct perf_guest_info_callbacks *callbacks);
extern int perf_unregister_guest_info_callbacks(struct perf_guest_info_callbacks *callbacks);

extern void perf_event_comm(struct task_struct *tsk);
extern void perf_event_fork(struct task_struct *tsk);

/*            */
DECLARE_PER_CPU(struct perf_callchain_entry, perf_callchain_entry);

extern void perf_callchain_user(struct perf_callchain_entry *entry, struct pt_regs *regs);
extern void perf_callchain_kernel(struct perf_callchain_entry *entry, struct pt_regs *regs);

static inline void perf_callchain_store(struct perf_callchain_entry *entry, u64 ip)
{
	if (entry->nr < PERF_MAX_STACK_DEPTH)
		entry->ip[entry->nr++] = ip;
}

extern int sysctl_perf_event_paranoid;
extern int sysctl_perf_event_mlock;
extern int sysctl_perf_event_sample_rate;
extern int sysctl_perf_cpu_time_max_percent;

extern void perf_sample_event_took(u64 sample_len_ns);

extern int perf_proc_update_handler(struct ctl_table *table, int write,
		void __user *buffer, size_t *lenp,
		loff_t *ppos);
extern int perf_cpu_time_max_percent_handler(struct ctl_table *table, int write,
		void __user *buffer, size_t *lenp,
		loff_t *ppos);


static inline bool perf_paranoid_tracepoint_raw(void)
{
	return sysctl_perf_event_paranoid > -1;
}

static inline bool perf_paranoid_cpu(void)
{
	return sysctl_perf_event_paranoid > 0;
}

static inline bool perf_paranoid_kernel(void)
{
	return sysctl_perf_event_paranoid > 1;
}

extern void perf_event_init(void);
extern void perf_tp_event(u64 addr, u64 count, void *record,
			  int entry_size, struct pt_regs *regs,
			  struct hlist_head *head, int rctx,
			  struct task_struct *task);
extern void perf_bp_event(struct perf_event *event, void *data);

#ifndef perf_misc_flags
# define perf_misc_flags(regs) \
		(user_mode(regs) ? PERF_RECORD_MISC_USER : PERF_RECORD_MISC_KERNEL)
# define perf_instruction_pointer(regs)	instruction_pointer(regs)
#endif

static inline bool has_branch_stack(struct perf_event *event)
{
	return event->attr.sample_type & PERF_SAMPLE_BRANCH_STACK;
}

extern int perf_output_begin(struct perf_output_handle *handle,
			     struct perf_event *event, unsigned int size);
extern void perf_output_end(struct perf_output_handle *handle);
extern unsigned int perf_output_copy(struct perf_output_handle *handle,
			     const void *buf, unsigned int len);
extern unsigned int perf_output_skip(struct perf_output_handle *handle,
				     unsigned int len);
extern int perf_swevent_get_recursion_context(void);
extern void perf_swevent_put_recursion_context(int rctx);
extern void perf_event_enable(struct perf_event *event);
extern void perf_event_disable(struct perf_event *event);
extern int __perf_event_disable(void *info);
extern void perf_event_task_tick(void);
#else
static inline void
perf_event_task_sched_in(struct task_struct *prev,
			 struct task_struct *task)			{ }
static inline void
perf_event_task_sched_out(struct task_struct *prev,
			  struct task_struct *next)			{ }
static inline int perf_event_init_task(struct task_struct *child)	{ return 0; }
static inline void perf_event_exit_task(struct task_struct *child)	{ }
static inline void perf_event_free_task(struct task_struct *task)	{ }
static inline void perf_event_delayed_put(struct task_struct *task)	{ }
static inline void perf_event_print_debug(void)				{ }
static inline int perf_event_task_disable(void)				{ return -EINVAL; }
static inline int perf_event_task_enable(void)				{ return -EINVAL; }
static inline int perf_event_refresh(struct perf_event *event, int refresh)
{
	return -EINVAL;
}

static inline void
perf_sw_event(u32 event_id, u64 nr, struct pt_regs *regs, u64 addr)	{ }
static inline void
perf_bp_event(struct perf_event *event, void *data)			{ }

static inline int perf_register_guest_info_callbacks
(struct perf_guest_info_callbacks *callbacks)				{ return 0; }
static inline int perf_unregister_guest_info_callbacks
(struct perf_guest_info_callbacks *callbacks)				{ return 0; }

static inline void perf_event_mmap(struct vm_area_struct *vma)		{ }
static inline void perf_event_comm(struct task_struct *tsk)		{ }
static inline void perf_event_fork(struct task_struct *tsk)		{ }
static inline void perf_event_init(void)				{ }
static inline int  perf_swevent_get_recursion_context(void)		{ return -1; }
static inline void perf_swevent_put_recursion_context(int rctx)		{ }
static inline void perf_event_enable(struct perf_event *event)		{ }
static inline void perf_event_disable(struct perf_event *event)		{ }
static inline int __perf_event_disable(void *info)			{ return -1; }
static inline void perf_event_task_tick(void)				{ }
#endif

#if defined(CONFIG_PERF_EVENTS) && defined(CONFIG_NO_HZ_FULL)
extern bool perf_event_can_stop_tick(void);
#else
static inline bool perf_event_can_stop_tick(void)			{ return true; }
#endif

#if defined(CONFIG_PERF_EVENTS) && defined(CONFIG_CPU_SUP_INTEL)
extern void perf_restore_debug_store(void);
#else
static inline void perf_restore_debug_store(void)			{ }
#endif

#define perf_output_put(handle, x) perf_output_copy((handle), &(x), sizeof(x))

/*
                                                                         
 */
#define perf_cpu_notifier(fn)						\
do {									\
	static struct notifier_block fn##_nb __cpuinitdata =		\
		{ .notifier_call = fn, .priority = CPU_PRI_PERF };	\
	unsigned long cpu = smp_processor_id();				\
	unsigned long flags;						\
	fn(&fn##_nb, (unsigned long)CPU_UP_PREPARE,			\
		(void *)(unsigned long)cpu);				\
	local_irq_save(flags);						\
	fn(&fn##_nb, (unsigned long)CPU_STARTING,			\
		(void *)(unsigned long)cpu);				\
	local_irq_restore(flags);					\
	fn(&fn##_nb, (unsigned long)CPU_ONLINE,				\
		(void *)(unsigned long)cpu);				\
	register_cpu_notifier(&fn##_nb);				\
} while (0)


struct perf_pmu_events_attr {
	struct device_attribute attr;
	u64 id;
	const char *event_str;
};

#define PMU_EVENT_ATTR(_name, _var, _id, _show)				\
static struct perf_pmu_events_attr _var = {				\
	.attr = __ATTR(_name, 0444, _show, NULL),			\
	.id   =  _id,							\
};

#define PMU_FORMAT_ATTR(_name, _format)					\
static ssize_t								\
_name##_show(struct device *dev,					\
			       struct device_attribute *attr,		\
			       char *page)				\
{									\
	BUILD_BUG_ON(sizeof(_format) >= PAGE_SIZE);			\
	return sprintf(page, _format "\n");				\
}									\
									\
static struct device_attribute format_attr_##_name = __ATTR_RO(_name)

#endif /*                     */
