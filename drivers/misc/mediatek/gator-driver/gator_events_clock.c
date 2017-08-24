/*
 * Example events provider
 *
 * Copyright (C) ARM Limited 2010-2014. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Similar entries to those below must be present in the events.xml file.
 * To add them to the events.xml, create an events-mmap.xml with the
 * following contents and rebuild gatord:
 *
 * <category name="MT6732Clock">
 *   <event counter="clock_cnt0" title="MT6732 Clock" name="EMI Memory" display="maximum" class="absolute" units="Hz" description="Frequency setting of the Memory"/>
 * </category>
 *
 * When adding custom events, be sure to do the following:
 * - add any needed .c files to the gator driver Makefile
 * - call gator_events_install in the events init function
 * - add the init function to GATOR_EVENTS_LIST in gator_main.c
 * - add a new events-*.xml file to the gator daemon and rebuild
 *
 * Troubleshooting:
 * - verify the new events are part of events.xml, which is created when building the daemon
 * - verify the new events exist at /dev/gator/events/ once gatord is launched
 * - verify the counter name in the XML matches the name at /dev/gator/events
 */

#include <linux/init.h>
#include <linux/io.h>
#include <linux/ratelimit.h>

#include "gator.h"

enum {
	EMI_CLOCK = 0,
	GPU_CLOCK = 1,
	CLOCK_COUNTERS_NUM,
};

static int clock_global_enabled;

static struct {
	unsigned long enabled;
	unsigned long key;
} clock_counters[CLOCK_COUNTERS_NUM];

extern int get_dramc_dfs_status(void);
extern unsigned int mt_get_emi_freq(void);

static int clock_buffer[CLOCK_COUNTERS_NUM * 2];

/*                                                                                    */
static int gator_events_clock_create_files(struct super_block *sb,
					     struct dentry *root)
{
	int i;

	for (i = 0; i < CLOCK_COUNTERS_NUM; i++) {
		char buf[16];
		struct dentry *dir;

		snprintf(buf, sizeof(buf), "clock_cnt%d", i);
		dir = gatorfs_mkdir(sb, root, buf);
		if (WARN_ON(!dir))
			return -1;
		gatorfs_create_ulong(sb, dir, "enabled",
				     &clock_counters[i].enabled);
		gatorfs_create_ro_ulong(sb, dir, "key",
					&clock_counters[i].key);
	}

	return 0;
}

static int gator_events_clock_start(void)
{
	int i;

	clock_global_enabled = 0;
	for (i = 0; i < CLOCK_COUNTERS_NUM; i++) {
		if (clock_counters[i].enabled) {
			clock_global_enabled = 1;
			break;
		}
	}

	return 0;
}

static void gator_events_clock_stop(void)
{
}

/*                                             */
extern unsigned int mt_gpufreq_get_dvfs_table_num(void);
extern unsigned int mt_gpufreq_get_cur_freq_index(void);

static unsigned int _mtk_gpu_dvfs_index_to_frequency(int iFreq)
{
    unsigned int iCurrentFreqCount;
    iCurrentFreqCount =mt_gpufreq_get_dvfs_table_num();
    if(iCurrentFreqCount == 6) //       
    {
        switch(iFreq)
        {
            case 0:
                return 7280000;
            case 1:
                return 6500000;
            case 2:
                return 5980000;
            case 3:
                return 5200000;
            case 4:
                return 4160000;
            case 5:
                return 3120000;
        }
    }
    else if(iCurrentFreqCount == 4) //                    
    {
        switch(iFreq)
        {
            case 0:
                return 5980000;
            case 1:
                return 5200000;
            case 2:
                return 4160000;
            case 3:
                return 3120000;
        }
    }
    else if(iCurrentFreqCount == 3) //              
    {
        switch(iFreq)
        {
            case 0:
                return 4940000;
            case 1:
                return 4160000;
            case 2:
                return 3120000;
        }
    }

}

static int get_mali_gpu_frequency(void)
{

    unsigned int iCurrentFreq;

    iCurrentFreq = mt_gpufreq_get_cur_freq_index();

    return _mtk_gpu_dvfs_index_to_frequency(iCurrentFreq);
}

/*                                                               */

static int get_clock_value(int clock_type)
{
	int result = 0;

	switch (clock_type) {
	case EMI_CLOCK:		/*                       */
		{
			//                                     
			result = get_dramc_dfs_status() ? 400000000 : 200000000;

		}
		break;
	case GPU_CLOCK:		/*                       */
		{
			result = get_mali_gpu_frequency()*100;
		}
		break;
	default:		/*                        */
		{
			result = 0;
		}
		break;
	}

	return result;
}

static int gator_events_clock_read(int **buffer, bool sched_switch)
{
	int i;
	int len = 0;

	/*                                                */
	if (!on_primary_core() || !clock_global_enabled)
		return 0;

	for (i = 0; i < CLOCK_COUNTERS_NUM; i++) {
		if (clock_counters[i].enabled) {
			clock_buffer[len++] = clock_counters[i].key;
			clock_buffer[len++] =
			    get_clock_value(i);
		}
	}

	if (buffer)
		*buffer = clock_buffer;

	return len;
}

static struct gator_interface gator_events_clock_interface = {
	.create_files = gator_events_clock_create_files,
	.start = gator_events_clock_start,
	.stop = gator_events_clock_stop,
	.read = gator_events_clock_read,
};

/*                     */
int __init gator_events_clock_init(void)
{
	int i;

	for (i = 0; i < CLOCK_COUNTERS_NUM; i++) {
		clock_counters[i].enabled = 0;
		clock_counters[i].key = gator_events_get_key();
	}

	return gator_events_install(&gator_events_clock_interface);
}
