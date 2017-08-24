/*                                                                          */

/*
 *	m525xsim.h -- ColdFire 525x System Integration Module support.
 *
 *	(C) Copyright 2012, Steven king <sfking@fdwdc.com>
 *	(C) Copyright 2002, Greg Ungerer (gerg@snapgear.com)
 */

/*                                                                          */
#ifndef	m525xsim_h
#define m525xsim_h
/*                                                                          */

/*
                                                                     
                                                                      
                              
 */
#define CPU_NAME		"COLDFIRE(m525x)"
#define CPU_INSTR_PER_JIFFY	3
#define MCF_BUSCLK		(MCF_CLK / 2)

#include <asm/m52xxacr.h>

/*
                                                         
 */
#define MCF_MBAR2		0x80000000

/*
                                              
 */
#define MCFSIM_RSR		(MCF_MBAR + 0x00)	/*              */
#define MCFSIM_SYPCR		(MCF_MBAR + 0x01)	/*                   */
#define MCFSIM_SWIVR		(MCF_MBAR + 0x02)	/*                  */
#define MCFSIM_SWSR		(MCF_MBAR + 0x03)	/*                 */
#define MCFSIM_MPARK		(MCF_MBAR + 0x0C)	/*                 */
#define MCFSIM_IPR		(MCF_MBAR + 0x40)	/*                   */
#define MCFSIM_IMR		(MCF_MBAR + 0x44)	/*                */
#define MCFSIM_ICR0		(MCF_MBAR + 0x4c)	/*                 */
#define MCFSIM_ICR1		(MCF_MBAR + 0x4d)	/*                 */
#define MCFSIM_ICR2		(MCF_MBAR + 0x4e)	/*                 */
#define MCFSIM_ICR3		(MCF_MBAR + 0x4f)	/*                 */
#define MCFSIM_ICR4		(MCF_MBAR + 0x50)	/*                 */
#define MCFSIM_ICR5		(MCF_MBAR + 0x51)	/*                 */
#define MCFSIM_ICR6		(MCF_MBAR + 0x52)	/*                 */
#define MCFSIM_ICR7		(MCF_MBAR + 0x53)	/*                 */
#define MCFSIM_ICR8		(MCF_MBAR + 0x54)	/*                 */
#define MCFSIM_ICR9		(MCF_MBAR + 0x55)	/*                 */
#define MCFSIM_ICR10		(MCF_MBAR + 0x56)	/*                  */
#define MCFSIM_ICR11		(MCF_MBAR + 0x57)	/*                  */

#define MCFSIM_CSAR0		(MCF_MBAR + 0x80)	/*                  */
#define MCFSIM_CSMR0		(MCF_MBAR + 0x84)	/*               */
#define MCFSIM_CSCR0		(MCF_MBAR + 0x8a)	/*                  */
#define MCFSIM_CSAR1		(MCF_MBAR + 0x8c)	/*                  */
#define MCFSIM_CSMR1		(MCF_MBAR + 0x90)	/*               */
#define MCFSIM_CSCR1		(MCF_MBAR + 0x96)	/*                  */
#define MCFSIM_CSAR2		(MCF_MBAR + 0x98)	/*                  */
#define MCFSIM_CSMR2		(MCF_MBAR + 0x9c)	/*               */
#define MCFSIM_CSCR2		(MCF_MBAR + 0xa2)	/*                  */
#define MCFSIM_CSAR3		(MCF_MBAR + 0xa4)	/*                  */
#define MCFSIM_CSMR3		(MCF_MBAR + 0xa8)	/*               */
#define MCFSIM_CSCR3		(MCF_MBAR + 0xae)	/*                  */
#define MCFSIM_CSAR4		(MCF_MBAR + 0xb0)	/*                  */
#define MCFSIM_CSMR4		(MCF_MBAR + 0xb4)	/*               */
#define MCFSIM_CSCR4		(MCF_MBAR + 0xba)	/*                  */

#define MCFSIM_DCR		(MCF_MBAR + 0x100)	/*              */
#define MCFSIM_DACR0		(MCF_MBAR + 0x108)	/*                  */
#define MCFSIM_DMR0		(MCF_MBAR + 0x10c)	/*             */
#define MCFSIM_DACR1		(MCF_MBAR + 0x110)	/*                  */
#define MCFSIM_DMR1		(MCF_MBAR + 0x114)	/*             */

/*
                                            
*/
#define MCFINTC2_INTBASE	(MCF_MBAR2 + 0x168)	/*                 */
#define MCFINTC2_INTPRI1	(MCF_MBAR2 + 0x140)	/*              */
#define MCFINTC2_INTPRI2	(MCF_MBAR2 + 0x144)	/*               */
#define MCFINTC2_INTPRI3	(MCF_MBAR2 + 0x148)	/*                */
#define MCFINTC2_INTPRI4	(MCF_MBAR2 + 0x14c)	/*                */
#define MCFINTC2_INTPRI5	(MCF_MBAR2 + 0x150)	/*                */
#define MCFINTC2_INTPRI6	(MCF_MBAR2 + 0x154)	/*                */
#define MCFINTC2_INTPRI7	(MCF_MBAR2 + 0x158)	/*                */
#define MCFINTC2_INTPRI8	(MCF_MBAR2 + 0x15c)	/*                */

#define MCFINTC2_INTPRI_REG(i)	(MCFINTC2_INTPRI1 + \
				((((i) - MCFINTC2_VECBASE) / 8) * 4))
#define MCFINTC2_INTPRI_BITS(b, i)	((b) << (((i) % 8) * 4))

/*
                
 */
#define MCFTIMER_BASE1		(MCF_MBAR + 0x140)	/*                */
#define MCFTIMER_BASE2		(MCF_MBAR + 0x180)	/*                */

/*
               
 */
#define MCFUART_BASE0		(MCF_MBAR + 0x1c0)	/*                    */
#define MCFUART_BASE1		(MCF_MBAR + 0x200)	/*                    */

/*
               
 */
#define MCFQSPI_BASE		(MCF_MBAR + 0x300)	/*                   */
#define MCFQSPI_SIZE		0x40			/*                   */

#ifdef CONFIG_M5249
#define MCFQSPI_CS0		29
#define MCFQSPI_CS1		24
#define MCFQSPI_CS2		21
#define MCFQSPI_CS3		22
#else
#define MCFQSPI_CS0		15
#define MCFQSPI_CS1		16
#define MCFQSPI_CS2		24
#define MCFQSPI_CS3		28
#endif

/*
              
 */
#define MCFI2C_BASE0		(MCF_MBAR + 0x280)	/*                    */
#define MCFI2C_SIZE0		0x20			/*                   */

#define MCFI2C_BASE1		(MCF_MBAR2 + 0x440)	/*                    */
#define MCFI2C_SIZE1		0x20			/*                   */

/*
                           
 */
#define MCFDMA_BASE0		(MCF_MBAR + 0x300)	/*                    */
#define MCFDMA_BASE1		(MCF_MBAR + 0x340)	/*                    */
#define MCFDMA_BASE2		(MCF_MBAR + 0x380)	/*                    */
#define MCFDMA_BASE3		(MCF_MBAR + 0x3C0)	/*                    */

/*
                                       
 */
#define MCFSIM_SWDICR		MCFSIM_ICR0	/*                    */
#define MCFSIM_TIMER1ICR	MCFSIM_ICR1	/*             */
#define MCFSIM_TIMER2ICR	MCFSIM_ICR2	/*             */
#define MCFSIM_I2CICR		MCFSIM_ICR3	/*         */
#define MCFSIM_UART1ICR		MCFSIM_ICR4	/*            */
#define MCFSIM_UART2ICR		MCFSIM_ICR5	/*            */
#define MCFSIM_DMA0ICR		MCFSIM_ICR6	/*           */
#define MCFSIM_DMA1ICR		MCFSIM_ICR7	/*           */
#define MCFSIM_DMA2ICR		MCFSIM_ICR8	/*           */
#define MCFSIM_DMA3ICR		MCFSIM_ICR9	/*           */
#define MCFSIM_QSPIICR		MCFSIM_ICR10	/*          */

/*
                                      
 */
#define MCF_IRQ_QSPI		28		/*               */
#define MCF_IRQ_I2C0		29
#define MCF_IRQ_TIMER		30		/*                 */
#define MCF_IRQ_PROFILER	31		/*                 */

#define MCF_IRQ_UART0		73		/*       */
#define MCF_IRQ_UART1		74		/*       */

/*
                                                                 
                                                                      
                                 
 */
#define MCFINTC2_VECBASE	128

#define MCF_IRQ_GPIO0		(MCFINTC2_VECBASE + 32)
#define MCF_IRQ_GPIO1		(MCFINTC2_VECBASE + 33)
#define MCF_IRQ_GPIO2		(MCFINTC2_VECBASE + 34)
#define MCF_IRQ_GPIO3		(MCFINTC2_VECBASE + 35)
#define MCF_IRQ_GPIO4		(MCFINTC2_VECBASE + 36)
#define MCF_IRQ_GPIO5		(MCFINTC2_VECBASE + 37)
#define MCF_IRQ_GPIO6		(MCFINTC2_VECBASE + 38)
#define MCF_IRQ_GPIO7		(MCFINTC2_VECBASE + 39)

#define MCF_IRQ_USBWUP		(MCFINTC2_VECBASE + 40)
#define MCF_IRQ_I2C1		(MCFINTC2_VECBASE + 62)

/*
                                           
 */
#define MCFSIM2_GPIOREAD	(MCF_MBAR2 + 0x000)	/*                  */
#define MCFSIM2_GPIOWRITE	(MCF_MBAR2 + 0x004)	/*                   */
#define MCFSIM2_GPIOENABLE	(MCF_MBAR2 + 0x008)	/*              */
#define MCFSIM2_GPIOFUNC	(MCF_MBAR2 + 0x00C)	/*               */
#define MCFSIM2_GPIO1READ	(MCF_MBAR2 + 0x0B0)	/*                   */
#define MCFSIM2_GPIO1WRITE	(MCF_MBAR2 + 0x0B4)	/*                    */
#define MCFSIM2_GPIO1ENABLE	(MCF_MBAR2 + 0x0B8)	/*               */
#define MCFSIM2_GPIO1FUNC	(MCF_MBAR2 + 0x0BC)	/*                */

#define MCFSIM2_GPIOINTSTAT	(MCF_MBAR2 + 0xc0)	/*                  */
#define MCFSIM2_GPIOINTCLEAR	(MCF_MBAR2 + 0xc0)	/*                 */
#define MCFSIM2_GPIOINTENABLE	(MCF_MBAR2 + 0xc4)	/*                  */

#define MCFSIM2_DMAROUTE	(MCF_MBAR2 + 0x188)     /*             */
#define MCFSIM2_IDECONFIG1	(MCF_MBAR2 + 0x18c)	/*            */
#define MCFSIM2_IDECONFIG2	(MCF_MBAR2 + 0x190)	/*            */

/*
                       
 */
#define MCFGPIO_PIN_MAX		64
#ifdef CONFIG_M5249
#define MCFGPIO_IRQ_MAX		-1
#define MCFGPIO_IRQ_VECBASE	-1
#else
#define MCFGPIO_IRQ_MAX		7
#define MCFGPIO_IRQ_VECBASE	MCF_IRQ_GPIO0
#endif

/*                                                                          */

#ifdef __ASSEMBLER__
#ifdef CONFIG_M5249C3
/*
                                                                    
                                                                 
                             
 */
.macro m5249c3_setup
	/*
                                                      
  */
	movel	#0x10000001,%a0
	movec	%a0,%MBAR			/*                 */
	subql	#1,%a0				/*                        */

	movel	#0x80000001,%a1
	movec	%a1,#3086			/*                  */
	subql	#1,%a1				/*                         */

	/*
                                                       
  */
	moveb	#MCFINTC2_VECBASE,%d0
	moveb	%d0,0x16b(%a1)			/*                         */

	/*
                                                      
  */
	movel	#0x001F0021,%d0			/*                 */
	movel	%d0,0x84(%a0)			/*           */

	/*
                                                        
              
  */
	movel	0x180(%a1),%d0			/*                       */
	andl	#0xfffffffe,%d0			/*                  */
	movel	%d0,0x180(%a1)			/*                  */
	nop

#if CONFIG_CLOCK_FREQ == 140000000
	/*
                                                           
                                                          
                                    
  */
	movel	#0x125a40f0,%d0			/*                */
	movel	%d0,0x180(%a1)			/*                  */
	orl	#0x1,%d0
	movel	%d0,0x180(%a1)			/*                  */
#endif

	/*
                                      
                                
  */
	movel  #0xe0000000,%d0			/*                          */
	movel  %d0,0x8c(%a0)
	movel  #0x001f0021,%d0			/*                 */
	movel  %d0,0x90(%a0)
	movew  #0x0080,%d0			/*                      */
	movew  %d0,0x96(%a0)

	/*
                                
  */
	movel	#0x50000000,%d0			/*                          */
	movel	%d0,0x98(%a0)
	movel	#0x001f0001,%d0			/*                 */
	movel	%d0,0x9c(%a0)
	movew	#0x0080,%d0			/*                 */
	movew	%d0,0xa2(%a0)

	movel	#0x00107000,%d0			/*            */
	movel	%d0,0x18c(%a1)
	movel	#0x000c0400,%d0			/*            */
	movel	%d0,0x190(%a1)

	movel	#0x00080000,%d0			/*                       */
	orl	%d0,0xc(%a1)			/*                 */
	orl	%d0,0x8(%a1)			/*                         */
        orl	%d0,0x4(%a1)			/*                     */
.endm

#define	PLATFORM_SETUP	m5249c3_setup

#endif /*                */
#endif /*               */
/*                                                                          */
#endif	/*            */
