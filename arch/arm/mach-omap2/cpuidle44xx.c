/*
 * OMAP4+ CPU idle Routines
 *
 * Copyright (C) 2011-2013 Texas Instruments, Inc.
 * Santosh Shilimkar <santosh.shilimkar@ti.com>
 * Rajendra Nayak <rnayak@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/sched.h>
#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
#include <linux/export.h>
#include <linux/clockchips.h>

#include <asm/cpuidle.h>
#include <asm/proc-fns.h>

#include "common.h"
#include "pm.h"
#include "prm.h"
#include "clockdomain.h"

/*                              */
struct idle_statedata {
	u32 cpu_state;
	u32 mpu_logic_state;
	u32 mpu_state;
};

static struct idle_statedata omap4_idle_data[] = {
	{
		.cpu_state = PWRDM_POWER_ON,
		.mpu_state = PWRDM_POWER_ON,
		.mpu_logic_state = PWRDM_POWER_RET,
	},
	{
		.cpu_state = PWRDM_POWER_OFF,
		.mpu_state = PWRDM_POWER_RET,
		.mpu_logic_state = PWRDM_POWER_RET,
	},
	{
		.cpu_state = PWRDM_POWER_OFF,
		.mpu_state = PWRDM_POWER_RET,
		.mpu_logic_state = PWRDM_POWER_OFF,
	},
};

static struct powerdomain *mpu_pd, *cpu_pd[NR_CPUS];
static struct clockdomain *cpu_clkdm[NR_CPUS];

static atomic_t abort_barrier;
static bool cpu_done[NR_CPUS];
static struct idle_statedata *state_ptr = &omap4_idle_data[0];

/*                   */

/* 
                                                                       
                       
                       
                                           
  
                                                                 
                                                      
                                                           
 */
static int omap_enter_idle_simple(struct cpuidle_device *dev,
			struct cpuidle_driver *drv,
			int index)
{
	omap_do_wfi();
	return index;
}

static int omap_enter_idle_coupled(struct cpuidle_device *dev,
			struct cpuidle_driver *drv,
			int index)
{
	struct idle_statedata *cx = state_ptr + index;
	int cpu_id = smp_processor_id();

	/*
                                                         
                                                       
                                                              
                                     
  */
	if (dev->cpu == 0 && cpumask_test_cpu(1, cpu_online_mask)) {
		while (pwrdm_read_pwrst(cpu_pd[1]) != PWRDM_POWER_OFF) {
			cpu_relax();

			/*
                                                   
                                             
                                                     
                                                
                           
    */
			if (cpu_done[1])
			    goto fail;

		}
	}

	clockevents_notify(CLOCK_EVT_NOTIFY_BROADCAST_ENTER, &cpu_id);

	/*
                                                 
                                               
  */
	cpu_pm_enter();

	if (dev->cpu == 0) {
		pwrdm_set_logic_retst(mpu_pd, cx->mpu_logic_state);
		omap_set_pwrdm_state(mpu_pd, cx->mpu_state);

		/*
                                                  
                                       
   */
		if ((cx->mpu_state == PWRDM_POWER_RET) &&
			(cx->mpu_logic_state == PWRDM_POWER_OFF))
				cpu_cluster_pm_enter();
	}

	omap4_enter_lowpower(dev->cpu, cx->cpu_state);
	cpu_done[dev->cpu] = true;

	/*                                        */
	if (dev->cpu == 0 && cpumask_test_cpu(1, cpu_online_mask)) {
		clkdm_wakeup(cpu_clkdm[1]);
		omap_set_pwrdm_state(cpu_pd[1], PWRDM_POWER_ON);
		clkdm_allow_idle(cpu_clkdm[1]);
	}

	/*
                                                   
                                
  */
	cpu_pm_exit();

	/*
                                                
                                         
  */
	if ((cx->mpu_state == PWRDM_POWER_RET) &&
		(cx->mpu_logic_state == PWRDM_POWER_OFF))
		cpu_cluster_pm_exit();

	clockevents_notify(CLOCK_EVT_NOTIFY_BROADCAST_EXIT, &cpu_id);

fail:
	cpuidle_coupled_parallel_barrier(dev, &abort_barrier);
	cpu_done[dev->cpu] = false;

	return index;
}

/*
                                                               
                                 
 */
static void omap_setup_broadcast_timer(void *arg)
{
	int cpu = smp_processor_id();
	clockevents_notify(CLOCK_EVT_NOTIFY_BROADCAST_ON, &cpu);
}

static struct cpuidle_driver omap4_idle_driver = {
	.name				= "omap4_idle",
	.owner				= THIS_MODULE,
	.states = {
		{
			/*                                 */
			.exit_latency = 2 + 2,
			.target_residency = 5,
			.flags = CPUIDLE_FLAG_TIME_VALID,
			.enter = omap_enter_idle_simple,
			.name = "C1",
			.desc = "CPUx ON, MPUSS ON"
		},
		{
			/*                                     */
			.exit_latency = 328 + 440,
			.target_residency = 960,
			.flags = CPUIDLE_FLAG_TIME_VALID | CPUIDLE_FLAG_COUPLED,
			.enter = omap_enter_idle_coupled,
			.name = "C2",
			.desc = "CPUx OFF, MPUSS CSWR",
		},
		{
			/*                                     */
			.exit_latency = 460 + 518,
			.target_residency = 1100,
			.flags = CPUIDLE_FLAG_TIME_VALID | CPUIDLE_FLAG_COUPLED,
			.enter = omap_enter_idle_coupled,
			.name = "C3",
			.desc = "CPUx OFF, MPUSS OSWR",
		},
	},
	.state_count = ARRAY_SIZE(omap4_idle_data),
	.safe_state_index = 0,
};

/*                  */

/* 
                                                 
  
                                                              
                                          
 */
int __init omap4_idle_init(void)
{
	mpu_pd = pwrdm_lookup("mpu_pwrdm");
	cpu_pd[0] = pwrdm_lookup("cpu0_pwrdm");
	cpu_pd[1] = pwrdm_lookup("cpu1_pwrdm");
	if ((!mpu_pd) || (!cpu_pd[0]) || (!cpu_pd[1]))
		return -ENODEV;

	cpu_clkdm[0] = clkdm_lookup("mpu0_clkdm");
	cpu_clkdm[1] = clkdm_lookup("mpu1_clkdm");
	if (!cpu_clkdm[0] || !cpu_clkdm[1])
		return -ENODEV;

	/*                                           */
	on_each_cpu(omap_setup_broadcast_timer, NULL, 1);

	return cpuidle_register(&omap4_idle_driver, cpu_online_mask);
}
