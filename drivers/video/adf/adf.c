/*
 * Copyright (C) 2013 Google, Inc.
 * adf_modeinfo_{set_name,set_vrefresh} modified from
 * drivers/gpu/drm/drm_modes.c
 * adf_format_validate_yuv modified from framebuffer_check in
 * drivers/gpu/drm/drm_crtc.c
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/device.h>
#include <linux/idr.h>
#include <linux/highmem.h>
#include <linux/memblock.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include <video/adf_format.h>

#include "sw_sync.h"
#include "sync.h"

#include "adf.h"
#include "adf_fops.h"
#include "adf_sysfs.h"

#define CREATE_TRACE_POINTS
#include "adf_trace.h"

#define ADF_SHORT_FENCE_TIMEOUT (1 * MSEC_PER_SEC)
#define ADF_LONG_FENCE_TIMEOUT (10 * MSEC_PER_SEC)

static DEFINE_IDR(adf_devices);

static void adf_fence_wait(struct adf_device *dev, struct sync_fence *fence)
{
	/*                                                                  
                                                                       
                                                               */
	int err = sync_fence_wait(fence, ADF_SHORT_FENCE_TIMEOUT);
	if (err >= 0)
		return;

	if (err == -ETIME)
		err = sync_fence_wait(fence, ADF_LONG_FENCE_TIMEOUT);

	if (err < 0)
		dev_warn(&dev->base.dev, "error waiting on fence: %d\n", err);
}

void adf_buffer_cleanup(struct adf_buffer *buf)
{
	size_t i;
	for (i = 0; i < ARRAY_SIZE(buf->dma_bufs); i++)
		if (buf->dma_bufs[i])
			dma_buf_put(buf->dma_bufs[i]);

	if (buf->acquire_fence)
		sync_fence_put(buf->acquire_fence);
}

void adf_buffer_mapping_cleanup(struct adf_buffer_mapping *mapping,
		struct adf_buffer *buf)
{
	/*                                                                
                                                             
                         */
	size_t i;
	for (i = 0; i < ARRAY_SIZE(mapping->sg_tables); i++) {
		if (mapping->sg_tables[i])
			dma_buf_unmap_attachment(mapping->attachments[i],
					mapping->sg_tables[i], DMA_TO_DEVICE);
		if (mapping->attachments[i])
			dma_buf_detach(buf->dma_bufs[i],
					mapping->attachments[i]);
	}
}

void adf_post_cleanup(struct adf_device *dev, struct adf_pending_post *post)
{
	size_t i;

	if (post->state)
		dev->ops->state_free(dev, post->state);

	for (i = 0; i < post->config.n_bufs; i++) {
		adf_buffer_mapping_cleanup(&post->config.mappings[i],
				&post->config.bufs[i]);
		adf_buffer_cleanup(&post->config.bufs[i]);
	}

	kfree(post->config.custom_data);
	kfree(post->config.mappings);
	kfree(post->config.bufs);
	kfree(post);
}

static void adf_sw_advance_timeline(struct adf_device *dev)
{
#ifdef CONFIG_SW_SYNC
	sw_sync_timeline_inc(dev->timeline, 1);
#else
	BUG();
#endif
}

static void adf_post_work_func(struct kthread_work *work)
{
	struct adf_device *dev =
			container_of(work, struct adf_device, post_work);
	struct adf_pending_post *post, *next;
	struct list_head saved_list;

	mutex_lock(&dev->post_lock);
	memcpy(&saved_list, &dev->post_list, sizeof(saved_list));
	list_replace_init(&dev->post_list, &saved_list);
	mutex_unlock(&dev->post_lock);

	list_for_each_entry_safe(post, next, &saved_list, head) {
		int i;

		for (i = 0; i < post->config.n_bufs; i++) {
			struct sync_fence *fence =
					post->config.bufs[i].acquire_fence;
			if (fence)
				adf_fence_wait(dev, fence);
		}

		dev->ops->post(dev, &post->config, post->state);

		if (dev->ops->advance_timeline)
			dev->ops->advance_timeline(dev, &post->config,
					post->state);
		else
			adf_sw_advance_timeline(dev);

		list_del(&post->head);
		if (dev->onscreen)
			adf_post_cleanup(dev, dev->onscreen);
		dev->onscreen = post;
	}
}

void adf_attachment_free(struct adf_attachment_list *attachment)
{
	list_del(&attachment->head);
	kfree(attachment);
}

struct adf_event_refcount *adf_obj_find_event_refcount(struct adf_obj *obj,
		enum adf_event_type type)
{
	struct rb_root *root = &obj->event_refcount;
	struct rb_node **new = &(root->rb_node);
	struct rb_node *parent = NULL;
	struct adf_event_refcount *refcount;

	while (*new) {
		refcount = container_of(*new, struct adf_event_refcount, node);
		parent = *new;

		if (refcount->type > type)
			new = &(*new)->rb_left;
		else if (refcount->type < type)
			new = &(*new)->rb_right;
		else
			return refcount;
	}

	refcount = kzalloc(sizeof(*refcount), GFP_KERNEL);
	if (!refcount)
		return NULL;
	refcount->type = type;

	rb_link_node(&refcount->node, parent, new);
	rb_insert_color(&refcount->node, root);
	return refcount;
}

/* 
                                                     
  
                                           
                        
  
                                                                        
                                                                          
  
                                                                          
                        
 */
int adf_event_get(struct adf_obj *obj, enum adf_event_type type)
{
	struct adf_event_refcount *refcount;
	int old_refcount;
	int ret;

	ret = adf_obj_check_supports_event(obj, type);
	if (ret < 0)
		return ret;

	mutex_lock(&obj->event_lock);

	refcount = adf_obj_find_event_refcount(obj, type);
	if (!refcount) {
		ret = -ENOMEM;
		goto done;
	}

	old_refcount = refcount->refcount++;

	if (old_refcount == 0) {
		obj->ops->set_event(obj, type, true);
		trace_adf_event_enable(obj, type);
	}

done:
	mutex_unlock(&obj->event_lock);
	return ret;
}
EXPORT_SYMBOL(adf_event_get);

/* 
                                                     
  
                                           
                        
  
                                                                        
                                                                          
  
                                                                       
                                                                    
 */
int adf_event_put(struct adf_obj *obj, enum adf_event_type type)
{
	struct adf_event_refcount *refcount;
	int old_refcount;
	int ret;

	ret = adf_obj_check_supports_event(obj, type);
	if (ret < 0)
		return ret;


	mutex_lock(&obj->event_lock);

	refcount = adf_obj_find_event_refcount(obj, type);
	if (!refcount) {
		ret = -ENOMEM;
		goto done;
	}

	old_refcount = refcount->refcount--;

	if (WARN_ON(old_refcount == 0)) {
		refcount->refcount++;
		ret = -EALREADY;
	} else if (old_refcount == 1) {
		obj->ops->set_event(obj, type, false);
		trace_adf_event_disable(obj, type);
	}

done:
	mutex_unlock(&obj->event_lock);
	return ret;
}
EXPORT_SYMBOL(adf_event_put);

/* 
                                                                 
  
                               
                                                       
  
                                                                               
  
                                                                        
                                                                               
                                                                          
           
 */
int adf_vsync_wait(struct adf_interface *intf, long timeout)
{
	ktime_t timestamp;
	int ret;
	unsigned long flags;

	read_lock_irqsave(&intf->vsync_lock, flags);
	timestamp = intf->vsync_timestamp;
	read_unlock_irqrestore(&intf->vsync_lock, flags);

	adf_vsync_get(intf);
	if (timeout) {
		ret = wait_event_interruptible_timeout(intf->vsync_wait,
				!ktime_equal(timestamp,
						intf->vsync_timestamp),
				msecs_to_jiffies(timeout));
		if (ret == 0 && ktime_equal(timestamp, intf->vsync_timestamp))
			ret = -ETIMEDOUT;
	} else {
		ret = wait_event_interruptible(intf->vsync_wait,
				!ktime_equal(timestamp,
						intf->vsync_timestamp));
	}
	adf_vsync_put(intf);

	return ret;
}
EXPORT_SYMBOL(adf_vsync_wait);

static void adf_event_queue(struct adf_obj *obj, struct adf_event *event)
{
	struct adf_file *file;
	unsigned long flags;

	trace_adf_event(obj, event->type);

	spin_lock_irqsave(&obj->file_lock, flags);

	list_for_each_entry(file, &obj->file_list, head)
		if (test_bit(event->type, file->event_subscriptions))
			adf_file_queue_event(file, event);

	spin_unlock_irqrestore(&obj->file_lock, flags);
}

/* 
                                                                
  
                                               
                    
  
                                                                           
                                                                         
  
                                                                         
                                      
                                                                  
 */
int adf_event_notify(struct adf_obj *obj, struct adf_event *event)
{
	if (WARN_ON(event->type == ADF_EVENT_VSYNC ||
			event->type == ADF_EVENT_HOTPLUG))
		return -EINVAL;

	adf_event_queue(obj, event);
	return 0;
}
EXPORT_SYMBOL(adf_event_notify);

/* 
                                                                     
  
                               
                                          
  
                                                                  
 */
void adf_vsync_notify(struct adf_interface *intf, ktime_t timestamp)
{
	unsigned long flags;
	struct adf_vsync_event event;

	write_lock_irqsave(&intf->vsync_lock, flags);
	intf->vsync_timestamp = timestamp;
	write_unlock_irqrestore(&intf->vsync_lock, flags);

	wake_up_interruptible_all(&intf->vsync_wait);

	event.base.type = ADF_EVENT_VSYNC;
	event.base.length = sizeof(event);
	event.timestamp = ktime_to_ns(timestamp);
	adf_event_queue(&intf->base, &event.base);
}
EXPORT_SYMBOL(adf_vsync_notify);

void adf_hotplug_notify(struct adf_interface *intf, bool connected,
		struct drm_mode_modeinfo *modelist, size_t n_modes)
{
	unsigned long flags;
	struct adf_hotplug_event event;
	struct drm_mode_modeinfo *old_modelist;

	write_lock_irqsave(&intf->hotplug_modelist_lock, flags);
	old_modelist = intf->modelist;
	intf->hotplug_detect = connected;
	intf->modelist = modelist;
	intf->n_modes = n_modes;
	write_unlock_irqrestore(&intf->hotplug_modelist_lock, flags);

	kfree(old_modelist);

	event.base.length = sizeof(event);
	event.base.type = ADF_EVENT_HOTPLUG;
	event.connected = connected;
	adf_event_queue(&intf->base, &event.base);
}

/* 
                                                                         
                         
  
                               
                                                 
                               
  
                                                                             
  
                                                                         
           
  
                                                    
 */
int adf_hotplug_notify_connected(struct adf_interface *intf,
		struct drm_mode_modeinfo *modelist, size_t n_modes)
{
	struct drm_mode_modeinfo *modelist_copy;

	if (n_modes > ADF_MAX_MODES)
		return -ENOMEM;

	modelist_copy = kzalloc(sizeof(modelist_copy[0]) * n_modes,
			GFP_KERNEL);
	if (!modelist_copy)
		return -ENOMEM;
	memcpy(modelist_copy, modelist, sizeof(modelist_copy[0]) * n_modes);

	adf_hotplug_notify(intf, true, modelist_copy, n_modes);
	return 0;
}
EXPORT_SYMBOL(adf_hotplug_notify_connected);

/* 
                                                                            
                              
  
                               
  
                                                                        
           
 */
void adf_hotplug_notify_disconnected(struct adf_interface *intf)
{
	adf_hotplug_notify(intf, false, NULL, 0);
}
EXPORT_SYMBOL(adf_hotplug_notify_disconnected);

static int adf_obj_init(struct adf_obj *obj, enum adf_obj_type type,
		struct idr *idr, struct adf_device *parent,
		const struct adf_obj_ops *ops, const char *fmt, va_list args)
{
	int ret;

	if (ops && ops->supports_event && !ops->set_event) {
		pr_err("%s: %s implements supports_event but not set_event\n",
				__func__, adf_obj_type_str(type));
		return -EINVAL;
	}

	ret = idr_alloc(idr, obj, 0, 0, GFP_KERNEL);
	if (ret < 0) {
		pr_err("%s: allocating object id failed: %d\n", __func__, ret);
		return ret;
	}
	obj->id = ret;

	vscnprintf(obj->name, sizeof(obj->name), fmt, args);

	obj->type = type;
	obj->ops = ops;
	obj->parent = parent;
	mutex_init(&obj->event_lock);
	obj->event_refcount = RB_ROOT;
	spin_lock_init(&obj->file_lock);
	INIT_LIST_HEAD(&obj->file_list);
	return 0;
}

static void adf_obj_destroy(struct adf_obj *obj, struct idr *idr)
{
	struct rb_node *node = rb_first(&obj->event_refcount);

	while (node) {
		struct adf_event_refcount *refcount =
				container_of(node, struct adf_event_refcount,
						node);
		rb_erase(&refcount->node, &obj->event_refcount);
		kfree(refcount);
		node = rb_first(&obj->event_refcount);
	}

	mutex_destroy(&obj->event_lock);
	idr_remove(idr, obj->id);
}

/* 
                                                                      
                           
  
                           
                                      
                                    
                                                        
  
                                                                      
                                                    
  
                                                      
 */
int adf_device_init(struct adf_device *dev, struct device *parent,
		const struct adf_device_ops *ops, const char *fmt, ...)
{
	int ret;
	va_list args;

	if (!ops->validate || !ops->post) {
		pr_err("%s: device must implement validate and post\n",
				__func__);
		return -EINVAL;
	}

	if (!ops->complete_fence && !ops->advance_timeline) {
		if (!IS_ENABLED(CONFIG_SW_SYNC)) {
			pr_err("%s: device requires sw_sync but it is not enabled in the kernel\n",
					__func__);
			return -EINVAL;
		}
	} else if (!(ops->complete_fence && ops->advance_timeline)) {
		pr_err("%s: device must implement both complete_fence and advance_timeline, or implement neither\n",
				__func__);
		return -EINVAL;
	}

	memset(dev, 0, sizeof(*dev));

	va_start(args, fmt);
	ret = adf_obj_init(&dev->base, ADF_OBJ_DEVICE, &adf_devices, dev,
			&ops->base, fmt, args);
	va_end(args);
	if (ret < 0)
		return ret;

	dev->dev = parent;
	dev->ops = ops;
	idr_init(&dev->overlay_engines);
	idr_init(&dev->interfaces);
	mutex_init(&dev->client_lock);
	INIT_LIST_HEAD(&dev->post_list);
	mutex_init(&dev->post_lock);
	init_kthread_worker(&dev->post_worker);
	INIT_LIST_HEAD(&dev->attached);
	INIT_LIST_HEAD(&dev->attach_allowed);

	dev->post_thread = kthread_run(kthread_worker_fn,
			&dev->post_worker, dev->base.name);
	if (IS_ERR(dev->post_thread)) {
		ret = PTR_ERR(dev->post_thread);
		dev->post_thread = NULL;

		pr_err("%s: failed to run config posting thread: %d\n",
				__func__, ret);
		goto err;
	}
	init_kthread_work(&dev->post_work, adf_post_work_func);

	ret = adf_device_sysfs_init(dev);
	if (ret < 0)
		goto err;

	return 0;

err:
	adf_device_destroy(dev);
	return ret;
}
EXPORT_SYMBOL(adf_device_init);

/* 
                                                                       
  
                           
 */
void adf_device_destroy(struct adf_device *dev)
{
	struct adf_attachment_list *entry, *next;

	idr_destroy(&dev->interfaces);
	idr_destroy(&dev->overlay_engines);

	if (dev->post_thread) {
		flush_kthread_worker(&dev->post_worker);
		kthread_stop(dev->post_thread);
	}

	if (dev->onscreen)
		adf_post_cleanup(dev, dev->onscreen);
	adf_device_sysfs_destroy(dev);
	list_for_each_entry_safe(entry, next, &dev->attach_allowed, head) {
		adf_attachment_free(entry);
	}
	list_for_each_entry_safe(entry, next, &dev->attached, head) {
		adf_attachment_free(entry);
	}
	mutex_destroy(&dev->post_lock);
	mutex_destroy(&dev->client_lock);

	if (dev->timeline)
		sync_timeline_destroy(&dev->timeline->obj);

	adf_obj_destroy(&dev->base, &adf_devices);
}
EXPORT_SYMBOL(adf_device_destroy);

/* 
                                                                            
                           
  
                               
                                                
                                                       
                                       
                                                           
                                                                   
                                       
                                                           
  
                                                                     
  
                                                          
                                                                         
                                     
  
                                                      
 */
int adf_interface_init(struct adf_interface *intf, struct adf_device *dev,
		enum adf_interface_type type, u32 idx, u32 flags,
		const struct adf_interface_ops *ops, const char *fmt, ...)
{
	int ret;
	va_list args;
	const u32 allowed_flags = ADF_INTF_FLAG_PRIMARY |
			ADF_INTF_FLAG_EXTERNAL;

	if (dev->n_interfaces == ADF_MAX_INTERFACES) {
		pr_err("%s: parent device %s has too many interfaces\n",
				__func__, dev->base.name);
		return -ENOMEM;
	}

	if (type >= ADF_INTF_MEMORY && type <= ADF_INTF_TYPE_DEVICE_CUSTOM) {
		pr_err("%s: invalid interface type %u\n", __func__, type);
		return -EINVAL;
	}

	if (flags & ~allowed_flags) {
		pr_err("%s: invalid interface flags 0x%X\n", __func__,
				flags & ~allowed_flags);
		return -EINVAL;
	}

	memset(intf, 0, sizeof(*intf));

	va_start(args, fmt);
	ret = adf_obj_init(&intf->base, ADF_OBJ_INTERFACE, &dev->interfaces,
			dev, ops ? &ops->base : NULL, fmt, args);
	va_end(args);
	if (ret < 0)
		return ret;

	intf->type = type;
	intf->idx = idx;
	intf->flags = flags;
	intf->ops = ops;
	intf->dpms_state = DRM_MODE_DPMS_OFF;
	init_waitqueue_head(&intf->vsync_wait);
	rwlock_init(&intf->vsync_lock);
	rwlock_init(&intf->hotplug_modelist_lock);

	ret = adf_interface_sysfs_init(intf);
	if (ret < 0)
		goto err;
	dev->n_interfaces++;

	return 0;

err:
	adf_obj_destroy(&intf->base, &dev->interfaces);
	return ret;
}
EXPORT_SYMBOL(adf_interface_init);

/* 
                                                                             
  
                               
 */
void adf_interface_destroy(struct adf_interface *intf)
{
	struct adf_device *dev = adf_interface_parent(intf);
	struct adf_attachment_list *entry, *next;

	mutex_lock(&dev->client_lock);
	list_for_each_entry_safe(entry, next, &dev->attach_allowed, head) {
		if (entry->attachment.interface == intf) {
			adf_attachment_free(entry);
			dev->n_attach_allowed--;
		}
	}
	list_for_each_entry_safe(entry, next, &dev->attached, head) {
		if (entry->attachment.interface == intf) {
			adf_device_detach_op(dev,
					entry->attachment.overlay_engine, intf);
			adf_attachment_free(entry);
			dev->n_attached--;
		}
	}
	kfree(intf->modelist);
	adf_interface_sysfs_destroy(intf);
	adf_obj_destroy(&intf->base, &dev->interfaces);
	dev->n_interfaces--;
	mutex_unlock(&dev->client_lock);
}
EXPORT_SYMBOL(adf_interface_destroy);

static bool adf_overlay_engine_has_custom_formats(
		const struct adf_overlay_engine_ops *ops)
{
	size_t i;
	for (i = 0; i < ops->n_supported_formats; i++)
		if (!adf_format_is_standard(ops->supported_formats[i]))
			return true;
	return false;
}

/* 
                                                                
                                          
  
                           
                                                     
                                            
                                                        
  
                                                                     
  
                                                          
                                                                              
                                     
  
                                                      
 */
int adf_overlay_engine_init(struct adf_overlay_engine *eng,
		struct adf_device *dev,
		const struct adf_overlay_engine_ops *ops, const char *fmt, ...)
{
	int ret;
	va_list args;

	if (!ops->supported_formats) {
		pr_err("%s: overlay engine must support at least one format\n",
				__func__);
		return -EINVAL;
	}

	if (ops->n_supported_formats > ADF_MAX_SUPPORTED_FORMATS) {
		pr_err("%s: overlay engine supports too many formats\n",
				__func__);
		return -EINVAL;
	}

	if (adf_overlay_engine_has_custom_formats(ops) &&
			!dev->ops->validate_custom_format) {
		pr_err("%s: overlay engine has custom formats but parent device %s does not implement validate_custom_format\n",
				__func__, dev->base.name);
		return -EINVAL;
	}

	memset(eng, 0, sizeof(*eng));

	va_start(args, fmt);
	ret = adf_obj_init(&eng->base, ADF_OBJ_OVERLAY_ENGINE,
			&dev->overlay_engines, dev, &ops->base, fmt, args);
	va_end(args);
	if (ret < 0)
		return ret;

	eng->ops = ops;

	ret = adf_overlay_engine_sysfs_init(eng);
	if (ret < 0)
		goto err;

	return 0;

err:
	adf_obj_destroy(&eng->base, &dev->overlay_engines);
	return ret;
}
EXPORT_SYMBOL(adf_overlay_engine_init);

/* 
                                                                           
  
                           
 */
void adf_overlay_engine_destroy(struct adf_overlay_engine *eng)
{
	struct adf_device *dev = adf_overlay_engine_parent(eng);
	struct adf_attachment_list *entry, *next;

	mutex_lock(&dev->client_lock);
	list_for_each_entry_safe(entry, next, &dev->attach_allowed, head) {
		if (entry->attachment.overlay_engine == eng) {
			adf_attachment_free(entry);
			dev->n_attach_allowed--;
		}
	}
	list_for_each_entry_safe(entry, next, &dev->attached, head) {
		if (entry->attachment.overlay_engine == eng) {
			adf_device_detach_op(dev, eng,
					entry->attachment.interface);
			adf_attachment_free(entry);
			dev->n_attached--;
		}
	}
	adf_overlay_engine_sysfs_destroy(eng);
	adf_obj_destroy(&eng->base, &dev->overlay_engines);
	mutex_unlock(&dev->client_lock);
}
EXPORT_SYMBOL(adf_overlay_engine_destroy);

struct adf_attachment_list *adf_attachment_find(struct list_head *list,
		struct adf_overlay_engine *eng, struct adf_interface *intf)
{
	struct adf_attachment_list *entry;
	list_for_each_entry(entry, list, head) {
		if (entry->attachment.interface == intf &&
				entry->attachment.overlay_engine == eng)
			return entry;
	}
	return NULL;
}

int adf_attachment_validate(struct adf_device *dev,
		struct adf_overlay_engine *eng, struct adf_interface *intf)
{
	struct adf_device *intf_dev = adf_interface_parent(intf);
	struct adf_device *eng_dev = adf_overlay_engine_parent(eng);

	if (intf_dev != dev) {
		dev_err(&dev->base.dev, "can't attach interface %s belonging to device %s\n",
				intf->base.name, intf_dev->base.name);
		return -EINVAL;
	}

	if (eng_dev != dev) {
		dev_err(&dev->base.dev, "can't attach overlay engine %s belonging to device %s\n",
				eng->base.name, eng_dev->base.name);
		return -EINVAL;
	}

	return 0;
}

/* 
                                                                
              
  
                          
                           
                       
  
                                                                               
                                                                   
                                                                            
  
                                                                             
                     
 */
int adf_attachment_allow(struct adf_device *dev,
		struct adf_overlay_engine *eng, struct adf_interface *intf)
{
	int ret;
	struct adf_attachment_list *entry = NULL;

	ret = adf_attachment_validate(dev, eng, intf);
	if (ret < 0)
		return ret;

	mutex_lock(&dev->client_lock);

	if (dev->n_attach_allowed == ADF_MAX_ATTACHMENTS) {
		ret = -ENOMEM;
		goto done;
	}

	if (adf_attachment_find(&dev->attach_allowed, eng, intf)) {
		ret = -EALREADY;
		goto done;
	}

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry) {
		ret = -ENOMEM;
		goto done;
	}

	entry->attachment.interface = intf;
	entry->attachment.overlay_engine = eng;
	list_add_tail(&entry->head, &dev->attach_allowed);
	dev->n_attach_allowed++;

done:
	mutex_unlock(&dev->client_lock);
	if (ret < 0)
		kfree(entry);

	return ret;
}
EXPORT_SYMBOL(adf_attachment_allow);

/* 
                                                              
  
                         
 */
const char *adf_obj_type_str(enum adf_obj_type type)
{
	switch (type) {
	case ADF_OBJ_OVERLAY_ENGINE:
		return "overlay engine";

	case ADF_OBJ_INTERFACE:
		return "interface";

	case ADF_OBJ_DEVICE:
		return "device";

	default:
		return "unknown";
	}
}
EXPORT_SYMBOL(adf_obj_type_str);

/* 
                                                                            
  
                       
 */
const char *adf_interface_type_str(struct adf_interface *intf)
{
	switch (intf->type) {
	case ADF_INTF_DSI:
		return "DSI";

	case ADF_INTF_eDP:
		return "eDP";

	case ADF_INTF_DPI:
		return "DPI";

	case ADF_INTF_VGA:
		return "VGA";

	case ADF_INTF_DVI:
		return "DVI";

	case ADF_INTF_HDMI:
		return "HDMI";

	case ADF_INTF_MEMORY:
		return "memory";

	default:
		if (intf->type >= ADF_INTF_TYPE_DEVICE_CUSTOM) {
			if (intf->ops && intf->ops->type_str)
				return intf->ops->type_str(intf);
			return "custom";
		}
		return "unknown";
	}
}
EXPORT_SYMBOL(adf_interface_type_str);

/* 
                                                                  
  
                                           
                    
 */
const char *adf_event_type_str(struct adf_obj *obj, enum adf_event_type type)
{
	switch (type) {
	case ADF_EVENT_VSYNC:
		return "vsync";

	case ADF_EVENT_HOTPLUG:
		return "hotplug";

	default:
		if (type >= ADF_EVENT_DEVICE_CUSTOM) {
			if (obj->ops && obj->ops->event_type_str)
				return obj->ops->event_type_str(obj, type);
			return "custom";
		}
		return "unknown";
	}
}
EXPORT_SYMBOL(adf_event_type_str);

/* 
                                                                     
  
                         
                                                             
 */
void adf_format_str(u32 format, char buf[ADF_FORMAT_STR_SIZE])
{
	buf[0] = format & 0xFF;
	buf[1] = (format >> 8) & 0xFF;
	buf[2] = (format >> 16) & 0xFF;
	buf[3] = (format >> 24) & 0xFF;
	buf[4] = '\0';
}
EXPORT_SYMBOL(adf_format_str);

/* 
                                                                              
                            
  
                                             
                           
                                         
                                                                  
                                                                
                                                                     
  
                                                                             
                               
  
                                                                     
                                             
 */
int adf_format_validate_yuv(struct adf_device *dev, struct adf_buffer *buf,
		u8 num_planes, u8 hsub, u8 vsub, u8 cpp[])
{
	u8 i;

	if (num_planes != buf->n_planes) {
		char format_str[ADF_FORMAT_STR_SIZE];
		adf_format_str(buf->format, format_str);
		dev_err(&dev->base.dev, "%u planes expected for format %s but %u planes provided\n",
				num_planes, format_str, buf->n_planes);
		return -EINVAL;
	}

	if (buf->w == 0 || buf->w % hsub) {
		dev_err(&dev->base.dev, "bad buffer width %u\n", buf->w);
		return -EINVAL;
	}

	if (buf->h == 0 || buf->h % vsub) {
		dev_err(&dev->base.dev, "bad buffer height %u\n", buf->h);
		return -EINVAL;
	}

	for (i = 0; i < num_planes; i++) {
		u32 width = buf->w / (i != 0 ? hsub : 1);
		u32 height = buf->h / (i != 0 ? vsub : 1);
		u8 cpp = adf_format_plane_cpp(buf->format, i);
		u32 last_line_size;

		if (buf->pitch[i] < (u64) width * cpp) {
			dev_err(&dev->base.dev, "plane %u pitch is shorter than buffer width (pitch = %u, width = %u, bpp = %u)\n",
					i, buf->pitch[i], width, cpp * 8);
			return -EINVAL;
		}

		switch (dev->ops->quirks.buffer_padding) {
		case ADF_BUFFER_PADDED_TO_PITCH:
			last_line_size = buf->pitch[i];
			break;

		case ADF_BUFFER_UNPADDED:
			last_line_size = width * cpp;
			break;

		default:
			BUG();
		}

		if ((u64) (height - 1) * buf->pitch[i] + last_line_size +
				buf->offset[i] > buf->dma_bufs[i]->size) {
			dev_err(&dev->base.dev, "plane %u buffer too small (height = %u, pitch = %u, offset = %u, size = %zu)\n",
					i, height, buf->pitch[i],
					buf->offset[i], buf->dma_bufs[i]->size);
			return -EINVAL;
		}
	}

	return 0;
}
EXPORT_SYMBOL(adf_format_validate_yuv);

/* 
                                                                              
  
              
  
                                                             
                                                                     
                                                   
 */
void adf_modeinfo_set_name(struct drm_mode_modeinfo *mode)
{
	bool interlaced = mode->flags & DRM_MODE_FLAG_INTERLACE;

	snprintf(mode->name, DRM_DISPLAY_MODE_LEN, "%dx%d%s",
		 mode->hdisplay, mode->vdisplay,
		 interlaced ? "i" : "");
}
EXPORT_SYMBOL(adf_modeinfo_set_name);

/* 
                                                                         
              
  
              
  
                                                              
                                                                        
                                                          
 */
void adf_modeinfo_set_vrefresh(struct drm_mode_modeinfo *mode)
{
	int refresh = 0;
	unsigned int calc_val;

	if (mode->vrefresh > 0)
		return;

	if (mode->htotal <= 0 || mode->vtotal <= 0)
		return;

	/*                                           */
	calc_val = (mode->clock * 1000);
	calc_val /= mode->htotal;
	refresh = (calc_val + mode->vtotal / 2) / mode->vtotal;

	if (mode->flags & DRM_MODE_FLAG_INTERLACE)
		refresh *= 2;
	if (mode->flags & DRM_MODE_FLAG_DBLSCAN)
		refresh /= 2;
	if (mode->vscan > 1)
		refresh /= mode->vscan;

	mode->vrefresh = refresh;
}
EXPORT_SYMBOL(adf_modeinfo_set_vrefresh);

static int __init adf_init(void)
{
	int err;

	err = adf_sysfs_init();
	if (err < 0)
		return err;

	return 0;
}

static void __exit adf_exit(void)
{
	adf_sysfs_destroy();
}

module_init(adf_init);
module_exit(adf_exit);
