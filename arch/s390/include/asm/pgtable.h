/*
 *  S390 version
 *    Copyright IBM Corp. 1999, 2000
 *    Author(s): Hartmut Penner (hp@de.ibm.com)
 *               Ulrich Weigand (weigand@de.ibm.com)
 *               Martin Schwidefsky (schwidefsky@de.ibm.com)
 *
 *  Derived from "include/asm-i386/pgtable.h"
 */

#ifndef _ASM_S390_PGTABLE_H
#define _ASM_S390_PGTABLE_H

/*
                                                                          
                                                                        
                                                                        
                                                                          
                                                                       
         
  
                                                               
                                                                       
                      
  
                                                                           
                            
 */
#ifndef __ASSEMBLY__
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/page-flags.h>
#include <asm/bug.h>
#include <asm/page.h>

extern pgd_t swapper_pg_dir[] __attribute__ ((aligned (4096)));
extern void paging_init(void);
extern void vmem_map_init(void);

/*
                                                               
                                                
 */
#define update_mmu_cache(vma, address, ptep)     do { } while (0)
#define update_mmu_cache_pmd(vma, address, ptep) do { } while (0)

/*
                                                              
                                     
 */

extern unsigned long empty_zero_page;
extern unsigned long zero_page_mask;

#define ZERO_PAGE(vaddr) \
	(virt_to_page((void *)(empty_zero_page + \
	 (((unsigned long)(vaddr)) &zero_page_mask))))
#define __HAVE_COLOR_ZERO_PAGE

/*                                                 */
#define io_remap_pfn_range(vma, vaddr, pfn, size, prot) 	       \
	remap_pfn_range(vma, vaddr, pfn, size, prot)

#endif /*               */

/*
                                                                
                
                                                                     
 */
#ifndef CONFIG_64BIT
# define PMD_SHIFT	20
# define PUD_SHIFT	20
# define PGDIR_SHIFT	20
#else /*              */
# define PMD_SHIFT	20
# define PUD_SHIFT	31
# define PGDIR_SHIFT	42
#endif /*              */

#define PMD_SIZE        (1UL << PMD_SHIFT)
#define PMD_MASK        (~(PMD_SIZE-1))
#define PUD_SIZE	(1UL << PUD_SHIFT)
#define PUD_MASK	(~(PUD_SIZE-1))
#define PGDIR_SIZE	(1UL << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SIZE-1))

/*
                                                              
                                                     
                                                         
                                 
 */
#define PTRS_PER_PTE	256
#ifndef CONFIG_64BIT
#define PTRS_PER_PMD	1
#define PTRS_PER_PUD	1
#else /*              */
#define PTRS_PER_PMD	2048
#define PTRS_PER_PUD	2048
#endif /*              */
#define PTRS_PER_PGD	2048

#define FIRST_USER_ADDRESS  0

#define pte_ERROR(e) \
	printk("%s:%d: bad pte %p.\n", __FILE__, __LINE__, (void *) pte_val(e))
#define pmd_ERROR(e) \
	printk("%s:%d: bad pmd %p.\n", __FILE__, __LINE__, (void *) pmd_val(e))
#define pud_ERROR(e) \
	printk("%s:%d: bad pud %p.\n", __FILE__, __LINE__, (void *) pud_val(e))
#define pgd_ERROR(e) \
	printk("%s:%d: bad pgd %p.\n", __FILE__, __LINE__, (void *) pgd_val(e))

#ifndef __ASSEMBLY__
/*
                                                                               
                                                                            
                                                                            
                                                                         
                                                                              
                                      
 */
extern unsigned long VMALLOC_START;
extern unsigned long VMALLOC_END;
extern struct page *vmemmap;

#define VMEM_MAX_PHYS ((unsigned long) vmemmap)

#ifdef CONFIG_64BIT
extern unsigned long MODULES_VADDR;
extern unsigned long MODULES_END;
#define MODULES_VADDR	MODULES_VADDR
#define MODULES_END	MODULES_END
#define MODULES_LEN	(1UL << 31)
#endif

/*
                                                         
                                   
                           
                                   
                                   
  
                                                                       
                                                            
  
                                                            
                                   
                               
                                   
                                   
  
                                                                             
                                                              
                                                                              
  
                                                               
  
                                   
                            
                                   
                                   
  
                        
                               
                                                               
                        
                                                                                   
  
                                                         
                                  
                                                                   
                                                                   
  
                                                                       
                                                            
                                                              
  
                                                            
                                                                 
                                                                   
                                                                   
  
                                                                             
                                                              
                                                            
             
  
                                                            
                                                                   
                                                                   
                                                                   
  
                                                                             
             
     
                  
  
                                                              
                                                                   
                                                                   
                                                                   
  
                        
                           
                           
                        
               
                   
  
                                          
                  
                 
                  
                            
                      
                   
 */

/*                                       */
#define _PAGE_CO	0x100		/*                        */
#define _PAGE_RO	0x200		/*                   */
#define _PAGE_INVALID	0x400		/*                   */

/*                                       */
#define _PAGE_SWT	0x001		/*                   */
#define _PAGE_SWX	0x002		/*                   */
#define _PAGE_SWC	0x004		/*                    */
#define _PAGE_SWR	0x008		/*                       */
#define _PAGE_SWW	0x010		/*                  */
#define _PAGE_SPECIAL	0x020		/*                                 */
#define __HAVE_ARCH_PTE_SPECIAL

/*                                       */
#define _PAGE_CHG_MASK		(PAGE_MASK | _PAGE_SPECIAL | _PAGE_CO | \
				 _PAGE_SWC | _PAGE_SWR)

/*                               */
#define _PAGE_TYPE_EMPTY	0x400
#define _PAGE_TYPE_NONE		0x401
#define _PAGE_TYPE_SWAP		0x403
#define _PAGE_TYPE_FILE		0x601	/*                                 */
#define _PAGE_TYPE_RO		0x200
#define _PAGE_TYPE_RW		0x000

/*
                                                                           
                            
 */
#define _HPAGE_TYPE_EMPTY	0x020	/*                    */
#define _HPAGE_TYPE_NONE	0x220
#define _HPAGE_TYPE_RO		0x200	/*                    */
#define _HPAGE_TYPE_RW		0x000

/*
                                                                           
                                                                          
                                                                          
                                                                           
                                                                            
                                                                        
                                                                           
                                                                          
                                                                         
                              
  
                     
                                    
                                   
                                   
                                   
                                 
                                 
  
                                                                
                                                                         
                                                    
                                                           
 */

#ifndef CONFIG_64BIT

/*                                                         */
#define _ASCE_SPACE_SWITCH	0x80000000UL	/*                        */
#define _ASCE_ORIGIN_MASK	0x7ffff000UL	/*                          */
#define _ASCE_PRIVATE_SPACE	0x100	/*                           */
#define _ASCE_ALT_EVENT		0x80	/*                                  */
#define _ASCE_TABLE_LENGTH	0x7f	/*                           */

/*                                 */
#define _SEGMENT_ENTRY_ORIGIN	0x7fffffc0UL	/*                       */
#define _SEGMENT_ENTRY_RO	0x200	/*                          */
#define _SEGMENT_ENTRY_INV	0x20	/*                                 */
#define _SEGMENT_ENTRY_COMMON	0x10	/*                         */
#define _SEGMENT_ENTRY_PTL	0x0f	/*                        */

#define _SEGMENT_ENTRY		(_SEGMENT_ENTRY_PTL)
#define _SEGMENT_ENTRY_EMPTY	(_SEGMENT_ENTRY_INV)

/*                                           */
#define RCP_ACC_BITS	0xf0000000UL
#define RCP_FP_BIT	0x08000000UL
#define RCP_PCL_BIT	0x00800000UL
#define RCP_HR_BIT	0x00400000UL
#define RCP_HC_BIT	0x00200000UL
#define RCP_GR_BIT	0x00040000UL
#define RCP_GC_BIT	0x00020000UL
#define RCP_IN_BIT	0x00002000UL	/*                 */

/*                                                         */
#define KVM_UR_BIT	0x00008000UL
#define KVM_UC_BIT	0x00004000UL

#else /*              */

/*                                                                */
#define _ASCE_ORIGIN		~0xfffUL/*                           */
#define _ASCE_PRIVATE_SPACE	0x100	/*                           */
#define _ASCE_ALT_EVENT		0x80	/*                                  */
#define _ASCE_SPACE_SWITCH	0x40	/*                         */
#define _ASCE_REAL_SPACE	0x20	/*                         */
#define _ASCE_TYPE_MASK		0x0c	/*                           */
#define _ASCE_TYPE_REGION1	0x0c	/*                             */
#define _ASCE_TYPE_REGION2	0x08	/*                              */
#define _ASCE_TYPE_REGION3	0x04	/*                             */
#define _ASCE_TYPE_SEGMENT	0x00	/*                         */
#define _ASCE_TABLE_LENGTH	0x03	/*                          */

/*                                */
#define _REGION_ENTRY_ORIGIN	~0xfffUL/*                                 */
#define _REGION_ENTRY_RO	0x200	/*                           */
#define _REGION_ENTRY_INV	0x20	/*                                */
#define _REGION_ENTRY_TYPE_MASK	0x0c	/*                                  */
#define _REGION_ENTRY_TYPE_R1	0x0c	/*                             */
#define _REGION_ENTRY_TYPE_R2	0x08	/*                              */
#define _REGION_ENTRY_TYPE_R3	0x04	/*                             */
#define _REGION_ENTRY_LENGTH	0x03	/*                          */

#define _REGION1_ENTRY		(_REGION_ENTRY_TYPE_R1 | _REGION_ENTRY_LENGTH)
#define _REGION1_ENTRY_EMPTY	(_REGION_ENTRY_TYPE_R1 | _REGION_ENTRY_INV)
#define _REGION2_ENTRY		(_REGION_ENTRY_TYPE_R2 | _REGION_ENTRY_LENGTH)
#define _REGION2_ENTRY_EMPTY	(_REGION_ENTRY_TYPE_R2 | _REGION_ENTRY_INV)
#define _REGION3_ENTRY		(_REGION_ENTRY_TYPE_R3 | _REGION_ENTRY_LENGTH)
#define _REGION3_ENTRY_EMPTY	(_REGION_ENTRY_TYPE_R3 | _REGION_ENTRY_INV)

#define _REGION3_ENTRY_LARGE	0x400	/*                                  */
#define _REGION3_ENTRY_RO	0x200	/*                          */
#define _REGION3_ENTRY_CO	0x100	/*                               */

/*                                 */
#define _SEGMENT_ENTRY_ORIGIN_LARGE ~0xfffffUL /*                        */
#define _SEGMENT_ENTRY_ORIGIN	~0x7ffUL/*                           */
#define _SEGMENT_ENTRY_RO	0x200	/*                          */
#define _SEGMENT_ENTRY_INV	0x20	/*                                 */

#define _SEGMENT_ENTRY		(0)
#define _SEGMENT_ENTRY_EMPTY	(_SEGMENT_ENTRY_INV)

#define _SEGMENT_ENTRY_LARGE	0x400	/*                                  */
#define _SEGMENT_ENTRY_CO	0x100	/*                             */
#define _SEGMENT_ENTRY_SPLIT_BIT 0	/*                          */
#define _SEGMENT_ENTRY_SPLIT	(1UL << _SEGMENT_ENTRY_SPLIT_BIT)

/*                                       */
#define _SEGMENT_CHG_MASK	(_SEGMENT_ENTRY_ORIGIN | _SEGMENT_ENTRY_LARGE \
				 | _SEGMENT_ENTRY_SPLIT | _SEGMENT_ENTRY_CO)

/*                                           */
#define RCP_ACC_BITS	0xf000000000000000UL
#define RCP_FP_BIT	0x0800000000000000UL
#define RCP_PCL_BIT	0x0080000000000000UL
#define RCP_HR_BIT	0x0040000000000000UL
#define RCP_HC_BIT	0x0020000000000000UL
#define RCP_GR_BIT	0x0004000000000000UL
#define RCP_GC_BIT	0x0002000000000000UL
#define RCP_IN_BIT	0x0000200000000000UL	/*                 */

/*                                                         */
#define KVM_UR_BIT	0x0000800000000000UL
#define KVM_UC_BIT	0x0000400000000000UL

#endif /*              */

/*
                                                                
                                                                     
                                                          
 */
#define _ASCE_USER_BITS		(_ASCE_SPACE_SWITCH | _ASCE_PRIVATE_SPACE | \
				 _ASCE_ALT_EVENT)

/*
                               
 */
#define PAGE_NONE	__pgprot(_PAGE_TYPE_NONE)
#define PAGE_RO		__pgprot(_PAGE_TYPE_RO)
#define PAGE_RW		__pgprot(_PAGE_TYPE_RO | _PAGE_SWW)
#define PAGE_RWC	__pgprot(_PAGE_TYPE_RW | _PAGE_SWW | _PAGE_SWC)

#define PAGE_KERNEL	PAGE_RWC
#define PAGE_SHARED	PAGE_KERNEL
#define PAGE_COPY	PAGE_RO

/*
                                                                       
                                                                  
                           
 */
         /*   */
#define __P000	PAGE_NONE
#define __P001	PAGE_RO
#define __P010	PAGE_RO
#define __P011	PAGE_RO
#define __P100	PAGE_RO
#define __P101	PAGE_RO
#define __P110	PAGE_RO
#define __P111	PAGE_RO

#define __S000	PAGE_NONE
#define __S001	PAGE_RO
#define __S010	PAGE_RW
#define __S011	PAGE_RW
#define __S100	PAGE_RO
#define __S101	PAGE_RO
#define __S110	PAGE_RW
#define __S111	PAGE_RW

/*
                                                     
 */
#define SEGMENT_NONE	__pgprot(_HPAGE_TYPE_NONE)
#define SEGMENT_RO	__pgprot(_HPAGE_TYPE_RO)
#define SEGMENT_RW	__pgprot(_HPAGE_TYPE_RW)

static inline int mm_exclusive(struct mm_struct *mm)
{
	return likely(mm == current->active_mm &&
		      atomic_read(&mm->context.attach_count) <= 1);
}

static inline int mm_has_pgste(struct mm_struct *mm)
{
#ifdef CONFIG_PGSTE
	if (unlikely(mm->context.has_pgste))
		return 1;
#endif
	return 0;
}
/*
                              
 */
#ifndef CONFIG_64BIT

static inline int pgd_present(pgd_t pgd) { return 1; }
static inline int pgd_none(pgd_t pgd)    { return 0; }
static inline int pgd_bad(pgd_t pgd)     { return 0; }

static inline int pud_present(pud_t pud) { return 1; }
static inline int pud_none(pud_t pud)	 { return 0; }
static inline int pud_large(pud_t pud)	 { return 0; }
static inline int pud_bad(pud_t pud)	 { return 0; }

#else /*              */

static inline int pgd_present(pgd_t pgd)
{
	if ((pgd_val(pgd) & _REGION_ENTRY_TYPE_MASK) < _REGION_ENTRY_TYPE_R2)
		return 1;
	return (pgd_val(pgd) & _REGION_ENTRY_ORIGIN) != 0UL;
}

static inline int pgd_none(pgd_t pgd)
{
	if ((pgd_val(pgd) & _REGION_ENTRY_TYPE_MASK) < _REGION_ENTRY_TYPE_R2)
		return 0;
	return (pgd_val(pgd) & _REGION_ENTRY_INV) != 0UL;
}

static inline int pgd_bad(pgd_t pgd)
{
	/*
                                                                
                                                              
                                   
  */
	unsigned long mask =
		~_SEGMENT_ENTRY_ORIGIN & ~_REGION_ENTRY_INV &
		~_REGION_ENTRY_TYPE_MASK & ~_REGION_ENTRY_LENGTH;
	return (pgd_val(pgd) & mask) != 0;
}

static inline int pud_present(pud_t pud)
{
	if ((pud_val(pud) & _REGION_ENTRY_TYPE_MASK) < _REGION_ENTRY_TYPE_R3)
		return 1;
	return (pud_val(pud) & _REGION_ENTRY_ORIGIN) != 0UL;
}

static inline int pud_none(pud_t pud)
{
	if ((pud_val(pud) & _REGION_ENTRY_TYPE_MASK) < _REGION_ENTRY_TYPE_R3)
		return 0;
	return (pud_val(pud) & _REGION_ENTRY_INV) != 0UL;
}

static inline int pud_large(pud_t pud)
{
	if ((pud_val(pud) & _REGION_ENTRY_TYPE_MASK) != _REGION_ENTRY_TYPE_R3)
		return 0;
	return !!(pud_val(pud) & _REGION3_ENTRY_LARGE);
}

static inline int pud_bad(pud_t pud)
{
	/*
                                                                
                                                              
                                   
  */
	unsigned long mask =
		~_SEGMENT_ENTRY_ORIGIN & ~_REGION_ENTRY_INV &
		~_REGION_ENTRY_TYPE_MASK & ~_REGION_ENTRY_LENGTH;
	return (pud_val(pud) & mask) != 0;
}

#endif /*              */

static inline int pmd_present(pmd_t pmd)
{
	unsigned long mask = _SEGMENT_ENTRY_INV | _SEGMENT_ENTRY_RO;
	return (pmd_val(pmd) & mask) == _HPAGE_TYPE_NONE ||
	       !(pmd_val(pmd) & _SEGMENT_ENTRY_INV);
}

static inline int pmd_none(pmd_t pmd)
{
	return (pmd_val(pmd) & _SEGMENT_ENTRY_INV) &&
	       !(pmd_val(pmd) & _SEGMENT_ENTRY_RO);
}

static inline int pmd_large(pmd_t pmd)
{
#ifdef CONFIG_64BIT
	return !!(pmd_val(pmd) & _SEGMENT_ENTRY_LARGE);
#else
	return 0;
#endif
}

static inline int pmd_bad(pmd_t pmd)
{
	unsigned long mask = ~_SEGMENT_ENTRY_ORIGIN & ~_SEGMENT_ENTRY_INV;
	return (pmd_val(pmd) & mask) != _SEGMENT_ENTRY;
}

#define __HAVE_ARCH_PMDP_SPLITTING_FLUSH
extern void pmdp_splitting_flush(struct vm_area_struct *vma,
				 unsigned long addr, pmd_t *pmdp);

#define  __HAVE_ARCH_PMDP_SET_ACCESS_FLAGS
extern int pmdp_set_access_flags(struct vm_area_struct *vma,
				 unsigned long address, pmd_t *pmdp,
				 pmd_t entry, int dirty);

#define __HAVE_ARCH_PMDP_CLEAR_YOUNG_FLUSH
extern int pmdp_clear_flush_young(struct vm_area_struct *vma,
				  unsigned long address, pmd_t *pmdp);

#define __HAVE_ARCH_PMD_WRITE
static inline int pmd_write(pmd_t pmd)
{
	return (pmd_val(pmd) & _SEGMENT_ENTRY_RO) == 0;
}

static inline int pmd_young(pmd_t pmd)
{
	return 0;
}

static inline int pte_none(pte_t pte)
{
	return (pte_val(pte) & _PAGE_INVALID) && !(pte_val(pte) & _PAGE_SWT);
}

static inline int pte_present(pte_t pte)
{
	unsigned long mask = _PAGE_RO | _PAGE_INVALID | _PAGE_SWT | _PAGE_SWX;
	return (pte_val(pte) & mask) == _PAGE_TYPE_NONE ||
		(!(pte_val(pte) & _PAGE_INVALID) &&
		 !(pte_val(pte) & _PAGE_SWT));
}

static inline int pte_file(pte_t pte)
{
	unsigned long mask = _PAGE_RO | _PAGE_INVALID | _PAGE_SWT;
	return (pte_val(pte) & mask) == _PAGE_TYPE_FILE;
}

static inline int pte_special(pte_t pte)
{
	return (pte_val(pte) & _PAGE_SPECIAL);
}

#define __HAVE_ARCH_PTE_SAME
static inline int pte_same(pte_t a, pte_t b)
{
	return pte_val(a) == pte_val(b);
}

static inline pgste_t pgste_get_lock(pte_t *ptep)
{
	unsigned long new = 0;
#ifdef CONFIG_PGSTE
	unsigned long old;

	preempt_disable();
	asm(
		"	lg	%0,%2\n"
		"0:	lgr	%1,%0\n"
		"	nihh	%0,0xff7f\n"	/*                          */
		"	oihh	%1,0x0080\n"	/*                        */
		"	csg	%0,%1,%2\n"
		"	jl	0b\n"
		: "=&d" (old), "=&d" (new), "=Q" (ptep[PTRS_PER_PTE])
		: "Q" (ptep[PTRS_PER_PTE]) : "cc", "memory");
#endif
	return __pgste(new);
}

static inline void pgste_set_unlock(pte_t *ptep, pgste_t pgste)
{
#ifdef CONFIG_PGSTE
	asm(
		"	nihh	%1,0xff7f\n"	/*                   */
		"	stg	%1,%0\n"
		: "=Q" (ptep[PTRS_PER_PTE])
		: "d" (pgste_val(pgste)), "Q" (ptep[PTRS_PER_PTE])
		: "cc", "memory");
	preempt_enable();
#endif
}

static inline void pgste_set(pte_t *ptep, pgste_t pgste)
{
#ifdef CONFIG_PGSTE
	*(pgste_t *)(ptep + PTRS_PER_PTE) = pgste;
#endif
}

static inline pgste_t pgste_update_all(pte_t *ptep, pgste_t pgste)
{
#ifdef CONFIG_PGSTE
	unsigned long address, bits;
	unsigned char skey;

	if (pte_val(*ptep) & _PAGE_INVALID)
		return pgste;
	address = pte_val(*ptep) & PAGE_MASK;
	skey = page_get_storage_key(address);
	bits = skey & (_PAGE_CHANGED | _PAGE_REFERENCED);
	/*                                                        */
	if (bits & _PAGE_CHANGED)
		page_set_storage_key(address, skey ^ bits, 0);
	else if (bits)
		page_reset_referenced(address);
	/*                                                               */
	pgste_val(pgste) |= bits << 48;		/*                         */
	/*                                               */
	bits |= (pgste_val(pgste) & (RCP_HR_BIT | RCP_HC_BIT)) >> 52;
	/*                                                         */
	pgste_val(pgste) |= bits << 45;		/*                         */
	/*                                    */
	pgste_val(pgste) &= ~(RCP_HR_BIT | RCP_HC_BIT);
	pgste_val(pgste) &= ~(RCP_ACC_BITS | RCP_FP_BIT);
	/*                                                        */
	pgste_val(pgste) |=
		(unsigned long) (skey & (_PAGE_ACC_BITS | _PAGE_FP_BIT)) << 56;
	/*                                */
	pte_val(*ptep) |= (bits & _PAGE_REFERENCED) << 1;
#endif
	return pgste;

}

static inline pgste_t pgste_update_young(pte_t *ptep, pgste_t pgste)
{
#ifdef CONFIG_PGSTE
	int young;

	if (pte_val(*ptep) & _PAGE_INVALID)
		return pgste;
	/*                                     */
	young = page_reset_referenced(pte_val(*ptep) & PAGE_MASK);
	if (young)
		pgste_val(pgste) |= RCP_GR_BIT;
	/*                                    */
	if (pgste_val(pgste) & RCP_HR_BIT) {
		pgste_val(pgste) &= ~RCP_HR_BIT;
		young = 1;
	}
	/*                                                  */
	if (young) {
		pgste_val(pgste) |= KVM_UR_BIT;
		pte_val(*ptep) |= _PAGE_SWR;
	}
#endif
	return pgste;
}

static inline void pgste_set_key(pte_t *ptep, pgste_t pgste, pte_t entry)
{
#ifdef CONFIG_PGSTE
	unsigned long address;
	unsigned long nkey;

	if (pte_val(entry) & _PAGE_INVALID)
		return;
	VM_BUG_ON(!(pte_val(*ptep) & _PAGE_INVALID));
	address = pte_val(entry) & PAGE_MASK;
	/*
                                                            
                                                             
                 
  */
	nkey = (pgste_val(pgste) & (RCP_ACC_BITS | RCP_FP_BIT)) >> 56;
	page_set_storage_key(address, nkey, 0);
#endif
}

static inline void pgste_set_pte(pte_t *ptep, pte_t entry)
{
	if (!MACHINE_HAS_ESOP && (pte_val(entry) & _PAGE_SWW)) {
		/*
                                                     
                                            
   */
		pte_val(entry) |= _PAGE_SWC;
		pte_val(entry) &= ~_PAGE_RO;
	}
	*ptep = entry;
}

/* 
                                           
                                       
                                        
                                                           
                                                                      
 */
struct gmap {
	struct list_head list;
	struct mm_struct *mm;
	unsigned long *table;
	unsigned long asce;
	struct list_head crst_list;
};

/* 
                                                               
                                    
                                           
                                                      
 */
struct gmap_rmap {
	struct list_head list;
	struct gmap *gmap;
	unsigned long *entry;
	unsigned long vmaddr;
};

/* 
                                                                  
                                                                    
                                                              
 */
struct gmap_pgtable {
	unsigned long vmaddr;
	struct list_head mapper;
};

/* 
                                                                     
                                               
 */
struct gmap_notifier {
	struct list_head list;
	void (*notifier_call)(struct gmap *gmap, unsigned long address);
};

struct gmap *gmap_alloc(struct mm_struct *mm);
void gmap_free(struct gmap *gmap);
void gmap_enable(struct gmap *gmap);
void gmap_disable(struct gmap *gmap);
int gmap_map_segment(struct gmap *gmap, unsigned long from,
		     unsigned long to, unsigned long len);
int gmap_unmap_segment(struct gmap *gmap, unsigned long to, unsigned long len);
unsigned long __gmap_translate(unsigned long address, struct gmap *);
unsigned long gmap_translate(unsigned long address, struct gmap *);
unsigned long __gmap_fault(unsigned long address, struct gmap *);
unsigned long gmap_fault(unsigned long address, struct gmap *);
void gmap_discard(unsigned long from, unsigned long to, struct gmap *);

void gmap_register_ipte_notifier(struct gmap_notifier *);
void gmap_unregister_ipte_notifier(struct gmap_notifier *);
int gmap_ipte_notify(struct gmap *, unsigned long start, unsigned long len);
void gmap_do_ipte_notify(struct mm_struct *, unsigned long addr, pte_t *);

static inline pgste_t pgste_ipte_notify(struct mm_struct *mm,
					unsigned long addr,
					pte_t *ptep, pgste_t pgste)
{
#ifdef CONFIG_PGSTE
	if (pgste_val(pgste) & RCP_IN_BIT) {
		pgste_val(pgste) &= ~RCP_IN_BIT;
		gmap_do_ipte_notify(mm, addr, ptep);
	}
#endif
	return pgste;
}

/*
                                                            
                                                                  
                          
 */
static inline void set_pte_at(struct mm_struct *mm, unsigned long addr,
			      pte_t *ptep, pte_t entry)
{
	pgste_t pgste;

	if (mm_has_pgste(mm)) {
		pgste = pgste_get_lock(ptep);
		pgste_set_key(ptep, pgste, entry);
		pgste_set_pte(ptep, entry);
		pgste_set_unlock(ptep, pgste);
	} else {
		if (!(pte_val(entry) & _PAGE_INVALID) && MACHINE_HAS_EDAT1)
			pte_val(entry) |= _PAGE_CO;
		*ptep = entry;
	}
}

/*
                                                             
                                                      
 */
static inline int pte_write(pte_t pte)
{
	return (pte_val(pte) & _PAGE_SWW) != 0;
}

static inline int pte_dirty(pte_t pte)
{
	return (pte_val(pte) & _PAGE_SWC) != 0;
}

static inline int pte_young(pte_t pte)
{
#ifdef CONFIG_PGSTE
	if (pte_val(pte) & _PAGE_SWR)
		return 1;
#endif
	return 0;
}

/*
                                     
 */

static inline void pgd_clear(pgd_t *pgd)
{
#ifdef CONFIG_64BIT
	if ((pgd_val(*pgd) & _REGION_ENTRY_TYPE_MASK) == _REGION_ENTRY_TYPE_R2)
		pgd_val(*pgd) = _REGION2_ENTRY_EMPTY;
#endif
}

static inline void pud_clear(pud_t *pud)
{
#ifdef CONFIG_64BIT
	if ((pud_val(*pud) & _REGION_ENTRY_TYPE_MASK) == _REGION_ENTRY_TYPE_R3)
		pud_val(*pud) = _REGION3_ENTRY_EMPTY;
#endif
}

static inline void pmd_clear(pmd_t *pmdp)
{
	pmd_val(*pmdp) = _SEGMENT_ENTRY_EMPTY;
}

static inline void pte_clear(struct mm_struct *mm, unsigned long addr, pte_t *ptep)
{
	pte_val(*ptep) = _PAGE_TYPE_EMPTY;
}

/*
                                                        
                                                      
 */
static inline pte_t pte_modify(pte_t pte, pgprot_t newprot)
{
	pte_val(pte) &= _PAGE_CHG_MASK;
	pte_val(pte) |= pgprot_val(newprot);
	if ((pte_val(pte) & _PAGE_SWC) && (pte_val(pte) & _PAGE_SWW))
		pte_val(pte) &= ~_PAGE_RO;
	return pte;
}

static inline pte_t pte_wrprotect(pte_t pte)
{
	pte_val(pte) &= ~_PAGE_SWW;
	/*                                        */
	if (!(pte_val(pte) & _PAGE_INVALID))
		pte_val(pte) |= _PAGE_RO;
	return pte;
}

static inline pte_t pte_mkwrite(pte_t pte)
{
	pte_val(pte) |= _PAGE_SWW;
	if (pte_val(pte) & _PAGE_SWC)
		pte_val(pte) &= ~_PAGE_RO;
	return pte;
}

static inline pte_t pte_mkclean(pte_t pte)
{
	pte_val(pte) &= ~_PAGE_SWC;
	/*                                        */
	if (!(pte_val(pte) & _PAGE_INVALID))
		pte_val(pte) |= _PAGE_RO;
	return pte;
}

static inline pte_t pte_mkdirty(pte_t pte)
{
	pte_val(pte) |= _PAGE_SWC;
	if (pte_val(pte) & _PAGE_SWW)
		pte_val(pte) &= ~_PAGE_RO;
	return pte;
}

static inline pte_t pte_mkold(pte_t pte)
{
#ifdef CONFIG_PGSTE
	pte_val(pte) &= ~_PAGE_SWR;
#endif
	return pte;
}

static inline pte_t pte_mkyoung(pte_t pte)
{
	return pte;
}

static inline pte_t pte_mkspecial(pte_t pte)
{
	pte_val(pte) |= _PAGE_SPECIAL;
	return pte;
}

#ifdef CONFIG_HUGETLB_PAGE
static inline pte_t pte_mkhuge(pte_t pte)
{
	pte_val(pte) |= (_SEGMENT_ENTRY_LARGE | _SEGMENT_ENTRY_CO);
	return pte;
}
#endif

/*
                                                
 */
static inline int ptep_test_and_clear_user_dirty(struct mm_struct *mm,
						 pte_t *ptep)
{
	pgste_t pgste;
	int dirty = 0;

	if (mm_has_pgste(mm)) {
		pgste = pgste_get_lock(ptep);
		pgste = pgste_update_all(ptep, pgste);
		dirty = !!(pgste_val(pgste) & KVM_UC_BIT);
		pgste_val(pgste) &= ~KVM_UC_BIT;
		pgste_set_unlock(ptep, pgste);
		return dirty;
	}
	return dirty;
}

/*
                                                     
 */
static inline int ptep_test_and_clear_user_young(struct mm_struct *mm,
						 pte_t *ptep)
{
	pgste_t pgste;
	int young = 0;

	if (mm_has_pgste(mm)) {
		pgste = pgste_get_lock(ptep);
		pgste = pgste_update_young(ptep, pgste);
		young = !!(pgste_val(pgste) & KVM_UR_BIT);
		pgste_val(pgste) &= ~KVM_UR_BIT;
		pgste_set_unlock(ptep, pgste);
	}
	return young;
}

#define __HAVE_ARCH_PTEP_TEST_AND_CLEAR_YOUNG
static inline int ptep_test_and_clear_young(struct vm_area_struct *vma,
					    unsigned long addr, pte_t *ptep)
{
	pgste_t pgste;
	pte_t pte;

	if (mm_has_pgste(vma->vm_mm)) {
		pgste = pgste_get_lock(ptep);
		pgste = pgste_update_young(ptep, pgste);
		pte = *ptep;
		*ptep = pte_mkold(pte);
		pgste_set_unlock(ptep, pgste);
		return pte_young(pte);
	}
	return 0;
}

#define __HAVE_ARCH_PTEP_CLEAR_YOUNG_FLUSH
static inline int ptep_clear_flush_young(struct vm_area_struct *vma,
					 unsigned long address, pte_t *ptep)
{
	/*                     
                                                              
                                                               
                         */
	return ptep_test_and_clear_young(vma, address, ptep);
}

static inline void __ptep_ipte(unsigned long address, pte_t *ptep)
{
	if (!(pte_val(*ptep) & _PAGE_INVALID)) {
#ifndef CONFIG_64BIT
		/*                                                  */
		pte_t *pto = (pte_t *) (((unsigned long) ptep) & 0x7ffffc00);
#else
		/*                                    */
		pte_t *pto = ptep;
#endif
		asm volatile(
			"	ipte	%2,%3"
			: "=m" (*ptep) : "m" (*ptep),
			  "a" (pto), "a" (address));
	}
}

/*
                                                                      
                                                              
                                                                    
                                           
                          
                  
                       
                                                                        
                                                                       
                                                                        
            
 */
#define __HAVE_ARCH_PTEP_GET_AND_CLEAR
static inline pte_t ptep_get_and_clear(struct mm_struct *mm,
				       unsigned long address, pte_t *ptep)
{
	pgste_t pgste;
	pte_t pte;

	mm->context.flush_mm = 1;
	if (mm_has_pgste(mm)) {
		pgste = pgste_get_lock(ptep);
		pgste = pgste_ipte_notify(mm, address, ptep, pgste);
	}

	pte = *ptep;
	if (!mm_exclusive(mm))
		__ptep_ipte(address, ptep);
	pte_val(*ptep) = _PAGE_TYPE_EMPTY;

	if (mm_has_pgste(mm)) {
		pgste = pgste_update_all(&pte, pgste);
		pgste_set_unlock(ptep, pgste);
	}
	return pte;
}

#define __HAVE_ARCH_PTEP_MODIFY_PROT_TRANSACTION
static inline pte_t ptep_modify_prot_start(struct mm_struct *mm,
					   unsigned long address,
					   pte_t *ptep)
{
	pgste_t pgste;
	pte_t pte;

	mm->context.flush_mm = 1;
	if (mm_has_pgste(mm)) {
		pgste = pgste_get_lock(ptep);
		pgste_ipte_notify(mm, address, ptep, pgste);
	}

	pte = *ptep;
	if (!mm_exclusive(mm))
		__ptep_ipte(address, ptep);

	if (mm_has_pgste(mm)) {
		pgste = pgste_update_all(&pte, pgste);
		pgste_set(ptep, pgste);
	}
	return pte;
}

static inline void ptep_modify_prot_commit(struct mm_struct *mm,
					   unsigned long address,
					   pte_t *ptep, pte_t pte)
{
	pgste_t pgste;

	if (mm_has_pgste(mm)) {
		pgste = *(pgste_t *)(ptep + PTRS_PER_PTE);
		pgste_set_key(ptep, pgste, pte);
		pgste_set_pte(ptep, pte);
		pgste_set_unlock(ptep, pgste);
	} else
		*ptep = pte;
}

#define __HAVE_ARCH_PTEP_CLEAR_FLUSH
static inline pte_t ptep_clear_flush(struct vm_area_struct *vma,
				     unsigned long address, pte_t *ptep)
{
	pgste_t pgste;
	pte_t pte;

	if (mm_has_pgste(vma->vm_mm)) {
		pgste = pgste_get_lock(ptep);
		pgste = pgste_ipte_notify(vma->vm_mm, address, ptep, pgste);
	}

	pte = *ptep;
	__ptep_ipte(address, ptep);
	pte_val(*ptep) = _PAGE_TYPE_EMPTY;

	if (mm_has_pgste(vma->vm_mm)) {
		pgste = pgste_update_all(&pte, pgste);
		pgste_set_unlock(ptep, pgste);
	}
	return pte;
}

/*
                                                                       
                                                                     
                                                                   
                                                                      
                                                       
 */
#define __HAVE_ARCH_PTEP_GET_AND_CLEAR_FULL
static inline pte_t ptep_get_and_clear_full(struct mm_struct *mm,
					    unsigned long address,
					    pte_t *ptep, int full)
{
	pgste_t pgste;
	pte_t pte;

	if (mm_has_pgste(mm)) {
		pgste = pgste_get_lock(ptep);
		if (!full)
			pgste = pgste_ipte_notify(mm, address, ptep, pgste);
	}

	pte = *ptep;
	if (!full)
		__ptep_ipte(address, ptep);
	pte_val(*ptep) = _PAGE_TYPE_EMPTY;

	if (mm_has_pgste(mm)) {
		pgste = pgste_update_all(&pte, pgste);
		pgste_set_unlock(ptep, pgste);
	}
	return pte;
}

#define __HAVE_ARCH_PTEP_SET_WRPROTECT
static inline pte_t ptep_set_wrprotect(struct mm_struct *mm,
				       unsigned long address, pte_t *ptep)
{
	pgste_t pgste;
	pte_t pte = *ptep;

	if (pte_write(pte)) {
		mm->context.flush_mm = 1;
		if (mm_has_pgste(mm)) {
			pgste = pgste_get_lock(ptep);
			pgste = pgste_ipte_notify(mm, address, ptep, pgste);
		}

		if (!mm_exclusive(mm))
			__ptep_ipte(address, ptep);
		pte = pte_wrprotect(pte);

		if (mm_has_pgste(mm)) {
			pgste_set_pte(ptep, pte);
			pgste_set_unlock(ptep, pgste);
		} else
			*ptep = pte;
	}
	return pte;
}

#define __HAVE_ARCH_PTEP_SET_ACCESS_FLAGS
static inline int ptep_set_access_flags(struct vm_area_struct *vma,
					unsigned long address, pte_t *ptep,
					pte_t entry, int dirty)
{
	pgste_t pgste;

	if (pte_same(*ptep, entry))
		return 0;
	if (mm_has_pgste(vma->vm_mm)) {
		pgste = pgste_get_lock(ptep);
		pgste = pgste_ipte_notify(vma->vm_mm, address, ptep, pgste);
	}

	__ptep_ipte(address, ptep);

	if (mm_has_pgste(vma->vm_mm)) {
		pgste_set_pte(ptep, entry);
		pgste_set_unlock(ptep, pgste);
	} else
		*ptep = entry;
	return 1;
}

/*
                                                                       
                                                                 
 */
static inline pte_t mk_pte_phys(unsigned long physpage, pgprot_t pgprot)
{
	pte_t __pte;
	pte_val(__pte) = physpage + pgprot_val(pgprot);
	return __pte;
}

static inline pte_t mk_pte(struct page *page, pgprot_t pgprot)
{
	unsigned long physpage = page_to_phys(page);
	pte_t __pte = mk_pte_phys(physpage, pgprot);

	if ((pte_val(__pte) & _PAGE_SWW) && PageDirty(page)) {
		pte_val(__pte) |= _PAGE_SWC;
		pte_val(__pte) &= ~_PAGE_RO;
	}
	return __pte;
}

#define pgd_index(address) (((address) >> PGDIR_SHIFT) & (PTRS_PER_PGD-1))
#define pud_index(address) (((address) >> PUD_SHIFT) & (PTRS_PER_PUD-1))
#define pmd_index(address) (((address) >> PMD_SHIFT) & (PTRS_PER_PMD-1))
#define pte_index(address) (((address) >> PAGE_SHIFT) & (PTRS_PER_PTE-1))

#define pgd_offset(mm, address) ((mm)->pgd + pgd_index(address))
#define pgd_offset_k(address) pgd_offset(&init_mm, address)

#ifndef CONFIG_64BIT

#define pmd_deref(pmd) (pmd_val(pmd) & _SEGMENT_ENTRY_ORIGIN)
#define pud_deref(pmd) ({ BUG(); 0UL; })
#define pgd_deref(pmd) ({ BUG(); 0UL; })

#define pud_offset(pgd, address) ((pud_t *) pgd)
#define pmd_offset(pud, address) ((pmd_t *) pud + pmd_index(address))

#else /*              */

#define pmd_deref(pmd) (pmd_val(pmd) & _SEGMENT_ENTRY_ORIGIN)
#define pud_deref(pud) (pud_val(pud) & _REGION_ENTRY_ORIGIN)
#define pgd_deref(pgd) (pgd_val(pgd) & _REGION_ENTRY_ORIGIN)

static inline pud_t *pud_offset(pgd_t *pgd, unsigned long address)
{
	pud_t *pud = (pud_t *) pgd;
	if ((pgd_val(*pgd) & _REGION_ENTRY_TYPE_MASK) == _REGION_ENTRY_TYPE_R2)
		pud = (pud_t *) pgd_deref(*pgd);
	return pud  + pud_index(address);
}

static inline pmd_t *pmd_offset(pud_t *pud, unsigned long address)
{
	pmd_t *pmd = (pmd_t *) pud;
	if ((pud_val(*pud) & _REGION_ENTRY_TYPE_MASK) == _REGION_ENTRY_TYPE_R3)
		pmd = (pmd_t *) pud_deref(*pud);
	return pmd + pmd_index(address);
}

#endif /*              */

#define pfn_pte(pfn,pgprot) mk_pte_phys(__pa((pfn) << PAGE_SHIFT),(pgprot))
#define pte_pfn(x) (pte_val(x) >> PAGE_SHIFT)
#define pte_page(x) pfn_to_page(pte_pfn(x))

#define pmd_page(pmd) pfn_to_page(pmd_val(pmd) >> PAGE_SHIFT)

/*                                                */
#define pte_offset(pmd, addr) ((pte_t *) pmd_deref(*(pmd)) + pte_index(addr))
#define pte_offset_kernel(pmd, address) pte_offset(pmd,address)
#define pte_offset_map(pmd, address) pte_offset_kernel(pmd, address)
#define pte_unmap(pte) do { } while (0)

static inline void __pmd_idte(unsigned long address, pmd_t *pmdp)
{
	unsigned long sto = (unsigned long) pmdp -
			    pmd_index(address) * sizeof(pmd_t);

	if (!(pmd_val(*pmdp) & _SEGMENT_ENTRY_INV)) {
		asm volatile(
			"	.insn	rrf,0xb98e0000,%2,%3,0,0"
			: "=m" (*pmdp)
			: "m" (*pmdp), "a" (sto),
			  "a" ((address & HPAGE_MASK))
			: "cc"
		);
	}
}

#if defined(CONFIG_TRANSPARENT_HUGEPAGE) || defined(CONFIG_HUGETLB_PAGE)
static inline unsigned long massage_pgprot_pmd(pgprot_t pgprot)
{
	/*
                                                                  
                                          
  */
	if (pgprot_val(pgprot) == pgprot_val(PAGE_NONE))
		return pgprot_val(SEGMENT_NONE);
	if (pgprot_val(pgprot) == pgprot_val(PAGE_RO))
		return pgprot_val(SEGMENT_RO);
	return pgprot_val(SEGMENT_RW);
}

static inline pmd_t pmd_modify(pmd_t pmd, pgprot_t newprot)
{
	pmd_val(pmd) &= _SEGMENT_CHG_MASK;
	pmd_val(pmd) |= massage_pgprot_pmd(newprot);
	return pmd;
}

static inline pmd_t mk_pmd_phys(unsigned long physpage, pgprot_t pgprot)
{
	pmd_t __pmd;
	pmd_val(__pmd) = physpage + massage_pgprot_pmd(pgprot);
	return __pmd;
}

static inline pmd_t pmd_mkwrite(pmd_t pmd)
{
	/*                                        */
	if (!(pmd_val(pmd) & _SEGMENT_ENTRY_INV))
		pmd_val(pmd) &= ~_SEGMENT_ENTRY_RO;
	return pmd;
}
#endif /*                                                    */

#ifdef CONFIG_TRANSPARENT_HUGEPAGE

#define __HAVE_ARCH_PGTABLE_DEPOSIT
extern void pgtable_trans_huge_deposit(struct mm_struct *mm, pgtable_t pgtable);

#define __HAVE_ARCH_PGTABLE_WITHDRAW
extern pgtable_t pgtable_trans_huge_withdraw(struct mm_struct *mm);

static inline int pmd_trans_splitting(pmd_t pmd)
{
	return pmd_val(pmd) & _SEGMENT_ENTRY_SPLIT;
}

static inline void set_pmd_at(struct mm_struct *mm, unsigned long addr,
			      pmd_t *pmdp, pmd_t entry)
{
	if (!(pmd_val(entry) & _SEGMENT_ENTRY_INV) && MACHINE_HAS_EDAT1)
		pmd_val(entry) |= _SEGMENT_ENTRY_CO;
	*pmdp = entry;
}

static inline pmd_t pmd_mkhuge(pmd_t pmd)
{
	pmd_val(pmd) |= _SEGMENT_ENTRY_LARGE;
	return pmd;
}

static inline pmd_t pmd_wrprotect(pmd_t pmd)
{
	pmd_val(pmd) |= _SEGMENT_ENTRY_RO;
	return pmd;
}

static inline pmd_t pmd_mkdirty(pmd_t pmd)
{
	/*                                          */
	return pmd;
}

static inline pmd_t pmd_mkold(pmd_t pmd)
{
	/*                                               */
	return pmd;
}

static inline pmd_t pmd_mkyoung(pmd_t pmd)
{
	/*                                               */
	return pmd;
}

#define __HAVE_ARCH_PMDP_TEST_AND_CLEAR_YOUNG
static inline int pmdp_test_and_clear_young(struct vm_area_struct *vma,
					    unsigned long address, pmd_t *pmdp)
{
	unsigned long pmd_addr = pmd_val(*pmdp) & HPAGE_MASK;
	long tmp, rc;
	int counter;

	rc = 0;
	if (MACHINE_HAS_RRBM) {
		counter = PTRS_PER_PTE >> 6;
		asm volatile(
			"0:	.insn	rre,0xb9ae0000,%0,%3\n"	/*      */
			"	ogr	%1,%0\n"
			"	la	%3,0(%4,%3)\n"
			"	brct	%2,0b\n"
			: "=&d" (tmp), "+&d" (rc), "+d" (counter),
			  "+a" (pmd_addr)
			: "a" (64 * 4096UL) : "cc");
		rc = !!rc;
	} else {
		counter = PTRS_PER_PTE;
		asm volatile(
			"0:	rrbe	0,%2\n"
			"	la	%2,0(%3,%2)\n"
			"	brc	12,1f\n"
			"	lhi	%0,1\n"
			"1:	brct	%1,0b\n"
			: "+d" (rc), "+d" (counter), "+a" (pmd_addr)
			: "a" (4096UL) : "cc");
	}
	return rc;
}

#define __HAVE_ARCH_PMDP_GET_AND_CLEAR
static inline pmd_t pmdp_get_and_clear(struct mm_struct *mm,
				       unsigned long address, pmd_t *pmdp)
{
	pmd_t pmd = *pmdp;

	__pmd_idte(address, pmdp);
	pmd_clear(pmdp);
	return pmd;
}

#define __HAVE_ARCH_PMDP_CLEAR_FLUSH
static inline pmd_t pmdp_clear_flush(struct vm_area_struct *vma,
				     unsigned long address, pmd_t *pmdp)
{
	return pmdp_get_and_clear(vma->vm_mm, address, pmdp);
}

#define __HAVE_ARCH_PMDP_INVALIDATE
static inline void pmdp_invalidate(struct vm_area_struct *vma,
				   unsigned long address, pmd_t *pmdp)
{
	__pmd_idte(address, pmdp);
}

#define __HAVE_ARCH_PMDP_SET_WRPROTECT
static inline void pmdp_set_wrprotect(struct mm_struct *mm,
				      unsigned long address, pmd_t *pmdp)
{
	pmd_t pmd = *pmdp;

	if (pmd_write(pmd)) {
		__pmd_idte(address, pmdp);
		set_pmd_at(mm, address, pmdp, pmd_wrprotect(pmd));
	}
}

#define pfn_pmd(pfn, pgprot)	mk_pmd_phys(__pa((pfn) << PAGE_SHIFT), (pgprot))
#define mk_pmd(page, pgprot)	pfn_pmd(page_to_pfn(page), (pgprot))

static inline int pmd_trans_huge(pmd_t pmd)
{
	return pmd_val(pmd) & _SEGMENT_ENTRY_LARGE;
}

static inline int has_transparent_hugepage(void)
{
	return MACHINE_HAS_HPAGE ? 1 : 0;
}

static inline unsigned long pmd_pfn(pmd_t pmd)
{
	return pmd_val(pmd) >> PAGE_SHIFT;
}
#endif /*                             */

/*
                            
                                                                      
                                                                    
                                                                    
                                                                  
                              
                                                                     
                                               
                                                                      
                                             
                                                                     
                                                                      
                          
                                         
                                        
                                        
  
                            
                                                                      
                                                                 
                                                                    
                                                                  
                              
                                                                     
                                               
                                                                      
                                             
                                                                     
                                                                      
                          
                                                                         
                                                                        
                                                                        
 */
#ifndef CONFIG_64BIT
#define __SWP_OFFSET_MASK (~0UL >> 12)
#else
#define __SWP_OFFSET_MASK (~0UL >> 11)
#endif
static inline pte_t mk_swap_pte(unsigned long type, unsigned long offset)
{
	pte_t pte;
	offset &= __SWP_OFFSET_MASK;
	pte_val(pte) = _PAGE_TYPE_SWAP | ((type & 0x1f) << 2) |
		((offset & 1UL) << 7) | ((offset & ~1UL) << 11);
	return pte;
}

#define __swp_type(entry)	(((entry).val >> 2) & 0x1f)
#define __swp_offset(entry)	(((entry).val >> 11) | (((entry).val >> 7) & 1))
#define __swp_entry(type,offset) ((swp_entry_t) { pte_val(mk_swap_pte((type),(offset))) })

#define __pte_to_swp_entry(pte)	((swp_entry_t) { pte_val(pte) })
#define __swp_entry_to_pte(x)	((pte_t) { (x).val })

#ifndef CONFIG_64BIT
# define PTE_FILE_MAX_BITS	26
#else /*              */
# define PTE_FILE_MAX_BITS	59
#endif /*              */

#define pte_to_pgoff(__pte) \
	((((__pte).pte >> 12) << 7) + (((__pte).pte >> 1) & 0x7f))

#define pgoff_to_pte(__off) \
	((pte_t) { ((((__off) & 0x7f) << 1) + (((__off) >> 7) << 12)) \
		   | _PAGE_TYPE_FILE })

#endif /*               */

#define kern_addr_valid(addr)   (1)

extern int vmem_add_mapping(unsigned long start, unsigned long size);
extern int vmem_remove_mapping(unsigned long start, unsigned long size);
extern int s390_enable_sie(void);

/*
                                     
 */
static inline void pgtable_cache_init(void) { }
static inline void check_pgt_cache(void) { }

#include <asm-generic/pgtable.h>

#endif /*              */
