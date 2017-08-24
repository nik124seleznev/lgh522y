/*
 * Copyright 2012 Calxeda, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _ASM_ARM_PERCPU_H_
#define _ASM_ARM_PERCPU_H_

/*
                                                                        
                                                      
 */
#if defined(CONFIG_SMP) && !defined(CONFIG_CPU_V6)
static inline void set_my_cpu_offset(unsigned long off)
{
	/*              */
	asm volatile("mcr p15, 0, %0, c13, c0, 4" : : "r" (off) : "memory");
}

static inline unsigned long __my_cpu_offset(void)
{
	unsigned long off;
	register unsigned long *sp asm ("sp");

	/*
                  
                                                                   
                                                              
  */
	asm("mrc p15, 0, %0, c13, c0, 4" : "=r" (off) : "Q" (*sp));

	return off;
}
#define __my_cpu_offset __my_cpu_offset()
#else
#define set_my_cpu_offset(x)	do {} while(0)

#endif /*            */

#include <asm-generic/percpu.h>

#endif /*                    */
