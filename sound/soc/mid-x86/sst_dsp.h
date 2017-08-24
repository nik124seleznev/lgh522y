#ifndef __SST_DSP_H__
#define __SST_DSP_H__
/*
 *  sst_dsp.h - Intel SST Driver for audio engine
 *
 *  Copyright (C) 2008-12 Intel Corporation
 *  Authors:	Vinod Koul <vinod.koul@linux.intel.com>
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

enum sst_codec_types {
	/*                                     */
	SST_CODEC_TYPE_UNKNOWN = 0,
	SST_CODEC_TYPE_PCM,	/*                          */
	SST_CODEC_TYPE_MP3,
	SST_CODEC_TYPE_MP24,
	SST_CODEC_TYPE_AAC,
	SST_CODEC_TYPE_AACP,
	SST_CODEC_TYPE_eAACP,
};

enum stream_type {
	SST_STREAM_TYPE_NONE = 0,
	SST_STREAM_TYPE_MUSIC = 1,
};

struct snd_pcm_params {
	u16 codec;	/*            */
	u8 num_chan;	/*                  */
	u8 pcm_wd_sz;	/*            */
	u32 reserved;	/*                            */
	u32 sfreq;	/*                     */
	u8 use_offload_path;
	u8 reserved2;
	u16 reserved3;
	u8 channel_map[8];
} __packed;

/*                              */
struct snd_mp3_params {
	u16 codec;
	u8  num_chan;	/*                  */
	u8  pcm_wd_sz; /*            */
	u8  crc_check; /*                                       */
	u8  reserved1; /*       */
	u16 reserved2;	/*        */
} __packed;

#define AAC_BIT_STREAM_ADTS		0
#define AAC_BIT_STREAM_ADIF		1
#define AAC_BIT_STREAM_RAW		2

/*                              */
struct snd_aac_params {
	u16 codec;
	u8 num_chan; /*                 */
	u8 pcm_wd_sz; /*            */
	u8 bdownsample; /*                                                  */
	u8 bs_format; /*                                               */
	u16  reser2;
	u32 externalsr; /*                                         */
	u8 sbr_signalling;/*                                             */
	u8 reser1;
	u16  reser3;
} __packed;

/*                              */
struct snd_wma_params {
	u16 codec;
	u8  num_chan;	/*                  */
	u8  pcm_wd_sz;	/*            */
	u32 brate;	/*                           */
	u32 sfreq;	/*                                       */
	u32 channel_mask;  /*              */
	u16 format_tag;	/*            */
	u16 block_align;	/*             */
	u16 wma_encode_opt;/*                */
	u8 op_align;	/*                                   */
	u8 reserved;	/*          */
} __packed;

/*                       */
union  snd_sst_codec_params {
	struct snd_pcm_params pcm_params;
	struct snd_mp3_params mp3_params;
	struct snd_aac_params aac_params;
	struct snd_wma_params wma_params;
} __packed;

/*                                         */
struct sst_address_info {
	u32 addr; /*               */
	u32 size; /*                    */
};

struct snd_sst_alloc_params_ext {
	struct sst_address_info  ring_buf_info[8];
	u8 sg_count;
	u8 reserved;
	u16 reserved2;
	u32 frag_size;	/*                                            
                                             */
} __packed;

struct snd_sst_stream_params {
	union snd_sst_codec_params uc;
} __packed;

struct snd_sst_params {
	u32 stream_id;
	u8 codec;
	u8 ops;
	u8 stream_type;
	u8 device_type;
	struct snd_sst_stream_params sparams;
	struct snd_sst_alloc_params_ext aparams;
};

#endif /*               */
