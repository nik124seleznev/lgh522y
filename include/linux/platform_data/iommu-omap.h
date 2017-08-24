/*
 * omap iommu: main structures
 *
 * Copyright (C) 2008-2009 Nokia Corporation
 *
 * Written by Hiroshi DOYU <Hiroshi.DOYU@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/platform_device.h>

#define MMU_REG_SIZE		256

/* 
                                                   
                                  
                                         
  
                                                                       
                                                                        
                                                                  
                                          
 */
struct omap_iommu_arch_data {
	const char *name;
	struct omap_iommu *iommu_dev;
};

/* 
                                                                       
                                                        
                                                    
                                                                  
                             
 */
struct omap_mmu_dev_attr {
	u32 da_start;
	u32 da_end;
	int nr_tlb_entries;
};

struct iommu_platform_data {
	const char *name;
	const char *reset_name;
	int nr_tlb_entries;
	u32 da_start;
	u32 da_end;

	int (*assert_reset)(struct platform_device *pdev, const char *name);
	int (*deassert_reset)(struct platform_device *pdev, const char *name);
};
