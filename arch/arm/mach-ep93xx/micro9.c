/*
 *  linux/arch/arm/mach-ep93xx/micro9.c
 *
 * Copyright (C) 2006 Contec Steuerungstechnik & Automation GmbH
 *                    Manfred Gruber <m.gruber@tirol.com>
 * Copyright (C) 2009 Contec Steuerungstechnik & Automation GmbH
 *                    Hubert Feurstein <hubert.feurstein@contec.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>

#include <mach/hardware.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include "soc.h"

/*                                                                        
                   
  
                                                    
                                                                    
                                                               
                                                                     
                                                                         */
static unsigned int __init micro9_detect_bootwidth(void)
{
	u32 v;

	/*                                                   */
	v = __raw_readl(EP93XX_SYSCON_SYSCFG);
	if (v & EP93XX_SYSCON_SYSCFG_LCSN7)
		return 4; /*        */
	else
		return 2; /*        */
}

static void __init micro9_register_flash(void)
{
	unsigned int width;

	if (machine_is_micro9())
		width = 4;
	else if (machine_is_micro9m() || machine_is_micro9s())
		width = micro9_detect_bootwidth();
	else
		width = 0;

	if (width)
		ep93xx_register_flash(width, EP93XX_CS1_PHYS_BASE, SZ_64M);
}


/*                                                                        
                  
                                                                         */
static struct ep93xx_eth_data __initdata micro9_eth_data = {
	.phy_id		= 0x1f,
};


static void __init micro9_init_machine(void)
{
	ep93xx_init_devices();
	ep93xx_register_eth(&micro9_eth_data, 1);
	micro9_register_flash();
}


#ifdef CONFIG_MACH_MICRO9H
MACHINE_START(MICRO9, "Contec Micro9-High")
	/*                                                           */
	.atag_offset	= 0x100,
	.map_io		= ep93xx_map_io,
	.init_irq	= ep93xx_init_irq,
	.init_time	= ep93xx_timer_init,
	.init_machine	= micro9_init_machine,
	.init_late	= ep93xx_init_late,
	.restart	= ep93xx_restart,
MACHINE_END
#endif

#ifdef CONFIG_MACH_MICRO9M
MACHINE_START(MICRO9M, "Contec Micro9-Mid")
	/*                                                           */
	.atag_offset	= 0x100,
	.map_io		= ep93xx_map_io,
	.init_irq	= ep93xx_init_irq,
	.init_time	= ep93xx_timer_init,
	.init_machine	= micro9_init_machine,
	.init_late	= ep93xx_init_late,
	.restart	= ep93xx_restart,
MACHINE_END
#endif

#ifdef CONFIG_MACH_MICRO9L
MACHINE_START(MICRO9L, "Contec Micro9-Lite")
	/*                                                           */
	.atag_offset	= 0x100,
	.map_io		= ep93xx_map_io,
	.init_irq	= ep93xx_init_irq,
	.init_time	= ep93xx_timer_init,
	.init_machine	= micro9_init_machine,
	.init_late	= ep93xx_init_late,
	.restart	= ep93xx_restart,
MACHINE_END
#endif

#ifdef CONFIG_MACH_MICRO9S
MACHINE_START(MICRO9S, "Contec Micro9-Slim")
	/*                                                           */
	.atag_offset	= 0x100,
	.map_io		= ep93xx_map_io,
	.init_irq	= ep93xx_init_irq,
	.init_time	= ep93xx_timer_init,
	.init_machine	= micro9_init_machine,
	.init_late	= ep93xx_init_late,
	.restart	= ep93xx_restart,
MACHINE_END
#endif