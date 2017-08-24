/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef MSM_IOMMU_H
#define MSM_IOMMU_H

#include <linux/interrupt.h>
#include <linux/clk.h>

/*                                              */
#define MSM_IOMMU_ATTR_NON_SH		0x0
#define MSM_IOMMU_ATTR_SH		0x4

/*                                               */
#define MSM_IOMMU_ATTR_NONCACHED	0x0
#define MSM_IOMMU_ATTR_CACHED_WB_WA	0x1
#define MSM_IOMMU_ATTR_CACHED_WB_NWA	0x2
#define MSM_IOMMU_ATTR_CACHED_WT	0x3

/*                                     */
#define MSM_IOMMU_CP_MASK		0x03

/*                                                                            
                                                                         
                                                                            
                                                                             
                                      
 */
#define MAX_NUM_MIDS	32

/* 
                                                          
                                                            
                                                                 
 */
struct msm_iommu_dev {
	const char *name;
	int ncb;
};

/* 
                                                            
                                                       
                                                      
                                                                    
                                                              
                                                                   
                                   
 */
struct msm_iommu_ctx_dev {
	const char *name;
	int num;
	int mids[MAX_NUM_MIDS];
};


/* 
                                                              
                                             
                                             
                         
                                                       
                                                  
  
                                                                        
                                 
 */
struct msm_iommu_drvdata {
	void __iomem *base;
	int irq;
	int ncb;
	struct clk *clk;
	struct clk *pclk;
};

/* 
                                                                
                                                 
                                                          
                                                                     
                     
  
                                                                          
                                      
 */
struct msm_iommu_ctx_drvdata {
	int num;
	struct platform_device *pdev;
	struct list_head attached_elm;
};

/*
                                                                           
                                                                           
                          
 */
struct device *msm_iommu_get_ctx(const char *ctx_name);

/*
                                                                       
                                                                          
                                           
 */
irqreturn_t msm_iommu_fault_handler(int irq, void *dev_id);

#endif
