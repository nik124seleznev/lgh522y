/*
 * Chip-specific header file for the SAMA5D3 family
 *
 *  Copyright (C) 2013 Atmel,
 *                2013 Ludovic Desroches <ludovic.desroches@atmel.com>
 *
 * Common definitions.
 * Based on SAMA5D3 datasheet.
 *
 * Licensed under GPLv2 or later.
 */

#ifndef SAMA5D3_H
#define SAMA5D3_H

/*
                                     
 */
#define AT91_ID_FIQ		 0	/*                                     */
#define AT91_ID_SYS		 1	/*                    */
#define SAMA5D3_ID_DBGU		 2	/*                                                */
#define AT91_ID_PIT		 3	/*     */
#define SAMA5D3_ID_WDT		 4	/*                          */
#define SAMA5D3_ID_HSMC		 5	/*                          */
#define SAMA5D3_ID_PIOA		 6	/*      */
#define SAMA5D3_ID_PIOB		 7	/*      */
#define SAMA5D3_ID_PIOC		 8	/*      */
#define SAMA5D3_ID_PIOD		 9	/*      */
#define SAMA5D3_ID_PIOE		10	/*      */
#define SAMA5D3_ID_SMD		11	/*                */
#define SAMA5D3_ID_USART0	12	/*        */
#define SAMA5D3_ID_USART1	13	/*        */
#define SAMA5D3_ID_USART2	14	/*        */
#define SAMA5D3_ID_USART3	15	/*        */
#define SAMA5D3_ID_UART0	16	/*        */
#define SAMA5D3_ID_UART1	17	/*        */
#define SAMA5D3_ID_TWI0		18	/*                      */
#define SAMA5D3_ID_TWI1		19	/*                      */
#define SAMA5D3_ID_TWI2		20	/*                      */
#define SAMA5D3_ID_HSMCI0	21	/*     */
#define SAMA5D3_ID_HSMCI1	22	/*     */
#define SAMA5D3_ID_HSMCI2	23	/*     */
#define SAMA5D3_ID_SPI0		24	/*                               */
#define SAMA5D3_ID_SPI1		25	/*                               */
#define SAMA5D3_ID_TC0		26	/*                 */
#define SAMA5D3_ID_TC1		27	/*                 */
#define SAMA5D3_ID_PWM		28	/*                                   */
#define SAMA5D3_ID_ADC		29	/*                             */
#define SAMA5D3_ID_DMA0		30	/*                  */
#define SAMA5D3_ID_DMA1		31	/*                  */
#define SAMA5D3_ID_UHPHS	32	/*                     */
#define SAMA5D3_ID_UDPHS	33	/*                       */
#define SAMA5D3_ID_GMAC		34	/*                      */
#define SAMA5D3_ID_EMAC		35	/*              */
#define SAMA5D3_ID_LCDC		36	/*                */
#define SAMA5D3_ID_ISI		37	/*                        */
#define SAMA5D3_ID_SSC0		38	/*                                 */
#define SAMA5D3_ID_SSC1		39	/*                                 */
#define SAMA5D3_ID_CAN0		40	/*                  */
#define SAMA5D3_ID_CAN1		41	/*                  */
#define SAMA5D3_ID_SHA		42	/*                       */
#define SAMA5D3_ID_AES		43	/*                              */
#define SAMA5D3_ID_TDES		44	/*                                 */
#define SAMA5D3_ID_TRNG		45	/*                              */
#define SAMA5D3_ID_IRQ0		47	/*                                      */

/*
                     
 */
#define SAMA5D3_BASE_RTC	0xfffffeb0

/*
                  
 */
#define SAMA5D3_SRAM_BASE	0x00300000	/*                            */
#define SAMA5D3_SRAM_SIZE	(128 * SZ_1K)	/*                            */

#endif
