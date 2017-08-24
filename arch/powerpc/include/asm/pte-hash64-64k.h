/*                                        */

/*                                                                       */
#define _PAGE_SPECIAL	0x00000400 /*                        */
#define _PAGE_HPTE_SUB	0x0ffff000 /*                                 */
#define _PAGE_HPTE_SUB0	0x08000000 /*                            */
#define _PAGE_COMBO	0x10000000 /*                         */
#define _PAGE_4K_PFN	0x20000000 /*                             */

/*                                                                   
                                                                  
                                                                  
                                                                   
                                                                     
                                                                     
                                                                    
                                                    
  
                                                                   
                                             
 */
#define _PAGE_HASHPTE	_PAGE_HPTE_SUB

/*                                                                   
                                                                 
                                                     
 */
#define _PAGE_F_SECOND  0x00008000 /*                      */
#define _PAGE_F_GIX     0x00007000 /*                      */

/*                                               */
#define _PAGE_HPTEFLAGS (_PAGE_BUSY | _PAGE_HASHPTE | _PAGE_COMBO)

/*                                   
  
                                                                   
                                                                     
 */
#define PTE_RPN_SHIFT	(30)

#ifndef __ASSEMBLY__

#include <asm/barrier.h>	/* for smp_rmb() */

 /*
  * With 64K pages on hash table, we have a special PTE format that
  * uses a second "half" of the page table to encode sub-page information
  * in order to deal with 64K made of 4K HW pages. Thus we override the
  * generic accessors and iterators here
  */
#define __real_pte __real_pte
static inline real_pte_t __real_pte(pte_t pte, pte_t *ptep)
{
	real_pte_t rpte;

	rpte.pte = pte;
	rpte.hidx = 0;
	if (pte_val(pte) & _PAGE_COMBO) {
		/*
		 * Make sure we order the hidx load against the _PAGE_COMBO
		 * check. The store side ordering is done in __hash_page_4K
		 */
		smp_rmb();
		rpte.hidx = pte_val(*((ptep) + PTRS_PER_PTE));
	}
	return rpte;
}

static inline unsigned long __rpte_to_hidx(real_pte_t rpte, unsigned long index)
{
	if ((pte_val(rpte.pte) & _PAGE_COMBO))
		return (rpte.hidx >> (index<<2)) & 0xf;
	return (pte_val(rpte.pte) >> 12) & 0xf;
}

#define __rpte_to_pte(r)	((r).pte)
#define __rpte_sub_valid(rpte, index) \
	(pte_val(rpte.pte) & (_PAGE_HPTE_SUB0 >> (index)))

/*                                                         
                                                   
 */
#define pte_iterate_hashed_subpages(rpte, psize, vpn, index, shift)	\
	do {								\
		unsigned long __end = vpn + (1UL << (PAGE_SHIFT - VPN_SHIFT));	\
		unsigned __split = (psize == MMU_PAGE_4K ||		\
				    psize == MMU_PAGE_64K_AP);		\
		shift = mmu_psize_defs[psize].shift;			\
		for (index = 0; vpn < __end; index++,			\
			     vpn += (1L << (shift - VPN_SHIFT))) {	\
			if (!__split || __rpte_sub_valid(rpte, index))	\
				do {

#define pte_iterate_hashed_end() } while(0); } } while(0)

#define pte_pagesize_index(mm, addr, pte)	\
	(((pte) & _PAGE_COMBO)? MMU_PAGE_4K: MMU_PAGE_64K)

#define remap_4k_pfn(vma, addr, pfn, prot)				\
	remap_pfn_range((vma), (addr), (pfn), PAGE_SIZE,		\
			__pgprot(pgprot_val((prot)) | _PAGE_4K_PFN))

#endif	/*              */
