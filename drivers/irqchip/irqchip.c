/*
 * Copyright (C) 2012 Thomas Petazzoni
 *
 * Thomas Petazzoni <thomas.petazzoni@free-electrons.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/init.h>
#include <linux/of_irq.h>

#include "irqchip.h"

/*
                                                              
                                                                      
                                                                
                   
 */
static const struct of_device_id
irqchip_of_match_end __used __section(__irqchip_of_end);

extern struct of_device_id __irqchip_begin[];

void __init irqchip_init(void)
{
	of_irq_init(__irqchip_begin);
}
