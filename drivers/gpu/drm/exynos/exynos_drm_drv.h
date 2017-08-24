/* exynos_drm_drv.h
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

#ifndef _EXYNOS_DRM_DRV_H_
#define _EXYNOS_DRM_DRV_H_

#include <linux/module.h>

#define MAX_CRTC	3
#define MAX_PLANE	5
#define MAX_FB_BUFFER	4
#define DEFAULT_ZPOS	-1

#define _wait_for(COND, MS) ({ \
	unsigned long timeout__ = jiffies + msecs_to_jiffies(MS);	\
	int ret__ = 0;							\
	while (!(COND)) {						\
		if (time_after(jiffies, timeout__)) {			\
			ret__ = -ETIMEDOUT;				\
			break;						\
		}							\
	}								\
	ret__;								\
})

#define wait_for(COND, MS) _wait_for(COND, MS)

struct drm_device;
struct exynos_drm_overlay;
struct drm_connector;

extern unsigned int drm_vblank_offdelay;

/*                               */
enum exynos_drm_output_type {
	EXYNOS_DISPLAY_TYPE_NONE,
	/*                       */
	EXYNOS_DISPLAY_TYPE_LCD,
	/*                 */
	EXYNOS_DISPLAY_TYPE_HDMI,
	/*                            */
	EXYNOS_DISPLAY_TYPE_VIDI,
};

/*
                                    
  
                                                                
                                                              
                                             
                                               
 */
struct exynos_drm_overlay_ops {
	void (*mode_set)(struct device *subdrv_dev,
			 struct exynos_drm_overlay *overlay);
	void (*commit)(struct device *subdrv_dev, int zpos);
	void (*enable)(struct device *subdrv_dev, int zpos);
	void (*disable)(struct device *subdrv_dev, int zpos);
};

/*
                                       
  
                                                    
                                    
                                                    
                                    
                                     
                                       
                                                                         
                                                                           
                                        
                                        
                                                               
                                                                 
                                     
                                       
                          
                                            
                                
                            
                                                     
                                                                        
                                 
                                             
                                        
                                   
                                                                         
                    
                                                               
                                         
                                
  
                                                                          
                                     
 */
struct exynos_drm_overlay {
	unsigned int fb_x;
	unsigned int fb_y;
	unsigned int fb_width;
	unsigned int fb_height;
	unsigned int src_width;
	unsigned int src_height;
	unsigned int crtc_x;
	unsigned int crtc_y;
	unsigned int crtc_width;
	unsigned int crtc_height;
	unsigned int mode_width;
	unsigned int mode_height;
	unsigned int refresh;
	unsigned int scan_flag;
	unsigned int bpp;
	unsigned int pitch;
	uint32_t pixel_format;
	dma_addr_t dma_addr[MAX_FB_BUFFER];
	int zpos;

	bool default_win;
	bool color_key;
	unsigned int index_color;
	bool local_path;
	bool transparency;
	bool activated;
};

/*
                                
                                                                     
  
                                                  
                                                             
                                                 
                                                    
                                                  
                                       
 */
struct exynos_drm_display_ops {
	enum exynos_drm_output_type type;
	bool (*is_connected)(struct device *dev);
	struct edid *(*get_edid)(struct device *dev,
			struct drm_connector *connector);
	void *(*get_panel)(struct device *dev);
	int (*check_timing)(struct device *dev, void *timing);
	int (*power_on)(struct device *dev, int mode);
};

/*
                         
  
                               
                                                            
                                                                    
                                                                      
                                                
                                                               
                                                       
                                                                          
                                                                            
                                                                
                               
 */
struct exynos_drm_manager_ops {
	void (*dpms)(struct device *subdrv_dev, int mode);
	void (*apply)(struct device *subdrv_dev);
	void (*mode_fixup)(struct device *subdrv_dev,
				struct drm_connector *connector,
				const struct drm_display_mode *mode,
				struct drm_display_mode *adjusted_mode);
	void (*mode_set)(struct device *subdrv_dev, void *mode);
	void (*get_max_resol)(struct device *subdrv_dev, unsigned int *width,
				unsigned int *height);
	void (*commit)(struct device *subdrv_dev);
	int (*enable_vblank)(struct device *subdrv_dev);
	void (*disable_vblank)(struct device *subdrv_dev);
	void (*wait_for_vblank)(struct device *subdrv_dev);
};

/*
                                       
  
                                                           
                                                         
                                
                                                                  
                                                              
                                                                    
                                                                          
                                                              
                                                                     
                                                                      
                                                              
                                                                 
                                                                         
 */
struct exynos_drm_manager {
	struct device *dev;
	int pipe;
	struct exynos_drm_manager_ops *ops;
	struct exynos_drm_overlay_ops *overlay_ops;
	struct exynos_drm_display_ops *display_ops;
};

struct exynos_drm_g2d_private {
	struct device		*dev;
	struct list_head	inuse_cmdlist;
	struct list_head	event_list;
	struct list_head	userptr_list;
};

struct exynos_drm_ipp_private {
	struct device	*dev;
	struct list_head	event_list;
};

struct drm_exynos_file_private {
	struct exynos_drm_g2d_private	*g2d_priv;
	struct exynos_drm_ipp_private	*ipp_priv;
};

/*
                                
  
                                                    
                                                            
                         
                                                
                                          
                                                  
 */
struct exynos_drm_private {
	struct drm_fb_helper *fb_helper;

	/*                                      */
	struct list_head pageflip_event_list;

	/*
                                                            
                                                                       
  */
	struct drm_crtc *crtc[MAX_CRTC];
	struct drm_property *plane_zpos_property;
	struct drm_property *crtc_mode_property;

	unsigned long da_start;
	unsigned long da_space_size;
	unsigned long da_space_order;
};

/*
                                   
  
                                                                              
                                                           
                                                                
                                                      
                                                                           
                                                        
                                                                   
                              
                                                              
                     
                                                         
                                                           
                                                     
                                                         
 */
struct exynos_drm_subdrv {
	struct list_head list;
	struct device *dev;
	struct drm_device *drm_dev;
	struct exynos_drm_manager *manager;

	int (*probe)(struct drm_device *drm_dev, struct device *dev);
	void (*remove)(struct drm_device *drm_dev, struct device *dev);
	int (*open)(struct drm_device *drm_dev, struct device *dev,
			struct drm_file *file);
	void (*close)(struct drm_device *drm_dev, struct device *dev,
			struct drm_file *file);

	struct drm_encoder *encoder;
	struct drm_connector *connector;
};

/*
                                                                         
                                                                      
                 
 */
int exynos_drm_device_register(struct drm_device *dev);
/*
                                                                          
                                        
 */
int exynos_drm_device_unregister(struct drm_device *dev);

/*
                                                                          
                                                                         
                                                                            
                                                                         
 */
int exynos_drm_subdrv_register(struct exynos_drm_subdrv *drm_subdrv);

/*                                                          */
int exynos_drm_subdrv_unregister(struct exynos_drm_subdrv *drm_subdrv);

int exynos_drm_subdrv_open(struct drm_device *dev, struct drm_file *file);
void exynos_drm_subdrv_close(struct drm_device *dev, struct drm_file *file);

/*
                                                                               
                                     
 */
int exynos_platform_device_hdmi_register(void);

/*
                                                                          
 */
void exynos_platform_device_hdmi_unregister(void);

/*
                                                          
 */
int exynos_platform_device_ipp_register(void);

/*
                                                                         
 */
void exynos_platform_device_ipp_unregister(void);

extern struct platform_driver fimd_driver;
extern struct platform_driver hdmi_driver;
extern struct platform_driver mixer_driver;
extern struct platform_driver exynos_drm_common_hdmi_driver;
extern struct platform_driver vidi_driver;
extern struct platform_driver g2d_driver;
extern struct platform_driver fimc_driver;
extern struct platform_driver rotator_driver;
extern struct platform_driver gsc_driver;
extern struct platform_driver ipp_driver;
#endif
