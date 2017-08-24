/*
 * IMX pinmux core definitions
 *
 * Copyright (C) 2012 Freescale Semiconductor, Inc.
 * Copyright (C) 2012 Linaro Ltd.
 *
 * Author: Dong Aisheng <dong.aisheng@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __DRIVERS_PINCTRL_IMX_H
#define __DRIVERS_PINCTRL_IMX_H

struct platform_device;

/* 
                                                    
                                             
                                                                      
                                              
                                                                     
                                                      
                                                                       
                             
                                                               
                                       
                                                                             
                                  
                                                                    
                             
 */
struct imx_pin_group {
	const char *name;
	unsigned int *pins;
	unsigned npins;
	unsigned int *mux_mode;
	u16 *input_reg;
	unsigned int *input_val;
	unsigned long *configs;
};

/* 
                                                       
                                            
                                    
                                    
 */
struct imx_pmx_func {
	const char *name;
	const char **groups;
	unsigned num_groups;
};

/* 
                                              
                                
                                    
 */
struct imx_pin_reg {
	u16 mux_reg;
	u16 conf_reg;
};

struct imx_pinctrl_soc_info {
	struct device *dev;
	const struct pinctrl_pin_desc *pins;
	unsigned int npins;
	struct imx_pin_reg *pin_regs;
	struct imx_pin_group *groups;
	unsigned int ngroups;
	struct imx_pmx_func *functions;
	unsigned int nfunctions;
};

#define NO_MUX		0x0
#define NO_PAD		0x0

#define IMX_PINCTRL_PIN(pin) PINCTRL_PIN(pin, #pin)

#define PAD_CTL_MASK(len)	((1 << len) - 1)
#define IMX_MUX_MASK	0x7
#define IOMUXC_CONFIG_SION	(0x1 << 4)

int imx_pinctrl_probe(struct platform_device *pdev,
			struct imx_pinctrl_soc_info *info);
int imx_pinctrl_remove(struct platform_device *pdev);
#endif /*                         */
