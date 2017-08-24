/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiTx.c
 *
 * \version       Revision: 1
 *
 * \date          Date: 10/08/07 10:00
 *
 * \brief         devlib driver component API for the TDA998x HDMI Transmitters
 *
 * \section refs  Reference Documents
 * HDMI Tx Driver - FRS.doc,
 *
 * \section info  Change Information
 *
 * \verbatim

   History:       tmdlHdmiTx.c
 *
 * *****************  Version 1  *****************
 * User: J. Lamotte Date: 10/08/07   Time: 10:00
 * Updated in $/Source/tmdlHdmiTx/inc
 * initial version

   \endverbatim
 *
*/

/*                                                                            */
/*                                                                            */
/*                                                                            */
#ifndef TMFL_TDA19989
#define TMFL_TDA19989
#endif

#ifndef TMFL_NO_RTOS
#define TMFL_NO_RTOS
#endif

#ifndef TMFL_LINUX_OS_KERNEL_DRIVER
#define TMFL_LINUX_OS_KERNEL_DRIVER
#endif


#include "tmdlHdmiTx_IW.h"
#include "tmdlHdmiTx.h"
#include "tmdlHdmiTx_local.h"
#include "tmdlHdmiTx_cfg.h"
#include "tmbslHdmiTx_funcMapping.h"

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                     */
#ifdef TMFL_OS_WINDOWS
#define DUMMY_ACCESS(x) x
#else
#define DUMMY_ACCESS(x)
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */



/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                  */
/*                */
#ifndef TMFL_NO_RTOS
static void CommandTaskUnit0(void);
static void HdcpTaskUnit0(void);
#endif				/*              */

/*                              */
static void dlHdmiTxHandleENCRYPT(tmInstance_t instance);
static void dlHdmiTxHandleHPD(tmInstance_t instance);
static void dlHdmiTxHandleT0(tmInstance_t instance);
static void dlHdmiTxHandleBCAPS(tmInstance_t instance);
static void dlHdmiTxHandleBSTATUS(tmInstance_t instance);
static void dlHdmiTxHandleSHA_1(tmInstance_t instance);
static void dlHdmiTxHandlePJ(tmInstance_t instance);
static void dlHdmiTxHandleR0(tmInstance_t instance);
static void dlHdmiTxHandleSW_INT(tmInstance_t instance);
static void dlHdmiTxHandleRX_SENSE(tmInstance_t instance);
static void dlHdmiTxHandleEDID_READ(tmInstance_t instance);
static void dlHdmiTxHandleVS_RPT(tmInstance_t instance);

/*                                                */
#ifndef NO_HDCP
static void dlHdmiTxCheckColorBar(tmInstance_t instance);
static void dlHdmiTxCheckHdcpColorBar(tmInstance_t instance);
#endif

#ifndef NO_HDCP
static void dlHdmiTxFindHdcpSeed(tmInstance_t instance);
#endif				/*         */

/*                                         */
static void dlHdmiTxSetState(tmInstance_t instance, tmdlHdmiTxDriverState_t state);

/*                                                           
                               */
static tmdlHdmiTxEventStatus_t dlHdmiTxGetEventStatus
    (tmInstance_t instance, tmdlHdmiTxEvent_t event);

/*                                                */
static Bool dlHdmiTxGetReflineRefpix
    (tmdlHdmiTxVidFmt_t vinFmt,
     tmdlHdmiTxVinMode_t vinMode,
     tmdlHdmiTxVidFmt_t voutFmt,
     UInt8 syncIn,
     tmdlHdmiTxPixRate_t pixRate,
     UInt16 *pRefPix,
     UInt16 *pRefLine, UInt16 *pScRefPix, UInt16 *pScRefLine, Bool *pbVerified);

/*                                                      */
static tmErrorCode_t dlHdmiTxSetVideoInfoframe
    (tmInstance_t instance, tmdlHdmiTxVidFmt_t voutFmt, tmdlHdmiTxVoutMode_t voutMode);

/*                                        */
static tmErrorCode_t dlHdmiTxSetRawVideoInfoframe
    (tmInstance_t instance, tmdlHdmiTxAviIfData_t *pContentVif, Bool enable);

/*                                   */
static UInt8 dlHdmiTxcalculateCheksumIF(tmbslHdmiTxPktRawAvi_t *pData	/*                          */
    );

/*                                                                         
                                                                          
                                                           */

/*                              */
tmdlHdmiTxDriverState_t dlHdmiTxGetState(tmInstance_t instance);

/*                                           */
tmErrorCode_t dlHdmiTxSetTestPatternOn
    (tmInstance_t instance,
     tmdlHdmiTxVidFmt_t voutFmt, tmdlHdmiTxVoutMode_t voutMode, tmdlHdmiTxTestPattern_t pattern);

/*                 */
tmErrorCode_t dlHdmiTxSetTestPatternOff
    (tmInstance_t instance, tmdlHdmiTxVidFmt_t voutFmt, tmdlHdmiTxVoutMode_t voutMode);

/*                  */
static tmErrorCode_t dlHdmiTxEdidGetDTD
    (tmInstance_t instance,
     tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors, UInt8 maxDTDesc, UInt8 *pWrittenDTDesc);

static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_640HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors);

static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_720HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors, tmdlHdmiTxPictAspectRatio_t pictureAspectRatio);

static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_1280HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors);

static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_1920HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors, Bool formatInterlaced);

static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_1440HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors,
     tmdlHdmiTxPictAspectRatio_t pictureAspectRatio, Bool formatInterlaced);

static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_2880HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors,
     tmdlHdmiTxPictAspectRatio_t pictureAspectRatio, Bool formatInterlaced);

static tmdlHdmiTxPictAspectRatio_t dlHdmiTxCalcAspectRatio(UInt16 HImageSize, UInt16 VImageSize);

#ifndef NO_HDCP
static void dlHdmiTxCheckHdcpBksv
    (tmInstance_t instance, UInt8 *pHdcpBksvTested, Bool *pbBksvSecure, Bool bBigEndian);
#endif

/*                                                       */
static tmdlHdmiTxVidFmt_t dlHdmiTxCalcVidFmtIndex(tmdlHdmiTxVidFmt_t vidFmt);

extern tmErrorCode_t tmbslDebugWriteFakeRegPage(tmUnitSelect_t txUnit);

/*                                                                            */
/*                                                                            */
/*                                                                            */

tmdlHdmiTxIWSemHandle_t dlHdmiTxItSemaphore[MAX_UNITS];

/*                                                                    */
unitConfig_t unitTableTx[MAX_UNITS] = {
	{
	 False,
	 False,
	 (tmdlHdmiTxHdcpOptions_t) HDCP_OPT_DEFAULT,
	 False,
	 False,
	 TMDL_HDMITX_DEVICE_UNKNOWN,
	 0,
	 0,
	 (tmdlHdmiTxIWTaskHandle_t) 0,
	 (tmdlHdmiTxIWQueueHandle_t) 0,
	 (tmdlHdmiTxIWTaskHandle_t) 0,
	 STATE_NOT_INITIALIZED,
	 (ptmdlHdmiTxCallback_t) 0,
	 {Null, 0,},
	 }
};

#ifndef TMFL_NO_RTOS

tmdlHdmiTxIWFuncPtr_t commandTaskTableTx[MAX_UNITS] = {
	CommandTaskUnit0
};

tmdlHdmiTxIWFuncPtr_t hdcpTaskTableTx[MAX_UNITS] = {
	HdcpTaskUnit0
};

#endif				/*              */

tmbslHdmiTxCallbackList_t callbackFuncTableTx;

/*                                                                           
                                         */
tmdlHdmiTxDriverConfigTable_t gtmdlHdmiTxDriverConfigTable[MAX_UNITS] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TMDL_HDMITX_PATTERN_OFF, 0},
};

/*                                       */
tmdlHdmiTxVideoInfo_t videoInfoListTx = {
	False,
	{TMDL_HDMITX_VFMT_03_720x480p_60Hz, TMDL_HDMITX_VINMODE_YUV422, TMDL_HDMITX_SYNCSRC_EXT_VS,
	 TMDL_HDMITX_PIXRATE_SINGLE, TMDL_HDMITX_3D_NONE},
	{TMDL_HDMITX_VFMT_03_720x480p_60Hz, TMDL_HDMITX_VOUTMODE_YUV422, TMDL_HDMITX_COLORDEPTH_24,
	 TMDL_HDMITX_VQR_DEFAULT}
};

/*                                       */
tmdlHdmiTxAudioInfo_t audioInfoListTx = {
	False,
	{TMDL_HDMITX_AFMT_SPDIF, TMDL_HDMITX_AFS_48K, TMDL_HDMITX_I2SFOR_PHILIPS_L,
	 TMDL_HDMITX_I2SQ_16BITS, TMDL_HDMITX_DSTRATE_SINGLE, 0x00}
};

/*                                        */
tmdlHdmiTxEventState_t eventStateListTx[EVENT_NB] = {
	{TMDL_HDMITX_HDCP_ACTIVE, TMDL_HDMITX_EVENT_DISABLED},
	{TMDL_HDMITX_HDCP_INACTIVE, TMDL_HDMITX_EVENT_DISABLED},
	{TMDL_HDMITX_HPD_ACTIVE, TMDL_HDMITX_EVENT_DISABLED},
	{TMDL_HDMITX_HPD_INACTIVE, TMDL_HDMITX_EVENT_DISABLED},
	{TMDL_HDMITX_RX_KEYS_RECEIVED, TMDL_HDMITX_EVENT_DISABLED},
	{TMDL_HDMITX_RX_DEVICE_ACTIVE, TMDL_HDMITX_EVENT_DISABLED},
	{TMDL_HDMITX_RX_DEVICE_INACTIVE, TMDL_HDMITX_EVENT_DISABLED},
	{TMDL_HDMITX_EDID_RECEIVED, TMDL_HDMITX_EVENT_DISABLED},
	{TMDL_HDMITX_VS_RPT_RECEIVED, TMDL_HDMITX_EVENT_DISABLED}
#ifdef HDMI_TX_REPEATER_ISR_MODE
	, {TMDL_HDMITX_B_STATUS, TMDL_HDMITX_EVENT_DISABLED}
#endif				/*                           */
};

/*                                             */
tmdlHdmiTxColBarState_t colorBarStateTx = {
	False,
	True,
	True,
	False,
	False,
	True,
	False
};

tmdlHdmiTxGamutState_t gamutStateTx = {
	False,
	0,
	TMDL_HDMITX_EXT_COLORIMETRY_XVYCC601,
	False,
	TMDL_HDMITX_YQR_LIMITED
};


/*                                                 */
instanceStatus_t instanceStatusInfoTx[MAX_UNITS] = {
	{(ptmdlHdmiTxVideoInfo_t) &videoInfoListTx,
	 (ptmdlHdmiTxAudioInfo_t) &audioInfoListTx,
	 (ptmdlHdmiTxEventState_t) eventStateListTx,
	 (ptmdlHdmiTxColBarState_t) &colorBarStateTx,
	 (ptmdlHdmiTxGamutState_t) &gamutStateTx}
};

/*                                                                                 
                                                                                           */
#define SEED_TABLE_LEN 10
static const UInt16 kSeedTable[SEED_TABLE_LEN][2] = {
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0},
	{0, 0}
};

#ifndef NO_HDCP
tmdlHdmiTxHdcpInfo_t hdcpInfoListTx[MAX_UNITS];
#endif				/*         */


static Bool gI2CDebugAccessesEnabled = True;	/*                                                                */

#ifdef HDMI_TX_REPEATER_ISR_MODE
static Bool gIgnoreNextSha1 = False;
#endif				/*                          */

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                             
                                                  

                                                       

                            
                                     
                                                                   
                   

                                                                             */
tmErrorCode_t tmdlHdmiTxGetSWVersion(tmSWVersion_t *pSWVersion) {
	/*                                    */
	RETIF(pSWVersion == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                 */
	    pSWVersion->compatibilityNr = VERSION_COMPATIBILITY;
	pSWVersion->majorVersionNr = VERSION_MAJOR;
	pSWVersion->minorVersionNr = VERSION_MINOR;

	return TM_OK;
}

/*                                                                             
                                                                               
                                                 

                                                               

                            
                                     
                                                                   
                   

                                                                             */
tmErrorCode_t tmdlHdmiTxGetNumberOfUnits(UInt32 *pUnitCount) {
	/*                                    */
	RETIF(pUnitCount == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                                  */
	    * pUnitCount = MAX_UNITS;

	return TM_OK;
}

/*                                                                             
                                                                         
                                                                 

                                                               

                            
                                     
                                                                     
                                               
                                                                   
                   
                                                                     
               
                                                                  
                                                                 
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetCapabilities(tmdlHdmiTxCapabilities_t *pCapabilities) {
	/*                                                                          */
	return (tmdlHdmiTxGetCapabilitiesM((tmUnitSelect_t) 0, pCapabilities));
}

/*                                                                             
                                                                           
                                                                
           

                                           
                                                               

                            
                                     
                                                                     
                                               
                                                                   
                   
                                                                     
               
                                                                  
                                                                 
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetCapabilitiesM
    (tmUnitSelect_t unit, tmdlHdmiTxCapabilities_t *pCapabilities) {
	tmErrorCode_t errCode = TM_OK;
	Bool featureSupported;

	/*                                  */
	RETIF((unit < 0) || (unit >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_UNIT_NUMBER)

	    /*                                     */
	    RETIF(pCapabilities == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                */
	    pCapabilities->deviceVersion = unitTableTx[unit].deviceVersion;

	/*                                              */

	/*              */
	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_HDCP,
						      &featureSupported)) != TM_OK, errCode)

	    pCapabilities->hdcp = featureSupported;

	/*                */
	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_SCALER,
						      &featureSupported)) != TM_OK, errCode)

	    pCapabilities->scaler = featureSupported;

	/*                   */
	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_AUDIO_HBR,
						      &featureSupported)) != TM_OK, errCode)

	    pCapabilities->audioPacket.HBR = featureSupported;

	/*                   */
	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_AUDIO_OBA,
						      &featureSupported)) != TM_OK, errCode)

	    pCapabilities->audioPacket.oneBitAudio = featureSupported;

	/*                   */
	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_AUDIO_DST,
						      &featureSupported)) != TM_OK, errCode)

	    pCapabilities->audioPacket.DST = featureSupported;

	/*                          */
	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_HDMI_1_1,
						      &featureSupported)) != TM_OK, errCode)

	    if (featureSupported) {
		pCapabilities->hdmiVersion = TMDL_HDMITX_HDMI_VERSION_1_1;
	}

	/*                           */
	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_HDMI_1_2A,
						      &featureSupported)) != TM_OK, errCode)

	    if (featureSupported) {
		pCapabilities->hdmiVersion = TMDL_HDMITX_HDMI_VERSION_1_2a;
	}

	/*                          */
	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_HDMI_1_3A,
						      &featureSupported)) != TM_OK, errCode)

	    if (featureSupported) {
		pCapabilities->hdmiVersion = TMDL_HDMITX_HDMI_VERSION_1_3a;
	}

	/*                    */
	/*            */
	pCapabilities->colorDepth = TMDL_HDMITX_COLORDEPTH_24;

	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_DEEP_COLOR_30,
						      &featureSupported)) != TM_OK, errCode)

	    if (featureSupported) {
		pCapabilities->colorDepth = TMDL_HDMITX_COLORDEPTH_30;
	}

	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_DEEP_COLOR_36,
						      &featureSupported)) != TM_OK, errCode)

	    if (featureSupported) {
		pCapabilities->colorDepth = TMDL_HDMITX_COLORDEPTH_36;
	}

	RETIF((errCode = tmbslHdmiTxHwGetCapabilities(unit,
						      HDMITX_FEATURE_HW_DEEP_COLOR_48,
						      &featureSupported)) != TM_OK, errCode)

	    if (featureSupported) {
		pCapabilities->colorDepth = TMDL_HDMITX_COLORDEPTH_48;
	}

	return errCode;
}

/*                                                                             
                                                                           
                                                                    
                                                                       

                                                                           
                  

                            
                                     
                                                                     
                                                  
                                                               
                                                                   
                   
                                                                       
                                                                
                   
                                                                  
                                                              
                                                                    
                                                                  
                                                                  
                                                                  
               
                                                               
                  
                                                                  
                                            
                                                                   
                                                               

                                                                             */
tmErrorCode_t tmdlHdmiTxOpen(tmInstance_t *pInstance) {
	/*                                                               */
	return (tmdlHdmiTxOpenM(pInstance, (tmUnitSelect_t) 0));
}

/*                                                                             
                                                                          
                                                                     
                                                                        
                                                            
                        

                                                                            
                  
                                              

                            
                                     
                                                                     
                                                  
                                                               
                                                                   
                   
                                                                       
                                                                
                   
                                                                  
                                                              
                                                                    
                                                                  
                                                                  
                                                                  
               
                                                               
                  
                                                                  
                                            
                                                                   
                                                               

                                                                             */

#include <linux/semaphore.h>
DEFINE_SEMAPHORE(hdmitx_mutex);

tmErrorCode_t tmdlHdmiTxOpenM(tmInstance_t *pInstance, tmUnitSelect_t unit) {
	tmErrorCode_t errCode;
	tmErrorCode_t errCodeSem;
	UInt16 i;
	UInt8 deviceVersion;
	Bool featureSupported;

	/*                                  */
	RETIF((unit < 0) || (unit >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_UNIT_NUMBER)

	    /*                                   */
	    RETIF(pInstance == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                                                                       */
	    /*                                                                                                */
	    dlHdmiTxItSemaphore[unit] = (tmdlHdmiTxIWSemHandle_t) (&hdmitx_mutex);

	/*                    */
	RETIF((errCodeSem = tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[unit])) != TM_OK, errCodeSem)

	    /*                                       */
	    RETIF_SEM(dlHdmiTxItSemaphore[unit],
		      unitTableTx[unit].opened == True, TMDL_ERR_DLHDMITX_RESOURCE_OWNED)

	    /*                 */
	    RETIF_SEM(dlHdmiTxItSemaphore[unit],
		      dlHdmiTxGetState(unit) != STATE_NOT_INITIALIZED,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                           */
	    /*                                                           */
	    unitTableTx[unit].opened = True;
	unitTableTx[unit].hdcpEnable = False;
	unitTableTx[unit].repeaterEnable = False;
	unitTableTx[unit].deviceVersion = TMDL_HDMITX_DEVICE_UNKNOWN;
	unitTableTx[unit].simplayHd = False;
	unitTableTx[unit].pCallback = Null;
	unitTableTx[unit].revocationList.pList = Null;
	unitTableTx[unit].revocationList.length = 0;

	/*                                                 */
	RETIF_SEM(dlHdmiTxItSemaphore[unit],
		  dlHdmiTxGetConfig(unit, &gtmdlHdmiTxDriverConfigTable[unit]) != TM_OK,
		  TMDL_ERR_DLHDMITX_INIT_FAILED)
#ifndef TMFL_NO_RTOS
	    /*                                                       */
	    RETIF_SEM(dlHdmiTxItSemaphore[unit],
		      tmdlHdmiTxIWQueueCreate(gtmdlHdmiTxDriverConfigTable[unit].
					      commandTaskQueueSize,
					      &(unitTableTx[unit].queueHandle)) != TM_OK,
		      TMDL_ERR_DLHDMITX_NO_RESOURCES)

	    /*                                                          */
	    RETIF_SEM(dlHdmiTxItSemaphore[unit],
		      tmdlHdmiTxIWTaskCreate(commandTaskTableTx[unit],
					     gtmdlHdmiTxDriverConfigTable[unit].commandTaskPriority,
					     gtmdlHdmiTxDriverConfigTable[unit].
					     commandTaskStackSize,
					     &(unitTableTx[unit].commandTaskHandle)) != TM_OK,
		      TMDL_ERR_DLHDMITX_NO_RESOURCES)

	    RETIF_SEM(dlHdmiTxItSemaphore[unit],
		      tmdlHdmiTxIWTaskStart(unitTableTx[unit].commandTaskHandle) != TM_OK,
		      TMDL_ERR_DLHDMITX_NO_RESOURCES)

	    /*                                                             */
#ifndef NO_HDCP
	    RETIF_SEM(dlHdmiTxItSemaphore[unit],
		      tmdlHdmiTxIWTaskCreate(hdcpTaskTableTx[unit],
					     gtmdlHdmiTxDriverConfigTable[unit].hdcpTaskPriority,
					     gtmdlHdmiTxDriverConfigTable[unit].hdcpTaskStackSize,
					     &(unitTableTx[unit].hdcpTaskHandle)) != TM_OK,
		      TMDL_ERR_DLHDMITX_NO_RESOURCES)
#endif				/*         */
#endif				/*              */
	    * pInstance = (tmInstance_t) unit;

#ifndef NO_HDCP
	hdcpInfoListTx[unit].bKsvSecure = False;
	hdcpInfoListTx[unit].hdcpKsvDevices = 0;
	for (i = 0; i < TMDL_HDMITX_KSV_BYTES_PER_DEVICE; i++) {
		hdcpInfoListTx[unit].hdcpBksv[i] = 0;
	}
	hdcpInfoListTx[unit].hdcpDeviceDepth = 0;
#endif				/*         */

	/*              */
	/*                                   */
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_HDCP_ACTIVE].status =
	    TMDL_HDMITX_EVENT_DISABLED;
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_HDCP_INACTIVE].status =
	    TMDL_HDMITX_EVENT_DISABLED;
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_HPD_ACTIVE].status =
	    TMDL_HDMITX_EVENT_DISABLED;
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_HPD_INACTIVE].status =
	    TMDL_HDMITX_EVENT_DISABLED;
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_RX_KEYS_RECEIVED].status =
	    TMDL_HDMITX_EVENT_DISABLED;
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_RX_DEVICE_ACTIVE].status =
	    TMDL_HDMITX_EVENT_DISABLED;
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_RX_DEVICE_INACTIVE].status =
	    TMDL_HDMITX_EVENT_DISABLED;
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_EDID_RECEIVED].status =
	    TMDL_HDMITX_EVENT_DISABLED;
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_VS_RPT_RECEIVED].status =
	    TMDL_HDMITX_EVENT_DISABLED;
#ifdef HDMI_TX_REPEATER_ISR_MODE
	instanceStatusInfoTx[unit].pEventState[TMDL_HDMITX_B_STATUS].status =
	    TMDL_HDMITX_EVENT_DISABLED;
#endif				/*                           */
	instanceStatusInfoTx[unit].pColBarState->disableColorBarOnR0 = False;
	instanceStatusInfoTx[unit].pColBarState->hdcpColbarChange = False;
	instanceStatusInfoTx[unit].pColBarState->hdcpEncryptOrT0 = True;
	instanceStatusInfoTx[unit].pColBarState->hdcpSecureOrT0 = False;
	instanceStatusInfoTx[unit].pColBarState->inOutFirstSetDone = False;
	instanceStatusInfoTx[unit].pColBarState->colorBarOn = False;
	instanceStatusInfoTx[unit].pColBarState->changeColorBarNow = False;

	instanceStatusInfoTx[unit].pGamutState->gamutOn = False;
	instanceStatusInfoTx[unit].pGamutState->gamutBufNum = 0;	/*                         */
	instanceStatusInfoTx[unit].pGamutState->wideGamutColorSpace =
	    TMDL_HDMITX_EXT_COLORIMETRY_XVYCC601;
	instanceStatusInfoTx[unit].pGamutState->extColOn = False;
	instanceStatusInfoTx[unit].pGamutState->yccQR = TMDL_HDMITX_YQR_LIMITED;


	instanceStatusInfoTx[unit].pAudioInfo->audioMuteState = False;	/*                              */



	/*                                                                      */
	for (i = 0; i < HDMITX_CALLBACK_INT_NUM; i++) {
		callbackFuncTableTx.funcCallback[i] = Null;
	}
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_ENCRYPT] = dlHdmiTxHandleENCRYPT;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_HPD] = dlHdmiTxHandleHPD;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_T0] = dlHdmiTxHandleT0;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_BCAPS] = dlHdmiTxHandleBCAPS;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_BSTATUS] = dlHdmiTxHandleBSTATUS;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_SHA_1] = dlHdmiTxHandleSHA_1;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_PJ] = dlHdmiTxHandlePJ;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_R0] = dlHdmiTxHandleR0;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_SW_INT] = dlHdmiTxHandleSW_INT;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_RX_SENSE] = dlHdmiTxHandleRX_SENSE;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_EDID_BLK_READ] =
	    dlHdmiTxHandleEDID_READ;
	callbackFuncTableTx.funcCallback[HDMITX_CALLBACK_INT_VS_RPT] = dlHdmiTxHandleVS_RPT;

	/*                                                                    */

	tmbslHdmiTxHwStartup();
	errCode = tmbslHdmiTxInit(*pInstance, gtmdlHdmiTxDriverConfigTable[unit].i2cAddress, gtmdlHdmiTxDriverConfigTable[unit].i2cWriteFunction, gtmdlHdmiTxDriverConfigTable[unit].i2cReadFunction, (ptmbslHdmiTxSysFuncEdid_t) 0,	/*                      */
				  (ptmbslHdmiTxSysFuncTimer_t) tmdlHdmiTxIWWait, &callbackFuncTableTx, False,	/*                                 */
				  (tmbslHdmiTxVidFmt_t) instanceStatusInfoTx[unit].pVideoInfo->
				  videoInConfig.format,
				  (tmbslHdmiTxPixRate_t) instanceStatusInfoTx[unit].pVideoInfo->
				  videoInConfig.pixelRate);
	if (errCode != TM_OK) {
		/*             */
		tmbslHdmiTxDeinit(unit);

		/*                       */
		RETIF((errCodeSem =
		       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[unit])) != TM_OK, errCodeSem)

		    return errCode;
	} else {
		/*                       */

		/*                                      */
		errCode = tmbslHdmiTxTmdsSetOutputs(unit, HDMITX_TMDSOUT_FORCED0);
		RETIF_SEM(dlHdmiTxItSemaphore[unit], (errCode != TM_OK)
			  && (errCode != TMBSL_ERR_HDMI_NOT_SUPPORTED), errCode)

		    RETIF_SEM(dlHdmiTxItSemaphore[unit],
			      (errCode = tmbslHdmiTxHwGetCapabilities(unit,
								      HDMITX_FEATURE_HW_HDCP,
								      &featureSupported)) != TM_OK,
			      errCode)
#ifndef NO_HDCP
		    if (featureSupported == True) {
			dlHdmiTxFindHdcpSeed(unit);
		}
#endif				/*         */

#ifdef TMFL_HDCP_OPTIMIZED_POWER
		tmbslHdmiTxHdcpPowerDown(unit, True);
#endif
		/*                                                         */
		RETIF_SEM(dlHdmiTxItSemaphore[unit],
			  (errCode = tmbslHdmiTxHwGetVersion(unit, &deviceVersion))
			  != TM_OK, errCode);

		/*                                                          */
		switch (deviceVersion) {
		case BSLHDMITX_TDA9984:
			unitTableTx[unit].deviceVersion = TMDL_HDMITX_DEVICE_TDA9984;
			break;

		case BSLHDMITX_TDA9989:
			unitTableTx[unit].deviceVersion = TMDL_HDMITX_DEVICE_TDA9989;
			break;

		case BSLHDMITX_TDA9981:
			unitTableTx[unit].deviceVersion = TMDL_HDMITX_DEVICE_TDA9981;
			break;

		case BSLHDMITX_TDA9983:
			unitTableTx[unit].deviceVersion = TMDL_HDMITX_DEVICE_TDA9983;
			break;

		case BSLHDMITX_TDA19989:
			unitTableTx[unit].deviceVersion = TMDL_HDMITX_DEVICE_TDA19989;
			break;

		case BSLHDMITX_TDA19988:
			unitTableTx[unit].deviceVersion = TMDL_HDMITX_DEVICE_TDA19988;
			break;

		default:
			unitTableTx[unit].deviceVersion = TMDL_HDMITX_DEVICE_UNKNOWN;
			break;
		}
	}


#ifndef TMFL_NO_RTOS
	/*                       */

#ifndef NO_HDCP
	RETIF_SEM(dlHdmiTxItSemaphore[unit],
		  tmdlHdmiTxIWTaskStart(unitTableTx[unit].hdcpTaskHandle) != TM_OK,
		  TMDL_ERR_DLHDMITX_NO_RESOURCES)
#endif				/*         */
#endif				/*              */
	    /*                                      */
	    dlHdmiTxSetState(unit, STATE_INITIALIZED);

	/*                       */
	RETIF((errCodeSem = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[unit])) != TM_OK, errCodeSem)

	    return TM_OK;
}

/*                                                                             
                                              

                                        

                            
                                     
                                                                      
                   
                                                                    
                   
                                                               

                                                                             */
tmErrorCode_t tmdlHdmiTxClose(tmInstance_t instance) {
	tmErrorCode_t errCode = TM_OK;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      unitTableTx[instance].opened == False, TMDL_ERR_DLHDMITX_RESOURCE_NOT_OWNED)

	    /*                */
	    unitTableTx[instance].opened = False;

	/*                       */
	dlHdmiTxSetState(instance, STATE_NOT_INITIALIZED);

	/*                                                    */

#ifndef TMFL_NO_RTOS

#ifndef NO_HDCP
	tmdlHdmiTxIWTaskDestroy(unitTableTx[instance].hdcpTaskHandle);
#endif				/*         */

	tmdlHdmiTxIWTaskDestroy(unitTableTx[instance].commandTaskHandle);
	tmdlHdmiTxIWQueueDestroy(unitTableTx[instance].queueHandle);

#endif				/*              */

	/*                                          */
	tmbslHdmiTxDeinit(instance);

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                   */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreDestroy(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                      

                                          
                                         

                            
                                     
                                                                      
                   
                                                               
                                                                  
                                                                 
                                                                  
                                                                   
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxSetPowerState(tmInstance_t instance, tmPowerState_t powerState) {
	tmErrorCode_t errCode;
	tmbslHdmiTxHotPlug_t hpdStatus;	/*            */

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (unitTableTx[instance].deviceVersion == TMDL_HDMITX_DEVICE_TDA9984) {
		if (powerState == tmPowerSuspend) {
			return TMDL_ERR_DLHDMITX_NOT_SUPPORTED;
		}
	}




	/*                 */
	if (((powerState == tmPowerOff) && (unitTableTx[instance].hdcpEnable == True))
	    || ((powerState == tmPowerStandby) && (unitTableTx[instance].hdcpEnable == True))
	    || ((powerState == tmPowerSuspend) && (unitTableTx[instance].hdcpEnable == True))
	    ) {
		/*                       */
		RETIF((errCode =
		       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)
		    /*                 */
		    RETIF((errCode = tmdlHdmiTxSetHdcp(instance, False)) != TM_OK, errCode)
		    /*                    */
		    RETIF((errCode =
			   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)
	}


	/*                                     */
	if ((unitTableTx[instance].deviceVersion == TMDL_HDMITX_DEVICE_TDA9989)
	    || (unitTableTx[instance].deviceVersion == TMDL_HDMITX_DEVICE_TDA19989)
	    || (unitTableTx[instance].deviceVersion == TMDL_HDMITX_DEVICE_TDA19988))
	{
		if ((powerState != tmPowerOn) && (powerState != tmPowerSuspend)) {
			dlHdmiTxSetState(instance, STATE_INITIALIZED);
		}

		if ((powerState == tmPowerOn) && (unitTableTx[instance].simplayHd == True)) {

			instanceStatusInfoTx[0].pColBarState->disableColorBarOnR0 = False;
			instanceStatusInfoTx[0].pColBarState->hdcpColbarChange = False;
			instanceStatusInfoTx[0].pColBarState->hdcpEncryptOrT0 = True;
			instanceStatusInfoTx[0].pColBarState->hdcpSecureOrT0 = False;
			instanceStatusInfoTx[0].pColBarState->inOutFirstSetDone = False;
			instanceStatusInfoTx[0].pColBarState->colorBarOn = True;
			instanceStatusInfoTx[0].pColBarState->changeColorBarNow = True;

		}

	}

	/*                                        */
	RETIF_SEM(dlHdmiTxItSemaphore[instance],
		  (errCode = tmbslHdmiTxPowerSetState(instance, powerState)) != TM_OK, errCode)

	    /*                     */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxHotPlugGetStatus(instance,
							     &hpdStatus, False)) != TM_OK, errCode)

	    if (powerState == tmPowerOn) {
		if ((hpdStatus == HDMITX_HOTPLUG_ACTIVE)
		    && (dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE)) {
			/*                                                      */
			tmbslHdmiTxSysTimerWait(instance, 500);	/*    */

			/*                   */
			RETIF_SEM(dlHdmiTxItSemaphore[instance],
				  (errCode = tmbslHdmiTxEdidRequestBlockData(instance,
									     unitTableTx[instance].
									     pEdidBuffer,
									     (Int) ((unitTableTx
										     [instance].
										     edidBufferSize)
										    >> 7),
									     (Int) (unitTableTx
										    [instance].
										    edidBufferSize)))
				  != TM_OK, errCode)
		}
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                      

                                           
                                                  

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                  
                                                                 
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetPowerState(tmInstance_t instance, tmPowerState_t *pPowerState) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                     */
	    RETIF(pPowerState == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                        */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxPowerGetState(instance, pPowerState)) != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                         
                                                                        
           

                                           

                            
                                     
                                                                      
                   

                                                                             */
tmErrorCode_t tmdlHdmiTxInstanceConfig(tmInstance_t instance) {
	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    return TM_OK;
}

/*                                                                             
                                                                              
                                                                    
                                                        

                                          
                                                                               

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                    
                   
                                                                
                   

                                                                             */
tmErrorCode_t tmdlHdmiTxInstanceSetup
    (tmInstance_t instance, tmdlHdmiTxInstanceSetupInfo_t *pSetupInfo) {
	tmErrorCode_t errCode;
#ifndef NO_HDCP
	UInt16 i;
#endif

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                    */
	    RETIF(pSetupInfo == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      unitTableTx[instance].opened == False, TMDL_ERR_DLHDMITX_RESOURCE_NOT_OWNED)

	    /*                 */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_INITIALIZED,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    unitTableTx[instance].repeaterEnable = pSetupInfo->repeaterEnable;
	unitTableTx[instance].simplayHd = pSetupInfo->simplayHd;
	unitTableTx[instance].pEdidBuffer = pSetupInfo->pEdidBuffer;
	unitTableTx[instance].edidBufferSize = pSetupInfo->edidBufferSize;

#ifndef NO_HDCP
	/*                        */
	hdcpInfoListTx[instance].hdcpCheckState = TMDL_HDMITX_HDCP_CHECK_NOT_STARTED;
	hdcpInfoListTx[instance].hdcpErrorState = 0;
	hdcpInfoListTx[instance].hdcpKsvDevices = 0;
	hdcpInfoListTx[instance].bKsvSecure = False;
	for (i = 0; i < TMDL_HDMITX_KSV_BYTES_PER_DEVICE; i++) {
		hdcpInfoListTx[instance].hdcpBksv[i] = 0;
	}
	hdcpInfoListTx[instance].hdcpDeviceDepth = 0;

	hdcpInfoListTx[instance].hdcpMaxCascExceeded = False;
	hdcpInfoListTx[instance].hdcpMaxDevsExceeded = False;
#endif				/*         */

	/*                                */
	dlHdmiTxSetState(instance, STATE_UNPLUGGED);

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                         

                                          
                                                                      
                   

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                    
                   

                                                                             */
tmErrorCode_t tmdlHdmiTxGetInstanceSetup
    (tmInstance_t instance, tmdlHdmiTxInstanceSetupInfo_t *pSetupInfo) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                    */
	    RETIF(pSetupInfo == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      unitTableTx[instance].opened == False, TMDL_ERR_DLHDMITX_RESOURCE_NOT_OWNED)

	    pSetupInfo->simplayHd = unitTableTx[instance].simplayHd;
	pSetupInfo->repeaterEnable = unitTableTx[instance].repeaterEnable;
	/*          */

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                             
                                                                    
                      

                                          

                            
                                     
                                                                      
                   
                                                               
                                                                    
                   
                                                

                                                                              */
tmErrorCode_t tmdlHdmiTxHandleInterrupt(tmInstance_t instance) {
#ifndef TMFL_NO_RTOS
	tmErrorCode_t errCode;
	UInt8 message = 0;
#else
	tmErrorCode_t err = TM_OK;
#endif				/*              */

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)
#ifndef TMFL_NO_RTOS
	    RETIF((errCode =
		   tmdlHdmiTxIWQueueSend(unitTableTx[instance].queueHandle, message)) != TM_OK,
		  errCode)

	    /*                                                                                  */
	    switch (instance) {
	case INSTANCE_0:
		tmdlHdmiTxIWDisableInterrupts(TMDL_HDMI_IW_TX_1);
		break;
	case INSTANCE_1:
		tmdlHdmiTxIWDisableInterrupts(TMDL_HDMI_IW_TX_2);
		break;
	default:
		return TMDL_ERR_DLHDMITX_BAD_INSTANCE;
	}
#else
	    /*                                             */
	    instanceStatusInfoTx[0].pColBarState->hdcpSecureOrT0 = False;

	if (gI2CDebugAccessesEnabled == True) {

		err = tmbslHdmiTxHwHandleInterrupt(0);

		if ((err == TMBSL_ERR_HDMI_I2C_WRITE) || (err == TMBSL_ERR_HDMI_I2C_READ)) {

			unitTableTx[0].pCallback(TMDL_HDMITX_DEBUG_EVENT_1);
		}

	}

	/*                                    */
#endif				/*              */

	return TM_OK;
}

/*                                                                             
                                                                            
                                                                
                                                                   
                                     
                                 
                                  

                                          
                                                                              
                        

                            
                                     
                                                                      
                   
                                                               
                                                                    
                   
                                                                
                   

                                                                             */
tmErrorCode_t tmdlHdmiTxRegisterCallbacks(tmInstance_t instance, ptmdlHdmiTxCallback_t pCallback) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      unitTableTx[instance].opened == False, TMDL_ERR_DLHDMITX_RESOURCE_NOT_OWNED)

	    /*                                    */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_INITIALIZED,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                         */
	    unitTableTx[instance].pCallback = pCallback;

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                          
                                           

                                        
                                    

                            
                                     
                                                                      
                   
                                                               

                                                                             */
tmErrorCode_t tmdlHdmiTxEnableEvent(tmInstance_t instance, tmdlHdmiTxEvent_t event) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                           */
	    RETIF_BADPARAM(event >= EVENT_NB)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                      */
	    instanceStatusInfoTx[instance].pEventState[event].status = TMDL_HDMITX_EVENT_ENABLED;

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                           
                                           

                                        
                                     

                            
                                     
                                                                      
                   
                                                               

                                                                             */
tmErrorCode_t tmdlHdmiTxDisableEvent(tmInstance_t instance, tmdlHdmiTxEvent_t event) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                           */
	    RETIF_BADPARAM(event >= EVENT_NB)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                      */
	    instanceStatusInfoTx[instance].pEventState[event].status = TMDL_HDMITX_EVENT_DISABLED;

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                          
                                                               
                                                           
                                 
                                  

                                                
                                                                        
                                                                     

                            
                                     
                                                                      
                   
                                                                   
                   
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxGetVideoFormatSpecs
    (tmInstance_t instance,
     tmdlHdmiTxVidFmt_t resolutionID, tmdlHdmiTxVidFmtSpecs_t *pResolutionSpecs) {
	UInt8 i;
	Bool find = False;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                          */
	    RETIF(pResolutionSpecs == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    for (i = 0; i < RESOLUTION_NB; i++) {
		if (resolutionID ==
		    gtmdlHdmiTxDriverConfigTable[instance].pResolutionInfo[i].resolutionID) {
			find = True;
			pResolutionSpecs->height =
			    gtmdlHdmiTxDriverConfigTable[instance].pResolutionInfo[i].height;
			pResolutionSpecs->width =
			    gtmdlHdmiTxDriverConfigTable[instance].pResolutionInfo[i].width;
			pResolutionSpecs->interlaced =
			    gtmdlHdmiTxDriverConfigTable[instance].pResolutionInfo[i].interlaced;
			pResolutionSpecs->vfrequency =
			    gtmdlHdmiTxDriverConfigTable[instance].pResolutionInfo[i].vfrequency;
			pResolutionSpecs->aspectRatio =
			    gtmdlHdmiTxDriverConfigTable[instance].pResolutionInfo[i].aspectRatio;

			/*                                                                      */
			if ((instanceStatusInfoTx[instance].pVideoInfo->videoInConfig.structure3D ==
			     TMDL_HDMITX_3D_FRAME_PACKING)
			    && pResolutionSpecs->interlaced
			    && ((resolutionID == TMDL_HDMITX_VFMT_20_1920x1080i_50Hz)
				|| (resolutionID == TMDL_HDMITX_VFMT_05_1920x1080i_60Hz))) {
				pResolutionSpecs->interlaced = False;
				if (pResolutionSpecs->vfrequency == TMDL_HDMITX_VFREQ_50Hz) {
					pResolutionSpecs->vfrequency = TMDL_HDMITX_VFREQ_25Hz;
				} else if ((pResolutionSpecs->vfrequency == TMDL_HDMITX_VFREQ_60Hz)
					   || (pResolutionSpecs->vfrequency ==
					       TMDL_HDMITX_VFREQ_59Hz)) {
					pResolutionSpecs->vfrequency = TMDL_HDMITX_VFREQ_30Hz;
				}
			}

			break;
		}
	}

	/*                               */
	RETIF(find == False, TMDL_ERR_DLHDMITX_RESOLUTION_UNKNOWN)

	    return TM_OK;
}

/*                                                                             
                                                                             
                                                                    
                                                                  
           
                                 
                                      

                                                 
                                                              
                                                               
                                                              
                                                                            

                            
                                     
                                                                      
                   
                                                               
                                                                  
                                                                 
                                                                  
                                                                   
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetInputOutput
    (tmInstance_t instance,
     tmdlHdmiTxVideoInConfig_t videoInputConfig,
     tmdlHdmiTxVideoOutConfig_t videoOutputConfig,
     tmdlHdmiTxAudioInConfig_t audioInputConfig, tmdlHdmiTxSinkType_t sinkType) {
	tmErrorCode_t errCode;
	UInt8 pixRepeat;	/*                  */
	tmbslHdmiTxVoutDbits_t pathBits;	/*                     */
	tmbslHdmiTxPixEdge_t pixelEdge;	/*                     */
	tmbslHdmiTxVsMeth_t syncMethod;	/*             */
	tmbslHdmiTxPixTogl_t toggle;	/*          */
	UInt8 syncIn;		/*                      */
	tmbslHdmiTxPixSubpkt_t spSync;	/*                */
	tmbslHdmiTxBlnkSrc_t blankit;	/*          */
	tmbslHdmiTxPixRate_t pixRateSingleDouble;	/*                       */
	UInt16 uRefPix;		/*                   */
	UInt16 uRefLine;	/*                    */
	UInt16 uScRefPix = 0;	/*                   */
	UInt16 uScRefLine = 0;	/*                    */
	Bool bVerified;		/*                         */
	tmbslHdmiTxTopSel_t topSel;	/*                                  */
	tmbslHdmiTxHPhases_t phasesH;	/*                  */
	tmbslHdmiTxVsOnce_t once;	/*                          */
	tmbslHdmiTxScaMode_t scalerMode;	/*                     */
	Bool OBASupported;	/*                      */
	Bool DSTSupported;	/*                      */
	Bool HBRSupported;	/*                       */

	UInt8 *pSwapTable = Null;	/*                                               */
	UInt8 *pMirrorTable = Null;	/*                                               */
#ifdef TMFL_RGB_DDR_12BITS
	UInt8 *pMux = Null;	/*                                               */
#endif
	UInt8 *pEnaVideoPortTable = Null;	/*                                               */
	UInt8 *pGndVideoPortTable = Null;	/*                                               */
	tmdlHdmiTxVidFmt_t vinFmtIndex;	/*                                   */

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                        */
	    instanceStatusInfoTx[instance].pVideoInfo->videoInConfig.format =
	    videoInputConfig.format;
	instanceStatusInfoTx[instance].pVideoInfo->videoInConfig.mode = videoInputConfig.mode;
	instanceStatusInfoTx[instance].pVideoInfo->videoInConfig.syncSource =
	    videoInputConfig.syncSource;
	instanceStatusInfoTx[instance].pVideoInfo->videoInConfig.pixelRate =
	    videoInputConfig.pixelRate;
	instanceStatusInfoTx[instance].pVideoInfo->videoInConfig.structure3D =
	    videoInputConfig.structure3D;

	instanceStatusInfoTx[instance].pVideoInfo->videoOutConfig.format = videoOutputConfig.format;
	instanceStatusInfoTx[instance].pVideoInfo->videoOutConfig.mode = videoOutputConfig.mode;
	instanceStatusInfoTx[instance].pVideoInfo->videoOutConfig.colorDepth =
	    videoOutputConfig.colorDepth;

	/*      */
	/*                                                                       */

	/*                   */
	RETIF_SEM(dlHdmiTxItSemaphore[instance],
		  (errCode = tmbslHdmiTxHwGetCapabilities(instance,
							  HDMITX_FEATURE_HW_AUDIO_OBA,
							  &OBASupported)) != TM_OK, errCode)

	    /*                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxHwGetCapabilities(instance,
							      HDMITX_FEATURE_HW_AUDIO_DST,
							      &DSTSupported)) != TM_OK, errCode)

	    /*                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxHwGetCapabilities(instance,
							      HDMITX_FEATURE_HW_AUDIO_HBR,
							      &HBRSupported)) != TM_OK, errCode)

	    /*                                         */
	    if (((audioInputConfig.format == TMDL_HDMITX_AFMT_OBA) && (OBASupported == False)) ||
		((audioInputConfig.format == TMDL_HDMITX_AFMT_DST) && (DSTSupported == False)) ||
		((audioInputConfig.format == TMDL_HDMITX_AFMT_HBR) && (HBRSupported == False))) {
		/*                       */
		RETIF((errCode =
		       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

		    return TMDL_ERR_DLHDMITX_NOT_SUPPORTED;
	}

	instanceStatusInfoTx[instance].pAudioInfo->audioInCfg.format = audioInputConfig.format;
	instanceStatusInfoTx[instance].pAudioInfo->audioInCfg.i2sFormat =
	    audioInputConfig.i2sFormat;
	instanceStatusInfoTx[instance].pAudioInfo->audioInCfg.i2sQualifier =
	    audioInputConfig.i2sQualifier;
	instanceStatusInfoTx[instance].pAudioInfo->audioInCfg.rate = audioInputConfig.rate;
	instanceStatusInfoTx[instance].pAudioInfo->audioInCfg.channelAllocation =
	    audioInputConfig.channelAllocation;


	if (sinkType == TMDL_HDMITX_SINK_EDID) {
		/*                                                     */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxEdidGetSinkType(instance,
								(tmbslHdmiTxSinkType_t *) &
								sinkType)) != TM_OK, errCode)
	}

	/*                                            */
	if (sinkType == TMDL_HDMITX_SINK_DVI) {
		if (((videoOutputConfig.format >= TMDL_HDMITX_VFMT_06_720x480i_60Hz)
		     && (videoOutputConfig.format <= TMDL_HDMITX_VFMT_15_1440x480p_60Hz))
		    || ((videoOutputConfig.format >= TMDL_HDMITX_VFMT_21_720x576i_50Hz)
			&& (videoOutputConfig.format <= TMDL_HDMITX_VFMT_30_1440x576p_50Hz))
		    || ((videoOutputConfig.format >= TMDL_HDMITX_VFMT_35_2880x480p_60Hz)
			&& (videoOutputConfig.format <= TMDL_HDMITX_VFMT_38_2880x576p_50Hz))
		    ) {
			/*                       */
			RETIF((errCode =
			       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK,
			      errCode)

			    return TMDL_ERR_DLHDMITX_BAD_PARAMETER;
		}
	}

	/*                                                                                */
	RETIF_SEM(dlHdmiTxItSemaphore[instance],
		  (errCode = tmbslHdmiTxSetColorDepth(instance,
						      (tmbslHdmiTxColorDepth) (videoOutputConfig.
									       colorDepth),
						      False)) != TM_OK, errCode)

	    /*                                        */
	    errCode = tmbslHdmiTxTmdsSetOutputs(instance, HDMITX_TMDSOUT_FORCED0);
	RETIF_SEM(dlHdmiTxItSemaphore[instance], (errCode != TM_OK)
		  && (errCode != TMBSL_ERR_HDMI_NOT_SUPPORTED), errCode)

	    /*                               */
	    errCode = tmbslHdmiTxTmdsSetSerializer(instance, 4, 8);
	RETIF_SEM(dlHdmiTxItSemaphore[instance], (errCode != TM_OK)
		  && (errCode != TMBSL_ERR_HDMI_NOT_SUPPORTED), errCode)

	    /*                                */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxVideoOutSetConfig(instance,
							      (tmbslHdmiTxSinkType_t) sinkType,
							      (tmbslHdmiTxVoutMode_t)
							      videoOutputConfig.mode,
							      HDMITX_VOUT_PREFIL_OFF,
							      HDMITX_VOUT_YUV_BLNK_16,
							      HDMITX_VOUT_QRANGE_FS)) != TM_OK,
		      errCode)

	    /*                    */
	    pixRepeat = HDMITX_PIXREP_DEFAULT;
	pathBits = HDMITX_VOUT_DBITS_12;
	pixelEdge = HDMITX_PIXEDGE_CLK_POS;
	syncMethod = HDMITX_VSMETH_V_H;
	toggle = HDMITX_PIXTOGL_ENABLE;

	/*                  */
	if (videoInputConfig.syncSource == TMDL_HDMITX_SYNCSRC_EMBEDDED) {
		/*               */
		syncIn = EMB;
		spSync = HDMITX_PIXSUBPKT_SYNC_HEMB;
		blankit = HDMITX_BLNKSRC_VS_HEMB_VEMB;
		syncMethod = HDMITX_VSMETH_V_XDE;
	} else {
		/*               */
		syncIn = EXT;


		if (gtmdlHdmiTxDriverConfigTable[instance].dataEnableSignalAvailable == 1) {
			/*                 */
			spSync = HDMITX_PIXSUBPKT_SYNC_DE;
		} else {
			/*                     */
			spSync = HDMITX_PIXSUBPKT_SYNC_HS;
		}



		blankit = HDMITX_BLNKSRC_NOT_DE;
	}


#ifdef TMFL_RGB_DDR_12BITS
	/*                             */
	pMux = &gtmdlHdmiTxDriverConfigTable[instance].pNoMux[0];
#endif

	/*                 */
	switch (videoInputConfig.mode) {
	case TMDL_HDMITX_VINMODE_CCIR656:
		pathBits = HDMITX_VOUT_DBITS_8;
		pixelEdge = HDMITX_PIXEDGE_CLK_NEG;
		pSwapTable = gtmdlHdmiTxDriverConfigTable[instance].pSwapTableCCIR656;
		pMirrorTable = gtmdlHdmiTxDriverConfigTable[instance].pMirrorTableCCIR656;
#ifdef TMFL_RGB_DDR_12BITS
		pMux = &gtmdlHdmiTxDriverConfigTable[instance].pMux_RGB_DDR_12bits[0];
#endif
		pEnaVideoPortTable = gtmdlHdmiTxDriverConfigTable[instance].pEnableVideoPortCCIR656;
		pGndVideoPortTable = gtmdlHdmiTxDriverConfigTable[instance].pGroundVideoPortCCIR656;
		break;

	case TMDL_HDMITX_VINMODE_RGB444:
		pSwapTable = gtmdlHdmiTxDriverConfigTable[instance].pSwapTableRGB444;
		pMirrorTable = gtmdlHdmiTxDriverConfigTable[instance].pMirrorTableRGB444;
		pEnaVideoPortTable = gtmdlHdmiTxDriverConfigTable[instance].pEnableVideoPortRGB444;
		pGndVideoPortTable = gtmdlHdmiTxDriverConfigTable[instance].pGroundVideoPortRGB444;
		break;

	case TMDL_HDMITX_VINMODE_YUV444:
		pSwapTable = gtmdlHdmiTxDriverConfigTable[instance].pSwapTableYUV444;
		pMirrorTable = gtmdlHdmiTxDriverConfigTable[instance].pMirrorTableYUV444;
		pEnaVideoPortTable = gtmdlHdmiTxDriverConfigTable[instance].pEnableVideoPortYUV444;
		pGndVideoPortTable = gtmdlHdmiTxDriverConfigTable[instance].pGroundVideoPortYUV444;
		break;

	case TMDL_HDMITX_VINMODE_YUV422:
		pSwapTable = gtmdlHdmiTxDriverConfigTable[instance].pSwapTableYUV422;
		pMirrorTable = gtmdlHdmiTxDriverConfigTable[instance].pMirrorTableYUV422;
#ifdef TMFL_RGB_DDR_12BITS
		pMux = &gtmdlHdmiTxDriverConfigTable[instance].pMux_RGB_DDR_12bits[0];
#endif
		pEnaVideoPortTable = gtmdlHdmiTxDriverConfigTable[instance].pEnableVideoPortYUV422;
		pGndVideoPortTable = gtmdlHdmiTxDriverConfigTable[instance].pGroundVideoPortYUV422;
		break;

#ifdef TMFL_RGB_DDR_12BITS
	case TMDL_HDMITX_VINMODE_RGB_DDR_12BITS:
		pSwapTable = gtmdlHdmiTxDriverConfigTable[instance].pSwapTableRGB_DDR_12bits;
		pMirrorTable = gtmdlHdmiTxDriverConfigTable[instance].pMirrorTableRGB_DDR_12bits;
		pMux = &gtmdlHdmiTxDriverConfigTable[instance].pMux_RGB_DDR_12bits[0];
		pEnaVideoPortTable =
		    gtmdlHdmiTxDriverConfigTable[instance].pEnableVideoPortRGB_DDR_12bits;
		pGndVideoPortTable =
		    gtmdlHdmiTxDriverConfigTable[instance].pGroundVideoPortRGB_DDR_12bits;
		break;
#endif
	default:
		break;
	}

	/*                                                  */
	errCode = tmbslHdmiTxSetVideoPortConfig(instance, pEnaVideoPortTable, pGndVideoPortTable);
	RETIF_SEM(dlHdmiTxItSemaphore[instance], (errCode != TM_OK)
		  && (errCode != TMBSL_ERR_HDMI_NOT_SUPPORTED), errCode);

#ifdef TMFL_RGB_DDR_12BITS
	errCode = tmbslHdmiTxVideoInSetMapping(instance, pSwapTable, pMirrorTable, pMux);
#else
	errCode = tmbslHdmiTxVideoInSetMapping(instance, pSwapTable, pMirrorTable);
#endif
	RETIF_SEM(dlHdmiTxItSemaphore[instance], (errCode != TM_OK)
		  && (errCode != TMBSL_ERR_HDMI_NOT_SUPPORTED), errCode);

	/*                         */
	RETIF_SEM(dlHdmiTxItSemaphore[instance],
		  (errCode =
		   tmbslHdmiTxVideoInSetFine(instance, spSync, HDMITX_PIXTOGL_NO_ACTION)) != TM_OK,
		  errCode);

	/*                    */
	errCode = tmbslHdmiTxVideoInSetBlanking(instance, blankit, HDMITX_BLNKCODE_ALL_0);
	RETIF_SEM(dlHdmiTxItSemaphore[instance], (errCode != TM_OK)
		  && (errCode != TMBSL_ERR_HDMI_NOT_SUPPORTED), errCode);

	/*                                                         */
	RETIF_SEM(dlHdmiTxItSemaphore[instance],
		  (errCode = tmbslHdmiTxVideoInSetConfig(instance,
							 (tmbslHdmiTxVinMode_t) videoInputConfig.
							 mode,
							 (tmbslHdmiTxVidFmt_t) videoOutputConfig.
							 format,
							 (tmbslHdmiTx3DStructure_t)
							 videoInputConfig.structure3D, pixelEdge,
							 (tmbslHdmiTxPixRate_t) videoInputConfig.
							 pixelRate, HDMITX_UPSAMPLE_AUTO)) != TM_OK,
		  errCode);


	/*                                                */
	RETIF_SEM(dlHdmiTxItSemaphore[instance],
		  (errCode =
		   tmbslHdmiTxVideoSetInOut(instance, (tmbslHdmiTxVidFmt_t) videoInputConfig.format,
					    (tmbslHdmiTx3DStructure_t) videoInputConfig.structure3D,
					    HDMITX_SCAMODE_AUTO,
					    (tmbslHdmiTxVidFmt_t) videoOutputConfig.format,
					    pixRepeat, HDMITX_MATMODE_AUTO, pathBits,
					    (tmbslHdmiTxVQR_t) videoOutputConfig.dviVqr)) != TM_OK,
		  errCode);


	/*                                  */
	if (sinkType == TMDL_HDMITX_SINK_HDMI) {
		/*                       */
		RETIF((errCode =
		       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)
		    /*                      */
		    RETIF((errCode =
			   tmdlHdmiTxSetAudioInput(instance, audioInputConfig, sinkType)) != TM_OK,
			  errCode)
		    /*                    */
		    RETIF((errCode =
			   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)
	}

	/*                        */
	pixRateSingleDouble = (tmbslHdmiTxPixRate_t) videoInputConfig.pixelRate;
	if (videoInputConfig.pixelRate == HDMITX_PIXRATE_SINGLE_REPEATED) {
		pixRateSingleDouble = HDMITX_PIXRATE_SINGLE;
	}


	if ((videoInputConfig.structure3D != HDMITX_3D_FRAME_PACKING) &&
	    dlHdmiTxGetReflineRefpix(videoInputConfig.format, videoInputConfig.mode,
				     videoOutputConfig.format, syncIn,
				     (tmdlHdmiTxPixRate_t) pixRateSingleDouble, &uRefPix, &uRefLine,
				     &uScRefPix, &uScRefLine, &bVerified) > 0) {
		/*                                                                           
                                                      */
		if (syncIn == EXT) {
			switch (videoInputConfig.format) {
			case TMDL_HDMITX_VFMT_04_1280x720p_60Hz:
			case TMDL_HDMITX_VFMT_19_1280x720p_50Hz:
			case TMDL_HDMITX_VFMT_05_1920x1080i_60Hz:
			case TMDL_HDMITX_VFMT_20_1920x1080i_50Hz:
				if ((videoOutputConfig.format ==
				     TMDL_HDMITX_VFMT_16_1920x1080p_60Hz)
				    || (videoOutputConfig.format ==
					TMDL_HDMITX_VFMT_31_1920x1080p_50Hz)) {
					toggle = HDMITX_PIXTOGL_NO_ACTION;
				}
				break;
			default:
				toggle = HDMITX_PIXTOGL_ENABLE;
				break;
			}
		}

		/*                                                                 */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxVideoInSetSyncManual(instance,
								     (tmbslHdmiTxSyncSource_t)
								     videoInputConfig.syncSource,
								     syncMethod, toggle, toggle,
								     toggle, uRefPix,
								     uRefLine)) != TM_OK, errCode)
	} else {
		/*                                                         */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxVideoInSetSyncAuto(instance,
								   (tmbslHdmiTxSyncSource_t)
								   videoInputConfig.syncSource,
								   (tmbslHdmiTxVidFmt_t)
								   videoInputConfig.format,
								   (tmbslHdmiTxVinMode_t)
								   videoInputConfig.mode,
								   (tmbslHdmiTx3DStructure_t)
								   videoInputConfig.structure3D)) !=
			  TM_OK, errCode)
	}

	/*                                       */
	if (sinkType == TMDL_HDMITX_SINK_HDMI) {
		/*                   */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode =
			   dlHdmiTxSetVideoInfoframe(instance, videoOutputConfig.format,
						     videoOutputConfig.mode)) != TM_OK, errCode)
	}

	errCode = tmbslHdmiTxScalerGetMode(instance, &scalerMode);

	/*                                                  */
	if ((errCode == TM_OK) && (scalerMode == HDMITX_SCAMODE_ON)) {
		/*                    */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxScalerInDisable(instance, False)) != TM_OK, errCode)

		    /*                         */
		    topSel = HDMITX_TOPSEL_INTERNAL;
		if ((videoOutputConfig.format == TMDL_HDMITX_VFMT_05_1920x1080i_60Hz)
		    || (videoOutputConfig.format == TMDL_HDMITX_VFMT_20_1920x1080i_50Hz)) {
			/*                                                                       */
			vinFmtIndex = dlHdmiTxCalcVidFmtIndex(videoInputConfig.format);
			if ((kVfmtToShortFmt_TV[vinFmtIndex] == TV_480p_60Hz)
			    || (kVfmtToShortFmt_TV[vinFmtIndex] == TV_576p_50Hz)) {
				/*                                                   */
				topSel = HDMITX_TOPSEL_VRF;
			}
		}

		/*                            */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxScalerSetFieldOrder(instance,
								    HDMITX_INTEXT_NO_CHANGE,
								    HDMITX_INTEXT_NO_CHANGE, topSel,
								    HDMITX_TOPTGL_NO_CHANGE)) !=
			  TM_OK, errCode)

		    /*                        */
		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxScalerSetFine(instance,
								  uScRefPix, uScRefLine)) != TM_OK,
			      errCode)

		    if ((videoOutputConfig.format == TMDL_HDMITX_VFMT_16_1920x1080p_60Hz)
			|| (videoOutputConfig.format == TMDL_HDMITX_VFMT_31_1920x1080p_50Hz)) {
			phasesH = HDMITX_H_PHASES_16;
		} else {
			phasesH = HDMITX_H_PHASES_15;
		}

		/*                  */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxScalerSetPhase(instance,
							       phasesH)) != TM_OK, errCode)

		    /*                    */
		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxScalerSetLatency(instance,
								     0x22)) != TM_OK, errCode)

		    /*                                   */
		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxScalerSetSync(instance,
								  syncMethod,
								  HDMITX_VSONCE_EACH_FRAME)) !=
			      TM_OK, errCode)

		    /*                                                                   */
		    once = HDMITX_VSONCE_ONCE;
	} else {
		once = HDMITX_VSONCE_EACH_FRAME;
	}

	/*                           */
	RETIF_SEM(dlHdmiTxItSemaphore[instance],
		  (errCode = tmbslHdmiTxVideoOutSetSync(instance,
							HDMITX_VSSRC_INTERNAL,
							HDMITX_VSSRC_INTERNAL,
							HDMITX_VSSRC_INTERNAL, HDMITX_VSTGL_TABLE,
							once)) != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    instanceStatusInfoTx[instance].pColBarState->inOutFirstSetDone = True;

	/*                               */
	if (instanceStatusInfoTx[instance].pColBarState->colorBarOn == True) {
		/*                                      */
		instanceStatusInfoTx[instance].pColBarState->changeColorBarNow = True;
	}

	return TM_OK;
}

/*                                                                           */
/* 
                                                                 
                                                                    
                              
                                 
                                      

                                                 
                                                              
                                                                            

                            
                                     
                                                                      
                   
                                                               
                                                                  
                                                                 
                                                                  
                                                                   
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetAudioInput
    (tmInstance_t instance,
     tmdlHdmiTxAudioInConfig_t audioInputConfig, tmdlHdmiTxSinkType_t sinkType) {
	tmErrorCode_t errCode;
	tmdlHdmiTxVidFmtSpecs_t resolutionSpecs;	/*                                                 */
	UInt8 layout;		/*        */
	UInt8 aifChannelCountCode = 0;	/*                           */
	tmbslHdmiTxVfreq_t vOutFreq;	/*                           */
	tmbslHdmiTxctsRef_t ctsRef;	/*                */
	UInt16 uCtsX;		/*            */
	tmbslHdmiTxPktAif_t pktAif;	/*                        */
	Bool OBASupported;	/*                      */
	Bool DSTSupported;	/*                      */
	Bool HBRSupported;	/*                       */
	UInt8 *pEnaAudioPortCfg;
	UInt8 *pGndAudioPortCfg;
	UInt8 *pEnaAudioClockPortCfg;
	UInt8 *pGndAudioClockPortCfg;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxHwGetCapabilities(instance,
							      HDMITX_FEATURE_HW_AUDIO_OBA,
							      &OBASupported)) != TM_OK, errCode)

	    /*                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxHwGetCapabilities(instance,
							      HDMITX_FEATURE_HW_AUDIO_DST,
							      &DSTSupported)) != TM_OK, errCode)

	    /*                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxHwGetCapabilities(instance,
							      HDMITX_FEATURE_HW_AUDIO_HBR,
							      &HBRSupported)) != TM_OK, errCode)

	    /*                                         */
	    if (((audioInputConfig.format == TMDL_HDMITX_AFMT_OBA) && (OBASupported == False)) ||
		((audioInputConfig.format == TMDL_HDMITX_AFMT_DST) && (DSTSupported == False)) ||
		((audioInputConfig.format == TMDL_HDMITX_AFMT_HBR) && (HBRSupported == False))) {
		/*                       */
		RETIF((errCode =
		       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

		    return TMDL_ERR_DLHDMITX_NOT_SUPPORTED;
	}

	if (sinkType == TMDL_HDMITX_SINK_EDID) {
		/*                                                     */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxEdidGetSinkType(instance,
								(tmbslHdmiTxSinkType_t *) &
								sinkType)) != TM_OK, errCode)
	}

	if (sinkType == TMDL_HDMITX_SINK_HDMI) {
		/*                  */
		layout = 1;
		if (audioInputConfig.channelAllocation == 0x00) {
			layout = 0;
		}
		aifChannelCountCode = kChanAllocChanNum[audioInputConfig.channelAllocation] - 1;

		/*                          */
		switch (audioInputConfig.format) {
		case TMDL_HDMITX_AFMT_SPDIF:
			pEnaAudioPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioPortSPDIF;
			pGndAudioPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioPortSPDIF;
			pEnaAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioClockPortSPDIF;
			pGndAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioClockPortSPDIF;
			break;

		case TMDL_HDMITX_AFMT_I2S:
			pEnaAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioClockPortI2S;
			pGndAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioClockPortI2S;

			if (audioInputConfig.channelAllocation >= 1) {	/*                    */
				pEnaAudioPortCfg =
				    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioPortI2S8C;
				pGndAudioPortCfg =
				    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioPortI2S8C;
			} else {
				pEnaAudioPortCfg =
				    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioPortI2S;
				pGndAudioPortCfg =
				    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioPortI2S;
			}
			break;

		case TMDL_HDMITX_AFMT_OBA:
			pEnaAudioPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioPortOBA;
			pGndAudioPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioPortOBA;
			pEnaAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioClockPortOBA;
			pGndAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioClockPortOBA;
			break;

		case TMDL_HDMITX_AFMT_DST:
			pEnaAudioPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioPortDST;
			pGndAudioPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioPortDST;
			pEnaAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioClockPortDST;
			pGndAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioClockPortDST;
			break;

		case TMDL_HDMITX_AFMT_HBR:
			pEnaAudioPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioPortHBR;
			pGndAudioPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioPortHBR;
			pEnaAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pEnableAudioClockPortHBR;
			pGndAudioClockPortCfg =
			    gtmdlHdmiTxDriverConfigTable[instance].pGroundAudioClockPortHBR;
			break;

		default:
			/*                       */
			RETIF((errCode =
			       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK,
			      errCode)

			    return TMDL_ERR_DLHDMITX_BAD_PARAMETER;
		}

		errCode = tmbslHdmiTxSetAudioPortConfig(instance,
							pEnaAudioPortCfg, pGndAudioPortCfg);
		RETIF_SEM(dlHdmiTxItSemaphore[instance], (errCode != TM_OK)
			  && (errCode != TMBSL_ERR_HDMI_NOT_SUPPORTED), errCode)

		    errCode = tmbslHdmiTxSetAudioClockPortConfig(instance,
								 pEnaAudioClockPortCfg,
								 pGndAudioClockPortCfg);
		RETIF_SEM(dlHdmiTxItSemaphore[instance], (errCode != TM_OK)
			  && (errCode != TMBSL_ERR_HDMI_NOT_SUPPORTED), errCode)

		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxAudioInSetConfig(instance,
								     (tmbslHdmiTxaFmt_t)
								     audioInputConfig.format,
								     (tmbslHdmiTxI2sFor_t)
								     audioInputConfig.i2sFormat,
								     audioInputConfig.
								     channelAllocation,
								     HDMITX_CHAN_NO_CHANGE,
								     HDMITX_CLKPOLDSD_NO_CHANGE,
								     HDMITX_SWAPDSD_NO_CHANGE,
								     layout, HDMITX_LATENCY_CURRENT,
								     (tmbslHdmiTxDstRate_t)
								     audioInputConfig.dstRate)) !=
			      TM_OK, errCode)

		    /*                                                   */
		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode =
			       tmdlHdmiTxGetVideoFormatSpecs(instance,
							     instanceStatusInfoTx[instance].
							     pVideoInfo->videoOutConfig.format,
							     &resolutionSpecs)) != TM_OK, errCode)
		    vOutFreq = (tmbslHdmiTxVfreq_t) resolutionSpecs.vfrequency;

		if ((audioInputConfig.format == TMDL_HDMITX_AFMT_SPDIF)
		    || (audioInputConfig.format == TMDL_HDMITX_AFMT_OBA)) {
			ctsRef = HDMITX_CTSREF_FS64SPDIF;
			uCtsX = HDMITX_CTSX_64;
		} else {	/*     */

			ctsRef = HDMITX_CTSREF_ACLK;
			if (audioInputConfig.i2sQualifier == TMDL_HDMITX_I2SQ_32BITS) {
				uCtsX = HDMITX_CTSX_64;
			} else {
				uCtsX = HDMITX_CTSX_32;
			}
		}

		/*                                                      */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxAudioInSetCts(instance,
							      ctsRef,
							      (tmbslHdmiTxafs_t) audioInputConfig.
							      rate,
							      (tmbslHdmiTxVidFmt_t)
							      instanceStatusInfoTx[instance].
							      pVideoInfo->videoOutConfig.format,
							      vOutFreq, HDMITX_CTS_AUTO, uCtsX,
							      HDMITX_CTSK_USE_CTSX,
							      HDMITX_CTSMTS_USE_CTSX,
							      (tmbslHdmiTxDstRate_t)
							      audioInputConfig.dstRate)) != TM_OK,
			  errCode)

		    /*                             
                                                                                       
                                                           */
		    if (audioInputConfig.format != TMDL_HDMITX_AFMT_SPDIF) {	/*                                                */

			if (audioInputConfig.format != TMDL_HDMITX_AFMT_HBR) {

				RETIF_SEM(dlHdmiTxItSemaphore[instance],
					  (errCode = tmbslHdmiTxAudioOutSetChanStatus(instance,
										      (tmbslHdmiTxAudioData_t)
										      audioInputConfig.
										      channelStatus.
										      PcmIdentification,
										      (tmbslHdmiTxCSformatInfo_t)
										      audioInputConfig.
										      channelStatus.
										      FormatInfo,
										      (tmbslHdmiTxCScopyright_t)
										      audioInputConfig.
										      channelStatus.
										      CopyrightInfo,
										      audioInputConfig.
										      channelStatus.
										      categoryCode,
										      (tmbslHdmiTxafs_t)
										      audioInputConfig.
										      rate,
										      (tmbslHdmiTxCSclkAcc_t)
										      audioInputConfig.
										      channelStatus.
										      clockAccuracy,
										      (tmbslHdmiTxCSmaxWordLength_t)
										      audioInputConfig.
										      channelStatus.
										      maxWordLength,
										      (tmbslHdmiTxCSwordLength_t)
										      audioInputConfig.
										      channelStatus.
										      wordLength,
										      (tmbslHdmiTxCSorigAfs_t)
										      audioInputConfig.
										      channelStatus.
										      origSampleFreq))
					  != TM_OK, errCode)
			} else {


				RETIF_SEM(dlHdmiTxItSemaphore[instance],
					  (errCode = tmbslHdmiTxAudioOutSetChanStatus(instance,
										      (tmbslHdmiTxAudioData_t)
										      audioInputConfig.
										      channelStatus.
										      PcmIdentification,
										      (tmbslHdmiTxCSformatInfo_t)
										      audioInputConfig.
										      channelStatus.
										      FormatInfo,
										      (tmbslHdmiTxCScopyright_t)
										      audioInputConfig.
										      channelStatus.
										      CopyrightInfo,
										      audioInputConfig.
										      channelStatus.
										      categoryCode,
										      HDMITX_AFS_768K,
										      (tmbslHdmiTxCSclkAcc_t)
										      audioInputConfig.
										      channelStatus.
										      clockAccuracy,
										      (tmbslHdmiTxCSmaxWordLength_t)
										      audioInputConfig.
										      channelStatus.
										      maxWordLength,
										      (tmbslHdmiTxCSwordLength_t)
										      audioInputConfig.
										      channelStatus.
										      wordLength,
										      (tmbslHdmiTxCSorigAfs_t)
										      audioInputConfig.
										      channelStatus.
										      origSampleFreq))
					  != TM_OK, errCode)


			}

		}


		/*                     */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxAudioOutSetMute(instance,
								HDMITX_AMUTE_ON)) != TM_OK, errCode)

		    /*                                            */
		    if (instanceStatusInfoTx[instance].pAudioInfo->audioMuteState == False) {

			/*                */
			RETIF_SEM(dlHdmiTxItSemaphore[instance],
				  (errCode = tmdlHdmiTxIWWait(20)) != TM_OK, errCode)
			    /*                     */
			    RETIF_SEM(dlHdmiTxItSemaphore[instance],
				      (errCode = tmbslHdmiTxAudioOutSetMute(instance,
									    HDMITX_AMUTE_OFF)) !=
				      TM_OK, errCode)

		}


		/*                     */
		pktAif.ChannelCount = aifChannelCountCode;
		pktAif.CodingType = 0;	/*                        */
		pktAif.SampleSize = 0;	/*                        */
		pktAif.ChannelAlloc = audioInputConfig.channelAllocation;
		pktAif.LevelShift = 0;	/*                 */
		pktAif.DownMixInhibit = 0;	/*                           */
		pktAif.SampleFreq = AIF_SF_REFER_TO_STREAM_HEADER;	/*                        */

		/*                                                                  */
		if ((audioInputConfig.format == TMDL_HDMITX_AFMT_OBA) ||
		    (audioInputConfig.format == TMDL_HDMITX_AFMT_DST)) {
			switch (audioInputConfig.rate) {
			case TMDL_HDMITX_AFS_32K:
				pktAif.SampleFreq = AIF_SF_32K;	/*                         */
				break;
			case TMDL_HDMITX_AFS_44K:
				pktAif.SampleFreq = AIF_SF_44K;
				break;
			case TMDL_HDMITX_AFS_48K:
				pktAif.SampleFreq = AIF_SF_48K;
				break;
			case TMDL_HDMITX_AFS_88K:
				pktAif.SampleFreq = AIF_SF_88K;
				break;
			case TMDL_HDMITX_AFS_96K:
				pktAif.SampleFreq = AIF_SF_96K;
				break;
			case TMDL_HDMITX_AFS_176K:
				pktAif.SampleFreq = AIF_SF_176K;
				break;
			case TMDL_HDMITX_AFS_192K:
				pktAif.SampleFreq = AIF_SF_192K;
				break;
			default:
				pktAif.SampleFreq = AIF_SF_REFER_TO_STREAM_HEADER;	/*                        */
				break;
			}
		}

		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetAudioInfoframe(instance,
								     &pktAif, True)) != TM_OK,
			  errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                        
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetVideoInfoframe
    (tmInstance_t instance, Bool enable, tmdlHdmiTxAviIfData_t *pAviIfData) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (enable == True) {
		/*                                    */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pAviIfData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    errCode = dlHdmiTxSetRawVideoInfoframe(instance, pAviIfData, enable);
	} else {
		errCode = dlHdmiTxSetRawVideoInfoframe(instance, Null, enable);
	}

	/*                                        */
	if (errCode == TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED) {
		errCode = TM_OK;
	}

	RETIF_SEM(dlHdmiTxItSemaphore[instance], errCode != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                        
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetAudioInfoframe
    (tmInstance_t instance, Bool enable, tmdlHdmiTxAudIfData_t *pAudIfData) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (enable == True) {
		/*                                    */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pAudIfData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxPktSetAudioInfoframe(instance,
									 (tmbslHdmiTxPktAif_t *)
									 pAudIfData,
									 enable)) != TM_OK, errCode)
	} else {
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetAudioInfoframe(instance,
								     Null, enable)) != TM_OK,
			  errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                           
                      
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetACPPacket
    (tmInstance_t instance, Bool enable, tmdlHdmiTxAcpPktData_t *pAcpPktData) {
	tmErrorCode_t errCode;
	tmbslHdmiTxPkt_t pkt;
	UInt8 i;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (enable == True) {
		/*                                     */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pAcpPktData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    switch (pAcpPktData->acpType) {
			/*                                */
		case 0:	/*               */
			for (i = 0; i < 28; i++) {
				pkt.dataByte[i] = 0;
			}
			break;

		case 1:	/*                            */
			for (i = 0; i < 28; i++) {
				pkt.dataByte[i] = 0;
			}
			break;

		case 2:	/*           */
			for (i = 0; i < 2; i++) {
				pkt.dataByte[i] = pAcpPktData->acpData[i];
			}
			for (i = 2; i < 28; i++) {
				pkt.dataByte[i] = 0;
			}
			break;

		case 3:	/*               */
			for (i = 0; i < 17; i++) {
				pkt.dataByte[i] = pAcpPktData->acpData[i];
			}
			for (i = 17; i < 28; i++) {
				pkt.dataByte[i] = 0;
			}
			break;

		default:
			/*                       */
			RETIF((errCode =
			       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK,
			      errCode)
			    return TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS;
		}

		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetAcp(instance,
							  &pkt, 28, pAcpPktData->acpType,
							  enable)) != TM_OK, errCode)
	} else {
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetAcp(instance,
							  Null, 0, 0, enable)) != TM_OK, errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                             
           
                                 
                                      

                                            
                                                           
                                                                                  

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetGeneralControlPacket
    (tmInstance_t instance, Bool enable, tmdlHdmiTxGcpPktData_t *pGcpPktData) {
	tmErrorCode_t errCode;
	tmbslHdmiTxaMute_t aMute;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (enable == True) {
		/*                                     */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pGcpPktData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    if (pGcpPktData->avMute == False) {
			aMute = HDMITX_AMUTE_OFF;
		} else {
			aMute = HDMITX_AMUTE_ON;
		}

		/*                                                                          */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetGeneralCntrl(instance,
								   &aMute, enable)) != TM_OK,
			  errCode)
	} else {
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetGeneralCntrl(instance,
								   Null, enable)) != TM_OK, errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                       
                                 
                                      

                                                
                                                               
                                                                                      

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetISRC1Packet
    (tmInstance_t instance, Bool enable, tmdlHdmiTxIsrc1PktData_t *pIsrc1PktData) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (enable == True) {
		/*                                       */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pIsrc1PktData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxPktSetIsrc1(instance,
								(tmbslHdmiTxPkt_t *) pIsrc1PktData->
								UPC_EAN_ISRC, 16,
								pIsrc1PktData->isrcCont,
								pIsrc1PktData->isrcValid,
								pIsrc1PktData->isrcStatus,
								enable)) != TM_OK, errCode)
	} else {
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetIsrc1(instance,
							    Null, 0, 0, 0, 0, enable)) != TM_OK,
			  errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                       
                                 
                                      

                                                
                                                               
                                                                                      

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetISRC2Packet
    (tmInstance_t instance, Bool enable, tmdlHdmiTxIsrc2PktData_t *pIsrc2PktData) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (enable == True) {
		/*                                       */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pIsrc2PktData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxPktSetIsrc2(instance,
								(tmbslHdmiTxPkt_t *) pIsrc2PktData->
								UPC_EAN_ISRC, 16, enable)) != TM_OK,
			      errCode)
	} else {
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetIsrc2(instance,
							    Null, 0, enable)) != TM_OK, errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                        
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetMPSInfoframe
    (tmInstance_t instance, Bool enable, tmdlHdmiTxMpsIfData_t *pMpsIfData) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (enable == True) {
		/*                                    */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pMpsIfData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxPktSetMpegInfoframe(instance,
									(tmbslHdmiTxPktMpeg_t *)
									pMpsIfData,
									enable)) != TM_OK, errCode)
	} else {
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetMpegInfoframe(instance,
								    Null, enable)) != TM_OK,
			  errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                        
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetSpdInfoframe
    (tmInstance_t instance, Bool enable, tmdlHdmiTxSpdIfData_t *pSpdIfData) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (enable == True) {
		/*                                    */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pSpdIfData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxPktSetSpdInfoframe(instance,
								       (tmbslHdmiTxPktSpd_t *)
								       pSpdIfData,
								       enable)) != TM_OK, errCode)
	} else {
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetSpdInfoframe(instance,
								   Null, enable)) != TM_OK, errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                       
                                 
                                      

                                            
                                                           
                                                                        
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetVsInfoframe
    (tmInstance_t instance, Bool enable, tmdlHdmiTxVsPktData_t *pVsIfData) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    if (enable == True) {
		/*                                   */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pVsIfData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxPktSetVsInfoframe(instance,
								      (tmbslHdmiTxPkt_t *)
								      pVsIfData->vsData,
								      HDMITX_PKT_DATA_BYTE_CNT - 1,
								      pVsIfData->version,
								      enable)) != TM_OK, errCode)
	} else {
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSetVsInfoframe(instance,
								  Null, 0, pVsIfData->version,
								  enable)) != TM_OK, errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                              
                                 
                                      

                                         
                                                     

                            
                                     
                                                                      
                   
                                                               
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxDebugSetNullPacket(tmInstance_t instance, Bool enable) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxPktSetNullInsert(instance, enable)) != TM_OK, errCode)


	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                     
                                 
                                      

                                         

                            
                                     
                                                                      
                   
                                                               
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxDebugSetSingleNullPacket(tmInstance_t instance) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxPktSetNullSingle(instance))
		      != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                              
                                                                       
                                                          
                                 
                                      

                                          
                                               

                            
                                     
                                                                      
                   
                                                               
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetAudioMute(tmInstance_t instance, Bool audioMute) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                  */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxAudioOutSetMute(instance,
							    (tmbslHdmiTxaMute_t) audioMute)) !=
		      TM_OK, errCode)

	    /*                                 */
	    instanceStatusInfoTx[instance].pAudioInfo->audioMuteState = audioMute;


	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                           
                                 
                                      

                                          

                            
                                     
                                                                      
                   
                                                               
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      


                                                                             */
tmErrorCode_t tmdlHdmiTxResetAudioCts(tmInstance_t instance) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                            */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance], (errCode = tmbslHdmiTxAudioInResetCts(instance)
		      ) != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                            
                                 
                                  

                                                
                                                                                   
                                                                                  
                       

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidStatus
    (tmInstance_t instance, tmdlHdmiTxEdidStatus_t *pEdidStatus, UInt8 *pEdidBlkCount) {
	tmErrorCode_t errCode;
	UInt8 edidStatus;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                                            */
	    RETIF(pEdidStatus == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pEdidBlkCount == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                     */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxEdidGetStatus(instance,
							  (UInt8 *) &edidStatus)) != TM_OK,
		      errCode)

	    if (edidStatus >= TMDL_HDMITX_EDID_STATUS_INVALID) {
		*pEdidStatus = TMDL_HDMITX_EDID_STATUS_INVALID;
	} else {
		*pEdidStatus = (tmdlHdmiTxEdidStatus_t) edidStatus;
	}

	if ((*pEdidStatus == TMDL_HDMITX_EDID_READ) || (*pEdidStatus == TMDL_HDMITX_EDID_ERROR_CHK)) {
		/*                                                */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxEdidGetBlockCount(instance,
								  pEdidBlkCount)) != TM_OK, errCode)
	}

	if (errCode != TM_OK) {
		/*                                                   */
		*pEdidBlkCount = 0;

		/*                       */
		(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);

		return errCode;
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                          
                                                          
                                 
                                      


                                                    
                                                                            
                
                                                  
                                                                                   
                                  
                                                                                        

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidAudioCaps
    (tmInstance_t instance,
     tmdlHdmiTxEdidAudioDesc_t *pAudioDescs,
     UInt maxAudioDescs, UInt *pWrittenAudioDescs, UInt8 *pAudioFlags) {
	tmErrorCode_t errCode;
	tmbslHdmiTxEdidSad_t edidSad[HDMI_TX_SAD_MAX_CNT];
	UInt i;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                                                         */
	    RETIF(pAudioDescs == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pWrittenAudioDescs == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pAudioFlags == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                                                              */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxEdidGetAudioCapabilities(instance,
								     edidSad, maxAudioDescs,
								     pWrittenAudioDescs,
								     pAudioFlags)) != TM_OK,
		      errCode)

	    for (i = 0; i < *pWrittenAudioDescs; i++) {
		pAudioDescs[i].format = (edidSad[i].ModeChans & 0x78) >> 3;	/*                            */
		pAudioDescs[i].channels = edidSad[i].ModeChans & 0x07;	/*                     */
		pAudioDescs[i].supportedFreqs = edidSad[i].Freqs;	/*                       */

		if (pAudioDescs[i].format == 1) {	/*             */
			pAudioDescs[i].supportedRes = edidSad[i].Byte3 & 0x07;
			pAudioDescs[i].maxBitrate = 0x00;
		} else if ((pAudioDescs[i].format >= 2) &&	/*                   */
			   (pAudioDescs[i].format <= 8)) {
			pAudioDescs[i].supportedRes = 0x00;
			pAudioDescs[i].maxBitrate = edidSad[i].Byte3;
		} else {
			pAudioDescs[i].supportedRes = 0x00;
			pAudioDescs[i].maxBitrate = 0x00;
		}
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                     
                                                                      
                      
                                 
                                      

                                                    
                                                                                
                      
                                                  
                                                                                   
                                  
                                                                                      
                           
                             
                             

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidVideoCaps
    (tmInstance_t instance,
     tmdlHdmiTxShortVidDesc_t *pVideoDesc,
     UInt maxVideoFormats, UInt *pWrittenVideoFormats, UInt8 *pVideoFlags) {
	tmErrorCode_t errCode;
	UInt8 edidVFmtsBuffer[HDMI_TX_SVD_MAX_CNT];
	tmdlHdmiTxEdidVideoTimings_t edidDTDBuffer[NUMBER_DTD_STORED];
	UInt8 i;
	UInt8 writtenDTD = 0;
	UInt8 dtdCounter = 0;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                                                             */
	    RETIF(pVideoDesc == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pWrittenVideoFormats == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pVideoFlags == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(maxVideoFormats == 0, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                                                              */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxEdidGetVideoCapabilities(instance,
								     edidVFmtsBuffer,
								     HDMI_TX_SVD_MAX_CNT,
								     pWrittenVideoFormats,
								     pVideoFlags)) != TM_OK,
		      errCode)

	    /*                                                                                                 */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode =
		       dlHdmiTxEdidGetDTD(instance, edidDTDBuffer, NUMBER_DTD_STORED,
					  &writtenDTD)) != TM_OK, errCode)

	    dtdCounter = 0;
	if (writtenDTD > 0) {
		/*                       */
		RETIF((errCode =
		       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

		    /*                                                        */
		    pVideoDesc[0].videoFormat =
		    tmdlHdmiTxConvertDTDtoCEA(instance, &(edidDTDBuffer[dtdCounter]));

		/*                    */
		RETIF((errCode =
		       tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

		    dtdCounter++;

		pVideoDesc[0].nativeVideoFormat = False;
	}

	/*                                                            */
	for (i = dtdCounter; i < maxVideoFormats; i++) {
		if ((i < (HDMI_TX_SVD_MAX_CNT + dtdCounter))
		    && (i < ((*pWrittenVideoFormats) + dtdCounter))) {
			/*           */
			pVideoDesc[i].videoFormat =
			    (tmdlHdmiTxVidFmt_t) ((Int) edidVFmtsBuffer[i - dtdCounter] & 0x7F);
			/*                                                             */
			if ((edidVFmtsBuffer[i - dtdCounter] & 0x80) == 0x80) {
				pVideoDesc[i].nativeVideoFormat = True;
			} else {
				pVideoDesc[i].nativeVideoFormat = False;
			}
		} else {
			if ((dtdCounter < NUMBER_DTD_STORED) && (dtdCounter < writtenDTD)) {
				/*                       */
				RETIF((errCode =
				       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) !=
				      TM_OK, errCode)
				    /*                            */
				    pVideoDesc[i].videoFormat =
				    tmdlHdmiTxConvertDTDtoCEA(instance,
							      &(edidDTDBuffer[dtdCounter]));
				/*                    */
				RETIF((errCode =
				       tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) !=
				      TM_OK, errCode)

				    dtdCounter++;

				pVideoDesc[i].nativeVideoFormat = False;
			} else {
				/*                         */
				pVideoDesc[i].videoFormat = TMDL_HDMITX_VFMT_01_640x480p_60Hz;
				pVideoDesc[i].nativeVideoFormat = False;
				/*                                          */
				break;
			}
		}
	}

	*pWrittenVideoFormats = *pWrittenVideoFormats + dtdCounter + 1;	/*                    */

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                     
                                                                      
                      
                                 
                                      

                                                        
                                                                             
                          

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidVideoPreferred
    (tmInstance_t instance, tmdlHdmiTxEdidVideoTimings_t *pNativeVideoFormat) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                            */
	    RETIF(pNativeVideoFormat == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                                                                  */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxEdidGetVideoPreferred(instance,
								  (tmbslHdmiTxEdidDtd_t *)
								  pNativeVideoFormat)) != TM_OK,
		      errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                           */
/* 
                                                               
                                                                      
                      
                                 
                                      

                                                
                                                                            
                          
                                              
                                                                               
                                     

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidDetailledTimingDescriptors
    (tmInstance_t instance,
     tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors, UInt8 maxDTDesc, UInt8 *pWrittenDTDesc) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                                         */
	    RETIF(pDTDescriptors == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pWrittenDTDesc == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                                                                                 */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode =
		       dlHdmiTxEdidGetDTD(instance, pDTDescriptors, maxDTDesc,
					  pWrittenDTDesc)) != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                           */
/* 
                                                                       
                                                     
                      
                                 
                                      

                                                
                                                                                    
                   
                                                                                     
                   
                                                                                    
                   
                                              
                                                                               
                                     

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidMonitorDescriptors
    (tmInstance_t instance,
     tmdlHdmiTxEdidFirstMD_t *pEdidFirstMD,
     tmdlHdmiTxEdidSecondMD_t *pEdidSecondMD,
     tmdlHdmiTxEdidOtherMD_t *pEdidOtherMD, UInt8 maxOtherMD, UInt8 *pWrittenOtherMD) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                                         */
	    RETIF(pEdidFirstMD == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pEdidSecondMD == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pEdidOtherMD == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                                                               */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxEdidGetMonitorDescriptors(instance,
								      (tmbslHdmiTxEdidFirstMD_t *)
								      pEdidFirstMD,
								      (tmbslHdmiTxEdidSecondMD_t *)
								      pEdidSecondMD,
								      (tmbslHdmiTxEdidOtherMD_t *)
								      pEdidOtherMD, maxOtherMD,
								      pWrittenOtherMD)) != TM_OK,
		      errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                           */
/* 
                                                           
                                                     
                      
                                 
                                      

                                                    
                                                                                     
          

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidTVPictureRatio
    (tmInstance_t instance, tmdlHdmiTxPictAspectRatio_t *pEdidTvPictureRatio) {
	tmErrorCode_t errCode;
	tmbslHdmiTxEdidBDParam_t edidBDParam;
	UInt16 horizontalSize;
	UInt16 verticalSize;
	tmbslHdmiTxEdidDtd_t edidDTDBuffer;
	UInt8 writtenDTD = 0;
	Bool bDataAvailable = False;	/*                                                    */

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                                         */
	    RETIF(pEdidTvPictureRatio == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                                                                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxEdidGetBasicDisplayParam(instance,
								     &edidBDParam)) != TM_OK,
		      errCode)

	    horizontalSize = (UInt16) edidBDParam.uMaxHorizontalSize;
	verticalSize = (UInt16) edidBDParam.uMaxVerticalSize;

	if ((horizontalSize == 0) && (verticalSize == 0)) {
		/*                                                                                                   */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxEdidGetDetailedTimingDescriptors
			   (instance, &edidDTDBuffer, 1, &writtenDTD)) != TM_OK, errCode);

		if (writtenDTD == 1) {
			horizontalSize = edidDTDBuffer.uHImageSize;
			verticalSize = edidDTDBuffer.uVImageSize;
			bDataAvailable = True;
		} else {
			*pEdidTvPictureRatio = TMDL_HDMITX_P_ASPECT_RATIO_UNDEFINED;
		}
	} else {
		bDataAvailable = True;
	}

	if (bDataAvailable == True) {
		*pEdidTvPictureRatio = dlHdmiTxCalcAspectRatio(horizontalSize, verticalSize);
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;


}

/*                                                                             
                                                                           
                                                                   
                                 
                                      

                                         
                                                                      

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidSinkType(tmInstance_t instance, tmdlHdmiTxSinkType_t *pSinkType) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                   */
	    RETIF(pSinkType == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                                                               */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxEdidGetSinkType(instance,
							    (tmbslHdmiTxSinkType_t *) pSinkType)) !=
		      TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                               
                                                                   
                                 
                                      

                                                
                                                                                    
               

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidSourceAddress(tmInstance_t instance, UInt16 *pSourceAddress) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                        */
	    RETIF(pSourceAddress == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                                                               */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxEdidGetSourceAddress(instance,
								 pSourceAddress)) != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                    
                                 
                                      

                                            
                                                                            
                                                                       
                                                                    
                                                                     

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                    

                                                                             */
tmErrorCode_t tmdlHdmiTxGetKsvList
    (tmInstance_t instance,
     UInt8 *pKsv,
     UInt8 maxKsv, UInt8 *pWrittenKsv, UInt8 *pDepth, Bool *pMaxCascExd, Bool *pMaxDevsExd) {
	tmErrorCode_t errCode;
#ifndef NO_HDCP
	UInt16 i, j;
#endif

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                                       */
	    RETIF(pKsv == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pWrittenKsv == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pDepth == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pMaxCascExd == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pMaxDevsExd == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                                                         */
	    RETIF_BADPARAM(maxKsv > HDMITX_KSV_LIST_MAX_DEVICES)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                  */
	    * pWrittenKsv = 0;

	/*                             */
	*pDepth = 0;

#ifndef NO_HDCP

	*pMaxCascExd = hdcpInfoListTx[instance].hdcpMaxCascExceeded;
	*pMaxDevsExd = hdcpInfoListTx[instance].hdcpMaxDevsExceeded;


	/*               */
	if (maxKsv) {

		for (j = 0; j < 5; j++) {
			pKsv[j] = hdcpInfoListTx[instance].hdcpBksv[4 - j];
		}
		*pWrittenKsv = *pWrittenKsv + 1;

	}


	/*        */
	/*                   */
	for (i = 1; i <= hdcpInfoListTx[instance].hdcpKsvDevices; i++) {
		if (i < maxKsv) {
			for (j = 0; j < 5; j++) {
				pKsv[(5 * i) + j] =
				    hdcpInfoListTx[instance].hdcpKsvList[(5 * (i - 1)) + j];
			}
			*pWrittenKsv = *pWrittenKsv + 1;
		}
	}

	*pDepth = hdcpInfoListTx[instance].hdcpDeviceDepth;

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
#else
	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TMDL_ERR_DLHDMITX_NOT_SUPPORTED;
#endif				/*         */
}

#ifdef HDMI_TX_REPEATER_ISR_MODE
/*                                                                             
                                                      
                                 
                                      

                                            
                                                                     

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                    

                                                                             */
tmErrorCode_t tmdlHdmiTxGetDepth(tmInstance_t instance, UInt8 *pDepth) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                          */
	    RETIF(pDepth == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                             */
	    * pDepth = 0;

#ifndef NO_HDCP


	*pDepth = hdcpInfoListTx[instance].hdcpDeviceDepth;

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
#else
	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TMDL_ERR_DLHDMITX_NOT_SUPPORTED;
#endif				/*         */
}

/*                                                                             
                                                   
                                 
                                      

                                            


                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                    

                                                                             */
tmErrorCode_t tmdlHdmiTxGeneSHA_1_IT(tmInstance_t instance) {
	tmErrorCode_t errCode;


	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    dlHdmiTxHandleSHA_1(instance);

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}
#endif				/*                           */
/*                                                                             
                                          
                                 
                                      

                                          
                                                           

                            
                                     
                                                                      
                   
                                                               
                                                                
                   
                                                                      
                                                                    
                                                                  
                                                                 
                                                                  
                                                                   
                                                               
                                                                 
                                                                 

                                                                             */
tmErrorCode_t tmdlHdmiTxSetHdcp(tmInstance_t instance, Bool hdcpEnable) {
	tmErrorCode_t errCode;
#ifndef NO_HDCP
	tmdlHdmiTxVidFmtSpecs_t resolutionSpecs;
	tmbslHdmiTxVfreq_t voutFreq;
	tmbslHdmiTxVidFmt_t voutFmt;
	tmbslHdmiTxHdcpTxMode_t txMode;
	tmbslHdmiTxHdcpOptions_t options;
	UInt8 slaveAddress;
	UInt16 i;
#endif
	tmbslHdmiTxRxSense_t rxSenseStatus;	/*                 */
	tmbslHdmiTxHotPlug_t hpdStatus;	/*            */

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                          */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      gtmdlHdmiTxDriverConfigTable[instance].keySeed == HDCP_SEED_NULL,
		      TMDL_ERR_DLHDMITX_NOT_SUPPORTED)

	    /*                                                                   */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxRxSenseGetStatus(instance,
							     &rxSenseStatus, False)) != TM_OK,
		      errCode)

	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxHotPlugGetStatus(instance,
							     &hpdStatus, False)) != TM_OK, errCode)
#ifndef NO_HDCP
	    if (hdcpEnable == True) {	/*         */
		if ((rxSenseStatus == HDMITX_RX_SENSE_ACTIVE)
		    && (hpdStatus == HDMITX_HOTPLUG_ACTIVE)) {

#ifdef TMFL_HDCP_OPTIMIZED_POWER
			tmbslHdmiTxHdcpPowerDown(instance, False);
#endif
			/*                                                           */
			hdcpInfoListTx[instance].hdcpCheckState =
			    TMDL_HDMITX_HDCP_CHECK_IN_PROGRESS;
			hdcpInfoListTx[instance].hdcpErrorState = 0;
			hdcpInfoListTx[instance].hdcpKsvDevices = 0;
			hdcpInfoListTx[instance].bKsvSecure = True;
			for (i = 0; i < TMDL_HDMITX_KSV_BYTES_PER_DEVICE; i++) {
				hdcpInfoListTx[instance].hdcpBksv[i] = 0;
			}
			hdcpInfoListTx[instance].hdcpDeviceDepth = 0;

			hdcpInfoListTx[instance].hdcpMaxCascExceeded = False;
			hdcpInfoListTx[instance].hdcpMaxDevsExceeded = False;

			/*                                  */
			voutFmt =
			    (tmbslHdmiTxVidFmt_t) instanceStatusInfoTx[instance].pVideoInfo->
			    videoOutConfig.format;

			/*                                                   */
			RETIF_SEM(dlHdmiTxItSemaphore[instance],
				  (errCode =
				   tmdlHdmiTxGetVideoFormatSpecs(instance,
								 (tmdlHdmiTxVidFmt_t) voutFmt,
								 &resolutionSpecs)) != TM_OK,
				  errCode)
			    voutFreq = (tmbslHdmiTxVfreq_t) resolutionSpecs.vfrequency;

			/*                */

			/*                        */
			slaveAddress = HDMITX_HDCP_SLAVE_PRIMARY;

			/*                                 */
			if (unitTableTx[instance].repeaterEnable == True) {
				txMode = HDMITX_HDCP_TXMODE_REPEATER;
			} else {
				txMode = HDMITX_HDCP_TXMODE_TOP_LEVEL;
			}

			instanceStatusInfoTx[instance].pColBarState->changeColorBarNow = True;
			instanceStatusInfoTx[instance].pColBarState->colorBarOn = True;
			dlHdmiTxCheckColorBar(instance);

			/*              */
			options = (tmbslHdmiTxHdcpOptions_t) unitTableTx[instance].hdcpOptions;

			RETIF_SEM(dlHdmiTxItSemaphore[instance],
				  (errCode = tmbslHdmiTxHdcpConfigure(instance,
								      slaveAddress, txMode, options,
								      HDCP_CHECK_INTERVAL_MS,
								      HDCP_NUM_CHECKS)) != TM_OK,
				  errCode)

			    /*            */
			    RETIF_SEM(dlHdmiTxItSemaphore[instance],
				      (errCode = tmbslHdmiTxHdcpInit(instance,
								     voutFmt, voutFreq)) != TM_OK,
				      errCode)

			    RETIF_SEM(dlHdmiTxItSemaphore[instance],
				      (errCode = tmbslHdmiTxHdcpRun(instance)
				      ) != TM_OK, errCode)

			    unitTableTx[instance].hdcpEnable = True;

			/*                       */
			RETIF((errCode =
			       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK,
			      errCode)

			    return errCode;
		}

		/*                       */
		RETIF((errCode =
		       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

		    return TMDL_ERR_DLHDMITX_INVALID_STATE;
	} else {		/*          */

		RETIF_SEM(dlHdmiTxItSemaphore[instance], (errCode = tmbslHdmiTxHdcpStop(instance)
			  ) != TM_OK, errCode)

		    unitTableTx[instance].hdcpEnable = False;

		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_HDCP_INACTIVE) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_HDCP_INACTIVE);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}

		/*                       */
		RETIF((errCode =
		       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode);

#ifdef TMFL_HDCP_OPTIMIZED_POWER
		tmbslHdmiTxHdcpPowerDown(instance, True);
#endif
		return TM_OK;
	}
#else
	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TMDL_ERR_DLHDMITX_NOT_SUPPORTED;
#endif				/*         */
}

/*                                                                             
                                     
                                 
                                      

                                                 
                                                                                           

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                    

                                                                             */
tmErrorCode_t tmdlHdmiTxGetHdcpState(tmInstance_t instance, tmdlHdmiTxHdcpCheck_t *pHdcpCheckState) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance], pHdcpCheckState == Null,
		      TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
#ifndef NO_HDCP
	    /*                                */
	    * pHdcpCheckState = hdcpInfoListTx[instance].hdcpCheckState;

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
#else
	    * pHdcpCheckState = TMDL_HDMITX_HDCP_CHECK_NOT_STARTED;

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TMDL_ERR_DLHDMITX_NOT_SUPPORTED;
#endif				/*         */
}

/*                                                                             
                                                                    
                                                                       
                                                                     
                                                     
                                 
                                      

                                                    
                                                                            
                                

                            
                                     
                                                                      
                   
                                                               
                                                                    
                                                                  
                                                                 
                                                                  
                                                               
                                                                
                                                                 

                                                                             */
tmErrorCode_t tmdlHdmiTxHdcpCheck(tmInstance_t instance, UInt16 timeSinceLastCall) {
	tmErrorCode_t errCode;
	Bool featureSupported;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode = tmbslHdmiTxHwGetCapabilities(instance,
							      HDMITX_FEATURE_HW_HDCP,
							      &featureSupported)) != TM_OK, errCode)
#ifndef NO_HDCP
	    dlHdmiTxCheckColorBar(instance);
	dlHdmiTxCheckHdcpColorBar(instance);

	if (featureSupported == True) {
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxHdcpCheck(instance,
							  timeSinceLastCall,
							  (tmbslHdmiTxHdcpCheck_t *) &
							  (hdcpInfoListTx[instance].
							   hdcpCheckState))) != TM_OK, errCode)
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
#else
	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TMDL_ERR_DLHDMITX_NOT_SUPPORTED;
#endif				/*         */
}

/*                                                                             
                                                                           
                                                                     
             
                                 
                                      

                                            
                                                                       
                                                                          
              

                            
                                     
                                                                      
                   
                                                               
                                                                
                   
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
tmErrorCode_t tmdlHdmiTxSetGamutPacket
    (tmInstance_t instance, Bool enable, tmdlHdmiTxGamutData_t *pGamutData) {
	tmErrorCode_t errCode;
	tmbslHdmiTxPktGamut_t pkt;
	UInt8 i;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    if (enable == True) {
		/*                                    */
		RETIF_SEM(dlHdmiTxItSemaphore[instance], pGamutData == Null,
			  TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

		    /*           */
		    pkt.HB[0] = 0x0A;

		pkt.HB[1] = 0x00;
		pkt.HB[1] = (UInt8) (pkt.HB[1] | ((pGamutData->nextField & 0x01) << 7));
		pkt.HB[1] |= (pGamutData->GBD_Profile & 0x07) << 4;
		pkt.HB[1] |= (pGamutData->affectedGamutSeqNum & 0x0F);

		pkt.HB[2] = 0x00;
		pkt.HB[2] = (UInt8) (pkt.HB[2] | ((pGamutData->noCurrentGBD & 0x01) << 7));
		pkt.HB[2] |= (pGamutData->packetSequence & 0x03) << 4;
		pkt.HB[2] |= (pGamutData->currentGamutSeqNum & 0x0F);

		for (i = 0; i < 28; i++) {
			pkt.PB[i] = pGamutData->payload[i];
		}

		/*                       */
		if (((pGamutData->payload[0]) & 0x03) == 2) {
			instanceStatusInfoTx[instance].pGamutState->wideGamutColorSpace =
			    TMDL_HDMITX_EXT_COLORIMETRY_XVYCC709;
		} else {
			instanceStatusInfoTx[instance].pGamutState->wideGamutColorSpace =
			    TMDL_HDMITX_EXT_COLORIMETRY_XVYCC601;
		}

		/*                            */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktFillGamut(instance,
							     &pkt,
							     instanceStatusInfoTx[instance].
							     pGamutState->gamutBufNum)) != TM_OK,
			  errCode)

		    /*                                    */
		    RETIF_SEM(dlHdmiTxItSemaphore[instance],
			      (errCode = tmbslHdmiTxPktSendGamut(instance,
								 instanceStatusInfoTx[instance].
								 pGamutState->gamutBufNum,
								 enable)) != TM_OK, errCode)

		    /*                               */
		    if (instanceStatusInfoTx[instance].pGamutState->gamutBufNum == 0) {
			instanceStatusInfoTx[instance].pGamutState->gamutBufNum = 1;
		} else {
			instanceStatusInfoTx[instance].pGamutState->gamutBufNum = 0;
		}
	} else {
		/*                                     */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSendGamut(instance,
							     0, enable)) != TM_OK, errCode)
	}

	/*                    */
	instanceStatusInfoTx[instance].pGamutState->gamutOn = enable;
	if (enable)
		instanceStatusInfoTx[instance].pGamutState->extColOn = False;

	/*                   */
	RETIF_SEM(dlHdmiTxItSemaphore[instance],
		  (errCode = dlHdmiTxSetVideoInfoframe(instance,
						       instanceStatusInfoTx[instance].pVideoInfo->
						       videoOutConfig.format,
						       instanceStatusInfoTx[instance].pVideoInfo->
						       videoOutConfig.mode)) != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                                               
                                                                    
                                                                    
                                                                             
                                                                        
                                                                           
                                                                   

                                 
                                      

                                                     
                                                                     
                                                                                      
                                                       
                                                                                   
                

                            
                                    
                                                                  
                                                                   
                                                                            
                                                                
                                                                 
                                                                  
                                                                 
                                                                 
                                                                     

                                                                             */
tmErrorCode_t tmdlHdmiTxSetExtendedColorimetry
    (tmInstance_t instance,
     Bool enable,
     tmdlHdmiTxExtColorimetry_t extendedColorimetry,
     tmdlHdmiTxYCCQR_t yccQR, tmdlHdmiTxGamutData_t *pGamutData) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                                                                      */
	    if (enable)		/*                                            */
	{
		RETIF(extendedColorimetry >= TMDL_HDMITX_EXT_COLORIMETRY_INVALID,
		      TMDL_ERR_DLHDMITX_BAD_PARAMETER)
		    RETIF(yccQR >= TMDL_HDMITX_YQR_INVALID, TMDL_ERR_DLHDMITX_BAD_PARAMETER)
	}

	/*                    */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                                      */
	    if (enable)
		instanceStatusInfoTx[instance].pGamutState->yccQR = yccQR;

	/*                                                            */
	if (((enable == True) && ((extendedColorimetry == TMDL_HDMITX_EXT_COLORIMETRY_XVYCC601) ||
				  (extendedColorimetry == TMDL_HDMITX_EXT_COLORIMETRY_XVYCC709)))
	    || ((enable == False) && (instanceStatusInfoTx[instance].pGamutState->gamutOn == True))) {
		/*                                                                                    */
		if (enable)
			instanceStatusInfoTx[instance].pGamutState->extColOn = False;

		/*                       */
		RETIF((errCode =
		       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

		    /*                                          */
		    RETIF((errCode =
			   tmdlHdmiTxSetGamutPacket(instance, enable, pGamutData)) != TM_OK,
			  errCode)

		    return TM_OK;
	}

	/*                                                                   */
	if (instanceStatusInfoTx[instance].pGamutState->gamutOn == True) {
		/*                                     */
		RETIF_SEM(dlHdmiTxItSemaphore[instance],
			  (errCode = tmbslHdmiTxPktSendGamut(instance, 0, False)) != TM_OK, errCode)

		    instanceStatusInfoTx[instance].pGamutState->gamutOn = False;
	}

	/*                                                                             */
	if (enable)
		instanceStatusInfoTx[instance].pGamutState->wideGamutColorSpace =
		    extendedColorimetry;
	instanceStatusInfoTx[instance].pGamutState->extColOn = enable;

	/*                   */
	RETIF_SEM(dlHdmiTxItSemaphore[instance],
		  (errCode = dlHdmiTxSetVideoInfoframe(instance,
						       instanceStatusInfoTx[instance].pVideoInfo->
						       videoOutConfig.format,
						       instanceStatusInfoTx[instance].pVideoInfo->
						       videoOutConfig.mode)) != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                                             
                                 
                                      

                                            
                                                                          
                                                  

                            
                                                           
                                                     
                                      
                                                                             
                                                                                     
                                                         
                                                                      
                   
                                                               
                                                                   
                   

                                                                             */

tmErrorCode_t tmdlHdmiTxSetHDCPRevocationList(tmInstance_t instance, void *listPtr, UInt32 length)
{
	tmErrorCode_t errCode = TM_OK;
#ifndef NO_HDCP
	tmErrorCode_t errCodeSem = TM_OK;
	UInt8 aCounter = 0;
	UInt8 indexKSVList = 0;
	UInt8 i;
	Bool bIsSecure = True;
#endif

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                  */
	    RETIF((listPtr == Null) || (length == 0), TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
#ifndef NO_HDCP
	    /*                       */
	    /*                       */
	    /*                       */
	    RETIF((errCodeSem =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCodeSem)

	    /*                          */
	    unitTableTx[instance].revocationList.pList = (UInt8 *) listPtr;
	unitTableTx[instance].revocationList.length = length;

	/*                               */
	for (i = 0; i < TMDL_HDMITX_KSV_BYTES_PER_DEVICE; i++) {
		if (hdcpInfoListTx[instance].hdcpBksv[i] == 0)
			aCounter++;
	}

	/*                            */
	if (aCounter != TMDL_HDMITX_KSV_BYTES_PER_DEVICE) {

		dlHdmiTxCheckHdcpBksv(instance, hdcpInfoListTx[instance].hdcpBksv, &bIsSecure,
				      True);

		/*                */
		if (bIsSecure == True) {

			/*                            */
			if (unitTableTx[instance].repeaterEnable == False) {

				/*                            */
				if (hdcpInfoListTx[instance].hdcpKsvDevices) {

					while ((indexKSVList < TMDL_HDMITX_KSV_LIST_MAX_DEVICES) &&
					       (indexKSVList <
						hdcpInfoListTx[instance].hdcpKsvDevices)
					       && (bIsSecure == True)) {

						dlHdmiTxCheckHdcpBksv(instance,
								      &(hdcpInfoListTx[instance].
									hdcpKsvList[indexKSVList *
										    TMDL_HDMITX_KSV_BYTES_PER_DEVICE]),
								      &bIsSecure, False);
						indexKSVList++;
					}

					if (bIsSecure == True) {
						errCode = TMDL_DLHDMITX_HDCP_SECURE;
					} else {
						errCode = TMDL_DLHDMITX_HDCP_NOT_SECURE;
					}
				} else {	/*                         */

					/*                                                                  */
					errCode = TMDL_DLHDMITX_HDCP_SECURE;
				}

			} else {	/*                   */
				errCode = TMDL_ERR_DLHDMITX_INVALID_STATE;
			}

		} else {	/*                 */

			errCode = TMDL_DLHDMITX_HDCP_NOT_SECURE;
		}

	} else {
		/*                                       */
		errCode = TM_OK;
	}

	/*                             */
	/*                       */
	/*                             */
	RETIF((errCodeSem =
	       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCodeSem)
#else
	    (void)instance;	/*                         */
#endif				/*         */
	return errCode;
}

/*                                                                             
                                            
                                 
                                      

                                            

                            
                                                           
                                                     
                                      
                                                                      
                   


                                                                             */

tmErrorCode_t tmdlHdmiTxSetBScreen(tmInstance_t instance, tmdlHdmiTxTestPattern_t pattern)
{

	tmErrorCode_t errCodeSem = TM_OK;
	tmErrorCode_t errCode = TM_OK;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                       */
	    /*                       */
	    /*                       */
	    RETIF((errCodeSem =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCodeSem)

	    gtmdlHdmiTxDriverConfigTable[instance].pattern = pattern;

	/*                                                                     */
	(void)dlHdmiTxSetTestPatternOn(instance,
				       instanceStatusInfoTx[instance].pVideoInfo->videoOutConfig.
				       format,
				       instanceStatusInfoTx[instance].pVideoInfo->videoOutConfig.
				       mode, gtmdlHdmiTxDriverConfigTable[instance].pattern);

	/*                             */
	/*                       */
	/*                             */
	RETIF((errCodeSem =
	       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCodeSem)

	    return errCode;

}

/*                                                                             
                                                    
                                 
                                      

                                            

                            
                                                           
                                                     
                                      
                                                                      
                   


                                                                             */

tmErrorCode_t tmdlHdmiTxRemoveBScreen(tmInstance_t instance)
{

	tmErrorCode_t errCodeSem = TM_OK;
	tmErrorCode_t errCode = TM_OK;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                       */
	    /*                       */
	    /*                       */
	    RETIF((errCodeSem =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCodeSem)

	    /*                                     */
	    (void)dlHdmiTxSetTestPatternOff(instance,
					    instanceStatusInfoTx[instance].pVideoInfo->
					    videoOutConfig.format,
					    instanceStatusInfoTx[instance].pVideoInfo->
					    videoOutConfig.mode);

	/*                             */
	/*                       */
	/*                             */
	RETIF((errCodeSem =
	       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCodeSem)

	    return errCode;

}

/*                                                                             
                                      

                                            

               

                                                                             */
tmdlHdmiTxVidFmt_t tmdlHdmiTxConvertDTDtoCEA
    (tmInstance_t instance, tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors) {

	tmdlHdmiTxVidFmt_t codeCEA;
	tmdlHdmiTxPictAspectRatio_t pictureAspectRatio;
	Bool formatInterlaced;

	/*                       */
	/*                       */
	/*                       */
	tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);

	formatInterlaced = False;

	if ((pDTDescriptors->flags) & 0x80) {
		formatInterlaced = True;
	}

	pictureAspectRatio =
	    dlHdmiTxCalcAspectRatio(pDTDescriptors->hImageSize, pDTDescriptors->vImageSize);

	switch (pDTDescriptors->hActivePixels) {
	case 640:
		codeCEA = dlHdmiTxConvertDTDtoCEA_640HAP(pDTDescriptors);
		break;

	case 720:
		codeCEA = dlHdmiTxConvertDTDtoCEA_720HAP(pDTDescriptors, pictureAspectRatio);
		break;

	case 1280:
		codeCEA = dlHdmiTxConvertDTDtoCEA_1280HAP(pDTDescriptors);
		break;

	case 1920:
		codeCEA = dlHdmiTxConvertDTDtoCEA_1920HAP(pDTDescriptors, formatInterlaced);
		break;

	case 1440:
		codeCEA =
		    dlHdmiTxConvertDTDtoCEA_1440HAP(pDTDescriptors, pictureAspectRatio,
						    formatInterlaced);
		break;

	case 2880:
		codeCEA =
		    dlHdmiTxConvertDTDtoCEA_2880HAP(pDTDescriptors, pictureAspectRatio,
						    formatInterlaced);
		break;

	default:
		/*                    */
		codeCEA = TMDL_HDMITX_VFMT_NULL;
		break;
	}

	/*                             */
	/*                       */
	/*                             */
	tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);

	return codeCEA;

}

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                             
                                                           
                              

                                           
                                         
                                            
                                                     
                                         
                                          
                                           
                                          
                                           
                                                                              
                                    

                                              

                                                                             */
static Bool dlHdmiTxGetReflineRefpix
    (tmdlHdmiTxVidFmt_t vinFmt,
     tmdlHdmiTxVinMode_t vinMode,
     tmdlHdmiTxVidFmt_t voutFmt,
     UInt8 syncIn,
     tmdlHdmiTxPixRate_t pixRate,
     UInt16 *pRefPix,
     UInt16 *pRefLine, UInt16 *pScRefPix, UInt16 *pScRefLine, Bool *pbVerified) {
	UInt8 shortVinFmt;
	UInt8 shortVoutFmt;
	int i;
	Bool bFound;
	tmdlHdmiTxVidFmt_t vinFmtIndex, voutFmtIndex;

	/*                                                                    
                                     */
	*pRefPix = 0;
	*pRefLine = 0;
	*pScRefPix = 0;
	*pScRefLine = 0;

	/*                                                                    
                */
	*pbVerified = 1;
	bFound = False;

	if ((voutFmt < TMDL_HDMITX_VFMT_TV_NO_REG_MIN)
	    || ((voutFmt >= HDMITX_VFMT_35_2880x480p_60Hz)
		&& (voutFmt <= HDMITX_VFMT_38_2880x576p_50Hz))) {
		vinFmtIndex = dlHdmiTxCalcVidFmtIndex(vinFmt);
		voutFmtIndex = dlHdmiTxCalcVidFmtIndex(voutFmt);
		shortVinFmt = kVfmtToShortFmt_TV[vinFmtIndex];
		shortVoutFmt = kVfmtToShortFmt_TV[voutFmtIndex];

		for (i = 0; kRefpixRefline[i].shortVinFmt != TV_INVALID; i++) {
			if ((kRefpixRefline[i].shortVinFmt == shortVinFmt)
			    && (UNPKMODE(kRefpixRefline[i].modeRateSyncVerf) == vinMode)
			    && (kRefpixRefline[i].shortVoutFmt == shortVoutFmt)
			    && (UNPKRATE(kRefpixRefline[i].modeRateSyncVerf) == pixRate)
			    && (UNPKSYNC(kRefpixRefline[i].modeRateSyncVerf) == syncIn)) {
				*pRefPix = kRefpixRefline[i].refPix;
				*pRefLine = kRefpixRefline[i].refLine;
				*pScRefPix = kRefpixRefline[i].scRefPix;
				*pScRefLine = kRefpixRefline[i].scRefLine;
				*pbVerified = UNPKVERF(kRefpixRefline[i].modeRateSyncVerf);
				bFound = True;
				break;
			}
		}
	}

	return bFound;
}

/*                                                                             
                                   

                                            
                                            
                                          

                            
                                     
                                                                      
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  

                                                                             */
static tmErrorCode_t dlHdmiTxSetVideoInfoframe
    (tmInstance_t instance, tmdlHdmiTxVidFmt_t voutFmt, tmdlHdmiTxVoutMode_t voutMode) {
	tmErrorCode_t errCode;
	tmdlHdmiTxAviIfData_t contentVif;
	tmdlHdmiTxVidFmt_t voutFmtIndex;


	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    contentVif.colorIndicator = voutMode;	/*                                 */
	contentVif.activeInfoPresent = 0;
	contentVif.barInformationDataValid = 0;
	contentVif.scanInformation = 0;

	voutFmtIndex = dlHdmiTxCalcVidFmtIndex(voutFmt);
	contentVif.pictureAspectRatio = kVfmtToAspect_TV[voutFmtIndex];

	contentVif.activeFormatAspectRatio = 8;
	contentVif.nonUniformPictureScaling = 0;

#ifdef FORMAT_PC
	if (voutFmt >= TMDL_HDMITX_VFMT_PC_MIN) {
		if (voutFmt == TMDL_HDMITX_VFMT_PC_640x480p_60Hz) {
			contentVif.videoFormatIdentificationCode =
			    (tmbslHdmiTxVidFmt_t) TMDL_HDMITX_VFMT_01_640x480p_60Hz;
		} else {
			/*                                */
			contentVif.videoFormatIdentificationCode =
			    (tmbslHdmiTxVidFmt_t) TMDL_HDMITX_VFMT_NULL;
		}
	} else {
#endif				/*           */

		contentVif.videoFormatIdentificationCode = (tmbslHdmiTxVidFmt_t) voutFmt;

#ifdef FORMAT_PC
	}
#endif				/*           */


	if (((voutFmt >= TMDL_HDMITX_VFMT_06_720x480i_60Hz)
	     && (voutFmt <= TMDL_HDMITX_VFMT_09_720x240p_60Hz))
	    || ((voutFmt >= TMDL_HDMITX_VFMT_21_720x576i_50Hz)
		&& (voutFmt <= TMDL_HDMITX_VFMT_24_720x288p_50Hz))) {
		/*                                                                                   */
		contentVif.pixelRepetitionFactor = 1;
	} else if ((voutFmt == TMDL_HDMITX_VFMT_10_720x480i_60Hz)
		   || (voutFmt == TMDL_HDMITX_VFMT_11_720x480i_60Hz)
		   || (voutFmt == TMDL_HDMITX_VFMT_25_720x576i_50Hz)
		   || (voutFmt == TMDL_HDMITX_VFMT_26_720x576i_50Hz)) {
		contentVif.pixelRepetitionFactor = HDMITX_PIXREP_3;	/*                                        */
	} else if ((voutFmt == TMDL_HDMITX_VFMT_14_1440x480p_60Hz)
		   || (voutFmt == TMDL_HDMITX_VFMT_15_1440x480p_60Hz)
		   || (voutFmt == TMDL_HDMITX_VFMT_29_1440x576p_50Hz)
		   || (voutFmt == TMDL_HDMITX_VFMT_30_1440x576p_50Hz)) {
		contentVif.pixelRepetitionFactor = HDMITX_PIXREP_1;	/*                                       */
	} else if ((voutFmt >= TMDL_HDMITX_VFMT_35_2880x480p_60Hz)
		   && (voutFmt <= TMDL_HDMITX_VFMT_38_2880x576p_50Hz)) {
		contentVif.pixelRepetitionFactor = HDMITX_PIXREP_3;
	} else {		/*                                            */

		contentVif.pixelRepetitionFactor = HDMITX_PIXREP_NONE;
	}

	if ((instanceStatusInfoTx[instance].pGamutState->gamutOn == True) ||
	    (instanceStatusInfoTx[instance].pGamutState->extColOn == True)) {
		contentVif.colorimetry = (UInt8) TMDL_HDMITX_COLORIMETRY_EXTENDED;
	} else {
		switch (voutFmt) {
		case TMDL_HDMITX_VFMT_04_1280x720p_60Hz:
		case TMDL_HDMITX_VFMT_05_1920x1080i_60Hz:
		case TMDL_HDMITX_VFMT_16_1920x1080p_60Hz:
		case TMDL_HDMITX_VFMT_19_1280x720p_50Hz:
		case TMDL_HDMITX_VFMT_20_1920x1080i_50Hz:
		case TMDL_HDMITX_VFMT_31_1920x1080p_50Hz:
			contentVif.colorimetry = (UInt8) TMDL_HDMITX_COLORIMETRY_ITU709;
			break;

		default:
			contentVif.colorimetry = (UInt8) TMDL_HDMITX_COLORIMETRY_ITU601;
			break;
		}
	}

	contentVif.lineNumberEndTopBar = 0;
	contentVif.lineNumberStartBottomBar = 0;
	contentVif.lineNumberEndLeftBar = 0;
	contentVif.lineNumberStartRightBar = 0;

	errCode = dlHdmiTxSetRawVideoInfoframe(instance, &contentVif, True);

	/*                                        */
	if (errCode == TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED) {
		errCode = TM_OK;
	}

	return errCode;
}

/*                                                                             
                                   

                                            
                                            
                                          

                            
                                     
                                                                      
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
static tmErrorCode_t dlHdmiTxSetRawVideoInfoframe
    (tmInstance_t instance, tmdlHdmiTxAviIfData_t *pContentVif, Bool enable) {

	tmErrorCode_t errCode;
	tmbslHdmiTxPktRawAvi_t PktInfoFrame;
	UInt8 i;

	if (pContentVif != Null) {

		for (i = 0; i < sizeof(PktInfoFrame.PB); i++) {
			PktInfoFrame.PB[i] = 0;
		}

		/*                    */
		PktInfoFrame.HB[0] = 0x82;	/*                 */
		PktInfoFrame.HB[1] = 0x02;	/*                      */
		PktInfoFrame.HB[2] = 0x0D;	/*                   */

		/*                                               */
		PktInfoFrame.PB[0] = 0;	/*                                               */
		PktInfoFrame.PB[1] = ((pContentVif->colorIndicator & 0x03) << 5) |	/*                 */
		    ((pContentVif->barInformationDataValid & 0x03) << 2) |
		    (pContentVif->scanInformation & 0x03);
		if (pContentVif->activeInfoPresent == True) {
			PktInfoFrame.PB[1] += 0x10;	/*        */
		}

		PktInfoFrame.PB[2] = ((pContentVif->colorimetry & 0x03) << 6) |	/*                  */
		    ((pContentVif->pictureAspectRatio & 0x03) << 4) |
		    (pContentVif->activeFormatAspectRatio & 0x0F);

		PktInfoFrame.PB[3] = (pContentVif->nonUniformPictureScaling & 0x03);	/*       *//*            */

		/*                                                                       */
		/*                                                                                 */

		if (pContentVif->colorimetry == TMDL_HDMITX_COLORIMETRY_EXTENDED) {
			PktInfoFrame.PB[3] =
			    ((((UInt8) instanceStatusInfoTx[instance].pGamutState->
			       wideGamutColorSpace) & 0x07) << 4)
			    | PktInfoFrame.PB[3];
		}

		/*                                                                          */
		/*                                  */

		PktInfoFrame.PB[4] = (pContentVif->videoFormatIdentificationCode & 0x7F);	/*        */

		PktInfoFrame.PB[5] = (pContentVif->pixelRepetitionFactor & 0x0F);	/*       */

		/*                       */
		/*                             */

		/*                                                                                */
		if (pContentVif->colorimetry == TMDL_HDMITX_COLORIMETRY_EXTENDED) {
			PktInfoFrame.PB[5] |=
			    (((UInt8) instanceStatusInfoTx[instance].pGamutState->
			      yccQR) & 0x03) << 6;
		}
		PktInfoFrame.PB[6] = (UInt8) (pContentVif->lineNumberEndTopBar & 0x00FF);
		PktInfoFrame.PB[7] = (UInt8) ((pContentVif->lineNumberEndTopBar & 0xFF00) >> 8);
		PktInfoFrame.PB[8] = (UInt8) (pContentVif->lineNumberStartBottomBar & 0x00FF);
		PktInfoFrame.PB[9] =
		    (UInt8) ((pContentVif->lineNumberStartBottomBar & 0xFF00) >> 8);
		PktInfoFrame.PB[10] = (UInt8) (pContentVif->lineNumberEndLeftBar & 0x00FF);
		PktInfoFrame.PB[11] = (UInt8) ((pContentVif->lineNumberEndLeftBar & 0xFF00) >> 8);
		PktInfoFrame.PB[12] = (UInt8) (pContentVif->lineNumberStartRightBar & 0x00FF);
		PktInfoFrame.PB[13] =
		    (UInt8) ((pContentVif->lineNumberStartRightBar & 0xFF00) >> 8);

		/*                                                                 
                                                                     
                  
   */
		PktInfoFrame.PB[0] = dlHdmiTxcalculateCheksumIF(&PktInfoFrame);

		errCode = tmbslHdmiTxPktSetRawVideoInfoframe(instance, &PktInfoFrame, enable);
	} else {
		errCode = tmbslHdmiTxPktSetVideoInfoframe(instance, Null, enable);
	}

	return errCode;

}

/*                                                                            */
/*                                                                            */
/*                                                                            */
static UInt8 dlHdmiTxcalculateCheksumIF(tmbslHdmiTxPktRawAvi_t *pData	/*                          */
    ) {
	UInt8 checksum = 0;	/*                              */
	UInt8 result = 0;	/*                      */
	UInt8 numBytes = 0;
	Int i;

	if (pData != Null) {

		numBytes = sizeof(pData->HB);

		for (i = 0; i < numBytes; i++) {
			checksum = checksum + pData->HB[i];
		}

		numBytes = sizeof(pData->PB);

		for (i = 0; i < numBytes; i++) {
			checksum = checksum + pData->PB[i];
		}

		result = (UInt8) ((255 - checksum) + 1);
	}
	return result;		/*                                              */
}

/*                                                                             
                                                           

                                            
                                            
                                          

                            
                                     
                                                                      
                   
                                                                  
                                                                 
                                                                  
                                                                   

                                                                             */
tmErrorCode_t dlHdmiTxSetTestPatternOn
    (tmInstance_t instance,
     tmdlHdmiTxVidFmt_t voutFmt, tmdlHdmiTxVoutMode_t voutMode, tmdlHdmiTxTestPattern_t pattern) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    RETIF((errCode = tmbslHdmiTxTestSetPattern(instance,
						       (tmbslHdmiTxTestPattern_t) pattern)) !=
		  TM_OK, errCode)

	    if (pattern > TMDL_HDMITX_PATTERN_CBAR8) {
		RETIF((errCode =
		       dlHdmiTxSetVideoInfoframe(instance, voutFmt, voutMode)) != TM_OK, errCode)
	} else {
		/*                                                                                                    */
		RETIF((errCode =
		       dlHdmiTxSetVideoInfoframe(instance, voutFmt,
						 TMDL_HDMITX_VOUTMODE_RGB444)) != TM_OK, errCode)
	}
	return TM_OK;
}

/*                                                                             
                                                                 

                                            
                                            
                                          

                            
                                     
                                                                      
                   
                                                                  
                                                                 
                                                                  
                                                                   

                                                                             */
tmErrorCode_t dlHdmiTxSetTestPatternOff
    (tmInstance_t instance, tmdlHdmiTxVidFmt_t voutFmt, tmdlHdmiTxVoutMode_t voutMode) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    RETIF((errCode = tmbslHdmiTxTestSetPattern(instance,
						       (tmbslHdmiTxTestPattern_t)
						       TMDL_HDMITX_PATTERN_OFF)) != TM_OK, errCode)

	    /*                         */
	    RETIF((errCode =
		   dlHdmiTxSetVideoInfoframe(instance, voutFmt, voutMode)) != TM_OK, errCode)

	    return TM_OK;
}

/*                                                                             
                                           

                                            

               

                                                                             */
static void dlHdmiTxHandleENCRYPT(tmInstance_t instance) {
#ifndef NO_HDCP

	tmbslHdmiTxHdcpHandleENCRYPT(instance);

	if (instanceStatusInfoTx[instance].pColBarState->disableColorBarOnR0 == False) {
		instanceStatusInfoTx[instance].pColBarState->hdcpColbarChange = False;
		instanceStatusInfoTx[instance].pColBarState->hdcpEncryptOrT0 = True;
	}
	instanceStatusInfoTx[instance].pColBarState->disableColorBarOnR0 = False;
#else
	(void)instance;		/*                         */
#endif				/*         */
}

/*                                                                             
                                  

                                            

               

                                                                             */
static void dlHdmiTxHandleHPD(tmInstance_t instance) {
	tmErrorCode_t errCode;
	tmbslHdmiTxHotPlug_t hpdStatus;	/*            */
	tmPowerState_t powerState;	/*                            */

	hpdStatus = HDMITX_HOTPLUG_INVALID;

	/*                     */
	errCode = tmbslHdmiTxHotPlugGetStatus(instance, &hpdStatus, False);

	if (errCode != TM_OK)
		return;

	/*                                        */
	errCode = tmbslHdmiTxPowerGetState(instance, &powerState);

	if (errCode != TM_OK)
		return;

	/*                                 */
	if (hpdStatus == HDMITX_HOTPLUG_ACTIVE) {
		/*                              */
		dlHdmiTxSetState(instance, STATE_PLUGGED);

		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_HPD_ACTIVE) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_HPD_ACTIVE);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}

		if (powerState == tmPowerOn) {
			/*                                                      */
			tmbslHdmiTxSysTimerWait(instance, 500);	/*    */

			/*                   */
			errCode = tmbslHdmiTxEdidRequestBlockData(instance,
								  unitTableTx[instance].pEdidBuffer,
								  (Int) ((unitTableTx[instance].
									  edidBufferSize) >> 7),
								  (Int) (unitTableTx[instance].
									 edidBufferSize));

			if (errCode != TM_OK)
				return;
		}
	} else {
#ifndef NO_HDCP
		if (unitTableTx[instance].hdcpEnable == True) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			/*                 */
			(void)tmdlHdmiTxSetHdcp(instance, False);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}
#endif				/*         */

		/*                                */
		dlHdmiTxSetState(instance, STATE_UNPLUGGED);

		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_HPD_INACTIVE) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_HPD_INACTIVE);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}
	}
}

/*                                                                             
                                 

                                            

               

                                                                             */
static void dlHdmiTxHandleT0(tmInstance_t instance) {
#ifndef NO_HDCP
	tmErrorCode_t errCode;

	errCode = tmbslHdmiTxHdcpHandleT0(instance);

	if (errCode != TM_OK)
		return;

	tmbslHdmiTxHdcpGetT0FailState(instance, &(hdcpInfoListTx[instance].hdcpErrorState));

	if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_HDCP_INACTIVE) ==
	    TMDL_HDMITX_EVENT_ENABLED) {
		/*                       */
		(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
		unitTableTx[instance].pCallback(TMDL_HDMITX_HDCP_INACTIVE);
		/*                    */
		(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
	}

	instanceStatusInfoTx[instance].pColBarState->hdcpColbarChange = False;
	instanceStatusInfoTx[instance].pColBarState->hdcpEncryptOrT0 = True;
	instanceStatusInfoTx[instance].pColBarState->hdcpSecureOrT0 = True;
#else
	(void)instance;
#endif				/*         */
}

/*                                                                             
                                    

                                            

               

                                                                             */
static void dlHdmiTxHandleBCAPS(tmInstance_t instance) {
#ifndef NO_HDCP
	Bool bCheckRequired;
	tmErrorCode_t errCode;

	/*                                    */
	errCode = tmbslHdmiTxHdcpHandleBCAPS(instance);

	if (errCode != TM_OK)
		return;

	/*                                             */
	tmbslHdmiTxSysTimerWait(instance, 10);

	/*                  */
	errCode = tmbslHdmiTxHdcpHandleBKSV(instance,
					    hdcpInfoListTx[instance].hdcpBksv, &bCheckRequired);

	if (errCode != TM_OK)
		return;

	if (bCheckRequired) {
		/*                                          */
		dlHdmiTxCheckHdcpBksv(instance, hdcpInfoListTx[instance].hdcpBksv,
				      &(hdcpInfoListTx[instance].bKsvSecure), True);
	} else {
		/*                                              */
		hdcpInfoListTx[instance].bKsvSecure = True;
	}

	/*                    */
	errCode = tmbslHdmiTxHdcpHandleBKSVResult(instance, hdcpInfoListTx[instance].bKsvSecure);

	if (errCode != TM_OK)
		return;


#else
	(void)instance;
#endif				/*         */
}

/*                                                                             
                                      

                                            

               

                                                                             */
static void dlHdmiTxHandleBSTATUS(tmInstance_t instance) {
#ifndef NO_HDCP
	UInt16 bstatus = 0;

	tmbslHdmiTxHdcpHandleBSTATUS(instance, &bstatus);

#ifdef HDMI_TX_REPEATER_ISR_MODE
	gIgnoreNextSha1 = False;
#endif				/*                          */

	if (((bstatus & HDMITX_HDCP_BSTATUS_MAX_CASCADE_EXCEEDED) > 0)
	    || ((bstatus & HDMITX_HDCP_BSTATUS_MAX_DEVS_EXCEEDED) > 0)) {

		hdcpInfoListTx[instance].hdcpDeviceDepth =
		    (UInt8) ((bstatus & HDMITX_HDCP_BSTATUS_CASCADE_DEPTH) >> 8);

		/*                                                               
                                                          */
		hdcpInfoListTx[instance].hdcpKsvDevices =
		    (UInt8) (bstatus & HDMITX_HDCP_BSTATUS_DEVICE_COUNT);

		if (HDMITX_KSV_LIST_MAX_DEVICES < hdcpInfoListTx[instance].hdcpKsvDevices) {
			hdcpInfoListTx[instance].hdcpKsvDevices = HDMITX_KSV_LIST_MAX_DEVICES;
		}

		if ((bstatus & HDMITX_HDCP_BSTATUS_MAX_CASCADE_EXCEEDED) > 0) {
			hdcpInfoListTx[instance].hdcpMaxCascExceeded = True;
		}

		if ((bstatus & HDMITX_HDCP_BSTATUS_MAX_DEVS_EXCEEDED) > 0) {
			hdcpInfoListTx[instance].hdcpMaxDevsExceeded = True;
		}

		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_RX_KEYS_RECEIVED) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_RX_KEYS_RECEIVED);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}
	} else {

#ifdef HDMI_TX_REPEATER_ISR_MODE
		/*                                         */
		hdcpInfoListTx[instance].hdcpDeviceDepth =
		    (UInt8) ((bstatus & HDMITX_HDCP_BSTATUS_CASCADE_DEPTH) >> 8);

		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_B_STATUS) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_B_STATUS);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}

#endif				/*                           */
	}


#else
	(void)instance;
#endif				/*         */
}

/*                                                                             
                                    

                                            

               

                                                                             */
static void dlHdmiTxHandleSHA_1(tmInstance_t instance) {
#ifndef NO_HDCP
	tmErrorCode_t errCode;
	UInt8 indexKSVList;

#ifdef HDMI_TX_REPEATER_ISR_MODE
	if (gIgnoreNextSha1 == False) {
		gIgnoreNextSha1 = True;
#endif				/*                          */

		errCode = tmbslHdmiTxHdcpHandleSHA_1(instance,
						     HDMITX_KSV_LIST_MAX_DEVICES,
						     hdcpInfoListTx[instance].hdcpKsvList,
						     &(hdcpInfoListTx[instance].hdcpKsvDevices),
						     &(hdcpInfoListTx[instance].hdcpDeviceDepth));
		if (errCode != TM_OK)
			return;

		/*                                 */
		if (unitTableTx[instance].repeaterEnable == False) {
			/*                                           */

			indexKSVList = 0;
			while ((indexKSVList < TMDL_HDMITX_KSV_LIST_MAX_DEVICES) &&
			       (indexKSVList < hdcpInfoListTx[instance].hdcpKsvDevices) &&
			       (hdcpInfoListTx[instance].bKsvSecure == True)
			    ) {
				dlHdmiTxCheckHdcpBksv(instance,
						      &(hdcpInfoListTx[instance].
							hdcpKsvList[indexKSVList *
								    TMDL_HDMITX_KSV_BYTES_PER_DEVICE]),
						      &(hdcpInfoListTx[instance].bKsvSecure),
						      False);
				indexKSVList++;
			}
		} else {
			hdcpInfoListTx[instance].bKsvSecure = True;
		}

		/*                     */
		errCode = tmbslHdmiTxHdcpHandleSHA_1Result(instance,
							   hdcpInfoListTx[instance].bKsvSecure);

		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_RX_KEYS_RECEIVED) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_RX_KEYS_RECEIVED);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}

		if (!hdcpInfoListTx[instance].bKsvSecure) {
			instanceStatusInfoTx[instance].pColBarState->changeColorBarNow = True;
			instanceStatusInfoTx[instance].pColBarState->colorBarOn = True;
			dlHdmiTxCheckColorBar(instance);
		}
#ifdef HDMI_TX_REPEATER_ISR_MODE
	}
#endif				/*                          */


#else
	(void)instance;
#endif				/*         */
}

/*                                                                             
                                 

                                            

               

                                                                             */
static void dlHdmiTxHandlePJ(tmInstance_t instance) {
#ifndef NO_HDCP
	tmbslHdmiTxHdcpHandlePJ(instance);
#else
	(void)instance;
#endif				/*         */
}

/*                                                                             
                                 

                                            

               

                                                                             */
static void dlHdmiTxHandleR0(tmInstance_t instance) {
#ifndef NO_HDCP
	tmErrorCode_t errCode;
	tmbslHdmiTxSinkCategory_t category;



	if (hdcpInfoListTx[instance].bKsvSecure == True) {

		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_HDCP_ACTIVE) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_HDCP_ACTIVE);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}

		instanceStatusInfoTx[instance].pColBarState->hdcpSecureOrT0 = False;
	}


	errCode = tmbslHdmiTxHdcpGetSinkCategory(instance, &category);
	if (errCode != TM_OK)
		return;

	if (category == HDMITX_SINK_CAT_NOT_REPEATER) {
		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_RX_KEYS_RECEIVED) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_RX_KEYS_RECEIVED);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}
	}

	instanceStatusInfoTx[instance].pColBarState->disableColorBarOnR0 = True;
	instanceStatusInfoTx[instance].pColBarState->hdcpColbarChange = True;
#else
	(void)instance;
#endif				/*         */
}

/*                                                                             
                                     

                                            

               

                                                                             */
static void dlHdmiTxHandleSW_INT(tmInstance_t instance) {
	DUMMY_ACCESS(instance);
}

/*                                                                             
                                       

                                            

               

                                                                             */
static void dlHdmiTxHandleRX_SENSE(tmInstance_t instance) {
	tmErrorCode_t errCode;
	tmbslHdmiTxRxSense_t rxSenseStatus;	/*                 */
	tmbslHdmiTxHotPlug_t hpdStatus;	/*            */

	errCode = tmbslHdmiTxRxSenseGetStatus(instance, &rxSenseStatus, False);

	if (errCode != TM_OK)
		return;

	errCode = tmbslHdmiTxHotPlugGetStatus(instance, &hpdStatus, False);

	if (errCode != TM_OK)
		return;

/*                                         */
/*   */
	if (rxSenseStatus == HDMITX_RX_SENSE_ACTIVE) {
		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_RX_DEVICE_ACTIVE) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_RX_DEVICE_ACTIVE);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}
	} else if (rxSenseStatus == HDMITX_RX_SENSE_INACTIVE) {

		if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_RX_DEVICE_INACTIVE) ==
		    TMDL_HDMITX_EVENT_ENABLED) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			unitTableTx[instance].pCallback(TMDL_HDMITX_RX_DEVICE_INACTIVE);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}
#ifndef NO_HDCP
		if (unitTableTx[instance].hdcpEnable == True) {
			/*                       */
			(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
			/*                 */
			(void)tmdlHdmiTxSetHdcp(instance, False);
			/*                    */
			(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
		}
#endif				/*         */
	}
/*   */
}

/*                                                                             
                                        

                                            

               

                                                                             */
static void dlHdmiTxHandleEDID_READ(tmInstance_t instance) {
	tmErrorCode_t errCode;
	UInt8 edidStatus = TMDL_HDMITX_EDID_NOT_READ;

	/*                                                          */

	/*                 */
	errCode = tmbslHdmiTxEdidGetStatus(instance, &edidStatus);

	if (errCode != TM_OK) {
		/*                              */
		dlHdmiTxSetState(instance, STATE_PLUGGED);
		return;
	}

	/*                                 */
	if ((edidStatus == TMDL_HDMITX_EDID_READ) || (edidStatus == TMDL_HDMITX_EDID_ERROR_CHK)) {
		/*                                     */
		dlHdmiTxSetState(instance, STATE_EDID_AVAILABLE);
	} else {
		/*                              */
		dlHdmiTxSetState(instance, STATE_PLUGGED);
	}


	if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_EDID_RECEIVED) ==
	    TMDL_HDMITX_EVENT_ENABLED) {
		/*                       */
		(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
		unitTableTx[instance].pCallback(TMDL_HDMITX_EDID_RECEIVED);
		/*                    */
		(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
	}


}

/*                                                                             
                                     

                                            

               

                                                                             */
static void dlHdmiTxHandleVS_RPT(tmInstance_t instance) {
	if (dlHdmiTxGetEventStatus(instance, TMDL_HDMITX_VS_RPT_RECEIVED) ==
	    TMDL_HDMITX_EVENT_ENABLED) {
		/*                       */
		(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance]);
		unitTableTx[instance].pCallback(TMDL_HDMITX_VS_RPT_RECEIVED);
		/*                    */
		(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance]);
	}
}

/*                                                                             
                                           

                                               
                                            
                                                     

               

                                                                             */
static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_640HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors) {
	tmdlHdmiTxVidFmt_t codeCEA;

	switch (pDTDescriptors->vActiveLines) {
	case 480:
		codeCEA = TMDL_HDMITX_VFMT_01_640x480p_60Hz;
		break;

	default:
		/*                    */
		codeCEA = TMDL_HDMITX_VFMT_NULL;
		break;
	}

	return codeCEA;

}

/*                                                                             
                                           

                                               
                                            
                                                     

               

                                                                             */
static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_720HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors,
     tmdlHdmiTxPictAspectRatio_t pictureAspectRatio) {
	tmdlHdmiTxVidFmt_t codeCEA;

	switch (pDTDescriptors->vActiveLines) {
	case 480:
		if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
			codeCEA = TMDL_HDMITX_VFMT_02_720x480p_60Hz;
		} else {
			codeCEA = TMDL_HDMITX_VFMT_03_720x480p_60Hz;
		}
		break;

	case 576:
		if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
			codeCEA = TMDL_HDMITX_VFMT_17_720x576p_50Hz;
		} else {
			codeCEA = TMDL_HDMITX_VFMT_18_720x576p_50Hz;
		}
		break;

	default:
		/*                    */
		codeCEA = TMDL_HDMITX_VFMT_NULL;
		break;
	}

	return codeCEA;

}

/*                                                                             
                                            

                                               
                                            
                                                     

               

                                                                             */
static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_1280HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors) {
	tmdlHdmiTxVidFmt_t codeCEA;

	switch (pDTDescriptors->vActiveLines) {
	case 720:
		switch (pDTDescriptors->hBlankPixels) {
		case 370:
			codeCEA = TMDL_HDMITX_VFMT_04_1280x720p_60Hz;
			break;

		case 700:
			codeCEA = TMDL_HDMITX_VFMT_19_1280x720p_50Hz;
			break;

		default:
			/*                    */
			codeCEA = TMDL_HDMITX_VFMT_NULL;
			break;
		}
		break;

	default:
		/*                    */
		codeCEA = TMDL_HDMITX_VFMT_NULL;
		break;
	}


	return codeCEA;
}

/*                                                                             
                                            

                                               
                                            
                                                     

               

                                                                             */
static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_1920HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors, Bool formatInterlaced) {
	tmdlHdmiTxVidFmt_t codeCEA;

	switch (pDTDescriptors->hBlankPixels) {
	case 280:
		if (formatInterlaced) {
			codeCEA = TMDL_HDMITX_VFMT_05_1920x1080i_60Hz;
		} else {
			if (pDTDescriptors->pixelClock == 14850) {
				codeCEA = TMDL_HDMITX_VFMT_16_1920x1080p_60Hz;
			} else {
				codeCEA = TMDL_HDMITX_VFMT_34_1920x1080p_30Hz;
			}
		}
		break;

	case 720:
		if (formatInterlaced) {
			codeCEA = TMDL_HDMITX_VFMT_20_1920x1080i_50Hz;
		} else {
			switch (pDTDescriptors->pixelClock) {
			case 14850:
				codeCEA = TMDL_HDMITX_VFMT_31_1920x1080p_50Hz;
				break;

			case 7425:
				codeCEA = TMDL_HDMITX_VFMT_33_1920x1080p_25Hz;
				break;
			default:
				/*                    */
				codeCEA = TMDL_HDMITX_VFMT_NULL;
				break;
			}
		}
		break;

	case 830:
		codeCEA = TMDL_HDMITX_VFMT_32_1920x1080p_24Hz;
		break;

	default:
		/*                    */
		codeCEA = TMDL_HDMITX_VFMT_NULL;
		break;
	}


	return codeCEA;
}

/*                                                                             
                                            

                                               
                                            
                                                     

               

                                                                             */
static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_1440HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors,
     tmdlHdmiTxPictAspectRatio_t pictureAspectRatio, Bool formatInterlaced) {
	tmdlHdmiTxVidFmt_t codeCEA;

	switch (pDTDescriptors->vActiveLines) {
	case 240:
		if (formatInterlaced) {
			if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
				codeCEA = TMDL_HDMITX_VFMT_06_720x480i_60Hz;
			} else {
				codeCEA = TMDL_HDMITX_VFMT_07_720x480i_60Hz;
			}
		} else {
			if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
				codeCEA = TMDL_HDMITX_VFMT_08_720x240p_60Hz;
			} else {
				codeCEA = TMDL_HDMITX_VFMT_09_720x240p_60Hz;
			}
		}
		break;

	case 288:
		if (formatInterlaced) {
			if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
				codeCEA = TMDL_HDMITX_VFMT_21_720x576i_50Hz;
			} else {
				codeCEA = TMDL_HDMITX_VFMT_22_720x576i_50Hz;
			}
		} else {
			if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
				codeCEA = TMDL_HDMITX_VFMT_23_720x288p_50Hz;
			} else {
				codeCEA = TMDL_HDMITX_VFMT_24_720x288p_50Hz;
			}
		}
		break;

	case 480:
		if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
			codeCEA = TMDL_HDMITX_VFMT_14_1440x480p_60Hz;
		} else {
			codeCEA = TMDL_HDMITX_VFMT_15_1440x480p_60Hz;
		}
		break;

	case 576:
		if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
			codeCEA = TMDL_HDMITX_VFMT_29_1440x576p_50Hz;
		} else {
			codeCEA = TMDL_HDMITX_VFMT_30_1440x576p_50Hz;
		}
		break;

	default:
		/*                    */
		codeCEA = TMDL_HDMITX_VFMT_NULL;
		break;
	}

	return codeCEA;
}

/*                                                                             
                                            

                                               
                                            
                                                     

               

                                                                             */
static tmdlHdmiTxVidFmt_t dlHdmiTxConvertDTDtoCEA_2880HAP
    (tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors,
     tmdlHdmiTxPictAspectRatio_t pictureAspectRatio, Bool formatInterlaced) {
	tmdlHdmiTxVidFmt_t codeCEA;

	switch (pDTDescriptors->vActiveLines) {
	case 240:
		if (formatInterlaced) {
			if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
				codeCEA = TMDL_HDMITX_VFMT_10_720x480i_60Hz;
			} else {
				codeCEA = TMDL_HDMITX_VFMT_11_720x480i_60Hz;
			}
		} else {
			if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
				codeCEA = TMDL_HDMITX_VFMT_12_720x240p_60Hz;
			} else {
				codeCEA = TMDL_HDMITX_VFMT_13_720x240p_60Hz;
			}
		}
		break;

	case 288:
		if (formatInterlaced) {
			if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
				codeCEA = TMDL_HDMITX_VFMT_25_720x576i_50Hz;
			} else {
				codeCEA = TMDL_HDMITX_VFMT_26_720x576i_50Hz;
			}
		} else {
			if (pictureAspectRatio == TMDL_HDMITX_P_ASPECT_RATIO_4_3) {
				codeCEA = TMDL_HDMITX_VFMT_27_720x288p_50Hz;
			} else {
				codeCEA = TMDL_HDMITX_VFMT_28_720x288p_50Hz;
			}
		}
		break;

	default:
		/*                    */
		codeCEA = TMDL_HDMITX_VFMT_NULL;
		break;
	}

	return codeCEA;
}

/*                                                                             
                       

            

               

                                                                             */
tmErrorCode_t dlHdmiTxEdidGetDTD
    (tmInstance_t instance,
     tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors, UInt8 maxDTDesc, UInt8 *pWrittenDTDesc) {
	tmErrorCode_t errCode;

	/*                         */
	RETIF(dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE, TMDL_ERR_DLHDMITX_INVALID_STATE)

	    /*                                                                                                 */
	    RETIF((errCode =
		   tmbslHdmiTxEdidGetDetailedTimingDescriptors(instance,
							       (tmbslHdmiTxEdidDtd_t *)
							       pDTDescriptors, maxDTDesc,
							       pWrittenDTDesc)) != TM_OK, errCode);

	return TM_OK;
}


/*                                                                             
                                                                 

              

               

                                                                             */

#ifndef TMFL_NO_RTOS
static void CommandTaskUnit0(void)
{
	UInt8 command;
	Bool loop = True;	/*                                */
	tmErrorCode_t err = TM_OK;

	while (loop) {
		tmdlHdmiTxIWQueueReceive(unitTableTx[0].queueHandle, &command);

		/*                    */
		(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[0]);

		/*                                             */
		instanceStatusInfoTx[0].pColBarState->hdcpSecureOrT0 = False;

		if (gI2CDebugAccessesEnabled == True) {

			err = tmbslHdmiTxHwHandleInterrupt(0);

			if ((err == TMBSL_ERR_HDMI_I2C_WRITE) || (err == TMBSL_ERR_HDMI_I2C_READ)) {

				unitTableTx[0].pCallback(TMDL_HDMITX_DEBUG_EVENT_1);
			}

		}

		/*                                    */
		/*                                                                                    */
		tmdlHdmiTxIWEnableInterrupts(TMDL_HDMI_IW_TX_1);

		/*                       */
		(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[0]);
	};
}
#endif				/*              */

/*                                                                             
                                                         

              

               

                                                                             */
#ifndef TMFL_NO_RTOS
static void HdcpTaskUnit0(void)
{
	Bool loop = True;	/*                                */
	Bool featureSupported;

	tmbslHdmiTxHwGetCapabilities(0, HDMITX_FEATURE_HW_HDCP, &featureSupported);

#ifndef NO_HDCP
	while (loop) {
		(void)tmdlHdmiTxIWWait(35);

		/*                    */
		(void)tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[0]);

		if (gI2CDebugAccessesEnabled == True) {

			dlHdmiTxCheckColorBar(0);
			dlHdmiTxCheckHdcpColorBar(0);

			if (featureSupported == True) {
				tmbslHdmiTxHdcpCheck(0, 35,
						     (tmbslHdmiTxHdcpCheck_t *) &
						     (hdcpInfoListTx[0].hdcpCheckState));
			}

		}

		/*                                   */
		/*                       */
		(void)tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[0]);
	};
#else
	(void)loop;
#endif				/*         */
}
#endif				/*              */

#ifndef NO_HDCP
/*                                                                             
                                                

                                        

               

                                                                             */
static void dlHdmiTxCheckHdcpColorBar(tmInstance_t instance) {
	/*                                                   */
	if ((instanceStatusInfoTx[instance].pColBarState->disableColorBarOnR0 == True)
	    && (instanceStatusInfoTx[instance].pColBarState->hdcpColbarChange == True)
	    && (instanceStatusInfoTx[instance].pColBarState->hdcpSecureOrT0 == False)) {
		/*                                                                    */
		if (instanceStatusInfoTx[instance].pColBarState->colorBarOn != False) {
			instanceStatusInfoTx[instance].pColBarState->colorBarOn = False;
			instanceStatusInfoTx[instance].pColBarState->changeColorBarNow = True;

			if (unitTableTx[instance].simplayHd == True) {

				/*                                  */
				tmbslHdmiTxAudioOutSetMute(instance,
							   (tmbslHdmiTxaMute_t) HDMITX_AMUTE_OFF);

				/*                                 */
				instanceStatusInfoTx[instance].pAudioInfo->audioMuteState = False;
			}


		}
		/*                   */
		instanceStatusInfoTx[instance].pColBarState->hdcpColbarChange = False;
		instanceStatusInfoTx[instance].pColBarState->hdcpSecureOrT0 = True;

#ifdef TMFL_TDA19989
		instanceStatusInfoTx[instance].pColBarState->disableColorBarOnR0 = False;
#endif				/*               */



	}

	if ((instanceStatusInfoTx[instance].pColBarState->hdcpEncryptOrT0 == True)
	    && (instanceStatusInfoTx[instance].pColBarState->inOutFirstSetDone == True)) {
		/*                                                                  */
		if (instanceStatusInfoTx[instance].pColBarState->colorBarOn != True) {
			instanceStatusInfoTx[instance].pColBarState->colorBarOn = True;
			instanceStatusInfoTx[instance].pColBarState->changeColorBarNow = True;

			if (unitTableTx[instance].simplayHd == True) {

				/*                                  */
				tmbslHdmiTxAudioOutSetMute(instance,
							   (tmbslHdmiTxaMute_t) HDMITX_AMUTE_ON);

				/*                                 */
				instanceStatusInfoTx[instance].pAudioInfo->audioMuteState = True;
			}

		}
		/*                  */
		instanceStatusInfoTx[instance].pColBarState->hdcpEncryptOrT0 = False;
	}
}
#endif

#ifndef NO_HDCP
/*                                                                             
                                                            

                                        

               

                                                                             */
static void dlHdmiTxCheckColorBar(tmInstance_t instance) {
	if ((instanceStatusInfoTx[instance].pColBarState->inOutFirstSetDone == True)
	    && (instanceStatusInfoTx[instance].pColBarState->changeColorBarNow == True)) {
		instanceStatusInfoTx[instance].pColBarState->changeColorBarNow = False;

		if (unitTableTx[instance].simplayHd == True) {
			if (instanceStatusInfoTx[instance].pColBarState->colorBarOn == True) {
				/*                                                                     */
				(void)dlHdmiTxSetTestPatternOn(instance,
							       instanceStatusInfoTx[instance].
							       pVideoInfo->videoOutConfig.format,
							       instanceStatusInfoTx[instance].
							       pVideoInfo->videoOutConfig.mode,
							       gtmdlHdmiTxDriverConfigTable
							       [instance].pattern);
			} else {
				/*                                     */
				(void)dlHdmiTxSetTestPatternOff(instance,
								instanceStatusInfoTx[instance].
								pVideoInfo->videoOutConfig.format,
								instanceStatusInfoTx[instance].
								pVideoInfo->videoOutConfig.mode);
			}
		}
	}
}
#endif

#ifndef NO_HDCP
/*                                                                             
                         

                                        

               

                                                                             */
static void dlHdmiTxFindHdcpSeed(tmInstance_t instance) {
#if HDCP_SEED_DEFAULT == HDCP_SEED_NULL
	UInt8 otp[3];
#endif

	/*                                                              */
#if HDCP_SEED_DEFAULT == HDCP_SEED_NULL
	/*                                                  */
	if (kSeedTable[0][0] != 0xFFFF) {
		/*                                                              */
		if ((tmbslHdmiTxHdcpGetOtp(instance, 0x00, otp)) == TM_OK) {
			int i;
			for (i = 0; i < SEED_TABLE_LEN; i++) {
				if (kSeedTable[i][0] == otp[2]) {	/*              */
					/*             */
					gtmdlHdmiTxDriverConfigTable[instance].keySeed =
					    kSeedTable[i][1];
					break;
				}
			}
		}
	}
#endif				/*                                     */

	/*                                  */
	if (gtmdlHdmiTxDriverConfigTable[instance].keySeed != HDCP_SEED_NULL) {
		/*                                           */
		tmbslHdmiTxHdcpDownloadKeys(instance,
					    gtmdlHdmiTxDriverConfigTable[instance].keySeed,
					    HDMITX_HDCP_DECRYPT_ENABLE);
	}
}
#endif				/*         */

/*                                                                             
                                              

                                        
                                               

               

                                                                             */
static void dlHdmiTxSetState(tmInstance_t instance, tmdlHdmiTxDriverState_t state) {
	/*               */
	unitTableTx[instance].state = state;
}

/*                                                                             
                                              

                                        

                                                                       

                                                                             */
tmdlHdmiTxDriverState_t dlHdmiTxGetState(tmInstance_t instance) {
	tmdlHdmiTxDriverState_t state;

	/*               */
	state = unitTableTx[instance].state;

	return (state);
}

/*                                                                             
                                                            

                                        
                                            

               

                                                                             */
static tmdlHdmiTxEventStatus_t dlHdmiTxGetEventStatus
    (tmInstance_t instance, tmdlHdmiTxEvent_t event) {
	tmdlHdmiTxEventStatus_t eventStatus;

	/*                      */
	eventStatus = instanceStatusInfoTx[instance].pEventState[event].status;

	return (eventStatus);
}

/*                                                                             
                                      

                                            
                                          

               

                                                                             */
static tmdlHdmiTxPictAspectRatio_t dlHdmiTxCalcAspectRatio(UInt16 HImageSize, UInt16 VImageSize)
{
	tmdlHdmiTxPictAspectRatio_t pictureAspectRatio;
	UInt16 calcPictureAspectRatio;

	/*                                                                      */
	/*                                                                      */
	/*                                                                      */
	/*                                                                      */
	/*                                                                      */
	/*                                                                      */
	/*                                                                      */

	calcPictureAspectRatio = ((UInt16) (HImageSize + ((VImageSize) >> 1))) >> 1;

	if (calcPictureAspectRatio > VImageSize) {
		pictureAspectRatio = TMDL_HDMITX_P_ASPECT_RATIO_16_9;
	} else {
		pictureAspectRatio = TMDL_HDMITX_P_ASPECT_RATIO_4_3;
	}

	return pictureAspectRatio;

}

#ifndef NO_HDCP
/*                                                                             
                                  

                                        
                                        
                                                                                

               

                                                                             */
static void dlHdmiTxCheckHdcpBksv
    (tmInstance_t instance, UInt8 *pHdcpBksvTested, Bool *pbBksvSecure, Bool bBigEndian) {

	UInt32 NbInRevocationList;

	NbInRevocationList = 0;

	/*                                                           */
	*pbBksvSecure = True;

	if ((unitTableTx[instance].revocationList.pList != Null)
	    && (unitTableTx[instance].revocationList.length > 0)) {
		while ((*pbBksvSecure == True)
		       && (NbInRevocationList < unitTableTx[instance].revocationList.length)) {
			if (bBigEndian) {
				if ((pHdcpBksvTested[0] ==
				     unitTableTx[instance].revocationList.pList[NbInRevocationList *
										HDMITX_KSV_BYTES_PER_DEVICE])
				    && (pHdcpBksvTested[1] ==
					unitTableTx[instance].revocationList.pList[1 +
										   (NbInRevocationList
										    *
										    HDMITX_KSV_BYTES_PER_DEVICE)])
				    && (pHdcpBksvTested[2] ==
					unitTableTx[instance].revocationList.pList[2 +
										   (NbInRevocationList
										    *
										    HDMITX_KSV_BYTES_PER_DEVICE)])
				    && (pHdcpBksvTested[3] ==
					unitTableTx[instance].revocationList.pList[3 +
										   (NbInRevocationList
										    *
										    HDMITX_KSV_BYTES_PER_DEVICE)])
				    && (pHdcpBksvTested[4] ==
					unitTableTx[instance].revocationList.pList[4 +
										   (NbInRevocationList
										    *
										    HDMITX_KSV_BYTES_PER_DEVICE)])
				    ) {
					*pbBksvSecure = False;
				}
			} else {
				if ((pHdcpBksvTested[4] ==
				     unitTableTx[instance].revocationList.pList[NbInRevocationList *
										HDMITX_KSV_BYTES_PER_DEVICE])
				    && (pHdcpBksvTested[3] ==
					unitTableTx[instance].revocationList.pList[1 +
										   (NbInRevocationList
										    *
										    HDMITX_KSV_BYTES_PER_DEVICE)])
				    && (pHdcpBksvTested[2] ==
					unitTableTx[instance].revocationList.pList[2 +
										   (NbInRevocationList
										    *
										    HDMITX_KSV_BYTES_PER_DEVICE)])
				    && (pHdcpBksvTested[1] ==
					unitTableTx[instance].revocationList.pList[3 +
										   (NbInRevocationList
										    *
										    HDMITX_KSV_BYTES_PER_DEVICE)])
				    && (pHdcpBksvTested[0] ==
					unitTableTx[instance].revocationList.pList[4 +
										   (NbInRevocationList
										    *
										    HDMITX_KSV_BYTES_PER_DEVICE)])
				    ) {
					*pbBksvSecure = False;
				}
			}
			NbInRevocationList++;
		}

	}


}
#endif

/*                                                                             
                                   

                                

                        

                                                                             */
static tmdlHdmiTxVidFmt_t dlHdmiTxCalcVidFmtIndex(tmdlHdmiTxVidFmt_t vidFmt)
{
	tmdlHdmiTxVidFmt_t vidFmtIndex = vidFmt;

	/*                                         */
	if ((vidFmt >= TMDL_HDMITX_VFMT_60_1280x720p_24Hz)
	    && (vidFmt <= TMDL_HDMITX_VFMT_62_1280x720p_30Hz)) {
		vidFmtIndex =
		    (tmdlHdmiTxVidFmt_t) (TMDL_HDMITX_VFMT_INDEX_60_1280x720p_24Hz +
					  (vidFmt - TMDL_HDMITX_VFMT_60_1280x720p_24Hz));
	}
#ifdef FORMAT_PC
	else if (vidFmt >= TMDL_HDMITX_VFMT_PC_MIN) {
		vidFmtIndex =
		    (tmdlHdmiTxVidFmt_t) (TMDL_HDMITX_VFMT_TV_NUM +
					  (vidFmt - TMDL_HDMITX_VFMT_PC_MIN));
	}
#endif				/*           */
	return (vidFmtIndex);
}


tmErrorCode_t tmdlHdmiTxDebugEnableI2CAccesses(tmInstance_t instance, Bool enableI2C)
{
	tmErrorCode_t errCode = TM_OK;

	/*                                      */
	if ((instance < 0) || (instance >= MAX_UNITS)) {
		errCode = TMDL_ERR_DLHDMITX_BAD_INSTANCE;
		return errCode;
	}

	if (enableI2C == True) {
		errCode = tmbslDebugWriteFakeRegPage(instance);
		gI2CDebugAccessesEnabled = True;
	} else {
		gI2CDebugAccessesEnabled = False;
	}


	return errCode;

}				/*                                  */

/*                                                                           */
/* 
                                                                              
                                                              
                

                                           
                                                                 
                                                                           

                            
                                     
                                                                      
                   
                                                               
                                                                     
               
                                                                            

                                                                             */
tmErrorCode_t tmdlHdmiTxGetHdcpFailStatus
    (tmInstance_t instance, tmdlHdmiTxHdcpStatus_t *pHdcpStatus, UInt8 *pRawStatus) {
	tmErrorCode_t errCode = TM_OK;

#ifndef NO_HDCP
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)
	    RETIF(pHdcpStatus == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)
	    RETIF(pRawStatus == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    * pRawStatus = hdcpInfoListTx[instance].hdcpErrorState;

	switch (hdcpInfoListTx[instance].hdcpErrorState) {

	case 0:
		*pHdcpStatus = TMDL_HDMITX_HDCP_OK;
		break;

	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
		*pHdcpStatus = TMDL_HDMITX_HDCP_BKSV_RCV_FAIL;
		break;

	case 0x08:
		*pHdcpStatus = TMDL_HDMITX_HDCP_BKSV_CHECK_FAIL;
		break;

	case 0x0C:
		*pHdcpStatus = TMDL_HDMITX_HDCP_BCAPS_RCV_FAIL;
		break;

	case 0x0F:
	case 0x10:
	case 0x11:
		*pHdcpStatus = TMDL_HDMITX_HDCP_AKSV_SEND_FAIL;
		break;

	case 0x23:
	case 0x24:
	case 0x25:
		*pHdcpStatus = TMDL_HDMITX_HDCP_R0_RCV_FAIL;
		break;

	case 0x26:
		*pHdcpStatus = TMDL_HDMITX_HDCP_R0_CHECK_FAIL;
		break;

	case 0x27:
		*pHdcpStatus = TMDL_HDMITX_HDCP_BKSV_NOT_SECURE;
		break;

	case 0x2B:
	case 0x2C:
	case 0x2D:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RI_RCV_FAIL;
		break;

	case 0x77:
	case 0x78:
	case 0x79:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RPT_RI_RCV_FAIL;
		break;

	case 0x2E:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RI_CHECK_FAIL;
		break;

	case 0x7A:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RPT_RI_CHECK_FAIL;
		break;

	case 0x66:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RPT_BCAPS_RCV_FAIL;
		break;

	case 0x67:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RPT_BCAPS_READY_TIMEOUT;
		break;

	case 0x6A:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RPT_V_RCV_FAIL;
		break;

	case 0x6C:
	case 0x6D:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RPT_BSTATUS_RCV_FAIL;
		break;

	case 0x6F:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RPT_KSVLIST_RCV_FAIL;
		break;

	case 0x74:
		*pHdcpStatus = TMDL_HDMITX_HDCP_RPT_KSVLIST_NOT_SECURE;
		break;

	default:
		*pHdcpStatus = TMDL_HDMITX_HDCP_UNKNOWN_STATUS;
		break;
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)
#else
	(void)instance;		/*                         */
#endif				/*         */

	return errCode;
}


tmErrorCode_t tmdlHdmiTxGetEdidLatencyInfo
    (tmInstance_t instance, tmdlHdmiTxEdidLatency_t *pLatency) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                       */
	    RETIF(pLatency == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode =
		       tmbslHdmiTxEdidGetLatencyInfo(instance,
						     (tmbslHdmiTxEdidLatency_t *) pLatency)) !=
		      TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}				/*                              */

/*                                                                             
                                                                             
                                                          
                                 
                                      

                                                    
                                                                                

                            
                                     
                                                                      
                   
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
tmErrorCode_t tmdlHdmiTxGetEdidExtraVsdbData
    (tmInstance_t instance, tmdlHdmiTxEdidExtraVsdbData_t **pExtraVsdbData) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    /*                       */
	    RETIF(pExtraVsdbData == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                         */
	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      dlHdmiTxGetState(instance) != STATE_EDID_AVAILABLE,
		      TMDL_ERR_DLHDMITX_INVALID_STATE)

	    RETIF_SEM(dlHdmiTxItSemaphore[instance],
		      (errCode =
		       tmbslHdmiTxEdidGetExtraVsdbData(instance,
						       (tmbslHdmiTxEdidExtraVsdbData_t **)
						       pExtraVsdbData)) != TM_OK, errCode)

	    /*                       */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;
}				/*                                */


tmErrorCode_t tmdlHdmiTxGetHPDStatus(tmInstance_t instance, tmdlHdmiTxHotPlug_t *pHPDStatus) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    RETIF(pHPDStatus == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)


	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                    */
	    errCode =
	    tmbslHdmiTxHotPlugGetStatus(instance, (tmbslHdmiTxHotPlug_t *) pHPDStatus, True);

	if (errCode == TM_OK) {
		/*            */
	} else {
		*pHPDStatus = TMDL_HDMITX_HOTPLUG_INVALID;
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;

}				/*                        */



tmErrorCode_t tmdlHdmiTxGetRXSenseStatus
    (tmInstance_t instance, tmdlHdmiTxRxSense_t *pRXSenseStatus) {
	tmErrorCode_t errCode;

	/*                                      */
	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    RETIF(pRXSenseStatus == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)


	    /*                    */
	    RETIF((errCode =
		   tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    /*                                          */
	    errCode =
	    tmbslHdmiTxRxSenseGetStatus(instance, (tmbslHdmiTxRxSense_t *) pRXSenseStatus, True);

	if (errCode == TM_OK) {
		/*            */
	} else {
		*pRXSenseStatus = TMDL_HDMITX_RX_SENSE_INVALID;
	}

	/*                       */
	RETIF((errCode = tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCode)

	    return TM_OK;

}				/*                            */


/*                                                                             
                                           

                                                
                                                      

               

                                                                             */
tmErrorCode_t tmdlHdmiTxTmdsSetOutputsMute(tmInstance_t instance, Bool muteTmdsOut) {
	tmErrorCode_t errCode;
	tmErrorCode_t errCodeSem;
	tmbslHdmiTxTmdsOut_t tmdsOut;

	RETIF((instance < 0) || (instance >= MAX_UNITS), TMDL_ERR_DLHDMITX_BAD_INSTANCE)

	    if (muteTmdsOut)
		tmdsOut = HDMITX_TMDSOUT_FORCED0;	/*                  */
	else
		tmdsOut = HDMITX_TMDSOUT_NORMAL;	/*                */

	/*                    */
	RETIF((errCodeSem =
	       tmdlHdmiTxIWSemaphoreP(dlHdmiTxItSemaphore[instance])) != TM_OK, errCodeSem)

	    errCode = tmbslHdmiTxTmdsSetOutputs(instance, tmdsOut);

	/*                       */
	RETIF((errCodeSem =
	       tmdlHdmiTxIWSemaphoreV(dlHdmiTxItSemaphore[instance])) != TM_OK, errCodeSem)


	    return errCode;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
