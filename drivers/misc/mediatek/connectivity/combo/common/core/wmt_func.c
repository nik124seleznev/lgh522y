/*       
                                            

                                                                                           
*/



/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#ifdef DFT_TAG
#undef DFT_TAG
#endif
#define DFT_TAG         "[WMT-FUNC]"


/*                                                                              
                                                          
                                                                                
*/
#include "osal_typedef.h"

#include "wmt_func.h"
#include "wmt_lib.h"
#include "wmt_core.h"



/*                                                                              
                                                
                                                                                
*/



/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/
#if CFG_FUNC_BT_SUPPORT

static INT32 wmt_func_bt_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);
static INT32 wmt_func_bt_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);

WMT_FUNC_OPS wmt_func_bt_ops = {
	/*                              */
	.func_on = wmt_func_bt_on,
	.func_off = wmt_func_bt_off
};
#endif

#if CFG_FUNC_FM_SUPPORT

static INT32 wmt_func_fm_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);
static INT32 wmt_func_fm_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);

WMT_FUNC_OPS wmt_func_fm_ops = {
	/*                              */
	.func_on = wmt_func_fm_on,
	.func_off = wmt_func_fm_off
};
#endif

#if CFG_FUNC_GPS_SUPPORT

static INT32 wmt_func_gps_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);
static INT32 wmt_func_gps_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);

WMT_FUNC_OPS wmt_func_gps_ops = {
	/*                               */
	.func_on = wmt_func_gps_on,
	.func_off = wmt_func_gps_off
};

#endif

#if CFG_FUNC_WIFI_SUPPORT
static INT32 wmt_func_wifi_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);
static INT32 wmt_func_wifi_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);

WMT_FUNC_OPS wmt_func_wifi_ops = {
	/*                                 */
	.func_on = wmt_func_wifi_on,
	.func_off = wmt_func_wifi_off
};
#endif


#if CFG_FUNC_ANT_SUPPORT

static INT32 wmt_func_ant_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);
static INT32 wmt_func_ant_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf);

WMT_FUNC_OPS wmt_func_ant_ops = {
	/*                              */
	.func_on = wmt_func_ant_on,
	.func_off = wmt_func_ant_off
};
#endif

/*                                                                              
                                                   
                                                                                
*/
#if CFG_FUNC_GPS_SUPPORT
CMB_PIN_CTRL_REG eediPinOhRegs[] = {
	{
	 /*                         */
	 .regAddr = 0x80050020,
	 .regValue = ~(0x1UL << 5),
	 .regMask = 0x00000020UL,
	 },
	{
	 /*                       */
	 .regAddr = 0x80050000,
	 .regValue = 0x1UL << 5,
	 .regMask = 0x00000020UL,
	 },
	{
	 /*                      */
	 .regAddr = 0x80050110,
	 .regValue = 0x1UL << 0,
	 .regMask = 0x00000007UL,
	 },
	{
	 /*                               */
	 .regAddr = 0x80050040,
	 .regValue = 0x1UL << 5,
	 .regMask = 0x00000020UL,
	 }

};

CMB_PIN_CTRL_REG eediPinOlRegs[] = {
	{
	 .regAddr = 0x80050020,
	 .regValue = 0x1UL << 5,
	 .regMask = 0x00000020UL,
	 },
	{
	 .regAddr = 0x80050000,
	 .regValue = ~(0x1UL << 5),
	 .regMask = 0x00000020UL,
	 },
	{
	 .regAddr = 0x80050110,
	 .regValue = 0x1UL << 0,
	 .regMask = 0x00000007UL,
	 },
	{
	 .regAddr = 0x80050040,
	 .regValue = ~(0x1UL << 5),
	 .regMask = 0x00000020UL,
	 }
};

CMB_PIN_CTRL_REG eedoPinOhRegs[] = {
	{
	 .regAddr = 0x80050020,
	 .regValue = ~(0x1UL << 7),
	 .regMask = 0x00000080UL,
	 },
	{
	 .regAddr = 0x80050000,
	 .regValue = 0x1UL << 7,
	 .regMask = 0x00000080UL,
	 },
	{
	 .regAddr = 0x80050110,
	 .regValue = 0x1UL << 12,
	 .regMask = 0x00007000UL,
	 },
	{
	 .regAddr = 0x80050040,
	 .regValue = 0x1UL << 7,
	 .regMask = 0x00000080UL,
	 }
};


CMB_PIN_CTRL_REG eedoPinOlRegs[] = {
	{
	 .regAddr = 0x80050020,
	 .regValue = 0x1UL << 7,
	 .regMask = 0x00000080UL,
	 },
	{
	 .regAddr = 0x80050000,
	 .regValue = ~(0x1UL << 7),
	 .regMask = 0x00000080UL,
	 },
	{
	 .regAddr = 0x80050110,
	 .regValue = 0x1UL << 12,
	 .regMask = 0x00007000UL,
	 },
	{
	 .regAddr = 0x80050040,
	 .regValue = ~(0x1UL << 7),
	 .regMask = 0x00000080UL,
	 }

};

CMB_PIN_CTRL_REG gsyncPinOnRegs[] = {
	{
	 .regAddr = 0x80050110,
	 .regValue = 0x3UL << 20,
	 .regMask = 0x7UL << 20,
	 }

};

CMB_PIN_CTRL_REG gsyncPinOffRegs[] = {
	{
	 .regAddr = 0x80050110,
	 .regValue = 0x0UL << 20,
	 .regMask = 0x7UL << 20,
	 }
};

/*                                 */
CMB_PIN_CTRL gCmbPinCtrl[3] = {
	{
	 .pinId = CMB_PIN_EEDI_ID,
	 .regNum = 4,
	 .pFuncOnArray = eediPinOhRegs,
	 .pFuncOffArray = eediPinOlRegs,
	 },
	{
	 .pinId = CMB_PIN_EEDO_ID,
	 .regNum = 4,
	 .pFuncOnArray = eedoPinOhRegs,
	 .pFuncOffArray = eedoPinOlRegs,
	 },
	{
	 .pinId = CMB_PIN_GSYNC_ID,
	 .regNum = 1,
	 .pFuncOnArray = gsyncPinOnRegs,
	 .pFuncOffArray = gsyncPinOffRegs,
	 }
};
#endif




/*                                                                              
                                                
                                                                                
*/

#if CFG_FUNC_BT_SUPPORT

INT32 _osal_inline_ wmt_func_bt_ctrl(ENUM_FUNC_STATE funcState)
{
	/*                                                */
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_BT,
				      (FUNC_ON ==
				       funcState) ? MTK_WCN_BOOL_TRUE : MTK_WCN_BOOL_FALSE);
}

INT32 wmt_func_bt_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	/*                                   */
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_BT, MTK_WCN_BOOL_TRUE);
}

INT32 wmt_func_bt_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	/*                                    */
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_BT, MTK_WCN_BOOL_FALSE);
}

#endif

#if CFG_FUNC_ANT_SUPPORT

INT32 _osal_inline_ wmt_func_ant_ctrl(ENUM_FUNC_STATE funcState)
{
	/*                                                */
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_ANT,
				      (FUNC_ON ==
				       funcState) ? MTK_WCN_BOOL_TRUE : MTK_WCN_BOOL_FALSE);
}

INT32 wmt_func_ant_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_ANT, MTK_WCN_BOOL_TRUE);
}

INT32 wmt_func_ant_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_ANT, MTK_WCN_BOOL_FALSE);
}

#endif


#if CFG_FUNC_GPS_SUPPORT

INT32 _osal_inline_ wmt_func_gps_ctrl(ENUM_FUNC_STATE funcState)
{
	/*                                    */
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_GPS,
				      (FUNC_ON ==
				       funcState) ? MTK_WCN_BOOL_TRUE : MTK_WCN_BOOL_FALSE);
}

INT32 wmt_func_gps_pre_ctrl(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf, ENUM_FUNC_STATE funcStatus)
{
	UINT32 i = 0;
	UINT32 iRet = 0;
	UINT32 regAddr = 0;
	UINT32 regValue = 0;
	UINT32 regMask = 0;
	UINT32 regNum = 0;
	P_CMB_PIN_CTRL_REG pReg;
	P_CMB_PIN_CTRL pCmbPinCtrl;
	WMT_CTRL_DATA ctrlData;
	WMT_IC_PIN_ID wmtIcPinId = WMT_IC_PIN_MAX;
	/*              */
	if (FUNC_ON != funcStatus && FUNC_OFF != funcStatus) {
		WMT_ERR_FUNC("invalid funcStatus(%d)\n", funcStatus);
		return -1;
	}
	/*                                        */
	ctrlData.ctrlId = WMT_CTRL_GPS_SYNC_SET;
	ctrlData.au4CtrlData[0] = (FUNC_ON == funcStatus) ? 1 : 0;
	iRet = wmt_ctrl(&ctrlData);
	if (iRet) {
		/*                                  */
		WMT_ERR_FUNC("ctrl GPS_SYNC_SET(%d) fail, ret(%d)\n", funcStatus, iRet);
		/*                                      */
		return -2;
	} else {
		WMT_INFO_FUNC("ctrl GPS_SYNC_SET(%d) ok\n", funcStatus);
	}
	if ((NULL == pOps->ic_pin_ctrl) || (0 > pOps->ic_pin_ctrl(WMT_IC_PIN_GSYNC, FUNC_ON == funcStatus ? WMT_IC_PIN_MUX : WMT_IC_PIN_GPIO, 1))) {	/*                 */
		pCmbPinCtrl = &gCmbPinCtrl[CMB_PIN_GSYNC_ID];
		regNum = pCmbPinCtrl->regNum;
		for (i = 0; i < regNum; i++) {
			pReg =
			    FUNC_ON ==
			    funcStatus ? &pCmbPinCtrl->pFuncOnArray[i] : &pCmbPinCtrl->
			    pFuncOffArray[i];
			regAddr = pReg->regAddr;
			regValue = pReg->regValue;
			regMask = pReg->regMask;

			iRet = wmt_core_reg_rw_raw(1, regAddr, &regValue, regMask);
			if (iRet) {
				WMT_ERR_FUNC("set reg for GPS_SYNC function fail(%d)\n", iRet);
				/*                                         */
				return -2;
			}

		}
	} else {
		WMT_INFO_FUNC("set reg for GPS_SYNC function okay by chip ic_pin_ctrl\n");
	}
	WMT_INFO_FUNC("ctrl combo chip gps sync function succeed\n");
	/*                               */
	if (NULL != pConf) {
		if (0 == pConf->wmt_gps_lna_enable) {

			WMT_INFO_FUNC("host pin used for gps lna\n");
			/*                          */
			ctrlData.ctrlId = WMT_CTRL_GPS_LNA_SET;
			ctrlData.au4CtrlData[0] = FUNC_ON == funcStatus ? 1 : 0;
			iRet = wmt_ctrl(&ctrlData);
			if (iRet) {
				/*                                  */
				WMT_ERR_FUNC("ctrl host GPS_LNA output high fail, ret(%d)\n", iRet);
				/*                                         */
				return -3;
			} else {
				WMT_INFO_FUNC("ctrl host gps lna function succeed\n");
			}
		} else {
			WMT_INFO_FUNC("combo chip pin(%s) used for gps lna\n",
				      0 == pConf->wmt_gps_lna_pin ? "EEDI" : "EEDO");
			wmtIcPinId =
			    0 == pConf->wmt_gps_lna_pin ? WMT_IC_PIN_EEDI : WMT_IC_PIN_EEDO;
			if ((NULL == pOps->ic_pin_ctrl) || (0 > pOps->ic_pin_ctrl(wmtIcPinId, FUNC_ON == funcStatus ? WMT_IC_PIN_GPIO_HIGH : WMT_IC_PIN_GPIO_LOW, 1))) {	/*                 */
				if (0 == pConf->wmt_gps_lna_pin) {
					/*             */
					pCmbPinCtrl = &gCmbPinCtrl[CMB_PIN_EEDI_ID];
				} else if (1 == pConf->wmt_gps_lna_pin) {
					/*             */
					pCmbPinCtrl = &gCmbPinCtrl[CMB_PIN_EEDO_ID];
				}
				regNum = pCmbPinCtrl->regNum;
				for (i = 0; i < regNum; i++) {
					pReg =
					    FUNC_ON ==
					    funcStatus ? &pCmbPinCtrl->
					    pFuncOnArray[i] : &pCmbPinCtrl->pFuncOffArray[i];
					regAddr = pReg->regAddr;
					regValue = pReg->regValue;
					regMask = pReg->regMask;

					iRet = wmt_core_reg_rw_raw(1, regAddr, &regValue, regMask);
					if (iRet) {
						WMT_ERR_FUNC
						    ("set reg for GPS_LNA function fail(%d)\n",
						     iRet);
						/*                                         */
						return -3;
					}
				}
				WMT_INFO_FUNC("ctrl combo chip gps lna succeed\n");
			} else {
				WMT_INFO_FUNC
				    ("set reg for GPS_LNA function okay by chip ic_pin_ctrl\n");
			}
		}
	}
	return 0;

}

INT32 wmt_func_gps_pre_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	return wmt_func_gps_pre_ctrl(pOps, pConf, FUNC_ON);
}

INT32 wmt_func_gps_pre_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{

	return wmt_func_gps_pre_ctrl(pOps, pConf, FUNC_OFF);
}


INT32 wmt_func_gps_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	INT32 iRet = 0;
	iRet = wmt_func_gps_pre_on(pOps, pConf);
	if (0 == iRet) {
		iRet = wmt_func_gps_ctrl(FUNC_ON);
	}
	return iRet;
}

INT32 wmt_func_gps_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	INT32 iRet = 0;
	iRet = wmt_func_gps_pre_off(pOps, pConf);
	if (0 == iRet) {
		iRet = wmt_func_gps_ctrl(FUNC_OFF);
	}
	return iRet;

}
#endif

#if CFG_FUNC_FM_SUPPORT

INT32 _osal_inline_ wmt_func_fm_ctrl(ENUM_FUNC_STATE funcState)
{
	/*                                                */
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_FM,
				      (FUNC_ON ==
				       funcState) ? MTK_WCN_BOOL_TRUE : MTK_WCN_BOOL_FALSE);
}


INT32 wmt_func_fm_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	/*                                   */
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_FM, MTK_WCN_BOOL_TRUE);
}

INT32 wmt_func_fm_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	/*                                    */
	return wmt_core_func_ctrl_cmd(WMTDRV_TYPE_FM, MTK_WCN_BOOL_FALSE);
}

#endif

#if CFG_FUNC_WIFI_SUPPORT

INT32 wmt_func_wifi_ctrl(ENUM_FUNC_STATE funcState)
{
	INT32 iRet = 0;
	UINT32 ctrlPa1 = WMT_SDIO_FUNC_WIFI;
	UINT32 ctrlPa2 = (FUNC_ON == funcState) ? 1 : 0;	/*                      */
	iRet = wmt_core_ctrl(WMT_CTRL_SDIO_FUNC, &ctrlPa1, &ctrlPa2);
	if (iRet) {
		WMT_ERR_FUNC("WMT-FUNC: turn on WIFI function fail (%d)", iRet);
		return -1;
	}
	return 0;
}


INT32 wmt_func_wifi_on(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	return wmt_func_wifi_ctrl(FUNC_ON);
}

INT32 wmt_func_wifi_off(P_WMT_IC_OPS pOps, P_WMT_GEN_CONF pConf)
{
	return wmt_func_wifi_ctrl(FUNC_OFF);
}
#endif
