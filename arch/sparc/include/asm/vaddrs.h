#ifndef _SPARC_VADDRS_H
#define _SPARC_VADDRS_H

#include <asm/head.h>

/*
 * asm/vaddrs.h:  Here we define the virtual addresses at
 *                      which important things will be mapped.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 * Copyright (C) 2000 Anton Blanchard (anton@samba.org)
 */

#define SRMMU_MAXMEM		0x0c000000

#define SRMMU_NOCACHE_VADDR	(KERNBASE + SRMMU_MAXMEM)
				/*               */
/*                                              */
#define SRMMU_MIN_NOCACHE_PAGES (550)
#define SRMMU_MAX_NOCACHE_PAGES	(1280)

/*                                                                       
                                                                      
  
                                              
                                              
  
                                               
                                 
 */
#define SRMMU_NOCACHE_ALCRATIO	64	/*                                  */

#ifndef __ASSEMBLY__
#include <asm/kmap_types.h>

enum fixed_addresses {
	FIX_HOLE,
#ifdef CONFIG_HIGHMEM
	FIX_KMAP_BEGIN,
	FIX_KMAP_END = (KM_TYPE_NR * NR_CPUS),
#endif
	__end_of_fixed_addresses
};
#endif

/*                                                        
                         
 */
#define FIXADDR_TOP		(0xfcfff000UL)
#define FIXADDR_SIZE		((FIX_KMAP_END + 1) << PAGE_SHIFT)
#define FIXADDR_START		(FIXADDR_TOP - FIXADDR_SIZE)

#define __fix_to_virt(x)        (FIXADDR_TOP - ((x) << PAGE_SHIFT))

#define SUN4M_IOBASE_VADDR	0xfd000000 /*                        */
#define IOBASE_VADDR		0xfe000000
#define IOBASE_END		0xfe600000

#define KADB_DEBUGGER_BEGVM	0xffc00000 /*                                    */
#define KADB_DEBUGGER_ENDVM	0xffd00000
#define DEBUG_FIRSTVADDR	KADB_DEBUGGER_BEGVM
#define DEBUG_LASTVADDR		KADB_DEBUGGER_ENDVM

#define LINUX_OPPROM_BEGVM	0xffd00000
#define LINUX_OPPROM_ENDVM	0xfff00000

#define DVMA_VADDR		0xfff00000 /*                               */
#define DVMA_END		0xfffc0000

#endif /*                    */
