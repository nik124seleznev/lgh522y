/*
 * Based on arch/arm/include/asm/tlbflush.h
 *
 * Copyright (C) 1999-2003 Russell King
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
 */
#ifndef __ASM_TLBFLUSH_H
#define __ASM_TLBFLUSH_H

#ifndef __ASSEMBLY__

#include <linux/sched.h>
#include <asm/cputype.h>

extern void __cpu_flush_user_tlb_range(unsigned long, unsigned long, struct vm_area_struct *);
extern void __cpu_flush_kern_tlb_range(unsigned long, unsigned long);

extern struct cpu_tlb_fns cpu_tlb;

/*
                 
                 
  
                                                         
  
                                                                       
                                                                     
                                                                         
                              
  
                  
  
                              
  
                   
  
                                                             
                                             
  
                                
  
                                                              
          
                                             
                                                
                                                       
  
                            
  
                                                                 
                                                  
                                               
  
                             
  
                                                                 
                                                              
                                             
                                           
 */
static inline void flush_tlb_all(void)
{
	dsb();
	asm("tlbi	vmalle1is");
	dsb();
	isb();
}

static inline void flush_tlb_mm(struct mm_struct *mm)
{
	unsigned long asid = (unsigned long)ASID(mm) << 48;

	dsb();
	asm("tlbi	aside1is, %0" : : "r" (asid));
	dsb();
}

static inline void flush_tlb_page(struct vm_area_struct *vma,
				  unsigned long uaddr)
{
	unsigned long addr = uaddr >> 12 |
		((unsigned long)ASID(vma->vm_mm) << 48);

	dsb();
	asm("tlbi	vae1is, %0" : : "r" (addr));
	dsb();
}

/*
                                           
 */
#define flush_tlb_range(vma,start,end)	__cpu_flush_user_tlb_range(start,end,vma)
#define flush_tlb_kernel_range(s,e)	__cpu_flush_kern_tlb_range(s,e)

/*
                                                                            
 */
static inline void update_mmu_cache(struct vm_area_struct *vma,
				    unsigned long addr, pte_t *ptep)
{
	/*
                                                                   
                     
  */
	dsb();
}

#endif

#endif
