/*
 * Copyright (C) 2009 Extreme Engineering Solutions, Inc.
 *
 * X-ES board-specific functionality
 *
 * Based on mpc85xx_ds code from Freescale Semiconductor, Inc.
 *
 * Author: Nate Case <ncase@xes-inc.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>

#include <asm/time.h>
#include <asm/machdep.h>
#include <asm/pci-bridge.h>
#include <mm/mmu_decl.h>
#include <asm/prom.h>
#include <asm/udbg.h>
#include <asm/mpic.h>

#include <sysdev/fsl_soc.h>
#include <sysdev/fsl_pci.h>
#include "smp.h"

#include "mpc85xx.h"

/*                                                        */
#define MPC85xx_L2CTL_L2E		0x80000000 /*           */
#define MPC85xx_L2CTL_L2I		0x40000000 /*                     */
#define MPC85xx_L2CTL_L2SIZ_MASK	0x30000000 /*                    */

void __init xes_mpc85xx_pic_init(void)
{
	struct mpic *mpic = mpic_alloc(NULL, 0, MPIC_BIG_ENDIAN,
			0, 256, " OpenPIC  ");
	BUG_ON(mpic == NULL);
	mpic_init(mpic);
}

static void xes_mpc85xx_configure_l2(void __iomem *l2_base)
{
	volatile uint32_t ctl, tmp;

	asm volatile("msync; isync");
	tmp = in_be32(l2_base);

	/*
                                                                  
                                          
  */
	printk(KERN_INFO "xes_mpc85xx: Enabling L2 as cache\n");

	ctl = MPC85xx_L2CTL_L2E | MPC85xx_L2CTL_L2I;
	if (of_machine_is_compatible("MPC8540") ||
	    of_machine_is_compatible("MPC8560"))
		/*
                                                   
                                                  
   */
		ctl |= (tmp & MPC85xx_L2CTL_L2SIZ_MASK) >> 2;

	asm volatile("msync; isync");
	out_be32(l2_base, ctl);
	asm volatile("msync; isync");
}

static void xes_mpc85xx_fixups(void)
{
	struct device_node *np;
	int err;

	/*
                                                               
                                                         
  */
	for_each_node_by_name(np, "l2-cache-controller") {
		struct resource r[2];
		void __iomem *l2_base;

		/*                                                        */
		if (!of_device_is_compatible(np,
				    "fsl,mpc8548-l2-cache-controller") &&
		    !of_device_is_compatible(np,
				    "fsl,mpc8540-l2-cache-controller") &&
		    !of_device_is_compatible(np,
				    "fsl,mpc8560-l2-cache-controller"))
			continue;

		err = of_address_to_resource(np, 0, &r[0]);
		if (err) {
			printk(KERN_WARNING "xes_mpc85xx: Could not get "
			       "resource for device tree node '%s'",
			       np->full_name);
			continue;
		}

		l2_base = ioremap(r[0].start, resource_size(&r[0]));

		xes_mpc85xx_configure_l2(l2_base);
	}
}

/*
                         
 */
static void __init xes_mpc85xx_setup_arch(void)
{
	struct device_node *root;
	const char *model = "Unknown";

	root = of_find_node_by_path("/");
	if (root == NULL)
		return;

	model = of_get_property(root, "model", NULL);

	printk(KERN_INFO "X-ES MPC85xx-based single-board computer: %s\n",
	       model + strlen("xes,"));

	xes_mpc85xx_fixups();

	mpc85xx_smp_init();

	fsl_pci_assign_primary();
}

machine_arch_initcall(xes_mpc8572, mpc85xx_common_publish_devices);
machine_arch_initcall(xes_mpc8548, mpc85xx_common_publish_devices);
machine_arch_initcall(xes_mpc8540, mpc85xx_common_publish_devices);

/*
                                                   
 */
static int __init xes_mpc8572_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	return of_flat_dt_is_compatible(root, "xes,MPC8572");
}

static int __init xes_mpc8548_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	return of_flat_dt_is_compatible(root, "xes,MPC8548");
}

static int __init xes_mpc8540_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	return of_flat_dt_is_compatible(root, "xes,MPC8540");
}

define_machine(xes_mpc8572) {
	.name			= "X-ES MPC8572",
	.probe			= xes_mpc8572_probe,
	.setup_arch		= xes_mpc85xx_setup_arch,
	.init_IRQ		= xes_mpc85xx_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
#endif
	.get_irq		= mpic_get_irq,
	.restart		= fsl_rstcr_restart,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};

define_machine(xes_mpc8548) {
	.name			= "X-ES MPC8548",
	.probe			= xes_mpc8548_probe,
	.setup_arch		= xes_mpc85xx_setup_arch,
	.init_IRQ		= xes_mpc85xx_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
#endif
	.get_irq		= mpic_get_irq,
	.restart		= fsl_rstcr_restart,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};

define_machine(xes_mpc8540) {
	.name			= "X-ES MPC8540",
	.probe			= xes_mpc8540_probe,
	.setup_arch		= xes_mpc85xx_setup_arch,
	.init_IRQ		= xes_mpc85xx_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
#endif
	.get_irq		= mpic_get_irq,
	.restart		= fsl_rstcr_restart,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};
