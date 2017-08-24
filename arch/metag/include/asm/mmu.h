#ifndef __MMU_H
#define __MMU_H

#ifdef CONFIG_METAG_USER_TCM
#include <linux/list.h>
#endif

#ifdef CONFIG_HUGETLB_PAGE
#include <asm/page.h>
#endif

typedef struct {
	/*                                                  */
	unsigned long pgd_base;
#ifdef CONFIG_METAG_USER_TCM
	struct list_head tcm;
#endif
#ifdef CONFIG_HUGETLB_PAGE
#if HPAGE_SHIFT < HUGEPT_SHIFT
	/*                                               */
	unsigned long part_huge;
#endif
#endif
} mm_context_t;

/*                                                                     
                          
                                                        
 */
unsigned long mmu_read_first_level_page(unsigned long vaddr);

/*                                                                 
                                                                   
 */
unsigned long mmu_read_second_level_page(unsigned long vaddr);

/*                                         */
unsigned long mmu_get_base(void);

/*                     */
void mmu_init(unsigned long mem_end);

#ifdef CONFIG_METAG_META21_MMU
/*
                                                        
                                                        
                                                        
 */
static inline unsigned long mmu_phys0_addr(unsigned int cpu)
{
	unsigned long phys0;

	phys0 = (MMCU_T0LOCAL_TABLE_PHYS0 +
		(MMCU_TnX_TABLE_PHYSX_STRIDE * cpu)) +
		(MMCU_TXG_TABLE_PHYSX_OFFSET * is_global_space(PAGE_OFFSET));

	return phys0;
}

/*
                                                        
                                                        
                                                        
 */
static inline unsigned long mmu_phys1_addr(unsigned int cpu)
{
	unsigned long phys1;

	phys1 = (MMCU_T0LOCAL_TABLE_PHYS1 +
		(MMCU_TnX_TABLE_PHYSX_STRIDE * cpu)) +
		(MMCU_TXG_TABLE_PHYSX_OFFSET * is_global_space(PAGE_OFFSET));

	return phys1;
}
#endif /*                         */

#endif
