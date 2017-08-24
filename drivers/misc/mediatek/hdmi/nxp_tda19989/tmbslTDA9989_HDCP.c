/**
 * Copyright (C) 2009 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmbslTDA9989_HDCP.c
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
#include "tmbslTDA9989_InOut_l.h"
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

/* 
                                                          
 */

tmHdmiTxRegMaskVal_t kVoutHdcpOff[] = {
	{E_REG_P00_TBG_CNTRL_1_W, E_MASKREG_P00_TBG_CNTRL_1_dwin_dis, 1},
	{E_REG_P12_TX33_RW, E_MASKREG_P12_TX33_hdmi, 0},
	{0, 0, 0}
};

/* 
                                                          
 */
tmHdmiTxRegMaskVal_t kVoutHdcpOn[] = {
	{E_REG_P00_TBG_CNTRL_1_W, E_MASKREG_P00_TBG_CNTRL_1_dwin_dis, 0},
	{E_REG_P11_ENC_CNTRL_RW, E_MASKREG_P11_ENC_CNTRL_ctl_code, 1},
	{E_REG_P12_TX33_RW, E_MASKREG_P12_TX33_hdmi, 1},
	{0, 0, 0}
};

#ifdef __LINUX_ARM_ARCH__

#include <linux/kernel.h>

typedef struct {
	tmErrorCode_t(*tmbslTDA9989HdcpCheck)
	(tmUnitSelect_t txUnit, UInt16 uTimeSinceLastCallMs, tmbslHdmiTxHdcpCheck_t *pResult);
	tmErrorCode_t(*tmbslTDA9989HdcpConfigure)
	(tmUnitSelect_t txUnit,
	 UInt8 slaveAddress,
	 tmbslHdmiTxHdcpTxMode_t txMode,
	 tmbslHdmiTxHdcpOptions_t options, UInt16 uCheckIntervalMs, UInt8 uChecksToDo);
	tmErrorCode_t(*tmbslTDA9989HdcpDownloadKeys)
	(tmUnitSelect_t txUnit, UInt16 seed, tmbslHdmiTxDecrypt_t keyDecryption);
	tmErrorCode_t(*tmbslTDA9989HdcpEncryptionOn)
	(tmUnitSelect_t txUnit, Bool bOn);
	tmErrorCode_t(*tmbslTDA9989HdcpGetOtp)
	(tmUnitSelect_t txUnit, UInt8 otpAddress, UInt8 *pOtpData);
	tmErrorCode_t(*tmbslTDA9989HdcpGetT0FailState)
	(tmUnitSelect_t txUnit, UInt8 *pFailState);
	tmErrorCode_t(*tmbslTDA9989HdcpHandleBCAPS)
	(tmUnitSelect_t txUnit);
	tmErrorCode_t(*tmbslTDA9989HdcpHandleBKSV)
	(tmUnitSelect_t txUnit, UInt8 *pBksv, Bool *pbCheckRequired	/*                                   */
	    );
	tmErrorCode_t(*tmbslTDA9989HdcpHandleBKSVResult)
	(tmUnitSelect_t txUnit, Bool bSecure);
	tmErrorCode_t(*tmbslTDA9989HdcpHandleBSTATUS)
	(tmUnitSelect_t txUnit, UInt16 *pBstatus	/*             */
	    );
	tmErrorCode_t(*tmbslTDA9989HdcpHandleENCRYPT)
	(tmUnitSelect_t txUnit);
	tmErrorCode_t(*tmbslTDA9989HdcpHandlePJ)
	(tmUnitSelect_t txUnit);
	tmErrorCode_t(*tmbslTDA9989HdcpHandleSHA_1)
	(tmUnitSelect_t txUnit, UInt8 maxKsvDevices, UInt8 *pKsvList,	/*                                   */
	 UInt8 *pnKsvDevices,	/*                                   */
	 UInt8 *pDepth		/*                                              */
	    );
	tmErrorCode_t(*tmbslTDA9989HdcpHandleSHA_1Result)
	(tmUnitSelect_t txUnit, Bool bSecure);
	tmErrorCode_t(*tmbslTDA9989HdcpHandleT0)
	(tmUnitSelect_t txUnit);
	tmErrorCode_t(*tmbslTDA9989HdcpInit)
	(tmUnitSelect_t txUnit, tmbslHdmiTxVidFmt_t voutFmt, tmbslHdmiTxVfreq_t voutFreq);
	tmErrorCode_t(*tmbslTDA9989HdcpRun)
	(tmUnitSelect_t txUnit);
	tmErrorCode_t(*tmbslTDA9989HdcpStop)
	(tmUnitSelect_t txUnit);
	tmErrorCode_t(*tmbslTDA9989HdcpGetSinkCategory)
	(tmUnitSelect_t txUnit, tmbslHdmiTxSinkCategory_t *category);
	tmErrorCode_t(*tmbslTDA9989handleBKSVResultSecure)
	(tmUnitSelect_t txUnit);
	tmErrorCode_t(*f1) (tmHdmiTxobject_t *pDis);
	int (*f2) (tmHdmiTxobject_t *pDis);
} hdcp_private_t;

#include <linux/module.h>	/*                        */

hdcp_private_t *h;

void register_hdcp_private(hdcp_private_t *hdcp)
{
	h = hdcp;
}
EXPORT_SYMBOL(register_hdcp_private);

tmErrorCode_t rej_f1(tmHdmiTxobject_t *pDis)
{
	return (h ? h->f1(pDis) : 0);
}

int rej_f2(tmHdmiTxobject_t *pDis)
{
	return (h ? h->f2(pDis) : 0);
}

tmErrorCode_t rej_f3(tmUnitSelect_t txUnit)
{
	return (h ? h->tmbslTDA9989handleBKSVResultSecure(txUnit) : TM_OK);
}

#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989HdcpCheck
    (tmUnitSelect_t txUnit, UInt16 uTimeSinceLastCallMs, tmbslHdmiTxHdcpCheck_t *pResult) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpCheck(txUnit, uTimeSinceLastCallMs, pResult);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989HdcpConfigure
    (tmUnitSelect_t txUnit,
     UInt8 slaveAddress,
     tmbslHdmiTxHdcpTxMode_t txMode,
     tmbslHdmiTxHdcpOptions_t options, UInt16 uCheckIntervalMs, UInt8 uChecksToDo) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpConfigure
		    (txUnit, slaveAddress, txMode, options, uCheckIntervalMs, uChecksToDo);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989HdcpDownloadKeys
    (tmUnitSelect_t txUnit, UInt16 seed, tmbslHdmiTxDecrypt_t keyDecryption) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpDownloadKeys(txUnit, seed, keyDecryption);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpEncryptionOn(tmUnitSelect_t txUnit, Bool bOn) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpEncryptionOn(txUnit, bOn);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpGetOtp(tmUnitSelect_t txUnit, UInt8 otpAddress, UInt8 *pOtpData) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpGetOtp(txUnit, otpAddress, pOtpData);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpGetT0FailState(tmUnitSelect_t txUnit, UInt8 *pFailState) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpGetT0FailState(txUnit, pFailState);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpHandleBCAPS(tmUnitSelect_t txUnit) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpHandleBCAPS(txUnit);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpHandleBKSV(tmUnitSelect_t txUnit, UInt8 *pBksv, Bool *pbCheckRequired	/*                                   */
    ) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpHandleBKSV(txUnit, pBksv, pbCheckRequired	/*                                   */
		    );
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpHandleBKSVResult(tmUnitSelect_t txUnit, Bool bSecure) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpHandleBKSVResult(txUnit, bSecure);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpHandleBSTATUS(tmUnitSelect_t txUnit, UInt16 *pBstatus	/*             */
    ) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpHandleBSTATUS(txUnit, pBstatus	/*             */
		    );
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpHandleENCRYPT(tmUnitSelect_t txUnit) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpHandleENCRYPT(txUnit);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpHandlePJ(tmUnitSelect_t txUnit) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpHandlePJ(txUnit);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpHandleSHA_1(tmUnitSelect_t txUnit, UInt8 maxKsvDevices, UInt8 *pKsvList,	/*                                   */
					  UInt8 *pnKsvDevices,	/*                                   */
					  UInt8 *pDepth	/*                                              */
    ) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpHandleSHA_1(txUnit, maxKsvDevices, pKsvList,	/*                                   */
						      pnKsvDevices,	/*                                   */
						      pDepth	/*                                              */
		    );
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpHandleSHA_1Result(tmUnitSelect_t txUnit, Bool bSecure) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpHandleSHA_1Result(txUnit, bSecure);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpHandleT0(tmUnitSelect_t txUnit) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpHandleT0(txUnit);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989HdcpInit
    (tmUnitSelect_t txUnit, tmbslHdmiTxVidFmt_t voutFmt, tmbslHdmiTxVfreq_t voutFreq) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpInit(txUnit, voutFmt, voutFreq);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpRun(tmUnitSelect_t txUnit) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpRun(txUnit);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989HdcpStop(tmUnitSelect_t txUnit) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpStop(txUnit);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989HdcpGetSinkCategory(tmUnitSelect_t txUnit, tmbslHdmiTxSinkCategory_t *category) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989HdcpGetSinkCategory(txUnit, category);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}




/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t tmbslTDA9989handleBKSVResultSecure(tmUnitSelect_t txUnit) {
#ifdef __LINUX_ARM_ARCH__
	if (h)
		return h->tmbslTDA9989handleBKSVResultSecure(txUnit);
/*                                             */
#endif
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
