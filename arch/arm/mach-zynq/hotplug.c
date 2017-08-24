/*
 * Copyright (C) 2012-2013 Xilinx
 *
 * based on linux/arch/arm/mach-realview/hotplug.c
 *
 * Copyright (C) 2002 ARM Ltd.
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/smp.h>

#include <asm/cacheflush.h>
#include <asm/cp15.h>
#include "common.h"

static inline void zynq_cpu_enter_lowpower(void)
{
	unsigned int v;

	flush_cache_all();
	asm volatile(
	"	mcr	p15, 0, %1, c7, c5, 0\n"
	"	dsb\n"
	/*
                      
  */
	"	mrc	p15, 0, %0, c1, c0, 1\n"
	"	bic	%0, %0, #0x40\n"
	"	mcr	p15, 0, %0, c1, c0, 1\n"
	"	mrc	p15, 0, %0, c1, c0, 0\n"
	"	bic	%0, %0, %2\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"
	  : "=&r" (v)
	  : "r" (0), "Ir" (CR_C)
	  : "cc");
}

static inline void zynq_cpu_leave_lowpower(void)
{
	unsigned int v;

	asm volatile(
	"	mrc	p15, 0, %0, c1, c0, 0\n"
	"	orr	%0, %0, %1\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"
	"	mrc	p15, 0, %0, c1, c0, 1\n"
	"	orr	%0, %0, #0x40\n"
	"	mcr	p15, 0, %0, c1, c0, 1\n"
	  : "=&r" (v)
	  : "Ir" (CR_C)
	  : "cc");
}

static inline void zynq_platform_do_lowpower(unsigned int cpu, int *spurious)
{
	/*
                                                               
                                                                   
                                              
  */
	for (;;) {
		dsb();
		wfi();

		/*
                                                             
                                                 
    
                                                             
                    
   */
		(*spurious)++;
	}
}

/*
                                           
  
                            
 */
void zynq_platform_cpu_die(unsigned int cpu)
{
	int spurious = 0;

	/*
                                          
  */
	zynq_cpu_enter_lowpower();
	zynq_platform_do_lowpower(cpu, &spurious);

	/*
                                               
                                          
  */
	zynq_cpu_leave_lowpower();

	if (spurious)
		pr_warn("CPU%u: %u spurious wakeup calls\n", cpu, spurious);
}
