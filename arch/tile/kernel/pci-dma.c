/*
 * Copyright 2010 Tilera Corporation. All Rights Reserved.
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
 */

#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/swiotlb.h>
#include <linux/vmalloc.h>
#include <linux/export.h>
#include <asm/tlbflush.h>
#include <asm/homecache.h>

/*                                */

/*
                                                                   
                                                          
 */
#ifdef __tilepro__
#define PAGE_HOME_DMA PAGE_HOME_UNCACHED
#else
#define PAGE_HOME_DMA PAGE_HOME_HASH
#endif

static void *tile_dma_alloc_coherent(struct device *dev, size_t size,
				     dma_addr_t *dma_handle, gfp_t gfp,
				     struct dma_attrs *attrs)
{
	u64 dma_mask = dev->coherent_dma_mask ?: DMA_BIT_MASK(32);
	int node = dev_to_node(dev);
	int order = get_order(size);
	struct page *pg;
	dma_addr_t addr;

	gfp |= __GFP_ZERO;

	/*
                                                                    
                                                               
                                                                
                                                                   
                                                                    
                                                                   
  */
	if (dma_mask <= DMA_BIT_MASK(32)) {
		gfp |= GFP_DMA;
		node = 0;
	}

	pg = homecache_alloc_pages_node(node, gfp, order, PAGE_HOME_DMA);
	if (pg == NULL)
		return NULL;

	addr = page_to_phys(pg);
	if (addr + size > dma_mask) {
		__homecache_free_pages(pg, order);
		return NULL;
	}

	*dma_handle = addr;

	return page_address(pg);
}

/*
                                                               
 */
static void tile_dma_free_coherent(struct device *dev, size_t size,
				   void *vaddr, dma_addr_t dma_handle,
				   struct dma_attrs *attrs)
{
	homecache_free_pages((unsigned long)vaddr, get_order(size));
}

/*
                                                             
                                                                 
                                                      
  
                                                                   
                                             
  
                                                                   
                                                                  
                                                                     
                                            
 */

/*                                      */
static void __dma_prep_page(struct page *page, unsigned long offset,
			    size_t size, enum dma_data_direction direction)
{
	/*
                                           
                                                                 
                                       
   
                                                               
                                                          
                                                                
                                                                   
  */
	int home = page_home(page);
	switch (home) {
	case PAGE_HOME_HASH:
#ifdef __tilegx__
		return;
#endif
		break;
	case PAGE_HOME_UNCACHED:
#ifdef __tilepro__
		return;
#endif
		break;
	case PAGE_HOME_IMMUTABLE:
		/*                                     */
		BUG_ON(direction == DMA_FROM_DEVICE ||
		       direction == DMA_BIDIRECTIONAL);
		return;
	case PAGE_HOME_INCOHERENT:
		/*                                                  */
		return;
	default:
		BUG_ON(home < 0 || home >= NR_CPUS);
		break;
	}
	homecache_finv_page(page);

#ifdef DEBUG_ALIGNMENT
	/*                                             */
	if (offset & (L2_CACHE_BYTES - 1) || (size & (L2_CACHE_BYTES - 1)))
		pr_warn("Unaligned DMA to non-hfh memory: PA %#llx/%#lx\n",
			PFN_PHYS(page_to_pfn(page)) + offset, size);
#endif
}

/*                                             */
static void __dma_complete_page(struct page *page, unsigned long offset,
				size_t size, enum dma_data_direction direction)
{
#ifdef __tilegx__
	switch (page_home(page)) {
	case PAGE_HOME_HASH:
		/*                                                      */
		break;
	case PAGE_HOME_INCOHERENT:
		/*                                                  */
		break;
	case PAGE_HOME_IMMUTABLE:
		/*                                           */
		break;
	default:
		/*                                                      */
		homecache_finv_map_page(page, PAGE_HOME_HASH);
		break;
	}
#endif
}

static void __dma_prep_pa_range(dma_addr_t dma_addr, size_t size,
				enum dma_data_direction direction)
{
	struct page *page = pfn_to_page(PFN_DOWN(dma_addr));
	unsigned long offset = dma_addr & (PAGE_SIZE - 1);
	size_t bytes = min(size, (size_t)(PAGE_SIZE - offset));

	while (size != 0) {
		__dma_prep_page(page, offset, bytes, direction);
		size -= bytes;
		++page;
		offset = 0;
		bytes = min((size_t)PAGE_SIZE, size);
	}
}

static void __dma_complete_pa_range(dma_addr_t dma_addr, size_t size,
				    enum dma_data_direction direction)
{
	struct page *page = pfn_to_page(PFN_DOWN(dma_addr));
	unsigned long offset = dma_addr & (PAGE_SIZE - 1);
	size_t bytes = min(size, (size_t)(PAGE_SIZE - offset));

	while (size != 0) {
		__dma_complete_page(page, offset, bytes, direction);
		size -= bytes;
		++page;
		offset = 0;
		bytes = min((size_t)PAGE_SIZE, size);
	}
}

static int tile_dma_map_sg(struct device *dev, struct scatterlist *sglist,
			   int nents, enum dma_data_direction direction,
			   struct dma_attrs *attrs)
{
	struct scatterlist *sg;
	int i;

	BUG_ON(!valid_dma_direction(direction));

	WARN_ON(nents == 0 || sglist->length == 0);

	for_each_sg(sglist, sg, nents, i) {
		sg->dma_address = sg_phys(sg);
		__dma_prep_pa_range(sg->dma_address, sg->length, direction);
#ifdef CONFIG_NEED_SG_DMA_LENGTH
		sg->dma_length = sg->length;
#endif
	}

	return nents;
}

static void tile_dma_unmap_sg(struct device *dev, struct scatterlist *sglist,
			      int nents, enum dma_data_direction direction,
			      struct dma_attrs *attrs)
{
	struct scatterlist *sg;
	int i;

	BUG_ON(!valid_dma_direction(direction));
	for_each_sg(sglist, sg, nents, i) {
		sg->dma_address = sg_phys(sg);
		__dma_complete_pa_range(sg->dma_address, sg->length,
					direction);
	}
}

static dma_addr_t tile_dma_map_page(struct device *dev, struct page *page,
				    unsigned long offset, size_t size,
				    enum dma_data_direction direction,
				    struct dma_attrs *attrs)
{
	BUG_ON(!valid_dma_direction(direction));

	BUG_ON(offset + size > PAGE_SIZE);
	__dma_prep_page(page, offset, size, direction);

	return page_to_pa(page) + offset;
}

static void tile_dma_unmap_page(struct device *dev, dma_addr_t dma_address,
				size_t size, enum dma_data_direction direction,
				struct dma_attrs *attrs)
{
	BUG_ON(!valid_dma_direction(direction));

	__dma_complete_page(pfn_to_page(PFN_DOWN(dma_address)),
			    dma_address & PAGE_OFFSET, size, direction);
}

static void tile_dma_sync_single_for_cpu(struct device *dev,
					 dma_addr_t dma_handle,
					 size_t size,
					 enum dma_data_direction direction)
{
	BUG_ON(!valid_dma_direction(direction));

	__dma_complete_pa_range(dma_handle, size, direction);
}

static void tile_dma_sync_single_for_device(struct device *dev,
					    dma_addr_t dma_handle, size_t size,
					    enum dma_data_direction direction)
{
	__dma_prep_pa_range(dma_handle, size, direction);
}

static void tile_dma_sync_sg_for_cpu(struct device *dev,
				     struct scatterlist *sglist, int nelems,
				     enum dma_data_direction direction)
{
	struct scatterlist *sg;
	int i;

	BUG_ON(!valid_dma_direction(direction));
	WARN_ON(nelems == 0 || sglist->length == 0);

	for_each_sg(sglist, sg, nelems, i) {
		dma_sync_single_for_cpu(dev, sg->dma_address,
					sg_dma_len(sg), direction);
	}
}

static void tile_dma_sync_sg_for_device(struct device *dev,
					struct scatterlist *sglist, int nelems,
					enum dma_data_direction direction)
{
	struct scatterlist *sg;
	int i;

	BUG_ON(!valid_dma_direction(direction));
	WARN_ON(nelems == 0 || sglist->length == 0);

	for_each_sg(sglist, sg, nelems, i) {
		dma_sync_single_for_device(dev, sg->dma_address,
					   sg_dma_len(sg), direction);
	}
}

static inline int
tile_dma_mapping_error(struct device *dev, dma_addr_t dma_addr)
{
	return 0;
}

static inline int
tile_dma_supported(struct device *dev, u64 mask)
{
	return 1;
}

static struct dma_map_ops tile_default_dma_map_ops = {
	.alloc = tile_dma_alloc_coherent,
	.free = tile_dma_free_coherent,
	.map_page = tile_dma_map_page,
	.unmap_page = tile_dma_unmap_page,
	.map_sg = tile_dma_map_sg,
	.unmap_sg = tile_dma_unmap_sg,
	.sync_single_for_cpu = tile_dma_sync_single_for_cpu,
	.sync_single_for_device = tile_dma_sync_single_for_device,
	.sync_sg_for_cpu = tile_dma_sync_sg_for_cpu,
	.sync_sg_for_device = tile_dma_sync_sg_for_device,
	.mapping_error = tile_dma_mapping_error,
	.dma_supported = tile_dma_supported
};

struct dma_map_ops *tile_dma_map_ops = &tile_default_dma_map_ops;
EXPORT_SYMBOL(tile_dma_map_ops);

/*                                   */

static void *tile_pci_dma_alloc_coherent(struct device *dev, size_t size,
					 dma_addr_t *dma_handle, gfp_t gfp,
					 struct dma_attrs *attrs)
{
	int node = dev_to_node(dev);
	int order = get_order(size);
	struct page *pg;
	dma_addr_t addr;

	gfp |= __GFP_ZERO;

	pg = homecache_alloc_pages_node(node, gfp, order, PAGE_HOME_DMA);
	if (pg == NULL)
		return NULL;

	addr = page_to_phys(pg);

	*dma_handle = phys_to_dma(dev, addr);

	return page_address(pg);
}

/*
                                                                   
 */
static void tile_pci_dma_free_coherent(struct device *dev, size_t size,
				       void *vaddr, dma_addr_t dma_handle,
				       struct dma_attrs *attrs)
{
	homecache_free_pages((unsigned long)vaddr, get_order(size));
}

static int tile_pci_dma_map_sg(struct device *dev, struct scatterlist *sglist,
			       int nents, enum dma_data_direction direction,
			       struct dma_attrs *attrs)
{
	struct scatterlist *sg;
	int i;

	BUG_ON(!valid_dma_direction(direction));

	WARN_ON(nents == 0 || sglist->length == 0);

	for_each_sg(sglist, sg, nents, i) {
		sg->dma_address = sg_phys(sg);
		__dma_prep_pa_range(sg->dma_address, sg->length, direction);

		sg->dma_address = phys_to_dma(dev, sg->dma_address);
#ifdef CONFIG_NEED_SG_DMA_LENGTH
		sg->dma_length = sg->length;
#endif
	}

	return nents;
}

static void tile_pci_dma_unmap_sg(struct device *dev,
				  struct scatterlist *sglist, int nents,
				  enum dma_data_direction direction,
				  struct dma_attrs *attrs)
{
	struct scatterlist *sg;
	int i;

	BUG_ON(!valid_dma_direction(direction));
	for_each_sg(sglist, sg, nents, i) {
		sg->dma_address = sg_phys(sg);
		__dma_complete_pa_range(sg->dma_address, sg->length,
					direction);
	}
}

static dma_addr_t tile_pci_dma_map_page(struct device *dev, struct page *page,
					unsigned long offset, size_t size,
					enum dma_data_direction direction,
					struct dma_attrs *attrs)
{
	BUG_ON(!valid_dma_direction(direction));

	BUG_ON(offset + size > PAGE_SIZE);
	__dma_prep_page(page, offset, size, direction);

	return phys_to_dma(dev, page_to_pa(page) + offset);
}

static void tile_pci_dma_unmap_page(struct device *dev, dma_addr_t dma_address,
				    size_t size,
				    enum dma_data_direction direction,
				    struct dma_attrs *attrs)
{
	BUG_ON(!valid_dma_direction(direction));

	dma_address = dma_to_phys(dev, dma_address);

	__dma_complete_page(pfn_to_page(PFN_DOWN(dma_address)),
			    dma_address & PAGE_OFFSET, size, direction);
}

static void tile_pci_dma_sync_single_for_cpu(struct device *dev,
					     dma_addr_t dma_handle,
					     size_t size,
					     enum dma_data_direction direction)
{
	BUG_ON(!valid_dma_direction(direction));

	dma_handle = dma_to_phys(dev, dma_handle);

	__dma_complete_pa_range(dma_handle, size, direction);
}

static void tile_pci_dma_sync_single_for_device(struct device *dev,
						dma_addr_t dma_handle,
						size_t size,
						enum dma_data_direction
						direction)
{
	dma_handle = dma_to_phys(dev, dma_handle);

	__dma_prep_pa_range(dma_handle, size, direction);
}

static void tile_pci_dma_sync_sg_for_cpu(struct device *dev,
					 struct scatterlist *sglist,
					 int nelems,
					 enum dma_data_direction direction)
{
	struct scatterlist *sg;
	int i;

	BUG_ON(!valid_dma_direction(direction));
	WARN_ON(nelems == 0 || sglist->length == 0);

	for_each_sg(sglist, sg, nelems, i) {
		dma_sync_single_for_cpu(dev, sg->dma_address,
					sg_dma_len(sg), direction);
	}
}

static void tile_pci_dma_sync_sg_for_device(struct device *dev,
					    struct scatterlist *sglist,
					    int nelems,
					    enum dma_data_direction direction)
{
	struct scatterlist *sg;
	int i;

	BUG_ON(!valid_dma_direction(direction));
	WARN_ON(nelems == 0 || sglist->length == 0);

	for_each_sg(sglist, sg, nelems, i) {
		dma_sync_single_for_device(dev, sg->dma_address,
					   sg_dma_len(sg), direction);
	}
}

static inline int
tile_pci_dma_mapping_error(struct device *dev, dma_addr_t dma_addr)
{
	return 0;
}

static inline int
tile_pci_dma_supported(struct device *dev, u64 mask)
{
	return 1;
}

static struct dma_map_ops tile_pci_default_dma_map_ops = {
	.alloc = tile_pci_dma_alloc_coherent,
	.free = tile_pci_dma_free_coherent,
	.map_page = tile_pci_dma_map_page,
	.unmap_page = tile_pci_dma_unmap_page,
	.map_sg = tile_pci_dma_map_sg,
	.unmap_sg = tile_pci_dma_unmap_sg,
	.sync_single_for_cpu = tile_pci_dma_sync_single_for_cpu,
	.sync_single_for_device = tile_pci_dma_sync_single_for_device,
	.sync_sg_for_cpu = tile_pci_dma_sync_sg_for_cpu,
	.sync_sg_for_device = tile_pci_dma_sync_sg_for_device,
	.mapping_error = tile_pci_dma_mapping_error,
	.dma_supported = tile_pci_dma_supported
};

struct dma_map_ops *gx_pci_dma_map_ops = &tile_pci_default_dma_map_ops;
EXPORT_SYMBOL(gx_pci_dma_map_ops);

/*                                                  */

#ifdef CONFIG_SWIOTLB
static void *tile_swiotlb_alloc_coherent(struct device *dev, size_t size,
					 dma_addr_t *dma_handle, gfp_t gfp,
					 struct dma_attrs *attrs)
{
	gfp |= GFP_DMA;
	return swiotlb_alloc_coherent(dev, size, dma_handle, gfp);
}

static void tile_swiotlb_free_coherent(struct device *dev, size_t size,
				       void *vaddr, dma_addr_t dma_addr,
				       struct dma_attrs *attrs)
{
	swiotlb_free_coherent(dev, size, vaddr, dma_addr);
}

static struct dma_map_ops pci_swiotlb_dma_ops = {
	.alloc = tile_swiotlb_alloc_coherent,
	.free = tile_swiotlb_free_coherent,
	.map_page = swiotlb_map_page,
	.unmap_page = swiotlb_unmap_page,
	.map_sg = swiotlb_map_sg_attrs,
	.unmap_sg = swiotlb_unmap_sg_attrs,
	.sync_single_for_cpu = swiotlb_sync_single_for_cpu,
	.sync_single_for_device = swiotlb_sync_single_for_device,
	.sync_sg_for_cpu = swiotlb_sync_sg_for_cpu,
	.sync_sg_for_device = swiotlb_sync_sg_for_device,
	.dma_supported = swiotlb_dma_supported,
	.mapping_error = swiotlb_dma_mapping_error,
};

struct dma_map_ops *gx_legacy_pci_dma_map_ops = &pci_swiotlb_dma_ops;
#else
struct dma_map_ops *gx_legacy_pci_dma_map_ops;
#endif
EXPORT_SYMBOL(gx_legacy_pci_dma_map_ops);

#ifdef CONFIG_ARCH_HAS_DMA_SET_COHERENT_MASK
int dma_set_coherent_mask(struct device *dev, u64 mask)
{
	struct dma_map_ops *dma_ops = get_dma_ops(dev);

	/*                                                               */
	if (((dma_ops == gx_pci_dma_map_ops) ||
	    (dma_ops == gx_legacy_pci_dma_map_ops)) &&
	    (mask <= DMA_BIT_MASK(32))) {
		if (mask > dev->archdata.max_direct_dma_addr)
			mask = dev->archdata.max_direct_dma_addr;
	}

	if (!dma_supported(dev, mask))
		return -EIO;
	dev->coherent_dma_mask = mask;
	return 0;
}
EXPORT_SYMBOL(dma_set_coherent_mask);
#endif
