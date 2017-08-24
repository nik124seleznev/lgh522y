/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * vineetg: Jan 1011
 *  -sched_clock( ) no longer jiffies based. Uses the same clocksource
 *   as gtod
 *
 * Rajeshwarr/Vineetg: Mar 2008
 *  -Implemented CONFIG_GENERIC_TIME (rather deleted arch specific code)
 *   for arch independent gettimeofday()
 *  -Implemented CONFIG_GENERIC_CLOCKEVENTS as base for hrtimers
 *
 * Vineetg: Mar 2008: Forked off from time.c which now is time-jiff.c
 */

/*                                                                
                                                                 
                               
                                                   
  
                                                   
                                              
  
                                                                              
 */

#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/timex.h>
#include <linux/profile.h>
#include <linux/clocksource.h>
#include <linux/clockchips.h>
#include <asm/irq.h>
#include <asm/arcregs.h>
#include <asm/clk.h>
#include <asm/mach_desc.h>

#define ARC_TIMER_MAX	0xFFFFFFFF

/*                                      */

#ifdef CONFIG_ARC_HAS_RTSC

int __cpuinit arc_counter_setup(void)
{
	/*                                             */

	/*                                                              */
	return 1;
}

static cycle_t arc_counter_read(struct clocksource *cs)
{
	unsigned long flags;
	union {
#ifdef CONFIG_CPU_BIG_ENDIAN
		struct { u32 high, low; };
#else
		struct { u32 low, high; };
#endif
		cycle_t  full;
	} stamp;

	flags = arch_local_irq_save();

	__asm__ __volatile(
	"	.extCoreRegister tsch, 58,  r, cannot_shortcut	\n"
	"	rtsc %0, 0	\n"
	"	mov  %1, 0	\n"
	: "=r" (stamp.low), "=r" (stamp.high));

	arch_local_irq_restore(flags);

	return stamp.full;
}

static struct clocksource arc_counter = {
	.name   = "ARC RTSC",
	.rating = 300,
	.read   = arc_counter_read,
	.mask   = CLOCKSOURCE_MASK(32),
	.flags  = CLOCK_SOURCE_IS_CONTINUOUS,
};

#else /*                      */

static bool is_usable_as_clocksource(void)
{
#ifdef CONFIG_SMP
	return 0;
#else
	return 1;
#endif
}

/*
                                                                 
 */
int __cpuinit arc_counter_setup(void)
{
	write_aux_reg(ARC_REG_TIMER1_LIMIT, ARC_TIMER_MAX);
	write_aux_reg(ARC_REG_TIMER1_CNT, 0);
	write_aux_reg(ARC_REG_TIMER1_CTRL, TIMER_CTRL_NH);

	return is_usable_as_clocksource();
}

static cycle_t arc_counter_read(struct clocksource *cs)
{
	return (cycle_t) read_aux_reg(ARC_REG_TIMER1_CNT);
}

static struct clocksource arc_counter = {
	.name   = "ARC Timer1",
	.rating = 300,
	.read   = arc_counter_read,
	.mask   = CLOCKSOURCE_MASK(32),
	.flags  = CLOCK_SOURCE_IS_CONTINUOUS,
};

#endif

/*                                     */

/*
                                                 
                                                                              
 */
static void arc_timer_event_setup(unsigned int limit)
{
	write_aux_reg(ARC_REG_TIMER0_LIMIT, limit);
	write_aux_reg(ARC_REG_TIMER0_CNT, 0);	/*              */

	write_aux_reg(ARC_REG_TIMER0_CTRL, TIMER_CTRL_IE | TIMER_CTRL_NH);
}

/*
                                                                          
                                                                           
                                          
  
                                                  
                      
                            
         
                     
                                                  
 */
static void arc_timer_event_ack(unsigned int irq_reenable)
{
	write_aux_reg(ARC_REG_TIMER0_CTRL, irq_reenable | TIMER_CTRL_NH);
}

static int arc_clkevent_set_next_event(unsigned long delta,
				       struct clock_event_device *dev)
{
	arc_timer_event_setup(delta);
	return 0;
}

static void arc_clkevent_set_mode(enum clock_event_mode mode,
				  struct clock_event_device *dev)
{
	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		arc_timer_event_setup(arc_get_core_freq() / HZ);
		break;
	case CLOCK_EVT_MODE_ONESHOT:
		break;
	default:
		break;
	}

	return;
}

static DEFINE_PER_CPU(struct clock_event_device, arc_clockevent_device) = {
	.name		= "ARC Timer0",
	.features	= CLOCK_EVT_FEAT_ONESHOT | CLOCK_EVT_FEAT_PERIODIC,
	.mode		= CLOCK_EVT_MODE_UNUSED,
	.rating		= 300,
	.irq		= TIMER0_IRQ,	/*                                  */
	.set_next_event = arc_clkevent_set_next_event,
	.set_mode	= arc_clkevent_set_mode,
};

static irqreturn_t timer_irq_handler(int irq, void *dev_id)
{
	struct clock_event_device *clk = &__get_cpu_var(arc_clockevent_device);

	arc_timer_event_ack(clk->mode == CLOCK_EVT_MODE_PERIODIC);
	clk->event_handler(clk);
	return IRQ_HANDLED;
}

static struct irqaction arc_timer_irq = {
	.name    = "Timer0 (clock-evt-dev)",
	.flags   = IRQF_TIMER | IRQF_PERCPU,
	.handler = timer_irq_handler,
};

/*
                                       
                                                                    
 */
void __attribute__((weak)) __cpuinit arc_local_timer_setup(unsigned int cpu)
{
	struct clock_event_device *clk = &per_cpu(arc_clockevent_device, cpu);

	clockevents_calc_mult_shift(clk, arc_get_core_freq(), 5);

	clk->max_delta_ns = clockevent_delta2ns(ARC_TIMER_MAX, clk);
	clk->cpumask = cpumask_of(cpu);

	clockevents_register_device(clk);

	/*
                                                      
                                              
                                                                    
  */
	if (!cpu)
		setup_irq(TIMER0_IRQ, &arc_timer_irq);
	else
		arch_unmask_irq(TIMER0_IRQ);
}

/*
                                             
  
                                                
                                                             
                                                                         
                                                                           
                                                                            
 */
void __init time_init(void)
{
	/*
                                                                   
                                                
  */
	if (arc_counter_setup())
		/*
                                                           
                                               
   */
		clocksource_register_hz(&arc_counter, arc_get_core_freq());

	/*                                  */
	arc_local_timer_setup(smp_processor_id());

	if (machine_desc->init_time)
		machine_desc->init_time();
}
