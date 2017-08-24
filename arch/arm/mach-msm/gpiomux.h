/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#ifndef __ARCH_ARM_MACH_MSM_GPIOMUX_H
#define __ARCH_ARM_MACH_MSM_GPIOMUX_H

#include <linux/bitops.h>
#include <linux/errno.h>
#include <mach/msm_gpiomux.h>

#if defined(CONFIG_MSM_V2_TLMM)
#include "gpiomux-v2.h"
#else
#include "gpiomux-v1.h"
#endif

/* 
                                                                 
  
                                                                   
                                                             
                                                           
                               
  
                                                              
                                                             
  
                                                              
                                         
                                                              
                                             
                                                              
                              
 */
struct msm_gpiomux_config {
	gpiomux_config_t active;
	gpiomux_config_t suspended;
	unsigned         ref;
};

/* 
                                                                 
                                                                        
                                                     
                   
 */
enum {
	GPIOMUX_VALID	 = BIT(sizeof(gpiomux_config_t) * BITS_PER_BYTE - 1),
	GPIOMUX_CTL_MASK = GPIOMUX_VALID,
};

#ifdef CONFIG_MSM_GPIOMUX

/*                                                               
                                                                
                                                            
                                                                 
                            
 */
extern struct msm_gpiomux_config msm_gpiomux_configs[GPIOMUX_NGPIOS];

/*                                                                    
                                            
 */
int msm_gpiomux_write(unsigned gpio,
		      gpiomux_config_t active,
		      gpiomux_config_t suspended);

/*                                                              
                                          
                                             
                                           
  
                                                               
                                
 */
void __msm_gpiomux_write(unsigned gpio, gpiomux_config_t val);
#else
static inline int msm_gpiomux_write(unsigned gpio,
				    gpiomux_config_t active,
				    gpiomux_config_t suspended)
{
	return -ENOSYS;
}
#endif
#endif
