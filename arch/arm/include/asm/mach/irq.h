/*
 *  arch/arm/include/asm/mach/irq.h
 *
 *  Copyright (C) 1995-2000 Russell King.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_ARM_MACH_IRQ_H
#define __ASM_ARM_MACH_IRQ_H

#include <linux/irq.h>

struct seq_file;

/*
                                    
 */
extern void init_FIQ(int);
extern int show_fiq_list(struct seq_file *, int);

/*
                                                                  
 */
#define do_bad_IRQ(irq,desc)				\
do {							\
	raw_spin_lock(&desc->lock);			\
	handle_bad_irq(irq, desc);			\
	raw_spin_unlock(&desc->lock);			\
} while(0)

#endif