/*
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/smp.h>

#include <asm/smp_plat.h>

#include "common.h"

static inline void cpu_enter_lowpower(void)
{
}

static inline void cpu_leave_lowpower(void)
{
}

static inline void platform_do_lowpower(unsigned int cpu)
{
	/*                                                          */
	for (;;) {
		/*
                   
   */
		asm("wfi"
		    :
		    :
		    : "memory", "cc");

		if (pen_release == cpu_logical_map(cpu)) {
			/*
                                   
    */
			break;
		}

		/*
                                                             
                                                 
    
                                                                 
                                                               
                                                           
   */
		pr_debug("CPU%u: spurious wakeup call\n", cpu);
	}
}

/*
                                           
  
                            
 */
void __ref msm_cpu_die(unsigned int cpu)
{
	/*
                                          
  */
	cpu_enter_lowpower();
	platform_do_lowpower(cpu);

	/*
                                               
                                          
  */
	cpu_leave_lowpower();
}
