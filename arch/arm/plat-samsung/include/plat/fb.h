/* arch/arm/plat-samsung/include/plat/fb.h
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * S3C - FB platform data definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __PLAT_S3C_FB_H
#define __PLAT_S3C_FB_H __FILE__

#include <linux/platform_data/video_s3c.h>

/* 
                                                                  
                                                                              
                                                                           
                                                           
 */
extern void s3c_fb_set_platdata(struct s3c_fb_platdata *pd);

/* 
                                                                     
                                                                              
                                                                           
                                                           
 */
extern void s5p_fimd0_set_platdata(struct s3c_fb_platdata *pd);

/* 
                                                                       
  
                                                                     
 */
extern void s3c64xx_fb_gpio_setup_24bpp(void);

/* 
                                                                       
  
                                                                     
 */
extern void s5pc100_fb_gpio_setup_24bpp(void);

/* 
                                                                               
  
                                                                     
 */
extern void s5pv210_fb_gpio_setup_24bpp(void);

/* 
                                                                           
  
                                                                       
 */
extern void exynos4_fimd0_gpio_setup_24bpp(void);

/* 
                                                                               
  
                                                                     
 */
extern void s5p64x0_fb_gpio_setup_24bpp(void);

#endif /*                 */
