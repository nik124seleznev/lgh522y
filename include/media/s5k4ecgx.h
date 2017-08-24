/*
 * S5K4ECGX image sensor header file
 *
 * Copyright (C) 2012, Linaro
 * Copyright (C) 2012, Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef S5K4ECGX_H
#define S5K4ECGX_H

/* 
                                                          
                      
                                              
 */
struct s5k4ecgx_gpio {
	int gpio;
	int level;
};

/* 
                                                                
                                       
                                      
 */

struct s5k4ecgx_platform_data {
	struct s5k4ecgx_gpio gpio_reset;
	struct s5k4ecgx_gpio gpio_stby;
};

#endif /*            */
