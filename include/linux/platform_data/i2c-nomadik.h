/*
 * Copyright (C) 2009 ST-Ericsson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 */
#ifndef __PDATA_I2C_NOMADIK_H
#define __PDATA_I2C_NOMADIK_H

enum i2c_freq_mode {
	I2C_FREQ_MODE_STANDARD,		/*                */
	I2C_FREQ_MODE_FAST,		/*                */
	I2C_FREQ_MODE_HIGH_SPEED,	/*                */
	I2C_FREQ_MODE_FAST_PLUS,	/*              */
};

/* 
                                                                       
                                                    
                                      
                                                      
                                                       
                                                  
                                   
                                   
                                      
                   
 */
struct nmk_i2c_controller {
	u32             clk_freq;
	unsigned short	slsu;
	unsigned char	tft;
	unsigned char	rft;
	int timeout;
	enum i2c_freq_mode	sm;
};

#endif	/*                       */
