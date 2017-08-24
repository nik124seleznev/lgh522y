/*
 * arch/arm/common/mcpm_entry.c -- entry point for multi-cluster PM
 *
 * Created by:  Nicolas Pitre, March 2012
 * Copyright:   (C) 2012-2013  Linaro Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/irqflags.h>

#include <asm/mcpm.h>
#include <asm/cacheflush.h>
#include <asm/idmap.h>
#include <asm/cputype.h>

extern unsigned long mcpm_entry_vectors[MAX_NR_CLUSTERS][MAX_CPUS_PER_CLUSTER];

void mcpm_set_entry_vector(unsigned cpu, unsigned cluster, void *ptr)
{
	unsigned long val = ptr ? virt_to_phys(ptr) : 0;
	mcpm_entry_vectors[cluster][cpu] = val;
	sync_cache_w(&mcpm_entry_vectors[cluster][cpu]);
}

static const struct mcpm_platform_ops *platform_ops;

int __init mcpm_platform_register(const struct mcpm_platform_ops *ops)
{
	if (platform_ops)
		return -EBUSY;
	platform_ops = ops;
	return 0;
}

int mcpm_cpu_power_up(unsigned int cpu, unsigned int cluster)
{
	if (!platform_ops)
		return -EUNATCH; /*                                   */
	might_sleep();
	return platform_ops->power_up(cpu, cluster);
}

typedef void (*phys_reset_t)(unsigned long);

void mcpm_cpu_power_down(void)
{
	phys_reset_t phys_reset;

	BUG_ON(!platform_ops);
	BUG_ON(!irqs_disabled());

	/*
                                                      
                                                    
  */
	setup_mm_for_reboot();

	platform_ops->power_down();

	/*
                                                                
                                                                
                                                           
                                                               
                                                         
                                                                
                                                               
                                
   
                                                                 
                                                              
                                                               
                                                               
                                                                
                                                                
                                                                  
                                    
  */

	phys_reset = (phys_reset_t)(unsigned long)virt_to_phys(cpu_reset);
	phys_reset(virt_to_phys(mcpm_entry_point));

	/*                       */
	BUG();
}

void mcpm_cpu_suspend(u64 expected_residency)
{
	phys_reset_t phys_reset;

	BUG_ON(!platform_ops);
	BUG_ON(!irqs_disabled());

	/*                                       */
	setup_mm_for_reboot();
	platform_ops->suspend(expected_residency);
	phys_reset = (phys_reset_t)(unsigned long)virt_to_phys(cpu_reset);
	phys_reset(virt_to_phys(mcpm_entry_point));
	BUG();
}

int mcpm_cpu_powered_up(void)
{
	if (!platform_ops)
		return -EUNATCH;
	if (platform_ops->powered_up)
		platform_ops->powered_up();
	return 0;
}

struct sync_struct mcpm_sync;

/*
                                                                    
                                                                          
                                                                 
 */
void __mcpm_cpu_going_down(unsigned int cpu, unsigned int cluster)
{
	mcpm_sync.clusters[cluster].cpus[cpu].cpu = CPU_GOING_DOWN;
	sync_cache_w(&mcpm_sync.clusters[cluster].cpus[cpu].cpu);
}

/*
                                                                        
                                                           
                                                                           
                                                        
                                                    
 */
void __mcpm_cpu_down(unsigned int cpu, unsigned int cluster)
{
	dmb();
	mcpm_sync.clusters[cluster].cpus[cpu].cpu = CPU_DOWN;
	sync_cache_w(&mcpm_sync.clusters[cluster].cpus[cpu].cpu);
	dsb_sev();
}

/*
                                                                               
                                          
                                                                            
                                                                      
                                                                        
                                                                     
 */
void __mcpm_outbound_leave_critical(unsigned int cluster, int state)
{
	dmb();
	mcpm_sync.clusters[cluster].cluster = state;
	sync_cache_w(&mcpm_sync.clusters[cluster].cluster);
	dsb_sev();
}

/*
                                                                               
                                                                           
                                                 
  
           
                                                                             
                                                            
                                                                              
                   
 */
bool __mcpm_outbound_enter_critical(unsigned int cpu, unsigned int cluster)
{
	unsigned int i;
	struct mcpm_sync_struct *c = &mcpm_sync.clusters[cluster];

	/*                                                        */
	c->cluster = CLUSTER_GOING_DOWN;
	sync_cache_w(&c->cluster);

	/*                                                                    */
	sync_cache_r(&c->inbound);
	if (c->inbound == INBOUND_COMING_UP)
		goto abort;

	/*
                                                                       
                                                                     
   
                                                                   
                                                                    
  */
	sync_cache_r(&c->cpus);
	for (i = 0; i < MAX_CPUS_PER_CLUSTER; i++) {
		int cpustate;

		if (i == cpu)
			continue;

		while (1) {
			cpustate = c->cpus[i].cpu;
			if (cpustate != CPU_GOING_DOWN)
				break;

			wfe();
			sync_cache_r(&c->cpus[i].cpu);
		}

		switch (cpustate) {
		case CPU_DOWN:
			continue;

		default:
			goto abort;
		}
	}

	return true;

abort:
	__mcpm_outbound_leave_critical(cluster, CLUSTER_UP);
	return false;
}

int __mcpm_cluster_state(unsigned int cluster)
{
	sync_cache_r(&mcpm_sync.clusters[cluster].cluster);
	return mcpm_sync.clusters[cluster].cluster;
}

extern unsigned long mcpm_power_up_setup_phys;

int __init mcpm_sync_init(
	void (*power_up_setup)(unsigned int affinity_level))
{
	unsigned int i, j, mpidr, this_cluster;

	BUILD_BUG_ON(MCPM_SYNC_CLUSTER_SIZE * MAX_NR_CLUSTERS != sizeof mcpm_sync);
	BUG_ON((unsigned long)&mcpm_sync & (__CACHE_WRITEBACK_GRANULE - 1));

	/*
                                       
                                                           
  */
	for (i = 0; i < MAX_NR_CLUSTERS; i++) {
		mcpm_sync.clusters[i].cluster = CLUSTER_DOWN;
		mcpm_sync.clusters[i].inbound = INBOUND_NOT_COMING_UP;
		for (j = 0; j < MAX_CPUS_PER_CLUSTER; j++)
			mcpm_sync.clusters[i].cpus[j].cpu = CPU_DOWN;
	}
	mpidr = read_cpuid_mpidr();
	this_cluster = MPIDR_AFFINITY_LEVEL(mpidr, 1);
	for_each_online_cpu(i)
		mcpm_sync.clusters[this_cluster].cpus[i].cpu = CPU_UP;
	mcpm_sync.clusters[this_cluster].cluster = CLUSTER_UP;
	sync_cache_w(&mcpm_sync);

	if (power_up_setup) {
		mcpm_power_up_setup_phys = virt_to_phys(power_up_setup);
		sync_cache_w(&mcpm_power_up_setup_phys);
	}

	return 0;
}
