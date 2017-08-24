/*
 * exynos_thermal.h - Samsung EXYNOS TMU (Thermal Management Unit)
 *
 *  Copyright (C) 2011 Samsung Electronics
 *  Donggeun Kim <dg77.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _LINUX_EXYNOS_THERMAL_H
#define _LINUX_EXYNOS_THERMAL_H
#include <linux/cpu_cooling.h>

enum calibration_type {
	TYPE_ONE_POINT_TRIMMING,
	TYPE_TWO_POINT_TRIMMING,
	TYPE_NONE,
};

enum soc_type {
	SOC_ARCH_EXYNOS4210 = 1,
	SOC_ARCH_EXYNOS,
};
/* 
                         
                                                                    
                                                                        
          
                                                                              
  
                                                            
                                       
 */
struct freq_clip_table {
	unsigned int freq_clip_max;
	unsigned int temp_level;
	const struct cpumask *mask_val;
};

/* 
                                  
                                                         
                                                       
                                                              
                                            
                                                   
                         
                                              
                                             
                                                       
                                              
                                             
                                                       
                                              
                                             
                                                       
                                              
                                             
                                                       
                      
                                       
                                       
                      
                                       
                                       
                      
                                       
                                       
                      
                                       
                                       
                                                              
                  
                                                     
                                     
                               
                                              
                                                    
                                    
                                            
                                              
                                                                       
                                                                       
                                                 
  
                                                                     
 */
struct exynos_tmu_platform_data {
	u8 threshold;
	u8 threshold_falling;
	u8 trigger_levels[4];
	bool trigger_level0_en;
	bool trigger_level1_en;
	bool trigger_level2_en;
	bool trigger_level3_en;

	u8 gain;
	u8 reference_voltage;
	u8 noise_cancel_mode;
	u32 efuse_value;

	enum calibration_type cal_type;
	enum soc_type type;
	struct freq_clip_table freq_tab[4];
	unsigned int freq_tab_count;
};
#endif /*                         */
