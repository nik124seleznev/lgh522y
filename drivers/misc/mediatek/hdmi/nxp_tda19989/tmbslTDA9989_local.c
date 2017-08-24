/**
 * Copyright (C) 2009 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmbslTDA9989_local.c
 *
 * \version       $Revision: 2 $
 *
 *
*/

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                           */

/*                                                                            */
/*                                                                            */
/*                                                                            */


/*                                                                            */
/*                                                                            */
/*                                                                            */
#include "tmbslHdmiTx_types.h"
#include "tmbslTDA9989_Functions.h"
#include "tmbslTDA9989_local.h"
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


/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                   
                     
 */
#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
#ifdef USE_SHARED_HDMI_INSTANCE
extern tmHdmiTxobject_t gHdmiTxInstance[HDMITX_UNITS_MAX];
#else
#include <linux/kernel.h>
#include <linux/module.h>
tmHdmiTxobject_t gHdmiTxInstance[HDMITX_UNITS_MAX];
EXPORT_SYMBOL(gHdmiTxInstance);
#endif
#else
tmHdmiTxobject_t gHdmiTxInstance[HDMITX_UNITS_MAX];
#endif

/* 
                                                                
 */
UInt8 kPageIndexToPage[E_PAGE_NUM] = {
	0x00,			/*           */
	0x01,			/*           */
	0x02,			/*           */
	0x09,			/*           */
	0x10,			/*           */
	0x11,			/*           */
	0x12,			/*           */
	0x13			/*           */
};


/*                                                                            */
/*                                                                            */
/*                                                                            */

/* 
                                                               
                                                           
                                                              
                                                                 
                                 
                                                                           
 */
static UInt8 kMaskToShift[256] = {	/*             */
	/*                                                       */
	0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,	/*    */
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0	/*    */
};

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t checkUnitSetDis(tmUnitSelect_t txUnit, tmHdmiTxobject_t **ppDis) {
	/*                                            */
	RETIF(txUnit < tmUnit0, TMBSL_ERR_HDMI_BAD_UNIT_NUMBER)
	    RETIF(txUnit >= HDMITX_UNITS_MAX, TMBSL_ERR_HDMI_BAD_UNIT_NUMBER)

	    /*                                           */
	    * ppDis = &gHdmiTxInstance[txUnit];

	/*                                                   */
	if (!(*ppDis)->bInitialized) {
		return TMBSL_ERR_HDMI_NOT_INITIALIZED;
	}

	return TM_OK;
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(checkUnitSetDis);
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    getHwRegisters(tmHdmiTxobject_t *pDis, UInt16 regShadPageAddr, UInt8 *pData, UInt16 lenData) {
	tmErrorCode_t err;	/*            */
	UInt8 regShad;		/*                                         */
	UInt8 regPage;		/*                                         */
	UInt8 regAddr;		/*                                         */
	UInt8 newRegPage;	/*                                         */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                       */

	/*                                                            */
	regShad = SPA2SHAD(regShadPageAddr);
	regPage = SPA2PAGE(regShadPageAddr);
	regAddr = SPA2ADDR(regShadPageAddr);
	newRegPage = kPageIndexToPage[regPage];

	/*                                     */
	RETIF_BADPARAM((regAddr + lenData) > E_REG_CURPAGE_ADR_W)

	    /*                                                                     */
	    RETIF_BADPARAM(regShad != E_SNONE)

	    /*                               */
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

	/*                                                        */
	sysArgs.slaveAddr = pDis->uHwAddress;
	sysArgs.firstRegister = regAddr;
	sysArgs.lenData = (UInt8) lenData;
	sysArgs.pData = pData;
	err = pDis->sysFuncRead(&sysArgs);
	return (err == TM_OK) ? TM_OK : TMBSL_ERR_HDMI_I2C_READ;
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(getHwRegisters);
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t getHwRegister(tmHdmiTxobject_t *pDis, UInt16 regShadPageAddr, UInt8 *pRegValue) {
	tmErrorCode_t err;	/*            */
	UInt8 regShad;		/*                                         */
	UInt8 regPage;		/*                                         */
	UInt8 regAddr;		/*                                         */
	UInt8 newRegPage;	/*                                         */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                       */

	/*                                                      */
	regShad = SPA2SHAD(regShadPageAddr);
	regPage = SPA2PAGE(regShadPageAddr);
	regAddr = SPA2ADDR(regShadPageAddr);
	newRegPage = kPageIndexToPage[regPage];

	/*                               */
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

	if ((regShad != E_SNONE)
	    && (regShadPageAddr != E_REG_P00_INT_FLAGS_0_RW)
	    && (regShadPageAddr != E_REG_P00_INT_FLAGS_1_RW)
	    && (regShadPageAddr != E_REG_P00_INT_FLAGS_2_RW)) {
		/*                                                    */
		/*                                                   */
		*pRegValue = pDis->shadowReg[regShad];
		return TM_OK;
	} else {
		/*                                                  */
		sysArgs.slaveAddr = pDis->uHwAddress;
		sysArgs.firstRegister = regAddr;
		sysArgs.lenData = 1;
		sysArgs.pData = pRegValue;
		err = pDis->sysFuncRead(&sysArgs);
		return (err == TM_OK) ? TM_OK : TMBSL_ERR_HDMI_I2C_READ;
	}
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(getHwRegister);
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    setHwRegisters(tmHdmiTxobject_t *pDis, UInt16 regShadPageAddr, UInt8 *pData, UInt16 lenData) {
	tmErrorCode_t err;	/*            */
	UInt8 regShad;		/*                                         */
	UInt8 regPage;		/*                                         */
	UInt8 regAddr;		/*                                         */
	UInt8 newRegPage;	/*                                         */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                       */

	/*                                                            */
	regShad = SPA2SHAD(regShadPageAddr);
	regPage = SPA2PAGE(regShadPageAddr);
	regAddr = SPA2ADDR(regShadPageAddr);
	newRegPage = kPageIndexToPage[regPage];

	/*                                     */
	RETIF_BADPARAM((regAddr + lenData) > E_REG_CURPAGE_ADR_W)

	    /*                                                                     */
	    RETIF_BADPARAM(regShad != E_SNONE)

	    /*                                                             */
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

	/*                                                             */
	sysArgs.slaveAddr = pDis->uHwAddress;
	sysArgs.firstRegister = regAddr;
	sysArgs.lenData = (UInt8) lenData;
	sysArgs.pData = pData;
	err = pDis->sysFuncWrite(&sysArgs);
	return (err == TM_OK) ? TM_OK : TMBSL_ERR_HDMI_I2C_WRITE;
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(setHwRegisters);
#endif


/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t setHwRegisterMsbLsb(tmHdmiTxobject_t *pDis, UInt16 regShadPageAddr, UInt16 regWord) {
	tmErrorCode_t err;	/*            */
	UInt8 regPage;		/*                                         */
	UInt8 regAddr;		/*                                         */
	UInt8 newRegPage;	/*                                         */
	UInt8 msbLsb[2];	/*                                         */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                       */

	/*                                                      */
	regPage = SPA2PAGE(regShadPageAddr);
	regAddr = SPA2ADDR(regShadPageAddr);
	newRegPage = kPageIndexToPage[regPage];

	/*                                 */
	msbLsb[0] = (UInt8) (regWord >> 8);
	msbLsb[1] = (UInt8) (regWord & 0xFF);

	/*                               */
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

	/*                                */

	/*                                              */
	sysArgs.slaveAddr = pDis->uHwAddress;
	sysArgs.firstRegister = regAddr;
	sysArgs.lenData = 2;
	sysArgs.pData = &msbLsb[0];
	err = pDis->sysFuncWrite(&sysArgs);
	return (err == TM_OK) ? TM_OK : TMBSL_ERR_HDMI_I2C_WRITE;
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(setHwRegisterMsbLsb);
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t setHwRegister(tmHdmiTxobject_t *pDis, UInt16 regShadPageAddr, UInt8 regValue) {
	tmErrorCode_t err;	/*            */
	UInt8 regShad;		/*                                         */
	UInt8 regPage;		/*                                         */
	UInt8 regAddr;		/*                                         */
	UInt8 newRegPage;	/*                                         */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                       */

	/*                                                      */
	regShad = SPA2SHAD(regShadPageAddr);
	regPage = SPA2PAGE(regShadPageAddr);
	regAddr = SPA2ADDR(regShadPageAddr);
	newRegPage = kPageIndexToPage[regPage];

	/*                               */
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

	/*                 */
	if (regShad != E_SNONE) {
		pDis->shadowReg[regShad] = regValue;
	}

	/*                                              */
	sysArgs.slaveAddr = pDis->uHwAddress;
	sysArgs.firstRegister = regAddr;
	sysArgs.lenData = 1;
	sysArgs.pData = &regValue;
	err = pDis->sysFuncWrite(&sysArgs);
	return (err == TM_OK) ? TM_OK : TMBSL_ERR_HDMI_I2C_WRITE;
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(setHwRegister);
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    setHwRegisterField
    (tmHdmiTxobject_t *pDis, UInt16 regShadPageAddr, UInt8 fieldMask, UInt8 fieldValue) {
	tmErrorCode_t err;	/*            */
	UInt8 regShad;		/*                                         */
	UInt8 regPage;		/*                                         */
	UInt8 regAddr;		/*                                         */
	UInt8 newRegPage;	/*                                         */
	UInt8 regValue;		/*                                         */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                       */

	/*                                                      */
	regShad = SPA2SHAD(regShadPageAddr);
	regPage = SPA2PAGE(regShadPageAddr);
	regAddr = SPA2ADDR(regShadPageAddr);
	newRegPage = kPageIndexToPage[regPage];

	/*                               */
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

	if (regShad != E_SNONE) {
		/*                 */
		regValue = pDis->shadowReg[regShad];
	} else {
		/*                         
                                                               
   */
		sysArgs.slaveAddr = pDis->uHwAddress;
		sysArgs.firstRegister = regAddr;
		sysArgs.lenData = 1;
		sysArgs.pData = &regValue;
		err = pDis->sysFuncRead(&sysArgs);
		RETIF(err != TM_OK, TMBSL_ERR_HDMI_I2C_READ)
	}

	/*                                              */
	regValue = regValue & (UInt8) (~fieldMask);

	/*                                                            */
	fieldValue <<= kMaskToShift[fieldMask];

	/*                                                       */
	fieldValue &= fieldMask;

	/*                          */
	regValue |= fieldValue;

	/*                 */
	if (regShad != E_SNONE) {
		pDis->shadowReg[regShad] = regValue;
	}

	/*                                              */
	sysArgs.slaveAddr = pDis->uHwAddress;
	sysArgs.firstRegister = regAddr;
	sysArgs.lenData = 1;
	sysArgs.pData = &regValue;
	err = pDis->sysFuncWrite(&sysArgs);
	return (err == TM_OK) ? TM_OK : TMBSL_ERR_HDMI_I2C_WRITE;
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(setHwRegisterField);
#endif

/*                                                                            */
/*                                                                               */
/*                                                                            */
tmErrorCode_t getCECHwRegister(tmHdmiTxobject_t *pDis, UInt16 regAddr, UInt8 *pRegValue) {
	tmErrorCode_t err;	/*            */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                       */
	UInt8 CECAddress;	/*                                     */

	/*                                   */
	CECAddress = pDis->uHwAddress;
	CECAddress = CECAddress ^ 0x44;	/*                                                     */
	/*                                                          */
	/*                        */
	sysArgs.slaveAddr = CECAddress;
	sysArgs.firstRegister = (UInt8) regAddr;
	sysArgs.lenData = 1;
	sysArgs.pData = pRegValue;

	/*                  */
	err = pDis->sysFuncRead(&sysArgs);	/*                       */

	return (err == TM_OK) ? TM_OK : TMBSL_ERR_HDMI_I2C_READ;

}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(getCECHwRegister);
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t setCECHwRegister(tmHdmiTxobject_t *pDis, UInt16 regAddr, UInt8 regValue) {
	tmErrorCode_t err;	/*            */
	tmbslHdmiTxSysArgs_t sysArgs;	/*                                       */
	UInt8 CECAddress;	/*                                     */

	/*                                   */
	CECAddress = pDis->uHwAddress;
	CECAddress = CECAddress ^ 0x44;	/*                                                     */

	/*                        */
	sysArgs.slaveAddr = CECAddress;
	sysArgs.firstRegister = (UInt8) regAddr;
	sysArgs.lenData = 1;
	sysArgs.pData = &regValue;

	/*                   */
	err = pDis->sysFuncWrite(&sysArgs);	/*                        */

	return (err == TM_OK) ? TM_OK : TMBSL_ERR_HDMI_I2C_WRITE;
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(setCECHwRegister);
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t setHwRegisterFieldTable(tmHdmiTxobject_t *pDis, const tmHdmiTxRegMaskVal_t *pTable) {
	tmErrorCode_t err;	/*            */
	Int i;			/*             */

	/*                                                                     */
	for (i = 0; pTable[i].Reg > 0; i++) {
		err = setHwRegisterField(pDis, pTable[i].Reg, pTable[i].Mask, pTable[i].Val);
		RETIF(err != TM_OK, err)
	}
	return TM_OK;
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(setHwRegisterFieldTable);
#endif


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t lmemcpy(void *pTable1, const void *pTable2, UInt Size) {
	char *ptrSource = (char *)pTable1;
	char *endSource = (char *)pTable1 + Size;
	char *ptrDest = (char *)pTable2;

	RETIF_BADPARAM(pTable1 == Null)
	    RETIF_BADPARAM(pTable2 == Null)

	    while (endSource > ptrSource) {
		*(ptrSource++) = *(ptrDest++);
	}
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t lmemset(void *pTable1, const UInt8 value, UInt Size) {
	char *ptrSource = (char *)pTable1;
	char *endSource = (char *)pTable1 + Size;

	RETIF_BADPARAM(pTable1 == Null)

	    while (endSource > ptrSource) {
		*(ptrSource++) = value;
	}
	return TM_OK;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */
