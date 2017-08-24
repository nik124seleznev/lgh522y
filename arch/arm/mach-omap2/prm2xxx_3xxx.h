/*
 * OMAP2xxx/3xxx-common Power/Reset Management (PRM) register definitions
 *
 * Copyright (C) 2007-2009, 2011-2012 Texas Instruments, Inc.
 * Copyright (C) 2008-2010 Nokia Corporation
 * Paul Walmsley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * The PRM hardware modules on the OMAP2/3 are quite similar to each
 * other.  The PRM on OMAP4 has a new register layout, and is handled
 * in a separate file.
 */
#ifndef __ARCH_ARM_MACH_OMAP2_PRM2XXX_3XXX_H
#define __ARCH_ARM_MACH_OMAP2_PRM2XXX_3XXX_H

#include "prcm-common.h"
#include "prm.h"

/*
                                                                     
  
                                                       
  
                                                                     
                                                                    
                                 
 */

/*                                                    */

#define OMAP2_RM_RSTCTRL				0x0050
#define OMAP2_RM_RSTTIME				0x0054
#define OMAP2_RM_RSTST					0x0058
#define OMAP2_PM_PWSTCTRL				0x00e0
#define OMAP2_PM_PWSTST					0x00e4

#define PM_WKEN						0x00a0
#define PM_WKEN1					PM_WKEN
#define PM_WKST						0x00b0
#define PM_WKST1					PM_WKST
#define PM_WKDEP					0x00c8
#define PM_EVGENCTRL					0x00d4
#define PM_EVGENONTIM					0x00d8
#define PM_EVGENOFFTIM					0x00dc


#ifndef __ASSEMBLER__

#include <linux/io.h>
#include "powerdomain.h"

/*                                                */
static inline u32 omap2_prm_read_mod_reg(s16 module, u16 idx)
{
	return __raw_readl(prm_base + module + idx);
}

static inline void omap2_prm_write_mod_reg(u32 val, s16 module, u16 idx)
{
	__raw_writel(val, prm_base + module + idx);
}

/*                                                                */
static inline u32 omap2_prm_rmw_mod_reg_bits(u32 mask, u32 bits, s16 module,
					     s16 idx)
{
	u32 v;

	v = omap2_prm_read_mod_reg(module, idx);
	v &= ~mask;
	v |= bits;
	omap2_prm_write_mod_reg(v, module, idx);

	return v;
}

/*                                                                 */
static inline u32 omap2_prm_read_mod_bits_shift(s16 domain, s16 idx, u32 mask)
{
	u32 v;

	v = omap2_prm_read_mod_reg(domain, idx);
	v &= mask;
	v >>= __ffs(mask);

	return v;
}

static inline u32 omap2_prm_set_mod_reg_bits(u32 bits, s16 module, s16 idx)
{
	return omap2_prm_rmw_mod_reg_bits(bits, bits, module, idx);
}

static inline u32 omap2_prm_clear_mod_reg_bits(u32 bits, s16 module, s16 idx)
{
	return omap2_prm_rmw_mod_reg_bits(bits, 0x0, module, idx);
}

/*                                                      */
extern int omap2_prm_is_hardreset_asserted(s16 prm_mod, u8 shift);
extern int omap2_prm_assert_hardreset(s16 prm_mod, u8 shift);
extern int omap2_prm_deassert_hardreset(s16 prm_mod, u8 rst_shift, u8 st_shift);

extern int omap2_pwrdm_set_next_pwrst(struct powerdomain *pwrdm, u8 pwrst);
extern int omap2_pwrdm_read_next_pwrst(struct powerdomain *pwrdm);
extern int omap2_pwrdm_read_pwrst(struct powerdomain *pwrdm);
extern int omap2_pwrdm_set_mem_onst(struct powerdomain *pwrdm, u8 bank,
				    u8 pwrst);
extern int omap2_pwrdm_set_mem_retst(struct powerdomain *pwrdm, u8 bank,
				     u8 pwrst);
extern int omap2_pwrdm_read_mem_pwrst(struct powerdomain *pwrdm, u8 bank);
extern int omap2_pwrdm_read_mem_retst(struct powerdomain *pwrdm, u8 bank);
extern int omap2_pwrdm_set_logic_retst(struct powerdomain *pwrdm, u8 pwrst);
extern int omap2_pwrdm_wait_transition(struct powerdomain *pwrdm);

extern int omap2_clkdm_add_wkdep(struct clockdomain *clkdm1,
				 struct clockdomain *clkdm2);
extern int omap2_clkdm_del_wkdep(struct clockdomain *clkdm1,
				 struct clockdomain *clkdm2);
extern int omap2_clkdm_read_wkdep(struct clockdomain *clkdm1,
				  struct clockdomain *clkdm2);
extern int omap2_clkdm_clear_all_wkdeps(struct clockdomain *clkdm);

#endif /*             */

/*
                                    
  
                                                      
                                     
 */

/*                   */
/*                                      */
#define OMAP_ONTIMEVAL_SHIFT				0
#define OMAP_ONTIMEVAL_MASK				(0xffffffff << 0)

/*                    */
/*                                       */
#define OMAP_OFFTIMEVAL_SHIFT				0
#define OMAP_OFFTIMEVAL_MASK				(0xffffffff << 0)

/*                                 */
/*                                  */
#define OMAP_SETUP_TIME_SHIFT				0
#define OMAP_SETUP_TIME_MASK				(0xffff << 0)

/*                 */
/*                                    */
#define OMAP_SYSCLKDIV_SHIFT				6
#define OMAP_SYSCLKDIV_MASK				(0x3 << 6)
#define OMAP_SYSCLKDIV_WIDTH				2
#define OMAP_AUTOEXTCLKMODE_SHIFT			3
#define OMAP_AUTOEXTCLKMODE_MASK			(0x3 << 3)
#define OMAP_SYSCLKSEL_SHIFT				0
#define OMAP_SYSCLKSEL_MASK				(0x3 << 0)

/*                  */
#define OMAP_OFFLOADMODE_SHIFT				3
#define OMAP_OFFLOADMODE_MASK				(0x3 << 3)
#define OMAP_ONLOADMODE_SHIFT				1
#define OMAP_ONLOADMODE_MASK				(0x3 << 1)
#define OMAP_ENABLE_MASK				(1 << 0)

/*             */
/*                                   */
#define OMAP_RSTTIME2_SHIFT				8
#define OMAP_RSTTIME2_MASK				(0x1f << 8)
#define OMAP_RSTTIME1_SHIFT				0
#define OMAP_RSTTIME1_MASK				(0xff << 0)

/*             */
/*                                   */
/*                                 */
#define OMAP_RST_DPLL3_MASK				(1 << 2)
#define OMAP_RST_GS_MASK				(1 << 1)


/*
                                         
  
                                                                     
                              
 */

/*
                                                                         
                         
  
                                                                 
                                             
 */
#define OMAP_COREDOMAINWKUP_RST_MASK			(1 << 3)

/*
                                                 
  
                     
  
                                    
 */
#define OMAP_DOMAINWKUP_RST_MASK			(1 << 2)

/*
                                                  
                                                         
  
                     
  
                                    
 */
#define OMAP_GLOBALWARM_RST_SHIFT			1
#define OMAP_GLOBALWARM_RST_MASK			(1 << 1)
#define OMAP_GLOBALCOLD_RST_SHIFT			0
#define OMAP_GLOBALCOLD_RST_MASK			(1 << 0)

/*
                                                                
                                                            
  
                     
  
                                                                 
                
 */
#define OMAP_EN_WKUP_SHIFT				4
#define OMAP_EN_WKUP_MASK				(1 << 4)

/*
                                                            
                   
  
                        
  
                                                             
                                                      
                    
 */
#define OMAP_LOGICRETSTATE_MASK				(1 << 2)


#endif
