/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * License terms: GNU General Public License (GPL) version 2
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/bug.h>
#include <linux/string.h>
#include <linux/pinctrl/machine.h>
#include <linux/platform_data/pinctrl-nomadik.h>

#include <asm/mach-types.h>

#include "pins-db8500.h"
#include "board-mop500.h"

enum custom_pin_cfg_t {
	PINS_FOR_DEFAULT,
	PINS_FOR_U9500,
};

static enum custom_pin_cfg_t pinsfor;

/*                                 */
#define BIAS(a,b) static unsigned long a[] = { b }

BIAS(pd, PIN_PULL_DOWN);
BIAS(in_nopull, PIN_INPUT_NOPULL);
BIAS(in_nopull_slpm_nowkup, PIN_INPUT_NOPULL|PIN_SLPM_WAKEUP_DISABLE);
BIAS(in_pu, PIN_INPUT_PULLUP);
BIAS(in_pd, PIN_INPUT_PULLDOWN);
BIAS(out_hi, PIN_OUTPUT_HIGH);
BIAS(out_lo, PIN_OUTPUT_LOW);
BIAS(out_lo_slpm_nowkup, PIN_OUTPUT_LOW|PIN_SLPM_WAKEUP_DISABLE);
/*                                      */
BIAS(gpio_in_pu, PIN_INPUT_PULLUP|PIN_GPIOMODE_ENABLED);
BIAS(gpio_in_pd, PIN_INPUT_PULLDOWN|PIN_GPIOMODE_ENABLED);
BIAS(gpio_in_pu_slpm_gpio_nopull, PIN_INPUT_PULLUP|PIN_GPIOMODE_ENABLED|PIN_SLPM_GPIO|PIN_SLPM_INPUT_NOPULL);
BIAS(gpio_in_pd_slpm_gpio_nopull, PIN_INPUT_PULLDOWN|PIN_GPIOMODE_ENABLED|PIN_SLPM_GPIO|PIN_SLPM_INPUT_NOPULL);
BIAS(gpio_out_hi, PIN_OUTPUT_HIGH|PIN_GPIOMODE_ENABLED);
BIAS(gpio_out_lo, PIN_OUTPUT_LOW|PIN_GPIOMODE_ENABLED);
/*             */
BIAS(slpm_in_nopull_wkup, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_DIR_INPUT|PIN_SLPM_PULL_NONE|PIN_SLPM_WAKEUP_ENABLE);
BIAS(slpm_in_wkup_pdis, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_DIR_INPUT|PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_DISABLED);
BIAS(slpm_in_wkup_pdis_en, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_DIR_INPUT|PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_ENABLED);
BIAS(slpm_wkup_pdis, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_DISABLED);
BIAS(slpm_wkup_pdis_en, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_ENABLED);
BIAS(slpm_out_lo_pdis, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_OUTPUT_LOW|PIN_SLPM_WAKEUP_DISABLE|PIN_SLPM_PDIS_DISABLED);
BIAS(slpm_out_lo_wkup, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_OUTPUT_LOW|PIN_SLPM_WAKEUP_ENABLE);
BIAS(slpm_out_lo_wkup_pdis, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_OUTPUT_LOW|PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_DISABLED);
BIAS(slpm_out_hi_wkup_pdis, PIN_SLEEPMODE_ENABLED|PIN_SLPM_OUTPUT_HIGH|
	PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_DISABLED);
BIAS(slpm_in_nopull_wkup_pdis, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_INPUT_NOPULL|PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_DISABLED);
BIAS(slpm_in_pu_wkup_pdis_en, PIN_SLEEPMODE_ENABLED|PIN_SLPM_INPUT_PULLUP|
	PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_ENABLED);
BIAS(slpm_out_wkup_pdis, PIN_SLEEPMODE_ENABLED|
	PIN_SLPM_DIR_OUTPUT|PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_DISABLED);
BIAS(out_lo_wkup_pdis, PIN_SLPM_OUTPUT_LOW|
	PIN_SLPM_WAKEUP_ENABLE|PIN_SLPM_PDIS_DISABLED);
BIAS(in_wkup_pdis_en, PIN_SLPM_DIR_INPUT|PIN_SLPM_WAKEUP_ENABLE|
	PIN_SLPM_PDIS_ENABLED);
BIAS(in_wkup_pdis, PIN_SLPM_DIR_INPUT|PIN_SLPM_WAKEUP_ENABLE|
	PIN_SLPM_PDIS_DISABLED);
BIAS(out_wkup_pdis, PIN_SLPM_DIR_OUTPUT|PIN_SLPM_WAKEUP_ENABLE|
	PIN_SLPM_PDIS_DISABLED);

/*                                                                  */
#define DB8500_MUX_HOG(group,func) \
	PIN_MAP_MUX_GROUP_HOG_DEFAULT("pinctrl-db8500", group, func)
#define DB8500_PIN_HOG(pin,conf) \
	PIN_MAP_CONFIGS_PIN_HOG_DEFAULT("pinctrl-db8500", pin, conf)

/*                                                                     */
#define DB8500_MUX(group,func,dev) \
	PIN_MAP_MUX_GROUP_DEFAULT(dev, "pinctrl-db8500", group, func)
#define DB8500_PIN(pin,conf,dev) \
	PIN_MAP_CONFIGS_PIN_DEFAULT(dev, "pinctrl-db8500", pin, conf)
#define DB8500_PIN_IDLE(pin, conf, dev) \
	PIN_MAP_CONFIGS_PIN(dev, PINCTRL_STATE_IDLE, "pinctrl-db8500",	\
			    pin, conf)
#define DB8500_PIN_SLEEP(pin, conf, dev) \
	PIN_MAP_CONFIGS_PIN(dev, PINCTRL_STATE_SLEEP, "pinctrl-db8500",	\
			    pin, conf)
#define DB8500_MUX_STATE(group, func, dev, state) \
	PIN_MAP_MUX_GROUP(dev, state, "pinctrl-db8500", group, func)
#define DB8500_PIN_STATE(pin, conf, dev, state) \
	PIN_MAP_CONFIGS_PIN(dev, state, "pinctrl-db8500", pin, conf)

/*                      */
static struct pinctrl_map __initdata mop500_family_pinmap[] = {
	/*
                                                    
                                      
  */
	DB8500_MUX_HOG("msp0txrx_a_1", "msp0"),
	DB8500_MUX_HOG("msp0tfstck_a_1", "msp0"),
	DB8500_PIN_HOG("GPIO12_AC4", in_nopull), /*     */
	DB8500_PIN_HOG("GPIO15_AC3", in_nopull), /*     */
	DB8500_PIN_HOG("GPIO13_AF3", in_nopull), /*     */
	DB8500_PIN_HOG("GPIO14_AE3", in_nopull), /*     */
	/*                                         */
	DB8500_MUX_HOG("msp2_a_1", "msp2"),
	DB8500_PIN_HOG("GPIO193_AH27", in_pd), /*     */
	DB8500_PIN_HOG("GPIO194_AF27", in_pd), /*     */
	DB8500_PIN_HOG("GPIO195_AG28", in_pd), /*     */
	DB8500_PIN_HOG("GPIO196_AG26", out_lo), /*     */
	/*
                                                                     
           
                                                              
  */
	DB8500_MUX_HOG("lcdvsi0_a_1", "lcd"),
	DB8500_PIN_HOG("GPIO68_E1", in_pu),
	DB8500_PIN_HOG("GPIO84_C2", gpio_in_pu),
	/*
                                         
                                            
  */
	DB8500_PIN_HOG("GPIO218_AH11", gpio_in_pu),
	/*
                                    
                                                                   
                                                                    
                                 
  */
	DB8500_PIN_HOG("GPIO0_AJ5", in_pu), /*     */
	DB8500_PIN_HOG("GPIO1_AJ3", out_hi), /*     */
	DB8500_PIN_HOG("GPIO2_AH4", in_pu), /*     */
	DB8500_PIN_HOG("GPIO3_AH3", out_hi), /*     */
	/*
                                                   
                                                             
                                                                    
  */
	/*                         */
	DB8500_MUX("u2rxtx_c_1", "u2", "uart2"),
	DB8500_PIN("GPIO29_W2", in_pu, "uart2"), /*     */
	DB8500_PIN("GPIO30_W3", out_hi, "uart2"), /*     */
	/*                       */
	DB8500_PIN_SLEEP("GPIO29_W2", in_wkup_pdis, "uart2"),
	DB8500_PIN_SLEEP("GPIO30_W3", out_wkup_pdis, "uart2"),
	/*
                                                                    
                                                                   
           
  */
	/*                                   */
	DB8500_MUX("u0_a_1", "u0", "uart0"),
	DB8500_PIN("GPIO0_AJ5", in_pu, "uart0"), /*     */
	DB8500_PIN("GPIO1_AJ3", out_hi, "uart0"), /*     */
	DB8500_PIN("GPIO2_AH4", in_pu, "uart0"), /*     */
	DB8500_PIN("GPIO3_AH3", out_hi, "uart0"), /*     */
	/*                       */
	DB8500_PIN_SLEEP("GPIO0_AJ5", slpm_in_wkup_pdis, "uart0"),
	DB8500_PIN_SLEEP("GPIO1_AJ3", slpm_out_hi_wkup_pdis, "uart0"),
	DB8500_PIN_SLEEP("GPIO2_AH4", slpm_in_wkup_pdis, "uart0"),
	DB8500_PIN_SLEEP("GPIO3_AH3", slpm_out_wkup_pdis, "uart0"),
	/*                                   */
	DB8500_MUX("u1rxtx_a_1", "u1", "uart1"),
	DB8500_PIN("GPIO4_AH6", in_pu, "uart1"), /*     */
	DB8500_PIN("GPIO5_AG6", out_hi, "uart1"), /*     */
	/*                       */
	DB8500_PIN_SLEEP("GPIO4_AH6", slpm_in_wkup_pdis, "uart1"),
	DB8500_PIN_SLEEP("GPIO5_AG6", slpm_out_wkup_pdis, "uart1"),
	/*                     */
	DB8500_MUX("msp1txrx_a_1", "msp1", "ux500-msp-i2s.1"),
	DB8500_MUX("msp1_a_1", "msp1", "ux500-msp-i2s.1"),
	DB8500_PIN("GPIO33_AF2", out_lo_slpm_nowkup, "ux500-msp-i2s.1"),
	DB8500_PIN("GPIO34_AE1", in_nopull_slpm_nowkup, "ux500-msp-i2s.1"),
	DB8500_PIN("GPIO35_AE2", in_nopull_slpm_nowkup, "ux500-msp-i2s.1"),
	DB8500_PIN("GPIO36_AG2", in_nopull_slpm_nowkup, "ux500-msp-i2s.1"),
	/*                  */
	DB8500_PIN_SLEEP("GPIO33_AF2", slpm_out_lo_wkup, "ux500-msp-i2s.1"),
	DB8500_PIN_SLEEP("GPIO34_AE1", slpm_in_nopull_wkup, "ux500-msp-i2s.1"),
	DB8500_PIN_SLEEP("GPIO35_AE2", slpm_in_nopull_wkup, "ux500-msp-i2s.1"),
	DB8500_PIN_SLEEP("GPIO36_AG2", slpm_in_nopull_wkup, "ux500-msp-i2s.1"),
	/*                                                             */
	DB8500_MUX("lcd_d8_d11_a_1", "lcd", "mcde-tvout"),
	DB8500_MUX("lcdaclk_b_1", "lcda", "mcde-tvout"),
	/*                                                     */
	DB8500_MUX("lcdvsi1_a_1", "lcd", "0-0070"),
	DB8500_PIN("GPIO69_E2", in_pu, "0-0070"),
	/*                      */
	DB8500_PIN_SLEEP("GPIO69_E2", slpm_in_wkup_pdis, "0-0070"),
	/*                                  */
	DB8500_MUX("i2c0_a_1", "i2c0", "nmk-i2c.0"),
	/*                  */
	DB8500_PIN_SLEEP("GPIO147_C15", slpm_in_nopull_wkup_pdis, "nmk-i2c.0"), /*     */
	DB8500_PIN_SLEEP("GPIO148_B16", slpm_in_nopull_wkup_pdis, "nmk-i2c.0"), /*     */
	/*                                   */
	DB8500_MUX("i2c1_b_2", "i2c1", "nmk-i2c.1"),
	/*                  */
	DB8500_PIN_SLEEP("GPIO16_AD3", slpm_in_nopull_wkup_pdis, "nmk-i2c.1"), /*     */
	DB8500_PIN_SLEEP("GPIO17_AD4", slpm_in_nopull_wkup_pdis, "nmk-i2c.1"), /*     */
	/*                                   */
	DB8500_MUX("i2c2_b_2", "i2c2", "nmk-i2c.2"),
	/*                  */
	DB8500_PIN_SLEEP("GPIO10_AF5", slpm_in_nopull_wkup_pdis, "nmk-i2c.2"), /*     */
	DB8500_PIN_SLEEP("GPIO11_AG4", slpm_in_nopull_wkup_pdis, "nmk-i2c.2"), /*     */
	/*                                   */
	DB8500_MUX("i2c3_c_2", "i2c3", "nmk-i2c.3"),
	/*                  */
	DB8500_PIN_SLEEP("GPIO229_AG7", slpm_in_nopull_wkup_pdis, "nmk-i2c.3"), /*     */
	DB8500_PIN_SLEEP("GPIO230_AF7", slpm_in_nopull_wkup_pdis, "nmk-i2c.3"), /*     */
	/*                                                                    */
	DB8500_MUX("mc0_a_1", "mc0", "sdi0"),
	DB8500_PIN("GPIO18_AC2", out_hi, "sdi0"), /*        */
	DB8500_PIN("GPIO19_AC1", out_hi, "sdi0"), /*         */
	DB8500_PIN("GPIO20_AB4", out_hi, "sdi0"), /*         */
	DB8500_PIN("GPIO22_AA3", in_nopull, "sdi0"), /*       */
	DB8500_PIN("GPIO23_AA4", out_lo, "sdi0"), /*     */
	DB8500_PIN("GPIO24_AB2", in_pu, "sdi0"), /*     */
	DB8500_PIN("GPIO25_Y4", in_pu, "sdi0"), /*      */
	DB8500_PIN("GPIO26_Y2", in_pu, "sdi0"), /*      */
	DB8500_PIN("GPIO27_AA2", in_pu, "sdi0"), /*      */
	DB8500_PIN("GPIO28_AA1", in_pu, "sdi0"), /*      */
	/*                  */
	DB8500_PIN_SLEEP("GPIO18_AC2", slpm_out_hi_wkup_pdis, "sdi0"),
	DB8500_PIN_SLEEP("GPIO19_AC1", slpm_out_hi_wkup_pdis, "sdi0"),
	DB8500_PIN_SLEEP("GPIO20_AB4", slpm_out_hi_wkup_pdis, "sdi0"),
	DB8500_PIN_SLEEP("GPIO22_AA3", slpm_in_wkup_pdis, "sdi0"),
	DB8500_PIN_SLEEP("GPIO23_AA4", slpm_out_lo_wkup_pdis, "sdi0"),
	DB8500_PIN_SLEEP("GPIO24_AB2", slpm_in_wkup_pdis, "sdi0"),
	DB8500_PIN_SLEEP("GPIO25_Y4", slpm_in_wkup_pdis, "sdi0"),
	DB8500_PIN_SLEEP("GPIO26_Y2", slpm_in_wkup_pdis, "sdi0"),
	DB8500_PIN_SLEEP("GPIO27_AA2", slpm_in_wkup_pdis, "sdi0"),
	DB8500_PIN_SLEEP("GPIO28_AA1", slpm_in_wkup_pdis, "sdi0"),

	/*                                                             */
	DB8500_MUX("mc1_a_1", "mc1", "sdi1"),
	DB8500_PIN("GPIO208_AH16", out_lo, "sdi1"), /*     */
	DB8500_PIN("GPIO209_AG15", in_nopull, "sdi1"), /*       */
	DB8500_PIN("GPIO210_AJ15", in_pu, "sdi1"), /*     */
	DB8500_PIN("GPIO211_AG14", in_pu, "sdi1"), /*      */
	DB8500_PIN("GPIO212_AF13", in_pu, "sdi1"), /*      */
	DB8500_PIN("GPIO213_AG13", in_pu, "sdi1"), /*      */
	DB8500_PIN("GPIO214_AH15", in_pu, "sdi1"), /*      */
	/*                  */
	DB8500_PIN_SLEEP("GPIO208_AH16", slpm_out_lo_wkup_pdis, "sdi1"), /*     */
	DB8500_PIN_SLEEP("GPIO209_AG15", slpm_in_wkup_pdis, "sdi1"), /*       */
	DB8500_PIN_SLEEP("GPIO210_AJ15", slpm_in_wkup_pdis, "sdi1"), /*     */
	DB8500_PIN_SLEEP("GPIO211_AG14", slpm_in_wkup_pdis, "sdi1"), /*      */
	DB8500_PIN_SLEEP("GPIO212_AF13", slpm_in_wkup_pdis, "sdi1"), /*      */
	DB8500_PIN_SLEEP("GPIO213_AG13", slpm_in_wkup_pdis, "sdi1"), /*      */
	DB8500_PIN_SLEEP("GPIO214_AH15", slpm_in_wkup_pdis, "sdi1"), /*      */

	/*                                                     */
	DB8500_MUX("mc2_a_1", "mc2", "sdi2"),
	DB8500_PIN("GPIO128_A5", out_lo, "sdi2"), /*     */
	DB8500_PIN("GPIO129_B4", in_pu, "sdi2"), /*     */
	DB8500_PIN("GPIO130_C8", in_nopull, "sdi2"), /*       */
	DB8500_PIN("GPIO131_A12", in_pu, "sdi2"), /*      */
	DB8500_PIN("GPIO132_C10", in_pu, "sdi2"), /*      */
	DB8500_PIN("GPIO133_B10", in_pu, "sdi2"), /*      */
	DB8500_PIN("GPIO134_B9", in_pu, "sdi2"), /*      */
	DB8500_PIN("GPIO135_A9", in_pu, "sdi2"), /*      */
	DB8500_PIN("GPIO136_C7", in_pu, "sdi2"), /*      */
	DB8500_PIN("GPIO137_A7", in_pu, "sdi2"), /*      */
	DB8500_PIN("GPIO138_C5", in_pu, "sdi2"), /*      */
	/*                  */
	DB8500_PIN_SLEEP("GPIO128_A5", out_lo_wkup_pdis, "sdi2"), /*     */
	DB8500_PIN_SLEEP("GPIO129_B4", in_wkup_pdis_en, "sdi2"), /*     */
	DB8500_PIN_SLEEP("GPIO130_C8", in_wkup_pdis_en, "sdi2"), /*       */
	DB8500_PIN_SLEEP("GPIO131_A12", in_wkup_pdis, "sdi2"), /*      */
	DB8500_PIN_SLEEP("GPIO132_C10", in_wkup_pdis, "sdi2"), /*      */
	DB8500_PIN_SLEEP("GPIO133_B10", in_wkup_pdis, "sdi2"), /*      */
	DB8500_PIN_SLEEP("GPIO134_B9", in_wkup_pdis, "sdi2"), /*      */
	DB8500_PIN_SLEEP("GPIO135_A9", in_wkup_pdis, "sdi2"), /*      */
	DB8500_PIN_SLEEP("GPIO136_C7", in_wkup_pdis, "sdi2"), /*      */
	DB8500_PIN_SLEEP("GPIO137_A7", in_wkup_pdis, "sdi2"), /*      */
	DB8500_PIN_SLEEP("GPIO138_C5", in_wkup_pdis, "sdi2"), /*      */

	/*                                                             */
	DB8500_MUX("mc4_a_1", "mc4", "sdi4"),
	DB8500_PIN("GPIO197_AH24", in_pu, "sdi4"), /*      */
	DB8500_PIN("GPIO198_AG25", in_pu, "sdi4"), /*      */
	DB8500_PIN("GPIO199_AH23", in_pu, "sdi4"), /*      */
	DB8500_PIN("GPIO200_AH26", in_pu, "sdi4"), /*      */
	DB8500_PIN("GPIO201_AF24", in_pu, "sdi4"), /*     */
	DB8500_PIN("GPIO202_AF25", in_nopull, "sdi4"), /*       */
	DB8500_PIN("GPIO203_AE23", out_lo, "sdi4"), /*     */
	DB8500_PIN("GPIO204_AF23", in_pu, "sdi4"), /*      */
	DB8500_PIN("GPIO205_AG23", in_pu, "sdi4"), /*      */
	DB8500_PIN("GPIO206_AG24", in_pu, "sdi4"), /*      */
	DB8500_PIN("GPIO207_AJ23", in_pu, "sdi4"), /*      */
	/*                 */
	DB8500_PIN_SLEEP("GPIO197_AH24", slpm_in_wkup_pdis, "sdi4"), /*      */
	DB8500_PIN_SLEEP("GPIO198_AG25", slpm_in_wkup_pdis, "sdi4"), /*      */
	DB8500_PIN_SLEEP("GPIO199_AH23", slpm_in_wkup_pdis, "sdi4"), /*      */
	DB8500_PIN_SLEEP("GPIO200_AH26", slpm_in_wkup_pdis, "sdi4"), /*      */
	DB8500_PIN_SLEEP("GPIO201_AF24", slpm_in_wkup_pdis, "sdi4"), /*     */
	DB8500_PIN_SLEEP("GPIO202_AF25", slpm_in_wkup_pdis, "sdi4"), /*       */
	DB8500_PIN_SLEEP("GPIO203_AE23", slpm_out_lo_wkup_pdis, "sdi4"), /*     */
	DB8500_PIN_SLEEP("GPIO204_AF23", slpm_in_wkup_pdis, "sdi4"), /*      */
	DB8500_PIN_SLEEP("GPIO205_AG23", slpm_in_wkup_pdis, "sdi4"), /*      */
	DB8500_PIN_SLEEP("GPIO206_AG24", slpm_in_wkup_pdis, "sdi4"), /*      */
	DB8500_PIN_SLEEP("GPIO207_AJ23", slpm_in_wkup_pdis, "sdi4"), /*      */

	/*                                 */
	/*                   */
	DB8500_MUX("usb_a_1", "usb", "ab8500-usb.0"),
	DB8500_PIN("GPIO257_AE29", out_hi, "ab8500-usb.0"), /*     */
	/*                 */
	DB8500_PIN_SLEEP("GPIO256_AF28", slpm_wkup_pdis_en, "ab8500-usb.0"), /*     */
	DB8500_PIN_SLEEP("GPIO257_AE29", slpm_out_hi_wkup_pdis, "ab8500-usb.0"), /*     */
	DB8500_PIN_SLEEP("GPIO258_AD29", slpm_wkup_pdis_en, "ab8500-usb.0"), /*      */
	DB8500_PIN_SLEEP("GPIO259_AC29", slpm_wkup_pdis_en, "ab8500-usb.0"), /*     */
	DB8500_PIN_SLEEP("GPIO260_AD28", slpm_in_wkup_pdis_en, "ab8500-usb.0"), /*      */
	DB8500_PIN_SLEEP("GPIO261_AD26", slpm_in_wkup_pdis_en, "ab8500-usb.0"), /*      */
	DB8500_PIN_SLEEP("GPIO262_AE26", slpm_in_wkup_pdis_en, "ab8500-usb.0"), /*      */
	DB8500_PIN_SLEEP("GPIO263_AG29", slpm_in_wkup_pdis_en, "ab8500-usb.0"), /*      */
	DB8500_PIN_SLEEP("GPIO264_AE27", slpm_in_wkup_pdis_en, "ab8500-usb.0"), /*      */
	DB8500_PIN_SLEEP("GPIO265_AD27", slpm_in_wkup_pdis_en, "ab8500-usb.0"), /*      */
	DB8500_PIN_SLEEP("GPIO266_AC28", slpm_in_wkup_pdis_en, "ab8500-usb.0"), /*      */
	DB8500_PIN_SLEEP("GPIO267_AC27", slpm_in_wkup_pdis_en, "ab8500-usb.0"), /*      */

	/*                                                */
	DB8500_MUX("spi2_oc1_2", "spi2", "spi2"),
	DB8500_PIN("GPIO216_AG12", gpio_out_hi, "spi2"), /*     */
	DB8500_PIN("GPIO218_AH11", in_pd, "spi2"), /*     */
	DB8500_PIN("GPIO215_AH13", out_lo, "spi2"), /*     */
	DB8500_PIN("GPIO217_AH12", out_lo, "spi2"), /*     */
	/*                 */
	DB8500_PIN_IDLE("GPIO218_AH11", slpm_in_wkup_pdis, "spi2"), /*     */
	DB8500_PIN_IDLE("GPIO215_AH13", slpm_out_lo_wkup_pdis, "spi2"), /*     */
	DB8500_PIN_IDLE("GPIO217_AH12", slpm_wkup_pdis, "spi2"), /*     */
	/*                  */
	DB8500_PIN_SLEEP("GPIO216_AG12", slpm_in_wkup_pdis, "spi2"), /*     */
	DB8500_PIN_SLEEP("GPIO218_AH11", slpm_in_wkup_pdis, "spi2"), /*     */
	DB8500_PIN_SLEEP("GPIO215_AH13", slpm_out_lo_wkup_pdis, "spi2"), /*     */
	DB8500_PIN_SLEEP("GPIO217_AH12", slpm_wkup_pdis, "spi2"), /*     */

	/*                   */
	DB8500_MUX("kp_a_2", "kp", "nmk-ske-keypad"),
	DB8500_PIN("GPIO153_B17", in_pd, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO154_C16", in_pd, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO155_C19", in_pd, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO156_C17", in_pd, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO161_D21", in_pd, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO162_D20", in_pd, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO163_C20", in_pd, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO164_B21", in_pd, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO157_A18", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO158_C18", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO159_B19", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO160_B20", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO165_C21", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO166_A22", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO167_B24", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO168_C22", out_lo, "nmk-ske-keypad"), /*    */
	/*                 */
	DB8500_PIN_SLEEP("GPIO153_B17", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO154_C16", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO155_C19", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO156_C17", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO161_D21", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO162_D20", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO163_C20", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO164_B21", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO157_A18", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO158_C18", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO159_B19", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO160_B20", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO165_C21", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO166_A22", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO167_B24", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO168_C22", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */

	/*                     */
	DB8500_MUX_STATE("stmape_c_1", "stmape",
		"stm", "ape_mipi34"),
	DB8500_PIN_STATE("GPIO70_G5", in_nopull,
		"stm", "ape_mipi34"), /*     */
	DB8500_PIN_STATE("GPIO71_G4", in_nopull,
		"stm", "ape_mipi34"), /*      */
	DB8500_PIN_STATE("GPIO72_H4", in_nopull,
		"stm", "ape_mipi34"), /*      */
	DB8500_PIN_STATE("GPIO73_H3", in_nopull,
		"stm", "ape_mipi34"), /*      */
	DB8500_PIN_STATE("GPIO74_J3", in_nopull,
		"stm", "ape_mipi34"), /*      */

	DB8500_PIN_STATE("GPIO70_G5", slpm_out_lo_pdis,
		"stm", "ape_mipi34_sleep"), /*     */
	DB8500_PIN_STATE("GPIO71_G4", slpm_out_lo_pdis,
		"stm", "ape_mipi34_sleep"), /*      */
	DB8500_PIN_STATE("GPIO72_H4", slpm_out_lo_pdis,
		"stm", "ape_mipi34_sleep"), /*      */
	DB8500_PIN_STATE("GPIO73_H3", slpm_out_lo_pdis,
		"stm", "ape_mipi34_sleep"), /*      */
	DB8500_PIN_STATE("GPIO74_J3", slpm_out_lo_pdis,
		"stm", "ape_mipi34_sleep"), /*      */

	DB8500_MUX_STATE("stmape_oc1_1", "stmape",
		"stm", "ape_microsd"),
	DB8500_PIN_STATE("GPIO23_AA4", in_nopull,
		"stm", "ape_microsd"), /*     */
	DB8500_PIN_STATE("GPIO25_Y4", in_nopull,
		"stm", "ape_microsd"), /*      */
	DB8500_PIN_STATE("GPIO26_Y2", in_nopull,
		"stm", "ape_microsd"), /*      */
	DB8500_PIN_STATE("GPIO27_AA2", in_nopull,
		"stm", "ape_microsd"), /*      */
	DB8500_PIN_STATE("GPIO28_AA1", in_nopull,
		"stm", "ape_microsd"), /*      */

	DB8500_PIN_STATE("GPIO23_AA4", slpm_out_lo_wkup_pdis,
		"stm", "ape_microsd_sleep"), /*     */
	DB8500_PIN_STATE("GPIO25_Y4", slpm_in_wkup_pdis,
		"stm", "ape_microsd_sleep"), /*      */
	DB8500_PIN_STATE("GPIO26_Y2", slpm_in_wkup_pdis,
		"stm", "ape_microsd_sleep"), /*      */
	DB8500_PIN_STATE("GPIO27_AA2", slpm_in_wkup_pdis,
		"stm", "ape_microsd_sleep"), /*      */
	DB8500_PIN_STATE("GPIO28_AA1", slpm_in_wkup_pdis,
		"stm", "ape_microsd_sleep"), /*      */

	/*                        */
	DB8500_MUX_STATE("stmmod_oc3_2", "stmmod",
		"stm", "mod_mipi34"),
	DB8500_MUX_STATE("uartmodrx_oc3_1", "uartmod",
		"stm", "mod_mipi34"),
	DB8500_MUX_STATE("uartmodtx_oc3_1", "uartmod",
		"stm", "mod_mipi34"),
	DB8500_PIN_STATE("GPIO70_G5", in_nopull,
		"stm", "mod_mipi34"), /*     */
	DB8500_PIN_STATE("GPIO71_G4", in_nopull,
		"stm", "mod_mipi34"), /*      */
	DB8500_PIN_STATE("GPIO72_H4", in_nopull,
		"stm", "mod_mipi34"), /*      */
	DB8500_PIN_STATE("GPIO73_H3", in_nopull,
		"stm", "mod_mipi34"), /*      */
	DB8500_PIN_STATE("GPIO74_J3", in_nopull,
		"stm", "mod_mipi34"), /*      */
	DB8500_PIN_STATE("GPIO75_H2", in_pu,
		"stm", "mod_mipi34"), /*            */
	DB8500_PIN_STATE("GPIO76_J2", out_lo,
		"stm", "mod_mipi34"), /*            */

	DB8500_PIN_STATE("GPIO70_G5", slpm_out_lo_pdis,
		"stm", "mod_mipi34_sleep"), /*     */
	DB8500_PIN_STATE("GPIO71_G4", slpm_out_lo_pdis,
		"stm", "mod_mipi34_sleep"), /*      */
	DB8500_PIN_STATE("GPIO72_H4", slpm_out_lo_pdis,
		"stm", "mod_mipi34_sleep"), /*      */
	DB8500_PIN_STATE("GPIO73_H3", slpm_out_lo_pdis,
		"stm", "mod_mipi34_sleep"), /*      */
	DB8500_PIN_STATE("GPIO74_J3", slpm_out_lo_pdis,
		"stm", "mod_mipi34_sleep"), /*      */
	DB8500_PIN_STATE("GPIO75_H2", slpm_in_wkup_pdis,
		"stm", "mod_mipi34_sleep"), /*            */
	DB8500_PIN_STATE("GPIO76_J2", slpm_out_lo_wkup_pdis,
		"stm", "mod_mipi34_sleep"), /*            */

	DB8500_MUX_STATE("stmmod_b_1", "stmmod",
		"stm", "mod_microsd"),
	DB8500_MUX_STATE("uartmodrx_oc3_1", "uartmod",
		"stm", "mod_microsd"),
	DB8500_MUX_STATE("uartmodtx_oc3_1", "uartmod",
		"stm", "mod_microsd"),
	DB8500_PIN_STATE("GPIO23_AA4", in_nopull,
		"stm", "mod_microsd"), /*     */
	DB8500_PIN_STATE("GPIO25_Y4", in_nopull,
		"stm", "mod_microsd"), /*      */
	DB8500_PIN_STATE("GPIO26_Y2", in_nopull,
		"stm", "mod_microsd"), /*      */
	DB8500_PIN_STATE("GPIO27_AA2", in_nopull,
		"stm", "mod_microsd"), /*      */
	DB8500_PIN_STATE("GPIO28_AA1", in_nopull,
		"stm", "mod_microsd"), /*      */
	DB8500_PIN_STATE("GPIO75_H2", in_pu,
		"stm", "mod_microsd"), /*            */
	DB8500_PIN_STATE("GPIO76_J2", out_lo,
		"stm", "mod_microsd"), /*            */

	DB8500_PIN_STATE("GPIO23_AA4", slpm_out_lo_wkup_pdis,
		"stm", "mod_microsd_sleep"), /*     */
	DB8500_PIN_STATE("GPIO25_Y4", slpm_in_wkup_pdis,
		"stm", "mod_microsd_sleep"), /*      */
	DB8500_PIN_STATE("GPIO26_Y2", slpm_in_wkup_pdis,
		"stm", "mod_microsd_sleep"), /*      */
	DB8500_PIN_STATE("GPIO27_AA2", slpm_in_wkup_pdis,
		"stm", "mod_microsd_sleep"), /*      */
	DB8500_PIN_STATE("GPIO28_AA1", slpm_in_wkup_pdis,
		"stm", "mod_microsd_sleep"), /*      */
	DB8500_PIN_STATE("GPIO75_H2", slpm_in_wkup_pdis,
		"stm", "mod_microsd_sleep"), /*            */
	DB8500_PIN_STATE("GPIO76_J2", slpm_out_lo_wkup_pdis,
		"stm", "mod_microsd_sleep"), /*            */

	/*                                */
	DB8500_MUX_STATE("stmmod_oc3_2", "stmmod",
		"stm", "mod_mipi34_ape_mipi60"),
	DB8500_MUX_STATE("stmape_c_2", "stmape",
		"stm", "mod_mipi34_ape_mipi60"),
	DB8500_MUX_STATE("uartmodrx_oc3_1", "uartmod",
		"stm", "mod_mipi34_ape_mipi60"),
	DB8500_MUX_STATE("uartmodtx_oc3_1", "uartmod",
		"stm", "mod_mipi34_ape_mipi60"),
	DB8500_PIN_STATE("GPIO70_G5", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*     */
	DB8500_PIN_STATE("GPIO71_G4", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*      */
	DB8500_PIN_STATE("GPIO72_H4", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*      */
	DB8500_PIN_STATE("GPIO73_H3", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*      */
	DB8500_PIN_STATE("GPIO74_J3", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*      */
	DB8500_PIN_STATE("GPIO75_H2", in_pu,
		"stm", "mod_mipi34_ape_mipi60"), /*            */
	DB8500_PIN_STATE("GPIO76_J2", out_lo,
		"stm", "mod_mipi34_ape_mipi60"), /*            */
	DB8500_PIN_STATE("GPIO155_C19", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*     */
	DB8500_PIN_STATE("GPIO156_C17", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*      */
	DB8500_PIN_STATE("GPIO157_A18", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*      */
	DB8500_PIN_STATE("GPIO158_C18", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*      */
	DB8500_PIN_STATE("GPIO159_B19", in_nopull,
		"stm", "mod_mipi34_ape_mipi60"), /*      */

	DB8500_PIN_STATE("GPIO70_G5", slpm_out_lo_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*     */
	DB8500_PIN_STATE("GPIO71_G4", slpm_out_lo_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*      */
	DB8500_PIN_STATE("GPIO72_H4", slpm_out_lo_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*      */
	DB8500_PIN_STATE("GPIO73_H3", slpm_out_lo_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*      */
	DB8500_PIN_STATE("GPIO74_J3", slpm_out_lo_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*      */
	DB8500_PIN_STATE("GPIO75_H2", slpm_in_wkup_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*            */
	DB8500_PIN_STATE("GPIO76_J2", slpm_out_lo_wkup_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*            */
	DB8500_PIN_STATE("GPIO155_C19", slpm_in_wkup_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*     */
	DB8500_PIN_STATE("GPIO156_C17", slpm_in_wkup_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*      */
	DB8500_PIN_STATE("GPIO157_A18", slpm_in_wkup_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*      */
	DB8500_PIN_STATE("GPIO158_C18", slpm_in_wkup_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*      */
	DB8500_PIN_STATE("GPIO159_B19", slpm_in_wkup_pdis,
		"stm", "mod_mipi34_ape_mipi60_sleep"), /*      */
};

/*
                                                                           
                                                                          
                                              
 */
static struct pinctrl_map __initdata mop500_pinmap[] = {
	/*                                */
	DB8500_MUX_HOG("ssp0_a_1", "ssp0"),
	DB8500_PIN_HOG("GPIO145_C13", pd),
	/*
                                                                 
                                                                  
                                                                     
             
  */
	DB8500_MUX_HOG("ipgpio0_c_1", "ipgpio"),
	DB8500_MUX_HOG("ipgpio1_c_1", "ipgpio"),
	DB8500_PIN_HOG("GPIO6_AF6", in_pu),
	DB8500_PIN_HOG("GPIO7_AG5", in_pu),
	/*                                                              */
	DB8500_PIN_HOG("GPIO217_AH12", gpio_in_pu),
	/*                                   */
	DB8500_MUX_HOG("u1rxtx_a_1", "u1"),
	DB8500_PIN_HOG("GPIO4_AH6", in_pu), /*     */
	DB8500_PIN_HOG("GPIO5_AG6", out_hi), /*     */
	/*
                                                            
                     
  */
	/*                   */
	DB8500_MUX("kp_a_2", "kp", "nmk-ske-keypad"),
	DB8500_PIN("GPIO153_B17", in_pu, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO154_C16", in_pu, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO155_C19", in_pu, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO156_C17", in_pu, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO161_D21", in_pu, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO162_D20", in_pu, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO163_C20", in_pu, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO164_B21", in_pu, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO157_A18", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO158_C18", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO159_B19", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO160_B20", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO165_C21", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO166_A22", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO167_B24", out_lo, "nmk-ske-keypad"), /*    */
	DB8500_PIN("GPIO168_C22", out_lo, "nmk-ske-keypad"), /*    */
	/*                 */
	DB8500_PIN_SLEEP("GPIO153_B17", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO154_C16", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO155_C19", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO156_C17", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO161_D21", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO162_D20", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO163_C20", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO164_B21", slpm_in_pu_wkup_pdis_en, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO157_A18", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO158_C18", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO159_B19", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO160_B20", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO165_C21", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO166_A22", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO167_B24", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */
	DB8500_PIN_SLEEP("GPIO168_C22", slpm_out_lo_pdis, "nmk-ske-keypad"), /*    */

	/*                                                         */
	DB8500_MUX("mc0dat31dir_a_1", "mc0", "sdi0"),
	DB8500_PIN("GPIO21_AB3", out_hi, "sdi0"),
};

/*
                                                                        
                                                                            
                                                                             
 */
static struct pinctrl_map __initdata hrefv60_pinmap[] = {
	/*                    */
	DB8500_PIN_HOG("GPIO85_D5", gpio_out_lo), /*          */
	/*
                                                                 
                                                                  
                                                               
                                                                 
                                                                     
  */
	DB8500_MUX_HOG("ipgpio0_c_1", "ipgpio"),
	DB8500_MUX_HOG("ipgpio1_c_1", "ipgpio"),
	DB8500_MUX_HOG("ipgpio4_c_1", "ipgpio"),
	DB8500_PIN_HOG("GPIO6_AF6", in_pu), /*                */
	DB8500_PIN_HOG("GPIO7_AG5", in_pu), /*             */
	DB8500_PIN_HOG("GPIO21_AB3", gpio_out_lo), /*           */
	DB8500_PIN_HOG("GPIO64_F3", out_lo), /*           */
	/*                                                                   */
	DB8500_PIN_HOG("GPIO31_V3", gpio_in_pu), /*     */
	DB8500_PIN_HOG("GPIO32_V2", gpio_in_pd), /*      */
	/*
                                                     
                                                     
                                                                      
  */
	DB8500_PIN_HOG("GPIO65_F1", gpio_out_hi), /*              */
	DB8500_PIN_HOG("GPIO66_G3", gpio_out_lo), /*           */
	/*
                                                              
                                                               
                      
  */
	DB8500_PIN_HOG("GPIO143_D12", gpio_out_lo), /*            */
	DB8500_PIN_HOG("GPIO67_G2", gpio_in_pu), /*            */
	DB8500_PIN_HOG("GPIO146_D13", gpio_out_lo), /*            */
	/*
                                                      
                                                             
                                                  
  */
	DB8500_PIN_HOG("GPIO70_G5", gpio_out_lo),
	DB8500_PIN_HOG("GPIO71_G4", gpio_out_lo),
	DB8500_PIN_HOG("GPIO72_H4", gpio_out_lo),
	DB8500_PIN_HOG("GPIO73_H3", gpio_out_lo),
	DB8500_PIN_HOG("GPIO74_J3", gpio_out_lo),
	/*                                                          */
	DB8500_PIN_HOG("GPIO76_J2", gpio_out_lo), /*      */
	DB8500_PIN_HOG("GPIO216_AG12", gpio_out_hi), /*          */
	/*                                             */
	DB8500_PIN_HOG("GPIO77_H1", gpio_out_lo), /*         */
	DB8500_PIN_HOG("GPIO144_B13", gpio_in_pd), /*         */
	DB8500_PIN_HOG("GPIO142_C11", gpio_out_lo), /*           */
	/*
                                                             
                                                             
                                                           
                                         
  */
	DB8500_MUX_HOG("kp_a_1", "kp"),
	DB8500_MUX_HOG("kp_oc1_1", "kp"),
	DB8500_PIN_HOG("GPIO90_A3", out_lo), /*       */
	DB8500_PIN_HOG("GPIO87_B3", out_lo), /*       */
	DB8500_PIN_HOG("GPIO86_C6", out_lo), /*       */
	DB8500_PIN_HOG("GPIO96_D8", out_lo), /*       */
	DB8500_PIN_HOG("GPIO94_D7", out_lo), /*       */
	DB8500_PIN_HOG("GPIO93_B7", in_pu), /*       */
	DB8500_PIN_HOG("GPIO89_E6", in_pu), /*       */
	DB8500_PIN_HOG("GPIO88_C4", in_pu), /*       */
	DB8500_PIN_HOG("GPIO91_B6", gpio_in_pu), /*                   */
	DB8500_PIN_HOG("GPIO92_D6", gpio_out_lo), /*                   */
	DB8500_PIN_HOG("GPIO97_D9", gpio_out_lo), /*                  */
	/*                        */
	DB8500_PIN_HOG("GPIO139_C9", gpio_in_pu), /*           */
	/*                           */
	DB8500_PIN_HOG("GPIO149_B14", gpio_out_hi), /*                              */
	/*                                        */
	DB8500_PIN_HOG("GPIO171_D23", gpio_out_lo), /*               */
	/*                                */
	DB8500_PIN_HOG("GPIO192_AJ27", gpio_in_pd),
	/*                               */
	DB8500_PIN_HOG("GPIO82_C1", gpio_in_pu), /*          */
	DB8500_PIN_HOG("GPIO83_D3", gpio_in_pu), /*          */
	/*                         */
	DB8500_PIN_HOG("GPIO95_E8", gpio_in_pu),
	/*
                 
                                                                    
        
  */
	DB8500_PIN("GPIO217_AH12", gpio_in_pu_slpm_gpio_nopull, "gpio-keys.0"),
	DB8500_PIN("GPIO145_C13", gpio_in_pd_slpm_gpio_nopull, "gpio-keys.0"),
	DB8500_PIN("GPIO139_C9", gpio_in_pu_slpm_gpio_nopull, "gpio-keys.0"),
};

static struct pinctrl_map __initdata u9500_pinmap[] = {
	/*                                                */
	DB8500_MUX_HOG("u1rxtx_a_1", "u1"),
	DB8500_PIN_HOG("GPIO4_AH6", in_pu),
	DB8500_PIN_HOG("GPIO5_AG6", out_hi),
	/*               */
	DB8500_PIN_HOG("GPIO144_B13", gpio_in_pu),
	/*     */
	DB8500_MUX_HOG("hsir_a_1", "hsi"),
	DB8500_MUX_HOG("hsit_a_2", "hsi"),
	DB8500_PIN_HOG("GPIO219_AG10", in_pd), /*         */
	DB8500_PIN_HOG("GPIO220_AH10", in_pd), /*         */
	DB8500_PIN_HOG("GPIO221_AJ11", out_lo), /*         */
	DB8500_PIN_HOG("GPIO222_AJ9", out_lo), /*         */
	DB8500_PIN_HOG("GPIO223_AH9", out_lo), /*         */
	DB8500_PIN_HOG("GPIO224_AG9", in_pd), /*         */
	DB8500_PIN_HOG("GPIO225_AG8", in_pd), /*         */
	DB8500_PIN_HOG("GPIO226_AF8", gpio_out_hi), /*         */
};

static struct pinctrl_map __initdata u8500_pinmap[] = {
	DB8500_PIN_HOG("GPIO226_AF8", gpio_out_lo), /*             */
	DB8500_PIN_HOG("GPIO4_AH6", gpio_in_pu), /*          */
};

static struct pinctrl_map __initdata snowball_pinmap[] = {
	/*                                                    */
	DB8500_MUX_HOG("ssp0_a_1", "ssp0"),
	DB8500_PIN_HOG("GPIO145_C13", pd),
	/*                                                         */
	DB8500_PIN_HOG("GPIO21_AB3", out_hi),
	/*                                                             */
	DB8500_MUX_HOG("sm_b_1", "sm"),
	/*          */
	DB8500_PIN_HOG("GPIO142_C11", gpio_out_hi),
	/*                     */
	DB8500_PIN_HOG("GPIO141_C12", gpio_out_hi),
	/*                             */
	DB8500_PIN_HOG("GPIO163_C20", gpio_in_pu), /*            */
	DB8500_PIN_HOG("GPIO164_B21", gpio_in_pu), /*            */
	DB8500_PIN_HOG("GPIO165_C21", gpio_in_pu), /*          */
	/*          */
	DB8500_PIN_HOG("GPIO161_D21", gpio_out_lo), /*             */
	DB8500_PIN_HOG("GPIO171_D23", gpio_out_hi), /*         */
	DB8500_PIN_HOG("GPIO215_AH13", gpio_out_lo), /*          */
	DB8500_PIN_HOG("GPIO216_AG12", gpio_in_pu), /*          */
};

/*
                                                         
                                                  
 */
static int __init early_pinsfor(char *p)
{
	pinsfor = PINS_FOR_DEFAULT;

	if (strcmp(p, "u9500-21") == 0)
		pinsfor = PINS_FOR_U9500;

	return 0;
}
early_param("pinsfor", early_pinsfor);

int pins_for_u9500(void)
{
	if (pinsfor == PINS_FOR_U9500)
		return 1;

	return 0;
}

static void __init mop500_href_family_pinmaps_init(void)
{
	switch (pinsfor) {
	case PINS_FOR_U9500:
		pinctrl_register_mappings(u9500_pinmap,
					  ARRAY_SIZE(u9500_pinmap));
		break;
	case PINS_FOR_DEFAULT:
		pinctrl_register_mappings(u8500_pinmap,
					  ARRAY_SIZE(u8500_pinmap));
	default:
		break;
	}
}

void __init mop500_pinmaps_init(void)
{
	pinctrl_register_mappings(mop500_family_pinmap,
				  ARRAY_SIZE(mop500_family_pinmap));
	pinctrl_register_mappings(mop500_pinmap,
				  ARRAY_SIZE(mop500_pinmap));
	mop500_href_family_pinmaps_init();
}

void __init snowball_pinmaps_init(void)
{
	pinctrl_register_mappings(mop500_family_pinmap,
				  ARRAY_SIZE(mop500_family_pinmap));
	pinctrl_register_mappings(snowball_pinmap,
				  ARRAY_SIZE(snowball_pinmap));
	pinctrl_register_mappings(u8500_pinmap,
				  ARRAY_SIZE(u8500_pinmap));
}

void __init hrefv60_pinmaps_init(void)
{
	pinctrl_register_mappings(mop500_family_pinmap,
				  ARRAY_SIZE(mop500_family_pinmap));
	pinctrl_register_mappings(hrefv60_pinmap,
				  ARRAY_SIZE(hrefv60_pinmap));
	mop500_href_family_pinmaps_init();
}
