/*
 * Copyright (C) 2001 Mike Corrigan & Dave Engebretsen, IBM Corporation
 * 
 * Rewrite, cleanup, new allocation schemes, virtual merging: 
 * Copyright (C) 2004 Olof Johansson, IBM Corporation
 *               and  Ben. Herrenschmidt, IBM Corporation
 *
 * Dynamic DMA mapping support, bus-independent parts.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/dma-mapping.h>
#include <linux/bitmap.h>
#include <linux/iommu-helper.h>
#include <linux/crash_dump.h>
#include <linux/hash.h>
#include <linux/fault-inject.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <asm/prom.h>
#include <asm/iommu.h>
#include <asm/pci-bridge.h>
#include <asm/machdep.h>
#include <asm/kdump.h>
#include <asm/fadump.h>
#include <asm/vio.h>

#define DBG(...)

static int novmerge;

static void __iommu_free(struct iommu_table *, dma_addr_t, unsigned int);

static int __init setup_iommu(char *str)
{
	if (!strcmp(str, "novmerge"))
		novmerge = 1;
	else if (!strcmp(str, "vmerge"))
		novmerge = 0;
	return 1;
}

__setup("iommu=", setup_iommu);

static DEFINE_PER_CPU(unsigned int, iommu_pool_hash);

/*
                                                                  
  
                                                                          
                                                                           
                                                               
 */
static int __init setup_iommu_pool_hash(void)
{
	unsigned int i;

	for_each_possible_cpu(i)
		per_cpu(iommu_pool_hash, i) = hash_32(i, IOMMU_POOL_HASHBITS);

	return 0;
}
subsys_initcall(setup_iommu_pool_hash);

#ifdef CONFIG_FAIL_IOMMU

static DECLARE_FAULT_ATTR(fail_iommu);

static int __init setup_fail_iommu(char *str)
{
	return setup_fault_attr(&fail_iommu, str);
}
__setup("fail_iommu=", setup_fail_iommu);

static bool should_fail_iommu(struct device *dev)
{
	return dev->archdata.fail_iommu && should_fail(&fail_iommu, 1);
}

static int __init fail_iommu_debugfs(void)
{
	struct dentry *dir = fault_create_debugfs_attr("fail_iommu",
						       NULL, &fail_iommu);

	return PTR_RET(dir);
}
late_initcall(fail_iommu_debugfs);

static ssize_t fail_iommu_show(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", dev->archdata.fail_iommu);
}

static ssize_t fail_iommu_store(struct device *dev,
				struct device_attribute *attr, const char *buf,
				size_t count)
{
	int i;

	if (count > 0 && sscanf(buf, "%d", &i) > 0)
		dev->archdata.fail_iommu = (i == 0) ? 0 : 1;

	return count;
}

static DEVICE_ATTR(fail_iommu, S_IRUGO|S_IWUSR, fail_iommu_show,
		   fail_iommu_store);

static int fail_iommu_bus_notify(struct notifier_block *nb,
				 unsigned long action, void *data)
{
	struct device *dev = data;

	if (action == BUS_NOTIFY_ADD_DEVICE) {
		if (device_create_file(dev, &dev_attr_fail_iommu))
			pr_warn("Unable to create IOMMU fault injection sysfs "
				"entries\n");
	} else if (action == BUS_NOTIFY_DEL_DEVICE) {
		device_remove_file(dev, &dev_attr_fail_iommu);
	}

	return 0;
}

static struct notifier_block fail_iommu_bus_notifier = {
	.notifier_call = fail_iommu_bus_notify
};

static int __init fail_iommu_setup(void)
{
#ifdef CONFIG_PCI
	bus_register_notifier(&pci_bus_type, &fail_iommu_bus_notifier);
#endif
#ifdef CONFIG_IBMVIO
	bus_register_notifier(&vio_bus_type, &fail_iommu_bus_notifier);
#endif

	return 0;
}
/*
                                                                       
                      
 */
arch_initcall(fail_iommu_setup);
#else
static inline bool should_fail_iommu(struct device *dev)
{
	return false;
}
#endif

static unsigned long iommu_range_alloc(struct device *dev,
				       struct iommu_table *tbl,
                                       unsigned long npages,
                                       unsigned long *handle,
                                       unsigned long mask,
                                       unsigned int align_order)
{ 
	unsigned long n, end, start;
	unsigned long limit;
	int largealloc = npages > 15;
	int pass = 0;
	unsigned long align_mask;
	unsigned long boundary_size;
	unsigned long flags;
	unsigned int pool_nr;
	struct iommu_pool *pool;

	align_mask = 0xffffffffffffffffl >> (64 - align_order);

	/*                                                            */

	/*              */
	if (unlikely(npages == 0)) {
		if (printk_ratelimit())
			WARN_ON(1);
		return DMA_ERROR_CODE;
	}

	if (should_fail_iommu(dev))
		return DMA_ERROR_CODE;

	/*
                                                                
                              
  */
	pool_nr = __raw_get_cpu_var(iommu_pool_hash) & (tbl->nr_pools - 1);

	if (largealloc)
		pool = &(tbl->large_pool);
	else
		pool = &(tbl->pools[pool_nr]);

	spin_lock_irqsave(&(pool->lock), flags);

again:
	if ((pass == 0) && handle && *handle &&
	    (*handle >= pool->start) && (*handle < pool->end))
		start = *handle;
	else
		start = pool->hint;

	limit = pool->end;

	/*                                                              
                                                                 
                                                             
  */
	if (start >= limit)
		start = pool->start;

	if (limit + tbl->it_offset > mask) {
		limit = mask - tbl->it_offset + 1;
		/*                                                 
                                                        
                                              
   */
		if ((start & mask) >= limit || pass > 0) {
			spin_unlock(&(pool->lock));
			pool = &(tbl->pools[0]);
			spin_lock(&(pool->lock));
			start = pool->start;
		} else {
			start &= mask;
		}
	}

	if (dev)
		boundary_size = ALIGN(dma_get_seg_boundary(dev) + 1,
				      1 << IOMMU_PAGE_SHIFT);
	else
		boundary_size = ALIGN(1UL << 32, 1 << IOMMU_PAGE_SHIFT);
	/*                                                      */

	n = iommu_area_alloc(tbl->it_map, limit, start, npages,
			     tbl->it_offset, boundary_size >> IOMMU_PAGE_SHIFT,
			     align_mask);
	if (n == -1) {
		if (likely(pass == 0)) {
			/*                                   */
			pool->hint = pool->start;
			pass++;
			goto again;

		} else if (pass <= tbl->nr_pools) {
			/*                                      */
			spin_unlock(&(pool->lock));
			pool_nr = (pool_nr + 1) & (tbl->nr_pools - 1);
			pool = &tbl->pools[pool_nr];
			spin_lock(&(pool->lock));
			pool->hint = pool->start;
			pass++;
			goto again;

		} else {
			/*         */
			spin_unlock_irqrestore(&(pool->lock), flags);
			return DMA_ERROR_CODE;
		}
	}

	end = n + npages;

	/*                                                */
	if (largealloc) {
		/*                                                */
		pool->hint = end;
	} else {
		/*                                                       */
		pool->hint = (end + tbl->it_blocksize - 1) &
		                ~(tbl->it_blocksize - 1);
	}

	/*                                  */
	if (handle)
		*handle = end;

	spin_unlock_irqrestore(&(pool->lock), flags);

	return n;
}

static dma_addr_t iommu_alloc(struct device *dev, struct iommu_table *tbl,
			      void *page, unsigned int npages,
			      enum dma_data_direction direction,
			      unsigned long mask, unsigned int align_order,
			      struct dma_attrs *attrs)
{
	unsigned long entry;
	dma_addr_t ret = DMA_ERROR_CODE;
	int build_fail;

	entry = iommu_range_alloc(dev, tbl, npages, NULL, mask, align_order);

	if (unlikely(entry == DMA_ERROR_CODE))
		return DMA_ERROR_CODE;

	entry += tbl->it_offset;	/*                            */
	ret = entry << IOMMU_PAGE_SHIFT;	/*                            */

	/*                              */
	build_fail = ppc_md.tce_build(tbl, entry, npages,
	                              (unsigned long)page & IOMMU_PAGE_MASK,
	                              direction, attrs);

	/*                                                               
                                                     
                                                             
                
  */
	if (unlikely(build_fail)) {
		__iommu_free(tbl, ret, npages);
		return DMA_ERROR_CODE;
	}

	/*                                          */
	if (ppc_md.tce_flush)
		ppc_md.tce_flush(tbl);

	/*                                        */
	mb();

	return ret;
}

static bool iommu_free_check(struct iommu_table *tbl, dma_addr_t dma_addr,
			     unsigned int npages)
{
	unsigned long entry, free_entry;

	entry = dma_addr >> IOMMU_PAGE_SHIFT;
	free_entry = entry - tbl->it_offset;

	if (((free_entry + npages) > tbl->it_size) ||
	    (entry < tbl->it_offset)) {
		if (printk_ratelimit()) {
			printk(KERN_INFO "iommu_free: invalid entry\n");
			printk(KERN_INFO "\tentry     = 0x%lx\n", entry); 
			printk(KERN_INFO "\tdma_addr  = 0x%llx\n", (u64)dma_addr);
			printk(KERN_INFO "\tTable     = 0x%llx\n", (u64)tbl);
			printk(KERN_INFO "\tbus#      = 0x%llx\n", (u64)tbl->it_busno);
			printk(KERN_INFO "\tsize      = 0x%llx\n", (u64)tbl->it_size);
			printk(KERN_INFO "\tstartOff  = 0x%llx\n", (u64)tbl->it_offset);
			printk(KERN_INFO "\tindex     = 0x%llx\n", (u64)tbl->it_index);
			WARN_ON(1);
		}

		return false;
	}

	return true;
}

static struct iommu_pool *get_pool(struct iommu_table *tbl,
				   unsigned long entry)
{
	struct iommu_pool *p;
	unsigned long largepool_start = tbl->large_pool.start;

	/*                                                         */
	if (entry >= largepool_start) {
		p = &tbl->large_pool;
	} else {
		unsigned int pool_nr = entry / tbl->poolsize;

		BUG_ON(pool_nr > tbl->nr_pools);
		p = &tbl->pools[pool_nr];
	}

	return p;
}

static void __iommu_free(struct iommu_table *tbl, dma_addr_t dma_addr,
			 unsigned int npages)
{
	unsigned long entry, free_entry;
	unsigned long flags;
	struct iommu_pool *pool;

	entry = dma_addr >> IOMMU_PAGE_SHIFT;
	free_entry = entry - tbl->it_offset;

	pool = get_pool(tbl, free_entry);

	if (!iommu_free_check(tbl, dma_addr, npages))
		return;

	ppc_md.tce_free(tbl, entry, npages);

	spin_lock_irqsave(&(pool->lock), flags);
	bitmap_clear(tbl->it_map, free_entry, npages);
	spin_unlock_irqrestore(&(pool->lock), flags);
}

static void iommu_free(struct iommu_table *tbl, dma_addr_t dma_addr,
		unsigned int npages)
{
	__iommu_free(tbl, dma_addr, npages);

	/*                                                         
                                                              
                          
  */
	if (ppc_md.tce_flush)
		ppc_md.tce_flush(tbl);
}

int iommu_map_sg(struct device *dev, struct iommu_table *tbl,
		 struct scatterlist *sglist, int nelems,
		 unsigned long mask, enum dma_data_direction direction,
		 struct dma_attrs *attrs)
{
	dma_addr_t dma_next = 0, dma_addr;
	struct scatterlist *s, *outs, *segstart;
	int outcount, incount, i, build_fail = 0;
	unsigned int align;
	unsigned long handle;
	unsigned int max_seg_size;

	BUG_ON(direction == DMA_NONE);

	if ((nelems == 0) || !tbl)
		return 0;

	outs = s = segstart = &sglist[0];
	outcount = 1;
	incount = nelems;
	handle = 0;

	/*                                                  */
	outs->dma_length = 0;

	DBG("sg mapping %d elements:\n", nelems);

	max_seg_size = dma_get_max_seg_size(dev);
	for_each_sg(sglist, s, nelems, i) {
		unsigned long vaddr, npages, entry, slen;

		slen = s->length;
		/*              */
		if (slen == 0) {
			dma_next = 0;
			continue;
		}
		/*                                         */
		vaddr = (unsigned long) sg_virt(s);
		npages = iommu_num_pages(vaddr, slen, IOMMU_PAGE_SIZE);
		align = 0;
		if (IOMMU_PAGE_SHIFT < PAGE_SHIFT && slen >= PAGE_SIZE &&
		    (vaddr & ~PAGE_MASK) == 0)
			align = PAGE_SHIFT - IOMMU_PAGE_SHIFT;
		entry = iommu_range_alloc(dev, tbl, npages, &handle,
					  mask >> IOMMU_PAGE_SHIFT, align);

		DBG("  - vaddr: %lx, size: %lx\n", vaddr, slen);

		/*                */
		if (unlikely(entry == DMA_ERROR_CODE)) {
			if (printk_ratelimit())
				dev_info(dev, "iommu_alloc failed, tbl %p "
					 "vaddr %lx npages %lu\n", tbl, vaddr,
					 npages);
			goto failure;
		}

		/*                               */
		entry += tbl->it_offset;
		dma_addr = entry << IOMMU_PAGE_SHIFT;
		dma_addr |= (s->offset & ~IOMMU_PAGE_MASK);

		DBG("  - %lu pages, entry: %lx, dma_addr: %lx\n",
			    npages, entry, dma_addr);

		/*                      */
		build_fail = ppc_md.tce_build(tbl, entry, npages,
		                              vaddr & IOMMU_PAGE_MASK,
		                              direction, attrs);
		if(unlikely(build_fail))
			goto failure;

		/*                                           */
		if (segstart != s) {
			DBG("  - trying merge...\n");
			/*                    
                                                                  
    */
			if (novmerge || (dma_addr != dma_next) ||
			    (outs->dma_length + s->length > max_seg_size)) {
				/*                                   */
				segstart = s;
				outcount++;
				outs = sg_next(outs);
				DBG("    can't merge, new segment.\n");
			} else {
				outs->dma_length += s->length;
				DBG("    merged, new len: %ux\n", outs->dma_length);
			}
		}

		if (segstart == s) {
			/*                                     */
			DBG("  - filling new segment.\n");
			outs->dma_address = dma_addr;
			outs->dma_length = slen;
		}

		/*                                                  */
		dma_next = dma_addr + slen;

		DBG("  - dma next is: %lx\n", dma_next);
	}

	/*                                          */
	if (ppc_md.tce_flush)
		ppc_md.tce_flush(tbl);

	DBG("mapped %d elements:\n", outcount);

	/*                                                               
                                                                  
  */
	if (outcount < incount) {
		outs = sg_next(outs);
		outs->dma_address = DMA_ERROR_CODE;
		outs->dma_length = 0;
	}

	/*                                        */
	mb();

	return outcount;

 failure:
	for_each_sg(sglist, s, nelems, i) {
		if (s->dma_length != 0) {
			unsigned long vaddr, npages;

			vaddr = s->dma_address & IOMMU_PAGE_MASK;
			npages = iommu_num_pages(s->dma_address, s->dma_length,
						 IOMMU_PAGE_SIZE);
			__iommu_free(tbl, vaddr, npages);
			s->dma_address = DMA_ERROR_CODE;
			s->dma_length = 0;
		}
		if (s == outs)
			break;
	}
	return 0;
}


void iommu_unmap_sg(struct iommu_table *tbl, struct scatterlist *sglist,
		int nelems, enum dma_data_direction direction,
		struct dma_attrs *attrs)
{
	struct scatterlist *sg;

	BUG_ON(direction == DMA_NONE);

	if (!tbl)
		return;

	sg = sglist;
	while (nelems--) {
		unsigned int npages;
		dma_addr_t dma_handle = sg->dma_address;

		if (sg->dma_length == 0)
			break;
		npages = iommu_num_pages(dma_handle, sg->dma_length,
					 IOMMU_PAGE_SIZE);
		__iommu_free(tbl, dma_handle, npages);
		sg = sg_next(sg);
	}

	/*                                                            
                                                                 
                 
  */
	if (ppc_md.tce_flush)
		ppc_md.tce_flush(tbl);
}

static void iommu_table_clear(struct iommu_table *tbl)
{
	/*
                                                               
                                                                  
                                                              
  */
	if (!is_kdump_kernel() || is_fadump_active()) {
		/*                                                         */
		ppc_md.tce_free(tbl, tbl->it_offset, tbl->it_size);
		return;
	}

#ifdef CONFIG_CRASH_DUMP
	if (ppc_md.tce_get) {
		unsigned long index, tceval, tcecount = 0;

		/*                                                         */
		for (index = 0; index < tbl->it_size; index++) {
			tceval = ppc_md.tce_get(tbl, index + tbl->it_offset);
			/*
                                                         
    */
			if (tceval && (tceval != 0x7fffffffffffffffUL)) {
				__set_bit(index, tbl->it_map);
				tcecount++;
			}
		}

		if ((tbl->it_size - tcecount) < KDUMP_MIN_TCE_ENTRIES) {
			printk(KERN_WARNING "TCE table is full; freeing ");
			printk(KERN_WARNING "%d entries for the kdump boot\n",
				KDUMP_MIN_TCE_ENTRIES);
			for (index = tbl->it_size - KDUMP_MIN_TCE_ENTRIES;
				index < tbl->it_size; index++)
				__clear_bit(index, tbl->it_map);
		}
	}
#endif
}

/*
                                                                
                                                 
 */
struct iommu_table *iommu_init_table(struct iommu_table *tbl, int nid)
{
	unsigned long sz;
	static int welcomed = 0;
	struct page *page;
	unsigned int i;
	struct iommu_pool *p;

	/*                                       */
	sz = BITS_TO_LONGS(tbl->it_size) * sizeof(unsigned long);

	page = alloc_pages_node(nid, GFP_KERNEL, get_order(sz));
	if (!page)
		panic("iommu_init_table: Can't allocate %ld bytes\n", sz);
	tbl->it_map = page_address(page);
	memset(tbl->it_map, 0, sz);

	/*
                                                           
                                                                
                                           
  */
	if (tbl->it_offset == 0)
		set_bit(0, tbl->it_map);

	/*                                                               */
	if ((tbl->it_size << IOMMU_PAGE_SHIFT) >= (1UL * 1024 * 1024 * 1024))
		tbl->nr_pools = IOMMU_NR_POOLS;
	else
		tbl->nr_pools = 1;

	/*                                                           */
	tbl->poolsize = (tbl->it_size * 3 / 4) / tbl->nr_pools;

	for (i = 0; i < tbl->nr_pools; i++) {
		p = &tbl->pools[i];
		spin_lock_init(&(p->lock));
		p->start = tbl->poolsize * i;
		p->hint = p->start;
		p->end = p->start + tbl->poolsize;
	}

	p = &tbl->large_pool;
	spin_lock_init(&(p->lock));
	p->start = tbl->poolsize * i;
	p->hint = p->start;
	p->end = tbl->it_size;

	iommu_table_clear(tbl);

	if (!welcomed) {
		printk(KERN_INFO "IOMMU table initialized, virtual merging %s\n",
		       novmerge ? "disabled" : "enabled");
		welcomed = 1;
	}

	return tbl;
}

void iommu_free_table(struct iommu_table *tbl, const char *node_name)
{
	unsigned long bitmap_sz;
	unsigned int order;

	if (!tbl || !tbl->it_map) {
		printk(KERN_ERR "%s: expected TCE map for %s\n", __func__,
				node_name);
		return;
	}

	/*
                                                              
                      
  */
	if (tbl->it_offset == 0)
		clear_bit(0, tbl->it_map);

	/*                                       */
	if (!bitmap_empty(tbl->it_map, tbl->it_size))
		pr_warn("%s: Unexpected TCEs for %s\n", __func__, node_name);

	/*                                */
	bitmap_sz = BITS_TO_LONGS(tbl->it_size) * sizeof(unsigned long);

	/*             */
	order = get_order(bitmap_sz);
	free_pages((unsigned long) tbl->it_map, order);

	/*            */
	kfree(tbl);
}

/*                                                                  
                                                                         
                                                                     
                                                                         
 */
dma_addr_t iommu_map_page(struct device *dev, struct iommu_table *tbl,
			  struct page *page, unsigned long offset, size_t size,
			  unsigned long mask, enum dma_data_direction direction,
			  struct dma_attrs *attrs)
{
	dma_addr_t dma_handle = DMA_ERROR_CODE;
	void *vaddr;
	unsigned long uaddr;
	unsigned int npages, align;

	BUG_ON(direction == DMA_NONE);

	vaddr = page_address(page) + offset;
	uaddr = (unsigned long)vaddr;
	npages = iommu_num_pages(uaddr, size, IOMMU_PAGE_SIZE);

	if (tbl) {
		align = 0;
		if (IOMMU_PAGE_SHIFT < PAGE_SHIFT && size >= PAGE_SIZE &&
		    ((unsigned long)vaddr & ~PAGE_MASK) == 0)
			align = PAGE_SHIFT - IOMMU_PAGE_SHIFT;

		dma_handle = iommu_alloc(dev, tbl, vaddr, npages, direction,
					 mask >> IOMMU_PAGE_SHIFT, align,
					 attrs);
		if (dma_handle == DMA_ERROR_CODE) {
			if (printk_ratelimit())  {
				dev_info(dev, "iommu_alloc failed, tbl %p "
					 "vaddr %p npages %d\n", tbl, vaddr,
					 npages);
			}
		} else
			dma_handle |= (uaddr & ~IOMMU_PAGE_MASK);
	}

	return dma_handle;
}

void iommu_unmap_page(struct iommu_table *tbl, dma_addr_t dma_handle,
		      size_t size, enum dma_data_direction direction,
		      struct dma_attrs *attrs)
{
	unsigned int npages;

	BUG_ON(direction == DMA_NONE);

	if (tbl) {
		npages = iommu_num_pages(dma_handle, size, IOMMU_PAGE_SIZE);
		iommu_free(tbl, dma_handle, npages);
	}
}

/*                                                                 
                                                                
                                                  
 */
void *iommu_alloc_coherent(struct device *dev, struct iommu_table *tbl,
			   size_t size,	dma_addr_t *dma_handle,
			   unsigned long mask, gfp_t flag, int node)
{
	void *ret = NULL;
	dma_addr_t mapping;
	unsigned int order;
	unsigned int nio_pages, io_order;
	struct page *page;

	size = PAGE_ALIGN(size);
	order = get_order(size);

 	/*
                                                               
                                                               
                   
  */
	if (order >= IOMAP_MAX_ORDER) {
		dev_info(dev, "iommu_alloc_consistent size too large: 0x%lx\n",
			 size);
		return NULL;
	}

	if (!tbl)
		return NULL;

	/*                                        */
	page = alloc_pages_node(node, flag, order);
	if (!page)
		return NULL;
	ret = page_address(page);
	memset(ret, 0, size);

	/*                                          */
	nio_pages = size >> IOMMU_PAGE_SHIFT;
	io_order = get_iommu_order(size);
	mapping = iommu_alloc(dev, tbl, ret, nio_pages, DMA_BIDIRECTIONAL,
			      mask >> IOMMU_PAGE_SHIFT, io_order, NULL);
	if (mapping == DMA_ERROR_CODE) {
		free_pages((unsigned long)ret, order);
		return NULL;
	}
	*dma_handle = mapping;
	return ret;
}

void iommu_free_coherent(struct iommu_table *tbl, size_t size,
			 void *vaddr, dma_addr_t dma_handle)
{
	if (tbl) {
		unsigned int nio_pages;

		size = PAGE_ALIGN(size);
		nio_pages = size >> IOMMU_PAGE_SHIFT;
		iommu_free(tbl, dma_handle, nio_pages);
		size = PAGE_ALIGN(size);
		free_pages((unsigned long)vaddr, get_order(size));
	}
}
