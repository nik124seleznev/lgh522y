/*
 * drivers/media/platform/samsung/mfc5/s5p_mfc_opr_v5.h
 *
 * Header file for Samsung MFC (Multi Function Codec - FIMV) driver
 * Contains declarations of hw related functions.
 *
 * Kamil Debski, Copyright (C) 2011 Samsung Electronics
 * http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef S5P_MFC_OPR_V5_H_
#define S5P_MFC_OPR_V5_H_

#include "s5p_mfc_common.h"
#include "s5p_mfc_opr.h"

enum MFC_SHM_OFS {
	EXTENEDED_DECODE_STATUS	= 0x00,	/*   */
	SET_FRAME_TAG		= 0x04, /*   */
	GET_FRAME_TAG_TOP	= 0x08, /*   */
	GET_FRAME_TAG_BOT	= 0x0C, /*   */
	PIC_TIME_TOP		= 0x10, /*   */
	PIC_TIME_BOT		= 0x14, /*   */
	START_BYTE_NUM		= 0x18, /*   */

	CROP_INFO_H		= 0x20, /*   */
	CROP_INFO_V		= 0x24, /*   */
	EXT_ENC_CONTROL		= 0x28,	/*   */
	ENC_PARAM_CHANGE	= 0x2C,	/*   */
	RC_VOP_TIMING		= 0x30,	/*          */
	HEC_PERIOD		= 0x34,	/*          */
	METADATA_ENABLE		= 0x38, /*   */
	METADATA_STATUS		= 0x3C, /*   */
	METADATA_DISPLAY_INDEX	= 0x40,	/*   */
	EXT_METADATA_START_ADDR	= 0x44, /*   */
	PUT_EXTRADATA		= 0x48, /*   */
	EXTRADATA_ADDR		= 0x4C, /*   */

	ALLOC_LUMA_DPB_SIZE	= 0x64,	/*   */
	ALLOC_CHROMA_DPB_SIZE	= 0x68,	/*   */
	ALLOC_MV_SIZE		= 0x6C,	/*   */
	P_B_FRAME_QP		= 0x70,	/*   */
	SAMPLE_ASPECT_RATIO_IDC	= 0x74, /*                    
                                               */
	EXTENDED_SAR		= 0x78, /*                    
                                               */
	DISP_PIC_PROFILE	= 0x7C, /*   */
	FLUSH_CMD_TYPE		= 0x80, /*   */
	FLUSH_CMD_INBUF1	= 0x84, /*   */
	FLUSH_CMD_INBUF2	= 0x88, /*   */
	FLUSH_CMD_OUTBUF	= 0x8C, /*   */
	NEW_RC_BIT_RATE		= 0x90, /*                                 
                                                    */
	NEW_RC_FRAME_RATE	= 0x94, /*                                   
                                                      */
	NEW_I_PERIOD		= 0x98, /*                                
                                                 */
	H264_I_PERIOD		= 0x9C, /*                    */
	RC_CONTROL_CONFIG	= 0xA0, /*   */
	BATCH_INPUT_ADDR	= 0xA4, /*   */
	BATCH_OUTPUT_ADDR	= 0xA8, /*   */
	BATCH_OUTPUT_SIZE	= 0xAC, /*   */
	MIN_LUMA_DPB_SIZE	= 0xB0, /*   */
	DEVICE_FORMAT_ID	= 0xB4, /*   */
	H264_POC_TYPE		= 0xB8, /*   */
	MIN_CHROMA_DPB_SIZE	= 0xBC, /*   */
	DISP_PIC_FRAME_TYPE	= 0xC0, /*   */
	FREE_LUMA_DPB		= 0xC4, /*              */
	ASPECT_RATIO_INFO	= 0xC8, /*          */
	EXTENDED_PAR		= 0xCC, /*          */
	DBG_HISTORY_INPUT0	= 0xD0, /*   */
	DBG_HISTORY_INPUT1	= 0xD4,	/*   */
	DBG_HISTORY_OUTPUT	= 0xD8,	/*   */
	HIERARCHICAL_P_QP	= 0xE0, /*          */
	FRAME_PACK_SEI_ENABLE	= 0x168, /*   */
	FRAME_PACK_SEI_AVAIL	= 0x16c, /*   */
	FRAME_PACK_SEI_INFO	= 0x17c, /*   */
};

struct s5p_mfc_hw_ops *s5p_mfc_init_hw_ops_v5(void);
#endif /*                */
