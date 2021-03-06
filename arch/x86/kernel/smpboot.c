 /*
 *	x86 SMP booting functions
 *
 *	(c) 1995 Alan Cox, Building #3 <alan@lxorguk.ukuu.org.uk>
 *	(c) 1998, 1999, 2000, 2009 Ingo Molnar <mingo@redhat.com>
 *	Copyright 2001 Andi Kleen, SuSE Labs.
 *
 *	Much of the core SMP work is based on previous work by Thomas Radke, to
 *	whom a great many thanks are extended.
 *
 *	Thanks to Intel for making available several different Pentium,
 *	Pentium Pro and Pentium-II/Xeon MP machines.
 *	Original development of Linux SMP code supported by Caldera.
 *
 *	This code is released under the GNU General Public License version 2 or
 *	later.
 *
 *	Fixes
 *		Felix Koop	:	NR_CPUS used properly
 *		Jose Renau	:	Handle single CPU case.
 *		Alan Cox	:	By repeated request 8) - Total BogoMIPS report.
 *		Greg Wright	:	Fix for kernel stacks panic.
 *		Erich Boleyn	:	MP v1.4 and additional changes.
 *	Matthias Sattler	:	Changes for 2.1 kernel map.
 *	Michel Lespinasse	:	Changes for 2.1 kernel map.
 *	Michael Chastain	:	Change trampoline.S to gnu as.
 *		Alan Cox	:	Dumb bug: 'B' step PPro's are fine
 *		Ingo Molnar	:	Added APIC timers, based on code
 *					from Jose Renau
 *		Ingo Molnar	:	various cleanups and rewrites
 *		Tigran Aivazian	:	fixed "0.00 in /proc/uptime on SMP" bug.
 *	Maciej W. Rozycki	:	Bits for genuine 82489DX APICs
 *	Andi Kleen		:	Changed for SMP boot into long mode.
 *		Martin J. Bligh	: 	Added support for multi-quad systems
 *		Dave Jones	:	Report invalid combinations of Athlon CPUs.
 *		Rusty Russell	:	Hacked into shape for new "hotplug" boot process.
 *      Andi Kleen              :       Converted to new state machine.
 *	Ashok Raj		: 	CPU hotplug support
 *	Glauber Costa		:	i386 and x86_64 integration
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/init.h>
#include <linux/smp.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/percpu.h>
#include <linux/bootmem.h>
#include <linux/err.h>
#include <linux/nmi.h>
#include <linux/tboot.h>
#include <linux/stackprotector.h>
#include <linux/gfp.h>
#include <linux/cpuidle.h>

#include <asm/acpi.h>
#include <asm/desc.h>
#include <asm/nmi.h>
#include <asm/irq.h>
#include <asm/idle.h>
#include <asm/realmode.h>
#include <asm/cpu.h>
#include <asm/numa.h>
#include <asm/pgtable.h>
#include <asm/tlbflush.h>
#include <asm/mtrr.h>
#include <asm/mwait.h>
#include <asm/apic.h>
#include <asm/io_apic.h>
#include <asm/i387.h>
#include <asm/fpu-internal.h>
#include <asm/setup.h>
#include <asm/uv/uv.h>
#include <linux/mc146818rtc.h>

#include <asm/smpboot_hooks.h>
#include <asm/i8259.h>

#include <asm/realmode.h>

/*                   */
DEFINE_PER_CPU(int, cpu_state) = { 0 };

#ifdef CONFIG_HOTPLUG_CPU
/*
                                                                            
                                  
 */
static DEFINE_MUTEX(x86_cpu_hotplug_driver_mutex);

void cpu_hotplug_driver_lock(void)
{
	mutex_lock(&x86_cpu_hotplug_driver_mutex);
}

void cpu_hotplug_driver_unlock(void)
{
	mutex_unlock(&x86_cpu_hotplug_driver_mutex);
}

ssize_t arch_cpu_probe(const char *buf, size_t count) { return -1; }
ssize_t arch_cpu_release(const char *buf, size_t count) { return -1; }
#endif

/*                                    */
int smp_num_siblings = 1;
EXPORT_SYMBOL(smp_num_siblings);

/*                                         */
DEFINE_PER_CPU_READ_MOSTLY(u16, cpu_llc_id) = BAD_APICID;

/*                                              */
DEFINE_PER_CPU_READ_MOSTLY(cpumask_var_t, cpu_sibling_map);
EXPORT_PER_CPU_SYMBOL(cpu_sibling_map);

/*                                                       */
DEFINE_PER_CPU_READ_MOSTLY(cpumask_var_t, cpu_core_map);
EXPORT_PER_CPU_SYMBOL(cpu_core_map);

DEFINE_PER_CPU_READ_MOSTLY(cpumask_var_t, cpu_llc_shared_map);

/*                                       */
DEFINE_PER_CPU_SHARED_ALIGNED(struct cpuinfo_x86, cpu_info);
EXPORT_PER_CPU_SYMBOL(cpu_info);

atomic_t init_deasserted;

/*
                                                                                
                     
 */
static void __cpuinit smp_callin(void)
{
	int cpuid, phys_id;
	unsigned long timeout;

	/*
                                                      
                                                       
                                                         
                              
   
                                                                      
  */
	cpuid = smp_processor_id();
	if (apic->wait_for_init_deassert && cpuid != 0)
		apic->wait_for_init_deassert(&init_deasserted);

	/*
                                                 
  */
	phys_id = read_apic_id();
	if (cpumask_test_cpu(cpuid, cpu_callin_mask)) {
		panic("%s: phys CPU#%d, CPU#%d already present??\n", __func__,
					phys_id, cpuid);
	}
	pr_debug("CPU#%d (phys ID: %d) waiting for CALLOUT\n", cpuid, phys_id);

	/*
                                                           
                                                          
                                                         
                                                         
                                              
  */

	/*
                                                            
  */
	timeout = jiffies + 2*HZ;
	while (time_before(jiffies, timeout)) {
		/*
                                                     
   */
		if (cpumask_test_cpu(cpuid, cpu_callout_mask))
			break;
		cpu_relax();
	}

	if (!time_before(jiffies, timeout)) {
		panic("%s: CPU%d started up but did not get a callout!\n",
		      __func__, cpuid);
	}

	/*
                                                            
                                                             
                                                            
           
  */

	pr_debug("CALLIN, before setup_local_APIC()\n");
	if (apic->smp_callin_clear_local_apic)
		apic->smp_callin_clear_local_apic();
	setup_local_APIC();
	end_local_APIC_setup();

	/*
                                                              
  */
	setup_vector_irq(smp_processor_id());

	/*
                                                         
                                    
  */
	smp_store_cpu_info(cpuid);

	/*
                     
                                                        
                                                                
                                          
  */
	calibrate_delay();
	cpu_data(cpuid).loops_per_jiffy = loops_per_jiffy;
	pr_debug("Stack at about %p\n", &cpuid);

	/*
                                                    
                                   
  */
	set_cpu_sibling_map(raw_smp_processor_id());
	wmb();

	notify_cpu_starting(cpuid);

	/*
                                 
  */
	cpumask_set_cpu(cpuid, cpu_callin_mask);
}

static int cpu0_logical_apicid;
static int enable_start_cpu0;
/*
                                  
 */
notrace static void __cpuinit start_secondary(void *unused)
{
	/*
                                                              
                                                             
                          
  */
	cpu_init();
	x86_cpuinit.early_percpu_clock_init();
	preempt_disable();
	smp_callin();

	enable_start_cpu0 = 0;

#ifdef CONFIG_X86_32
	/*                                         */
	load_cr3(swapper_pg_dir);
	__flush_tlb_all();
#endif

	/*                                                                 */
	barrier();
	/*
                                          
  */
	check_tsc_sync_target();

	/*
                                       
  */
#ifdef CONFIG_X86_ESPFIX64
	init_espfix_ap();
#endif

	/*
                                                               
                                                                    
                                                                       
  */
	lock_vector_lock();
	set_cpu_online(smp_processor_id(), true);
	unlock_vector_lock();
	per_cpu(cpu_state, smp_processor_id()) = CPU_ONLINE;
	x86_platform.nmi_init();

	/*                         */
	local_irq_enable();

	/*                                                    */
	boot_init_stack_canary();

	x86_cpuinit.setup_percpu_clockev();

	wmb();
	cpu_startup_entry(CPUHP_ONLINE);
}

void __init smp_store_boot_cpu_info(void)
{
	int id = 0; /*       */
	struct cpuinfo_x86 *c = &cpu_data(id);

	*c = boot_cpu_data;
	c->cpu_index = id;
}

/*
                                                                  
              
 */
void __cpuinit smp_store_cpu_info(int id)
{
	struct cpuinfo_x86 *c = &cpu_data(id);

	*c = boot_cpu_data;
	c->cpu_index = id;
	/*
                                                                     
                                    
  */
	identify_secondary_cpu(c);
}

static bool __cpuinit
topology_sane(struct cpuinfo_x86 *c, struct cpuinfo_x86 *o, const char *name)
{
	int cpu1 = c->cpu_index, cpu2 = o->cpu_index;

	return !WARN_ONCE(cpu_to_node(cpu1) != cpu_to_node(cpu2),
		"sched: CPU #%d's %s-sibling CPU #%d is not on the same node! "
		"[node: %d != %d]. Ignoring dependency.\n",
		cpu1, name, cpu2, cpu_to_node(cpu1), cpu_to_node(cpu2));
}

#define link_mask(_m, c1, c2)						\
do {									\
	cpumask_set_cpu((c1), cpu_##_m##_mask(c2));			\
	cpumask_set_cpu((c2), cpu_##_m##_mask(c1));			\
} while (0)

static bool __cpuinit match_smt(struct cpuinfo_x86 *c, struct cpuinfo_x86 *o)
{
	if (cpu_has_topoext) {
		int cpu1 = c->cpu_index, cpu2 = o->cpu_index;

		if (c->phys_proc_id == o->phys_proc_id &&
		    per_cpu(cpu_llc_id, cpu1) == per_cpu(cpu_llc_id, cpu2) &&
		    c->compute_unit_id == o->compute_unit_id)
			return topology_sane(c, o, "smt");

	} else if (c->phys_proc_id == o->phys_proc_id &&
		   c->cpu_core_id == o->cpu_core_id) {
		return topology_sane(c, o, "smt");
	}

	return false;
}

static bool __cpuinit match_llc(struct cpuinfo_x86 *c, struct cpuinfo_x86 *o)
{
	int cpu1 = c->cpu_index, cpu2 = o->cpu_index;

	if (per_cpu(cpu_llc_id, cpu1) != BAD_APICID &&
	    per_cpu(cpu_llc_id, cpu1) == per_cpu(cpu_llc_id, cpu2))
		return topology_sane(c, o, "llc");

	return false;
}

static bool __cpuinit match_mc(struct cpuinfo_x86 *c, struct cpuinfo_x86 *o)
{
	if (c->phys_proc_id == o->phys_proc_id) {
		if (cpu_has(c, X86_FEATURE_AMD_DCM))
			return true;

		return topology_sane(c, o, "mc");
	}
	return false;
}

void __cpuinit set_cpu_sibling_map(int cpu)
{
	bool has_smt = smp_num_siblings > 1;
	bool has_mp = has_smt || boot_cpu_data.x86_max_cores > 1;
	struct cpuinfo_x86 *c = &cpu_data(cpu);
	struct cpuinfo_x86 *o;
	int i;

	cpumask_set_cpu(cpu, cpu_sibling_setup_mask);

	if (!has_mp) {
		cpumask_set_cpu(cpu, cpu_sibling_mask(cpu));
		cpumask_set_cpu(cpu, cpu_llc_shared_mask(cpu));
		cpumask_set_cpu(cpu, cpu_core_mask(cpu));
		c->booted_cores = 1;
		return;
	}

	for_each_cpu(i, cpu_sibling_setup_mask) {
		o = &cpu_data(i);

		if ((i == cpu) || (has_smt && match_smt(c, o)))
			link_mask(sibling, cpu, i);

		if ((i == cpu) || (has_mp && match_llc(c, o)))
			link_mask(llc_shared, cpu, i);

	}

	/*
                                                                        
                                        
  */
	for_each_cpu(i, cpu_sibling_setup_mask) {
		o = &cpu_data(i);

		if ((i == cpu) || (has_mp && match_mc(c, o))) {
			link_mask(core, cpu, i);

			/*
                                            
    */
			if (cpumask_weight(cpu_sibling_mask(cpu)) == 1) {
				/*
                                          
                                        
     */
				if (cpumask_first(cpu_sibling_mask(i)) == i)
					c->booted_cores++;
				/*
                                       
                                     
     */
				if (i != cpu)
					cpu_data(i).booted_cores++;
			} else if (i != cpu && !c->booted_cores)
				c->booted_cores = cpu_data(i).booted_cores;
		}
	}
}

/*                                                          */
const struct cpumask *cpu_coregroup_mask(int cpu)
{
	return cpu_llc_shared_mask(cpu);
}

static void impress_friends(void)
{
	int cpu;
	unsigned long bogosum = 0;
	/*
                                      
  */
	pr_debug("Before bogomips\n");
	for_each_possible_cpu(cpu)
		if (cpumask_test_cpu(cpu, cpu_callout_mask))
			bogosum += cpu_data(cpu).loops_per_jiffy;
	pr_info("Total of %d processors activated (%lu.%02lu BogoMIPS)\n",
		num_online_cpus(),
		bogosum/(500000/HZ),
		(bogosum/(5000/HZ))%100);

	pr_debug("Before bogocount - setting activated=1\n");
}

void __inquire_remote_apic(int apicid)
{
	unsigned i, regs[] = { APIC_ID >> 4, APIC_LVR >> 4, APIC_SPIV >> 4 };
	const char * const names[] = { "ID", "VERSION", "SPIV" };
	int timeout;
	u32 status;

	pr_info("Inquiring remote APIC 0x%x...\n", apicid);

	for (i = 0; i < ARRAY_SIZE(regs); i++) {
		pr_info("... APIC 0x%x %s: ", apicid, names[i]);

		/*
                   
   */
		status = safe_apic_wait_icr_idle();
		if (status)
			pr_cont("a previous APIC delivery may have failed\n");

		apic_icr_write(APIC_DM_REMRD | regs[i], apicid);

		timeout = 0;
		do {
			udelay(100);
			status = apic_read(APIC_ICR) & APIC_ICR_RR_MASK;
		} while (status == APIC_ICR_RR_INPROG && timeout++ < 1000);

		switch (status) {
		case APIC_ICR_RR_VALID:
			status = apic_read(APIC_RRR);
			pr_cont("%08x\n", status);
			break;
		default:
			pr_cont("failed\n");
		}
	}
}

/*
                                                                                
                                                                         
                                                        
 */
int __cpuinit
wakeup_secondary_cpu_via_nmi(int apicid, unsigned long start_eip)
{
	unsigned long send_status, accept_status = 0;
	int maxlvt;

	/*             */
	/*                   */
	/*                 */
	apic_icr_write(APIC_DM_NMI | apic->dest_logical, apicid);

	pr_debug("Waiting for send to finish...\n");
	send_status = safe_apic_wait_icr_idle();

	/*
                                                   
  */
	udelay(200);
	if (APIC_INTEGRATED(apic_version[boot_cpu_physical_apicid])) {
		maxlvt = lapic_get_maxlvt();
		if (maxlvt > 3)			/*                                  */
			apic_write(APIC_ESR, 0);
		accept_status = (apic_read(APIC_ESR) & 0xEF);
	}
	pr_debug("NMI sent\n");

	if (send_status)
		pr_err("APIC never delivered???\n");
	if (accept_status)
		pr_err("APIC delivery error (%lx)\n", accept_status);

	return (send_status | accept_status);
}

static int __cpuinit
wakeup_secondary_cpu_via_init(int phys_apicid, unsigned long start_eip)
{
	unsigned long send_status, accept_status = 0;
	int maxlvt, num_starts, j;

	maxlvt = lapic_get_maxlvt();

	/*
                                           
  */
	if (APIC_INTEGRATED(apic_version[phys_apicid])) {
		if (maxlvt > 3)		/*                                  */
			apic_write(APIC_ESR, 0);
		apic_read(APIC_ESR);
	}

	pr_debug("Asserting INIT\n");

	/*
                            
  */
	/*
            
  */
	apic_icr_write(APIC_INT_LEVELTRIG | APIC_INT_ASSERT | APIC_DM_INIT,
		       phys_apicid);

	pr_debug("Waiting for send to finish...\n");
	send_status = safe_apic_wait_icr_idle();

	mdelay(10);

	pr_debug("Deasserting INIT\n");

	/*             */
	/*          */
	apic_icr_write(APIC_INT_LEVELTRIG | APIC_DM_INIT, phys_apicid);

	pr_debug("Waiting for send to finish...\n");
	send_status = safe_apic_wait_icr_idle();

	mb();
	atomic_set(&init_deasserted, 1);

	/*
                                 
   
                                             
                                                                     
  */
	if (APIC_INTEGRATED(apic_version[phys_apicid]))
		num_starts = 2;
	else
		num_starts = 0;

	/*
                                                              
                           
  */
	startup_ipi_hook(phys_apicid, (unsigned long) start_secondary,
			 stack_start);

	/*
                         
  */
	pr_debug("#startup loops: %d\n", num_starts);

	for (j = 1; j <= num_starts; j++) {
		pr_debug("Sending STARTUP #%d\n", j);
		if (maxlvt > 3)		/*                                  */
			apic_write(APIC_ESR, 0);
		apic_read(APIC_ESR);
		pr_debug("After apic_write\n");

		/*
                
   */

		/*             */
		/*                   */
		/*                 */
		apic_icr_write(APIC_DM_STARTUP | (start_eip >> 12),
			       phys_apicid);

		/*
                                                    
   */
		udelay(300);

		pr_debug("Startup point 1\n");

		pr_debug("Waiting for send to finish...\n");
		send_status = safe_apic_wait_icr_idle();

		/*
                                                    
   */
		udelay(200);
		if (maxlvt > 3)		/*                                  */
			apic_write(APIC_ESR, 0);
		accept_status = (apic_read(APIC_ESR) & 0xEF);
		if (send_status || accept_status)
			break;
	}
	pr_debug("After Startup\n");

	if (send_status)
		pr_err("APIC never delivered???\n");
	if (accept_status)
		pr_err("APIC delivery error (%lx)\n", accept_status);

	return (send_status | accept_status);
}

/*                                                                          */
static void __cpuinit announce_cpu(int cpu, int apicid)
{
	static int current_node = -1;
	int node = early_cpu_to_node(cpu);

	if (system_state == SYSTEM_BOOTING) {
		if (node != current_node) {
			if (current_node > (-1))
				pr_cont(" OK\n");
			current_node = node;
			pr_info("Booting Node %3d, Processors ", node);
		}
		pr_cont(" #%d%s", cpu, cpu == (nr_cpu_ids - 1) ? " OK\n" : "");
		return;
	} else
		pr_info("Booting Node %d Processor %d APIC 0x%x\n",
			node, cpu, apicid);
}

static int wakeup_cpu0_nmi(unsigned int cmd, struct pt_regs *regs)
{
	int cpu;

	cpu = smp_processor_id();
	if (cpu == 0 && !cpu_online(cpu) && enable_start_cpu0)
		return NMI_HANDLED;

	return NMI_DONE;
}

/*
                                              
  
                                                                        
                                                                        
                                                         
  
                                                                          
                                                                          
                                                                        
                                           
 */
static int __cpuinit
wakeup_cpu_via_init_nmi(int cpu, unsigned long start_ip, int apicid,
	       int *cpu0_nmi_registered)
{
	int id;
	int boot_error;

	/*
                                               
  */
	if (cpu)
		return wakeup_secondary_cpu_via_init(apicid, start_ip);

	/*
                       
   
                                                
  */
	boot_error = register_nmi_handler(NMI_LOCAL,
					  wakeup_cpu0_nmi, 0, "wake_cpu0");

	if (!boot_error) {
		enable_start_cpu0 = 1;
		*cpu0_nmi_registered = 1;
		if (apic->dest_logical == APIC_DEST_LOGICAL)
			id = cpu0_logical_apicid;
		else
			id = apicid;
		boot_error = wakeup_secondary_cpu_via_nmi(id, start_ip);
	}

	return boot_error;
}

/*
                                                                      
                                                                  
                                                      
                          
 */
static int __cpuinit do_boot_cpu(int apicid, int cpu, struct task_struct *idle)
{
	volatile u32 *trampoline_status =
		(volatile u32 *) __va(real_mode_header->trampoline_status);
	/*                                      */
	unsigned long start_ip = real_mode_header->trampoline_start;

	unsigned long boot_error = 0;
	int timeout;
	int cpu0_nmi_registered = 0;

	/*                                           */
	alternatives_enable_smp();

	idle->thread.sp = (unsigned long) (((struct pt_regs *)
			  (THREAD_SIZE +  task_stack_page(idle))) - 1);
	per_cpu(current_task, cpu) = idle;

#ifdef CONFIG_X86_32
	/*                                                                 */
	irq_ctx_init(cpu);
#else
	clear_tsk_thread_flag(idle, TIF_FORK);
	initial_gs = per_cpu_offset(cpu);
	per_cpu(kernel_stack, cpu) =
		(unsigned long)task_stack_page(idle) -
		KERNEL_STACK_OFFSET + THREAD_SIZE;
#endif
	early_gdt_descr.address = (unsigned long)get_cpu_gdt_table(cpu);
	initial_code = (unsigned long)start_secondary;
	stack_start  = idle->thread.sp;

	/*                     */
	announce_cpu(cpu, apicid);

	/*
                                            
                           
  */

	atomic_set(&init_deasserted, 0);

	if (get_uv_system_type() != UV_NON_UNIQUE_APIC) {

		pr_debug("Setting warm reset code and vector.\n");

		smpboot_setup_warm_reset_vector(start_ip);
		/*
                                            
  */
		if (APIC_INTEGRATED(apic_version[boot_cpu_physical_apicid])) {
			apic_write(APIC_ESR, 0);
			apic_read(APIC_ESR);
		}
	}

	/*
                                      
                                                       
              
                                                           
  */
	if (apic->wakeup_secondary_cpu)
		boot_error = apic->wakeup_secondary_cpu(apicid, start_ip);
	else
		boot_error = wakeup_cpu_via_init_nmi(cpu, start_ip, apicid,
						     &cpu0_nmi_registered);

	if (!boot_error) {
		/*
                                     
   */
		pr_debug("Before Callout %d\n", cpu);
		cpumask_set_cpu(cpu, cpu_callout_mask);
		pr_debug("After Callout %d\n", cpu);

		/*
                                 
   */
		for (timeout = 0; timeout < 50000; timeout++) {
			if (cpumask_test_cpu(cpu, cpu_callin_mask))
				break;	/*               */
			udelay(100);
			/*
                                                    
                                                 
                                                          
                                                  
    */
			schedule();
		}

		if (cpumask_test_cpu(cpu, cpu_callin_mask)) {
			print_cpu_msr(&cpu_data(cpu));
			pr_debug("CPU%d: has booted.\n", cpu);
		} else {
			boot_error = 1;
			if (*trampoline_status == 0xA5A5A5A5)
				/*                            */
				pr_err("CPU%d: Stuck ??\n", cpu);
			else
				/*                         */
				pr_err("CPU%d: Not responding\n", cpu);
			if (apic->inquire_remote_apic)
				apic->inquire_remote_apic(apicid);
		}
	}

	if (boot_error) {
		/*                                                     */
		numa_remove_cpu(cpu); /*                         */

		/*                          */
		cpumask_clear_cpu(cpu, cpu_callout_mask);

		/*                       */
		cpumask_clear_cpu(cpu, cpu_initialized_mask);

		set_cpu_present(cpu, false);
		per_cpu(x86_cpu_to_apicid, cpu) = BAD_APICID;
	}

	/*                                */
	*trampoline_status = 0;

	if (get_uv_system_type() != UV_NON_UNIQUE_APIC) {
		/*
                                      
   */
		smpboot_restore_warm_reset_vector();
	}
	/*
                                                                       
                                                     
  */
	if (cpu0_nmi_registered)
		unregister_nmi_handler(NMI_LOCAL, "wake_cpu0");

	return boot_error;
}

int __cpuinit native_cpu_up(unsigned int cpu, struct task_struct *tidle)
{
	int apicid = apic->cpu_present_to_apicid(cpu);
	unsigned long flags;
	int err;

	WARN_ON(irqs_disabled());

	pr_debug("++++++++++++++++++++=_---CPU UP  %u\n", cpu);

	if (apicid == BAD_APICID ||
	    !physid_isset(apicid, phys_cpu_present_map) ||
	    !apic->apic_id_valid(apicid)) {
		pr_err("%s: bad cpu %d\n", __func__, cpu);
		return -EINVAL;
	}

	/*
                       
  */
	if (cpumask_test_cpu(cpu, cpu_callin_mask)) {
		pr_debug("do_boot_cpu %d Already started\n", cpu);
		return -ENOSYS;
	}

	/*
                                                                   
                                                                     
  */
	mtrr_save_state();

	per_cpu(cpu_state, cpu) = CPU_UP_PREPARE;

	/*                                             */
	__cpu_disable_lazy_restore(cpu);

	err = do_boot_cpu(apicid, cpu, tidle);
	if (err) {
		pr_debug("do_boot_cpu failed %d\n", err);
		return -EIO;
	}

	/*
                                                             
                    
  */
	local_irq_save(flags);
	check_tsc_sync_source(cpu);
	local_irq_restore(flags);

	while (!cpu_online(cpu)) {
		cpu_relax();
		touch_nmi_watchdog();
	}

	return 0;
}

/* 
                                                                       
 */
void arch_disable_smp_support(void)
{
	disable_ioapic_support();
}

/*
                                          
  
                                                                          
 */
static __init void disable_smp(void)
{
	init_cpu_present(cpumask_of(0));
	init_cpu_possible(cpumask_of(0));
	smpboot_clear_io_apic_irqs();

	if (smp_found_config)
		physid_set_mask_of_physid(boot_cpu_physical_apicid, &phys_cpu_present_map);
	else
		physid_set_mask_of_physid(0, &phys_cpu_present_map);
	cpumask_set_cpu(0, cpu_sibling_mask(0));
	cpumask_set_cpu(0, cpu_core_mask(0));
}

/*
                         
 */
static int __init smp_sanity_check(unsigned max_cpus)
{
	preempt_disable();

#if !defined(CONFIG_X86_BIGSMP) && defined(CONFIG_X86_32)
	if (def_to_bigsmp && nr_cpu_ids > 8) {
		unsigned int cpu;
		unsigned nr;

		pr_warn("More than 8 CPUs detected - skipping them\n"
			"Use CONFIG_X86_BIGSMP\n");

		nr = 0;
		for_each_present_cpu(cpu) {
			if (nr >= 8)
				set_cpu_present(cpu, false);
			nr++;
		}

		nr = 0;
		for_each_possible_cpu(cpu) {
			if (nr >= 8)
				set_cpu_possible(cpu, false);
			nr++;
		}

		nr_cpu_ids = 8;
	}
#endif

	if (!physid_isset(hard_smp_processor_id(), phys_cpu_present_map)) {
		pr_warn("weird, boot CPU (#%d) not listed by the BIOS\n",
			hard_smp_processor_id());

		physid_set(hard_smp_processor_id(), phys_cpu_present_map);
	}

	/*
                                                          
                        
  */
	if (!smp_found_config && !acpi_lapic) {
		preempt_enable();
		pr_notice("SMP motherboard not detected\n");
		disable_smp();
		if (APIC_init_uniprocessor())
			pr_notice("Local APIC not detected. Using dummy APIC emulation.\n");
		return -1;
	}

	/*
                                                                     
                                                            
  */
	if (!apic->check_phys_apicid_present(boot_cpu_physical_apicid)) {
		pr_notice("weird, boot CPU (#%d) not listed by the BIOS\n",
			  boot_cpu_physical_apicid);
		physid_set(hard_smp_processor_id(), phys_cpu_present_map);
	}
	preempt_enable();

	/*
                                                               
  */
	if (APIC_INTEGRATED(apic_version[boot_cpu_physical_apicid]) &&
	    !cpu_has_apic) {
		if (!disable_apic) {
			pr_err("BIOS bug, local APIC #%d not detected!...\n",
				boot_cpu_physical_apicid);
			pr_err("... forcing use of dummy APIC emulation (tell your hw vendor)\n");
		}
		smpboot_clear_io_apic();
		disable_ioapic_support();
		return -1;
	}

	verify_local_APIC();

	/*
                                                      
  */
	if (!max_cpus) {
		pr_info("SMP mode deactivated\n");
		smpboot_clear_io_apic();

		connect_bsp_APIC();
		setup_local_APIC();
		bsp_end_local_APIC_setup();
		return -1;
	}

	return 0;
}

static void __init smp_cpu_index_default(void)
{
	int i;
	struct cpuinfo_x86 *c;

	for_each_possible_cpu(i) {
		c = &cpu_data(i);
		/*                     */
		c->cpu_index = nr_cpu_ids;
	}
}

/*
                                                              
                                                                    
 */
void __init native_smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int i;

	preempt_disable();
	smp_cpu_index_default();

	/*
                              
  */
	smp_store_boot_cpu_info(); /*                                */
	cpumask_copy(cpu_callin_mask, cpumask_of(0));
	mb();

	current_thread_info()->cpu = 0;  /*         */
	for_each_possible_cpu(i) {
		zalloc_cpumask_var(&per_cpu(cpu_sibling_map, i), GFP_KERNEL);
		zalloc_cpumask_var(&per_cpu(cpu_core_map, i), GFP_KERNEL);
		zalloc_cpumask_var(&per_cpu(cpu_llc_shared_map, i), GFP_KERNEL);
	}
	set_cpu_sibling_map(0);


	if (smp_sanity_check(max_cpus) < 0) {
		pr_info("SMP disabled\n");
		disable_smp();
		goto out;
	}

	default_setup_apic_routing();

	preempt_disable();
	if (read_apic_id() != boot_cpu_physical_apicid) {
		panic("Boot APIC ID in local APIC unexpected (%d vs %d)",
		     read_apic_id(), boot_cpu_physical_apicid);
		/*                                    */
	}
	preempt_enable();

	connect_bsp_APIC();

	/*
                                 
  */
	setup_local_APIC();

	if (x2apic_mode)
		cpu0_logical_apicid = apic_read(APIC_LDR);
	else
		cpu0_logical_apicid = GET_APIC_LOGICAL_ID(apic_read(APIC_LDR));

	/*
                                                 
  */
	if (!skip_ioapic_setup && nr_ioapics)
		enable_IO_APIC();

	bsp_end_local_APIC_setup();

	if (apic->setup_portio_remap)
		apic->setup_portio_remap();

	smpboot_setup_io_apic();
	/*
                                        
  */

	pr_info("CPU%d: ", 0);
	print_cpu_info(&cpu_data(0));
	x86_init.timers.setup_percpu_clockev();

	if (is_uv_system())
		uv_system_init();

	set_mtrr_aps_delayed_init();
out:
	preempt_enable();
}

void arch_enable_nonboot_cpus_begin(void)
{
	set_mtrr_aps_delayed_init();
}

void arch_enable_nonboot_cpus_end(void)
{
	mtrr_aps_init();
}

/*
                                   
 */
void __init native_smp_prepare_boot_cpu(void)
{
	int me = smp_processor_id();
	switch_to_new_gdt(me);
	/*                                                      */
	cpumask_set_cpu(me, cpu_callout_mask);
	per_cpu(cpu_state, me) = CPU_ONLINE;
}

void __init native_smp_cpus_done(unsigned int max_cpus)
{
	pr_debug("Boot done\n");

	nmi_selftest();
	impress_friends();
#ifdef CONFIG_X86_IO_APIC
	setup_ioapic_dest();
#endif
	mtrr_aps_init();
}

static int __initdata setup_possible_cpus = -1;
static int __init _setup_possible_cpus(char *str)
{
	get_option(&str, &setup_possible_cpus);
	return 0;
}
early_param("possible_cpus", _setup_possible_cpus);


/*
                                                                
                                                                  
                                                                 
                                                
                                                             
                                                                      
                                                   
              
  
                                                                
                                                                   
                                                     
                                             
                                                            
      
 */
__init void prefill_possible_map(void)
{
	int i, possible;

	/*                                   */
	if (!num_processors)
		num_processors = 1;

	i = setup_max_cpus ?: 1;
	if (setup_possible_cpus == -1) {
		possible = num_processors;
#ifdef CONFIG_HOTPLUG_CPU
		if (setup_max_cpus)
			possible += disabled_cpus;
#else
		if (possible > i)
			possible = i;
#endif
	} else
		possible = setup_possible_cpus;

	total_cpus = max_t(int, possible, num_processors + disabled_cpus);

	/*                                          */
	if (possible > nr_cpu_ids) {
		pr_warn("%d Processors exceeds NR_CPUS limit of %d\n",
			possible, nr_cpu_ids);
		possible = nr_cpu_ids;
	}

#ifdef CONFIG_HOTPLUG_CPU
	if (!setup_max_cpus)
#endif
	if (possible > i) {
		pr_warn("%d Processors exceeds max_cpus limit of %u\n",
			possible, setup_max_cpus);
		possible = i;
	}

	pr_info("Allowing %d CPUs, %d hotplug CPUs\n",
		possible, max_t(int, possible - num_processors, 0));

	for (i = 0; i < possible; i++)
		set_cpu_possible(i, true);
	for (; i < NR_CPUS; i++)
		set_cpu_possible(i, false);

	nr_cpu_ids = possible;
}

#ifdef CONFIG_HOTPLUG_CPU

static void remove_siblinginfo(int cpu)
{
	int sibling;
	struct cpuinfo_x86 *c = &cpu_data(cpu);

	for_each_cpu(sibling, cpu_core_mask(cpu)) {
		cpumask_clear_cpu(cpu, cpu_core_mask(sibling));
		/* 
                                                    
   */
		if (cpumask_weight(cpu_sibling_mask(cpu)) == 1)
			cpu_data(sibling).booted_cores--;
	}

	for_each_cpu(sibling, cpu_sibling_mask(cpu))
		cpumask_clear_cpu(cpu, cpu_sibling_mask(sibling));
	for_each_cpu(sibling, cpu_llc_shared_mask(cpu))
		cpumask_clear_cpu(cpu, cpu_llc_shared_mask(sibling));
	cpumask_clear(cpu_llc_shared_mask(cpu));
	cpumask_clear(cpu_sibling_mask(cpu));
	cpumask_clear(cpu_core_mask(cpu));
	c->phys_proc_id = 0;
	c->cpu_core_id = 0;
	cpumask_clear_cpu(cpu, cpu_sibling_setup_mask);
}

static void __ref remove_cpu_from_maps(int cpu)
{
	set_cpu_online(cpu, false);
	cpumask_clear_cpu(cpu, cpu_callout_mask);
	cpumask_clear_cpu(cpu, cpu_callin_mask);
	/*                       */
	cpumask_clear_cpu(cpu, cpu_initialized_mask);
	numa_remove_cpu(cpu);
}

void cpu_disable_common(void)
{
	int cpu = smp_processor_id();

	remove_siblinginfo(cpu);

	/*                                                            */
	lock_vector_lock();
	remove_cpu_from_maps(cpu);
	unlock_vector_lock();
	fixup_irqs();
}

int native_cpu_disable(void)
{
	clear_local_APIC();

	cpu_disable_common();
	return 0;
}

void native_cpu_die(unsigned int cpu)
{
	/*                                                              */
	unsigned int i;

	for (i = 0; i < 10; i++) {
		/*                                                */
		if (per_cpu(cpu_state, cpu) == CPU_DEAD) {
			if (system_state == SYSTEM_RUNNING)
				pr_info("CPU %u is now offline\n", cpu);
			return;
		}
		msleep(100);
	}
	pr_err("CPU %u didn't die...\n", cpu);
}

void play_dead_common(void)
{
	idle_task_exit();
	reset_lazy_tlbstate();
	amd_e400_remove_cpu(raw_smp_processor_id());

	mb();
	/*        */
	__this_cpu_write(cpu_state, CPU_DEAD);

	/*
                                                     
  */
	local_irq_disable();
}

static bool wakeup_cpu0(void)
{
	if (smp_processor_id() == 0 && enable_start_cpu0)
		return true;

	return false;
}

/*
                                                                  
                                                 
 */
static inline void mwait_play_dead(void)
{
	unsigned int eax, ebx, ecx, edx;
	unsigned int highest_cstate = 0;
	unsigned int highest_subcstate = 0;
	void *mwait_ptr;
	int i;

	if (!this_cpu_has(X86_FEATURE_MWAIT))
		return;
	if (!this_cpu_has(X86_FEATURE_CLFLSH))
		return;
	if (__this_cpu_read(cpu_info.cpuid_level) < CPUID_MWAIT_LEAF)
		return;

	eax = CPUID_MWAIT_LEAF;
	ecx = 0;
	native_cpuid(&eax, &ebx, &ecx, &edx);

	/*
                                                  
                                                                    
  */
	if (!(ecx & CPUID5_ECX_EXTENSIONS_SUPPORTED)) {
		eax = 0;
	} else {
		edx >>= MWAIT_SUBSTATE_SIZE;
		for (i = 0; i < 7 && edx; i++, edx >>= MWAIT_SUBSTATE_SIZE) {
			if (edx & MWAIT_SUBSTATE_MASK) {
				highest_cstate = i;
				highest_subcstate = edx & MWAIT_SUBSTATE_MASK;
			}
		}
		eax = (highest_cstate << MWAIT_SUBSTATE_SIZE) |
			(highest_subcstate - 1);
	}

	/*
                                                             
                                                           
                                                                    
  */
	mwait_ptr = &current_thread_info()->flags;

	wbinvd();

	while (1) {
		/*
                                                      
                                                         
                                                      
                                                          
                                          
   */
		clflush(mwait_ptr);
		__monitor(mwait_ptr, 0, 0);
		mb();
		__mwait(eax, 0);
		/*
                                              
   */
		if (wakeup_cpu0())
			start_cpu0();
	}
}

static inline void hlt_play_dead(void)
{
	if (__this_cpu_read(cpu_info.x86) >= 4)
		wbinvd();

	while (1) {
		native_halt();
		/*
                                              
   */
		if (wakeup_cpu0())
			start_cpu0();
	}
}

void native_play_dead(void)
{
	play_dead_common();
	tboot_shutdown(TB_SHUTDOWN_WFS);

	mwait_play_dead();	/*                         */
	if (cpuidle_play_dead())
		hlt_play_dead();
}

#else /*                         */
int native_cpu_disable(void)
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
