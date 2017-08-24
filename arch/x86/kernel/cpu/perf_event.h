/*
 * Performance events x86 architecture header
 *
 *  Copyright (C) 2008 Thomas Gleixner <tglx@linutronix.de>
 *  Copyright (C) 2008-2009 Red Hat, Inc., Ingo Molnar
 *  Copyright (C) 2009 Jaswinder Singh Rajput
 *  Copyright (C) 2009 Advanced Micro Devices, Inc., Robert Richter
 *  Copyright (C) 2008-2009 Red Hat, Inc., Peter Zijlstra <pzijlstr@redhat.com>
 *  Copyright (C) 2009 Intel Corporation, <markus.t.metzger@intel.com>
 *  Copyright (C) 2009 Google, Inc., Stephane Eranian
 *
 *  For licencing details see kernel-base/COPYING
 */

#include <linux/perf_event.h>

#if 0
#undef wrmsrl
#define wrmsrl(msr, val) 						\
do {									\
	unsigned int _msr = (msr);					\
	u64 _val = (val);						\
	trace_printk("wrmsrl(%x, %Lx)\n", (unsigned int)(_msr),		\
			(unsigned long long)(_val));			\
	native_write_msr((_msr), (u32)(_val), (u32)(_val >> 32));	\
} while (0)
#endif

/*
                                           
                                           
                                           
                                           
                                           
                                           
                                           
                                           
  
                                                     
                                                
                       
 */
enum extra_reg_type {
	EXTRA_REG_NONE  = -1,	/*          */

	EXTRA_REG_RSP_0 = 0,	/*                    */
	EXTRA_REG_RSP_1 = 1,	/*                    */
	EXTRA_REG_LBR   = 2,	/*            */
	EXTRA_REG_LDLAT = 3,	/*                  */

	EXTRA_REG_MAX		/*                          */
};

struct event_constraint {
	union {
		unsigned long	idxmsk[BITS_TO_LONGS(X86_PMC_IDX_MAX)];
		u64		idxmsk64;
	};
	u64	code;
	u64	cmask;
	int	weight;
	int	overlap;
	int	flags;
};
/*
                                
 */
#define PERF_X86_EVENT_PEBS_LDLAT	0x1 /*                                */
#define PERF_X86_EVENT_PEBS_ST		0x2 /*                          */

struct amd_nb {
	int nb_id;  /*                */
	int refcnt; /*                 */
	struct perf_event *owners[X86_PMC_IDX_MAX];
	struct event_constraint event_constraints[X86_PMC_IDX_MAX];
};

/*                                    */
#define MAX_PEBS_EVENTS		8

/*
                               
  
                                                       
 */
struct debug_store {
	u64	bts_buffer_base;
	u64	bts_index;
	u64	bts_absolute_maximum;
	u64	bts_interrupt_threshold;
	u64	pebs_buffer_base;
	u64	pebs_index;
	u64	pebs_absolute_maximum;
	u64	pebs_interrupt_threshold;
	u64	pebs_event_reset[MAX_PEBS_EVENTS];
};

/*
                      
 */
struct er_account {
	raw_spinlock_t		lock;	/*                             */
	u64                 config;	/*                  */
	u64                 reg;	/*                  */
	atomic_t            ref;	/*                 */
};

/*
                     
  
                                                            
                                
 */
struct intel_shared_regs {
	struct er_account       regs[EXTRA_REG_MAX];
	int                     refcnt;		/*                       */
	unsigned                core_id;	/*                   */
};

#define MAX_LBR_ENTRIES		16

struct cpu_hw_events {
	/*
                        
  */
	struct perf_event	*events[X86_PMC_IDX_MAX]; /*                  */
	unsigned long		active_mask[BITS_TO_LONGS(X86_PMC_IDX_MAX)];
	unsigned long		running[BITS_TO_LONGS(X86_PMC_IDX_MAX)];
	int			enabled;

	int			n_events;
	int			n_added;
	int			n_txn;
	int			assign[X86_PMC_IDX_MAX]; /*                             */
	u64			tags[X86_PMC_IDX_MAX];
	struct perf_event	*event_list[X86_PMC_IDX_MAX]; /*                  */

	unsigned int		group_flag;
	int			is_fake;

	/*
                         
  */
	struct debug_store	*ds;
	u64			pebs_enabled;

	/*
                  
  */
	int				lbr_users;
	void				*lbr_context;
	struct perf_branch_stack	lbr_stack;
	struct perf_branch_entry	lbr_entries[MAX_LBR_ENTRIES];
	struct er_account		*lbr_sel;
	u64				br_sel;

	/*
                                 
  */
	u64				intel_ctrl_guest_mask;
	u64				intel_ctrl_host_mask;
	struct perf_guest_switch_msr	guest_switch_msrs[X86_PMC_IDX_MAX];

	/*
                                               
                             
  */
	struct intel_shared_regs	*shared_regs;

	/*
                     
  */
	struct amd_nb			*amd_nb;
	/*                                                               */
	u64				perf_ctr_virt_mask;

	void				*kfree_on_online;
};

#define __EVENT_CONSTRAINT(c, n, m, w, o, f) {\
	{ .idxmsk64 = (n) },		\
	.code = (c),			\
	.cmask = (m),			\
	.weight = (w),			\
	.overlap = (o),			\
	.flags = f,			\
}

#define EVENT_CONSTRAINT(c, n, m)	\
	__EVENT_CONSTRAINT(c, n, m, HWEIGHT(n), 0, 0)

/*
                                                                    
                                                                      
                                                                      
                       
  
                                                      
                                              
                                              
  
                                                                      
                                                                 
                                                                    
                                                                       
                                             
  
                                                                  
                                                              
                                                                      
                                                   
 */
#define EVENT_CONSTRAINT_OVERLAP(c, n, m)	\
	__EVENT_CONSTRAINT(c, n, m, HWEIGHT(n), 1, 0)

/*
                                
 */
#define INTEL_EVENT_CONSTRAINT(c, n)	\
	EVENT_CONSTRAINT(c, n, ARCH_PERFMON_EVENTSEL_EVENT)

/*
                                                    
  
                                                
                                                       
         
          
              
                                                      
                                                        
 */
#define FIXED_EVENT_CONSTRAINT(c, n)	\
	EVENT_CONSTRAINT(c, (1ULL << (32+n)), X86_RAW_EVENT_MASK)

/*
                                       
 */
#define INTEL_UEVENT_CONSTRAINT(c, n)	\
	EVENT_CONSTRAINT(c, n, INTEL_ARCH_EVENT_MASK)

#define INTEL_PLD_CONSTRAINT(c, n)	\
	__EVENT_CONSTRAINT(c, n, INTEL_ARCH_EVENT_MASK, \
			   HWEIGHT(n), 0, PERF_X86_EVENT_PEBS_LDLAT)

#define INTEL_PST_CONSTRAINT(c, n)	\
	__EVENT_CONSTRAINT(c, n, INTEL_ARCH_EVENT_MASK, \
			  HWEIGHT(n), 0, PERF_X86_EVENT_PEBS_ST)

#define EVENT_CONSTRAINT_END		\
	EVENT_CONSTRAINT(0, 0, 0)

#define for_each_event_constraint(e, c)	\
	for ((e) = (c); (e)->weight; (e)++)

/*
                                       
  
                                                          
                                                         
                                                         
                                                         
                                                            
                                                       
 */
struct extra_reg {
	unsigned int		event;
	unsigned int		msr;
	u64			config_mask;
	u64			valid_mask;
	int			idx;  /*                           */
};

#define EVENT_EXTRA_REG(e, ms, m, vm, i) {	\
	.event = (e),		\
	.msr = (ms),		\
	.config_mask = (m),	\
	.valid_mask = (vm),	\
	.idx = EXTRA_REG_##i,	\
	}

#define INTEL_EVENT_EXTRA_REG(event, msr, vm, idx)	\
	EVENT_EXTRA_REG(event, msr, ARCH_PERFMON_EVENTSEL_EVENT, vm, idx)

#define INTEL_UEVENT_EXTRA_REG(event, msr, vm, idx) \
	EVENT_EXTRA_REG(event, msr, ARCH_PERFMON_EVENTSEL_EVENT | \
			ARCH_PERFMON_EVENTSEL_UMASK, vm, idx)

#define INTEL_UEVENT_PEBS_LDLAT_EXTRA_REG(c) \
	INTEL_UEVENT_EXTRA_REG(c, \
			       MSR_PEBS_LD_LAT_THRESHOLD, \
			       0xffff, \
			       LDLAT)

#define EVENT_EXTRA_END EVENT_EXTRA_REG(0, 0, 0, 0, RSP_0)

union perf_capabilities {
	struct {
		u64	lbr_format:6;
		u64	pebs_trap:1;
		u64	pebs_arch_reg:1;
		u64	pebs_format:4;
		u64	smm_freeze:1;
	};
	u64	capabilities;
};

struct x86_pmu_quirk {
	struct x86_pmu_quirk *next;
	void (*func)(void);
};

union x86_pmu_config {
	struct {
		u64 event:8,
		    umask:8,
		    usr:1,
		    os:1,
		    edge:1,
		    pc:1,
		    interrupt:1,
		    __reserved1:1,
		    en:1,
		    inv:1,
		    cmask:8,
		    event2:4,
		    __reserved2:4,
		    go:1,
		    ho:1;
	} bits;
	u64 value;
};

#define X86_CONFIG(args...) ((union x86_pmu_config){.bits = {args}}).value

/*
                                   
 */
struct x86_pmu {
	/*
                        
  */
	const char	*name;
	int		version;
	int		(*handle_irq)(struct pt_regs *);
	void		(*disable_all)(void);
	void		(*enable_all)(int added);
	void		(*enable)(struct perf_event *);
	void		(*disable)(struct perf_event *);
	int		(*hw_config)(struct perf_event *event);
	int		(*schedule_events)(struct cpu_hw_events *cpuc, int n, int *assign);
	unsigned	eventsel;
	unsigned	perfctr;
	int		(*addr_offset)(int index, bool eventsel);
	int		(*rdpmc_index)(int index);
	u64		(*event_map)(int);
	int		max_events;
	int		num_counters;
	int		num_counters_fixed;
	int		cntval_bits;
	u64		cntval_mask;
	union {
			unsigned long events_maskl;
			unsigned long events_mask[BITS_TO_LONGS(ARCH_PERFMON_EVENTS_COUNT)];
	};
	int		events_mask_len;
	int		apic;
	u64		max_period;
	struct event_constraint *
			(*get_event_constraints)(struct cpu_hw_events *cpuc,
						 struct perf_event *event);

	void		(*put_event_constraints)(struct cpu_hw_events *cpuc,
						 struct perf_event *event);
	struct event_constraint *event_constraints;
	struct x86_pmu_quirk *quirks;
	int		perfctr_second_write;

	/*
               
  */
	int		attr_rdpmc;
	struct attribute **format_attrs;
	struct attribute **event_attrs;

	ssize_t		(*events_sysfs_show)(char *page, u64 config);
	struct attribute **cpu_events;

	/*
                     
  */
	int		(*cpu_prepare)(int cpu);
	void		(*cpu_starting)(int cpu);
	void		(*cpu_dying)(int cpu);
	void		(*cpu_dead)(int cpu);

	void		(*check_microcode)(void);
	void		(*flush_branch_stack)(void);

	/*
                          
  */
	u64			intel_ctrl;
	union perf_capabilities intel_cap;

	/*
                         
  */
	unsigned int	bts		:1,
			bts_active	:1,
			pebs		:1,
			pebs_active	:1,
			pebs_broken	:1;
	int		pebs_record_size;
	void		(*drain_pebs)(struct pt_regs *regs);
	struct event_constraint *pebs_constraints;
	void		(*pebs_aliases)(struct perf_event *event);
	int 		max_pebs_events;

	/*
             
  */
	unsigned long	lbr_tos, lbr_from, lbr_to; /*                     */
	int		lbr_nr;			   /*                     */
	u64		lbr_sel_mask;		   /*                       */
	const int	*lbr_sel_map;		   /*                     */

	/*
                              
  */
	struct extra_reg *extra_regs;
	unsigned int er_flags;

	/*
                                  
  */
	struct perf_guest_switch_msr *(*guest_get_msrs)(int *nr);
};

#define x86_add_quirk(func_)						\
do {									\
	static struct x86_pmu_quirk __quirk __initdata = {		\
		.func = func_,						\
	};								\
	__quirk.next = x86_pmu.quirks;					\
	x86_pmu.quirks = &__quirk;					\
} while (0)

#define ERF_NO_HT_SHARING	1
#define ERF_HAS_RSP_1		2

#define EVENT_VAR(_id)  event_attr_##_id
#define EVENT_PTR(_id) &event_attr_##_id.attr.attr

#define EVENT_ATTR(_name, _id)						\
static struct perf_pmu_events_attr EVENT_VAR(_id) = {			\
	.attr		= __ATTR(_name, 0444, events_sysfs_show, NULL),	\
	.id		= PERF_COUNT_HW_##_id,				\
	.event_str	= NULL,						\
};

#define EVENT_ATTR_STR(_name, v, str)					\
static struct perf_pmu_events_attr event_attr_##v = {			\
	.attr		= __ATTR(_name, 0444, events_sysfs_show, NULL),	\
	.id		= 0,						\
	.event_str	= str,						\
};

extern struct x86_pmu x86_pmu __read_mostly;

DECLARE_PER_CPU(struct cpu_hw_events, cpu_hw_events);

int x86_perf_event_set_period(struct perf_event *event);

/*
                                                        
                                              
                                                        
                                                    
      
 */

#define C(x) PERF_COUNT_HW_CACHE_##x

extern u64 __read_mostly hw_cache_event_ids
				[PERF_COUNT_HW_CACHE_MAX]
				[PERF_COUNT_HW_CACHE_OP_MAX]
				[PERF_COUNT_HW_CACHE_RESULT_MAX];
extern u64 __read_mostly hw_cache_extra_regs
				[PERF_COUNT_HW_CACHE_MAX]
				[PERF_COUNT_HW_CACHE_OP_MAX]
				[PERF_COUNT_HW_CACHE_RESULT_MAX];

u64 x86_perf_event_update(struct perf_event *event);

static inline unsigned int x86_pmu_config_addr(int index)
{
	return x86_pmu.eventsel + (x86_pmu.addr_offset ?
				   x86_pmu.addr_offset(index, true) : index);
}

static inline unsigned int x86_pmu_event_addr(int index)
{
	return x86_pmu.perfctr + (x86_pmu.addr_offset ?
				  x86_pmu.addr_offset(index, false) : index);
}

static inline int x86_pmu_rdpmc_index(int index)
{
	return x86_pmu.rdpmc_index ? x86_pmu.rdpmc_index(index) : index;
}

int x86_setup_perfctr(struct perf_event *event);

int x86_pmu_hw_config(struct perf_event *event);

void x86_pmu_disable_all(void);

static inline void __x86_pmu_enable_event(struct hw_perf_event *hwc,
					  u64 enable_mask)
{
	u64 disable_mask = __this_cpu_read(cpu_hw_events.perf_ctr_virt_mask);

	if (hwc->extra_reg.reg)
		wrmsrl(hwc->extra_reg.reg, hwc->extra_reg.config);
	wrmsrl(hwc->config_base, (hwc->config | enable_mask) & ~disable_mask);
}

void x86_pmu_enable_all(int added);

int perf_assign_events(struct event_constraint **constraints, int n,
			int wmin, int wmax, int *assign);
int x86_schedule_events(struct cpu_hw_events *cpuc, int n, int *assign);

void x86_pmu_stop(struct perf_event *event, int flags);

static inline void x86_pmu_disable_event(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;

	wrmsrl(hwc->config_base, hwc->config);
}

void x86_pmu_enable_event(struct perf_event *event);

int x86_pmu_handle_irq(struct pt_regs *regs);

extern struct event_constraint emptyconstraint;

extern struct event_constraint unconstrained;

static inline bool kernel_ip(unsigned long ip)
{
#ifdef CONFIG_X86_32
	return ip > PAGE_OFFSET;
#else
	return (long)ip < 0;
#endif
}

/*
                                                                              
                                                                      
            
  
                                                                               
                                                                               
                   
  
                                                                            
                                                                         
 */
static inline void set_linear_ip(struct pt_regs *regs, unsigned long ip)
{
	regs->cs = kernel_ip(ip) ? __KERNEL_CS : __USER_CS;
	if (regs->flags & X86_VM_MASK)
		regs->flags ^= (PERF_EFLAGS_VM | X86_VM_MASK);
	regs->ip = ip;
}

ssize_t x86_event_sysfs_show(char *page, u64 config, u64 event);
ssize_t intel_event_sysfs_show(char *page, u64 config);

#ifdef CONFIG_CPU_SUP_AMD

int amd_pmu_init(void);

#else /*                    */

static inline int amd_pmu_init(void)
{
	return 0;
}

#endif /*                    */

#ifdef CONFIG_CPU_SUP_INTEL

int intel_pmu_save_and_restart(struct perf_event *event);

struct event_constraint *
x86_get_event_constraints(struct cpu_hw_events *cpuc, struct perf_event *event);

struct intel_shared_regs *allocate_shared_regs(int cpu);

int intel_pmu_init(void);

void init_debug_store_on_cpu(int cpu);

void fini_debug_store_on_cpu(int cpu);

void release_ds_buffers(void);

void reserve_ds_buffers(void);

extern struct event_constraint bts_constraint;

void intel_pmu_enable_bts(u64 config);

void intel_pmu_disable_bts(void);

int intel_pmu_drain_bts_buffer(void);

extern struct event_constraint intel_core2_pebs_event_constraints[];

extern struct event_constraint intel_atom_pebs_event_constraints[];

extern struct event_constraint intel_nehalem_pebs_event_constraints[];

extern struct event_constraint intel_westmere_pebs_event_constraints[];

extern struct event_constraint intel_snb_pebs_event_constraints[];

extern struct event_constraint intel_ivb_pebs_event_constraints[];

struct event_constraint *intel_pebs_constraints(struct perf_event *event);

void intel_pmu_pebs_enable(struct perf_event *event);

void intel_pmu_pebs_disable(struct perf_event *event);

void intel_pmu_pebs_enable_all(void);

void intel_pmu_pebs_disable_all(void);

void intel_ds_init(void);

void intel_pmu_lbr_reset(void);

void intel_pmu_lbr_enable(struct perf_event *event);

void intel_pmu_lbr_disable(struct perf_event *event);

void intel_pmu_lbr_enable_all(void);

void intel_pmu_lbr_disable_all(void);

void intel_pmu_lbr_read(void);

void intel_pmu_lbr_init_core(void);

void intel_pmu_lbr_init_nhm(void);

void intel_pmu_lbr_init_atom(void);

void intel_pmu_lbr_init_snb(void);

int intel_pmu_setup_lbr_filter(struct perf_event *event);

int p4_pmu_init(void);

int p6_pmu_init(void);

int knc_pmu_init(void);

ssize_t events_sysfs_show(struct device *dev, struct device_attribute *attr,
			  char *page);

#else /*                      */

static inline void reserve_ds_buffers(void)
{
}

static inline void release_ds_buffers(void)
{
}

static inline int intel_pmu_init(void)
{
	return 0;
}

static inline struct intel_shared_regs *allocate_shared_regs(int cpu)
{
	return NULL;
}

#endif /*                      */
