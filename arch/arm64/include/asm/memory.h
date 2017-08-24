/*
 * Based on arch/arm/include/asm/memory.h
 *
 * Copyright (C) 2000-2002 Russell King
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Note: this file should not be included by non-asm/.h files
 */
#ifndef __ASM_MEMORY_H
#define __ASM_MEMORY_H

#include <linux/compiler.h>
#include <linux/const.h>
#include <linux/types.h>
#include <asm/sizes.h>

/*
                                                                 
                                                                   
 */
#define UL(x) _AC(x, UL)

/*
                                                                      
                                                              
                                                     
                                                               
                                                                  
                                                                
 */
#define PAGE_OFFSET		UL(0xffffffc000000000)
#define MODULES_END		(PAGE_OFFSET)
#define MODULES_VADDR		(MODULES_END - SZ_64M)
#define EARLYCON_IOBASE		(MODULES_VADDR - SZ_4M)
#define VA_BITS			(39)
#define TASK_SIZE_64		(UL(1) << VA_BITS)

#ifdef CONFIG_COMPAT
#define TASK_SIZE_32		UL(0x100000000)
#define TASK_SIZE		(test_thread_flag(TIF_32BIT) ? \
				TASK_SIZE_32 : TASK_SIZE_64)
#define TASK_SIZE_OF(tsk)	(test_tsk_thread_flag(tsk, TIF_32BIT) ? \
				TASK_SIZE_32 : TASK_SIZE_64)
#else
#define TASK_SIZE		TASK_SIZE_64
#endif /*               */

#define TASK_UNMAPPED_BASE	(PAGE_ALIGN(TASK_SIZE / 4))

#if TASK_SIZE_64 > MODULES_VADDR
#error Top of 64-bit user space clashes with start of module space
#endif

/*
                                                               
                                                                
                                                           
 */
#define __virt_to_phys(x)	(((phys_addr_t)(x) - PAGE_OFFSET + PHYS_OFFSET))
#define __phys_to_virt(x)	((unsigned long)((x) - PHYS_OFFSET + PAGE_OFFSET))

/*
                                                             
 */
#define	__phys_to_pfn(paddr)	((unsigned long)((paddr) >> PAGE_SHIFT))
#define	__pfn_to_phys(pfn)	((phys_addr_t)(pfn) << PAGE_SHIFT)

/*
                                            
 */
#define page_to_phys(page)	(__pfn_to_phys(page_to_pfn(page)))
#define phys_to_page(phys)	(pfn_to_page(__phys_to_pfn(phys)))

/*
                          
 */
#define MT_DEVICE_nGnRnE	0
#define MT_DEVICE_nGnRE		1
#define MT_DEVICE_GRE		2
#define MT_NORMAL_NC		3
#define MT_NORMAL		4

#ifndef __ASSEMBLY__

extern phys_addr_t		memstart_addr;
/*                                                            */
#define PHYS_OFFSET		({ memstart_addr; })

/*
                                                          
                               
  
                                                      
                                                        
                                 
 */
#define PHYS_PFN_OFFSET	(PHYS_OFFSET >> PAGE_SHIFT)

/*
                                                          
                                                             
                                   
 */
static inline phys_addr_t virt_to_phys(const volatile void *x)
{
	return __virt_to_phys((unsigned long)(x));
}

static inline void *phys_to_virt(phys_addr_t x)
{
	return (void *)(__phys_to_virt(x));
}

/*
                                       
 */
#define __pa(x)			__virt_to_phys((unsigned long)(x))
#define __va(x)			((void *)__phys_to_virt((phys_addr_t)(x)))
#define pfn_to_kaddr(pfn)	__va((pfn) << PAGE_SHIFT)

/*
                                                                      
                                                                   
 */
#define ARCH_PFN_OFFSET		PHYS_PFN_OFFSET

#define virt_to_page(kaddr)	pfn_to_page(__pa(kaddr) >> PAGE_SHIFT)
#define	virt_addr_valid(kaddr)	(((void *)(kaddr) >= (void *)PAGE_OFFSET) && \
				 ((void *)(kaddr) < (void *)high_memory))

#endif

#include <asm-generic/memory_model.h>

#endif
