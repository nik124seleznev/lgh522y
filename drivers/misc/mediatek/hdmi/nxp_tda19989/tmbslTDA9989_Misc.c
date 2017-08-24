/**
 * Copyright (C) 2009 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmbslTDA9989_misc.c
 *
 * \version        %version: 3 %
 *
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


#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
#include <linux/kernel.h>
#endif

#include "tmbslHdmiTx_types.h"
#include "tmbslTDA9989_Functions.h"
#include "tmbslTDA9989_local.h"
#include "tmbslTDA9989_HDCP_l.h"
#include "tmbslTDA9989_State_l.h"
#include "tmbslTDA9989_InOut_l.h"
#include "tmbslTDA9989_Edid_l.h"
#include "tmbslTDA9989_Misc_l.h"

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                               */
static tmHdmiTxobject_t kHdmiTxInstanceDefault = {
	ST_UNINITIALIZED,	/*       */
	0,			/*                */
	tmUnit0,		/*        */
	0,			/*            */
	(ptmbslHdmiTxSysFunc_t) 0,	/*              */
	(ptmbslHdmiTxSysFunc_t) 0,	/*             */
	(ptmbslHdmiTxSysFuncEdid_t) 0,	/*                 */
	(ptmbslHdmiTxSysFuncTimer_t) 0,	/*              */
	{			/*                    */
	 (ptmbslHdmiTxCallback_t) 0},
	0,			/*                  */
	{			/*                      */
	 E_DEV_VERSION_N2,
	 E_DEV_VERSION_TDA19989,
	 E_DEV_VERSION_TDA19989_N2,
	 E_DEV_VERSION_TDA19988,
	 E_DEV_VERSION_LIST_END},
	E_DEV_VERSION_LIST_END,	/*                */
	E_DEV_VERSION_LIST_END,	/*                 */
	(tmbslHdmiTxPowerState_t) tmPowerOff,	/*             */
	False,			/*                   */
	HDMITX_SINK_DVI,	/*          */
	HDMITX_SINK_DVI,	/*              */
	False,			/*            */
	0,			/*              */

	0,			/*                   */
	{
	 False,			/*                   */
	 False,			/*                    */
	 0,			/*                   */
	 0,			/*                   */
	 0,			/*                    */
	 0},			/*                    */

	{
	 0,			/*                              */
	 0,			/*                              */
	 0,			/*                    */
	 0,			/*                     */
	 0,			/*                   */
	 0,			/*                         */
	 0,			/*                             */
	 0,			/*                            */
	 0,			/*                                                       */
	 0,			/*                                  */
	 0,			/*                                        */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}	/*                          */
	 },

	HDMITX_EDID_NOT_READ,	/*            */
	0,			/*             */
	{			/*          *//*                     */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/*  */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/*  */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/*  */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/*  */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/*  */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/*  */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/*  */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/*  */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/*  */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}	/*   */
	 },
	{			/*                       */
	 False,			/*             */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}	/*                                                  */
	 },
	{
	 False,			/*             */
	 0,			/*                                                  */
	 0,			/*                                                  */
	 0,			/*                                                  */
	 0,			/*                                                  */
	 0			/*                                                  */
	 },
	{
	 False,			/*             */
	 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}	/*                                                  */
	 },
	{			/*             */
	 HDMITX_VFMT_NULL},
	0,			/*            */
	{			/*              */
	 {0, 0, 0}		/*                           */
	 },
	0,			/*            */
	{
	 0			/*              */
	 },
	0,			/*              */
	0,			/*                   */
	0,			/*                    */
	False,			/*                 */
	{			/*           */
	 0,			/*          */
	 0			/*           */
	 },
	{			/*                       */
	 0,			/*                */
	 0,			/*                    */
	 0,			/*                  */
	 0,			/*        */
	 0,			/*                 */
	 },
#ifdef TMFL_HDCP_SUPPORT
	False,			/*                   */
	0,			/*                 */
	HDMITX_HDCP_TXMODE_NOT_SET,	/*            */
	HDMITX_HDCP_OPTION_DEFAULT,	/*             */
	0,			/*           */
	0,			/*             */
	0,			/*        */
	0,			/*              */
	0,			/*                 */
	0,			/*          */
	{0, 0, 0, 0, 0},	/*          */
	(ptmHdmiTxFunc_t) 0,	/*                   */
	0,			/*                     */
	0,			/*                     */
	0,			/*                      */
	0,			/*              */
	0,			/*                */
#endif				/*                   */
	HDMITX_VFMT_NULL,	/*        */
	HDMITX_VFMT_NULL,	/*         */
	HDMITX_PIXRATE_DOUBLE,	/*         */
	HDMITX_VINMODE_RGB444,	/*         */
	HDMITX_VOUTMODE_RGB444,	/*          */
	HDMITX_VFREQ_INVALID,	/*          */
	HDMITX_SCAMODE_OFF,	/*         */
	HDMITX_UPSAMPLE_AUTO,	/*              */
	HDMITX_PIXREP_MIN,	/*                  */
	HDMITX_HOTPLUG_INVALID,	/*               */
	HDMITX_RX_SENSE_INVALID,	/*               */
	E_PAGE_INVALID,		/*            */
	{
	 /*                                              */
	 0x00,			/*                    */
	 0x00,			/*                    */
	 0x00,			/*                    */
	 0x01,			/*                    */
	 0x24,			/*                    */
	 0x56,			/*                    */
	 0x17,			/*                    */
	 0x01,			/*                    */
	 0x00,			/*                    */
	 0x05,			/*                    */
	 0x00,			/*                    */
	 0x00,			/*                    */
	 0x00,			/*                    */
	 0x00,			/*                    */
	 0x00,			/*                    */
	 0x00,			/*                    */
	 0x00			/*                    */
	 , 0x00			/*                     */
	 , 0x00			/*                    */
	 , 0x00			/*                    */
#ifdef TMFL_HDCP_SUPPORT
	 , 0x00			/*               */
	 , 0x00			/*               */
#endif				/*                   */
	 },
	False,			/*                                 */
	False,			/*                           */
	False,			/*              */
	HDMITX_VQR_DEFAULT
};


/* 
                                                             
                                                                         
                      
 */
static UInt16 kShadowReg[E_SNUM] = {	/*                                                     */
	E_REG_P00_INT_FLAGS_0_RW,	/*                     */
	E_REG_P00_INT_FLAGS_1_RW,	/*                     */
	E_REG_P00_INT_FLAGS_2_RW,	/*                     */
	E_REG_P00_VIP_CNTRL_0_W,	/*                     */
	E_REG_P00_VIP_CNTRL_1_W,	/*                     */
	E_REG_P00_VIP_CNTRL_2_W,	/*                     */
	E_REG_P00_VIP_CNTRL_3_W,	/*                     */
	E_REG_P00_VIP_CNTRL_4_W,	/*                     */
	E_REG_P00_VIP_CNTRL_5_W,	/*                     */
	E_REG_P00_MAT_CONTRL_W,	/*                     */
	E_REG_P00_TBG_CNTRL_0_W,	/*                     */
	E_REG_P00_TBG_CNTRL_1_W,	/*                     */
	E_REG_P00_HVF_CNTRL_0_W,	/*                     */
	E_REG_P00_HVF_CNTRL_1_W,	/*                     */
	E_REG_P00_TIMER_H_W,	/*                     */
	E_REG_P00_DEBUG_PROBE_W,	/*                     */
	E_REG_P00_AIP_CLKSEL_W,	/*                     */
	E_REG_P01_SC_VIDFORMAT_W,	/*                     */
	E_REG_P01_SC_CNTRL_W,	/*                     */
	E_REG_P01_TBG_CNTRL_0_W	/*                     */
#ifdef TMFL_HDCP_SUPPORT
	    , E_REG_P12_CTRL_W	/*                */
	    , E_REG_P12_BCAPS_W	/*                */
#endif				/*                   */
};


/* 
                                                      

                                         
 
                                                                       
                                                                       
                                                                       
                                                                       
                                                                       
                                                                       
                                                                       
           
  
*/
/* 
                                                        

                                        
 
                                                                       
                                                                       
                                                                       
                                                                       
                                                                       
                                                                       
                                                                       
                                                                       
           
  
*/

static tmbslHdmiTxCallbackInt_t kITCallbackPriority[HDMITX_CALLBACK_INT_NUM] = {
	HDMITX_CALLBACK_INT_R0,		/*                                    */
	HDMITX_CALLBACK_INT_ENCRYPT,	/*                                    */
	HDMITX_CALLBACK_INT_HPD,	/*                                    */
	HDMITX_CALLBACK_INT_T0,		/*                                    */
	HDMITX_CALLBACK_INT_BCAPS,	/*                                    */
	HDMITX_CALLBACK_INT_BSTATUS,	/*                                    */
	HDMITX_CALLBACK_INT_SHA_1,	/*                                    */
	HDMITX_CALLBACK_INT_PJ,		/*                                    */
	HDMITX_CALLBACK_INT_SW_INT,	/*                                    */
	HDMITX_CALLBACK_INT_RX_SENSE,	/*                                    */
	HDMITX_CALLBACK_INT_EDID_BLK_READ,
					/*                                    */
	HDMITX_CALLBACK_INT_VS_RPT,	/*                                    */
	HDMITX_CALLBACK_INT_PLL_LOCK	/*                                    */
};



#ifdef TMFL_TDA9989_PIXEL_CLOCK_ON_DDC

UInt8 kndiv_im[] = {
	0,			/*                       */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	12,			/*                               */
	12,			/*                                */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                               */
	4,			/*                               */
	12,			/*                                */
	4,			/*                              */
	4,			/*                              */
	12,			/*                               */
	12,			/*                                */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                              */
	4,			/*                               */
	4,			/*                               */
	12,			/*                                */
	12,			/*                                */
	12,			/*                                */
	12,			/*                                */

};

UInt8 kclk_div[] = {
	0,			/*                       */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                               */
	44,			/*                                */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                               */
	44,			/*                               */
	44,			/*                                */
	44,			/*                              */
	44,			/*                              */
	44,			/*                               */
	44,			/*                                */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                              */
	44,			/*                               */
	44,			/*                               */
	44,			/*                                */
	44,			/*                                */
	44,			/*                                */
	44,			/*                                */
};

#endif				/*                                 */


/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */

#ifdef TMFL_HDCP_SUPPORT
static UInt32 sgBcapsCounter;
#endif				/*                   */

#define TDA19989_DDC_SPEED_FACTOR 39

static Bool gMiscInterruptHpdRxEnable = False;	/*                                  */
					       /*                                  */
static UInt8 int_level = 0xFF;
/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989Deinit(tmUnitSelect_t txUnit) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 EnableModeMask = 0;	/*                */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                                                     */
	    /*                                   */
	    EnableModeMask = 0x40;
	EnableModeMask |= E_MASKREG_CEC_ENAMODS_ena_hdmi;	/*                 */
	EnableModeMask &= ~E_MASKREG_CEC_ENAMODS_dis_fro;	/*             */
	err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, EnableModeMask);
	RETIF_REG_FAIL(err)

	    /*                                        */
	    err = setHwRegisterField(pDis, E_REG_P00_MAIN_CNTRL0_RW,
				     E_MASKREG_P00_MAIN_CNTRL0_sr, 1);
	RETIF_REG_FAIL(err)

	    /*                                                                                     */
	    EnableModeMask &= ~E_MASKREG_CEC_ENAMODS_ena_hdmi;	/*                   */
	EnableModeMask &= ~E_MASKREG_CEC_ENAMODS_ena_rxs;	/*                    */
	EnableModeMask |= E_MASKREG_CEC_ENAMODS_dis_fro;	/*              */
	err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, EnableModeMask);
	RETIF_REG_FAIL(err)

	    /*                                                           */
	    pDis->bInitialized = False;

	setState(pDis, EV_DEINIT);
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HotPlugGetStatus(tmUnitSelect_t txUnit, tmbslHdmiTxHotPlug_t *pHotPlugStatus, Bool client	/*                                                                  */
    ) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regVal;		/*                */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                            */
	    RETIF_BADPARAM(pHotPlugStatus == (tmbslHdmiTxHotPlug_t *) 0)

	    /*                    */
	    err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &regVal);
	RETIF(err != TM_OK, err)

	    /*                                                                               */
	    if (client) {
		*pHotPlugStatus = (regVal & E_MASKREG_CEC_RXSHPDLEV_hpd_level) ?
		    HDMITX_HOTPLUG_ACTIVE : HDMITX_HOTPLUG_INACTIVE;
	} else {

		*pHotPlugStatus = pDis->hotPlugStatus;

	}
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989RxSenseGetStatus(tmUnitSelect_t txUnit, tmbslHdmiTxRxSense_t *pRxSenseStatus, Bool client	/*                                                                  */
    ) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regVal;		/*                */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                            */
	    RETIF_BADPARAM(pRxSenseStatus == (tmbslHdmiTxRxSense_t *) 0)


	    /*                    */
	    err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &regVal);
	RETIF(err != TM_OK, err)


	    /*                                                                       */
	    if (client) {
		*pRxSenseStatus = (regVal & E_MASKREG_CEC_RXSHPDLEV_rxs_level) ?
		    HDMITX_RX_SENSE_ACTIVE : HDMITX_RX_SENSE_INACTIVE;
	} else {
		*pRxSenseStatus = pDis->rxSenseStatus;
	}

	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989HwGetRegisters
    (tmUnitSelect_t txUnit, Int regPage, Int regAddr, UInt8 *pRegBuf, Int nRegs) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	Int i;			/*            */
	UInt8 newRegPage;	/*                                */
	UInt8 regShad;		/*                                     */
	UInt16 regShadPageAddr;	/*                                 */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                         */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                            */
	    RETIF_BADPARAM((regPage < kPageIndexToPage[E_PAGE_00])
			   || ((regPage > kPageIndexToPage[E_PAGE_02])
			       && (regPage < kPageIndexToPage[E_PAGE_09]))
			   || ((regPage > kPageIndexToPage[E_PAGE_09])
			       && (regPage < kPageIndexToPage[E_PAGE_11]))
			   || (regPage > kPageIndexToPage[E_PAGE_12]))
	    RETIF_BADPARAM((regAddr < E_REG_MIN_ADR) || (regAddr >= E_REG_CURPAGE_ADR_W))
	    RETIF_BADPARAM(pRegBuf == (pUInt8) 0)
	    RETIF_BADPARAM((nRegs < 1) || ((nRegs + regAddr) > E_REG_CURPAGE_ADR_W))

	    /*                               */
	    newRegPage = (UInt8) regPage;
	if (pDis->curRegPage != newRegPage) {
		/*                               */
		sysArgs.slaveAddr = pDis->uHwAddress;
		sysArgs.firstRegister = E_REG_CURPAGE_ADR_W;
		sysArgs.lenData = 1;
		sysArgs.pData = &newRegPage;
		err = pDis->sysFuncWrite(&sysArgs);
		RETIF(err != TM_OK, TMBSL_ERR_HDMI_I2C_WRITE)
		    pDis->curRegPage = newRegPage;
	}

	/*                                                                */
	for (; nRegs > 0; pRegBuf++, regAddr++, nRegs--) {
		/*                            
                                                                     
                                                                    
                                                                       
   */
		regShad = E_SNONE;
		for (i = 0; i < E_SNUM; i++) {
			/*                                                    */
			regShadPageAddr = kShadowReg[i];
			if ((SPA2PAGE(regShadPageAddr) == newRegPage)
			    && (SPA2ADDR(regShadPageAddr) == regAddr)) {
				/*                                                   */
				regShad = SPA2SHAD(regShadPageAddr);
				break;
			}
		}
		/*                                                                
                                          */
		if (regShad != E_SNONE) {
			*pRegBuf = pDis->shadowReg[regShad];
		} else {
			/*                                                          
                                                                   
                      */
			sysArgs.slaveAddr = pDis->uHwAddress;
			sysArgs.firstRegister = (UInt8) regAddr;
			sysArgs.lenData = 1;
			sysArgs.pData = pRegBuf;
			err = pDis->sysFuncRead(&sysArgs);
			RETIF(err != TM_OK, TMBSL_ERR_HDMI_I2C_READ)
		}
	}

	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t tmbslTDA9989HwGetVersion(tmUnitSelect_t txUnit, pUInt8 pHwVersion) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regVal;

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                            */
	    RETIF_BADPARAM(pHwVersion == (pUInt8) 0)

	    /*                       */
	    err = getHwRegister(pDis, E_REG_P00_VERSION_MSB_RW, &regVal);
	RETIF(err != TM_OK, err)

	    switch (regVal) {
	case 0x01:
		*pHwVersion = (UInt8) (BSLHDMITX_TDA9989);
		break;
	case 0x02:
		*pHwVersion = (UInt8) (BSLHDMITX_TDA19989);
		break;
	case 0x03:
		*pHwVersion = (UInt8) (BSLHDMITX_TDA19988);
		break;
	default:
		*pHwVersion = (UInt8) (BSLHDMITX_UNKNOWN);
		break;
	}

	return TM_OK;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HwHandleInterrupt(tmUnitSelect_t txUnit) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regVal;		/*                */
	UInt8 regVal1;		/*                */
	UInt16 fInterruptStatus;	/*                 */
	UInt16 fInterruptMask;	/*                                 */
	tmbslHdmiTxRxSense_t newRxs_fil;	/*                         */
	Int i;			/*              */
	tmbslHdmiTxHotPlug_t newHpdIn;	/*                          */
	Bool sendEdidCallback;
	Bool hpdOrRxsLevelHasChanged = False;

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    fInterruptStatus = 0;
	sendEdidCallback = False;



	/*                         */
	err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDINT_R, &regVal);
	RETIF(err != TM_OK, err);

	/*                    */
	err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &regVal1);
	RETIF(err != TM_OK, err);

	if (int_level != 0xFF) {	/*                     */
		/*                        */
		if ((regVal == 0) && (int_level != regVal1)) {
#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
			pr_debug("HDMI Int multi-transition\n");
#endif
			err = setCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, 0x00);
			err += setCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW,
						E_MASKREG_CEC_RXSHPDINTENA_ena_rxs_int |
						E_MASKREG_CEC_RXSHPDINTENA_ena_hpd_int);
			err += getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &regVal1);
			RETIF(err != TM_OK, err)
		}
	}
	int_level = regVal1;

	/*                                                                               */
	newHpdIn = (regVal1 & E_MASKREG_CEC_RXSHPDLEV_hpd_level) ?
	    HDMITX_HOTPLUG_ACTIVE : HDMITX_HOTPLUG_INACTIVE;

	/*                                                                       */
	newRxs_fil = (regVal1 & E_MASKREG_CEC_RXSHPDLEV_rxs_level) ?
	    HDMITX_RX_SENSE_ACTIVE : HDMITX_RX_SENSE_INACTIVE;

	/*                                              */

	if (newHpdIn != pDis->hotPlugStatus) {
		fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_HPD);
		/*                         */
		pDis->hotPlugStatus = newHpdIn;
		hpdOrRxsLevelHasChanged = True;
	}

	/*                                                   */
	if (newRxs_fil != pDis->rxSenseStatus) {
		fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_RX_SENSE);
		/*                             */
		pDis->rxSenseStatus = newRxs_fil;
		hpdOrRxsLevelHasChanged = True;
	}



	/*                         */
	err = getCECHwRegister(pDis, E_REG_CEC_INTERRUPTSTATUS_R, &regVal);
	RETIF(err != TM_OK, err)

	    /*                                         */
	    if ((regVal & E_MASKREG_CEC_INTERRUPTSTATUS_hdmi_int) == 0x00) {

		if (hpdOrRxsLevelHasChanged == True) {
		} else {
			return TM_OK;
		}
	}



    /*                                                                                              */
    /*                                                                                              */
    /*                                                                                              */

	/*                       */
	if (pDis->ePowerState == tmPowerOn) {
		/*                                                                  
                                                                         
                     
   */
		err = getHwRegister(pDis, E_REG_P00_INT_FLAGS_0_RW, &regVal);
		RETIF(err != TM_OK, err)
#ifdef TMFL_HDCP_SUPPORT
		    /*         */
		    if ((regVal & E_MASKREG_P00_INT_FLAGS_0_encrypt) != 0) {
			fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_ENCRYPT);
		}
#endif				/*                   */

		/*                       */
		if ((regVal & E_MASKREG_P00_INT_FLAGS_0_t0) != 0) {
			fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_T0);
		}
#ifdef TMFL_HDCP_SUPPORT
		/*       */
		if ((regVal & E_MASKREG_P00_INT_FLAGS_0_bcaps) != 0) {
			fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_BCAPS);

			/*                  */
			if (pDis->uDeviceVersion == E_DEV_VERSION_TDA19989) {

				/*                         */

				sgBcapsCounter++;

				if (sgBcapsCounter == 49) {
					sgBcapsCounter = 0;
					/*                      */
					fInterruptStatus =
					    fInterruptStatus | (1 << HDMITX_CALLBACK_INT_T0);
				}

			}
			/*                  */
		}

		/*         */
		if ((regVal & E_MASKREG_P00_INT_FLAGS_0_bstatus) != 0) {
			fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_BSTATUS);

			/*                  */
			if (pDis->uDeviceVersion == E_DEV_VERSION_TDA19989) {

				/*                         */
				sgBcapsCounter = 0;

			}
			/*                  */
		}

		/*       */
		if ((regVal & E_MASKREG_P00_INT_FLAGS_0_sha_1) != 0) {
			fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_SHA_1);
		}

		/*    */
		if ((regVal & E_MASKREG_P00_INT_FLAGS_0_pj) != 0) {
			fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_PJ);
		}

		/*    */
		if ((regVal & E_MASKREG_P00_INT_FLAGS_0_r0) != 0) {
			fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_R0);

			/*                  */
			if (pDis->uDeviceVersion == E_DEV_VERSION_TDA19989) {

				/*                         */
				sgBcapsCounter = 0;

			}
			/*                  */
		}
#endif				/*                   */


		err = getHwRegister(pDis, E_REG_P00_INT_FLAGS_1_RW, &regVal);
		RETIF(err != TM_OK, err)


		    /*                                  */
		    if ((regVal & E_MASKREG_P00_INT_FLAGS_1_sw_int) != 0) {
			fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_SW_INT);
		}

		/*                                */
		if (((pDis->InterruptsEnable & E_MASKREG_P00_INT_FLAGS_1_vs_rpt) != 0) &&
		    ((regVal & E_MASKREG_P00_INT_FLAGS_1_vs_rpt) != 0)
		    ) {
			fInterruptStatus = fInterruptStatus | (1 << HDMITX_CALLBACK_INT_VS_RPT);
		}

		/*                                          
                                                      
                      */
		err = getHwRegister(pDis, E_REG_P00_INT_FLAGS_2_RW, &regVal);
		RETIF(err != TM_OK, err)

		    /*                                      */
		    if ((regVal & E_MASKREG_P00_INT_FLAGS_2_edid_blk_rd) != 0) {
			fInterruptStatus =
			    fInterruptStatus | (1 << HDMITX_CALLBACK_INT_EDID_BLK_READ);
		}
	}


	/*                          */
	if ((fInterruptStatus & (1 << HDMITX_CALLBACK_INT_HPD)) != 0) {
		/*                                                              */
		if (gMiscInterruptHpdRxEnable) {
			/*                   */
			err = ClearEdidRequest(txUnit);

			/*                                                    
                                                                                         */
			fInterruptStatus &= (1 << HDMITX_CALLBACK_INT_HPD) |
			    (1 << HDMITX_CALLBACK_INT_RX_SENSE) | (1 << HDMITX_CALLBACK_INT_SW_INT);

			if (pDis->ePowerState == tmPowerOn) {
				if ((pDis->hotPlugStatus == HDMITX_HOTPLUG_ACTIVE)) {

					/*                  */
					if (pDis->uDeviceVersion == E_DEV_VERSION_TDA19989) {

						err = tmbslTDA9989Reset(txUnit);
						RETIF(err != TM_OK, err)

						    err = hotPlugRestore(txUnit);
						RETIF(err != TM_OK, err)

					}
					/*                  */
					else {	/*             */

						HDCP_F2;
					}

#ifdef TMFL_TDA9989_PIXEL_CLOCK_ON_DDC

					err = tmbslTDA9989Reset(txUnit);
					RETIF(err != TM_OK, err)

					    err = hotPlugRestore(txUnit);
					RETIF(err != TM_OK, err)
#endif				/*                                 */
					    setState(pDis, EV_PLUGGEDIN);
				} else {
					setState(pDis, EV_UNPLUGGED);
				}
			}
		}
	} else {
		/*                                           */
		fInterruptStatus &= ~(1 << HDMITX_CALLBACK_INT_HPD);

		if (fInterruptStatus & (1 << HDMITX_CALLBACK_INT_EDID_BLK_READ)) {
			err = EdidBlockAvailable(txUnit, &sendEdidCallback);
			RETIF(err != TM_OK, err)
			    if (sendEdidCallback == False) {
				/*                                       */
				fInterruptStatus &= ~(1 << HDMITX_CALLBACK_INT_EDID_BLK_READ);
			} else {
#ifdef TMFL_TDA9989_PIXEL_CLOCK_ON_DDC

				if ((pDis->vinFmt == HDMITX_VFMT_16_1920x1080p_60Hz)
				    || (pDis->vinFmt == HDMITX_VFMT_31_1920x1080p_50Hz)) {

					err = setHwRegisterField(pDis,
								 E_REG_P02_PLL_SERIAL_3_RW,
								 E_MASKREG_P02_PLL_SERIAL_3_srl_ccir,
								 0x00);
					RETIF_REG_FAIL(err)
				}
#endif				/*                                 */
			}


		}
	}

	/*                         */
	if ((fInterruptStatus & (1 << HDMITX_CALLBACK_INT_RX_SENSE)) != 0) {
		/*                                                              */
		if (gMiscInterruptHpdRxEnable) {


			fInterruptStatus &= (1 << HDMITX_CALLBACK_INT_HPD) |
			    (1 << HDMITX_CALLBACK_INT_RX_SENSE) | (1 << HDMITX_CALLBACK_INT_SW_INT);

			if (pDis->rxSenseStatus == HDMITX_RX_SENSE_ACTIVE) {
				setState(pDis, EV_SINKON);
			} else {
				setState(pDis, EV_SINKOFF);
			}
		}
	} else {
		/*                                            */
		fInterruptStatus &= ~(1 << HDMITX_CALLBACK_INT_RX_SENSE);
	}

	/*                                                           
                                 */

	if (fInterruptStatus & (1 << HDMITX_CALLBACK_INT_T0)) {
		if (pDis->EdidReadStarted) {

#ifdef TMFL_HDCP_SUPPORT
			err = getHwRegister(pDis, E_REG_P12_TX0_RW, &regVal);
			RETIF(err != TM_OK, err)

			    /*                   */
			    if ((regVal & E_MASKREG_P12_TX0_sr_hdcp) != 0) {

#endif				/*                   */


				/*                   */
				err = ClearEdidRequest(txUnit);
				RETIF(err != TM_OK, err)

				    /*                      */
				    fInterruptStatus =
				    (UInt16) (fInterruptStatus & (~(1 << HDMITX_CALLBACK_INT_T0)));
				fInterruptStatus =
				    fInterruptStatus | (1 << HDMITX_CALLBACK_INT_EDID_BLK_READ);

#ifdef TMFL_HDCP_SUPPORT
			}
#endif				/*                   */

		} else {
			fInterruptStatus &= ((1 << HDMITX_CALLBACK_INT_HPD)
					     | (1 << HDMITX_CALLBACK_INT_T0)
					     | (1 << HDMITX_CALLBACK_INT_RX_SENSE)
					     | (1 << HDMITX_CALLBACK_INT_SW_INT)
			    );
		}
	}

	HDCP_F3;

	/*                                                                        
                                                              
                           
  */
	fInterruptMask = 1;
	for (i = 0; i < HDMITX_CALLBACK_INT_NUM; i++) {
		if (i != HDMITX_CALLBACK_INT_PLL_LOCK) {	/*                                 */

			fInterruptMask = 1;
			fInterruptMask = fInterruptMask << ((UInt16) kITCallbackPriority[i]);

			if (fInterruptStatus & fInterruptMask) {
				/*                                                            */
				if (pDis->funcIntCallbacks[kITCallbackPriority[i]] !=
				    (ptmbslHdmiTxCallback_t) 0) {
					pDis->funcIntCallbacks[kITCallbackPriority[i]] (txUnit);
				}
			}

		}
	}
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
/*                                                                            */
tmErrorCode_t tmbslTDA9989FlagSwInt(tmUnitSelect_t txUnit, UInt32 uSwInt) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	DUMMY_ACCESS(uSwInt);

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    err = setHwRegister(pDis, E_REG_P00_SW_INT_W, E_MASKREG_P00_SW_INT_sw_int);


	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t
    tmbslTDA9989HwSetRegisters
    (tmUnitSelect_t txUnit, Int regPage, Int regAddr, UInt8 *pRegBuf, Int nRegs) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	Int i;			/*            */
	UInt8 newRegPage;	/*                                */
	UInt8 regShad;		/*                                     */
	UInt16 regShadPageAddr;	/*                                 */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                     */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                            */
	    RETIF_BADPARAM((regPage < kPageIndexToPage[E_PAGE_00])
			   || ((regPage > kPageIndexToPage[E_PAGE_02])
			       && (regPage < kPageIndexToPage[E_PAGE_11]))
			   || (regPage > kPageIndexToPage[E_PAGE_12]))
	    RETIF_BADPARAM((regAddr < E_REG_MIN_ADR) || (regAddr >= E_REG_CURPAGE_ADR_W))
	    RETIF_BADPARAM(pRegBuf == (pUInt8) 0)
	    RETIF_BADPARAM((nRegs < 0) || ((nRegs + regAddr) > E_REG_CURPAGE_ADR_W))

	    /*                               */
	    newRegPage = (UInt8) regPage;
	if (pDis->curRegPage != newRegPage) {
		/*                               */
		sysArgs.slaveAddr = pDis->uHwAddress;
		sysArgs.firstRegister = E_REG_CURPAGE_ADR_W;
		sysArgs.lenData = 1;
		sysArgs.pData = &newRegPage;
		err = pDis->sysFuncWrite(&sysArgs);
		RETIF(err != TM_OK, TMBSL_ERR_HDMI_I2C_WRITE)
		    pDis->curRegPage = newRegPage;
	}

	/*                                                                 
                                                       
  */
	for (; nRegs > 0; pRegBuf++, regAddr++, nRegs--) {
		/*                            
                                                                     
                                                                    
                                                                       
   */
		for (i = 0; i < E_SNUM; i++) {
			/*                                                    */
			regShadPageAddr = kShadowReg[i];
			if ((SPA2PAGE(regShadPageAddr) == newRegPage)
			    && (SPA2ADDR(regShadPageAddr) == regAddr)) {
				/*                                         */
				regShad = SPA2SHAD(regShadPageAddr);
				pDis->shadowReg[regShad] = *pRegBuf;
				break;
			}
		}
		/*                                                           */
		sysArgs.slaveAddr = pDis->uHwAddress;
		sysArgs.firstRegister = (UInt8) regAddr;
		sysArgs.lenData = 1;
		sysArgs.pData = pRegBuf;
		err = pDis->sysFuncWrite(&sysArgs);
		RETIF(err != TM_OK, TMBSL_ERR_HDMI_I2C_WRITE)
	}

	return TM_OK;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
void
 tmbslTDA9989HwStartup(void
    ) {
	pr_debug("%s\n", __func__);
	/*                                                            */
	lmemset(&gHdmiTxInstance, 0, sizeof(gHdmiTxInstance));
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989Init
    (tmUnitSelect_t txUnit,
     UInt8 uHwAddress,
     ptmbslHdmiTxSysFunc_t sysFuncWrite,
     ptmbslHdmiTxSysFunc_t sysFuncRead,
     ptmbslHdmiTxSysFuncEdid_t sysFuncEdidRead,
     ptmbslHdmiTxSysFuncTimer_t sysFuncTimer,
     tmbslHdmiTxCallbackList_t *funcIntCallbacks,
     Bool bEdidAltAddr, tmbslHdmiTxVidFmt_t vinFmt, tmbslHdmiTxPixRate_t pixRate) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	Int i;			/*            */
	Bool bFound;		/*                      */
	UInt8 EnableIntMask = 0;	/*                                                */
	UInt8 EnableModeMask;	/*                                         */
	UInt16 val16Bits;	/*                 */
	UInt8 regVal;		/*                */

	pr_debug("%s\n", __func__);
	/*                                              */
	RETIF(txUnit < tmUnit0, TMBSL_ERR_HDMI_BAD_UNIT_NUMBER)
	    RETIF(txUnit >= HDMITX_UNITS_MAX, TMBSL_ERR_HDMI_BAD_UNIT_NUMBER)
	    pDis = &gHdmiTxInstance[txUnit];

	/*                                                                   */
	RETIF(pDis->bInitialized, TMBSL_ERR_HDMI_INIT_FAILED)

	    /*                            */
	    RETIF_BADPARAM(uHwAddress < HDMITX_SLAVE_ADDRESS_MIN)
	    RETIF_BADPARAM(uHwAddress > HDMITX_SLAVE_ADDRESS_MAX)
	    RETIF_BADPARAM(sysFuncWrite == (ptmbslHdmiTxSysFunc_t) 0)
	    RETIF_BADPARAM(sysFuncRead == (ptmbslHdmiTxSysFunc_t) 0)
	    /*                                                                *//*                      */
	    /*                                                              */
	    RETIF_BADPARAM((bEdidAltAddr != True) && (bEdidAltAddr != False))
	    RETIF_BADPARAM(!IS_VALID_FMT(vinFmt))
	    RETIF_BADPARAM(pixRate >= HDMITX_PIXRATE_INVALID)

	    /*                                                             */
	    lmemcpy(pDis, &kHdmiTxInstanceDefault, sizeof(*pDis));

	/*                                                                
                                                                       
  */
	pDis->txUnit = txUnit;
#ifdef UNIT_TEST
	/*                                                                         
                                                                  */
	pDis->uHwAddress = (UInt8) txUnit;
#else
	/*                          */
	pDis->uHwAddress = uHwAddress;
#endif
	pDis->sysFuncWrite = sysFuncWrite;
	pDis->sysFuncRead = sysFuncRead;
	pDis->sysFuncEdidRead = sysFuncEdidRead;
	pDis->sysFuncTimer = sysFuncTimer;

	/*                                                 
                                                            
                                                       
                             
  */

	for (i = 0; i < HDMITX_CALLBACK_INT_NUM; i++) {
		if ((funcIntCallbacks != (tmbslHdmiTxCallbackList_t *) 0)
		    && (funcIntCallbacks->funcCallback[i] != (ptmbslHdmiTxCallback_t) 0)) {
			pDis->funcIntCallbacks[i] = funcIntCallbacks->funcCallback[i];
		} else {
			pDis->funcIntCallbacks[i] = (ptmbslHdmiTxCallback_t) 0;
		}
	}

	/*                                               */
	pDis->bEdidAlternateAddr = bEdidAltAddr;

/*                                                                                         *// */
/*                                                                                         *// */
/*                                                                     *///   

	/*               */
	err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, 0x40);
	RETIF_REG_FAIL(err)





	    /*                                      */
	    err = getCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, &EnableModeMask);

	/*                      */
	EnableModeMask |= E_MASKREG_CEC_ENAMODS_ena_hdmi;	/*                 */
	EnableModeMask |= E_MASKREG_CEC_ENAMODS_ena_rxs;	/*                    */
	EnableModeMask &= ~E_MASKREG_CEC_ENAMODS_dis_fro;	/*             */

	/*                                   */
	err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, EnableModeMask);

	pDis->ePowerState = (tmbslHdmiTxPowerState_t) tmPowerOn;

	/*                                                */
	pDis->bInitialized = True;

	/*                  */
	err = tmbslTDA9989Reset(txUnit);
	RETIF(err != TM_OK, err)
/*                                                                                       *// */
/*                                                                                       *// */
	    /*                                                          
                                 
      */
	    err = getHwRegister(pDis, E_REG_P00_VERSION_R, &regVal);
	RETIF(err != TM_OK, err)

	    /*                              */
	    pDis->uDeviceFeatures = regVal &
	    (E_MASKREG_P00_VERSION_not_h | E_MASKREG_P00_VERSION_not_s);

	pDis->uDeviceVersion = regVal;

	/*                       */
	err = getHwRegister(pDis, E_REG_P00_VERSION_MSB_RW, &regVal);
	RETIF(err != TM_OK, err)

	    /*                           */
	    val16Bits = regVal;
	pDis->uDeviceVersion = pDis->uDeviceVersion | (val16Bits << 8);
	val16Bits = pDis->uDeviceFeatures;
	pDis->uDeviceVersion &= ~val16Bits;

	if (pDis->uDeviceVersion != E_DEV_VERSION_LIST_END) {
		/*                                                       
                                           
   */
		for (i = 0, bFound = False; i < E_DEV_VERSION_LIST_NUM; i++) {
			if (pDis->uDeviceVersion == pDis->uSupportedVersions[i]) {
				bFound = True;
			}
		}
		if (bFound == False) {
			/*                                                                      
                                                              */
			err = tmbslTDA9989Deinit(txUnit);
			RETIF(err != TM_OK, err)
			    return TMBSL_ERR_HDMI_COMPATIBILITY;
		}
	} else {
		/*                            */
		err = tmbslTDA9989Deinit(txUnit);
		RETIF(err != TM_OK, err)
		    return TMBSL_ERR_HDMI_COMPATIBILITY;
	}

/*                                                                                     */
/*                                                                                     */
	regVal = HDMI_TX_VSWING_VALUE;

	err = setHwRegister(pDis, E_REG_P02_ANA_GENERAL_RW, regVal);
	RETIF(err != TM_OK, err)

/*                                                                                       */
/*                                                                                       */
	    /*                                         */
	    /*                                    */
	    err = setHwRegisterFieldTable(pDis, &kCommonPllCfg[0]);
	RETIF_REG_FAIL(err)

	    /*                                            */
	    err =
	    setHwRegisterField(pDis, E_REG_P00_VIP_CNTRL_4_W, E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);

	switch (vinFmt) {
		/*                                 */
	case HDMITX_VFMT_06_720x480i_60Hz:
	case HDMITX_VFMT_07_720x480i_60Hz:
	case HDMITX_VFMT_21_720x576i_50Hz:
	case HDMITX_VFMT_22_720x576i_50Hz:
		err = setHwRegisterFieldTable(pDis, &kVfmt480i576iPllCfg[0]);
		RETIF_REG_FAIL(err)

		    switch (pixRate) {
		case HDMITX_PIXRATE_SINGLE:
			/*                                       */
			err = setHwRegisterFieldTable(pDis, &kSinglePrateVfmt480i576iPllCfg[0]);
			RETIF_REG_FAIL(err)

			    break;
		case HDMITX_PIXRATE_SINGLE_REPEATED:
			/*                                                */
			err = setHwRegisterFieldTable(pDis, &kSrepeatedPrateVfmt480i576iPllCfg[0]);
			RETIF_REG_FAIL(err)

			    break;
		default:
			/*                                                        */
			return (TMBSL_ERR_HDMI_INCONSISTENT_PARAMS);
		}

		break;


		/*                           */
	default:
		err = setHwRegisterFieldTable(pDis, &kVfmtOtherPllCfg[0]);
		RETIF_REG_FAIL(err)

		    switch (pixRate) {
		case HDMITX_PIXRATE_SINGLE:
			/*                                                  */
			err = setHwRegisterFieldTable(pDis, &kSinglePrateVfmtOtherPllCfg[0]);
			RETIF_REG_FAIL(err)
			    break;
		case HDMITX_PIXRATE_DOUBLE:
			/*                                                  */
			err = setHwRegisterFieldTable(pDis, &kDoublePrateVfmtOtherPllCfg[0]);
			RETIF_REG_FAIL(err)
			    break;
		default:
			/*                                                          */
			return (TMBSL_ERR_HDMI_INCONSISTENT_PARAMS);
		}
		break;

	}

	/*                                                             */
	err = setHwRegister(pDis, E_REG_P00_DDC_DISABLE_RW, 0x00);
	RETIF(err != TM_OK, err)

	    /*                                    */
	    err = setHwRegister(pDis, E_REG_P12_TX3_RW, TDA19989_DDC_SPEED_FACTOR);
	RETIF(err != TM_OK, err)

	    /*                  */
	    if (pDis->uDeviceVersion == E_DEV_VERSION_TDA19989) {

		err = setHwRegisterField(pDis, E_REG_P00_I2C_MASTER_RW, E_MASKREG_P00_I2C_MASTER_dis_mm, 0);	/*                             */
		RETIF_REG_FAIL(err)

	}

	/*                  */
	err =
	    setCECHwRegister(pDis, E_REG_CEC_FRO_IM_CLK_CTRL_RW,
			     E_MASKREG_CEC_FRO_IM_CLK_CTRL_ghost_dis |
			     E_MASKREG_CEC_FRO_IM_CLK_CTRL_imclk_sel);
	RETIF_REG_FAIL(err)

	    /*                                                                   
                                                                         
                                                                    */
	    /*                                              */
	    RETIF(err != TM_OK, err)

	    /*                                                   */
	    err = setHwRegister(pDis, E_REG_P00_INT_FLAGS_1_RW, E_MASKREG_P00_INT_FLAGS_1_sw_int);

	/*                  */
	err = setHwRegister(pDis, E_REG_P00_INT_FLAGS_2_RW, E_MASKREG_P00_INT_FLAGS_2_edid_blk_rd);


	/*                    */
	err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &regVal);
	RETIF(err != TM_OK, err)

	    /*                                                                               */
	    pDis->hotPlugStatus = (regVal & E_MASKREG_CEC_RXSHPDLEV_hpd_level) ?
	    HDMITX_HOTPLUG_ACTIVE : HDMITX_HOTPLUG_INACTIVE;

	/*                                                                       */
	pDis->rxSenseStatus = (regVal & E_MASKREG_CEC_RXSHPDLEV_rxs_level) ?
	    HDMITX_RX_SENSE_ACTIVE : HDMITX_RX_SENSE_INACTIVE;

	/*                            */
	err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, &EnableIntMask);
	EnableIntMask |= E_MASKREG_CEC_RXSHPDINTENA_ena_rxs_int;	/*                          */
	EnableIntMask |= E_MASKREG_CEC_RXSHPDINTENA_ena_hpd_int;	/*                      */

	/*                                                   */
	setState(pDis, EV_INIT);

	/*                              */
	err += setCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, EnableIntMask);
	err += getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &int_level);

	/*                                                                   */
	gMiscInterruptHpdRxEnable = True;

	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t tmbslTDA9989PowerGetState(tmUnitSelect_t txUnit, tmPowerState_t *pePowerState) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                            */
	    RETIF_BADPARAM(pePowerState == (tmPowerState_t) 0)

	    /*                 */
	    * pePowerState = (tmPowerState_t) pDis->ePowerState;

	return TM_OK;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989PowerSetState(tmUnitSelect_t txUnit, tmPowerState_t ePowerState) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 RegVal = 0;	/*                */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    if (ePowerState == tmPowerOff) {
		ePowerState = tmPowerStandby;
	}


	/*                            */
	RETIF_BADPARAM((ePowerState != tmPowerStandby)
		       && (ePowerState != tmPowerSuspend)
		       && (ePowerState != tmPowerOn)
	    )

	    if ((ePowerState == tmPowerStandby) && (pDis->ePowerState != tmPowerStandby)) {
		/*                                   */
		err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, &RegVal);
		RegVal &= ~E_MASKREG_CEC_RXSHPDINTENA_ena_hpd_int;
		RegVal &= ~E_MASKREG_CEC_RXSHPDINTENA_ena_rxs_int;
		err += setCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, RegVal);
		err += getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &int_level);
		RETIF_REG_FAIL(err)

		    /*                               */
		    if (pDis->ePowerState == tmPowerOn) {

			/*                               */
			err = setHwRegister(pDis, E_REG_P00_ENA_AP_RW, 0x00);	/*            */
			RETIF_REG_FAIL(err)


			    err = setHwRegister(pDis, E_REG_P00_ENA_VP_0_RW, 0x00);	/*            */
			RETIF_REG_FAIL(err)

			    err = setHwRegister(pDis, E_REG_P00_ENA_VP_1_RW, 0x00);	/*            */
			RETIF_REG_FAIL(err)

			    err = setHwRegister(pDis, E_REG_P00_ENA_VP_2_RW, 0x00);	/*            */
			RETIF_REG_FAIL(err)

			    /*             */
			    err = setHwRegisterField(pDis, E_REG_P00_DDC_DISABLE_RW, E_MASKREG_P00_DDC_DISABLE_ddc_dis, 1);	/*            */
			RETIF_REG_FAIL(err);

#ifdef TMFL_HDCP_OPTIMIZED_POWER
			/*                   */
			tmbslTDA9989HdcpPowerDown(txUnit, True);
			err = setHwRegisterField(pDis, E_REG_FEAT_POWER_DOWN,
						 E_MASKREG_FEAT_POWER_DOWN_all, 0x0F);
#endif
		}

		/*                                                   */
		err = getCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, &RegVal);
		RegVal &= ~E_MASKREG_CEC_ENAMODS_ena_hdmi;	/*                 */

		err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, RegVal);
		RETIF_REG_FAIL(err)

		    err = getCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, &RegVal);
		RETIF_REG_FAIL(err)
		    RegVal |= E_MASKREG_CEC_ENAMODS_ena_hdmi;	/*               */
		err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, RegVal);
		RETIF_REG_FAIL(err)

		    err = getCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, &RegVal);
		RegVal &= ~E_MASKREG_CEC_ENAMODS_ena_hdmi;	/*                 */

		RegVal &= ~E_MASKREG_CEC_ENAMODS_ena_rxs;	/*                    */


		/*             */
		RegVal |= E_MASKREG_CEC_ENAMODS_dis_fro;
		err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, RegVal);
		RETIF_REG_FAIL(err)


		    /*                                            */
		    setState(pDis, EV_STANDBY);
	} else if ((ePowerState == tmPowerSuspend) && (pDis->ePowerState != tmPowerSuspend)) {

		/*                               */
		if (pDis->ePowerState == tmPowerOn) {
			/*                               */
			err = setHwRegister(pDis, E_REG_P00_ENA_AP_RW, 0x00);	/*            */
			RETIF_REG_FAIL(err)


			    err = setHwRegister(pDis, E_REG_P00_ENA_VP_0_RW, 0x00);	/*            */
			RETIF_REG_FAIL(err)

			    err = setHwRegister(pDis, E_REG_P00_ENA_VP_1_RW, 0x00);	/*            */
			RETIF_REG_FAIL(err)

			    err = setHwRegister(pDis, E_REG_P00_ENA_VP_2_RW, 0x00);	/*            */
			RETIF_REG_FAIL(err);

			/*             */
			err = setHwRegisterField(pDis, E_REG_P00_DDC_DISABLE_RW, E_MASKREG_P00_DDC_DISABLE_ddc_dis, 1);	/*            */
			RETIF_REG_FAIL(err);

#ifdef TMFL_HDCP_OPTIMIZED_POWER
			/*                   */
			tmbslTDA9989HdcpPowerDown(txUnit, True);
			err = setHwRegisterField(pDis, E_REG_FEAT_POWER_DOWN,
						 E_MASKREG_FEAT_POWER_DOWN_all, 0x0F);
#endif
		}

		/*                                          */
		err = getCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, &RegVal);
		RegVal &= ~E_MASKREG_CEC_ENAMODS_ena_hdmi;	/*                 */
		RegVal |= E_MASKREG_CEC_ENAMODS_ena_rxs;	/*                  */
		err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, RegVal);
		RETIF_REG_FAIL(err)

		    /*                                                                                  */
		    /*                                                           */
		    if (pDis->ePowerState == tmPowerStandby) {
			/*                                   */
			err = getCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, &RegVal);
			RegVal &= ~E_MASKREG_CEC_ENAMODS_dis_fro;	/*             */
			err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, RegVal);
			RETIF_REG_FAIL(err)

			    /*                            */
			    err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, &RegVal);
			RegVal |= E_MASKREG_CEC_RXSHPDINTENA_ena_hpd_int;	/*                      */
			RegVal |= E_MASKREG_CEC_RXSHPDINTENA_ena_rxs_int;	/*                          */
			err += setCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, RegVal);
			err += getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &int_level);
			RETIF_REG_FAIL(err)

			    /*                                           */
			    err = tmbslTDA9989HwHandleInterrupt(txUnit);
			RETIF(err != TM_OK, err)


		}

		/*                                              */
		setState(pDis, EV_SLEEP);

	}

	else if ((ePowerState == tmPowerOn) && (pDis->ePowerState != tmPowerOn)) {

		/*                           */
		err = getCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, &RegVal);
		RegVal |= E_MASKREG_CEC_ENAMODS_ena_hdmi;	/*               */
		RegVal |= E_MASKREG_CEC_ENAMODS_ena_rxs;	/*                  */
		err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, RegVal);
		RETIF_REG_FAIL(err)

		    /*                                                                                  */
		    /*                                                           */
		    if (pDis->ePowerState == tmPowerStandby) {
			/*                                   */
			err = getCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, &RegVal);
			RegVal &= ~E_MASKREG_CEC_ENAMODS_dis_fro;	/*             */
			err = setCECHwRegister(pDis, E_REG_CEC_ENAMODS_RW, RegVal);
			RETIF_REG_FAIL(err)


			    /*                                                 */
			    err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, &RegVal);
			RegVal |= E_MASKREG_CEC_RXSHPDINTENA_ena_hpd_int;	/*                      */
			RegVal |= E_MASKREG_CEC_RXSHPDINTENA_ena_rxs_int;	/*                          */
			err += setCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, RegVal);
			err += getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &int_level);
			RETIF_REG_FAIL(err)
		}


		/*                   */
		RegVal = HDMI_TX_VSWING_VALUE;
		err = setHwRegister(pDis, E_REG_P02_ANA_GENERAL_RW, RegVal);
		RETIF(err != TM_OK, err)


		    err = tmbslTDA9989Reset(txUnit);
		RETIF(err != TM_OK, err)


		    err =
		    setHwRegisterField(pDis, E_REG_P00_VIP_CNTRL_4_W,
				       E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);

		err = setHwRegister(pDis, E_REG_P12_TX3_RW, TDA19989_DDC_SPEED_FACTOR);
		RETIF(err != TM_OK, err)

		    /*                  */
		    if (pDis->uDeviceVersion == E_DEV_VERSION_TDA19989) {

			err = setHwRegisterField(pDis,
						 E_REG_P00_I2C_MASTER_RW,
						 E_MASKREG_P00_I2C_MASTER_dis_mm, 0);
			RETIF_REG_FAIL(err)

		}
		/*                  */
		err =
		    setCECHwRegister(pDis, E_REG_CEC_FRO_IM_CLK_CTRL_RW,
				     E_MASKREG_CEC_FRO_IM_CLK_CTRL_ghost_dis |
				     E_MASKREG_CEC_FRO_IM_CLK_CTRL_imclk_sel);
		RETIF_REG_FAIL(err)
#ifdef TMFL_HDCP_SUPPORT
		    if (pDis->HdcpSeed) {
			err =
			    tmbslTDA9989HdcpDownloadKeys(txUnit, pDis->HdcpSeed,
							 HDMITX_HDCP_DECRYPT_ENABLE);
		}
#endif				/*                   */


		/*            */
		err = setHwRegisterField(pDis, E_REG_P00_DDC_DISABLE_RW, E_MASKREG_P00_DDC_DISABLE_ddc_dis, 0);	/*           */
		RETIF_REG_FAIL(err)

		    /*                              */
		    err = setHwRegister(pDis, E_REG_P00_ENA_AP_RW, 0xFF);	/*           */
		RETIF_REG_FAIL(err)


		    err = setHwRegister(pDis, E_REG_P00_ENA_VP_0_RW, 0xFF);	/*           */
		RETIF_REG_FAIL(err)

		    err = setHwRegister(pDis, E_REG_P00_ENA_VP_1_RW, 0xFF);	/*           */
		RETIF_REG_FAIL(err)

		    err = setHwRegister(pDis, E_REG_P00_ENA_VP_2_RW, 0xFF);	/*           */
		RETIF_REG_FAIL(err)


		    /*                                                                  */
		    if (pDis->hotPlugStatus == HDMITX_HOTPLUG_ACTIVE) {
			setState(pDis, EV_PLUGGEDIN);
		} else {
			setState(pDis, EV_UNPLUGGED);
		}

	}

	/*                                                                  */
	pDis->ePowerState = (tmbslHdmiTxPowerState_t) ePowerState;


	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989Reset(tmUnitSelect_t txUnit) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                            */
	    (void)setHwRegisterField(pDis, E_REG_P00_SR_REG_W,
				     E_MASKREG_P00_SR_REG_sr_i2c_ms |
				     E_MASKREG_P00_SR_REG_sr_audio, 1);

	pDis->sysFuncTimer(50);	/*    */

	(void)setHwRegisterField(pDis, E_REG_P00_SR_REG_W,
				 E_MASKREG_P00_SR_REG_sr_i2c_ms | E_MASKREG_P00_SR_REG_sr_audio, 0);

	pDis->sysFuncTimer(50);	/*    */

	/*                                                                   
                                     
  */
	(void)setHwRegisterField(pDis, E_REG_P00_MAIN_CNTRL0_RW, E_MASKREG_P00_MAIN_CNTRL0_sr, 1);
	/*                         *//*    */
	(void)setHwRegisterField(pDis, E_REG_P00_MAIN_CNTRL0_RW, E_MASKREG_P00_MAIN_CNTRL0_sr, 0);
	/*                         *//*    */
	/*                      */
	(void)setHwRegisterField(pDis, E_REG_P00_HVF_CNTRL_0_W, E_MASKREG_P00_HVF_CNTRL_0_sm, 0);

#ifdef TMFL_HDCP_SUPPORT
	/*                                                     */
	pDis->HdcpFuncRemainingMs = 0;
	pDis->HdcpCheckNum = 0;
#endif				/*                   */

	/*                                                   */
	setState(pDis, EV_DEINIT);
	/*                                        */
	/*                                     */
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989SwGetVersion(ptmSWVersion_t pSWVersion) {
	/*                  */
	RETIF_BADPARAM(pSWVersion == (ptmSWVersion_t) 0)

	    /*                                           */
	    pSWVersion->compatibilityNr = HDMITX_BSL_COMP_NUM;
	pSWVersion->majorVersionNr = HDMITX_BSL_MAJOR_VER;
	pSWVersion->minorVersionNr = HDMITX_BSL_MINOR_VER;

	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989SysTimerWait(tmUnitSelect_t txUnit, UInt16 waitMs) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                           */
	    RETIF(!pDis->sysFuncTimer, TMBSL_ERR_HDMI_NOT_INITIALIZED)

	    /*                             */
	    pDis->sysFuncTimer(waitMs);

	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t
    tmbslTDA9989TestSetMode
    (tmUnitSelect_t txUnit, tmbslHdmiTxTestMode_t testMode, tmbslHdmiTxTestState_t testState) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	/*                                  */
	UInt16 testReg = E_REG_P00_VIP_CNTRL_4_W;
	/*                             */
	UInt8 testMask = E_MASKREG_P00_VIP_CNTRL_4_tst_pat;

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(testMode >= HDMITX_TESTMODE_INVALID)
	    RETIF_BADPARAM(testState >= HDMITX_TESTSTATE_INVALID)

	    /*                                                                       */
	    switch (testMode) {
	case HDMITX_TESTMODE_PAT:
		testReg = E_REG_P00_VIP_CNTRL_4_W;
		testMask = E_MASKREG_P00_VIP_CNTRL_4_tst_pat;
		break;
	case HDMITX_TESTMODE_656:
		testReg = E_REG_P00_VIP_CNTRL_4_W;
		testMask = E_MASKREG_P00_VIP_CNTRL_4_tst_656;
		break;
	case HDMITX_TESTMODE_SERPHOE:
		/*                              
                                                */
		break;
	case HDMITX_TESTMODE_NOSC:
		testReg = E_REG_P02_TEST1_RW;
		testMask = E_MASKREG_P02_TEST1_tst_nosc;
		break;
	case HDMITX_TESTMODE_HVP:
		/*                              
                                             */
		break;
	case HDMITX_TESTMODE_PWD:
		/*                              
                                            */
		break;
	case HDMITX_TESTMODE_DIVOE:
		/*                              
                                               */
		break;
	case HDMITX_TESTMODE_INVALID:
		break;
	}
	err = setHwRegisterField(pDis, testReg, testMask, (UInt8) testState);
	return err;
}

/*                                                                            */
/* 
                                                                           
                                                                    
                                        

                                         
                                                       
                                                    

                            
                                     
                                                                       
                                               
                                                                       
               
                                                                    
          

                                                                              */
tmErrorCode_t tmbslTDA9989PktFillGamut
    (tmUnitSelect_t txUnit, tmbslHdmiTxPktGamut_t *pPkt, UInt8 bufSel) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */

	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM((bufSel != 0) && (bufSel != 1))

	    /*                      */
	    /*              */
	    if (bufSel == 0) {
		/*             */
		err = setHwRegisters(pDis, E_REG_P13_GMD_0_HB0_RW, &pPkt->HB[0], 3);
		RETIF(err != TM_OK, err)
		    /*              */
		    err = setHwRegisters(pDis, E_REG_P13_GMD_0_PB0_RW, &pPkt->PB[0], 28);
		RETIF(err != TM_OK, err)
	}

	/*              */
	else {
		/*             */
		err = setHwRegisters(pDis, E_REG_P13_GMD_1_HB0_RW, &pPkt->HB[0], 3);
		RETIF(err != TM_OK, err)
		    /*              */
		    err = setHwRegisters(pDis, E_REG_P13_GMD_1_PB0_RW, &pPkt->PB[0], 28);
		RETIF(err != TM_OK, err)
	}

	return err;
}

/*                                                                            */
/* 
                                                                              
                                                                     
                                                                      
                

                                         
                                                       
                                                          

                            
                                     
                                                                       
                                               
                                                                       
               
                                                                    
          

                                                                              */
tmErrorCode_t tmbslTDA9989PktSendGamut(tmUnitSelect_t txUnit, UInt8 bufSel, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 GMD_Ctrl_Val;	/*                  */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM((bufSel != 0) && (bufSel != 1))

	    /*           */
	    GMD_Ctrl_Val = 0x00;

	/*                              */
	if (bEnable) {
		/*              */
		if (bufSel == 0) {
			GMD_Ctrl_Val |= E_MASKREG_P13_GMD_CONTROL_enable;
			GMD_Ctrl_Val &= ~E_MASKREG_P13_GMD_CONTROL_buf_sel;
			err = setHwRegister(pDis, E_REG_P13_GMD_CONTROL_RW, GMD_Ctrl_Val);
			RETIF(err != TM_OK, err)
		}
		/*              */
		else {
			GMD_Ctrl_Val |= E_MASKREG_P13_GMD_CONTROL_enable;
			GMD_Ctrl_Val |= E_MASKREG_P13_GMD_CONTROL_buf_sel;
			err = setHwRegister(pDis, E_REG_P13_GMD_CONTROL_RW, GMD_Ctrl_Val);
			RETIF(err != TM_OK, err)
		}
	}
	/*                               */
	else {
		GMD_Ctrl_Val &= ~E_MASKREG_P13_GMD_CONTROL_enable;
		GMD_Ctrl_Val &= ~E_MASKREG_P13_GMD_CONTROL_buf_sel;
		err = setHwRegister(pDis, E_REG_P13_GMD_CONTROL_RW, GMD_Ctrl_Val);
		RETIF(err != TM_OK, err)
	}

	return err;
}




/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989EnableCallback
    (tmUnitSelect_t txUnit, tmbslHdmiTxCallbackInt_t callbackSource, Bool enable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err = TM_OK;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(callbackSource >= HDMITX_CALLBACK_INT_NUM)

	    switch (callbackSource) {
	case HDMITX_CALLBACK_INT_VS_RPT:
		/*                                */
		err = setHwRegisterField(pDis,
					 E_REG_P00_INT_FLAGS_1_RW,
					 E_MASKREG_P00_INT_FLAGS_1_vs_rpt, (UInt8) enable);
		if (enable) {
			pDis->InterruptsEnable |= (1 << callbackSource);
		} else {
			pDis->InterruptsEnable &= ~(1 << callbackSource);
		}
		break;
	default:
		err = TMBSL_ERR_HDMI_NOT_SUPPORTED;
		break;
	}

	return err;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989SetColorDepth
    (tmUnitSelect_t txUnit, tmbslHdmiTxColorDepth colorDepth, Bool termEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err = TM_OK;	/*            */

	DUMMY_ACCESS(termEnable);

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(colorDepth >= HDMITX_COLORDEPTH_INVALID)

	    switch (colorDepth) {
	case HDMITX_COLORDEPTH_NO_CHANGE:
		break;

	case HDMITX_COLORDEPTH_24:

		break;

	default:
		err = TMBSL_ERR_HDMI_NOT_SUPPORTED;
		break;
	}

	return err;

}


/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t tmbslTDA9989Set5vpower(tmUnitSelect_t txUnit, Bool pwrEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	DUMMY_ACCESS(pwrEnable);

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)


	    return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989SetDefaultPhase
    (tmUnitSelect_t txUnit, Bool bEnable, tmbslHdmiTxColorDepth colorDepth, UInt8 videoFormat) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	DUMMY_ACCESS(bEnable);
	DUMMY_ACCESS(colorDepth);
	DUMMY_ACCESS(videoFormat);

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}



/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t tmbslDebugWriteFakeRegPage(tmUnitSelect_t txUnit)
{
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	err = checkUnitSetDis(txUnit, &pDis);

	pDis->curRegPage = 0x20;

	return err;
}


/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t hotPlugRestore(tmUnitSelect_t txUnit)
{
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regVal;
	UInt8 EnableIntMask = 0;	/*                                                */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                         */
	    regVal = HDMI_TX_VSWING_VALUE;
	err = setHwRegister(pDis, E_REG_P02_ANA_GENERAL_RW, regVal);
	RETIF(err != TM_OK, err)

	    /*                                    */
	    err = setHwRegisterFieldTable(pDis, &kCommonPllCfg[0]);
	RETIF_REG_FAIL(err)

	    /*                                            */
	    err =
	    setHwRegisterField(pDis, E_REG_P00_VIP_CNTRL_4_W, E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);

	/*                                                             */
	err = setHwRegister(pDis, E_REG_P00_DDC_DISABLE_RW, 0x00);
	RETIF(err != TM_OK, err)

	    err = setHwRegister(pDis, E_REG_P12_TX3_RW, TDA19989_DDC_SPEED_FACTOR);
	RETIF(err != TM_OK, err)

	    /*                  */
	    if (pDis->uDeviceVersion == E_DEV_VERSION_TDA19989) {

		err = setHwRegisterField(pDis, E_REG_P00_I2C_MASTER_RW, E_MASKREG_P00_I2C_MASTER_dis_mm, 0);	/*                             */
		RETIF_REG_FAIL(err)

	}

	/*                  */
	err =
	    setCECHwRegister(pDis, E_REG_CEC_FRO_IM_CLK_CTRL_RW,
			     E_MASKREG_CEC_FRO_IM_CLK_CTRL_ghost_dis |
			     E_MASKREG_CEC_FRO_IM_CLK_CTRL_imclk_sel);
	RETIF_REG_FAIL(err)




	    /*                               */
	    err = setHwRegister(pDis, E_REG_P00_INT_FLAGS_1_RW, E_MASKREG_P00_INT_FLAGS_1_sw_int);

	/*                  */
	err = setHwRegister(pDis, E_REG_P00_INT_FLAGS_2_RW, E_MASKREG_P00_INT_FLAGS_2_edid_blk_rd);

	err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, &EnableIntMask);
	EnableIntMask |= E_MASKREG_CEC_RXSHPDINTENA_ena_rxs_int;	/*                          */
	EnableIntMask |= E_MASKREG_CEC_RXSHPDINTENA_ena_hpd_int;	/*                      */
	err += setCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, EnableIntMask);
	err += getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &int_level);

#ifdef TMFL_HDCP_SUPPORT

	if (pDis->HdcpSeed) {
		err =
		    tmbslTDA9989HdcpDownloadKeys(txUnit, pDis->HdcpSeed,
						 HDMITX_HDCP_DECRYPT_ENABLE);
	}
#endif				/*                   */

	setState(pDis, EV_INIT);

	return err;
}

#ifdef TMFL_TDA9989_PIXEL_CLOCK_ON_DDC

tmErrorCode_t hotPlugRestore(tmUnitSelect_t txUnit)
{
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regVal;
	UInt8 EnableIntMask = 0;	/*                                                */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                         */
	    regVal = HDMI_TX_VSWING_VALUE;
	err = setHwRegister(pDis, E_REG_P02_ANA_GENERAL_RW, regVal);
	RETIF(err != TM_OK, err)

	    /*                                    */
	    err = setHwRegisterFieldTable(pDis, &kCommonPllCfg[0]);
	RETIF_REG_FAIL(err)

	    /*                                            */
	    err =
	    setHwRegisterField(pDis, E_REG_P00_VIP_CNTRL_4_W, E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);

	/*                                                             */
	err = setHwRegister(pDis, E_REG_P00_DDC_DISABLE_RW, 0x00);
	RETIF(err != TM_OK, err)



	    if ((pDis->vinFmt != HDMITX_VFMT_NO_CHANGE) && (pDis->vinFmt <= HDMITX_VFMT_TV_MAX)) {

		err = setHwRegister(pDis, E_REG_P00_TIMER_H_W, 0);
		RETIF(err != TM_OK, err)

		    err = setHwRegister(pDis, E_REG_P00_NDIV_IM_W, kndiv_im[pDis->vinFmt]);
		RETIF(err != TM_OK, err)

		    err = setHwRegister(pDis, E_REG_P12_TX3_RW, kclk_div[pDis->vinFmt]);
		RETIF(err != TM_OK, err)

	} else if (pDis->vinFmt > HDMITX_VFMT_TV_MAX) {

		err = setHwRegister(pDis, E_REG_P00_TIMER_H_W, E_MASKREG_P00_TIMER_H_im_clksel);
		RETIF(err != TM_OK, err)
		    err = setHwRegister(pDis, E_REG_P12_TX3_RW, 17);
		RETIF(err != TM_OK, err)
	}





	/*                               */
	err = setHwRegister(pDis, E_REG_P00_INT_FLAGS_1_RW, E_MASKREG_P00_INT_FLAGS_1_sw_int);

	/*                  */
	err = setHwRegister(pDis, E_REG_P00_INT_FLAGS_2_RW, E_MASKREG_P00_INT_FLAGS_2_edid_blk_rd);

	err = getCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, &EnableIntMask);
	EnableIntMask |= E_MASKREG_CEC_RXSHPDINTENA_ena_rxs_int;	/*                          */
	EnableIntMask |= E_MASKREG_CEC_RXSHPDINTENA_ena_hpd_int;	/*                      */
	err += setCECHwRegister(pDis, E_REG_CEC_RXSHPDINTENA_RW, EnableIntMask);
	err += getCECHwRegister(pDis, E_REG_CEC_RXSHPDLEV_R, &int_level);

	return err;
}

#endif				/*                                 */

#ifdef TMFL_HDCP_OPTIMIZED_POWER
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpPowerDown(tmUnitSelect_t txUnit, Bool requested) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err);

	err = setHwRegisterField(pDis, E_REG_P12_TX4_RW,
				 E_MASKREG_P12_TX4_pd_all, (requested ? 0x07 : 0x00));
	return err;

}
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
