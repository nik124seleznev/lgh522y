/*
 * ARC FPGA Platform IRQ hookups
 *
 * Copyright (C) 2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/interrupt.h>
#include <plat/irq.h>

void __init plat_fpga_init_IRQ(void)
{
	/*
                                                                     
                                                                       
                                                                      
                              
  */
#ifdef CONFIG_SMP
	arch_unmask_irq(UART0_IRQ);
#endif
}
