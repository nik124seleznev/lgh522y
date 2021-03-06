/*
 * GPIOs and interrupts for Palm LifeDrive Handheld Computer
 *
 * Authors:	Alex Osborne <ato@meshy.org>
 *		Marek Vasut <marek.vasut@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef _INCLUDE_PALMLD_H_
#define _INCLUDE_PALMLD_H_

/*                  */

/*       */
#define GPIO_NR_PALMLD_GPIO_RESET	1
#define GPIO_NR_PALMLD_POWER_DETECT	4
#define GPIO_NR_PALMLD_HOTSYNC_BUTTON_N	10
#define GPIO_NR_PALMLD_POWER_SWITCH	12
#define GPIO_NR_PALMLD_EARPHONE_DETECT 	13
#define GPIO_NR_PALMLD_LOCK_SWITCH	15

/*        */
#define GPIO_NR_PALMLD_SD_DETECT_N	14
#define GPIO_NR_PALMLD_SD_POWER		114
#define GPIO_NR_PALMLD_SD_READONLY	116

/*             */
#define GPIO_NR_PALMLD_WM9712_IRQ	27

/*      */
#define GPIO_NR_PALMLD_IR_DISABLE	108

/*               */
#define GPIO_NR_PALMLD_BL_POWER		19
#define GPIO_NR_PALMLD_LCD_POWER	96

/*            */
#define GPIO_NR_PALMLD_BORDER_SWITCH	21
#define GPIO_NR_PALMLD_BORDER_SELECT	22

/*           */
#define GPIO_NR_PALMLD_BT_POWER		17
#define GPIO_NR_PALMLD_BT_RESET		83

/*               */
#define GPIO_NR_PALMLD_PCMCIA_READY	38
#define GPIO_NR_PALMLD_PCMCIA_POWER	36
#define GPIO_NR_PALMLD_PCMCIA_RESET	81

/*      */
#define GPIO_NR_PALMLD_LED_GREEN	52
#define GPIO_NR_PALMLD_LED_AMBER	94

/*     */
#define GPIO_NR_PALMLD_IDE_RESET	98
#define GPIO_NR_PALMLD_IDE_PWEN		115

/*     */
#define GPIO_NR_PALMLD_USB_DETECT_N	3
#define GPIO_NR_PALMLD_USB_READY	86
#define GPIO_NR_PALMLD_USB_RESET	88
#define GPIO_NR_PALMLD_USB_INT		106
#define GPIO_NR_PALMLD_USB_POWER	118
/*                                   */

/*            */
#define IRQ_GPIO_PALMLD_GPIO_RESET	PXA_GPIO_TO_IRQ(GPIO_NR_PALMLD_GPIO_RESET)
#define IRQ_GPIO_PALMLD_SD_DETECT_N	PXA_GPIO_TO_IRQ(GPIO_NR_PALMLD_SD_DETECT_N)
#define IRQ_GPIO_PALMLD_WM9712_IRQ	PXA_GPIO_TO_IRQ(GPIO_NR_PALMLD_WM9712_IRQ)
#define IRQ_GPIO_PALMLD_IDE_IRQ		PXA_GPIO_TO_IRQ(GPIO_NR_PALMLD_IDE_IRQ)


/*                        */

/*             */
#define PALMLD_USB_PHYS		PXA_CS2_PHYS
#define PALMLD_USB_VIRT		0xf0000000
#define PALMLD_USB_SIZE		0x00100000

#define PALMLD_IDE_PHYS		0x20000000
#define PALMLD_IDE_VIRT		0xf1000000
#define PALMLD_IDE_SIZE		0x00100000

#define PALMLD_PHYS_IO_START	0x40000000
#define PALMLD_STR_BASE		0xa0200000

/*         */
#define PALMLD_BAT_MAX_VOLTAGE		4000	/*                       */
#define PALMLD_BAT_MIN_VOLTAGE		3550	/*                        */
#define PALMLD_BAT_MAX_CURRENT		0	/*         */
#define PALMLD_BAT_MIN_CURRENT		0	/*         */
#define PALMLD_BAT_MAX_CHARGE		1	/*         */
#define PALMLD_BAT_MIN_CHARGE		1	/*         */
#define PALMLD_MAX_LIFE_MINS		240	/*                    */

#define PALMLD_BAT_MEASURE_DELAY	(HZ * 1)

/*           */
#define PALMLD_MAX_INTENSITY		0xFE
#define PALMLD_DEFAULT_INTENSITY	0x7E
#define PALMLD_LIMIT_MASK		0x7F
#define PALMLD_PRESCALER		0x3F
#define PALMLD_PERIOD_NS		3500

#endif
