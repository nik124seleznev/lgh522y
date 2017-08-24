/*
 * OMAP4 CM instance functions
 *
 * Copyright (C) 2009 Nokia Corporation
 * Copyright (C) 2008-2011 Texas Instruments, Inc.
 * Paul Walmsley
 * Rajendra Nayak <rnayak@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This is needed since CM instances can be in the PRM, PRCM_MPU, CM1,
 * or CM2 hardware modules.  For example, the EMU_CM CM instance is in
 * the PRM hardware module.  What a mess...
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/io.h>

#include "iomap.h"
#include "common.h"
#include "clockdomain.h"
#include "cm.h"
#include "cm1_44xx.h"
#include "cm2_44xx.h"
#include "cm44xx.h"
#include "cminst44xx.h"
#include "cm-regbits-34xx.h"
#include "cm-regbits-44xx.h"
#include "prcm44xx.h"
#include "prm44xx.h"
#include "prcm_mpu44xx.h"
#include "prcm-common.h"

/*
                                                                          
  
                                                            
                                                                              
                           
                                                                              
                                                  
                                                            
  
 */
#define CLKCTRL_IDLEST_FUNCTIONAL		0x0
#define CLKCTRL_IDLEST_INTRANSITION		0x1
#define CLKCTRL_IDLEST_INTERFACE_IDLE		0x2
#define CLKCTRL_IDLEST_DISABLED			0x3

static void __iomem *_cm_bases[OMAP4_MAX_PRCM_PARTITIONS];

/* 
                                                  
  
                                                
                                                    
 */
void omap_cm_base_init(void)
{
	_cm_bases[OMAP4430_PRM_PARTITION] = prm_base;
	_cm_bases[OMAP4430_CM1_PARTITION] = cm_base;
	_cm_bases[OMAP4430_CM2_PARTITION] = cm2_base;
	_cm_bases[OMAP4430_PRCM_MPU_PARTITION] = prcm_mpu_base;
}

/*                   */

/* 
                                                                               
                                                                  
                                                    
                                                        
                                                                        
  
                                                                         
         
 */
static u32 _clkctrl_idlest(u8 part, u16 inst, s16 cdoffs, u16 clkctrl_offs)
{
	u32 v = omap4_cminst_read_inst_reg(part, inst, clkctrl_offs);
	v &= OMAP4430_IDLEST_MASK;
	v >>= OMAP4430_IDLEST_SHIFT;
	return v;
}

/* 
                                                                                
                                                                  
                                                    
                                                        
                                                                        
  
                                                                      
                                                   
 */
static bool _is_module_ready(u8 part, u16 inst, s16 cdoffs, u16 clkctrl_offs)
{
	u32 v;

	v = _clkctrl_idlest(part, inst, cdoffs, clkctrl_offs);

	return (v == CLKCTRL_IDLEST_FUNCTIONAL ||
		v == CLKCTRL_IDLEST_INTERFACE_IDLE) ? true : false;
}

/*                  */

/*                                  */
u32 omap4_cminst_read_inst_reg(u8 part, s16 inst, u16 idx)
{
	BUG_ON(part >= OMAP4_MAX_PRCM_PARTITIONS ||
	       part == OMAP4430_INVALID_PRCM_PARTITION ||
	       !_cm_bases[part]);
	return __raw_readl(_cm_bases[part] + inst + idx);
}

/*                                        */
void omap4_cminst_write_inst_reg(u32 val, u8 part, s16 inst, u16 idx)
{
	BUG_ON(part >= OMAP4_MAX_PRCM_PARTITIONS ||
	       part == OMAP4430_INVALID_PRCM_PARTITION ||
	       !_cm_bases[part]);
	__raw_writel(val, _cm_bases[part] + inst + idx);
}

/*                                                       */
u32 omap4_cminst_rmw_inst_reg_bits(u32 mask, u32 bits, u8 part, s16 inst,
				   s16 idx)
{
	u32 v;

	v = omap4_cminst_read_inst_reg(part, inst, idx);
	v &= ~mask;
	v |= bits;
	omap4_cminst_write_inst_reg(v, part, inst, idx);

	return v;
}

u32 omap4_cminst_set_inst_reg_bits(u32 bits, u8 part, s16 inst, s16 idx)
{
	return omap4_cminst_rmw_inst_reg_bits(bits, bits, part, inst, idx);
}

u32 omap4_cminst_clear_inst_reg_bits(u32 bits, u8 part, s16 inst, s16 idx)
{
	return omap4_cminst_rmw_inst_reg_bits(bits, 0x0, part, inst, idx);
}

u32 omap4_cminst_read_inst_reg_bits(u8 part, u16 inst, s16 idx, u32 mask)
{
	u32 v;

	v = omap4_cminst_read_inst_reg(part, inst, idx);
	v &= mask;
	v >>= __ffs(mask);

	return v;
}

/*
  
 */

/* 
                                                                            
                                                                 
                                                                    
                                                    
                                                        
  
                                                                     
                                
 */
static void _clktrctrl_write(u8 c, u8 part, s16 inst, u16 cdoffs)
{
	u32 v;

	v = omap4_cminst_read_inst_reg(part, inst, cdoffs + OMAP4_CM_CLKSTCTRL);
	v &= ~OMAP4430_CLKTRCTRL_MASK;
	v |= c << OMAP4430_CLKTRCTRL_SHIFT;
	omap4_cminst_write_inst_reg(v, part, inst, cdoffs + OMAP4_CM_CLKSTCTRL);
}

/* 
                                                                        
                                                                    
                                                    
                                                        
  
                                                                         
                                                       
 */
bool omap4_cminst_is_clkdm_in_hwsup(u8 part, s16 inst, u16 cdoffs)
{
	u32 v;

	v = omap4_cminst_read_inst_reg(part, inst, cdoffs + OMAP4_CM_CLKSTCTRL);
	v &= OMAP4430_CLKTRCTRL_MASK;
	v >>= OMAP4430_CLKTRCTRL_SHIFT;

	return (v == OMAP34XX_CLKSTCTRL_ENABLE_AUTO) ? true : false;
}

/* 
                                                                         
                                                                   
                                                    
                                                        
  
                                                                
                                                   
 */
void omap4_cminst_clkdm_enable_hwsup(u8 part, s16 inst, u16 cdoffs)
{
	_clktrctrl_write(OMAP34XX_CLKSTCTRL_ENABLE_AUTO, part, inst, cdoffs);
}

/* 
                                                                          
                                                                   
                                                    
                                                        
  
                                                                
                                                                  
                                                 
 */
void omap4_cminst_clkdm_disable_hwsup(u8 part, s16 inst, u16 cdoffs)
{
	_clktrctrl_write(OMAP34XX_CLKSTCTRL_DISABLE_AUTO, part, inst, cdoffs);
}

/* 
                                                                         
                                                                   
                                                    
                                                        
  
                                                                         
                                  
 */
void omap4_cminst_clkdm_force_wakeup(u8 part, s16 inst, u16 cdoffs)
{
	_clktrctrl_write(OMAP34XX_CLKSTCTRL_FORCE_WAKEUP, part, inst, cdoffs);
}

/*
  
 */

/* 
                                                                           
                                                                  
                                                    
                                                        
                                                                        
  
                                                                           
                                                                          
                                                                       
                  
 */
int omap4_cminst_wait_module_ready(u8 part, u16 inst, s16 cdoffs,
				   u16 clkctrl_offs)
{
	int i = 0;

	if (!clkctrl_offs)
		return 0;

	omap_test_timeout(_is_module_ready(part, inst, cdoffs, clkctrl_offs),
			  MAX_MODULE_READY_TIME, i);

	return (i < MAX_MODULE_READY_TIME) ? 0 : -EBUSY;
}

/* 
                                                                        
        
                                                                  
                                                    
                                                        
                                                                        
  
                                                                   
                                                                   
                               
 */
int omap4_cminst_wait_module_idle(u8 part, u16 inst, s16 cdoffs, u16 clkctrl_offs)
{
	int i = 0;

	if (!clkctrl_offs)
		return 0;

	omap_test_timeout((_clkctrl_idlest(part, inst, cdoffs, clkctrl_offs) ==
			   CLKCTRL_IDLEST_DISABLED),
			  MAX_MODULE_DISABLE_TIME, i);

	return (i < MAX_MODULE_DISABLE_TIME) ? 0 : -EBUSY;
}

/* 
                                                                    
                                
                                                                  
                                                    
                                                        
                                                                        
  
                   
 */
void omap4_cminst_module_enable(u8 mode, u8 part, u16 inst, s16 cdoffs,
			    u16 clkctrl_offs)
{
	u32 v;

	v = omap4_cminst_read_inst_reg(part, inst, clkctrl_offs);
	v &= ~OMAP4430_MODULEMODE_MASK;
	v |= mode << OMAP4430_MODULEMODE_SHIFT;
	omap4_cminst_write_inst_reg(v, part, inst, clkctrl_offs);
}

/* 
                                                                  
                                                                  
                                                    
                                                        
                                                                        
  
                   
 */
void omap4_cminst_module_disable(u8 part, u16 inst, s16 cdoffs,
			     u16 clkctrl_offs)
{
	u32 v;

	v = omap4_cminst_read_inst_reg(part, inst, clkctrl_offs);
	v &= ~OMAP4430_MODULEMODE_MASK;
	omap4_cminst_write_inst_reg(v, part, inst, clkctrl_offs);
}

/*
                                  
 */

static int omap4_clkdm_add_wkup_sleep_dep(struct clockdomain *clkdm1,
					struct clockdomain *clkdm2)
{
	omap4_cminst_set_inst_reg_bits((1 << clkdm2->dep_bit),
				       clkdm1->prcm_partition,
				       clkdm1->cm_inst, clkdm1->clkdm_offs +
				       OMAP4_CM_STATICDEP);
	return 0;
}

static int omap4_clkdm_del_wkup_sleep_dep(struct clockdomain *clkdm1,
					struct clockdomain *clkdm2)
{
	omap4_cminst_clear_inst_reg_bits((1 << clkdm2->dep_bit),
					 clkdm1->prcm_partition,
					 clkdm1->cm_inst, clkdm1->clkdm_offs +
					 OMAP4_CM_STATICDEP);
	return 0;
}

static int omap4_clkdm_read_wkup_sleep_dep(struct clockdomain *clkdm1,
					struct clockdomain *clkdm2)
{
	return omap4_cminst_read_inst_reg_bits(clkdm1->prcm_partition,
					       clkdm1->cm_inst,
					       clkdm1->clkdm_offs +
					       OMAP4_CM_STATICDEP,
					       (1 << clkdm2->dep_bit));
}

static int omap4_clkdm_clear_all_wkup_sleep_deps(struct clockdomain *clkdm)
{
	struct clkdm_dep *cd;
	u32 mask = 0;

	if (!clkdm->prcm_partition)
		return 0;

	for (cd = clkdm->wkdep_srcs; cd && cd->clkdm_name; cd++) {
		if (!cd->clkdm)
			continue; /*                                   */

		mask |= 1 << cd->clkdm->dep_bit;
		cd->wkdep_usecount = 0;
	}

	omap4_cminst_clear_inst_reg_bits(mask, clkdm->prcm_partition,
					 clkdm->cm_inst, clkdm->clkdm_offs +
					 OMAP4_CM_STATICDEP);
	return 0;
}

static int omap4_clkdm_sleep(struct clockdomain *clkdm)
{
	omap4_cminst_clkdm_enable_hwsup(clkdm->prcm_partition,
					clkdm->cm_inst, clkdm->clkdm_offs);
	return 0;
}

static int omap4_clkdm_wakeup(struct clockdomain *clkdm)
{
	omap4_cminst_clkdm_force_wakeup(clkdm->prcm_partition,
					clkdm->cm_inst, clkdm->clkdm_offs);
	return 0;
}

static void omap4_clkdm_allow_idle(struct clockdomain *clkdm)
{
	omap4_cminst_clkdm_enable_hwsup(clkdm->prcm_partition,
					clkdm->cm_inst, clkdm->clkdm_offs);
}

static void omap4_clkdm_deny_idle(struct clockdomain *clkdm)
{
	if (clkdm->flags & CLKDM_CAN_FORCE_WAKEUP)
		omap4_clkdm_wakeup(clkdm);
	else
		omap4_cminst_clkdm_disable_hwsup(clkdm->prcm_partition,
						 clkdm->cm_inst,
						 clkdm->clkdm_offs);
}

static int omap4_clkdm_clk_enable(struct clockdomain *clkdm)
{
	if (clkdm->flags & CLKDM_CAN_FORCE_WAKEUP)
		return omap4_clkdm_wakeup(clkdm);

	return 0;
}

static int omap4_clkdm_clk_disable(struct clockdomain *clkdm)
{
	bool hwsup = false;

	if (!clkdm->prcm_partition)
		return 0;

	/*
                                                           
                                                          
          
  */
	if (clkdm->flags & CLKDM_MISSING_IDLE_REPORTING &&
	    !(clkdm->flags & CLKDM_CAN_FORCE_SLEEP)) {
		omap4_clkdm_allow_idle(clkdm);
		return 0;
	}

	hwsup = omap4_cminst_is_clkdm_in_hwsup(clkdm->prcm_partition,
					clkdm->cm_inst, clkdm->clkdm_offs);

	if (!hwsup && (clkdm->flags & CLKDM_CAN_FORCE_SLEEP))
		omap4_clkdm_sleep(clkdm);

	return 0;
}

struct clkdm_ops omap4_clkdm_operations = {
	.clkdm_add_wkdep	= omap4_clkdm_add_wkup_sleep_dep,
	.clkdm_del_wkdep	= omap4_clkdm_del_wkup_sleep_dep,
	.clkdm_read_wkdep	= omap4_clkdm_read_wkup_sleep_dep,
	.clkdm_clear_all_wkdeps	= omap4_clkdm_clear_all_wkup_sleep_deps,
	.clkdm_add_sleepdep	= omap4_clkdm_add_wkup_sleep_dep,
	.clkdm_del_sleepdep	= omap4_clkdm_del_wkup_sleep_dep,
	.clkdm_read_sleepdep	= omap4_clkdm_read_wkup_sleep_dep,
	.clkdm_clear_all_sleepdeps	= omap4_clkdm_clear_all_wkup_sleep_deps,
	.clkdm_sleep		= omap4_clkdm_sleep,
	.clkdm_wakeup		= omap4_clkdm_wakeup,
	.clkdm_allow_idle	= omap4_clkdm_allow_idle,
	.clkdm_deny_idle	= omap4_clkdm_deny_idle,
	.clkdm_clk_enable	= omap4_clkdm_clk_enable,
	.clkdm_clk_disable	= omap4_clkdm_clk_disable,
};
