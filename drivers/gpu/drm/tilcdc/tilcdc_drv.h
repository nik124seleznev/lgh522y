/*
 * Copyright (C) 2012 Texas Instruments
 * Author: Rob Clark <robdclark@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TILCDC_DRV_H__
#define __TILCDC_DRV_H__

#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/list.h>

#include <drm/drmP.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_fb_cma_helper.h>

struct tilcdc_drm_private {
	void __iomem *mmio;

	struct clk *disp_clk;    /*              */
	struct clk *clk;         /*                  */
	int rev;                 /*             */

	/*                                                 */
	uint32_t max_bandwidth;

	/*                                                */
	u32 saved_register[12];

#ifdef CONFIG_CPU_FREQ
	struct notifier_block freq_transition;
	unsigned int lcd_fck_rate;
#endif

	struct workqueue_struct *wq;

	struct drm_fbdev_cma *fbdev;

	struct drm_crtc *crtc;

	unsigned int num_encoders;
	struct drm_encoder *encoders[8];

	unsigned int num_connectors;
	struct drm_connector *connectors[8];
};

/*                                                                         
                                                                       
                                                                             
                                                                      
                                                     
 */
struct tilcdc_module;

struct tilcdc_module_ops {
	/*                                         */
	int (*modeset_init)(struct tilcdc_module *mod, struct drm_device *dev);
	void (*destroy)(struct tilcdc_module *mod);
#ifdef CONFIG_DEBUG_FS
	/*                                     */
	int (*debugfs_init)(struct tilcdc_module *mod, struct drm_minor *minor);
	/*                                      */
	void (*debugfs_cleanup)(struct tilcdc_module *mod, struct drm_minor *minor);
#endif
};

struct tilcdc_module {
	const char *name;
	struct list_head list;
	const struct tilcdc_module_ops *funcs;
};

void tilcdc_module_init(struct tilcdc_module *mod, const char *name,
		const struct tilcdc_module_ops *funcs);
void tilcdc_module_cleanup(struct tilcdc_module *mod);


/*                                                                      
                                                            
                                                           
 */
struct tilcdc_panel_info {

	/*                       */
	uint32_t ac_bias;

	/*                                       */
	uint32_t ac_bias_intrpt;

	/*                */
	uint32_t dma_burst_sz;

	/*                */
	uint32_t bpp;

	/*                        */
	uint32_t fdd;

	/*                                                  */
	bool tft_alt_mode;

	/*                    */
	bool invert_pxl_clk;

	/*                                                       */
	uint32_t sync_edge;

	/*                                                 */
	uint32_t sync_ctrl;

	/*                                                           */
	uint32_t raster_order;

	/*                    */
	uint32_t fifo_th;
};

#define DBG(fmt, ...) DRM_DEBUG(fmt"\n", ##__VA_ARGS__)

struct drm_crtc *tilcdc_crtc_create(struct drm_device *dev);
void tilcdc_crtc_cancel_page_flip(struct drm_crtc *crtc, struct drm_file *file);
irqreturn_t tilcdc_crtc_irq(struct drm_crtc *crtc);
void tilcdc_crtc_update_clk(struct drm_crtc *crtc);
void tilcdc_crtc_set_panel_info(struct drm_crtc *crtc,
		const struct tilcdc_panel_info *info);
int tilcdc_crtc_mode_valid(struct drm_crtc *crtc, struct drm_display_mode *mode);
int tilcdc_crtc_max_width(struct drm_crtc *crtc);

#endif /*                  */
