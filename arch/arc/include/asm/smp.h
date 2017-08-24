/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_ARC_SMP_H
#define __ASM_ARC_SMP_H

#ifdef CONFIG_SMP

#include <linux/types.h>
#include <linux/init.h>
#include <linux/threads.h>

#define raw_smp_processor_id() (current_thread_info()->cpu)

/*                                                                         */
struct cpumask;

/*
                                                 
 */
extern void arch_send_call_function_single_ipi(int cpu);
extern void arch_send_call_function_ipi_mask(const struct cpumask *mask);

/*
                                                      
 */
extern void __init smp_init_cpus(void);
extern void __init first_lines_of_secondary(void);
extern const char *arc_platform_smp_cpuinfo(void);

/*
                                                         
  
                     
                                                                
 */
extern int smp_ipi_irq_setup(int cpu, int irq);

/*
                                                                      
  
                                                      
                                                                 
                                        
                                                     
 */
struct plat_smp_ops {
	const char 	*info;
	void		(*cpu_kick)(int cpu, unsigned long pc);
	void		(*ipi_send)(void *callmap);
	void		(*ipi_clear)(int cpu, int irq);
};

/*                                                          */
extern struct plat_smp_ops  plat_smp_ops;

#endif  /*            */

/*
                                                       
                                                                               
                                                                             
                                                          
                                                              
  
                                                                             
                  
  
                                                                          
                                                                 
                                                                          
                           
  
                                                                     
                                                
                                                      
                                                            
  
                                                                  
                                                                       
                                                                       
 */
#ifndef CONFIG_ARC_HAS_LLSC

#include <linux/irqflags.h>
#ifdef CONFIG_SMP

#include <asm/spinlock.h>

extern arch_spinlock_t smp_atomic_ops_lock;
extern arch_spinlock_t smp_bitops_lock;

#define atomic_ops_lock(flags)	do {		\
	local_irq_save(flags);			\
	arch_spin_lock(&smp_atomic_ops_lock);	\
} while (0)

#define atomic_ops_unlock(flags) do {		\
	arch_spin_unlock(&smp_atomic_ops_lock);	\
	local_irq_restore(flags);		\
} while (0)

#define bitops_lock(flags)	do {		\
	local_irq_save(flags);			\
	arch_spin_lock(&smp_bitops_lock);	\
} while (0)

#define bitops_unlock(flags) do {		\
	arch_spin_unlock(&smp_bitops_lock);	\
	local_irq_restore(flags);		\
} while (0)

#else /*             */

#define atomic_ops_lock(flags)		local_irq_save(flags)
#define atomic_ops_unlock(flags)	local_irq_restore(flags)

#define bitops_lock(flags)		local_irq_save(flags)
#define bitops_unlock(flags)		local_irq_restore(flags)

#endif /*             */

#endif	/*                      */

#endif
