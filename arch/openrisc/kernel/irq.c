/*
 * OpenRISC irq.c
 *
 * Linux architectural port borrowing liberally from similar works of
 * others.  All original copyrights apply as per the original source
 * declaration.
 *
 * Modifications for the OpenRISC architecture:
 * Copyright (C) 2010-2011 Jonas Bonn <jonas@southpole.se>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/of.h>
#include <linux/ftrace.h>
#include <linux/irq.h>
#include <linux/export.h>
#include <linux/irqdomain.h>
#include <linux/irqflags.h>

/*                               */
unsigned long arch_local_save_flags(void)
{
	return mfspr(SPR_SR) & (SPR_SR_IEE|SPR_SR_TEE);
}
EXPORT_SYMBOL(arch_local_save_flags);

/*                              */
void arch_local_irq_restore(unsigned long flags)
{
	mtspr(SPR_SR, ((mfspr(SPR_SR) & ~(SPR_SR_IEE|SPR_SR_TEE)) | flags));
}
EXPORT_SYMBOL(arch_local_irq_restore);


/*                         */

/*                                                                      
                         
 */

static void or1k_pic_mask(struct irq_data *data)
{
	mtspr(SPR_PICMR, mfspr(SPR_PICMR) & ~(1UL << data->hwirq));
}

static void or1k_pic_unmask(struct irq_data *data)
{
	mtspr(SPR_PICMR, mfspr(SPR_PICMR) | (1UL << data->hwirq));
}

static void or1k_pic_ack(struct irq_data *data)
{
	/*                                                              
              
                                                                 
                                                               
                                           
  */

	/*                                                               
                                                                  
                    
  */

#ifdef CONFIG_OR1K_1200
	/*                                                           
                                                                     
                                                                 
                                                 
  */

	mtspr(SPR_PICSR, mfspr(SPR_PICSR) & ~(1UL << data->hwirq));
#else
	WARN(1, "Interrupt handling possibly broken\n");
	mtspr(SPR_PICSR, (1UL << data->hwirq));
#endif
}

static void or1k_pic_mask_ack(struct irq_data *data)
{
	/*                                      */

#ifdef CONFIG_OR1K_1200
	mtspr(SPR_PICMR, mfspr(SPR_PICMR) & ~(1UL << data->hwirq));
	mtspr(SPR_PICSR, mfspr(SPR_PICSR) & ~(1UL << data->hwirq));
#else
	WARN(1, "Interrupt handling possibly broken\n");
	mtspr(SPR_PICMR, (1UL << data->hwirq));
	mtspr(SPR_PICSR, (1UL << data->hwirq));
#endif
}

#if 0
static int or1k_pic_set_type(struct irq_data *data, unsigned int flow_type)
{
	/*                                                             
                                                            
                                                              
                */

	return irq_setup_alt_chip(data, flow_type);
}
#endif

static struct irq_chip or1k_dev = {
	.name = "or1k-PIC",
	.irq_unmask = or1k_pic_unmask,
	.irq_mask = or1k_pic_mask,
	.irq_ack = or1k_pic_ack,
	.irq_mask_ack = or1k_pic_mask_ack,
};

static struct irq_domain *root_domain;

static inline int pic_get_irq(int first)
{
	int hwirq;

	hwirq = ffs(mfspr(SPR_PICSR) >> first);
	if (!hwirq)
		return NO_IRQ;
	else
		hwirq = hwirq + first -1;

	return irq_find_mapping(root_domain, hwirq);
}


static int or1k_map(struct irq_domain *d, unsigned int irq, irq_hw_number_t hw)
{
	irq_set_chip_and_handler_name(irq, &or1k_dev,
				      handle_level_irq, "level");
	irq_set_status_flags(irq, IRQ_LEVEL | IRQ_NOPROBE);

	return 0;
}

static const struct irq_domain_ops or1k_irq_domain_ops = {
	.xlate = irq_domain_xlate_onecell,
	.map = or1k_map,
};

/*
                                                                   
                                                                   
                                                 
 */
static void __init or1k_irq_init(void)
{
	struct device_node *intc = NULL;

	/*                                                   */
	intc = of_find_compatible_node(NULL, NULL, "opencores,or1k-pic");
	BUG_ON(!intc);

	/*                                                   */
	mtspr(SPR_PICMR, (0UL));

	root_domain = irq_domain_add_linear(intc, 32,
					    &or1k_irq_domain_ops, NULL);
}

void __init init_IRQ(void)
{
	or1k_irq_init();
}

void __irq_entry do_IRQ(struct pt_regs *regs)
{
	int irq = -1;
	struct pt_regs *old_regs = set_irq_regs(regs);

	irq_enter();

	while ((irq = pic_get_irq(irq + 1)) != NO_IRQ)
		generic_handle_irq(irq);

	irq_exit();
	set_irq_regs(old_regs);
}
