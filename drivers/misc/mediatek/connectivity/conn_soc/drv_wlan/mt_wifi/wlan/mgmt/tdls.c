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
#include "precomp.h"

#if (CFG_SUPPORT_TDLS == 1)
#include "gl_wext.h"
#include "tdls.h"
#include "gl_cfg80211.h"
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
#include <uapi/linux/nl80211.h>
#endif
/*                                                                              
                        
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
static VOID
TdlsCmdTestRxIndicatePkts (
	GLUE_INFO_T 						*prGlueInfo,
	struct sk_buff						*prSkb
	);

#if TDLS_CFG_CMD_TEST
static void
TdlsCmdTestAddPeer(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestChSwProhibitedBitSet(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestChSwReqRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestChSwRspRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestChSwTimeoutSkip(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestDataContSend(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestDataRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestDataSend(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestDelay(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestDiscoveryReqRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestKeepAliveSkip(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestProhibitedBitSet(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestPtiReqRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestPtiRspRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestPtiTxDoneFail(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestRvFrame(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestSetupConfirmRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestSetupReqRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestSetupRspRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestScanCtrl(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestTearDownRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestTxFailSkip(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestTxTdlsFrame(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestTxFrame(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static TDLS_STATUS
TdlsCmdTestTxFmeSetupReqBufTranslate(
	UINT_8								*pCmdBuf,
	UINT_32								u4BufLen,
	PARAM_CUSTOM_TDLS_CMD_STRUC_T		*prCmd
	);

static void
TdlsCmdTestUpdatePeer(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdTestNullRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static VOID
TdlsTimerTestDataContSend(
	ADAPTER_T							*prAdapter,
	UINT_32								u4Param
    );

static TDLS_STATUS
TdlsTestChStReqRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestChStRspRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestFrameSend(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestNullRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestPtiReqRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestPtiRspRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestTearDownRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestDataRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestPtiTxFail(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestTdlsFrameSend(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestTxFailSkip(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestKeepAliveSkip(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestChSwTimeoutSkip(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsTestScanSkip(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsChSwConf(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static void
TdlsCmdChSwConf(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdInfoDisplay(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdKeyInfoDisplay(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdMibParamUpdate(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdSetupConf(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static void
TdlsCmdUapsdConf(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	);

static TDLS_STATUS
TdlsInfoDisplay(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsKeyInfoDisplay(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static VOID
TdlsLinkHistoryRecord(
	GLUE_INFO_T							*prGlueInfo,
	BOOLEAN								fgIsTearDown,
	UINT8								*pucPeerMac,
	BOOLEAN								fgIsFromUs,
	UINT16								u2ReasonCode,
	VOID								*prOthers
	);

static VOID
TdlsLinkHistoryRecordUpdate(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*pucPeerMac,
	TDLS_EVENT_HOST_SUBID_SPECIFIC_FRAME	eFmeStatus,
	VOID								*pInfo
	);

static TDLS_STATUS
TdlsMibParamUpdate(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsSetupConf(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static TDLS_STATUS
TdlsUapsdConf(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

static void
TdlsEventStatistics(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	);

static void
TdlsEventTearDown(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	);

#endif /*                   */

/*                                                                              
                              
                                                                                
*/
static BOOLEAN fgIsPtiTimeoutSkip = FALSE;


/*                                                                              
                                       
                                                                                
*/

/*                                                                            */
/* 
                                                                   
 
                                                         
                                                     
 
              
 
*/
/*                                                                            */
static VOID
TdlsCmdTestRxIndicatePkts (
	GLUE_INFO_T							*prGlueInfo,
	struct sk_buff						*prSkb
    )
{
    struct net_device *prNetDev;


	/*      */
	prNetDev = prGlueInfo->prDevHandler;
	prGlueInfo->rNetDevStats.rx_bytes += prSkb->len;
	prGlueInfo->rNetDevStats.rx_packets++;

	/*                     */
	prNetDev->last_rx = jiffies;
	prSkb->protocol = eth_type_trans(prSkb, prNetDev);
	prSkb->dev = prNetDev;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
    if(!in_interrupt()){
        netif_rx_ni(prSkb); /*                               */
    }
    else {
        netif_rx(prSkb);
    }
#else
    netif_rx(prSkb);
#endif
}


#if TDLS_CFG_CMD_TEST

#define LR_TDLS_FME_FIELD_FILL(__Len) \
	pPkt += __Len; \
	u4PktLen += __Len;

/*                                                                            */
/*                                                    
 
                                                         
                                                            
                                                
                  
 
              
 
                                                   

                                                
*/
/*                                                                            */
static void
TdlsCmdTestAddPeer(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	PARAM_CUSTOM_TDLS_CMD_STRUC_T rCmd;
	struct wireless_dev *prWdev;


	/*       */
	kalMemZero(&rCmd, sizeof(rCmd));

	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.arRspAddr);

	/*      */
	rCmd.rPeerInfo.supported_rates = NULL;
	rCmd.rPeerInfo.ht_capa = &rCmd.rHtCapa;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0)
	rCmd.rPeerInfo.vht_capa = &rCmd.rVhtCapa;
#endif
	rCmd.rPeerInfo.sta_flags_set = BIT(NL80211_STA_FLAG_TDLS_PEER);

	/*                                     */
	prWdev = prGlueInfo->prDevHandler->ieee80211_ptr;
	mtk_cfg80211_add_station(\
		prWdev->wiphy, (void *)0x1, rCmd.arRspAddr, &rCmd.rPeerInfo);
}


/*                                                                            */
/*                                                                            
 
                                                         
                                                            
                                                
                  
 
              
 
                                                                 

                                   
*/
/*                                                                            */
static void
TdlsCmdTestChSwProhibitedBitSet(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	extern BOOLEAN flgTdlsTestExtCapElm;
	extern UINT8 aucTdlsTestExtCapElm[];
	TDLS_CMD_CORE_T rCmd;


	/*                 */
	kalMemZero(rCmd.aucPeerMac, sizeof(rCmd.aucPeerMac));
	rCmd.Content.rCmdProhibit.fgIsEnable = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdProhibit.fgIsSet = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: fgIsEnable = %d\n",
		__FUNCTION__, rCmd.Content.rCmdProhibit.fgIsEnable));

	/*                    */
	flgTdlsTestExtCapElm = rCmd.Content.rCmdProhibit.fgIsEnable;

	aucTdlsTestExtCapElm[0] = ELEM_ID_EXTENDED_CAP;
	aucTdlsTestExtCapElm[1] = 5;
	aucTdlsTestExtCapElm[2] = 0;
	aucTdlsTestExtCapElm[3] = 0;
	aucTdlsTestExtCapElm[4] = 0;
	aucTdlsTestExtCapElm[5] = 0;
	aucTdlsTestExtCapElm[6] = (rCmd.Content.rCmdProhibit.fgIsSet << 7); /*       */
}


/*                                                                            */
/*                                                                                   
 
                                                        
                                                           
                                                
                  
 
              
 
                                                                               
                                                        

                                                                      

                                                               
                                                          
                                         
                                         
                                   

*/
/*                                                                            */
static void
TdlsCmdTestChSwReqRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.aucPeerMac);

	rCmd.Content.rCmdChStReqRcv.u4Chan = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChStReqRcv.u4RegClass = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChStReqRcv.u4SecChanOff = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChStReqRcv.u4SwitchTime = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChStReqRcv.u4SwitchTimeout = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: ["MACSTR"] u4Chan=%u u4RegClass=%u"
		" u4SecChanOff=%u u4SwitchTime=%u u4SwitchTimeout=%u\n",
		__FUNCTION__, MAC2STR(rCmd.aucPeerMac),
		(UINT32)rCmd.Content.rCmdChStReqRcv.u4Chan,
		(UINT32)rCmd.Content.rCmdChStReqRcv.u4RegClass,
		(UINT32)rCmd.Content.rCmdChStReqRcv.u4SecChanOff,
		(UINT32)rCmd.Content.rCmdChStReqRcv.u4SwitchTime,
		(UINT32)rCmd.Content.rCmdChStReqRcv.u4SwitchTimeout));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestChStReqRecv,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                                    
 
                                                        
                                                           
                                                
                  
 
              
 
                                                             
                                            

                                                                   

                                                               
                                                          
                                         
                                         
                                   

*/
/*                                                                            */
static void
TdlsCmdTestChSwRspRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.aucPeerMac);

	rCmd.Content.rCmdChStRspRcv.u4Chan = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChStRspRcv.u4SwitchTime = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChStRspRcv.u4SwitchTimeout = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChStRspRcv.u4StatusCode = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: ["MACSTR"] u4Chan=%u "
		" u4SwitchTime=%u u4SwitchTimeout=%u u4StatusCode=%u\n",
		__FUNCTION__, MAC2STR(rCmd.aucPeerMac),
		(UINT32)rCmd.Content.rCmdChStRspRcv.u4Chan,
		(UINT32)rCmd.Content.rCmdChStRspRcv.u4SwitchTime,
		(UINT32)rCmd.Content.rCmdChStRspRcv.u4SwitchTimeout,
		(UINT32)rCmd.Content.rCmdChStRspRcv.u4StatusCode));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestChStRspRecv,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                                            
 
                                                         
                                                            
                                                
                  
 
              
 
                                                     

                                 
*/
/*                                                                            */
static void
TdlsCmdTestChSwTimeoutSkip(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	kalMemZero(rCmd.aucPeerMac, sizeof(rCmd.aucPeerMac));
	rCmd.Content.rCmdKeepAliveSkip.fgIsEnable = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: fgIsEnable = %d\n",
		__FUNCTION__, rCmd.Content.rCmdKeepAliveSkip.fgIsEnable));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestChSwTimeoutSkip,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                               
 
                                                         
                                                            
                                                
                  
 
              
 
*/
/*                                                                            */
static TIMER_T rTdlsTimerTestDataSend;
static UINT_8 aucTdlsTestDataSPeerMac[6];
static UINT_16 u2TdlsTestDataSInterval;

static void
TdlsCmdTestDataContSend(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	ADAPTER_T *prAdapter;
	BOOLEAN fgIsEnabled;


	/*      */
	prAdapter = prGlueInfo->prAdapter;

	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, aucTdlsTestDataSPeerMac);
	u2TdlsTestDataSInterval = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	fgIsEnabled = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	cnmTimerStopTimer(prAdapter, &rTdlsTimerTestDataSend);

	if (fgIsEnabled == FALSE)
	{
		/*                 */
		return;
	}

	/*                    */
    cnmTimerInitTimer(prAdapter,
		&rTdlsTimerTestDataSend,
		(PFN_MGMT_TIMEOUT_FUNC)TdlsTimerTestDataContSend,
		(ULONG)NULL);

	cnmTimerStartTimer(prAdapter,
		&rTdlsTimerTestDataSend,
		u2TdlsTestDataSInterval);
}


/*                                                                            */
/*                                                                       
 
                                                        
                                                           
                                                
                  
 
              
 
                                                                                 

                                                            
*/
/*                                                                            */
static void
TdlsCmdTestDataRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.aucPeerMac);
	rCmd.Content.rCmdDatRcv.u4PM = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdDatRcv.u4UP = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdDatRcv.u4EOSP = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdDatRcv.u4IsNull = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO,
		("<tdls_cmd> %s: ["MACSTR"] PM(%u) UP(%u) EOSP(%u) NULL(%u)\n",
		__FUNCTION__, MAC2STR(rCmd.aucPeerMac),
		(UINT32)rCmd.Content.rCmdDatRcv.u4PM,
		(UINT32)rCmd.Content.rCmdDatRcv.u4UP,
		(UINT32)rCmd.Content.rCmdDatRcv.u4EOSP,
		(UINT32)rCmd.Content.rCmdDatRcv.u4IsNull));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestDataRecv,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                  
 
                                                        
                                                           
                                                
                  
 
              
 
                                                               

                                                  
*/
/*                                                                            */
static void
TdlsCmdTestDataSend(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	P_ADAPTER_T prAdapter;
    struct sk_buff *prMsduInfo;
	UINT_8 *prPkt;
	UINT_8 MAC[6];
	UINT_8 ucTxStatus;


	/*      */
	prAdapter = prGlueInfo->prAdapter;

	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, MAC);
	ucTxStatus = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	/*                       */
	prMsduInfo = kalPacketAlloc(prGlueInfo, 1000, &prPkt);
	if (prMsduInfo == NULL)
	{
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s allocate pkt fail!\n",
			__FUNCTION__));
		return;
	}

	/*          */
    prMsduInfo->dev = prGlueInfo->prDevHandler;
    if (prMsduInfo->dev == NULL)
    {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s prMsduInfo->dev == NULL!\n",
			__FUNCTION__));
        kalPacketFree(prGlueInfo, prMsduInfo);
        return;
    }

	/*             */
	prMsduInfo->len = 1000;
	kalMemZero(prMsduInfo->data, 100); /*               */
	kalMemCopy(prMsduInfo->data, MAC, 6);
	kalMemCopy(prMsduInfo->data+6, prAdapter->rMyMacAddr, 6);
	*(UINT_16 *)(prMsduInfo->data+12) = 0x0800;

    /*                                */
	wlanHardStartXmit(prMsduInfo, prMsduInfo->dev);
}


/*                                                                            */
/*                                                                            
 
                                                         
                                                            
                                                
                  
 
              
 
                                                   

                                    
*/
/*                                                                            */
static void
TdlsCmdTestDelay(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	UINT32 u4Delay;


	u4Delay = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	DBGLOG(TDLS, INFO, ("%s: Delay = %d\n",
		__FUNCTION__, u4Delay));

	kalMdelay(u4Delay);
}


/*                                                                            */
/*                                                                                   
 
                                                         
                                                            
                                                
                  
 
              
 
                                                                       

                                                     
                                                                       
*/
/*                                                                            */
static void
TdlsCmdTestDiscoveryReqRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	ADAPTER_T *prAdapter;
    P_BSS_INFO_T prBssInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;
	UINT_8 ucDialogToken, aucPeerMac[6], aucBSSID[6], aucZeroMac[6];


	/*                 */
	ucDialogToken = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, aucPeerMac);

	kalMemZero(aucZeroMac, sizeof(aucZeroMac));
	kalMemZero(aucBSSID, sizeof(aucBSSID));
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, aucBSSID);

	DBGLOG(TDLS, INFO,
		("<tdls_fme> %s: DialogToken=%d from "MACSTR"\n",
		__FUNCTION__, ucDialogToken, MAC2STR(aucPeerMac)));

	/*                      */
	prAdapter = prGlueInfo->prAdapter;
	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	u4PktLen = 0;

    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: allocate pkt fail\n", __FUNCTION__));
        return;
    }

    prMsduInfo->dev = prGlueInfo->prDevHandler;
    if (prMsduInfo->dev == NULL)
    {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: MsduInfo->dev == NULL\n", __FUNCTION__));
        kalPacketFree(prGlueInfo, prMsduInfo);
        return;
    }

	/*                       */
	/*                 */
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	kalMemCopy(pPkt, aucPeerMac, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	*(UINT_16 *)pPkt = htons(TDLS_FRM_PROT_TYPE);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                 */
	*pPkt = TDLS_FRM_ACTION_DISCOVERY_REQ;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                       */
	*pPkt = ucDialogToken;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                                   */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = 18;

	if (kalMemCmp(aucBSSID, aucZeroMac, 6) == 0)
		kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	else
		kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, aucBSSID, 6);

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, aucPeerMac, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, prAdapter->rMyMacAddr, 6);

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                         */
	prMsduInfo->len = u4PktLen;
	dumpMemory8(ANDROID_LOG_INFO, prMsduInfo->data, u4PktLen);

	/*            */
	TdlsCmdTestRxIndicatePkts(prGlueInfo, prMsduInfo);
}


/*                                                                            */
/*                                                                                
 
                                                         
                                                            
                                                
                  
 
              
 
                                                     

                                 
*/
/*                                                                            */
static void
TdlsCmdTestKeepAliveSkip(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	kalMemZero(rCmd.aucPeerMac, sizeof(rCmd.aucPeerMac));
	rCmd.Content.rCmdKeepAliveSkip.fgIsEnable = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: fgIsEnable = %d\n",
		__FUNCTION__, rCmd.Content.rCmdKeepAliveSkip.fgIsEnable));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestKeepAliveSkip,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                            
 
                                                         
                                                            
                                                
                  
 
              
 
                                                                 

                                   
*/
/*                                                                            */
static void
TdlsCmdTestProhibitedBitSet(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	extern BOOLEAN flgTdlsTestExtCapElm;
	extern UINT8 aucTdlsTestExtCapElm[];
	TDLS_CMD_CORE_T rCmd;


	/*                 */
	kalMemZero(rCmd.aucPeerMac, sizeof(rCmd.aucPeerMac));
	rCmd.Content.rCmdProhibit.fgIsEnable = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdProhibit.fgIsSet = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: fgIsEnable = %d\n",
		__FUNCTION__, rCmd.Content.rCmdProhibit.fgIsEnable));

	/*                    */
	flgTdlsTestExtCapElm = rCmd.Content.rCmdProhibit.fgIsEnable;

	aucTdlsTestExtCapElm[0] = ELEM_ID_EXTENDED_CAP;
	aucTdlsTestExtCapElm[1] = 5;
	aucTdlsTestExtCapElm[2] = 0;
	aucTdlsTestExtCapElm[3] = 0;
	aucTdlsTestExtCapElm[4] = 0;
	aucTdlsTestExtCapElm[5] = 0;
	aucTdlsTestExtCapElm[6] = (rCmd.Content.rCmdProhibit.fgIsSet << 6); /*       */
}


/*                                                                            */
/*                                                                      
 
                                                        
                                                           
                                                
                  
 
              
 
                                                                    

                                                    
*/
/*                                                                            */
static void
TdlsCmdTestPtiReqRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.aucPeerMac);

	rCmd.Content.rCmdPtiRspRcv.u4DialogToken = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: ["MACSTR"] u4DialogToken = %u\n",
		__FUNCTION__, MAC2STR(rCmd.aucPeerMac),
		(UINT32)rCmd.Content.rCmdPtiRspRcv.u4DialogToken));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestPtiReqRecv,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                         
 
                                                        
                                                           
                                                
                  
 
              
 
                                                                         

                                                      
*/
/*                                                                            */
static void
TdlsCmdTestPtiRspRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.aucPeerMac);

	rCmd.Content.rCmdPtiRspRcv.u4DialogToken = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdPtiRspRcv.u4PM = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: ["MACSTR"] u4DialogToken = %u %u\n",
		__FUNCTION__, MAC2STR(rCmd.aucPeerMac),
		(UINT32)rCmd.Content.rCmdPtiRspRcv.u4DialogToken,
		(UINT32)rCmd.Content.rCmdPtiRspRcv.u4PM));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestPtiRspRecv,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                                      
 
                                                         
                                                            
                                                
                  
 
              
 
                                                     

                                 
*/
/*                                                                            */
static void
TdlsCmdTestPtiTxDoneFail(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	kalMemZero(rCmd.aucPeerMac, sizeof(rCmd.aucPeerMac));
	rCmd.Content.rCmdPtiTxFail.fgIsEnable = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: fgIsEnable = %d\n",
		__FUNCTION__, rCmd.Content.rCmdPtiTxFail.fgIsEnable));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestPtiTxFail,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                         
 
                                                         
                                                            
                                                
                  
 
              
 
*/
/*                                                                            */
static void
TdlsCmdTestRvFrame(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
//                                    
//                      
	UINT_32 u4Subcmd;
//                  


	/*                   */
	u4Subcmd = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	DBGLOG(TDLS, INFO, ("<tdls_cmd> test rv frame sub command = %u\n",
		(UINT32)u4Subcmd));

	/*                         */
	switch(u4Subcmd)
	{
		case TDLS_FRM_ACTION_SETUP_REQ:
			/*                                           */
			TdlsCmdTestSetupReqRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_FRM_ACTION_SETUP_RSP:
			/*                                            */
			TdlsCmdTestSetupRspRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_FRM_ACTION_CONFIRM:
			/*                                           */
			TdlsCmdTestSetupConfirmRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_FRM_ACTION_TEARDOWN:
			/*                                       */
			TdlsCmdTestTearDownRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_FRM_ACTION_PTI:
			/*                                         */
			TdlsCmdTestPtiReqRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_FRM_ACTION_PTI_RSP:
			/*                                          */
			TdlsCmdTestPtiRspRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_FRM_DATA_TEST_DATA:
			/*                                  */
			TdlsCmdTestDataRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_FRM_ACTION_CHAN_SWITCH_REQ:
			/*                                                    */
			TdlsCmdTestChSwReqRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_FRM_ACTION_CHAN_SWITCH_RSP:
			/*                                                     */
			TdlsCmdTestChSwRspRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_FRM_ACTION_DISCOVERY_REQ:
			/*                                               */
			TdlsCmdTestDiscoveryReqRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		default:
			DBGLOG(TDLS, ERROR, ("<tdls_cmd> wrong test rv frame sub command\n"));
			return;
	}

//                                     
	{
//                                                           
//         
	}

	/*                                     */
#if 0
	kalIoctl(prGlueInfo,
			TdlsTestFrameSend,
			(PVOID)&rCmd,
			sizeof(PARAM_CUSTOM_TDLS_CMD_STRUC_T),
			FALSE,
			FALSE,
			TRUE,
			FALSE,
			&u4BufLen);
#endif
}


/*                                                                            */
/*                                                                               
 
                                                         
                                                            
                                                
                  
 
              
 
                                                                           

                                                      
*/
/*                                                                            */
static void
TdlsCmdTestSetupConfirmRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	ADAPTER_T *prAdapter;
    P_BSS_INFO_T prBssInfo;
	PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;
	UINT_8 ucDialogToken, ucStatusCode, aucPeerMac[6];


	/*                 */
	ucDialogToken = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	ucStatusCode = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, aucPeerMac);

	DBGLOG(TDLS, INFO,
		("<tdls_fme> %s: DialogToken=%d StatusCode=%d from "MACSTR"\n",
		__FUNCTION__, ucDialogToken, ucStatusCode, MAC2STR(aucPeerMac)));

	/*                      */
	prAdapter = prGlueInfo->prAdapter;
	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
	u4PktLen = 0;

    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: allocate pkt fail\n", __FUNCTION__));
        return;
    }

    prMsduInfo->dev = prGlueInfo->prDevHandler;
    if (prMsduInfo->dev == NULL)
    {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: MsduInfo->dev == NULL\n", __FUNCTION__));
        kalPacketFree(prGlueInfo, prMsduInfo);
        return;
    }

	/*                       */
	/*                 */
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	kalMemCopy(pPkt, aucPeerMac, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	*(UINT_16 *)pPkt = htons(TDLS_FRM_PROT_TYPE);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                 */
	*pPkt = TDLS_FRM_ACTION_CONFIRM;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                      */
	*pPkt = ucStatusCode;
	*(pPkt+1) = 0x00;
	LR_TDLS_FME_FIELD_FILL(2);

	/*                                       */
	*pPkt = ucDialogToken;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                                   */
	if (prAdapter->rWifiVar.fgSupportQoS)
	{
		u4IeLen = mqmGenerateWmmParamIEByParam(\
							prAdapter, prBssInfo, pPkt);
		LR_TDLS_FME_FIELD_FILL(u4IeLen);
	}

	/*                                                   */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = ELEM_LEN_LINK_IDENTIFIER;

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, aucPeerMac, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, prAdapter->rMyMacAddr, 6);

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                         */
	prMsduInfo->len = u4PktLen;
	dumpMemory8(ANDROID_LOG_INFO, prMsduInfo->data, u4PktLen);

	/*            */
	TdlsCmdTestRxIndicatePkts(prGlueInfo, prMsduInfo);
}


/*                                                                            */
/*                                                                               
 
                                                         
                                                            
                                                
                  
 
              
 
                                                                      

                                                    
                                                                      
*/
/*                                                                            */
static void
TdlsCmdTestSetupReqRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	ADAPTER_T *prAdapter;
    P_BSS_INFO_T prBssInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;
	UINT_8 ucDialogToken, aucPeerMac[6], aucBSSID[6], aucZeroMac[6];
	UINT_16 u2CapInfo;


	/*                 */
	ucDialogToken = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, aucPeerMac);

	kalMemZero(aucZeroMac, sizeof(aucZeroMac));
	kalMemZero(aucBSSID, sizeof(aucBSSID));
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, aucBSSID);

	DBGLOG(TDLS, INFO,
		("<tdls_fme> %s: DialogToken=%d from "MACSTR"\n",
		__FUNCTION__, ucDialogToken, MAC2STR(aucPeerMac)));

	/*                      */
	prAdapter = prGlueInfo->prAdapter;
	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	u4PktLen = 0;

    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: allocate pkt fail\n", __FUNCTION__));
        return;
    }

    prMsduInfo->dev = prGlueInfo->prDevHandler;
    if (prMsduInfo->dev == NULL)
    {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: MsduInfo->dev == NULL\n", __FUNCTION__));
        kalPacketFree(prGlueInfo, prMsduInfo);
        return;
    }

	/*                       */
	/*                 */
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	kalMemCopy(pPkt, aucPeerMac, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	*(UINT_16 *)pPkt = htons(TDLS_FRM_PROT_TYPE);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                 */
	*pPkt = TDLS_FRM_ACTION_SETUP_REQ;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                       */
	*pPkt = ucDialogToken;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                     */
	u2CapInfo = assocBuildCapabilityInfo(prAdapter, NULL);
	WLAN_SET_FIELD_16(pPkt, u2CapInfo);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                       */
	u4IeLen = TdlsFrameGeneralIeAppend(prAdapter, NULL, 0, pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                                                         */
	EXT_CAP_IE(pPkt)->ucId = ELEM_ID_EXTENDED_CAP;
	EXT_CAP_IE(pPkt)->ucLength = 5;

	EXT_CAP_IE(pPkt)->aucCapabilities[0] = 0x00; /*             */
	EXT_CAP_IE(pPkt)->aucCapabilities[1] = 0x00; /*              */
	EXT_CAP_IE(pPkt)->aucCapabilities[2] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[3] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[4] = 0x00; /*               */

	/*                        */
		EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((28-24));
	/*                         */
		EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((30-24));
	/*                  */
		EXT_CAP_IE(pPkt)->aucCapabilities[4] |= BIT((37-32));

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                                                   */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = 18;

	if (kalMemCmp(aucBSSID, aucZeroMac, 6) == 0)
		kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	else
		kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, aucBSSID, 6);

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, aucPeerMac, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, prAdapter->rMyMacAddr, 6);

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                         */
	prMsduInfo->len = u4PktLen;
	dumpMemory8(ANDROID_LOG_INFO, prMsduInfo->data, u4PktLen);

	/*            */
	TdlsCmdTestRxIndicatePkts(prGlueInfo, prMsduInfo);
}


/*                                                                            */
/*                                                                                
 
                                                         
                                                            
                                                
                  
 
              
 
                                                                           

                                                      
*/
/*                                                                            */
static void
TdlsCmdTestSetupRspRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	ADAPTER_T *prAdapter;
    P_BSS_INFO_T prBssInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;
	UINT_8 ucDialogToken, ucStatusCode, aucPeerMac[6];
	UINT_16 u2CapInfo;


	/*                 */
	ucDialogToken = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	ucStatusCode = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, aucPeerMac);

	DBGLOG(TDLS, INFO,
		("<tdls_fme> %s: DialogToken=%d StatusCode=%d from "MACSTR"\n",
		__FUNCTION__, ucDialogToken, ucStatusCode, MAC2STR(aucPeerMac)));

	/*                      */
	prAdapter = prGlueInfo->prAdapter;
	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	u4PktLen = 0;

    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: allocate pkt fail\n", __FUNCTION__));
        return;
    }

    prMsduInfo->dev = prGlueInfo->prDevHandler;
    if (prMsduInfo->dev == NULL)
    {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: MsduInfo->dev == NULL\n", __FUNCTION__));
        kalPacketFree(prGlueInfo, prMsduInfo);
        return;
    }

	/*                       */
	/*                 */
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	kalMemCopy(pPkt, aucPeerMac, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	*(UINT_16 *)pPkt = htons(TDLS_FRM_PROT_TYPE);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                 */
	*pPkt = TDLS_FRM_ACTION_SETUP_RSP;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                      */
	*pPkt = ucStatusCode;
	*(pPkt+1) = 0x00;
	LR_TDLS_FME_FIELD_FILL(2);

	/*                                       */
	*pPkt = ucDialogToken;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                     */
	u2CapInfo = assocBuildCapabilityInfo(prAdapter, NULL);
	WLAN_SET_FIELD_16(pPkt, u2CapInfo);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                       */
	u4IeLen = TdlsFrameGeneralIeAppend(prAdapter, NULL, 0, pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                                                         */
	EXT_CAP_IE(pPkt)->ucId = ELEM_ID_EXTENDED_CAP;
	EXT_CAP_IE(pPkt)->ucLength = 5;

	EXT_CAP_IE(pPkt)->aucCapabilities[0] = 0x00; /*             */
	EXT_CAP_IE(pPkt)->aucCapabilities[1] = 0x00; /*              */
	EXT_CAP_IE(pPkt)->aucCapabilities[2] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[3] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[4] = 0x00; /*               */

	/*                        */
		EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((28-24));
	/*                         */
		EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((30-24));
	/*                  */
		EXT_CAP_IE(pPkt)->aucCapabilities[4] |= BIT((37-32));

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                                                   */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = ELEM_LEN_LINK_IDENTIFIER;

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, prAdapter->rMyMacAddr, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, aucPeerMac, 6);

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                         */
	prMsduInfo->len = u4PktLen;
	dumpMemory8(ANDROID_LOG_INFO, prMsduInfo->data, u4PktLen);

	/*            */
	TdlsCmdTestRxIndicatePkts(prGlueInfo, prMsduInfo);
}


/*                                                                            */
/*                                                                                            
 
                                                         
                                                            
                                                
                  
 
              
 
                                                     

                                 
*/
/*                                                                            */
static void
TdlsCmdTestScanCtrl(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	kalMemZero(rCmd.aucPeerMac, sizeof(rCmd.aucPeerMac));
	rCmd.Content.rCmdScanSkip.fgIsEnable = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: fgIsEnable = %d\n",
		__FUNCTION__, rCmd.Content.rCmdScanSkip.fgIsEnable));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestScanSkip,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                           
 
                                                         
                                                            
                                                
                  
 
              
 
                                                                                   

                                      

                                                         
*/
/*                                                                            */
static void
TdlsCmdTestTearDownRecv(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	ADAPTER_T *prAdapter;
    P_BSS_INFO_T prBssInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;
	BOOLEAN fgIsInitiator;
	UINT_8 ucReasonCode, aucPeerMac[6];
	BOOLEAN fgIsFromWhich;
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	fgIsInitiator = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	ucReasonCode = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, aucPeerMac);
	fgIsFromWhich = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO,
		("<tdls_fme> %s: ReasonCode=%d from "MACSTR" %d\n",
		__FUNCTION__, ucReasonCode, MAC2STR(aucPeerMac), fgIsFromWhich));

	if (fgIsFromWhich == 0)
	{
		/*                      */
		prAdapter = prGlueInfo->prAdapter;
		prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
		u4PktLen = 0;

	    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
	    if (prMsduInfo == NULL) {
			DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: allocate pkt fail\n", __FUNCTION__));
	        return;
	    }

	    prMsduInfo->dev = prGlueInfo->prDevHandler;
	    if (prMsduInfo->dev == NULL)
	    {
			DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: MsduInfo->dev == NULL\n", __FUNCTION__));
	        kalPacketFree(prGlueInfo, prMsduInfo);
	        return;
	    }

		/*                       */
		/*                 */
		kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
		LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
		kalMemCopy(pPkt, aucPeerMac, TDLS_FME_MAC_ADDR_LEN);
		LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
		*(UINT_16 *)pPkt = htons(TDLS_FRM_PROT_TYPE);
		LR_TDLS_FME_FIELD_FILL(2);

		/*                 */
		*pPkt = TDLS_FRM_PAYLOAD_TYPE;
		LR_TDLS_FME_FIELD_FILL(1);

		/*                                   */
		*pPkt = TDLS_FRM_CATEGORY;
		LR_TDLS_FME_FIELD_FILL(1);

		/*                                 */
		*pPkt = TDLS_FRM_ACTION_TEARDOWN;
		LR_TDLS_FME_FIELD_FILL(1);

		/*                                      */
		*pPkt = ucReasonCode;
		*(pPkt+1) = 0x00;
		LR_TDLS_FME_FIELD_FILL(2);

		/*                                                   */
		TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
		TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = ELEM_LEN_LINK_IDENTIFIER;

		kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
		if (fgIsInitiator == 1)
		{
			kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, aucPeerMac, 6);
			kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, prAdapter->rMyMacAddr, 6);
		}
		else
		{
			kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, prAdapter->rMyMacAddr, 6);
			kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, aucPeerMac, 6);
		}

		u4IeLen = IE_SIZE(pPkt);
		LR_TDLS_FME_FIELD_FILL(u4IeLen);

		/*                         */
		prMsduInfo->len = u4PktLen;
		dumpMemory8(ANDROID_LOG_INFO, prMsduInfo->data, u4PktLen);

		/*            */
		TdlsCmdTestRxIndicatePkts(prGlueInfo, prMsduInfo);
	}
	else
	{
		kalMemZero(&rCmd, sizeof(rCmd));
		kalMemCopy(rCmd.aucPeerMac, aucPeerMac, 6);
		rCmd.Content.rCmdTearDownRcv.u4ReasonCode = (UINT32)ucReasonCode;
		
		/*                    */
		rStatus = kalIoctl(prGlueInfo,
			TdlsTestTearDownRecv,
			&rCmd,
			sizeof(rCmd),
			FALSE,
			FALSE,
			FALSE,
			FALSE,
			&u4BufLen);
		
		if (rStatus != WLAN_STATUS_SUCCESS)
		{
			DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
			return;
		}
	}
}


/*                                                                            */
/*                                                                         
 
                                                         
                                                            
                                                
                  
 
              
 
                                                    

                                
*/
/*                                                                            */
static void
TdlsCmdTestTxFailSkip(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	kalMemZero(rCmd.aucPeerMac, sizeof(rCmd.aucPeerMac));
	rCmd.Content.rCmdTxFailSkip.fgIsEnable = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: fgIsEnable = %d\n",
		__FUNCTION__, rCmd.Content.rCmdTxFailSkip.fgIsEnable));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestTxFailSkip,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                           
 
                                                         
                                                            
                                                
                  
 
              
 
                                                                           

                                                       
 
                                                                           

                                                       
*/
/*                                                                            */
static void
TdlsCmdTestTxTdlsFrame(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	PARAM_CUSTOM_TDLS_CMD_STRUC_T rCmd;
	UINT32 u4Subcmd;
	UINT_32 u4BufLen;


	/*                   */
	u4Subcmd = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	DBGLOG(TDLS, INFO, ("<tdls_cmd> test tx tdls frame sub command = %u\n",
		u4Subcmd));

	/*                         */
	rCmd.ucFmeType = CmdStringDecParse(prInBuf, &prInBuf, &u4BufLen);

	switch(u4Subcmd)
	{
		case TDLS_FRM_ACTION_SETUP_REQ:
		case TDLS_FRM_ACTION_SETUP_RSP:
		case TDLS_FRM_ACTION_CONFIRM:
			rCmd.ucToken = CmdStringDecParse(prInBuf, &prInBuf, &u4BufLen);
			CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.arRspAddr);

			DBGLOG(TDLS, INFO, ("<tdls_cmd> setup FmeType=%d Token=%d to "
				"["MACSTR"]\n",
				rCmd.ucFmeType, rCmd.ucToken,
				MAC2STR(rCmd.arRspAddr)));
			break;

		default:
			DBGLOG(TDLS, ERROR, ("<tdls_cmd> wrong test tx frame sub command\n"));
			return;
	}

	/*                                     */
	kalIoctl(prGlueInfo,
			TdlsTestTdlsFrameSend,
			(PVOID)&rCmd,
			sizeof(PARAM_CUSTOM_TDLS_CMD_STRUC_T),
			FALSE,
			FALSE,
			TRUE,
			FALSE,
			&u4BufLen);
}


/*                                                                            */
/*                                                                           
 
                                                         
                                                            
                                                
                  
 
              
 
                                                                              
                                              
                                              
                      

                                                                              
*/
/*                                                                            */
static void
TdlsCmdTestTxFrame(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	PARAM_CUSTOM_TDLS_CMD_STRUC_T rCmd;
	TDLS_STATUS u4Status;
	UINT_32 u4Subcmd;
	UINT_32 u4BufLen;


	/*                   */
	u4Subcmd = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	DBGLOG(TDLS, INFO, ("<tdls_cmd> test tx frame sub command = %u\n",
		(UINT32)u4Subcmd));

	/*                         */
	switch(u4Subcmd)
	{
		case TDLS_FRM_ACTION_SETUP_REQ:
			u4Status = TdlsCmdTestTxFmeSetupReqBufTranslate(
												prInBuf, u4InBufLen, &rCmd);
			break;

		default:
			DBGLOG(TDLS, ERROR, ("<tdls_cmd> wrong test tx frame sub command\n"));
			return;
	}

	if (u4Status != TDLS_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> command parse fail\n"));
		return;
	}

	/*                                     */
	kalIoctl(prGlueInfo,
			TdlsTestFrameSend,
			(PVOID)&rCmd,
			sizeof(PARAM_CUSTOM_TDLS_CMD_STRUC_T),
			FALSE,
			FALSE,
			TRUE,
			FALSE,
			&u4BufLen);
}


/*                                                                            */
/* 
                                                         
 
                                       
                                      
                                           
 
                                            
                                              
                                                  
 
                                                                              
                                              
                                              
                      

                                                                              
*/
/*                                                                            */
static TDLS_STATUS
TdlsCmdTestTxFmeSetupReqBufTranslate(
	UINT_8								*pCmdBuf,
	UINT_32								u4BufLen,
	PARAM_CUSTOM_TDLS_CMD_STRUC_T		*prCmd
	)
{
//                                                  

	prCmd->ucFmeType = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->ucToken = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->u2Cap = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->ucExCap = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->arSupRate[0] = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->arSupRate[1] = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->arSupRate[2] = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->arSupRate[3] = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->arSupChan[0] = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->arSupChan[1] = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->arSupChan[2] = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->arSupChan[3] = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	prCmd->u4Timeout = CmdStringDecParse(pCmdBuf, &pCmdBuf, &u4BufLen);
	CmdStringMacParse(pCmdBuf, &pCmdBuf, &u4BufLen, prCmd->arRspAddr);

	DBGLOG(TDLS, INFO, ("<tdls_cmd> command content =\n"));
	DBGLOG(TDLS, INFO, ("\tPeer MAC = "MACSTR"\n", MAC2STR(prCmd->arRspAddr)));
	DBGLOG(TDLS, INFO, ("\tToken = %u, Cap = 0x%x, ExCap = 0x%x, Timeout = %us "
			"FrameType = %u\n",
			(UINT32)prCmd->ucToken, prCmd->u2Cap, prCmd->ucExCap,
			(UINT32)prCmd->u4Timeout, (UINT32)prCmd->ucFmeType));
	DBGLOG(TDLS, INFO, ("\tSupRate = 0x%x %x %x %x\n",
			prCmd->arSupRate[0], prCmd->arSupRate[1],
			prCmd->arSupRate[2], prCmd->arSupRate[3]));
	DBGLOG(TDLS, INFO, ("\tSupChan = %d %d %d %d\n",
			prCmd->arSupChan[0], prCmd->arSupChan[1],
			prCmd->arSupChan[2], prCmd->arSupChan[3]));

	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                       
 
                                                         
                                                            
                                                
                  
 
              
 
                                                   

                                                
*/
/*                                                                            */
static void
TdlsCmdTestUpdatePeer(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	PARAM_CUSTOM_TDLS_CMD_STRUC_T rCmd;
	struct wireless_dev *prWdev;


	/*       */
	kalMemZero(&rCmd, sizeof(rCmd));

	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.arRspAddr);

	/*      */
	rCmd.rPeerInfo.supported_rates = rCmd.arSupRate;
	rCmd.rPeerInfo.ht_capa = &rCmd.rHtCapa;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0)
	rCmd.rPeerInfo.vht_capa = &rCmd.rVhtCapa;
#endif
	rCmd.rPeerInfo.sta_flags_set = BIT(NL80211_STA_FLAG_TDLS_PEER);
	rCmd.rPeerInfo.uapsd_queues = 0xf; /*        */
	rCmd.rPeerInfo.max_sp = 0; /*                      */

	/*                                     */
	prWdev = prGlueInfo->prDevHandler->ieee80211_ptr;
	mtk_cfg80211_add_station(\
		prWdev->wiphy, (void *)0x1, rCmd.arRspAddr, &rCmd.rPeerInfo);

	/*        */
	TdlsexCfg80211TdlsOper(\
		prWdev->wiphy, (void *)0x1, rCmd.arRspAddr, NL80211_TDLS_ENABLE_LINK);
}


/*                                                                            */
/*                                                                       
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
                                                            

                                                  
*/
/*                                                                            */
static void
TdlsCmdTestNullRecv(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.aucPeerMac);
	rCmd.Content.rCmdNullRcv.u4PM = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: ["MACSTR"] u4PM = %u\n",
		__FUNCTION__, MAC2STR(rCmd.aucPeerMac),
		(UINT32)rCmd.Content.rCmdNullRcv.u4PM));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsTestNullRecv,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                               
 
                                                        
                            
                  
 
              
 
                                                                                    

                                                        
*/
/*                                                                            */
static VOID
TdlsTimerTestDataContSend(
	ADAPTER_T							*prAdapter,
	UINT_32								u4Param
    )
{
	GLUE_INFO_T	*prGlueInfo;
    struct sk_buff *prMsduInfo;
	UINT_8 *prPkt;


	/*      */
	prGlueInfo = prAdapter->prGlueInfo;

	/*                       */
	prMsduInfo = kalPacketAlloc(prGlueInfo, 1000, &prPkt);
	if (prMsduInfo == NULL)
	{
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s allocate pkt fail!\n",
			__FUNCTION__));
		return;
	}

	/*          */
    prMsduInfo->dev = prGlueInfo->prDevHandler;
    if (prMsduInfo->dev == NULL)
    {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s prMsduInfo->dev == NULL!\n",
			__FUNCTION__));
        kalPacketFree(prGlueInfo, prMsduInfo);
        return;
    }

	/*             */
	prMsduInfo->len = 1000;
	kalMemCopy(prMsduInfo->data, aucTdlsTestDataSPeerMac, 6);
	kalMemCopy(prMsduInfo->data+6, prAdapter->rMyMacAddr, 6);
	*(UINT_16 *)(prMsduInfo->data+12) = 0x0800;

	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s try to send a data frame to "MACSTR"\n",
		__FUNCTION__, MAC2STR(aucTdlsTestDataSPeerMac)));

    /*                                */
	wlanHardStartXmit(prMsduInfo, prMsduInfo->dev);

	/*                    */
	cnmTimerStartTimer(prAdapter,
		&rTdlsTimerTestDataSend,
		u2TdlsTestDataSInterval);
}


/*                                                                            */
/*                                                                           
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestChStReqRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_CHSW_REQ;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                                            
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestChStRspRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_CHSW_RSP;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                     
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestFrameSend(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	GLUE_INFO_T *prGlueInfo;
	PARAM_CUSTOM_TDLS_CMD_STRUC_T *prCmd;
    P_BSS_INFO_T prBssInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;


	/*              */
	ASSERT(prAdapter);
	ASSERT(pvSetBuffer);
	ASSERT(pu4SetInfoLen);

	DBGLOG(TDLS, INFO, ("<tdls_fme> %s\n", __FUNCTION__));

	if(u4SetBufferLen == 0)
		return TDLS_STATUS_INVALID_LENGTH;

	/*                      */
	prGlueInfo = (GLUE_INFO_T *)prAdapter->prGlueInfo;
	prCmd = (PARAM_CUSTOM_TDLS_CMD_STRUC_T *)pvSetBuffer;
	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	*pu4SetInfoLen = u4SetBufferLen;
	u4PktLen = 0;

    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: allocate pkt fail\n", __FUNCTION__));
        return TDLS_STATUS_RESOURCES;
    }

    prMsduInfo->dev = prGlueInfo->prDevHandler;
    if (prMsduInfo->dev == NULL)
    {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: MsduInfo->dev == NULL\n", __FUNCTION__));
        kalPacketFree(prGlueInfo, prMsduInfo);
        return TDLS_STATUS_FAILURE;
    }

	/*                       */
	/*                 */
	kalMemCopy(pPkt, prCmd->arRspAddr, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	*(UINT_16 *)pPkt = htons(TDLS_FRM_PROT_TYPE);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                 */
	*pPkt = prCmd->ucFmeType;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                       */
	*pPkt = prCmd->ucToken;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                     */
	WLAN_SET_FIELD_16(pPkt, prCmd->u2Cap);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                       */
	u4IeLen = TdlsFrameGeneralIeAppend(prAdapter, NULL, 0, pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                                                         */
	EXT_CAP_IE(pPkt)->ucId = ELEM_ID_EXTENDED_CAP;
	EXT_CAP_IE(pPkt)->ucLength = 5;

	EXT_CAP_IE(pPkt)->aucCapabilities[0] = 0x00; /*             */
	EXT_CAP_IE(pPkt)->aucCapabilities[1] = 0x00; /*              */
	EXT_CAP_IE(pPkt)->aucCapabilities[2] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[3] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[4] = 0x00; /*               */

	if (prCmd->ucExCap & TDLS_EX_CAP_PEER_UAPSD)
		EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((28-24));
	if (prCmd->ucExCap & TDLS_EX_CAP_CHAN_SWITCH)
		EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((30-24));
	if (prCmd->ucExCap & TDLS_EX_CAP_TDLS)
		EXT_CAP_IE(pPkt)->aucCapabilities[4] |= BIT((37-32));

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                                                                       */
	TIMEOUT_INTERVAL_IE(pPkt)->ucId = ELEM_ID_TIMEOUT_INTERVAL;
	TIMEOUT_INTERVAL_IE(pPkt)->ucLength = 5;

	TIMEOUT_INTERVAL_IE(pPkt)->ucType = IE_TIMEOUT_INTERVAL_TYPE_KEY_LIFETIME;
	TIMEOUT_INTERVAL_IE(pPkt)->u4Value = htonl(prCmd->u4Timeout);

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                                                   */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = ELEM_LEN_LINK_IDENTIFIER;

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, prAdapter->rMyMacAddr, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, prCmd->arRspAddr, 6);

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                         */
	prMsduInfo->len = u4PktLen;
	dumpMemory8(ANDROID_LOG_INFO, prMsduInfo->data, u4PktLen);

	/*                        */
	wlanHardStartXmit(prMsduInfo, prMsduInfo->dev);
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                         
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestNullRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_NULL_RCV;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                        
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestPtiReqRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_PTI_REQ;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                                 
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestPtiRspRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_PTI_RSP;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                              
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestTearDownRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_TEAR_DOWN;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                         
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestDataRecv(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_DATA_RCV;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                            
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestPtiTxFail(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_PTI_TX_FAIL;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}



/*                                                                            */
/* 
                                                            
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
                                                                           

                                                       
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestTdlsFrameSend(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	GLUE_INFO_T *prGlueInfo;
	PARAM_CUSTOM_TDLS_CMD_STRUC_T *prCmd;
	struct wireless_dev *prWdev;


	/*              */
	ASSERT(prAdapter);
	ASSERT(pvSetBuffer);
	ASSERT(pu4SetInfoLen);

	DBGLOG(TDLS, INFO, ("<tdls_fme> %s\n", __FUNCTION__));

	if(u4SetBufferLen == 0)
		return TDLS_STATUS_INVALID_LENGTH;

	/*                      */
	prGlueInfo = (GLUE_INFO_T *)prAdapter->prGlueInfo;
	prCmd = (PARAM_CUSTOM_TDLS_CMD_STRUC_T *)pvSetBuffer;
	prWdev = (struct wireless_dev *)prGlueInfo->prDevHandler->ieee80211_ptr;

	TdlsexCfg80211TdlsMgmt(prWdev->wiphy,
		NULL,
		prCmd->arRspAddr,
		prCmd->ucFmeType,
		1,
		0,
		NULL, /*           */
		0);

	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                                                                  
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestTxFailSkip(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_TX_FAIL_SKIP;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}



/*                                                                            */
/*                                                                               
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestKeepAliveSkip(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_KEEP_ALIVE_SKIP;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                                
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestChSwTimeoutSkip(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_CHSW_TIMEOUT_SKIP;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                      
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsTestScanSkip(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_TEST_SCAN_SKIP;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


#endif /*                   */


#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0)
/*                                                                            */
/*                                                                    
 
                                                        
                                                           
                                                
                  
 
              
 
*/
/*                                                                            */
void cfg80211_tdls_oper_request(
	struct net_device					*dev,
	const u8							*peer,
	u16									oper,
	u16									reason_code,
	gfp_t								gfp
	)
{
    GLUE_INFO_T *prGlueInfo;
	ADAPTER_T *prAdapter;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen;


	/*              */
	if ((dev == NULL) || (peer == NULL))
		return; /*                   */

	DBGLOG(TDLS, INFO,
		("<tdls_fme> %s: Oper=%d ReasonCode=%d from "MACSTR"\n",
		__FUNCTION__, oper, reason_code, MAC2STR(peer)));

	/*      */
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(dev));
	prAdapter = prGlueInfo->prAdapter;
	u4PktLen = 0;

	/*                      */
    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
		DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: allocate pkt fail\n", __FUNCTION__));
        return;
    }
    prMsduInfo->dev = dev;

	/*                       */
	/*                 */
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	kalMemCopy(pPkt, peer, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	*(UINT_16 *)pPkt = htons(TDLS_FRM_PROT_TYPE);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                 */
	*pPkt = TDLS_FRM_ACTION_EVENT_TEAR_DOWN_TO_SUPPLICANT;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                    */
	*pPkt = oper;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                      */
	*pPkt = reason_code;
	*(pPkt+1) = 0x00;
	LR_TDLS_FME_FIELD_FILL(2);

	/*                                   */
	kalMemCopy(pPkt, peer, 6);
	LR_TDLS_FME_FIELD_FILL(6);

	/*                         */
	prMsduInfo->len = u4PktLen;
	dumpMemory8(ANDROID_LOG_INFO, prMsduInfo->data, u4PktLen);

	/*            */
	TdlsCmdTestRxIndicatePkts(prGlueInfo, prMsduInfo);
}
#endif /*                    */


/*                                                                            */
/*                                                                        
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsChSwConf(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_CHSW_CONF;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                                     
 
                                                        
                                                           
                                                
                  
 
              
 
                                                    
                                                                          
                                                               

                                                               
                                                          
                                         
                                         
                                   

                                                              
*/
/*                                                                            */
static void
TdlsCmdChSwConf(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.aucPeerMac);

	rCmd.Content.rCmdChSwConf.ucNetTypeIndex = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChSwConf.fgIsChSwEnabled = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChSwConf.fgIsChSwStarted = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChSwConf.ucRegClass = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChSwConf.ucTargetChan = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChSwConf.ucSecChanOff = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdChSwConf.fgIsChSwRegular = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO,
		("%s: "MACSTR" ucNetTypeIndex=%d, fgIsChSwEnabled=%d, fgIsChSwStarted=%d "
		"RegClass=%d, TargetChan=%d, SecChanOff=%d, Regular=%d\n",
		__FUNCTION__, MAC2STR(rCmd.aucPeerMac),
		rCmd.Content.rCmdChSwConf.ucNetTypeIndex,
		rCmd.Content.rCmdChSwConf.fgIsChSwEnabled,
		rCmd.Content.rCmdChSwConf.fgIsChSwStarted,
		rCmd.Content.rCmdChSwConf.ucRegClass,
		rCmd.Content.rCmdChSwConf.ucTargetChan,
		rCmd.Content.rCmdChSwConf.ucSecChanOff,
		rCmd.Content.rCmdChSwConf.fgIsChSwRegular));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsChSwConf,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                      
 
                                                        
                                                           
                                                
                  
 
              
 
                                                                                

                                                              

                                           
                              
                          
                          
                         
                              

                                                     
*/
/*                                                                            */
static void
TdlsCmdInfoDisplay(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	kalMemZero(&rCmd, sizeof(rCmd));

	CmdStringMacParse(prInBuf, &prInBuf, &u4InBufLen, rCmd.aucPeerMac);
	rCmd.ucNetTypeIndex = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdInfoDisplay.fgIsToClearAllHistory = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: Command PeerMac="MACSTR" in BSS%u\n",
		__FUNCTION__, MAC2STR(rCmd.aucPeerMac), rCmd.ucNetTypeIndex));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsInfoDisplay,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                     
 
                                                        
                                                           
                                                
                  
 
              
 
                                    

                               
*/
/*                                                                            */
static void
TdlsCmdKeyInfoDisplay(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	kalMemZero(&rCmd, sizeof(rCmd));

	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s\n", __FUNCTION__));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsKeyInfoDisplay,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                          
 
                                                        
                                                           
                                                
                  
 
              
 
                                                                                
                                                               

                                                     

                                               
*/
/*                                                                            */
static void
TdlsCmdMibParamUpdate(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*       */
	kalMemZero(&rCmd, sizeof(rCmd));

	/*                 */
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TunneledDirectLinkSetupImplemented = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSPeerUAPSDBufferSTAActivated = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSPeerPSMActivated = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSPeerUAPSDIndicationWindow = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSChannelSwitchingActivated = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSPeerSTAMissingAckRetryLimit = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSResponseTimeout = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSProbeDelay = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSDiscoveryRequestWindow = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSACDeterminationInterval = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("<tdls_cmd> MIB param = %d %d %d %d %d %d %d %d %d %d \n",
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TunneledDirectLinkSetupImplemented,
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSPeerUAPSDBufferSTAActivated,
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSPeerPSMActivated,
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSPeerUAPSDIndicationWindow,
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSChannelSwitchingActivated,
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSPeerSTAMissingAckRetryLimit,
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSResponseTimeout,
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSProbeDelay,
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSDiscoveryRequestWindow,
		rCmd.Content.rCmdMibUpdate.Tdlsdot11TDLSACDeterminationInterval));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsMibParamUpdate,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                            
 
                                                        
                                                           
                                                
                  
 
              
 
                                                    

                                 
*/
/*                                                                            */
static void
TdlsCmdSetupConf(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	kalMemZero(rCmd.aucPeerMac, sizeof(rCmd.aucPeerMac));

	rCmd.Content.rCmdSetupConf.fgIs2040Supported = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);

	DBGLOG(TDLS, INFO, ("%s: rCmdSetupConf=%d\n",
		__FUNCTION__, rCmd.Content.rCmdSetupConf.fgIs2040Supported));

	/*                    */
	prGlueInfo->rTdlsLink.fgIs2040Sup = rCmd.Content.rCmdSetupConf.fgIs2040Supported;

	rStatus = kalIoctl(prGlueInfo,
		TdlsSetupConf,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                            
 
                                                        
                                                           
                                                
                  
 
              
 
                                                                        

                                  
*/
/*                                                                            */
static void
TdlsCmdUapsdConf(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
    WLAN_STATUS rStatus;
	TDLS_CMD_CORE_T rCmd;
	UINT_32 u4BufLen;


	/*                 */
	kalMemZero(rCmd.aucPeerMac, sizeof(rCmd.aucPeerMac));

	/*                      */
	rCmd.Content.rCmdUapsdConf.fgIsSpTimeoutSkip = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	rCmd.Content.rCmdUapsdConf.fgIsPtiTimeoutSkip = \
		CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	/*                    */
	fgIsPtiTimeoutSkip = rCmd.Content.rCmdUapsdConf.fgIsPtiTimeoutSkip;

	DBGLOG(TDLS, INFO, ("%s: fgIsSpTimeoutSkip=%d, fgIsPtiTimeoutSkip=%d\n",
		__FUNCTION__, rCmd.Content.rCmdUapsdConf.fgIsSpTimeoutSkip,
		fgIsPtiTimeoutSkip));

	/*                    */
	rStatus = kalIoctl(prGlueInfo,
		TdlsUapsdConf,
		&rCmd,
		sizeof(rCmd),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR, ("%s kalIoctl fail:%x\n", __FUNCTION__, rStatus));
		return;
	}
}


/*                                                                            */
/*                                                                 
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
                                                     
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsInfoDisplay(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	GLUE_INFO_T *prGlueInfo;
	TDLS_CMD_CORE_T *prCmdContent;
	STA_RECORD_T *prStaRec;
	TDLS_INFO_LINK_T *prLink;
	UINT32 u4StartIdx;
	UINT32 u4PeerNum;
	BOOLEAN fgIsListAll;
	UINT8 ucMacZero[6];
	UINT32 u4HisIdx;
	UINT8 ucNetTypeIndex;


	/*      */
	prGlueInfo = prAdapter->prGlueInfo;
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	u4StartIdx = 0;
	u4PeerNum = 1;
	fgIsListAll = TRUE;
	kalMemZero(ucMacZero, sizeof(ucMacZero));
	ucNetTypeIndex = NETWORK_TYPE_AIS_INDEX;

	/*                            */
	DBGLOG(TDLS, TRACE, ("TDLS common:\n"));
	DBGLOG(TDLS, TRACE, ("\t\trFreeSwRfbList=%u\n",
		(UINT32)prAdapter->rRxCtrl.rFreeSwRfbList.u4NumElem));
	DBGLOG(TDLS, TRACE, ("\t\tjiffies=%u %ums (HZ=%d)\n", (UINT32)jiffies,
		(UINT32)kalGetTimeTick(), HZ));

	/*                                           */
	DBGLOG(TDLS, TRACE, ("TDLS link history: %d\n",
		prGlueInfo->rTdlsLink.u4LinkIdx));

	for(u4HisIdx = prGlueInfo->rTdlsLink.u4LinkIdx+1;
		u4HisIdx < TDLS_LINK_HISTORY_MAX;
		u4HisIdx++)
	{
		prLink = &prGlueInfo->rTdlsLink.rLinkHistory[u4HisIdx];

		if (kalMemCmp(prLink->aucPeerMac, ucMacZero, 6) == 0)
			continue; /*               */

		DBGLOG(TDLS, TRACE,
			("\t\t%d. "MACSTR": jiffies start(%lu %ums) jiffies end(%lu %ums) "
			"Reason(%u) fromUs(%u) Dup(%u) HT(%u)\n",
			u4HisIdx,
			MAC2STR(prLink->aucPeerMac),
			prLink->jiffies_start, jiffies_to_msecs(prLink->jiffies_start),
			prLink->jiffies_end, jiffies_to_msecs(prLink->jiffies_end),
			prLink->ucReasonCode,
			prLink->fgIsFromUs,
			prLink->ucDupCount,
			(prLink->ucHtCap & TDLS_INFO_LINK_HT_CAP_SUP)));

		if (prLink->ucHtCap & TDLS_INFO_LINK_HT_CAP_SUP)
		{
			DBGLOG(TDLS, TRACE,
				("\t\t\tBA (0x%x %x %x %x %x %x %x %x)\n",
				prLink->ucHtBa[0], prLink->ucHtBa[1],
				prLink->ucHtBa[2], prLink->ucHtBa[3],
				prLink->ucHtBa[4], prLink->ucHtBa[5],
				prLink->ucHtBa[6], prLink->ucHtBa[7]));
		}
	}
	for(u4HisIdx = 0;
		u4HisIdx <= prGlueInfo->rTdlsLink.u4LinkIdx;
		u4HisIdx++)
	{
		prLink = &prGlueInfo->rTdlsLink.rLinkHistory[u4HisIdx];

		if (kalMemCmp(prLink->aucPeerMac, ucMacZero, 6) == 0)
			continue; /*                                        */

		DBGLOG(TDLS, TRACE,
			("\t\t%d. "MACSTR": jiffies start(%lu %ums) jiffies end(%lu %ums) "
			"Reason(%u) fromUs(%u) Dup(%u) HT(%u)\n",
			u4HisIdx,
			MAC2STR(prLink->aucPeerMac),
			prLink->jiffies_start, jiffies_to_msecs(prLink->jiffies_start),
			prLink->jiffies_end, jiffies_to_msecs(prLink->jiffies_end),
			prLink->ucReasonCode,
			prLink->fgIsFromUs,
			prLink->ucDupCount,
			(prLink->ucHtCap & TDLS_INFO_LINK_HT_CAP_SUP)));

		if (prLink->ucHtCap & TDLS_INFO_LINK_HT_CAP_SUP)
		{
			DBGLOG(TDLS, TRACE,
				("\t\t\tBA (0x%x %x %x %x %x %x %x %x)\n",
				prLink->ucHtBa[0], prLink->ucHtBa[1],
				prLink->ucHtBa[2], prLink->ucHtBa[3],
				prLink->ucHtBa[4], prLink->ucHtBa[5],
				prLink->ucHtBa[6], prLink->ucHtBa[7]));
		}
	}
	DBGLOG(TDLS, TRACE, ("\n"));

	/*                          */
	if (prCmdContent != NULL)
	{
		if (kalMemCmp(prCmdContent->aucPeerMac, ucMacZero, 6) != 0)
		{
			prStaRec = cnmGetStaRecByAddress(prAdapter,
				prCmdContent->ucNetTypeIndex, prCmdContent->aucPeerMac);
			if (prStaRec == NULL)
				fgIsListAll = TRUE;
		}

		ucNetTypeIndex = prCmdContent->ucNetTypeIndex;
	}

	while(1)
	{
		if (fgIsListAll == TRUE)
		{
			/*                     */
			prStaRec = cnmStaTheTypeGet(prAdapter, ucNetTypeIndex,
				STA_TYPE_TDLS_PEER, &u4StartIdx);
			if  (prStaRec == NULL)
				break;
		}

		DBGLOG(TDLS, TRACE, ("-------- TDLS %d: 0x"MACSTR"\n",
			u4PeerNum, MAC2STR(prStaRec->aucMacAddr)));
		DBGLOG(TDLS, TRACE, ("\t\t\t State %d, PM %d, Cap 0x%x\n",
			prStaRec->ucStaState, prStaRec->fgIsInPS, prStaRec->u2CapInfo));
		DBGLOG(TDLS, TRACE, ("\t\t\t SetupDisable %d, ChSwDisable %d\n",
			prStaRec->fgTdlsIsProhibited, prStaRec->fgTdlsIsChSwProhibited));

		if (fgIsListAll == FALSE)
			break; /*               */
	}

	/*                                         */
	if ((prCmdContent != NULL) &&
		(prCmdContent->Content.rCmdInfoDisplay.fgIsToClearAllHistory == TRUE))
	{
		kalMemZero(&prGlueInfo->rTdlsLink, sizeof(prGlueInfo->rTdlsLink));
		prGlueInfo->rTdlsLink.u4LinkIdx = TDLS_LINK_HISTORY_MAX-1;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                            
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsKeyInfoDisplay(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_KEY_INFO;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                                 
 
                                                              
                                                      
                                                            
                                                   
                                                                  
 
              
*/
/*                                                                            */
static VOID
TdlsLinkHistoryRecord(
	GLUE_INFO_T							*prGlueInfo,
	BOOLEAN								fgIsTearDown,
	UINT8								*pucPeerMac,
	BOOLEAN								fgIsFromUs,
	UINT16								u2ReasonCode,
	VOID								*prOthers
	)
{
	TDLS_INFO_LINK_T *prLink;


	DBGLOG(TDLS, INFO,
		("<tdls_evt> %s: record history for "MACSTR" %d %d %d %d\n",
		__FUNCTION__, MAC2STR(pucPeerMac), prGlueInfo->rTdlsLink.u4LinkIdx,
		fgIsTearDown, fgIsFromUs, u2ReasonCode));

	/*                     */
	if (prGlueInfo->rTdlsLink.u4LinkIdx >= TDLS_LINK_HISTORY_MAX)
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_evt> %s: u4LinkIdx >= TDLS_LINK_HISTORY_MAX\n",
			__FUNCTION__));

		/*            */
		prGlueInfo->rTdlsLink.u4LinkIdx = 0;
	}

	prLink = &prGlueInfo->rTdlsLink.rLinkHistory[\
		prGlueInfo->rTdlsLink.u4LinkIdx];

	if (kalMemCmp(&prLink->aucPeerMac, pucPeerMac, 6) == 0)
	{
		if ((prLink->ucReasonCode == u2ReasonCode) &&
			(prLink->fgIsFromUs == fgIsFromUs))
		{
			/*                                            */
			if (fgIsTearDown == TRUE)
			{
				if (prLink->jiffies_end != 0)
				{
					/*                   */
					prLink->ucDupCount ++;
					return;
				}
			}
			else
			{
				/*               */
				prLink->ucDupCount ++;
				return;
			}
		}
	}

	/*                  */
	if (fgIsTearDown == TRUE)
	{
		/*                                                                                     */
		if (kalMemCmp(&prLink->aucPeerMac, pucPeerMac, 6) != 0)
		{
			/*                                    */
			DBGLOG(TDLS, INFO,
				("<tdls_evt> %s: cannot find the same entry!!!\n",
				__FUNCTION__));
			return;
		}

		prLink->jiffies_end = jiffies;
		prLink->ucReasonCode = (UINT8)u2ReasonCode;
		prLink->fgIsFromUs = fgIsFromUs;
	}
	else
	{
		/*                */
		prGlueInfo->rTdlsLink.u4LinkIdx ++;
		if (prGlueInfo->rTdlsLink.u4LinkIdx >= TDLS_LINK_HISTORY_MAX)
			prGlueInfo->rTdlsLink.u4LinkIdx = 0;

		prLink = &prGlueInfo->rTdlsLink.rLinkHistory[\
			prGlueInfo->rTdlsLink.u4LinkIdx];

		prLink->jiffies_start = jiffies;
		prLink->jiffies_end = 0;
		kalMemCopy(&prLink->aucPeerMac, pucPeerMac, 6);
		prLink->ucReasonCode = 0;
		prLink->fgIsFromUs = (UINT8)fgIsFromUs;
		prLink->ucDupCount = 0;

		if (prOthers != NULL)
		{
			/*                         */
			TDLS_LINK_HIS_OTHERS_T *prHisOthers;
			prHisOthers = (TDLS_LINK_HIS_OTHERS_T *)prOthers;
			if (prHisOthers->fgIsHt == TRUE)
				prLink->ucHtCap |= TDLS_INFO_LINK_HT_CAP_SUP;
		}
	}
}


/*                                                                            */
/*                                                                 
 
                                                              
                                                            
                                                             
                                       
 
              
*/
/*                                                                            */
static VOID
TdlsLinkHistoryRecordUpdate(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*pucPeerMac,
	TDLS_EVENT_HOST_SUBID_SPECIFIC_FRAME	eFmeStatus,
	VOID								*pInfo
	)
{
	TDLS_INFO_LINK_T *prLink;
	UINT32 u4LinkIdx;
	UINT32 u4Tid;


	/*              */
	if ((eFmeStatus < TDLS_HOST_EVENT_SF_BA) ||
		(eFmeStatus > TDLS_HOST_EVENT_SF_BA_RSP_DECLINE))
	{
		/*                          */
		return;
	}

	DBGLOG(TDLS, INFO,
		("<tdls_evt> %s: update history for "MACSTR" %d %d\n",
		__FUNCTION__, MAC2STR(pucPeerMac), prGlueInfo->rTdlsLink.u4LinkIdx,
		eFmeStatus));

	/*      */
	u4LinkIdx = prGlueInfo->rTdlsLink.u4LinkIdx;
	prLink = &prGlueInfo->rTdlsLink.rLinkHistory[u4LinkIdx];

	/*                                                                                     */
	if (kalMemCmp(&prLink->aucPeerMac, pucPeerMac, 6) != 0)
	{
		/*                                    */
		DBGLOG(TDLS, INFO,
			("<tdls_evt> %s: cannot find the same entry!!!\n",
			__FUNCTION__));
		return;
	}

	/*        */
	u4Tid = *(UINT32 *)pInfo;
	switch(eFmeStatus)
	{
		case TDLS_HOST_EVENT_SF_BA:
			prLink->ucHtBa[u4Tid] |= TDLS_INFO_LINK_HT_BA_SETUP;
			break;

		case TDLS_HOST_EVENT_SF_BA_OK:
			prLink->ucHtBa[u4Tid] |= TDLS_INFO_LINK_HT_BA_SETUP_OK;
			break;

		case TDLS_HOST_EVENT_SF_BA_DECLINE:
			prLink->ucHtBa[u4Tid] |= TDLS_INFO_LINK_HT_BA_SETUP_DECLINE;
			break;

		case TDLS_HOST_EVENT_SF_BA_PEER:
			prLink->ucHtBa[u4Tid] |= TDLS_INFO_LINK_HT_BA_PEER;
			break;

		case TDLS_HOST_EVENT_SF_BA_RSP_OK:
			prLink->ucHtBa[u4Tid] |= TDLS_INFO_LINK_HT_BA_RSP_OK;
			break;

		case TDLS_HOST_EVENT_SF_BA_RSP_DECLINE:
			prLink->ucHtBa[u4Tid] |= TDLS_INFO_LINK_HT_BA_RSP_DECLINE;
			break;
	}

	/*                           */
	TdlsInfoDisplay(prGlueInfo->prAdapter, NULL, 0, NULL);
}


/*                                                                            */
/*                                                                  
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsMibParamUpdate(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_MIB_UPDATE;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                                    
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsSetupConf(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_SETUP_CONF;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                               
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static TDLS_STATUS
TdlsUapsdConf(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	TDLS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (TDLS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = TDLS_CORE_CMD_UAPSD_CONF;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_TDLS_CORE,			/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(TDLS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(TDLS, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return TDLS_STATUS_RESOURCES;
	}

	DBGLOG(TDLS, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                        
 
                                                         
                                                                               
                                                
                  
 
              
 
*/
/*                                                                            */
static void
TdlsEventFmeStatus(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	)
{
	TDLS_EVENT_HOST_SUBID_SPECIFIC_FRAME eFmeStatus;
	STA_RECORD_T *prStaRec;
	UINT32 u4Tid;


	/*      */
	u4Tid = *(UINT32 *)prInBuf;
	prInBuf += 4; /*                   */

	/*              */
	prStaRec = cnmGetStaRecByIndex(prGlueInfo->prAdapter, *prInBuf);
	if ((prStaRec == NULL) || (!IS_TDLS_STA(prStaRec)))
		return;
	prInBuf ++;

	/*               */
	eFmeStatus = *prInBuf;
	TdlsLinkHistoryRecordUpdate(prGlueInfo, prStaRec->aucMacAddr,
		eFmeStatus, &u4Tid);
}


/*                                                                            */
/*                                                                          
 
                                                         
                                                                               
                                                
                  
 
              
 
*/
/*                                                                            */
static void
TdlsEventStatistics(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	)
{
	STA_RECORD_T *prStaRec;
	STAT_CNT_INFO_FW_T *prStat;
	UINT32 u4RateId;


	/*      */
	prStaRec = cnmGetStaRecByIndex(prGlueInfo->prAdapter, *prInBuf);
	if ((prStaRec == NULL) || (!IS_TDLS_STA(prStaRec)))
		return;

	prInBuf += 4; /*                        */

	/*                   */
	kalMemCopy(&prStaRec->rTdlsStatistics.rFw, prInBuf,\
		sizeof(prStaRec->rTdlsStatistics.rFw));

	/*                    */
	prStat = &prStaRec->rTdlsStatistics.rFw;

	DBGLOG(TDLS, TRACE, ("<tdls_evt> peer ["MACSTR"] statistics:\n",
		MAC2STR(prStaRec->aucMacAddr)));
	DBGLOG(TDLS, TRACE, ("\t\tT%d %d %d (P%d %d) (%dus) - E%d 0x%x - R%d (P%d)\n",
		prStat->u4NumOfTx, prStat->u4NumOfTxOK, prStat->u4NumOfTxRetry,
		prStat->u4NumOfPtiRspTxOk, prStat->u4NumOfPtiRspTxErr,
		prStat->u4TxDoneAirTimeMax,
		prStat->u4NumOfTxErr, prStat->u4TxErrBitmap,
		prStat->u4NumOfRx, prStat->u4NumOfPtiRspRx));

	DBGLOG(TDLS, TRACE, ("\t\t"));

	for(u4RateId=prStat->u4TxRateOkHisId;
		u4RateId<STAT_CNT_INFO_MAX_TX_RATE_OK_HIS_NUM;
		u4RateId++)
	{
		printk("%d(%d) ", prStat->aucTxRateOkHis[u4RateId][0],
			prStat->aucTxRateOkHis[u4RateId][1]);
	}
	for(u4RateId=0;
		u4RateId<prStat->u4TxRateOkHisId;
		u4RateId++)
	{
		printk("%d(%d) ", prStat->aucTxRateOkHis[u4RateId][0],
			prStat->aucTxRateOkHis[u4RateId][1]);
	}

	printk("\n");

	printk("\n");
}


/*                                                                            */
/*                                                 
 
                                                         
                                                                               
                                                
                  
 
              
 
*/
/*                                                                            */
static void
TdlsEventTearDown(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	)
{
	STA_RECORD_T *prStaRec;
	UINT16 u2ReasonCode;
	UINT32 u4TearDownSubId;
	UINT8 *pMac, aucZeroMac[6];


	/*      */
	u4TearDownSubId = *(UINT32 *)prInBuf;
	kalMemZero(aucZeroMac, sizeof(aucZeroMac));
	pMac = aucZeroMac;

	prStaRec = cnmGetStaRecByIndex(prGlueInfo->prAdapter, *(prInBuf+4));
	if (prStaRec != NULL)
		pMac = prStaRec->aucMacAddr;

	/*        */
	if (u4TearDownSubId == TDLS_HOST_EVENT_TD_PTI_TIMEOUT)
	{
		DBGLOG(TDLS, WARN, ("<tdls_evt> %s: peer ["MACSTR"] Reason=PTI timeout\n",
			__FUNCTION__, MAC2STR(pMac)));
	}
	else if (u4TearDownSubId == TDLS_HOST_EVENT_TD_AGE_TIMEOUT)
	{
		DBGLOG(TDLS, WARN, ("<tdls_evt> %s: peer ["MACSTR"] Reason=AGE timeout\n",
			__FUNCTION__, MAC2STR(pMac)));
	}
	else
	{
		DBGLOG(TDLS, WARN, ("<tdls_evt> %s: peer ["MACSTR"] Reason=%d\n",
			__FUNCTION__, MAC2STR(pMac), u4TearDownSubId));
	}

	/*              */
	if (prStaRec == NULL)
		return;

	if (fgIsPtiTimeoutSkip == TRUE)
	{
		/*                        */
		if (u4TearDownSubId == TDLS_HOST_EVENT_TD_PTI_TIMEOUT)
		{
			DBGLOG(TDLS, WARN, ("<tdls_evt> %s: skip PTI timeout\n",
				__FUNCTION__));
			return;
		}
	}

	/*                */
	if (u4TearDownSubId == TDLS_HOST_EVENT_TD_AGE_TIMEOUT)
		u2ReasonCode = TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_AGE_TIMEOUT;
	else if (u4TearDownSubId == TDLS_HOST_EVENT_TD_PTI_TIMEOUT)
		u2ReasonCode = TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_PTI_TIMEOUT;
	else if (u4TearDownSubId == TDLS_HOST_EVENT_TD_PTI_SEND_FAIL)
		u2ReasonCode = TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_PTI_SEND_FAIL;
	else if (u4TearDownSubId == TDLS_HOST_EVENT_TD_PTI_SEND_MAX_FAIL)
		u2ReasonCode = TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_PTI_SEND_MAX_FAIL;
	else if (u4TearDownSubId == TDLS_HOST_EVENT_TD_WRONG_NETWORK_IDX)
		u2ReasonCode = TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_WRONG_NETWORK_IDX;
	else if (u4TearDownSubId == TDLS_HOST_EVENT_TD_NON_STATE3)
		u2ReasonCode = TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_NON_STATE3;
	else if (u4TearDownSubId == TDLS_HOST_EVENT_TD_LOST_TEAR_DOWN)
		u2ReasonCode = TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_LOST_TEAR_DOWN;
	else
	{
		/*                   */
		u2ReasonCode = TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_UNKNOWN;
	}

	TdlsLinkHistoryRecord(prGlueInfo, TRUE, prStaRec->aucMacAddr, TRUE,
		u2ReasonCode, NULL);

	/*                                                                  */
	if ((u2ReasonCode == TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_AGE_TIMEOUT) ||
		(u2ReasonCode == TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_PTI_TIMEOUT))
	{
		u2ReasonCode = TDLS_REASON_CODE_UNREACHABLE;
	}

	/*                                                                                        */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0)
	cfg80211_tdls_oper_request(prGlueInfo->prDevHandler,
		prStaRec->aucMacAddr, TDLS_FRM_ACTION_TEARDOWN, u2ReasonCode, GFP_ATOMIC);
#else
	cfg80211_tdls_oper_request(prGlueInfo->prDevHandler,
		prStaRec->aucMacAddr, NL80211_TDLS_TEARDOWN, u2ReasonCode, GFP_ATOMIC);
#endif
}


/*                                                                            */
/*                                               
 
                                                         
                                                                               
                                                
                  
 
              
 
*/
/*                                                                            */
static void
TdlsEventTxDone(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	)
{
	UINT32 u4FmeIdx;
	UINT8 *pucFmeHdr;
	UINT8 ucErrStatus;


	ucErrStatus = *(UINT32 *)prInBuf;

	pucFmeHdr = prInBuf+4; /*                  */

	if (ucErrStatus == 0)
	{
		DBGLOG(TDLS, TRACE,
			("<tdls_evt> %s: OK to tx a TDLS action:",
			__FUNCTION__));
	}
	else
	{
		DBGLOG(TDLS, TRACE,
			("<tdls_evt> %s: fail to tx a TDLS action (err=0x%x):",
			__FUNCTION__, ucErrStatus));
	}

	/*                                         */
	for(u4FmeIdx=0; u4FmeIdx<(u4InBufLen-4); u4FmeIdx++)
	{
		if ((u4FmeIdx % 16) == 0)
		{
			printk("\n");
		}

		printk("%02x ", *pucFmeHdr++);
	}

	printk("\n\n");
}


/*                                                                              
                                     
                                                                                
*/

/*                                                                            */
/*                                                                             
 
                                                         
                                                            
                                                
                  
 
              
*/
/*                                                                            */
VOID
TdlsexBssExtCapParse(
	STA_RECORD_T						*prStaRec,
	UINT_8								*pucIE
	)
{
	UINT_8 *pucIeExtCap;


	/*              */
	if ((prStaRec == NULL) || (pucIE == NULL))
		return;

	if (IE_ID(pucIE) != ELEM_ID_EXTENDED_CAP)
		return;

	/*
             

                         
                                                                                   
                                                                                         
          
 */
	if (IE_LEN(pucIE) < 5)
		return; /*                        */

	/*      */
	prStaRec->fgTdlsIsProhibited = FALSE;
	prStaRec->fgTdlsIsChSwProhibited = FALSE;

	/*       */
	pucIeExtCap = pucIE + 2;
	pucIeExtCap += 4; /*                                 */

	if ((*pucIeExtCap) && BIT(38-32))
		prStaRec->fgTdlsIsProhibited = TRUE;
	if ((*pucIeExtCap) && BIT(39-32))
		prStaRec->fgTdlsIsChSwProhibited = TRUE;

	DBGLOG(TDLS, TRACE,
		("<tdls> %s: AP ["MACSTR"] tdls prohibit bit=%d %d\n",
		__FUNCTION__,
		MAC2STR(prStaRec->aucMacAddr),
		prStaRec->fgTdlsIsProhibited,
		prStaRec->fgTdlsIsChSwProhibited));
}


/*                                                                            */
/* 
                                                                           
 
                                                         
             
             
                                                           
 
                             
                                    
*/
/*                                                                            */
int
TdlsexCfg80211TdlsMgmt(
	struct wiphy						*wiphy,
	struct net_device					*dev,
	u8									*peer,
	u8									action_code,
	u8									dialog_token,
	u16									status_code,
	const u8							*buf,
	size_t								len
	)
{
	ADAPTER_T *prAdapter;
	GLUE_INFO_T *prGlueInfo;
	BSS_INFO_T *prAisBssInfo;
    WLAN_STATUS rStatus;
    UINT_32 u4BufLen;
	TDLS_MGMT_TX_INFO *prMgmtTxInfo;


	/*
                                                                                  
                                                       
                                                                      
                                                                     

                                                               

                                                       
 */

	/*              */
	if ((wiphy == NULL) || (peer == NULL))
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cfg> %s: wrong 0x%p 0x%p!\n",
			__FUNCTION__, wiphy, peer));
		return -EINVAL;
	}

	DBGLOG(TDLS, INFO, ("<tdls_cfg> %s: ["MACSTR"] %d %d %d 0x%p %u\n",
		__FUNCTION__, MAC2STR(peer),
		action_code, dialog_token, status_code, buf, (UINT32)len));

	/*      */
	prGlueInfo = (GLUE_INFO_T *) wiphy_priv(wiphy);
	if (prGlueInfo == NULL)
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cfg> %s: wrong prGlueInfo 0x%p!\n",
			__FUNCTION__, prGlueInfo));
		return -EINVAL;
	}

	prAdapter = prGlueInfo->prAdapter;
	if (prAdapter->fgTdlsIsSup == FALSE)
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cfg> %s: firmware TDLS is not supported!\n",
			__FUNCTION__));
		return -EBUSY;
	}

	prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	if (prAisBssInfo->fgTdlsIsProhibited == TRUE)
	{
		/*                                                       */
		return 0;
	}

	prMgmtTxInfo = kalMemAlloc(sizeof(TDLS_MGMT_TX_INFO), VIR_MEM_TYPE);
	if (prMgmtTxInfo == NULL)
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cfg> %s: allocate fail!\n",
			__FUNCTION__));
		return -ENOMEM;
	}

	kalMemZero(prMgmtTxInfo, sizeof(TDLS_MGMT_TX_INFO));

	if (peer != NULL)
		kalMemCopy(prMgmtTxInfo->aucPeer, peer, 6);
	prMgmtTxInfo->ucActionCode = action_code;
	prMgmtTxInfo->ucDialogToken = dialog_token;
	prMgmtTxInfo->u2StatusCode = status_code;

	if (buf != NULL)
	{
		if (len > sizeof(prMgmtTxInfo->aucSecBuf))
		{
			kalMemFree(prMgmtTxInfo, VIR_MEM_TYPE, sizeof(TDLS_MGMT_TX_INFO));
			return -EINVAL;
		}
		prMgmtTxInfo->u4SecBufLen = len;
		kalMemCopy(prMgmtTxInfo->aucSecBuf, buf, len);
	}

	/*                                 */
	rStatus = kalIoctl(prGlueInfo,
		TdlsexMgmtCtrl,
		prMgmtTxInfo,
		sizeof(TDLS_MGMT_TX_INFO),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);

	/*
                                                                            
                        
 */
	kalMemZero(prMgmtTxInfo, sizeof(TDLS_MGMT_TX_INFO));

	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR,
			("%s enable or disable link fail:%x\n", __FUNCTION__, rStatus));
		kalMemFree(prMgmtTxInfo, VIR_MEM_TYPE, sizeof(TDLS_MGMT_TX_INFO));
		return -EINVAL;
	}

	kalMemFree(prMgmtTxInfo, VIR_MEM_TYPE, sizeof(TDLS_MGMT_TX_INFO));
	return 0;
}


/*                                                                            */
/* 
                                                                                
 
                                                         
             
             
                                                           
 
                             
                                    
*/
/*                                                                            */
int
TdlsexCfg80211TdlsOper(
	struct wiphy						*wiphy,
	struct net_device					*dev,
	u8									*peer,
	enum nl80211_tdls_operation			oper
	)
{
	ADAPTER_T *prAdapter;
	GLUE_INFO_T *prGlueInfo;
    WLAN_STATUS rStatus;
    UINT_32 u4BufLen;
	TDLS_CMD_LINK_T rCmdLink;


	/*              */
	if (peer == NULL)
	{
		DBGLOG(TDLS, ERROR, ("<tdls_cfg> %s: peer == NULL!\n", __FUNCTION__));
		return -EINVAL;
	}

	DBGLOG(TDLS, INFO, ("<tdls_cfg> %s: ["MACSTR"] %d %d\n",
		__FUNCTION__, MAC2STR(peer), oper,
		(wiphy->flags & WIPHY_FLAG_SUPPORTS_TDLS)));

	if (!(wiphy->flags & WIPHY_FLAG_SUPPORTS_TDLS))
		return -ENOTSUPP;

	/*      */
	prGlueInfo = (GLUE_INFO_T *) wiphy_priv(wiphy);
	if (prGlueInfo == NULL)
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cfg> %s: wrong prGlueInfo 0x%p!\n",
			__FUNCTION__, prGlueInfo));
		return -EINVAL;
	}
	prAdapter = prGlueInfo->prAdapter;
	kalMemCopy(rCmdLink.aucPeerMac, peer, sizeof(rCmdLink.aucPeerMac));
	rCmdLink.fgIsEnabled = FALSE;

	/*
                               
                              
                      
                         
                            
                             
    
 */

	switch(oper)
	{
		case NL80211_TDLS_ENABLE_LINK:
			rCmdLink.fgIsEnabled = TRUE;
			break;

		case NL80211_TDLS_DISABLE_LINK:
			rCmdLink.fgIsEnabled = FALSE;
			break;

		case NL80211_TDLS_TEARDOWN:
		case NL80211_TDLS_SETUP:
		case NL80211_TDLS_DISCOVERY_REQ:
			/*                                                        */
			return -ENOTSUPP;

		default:
			return -ENOTSUPP;
	}

	/*                             */
	rStatus = kalIoctl(prGlueInfo,
		TdlsexLinkCtrl,
		&rCmdLink,
		sizeof(TDLS_CMD_LINK_T),
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&u4BufLen);
	
	if (rStatus != WLAN_STATUS_SUCCESS)
	{
		DBGLOG(TDLS, ERROR,
			("%s enable or disable link fail:%x\n", __FUNCTION__, rStatus));
		return -EINVAL;
	}

	return 0;
}


/*                                                                            */
/*                                                                  
 
                                                         
                                                            
                                                
                  
 
              
*/
/*                                                                            */
VOID
TdlsexCmd(
	P_GLUE_INFO_T						prGlueInfo,
	UINT_8								*prInBuf,
	UINT_32 							u4InBufLen
	)
{
	UINT_32 u4Subcmd;


	/*                        */
	u4Subcmd = CmdStringDecParse(prInBuf, &prInBuf, &u4InBufLen);
	DBGLOG(TDLS, INFO, ("<tdls_cmd> sub command = %u\n", (UINT32)u4Subcmd));

	/*                              */
	switch(u4Subcmd)
	{
#if TDLS_CFG_CMD_TEST /*                    */
		case TDLS_CMD_TEST_TX_FRAME:
			/*                               */
			TdlsCmdTestTxFrame(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_TX_TDLS_FRAME:
			/*                                               */
			TdlsCmdTestTxTdlsFrame(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_RCV_FRAME:
			/*                                  */
			TdlsCmdTestRvFrame(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_PEER_ADD:
			/*                             */
			TdlsCmdTestAddPeer(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_PEER_UPDATE:
			/*                                */
			TdlsCmdTestUpdatePeer(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_DATA_FRAME:
			/*                                           */
			TdlsCmdTestDataSend(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_RCV_NULL:
			/*                                                    */
			TdlsCmdTestNullRecv(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_SKIP_TX_FAIL:
			/*                                       */
			TdlsCmdTestTxFailSkip(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_SKIP_KEEP_ALIVE:
			/*                                              */
			TdlsCmdTestKeepAliveSkip(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_SKIP_CHSW_TIMEOUT:
			/*                                                          */
			TdlsCmdTestChSwTimeoutSkip(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_PROHIBIT_SET_IN_AP:
			/*                                      */
			TdlsCmdTestProhibitedBitSet(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_SCAN_DISABLE:
			/*                                                      */
			TdlsCmdTestScanCtrl(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_DATA_FRAME_CONT:
			/*                                                        */
			TdlsCmdTestDataContSend(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_CH_SW_PROHIBIT_SET_IN_AP:
			/*                                                     */
			TdlsCmdTestChSwProhibitedBitSet(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_DELAY:
			/*               */
			TdlsCmdTestDelay(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_TEST_PTI_TX_FAIL:
			/*                                   */
			TdlsCmdTestPtiTxDoneFail(prGlueInfo, prInBuf, u4InBufLen);
			break;
#endif /*                   */

		case TDLS_CMD_MIB_UPDATE:
			/*                       */
			TdlsCmdMibParamUpdate(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_UAPSD_CONF:
			/*                         */
			TdlsCmdUapsdConf(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_CH_SW_CONF:
			/*                                                            */
			TdlsCmdChSwConf(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_SETUP_CONF:
			/*                         */
			TdlsCmdSetupConf(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_INFO:
			/*                              */
			TdlsCmdInfoDisplay(prGlueInfo, prInBuf, u4InBufLen);
			break;

		case TDLS_CMD_KEY_INFO:
			/*                         */
			TdlsCmdKeyInfoDisplay(prGlueInfo, prInBuf, u4InBufLen);
			break;

		default:
			break;
	}
}


/*                                                                            */
/*                                                                 
 
                                                              
                                          
                                                            
                                                   
                                                                  
 
              
 
*/
/*                                                                            */
VOID
TdlsexLinkHistoryRecord(
	GLUE_INFO_T							*prGlueInfo,
	BOOLEAN								fgIsTearDown,
	UINT8								*pucPeerMac,
	BOOLEAN								fgIsFromUs,
	UINT16								u2ReasonCode
	)
{
	/*              */
	if ((prGlueInfo == NULL) || (pucPeerMac == NULL))
		return;

	DBGLOG(TDLS, INFO,
		("<tdls_evt> %s: Rcv a inform from "MACSTR" %d %d\n",
		__FUNCTION__, MAC2STR(pucPeerMac), fgIsFromUs, u2ReasonCode));

	/*        */
	TdlsLinkHistoryRecord(prGlueInfo, fgIsTearDown, pucPeerMac,\
		fgIsFromUs, u2ReasonCode, NULL);
}


/*                                                                            */
/*                                                                  
 
                                                         
                                                            
                                                
                  
 
              
*/
/*                                                                            */
VOID
TdlsexEventHandle(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	)
{
	UINT32 u4EventId;


	/*              */
	if ((prGlueInfo == NULL) || (prInBuf == NULL))
		return; /*                   */

	/*        */
	u4EventId = *(UINT32 *)prInBuf;
	u4InBufLen -= 4;

	DBGLOG(TDLS, INFO,
		("<tdls> %s: Rcv a event: %d\n", __FUNCTION__, u4EventId));

	switch(u4EventId)
	{
		case TDLS_HOST_EVENT_TEAR_DOWN:
			TdlsEventTearDown(prGlueInfo, prInBuf+4, u4InBufLen);
			break;

		case TDLS_HOST_EVENT_TX_DONE:
			TdlsEventTxDone(prGlueInfo, prInBuf+4, u4InBufLen);
			break;

		case TDLS_HOST_EVENT_FME_STATUS:
			TdlsEventFmeStatus(prGlueInfo, prInBuf+4, u4InBufLen);
			break;

		case TDLS_HOST_EVENT_STATISTICS:
			TdlsEventStatistics(prGlueInfo, prInBuf+4, u4InBufLen);
			break;
	}
}


/*                                                                            */
/* 
                                                               
 
                                                         
 
                                                                      
                               
*/
/*                                                                            */
TDLS_STATUS
TdlsexKeyHandle(
	ADAPTER_T							*prAdapter,
	PARAM_KEY_T							*prNewKey
	)
{
	STA_RECORD_T *prStaRec;


	/*              */
	if ((prAdapter == NULL) || (prNewKey == NULL))
		return TDLS_STATUS_FAILURE;

	/*
                                                                                   
                                                             
 */
	prStaRec = cnmGetStaRecByAddress(\
		prAdapter, NETWORK_TYPE_AIS_INDEX, prNewKey->arBSSID);
	if ((prStaRec != NULL) && IS_TDLS_STA(prStaRec))
	{
		DBGLOG(TDLS, TRACE, ("<tdls> %s: ["MACSTR"] queue key (len=%d) "
			"until link is enabled\n",
			__FUNCTION__, MAC2STR(prNewKey->arBSSID),
			(UINT32)prNewKey->u4KeyLength));

		if (prStaRec->ucStaState == STA_STATE_3)
		{
			DBGLOG(TDLS, TRACE, ("<tdls> %s: ["MACSTR"] tear down the link "
				"due to STA_STATE_3\n",
				__FUNCTION__, MAC2STR(prNewKey->arBSSID)));

			/*        */
			TdlsLinkHistoryRecord(prAdapter->prGlueInfo, TRUE,
				prStaRec->aucMacAddr, TRUE,
				TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_REKEY, NULL);
			
			/*                                                                                        */
			cfg80211_tdls_oper_request(prAdapter->prGlueInfo->prDevHandler,
				prStaRec->aucMacAddr, TDLS_FRM_ACTION_TEARDOWN,
				TDLS_REASON_CODE_UNSPECIFIED, GFP_ATOMIC);
			return TDLS_STATUS_SUCCESS;
		}

		/*                */
		kalMemCopy(&prStaRec->rTdlsKeyTemp, prNewKey, sizeof(prStaRec->rTdlsKeyTemp));
		return TDLS_STATUS_SUCCESS;
	}

	return TDLS_STATUS_FAILURE;
}


/*                                                                            */
/* 
                                                               
 
                                                         
 
                
*/
/*                                                                            */
VOID
TdlsexInit (
	ADAPTER_T							*prAdapter
    )
{
	GLUE_INFO_T *prGlueInfo;


	/*      */
	prGlueInfo = (GLUE_INFO_T *)prAdapter->prGlueInfo;

	/*       */
	kalMemZero(&prGlueInfo->rTdlsLink, sizeof(prGlueInfo->rTdlsLink));
}


/*                                                                            */
/* 
                                                                 
 
                                                         
 
                                                   
*/
/*                                                                            */
BOOLEAN
TdlsexIsAnyPeerInPowerSave(
	ADAPTER_T 	  						*prAdapter
	)
{
	STA_RECORD_T *prStaRec;
	UINT32 u4StaId, u4StartIdx;


	for(u4StaId=0, u4StartIdx=0; u4StaId<CFG_STA_REC_NUM; u4StaId++)
	{
		/*                     */
		prStaRec = cnmStaTheTypeGet(prAdapter, NETWORK_TYPE_AIS_INDEX,
			STA_TYPE_TDLS_PEER, &u4StartIdx);
		if  (prStaRec == NULL)
			break;

		if (prStaRec->fgIsInPS == TRUE)
		{
			printk("<tx> yes, at least one peer is in ps\n");
			return TRUE;
		}
	}

	return FALSE;
}
	

/*                                                                            */
/* 
                                                                 
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
*/
/*                                                                            */
TDLS_STATUS
TdlsexLinkCtrl (
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
    )
{
	GLUE_INFO_T *prGlueInfo;
	TDLS_CMD_LINK_T *prCmd;
	BSS_INFO_T *prBssInfo;
	STA_RECORD_T *prStaRec;
	TDLS_LINK_HIS_OTHERS_T rHisOthers;


	/*              */
	if ((prAdapter == NULL) || (pvSetBuffer == NULL) || (pu4SetInfoLen == NULL))
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cmd> %s: sanity fail!\n", __FUNCTION__));
		return TDLS_STATUS_FAILURE;
	}
	
	/*      */
	prGlueInfo = (GLUE_INFO_T *)prAdapter->prGlueInfo;
	*pu4SetInfoLen = sizeof(TDLS_CMD_LINK_T);
	prCmd = (TDLS_CMD_LINK_T *)pvSetBuffer;

	/*                  */
	prStaRec = cnmGetStaRecByAddress(prAdapter,
		(UINT_8) NETWORK_TYPE_AIS_INDEX,
		prCmd->aucPeerMac);
	if (prStaRec == NULL)
	{
		DBGLOG(TDLS, ERROR, ("<tdls_cfg> %s: cannot find the peer! "MACSTR"\n",
			__FUNCTION__, MAC2STR(prCmd->aucPeerMac)));
		return TDLS_STATUS_FAILURE;
	}

	if (prCmd->fgIsEnabled == TRUE)
	{
		cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_3);
		DBGLOG(TDLS, TRACE, ("<tdls_cfg> %s: NL80211_TDLS_ENABLE_LINK\n",
			__FUNCTION__));

		/*                                                                */
		prStaRec->fgTdlsInSecurityMode = FALSE;

		if (prStaRec->rTdlsKeyTemp.u4Length > 0)
		{
			UINT_32 u4BufLen; /*        */

			DBGLOG(TDLS, INFO, ("<tdls_cfg> %s: key len=%d\n",
				__FUNCTION__, (UINT32)prStaRec->rTdlsKeyTemp.u4Length));

			/*
                                                        
                                                    
   */
			_wlanoidSetAddKey(prAdapter, &prStaRec->rTdlsKeyTemp,
				prStaRec->rTdlsKeyTemp.u4Length, FALSE, CIPHER_SUITE_CCMP,
				&u4BufLen);

			/*                    */
			prStaRec->fgTdlsInSecurityMode = TRUE;
			kalMemZero(&prStaRec->rTdlsKeyTemp, sizeof(prStaRec->rTdlsKeyTemp));
		}

		/*                                                     */
		prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);
 		if (prBssInfo->fgTdlsIsChSwProhibited == TRUE)
		{
			TDLS_CMD_CORE_T rCmd;
			kalMemZero(&rCmd, sizeof(TDLS_CMD_CORE_T));
			rCmd.Content.rCmdChSwConf.ucNetTypeIndex = prStaRec->ucNetTypeIndex;
			rCmd.Content.rCmdChSwConf.fgIsChSwEnabled = FALSE;
			kalMemCopy(rCmd.aucPeerMac, prStaRec->aucMacAddr, 6);
			TdlsChSwConf(prAdapter, &rCmd, 0, 0);

			DBGLOG(TDLS, INFO, ("<tdls_cfg> %s: disable channel switch\n",
				__FUNCTION__));
		}

		TDLS_LINK_INCREASE(prGlueInfo);

		/*             */
		if (prStaRec->ucDesiredPhyTypeSet & PHY_TYPE_SET_802_11N)
			rHisOthers.fgIsHt = TRUE;
		else
			rHisOthers.fgIsHt = FALSE;

		TdlsLinkHistoryRecord(prAdapter->prGlueInfo, FALSE,
			prStaRec->aucMacAddr, !prStaRec->flgTdlsIsInitiator, 0, &rHisOthers);
	}
	else
	{
		cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);
		cnmStaRecFree(prAdapter, prStaRec, TRUE); /*                             */
		DBGLOG(TDLS, TRACE, ("<tdls_cfg> %s: NL80211_TDLS_DISABLE_LINK\n",
			__FUNCTION__));

		TDLS_LINK_DECREASE(prGlueInfo);
//                        
	}

	/*                        */
	if ((TDLS_LINK_COUNT(prGlueInfo) < 0) ||
		(TDLS_LINK_COUNT(prGlueInfo) > 1))
	{
		/*                                                                     */
		UINT32 u4Idx;

		TDLS_LINK_COUNT_RESET(prGlueInfo);

	    for(u4Idx=0; u4Idx<CFG_STA_REC_NUM; u4Idx++)
		{
	        prStaRec = &prAdapter->arStaRec[u4Idx];

	        if (prStaRec->fgIsInUse &&
				IS_TDLS_STA(prStaRec))
			{
				TDLS_LINK_INCREASE(prGlueInfo);
	        }
	    }

		if (TDLS_LINK_COUNT(prGlueInfo) > 1)
		{
			/*                              */
			DBGLOG(TDLS, INFO, ("<tdls_cfg> %s: cTdlsLinkCnt %d > 1?\n",
				__FUNCTION__, TDLS_LINK_COUNT(prGlueInfo)));

			TDLS_LINK_COUNT_RESET(prGlueInfo);

			/*                     */
			for(u4Idx=0; u4Idx<CFG_STA_REC_NUM; u4Idx++)
			{
				prStaRec = &prAdapter->arStaRec[u4Idx];
			
				if (prStaRec->fgIsInUse &&
					IS_TDLS_STA(prStaRec))
				{
					cnmStaRecFree(prAdapter, prStaRec, TRUE);
				}
			}

			/*                           */
		}
	}

	/*                           */
	TdlsInfoDisplay(prAdapter, NULL, 0, NULL);

	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                                 
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
*/
/*                                                                            */
TDLS_STATUS
TdlsexMgmtCtrl (
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
    )
{
	GLUE_INFO_T *prGlueInfo;
	TDLS_MGMT_TX_INFO *prMgmtTxInfo;
	STA_RECORD_T *prStaRec;


	/*              */
	if ((prAdapter == NULL) || (pvSetBuffer == NULL) || (pu4SetInfoLen == NULL))
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cmd> %s: sanity fail!\n", __FUNCTION__));
		return TDLS_STATUS_FAILURE;
	}
	
	/*      */
	prGlueInfo = (GLUE_INFO_T *)prAdapter->prGlueInfo;
	*pu4SetInfoLen = sizeof(TDLS_MGMT_TX_INFO);
	prMgmtTxInfo = (TDLS_MGMT_TX_INFO *)pvSetBuffer;

	switch(prMgmtTxInfo->ucActionCode)
	{
		case TDLS_FRM_ACTION_DISCOVERY_RESPONSE:
			prStaRec = NULL;
			break;

		case TDLS_FRM_ACTION_SETUP_REQ:
			prStaRec = cnmGetStaRecByAddress(prAdapter,
										(UINT_8) NETWORK_TYPE_AIS_INDEX,
										prMgmtTxInfo->aucPeer);
			if ((prStaRec != NULL) && (prStaRec->ucStaState == STA_STATE_3))
			{
				/*                                          */
				/*                                         */

				/*
                                                         
                                                               
    */
				DBGLOG(TDLS, TRACE,
					("<tdls_cmd> %s: skip new setup on the exist link!\n",
					__FUNCTION__));
				return TDLS_STATUS_SUCCESS;
			}

			prStaRec = NULL;
			break;

		case TDLS_FRM_ACTION_SETUP_RSP:
		case TDLS_FRM_ACTION_CONFIRM:
		case TDLS_FRM_ACTION_TEARDOWN:
			prStaRec = cnmGetStaRecByAddress(prAdapter,
										(UINT_8) NETWORK_TYPE_AIS_INDEX,
										prMgmtTxInfo->aucPeer);
#if 0 /*                                           */
			/*
                                                                 
                                                                   
                                                                  

                                          
   */
			if (prStaRec == NULL)
			{
				DBGLOG(TDLS, ERROR,
					("<tdls_cfg> %s: cannot find the peer!\n",
					__FUNCTION__));
				return -EINVAL;
			}
#endif
			break;

		/*
                                 
                                                                           
                       
                                             
                                                                          
  */

		default:
			DBGLOG(TDLS, ERROR,
				("<tdls_cfg> %s: wrong action_code %d!\n",
				__FUNCTION__, prMgmtTxInfo->ucActionCode));
			return TDLS_STATUS_FAILURE;
	}

	/*                          */
	if (prStaRec != NULL)
	{
		DBGLOG(TDLS, INFO, ("<tdls_cfg> %s: ["MACSTR"] ps=%d status=%d\n",
			__FUNCTION__, MAC2STR(prStaRec->aucMacAddr),
			prStaRec->fgIsInPS, prMgmtTxInfo->u2StatusCode));

		if (prMgmtTxInfo->ucActionCode == TDLS_FRM_ACTION_TEARDOWN)
		{
			/*                           */
			TdlsLinkHistoryRecord(prGlueInfo, TRUE, prMgmtTxInfo->aucPeer,
				TRUE, prMgmtTxInfo->u2StatusCode, NULL);
		}
	}

	return TdlsDataFrameSend(
					prAdapter,
					prStaRec,
					prMgmtTxInfo->aucPeer,
					prMgmtTxInfo->ucActionCode,
					prMgmtTxInfo->ucDialogToken,
					prMgmtTxInfo->u2StatusCode,
					(UINT_8 *)prMgmtTxInfo->aucSecBuf,
					prMgmtTxInfo->u4SecBufLen);
}


/*                                                                            */
/* 
                                                     
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
*/
/*                                                                            */
TDLS_STATUS
TdlsexPeerAdd (
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
    )
{
	GLUE_INFO_T *prGlueInfo;
	TDLS_CMD_PEER_ADD_T *prCmd;
	BSS_INFO_T *prAisBssInfo;
	STA_RECORD_T *prStaRec;
	UINT_8 ucNonHTPhyTypeSet;
	UINT32 u4StartIdx;
	OS_SYSTIME rCurTime;


	/*              */
	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s\n", __FUNCTION__));

	if ((prAdapter == NULL) || (pvSetBuffer == NULL) || (pu4SetInfoLen == NULL))
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cmd> %s: sanity fail!\n", __FUNCTION__));
		return TDLS_STATUS_FAILURE;
	}

	/*      */
	prGlueInfo = (GLUE_INFO_T *)prAdapter->prGlueInfo;
	*pu4SetInfoLen = sizeof(TDLS_CMD_PEER_ADD_T);
	prCmd = (TDLS_CMD_PEER_ADD_T *)pvSetBuffer;
	prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	u4StartIdx = 0;

	/*                  */
	prStaRec = cnmGetStaRecByAddress(prAdapter,
								(UINT_8) NETWORK_TYPE_AIS_INDEX,
								prCmd->aucPeerMac);

	/*                                                                               */
	if (prStaRec == NULL)
	{
		/*                     */
		prStaRec = cnmStaTheTypeGet(prAdapter, NETWORK_TYPE_AIS_INDEX,
			STA_TYPE_TDLS_PEER, &u4StartIdx);

		if (prStaRec != NULL)
		{
			/*                             */
			DBGLOG(TDLS, ERROR,
				("<tdls_cmd> %s: one TDLS link setup ["MACSTR"] is going...\n",
				__FUNCTION__, MAC2STR(prStaRec->aucMacAddr)));

			if (prStaRec->ucStaState != STA_STATE_3)
			{
				/*               */
				GET_CURRENT_SYSTIME(&rCurTime);

		        if (CHECK_FOR_TIMEOUT(rCurTime, prStaRec->rTdlsSetupStartTime,
					SEC_TO_SYSTIME(TDLS_SETUP_TIMEOUT_SEC)))
				{
					/*                 */
					cnmStaRecFree(prAdapter, prStaRec, TRUE);

					DBGLOG(TDLS, ERROR,
						("<tdls_cmd> %s: free going TDLS link setup ["MACSTR"]\n",
						__FUNCTION__, MAC2STR(prStaRec->aucMacAddr)));

					/*                  */
					prStaRec = NULL;
				}
				else
					return TDLS_STATUS_FAILURE;
			}
			else
			{
				/*                                                  */
				return TDLS_STATUS_FAILURE;
			}	
		}
	}
	else
	{
		if (prStaRec->ucStaState == STA_STATE_3)
		{
			/*                                                                            */
			TdlsLinkHistoryRecord(prAdapter->prGlueInfo, TRUE,
				prStaRec->aucMacAddr, TRUE,
				TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_REKEY, NULL);
			
			/*                                                                                        */
			cfg80211_tdls_oper_request(prAdapter->prGlueInfo->prDevHandler,
				prStaRec->aucMacAddr, TDLS_FRM_ACTION_TEARDOWN,
				TDLS_REASON_CODE_UNSPECIFIED, GFP_ATOMIC);

			DBGLOG(TDLS, TRACE,
				("<tdls_cmd> %s: re-setup link for ["MACSTR"] maybe re-key?\n",
				__FUNCTION__, MAC2STR(prStaRec->aucMacAddr)));
			return TDLS_STATUS_FAILURE;
		}
	}

	/*
                               

                     
                             
                                                                                     
                                            
                                                         
                         
                                            

                     
                            
                                                                                     
                                            
                              
                            
                         
                                            
 */
	if (prStaRec == NULL)
	{
		prStaRec = cnmStaRecAlloc(prAdapter, (UINT_8) NETWORK_TYPE_AIS_INDEX);

		if (prStaRec == NULL)
		{
			/*                   */
			DBGLOG(TDLS, ERROR,
				("<tdls_cmd> %s: alloc prStaRec fail!\n", __FUNCTION__));
			return TDLS_STATUS_RESOURCES;
		}

		/*                   */
		/*                                                 */
		COPY_MAC_ADDR(prStaRec->aucMacAddr, prCmd->aucPeerMac);

//                                                         
	}
	else
	{
#if 0
		if ((prStaRec->ucStaState > STA_STATE_1) && (IS_TDLS_STA(prStaRec)))
		{
			/*
                                                                               
                                       
   */
			cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);
		}
#endif
	}

	/*                                                                       */
	/*                                                                    */

	/*                     */
	/*                                                       */
	prStaRec->u2OperationalRateSet = prAisBssInfo->u2OperationalRateSet;
	prStaRec->u2BSSBasicRateSet = prAisBssInfo->u2BSSBasicRateSet;
	prStaRec->u2DesiredNonHTRateSet = prAdapter->rWifiVar.ucAvailablePhyTypeSet;
	prStaRec->ucPhyTypeSet = prAisBssInfo->ucPhyTypeSet;
	prStaRec->eStaType = STA_TYPE_TDLS_PEER;

    prStaRec->ucDesiredPhyTypeSet = /*                         */
		prAdapter->rWifiVar.ucAvailablePhyTypeSet;
    ucNonHTPhyTypeSet = prStaRec->ucDesiredPhyTypeSet & PHY_TYPE_SET_802_11ABG;

    /*                                         */
    if (ucNonHTPhyTypeSet)
	{
        if (ucNonHTPhyTypeSet & PHY_TYPE_BIT_ERP)
		{
            prStaRec->ucNonHTBasicPhyType = PHY_TYPE_ERP_INDEX;
        }
        else if (ucNonHTPhyTypeSet & PHY_TYPE_BIT_OFDM)
		{
            prStaRec->ucNonHTBasicPhyType = PHY_TYPE_OFDM_INDEX;
        }
        else /*                                                 */
		{
            prStaRec->ucNonHTBasicPhyType = PHY_TYPE_HR_DSSS_INDEX;
        }

        prStaRec->fgHasBasicPhyType = TRUE;
    }
    else
	{
        /*                                */
//                                                              

		prStaRec->ucNonHTBasicPhyType = PHY_TYPE_HR_DSSS_INDEX;
        prStaRec->fgHasBasicPhyType = FALSE;
    }

    /*                                */
    {
        P_CONNECTION_SETTINGS_T prConnSettings;

        prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
        prStaRec->u2DesiredNonHTRateSet =\
			(prStaRec->u2OperationalRateSet & prConnSettings->u2DesiredNonHTRateSet);
    }

#if 0 /*                                                                                     */
	/*                                                                                 */
	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: ["MACSTR"] ["MACSTR"]\n",
		__FUNCTION__, MAC2STR(prGlueInfo->aucTdlsHtPeerMac),
		MAC2STR(prCmd->aucPeerMac)));

	if (kalMemCmp(prGlueInfo->aucTdlsHtPeerMac, prCmd->aucPeerMac, 6) == 0)
	{
		/*                                               */
		kalMemCopy(&prStaRec->rTdlsHtCap,
			&prGlueInfo->rTdlsHtCap, sizeof(IE_HT_CAP_T));

		prStaRec->ucPhyTypeSet |= PHY_TYPE_SET_802_11N;
		prStaRec->u2DesiredNonHTRateSet |= BIT(RATE_HT_PHY_INDEX);

		/*              */
		kalMemZero(&prGlueInfo->rTdlsHtCap, sizeof(prStaRec->rTdlsHtCap));
		kalMemZero(prGlueInfo->aucTdlsHtPeerMac, sizeof(prGlueInfo->aucTdlsHtPeerMac));

		DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: peer is a HT device\n", __FUNCTION__));
	}
#endif

	/*                                                    */
	prStaRec->fgIsWmmSupported = TRUE;
	prStaRec->fgIsUapsdSupported = TRUE;

	/*                                   */
	cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);

	/*             */
	GET_CURRENT_SYSTIME(&prStaRec->rTdlsSetupStartTime);

	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: create a peer ["MACSTR"]\n",
		__FUNCTION__, MAC2STR(prStaRec->aucMacAddr)));

	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                        
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
*/
/*                                                                            */
TDLS_STATUS
TdlsexPeerUpdate (
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
    )
{
	GLUE_INFO_T *prGlueInfo;
	TDLS_CMD_PEER_UPDATE_T *prCmd;
	BSS_INFO_T *prAisBssInfo;
	STA_RECORD_T *prStaRec;
	IE_HT_CAP_T *prHtCap;


	/*              */
	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s\n", __FUNCTION__));

	if ((prAdapter == NULL) || (pvSetBuffer == NULL) || (pu4SetInfoLen == NULL))
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cmd> %s: sanity fail!\n", __FUNCTION__));
		return TDLS_STATUS_FAILURE;
	}

	/*      */
	prGlueInfo = (GLUE_INFO_T *)prAdapter->prGlueInfo;
	*pu4SetInfoLen = sizeof(TDLS_CMD_PEER_ADD_T);
	prCmd = (TDLS_CMD_PEER_UPDATE_T *)pvSetBuffer;
	prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

	/*                  */
	prStaRec = cnmGetStaRecByAddress(prAdapter,
		(UINT_8) NETWORK_TYPE_AIS_INDEX,
		prCmd->aucPeerMac);

	/*
                               

                     
                             
                                                                                     
                                            
                                                         
                         
                                            

                     
                            
                                                                                     
                                            
                              
                            
                         
                                            
 */
	if ((prStaRec == NULL) || (prStaRec->fgIsInUse == 0))
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cmd> %s: cannot find the peer!\n", __FUNCTION__));
		return TDLS_STATUS_FAILURE;
	}

	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: update a peer ["MACSTR"] %d -> %d, 0x%x\n",
		__FUNCTION__, MAC2STR(prStaRec->aucMacAddr),
		prStaRec->ucStaState, STA_STATE_3, prStaRec->eStaType));

	if (!IS_TDLS_STA(prStaRec))
	{
		DBGLOG(TDLS, ERROR,
			("<tdls_cmd> %s: peer is not TDLS one!\n", __FUNCTION__));
		return TDLS_STATUS_FAILURE;
	}

	/*                                                                                 */
	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: ["MACSTR"] ["MACSTR"]\n",
		__FUNCTION__, MAC2STR(prGlueInfo->aucTdlsHtPeerMac),
		MAC2STR(prCmd->aucPeerMac)));

	if (kalMemCmp(prGlueInfo->aucTdlsHtPeerMac, prCmd->aucPeerMac, 6) == 0)
	{
		/*                                               */
		kalMemCopy(&prStaRec->rTdlsHtCap,
			&prGlueInfo->rTdlsHtCap, sizeof(IE_HT_CAP_T));

		prStaRec->ucPhyTypeSet |= PHY_TYPE_SET_802_11N;
		prStaRec->u2DesiredNonHTRateSet |= BIT(RATE_HT_PHY_INDEX);

		/*              */
		kalMemZero(&prGlueInfo->rTdlsHtCap, sizeof(prStaRec->rTdlsHtCap));
		kalMemZero(prGlueInfo->aucTdlsHtPeerMac, sizeof(prGlueInfo->aucTdlsHtPeerMac));

		DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: peer is a HT device\n", __FUNCTION__));
	}

	/*                              */
	GET_CURRENT_SYSTIME(&prStaRec->rUpdateTime);

	/*                                            */
	prStaRec->u2StatusCode = prCmd->u2StatusCode;

	/*                                           */

	prStaRec->u2CapInfo = prCmd->u2Capability;
/*                                */
	prStaRec->u2AssocId = 0; /*        */
	prStaRec->u2ListenInterval = 0; /*         */
/*                               */
/*                                 */
/*                             */
/*                    */
/*                      */
/*                       */
	prStaRec->fgIsQoS = TRUE;
	prStaRec->fgIsUapsdSupported = (prCmd->UapsdBitmap == 0)?FALSE:TRUE;
/*                        */
/*                           */
	prStaRec->u4TxBeamformingCap = 0; /*        */
	prStaRec->ucAselCap = 0; /*        */
	prStaRec->ucRCPI = 0;
	prStaRec->ucBmpTriggerAC = prCmd->UapsdBitmap;
	prStaRec->ucBmpDeliveryAC = prCmd->UapsdBitmap;
	prStaRec->ucUapsdSp = prCmd->UapsdMaxSp;

	/*           */
#if (TDLS_CFG_HT_SUP == 1)
	if (prCmd->fgIsSupHt == FALSE)
	{
		/*                                                  */
		prHtCap = (IE_HT_CAP_T *) &prStaRec->rTdlsHtCap;

		DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: ["MACSTR"] update ht ie 0x%x\n",
			__FUNCTION__, MAC2STR(prStaRec->aucMacAddr), prHtCap->ucId));

		if (prHtCap->ucId == ELEM_ID_HT_CAP)
		{
			prStaRec->ucMcsSet = prHtCap->rSupMcsSet.aucRxMcsBitmask[0];
			prStaRec->fgSupMcs32 =
				(prHtCap->rSupMcsSet.aucRxMcsBitmask[32/8] & BIT(0)) ?
				TRUE : FALSE;
			
			prStaRec->u2HtCapInfo = prHtCap->u2HtCapInfo;
			prStaRec->ucAmpduParam = prHtCap->ucAmpduParam;
			prStaRec->u2HtExtendedCap = prHtCap->u2HtExtendedCap;
			prStaRec->u4TxBeamformingCap = prHtCap->u4TxBeamformingCap;
			prStaRec->ucAselCap = prHtCap->ucAselCap;
			prStaRec->ucDesiredPhyTypeSet |= PHY_TYPE_SET_802_11N;
		}
	}
	else
	{
		/*                                     */
	}
#endif /*                 */

	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: UAPSD 0x%x %d MCS=0x%x\n",
		__FUNCTION__, prCmd->UapsdBitmap, prCmd->UapsdMaxSp,
		prStaRec->ucMcsSet));

//                                                        

	DBGLOG(TDLS, INFO, ("<tdls_cmd> %s: update a peer ["MACSTR"]\n",
		__FUNCTION__, MAC2STR(prStaRec->aucMacAddr)));

	return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                                                
 
                                                        
             
             
 
              
*/
/*                                                                            */
BOOLEAN
TdlsexRxFrameDrop(
	GLUE_INFO_T							*prGlueInfo,
	UINT_8								*pPkt
	)
{
	ADAPTER_T *prAdapter;
	UINT8 ucActionId;


	/*              */
	if ((pPkt == NULL) || (*(pPkt+12) != 0x89) || (*(pPkt+13) != 0x0d))
		return FALSE; /*                                   */

#if 0 /*                               */
	if (prStaRec == NULL)
		return FALSE; /*                   */

	DBGLOG(TDLS, INFO,
		("<tdls_fme> %s: Rcv a TDLS action frame (id=%d) %d %d\n",
		__FUNCTION__, *(pPkt+13+4),
		prStaRec->fgTdlsIsProhibited, fgIsPtiTimeoutSkip));

	/*                                                    */
	if (prStaRec->fgTdlsIsProhibited == TRUE)
		return TRUE;
#endif

	ucActionId = *(pPkt+12+2+2); /*                                                 */

	if (fgIsPtiTimeoutSkip == TRUE)
	{
		/*                                             */
		if (ucActionId == TDLS_FRM_ACTION_TEARDOWN)
			return TRUE;
	}

	prAdapter = prGlueInfo->prAdapter;
	DBGLOG(TDLS, INFO,
		("<tdls_fme> %s: Rcv a TDLS action frame %d (%u)\n",
		__FUNCTION__, ucActionId,
		(UINT32)prAdapter->rRxCtrl.rFreeSwRfbList.u4NumElem));

	if (ucActionId == TDLS_FRM_ACTION_TEARDOWN)
	{
		DBGLOG(TDLS, WARN,
			("<tdls_fme> %s: Rcv a TDLS tear down frame %d, will DISABLE link\n",
			__FUNCTION__, *(pPkt+13+4))); /*             */

		/*                           */
		TdlsLinkHistoryRecord(prGlueInfo, TRUE, pPkt+6, FALSE, *(pPkt+13+4), NULL);

		/*                                                       */
		/*
                                                                     
                                                                
  */
		cfg80211_tdls_oper_request(prGlueInfo->prDevHandler,
			pPkt+6, TDLS_FRM_ACTION_TEARDOWN, *(pPkt+13+4), GFP_ATOMIC);
	}

#if 0 /*                                                                   */
	if (((*(pPkt+13+3)) == TDLS_FRM_ACTION_PTI) ||
		((*(pPkt+13+3)) == TDLS_FRM_ACTION_CHAN_SWITCH_REQ) ||
		((*(pPkt+13+3)) == TDLS_FRM_ACTION_CHAN_SWITCH_RSP) ||
		((*(pPkt+13+3)) == TDLS_FRM_ACTION_PTI_RSP))
	{
		return TRUE;
	}
#endif

	return FALSE;
}


/*                                                                            */
/*                                                                                    
 
                                                          
                                                   
 
              
 
*/
/*                                                                            */
VOID
TdlsexRxFrameHandle(
	GLUE_INFO_T 						*prGlueInfo,
	UINT8								*pPkt,
	UINT16								u2PktLen
	)
{
	ADAPTER_T *prAdapter;
	STA_RECORD_T *prStaRec;
	UINT8 ucActionId;
	UINT8 *pucPeerMac, ucElmId, ucElmLen;
	INT16 s2FmeLen;


	/*              */
	if ((prGlueInfo == NULL) || (pPkt == NULL) ||
		(*(pPkt+12) != 0x89) || (*(pPkt+13) != 0x0d))
	{
		return;
	}

	ucActionId = *(pPkt+12+2+2); /*                                                 */

	if ((ucActionId != TDLS_FRM_ACTION_SETUP_REQ) &&
		(ucActionId != TDLS_FRM_ACTION_SETUP_RSP))
	{
		return;
	}

	/*      */
	prAdapter = prGlueInfo->prAdapter;
	pucPeerMac = pPkt+6;
	s2FmeLen = (INT16)u2PktLen;

	DBGLOG(TDLS, TRACE,
		("<tdls_fme> %s: get a setup frame %d from "MACSTR"\n",
		__FUNCTION__, ucActionId, MAC2STR(pucPeerMac)));

	if (ucActionId == TDLS_FRM_ACTION_SETUP_REQ)
		pPkt += 12+2+2+1+1+2; /*                                       */
	else
		pPkt += 12+2+2+1+2+1+2; /*                                                    */

	/*                      */
	prStaRec = cnmGetStaRecByAddress(prGlueInfo->prAdapter,
		(UINT_8)NETWORK_TYPE_AIS_INDEX, pucPeerMac);

	if (prStaRec == NULL)
	{
		prStaRec = cnmStaRecAlloc(prAdapter, (UINT_8) NETWORK_TYPE_AIS_INDEX);

		if (prStaRec == NULL)
		{
			/*                                                            */
			DBGLOG(TDLS, ERROR,
				("<tdls_cmd> %s: alloc prStaRec fail!\n", __FUNCTION__));
			return;
		}

		/*                   */
		/*                                                 */
		COPY_MAC_ADDR(prStaRec->aucMacAddr, pucPeerMac);

		cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);
	}

 	/*                                */
	/*                                            */
	while(s2FmeLen > 0)
	{
		ucElmId = *pPkt++;
		ucElmLen = *pPkt++;

		switch(ucElmId)
		{
			case ELEM_ID_HT_CAP: /*      */
				/*                                                       */
				if (prGlueInfo->rTdlsHtCap.ucId == 0x00)
				{
					kalMemCopy(prGlueInfo->aucTdlsHtPeerMac, pucPeerMac, 6);
					kalMemCopy(&prGlueInfo->rTdlsHtCap, pPkt-2, ucElmLen+2);

					/*
                                    

                             
                                
                                 
                                     
                              
     */

					DBGLOG(TDLS, TRACE,
						("<tdls_fme> %s: "MACSTR": find a HT IE\n",
						__FUNCTION__, MAC2STR(pucPeerMac)));
				}
				return;

			case ELEM_ID_EXTENDED_CAP:
				/*                                         */
				break;
		}

		pPkt += ucElmLen;
		s2FmeLen -= (2+ucElmLen);
	}
}
	

/*                                                                            */
/*                                                                
 
                                                         
                                                            
                                                
                  
 
                                                  
                                               
*/
/*                                                                            */
TDLS_STATUS
TdlsexStaRecIdxGet(
	ADAPTER_T							*prAdapter,
	MSDU_INFO_T							*prMsduInfo
	)
{
	BSS_INFO_T *prBssInfo;
	STA_RECORD_T *prStaRec;
	TDLS_STATUS Status;


	/*              */
	if (prAdapter->prGlueInfo == NULL)
		return TDLS_STATUS_FAILURE;
	if (TDLS_IS_NO_LINK_GOING(prAdapter->prGlueInfo))
		return TDLS_STATUS_FAILURE;

	if ((prAdapter == NULL) || (prMsduInfo == NULL))
		return TDLS_STATUS_FAILURE;

	/*      */
	prMsduInfo->ucStaRecIndex = STA_REC_INDEX_NOT_FOUND;
	Status = TDLS_STATUS_SUCCESS;

	/*                          */
	prStaRec = cnmGetStaRecByAddress(prAdapter,
								(UINT_8) NETWORK_TYPE_AIS_INDEX,
								prMsduInfo->aucEthDestAddr);

	/*
                                                                               
                                                                                    
          

                                                                            
                           
                                                                              
                                                                           
                           
 */

	do {
		if ((prStaRec != NULL) &&
			(prStaRec->ucStaState == STA_STATE_3) &&
			(IS_TDLS_STA(prStaRec)))
		{
			/*
                                
                                                           

                                     
                                                                             
                                                                               
                                                                          
                                                                           
                                                                               
                                              
   */
//                                  
			{
				/*
                                      
                                                            
                                                                   
    */
				struct sk_buff *prSkb;
				UINT8 *pEth;
				UINT_16 u2EtherTypeLen;

				prSkb = (struct sk_buff *) prMsduInfo->prPacket;
				if (prSkb != NULL)
				{
					UINT8 ucActionCode, ucReasonCode;

					/*      */
					pEth = prSkb->data;
					u2EtherTypeLen = (pEth[ETH_TYPE_LEN_OFFSET] << 8) |\
						(pEth[ETH_TYPE_LEN_OFFSET + 1]);
					ucActionCode = pEth[ETH_TYPE_LEN_OFFSET+1+3];
					ucReasonCode = pEth[ETH_TYPE_LEN_OFFSET+1+4] |\
						(pEth[ETH_TYPE_LEN_OFFSET+1+5] << 8);

					/*                                                              */
					if ((u2EtherTypeLen == TDLS_FRM_PROT_TYPE) &&
					    (ucActionCode == TDLS_FRM_ACTION_TEARDOWN) &&
                        (ucReasonCode == TDLS_REASON_CODE_UNREACHABLE))
					{
						/*
                                                                          
                                                       
                                        */
						prBssInfo = &(prAdapter->rWifiVar.arBssInfo[\
							NETWORK_TYPE_AIS_INDEX]);
						prStaRec = prBssInfo->prStaRecOfAP;
						if (prStaRec == NULL)
						{
							Status = TDLS_STATUS_FAILURE;
							break;
						}

#if 0
						/*                    */
						pEth[ETH_TYPE_LEN_OFFSET+1+4] = \
							TDLS_REASON_CODE_UNREACHABLE;
#endif
					}
				}
			}

			prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
		}
	} while(FALSE);

	DBGLOG(TDLS, INFO, ("<tdls> %s: (Status=%x) ["MACSTR"] ucStaRecIndex = %d!\n",
		__FUNCTION__, (INT32)Status, MAC2STR(prMsduInfo->aucEthDestAddr),
		prMsduInfo->ucStaRecIndex));
	return Status;
}


/*                                                                            */
/* 
                                                                                     
 
                                                         
 
                
*/
/*                                                                            */
VOID
TdlsexTxQuotaCheck(
	GLUE_INFO_T 						*prGlueInfo,
	STA_RECORD_T						*prStaRec,
	UINT8								FreeQuota
	)
{
	OS_SYSTIME rCurTime;


	/*              */
	if (!IS_TDLS_STA(prStaRec))
		return;

	if (FreeQuota != 0)
	{
		/*               */
		prStaRec->rTdlsTxQuotaEmptyTime = 0;
		return;
	}

	/*                                                          */
	GET_CURRENT_SYSTIME(&rCurTime);

	if (prStaRec->rTdlsTxQuotaEmptyTime == 0)
	{
		prStaRec->rTdlsTxQuotaEmptyTime = rCurTime;
	}
	else
	{
		if (CHECK_FOR_TIMEOUT(rCurTime, prStaRec->rTdlsTxQuotaEmptyTime,
			SEC_TO_SYSTIME(TDLS_TX_QUOTA_EMPTY_TIMEOUT)))
		{
			/*                    */
			DBGLOG(TDLS, WARN,
				("<tdls> %s: ["MACSTR"] TX quota empty timeout!\n",
				__FUNCTION__, MAC2STR(prStaRec->aucMacAddr)));

			/*                           */
			TdlsLinkHistoryRecord(prGlueInfo, TRUE, prStaRec->aucMacAddr,
				TRUE, TDLS_REASON_CODE_MTK_DIS_BY_US_DUE_TO_TX_QUOTA_EMPTY, NULL);
			
			/*                                                       */
			/*
                                                                      
                                                                 
   */
			cfg80211_tdls_oper_request(prGlueInfo->prDevHandler,
				prStaRec->aucMacAddr, TDLS_FRM_ACTION_TEARDOWN,
				TDLS_REASON_CODE_UNREACHABLE, GFP_ATOMIC);
		}
	}
}


/*                                                                            */
/* 
                                                                  
 
                                                         
 
                
*/
/*                                                                            */
VOID
TdlsexUninit (
	ADAPTER_T							*prAdapter
    )
{
#if TDLS_CFG_CMD_TEST
	cnmTimerStopTimer(prAdapter, &rTdlsTimerTestDataSend);
#endif /*                   */
}


#endif /*                  */

/*               */

