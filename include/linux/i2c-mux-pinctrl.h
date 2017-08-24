/*
 * i2c-mux-pinctrl platform data
 *
 * Copyright (c) 2012, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LINUX_I2C_MUX_PINCTRL_H
#define _LINUX_I2C_MUX_PINCTRL_H

/* 
                                                                           
                                         
                                                                          
                                                                     
                  
                                                                               
                                                                              
                                                                       
            
 */
struct i2c_mux_pinctrl_platform_data {
	int parent_bus_num;
	int base_bus_num;
	int bus_count;
	const char **pinctrl_states;
	const char *pinctrl_state_idle;
};

#endif
