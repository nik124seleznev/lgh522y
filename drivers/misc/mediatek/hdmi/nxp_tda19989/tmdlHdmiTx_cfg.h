/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiTx_cfg.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 08/08/07 11:00 $
 *
 * \brief         devlib driver component API for the TDA998x HDMI Transmitters
 *
 * \section refs  Reference Documents
 * HDMI Tx Driver - FRS.doc,
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmbslHdmiTx_cfg.h $
 *
 * *****************  Version 1  *****************
 * User: J. Lamotte Date: 08/08/07  Time: 11:00
 * initial version
 *

   \endverbatim
 *
*/

/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */

#ifndef TMDLHDMITX_CFG_H
#define TMDLHDMITX_CFG_H

#include "tmNxTypes.h"
#include "tmbslHdmiTx_types.h"
#include "tmdlHdmiTx_Types.h"


#ifdef __cplusplus
extern "C" {
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */

/* 
                                                                              
 */
	typedef enum {
		TMDL_HDMITX_VID444_GY_4_TO_7 = 0x00,
						/*                                 */
		TMDL_HDMITX_VID444_GY_0_TO_3 = 0x01,
						/*                                 */
		TMDL_HDMITX_VID444_BU_4_TO_7 = 0x02,
						/*                                 */
		TMDL_HDMITX_VID444_BU_0_TO_3 = 0x03,
						/*                                 */
		TMDL_HDMITX_VID444_VR_4_TO_7 = 0x04,
						/*                                 */
		TMDL_HDMITX_VID444_VR_0_TO_3 = 0x05,
						/*                                 */
		TMDL_HDMITX_VID444_GY_7_TO_4 = 0x80,
						/*                                            */
		TMDL_HDMITX_VID444_GY_3_TO_0 = 0x81,
						/*                                            */
		TMDL_HDMITX_VID444_BU_7_TO_4 = 0x82,
						/*                                            */
		TMDL_HDMITX_VID444_BU_3_TO_0 = 0x83,
						/*                                            */
		TMDL_HDMITX_VID444_VR_7_TO_4 = 0x84,
						/*                                            */
		TMDL_HDMITX_VID444_VR_3_TO_0 = 0x85,
						/*                                            */
		TMDL_HDMITX_VID444_NOT_CONNECTED = 0x100
						/*                       */
	} tmdlHdmiTxCfgVideoSignal444;

/* 
                                                                                      
 */
	typedef enum {
		TMDL_HDMITX_VID422_Y_8_TO_11 = 0x00,
						/*                                  */
		TMDL_HDMITX_VID422_Y_4_TO_7 = 0x01,
						/*                                 */
		TMDL_HDMITX_VID422_Y_0_TO_3 = 0x02,
						/*                                 */
		TMDL_HDMITX_VID422_UV_8_TO_11 = 0x03,
						/*                                  */
		TMDL_HDMITX_VID422_UV_4_TO_7 = 0x04,
						/*                                 */
		TMDL_HDMITX_VID422_UV_0_TO_3 = 0x05,
						/*                                 */
		TMDL_HDMITX_VID422_Y_11_TO_8 = 0x80,
						/*                                             */
		TMDL_HDMITX_VID422_Y_7_TO_4 = 0x81,
						/*                                            */
		TMDL_HDMITX_VID422_Y_3_TO_0 = 0x82,
						/*                                            */
		TMDL_HDMITX_VID422_UV_11_TO_8 = 0x83,
						/*                                             */
		TMDL_HDMITX_VID422_UV_7_TO_4 = 0x84,
						/*                                            */
		TMDL_HDMITX_VID422_UV_3_TO_0 = 0x85,
						/*                                            */
		TMDL_HDMITX_VID422_NOT_CONNECTED = 0x100
						/*                       */
	} tmdlHdmiTxCfgVideoSignal422;

/* 
                                                                                     
 */
	typedef enum {
		TMDL_HDMITX_VIDCCIR_8_TO_11 = 0x00,
						/*                                   */
		TMDL_HDMITX_VIDCCIR_4_TO_7 = 0x01,
						/*                                  */
		TMDL_HDMITX_VIDCCIR_0_TO_3 = 0x02,
						/*                                  */
		TMDL_HDMITX_VIDCCIR_11_TO_8 = 0x80,
						/*                                              */
		TMDL_HDMITX_VIDCCIR_7_TO_4 = 0x81,
						/*                                             */
		TMDL_HDMITX_VIDCCIR_3_TO_0 = 0x82,
						/*                                             */
		TMDL_HDMITX_VIDCCIR_NOT_CONNECTED = 0x100
						/*                       */
	} tmdlHdmiTxCfgVideoSignalCCIR656;

#ifdef TMFL_RGB_DDR_12BITS
/* 
                                                                                     
 */
	typedef enum {
		TMDL_HDMITX_VID_B_0_3_G_4_7 = 0x00,
						 /*                                              */
		TMDL_HDMITX_VID_B_4_7_R_0_3 = 0x01,
						 /*                                            */
		TMDL_HDMITX_VID_G_0_3_R_4_7 = 0x02,
						 /*                                             */
		TMDL_HDMITX_VID_DDR_NOT_CONNECTED = 0x100
						 /*                       */
	} tmdlHdmiTxCfgVideoSignal_RGB_DDR_12bits;

/* 
                                                                                   
                                                                                             
                                                                                               
 */
	typedef enum {
		VIP_MUX_R_B = 0x00,		 /*                           */
		VIP_MUX_R_G = 0x10,		 /*                            */
		VIP_MUX_R_R = 0x20,		 /*                          */
		VIP_MUX_G_B = 0x00,		 /*                           */
		VIP_MUX_G_G = 0x04,		 /*                            */
		VIP_MUX_G_R = 0x08,		 /*                          */
		VIP_MUX_B_B = 0x00,		 /*                           */
		VIP_MUX_B_G = 0x01,		 /*                            */
		VIP_MUX_B_R = 0x02,		 /*                          */
	} tmdlHdmiTxCfgVideoSignal_VIP_OUTPUT_MUX;
#endif

/*                                                                            */
#define ENABLE_ALL_AUDIO_PORT       0xFF
/*                                */
#define ENABLE_AUDIO_CLOCK_PORT     1
#define DISABLE_AUDIO_CLOCK_PORT    0
/*                                                      */
#define DISABLE_ALL_AUDIO_PORT_PULLDOWN 0x00
/*                                         */
#define ENABLE_AUDIO_CLOCK_PORT_PULLDOWN    1
#define DISABLE_AUDIO_CLOCK_PORT_PULLDOWN   0

/* 
                                         
 */
	typedef struct _tmdlHdmiTxCfgResolution_t {
		tmdlHdmiTxVidFmt_t resolutionID;
		UInt16 width;
		UInt16 height;
		Bool interlaced;
		tmdlHdmiTxVfreq_t vfrequency;
		tmdlHdmiTxPictAspectRatio_t aspectRatio;
	} tmdlHdmiTxCfgResolution_t, *ptmdlHdmiTxCfgResolution_t;

/* 
                                                          
 */
	typedef struct {
		UInt8 commandTaskPriority;
		UInt8 commandTaskStackSize;
		UInt8 commandTaskQueueSize;
		UInt8 hdcpTaskPriority;
		UInt8 hdcpTaskStackSize;
		UInt8 i2cAddress;
		ptmbslHdmiTxSysFunc_t i2cReadFunction;
		ptmbslHdmiTxSysFunc_t i2cWriteFunction;
		ptmdlHdmiTxCfgResolution_t pResolutionInfo;
		UInt8 *pMirrorTableCCIR656;
		UInt8 *pSwapTableCCIR656;
		UInt8 *pEnableVideoPortCCIR656;
		UInt8 *pGroundVideoPortCCIR656;
		UInt8 *pMirrorTableYUV422;
		UInt8 *pSwapTableYUV422;
		UInt8 *pEnableVideoPortYUV422;
		UInt8 *pGroundVideoPortYUV422;
		UInt8 *pMirrorTableYUV444;
		UInt8 *pSwapTableYUV444;
		UInt8 *pEnableVideoPortYUV444;
		UInt8 *pGroundVideoPortYUV444;
		UInt8 *pMirrorTableRGB444;
		UInt8 *pSwapTableRGB444;
		UInt8 *pEnableVideoPortRGB444;
		UInt8 *pGroundVideoPortRGB444;
#ifdef TMFL_RGB_DDR_12BITS
		UInt8 *pMirrorTableRGB_DDR_12bits;
		UInt8 *pSwapTableRGB_DDR_12bits;
		UInt8 *pNoMux;
		UInt8 *pMux_RGB_DDR_12bits;
		UInt8 *pEnableVideoPortRGB_DDR_12bits;
		UInt8 *pGroundVideoPortRGB_DDR_12bits;
#endif
		UInt8 *pEnableAudioPortSPDIF;
		UInt8 *pGroundAudioPortSPDIF;
		UInt8 *pEnableAudioClockPortSPDIF;
		UInt8 *pGroundAudioClockPortSPDIF;
		UInt8 *pEnableAudioPortI2S;
		UInt8 *pGroundAudioPortI2S;
		UInt8 *pEnableAudioPortI2S8C;
		UInt8 *pGroundAudioPortI2S8C;
		UInt8 *pEnableAudioClockPortI2S;
		UInt8 *pGroundAudioClockPortI2S;
		UInt8 *pEnableAudioPortOBA;
		UInt8 *pGroundAudioPortOBA;
		UInt8 *pEnableAudioClockPortOBA;
		UInt8 *pGroundAudioClockPortOBA;
		UInt8 *pEnableAudioPortDST;
		UInt8 *pGroundAudioPortDST;
		UInt8 *pEnableAudioClockPortDST;
		UInt8 *pGroundAudioClockPortDST;
		UInt8 *pEnableAudioPortHBR;
		UInt8 *pGroundAudioPortHBR;
		UInt8 *pEnableAudioClockPortHBR;
		UInt8 *pGroundAudioClockPortHBR;
		UInt16 keySeed;
		tmdlHdmiTxTestPattern_t pattern;
		UInt8 dataEnableSignalAvailable;	/*                                      */
	} tmdlHdmiTxDriverConfigTable_t;

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                           */
#define MAX_UNITS 1

#ifdef TMFL_OS_WINDOWS		/*            */
	extern tmdlHdmiTxCfgVideoSignalCCIR656 videoPortMapping_CCIR656[MAX_UNITS][6];
	extern tmdlHdmiTxCfgVideoSignal422 videoPortMapping_YUV422[MAX_UNITS][6];
	extern tmdlHdmiTxCfgVideoSignal444 videoPortMapping_YUV444[MAX_UNITS][6];
	extern tmdlHdmiTxCfgVideoSignal444 videoPortMapping_RGB444[MAX_UNITS][6];
#ifdef TMFL_RGB_DDR_12BITS
	extern tmdlHdmiTxCfgVideoSignal_RGB_DDR_12bits
	    VideoPortMapping_RGB_DDR_12bits[MAX_UNITS][6];
	extern UInt8 VideoPortMux_RGB_DDR_12bits[MAX_UNITS];
	extern UInt8 VideoPortNoMux[MAX_UNITS];
#endif
#else				/*                 */
	extern const tmdlHdmiTxCfgVideoSignalCCIR656 videoPortMapping_CCIR656[MAX_UNITS][6];
	extern const tmdlHdmiTxCfgVideoSignal422 videoPortMapping_YUV422[MAX_UNITS][6];
	extern const tmdlHdmiTxCfgVideoSignal444 videoPortMapping_YUV444[MAX_UNITS][6];
	extern const tmdlHdmiTxCfgVideoSignal444 videoPortMapping_RGB444[MAX_UNITS][6];
#ifdef TMFL_RGB_DDR_12BITS
	extern const tmdlHdmiTxCfgVideoSignal_RGB_DDR_12bits
	    VideoPortMapping_RGB_DDR_12bits[MAX_UNITS][6];
	extern const UInt8 VideoPortMux_RGB_DDR_12bits[MAX_UNITS];
	extern const UInt8 VideoPortNoMux[MAX_UNITS];
#endif
#endif				/*                 */

/*                                                                            */
/*                                                                            */
/*                                                                            */
	extern tmdlHdmiTxDriverConfigTable_t driverConfigTableTx[MAX_UNITS];


/*                                                                            */
/*                                                                            */
/*                                                                            */
#ifdef TMFL_CEC_AVAILABLE

#include "tmdlHdmiCEC_Types.h"

	typedef struct {
		UInt8 commandTaskPriority;
		UInt8 commandTaskStackSize;
		UInt8 commandTaskQueueSize;
		UInt8 i2cAddress;
		ptmdlHdmiCecSysFunc_t i2cReadFunction;
		ptmdlHdmiCecSysFunc_t i2cWriteFunction;
		tmdlHdmiCecCapabilities_t *pCapabilitiesList;
	} tmdlHdmiCecDriverConfigTable_t;

	tmErrorCode_t tmdlHdmiCecCfgGetConfig
	    (tmUnitSelect_t unit, tmdlHdmiCecDriverConfigTable_t *pConfig);
#endif

#ifdef __cplusplus
}
#endif
#endif				/*                  */
/*                                                                            *//*                                                                            *//*                                                                            */
