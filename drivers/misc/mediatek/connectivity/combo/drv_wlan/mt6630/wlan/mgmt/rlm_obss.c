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

/*                                                                              
                                                             
                                                                                
*/
static VOID rlmObssScanTimeout(P_ADAPTER_T prAdapter, ULONG ulParamPtr);

/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmObssInit(P_ADAPTER_T prAdapter)
{
	P_BSS_INFO_T prBssInfo;
	UINT_8 i;

	for (i = 0; i < BSS_INFO_NUM; i++) {
		prBssInfo = prAdapter->aprBssInfo[i];

		cnmTimerInitTimer(prAdapter, &prBssInfo->rObssScanTimer,
				  (PFN_MGMT_TIMEOUT_FUNC) rlmObssScanTimeout, (ULONG) prBssInfo);
	}
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
BOOLEAN rlmObssUpdateChnlLists(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb)
{
	return TRUE;
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmObssScanDone(P_ADAPTER_T prAdapter, P_MSG_HDR_T prMsgHdr)
{
	P_MSG_SCN_SCAN_DONE prScanDoneMsg;
	P_BSS_INFO_T prBssInfo;
	P_MSDU_INFO_T prMsduInfo;
	P_ACTION_20_40_COEXIST_FRAME prTxFrame;
	UINT_16 i, u2PayloadLen;

	ASSERT(prMsgHdr);

	prScanDoneMsg = (P_MSG_SCN_SCAN_DONE) prMsgHdr;
	prBssInfo = prAdapter->aprBssInfo[prScanDoneMsg->ucBssIndex];
	ASSERT(prBssInfo);

	DBGLOG(RLM, INFO, ("OBSS Scan Done (NetIdx=%d, Mode=%d)\n",
			   prScanDoneMsg->ucBssIndex, prBssInfo->eCurrentOPMode));

	cnmMemFree(prAdapter, prMsgHdr);

#if CFG_ENABLE_WIFI_DIRECT
	/*         */
	if ((prAdapter->fgIsP2PRegistered) &&
	    (IS_NET_ACTIVE(prAdapter, prBssInfo->ucBssIndex)) &&
	    (prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT)) {
		return;
	}
#endif

	/*          */
	if (prBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE ||
	    !RLM_NET_PARAM_VALID(prBssInfo) || prBssInfo->u2ObssScanInterval == 0) {
		DBGLOG(RLM, WARN, ("OBSS Scan Done (NetIdx=%d) -- Aborted!!\n",
				   prBssInfo->ucBssIndex));
		return;
	}

	/*                                                                
                                                                         
                                                                       
                                      
  */
	prMsduInfo = (P_MSDU_INFO_T) cnmMgtPktAlloc(prAdapter,
							 MAC_TX_RESERVED_FIELD + PUBLIC_ACTION_MAX_LEN);
	if ((prBssInfo->auc2G_20mReqChnlList[0] > 0 ||
	     prBssInfo->auc2G_NonHtChnlList[0] > 0) &&
	    prMsduInfo != NULL) {

		DBGLOG(RLM, INFO, ("Send 20/40 coexistence mgmt(20mReq=%d, NonHt=%d)\n",
				   prBssInfo->auc2G_20mReqChnlList[0],
				   prBssInfo->auc2G_NonHtChnlList[0]));

		prTxFrame = (P_ACTION_20_40_COEXIST_FRAME)
		    ((ULONG) (prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD);

		prTxFrame->u2FrameCtrl = MAC_FRAME_ACTION;
		COPY_MAC_ADDR(prTxFrame->aucDestAddr, prBssInfo->aucBSSID);
		COPY_MAC_ADDR(prTxFrame->aucSrcAddr, prBssInfo->aucOwnMacAddr);
		COPY_MAC_ADDR(prTxFrame->aucBSSID, prBssInfo->aucBSSID);

		prTxFrame->ucCategory = CATEGORY_PUBLIC_ACTION;
		prTxFrame->ucAction = ACTION_PUBLIC_20_40_COEXIST;

		/*                               */
		prTxFrame->rBssCoexist.ucId = ELEM_ID_20_40_BSS_COEXISTENCE;
		prTxFrame->rBssCoexist.ucLength = 1;
		prTxFrame->rBssCoexist.ucData =
		    (prBssInfo->auc2G_20mReqChnlList[0] > 0) ? BSS_COEXIST_20M_REQ : 0;

		u2PayloadLen = 2 + 3;

		if (prBssInfo->auc2G_NonHtChnlList[0] > 0) {
			ASSERT(prBssInfo->auc2G_NonHtChnlList[0] <= CHNL_LIST_SZ_2G);

			prTxFrame->rChnlReport.ucId = ELEM_ID_20_40_INTOLERANT_CHNL_REPORT;
			prTxFrame->rChnlReport.ucLength = prBssInfo->auc2G_NonHtChnlList[0] + 1;
			prTxFrame->rChnlReport.ucRegulatoryClass = 81;	/*                */
			for (i = 0; i < prBssInfo->auc2G_NonHtChnlList[0] &&
			     i < CHNL_LIST_SZ_2G; i++) {
				prTxFrame->rChnlReport.aucChannelList[i] =
				    prBssInfo->auc2G_NonHtChnlList[i + 1];
			}

			u2PayloadLen += IE_SIZE(&prTxFrame->rChnlReport);
		}
		ASSERT((WLAN_MAC_HEADER_LEN + u2PayloadLen) <= PUBLIC_ACTION_MAX_LEN);

		/*                                     */
		prBssInfo->auc2G_20mReqChnlList[0] = 0;
		prBssInfo->auc2G_NonHtChnlList[0] = 0;


		/*                                     */

		TX_SET_MMPDU(prAdapter,
			     prMsduInfo,
			     prBssInfo->ucBssIndex,
			     prBssInfo->prStaRecOfAP->ucIndex,
			     WLAN_MAC_MGMT_HEADER_LEN,
			     WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen, NULL, MSDU_RATE_MODE_AUTO);

		/*                                                */
		nicTxEnqueueMsdu(prAdapter, prMsduInfo);
	}
	/*                           */
	if (prBssInfo->u2ObssScanInterval > 0) {
		DBGLOG(RLM, INFO, ("Set OBSS timer (NetIdx=%d, %d sec)\n",
				   prBssInfo->ucBssIndex, prBssInfo->u2ObssScanInterval));

		cnmTimerStartTimer(prAdapter, &prBssInfo->rObssScanTimer,
				   prBssInfo->u2ObssScanInterval * MSEC_PER_SEC);
	}
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
static VOID rlmObssScanTimeout(P_ADAPTER_T prAdapter, ULONG ulParamPtr)
{
	P_BSS_INFO_T prBssInfo;

	prBssInfo = (P_BSS_INFO_T) ulParamPtr;
	ASSERT(prBssInfo);

#if CFG_ENABLE_WIFI_DIRECT
	/*         */
	if (prAdapter->fgIsP2PRegistered &&
	    (IS_NET_ACTIVE(prAdapter, prBssInfo->ucBssIndex)) &&
	    (prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT)) {

		prBssInfo->fgObssActionForcedTo20M = FALSE;

		/*                                            */
		rlmUpdateParamsForAP(prAdapter, prBssInfo, FALSE);

		return;
	}
#if CFG_SUPPORT_WFD
	/*               */
	else {
		P_WFD_CFG_SETTINGS_T prWfdCfgSettings = &prAdapter->rWifiVar.rWfdConfigureSettings;

		/*                               */
		if (prWfdCfgSettings->ucWfdEnable) {

			/*                */
			prBssInfo->u2ObssScanInterval = 0;
			DBGLOG(RLM, INFO, ("WFD is running. Stop OBSS scan.\n"));
			return;
		}		/*                */
	}
#endif
#endif				/*                               */


	/*          */
	if (prBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE ||
	    !RLM_NET_PARAM_VALID(prBssInfo) || prBssInfo->u2ObssScanInterval == 0) {
		DBGLOG(RLM, WARN, ("OBSS Scan timeout (NetIdx=%d) -- Aborted!!\n",
				   prBssInfo->ucBssIndex));
		return;
	}

	rlmObssTriggerScan(prAdapter, prBssInfo);
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmObssTriggerScan(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo)
{
	P_MSG_SCN_SCAN_REQ prScanReqMsg;

	ASSERT(prBssInfo);

	prScanReqMsg = (P_MSG_SCN_SCAN_REQ)
	    cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_SCN_SCAN_REQ));
	ASSERT(prScanReqMsg);

	if (!prScanReqMsg) {
		DBGLOG(RLM, WARN, ("No buf for OBSS scan (NetIdx=%d)!!\n", prBssInfo->ucBssIndex));

		cnmTimerStartTimer(prAdapter, &prBssInfo->rObssScanTimer,
				   prBssInfo->u2ObssScanInterval * MSEC_PER_SEC);
		return;
	}

	/*                                                                      
                                                                        
                                                      
  */
	prScanReqMsg->rMsgHdr.eMsgId = MID_RLM_SCN_SCAN_REQ;
	prScanReqMsg->ucSeqNum = 0x33;
	prScanReqMsg->ucBssIndex = prBssInfo->ucBssIndex;
	prScanReqMsg->eScanType = SCAN_TYPE_ACTIVE_SCAN;
	prScanReqMsg->ucSSIDType = SCAN_REQ_SSID_WILDCARD;
	prScanReqMsg->ucSSIDLength = 0;
	prScanReqMsg->eScanChannel = SCAN_CHANNEL_2G4;
	prScanReqMsg->u2IELen = 0;

	mboxSendMsg(prAdapter, MBOX_ID_0, (P_MSG_HDR_T) prScanReqMsg, MSG_SEND_METHOD_BUF);

	DBGLOG(RLM, INFO, ("Timeout to trigger OBSS scan (NetIdx=%d)!!\n", prBssInfo->ucBssIndex));
}
