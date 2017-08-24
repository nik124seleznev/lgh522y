/*
 * i2c-mux-gpio interface to platform code
 *
 * Peter Korsgaard <peter.korsgaard@barco.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _LINUX_I2C_MUX_GPIO_H
#define _LINUX_I2C_MUX_GPIO_H

/*                               */
#define I2C_MUX_GPIO_NO_IDLE	((unsigned)-1)

/* 
                                                                               
                                         
                                                                            
                                                                  
           
                                                                     
                                                
                                                                          
                                                
                                                    
                                                
                                                                              
 */
struct i2c_mux_gpio_platform_data {
	int parent;
	int base_nr;
	const unsigned *values;
	int n_values;
	const unsigned *classes;
	char *gpio_chip;
	const unsigned *gpios;
	int n_gpios;
	unsigned idle;
};

#endif /*                       */
