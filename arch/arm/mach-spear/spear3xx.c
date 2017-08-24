/*
 * arch/arm/mach-spear3xx/spear3xx.c
 *
 * SPEAr3XX machines common source file
 *
 * Copyright (C) 2009-2012 ST Microelectronics
 * Viresh Kumar <viresh.linux@gmail.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#define pr_fmt(fmt) "SPEAr3xx: " fmt

#include <linux/amba/pl022.h>
#include <linux/amba/pl080.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <asm/mach/map.h>
#include "pl080.h"
#include "generic.h"
#include <mach/spear.h>
#include <mach/misc_regs.h>

/*                         */
struct pl022_ssp_controller pl022_plat_data = {
	.bus_id = 0,
	.enable_dma = 1,
	.dma_filter = pl08x_filter_id,
	.dma_tx_param = "ssp0_tx",
	.dma_rx_param = "ssp0_rx",
	/*
                                                                         
                                                                        
                                                                       
                                                                        
                                                                     
                                                                      
                                                      
  */
	.num_chipselect = 2,
};

/*                          */
struct pl08x_platform_data pl080_plat_data = {
	.memcpy_channel = {
		.bus_id = "memcpy",
		.cctl_memcpy =
			(PL080_BSIZE_16 << PL080_CONTROL_SB_SIZE_SHIFT | \
			PL080_BSIZE_16 << PL080_CONTROL_DB_SIZE_SHIFT | \
			PL080_WIDTH_32BIT << PL080_CONTROL_SWIDTH_SHIFT | \
			PL080_WIDTH_32BIT << PL080_CONTROL_DWIDTH_SHIFT | \
			PL080_CONTROL_PROT_BUFF | PL080_CONTROL_PROT_CACHE | \
			PL080_CONTROL_PROT_SYS),
	},
	.lli_buses = PL08X_AHB1,
	.mem_buses = PL08X_AHB1,
	.get_signal = pl080_get_signal,
	.put_signal = pl080_put_signal,
};

/*
                                                              
                    
                         
                         
 */
struct map_desc spear3xx_io_desc[] __initdata = {
	{
		.virtual	= (unsigned long)VA_SPEAR_ICM1_2_BASE,
		.pfn		= __phys_to_pfn(SPEAR_ICM1_2_BASE),
		.length		= SZ_16M,
		.type		= MT_DEVICE
	}, {
		.virtual	= (unsigned long)VA_SPEAR_ICM3_SMI_CTRL_BASE,
		.pfn		= __phys_to_pfn(SPEAR_ICM3_SMI_CTRL_BASE),
		.length		= SZ_16M,
		.type		= MT_DEVICE
	},
};

/*                                                             */
void __init spear3xx_map_io(void)
{
	iotable_init(spear3xx_io_desc, ARRAY_SIZE(spear3xx_io_desc));
}

void __init spear3xx_timer_init(void)
{
	char pclk_name[] = "pll3_clk";
	struct clk *gpt_clk, *pclk;

	spear3xx_clk_init(MISC_BASE, VA_SPEAR320_SOC_CONFIG_BASE);

	/*                            */
	gpt_clk = clk_get_sys("gpt0", NULL);
	if (IS_ERR(gpt_clk)) {
		pr_err("%s:couldn't get clk for gpt\n", __func__);
		BUG();
	}

	/*                                        */
	pclk = clk_get(NULL, pclk_name);
	if (IS_ERR(pclk)) {
		pr_err("%s:couldn't get %s as parent for gpt\n",
				__func__, pclk_name);
		BUG();
	}

	clk_set_parent(gpt_clk, pclk);
	clk_put(gpt_clk);
	clk_put(pclk);

	spear_setup_of_timer();
}
