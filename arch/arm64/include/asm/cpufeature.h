/*
 * Copyright (C) 2014 Linaro Ltd. <ard.biesheuvel@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_CPUFEATURE_H
#define __ASM_CPUFEATURE_H

#include <asm/hwcap.h>

/*
                                                                              
                                                                            
                                                                            
                                                                         
 */

#define MAX_CPU_FEATURES	(8 * sizeof(elf_hwcap))
#define cpu_feature(x)		ilog2(HWCAP_ ## x)

static inline bool cpu_have_feature(unsigned int num)
{
	return elf_hwcap & (1UL << num);
}

#endif
