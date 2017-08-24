/*
 *  External connector (extcon) class generic GPIO driver
 *
 * Copyright (C) 2012 Samsung Electronics
 * Author: MyungJoo Ham <myungjoo.ham@samsung.com>
 *
 * based on switch class driver
 * Copyright (C) 2008 Google, Inc.
 * Author: Mike Lockwood <lockwood@android.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
*/
#ifndef __EXTCON_GPIO_H__
#define __EXTCON_GPIO_H__ __FILE__

#include <linux/extcon.h>

/* 
                                                                             
                                             
                            
                                              
                                                 
                                                                         
                                           
                                                                          
                                           
  
                                                                          
                                                                     
                                    
 */
struct gpio_extcon_platform_data {
	const char *name;
	unsigned gpio;
	unsigned long debounce;
	unsigned long irq_flags;

	/*                                     */
	const char *state_on;
	const char *state_off;
};

#endif /*                   */
