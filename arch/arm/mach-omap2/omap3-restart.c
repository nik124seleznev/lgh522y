/*
 * omap3-restart.c - Code common to all OMAP3xxx machines.
 *
 * Copyright (C) 2009, 2012 Texas Instruments
 * Copyright (C) 2010 Nokia Corporation
 * Tony Lindgren <tony@atomide.com>
 * Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/init.h>

#include "iomap.h"
#include "common.h"
#include "control.h"
#include "prm3xxx.h"

/*                                */

/* 
                                                           
                                                                  
                                                                             
  
                                                         
                                  
 */
void omap3xxx_restart(char mode, const char *cmd)
{
	omap3_ctrl_write_boot_mode((cmd ? (u8)*cmd : 0));
	omap3xxx_prm_dpll3_reset(); /*               */
	while (1);
}
