/*
 * Watchdog timer for PowerPC Book-E systems
 *
 * Author: Matthew McClintock
 * Maintainer: Kumar Gala <galak@kernel.crashing.org>
 *
 * Copyright 2005, 2008, 2010-2011 Freescale Semiconductor Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/smp.h>
#include <linux/watchdog.h>

#include <asm/reg_booke.h>
#include <asm/time.h>
#include <asm/div64.h>

/*                                                                     
                                                               
                                                                      
                                                                           
                                                                            
                                                  
 */

u32 booke_wdt_enabled;
u32 booke_wdt_period = CONFIG_BOOKE_WDT_DEFAULT_TIMEOUT;

#ifdef	CONFIG_PPC_FSL_BOOK3E
#define WDTP(x)		((((x)&0x3)<<30)|(((x)&0x3c)<<15))
#define WDTP_MASK	(WDTP(0x3f))
#else
#define WDTP(x)		(TCR_WP(x))
#define WDTP_MASK	(TCR_WP_MASK)
#endif

#ifdef CONFIG_PPC_FSL_BOOK3E

/*                                                          
                                                             
                                               
  
                                             
                                          
  
                                                        
                                                              
 */
static unsigned long long period_to_sec(unsigned int period)
{
	unsigned long long tmp = 1ULL << (64 - period);
	unsigned long tmp2 = ppc_tb_freq;

	/*                                                              
                                                          
  */
	tmp2 = tmp2 / 5 * 2;

	do_div(tmp, tmp2);
	return tmp;
}

/*
                                                                        
                                                                      
                                                                            
 */
static unsigned int sec_to_period(unsigned int secs)
{
	unsigned int period;
	for (period = 63; period > 0; period--) {
		if (period_to_sec(period) >= secs)
			return period;
	}
	return 0;
}

#define MAX_WDT_TIMEOUT		period_to_sec(1)

#else /*                       */

static unsigned long long period_to_sec(unsigned int period)
{
	return period;
}

static unsigned int sec_to_period(unsigned int secs)
{
	return secs;
}

#define MAX_WDT_TIMEOUT		3	/*              */

#endif /*                        */

static void __booke_wdt_set(void *data)
{
	u32 val;

	val = mfspr(SPRN_TCR);
	val &= ~WDTP_MASK;
	val |= WDTP(booke_wdt_period);

	mtspr(SPRN_TCR, val);
}

static void booke_wdt_set(void)
{
	on_each_cpu(__booke_wdt_set, NULL, 0);
}

static void __booke_wdt_ping(void *data)
{
	mtspr(SPRN_TSR, TSR_ENW|TSR_WIS);
}

static int booke_wdt_ping(struct watchdog_device *wdog)
{
	on_each_cpu(__booke_wdt_ping, NULL, 0);

	return 0;
}

static void __booke_wdt_enable(void *data)
{
	u32 val;

	/*                                       */
	__booke_wdt_ping(NULL);
	val = mfspr(SPRN_TCR);
	val &= ~WDTP_MASK;
	val |= (TCR_WIE|TCR_WRC(WRC_CHIP)|WDTP(booke_wdt_period));

	mtspr(SPRN_TCR, val);
}

/* 
                                                            
  
                                                                              
  
                                                                          
                                                                               
 */
static void __booke_wdt_disable(void *data)
{
	u32 val;

	val = mfspr(SPRN_TCR);
	val &= ~(TCR_WIE | WDTP_MASK);
	mtspr(SPRN_TCR, val);

	/*                                              */
	__booke_wdt_ping(NULL);

}

static void __booke_wdt_start(struct watchdog_device *wdog)
{
	on_each_cpu(__booke_wdt_enable, NULL, 0);
	pr_debug("watchdog enabled (timeout = %u sec)\n", wdog->timeout);
}

static int booke_wdt_start(struct watchdog_device *wdog)
{
	if (booke_wdt_enabled == 0) {
		booke_wdt_enabled = 1;
		__booke_wdt_start(wdog);
	}
	return 0;
}

static int booke_wdt_stop(struct watchdog_device *wdog)
{
	on_each_cpu(__booke_wdt_disable, NULL, 0);
	booke_wdt_enabled = 0;
	pr_debug("watchdog disabled\n");

	return 0;
}

static int booke_wdt_set_timeout(struct watchdog_device *wdt_dev,
				 unsigned int timeout)
{
	if (timeout > MAX_WDT_TIMEOUT)
		return -EINVAL;
	booke_wdt_period = sec_to_period(timeout);
	wdt_dev->timeout = timeout;
	booke_wdt_set();

	return 0;
}

static struct watchdog_info booke_wdt_info = {
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING,
	.identity = "PowerPC Book-E Watchdog",
};

static struct watchdog_ops booke_wdt_ops = {
	.owner = THIS_MODULE,
	.start = booke_wdt_start,
	.stop = booke_wdt_stop,
	.ping = booke_wdt_ping,
	.set_timeout = booke_wdt_set_timeout,
};

static struct watchdog_device booke_wdt_dev = {
	.info = &booke_wdt_info,
	.ops = &booke_wdt_ops,
	.min_timeout = 1,
	.max_timeout = 0xFFFF
};

static void __exit booke_wdt_exit(void)
{
	watchdog_unregister_device(&booke_wdt_dev);
}

static int __init booke_wdt_init(void)
{
	int ret = 0;
	bool nowayout = WATCHDOG_NOWAYOUT;

	pr_info("powerpc book-e watchdog driver loaded\n");
	booke_wdt_info.firmware_version = cur_cpu_spec->pvr_value;
	booke_wdt_set_timeout(&booke_wdt_dev,
			      period_to_sec(CONFIG_BOOKE_WDT_DEFAULT_TIMEOUT));
	watchdog_set_nowayout(&booke_wdt_dev, nowayout);
	if (booke_wdt_enabled)
		__booke_wdt_start(&booke_wdt_dev);

	ret = watchdog_register_device(&booke_wdt_dev);

	return ret;
}

module_init(booke_wdt_init);
module_exit(booke_wdt_exit);

MODULE_DESCRIPTION("PowerPC Book-E watchdog driver");
MODULE_LICENSE("GPL");
