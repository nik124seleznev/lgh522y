/*
 * Copyright (C) 2012, Anish Kumar <anish198519851985@gmail.com>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef GENERIC_ADC_BATTERY_H
#define GENERIC_ADC_BATTERY_H

/* 
                                                                               
                                                                              
                  
                                                 
                                               
                                                                           
                                                                             
                                  
 */
struct gab_platform_data {
	struct power_supply_info battery_info;
	int	(*cal_charge)(long value);
	int	gpio_charge_finished;
	bool	gpio_inverted;
	int     jitter_delay;
};

#endif /*                       */
