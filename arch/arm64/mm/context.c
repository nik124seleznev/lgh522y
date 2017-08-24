/*
 * Based on arch/arm/mm/context.c
 *
 * Copyright (C) 2002-2003 Deep Blue Solutions Ltd, all rights reserved.
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

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/percpu.h>

#include <asm/mmu_context.h>
#include <asm/tlbflush.h>
#include <asm/cachetype.h>

#define asid_bits(reg) \
	(((read_cpuid(ID_AA64MMFR0_EL1) & 0xf0) >> 2) + 8)

#define ASID_FIRST_VERSION	(1 << MAX_ASID_BITS)

static DEFINE_RAW_SPINLOCK(cpu_asid_lock);
unsigned int cpu_last_asid = ASID_FIRST_VERSION;

/*
                                                                            
 */
void __init_new_context(struct task_struct *tsk, struct mm_struct *mm)
{
	mm->context.id = 0;
	raw_spin_lock_init(&mm->context.id_lock);
}

static void flush_context(void)
{
	/*                                                */
	cpu_set_reserved_ttbr0();
	flush_tlb_all();
	if (icache_is_aivivt())
		__flush_icache_all();
}

#ifdef CONFIG_SMP

static void set_mm_context(struct mm_struct *mm, unsigned int asid)
{
	unsigned long flags;

	/*
                                                                 
                                                              
                                                          
                                           
  */
	raw_spin_lock_irqsave(&mm->context.id_lock, flags);
	if (likely((mm->context.id ^ cpu_last_asid) >> MAX_ASID_BITS)) {
		/*
                                                         
                    
   */
		mm->context.id = asid;
		cpumask_clear(mm_cpumask(mm));
	}
	raw_spin_unlock_irqrestore(&mm->context.id_lock, flags);

	/*
                                                   
  */
	cpumask_set_cpu(smp_processor_id(), mm_cpumask(mm));
}

/*
                                                                              
                                                            
 */
static void reset_context(void *info)
{
	unsigned int asid;
	unsigned int cpu = smp_processor_id();
	struct mm_struct *mm = current->active_mm;

	smp_rmb();
	asid = cpu_last_asid + cpu;

	flush_context();
	set_mm_context(mm, asid);

	/*                  */
	cpu_switch_mm(mm->pgd, mm);
}

#else

static inline void set_mm_context(struct mm_struct *mm, unsigned int asid)
{
	mm->context.id = asid;
	cpumask_copy(mm_cpumask(mm), cpumask_of(smp_processor_id()));
}

#endif

void __new_context(struct mm_struct *mm)
{
	unsigned int asid;
	unsigned int bits = asid_bits();

	raw_spin_lock(&cpu_asid_lock);
#ifdef CONFIG_SMP
	/*
                                                                       
                                    
  */
	if (!unlikely((mm->context.id ^ cpu_last_asid) >> MAX_ASID_BITS)) {
		cpumask_set_cpu(smp_processor_id(), mm_cpumask(mm));
		raw_spin_unlock(&cpu_asid_lock);
		return;
	}
#endif
	/*
                                                                    
                                                                   
                           
  */
	asid = ++cpu_last_asid;

	/*
                                                                      
                  
  */
	if (unlikely((asid & ((1 << bits) - 1)) == 0)) {
		/*                            */
		cpu_last_asid += (1 << MAX_ASID_BITS) - (1 << bits);
		if (cpu_last_asid == 0)
			cpu_last_asid = ASID_FIRST_VERSION;
		asid = cpu_last_asid + smp_processor_id();
		flush_context();
#ifdef CONFIG_SMP
		smp_wmb();
		smp_call_function(reset_context, NULL, 1);
#endif
		cpu_last_asid += NR_CPUS - 1;
	}

	set_mm_context(mm, asid);
	raw_spin_unlock(&cpu_asid_lock);
}
