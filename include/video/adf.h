/*
 * Copyright (C) 2013 Google, Inc.
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

#ifndef _VIDEO_ADF_H
#define _VIDEO_ADF_H

#include <linux/device.h>
#include <linux/dma-buf.h>
#include <linux/idr.h>
#include <linux/kref.h>
#include <linux/kthread.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/scatterlist.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include <uapi/video/adf.h>
#include "sync.h"

struct adf_obj;
struct adf_obj_ops;
struct adf_device;
struct adf_device_ops;
struct adf_interface;
struct adf_interface_ops;
struct adf_overlay_engine;
struct adf_overlay_engine_ops;

/* 
                                                   
  
                                         
                                        
                                         
                                                                   
                                    
                                                         
                                                           
                                        
                                                                 
                    
  
                                                                          
                             
 */
struct adf_buffer {
	struct adf_overlay_engine *overlay_engine;

	u32 w;
	u32 h;
	u32 format;

	struct dma_buf *dma_bufs[ADF_MAX_PLANES];
	u32 offset[ADF_MAX_PLANES];
	u32 pitch[ADF_MAX_PLANES];
	u8 n_planes;

	struct sync_fence *acquire_fence;
};

/* 
                                                                              
                 
  
                                                  
                                       
 */
struct adf_buffer_mapping {
	struct dma_buf_attachment *attachments[ADF_MAX_PLANES];
	struct sg_table *sg_tables[ADF_MAX_PLANES];
};

/* 
                                                            
  
                                       
                           
                                                     
                                                 
                                    
  
                                                                        
                           
 */
struct adf_post {
	size_t n_bufs;
	struct adf_buffer *bufs;
	struct adf_buffer_mapping *mappings;

	size_t custom_data_size;
	void *custom_data;
};

/* 
                                                                              
                   
  
                                      
                            
  
                                                                              
                                 
 */
struct adf_attachment {
	struct adf_overlay_engine *overlay_engine;
	struct adf_interface *interface;
};

struct adf_pending_post {
	struct list_head head;
	struct adf_post config;
	void *state;
};

enum adf_obj_type {
	ADF_OBJ_OVERLAY_ENGINE = 0,
	ADF_OBJ_INTERFACE = 1,
	ADF_OBJ_DEVICE = 2,
};

/* 
                                                            
  
                                                 
                                          
                               
  
                                                                                
        
                                                     
                                                                        
                                       
  
                                                                             
                                                                           
                                                                   
 */
struct adf_obj_ops {
	/*          */
	int (*open)(struct adf_obj *obj, struct inode *inode,
			struct file *file);
	/*          */
	void (*release)(struct adf_obj *obj, struct inode *inode,
			struct file *file);
	/*          */
	long (*ioctl)(struct adf_obj *obj, unsigned int cmd, unsigned long arg);

	/*          */
	bool (*supports_event)(struct adf_obj *obj, enum adf_event_type type);
	/*                                           */
	void (*set_event)(struct adf_obj *obj, enum adf_event_type type,
			bool enabled);
	/*          */
	const char *(*event_type_str)(struct adf_obj *obj,
			enum adf_event_type type);

	/*          */
	int (*custom_data)(struct adf_obj *obj, void *data, size_t *size);
};

struct adf_obj {
	enum adf_obj_type type;
	char name[ADF_NAME_LEN];
	struct adf_device *parent;

	const struct adf_obj_ops *ops;

	struct device dev;

	struct spinlock file_lock;
	struct list_head file_list;

	struct mutex event_lock;
	struct rb_root event_refcount;

	int id;
	int minor;
};

/* 
                                                          
  
                                                                        
                                                                       
                                
 */
struct adf_device_quirks {
	/*                                                  */
	enum {
		ADF_BUFFER_PADDED_TO_PITCH = 0,
		ADF_BUFFER_UNPADDED = 1,
	} buffer_padding;
};

/* 
                                                            
  
                          
                                                     
                                                           
  
                                                                               
                                 
                                                                         
                                         
  
                                                                  
                                                                      
                                                                          
                                
  
                                                                          
                                                                         
                                                                        
                                                                      
                                         
                                                                           
                                                                        
                                                                   
                        
                                                                                
                         
                                                                               
                                                                        
            
                                                                     
 */
struct adf_device_ops {
	/*          */
	struct module *owner;
	const struct adf_obj_ops base;
	/*          */
	const struct adf_device_quirks quirks;

	/*          */
	int (*attach)(struct adf_device *dev, struct adf_overlay_engine *eng,
			struct adf_interface *intf);
	/*          */
	int (*detach)(struct adf_device *dev, struct adf_overlay_engine *eng,
			struct adf_interface *intf);

	/*                                                                      
                  */
	int (*validate_custom_format)(struct adf_device *dev,
			struct adf_buffer *buf);

	/*          */
	int (*validate)(struct adf_device *dev, struct adf_post *cfg,
			void **driver_state);
	/*          */
	struct sync_fence *(*complete_fence)(struct adf_device *dev,
			struct adf_post *cfg, void *driver_state);
	/*          */
	void (*post)(struct adf_device *dev, struct adf_post *cfg,
			void *driver_state);
	/*                                           */
	void (*advance_timeline)(struct adf_device *dev,
			struct adf_post *cfg, void *driver_state);
	/*                                             */
	void (*state_free)(struct adf_device *dev, void *driver_state);
};

struct adf_attachment_list {
	struct adf_attachment attachment;
	struct list_head head;
};

struct adf_device {
	struct adf_obj base;
	struct device *dev;

	const struct adf_device_ops *ops;

	struct mutex client_lock;

	struct idr interfaces;
	size_t n_interfaces;
	struct idr overlay_engines;

	struct list_head post_list;
	struct mutex post_lock;
	struct kthread_worker post_worker;
	struct task_struct *post_thread;
	struct kthread_work post_work;

	struct list_head attached;
	size_t n_attached;
	struct list_head attach_allowed;
	size_t n_attach_allowed;

	struct adf_pending_post *onscreen;

	struct sw_sync_timeline *timeline;
	int timeline_max;
};

/* 
                                                                  
  
                                                     
  
                                                                         
                        
  
                                                                         
                                                         
                                                                          
                                                                        
                                              
                                                                             
                                                                     
                                                                           
                                         
  
                                                                               
                                                                    
                                                                           
                                                
  
                                                                         
                                                                         
                          
  
                                                                  
                                                 
 */
struct adf_interface_ops {
	const struct adf_obj_ops base;

	/*          */
	int (*blank)(struct adf_interface *intf, u8 state);

	/*          */
	int (*alloc_simple_buffer)(struct adf_interface *intf,
			u16 w, u16 h, u32 format,
			struct dma_buf **dma_buf, u32 *offset, u32 *pitch);
	/*          */
	int (*describe_simple_post)(struct adf_interface *intf,
			struct adf_buffer *fb, void *data, size_t *size);

	/*          */
	int (*modeset)(struct adf_interface *intf,
			struct drm_mode_modeinfo *mode);

	/*          */
	int (*screen_size)(struct adf_interface *intf, u16 *width_mm,
			u16 *height_mm);

	/*          */
	const char *(*type_str)(struct adf_interface *intf);
};

struct adf_interface {
	struct adf_obj base;
	const struct adf_interface_ops *ops;

	struct drm_mode_modeinfo current_mode;

	enum adf_interface_type type;
	u32 idx;
	u32 flags;

	wait_queue_head_t vsync_wait;
	ktime_t vsync_timestamp;
	rwlock_t vsync_lock;

	u8 dpms_state;

	bool hotplug_detect;
	struct drm_mode_modeinfo *modelist;
	size_t n_modes;
	rwlock_t hotplug_modelist_lock;
};

/* 
                                                               
  
                                                     
  
                                                                      
                                                           
                            
 */
struct adf_overlay_engine_ops {
	const struct adf_obj_ops base;

	/*          */
	const u32 *supported_formats;
	/*          */
	const size_t n_supported_formats;
};

struct adf_overlay_engine {
	struct adf_obj base;

	const struct adf_overlay_engine_ops *ops;
};

#define adf_obj_to_device(ptr) \
	container_of((ptr), struct adf_device, base)

#define adf_obj_to_interface(ptr) \
	container_of((ptr), struct adf_interface, base)

#define adf_obj_to_overlay_engine(ptr) \
	container_of((ptr), struct adf_overlay_engine, base)

int __printf(4, 5) adf_device_init(struct adf_device *dev,
		struct device *parent, const struct adf_device_ops *ops,
		const char *fmt, ...);
void adf_device_destroy(struct adf_device *dev);
int __printf(7, 8) adf_interface_init(struct adf_interface *intf,
		struct adf_device *dev, enum adf_interface_type type, u32 idx,
		u32 flags, const struct adf_interface_ops *ops, const char *fmt,
		...);
void adf_interface_destroy(struct adf_interface *intf);
static inline struct adf_device *adf_interface_parent(
		struct adf_interface *intf)
{
	return intf->base.parent;
}
int __printf(4, 5) adf_overlay_engine_init(struct adf_overlay_engine *eng,
		struct adf_device *dev,
		const struct adf_overlay_engine_ops *ops, const char *fmt, ...);
void adf_overlay_engine_destroy(struct adf_overlay_engine *eng);
static inline struct adf_device *adf_overlay_engine_parent(
		struct adf_overlay_engine *eng)
{
	return eng->base.parent;
}

int adf_attachment_allow(struct adf_device *dev, struct adf_overlay_engine *eng,
		struct adf_interface *intf);

const char *adf_obj_type_str(enum adf_obj_type type);
const char *adf_interface_type_str(struct adf_interface *intf);
const char *adf_event_type_str(struct adf_obj *obj, enum adf_event_type type);

#define ADF_FORMAT_STR_SIZE 5
void adf_format_str(u32 format, char buf[ADF_FORMAT_STR_SIZE]);
int adf_format_validate_yuv(struct adf_device *dev, struct adf_buffer *buf,
		u8 num_planes, u8 hsub, u8 vsub, u8 cpp[]);
/* 
                                                                              
                            
  
                                             
                           
                                 
  
                                                                             
                                                                   
  
                                                                        
             
 */
static inline int adf_format_validate_rgb(struct adf_device *dev,
		struct adf_buffer *buf, u8 cpp)
{
	return adf_format_validate_yuv(dev, buf, 1, 1, 1, &cpp);
}

int adf_event_get(struct adf_obj *obj, enum adf_event_type type);
int adf_event_put(struct adf_obj *obj, enum adf_event_type type);
int adf_event_notify(struct adf_obj *obj, struct adf_event *event);

static inline void adf_vsync_get(struct adf_interface *intf)
{
	adf_event_get(&intf->base, ADF_EVENT_VSYNC);
}

static inline void adf_vsync_put(struct adf_interface *intf)
{
	adf_event_put(&intf->base, ADF_EVENT_VSYNC);
}

int adf_vsync_wait(struct adf_interface *intf, long timeout);
void adf_vsync_notify(struct adf_interface *intf, ktime_t timestamp);

int adf_hotplug_notify_connected(struct adf_interface *intf,
		struct drm_mode_modeinfo *modelist, size_t n_modes);
void adf_hotplug_notify_disconnected(struct adf_interface *intf);

void adf_modeinfo_set_name(struct drm_mode_modeinfo *mode);
void adf_modeinfo_set_vrefresh(struct drm_mode_modeinfo *mode);

#endif /*              */
