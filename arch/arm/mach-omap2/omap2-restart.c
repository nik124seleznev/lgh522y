/*
 * omap2-restart.c - code common to all OMAP2xxx machines.
 *
 * Copyright (C) 2012 Texas Instruments
 * Paul Walmsley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/io.h>

#include "soc.h"
#include "common.h"
#include "prm2xxx.h"

/*
                                                                      
                                                       
 */
static struct clk *reset_virt_prcm_set_ck, *reset_sys_ck;

/*                 */

/* 
                                                                
  
                                                                    
                
 */
void omap2xxx_restart(char mode, const char *cmd)
{
	u32 rate;

	rate = clk_get_rate(reset_sys_ck);
	clk_set_rate(reset_virt_prcm_set_ck, rate);

	/*                                                           */

	omap2xxx_prm_dpll_reset(); /*               */
	while (1);
}

/* 
                                                                             
  
                                                                  
                                                                    
                           
 */
static int __init omap2xxx_common_look_up_clks_for_reset(void)
{
	reset_virt_prcm_set_ck = clk_get(NULL, "virt_prcm_set");
	if (IS_ERR(reset_virt_prcm_set_ck))
		return -EINVAL;

	reset_sys_ck = clk_get(NULL, "sys_ck");
	if (IS_ERR(reset_sys_ck))
		return -EINVAL;

	return 0;
}
omap_core_initcall(omap2xxx_common_look_up_clks_for_reset);
