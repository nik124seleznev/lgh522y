/* linux/arch/arm/plat-s3c24xx/setup-ts.c
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *                     http://www.samsung.com/
 *
 * Based on S3C24XX setup for i2c device
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/gpio.h>

struct platform_device; /*                         */

#include <mach/hardware.h>

/* 
                                                           
  
                                                                         
                                                                       
                          
 */
void s3c24xx_ts_cfg_gpio(struct platform_device *dev)
{
	s3c_gpio_cfgpin_range(S3C2410_GPG(12), 4, S3C_GPIO_SFN(3));
}
