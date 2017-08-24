/*
 * arch/arm/mach-spear13xx/spear13xx.c
 *
 * SPEAr13XX machines common source file
 *
 * Copyright (C) 2012 ST Microelectronics
 * Viresh Kumar <viresh.linux@gmail.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#define pr_fmt(fmt) "SPEAr13xx: " fmt

#include <linux/amba/pl022.h>
#include <linux/clk.h>
#include <linux/clocksource.h>
#include <linux/err.h>
#include <linux/of.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/mach/map.h>
#include <mach/spear.h>
#include "generic.h"

void __init spear13xx_l2x0_init(void)
{
	/*
                                                           
   
                                                                
                                                       
   
                                                             
                                               
   
  */
	if (!IS_ENABLED(CONFIG_CACHE_L2X0))
		return;

	writel_relaxed(0x06, VA_L2CC_BASE + L2X0_PREFETCH_CTRL);

	/*
                                                
                             
  */
	writel_relaxed(0x221, VA_L2CC_BASE + L2X0_TAG_LATENCY_CTRL);
	writel_relaxed(0x441, VA_L2CC_BASE + L2X0_DATA_LATENCY_CTRL);
	l2x0_init(VA_L2CC_BASE, 0x70A60001, 0xfe00ffff);
}

/*
                                                              
                    
                         
                         
                         
                         
 */
struct map_desc spear13xx_io_desc[] __initdata = {
	{
		.virtual	= (unsigned long)VA_PERIP_GRP2_BASE,
		.pfn		= __phys_to_pfn(PERIP_GRP2_BASE),
		.length		= SZ_16M,
		.type		= MT_DEVICE
	}, {
		.virtual	= (unsigned long)VA_PERIP_GRP1_BASE,
		.pfn		= __phys_to_pfn(PERIP_GRP1_BASE),
		.length		= SZ_16M,
		.type		= MT_DEVICE
	}, {
		.virtual	= (unsigned long)VA_A9SM_AND_MPMC_BASE,
		.pfn		= __phys_to_pfn(A9SM_AND_MPMC_BASE),
		.length		= SZ_16M,
		.type		= MT_DEVICE
	}, {
		.virtual	= (unsigned long)VA_L2CC_BASE,
		.pfn		= __phys_to_pfn(L2CC_BASE),
		.length		= SZ_4K,
		.type		= MT_DEVICE
	},
};

/*                                                             */
void __init spear13xx_map_io(void)
{
	iotable_init(spear13xx_io_desc, ARRAY_SIZE(spear13xx_io_desc));
}

static void __init spear13xx_clk_init(void)
{
	if (of_machine_is_compatible("st,spear1310"))
		spear1310_clk_init(VA_MISC_BASE, VA_SPEAR1310_RAS_BASE);
	else if (of_machine_is_compatible("st,spear1340"))
		spear1340_clk_init(VA_MISC_BASE);
	else
		pr_err("%s: Unknown machine\n", __func__);
}

void __init spear13xx_timer_init(void)
{
	char pclk_name[] = "osc_24m_clk";
	struct clk *gpt_clk, *pclk;

	spear13xx_clk_init();

	/*                            */
	gpt_clk = clk_get_sys("gpt0", NULL);
	if (IS_ERR(gpt_clk)) {
		pr_err("%s:couldn't get clk for gpt\n", __func__);
		BUG();
	}

	/*                                        */
	pclk = clk_get(NULL, pclk_name);
	if (IS_ERR(pclk)) {
		pr_err("%s:couldn't get %s as parent for gpt\n", __func__,
				pclk_name);
		BUG();
	}

	clk_set_parent(gpt_clk, pclk);
	clk_put(gpt_clk);
	clk_put(pclk);

	spear_setup_of_timer();
	clocksource_of_init();
}