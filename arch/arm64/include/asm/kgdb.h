/*
 * AArch64 KGDB support
 *
 * Based on arch/arm/include/kgdb.h
 *
 * Copyright (C) 2013 Cavium Inc.
 * Author: Vijaya Kumar K <vijaya.kumar@caviumnetworks.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ARM_KGDB_H
#define __ARM_KGDB_H

#include <linux/ptrace.h>
#include <asm/debug-monitors.h>

#ifndef	__ASSEMBLY__

static inline void arch_kgdb_breakpoint(void)
{
	asm ("brk %0" : : "I" (KGDB_COMPILED_DBG_BRK_IMM));
}

extern void kgdb_handle_bus_error(void);
extern int kgdb_fault_expected;

#endif /*               */

/*
                                                   
  
                        
                     
                     
                       
                
            
                      
                
             
                          
               
  
 */

#define _GP_REGS		34
#define _FP_REGS		32
#define _EXTRA_REGS		2
/*
                                           
                                           
 */
#define GP_REG_BYTES		(_GP_REGS * 8)
#define DBG_MAX_REG_NUM		(_GP_REGS + _FP_REGS + _EXTRA_REGS)

/*
                                     
                                                         
 */

#define BUFMAX			2048

/*
                                                
                                                                      
                                                       
                                 
 */

#define NUMREGBYTES	((_GP_REGS * 8) + (_FP_REGS * 16) + \
			(_EXTRA_REGS * 4))

#endif /*              */
