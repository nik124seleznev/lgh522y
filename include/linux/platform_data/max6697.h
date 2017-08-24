/*
 * max6697.h
 *     Copyright (c) 2012 Guenter Roeck <linux@roeck-us.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef MAX6697_H
#define MAX6697_H

#include <linux/types.h>

/*
                     
                              
                                
 */
struct max6697_platform_data {
	bool smbus_timeout_disable;	/*                               */
	bool extended_range_enable;	/*                                   */
	bool beta_compensation;		/*                                 */
	u8 alert_mask;			/*                               */
	u8 over_temperature_mask;	/*                         */
	u8 resistance_cancellation;	/*                        
                             
                               
      */
	u8 ideality_mask;		/*                         */
	u8 ideality_value;		/*                           
                         
      */
};

#endif /*           */
