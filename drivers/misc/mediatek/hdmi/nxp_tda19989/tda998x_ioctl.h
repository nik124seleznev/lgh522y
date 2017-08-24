/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * Version       Revision: 1.0
 *
 * Date          Date: 27/10/09
 *
 * Brief         API for the TDA1998x HDMI Transmitters
 *
 **/

#include <linux/types.h>

#ifndef __tx_ioctl__
#define __tx_ioctl__

#ifdef __tx_h__

#define TRANS_TYPE 1

#if TRANS_TYPE

#define EXAMPLE_MAX_SVD 30

/*
            
*/
typedef tmSWVersion_t tda_version;
typedef tmPowerState_t tda_power;
typedef tmdlHdmiTxInstanceSetupInfo_t tda_setup;
typedef tmdlHdmiTxCapabilities_t tda_capabilities;
typedef tmdlHdmiTxVideoOutConfig_t tda_video_out;
typedef tmdlHdmiTxVideoInConfig_t tda_video_in;
typedef tmdlHdmiTxSinkType_t tda_sink;
typedef tmdlHdmiTxAudioInConfig_t tda_audio_in;
typedef tmdlHdmiTxEdidAudioDesc_t tda_edid_audio_desc;
typedef tmdlHdmiTxShortVidDesc_t tda_edid_video_desc;
typedef tmdlHdmiTxEvent_t tda_event;
typedef tmdlHdmiTxInstanceSetupInfo_t tda_setup_info;
typedef tmdlHdmiTxEdidVideoTimings_t tda_edid_video_timings;
typedef tmdlHdmiTxPictAspectRatio_t tda_edid_tv_aspect_ratio;
typedef tmdlHdmiTxHdcpCheck_t tda_hdcp_status;
#if defined(TMFL_TDA19989) || defined(TMFL_TDA9984)
typedef tmdlHdmiTxHdcpStatus_t tda_hdcp_fail;
#endif
#ifdef TMFL_TDA19989
typedef tmdlHdmiTxEdidLatency_t tda_edid_latency;
#endif

typedef struct {
	Bool enable;
	tmdlHdmiTxGamutData_t data;
} tda_gammut;

typedef struct {
	Bool enable;
	tmdlHdmiTxVsPktData_t data;
} tda_vs_infoframe;

typedef struct {
	Bool enable;
	tmdlHdmiTxSpdIfData_t data;
} tda_spd_infoframe;

typedef struct {
	Bool enable;
	tmdlHdmiTxMpsIfData_t data;
} tda_mps_infoframe;

typedef struct {
	Bool enable;
	tmdlHdmiTxIsrc1PktData_t data;
} tda_isrc1;

typedef struct {
	Bool enable;
	tmdlHdmiTxIsrc2PktData_t data;
} tda_isrc2;

typedef struct {
	Bool enable;
	tmdlHdmiTxAcpPktData_t data;
} tda_acp;

typedef struct {
	Bool enable;
	tmdlHdmiTxGcpPktData_t data;
} tda_gcp;

typedef struct {
	Bool enable;
	tmdlHdmiTxAviIfData_t data;
} tda_video_infoframe;

typedef struct {
	Bool enable;
	tmdlHdmiTxAudIfData_t data;
} tda_audio_infoframe;

typedef struct {
	tmdlHdmiTxVidFmt_t id;
	tmdlHdmiTxVidFmtSpecs_t spec;
} tda_video_format;

typedef struct {
	tda_video_in video_in;
	tda_video_out video_out;
	tda_audio_in audio_in;	/*                                          */
	tda_sink sink;		/*                                          */
} tda_set_in_out;

typedef struct {
	tda_audio_in audio_in;
	tda_sink sink;
} tda_set_audio_in;

typedef struct {
	tda_edid_audio_desc desc[EXAMPLE_MAX_SVD];
	unsigned int max;
	unsigned int written;
	unsigned char flags;
} tda_edid_audio_caps;

typedef struct {
	tda_edid_video_desc desc[EXAMPLE_MAX_SVD];
	unsigned int max;
	unsigned int written;
	unsigned char flags;
} tda_edid_video_caps;

typedef struct {
	tmdlHdmiTxEdidStatus_t status;
	unsigned char block_count;
} tda_edid;

typedef struct {
	tmdlHdmiTxEdidVideoTimings_t desc[EXAMPLE_MAX_SVD];
	unsigned char max;
	unsigned char written;
} tda_edid_dtd;

typedef struct {
	tmdlHdmiTxEdidFirstMD_t desc1[EXAMPLE_MAX_SVD];
	tmdlHdmiTxEdidSecondMD_t desc2[EXAMPLE_MAX_SVD];
	tmdlHdmiTxEdidOtherMD_t other[EXAMPLE_MAX_SVD];
	unsigned char max;
	unsigned char written;
} tda_edid_md;

#else

#error do not compiled this !

typedef enum {
	TDA_HDCP_ACTIVE = 0,	    /*                                             */
	TDA_HDCP_INACTIVE = 1,	    /*                                               */
	TDA_HPD_ACTIVE = 2,	    /*                                     */
	TDA_HPD_INACTIVE = 3,	    /*                                       */
	TDA_RX_KEYS_RECEIVED = 4,   /*                               */
	TDA_RX_DEVICE_ACTIVE = 5,   /*                                     */
	TDA_RX_DEVICE_INACTIVE = 6, /*                                                 */
	TDA_EDID_RECEIVED = 7,	    /*                          */
	TDA_VS_RPT_RECEIVED = 8,    /*                                  */
#ifdef HDMI_TX_REPEATER_ISR_MODE
	TDA_B_STATUS = 9,	    /*                       */
#endif				/*                           */
	TDA_DEBUG_EVENT_1 = 10	     /*                         */
} tda_event;

typedef struct {
	unsigned char format;	/*                 */
	unsigned char channels;	/*                    */
	unsigned char supportedFreqs;	/*                                  */
	unsigned char supportedRes;	/*                                              */
	unsigned char maxBitrate;	/*                                                           */
} tda_edid_audio_desc;

typedef enum {
	TDA_EDID_READ = 0,		/*                      */
	TDA_EDID_READ_INCOMPLETE = 1,	/*                                                                 */
	TDA_EDID_ERROR_CHK_BLOCK_0 = 2,	/*                          */
	TDA_EDID_ERROR_CHK = 3,		/*                                                          */
	TDA_EDID_NOT_READ = 4,		/*                 */
	TDA_EDID_STATUS_INVALID = 5	/*             */
} tda_edid_status;

typedef struct {
	int HBR;	 /*                             */
	int DST;	 /*                                        */
	int oneBitAudio; /*                               */
} tda_audio_packet;

typedef enum {
	TDA_AFMT_SPDIF = 0,  /*         */
	TDA_AFMT_I2S = 1,    /*       */
	TDA_AFMT_OBA = 2,    /*                       */
	TDA_AFMT_DST = 3,    /*       */
	TDA_AFMT_HBR = 4     /*       */
} tda_audio_format;

typedef enum {
	TDA_AFS_32K = 0,       /*            */
	TDA_AFS_44K = 1,       /*            */
	TDA_AFS_48K = 2,       /*            */
	TDA_AFS_88K = 3,       /*            */
	TDA_AFS_96K = 4,       /*            */
	TDA_AFS_176K = 5,      /*            */
	TDA_AFS_192K = 6       /*            */
} tda_audio_rate;

typedef enum {
	TDA_I2SQ_16BITS = 16,	/*           */
	TDA_I2SQ_32BITS = 32,	/*           */
	TDA_I2SQ_OTHERS = 0	/*                     */
} tda_audio_I2S_qualifier;

typedef enum {
	TDA_I2SFOR_PHILIPS_L = 0,
				/*                       */
	TDA_I2SFOR_OTH_L = 2,	/*                                    */
	TDA_I2SFOR_OTH_R = 3,	/*                                     */
	TDA_I2SFOR_INVALID = 4	/*                  */
} tda_audio_I2S_format;

typedef enum {
	TDA_DSTRATE_SINGLE = 0,	/*                        */
	TDA_DSTRATE_DOUBLE = 1	/*                    */
} tda_dst_rate;

typedef struct {
	int simplayHd;	       /*                            */
	int repeaterEnable;    /*                        */
	unsigned char *pEdidBuffer;	/*                            */
	unsigned long edidBufferSize;  /*                                    */
} tda_instance_setup_info;

typedef enum {
	TDA_VFMT_NULL = 0,		/*                                */
	TDA_VFMT_NO_CHANGE = 0,		/*                                */
	TDA_VFMT_MIN = 1,		/*                                */
	TDA_VFMT_TV_MIN = 1,		/*                                */
	TDA_VFMT_01_640x480p_60Hz = 1,	/*                                */
	TDA_VFMT_02_720x480p_60Hz = 2,	/*                                */
	TDA_VFMT_03_720x480p_60Hz = 3,	/*                                */
	TDA_VFMT_04_1280x720p_60Hz = 4,	/*                                */
	TDA_VFMT_05_1920x1080i_60Hz = 5,/*                                */
	TDA_VFMT_06_720x480i_60Hz = 6,	/*                                */
	TDA_VFMT_07_720x480i_60Hz = 7,	/*                                */
	TDA_VFMT_08_720x240p_60Hz = 8,	/*                                */
	TDA_VFMT_09_720x240p_60Hz = 9,	/*                                */
	TDA_VFMT_10_720x480i_60Hz = 10,	/*                                */
	TDA_VFMT_11_720x480i_60Hz = 11,	/*                                */
	TDA_VFMT_12_720x240p_60Hz = 12,	/*                                */
	TDA_VFMT_13_720x240p_60Hz = 13,	/*                                */
	TDA_VFMT_14_1440x480p_60Hz = 14,/*                                */
	TDA_VFMT_15_1440x480p_60Hz = 15,/*                                */
	TDA_VFMT_16_1920x1080p_60Hz = 16,
					/*                                */
	TDA_VFMT_17_720x576p_50Hz = 17,	/*                                */
	TDA_VFMT_18_720x576p_50Hz = 18,	/*                                */
	TDA_VFMT_19_1280x720p_50Hz = 19,/*                                */
	TDA_VFMT_20_1920x1080i_50Hz = 20,
					/*                                */
	TDA_VFMT_21_720x576i_50Hz = 21,	/*                                */
	TDA_VFMT_22_720x576i_50Hz = 22,	/*                                */
	TDA_VFMT_23_720x288p_50Hz = 23,	/*                                */
	TDA_VFMT_24_720x288p_50Hz = 24,	/*                                */
	TDA_VFMT_25_720x576i_50Hz = 25,	/*                                */
	TDA_VFMT_26_720x576i_50Hz = 26,	/*                                */
	TDA_VFMT_27_720x288p_50Hz = 27,	/*                                */
	TDA_VFMT_28_720x288p_50Hz = 28,	/*                                */
	TDA_VFMT_29_1440x576p_50Hz = 29,/*                                */
	TDA_VFMT_30_1440x576p_50Hz = 30,/*                                */
	TDA_VFMT_31_1920x1080p_50Hz = 31,
					/*                                */
	TDA_VFMT_32_1920x1080p_24Hz = 32,
					/*                                */
	TDA_VFMT_33_1920x1080p_25Hz = 33,
					/*                                */
	TDA_VFMT_34_1920x1080p_30Hz = 34,
					/*                                */
	TDA_VFMT_TV_MAX = 34,		/*                                */
	TDA_VFMT_TV_NO_REG_MIN = 32,	/*                                             */
	TDA_VFMT_TV_NUM = 35,		/*                                */
	TDA_VFMT_PC_MIN = 128,		/*                                */
	TDA_VFMT_PC_640x480p_60Hz = 128,/*                                */
	TDA_VFMT_PC_800x600p_60Hz = 129,/*                                */
	TDA_VFMT_PC_1152x960p_60Hz = 130,
					/*                                */
	TDA_VFMT_PC_1024x768p_60Hz = 131,
					/*                                */
	TDA_VFMT_PC_1280x768p_60Hz = 132,
					/*                                */
	TDA_VFMT_PC_1280x1024p_60Hz = 133,
					/*                                */
	TDA_VFMT_PC_1360x768p_60Hz = 134,
					/*                                */
	TDA_VFMT_PC_1400x1050p_60Hz = 135,
					/*                                */
	TDA_VFMT_PC_1600x1200p_60Hz = 136,
					/*                                */
	TDA_VFMT_PC_1024x768p_70Hz = 137,
					/*                                */
	TDA_VFMT_PC_640x480p_72Hz = 138,/*                                */
	TDA_VFMT_PC_800x600p_72Hz = 139,/*                                */
	TDA_VFMT_PC_640x480p_75Hz = 140,/*                                */
	TDA_VFMT_PC_1024x768p_75Hz = 141,
					/*                                */
	TDA_VFMT_PC_800x600p_75Hz = 142,/*                                */
	TDA_VFMT_PC_1024x864p_75Hz = 143,
					/*                                */
	TDA_VFMT_PC_1280x1024p_75Hz = 144,
					/*                                */
	TDA_VFMT_PC_640x350p_85Hz = 145,/*                                */
	TDA_VFMT_PC_640x400p_85Hz = 146,/*                                */
	TDA_VFMT_PC_720x400p_85Hz = 147,/*                                */
	TDA_VFMT_PC_640x480p_85Hz = 148,/*                                */
	TDA_VFMT_PC_800x600p_85Hz = 149,/*                                */
	TDA_VFMT_PC_1024x768p_85Hz = 150,
					/*                                */
	TDA_VFMT_PC_1152x864p_85Hz = 151,
					/*                                */
	TDA_VFMT_PC_1280x960p_85Hz = 152,
					/*                                */
	TDA_VFMT_PC_1280x1024p_85Hz = 153,
					/*                                */
	TDA_VFMT_PC_1024x768i_87Hz = 154,
					/*                                */
	TDA_VFMT_PC_MAX = 154,		/*                                */
	TDA_VFMT_PC_NUM = (1 + 154 - 128)	    /*                                */
} tda_video_fmt_id;

typedef struct {
	tda_video_fmt_id videoFormat;	     /*                                            */
	int nativeVideoFormat;		  /*                                                */
} tda_edid_video_desc;

typedef struct {
	tda_video_fmt_id videoFormat;	     /*                                            */
	int nativeVideoFormat;		  /*                                                */
} tda_short_video_desc;

typedef enum {
	TDA_P_ASPECT_RATIO_UNDEFINED = 0,   /*                                  */
	TDA_P_ASPECT_RATIO_6_5 = 1,	    /*                                  */
	TDA_P_ASPECT_RATIO_5_4 = 2,	    /*           */
	TDA_P_ASPECT_RATIO_4_3 = 3,	    /*           */
	TDA_P_ASPECT_RATIO_16_10 = 4,	    /*             */
	TDA_P_ASPECT_RATIO_5_3 = 5,	    /*           */
	TDA_P_ASPECT_RATIO_16_9 = 6,	    /*            */
	TDA_P_ASPECT_RATIO_9_5 = 7	    /*           */
} tda_pict_aspect_ratio;

typedef enum {
	TDA_VFREQ_24Hz = 0,	/*                 */
	TDA_VFREQ_25Hz = 1,	/*                 */
	TDA_VFREQ_30Hz = 2,	/*                 */
	TDA_VFREQ_50Hz = 3,	/*                 */
	TDA_VFREQ_59Hz = 4,	/*                 */
	TDA_VFREQ_60Hz = 5,	/*                 */
	TDA_VFREQ_70Hz = 6,	/*                 */
	TDA_VFREQ_72Hz = 7,	/*                 */
	TDA_VFREQ_75Hz = 8,	/*                 */
	TDA_VFREQ_85Hz = 9,	/*                 */
	TDA_VFREQ_87Hz = 10,	/*                 */
	TDA_VFREQ_INVALID = 11,	/*                 */
	TDA_VFREQ_NUM = 11	/*                 */
} tda_vfreq;

typedef struct {
	unsigned short width;			       /*                                */
	unsigned short height;			       /*                                 */
	int interlaced;			      /*                                */
	tda_vfreq vfrequency;	       /*                            */
	tda_pict_aspect_ratio aspectRatio;
					 /*                              */
} tda_video_fmt_specs;

typedef enum {
	TDA_VINMODE_CCIR656 = 0,    /*           */
	TDA_VINMODE_RGB444 = 1,	    /*           */
	TDA_VINMODE_YUV444 = 2,	    /*           */
	TDA_VINMODE_YUV422 = 3,	    /*           */
	TDA_VINMODE_NO_CHANGE = 4,  /*             */
	TDA_VINMODE_INVALID = 5	    /*           */
} tda_vinmode;

typedef enum {
	TDA_SYNCSRC_EMBEDDED = 0,
			      /*                 */
	TDA_SYNCSRC_EXT_VREF = 1,
			      /*                                  */
	TDA_SYNCSRC_EXT_VS = 2/*                        */
} tda_sync_source;

typedef enum {
	TDA_PIXRATE_DOUBLE = 0,		    /*                     */
	TDA_PIXRATE_SINGLE = 1,		    /*                     */
	TDA_PIXRATE_SINGLE_REPEATED = 2	    /*                         */
} tda_pix_rate;

typedef struct {
	tda_video_fmt_id format;      /*                                            */
	tda_vinmode mode;	/*                                     */
	tda_sync_source syncSource;
				 /*                    */
	tda_pix_rate pixelRate;	 /*              */
} tda_video_in;

typedef enum {
	TDA_VOUTMODE_RGB444 = 0,    /*             */
	TDA_VOUTMODE_YUV422 = 1,    /*             */
	TDA_VOUTMODE_YUV444 = 2	    /*             */
} tda_vout_mode;

typedef enum {
	TDA_VQR_DEFAULT = 0,	/*                   */
	TDA_RGB_FULL = 1,	/*                           */
	TDA_RGB_LIMITED = 2	/*                              */
} tda_vqr;

typedef enum {
	TDA_COLORDEPTH_24 = 0,	/*                    */
	TDA_COLORDEPTH_30 = 1,	/*                     */
	TDA_COLORDEPTH_36 = 2,	/*                     */
	TDA_COLORDEPTH_48 = 3	/*                     */
} tda_color_depth;

typedef struct {
	tda_video_fmt_id format;      /*                                            */
	tda_vout_mode mode;	 /*                                     */
	tda_color_depth colorDepth;
				/*               */
	tda_vqr dviVqr;		/*                           */
} tda_video_out;

typedef struct {
	tda_audio_format format;		 /*                                   */
	tda_audio_rate rate;			 /*                       */
	tda_audio_I2S_format i2sFormat;		  /*                                 */
	tda_audio_I2S_qualifier i2sQualifier;	  /*                                                   */
	tda_dst_rate dstRate;			 /*                          */
	unsigned char channelAllocation;			/*                       */
} tda_audio_in;

typedef enum {
	TDA_SINK_DVI = 0,
		       /*        */
	TDA_SINK_HDMI = 1,
		       /*        */
	TDA_SINK_EDID = 2
		       /*                                */
} tda_sink;

typedef enum {
	TDA_DEVICE_UNKNOWN,/*                        */
	TDA_DEVICE_TDA9984,/*                           */
	TDA_DEVICE_TDA9989,/*                           */
	TDA_DEVICE_TDA9981,/*                           */
	TDA_DEVICE_TDA9983,/*                           */
	TDA_DEVICE_TDA19989/*                            */
} tda_device_version;

typedef enum {
	TDA_HDMI_VERSION_UNKNOWN,
			     /*             */
	TDA_HDMI_VERSION_1_1,/*             */
	TDA_HDMI_VERSION_1_2a,
			     /*             */
	TDA_HDMI_VERSION_1_3a/*             */
} tda_hdmi_version;

typedef struct {
	int HBR;	 /*                             */
	int DST;	 /*                                        */
	int oneBitAudio; /*                               */
} tda_audio_packet;

typedef enum {
	TDA_COLORDEPTH_24 = 0,	/*                    */
	TDA_COLORDEPTH_30 = 1,	/*                     */
	TDA_COLORDEPTH_36 = 2,	/*                     */
	TDA_COLORDEPTH_48 = 3	/*                     */
} tda_color_depth;

typedef struct {
	tda_device_version deviceVersion;
				       /*                     */
	tda_hdmi_version hdmiVersion;/*                                    */
	tda_audio_packet audioPacket;/*                           */
	tda_color_depth colorDepth; /*                         */
	int hdcp;	/*                                          */
	int scaler;    /*                                 */
} tda_capabilities;

typedef struct {
	unsigned long compatibilityNr;	/*                                */
	unsigned long majorVersionNr;	/*                                */
	unsigned long minorVersionNr;	/*                                */
} tda_version;

typedef enum {
	PowerOn,		/*                                   */
	PowerStandby,		/*                                   */
	PowerSuspend,		/*                                   */
	PowerOff		/*                                   */
} tda_powerXXX;

typedef struct {
	unsigned int simplayHd;	     /*                            */
	unsigned int repeaterEnable; /*                        */
	unsigned char *pEdidBuffer;   /*                            */
	unsigned long edidBufferSize; /*                                    */
} tda_setup;

typedef struct {
	tda_video_fmt_id id;
	tda_video_fmt_specs spec;
} tda_video_format;

typedef struct {
	tda_video_in video_in;
	tda_video_out video_out;
	tda_audio_in audio_in;
} tda_set_in_out;

typedef struct {
	tda_edid_audio_desc desc;
	unsigned int max;
	unsigned int written;
	unsigned char flags;
} tda_edid_audio_caps;

typedef struct {
	tda_edid_video_desc desc;
	unsigned int max;
	unsigned int written;
	unsigned char flags;
} tda_edid_video_caps;

typedef struct {
	tda_edid_status status;
	unsigned char block_count;
} tda_edid;

#endif

#define TDA_IOCTL_BASE 0x40
#define RELEASE 0xFF

enum {
	/*                 */
	TDA_VERBOSE_ON_CMD = 0,
	TDA_VERBOSE_OFF_CMD,
	TDA_BYEBYE_CMD,
	/*         */
	TDA_GET_SW_VERSION_CMD,
	TDA_SET_POWER_CMD,
	TDA_GET_POWER_CMD,
	TDA_SETUP_CMD,
	TDA_GET_SETUP_CMD,
	TDA_WAIT_EVENT_CMD,
	TDA_ENABLE_EVENT_CMD,
	TDA_DISABLE_EVENT_CMD,
	TDA_GET_VIDEO_SPEC_CMD,
	TDA_SET_INPUT_OUTPUT_CMD,
	TDA_SET_AUDIO_INPUT_CMD,
	TDA_SET_VIDEO_INFOFRAME_CMD,
	TDA_SET_AUDIO_INFOFRAME_CMD,
	TDA_SET_ACP_CMD,
	TDA_SET_GCP_CMD,
	TDA_SET_ISRC1_CMD,
	TDA_SET_ISRC2_CMD,
	TDA_SET_MPS_INFOFRAME_CMD,
	TDA_SET_SPD_INFOFRAME_CMD,
	TDA_SET_VS_INFOFRAME_CMD,
	TDA_SET_AUDIO_MUTE_CMD,
	TDA_RESET_AUDIO_CTS_CMD,
	TDA_GET_EDID_STATUS_CMD,
	TDA_GET_EDID_AUDIO_CAPS_CMD,
	TDA_GET_EDID_VIDEO_CAPS_CMD,
	TDA_GET_EDID_VIDEO_PREF_CMD,
	TDA_GET_EDID_SINK_TYPE_CMD,
	TDA_GET_EDID_SOURCE_ADDRESS_CMD,
	TDA_SET_GAMMUT_CMD,
	TDA_GET_EDID_DTD_CMD,
	TDA_GET_EDID_MD_CMD,
	TDA_GET_EDID_TV_ASPECT_RATIO_CMD,
	TDA_GET_EDID_LATENCY_CMD,
	TDA_SET_HDCP_CMD,
	TDA_GET_HDCP_STATUS_CMD,
	TDA_GET_HPD_STATUS_CMD,
};


/*                 */
#define TDA_IOCTL_VERBOSE_ON     _IO(TDA_IOCTL_BASE, TDA_VERBOSE_ON_CMD)
#define TDA_IOCTL_VERBOSE_OFF     _IO(TDA_IOCTL_BASE, TDA_VERBOSE_OFF_CMD)
#define TDA_IOCTL_BYEBYE     _IO(TDA_IOCTL_BASE, TDA_BYEBYE_CMD)
/*         */
#define TDA_IOCTL_GET_SW_VERSION     _IOWR(TDA_IOCTL_BASE, TDA_GET_SW_VERSION_CMD, tda_version)
#define TDA_IOCTL_SET_POWER     _IOWR(TDA_IOCTL_BASE, TDA_SET_POWER_CMD, tda_power)
#define TDA_IOCTL_GET_POWER     _IOWR(TDA_IOCTL_BASE, TDA_GET_POWER_CMD, tda_power)
#define TDA_IOCTL_SETUP     _IOWR(TDA_IOCTL_BASE, TDA_SETUP_CMD, tda_setup_info)
#define TDA_IOCTL_GET_SETUP     _IOWR(TDA_IOCTL_BASE, TDA_GET_SETUP_CMD, tda_setup_info)
#define TDA_IOCTL_WAIT_EVENT     _IOWR(TDA_IOCTL_BASE, TDA_WAIT_EVENT_CMD, tda_event)
#define TDA_IOCTL_ENABLE_EVENT     _IOWR(TDA_IOCTL_BASE, TDA_ENABLE_EVENT_CMD, tda_event)
#define TDA_IOCTL_DISABLE_EVENT     _IOWR(TDA_IOCTL_BASE, TDA_DISABLE_EVENT_CMD, tda_event)
#define TDA_IOCTL_GET_VIDEO_SPEC     _IOWR(TDA_IOCTL_BASE, TDA_GET_VIDEO_SPEC_CMD, tda_video_format)
#define TDA_IOCTL_SET_INPUT_OUTPUT     _IOWR(TDA_IOCTL_BASE, TDA_SET_INPUT_OUTPUT_CMD, tda_set_in_out)
#define TDA_IOCTL_SET_AUDIO_INPUT     _IOWR(TDA_IOCTL_BASE, TDA_SET_AUDIO_INPUT_CMD, tda_audio_in)
#define TDA_IOCTL_SET_VIDEO_INFOFRAME     _IOWR(TDA_IOCTL_BASE, TDA_SET_VIDEO_INFOFRAME_CMD, tda_video_infoframe)
#define TDA_IOCTL_SET_AUDIO_INFOFRAME     _IOWR(TDA_IOCTL_BASE, TDA_SET_AUDIO_INFOFRAME_CMD, tda_audio_infoframe)
#define TDA_IOCTL_SET_ACP     _IOWR(TDA_IOCTL_BASE, TDA_SET_ACP_CMD, tda_acp)
#define TDA_IOCTL_SET_GCP     _IOWR(TDA_IOCTL_BASE, TDA_SET_GCP_CMD, tda_gcp)
#define TDA_IOCTL_SET_ISRC1     _IOWR(TDA_IOCTL_BASE, TDA_SET_ISRC1_CMD, tda_isrc1)
#define TDA_IOCTL_SET_ISRC2     _IOWR(TDA_IOCTL_BASE, TDA_SET_ISRC2_CMD, tda_isrc2)
#define TDA_IOCTL_SET_MPS_INFOFRAME     _IOWR(TDA_IOCTL_BASE, TDA_SET_MPS_INFOFRAME_CMD, tda_mps_infoframe)
#define TDA_IOCTL_SET_SPD_INFOFRAME     _IOWR(TDA_IOCTL_BASE, TDA_SET_SPD_INFOFRAME_CMD, tda_spd_infoframe)
#define TDA_IOCTL_SET_VS_INFOFRAME     _IOWR(TDA_IOCTL_BASE, TDA_SET_VS_INFOFRAME_CMD, tda_vs_infoframe)
#define TDA_IOCTL_SET_AUDIO_MUTE     _IOWR(TDA_IOCTL_BASE, TDA_SET_AUDIO_MUTE_CMD, bool)
#define TDA_IOCTL_RESET_AUDIO_CTS     _IO(TDA_IOCTL_BASE, TDA_RESET_AUDIO_CTS_CMD)
#define TDA_IOCTL_GET_EDID_STATUS     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_STATUS_CMD, tda_edid)
#define TDA_IOCTL_GET_EDID_AUDIO_CAPS     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_AUDIO_CAPS_CMD, tda_edid_audio_caps)
#define TDA_IOCTL_GET_EDID_VIDEO_CAPS     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_VIDEO_CAPS_CMD, tda_edid_video_caps)
#define TDA_IOCTL_GET_EDID_VIDEO_PREF     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_VIDEO_PREF_CMD, tda_edid_video_timings)
#define TDA_IOCTL_GET_EDID_SINK_TYPE     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_SINK_TYPE_CMD, tda_sink)
#define TDA_IOCTL_GET_EDID_SOURCE_ADDRESS     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_SOURCE_ADDRESS_CMD, unsigned short)
#define TDA_IOCTL_SET_GAMMUT     _IOWR(TDA_IOCTL_BASE, TDA_SET_GAMMUT_CMD, tda_gammut)
#define TDA_IOCTL_GET_EDID_DTD     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_DTD_CMD, tda_edid_dtd)
#define TDA_IOCTL_GET_EDID_MD     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_MD_CMD, tda_edid_md)
#define TDA_IOCTL_GET_EDID_TV_ASPECT_RATIO     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_TV_ASPECT_RATIO_CMD, tda_edid_tv_aspect_ratio)
#define TDA_IOCTL_GET_HPD_STATUS    _IOWR(TDA_IOCTL_BASE, TDA_GET_HPD_STATUS_CMD, tmdlHdmiTxHotPlug_t)
#ifdef TMFL_TDA19989
#define TDA_IOCTL_GET_EDID_LATENCY     _IOWR(TDA_IOCTL_BASE, TDA_GET_EDID_LATENCY_CMD, tda_edid_latency)
#define TDA_IOCTL_SET_HDCP     _IOWR(TDA_IOCTL_BASE, TDA_SET_HDCP_CMD, bool)
#define TDA_IOCTL_GET_HDCP_STATUS     _IOWR(TDA_IOCTL_BASE, TDA_GET_HDCP_STATUS_CMD, tda_hdcp_status)
#endif


/*                   */

/*         */
/*         */
/*                     */
/*             */
/*                     */

/*                            */
/*                                */
/*                               */
/*                                */
/*                    */
/*                     */
/*                     */
/*                             */
/*                             */
/*                              */
/*                              */
/*                                */
/*                                                    */
/*                                 */
/*                           */
/*                            */
/*                                   */
/*                              */
/*                             */
/*                                 */
/*                                 */
/*                            */
/*                                       */
/*                              */
/*                              */
/*                               */
/*                               */
/*                              */
/*                                  */
/*                                        */
/*                            */
/*                             */
/*                             */
/*                                */
/*                                */
/*                                     */
/*                               */
/*                                    */
/*                          */
/*                        */
/*                            */
/*                       */
/*                            */
/*                         */
/*                              */
/*                                                 */
/*                                         */
/*                                     */
/*                                     */
/*                                 */
/*                                  */
/*                          */
/*                             */


#endif				/*          */
#endif				/*              */

#ifndef __cec_ioctl__
#define __cec_ioctl__

#ifdef __cec_h__

typedef struct {
	UInt8 DayOfMonth;
	UInt8 MonthOfYear;
	UInt16 StartTime;
	tmdlHdmiCECDuration_t Duration;
	UInt8 RecordingSequence;
	tmdlHdmiCECAnalogueBroadcastType_t AnalogueBroadcastType;
	UInt16 AnalogueFrequency;
	tmdlHdmiCECBroadcastSystem_t BroadcastSystem;
} cec_analogue_timer;

typedef struct {
	UInt8 DayOfMonth;
	UInt8 MonthOfYear;
	UInt16 StartTime;
	tmdlHdmiCECDuration_t Duration;
	UInt8 RecordingSequence;
	tmdlHdmiCECDigitalServiceIdentification_t ServiceIdentification;
} cec_digital_timer;

typedef struct {
	UInt8 DayOfMonth;
	UInt8 MonthOfYear;
	UInt16 StartTime;
	tmdlHdmiCECDuration_t Duration;
	UInt8 RecordingSequence;
	tmdlHdmiCECExternalPlug_t ExternalPlug;
} cec_ext_timer_with_ext_plug;

typedef struct {
	UInt8 DayOfMonth;
	UInt8 MonthOfYear;
	UInt16 StartTime;
	tmdlHdmiCECDuration_t Duration;
	UInt8 RecordingSequence;
	tmdlHdmiCECExternalPhysicalAddress_t ExternalPhysicalAddress;
} cec_ext_timer_with_phy_addr;

typedef struct {
	tmdlHdmiCECFeatureOpcode_t FeatureOpcode;
	tmdlHdmiCECAbortReason_t AbortReason;
} cec_feature_abort;

typedef struct {
	tmdlHdmiCECAnalogueBroadcastType_t AnalogueBroadcastType;
	UInt16 AnalogueFrequency;
	tmdlHdmiCECBroadcastSystem_t BroadcastSystem;
} cec_analogue_service;

typedef struct {
	UInt16 OriginalAddress;
	UInt16 NewAddress;
} cec_routing_change;

typedef struct {
	char data[15];
	unsigned char length;
} cec_string;

typedef struct {
	tmdlHdmiCECDisplayControl_t DisplayControl;
	char data[15];
	unsigned char length;
} cec_osd_string;

typedef struct {
	tmdlHdmiCECRecordingFlag_t RecordingFlag;
	tmdlHdmiCECTunerDisplayInfo_t TunerDisplayInfo;
	tmdlHdmiCECAnalogueBroadcastType_t AnalogueBroadcastType;
	UInt16 AnalogueFrequency;
	tmdlHdmiCECBroadcastSystem_t BroadcastSystem;
} cec_tuner_device_status_analogue;

typedef struct {
	tmdlHdmiCECRecordingFlag_t RecordingFlag;
	tmdlHdmiCECTunerDisplayInfo_t TunerDisplayInfo;
	tmdlHdmiCECDigitalServiceIdentification_t ServiceIdentification;
} cec_tuner_device_status_digital;

typedef struct {
	unsigned long VendorID;
	cec_string cmd;
} cec_vendor_command_with_id;

/*
                  
               
                 
                 
*/

typedef struct {
	unsigned char count;
	unsigned char service;
	unsigned char addr;
	unsigned char data[15];
} cec_frame;
/*                                             */

typedef tmSWVersion_t cec_sw_version;
typedef tmPowerState_t cec_power;
typedef tmdlHdmiCecInstanceSetup_t cec_setup;
typedef tmdlHdmiCecEvent_t cec_event;
typedef tmdlHdmiCecClockSource_t cec_clock;
typedef tmdlHdmiCECSystemAudioStatus_t cec_sys_audio_status;
typedef tmdlHdmiCECAudioRate_t cec_audio_rate;
typedef tmdlHdmiCECDigitalServiceIdentification_t cec_digital_service;
typedef tmdlHdmiCECVersion_t cec_version;
typedef tmdlHdmiCECDecControlMode_t cec_deck_ctrl;
typedef tmdlHdmiCECDecInfo_t cec_deck_status;
typedef tmdlHdmiCECStatusRequest_t cec_status_request;
typedef tmdlHdmiCECMenuRequestType_t cec_menu_request;
typedef tmdlHdmiCECMenuState_t cec_menu_status;
typedef tmdlHdmiCECPlayMode_t cec_play;
typedef tmdlHdmiCECExternalPlug_t cec_ext_plug;
typedef tmdlHdmiCECRecordStatusInfo_t cec_rec_status;
typedef tmdlHdmiCECAudioStatus_t cec_audio_status;
typedef tmdlHdmiCECPowerStatus_t cec_power_status;
typedef tmdlHdmiCECTimerClearedStatusData_t cec_timer_cleared_status;
typedef tmdlHdmiCECTimerStatusData_t cec_timer_status;
typedef tmdlHdmiCECUserRemoteControlCommand_t cec_user_ctrl;
typedef tmdlHdmiCECChannelIdentifier_t cec_user_ctrl_tune;
typedef tmdlHdmiCECDeviceType_t cec_device_type;

#define CEC_IOCTL_BASE 0x40

/*         */
enum {
	CEC_WAITING = 0x80,
	CEC_RELEASE,
	CEC_RX_DONE,
	CEC_TX_DONE
};

enum {
	/*                 */
	CEC_VERBOSE_ON_CMD = 0,
	CEC_VERBOSE_OFF_CMD,
	CEC_BYEBYE_CMD,

	/*     */
	CEC_IOCTL_RX_ADDR_CMD,	/*                                   */
	CEC_IOCTL_PHY_ADDR_CMD,	/*                           */
	CEC_IOCTL_WAIT_FRAME_CMD,
	CEC_IOCTL_ABORT_MSG_CMD,
	CEC_IOCTL_ACTIVE_SRC_CMD,
	CEC_IOCTL_VERSION_CMD,
	CEC_IOCTL_CLEAR_ANALOGUE_TIMER_CMD,
	CEC_IOCTL_CLEAR_DIGITAL_TIMER_CMD,
	CEC_IOCTL_CLEAR_EXT_TIMER_WITH_EXT_PLUG_CMD,
	CEC_IOCTL_CLEAR_EXT_TIMER_WITH_PHY_ADDR_CMD,
	CEC_IOCTL_DECK_CTRL_CMD,
	CEC_IOCTL_DECK_STATUS_CMD,
	CEC_IOCTL_DEVICE_VENDOR_ID_CMD,
	CEC_IOCTL_FEATURE_ABORT_CMD,
	CEC_IOCTL_GET_CEC_VERSION_CMD,
	CEC_IOCTL_GET_MENU_LANGUAGE_CMD,
	CEC_IOCTL_GIVE_AUDIO_STATUS_CMD,
	CEC_IOCTL_GIVE_DECK_STATUS_CMD,
	CEC_IOCTL_GIVE_DEVICE_POWER_STATUS_CMD,
	CEC_IOCTL_GIVE_DEVICE_VENDOR_ID_CMD,
	CEC_IOCTL_GIVE_OSD_NAME_CMD,
	CEC_IOCTL_GIVE_PHY_ADDR_CMD,
	CEC_IOCTL_GIVE_SYS_AUDIO_MODE_STATUS_CMD,
	CEC_IOCTL_GIVE_TUNER_DEVICE_STATUS_CMD,
	CEC_IOCTL_IMAGE_VIEW_ON_CMD,
	CEC_IOCTL_INACTIVE_SRC_CMD,
	CEC_IOCTL_MENU_REQUEST_CMD,
	CEC_IOCTL_MENU_STATUS_CMD,
	CEC_IOCTL_PLAY_CMD,
	CEC_IOCTL_POLLING_MSG_CMD,
	CEC_IOCTL_REC_OFF_CMD,
	CEC_IOCTL_REC_ON_ANALOGUE_SERVICE_CMD,
	CEC_IOCTL_REC_ON_DIGITAL_SERVICE_CMD,
	CEC_IOCTL_REC_ON_EXT_PHY_ADDR_CMD,
	CEC_IOCTL_REC_ON_EXT_PLUG_CMD,
	CEC_IOCTL_REC_ON_OWN_SRC_CMD,
	CEC_IOCTL_REC_STATUS_CMD,
	CEC_IOCTL_REC_TV_SCREEN_CMD,
	CEC_IOCTL_REPORT_AUDIO_STATUS_CMD,
	CEC_IOCTL_REPORT_PHY_ADDR_CMD,
	CEC_IOCTL_REPORT_POWER_STATUS_CMD,
	CEC_IOCTL_REQUEST_ACTIVE_SRC_CMD,
	CEC_IOCTL_ROUTING_CHANGE_CMD,
	CEC_IOCTL_ROUTING_INFORMATION_CMD,
	CEC_IOCTL_SELECT_ANALOGUE_SERVICE_CMD,
	CEC_IOCTL_SELECT_DIGITAL_SERVICE_CMD,
	CEC_IOCTL_SET_ANALOGUE_TIMER_CMD,
	CEC_IOCTL_SET_AUDIO_RATE_CMD,
	CEC_IOCTL_SET_DIGITAL_TIMER_CMD,
	CEC_IOCTL_SET_EXT_TIMER_WITH_EXT_PLUG_CMD,
	CEC_IOCTL_SET_EXT_TIMER_WITH_PHY_ADDR_CMD,
	CEC_IOCTL_SET_MENU_LANGUAGE_CMD,
	CEC_IOCTL_SET_OSD_NAME_CMD,
	CEC_IOCTL_SET_OSD_STRING_CMD,
	CEC_IOCTL_SET_STREAM_PATH_CMD,
	CEC_IOCTL_SET_SYS_AUDIO_MODE_CMD,
	CEC_IOCTL_SET_TIMER_PROGRAM_TITLE_CMD,
	CEC_IOCTL_STANDBY_CMD,
	CEC_IOCTL_SYS_AUDIO_MODE_REQUEST_CMD,
	CEC_IOCTL_SYS_AUDIO_MODE_STATUS_CMD,
	CEC_IOCTL_TEXT_VIEW_ON_CMD,
	CEC_IOCTL_TIMER_CLEARED_STATUS_CMD,
	CEC_IOCTL_TIMER_STATUS_CMD,
	CEC_IOCTL_TUNER_DEVICE_STATUS_ANALOGUE_CMD,
	CEC_IOCTL_TUNER_DEVICE_STATUS_DIGITAL_CMD,
	CEC_IOCTL_TUNER_STEP_DECREMENT_CMD,
	CEC_IOCTL_TUNER_STEP_INCREMENT_CMD,
	CEC_IOCTL_USER_CTRL_CMD,
	CEC_IOCTL_USER_CTRL_PLAY_CMD,
	CEC_IOCTL_USER_CTRL_SELECT_AUDIOINPUT_CMD,
	CEC_IOCTL_USER_CTRL_SELECT_AVINPUT_CMD,
	CEC_IOCTL_USER_CTRL_SELECT_MEDIA_CMD,
	CEC_IOCTL_USER_CTRL_TUNE_CMD,
	CEC_IOCTL_USER_CTRL_RELEASED_CMD,
	CEC_IOCTL_VENDOR_COMMAND_CMD,
	CEC_IOCTL_VENDOR_COMMAND_WITH_ID_CMD,
	CEC_IOCTL_VENDOR_REMOTE_BUTTON_DOWN_CMD,
	CEC_IOCTL_VENDOR_REMOTE_BUTTON_UP_CMD,
	CEC_IOCTL_GET_SW_VERSION_CMD,
	CEC_IOCTL_SET_POWER_STATE_CMD,
	CEC_IOCTL_GET_POWER_STATE_CMD,
	CEC_IOCTL_INSTANCE_CONFIG_CMD,
	CEC_IOCTL_INSTANCE_SETUP_CMD,
	CEC_IOCTL_GET_INSTANCE_SETUP_CMD,
	CEC_IOCTL_ENABLE_EVENT_CMD,
	CEC_IOCTL_DISABLE_EVENT_CMD,
	CEC_IOCTL_ENABLE_CALIBRATION_CMD,
	CEC_IOCTL_DISABLE_CALIBRATION_CMD,
	CEC_IOCTL_SEND_MSG_CMD,
	CEC_IOCTL_SET_REGISTER_CMD
};


/*                 */
#define CEC_IOCTL_VERBOSE_ON       _IO(CEC_IOCTL_BASE, CEC_VERBOSE_ON_CMD)
#define CEC_IOCTL_VERBOSE_OFF      _IO(CEC_IOCTL_BASE, CEC_VERBOSE_OFF_CMD)
#define CEC_IOCTL_BYEBYE      _IO(CEC_IOCTL_BASE, CEC_BYEBYE_CMD)

/*     */
#define CEC_IOCTL_RX_ADDR      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_RX_ADDR_CMD, unsigned char)
#define CEC_IOCTL_PHY_ADDR      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_PHY_ADDR_CMD, unsigned short)
#define CEC_IOCTL_WAIT_FRAME      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_WAIT_FRAME_CMD, cec_frame)
#define CEC_IOCTL_ABORT_MSG      _IO(CEC_IOCTL_BASE, CEC_IOCTL_ABORT_MSG_CMD)
#define CEC_IOCTL_ACTIVE_SRC      _IO(CEC_IOCTL_BASE, CEC_IOCTL_ACTIVE_SRC_CMD)
#define CEC_IOCTL_VERSION      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_VERSION_CMD, cec_version)
#define CEC_IOCTL_CLEAR_ANALOGUE_TIMER      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_CLEAR_ANALOGUE_TIMER_CMD, cec_analogue_timer)
#define CEC_IOCTL_CLEAR_DIGITAL_TIMER      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_CLEAR_DIGITAL_TIMER_CMD, cec_digital_timer)
#define CEC_IOCTL_CLEAR_EXT_TIMER_WITH_EXT_PLUG      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_CLEAR_EXT_TIMER_WITH_EXT_PLUG_CMD, cec_ext_timer_with_ext_plug)
#define CEC_IOCTL_CLEAR_EXT_TIMER_WITH_PHY_ADDR      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_CLEAR_EXT_TIMER_WITH_PHY_ADDR_CMD, cec_ext_timer_with_phy_addr)
#define CEC_IOCTL_DECK_CTRL      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_DECK_CTRL_CMD, cec_deck_ctrl)
#define CEC_IOCTL_DECK_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_DECK_STATUS_CMD, cec_deck_status)
#define CEC_IOCTL_DEVICE_VENDOR_ID      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_DEVICE_VENDOR_ID_CMD, unsigned long)
#define CEC_IOCTL_FEATURE_ABORT      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_FEATURE_ABORT_CMD, cec_feature_abort)
#define CEC_IOCTL_GET_CEC_VERSION      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_GET_CEC_VERSION_CMD, unsigned char)
#define CEC_IOCTL_GET_MENU_LANGUAGE      _IO(CEC_IOCTL_BASE, CEC_IOCTL_GET_MENU_LANGUAGE_CMD)
#define CEC_IOCTL_GIVE_AUDIO_STATUS      _IO(CEC_IOCTL_BASE, CEC_IOCTL_GIVE_AUDIO_STATUS_CMD)
#define CEC_IOCTL_GIVE_DECK_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_GIVE_DECK_STATUS_CMD, cec_status_request)
#define CEC_IOCTL_GIVE_DEVICE_POWER_STATUS      _IO(CEC_IOCTL_BASE, CEC_IOCTL_GIVE_DEVICE_POWER_STATUS_CMD)
#define CEC_IOCTL_GIVE_DEVICE_VENDOR_ID      _IO(CEC_IOCTL_BASE, CEC_IOCTL_GIVE_DEVICE_VENDOR_ID_CMD)
#define CEC_IOCTL_GIVE_OSD_NAME      _IO(CEC_IOCTL_BASE, CEC_IOCTL_GIVE_OSD_NAME_CMD)
#define CEC_IOCTL_GIVE_PHY_ADDR      _IO(CEC_IOCTL_BASE, CEC_IOCTL_GIVE_PHY_ADDR_CMD)
#define CEC_IOCTL_GIVE_SYS_AUDIO_MODE_STATUS      _IO(CEC_IOCTL_BASE, CEC_IOCTL_GIVE_SYS_AUDIO_MODE_STATUS_CMD)
#define CEC_IOCTL_GIVE_TUNER_DEVICE_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_GIVE_TUNER_DEVICE_STATUS_CMD, cec_status_request)
#define CEC_IOCTL_IMAGE_VIEW_ON      _IO(CEC_IOCTL_BASE, CEC_IOCTL_IMAGE_VIEW_ON_CMD)
#define CEC_IOCTL_INACTIVE_SRC      _IO(CEC_IOCTL_BASE, CEC_IOCTL_INACTIVE_SRC_CMD)
#define CEC_IOCTL_MENU_REQUEST      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_MENU_REQUEST_CMD, cec_menu_request)
#define CEC_IOCTL_MENU_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_MENU_STATUS_CMD, cec_menu_status)
#define CEC_IOCTL_PLAY      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_PLAY_CMD, cec_play)
#define CEC_IOCTL_POLLING_MSG      _IO(CEC_IOCTL_BASE, CEC_IOCTL_POLLING_MSG_CMD)
#define CEC_IOCTL_REC_OFF      _IO(CEC_IOCTL_BASE, CEC_IOCTL_REC_OFF_CMD)
#define CEC_IOCTL_REC_ON_ANALOGUE_SERVICE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_REC_ON_ANALOGUE_SERVICE_CMD, cec_analogue_service)
#define CEC_IOCTL_REC_ON_DIGITAL_SERVICE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_REC_ON_DIGITAL_SERVICE_CMD, cec_digital_service)
#define CEC_IOCTL_REC_ON_EXT_PHY_ADDR      _IO(CEC_IOCTL_BASE, CEC_IOCTL_REC_ON_EXT_PHY_ADDR_CMD)
#define CEC_IOCTL_REC_ON_EXT_PLUG      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_REC_ON_EXT_PLUG_CMD, cec_ext_plug)
#define CEC_IOCTL_REC_ON_OWN_SRC      _IO(CEC_IOCTL_BASE, CEC_IOCTL_REC_ON_OWN_SRC_CMD)
#define CEC_IOCTL_REC_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_REC_STATUS_CMD, cec_rec_status)
#define CEC_IOCTL_REC_TV_SCREEN      _IO(CEC_IOCTL_BASE, CEC_IOCTL_REC_TV_SCREEN_CMD)
#define CEC_IOCTL_REPORT_AUDIO_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_REPORT_AUDIO_STATUS_CMD, cec_audio_status)
#define CEC_IOCTL_REPORT_PHY_ADDR      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_REPORT_PHY_ADDR_CMD, cec_device_type)
#define CEC_IOCTL_REPORT_POWER_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_REPORT_POWER_STATUS_CMD, cec_power_status)
#define CEC_IOCTL_REQUEST_ACTIVE_SRC      _IO(CEC_IOCTL_BASE, CEC_IOCTL_REQUEST_ACTIVE_SRC_CMD)
#define CEC_IOCTL_ROUTING_CHANGE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_ROUTING_CHANGE_CMD, cec_routing_change)
#define CEC_IOCTL_ROUTING_INFORMATION      _IO(CEC_IOCTL_BASE, CEC_IOCTL_ROUTING_INFORMATION_CMD)
#define CEC_IOCTL_SELECT_ANALOGUE_SERVICE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SELECT_ANALOGUE_SERVICE_CMD, cec_analogue_service)
#define CEC_IOCTL_SELECT_DIGITAL_SERVICE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SELECT_DIGITAL_SERVICE_CMD, cec_digital_service)
#define CEC_IOCTL_SET_ANALOGUE_TIMER      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_ANALOGUE_TIMER_CMD, cec_analogue_timer)
#define CEC_IOCTL_SET_AUDIO_RATE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_AUDIO_RATE_CMD, cec_audio_rate)
#define CEC_IOCTL_SET_DIGITAL_TIMER      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_DIGITAL_TIMER_CMD, cec_digital_timer)
#define CEC_IOCTL_SET_EXT_TIMER_WITH_EXT_PLUG      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_EXT_TIMER_WITH_EXT_PLUG_CMD, cec_ext_timer_with_ext_plug)
#define CEC_IOCTL_SET_EXT_TIMER_WITH_PHY_ADDR      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_EXT_TIMER_WITH_PHY_ADDR_CMD, cec_ext_timer_with_phy_addr)
#define CEC_IOCTL_SET_MENU_LANGUAGE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_MENU_LANGUAGE_CMD, cec_string)
#define CEC_IOCTL_SET_OSD_NAME      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_OSD_NAME_CMD, cec_string)
#define CEC_IOCTL_SET_OSD_STRING      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_OSD_STRING_CMD, cec_osd_string)
#define CEC_IOCTL_SET_STREAM_PATH      _IO(CEC_IOCTL_BASE, CEC_IOCTL_SET_STREAM_PATH_CMD)
#define CEC_IOCTL_SET_SYS_AUDIO_MODE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_SYS_AUDIO_MODE_CMD, cec_sys_audio_status)
#define CEC_IOCTL_SET_TIMER_PROGRAM_TITLE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_TIMER_PROGRAM_TITLE_CMD, cec_string)
#define CEC_IOCTL_STANDBY      _IO(CEC_IOCTL_BASE, CEC_IOCTL_STANDBY_CMD)
#define CEC_IOCTL_SYS_AUDIO_MODE_REQUEST      _IO(CEC_IOCTL_BASE, CEC_IOCTL_SYS_AUDIO_MODE_REQUEST_CMD)
#define CEC_IOCTL_SYS_AUDIO_MODE_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SYS_AUDIO_MODE_STATUS_CMD, cec_sys_audio_status)
#define CEC_IOCTL_TEXT_VIEW_ON      _IO(CEC_IOCTL_BASE, CEC_IOCTL_TEXT_VIEW_ON_CMD)
#define CEC_IOCTL_TIMER_CLEARED_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_TIMER_CLEARED_STATUS_CMD, cec_timer_cleared_status)
#define CEC_IOCTL_TIMER_STATUS      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_TIMER_STATUS_CMD, cec_timer_status)
#define CEC_IOCTL_TUNER_DEVICE_STATUS_ANALOGUE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_TUNER_DEVICE_STATUS_ANALOGUE_CMD, cec_tuner_device_status_analogue)
#define CEC_IOCTL_TUNER_DEVICE_STATUS_DIGITAL      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_TUNER_DEVICE_STATUS_DIGITAL_CMD, cec_tuner_device_status_digital)
#define CEC_IOCTL_TUNER_STEP_DECREMENT      _IO(CEC_IOCTL_BASE, CEC_IOCTL_TUNER_STEP_DECREMENT_CMD)
#define CEC_IOCTL_TUNER_STEP_INCREMENT      _IO(CEC_IOCTL_BASE, CEC_IOCTL_TUNER_STEP_INCREMENT_CMD)
#define CEC_IOCTL_USER_CTRL_PRESSED      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_USER_CTRL_CMD, cec_user_ctrl)
#define CEC_IOCTL_USER_CTRL_PLAY      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_USER_CTRL_PLAY_CMD, cec_play)
#define CEC_IOCTL_USER_CTRL_SELECT_AUDIOINPUT      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_USER_CTRL_SELECT_AUDIOINPUT_CMD, unsigned char)
#define CEC_IOCTL_USER_CTRL_SELECT_AVINPUT      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_USER_CTRL_SELECT_AVINPUT_CMD, unsigned char)
#define CEC_IOCTL_USER_CTRL_SELECT_MEDIA      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_USER_CTRL_SELECT_MEDIA_CMD, unsigned char)
#define CEC_IOCTL_USER_CTRL_TUNE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_USER_CTRL_TUNE_CMD, cec_user_ctrl_tune)
#define CEC_IOCTL_USER_CTRL_RELEASED      _IO(CEC_IOCTL_BASE, CEC_IOCTL_USER_CTRL_RELEASED_CMD)
#define CEC_IOCTL_VENDOR_COMMAND      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_VENDOR_COMMAND_CMD, cec_string)
#define CEC_IOCTL_VENDOR_COMMAND_WITH_ID      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_VENDOR_COMMAND_WITH_ID_CMD, cec_vendor_command_with_id)
#define CEC_IOCTL_VENDOR_REMOTE_BUTTON_DOWN      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_VENDOR_REMOTE_BUTTON_DOWN_CMD, cec_string)
#define CEC_IOCTL_VENDOR_REMOTE_BUTTON_UP      _IO(CEC_IOCTL_BASE, CEC_IOCTL_VENDOR_REMOTE_BUTTON_UP_CMD)
#define CEC_IOCTL_GET_SW_VERSION      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_GET_SW_VERSION_CMD, cec_sw_version)
#define CEC_IOCTL_SET_POWER_STATE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_SET_POWER_STATE_CMD, cec_power)
#define CEC_IOCTL_GET_POWER_STATE      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_GET_POWER_STATE_CMD, cec_power)
#define CEC_IOCTL_INSTANCE_CONFIG      _IO(CEC_IOCTL_BASE, CEC_IOCTL_INSTANCE_CONFIG_CMD)
#define CEC_IOCTL_INSTANCE_SETUP      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_INSTANCE_SETUP_CMD, cec_setup)
#define CEC_IOCTL_GET_INSTANCE_SETUP      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_GET_INSTANCE_SETUP_CMD, cec_setup)
#define CEC_IOCTL_ENABLE_EVENT      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_ENABLE_EVENT_CMD, cec_event)
#define CEC_IOCTL_DISABLE_EVENT      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_DISABLE_EVENT_CMD, cec_event)
#define CEC_IOCTL_ENABLE_CALIBRATION      _IOWR(CEC_IOCTL_BASE, CEC_IOCTL_ENABLE_CALIBRATION_CMD, cec_clock)
#define CEC_IOCTL_DISABLE_CALIBRATION      _IO(CEC_IOCTL_BASE, CEC_IOCTL_DISABLE_CALIBRATION_CMD)
/*                                                                                           */

/*                   */

/*         */
/*         */
/*                     */
/*             */
/*                     */

/*                             */
/*                             */
/*                        */
/*                                   */
/*                                  */
/*                                                   */
/*                                                      */
/*                            */
/*                           */
/*                               */
/*                             */
/*                              */
/*                                */
/*                                */
/*                               */
/*                                      */
/*                                   */
/*                            */
/*                                    */
/*                                          */
/*                                      */
/*                            */
/*                               */
/*                            */
/*                           */
/*                     */
/*                               */
/*                          */
/*                                        */
/*                                       */
/*                                                */
/*                                     */
/*                                  */
/*                             */
/*                               */
/*                                  */
/*                                      */
/*                                  */
/*                                    */
/*                              */
/*                                   */
/*                                      */
/*                                     */
/*                                 */
/*                             */
/*                                */
/*                                                 */
/*                                                    */
/*                                */
/*                           */
/*                             */
/*                              */
/*                                   */
/*                                     */
/*                        */
/*                                       */
/*                                      */
/*                           */
/*                                   */
/*                            */
/*                                          */
/*                                         */
/*                                   */
/*                                   */
/*                                   */
/*                                       */
/*                                                   */
/*                                                */
/*                                              */
/*                                       */
/*                                    */
/*                              */
/*                                    */
/*                                       */
/*                                     */
/*                             */
/*                                 */
/*                                */
/*                                 */
/*                     */
/*                      */
/*                      */
/*                              */
/*                              */
/*                               */
/*                              */
/*                                 */
/*                                */
/*                                  */
/*                              */
/*                                  */
/*                            */
/*                             */
/*                                  */
/*                                   */
/*                            */
/*                            */


#endif				/*           */
#endif				/*               */
