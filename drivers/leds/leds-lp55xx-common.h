/*
 * LP55XX Common Driver Header
 *
 * Copyright (C) 2012 Texas Instruments
 *
 * Author: Milo(Woogyom) Kim <milo.kim@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * Derived from leds-lp5521.c, leds-lp5523.c
 */

#ifndef _LEDS_LP55XX_COMMON_H
#define _LEDS_LP55XX_COMMON_H

enum lp55xx_engine_index {
	LP55XX_ENGINE_INVALID,
	LP55XX_ENGINE_1,
	LP55XX_ENGINE_2,
	LP55XX_ENGINE_3,
};

struct lp55xx_led;
struct lp55xx_chip;

/*
                    
                           
                         
 */
struct lp55xx_reg {
	u8 addr;
	u8 val;
};

/*
                              
                                                    
                                                     
                                                   
                                                          
                                                 
                                                 
                                                                  
                                                        
                                                   
 */
struct lp55xx_device_config {
	const struct lp55xx_reg reset;
	const struct lp55xx_reg enable;
	const int max_channel;

	/*                                                          */
	int (*post_init_device) (struct lp55xx_chip *chip);

	/*                            */
	void (*brightness_work_fn)(struct work_struct *work);

	/*                          */
	void (*set_led_current) (struct lp55xx_led *led, u8 led_current);

	/*                                                   */
	void (*firmware_cb)(struct lp55xx_chip *chip);

	/*                                        */
	void (*run_engine) (struct lp55xx_chip *chip, bool start);

	/*                                       */
	const struct attribute_group *dev_attr_group;
};

/*
                     
                                                       
                                       
                                              
                                          
                                                   
                                       
                                                        
 */
struct lp55xx_chip {
	struct i2c_client *cl;
	struct clk *clk;
	struct lp55xx_platform_data *pdata;
	struct mutex lock;	/*                               */
	int num_leds;
	struct lp55xx_device_config *cfg;
	enum lp55xx_engine_index engine_idx;
	const struct firmware *fw;
};

/*
                    
                                    
                                      
                                                         
                                                         
                                                      
                                      
                                          
 */
struct lp55xx_led {
	int chan_nr;
	struct led_classdev cdev;
	u8 led_current;
	u8 max_current;
	struct work_struct brightness_work;
	u8 brightness;
	struct lp55xx_chip *chip;
};

/*                 */
extern int lp55xx_write(struct lp55xx_chip *chip, u8 reg, u8 val);
extern int lp55xx_read(struct lp55xx_chip *chip, u8 reg, u8 *val);
extern int lp55xx_update_bits(struct lp55xx_chip *chip, u8 reg,
			u8 mask, u8 val);

/*                          */
extern bool lp55xx_is_extclk_used(struct lp55xx_chip *chip);

/*                                     */
extern int lp55xx_init_device(struct lp55xx_chip *chip);
extern void lp55xx_deinit_device(struct lp55xx_chip *chip);

/*                                   */
extern int lp55xx_register_leds(struct lp55xx_led *led,
				struct lp55xx_chip *chip);
extern void lp55xx_unregister_leds(struct lp55xx_led *led,
				struct lp55xx_chip *chip);

/*                                    */
extern int lp55xx_register_sysfs(struct lp55xx_chip *chip);
extern void lp55xx_unregister_sysfs(struct lp55xx_chip *chip);

#endif /*                       */
