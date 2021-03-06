/*
 * linux/arch/arm/mach-at91/board-flexibity.c
 *
 *  Copyright (C) 2010-2011 Flexibity
 *  Copyright (C) 2005 SAN People
 *  Copyright (C) 2006 Atmel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/input.h>
#include <linux/gpio.h>

#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/hardware.h>

#include "at91_aic.h"
#include "board.h"
#include "generic.h"

static void __init flexibity_init_early(void)
{
	/*                                          */
	at91_initialize(18432000);
}

/*               */
static struct at91_usbh_data __initdata flexibity_usbh_data = {
	.ports		= 2,
	.vbus_pin	= {-EINVAL, -EINVAL},
	.overcurrent_pin= {-EINVAL, -EINVAL},
};

/*                 */
static struct at91_udc_data __initdata flexibity_udc_data = {
	.vbus_pin	= AT91_PIN_PC5,
	.pullup_pin	= -EINVAL,		/*                       */
};

/*             */
static struct i2c_board_info __initdata flexibity_i2c_devices[] = {
	{
		I2C_BOARD_INFO("ds1307", 0x68),
	},
};

/*             */
static struct spi_board_info flexibity_spi_devices[] = {
	{	/*                */
		.modalias	= "mtd_dataflash",
		.chip_select	= 1,
		.max_speed_hz	= 15 * 1000 * 1000,
		.bus_num	= 0,
	},
};

/*              */
static struct mci_platform_data __initdata flexibity_mci0_data = {
	.slot[0] = {
		.bus_width	= 4,
		.detect_pin	= AT91_PIN_PC9,
		.wp_pin		= AT91_PIN_PC4,
	},
};

/*      */
static struct gpio_led flexibity_leds[] = {
	{
		.name			= "usb1:green",
		.gpio			= AT91_PIN_PA12,
		.active_low		= 1,
		.default_trigger	= "default-on",
	},
	{
		.name			= "usb1:red",
		.gpio			= AT91_PIN_PA13,
		.active_low		= 1,
		.default_trigger	= "default-on",
	},
	{
		.name			= "usb2:green",
		.gpio			= AT91_PIN_PB26,
		.active_low		= 1,
		.default_trigger	= "default-on",
	},
	{
		.name			= "usb2:red",
		.gpio			= AT91_PIN_PB27,
		.active_low		= 1,
		.default_trigger	= "default-on",
	},
	{
		.name			= "usb3:green",
		.gpio			= AT91_PIN_PC8,
		.active_low		= 1,
		.default_trigger	= "default-on",
	},
	{
		.name			= "usb3:red",
		.gpio			= AT91_PIN_PC6,
		.active_low		= 1,
		.default_trigger	= "default-on",
	},
	{
		.name			= "usb4:green",
		.gpio			= AT91_PIN_PB4,
		.active_low		= 1,
		.default_trigger	= "default-on",
	},
	{
		.name			= "usb4:red",
		.gpio			= AT91_PIN_PB5,
		.active_low		= 1,
		.default_trigger	= "default-on",
	}
};

static void __init flexibity_board_init(void)
{
	/*        */
	/*                               */
	at91_register_uart(0, 0, 0);
	at91_add_device_serial();
	/*          */
	at91_add_device_usbh(&flexibity_usbh_data);
	/*            */
	at91_add_device_udc(&flexibity_udc_data);
	/*     */
	at91_add_device_i2c(flexibity_i2c_devices,
		ARRAY_SIZE(flexibity_i2c_devices));
	/*     */
	at91_add_device_spi(flexibity_spi_devices,
		ARRAY_SIZE(flexibity_spi_devices));
	/*     */
	at91_add_device_mci(0, &flexibity_mci0_data);
	/*      */
	at91_gpio_leds(flexibity_leds, ARRAY_SIZE(flexibity_leds));
}

MACHINE_START(FLEXIBITY, "Flexibity Connect")
	/*                          */
	.init_time	= at91sam926x_pit_init,
	.map_io		= at91_map_io,
	.handle_irq	= at91_aic_handle_irq,
	.init_early	= flexibity_init_early,
	.init_irq	= at91_init_irq_default,
	.init_machine	= flexibity_board_init,
MACHINE_END
