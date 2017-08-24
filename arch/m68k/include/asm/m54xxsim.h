/*
                                                                    
 */

#ifndef	m54xxsim_h
#define m54xxsim_h

#define	CPU_NAME		"COLDFIRE(m54xx)"
#define	CPU_INSTR_PER_JIFFY	2
#define	MCF_BUSCLK		(MCF_CLK / 2)

#include <asm/m54xxacr.h>

#define MCFINT_VECBASE		64

/*
                                      
 */
#define MCFICM_INTC0		(MCF_MBAR + 0x700) 	/*                           */

#define MCFINTC_IPRH		0x00		/*                         */
#define MCFINTC_IPRL		0x04		/*                        */
#define MCFINTC_IMRH		0x08		/*                      */
#define MCFINTC_IMRL		0x0c		/*                     */
#define MCFINTC_INTFRCH		0x10		/*                       */
#define MCFINTC_INTFRCL		0x14		/*                      */
#define MCFINTC_IRLR		0x18		/* */
#define MCFINTC_IACKL		0x19		/* */
#define MCFINTC_ICR0		0x40		/*                   */

/*
               
 */
#define MCFUART_BASE0		(MCF_MBAR + 0x8600)	/*                    */
#define MCFUART_BASE1		(MCF_MBAR + 0x8700)	/*                    */
#define MCFUART_BASE2		(MCF_MBAR + 0x8800)	/*                    */
#define MCFUART_BASE3		(MCF_MBAR + 0x8900)	/*                    */

/*
                                      
 */
#define MCF_IRQ_TIMER		(MCFINT_VECBASE + 54)	/*               */
#define MCF_IRQ_PROFILER	(MCFINT_VECBASE + 53)	/*               */
#define MCF_IRQ_UART0		(MCFINT_VECBASE + 35)
#define MCF_IRQ_UART1		(MCFINT_VECBASE + 34)
#define MCF_IRQ_UART2		(MCFINT_VECBASE + 33)
#define MCF_IRQ_UART3		(MCFINT_VECBASE + 32)

/*
                       
 */
#define MCFSLT_TIMER0		(MCF_MBAR + 0x900)	/*                  */
#define MCFSLT_TIMER1		(MCF_MBAR + 0x910)	/*                  */

/*
                       
 */
#define MCFGPIO_PIN_MAX		0	/*                        */
#define MCFGPIO_IRQ_MAX		-1
#define MCFGPIO_IRQ_VECBASE	-1

/*
                     
 */
#define	MCFEPORT_EPPAR		(MCF_MBAR + 0xf00)	/*                */
#define	MCFEPORT_EPDDR		(MCF_MBAR + 0xf04)	/*                */
#define	MCFEPORT_EPIER		(MCF_MBAR + 0xf05)	/*                  */
#define	MCFEPORT_EPDR		(MCF_MBAR + 0xf08)	/*               */
#define	MCFEPORT_EPPDR		(MCF_MBAR + 0xf09)	/*               */
#define	MCFEPORT_EPFR		(MCF_MBAR + 0xf0c)	/*       */

/*
                                      
 */
#define MCFGPIO_PAR_FBCTL	(MCF_MBAR + 0xA40)
#define MCFGPIO_PAR_FBCS	(MCF_MBAR + 0xA42)
#define MCFGPIO_PAR_DMA		(MCF_MBAR + 0xA43)
#define MCFGPIO_PAR_FECI2CIRQ	(MCF_MBAR + 0xA44)
#define MCFGPIO_PAR_PCIBG	(MCF_MBAR + 0xA48)	/*               */
#define MCFGPIO_PAR_PCIBR	(MCF_MBAR + 0xA4A)	/*     */
#define MCFGPIO_PAR_PSC0	(MCF_MBAR + 0xA4F)
#define MCFGPIO_PAR_PSC1	(MCF_MBAR + 0xA4E)
#define MCFGPIO_PAR_PSC2	(MCF_MBAR + 0xA4D)
#define MCFGPIO_PAR_PSC3	(MCF_MBAR + 0xA4C)
#define MCFGPIO_PAR_DSPI	(MCF_MBAR + 0xA50)
#define MCFGPIO_PAR_TIMER	(MCF_MBAR + 0xA52)

#define MCF_PAR_SDA		(0x0008)
#define MCF_PAR_SCL		(0x0004)
#define MCF_PAR_PSC_TXD		(0x04)
#define MCF_PAR_PSC_RXD		(0x08)
#define MCF_PAR_PSC_CTS_GPIO	(0x00)
#define MCF_PAR_PSC_CTS_BCLK	(0x80)
#define MCF_PAR_PSC_CTS_CTS	(0xC0)
#define MCF_PAR_PSC_RTS_GPIO    (0x00)
#define MCF_PAR_PSC_RTS_FSYNC	(0x20)
#define MCF_PAR_PSC_RTS_RTS	(0x30)
#define MCF_PAR_PSC_CANRX	(0x40)

#endif	/*            */
