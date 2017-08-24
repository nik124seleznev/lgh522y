/*
                                                                                         
*/

/*                      
                                                    
*/



/*
                         
  
                        
       
                       
  
                        
       
                           
                                                      
  
                   
                                                                                                                                                                   
                                                     
  
                      
       
                                                               
  
                   
                                                                                        
                                                                              
  
                   
                                                                                                     
                                                                        
  
                   
                                                                                                     
                                                                                   
  
                   
                                                                                                 
                                                         
                                                                
  
                         
                                                      
              
  
                           
                                                                                                             
    
  
                         
                                                      
                          
  
                         
                                                                                                                    
                                  
  
                           
                                                                                
    
  
                      
                                                                           
                                                  
  
                      
                                                                                    
    
  
                   
                                                                        
                                                                
  
                        
                                                       
                    
  
                        
                                                       
                                       
  
                      
                                                                                          
                                                                       
  
                      
                                                                                          
                                                          
  
                          
                                                               
                                                        
  
                   
                                                                                        
                                          
  
                   
                                                                                                                                                              
                                                  
  
                   
                                                                                 
                                 
  
                   
                                                                                 
                                     
  
                   
       
                                                                                     
  
                   
       
                                                   
  
                   
  
                                                                               
  
                         
                                                
                                   
  
                   
                                                          
                                                         
  
                           
                                             
                               
  
                           
                                             
                                
                                                              
                                                                                                                                         
                                                              
                           
                                                              
                       
                                                              
                       
                                                              
                                           
                                                              
                                                                                                           
                                                              
                                           
                                     
                                 
                                                              
                                                       
                                                              
                                      
                                                              
                                                     
                                                              
                           
                                                              
                                                                   
                                                              
                      
                                                              
                        
                                                              
                                                                   
                                                              
                               
                                                              
                                       
                                                              
                                            
                                                              
                            
                                                              
                                           
                                                              
                  
                                                              
                                               
                                                              
                                            
                                                             
                                   
                                                             
                                           
                                                             
                                         
                                                             
                    
                                                             
                
                                                             
                                                         
*/

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "gl_os.h"
#include "gl_wext_priv.h"
#if CFG_SUPPORT_WAPI
#include "gl_sec.h"
#endif
#if CFG_ENABLE_WIFI_DIRECT
#include "gl_p2p_os.h"
#endif

/*                                                                              
                                                
                                                                                
*/
#define NUM_SUPPORTED_OIDS      (sizeof(arWlanOidReqTable) / sizeof(WLAN_REQ_ENTRY))

/*                                                                              
                                                            
                                                                                
*/

static int
priv_get_ndis(IN struct net_device *prNetDev,
	      IN NDIS_TRANSPORT_STRUCT * prNdisReq, OUT PUINT_32 pu4OutputLen);

static int
priv_set_ndis(IN struct net_device *prNetDev,
	      IN NDIS_TRANSPORT_STRUCT * prNdisReq, OUT PUINT_32 pu4OutputLen);

#if 0				/*                 */
static int
priv_set_appie(IN struct net_device *prNetDev,
	       IN struct iw_request_info *prIwReqInfo,
	       IN union iwreq_data *prIwReqData, OUT char *pcExtra);

static int
priv_set_filter(IN struct net_device *prNetDev,
		IN struct iw_request_info *prIwReqInfo,
		IN union iwreq_data *prIwReqData, OUT char *pcExtra);
#endif				/*                 */

static BOOLEAN reqSearchSupportedOidEntry(IN UINT_32 rOid, OUT P_WLAN_REQ_ENTRY * ppWlanReqEntry);

#if 0
static WLAN_STATUS
reqExtQueryConfiguration(IN P_GLUE_INFO_T prGlueInfo,
			 OUT PVOID pvQueryBuffer,
			 IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen);

static WLAN_STATUS
reqExtSetConfiguration(IN P_GLUE_INFO_T prGlueInfo,
		       IN PVOID pvSetBuffer, IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen);
#endif

static WLAN_STATUS
reqExtSetAcpiDevicePowerState(IN P_GLUE_INFO_T prGlueInfo,
			      IN PVOID pvSetBuffer,
			      IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen);

/*                                                                              
                                               
                                                                                
*/
static UINT_8 aucOidBuf[4096] = { 0 };

/*                      */
/*                                                               
                                          */
static WLAN_REQ_ENTRY arWlanOidReqTable[] = {
	/*
                    
                        
                                                                                      
                      
                    
  */
	/*                                     */

	/*                                      */
	{OID_802_3_CURRENT_ADDRESS,
	 DISP_STRING("OID_802_3_CURRENT_ADDRESS"),
	 TRUE, TRUE, ENUM_OID_DRIVER_CORE, 6,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQueryCurrentAddr,
	 NULL},

	/*                          */
	/*                             */
	/*                     */

	/*                               */
	{OID_802_11_SUPPORTED_RATES,
	 DISP_STRING("OID_802_11_SUPPORTED_RATES"),
	 TRUE, FALSE, ENUM_OID_DRIVER_CORE, sizeof(PARAM_RATES_EX),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQuerySupportedRates,
	 NULL}
	,
	/*
                              
                                            
                                                                       
                                                       
                                                      
  */
	{OID_PNP_SET_POWER,
	 DISP_STRING("OID_PNP_SET_POWER"),
	 TRUE, FALSE, ENUM_OID_GLUE_EXTENSION, sizeof(PARAM_DEVICE_POWER_STATE),
	 NULL,
	 (PFN_OID_HANDLER_FUNC_REQ) reqExtSetAcpiDevicePowerState}
	,

	/*             */
	{OID_CUSTOM_OID_INTERFACE_VERSION,
	 DISP_STRING("OID_CUSTOM_OID_INTERFACE_VERSION"),
	 TRUE, FALSE, ENUM_OID_DRIVER_CORE, 4,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQueryOidInterfaceVersion,
	 NULL}
	,

	/*
                   
                                
                                              
                                                                         
         
                                                       
          
  */

	/*
                                         
                                                       
                                          
                                                           
                                                          
                               
                                             
                                                                                         
         
                                                       
                                      
                                                    
                                          
         
                                                     
                           
                                         
                                        
                                                               
                                                              
                       
                                     
                                        
         
                                            
                       
                                     
                                        
         
                                            
  */

	/*
                                 
                    
                                  
                                          
                                                    
                                                 
          

                           
                                         
                                        
                                                          
                                                         
                            
                                          
                                        
         
                                                       
                               
                                             
                                          
                                                         
                                                        

                           
                                        
                                                      
                                          
                                                         
                                                        
          

                           
                                         
                                                                       
         
                                                   
                                
                                              
                                                                       
                                                                 
                                                               
                                         
                                                       
                                         
                                                                     
                                                                    
  */

	/*     */
	/*
                                      
                                                    
                                         
         
                                                             
  */

	{OID_CUSTOM_MCR_RW,
	 DISP_STRING("OID_CUSTOM_MCR_RW"),
	 TRUE, TRUE, ENUM_OID_DRIVER_CORE, sizeof(PARAM_CUSTOM_MCR_RW_STRUC_T),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQueryMcrRead,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidSetMcrWrite}
	,

	{OID_CUSTOM_EEPROM_RW,
	 DISP_STRING("OID_CUSTOM_EEPROM_RW"),
	 TRUE, TRUE, ENUM_OID_DRIVER_CORE, sizeof(PARAM_CUSTOM_EEPROM_RW_STRUC_T),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQueryEepromRead,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidSetEepromWrite}
	,

	{OID_CUSTOM_SW_CTRL,
	 DISP_STRING("OID_CUSTOM_SW_CTRL"),
	 TRUE, TRUE, ENUM_OID_DRIVER_CORE, sizeof(PARAM_CUSTOM_SW_CTRL_STRUC_T),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQuerySwCtrlRead,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidSetSwCtrlWrite}
	,

	{OID_CUSTOM_MEM_DUMP,
	 DISP_STRING("OID_CUSTOM_MEM_DUMP"),
	 TRUE, TRUE, ENUM_OID_DRIVER_CORE, sizeof(PARAM_CUSTOM_MEM_DUMP_STRUC_T),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQueryMemDump,
	 NULL}
	,

	{OID_CUSTOM_TEST_MODE,
	 DISP_STRING("OID_CUSTOM_TEST_MODE"),
	 FALSE, FALSE, ENUM_OID_DRIVER_CORE, 0,
	 NULL,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidRftestSetTestMode}
	,

	/*
                               
                                             
                                                                                     
                                                         
          
                               
                                             
                                                                                     
                                                         
          
  */
	{OID_CUSTOM_ABORT_TEST_MODE,
	 DISP_STRING("OID_CUSTOM_ABORT_TEST_MODE"),
	 FALSE, FALSE, ENUM_OID_DRIVER_CORE, 0,
	 NULL,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidRftestSetAbortTestMode}
	,
	{OID_CUSTOM_MTK_WIFI_TEST,
	 DISP_STRING("OID_CUSTOM_MTK_WIFI_TEST"),
	 TRUE, TRUE, ENUM_OID_DRIVER_CORE, sizeof(PARAM_MTK_WIFI_TEST_STRUC_T),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidRftestQueryAutoTest,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidRftestSetAutoTest}
	,

	/*                                      */

	/*      */
#if CFG_SUPPORT_BCM && CFG_SUPPORT_BCM_BWCS
	{OID_CUSTOM_BWCS_CMD,
	 DISP_STRING("OID_CUSTOM_BWCS_CMD"),
	 FALSE, FALSE, ENUM_OID_DRIVER_CORE, sizeof(PTA_IPC_T),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQueryBT,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidSetBT}
	,
#endif

/*                               
                                          
                                       
                                                       
                                                      
                        
                                   
                                       
                                           
                                          
    */

	{OID_CUSTOM_MTK_NVRAM_RW,
	 DISP_STRING("OID_CUSTOM_MTK_NVRAM_RW"),
	 TRUE, TRUE, ENUM_OID_DRIVER_CORE, sizeof(PARAM_CUSTOM_NVRAM_RW_STRUCT_T),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQueryNvramRead,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidSetNvramWrite}
	,

	{OID_CUSTOM_CFG_SRC_TYPE,
	 DISP_STRING("OID_CUSTOM_CFG_SRC_TYPE"),
	 FALSE, FALSE, ENUM_OID_DRIVER_CORE, sizeof(ENUM_CFG_SRC_TYPE_T),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQueryCfgSrcType,
	 NULL}
	,

	{OID_CUSTOM_EEPROM_TYPE,
	 DISP_STRING("OID_CUSTOM_EEPROM_TYPE"),
	 FALSE, FALSE, ENUM_OID_DRIVER_CORE, sizeof(ENUM_EEPROM_TYPE_T),
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidQueryEepromType,
	 NULL}
	,

#if CFG_SUPPORT_WAPI
	{OID_802_11_WAPI_MODE,
	 DISP_STRING("OID_802_11_WAPI_MODE"),
	 FALSE, TRUE, ENUM_OID_DRIVER_CORE, 4,
	 NULL,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidSetWapiMode}
	,
	{OID_802_11_WAPI_ASSOC_INFO,
	 DISP_STRING("OID_802_11_WAPI_ASSOC_INFO"),
	 FALSE, FALSE, ENUM_OID_DRIVER_CORE, 0,
	 NULL,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidSetWapiAssocInfo}
	,
	{OID_802_11_SET_WAPI_KEY,
	 DISP_STRING("OID_802_11_SET_WAPI_KEY"),
	 FALSE, FALSE, ENUM_OID_DRIVER_CORE, sizeof(PARAM_WPI_KEY_T),
	 NULL,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidSetWapiKey}
	,
#endif

#if CFG_SUPPORT_WPS2
	{OID_802_11_WSC_ASSOC_INFO,
	 DISP_STRING("OID_802_11_WSC_ASSOC_INFO"),
	 FALSE, FALSE, ENUM_OID_DRIVER_CORE, 0,
	 NULL,
	 (PFN_OID_HANDLER_FUNC_REQ) wlanoidSetWSCAssocInfo}
	,
#endif
};

/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
                                                                         
                    
 
                                           
                                                
                                                                         
 
                        
                                              
                           
 
*/
/*                                                                            */
int priv_support_ioctl(IN struct net_device *prNetDev, IN OUT struct ifreq *prIfReq, IN int i4Cmd)
{
	/*                                                          */
	struct iwreq *prIwReq = (struct iwreq *)prIfReq;
	struct iw_request_info rIwReqInfo;

	/*                                                        */

	/*                  */
	rIwReqInfo.cmd = (__u16) i4Cmd;
	rIwReqInfo.flags = 0;

	switch (i4Cmd) {
	case IOCTL_SET_INT:
		/*                                                                             */
		return priv_set_int(prNetDev, &rIwReqInfo, &(prIwReq->u), (char *)&(prIwReq->u));

	case IOCTL_GET_INT:
		/*                                                                             */
		return priv_get_int(prNetDev, &rIwReqInfo, &(prIwReq->u), (char *)&(prIwReq->u));

	case IOCTL_SET_STRUCT:
	case IOCTL_SET_STRUCT_FOR_EM:
		return priv_set_struct(prNetDev, &rIwReqInfo, &prIwReq->u, (char *)&(prIwReq->u));

	case IOCTL_GET_STRUCT:
		return priv_get_struct(prNetDev, &rIwReqInfo, &prIwReq->u, (char *)&(prIwReq->u));

	default:
		return -EOPNOTSUPP;

	}			/*               */

}				/*                    */


/*                                                                            */
/* 
                                       
 
                                           
                                                    
                                                                                
                                                
 
                        
                                              
                                             
 
*/
/*                                                                            */
int
priv_set_int(IN struct net_device *prNetDev,
	     IN struct iw_request_info *prIwReqInfo,
	     IN union iwreq_data *prIwReqData, IN char *pcExtra)
{
	UINT_32 u4SubCmd;
	PUINT_32 pu4IntBuf;
	P_NDIS_TRANSPORT_STRUCT prNdisReq;
	P_GLUE_INFO_T prGlueInfo;
	UINT_32 u4BufLen = 0;
	int status = 0;
	P_PTA_IPC_T prPtaIpc;

	ASSERT(prNetDev);
	ASSERT(prIwReqInfo);
	ASSERT(prIwReqData);
	ASSERT(pcExtra);

	if (FALSE == GLUE_CHK_PR3(prNetDev, prIwReqData, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	u4SubCmd = (UINT_32) prIwReqData->mode;
	pu4IntBuf = (PUINT_32) pcExtra;

	switch (u4SubCmd) {
	case PRIV_CMD_TEST_MODE:
		/*                                         */
		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

		if (pu4IntBuf[1] == PRIV_CMD_TEST_MAGIC_KEY) {
			prNdisReq->ndisOidCmd = OID_CUSTOM_TEST_MODE;
		} else if (pu4IntBuf[1] == 0) {
			prNdisReq->ndisOidCmd = OID_CUSTOM_ABORT_TEST_MODE;
		} else {
			status = 0;
			break;
		}
		prNdisReq->inNdisOidlength = 0;
		prNdisReq->outNdisOidLength = 0;

		/*                  */
		status = priv_set_ndis(prNetDev, prNdisReq, &u4BufLen);
		break;

	case PRIV_CMD_TEST_CMD:
		/*                                                                    */
		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

		kalMemCopy(&prNdisReq->ndisOidContent[0], &pu4IntBuf[1], 8);

		prNdisReq->ndisOidCmd = OID_CUSTOM_MTK_WIFI_TEST;
		prNdisReq->inNdisOidlength = 8;
		prNdisReq->outNdisOidLength = 8;

		/*                  */
		status = priv_set_ndis(prNetDev, prNdisReq, &u4BufLen);
		break;

#if CFG_SUPPORT_PRIV_MCR_RW
	case PRIV_CMD_ACCESS_MCR:
		/*                                                                     */
		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

		if (!prGlueInfo->fgMcrAccessAllowed) {
			if (pu4IntBuf[1] == PRIV_CMD_TEST_MAGIC_KEY &&
			    pu4IntBuf[2] == PRIV_CMD_TEST_MAGIC_KEY) {
				prGlueInfo->fgMcrAccessAllowed = TRUE;
			}
			status = 0;
			break;
		}

		kalMemCopy(&prNdisReq->ndisOidContent[0], &pu4IntBuf[1], 8);

		prNdisReq->ndisOidCmd = OID_CUSTOM_MCR_RW;
		prNdisReq->inNdisOidlength = 8;
		prNdisReq->outNdisOidLength = 8;

		/*                  */
		status = priv_set_ndis(prNetDev, prNdisReq, &u4BufLen);
		break;
#endif

	case PRIV_CMD_SW_CTRL:
		/*                                                                     */
		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

		kalMemCopy(&prNdisReq->ndisOidContent[0], &pu4IntBuf[1], 8);

		prNdisReq->ndisOidCmd = OID_CUSTOM_SW_CTRL;
		prNdisReq->inNdisOidlength = 8;
		prNdisReq->outNdisOidLength = 8;

		/*                  */
		status = priv_set_ndis(prNetDev, prNdisReq, &u4BufLen);
		break;


#if 0
	case PRIV_CMD_BEACON_PERIOD:
		rStatus = wlanSetInformation(prGlueInfo->prAdapter, wlanoidSetBeaconInterval, (PVOID) & pu4IntBuf[1],	/*                                      */
					     sizeof(UINT_32), &u4BufLen);
		break;
#endif

#if CFG_TCP_IP_CHKSUM_OFFLOAD
	case PRIV_CMD_CSUM_OFFLOAD:
		{
			UINT_32 u4CSUMFlags;


			if (pu4IntBuf[1] == 1) {
				u4CSUMFlags = CSUM_OFFLOAD_EN_ALL;
			} else if (pu4IntBuf[1] == 0) {
				u4CSUMFlags = 0;
			} else {
				return -EINVAL;
			}

			if (kalIoctl(prGlueInfo,
				     wlanoidSetCSUMOffload,
				     (PVOID) & u4CSUMFlags,
				     sizeof(UINT_32),
				     FALSE, FALSE, TRUE, FALSE, &u4BufLen) == WLAN_STATUS_SUCCESS) {
				if (pu4IntBuf[1] == 1) {
					prNetDev->features |= NETIF_F_HW_CSUM;
				} else if (pu4IntBuf[1] == 0) {
					prNetDev->features &= ~NETIF_F_HW_CSUM;
				}
			}
		}
		break;
#endif				/*                           */

	case PRIV_CMD_POWER_MODE:
		kalIoctl(prGlueInfo, wlanoidSet802dot11PowerSaveProfile, (PVOID) & pu4IntBuf[1],	/*                                      */
			 sizeof(UINT_32), FALSE, FALSE, TRUE, FALSE, &u4BufLen);
		break;

	case PRIV_CMD_WMM_PS:
		{
			PARAM_CUSTOM_WMM_PS_TEST_STRUC_T rWmmPsTest;

			rWmmPsTest.bmfgApsdEnAc = (UINT_8) pu4IntBuf[1];
			rWmmPsTest.ucIsEnterPsAtOnce = (UINT_8) pu4IntBuf[2];
			rWmmPsTest.ucIsDisableUcTrigger = (UINT_8) pu4IntBuf[3];
			rWmmPsTest.reserved = 0;

			kalIoctl(prGlueInfo,
				 wlanoidSetWiFiWmmPsTest,
				 (PVOID) & rWmmPsTest,
				 sizeof(PARAM_CUSTOM_WMM_PS_TEST_STRUC_T),
				 FALSE, FALSE, TRUE, FALSE, &u4BufLen);
		}
		break;

#if 0
	case PRIV_CMD_ADHOC_MODE:
		rStatus = wlanSetInformation(prGlueInfo->prAdapter, wlanoidSetAdHocMode, (PVOID) & pu4IntBuf[1],	/*                                      */
					     sizeof(UINT_32), &u4BufLen);
		break;
#endif

	case PRIV_CUSTOM_BWCS_CMD:

		DBGLOG(REQ, INFO,
		       ("pu4IntBuf[1] = %lx, size of PTA_IPC_T = %d.\n", pu4IntBuf[1],
			sizeof(PARAM_PTA_IPC_T)));

		prPtaIpc = (P_PTA_IPC_T) aucOidBuf;
		prPtaIpc->u.aucBTPParams[0] = (UINT_8) (pu4IntBuf[1] >> 24);
		prPtaIpc->u.aucBTPParams[1] = (UINT_8) (pu4IntBuf[1] >> 16);
		prPtaIpc->u.aucBTPParams[2] = (UINT_8) (pu4IntBuf[1] >> 8);
		prPtaIpc->u.aucBTPParams[3] = (UINT_8) (pu4IntBuf[1]);

		DBGLOG(REQ, INFO,
		       ("BCM BWCS CMD : PRIV_CUSTOM_BWCS_CMD : aucBTPParams[0] = %02x, aucBTPParams[1] = %02x, aucBTPParams[2] = %02x, aucBTPParams[3] = %02x.\n",
			prPtaIpc->u.aucBTPParams[0], prPtaIpc->u.aucBTPParams[1],
			prPtaIpc->u.aucBTPParams[2], prPtaIpc->u.aucBTPParams[3]));

#if 0
		status = wlanSetInformation(prGlueInfo->prAdapter,
					    wlanoidSetBT,
					    (PVOID) & aucOidBuf[0], u4CmdLen, &u4BufLen);
#endif

		status = wlanoidSetBT(prGlueInfo->prAdapter,
				      (PVOID) & aucOidBuf[0], sizeof(PARAM_PTA_IPC_T), &u4BufLen);

		if (WLAN_STATUS_SUCCESS != status) {
			status = -EFAULT;
		}

		break;

	case PRIV_CMD_BAND_CONFIG:
		{
			DBGLOG(INIT, INFO, ("CMD set_band=%lu\n", pu4IntBuf[1]));
		}
		break;

#if CFG_ENABLE_WIFI_DIRECT
	case PRIV_CMD_P2P_MODE:
		{
			PARAM_CUSTOM_P2P_SET_STRUC_T rSetP2P;
			WLAN_STATUS rWlanStatus = WLAN_STATUS_SUCCESS;

			rSetP2P.u4Enable = pu4IntBuf[1];
			rSetP2P.u4Mode = pu4IntBuf[2];

			if (!rSetP2P.u4Enable) {
				p2pNetUnregister(prGlueInfo, TRUE);
			}

			rWlanStatus = kalIoctl(prGlueInfo, wlanoidSetP2pMode, (PVOID) & rSetP2P,	/*                                      */
					       sizeof(PARAM_CUSTOM_P2P_SET_STRUC_T),
					       FALSE, FALSE, TRUE, FALSE, &u4BufLen);

			if (rSetP2P.u4Enable) {
				p2pNetRegister(prGlueInfo, TRUE);
			}


		}
		break;
#endif

	default:
		return -EOPNOTSUPP;
	}

	return status;
}


/*                                                                            */
/* 
                                       
 
                                       
                                                
                                                                               
                                                          
 
                        
                                              
                           
 
*/
/*                                                                            */
int
priv_get_int(IN struct net_device *prNetDev,
	     IN struct iw_request_info *prIwReqInfo,
	     IN union iwreq_data *prIwReqData, IN OUT char *pcExtra)
{
	UINT_32 u4SubCmd;
	PUINT_32 pu4IntBuf;
	P_GLUE_INFO_T prGlueInfo;
	UINT_32 u4BufLen = 0;
	int status = 0;
	P_NDIS_TRANSPORT_STRUCT prNdisReq;
	INT_32 ch[50];

	ASSERT(prNetDev);
	ASSERT(prIwReqInfo);
	ASSERT(prIwReqData);
	ASSERT(pcExtra);
	if (FALSE == GLUE_CHK_PR3(prNetDev, prIwReqData, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	u4SubCmd = (UINT_32) prIwReqData->mode;
	pu4IntBuf = (PUINT_32) pcExtra;

	switch (u4SubCmd) {
	case PRIV_CMD_TEST_CMD:
		/*                                                                    */
		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

		kalMemCopy(&prNdisReq->ndisOidContent[0], &pu4IntBuf[1], 8);

		prNdisReq->ndisOidCmd = OID_CUSTOM_MTK_WIFI_TEST;
		prNdisReq->inNdisOidlength = 8;
		prNdisReq->outNdisOidLength = 8;

		status = priv_get_ndis(prNetDev, prNdisReq, &u4BufLen);
		if (status == 0) {
			/*                                                                   */
			prIwReqData->mode = *(PUINT_32) &prNdisReq->ndisOidContent[4];
			/*
                                                 
                                          
                                                                         
                     
       
    */
		}
		return status;

#if CFG_SUPPORT_PRIV_MCR_RW
	case PRIV_CMD_ACCESS_MCR:
		/*                                         */
		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

		if (!prGlueInfo->fgMcrAccessAllowed) {
			status = 0;
			return status;
		}

		kalMemCopy(&prNdisReq->ndisOidContent[0], &pu4IntBuf[1], 8);

		prNdisReq->ndisOidCmd = OID_CUSTOM_MCR_RW;
		prNdisReq->inNdisOidlength = 8;
		prNdisReq->outNdisOidLength = 8;

		status = priv_get_ndis(prNetDev, prNdisReq, &u4BufLen);
		if (status == 0) {
			/*                                                                   */
			prIwReqData->mode = *(PUINT_32) &prNdisReq->ndisOidContent[4];
		}
		return status;
#endif

	case PRIV_CMD_DUMP_MEM:
		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

#if 1
		if (!prGlueInfo->fgMcrAccessAllowed) {
			status = 0;
			return status;
		}
#endif
		kalMemCopy(&prNdisReq->ndisOidContent[0], &pu4IntBuf[1], 8);

		prNdisReq->ndisOidCmd = OID_CUSTOM_MEM_DUMP;
		prNdisReq->inNdisOidlength = sizeof(PARAM_CUSTOM_MEM_DUMP_STRUC_T);
		prNdisReq->outNdisOidLength = sizeof(PARAM_CUSTOM_MEM_DUMP_STRUC_T);

		status = priv_get_ndis(prNetDev, prNdisReq, &u4BufLen);
		if (status == 0) {
			prIwReqData->mode = *(PUINT_32) &prNdisReq->ndisOidContent[0];
		}
		return status;

	case PRIV_CMD_SW_CTRL:
		/*                                          */

		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

		kalMemCopy(&prNdisReq->ndisOidContent[0], &pu4IntBuf[1], 8);

		prNdisReq->ndisOidCmd = OID_CUSTOM_SW_CTRL;
		prNdisReq->inNdisOidlength = 8;
		prNdisReq->outNdisOidLength = 8;

		status = priv_get_ndis(prNetDev, prNdisReq, &u4BufLen);
		if (status == 0) {
			/*                                                                   */
			prIwReqData->mode = *(PUINT_32) &prNdisReq->ndisOidContent[4];
		}
		return status;

#if 0
	case PRIV_CMD_BEACON_PERIOD:
		status = wlanQueryInformation(prGlueInfo->prAdapter,
					      wlanoidQueryBeaconInterval,
					      (PVOID) pu4IntBuf, sizeof(UINT_32), &u4BufLen);
		return status;

	case PRIV_CMD_POWER_MODE:
		status = wlanQueryInformation(prGlueInfo->prAdapter,
					      wlanoidQuery802dot11PowerSaveProfile,
					      (PVOID) pu4IntBuf, sizeof(UINT_32), &u4BufLen);
		return status;

	case PRIV_CMD_ADHOC_MODE:
		status = wlanQueryInformation(prGlueInfo->prAdapter,
					      wlanoidQueryAdHocMode,
					      (PVOID) pu4IntBuf, sizeof(UINT_32), &u4BufLen);
		return status;
#endif

	case PRIV_CMD_BAND_CONFIG:
		DBGLOG(INIT, INFO, ("CMD get_band=\n"));
		prIwReqData->mode = 0;
		return status;

	default:
		break;
	}

	u4SubCmd = (UINT_32) prIwReqData->data.flags;

	switch (u4SubCmd) {
	case PRIV_CMD_GET_CH_LIST:
		{
			UINT_16 i, j = 0;
			UINT_8 NumOfChannel = 50;
			UINT_8 ucMaxChannelNum = 50;
			RF_CHANNEL_INFO_T aucChannelList[50];

			kalGetChannelList(prGlueInfo, BAND_NULL, ucMaxChannelNum, &NumOfChannel,
					  aucChannelList);
			if (NumOfChannel > 50)
				NumOfChannel = 50;

			if (kalIsAPmode(prGlueInfo)) {
				for (i = 0; i < NumOfChannel; i++) {
					if ((aucChannelList[i].ucChannelNum <= 13) ||
					    (aucChannelList[i].ucChannelNum == 36
					     || aucChannelList[i].ucChannelNum == 40
					     || aucChannelList[i].ucChannelNum == 44
					     || aucChannelList[i].ucChannelNum == 48)) {
						ch[j] = (INT_32) aucChannelList[i].ucChannelNum;
						j++;
					}
				}
			} else {
				for (j = 0; j < NumOfChannel; j++) {
					ch[j] = (INT_32) aucChannelList[j].ucChannelNum;
				}
			}

			prIwReqData->data.length = j;
			if (copy_to_user
			    (prIwReqData->data.pointer, ch, NumOfChannel * sizeof(INT_32))) {
				return -EFAULT;
			} else
				return status;
		}

	case PRIV_CMD_GET_BUILD_DATE_CODE:
		{
			UINT_8 aucBuffer[16];

			if (kalIoctl(prGlueInfo,
				     wlanoidQueryBuildDateCode,
				     (PVOID) aucBuffer,
				     sizeof(UINT_8) * 16,
				     TRUE, TRUE, TRUE, FALSE, &u4BufLen) == WLAN_STATUS_SUCCESS) {
				prIwReqData->data.length = sizeof(UINT_8) * 16;

				if (copy_to_user
				    (prIwReqData->data.pointer, aucBuffer,
				     prIwReqData->data.length)) {
					return -EFAULT;
				} else
					return status;
			} else {
				return -EFAULT;
			}
		}

	default:
		return -EOPNOTSUPP;
	}

	return status;
}				/*              */


/*                                                                            */
/* 
                                             
 
                                           
                                                    
                                                                                
                                                
 
                        
                                              
                                             
 
*/
/*                                                                            */
int
priv_set_ints(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwReqInfo,
	      IN union iwreq_data *prIwReqData, IN char *pcExtra)
{
	UINT_32 u4SubCmd, u4BufLen;
	P_GLUE_INFO_T prGlueInfo;
	int status = 0;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	P_SET_TXPWR_CTRL_T prTxpwr;

	ASSERT(prNetDev);
	ASSERT(prIwReqInfo);
	ASSERT(prIwReqData);
	ASSERT(pcExtra);

	if (FALSE == GLUE_CHK_PR3(prNetDev, prIwReqData, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	u4SubCmd = (UINT_32) prIwReqData->data.flags;

	switch (u4SubCmd) {
	case PRIV_CMD_SET_TX_POWER:
		{
			INT_32 *setting = prIwReqData->data.pointer;
			UINT_16 i;

#if 0
			printk("Tx power num = %d\n", prIwReqData->data.length);

			printk("Tx power setting = %d %d %d %d\n",
			       setting[0], setting[1], setting[2], setting[3]);
#endif
			prTxpwr = &prGlueInfo->rTxPwr;
			if (setting[0] == 0 && prIwReqData->data.length == 4 /*          */) {
				/*                                                                              */
				if (setting[1] == 1 || setting[1] == 0) {
					if (setting[2] == 0 || setting[2] == 1)
						prTxpwr->c2GLegacyStaPwrOffset = setting[3];
					if (setting[2] == 0 || setting[2] == 2)
						prTxpwr->c5GLegacyStaPwrOffset = setting[3];
				}
				if (setting[1] == 2 || setting[1] == 0) {
					if (setting[2] == 0 || setting[2] == 1)
						prTxpwr->c2GHotspotPwrOffset = setting[3];
					if (setting[2] == 0 || setting[2] == 2)
						prTxpwr->c5GHotspotPwrOffset = setting[3];
				}
				if (setting[1] == 3 || setting[1] == 0) {
					if (setting[2] == 0 || setting[2] == 1)
						prTxpwr->c2GP2pPwrOffset = setting[3];
					if (setting[2] == 0 || setting[2] == 2)
						prTxpwr->c5GP2pPwrOffset = setting[3];
				}
				if (setting[1] == 4 || setting[1] == 0) {
					if (setting[2] == 0 || setting[2] == 1)
						prTxpwr->c2GBowPwrOffset = setting[3];
					if (setting[2] == 0 || setting[2] == 2)
						prTxpwr->c5GBowPwrOffset = setting[3];
				}
			} else if (setting[0] == 1 && prIwReqData->data.length == 2) {
				prTxpwr->ucConcurrencePolicy = setting[1];
			} else if (setting[0] == 2 && prIwReqData->data.length == 3) {
				if (setting[1] == 0) {
					for (i = 0; i < 14; i++)
						prTxpwr->acTxPwrLimit2G[i] = setting[2];
				} else if (setting[1] <= 14)
					prTxpwr->acTxPwrLimit2G[setting[1] - 1] = setting[2];
			} else if (setting[0] == 3 && prIwReqData->data.length == 3) {
				if (setting[1] == 0) {
					for (i = 0; i < 4; i++)
						prTxpwr->acTxPwrLimit5G[i] = setting[2];
				} else if (setting[1] <= 4)
					prTxpwr->acTxPwrLimit5G[setting[1] - 1] = setting[2];
			} else if (setting[0] == 4 && prIwReqData->data.length == 2) {
				if (setting[1] == 0) {
					wlanDefTxPowerCfg(prGlueInfo->prAdapter);
				}
				rStatus = kalIoctl(prGlueInfo,
						   wlanoidSetTxPower,
						   prTxpwr,
						   sizeof(SET_TXPWR_CTRL_T),
						   TRUE, FALSE, FALSE, FALSE, &u4BufLen);
			} else
				return -EFAULT;
		}
		return status;
	default:
		break;
	}

	return status;
}


/*                                                                            */
/* 
                                             
 
                                       
                                                
                                                                               
                                                          
 
                        
                                              
                           
 
*/
/*                                                                            */
int
priv_get_ints(IN struct net_device *prNetDev,
	      IN struct iw_request_info *prIwReqInfo,
	      IN union iwreq_data *prIwReqData, IN OUT char *pcExtra)
{
	UINT_32 u4SubCmd;
	P_GLUE_INFO_T prGlueInfo;
	int status = 0;
	INT_32 ch[50];

	ASSERT(prNetDev);
	ASSERT(prIwReqInfo);
	ASSERT(prIwReqData);
	ASSERT(pcExtra);
	if (FALSE == GLUE_CHK_PR3(prNetDev, prIwReqData, pcExtra)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	u4SubCmd = (UINT_32) prIwReqData->data.flags;

	switch (u4SubCmd) {
	case PRIV_CMD_GET_CH_LIST:
		{
			UINT_16 i;
			UINT_8 NumOfChannel = 50;
			UINT_8 ucMaxChannelNum = 50;
			RF_CHANNEL_INFO_T aucChannelList[50];

			kalGetChannelList(prGlueInfo, BAND_NULL, ucMaxChannelNum, &NumOfChannel,
					  aucChannelList);
			if (NumOfChannel > 50)
				NumOfChannel = 50;

			for (i = 0; i < NumOfChannel; i++) {
				ch[i] = (INT_32) aucChannelList[i].ucChannelNum;
			}

			prIwReqData->data.length = NumOfChannel;
			if (copy_to_user
			    (prIwReqData->data.pointer, ch, NumOfChannel * sizeof(INT_32))) {
				return -EFAULT;
			} else
				return status;
		}
	default:
		break;
	}

	return status;
}				/*              */

/*                                                                            */
/* 
                                             
 
                                       
                                                         
 
                        
                                              
                                             
 
*/
/*                                                                            */
int
priv_set_struct(IN struct net_device *prNetDev,
		IN struct iw_request_info *prIwReqInfo,
		IN union iwreq_data *prIwReqData, IN char *pcExtra)
{
	UINT_32 u4SubCmd = 0;
	int status = 0;
	/*                                            */
	UINT_32 u4CmdLen = 0;
	P_NDIS_TRANSPORT_STRUCT prNdisReq;
	PUINT_32 pu4IntBuf = NULL;

	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_32 u4BufLen = 0;

	ASSERT(prNetDev);
	/*                      */
	ASSERT(prIwReqData);
	/*                  */

	kalMemZero(&aucOidBuf[0], sizeof(aucOidBuf));

	if (FALSE == GLUE_CHK_PR2(prNetDev, prIwReqData)) {
		return -EINVAL;
	}
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));

	u4SubCmd = (UINT_32) prIwReqData->data.flags;

#if 0
	printk(KERN_INFO DRV_NAME "priv_set_struct(): prIwReqInfo->cmd(0x%X), u4SubCmd(%ld)\n",
	       prIwReqInfo->cmd, u4SubCmd);
#endif

	switch (u4SubCmd) {
#if 0				/*             */
	case PRIV_CMD_BT_COEXIST:
		u4CmdLen = prIwReqData->data.length * sizeof(UINT_32);
		ASSERT(sizeof(PARAM_CUSTOM_BT_COEXIST_T) >= u4CmdLen);
		if (sizeof(PARAM_CUSTOM_BT_COEXIST_T) < u4CmdLen) {
			return -EFAULT;
		}

		if (copy_from_user(&aucOidBuf[0], prIwReqData->data.pointer, u4CmdLen)) {
			status = -EFAULT;	/*                 */
			break;
		}

		rStatus = wlanSetInformation(prGlueInfo->prAdapter,
					     wlanoidSetBtCoexistCtrl,
					     (PVOID) & aucOidBuf[0], u4CmdLen, &u4BufLen);
		if (WLAN_STATUS_SUCCESS != rStatus) {
			status = -EFAULT;
		}
		break;
#endif

	case PRIV_CUSTOM_BWCS_CMD:
		u4CmdLen = prIwReqData->data.length * sizeof(UINT_32);
		ASSERT(sizeof(PARAM_PTA_IPC_T) >= u4CmdLen);
		if (sizeof(PARAM_PTA_IPC_T) < u4CmdLen) {
			return -EFAULT;
		}
#if CFG_SUPPORT_BCM && CFG_SUPPORT_BCM_BWCS && CFG_SUPPORT_BCM_BWCS_DEBUG
		DBGLOG(REQ, INFO,
		       ("ucCmdLen = %d, size of PTA_IPC_T = %d, prIwReqData->data = 0x%x.\n",
			u4CmdLen, sizeof(PARAM_PTA_IPC_T), prIwReqData->data));

		DBGLOG(REQ, INFO, ("priv_set_struct(): prIwReqInfo->cmd(0x%X), u4SubCmd(%ld)\n",
				   prIwReqInfo->cmd, u4SubCmd));

		DBGLOG(REQ, INFO, ("*pcExtra = 0x%x\n", *pcExtra));
#endif

		if (copy_from_user(&aucOidBuf[0], prIwReqData->data.pointer, u4CmdLen)) {
			status = -EFAULT;	/*                 */
			break;
		}
#if CFG_SUPPORT_BCM && CFG_SUPPORT_BCM_BWCS && CFG_SUPPORT_BCM_BWCS_DEBUG
		DBGLOG(REQ, INFO, ("priv_set_struct(): BWCS CMD = %02x%02x%02x%02x\n",
				   aucOidBuf[2], aucOidBuf[3], aucOidBuf[4], aucOidBuf[5]));
#endif

#if 0
		status = wlanSetInformation(prGlueInfo->prAdapter,
					    wlanoidSetBT,
					    (PVOID) & aucOidBuf[0], u4CmdLen, &u4BufLen);
#endif

#if 1
		status = wlanoidSetBT(prGlueInfo->prAdapter,
				      (PVOID) & aucOidBuf[0], u4CmdLen, &u4BufLen);
#endif

		if (WLAN_STATUS_SUCCESS != status) {
			status = -EFAULT;
		}

		break;

#if CFG_SUPPORT_WPS2
	case PRIV_CMD_WSC_PROBE_REQ:
		{
			/*                               */
			if (prIwReqData->data.length > 0) {
				if (copy_from_user(prGlueInfo->aucWSCIE, prIwReqData->data.pointer,
						   prIwReqData->data.length)) {
					status = -EFAULT;
					break;
				}
				prGlueInfo->u2WSCIELen = prIwReqData->data.length;
			} else {
				prGlueInfo->u2WSCIELen = 0;
			}
		}
		break;
#endif
	case PRIV_CMD_OID:
		if (copy_from_user(&aucOidBuf[0],
				   prIwReqData->data.pointer, prIwReqData->data.length)) {
			status = -EFAULT;
			break;
		}
		if (!kalMemCmp(&aucOidBuf[0], pcExtra, prIwReqData->data.length)) {
			DBGLOG(REQ, INFO, ("pcExtra buffer is valid\n"));
		} else
			DBGLOG(REQ, INFO, ("pcExtra 0x%p\n", pcExtra));

		/*                  */
		status =
		    priv_set_ndis(prNetDev, (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0], &u4BufLen);
		/*                           */
		((P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0])->outNdisOidLength = u4BufLen;

		if (copy_to_user(prIwReqData->data.pointer,
				 &aucOidBuf[0], OFFSET_OF(NDIS_TRANSPORT_STRUCT, ndisOidContent))) {
			DBGLOG(REQ, INFO, ("copy_to_user oidBuf fail\n"));
			status = -EFAULT;
		}

		break;

	case PRIV_CMD_SW_CTRL:
		pu4IntBuf = (PUINT_32) prIwReqData->data.pointer;
		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

		/*                                                                          */
		if (copy_from_user(&prNdisReq->ndisOidContent[0],
				   prIwReqData->data.pointer, prIwReqData->data.length)) {
			status = -EFAULT;
			break;
		}
		prNdisReq->ndisOidCmd = OID_CUSTOM_SW_CTRL;
		prNdisReq->inNdisOidlength = 8;
		prNdisReq->outNdisOidLength = 8;

		/*                  */
		status = priv_set_ndis(prNetDev, prNdisReq, &u4BufLen);
		break;

	default:
		return -EOPNOTSUPP;
	}

	return status;
}

/*                                                                            */
/* 
                                          
 
                                       
                                                
                                     
 
                        
                                                      
                                                     
 
*/
/*                                                                            */
int
priv_get_struct(IN struct net_device *prNetDev,
		IN struct iw_request_info *prIwReqInfo,
		IN union iwreq_data *prIwReqData, IN OUT char *pcExtra)
{
	UINT_32 u4SubCmd = 0;
	P_NDIS_TRANSPORT_STRUCT prNdisReq = NULL;

	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_32 u4BufLen = 0;
	PUINT_32 pu4IntBuf = NULL;
	int status = 0;

	kalMemZero(&aucOidBuf[0], sizeof(aucOidBuf));

	ASSERT(prNetDev);
	ASSERT(prIwReqData);
	if (!prNetDev || !prIwReqData) {
		DBGLOG(REQ, INFO, ("priv_get_struct(): invalid param(0x%p, 0x%p)\n",
				   prNetDev, prIwReqData));
		return -EINVAL;
	}

	u4SubCmd = (UINT_32) prIwReqData->data.flags;
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));
	ASSERT(prGlueInfo);
	if (!prGlueInfo) {
		DBGLOG(REQ, INFO, ("priv_get_struct(): invalid prGlueInfo(0x%p, 0x%p)\n",
				   prNetDev, *((P_GLUE_INFO_T *) netdev_priv(prNetDev))));
		return -EINVAL;
	}
#if 0
	printk(KERN_INFO DRV_NAME "priv_get_struct(): prIwReqInfo->cmd(0x%X), u4SubCmd(%ld)\n",
	       prIwReqInfo->cmd, u4SubCmd);
#endif
	memset(aucOidBuf, 0, sizeof(aucOidBuf));

	switch (u4SubCmd) {
	case PRIV_CMD_OID:
		if (copy_from_user(&aucOidBuf[0],
				   prIwReqData->data.pointer, sizeof(NDIS_TRANSPORT_STRUCT))) {
			DBGLOG(REQ, INFO, ("priv_get_struct() copy_from_user oidBuf fail\n"));
			return -EFAULT;
		}

		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];
#if 0
		printk(KERN_NOTICE "\n priv_get_struct cmd 0x%02x len:%d OID:0x%08x OID Len:%d\n",
		       cmd, pIwReq->u.data.length, ndisReq->ndisOidCmd, ndisReq->inNdisOidlength);
#endif
		if (priv_get_ndis(prNetDev, prNdisReq, &u4BufLen) == 0) {
			prNdisReq->outNdisOidLength = u4BufLen;
			if (copy_to_user(prIwReqData->data.pointer,
					 &aucOidBuf[0],
					 u4BufLen + sizeof(NDIS_TRANSPORT_STRUCT) -
					 sizeof(prNdisReq->ndisOidContent))) {
				DBGLOG(REQ, INFO,
				       ("priv_get_struct() copy_to_user oidBuf fail(1)\n"));
				return -EFAULT;
			}
			return 0;
		} else {
			prNdisReq->outNdisOidLength = u4BufLen;
			if (copy_to_user(prIwReqData->data.pointer,
					 &aucOidBuf[0],
					 OFFSET_OF(NDIS_TRANSPORT_STRUCT, ndisOidContent))) {
				DBGLOG(REQ, INFO,
				       ("priv_get_struct() copy_to_user oidBuf fail(2)\n"));
			}
			return -EFAULT;
		}
		break;

	case PRIV_CMD_SW_CTRL:
		pu4IntBuf = (PUINT_32) prIwReqData->data.pointer;
		prNdisReq = (P_NDIS_TRANSPORT_STRUCT) &aucOidBuf[0];

		if (copy_from_user(&prNdisReq->ndisOidContent[0],
				   prIwReqData->data.pointer, prIwReqData->data.length)) {
			DBGLOG(REQ, INFO, ("priv_get_struct() copy_from_user oidBuf fail\n"));
			return -EFAULT;
		}

		prNdisReq->ndisOidCmd = OID_CUSTOM_SW_CTRL;
		prNdisReq->inNdisOidlength = 8;
		prNdisReq->outNdisOidLength = 8;

		status = priv_get_ndis(prNetDev, prNdisReq, &u4BufLen);
		if (status == 0) {
			prNdisReq->outNdisOidLength = u4BufLen;
			/*                                                                                      */

			if (copy_to_user(prIwReqData->data.pointer,
					 &prNdisReq->ndisOidContent[4],
					 4 /*                                                  */))
			{
				DBGLOG(REQ, INFO,
				       ("priv_get_struct() copy_to_user oidBuf fail(2)\n"));
			}
		}
		return 0;
		break;
	default:
		DBGLOG(REQ, WARN, ("get struct cmd:0x%lx\n", u4SubCmd));
		return -EOPNOTSUPP;
	}
}				/*                 */

/*                                                                            */
/* 
                                                              
 
                                       
                                                                 
                                                                          
                                                                     
                                                                        
                                                                        
 
                       
                                              
 
*/
/*                                                                            */
static int
priv_set_ndis(IN struct net_device *prNetDev,
	      IN NDIS_TRANSPORT_STRUCT * prNdisReq, OUT PUINT_32 pu4OutputLen)
{
	P_WLAN_REQ_ENTRY prWlanReqEntry = NULL;
	WLAN_STATUS status = WLAN_STATUS_SUCCESS;
	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT_32 u4SetInfoLen = 0;

	ASSERT(prNetDev);
	ASSERT(prNdisReq);
	ASSERT(pu4OutputLen);

	if (!prNetDev || !prNdisReq || !pu4OutputLen) {
		DBGLOG(REQ, INFO, ("priv_set_ndis(): invalid param(0x%p, 0x%p, 0x%p)\n",
				   prNetDev, prNdisReq, pu4OutputLen));
		return -EINVAL;
	}

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));
	ASSERT(prGlueInfo);
	if (!prGlueInfo) {
		DBGLOG(REQ, INFO, ("priv_set_ndis(): invalid prGlueInfo(0x%p, 0x%p)\n",
				   prNetDev, *((P_GLUE_INFO_T *) netdev_priv(prNetDev))));
		return -EINVAL;
	}
#if 0
	printk(KERN_INFO DRV_NAME "priv_set_ndis(): prNdisReq->ndisOidCmd(0x%lX)\n",
	       prNdisReq->ndisOidCmd);
#endif

	if (FALSE == reqSearchSupportedOidEntry(prNdisReq->ndisOidCmd, &prWlanReqEntry)) {
		/*                                                                   */
		return -EOPNOTSUPP;
	}

	if (NULL == prWlanReqEntry->pfOidSetHandler) {
		/*                                                                      */
		return -EOPNOTSUPP;
	}
#if 0
	printk(KERN_INFO DRV_NAME "priv_set_ndis(): %s\n", prWlanReqEntry->pucOidName);
#endif

	if (prWlanReqEntry->fgSetBufLenChecking) {
		if (prNdisReq->inNdisOidlength != prWlanReqEntry->u4InfoBufLen) {
			DBGLOG(REQ, WARN, ("Set %s: Invalid length (current=%ld, needed=%ld)\n",
					   prWlanReqEntry->pucOidName,
					   prNdisReq->inNdisOidlength,
					   prWlanReqEntry->u4InfoBufLen));

			*pu4OutputLen = prWlanReqEntry->u4InfoBufLen;
			return -EINVAL;
		}
	}

	if (prWlanReqEntry->eOidMethod == ENUM_OID_GLUE_ONLY) {
		/*                   */
		status = prWlanReqEntry->pfOidSetHandler(prGlueInfo,
							 prNdisReq->ndisOidContent,
							 prNdisReq->inNdisOidlength, &u4SetInfoLen);
	} else if (prWlanReqEntry->eOidMethod == ENUM_OID_GLUE_EXTENSION) {
		/*                        */
		status = prWlanReqEntry->pfOidSetHandler(prGlueInfo,
							 prNdisReq->ndisOidContent,
							 prNdisReq->inNdisOidlength, &u4SetInfoLen);
	} else if (prWlanReqEntry->eOidMethod == ENUM_OID_DRIVER_CORE) {
		/*             */

		status = kalIoctl(prGlueInfo,
				  (PFN_OID_HANDLER_FUNC) prWlanReqEntry->pfOidSetHandler,
				  prNdisReq->ndisOidContent,
				  prNdisReq->inNdisOidlength,
				  FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);
	} else {
		DBGLOG(REQ, INFO, ("priv_set_ndis(): unsupported OID method:0x%x\n",
				   prWlanReqEntry->eOidMethod));
		return -EOPNOTSUPP;
	}

	*pu4OutputLen = u4SetInfoLen;

	switch (status) {
	case WLAN_STATUS_SUCCESS:
		break;

	case WLAN_STATUS_INVALID_LENGTH:
		/*                                                                */
		/*                             */
		/*                             */
		/*                 */
		break;
	}

	if (WLAN_STATUS_SUCCESS != status) {
		return -EFAULT;
	}

	return 0;
}				/*               */

/*                                                                            */
/* 
                                                                             
                                                                      
 
                                       
                                                                 
                                                                          
                                                                    
                                                                       
                                                                       
 
                       
                                              
                                          
 
*/
/*                                                                            */
static int
priv_get_ndis(IN struct net_device *prNetDev,
	      IN NDIS_TRANSPORT_STRUCT * prNdisReq, OUT PUINT_32 pu4OutputLen)
{
	P_WLAN_REQ_ENTRY prWlanReqEntry = NULL;
	UINT_32 u4BufLen = 0;
	WLAN_STATUS status = WLAN_STATUS_SUCCESS;
	P_GLUE_INFO_T prGlueInfo = NULL;

	ASSERT(prNetDev);
	ASSERT(prNdisReq);
	ASSERT(pu4OutputLen);

	if (!prNetDev || !prNdisReq || !pu4OutputLen) {
		DBGLOG(REQ, INFO, ("priv_get_ndis(): invalid param(0x%p, 0x%p, 0x%p)\n",
				   prNetDev, prNdisReq, pu4OutputLen));
		return -EINVAL;
	}

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prNetDev));
	ASSERT(prGlueInfo);
	if (!prGlueInfo) {
		DBGLOG(REQ, INFO, ("priv_get_ndis(): invalid prGlueInfo(0x%p, 0x%p)\n",
				   prNetDev, *((P_GLUE_INFO_T *) netdev_priv(prNetDev))));
		return -EINVAL;
	}
#if 0
	printk(KERN_INFO DRV_NAME "priv_get_ndis(): prNdisReq->ndisOidCmd(0x%lX)\n",
	       prNdisReq->ndisOidCmd);
#endif

	if (FALSE == reqSearchSupportedOidEntry(prNdisReq->ndisOidCmd, &prWlanReqEntry)) {
		/*                                                                     */
		return -EOPNOTSUPP;
	}


	if (NULL == prWlanReqEntry->pfOidQueryHandler) {
		/*                                                                          */
		return -EOPNOTSUPP;
	}
#if 0
	printk(KERN_INFO DRV_NAME "priv_get_ndis(): %s\n", prWlanReqEntry->pucOidName);
#endif

	if (prWlanReqEntry->fgQryBufLenChecking) {
		if (prNdisReq->inNdisOidlength < prWlanReqEntry->u4InfoBufLen) {
			/*                                            */
			/*                                                                    */
			/*                             */
			/*                             */
			/*                                 */

			*pu4OutputLen = prWlanReqEntry->u4InfoBufLen;

			status = WLAN_STATUS_INVALID_LENGTH;
			return -EINVAL;
		}
	}


	if (prWlanReqEntry->eOidMethod == ENUM_OID_GLUE_ONLY) {
		/*                   */
		status = prWlanReqEntry->pfOidQueryHandler(prGlueInfo,
							   prNdisReq->ndisOidContent,
							   prNdisReq->inNdisOidlength, &u4BufLen);
	} else if (prWlanReqEntry->eOidMethod == ENUM_OID_GLUE_EXTENSION) {
		/*                        */
		status = prWlanReqEntry->pfOidQueryHandler(prGlueInfo,
							   prNdisReq->ndisOidContent,
							   prNdisReq->inNdisOidlength, &u4BufLen);
	} else if (prWlanReqEntry->eOidMethod == ENUM_OID_DRIVER_CORE) {
		/*             */

		status = kalIoctl(prGlueInfo,
				  (PFN_OID_HANDLER_FUNC) prWlanReqEntry->pfOidQueryHandler,
				  prNdisReq->ndisOidContent,
				  prNdisReq->inNdisOidlength, TRUE, TRUE, TRUE, FALSE, &u4BufLen);
	} else {
		DBGLOG(REQ, INFO, ("priv_set_ndis(): unsupported OID method:0x%x\n",
				   prWlanReqEntry->eOidMethod));
		return -EOPNOTSUPP;
	}

	*pu4OutputLen = u4BufLen;

	switch (status) {
	case WLAN_STATUS_SUCCESS:
		break;

	case WLAN_STATUS_INVALID_LENGTH:
		/*                                                                */
		/*                             */
		/*                             */
		/*             */
		break;
	}

	if (WLAN_STATUS_SUCCESS != status) {
		return -EOPNOTSUPP;
	}

	return 0;
}				/*               */

/*                                                                            */
/* 
                                                      
 
                                                
                                                          
 
                                    
                                        
*/
/*                                                                            */
static BOOLEAN reqSearchSupportedOidEntry(IN UINT_32 rOid, OUT P_WLAN_REQ_ENTRY *ppWlanReqEntry)
{
	INT_32 i, j, k;

	i = 0;
	j = NUM_SUPPORTED_OIDS - 1;

	while (i <= j) {
		k = (i + j) / 2;

		if (rOid == arWlanOidReqTable[k].rOid) {
			*ppWlanReqEntry = &arWlanOidReqTable[k];
			return TRUE;
		} else if (rOid < arWlanOidReqTable[k].rOid) {
			j = k - 1;
		} else {
			i = k + 1;
		}
	}

	return FALSE;
}				/*                            */

#if 0
/*                                                                            */
/* 
                                                                             
                               
 
                                                                     
                                                                                         
                                                               
                                                                                
                                                                                
                                                                                 
                                                                     
 
                             
                                    
*/
/*                                                                            */
static WLAN_STATUS
reqExtQueryConfiguration(IN P_GLUE_INFO_T prGlueInfo,
			 OUT PVOID pvQueryBuffer,
			 IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen)
{
	P_PARAM_802_11_CONFIG_T prQueryConfig = (P_PARAM_802_11_CONFIG_T) pvQueryBuffer;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	UINT_32 u4QueryInfoLen = 0;

	DEBUGFUNC("wlanoidQueryConfiguration");


	ASSERT(prGlueInfo);
	ASSERT(pu4QueryInfoLen);

	*pu4QueryInfoLen = sizeof(PARAM_802_11_CONFIG_T);
	if (u4QueryBufferLen < sizeof(PARAM_802_11_CONFIG_T)) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	ASSERT(pvQueryBuffer);

	kalMemZero(prQueryConfig, sizeof(PARAM_802_11_CONFIG_T));

	/*                                         */
	prQueryConfig->u4Length = sizeof(PARAM_802_11_CONFIG_T);

#if defined(_HIF_SDIO)
	rStatus = sdio_io_ctrl(prGlueInfo,
			       wlanoidSetBeaconInterval,
			       &prQueryConfig->u4BeaconPeriod,
			       sizeof(UINT_32), TRUE, TRUE, &u4QueryInfoLen);
#else
	rStatus = wlanQueryInformation(prGlueInfo->prAdapter,
				       wlanoidQueryBeaconInterval,
				       &prQueryConfig->u4BeaconPeriod,
				       sizeof(UINT_32), &u4QueryInfoLen);
#endif
	if (rStatus != WLAN_STATUS_SUCCESS) {
		return rStatus;
	}
#if defined(_HIF_SDIO)
	rStatus = sdio_io_ctrl(prGlueInfo,
			       wlanoidQueryAtimWindow,
			       &prQueryConfig->u4ATIMWindow,
			       sizeof(UINT_32), TRUE, TRUE, &u4QueryInfoLen);
#else
	rStatus = wlanQueryInformation(prGlueInfo->prAdapter,
				       wlanoidQueryAtimWindow,
				       &prQueryConfig->u4ATIMWindow,
				       sizeof(UINT_32), &u4QueryInfoLen);
#endif
	if (rStatus != WLAN_STATUS_SUCCESS) {
		return rStatus;
	}
#if defined(_HIF_SDIO)
	rStatus = sdio_io_ctrl(prGlueInfo,
			       wlanoidQueryFrequency,
			       &prQueryConfig->u4DSConfig,
			       sizeof(UINT_32), TRUE, TRUE, &u4QueryInfoLen);
#else
	rStatus = wlanQueryInformation(prGlueInfo->prAdapter,
				       wlanoidQueryFrequency,
				       &prQueryConfig->u4DSConfig,
				       sizeof(UINT_32), &u4QueryInfoLen);
#endif
	if (rStatus != WLAN_STATUS_SUCCESS) {
		return rStatus;
	}

	prQueryConfig->rFHConfig.u4Length = sizeof(PARAM_802_11_CONFIG_FH_T);

	return rStatus;

}				/*                                   */


/*                                                                            */
/* 
                                                                           
              
 
                                                                 
                                                                                  
                                                         
                                                                            
                                                                              
                                                                            
                                                         
 
                             
                                    
                                  
*/
/*                                                                            */
static WLAN_STATUS
reqExtSetConfiguration(IN P_GLUE_INFO_T prGlueInfo,
		       IN PVOID pvSetBuffer, IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen)
{
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;
	P_PARAM_802_11_CONFIG_T prNewConfig = (P_PARAM_802_11_CONFIG_T) pvSetBuffer;
	UINT_32 u4SetInfoLen = 0;

	DEBUGFUNC("wlanoidSetConfiguration");


	ASSERT(prGlueInfo);
	ASSERT(pu4SetInfoLen);

	*pu4SetInfoLen = sizeof(PARAM_802_11_CONFIG_T);

	if (u4SetBufferLen < *pu4SetInfoLen) {
		return WLAN_STATUS_INVALID_LENGTH;
	}

	/*                                                                          */
	if (prGlueInfo->eParamMediaStateIndicated == PARAM_MEDIA_STATE_CONNECTED) {
		return WLAN_STATUS_NOT_ACCEPTED;
	}

	ASSERT(pvSetBuffer);

#if defined(_HIF_SDIO)
	rStatus = sdio_io_ctrl(prGlueInfo,
			       wlanoidSetBeaconInterval,
			       &prNewConfig->u4BeaconPeriod,
			       sizeof(UINT_32), FALSE, TRUE, &u4SetInfoLen);
#else
	rStatus = wlanSetInformation(prGlueInfo->prAdapter,
				     wlanoidSetBeaconInterval,
				     &prNewConfig->u4BeaconPeriod, sizeof(UINT_32), &u4SetInfoLen);
#endif
	if (rStatus != WLAN_STATUS_SUCCESS) {
		return rStatus;
	}
#if defined(_HIF_SDIO)
	rStatus = sdio_io_ctrl(prGlueInfo,
			       wlanoidSetAtimWindow,
			       &prNewConfig->u4ATIMWindow,
			       sizeof(UINT_32), FALSE, TRUE, &u4SetInfoLen);
#else
	rStatus = wlanSetInformation(prGlueInfo->prAdapter,
				     wlanoidSetAtimWindow,
				     &prNewConfig->u4ATIMWindow, sizeof(UINT_32), &u4SetInfoLen);
#endif
	if (rStatus != WLAN_STATUS_SUCCESS) {
		return rStatus;
	}
#if defined(_HIF_SDIO)
	rStatus = sdio_io_ctrl(prGlueInfo,
			       wlanoidSetFrequency,
			       &prNewConfig->u4DSConfig,
			       sizeof(UINT_32), FALSE, TRUE, &u4SetInfoLen);
#else
	rStatus = wlanSetInformation(prGlueInfo->prAdapter,
				     wlanoidSetFrequency,
				     &prNewConfig->u4DSConfig, sizeof(UINT_32), &u4SetInfoLen);
#endif

	if (rStatus != WLAN_STATUS_SUCCESS) {
		return rStatus;
	}

	return rStatus;

}				/*                                 */

#endif
/*                                                                            */
/* 
                                                                                     
                                                                                     
 
                                                       
                                                                              
                                                        
                                                                            
                                                                               
                                                         
 
                             
                                                                 
                                    
 
*/
/*                                                                            */
static WLAN_STATUS
reqExtSetAcpiDevicePowerState(IN P_GLUE_INFO_T prGlueInfo,
			      IN PVOID pvSetBuffer,
			      IN UINT_32 u4SetBufferLen, OUT PUINT_32 pu4SetInfoLen)
{
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;

	ASSERT(prGlueInfo);
	ASSERT(pvSetBuffer);
	ASSERT(pu4SetInfoLen);

	/*                                                                           */

	/*                                                     */
	/*                                 */
	/*              */
	/*                 */
	/*                 */
	return rStatus;
}
