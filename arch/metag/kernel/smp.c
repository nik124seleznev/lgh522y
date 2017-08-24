/*
 *  Copyright (C) 2009,2010,2011 Imagination Technologies Ltd.
 *
 *  Copyright (C) 2002 ARM Limited, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/atomic.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/cache.h>
#include <linux/profile.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/err.h>
#include <linux/cpu.h>
#include <linux/smp.h>
#include <linux/seq_file.h>
#include <linux/irq.h>
#include <linux/bootmem.h>

#include <asm/cacheflush.h>
#include <asm/cachepart.h>
#include <asm/core_reg.h>
#include <asm/cpu.h>
#include <asm/global_lock.h>
#include <asm/metag_mem.h>
#include <asm/mmu_context.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/processor.h>
#include <asm/setup.h>
#include <asm/tlbflush.h>
#include <asm/hwthread.h>
#include <asm/traps.h>

#define SYSC_DCPART(n)	(SYSC_DCPART0 + SYSC_xCPARTn_STRIDE * (n))
#define SYSC_ICPART(n)	(SYSC_ICPART0 + SYSC_xCPARTn_STRIDE * (n))

DECLARE_PER_CPU(PTBI, pTBI);

void *secondary_data_stack;

/*
                                       
                                             
 */
struct ipi_data {
	spinlock_t lock;
	unsigned long ipi_count;
	unsigned long bits;
};

static DEFINE_PER_CPU(struct ipi_data, ipi_data) = {
	.lock	= __SPIN_LOCK_UNLOCKED(ipi_data.lock),
};

static DEFINE_SPINLOCK(boot_lock);

/*
                                                             
 */
int __cpuinit boot_secondary(unsigned int thread, struct task_struct *idle)
{
	u32 val;

	/*
                                                         
                         
  */
	spin_lock(&boot_lock);

	core_reg_write(TXUPC_ID, 0, thread, (unsigned int)secondary_startup);
	core_reg_write(TXUPC_ID, 1, thread, 0);

	/*
                                                                       
                                                                      
  */
	core_reg_write(TXUCT_ID, TXSTATUS_REGNUM, thread, TXSTATUS_PSTAT_BIT);

	/*                             */
	val = core_reg_read(TXUCT_ID, TXPRIVEXT_REGNUM, thread);
	core_reg_write(TXUCT_ID, TXPRIVEXT_REGNUM, thread, val & ~0x80);

	/*
                                                           
                                           
  */
	val = core_reg_read(TXUCT_ID, TXENABLE_REGNUM, thread);
	core_reg_write(TXUCT_ID, TXENABLE_REGNUM, thread, val | 0x1);

	/*
                                                        
                                            
  */
	spin_unlock(&boot_lock);

	return 0;
}

/* 
                                                                    
                                   
                                                          
                                 
                                                              
                                                              
  
                                                                             
                 
 */
static __cpuinit void describe_cachepart_change(unsigned int thread,
						const char *label,
						unsigned int sz,
						unsigned int old,
						unsigned int new)
{
	unsigned int lor1, land1, gor1, gand1;
	unsigned int lor2, land2, gor2, gand2;
	unsigned int diff = old ^ new;

	if (!diff)
		return;

	pr_info("Thread %d: %s partition changed:", thread, label);
	if (diff & (SYSC_xCPARTL_OR_BITS | SYSC_xCPARTL_AND_BITS)) {
		lor1   = (old & SYSC_xCPARTL_OR_BITS)  >> SYSC_xCPARTL_OR_S;
		lor2   = (new & SYSC_xCPARTL_OR_BITS)  >> SYSC_xCPARTL_OR_S;
		land1  = (old & SYSC_xCPARTL_AND_BITS) >> SYSC_xCPARTL_AND_S;
		land2  = (new & SYSC_xCPARTL_AND_BITS) >> SYSC_xCPARTL_AND_S;
		pr_cont(" L:%#x+%#x->%#x+%#x",
			(lor1 * sz) >> 4,
			((land1 + 1) * sz) >> 4,
			(lor2 * sz) >> 4,
			((land2 + 1) * sz) >> 4);
	}
	if (diff & (SYSC_xCPARTG_OR_BITS | SYSC_xCPARTG_AND_BITS)) {
		gor1   = (old & SYSC_xCPARTG_OR_BITS)  >> SYSC_xCPARTG_OR_S;
		gor2   = (new & SYSC_xCPARTG_OR_BITS)  >> SYSC_xCPARTG_OR_S;
		gand1  = (old & SYSC_xCPARTG_AND_BITS) >> SYSC_xCPARTG_AND_S;
		gand2  = (new & SYSC_xCPARTG_AND_BITS) >> SYSC_xCPARTG_AND_S;
		pr_cont(" G:%#x+%#x->%#x+%#x",
			(gor1 * sz) >> 4,
			((gand1 + 1) * sz) >> 4,
			(gor2 * sz) >> 4,
			((gand2 + 1) * sz) >> 4);
	}
	if (diff & SYSC_CWRMODE_BIT)
		pr_cont(" %sWR",
			(new & SYSC_CWRMODE_BIT) ? "+" : "-");
	if (diff & SYSC_DCPART_GCON_BIT)
		pr_cont(" %sGCOn",
			(new & SYSC_DCPART_GCON_BIT) ? "+" : "-");
	pr_cont("\n");
}

/* 
                                                              
                                       
  
                                                                              
              
 */
static __cpuinit void setup_smp_cache(unsigned int thread)
{
	unsigned int this_thread, lflags;
	unsigned int dcsz, dcpart_this, dcpart_old, dcpart_new;
	unsigned int icsz, icpart_old, icpart_new;

	/*
                                                                       
                                                   
  */
	__global_lock2(lflags);
	this_thread = hard_processor_id();
	/*                        */
	dcpart_this = metag_in32(SYSC_DCPART(this_thread));
	dcpart_old = metag_in32(SYSC_DCPART(thread));
	dcpart_new = dcpart_this;
#if PAGE_OFFSET < LINGLOBAL_BASE
	/*
                                                                      
                 
  */
	dcpart_new |= SYSC_DCPART_GCON_BIT;
	metag_out32(dcpart_new, SYSC_DCPART(this_thread));
#endif
	metag_out32(dcpart_new, SYSC_DCPART(thread));
	/*                            */
	icpart_new = metag_in32(SYSC_ICPART(this_thread));
	icpart_old = metag_in32(SYSC_ICPART(thread));
	metag_out32(icpart_new, SYSC_ICPART(thread));
	__global_unlock2(lflags);

	/*
                                                                        
                                          
  */
	dcsz = get_dcache_size();
	icsz = get_dcache_size();
	describe_cachepart_change(this_thread, "dcache", dcsz,
				  dcpart_this, dcpart_new);
	describe_cachepart_change(thread, "dcache", dcsz,
				  dcpart_old, dcpart_new);
	describe_cachepart_change(thread, "icache", icsz,
				  icpart_old, icpart_new);
}

int __cpuinit __cpu_up(unsigned int cpu, struct task_struct *idle)
{
	unsigned int thread = cpu_2_hwthread_id[cpu];
	int ret;

	load_pgd(swapper_pg_dir, thread);

	flush_tlb_all();

	setup_smp_cache(thread);

	/*
                                                                 
  */
	secondary_data_stack = task_stack_page(idle);

	wmb();

	/*
                                     
  */
	ret = boot_secondary(thread, idle);
	if (ret == 0) {
		unsigned long timeout;

		/*
                                              
                                
   */
		timeout = jiffies + HZ;
		while (time_before(jiffies, timeout)) {
			if (cpu_online(cpu))
				break;

			udelay(10);
			barrier();
		}

		if (!cpu_online(cpu))
			ret = -EIO;
	}

	secondary_data_stack = NULL;

	if (ret) {
		pr_crit("CPU%u: processor failed to boot\n", cpu);

		/*
                                                          
   */
	}

	return ret;
}

#ifdef CONFIG_HOTPLUG_CPU
static DECLARE_COMPLETION(cpu_killed);

/*
                                                      
 */
int __cpuexit __cpu_disable(void)
{
	unsigned int cpu = smp_processor_id();
	struct task_struct *p;

	/*
                                                                
                                                                  
  */
	set_cpu_online(cpu, false);

	/*
                                        
  */
	migrate_irqs();

	/*
                                                               
                                          
  */
	flush_cache_all();
	local_flush_tlb_all();

	read_lock(&tasklist_lock);
	for_each_process(p) {
		if (p->mm)
			cpumask_clear_cpu(cpu, mm_cpumask(p->mm));
	}
	read_unlock(&tasklist_lock);

	return 0;
}

/*
                                                                  
                                                          
 */
void __cpuexit __cpu_die(unsigned int cpu)
{
	if (!wait_for_completion_timeout(&cpu_killed, msecs_to_jiffies(1)))
		pr_err("CPU%u: unable to kill\n", cpu);
}

/*
                                                                   
  
                                                                
                                                                
 */
void __cpuexit cpu_die(void)
{
	local_irq_disable();
	idle_task_exit();

	complete(&cpu_killed);

	asm ("XOR	TXENABLE, D0Re0,D0Re0\n");
}
#endif /*                    */

/*
                                                               
                         
 */
void __cpuinit smp_store_cpu_info(unsigned int cpuid)
{
	struct cpuinfo_metag *cpu_info = &per_cpu(cpu_data, cpuid);

	cpu_info->loops_per_jiffy = loops_per_jiffy;
}

/*
                                                               
                                                
 */
asmlinkage void secondary_start_kernel(void)
{
	struct mm_struct *mm = &init_mm;
	unsigned int cpu = smp_processor_id();

	/*
                                                        
                               
  */
	atomic_inc(&mm->mm_users);
	atomic_inc(&mm->mm_count);
	current->active_mm = mm;
	cpumask_set_cpu(cpu, mm_cpumask(mm));
	enter_lazy_tlb(mm, current);
	local_flush_tlb_all();

	/*
                                                           
                                                               
                                                        
           
   
                                                             
                                                             
                                                            
  */
	per_cpu(pTBI, cpu) = __TBI(TBID_ISTAT_BIT);

	if (!per_cpu(pTBI, cpu))
		panic("No TBI found!");

	per_cpu_trap_init(cpu);

	preempt_disable();

	setup_priv();

	/*
                            
  */
	tbi_startup_interrupt(TBID_SIGNUM_TRT);
	notify_cpu_starting(cpu);
	local_irq_enable();

	pr_info("CPU%u (thread %u): Booted secondary processor\n",
		cpu, cpu_2_hwthread_id[cpu]);

	calibrate_delay();
	smp_store_cpu_info(cpu);

	/*
                                                  
  */
	set_cpu_online(cpu, true);

	/*
                             
                          
  */
	check_for_cache_aliasing(cpu);

	/*
                                          
  */
	cpu_startup_entry(CPUHP_ONLINE);
}

void __init smp_cpus_done(unsigned int max_cpus)
{
	int cpu;
	unsigned long bogosum = 0;

	for_each_online_cpu(cpu)
		bogosum += per_cpu(cpu_data, cpu).loops_per_jiffy;

	pr_info("SMP: Total of %d processors activated (%lu.%02lu BogoMIPS).\n",
		num_online_cpus(),
		bogosum / (500000/HZ),
		(bogosum / (5000/HZ)) % 100);
}

void __init smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int cpu = smp_processor_id();

	init_new_context(current, &init_mm);
	current_thread_info()->cpu = cpu;

	smp_store_cpu_info(cpu);
	init_cpu_present(cpu_possible_mask);
}

void __init smp_prepare_boot_cpu(void)
{
	unsigned int cpu = smp_processor_id();

	per_cpu(pTBI, cpu) = __TBI(TBID_ISTAT_BIT);

	if (!per_cpu(pTBI, cpu))
		panic("No TBI found!");
}

static void smp_cross_call(cpumask_t callmap, enum ipi_msg_type msg);

static void send_ipi_message(const struct cpumask *mask, enum ipi_msg_type msg)
{
	unsigned long flags;
	unsigned int cpu;
	cpumask_t map;

	cpumask_clear(&map);
	local_irq_save(flags);

	for_each_cpu(cpu, mask) {
		struct ipi_data *ipi = &per_cpu(ipi_data, cpu);

		spin_lock(&ipi->lock);

		/*
                                                        
                                                    
                                                       
                                                        
                      
    
                                                     
                                                       
                                                       
                                                   
                                                       
                                                 
                                                    
                                                   
    
                                                     
                                       
   */
		if (!(ipi->bits & (1 << msg))) {
			ipi->bits |= 1 << msg;
			cpumask_set_cpu(cpu, &map);
		}

		spin_unlock(&ipi->lock);
	}

	/*
                                                       
  */
	smp_cross_call(map, msg);

	local_irq_restore(flags);
}

void arch_send_call_function_ipi_mask(const struct cpumask *mask)
{
	send_ipi_message(mask, IPI_CALL_FUNC);
}

void arch_send_call_function_single_ipi(int cpu)
{
	send_ipi_message(cpumask_of(cpu), IPI_CALL_FUNC_SINGLE);
}

void show_ipi_list(struct seq_file *p)
{
	unsigned int cpu;

	seq_puts(p, "IPI:");

	for_each_present_cpu(cpu)
		seq_printf(p, " %10lu", per_cpu(ipi_data, cpu).ipi_count);

	seq_putc(p, '\n');
}

static DEFINE_SPINLOCK(stop_lock);

/*
                                              
  
                                                     
                                                      
                        
  
                                         
 */
static int do_IPI(struct pt_regs *regs)
{
	unsigned int cpu = smp_processor_id();
	struct ipi_data *ipi = &per_cpu(ipi_data, cpu);
	struct pt_regs *old_regs = set_irq_regs(regs);
	unsigned long msgs, nextmsg;
	int handled = 0;

	ipi->ipi_count++;

	spin_lock(&ipi->lock);
	msgs = ipi->bits;
	nextmsg = msgs & -msgs;
	ipi->bits &= ~nextmsg;
	spin_unlock(&ipi->lock);

	if (nextmsg) {
		handled = 1;

		nextmsg = ffz(~nextmsg);
		switch (nextmsg) {
		case IPI_RESCHEDULE:
			scheduler_ipi();
			break;

		case IPI_CALL_FUNC:
			generic_smp_call_function_interrupt();
			break;

		case IPI_CALL_FUNC_SINGLE:
			generic_smp_call_function_single_interrupt();
			break;

		default:
			pr_crit("CPU%u: Unknown IPI message 0x%lx\n",
				cpu, nextmsg);
			break;
		}
	}

	set_irq_regs(old_regs);

	return handled;
}

void smp_send_reschedule(int cpu)
{
	send_ipi_message(cpumask_of(cpu), IPI_RESCHEDULE);
}

static void stop_this_cpu(void *data)
{
	unsigned int cpu = smp_processor_id();

	if (system_state == SYSTEM_BOOTING ||
	    system_state == SYSTEM_RUNNING) {
		spin_lock(&stop_lock);
		pr_crit("CPU%u: stopping\n", cpu);
		dump_stack();
		spin_unlock(&stop_lock);
	}

	set_cpu_online(cpu, false);

	local_irq_disable();

	hard_processor_halt(HALT_OK);
}

void smp_send_stop(void)
{
	smp_call_function(stop_this_cpu, NULL, 0);
}

/*
                     
 */
int setup_profiling_timer(unsigned int multiplier)
{
	return -EINVAL;
}

/*
                                               
  
                                                                 
                                                               
            
 */
static void kick_raise_softirq(cpumask_t callmap, unsigned int irq)
{
	int cpu;

	for_each_cpu(cpu, &callmap) {
		unsigned int thread;

		thread = cpu_2_hwthread_id[cpu];

		BUG_ON(thread == BAD_HWTHREAD_ID);

		metag_out32(1, T0KICKI + (thread * TnXKICK_STRIDE));
	}
}

static TBIRES ipi_handler(TBIRES State, int SigNum, int Triggers,
		   int Inst, PTBI pTBI, int *handled)
{
	*handled = do_IPI((struct pt_regs *)State.Sig.pCtx);

	return State;
}

static struct kick_irq_handler ipi_irq = {
	.func = ipi_handler,
};

static void smp_cross_call(cpumask_t callmap, enum ipi_msg_type msg)
{
	kick_raise_softirq(callmap, 1);
}

static inline unsigned int get_core_count(void)
{
	int i;
	unsigned int ret = 0;

	for (i = 0; i < CONFIG_NR_CPUS; i++) {
		if (core_reg_read(TXUCT_ID, TXENABLE_REGNUM, i))
			ret++;
	}

	return ret;
}

/*
                                                                  
                                                        
 */
void __init smp_init_cpus(void)
{
	unsigned int i, ncores = get_core_count();

	/*                                                            */
	for (i = 0; i < NR_CPUS; i++)
		if (cpu_2_hwthread_id[i] == BAD_HWTHREAD_ID) {
			cpu_2_hwthread_id[i] = i;
			hwthread_id_2_cpu[i] = i;
		}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);

	kick_register_func(&ipi_irq);
}
