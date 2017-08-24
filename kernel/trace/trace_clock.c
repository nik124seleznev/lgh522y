/*
 * tracing clocks
 *
 *  Copyright (C) 2009 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *
 * Implements 3 trace clock variants, with differing scalability/precision
 * tradeoffs:
 *
 *  -   local: CPU-local trace clock
 *  -  medium: scalable global clock with some jitter
 *  -  global: globally monotonic, serialized clock
 *
 * Tracer plugins will chose a default from these clocks.
 */
#include <linux/spinlock.h>
#include <linux/irqflags.h>
#include <linux/hardirq.h>
#include <linux/module.h>
#include <linux/percpu.h>
#include <linux/sched.h>
#include <linux/ktime.h>
#include <linux/trace_clock.h>

/*
                                                                      
  
                                                           
                                  
 */
u64 notrace trace_clock_local(void)
{
	u64 clock;

	/*
                                                                 
                                                              
                                     
  */
	preempt_disable_notrace();
	clock = sched_clock();
	preempt_enable_notrace();

	return clock;
}
EXPORT_SYMBOL_GPL(trace_clock_local);

/*
                                                                   
                                                          
  
                                                                     
                                                                  
                                    
 */
u64 notrace trace_clock(void)
{
	return local_clock();
}

/*
                                                              
                                                             
                                                            
                                                          
                                                          
 */
u64 notrace trace_clock_jiffies(void)
{
	return jiffies_64_to_clock_t(jiffies_64 - INITIAL_JIFFIES);
}

/*
                                                              
  
                                                                  
                                                                  
  
                                                          
 */

/*                                                */
static struct {
	u64 prev_time;
	arch_spinlock_t lock;
} trace_clock_struct ____cacheline_aligned_in_smp =
	{
		.lock = (arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED,
	};

u64 notrace trace_clock_global(void)
{
	unsigned long flags;
	int this_cpu;
	u64 now;

	local_irq_save(flags);

	this_cpu = raw_smp_processor_id();
	now = sched_clock_cpu(this_cpu);
	/*
                                                              
                     
  */
	if (unlikely(in_nmi()))
		goto out;

	arch_spin_lock(&trace_clock_struct.lock);

	/*
                                                          
                                              
                                                      
  */
	if ((s64)(now - trace_clock_struct.prev_time) < 0)
		now = trace_clock_struct.prev_time + 1;

	trace_clock_struct.prev_time = now;

	arch_spin_unlock(&trace_clock_struct.lock);

 out:
	local_irq_restore(flags);

	return now;
}

static atomic64_t trace_counter;

/*
                                                   
                                                                  
                                                        
 */
u64 notrace trace_clock_counter(void)
{
	return atomic64_add_return(1, &trace_counter);
}
