/*
 * Copyright (C) 2002 ARM Ltd.
 * Copyright (C) 2008 STMicroelctronics.
 * Copyright (C) 2009 ST-Ericsson.
 * Author: Srinidhi Kasagar <srinidhi.kasagar@stericsson.com>
 *
 * This file is based on arm realview platform
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/smp.h>
#include <linux/io.h>

#include <asm/cacheflush.h>
#include <asm/smp_plat.h>
#include <asm/smp_scu.h>

#include "setup.h"

#include "db8500-regs.h"
#include "id.h"

/*                                                            */
extern void u8500_secondary_startup(void);

/*
                                                                     
                                                                      
                                                                    
 */
static void write_pen_release(int val)
{
	pen_release = val;
	smp_wmb();
	__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
	outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));
}

static void __iomem *scu_base_addr(void)
{
	if (cpu_is_u8500_family() || cpu_is_ux540_family())
		return __io_address(U8500_SCU_BASE);
	else
		ux500_unknown_soc();

	return NULL;
}

static DEFINE_SPINLOCK(boot_lock);

static void __cpuinit ux500_secondary_init(unsigned int cpu)
{
	/*
                                                   
                                             
  */
	write_pen_release(-1);

	/*
                                     
  */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

static int __cpuinit ux500_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	/*
                                                         
                         
  */
	spin_lock(&boot_lock);

	/*
                                                          
                                                          
                                                       
  */
	write_pen_release(cpu_logical_map(cpu));

	arch_send_wakeup_ipi_mask(cpumask_of(cpu));

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		if (pen_release == -1)
			break;
	}

	/*
                                                        
                                            
  */
	spin_unlock(&boot_lock);

	return pen_release != -1 ? -ENOSYS : 0;
}

static void __init wakeup_secondary(void)
{
	void __iomem *backupram;

	if (cpu_is_u8500_family() || cpu_is_ux540_family())
		backupram = __io_address(U8500_BACKUPRAM0_BASE);
	else
		ux500_unknown_soc();

	/*
                                                                       
                                                                   
                                                                     
                                                                  
  */
#define UX500_CPU1_JUMPADDR_OFFSET 0x1FF4
	__raw_writel(virt_to_phys(u8500_secondary_startup),
		     backupram + UX500_CPU1_JUMPADDR_OFFSET);

#define UX500_CPU1_WAKEMAGIC_OFFSET 0x1FF0
	__raw_writel(0xA1FEED01,
		     backupram + UX500_CPU1_WAKEMAGIC_OFFSET);

	/*                                   */
	mb();
}

/*
                                                                  
                                                        
 */
static void __init ux500_smp_init_cpus(void)
{
	void __iomem *scu_base = scu_base_addr();
	unsigned int i, ncores;

	ncores = scu_base ? scu_get_core_count(scu_base) : 1;

	/*              */
	if (ncores > nr_cpu_ids) {
		pr_warn("SMP: %u cores greater than maximum (%u), clipping\n",
			ncores, nr_cpu_ids);
		ncores = nr_cpu_ids;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);
}

static void __init ux500_smp_prepare_cpus(unsigned int max_cpus)
{

	scu_enable(scu_base_addr());
	wakeup_secondary();
}

struct smp_operations ux500_smp_ops __initdata = {
	.smp_init_cpus		= ux500_smp_init_cpus,
	.smp_prepare_cpus	= ux500_smp_prepare_cpus,
	.smp_secondary_init	= ux500_secondary_init,
	.smp_boot_secondary	= ux500_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= ux500_cpu_die,
#endif
};
