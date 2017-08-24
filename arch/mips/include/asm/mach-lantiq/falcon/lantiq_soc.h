/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#ifndef _LTQ_FALCON_H__
#define _LTQ_FALCON_H__

#ifdef CONFIG_SOC_FALCON

#include <linux/pinctrl/pinctrl.h>
#include <lantiq.h>

/*          */
#define SOC_ID_FALCON		0x01B8

/*           */
#define SOC_TYPE_FALCON		0x01

/*
                                                              
                         
 */
#define LTQ_ASC0_BASE_ADDR	0x1E100C00
#define LTQ_EARLY_ASC		KSEG1ADDR(LTQ_ASC0_BASE_ADDR)

/*     */
#define LTQ_RST_CAUSE_WDTRST	0x0002

/*         */
#define LTQ_STATUS_BASE_ADDR	0x1E802000

#define FALCON_CHIPID		((u32 *)(KSEG1 + LTQ_STATUS_BASE_ADDR + 0x0c))
#define FALCON_CHIPTYPE		((u32 *)(KSEG1 + LTQ_STATUS_BASE_ADDR + 0x38))
#define FALCON_CHIPCONF		((u32 *)(KSEG1 + LTQ_STATUS_BASE_ADDR + 0x40))

/*                                                                      */
#define SYSCTL_SYS1		0
#define SYSCTL_SYSETH		1
#define SYSCTL_SYSGPE		2

/*                                         */
#define BS_FLASH		0x1
#define BS_SPI			0x4

/*                        */
extern __iomem void *ltq_ebu_membase;
extern __iomem void *ltq_sys1_membase;
#define ltq_ebu_w32(x, y)	ltq_w32((x), ltq_ebu_membase + (y))
#define ltq_ebu_r32(x)		ltq_r32(ltq_ebu_membase + (x))

#define ltq_sys1_w32(x, y)	ltq_w32((x), ltq_sys1_membase + (y))
#define ltq_sys1_r32(x)		ltq_r32(ltq_sys1_membase + (x))
#define ltq_sys1_w32_mask(clear, set, reg)   \
	ltq_sys1_w32((ltq_sys1_r32(reg) & ~(clear)) | (set), reg)

/*                                                         */
extern int pinctrl_falcon_get_range_size(int id);
extern void pinctrl_falcon_add_gpio_range(struct pinctrl_gpio_range *range);

/*
                                                                     
                 
 */
#define LTQ_EBU_PCC_ISTAT	0

#endif /*                   */
#endif /*               */
