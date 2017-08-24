#include <linux/export.h>
#include <linux/highmem.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <asm/fixmap.h>
#include <asm/tlbflush.h>

static pte_t *kmap_pte;

unsigned long highstart_pfn, highend_pfn;

void *kmap(struct page *page)
{
	might_sleep();
	if (!PageHighMem(page))
		return page_address(page);
	return kmap_high(page);
}
EXPORT_SYMBOL(kmap);

void kunmap(struct page *page)
{
	BUG_ON(in_interrupt());
	if (!PageHighMem(page))
		return;
	kunmap_high(page);
}
EXPORT_SYMBOL(kunmap);

/*
                                                                             
                                                                               
                                         
  
                                                                          
                                                          
 */

void *kmap_atomic(struct page *page)
{
	enum fixed_addresses idx;
	unsigned long vaddr;
	int type;

	/*                                                                 */
	pagefault_disable();
	if (!PageHighMem(page))
		return page_address(page);

	type = kmap_atomic_idx_push();
	idx = type + KM_TYPE_NR * smp_processor_id();
	vaddr = __fix_to_virt(FIX_KMAP_BEGIN + idx);
#ifdef CONFIG_DEBUG_HIGHMEM
	BUG_ON(!pte_none(*(kmap_pte - idx)));
#endif
	set_pte(kmap_pte - idx, mk_pte(page, PAGE_KERNEL));

	return (void *)vaddr;
}
EXPORT_SYMBOL(kmap_atomic);

void __kunmap_atomic(void *kvaddr)
{
	unsigned long vaddr = (unsigned long) kvaddr & PAGE_MASK;
	int idx, type;

	if (kvaddr >= (void *)FIXADDR_START) {
		type = kmap_atomic_idx();
		idx = type + KM_TYPE_NR * smp_processor_id();

		/*
                                                               
                                                               
                                                              
                                                            
   */
		pte_clear(&init_mm, vaddr, kmap_pte-idx);
		flush_tlb_kernel_range(vaddr, vaddr + PAGE_SIZE);

		kmap_atomic_idx_pop();
	}

	pagefault_enable();
}
EXPORT_SYMBOL(__kunmap_atomic);

/*
                                                                    
                                         
 */
void *kmap_atomic_pfn(unsigned long pfn)
{
	enum fixed_addresses idx;
	unsigned long vaddr;
	int type;

	pagefault_disable();

	type = kmap_atomic_idx_push();
	idx = type + KM_TYPE_NR * smp_processor_id();
	vaddr = __fix_to_virt(FIX_KMAP_BEGIN + idx);
#ifdef CONFIG_DEBUG_HIGHMEM
	BUG_ON(!pte_none(*(kmap_pte - idx)));
#endif
	set_pte(kmap_pte - idx, pfn_pte(pfn, PAGE_KERNEL));
	flush_tlb_kernel_range(vaddr, vaddr + PAGE_SIZE);

	return (void *)vaddr;
}

struct page *kmap_atomic_to_page(void *ptr)
{
	unsigned long vaddr = (unsigned long)ptr;
	int idx;
	pte_t *pte;

	if (vaddr < FIXADDR_START)
		return virt_to_page(ptr);

	idx = virt_to_fix(vaddr);
	pte = kmap_pte - (idx - FIX_KMAP_BEGIN);
	return pte_page(*pte);
}

void __init kmap_init(void)
{
	unsigned long kmap_vstart;

	/*                          */
	kmap_vstart = __fix_to_virt(FIX_KMAP_BEGIN);
	kmap_pte = kmap_get_fixmap_pte(kmap_vstart);
}
