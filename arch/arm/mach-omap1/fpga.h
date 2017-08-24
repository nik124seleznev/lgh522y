/*
 * Interrupt handler for OMAP-1510 FPGA
 *
 * Copyright (C) 2001 RidgeRun, Inc.
 * Author: Greg Lonnon <glonnon@ridgerun.com>
 *
 * Copyright (C) 2002 MontaVista Software, Inc.
 *
 * Separated FPGA interrupts from innovator1510.c and cleaned up for 2.6
 * Copyright (C) 2004 Nokia Corporation by Tony Lindrgen <tony@atomide.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_ARCH_OMAP_FPGA_H
#define __ASM_ARCH_OMAP_FPGA_H

/*
                                                                              
                          
                                                                              
 */
/*                                                   */
#define H2P2_DBG_FPGA_BASE		0xE8000000		/*    */
#define H2P2_DBG_FPGA_SIZE		SZ_4K			/*      */
#define H2P2_DBG_FPGA_START		0x04000000		/*    */

#define H2P2_DBG_FPGA_ETHR_START	(H2P2_DBG_FPGA_START + 0x300)
#define H2P2_DBG_FPGA_FPGA_REV		IOMEM(H2P2_DBG_FPGA_BASE + 0x10)	/*               */
#define H2P2_DBG_FPGA_BOARD_REV		IOMEM(H2P2_DBG_FPGA_BASE + 0x12)	/*                */
#define H2P2_DBG_FPGA_GPIO		IOMEM(H2P2_DBG_FPGA_BASE + 0x14)	/*              */
#define H2P2_DBG_FPGA_LEDS		IOMEM(H2P2_DBG_FPGA_BASE + 0x16)	/*              */
#define H2P2_DBG_FPGA_MISC_INPUTS	IOMEM(H2P2_DBG_FPGA_BASE + 0x18)	/*             */
#define H2P2_DBG_FPGA_LAN_STATUS	IOMEM(H2P2_DBG_FPGA_BASE + 0x1A)	/*                 */
#define H2P2_DBG_FPGA_LAN_RESET		IOMEM(H2P2_DBG_FPGA_BASE + 0x1C)	/*                */

/*                                                                */
#define H2P2_DBG_FPGA_LED_GREEN		(1 << 15)
#define H2P2_DBG_FPGA_LED_AMBER		(1 << 14)
#define H2P2_DBG_FPGA_LED_RED		(1 << 13)
#define H2P2_DBG_FPGA_LED_BLUE		(1 << 12)
/*                       */
#define H2P2_DBG_FPGA_LOAD_METER	(1 << 0)	//                              
#define H2P2_DBG_FPGA_LOAD_METER_SIZE	11
#define H2P2_DBG_FPGA_LOAD_METER_MASK	((1 << H2P2_DBG_FPGA_LOAD_METER_SIZE) - 1)

#define H2P2_DBG_FPGA_P2_LED_TIMER		(1 << 0)
#define H2P2_DBG_FPGA_P2_LED_IDLE		(1 << 1)

#endif
