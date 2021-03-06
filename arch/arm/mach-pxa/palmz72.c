/*
 * Hardware definitions for Palm Zire72
 *
 * Authors:
 *	Vladimir "Farcaller" Pouzanov <farcaller@gmail.com>
 *	Sergey Lapin <slapin@ossfans.org>
 *	Alex Osborne <bobofdoom@gmail.com>
 *	Jan Herman <2hp@seznam.cz>
 *
 * Rewrite for mainline:
 *	Marek Vasut <marek.vasut@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * (find more info at www.hackndev.com)
 *
 */

#include <linux/platform_device.h>
#include <linux/syscore_ops.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/pda_power.h>
#include <linux/pwm_backlight.h>
#include <linux/gpio.h>
#include <linux/wm97xx.h>
#include <linux/power_supply.h>
#include <linux/usb/gpio_vbus.h>
#include <linux/i2c-gpio.h>

#include <asm/mach-types.h>
#include <asm/suspend.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <mach/pxa27x.h>
#include <mach/audio.h>
#include <mach/palmz72.h>
#include <linux/platform_data/mmc-pxamci.h>
#include <linux/platform_data/video-pxafb.h>
#include <linux/platform_data/irda-pxaficp.h>
#include <linux/platform_data/keypad-pxa27x.h>
#include <mach/udc.h>
#include <linux/platform_data/asoc-palm27x.h>
#include <mach/palm27x.h>

#include <mach/pm.h>
#include <linux/platform_data/camera-pxa.h>

#include <media/soc_camera.h>

#include "generic.h"
#include "devices.h"

/*                                                                             
                    
                                                                              */
static unsigned long palmz72_pin_config[] __initdata = {
	/*     */
	GPIO32_MMC_CLK,
	GPIO92_MMC_DAT_0,
	GPIO109_MMC_DAT_1,
	GPIO110_MMC_DAT_2,
	GPIO111_MMC_DAT_3,
	GPIO112_MMC_CMD,
	GPIO14_GPIO,	/*           */
	GPIO115_GPIO,	/*       */
	GPIO98_GPIO,	/*          */

	/*      */
	GPIO28_AC97_BITCLK,
	GPIO29_AC97_SDATA_IN_0,
	GPIO30_AC97_SDATA_OUT,
	GPIO31_AC97_SYNC,
	GPIO89_AC97_SYSCLK,
	GPIO113_AC97_nRESET,

	/*      */
	GPIO49_GPIO,	/*            */
	GPIO46_FICP_RXD,
	GPIO47_FICP_TXD,

	/*     */
	GPIO16_PWM0_OUT,

	/*     */
	GPIO15_GPIO,	/*            */
	GPIO95_GPIO,	/*            */

	/*               */
	GPIO100_KP_MKIN_0	| WAKEUP_ON_LEVEL_HIGH,
	GPIO101_KP_MKIN_1	| WAKEUP_ON_LEVEL_HIGH,
	GPIO102_KP_MKIN_2	| WAKEUP_ON_LEVEL_HIGH,
	GPIO97_KP_MKIN_3	| WAKEUP_ON_LEVEL_HIGH,
	GPIO103_KP_MKOUT_0,
	GPIO104_KP_MKOUT_1,
	GPIO105_KP_MKOUT_2,

	/*     */
	GPIOxx_LCD_TFT_16BPP,

	GPIO20_GPIO,	/*          */
	GPIO21_GPIO,	/*                   */
	GPIO22_GPIO,	/*                  */
	GPIO96_GPIO,	/*           */

	/*            */
	GPIO81_CIF_DD_0,
	GPIO48_CIF_DD_5,
	GPIO50_CIF_DD_3,
	GPIO51_CIF_DD_2,
	GPIO52_CIF_DD_4,
	GPIO53_CIF_MCLK,
	GPIO54_CIF_PCLK,
	GPIO55_CIF_DD_1,
	GPIO84_CIF_FV,
	GPIO85_CIF_LV,
	GPIO93_CIF_DD_6,
	GPIO108_CIF_DD_7,

	GPIO56_GPIO,	/*                  */
	GPIO57_GPIO,	/*              */
	GPIO91_GPIO,	/*              */

	/*     */
	GPIO117_GPIO,	/*         */
	GPIO118_GPIO,	/*         */

	/*       */
	GPIO0_GPIO	| WAKEUP_ON_LEVEL_HIGH,	/*              */
	GPIO88_GPIO,				/*           */
	GPIO27_GPIO,				/*            */
};

/*                                                                             
                
                                                                              */
#if defined(CONFIG_KEYBOARD_PXA27x) || defined(CONFIG_KEYBOARD_PXA27x_MODULE)
static unsigned int palmz72_matrix_keys[] = {
	KEY(0, 0, KEY_POWER),
	KEY(0, 1, KEY_F1),
	KEY(0, 2, KEY_ENTER),

	KEY(1, 0, KEY_F2),
	KEY(1, 1, KEY_F3),
	KEY(1, 2, KEY_F4),

	KEY(2, 0, KEY_UP),
	KEY(2, 2, KEY_DOWN),

	KEY(3, 0, KEY_RIGHT),
	KEY(3, 2, KEY_LEFT),
};

static struct pxa27x_keypad_platform_data palmz72_keypad_platform_data = {
	.matrix_key_rows	= 4,
	.matrix_key_cols	= 3,
	.matrix_key_map		= palmz72_matrix_keys,
	.matrix_key_map_size	= ARRAY_SIZE(palmz72_matrix_keys),

	.debounce_interval	= 30,
};

static void __init palmz72_kpc_init(void)
{
	pxa_set_keypad_info(&palmz72_keypad_platform_data);
}
#else
static inline void palmz72_kpc_init(void) {}
#endif

/*                                                                             
       
                                                                              */
#if defined(CONFIG_LEDS_GPIO) || defined(CONFIG_LEDS_GPIO_MODULE)
static struct gpio_led gpio_leds[] = {
	{
		.name			= "palmz72:green:led",
		.default_trigger	= "none",
		.gpio			= GPIO_NR_PALMZ72_LED_GREEN,
	},
};

static struct gpio_led_platform_data gpio_led_info = {
	.leds		= gpio_leds,
	.num_leds	= ARRAY_SIZE(gpio_leds),
};

static struct platform_device palmz72_leds = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data	= &gpio_led_info,
	}
};

static void __init palmz72_leds_init(void)
{
	platform_device_register(&palmz72_leds);
}
#else
static inline void palmz72_leds_init(void) {}
#endif

#ifdef CONFIG_PM

/*                              
                                                                
                                                                
                                                               
                                                                      
                                                                     
                                                                       
                                   
 */

#define PALMZ72_SAVE_DWORD ((unsigned long *)0xc0000050)

static struct palmz72_resume_info palmz72_resume_info = {
	.magic0 = 0xb4e6,
	.magic1 = 1,

	/*                          */
	.arm_control = 0,
	.aux_control = 0,
	.ttb = 0,
	.domain_access = 0,
	.process_id = 0,
};

static unsigned long store_ptr;

/*                                 */

static int palmz72_pm_suspend(void)
{
	/*                                                          */
	palmz72_resume_info.resume_addr = (u32) cpu_resume;

	/*                               */
	store_ptr = *PALMZ72_SAVE_DWORD;

	/*                                */
	PSPR = virt_to_phys(&palmz72_resume_info);

	return 0;
}

static void palmz72_pm_resume(void)
{
	*PALMZ72_SAVE_DWORD = store_ptr;
}

static struct syscore_ops palmz72_pm_syscore_ops = {
	.suspend = palmz72_pm_suspend,
	.resume = palmz72_pm_resume,
};

static int __init palmz72_pm_init(void)
{
	if (machine_is_palmz72()) {
		register_syscore_ops(&palmz72_pm_syscore_ops);
		return 0;
	}
	return -ENODEV;
}

device_initcall(palmz72_pm_init);
#endif

/*                                                                             
             
                                                                              */
#if defined(CONFIG_SOC_CAMERA_OV9640) || \
	defined(CONFIG_SOC_CAMERA_OV9640_MODULE)
static struct pxacamera_platform_data palmz72_pxacamera_platform_data = {
	.flags		= PXA_CAMERA_MASTER | PXA_CAMERA_DATAWIDTH_8 |
			PXA_CAMERA_PCLK_EN | PXA_CAMERA_MCLK_EN,
	.mclk_10khz	= 2600,
};

/*                    */
static struct i2c_board_info palmz72_i2c_device[] = {
	{
		I2C_BOARD_INFO("ov9640", 0x30),
	}
};

static int palmz72_camera_power(struct device *dev, int power)
{
	gpio_set_value(GPIO_NR_PALMZ72_CAM_PWDN, !power);
	mdelay(50);
	return 0;
}

static int palmz72_camera_reset(struct device *dev)
{
	gpio_set_value(GPIO_NR_PALMZ72_CAM_RESET, 1);
	mdelay(50);
	gpio_set_value(GPIO_NR_PALMZ72_CAM_RESET, 0);
	mdelay(50);
	return 0;
}

static struct soc_camera_link palmz72_iclink = {
	.bus_id		= 0, /*                                              */
	.board_info	= &palmz72_i2c_device[0],
	.i2c_adapter_id	= 0,
	.module_name	= "ov96xx",
	.power		= &palmz72_camera_power,
	.reset		= &palmz72_camera_reset,
	.flags		= SOCAM_DATAWIDTH_8,
};

static struct i2c_gpio_platform_data palmz72_i2c_bus_data = {
	.sda_pin	= 118,
	.scl_pin	= 117,
	.udelay		= 10,
	.timeout	= 100,
};

static struct platform_device palmz72_i2c_bus_device = {
	.name		= "i2c-gpio",
	.id		= 0, /*                                          */
	.dev		= {
		.platform_data	= &palmz72_i2c_bus_data,
	}
};

static struct platform_device palmz72_camera = {
	.name	= "soc-camera-pdrv",
	.id	= -1,
	.dev	= {
		.platform_data	= &palmz72_iclink,
	},
};

/*                                                                            
                                                                             
                                                                        */
static struct gpio palmz72_camera_gpios[] = {
	{ GPIO_NR_PALMZ72_CAM_POWER,	GPIOF_INIT_HIGH,"Camera DVDD" },
	{ GPIO_NR_PALMZ72_CAM_RESET,	GPIOF_INIT_LOW,	"Camera RESET" },
	{ GPIO_NR_PALMZ72_CAM_PWDN,	GPIOF_INIT_LOW,	"Camera PWDN" },
};

static inline void __init palmz72_cam_gpio_init(void)
{
	int ret;

	ret = gpio_request_array(ARRAY_AND_SIZE(palmz72_camera_gpios));
	if (!ret)
		gpio_free_array(ARRAY_AND_SIZE(palmz72_camera_gpios));
	else
		printk(KERN_ERR "Camera GPIO init failed!\n");

	return;
}

static void __init palmz72_camera_init(void)
{
	palmz72_cam_gpio_init();
	pxa_set_camera_info(&palmz72_pxacamera_platform_data);
	platform_device_register(&palmz72_i2c_bus_device);
	platform_device_register(&palmz72_camera);
}
#else
static inline void palmz72_camera_init(void) {}
#endif

/*                                                                             
               
                                                                              */
static void __init palmz72_init(void)
{
	pxa2xx_mfp_config(ARRAY_AND_SIZE(palmz72_pin_config));
	pxa_set_ffuart_info(NULL);
	pxa_set_btuart_info(NULL);
	pxa_set_stuart_info(NULL);

	palm27x_mmc_init(GPIO_NR_PALMZ72_SD_DETECT_N, GPIO_NR_PALMZ72_SD_RO,
			GPIO_NR_PALMZ72_SD_POWER_N, 1);
	palm27x_lcd_init(-1, &palm_320x320_lcd_mode);
	palm27x_udc_init(GPIO_NR_PALMZ72_USB_DETECT_N,
			GPIO_NR_PALMZ72_USB_PULLUP, 0);
	palm27x_irda_init(GPIO_NR_PALMZ72_IR_DISABLE);
	palm27x_ac97_init(PALMZ72_BAT_MIN_VOLTAGE, PALMZ72_BAT_MAX_VOLTAGE,
			-1, 113);
	palm27x_pwm_init(-1, -1);
	palm27x_power_init(-1, -1);
	palm27x_pmic_init();
	palmz72_kpc_init();
	palmz72_leds_init();
	palmz72_camera_init();
}

MACHINE_START(PALMZ72, "Palm Zire72")
	.atag_offset	= 0x100,
	.map_io		= pxa27x_map_io,
	.nr_irqs	= PXA_NR_IRQS,
	.init_irq	= pxa27x_init_irq,
	.handle_irq	= pxa27x_handle_irq,
	.init_time	= pxa_timer_init,
	.init_machine	= palmz72_init,
	.restart	= pxa_restart,
MACHINE_END
