/*
 * arch/arm/include/asm/mcpm.h
 *
 * Created by:  Nicolas Pitre, April 2012
 * Copyright:   (C) 2012-2013  Linaro Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef MCPM_H
#define MCPM_H

/*
                                                          
  
                                                                 
                                                                     
                                  
 */
#define MAX_CPUS_PER_CLUSTER	4
#define MAX_NR_CLUSTERS		2

#ifndef __ASSEMBLY__

#include <linux/types.h>
#include <asm/cacheflush.h>

/*
                                                                    
                                                                 
 */
extern void mcpm_entry_point(void);

/*
                                                                         
                                                                          
                                                                       
                    
 */
void mcpm_set_entry_vector(unsigned cpu, unsigned cluster, void *ptr);

/*
                                                                 
 */

/* 
                                                              
  
                                        
                                       
  
                                                                          
                                                                            
                                                              
  
                                                                      
                                                 
  
                                                                           
                                                                      
                                                                   
  
                                                                       
 */
int mcpm_cpu_power_up(unsigned int cpu, unsigned int cluster);

/* 
                                                   
  
                                   
  
                                                                    
                                                   
  
                                                
  
                                                                
                    
 */
void mcpm_cpu_power_down(void);

/* 
                                                                
  
                                                                    
                                                   
  
                                                                         
                                                                          
                                                                         
                          
  
                                                                    
                                                                       
                                 
  
                                                
  
                                                                
                    
 */
void mcpm_cpu_suspend(u64 expected_residency);

/* 
                                                                         
  
                                                                           
                                                                         
                                                                   
  
                                                                       
 */
int mcpm_cpu_powered_up(void);

/*
                                                                         
 */
struct mcpm_platform_ops {
	int (*power_up)(unsigned int cpu, unsigned int cluster);
	void (*power_down)(void);
	void (*suspend)(u64);
	void (*powered_up)(void);
};

/* 
                                                                    
  
                                                
  
                                                                     
 */
int __init mcpm_platform_register(const struct mcpm_platform_ops *ops);

/*                                                                          */

/*
                                                                             
            
 */
struct mcpm_sync_struct {
	/*                       */
	struct {
		s8 cpu __aligned(__CACHE_WRITEBACK_GRANULE);
	} cpus[MAX_CPUS_PER_CLUSTER];

	/*               */
	s8 cluster __aligned(__CACHE_WRITEBACK_GRANULE);

	/*                    */
	s8 inbound __aligned(__CACHE_WRITEBACK_GRANULE);
};

struct sync_struct {
	struct mcpm_sync_struct clusters[MAX_NR_CLUSTERS];
};

extern unsigned long sync_phys;	/*                                */

void __mcpm_cpu_going_down(unsigned int cpu, unsigned int cluster);
void __mcpm_cpu_down(unsigned int cpu, unsigned int cluster);
void __mcpm_outbound_leave_critical(unsigned int cluster, int state);
bool __mcpm_outbound_enter_critical(unsigned int this_cpu, unsigned int cluster);
int __mcpm_cluster_state(unsigned int cluster);

int __init mcpm_sync_init(
	void (*power_up_setup)(unsigned int affinity_level));

void __init mcpm_smp_set_ops(void);

#else

/* 
                                                                           
                                                                              
 */
#include <asm/asm-offsets.h>
#define __CACHE_WRITEBACK_GRANULE CACHE_WRITEBACK_GRANULE

#endif /*                */

/*                                  */
#define CPU_DOWN		0x11
#define CPU_COMING_UP		0x12
#define CPU_UP			0x13
#define CPU_GOING_DOWN		0x14

#define CLUSTER_DOWN		0x21
#define CLUSTER_UP		0x22
#define CLUSTER_GOING_DOWN	0x23

#define INBOUND_NOT_COMING_UP	0x31
#define INBOUND_COMING_UP	0x32

/*
                                                            
                                                                     
 */
#define MCPM_SYNC_CLUSTER_CPUS	0
#define MCPM_SYNC_CPU_SIZE	__CACHE_WRITEBACK_GRANULE
#define MCPM_SYNC_CLUSTER_CLUSTER \
	(MCPM_SYNC_CLUSTER_CPUS + MCPM_SYNC_CPU_SIZE * MAX_CPUS_PER_CLUSTER)
#define MCPM_SYNC_CLUSTER_INBOUND \
	(MCPM_SYNC_CLUSTER_CLUSTER + __CACHE_WRITEBACK_GRANULE)
#define MCPM_SYNC_CLUSTER_SIZE \
	(MCPM_SYNC_CLUSTER_INBOUND + __CACHE_WRITEBACK_GRANULE)

#endif
