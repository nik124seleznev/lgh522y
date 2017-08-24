/* linux arch/arm/mach-exynos4/hotplug.c
 *
 *  Cloned from linux/arch/arm/mach-realview/hotplug.c
 *
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
#include <linux/io.h>

#include <asm/cacheflush.h>
#include <asm/cp15.h>
#include <asm/smp_plat.h>

#include <mach/regs-pmu.h>
#include <plat/cpu.h>

#include "common.h"

static inline void cpu_enter_lowpower_a9(void)
{
	unsigned int v;

	asm volatile(
	"	mcr	p15, 0, %1, c7, c5, 0\n"
	"	mcr	p15, 0, %1, c7, c10, 4\n"
	/*
                      
  */
	"	mrc	p15, 0, %0, c1, c0, 1\n"
	"	bic	%0, %0, %3\n"
	"	mcr	p15, 0, %0, c1, c0, 1\n"
	"	mrc	p15, 0, %0, c1, c0, 0\n"
	"	bic	%0, %0, %2\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"
	  : "=&r" (v)
	  : "r" (0), "Ir" (CR_C), "Ir" (0x40)
	  : "cc");
}

static inline void cpu_enter_lowpower_a15(void)
{
	unsigned int v;

	asm volatile(
	"	mrc	p15, 0, %0, c1, c0, 0\n"
	"	bic	%0, %0, %1\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"
	  : "=&r" (v)
	  : "Ir" (CR_C)
	  : "cc");

	flush_cache_louis();

	asm volatile(
	/*
                     
 */
	"	mrc	p15, 0, %0, c1, c0, 1\n"
	"	bic	%0, %0, %1\n"
	"	mcr	p15, 0, %0, c1, c0, 1\n"
	: "=&r" (v)
	: "Ir" (0x40)
	: "cc");

	isb();
	dsb();
}

static inline void cpu_leave_lowpower(void)
{
	unsigned int v;

	asm volatile(
	"mrc	p15, 0, %0, c1, c0, 0\n"
	"	orr	%0, %0, %1\n"
	"	mcr	p15, 0, %0, c1, c0, 0\n"
	"	mrc	p15, 0, %0, c1, c0, 1\n"
	"	orr	%0, %0, %2\n"
	"	mcr	p15, 0, %0, c1, c0, 1\n"
	  : "=&r" (v)
	  : "Ir" (CR_C), "Ir" (0x40)
	  : "cc");
}

static inline void platform_do_lowpower(unsigned int cpu, int *spurious)
{
	for (;;) {

		/*                                                */
		if (cpu == 1)
			__raw_writel(0, S5P_ARM_CORE1_CONFIGURATION);

		/*
                   
   */
		asm(".word	0xe320f003\n"
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
		(*spurious)++;
	}
}

/*
                                           
  
                            
 */
void __ref exynos_cpu_die(unsigned int cpu)
{
	int spurious = 0;
	int primary_part = 0;

	/*
                                           
                                                                
                                                               
                                                
  */
	asm("mrc p15, 0, %0, c0, c0, 0" : "=r"(primary_part) : : "cc");
	if ((primary_part & 0xfff0) == 0xc0f0)
		cpu_enter_lowpower_a15();
	else
		cpu_enter_lowpower_a9();

	platform_do_lowpower(cpu, &spurious);

	/*
                                               
                                          
  */
	cpu_leave_lowpower();

	if (spurious)
		pr_warn("CPU%u: %u spurious wakeup calls\n", cpu, spurious);
}