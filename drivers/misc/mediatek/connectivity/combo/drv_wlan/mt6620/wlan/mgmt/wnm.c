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

#if CFG_SUPPORT_802_11V

/*                                                                              
                                                
                                                                                
*/

#define WNM_MAX_TOD_ERROR  0
#define WNM_MAX_TOA_ERROR  0
#define MICRO_TO_10NANO(x) ((x)*100)
/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

static UINT_8 ucTimingMeasToken;

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

WLAN_STATUS
wnmRunEventTimgingMeasTxDone(IN P_ADAPTER_T prAdapter,
			     IN P_MSDU_INFO_T prMsduInfo, IN ENUM_TX_RESULT_CODE_T rTxDoneStatus);

VOID
wnmComposeTimingMeasFrame(IN P_ADAPTER_T prAdapter,
			  IN P_STA_RECORD_T prStaRec, IN PFN_TX_DONE_HANDLER pfTxDoneHandler);

VOID wnmTimingMeasRequest(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);
/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
 
                                                                                 
 
 
       
                                        
*/
/*                                                                            */
VOID wnmWNMAction(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_WLAN_ACTION_FRAME prRxFrame;

	ASSERT(prAdapter);
	ASSERT(prSwRfb);

	prRxFrame = (P_WLAN_ACTION_FRAME) prSwRfb->pvHeader;

#if CFG_SUPPORT_802_11V_TIMING_MEASUREMENT
	if (prRxFrame->ucAction == ACTION_WNM_TIMING_MEASUREMENT_REQUEST) {
		wnmTimingMeasRequest(prAdapter, prSwRfb);
		return;
	}
#endif

	DBGLOG(WNM, TRACE, ("Unsupport WNM action frame: %d\n", prRxFrame->ucAction));
}

#if CFG_SUPPORT_802_11V_TIMING_MEASUREMENT
/*                                                                            */
/* 
 
                                                                  
 
*/
/*                                                                            */
VOID
wnmReportTimingMeas(IN P_ADAPTER_T prAdapter,
		    IN UINT_8 ucStaRecIndex, IN UINT_32 u4ToD, IN UINT_32 u4ToA)
{
	P_STA_RECORD_T prStaRec;

	prStaRec = cnmGetStaRecByIndex(prAdapter, ucStaRecIndex);

	if ((!prStaRec) || (!prStaRec->fgIsInUse)) {
		return;
	}

	DBGLOG(WNM, TRACE, ("wnmReportTimingMeas: u4ToD %x u4ToA %x", u4ToD, u4ToA));

	if (!prStaRec->rWNMTimingMsmt.ucTrigger)
		return;

	prStaRec->rWNMTimingMsmt.u4ToD = MICRO_TO_10NANO(u4ToD);
	prStaRec->rWNMTimingMsmt.u4ToA = MICRO_TO_10NANO(u4ToA);
}

/*                                                                            */
/* 
                                                                   
 
                                                             
                                                       
                                                                             
 
                             
*/
/*                                                                            */
WLAN_STATUS
wnmRunEventTimgingMeasTxDone(IN P_ADAPTER_T prAdapter,
			     IN P_MSDU_INFO_T prMsduInfo, IN ENUM_TX_RESULT_CODE_T rTxDoneStatus)
{
	P_STA_RECORD_T prStaRec;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);

	DBGLOG(WNM, LOUD, ("EVENT-TX DONE: Current Time = %ld\n", kalGetTimeTick()));

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	if ((!prStaRec) || (!prStaRec->fgIsInUse)) {
		return WLAN_STATUS_SUCCESS;	/*                                            */
	}

	DBGLOG(WNM, TRACE,
	       ("wnmRunEventTimgingMeasTxDone: ucDialog %d ucFollowUp %d u4ToD %x u4ToA %x",
		prStaRec->rWNMTimingMsmt.ucDialogToken,
		prStaRec->rWNMTimingMsmt.ucFollowUpDialogToken, prStaRec->rWNMTimingMsmt.u4ToD,
		prStaRec->rWNMTimingMsmt.u4ToA));

	prStaRec->rWNMTimingMsmt.ucFollowUpDialogToken = prStaRec->rWNMTimingMsmt.ucDialogToken;
	prStaRec->rWNMTimingMsmt.ucDialogToken = ++ucTimingMeasToken;

	wnmComposeTimingMeasFrame(prAdapter, prStaRec, NULL);

	return WLAN_STATUS_SUCCESS;

}				/*                                       */

/*                                                                            */
/* 
                                                                 
 
                                                                     
                                                                
 
                
*/
/*                                                                            */
VOID
wnmComposeTimingMeasFrame(IN P_ADAPTER_T prAdapter,
			  IN P_STA_RECORD_T prStaRec, IN PFN_TX_DONE_HANDLER pfTxDoneHandler)
{
	P_MSDU_INFO_T prMsduInfo;
	P_BSS_INFO_T prBssInfo;
	P_ACTION_UNPROTECTED_WNM_TIMING_MEAS_FRAME prTxFrame;
	UINT_16 u2PayloadLen;

	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex];
	ASSERT(prBssInfo);

	prMsduInfo = (P_MSDU_INFO_T) cnmMgtPktAlloc(prAdapter,
						    MAC_TX_RESERVED_FIELD + PUBLIC_ACTION_MAX_LEN);

	if (!prMsduInfo)
		return;

	prTxFrame = (P_ACTION_UNPROTECTED_WNM_TIMING_MEAS_FRAME)
	    ((UINT_32) (prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD);

	prTxFrame->u2FrameCtrl = MAC_FRAME_ACTION;

	COPY_MAC_ADDR(prTxFrame->aucDestAddr, prStaRec->aucMacAddr);
	COPY_MAC_ADDR(prTxFrame->aucSrcAddr, prBssInfo->aucOwnMacAddr);
	COPY_MAC_ADDR(prTxFrame->aucBSSID, prBssInfo->aucBSSID);

	prTxFrame->ucCategory = CATEGORY_UNPROTECTED_WNM_ACTION;
	prTxFrame->ucAction = ACTION_UNPROTECTED_WNM_TIMING_MEASUREMENT;

	/*                                   */
	prTxFrame->ucDialogToken = prStaRec->rWNMTimingMsmt.ucDialogToken;
	prTxFrame->ucFollowUpDialogToken = prStaRec->rWNMTimingMsmt.ucFollowUpDialogToken;
	prTxFrame->u4ToD = prStaRec->rWNMTimingMsmt.u4ToD;
	prTxFrame->u4ToA = prStaRec->rWNMTimingMsmt.u4ToA;
	prTxFrame->ucMaxToDErr = WNM_MAX_TOD_ERROR;
	prTxFrame->ucMaxToAErr = WNM_MAX_TOA_ERROR;

	u2PayloadLen = 2 + ACTION_UNPROTECTED_WNM_TIMING_MEAS_LEN;

	/*                                     */
	prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;	/*                  */
	prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
	prMsduInfo->ucNetworkType = prStaRec->ucNetTypeIndex;
	prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
	prMsduInfo->fgIs802_1x = FALSE;
	prMsduInfo->fgIs802_11 = TRUE;
	prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen;
	prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
	prMsduInfo->pfTxDoneHandler = pfTxDoneHandler;
	prMsduInfo->fgIsBasicRate = FALSE;

	DBGLOG(WNM, TRACE,
	       ("wnmComposeTimingMeasFrame: ucDialogToken %d ucFollowUpDialogToken %d u4ToD %x u4ToA %x\n",
		prTxFrame->ucDialogToken, prTxFrame->ucFollowUpDialogToken, prTxFrame->u4ToD,
		prTxFrame->u4ToA));

	/*                                                */
	nicTxEnqueueMsdu(prAdapter, prMsduInfo);

	return;

}				/*                                    */

/*                                                                            */
/* 
 
                                                                                  
 
 
       
                             
*/
/*                                                                            */
VOID wnmTimingMeasRequest(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_ACTION_WNM_TIMING_MEAS_REQ_FRAME prRxFrame = NULL;
	P_STA_RECORD_T prStaRec;

	prRxFrame = (P_ACTION_WNM_TIMING_MEAS_REQ_FRAME) prSwRfb->pvHeader;
	if (!prRxFrame)
		return;

	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
	if ((!prStaRec) || (!prStaRec->fgIsInUse)) {
		return;
	}

	DBGLOG(WNM, TRACE, ("IEEE 802.11: Received Timing Measuremen Request from "
			    MACSTR "\n", MAC2STR(prStaRec->aucMacAddr)));

	/*                   */
	prStaRec->rWNMTimingMsmt.fgInitiator = TRUE;
	prStaRec->rWNMTimingMsmt.ucTrigger = prRxFrame->ucTrigger;
	if (!prRxFrame->ucTrigger)
		return;

	prStaRec->rWNMTimingMsmt.ucDialogToken = ++ucTimingMeasToken;
	prStaRec->rWNMTimingMsmt.ucFollowUpDialogToken = 0;

	wnmComposeTimingMeasFrame(prAdapter, prStaRec, wnmRunEventTimgingMeasTxDone);
}

#if WNM_UNIT_TEST
VOID wnmTimingMeasUnitTest1(P_ADAPTER_T prAdapter, UINT_8 ucStaRecIndex)
{
	P_STA_RECORD_T prStaRec;

	prStaRec = cnmGetStaRecByIndex(prAdapter, ucStaRecIndex);
	if ((!prStaRec) || (!prStaRec->fgIsInUse)) {
		return;
	}

	DBGLOG(WNM, INFO, ("IEEE 802.11v: Test Timing Measuremen Request from "
			   MACSTR "\n", MAC2STR(prStaRec->aucMacAddr)));

	prStaRec->rWNMTimingMsmt.fgInitiator = TRUE;
	prStaRec->rWNMTimingMsmt.ucTrigger = 1;

	prStaRec->rWNMTimingMsmt.ucDialogToken = ++ucTimingMeasToken;
	prStaRec->rWNMTimingMsmt.ucFollowUpDialogToken = 0;

	wnmComposeTimingMeasFrame(prAdapter, prStaRec, wnmRunEventTimgingMeasTxDone);
}
#endif

#endif				/*                                        */

#endif				/*                     */
