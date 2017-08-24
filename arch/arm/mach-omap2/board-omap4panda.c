/*
 * Board support file for OMAP4430 based PandaBoard.
 *
 * Copyright (C) 2010 Texas Instruments
 *
 * Author: David Anders <x0132446@ti.com>
 *
 * Based on mach-omap2/board-4430sdp.c
 *
 * Author: Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * Based on mach-omap2/board-3430sdp.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/usb/otg.h>
#include <linux/i2c/twl.h>
#include <linux/mfd/twl6040.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/fixed.h>
#include <linux/ti_wilink_st.h>
#include <linux/usb/musb.h>
#include <linux/usb/phy.h>
#include <linux/usb/nop-usb-xceiv.h>
#include <linux/wl12xx.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/platform_data/omap-abe-twl6040.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include "common.h"
#include "soc.h"
#include "mmc.h"
#include "hsmmc.h"
#include "control.h"
#include "mux.h"
#include "common-board-devices.h"
#include "dss-common.h"

#define GPIO_HUB_POWER		1
#define GPIO_HUB_NRESET		62
#define GPIO_WIFI_PMENA		43
#define GPIO_WIFI_IRQ		53

/*                                      */
static struct ti_st_plat_data wilink_platform_data = {
	.nshutdown_gpio	= 46,
	.dev_name	= "/dev/ttyO1",
	.flow_cntrl	= 1,
	.baud_rate	= 3000000,
	.chip_enable	= NULL,
	.suspend	= NULL,
	.resume		= NULL,
};

static struct platform_device wl1271_device = {
	.name	= "kim",
	.id	= -1,
	.dev	= {
		.platform_data	= &wilink_platform_data,
	},
};

static struct gpio_led gpio_leds[] = {
	{
		.name			= "pandaboard::status1",
		.default_trigger	= "heartbeat",
		.gpio			= 7,
	},
	{
		.name			= "pandaboard::status2",
		.default_trigger	= "mmc0",
		.gpio			= 8,
	},
};

static struct gpio_led_platform_data gpio_led_info = {
	.leds		= gpio_leds,
	.num_leds	= ARRAY_SIZE(gpio_leds),
};

static struct platform_device leds_gpio = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data	= &gpio_led_info,
	},
};

static struct omap_abe_twl6040_data panda_abe_audio_data = {
	/*           */
	.has_hs		= ABE_TWL6040_LEFT | ABE_TWL6040_RIGHT,
	/*                                       */
	.has_hf		= ABE_TWL6040_LEFT | ABE_TWL6040_RIGHT,
	/*                                                                */
	.has_aux	= ABE_TWL6040_LEFT | ABE_TWL6040_RIGHT,
	/*                                           */
	.has_afm	= ABE_TWL6040_LEFT | ABE_TWL6040_RIGHT,
	/*                    */
	.jack_detection	= 0,
	/*                       */
	.mclk_freq	= 38400000,

};

static struct platform_device panda_abe_audio = {
	.name		= "omap-abe-twl6040",
	.id		= -1,
	.dev = {
		.platform_data = &panda_abe_audio_data,
	},
};

static struct platform_device panda_hdmi_audio_codec = {
	.name	= "hdmi-audio-codec",
	.id	= -1,
};

static struct platform_device btwilink_device = {
	.name	= "btwilink",
	.id	= -1,
};

/*                                                  */
static struct nop_usb_xceiv_platform_data hsusb1_phy_data = {
	/*                                                            */
	.clk_rate = 19200000,
};

static struct usbhs_phy_data phy_data[] __initdata = {
	{
		.port = 1,
		.reset_gpio = GPIO_HUB_NRESET,
		.vcc_gpio = GPIO_HUB_POWER,
		.vcc_polarity = 1,
		.platform_data = &hsusb1_phy_data,
	},
};

static struct platform_device *panda_devices[] __initdata = {
	&leds_gpio,
	&wl1271_device,
	&panda_abe_audio,
	&panda_hdmi_audio_codec,
	&btwilink_device,
};

static struct usbhs_omap_platform_data usbhs_bdata __initdata = {
	.port_mode[0] = OMAP_EHCI_PORT_MODE_PHY,
};

static void __init omap4_ehci_init(void)
{
	int ret;

	/*                                                            */
	ret = clk_add_alias("main_clk", "nop_usb_xceiv.1", "auxclk3_ck", NULL);
	if (ret)
		pr_err("Failed to add main_clk alias to auxclk3_ck\n");

	usbhs_init_phys(phy_data, ARRAY_SIZE(phy_data));
	usbhs_init(&usbhs_bdata);
}

static struct omap_musb_board_data musb_board_data = {
	.interface_type		= MUSB_INTERFACE_UTMI,
	.mode			= MUSB_OTG,
	.power			= 100,
};

static struct omap2_hsmmc_info mmc[] = {
	{
		.mmc		= 1,
		.caps		= MMC_CAP_4_BIT_DATA | MMC_CAP_8_BIT_DATA,
		.gpio_wp	= -EINVAL,
		.gpio_cd	= -EINVAL,
	},
	{
		.name		= "wl1271",
		.mmc		= 5,
		.caps		= MMC_CAP_4_BIT_DATA | MMC_CAP_POWER_OFF_CARD,
		.gpio_wp	= -EINVAL,
		.gpio_cd	= -EINVAL,
		.ocr_mask	= MMC_VDD_165_195,
		.nonremovable	= true,
	},
	{}	/*            */
};

static struct regulator_consumer_supply omap4_panda_vmmc5_supply[] = {
	REGULATOR_SUPPLY("vmmc", "omap_hsmmc.4"),
};

static struct regulator_init_data panda_vmmc5 = {
	.constraints = {
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(omap4_panda_vmmc5_supply),
	.consumer_supplies = omap4_panda_vmmc5_supply,
};

static struct fixed_voltage_config panda_vwlan = {
	.supply_name = "vwl1271",
	.microvolts = 1800000, /*      */
	.gpio = GPIO_WIFI_PMENA,
	.startup_delay = 70000, /*        */
	.enable_high = 1,
	.enabled_at_boot = 0,
	.init_data = &panda_vmmc5,
};

static struct platform_device omap_vwlan_device = {
	.name		= "reg-fixed-voltage",
	.id		= 1,
	.dev = {
		.platform_data = &panda_vwlan,
	},
};

static struct wl12xx_platform_data omap_panda_wlan_data  __initdata = {
	.board_ref_clock = WL12XX_REFCLOCK_38, /*          */
};

static struct twl6040_codec_data twl6040_codec = {
	/*                                            */
	.hs_left_step	= 0x0f,
	.hs_right_step	= 0x0f,
	.hf_left_step	= 0x1d,
	.hf_right_step	= 0x1d,
};

static struct twl6040_platform_data twl6040_data = {
	.codec		= &twl6040_codec,
	.audpwron_gpio	= 127,
};

static struct i2c_board_info __initdata panda_i2c_1_boardinfo[] = {
	{
		I2C_BOARD_INFO("twl6040", 0x4b),
		.irq = 119 + OMAP44XX_IRQ_GIC_START,
		.platform_data = &twl6040_data,
	},
};

/*                                                */
static struct twl4030_platform_data omap4_panda_twldata;

/*
                                                                              
                                                                        
 */
static struct i2c_board_info __initdata panda_i2c_eeprom[] = {
	{
		I2C_BOARD_INFO("eeprom", 0x50),
	},
};

static int __init omap4_panda_i2c_init(void)
{
	omap4_pmic_get_config(&omap4_panda_twldata, TWL_COMMON_PDATA_USB,
			TWL_COMMON_REGULATOR_VDAC |
			TWL_COMMON_REGULATOR_VAUX2 |
			TWL_COMMON_REGULATOR_VAUX3 |
			TWL_COMMON_REGULATOR_VMMC |
			TWL_COMMON_REGULATOR_VPP |
			TWL_COMMON_REGULATOR_VANA |
			TWL_COMMON_REGULATOR_VCXIO |
			TWL_COMMON_REGULATOR_VUSB |
			TWL_COMMON_REGULATOR_CLK32KG |
			TWL_COMMON_REGULATOR_V1V8 |
			TWL_COMMON_REGULATOR_V2V1);
	omap4_pmic_init("twl6030", &omap4_panda_twldata, panda_i2c_1_boardinfo,
			ARRAY_SIZE(panda_i2c_1_boardinfo));
	omap_register_i2c_bus(2, 400, NULL, 0);
	/*
                                                                     
                                             
  */
	omap_register_i2c_bus(3, 100, panda_i2c_eeprom,
					ARRAY_SIZE(panda_i2c_eeprom));
	omap_register_i2c_bus(4, 400, NULL, 0);
	return 0;
}

#ifdef CONFIG_OMAP_MUX
static struct omap_board_mux board_mux[] __initdata = {
	/*                    */
	OMAP4_MUX(GPMC_NCS3, OMAP_MUX_MODE3 | OMAP_PIN_INPUT),
	/*                             */
	OMAP4_MUX(GPMC_A19, OMAP_MUX_MODE3 | OMAP_PIN_OUTPUT),
	/*                     */
	OMAP4_MUX(SDMMC5_CMD, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP),
	/*                     */
	OMAP4_MUX(SDMMC5_CLK, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP),
	/*                          */
	OMAP4_MUX(SDMMC5_DAT0, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP),
	OMAP4_MUX(SDMMC5_DAT1, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP),
	OMAP4_MUX(SDMMC5_DAT2, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP),
	OMAP4_MUX(SDMMC5_DAT3, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP),
	/*                    */
	OMAP4_MUX(KPD_COL1, OMAP_PIN_OUTPUT | OMAP_MUX_MODE3),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_STP, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_DIR, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_NXT, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_DAT0, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_DAT1, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_DAT2, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_DAT3, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_DAT4, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_DAT5, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_DAT6, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(USBB2_ULPITLL_DAT7, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(DPM_EMU3, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU4, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(DPM_EMU5, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*               */
	OMAP4_MUX(DPM_EMU6, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU7, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*             */
	OMAP4_MUX(DPM_EMU8, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU9, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*           */
	OMAP4_MUX(DPM_EMU10, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU11, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU12, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU13, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU14, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU15, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU16, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU17, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU18, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*              */
	OMAP4_MUX(DPM_EMU19, OMAP_PIN_OUTPUT | OMAP_MUX_MODE5),
	/*                   */
	OMAP4_MUX(SYS_NIRQ2, OMAP_MUX_MODE0 |
		  OMAP_PIN_INPUT_PULLUP | OMAP_PIN_OFF_WAKEUPENABLE),
	/*                      */
	OMAP4_MUX(HDQ_SIO, OMAP_MUX_MODE3 | OMAP_PIN_OUTPUT),
	/*       */
	OMAP4_MUX(ABE_PDM_UL_DATA, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLDOWN),
	OMAP4_MUX(ABE_PDM_DL_DATA, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLDOWN),
	OMAP4_MUX(ABE_PDM_FRAME, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLUP),
	OMAP4_MUX(ABE_PDM_LB_CLK, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLDOWN),
	OMAP4_MUX(ABE_CLKS, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLDOWN),
	/*        */
	OMAP4_MUX(ABE_MCBSP1_CLKX, OMAP_MUX_MODE0 | OMAP_PIN_INPUT),
	OMAP4_MUX(ABE_MCBSP1_DR, OMAP_MUX_MODE0 | OMAP_PIN_INPUT_PULLDOWN),
	OMAP4_MUX(ABE_MCBSP1_DX, OMAP_MUX_MODE0 | OMAP_PIN_OUTPUT |
		  OMAP_PULL_ENA),
	OMAP4_MUX(ABE_MCBSP1_FSX, OMAP_MUX_MODE0 | OMAP_PIN_INPUT),

	/*                                    */
	OMAP4_MUX(UART2_CTS,	OMAP_PIN_INPUT	| OMAP_MUX_MODE0),
	OMAP4_MUX(UART2_RTS,	OMAP_PIN_OUTPUT	| OMAP_MUX_MODE0),
	OMAP4_MUX(UART2_RX,	OMAP_PIN_INPUT	| OMAP_MUX_MODE0),
	OMAP4_MUX(UART2_TX,	OMAP_PIN_OUTPUT	| OMAP_MUX_MODE0),

	{ .reg_offset = OMAP_MUX_TERMINATOR },
};

#else
#define board_mux	NULL
#endif


static void omap4_panda_init_rev(void)
{
	if (cpu_is_omap443x()) {
		/*                 */
		/*                          */
		panda_abe_audio_data.card_name = "PandaBoard";
		panda_abe_audio_data.has_hsmic = 1;
	} else {
		/*               */
		/*                          */
		panda_abe_audio_data.card_name = "PandaBoardES";
	}
}

static void __init omap4_panda_init(void)
{
	int package = OMAP_PACKAGE_CBS;
	int ret;

	if (omap_rev() == OMAP4430_REV_ES1_0)
		package = OMAP_PACKAGE_CBL;
	omap4_mux_init(board_mux, NULL, package);

	omap_panda_wlan_data.irq = gpio_to_irq(GPIO_WIFI_IRQ);
	ret = wl12xx_set_platform_data(&omap_panda_wlan_data);
	if (ret)
		pr_err("error setting wl12xx data: %d\n", ret);

	omap4_panda_init_rev();
	omap4_panda_i2c_init();
	platform_add_devices(panda_devices, ARRAY_SIZE(panda_devices));
	platform_device_register(&omap_vwlan_device);
	omap_serial_init();
	omap_sdrc_init(NULL, NULL);
	omap4_twl6030_hsmmc_init(mmc);
	omap4_ehci_init();
	usb_bind_phy("musb-hdrc.2.auto", 0, "omap-usb2.3.auto");
	usb_musb_init(&musb_board_data);
	omap4_panda_display_init();
}

MACHINE_START(OMAP4_PANDA, "OMAP4 Panda board")
	/*                                                  */
	.atag_offset	= 0x100,
	.smp		= smp_ops(omap4_smp_ops),
	.reserve	= omap_reserve,
	.map_io		= omap4_map_io,
	.init_early	= omap4430_init_early,
	.init_irq	= gic_init_irq,
	.init_machine	= omap4_panda_init,
	.init_late	= omap4430_init_late,
	.init_time	= omap4_local_timer_init,
	.restart	= omap44xx_restart,
MACHINE_END
