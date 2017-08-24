/*
 * Copyright (C) 2011-12 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/irqdomain.h>
#include <linux/irqchip.h>
#include "../../drivers/irqchip/irqchip.h"
#include <asm/sections.h>
#include <asm/irq.h>
#include <asm/mach_desc.h>

/*
                                          
                                                                     
                                              
                                                          
  
                 
                                  
                                                                  
 */
void __cpuinit arc_init_IRQ(void)
{
	int level_mask = 0;

	/*                                                  */
	write_aux_reg(AUX_IENABLE, 0);

       /*                                                                 */
#ifdef CONFIG_ARC_IRQ3_LV2
	level_mask |= (1 << 3);
#endif
#ifdef CONFIG_ARC_IRQ5_LV2
	level_mask |= (1 << 5);
#endif
#ifdef CONFIG_ARC_IRQ6_LV2
	level_mask |= (1 << 6);
#endif

	if (level_mask) {
		pr_info("Level-2 interrupts bitset %x\n", level_mask);
		write_aux_reg(AUX_IRQ_LEV, level_mask);
	}
}

/*
                                                        
                          
                                     
                                        
  
                                                                            
                                                                           
                  
 */

static void arc_mask_irq(struct irq_data *data)
{
	arch_mask_irq(data->irq);
}

static void arc_unmask_irq(struct irq_data *data)
{
	arch_unmask_irq(data->irq);
}

static struct irq_chip onchip_intc = {
	.name           = "ARC In-core Intc",
	.irq_mask	= arc_mask_irq,
	.irq_unmask	= arc_unmask_irq,
};

static int arc_intc_domain_map(struct irq_domain *d, unsigned int irq,
				irq_hw_number_t hw)
{
	if (irq == TIMER0_IRQ)
		irq_set_chip_and_handler(irq, &onchip_intc, handle_percpu_irq);
	else
		irq_set_chip_and_handler(irq, &onchip_intc, handle_level_irq);

	return 0;
}

static const struct irq_domain_ops arc_intc_domain_ops = {
	.xlate = irq_domain_xlate_onecell,
	.map = arc_intc_domain_map,
};

static struct irq_domain *root_domain;

static int __init
init_onchip_IRQ(struct device_node *intc, struct device_node *parent)
{
	if (parent)
		panic("DeviceTree incore intc not a root irq controller\n");

	root_domain = irq_domain_add_legacy(intc, NR_CPU_IRQS, 0, 0,
					    &arc_intc_domain_ops, NULL);

	if (!root_domain)
		panic("root irq domain not avail\n");

	/*                                               */
	irq_set_default_host(root_domain);

	return 0;
}

IRQCHIP_DECLARE(arc_intc, "snps,arc700-intc", init_onchip_IRQ);

/*
                                                                        
  
                                                                        
                         
 */
void __init init_IRQ(void)
{
	/*                                     */
	if (machine_desc->init_irq)
		machine_desc->init_irq();

	/*                                             */
	irqchip_init();

#ifdef CONFIG_SMP
	/*                                            */
	if (machine_desc->init_smp)
		machine_desc->init_smp(smp_processor_id());
#endif
}

/*
                                                                        
                                                                 
 */
void arch_do_IRQ(unsigned int irq, struct pt_regs *regs)
{
	struct pt_regs *old_regs = set_irq_regs(regs);

	irq_enter();
	generic_handle_irq(irq);
	irq_exit();
	set_irq_regs(old_regs);
}

int __init get_hw_config_num_irq(void)
{
	uint32_t val = read_aux_reg(ARC_REG_VECBASE_BCR);

	switch (val & 0x03) {
	case 0:
		return 16;
	case 1:
		return 32;
	case 2:
		return 8;
	default:
		return 0;
	}

	return 0;
}

/*
                                             
  
                                               
                                                                  
                                    
  
                                                                          
  
                                                                    
                                                     
                                                                            
                                                                          
                                                             
                                                                         
                                     
                                                                         
                                                 
                                                  
                                                        
                                                                                
                                                                      
 */

#ifdef CONFIG_ARC_COMPACT_IRQ_LEVELS	/*                                  */

void arch_local_irq_enable(void)
{

	unsigned long flags;
	flags = arch_local_save_flags();

	/*                                   */
	flags |= (STATUS_E1_MASK | STATUS_E2_MASK);

	/*                                                       */
	if (in_irq()) {

		/*                                                           
                                                                 
                                                                
                                                            
   */
		if (flags & STATUS_A2_MASK)
			flags &= ~(STATUS_E1_MASK | STATUS_E2_MASK);

		/*                                               */
		else if (flags & STATUS_A1_MASK)
			flags &= ~(STATUS_E1_MASK);
	}

	/*                                                               */

	else if (in_softirq()) {

		/*                                                 
                                                       
                                                                
                         
                                                             
                                          
    
                                      
                                  
                                                         
                                                        
   */
		struct pt_regs *pt = get_irq_regs();
		if ((flags & STATUS_A2_MASK) && pt &&
		    (pt->status32 & STATUS_A1_MASK)) {
			/*                                             */
			flags &= ~(STATUS_E1_MASK);
		}
	}

	arch_local_irq_restore(flags);
}

#else /*                                 */

/*
                                                
                                        
 */
void arch_local_irq_enable(void)
{
	unsigned long flags;

	/*
                                                               
                                 
  */
	if (in_irq()) {
		WARN_ONCE(1, "IRQ enabled from hard-isr");
		return;
	}

	flags = arch_local_save_flags();
	flags |= (STATUS_E1_MASK | STATUS_E2_MASK);
	arch_local_irq_restore(flags);
}
#endif
EXPORT_SYMBOL(arch_local_irq_enable);
