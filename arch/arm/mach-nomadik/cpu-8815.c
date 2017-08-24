/*
 * Copyright STMicroelectronics, 2007.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/amba/bus.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/dma-mapping.h>
#include <linux/irqchip.h>
#include <linux/platform_data/clk-nomadik.h>
#include <linux/platform_data/pinctrl-nomadik.h>
#include <linux/pinctrl/machine.h>
#include <linux/platform_data/clocksource-nomadik-mtu.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/mtd/fsmc.h>
#include <linux/gpio.h>
#include <linux/amba/mmci.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include <asm/mach-types.h>

#include <asm/cacheflush.h>
#include <asm/hardware/cache-l2x0.h>

/*
                                                                      
 */
#define NOMADIK_FSMC_BASE	0x10100000	/*                */
#define NOMADIK_SDRAMC_BASE	0x10110000	/*                  */
#define NOMADIK_CLCDC_BASE	0x10120000	/*                 */
#define NOMADIK_MDIF_BASE	0x10120000	/*      */
#define NOMADIK_DMA0_BASE	0x10130000	/*                 */
#define NOMADIK_IC_BASE		0x10140000	/*                         */
#define NOMADIK_DMA1_BASE	0x10150000	/*                 */
#define NOMADIK_USB_BASE	0x10170000	/*                       */
#define NOMADIK_CRYP_BASE	0x10180000	/*                  */
#define NOMADIK_SHA1_BASE	0x10190000	/*                 */
#define NOMADIK_XTI_BASE	0x101A0000	/*     */
#define NOMADIK_RNG_BASE	0x101B0000	/*                         */
#define NOMADIK_SRC_BASE	0x101E0000	/*          */
#define NOMADIK_WDOG_BASE	0x101E1000	/*          */
#define NOMADIK_MTU0_BASE	0x101E2000	/*                  */
#define NOMADIK_MTU1_BASE	0x101E3000	/*                  */
#define NOMADIK_GPIO0_BASE	0x101E4000	/*       */
#define NOMADIK_GPIO1_BASE	0x101E5000	/*       */
#define NOMADIK_GPIO2_BASE	0x101E6000	/*       */
#define NOMADIK_GPIO3_BASE	0x101E7000	/*       */
#define NOMADIK_RTC_BASE	0x101E8000	/*                      */
#define NOMADIK_PMU_BASE	0x101E9000	/*                       */
#define NOMADIK_OWM_BASE	0x101EA000	/*                 */
#define NOMADIK_SCR_BASE	0x101EF000	/*                          */
#define NOMADIK_MSP2_BASE	0x101F0000	/*                 */
#define NOMADIK_MSP1_BASE	0x101F1000	/*                 */
#define NOMADIK_UART2_BASE	0x101F2000	/*                  */
#define NOMADIK_SSIRx_BASE	0x101F3000	/*                       */
#define NOMADIK_SSITx_BASE	0x101F4000	/*                       */
#define NOMADIK_MSHC_BASE	0x101F5000	/*                        */
#define NOMADIK_SDI_BASE	0x101F6000	/*                 */
#define NOMADIK_I2C1_BASE	0x101F7000	/*                */
#define NOMADIK_I2C0_BASE	0x101F8000	/*                */
#define NOMADIK_MSP0_BASE	0x101F9000	/*                 */
#define NOMADIK_FIRDA_BASE	0x101FA000	/*                 */
#define NOMADIK_UART1_BASE	0x101FB000	/*                  */
#define NOMADIK_SSP_BASE	0x101FC000	/*               */
#define NOMADIK_UART0_BASE	0x101FD000	/*                  */
#define NOMADIK_SGA_BASE	0x101FE000	/*               */
#define NOMADIK_L2CC_BASE	0x10210000	/*                     */
#define NOMADIK_UART1_VBASE	0xF01FB000

static unsigned long out_low[] = { PIN_OUTPUT_LOW };
static unsigned long out_high[] = { PIN_OUTPUT_HIGH };
static unsigned long in_nopull[] = { PIN_INPUT_NOPULL };
static unsigned long in_pullup[] = { PIN_INPUT_PULLUP };

static struct pinctrl_map __initdata nhk8815_pinmap[] = {
	PIN_MAP_MUX_GROUP_DEFAULT("uart0", "pinctrl-stn8815", "u0_a_1", "u0"),
	PIN_MAP_MUX_GROUP_DEFAULT("uart1", "pinctrl-stn8815", "u1_a_1", "u1"),
	/*                        */
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("pinctrl-stn8815", "mmcsd_a_1", "mmcsd"),
	/*       */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO8_B10", out_low),
	/*       */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO9_A10", in_pullup),
	/*          */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO10_C11", out_high),
	/*          */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO11_B11", in_pullup),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO12_A11", in_pullup),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO13_C12", in_pullup),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO14_B12", in_pullup),
	/*           */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO15_A12", out_high),
	/*            */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO16_C13", out_high),
	/*           */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO24_C15", in_pullup),
	/*               */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO111_H21", in_nopull),
	/*               */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO112_J21", out_low),
	/*      */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO62_D3", in_pullup),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO63_D2", in_pullup),
	/*      */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO53_L4", in_pullup),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO54_L3", in_pullup),
	/*      */
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO73_C21", in_pullup),
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-stn8815", "GPIO74_C20", in_pullup),
};

/*                                                       */
static struct map_desc cpu8815_io_desc[] __initdata = {
	{
		.virtual =	NOMADIK_UART1_VBASE,
		.pfn =		__phys_to_pfn(NOMADIK_UART1_BASE),
		.length =	SZ_4K,
		.type =		MT_DEVICE,
	},
};

static void __init cpu8815_map_io(void)
{
	iotable_init(cpu8815_io_desc, ARRAY_SIZE(cpu8815_io_desc));
}

static void cpu8815_restart(char mode, const char *cmd)
{
	void __iomem *srcbase = ioremap(NOMADIK_SRC_BASE, SZ_4K);

	/*                                   */

	/*                                         */
	writel(1, srcbase + 0x18);
}

/*                                                                       */
#define SRC_CR_INIT_MASK	0x00007fff
#define SRC_CR_INIT_VAL		0x2aaa8000

static void __init cpu8815_timer_init_of(void)
{
	struct device_node *mtu;
	void __iomem *base;
	int irq;
	u32 src_cr;

	/*                           */
	nomadik_clk_init();

	mtu = of_find_node_by_path("/mtu0");
	if (!mtu)
		return;
	base = of_iomap(mtu, 0);
	if (WARN_ON(!base))
		return;
	irq = irq_of_parse_and_map(mtu, 0);

	pr_info("Remapped MTU @ %p, irq: %d\n", base, irq);

	/*                                                               */
	src_cr = readl(base);
	src_cr &= SRC_CR_INIT_MASK;
	src_cr |= SRC_CR_INIT_VAL;
	writel(src_cr, base);

	nmdk_timer_init(base, irq);
}

static struct fsmc_nand_timings cpu8815_nand_timings = {
	.thiz	= 0,
	.thold	= 0x10,
	.twait	= 0x0A,
	.tset	= 0,
};

static struct fsmc_nand_platform_data cpu8815_nand_data = {
	.nand_timings = &cpu8815_nand_timings,
};

/*
                                                                      
                                                                       
                                                
 */
static int __init cpu8815_eth_init(void)
{
	struct device_node *eth;
	int gpio, irq, err;

	eth = of_find_node_by_path("/usb-s8815/ethernet-gpio");
	if (!eth) {
		pr_info("could not find any ethernet GPIO\n");
		return 0;
	}
	gpio = of_get_gpio(eth, 0);
	err = gpio_request(gpio, "eth_irq");
	if (err) {
		pr_info("failed to request ethernet GPIO\n");
		return -ENODEV;
	}
	err = gpio_direction_input(gpio);
	if (err) {
		pr_info("failed to set ethernet GPIO as input\n");
		return -ENODEV;
	}
	irq = gpio_to_irq(gpio);
	pr_info("enabled USB-S8815 ethernet GPIO %d, IRQ %d\n", gpio, irq);
	return 0;
}
device_initcall(cpu8815_eth_init);

/*
        
                                                         
           
 */
static struct mmci_platform_data mmcsd_plat_data = {
	.ocr_mask = MMC_VDD_29_30,
};

/*
                                                                      
                 
 */
static int __init cpu8815_mmcsd_init(void)
{
	struct device_node *cdbias;
	int gpio, err;

	cdbias = of_find_node_by_path("/usb-s8815/mmcsd-gpio");
	if (!cdbias) {
		pr_info("could not find MMC/SD card detect bias node\n");
		return 0;
	}
	gpio = of_get_gpio(cdbias, 0);
	if (gpio < 0) {
		pr_info("could not obtain MMC/SD card detect bias GPIO\n");
		return 0;
	}
	err = gpio_request(gpio, "card detect bias");
	if (err) {
		pr_info("failed to request card detect bias GPIO %d\n", gpio);
		return -ENODEV;
	}
	err = gpio_direction_output(gpio, 0);
	if (err){
		pr_info("failed to set GPIO %d as output, low\n", gpio);
		return err;
	}
	pr_info("enabled USB-S8815 CD bias GPIO %d, low\n", gpio);
	return 0;
}
device_initcall(cpu8815_mmcsd_init);


/*                                                                      */
static struct of_dev_auxdata cpu8815_auxdata_lookup[] __initdata = {
	OF_DEV_AUXDATA("st,nomadik-gpio", NOMADIK_GPIO0_BASE,
		"gpio.0", NULL),
	OF_DEV_AUXDATA("st,nomadik-gpio", NOMADIK_GPIO1_BASE,
		"gpio.1", NULL),
	OF_DEV_AUXDATA("st,nomadik-gpio", NOMADIK_GPIO2_BASE,
		"gpio.2", NULL),
	OF_DEV_AUXDATA("st,nomadik-gpio", NOMADIK_GPIO3_BASE,
		"gpio.3", NULL),
	OF_DEV_AUXDATA("stericsson,nmk-pinctrl-stn8815", 0,
		"pinctrl-stn8815", NULL),
	OF_DEV_AUXDATA("arm,primecell", NOMADIK_UART0_BASE,
		"uart0", NULL),
	OF_DEV_AUXDATA("arm,primecell", NOMADIK_UART1_BASE,
		"uart1", NULL),
	OF_DEV_AUXDATA("arm,primecell", NOMADIK_RNG_BASE,
		"rng", NULL),
	OF_DEV_AUXDATA("arm,primecell", NOMADIK_RTC_BASE,
		"rtc-pl031", NULL),
	OF_DEV_AUXDATA("stericsson,fsmc-nand", NOMADIK_FSMC_BASE,
		"fsmc-nand", &cpu8815_nand_data),
	OF_DEV_AUXDATA("arm,primecell", NOMADIK_SDI_BASE,
		"mmci", &mmcsd_plat_data),
	{ /*          */ },
};

static void __init cpu8815_init_of(void)
{
#ifdef CONFIG_CACHE_L2X0
	/*                                                         */
	l2x0_of_init(0x00730249, 0xfe000fff);
#endif
	pinctrl_register_mappings(nhk8815_pinmap, ARRAY_SIZE(nhk8815_pinmap));
	of_platform_populate(NULL, of_default_bus_match_table,
			cpu8815_auxdata_lookup, NULL);
}

static const char * cpu8815_board_compat[] = {
	"calaosystems,usb-s8815",
	NULL,
};

DT_MACHINE_START(NOMADIK_DT, "Nomadik STn8815")
	.map_io		= cpu8815_map_io,
	.init_irq	= irqchip_init,
	.init_time	= cpu8815_timer_init_of,
	.init_machine	= cpu8815_init_of,
	.restart	= cpu8815_restart,
	.dt_compat      = cpu8815_board_compat,
MACHINE_END
