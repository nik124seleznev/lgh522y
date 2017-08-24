/*
 * LP55XX Platform Data Header
 *
 * Copyright (C) 2012 Texas Instruments
 *
 * Author: Milo(Woogyom) Kim <milo.kim@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * Derived from leds-lp5521.h, leds-lp5523.h
 */

#ifndef _LEDS_LP55XX_H
#define _LEDS_LP55XX_H

/*                     */
#define LP55XX_CLOCK_AUTO	0
#define LP55XX_CLOCK_INT	1
#define LP55XX_CLOCK_EXT	2

struct lp55xx_led_config {
	const char *name;
	u8 chan_nr;
	u8 led_current; /*                                   */
	u8 max_current;
};

struct lp55xx_predef_pattern {
	const u8 *r;
	const u8 *g;
	const u8 *b;
	u8 size_r;
	u8 size_g;
	u8 size_b;
};

/*
                              
                                                     
                                              
                                            
                                                                           
                                                                           
                                                                            
                                                       
                                                                
                                          
                                                
 */
struct lp55xx_platform_data {

	/*                           */
	struct lp55xx_led_config *led_config;
	u8 num_channels;
	const char *label;

	/*                     */
	u8 clock_mode;

	/*                             */
	int (*setup_resources)(void);
	void (*release_resources)(void);
	void (*enable)(bool state);

	/*                         */
	struct lp55xx_predef_pattern *patterns;
	unsigned int num_patterns;
};

#endif /*                */
