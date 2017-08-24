/*
 * linux/arch/arm/mach-omap2/cpuidle34xx.c
 *
 * OMAP3 CPU IDLE Routines
 *
 * Copyright (C) 2008 Texas Instruments, Inc.
 * Rajendra Nayak <rnayak@ti.com>
 *
 * Copyright (C) 2007 Texas Instruments, Inc.
 * Karthik Dasu <karthik-dp@ti.com>
 *
 * Copyright (C) 2006 Nokia Corporation
 * Tony Lindgren <tony@atomide.com>
 *
 * Copyright (C) 2005 Texas Instruments, Inc.
 * Richard Woodruff <r-woodruff2@ti.com>
 *
 * Based on pm.c for omap2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/sched.h>
#include <linux/cpuidle.h>
#include <linux/export.h>
#include <linux/cpu_pm.h>
#include <asm/cpuidle.h>

#include "powerdomain.h"
#include "clockdomain.h"

#include "pm.h"
#include "control.h"
#include "common.h"

/*                                                                     */
struct omap3_idle_statedata {
	u8 mpu_state;
	u8 core_state;
	u8 per_min_state;
	u8 flags;
};

static struct powerdomain *mpu_pd, *core_pd, *per_pd, *cam_pd;

/*
                                                            
  
                                                                       
                                                                          
                                                                           
                                                                       
                    
 */
#define OMAP_CPUIDLE_CX_NO_CLKDM_IDLE		BIT(0)

/*
                                                                   
                                  
 */
static struct omap3_idle_statedata omap3_idle_data[] = {
	{
		.mpu_state = PWRDM_POWER_ON,
		.core_state = PWRDM_POWER_ON,
		/*                                                    */
		.per_min_state = PWRDM_POWER_ON,
		.flags = OMAP_CPUIDLE_CX_NO_CLKDM_IDLE,
	},
	{
		.mpu_state = PWRDM_POWER_ON,
		.core_state = PWRDM_POWER_ON,
		.per_min_state = PWRDM_POWER_RET,
	},
	{
		.mpu_state = PWRDM_POWER_RET,
		.core_state = PWRDM_POWER_ON,
		.per_min_state = PWRDM_POWER_RET,
	},
	{
		.mpu_state = PWRDM_POWER_OFF,
		.core_state = PWRDM_POWER_ON,
		.per_min_state = PWRDM_POWER_RET,
	},
	{
		.mpu_state = PWRDM_POWER_RET,
		.core_state = PWRDM_POWER_RET,
		.per_min_state = PWRDM_POWER_OFF,
	},
	{
		.mpu_state = PWRDM_POWER_OFF,
		.core_state = PWRDM_POWER_RET,
		.per_min_state = PWRDM_POWER_OFF,
	},
	{
		.mpu_state = PWRDM_POWER_OFF,
		.core_state = PWRDM_POWER_OFF,
		.per_min_state = PWRDM_POWER_OFF,
	},
};

/* 
                                                                 
                       
                       
                                           
 */
static int omap3_enter_idle(struct cpuidle_device *dev,
			    struct cpuidle_driver *drv,
			    int index)
{
	struct omap3_idle_statedata *cx = &omap3_idle_data[index];

	if (omap_irq_pending() || need_resched())
		goto return_sleep_time;

	/*                  */
	if (cx->flags & OMAP_CPUIDLE_CX_NO_CLKDM_IDLE) {
		clkdm_deny_idle(mpu_pd->pwrdm_clkdms[0]);
	} else {
		pwrdm_set_next_pwrst(mpu_pd, cx->mpu_state);
		pwrdm_set_next_pwrst(core_pd, cx->core_state);
	}

	/*
                                                 
                         
  */
	if (cx->mpu_state == PWRDM_POWER_OFF)
		cpu_pm_enter();

	/*                 */
	omap_sram_idle();

	/*
                                                    
                
  */
	if (cx->mpu_state == PWRDM_POWER_OFF &&
	    pwrdm_read_prev_pwrst(mpu_pd) == PWRDM_POWER_OFF)
		cpu_pm_exit();

	/*                      */
	if (cx->flags & OMAP_CPUIDLE_CX_NO_CLKDM_IDLE)
		clkdm_allow_idle(mpu_pd->pwrdm_clkdms[0]);

return_sleep_time:

	return index;
}

/* 
                                             
                       
                       
                                              
  
                                                                       
                                                                           
                                                                          
  
                                                       
                                                 
 */
static int next_valid_state(struct cpuidle_device *dev,
			    struct cpuidle_driver *drv, int index)
{
	struct omap3_idle_statedata *cx = &omap3_idle_data[index];
	u32 mpu_deepest_state = PWRDM_POWER_RET;
	u32 core_deepest_state = PWRDM_POWER_RET;
	int idx;
	int next_index = 0; /*                         */

	if (enable_off_mode) {
		mpu_deepest_state = PWRDM_POWER_OFF;
		/*
                                                      
                                                              
                                   
   */
		if (!IS_PM34XX_ERRATUM(PM_SDRC_WAKEUP_ERRATUM_i583))
			core_deepest_state = PWRDM_POWER_OFF;
	}

	/*                                 */
	if ((cx->mpu_state >= mpu_deepest_state) &&
	    (cx->core_state >= core_deepest_state))
		return index;

	/*
                             
                                             
  */
	for (idx = index - 1; idx >= 0; idx--) {
		cx = &omap3_idle_data[idx];
		if ((cx->mpu_state >= mpu_deepest_state) &&
		    (cx->core_state >= core_deepest_state)) {
			next_index = idx;
			break;
		}
	}

	return next_index;
}

/* 
                                                    
                       
                       
                                                       
  
                                                                  
                                                
 */
static int omap3_enter_idle_bm(struct cpuidle_device *dev,
			       struct cpuidle_driver *drv,
			       int index)
{
	int new_state_idx, ret;
	u8 per_next_state, per_saved_state;
	struct omap3_idle_statedata *cx;

	/*
                                 
                                                 
  */
	if (pwrdm_read_pwrst(cam_pd) == PWRDM_POWER_ON)
		new_state_idx = drv->safe_state_index;
	else
		new_state_idx = next_valid_state(dev, drv, index);

	/*
                                                          
                                                            
                                                           
                                                         
                        
  */

	/*                   */
	cx = &omap3_idle_data[new_state_idx];

	per_next_state = pwrdm_read_next_pwrst(per_pd);
	per_saved_state = per_next_state;
	if (per_next_state < cx->per_min_state) {
		per_next_state = cx->per_min_state;
		pwrdm_set_next_pwrst(per_pd, per_next_state);
	}

	ret = omap3_enter_idle(dev, drv, new_state_idx);

	/*                                               */
	if (per_next_state != per_saved_state)
		pwrdm_set_next_pwrst(per_pd, per_saved_state);

	return ret;
}

static struct cpuidle_driver omap3_idle_driver = {
	.name             = "omap3_idle",
	.owner            = THIS_MODULE,
	.states = {
		{
			.enter		  = omap3_enter_idle_bm,
			.exit_latency	  = 2 + 2,
			.target_residency = 5,
			.flags		  = CPUIDLE_FLAG_TIME_VALID,
			.name		  = "C1",
			.desc		  = "MPU ON + CORE ON",
		},
		{
			.enter		  = omap3_enter_idle_bm,
			.exit_latency	  = 10 + 10,
			.target_residency = 30,
			.flags		  = CPUIDLE_FLAG_TIME_VALID,
			.name		  = "C2",
			.desc		  = "MPU ON + CORE ON",
		},
		{
			.enter		  = omap3_enter_idle_bm,
			.exit_latency	  = 50 + 50,
			.target_residency = 300,
			.flags		  = CPUIDLE_FLAG_TIME_VALID,
			.name		  = "C3",
			.desc		  = "MPU RET + CORE ON",
		},
		{
			.enter		  = omap3_enter_idle_bm,
			.exit_latency	  = 1500 + 1800,
			.target_residency = 4000,
			.flags		  = CPUIDLE_FLAG_TIME_VALID,
			.name		  = "C4",
			.desc		  = "MPU OFF + CORE ON",
		},
		{
			.enter		  = omap3_enter_idle_bm,
			.exit_latency	  = 2500 + 7500,
			.target_residency = 12000,
			.flags		  = CPUIDLE_FLAG_TIME_VALID,
			.name		  = "C5",
			.desc		  = "MPU RET + CORE RET",
		},
		{
			.enter		  = omap3_enter_idle_bm,
			.exit_latency	  = 3000 + 8500,
			.target_residency = 15000,
			.flags		  = CPUIDLE_FLAG_TIME_VALID,
			.name		  = "C6",
			.desc		  = "MPU OFF + CORE RET",
		},
		{
			.enter		  = omap3_enter_idle_bm,
			.exit_latency	  = 10000 + 30000,
			.target_residency = 30000,
			.flags		  = CPUIDLE_FLAG_TIME_VALID,
			.name		  = "C7",
			.desc		  = "MPU OFF + CORE OFF",
		},
	},
	.state_count = ARRAY_SIZE(omap3_idle_data),
	.safe_state_index = 0,
};

/*                  */

/* 
                                                
  
                                                             
                                          
 */
int __init omap3_idle_init(void)
{
	mpu_pd = pwrdm_lookup("mpu_pwrdm");
	core_pd = pwrdm_lookup("core_pwrdm");
	per_pd = pwrdm_lookup("per_pwrdm");
	cam_pd = pwrdm_lookup("cam_pwrdm");

	if (!mpu_pd || !core_pd || !per_pd || !cam_pd)
		return -ENODEV;

	return cpuidle_register(&omap3_idle_driver, NULL);
}
