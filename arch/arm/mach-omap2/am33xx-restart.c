/*
 * am33xx-restart.c - Code common to all AM33xx machines.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>

#include "common.h"
#include "prm-regbits-33xx.h"
#include "prm33xx.h"

/* 
                                                        
                                                                  
                                                                             
  
                                                         
                                  
 */
void am33xx_restart(char mode, const char *cmd)
{
	/*                                        */

	am33xx_prm_rmw_reg_bits(AM33XX_GLOBAL_WARM_SW_RST_MASK,
				AM33XX_GLOBAL_WARM_SW_RST_MASK,
				AM33XX_PRM_DEVICE_MOD,
				AM33XX_PRM_RSTCTRL_OFFSET);

	/*             */
	(void)am33xx_prm_read_reg(AM33XX_PRM_DEVICE_MOD,
				  AM33XX_PRM_RSTCTRL_OFFSET);
}
