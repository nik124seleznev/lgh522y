/*
 * Samsung EXYNOS4x12 FIMC-IS (Imaging Subsystem) driver
 *
 * Copyright (C) 2013 Samsung Electronics Co., Ltd.
 *
 * Authors: Sylwester Nawrocki <s.nawrocki@samsung.com>
 *          Younghwan Joo <yhwan.joo@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef FIMC_ISP_H_
#define FIMC_ISP_H_

#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/videodev2.h>

#include <media/media-entity.h>
#include <media/videobuf2-core.h>
#include <media/v4l2-device.h>
#include <media/v4l2-mediabus.h>
#include <media/s5p_fimc.h>

/*                                  */
#define FIMC_ISP_SINK_WIDTH_MIN		(16 + 8)
#define FIMC_ISP_SINK_HEIGHT_MIN	(12 + 8)
#define FIMC_ISP_SOURCE_WIDTH_MIN	8
#define FIMC_ISP_SOURC_HEIGHT_MIN	8
#define FIMC_ISP_CAC_MARGIN_WIDTH	16
#define FIMC_ISP_CAC_MARGIN_HEIGHT	12

#define FIMC_ISP_SINK_WIDTH_MAX		(4000 - 16)
#define FIMC_ISP_SINK_HEIGHT_MAX	(4000 + 12)
#define FIMC_ISP_SOURCE_WIDTH_MAX	4000
#define FIMC_ISP_SOURC_HEIGHT_MAX	4000

#define FIMC_ISP_NUM_FORMATS		3
#define FIMC_ISP_REQ_BUFS_MIN		2

#define FIMC_ISP_SD_PAD_SINK		0
#define FIMC_ISP_SD_PAD_SRC_FIFO	1
#define FIMC_ISP_SD_PAD_SRC_DMA		2
#define FIMC_ISP_SD_PADS_NUM		3
#define FIMC_ISP_MAX_PLANES		1

/* 
                                                         
                           
                             
                                    
 */
struct fimc_isp_frame {
	u16 width;
	u16 height;
	struct v4l2_rect rect;
};

struct fimc_isp_ctrls {
	struct v4l2_ctrl_handler handler;

	/*                    */
	struct v4l2_ctrl *auto_wb;
	/*                          */
	struct {
		struct v4l2_ctrl *auto_iso;
		struct v4l2_ctrl *iso;
	};
	/*                   */
	struct v4l2_ctrl *contrast;
	/*                     */
	struct v4l2_ctrl *saturation;
	/*                    */
	struct v4l2_ctrl *sharpness;
	/*                     */
	struct v4l2_ctrl *brightness;
	/*              */
	struct v4l2_ctrl *hue;

	/*                      */
	struct v4l2_ctrl *auto_exp;
	/*                       */
	struct v4l2_ctrl *exposure;
	/*                    */
	struct v4l2_ctrl *aewb_lock;
	/*                        */
	struct v4l2_ctrl *exp_metering;
	/*     */
	struct v4l2_ctrl *afc;
	/*                  */
	struct v4l2_ctrl *colorfx;
};

/* 
                                                        
                                
                                            
                                      
                                             
                                                               
                              
                                                                   
                                                         
                                                                 
                                
 */
struct fimc_is_video {
	struct video_device	vdev;
	enum v4l2_buf_type	type;
	struct media_pad	pad;
	struct list_head	pending_buf_q;
	struct list_head	active_buf_q;
	struct vb2_queue	vb_queue;
	unsigned int		frame_count;
	unsigned int		reqbufs_count;
	int			streaming;
	unsigned long		payload[FIMC_ISP_MAX_PLANES];
	const struct fimc_fmt	*format;
};

/* 
                                               
                                            
                                                 
                           
                                          
                                       
                                       
                                                   
                                                                        
                                                      
                                              
                                           
                                         
                                                          
                                                 
                                               
                             
                                                     
 */
struct fimc_isp {
	struct platform_device		*pdev;
	struct vb2_alloc_ctx		*alloc_ctx;
	struct v4l2_subdev		subdev;
	struct media_pad		subdev_pads[FIMC_ISP_SD_PADS_NUM];
	struct v4l2_mbus_framefmt	subdev_fmt;
	struct v4l2_ctrl		*test_pattern;
	struct fimc_isp_ctrls		ctrls;

	struct mutex			video_lock;
	struct mutex			subdev_lock;

	struct fimc_isp_frame		inp_frame;
	struct fimc_isp_frame		out_frame;
	unsigned int			source_subdev_grp_id;

	unsigned int			cac_margin_x;
	unsigned int			cac_margin_y;

	unsigned long			state;

	struct fimc_is_video		video_capture;
};

#define ctrl_to_fimc_isp(_ctrl) \
	container_of(ctrl->handler, struct fimc_isp, ctrls.handler)

struct fimc_is;

int fimc_isp_subdev_create(struct fimc_isp *isp);
void fimc_isp_subdev_destroy(struct fimc_isp *isp);
void fimc_isp_irq_handler(struct fimc_is *is);
int fimc_is_create_controls(struct fimc_isp *isp);
int fimc_is_delete_controls(struct fimc_isp *isp);
const struct fimc_fmt *fimc_isp_find_format(const u32 *pixelformat,
					const u32 *mbus_code, int index);
#endif /*             */
