/*
 * Driver for the NVIDIA Tegra pinmux
 *
 * Copyright (c) 2011, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef __PINMUX_TEGRA_H__
#define __PINMUX_TEGRA_H__

enum tegra_pinconf_param {
	/*                              */
	TEGRA_PINCONF_PARAM_PULL,
	/*                                  */
	TEGRA_PINCONF_PARAM_TRISTATE,
	/*                   */
	TEGRA_PINCONF_PARAM_ENABLE_INPUT,
	/*                   */
	TEGRA_PINCONF_PARAM_OPEN_DRAIN,
	/*                   */
	TEGRA_PINCONF_PARAM_LOCK,
	/*                   */
	TEGRA_PINCONF_PARAM_IORESET,
	/*                   */
	TEGRA_PINCONF_PARAM_RCV_SEL,
	/*                   */
	TEGRA_PINCONF_PARAM_HIGH_SPEED_MODE,
	/*                   */
	TEGRA_PINCONF_PARAM_SCHMITT,
	/*                   */
	TEGRA_PINCONF_PARAM_LOW_POWER_MODE,
	/*                                          */
	TEGRA_PINCONF_PARAM_DRIVE_DOWN_STRENGTH,
	/*                                          */
	TEGRA_PINCONF_PARAM_DRIVE_UP_STRENGTH,
	/*                                          */
	TEGRA_PINCONF_PARAM_SLEW_RATE_FALLING,
	/*                                          */
	TEGRA_PINCONF_PARAM_SLEW_RATE_RISING,
	/*                                          */
	TEGRA_PINCONF_PARAM_DRIVE_TYPE,
};

enum tegra_pinconf_pull {
	TEGRA_PINCONFIG_PULL_NONE,
	TEGRA_PINCONFIG_PULL_DOWN,
	TEGRA_PINCONFIG_PULL_UP,
};

enum tegra_pinconf_tristate {
	TEGRA_PINCONFIG_DRIVEN,
	TEGRA_PINCONFIG_TRISTATE,
};

#define TEGRA_PINCONF_PACK(_param_, _arg_) ((_param_) << 16 | (_arg_))
#define TEGRA_PINCONF_UNPACK_PARAM(_conf_) ((_conf_) >> 16)
#define TEGRA_PINCONF_UNPACK_ARG(_conf_) ((_conf_) & 0xffff)

/* 
                                                     
                                                             
                                                                 
                                              
 */
struct tegra_function {
	const char *name;
	const char * const *groups;
	unsigned ngroups;
};

/* 
                                          
                                                     
                                                   
                                                 
                                                               
                                                             
                                                           
                                                           
                                                         
                                                       
                                                                 
                                                              
                                                             
                                                               
                                                            
                                                           
                                                       
                                                     
                                                   
                                                             
                                                           
                                                         
                                                           
                                                         
                                                       
                                                              
                                                          
                                      
                                                            
                                                             
                                                       
                                                             
                                                          
                                                          
                                                        
                                                        
                                                          
                                                           
                                                           
                                                            
                                                                      
                                                                    
                                                           
  
                                                                           
                                                                       
                                                                           
                                                                           
                                                                        
                                                               
 */
struct tegra_pingroup {
	const char *name;
	const unsigned *pins;
	unsigned npins;
	unsigned funcs[4];
	unsigned func_safe;
	s16 mux_reg;
	s16 pupd_reg;
	s16 tri_reg;
	s16 einput_reg;
	s16 odrain_reg;
	s16 lock_reg;
	s16 ioreset_reg;
	s16 rcv_sel_reg;
	s16 drv_reg;
	s16 drvtype_reg;
	u32 mux_bank:2;
	u32 pupd_bank:2;
	u32 tri_bank:2;
	u32 einput_bank:2;
	u32 odrain_bank:2;
	u32 ioreset_bank:2;
	u32 rcv_sel_bank:2;
	u32 lock_bank:2;
	u32 drv_bank:2;
	u32 drvtype_bank:2;
	u32 mux_bit:5;
	u32 pupd_bit:5;
	u32 tri_bit:5;
	u32 einput_bit:5;
	u32 odrain_bit:5;
	u32 lock_bit:5;
	u32 ioreset_bit:5;
	u32 rcv_sel_bit:5;
	u32 hsm_bit:5;
	u32 schmitt_bit:5;
	u32 lpmd_bit:5;
	u32 drvdn_bit:5;
	u32 drvup_bit:5;
	u32 slwr_bit:5;
	u32 slwf_bit:5;
	u32 drvtype_bit:5;
	u32 drvdn_width:6;
	u32 drvup_width:6;
	u32 slwr_width:6;
	u32 slwf_width:6;
};

/* 
                                                                            
                                                                  
                                                                  
                                                                 
                                                               
              
                                           
                                                                      
                                                     
                                                                    
                                               
 */
struct tegra_pinctrl_soc_data {
	unsigned ngpios;
	const struct pinctrl_pin_desc *pins;
	unsigned npins;
	const struct tegra_function *functions;
	unsigned nfunctions;
	const struct tegra_pingroup *groups;
	unsigned ngroups;
};

int tegra_pinctrl_probe(struct platform_device *pdev,
			const struct tegra_pinctrl_soc_data *soc_data);
int tegra_pinctrl_remove(struct platform_device *pdev);

#endif
