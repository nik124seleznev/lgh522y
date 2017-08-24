/*
 * OMAP4 SMP source file. It contains platform specific fucntions
 * needed for the linux smp kernel.
 *
 * Copyright (C) 2009 Texas Instruments, Inc.
 *
 * Author:
 *      Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * Platform file needed for the OMAP4 SMP. This file is based on arm
 * realview smp platform.
 * * Copyright (c) 2002 ARM Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/device.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/irqchip/arm-gic.h>

#include <asm/smp_scu.h>

#include "omap-secure.h"
#include "omap-wakeupgen.h"
#include <asm/cputype.h>

#include "soc.h"
#include "iomap.h"
#include "common.h"
#include "clockdomain.h"
#include "pm.h"

#define CPU_MASK		0xff0ffff0
#define CPU_CORTEX_A9		0x410FC090
#define CPU_CORTEX_A15		0x410FC0F0

#define OMAP5_CORE_COUNT	0x2

u16 pm44xx_errata;

/*                  */
static void __iomem *scu_base;

static DEFINE_SPINLOCK(boot_lock);

void __iomem *omap4_get_scu_base(void)
{
	return scu_base;
}

static void __cpuinit omap4_secondary_init(unsigned int cpu)
{
	/*
                                                                      
                                                                   
                                                                 
                                                                      
                                                  
                                                                 
  */
	if (cpu_is_omap443x() && (omap_type() != OMAP2_DEVICE_TYPE_GP))
		omap_secure_dispatcher(OMAP4_PPA_CPU_ACTRL_SMP_INDEX,
							4, 0, 0, 0, 0, 0);

	/*
                                     
  */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

static int __cpuinit omap4_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	static struct clockdomain *cpu1_clkdm;
	static bool booted;
	static struct powerdomain *cpu1_pwrdm;
	void __iomem *base = omap_get_wakeupgen_base();

	/*
                                                         
                         
  */
	spin_lock(&boot_lock);

	/*
                                                               
                                                                      
                                                       
                                                             
  */
	if (omap_secure_apis_support())
		omap_modify_auxcoreboot0(0x200, 0xfffffdff);
	else
		__raw_writel(0x20, base + OMAP_AUX_CORE_BOOT_0);

	if (!cpu1_clkdm && !cpu1_pwrdm) {
		cpu1_clkdm = clkdm_lookup("mpu1_clkdm");
		cpu1_pwrdm = pwrdm_lookup("cpu1_pwrdm");
	}

	/*
                                                                 
                                                                
                                                                  
                                                                
                                                              
                             
                                                         
             
                                         
  */
	if (booted && cpu1_pwrdm && cpu1_clkdm) {
		/*
                                                         
                                                        
                                              
                           
                               
                                                   
                                                            
                                                                
                            
                                                       
                        
                                                  
                      
   */
		if (IS_PM44XX_ERRATUM(PM_OMAP4_ROM_SMP_BOOT_ERRATUM_GICD)) {
			local_irq_disable();
			gic_dist_disable();
		}

		/*
                                                          
                                  
   */
		clkdm_wakeup(cpu1_clkdm);
		omap_set_pwrdm_state(cpu1_pwrdm, PWRDM_POWER_ON);
		clkdm_allow_idle(cpu1_clkdm);

		if (IS_PM44XX_ERRATUM(PM_OMAP4_ROM_SMP_BOOT_ERRATUM_GICD)) {
			while (gic_dist_disabled()) {
				udelay(1);
				cpu_relax();
			}
			gic_timer_retrigger();
			local_irq_enable();
		}
	} else {
		dsb_sev();
		booted = true;
	}

	arch_send_wakeup_ipi_mask(cpumask_of(cpu));

	/*
                                                        
                                            
  */
	spin_unlock(&boot_lock);

	return 0;
}

/*
                                                                  
                                                        
 */
static void __init omap4_smp_init_cpus(void)
{
	unsigned int i = 0, ncores = 1, cpu_id;

	/*                                                                   */
	cpu_id = read_cpuid_id() & CPU_MASK;
	if (cpu_id == CPU_CORTEX_A9) {
		/*
                                                 
                                                     
   */
		scu_base =  OMAP2_L4_IO_ADDRESS(scu_a9_get_base());
		BUG_ON(!scu_base);
		ncores = scu_get_core_count(scu_base);
	} else if (cpu_id == CPU_CORTEX_A15) {
		ncores = OMAP5_CORE_COUNT;
	}

	/*              */
	if (ncores > nr_cpu_ids) {
		pr_warn("SMP: %u cores greater than maximum (%u), clipping\n",
			ncores, nr_cpu_ids);
		ncores = nr_cpu_ids;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);
}

static void __init omap4_smp_prepare_cpus(unsigned int max_cpus)
{
	void *startup_addr = omap_secondary_startup;
	void __iomem *base = omap_get_wakeupgen_base();

	/*
                                                           
                       
  */
	if (scu_base)
		scu_enable(scu_base);

	if (cpu_is_omap446x()) {
		startup_addr = omap_secondary_startup_4460;
		pm44xx_errata |= PM_OMAP4_ROM_SMP_BOOT_ERRATUM_GICD;
	}

	/*
                                                           
                                                             
                                     
                                                             
  */
	if (omap_secure_apis_support())
		omap_auxcoreboot_addr(virt_to_phys(startup_addr));
	else
		__raw_writel(virt_to_phys(omap5_secondary_startup),
						base + OMAP_AUX_CORE_BOOT_1);

}

struct smp_operations omap4_smp_ops __initdata = {
	.smp_init_cpus		= omap4_smp_init_cpus,
	.smp_prepare_cpus	= omap4_smp_prepare_cpus,
	.smp_secondary_init	= omap4_secondary_init,
	.smp_boot_secondary	= omap4_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= omap4_cpu_die,
#endif
};
