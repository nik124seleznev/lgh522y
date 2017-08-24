/*
 * OMAP3-specific clock framework functions
 *
 * Copyright (C) 2007-2008 Texas Instruments, Inc.
 * Copyright (C) 2007-2011 Nokia Corporation
 *
 * Paul Walmsley
 * Jouni Högander
 *
 * Parts of this code are based on code written by
 * Richard Woodruff, Tony Lindgren, Tuukka Tikkanen, Karthik Dasu,
 * Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#undef DEBUG

#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/io.h>

#include "clock.h"
#include "clock34xx.h"
#include "cm3xxx.h"
#include "cm-regbits-34xx.h"

/* 
                                                                  
                                   
                                                                   
                                                                     
                                                                
  
                                                                   
                                                              
                                                 
 */
static void omap3430es2_clk_ssi_find_idlest(struct clk_hw_omap *clk,
					    void __iomem **idlest_reg,
					    u8 *idlest_bit,
					    u8 *idlest_val)
{
	u32 r;

	r = (((__force u32)clk->enable_reg & ~0xf0) | 0x20);
	*idlest_reg = (__force void __iomem *)r;
	*idlest_bit = OMAP3430ES2_ST_SSI_IDLE_SHIFT;
	*idlest_val = OMAP34XX_CM_IDLEST_VAL;
}
const struct clk_hw_omap_ops clkhwops_omap3430es2_ssi_wait = {
	.find_idlest	= omap3430es2_clk_ssi_find_idlest,
	.find_companion	= omap2_clk_dflt_find_companion,
};

const struct clk_hw_omap_ops clkhwops_omap3430es2_iclk_ssi_wait = {
	.allow_idle	= omap2_clkt_iclk_allow_idle,
	.deny_idle	= omap2_clkt_iclk_deny_idle,
	.find_idlest	= omap3430es2_clk_ssi_find_idlest,
	.find_companion = omap2_clk_dflt_find_companion,
};

/* 
                                                                            
                                   
                                                                   
                                                                     
                                                                
  
                                                                
                                                                   
                                                                   
                                                                   
                                                             
                               
 */
static void omap3430es2_clk_dss_usbhost_find_idlest(struct clk_hw_omap *clk,
						    void __iomem **idlest_reg,
						    u8 *idlest_bit,
						    u8 *idlest_val)
{
	u32 r;

	r = (((__force u32)clk->enable_reg & ~0xf0) | 0x20);
	*idlest_reg = (__force void __iomem *)r;
	/*                             */
	*idlest_bit = OMAP3430ES2_ST_DSS_IDLE_SHIFT;
	*idlest_val = OMAP34XX_CM_IDLEST_VAL;
}

const struct clk_hw_omap_ops clkhwops_omap3430es2_dss_usbhost_wait = {
	.find_idlest	= omap3430es2_clk_dss_usbhost_find_idlest,
	.find_companion	= omap2_clk_dflt_find_companion,
};

const struct clk_hw_omap_ops clkhwops_omap3430es2_iclk_dss_usbhost_wait = {
	.allow_idle	= omap2_clkt_iclk_allow_idle,
	.deny_idle	= omap2_clkt_iclk_deny_idle,
	.find_idlest	= omap3430es2_clk_dss_usbhost_find_idlest,
	.find_companion	= omap2_clk_dflt_find_companion,
};

/* 
                                                                            
                                   
                                                                   
                                                                     
                                                                
  
                                                                  
                                                                    
                                                 
 */
static void omap3430es2_clk_hsotgusb_find_idlest(struct clk_hw_omap *clk,
						 void __iomem **idlest_reg,
						 u8 *idlest_bit,
						 u8 *idlest_val)
{
	u32 r;

	r = (((__force u32)clk->enable_reg & ~0xf0) | 0x20);
	*idlest_reg = (__force void __iomem *)r;
	*idlest_bit = OMAP3430ES2_ST_HSOTGUSB_IDLE_SHIFT;
	*idlest_val = OMAP34XX_CM_IDLEST_VAL;
}

const struct clk_hw_omap_ops clkhwops_omap3430es2_iclk_hsotgusb_wait = {
	.allow_idle	= omap2_clkt_iclk_allow_idle,
	.deny_idle	= omap2_clkt_iclk_deny_idle,
	.find_idlest	= omap3430es2_clk_hsotgusb_find_idlest,
	.find_companion	= omap2_clk_dflt_find_companion,
};

const struct clk_hw_omap_ops clkhwops_omap3430es2_hsotgusb_wait = {
	.find_idlest	= omap3430es2_clk_hsotgusb_find_idlest,
	.find_companion	= omap2_clk_dflt_find_companion,
};
