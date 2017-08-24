/*
 * arch/metag/include/asm/clock.h
 *
 * Copyright (C) 2012 Imagination Technologies Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _METAG_CLOCK_H_
#define _METAG_CLOCK_H_

#include <asm/mach/arch.h>

/* 
                                                      
                                                                           
                                                 
                                      
                                        
 */
struct meta_clock_desc {
	unsigned long		(*get_core_freq)(void);
};

extern struct meta_clock_desc _meta_clock;

/*
                                                                               
               
 */
void setup_meta_clocks(struct meta_clock_desc *desc);

/* 
                                                              
  
                                                
 */
static inline unsigned long get_coreclock(void)
{
	/*
                                                                      
                                                                         
                                                                      
            
  */
	return _meta_clock.get_core_freq();
}

#endif /*                 */
