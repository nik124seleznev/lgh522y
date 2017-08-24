/**
 * Copyright (C) ARM Limited 2011-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include "gator.h"

#include <linux/module.h>
#include <linux/time.h>
#include <linux/math64.h>
#include <linux/slab.h>
#include <linux/io.h>

#ifdef MALI_DIR_MIDGARD
/*                                                                */
#include "mali_linux_trace.h"
#else
/*                                                             */
#include "linux/mali_linux_trace.h"
#endif

#include "gator_events_mali_common.h"

/*
                                                                                  
 */
#if (MALI_SUPPORT != MALI_MIDGARD)
#error MALI_SUPPORT set to an invalid device code: expecting MALI_MIDGARD
#endif

static const char mali_name[] = "Midgard";

/*                           
  
                     
                                                                                             
                                                                                                         
  
                 
                                                         
                                                                                                              
  
                  
                                                                           
 */

/*                                */
static const char *const timeline_event_names[] = {
	"PM_SHADER_0",
	"PM_SHADER_1",
	"PM_SHADER_2",
	"PM_SHADER_3",
	"PM_SHADER_4",
	"PM_SHADER_5",
	"PM_SHADER_6",
	"PM_SHADER_7",
	"PM_TILER_0",
	"PM_L2_0",
	"PM_L2_1",
	"MMU_AS_0",
	"MMU_AS_1",
	"MMU_AS_2",
	"MMU_AS_3"
};

enum {
	PM_SHADER_0 = 0,
	PM_SHADER_1,
	PM_SHADER_2,
	PM_SHADER_3,
	PM_SHADER_4,
	PM_SHADER_5,
	PM_SHADER_6,
	PM_SHADER_7,
	PM_TILER_0,
	PM_L2_0,
	PM_L2_1,
	MMU_AS_0,
	MMU_AS_1,
	MMU_AS_2,
	MMU_AS_3
};
/*                                               */
#define NUM_PM_SHADER (8)

/*                   */
static const char *const software_counter_names[] = {
	"MMU_PAGE_FAULT_0",
	"MMU_PAGE_FAULT_1",
	"MMU_PAGE_FAULT_2",
	"MMU_PAGE_FAULT_3"
};

enum {
	MMU_PAGE_FAULT_0 = 0,
	MMU_PAGE_FAULT_1,
	MMU_PAGE_FAULT_2,
	MMU_PAGE_FAULT_3
};

/*                   */
static const char *const accumulators_names[] = {
	"TOTAL_ALLOC_PAGES"
};

enum {
	TOTAL_ALLOC_PAGES = 0
};

#define FIRST_TIMELINE_EVENT (0)
#define NUMBER_OF_TIMELINE_EVENTS (sizeof(timeline_event_names) / sizeof(timeline_event_names[0]))
#define FIRST_SOFTWARE_COUNTER (FIRST_TIMELINE_EVENT + NUMBER_OF_TIMELINE_EVENTS)
#define NUMBER_OF_SOFTWARE_COUNTERS (sizeof(software_counter_names) / sizeof(software_counter_names[0]))
#define FIRST_ACCUMULATOR (FIRST_SOFTWARE_COUNTER + NUMBER_OF_SOFTWARE_COUNTERS)
#define NUMBER_OF_ACCUMULATORS (sizeof(accumulators_names) / sizeof(accumulators_names[0]))
#define FILMSTRIP (NUMBER_OF_TIMELINE_EVENTS + NUMBER_OF_SOFTWARE_COUNTERS + NUMBER_OF_ACCUMULATORS)
#define NUMBER_OF_EVENTS (NUMBER_OF_TIMELINE_EVENTS + NUMBER_OF_SOFTWARE_COUNTERS + NUMBER_OF_ACCUMULATORS + 1)

/*
                                             
 */
static struct mali_counter counters[NUMBER_OF_EVENTS];
static unsigned long filmstrip_event;

/*                                             
                                  
 */
static int counter_dump[NUMBER_OF_EVENTS * 2];

/*
                                                                     
                                                                    
           
 */
static struct timespec timeline_event_starttime[NUMBER_OF_TIMELINE_EVENTS];

/*                           */
static unsigned int timeline_data[NUMBER_OF_TIMELINE_EVENTS];
static unsigned int sw_counter_data[NUMBER_OF_SOFTWARE_COUNTERS];
static unsigned int accumulators_data[NUMBER_OF_ACCUMULATORS];

/*                                                                     */
static struct timespec prev_timestamp;

/* 
                                                                               
  
                                          
                                      
  
                                                                                                    
 */
static inline long get_duration_us(const struct timespec *start, const struct timespec *end)
{
	long event_duration_us = (end->tv_nsec - start->tv_nsec) / 1000;

	event_duration_us += (end->tv_sec - start->tv_sec) * 1000000;

	return event_duration_us;
}

static void record_timeline_event(unsigned int timeline_index, unsigned int type)
{
	struct timespec event_timestamp;
	struct timespec *event_start = &timeline_event_starttime[timeline_index];

	switch (type) {
	case ACTIVITY_START:
		/*                       */
		getnstimeofday(&event_timestamp);

		/*                                                                */
		if (event_start->tv_sec == 0)
			*event_start = event_timestamp;	/*                */
		break;

	case ACTIVITY_STOP:
		/*                               */
		if (event_start->tv_sec != 0) {
			/*                       */
			getnstimeofday(&event_timestamp);

			/*                               */
			timeline_data[timeline_index] += get_duration_us(event_start, &event_timestamp);

			/*                                                           */
			event_start->tv_sec = 0;
		}
		break;

	default:
		/*                                    */
		break;
	}
}

/*
                                                                         
 */

GATOR_DEFINE_PROBE(mali_pm_status, TP_PROTO(unsigned int event_id, unsigned long long value))
{
#define SHADER_PRESENT_LO       0x100	/*                                           */
#define TILER_PRESENT_LO        0x110	/*                                          */
#define L2_PRESENT_LO           0x120	/*                                             */
#define BIT_AT(value, pos) ((value >> pos) & 1)

	static unsigned long long previous_shader_bitmask;
	static unsigned long long previous_tiler_bitmask;
	static unsigned long long previous_l2_bitmask;

	switch (event_id) {
	case SHADER_PRESENT_LO:
		{
			unsigned long long changed_bitmask = previous_shader_bitmask ^ value;
			int pos;

			for (pos = 0; pos < NUM_PM_SHADER; ++pos) {
				if (BIT_AT(changed_bitmask, pos))
					record_timeline_event(PM_SHADER_0 + pos, BIT_AT(value, pos) ? ACTIVITY_START : ACTIVITY_STOP);
			}

			previous_shader_bitmask = value;
			break;
		}

	case TILER_PRESENT_LO:
		{
			unsigned long long changed = previous_tiler_bitmask ^ value;

			if (BIT_AT(changed, 0))
				record_timeline_event(PM_TILER_0, BIT_AT(value, 0) ? ACTIVITY_START : ACTIVITY_STOP);

			previous_tiler_bitmask = value;
			break;
		}

	case L2_PRESENT_LO:
		{
			unsigned long long changed = previous_l2_bitmask ^ value;

			if (BIT_AT(changed, 0))
				record_timeline_event(PM_L2_0, BIT_AT(value, 0) ? ACTIVITY_START : ACTIVITY_STOP);
			if (BIT_AT(changed, 4))
				record_timeline_event(PM_L2_1, BIT_AT(value, 4) ? ACTIVITY_START : ACTIVITY_STOP);

			previous_l2_bitmask = value;
			break;
		}

	default:
		/*                                          */
		break;
	}

#undef SHADER_PRESENT_LO
#undef TILER_PRESENT_LO
#undef L2_PRESENT_LO
#undef BIT_AT
}

GATOR_DEFINE_PROBE(mali_page_fault_insert_pages, TP_PROTO(int event_id, unsigned long value))
{
	/*                                                                                   */
	sw_counter_data[MMU_PAGE_FAULT_0 + event_id] += value;
}

GATOR_DEFINE_PROBE(mali_mmu_as_in_use, TP_PROTO(int event_id))
{
	record_timeline_event(MMU_AS_0 + event_id, ACTIVITY_START);
}

GATOR_DEFINE_PROBE(mali_mmu_as_released, TP_PROTO(int event_id))
{
	record_timeline_event(MMU_AS_0 + event_id, ACTIVITY_STOP);
}

GATOR_DEFINE_PROBE(mali_total_alloc_pages_change, TP_PROTO(long long int event_id))
{
	accumulators_data[TOTAL_ALLOC_PAGES] = event_id;
}

static int create_files(struct super_block *sb, struct dentry *root)
{
	int event;
	/*
                                        
  */
	int counter_index = 0;
	mali_profiling_control_type *mali_control;

	for (event = FIRST_TIMELINE_EVENT; event < FIRST_TIMELINE_EVENT + NUMBER_OF_TIMELINE_EVENTS; event++) {
		if (gator_mali_create_file_system(mali_name, timeline_event_names[counter_index], sb, root, &counters[event], NULL) != 0)
			return -1;
		counter_index++;
	}
	counter_index = 0;
	for (event = FIRST_SOFTWARE_COUNTER; event < FIRST_SOFTWARE_COUNTER + NUMBER_OF_SOFTWARE_COUNTERS; event++) {
		if (gator_mali_create_file_system(mali_name, software_counter_names[counter_index], sb, root, &counters[event], NULL) != 0)
			return -1;
		counter_index++;
	}
	counter_index = 0;
	for (event = FIRST_ACCUMULATOR; event < FIRST_ACCUMULATOR + NUMBER_OF_ACCUMULATORS; event++) {
		if (gator_mali_create_file_system(mali_name, accumulators_names[counter_index], sb, root, &counters[event], NULL) != 0)
			return -1;
		counter_index++;
	}

	mali_control = symbol_get(_mali_profiling_control);
	if (mali_control) {
		if (gator_mali_create_file_system(mali_name, "Filmstrip_cnt0", sb, root, &counters[FILMSTRIP], &filmstrip_event) != 0)
			return -1;
		symbol_put(_mali_profiling_control);
	}

	return 0;
}

static int register_tracepoints(void)
{
	if (GATOR_REGISTER_TRACE(mali_pm_status)) {
		pr_debug("gator: Mali-Midgard: mali_pm_status tracepoint failed to activate\n");
		return 0;
	}

	if (GATOR_REGISTER_TRACE(mali_page_fault_insert_pages)) {
		pr_debug("gator: Mali-Midgard: mali_page_fault_insert_pages tracepoint failed to activate\n");
		return 0;
	}

	if (GATOR_REGISTER_TRACE(mali_mmu_as_in_use)) {
		pr_debug("gator: Mali-Midgard: mali_mmu_as_in_use tracepoint failed to activate\n");
		return 0;
	}

	if (GATOR_REGISTER_TRACE(mali_mmu_as_released)) {
		pr_debug("gator: Mali-Midgard: mali_mmu_as_released tracepoint failed to activate\n");
		return 0;
	}

	if (GATOR_REGISTER_TRACE(mali_total_alloc_pages_change)) {
		pr_debug("gator: Mali-Midgard: mali_total_alloc_pages_change tracepoint failed to activate\n");
		return 0;
	}

	pr_debug("gator: Mali-Midgard: start\n");
	pr_debug("gator: Mali-Midgard: mali_pm_status probe is at %p\n", &probe_mali_pm_status);
	pr_debug("gator: Mali-Midgard: mali_page_fault_insert_pages probe is at %p\n", &probe_mali_page_fault_insert_pages);
	pr_debug("gator: Mali-Midgard: mali_mmu_as_in_use probe is at %p\n", &probe_mali_mmu_as_in_use);
	pr_debug("gator: Mali-Midgard: mali_mmu_as_released probe is at %p\n", &probe_mali_mmu_as_released);
	pr_debug("gator: Mali-Midgard: mali_total_alloc_pages_change probe is at %p\n", &probe_mali_total_alloc_pages_change);

	return 1;
}

static int start(void)
{
	unsigned int cnt;
	mali_profiling_control_type *mali_control;

	/*                                     */
	for (cnt = 0; cnt < NUMBER_OF_TIMELINE_EVENTS; cnt++) {
		timeline_event_starttime[cnt].tv_sec = timeline_event_starttime[cnt].tv_nsec = 0;
		timeline_data[cnt] = 0;
	}

	for (cnt = 0; cnt < NUMBER_OF_SOFTWARE_COUNTERS; cnt++)
		sw_counter_data[cnt] = 0;

	for (cnt = 0; cnt < NUMBER_OF_ACCUMULATORS; cnt++)
		accumulators_data[cnt] = 0;

	/*                      */
	if (register_tracepoints() == 0)
		return -1;

	/*                                         */
	mali_control = symbol_get(_mali_profiling_control);
	if (mali_control) {
		/*                                                                  */
		unsigned int enabled = counters[FILMSTRIP].enabled ? 1 : 0;
		unsigned int rate = filmstrip_event & 0xff;
		unsigned int resize_factor = (filmstrip_event >> 8) & 0xff;

		pr_debug("gator: mali online _mali_profiling_control symbol @ %p\n", mali_control);

#define FBDUMP_CONTROL_ENABLE (1)
#define FBDUMP_CONTROL_RATE (2)
#define FBDUMP_CONTROL_RESIZE_FACTOR (4)
		mali_control(FBDUMP_CONTROL_ENABLE, enabled);
		mali_control(FBDUMP_CONTROL_RATE, rate);
		mali_control(FBDUMP_CONTROL_RESIZE_FACTOR, resize_factor);

		pr_debug("gator: sent mali_control enabled=%d, rate=%d, resize_factor=%d\n", enabled, rate, resize_factor);

		symbol_put(_mali_profiling_control);
	} else {
		pr_err("gator: mali online _mali_profiling_control symbol not found\n");
	}

	/*
                                                                                                        
                                                                   
                                                                                         
  */
	getnstimeofday(&prev_timestamp);

	return 0;
}

static void stop(void)
{
	mali_profiling_control_type *mali_control;

	pr_debug("gator: Mali-Midgard: stop\n");

	/*
                                                                      
                                    
  */
	GATOR_UNREGISTER_TRACE(mali_pm_status);
	pr_debug("gator: Mali-Midgard: mali_pm_status tracepoint deactivated\n");

	GATOR_UNREGISTER_TRACE(mali_page_fault_insert_pages);
	pr_debug("gator: Mali-Midgard: mali_page_fault_insert_pages tracepoint deactivated\n");

	GATOR_UNREGISTER_TRACE(mali_mmu_as_in_use);
	pr_debug("gator: Mali-Midgard: mali_mmu_as_in_use tracepoint deactivated\n");

	GATOR_UNREGISTER_TRACE(mali_mmu_as_released);
	pr_debug("gator: Mali-Midgard: mali_mmu_as_released tracepoint deactivated\n");

	GATOR_UNREGISTER_TRACE(mali_total_alloc_pages_change);
	pr_debug("gator: Mali-Midgard: mali_total_alloc_pages_change tracepoint deactivated\n");

	/*                                         */
	mali_control = symbol_get(_mali_profiling_control);
	if (mali_control) {
		pr_debug("gator: mali offline _mali_profiling_control symbol @ %p\n", mali_control);

		mali_control(FBDUMP_CONTROL_ENABLE, 0);

		symbol_put(_mali_profiling_control);
	} else {
		pr_err("gator: mali offline _mali_profiling_control symbol not found\n");
	}
}

static int read(int **buffer, bool sched_switch)
{
	int cnt;
	int len = 0;
	long sample_interval_us = 0;
	struct timespec read_timestamp;

	if (!on_primary_core())
		return 0;

	/*                                      */
	getnstimeofday(&read_timestamp);

	/*
                                                                       
                                         
  */
	if (prev_timestamp.tv_sec != 0)
		sample_interval_us = get_duration_us(&prev_timestamp, &read_timestamp);

	/*                                                */
	prev_timestamp = read_timestamp;

	/*
                                                      
  */
	for (cnt = FIRST_TIMELINE_EVENT; cnt < (FIRST_TIMELINE_EVENT + NUMBER_OF_TIMELINE_EVENTS); cnt++) {
		struct mali_counter *counter = &counters[cnt];

		if (counter->enabled) {
			const int index = cnt - FIRST_TIMELINE_EVENT;
			unsigned int value;

			/*                                                              
                                                               
                                                      
    */
			if (timeline_event_starttime[index].tv_sec != 0) {
				const long event_duration = get_duration_us(&timeline_event_starttime[index], &read_timestamp);

				timeline_data[index] += event_duration;
				timeline_event_starttime[index] = read_timestamp;	/*                            */
			}

			if (sample_interval_us != 0) {
				/*                                                  */
				value = (timeline_data[index] * 100) / sample_interval_us;
			} else {
				pr_debug("gator: Mali-Midgard: setting value to zero\n");
				value = 0;
			}

			/*                                                    */
			timeline_data[index] = 0;

			counter_dump[len++] = counter->key;
			counter_dump[len++] = value;
		}
	}

	/*                              */
	for (cnt = FIRST_SOFTWARE_COUNTER; cnt < (FIRST_SOFTWARE_COUNTER + NUMBER_OF_SOFTWARE_COUNTERS); cnt++) {
		const struct mali_counter *counter = &counters[cnt];

		if (counter->enabled) {
			const int index = cnt - FIRST_SOFTWARE_COUNTER;

			counter_dump[len++] = counter->key;
			counter_dump[len++] = sw_counter_data[index];
			/*                                         */
			sw_counter_data[index] = 0;
		}
	}

	/*                         */
	for (cnt = FIRST_ACCUMULATOR; cnt < (FIRST_ACCUMULATOR + NUMBER_OF_ACCUMULATORS); cnt++) {
		const struct mali_counter *counter = &counters[cnt];

		if (counter->enabled) {
			const int index = cnt - FIRST_ACCUMULATOR;

			counter_dump[len++] = counter->key;
			counter_dump[len++] = accumulators_data[index];
			/*                             */
		}
	}

	/*                   */
	if (buffer)
		*buffer = counter_dump;

	return len;
}

static struct gator_interface gator_events_mali_midgard_interface = {
	.create_files = create_files,
	.start = start,
	.stop = stop,
	.read = read
};

extern int gator_events_mali_midgard_init(void)
{
	pr_debug("gator: Mali-Midgard: sw_counters init\n");

	gator_mali_initialise_counters(counters, NUMBER_OF_EVENTS);

	return gator_events_install(&gator_events_mali_midgard_interface);
}
