/**
 * Copyright (C) 2009 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmbslTDA9989_edid.c
 *
 * \version       $Revision: 2 $
 *
*/

/*                                                                            */
/*                                                                            */
/*                                                                            */

#include "tmbslHdmiTx_types.h"
#include "tmbslTDA9989_Functions.h"
#include "tmbslTDA9989_local.h"
#include "tmbslTDA9989_State_l.h"
#include "tmbslTDA9989_Edid_l.h"
#ifndef TMFL_TDA19989
#define TMFL_TDA19989
#endif

#ifndef TMFL_NO_RTOS
#define TMFL_NO_RTOS
#endif

#ifndef TMFL_LINUX_OS_KERNEL_DRIVER
#define TMFL_LINUX_OS_KERNEL_DRIVER
#endif



/*                                                                            */
/*                                                                            */
/*                                                                            */


/*                                                                            */
/*                                                                            */
/*                                                                            */
#define EDID_NUMBER_MAX_DTD_BLK_1           6
/*                                 */
#define EDID_BLK0_BASE_DTD                  0x36

#define EDID_BLK1_OFFSET_BASE_DTD           2

/*                                     */
#define EDID_BLK0_EXT_CNT                   0x7E

/*                                         */
#define EDID_BLK_EXT_BASE                   0x04

/*                           */
#define EDID_CEA_EXTENSION                  0x02

/*                */
#define EDID_BLOCK_MAP                      0xF0

/*                                                 */
#define EDID_NB_MAX_DESCRIP_BLK_IN_BLK_0    4

#define EDID_MONITOR_NAME_DESC_DATA_TYPE    252

#define EDID_MONITOR_RANGE_DESC_DATA_TYPE   253

/*                                                                            */
/*                                                                          */
/*                                                                            */


/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */

static tmErrorCode_t requestEdidBlock(tmHdmiTxobject_t *pDis);

static tmErrorCode_t parseEdidBlock(tmHdmiTxobject_t *pDis, Int blockNumber);
static Bool storeDtdBlock(tmHdmiTxobject_t *pDis, UInt8 blockPtr);

static Bool storeMonitorDescriptor(tmHdmiTxobject_t *pDis, UInt8 blockPtr);



/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989HwGetCapabilities
    (tmUnitSelect_t txUnit, tmbslHdmiTxHwFeature_t deviceCapability, Bool *pFeatureSupported) {
	tmHdmiTxobject_t *pDis;
	tmErrorCode_t err = TM_OK;

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    RETIF_BADPARAM(pFeatureSupported == Null)

	    * pFeatureSupported = False;


	switch (deviceCapability) {
	case HDMITX_FEATURE_HW_HDCP:
		if ((pDis->uDeviceFeatures & E_MASKREG_P00_VERSION_not_h) == 0) {
			*pFeatureSupported = True;
		}
		break;
	case HDMITX_FEATURE_HW_SCALER:
		if ((pDis->uDeviceFeatures & E_MASKREG_P00_VERSION_not_s) == 0) {
			*pFeatureSupported = True;
		}
		break;
	case HDMITX_FEATURE_HW_AUDIO_OBA:
		*pFeatureSupported = True;
		break;
	case HDMITX_FEATURE_HW_AUDIO_DST:
		*pFeatureSupported = False;
		break;
	case HDMITX_FEATURE_HW_AUDIO_HBR:
		*pFeatureSupported = False;
		break;
	case HDMITX_FEATURE_HW_HDMI_1_1:
		*pFeatureSupported = True;
		break;
	case HDMITX_FEATURE_HW_HDMI_1_2A:
		*pFeatureSupported = True;
		break;
	case HDMITX_FEATURE_HW_HDMI_1_3A:
		*pFeatureSupported = False;
		break;

	case HDMITX_FEATURE_HW_DEEP_COLOR_30:
		*pFeatureSupported = False;
		break;

	case HDMITX_FEATURE_HW_DEEP_COLOR_36:
		*pFeatureSupported = False;
		break;

	case HDMITX_FEATURE_HW_DEEP_COLOR_48:
		*pFeatureSupported = False;
		break;

	case HDMITX_FEATURE_HW_UPSAMPLER:
		*pFeatureSupported = True;
		break;

	case HDMITX_FEATURE_HW_DOWNSAMPLER:
		*pFeatureSupported = True;
		break;

	case HDMITX_FEATURE_HW_COLOR_CONVERSION:
		*pFeatureSupported = True;
		break;

	default:
		*pFeatureSupported = False;
		break;
	}


	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989EdidGetAudioCapabilities
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxEdidSad_t *pEdidAFmts, UInt aFmtLength, UInt *pAFmtsAvail, UInt8 *pAudioFlags) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt i;			/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pEdidAFmts == Null)
	    RETIF_BADPARAM(aFmtLength < 1)
	    RETIF_BADPARAM(pAFmtsAvail == Null)
	    RETIF_BADPARAM(pAudioFlags == Null)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		/*                                                                                   */

		/*                                                            
                                                               
   */
		if (pDis->EdidSadCnt > 0) {
			for (i = 0; (i < (UInt) pDis->EdidSadCnt) && (i < aFmtLength); i++) {
				pEdidAFmts[i].ModeChans = pDis->EdidAFmts[i].ModeChans;
				pEdidAFmts[i].Freqs = pDis->EdidAFmts[i].Freqs;
				pEdidAFmts[i].Byte3 = pDis->EdidAFmts[i].Byte3;
			}
		} else {
			/*                                                       */
			pEdidAFmts[0].ModeChans = 0;
			pEdidAFmts[0].Freqs = 0;
			pEdidAFmts[0].Byte3 = 0;
		}

		/*                            */
		*pAudioFlags = ((pDis->EdidCeaFlags & 0x40) << 1);	/*             */
		if (pDis->EdidSinkAi == True) {
			*pAudioFlags += 0x40;	/*                    */
		}

		/*                                         */
		*pAFmtsAvail = pDis->EdidSadCnt;
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}
	return err;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989EdidGetBlockCount(tmUnitSelect_t txUnit, UInt8 *puEdidBlockCount) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(puEdidBlockCount == Null)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		/*                                                                                   */
		*puEdidBlockCount = pDis->EdidBlockCnt;
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}

	return err;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t tmbslTDA9989EdidGetStatus(tmUnitSelect_t txUnit, UInt8 *puEdidStatus) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(puEdidStatus == Null)

	    if (puEdidStatus) {
		*puEdidStatus = pDis->EdidStatus;
	}

	return err;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t tmbslTDA9989EdidRequestBlockData(tmUnitSelect_t txUnit, UInt8 *pRawEdid, Int numBlocks,	/*                                 */
					       Int lenRawEdid	/*                                 */
    ) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regval;		/*                              */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)
#ifdef TMFL_TDA9989_PIXEL_CLOCK_ON_DDC
	    if ((pDis->vinFmt == HDMITX_VFMT_16_1920x1080p_60Hz)
		|| (pDis->vinFmt == HDMITX_VFMT_31_1920x1080p_50Hz)) {

		err = setHwRegisterField(pDis,
					 E_REG_P02_PLL_SERIAL_3_RW,
					 E_MASKREG_P02_PLL_SERIAL_3_srl_ccir, 0x01);
		RETIF_REG_FAIL(err)

	}
#endif				/*                                 */

#ifdef TMFL_RGB_DDR_12BITS
	/*        */
	setHwRegisterField(pDis, E_REG_P12_TX4_RW, E_MASKREG_P12_TX4_pd_ram, 0);
#endif

	/*                  */
	err = setHwRegister(pDis, E_REG_P00_INT_FLAGS_2_RW, E_MASKREG_P00_INT_FLAGS_2_edid_blk_rd);

	/*                             
                                                                       
                                                                  
                                                            
  */
	RETIF_BADPARAM((pRawEdid != Null) && (lenRawEdid < EDID_BLOCK_SIZE))
	    /*                              */
	    RETIF((pRawEdid != Null) && ((numBlocks < 1) || (numBlocks > 255)),
		  TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)
	    /*                                      */
	    RETIF((pRawEdid != Null) && (lenRawEdid < (numBlocks * EDID_BLOCK_SIZE)),
		  TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)

	    /*                                                                   
                                                                           
                
                                                                      
                                         
      */
	    RETIF(pDis->hotPlugStatus != HDMITX_HOTPLUG_ACTIVE, TMBSL_ERR_HDMI_NULL_CONNECTION)

	    if (pDis->EdidReadStarted == False) {

		/*                                                       */
		pDis->EdidStatus = HDMITX_EDID_NOT_READ;

		pDis->EdidReadStarted = True;

		/*                                                                    */
		pDis->EdidSinkType = HDMITX_SINK_DVI;
		pDis->EdidSinkAi = False;
		pDis->EdidCeaFlags = 0;
		pDis->EdidCeaXVYCCFlags = 0;
		pDis->EdidSvdCnt = 0;
		pDis->EdidSadCnt = 0;
		pDis->EdidSourceAddress = 0;	/*         */
		pDis->NbDTDStored = 0;
		pDis->EdidFirstMonitorDescriptor.bDescRecord = False;
		pDis->EdidSecondMonitorDescriptor.bDescRecord = False;
		pDis->EdidOtherMonitorDescriptor.bDescRecord = False;

		pDis->EdidLatency.latency_available = False;
		pDis->EdidLatency.Ilatency_available = False;

		pDis->EdidExtraVsdbData.hdmiVideoPresent = False;


		pDis->EdidToApp.pRawEdid = pRawEdid;
		pDis->EdidToApp.numBlocks = numBlocks;

		/*                                                             */
		regval = E_MASKREG_P00_INT_FLAGS_0_t0;
		err = setHwRegister(pDis, E_REG_P00_INT_FLAGS_0_RW, regval);
		RETIF(err != TM_OK, err);


		/*                                                                    */
		pDis->EdidBlockRequested = 0;
		err = requestEdidBlock(pDis);
	} else {
		/*                                      */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}

	return err;
}


/*                                                                            */
/*                                                                            */
/*                                                                            */

tmErrorCode_t EdidBlockAvailable(tmUnitSelect_t txUnit, Bool *pSendEDIDCallback)
{

	tmErrorCode_t err;	/*            */
	UInt8 chksum;		/*                */
	UInt8 LoopIndex;	/*            */
	UInt8 extBlockCnt;
	tmHdmiTxobject_t *pDis;	/*                                      */

	err = TM_OK;
	/*                              */
	RETIF_BADPARAM(pSendEDIDCallback == Null)

	    /*                                              */
	    err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    if (pDis->EdidReadStarted == True) {

		err = tmbslTDA9989HwGetRegisters(txUnit, kPageIndexToPage[E_PAGE_09],
						 SPA2ADDR(E_REG_P09_EDID_DATA_0_R), pDis->EdidBlock,
						 EDID_BLOCK_SIZE);
		RETIF(err != TM_OK, err)

		    if (pSendEDIDCallback) {
			*pSendEDIDCallback = False;
		}

		if (pDis->EdidStatus == HDMITX_EDID_NOT_READ) {
			err = getHwRegisters(pDis, E_REG_P09_EDID_DATA_0_R, pDis->EdidBlock,
					     EDID_BLOCK_SIZE);
			RETIF_REG_FAIL(err)

			    /*                                                             
                       
        */
			    chksum = 0;
			for (LoopIndex = 0; LoopIndex < EDID_BLOCK_SIZE; LoopIndex++) {
				chksum = chksum + pDis->EdidBlock[LoopIndex];
			}

			/*                                                    
    */
			if (chksum != 0) {
				if (pDis->EdidBlockRequested == 0) {
					/*                                        */
					pDis->EdidStatus = HDMITX_EDID_ERROR_CHK_BLOCK_0;
				} else {
					/*                                            */
					pDis->EdidStatus = HDMITX_EDID_ERROR_CHK;
				}
			}
		}

		if (pDis->EdidStatus == HDMITX_EDID_ERROR_CHK_BLOCK_0) {
			/*                                                */
			/*                                       */
			/*                                                                
                                 */

			if (pSendEDIDCallback) {
				*pSendEDIDCallback = True;
			}

			setState(pDis, EV_GETBLOCKDATA);
			if (pDis->rxSenseStatus == HDMITX_RX_SENSE_ACTIVE) {
				setState(pDis, EV_SINKON);
			}
			pDis->EdidReadStarted = False;
			return err;
		}

		/*                  */
		if (pDis->EdidBlockRequested == 0) {
			/*                                                             
                                                
    */

			/*                                                                  */
			extBlockCnt = pDis->EdidBlock[EDID_BLK0_EXT_CNT];

			pDis->EdidBlockCnt = extBlockCnt + 1;	/*                              */

		}

		/*                                                        */
		if (pDis->EdidToApp.pRawEdid != Null) {
			/*                                                 */
			if (pDis->EdidBlockRequested < pDis->EdidToApp.numBlocks) {
				lmemcpy(pDis->EdidToApp.pRawEdid +
					(pDis->EdidBlockRequested * EDID_BLOCK_SIZE),
					pDis->EdidBlock, EDID_BLOCK_SIZE);
			}
		}
		parseEdidBlock(pDis, pDis->EdidBlockRequested);

		/*                                               */
		if ((pDis->EdidBlockRequested + 1) < pDis->EdidBlockCnt) {
			pDis->EdidBlockRequested = pDis->EdidBlockRequested + 1;
			/*                           */
			err = requestEdidBlock(pDis);
		} else {
			if (pDis->EdidStatus == HDMITX_EDID_NOT_READ) {
				pDis->EdidStatus = HDMITX_EDID_READ;

#ifdef TMFL_RGB_DDR_12BITS
				/*         */
				setHwRegisterField(pDis, E_REG_P12_TX4_RW, E_MASKREG_P12_TX4_pd_ram,
						   1);
#endif
			}

			if (pSendEDIDCallback) {
				*pSendEDIDCallback = True;
			}

			setState(pDis, EV_GETBLOCKDATA);

			if (pDis->rxSenseStatus == HDMITX_RX_SENSE_ACTIVE) {
				setState(pDis, EV_SINKON);
			}
			pDis->EdidReadStarted = False;
		}
	} else {
		/*                                     */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}

	return err;

}

/*                                                                            */
/*                                                                            */
/*                                                                            */

tmErrorCode_t ClearEdidRequest(tmUnitSelect_t txUnit)
{

	tmErrorCode_t err;	/*            */
	tmHdmiTxobject_t *pDis;	/*                                      */

	err = TM_OK;

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)


	    /*                                                       */
	    pDis->EdidStatus = HDMITX_EDID_NOT_READ;

	pDis->EdidReadStarted = False;

	/*                                                                    */
	pDis->EdidSinkType = HDMITX_SINK_DVI;
	pDis->EdidSinkAi = False;
	pDis->EdidCeaFlags = 0;
	pDis->EdidCeaXVYCCFlags = 0;
	pDis->EdidSvdCnt = 0;
	pDis->EdidSadCnt = 0;
	pDis->EdidSourceAddress = 0;	/*         */
	pDis->NbDTDStored = 0;
	pDis->EdidFirstMonitorDescriptor.bDescRecord = False;
	pDis->EdidSecondMonitorDescriptor.bDescRecord = False;
	pDis->EdidOtherMonitorDescriptor.bDescRecord = False;

	pDis->EdidLatency.latency_available = False;
	pDis->EdidLatency.Ilatency_available = False;

	pDis->EdidExtraVsdbData.hdmiVideoPresent = False;

	/*                                                                    */
	pDis->EdidBlockRequested = 0;


	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989EdidGetSinkType(tmUnitSelect_t txUnit, tmbslHdmiTxSinkType_t *pSinkType) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pSinkType == Null)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		/*                                                                                   */

		*pSinkType = pDis->EdidSinkType;
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}
	return err;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t tmbslTDA9989EdidGetSourceAddress(tmUnitSelect_t txUnit, UInt16 *pSourceAddress) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pSourceAddress == Null)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		/*                                                                                   */

		*pSourceAddress = pDis->EdidSourceAddress;
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}
	return err;

}


/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989EdidGetDetailedTimingDescriptors
    (tmUnitSelect_t txUnit, tmbslHdmiTxEdidDtd_t *pEdidDTD, UInt8 nb_size, UInt8 *pDTDAvail) {

	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pEdidDTD == Null)
	    RETIF_BADPARAM(pDTDAvail == Null)
	    RETIF_BADPARAM(nb_size == 0)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		/*                                                                                   */
		if (nb_size > pDis->NbDTDStored) {
			*pDTDAvail = pDis->NbDTDStored;
		} else {
			*pDTDAvail = nb_size;
		}

		lmemcpy(pEdidDTD, pDis->EdidDTD, sizeof(tmbslHdmiTxEdidDtd_t) * (*pDTDAvail));
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}

	return TM_OK;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989EdidGetMonitorDescriptors
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxEdidFirstMD_t *pEdidFirstMD,
     tmbslHdmiTxEdidSecondMD_t *pEdidSecondMD,
     tmbslHdmiTxEdidOtherMD_t *pEdidOtherMD, UInt8 sizeOtherMD, UInt8 *pOtherMDAvail) {

	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pEdidFirstMD == Null)
	    RETIF_BADPARAM(pEdidSecondMD == Null)
	    RETIF_BADPARAM(pEdidOtherMD == Null)

	    DUMMY_ACCESS(pOtherMDAvail);
	DUMMY_ACCESS(sizeOtherMD);

	if ((pDis->EdidStatus == HDMITX_EDID_READ) || (pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		*pOtherMDAvail = 1;
		lmemcpy(pEdidFirstMD, &(pDis->EdidFirstMonitorDescriptor),
			sizeof(tmbslHdmiTxEdidFirstMD_t));
		lmemcpy(pEdidSecondMD, &(pDis->EdidSecondMonitorDescriptor),
			sizeof(tmbslHdmiTxEdidSecondMD_t));
		lmemcpy(pEdidOtherMD, &(pDis->EdidOtherMonitorDescriptor),
			sizeof(tmbslHdmiTxEdidOtherMD_t));
	} else {
		/*                                              */
		*pOtherMDAvail = 0;
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}

	return TM_OK;

}

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989EdidGetBasicDisplayParam
    (tmUnitSelect_t txUnit, tmbslHdmiTxEdidBDParam_t *pEdidBDParam) {

	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pEdidBDParam == Null)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		lmemcpy(pEdidBDParam, &(pDis->EDIDBasicDisplayParam),
			sizeof(tmbslHdmiTxEdidBDParam_t));
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}

	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989EdidGetVideoCapabilities
    (tmUnitSelect_t txUnit,
     UInt8 *pEdidVFmts, UInt vFmtLength, UInt *pVFmtsAvail, UInt8 *pVidFlags) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt i;			/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pEdidVFmts == Null)
	    RETIF_BADPARAM(vFmtLength < 1)
	    RETIF_BADPARAM(pVFmtsAvail == Null)
	    RETIF_BADPARAM(pVidFlags == Null)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		/*                                                                                   */

		/*                                                            
                                                               
   */
		if (pDis->EdidSvdCnt > 0) {
			for (i = 0; (i < (UInt) pDis->EdidSvdCnt) && (i < vFmtLength); i++) {
				pEdidVFmts[i] = pDis->EdidVFmts[i];
			}
		} else {
			/*                                                       */
			pEdidVFmts[0] = HDMITX_VFMT_NULL;
		}

		/*                            */
		*pVidFlags = ((pDis->EdidCeaFlags & 0x80) |	/*           */
			      ((pDis->EdidCeaFlags & 0x30) << 1));	/*                */


		/*                                  */
		*pVidFlags = *pVidFlags | (pDis->EdidCeaXVYCCFlags & 0x03);

		/*                                         */
		*pVFmtsAvail = pDis->EdidSvdCnt;
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}

	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t
    tmbslTDA9989EdidGetVideoPreferred(tmUnitSelect_t txUnit, tmbslHdmiTxEdidDtd_t *pEdidDTD) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pEdidDTD == Null)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		/*                                                                                   */

		/*                                                                
                                              
   */
		lmemcpy(pEdidDTD, &pDis->EdidDTD, sizeof(tmbslHdmiTxEdidDtd_t));
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}
	return err;

}


/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */
static tmErrorCode_t requestEdidBlock(tmHdmiTxobject_t *pDis	/*                                 */
    ) {
	tmErrorCode_t err;	/*            */
	UInt8 segptr;		/*                   */
	UInt8 offset;		/*                   */

	/*                                             */
	RETIF_BADPARAM(pDis->EdidBlockRequested >= 255)

	    err = setHwRegister(pDis, E_REG_P09_DDC_ADDR_RW, DDC_EDID_ADDRESS);
	RETIF_REG_FAIL(err)

	    /*                                                                */
	    offset = (((UInt8) pDis->EdidBlockRequested & 1) == 1) ? 128 : 0;

	err = setHwRegister(pDis, E_REG_P09_DDC_OFFS_RW, offset);
	RETIF_REG_FAIL(err)

	    err = setHwRegister(pDis, E_REG_P09_DDC_SEGM_ADDR_RW, DDC_SGMT_PTR_ADDRESS);
	RETIF_REG_FAIL(err)

	    /*                                                                    */
	    segptr = (UInt8) pDis->EdidBlockRequested / 2;

	err = setHwRegister(pDis, E_REG_P09_DDC_SEGM_RW, segptr);
	RETIF_REG_FAIL(err)

	    /*                     */
	    err = setHwRegister(pDis, E_REG_P09_EDID_CTRL_RW, 0x1);
	RETIF_REG_FAIL(err)

	    /*                                                             */
	    err = setHwRegister(pDis, E_REG_P09_EDID_CTRL_RW, 0x0);
	RETIF_REG_FAIL(err)

	    return err;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
static tmErrorCode_t parseEdidBlock(tmHdmiTxobject_t *pDis,	/*                                        */
				    Int blockNumber	/*              */
    ) {
	UInt8 i;		/*            */
	UInt8 blockPtr, endPtr;	/*                  */
	UInt8 blockType, blockLength;
	Bool dtdFound;
	UInt8 NbBlkRead, offset3D = 0;

	/*                                             */
	RETIF_BADPARAM(blockNumber >= 255)

	    NbBlkRead = 0;
	dtdFound = True;
	blockPtr = 0;

	if (blockNumber == 0) {
		pDis->EDIDBasicDisplayParam.uVideoInputDef = pDis->EdidBlock[0x14];
		pDis->EDIDBasicDisplayParam.uMaxHorizontalSize = pDis->EdidBlock[0x15];
		pDis->EDIDBasicDisplayParam.uMaxVerticalSize = pDis->EdidBlock[0x16];
		pDis->EDIDBasicDisplayParam.uGamma = pDis->EdidBlock[0x17];
		pDis->EDIDBasicDisplayParam.uFeatureSupport = pDis->EdidBlock[0x18];

		/*                                                        */
		for (i = 0; (i < 2) && (dtdFound); i++) {	/*                                         */
			blockPtr = (UInt8) (EDID_BLK0_BASE_DTD + (i * EDID_DTD_BLK_SIZE));
			if ((blockPtr + EDID_DTD_BLK_SIZE - 1) < EDID_BLOCK_SIZE) {
				dtdFound = storeDtdBlock(pDis, blockPtr);
				if (dtdFound) {
					NbBlkRead++;
				}
			}
		}

		dtdFound = True;

		/*                          */
		for (i = NbBlkRead; (i < EDID_NB_MAX_DESCRIP_BLK_IN_BLK_0) && (dtdFound); i++) {
			blockPtr = (UInt8) (EDID_BLK0_BASE_DTD + (i * EDID_DTD_BLK_SIZE));
			if ((blockPtr + EDID_DTD_BLK_SIZE - 1) < EDID_BLOCK_SIZE) {
				dtdFound = storeMonitorDescriptor(pDis, blockPtr);
			}
		}
	} else if (blockNumber >= 1) {
		switch (pDis->EdidBlock[0]) {
			/*               */
		case EDID_CEA_EXTENSION:
			/*                                                     */
			pDis->EdidCeaFlags = pDis->EdidBlock[3];

			blockPtr = EDID_BLK_EXT_BASE;	/*                               */
			endPtr = pDis->EdidBlock[2];	/*                               */
			if (endPtr >= (EDID_BLK_EXT_BASE + 2) && (endPtr <= EDID_BLOCK_SIZE))
				/*                                                               
                                                  
     */
			{
				while (blockPtr < endPtr) {
					blockType =
					    (UInt8) ((pDis->EdidBlock[blockPtr] & 0xE0) >> 5);
					blockLength = (pDis->EdidBlock[blockPtr] & 0x1F);

					switch ((Int) blockType) {
					case E_CEA_VIDEO_BLOCK:	/*                            */
						for (i = 1; i <= blockLength; i++) {
							if ((blockPtr + i) < (EDID_BLOCK_SIZE)) {
								/*                               */
								if ((pDis->
								     EdidBlock[blockPtr + i] != 0)
								    && (pDis->EdidSvdCnt <
									HDMI_TX_SVD_MAX_CNT)) {
									pDis->EdidVFmts[pDis->
											EdidSvdCnt]
									    =
									    pDis->
									    EdidBlock[blockPtr + i];
									pDis->EdidSvdCnt++;
								}
							} else {
								/*            */
							}
						}
						break;
					case E_CEA_AUDIO_BLOCK:	/*                             */
						for (i = 1; (i + 2) <= blockLength; i += 3) {	/*                                    */
							/*                               */
							if ((blockPtr) <
							    (EDID_BLOCK_SIZE - (i + 2))) {
								if (((pDis->
								      EdidBlock[blockPtr +
										i] & 0x78) != 0)
								    && (pDis->EdidSadCnt <
									HDMI_TX_SAD_MAX_CNT)) {
									pDis->EdidAFmts[pDis->
											EdidSadCnt].
									    ModeChans =
									    pDis->
									    EdidBlock[blockPtr + i];
									pDis->EdidAFmts[pDis->
											EdidSadCnt].
									    Freqs =
									    pDis->
									    EdidBlock[blockPtr + i +
										      1];
									pDis->EdidAFmts[pDis->
											EdidSadCnt].
									    Byte3 =
									    pDis->
									    EdidBlock[blockPtr + i +
										      2];
									pDis->EdidSadCnt++;
								}
							} else {
								/*            */
							}
						}
						break;
					case E_CEA_VSDB:	/*                */
						/*                                                         */
						if (blockLength >= 5) {
							if ((blockPtr) < (EDID_BLOCK_SIZE - 5)) {
								if ((pDis->
								     EdidBlock[blockPtr + 1] ==
								     0x03)
								    && (pDis->
									EdidBlock[blockPtr + 2] ==
									0x0C)
								    && (pDis->
									EdidBlock[blockPtr + 3] ==
									0x00)) {
									pDis->EdidSinkType =
									    HDMITX_SINK_HDMI;
									if ((blockPtr) <
									    (EDID_BLOCK_SIZE - 5)) {
										pDis->
										    EdidSourceAddress
										    =
										    ((UInt16) pDis->
										     EdidBlock
										     [blockPtr +
										      4] << 8) +
										    pDis->
										    EdidBlock
										    [blockPtr + 5];
									} else {
										/*            */
									}
								} else {
									pDis->EdidSinkType =
									    HDMITX_SINK_DVI;
								}
							} else {
								/*            */
							}
						}

						if (blockLength >= 6) {	/*                             *//*             */
							if ((blockPtr) < (EDID_BLOCK_SIZE - 6)) {
								if ((pDis->
								     EdidBlock[blockPtr +
									       6] & 0x80) == 0x80) {
									pDis->EdidSinkAi = True;
								}
							} else {
								/*            */
							}
						}

						/*                     */
						if (blockLength >= 7)
							pDis->EdidExtraVsdbData.maxTmdsClock =
							    pDis->EdidBlock[blockPtr + 7];
						else
							pDis->EdidExtraVsdbData.maxTmdsClock = 0;


						/*                                                     */
						if (blockLength >= 8) {
							if ((blockPtr) < (EDID_BLOCK_SIZE - 10)) {
								/*             */
								pDis->EdidExtraVsdbData.cnc0 = pDis->EdidBlock[blockPtr + 8] & 0x01;	/*                 */
								pDis->EdidExtraVsdbData.cnc1 =
								    (pDis->
								     EdidBlock[blockPtr +
									       8] & 0x02) >> 1;
								pDis->EdidExtraVsdbData.cnc2 =
								    (pDis->
								     EdidBlock[blockPtr +
									       8] & 0x04) >> 2;
								pDis->EdidExtraVsdbData.cnc3 =
								    (pDis->
								     EdidBlock[blockPtr +
									       8] & 0x08) >> 3;

								if ((pDis->
								     EdidBlock[blockPtr +
									       8] & 0xC0) == 0xC0) {
									/*                                                                     */

									if ((blockPtr) <
									    (EDID_BLOCK_SIZE -
									     12)) {
										pDis->EdidLatency.
										    Edidvideo_latency
										    =
										    pDis->
										    EdidBlock
										    [blockPtr + 9];
										pDis->EdidLatency.
										    Edidaudio_latency
										    =
										    pDis->
										    EdidBlock
										    [blockPtr + 10];
										pDis->EdidLatency.
										    EdidIvideo_latency
										    =
										    pDis->
										    EdidBlock
										    [blockPtr + 11];
										pDis->EdidLatency.
										    EdidIaudio_latency
										    =
										    pDis->
										    EdidBlock
										    [blockPtr + 12];

										pDis->EdidLatency.
										    latency_available
										    = True;
										pDis->EdidLatency.
										    Ilatency_available
										    = True;

										offset3D = 13;	/*                                  */

									} else {
										/*            */
									}
								} else
								    if ((pDis->
									 EdidBlock[blockPtr +
										   8] & 0x80) ==
									0x80) {
									/*                                   */

									pDis->EdidLatency.
									    Edidvideo_latency =
									    pDis->
									    EdidBlock[blockPtr + 9];
									pDis->EdidLatency.
									    Edidaudio_latency =
									    pDis->
									    EdidBlock[blockPtr +
										      10];

									pDis->EdidLatency.
									    latency_available =
									    True;

									offset3D = 11;
								} else {
									pDis->EdidLatency.
									    latency_available =
									    False;
									pDis->EdidLatency.
									    Ilatency_available =
									    False;
									offset3D = 9;
								}

								/*                         */
								pDis->EdidExtraVsdbData.
								    hdmiVideoPresent =
								    (pDis->
								     EdidBlock[blockPtr +
									       8] & 0x20) >> 5;

							} else {
								/*            */
							}
						} else {
							pDis->EdidLatency.latency_available = False;
							pDis->EdidLatency.Ilatency_available =
							    False;
							pDis->EdidExtraVsdbData.hdmiVideoPresent =
							    False;
							pDis->EdidExtraVsdbData.cnc0 = False;
							pDis->EdidExtraVsdbData.cnc1 = False;
							pDis->EdidExtraVsdbData.cnc2 = False;
							pDis->EdidExtraVsdbData.cnc3 = False;
						}


						/*                                                */
						if (pDis->EdidExtraVsdbData.hdmiVideoPresent) {

							/*                 */
							pDis->EdidExtraVsdbData.h3DPresent =
							    (pDis->
							     EdidBlock[blockPtr +
								       offset3D] & 0x80) >> 7;
							/*                       */
							pDis->EdidExtraVsdbData.h3DMultiPresent =
							    (pDis->
							     EdidBlock[blockPtr +
								       offset3D] & 0x60) >> 5;
							/*                 */
							pDis->EdidExtraVsdbData.imageSize =
							    (pDis->
							     EdidBlock[blockPtr +
								       offset3D] & 0x18) >> 3;

							/*                                  */
							offset3D += 1;
							pDis->EdidExtraVsdbData.hdmi3DLen =
							    pDis->EdidBlock[blockPtr +
									    offset3D] & 0x1F;
							pDis->EdidExtraVsdbData.hdmiVicLen =
							    (pDis->
							     EdidBlock[blockPtr +
								       offset3D] & 0xE0) >> 5;

							if ((pDis->EdidExtraVsdbData.hdmi3DLen +
							     pDis->EdidExtraVsdbData.hdmiVicLen) >
							    0) {
								/*                            */
								lmemcpy(pDis->EdidExtraVsdbData.
									ext3DData,
									&(pDis->
									  EdidBlock[blockPtr +
										    offset3D + 1]),
									blockLength - offset3D);
							}
						} else {
							pDis->EdidExtraVsdbData.h3DPresent = False;
							pDis->EdidExtraVsdbData.h3DMultiPresent = 0;
							pDis->EdidExtraVsdbData.imageSize = 0;
							pDis->EdidExtraVsdbData.hdmi3DLen = 0;
							pDis->EdidExtraVsdbData.hdmiVicLen = 0;
						}


						break;


					case E_CEA_EXTENDED:	/*                  */

						/*                                       */

						if ((blockPtr) < (EDID_BLOCK_SIZE - 2)) {
							switch (pDis->EdidBlock[blockPtr + 1]) {
							case EXT_CEA_COLORIMETRY_DB:

								/*                                       */
								pDis->EdidCeaXVYCCFlags =
								    pDis->EdidBlock[blockPtr + 2];

								break;
							}
						} else {
							/*            */
						}
						break;	/*                */



					default:
						break;
					}
					blockPtr += (blockLength + 1);	/*                     */
				}
			}
			dtdFound = True;

			for (i = 0; (i < EDID_NUMBER_MAX_DTD_BLK_1) && (dtdFound); i++) {	/*                                       */
				blockPtr =
				    ((UInt8) pDis->EdidBlock[EDID_BLK1_OFFSET_BASE_DTD]) +
				    ((UInt8) (i * EDID_DTD_BLK_SIZE));
				if ((blockPtr + EDID_DTD_BLK_SIZE - 1) < EDID_BLOCK_SIZE) {
					dtdFound = storeDtdBlock(pDis, blockPtr);
				}
			}

			break;


		case EDID_BLOCK_MAP:
			/*           */

			if (pDis->EdidBlockCnt > 1) {
				if ((pDis->EdidBlockCnt - 1) < EDID_BLOCK_SIZE) {
					if (pDis->EdidBlock[pDis->EdidBlockCnt - 1] ==
					    EDID_CEA_EXTENSION) {
						/*                                                                                         */
						/*                                                                                                 */
						/*                             */
						pDis->EdidBlockCnt = pDis->EdidBlockCnt + 1;
					}
				} else {
					/*            */
				}
			}


			break;


		}

	}

	return TM_OK;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
static Bool storeDtdBlock(tmHdmiTxobject_t *pDis,	/*                                        */
			  UInt8 blockPtr) {

	Bool dtdFound = False;

	if (blockPtr >= (EDID_BLOCK_SIZE - 17)) {
		/*            */
		return dtdFound;
	}

	/*                                                     */
	if (((pDis->EdidBlock[blockPtr + 0] != 0) ||
	     (pDis->EdidBlock[blockPtr + 1] != 0) ||
	     (pDis->EdidBlock[blockPtr + 2] != 0) || (pDis->EdidBlock[blockPtr + 4] != 0))
	    && (pDis->NbDTDStored < NUMBER_DTD_STORED)) {	/*                                                     */
		pDis->EdidDTD[pDis->NbDTDStored].uPixelClock =
		    ((UInt16) pDis->EdidBlock[blockPtr + 1] << 8) |
		    (UInt16) pDis->EdidBlock[blockPtr + 0];

		pDis->EdidDTD[pDis->NbDTDStored].uHActivePixels =
		    (((UInt16) pDis->EdidBlock[blockPtr + 4] & 0x00F0) << 4) |
		    (UInt16) pDis->EdidBlock[blockPtr + 2];

		pDis->EdidDTD[pDis->NbDTDStored].uHBlankPixels =
		    (((UInt16) pDis->EdidBlock[blockPtr + 4] & 0x000F) << 8) |
		    (UInt16) pDis->EdidBlock[blockPtr + 3];

		pDis->EdidDTD[pDis->NbDTDStored].uVActiveLines =
		    (((UInt16) pDis->EdidBlock[blockPtr + 7] & 0x00F0) << 4) |
		    (UInt16) pDis->EdidBlock[blockPtr + 5];

		pDis->EdidDTD[pDis->NbDTDStored].uVBlankLines =
		    (((UInt16) pDis->EdidBlock[blockPtr + 7] & 0x000F) << 8) |
		    (UInt16) pDis->EdidBlock[blockPtr + 6];

		pDis->EdidDTD[pDis->NbDTDStored].uHSyncOffset =
		    (((UInt16) pDis->EdidBlock[blockPtr + 11] & 0x00C0) << 2) |
		    (UInt16) pDis->EdidBlock[blockPtr + 8];

		pDis->EdidDTD[pDis->NbDTDStored].uHSyncWidth =
		    (((UInt16) pDis->EdidBlock[blockPtr + 11] & 0x0030) << 4) |
		    (UInt16) pDis->EdidBlock[blockPtr + 9];

		pDis->EdidDTD[pDis->NbDTDStored].uVSyncOffset =
		    (((UInt16) pDis->EdidBlock[blockPtr + 11] & 0x000C) << 2) |
		    (((UInt16) pDis->EdidBlock[blockPtr + 10] & 0x00F0) >> 4);

		pDis->EdidDTD[pDis->NbDTDStored].uVSyncWidth =
		    (((UInt16) pDis->EdidBlock[blockPtr + 11] & 0x0003) << 4) |
		    ((UInt16) pDis->EdidBlock[blockPtr + 10] & 0x000F);

		pDis->EdidDTD[pDis->NbDTDStored].uHImageSize =
		    (((UInt16) pDis->EdidBlock[blockPtr + 14] & 0x00F0) << 4) |
		    (UInt16) pDis->EdidBlock[blockPtr + 12];

		pDis->EdidDTD[pDis->NbDTDStored].uVImageSize =
		    (((UInt16) pDis->EdidBlock[blockPtr + 14] & 0x000F) << 8) |
		    (UInt16) pDis->EdidBlock[blockPtr + 13];

		pDis->EdidDTD[pDis->NbDTDStored].uHBorderPixels =
		    (UInt16) pDis->EdidBlock[blockPtr + 15];

		pDis->EdidDTD[pDis->NbDTDStored].uVBorderPixels =
		    (UInt16) pDis->EdidBlock[blockPtr + 16];

		pDis->EdidDTD[pDis->NbDTDStored].Flags = pDis->EdidBlock[blockPtr + 17];

		pDis->NbDTDStored++;

		dtdFound = True;	/*                                 */
	}

	return (dtdFound);
}


/*                                                                            */
/*                                                                            */
/*                                                                            */
static Bool storeMonitorDescriptor(tmHdmiTxobject_t *pDis,	/*                                        */
				   UInt8 blockPtr) {

	Bool dtdFound = False;

	if (blockPtr >= (EDID_BLOCK_SIZE - 5)) {
		/*            */
		return dtdFound;
	}

	/*                                                     */
	if ((pDis->EdidBlock[blockPtr + 0] == 0) &&
	    (pDis->EdidBlock[blockPtr + 1] == 0) && (pDis->EdidBlock[blockPtr + 2] == 0)
	    ) {
		if (pDis->EdidBlock[blockPtr + 3] == EDID_MONITOR_NAME_DESC_DATA_TYPE) {
			if (pDis->EdidFirstMonitorDescriptor.bDescRecord == False) {
				pDis->EdidFirstMonitorDescriptor.bDescRecord = True;
				lmemcpy(&(pDis->EdidFirstMonitorDescriptor.uMonitorName),
					&(pDis->EdidBlock[blockPtr + 5]),
					EDID_MONITOR_DESCRIPTOR_SIZE);
				dtdFound = True;
			} else if ((pDis->EdidOtherMonitorDescriptor.bDescRecord == False)) {
				pDis->EdidOtherMonitorDescriptor.bDescRecord = True;
				lmemcpy(&(pDis->EdidOtherMonitorDescriptor.uOtherDescriptor),
					&(pDis->EdidBlock[blockPtr + 5]),
					EDID_MONITOR_DESCRIPTOR_SIZE);
				dtdFound = True;
			}
		} else if (pDis->EdidBlock[blockPtr + 3] == EDID_MONITOR_RANGE_DESC_DATA_TYPE) {
			if (pDis->EdidSecondMonitorDescriptor.bDescRecord == False) {
				if (blockPtr < (EDID_BLOCK_SIZE - 9)) {
					pDis->EdidSecondMonitorDescriptor.bDescRecord = True;
					pDis->EdidSecondMonitorDescriptor.uMinVerticalRate =
					    pDis->EdidBlock[blockPtr + 5];
					pDis->EdidSecondMonitorDescriptor.uMaxVerticalRate =
					    pDis->EdidBlock[blockPtr + 6];
					pDis->EdidSecondMonitorDescriptor.uMinHorizontalRate =
					    pDis->EdidBlock[blockPtr + 7];
					pDis->EdidSecondMonitorDescriptor.uMaxHorizontalRate =
					    pDis->EdidBlock[blockPtr + 8];
					pDis->EdidSecondMonitorDescriptor.uMaxSupportedPixelClk =
					    pDis->EdidBlock[blockPtr + 9];
					dtdFound = True;
				} else {
					/*            */
				}
			}
		}
	}

	return (dtdFound);

}


/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989EdidGetLatencyInfo(tmUnitSelect_t txUnit, tmbslHdmiTxEdidLatency_t *pEdidLatency) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pEdidLatency == Null)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		/*                                                                                   */

		*pEdidLatency = pDis->EdidLatency;
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}
	return err;

}

/*                                                                            */
/*                                                                              */
/*                                                                            */
tmErrorCode_t tmbslTDA9989EdidGetExtraVsdbData
    (tmUnitSelect_t txUnit, tmbslHdmiTxEdidExtraVsdbData_t **pExtraVsdbData) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pExtraVsdbData == Null)

	    if ((pDis->EdidStatus == HDMITX_EDID_READ) ||
		(pDis->EdidStatus == HDMITX_EDID_ERROR_CHK)) {
		/*                                                                                   */
		*pExtraVsdbData = &(pDis->EdidExtraVsdbData);
	} else {
		/*                                              */
		err = TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE;
	}
	return err;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
