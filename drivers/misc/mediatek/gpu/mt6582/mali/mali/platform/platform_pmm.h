/*
* Copyright (C) 2011-2014 MediaTek Inc.
* 
* This program is free software: you can redistribute it and/or modify it under the terms of the 
* GNU General Public License version 2 as published by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __PLATFORM_PMM_H__
#define __PLATFORM_PMM_H__

struct mali_gpu_utilization_data;

typedef enum mali_power_mode
{
    MALI_POWER_MODE_ON  = 0x1,
    MALI_POWER_MODE_DEEP_SLEEP,
    MALI_POWER_MODE_LIGHT_SLEEP,
    //                   
} mali_power_mode;

/*                                                         
  
                                                                              
  
 */
void mali_pmm_init(void);

/*                                                           
  
                                                                     
  
                                                                                       
 */
void mali_pmm_deinit(void);

/*                                                      
  
                                                         
                                      
 */
void mali_pmm_tri_mode(mali_power_mode mode);

/*                                                                   
  
                                                                 
                       
  
                                                 
 */
void mali_pmm_utilization_handler(struct mali_gpu_utilization_data *data);
unsigned int gpu_get_current_utilization(void);

void mali_platform_power_mode_change(mali_power_mode power_mode);

#endif

