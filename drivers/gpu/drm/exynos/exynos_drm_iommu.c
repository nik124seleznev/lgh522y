/* exynos_drm_iommu.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 * Author: Inki Dae <inki.dae@samsung.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <drmP.h>
#include <drm/exynos_drm.h>

#include <linux/dma-mapping.h>
#include <linux/iommu.h>
#include <linux/kref.h>

#include <asm/dma-iommu.h>

#include "exynos_drm_drv.h"
#include "exynos_drm_iommu.h"

/*
                                                        
  
                       
 */
int drm_create_iommu_mapping(struct drm_device *drm_dev)
{
	struct dma_iommu_mapping *mapping = NULL;
	struct exynos_drm_private *priv = drm_dev->dev_private;
	struct device *dev = drm_dev->dev;

	if (!priv->da_start)
		priv->da_start = EXYNOS_DEV_ADDR_START;
	if (!priv->da_space_size)
		priv->da_space_size = EXYNOS_DEV_ADDR_SIZE;
	if (!priv->da_space_order)
		priv->da_space_order = EXYNOS_DEV_ADDR_ORDER;

	mapping = arm_iommu_create_mapping(&platform_bus_type, priv->da_start,
						priv->da_space_size,
						priv->da_space_order);
	if (IS_ERR(mapping))
		return PTR_ERR(mapping);

	dev->dma_parms = devm_kzalloc(dev, sizeof(*dev->dma_parms),
					GFP_KERNEL);
	dma_set_max_seg_size(dev, 0xffffffffu);
	dev->archdata.mapping = mapping;

	return 0;
}

/*
                                                              
  
                       
  
                                                                      
                   
 */
void drm_release_iommu_mapping(struct drm_device *drm_dev)
{
	struct device *dev = drm_dev->dev;

	arm_iommu_release_mapping(dev->archdata.mapping);
}

/*
                                                          
  
                       
                                   
  
                                                                      
           
 */
int drm_iommu_attach_device(struct drm_device *drm_dev,
				struct device *subdrv_dev)
{
	struct device *dev = drm_dev->dev;
	int ret;

	if (!dev->archdata.mapping) {
		DRM_ERROR("iommu_mapping is null.\n");
		return -EFAULT;
	}

	subdrv_dev->dma_parms = devm_kzalloc(subdrv_dev,
					sizeof(*subdrv_dev->dma_parms),
					GFP_KERNEL);
	dma_set_max_seg_size(subdrv_dev, 0xffffffffu);

	ret = arm_iommu_attach_device(subdrv_dev, dev->archdata.mapping);
	if (ret < 0) {
		DRM_DEBUG_KMS("failed iommu attach.\n");
		return ret;
	}

	/*
                                            
   
                                                               
                                                           
                                                                
                                                         
  */
	if (!dev->archdata.dma_ops)
		dev->archdata.dma_ops = subdrv_dev->archdata.dma_ops;

	return 0;
}

/*
                                                                           
  
                       
                                     
  
                                                                        
          
 */
void drm_iommu_detach_device(struct drm_device *drm_dev,
				struct device *subdrv_dev)
{
	struct device *dev = drm_dev->dev;
	struct dma_iommu_mapping *mapping = dev->archdata.mapping;

	if (!mapping || !mapping->domain)
		return;

	iommu_detach_device(mapping->domain, subdrv_dev);
	drm_release_iommu_mapping(drm_dev);
}