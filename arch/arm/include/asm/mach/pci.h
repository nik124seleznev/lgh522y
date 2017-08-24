/*
 *  arch/arm/include/asm/mach/pci.h
 *
 *  Copyright (C) 2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_MACH_PCI_H
#define __ASM_MACH_PCI_H

#include <linux/ioport.h>

struct pci_sys_data;
struct pci_ops;
struct pci_bus;

struct hw_pci {
#ifdef CONFIG_PCI_DOMAINS
	int		domain;
#endif
	struct pci_ops	*ops;
	int		nr_controllers;
	void		**private_data;
	int		(*setup)(int nr, struct pci_sys_data *);
	struct pci_bus *(*scan)(int nr, struct pci_sys_data *);
	void		(*preinit)(void);
	void		(*postinit)(void);
	u8		(*swizzle)(struct pci_dev *dev, u8 *pin);
	int		(*map_irq)(const struct pci_dev *dev, u8 slot, u8 pin);
	resource_size_t (*align_resource)(struct pci_dev *dev,
					  const struct resource *res,
					  resource_size_t start,
					  resource_size_t size,
					  resource_size_t align);
};

/*
                           
 */
struct pci_sys_data {
#ifdef CONFIG_PCI_DOMAINS
	int		domain;
#endif
	struct list_head node;
	int		busnr;		/*                      */
	u64		mem_offset;	/*                                */
	unsigned long	io_offset;	/*                             */
	struct pci_bus	*bus;		/*            */
	struct list_head resources;	/*                                      */
	struct resource io_res;
	char		io_res_name[12];
					/*                    */
	u8		(*swizzle)(struct pci_dev *, u8 *);
					/*                */
	int		(*map_irq)(const struct pci_dev *, u8, u8);
					/*                                  */
	resource_size_t (*align_resource)(struct pci_dev *dev,
					  const struct resource *res,
					  resource_size_t start,
					  resource_size_t size,
					  resource_size_t align);
	void		*private_data;	/*                                  */
};

/*
                                                                  
 */
void pci_common_init(struct hw_pci *);

/*
                                 
 */
#if defined(CONFIG_PCI)
extern void pci_map_io_early(unsigned long pfn);
#else
static inline void pci_map_io_early(unsigned long pfn) {}
#endif

/*
                  
 */
extern struct pci_ops iop3xx_ops;
extern int iop3xx_pci_setup(int nr, struct pci_sys_data *);
extern void iop3xx_pci_preinit(void);
extern void iop3xx_pci_preinit_cond(void);

extern struct pci_ops dc21285_ops;
extern int dc21285_setup(int nr, struct pci_sys_data *);
extern void dc21285_preinit(void);
extern void dc21285_postinit(void);

extern struct pci_ops via82c505_ops;
extern int via82c505_setup(int nr, struct pci_sys_data *);
extern void via82c505_init(void *sysdata);

extern struct pci_ops pci_v3_ops;
extern int pci_v3_setup(int nr, struct pci_sys_data *);
extern void pci_v3_preinit(void);
extern void pci_v3_postinit(void);

#endif /*                  */
