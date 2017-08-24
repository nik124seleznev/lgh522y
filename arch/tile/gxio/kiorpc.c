/*
 * Copyright 2012 Tilera Corporation. All Rights Reserved.
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
 *
 * TILE-Gx IORPC support for kernel I/O drivers.
 */

#include <linux/mmzone.h>
#include <linux/module.h>
#include <linux/io.h>
#include <gxio/iorpc_globals.h>
#include <gxio/kiorpc.h>

#ifdef DEBUG_IORPC
#define TRACE(FMT, ...) pr_info(SIMPLE_MSG_LINE FMT, ## __VA_ARGS__)
#else
#define TRACE(...)
#endif

/*                                                               */
void __iomem *iorpc_ioremap(int hv_fd, resource_size_t offset,
			    unsigned long size)
{
	pgprot_t mmio_base, prot = { 0 };
	unsigned long pfn;
	int err;

	/*                                       */
	err = __iorpc_get_mmio_base(hv_fd, &mmio_base);
	if (err) {
		TRACE("get_mmio_base() failure: %d\n", err);
		return NULL;
	}

	/*                                                          */
	err = __iorpc_check_mmio_offset(hv_fd, offset, size);
	if (err) {
		TRACE("check_mmio_offset() failure: %d\n", err);
		return NULL;
	}

	/*
                                                               
                                          
  */
	prot = hv_pte_set_lotar(prot, hv_pte_get_lotar(mmio_base));
	pfn = pte_pfn(mmio_base) + PFN_DOWN(offset);

	return ioremap_prot(PFN_PHYS(pfn), size, prot);
}

EXPORT_SYMBOL(iorpc_ioremap);
