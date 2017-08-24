/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

#ifndef _ASM_TILE_PCI_H
#define _ASM_TILE_PCI_H

#include <linux/dma-mapping.h>
#include <linux/pci.h>
#include <linux/numa.h>
#include <asm-generic/pci_iomap.h>

#ifndef __tilegx__

/*
                                              
 */
struct pci_controller {
	int index;		/*                   */
	struct pci_bus *root_bus;

	int first_busno;
	int last_busno;

	int hv_cfg_fd[2];	/*                                          */
	int hv_mem_fd;		/*                                      */

	struct pci_ops *ops;

	int irq_base;		/*                              */
	int plx_gen1;		/*                                  */

	/*                                                           */
	struct resource mem_resources[3];
};

/*
                                                           
                                                 
 */
extern int tile_plx_gen1;

static inline void pci_iounmap(struct pci_dev *dev, void __iomem *addr) {}

#define	TILE_NUM_PCIE	2

/*
                                                                     
                                                                  
                                                              
 */
#define PCI_DMA_BUS_IS_PHYS     1

/*                   */
#include <asm-generic/pci.h>

#else

#include <asm/page.h>
#include <gxio/trio.h>

/* 
                                                                              
                                                                            
                                                                          
                                                                            
                                                                             
          
 */
#define PCIE_HOST_BAR0_SIZE		HPAGE_SIZE
#define PCIE_HOST_BAR0_START		HPAGE_MASK

/* 
                                                                 
                             
 */
#define PCIE_HOST_REGS_SIZE		PAGE_SIZE

/*
                                                                     
                                                                    
                                                                         
 */
#define	MEM_MAP_INTR_REGIONS_BASE	(HPAGE_MASK - HPAGE_SIZE)

/*
                                              
 */
#define	MEM_MAP_INTR_REGION_SIZE	(1 << TRIO_MAP_MEM_LIM__ADDR_SHIFT)

/*
                                               
 */
#define	TILE_PCI_BAR_WINDOW_TOP		(1ULL << 32)

/*
                                       
 */
#define	TILE_PCI_BAR_WINDOW_SIZE	(1 << 30)

/*
                                                                 
                                                             
                                                                
 */
#define	TILE_PCI_MAX_DIRECT_DMA_ADDRESS \
	(TILE_PCI_BAR_WINDOW_TOP - TILE_PCI_BAR_WINDOW_SIZE - 1)

/*
                                                                            
                                                                          
                                                                
                                                                 
                                                               
                                                                
       
 */
#define	TILE_PCI_MEM_MAP_BASE_OFFSET	(1ULL << CHIP_PA_WIDTH())

/*
                                                                   
                                       
 */
#define	TILE_PCI_MEM_START	(1ULL << CHIP_PA_WIDTH())

/*
                                                     
 */
struct pci_controller {

	/*                                                               */
	gxio_trio_context_t *trio;
	int mac;		/*                                 */
	int trio_index;		/*                                           */

	int pio_mem_index;	/*                                    */

	/*
                                                                    
                                                        
  */
	int mem_maps[MAX_NUMNODES];

	int index;		/*                   */
	struct pci_bus *root_bus;

	/*                                                */
	struct resource mem_space;
	char mem_space_name[32];

	uint64_t mem_offset;	/*                                 */

	int first_busno;

	struct pci_ops *ops;

	/*                                                        */
	int irq_intx_table[4];

	/*                                                           */
	struct resource mem_resources[3];
};

extern struct pci_controller pci_controllers[TILEGX_NUM_TRIO * TILEGX_TRIO_PCIES];
extern gxio_trio_context_t trio_contexts[TILEGX_NUM_TRIO];

extern void pci_iounmap(struct pci_dev *dev, void __iomem *);

/*
                                                                   
                                                                    
                                       
 */
#define PCI_DMA_BUS_IS_PHYS     0

#endif /*            */

int __init tile_pci_init(void);
int __init pcibios_init(void);

void pcibios_fixup_bus(struct pci_bus *bus);

#define pci_domain_nr(bus) (((struct pci_controller *)(bus)->sysdata)->index)

/*
                                                              
 */
static inline int pci_proc_domain(struct pci_bus *bus)
{
	return 1;
}

/*
                                                                   
                                                                    
                                                               
 */
static inline int pcibios_assign_all_busses(void)
{
	return 1;
}

#define PCIBIOS_MIN_MEM		0
#define PCIBIOS_MIN_IO		0

/*                      */
#define cpumask_of_pcibus(bus) cpu_online_mask

/*                                                                    */
#include <asm-generic/pci-dma-compat.h>

#endif /*                 */
