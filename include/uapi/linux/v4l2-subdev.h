/*
 * V4L2 subdev userspace API
 *
 * Copyright (C) 2010 Nokia Corporation
 *
 * Contacts: Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 *	     Sakari Ailus <sakari.ailus@iki.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __LINUX_V4L2_SUBDEV_H
#define __LINUX_V4L2_SUBDEV_H

#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/v4l2-common.h>
#include <linux/v4l2-mediabus.h>

/* 
                                                         
                                                            
                                                                   
 */
enum v4l2_subdev_format_whence {
	V4L2_SUBDEV_FORMAT_TRY = 0,
	V4L2_SUBDEV_FORMAT_ACTIVE = 1,
};

/* 
                                                         
                                                            
                                                 
                                                         
 */
struct v4l2_subdev_format {
	__u32 which;
	__u32 pad;
	struct v4l2_mbus_framefmt format;
	__u32 reserved[8];
};

/* 
                                                    
                                                            
                                                 
                                       
 */
struct v4l2_subdev_crop {
	__u32 which;
	__u32 pad;
	struct v4l2_rect rect;
	__u32 reserved[8];
};

/* 
                                                                   
                                                 
                                          
                                                     
 */
struct v4l2_subdev_mbus_code_enum {
	__u32 pad;
	__u32 index;
	__u32 code;
	__u32 reserved[9];
};

/* 
                                                                    
                                                 
                                          
                                                     
 */
struct v4l2_subdev_frame_size_enum {
	__u32 index;
	__u32 pad;
	__u32 code;
	__u32 min_width;
	__u32 max_width;
	__u32 min_height;
	__u32 max_height;
	__u32 reserved[9];
};

/* 
                                                           
                                                 
                                       
 */
struct v4l2_subdev_frame_interval {
	__u32 pad;
	struct v4l2_fract interval;
	__u32 reserved[9];
};

/* 
                                                                      
                                                 
                                                  
                                                     
                                
                                  
                                       
 */
struct v4l2_subdev_frame_interval_enum {
	__u32 index;
	__u32 pad;
	__u32 code;
	__u32 width;
	__u32 height;
	struct v4l2_fract interval;
	__u32 reserved[9];
};

/* 
                                                
  
                                                                     
                                                 
                                                                        
                                                 
                                                                       
                                          
                                                 
  
                                                                      
                                                                 
                               
 */
struct v4l2_subdev_selection {
	__u32 which;
	__u32 pad;
	__u32 target;
	__u32 flags;
	struct v4l2_rect r;
	__u32 reserved[8];
};

struct v4l2_subdev_edid {
	__u32 pad;
	__u32 start_block;
	__u32 blocks;
	__u32 reserved[5];
	__u8 __user *edid;
};

#define VIDIOC_SUBDEV_G_FMT	_IOWR('V',  4, struct v4l2_subdev_format)
#define VIDIOC_SUBDEV_S_FMT	_IOWR('V',  5, struct v4l2_subdev_format)
#define VIDIOC_SUBDEV_G_FRAME_INTERVAL \
			_IOWR('V', 21, struct v4l2_subdev_frame_interval)
#define VIDIOC_SUBDEV_S_FRAME_INTERVAL \
			_IOWR('V', 22, struct v4l2_subdev_frame_interval)
#define VIDIOC_SUBDEV_ENUM_MBUS_CODE \
			_IOWR('V',  2, struct v4l2_subdev_mbus_code_enum)
#define VIDIOC_SUBDEV_ENUM_FRAME_SIZE \
			_IOWR('V', 74, struct v4l2_subdev_frame_size_enum)
#define VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL \
			_IOWR('V', 75, struct v4l2_subdev_frame_interval_enum)
#define VIDIOC_SUBDEV_G_CROP	_IOWR('V', 59, struct v4l2_subdev_crop)
#define VIDIOC_SUBDEV_S_CROP	_IOWR('V', 60, struct v4l2_subdev_crop)
#define VIDIOC_SUBDEV_G_SELECTION \
	_IOWR('V', 61, struct v4l2_subdev_selection)
#define VIDIOC_SUBDEV_S_SELECTION \
	_IOWR('V', 62, struct v4l2_subdev_selection)
#define VIDIOC_SUBDEV_G_EDID	_IOWR('V', 40, struct v4l2_subdev_edid)
#define VIDIOC_SUBDEV_S_EDID	_IOWR('V', 41, struct v4l2_subdev_edid)

#endif
