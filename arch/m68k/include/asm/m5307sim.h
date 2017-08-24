/*                                                                          */

/*
 *	m5307sim.h -- ColdFire 5307 System Integration Module support.
 *
 *	(C) Copyright 1999,  Moreton Bay Ventures Pty Ltd.
 *	(C) Copyright 1999,  Lineo (www.lineo.com)
 *
 *      Modified by David W. Miller for the MCF5307 Eval Board.
 */

/*                                                                          */
#ifndef	m5307sim_h
#define	m5307sim_h
/*                                                                          */

#define	CPU_NAME		"COLDFIRE(m5307)"
#define	CPU_INSTR_PER_JIFFY	3
#define	MCF_BUSCLK		(MCF_CLK / 2)

#include <asm/m53xxacr.h>

/*
                                              
 */
#define	MCFSIM_RSR		(MCF_MBAR + 0x00)	/*                  */
#define	MCFSIM_SYPCR		(MCF_MBAR + 0x01)	/*                   */
#define	MCFSIM_SWIVR		(MCF_MBAR + 0x02)	/*                  */
#define	MCFSIM_SWSR		(MCF_MBAR + 0x03)	/*                    */
#define	MCFSIM_PAR		(MCF_MBAR + 0x04)	/*                */
#define	MCFSIM_IRQPAR		(MCF_MBAR + 0x06)	/*                */
#define	MCFSIM_PLLCR		(MCF_MBAR + 0x08)	/*              */
#define	MCFSIM_MPARK		(MCF_MBAR + 0x0C)	/*                 */
#define	MCFSIM_IPR		(MCF_MBAR + 0x40)	/*                */
#define	MCFSIM_IMR		(MCF_MBAR + 0x44)	/*                */
#define	MCFSIM_AVR		(MCF_MBAR + 0x4b)	/*                 */
#define	MCFSIM_ICR0		(MCF_MBAR + 0x4c)	/*                 */
#define	MCFSIM_ICR1		(MCF_MBAR + 0x4d)	/*                 */
#define	MCFSIM_ICR2		(MCF_MBAR + 0x4e)	/*                 */
#define	MCFSIM_ICR3		(MCF_MBAR + 0x4f)	/*                 */
#define	MCFSIM_ICR4		(MCF_MBAR + 0x50)	/*                 */
#define	MCFSIM_ICR5		(MCF_MBAR + 0x51)	/*                 */
#define	MCFSIM_ICR6		(MCF_MBAR + 0x52)	/*                 */
#define	MCFSIM_ICR7		(MCF_MBAR + 0x53)	/*                 */
#define	MCFSIM_ICR8		(MCF_MBAR + 0x54)	/*                 */
#define	MCFSIM_ICR9		(MCF_MBAR + 0x55)	/*                 */
#define	MCFSIM_ICR10		(MCF_MBAR + 0x56)	/*                  */
#define	MCFSIM_ICR11		(MCF_MBAR + 0x57)	/*                  */

#define MCFSIM_CSAR0		(MCF_MBAR + 0x80)	/*                  */
#define MCFSIM_CSMR0		(MCF_MBAR + 0x84)	/*               */
#define MCFSIM_CSCR0		(MCF_MBAR + 0x8a)	/*                  */
#define MCFSIM_CSAR1		(MCF_MBAR + 0x8c)	/*                  */
#define MCFSIM_CSMR1		(MCF_MBAR + 0x90)	/*               */
#define MCFSIM_CSCR1		(MCF_MBAR + 0x96)	/*                  */

#ifdef CONFIG_OLDMASK
#define MCFSIM_CSBAR		(MCF_MBAR + 0x98)	/*                 */
#define MCFSIM_CSBAMR		(MCF_MBAR + 0x9c)	/*              */
#define MCFSIM_CSMR2		(MCF_MBAR + 0x9e)	/*               */
#define MCFSIM_CSCR2		(MCF_MBAR + 0xa2)	/*                  */
#define MCFSIM_CSMR3		(MCF_MBAR + 0xaa)	/*               */
#define MCFSIM_CSCR3		(MCF_MBAR + 0xae)	/*                  */
#define MCFSIM_CSMR4		(MCF_MBAR + 0xb6)	/*               */
#define MCFSIM_CSCR4		(MCF_MBAR + 0xba)	/*                  */
#define MCFSIM_CSMR5		(MCF_MBAR + 0xc2)	/*               */
#define MCFSIM_CSCR5		(MCF_MBAR + 0xc6)	/*                  */
#define MCFSIM_CSMR6		(MCF_MBAR + 0xce)	/*               */
#define MCFSIM_CSCR6		(MCF_MBAR + 0xd2)	/*                  */
#define MCFSIM_CSMR7		(MCF_MBAR + 0xda)	/*               */
#define MCFSIM_CSCR7		(MCF_MBAR + 0xde)	/*                  */
#else
#define MCFSIM_CSAR2		(MCF_MBAR + 0x98)	/*                  */
#define MCFSIM_CSMR2		(MCF_MBAR + 0x9c)	/*               */
#define MCFSIM_CSCR2		(MCF_MBAR + 0xa2)	/*                  */
#define MCFSIM_CSAR3		(MCF_MBAR + 0xa4)	/*                  */
#define MCFSIM_CSMR3		(MCF_MBAR + 0xa8)	/*               */
#define MCFSIM_CSCR3		(MCF_MBAR + 0xae)	/*                  */
#define MCFSIM_CSAR4		(MCF_MBAR + 0xb0)	/*                  */
#define MCFSIM_CSMR4		(MCF_MBAR + 0xb4)	/*               */
#define MCFSIM_CSCR4		(MCF_MBAR + 0xba)	/*                  */
#define MCFSIM_CSAR5		(MCF_MBAR + 0xbc)	/*                  */
#define MCFSIM_CSMR5		(MCF_MBAR + 0xc0)	/*               */
#define MCFSIM_CSCR5		(MCF_MBAR + 0xc6)	/*                  */
#define MCFSIM_CSAR6		(MCF_MBAR + 0xc8)	/*                  */
#define MCFSIM_CSMR6		(MCF_MBAR + 0xcc)	/*               */
#define MCFSIM_CSCR6		(MCF_MBAR + 0xd2)	/*                  */
#define MCFSIM_CSAR7		(MCF_MBAR + 0xd4)	/*                  */
#define MCFSIM_CSMR7		(MCF_MBAR + 0xd8)	/*               */
#define MCFSIM_CSCR7		(MCF_MBAR + 0xde)	/*                  */
#endif /*                */

#define MCFSIM_DCR		(MCF_MBAR + 0x100)	/*              */
#define MCFSIM_DACR0		(MCF_MBAR + 0x108)	/*                  */
#define MCFSIM_DMR0		(MCF_MBAR + 0x10c)	/*             */
#define MCFSIM_DACR1		(MCF_MBAR + 0x110)	/*                  */
#define MCFSIM_DMR1		(MCF_MBAR + 0x114)	/*             */

/*
                 
 */
#define MCFTIMER_BASE1		(MCF_MBAR + 0x140)	/*                */
#define MCFTIMER_BASE2		(MCF_MBAR + 0x180)	/*                */

#define	MCFSIM_PADDR		(MCF_MBAR + 0x244)
#define	MCFSIM_PADAT		(MCF_MBAR + 0x248)

/*
                            
 */
#define MCFDMA_BASE0		(MCF_MBAR + 0x300)	/*                    */
#define MCFDMA_BASE1		(MCF_MBAR + 0x340)	/*                    */
#define MCFDMA_BASE2		(MCF_MBAR + 0x380)	/*                    */
#define MCFDMA_BASE3		(MCF_MBAR + 0x3C0)	/*                    */

/*
                
 */
#if defined(CONFIG_NETtel) || defined(CONFIG_SECUREEDGEMP3)
#define MCFUART_BASE0		(MCF_MBAR + 0x200)	/*                    */
#define MCFUART_BASE1		(MCF_MBAR + 0x1c0)	/*                    */
#else
#define MCFUART_BASE0		(MCF_MBAR + 0x1c0)	/*                    */
#define MCFUART_BASE1		(MCF_MBAR + 0x200)	/*                    */
#endif

/*
                       
 */
#define MCFGPIO_PIN_MAX		16
#define MCFGPIO_IRQ_MAX		-1
#define MCFGPIO_IRQ_VECBASE	-1


/*                                                       */

#define	MCF5307_CS2		(0x400000)
#define	MCF5307_CS3		(0x600000)
#define	MCF5307_CS4		(0x800000)
#define	MCF5307_CS5		(0xA00000)
#define	MCF5307_CS6		(0xC00000)
#define	MCF5307_CS7		(0xE00000)


/*
                                       
 */
#define	MCFSIM_SWDICR		MCFSIM_ICR0	/*                    */
#define	MCFSIM_TIMER1ICR	MCFSIM_ICR1	/*             */
#define	MCFSIM_TIMER2ICR	MCFSIM_ICR2	/*             */
#define	MCFSIM_UART1ICR		MCFSIM_ICR4	/*            */
#define	MCFSIM_UART2ICR		MCFSIM_ICR5	/*            */
#define	MCFSIM_DMA0ICR		MCFSIM_ICR6	/*           */
#define	MCFSIM_DMA1ICR		MCFSIM_ICR7	/*           */
#define	MCFSIM_DMA2ICR		MCFSIM_ICR8	/*           */
#define	MCFSIM_DMA3ICR		MCFSIM_ICR9	/*           */


/*
                                                                    
 */
#define MCFSIM_PAR_DREQ0        0x40            /*                           */
                                                /*                         */
#define MCFSIM_PAR_DREQ1        0x20            /*                    */
                                                /*                         */

/*
                                        
 */
#define IRQ5_LEVEL4		0x80
#define IRQ3_LEVEL6		0x40
#define IRQ1_LEVEL2		0x20

/*
                                      
 */
#define	MCF_IRQ_TIMER		30		/*                 */
#define	MCF_IRQ_PROFILER	31		/*                 */
#define	MCF_IRQ_UART0		73		/*       */
#define	MCF_IRQ_UART1		74		/*       */

/*                                                                          */
#endif	/*            */
