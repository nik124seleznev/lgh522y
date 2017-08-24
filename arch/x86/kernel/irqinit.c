#include <linux/linkage.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/timex.h>
#include <linux/random.h>
#include <linux/kprobes.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/device.h>
#include <linux/bitops.h>
#include <linux/acpi.h>
#include <linux/io.h>
#include <linux/delay.h>

#include <linux/atomic.h>
#include <asm/timer.h>
#include <asm/hw_irq.h>
#include <asm/pgtable.h>
#include <asm/desc.h>
#include <asm/apic.h>
#include <asm/setup.h>
#include <asm/i8259.h>
#include <asm/traps.h>
#include <asm/prom.h>

/*
                                                                    
                                                  
 */

/*
                                                                  
                                                                     
                                                               
                                                                   
                                                                
                     
  
                                                             
 */

/*
                                                           
 */
static struct irqaction irq2 = {
	.handler = no_action,
	.name = "cascade",
	.flags = IRQF_NO_THREAD,
};

DEFINE_PER_CPU(vector_irq_t, vector_irq) = {
	[0 ... NR_VECTORS - 1] = -1,
};

int vector_used_by_percpu_irq(unsigned int vector)
{
	int cpu;

	for_each_online_cpu(cpu) {
		if (per_cpu(vector_irq, cpu)[vector] != -1)
			return 1;
	}

	return 0;
}

void __init init_ISA_irqs(void)
{
	struct irq_chip *chip = legacy_pic->chip;
	const char *name = chip->name;
	int i;

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_LOCAL_APIC)
	init_bsp_APIC();
#endif
	legacy_pic->init(0);

	for (i = 0; i < legacy_pic->nr_legacy_irqs; i++)
		irq_set_chip_and_handler_name(i, chip, handle_level_irq, name);
}

void __init init_IRQ(void)
{
	int i;

	/*
                                                              
           
  */
	x86_add_irq_domains();

	/*
                                                              
                                                                        
                                                                    
                                                                     
                                                                      
                      
  */
	for (i = 0; i < legacy_pic->nr_legacy_irqs; i++)
		per_cpu(vector_irq, 0)[IRQ0_VECTOR + i] = i;

	x86_init.irqs.intr_init();
}

/*
                                    
 */
void setup_vector_irq(int cpu)
{
#ifndef CONFIG_X86_IO_APIC
	int irq;

	/*
                                                                       
                                                                      
                                                                    
                                                                       
                                 
  */
	for (irq = 0; irq < legacy_pic->nr_legacy_irqs; irq++)
		per_cpu(vector_irq, cpu)[IRQ0_VECTOR + irq] = irq;
#endif

	__setup_vector_irq(cpu);
}

static void __init smp_intr_init(void)
{
#ifdef CONFIG_SMP
#if defined(CONFIG_X86_64) || defined(CONFIG_X86_LOCAL_APIC)
	/*
                                                              
                          
  */
	alloc_intr_gate(RESCHEDULE_VECTOR, reschedule_interrupt);

	/*                               */
	alloc_intr_gate(CALL_FUNCTION_VECTOR, call_function_interrupt);

	/*                                      */
	alloc_intr_gate(CALL_FUNCTION_SINGLE_VECTOR,
			call_function_single_interrupt);

	/*                                                 */
	set_intr_gate(IRQ_MOVE_CLEANUP_VECTOR, irq_move_cleanup_interrupt);
	set_bit(IRQ_MOVE_CLEANUP_VECTOR, used_vectors);

	/*                                 */
	alloc_intr_gate(REBOOT_VECTOR, reboot_interrupt);
#endif
#endif /*            */
}

static void __init apic_intr_init(void)
{
	smp_intr_init();

#ifdef CONFIG_X86_THERMAL_VECTOR
	alloc_intr_gate(THERMAL_APIC_VECTOR, thermal_interrupt);
#endif
#ifdef CONFIG_X86_MCE_THRESHOLD
	alloc_intr_gate(THRESHOLD_APIC_VECTOR, threshold_interrupt);
#endif

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_LOCAL_APIC)
	/*                                         */
	alloc_intr_gate(LOCAL_TIMER_VECTOR, apic_timer_interrupt);

	/*                                   */
	alloc_intr_gate(X86_PLATFORM_IPI_VECTOR, x86_platform_ipi);
#ifdef CONFIG_HAVE_KVM
	/*                                         */
	alloc_intr_gate(POSTED_INTR_VECTOR, kvm_posted_intr_ipi);
#endif

	/*                                                    */
	alloc_intr_gate(SPURIOUS_APIC_VECTOR, spurious_interrupt);
	alloc_intr_gate(ERROR_APIC_VECTOR, error_interrupt);

	/*                      */
# ifdef CONFIG_IRQ_WORK
	alloc_intr_gate(IRQ_WORK_VECTOR, irq_work_interrupt);
# endif

#endif
}

void __init native_init_IRQ(void)
{
	int i;

	/*                                                           */
	x86_init.irqs.pre_vector_init();

	apic_intr_init();

	/*
                                                      
                                                    
                             
  */
	i = FIRST_EXTERNAL_VECTOR;
	for_each_clear_bit_from(i, used_vectors, NR_VECTORS) {
		/*                                                         */
		set_intr_gate(i, interrupt[i - FIRST_EXTERNAL_VECTOR]);
	}

	if (!acpi_ioapic && !of_ioapic)
		setup_irq(2, &irq2);

#ifdef CONFIG_X86_32
	irq_ctx_init(smp_processor_id());
#endif
}
