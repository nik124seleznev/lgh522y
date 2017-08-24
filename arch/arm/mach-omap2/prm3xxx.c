/*
 * OMAP3xxx PRM module functions
 *
 * Copyright (C) 2010-2012 Texas Instruments, Inc.
 * Copyright (C) 2010 Nokia Corporation
 * Benoît Cousson
 * Paul Walmsley
 * Rajendra Nayak <rnayak@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/irq.h>

#include "soc.h"
#include "common.h"
#include "vp.h"
#include "powerdomain.h"
#include "prm3xxx.h"
#include "prm2xxx_3xxx.h"
#include "cm2xxx_3xxx.h"
#include "prm-regbits-34xx.h"

static const struct omap_prcm_irq omap3_prcm_irqs[] = {
	OMAP_PRCM_IRQ("wkup",	0,	0),
	OMAP_PRCM_IRQ("io",	9,	1),
};

static struct omap_prcm_irq_setup omap3_prcm_irq_setup = {
	.ack			= OMAP3_PRM_IRQSTATUS_MPU_OFFSET,
	.mask			= OMAP3_PRM_IRQENABLE_MPU_OFFSET,
	.nr_regs		= 1,
	.irqs			= omap3_prcm_irqs,
	.nr_irqs		= ARRAY_SIZE(omap3_prcm_irqs),
	.irq			= 11 + OMAP_INTC_START,
	.read_pending_irqs	= &omap3xxx_prm_read_pending_irqs,
	.ocp_barrier		= &omap3xxx_prm_ocp_barrier,
	.save_and_clear_irqen	= &omap3xxx_prm_save_and_clear_irqen,
	.restore_irqen		= &omap3xxx_prm_restore_irqen,
};

/*
                                                                    
                                                                      
                                                              
 */
static struct prm_reset_src_map omap3xxx_prm_reset_src_map[] = {
	{ OMAP3430_GLOBAL_COLD_RST_SHIFT, OMAP_GLOBAL_COLD_RST_SRC_ID_SHIFT },
	{ OMAP3430_GLOBAL_SW_RST_SHIFT, OMAP_GLOBAL_WARM_RST_SRC_ID_SHIFT },
	{ OMAP3430_SECURITY_VIOL_RST_SHIFT, OMAP_SECU_VIOL_RST_SRC_ID_SHIFT },
	{ OMAP3430_MPU_WD_RST_SHIFT, OMAP_MPU_WD_RST_SRC_ID_SHIFT },
	{ OMAP3430_SECURE_WD_RST_SHIFT, OMAP_MPU_WD_RST_SRC_ID_SHIFT },
	{ OMAP3430_EXTERNAL_WARM_RST_SHIFT, OMAP_EXTWARM_RST_SRC_ID_SHIFT },
	{ OMAP3430_VDD1_VOLTAGE_MANAGER_RST_SHIFT,
	  OMAP_VDD_MPU_VM_RST_SRC_ID_SHIFT },
	{ OMAP3430_VDD2_VOLTAGE_MANAGER_RST_SHIFT,
	  OMAP_VDD_CORE_VM_RST_SRC_ID_SHIFT },
	{ OMAP3430_ICEPICK_RST_SHIFT, OMAP_ICEPICK_RST_SRC_ID_SHIFT },
	{ OMAP3430_ICECRUSHER_RST_SHIFT, OMAP_ICECRUSHER_RST_SRC_ID_SHIFT },
	{ -1, -1 },
};

/*        */

/*
                                                          
                                                                      
 */
struct omap3_vp {
	u32 tranxdone_status;
};

static struct omap3_vp omap3_vp[] = {
	[OMAP3_VP_VDD_MPU_ID] = {
		.tranxdone_status = OMAP3430_VP1_TRANXDONE_ST_MASK,
	},
	[OMAP3_VP_VDD_CORE_ID] = {
		.tranxdone_status = OMAP3430_VP2_TRANXDONE_ST_MASK,
	},
};

#define MAX_VP_ID ARRAY_SIZE(omap3_vp);

u32 omap3_prm_vp_check_txdone(u8 vp_id)
{
	struct omap3_vp *vp = &omap3_vp[vp_id];
	u32 irqstatus;

	irqstatus = omap2_prm_read_mod_reg(OCP_MOD,
					   OMAP3_PRM_IRQSTATUS_MPU_OFFSET);
	return irqstatus & vp->tranxdone_status;
}

void omap3_prm_vp_clear_txdone(u8 vp_id)
{
	struct omap3_vp *vp = &omap3_vp[vp_id];

	omap2_prm_write_mod_reg(vp->tranxdone_status,
				OCP_MOD, OMAP3_PRM_IRQSTATUS_MPU_OFFSET);
}

u32 omap3_prm_vcvp_read(u8 offset)
{
	return omap2_prm_read_mod_reg(OMAP3430_GR_MOD, offset);
}

void omap3_prm_vcvp_write(u32 val, u8 offset)
{
	omap2_prm_write_mod_reg(val, OMAP3430_GR_MOD, offset);
}

u32 omap3_prm_vcvp_rmw(u32 mask, u32 bits, u8 offset)
{
	return omap2_prm_rmw_mod_reg_bits(mask, bits, OMAP3430_GR_MOD, offset);
}

/* 
                                                                    
  
                                                                     
                                                                   
                
 */
void omap3xxx_prm_dpll3_reset(void)
{
	omap2_prm_set_mod_reg_bits(OMAP_RST_DPLL3_MASK, OMAP3430_GR_MOD,
				   OMAP2_RM_RSTCTRL);
	/*             */
	omap2_prm_read_mod_reg(OMAP3430_GR_MOD, OMAP2_RM_RSTCTRL);
}

/* 
                                                                          
                                                
  
                                                                     
                                                                     
                   
 */
void omap3xxx_prm_read_pending_irqs(unsigned long *events)
{
	u32 mask, st;

	/*                                                                */
	mask = omap2_prm_read_mod_reg(OCP_MOD, OMAP3_PRM_IRQENABLE_MPU_OFFSET);
	st = omap2_prm_read_mod_reg(OCP_MOD, OMAP3_PRM_IRQSTATUS_MPU_OFFSET);

	events[0] = mask & st;
}

/* 
                                                                              
  
                                                                     
                                                                    
                                                                      
                          
 */
void omap3xxx_prm_ocp_barrier(void)
{
	omap2_prm_read_mod_reg(OCP_MOD, OMAP3_PRM_REVISION_OFFSET);
}

/* 
                                                                       
                                                         
  
                                                                   
                                                                   
                                                                    
                                                                    
                                                                    
         
 */
void omap3xxx_prm_save_and_clear_irqen(u32 *saved_mask)
{
	saved_mask[0] = omap2_prm_read_mod_reg(OCP_MOD,
					       OMAP3_PRM_IRQENABLE_MPU_OFFSET);
	omap2_prm_write_mod_reg(0, OCP_MOD, OMAP3_PRM_IRQENABLE_MPU_OFFSET);

	/*             */
	omap2_prm_read_mod_reg(OCP_MOD, OMAP3_PRM_REVISION_OFFSET);
}

/* 
                                                                        
                                                                     
  
                                                                     
                                                                     
                                                               
                                                                      
                                                   
 */
void omap3xxx_prm_restore_irqen(u32 *saved_mask)
{
	omap2_prm_write_mod_reg(saved_mask[0], OCP_MOD,
				OMAP3_PRM_IRQENABLE_MPU_OFFSET);
}

/* 
                                                                              
  
                                                                     
                                                                     
                                                                      
                                                                 
                
 */
void omap3xxx_prm_reconfigure_io_chain(void)
{
	int i = 0;

	omap2_prm_set_mod_reg_bits(OMAP3430_EN_IO_CHAIN_MASK, WKUP_MOD,
				   PM_WKEN);

	omap_test_timeout(omap2_prm_read_mod_reg(WKUP_MOD, PM_WKST) &
			  OMAP3430_ST_IO_CHAIN_MASK,
			  MAX_IOPAD_LATCH_TIME, i);
	if (i == MAX_IOPAD_LATCH_TIME)
		pr_warn("PRM: I/O chain clock line assertion timed out\n");

	omap2_prm_clear_mod_reg_bits(OMAP3430_EN_IO_CHAIN_MASK, WKUP_MOD,
				     PM_WKEN);

	omap2_prm_set_mod_reg_bits(OMAP3430_ST_IO_CHAIN_MASK, WKUP_MOD,
				   PM_WKST);

	omap2_prm_read_mod_reg(WKUP_MOD, PM_WKST);
}

/* 
                                                                               
  
                                                                     
                                                               
                                                                 
                                                                     
                   
 */
static void __init omap3xxx_prm_enable_io_wakeup(void)
{
	if (omap3_has_io_wakeup())
		omap2_prm_set_mod_reg_bits(OMAP3430_EN_IO_MASK, WKUP_MOD,
					   PM_WKEN);
}

/* 
                                                                     
  
                                                                    
                                                                
 */
static u32 omap3xxx_prm_read_reset_sources(void)
{
	struct prm_reset_src_map *p;
	u32 r = 0;
	u32 v;

	v = omap2_prm_read_mod_reg(WKUP_MOD, OMAP2_RM_RSTST);

	p = omap3xxx_prm_reset_src_map;
	while (p->reg_shift >= 0 && p->std_shift >= 0) {
		if (v & (1 << p->reg_shift))
			r |= 1 << p->std_shift;
		p++;
	}

	return r;
}

/*                                 */

static int omap3_pwrdm_set_next_pwrst(struct powerdomain *pwrdm, u8 pwrst)
{
	omap2_prm_rmw_mod_reg_bits(OMAP_POWERSTATE_MASK,
				   (pwrst << OMAP_POWERSTATE_SHIFT),
				   pwrdm->prcm_offs, OMAP2_PM_PWSTCTRL);
	return 0;
}

static int omap3_pwrdm_read_next_pwrst(struct powerdomain *pwrdm)
{
	return omap2_prm_read_mod_bits_shift(pwrdm->prcm_offs,
					     OMAP2_PM_PWSTCTRL,
					     OMAP_POWERSTATE_MASK);
}

static int omap3_pwrdm_read_pwrst(struct powerdomain *pwrdm)
{
	return omap2_prm_read_mod_bits_shift(pwrdm->prcm_offs,
					     OMAP2_PM_PWSTST,
					     OMAP_POWERSTATEST_MASK);
}

/*                                                   */
static int omap3_pwrdm_read_prev_pwrst(struct powerdomain *pwrdm)
{
	return omap2_prm_read_mod_bits_shift(pwrdm->prcm_offs,
					     OMAP3430_PM_PREPWSTST,
					     OMAP3430_LASTPOWERSTATEENTERED_MASK);
}

static int omap3_pwrdm_read_logic_pwrst(struct powerdomain *pwrdm)
{
	return omap2_prm_read_mod_bits_shift(pwrdm->prcm_offs,
					     OMAP2_PM_PWSTST,
					     OMAP3430_LOGICSTATEST_MASK);
}

static int omap3_pwrdm_read_logic_retst(struct powerdomain *pwrdm)
{
	return omap2_prm_read_mod_bits_shift(pwrdm->prcm_offs,
					     OMAP2_PM_PWSTCTRL,
					     OMAP3430_LOGICSTATEST_MASK);
}

static int omap3_pwrdm_read_prev_logic_pwrst(struct powerdomain *pwrdm)
{
	return omap2_prm_read_mod_bits_shift(pwrdm->prcm_offs,
					     OMAP3430_PM_PREPWSTST,
					     OMAP3430_LASTLOGICSTATEENTERED_MASK);
}

static int omap3_get_mem_bank_lastmemst_mask(u8 bank)
{
	switch (bank) {
	case 0:
		return OMAP3430_LASTMEM1STATEENTERED_MASK;
	case 1:
		return OMAP3430_LASTMEM2STATEENTERED_MASK;
	case 2:
		return OMAP3430_LASTSHAREDL2CACHEFLATSTATEENTERED_MASK;
	case 3:
		return OMAP3430_LASTL2FLATMEMSTATEENTERED_MASK;
	default:
		WARN_ON(1); /*                     */
		return -EEXIST;
	}
	return 0;
}

static int omap3_pwrdm_read_prev_mem_pwrst(struct powerdomain *pwrdm, u8 bank)
{
	u32 m;

	m = omap3_get_mem_bank_lastmemst_mask(bank);

	return omap2_prm_read_mod_bits_shift(pwrdm->prcm_offs,
				OMAP3430_PM_PREPWSTST, m);
}

static int omap3_pwrdm_clear_all_prev_pwrst(struct powerdomain *pwrdm)
{
	omap2_prm_write_mod_reg(0, pwrdm->prcm_offs, OMAP3430_PM_PREPWSTST);
	return 0;
}

static int omap3_pwrdm_enable_hdwr_sar(struct powerdomain *pwrdm)
{
	return omap2_prm_rmw_mod_reg_bits(0,
					  1 << OMAP3430ES2_SAVEANDRESTORE_SHIFT,
					  pwrdm->prcm_offs, OMAP2_PM_PWSTCTRL);
}

static int omap3_pwrdm_disable_hdwr_sar(struct powerdomain *pwrdm)
{
	return omap2_prm_rmw_mod_reg_bits(1 << OMAP3430ES2_SAVEANDRESTORE_SHIFT,
					  0, pwrdm->prcm_offs,
					  OMAP2_PM_PWSTCTRL);
}

struct pwrdm_ops omap3_pwrdm_operations = {
	.pwrdm_set_next_pwrst	= omap3_pwrdm_set_next_pwrst,
	.pwrdm_read_next_pwrst	= omap3_pwrdm_read_next_pwrst,
	.pwrdm_read_pwrst	= omap3_pwrdm_read_pwrst,
	.pwrdm_read_prev_pwrst	= omap3_pwrdm_read_prev_pwrst,
	.pwrdm_set_logic_retst	= omap2_pwrdm_set_logic_retst,
	.pwrdm_read_logic_pwrst	= omap3_pwrdm_read_logic_pwrst,
	.pwrdm_read_logic_retst	= omap3_pwrdm_read_logic_retst,
	.pwrdm_read_prev_logic_pwrst	= omap3_pwrdm_read_prev_logic_pwrst,
	.pwrdm_set_mem_onst	= omap2_pwrdm_set_mem_onst,
	.pwrdm_set_mem_retst	= omap2_pwrdm_set_mem_retst,
	.pwrdm_read_mem_pwrst	= omap2_pwrdm_read_mem_pwrst,
	.pwrdm_read_mem_retst	= omap2_pwrdm_read_mem_retst,
	.pwrdm_read_prev_mem_pwrst	= omap3_pwrdm_read_prev_mem_pwrst,
	.pwrdm_clear_all_prev_pwrst	= omap3_pwrdm_clear_all_prev_pwrst,
	.pwrdm_enable_hdwr_sar	= omap3_pwrdm_enable_hdwr_sar,
	.pwrdm_disable_hdwr_sar	= omap3_pwrdm_disable_hdwr_sar,
	.pwrdm_wait_transition	= omap2_pwrdm_wait_transition,
};

/*
  
 */

static struct prm_ll_data omap3xxx_prm_ll_data = {
	.read_reset_sources = &omap3xxx_prm_read_reset_sources,
};

int __init omap3xxx_prm_init(void)
{
	if (!cpu_is_omap34xx())
		return 0;

	return prm_register(&omap3xxx_prm_ll_data);
}

static int __init omap3xxx_prm_late_init(void)
{
	int ret;

	if (!cpu_is_omap34xx())
		return 0;

	omap3xxx_prm_enable_io_wakeup();
	ret = omap_prcm_register_chain_handler(&omap3_prcm_irq_setup);
	if (!ret)
		irq_set_status_flags(omap_prcm_event_to_irq("io"),
				     IRQ_NOAUTOEN);

	return ret;
}
omap_subsys_initcall(omap3xxx_prm_late_init);

static void __exit omap3xxx_prm_exit(void)
{
	if (!cpu_is_omap34xx())
		return;

	/*                     */
	WARN(prm_unregister(&omap3xxx_prm_ll_data),
	     "%s: prm_ll_data function pointer mismatch\n", __func__);
}
__exitcall(omap3xxx_prm_exit);
