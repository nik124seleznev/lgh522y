#ifndef _ASM_POWERPC_PGALLOC_H
#define _ASM_POWERPC_PGALLOC_H
#ifdef __KERNEL__

#include <linux/mm.h>
#include <asm-generic/tlb.h>

#ifdef CONFIG_PPC_BOOK3E
extern void tlb_flush_pgtable(struct mmu_gather *tlb, unsigned long address);
#else /*                   */
static inline void tlb_flush_pgtable(struct mmu_gather *tlb,
				     unsigned long address)
{
}
#endif /*                    */

#ifdef CONFIG_PPC64
#include <asm/pgalloc-64.h>
#else
#include <asm/pgalloc-32.h>
#endif

#endif /*            */
#endif /*                        */
