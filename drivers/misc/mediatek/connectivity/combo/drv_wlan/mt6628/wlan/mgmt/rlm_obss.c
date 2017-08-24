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
static VOID rlmObssScanTimeout(P_ADAPTER_T prAdapter, UINT_32 u4Data);

/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmObssInit(P_ADAPTER_T prAdapter)
{
	P_BSS_INFO_T prBssInfo;
	UINT_8 ucNetIdx;

	RLM_NET_FOR_EACH(ucNetIdx) {
		prBssInfo = &prAdapter->rWifiVar.arBssInfo[ucNetIdx];
		ASSERT(prBssInfo);

		cnmTimerInitTimer(prAdapter, &prBssInfo->rObssScanTimer,
				  rlmObssScanTimeout, (UINT_32) prBssInfo);
	}			/*                         */
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
	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prScanDoneMsg->ucNetTypeIndex];
	ASSERT(prBssInfo);

	DBGLOG(RLM, INFO, ("OBSS Scan Done (NetIdx=%d, Mode=%d)\n",
			   prScanDoneMsg->ucNetTypeIndex, prBssInfo->eCurrentOPMode));

	cnmMemFree(prAdapter, prMsgHdr);

#if CFG_ENABLE_WIFI_DIRECT
	/*         */
	if ((prAdapter->fgIsP2PRegistered) &&
	    (IS_NET_ACTIVE(prAdapter, prBssInfo->ucNetTypeIndex)) &&
	    (prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT)) {
		return;
	}
#endif

	/*          */
	if (prBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE ||
	    !RLM_NET_PARAM_VALID(prBssInfo) || prBssInfo->u2ObssScanInterval == 0) {
		DBGLOG(RLM, WARN, ("OBSS Scan Done (NetIdx=%d) -- Aborted!!\n",
				   prBssInfo->ucNetTypeIndex));
		return;
	}

	/*                                                                
                                                                         
                                                                       
                                      
  */
	if ((prBssInfo->auc2G_20mReqChnlList[0] > 0 ||
	     prBssInfo->auc2G_NonHtChnlList[0] > 0) &&
	    (prMsduInfo = (P_MSDU_INFO_T) cnmMgtPktAlloc(prAdapter,
							 MAC_TX_RESERVED_FIELD +
							 PUBLIC_ACTION_MAX_LEN)) != NULL) {

		DBGLOG(RLM, INFO, ("Send 20/40 coexistence mgmt(20mReq=%d, NonHt=%d)\n",
				   prBssInfo->auc2G_20mReqChnlList[0],
				   prBssInfo->auc2G_NonHtChnlList[0]));

		prTxFrame = (P_ACTION_20_40_COEXIST_FRAME)
		    ((UINT_32) (prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD);

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
		prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;	/*                  */
		prMsduInfo->ucStaRecIndex = prBssInfo->prStaRecOfAP->ucIndex;
		prMsduInfo->ucNetworkType = prBssInfo->ucNetTypeIndex;
		prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
		prMsduInfo->fgIs802_1x = FALSE;
		prMsduInfo->fgIs802_11 = TRUE;
		prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen;
		prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
		prMsduInfo->pfTxDoneHandler = NULL;
		prMsduInfo->fgIsBasicRate = FALSE;

		/*                                                */
		nicTxEnqueueMsdu(prAdapter, prMsduInfo);
	}
	/*                           */
	if (prBssInfo->u2ObssScanInterval > 0) {
		DBGLOG(RLM, INFO, ("Set OBSS timer (NetIdx=%d, %d sec)\n",
				   prBssInfo->ucNetTypeIndex, prBssInfo->u2ObssScanInterval));

		cnmTimerStartTimer(prAdapter, &prBssInfo->rObssScanTimer,
				   prBssInfo->u2ObssScanInterval * MSEC_PER_SEC);
	}
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
static VOID rlmObssScanTimeout(P_ADAPTER_T prAdapter, UINT_32 u4Data)
{
	P_BSS_INFO_T prBssInfo;

	prBssInfo = (P_BSS_INFO_T) u4Data;
	ASSERT(prBssInfo);

#if CFG_ENABLE_WIFI_DIRECT
	/*         */
	if (prAdapter->fgIsP2PRegistered &&
	    (IS_NET_ACTIVE(prAdapter, prBssInfo->ucNetTypeIndex)) &&
	    (prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT)) {

		prBssInfo->fgObssActionForcedTo20M = FALSE;

		/*                                            */
		rlmUpdateParamsForAP(prAdapter, prBssInfo, FALSE);

		return;
	}
#endif				/*                               */


	/*          */
	if (prBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE ||
	    !RLM_NET_PARAM_VALID(prBssInfo) || prBssInfo->u2ObssScanInterval == 0) {
		DBGLOG(RLM, WARN, ("OBSS Scan timeout (NetIdx=%d) -- Aborted!!\n",
				   prBssInfo->ucNetTypeIndex));
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
		DBGLOG(RLM, WARN, ("No buf for OBSS scan (NetIdx=%d)!!\n",
				   prBssInfo->ucNetTypeIndex));

		cnmTimerStartTimer(prAdapter, &prBssInfo->rObssScanTimer,
				   prBssInfo->u2ObssScanInterval * MSEC_PER_SEC);
		return;
	}

	/*                                                                      
                                                                        
                                                      
  */
	prScanReqMsg->rMsgHdr.eMsgId = MID_RLM_SCN_SCAN_REQ;
	prScanReqMsg->ucSeqNum = 0x33;
	prScanReqMsg->ucNetTypeIndex = prBssInfo->ucNetTypeIndex;
	prScanReqMsg->eScanType = SCAN_TYPE_ACTIVE_SCAN;
	prScanReqMsg->ucSSIDType = SCAN_REQ_SSID_WILDCARD;
	prScanReqMsg->ucSSIDLength = 0;
	prScanReqMsg->eScanChannel = SCAN_CHANNEL_2G4;
	prScanReqMsg->u2IELen = 0;

	mboxSendMsg(prAdapter, MBOX_ID_0, (P_MSG_HDR_T) prScanReqMsg, MSG_SEND_METHOD_BUF);

	DBGLOG(RLM, INFO, ("Timeout to trigger OBSS scan (NetIdx=%d)!!\n",
			   prBssInfo->ucNetTypeIndex));
}
