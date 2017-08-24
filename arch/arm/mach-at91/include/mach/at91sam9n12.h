/*
 * SoC specific header file for the AT91SAM9N12
 *
 * Copyright (C) 2012 Atmel Corporation
 *
 * Common definitions, based on AT91SAM9N12 SoC datasheet
 *
 * Licensed under GPLv2 or later
 */

#ifndef _AT91SAM9N12_H_
#define _AT91SAM9N12_H_

/*
                                     
 */
#define AT91SAM9N12_ID_PIOAB	2	/*                                 */
#define AT91SAM9N12_ID_PIOCD	3	/*                                 */
#define AT91SAM9N12_ID_FUSE	4	/*                 */
#define AT91SAM9N12_ID_USART0	5	/*         */
#define AT91SAM9N12_ID_USART1	6	/*         */
#define AT91SAM9N12_ID_USART2	7	/*         */
#define AT91SAM9N12_ID_USART3	8	/*         */
#define AT91SAM9N12_ID_TWI0	9	/*                      */
#define AT91SAM9N12_ID_TWI1	10	/*                      */
#define AT91SAM9N12_ID_MCI	12	/*                                      */
#define AT91SAM9N12_ID_SPI0	13	/*                               */
#define AT91SAM9N12_ID_SPI1	14	/*                               */
#define AT91SAM9N12_ID_UART0	15	/*        */
#define AT91SAM9N12_ID_UART1	16	/*        */
#define AT91SAM9N12_ID_TCB	17	/*                                   */
#define AT91SAM9N12_ID_PWM	18	/*                                   */
#define AT91SAM9N12_ID_ADC	19	/*                */
#define AT91SAM9N12_ID_DMA	20	/*                */
#define AT91SAM9N12_ID_UHP	22	/*                     */
#define AT91SAM9N12_ID_UDP	23	/*                       */
#define AT91SAM9N12_ID_LCDC	25	/*                */
#define AT91SAM9N12_ID_ISI	25	/*                        */
#define AT91SAM9N12_ID_SSC	28	/*                               */
#define AT91SAM9N12_ID_TRNG	30	/*      */
#define AT91SAM9N12_ID_IRQ0	31	/*                               */

/*
                                           
 */
#define AT91SAM9N12_BASE_USART0	0xf801c000
#define AT91SAM9N12_BASE_USART1	0xf8020000
#define AT91SAM9N12_BASE_USART2	0xf8024000
#define AT91SAM9N12_BASE_USART3	0xf8028000

/*
                     
 */
#define AT91SAM9N12_BASE_RTC	0xfffffeb0

/*
                   
 */
#define AT91SAM9N12_SRAM_BASE	0x00300000	/*                            */
#define AT91SAM9N12_SRAM_SIZE	SZ_32K		/*                           */

#define AT91SAM9N12_ROM_BASE	0x00100000	/*                           */
#define AT91SAM9N12_ROM_SIZE	SZ_128K		/*                           */

#endif
