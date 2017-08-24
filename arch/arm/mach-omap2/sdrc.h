#ifndef __ARCH_ARM_MACH_OMAP2_SDRC_H
#define __ARCH_ARM_MACH_OMAP2_SDRC_H

/*
 * OMAP2/3 SDRC/SMS macros and prototypes
 *
 * Copyright (C) 2007-2008, 2012 Texas Instruments, Inc.
 * Copyright (C) 2007-2008 Nokia Corporation
 *
 * Paul Walmsley
 * Tony Lindgren
 * Richard Woodruff
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#undef DEBUG

#ifndef __ASSEMBLER__

#include <linux/io.h>

extern void __iomem *omap2_sdrc_base;
extern void __iomem *omap2_sms_base;

#define OMAP_SDRC_REGADDR(reg)			(omap2_sdrc_base + (reg))
#define OMAP_SMS_REGADDR(reg)			(omap2_sms_base + (reg))

/*                              */

static inline void sdrc_write_reg(u32 val, u16 reg)
{
	__raw_writel(val, OMAP_SDRC_REGADDR(reg));
}

static inline u32 sdrc_read_reg(u16 reg)
{
	return __raw_readl(OMAP_SDRC_REGADDR(reg));
}

/*                             */

static inline void sms_write_reg(u32 val, u16 reg)
{
	__raw_writel(val, OMAP_SMS_REGADDR(reg));
}

static inline u32 sms_read_reg(u16 reg)
{
	return __raw_readl(OMAP_SMS_REGADDR(reg));
}

extern void omap2_set_globals_sdrc(void __iomem *sdrc, void __iomem *sms);


/* 
                                                                        
                                 
                                                                   
                                                                   
                                                             
                                                 
  
                                                                     
                                                              
                                                                         
                                                       
 */
struct omap_sdrc_params {
	unsigned long rate;
	u32 actim_ctrla;
	u32 actim_ctrlb;
	u32 rfr_ctrl;
	u32 mr;
};

#ifdef CONFIG_SOC_HAS_OMAP2_SDRC
void omap2_sdrc_init(struct omap_sdrc_params *sdrc_cs0,
			    struct omap_sdrc_params *sdrc_cs1);
#else
static inline void __init omap2_sdrc_init(struct omap_sdrc_params *sdrc_cs0,
					  struct omap_sdrc_params *sdrc_cs1) {};
#endif

int omap2_sdrc_get_params(unsigned long r,
			  struct omap_sdrc_params **sdrc_cs0,
			  struct omap_sdrc_params **sdrc_cs1);
void omap2_sms_save_context(void);
void omap2_sms_restore_context(void);

struct memory_timings {
	u32 m_type;		/*                  */
	u32 dll_mode;		/*                                    */
	u32 slow_dll_ctrl;	/*                                       */
	u32 fast_dll_ctrl;	/*                                       */
	u32 base_cs;		/*                                          */
};

extern void omap2xxx_sdrc_init_params(u32 force_lock_to_unlock_mode);
struct omap_sdrc_params *rx51_get_sdram_timings(void);

u32 omap2xxx_sdrc_dll_is_unlocked(void);
u32 omap2xxx_sdrc_reprogram(u32 level, u32 force);


#else
#define OMAP242X_SDRC_REGADDR(reg)					\
			OMAP2_L3_IO_ADDRESS(OMAP2420_SDRC_BASE + (reg))
#define OMAP243X_SDRC_REGADDR(reg)					\
			OMAP2_L3_IO_ADDRESS(OMAP243X_SDRC_BASE + (reg))
#define OMAP34XX_SDRC_REGADDR(reg)					\
			OMAP2_L3_IO_ADDRESS(OMAP343X_SDRC_BASE + (reg))

#endif	/*               */

/*                                                 */
#define MIN_SDRC_DLL_LOCK_FREQ		83000000

/*                                                                     */
#define SDRC_MPURATE_SCALE		8

/*                                                                           */
#define SDRC_MPURATE_BASE_SHIFT		9

/*
                                                        
                                                 
 */
#define SDRC_MPURATE_LOOPS		96

/*                                                                 */

#define SDRC_SYSCONFIG		0x010
#define SDRC_CS_CFG		0x040
#define SDRC_SHARING		0x044
#define SDRC_ERR_TYPE		0x04C
#define SDRC_DLLA_CTRL		0x060
#define SDRC_DLLA_STATUS	0x064
#define SDRC_DLLB_CTRL		0x068
#define SDRC_DLLB_STATUS	0x06C
#define SDRC_POWER		0x070
#define SDRC_MCFG_0		0x080
#define SDRC_MR_0		0x084
#define SDRC_EMR2_0		0x08c
#define SDRC_ACTIM_CTRL_A_0	0x09c
#define SDRC_ACTIM_CTRL_B_0	0x0a0
#define SDRC_RFR_CTRL_0		0x0a4
#define SDRC_MANUAL_0		0x0a8
#define SDRC_MCFG_1		0x0B0
#define SDRC_MR_1		0x0B4
#define SDRC_EMR2_1		0x0BC
#define SDRC_ACTIM_CTRL_A_1	0x0C4
#define SDRC_ACTIM_CTRL_B_1	0x0C8
#define SDRC_RFR_CTRL_1		0x0D4
#define SDRC_MANUAL_1		0x0D8

#define SDRC_POWER_AUTOCOUNT_SHIFT	8
#define SDRC_POWER_AUTOCOUNT_MASK	(0xffff << SDRC_POWER_AUTOCOUNT_SHIFT)
#define SDRC_POWER_CLKCTRL_SHIFT	4
#define SDRC_POWER_CLKCTRL_MASK		(0x3 << SDRC_POWER_CLKCTRL_SHIFT)
#define SDRC_SELF_REFRESH_ON_AUTOCOUNT	(0x2 << SDRC_POWER_CLKCTRL_SHIFT)

/*
                                                                   
                                                                         
                                                                         
                                                                            
                                                                            
                     
  
                                                                          
                                                                       
                                                                           
                                                                           
                                                                            
                                                                          
                                                                         
                                                                           
  
                                                                       
                                                                         
                                                                
  
                                                                             
                                                         
 */
#define SDRC_RFR_CTRL_165MHz	(0x00044c00 | 1)
#define SDRC_RFR_CTRL_133MHz	(0x0003de00 | 1)
#define SDRC_RFR_CTRL_100MHz	(0x0002da01 | 1)
#define SDRC_RFR_CTRL_110MHz	(0x0002da01 | 1) /*              */
#define SDRC_RFR_CTRL_BYPASS	(0x00005000 | 1) /*              */


/*
                      
 */

#define OMAP242X_SMS_REGADDR(reg)					\
		(void __iomem *)OMAP2_L3_IO_ADDRESS(OMAP2420_SMS_BASE + reg)
#define OMAP243X_SMS_REGADDR(reg)					\
		(void __iomem *)OMAP2_L3_IO_ADDRESS(OMAP243X_SMS_BASE + reg)
#define OMAP343X_SMS_REGADDR(reg)					\
		(void __iomem *)OMAP2_L3_IO_ADDRESS(OMAP343X_SMS_BASE + reg)

/*                                                               */

#define SMS_SYSCONFIG			0x010
/*                                           */



#endif
