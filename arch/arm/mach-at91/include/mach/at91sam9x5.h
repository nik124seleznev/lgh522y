/*
 * Chip-specific header file for the AT91SAM9x5 family
 *
 *  Copyright (C) 2009-2012 Atmel Corporation.
 *
 * Common definitions.
 * Based on AT91SAM9x5 datasheet.
 *
 * Licensed under GPLv2 or later.
 */

#ifndef AT91SAM9X5_H
#define AT91SAM9X5_H

/*
                                     
 */
#define AT91SAM9X5_ID_PIOAB	2	/*                                 */
#define AT91SAM9X5_ID_PIOCD	3	/*                                 */
#define AT91SAM9X5_ID_SMD	4	/*                      */
#define AT91SAM9X5_ID_USART0	5	/*         */
#define AT91SAM9X5_ID_USART1	6	/*         */
#define AT91SAM9X5_ID_USART2	7	/*         */
#define AT91SAM9X5_ID_USART3	8	/*         */
#define AT91SAM9X5_ID_TWI0	9	/*                      */
#define AT91SAM9X5_ID_TWI1	10	/*                      */
#define AT91SAM9X5_ID_TWI2	11	/*                      */
#define AT91SAM9X5_ID_MCI0	12	/*                                        */
#define AT91SAM9X5_ID_SPI0	13	/*                               */
#define AT91SAM9X5_ID_SPI1	14	/*                               */
#define AT91SAM9X5_ID_UART0	15	/*        */
#define AT91SAM9X5_ID_UART1	16	/*        */
#define AT91SAM9X5_ID_TCB	17	/*                                   */
#define AT91SAM9X5_ID_PWM	18	/*                                   */
#define AT91SAM9X5_ID_ADC	19	/*                */
#define AT91SAM9X5_ID_DMA0	20	/*                  */
#define AT91SAM9X5_ID_DMA1	21	/*                  */
#define AT91SAM9X5_ID_UHPHS	22	/*                     */
#define AT91SAM9X5_ID_UDPHS	23	/*                       */
#define AT91SAM9X5_ID_EMAC0	24	/*               */
#define AT91SAM9X5_ID_LCDC	25	/*                */
#define AT91SAM9X5_ID_ISI	25	/*                        */
#define AT91SAM9X5_ID_MCI1	26	/*                                        */
#define AT91SAM9X5_ID_EMAC1	27	/*               */
#define AT91SAM9X5_ID_SSC	28	/*                               */
#define AT91SAM9X5_ID_CAN0	29	/*                  */
#define AT91SAM9X5_ID_CAN1	30	/*                  */
#define AT91SAM9X5_ID_IRQ0	31	/*                               */

/*
                                           
 */
#define AT91SAM9X5_BASE_USART0	0xf801c000
#define AT91SAM9X5_BASE_USART1	0xf8020000
#define AT91SAM9X5_BASE_USART2	0xf8024000

/*
                     
 */
#define AT91SAM9X5_BASE_RTC	0xfffffeb0

/*
                   
 */
#define AT91SAM9X5_SRAM_BASE	0x00300000	/*                            */
#define AT91SAM9X5_SRAM_SIZE	SZ_32K		/*                           */

#define AT91SAM9X5_ROM_BASE	0x00400000	/*                           */
#define AT91SAM9X5_ROM_SIZE	SZ_64K		/*                          */

#endif
