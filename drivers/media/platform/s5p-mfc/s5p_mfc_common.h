/*
 * Samsung S5P Multi Format Codec v 5.0
 *
 * This file contains definitions of enums and structs used by the codec
 * driver.
 *
 * Copyright (C) 2011 Samsung Electronics Co., Ltd.
 * Kamil Debski, <k.debski@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version
 */

#ifndef S5P_MFC_COMMON_H_
#define S5P_MFC_COMMON_H_

#include <linux/platform_device.h>
#include <linux/videodev2.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-core.h>
#include "regs-mfc.h"
#include "regs-mfc-v6.h"

/*                                   */

/*                                                             
                 */
#define DST_QUEUE_OFF_BASE      (TASK_SIZE / 2)

#define MFC_BANK1_ALLOC_CTX	0
#define MFC_BANK2_ALLOC_CTX	1

#define MFC_BANK1_ALIGN_ORDER	13
#define MFC_BANK2_ALIGN_ORDER	13
#define MFC_BASE_ALIGN_ORDER	17

#include <media/videobuf2-dma-contig.h>

static inline dma_addr_t s5p_mfc_mem_cookie(void *a, void *b)
{
	/*                                                      */
	dma_addr_t *paddr = vb2_dma_contig_memops.cookie(b);

	return *paddr;
}

/*                 */
#define MFC_MAX_EXTRA_DPB       5
#define MFC_MAX_BUFFERS		32
#define MFC_NUM_CONTEXTS	4
/*                   */
#define MFC_INT_TIMEOUT		2000
/*                   */
#define MFC_BW_TIMEOUT		500
/*                   */
#define MFC_WATCHDOG_INTERVAL   1000
/*                                                          */
#define MFC_WATCHDOG_CNT        10
#define MFC_NO_INSTANCE_SET	-1
#define MFC_ENC_CAP_PLANE_COUNT	1
#define MFC_ENC_OUT_PLANE_COUNT	2
#define STUFF_BYTE		4
#define MFC_MAX_CTRLS		70

#define S5P_MFC_CODEC_NONE		-1
#define S5P_MFC_CODEC_H264_DEC		0
#define S5P_MFC_CODEC_H264_MVC_DEC	1
#define S5P_MFC_CODEC_VC1_DEC		2
#define S5P_MFC_CODEC_MPEG4_DEC		3
#define S5P_MFC_CODEC_MPEG2_DEC		4
#define S5P_MFC_CODEC_H263_DEC		5
#define S5P_MFC_CODEC_VC1RCV_DEC	6
#define S5P_MFC_CODEC_VP8_DEC		7

#define S5P_MFC_CODEC_H264_ENC		20
#define S5P_MFC_CODEC_H264_MVC_ENC	21
#define S5P_MFC_CODEC_MPEG4_ENC		22
#define S5P_MFC_CODEC_H263_ENC		23

#define S5P_MFC_R2H_CMD_EMPTY			0
#define S5P_MFC_R2H_CMD_SYS_INIT_RET		1
#define S5P_MFC_R2H_CMD_OPEN_INSTANCE_RET	2
#define S5P_MFC_R2H_CMD_SEQ_DONE_RET		3
#define S5P_MFC_R2H_CMD_INIT_BUFFERS_RET	4
#define S5P_MFC_R2H_CMD_CLOSE_INSTANCE_RET	6
#define S5P_MFC_R2H_CMD_SLEEP_RET		7
#define S5P_MFC_R2H_CMD_WAKEUP_RET		8
#define S5P_MFC_R2H_CMD_COMPLETE_SEQ_RET	9
#define S5P_MFC_R2H_CMD_DPB_FLUSH_RET		10
#define S5P_MFC_R2H_CMD_NAL_ABORT_RET		11
#define S5P_MFC_R2H_CMD_FW_STATUS_RET		12
#define S5P_MFC_R2H_CMD_FRAME_DONE_RET		13
#define S5P_MFC_R2H_CMD_FIELD_DONE_RET		14
#define S5P_MFC_R2H_CMD_SLICE_DONE_RET		15
#define S5P_MFC_R2H_CMD_ENC_BUFFER_FUL_RET	16
#define S5P_MFC_R2H_CMD_ERR_RET			32

#define mfc_read(dev, offset)		readl(dev->regs_base + (offset))
#define mfc_write(dev, data, offset)	writel((data), dev->regs_base + \
								(offset))

/* 
                                                  
 */
enum s5p_mfc_fmt_type {
	MFC_FMT_DEC,
	MFC_FMT_ENC,
	MFC_FMT_RAW,
};

/* 
                                                        
 */
enum s5p_mfc_inst_type {
	MFCINST_INVALID,
	MFCINST_DECODER,
	MFCINST_ENCODER,
};

/* 
                                                          
 */
enum s5p_mfc_inst_state {
	MFCINST_FREE = 0,
	MFCINST_INIT = 100,
	MFCINST_GOT_INST,
	MFCINST_HEAD_PARSED,
	MFCINST_HEAD_PRODUCED,
	MFCINST_BUFS_SET,
	MFCINST_RUNNING,
	MFCINST_FINISHING,
	MFCINST_FINISHED,
	MFCINST_RETURN_INST,
	MFCINST_ERROR,
	MFCINST_ABORT,
	MFCINST_FLUSH,
	MFCINST_RES_CHANGE_INIT,
	MFCINST_RES_CHANGE_FLUSH,
	MFCINST_RES_CHANGE_END,
};

/* 
                                                        
 */
enum s5p_mfc_queue_state {
	QUEUE_FREE,
	QUEUE_BUFS_REQUESTED,
	QUEUE_BUFS_QUERIED,
	QUEUE_BUFS_MMAPED,
};

/* 
                                                   
 */
enum s5p_mfc_decode_arg {
	MFC_DEC_FRAME,
	MFC_DEC_LAST_FRAME,
	MFC_DEC_RES_CHANGE,
};

#define MFC_BUF_FLAG_USED	(1 << 0)
#define MFC_BUF_FLAG_EOS	(1 << 1)

struct s5p_mfc_ctx;

/* 
                                  
 */
struct s5p_mfc_buf {
	struct list_head list;
	struct vb2_buffer *b;
	union {
		struct {
			size_t luma;
			size_t chroma;
		} raw;
		size_t stream;
	} cookie;
	int flags;
};

/* 
                                                      
 */
struct s5p_mfc_pm {
	struct clk	*clock;
	struct clk	*clock_gate;
	atomic_t	power;
	struct device	*device;
};

struct s5p_mfc_buf_size_v5 {
	unsigned int h264_ctx;
	unsigned int non_h264_ctx;
	unsigned int dsc;
	unsigned int shm;
};

struct s5p_mfc_buf_size_v6 {
	unsigned int dev_ctx;
	unsigned int h264_dec_ctx;
	unsigned int other_dec_ctx;
	unsigned int h264_enc_ctx;
	unsigned int other_enc_ctx;
};

struct s5p_mfc_buf_size {
	unsigned int fw;
	unsigned int cpb;
	void *priv;
};

struct s5p_mfc_buf_align {
	unsigned int base;
};

struct s5p_mfc_variant {
	unsigned int version;
	unsigned int port_num;
	struct s5p_mfc_buf_size *buf_size;
	struct s5p_mfc_buf_align *buf_align;
	char	*fw_name;
};

/* 
                                                            
                                                       
                          
                                                     
                                                      
                              
                                                          
                             
 */
struct s5p_mfc_priv_buf {
	void		*alloc;
	unsigned long	ofs;
	void		*virt;
	dma_addr_t	dma;
	size_t		size;
};

/* 
                                                                         
  
                          
                                       
                                       
                              
                                                        
                                                         
                                                    
                      
                                                            
                                                            
                                  
                                              
                                             
                                                     
                                                                     
              
                                     
                                             
                                     
                                             
                                                                   
                              
                                          
                                                     
                                                     
                                       
                                  
                                                      
                                                                       
                                          
                                                  
                                          
                                                                 
                                                 
                                           
                                                              
                                                                  
                                                                  
  
 */
struct s5p_mfc_dev {
	struct v4l2_device	v4l2_dev;
	struct video_device	*vfd_dec;
	struct video_device	*vfd_enc;
	struct platform_device	*plat_dev;
	struct device		*mem_dev_l;
	struct device		*mem_dev_r;
	void __iomem		*regs_base;
	int			irq;
	struct v4l2_ctrl_handler dec_ctrl_handler;
	struct v4l2_ctrl_handler enc_ctrl_handler;
	struct s5p_mfc_pm	pm;
	struct s5p_mfc_variant	*variant;
	int num_inst;
	spinlock_t irqlock;	/*                                         */
	spinlock_t condlock;	/*                                            
                           */
	struct mutex mfc_mutex; /*                   */
	int int_cond;
	int int_type;
	unsigned int int_err;
	wait_queue_head_t queue;
	size_t fw_size;
	void *fw_virt_addr;
	dma_addr_t bank1;
	dma_addr_t bank2;
	unsigned long hw_lock;
	struct s5p_mfc_ctx *ctx[MFC_NUM_CONTEXTS];
	int curr_ctx;
	unsigned long ctx_work_bits;
	atomic_t watchdog_cnt;
	struct timer_list watchdog_timer;
	struct workqueue_struct *watchdog_workqueue;
	struct work_struct watchdog_work;
	void *alloc_ctx[2];
	unsigned long enter_suspend;

	struct s5p_mfc_priv_buf ctx_buf;
	int warn_start;
	struct s5p_mfc_hw_ops *mfc_ops;
	struct s5p_mfc_hw_cmds *mfc_cmds;
};

/* 
                                                                
 */
struct s5p_mfc_h264_enc_params {
	enum v4l2_mpeg_video_h264_profile profile;
	enum v4l2_mpeg_video_h264_loop_filter_mode loop_filter_mode;
	s8 loop_filter_alpha;
	s8 loop_filter_beta;
	enum v4l2_mpeg_video_h264_entropy_mode entropy_mode;
	u8 max_ref_pic;
	u8 num_ref_pic_4p;
	int _8x8_transform;
	int rc_mb_dark;
	int rc_mb_smooth;
	int rc_mb_static;
	int rc_mb_activity;
	int vui_sar;
	u8 vui_sar_idc;
	u16 vui_ext_sar_width;
	u16 vui_ext_sar_height;
	int open_gop;
	u16 open_gop_size;
	u8 rc_frame_qp;
	u8 rc_min_qp;
	u8 rc_max_qp;
	u8 rc_p_frame_qp;
	u8 rc_b_frame_qp;
	enum v4l2_mpeg_video_h264_level level_v4l2;
	int level;
	u16 cpb_size;
	int interlace;
	u8 hier_qp;
	u8 hier_qp_type;
	u8 hier_qp_layer;
	u8 hier_qp_layer_qp[7];
	u8 sei_frame_packing;
	u8 sei_fp_curr_frame_0;
	u8 sei_fp_arrangement_type;

	u8 fmo;
	u8 fmo_map_type;
	u8 fmo_slice_grp;
	u8 fmo_chg_dir;
	u32 fmo_chg_rate;
	u32 fmo_run_len[4];
	u8 aso;
	u32 aso_slice_order[8];
};

/* 
                                                                           
 */
struct s5p_mfc_mpeg4_enc_params {
	/*            */
	enum v4l2_mpeg_video_mpeg4_profile profile;
	int quarter_pixel;
	/*                        */
	u16 vop_time_res;
	u16 vop_frm_delta;
	u8 rc_frame_qp;
	u8 rc_min_qp;
	u8 rc_max_qp;
	u8 rc_p_frame_qp;
	u8 rc_b_frame_qp;
	enum v4l2_mpeg_video_mpeg4_level level_v4l2;
	int level;
};

/* 
                                                          
 */
struct s5p_mfc_enc_params {
	u16 width;
	u16 height;

	u16 gop_size;
	enum v4l2_mpeg_video_multi_slice_mode slice_mode;
	u16 slice_mb;
	u32 slice_bit;
	u16 intra_refresh_mb;
	int pad;
	u8 pad_luma;
	u8 pad_cb;
	u8 pad_cr;
	int rc_frame;
	int rc_mb;
	u32 rc_bitrate;
	u16 rc_reaction_coeff;
	u16 vbv_size;
	u32 vbv_delay;

	enum v4l2_mpeg_video_header_mode seq_hdr_mode;
	enum v4l2_mpeg_mfc51_video_frame_skip_mode frame_skip_mode;
	int fixed_target_bit;

	u8 num_b_frame;
	u32 rc_framerate_num;
	u32 rc_framerate_denom;

	struct {
		struct s5p_mfc_h264_enc_params h264;
		struct s5p_mfc_mpeg4_enc_params mpeg4;
	} codec;

};

/* 
                                                         
 */
struct s5p_mfc_codec_ops {
	/*                         */
	int (*pre_seq_start) (struct s5p_mfc_ctx *ctx);
	int (*post_seq_start) (struct s5p_mfc_ctx *ctx);
	/*                    */
	int (*pre_frame_start) (struct s5p_mfc_ctx *ctx);
	int (*post_frame_start) (struct s5p_mfc_ctx *ctx);
};

#define call_cop(c, op, args...)				\
	(((c)->c_ops->op) ?					\
		((c)->c_ops->op(args)) : 0)

/* 
                                                                 
  
                                                  
                        
                                                             
                                             
                                         
                                                                
                                                                  
           
                                            
                                                 
                                         
                                              
                                                        
                                                             
                                                                        
                                                                      
                                                    
                                
                                                               
                                                             
                                                               
                                                       
                                                         
                                    
                                       
                                             
                                                                        
                    
                                                                      
            
                                                                        
                      
                                                                 
                  
                                                                 
                  
                                                     
                                                   
                                                      
                                                           
                                                       
                                                          
                                                                       
                                                    
                                         
                                                 
                                                      
                                                            
                                                                
                     
                                                                         
                                                           
                                                                 
                                 
                                    
                                       
                                          
                                                          
                                            
                                                            
                                           
                                               
                                                        
                                                                    
                                                                 
                                                          
                                                              
                            
                                                               
                           
                                            
 */
struct s5p_mfc_ctx {
	struct s5p_mfc_dev *dev;
	struct v4l2_fh fh;

	int num;

	int int_cond;
	int int_type;
	unsigned int int_err;
	wait_queue_head_t queue;

	struct s5p_mfc_fmt *src_fmt;
	struct s5p_mfc_fmt *dst_fmt;

	struct vb2_queue vq_src;
	struct vb2_queue vq_dst;

	struct list_head src_queue;
	struct list_head dst_queue;

	unsigned int src_queue_cnt;
	unsigned int dst_queue_cnt;

	enum s5p_mfc_inst_type type;
	enum s5p_mfc_inst_state state;
	int inst_no;

	/*                  */
	int img_width;
	int img_height;
	int buf_width;
	int buf_height;

	int luma_size;
	int chroma_size;
	int mv_size;

	unsigned long consumed_stream;

	unsigned int dpb_flush_flag;
	unsigned int head_processed;

	struct s5p_mfc_priv_buf bank1;
	struct s5p_mfc_priv_buf bank2;

	enum s5p_mfc_queue_state capture_state;
	enum s5p_mfc_queue_state output_state;

	struct s5p_mfc_buf src_bufs[MFC_MAX_BUFFERS];
	int src_bufs_cnt;
	struct s5p_mfc_buf dst_bufs[MFC_MAX_BUFFERS];
	int dst_bufs_cnt;

	unsigned int sequence;
	unsigned long dec_dst_flag;
	size_t dec_src_buf_size;

	/*                */
	int codec_mode;
	int slice_interface;
	int loop_filter_mpeg4;
	int display_delay;
	int display_delay_enable;
	int after_packed_pb;
	int sei_fp_parse;

	int pb_count;
	int total_dpb_count;
	int mv_count;
	/*         */
	struct s5p_mfc_priv_buf ctx;
	struct s5p_mfc_priv_buf dsc;
	struct s5p_mfc_priv_buf shm;

	struct s5p_mfc_enc_params enc_params;

	size_t enc_dst_buf_size;
	size_t luma_dpb_size;
	size_t chroma_dpb_size;
	size_t me_buffer_size;
	size_t tmv_buffer_size;

	enum v4l2_mpeg_mfc51_video_force_frame_type force_frame_type;

	struct list_head ref_queue;
	unsigned int ref_queue_cnt;

	enum v4l2_mpeg_video_multi_slice_mode slice_mode;
	union {
		unsigned int mb;
		unsigned int bits;
	} slice_size;

	struct s5p_mfc_codec_ops *c_ops;

	struct v4l2_ctrl *ctrls[MFC_MAX_CTRLS];
	struct v4l2_ctrl_handler ctrl_handler;
	unsigned int frame_tag;
	size_t scratch_buf_size;
};

/*
                                                                              
                    
 */
struct s5p_mfc_fmt {
	char *name;
	u32 fourcc;
	u32 codec_mode;
	enum s5p_mfc_fmt_type type;
	u32 num_planes;
};

/* 
                                                                              
                                                    
 */
struct mfc_control {
	__u32			id;
	enum v4l2_ctrl_type	type;
	__u8			name[32];  /*          */
	__s32			minimum;   /*                 */
	__s32			maximum;
	__s32			step;
	__u32			menu_skip_mask;
	__s32			default_value;
	__u32			flags;
	__u32			reserved[2];
	__u8			is_volatile;
};

/*                                          */
#define s5p_mfc_hw_call(f, op, args...) \
	((f && f->op) ? f->op(args) : -ENODEV)

#define fh_to_ctx(__fh) container_of(__fh, struct s5p_mfc_ctx, fh)
#define ctrl_to_ctx(__ctrl) \
	container_of((__ctrl)->handler, struct s5p_mfc_ctx, ctrl_handler)

void clear_work_bit(struct s5p_mfc_ctx *ctx);
void set_work_bit(struct s5p_mfc_ctx *ctx);
void clear_work_bit_irqsave(struct s5p_mfc_ctx *ctx);
void set_work_bit_irqsave(struct s5p_mfc_ctx *ctx);

#define HAS_PORTNUM(dev)	(dev ? (dev->variant ? \
				(dev->variant->port_num ? 1 : 0) : 0) : 0)
#define IS_TWOPORT(dev)		(dev->variant->port_num == 2 ? 1 : 0)
#define IS_MFCV6(dev)		(dev->variant->version >= 0x60 ? 1 : 0)

#endif /*                   */
