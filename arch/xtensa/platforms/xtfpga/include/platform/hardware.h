/*
 * arch/xtensa/platform/xtavnet/include/platform/hardware.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006 Tensilica Inc.
 */

/*
                                                                       
 */

#ifndef __XTENSA_XTAVNET_HARDWARE_H
#define __XTENSA_XTAVNET_HARDWARE_H

/*                                                           
                           */
#define NO_IRQ                 -1

/*                       */

#define PLATFORM_DEFAULT_MEM_START 0x00000000
#define PLATFORM_DEFAULT_MEM_SIZE  0x04000000

/*                          */

#define PLATFORM_NR_IRQS	10

/*                                                            */

#ifdef CONFIG_ARCH_HAS_SMP
#define DUART16552_INTNUM	XCHAL_EXTINT3_NUM
#define OETH_IRQ		XCHAL_EXTINT4_NUM
#else
#define DUART16552_INTNUM	XCHAL_EXTINT0_NUM
#define OETH_IRQ		XCHAL_EXTINT1_NUM
#endif

/*
                                    
 */

/*      */
#define DUART16552_PADDR	(XCHAL_KIO_PADDR + 0x0D050020)
/*                                     */
#define LCD_INSTR_ADDR		((char *)IOADDR(0x0D040000))
#define LCD_DATA_ADDR		((char *)IOADDR(0x0D040004))

/*       */
#define XTFPGA_FPGAREGS_VADDR	IOADDR(0x0D020000)
/*                                     */
#define XTFPGA_CLKFRQ_VADDR	(XTFPGA_FPGAREGS_VADDR + 0x04)
/*                             */
#define DIP_SWITCHES_VADDR	(XTFPGA_FPGAREGS_VADDR + 0x0C)
/*                                 */
#define XTFPGA_SWRST_VADDR	(XTFPGA_FPGAREGS_VADDR + 0x10)

/*                                  */
				/*                          */
#define OETH_REGS_PADDR		(XCHAL_KIO_PADDR + 0x0D030000)
#define OETH_REGS_SIZE		0x1000
#define OETH_SRAMBUFF_PADDR	(XCHAL_KIO_PADDR + 0x0D800000)

				/*                         */
#define OETH_SRAMBUFF_SIZE	(5 * 0x600 + 5 * 0x600)

#endif /*                             */
