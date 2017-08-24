/*                                                */

#include <linux/perf_event.h>
#include <linux/types.h>

#include <asm/hardirq.h>

#include "perf_event.h"

static const u64 knc_perfmon_event_map[] =
{
  [PERF_COUNT_HW_CPU_CYCLES]		= 0x002a,
  [PERF_COUNT_HW_INSTRUCTIONS]		= 0x0016,
  [PERF_COUNT_HW_CACHE_REFERENCES]	= 0x0028,
  [PERF_COUNT_HW_CACHE_MISSES]		= 0x0029,
  [PERF_COUNT_HW_BRANCH_INSTRUCTIONS]	= 0x0012,
  [PERF_COUNT_HW_BRANCH_MISSES]		= 0x002b,
};

static const u64 __initconst knc_hw_cache_event_ids
				[PERF_COUNT_HW_CACHE_MAX]
				[PERF_COUNT_HW_CACHE_OP_MAX]
				[PERF_COUNT_HW_CACHE_RESULT_MAX] =
{
 [ C(L1D) ] = {
	[ C(OP_READ) ] = {
		/*                                                     */
		/*                                                     */
		/*                                                     */
		/*                                                     */
		[ C(RESULT_ACCESS) ] = ARCH_PERFMON_EVENTSEL_INT,
						/*                     */
		[ C(RESULT_MISS)   ] = 0x0003,	/*                     */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = 0x0001,	/*                     */
		[ C(RESULT_MISS)   ] = 0x0004,	/*                     */
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0x0011,	/*                     */
		[ C(RESULT_MISS)   ] = 0x001c,	/*                     */
	},
 },
 [ C(L1I ) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x000c,	/*                    */
		[ C(RESULT_MISS)   ] = 0x000e,	/*                    */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = -1,
		[ C(RESULT_MISS)   ] = -1,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0x0,
		[ C(RESULT_MISS)   ] = 0x0,
	},
 },
 [ C(LL  ) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0,
		[ C(RESULT_MISS)   ] = 0x10cb,	/*              */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = 0x10cc,	/*              */
		[ C(RESULT_MISS)   ] = 0,
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0x10fc,	/*                  */
		[ C(RESULT_MISS)   ] = 0x10fe,	/*                  */
	},
 },
 [ C(DTLB) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = ARCH_PERFMON_EVENTSEL_INT,
						/*           */
						/*                        */
		[ C(RESULT_MISS)   ] = 0x0002,	/*                */
	},
	[ C(OP_WRITE) ] = {
		[ C(RESULT_ACCESS) ] = 0x0001,	/*            */
		[ C(RESULT_MISS)   ] = 0x0002,	/*                */
	},
	[ C(OP_PREFETCH) ] = {
		[ C(RESULT_ACCESS) ] = 0x0,
		[ C(RESULT_MISS)   ] = 0x0,
	},
 },
 [ C(ITLB) ] = {
	[ C(OP_READ) ] = {
		[ C(RESULT_ACCESS) ] = 0x000c,	/*           */
		[ C(RESULT_MISS)   ] = 0x000d,	/*                */
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
		[ C(RESULT_ACCESS) ] = 0x0012,	/*          */
		[ C(RESULT_MISS)   ] = 0x002b,	/*                       */
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


static u64 knc_pmu_event_map(int hw_event)
{
	return knc_perfmon_event_map[hw_event];
}

static struct event_constraint knc_event_constraints[] =
{
	INTEL_EVENT_CONSTRAINT(0xc3, 0x1),	/*           */
	INTEL_EVENT_CONSTRAINT(0xc4, 0x1),	/*            */
	INTEL_EVENT_CONSTRAINT(0xc8, 0x1),	/*               */
	INTEL_EVENT_CONSTRAINT(0xc9, 0x1),	/*               */
	INTEL_EVENT_CONSTRAINT(0xca, 0x1),	/*               */
	INTEL_EVENT_CONSTRAINT(0xcb, 0x1),	/*              */
	INTEL_EVENT_CONSTRAINT(0xcc, 0x1),	/*              */
	INTEL_EVENT_CONSTRAINT(0xce, 0x1),	/*                                            */
	INTEL_EVENT_CONSTRAINT(0xcf, 0x1),	/*                                         */
	INTEL_EVENT_CONSTRAINT(0xd7, 0x1),	/*                         */
	INTEL_EVENT_CONSTRAINT(0xe3, 0x1),	/*                */
	INTEL_EVENT_CONSTRAINT(0xe6, 0x1),	/*            */
	INTEL_EVENT_CONSTRAINT(0xe7, 0x1),	/*             */
	INTEL_EVENT_CONSTRAINT(0xf1, 0x1),	/*                              */
	INTEL_EVENT_CONSTRAINT(0xf2, 0x1),	/*                               */
	INTEL_EVENT_CONSTRAINT(0xf6, 0x1),	/*                            */
	INTEL_EVENT_CONSTRAINT(0xf7, 0x1),	/*                             */
	INTEL_EVENT_CONSTRAINT(0xfc, 0x1),	/*             */
	INTEL_EVENT_CONSTRAINT(0xfd, 0x1),	/*                  */
	INTEL_EVENT_CONSTRAINT(0xfe, 0x1),	/*                  */
	INTEL_EVENT_CONSTRAINT(0xff, 0x1),	/*                          */
	EVENT_CONSTRAINT_END
};

#define MSR_KNC_IA32_PERF_GLOBAL_STATUS		0x0000002d
#define MSR_KNC_IA32_PERF_GLOBAL_OVF_CONTROL	0x0000002e
#define MSR_KNC_IA32_PERF_GLOBAL_CTRL		0x0000002f

#define KNC_ENABLE_COUNTER0			0x00000001
#define KNC_ENABLE_COUNTER1			0x00000002

static void knc_pmu_disable_all(void)
{
	u64 val;

	rdmsrl(MSR_KNC_IA32_PERF_GLOBAL_CTRL, val);
	val &= ~(KNC_ENABLE_COUNTER0|KNC_ENABLE_COUNTER1);
	wrmsrl(MSR_KNC_IA32_PERF_GLOBAL_CTRL, val);
}

static void knc_pmu_enable_all(int added)
{
	u64 val;

	rdmsrl(MSR_KNC_IA32_PERF_GLOBAL_CTRL, val);
	val |= (KNC_ENABLE_COUNTER0|KNC_ENABLE_COUNTER1);
	wrmsrl(MSR_KNC_IA32_PERF_GLOBAL_CTRL, val);
}

static inline void
knc_pmu_disable_event(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 val;

	val = hwc->config;
	val &= ~ARCH_PERFMON_EVENTSEL_ENABLE;

	(void)wrmsrl_safe(hwc->config_base + hwc->idx, val);
}

static void knc_pmu_enable_event(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 val;

	val = hwc->config;
	val |= ARCH_PERFMON_EVENTSEL_ENABLE;

	(void)wrmsrl_safe(hwc->config_base + hwc->idx, val);
}

static inline u64 knc_pmu_get_status(void)
{
	u64 status;

	rdmsrl(MSR_KNC_IA32_PERF_GLOBAL_STATUS, status);

	return status;
}

static inline void knc_pmu_ack_status(u64 ack)
{
	wrmsrl(MSR_KNC_IA32_PERF_GLOBAL_OVF_CONTROL, ack);
}

static int knc_pmu_handle_irq(struct pt_regs *regs)
{
	struct perf_sample_data data;
	struct cpu_hw_events *cpuc;
	int handled = 0;
	int bit, loops;
	u64 status;

	cpuc = &__get_cpu_var(cpu_hw_events);

	knc_pmu_disable_all();

	status = knc_pmu_get_status();
	if (!status) {
		knc_pmu_enable_all(0);
		return handled;
	}

	loops = 0;
again:
	knc_pmu_ack_status(status);
	if (++loops > 100) {
		WARN_ONCE(1, "perf: irq loop stuck!\n");
		perf_event_print_debug();
		goto done;
	}

	inc_irq_stat(apic_perf_irqs);

	for_each_set_bit(bit, (unsigned long *)&status, X86_PMC_IDX_MAX) {
		struct perf_event *event = cpuc->events[bit];

		handled++;

		if (!test_bit(bit, cpuc->active_mask))
			continue;

		if (!intel_pmu_save_and_restart(event))
			continue;

		perf_sample_data_init(&data, 0, event->hw.last_period);

		if (perf_event_overflow(event, &data, regs))
			x86_pmu_stop(event, 0);
	}

	/*
                                            
  */
	status = knc_pmu_get_status();
	if (status)
		goto again;

done:
	knc_pmu_enable_all(0);

	return handled;
}


PMU_FORMAT_ATTR(event,	"config:0-7"	);
PMU_FORMAT_ATTR(umask,	"config:8-15"	);
PMU_FORMAT_ATTR(edge,	"config:18"	);
PMU_FORMAT_ATTR(inv,	"config:23"	);
PMU_FORMAT_ATTR(cmask,	"config:24-31"	);

static struct attribute *intel_knc_formats_attr[] = {
	&format_attr_event.attr,
	&format_attr_umask.attr,
	&format_attr_edge.attr,
	&format_attr_inv.attr,
	&format_attr_cmask.attr,
	NULL,
};

static const struct x86_pmu knc_pmu __initconst = {
	.name			= "knc",
	.handle_irq		= knc_pmu_handle_irq,
	.disable_all		= knc_pmu_disable_all,
	.enable_all		= knc_pmu_enable_all,
	.enable			= knc_pmu_enable_event,
	.disable		= knc_pmu_disable_event,
	.hw_config		= x86_pmu_hw_config,
	.schedule_events	= x86_schedule_events,
	.eventsel		= MSR_KNC_EVNTSEL0,
	.perfctr		= MSR_KNC_PERFCTR0,
	.event_map		= knc_pmu_event_map,
	.max_events             = ARRAY_SIZE(knc_perfmon_event_map),
	.apic			= 1,
	.max_period		= (1ULL << 39) - 1,
	.version		= 0,
	.num_counters		= 2,
	.cntval_bits		= 40,
	.cntval_mask		= (1ULL << 40) - 1,
	.get_event_constraints	= x86_get_event_constraints,
	.event_constraints	= knc_event_constraints,
	.format_attrs		= intel_knc_formats_attr,
};

__init int knc_pmu_init(void)
{
	x86_pmu = knc_pmu;

	memcpy(hw_cache_event_ids, knc_hw_cache_event_ids, 
		sizeof(hw_cache_event_ids));

	return 0;
}
