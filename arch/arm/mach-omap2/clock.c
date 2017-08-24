/*
 *  linux/arch/arm/mach-omap2/clock.c
 *
 *  Copyright (C) 2005-2008 Texas Instruments, Inc.
 *  Copyright (C) 2004-2010 Nokia Corporation
 *
 *  Contacts:
 *  Richard Woodruff <r-woodruff2@ti.com>
 *  Paul Walmsley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#undef DEBUG

#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/clk-provider.h>
#include <linux/io.h>
#include <linux/bitops.h>
#include <linux/clk-private.h>
#include <asm/cpu.h>


#include <trace/events/power.h>

#include "soc.h"
#include "clockdomain.h"
#include "clock.h"
#include "cm.h"
#include "cm2xxx.h"
#include "cm3xxx.h"
#include "cm-regbits-24xx.h"
#include "cm-regbits-34xx.h"
#include "common.h"

/*
                                                                    
                                                        
 */
#define MAX_MODULE_ENABLE_WAIT		100000

u16 cpu_mask;

/*
                                                              
                                                                    
                                                                
              
 */
static bool clkdm_control = true;

static LIST_HEAD(clk_hw_omap_clocks);

/*
                                                                
                         
 */
unsigned long omap_fixed_divisor_recalc(struct clk_hw *hw,
		unsigned long parent_rate)
{
	struct clk_hw_omap *oclk;

	if (!hw) {
		pr_warn("%s: hw is NULL\n", __func__);
		return -EINVAL;
	}

	oclk = to_clk_hw_omap(hw);

	WARN_ON(!oclk->fixed_div);

	return parent_rate / oclk->fixed_div;
}

/*
                                  
 */

/*                   */


/* 
                                                                   
                                                  
                                                                    
                                                       
                                        
  
                                                                     
                                                               
                                                                     
                                                                  
                                                          
 */
static int _wait_idlest_generic(void __iomem *reg, u32 mask, u8 idlest,
				const char *name)
{
	int i = 0, ena = 0;

	ena = (idlest) ? 0 : mask;

	omap_test_timeout(((__raw_readl(reg) & mask) == ena),
			  MAX_MODULE_ENABLE_WAIT, i);

	if (i < MAX_MODULE_ENABLE_WAIT)
		pr_debug("omap clock: module associated with clock %s ready after %d loops\n",
			 name, i);
	else
		pr_err("omap clock: module associated with clock %s didn't enable in %d tries\n",
		       name, MAX_MODULE_ENABLE_WAIT);

	return (i < MAX_MODULE_ENABLE_WAIT) ? 1 : 0;
};

/* 
                                                                   
                                             
  
                                                              
                                                                     
                                                                
                                                                   
                                           
 */
static void _omap2_module_wait_ready(struct clk_hw_omap *clk)
{
	void __iomem *companion_reg, *idlest_reg;
	u8 other_bit, idlest_bit, idlest_val, idlest_reg_id;
	s16 prcm_mod;
	int r;

	/*                                                                   */
	if (clk->ops->find_companion) {
		clk->ops->find_companion(clk, &companion_reg, &other_bit);
		if (!(__raw_readl(companion_reg) & (1 << other_bit)))
			return;
	}

	clk->ops->find_idlest(clk, &idlest_reg, &idlest_bit, &idlest_val);
	r = cm_split_idlest_reg(idlest_reg, &prcm_mod, &idlest_reg_id);
	if (r) {
		/*                                      */
		_wait_idlest_generic(idlest_reg, (1 << idlest_bit), idlest_val,
				     __clk_get_name(clk->hw.clk));
	} else {
		cm_wait_module_ready(prcm_mod, idlest_reg_id, idlest_bit);
	};
}

/*                  */

/* 
                                                                          
                                     
  
                                                                 
                                                                        
                                                  
 */
void omap2_init_clk_clkdm(struct clk_hw *hw)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	struct clockdomain *clkdm;
	const char *clk_name;

	if (!clk->clkdm_name)
		return;

	clk_name = __clk_get_name(hw->clk);

	clkdm = clkdm_lookup(clk->clkdm_name);
	if (clkdm) {
		pr_debug("clock: associated clk %s to clkdm %s\n",
			 clk_name, clk->clkdm_name);
		clk->clkdm = clkdm;
	} else {
		pr_debug("clock: could not associate clk %s to clkdm %s\n",
			 clk_name, clk->clkdm_name);
	}
}

/* 
                                                                                
  
                                                                     
                                                                    
                                                                
                
 */
void __init omap2_clk_disable_clkdm_control(void)
{
	clkdm_control = false;
}

/* 
                                                               
                                                    
                                                                            
                                                               
  
                                                                  
                                                                   
               
  
                                                                   
                                    
  
                                                                      
                                                                     
                                                                      
                                                            
                                                                
                                                                 
                                                            
 */
void omap2_clk_dflt_find_companion(struct clk_hw_omap *clk,
			void __iomem **other_reg, u8 *other_bit)
{
	u32 r;

	/*
                                                               
                                          
  */
	r = ((__force u32)clk->enable_reg ^ (CM_FCLKEN ^ CM_ICLKEN));

	*other_reg = (__force void __iomem *)r;
	*other_bit = clk->enable_bit;
}

/* 
                                                                         
                                             
                                                             
                                                         
                                                        
  
                                                                    
                                                                   
                                                                    
                                                              
                                                            
                                                                    
 */
void omap2_clk_dflt_find_idlest(struct clk_hw_omap *clk,
		void __iomem **idlest_reg, u8 *idlest_bit, u8 *idlest_val)
{
	u32 r;

	r = (((__force u32)clk->enable_reg & ~0xf0) | 0x20);
	*idlest_reg = (__force void __iomem *)r;
	*idlest_bit = clk->enable_bit;

	/*
                                                               
                                                   
                                              
  */
	if (cpu_is_omap24xx())
		*idlest_val = OMAP24XX_CM_IDLEST_VAL;
	else if (cpu_is_omap34xx())
		*idlest_val = OMAP34XX_CM_IDLEST_VAL;
	else
		BUG();

}

/* 
                                                         
                                              
  
                                                                     
                                                                     
                                                                  
                                                                      
                                                                     
                                                                     
                                                            
 */
int omap2_dflt_clk_enable(struct clk_hw *hw)
{
	struct clk_hw_omap *clk;
	u32 v;
	int ret = 0;

	clk = to_clk_hw_omap(hw);

	if (clkdm_control && clk->clkdm) {
		ret = clkdm_clk_enable(clk->clkdm, hw->clk);
		if (ret) {
			WARN(1, "%s: could not enable %s's clockdomain %s: %d\n",
			     __func__, __clk_get_name(hw->clk),
			     clk->clkdm->name, ret);
			return ret;
		}
	}

	if (unlikely(clk->enable_reg == NULL)) {
		pr_err("%s: %s missing enable_reg\n", __func__,
		       __clk_get_name(hw->clk));
		ret = -EINVAL;
		goto err;
	}

	/*                                              */
	v = __raw_readl(clk->enable_reg);
	if (clk->flags & INVERT_ENABLE)
		v &= ~(1 << clk->enable_bit);
	else
		v |= (1 << clk->enable_bit);
	__raw_writel(v, clk->enable_reg);
	v = __raw_readl(clk->enable_reg); /*             */

	if (clk->ops && clk->ops->find_idlest)
		_omap2_module_wait_ready(clk);

	return 0;

err:
	if (clkdm_control && clk->clkdm)
		clkdm_clk_disable(clk->clkdm, hw->clk);
	return ret;
}

/* 
                                                           
                                               
  
                                                                
                                                                     
                                                                 
         
 */
void omap2_dflt_clk_disable(struct clk_hw *hw)
{
	struct clk_hw_omap *clk;
	u32 v;

	clk = to_clk_hw_omap(hw);
	if (!clk->enable_reg) {
		/*
                                                      
                              
   */
		pr_err("%s: independent clock %s has no enable_reg\n",
		       __func__, __clk_get_name(hw->clk));
		return;
	}

	v = __raw_readl(clk->enable_reg);
	if (clk->flags & INVERT_ENABLE)
		v |= (1 << clk->enable_bit);
	else
		v &= ~(1 << clk->enable_bit);
	__raw_writel(v, clk->enable_reg);
	/*                                                            */

	if (clkdm_control && clk->clkdm)
		clkdm_clk_disable(clk->clkdm, hw->clk);
}

/* 
                                                                 
                                                  
  
                                                                    
                                                                   
                                                                   
                                                                   
                                                              
                                                                    
                   
 */
int omap2_clkops_enable_clkdm(struct clk_hw *hw)
{
	struct clk_hw_omap *clk;
	int ret = 0;

	clk = to_clk_hw_omap(hw);

	if (unlikely(!clk->clkdm)) {
		pr_err("%s: %s: no clkdm set ?!\n", __func__,
		       __clk_get_name(hw->clk));
		return -EINVAL;
	}

	if (unlikely(clk->enable_reg))
		pr_err("%s: %s: should use dflt_clk_enable ?!\n", __func__,
		       __clk_get_name(hw->clk));

	if (!clkdm_control) {
		pr_err("%s: %s: clkfw-based clockdomain control disabled ?!\n",
		       __func__, __clk_get_name(hw->clk));
		return 0;
	}

	ret = clkdm_clk_enable(clk->clkdm, hw->clk);
	WARN(ret, "%s: could not enable %s's clockdomain %s: %d\n",
	     __func__, __clk_get_name(hw->clk), clk->clkdm->name, ret);

	return ret;
}

/* 
                                                                  
                                                   
  
                                                                    
                                                                    
                                                                          
                                              
 */
void omap2_clkops_disable_clkdm(struct clk_hw *hw)
{
	struct clk_hw_omap *clk;

	clk = to_clk_hw_omap(hw);

	if (unlikely(!clk->clkdm)) {
		pr_err("%s: %s: no clkdm set ?!\n", __func__,
		       __clk_get_name(hw->clk));
		return;
	}

	if (unlikely(clk->enable_reg))
		pr_err("%s: %s: should use dflt_clk_disable ?!\n", __func__,
		       __clk_get_name(hw->clk));

	if (!clkdm_control) {
		pr_err("%s: %s: clkfw-based clockdomain control disabled ?!\n",
		       __func__, __clk_get_name(hw->clk));
		return;
	}

	clkdm_clk_disable(clk->clkdm, hw->clk);
}

/* 
                                                                
                                
  
                                                             
                                                            
                                       
 */
int omap2_dflt_clk_is_enabled(struct clk_hw *hw)
{
	struct clk_hw_omap *clk = to_clk_hw_omap(hw);
	u32 v;

	v = __raw_readl(clk->enable_reg);

	if (clk->flags & INVERT_ENABLE)
		v ^= BIT(clk->enable_bit);

	v &= BIT(clk->enable_bit);

	return v ? 1 : 0;
}

static int __initdata mpurate;

/*
                                                    
                                                      
 */
static int __init omap_clk_setup(char *str)
{
	get_option(&str, &mpurate);

	if (!mpurate)
		return 1;

	if (mpurate < 1000)
		mpurate *= 1000000;

	return 1;
}
__setup("mpurate=", omap_clk_setup);

/* 
                                                           
                                   
  
                                                                    
                                                               
                                                                   
                
 */
void omap2_init_clk_hw_omap_clocks(struct clk *clk)
{
	struct clk_hw_omap *c;

	if (__clk_get_flags(clk) & CLK_IS_BASIC)
		return;

	c = to_clk_hw_omap(__clk_get_hw(clk));
	list_add(&c->node, &clk_hw_omap_clocks);
}

/* 
                                                                          
             
  
                                                                
                                                                     
                                                                      
                    
 */
int omap2_clk_enable_autoidle_all(void)
{
	struct clk_hw_omap *c;

	list_for_each_entry(c, &clk_hw_omap_clocks, node)
		if (c->ops && c->ops->allow_idle)
			c->ops->allow_idle(c);
	return 0;
}

/* 
                                                                            
             
  
                                                                 
                                                                     
                                                                      
                    
 */
int omap2_clk_disable_autoidle_all(void)
{
	struct clk_hw_omap *c;

	list_for_each_entry(c, &clk_hw_omap_clocks, node)
		if (c->ops && c->ops->deny_idle)
			c->ops->deny_idle(c);
	return 0;
}

/* 
                                                                   
                                                                  
                                                   
  
                                                                
                                                                   
                                                                     
                                                                 
 */
void omap2_clk_enable_init_clocks(const char **clk_names, u8 num_clocks)
{
	struct clk *init_clk;
	int i;

	for (i = 0; i < num_clocks; i++) {
		init_clk = clk_get(NULL, clk_names[i]);
		clk_prepare_enable(init_clk);
	}
}

const struct clk_hw_omap_ops clkhwops_wait = {
	.find_idlest	= omap2_clk_dflt_find_idlest,
	.find_companion	= omap2_clk_dflt_find_companion,
};

/* 
                                                       
                                                    
 */
void __init omap_clocks_register(struct omap_clk oclks[], int cnt)
{
	struct omap_clk *c;

	for (c = oclks; c < oclks + cnt; c++) {
		clkdev_add(&c->lk);
		if (!__clk_init(NULL, c->lk.clk))
			omap2_init_clk_hw_omap_clocks(c->lk.clk);
	}
}

/* 
                                                                               
                                                         
  
                                                                     
                                                          
                                                                   
                                                                      
                                                                    
                                                                      
                                                                     
                                                                    
                                                                      
                                      
 */
int __init omap2_clk_switch_mpurate_at_boot(const char *mpurate_ck_name)
{
	struct clk *mpurate_ck;
	int r;

	if (!mpurate)
		return -EINVAL;

	mpurate_ck = clk_get(NULL, mpurate_ck_name);
	if (WARN(IS_ERR(mpurate_ck), "Failed to get %s.\n", mpurate_ck_name))
		return -ENOENT;

	r = clk_set_rate(mpurate_ck, mpurate);
	if (r < 0) {
		WARN(1, "clock: %s: unable to set MPU rate to %d: %d\n",
		     mpurate_ck_name, mpurate, r);
		clk_put(mpurate_ck);
		return -EINVAL;
	}

	calibrate_delay();
	clk_put(mpurate_ck);

	return 0;
}

/* 
                                                                        
                                                            
                                                   
                                               
  
                                                                    
                                                                       
                                                                          
                                                                       
                                                 
 */
void __init omap2_clk_print_new_rates(const char *hfclkin_ck_name,
				      const char *core_ck_name,
				      const char *mpu_ck_name)
{
	struct clk *hfclkin_ck, *core_ck, *mpu_ck;
	unsigned long hfclkin_rate;

	mpu_ck = clk_get(NULL, mpu_ck_name);
	if (WARN(IS_ERR(mpu_ck), "clock: failed to get %s.\n", mpu_ck_name))
		return;

	core_ck = clk_get(NULL, core_ck_name);
	if (WARN(IS_ERR(core_ck), "clock: failed to get %s.\n", core_ck_name))
		return;

	hfclkin_ck = clk_get(NULL, hfclkin_ck_name);
	if (WARN(IS_ERR(hfclkin_ck), "Failed to get %s.\n", hfclkin_ck_name))
		return;

	hfclkin_rate = clk_get_rate(hfclkin_ck);

	pr_info("Switched to new clocking rate (Crystal/Core/MPU): %ld.%01ld/%ld/%ld MHz\n",
		(hfclkin_rate / 1000000), ((hfclkin_rate / 100000) % 10),
		(clk_get_rate(core_ck) / 1000000),
		(clk_get_rate(mpu_ck) / 1000000));
}
