/*
 * Copyright (C) 1998 Ingo Molnar
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

#ifndef _ASM_TILE_FIXMAP_H
#define _ASM_TILE_FIXMAP_H

#include <asm/page.h>

#ifndef __ASSEMBLY__
#include <linux/kernel.h>
#ifdef CONFIG_HIGHMEM
#include <linux/threads.h>
#include <asm/kmap_types.h>
#endif

#define __fix_to_virt(x)	(FIXADDR_TOP - ((x) << PAGE_SHIFT))
#define __virt_to_fix(x)	((FIXADDR_TOP - ((x)&PAGE_MASK)) >> PAGE_SHIFT)

/*
                                                        
                                                        
                                                     
                                                           
                                                       
                                               
                                                 
                                        
  
                                                    
                                                            
                                                       
                                       
  
                                                         
                 
 */
enum fixed_addresses {
#ifdef __tilegx__
	/*
                                                               
                                                              
  */
	FIX_HOLE,
#endif
#ifdef CONFIG_HIGHMEM
	FIX_KMAP_BEGIN,	/*                                              */
	FIX_KMAP_END = FIX_KMAP_BEGIN+(KM_TYPE_NR*NR_CPUS)-1,
#endif
#ifdef __tilegx__  /*                 */
	FIX_HOMECACHE_BEGIN,
	FIX_HOMECACHE_END = FIX_HOMECACHE_BEGIN+(NR_CPUS)-1,
#endif
	__end_of_permanent_fixed_addresses,

	/*
                                                                      
                                                  
  */
#define NR_FIX_BTMAPS	0
#if NR_FIX_BTMAPS
	FIX_BTMAP_END = __end_of_permanent_fixed_addresses,
	FIX_BTMAP_BEGIN = FIX_BTMAP_END + NR_FIX_BTMAPS - 1,
	__end_of_fixed_addresses
#else
	__end_of_fixed_addresses = __end_of_permanent_fixed_addresses
#endif
};

extern void __set_fixmap(enum fixed_addresses idx,
			 unsigned long phys, pgprot_t flags);

#define set_fixmap(idx, phys) \
		__set_fixmap(idx, phys, PAGE_KERNEL)
#define clear_fixmap(idx) \
		__set_fixmap(idx, 0, __pgprot(0))

#define __FIXADDR_SIZE	(__end_of_permanent_fixed_addresses << PAGE_SHIFT)
#define __FIXADDR_BOOT_SIZE	(__end_of_fixed_addresses << PAGE_SHIFT)
#define FIXADDR_START		(FIXADDR_TOP + PAGE_SIZE - __FIXADDR_SIZE)
#define FIXADDR_BOOT_START	(FIXADDR_TOP + PAGE_SIZE - __FIXADDR_BOOT_SIZE)

extern void __this_fixmap_does_not_exist(void);

/*
                                                                 
                                                                      
                                                                  
 */
static __always_inline unsigned long fix_to_virt(const unsigned int idx)
{
	/*
                                                          
                                                          
                                                          
                          
   
                                                       
                                                  
  */
	if (idx >= __end_of_fixed_addresses)
		__this_fixmap_does_not_exist();

	return __fix_to_virt(idx);
}

static inline unsigned long virt_to_fix(const unsigned long vaddr)
{
	BUG_ON(vaddr >= FIXADDR_TOP || vaddr < FIXADDR_START);
	return __virt_to_fix(vaddr);
}

#endif /*               */

#endif /*                    */