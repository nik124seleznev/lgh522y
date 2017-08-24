/*
 * Copyright (C) ST-Ericsson SA 2012
 *
 * Author: Patrice Chotard <patrice.chotard@stericsson.com> for ST-Ericsson.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/mfd/abx500/ab8500.h>
#include "pinctrl-abx500.h"

/*                                                                 */
#define ABX500_GPIO(offset)		(offset)

#define AB8540_PIN_J16		ABX500_GPIO(1)
#define AB8540_PIN_D17		ABX500_GPIO(2)
#define AB8540_PIN_C12		ABX500_GPIO(3)
#define AB8540_PIN_G12		ABX500_GPIO(4)
/*      */
#define AB8540_PIN_D16		ABX500_GPIO(14)
#define AB8540_PIN_F15		ABX500_GPIO(15)
#define AB8540_PIN_J8		ABX500_GPIO(16)
#define AB8540_PIN_K16		ABX500_GPIO(17)
#define AB8540_PIN_G15		ABX500_GPIO(18)
#define AB8540_PIN_F17		ABX500_GPIO(19)
#define AB8540_PIN_E17		ABX500_GPIO(20)
/*      */
#define AB8540_PIN_AA16		ABX500_GPIO(27)
#define AB8540_PIN_W18		ABX500_GPIO(28)
#define AB8540_PIN_Y15		ABX500_GPIO(29)
#define AB8540_PIN_W16		ABX500_GPIO(30)
#define AB8540_PIN_V15		ABX500_GPIO(31)
#define AB8540_PIN_W17		ABX500_GPIO(32)
/*      */
#define AB8540_PIN_D12		ABX500_GPIO(42)
#define AB8540_PIN_P4		ABX500_GPIO(43)
#define AB8540_PIN_AB1		ABX500_GPIO(44)
#define AB8540_PIN_K7		ABX500_GPIO(45)
#define AB8540_PIN_L7		ABX500_GPIO(46)
#define AB8540_PIN_G10		ABX500_GPIO(47)
#define AB8540_PIN_K12		ABX500_GPIO(48)
/*      */
#define AB8540_PIN_N8		ABX500_GPIO(51)
#define AB8540_PIN_P12		ABX500_GPIO(52)
#define AB8540_PIN_K8		ABX500_GPIO(53)
#define AB8540_PIN_J11		ABX500_GPIO(54)
#define AB8540_PIN_AC2		ABX500_GPIO(55)
#define AB8540_PIN_AB2		ABX500_GPIO(56)

/*                                   */
#define AB8540_GPIO_MAX_NUMBER	56

/*
                                                                       
                                                                        
                                                                         
        
 */
static const struct pinctrl_pin_desc ab8540_pins[] = {
	PINCTRL_PIN(AB8540_PIN_J16, "GPIO1_J16"),
	PINCTRL_PIN(AB8540_PIN_D17, "GPIO2_D17"),
	PINCTRL_PIN(AB8540_PIN_C12, "GPIO3_C12"),
	PINCTRL_PIN(AB8540_PIN_G12, "GPIO4_G12"),
	/*      */
	PINCTRL_PIN(AB8540_PIN_D16, "GPIO14_D16"),
	PINCTRL_PIN(AB8540_PIN_F15, "GPIO15_F15"),
	PINCTRL_PIN(AB8540_PIN_J8, "GPIO16_J8"),
	PINCTRL_PIN(AB8540_PIN_K16, "GPIO17_K16"),
	PINCTRL_PIN(AB8540_PIN_G15, "GPIO18_G15"),
	PINCTRL_PIN(AB8540_PIN_F17, "GPIO19_F17"),
	PINCTRL_PIN(AB8540_PIN_E17, "GPIO20_E17"),
	/*      */
	PINCTRL_PIN(AB8540_PIN_AA16, "GPIO27_AA16"),
	PINCTRL_PIN(AB8540_PIN_W18, "GPIO28_W18"),
	PINCTRL_PIN(AB8540_PIN_Y15, "GPIO29_Y15"),
	PINCTRL_PIN(AB8540_PIN_W16, "GPIO30_W16"),
	PINCTRL_PIN(AB8540_PIN_V15, "GPIO31_V15"),
	PINCTRL_PIN(AB8540_PIN_W17, "GPIO32_W17"),
	/*      */
	PINCTRL_PIN(AB8540_PIN_D12, "GPIO42_D12"),
	PINCTRL_PIN(AB8540_PIN_P4, "GPIO43_P4"),
	PINCTRL_PIN(AB8540_PIN_AB1, "GPIO44_AB1"),
	PINCTRL_PIN(AB8540_PIN_K7, "GPIO45_K7"),
	PINCTRL_PIN(AB8540_PIN_L7, "GPIO46_L7"),
	PINCTRL_PIN(AB8540_PIN_G10, "GPIO47_G10"),
	PINCTRL_PIN(AB8540_PIN_K12, "GPIO48_K12"),
	/*      */
	PINCTRL_PIN(AB8540_PIN_N8, "GPIO51_N8"),
	PINCTRL_PIN(AB8540_PIN_P12, "GPIO52_P12"),
	PINCTRL_PIN(AB8540_PIN_K8, "GPIO53_K8"),
	PINCTRL_PIN(AB8540_PIN_J11, "GPIO54_J11"),
	PINCTRL_PIN(AB8540_PIN_AC2, "GPIO55_AC2"),
	PINCTRL_PIN(AB8540_PIN_AB2, "GPIO56_AB2"),
};

/*
                                               
 */
static const struct abx500_pinrange ab8540_pinranges[] = {
	ABX500_PINRANGE(1, 4, ABX500_ALT_A),
	ABX500_PINRANGE(14, 7, ABX500_ALT_A),
	ABX500_PINRANGE(27, 6, ABX500_ALT_A),
	ABX500_PINRANGE(42, 7, ABX500_ALT_A),
	ABX500_PINRANGE(51, 6, ABX500_ALT_A),
};

/*
                                      
                                                                           
  
                                                                    
                                                                        
                                                                       
                             
 */

/*                */
static const unsigned sysclkreq2_d_1_pins[] = { AB8540_PIN_J16 };
static const unsigned sysclkreq3_d_1_pins[] = { AB8540_PIN_D17 };
static const unsigned sysclkreq4_d_1_pins[] = { AB8540_PIN_C12 };
static const unsigned sysclkreq6_d_1_pins[] = { AB8540_PIN_G12 };
static const unsigned pwmout1_d_1_pins[] = { AB8540_PIN_D16 };
static const unsigned pwmout2_d_1_pins[] = { AB8540_PIN_F15 };
static const unsigned pwmout3_d_1_pins[] = { AB8540_PIN_J8 };

/*                       */
static const unsigned adi1_d_1_pins[] = { AB8540_PIN_K16, AB8540_PIN_G15,
					AB8540_PIN_F17, AB8540_PIN_E17 };
/*                            */
static const unsigned dmic12_d_1_pins[] = { AB8540_PIN_AA16, AB8540_PIN_W18 };
/*                            */
static const unsigned dmic34_d_1_pins[] = { AB8540_PIN_Y15, AB8540_PIN_W16 };
/*                            */
static const unsigned dmic56_d_1_pins[] = { AB8540_PIN_V15, AB8540_PIN_W17 };
static const unsigned sysclkreq5_d_1_pins[] = { AB8540_PIN_D12 };
static const unsigned batremn_d_1_pins[] = { AB8540_PIN_P4 };
static const unsigned service_d_1_pins[] = { AB8540_PIN_AB1 };
static const unsigned pwrctrl0_d_1_pins[] = { AB8540_PIN_K7 };
static const unsigned pwrctrl1_d_1_pins[] = { AB8540_PIN_L7 };
static const unsigned pwmextvibra1_d_1_pins[] = { AB8540_PIN_G10 };
static const unsigned pwmextvibra2_d_1_pins[] = { AB8540_PIN_K12 };
static const unsigned gpio1_vbat_d_1_pins[] = { AB8540_PIN_N8 };
static const unsigned gpio2_vbat_d_1_pins[] = { AB8540_PIN_P12 };
static const unsigned gpio3_vbat_d_1_pins[] = { AB8540_PIN_K8 };
static const unsigned gpio4_vbat_d_1_pins[] = { AB8540_PIN_J11 };
static const unsigned pdmclkdat_d_1_pins[] = { AB8540_PIN_AC2, AB8540_PIN_AB2 };

/*                      */
static const unsigned gpio1_a_1_pins[] = { AB8540_PIN_J16 };
static const unsigned gpio2_a_1_pins[] = { AB8540_PIN_D17 };
static const unsigned gpio3_a_1_pins[] = { AB8540_PIN_C12 };
static const unsigned gpio4_a_1_pins[] = { AB8540_PIN_G12 };
static const unsigned gpio14_a_1_pins[] = { AB8540_PIN_D16 };
static const unsigned gpio15_a_1_pins[] = { AB8540_PIN_F15 };
static const unsigned gpio16_a_1_pins[] = { AB8540_PIN_J8 };
static const unsigned gpio17_a_1_pins[] = { AB8540_PIN_K16 };
static const unsigned gpio18_a_1_pins[] = { AB8540_PIN_G15 };
static const unsigned gpio19_a_1_pins[] = { AB8540_PIN_F17 };
static const unsigned gpio20_a_1_pins[] = { AB8540_PIN_E17 };
static const unsigned gpio27_a_1_pins[] = { AB8540_PIN_AA16 };
static const unsigned gpio28_a_1_pins[] = { AB8540_PIN_W18 };
static const unsigned gpio29_a_1_pins[] = { AB8540_PIN_Y15 };
static const unsigned gpio30_a_1_pins[] = { AB8540_PIN_W16 };
static const unsigned gpio31_a_1_pins[] = { AB8540_PIN_V15 };
static const unsigned gpio32_a_1_pins[] = { AB8540_PIN_W17 };
static const unsigned gpio42_a_1_pins[] = { AB8540_PIN_D12 };
static const unsigned gpio43_a_1_pins[] = { AB8540_PIN_P4 };
static const unsigned gpio44_a_1_pins[] = { AB8540_PIN_AB1 };
static const unsigned gpio45_a_1_pins[] = { AB8540_PIN_K7 };
static const unsigned gpio46_a_1_pins[] = { AB8540_PIN_L7 };
static const unsigned gpio47_a_1_pins[] = { AB8540_PIN_G10 };
static const unsigned gpio48_a_1_pins[] = { AB8540_PIN_K12 };
static const unsigned gpio51_a_1_pins[] = { AB8540_PIN_N8 };
static const unsigned gpio52_a_1_pins[] = { AB8540_PIN_P12 };
static const unsigned gpio53_a_1_pins[] = { AB8540_PIN_K8 };
static const unsigned gpio54_a_1_pins[] = { AB8540_PIN_J11 };
static const unsigned gpio55_a_1_pins[] = { AB8540_PIN_AC2 };
static const unsigned gpio56_a_1_pins[] = { AB8540_PIN_AB2 };

#define AB8540_PIN_GROUP(a, b) { .name = #a, .pins = a##_pins,		\
			.npins = ARRAY_SIZE(a##_pins), .altsetting = b }

static const struct abx500_pingroup ab8540_groups[] = {
	/*                */
	AB8540_PIN_GROUP(sysclkreq2_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(sysclkreq3_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(sysclkreq4_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(sysclkreq6_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(pwmout1_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(pwmout2_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(pwmout3_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(adi1_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(dmic12_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(dmic34_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(dmic56_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(sysclkreq5_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(batremn_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(service_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(pwrctrl0_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(pwrctrl1_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(pwmextvibra1_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(pwmextvibra2_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(gpio1_vbat_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(gpio2_vbat_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(gpio3_vbat_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(gpio4_vbat_d_1, ABX500_DEFAULT),
	AB8540_PIN_GROUP(pdmclkdat_d_1, ABX500_DEFAULT),
	/*                      */
	AB8540_PIN_GROUP(gpio1_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio2_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio3_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio4_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio14_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio15_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio16_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio17_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio18_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio19_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio20_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio27_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio28_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio29_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio30_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio31_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio32_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio42_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio43_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio44_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio45_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio46_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio47_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio48_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio51_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio52_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio53_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio54_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio55_a_1, ABX500_ALT_A),
	AB8540_PIN_GROUP(gpio56_a_1, ABX500_ALT_A),
};

/*                                                                 */
#define AB8540_FUNC_GROUPS(a, b...)	   \
static const char * const a##_groups[] = { b };

AB8540_FUNC_GROUPS(sysclkreq, "sysclkreq2_d_1", "sysclkreq3_d_1",
		"sysclkreq4_d_1", "sysclkreq5_d_1", "sysclkreq6_d_1");
AB8540_FUNC_GROUPS(gpio, "gpio1_a_1", "gpio2_a_1", "gpio3_a_1", "gpio4_a_1",
		"gpio14_a_1", "gpio15_a_1", "gpio16_a_1", "gpio17_a_1",
		"gpio18_a_1", "gpio19_a_1", "gpio20_a_1", "gpio27_a_1",
		"gpio28_a_1", "gpio29_a_1", "gpio30_a_1", "gpio31_a_1",
		"gpio32_a_1", "gpio42_a_1", "gpio43_a_1", "gpio44_a_1",
		"gpio45_a_1", "gpio46_a_1", "gpio47_a_1", "gpio48_a_1",
		"gpio51_a_1", "gpio52_a_1", "gpio53_a_1", "gpio54_a_1",
		"gpio55_a_1", "gpio56_a_1");
AB8540_FUNC_GROUPS(pwmout, "pwmout1_d_1", "pwmout2_d_1", "pwmout3_d_1");
AB8540_FUNC_GROUPS(adi1, "adi1_d_1");
AB8540_FUNC_GROUPS(dmic, "dmic12_d_1", "dmic34_d_1", "dmic56_d_1");
AB8540_FUNC_GROUPS(batremn, "batremn_d_1");
AB8540_FUNC_GROUPS(service, "service_d_1");
AB8540_FUNC_GROUPS(pwrctrl, "pwrctrl0_d_1", "pwrctrl1_d_1");
AB8540_FUNC_GROUPS(pwmextvibra, "pwmextvibra1_d_1", "pwmextvibra2_d_1");
AB8540_FUNC_GROUPS(gpio_vbat, "gpio1_vbat_d_1", "gpio2_vbat_d_1",
		"gpio3_vbat_d_1", "gpio4_vbat_d_1");
AB8540_FUNC_GROUPS(pdm, "pdmclkdat_d_1");

#define FUNCTION(fname)					\
	{						\
		.name = #fname,				\
		.groups = fname##_groups,		\
		.ngroups = ARRAY_SIZE(fname##_groups),	\
	}

static const struct abx500_function ab8540_functions[] = {
	FUNCTION(sysclkreq),
	FUNCTION(gpio),
	FUNCTION(pwmout),
	FUNCTION(adi1),
	FUNCTION(dmic),
	FUNCTION(batremn),
	FUNCTION(service),
	FUNCTION(pwrctrl),
	FUNCTION(pwmextvibra),
	FUNCTION(gpio_vbat),
	FUNCTION(pdm),
};

/*
                                                                            
                                                                          
                                                                   
                                                     
                                                                   
                        
  
 */

static struct
alternate_functions ab8540_alternate_functions[AB8540_GPIO_MAX_NUMBER + 1] = {
	/*                             */
	ALTERNATE_FUNCTIONS(0, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*          */
	ALTERNATE_FUNCTIONS(1,	    0, UNUSED, UNUSED, 0, 0, 0), /*                                 */
	ALTERNATE_FUNCTIONS(2,      1, UNUSED, UNUSED, 0, 0, 0), /*                                 */
	ALTERNATE_FUNCTIONS(3,      2, UNUSED, UNUSED, 0, 0, 0), /*                                */
	ALTERNATE_FUNCTIONS(4,      3, UNUSED, UNUSED, 0, 0, 0), /*                                */
	ALTERNATE_FUNCTIONS(5, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*          */
	ALTERNATE_FUNCTIONS(6, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*          */
	ALTERNATE_FUNCTIONS(7, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*          */
	ALTERNATE_FUNCTIONS(8, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*          */
	/*                             */
	ALTERNATE_FUNCTIONS(9,  UNUSED, UNUSED, UNUSED, 0, 0, 0), /*          */
	ALTERNATE_FUNCTIONS(10, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(11, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(12, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(13, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(14,      5, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(15,      6, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(16,      7, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	/*                             */
	ALTERNATE_FUNCTIONS(17,      0, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(18,      1, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(19,      2, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(20,      3, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(21, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(22, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(23, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(24, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	/*                             */
	ALTERNATE_FUNCTIONS(25, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(26, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(27,      2, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(28,      3, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(29,      4, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(30,      5, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(31,      6, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(32,      7, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	/*                             */
	ALTERNATE_FUNCTIONS(33, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(34, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(35, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(36, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(37, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(38, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(39, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(40, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	/*                           */
	ALTERNATE_FUNCTIONS(41, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(42,      1, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(43,      2, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(44,      3, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(45,      4, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(46,      5, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(47,      6, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(48,      7, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	/*                             */
	ALTERNATE_FUNCTIONS(49, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(50, UNUSED, UNUSED, UNUSED, 0, 0, 0), /*           */
	ALTERNATE_FUNCTIONS(51,      2, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(52,      3, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(53,      4, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(54,      5, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(55,      6, UNUSED, UNUSED, 0, 0, 0), /*                                  */
	ALTERNATE_FUNCTIONS(56,      7, UNUSED, UNUSED, 0, 0, 0), /*                                  */
};

static struct pullud ab8540_pullud = {
	.first_pin = 51,	/*            */
	.last_pin = 54,		/*            */
};

/*
                                                    
                   
                   
 */
static struct abx500_gpio_irq_cluster ab8540_gpio_irq_cluster[] = {
	GPIO_IRQ_CLUSTER(43, 43, AB8540_INT_GPIO43F),
	GPIO_IRQ_CLUSTER(44, 44, AB8540_INT_GPIO44F),
	GPIO_IRQ_CLUSTER(51, 54, AB9540_INT_GPIO51R),
};

static struct abx500_pinctrl_soc_data ab8540_soc = {
	.gpio_ranges = ab8540_pinranges,
	.gpio_num_ranges = ARRAY_SIZE(ab8540_pinranges),
	.pins = ab8540_pins,
	.npins = ARRAY_SIZE(ab8540_pins),
	.functions = ab8540_functions,
	.nfunctions = ARRAY_SIZE(ab8540_functions),
	.groups = ab8540_groups,
	.ngroups = ARRAY_SIZE(ab8540_groups),
	.alternate_functions = ab8540_alternate_functions,
	.pullud = &ab8540_pullud,
	.gpio_irq_cluster = ab8540_gpio_irq_cluster,
	.ngpio_irq_cluster = ARRAY_SIZE(ab8540_gpio_irq_cluster),
	.irq_gpio_rising_offset = AB8540_INT_GPIO43R,
	.irq_gpio_falling_offset = AB8540_INT_GPIO43F,
	.irq_gpio_factor = 2,
};

void
abx500_pinctrl_ab8540_init(struct abx500_pinctrl_soc_data **soc)
{
	*soc = &ab8540_soc;
}
