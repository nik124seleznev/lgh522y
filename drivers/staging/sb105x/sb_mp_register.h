
/*
 * SB105X_UART.h
 *
 * Copyright (C) 2008 systembase
 *
 * UART registers.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef UART_SB105X_H
#define UART_SB105X_H

/* 
                   
 */

/*                             */
#define MP_OPTR_DIR0		0x04 	/*               */
#define MP_OPTR_DIR1		0x05 	/*                */
#define MP_OPTR_DIR2		0x06 	/*                 */
#define MP_OPTR_DIR3		0x07 	/*                 */

#define DIR_UART_16C550 	0
#define DIR_UART_16C1050	1
#define DIR_UART_16C1050A	2

#define	DIR_CLK_1843200		0x0		/*                        */
#define	DIR_CLK_3686400		0x1		/*                        */
#define	DIR_CLK_7372800		0x2		/*                        */
#define	DIR_CLK_14745600	0x3		/*                         */
#define	DIR_CLK_29491200	0x4		/*                         */
#define	DIR_CLK_58985400	0x5		/*                         */

/*                                */
#define MP_OPTR_IIR0		0x08 	/*               */
#define MP_OPTR_IIR1		0x09 	/*                */
#define MP_OPTR_IIR2		0x0A 	/*                 */
#define MP_OPTR_IIR3		0x0B 	/*                 */

#define IIR_RS232		0x00		/*            */
#define IIR_RS422		0x10		/*            */
#define IIR_RS485		0x20		/*            */
#define IIR_TYPE_MASK		0x30

/*                         */
#define MP_OPTR_IMR0		0x0C 	/*               */
#define MP_OPTR_IMR1		0x0D 	/*                */
#define MP_OPTR_IMR2		0x0E 	/*                 */
#define MP_OPTR_IMR3		0x0F 	/*                 */

/*                         */
#define MP_OPTR_IPR0		0x10 	/*               */
#define MP_OPTR_IPR1		0x11 	/*                */
#define MP_OPTR_IPR2		0x12 	/*                 */
#define MP_OPTR_IPR3		0x13 	/*                 */

/*                                         */
#define MP_OPTR_GPOCR		0x20

/*                                      */
#define MP_OPTR_GPODR		0x21

/*                                       */
#define MP_OPTR_PAFR		0x23

/*
                                   
 */

#define PAGE_0 0
#define PAGE_1 1
#define PAGE_2 2
#define PAGE_3 3
#define PAGE_4 4

/*
                                                                      
                                                                      
                                                                      
 */

#define SB105X_RX		0	/*                     */
#define SB105X_TX		0	/*                      */

#define SB105X_IER		1	/*                                */

#define SB105X_IER_CTSI	  	0x80	/*                                             */
#define SB105X_IER_RTSI	  	0x40	/*                                             */
#define SB105X_IER_XOI	  	0x20	/*                                             */
#define SB105X_IER_SME	  	0x10	/*                                         */
#define SB105X_IER_MSI	  	0x08	/*                               */
#define SB105X_IER_RLSI	  	0x04	/*                                       */
#define SB105X_IER_THRI	  	0x02	/*                                          */
#define SB105X_IER_RDI	  	0x01	/*                                */

#define SB105X_ISR		2	/*                            */

#define SB105X_ISR_NOINT	0x01	/*                       */
#define SB105X_ISR_RLSI	  	0x06	/*                                              */
#define SB105X_ISR_RDAI	  	0x0c	/*                                  */
#define SB105X_ISR_CTII	  	0x04	/*                                        */
#define SB105X_ISR_THRI	  	0x02	/*                                    */
#define SB105X_ISR_MSI	  	0x00	/*                        */
#define SB105X_ISR_RXCI	  	0x10	/*                                             */
#define SB105X_ISR_RCSI	  	0x20	/*                                                              */

#define SB105X_FCR		2	/*                            */

#define SB105X_FCR_FEN    	0x01	/*             */
#define SB105X_FCR_RXFR	  	0x02	/*               */
#define SB105X_FCR_TXFR	  	0x04	/*               */
#define SB105X_FCR_DMS	  	0x08	/*                 */

#define SB105X_FCR_RTR08  	0x00	/*                                */
#define SB105X_FCR_RTR16  	0x40  /*                                 */
#define SB105X_FCR_RTR56  	0x80  /*                                 */
#define SB105X_FCR_RTR60  	0xc0  /*                                 */
#define SB105X_FCR_TTR08  	0x00  /*                                 */
#define SB105X_FCR_TTR16	0x10  /*                                  */
#define SB105X_FCR_TTR32	0x20  /*                                  */
#define SB105X_FCR_TTR56	0x30  /*                                  */

#define SB105X_LCR		3	/*                            */
/*
                                                                          
                                                                 
 */
#define SB105X_LCR_DLAB   	0x80  /*                      */
#define SB105X_LCR_SBC    	0x40  /*             */
#define SB105X_LCR_SPAR   	0x20  /*                  */
#define SB105X_LCR_EPAR   	0x10  /*                    */
#define SB105X_LCR_PAREN  	0x08  /*               */
#define SB105X_LCR_STOP   	0x04  /*                                                                   */
#define SB105X_LCR_WLEN5  	0x00  /*                    */
#define SB105X_LCR_WLEN6  	0x01  /*                    */
#define SB105X_LCR_WLEN7  	0x02  /*                    */
#define SB105X_LCR_WLEN8  	0x03  /*                    */

#define SB105X_LCR_BF		0xBF

#define SB105X_MCR		4	/*                             */
#define SB105X_MCR_CPS    	0x80  /*                        */
#define SB105X_MCR_P2S    	0x40  /*                                               */
#define SB105X_MCR_XOA    	0x20  /*                */
#define SB105X_MCR_ILB		0x10  /*                          */
#define SB105X_MCR_OUT2		0x08  /*                             */
#define SB105X_MCR_OUT1		0x04  /*                              */
#define SB105X_MCR_RTS    	0x02  /*             */
#define SB105X_MCR_DTR    	0x01  /*             */

#define SB105X_LSR		5	/*                           */
#define SB105X_LSR_RFEI   	0x80  /*                                   */
#define SB105X_LSR_TEMI   	0x40  /*                             */
#define SB105X_LSR_THRE		0x20  /*                     */
#define SB105X_LSR_BII		0x10  /*                           */
#define SB105X_LSR_FEI		0x08  /*                       */
#define SB105X_LSR_PEI		0x04  /*                        */
#define SB105X_LSR_OEI		0x02  /*                         */
#define SB105X_LSR_RDRI		0x01  /*                             */

#define SB105X_MSR		6	/*                            */
#define SB105X_MSR_DCD		0x80  /*                     */
#define SB105X_MSR_RI		0x40  /*                */
#define SB105X_MSR_DSR		0x20  /*                */
#define SB105X_MSR_CTS		0x10  /*               */
#define SB105X_MSR_DDCD		0x08  /*           */
#define SB105X_MSR_DRI		0x04  /*                      */
#define SB105X_MSR_DDSR		0x02  /*           */
#define SB105X_MSR_DCTS		0x01  /*           */

#define SB105XA_MDR		6	/*                               */
#define SB105XA_MDR_NPS		0x08  /*                         */
#define SB105XA_MDR_AME		0x02  /*                        */
#define SB105XA_MDR_MDE		0x01  /*                   */

#define SB105X_SPR		7	/*                       */

/*
         
 */
#define SB105X_DLL		0	/*                        */
#define SB105X_DLM		1	/*                         */

/*
                                                                      
                                                                      
                                                                      
 */
#define SB105X_GICR		1	/*                                   */
#define SB105X_GICR_GIM   	0x01  /*                       */

#define SB105X_GISR		2	/*                                  */
#define SB105X_GISR_MGICR0  	0x80  /*                               */
#define SB105X_GISR_CS3IS   	0x08  /*                                 */
#define SB105X_GISR_CS2IS   	0x04  /*                                 */
#define SB105X_GISR_CS1IS   	0x02  /*                                 */
#define SB105X_GISR_CS0IS   	0x01  /*                                 */

#define SB105X_TFCR		5	/*                              */

#define SB105X_RFCR		6	/*                             */

#define	SB105X_FSR		7	/*                              */
#define SB105X_FSR_THFS     	0x20  /*                                       */
#define SB105X_FSR_TSFS     	0x10  /*                                       */
#define SB105X_FSR_RHFS     	0x02  /*                                      */
#define SB105X_FSR_RSFS     	0x01  /*                                      */

/*
                                                                      
                                                                      
                                                                      
 */

#define SB105X_PSR		0	/*                      */
#define SB105X_PSR_P3KEY    	0xA4 /*                   */
#define SB105X_PSR_P4KEY    	0xA5 /*                   */

#define SB105X_ATR		1	/*                              */
#define SB105X_ATR_RPS      	0x80  /*                      */
#define SB105X_ATR_RCMS     	0x40  /*                          */
#define SB105X_ATR_TPS      	0x20  /*                      */
#define SB105X_ATR_TCMS     	0x10  /*                          */
#define SB105X_ATR_ATDIS    	0x00  /*                         */
#define SB105X_ATR_ART      	0x01  /*                                */
#define SB105X_ATR_ADT      	0x02  /*                                */
#define SB105X_ATR_A80      	0x03  /*                    */

#define SB105X_EFR		2	/*                                  */
#define SB105X_EFR_ACTS     	0x80  /*                              */
#define SB105X_EFR_ARTS     	0x40  /*                              */
#define SB105X_EFR_SCD      	0x20  /*                          */
#define SB105X_EFR_EFBEN    	0x10  /*                               */

#define SB105X_XON1		4	/*                         */
#define SB105X_XON2		5	/*                         */
#define SB105X_XOFF1		6	/*                          */
#define SB105X_XOFF2		7	/*                          */

/*
                                                                      
                                                                      
                                                                      
 */

#define SB105X_AFR		1	/*                             */
#define SB105X_AFR_GIPS     	0x20  /*                                  */
#define SB105X_AFR_GIEN     	0x10  /*                         */
#define SB105X_AFR_AFEN     	0x01  /*                      */

#define SB105X_XRCR		2	/*                                 */
#define SB105X_XRCR_NRC1    	0x00  /*                                                                                       */
#define SB105X_XRCR_NRC4    	0x01  /*                                                                                       */
#define SB105X_XRCR_NRC8    	0x02  /*                                                                                       */
#define SB105X_XRCR_NRC16   	0x03  /*                                                                                        */

#define SB105X_TTR		4	/*                                      */
#define SB105X_RTR		5	/*                                     */
#define SB105X_FUR		6	/*                                       */
#define SB105X_FLR		7	/*                                       */


/*        */

#define SB105X_GET_CHAR(port)	inb((port)->iobase + SB105X_RX)
#define SB105X_GET_IER(port)	inb((port)->iobase + SB105X_IER)
#define SB105X_GET_ISR(port)	inb((port)->iobase + SB105X_ISR)
#define SB105X_GET_LCR(port)	inb((port)->iobase + SB105X_LCR)
#define SB105X_GET_MCR(port)	inb((port)->iobase + SB105X_MCR)
#define SB105X_GET_LSR(port)	inb((port)->iobase + SB105X_LSR)
#define SB105X_GET_MSR(port)	inb((port)->iobase + SB105X_MSR)
#define SB105X_GET_SPR(port)	inb((port)->iobase + SB105X_SPR)

#define SB105X_PUT_CHAR(port,v)	outb((v),(port)->iobase + SB105X_TX )
#define SB105X_PUT_IER(port,v)	outb((v),(port)->iobase + SB105X_IER )
#define SB105X_PUT_FCR(port,v)	outb((v),(port)->iobase + SB105X_FCR )
#define SB105X_PUT_LCR(port,v)	outb((v),(port)->iobase + SB105X_LCR )
#define SB105X_PUT_MCR(port,v)	outb((v),(port)->iobase + SB105X_MCR )
#define SB105X_PUT_SPR(port,v)	outb((v),(port)->iobase + SB105X_SPR )


/*        */
#define SB105X_GET_REG(port,reg)	inb((port)->iobase + (reg))
#define SB105X_PUT_REG(port,reg,v)	outb((v),(port)->iobase + (reg))

/*        */

#define SB105X_PUT_PSR(port,v)	outb((v),(port)->iobase + SB105X_PSR )

#endif 
