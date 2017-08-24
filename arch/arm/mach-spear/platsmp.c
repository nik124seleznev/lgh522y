/*
 * arch/arm/mach-spear13xx/platsmp.c
 *
 * based upon linux/arch/arm/mach-realview/platsmp.c
 *
 * Copyright (C) 2012 ST Microelectronics Ltd.
 * Shiraz Hashim <shiraz.hashim@st.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/io.h>
#include <linux/smp.h>
#include <asm/cacheflush.h>
#include <asm/smp_scu.h>
#include <mach/spear.h>
#include "generic.h"

static DEFINE_SPINLOCK(boot_lock);

static void __iomem *scu_base = IOMEM(VA_SCU_BASE);

static void __cpuinit spear13xx_secondary_init(unsigned int cpu)
{
	/*
                                                   
                                             
  */
	pen_release = -1;
	smp_wmb();

	/*
                                     
  */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

static int __cpuinit spear13xx_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	/*
                                                         
                         
  */
	spin_lock(&boot_lock);

	/*
                                                          
                                                          
                                                       
   
                                                           
                                 
  */
	pen_release = cpu;
	flush_cache_all();
	outer_flush_all();

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (pen_release == -1)
			break;

		udelay(10);
	}

	/*
                                                        
                                            
  */
	spin_unlock(&boot_lock);

	return pen_release != -1 ? -ENOSYS : 0;
}

/*
                                                                  
                                                        
 */
static void __init spear13xx_smp_init_cpus(void)
{
	unsigned int i, ncores = scu_get_core_count(scu_base);

	if (ncores > nr_cpu_ids) {
		pr_warn("SMP: %u cores greater than maximum (%u), clipping\n",
			ncores, nr_cpu_ids);
		ncores = nr_cpu_ids;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);
}

static void __init spear13xx_smp_prepare_cpus(unsigned int max_cpus)
{

	scu_enable(scu_base);

	/*
                                                                        
                                                                        
                                                                        
  */
	__raw_writel(virt_to_phys(spear13xx_secondary_startup), SYS_LOCATION);
}

struct smp_operations spear13xx_smp_ops __initdata = {
       .smp_init_cpus		= spear13xx_smp_init_cpus,
       .smp_prepare_cpus	= spear13xx_smp_prepare_cpus,
       .smp_secondary_init	= spear13xx_secondary_init,
       .smp_boot_secondary	= spear13xx_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
       .cpu_die			= spear13xx_cpu_die,
#endif
};
