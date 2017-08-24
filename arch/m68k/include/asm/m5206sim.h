/*                                                                          */

/*
 *	m5206sim.h -- ColdFire 5206 System Integration Module support.
 *
 *	(C) Copyright 1999, Greg Ungerer (gerg@snapgear.com)
 * 	(C) Copyright 2000, Lineo Inc. (www.lineo.com) 
 */

/*                                                                          */
#ifndef	m5206sim_h
#define	m5206sim_h
/*                                                                          */

#define	CPU_NAME		"COLDFIRE(m5206)"
#define	CPU_INSTR_PER_JIFFY	3
#define	MCF_BUSCLK		MCF_CLK

#include <asm/m52xxacr.h>

/*
                                              
 */
#define	MCFSIM_SIMR		(MCF_MBAR + 0x03)	/*                */
#define	MCFSIM_ICR1		(MCF_MBAR + 0x14)	/*                 */
#define	MCFSIM_ICR2		(MCF_MBAR + 0x15)	/*                 */
#define	MCFSIM_ICR3		(MCF_MBAR + 0x16)	/*                 */
#define	MCFSIM_ICR4		(MCF_MBAR + 0x17)	/*                 */
#define	MCFSIM_ICR5		(MCF_MBAR + 0x18)	/*                 */
#define	MCFSIM_ICR6		(MCF_MBAR + 0x19)	/*                 */
#define	MCFSIM_ICR7		(MCF_MBAR + 0x1a)	/*                 */
#define	MCFSIM_ICR8		(MCF_MBAR + 0x1b)	/*                 */
#define	MCFSIM_ICR9		(MCF_MBAR + 0x1c)	/*                 */
#define	MCFSIM_ICR10		(MCF_MBAR + 0x1d)	/*                  */
#define	MCFSIM_ICR11		(MCF_MBAR + 0x1e)	/*                  */
#define	MCFSIM_ICR12		(MCF_MBAR + 0x1f)	/*                  */
#define	MCFSIM_ICR13		(MCF_MBAR + 0x20)	/*                  */
#ifdef CONFIG_M5206e
#define	MCFSIM_ICR14		(MCF_MBAR + 0x21)	/*                  */
#define	MCFSIM_ICR15		(MCF_MBAR + 0x22)	/*                  */
#endif

#define	MCFSIM_IMR		(MCF_MBAR + 0x36)	/*                */
#define	MCFSIM_IPR		(MCF_MBAR + 0x3a)	/*                   */

#define	MCFSIM_RSR		(MCF_MBAR + 0x40)	/*              */
#define	MCFSIM_SYPCR		(MCF_MBAR + 0x41)	/*                   */

#define	MCFSIM_SWIVR		(MCF_MBAR + 0x42)	/*                  */
#define	MCFSIM_SWSR		(MCF_MBAR + 0x43)	/*                 */

#define	MCFSIM_DCRR		(MCF_MBAR + 0x46) /*                        */
#define	MCFSIM_DCTR		(MCF_MBAR + 0x4a) /*                       */
#define	MCFSIM_DAR0		(MCF_MBAR + 0x4c) /*                         */
#define	MCFSIM_DMR0		(MCF_MBAR + 0x50) /*                       */
#define	MCFSIM_DCR0		(MCF_MBAR + 0x57) /*                          */
#define	MCFSIM_DAR1		(MCF_MBAR + 0x58) /*                          */
#define	MCFSIM_DMR1		(MCF_MBAR + 0x5c) /*                       */
#define	MCFSIM_DCR1		(MCF_MBAR + 0x63) /*                          */

#define	MCFSIM_CSAR0		(MCF_MBAR + 0x64)	/*                  */
#define	MCFSIM_CSMR0		(MCF_MBAR + 0x68)	/*               */
#define	MCFSIM_CSCR0		(MCF_MBAR + 0x6e)	/*                  */
#define	MCFSIM_CSAR1		(MCF_MBAR + 0x70)	/*                  */
#define	MCFSIM_CSMR1		(MCF_MBAR + 0x74)	/*               */
#define	MCFSIM_CSCR1		(MCF_MBAR + 0x7a)	/*                  */
#define	MCFSIM_CSAR2		(MCF_MBAR + 0x7c)	/*                  */
#define	MCFSIM_CSMR2		(MCF_MBAR + 0x80)	/*               */
#define	MCFSIM_CSCR2		(MCF_MBAR + 0x86)	/*                  */
#define	MCFSIM_CSAR3		(MCF_MBAR + 0x88)	/*                  */
#define	MCFSIM_CSMR3		(MCF_MBAR + 0x8c)	/*               */
#define	MCFSIM_CSCR3		(MCF_MBAR + 0x92)	/*                  */
#define	MCFSIM_CSAR4		(MCF_MBAR + 0x94)	/*                  */
#define	MCFSIM_CSMR4		(MCF_MBAR + 0x98)	/*               */
#define	MCFSIM_CSCR4		(MCF_MBAR + 0x9e)	/*                  */
#define	MCFSIM_CSAR5		(MCF_MBAR + 0xa0)	/*                  */
#define	MCFSIM_CSMR5		(MCF_MBAR + 0xa4)	/*               */
#define	MCFSIM_CSCR5		(MCF_MBAR + 0xaa)	/*                  */
#define	MCFSIM_CSAR6		(MCF_MBAR + 0xac)	/*                  */
#define	MCFSIM_CSMR6		(MCF_MBAR + 0xb0)	/*               */
#define	MCFSIM_CSCR6		(MCF_MBAR + 0xb6)	/*                  */
#define	MCFSIM_CSAR7		(MCF_MBAR + 0xb8)	/*                  */
#define	MCFSIM_CSMR7		(MCF_MBAR + 0xbc)	/*               */
#define	MCFSIM_CSCR7		(MCF_MBAR + 0xc2)	/*                  */
#define	MCFSIM_DMCR		(MCF_MBAR + 0xc6)	/*                 */

#ifdef CONFIG_M5206e
#define	MCFSIM_PAR		(MCF_MBAR + 0xca)	/*                */
#else
#define	MCFSIM_PAR		(MCF_MBAR + 0xcb)	/*                */
#endif

#define	MCFTIMER_BASE1		(MCF_MBAR + 0x100)	/*                */
#define	MCFTIMER_BASE2		(MCF_MBAR + 0x120)	/*                */

#define	MCFSIM_PADDR		(MCF_MBAR + 0x1c5)	/*                          */
#define	MCFSIM_PADAT		(MCF_MBAR + 0x1c9)	/*                           */

#define	MCFDMA_BASE0		(MCF_MBAR + 0x200)	/*                    */
#define	MCFDMA_BASE1		(MCF_MBAR + 0x240)	/*                    */

#if defined(CONFIG_NETtel)
#define	MCFUART_BASE0		(MCF_MBAR + 0x180)	/*                    */
#define	MCFUART_BASE1		(MCF_MBAR + 0x140)	/*                    */
#else
#define	MCFUART_BASE0		(MCF_MBAR + 0x140)	/*                    */
#define	MCFUART_BASE1		(MCF_MBAR + 0x180)	/*                    */
#endif

/*
                                      
 */
#define	MCF_IRQ_TIMER		30		/*                 */
#define	MCF_IRQ_PROFILER	31		/*                 */
#define	MCF_IRQ_UART0		73		/*       */
#define	MCF_IRQ_UART1		74		/*       */

/*
               
 */
#define MCFGPIO_PIN_MAX		8
#define MCFGPIO_IRQ_VECBASE	-1
#define MCFGPIO_IRQ_MAX		-1

/*
                                                                    
 */
#ifdef CONFIG_M5206e
#define MCFSIM_PAR_DREQ0        0x100           /*                           */
                                                /*                          */
#define MCFSIM_PAR_DREQ1        0x200           /*                    */
                                                /*                           */
#endif

/*
                                                         
 */
#define	MCFSIM_SWDICR		MCFSIM_ICR8	/*                    */
#define	MCFSIM_TIMER1ICR	MCFSIM_ICR9	/*             */
#define	MCFSIM_TIMER2ICR	MCFSIM_ICR10	/*             */
#define	MCFSIM_UART1ICR		MCFSIM_ICR12	/*            */
#define	MCFSIM_UART2ICR		MCFSIM_ICR13	/*            */
#ifdef CONFIG_M5206e
#define	MCFSIM_DMA1ICR		MCFSIM_ICR14	/*           */
#define	MCFSIM_DMA2ICR		MCFSIM_ICR15	/*           */
#endif

/*                                                                          */
#endif	/*            */
