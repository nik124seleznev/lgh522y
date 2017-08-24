/*
 * arch/sh/kernel/smp.c
 *
 * SMP support for the SuperH processors.
 *
 * Copyright (C) 2002 - 2010 Paul Mundt
 * Copyright (C) 2006 - 2007 Akio Idehara
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/err.h>
#include <linux/cache.h>
#include <linux/cpumask.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/cpu.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/atomic.h>
#include <asm/processor.h>
#include <asm/mmu_context.h>
#include <asm/smp.h>
#include <asm/cacheflush.h>
#include <asm/sections.h>
#include <asm/setup.h>

int __cpu_number_map[NR_CPUS];		/*                         */
int __cpu_logical_map[NR_CPUS];		/*                         */

struct plat_smp_ops *mp_ops = NULL;

/*                   */
DEFINE_PER_CPU(int, cpu_state) = { 0 };

void __cpuinit register_smp_ops(struct plat_smp_ops *ops)
{
	if (mp_ops)
		printk(KERN_WARNING "Overriding previously set SMP ops\n");

	mp_ops = ops;
}

static inline void __cpuinit smp_store_cpu_info(unsigned int cpu)
{
	struct sh_cpuinfo *c = cpu_data + cpu;

	memcpy(c, &boot_cpu_data, sizeof(struct sh_cpuinfo));

	c->loops_per_jiffy = loops_per_jiffy;
}

void __init smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int cpu = smp_processor_id();

	init_new_context(current, &init_mm);
	current_thread_info()->cpu = cpu;
	mp_ops->prepare_cpus(max_cpus);

#ifndef CONFIG_HOTPLUG_CPU
	init_cpu_present(cpu_possible_mask);
#endif
}

void __init smp_prepare_boot_cpu(void)
{
	unsigned int cpu = smp_processor_id();

	__cpu_number_map[0] = cpu;
	__cpu_logical_map[0] = cpu;

	set_cpu_online(cpu, true);
	set_cpu_possible(cpu, true);

	per_cpu(cpu_state, cpu) = CPU_ONLINE;
}

#ifdef CONFIG_HOTPLUG_CPU
void native_cpu_die(unsigned int cpu)
{
	unsigned int i;

	for (i = 0; i < 10; i++) {
		smp_rmb();
		if (per_cpu(cpu_state, cpu) == CPU_DEAD) {
			if (system_state == SYSTEM_RUNNING)
				pr_info("CPU %u is now offline\n", cpu);

			return;
		}

		msleep(100);
	}

	pr_err("CPU %u didn't die...\n", cpu);
}

int native_cpu_disable(unsigned int cpu)
{
	return cpu == 0 ? -EPERM : 0;
}

void play_dead_common(void)
{
	idle_task_exit();
	irq_ctx_exit(raw_smp_processor_id());
	mb();

	__get_cpu_var(cpu_state) = CPU_DEAD;
	local_irq_disable();
}

void native_play_dead(void)
{
	play_dead_common();
}

int __cpu_disable(void)
{
	unsigned int cpu = smp_processor_id();
	int ret;

	ret = mp_ops->cpu_disable(cpu);
	if (ret)
		return ret;

	/*
                                                                
                                                                  
  */
	set_cpu_online(cpu, false);

	/*
                                        
  */
	migrate_irqs();

	/*
                                      
  */
	local_timer_stop(cpu);

	/*
                                                               
                                          
  */
	flush_cache_all();
	local_flush_tlb_all();

	clear_tasks_mm_cpumask(cpu);

	return 0;
}
#else /*                         */
int native_cpu_disable(unsigned int cpu)
{
	return -ENOSYS;
}

void native_cpu_die(unsigned int cpu)
{
	/*                               */
	BUG();
}

void native_play_dead(void)
{
	BUG();
}
#endif

asmlinkage void __cpuinit start_secondary(void)
{
	unsigned int cpu = smp_processor_id();
	struct mm_struct *mm = &init_mm;

	enable_mmu();
	atomic_inc(&mm->mm_count);
	atomic_inc(&mm->mm_users);
	current->active_mm = mm;
	enter_lazy_tlb(mm, current);
	local_flush_tlb_all();

	per_cpu_trap_init();

	preempt_disable();

	notify_cpu_starting(cpu);

	local_irq_enable();

	/*                     */
	local_timer_setup(cpu);
	calibrate_delay();

	smp_store_cpu_info(cpu);

	set_cpu_online(cpu, true);
	per_cpu(cpu_state, cpu) = CPU_ONLINE;

	cpu_startup_entry(CPUHP_ONLINE);
}

extern struct {
	unsigned long sp;
	unsigned long bss_start;
	unsigned long bss_end;
	void *start_kernel_fn;
	void *cpu_init_fn;
	void *thread_info;
} stack_start;

int __cpuinit __cpu_up(unsigned int cpu, struct task_struct *tsk)
{
	unsigned long timeout;

	per_cpu(cpu_state, cpu) = CPU_UP_PREPARE;

	/*                                           */
	stack_start.sp = tsk->thread.sp;
	stack_start.thread_info = tsk->stack;
	stack_start.bss_start = 0; /*                                    */
	stack_start.start_kernel_fn = start_secondary;

	flush_icache_range((unsigned long)&stack_start,
			   (unsigned long)&stack_start + sizeof(stack_start));
	wmb();

	mp_ops->start_cpu(cpu, (unsigned long)_stext);

	timeout = jiffies + HZ;
	while (time_before(jiffies, timeout)) {
		if (cpu_online(cpu))
			break;

		udelay(10);
		barrier();
	}

	if (cpu_online(cpu))
		return 0;

	return -ENOENT;
}

void __init smp_cpus_done(unsigned int max_cpus)
{
	unsigned long bogosum = 0;
	int cpu;

	for_each_online_cpu(cpu)
		bogosum += cpu_data[cpu].loops_per_jiffy;

	printk(KERN_INFO "SMP: Total of %d processors activated "
	       "(%lu.%02lu BogoMIPS).\n", num_online_cpus(),
	       bogosum / (500000/HZ),
	       (bogosum / (5000/HZ)) % 100);
}

void smp_send_reschedule(int cpu)
{
	mp_ops->send_ipi(cpu, SMP_MSG_RESCHEDULE);
}

void smp_send_stop(void)
{
	smp_call_function(stop_this_cpu, 0, 0);
}

void arch_send_call_function_ipi_mask(const struct cpumask *mask)
{
	int cpu;

	for_each_cpu(cpu, mask)
		mp_ops->send_ipi(cpu, SMP_MSG_FUNCTION);
}

void arch_send_call_function_single_ipi(int cpu)
{
	mp_ops->send_ipi(cpu, SMP_MSG_FUNCTION_SINGLE);
}

void smp_timer_broadcast(const struct cpumask *mask)
{
	int cpu;

	for_each_cpu(cpu, mask)
		mp_ops->send_ipi(cpu, SMP_MSG_TIMER);
}

static void ipi_timer(void)
{
	irq_enter();
	local_timer_interrupt();
	irq_exit();
}

void smp_message_recv(unsigned int msg)
{
	switch (msg) {
	case SMP_MSG_FUNCTION:
		generic_smp_call_function_interrupt();
		break;
	case SMP_MSG_RESCHEDULE:
		scheduler_ipi();
		break;
	case SMP_MSG_FUNCTION_SINGLE:
		generic_smp_call_function_single_interrupt();
		break;
	case SMP_MSG_TIMER:
		ipi_timer();
		break;
	default:
		printk(KERN_WARNING "SMP %d: %s(): unknown IPI %d\n",
		       smp_processor_id(), __func__, msg);
		break;
	}
}

/*                          */
int setup_profiling_timer(unsigned int multiplier)
{
	return 0;
}

static void flush_tlb_all_ipi(void *info)
{
	local_flush_tlb_all();
}

void flush_tlb_all(void)
{
	on_each_cpu(flush_tlb_all_ipi, 0, 1);
}

static void flush_tlb_mm_ipi(void *mm)
{
	local_flush_tlb_mm((struct mm_struct *)mm);
}

/*
                                                                      
                                                                       
                                                                        
                                                                          
                                                                   
                                                                     
                                                                         
                                                                       
                                                                       
               
 */
void flush_tlb_mm(struct mm_struct *mm)
{
	preempt_disable();

	if ((atomic_read(&mm->mm_users) != 1) || (current->mm != mm)) {
		smp_call_function(flush_tlb_mm_ipi, (void *)mm, 1);
	} else {
		int i;
		for (i = 0; i < num_online_cpus(); i++)
			if (smp_processor_id() != i)
				cpu_context(i, mm) = 0;
	}
	local_flush_tlb_mm(mm);

	preempt_enable();
}

struct flush_tlb_data {
	struct vm_area_struct *vma;
	unsigned long addr1;
	unsigned long addr2;
};

static void flush_tlb_range_ipi(void *info)
{
	struct flush_tlb_data *fd = (struct flush_tlb_data *)info;

	local_flush_tlb_range(fd->vma, fd->addr1, fd->addr2);
}

void flush_tlb_range(struct vm_area_struct *vma,
		     unsigned long start, unsigned long end)
{
	struct mm_struct *mm = vma->vm_mm;

	preempt_disable();
	if ((atomic_read(&mm->mm_users) != 1) || (current->mm != mm)) {
		struct flush_tlb_data fd;

		fd.vma = vma;
		fd.addr1 = start;
		fd.addr2 = end;
		smp_call_function(flush_tlb_range_ipi, (void *)&fd, 1);
	} else {
		int i;
		for (i = 0; i < num_online_cpus(); i++)
			if (smp_processor_id() != i)
				cpu_context(i, mm) = 0;
	}
	local_flush_tlb_range(vma, start, end);
	preempt_enable();
}

static void flush_tlb_kernel_range_ipi(void *info)
{
	struct flush_tlb_data *fd = (struct flush_tlb_data *)info;

	local_flush_tlb_kernel_range(fd->addr1, fd->addr2);
}

void flush_tlb_kernel_range(unsigned long start, unsigned long end)
{
	struct flush_tlb_data fd;

	fd.addr1 = start;
	fd.addr2 = end;
	on_each_cpu(flush_tlb_kernel_range_ipi, (void *)&fd, 1);
}

static void flush_tlb_page_ipi(void *info)
{
	struct flush_tlb_data *fd = (struct flush_tlb_data *)info;

	local_flush_tlb_page(fd->vma, fd->addr1);
}

void flush_tlb_page(struct vm_area_struct *vma, unsigned long page)
{
	preempt_disable();
	if ((atomic_read(&vma->vm_mm->mm_users) != 1) ||
	    (current->mm != vma->vm_mm)) {
		struct flush_tlb_data fd;

		fd.vma = vma;
		fd.addr1 = page;
		smp_call_function(flush_tlb_page_ipi, (void *)&fd, 1);
	} else {
		int i;
		for (i = 0; i < num_online_cpus(); i++)
			if (smp_processor_id() != i)
				cpu_context(i, vma->vm_mm) = 0;
	}
	local_flush_tlb_page(vma, page);
	preempt_enable();
}

static void flush_tlb_one_ipi(void *info)
{
	struct flush_tlb_data *fd = (struct flush_tlb_data *)info;
	local_flush_tlb_one(fd->addr1, fd->addr2);
}

void flush_tlb_one(unsigned long asid, unsigned long vaddr)
{
	struct flush_tlb_data fd;

	fd.addr1 = asid;
	fd.addr2 = vaddr;

	smp_call_function(flush_tlb_one_ipi, (void *)&fd, 1);
	local_flush_tlb_one(asid, vaddr);
}