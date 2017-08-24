/*
 * Copyright (c) 2012 Linaro : Daniel Lezcano <daniel.lezcano@linaro.org> (IBM)
 *
 * Based on the work of Rickard Andersson <rickard.andersson@stericsson.com>
 * and Jonas Aaberg <jonas.aberg@stericsson.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/cpuidle.h>
#include <linux/spinlock.h>
#include <linux/atomic.h>
#include <linux/smp.h>
#include <linux/mfd/dbx500-prcmu.h>
#include <linux/platform_data/arm-ux500-pm.h>

#include <asm/cpuidle.h>
#include <asm/proc-fns.h>

#include "db8500-regs.h"
#include "id.h"

static atomic_t master = ATOMIC_INIT(0);
static DEFINE_SPINLOCK(master_lock);

static inline int ux500_enter_idle(struct cpuidle_device *dev,
				   struct cpuidle_driver *drv, int index)
{
	int this_cpu = smp_processor_id();
	bool recouple = false;

	if (atomic_inc_return(&master) == num_online_cpus()) {

		/*                                                           
                                               */
		if (!spin_trylock(&master_lock))
			goto wfi;

		/*                                    */
		if (prcmu_gic_decouple()) {
			spin_unlock(&master_lock);
			goto out;
		}

		/*                                                    
              */
		recouple = true;

		/*                                                     
                                                          
                                          */
		if (!prcmu_is_cpu_in_wfi(this_cpu ? 0 : 1))
			goto out;

		/*                                                       
                          */
		if (prcmu_copy_gic_settings())
			goto out;

		/*                                       
                              */
		if (prcmu_gic_pending_irq())
			goto out;

		/*                   */
		if (prcmu_pending_irq())
			goto out;

		/*                                                       
                                                              
                               */
		if (prcmu_set_power_state(PRCMU_AP_IDLE, true, true))
			goto out;

		/*                                                    
                                         */
		recouple = false;

		spin_unlock(&master_lock);
	}
wfi:
	cpu_do_idle();
out:
	atomic_dec(&master);

	if (recouple) {
		prcmu_gic_recouple();
		spin_unlock(&master_lock);
	}

	return index;
}

static struct cpuidle_driver ux500_idle_driver = {
	.name = "ux500_idle",
	.owner = THIS_MODULE,
	.states = {
		ARM_CPUIDLE_WFI_STATE,
		{
			.enter		  = ux500_enter_idle,
			.exit_latency	  = 70,
			.target_residency = 260,
			.flags		  = CPUIDLE_FLAG_TIME_VALID |
			                    CPUIDLE_FLAG_TIMER_STOP,
			.name		  = "ApIdle",
			.desc		  = "ARM Retention",
		},
	},
	.safe_state_index = 0,
	.state_count = 2,
};

int __init ux500_idle_init(void)
{
	if (!(cpu_is_u8500_family() || cpu_is_ux540_family()))
		return -ENODEV;

	/*                           */
	prcmu_enable_wakeups(PRCMU_WAKEUP(ARM) | PRCMU_WAKEUP(RTC) |
			     PRCMU_WAKEUP(ABB));

	return cpuidle_register(&ux500_idle_driver, NULL);
}

device_initcall(ux500_idle_init);
