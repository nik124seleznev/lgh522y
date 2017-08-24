/*
                                           
 */
#include <linux/sched.h>
#include <linux/cpu.h>
#include <linux/tick.h>
#include <linux/mm.h>
#ifdef CONFIG_MT_LOAD_BALANCE_PROFILER
#include <mtlbprof/mtlbprof.h>
#endif
#include <linux/stackprotector.h>

#include <asm/tlb.h>

#include <trace/events/power.h>

static int __read_mostly cpu_idle_force_poll;

void cpu_idle_poll_ctrl(bool enable)
{
	if (enable) {
		cpu_idle_force_poll++;
	} else {
		cpu_idle_force_poll--;
		WARN_ON_ONCE(cpu_idle_force_poll < 0);
	}
}

#ifdef CONFIG_GENERIC_IDLE_POLL_SETUP
static int __init cpu_idle_poll_setup(char *__unused)
{
	cpu_idle_force_poll = 1;
	return 1;
}
__setup("nohlt", cpu_idle_poll_setup);

static int __init cpu_idle_nopoll_setup(char *__unused)
{
	cpu_idle_force_poll = 0;
	return 1;
}
__setup("hlt", cpu_idle_nopoll_setup);
#endif

static inline int cpu_idle_poll(void)
{
	rcu_idle_enter();
	trace_cpu_idle_rcuidle(0, smp_processor_id());
	local_irq_enable();
	while (!tif_need_resched())
		cpu_relax();
	trace_cpu_idle_rcuidle(PWR_EVENT_EXIT, smp_processor_id());
	rcu_idle_exit();
	return 1;
}

/*                                                           */
void __weak arch_cpu_idle_prepare(void) { }
void __weak arch_cpu_idle_enter(void) { }
void __weak arch_cpu_idle_exit(void) { }
void __weak arch_cpu_idle_dead(void) { }
void __weak arch_cpu_idle(void)
{
	cpu_idle_force_poll = 1;
	local_irq_enable();
}

/*
                                   
 */
static void cpu_idle_loop(void)
{
#ifdef CONFIG_MT_LOAD_BALANCE_PROFILER
	mt_lbprof_update_state(smp_processor_id(), MT_LBPROF_NO_TASK_STATE);
#endif

	while (1) {
		tick_nohz_idle_enter();

		while (!need_resched()) {
			check_pgt_cache();
			rmb();

			if (cpu_is_offline(smp_processor_id()))
				arch_cpu_idle_dead();

#ifdef CONFIG_MT_LOAD_BALANCE_PROFILER
			mt_lbprof_update_state(smp_processor_id(), MT_LBPROF_IDLE_STATE);
#endif

			local_irq_disable();
			arch_cpu_idle_enter();

			/*
                                                   
     
                                                 
                                                 
                                                 
                                                
          
    */
			if (cpu_idle_force_poll || tick_check_broadcast_expired()) {
				cpu_idle_poll();
			} else {
				if (!current_clr_polling_and_test()) {
					stop_critical_timings();
					rcu_idle_enter();
					arch_cpu_idle();
					WARN_ON_ONCE(irqs_disabled());
					rcu_idle_exit();
					start_critical_timings();
#ifdef CONFIG_MT_LOAD_BALANCE_PROFILER				
					mt_lbprof_update_state(smp_processor_id(), MT_LBPROF_NO_TASK_STATE);
#endif				
				} else {
					local_irq_enable();
				}
				__current_set_polling();
			}
			arch_cpu_idle_exit();
		}
		tick_nohz_idle_exit();
		schedule_preempt_disabled();
	}
}

void cpu_startup_entry(enum cpuhp_state state)
{
	/*
                                                               
                                                               
                                                       
  */
#ifdef CONFIG_X86
	/*
                                                              
                                                               
                                                               
                                                          
                                                     
  */
	boot_init_stack_canary();
#endif
	__current_set_polling();
	arch_cpu_idle_prepare();
	cpu_idle_loop();
}
