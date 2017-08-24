/*
 * SPEAr platform shared irq layer header file
 *
 * Copyright (C) 2009-2012 ST Microelectronics
 * Viresh Kumar <viresh.linux@gmail.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __SPEAR_SHIRQ_H
#define __SPEAR_SHIRQ_H

#include <linux/irq.h>
#include <linux/types.h>

/*
                                                       
  
                                  
                                                                             
                                     
                                              
                                   
                                                                               
 */
struct shirq_regs {
	u32 enb_reg;
	u32 reset_to_enb;
	u32 status_reg;
	u32 clear_reg;
	u32 reset_to_clear;
};

/*
                                           
  
                           
                                     
                                                         
                                                          
                                               
                                            
                                                    
 */
struct spear_shirq {
	u32 irq;
	u32 irq_base;
	u32 irq_nr;
	u32 irq_bit_off;
	int invalid_irq;
	void __iomem *base;
	struct shirq_regs regs;
};

int __init spear300_shirq_of_init(struct device_node *np,
		struct device_node *parent);
int __init spear310_shirq_of_init(struct device_node *np,
		struct device_node *parent);
int __init spear320_shirq_of_init(struct device_node *np,
		struct device_node *parent);

#endif /*                 */
