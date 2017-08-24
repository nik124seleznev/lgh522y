#include <linux/perf_event.h>
#include <linux/types.h>

#include "perf_event.h"

/*
                               
 */
static const u64 p6_perfmon_event_map[] =
{
  [PERF_COUNT_HW_CPU_CYCLES]		= 0x0079,	/*                  */
  [PERF_COUNT_HW_INSTRUCTIONS]		= 0x00c0,	/*                  */
  [PERF_COUNT_HW_CACHE_REFERENCES]	= 0x0f2e,	/*                  */
  [PERF_COUNT_HW_CACHE_MISSES]		= 0x012e,	/*                  */
  [PERF_COUNT_HW_BRANCH_INSTRUCTIONS]	= 0x00c4,	/*                  */
  [PERF_COUNT_HW_BRANCH_MISSES]		= 0x00c5,	/*                      */
  [PERF_COUNT_HW_BUS_CYCLES]		= 0x0062,	/*                  */
  [PERF_COUNT_HW_STALLED_CYCLES_FRONTEND] = 0x00a2,	/*                  */

};

static const u64 __initconst p6_hw_cache_event_ids
				[PERF_COUNT_HW_CACHE_MAX]
				[PERF_COUNT_HW_CACHE_OP_MAX]
				[PERF_COUNT_HW_CACHE_RESULT_MAX] =
{
 [ C(L1D) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x0043,	/*                     */
                [ C(RESULT_MISS)   ] = 0x0045,	/*                     */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0x0f29,	/*                     */
	},
        [ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0,
        },
 },
 [ C(L1I ) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x0080,	/*                    */
		[ C(RESULT_MISS)   ] = 0x0f28,	/*                    */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = -1,
		[ C(RESULT_MISS)   ] = -1,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0,
	},
 },
 [ C(LL  ) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0,
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0x0025,	/*                    */
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0,
	},
 },
 [ C(DTLB) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x0043,	/*                    */
		[ C(RESULT_MISS)   ] = 0,
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
		[ C(RESULT_ACCESS) ] = 0x0080,	/*                    */
		[ C(RESULT_MISS)   ] = 0x0085,	/*                    */
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
		[ C(RESULT_ACCESS) ] = 0x00c4,	/*                      */
		[ C(RESULT_MISS)   ] = 0x00c5,	/*                      */
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

static u64 p6_pmu_event_map(int hw_event)
{
	return p6_perfmon_event_map[hw_event];
}

/*
                                                         
                                                
  
                                  
 */
#define P6_NOP_EVENT			0x0000002EULL

static struct event_constraint p6_event_constraints[] =
{
	INTEL_EVENT_CONSTRAINT(0xc1, 0x1),	/*       */
	INTEL_EVENT_CONSTRAINT(0x10, 0x1),	/*                 */
	INTEL_EVENT_CONSTRAINT(0x11, 0x2),	/*           */
	INTEL_EVENT_CONSTRAINT(0x12, 0x2),	/*     */
	INTEL_EVENT_CONSTRAINT(0x13, 0x2),	/*     */
	INTEL_EVENT_CONSTRAINT(0x14, 0x1),	/*                 */
	EVENT_CONSTRAINT_END
};

static void p6_pmu_disable_all(void)
{
	u64 val;

	/*                                 */
	rdmsrl(MSR_P6_EVNTSEL0, val);
	val &= ~ARCH_PERFMON_EVENTSEL_ENABLE;
	wrmsrl(MSR_P6_EVNTSEL0, val);
}

static void p6_pmu_enable_all(int added)
{
	unsigned long val;

	/*                                 */
	rdmsrl(MSR_P6_EVNTSEL0, val);
	val |= ARCH_PERFMON_EVENTSEL_ENABLE;
	wrmsrl(MSR_P6_EVNTSEL0, val);
}

static inline void
p6_pmu_disable_event(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 val = P6_NOP_EVENT;

	(void)wrmsrl_safe(hwc->config_base, val);
}

static void p6_pmu_enable_event(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 val;

	val = hwc->config;

	/*
                                                         
                                                   
                                                   
                                  
  */

	(void)wrmsrl_safe(hwc->config_base, val);
}

PMU_FORMAT_ATTR(event,	"config:0-7"	);
PMU_FORMAT_ATTR(umask,	"config:8-15"	);
PMU_FORMAT_ATTR(edge,	"config:18"	);
PMU_FORMAT_ATTR(pc,	"config:19"	);
PMU_FORMAT_ATTR(inv,	"config:23"	);
PMU_FORMAT_ATTR(cmask,	"config:24-31"	);

static struct attribute *intel_p6_formats_attr[] = {
	&format_attr_event.attr,
	&format_attr_umask.attr,
	&format_attr_edge.attr,
	&format_attr_pc.attr,
	&format_attr_inv.attr,
	&format_attr_cmask.attr,
	NULL,
};

static __initconst const struct x86_pmu p6_pmu = {
	.name			= "p6",
	.handle_irq		= x86_pmu_handle_irq,
	.disable_all		= p6_pmu_disable_all,
	.enable_all		= p6_pmu_enable_all,
	.enable			= p6_pmu_enable_event,
	.disable		= p6_pmu_disable_event,
	.hw_config		= x86_pmu_hw_config,
	.schedule_events	= x86_schedule_events,
	.eventsel		= MSR_P6_EVNTSEL0,
	.perfctr		= MSR_P6_PERFCTR0,
	.event_map		= p6_pmu_event_map,
	.max_events		= ARRAY_SIZE(p6_perfmon_event_map),
	.apic			= 1,
	.max_period		= (1ULL << 31) - 1,
	.version		= 0,
	.num_counters		= 2,
	/*
                                                                   
                                                                
                                                               
   
                                                                 
  */
	.cntval_bits		= 32,
	.cntval_mask		= (1ULL << 32) - 1,
	.get_event_constraints	= x86_get_event_constraints,
	.event_constraints	= p6_event_constraints,

	.format_attrs		= intel_p6_formats_attr,
	.events_sysfs_show	= intel_event_sysfs_show,

};

__init int p6_pmu_init(void)
{
	switch (boot_cpu_data.x86_model) {
	case 1:
	case 3:  /*             */
	case 5:
	case 6:  /*            */
	case 7:
	case 8:
	case 11: /*             */
	case 9:
	case 13:
		/*           */
		break;
	default:
		pr_cont("unsupported p6 CPU model %d ",
			boot_cpu_data.x86_model);
		return -ENODEV;
	}

	x86_pmu = p6_pmu;

	memcpy(hw_cache_event_ids, p6_hw_cache_event_ids,
		sizeof(hw_cache_event_ids));


	return 0;
}