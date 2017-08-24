/* exynos_drm.h
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 * Authors:
 *	Inki Dae <inki.dae@samsung.com>
 *	Joonyoung Shim <jy0922.shim@samsung.com>
 *	Seung-Woo Kim <sw0312.kim@samsung.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
#ifndef _EXYNOS_DRM_H_
#define _EXYNOS_DRM_H_

#include <uapi/drm/exynos_drm.h>

/* 
                                         
  
                                               
                                         
                                           
 */
struct exynos_drm_panel_info {
	struct fb_videomode timing;
	u32 width_mm;
	u32 height_mm;
};

/* 
                                                  
  
                                              
                                                               
                               
 */
struct exynos_drm_fimd_pdata {
	struct exynos_drm_panel_info panel;
	u32				vidcon0;
	u32				vidcon1;
	unsigned int			default_win;
	unsigned int			bpp;
};

/* 
                                                  
  
                                                   
                                                     
  
                                                                       
                                                                       
 */
struct exynos_drm_common_hdmi_pd {
	struct device *hdmi_dev;
	struct device *mixer_dev;
};

/* 
                                                       
  
                                      
                                                                     
                                                                        
 */
struct exynos_drm_hdmi_pdata {
	bool is_v13;
	void (*cfg_hpd)(bool external);
	int (*get_hpd)(void);
};

/* 
                                                 
  
                                          
                                                   
                                          
                                                   
 */
struct exynos_drm_ipp_pol {
	unsigned int inv_pclk;
	unsigned int inv_vsync;
	unsigned int inv_href;
	unsigned int inv_hsync;
};

/* 
                                                  
  
                                                  
                                          
 */
struct exynos_drm_fimc_pdata {
	struct exynos_drm_ipp_pol pol;
	int clk_rate;
};

#endif	/*                */
