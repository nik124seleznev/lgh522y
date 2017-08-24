/*
 *  linux/arch/arm/mach-omap1/clock.c
 *
 *  Copyright (C) 2004 - 2005, 2009-2010 Nokia Corporation
 *  Written by Tuukka Tikkanen <tuukka.tikkanen@elektrobit.com>
 *
 *  Modified to use omap shared clock framework by
 *  Tony Lindgren <tony@atomide.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/clkdev.h>

#include <asm/mach-types.h>

#include <mach/hardware.h>

#include "soc.h"
#include "iomap.h"
#include "clock.h"
#include "opp.h"
#include "sram.h"

__u32 arm_idlect1_mask;
struct clk *api_ck_p, *ck_dpll1_p, *ck_ref_p;

static LIST_HEAD(clocks);
static DEFINE_MUTEX(clocks_mutex);
static DEFINE_SPINLOCK(clockfw_lock);

/*
                                 
 */

unsigned long omap1_uart_recalc(struct clk *clk)
{
	unsigned int val = __raw_readl(clk->enable_reg);
	return val & clk->enable_bit ? 48000000 : 12000000;
}

unsigned long omap1_sossi_recalc(struct clk *clk)
{
	u32 div = omap_readl(MOD_CONF_CTRL_1);

	div = (div >> 17) & 0x7;
	div++;

	return clk->parent->rate / div;
}

static void omap1_clk_allow_idle(struct clk *clk)
{
	struct arm_idlect1_clk * iclk = (struct arm_idlect1_clk *)clk;

	if (!(clk->flags & CLOCK_IDLE_CONTROL))
		return;

	if (iclk->no_idle_count > 0 && !(--iclk->no_idle_count))
		arm_idlect1_mask |= 1 << iclk->idlect_shift;
}

static void omap1_clk_deny_idle(struct clk *clk)
{
	struct arm_idlect1_clk * iclk = (struct arm_idlect1_clk *)clk;

	if (!(clk->flags & CLOCK_IDLE_CONTROL))
		return;

	if (iclk->no_idle_count++ == 0)
		arm_idlect1_mask &= ~(1 << iclk->idlect_shift);
}

static __u16 verify_ckctl_value(__u16 newval)
{
	/*                                                   
                                                  
                                                  
                   
                   
                      
   
                                             
                   
                      
   
                                                     
  */
	__u8 per_exp;
	__u8 lcd_exp;
	__u8 arm_exp;
	__u8 dsp_exp;
	__u8 tc_exp;
	__u8 dspmmu_exp;

	per_exp = (newval >> CKCTL_PERDIV_OFFSET) & 3;
	lcd_exp = (newval >> CKCTL_LCDDIV_OFFSET) & 3;
	arm_exp = (newval >> CKCTL_ARMDIV_OFFSET) & 3;
	dsp_exp = (newval >> CKCTL_DSPDIV_OFFSET) & 3;
	tc_exp = (newval >> CKCTL_TCDIV_OFFSET) & 3;
	dspmmu_exp = (newval >> CKCTL_DSPMMUDIV_OFFSET) & 3;

	if (dspmmu_exp < dsp_exp)
		dspmmu_exp = dsp_exp;
	if (dspmmu_exp > dsp_exp+1)
		dspmmu_exp = dsp_exp+1;
	if (tc_exp < arm_exp)
		tc_exp = arm_exp;
	if (tc_exp < dspmmu_exp)
		tc_exp = dspmmu_exp;
	if (tc_exp > lcd_exp)
		lcd_exp = tc_exp;
	if (tc_exp > per_exp)
		per_exp = tc_exp;

	newval &= 0xf000;
	newval |= per_exp << CKCTL_PERDIV_OFFSET;
	newval |= lcd_exp << CKCTL_LCDDIV_OFFSET;
	newval |= arm_exp << CKCTL_ARMDIV_OFFSET;
	newval |= dsp_exp << CKCTL_DSPDIV_OFFSET;
	newval |= tc_exp << CKCTL_TCDIV_OFFSET;
	newval |= dspmmu_exp << CKCTL_DSPMMUDIV_OFFSET;

	return newval;
}

static int calc_dsor_exp(struct clk *clk, unsigned long rate)
{
	/*                                                                   
                                                                    
                
   
                                                                    
                                                  
                                                  
                   
                   
                      
  */
	unsigned long realrate;
	struct clk * parent;
	unsigned  dsor_exp;

	parent = clk->parent;
	if (unlikely(parent == NULL))
		return -EIO;

	realrate = parent->rate;
	for (dsor_exp=0; dsor_exp<4; dsor_exp++) {
		if (realrate <= rate)
			break;

		realrate /= 2;
	}

	return dsor_exp;
}

unsigned long omap1_ckctl_recalc(struct clk *clk)
{
	/*                                             */
	int dsor = 1 << (3 & (omap_readw(ARM_CKCTL) >> clk->rate_offset));

	return clk->parent->rate / dsor;
}

unsigned long omap1_ckctl_recalc_dsp_domain(struct clk *clk)
{
	int dsor;

	/*                                            
   
                                             
                                   
                                                 
                                                      
  */
	omap1_clk_enable(api_ck_p);
	dsor = 1 << (3 & (__raw_readw(DSP_CKCTL) >> clk->rate_offset));
	omap1_clk_disable(api_ck_p);

	return clk->parent->rate / dsor;
}

/*                             */
int omap1_select_table_rate(struct clk *clk, unsigned long rate)
{
	/*                                                               */
	struct mpu_rate * ptr;
	unsigned long ref_rate;

	ref_rate = ck_ref_p->rate;

	for (ptr = omap1_rate_table; ptr->rate; ptr++) {
		if (!(ptr->flags & cpu_mask))
			continue;

		if (ptr->xtal != ref_rate)
			continue;

		/*                                           */
		if (ptr->rate <= rate)
			break;
	}

	if (!ptr->rate)
		return -EINVAL;

	/*
                                                       
                                                                
  */
	omap_sram_reprogram_clock(ptr->dpllctl_val, ptr->ckctl_val);

	/*                                                                   */
	ck_dpll1_p->rate = ptr->pll_rate;

	return 0;
}

int omap1_clk_set_rate_dsp_domain(struct clk *clk, unsigned long rate)
{
	int dsor_exp;
	u16 regval;

	dsor_exp = calc_dsor_exp(clk, rate);
	if (dsor_exp > 3)
		dsor_exp = -EINVAL;
	if (dsor_exp < 0)
		return dsor_exp;

	regval = __raw_readw(DSP_CKCTL);
	regval &= ~(3 << clk->rate_offset);
	regval |= dsor_exp << clk->rate_offset;
	__raw_writew(regval, DSP_CKCTL);
	clk->rate = clk->parent->rate / (1 << dsor_exp);

	return 0;
}

long omap1_clk_round_rate_ckctl_arm(struct clk *clk, unsigned long rate)
{
	int dsor_exp = calc_dsor_exp(clk, rate);
	if (dsor_exp < 0)
		return dsor_exp;
	if (dsor_exp > 3)
		dsor_exp = 3;
	return clk->parent->rate / (1 << dsor_exp);
}

int omap1_clk_set_rate_ckctl_arm(struct clk *clk, unsigned long rate)
{
	int dsor_exp;
	u16 regval;

	dsor_exp = calc_dsor_exp(clk, rate);
	if (dsor_exp > 3)
		dsor_exp = -EINVAL;
	if (dsor_exp < 0)
		return dsor_exp;

	regval = omap_readw(ARM_CKCTL);
	regval &= ~(3 << clk->rate_offset);
	regval |= dsor_exp << clk->rate_offset;
	regval = verify_ckctl_value(regval);
	omap_writew(regval, ARM_CKCTL);
	clk->rate = clk->parent->rate / (1 << dsor_exp);
	return 0;
}

long omap1_round_to_table_rate(struct clk *clk, unsigned long rate)
{
	/*                                              */
	struct mpu_rate * ptr;
	long highest_rate;
	unsigned long ref_rate;

	ref_rate = ck_ref_p->rate;

	highest_rate = -EINVAL;

	for (ptr = omap1_rate_table; ptr->rate; ptr++) {
		if (!(ptr->flags & cpu_mask))
			continue;

		if (ptr->xtal != ref_rate)
			continue;

		highest_rate = ptr->rate;

		/*                                           */
		if (ptr->rate <= rate)
			break;
	}

	return highest_rate;
}

static unsigned calc_ext_dsor(unsigned long rate)
{
	unsigned dsor;

	/*                                               
                       
   
                                       
                                    
                                                  
                                                                     
                    
  */
	for (dsor = 2; dsor < 96; ++dsor) {
		if ((dsor & 1) && dsor > 8)
			continue;
		if (rate >= 96000000 / dsor)
			break;
	}
	return dsor;
}

/*                         */
int omap1_set_uart_rate(struct clk *clk, unsigned long rate)
{
	unsigned int val;

	val = __raw_readl(clk->enable_reg);
	if (rate == 12000000)
		val &= ~(1 << clk->enable_bit);
	else if (rate == 48000000)
		val |= (1 << clk->enable_bit);
	else
		return -EINVAL;
	__raw_writel(val, clk->enable_reg);
	clk->rate = rate;

	return 0;
}

/*                                        */
int omap1_set_ext_clk_rate(struct clk *clk, unsigned long rate)
{
	unsigned dsor;
	__u16 ratio_bits;

	dsor = calc_ext_dsor(rate);
	clk->rate = 96000000 / dsor;
	if (dsor > 8)
		ratio_bits = ((dsor - 8) / 2 + 6) << 2;
	else
		ratio_bits = (dsor - 2) << 2;

	ratio_bits |= __raw_readw(clk->enable_reg) & ~0xfd;
	__raw_writew(ratio_bits, clk->enable_reg);

	return 0;
}

int omap1_set_sossi_rate(struct clk *clk, unsigned long rate)
{
	u32 l;
	int div;
	unsigned long p_rate;

	p_rate = clk->parent->rate;
	/*                                */
	div = (p_rate + rate - 1) / rate;
	div--;
	if (div < 0 || div > 7)
		return -EINVAL;

	l = omap_readl(MOD_CONF_CTRL_1);
	l &= ~(7 << 17);
	l |= div << 17;
	omap_writel(l, MOD_CONF_CTRL_1);

	clk->rate = p_rate / (div + 1);

	return 0;
}

long omap1_round_ext_clk_rate(struct clk *clk, unsigned long rate)
{
	return 96000000 / calc_ext_dsor(rate);
}

void omap1_init_ext_clk(struct clk *clk)
{
	unsigned dsor;
	__u16 ratio_bits;

	/*                                                                */
	ratio_bits = __raw_readw(clk->enable_reg) & ~1;
	__raw_writew(ratio_bits, clk->enable_reg);

	ratio_bits = (ratio_bits & 0xfc) >> 2;
	if (ratio_bits > 6)
		dsor = (ratio_bits - 6) * 2 + 8;
	else
		dsor = ratio_bits + 2;

	clk-> rate = 96000000 / dsor;
}

int omap1_clk_enable(struct clk *clk)
{
	int ret = 0;

	if (clk->usecount++ == 0) {
		if (clk->parent) {
			ret = omap1_clk_enable(clk->parent);
			if (ret)
				goto err;

			if (clk->flags & CLOCK_NO_IDLE_PARENT)
				omap1_clk_deny_idle(clk->parent);
		}

		ret = clk->ops->enable(clk);
		if (ret) {
			if (clk->parent)
				omap1_clk_disable(clk->parent);
			goto err;
		}
	}
	return ret;

err:
	clk->usecount--;
	return ret;
}

void omap1_clk_disable(struct clk *clk)
{
	if (clk->usecount > 0 && !(--clk->usecount)) {
		clk->ops->disable(clk);
		if (likely(clk->parent)) {
			omap1_clk_disable(clk->parent);
			if (clk->flags & CLOCK_NO_IDLE_PARENT)
				omap1_clk_allow_idle(clk->parent);
		}
	}
}

static int omap1_clk_enable_generic(struct clk *clk)
{
	__u16 regval16;
	__u32 regval32;

	if (unlikely(clk->enable_reg == NULL)) {
		printk(KERN_ERR "clock.c: Enable for %s without enable code\n",
		       clk->name);
		return -EINVAL;
	}

	if (clk->flags & ENABLE_REG_32BIT) {
		regval32 = __raw_readl(clk->enable_reg);
		regval32 |= (1 << clk->enable_bit);
		__raw_writel(regval32, clk->enable_reg);
	} else {
		regval16 = __raw_readw(clk->enable_reg);
		regval16 |= (1 << clk->enable_bit);
		__raw_writew(regval16, clk->enable_reg);
	}

	return 0;
}

static void omap1_clk_disable_generic(struct clk *clk)
{
	__u16 regval16;
	__u32 regval32;

	if (clk->enable_reg == NULL)
		return;

	if (clk->flags & ENABLE_REG_32BIT) {
		regval32 = __raw_readl(clk->enable_reg);
		regval32 &= ~(1 << clk->enable_bit);
		__raw_writel(regval32, clk->enable_reg);
	} else {
		regval16 = __raw_readw(clk->enable_reg);
		regval16 &= ~(1 << clk->enable_bit);
		__raw_writew(regval16, clk->enable_reg);
	}
}

const struct clkops clkops_generic = {
	.enable		= omap1_clk_enable_generic,
	.disable	= omap1_clk_disable_generic,
};

static int omap1_clk_enable_dsp_domain(struct clk *clk)
{
	int retval;

	retval = omap1_clk_enable(api_ck_p);
	if (!retval) {
		retval = omap1_clk_enable_generic(clk);
		omap1_clk_disable(api_ck_p);
	}

	return retval;
}

static void omap1_clk_disable_dsp_domain(struct clk *clk)
{
	if (omap1_clk_enable(api_ck_p) == 0) {
		omap1_clk_disable_generic(clk);
		omap1_clk_disable(api_ck_p);
	}
}

const struct clkops clkops_dspck = {
	.enable		= omap1_clk_enable_dsp_domain,
	.disable	= omap1_clk_disable_dsp_domain,
};

/*                                                                   */
static int omap1_clk_enable_uart_functional_16xx(struct clk *clk)
{
	int ret;
	struct uart_clk *uclk;

	ret = omap1_clk_enable_generic(clk);
	if (ret == 0) {
		/*                                     */
		uclk = (struct uart_clk *)clk;
		omap_writeb((omap_readb(uclk->sysc_addr) & ~0x10) | 8,
			    uclk->sysc_addr);
	}

	return ret;
}

/*                                                                   */
static void omap1_clk_disable_uart_functional_16xx(struct clk *clk)
{
	struct uart_clk *uclk;

	/*                                     */
	uclk = (struct uart_clk *)clk;
	omap_writeb((omap_readb(uclk->sysc_addr) & ~0x18), uclk->sysc_addr);

	omap1_clk_disable_generic(clk);
}

/*                                                                   */
const struct clkops clkops_uart_16xx = {
	.enable		= omap1_clk_enable_uart_functional_16xx,
	.disable	= omap1_clk_disable_uart_functional_16xx,
};

long omap1_clk_round_rate(struct clk *clk, unsigned long rate)
{
	if (clk->round_rate != NULL)
		return clk->round_rate(clk, rate);

	return clk->rate;
}

int omap1_clk_set_rate(struct clk *clk, unsigned long rate)
{
	int  ret = -EINVAL;

	if (clk->set_rate)
		ret = clk->set_rate(clk, rate);
	return ret;
}

/*
                                       
 */

#ifdef CONFIG_OMAP_RESET_CLOCKS

void omap1_clk_disable_unused(struct clk *clk)
{
	__u32 regval32;

	/*                                                             
                                   */
	if (clk->enable_reg == DSP_IDLECT2) {
		pr_info("Skipping reset check for DSP domain clock \"%s\"\n",
			clk->name);
		return;
	}

	/*                                */
	if (clk->flags & ENABLE_REG_32BIT)
		regval32 = __raw_readl(clk->enable_reg);
	else
		regval32 = __raw_readw(clk->enable_reg);

	if ((regval32 & (1 << clk->enable_bit)) == 0)
		return;

	printk(KERN_INFO "Disabling unused clock \"%s\"... ", clk->name);
	clk->ops->disable(clk);
	printk(" done\n");
}

#endif


int clk_enable(struct clk *clk)
{
	unsigned long flags;
	int ret;

	if (clk == NULL || IS_ERR(clk))
		return -EINVAL;

	spin_lock_irqsave(&clockfw_lock, flags);
	ret = omap1_clk_enable(clk);
	spin_unlock_irqrestore(&clockfw_lock, flags);

	return ret;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable(struct clk *clk)
{
	unsigned long flags;

	if (clk == NULL || IS_ERR(clk))
		return;

	spin_lock_irqsave(&clockfw_lock, flags);
	if (clk->usecount == 0) {
		pr_err("Trying disable clock %s with 0 usecount\n",
		       clk->name);
		WARN_ON(1);
		goto out;
	}

	omap1_clk_disable(clk);

out:
	spin_unlock_irqrestore(&clockfw_lock, flags);
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
	unsigned long flags;
	unsigned long ret;

	if (clk == NULL || IS_ERR(clk))
		return 0;

	spin_lock_irqsave(&clockfw_lock, flags);
	ret = clk->rate;
	spin_unlock_irqrestore(&clockfw_lock, flags);

	return ret;
}
EXPORT_SYMBOL(clk_get_rate);

/*
                                                          
 */

long clk_round_rate(struct clk *clk, unsigned long rate)
{
	unsigned long flags;
	long ret;

	if (clk == NULL || IS_ERR(clk))
		return 0;

	spin_lock_irqsave(&clockfw_lock, flags);
	ret = omap1_clk_round_rate(clk, rate);
	spin_unlock_irqrestore(&clockfw_lock, flags);

	return ret;
}
EXPORT_SYMBOL(clk_round_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	unsigned long flags;
	int ret = -EINVAL;

	if (clk == NULL || IS_ERR(clk))
		return ret;

	spin_lock_irqsave(&clockfw_lock, flags);
	ret = omap1_clk_set_rate(clk, rate);
	if (ret == 0)
		propagate_rate(clk);
	spin_unlock_irqrestore(&clockfw_lock, flags);

	return ret;
}
EXPORT_SYMBOL(clk_set_rate);

int clk_set_parent(struct clk *clk, struct clk *parent)
{
	WARN_ONCE(1, "clk_set_parent() not implemented for OMAP1\n");

	return -EINVAL;
}
EXPORT_SYMBOL(clk_set_parent);

struct clk *clk_get_parent(struct clk *clk)
{
	return clk->parent;
}
EXPORT_SYMBOL(clk_get_parent);

/*
                                                               
 */

int __initdata mpurate;

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

/*                                                                 */
unsigned long followparent_recalc(struct clk *clk)
{
	return clk->parent->rate;
}

/*
                                                                
                         
 */
unsigned long omap_fixed_divisor_recalc(struct clk *clk)
{
	WARN_ON(!clk->fixed_div);

	return clk->parent->rate / clk->fixed_div;
}

void clk_reparent(struct clk *child, struct clk *parent)
{
	list_del_init(&child->sibling);
	if (parent)
		list_add(&child->sibling, &parent->children);
	child->parent = parent;

	/*                                                  
                         */
}

/*                            */
void propagate_rate(struct clk *tclk)
{
	struct clk *clkp;

	list_for_each_entry(clkp, &tclk->children, sibling) {
		if (clkp->recalc)
			clkp->rate = clkp->recalc(clkp);
		propagate_rate(clkp);
	}
}

static LIST_HEAD(root_clks);

/* 
                                                                      
  
                                                                     
                                                                       
                  
 */
void recalculate_root_clocks(void)
{
	struct clk *clkp;

	list_for_each_entry(clkp, &root_clks, sibling) {
		if (clkp->recalc)
			clkp->rate = clkp->recalc(clkp);
		propagate_rate(clkp);
	}
}

/* 
                                                                        
                                   
  
                                                                           
                             
 */
void clk_preinit(struct clk *clk)
{
	INIT_LIST_HEAD(&clk->children);
}

int clk_register(struct clk *clk)
{
	if (clk == NULL || IS_ERR(clk))
		return -EINVAL;

	/*
                                      
  */
	if (clk->node.next || clk->node.prev)
		return 0;

	mutex_lock(&clocks_mutex);
	if (clk->parent)
		list_add(&clk->sibling, &clk->parent->children);
	else
		list_add(&clk->sibling, &root_clks);

	list_add(&clk->node, &clocks);
	if (clk->init)
		clk->init(clk);
	mutex_unlock(&clocks_mutex);

	return 0;
}
EXPORT_SYMBOL(clk_register);

void clk_unregister(struct clk *clk)
{
	if (clk == NULL || IS_ERR(clk))
		return;

	mutex_lock(&clocks_mutex);
	list_del(&clk->sibling);
	list_del(&clk->node);
	mutex_unlock(&clocks_mutex);
}
EXPORT_SYMBOL(clk_unregister);

void clk_enable_init_clocks(void)
{
	struct clk *clkp;

	list_for_each_entry(clkp, &clocks, node)
		if (clkp->flags & ENABLE_ON_INIT)
			clk_enable(clkp);
}

/* 
                                                            
                                          
  
                                                                  
                                                                   
                       
 */
struct clk *omap_clk_get_by_name(const char *name)
{
	struct clk *c;
	struct clk *ret = NULL;

	mutex_lock(&clocks_mutex);

	list_for_each_entry(c, &clocks, node) {
		if (!strcmp(c->name, name)) {
			ret = c;
			break;
		}
	}

	mutex_unlock(&clocks_mutex);

	return ret;
}

int omap_clk_enable_autoidle_all(void)
{
	struct clk *c;
	unsigned long flags;

	spin_lock_irqsave(&clockfw_lock, flags);

	list_for_each_entry(c, &clocks, node)
		if (c->ops->allow_idle)
			c->ops->allow_idle(c);

	spin_unlock_irqrestore(&clockfw_lock, flags);

	return 0;
}

int omap_clk_disable_autoidle_all(void)
{
	struct clk *c;
	unsigned long flags;

	spin_lock_irqsave(&clockfw_lock, flags);

	list_for_each_entry(c, &clocks, node)
		if (c->ops->deny_idle)
			c->ops->deny_idle(c);

	spin_unlock_irqrestore(&clockfw_lock, flags);

	return 0;
}

/*
                    
 */
static int clkll_enable_null(struct clk *clk)
{
	return 0;
}

static void clkll_disable_null(struct clk *clk)
{
}

const struct clkops clkops_null = {
	.enable		= clkll_enable_null,
	.disable	= clkll_disable_null,
};

/*
              
  
                                                                       
 */
struct clk dummy_ck = {
	.name	= "dummy",
	.ops	= &clkops_null,
};

/*
  
 */

#ifdef CONFIG_OMAP_RESET_CLOCKS
/*
                                                      
 */
static int __init clk_disable_unused(void)
{
	struct clk *ck;
	unsigned long flags;

	pr_info("clock: disabling unused clocks to save power\n");

	spin_lock_irqsave(&clockfw_lock, flags);
	list_for_each_entry(ck, &clocks, node) {
		if (ck->ops == &clkops_null)
			continue;

		if (ck->usecount > 0 || !ck->enable_reg)
			continue;

		omap1_clk_disable_unused(ck);
	}
	spin_unlock_irqrestore(&clockfw_lock, flags);

	return 0;
}
late_initcall(clk_disable_unused);
late_initcall(omap_clk_enable_autoidle_all);
#endif

#if defined(CONFIG_PM_DEBUG) && defined(CONFIG_DEBUG_FS)
/*
                                                               
 */

#include <linux/debugfs.h>
#include <linux/seq_file.h>

static struct dentry *clk_debugfs_root;

static int clk_dbg_show_summary(struct seq_file *s, void *unused)
{
	struct clk *c;
	struct clk *pa;

	mutex_lock(&clocks_mutex);
	seq_printf(s, "%-30s %-30s %-10s %s\n",
		   "clock-name", "parent-name", "rate", "use-count");

	list_for_each_entry(c, &clocks, node) {
		pa = c->parent;
		seq_printf(s, "%-30s %-30s %-10lu %d\n",
			   c->name, pa ? pa->name : "none", c->rate,
			   c->usecount);
	}
	mutex_unlock(&clocks_mutex);

	return 0;
}

static int clk_dbg_open(struct inode *inode, struct file *file)
{
	return single_open(file, clk_dbg_show_summary, inode->i_private);
}

static const struct file_operations debug_clock_fops = {
	.open           = clk_dbg_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

static int clk_debugfs_register_one(struct clk *c)
{
	int err;
	struct dentry *d;
	struct clk *pa = c->parent;

	d = debugfs_create_dir(c->name, pa ? pa->dent : clk_debugfs_root);
	if (!d)
		return -ENOMEM;
	c->dent = d;

	d = debugfs_create_u8("usecount", S_IRUGO, c->dent, (u8 *)&c->usecount);
	if (!d) {
		err = -ENOMEM;
		goto err_out;
	}
	d = debugfs_create_u32("rate", S_IRUGO, c->dent, (u32 *)&c->rate);
	if (!d) {
		err = -ENOMEM;
		goto err_out;
	}
	d = debugfs_create_x32("flags", S_IRUGO, c->dent, (u32 *)&c->flags);
	if (!d) {
		err = -ENOMEM;
		goto err_out;
	}
	return 0;

err_out:
	debugfs_remove_recursive(c->dent);
	return err;
}

static int clk_debugfs_register(struct clk *c)
{
	int err;
	struct clk *pa = c->parent;

	if (pa && !pa->dent) {
		err = clk_debugfs_register(pa);
		if (err)
			return err;
	}

	if (!c->dent) {
		err = clk_debugfs_register_one(c);
		if (err)
			return err;
	}
	return 0;
}

static int __init clk_debugfs_init(void)
{
	struct clk *c;
	struct dentry *d;
	int err;

	d = debugfs_create_dir("clock", NULL);
	if (!d)
		return -ENOMEM;
	clk_debugfs_root = d;

	list_for_each_entry(c, &clocks, node) {
		err = clk_debugfs_register(c);
		if (err)
			goto err_out;
	}

	d = debugfs_create_file("summary", S_IRUGO,
		d, NULL, &debug_clock_fops);
	if (!d)
		return -ENOMEM;

	return 0;
err_out:
	debugfs_remove_recursive(clk_debugfs_root);
	return err;
}
late_initcall(clk_debugfs_init);

#endif /*                                                      */
