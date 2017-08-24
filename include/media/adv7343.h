/*
 * ADV7343 header file
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed .as is. WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef ADV7343_H
#define ADV7343_H

#define ADV7343_COMPOSITE_ID	(0)
#define ADV7343_COMPONENT_ID	(1)
#define ADV7343_SVIDEO_ID	(2)

/* 
                                                 
                                                                            
                                                              
                                                         
                                                                           
                                                                
                  
                              
                              
                              
                              
                              
                              
  
                                                                              
                                                
  
                                                                               
 */
struct adv7343_power_mode {
	bool sleep_mode;
	bool pll_control;
	bool dac_1;
	bool dac_2;
	bool dac_3;
	bool dac_4;
	bool dac_5;
	bool dac_6;
};

/* 
                                                           
                                           
                                           
 */
struct adv7343_sd_config {
	/*                              */
	bool sd_dac_out1;
	bool sd_dac_out2;
};

/* 
                                                                            
                                              
                                     
 */
struct adv7343_platform_data {
	struct adv7343_power_mode mode_config;
	struct adv7343_sd_config sd_config;
};

#endif				/*                          */