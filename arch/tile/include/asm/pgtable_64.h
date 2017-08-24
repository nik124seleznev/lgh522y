/*
 * Copyright 2011 Tilera Corporation. All Rights Reserved.
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
 *
 */

#ifndef _ASM_TILE_PGTABLE_64_H
#define _ASM_TILE_PGTABLE_64_H

/*                                                                     */
#define PGDIR_SHIFT	HV_LOG2_L1_SPAN
#define PGDIR_SIZE	HV_L1_SPAN
#define PGDIR_MASK	(~(PGDIR_SIZE-1))
#define PTRS_PER_PGD	HV_L0_ENTRIES
#define PGD_INDEX(va)	HV_L0_INDEX(va)
#define SIZEOF_PGD	HV_L0_SIZE

/*
                                                                         
                                                                     
 */
#define PMD_SHIFT	HPAGE_SHIFT
#define PMD_SIZE	HPAGE_SIZE
#define PMD_MASK	(~(PMD_SIZE-1))
#define PTRS_PER_PMD	_HV_L1_ENTRIES(HPAGE_SHIFT)
#define PMD_INDEX(va)	_HV_L1_INDEX(va, HPAGE_SHIFT)
#define SIZEOF_PMD	_HV_L1_SIZE(HPAGE_SHIFT)

/*
                                                                  
                                                            
                                                                  
                                                                   
                                           
 */
#define PTRS_PER_PTE	_HV_L2_ENTRIES(HPAGE_SHIFT, PAGE_SHIFT)
#define PTE_INDEX(va)	_HV_L2_INDEX(va, HPAGE_SHIFT, PAGE_SHIFT)
#define SIZEOF_PTE	_HV_L2_SIZE(HPAGE_SHIFT, PAGE_SHIFT)

/*
                                                                   
                                                                    
                                                                 
                                      
 */
#define _VMALLOC_END	HUGE_VMAP_BASE
#define VMALLOC_END	_VMALLOC_END
#define VMALLOC_START	_VMALLOC_START

#define HUGE_VMAP_END	(HUGE_VMAP_BASE + PGDIR_SIZE)

#ifndef __ASSEMBLY__

/*                                                       */
#include <asm-generic/pgtable-nopud.h>

static inline int pud_none(pud_t pud)
{
	return pud_val(pud) == 0;
}

static inline int pud_present(pud_t pud)
{
	return pud_val(pud) & _PAGE_PRESENT;
}

#define pmd_ERROR(e) \
	pr_err("%s:%d: bad pmd 0x%016llx.\n", __FILE__, __LINE__, pmd_val(e))

static inline void pud_clear(pud_t *pudp)
{
	__pte_clear(&pudp->pgd);
}

static inline int pud_bad(pud_t pud)
{
	return ((pud_val(pud) & _PAGE_ALL) != _PAGE_TABLE);
}

/*                                                                   */
#define pud_ptfn(pud) hv_pte_get_ptfn((pud).pgd)

/*
                                                                  
                                                                
                                                               
                              
 */
#define pud_page_vaddr(pud) \
	(__va((phys_addr_t)pud_ptfn(pud) << HV_LOG2_PAGE_TABLE_ALIGN))

/*
                                                                   
                                                                
 */
#define pud_page(pud) pfn_to_page(PFN_DOWN(HV_PTFN_TO_CPA(pud_ptfn(pud))))

static inline unsigned long pud_index(unsigned long address)
{
	return (address >> PUD_SHIFT) & (PTRS_PER_PUD - 1);
}

#define pmd_offset(pud, address) \
	((pmd_t *)pud_page_vaddr(*(pud)) + pmd_index(address))

/*                                                           */
#define pgd_addr_normalize pgd_addr_normalize
static inline unsigned long pgd_addr_normalize(unsigned long addr)
{
	return ((long)addr << (CHIP_WORD_SIZE() - CHIP_VA_WIDTH())) >>
		(CHIP_WORD_SIZE() - CHIP_VA_WIDTH());
}

/*                                               */
static inline int pgd_addr_invalid(unsigned long addr)
{
	return addr >= MEM_HV_START ||
		(addr > MEM_LOW_END && addr < MEM_HIGH_START);
}

/*
                                                                       
 */
#define __HAVE_ARCH_PTEP_TEST_AND_CLEAR_YOUNG
static inline int ptep_test_and_clear_young(struct vm_area_struct *vma,
					    unsigned long addr, pte_t *ptep)
{
	return (__insn_fetchand(&ptep->val, ~HV_PTE_ACCESSED) >>
		HV_PTE_INDEX_ACCESSED) & 0x1;
}

#define __HAVE_ARCH_PTEP_SET_WRPROTECT
static inline void ptep_set_wrprotect(struct mm_struct *mm,
				      unsigned long addr, pte_t *ptep)
{
	__insn_fetchand(&ptep->val, ~HV_PTE_WRITABLE);
}

#define __HAVE_ARCH_PTEP_GET_AND_CLEAR
static inline pte_t ptep_get_and_clear(struct mm_struct *mm,
				       unsigned long addr, pte_t *ptep)
{
	return hv_pte(__insn_exch(&ptep->val, 0UL));
}

/*
                                                                
 */
#define pmd_pte(pmd) (pmd)
#define pmdp_ptep(pmdp) (pmdp)
#define pte_pmd(pte) (pte)

#endif /*              */

#endif /*                        */
