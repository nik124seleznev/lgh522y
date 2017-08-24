/*
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_SMP_H
#define __ASM_SMP_H

#include <linux/threads.h>
#include <linux/cpumask.h>
#include <linux/thread_info.h>

#ifndef CONFIG_SMP
# error "<asm/smp.h> included in non-SMP build"
#endif

#define raw_smp_processor_id() (current_thread_info()->cpu)

struct seq_file;

/*
                         
 */
extern void show_ipi_list(struct seq_file *p, int prec);

/*
                                           
 */
extern void handle_IPI(int ipinr, struct pt_regs *regs);

/*
                                                        
 */
extern void smp_init_cpus(void);

/*
                                                                    
 */
extern void set_smp_cross_call(void (*)(const struct cpumask *, unsigned int));

/*
                                                                                
 */
asmlinkage void secondary_start_kernel(void);

/*
                                                
 */
struct secondary_data {
	void *stack;
};
extern struct secondary_data secondary_data;
extern void secondary_entry(void);

extern void arch_send_call_function_single_ipi(int cpu);
extern void arch_send_call_function_ipi_mask(const struct cpumask *mask);

extern int __cpu_disable(void);

extern void __cpu_die(unsigned int cpu);
extern void cpu_die(void);

#endif /*                    */
