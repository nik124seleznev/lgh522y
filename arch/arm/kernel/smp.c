/*
 *  linux/arch/arm/kernel/smp.c
 *
 *  Copyright (C) 2002 ARM Limited, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
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
#include <linux/seq_file.h>
#include <linux/irq.h>
#include <linux/percpu.h>
#include <linux/clockchips.h>
#include <linux/completion.h>
#include <linux/cpufreq.h>

#include <linux/atomic.h>
#include <asm/smp.h>
#include <asm/cacheflush.h>
#include <asm/cpu.h>
#include <asm/cputype.h>
#include <asm/exception.h>
#include <asm/idmap.h>
#include <asm/topology.h>
#include <asm/mmu_context.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/processor.h>
#include <asm/sections.h>
#include <asm/tlbflush.h>
#include <asm/ptrace.h>
#include <asm/localtimer.h>
#include <asm/smp_plat.h>
#include <asm/virt.h>
#include <asm/mach/arch.h>
#include <linux/mt_sched_mon.h>
/*                                                                              
                                                                                
                                                                                
                                                                              */
#include <linux/mtk_ram_console.h>
/*                                                                            */

#define CREATE_TRACE_POINTS
#include <trace/events/ipi.h>

/*
                                                              
                                                            
                               
 */
struct secondary_data secondary_data;

/*
                                                                  
                     
 */
volatile int __cpuinitdata pen_release = -1;

enum ipi_msg_type {
	IPI_WAKEUP,
	IPI_TIMER,
	IPI_RESCHEDULE,
	IPI_CALL_FUNC,
	IPI_CALL_FUNC_SINGLE,
	IPI_CPU_STOP,
	IPI_CPU_BACKTRACE,
};

static DECLARE_COMPLETION(cpu_running);

static struct smp_operations smp_ops;

void __init smp_set_ops(struct smp_operations *ops)
{
	if (ops)
		smp_ops = *ops;
};

int __cpuinit __cpu_up(unsigned int cpu, struct task_struct *idle)
{
	int ret;

	/*
                                                    
                                  
  */
	secondary_data.stack = task_stack_page(idle) + THREAD_START_SP;
	secondary_data.pgdir = virt_to_phys(idmap_pgd);
	secondary_data.swapper_pg_dir = virt_to_phys(swapper_pg_dir);
	__cpuc_flush_dcache_area(&secondary_data, sizeof(secondary_data));
	outer_clean_range(__pa(&secondary_data), __pa(&secondary_data + 1));

	/*
                                     
  */
	ret = boot_secondary(cpu, idle);
	if (ret == 0) {
		/*
                                              
                                
   */
		wait_for_completion_timeout(&cpu_running,
						 msecs_to_jiffies(1000));

		if (!cpu_online(cpu)) {
			pr_crit("CPU%u: failed to come online\n", cpu);
			ret = -EIO;
		}
	} else {
		pr_err("CPU%u: failed to boot: %d\n", cpu, ret);
	}

	secondary_data.stack = NULL;
	secondary_data.pgdir = 0;

	return ret;
}

/*                                  */
void __init smp_init_cpus(void)
{
	if (smp_ops.smp_init_cpus)
		smp_ops.smp_init_cpus();
}

int __cpuinit boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	if (smp_ops.smp_boot_secondary)
		return smp_ops.smp_boot_secondary(cpu, idle);
	return -ENOSYS;
}

#ifdef CONFIG_HOTPLUG_CPU
static void percpu_timer_stop(void);

static int platform_cpu_kill(unsigned int cpu)
{
	if (smp_ops.cpu_kill)
		return smp_ops.cpu_kill(cpu);
	return 1;
}

static int platform_cpu_disable(unsigned int cpu)
{
	if (smp_ops.cpu_disable)
		return smp_ops.cpu_disable(cpu);

	/*
                                                              
                                                             
                             
  */
	return cpu == 0 ? -EPERM : 0;
}
/*
                                                      
 */
int __cpuinit __cpu_disable(void)
{
	unsigned int cpu = smp_processor_id();
	int ret;

	ret = platform_cpu_disable(cpu);
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
	percpu_timer_stop();

	/*
                                                               
                                          
   
                                                                  
                                                                   
  */
	flush_cache_louis();
	local_flush_tlb_all();

	clear_tasks_mm_cpumask(cpu);

	return 0;
}

static DECLARE_COMPLETION(cpu_died);

/*
                                                                  
                                                          
 */
void __cpuinit __cpu_die(unsigned int cpu)
{
	if (!wait_for_completion_timeout(&cpu_died, msecs_to_jiffies(5000))) {
		pr_err("CPU%u: cpu didn't die\n", cpu);
		return;
	}
	printk(KERN_NOTICE "CPU%u: shutdown\n", cpu);

	/*
                                                                    
                                                                    
                                                                 
                                                                   
                                                               
  */
	if (!platform_cpu_kill(cpu))
		printk("CPU%u: unable to kill\n", cpu);
}

/*
                                                                   
  
                                                            
                                                             
                                                               
                          
 */
void __ref cpu_die(void)
{
	unsigned int cpu = smp_processor_id();
	aee_rr_rec_hoplug(cpu, 51, 0);

	idle_task_exit();
	aee_rr_rec_hoplug(cpu, 52, 0);

	local_irq_disable();
	aee_rr_rec_hoplug(cpu, 53, 0);

	/*
                                                                  
                                                                   
                                                              
                                        
  */
	flush_cache_louis();
	aee_rr_rec_hoplug(cpu, 54, 0);

	/*
                                                                   
                                                                 
                                                       
  */
	complete(&cpu_died);
	aee_rr_rec_hoplug(cpu, 55, 0);

	/*
                                                                   
                                                                    
                                                                  
                             
  */
	flush_cache_louis();
	aee_rr_rec_hoplug(cpu, 56, 0);

	/*
                                                                  
                                                                 
   
                                                                    
                                                                
                                                                 
                                                                 
   
                                                              
                      
  */
	if (smp_ops.cpu_die)
		smp_ops.cpu_die(cpu);

	/*
                                                               
                                                                
                                                                 
  */
	__asm__("mov	sp, %0\n"
	"	mov	fp, #0\n"
	"	b	secondary_start_kernel"
		:
		: "r" (task_stack_page(current) + THREAD_SIZE - 8));
}
#endif /*                    */

/*
                                                               
                         
 */
static void __cpuinit smp_store_cpu_info(unsigned int cpuid)
{
	struct cpuinfo_arm *cpu_info = &per_cpu(cpu_data, cpuid);

	cpu_info->loops_per_jiffy = loops_per_jiffy;
	cpu_info->cpuid = read_cpuid_id();

	store_cpu_topology(cpuid);
}

static void percpu_timer_setup(void);

/*
                                                               
                                                         
 */
asmlinkage void __cpuinit secondary_start_kernel(void)
{
	struct mm_struct *mm = &init_mm;
	unsigned int cpu = 0;
	aee_rr_rec_hoplug(cpu, 1, 0);

	/*
                                                           
                                                                 
  */
	cpu_switch_mm(mm->pgd, mm);
	local_flush_bp_all();
	enter_lazy_tlb(mm, current);
	local_flush_tlb_all();
	aee_rr_rec_hoplug(cpu, 2, 0);

	/*
                                                        
                               
  */
	cpu = smp_processor_id();
	aee_rr_rec_hoplug(cpu, 3, 0);
	atomic_inc(&mm->mm_count);
	current->active_mm = mm;
	cpumask_set_cpu(cpu, mm_cpumask(mm));
	aee_rr_rec_hoplug(cpu, 4, 0);

	cpu_init();
	aee_rr_rec_hoplug(cpu, 5, 0);

	printk("CPU%u: Booted secondary processor\n", cpu);

	preempt_disable();
	aee_rr_rec_hoplug(cpu, 6, 0);
	trace_hardirqs_off();
	aee_rr_rec_hoplug(cpu, 7, 0);

	/*
                                                            
  */
	if (smp_ops.smp_secondary_init)
		smp_ops.smp_secondary_init(cpu);
	aee_rr_rec_hoplug(cpu, 8, 0);

	notify_cpu_starting(cpu);
	aee_rr_rec_hoplug(cpu, 9, 0);

	calibrate_delay();
	aee_rr_rec_hoplug(cpu, 10, 0);

	smp_store_cpu_info(cpu);
	aee_rr_rec_hoplug(cpu, 11, 0);

	/*
                                                             
                                                           
                                                              
  */
	set_cpu_online(cpu, true);
	aee_rr_rec_hoplug(cpu, 12, 0);
	complete(&cpu_running);
	aee_rr_rec_hoplug(cpu, 13, 0);

	/*
                                        
  */
	percpu_timer_setup();
	aee_rr_rec_hoplug(cpu, 14, 0);

	local_irq_enable();
	aee_rr_rec_hoplug(cpu, 15, 0);
	local_fiq_enable();
	aee_rr_rec_hoplug(cpu, 16, 0);

	/*
                                          
  */
	cpu_startup_entry(CPUHP_ONLINE);
	aee_rr_rec_hoplug(cpu, 17, 0);
}

void __init smp_cpus_done(unsigned int max_cpus)
{
	int cpu;
	unsigned long bogosum = 0;

	for_each_online_cpu(cpu)
		bogosum += per_cpu(cpu_data, cpu).loops_per_jiffy;

	printk(KERN_INFO "SMP: Total of %d processors activated "
	       "(%lu.%02lu BogoMIPS).\n",
	       num_online_cpus(),
	       bogosum / (500000/HZ),
	       (bogosum / (5000/HZ)) % 100);

	hyp_mode_check();
}

void __init smp_prepare_boot_cpu(void)
{
	set_my_cpu_offset(per_cpu_offset(smp_processor_id()));
}

void __init smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int ncores = num_possible_cpus();

	init_cpu_topology();

	smp_store_cpu_info(smp_processor_id());

	/*
                                                
  */
	if (max_cpus > ncores)
		max_cpus = ncores;
	if (ncores > 1 && max_cpus) {
		/*
                                                       
                                                     
   */
		percpu_timer_setup();

		/*
                                                                
                                                              
                                                              
                                                    
   */
		init_cpu_present(cpu_possible_mask);

		/*
                                                      
                                      
   */
		if (smp_ops.smp_prepare_cpus)
			smp_ops.smp_prepare_cpus(max_cpus);
	}
}

static void (*__smp_cross_call)(const struct cpumask *, unsigned int);

void __init set_smp_cross_call(void (*fn)(const struct cpumask *, unsigned int))
{
    if (!__smp_cross_call)
        __smp_cross_call = fn;
}

static const char *ipi_types[NR_IPI] __tracepoint_string = {
#define S(x,s)	[x] = s
	S(IPI_WAKEUP, "CPU wakeup interrupts"),
	S(IPI_TIMER, "Timer broadcast interrupts"),
	S(IPI_RESCHEDULE, "Rescheduling interrupts"),
	S(IPI_CALL_FUNC, "Function call interrupts"),
	S(IPI_CALL_FUNC_SINGLE, "Single function call interrupts"),
	S(IPI_CPU_STOP, "CPU stop interrupts"),
	S(IPI_CPU_BACKTRACE, "CPU backtrace"),
};

static void smp_cross_call(const struct cpumask *target, unsigned int ipinr)
{
	trace_ipi_raise(target, ipi_types[ipinr]);
	__smp_cross_call(target, ipinr);
}

void show_ipi_list(struct seq_file *p, int prec)
{
	unsigned int cpu, i;

	for (i = 0; i < NR_IPI; i++) {
		seq_printf(p, "%*s%u: ", prec - 1, "IPI", i);

		for_each_online_cpu(cpu)
			seq_printf(p, "%10u ",
				   __get_irq_stat(cpu, ipi_irqs[i]));

		seq_printf(p, " %s\n", ipi_types[i]);
	}
}

u64 smp_irq_stat_cpu(unsigned int cpu)
{
	u64 sum = 0;
	int i;

	for (i = 0; i < NR_IPI; i++)
		sum += __get_irq_stat(cpu, ipi_irqs[i]);

	return sum;
}

/*
                                     
 */
static DEFINE_PER_CPU(struct clock_event_device, percpu_clockevent);

void arch_send_call_function_ipi_mask(const struct cpumask *mask)
{
	smp_cross_call(mask, IPI_CALL_FUNC);
}

void arch_send_wakeup_ipi_mask(const struct cpumask *mask)
{
	smp_cross_call(mask, IPI_WAKEUP);
}

void arch_send_call_function_single_ipi(int cpu)
{
	smp_cross_call(cpumask_of(cpu), IPI_CALL_FUNC_SINGLE);
}

#ifdef CONFIG_GENERIC_CLOCKEVENTS_BROADCAST
void tick_broadcast(const struct cpumask *mask)
{
	smp_cross_call(mask, IPI_TIMER);
}
#endif

static void broadcast_timer_set_mode(enum clock_event_mode mode,
	struct clock_event_device *evt)
{
}

static void __cpuinit broadcast_timer_setup(struct clock_event_device *evt)
{
	evt->name	= "dummy_timer";
	evt->features	= CLOCK_EVT_FEAT_ONESHOT |
			  CLOCK_EVT_FEAT_PERIODIC |
			  CLOCK_EVT_FEAT_DUMMY;
	evt->rating	= 100;
	evt->mult	= 1;
	evt->set_mode	= broadcast_timer_set_mode;

	clockevents_register_device(evt);
}

static struct local_timer_ops *lt_ops;

#ifdef CONFIG_LOCAL_TIMERS
int local_timer_register(struct local_timer_ops *ops)
{
	if (!is_smp() || !setup_max_cpus)
		return -ENXIO;

	if (lt_ops)
		return -EBUSY;

	lt_ops = ops;
	return 0;
}
#endif

static void __cpuinit percpu_timer_setup(void)
{
	unsigned int cpu = smp_processor_id();
	struct clock_event_device *evt = &per_cpu(percpu_clockevent, cpu);

	evt->cpumask = cpumask_of(cpu);

	if (!lt_ops || lt_ops->setup(evt))
		broadcast_timer_setup(evt);
}

#ifdef CONFIG_HOTPLUG_CPU
/*
                                                                        
                                                                  
                 
 */
static void percpu_timer_stop(void)
{
	unsigned int cpu = smp_processor_id();
	struct clock_event_device *evt = &per_cpu(percpu_clockevent, cpu);

	if (lt_ops)
		lt_ops->stop(evt);
}
#endif

static DEFINE_RAW_SPINLOCK(stop_lock);

/*
                                                 
 */
static void ipi_cpu_stop(unsigned int cpu)
{
	if (system_state == SYSTEM_BOOTING ||
	    system_state == SYSTEM_RUNNING) {
		raw_spin_lock(&stop_lock);
		printk(KERN_CRIT "CPU%u: stopping\n", cpu);
		dump_stack();
		raw_spin_unlock(&stop_lock);
	}

	set_cpu_online(cpu, false);

	local_fiq_disable();
	local_irq_disable();

	while (1)
		cpu_relax();
}

static cpumask_t backtrace_mask;
static DEFINE_RAW_SPINLOCK(backtrace_lock);

/*                                                      */
static unsigned long backtrace_flag;

void smp_send_all_cpu_backtrace(void)
{
	unsigned int this_cpu = smp_processor_id();
	int i;

	if (test_and_set_bit(0, &backtrace_flag))
		/*
                                                                  
                                                               
   */
		return;

	cpumask_copy(&backtrace_mask, cpu_online_mask);
	cpu_clear(this_cpu, backtrace_mask);

	pr_info("Backtrace for cpu %d (current):\n", this_cpu);
	dump_stack();

	pr_info("\nsending IPI to all other CPUs:\n");
	smp_cross_call(&backtrace_mask, IPI_CPU_BACKTRACE);

	/*                                                                  */
	for (i = 0; i < 10 * 1000; i++) {
		if (cpumask_empty(&backtrace_mask))
			break;
		mdelay(1);
	}

	clear_bit(0, &backtrace_flag);
	smp_mb__after_clear_bit();
}

/*
                                                                   
 */
static void ipi_cpu_backtrace(unsigned int cpu, struct pt_regs *regs)
{
	if (cpu_isset(cpu, backtrace_mask)) {
		raw_spin_lock(&backtrace_lock);
		pr_warning("IPI backtrace for cpu %d\n", cpu);
		show_regs(regs);
		raw_spin_unlock(&backtrace_lock);
		cpu_clear(cpu, backtrace_mask);
	}
}

/*
                                              
 */
asmlinkage void __exception_irq_entry do_IPI(int ipinr, struct pt_regs *regs)
{
	handle_IPI(ipinr, regs);
}

void handle_IPI(int ipinr, struct pt_regs *regs)
{
	unsigned int cpu = smp_processor_id();
	struct pt_regs *old_regs = set_irq_regs(regs);

	if ((unsigned)ipinr < NR_IPI) {
		trace_ipi_entry(ipi_types[ipinr]);
		__inc_irq_stat(cpu, ipi_irqs[ipinr]);
	}

	switch (ipinr) {
	case IPI_WAKEUP:
        mt_trace_ISR_start(ipinr);
        mt_trace_ISR_end(ipinr);
		break;

#ifdef CONFIG_GENERIC_CLOCKEVENTS_BROADCAST
	case IPI_TIMER:
		irq_enter();
        mt_trace_ISR_start(ipinr);
		tick_receive_broadcast();
        mt_trace_ISR_end(ipinr);
		irq_exit();
		break;
#endif

	case IPI_RESCHEDULE:
		scheduler_ipi();
		break;

	case IPI_CALL_FUNC:
		irq_enter();
        mt_trace_ISR_start(ipinr);
		generic_smp_call_function_interrupt();
        mt_trace_ISR_end(ipinr);
		irq_exit();
		break;

	case IPI_CALL_FUNC_SINGLE:
		irq_enter();
        mt_trace_ISR_start(ipinr);
		generic_smp_call_function_single_interrupt();
        mt_trace_ISR_end(ipinr);
		irq_exit();
		break;

	case IPI_CPU_STOP:
		irq_enter();
        mt_trace_ISR_start(ipinr);
		ipi_cpu_stop(cpu);
        mt_trace_ISR_end(ipinr);
		irq_exit();
		break;

	case IPI_CPU_BACKTRACE:
        mt_trace_ISR_start(ipinr);
		ipi_cpu_backtrace(cpu, regs);
        mt_trace_ISR_end(ipinr);
		break;

	default:
        mt_trace_ISR_start(ipinr);
		printk(KERN_CRIT "CPU%u: Unknown IPI message 0x%x\n",
		       cpu, ipinr);
        mt_trace_ISR_end(ipinr);
		break;
	}

	if ((unsigned)ipinr < NR_IPI)
		trace_ipi_exit(ipi_types[ipinr]);
	set_irq_regs(old_regs);
}

void smp_send_reschedule(int cpu)
{
	smp_cross_call(cpumask_of(cpu), IPI_RESCHEDULE);
}

void smp_send_stop(void)
{
	unsigned long timeout;
	struct cpumask mask;

	cpumask_copy(&mask, cpu_online_mask);
	cpumask_clear_cpu(smp_processor_id(), &mask);
	if (!cpumask_empty(&mask))
		smp_cross_call(&mask, IPI_CPU_STOP);

	/*                                              */
	timeout = USEC_PER_SEC;
	while (num_online_cpus() > 1 && timeout--)
		udelay(1);

	if (num_online_cpus() > 1)
		pr_warning("SMP: failed to stop secondary CPUs\n");
}

/*
                     
 */
int setup_profiling_timer(unsigned int multiplier)
{
	return -EINVAL;
}

#ifdef CONFIG_CPU_FREQ

static DEFINE_PER_CPU(unsigned long, l_p_j_ref);
static DEFINE_PER_CPU(unsigned long, l_p_j_ref_freq);
static unsigned long global_l_p_j_ref;
static unsigned long global_l_p_j_ref_freq;

static int cpufreq_callback(struct notifier_block *nb,
					unsigned long val, void *data)
{
	struct cpufreq_freqs *freq = data;
	int cpu = freq->cpu;

	if (freq->flags & CPUFREQ_CONST_LOOPS)
		return NOTIFY_OK;

	if (!per_cpu(l_p_j_ref, cpu)) {
		per_cpu(l_p_j_ref, cpu) =
			per_cpu(cpu_data, cpu).loops_per_jiffy;
		per_cpu(l_p_j_ref_freq, cpu) = freq->old;
		if (!global_l_p_j_ref) {
			global_l_p_j_ref = loops_per_jiffy;
			global_l_p_j_ref_freq = freq->old;
		}
	}

	if ((val == CPUFREQ_PRECHANGE  && freq->old < freq->new) ||
	    (val == CPUFREQ_POSTCHANGE && freq->old > freq->new) ||
	    (val == CPUFREQ_RESUMECHANGE || val == CPUFREQ_SUSPENDCHANGE)) {
		loops_per_jiffy = cpufreq_scale(global_l_p_j_ref,
						global_l_p_j_ref_freq,
						freq->new);
		per_cpu(cpu_data, cpu).loops_per_jiffy =
			cpufreq_scale(per_cpu(l_p_j_ref, cpu),
					per_cpu(l_p_j_ref_freq, cpu),
					freq->new);
	}
	return NOTIFY_OK;
}

static struct notifier_block cpufreq_notifier = {
	.notifier_call  = cpufreq_callback,
};

static int __init register_cpufreq_notifier(void)
{
	return cpufreq_register_notifier(&cpufreq_notifier,
						CPUFREQ_TRANSITION_NOTIFIER);
}
core_initcall(register_cpufreq_notifier);

#endif