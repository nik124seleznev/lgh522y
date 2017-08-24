/*
 * linux/arch/arm/mach-omap2/timer.c
 *
 * OMAP2 GP timer support.
 *
 * Copyright (C) 2009 Nokia Corporation
 *
 * Update to use new clocksource/clockevent layers
 * Author: Kevin Hilman, MontaVista Software, Inc. <source@mvista.com>
 * Copyright (C) 2007 MontaVista Software, Inc.
 *
 * Original driver:
 * Copyright (C) 2005 Nokia Corporation
 * Author: Paul Mundt <paul.mundt@nokia.com>
 *         Juha Yrjölä <juha.yrjola@nokia.com>
 * OMAP Dual-mode timer framework support by Timo Teras
 *
 * Some parts based off of TI's 24xx code:
 *
 * Copyright (C) 2004-2009 Texas Instruments, Inc.
 *
 * Roughly modelled after the OMAP1 MPU timer code.
 * Added OMAP4 support - Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License. See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/platform_data/dmtimer-omap.h>

#include <asm/mach/time.h>
#include <asm/smp_twd.h>
#include <asm/sched_clock.h>

#include "omap_hwmod.h"
#include "omap_device.h"
#include <plat/counter-32k.h>
#include <plat/dmtimer.h>
#include "omap-pm.h"

#include "soc.h"
#include "common.h"
#include "powerdomain.h"

#define REALTIME_COUNTER_BASE				0x48243200
#define INCREMENTER_NUMERATOR_OFFSET			0x10
#define INCREMENTER_DENUMERATOR_RELOAD_OFFSET		0x14
#define NUMERATOR_DENUMERATOR_MASK			0xfffff000

/*                 */

static struct omap_dm_timer clkev;
static struct clock_event_device clockevent_gpt;

static irqreturn_t omap2_gp_timer_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *evt = &clockevent_gpt;

	__omap_dm_timer_write_status(&clkev, OMAP_TIMER_INT_OVERFLOW);

	evt->event_handler(evt);
	return IRQ_HANDLED;
}

static struct irqaction omap2_gp_timer_irq = {
	.name		= "gp_timer",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= omap2_gp_timer_interrupt,
};

static int omap2_gp_timer_set_next_event(unsigned long cycles,
					 struct clock_event_device *evt)
{
	__omap_dm_timer_load_start(&clkev, OMAP_TIMER_CTRL_ST,
				   0xffffffff - cycles, OMAP_TIMER_POSTED);

	return 0;
}

static void omap2_gp_timer_set_mode(enum clock_event_mode mode,
				    struct clock_event_device *evt)
{
	u32 period;

	__omap_dm_timer_stop(&clkev, OMAP_TIMER_POSTED, clkev.rate);

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		period = clkev.rate / HZ;
		period -= 1;
		/*                                                           */
		__omap_dm_timer_write(&clkev, OMAP_TIMER_LOAD_REG,
				      0xffffffff - period, OMAP_TIMER_POSTED);
		__omap_dm_timer_load_start(&clkev,
					OMAP_TIMER_CTRL_AR | OMAP_TIMER_CTRL_ST,
					0xffffffff - period, OMAP_TIMER_POSTED);
		break;
	case CLOCK_EVT_MODE_ONESHOT:
		break;
	case CLOCK_EVT_MODE_UNUSED:
	case CLOCK_EVT_MODE_SHUTDOWN:
	case CLOCK_EVT_MODE_RESUME:
		break;
	}
}

static struct clock_event_device clockevent_gpt = {
	.features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
	.rating		= 300,
	.set_next_event	= omap2_gp_timer_set_next_event,
	.set_mode	= omap2_gp_timer_set_mode,
};

static struct property device_disabled = {
	.name = "status",
	.length = sizeof("disabled"),
	.value = "disabled",
};

static struct of_device_id omap_timer_match[] __initdata = {
	{ .compatible = "ti,omap2420-timer", },
	{ .compatible = "ti,omap3430-timer", },
	{ .compatible = "ti,omap4430-timer", },
	{ .compatible = "ti,omap5430-timer", },
	{ .compatible = "ti,am335x-timer", },
	{ .compatible = "ti,am335x-timer-1ms", },
	{ }
};

/* 
                                                    
                                                                  
                                               
  
                                                                             
                                                                           
                                                                           
                                                                        
                                                                             
 */
static struct device_node * __init omap_get_timer_dt(struct of_device_id *match,
						     const char *property)
{
	struct device_node *np;

	for_each_matching_node(np, match) {
		if (!of_device_is_available(np))
			continue;

		if (property && !of_get_property(np, property, NULL))
			continue;

		if (!property && (of_get_property(np, "ti,timer-alwon", NULL) ||
				  of_get_property(np, "ti,timer-dsp", NULL) ||
				  of_get_property(np, "ti,timer-pwm", NULL) ||
				  of_get_property(np, "ti,timer-secure", NULL)))
			continue;

		of_add_property(np, &device_disabled);
		return np;
	}

	return NULL;
}

/* 
                                                                       
  
                                                                          
                                                                        
                                                                           
                
 */
static void __init omap_dmtimer_init(void)
{
	struct device_node *np;

	if (!cpu_is_omap34xx())
		return;

	/*                                                          */
	if ((omap_type() != OMAP2_DEVICE_TYPE_GP)) {
		np = omap_get_timer_dt(omap_timer_match, "ti,timer-secure");
		if (np)
			of_node_put(np);
	}
}

/* 
                                                          
  
                                                                              
 */
static u32 __init omap_dm_timer_get_errata(void)
{
	if (cpu_is_omap24xx())
		return 0;

	return OMAP_TIMER_ERRATA_I103_I767;
}

static int __init omap_dm_timer_init_one(struct omap_dm_timer *timer,
					 const char *fck_source,
					 const char *property,
					 const char **timer_name,
					 int posted)
{
	char name[10]; /*                           */
	const char *oh_name;
	struct device_node *np;
	struct omap_hwmod *oh;
	struct resource irq, mem;
	struct clk *src;
	int r = 0;

	if (of_have_populated_dt()) {
		np = omap_get_timer_dt(omap_timer_match, property);
		if (!np)
			return -ENODEV;

		of_property_read_string_index(np, "ti,hwmods", 0, &oh_name);
		if (!oh_name)
			return -ENODEV;

		timer->irq = irq_of_parse_and_map(np, 0);
		if (!timer->irq)
			return -ENXIO;

		timer->io_base = of_iomap(np, 0);

		of_node_put(np);
	} else {
		if (omap_dm_timer_reserve_systimer(timer->id))
			return -ENODEV;

		sprintf(name, "timer%d", timer->id);
		oh_name = name;
	}

	oh = omap_hwmod_lookup(oh_name);
	if (!oh)
		return -ENODEV;

	*timer_name = oh->name;

	if (!of_have_populated_dt()) {
		r = omap_hwmod_get_resource_byname(oh, IORESOURCE_IRQ, NULL,
						   &irq);
		if (r)
			return -ENXIO;
		timer->irq = irq.start;

		r = omap_hwmod_get_resource_byname(oh, IORESOURCE_MEM, NULL,
						   &mem);
		if (r)
			return -ENXIO;

		/*                                */
		timer->io_base = ioremap(mem.start, mem.end - mem.start);
	}

	if (!timer->io_base)
		return -ENXIO;

	/*                                                               */
	timer->fclk = clk_get(NULL, omap_hwmod_get_main_clk(oh));
	if (IS_ERR(timer->fclk))
		return PTR_ERR(timer->fclk);

	src = clk_get(NULL, fck_source);
	if (IS_ERR(src))
		return PTR_ERR(src);

	if (clk_get_parent(timer->fclk) != src) {
		r = clk_set_parent(timer->fclk, src);
		if (r < 0) {
			pr_warn("%s: %s cannot set source\n", __func__,
				oh->name);
			clk_put(src);
			return r;
		}
	}

	clk_put(src);

	omap_hwmod_setup_one(oh_name);
	omap_hwmod_enable(oh);
	__omap_dm_timer_init_regs(timer);

	if (posted)
		__omap_dm_timer_enable_posted(timer);

	/*                                                                 */
	if (posted != timer->posted)
		return -EINVAL;

	timer->rate = clk_get_rate(timer->fclk);
	timer->reserved = 1;

	return r;
}

static void __init omap2_gp_clockevent_init(int gptimer_id,
						const char *fck_source,
						const char *property)
{
	int res;

	clkev.id = gptimer_id;
	clkev.errata = omap_dm_timer_get_errata();

	/*
                                                              
                                                              
                                                            
  */
	__omap_dm_timer_override_errata(&clkev, OMAP_TIMER_ERRATA_I103_I767);

	res = omap_dm_timer_init_one(&clkev, fck_source, property,
				     &clockevent_gpt.name, OMAP_TIMER_POSTED);
	BUG_ON(res);

	omap2_gp_timer_irq.dev_id = &clkev;
	setup_irq(clkev.irq, &omap2_gp_timer_irq);

	__omap_dm_timer_int_enable(&clkev, OMAP_TIMER_INT_OVERFLOW);

	clockevent_gpt.cpumask = cpu_possible_mask;
	clockevent_gpt.irq = omap_dm_timer_get_irq(&clkev);
	clockevents_config_and_register(&clockevent_gpt, clkev.rate,
					3, /*                                */
					0xffffffff);

	pr_info("OMAP clockevent source: %s at %lu Hz\n", clockevent_gpt.name,
		clkev.rate);
}

/*                  */
static struct omap_dm_timer clksrc;
static bool use_gptimer_clksrc;

/*
              
 */
static cycle_t clocksource_read_cycles(struct clocksource *cs)
{
	return (cycle_t)__omap_dm_timer_read_counter(&clksrc,
						     OMAP_TIMER_NONPOSTED);
}

static struct clocksource clocksource_gpt = {
	.rating		= 300,
	.read		= clocksource_read_cycles,
	.mask		= CLOCKSOURCE_MASK(32),
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};

static u32 notrace dmtimer_read_sched_clock(void)
{
	if (clksrc.reserved)
		return __omap_dm_timer_read_counter(&clksrc,
						    OMAP_TIMER_NONPOSTED);

	return 0;
}

static struct of_device_id omap_counter_match[] __initdata = {
	{ .compatible = "ti,omap-counter32k", },
	{ }
};

/*                                            */
static int __init __maybe_unused omap2_sync32k_clocksource_init(void)
{
	int ret;
	struct device_node *np = NULL;
	struct omap_hwmod *oh;
	void __iomem *vbase;
	const char *oh_name = "counter_32k";

	/*
                                                      
                                             
  */
	if (of_have_populated_dt()) {
		np = omap_get_timer_dt(omap_counter_match, NULL);
		if (!np)
			return -ENODEV;

		of_property_read_string_index(np, "ti,hwmods", 0, &oh_name);
		if (!oh_name)
			return -ENODEV;
	}

	/*
                                                           
  */
	oh = omap_hwmod_lookup(oh_name);
	if (!oh || oh->slaves_cnt == 0)
		return -ENODEV;

	omap_hwmod_setup_one(oh_name);

	if (np) {
		vbase = of_iomap(np, 0);
		of_node_put(np);
	} else {
		vbase = omap_hwmod_get_mpu_rt_va(oh);
	}

	if (!vbase) {
		pr_warn("%s: failed to get counter_32k resource\n", __func__);
		return -ENXIO;
	}

	ret = omap_hwmod_enable(oh);
	if (ret) {
		pr_warn("%s: failed to enable counter_32k module (%d)\n",
							__func__, ret);
		return ret;
	}

	ret = omap_init_clocksource_32k(vbase);
	if (ret) {
		pr_warn("%s: failed to initialize counter_32k as a clocksource (%d)\n",
							__func__, ret);
		omap_hwmod_idle(oh);
	}

	return ret;
}

static void __init omap2_gptimer_clocksource_init(int gptimer_id,
						  const char *fck_source,
						  const char *property)
{
	int res;

	clksrc.id = gptimer_id;
	clksrc.errata = omap_dm_timer_get_errata();

	res = omap_dm_timer_init_one(&clksrc, fck_source, property,
				     &clocksource_gpt.name,
				     OMAP_TIMER_NONPOSTED);
	BUG_ON(res);

	__omap_dm_timer_load_start(&clksrc,
				   OMAP_TIMER_CTRL_ST | OMAP_TIMER_CTRL_AR, 0,
				   OMAP_TIMER_NONPOSTED);
	setup_sched_clock(dmtimer_read_sched_clock, 32, clksrc.rate);

	if (clocksource_register_hz(&clocksource_gpt, clksrc.rate))
		pr_err("Could not register clocksource %s\n",
			clocksource_gpt.name);
	else
		pr_info("OMAP clocksource: %s at %lu Hz\n",
			clocksource_gpt.name, clksrc.rate);
}

#ifdef CONFIG_SOC_HAS_REALTIME_COUNTER
/*
                                                                     
                                                                     
                                                                          
                                                                          
                                                                        
                                                                       
                                  
 */
static void __init realtime_counter_init(void)
{
	void __iomem *base;
	static struct clk *sys_clk;
	unsigned long rate;
	unsigned int reg, num, den;

	base = ioremap(REALTIME_COUNTER_BASE, SZ_32);
	if (!base) {
		pr_err("%s: ioremap failed\n", __func__);
		return;
	}
	sys_clk = clk_get(NULL, "sys_clkin");
	if (IS_ERR(sys_clk)) {
		pr_err("%s: failed to get system clock handle\n", __func__);
		iounmap(base);
		return;
	}

	rate = clk_get_rate(sys_clk);
	/*                                                                 */
	switch (rate) {
	case 1200000:
		num = 64;
		den = 125;
		break;
	case 1300000:
		num = 768;
		den = 1625;
		break;
	case 19200000:
		num = 8;
		den = 25;
		break;
	case 2600000:
		num = 384;
		den = 1625;
		break;
	case 2700000:
		num = 256;
		den = 1125;
		break;
	case 38400000:
	default:
		/*                         */
		num = 4;
		den = 25;
		break;
	}

	/*                                             */
	reg = __raw_readl(base + INCREMENTER_NUMERATOR_OFFSET) &
			NUMERATOR_DENUMERATOR_MASK;
	reg |= num;
	__raw_writel(reg, base + INCREMENTER_NUMERATOR_OFFSET);

	reg = __raw_readl(base + INCREMENTER_NUMERATOR_OFFSET) &
			NUMERATOR_DENUMERATOR_MASK;
	reg |= den;
	__raw_writel(reg, base + INCREMENTER_DENUMERATOR_RELOAD_OFFSET);

	iounmap(base);
}
#else
static inline void __init realtime_counter_init(void)
{}
#endif

#define OMAP_SYS_GP_TIMER_INIT(name, clkev_nr, clkev_src, clkev_prop,	\
			       clksrc_nr, clksrc_src, clksrc_prop)	\
void __init omap##name##_gptimer_timer_init(void)			\
{									\
	if (omap_clk_init)						\
		omap_clk_init();					\
	omap_dmtimer_init();						\
	omap2_gp_clockevent_init((clkev_nr), clkev_src, clkev_prop);	\
	omap2_gptimer_clocksource_init((clksrc_nr), clksrc_src,		\
					clksrc_prop);			\
}

#define OMAP_SYS_32K_TIMER_INIT(name, clkev_nr, clkev_src, clkev_prop,	\
				clksrc_nr, clksrc_src, clksrc_prop)	\
void __init omap##name##_sync32k_timer_init(void)		\
{									\
	if (omap_clk_init)						\
		omap_clk_init();					\
	omap_dmtimer_init();						\
	omap2_gp_clockevent_init((clkev_nr), clkev_src, clkev_prop);	\
	/*                                                           */	\
	if (use_gptimer_clksrc)						\
		omap2_gptimer_clocksource_init((clksrc_nr), clksrc_src,	\
						clksrc_prop);		\
	else								\
		omap2_sync32k_clocksource_init();			\
}

#ifdef CONFIG_ARCH_OMAP2
OMAP_SYS_32K_TIMER_INIT(2, 1, "timer_32k_ck", "ti,timer-alwon",
			2, "timer_sys_ck", NULL);
#endif /*                   */

#ifdef CONFIG_ARCH_OMAP3
OMAP_SYS_32K_TIMER_INIT(3, 1, "timer_32k_ck", "ti,timer-alwon",
			2, "timer_sys_ck", NULL);
OMAP_SYS_32K_TIMER_INIT(3_secure, 12, "secure_32k_fck", "ti,timer-secure",
			2, "timer_sys_ck", NULL);
#endif /*                   */

#if defined(CONFIG_ARCH_OMAP3) || defined(CONFIG_SOC_AM33XX)
OMAP_SYS_GP_TIMER_INIT(3, 2, "timer_sys_ck", NULL,
		       1, "timer_sys_ck", "ti,timer-alwon");
#endif

#if defined(CONFIG_ARCH_OMAP4) || defined(CONFIG_SOC_OMAP5)
static OMAP_SYS_32K_TIMER_INIT(4, 1, "timer_32k_ck", "ti,timer-alwon",
			       2, "sys_clkin_ck", NULL);
#endif

#ifdef CONFIG_ARCH_OMAP4
#ifdef CONFIG_LOCAL_TIMERS
static DEFINE_TWD_LOCAL_TIMER(twd_local_timer, OMAP44XX_LOCAL_TWD_BASE, 29);
void __init omap4_local_timer_init(void)
{
	omap4_sync32k_timer_init();
	/*                                                 */
	if (omap_rev() != OMAP4430_REV_ES1_0) {
		int err;

		if (of_have_populated_dt()) {
			clocksource_of_init();
			return;
		}

		err = twd_local_timer_register(&twd_local_timer);
		if (err)
			pr_err("twd_local_timer_register failed %d\n", err);
	}
}
#else /*                     */
void __init omap4_local_timer_init(void)
{
	omap4_sync32k_timer_init();
}
#endif /*                     */
#endif /*                   */

#ifdef CONFIG_SOC_OMAP5
void __init omap5_realtime_timer_init(void)
{
	omap4_sync32k_timer_init();
	realtime_counter_init();

	clocksource_of_init();
}
#endif /*                  */

/* 
                                                            
                         
                                                            
                                                             
  
                                                                       
                                                                      
                                                                       
                                                                      
                                                                
 */
static int __init omap_timer_init(struct omap_hwmod *oh, void *unused)
{
	int id;
	int ret = 0;
	char *name = "omap_timer";
	struct dmtimer_platform_data *pdata;
	struct platform_device *pdev;
	struct omap_timer_capability_dev_attr *timer_dev_attr;

	pr_debug("%s: %s\n", __func__, oh->name);

	/*                                                */
	timer_dev_attr = oh->dev_attr;
	if (omap_type() != OMAP2_DEVICE_TYPE_GP && timer_dev_attr)
		if (timer_dev_attr->timer_capability == OMAP_TIMER_SECURE)
			return ret;

	pdata = kzalloc(sizeof(*pdata), GFP_KERNEL);
	if (!pdata) {
		pr_err("%s: No memory for [%s]\n", __func__, oh->name);
		return -ENOMEM;
	}

	/*
                                                     
                                                  
                                                     
                                                      
                                                    
                                                        
                                                     
                                          
  */
	sscanf(oh->name, "timer%2d", &id);

	if (timer_dev_attr)
		pdata->timer_capability = timer_dev_attr->timer_capability;

	pdata->timer_errata = omap_dm_timer_get_errata();
	pdata->get_context_loss_count = omap_pm_get_dev_context_loss_count;

	pdev = omap_device_build(name, id, oh, pdata, sizeof(*pdata));

	if (IS_ERR(pdev)) {
		pr_err("%s: Can't build omap_device for %s: %s.\n",
			__func__, name, oh->name);
		ret = -EINVAL;
	}

	kfree(pdata);

	return ret;
}

/* 
                                                                
  
                                                               
                                                                 
 */
static int __init omap2_dm_timer_init(void)
{
	int ret;

	/*                                                          */
	if (of_have_populated_dt())
		return -ENODEV;

	ret = omap_hwmod_for_each_by_class("timer", omap_timer_init, NULL);
	if (unlikely(ret)) {
		pr_err("%s: device registration failed.\n", __func__);
		return -EINVAL;
	}

	return 0;
}
omap_arch_initcall(omap2_dm_timer_init);

/* 
                                                                            
  
                                                                      
                                                             
  
                                                                              
                                                   
 */
static int __init omap2_override_clocksource(char *str)
{
	if (!str)
		return 0;
	/*
                                                   
                           
                                 
  */
	if (!strcmp(str, "gp_timer"))
		use_gptimer_clksrc = true;

	return 0;
}
early_param("clocksource", omap2_override_clocksource);
