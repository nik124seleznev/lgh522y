/*
 * Copyright (c) 2008 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * S3C2410 - GPIO lib support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

/*                                                            
                          
 */

#ifndef __MACH_GPIO_H
#define __MACH_GPIO_H __FILE__

#ifdef CONFIG_CPU_S3C244X
#define ARCH_NR_GPIOS	(32 * 9 + CONFIG_S3C24XX_GPIO_EXTRA)
#elif defined(CONFIG_CPU_S3C2443) || defined(CONFIG_CPU_S3C2416)
#define ARCH_NR_GPIOS	(32 * 12 + CONFIG_S3C24XX_GPIO_EXTRA)
#else
#define ARCH_NR_GPIOS	(256 + CONFIG_S3C24XX_GPIO_EXTRA)
#endif

/*
                               
  
                             
                   
                             
                            
                           
                            
                            
                            
                           
                           
                            
                            
                            
                           
                           
 */

/*                 */

#define S3C2410_GPIO_A_NR	(32)
#define S3C2410_GPIO_B_NR	(32)
#define S3C2410_GPIO_C_NR	(32)
#define S3C2410_GPIO_D_NR	(32)
#define S3C2410_GPIO_E_NR	(32)
#define S3C2410_GPIO_F_NR	(32)
#define S3C2410_GPIO_G_NR	(32)
#define S3C2410_GPIO_H_NR	(32)
#define S3C2410_GPIO_J_NR	(32)	/*                 */
#define S3C2410_GPIO_K_NR	(32)	/*                 */
#define S3C2410_GPIO_L_NR	(32)	/*                 */
#define S3C2410_GPIO_M_NR	(32)	/*                */

#if CONFIG_S3C_GPIO_SPACE != 0
#error CONFIG_S3C_GPIO_SPACE cannot be nonzero at the moment
#endif

#define S3C2410_GPIO_NEXT(__gpio) \
	((__gpio##_START) + (__gpio##_NR) + CONFIG_S3C_GPIO_SPACE + 0)

#ifndef __ASSEMBLY__

enum s3c_gpio_number {
	S3C2410_GPIO_A_START = 0,
	S3C2410_GPIO_B_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_A),
	S3C2410_GPIO_C_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_B),
	S3C2410_GPIO_D_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_C),
	S3C2410_GPIO_E_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_D),
	S3C2410_GPIO_F_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_E),
	S3C2410_GPIO_G_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_F),
	S3C2410_GPIO_H_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_G),
	S3C2410_GPIO_J_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_H),
	S3C2410_GPIO_K_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_J),
	S3C2410_GPIO_L_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_K),
	S3C2410_GPIO_M_START = S3C2410_GPIO_NEXT(S3C2410_GPIO_L),
};

#endif /*              */

/*                                  */

#define S3C2410_GPA(_nr)	(S3C2410_GPIO_A_START + (_nr))
#define S3C2410_GPB(_nr)	(S3C2410_GPIO_B_START + (_nr))
#define S3C2410_GPC(_nr)	(S3C2410_GPIO_C_START + (_nr))
#define S3C2410_GPD(_nr)	(S3C2410_GPIO_D_START + (_nr))
#define S3C2410_GPE(_nr)	(S3C2410_GPIO_E_START + (_nr))
#define S3C2410_GPF(_nr)	(S3C2410_GPIO_F_START + (_nr))
#define S3C2410_GPG(_nr)	(S3C2410_GPIO_G_START + (_nr))
#define S3C2410_GPH(_nr)	(S3C2410_GPIO_H_START + (_nr))
#define S3C2410_GPJ(_nr)	(S3C2410_GPIO_J_START + (_nr))
#define S3C2410_GPK(_nr)	(S3C2410_GPIO_K_START + (_nr))
#define S3C2410_GPL(_nr)	(S3C2410_GPIO_L_START + (_nr))
#define S3C2410_GPM(_nr)	(S3C2410_GPIO_M_START + (_nr))

#include <plat/gpio-cfg.h>

#ifdef CONFIG_CPU_S3C244X
#define S3C_GPIO_END	(S3C2410_GPJ(0) + 32)
#elif defined(CONFIG_CPU_S3C2443) || defined(CONFIG_CPU_S3C2416)
#define S3C_GPIO_END	(S3C2410_GPM(0) + 32)
#else
#define S3C_GPIO_END	(S3C2410_GPH(0) + 32)
#endif

#endif /*               */
