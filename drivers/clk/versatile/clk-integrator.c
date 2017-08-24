/*
 * Clock driver for the ARM Integrator/AP and Integrator/CP boards
 * Copyright (C) 2012 Linus Walleij
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/err.h>
#include <linux/platform_data/clk-integrator.h>

#include <mach/hardware.h>
#include <mach/platform.h>

#include "clk-icst.h"

/*
                                                                        
                           
                                                                 
 */

static const struct icst_params cp_auxvco_params = {
	.ref		= 24000000,
	.vco_max	= ICST525_VCO_MAX_5V,
	.vco_min	= ICST525_VCO_MIN,
	.vd_min 	= 8,
	.vd_max 	= 263,
	.rd_min 	= 3,
	.rd_max 	= 65,
	.s2div		= icst525_s2div,
	.idx2s		= icst525_idx2s,
};

static const struct clk_icst_desc __initdata cp_icst_desc = {
	.params = &cp_auxvco_params,
	.vco_offset = 0x1c,
	.lock_offset = INTEGRATOR_HDR_LOCK_OFFSET,
};

/*
                                                           
                                                                 
 */
void __init integrator_clk_init(bool is_cp)
{
	struct clk *clk;

	/*                 */
	clk = clk_register_fixed_rate(NULL, "apb_pclk", NULL, CLK_IS_ROOT, 0);
	clk_register_clkdev(clk, "apb_pclk", NULL);

	/*                      */
	clk = clk_register_fixed_rate(NULL, "uartclk", NULL, CLK_IS_ROOT,
				14745600);
	clk_register_clkdev(clk, NULL, "uart0");
	clk_register_clkdev(clk, NULL, "uart1");
	if (is_cp)
		clk_register_clkdev(clk, NULL, "mmci");

	/*              */
	clk = clk_register_fixed_rate(NULL, "clk24mhz", NULL, CLK_IS_ROOT,
				24000000);
	clk_register_clkdev(clk, NULL, "kmi0");
	clk_register_clkdev(clk, NULL, "kmi1");
	if (!is_cp)
		clk_register_clkdev(clk, NULL, "ap_timer");

	if (!is_cp)
		return;

	/*             */
	clk = clk_register_fixed_rate(NULL, "clk1mhz", NULL, CLK_IS_ROOT,
				1000000);
	clk_register_clkdev(clk, NULL, "sp804");

	/*                                               */
	clk = icst_clk_register(NULL, &cp_icst_desc,
				__io_address(INTEGRATOR_HDR_BASE));
	clk_register_clkdev(clk, NULL, "clcd");
}
