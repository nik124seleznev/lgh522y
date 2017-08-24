/*
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Author: Joerg Roedel <joerg.roedel@amd.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * This header file contains stuff that is shared between different interrupt
 * remapping drivers but with no need to be visible outside of the IOMMU layer.
 */

#ifndef __IRQ_REMAPPING_H
#define __IRQ_REMAPPING_H

#ifdef CONFIG_IRQ_REMAP

struct IO_APIC_route_entry;
struct io_apic_irq_attr;
struct irq_data;
struct cpumask;
struct pci_dev;
struct msi_msg;

extern int disable_irq_remap;
extern int irq_remap_broken;
extern int disable_sourceid_checking;
extern int no_x2apic_optout;
extern int irq_remapping_enabled;

struct irq_remap_ops {
	/*                                                */
	int (*supported)(void);

	/*                                                                  */
	int  (*prepare)(void);

	/*                                */
	int  (*enable)(void);

	/*                                 */
	void (*disable)(void);

	/*                                  */
	int  (*reenable)(int);

	/*                       */
	int  (*enable_faulting)(void);

	/*                       */
	int (*setup_ioapic_entry)(int irq, struct IO_APIC_route_entry *,
				  unsigned int, int,
				  struct io_apic_irq_attr *);

	/*                                              */
	int (*set_affinity)(struct irq_data *data, const struct cpumask *mask,
			    bool force);

	/*             */
	int (*free_irq)(int);

	/*                                               */
	void (*compose_msi_msg)(struct pci_dev *,
				unsigned int, unsigned int,
				struct msi_msg *, u8);

	/*                                      */
	int (*msi_alloc_irq)(struct pci_dev *, int, int);

	/*                            */
	int (*msi_setup_irq)(struct pci_dev *, unsigned int, int, int);

	/*                                           */
	int (*setup_hpet_msi)(unsigned int, unsigned int);
};

extern struct irq_remap_ops intel_irq_remap_ops;
extern struct irq_remap_ops amd_iommu_irq_ops;

#else  /*                  */

#define irq_remapping_enabled 0
#define disable_irq_remap     1
#define irq_remap_broken      0

#endif /*                  */

#endif /*                   */
