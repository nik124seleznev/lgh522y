/*
 * Copyright (c) 2012 Linaro Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef VIRT_H
#define VIRT_H

#include <asm/ptrace.h>

/*
                                                                            
                                                                       
                                         
 */
#define BOOT_CPU_MODE_MISMATCH	PSR_N_BIT

#ifndef __ASSEMBLY__

#ifdef CONFIG_ARM_VIRT_EXT
/*
                                                                   
                                                                           
                                                                             
                                                    
  
                                                                             
 */
extern int __boot_cpu_mode;

void __hyp_set_vectors(unsigned long phys_vector_base);
unsigned long __hyp_get_vectors(void);
#else
#define __boot_cpu_mode	(SVC_MODE)
#endif

#ifndef ZIMAGE
void hyp_mode_check(void);

/*                                      */
static inline bool is_hyp_mode_available(void)
{
	return ((__boot_cpu_mode & MODE_MASK) == HYP_MODE &&
		!(__boot_cpu_mode & BOOT_CPU_MODE_MISMATCH));
}

/*                                                            */
static inline bool is_hyp_mode_mismatched(void)
{
	return !!(__boot_cpu_mode & BOOT_CPU_MODE_MISMATCH);
}
#endif

#endif /*              */

#endif /*          */
