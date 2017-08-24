/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */



/* 
                   
                                                     
 */
#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <linux/version.h>
#include <linux/shmem_fs.h>
#include <linux/dma-buf.h>
#include <linux/module.h>

#include <drm/drmP.h>
#include <drm/drm_crtc_helper.h>

#include "pl111_drm.h"

static int pl111_crtc_num;

void pl111_common_irq(struct pl111_drm_crtc *pl111_crtc)
{
	struct drm_device *dev = pl111_crtc->crtc.dev;
	struct pl111_drm_flip_resource *old_flip_res;
	struct pl111_gem_bo *bo;
	unsigned long irq_flags;
	int flips_in_flight;
#ifdef CONFIG_DMA_SHARED_BUFFER_USES_KDS
	unsigned long flags;
#endif

	spin_lock_irqsave(&pl111_crtc->base_update_lock, irq_flags);

	/*
                                                                
                                                                 
                           
    
                                                               
  */
	old_flip_res = pl111_crtc->current_update_res;
	if (!old_flip_res) {
		spin_unlock_irqrestore(&pl111_crtc->base_update_lock, irq_flags);
		return;
	}
	pl111_crtc->current_update_res = NULL;

	/*                                                                  */
	if (!list_empty(&pl111_crtc->update_queue)) {
		struct pl111_drm_flip_resource *flip_res;
		/*                             */
		flip_res = list_first_entry(&pl111_crtc->update_queue,
			struct pl111_drm_flip_resource, link);
		list_del(&flip_res->link);
		do_flip_to_res(flip_res);
		/*
                                                       
                                                      
                        
   */
	}
	spin_unlock_irqrestore(&pl111_crtc->base_update_lock, irq_flags);

	/*                                                */
	DRM_DEBUG_KMS("DRM Finalizing old_flip_res=%p\n", old_flip_res);

	bo = PL111_BO_FROM_FRAMEBUFFER(old_flip_res->fb);
#ifdef CONFIG_DMA_SHARED_BUFFER_USES_KDS
	spin_lock_irqsave(&pl111_crtc->current_displaying_lock, flags);
	release_kds_resource_and_display(old_flip_res);
	spin_unlock_irqrestore(&pl111_crtc->current_displaying_lock, flags);
#endif
	/*                                 */
	if (bo->gem_object.export_dma_buf != NULL)
		dma_buf_put(bo->gem_object.export_dma_buf);

	drm_handle_vblank(dev, pl111_crtc->crtc_index);

	/*                                                   */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
	if (old_flip_res->event) {
		spin_lock_bh(&dev->event_lock);
		drm_send_vblank_event(dev, pl111_crtc->crtc_index,
					old_flip_res->event);
		spin_unlock_bh(&dev->event_lock);
	}
#else
	if (old_flip_res->event) {
		struct drm_pending_vblank_event *e = old_flip_res->event;
		struct timeval now;
		unsigned int seq;

		DRM_DEBUG_KMS("%s: wake up page flip event (%p)\n", __func__,
				old_flip_res->event);

		spin_lock_bh(&dev->event_lock);
		seq = drm_vblank_count_and_time(dev, pl111_crtc->crtc_index,
							&now);
		e->pipe = pl111_crtc->crtc_index;
		e->event.sequence = seq;
		e->event.tv_sec = now.tv_sec;
		e->event.tv_usec = now.tv_usec;

		list_add_tail(&e->base.link,
			&e->base.file_priv->event_list);

		wake_up_interruptible(&e->base.file_priv->event_wait);
		spin_unlock_bh(&dev->event_lock);
	}
#endif

	drm_vblank_put(dev, pl111_crtc->crtc_index);

	/*
                                   
                                                          
                                                
  */
	kmem_cache_free(priv.page_flip_slab, old_flip_res);

	flips_in_flight = atomic_dec_return(&priv.nr_flips_in_flight);
	if (flips_in_flight == 0 ||
			flips_in_flight == (NR_FLIPS_IN_FLIGHT_THRESHOLD - 1))
		wake_up(&priv.wait_for_flips);

	DRM_DEBUG_KMS("DRM release flip_res=%p\n", old_flip_res);
}

void show_framebuffer_on_crtc_cb(void *cb1, void *cb2)
{
	struct pl111_drm_flip_resource *flip_res = cb1;
	struct pl111_drm_crtc *pl111_crtc = to_pl111_crtc(flip_res->crtc);

	pl111_crtc->show_framebuffer_cb(cb1, cb2);
}

int show_framebuffer_on_crtc(struct drm_crtc *crtc,
				struct drm_framebuffer *fb, bool page_flip,
				struct drm_pending_vblank_event *event)
{
	struct pl111_gem_bo *bo;
	struct pl111_drm_flip_resource *flip_res;
	int flips_in_flight;
	int old_flips_in_flight;

	crtc->fb = fb;

	bo = PL111_BO_FROM_FRAMEBUFFER(fb);
	if (bo == NULL) {
		DRM_DEBUG_KMS("Failed to get pl111_gem_bo object\n");
		return -EINVAL;
	}

	/*                                                                      
                                                                      
                                                                       
               
   
                                                                     
                                                                      
  */
	if (!page_flip) {
		int ret = wait_event_killable(priv.wait_for_flips,
				atomic_read(&priv.nr_flips_in_flight) == 0);
		if (ret)
			return ret;
	}

	/*
                                                                    
                                                                        
                                                        
   
                                                                       
                                                                      
                                 
  */
	flip_res = kmem_cache_alloc(priv.page_flip_slab, GFP_KERNEL);
	if (flip_res == NULL) {
		pr_err("kmem_cache_alloc failed to alloc - flip ignored\n");
		return -ENOMEM;
	}

	/*
                                                            
                                
  */
	do {
		/*
                                                                 
                    
   */
		int ret = wait_event_killable(priv.wait_for_flips,
				(flips_in_flight =
					atomic_read(&priv.nr_flips_in_flight))
				< NR_FLIPS_IN_FLIGHT_THRESHOLD);
		if (ret != 0) {
			kmem_cache_free(priv.page_flip_slab, flip_res);
			return ret;
		}

		old_flips_in_flight = atomic_cmpxchg(&priv.nr_flips_in_flight,
					flips_in_flight, flips_in_flight + 1);
	} while (old_flips_in_flight != flips_in_flight);

	flip_res->fb = fb;
	flip_res->crtc = crtc;
	flip_res->page_flip = page_flip;
	flip_res->event = event;
	INIT_LIST_HEAD(&flip_res->link);
	DRM_DEBUG_KMS("DRM alloc flip_res=%p\n", flip_res);
#ifdef CONFIG_DMA_SHARED_BUFFER_USES_KDS
	if (bo->gem_object.export_dma_buf != NULL) {
		struct dma_buf *buf = bo->gem_object.export_dma_buf;
		unsigned long shared[1] = { 0 };
		struct kds_resource *resource_list[1] = {
				get_dma_buf_kds_resource(buf) };
		int err;

		get_dma_buf(buf);
		DRM_DEBUG_KMS("Got dma_buf %p\n", buf);

		/*                                                       */
		err = kds_async_waitall(&flip_res->kds_res_set,
					&priv.kds_cb, flip_res, fb, 1, shared,
					resource_list);
		BUG_ON(err);
	} else {
		struct pl111_drm_crtc *pl111_crtc = to_pl111_crtc(crtc);

		DRM_DEBUG_KMS("No dma_buf for this flip\n");

		/*                                                        */
		flip_res->kds_res_set = NULL;
		pl111_crtc->show_framebuffer_cb(flip_res, fb);
	}
#else
	if (bo->gem_object.export_dma_buf != NULL) {
		struct dma_buf *buf = bo->gem_object.export_dma_buf;

		get_dma_buf(buf);
		DRM_DEBUG_KMS("Got dma_buf %p\n", buf);
	} else {
		DRM_DEBUG_KMS("No dma_buf for this flip\n");
	}

	/*                                                                */
	{
		struct pl111_drm_crtc *pl111_crtc = to_pl111_crtc(crtc);
		pl111_crtc->show_framebuffer_cb(flip_res, fb);
	}
#endif

	/*                                                                
                                                       
  */
	if (!page_flip) {
		int ret = wait_event_killable(priv.wait_for_flips,
				flips_in_flight == 0);
		if (ret)
			return ret;
	}

	return 0;
}

int pl111_crtc_page_flip(struct drm_crtc *crtc, struct drm_framebuffer *fb,
			struct drm_pending_vblank_event *event)
{
	DRM_DEBUG_KMS("%s: crtc=%p, fb=%p, event=%p\n",
			__func__, crtc, fb, event);
	return show_framebuffer_on_crtc(crtc, fb, true, event);
}

int pl111_crtc_helper_mode_set(struct drm_crtc *crtc,
				struct drm_display_mode *mode,
				struct drm_display_mode *adjusted_mode,
				int x, int y, struct drm_framebuffer *old_fb)
{
	int ret;
	struct pl111_drm_crtc *pl111_crtc = to_pl111_crtc(crtc);
	struct drm_display_mode *duplicated_mode;

	DRM_DEBUG_KMS("DRM crtc_helper_mode_set, x=%d y=%d bpp=%d\n",
			adjusted_mode->hdisplay, adjusted_mode->vdisplay,
			crtc->fb->bits_per_pixel);

	duplicated_mode = drm_mode_duplicate(crtc->dev, adjusted_mode);
	if (!duplicated_mode)
		return -ENOMEM;

	pl111_crtc->new_mode = duplicated_mode;
	ret = show_framebuffer_on_crtc(crtc, crtc->fb, false, NULL);
	if (ret != 0) {
		pl111_crtc->new_mode = pl111_crtc->current_mode;
		drm_mode_destroy(crtc->dev, duplicated_mode);
	}

	return ret;
}

void pl111_crtc_helper_prepare(struct drm_crtc *crtc)
{
	DRM_DEBUG_KMS("DRM %s on crtc=%p\n", __func__, crtc);
}

void pl111_crtc_helper_commit(struct drm_crtc *crtc)
{
	DRM_DEBUG_KMS("DRM %s on crtc=%p\n", __func__, crtc);
}

bool pl111_crtc_helper_mode_fixup(struct drm_crtc *crtc,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0))
				const struct drm_display_mode *mode,
#else
				struct drm_display_mode *mode,
#endif
				struct drm_display_mode *adjusted_mode)
{
	DRM_DEBUG_KMS("DRM %s on crtc=%p\n", __func__, crtc);

#ifdef CONFIG_ARCH_VEXPRESS
	/*
                                                           
                                                          
  */
	if (mode->hdisplay == 1024 && mode->vdisplay == 768 &&
			crtc->fb->bits_per_pixel > 16) {
		DRM_INFO("*WARNING* 1024x768 at > 16 bpp may suffer corruption\n");
	}
#endif

	return true;
}

void pl111_crtc_helper_disable(struct drm_crtc *crtc)
{
	int ret;

	DRM_DEBUG_KMS("DRM %s on crtc=%p\n", __func__, crtc);

	/*                                                                     */
	ret = wait_event_killable(priv.wait_for_flips, atomic_read(&priv.nr_flips_in_flight) == 0);
	if(ret) {
		pr_err("pl111_crtc_helper_disable failed\n");
		return;
	}
	clcd_disable(crtc);
}

void pl111_crtc_destroy(struct drm_crtc *crtc)
{
	struct pl111_drm_crtc *pl111_crtc = to_pl111_crtc(crtc);

	DRM_DEBUG_KMS("DRM %s on crtc=%p\n", __func__, crtc);

	drm_crtc_cleanup(crtc);
	kfree(pl111_crtc);
}

const struct drm_crtc_funcs crtc_funcs = {
	.cursor_set = pl111_crtc_cursor_set,
	.cursor_move = pl111_crtc_cursor_move,
	.set_config = drm_crtc_helper_set_config,
	.page_flip = pl111_crtc_page_flip,
	.destroy = pl111_crtc_destroy
};

const struct drm_crtc_helper_funcs crtc_helper_funcs = {
	.mode_set = pl111_crtc_helper_mode_set,
	.prepare = pl111_crtc_helper_prepare,
	.commit = pl111_crtc_helper_commit,
	.mode_fixup = pl111_crtc_helper_mode_fixup,
	.disable = pl111_crtc_helper_disable,
};

struct pl111_drm_crtc *pl111_crtc_create(struct drm_device *dev)
{
	struct pl111_drm_crtc *pl111_crtc;

	pl111_crtc = kzalloc(sizeof(struct pl111_drm_crtc), GFP_KERNEL);
	if (pl111_crtc == NULL) {
		pr_err("Failed to allocated pl111_drm_crtc\n");
		return NULL;
	}

	drm_crtc_init(dev, &pl111_crtc->crtc, &crtc_funcs);
	drm_crtc_helper_add(&pl111_crtc->crtc, &crtc_helper_funcs);

	pl111_crtc->crtc_index = pl111_crtc_num;
	pl111_crtc_num++;
	pl111_crtc->crtc.enabled = 0;
	pl111_crtc->last_bpp = 0;
	pl111_crtc->current_update_res = NULL;
#ifdef CONFIG_DMA_SHARED_BUFFER_USES_KDS
	pl111_crtc->displaying_fb = NULL;
	pl111_crtc->old_kds_res_set = NULL;
	spin_lock_init(&pl111_crtc->current_displaying_lock);
#endif
	pl111_crtc->show_framebuffer_cb = show_framebuffer_on_crtc_cb_internal;
	INIT_LIST_HEAD(&pl111_crtc->update_queue);
	spin_lock_init(&pl111_crtc->base_update_lock);

	return pl111_crtc;
}

