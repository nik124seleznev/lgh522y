/*
 * pgtsrmmu.h:  SRMMU page table defines and code.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 */

#ifndef _SPARC_PGTSRMMU_H
#define _SPARC_PGTSRMMU_H

#include <asm/page.h>

#ifdef __ASSEMBLY__
#include <asm/thread_info.h>	/*                              */
#endif

/*                                                                            */
#define SRMMU_MAX_CONTEXTS	65536

/*                                                                                   */
#define SRMMU_REAL_PMD_SHIFT		18
#define SRMMU_REAL_PMD_SIZE		(1UL << SRMMU_REAL_PMD_SHIFT)
#define SRMMU_REAL_PMD_MASK		(~(SRMMU_REAL_PMD_SIZE-1))
#define SRMMU_REAL_PMD_ALIGN(__addr)	(((__addr)+SRMMU_REAL_PMD_SIZE-1)&SRMMU_REAL_PMD_MASK)

/*                                                                    */
#define SRMMU_PGDIR_SHIFT       24
#define SRMMU_PGDIR_SIZE        (1UL << SRMMU_PGDIR_SHIFT)
#define SRMMU_PGDIR_MASK        (~(SRMMU_PGDIR_SIZE-1))
#define SRMMU_PGDIR_ALIGN(addr) (((addr)+SRMMU_PGDIR_SIZE-1)&SRMMU_PGDIR_MASK)

#define SRMMU_REAL_PTRS_PER_PTE	64
#define SRMMU_REAL_PTRS_PER_PMD	64
#define SRMMU_PTRS_PER_PGD	256

#define SRMMU_REAL_PTE_TABLE_SIZE	(SRMMU_REAL_PTRS_PER_PTE*4)
#define SRMMU_PMD_TABLE_SIZE		(SRMMU_REAL_PTRS_PER_PMD*4)
#define SRMMU_PGD_TABLE_SIZE		(SRMMU_PTRS_PER_PGD*4)

/*
                                                                
                                                                
                                                                  
                                                                      
                   
  
                                                                            
                                                                           
                                                                      
                      
 */
#define SRMMU_PTRS_PER_PMD	4

/*                                                             */
#define SRMMU_ET_MASK         0x3
#define SRMMU_ET_INVALID      0x0
#define SRMMU_ET_PTD          0x1
#define SRMMU_ET_PTE          0x2
#define SRMMU_ET_REPTE        0x3 /*                                           */

/*                                                */
#define SRMMU_CTX_PMASK    0xfffffff0
#define SRMMU_PTD_PMASK    0xfffffff0
#define SRMMU_PTE_PMASK    0xffffff00

/*                                    
                                                
                                         
                                                      
                   
                                                 
                                        
 */
#define SRMMU_CACHE        0x80
#define SRMMU_DIRTY        0x40
#define SRMMU_REF          0x20
#define SRMMU_NOREAD       0x10
#define SRMMU_EXEC         0x08
#define SRMMU_WRITE        0x04
#define SRMMU_VALID        0x02 /*              */
#define SRMMU_PRIV         0x1c
#define SRMMU_PRIV_RDONLY  0x18

#define SRMMU_FILE         0x40	/*                         */

#define SRMMU_PTE_FILE_SHIFT     8	/*                         */

#define SRMMU_CHG_MASK    (0xffffff00 | SRMMU_REF | SRMMU_DIRTY)

/*                          
  
                                                                   
                                                  
  
                                   
                                   
  
                                                                            
                    
 */
#define SRMMU_SWP_TYPE_MASK	0x1f
#define SRMMU_SWP_TYPE_SHIFT	SRMMU_PTE_FILE_SHIFT
#define SRMMU_SWP_OFF_MASK	0x7ffff
#define SRMMU_SWP_OFF_SHIFT	(SRMMU_PTE_FILE_SHIFT + 5)

/*                                                            
                                                            
                                                         
                     
 */
#define SRMMU_PAGE_NONE    __pgprot(SRMMU_CACHE | \
				    SRMMU_PRIV | SRMMU_REF)
#define SRMMU_PAGE_SHARED  __pgprot(SRMMU_VALID | SRMMU_CACHE | \
				    SRMMU_EXEC | SRMMU_WRITE | SRMMU_REF)
#define SRMMU_PAGE_COPY    __pgprot(SRMMU_VALID | SRMMU_CACHE | \
				    SRMMU_EXEC | SRMMU_REF)
#define SRMMU_PAGE_RDONLY  __pgprot(SRMMU_VALID | SRMMU_CACHE | \
				    SRMMU_EXEC | SRMMU_REF)
#define SRMMU_PAGE_KERNEL  __pgprot(SRMMU_VALID | SRMMU_CACHE | SRMMU_PRIV | \
				    SRMMU_DIRTY | SRMMU_REF)

/*                                                              
                                            
 */
#define SRMMU_CTRL_REG           0x00000000
#define SRMMU_CTXTBL_PTR         0x00000100
#define SRMMU_CTX_REG            0x00000200
#define SRMMU_FAULT_STATUS       0x00000300
#define SRMMU_FAULT_ADDR         0x00000400

#define WINDOW_FLUSH(tmp1, tmp2)					\
	mov	0, tmp1;						\
98:	ld	[%g6 + TI_UWINMASK], tmp2;				\
	orcc	%g0, tmp2, %g0;						\
	add	tmp1, 1, tmp1;						\
	bne	98b;							\
	 save	%sp, -64, %sp;						\
99:	subcc	tmp1, 1, tmp1;						\
	bne	99b;							\
	 restore %g0, %g0, %g0;

#ifndef __ASSEMBLY__
extern unsigned long last_valid_pfn;

/*                                          */
/*                                            */
extern void *srmmu_nocache_pool;
#define __nocache_pa(VADDR) (((unsigned long)VADDR) - SRMMU_NOCACHE_VADDR + __pa((unsigned long)srmmu_nocache_pool))
#define __nocache_va(PADDR) (__va((unsigned long)PADDR) - (unsigned long)srmmu_nocache_pool + SRMMU_NOCACHE_VADDR)
#define __nocache_fix(VADDR) __va(__nocache_pa(VADDR))

/*                                     */
unsigned int srmmu_get_mmureg(void);
void srmmu_set_mmureg(unsigned long regval);
void srmmu_set_ctable_ptr(unsigned long paddr);
void srmmu_set_context(int context);
int srmmu_get_context(void);
unsigned int srmmu_get_fstatus(void);
unsigned int srmmu_get_faddr(void);

/*                                    */
static inline void srmmu_flush_whole_tlb(void)
{
	__asm__ __volatile__("sta %%g0, [%0] %1\n\t": :
			     "r" (0x400),        /*                    */
			     "i" (ASI_M_FLUSH_PROBE) : "memory");

}

static inline int
srmmu_get_pte (unsigned long addr)
{
	register unsigned long entry;
        
	__asm__ __volatile__("\n\tlda [%1] %2,%0\n\t" :
				"=r" (entry):
				"r" ((addr & 0xfffff000) | 0x400), "i" (ASI_M_FLUSH_PROBE));
	return entry;
}

#endif /*                 */

#endif /*                      */
