/*
 * Copyright (C) 2012 Texas Instruments Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * Contributors:
 *      Manjunath Hadli <manjunath.hadli@ti.com>
 *      Prabhakar Lad <prabhakar.lad@ti.com>
 */

#ifndef _DAVINCI_VPFE_VIDEO_H
#define _DAVINCI_VPFE_VIDEO_H

#include <media/videobuf2-dma-contig.h>

struct vpfe_device;

/*
                                                       
                                                                          
                                             
 */
struct vpfe_video_operations {
	int(*queue) (struct vpfe_device *vpfe_dev, unsigned long addr);
};

enum vpfe_pipeline_stream_state {
	VPFE_PIPELINE_STREAM_STOPPED = 0,
	VPFE_PIPELINE_STREAM_CONTINUOUS = 1,
	VPFE_PIPELINE_STREAM_SINGLESHOT = 2,
};

enum vpfe_video_state {
	/*                                     */
	VPFE_VIDEO_BUFFER_NOT_QUEUED = 0,
	/*                                 */
	VPFE_VIDEO_BUFFER_QUEUED = 1,
};

struct vpfe_pipeline {
	/*                */
	struct media_pipeline		*pipe;
	/*                                   
                          
  */
	enum vpfe_pipeline_stream_state	state;
	/*                                       */
	unsigned int			input_num;
	/*                                        */
	unsigned int			output_num;
	/*                                               */
	struct vpfe_video_device	*inputs[10];
	/*                       */
	struct vpfe_video_device	*outputs[10];
};

#define to_vpfe_pipeline(__e) \
	container_of((__e)->pipe, struct vpfe_pipeline, pipe)

#define to_vpfe_video(vdev) \
	container_of(vdev, struct vpfe_video_device, video_dev)

struct vpfe_cap_buffer {
	struct vb2_buffer vb;
	struct list_head list;
};

struct vpfe_video_device {
	/*             */
	struct vpfe_device			*vpfe_dev;
	/*           */
	struct video_device			video_dev;
	/*                           */
	struct media_pad			pad;
	/*                                            */
	const struct vpfe_video_operations	*ops;
	/*                                        */
	enum v4l2_buf_type			type;
	/*                                                   */
	u32					field_id;
	/*                                            */
	struct vpfe_pipeline			pipe;
	/*                                     */
	u8					started;
	/*                               */
	unsigned int				state;
	/*                                 */
	int					current_input;
	/*
                                                               
                     
  */
	enum v4l2_memory			memory;
	/*                                             */
	struct v4l2_prio_state			prio;
	/*                                         */
	u32					usrs;
	/*                                                 */
	u8					initialized;
	/*                  */
	u8					skip_frame_count;
	/*                             */
	u8					skip_frame_count_init;
	/*                             */
	struct v4l2_fract			timeperframe;
	/*                                      */
	struct vpfe_ext_subdev_info		*current_ext_subdev;
	/*                                             */
	struct vpfe_cap_buffer			*cur_frm;
	/*                                          */
	struct vpfe_cap_buffer			*next_frm;
	/*                            */
	struct v4l2_format			fmt;
	struct vb2_queue			buffer_queue;
	/*                                            */
	struct vb2_alloc_ctx *alloc_ctx;
	/*                        */
	struct list_head			dma_queue;
	spinlock_t				irqlock;
	/*                        */
	spinlock_t				dma_queue_lock;
	/*                                    */
	struct mutex				lock;
	/*                               */
	u32					io_usrs;
	/*                                        */
	v4l2_std_id				stdid;
	/*
                                                             
                                            
  */
	u32					field_off;
};

int vpfe_video_is_pipe_ready(struct vpfe_pipeline *pipe);
void vpfe_video_unregister(struct vpfe_video_device *video);
int vpfe_video_register(struct vpfe_video_device *video,
			struct v4l2_device *vdev);
int vpfe_video_init(struct vpfe_video_device *video, const char *name);
void vpfe_video_process_buffer_complete(struct vpfe_video_device *video);
void vpfe_video_schedule_bottom_field(struct vpfe_video_device *video);
void vpfe_video_schedule_next_buffer(struct vpfe_video_device *video);

#endif		/*                       */
