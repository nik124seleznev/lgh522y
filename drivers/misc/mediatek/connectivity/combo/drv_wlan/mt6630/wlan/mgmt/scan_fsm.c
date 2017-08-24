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
#if DBG
/*                              */
static PUINT_8 apucDebugScanState[SCAN_STATE_NUM] = {
	(PUINT_8) DISP_STRING("SCAN_STATE_IDLE"),
	(PUINT_8) DISP_STRING("SCAN_STATE_SCANNING"),
};

/*              */
#endif				/*     */

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID scnFsmSteps(IN P_ADAPTER_T prAdapter, IN ENUM_SCAN_STATE_T eNextState)
{
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;
	P_MSG_HDR_T prMsgHdr;

	BOOLEAN fgIsTransition = (BOOLEAN) FALSE;

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;

	do {

#if DBG
		DBGLOG(SCN, STATE, ("TRANSITION: [%s] -> [%s]\n",
				    apucDebugScanState[prScanInfo->eCurrentState],
				    apucDebugScanState[eNextState]));
#else
		DBGLOG(SCN, STATE, ("[%d] TRANSITION: [%d] -> [%d]\n",
				    DBG_SCN_IDX, prScanInfo->eCurrentState, eNextState));
#endif

		/*                                                                                 */
		prScanInfo->eCurrentState = eNextState;

		fgIsTransition = (BOOLEAN) FALSE;

		switch (prScanInfo->eCurrentState) {
		case SCAN_STATE_IDLE:
			/*                                     */
			if (!LINK_IS_EMPTY(&(prScanInfo->rPendingMsgList))) {
				/*                                                        */
				LINK_REMOVE_HEAD(&(prScanInfo->rPendingMsgList), prMsgHdr,
						 P_MSG_HDR_T);

				if (prMsgHdr->eMsgId == MID_AIS_SCN_SCAN_REQ
				    || prMsgHdr->eMsgId == MID_BOW_SCN_SCAN_REQ
				    || prMsgHdr->eMsgId == MID_P2P_SCN_SCAN_REQ
				    || prMsgHdr->eMsgId == MID_RLM_SCN_SCAN_REQ) {
					scnFsmHandleScanMsg(prAdapter,
							    (P_MSG_SCN_SCAN_REQ) prMsgHdr);

					eNextState = SCAN_STATE_SCANNING;
					fgIsTransition = TRUE;
				} else if (prMsgHdr->eMsgId == MID_AIS_SCN_SCAN_REQ_V2
					   || prMsgHdr->eMsgId == MID_BOW_SCN_SCAN_REQ_V2
					   || prMsgHdr->eMsgId == MID_P2P_SCN_SCAN_REQ_V2
					   || prMsgHdr->eMsgId == MID_RLM_SCN_SCAN_REQ_V2) {
					scnFsmHandleScanMsgV2(prAdapter,
							      (P_MSG_SCN_SCAN_REQ_V2) prMsgHdr);

					eNextState = SCAN_STATE_SCANNING;
					fgIsTransition = TRUE;
				} else {
					/*                   */
					ASSERT(0);
				}

				/*                      */
				cnmMemFree(prAdapter, prMsgHdr);
			}
			break;

		case SCAN_STATE_SCANNING:
			if (prScanParam->fgIsScanV2 == FALSE) {
				scnSendScanReq(prAdapter);
			} else {
				scnSendScanReqV2(prAdapter);
			}
			break;

		default:
			ASSERT(0);
			break;

		}
	} while (fgIsTransition);

	return;
}


/*                                                                            */
/* 
                                                
 
            
 
              
*/
/*                                                                            */
VOID scnSendScanReq(IN P_ADAPTER_T prAdapter)
{
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;
	CMD_SCAN_REQ rCmdScanReq;
	UINT_32 i;

	ASSERT(prAdapter);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;

	/*                              */
	kalMemZero(&rCmdScanReq, sizeof(CMD_SCAN_REQ));

	rCmdScanReq.ucSeqNum = prScanParam->ucSeqNum;
	rCmdScanReq.ucBssIndex = prScanParam->ucBssIndex;
	rCmdScanReq.ucScanType = (UINT_8) prScanParam->eScanType;
	rCmdScanReq.ucSSIDType = prScanParam->ucSSIDType;

	if (prScanParam->ucSSIDNum == 1) {
		COPY_SSID(rCmdScanReq.aucSSID,
			  rCmdScanReq.ucSSIDLength,
			  prScanParam->aucSpecifiedSSID[0], prScanParam->ucSpecifiedSSIDLen[0]);
	}

	rCmdScanReq.ucChannelType = (UINT_8) prScanParam->eScanChannel;

	if (prScanParam->eScanChannel == SCAN_CHANNEL_SPECIFIED) {
		/*               
                                                                  
                                                                                                   
   */
		rCmdScanReq.ucChannelListNum = prScanParam->ucChannelListNum;

		for (i = 0; i < rCmdScanReq.ucChannelListNum; i++) {
			rCmdScanReq.arChannelList[i].ucBand =
			    (UINT_8) prScanParam->arChnlInfoList[i].eBand;

			rCmdScanReq.arChannelList[i].ucChannelNum =
			    (UINT_8) prScanParam->arChnlInfoList[i].ucChannelNum;
		}
	}

	rCmdScanReq.u2ChannelDwellTime = prScanParam->u2ChannelDwellTime;
	rCmdScanReq.u2TimeoutValue = prScanParam->u2TimeoutValue;

	if (prScanParam->u2IELen <= MAX_IE_LENGTH) {
		rCmdScanReq.u2IELen = prScanParam->u2IELen;
	} else {
		rCmdScanReq.u2IELen = MAX_IE_LENGTH;
	}

	if (prScanParam->u2IELen) {
		kalMemCopy(rCmdScanReq.aucIE,
			   prScanParam->aucIE, sizeof(UINT_8) * rCmdScanReq.u2IELen);
	}

	wlanSendSetQueryCmd(prAdapter,
			    CMD_ID_SCAN_REQ,
			    TRUE,
			    FALSE,
			    FALSE,
			    NULL,
			    NULL,
			    OFFSET_OF(CMD_SCAN_REQ, aucIE) + rCmdScanReq.u2IELen,
			    (PUINT_8) &rCmdScanReq, NULL, 0);
}


/*                                                                            */
/* 
                                                   
 
            
 
              
*/
/*                                                                            */
VOID scnSendScanReqV2(IN P_ADAPTER_T prAdapter)
{
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;
	CMD_SCAN_REQ_V2 rCmdScanReq;
	UINT_32 i;

	ASSERT(prAdapter);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;

	/*                              */
	kalMemZero(&rCmdScanReq, sizeof(CMD_SCAN_REQ_V2));

	rCmdScanReq.ucSeqNum = prScanParam->ucSeqNum;
	rCmdScanReq.ucBssIndex = prScanParam->ucBssIndex;
	rCmdScanReq.ucScanType = (UINT_8) prScanParam->eScanType;
	rCmdScanReq.ucSSIDType = prScanParam->ucSSIDType;

	for (i = 0; i < prScanParam->ucSSIDNum; i++) {
		COPY_SSID(rCmdScanReq.arSSID[i].aucSsid,
			  rCmdScanReq.arSSID[i].u4SsidLen,
			  prScanParam->aucSpecifiedSSID[i], prScanParam->ucSpecifiedSSIDLen[i]);
	}

	rCmdScanReq.u2ProbeDelayTime = (UINT_8) prScanParam->u2ProbeDelayTime;
	rCmdScanReq.ucChannelType = (UINT_8) prScanParam->eScanChannel;

	if (prScanParam->eScanChannel == SCAN_CHANNEL_SPECIFIED) {
		/*               
                                                                  
                                                                                                   
   */
		rCmdScanReq.ucChannelListNum = prScanParam->ucChannelListNum;

		for (i = 0; i < rCmdScanReq.ucChannelListNum; i++) {
			rCmdScanReq.arChannelList[i].ucBand =
			    (UINT_8) prScanParam->arChnlInfoList[i].eBand;

			rCmdScanReq.arChannelList[i].ucChannelNum =
			    (UINT_8) prScanParam->arChnlInfoList[i].ucChannelNum;
		}
	}

	rCmdScanReq.u2ChannelDwellTime = prScanParam->u2ChannelDwellTime;
	rCmdScanReq.u2TimeoutValue = prScanParam->u2TimeoutValue;

	if (prScanParam->u2IELen <= MAX_IE_LENGTH) {
		rCmdScanReq.u2IELen = prScanParam->u2IELen;
	} else {
		rCmdScanReq.u2IELen = MAX_IE_LENGTH;
	}

	if (prScanParam->u2IELen) {
		kalMemCopy(rCmdScanReq.aucIE,
			   prScanParam->aucIE, sizeof(UINT_8) * rCmdScanReq.u2IELen);
	}

	wlanSendSetQueryCmd(prAdapter,
			    CMD_ID_SCAN_REQ_V2,
			    TRUE,
			    FALSE,
			    FALSE,
			    NULL,
			    NULL,
			    OFFSET_OF(CMD_SCAN_REQ_V2, aucIE) + rCmdScanReq.u2IELen,
			    (PUINT_8) &rCmdScanReq, NULL, 0);

}


/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID scnFsmMsgStart(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr)
{
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;

	ASSERT(prMsgHdr);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;


	if (prScanInfo->eCurrentState == SCAN_STATE_IDLE) {
		if (prMsgHdr->eMsgId == MID_AIS_SCN_SCAN_REQ
		    || prMsgHdr->eMsgId == MID_BOW_SCN_SCAN_REQ
		    || prMsgHdr->eMsgId == MID_P2P_SCN_SCAN_REQ
		    || prMsgHdr->eMsgId == MID_RLM_SCN_SCAN_REQ) {
			scnFsmHandleScanMsg(prAdapter, (P_MSG_SCN_SCAN_REQ) prMsgHdr);
		} else if (prMsgHdr->eMsgId == MID_AIS_SCN_SCAN_REQ_V2
			   || prMsgHdr->eMsgId == MID_BOW_SCN_SCAN_REQ_V2
			   || prMsgHdr->eMsgId == MID_P2P_SCN_SCAN_REQ_V2
			   || prMsgHdr->eMsgId == MID_RLM_SCN_SCAN_REQ_V2) {
			scnFsmHandleScanMsgV2(prAdapter, (P_MSG_SCN_SCAN_REQ_V2) prMsgHdr);
		} else {
			/*                                     */
			ASSERT(0);
		}

		cnmMemFree(prAdapter, prMsgHdr);
		scnFsmSteps(prAdapter, SCAN_STATE_SCANNING);
	} else {
		LINK_INSERT_TAIL(&prScanInfo->rPendingMsgList, &prMsgHdr->rLinkEntry);
	}

	return;
}



/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID scnFsmMsgAbort(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr)
{
	P_MSG_SCN_SCAN_CANCEL prScanCancel;
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;
	CMD_SCAN_CANCEL rCmdScanCancel;

	ASSERT(prMsgHdr);

	prScanCancel = (P_MSG_SCN_SCAN_CANCEL) prMsgHdr;
	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;

	if (prScanInfo->eCurrentState != SCAN_STATE_IDLE) {
		if (prScanCancel->ucSeqNum == prScanParam->ucSeqNum &&
		    prScanCancel->ucBssIndex == prScanParam->ucBssIndex) {
			/*                                        */
			rCmdScanCancel.ucSeqNum = prScanParam->ucSeqNum;
			rCmdScanCancel.ucIsExtChannel = (UINT_8) prScanCancel->fgIsChannelExt;

			wlanSendSetQueryCmd(prAdapter,
					    CMD_ID_SCAN_CANCEL,
					    TRUE,
					    FALSE,
					    FALSE,
					    NULL,
					    NULL,
					    sizeof(CMD_SCAN_CANCEL),
					    (PUINT_8) &rCmdScanCancel, NULL, 0);

			/*                                     */
			scnFsmGenerateScanDoneMsg(prAdapter,
						  prScanParam->ucSeqNum,
						  prScanParam->ucBssIndex, SCAN_STATUS_CANCELLED);

			/*                             */
			scnFsmSteps(prAdapter, SCAN_STATE_IDLE);
		} else {
			scnFsmRemovePendingMsg(prAdapter, prScanCancel->ucSeqNum,
					       prScanCancel->ucBssIndex);
		}
	}

	cnmMemFree(prAdapter, prMsgHdr);

	return;
}


/*                                                                            */
/* 
                                                 
 
            
 
              
*/
/*                                                                            */
VOID scnFsmHandleScanMsg(IN P_ADAPTER_T prAdapter, IN P_MSG_SCN_SCAN_REQ prScanReqMsg)
{
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;
	UINT_32 i;

	ASSERT(prAdapter);
	ASSERT(prScanReqMsg);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;

	prScanParam->eScanType = prScanReqMsg->eScanType;
	prScanParam->ucBssIndex = prScanReqMsg->ucBssIndex;
	prScanParam->ucSSIDType = prScanReqMsg->ucSSIDType;
	if (prScanParam->ucSSIDType & (SCAN_REQ_SSID_SPECIFIED | SCAN_REQ_SSID_P2P_WILDCARD)) {
		prScanParam->ucSSIDNum = 1;

		COPY_SSID(prScanParam->aucSpecifiedSSID[0],
			  prScanParam->ucSpecifiedSSIDLen[0],
			  prScanReqMsg->aucSSID, prScanReqMsg->ucSSIDLength);

		/*                                                  */
		for (i = 1; i < SCN_SSID_MAX_NUM; i++) {
			prScanParam->ucSpecifiedSSIDLen[i] = 0;
		}
	} else {
		prScanParam->ucSSIDNum = 0;

		for (i = 0; i < SCN_SSID_MAX_NUM; i++) {
			prScanParam->ucSpecifiedSSIDLen[i] = 0;
		}
	}

	prScanParam->u2ProbeDelayTime = 0;
	prScanParam->eScanChannel = prScanReqMsg->eScanChannel;
	if (prScanParam->eScanChannel == SCAN_CHANNEL_SPECIFIED) {
		if (prScanReqMsg->ucChannelListNum <= MAXIMUM_OPERATION_CHANNEL_LIST) {
			prScanParam->ucChannelListNum = prScanReqMsg->ucChannelListNum;
		} else {
			prScanParam->ucChannelListNum = MAXIMUM_OPERATION_CHANNEL_LIST;
		}

		kalMemCopy(prScanParam->arChnlInfoList,
			   prScanReqMsg->arChnlInfoList,
			   sizeof(RF_CHANNEL_INFO_T) * prScanParam->ucChannelListNum);
	}

	if (prScanReqMsg->u2IELen <= MAX_IE_LENGTH) {
		prScanParam->u2IELen = prScanReqMsg->u2IELen;
	} else {
		prScanParam->u2IELen = MAX_IE_LENGTH;
	}

	if (prScanParam->u2IELen) {
		kalMemCopy(prScanParam->aucIE, prScanReqMsg->aucIE, prScanParam->u2IELen);
	}

	prScanParam->u2ChannelDwellTime = prScanReqMsg->u2ChannelDwellTime;
	prScanParam->u2TimeoutValue = prScanReqMsg->u2TimeoutValue;
	prScanParam->ucSeqNum = prScanReqMsg->ucSeqNum;

	if (prScanReqMsg->rMsgHdr.eMsgId == MID_RLM_SCN_SCAN_REQ) {
		prScanParam->fgIsObssScan = TRUE;
	} else {
		prScanParam->fgIsObssScan = FALSE;
	}

	prScanParam->fgIsScanV2 = FALSE;

	return;
}


/*                                                                            */
/* 
                                                                        
 
            
 
              
*/
/*                                                                            */
VOID scnFsmHandleScanMsgV2(IN P_ADAPTER_T prAdapter, IN P_MSG_SCN_SCAN_REQ_V2 prScanReqMsg)
{
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;
	UINT_32 i;

	ASSERT(prAdapter);
	ASSERT(prScanReqMsg);
	ASSERT(prScanReqMsg->ucSSIDNum <= SCN_SSID_MAX_NUM);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;

	prScanParam->eScanType = prScanReqMsg->eScanType;
	prScanParam->ucBssIndex = prScanReqMsg->ucBssIndex;
	prScanParam->ucSSIDType = prScanReqMsg->ucSSIDType;
	prScanParam->ucSSIDNum = prScanReqMsg->ucSSIDNum;

	for (i = 0; i < prScanReqMsg->ucSSIDNum; i++) {
		COPY_SSID(prScanParam->aucSpecifiedSSID[i],
			  prScanParam->ucSpecifiedSSIDLen[i],
			  prScanReqMsg->prSsid[i].aucSsid,
			  (UINT_8) prScanReqMsg->prSsid[i].u4SsidLen);
	}

	prScanParam->u2ProbeDelayTime = prScanReqMsg->u2ProbeDelay;
	prScanParam->eScanChannel = prScanReqMsg->eScanChannel;
	if (prScanParam->eScanChannel == SCAN_CHANNEL_SPECIFIED) {
		if (prScanReqMsg->ucChannelListNum <= MAXIMUM_OPERATION_CHANNEL_LIST) {
			prScanParam->ucChannelListNum = prScanReqMsg->ucChannelListNum;
		} else {
			prScanParam->ucChannelListNum = MAXIMUM_OPERATION_CHANNEL_LIST;
		}

		kalMemCopy(prScanParam->arChnlInfoList,
			   prScanReqMsg->arChnlInfoList,
			   sizeof(RF_CHANNEL_INFO_T) * prScanParam->ucChannelListNum);
	}

	if (prScanReqMsg->u2IELen <= MAX_IE_LENGTH) {
		prScanParam->u2IELen = prScanReqMsg->u2IELen;
	} else {
		prScanParam->u2IELen = MAX_IE_LENGTH;
	}

	if (prScanParam->u2IELen) {
		kalMemCopy(prScanParam->aucIE, prScanReqMsg->aucIE, prScanParam->u2IELen);
	}

	prScanParam->u2ChannelDwellTime = prScanReqMsg->u2ChannelDwellTime;
	prScanParam->u2TimeoutValue = prScanReqMsg->u2TimeoutValue;
	prScanParam->ucSeqNum = prScanReqMsg->ucSeqNum;

	if (prScanReqMsg->rMsgHdr.eMsgId == MID_RLM_SCN_SCAN_REQ) {
		prScanParam->fgIsObssScan = TRUE;
	} else {
		prScanParam->fgIsObssScan = FALSE;
	}

	prScanParam->fgIsScanV2 = TRUE;

	return;
}


/*                                                                            */
/* 
                                               
 
            
 
              
*/
/*                                                                            */
VOID scnFsmRemovePendingMsg(IN P_ADAPTER_T prAdapter, IN UINT_8 ucSeqNum, IN UINT_8 ucBssIndex)
{
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;
	P_MSG_HDR_T prPendingMsgHdr, prPendingMsgHdrNext, prRemoveMsgHdr = NULL;
	P_LINK_ENTRY_T prRemoveLinkEntry = NULL;
	BOOLEAN fgIsRemovingScan = FALSE;

	ASSERT(prAdapter);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;

	/*                                              */
	LINK_FOR_EACH_ENTRY_SAFE(prPendingMsgHdr,
				 prPendingMsgHdrNext,
				 &(prScanInfo->rPendingMsgList), rLinkEntry, MSG_HDR_T) {
		if (prPendingMsgHdr->eMsgId == MID_AIS_SCN_SCAN_REQ
		    || prPendingMsgHdr->eMsgId == MID_BOW_SCN_SCAN_REQ
		    || prPendingMsgHdr->eMsgId == MID_P2P_SCN_SCAN_REQ
		    || prPendingMsgHdr->eMsgId == MID_RLM_SCN_SCAN_REQ) {
			P_MSG_SCN_SCAN_REQ prScanReqMsg = (P_MSG_SCN_SCAN_REQ) prPendingMsgHdr;

			if (ucSeqNum == prScanReqMsg->ucSeqNum &&
			    ucBssIndex == prScanReqMsg->ucBssIndex) {
				prRemoveLinkEntry = &(prScanReqMsg->rMsgHdr.rLinkEntry);
				prRemoveMsgHdr = prPendingMsgHdr;
				fgIsRemovingScan = TRUE;
			}
		} else if (prPendingMsgHdr->eMsgId == MID_AIS_SCN_SCAN_REQ_V2
			   || prPendingMsgHdr->eMsgId == MID_BOW_SCN_SCAN_REQ_V2
			   || prPendingMsgHdr->eMsgId == MID_P2P_SCN_SCAN_REQ_V2
			   || prPendingMsgHdr->eMsgId == MID_RLM_SCN_SCAN_REQ_V2) {
			P_MSG_SCN_SCAN_REQ_V2 prScanReqMsgV2 =
			    (P_MSG_SCN_SCAN_REQ_V2) prPendingMsgHdr;

			if (ucSeqNum == prScanReqMsgV2->ucSeqNum &&
			    ucBssIndex == prScanReqMsgV2->ucBssIndex) {
				prRemoveLinkEntry = &(prScanReqMsgV2->rMsgHdr.rLinkEntry);
				prRemoveMsgHdr = prPendingMsgHdr;
				fgIsRemovingScan = TRUE;
			}
		}

		if (prRemoveLinkEntry) {
			if (fgIsRemovingScan == TRUE) {
				/*                                     */
				scnFsmGenerateScanDoneMsg(prAdapter,
							  ucSeqNum,
							  ucBssIndex, SCAN_STATUS_CANCELLED);
			}

			/*                          */
			LINK_REMOVE_KNOWN_ENTRY(&(prScanInfo->rPendingMsgList), prRemoveLinkEntry);
			cnmMemFree(prAdapter, prRemoveMsgHdr);

			break;
		}
	}

	return;
}


/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID scnEventScanDone(IN P_ADAPTER_T prAdapter, IN P_EVENT_SCAN_DONE prScanDone)
{
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;

	/*                                  */
	if (prScanParam->eScanChannel == SCAN_CHANNEL_FULL
	    || prScanParam->eScanChannel == SCAN_CHANNEL_2G4) {
		if (prScanDone->ucSparseChannelValid) {
			prScanInfo->fgIsSparseChannelValid = TRUE;
			prScanInfo->rSparseChannel.eBand =
			    (ENUM_BAND_T) prScanDone->rSparseChannel.ucBand;
			prScanInfo->rSparseChannel.ucChannelNum =
			    prScanDone->rSparseChannel.ucChannelNum;
		} else {
			prScanInfo->fgIsSparseChannelValid = FALSE;
		}
	}

	if (prScanInfo->eCurrentState == SCAN_STATE_SCANNING &&
	    prScanDone->ucSeqNum == prScanParam->ucSeqNum) {
		/*                                     */
		scnFsmGenerateScanDoneMsg(prAdapter,
					  prScanParam->ucSeqNum,
					  prScanParam->ucBssIndex, SCAN_STATUS_DONE);

		/*                             */
		scnFsmSteps(prAdapter, SCAN_STATE_IDLE);
	} else {
		DBGLOG(SCN, LOUD, ("Unexpected SCAN-DONE event: SeqNum = %d, Current State = %d\n",
				   prScanDone->ucSeqNum, prScanInfo->eCurrentState));
	}

	return;
}				/*                         */


/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID
scnFsmGenerateScanDoneMsg(IN P_ADAPTER_T prAdapter,
			  IN UINT_8 ucSeqNum, IN UINT_8 ucBssIndex, IN ENUM_SCAN_STATUS eScanStatus)
{
	P_SCAN_INFO_T prScanInfo;
	P_SCAN_PARAM_T prScanParam;
	P_MSG_SCN_SCAN_DONE prScanDoneMsg;

	ASSERT(prAdapter);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prScanParam = &prScanInfo->rScanParam;

	prScanDoneMsg =
	    (P_MSG_SCN_SCAN_DONE) cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_SCN_SCAN_DONE));
	if (!prScanDoneMsg) {
		ASSERT(0);	/*                                  */
		return;
	}

	if (prScanParam->fgIsObssScan == TRUE) {
		prScanDoneMsg->rMsgHdr.eMsgId = MID_SCN_RLM_SCAN_DONE;
	} else {
		switch (GET_BSS_INFO_BY_INDEX(prAdapter, ucBssIndex)->eNetworkType) {
		case NETWORK_TYPE_AIS:
			prScanDoneMsg->rMsgHdr.eMsgId = MID_SCN_AIS_SCAN_DONE;
			break;

		case NETWORK_TYPE_P2P:
			prScanDoneMsg->rMsgHdr.eMsgId = MID_SCN_P2P_SCAN_DONE;
			break;

		case NETWORK_TYPE_BOW:
			prScanDoneMsg->rMsgHdr.eMsgId = MID_SCN_BOW_SCAN_DONE;
			break;

		default:
			DBGLOG(SCN, LOUD,
			       ("Unexpected Network Type: %d\n",
				GET_BSS_INFO_BY_INDEX(prAdapter, ucBssIndex)->eNetworkType));
			ASSERT(0);
			break;
		}
	}

	prScanDoneMsg->ucSeqNum = ucSeqNum;
	prScanDoneMsg->ucBssIndex = ucBssIndex;
	prScanDoneMsg->eScanStatus = eScanStatus;

	mboxSendMsg(prAdapter, MBOX_ID_0, (P_MSG_HDR_T) prScanDoneMsg, MSG_SEND_METHOD_BUF);

}				/*                                    */


/*                                                                            */
/* 
                                             
 
            
 
              
*/
/*                                                                            */
BOOLEAN
scnQuerySparseChannel(IN P_ADAPTER_T prAdapter,
		      P_ENUM_BAND_T prSparseBand, PUINT_8 pucSparseChannel)
{
	P_SCAN_INFO_T prScanInfo;

	ASSERT(prAdapter);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);

	if (prScanInfo->fgIsSparseChannelValid == TRUE) {
		if (prSparseBand) {
			*prSparseBand = prScanInfo->rSparseChannel.eBand;
		}

		if (pucSparseChannel) {
			*pucSparseChannel = prScanInfo->rSparseChannel.ucChannelNum;
		}

		return TRUE;
	} else {
		return FALSE;
	}
}


/*                                                                            */
/* 
                                                
 
            
 
              
*/
/*                                                                            */
VOID scnEventNloDone(IN P_ADAPTER_T prAdapter, IN P_EVENT_NLO_DONE_T prNloDone)
{
	P_SCAN_INFO_T prScanInfo;
	P_NLO_PARAM_T prNloParam;
	P_SCAN_PARAM_T prScanParam;

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prNloParam = &prScanInfo->rNloParam;
	prScanParam = &prNloParam->rScanParam;

	if (prScanInfo->fgNloScanning == TRUE && prNloDone->ucSeqNum == prScanParam->ucSeqNum) {
		kalSchedScanResults(prAdapter->prGlueInfo);

		if (prNloParam->fgStopAfterIndication == TRUE) {
			prScanInfo->fgNloScanning = FALSE;
		}
	} else {
		DBGLOG(SCN, LOUD, ("Unexpected NLO-DONE event: SeqNum = %d, Current State = %d\n",
				   prNloDone->ucSeqNum, prScanInfo->eCurrentState));
	}

	return;
}


/*                                                                            */
/* 
                                                       
 
            
 
              
*/
/*                                                                            */
BOOLEAN
scnFsmSchedScanRequest(IN P_ADAPTER_T prAdapter,
		       IN UINT_8 ucSsidNum,
		       IN P_PARAM_SSID_T prSsid,
		       IN UINT_32 u4IeLength, IN PUINT_8 pucIe, IN UINT_16 u2Interval)
{
	P_SCAN_INFO_T prScanInfo;
	P_NLO_PARAM_T prNloParam;
	P_SCAN_PARAM_T prScanParam;
	P_CMD_NLO_REQ prCmdNloReq;
	UINT_32 i, j;

	ASSERT(prAdapter);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prNloParam = &prScanInfo->rNloParam;
	prScanParam = &prNloParam->rScanParam;

	ASSERT(prScanInfo->fgNloScanning == FALSE);

	/*                    */
	prScanParam->ucSeqNum++;
	prScanParam->ucBssIndex = prAdapter->prAisBssInfo->ucBssIndex;
	prNloParam->fgStopAfterIndication = TRUE;
	prNloParam->ucFastScanIteration = 0;
	prNloParam->u2FastScanPeriod = u2Interval;
	prNloParam->u2SlowScanPeriod = u2Interval;

	if (prScanParam->ucSSIDNum > CFG_SCAN_SSID_MAX_NUM) {
		prScanParam->ucSSIDNum = CFG_SCAN_SSID_MAX_NUM;
	} else {
		prScanParam->ucSSIDNum = ucSsidNum;
	}

	if (prNloParam->ucMatchSSIDNum > CFG_SCAN_SSID_MATCH_MAX_NUM) {
		prNloParam->ucMatchSSIDNum = CFG_SCAN_SSID_MATCH_MAX_NUM;
	} else {
		prNloParam->ucMatchSSIDNum = ucSsidNum;
	}

	for (i = 0; i < prNloParam->ucMatchSSIDNum; i++) {
		if (i < CFG_SCAN_SSID_MAX_NUM) {
			COPY_SSID(prScanParam->aucSpecifiedSSID[i],
				  prScanParam->ucSpecifiedSSIDLen[i],
				  prSsid[i].aucSsid, (UINT_8) prSsid[i].u4SsidLen);
		}

		COPY_SSID(prNloParam->aucMatchSSID[i],
			  prNloParam->ucMatchSSIDLen[i],
			  prSsid[i].aucSsid, (UINT_8) prSsid[i].u4SsidLen);

		prNloParam->aucCipherAlgo[i] = 0;
		prNloParam->au2AuthAlgo[i] = 0;

		for (j = 0; j < SCN_NLO_NETWORK_CHANNEL_NUM; j++) {
			prNloParam->aucChannelHint[i][j] = 0;
		}
	}

	/*                                */
	prCmdNloReq = (P_CMD_NLO_REQ) cnmMemAlloc(prAdapter,
						  RAM_TYPE_BUF,
						  sizeof(CMD_NLO_REQ) + prScanParam->u2IELen);

	if (!prCmdNloReq) {
		ASSERT(0);	/*                              */
		return FALSE;
	}

	/*                                          */
	kalMemZero(prCmdNloReq, sizeof(CMD_NLO_REQ));

	prCmdNloReq->ucSeqNum = prScanParam->ucSeqNum;
	prCmdNloReq->ucBssIndex = prScanParam->ucBssIndex;
	prCmdNloReq->fgStopAfterIndication = prNloParam->fgStopAfterIndication;
	prCmdNloReq->ucFastScanIteration = prNloParam->ucFastScanIteration;
	prCmdNloReq->u2FastScanPeriod = prNloParam->u2FastScanPeriod;
	prCmdNloReq->u2SlowScanPeriod = prNloParam->u2SlowScanPeriod;
	prCmdNloReq->ucEntryNum = prNloParam->ucMatchSSIDNum;
	for (i = 0; i < prNloParam->ucMatchSSIDNum; i++) {
		COPY_SSID(prCmdNloReq->arNetworkList[i].aucSSID,
			  prCmdNloReq->arNetworkList[i].ucSSIDLength,
			  prNloParam->aucMatchSSID[i], prNloParam->ucMatchSSIDLen[i]);

		prCmdNloReq->arNetworkList[i].ucCipherAlgo = prNloParam->aucCipherAlgo[i];
		prCmdNloReq->arNetworkList[i].u2AuthAlgo = prNloParam->au2AuthAlgo[i];

		for (j = 0; j < SCN_NLO_NETWORK_CHANNEL_NUM; j++) {
			prCmdNloReq->arNetworkList[i].ucNumChannelHint[j] =
			    prNloParam->aucChannelHint[i][j];
		}
	}

	if (prScanParam->u2IELen <= MAX_IE_LENGTH) {
		prCmdNloReq->u2IELen = prScanParam->u2IELen;
	} else {
		prCmdNloReq->u2IELen = MAX_IE_LENGTH;
	}

	if (prScanParam->u2IELen) {
		kalMemCopy(prCmdNloReq->aucIE,
			   prScanParam->aucIE, sizeof(UINT_8) * prCmdNloReq->u2IELen);
	}

	wlanSendSetQueryCmd(prAdapter,
			    CMD_ID_SET_NLO_REQ,
			    TRUE,
			    FALSE,
			    TRUE,
			    nicCmdEventSetCommon,
			    nicOidCmdTimeoutCommon,
			    sizeof(CMD_NLO_REQ) + prCmdNloReq->u2IELen,
			    (PUINT_8) prCmdNloReq, NULL, 0);

	cnmMemFree(prAdapter, (PVOID) prCmdNloReq);

	return TRUE;
}


/*                                                                            */
/* 
                                                       
 
            
 
              
*/
/*                                                                            */
BOOLEAN scnFsmSchedScanStopRequest(IN P_ADAPTER_T prAdapter)
{
	P_SCAN_INFO_T prScanInfo;
	P_NLO_PARAM_T prNloParam;
	P_SCAN_PARAM_T prScanParam;
	CMD_NLO_CANCEL rCmdNloCancel;

	ASSERT(prAdapter);

	prScanInfo = &(prAdapter->rWifiVar.rScanInfo);
	prNloParam = &prScanInfo->rNloParam;
	prScanParam = &prNloParam->rScanParam;

	if (prScanInfo->fgNloScanning == TRUE) {
		/*                                        */
		rCmdNloCancel.ucSeqNum = prScanParam->ucSeqNum;

		wlanSendSetQueryCmd(prAdapter,
				    CMD_ID_SET_NLO_CANCEL,
				    TRUE,
				    FALSE,
				    TRUE,
				    nicCmdEventSetStopSchedScan,
				    nicOidCmdTimeoutCommon,
				    sizeof(CMD_NLO_CANCEL), (PUINT_8) &rCmdNloCancel, NULL, 0);

		prScanInfo->fgNloScanning = FALSE;

		return TRUE;
	} else {
		return FALSE;
	}
}
