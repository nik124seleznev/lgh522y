/*
 * Copyright (C) 2014 Linaro Ltd. <ard.biesheuvel@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __LINUX_CPUFEATURE_H
#define __LINUX_CPUFEATURE_H

#ifdef CONFIG_GENERIC_CPU_AUTOPROBE

#include <linux/mod_devicetable.h>
#include <asm/cpufeature.h>

/*
                                           
                                                        
                                                            
                                                             
            
                                                                         
                                                                       
 */

#ifndef CPU_FEATURE_TYPEFMT
#define CPU_FEATURE_TYPEFMT	"%s"
#endif

#ifndef CPU_FEATURE_TYPEVAL
#define CPU_FEATURE_TYPEVAL	ELF_PLATFORM
#endif

/*
                                                                 
               
                                                                        
                                         
                                                                           
                                  
  
                                                                    
                                                    
 */
#define module_cpu_feature_match(x, __init)			\
static struct cpu_feature const cpu_feature_match_ ## x[] =	\
	{ { .feature = cpu_feature(x) }, { } };			\
MODULE_DEVICE_TABLE(cpu, cpu_feature_match_ ## x);		\
								\
static int cpu_feature_match_ ## x ## _init(void)		\
{								\
	if (!cpu_have_feature(cpu_feature(x)))			\
		return -ENODEV;					\
	return __init();					\
}								\
module_init(cpu_feature_match_ ## x ## _init)

#endif
#endif
