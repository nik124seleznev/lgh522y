/*
                                                             
  
                                                                       
                                                                  
                                                               
                               
                                                          
  
                                                              
                                                               
 */

/*
                                                                    
                                                                    
                                                                       
  
                                                                    
                                                                  
                                                                
                                                                       
                                                                   
 */
#include <linux/highmem.h>
#include <linux/export.h>
#include <linux/mm.h>

#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/pgalloc.h>
#include <asm/vaddrs.h>

pgprot_t kmap_prot;

static pte_t *kmap_pte;

void __init kmap_init(void)
{
	unsigned long address;
	pmd_t *dir;

	address = __fix_to_virt(FIX_KMAP_BEGIN);
	dir = pmd_offset(pgd_offset_k(address), address);

        /*                          */
        kmap_pte = pte_offset_kernel(dir, address);
        kmap_prot = __pgprot(SRMMU_ET_PTE | SRMMU_PRIV | SRMMU_CACHE);
}

void *kmap_atomic(struct page *page)
{
	unsigned long vaddr;
	long idx, type;

	/*                                                                 */
	pagefault_disable();
	if (!PageHighMem(page))
		return page_address(page);

	type = kmap_atomic_idx_push();
	idx = type + KM_TYPE_NR*smp_processor_id();
	vaddr = __fix_to_virt(FIX_KMAP_BEGIN + idx);

/*                 */
#if 0
	__flush_cache_one(vaddr);
#else
	flush_cache_all();
#endif

#ifdef CONFIG_DEBUG_HIGHMEM
	BUG_ON(!pte_none(*(kmap_pte-idx)));
#endif
	set_pte(kmap_pte-idx, mk_pte(page, kmap_prot));
/*                 */
#if 0
	__flush_tlb_one(vaddr);
#else
	flush_tlb_all();
#endif

	return (void*) vaddr;
}
EXPORT_SYMBOL(kmap_atomic);

void __kunmap_atomic(void *kvaddr)
{
	unsigned long vaddr = (unsigned long) kvaddr & PAGE_MASK;
	int type;

	if (vaddr < FIXADDR_START) { //      
		pagefault_enable();
		return;
	}

	type = kmap_atomic_idx();

#ifdef CONFIG_DEBUG_HIGHMEM
	{
		unsigned long idx;

		idx = type + KM_TYPE_NR * smp_processor_id();
		BUG_ON(vaddr != __fix_to_virt(FIX_KMAP_BEGIN+idx));

		/*                 */
#if 0
		__flush_cache_one(vaddr);
#else
		flush_cache_all();
#endif

		/*
                                                          
                                    
   */
		pte_clear(&init_mm, vaddr, kmap_pte-idx);
		/*                 */
#if 0
		__flush_tlb_one(vaddr);
#else
		flush_tlb_all();
#endif
	}
#endif

	kmap_atomic_idx_pop();
	pagefault_enable();
}
EXPORT_SYMBOL(__kunmap_atomic);
