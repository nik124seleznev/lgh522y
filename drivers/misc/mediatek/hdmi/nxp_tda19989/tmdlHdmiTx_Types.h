/**
 * Copyright (C) 2007 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiTx_Types.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 02/08/07 08:32 $
 *
 * \brief         devlib driver component API for the TDA998x HDMI Transmitters
 *
 * \section refs  Reference Documents
 * HDMI Tx Driver - FRS.doc,
 * HDMI Tx Driver - tmdlHdmiTx - SCS.doc
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmdlHdmiTx_Types.h $
 *
 * *****************  Version 1  *****************
 * User: Demoment     Date: 02/08/07   Time: 08:32
 * Updated in $/Source/tmdlHdmiTx/inc
 * initial version

   \endverbatim
 *
*/

#ifndef TMDLHDMITX_TYPES_H
#define TMDLHDMITX_TYPES_H

/*                                                                            */
/*                                                                            */
/*                                                                            */

#include "tmNxTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*               */
#define TMDL_ERR_DLHDMITX_BASE                      CID_DL_HDMITX
#define TMDL_ERR_DLHDMITX_COMP                      (TMDL_ERR_DLHDMITX_BASE | TM_ERR_COMP_UNIQUE_START)

#define TMDL_ERR_DLHDMITX_COMPATIBILITY             (TMDL_ERR_DLHDMITX_BASE + TM_ERR_COMPATIBILITY)		/*                                */
#define TMDL_ERR_DLHDMITX_MAJOR_VERSION             (TMDL_ERR_DLHDMITX_BASE + TM_ERR_MAJOR_VERSION)		/*                                */
#define TMDL_ERR_DLHDMITX_COMP_VERSION              (TMDL_ERR_DLHDMITX_BASE + TM_ERR_COMP_VERSION)		/*                                */
#define TMDL_ERR_DLHDMITX_BAD_UNIT_NUMBER           (TMDL_ERR_DLHDMITX_BASE + TM_ERR_BAD_UNIT_NUMBER)		/*                                */
#define TMDL_ERR_DLHDMITX_BAD_INSTANCE              (TMDL_ERR_DLHDMITX_BASE + TM_ERR_BAD_INSTANCE)		/*                                */
#define TMDL_ERR_DLHDMITX_BAD_HANDLE                (TMDL_ERR_DLHDMITX_BASE + TM_ERR_BAD_HANDLE)		/*                                */
#define TMDL_ERR_DLHDMITX_BAD_PARAMETER             (TMDL_ERR_DLHDMITX_BASE + TM_ERR_BAD_PARAMETER)		/*                                */
#define TMDL_ERR_DLHDMITX_NO_RESOURCES              (TMDL_ERR_DLHDMITX_BASE + TM_ERR_NO_RESOURCES)		/*                                */
#define TMDL_ERR_DLHDMITX_RESOURCE_OWNED            (TMDL_ERR_DLHDMITX_BASE + TM_ERR_RESOURCE_OWNED)		/*                                */
#define TMDL_ERR_DLHDMITX_RESOURCE_NOT_OWNED        (TMDL_ERR_DLHDMITX_BASE + TM_ERR_RESOURCE_NOT_OWNED)	/*                                */
#define TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS       (TMDL_ERR_DLHDMITX_BASE + TM_ERR_INCONSISTENT_PARAMS)	/*                                */
#define TMDL_ERR_DLHDMITX_NOT_INITIALIZED           (TMDL_ERR_DLHDMITX_BASE + TM_ERR_NOT_INITIALIZED)		/*                                */
#define TMDL_ERR_DLHDMITX_NOT_SUPPORTED             (TMDL_ERR_DLHDMITX_BASE + TM_ERR_NOT_SUPPORTED)		/*                                */
#define TMDL_ERR_DLHDMITX_INIT_FAILED               (TMDL_ERR_DLHDMITX_BASE + TM_ERR_INIT_FAILED)		/*                                */
#define TMDL_ERR_DLHDMITX_BUSY                      (TMDL_ERR_DLHDMITX_BASE + TM_ERR_BUSY)			/*                                */
#define TMDL_ERR_DLHDMITX_I2C_READ                  (TMDL_ERR_DLHDMITX_BASE + TM_ERR_READ)			/*                                */
#define TMDL_ERR_DLHDMITX_I2C_WRITE                 (TMDL_ERR_DLHDMITX_BASE + TM_ERR_WRITE)			/*                                */
#define TMDL_ERR_DLHDMITX_FULL                      (TMDL_ERR_DLHDMITX_BASE + TM_ERR_FULL)			/*                                */
#define TMDL_ERR_DLHDMITX_NOT_STARTED               (TMDL_ERR_DLHDMITX_BASE + TM_ERR_NOT_STARTED)		/*                                */
#define TMDL_ERR_DLHDMITX_ALREADY_STARTED           (TMDL_ERR_DLHDMITX_BASE + TM_ERR_ALREADY_STARTED)		/*                                */
#define TMDL_ERR_DLHDMITX_ASSERTION                 (TMDL_ERR_DLHDMITX_BASE + TM_ERR_ASSERTION)			/*                                */
#define TMDL_ERR_DLHDMITX_INVALID_STATE             (TMDL_ERR_DLHDMITX_BASE + TM_ERR_INVALID_STATE)		/*                                */
#define TMDL_ERR_DLHDMITX_OPERATION_NOT_PERMITTED   (TMDL_ERR_DLHDMITX_BASE + TM_ERR_OPERATION_NOT_PERMITTED)	/*                                */
#define TMDL_ERR_DLHDMITX_RESOLUTION_UNKNOWN        (TMDL_ERR_DLHDMITX_BASE + TM_ERR_BAD_FORMAT)		/*                                */

#define TMDL_DLHDMITX_HDCP_SECURE               (TMDL_ERR_DLHDMITX_COMP + 0x0001) /*                             */
#define TMDL_DLHDMITX_HDCP_NOT_SECURE           (TMDL_ERR_DLHDMITX_COMP + 0x0002) /*                                 */


/*                                                                            */
/*                                                                            */
/*                                                                            */

/* 
                                                                      
 */
	typedef enum {
		TMDL_HDMITX_HDCP_ACTIVE = 0,/*                                             */
		TMDL_HDMITX_HDCP_INACTIVE = 1,
					    /*                                               */
		TMDL_HDMITX_HPD_ACTIVE = 2, /*                                     */
		TMDL_HDMITX_HPD_INACTIVE = 3,
					    /*                                       */
		TMDL_HDMITX_RX_KEYS_RECEIVED = 4,
					    /*                               */
		TMDL_HDMITX_RX_DEVICE_ACTIVE = 5,
					    /*                                     */
		TMDL_HDMITX_RX_DEVICE_INACTIVE = 6,
					    /*                                                 */
		TMDL_HDMITX_EDID_RECEIVED = 7,
					    /*                          */
		TMDL_HDMITX_VS_RPT_RECEIVED = 8,
					    /*                                  */
#ifdef HDMI_TX_REPEATER_ISR_MODE
		TMDL_HDMITX_B_STATUS = 9,   /*                       */
#endif				/*                           */
		TMDL_HDMITX_DEBUG_EVENT_1 = 10
					     /*                         */
	} tmdlHdmiTxEvent_t;

/* 
                                                 
 */
	typedef enum {
		TMDL_HDMITX_EVENT_ENABLED,
				/*                    */
		TMDL_HDMITX_EVENT_DISABLED
				/*                     */
	} tmdlHdmiTxEventStatus_t;

/* 
                                                                          
                                                         
                                               
 */
	typedef void (*ptmdlHdmiTxCallback_t) (tmdlHdmiTxEvent_t event);

/* 
                                                    
 */
	typedef enum {
		TMDL_HDMITX_DEVICE_UNKNOWN,
				   /*                        */
		TMDL_HDMITX_DEVICE_TDA9984,
				   /*                           */
		TMDL_HDMITX_DEVICE_TDA9989,
				   /*                           */
		TMDL_HDMITX_DEVICE_TDA9981,
				   /*                           */
		TMDL_HDMITX_DEVICE_TDA9983,
				   /*                           */
		TMDL_HDMITX_DEVICE_TDA19989,
				   /*                            */
		TMDL_HDMITX_DEVICE_TDA19988
				   /*                                 */
	} tmdlHdmiTxDeviceVersion_t;

/* 
                                                           
 */
	typedef enum {
		TMDL_HDMITX_HDMI_VERSION_UNKNOWN,
				     /*             */
		TMDL_HDMITX_HDMI_VERSION_1_1,
				     /*             */
		TMDL_HDMITX_HDMI_VERSION_1_2a,
				     /*             */
		TMDL_HDMITX_HDMI_VERSION_1_3a
				     /*             */
	} tmdlHdmiTxHdmiVersion_t;

/* 
                                                                          
 */
	typedef enum {
		TMDL_HDMITX_COLORDEPTH_24 = 0,
					/*                    */
		TMDL_HDMITX_COLORDEPTH_30 = 1,
					/*                     */
		TMDL_HDMITX_COLORDEPTH_36 = 2,
					/*                     */
		TMDL_HDMITX_COLORDEPTH_48 = 3
					/*                     */
	} tmdlHdmiTxColorDepth_t;

/* 
                                       
 */
	typedef enum {
		TMDL_HDMITX_EDID_READ = 0,	/*                      */
		TMDL_HDMITX_EDID_READ_INCOMPLETE = 1,
						/*                                                                 */
		TMDL_HDMITX_EDID_ERROR_CHK_BLOCK_0 = 2,
						/*                          */
		TMDL_HDMITX_EDID_ERROR_CHK = 3,	/*                                                          */
		TMDL_HDMITX_EDID_NOT_READ = 4,	/*                 */
		TMDL_HDMITX_EDID_STATUS_INVALID = 5
						/*             */
	} tmdlHdmiTxEdidStatus_t;

/* 
                                                        
 */
	typedef struct {
		Bool HBR; /*                             */
		Bool DST; /*                                        */
		Bool oneBitAudio;
			  /*                               */
	} tmdlHdmiTxAudioPacket_t;

/* 
                                                       
 */
	typedef enum {
		TMDL_HDMITX_AFMT_SPDIF = 0,
				     /*         */
		TMDL_HDMITX_AFMT_I2S = 1,
				     /*       */
		TMDL_HDMITX_AFMT_OBA = 2,
				     /*                       */
		TMDL_HDMITX_AFMT_DST = 3,
				     /*       */
		TMDL_HDMITX_AFMT_HBR = 4
				     /*       */
	} tmdlHdmiTxAudioFormat_t;

/* 
                                                            
 */
	typedef enum {
		TMDL_HDMITX_AFS_32K = 0,
				       /*            */
		TMDL_HDMITX_AFS_44K = 1,
				       /*            */
		TMDL_HDMITX_AFS_48K = 2,
				       /*            */
		TMDL_HDMITX_AFS_88K = 3,
				       /*            */
		TMDL_HDMITX_AFS_96K = 4,
				       /*            */
		TMDL_HDMITX_AFS_176K = 5,
				       /*            */
		TMDL_HDMITX_AFS_192K = 6
				       /*            */
	} tmdlHdmiTxAudioRate_t;

/* 
                                                            
 */
	typedef enum {
		TMDL_HDMITX_I2SQ_16BITS = 16,
					/*           */
		TMDL_HDMITX_I2SQ_32BITS = 32,
					/*           */
		TMDL_HDMITX_I2SQ_OTHERS = 0
					/*                     */
	} tmdlHdmiTxAudioI2SQualifier_t;

/* 
                                                     
 */
	typedef enum {
		TMDL_HDMITX_I2SFOR_PHILIPS_L = 0,
					/*                       */
		TMDL_HDMITX_I2SFOR_OTH_L = 2,
					/*                                    */
		TMDL_HDMITX_I2SFOR_OTH_R = 3,
					/*                                     */
		TMDL_HDMITX_I2SFOR_INVALID = 4
					/*                  */
	} tmdlHdmiTxAudioI2SFormat_t;

/* 
                                                           
 */
	typedef enum {
		TMDL_HDMITX_DSTRATE_SINGLE = 0,
					/*                        */
		TMDL_HDMITX_DSTRATE_DOUBLE = 1
					/*                    */
	} tmdlHdmiTxDstRate_t;

/* 
                                                
 */
	typedef struct {
		tmdlHdmiTxDeviceVersion_t deviceVersion;
						/*                     */
		tmdlHdmiTxHdmiVersion_t hdmiVersion;
						/*                                    */
		tmdlHdmiTxAudioPacket_t audioPacket;
						/*                           */
		tmdlHdmiTxColorDepth_t colorDepth;
						/*                         */
		Bool hdcp;			/*                                          */
		Bool scaler;			    /*                                 */
	} tmdlHdmiTxCapabilities_t;

/* 
                                                           
 */
	typedef struct {
		Bool simplayHd;	/*                            */
		Bool repeaterEnable;
				/*                        */
		UInt8 *pEdidBuffer;
				/*                            */
		UInt32 edidBufferSize;
				/*                                    */
	} tmdlHdmiTxInstanceSetupInfo_t;

/* 
                                                     
 */
	typedef enum {
		TMDL_HDMITX_VFMT_NULL = 0,	/*                                */
		TMDL_HDMITX_VFMT_NO_CHANGE = 0,	/*                                */
		TMDL_HDMITX_VFMT_MIN = 1,	/*                                */
		TMDL_HDMITX_VFMT_TV_MIN = 1,	/*                                */
		TMDL_HDMITX_VFMT_01_640x480p_60Hz = 1,	/*                                */* /4:3 */
		TMDL_HDMITX_VFMT_02_720x480p_60Hz = 2,	/*                                */* /4:3 */
		TMDL_HDMITX_VFMT_03_720x480p_60Hz = 3,	/*                                */* /16:9 */
		TMDL_HDMITX_VFMT_04_1280x720p_60Hz = 4,
						/*                                */
		TMDL_HDMITX_VFMT_05_1920x1080i_60Hz = 5,
						/*                                */
		TMDL_HDMITX_VFMT_06_720x480i_60Hz = 6,
						/*                                */
		TMDL_HDMITX_VFMT_07_720x480i_60Hz = 7,
						/*                                */
		TMDL_HDMITX_VFMT_08_720x240p_60Hz = 8,
						/*                                */
		TMDL_HDMITX_VFMT_09_720x240p_60Hz = 9,
						/*                                */
		TMDL_HDMITX_VFMT_10_720x480i_60Hz = 10,
						/*                                */
		TMDL_HDMITX_VFMT_11_720x480i_60Hz = 11,
						/*                                */
		TMDL_HDMITX_VFMT_12_720x240p_60Hz = 12,
						/*                                */
		TMDL_HDMITX_VFMT_13_720x240p_60Hz = 13,
						/*                                */
		TMDL_HDMITX_VFMT_14_1440x480p_60Hz = 14,
						/*                                */
		TMDL_HDMITX_VFMT_15_1440x480p_60Hz = 15,
						/*                                */
		TMDL_HDMITX_VFMT_16_1920x1080p_60Hz = 16,
						/*                                */
		TMDL_HDMITX_VFMT_17_720x576p_50Hz = 17,
						/*                                */
		TMDL_HDMITX_VFMT_18_720x576p_50Hz = 18,
						/*                                */
		TMDL_HDMITX_VFMT_19_1280x720p_50Hz = 19,
						/*                                */
		TMDL_HDMITX_VFMT_20_1920x1080i_50Hz = 20,
						/*                                */
		TMDL_HDMITX_VFMT_21_720x576i_50Hz = 21,
						/*                                */
		TMDL_HDMITX_VFMT_22_720x576i_50Hz = 22,
						/*                                */
		TMDL_HDMITX_VFMT_23_720x288p_50Hz = 23,
						/*                                */
		TMDL_HDMITX_VFMT_24_720x288p_50Hz = 24,
						/*                                */
		TMDL_HDMITX_VFMT_25_720x576i_50Hz = 25,
						/*                                */
		TMDL_HDMITX_VFMT_26_720x576i_50Hz = 26,
						/*                                */
		TMDL_HDMITX_VFMT_27_720x288p_50Hz = 27,
						/*                                */
		TMDL_HDMITX_VFMT_28_720x288p_50Hz = 28,
						/*                                */
		TMDL_HDMITX_VFMT_29_1440x576p_50Hz = 29,
						/*                                */
		TMDL_HDMITX_VFMT_30_1440x576p_50Hz = 30,
						/*                                */
		TMDL_HDMITX_VFMT_31_1920x1080p_50Hz = 31,
						/*                                */
		TMDL_HDMITX_VFMT_32_1920x1080p_24Hz = 32,
						/*                                */
		TMDL_HDMITX_VFMT_33_1920x1080p_25Hz = 33,
						/*                                */
		TMDL_HDMITX_VFMT_34_1920x1080p_30Hz = 34,
						/*                                */
		TMDL_HDMITX_VFMT_35_2880x480p_60Hz = 35,
						/*                                    */
		TMDL_HDMITX_VFMT_36_2880x480p_60Hz = 36,
						/*                                    */
		TMDL_HDMITX_VFMT_37_2880x576p_50Hz = 37,
						/*                                    */
		TMDL_HDMITX_VFMT_38_2880x576p_50Hz = 38,
						/*                                    */

		TMDL_HDMITX_VFMT_INDEX_60_1280x720p_24Hz = 39,
						  /*                                          */
		TMDL_HDMITX_VFMT_60_1280x720p_24Hz = 60,
						/*                                          */
		TMDL_HDMITX_VFMT_61_1280x720p_25Hz = 61,
						/*                                    */
		TMDL_HDMITX_VFMT_62_1280x720p_30Hz = 62,
						/*                                          */

		TMDL_HDMITX_VFMT_TV_MAX = 62,	/*                                */
		TMDL_HDMITX_VFMT_TV_NO_REG_MIN = 32,
						/*                                             */
		TMDL_HDMITX_VFMT_TV_NUM = 42,	/*                                */

		TMDL_HDMITX_VFMT_PC_MIN = 128,	/*                                */
		TMDL_HDMITX_VFMT_PC_640x480p_60Hz = 128,
						/*                                */
		TMDL_HDMITX_VFMT_PC_800x600p_60Hz = 129,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1152x960p_60Hz = 130,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1024x768p_60Hz = 131,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1280x768p_60Hz = 132,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1280x1024p_60Hz = 133,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1360x768p_60Hz = 134,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1400x1050p_60Hz = 135,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1600x1200p_60Hz = 136,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1024x768p_70Hz = 137,
						/*                                */
		TMDL_HDMITX_VFMT_PC_640x480p_72Hz = 138,
						/*                                */
		TMDL_HDMITX_VFMT_PC_800x600p_72Hz = 139,
						/*                                */
		TMDL_HDMITX_VFMT_PC_640x480p_75Hz = 140,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1024x768p_75Hz = 141,
						/*                                */
		TMDL_HDMITX_VFMT_PC_800x600p_75Hz = 142,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1024x864p_75Hz = 143,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1280x1024p_75Hz = 144,
						/*                                */
		TMDL_HDMITX_VFMT_PC_640x350p_85Hz = 145,
						/*                                */
		TMDL_HDMITX_VFMT_PC_640x400p_85Hz = 146,
						/*                                */
		TMDL_HDMITX_VFMT_PC_720x400p_85Hz = 147,
						/*                                */
		TMDL_HDMITX_VFMT_PC_640x480p_85Hz = 148,
						/*                                */
		TMDL_HDMITX_VFMT_PC_800x600p_85Hz = 149,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1024x768p_85Hz = 150,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1152x864p_85Hz = 151,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1280x960p_85Hz = 152,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1280x1024p_85Hz = 153,
						/*                                */
		TMDL_HDMITX_VFMT_PC_1024x768i_87Hz = 154,
						/*                                */
		TMDL_HDMITX_VFMT_PC_MAX = 154,	/*                                */
		TMDL_HDMITX_VFMT_PC_NUM = (TMDL_HDMITX_VFMT_PC_MAX - TMDL_HDMITX_VFMT_PC_MIN + 1)   /*                                */
	} tmdlHdmiTxVidFmt_t;

/* 
                                                            
 */
	typedef struct {
		tmdlHdmiTxVidFmt_t videoFormat;	/*                                            */
		Bool nativeVideoFormat;	     /*                                                */
	} tmdlHdmiTxShortVidDesc_t;

/* 
                                                                 
 */
	typedef enum {
		TMDL_HDMITX_P_ASPECT_RATIO_UNDEFINED = 0,
						    /*                                  */
		TMDL_HDMITX_P_ASPECT_RATIO_6_5 = 1, /*                                  */
		TMDL_HDMITX_P_ASPECT_RATIO_5_4 = 2, /*           */
		TMDL_HDMITX_P_ASPECT_RATIO_4_3 = 3, /*           */
		TMDL_HDMITX_P_ASPECT_RATIO_16_10 = 4,
						    /*             */
		TMDL_HDMITX_P_ASPECT_RATIO_5_3 = 5, /*           */
		TMDL_HDMITX_P_ASPECT_RATIO_16_9 = 6,/*            */
		TMDL_HDMITX_P_ASPECT_RATIO_9_5 = 7  /*           */
	} tmdlHdmiTxPictAspectRatio_t;

/* 
                                             
 */
	typedef enum {
		TMDL_HDMITX_VFREQ_24Hz = 0,
					/*                 */
		TMDL_HDMITX_VFREQ_25Hz = 1,
					/*                 */
		TMDL_HDMITX_VFREQ_30Hz = 2,
					/*                 */
		TMDL_HDMITX_VFREQ_50Hz = 3,
					/*                 */
		TMDL_HDMITX_VFREQ_59Hz = 4,
					/*                 */
		TMDL_HDMITX_VFREQ_60Hz = 5,
					/*                 */
#ifndef FORMAT_PC
		TMDL_HDMITX_VFREQ_INVALID = 6,
					/*                 */
		TMDL_HDMITX_VFREQ_NUM = 6
					/*                 */
#else				/*           */
		TMDL_HDMITX_VFREQ_70Hz = 6,
					/*                 */
		TMDL_HDMITX_VFREQ_72Hz = 7,
					/*                 */
		TMDL_HDMITX_VFREQ_75Hz = 8,
					/*                 */
		TMDL_HDMITX_VFREQ_85Hz = 9,
					/*                 */
		TMDL_HDMITX_VFREQ_87Hz = 10,
					/*                 */
		TMDL_HDMITX_VFREQ_INVALID = 11,
					/*                 */
		TMDL_HDMITX_VFREQ_NUM = 11
					/*                 */
#endif				/*           */
	} tmdlHdmiTxVfreq_t;

/* 
                                                                
 */
	typedef struct {
		UInt16 width;		       /*                                */
		UInt16 height;		       /*                                 */
		Bool interlaced;	       /*                                */
		tmdlHdmiTxVfreq_t vfrequency;  /*                            */
		tmdlHdmiTxPictAspectRatio_t aspectRatio;
					       /*                              */
	} tmdlHdmiTxVidFmtSpecs_t;

/* 
                                                              
 */
	typedef enum {
		TMDL_HDMITX_VINMODE_CCIR656 = 0,
					/*           */
		TMDL_HDMITX_VINMODE_RGB444,
					/*           */
		TMDL_HDMITX_VINMODE_YUV444,
					/*           */
		TMDL_HDMITX_VINMODE_YUV422,
					/*           */
#ifdef TMFL_RGB_DDR_12BITS
		TMDL_HDMITX_VINMODE_RGB_DDR_12BITS,
					/*                                                               */
#endif
		TMDL_HDMITX_VINMODE_NO_CHANGE,
					/*             */
		TMDL_HDMITX_VINMODE_INVALID
					/*           */
	} tmdlHdmiTxVinMode_t;

/* 
                                                
 */
	typedef enum {
		TMDL_HDMITX_SYNCSRC_EMBEDDED = 0,
				      /*                 */
		TMDL_HDMITX_SYNCSRC_EXT_VREF = 1,
				      /*                                  */
		TMDL_HDMITX_SYNCSRC_EXT_VS = 2
				      /*                        */
	} tmdlHdmiTxSyncSource_t;

/* 
                                                                   
 */
	typedef enum {
		TMDL_HDMITX_PIXRATE_DOUBLE = 0,	    /*                     */
		TMDL_HDMITX_PIXRATE_SINGLE = 1,	    /*                     */
		TMDL_HDMITX_PIXRATE_SINGLE_REPEATED = 2
						    /*                         */
	} tmdlHdmiTxPixRate_t;

/* 
                                                                          
 */
	typedef enum {
		TMDL_HDMITX_3D_NONE = 0,	    /*                             */
		TMDL_HDMITX_3D_FRAME_PACKING = 1,   /*                                         */
		TMDL_HDMITX_3D_TOP_AND_BOTTOM = 2,  /*                                          */
		TMDL_HDMITX_3D_SIDE_BY_SIDE_HALF = 3,
						    /*                                             */
		TMDL_HDMITX_3D_INVALID = 4	    /*           */
	} tmdlHdmiTx3DStructure_t;

/* 
                                                          
 */
	typedef struct {
		tmdlHdmiTxVidFmt_t format;/*                                            */
		tmdlHdmiTxVinMode_t mode; /*                                     */
		tmdlHdmiTxSyncSource_t syncSource;
					  /*                    */
		tmdlHdmiTxPixRate_t pixelRate;
					  /*              */
		tmdlHdmiTx3DStructure_t structure3D;
					  /*                                       */
	} tmdlHdmiTxVideoInConfig_t;

/* 
                                                              
 */
	typedef enum {
		TMDL_HDMITX_VOUTMODE_RGB444 = 0,
					    /*             */
		TMDL_HDMITX_VOUTMODE_YUV422 = 1,
					    /*             */
		TMDL_HDMITX_VOUTMODE_YUV444 = 2,
					     /*             */
		TMDL_HDMITX_VOUTMODE_INVALID = 0xff
	} tmdlHdmiTxVoutMode_t;

/* 
                                                   
 */
	typedef enum {
		TMDL_HDMITX_VQR_DEFAULT = 0,	/*                   */
		TMDL_HDMITX_RGB_FULL = 1,	/*                           */
		TMDL_HDMITX_RGB_LIMITED = 2	/*                              */
	} tmdlHdmiTxVQR_t;


/* 
                                                       
 */
	typedef enum {
		TMDL_HDMITX_YQR_LIMITED = 0,	/*               */
		TMDL_HDMITX_YQR_FULL = 1,	/*            */
		TMDL_HDMITX_YQR_INVALID = 2	/*               */
	} tmdlHdmiTxYCCQR_t;


/* 
                                                           
 */
	typedef struct {
		tmdlHdmiTxVidFmt_t format;
					/*                                            */
		tmdlHdmiTxVoutMode_t mode;
					/*                                     */
		tmdlHdmiTxColorDepth_t colorDepth;
					/*               */
		tmdlHdmiTxVQR_t dviVqr;	/*                           */
	} tmdlHdmiTxVideoOutConfig_t;


	typedef enum {
		TMDL_HDMITX_AUDIO_DATA_PCM = 0,
				       /*                                                     */
		TMDL_HDMITX_AUDIO_DATA_OTHER = 1,
				       /*                                                     */
		TMDL_HDMITX_AUDIO_DATA_INVALID = 2
				       /*                 */
	} tmdlHdmiTxAudioData_t;


	typedef enum {
		TMDL_HDMITX_CSCOPYRIGHT_PROTECTED = 0,
					      /**< Copyright protected     */
		TMDL_HDMITX_CSCOPYRIGHT_UNPROTECTED = 1,
					      /**< Not copyright protected */
		TMDL_HDMITX_CSCOPYRIGHT_INVALID = 2
					      /*                           */
	} tmdlHdmiTxCScopyright_t;

	typedef enum {
		TMDL_HDMITX_CSFI_PCM_2CHAN_NO_PRE = 0,
					       /*                                                         */
		TMDL_HDMITX_CSFI_PCM_2CHAN_PRE = 1,
					       /*                                                 */
		TMDL_HDMITX_CSFI_PCM_2CHAN_PRE_RSVD1 = 2,
					       /*                                                 */
		TMDL_HDMITX_CSFI_PCM_2CHAN_PRE_RSVD2 = 3,
					       /*                                                 */
		TMDL_HDMITX_CSFI_INVALID = 4   /*                                                 */
	} tmdlHdmiTxCSformatInfo_t;


	typedef enum {
		TMDL_HDMITX_CSCLK_LEVEL_II = 0,
					   /*                                */
		TMDL_HDMITX_CSCLK_LEVEL_I = 1,
					   /*                                */
		TMDL_HDMITX_CSCLK_LEVEL_III = 2,
					   /*                                */
		TMDL_HDMITX_CSCLK_NOT_MATCHED = 3,
					   /*                                */
		TMDL_HDMITX_CSCLK_INVALID = 4
					   /*                                */
	} tmdlHdmiTxCSclkAcc_t;


	typedef enum {
		TMDL_HDMITX_CSMAX_LENGTH_20 = 0,
					/*                                */
		TMDL_HDMITX_CSMAX_LENGTH_24 = 1,
					/*                                */
		TMDL_HDMITX_CSMAX_INVALID = 2
					/*                                */
	} tmdlHdmiTxCSmaxWordLength_t;



	typedef enum {
		TMDL_HDMITX_CSWORD_DEFAULT = 0,	 /*                                                   */
		TMDL_HDMITX_CSWORD_20_OF_24 = 1, /*                                                   */
		TMDL_HDMITX_CSWORD_16_OF_20 = 1, /*                                                   */
		TMDL_HDMITX_CSWORD_22_OF_24 = 2, /*                                                   */
		TMDL_HDMITX_CSWORD_18_OF_20 = 2, /*                                                   */
		TMDL_HDMITX_CSWORD_RESVD = 3,	 /*                                */
		TMDL_HDMITX_CSWORD_23_OF_24 = 4, /*                                                   */
		TMDL_HDMITX_CSWORD_19_OF_20 = 4, /*                                                   */
		TMDL_HDMITX_CSWORD_24_OF_24 = 5, /*                                                   */
		TMDL_HDMITX_CSWORD_20_OF_20 = 5, /*                                                   */
		TMDL_HDMITX_CSWORD_21_OF_24 = 6, /*                                                   */
		TMDL_HDMITX_CSWORD_17_OF_20 = 6, /*                                                   */
		TMDL_HDMITX_CSWORD_INVALID = 7	 /*           */
	} tmdlHdmiTxCSwordLength_t;


	typedef enum {
		TMDL_HDMITX_CSOFREQ_NOT_INDICATED = 0,
					     /*                 */
		TMDL_HDMITX_CSOFREQ_192k = 1,/*                 */
		TMDL_HDMITX_CSOFREQ_12k = 2, /*                 */
		TMDL_HDMITX_CSOFREQ_176_4k = 3,
					     /*                 */
		TMDL_HDMITX_CSOFREQ_RSVD1 = 4,
					     /*                 */
		TMDL_HDMITX_CSOFREQ_96k = 5, /*                 */
		TMDL_HDMITX_CSOFREQ_8k = 6,  /*                 */
		TMDL_HDMITX_CSOFREQ_88_2k = 7,
					     /*                 */
		TMDL_HDMITX_CSOFREQ_16k = 8, /*                 */
		TMDL_HDMITX_CSOFREQ_24k = 9, /*                 */
		TMDL_HDMITX_CSOFREQ_11_025k = 10,
					     /*                 */
		TMDL_HDMITX_CSOFREQ_22_05k = 11,
					     /*                 */
		TMDL_HDMITX_CSOFREQ_32k = 12,/*                 */
		TMDL_HDMITX_CSOFREQ_48k = 13,/*                 */
		TMDL_HDMITX_CSOFREQ_RSVD2 = 14,
					     /*                 */
		TMDL_HDMITX_CSOFREQ_44_1k = 15,
					     /*                 */
		TMDL_HDMITX_CSAFS_INVALID = 16
					     /*                 */
	} tmdlHdmiTxCSorigAfs_t;



	typedef struct {
		tmdlHdmiTxAudioData_t PcmIdentification;
		tmdlHdmiTxCScopyright_t CopyrightInfo;
		tmdlHdmiTxCSformatInfo_t FormatInfo;
		UInt8 categoryCode;
		tmdlHdmiTxCSclkAcc_t clockAccuracy;
		tmdlHdmiTxCSmaxWordLength_t maxWordLength;
		tmdlHdmiTxCSwordLength_t wordLength;
		tmdlHdmiTxCSorigAfs_t origSampleFreq;
	} tmdlHdmiTxAudioInChannelStatus;


/* 
                                                          
 */
	typedef struct {
		tmdlHdmiTxAudioFormat_t format;		/*                                   */
		tmdlHdmiTxAudioRate_t rate;		/*                       */
		tmdlHdmiTxAudioI2SFormat_t i2sFormat;	/*                                 */
		tmdlHdmiTxAudioI2SQualifier_t i2sQualifier;
							/*                                                   */
		tmdlHdmiTxDstRate_t dstRate;		/*                          */
		UInt8 channelAllocation;		/*                       */
		tmdlHdmiTxAudioInChannelStatus channelStatus;
							/*                      */
	} tmdlHdmiTxAudioInConfig_t;

/* 
                                           
 */
	typedef enum {
		TMDL_HDMITX_SINK_DVI = 0,
			       /*        */
		TMDL_HDMITX_SINK_HDMI = 1,
			       /*        */
		TMDL_HDMITX_SINK_EDID = 2
			       /*                                */
	} tmdlHdmiTxSinkType_t;

/* 
                                                          
 */
	typedef struct {
		Bool nextField;	/*                                                       */
		UInt8 GBD_Profile;
				/*                                                */
		UInt8 affectedGamutSeqNum;
				/*                                                                                    */
		Bool noCurrentGBD;
				/*                                          */
		UInt8 currentGamutSeqNum;
				/*                                              */
		UInt8 packetSequence;
				/*                                                         */
		UInt8 payload[28];
				/*                               */
	} tmdlHdmiTxGamutData_t;

/* 
                                                       
 */
	typedef UInt8 tmdlHdmiTxGenericPacket[28];

/* 
                                                         
 */
	typedef struct {
		UInt8 acpType;
		UInt8 acpData[28];
	} tmdlHdmiTxAcpPktData_t;

/* 
                                                            
 */
	typedef struct {
		UInt8 colorIndicator;	  /*                                                             */
		UInt8 activeInfoPresent;  /*                                                                                   */
		UInt8 barInformationDataValid;
					  /*                              */
		UInt8 scanInformation;	  /*                                                       */
		UInt8 colorimetry;	  /*                                                  */
		UInt8 pictureAspectRatio; /*                                                           */
		UInt8 activeFormatAspectRatio;
					  /*                                                                              */
		UInt8 nonUniformPictureScaling;
					  /*                                                                   */
		UInt8 videoFormatIdentificationCode;
					  /*                                                                            */
		UInt8 pixelRepetitionFactor;
					  /*                                                               */
		UInt16 lineNumberEndTopBar;
		UInt16 lineNumberStartBottomBar;
		UInt16 lineNumberEndLeftBar;
		UInt16 lineNumberStartRightBar;
	} tmdlHdmiTxAviIfData_t;

/* 
                                                         
 */
	typedef struct {
		Bool avMute;
	} tmdlHdmiTxGcpPktData_t;

/* 
                                                            
 */
	typedef struct {
		UInt8 codingType;
			     /*                                    */
		UInt8 channelCount;
			     /*                                                     */
		UInt8 samplefrequency;
			     /*                                                        */
		UInt8 sampleSize;
			     /*                                                        */
		UInt8 channelAllocation;
			     /*                                                               */
		Bool downmixInhibit;
			     /*                                                            */
		UInt8 levelShiftValue;
			     /*                                                                                          */
	} tmdlHdmiTxAudIfData_t;

/* 
                                                           
 */
	typedef struct {
		Bool isrcCont;
			    /*                                        */
		Bool isrcValid;
			    /*                                                            */
		UInt8 isrcStatus;
			    /*               */
		UInt8 UPC_EAN_ISRC[16];
			    /*                    */
	} tmdlHdmiTxIsrc1PktData_t;

/* 
                                                           
 */
	typedef struct {
		UInt8 UPC_EAN_ISRC[16];
			     /*                    */
	} tmdlHdmiTxIsrc2PktData_t;

/* 
                                                            
 */
	typedef struct {
		UInt32 bitRate;
			    /*                       */
		UInt32 frameType;
			    /*                   */
		Bool fieldRepeat;
			    /*                                  */
	} tmdlHdmiTxMpsIfData_t;

/* 
                                                            
 */
	typedef struct {
		UInt8 vendorName[8];
			     /*               */
		UInt8 productDesc[16];
			     /*                       */
		UInt32 sourceDevInfo;
			     /*                      */
	} tmdlHdmiTxSpdIfData_t;


/* 
                                                                                                 
 */

/*              */
#define TMDL_HDMITX_VERSION            0x01

/*                           */
#define TMDL_HDMITX_VIDEO_FORMAT_SHIFT 5
#define TMDL_HDMITX_FORMAT_EXTENDED    (0x01 << TMDL_HDMITX_VIDEO_FORMAT_SHIFT)
#define TMDL_HDMITX_3D                 (0x02 << TMDL_HDMITX_VIDEO_FORMAT_SHIFT)

/*                                                                           */
#define TMDL_HDMITX_HDMI_IEEE_BYTE0    0x03
#define TMDL_HDMITX_HDMI_IEEE_BYTE1    0x0C
#define TMDL_HDMITX_HDMI_IEEE_BYTE2    0x00

/*                      */
#define TMDL_HDMITX_3D_STRUCTURE_SHIFT 4
#define TMDL_HDMITX_FRAME_PACKING      (0x00 << TMDL_HDMITX_3D_STRUCTURE_SHIFT)
#define TMDL_HDMITX_FIELD_ALTERNATIVE  (0x01 << TMDL_HDMITX_3D_STRUCTURE_SHIFT)
#define TMDL_HDMITX_LINE_ALTERNATIVE   (0x02 << TMDL_HDMITX_3D_STRUCTURE_SHIFT)
#define TMDL_HDMITX_SIDE_BY_SIDE_FULL  (0x03 << TMDL_HDMITX_3D_STRUCTURE_SHIFT)
#define TMDL_HDMITX_L_DEPTH            (0x04 << TMDL_HDMITX_3D_STRUCTURE_SHIFT)
#define TMDL_HDMITX_L_DEPTH_GFX        (0x05 << TMDL_HDMITX_3D_STRUCTURE_SHIFT)
#define TMDL_HDMITX_TOP_AND_BOTTOM     (0x06 << TMDL_HDMITX_3D_STRUCTURE_SHIFT)
#define TMDL_HDMITX_SIDE_BY_SIDE_HALF  (0x08 << TMDL_HDMITX_3D_STRUCTURE_SHIFT)

/*                     */
#define TMDL_HDMITX_3D_EXT_DATA_SHIFT  4
#define TMDL_HDMITX_HORIZONTAL_SUB     (0x00 << TMDL_HDMITX_3D_EXT_DATA_SHIFT)	/*                         */
#define TMDL_HDMITX_QUINCUNX_OLOR      (0x04 << TMDL_HDMITX_3D_EXT_DATA_SHIFT)	/*                                     */
#define TMDL_HDMITX_QUINCUNX_OLER      (0x05 << TMDL_HDMITX_3D_EXT_DATA_SHIFT)	/*                                      */
#define TMDL_HDMITX_QUINCUNX_ELOR      (0x06 << TMDL_HDMITX_3D_EXT_DATA_SHIFT)	/*                                      */
#define TMDL_HDMITX_QUINCUNX_ELER      (0x07 << TMDL_HDMITX_3D_EXT_DATA_SHIFT)	/*                                       */

/*               */
#define TMDL_HDMITX_3D_META_TYPE_SHIFT 5
#define TMDL_HDMITX_3D_META_PRESENT    (0x01 << 3)
#define TMDL_HDMITX_3D_META_PARALLAX   (0x00 << TMDL_HDMITX_3D_META_TYPE_SHIFT)

#define TMDL_HDMITX_VS_PKT_DATA_LEN    27
	typedef struct {
		UInt8 version;
		/*
                                                                     

                                                               
                                                         
        
                                                                   
                                                                   
                                                                   
                                                                   
                                                                   
                                                         
                                                                   
                                                                   
   */
		UInt8 vsData[TMDL_HDMITX_VS_PKT_DATA_LEN];

	} tmdlHdmiTxVsPktData_t;

/* 
                                                                                          
 */
	typedef struct {
		UInt8 maxTmdsClock;	/*                              */
		UInt8 cnc0;	/*                              */
		UInt8 cnc1;	/*                    */
		UInt8 cnc2;	/*                     */
		UInt8 cnc3;	/*                   */
		UInt8 hdmiVideoPresent;	/*                         */
		UInt8 h3DPresent;	/*                             */
		UInt8 h3DMultiPresent;	/*                            */
		UInt8 imageSize;	/*                                                       */
		UInt8 hdmi3DLen;	/*                                  */
		UInt8 hdmiVicLen;	/*                                        */
		UInt8 ext3DData[21];	/*                          */
	} tmdlHdmiTxEdidExtraVsdbData_t;

/* 
                                                      
 */
	typedef struct {
		UInt8 format;	/*                 */
		UInt8 channels;	/*                    */
		UInt8 supportedFreqs;	/*                                  */
		UInt8 supportedRes;	/*                                              */
		UInt8 maxBitrate;	/*                                                           */
	} tmdlHdmiTxEdidAudioDesc_t;

/* 
                                                               
 */
	typedef struct {
		UInt16 pixelClock;
			       /*                              */
		UInt16 hActivePixels;
			       /*                              */
		UInt16 hBlankPixels;
			       /*                              */
		UInt16 vActiveLines;
			       /*                              */
		UInt16 vBlankLines;
			       /*                              */
		UInt16 hSyncOffset;
			       /*                              */
		UInt16 hSyncWidth;
			       /*                              */
		UInt16 vSyncOffset;
			       /*                              */
		UInt16 vSyncWidth;
			       /*                              */
		UInt16 hImageSize;
			       /*                              */
		UInt16 vImageSize;
			       /*                              */
		UInt16 hBorderPixels;
			       /*                              */
		UInt16 vBorderPixels;
			       /*                              */
		UInt8 flags;   /*                              */
	} tmdlHdmiTxEdidVideoTimings_t;

/*                                              */
#define EDID_MONITOR_DESCRIPTOR_SIZE   13

/* 
                                                         
 */
	typedef struct {
		Bool descRecord;			/*                                                  */
		UInt8 monitorName[EDID_MONITOR_DESCRIPTOR_SIZE];
							/*                                                  */
	} tmdlHdmiTxEdidFirstMD_t;

/* 
                                                          
 */
	typedef struct {
		Bool descRecord;    /*                                                  */
		UInt8 minVerticalRate;
				    /*                                                  */
		UInt8 maxVerticalRate;
				    /*                                                  */
		UInt8 minHorizontalRate;
				    /*                                                  */
		UInt8 maxHorizontalRate;
				    /*                                                  */
		UInt8 maxSupportedPixelClk;
				    /*                                                  */
	} tmdlHdmiTxEdidSecondMD_t;

/* 
                                                         
 */
	typedef struct {
		Bool descRecord;			    /*                                                  */
		UInt8 otherDescriptor[EDID_MONITOR_DESCRIPTOR_SIZE];
							    /*                                                  */
	} tmdlHdmiTxEdidOtherMD_t;

/* 
                            
 */
	typedef enum {
		TMDL_HDMITX_PATTERN_OFF = 0,
				     /*                             */
		TMDL_HDMITX_PATTERN_CBAR4 = 1,
				     /*                             */
		TMDL_HDMITX_PATTERN_CBAR8 = 2,
				     /*                             */
		TMDL_HDMITX_PATTERN_BLUE = 3,
				     /*                             */
		TMDL_HDMITX_PATTERN_BLACK = 4,
				     /*                             */
		TMDL_HDMITX_PATTERN_INVALID = 5
				     /*                        */
	} tmdlHdmiTxTestPattern_t;

/* 
                                     
 */
	typedef enum {
		TMDL_HDMITX_HDCP_CHECK_NOT_STARTED = 0,
						     /*                     */
		TMDL_HDMITX_HDCP_CHECK_IN_PROGRESS = 1,
						     /*                           */
		TMDL_HDMITX_HDCP_CHECK_PASS = 2,     /*                          */
		TMDL_HDMITX_HDCP_CHECK_FAIL_FIRST = 3,
						     /*                            */
		TMDL_HDMITX_HDCP_CHECK_FAIL_DRIVER_STATE = 3,
						     /*                            */
		TMDL_HDMITX_HDCP_CHECK_FAIL_DEVICE_T0 = 4,
						     /*                           */
		TMDL_HDMITX_HDCP_CHECK_FAIL_DEVICE_RI = 5,
						     /*                     */
		TMDL_HDMITX_HDCP_CHECK_FAIL_DEVICE_FSM = 6,
						     /*                      */
		TMDL_HDMITX_HDCP_CHECK_NUM = 7	     /*                           */
	} tmdlHdmiTxHdcpCheck_t;

/* 
                                            
 */
	typedef enum {
		TMDL_HDMITX_HDCP_OPTION_FORCE_PJ_IGNORED = 0x01,	/*                             */
		TMDL_HDMITX_HDCP_OPTION_FORCE_SLOW_DDC = 0x02,	/*                             */
		TMDL_HDMITX_HDCP_OPTION_FORCE_NO_1_1 = 0x04,	/*                             */
		TMDL_HDMITX_HDCP_OPTION_FORCE_REPEATER = 0x08,	/*                             */
		TMDL_HDMITX_HDCP_OPTION_FORCE_NO_REPEATER = 0x10,	/*                             */
		TMDL_HDMITX_HDCP_OPTION_FORCE_V_EQU_VBAR = 0x20,	/*                             */
		TMDL_HDMITX_HDCP_OPTION_FORCE_VSLOW_DDC = 0x40,	/*                */
		TMDL_HDMITX_HDCP_OPTION_DEFAULT = 0x00,	/*                            */
		TMDL_HDMITX_HDCP_OPTION_MASK = 0x7F,	/*                             */
		TMDL_HDMITX_HDCP_OPTION_MASK_BAD = 0x80	/*                             */
	} tmdlHdmiTxHdcpOptions_t;

#ifndef NO_HDCP
/*                 */
	typedef enum {
		TMDL_HDMITX_KSV_LIST_MAX_DEVICES = 128,
		TMDL_HDMITX_KSV_BYTES_PER_DEVICE = 5
	} tmdlHdmiTxHdcpHandleSHA_1;

/* 
                                                   
 */
	typedef struct {
		tmdlHdmiTxHdcpCheck_t hdcpCheckState;	/*                  */
		UInt8 hdcpErrorState;	/*                             */
		Bool bKsvSecure;	/*                 */
		UInt8 hdcpBksv[TMDL_HDMITX_KSV_BYTES_PER_DEVICE];	/*                       */
		UInt8 hdcpKsvList[TMDL_HDMITX_KSV_BYTES_PER_DEVICE * TMDL_HDMITX_KSV_LIST_MAX_DEVICES];	/*                                 
                                */
		UInt8 hdcpKsvDevices;	/*                            
                                      */
		UInt8 hdcpDeviceDepth;	/*                       */
		Bool hdcpMaxCascExceeded;
		Bool hdcpMaxDevsExceeded;
	} tmdlHdmiTxHdcpInfo_t;
#endif				/*         */

/* 
                                     
 */
	typedef enum {
		TMDL_HDMITX_HDCP_OK = 0,
		TMDL_HDMITX_HDCP_BKSV_RCV_FAIL,	/*                                    */
		TMDL_HDMITX_HDCP_BKSV_CHECK_FAIL,	/*                                            */
		TMDL_HDMITX_HDCP_BCAPS_RCV_FAIL,	/*                                    */
		TMDL_HDMITX_HDCP_AKSV_SEND_FAIL,	/*                           */
		TMDL_HDMITX_HDCP_R0_RCV_FAIL,	/*                             */
		TMDL_HDMITX_HDCP_R0_CHECK_FAIL,	/*                     */
		TMDL_HDMITX_HDCP_BKSV_NOT_SECURE,
		TMDL_HDMITX_HDCP_RI_RCV_FAIL,	/*                             */
		TMDL_HDMITX_HDCP_RPT_RI_RCV_FAIL,	/*                                           */
		TMDL_HDMITX_HDCP_RI_CHECK_FAIL,	/*                     */
		TMDL_HDMITX_HDCP_RPT_RI_CHECK_FAIL,	/*                                   */
		TMDL_HDMITX_HDCP_RPT_BCAPS_RCV_FAIL,	/*                                                  */
		TMDL_HDMITX_HDCP_RPT_BCAPS_READY_TIMEOUT,
		TMDL_HDMITX_HDCP_RPT_V_RCV_FAIL,	/*                            */
		TMDL_HDMITX_HDCP_RPT_BSTATUS_RCV_FAIL,	/*                                               */
		TMDL_HDMITX_HDCP_RPT_KSVLIST_RCV_FAIL,	/*                                                   */
		TMDL_HDMITX_HDCP_RPT_KSVLIST_NOT_SECURE,
		TMDL_HDMITX_HDCP_UNKNOWN_STATUS
	} tmdlHdmiTxHdcpStatus_t;


/* 
                                             
 */
	typedef struct {
		Bool latency_available;
		Bool Ilatency_available;
		UInt8 Edidvideo_latency;
		UInt8 Edidaudio_latency;
		UInt8 EdidIvideo_latency;
		UInt8 EdidIaudio_latency;

	} tmdlHdmiTxEdidLatency_t;


/* 
                                               
 */
	typedef enum {
		TMDL_HDMITX_HOTPLUG_INACTIVE = 0,
					    /*                    */
		TMDL_HDMITX_HOTPLUG_ACTIVE = 1,
					    /*                    */
		TMDL_HDMITX_HOTPLUG_INVALID = 2
					    /*                    */
	} tmdlHdmiTxHotPlug_t;


/* 
                                               
 */
	typedef enum {
		TMDL_HDMITX_RX_SENSE_INACTIVE = 0,
					     /*                    */
		TMDL_HDMITX_RX_SENSE_ACTIVE = 1,
					     /*                    */
		TMDL_HDMITX_RX_SENSE_INVALID = 2
					     /*                    */
	} tmdlHdmiTxRxSense_t;


/* 
                                                               
 */
	typedef enum {
		TMDL_HDMITX_EXT_COLORIMETRY_XVYCC601 = 0,
		TMDL_HDMITX_EXT_COLORIMETRY_XVYCC709 = 1,
		TMDL_HDMITX_EXT_COLORIMETRY_SYCC601 = 2,
		TMDL_HDMITX_EXT_COLORIMETRY_ADOBEYCC601 = 3,
		TMDL_HDMITX_EXT_COLORIMETRY_ADOBERGB = 4,
		TMDL_HDMITX_EXT_COLORIMETRY_INVALID = 5
	} tmdlHdmiTxExtColorimetry_t;

#ifdef __cplusplus
}
#endif
#endif				/*                    */
/*                                                                            *//*                                                                            *//*                                                                            */
