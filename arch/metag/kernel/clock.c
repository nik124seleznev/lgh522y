/*
 * arch/metag/kernel/clock.c
 *
 * Copyright (C) 2012 Imagination Technologies Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/io.h>

#include <asm/param.h>
#include <asm/clock.h>

struct meta_clock_desc _meta_clock;

/*                                         */
static unsigned long get_core_freq_default(void)
{
#ifdef CONFIG_METAG_META21
	/*
                                                                      
                                                 
  */
	return (metag_in32(EXPAND_TIMER_DIV) + 1) * 1000000;
#else
	/*
                                                                       
                                                            
  */
	return (loops_per_jiffy * HZ * 5) >> 1;
#endif
}

/* 
                                               
                                                                  
  
                                   
 */
void __init setup_meta_clocks(struct meta_clock_desc *desc)
{
	/*                */
	if (desc)
		_meta_clock = *desc;

	/*                        */
	if (!_meta_clock.get_core_freq)
		_meta_clock.get_core_freq = get_core_freq_default;
}

