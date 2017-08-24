/*
 *	pci.h
 *
 *	PCI defines and function prototypes
 *	Copyright 1994, Drew Eckhardt
 *	Copyright 1997--1999 Martin Mares <mj@ucw.cz>
 *
 *	For more information, please consult the following manuals (look at
 *	http://www.pcisig.com/ for how to get them):
 *
 *	PCI BIOS Specification
 *	PCI Local Bus Specification
 *	PCI to PCI Bridge Specification
 *	PCI System Design Guide
 */

#ifndef _UAPILINUX_PCI_H
#define _UAPILINUX_PCI_H

#include <linux/pci_regs.h>	/*                          */

/*
                                                                 
                                                                
                               
  
             
                 
 */
#define PCI_DEVFN(slot, func)	((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(devfn)		(((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)		((devfn) & 0x07)

/*                                     */
#define PCIIOC_BASE		('P' << 24 | 'C' << 16 | 'I' << 8)
#define PCIIOC_CONTROLLER	(PCIIOC_BASE | 0x00)	/*                                */
#define PCIIOC_MMAP_IS_IO	(PCIIOC_BASE | 0x01)	/*                              */
#define PCIIOC_MMAP_IS_MEM	(PCIIOC_BASE | 0x02)	/*                              */
#define PCIIOC_WRITE_COMBINE	(PCIIOC_BASE | 0x03)	/*                                 */

#endif /*                  */
