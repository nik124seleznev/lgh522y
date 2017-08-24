/*
 * This file contains Xilinx specific SMP code, used to start up
 * the second processor.
 *
 * Copyright (C) 2011-2013 Xilinx
 *
 * based on linux/arch/arm/mach-realview/platsmp.c
 *
 * Copyright (C) 2002 ARM Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/export.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/io.h>
#include <asm/cacheflush.h>
#include <asm/smp_scu.h>
#include <linux/irqchip/arm-gic.h>
#include "common.h"

/*
                                      
                                                                      
                                                                       
 */
static int ncores;

int __cpuinit zynq_cpun_start(u32 address, int cpu)
{
	u32 trampoline_code_size = &zynq_secondary_trampoline_end -
						&zynq_secondary_trampoline;

	if (cpu > ncores) {
		pr_warn("CPU No. is not available in the system\n");
		return -1;
	}

	/*                                                           */
	/*                                             */
	if (!(address & 3) && (!address || (address >= trampoline_code_size))) {
		/*                                                           */
		static u8 __iomem *zero;
		u32 trampoline_size = &zynq_secondary_trampoline_jump -
						&zynq_secondary_trampoline;

		zynq_slcr_cpu_stop(cpu);

		if (__pa(PAGE_OFFSET)) {
			zero = ioremap(0, trampoline_code_size);
			if (!zero) {
				pr_warn("BOOTUP jump vectors not accessible\n");
				return -1;
			}
		} else {
			zero = (__force u8 __iomem *)PAGE_OFFSET;
		}

		/*
                                                   
                                   
                                 
                         
   */
		memcpy((__force void *)zero, &zynq_secondary_trampoline,
						trampoline_size);
		writel(address, zero + trampoline_size);

		flush_cache_all();
		outer_flush_range(0, trampoline_code_size);
		smp_wmb();

		if (__pa(PAGE_OFFSET))
			iounmap(zero);

		zynq_slcr_cpu_start(cpu);

		return 0;
	}

	pr_warn("Can't start CPU%d: Wrong starting address %x\n", cpu, address);

	return -1;
}
EXPORT_SYMBOL(zynq_cpun_start);

static int __cpuinit zynq_boot_secondary(unsigned int cpu,
						struct task_struct *idle)
{
	return zynq_cpun_start(virt_to_phys(secondary_startup), cpu);
}

/*
                                                                  
                                                        
 */
static void __init zynq_smp_init_cpus(void)
{
	int i;

	ncores = scu_get_core_count(zynq_scu_base);

	for (i = 0; i < ncores && i < CONFIG_NR_CPUS; i++)
		set_cpu_possible(i, true);
}

static void __init zynq_smp_prepare_cpus(unsigned int max_cpus)
{
	int i;

	/*
                                                               
                                           
  */
	for (i = 0; i < max_cpus; i++)
		set_cpu_present(i, true);

	scu_enable(zynq_scu_base);
}

struct smp_operations zynq_smp_ops __initdata = {
	.smp_init_cpus		= zynq_smp_init_cpus,
	.smp_prepare_cpus	= zynq_smp_prepare_cpus,
	.smp_boot_secondary	= zynq_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= zynq_platform_cpu_die,
#endif
};
