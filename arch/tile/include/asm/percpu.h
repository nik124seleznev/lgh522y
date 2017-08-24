/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

#ifndef _ASM_TILE_PERCPU_H
#define _ASM_TILE_PERCPU_H

register unsigned long my_cpu_offset_reg asm("tp");

#ifdef CONFIG_PREEMPT
/*
                                                               
                                                                      
                                                                   
                                                                  
                                                                       
                                                             
                                                              
 */
static inline unsigned long __my_cpu_offset(void)
{
	unsigned long tp;
	register unsigned long *sp asm("sp");
	asm("move %0, tp" : "=r" (tp) : "U" (*sp));
	return tp;
}
#define __my_cpu_offset __my_cpu_offset()
#else
/*
                                                                    
                                                               
                                                                      
                                                                        
 */
#define __my_cpu_offset my_cpu_offset_reg
#endif

#define set_my_cpu_offset(tp) (my_cpu_offset_reg = (tp))

#include <asm-generic/percpu.h>

#endif /*                    */
