/*
 *  Meta version derived from arch/powerpc/lib/dma-noncoherent.c
 *    Copyright (C) 2008 Imagination Technologies Ltd.
 *
 *  PowerPC version derived from arch/arm/mm/consistent.c
 *    Copyright (C) 2001 Dan Malek (dmalek@jlc.net)
 *
 *  Copyright (C) 2000 Russell King
 *
 * Consistent memory allocators.  Used for DMA devices that want to
 * share uncached memory with the processor core.  The function return
 * is the virtual address and 'dma_handle' is the physical address.
 * Mostly stolen from the ARM port, with some changes for PowerPC.
 *						-- Dan
 *
 * Reorganized to get rid of the arch-specific consistent_* functions
 * and provide non-coherent implementations for the DMA API. -Matt
 *
 * Added in_interrupt() safe dma_alloc_coherent()/dma_free_coherent()
 * implementation. This is pulled straight from ARM and barely
 * modified. -Matt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/export.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/highmem.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>

#include <asm/tlbflush.h>
#include <asm/mmu.h>

#define CONSISTENT_OFFSET(x)	(((unsigned long)(x) - CONSISTENT_START) \
					>> PAGE_SHIFT)

static u64 get_coherent_dma_mask(struct device *dev)
{
	u64 mask = ~0ULL;

	if (dev) {
		mask = dev->coherent_dma_mask;

		/*
                                                         
                                                      
   */
		if (mask == 0) {
			dev_warn(dev, "coherent DMA mask is unset\n");
			return 0;
		}
	}

	return mask;
}
/*
                                                                             
 */
static pte_t *consistent_pte;
static DEFINE_SPINLOCK(consistent_lock);

/*
                              
  
                                                                           
                                                    
  
                                                        
  
                      
                                    
                          
                          
                            
                              
      
  
                                                                          
                                    
  
                                           
                                                   
                             
                          
      
  
                                                                            
                                                                             
                                                           
                           
 */
struct metag_vm_region {
	struct list_head vm_list;
	unsigned long vm_start;
	unsigned long vm_end;
	struct page		*vm_pages;
	int			vm_active;
};

static struct metag_vm_region consistent_head = {
	.vm_list = LIST_HEAD_INIT(consistent_head.vm_list),
	.vm_start = CONSISTENT_START,
	.vm_end = CONSISTENT_END,
};

static struct metag_vm_region *metag_vm_region_alloc(struct metag_vm_region
						     *head, size_t size,
						     gfp_t gfp)
{
	unsigned long addr = head->vm_start, end = head->vm_end - size;
	unsigned long flags;
	struct metag_vm_region *c, *new;

	new = kmalloc(sizeof(struct metag_vm_region), gfp);
	if (!new)
		goto out;

	spin_lock_irqsave(&consistent_lock, flags);

	list_for_each_entry(c, &head->vm_list, vm_list) {
		if ((addr + size) < addr)
			goto nospc;
		if ((addr + size) <= c->vm_start)
			goto found;
		addr = c->vm_end;
		if (addr > end)
			goto nospc;
	}

found:
	/*
                                                
  */
	list_add_tail(&new->vm_list, &c->vm_list);
	new->vm_start = addr;
	new->vm_end = addr + size;
	new->vm_active = 1;

	spin_unlock_irqrestore(&consistent_lock, flags);
	return new;

nospc:
	spin_unlock_irqrestore(&consistent_lock, flags);
	kfree(new);
out:
	return NULL;
}

static struct metag_vm_region *metag_vm_region_find(struct metag_vm_region
						    *head, unsigned long addr)
{
	struct metag_vm_region *c;

	list_for_each_entry(c, &head->vm_list, vm_list) {
		if (c->vm_active && c->vm_start == addr)
			goto out;
	}
	c = NULL;
out:
	return c;
}

/*
                                                                         
                                          
 */
void *dma_alloc_coherent(struct device *dev, size_t size,
			 dma_addr_t *handle, gfp_t gfp)
{
	struct page *page;
	struct metag_vm_region *c;
	unsigned long order;
	u64 mask = get_coherent_dma_mask(dev);
	u64 limit;

	if (!consistent_pte) {
		pr_err("%s: not initialised\n", __func__);
		dump_stack();
		return NULL;
	}

	if (!mask)
		goto no_page;
	size = PAGE_ALIGN(size);
	limit = (mask + 1) & ~mask;
	if ((limit && size >= limit)
	    || size >= (CONSISTENT_END - CONSISTENT_START)) {
		pr_warn("coherent allocation too big (requested %#x mask %#Lx)\n",
			size, mask);
		return NULL;
	}

	order = get_order(size);

	if (mask != 0xffffffff)
		gfp |= GFP_DMA;

	page = alloc_pages(gfp, order);
	if (!page)
		goto no_page;

	/*
                                                    
                                               
  */
	{
		void *kaddr = page_address(page);
		memset(kaddr, 0, size);
		flush_dcache_region(kaddr, size);
	}

	/*
                                                                
  */
	c = metag_vm_region_alloc(&consistent_head, size,
				  gfp & ~(__GFP_DMA | __GFP_HIGHMEM));
	if (c) {
		unsigned long vaddr = c->vm_start;
		pte_t *pte = consistent_pte + CONSISTENT_OFFSET(vaddr);
		struct page *end = page + (1 << order);

		c->vm_pages = page;
		split_page(page, order);

		/*
                         
   */
		*handle = page_to_bus(page);

		do {
			BUG_ON(!pte_none(*pte));

			SetPageReserved(page);
			set_pte_at(&init_mm, vaddr,
				   pte, mk_pte(page,
					       pgprot_writecombine
					       (PAGE_KERNEL)));
			page++;
			pte++;
			vaddr += PAGE_SIZE;
		} while (size -= PAGE_SIZE);

		/*
                                     
   */
		while (page < end) {
			__free_page(page);
			page++;
		}

		return (void *)c->vm_start;
	}

	if (page)
		__free_pages(page, order);
no_page:
	return NULL;
}
EXPORT_SYMBOL(dma_alloc_coherent);

/*
                                               
 */
void dma_free_coherent(struct device *dev, size_t size,
		       void *vaddr, dma_addr_t dma_handle)
{
	struct metag_vm_region *c;
	unsigned long flags, addr;
	pte_t *ptep;

	size = PAGE_ALIGN(size);

	spin_lock_irqsave(&consistent_lock, flags);

	c = metag_vm_region_find(&consistent_head, (unsigned long)vaddr);
	if (!c)
		goto no_area;

	c->vm_active = 0;
	if ((c->vm_end - c->vm_start) != size) {
		pr_err("%s: freeing wrong coherent size (%ld != %d)\n",
		       __func__, c->vm_end - c->vm_start, size);
		dump_stack();
		size = c->vm_end - c->vm_start;
	}

	ptep = consistent_pte + CONSISTENT_OFFSET(c->vm_start);
	addr = c->vm_start;
	do {
		pte_t pte = ptep_get_and_clear(&init_mm, addr, ptep);
		unsigned long pfn;

		ptep++;
		addr += PAGE_SIZE;

		if (!pte_none(pte) && pte_present(pte)) {
			pfn = pte_pfn(pte);

			if (pfn_valid(pfn)) {
				struct page *page = pfn_to_page(pfn);
				ClearPageReserved(page);

				__free_page(page);
				continue;
			}
		}

		pr_crit("%s: bad page in kernel page table\n",
			__func__);
	} while (size -= PAGE_SIZE);

	flush_tlb_kernel_range(c->vm_start, c->vm_end);

	list_del(&c->vm_list);

	spin_unlock_irqrestore(&consistent_lock, flags);

	kfree(c);
	return;

no_area:
	spin_unlock_irqrestore(&consistent_lock, flags);
	pr_err("%s: trying to free invalid coherent area: %p\n",
	       __func__, vaddr);
	dump_stack();
}
EXPORT_SYMBOL(dma_free_coherent);


static int dma_mmap(struct device *dev, struct vm_area_struct *vma,
		    void *cpu_addr, dma_addr_t dma_addr, size_t size)
{
	int ret = -ENXIO;

	unsigned long flags, user_size, kern_size;
	struct metag_vm_region *c;

	user_size = (vma->vm_end - vma->vm_start) >> PAGE_SHIFT;

	spin_lock_irqsave(&consistent_lock, flags);
	c = metag_vm_region_find(&consistent_head, (unsigned long)cpu_addr);
	spin_unlock_irqrestore(&consistent_lock, flags);

	if (c) {
		unsigned long off = vma->vm_pgoff;

		kern_size = (c->vm_end - c->vm_start) >> PAGE_SHIFT;

		if (off < kern_size &&
		    user_size <= (kern_size - off)) {
			ret = remap_pfn_range(vma, vma->vm_start,
					      page_to_pfn(c->vm_pages) + off,
					      user_size << PAGE_SHIFT,
					      vma->vm_page_prot);
		}
	}


	return ret;
}

int dma_mmap_coherent(struct device *dev, struct vm_area_struct *vma,
		      void *cpu_addr, dma_addr_t dma_addr, size_t size)
{
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	return dma_mmap(dev, vma, cpu_addr, dma_addr, size);
}
EXPORT_SYMBOL(dma_mmap_coherent);

int dma_mmap_writecombine(struct device *dev, struct vm_area_struct *vma,
			  void *cpu_addr, dma_addr_t dma_addr, size_t size)
{
	vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);
	return dma_mmap(dev, vma, cpu_addr, dma_addr, size);
}
EXPORT_SYMBOL(dma_mmap_writecombine);




/*
                                               
 */
static int __init dma_alloc_init(void)
{
	pgd_t *pgd, *pgd_k;
	pud_t *pud, *pud_k;
	pmd_t *pmd, *pmd_k;
	pte_t *pte;
	int ret = 0;

	do {
		int offset = pgd_index(CONSISTENT_START);
		pgd = pgd_offset(&init_mm, CONSISTENT_START);
		pud = pud_alloc(&init_mm, pgd, CONSISTENT_START);
		pmd = pmd_alloc(&init_mm, pud, CONSISTENT_START);
		if (!pmd) {
			pr_err("%s: no pmd tables\n", __func__);
			ret = -ENOMEM;
			break;
		}
		WARN_ON(!pmd_none(*pmd));

		pte = pte_alloc_kernel(pmd, CONSISTENT_START);
		if (!pte) {
			pr_err("%s: no pte tables\n", __func__);
			ret = -ENOMEM;
			break;
		}

		pgd_k = ((pgd_t *) mmu_get_base()) + offset;
		pud_k = pud_offset(pgd_k, CONSISTENT_START);
		pmd_k = pmd_offset(pud_k, CONSISTENT_START);
		set_pmd(pmd_k, *pmd);

		consistent_pte = pte;
	} while (0);

	return ret;
}
early_initcall(dma_alloc_init);

/*
                                      
 */
void dma_sync_for_device(void *vaddr, size_t size, int dma_direction)
{
	/*
                                                                       
                                                                     
                                                             
  */

	barrier();

	switch (dma_direction) {
	case DMA_BIDIRECTIONAL:
		/*
                                                                  
                                                             
                                                           
                              
   */
		flush_dcache_region(vaddr, size);
		break;
	case DMA_TO_DEVICE:
		/*
                                                               
                                                                
                   
   */
		writeback_dcache_region(vaddr, size);
		break;
	case DMA_FROM_DEVICE:
		/*
                                                               
                                                         
                                   
   */
		invalidate_dcache_region(vaddr, size);
		break;
	case DMA_NONE:
		BUG();
	}

	wmb();
}
EXPORT_SYMBOL(dma_sync_for_device);

/*
                                       
 */
void dma_sync_for_cpu(void *vaddr, size_t size, int dma_direction)
{
	/*
                                                               
                                                                    
                                                                    
                                           
   
                                                                        
                                                    
  */
	if (_meta_l2c_pf_is_enabled()) {
		switch (dma_direction) {
		case DMA_BIDIRECTIONAL:
		case DMA_FROM_DEVICE:
			invalidate_dcache_region(vaddr, size);
			break;
		case DMA_TO_DEVICE:
			/*                                                 */
			break;
		case DMA_NONE:
			BUG();
		}
	}

	rmb();
}
EXPORT_SYMBOL(dma_sync_for_cpu);
