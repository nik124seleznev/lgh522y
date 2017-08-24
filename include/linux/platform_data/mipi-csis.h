/*
 * Copyright (C) 2010 - 2012 Samsung Electronics Co., Ltd.
 *
 * Samsung S5P/Exynos SoC series MIPI CSIS device support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PLAT_SAMSUNG_MIPI_CSIS_H_
#define __PLAT_SAMSUNG_MIPI_CSIS_H_ __FILE__

/* 
                                                                         
                                    
                                                                              
                                          
                                  
 */
struct s5p_platform_mipi_csis {
	unsigned long clk_rate;
	u8 wclk_source;
	u8 lanes;
	u8 hs_settle;
};

/* 
                                                               
                                                    
                                                       
                                  
                                                           
 */
int s5p_csis_phy_enable(int id, bool on);

#endif /*                             */
