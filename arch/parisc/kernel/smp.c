/*
** SMP Support
**
** Copyright (C) 1999 Walt Drummond <drummond@valinux.com>
** Copyright (C) 1999 David Mosberger-Tang <davidm@hpl.hp.com>
** Copyright (C) 2001,2004 Grant Grundler <grundler@parisc-linux.org>
** 
** Lots of stuff stolen from arch/alpha/kernel/smp.c
** ...and then parisc stole from arch/ia64/kernel/smp.c. Thanks David! :^)
**
** Thanks to John Curry and Ullas Ponnadi. I learned a lot from their work.
** -grant (1/12/2001)
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
*/
#include <linux/types.h>
#include <linux/spinlock.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/smp.h>
#include <linux/kernel_stat.h>
#include <linux/mm.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/ftrace.h>
#include <linux/cpu.h>

#include <linux/atomic.h>
#include <asm/current.h>
#include <asm/delay.h>
#include <asm/tlbflush.h>

#include <asm/io.h>
#include <asm/irq.h>		/*                                */
#include <asm/mmu_context.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/processor.h>
#include <asm/ptrace.h>
#include <asm/unistd.h>
#include <asm/cacheflush.h>

#undef DEBUG_SMP
#ifdef DEBUG_SMP
static int smp_debug_lvl = 0;
#define smp_debug(lvl, printargs...)		\
		if (lvl >= smp_debug_lvl)	\
			printk(printargs);
#else
#define smp_debug(lvl, ...)	do { } while(0)
#endif /*           */

volatile struct task_struct *smp_init_current_idle_task;

/*                            */
static volatile int cpu_now_booting __cpuinitdata;

static int parisc_max_cpus __cpuinitdata = 1;

static DEFINE_PER_CPU(spinlock_t, ipi_lock);

enum ipi_message_type {
	IPI_NOP=0,
	IPI_RESCHEDULE=1,
	IPI_CALL_FUNC,
	IPI_CALL_FUNC_SINGLE,
	IPI_CPU_START,
	IPI_CPU_STOP,
	IPI_CPU_TEST
};


/*                                                                   */

#undef PER_CPU_IRQ_REGION
#ifdef PER_CPU_IRQ_REGION
/*                            
                                                    
                                                
*/
static void
ipi_init(int cpuid)
{
#error verify IRQ_OFFSET(IPI_IRQ) is ipi_interrupt() in new IRQ region

	if(cpu_online(cpuid) )
	{
		switch_to_idle_task(current);
	}

	return;
}
#endif


/*
                                            
  
*/
static void
halt_processor(void) 
{
	/*                                                   */
	/*                                                    */
	set_cpu_online(smp_processor_id(), false);
	local_irq_disable();
	for (;;)
		;
}


irqreturn_t __irq_entry
ipi_interrupt(int irq, void *dev_id) 
{
	int this_cpu = smp_processor_id();
	struct cpuinfo_parisc *p = &per_cpu(cpu_data, this_cpu);
	unsigned long ops;
	unsigned long flags;

	/*                                                        */
	inc_irq_stat(irq_call_count);

	mb();	/*                                  */

	for (;;) {
		spinlock_t *lock = &per_cpu(ipi_lock, this_cpu);
		spin_lock_irqsave(lock, flags);
		ops = p->pending_ipi;
		p->pending_ipi = 0;
		spin_unlock_irqrestore(lock, flags);

		mb(); /*                                     */

		if (!ops)
		    break;

		while (ops) {
			unsigned long which = ffz(~ops);

			ops &= ~(1 << which);

			switch (which) {
			case IPI_NOP:
				smp_debug(100, KERN_DEBUG "CPU%d IPI_NOP\n", this_cpu);
				break;
				
			case IPI_RESCHEDULE:
				smp_debug(100, KERN_DEBUG "CPU%d IPI_RESCHEDULE\n", this_cpu);
				inc_irq_stat(irq_resched_count);
				scheduler_ipi();
				break;

			case IPI_CALL_FUNC:
				smp_debug(100, KERN_DEBUG "CPU%d IPI_CALL_FUNC\n", this_cpu);
				generic_smp_call_function_interrupt();
				break;

			case IPI_CALL_FUNC_SINGLE:
				smp_debug(100, KERN_DEBUG "CPU%d IPI_CALL_FUNC_SINGLE\n", this_cpu);
				generic_smp_call_function_single_interrupt();
				break;

			case IPI_CPU_START:
				smp_debug(100, KERN_DEBUG "CPU%d IPI_CPU_START\n", this_cpu);
				break;

			case IPI_CPU_STOP:
				smp_debug(100, KERN_DEBUG "CPU%d IPI_CPU_STOP\n", this_cpu);
				halt_processor();
				break;

			case IPI_CPU_TEST:
				smp_debug(100, KERN_DEBUG "CPU%d is alive!\n", this_cpu);
				break;

			default:
				printk(KERN_CRIT "Unknown IPI num on CPU%d: %lu\n",
					this_cpu, which);
				return IRQ_NONE;
			} /*        */
		/*                               */
		local_irq_enable();
		local_irq_disable();
		} /*             */
	}
	return IRQ_HANDLED;
}


static inline void
ipi_send(int cpu, enum ipi_message_type op)
{
	struct cpuinfo_parisc *p = &per_cpu(cpu_data, cpu);
	spinlock_t *lock = &per_cpu(ipi_lock, cpu);
	unsigned long flags;

	spin_lock_irqsave(lock, flags);
	p->pending_ipi |= 1 << op;
	gsc_writel(IPI_IRQ - CPU_IRQ_BASE, p->hpa);
	spin_unlock_irqrestore(lock, flags);
}

static void
send_IPI_mask(const struct cpumask *mask, enum ipi_message_type op)
{
	int cpu;

	for_each_cpu(cpu, mask)
		ipi_send(cpu, op);
}

static inline void
send_IPI_single(int dest_cpu, enum ipi_message_type op)
{
	BUG_ON(dest_cpu == NO_PROC_ID);

	ipi_send(dest_cpu, op);
}

static inline void
send_IPI_allbutself(enum ipi_message_type op)
{
	int i;
	
	for_each_online_cpu(i) {
		if (i != smp_processor_id())
			send_IPI_single(i, op);
	}
}


inline void 
smp_send_stop(void)	{ send_IPI_allbutself(IPI_CPU_STOP); }

static inline void
smp_send_start(void)	{ send_IPI_allbutself(IPI_CPU_START); }

void 
smp_send_reschedule(int cpu) { send_IPI_single(cpu, IPI_RESCHEDULE); }

void
smp_send_all_nop(void)
{
	send_IPI_allbutself(IPI_NOP);
}

void arch_send_call_function_ipi_mask(const struct cpumask *mask)
{
	send_IPI_mask(mask, IPI_CALL_FUNC);
}

void arch_send_call_function_single_ipi(int cpu)
{
	send_IPI_single(cpu, IPI_CALL_FUNC_SINGLE);
}

/*
                                                                      
 */
static void __init
smp_cpu_init(int cpunum)
{
	extern int init_per_cpu(int);  /*                                */
	extern void init_IRQ(void);    /*                          */
	extern void start_cpu_itimer(void); /*                           */

	/*                                                 */
	(void) init_per_cpu(cpunum);

	disable_sr_hashing();

	mb();

	/*                                         */
	if (cpu_online(cpunum))	{
		extern void machine_halt(void); /*                          */

		printk(KERN_CRIT "CPU#%d already initialized!\n", cpunum);
		machine_halt();
	}

	notify_cpu_starting(cpunum);

	set_cpu_online(cpunum, true);

	/*                                       */
	atomic_inc(&init_mm.mm_count);
	current->active_mm = &init_mm;
	BUG_ON(current->mm);
	enter_lazy_tlb(&init_mm, current);

	init_IRQ();   /*                                          */
	start_cpu_itimer();
}


/*
                                                                     
                                                                              
 */
void __init smp_callin(void)
{
	int slave_id = cpu_now_booting;

	smp_cpu_init(slave_id);
	preempt_disable();

	flush_cache_all_local(); /*                        */
	flush_tlb_all_local(NULL);

	local_irq_enable();  /*                                    */

	cpu_startup_entry(CPUHP_ONLINE);

	/*            */
	panic("smp_callin() AAAAaaaaahhhh....\n");
}

/*
                        
 */
int __cpuinit smp_boot_one_cpu(int cpuid, struct task_struct *idle)
{
	const struct cpuinfo_parisc *p = &per_cpu(cpu_data, cpuid);
	long timeout;

	task_thread_info(idle)->cpu = cpuid;

	/*                                               
                                         
 */
	cpu_now_booting = cpuid;

	/* 
                                                        
                                       
 */
	smp_init_current_idle_task = idle ;
	mb();

	printk(KERN_INFO "Releasing cpu %d now, hpa=%lx\n", cpuid, p->hpa);

	/*
                                                            
   
                                                                       
                                                                     
                                                                       
                                                                 
                                  
 */
	gsc_writel(TIMER_IRQ - CPU_IRQ_BASE, p->hpa);
	mb();

	/* 
                                                            
                                                        
                                                               
  */
	for (timeout = 0; timeout < 10000; timeout++) {
		if(cpu_online(cpuid)) {
			/*                                    */
			cpu_now_booting = 0;
			smp_init_current_idle_task = NULL;
			goto alive ;
		}
		udelay(100);
		barrier();
	}
	printk(KERN_CRIT "SMP: CPU:%d is stuck.\n", cpuid);
	return -1;

alive:
	/*                         */
	smp_debug(100, KERN_DEBUG "SMP: CPU:%d came alive after %ld _us\n",
		cpuid, timeout * 100);
	return 0;
}

void __init smp_prepare_boot_cpu(void)
{
	int bootstrap_processor = per_cpu(cpu_data, 0).cpuid;

	/*                    */
	printk(KERN_INFO "SMP: bootstrap CPU ID is %d\n", bootstrap_processor);

	set_cpu_online(bootstrap_processor, true);
	set_cpu_present(bootstrap_processor, true);
}



/*
                                                             
                                                    
*/
void __init smp_prepare_cpus(unsigned int max_cpus)
{
	int cpu;

	for_each_possible_cpu(cpu)
		spin_lock_init(&per_cpu(ipi_lock, cpu));

	init_cpu_present(cpumask_of(0));

	parisc_max_cpus = max_cpus;
	if (!max_cpus)
		printk(KERN_INFO "SMP mode deactivated.\n");
}


void smp_cpus_done(unsigned int cpu_max)
{
	return;
}


int __cpuinit __cpu_up(unsigned int cpu, struct task_struct *tidle)
{
	if (cpu != 0 && cpu < parisc_max_cpus)
		smp_boot_one_cpu(cpu, tidle);

	return cpu_online(cpu) ? 0 : -ENOSYS;
}

#ifdef CONFIG_PROC_FS
int __init
setup_profiling_timer(unsigned int multiplier)
{
	return -EINVAL;
}
#endif