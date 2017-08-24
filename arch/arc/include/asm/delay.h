/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Delay routines using pre computed loops_per_jiffy value.
 *
 * vineetg: Feb 2012
 *  -Rewrote in "C" to avoid dealing with availability of H/w MPY
 *  -Also reduced the num of MPY operations from 3 to 2
 *
 * Amit Bhor: Codito Technologies 2004
 */

#ifndef __ASM_ARC_UDELAY_H
#define __ASM_ARC_UDELAY_H

#include <asm/param.h>		/*    */

static inline void __delay(unsigned long loops)
{
	__asm__ __volatile__(
	"1:	sub.f %0, %0, 1	\n"
	"	jpnz 1b		\n"
	: "+r"(loops)
	:
	: "cc");
}

extern void __bad_udelay(void);

/*
                                               
                                         
                         
                                                               
  
                                          
                                                                        
                                                         
                                        
                                                             
                                               
  
                                                    
                                                                        
                                                                       
 */

static inline void __udelay(unsigned long usecs)
{
	unsigned long loops;

	/*                                                 
                                                            
  */
	loops = ((u64) usecs * 4295 * HZ * loops_per_jiffy) >> 32;

	__delay(loops);
}

#define udelay(n) (__builtin_constant_p(n) ? ((n) > 20000 ? __bad_udelay() \
				: __udelay(n)) : __udelay(n))

#endif /*                    */
