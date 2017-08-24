/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
                         
  
                                                                     
                                                                    
                                  
  
                                                                     
                                                           
                                         
 */

#include <linux/dma-mapping.h>
#include <linux/dma-debug.h>
#include <linux/export.h>
#include <asm/cacheflush.h>

/*
                                
 */
void *dma_alloc_noncoherent(struct device *dev, size_t size,
			    dma_addr_t *dma_handle, gfp_t gfp)
{
	void *paddr;

	/*                                         */
	paddr = alloc_pages_exact(size, gfp);
	if (!paddr)
		return NULL;

	/*                                         */
	*dma_handle = plat_kernel_addr_to_dma(dev, paddr);

	return paddr;
}
EXPORT_SYMBOL(dma_alloc_noncoherent);

void dma_free_noncoherent(struct device *dev, size_t size, void *vaddr,
			  dma_addr_t dma_handle)
{
	free_pages_exact((void *)plat_dma_addr_to_kernel(dev, dma_handle),
			 size);
}
EXPORT_SYMBOL(dma_free_noncoherent);

void *dma_alloc_coherent(struct device *dev, size_t size,
			 dma_addr_t *dma_handle, gfp_t gfp)
{
	void *paddr, *kvaddr;

	/*                                         */
	paddr = alloc_pages_exact(size, gfp);
	if (!paddr)
		return NULL;

	/*                                                    */
	kvaddr = ioremap_nocache((unsigned long)paddr, size);
	if (kvaddr != NULL)
		memset(kvaddr, 0, size);

	/*                                         */
	*dma_handle = plat_kernel_addr_to_dma(dev, paddr);

	return kvaddr;
}
EXPORT_SYMBOL(dma_alloc_coherent);

void dma_free_coherent(struct device *dev, size_t size, void *kvaddr,
		       dma_addr_t dma_handle)
{
	iounmap((void __force __iomem *)kvaddr);

	free_pages_exact((void *)plat_dma_addr_to_kernel(dev, dma_handle),
			 size);
}
EXPORT_SYMBOL(dma_free_coherent);

/*
                              
 */
void __arc_dma_cache_sync(unsigned long paddr, size_t size,
			  enum dma_data_direction dir)
{
	__inline_dma_cache_sync(paddr, size, dir);
}
EXPORT_SYMBOL(__arc_dma_cache_sync);
