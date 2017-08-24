/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2012 MIPS Technologies, Inc.  All rights reserved.
 */
#include <linux/init.h>
#include <linux/of_platform.h>
#include <linux/of_fdt.h>
#include <linux/bootmem.h>

#include <asm/mips-boards/generic.h>

const char *get_system_type(void)
{
	return "MIPS SEAD3";
}

void __init plat_mem_setup(void)
{
	/*
                                                                  
                                            
  */
	__dt_setup_arch(&__dtb_start);
}

void __init device_tree_init(void)
{
	unsigned long base, size;

	if (!initial_boot_params)
		return;

	base = virt_to_phys((void *)initial_boot_params);
	size = be32_to_cpu(initial_boot_params->totalsize);

	/*                                                 */
	reserve_bootmem(base, size, BOOTMEM_DEFAULT);

	unflatten_device_tree();
}
