/*
 * Header containing platform_data structs for omap panels
 *
 * Copyright (C) 2013 Texas Instruments
 * Author: Tomi Valkeinen <tomi.valkeinen@ti.com>
 *	   Archit Taneja <archit@ti.com>
 *
 * Copyright (C) 2011 Texas Instruments
 * Author: Mayuresh Janorkar <mayur@ti.com>
 *
 * Copyright (C) 2010 Canonical Ltd.
 * Author: Bryan Wu <bryan.wu@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __OMAP_PANEL_DATA_H
#define __OMAP_PANEL_DATA_H

struct omap_dss_device;

/* 
                                                                  
                    
                                                            
                                                              
                                                 
                                       
                                                     
 */
struct panel_generic_dpi_data {
	const char *name;
	int (*platform_enable)(struct omap_dss_device *dssdev);
	void (*platform_disable)(struct omap_dss_device *dssdev);

	int num_gpios;
	int gpios[10];
	bool gpio_invert[10];
};

/* 
                                                                
 */
struct panel_n8x0_data {
	int (*platform_enable)(struct omap_dss_device *dssdev);
	void (*platform_disable)(struct omap_dss_device *dssdev);
	int panel_reset;
	int ctrl_pwrdown;
};

/* 
                                                                          
                    
                               
                                 
                                                           
                                                                      
                                                                          
                                     
 */

struct nokia_dsi_panel_data {
	const char *name;

	int reset_gpio;

	bool use_ext_te;
	int ext_te_gpio;

	unsigned esd_interval;
	unsigned ulps_timeout;

	bool use_dsi_backlight;

	struct omap_dsi_pin_config pin_config;
};

/* 
                                                                      
                                                      
 */
struct picodlp_panel_data {
	int picodlp_adapter_id;
	int emu_done_gpio;
	int pwrgood_gpio;
};

/* 
                                                                       
                                         
                                                                    
 */
struct tfp410_platform_data {
	int i2c_bus_num;
	int power_down_gpio;
};

/* 
                                           
                           
                              
                                               
                                                        
                                                      
 */
struct panel_sharp_ls037v7dw01_data {
	int resb_gpio;
	int ini_gpio;
	int mo_gpio;
	int lr_gpio;
	int ud_gpio;
};

/* 
                                            
                            
 */
struct panel_acx565akm_data {
	int reset_gpio;
};

/* 
                                             
                          
                                                  
 */
struct panel_nec_nl8048_data {
	int res_gpio;
	int qvga_gpio;
};

/* 
                                            
                             
 */
struct panel_tpo_td043_data {
	int nreset_gpio;
};

#endif /*                     */
