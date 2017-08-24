#include <linux/perf_event.h>
#include <linux/export.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <asm/apicdef.h>

#include "perf_event.h"

static __initconst const u64 amd_hw_cache_event_ids
				[PERF_COUNT_HW_CACHE_MAX]
				[PERF_COUNT_HW_CACHE_OP_MAX]
				[PERF_COUNT_HW_CACHE_RESULT_MAX] =
{
 [ C(L1D) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x0040, /*                            */
		[ C(RESULT_MISS)   ] = 0x0141, /*                            */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = 0x0142, /*                            */
		[ C(RESULT_MISS)   ] = 0,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0x0267, /*                            */
		[ C(RESULT_MISS)   ] = 0x0167, /*                            */
	},
 },
 [ C(L1I ) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x0080, /*                            */
		[ C(RESULT_MISS)   ] = 0x0081, /*                            */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = -1,
		[ C(RESULT_MISS)   ] = -1,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0x014B, /*                             */
		[ C(RESULT_MISS)   ] = 0,
	},
 },
 [ C(LL  ) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x037D, /*                             */
		[ C(RESULT_MISS)   ] = 0x037E, /*                             */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = 0x017F, /*                             */
		[ C(RESULT_MISS)   ] = 0,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0,
	},
 },
 [ C(DTLB) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x0040, /*                            */
		[ C(RESULT_MISS)   ] = 0x0746, /*                              */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0,
	},
 },
 [ C(ITLB) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x0080, /*                            */
		[ C(RESULT_MISS)   ] = 0x0385, /*                              */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = -1,
		[ C(RESULT_MISS)   ] = -1,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = -1,
		[ C(RESULT_MISS)   ] = -1,
	},
 },
 [ C(BPU ) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x00c2, /*                            */
		[ C(RESULT_MISS)   ] = 0x00c3, /*                            */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = -1,
		[ C(RESULT_MISS)   ] = -1,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = -1,
		[ C(RESULT_MISS)   ] = -1,
	},
 },
 [ C(NODE) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0xb8e9, /*                            */
		[ C(RESULT_MISS)   ] = 0x98e9, /*                            */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = -1,
		[ C(RESULT_MISS)   ] = -1,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = -1,
		[ C(RESULT_MISS)   ] = -1,
	},
 },
};

/*
                                        
 */
static const u64 amd_perfmon_event_map[] =
{
  [PERF_COUNT_HW_CPU_CYCLES]			= 0x0076,
  [PERF_COUNT_HW_INSTRUCTIONS]			= 0x00c0,
  [PERF_COUNT_HW_CACHE_REFERENCES]		= 0x0080,
  [PERF_COUNT_HW_CACHE_MISSES]			= 0x0081,
  [PERF_COUNT_HW_BRANCH_INSTRUCTIONS]		= 0x00c2,
  [PERF_COUNT_HW_BRANCH_MISSES]			= 0x00c3,
  [PERF_COUNT_HW_STALLED_CYCLES_FRONTEND]	= 0x00d0, /*                       */
  [PERF_COUNT_HW_STALLED_CYCLES_BACKEND]	= 0x00d1, /*                         */
};

static u64 amd_pmu_event_map(int hw_event)
{
	return amd_perfmon_event_map[hw_event];
}

/*
                                
 */
static unsigned int event_offsets[X86_PMC_IDX_MAX] __read_mostly;
static unsigned int count_offsets[X86_PMC_IDX_MAX] __read_mostly;

/*
               
                                                       
  
                                                 
                                                       
 */
static inline int amd_pmu_addr_offset(int index, bool eventsel)
{
	int offset;

	if (!index)
		return index;

	if (eventsel)
		offset = event_offsets[index];
	else
		offset = count_offsets[index];

	if (offset)
		return offset;

	if (!cpu_has_perfctr_core)
		offset = index;
	else
		offset = index << 1;

	if (eventsel)
		event_offsets[index] = offset;
	else
		count_offsets[index] = offset;

	return offset;
}

static int amd_core_hw_config(struct perf_event *event)
{
	if (event->attr.exclude_host && event->attr.exclude_guest)
		/*
                                                                 
                                                                 
                                                           
   */
		event->hw.config &= ~(ARCH_PERFMON_EVENTSEL_USR |
				      ARCH_PERFMON_EVENTSEL_OS);
	else if (event->attr.exclude_host)
		event->hw.config |= AMD64_EVENTSEL_GUESTONLY;
	else if (event->attr.exclude_guest)
		event->hw.config |= AMD64_EVENTSEL_HOSTONLY;

	return 0;
}

/*
                                                        
 */
static inline unsigned int amd_get_event_code(struct hw_perf_event *hwc)
{
	return ((hwc->config >> 24) & 0x0f00) | (hwc->config & 0x00ff);
}

static inline int amd_is_nb_event(struct hw_perf_event *hwc)
{
	return (hwc->config & 0xe0) == 0xe0;
}

static inline int amd_has_nb(struct cpu_hw_events *cpuc)
{
	struct amd_nb *nb = cpuc->amd_nb;

	return nb && nb->nb_id != -1;
}

static int amd_pmu_hw_config(struct perf_event *event)
{
	int ret;

	/*                                     */
	if (event->attr.precise_ip && get_ibs_caps())
		return -ENOENT;

	if (has_branch_stack(event))
		return -EOPNOTSUPP;

	ret = x86_pmu_hw_config(event);
	if (ret)
		return ret;

	if (event->attr.type == PERF_TYPE_RAW)
		event->hw.config |= event->attr.config & AMD64_RAW_EVENT_MASK;

	return amd_core_hw_config(event);
}

static void __amd_put_nb_event_constraints(struct cpu_hw_events *cpuc,
					   struct perf_event *event)
{
	struct amd_nb *nb = cpuc->amd_nb;
	int i;

	/*
                                                      
                                   
   
                                                     
                                                       
                     
  */
	for (i = 0; i < x86_pmu.num_counters; i++) {
		if (cmpxchg(nb->owners + i, event, NULL) == event)
			break;
	}
}

 /*
                                                           
                                                            
                                            
   
                                                           
                                                           
                                                         
                                                         
                                                         
                                                       
                                                       
                 
   
                                                           
                                                             
                                                             
                                                       
   
                                                         
                                                                 
                                                                
                                                             
                                                              
                   
   
                                                                 
                                                                   
                                              
   
                                                              
                                                               
                                            
   
                                                       
  */
static struct event_constraint *
__amd_get_nb_event_constraints(struct cpu_hw_events *cpuc, struct perf_event *event,
			       struct event_constraint *c)
{
	struct hw_perf_event *hwc = &event->hw;
	struct amd_nb *nb = cpuc->amd_nb;
	struct perf_event *old;
	int idx, new = -1;

	if (!c)
		c = &unconstrained;

	if (cpuc->is_fake)
		return c;

	/*
                                          
   
                                       
                             
   
                                                               
                                                             
                                                     
  */
	for_each_set_bit(idx, c->idxmsk, x86_pmu.num_counters) {
		if (new == -1 || hwc->idx == idx)
			/*                                   */
			old = cmpxchg(nb->owners + idx, NULL, event);
		else if (nb->owners[idx] == event)
			/*                       */
			old = event;
		else
			continue;

		if (old && old != event)
			continue;

		/*                       */
		if (new != -1)
			cmpxchg(nb->owners + new, event, NULL);
		new = idx;

		/*                        */
		if (old == event)
			break;
	}

	if (new == -1)
		return &emptyconstraint;

	return &nb->event_constraints[new];
}

static struct amd_nb *amd_alloc_nb(int cpu)
{
	struct amd_nb *nb;
	int i;

	nb = kmalloc_node(sizeof(struct amd_nb), GFP_KERNEL | __GFP_ZERO,
			  cpu_to_node(cpu));
	if (!nb)
		return NULL;

	nb->nb_id = -1;

	/*
                                          
  */
	for (i = 0; i < x86_pmu.num_counters; i++) {
		__set_bit(i, nb->event_constraints[i].idxmsk);
		nb->event_constraints[i].weight = 1;
	}
	return nb;
}

static int amd_pmu_cpu_prepare(int cpu)
{
	struct cpu_hw_events *cpuc = &per_cpu(cpu_hw_events, cpu);

	WARN_ON_ONCE(cpuc->amd_nb);

	if (boot_cpu_data.x86_max_cores < 2)
		return NOTIFY_OK;

	cpuc->amd_nb = amd_alloc_nb(cpu);
	if (!cpuc->amd_nb)
		return NOTIFY_BAD;

	return NOTIFY_OK;
}

static void amd_pmu_cpu_starting(int cpu)
{
	struct cpu_hw_events *cpuc = &per_cpu(cpu_hw_events, cpu);
	struct amd_nb *nb;
	int i, nb_id;

	cpuc->perf_ctr_virt_mask = AMD64_EVENTSEL_HOSTONLY;

	if (boot_cpu_data.x86_max_cores < 2)
		return;

	nb_id = amd_get_nb_id(cpu);
	WARN_ON_ONCE(nb_id == BAD_APICID);

	for_each_online_cpu(i) {
		nb = per_cpu(cpu_hw_events, i).amd_nb;
		if (WARN_ON_ONCE(!nb))
			continue;

		if (nb->nb_id == nb_id) {
			cpuc->kfree_on_online = cpuc->amd_nb;
			cpuc->amd_nb = nb;
			break;
		}
	}

	cpuc->amd_nb->nb_id = nb_id;
	cpuc->amd_nb->refcnt++;
}

static void amd_pmu_cpu_dead(int cpu)
{
	struct cpu_hw_events *cpuhw;

	if (boot_cpu_data.x86_max_cores < 2)
		return;

	cpuhw = &per_cpu(cpu_hw_events, cpu);

	if (cpuhw->amd_nb) {
		struct amd_nb *nb = cpuhw->amd_nb;

		if (nb->nb_id == -1 || --nb->refcnt == 0)
			kfree(nb);

		cpuhw->amd_nb = NULL;
	}
}

static struct event_constraint *
amd_get_event_constraints(struct cpu_hw_events *cpuc, struct perf_event *event)
{
	/*
                                                 
  */
	if (!(amd_has_nb(cpuc) && amd_is_nb_event(&event->hw)))
		return &unconstrained;

	return __amd_get_nb_event_constraints(cpuc, event, NULL);
}

static void amd_put_event_constraints(struct cpu_hw_events *cpuc,
				      struct perf_event *event)
{
	if (amd_has_nb(cpuc) && amd_is_nb_event(&event->hw))
		__amd_put_nb_event_constraints(cpuc, event);
}

PMU_FORMAT_ATTR(event,	"config:0-7,32-35");
PMU_FORMAT_ATTR(umask,	"config:8-15"	);
PMU_FORMAT_ATTR(edge,	"config:18"	);
PMU_FORMAT_ATTR(inv,	"config:23"	);
PMU_FORMAT_ATTR(cmask,	"config:24-31"	);

static struct attribute *amd_format_attr[] = {
	&format_attr_event.attr,
	&format_attr_umask.attr,
	&format_attr_edge.attr,
	&format_attr_inv.attr,
	&format_attr_cmask.attr,
	NULL,
};

/*                */

#define AMD_EVENT_TYPE_MASK	0x000000F0ULL

#define AMD_EVENT_FP		0x00000000ULL ... 0x00000010ULL
#define AMD_EVENT_LS		0x00000020ULL ... 0x00000030ULL
#define AMD_EVENT_DC		0x00000040ULL ... 0x00000050ULL
#define AMD_EVENT_CU		0x00000060ULL ... 0x00000070ULL
#define AMD_EVENT_IC_DE		0x00000080ULL ... 0x00000090ULL
#define AMD_EVENT_EX_LS		0x000000C0ULL
#define AMD_EVENT_DE		0x000000D0ULL
#define AMD_EVENT_NB		0x000000E0ULL ... 0x000000F0ULL

/*
                                          
  
                             
  
                         
                         
                         
                         
                         
                         
                         
                         
                            
                            
            
            
                            
                         
                            
                            
  
              
  
                                          
                       
                                          
                       
                       
                         
                       
                         
                              
                         
                         
                         
                         
                         
                       
                       
                       
                         
                         
                         
                         
                         
                         
                         
                         
  
                                                           
                                           
 */

static struct event_constraint amd_f15_PMC0  = EVENT_CONSTRAINT(0, 0x01, 0);
static struct event_constraint amd_f15_PMC20 = EVENT_CONSTRAINT(0, 0x07, 0);
static struct event_constraint amd_f15_PMC3  = EVENT_CONSTRAINT(0, 0x08, 0);
static struct event_constraint amd_f15_PMC30 = EVENT_CONSTRAINT_OVERLAP(0, 0x09, 0);
static struct event_constraint amd_f15_PMC50 = EVENT_CONSTRAINT(0, 0x3F, 0);
static struct event_constraint amd_f15_PMC53 = EVENT_CONSTRAINT(0, 0x38, 0);

static struct event_constraint *
amd_get_event_constraints_f15h(struct cpu_hw_events *cpuc, struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	unsigned int event_code = amd_get_event_code(hwc);

	switch (event_code & AMD_EVENT_TYPE_MASK) {
	case AMD_EVENT_FP:
		switch (event_code) {
		case 0x000:
			if (!(hwc->config & 0x0000F000ULL))
				break;
			if (!(hwc->config & 0x00000F00ULL))
				break;
			return &amd_f15_PMC3;
		case 0x004:
			if (hweight_long(hwc->config & ARCH_PERFMON_EVENTSEL_UMASK) <= 1)
				break;
			return &amd_f15_PMC3;
		case 0x003:
		case 0x00B:
		case 0x00D:
			return &amd_f15_PMC3;
		}
		return &amd_f15_PMC53;
	case AMD_EVENT_LS:
	case AMD_EVENT_DC:
	case AMD_EVENT_EX_LS:
		switch (event_code) {
		case 0x023:
		case 0x043:
		case 0x045:
		case 0x046:
		case 0x054:
		case 0x055:
			return &amd_f15_PMC20;
		case 0x02D:
			return &amd_f15_PMC3;
		case 0x02E:
			return &amd_f15_PMC30;
		case 0x031:
			if (hweight_long(hwc->config & ARCH_PERFMON_EVENTSEL_UMASK) <= 1)
				return &amd_f15_PMC20;
			return &emptyconstraint;
		case 0x1C0:
			return &amd_f15_PMC53;
		default:
			return &amd_f15_PMC50;
		}
	case AMD_EVENT_CU:
	case AMD_EVENT_IC_DE:
	case AMD_EVENT_DE:
		switch (event_code) {
		case 0x08F:
		case 0x187:
		case 0x188:
			return &amd_f15_PMC0;
		case 0x0DB ... 0x0DF:
		case 0x1D6:
		case 0x1D8:
			return &amd_f15_PMC50;
		default:
			return &amd_f15_PMC20;
		}
	case AMD_EVENT_NB:
		/*                                  */
		return &emptyconstraint;
	default:
		return &emptyconstraint;
	}
}

static ssize_t amd_event_sysfs_show(char *page, u64 config)
{
	u64 event = (config & ARCH_PERFMON_EVENTSEL_EVENT) |
		    (config & AMD64_EVENTSEL_EVENT) >> 24;

	return x86_event_sysfs_show(page, config, event);
}

static __initconst const struct x86_pmu amd_pmu = {
	.name			= "AMD",
	.handle_irq		= x86_pmu_handle_irq,
	.disable_all		= x86_pmu_disable_all,
	.enable_all		= x86_pmu_enable_all,
	.enable			= x86_pmu_enable_event,
	.disable		= x86_pmu_disable_event,
	.hw_config		= amd_pmu_hw_config,
	.schedule_events	= x86_schedule_events,
	.eventsel		= MSR_K7_EVNTSEL0,
	.perfctr		= MSR_K7_PERFCTR0,
	.addr_offset            = amd_pmu_addr_offset,
	.event_map		= amd_pmu_event_map,
	.max_events		= ARRAY_SIZE(amd_perfmon_event_map),
	.num_counters		= AMD64_NUM_COUNTERS,
	.cntval_bits		= 48,
	.cntval_mask		= (1ULL << 48) - 1,
	.apic			= 1,
	/*                                    */
	.max_period		= (1ULL << 47) - 1,
	.get_event_constraints	= amd_get_event_constraints,
	.put_event_constraints	= amd_put_event_constraints,

	.format_attrs		= amd_format_attr,
	.events_sysfs_show	= amd_event_sysfs_show,

	.cpu_prepare		= amd_pmu_cpu_prepare,
	.cpu_starting		= amd_pmu_cpu_starting,
	.cpu_dead		= amd_pmu_cpu_dead,
};

static int setup_event_constraints(void)
{
	if (boot_cpu_data.x86 == 0x15)
		x86_pmu.get_event_constraints = amd_get_event_constraints_f15h;
	return 0;
}

static int setup_perfctr_core(void)
{
	if (!cpu_has_perfctr_core) {
		WARN(x86_pmu.get_event_constraints == amd_get_event_constraints_f15h,
		     KERN_ERR "Odd, counter constraints enabled but no core perfctrs detected!");
		return -ENODEV;
	}

	WARN(x86_pmu.get_event_constraints == amd_get_event_constraints,
	     KERN_ERR "hw perf events core counters need constraints handler!");

	/*
                                                              
                                                      
                          
  */
	x86_pmu.eventsel	= MSR_F15H_PERF_CTL;
	x86_pmu.perfctr		= MSR_F15H_PERF_CTR;
	x86_pmu.num_counters	= AMD64_NUM_COUNTERS_CORE;

	printk(KERN_INFO "perf: AMD core performance counters detected\n");

	return 0;
}

__init int amd_pmu_init(void)
{
	/*                                                     */
	if (boot_cpu_data.x86 < 6)
		return -ENODEV;

	x86_pmu = amd_pmu;

	setup_event_constraints();
	setup_perfctr_core();

	/*                                */
	memcpy(hw_cache_event_ids, amd_hw_cache_event_ids,
	       sizeof(hw_cache_event_ids));

	return 0;
}

void amd_pmu_enable_virt(void)
{
	struct cpu_hw_events *cpuc = &__get_cpu_var(cpu_hw_events);

	cpuc->perf_ctr_virt_mask = 0;

	/*                   */
	x86_pmu_disable_all();
	x86_pmu_enable_all(0);
}
EXPORT_SYMBOL_GPL(amd_pmu_enable_virt);

void amd_pmu_disable_virt(void)
{
	struct cpu_hw_events *cpuc = &__get_cpu_var(cpu_hw_events);

	/*
                                                                       
                                                                      
                                                                      
                            
  */
	cpuc->perf_ctr_virt_mask = AMD64_EVENTSEL_HOSTONLY;

	/*                   */
	x86_pmu_disable_all();
	x86_pmu_enable_all(0);
}
EXPORT_SYMBOL_GPL(amd_pmu_disable_virt);
