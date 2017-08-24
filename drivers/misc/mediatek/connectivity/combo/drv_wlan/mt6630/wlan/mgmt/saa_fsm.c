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
static PUINT_8 apucDebugAAState[AA_STATE_NUM] = {
	(PUINT_8) DISP_STRING("AA_STATE_IDLE"),
	(PUINT_8) DISP_STRING("SAA_STATE_SEND_AUTH1"),
	(PUINT_8) DISP_STRING("SAA_STATE_WAIT_AUTH2"),
	(PUINT_8) DISP_STRING("SAA_STATE_SEND_AUTH3"),
	(PUINT_8) DISP_STRING("SAA_STATE_WAIT_AUTH4"),
	(PUINT_8) DISP_STRING("SAA_STATE_SEND_ASSOC1"),
	(PUINT_8) DISP_STRING("SAA_STATE_WAIT_ASSOC2"),
	(PUINT_8) DISP_STRING("AAA_STATE_SEND_AUTH2"),
	(PUINT_8) DISP_STRING("AAA_STATE_SEND_AUTH4"),
	(PUINT_8) DISP_STRING("AAA_STATE_SEND_ASSOC2"),
	(PUINT_8) DISP_STRING("AA_STATE_RESOURCE")
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
VOID
saaFsmSteps(IN P_ADAPTER_T prAdapter,
	    IN P_STA_RECORD_T prStaRec,
	    IN ENUM_AA_STATE_T eNextState, IN P_SW_RFB_T prRetainedSwRfb)
{
	ENUM_AA_STATE_T ePreviousState;
	BOOLEAN fgIsTransition;


	ASSERT(prStaRec);
	if (!prStaRec) {
		return;
	}

	do {

#if DBG
		DBGLOG(SAA, STATE, ("TRANSITION: [%s] -> [%s]\n",
				    apucDebugAAState[prStaRec->eAuthAssocState],
				    apucDebugAAState[eNextState]));
#else
		DBGLOG(SAA, STATE, ("[%d] TRANSITION: [%d] -> [%d]\n",
				    DBG_SAA_IDX, prStaRec->eAuthAssocState, eNextState));
#endif
		ePreviousState = prStaRec->eAuthAssocState;

		/*                                                                                   */
		prStaRec->eAuthAssocState = eNextState;


		fgIsTransition = (BOOLEAN) FALSE;
		switch (prStaRec->eAuthAssocState) {
		case AA_STATE_IDLE:

			if (ePreviousState != prStaRec->eAuthAssocState) {	/*                              */

				if (prRetainedSwRfb) {

					if (saaFsmSendEventJoinComplete(prAdapter,
									WLAN_STATUS_SUCCESS,
									prStaRec,
									prRetainedSwRfb) ==
						WLAN_STATUS_SUCCESS) {
					} else {
						eNextState = AA_STATE_RESOURCE;
						fgIsTransition = TRUE;
					}
				} else {
					if (saaFsmSendEventJoinComplete(prAdapter,
									WLAN_STATUS_FAILURE,
									prStaRec,
									NULL) ==
					    WLAN_STATUS_RESOURCES) {
						eNextState = AA_STATE_RESOURCE;
						fgIsTransition = TRUE;
					}
				}

			}

			/*                           */
			if (prStaRec->prChallengeText) {
				cnmMemFree(prAdapter, prStaRec->prChallengeText);
				prStaRec->prChallengeText = (P_IE_CHALLENGE_TEXT_T) NULL;
			}

			break;

		case SAA_STATE_SEND_AUTH1:

				/*                        */
			if (prStaRec->ucTxAuthAssocRetryCount >=
			    prStaRec->ucTxAuthAssocRetryLimit) {

				/*                                                  */
				prStaRec->u2StatusCode = STATUS_CODE_AUTH_TIMEOUT;

				eNextState = AA_STATE_IDLE;
				fgIsTransition = TRUE;
			} else {
				prStaRec->ucTxAuthAssocRetryCount++;

				/*                                      */
				cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);

#if !CFG_SUPPORT_AAA
				if (authSendAuthFrame(prAdapter,
						      prStaRec,
						      AUTH_TRANSACTION_SEQ_1) !=
					    WLAN_STATUS_SUCCESS)
#else
				if (authSendAuthFrame(prAdapter,
						      prStaRec,
						      prStaRec->ucBssIndex,
						      NULL,
						      AUTH_TRANSACTION_SEQ_1,
						      STATUS_CODE_RESERVED) !=
				    WLAN_STATUS_SUCCESS)
#endif				/*                 */
				{

					cnmTimerInitTimer(prAdapter,
							  &prStaRec->
							  rTxReqDoneOrRxRespTimer,
							  (PFN_MGMT_TIMEOUT_FUNC)
							  saaFsmRunEventTxReqTimeOut,
							  (ULONG) prStaRec);

					cnmTimerStartTimer(prAdapter,
							   &prStaRec->
							   rTxReqDoneOrRxRespTimer,
							   TU_TO_MSEC
							   (TX_AUTHENTICATION_RETRY_TIMEOUT_TU));
				}
			}

			break;

		case SAA_STATE_WAIT_AUTH2:
			break;

		case SAA_STATE_SEND_AUTH3:

			/*                        */
			if (prStaRec->ucTxAuthAssocRetryCount >=
			    prStaRec->ucTxAuthAssocRetryLimit) {

				/*                                                  */
				prStaRec->u2StatusCode = STATUS_CODE_AUTH_TIMEOUT;

				eNextState = AA_STATE_IDLE;
				fgIsTransition = TRUE;
			} else {
				prStaRec->ucTxAuthAssocRetryCount++;

#if !CFG_SUPPORT_AAA
				if (authSendAuthFrame(prAdapter,
						      prStaRec,
						      AUTH_TRANSACTION_SEQ_3) !=
				    WLAN_STATUS_SUCCESS)
#else
				if (authSendAuthFrame(prAdapter,
						      prStaRec,
						      prStaRec->ucBssIndex,
						      NULL,
						      AUTH_TRANSACTION_SEQ_3,
						      STATUS_CODE_RESERVED) !=
				    WLAN_STATUS_SUCCESS)
#endif				/*                 */
				{

					cnmTimerInitTimer(prAdapter,
							  &prStaRec->
							  rTxReqDoneOrRxRespTimer,
							  (PFN_MGMT_TIMEOUT_FUNC)
							  saaFsmRunEventTxReqTimeOut,
							  (ULONG) prStaRec);

					cnmTimerStartTimer(prAdapter,
							   &prStaRec->
							   rTxReqDoneOrRxRespTimer,
							   TU_TO_MSEC
							   (TX_AUTHENTICATION_RETRY_TIMEOUT_TU));
				}
			}

			break;

		case SAA_STATE_WAIT_AUTH4:
			break;

		case SAA_STATE_SEND_ASSOC1:
			/*                        */
			if (prStaRec->ucTxAuthAssocRetryCount >= prStaRec->ucTxAuthAssocRetryLimit) {

				/*                                                  */
				prStaRec->u2StatusCode = STATUS_CODE_ASSOC_TIMEOUT;

				eNextState = AA_STATE_IDLE;
				fgIsTransition = TRUE;
			} else {
				prStaRec->ucTxAuthAssocRetryCount++;

				if (assocSendReAssocReqFrame(prAdapter, prStaRec) !=
				    WLAN_STATUS_SUCCESS) {

					cnmTimerInitTimer(prAdapter,
							  &prStaRec->rTxReqDoneOrRxRespTimer,
							  (PFN_MGMT_TIMEOUT_FUNC)
							  saaFsmRunEventTxReqTimeOut,
							  (ULONG) prStaRec);

					cnmTimerStartTimer(prAdapter,
							   &prStaRec->rTxReqDoneOrRxRespTimer,
							   TU_TO_MSEC
							   (TX_ASSOCIATION_RETRY_TIMEOUT_TU));
				}
			}

			break;

		case SAA_STATE_WAIT_ASSOC2:
			break;

		case AA_STATE_RESOURCE:
			/*                                                      */
			break;

		default:
			DBGLOG(SAA, ERROR, ("Unknown AA STATE\n"));
			ASSERT(0);
			break;
		}

	} while (fgIsTransition);

	return;

}				/*                      */


/*                                                                            */
/* 
                                                     
 
                                                                    
                                                           
                                                       

                
*/
/*                                                                            */
WLAN_STATUS
saaFsmSendEventJoinComplete(IN P_ADAPTER_T prAdapter,
			    IN WLAN_STATUS rJoinStatus,
			    IN P_STA_RECORD_T prStaRec, IN P_SW_RFB_T prSwRfb)
{
	P_BSS_INFO_T prBssInfo;

	ASSERT(prStaRec);
	if (!prStaRec) {
		DBGLOG(SAA, ERROR, ("[%s]prStaRec is NULL\n", __func__));
		return WLAN_STATUS_INVALID_PACKET;
	}
	if (!prAdapter) {
		DBGLOG(SAA, ERROR, ("[%s]prAdapter is NULL\n", __func__));
		return WLAN_STATUS_INVALID_PACKET;
	}
	if (!prAdapter->prAisBssInfo) {
		DBGLOG(SAA, ERROR, ("[%s]prAdapter->prAisBssInfo is NULL\n", __func__));
		return WLAN_STATUS_INVALID_PACKET;
	}

	/*                                                                      */
	if (prStaRec->ucBssIndex < BSS_INFO_NUM) {
		prBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, prStaRec->ucBssIndex);

		if (rJoinStatus == WLAN_STATUS_SUCCESS) {
			prBssInfo->fg40mBwAllowed = prBssInfo->fgAssoc40mBwAllowed;
		}
		prBssInfo->fgAssoc40mBwAllowed = FALSE;
	}

	if (prStaRec->ucBssIndex == prAdapter->prAisBssInfo->ucBssIndex) {
		P_MSG_SAA_FSM_COMP_T prSaaFsmCompMsg;

		prSaaFsmCompMsg = cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_SAA_FSM_COMP_T));
		if (!prSaaFsmCompMsg) {
			return WLAN_STATUS_RESOURCES;
		}

		prSaaFsmCompMsg->rMsgHdr.eMsgId = MID_SAA_AIS_JOIN_COMPLETE;
		prSaaFsmCompMsg->ucSeqNum = prStaRec->ucAuthAssocReqSeqNum;
		prSaaFsmCompMsg->rJoinStatus = rJoinStatus;
		prSaaFsmCompMsg->prStaRec = prStaRec;
		prSaaFsmCompMsg->prSwRfb = prSwRfb;

		/*                                                         */
		mboxSendMsg(prAdapter,
			    MBOX_ID_0, (P_MSG_HDR_T) prSaaFsmCompMsg, MSG_SEND_METHOD_UNBUF);

		return WLAN_STATUS_SUCCESS;
	}
#if CFG_ENABLE_WIFI_DIRECT
	else if ((prAdapter->fgIsP2PRegistered) && (IS_STA_IN_P2P(prStaRec))) {
		P_MSG_SAA_FSM_COMP_T prSaaFsmCompMsg;

		prSaaFsmCompMsg = cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_SAA_FSM_COMP_T));
		if (!prSaaFsmCompMsg) {
			return WLAN_STATUS_RESOURCES;
		}

		prSaaFsmCompMsg->rMsgHdr.eMsgId = MID_SAA_P2P_JOIN_COMPLETE;
		prSaaFsmCompMsg->ucSeqNum = prStaRec->ucAuthAssocReqSeqNum;
		prSaaFsmCompMsg->rJoinStatus = rJoinStatus;
		prSaaFsmCompMsg->prStaRec = prStaRec;
		prSaaFsmCompMsg->prSwRfb = prSwRfb;

		/*                                                         */
		mboxSendMsg(prAdapter,
			    MBOX_ID_0, (P_MSG_HDR_T) prSaaFsmCompMsg, MSG_SEND_METHOD_UNBUF);

		return WLAN_STATUS_SUCCESS;
	}
#endif
#if CFG_ENABLE_BT_OVER_WIFI
	else if (IS_STA_BOW_TYPE(prStaRec)) {
		/*                    */

		P_MSG_SAA_FSM_COMP_T prSaaFsmCompMsg;

		prSaaFsmCompMsg = cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_SAA_FSM_COMP_T));
		if (!prSaaFsmCompMsg) {
			return WLAN_STATUS_RESOURCES;
		}

		prSaaFsmCompMsg->rMsgHdr.eMsgId = MID_SAA_BOW_JOIN_COMPLETE;
		prSaaFsmCompMsg->ucSeqNum = prStaRec->ucAuthAssocReqSeqNum;
		prSaaFsmCompMsg->rJoinStatus = rJoinStatus;
		prSaaFsmCompMsg->prStaRec = prStaRec;
		prSaaFsmCompMsg->prSwRfb = prSwRfb;

		/*                                                         */
		mboxSendMsg(prAdapter,
			    MBOX_ID_0, (P_MSG_HDR_T) prSaaFsmCompMsg, MSG_SEND_METHOD_UNBUF);

		return WLAN_STATUS_SUCCESS;
	}
#endif
	else {
		ASSERT(0);
		return WLAN_STATUS_FAILURE;
	}

}				/*                                      */


/*                                                                            */
/* 
                                                              
 
                                                                     
 
                
*/
/*                                                                            */
VOID saaFsmRunEventStart(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr)
{
	P_MSG_SAA_FSM_START_T prSaaFsmStartMsg;
	P_STA_RECORD_T prStaRec;
	P_BSS_INFO_T prBssInfo;

	ASSERT(prAdapter);
	ASSERT(prMsgHdr);

	prSaaFsmStartMsg = (P_MSG_SAA_FSM_START_T) prMsgHdr;
	prStaRec = prSaaFsmStartMsg->prStaRec;

	if ((!prStaRec) || (prStaRec->fgIsInUse == FALSE)) {
		cnmMemFree(prAdapter, prMsgHdr);
		return;
	}

	ASSERT(prStaRec);

	DBGLOG(SAA, LOUD, ("EVENT-START: Trigger SAA FSM.\n"));

	/*                                           */
	prStaRec->ucAuthAssocReqSeqNum = prSaaFsmStartMsg->ucSeqNum;

	cnmMemFree(prAdapter, prMsgHdr);

	/*                                     */
	if (!IS_AP_STA(prStaRec)) {

		DBGLOG(SAA, ERROR,
		       ("EVENT-START: STA Type - %d was not supported.\n", prStaRec->eStaType));

		/*                                                        */
		saaFsmSendEventJoinComplete(prAdapter, WLAN_STATUS_FAILURE, prStaRec, NULL);

		return;
	}
	/*                                                    */
	if (prStaRec->eAuthAssocState != AA_STATE_IDLE) {
		DBGLOG(SAA, ERROR, ("EVENT-START: Reentry of SAA Module.\n"));
		prStaRec->eAuthAssocState = AA_STATE_IDLE;
	}
	/*                                  */
	/*                                            */
	prStaRec->u2StatusCode = STATUS_CODE_SUCCESSFUL;

	/*                              */
	GET_CURRENT_SYSTIME(&prStaRec->rLastJoinTime);

	prStaRec->ucTxAuthAssocRetryCount = 0;

	if (prStaRec->prChallengeText) {
		cnmMemFree(prAdapter, prStaRec->prChallengeText);
		prStaRec->prChallengeText = (P_IE_CHALLENGE_TEXT_T) NULL;
	}

	cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);

	/*                        */
	/*                                  */

	/*                           */
	/*                                      */
	/*                                                    
                                                                                
                         
  */
	/*                                              */

	/*                                                      */
	if (prStaRec->ucBssIndex < BSS_INFO_NUM) {
		prBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, prStaRec->ucBssIndex);

		if ((prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11N)
		    && (prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N)) {
			prBssInfo->fgAssoc40mBwAllowed =
			    cnmBss40mBwPermitted(prAdapter, prBssInfo->ucBssIndex);
		} else {
			prBssInfo->fgAssoc40mBwAllowed = FALSE;
		}
		DBGLOG(RLM, TRACE, ("STA 40mAllowed=%d\n", prBssInfo->fgAssoc40mBwAllowed));
	}
	/*                       */
	if (prStaRec->ucStaState == STA_STATE_1) {
		saaFsmSteps(prAdapter, prStaRec, SAA_STATE_SEND_AUTH1, (P_SW_RFB_T) NULL);
	} else if (prStaRec->ucStaState == STA_STATE_2 || prStaRec->ucStaState == STA_STATE_3) {
		saaFsmSteps(prAdapter, prStaRec, SAA_STATE_SEND_ASSOC1, (P_SW_RFB_T) NULL);
	}

	return;
}				/*                              */


/*                                                                            */
/* 
                                                                                 
 
                                                       
                                                                               
 
                             
*/
/*                                                                            */
WLAN_STATUS
saaFsmRunEventTxDone(IN P_ADAPTER_T prAdapter,
		     IN P_MSDU_INFO_T prMsduInfo, IN ENUM_TX_RESULT_CODE_T rTxDoneStatus)
{

	P_STA_RECORD_T prStaRec;
	ENUM_AA_STATE_T eNextState;


	ASSERT(prMsduInfo);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	if (!prStaRec) {
		return WLAN_STATUS_INVALID_PACKET;
	}

	ASSERT(prStaRec);

	DBGLOG(SAA, LOUD, ("EVENT-TX DONE: Current Time = %d\n", kalGetTimeTick()));

	eNextState = prStaRec->eAuthAssocState;

	switch (prStaRec->eAuthAssocState) {
	case SAA_STATE_SEND_AUTH1:
		{
			/*                                                                    */
			if (authCheckTxAuthFrame(prAdapter,
						 prMsduInfo,
						 AUTH_TRANSACTION_SEQ_1) != WLAN_STATUS_SUCCESS) {
				break;
			}

			if (rTxDoneStatus == TX_RESULT_SUCCESS) {
				eNextState = SAA_STATE_WAIT_AUTH2;

				cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);

				cnmTimerInitTimer(prAdapter,
						  &prStaRec->rTxReqDoneOrRxRespTimer,
						  (PFN_MGMT_TIMEOUT_FUNC)
						  saaFsmRunEventRxRespTimeOut, (ULONG) prStaRec);

				cnmTimerStartTimer(prAdapter,
						   &prStaRec->rTxReqDoneOrRxRespTimer,
						   TU_TO_MSEC
						   (DOT11_AUTHENTICATION_RESPONSE_TIMEOUT_TU));
			}

			/*                                            
                                             
    */
			saaFsmSteps(prAdapter, prStaRec, eNextState, (P_SW_RFB_T) NULL);
		}
		break;

	case SAA_STATE_SEND_AUTH3:
		{
			/*                                                                      */
			if (authCheckTxAuthFrame(prAdapter,
						 prMsduInfo,
						 AUTH_TRANSACTION_SEQ_3) != WLAN_STATUS_SUCCESS) {
				break;
			}

			if (rTxDoneStatus == TX_RESULT_SUCCESS) {
				eNextState = SAA_STATE_WAIT_AUTH4;

				cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);

				cnmTimerInitTimer(prAdapter,
						  &prStaRec->rTxReqDoneOrRxRespTimer,
						  (PFN_MGMT_TIMEOUT_FUNC)
						  saaFsmRunEventRxRespTimeOut, (ULONG) prStaRec);

				cnmTimerStartTimer(prAdapter,
						   &prStaRec->rTxReqDoneOrRxRespTimer,
						   TU_TO_MSEC
						   (DOT11_AUTHENTICATION_RESPONSE_TIMEOUT_TU));
			}

			/*                                            
                                             
    */
			saaFsmSteps(prAdapter, prStaRec, eNextState, (P_SW_RFB_T) NULL);
		}
		break;

	case SAA_STATE_SEND_ASSOC1:
		{
			/*                                                                     */
			if (assocCheckTxReAssocReqFrame(prAdapter, prMsduInfo) !=
			    WLAN_STATUS_SUCCESS) {
				break;
			}

			if (rTxDoneStatus == TX_RESULT_SUCCESS) {
				eNextState = SAA_STATE_WAIT_ASSOC2;

				cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);

				cnmTimerInitTimer(prAdapter,
						  &prStaRec->rTxReqDoneOrRxRespTimer,
						  (PFN_MGMT_TIMEOUT_FUNC)
						  saaFsmRunEventRxRespTimeOut, (ULONG) prStaRec);

				cnmTimerStartTimer(prAdapter,
						   &(prStaRec->rTxReqDoneOrRxRespTimer),
						   TU_TO_MSEC
						   (DOT11_ASSOCIATION_RESPONSE_TIMEOUT_TU));
			}

			/*                                            
                                             
    */
			saaFsmSteps(prAdapter, prStaRec, eNextState, (P_SW_RFB_T) NULL);
		}
		break;

	default:
		break;		/*                    */
	}


	return WLAN_STATUS_SUCCESS;

}				/*                               */


/*                                                                            */
/* 
                                                                     
 
                                                           
 
                
*/
/*                                                                            */
VOID saaFsmRunEventTxReqTimeOut(IN P_ADAPTER_T prAdapter, IN ULONG plParamPtr)
{
	P_STA_RECORD_T	prStaRec = (P_STA_RECORD_T) plParamPtr;

	ASSERT(prStaRec);
	if (!prStaRec) {
		return;
	}

	DBGLOG(SAA, LOUD, ("EVENT-TIMER: TX REQ TIMEOUT, Current Time = %d\n", kalGetTimeTick()));

	switch (prStaRec->eAuthAssocState) {
	case SAA_STATE_SEND_AUTH1:
	case SAA_STATE_SEND_AUTH3:
	case SAA_STATE_SEND_ASSOC1:
		saaFsmSteps(prAdapter, prStaRec, prStaRec->eAuthAssocState, (P_SW_RFB_T) NULL);
		break;

	default:
		return;
	}

	return;
}				/*                                     */


/*                                                                            */
/* 
                                                                      
 
                                                           
 
                
*/
/*                                                                            */
VOID saaFsmRunEventRxRespTimeOut(IN P_ADAPTER_T prAdapter, IN ULONG ulParamPtr)
{
	P_STA_RECORD_T prStaRec = (P_STA_RECORD_T) ulParamPtr;
	ENUM_AA_STATE_T eNextState;


	DBGLOG(SAA, LOUD, ("EVENT-TIMER: RX RESP TIMEOUT, Current Time = %d\n", kalGetTimeTick()));

	ASSERT(prStaRec);
	if (!prStaRec) {
		return;
	}

	eNextState = prStaRec->eAuthAssocState;

	switch (prStaRec->eAuthAssocState) {
	case SAA_STATE_WAIT_AUTH2:
		/*                                                  */
		prStaRec->u2StatusCode = STATUS_CODE_AUTH_TIMEOUT;

		/*                                        */
		eNextState = SAA_STATE_SEND_AUTH1;
		break;

	case SAA_STATE_WAIT_AUTH4:
		/*                                                  */
		prStaRec->u2StatusCode = STATUS_CODE_AUTH_TIMEOUT;

		/*                                        */
		eNextState = SAA_STATE_SEND_AUTH3;
		break;

	case SAA_STATE_WAIT_ASSOC2:
		/*                                                  */
		prStaRec->u2StatusCode = STATUS_CODE_ASSOC_TIMEOUT;

		/*                                        */
		eNextState = SAA_STATE_SEND_ASSOC1;
		break;

	default:
		break;		/*                    */
	}


	if (eNextState != prStaRec->eAuthAssocState) {
		saaFsmSteps(prAdapter, prStaRec, eNextState, (P_SW_RFB_T) NULL);
	}

	return;
}				/*                                      */


/*                                                                            */
/* 
                                                                       
                         
 
                                                                  
 
                
*/
/*                                                                            */
VOID saaFsmRunEventRxAuth(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_STA_RECORD_T prStaRec;
	UINT_16 u2StatusCode;
	ENUM_AA_STATE_T eNextState;


	ASSERT(prSwRfb);
	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);

	/*                                              */
	if (!prStaRec) {
		return;
	}

	if (!IS_AP_STA(prStaRec)) {
		return;
	}

	switch (prStaRec->eAuthAssocState) {
	case SAA_STATE_SEND_AUTH1:
	case SAA_STATE_WAIT_AUTH2:
		/*                                                        */
		if (authCheckRxAuthFrameStatus(prAdapter,
					       prSwRfb,
					       AUTH_TRANSACTION_SEQ_2,
					       &u2StatusCode) == WLAN_STATUS_SUCCESS) {

			cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);

			/*                                                  */
			prStaRec->u2StatusCode = u2StatusCode;

			if (u2StatusCode == STATUS_CODE_SUCCESSFUL) {

				authProcessRxAuth2_Auth4Frame(prAdapter, prSwRfb);

				if (prStaRec->ucAuthAlgNum ==
				    (UINT_8) AUTH_ALGORITHM_NUM_SHARED_KEY) {

					eNextState = SAA_STATE_SEND_AUTH3;
				} else {
					/*                                      */
					cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_2);

					eNextState = SAA_STATE_SEND_ASSOC1;
				}
			} else {
				DBGLOG(SAA, INFO,
				       ("Auth Req was rejected by [" MACSTR "], Status Code = %d\n",
					MAC2STR(prStaRec->aucMacAddr), u2StatusCode));

				eNextState = AA_STATE_IDLE;
			}

			/*                                       */
			prStaRec->ucTxAuthAssocRetryCount = 0;

			saaFsmSteps(prAdapter, prStaRec, eNextState, (P_SW_RFB_T) NULL);
		}
		break;

	case SAA_STATE_SEND_AUTH3:
	case SAA_STATE_WAIT_AUTH4:
		/*                                                        */
		if (authCheckRxAuthFrameStatus(prAdapter,
					       prSwRfb,
					       AUTH_TRANSACTION_SEQ_4,
					       &u2StatusCode) == WLAN_STATUS_SUCCESS) {

			cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);

			/*                                                  */
			prStaRec->u2StatusCode = u2StatusCode;

			if (u2StatusCode == STATUS_CODE_SUCCESSFUL) {

				authProcessRxAuth2_Auth4Frame(prAdapter, prSwRfb);	/*                          */

				/*                                      */
				cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_2);

				eNextState = SAA_STATE_SEND_ASSOC1;
			} else {
				DBGLOG(SAA, INFO,
				       ("Auth Req was rejected by [" MACSTR "], Status Code = %d\n",
					MAC2STR(prStaRec->aucMacAddr), u2StatusCode));

				eNextState = AA_STATE_IDLE;
			}

			/*                                       */
			prStaRec->ucTxAuthAssocRetryCount = 0;

			saaFsmSteps(prAdapter, prStaRec, eNextState, (P_SW_RFB_T) NULL);
		}
		break;

	default:
		break;		/*                    */
	}

	return;
}				/*                               */


/*                                                                            */
/* 
                                                                                  
                         
 
                                                                  
 
                                                                          
                                                                      
*/
/*                                                                            */
WLAN_STATUS saaFsmRunEventRxAssoc(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_STA_RECORD_T prStaRec;
	UINT_16 u2StatusCode;
	ENUM_AA_STATE_T eNextState;
	P_SW_RFB_T prRetainedSwRfb = (P_SW_RFB_T) NULL;
	WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;


	ASSERT(prSwRfb);
	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);

	/*                                              */
	if (!prStaRec) {
		/*            */
		return rStatus;
	}

	if (!IS_AP_STA(prStaRec)) {
		return rStatus;
	}

	switch (prStaRec->eAuthAssocState) {
	case SAA_STATE_SEND_ASSOC1:
	case SAA_STATE_WAIT_ASSOC2:
		/*                                                       */
		if (assocCheckRxReAssocRspFrameStatus(prAdapter,
						      prSwRfb,
						      &u2StatusCode) == WLAN_STATUS_SUCCESS) {

			cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);


			/*                                                  */
			prStaRec->u2StatusCode = u2StatusCode;

			if (u2StatusCode == STATUS_CODE_SUCCESSFUL) {

				/*                                      */
				/*                                                  
                                                                  
                                  
     */
				/*                                              */

				prStaRec->ucJoinFailureCount = 0;	/*                */

				prRetainedSwRfb = prSwRfb;
				rStatus = WLAN_STATUS_PENDING;
			} else {
				DBGLOG(SAA, INFO,
				       ("Assoc Req was rejected by [" MACSTR
					"], Status Code = %d\n", MAC2STR(prStaRec->aucMacAddr),
					u2StatusCode));
			}

			/*                                       */
			prStaRec->ucTxAuthAssocRetryCount = 0;

			/*             */
			ASSERT(prSwRfb->prRxStatusGroup3);
			prStaRec->ucRCPI =
			    (UINT_8) HAL_RX_STATUS_GET_RCPI(prSwRfb->prRxStatusGroup3);


			eNextState = AA_STATE_IDLE;

			saaFsmSteps(prAdapter, prStaRec, eNextState, prRetainedSwRfb);
		}
		break;

	default:
		break;		/*                    */
	}

	return rStatus;

}				/*                                */


/*                                                                            */
/* 
                                                            
 
                                                                  
 
                                                                         
*/
/*                                                                            */
WLAN_STATUS saaFsmRunEventRxDeauth(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_STA_RECORD_T prStaRec;
	P_WLAN_DEAUTH_FRAME_T prDeauthFrame;


	ASSERT(prSwRfb);
	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
	prDeauthFrame = (P_WLAN_DEAUTH_FRAME_T) prSwRfb->pvHeader;

	DBGLOG(SAA, INFO,
	       ("Rx Deauth frame from BSSID[" MACSTR "] DA[" MACSTR "] ReasonCode[0x%x]\n",
		MAC2STR(prDeauthFrame->aucBSSID), MAC2STR(prDeauthFrame->aucDestAddr),
		prDeauthFrame->u2ReasonCode));

	do {

		/*                                              */
		if (!prStaRec) {
			break;
		}

		if (IS_STA_IN_AIS(prStaRec)) {
			P_BSS_INFO_T prAisBssInfo;


			if (!IS_AP_STA(prStaRec)) {
				break;
			}

			prAisBssInfo = prAdapter->prAisBssInfo;

			if (prStaRec->ucStaState > STA_STATE_1) {

				/*                                                               */
				if (authProcessRxDeauthFrame(prSwRfb,
							     prStaRec->aucMacAddr,
							     &prStaRec->u2ReasonCode) ==
				    WLAN_STATUS_SUCCESS) {

#if CFG_SUPPORT_802_11W
					P_AIS_SPECIFIC_BSS_INFO_T prAisSpecBssInfo;

					prAisSpecBssInfo =
					    &(prAdapter->rWifiVar.rAisSpecificBssInfo);

					DBGLOG(RSN, INFO,
					       ("QM RX MGT: Deauth frame, P=%d Sec=%d CM=%d BC=%d fc=%02x\n",
						prAisSpecBssInfo->fgMgmtProtection,
						HAL_RX_STATUS_GET_SEC_MODE(prSwRfb->prRxStatus),
						HAL_RX_STATUS_IS_CIPHER_MISMATCH(prSwRfb->
										 prRxStatus),
						IS_BMCAST_MAC_ADDR(prDeauthFrame->aucDestAddr),
						prDeauthFrame->u2FrameCtrl));
					if (prAisSpecBssInfo->fgMgmtProtection && HAL_RX_STATUS_IS_CIPHER_MISMATCH(prSwRfb->prRxStatus)
					    /*                                                                     */
					    ) {
						saaChkDeauthfrmParamHandler(prAdapter, prSwRfb, prStaRec);
						return WLAN_STATUS_SUCCESS;
					}
#endif
					saaSendDisconnectMsgHandler(prAdapter, prStaRec, prAisBssInfo, FRM_DEAUTH);
				}
			}
		}
#if CFG_ENABLE_WIFI_DIRECT
		else if (prAdapter->fgIsP2PRegistered && IS_STA_IN_P2P(prStaRec)) {
			/*             */
			p2pRoleFsmRunEventRxDeauthentication(prAdapter, prStaRec, prSwRfb);
		}
#endif
#if CFG_ENABLE_BT_OVER_WIFI
		else if (IS_STA_BOW_TYPE(prStaRec)) {
			bowRunEventRxDeAuth(prAdapter, prStaRec, prSwRfb);
		}
#endif
		else {
			ASSERT(0);
		}

	} while (FALSE);

	return WLAN_STATUS_SUCCESS;

}				/*                                 */

/*          */
/*                                                                            */
/* 
                                                                                  
 
             
 
          
*/
/*                                                                            */

VOID
saaChkDeauthfrmParamHandler(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb, IN P_STA_RECORD_T prStaRec)
{
	P_WLAN_DEAUTH_FRAME_T prDeauthFrame;
	do {
		prDeauthFrame = (P_WLAN_DEAUTH_FRAME_T) prSwRfb->pvHeader;
		if (!IS_BMCAST_MAC_ADDR(prDeauthFrame->aucDestAddr) &&
			(prStaRec->u2ReasonCode == REASON_CODE_CLASS_2_ERR || prStaRec->u2ReasonCode == REASON_CODE_CLASS_3_ERR)) {
			DBGLOG(RSN, INFO, ("QM RX MGT: rsnStartSaQuery\n"));
			/*                       */
			rsnStartSaQuery(prAdapter);
		} else {
			DBGLOG(RSN, INFO, ("RXM: Drop unprotected Mgmt frame \n"));
			DBGLOG(RSN, INFO,
			       ("RXM: (MAC RX Done) RX (u2StatusFlag=0x%x) (ucKIdxSecMode=0x%x) (ucWlanIdx=0x%x)\n",
				prSwRfb->prRxStatus->u2StatusFlag,
				prSwRfb->prRxStatus->ucTidSecMode,
				prSwRfb->prRxStatus->ucWlanIdx));
		}
	} while (0);
}

/*          */
/*                                                                            */
/* 
                                                                            
 
             
 
          
*/
/*                                                                            */
VOID
saaSendDisconnectMsgHandler(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec, IN P_BSS_INFO_T prAisBssInfo, IN ENUM_AA_FRM_TYPE_T eFrmType)
{
	do {
		if (eFrmType == FRM_DEAUTH) {
			if (STA_STATE_3 == prStaRec->ucStaState) {
				P_MSG_AIS_ABORT_T prAisAbortMsg;

				/*                                                             
                                                                   
                     
     */
				cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);

				prAisAbortMsg = (P_MSG_AIS_ABORT_T) cnmMemAlloc(prAdapter, RAM_TYPE_MSG, sizeof(MSG_AIS_ABORT_T));
				if (!prAisAbortMsg) {
					break;
				}

				prAisAbortMsg->rMsgHdr.eMsgId = MID_SAA_AIS_FSM_ABORT;
				prAisAbortMsg->ucReasonOfDisconnect = DISCONNECT_REASON_CODE_DEAUTHENTICATED;
				prAisAbortMsg->fgDelayIndication = FALSE;
				mboxSendMsg(prAdapter,
					    MBOX_ID_0,
					    (P_MSG_HDR_T) prAisAbortMsg,
					    MSG_SEND_METHOD_BUF);
			} else {
				/*                            */
			}
			prAisBssInfo->u2DeauthReason = prStaRec->u2ReasonCode;
		} else { /*             */
			if (STA_STATE_3 == prStaRec->ucStaState) {
				P_MSG_AIS_ABORT_T prAisAbortMsg;
				prAisAbortMsg = (P_MSG_AIS_ABORT_T) cnmMemAlloc(prAdapter,
								    RAM_TYPE_MSG,
								    sizeof
								    (MSG_AIS_ABORT_T));
				if (!prAisAbortMsg) {
					break;
				}

				prAisAbortMsg->rMsgHdr.eMsgId = MID_SAA_AIS_FSM_ABORT;
				prAisAbortMsg->ucReasonOfDisconnect = DISCONNECT_REASON_CODE_DISASSOCIATED;
				prAisAbortMsg->fgDelayIndication = FALSE;
				mboxSendMsg(prAdapter,
					    MBOX_ID_0,
					    (P_MSG_HDR_T) prAisAbortMsg,
					    MSG_SEND_METHOD_BUF);
			} else {
				/*                            */
			}			
		}
	} while (0);
}

/*                                                                            */
/* 
                                                                    
 
                                                                  
 
                                                                       
*/
/*                                                                            */
WLAN_STATUS saaFsmRunEventRxDisassoc(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_STA_RECORD_T prStaRec;
	P_WLAN_DISASSOC_FRAME_T prDisassocFrame;

	ASSERT(prSwRfb);
	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
	prDisassocFrame = (P_WLAN_DISASSOC_FRAME_T) prSwRfb->pvHeader;

	DBGLOG(SAA, INFO,
	       ("Rx Disassoc frame from BSSID[" MACSTR "] DA[" MACSTR "] ReasonCode[0x%x]\n",
		MAC2STR(prDisassocFrame->aucBSSID), MAC2STR(prDisassocFrame->aucDestAddr),
		prDisassocFrame->u2ReasonCode));

	do {

		/*                                              */
		if (!prStaRec) {
			break;
		}

		if (IS_STA_IN_AIS(prStaRec)) {
			P_BSS_INFO_T prAisBssInfo;


			if (!IS_AP_STA(prStaRec)) {
				break;
			}

			prAisBssInfo = prAdapter->prAisBssInfo;

			if (prStaRec->ucStaState > STA_STATE_1) {

				/*                                                               */
				if (assocProcessRxDisassocFrame(prAdapter,
								prSwRfb,
								prStaRec->aucMacAddr,
								&prStaRec->u2ReasonCode) ==
				    WLAN_STATUS_SUCCESS) {

#if CFG_SUPPORT_802_11W
					P_AIS_SPECIFIC_BSS_INFO_T prAisSpecBssInfo;

					prAisSpecBssInfo =
					    &(prAdapter->rWifiVar.rAisSpecificBssInfo);

					DBGLOG(RSN, INFO,
					       ("QM RX MGT: Disassoc frame, P=%d Sec=%d CM=%d BC=%d fc=%02x\n",
						prAisSpecBssInfo->fgMgmtProtection,
						HAL_RX_STATUS_GET_SEC_MODE(prSwRfb->prRxStatus),
						HAL_RX_STATUS_IS_CIPHER_MISMATCH(prSwRfb->
										 prRxStatus),
						IS_BMCAST_MAC_ADDR(prDisassocFrame->aucDestAddr),
						prDisassocFrame->u2FrameCtrl));
					if (IS_STA_IN_AIS(prStaRec)
					    && prAisSpecBssInfo->fgMgmtProtection
					    && HAL_RX_STATUS_IS_CIPHER_MISMATCH(prSwRfb->
										prRxStatus)
					    /*                                                                      */
					    ) {
						/*                                                                */
						saaChkDisassocfrmParamHandler(prAdapter, prDisassocFrame, prStaRec, prSwRfb);
						return WLAN_STATUS_SUCCESS;
					}
#endif
					saaSendDisconnectMsgHandler(prAdapter, prStaRec, NULL, FRM_DISASSOC);
				}
			}
		}
#if CFG_ENABLE_WIFI_DIRECT
		else if (prAdapter->fgIsP2PRegistered && (IS_STA_IN_P2P(prStaRec))) {
			/*             */
			p2pRoleFsmRunEventRxDisassociation(prAdapter, prStaRec, prSwRfb);
		}
#endif
#if CFG_ENABLE_BT_OVER_WIFI
		else if (IS_STA_BOW_TYPE(prStaRec)) {
			/*             */
		}
#endif
		else {
			ASSERT(0);
		}

	} while (FALSE);

	return WLAN_STATUS_SUCCESS;

}				/*                                   */

/*          */
/*                                                                            */
/* 
                                                                                      
 
             
 
          
*/
/*                                                                            */

VOID
saaChkDisassocfrmParamHandler(IN P_ADAPTER_T prAdapter,
		IN P_WLAN_DISASSOC_FRAME_T prDisassocFrame, IN P_STA_RECORD_T prStaRec, IN P_SW_RFB_T prSwRfb)
{
	if (!IS_BMCAST_MAC_ADDR(prDisassocFrame->aucDestAddr) &&
		(prStaRec->u2ReasonCode == REASON_CODE_CLASS_2_ERR || prStaRec->u2ReasonCode == REASON_CODE_CLASS_3_ERR)) {
		/*                       */
		DBGLOG(RSN, INFO, ("QM RX MGT: rsnStartSaQuery\n"));
		rsnStartSaQuery(prAdapter);
	} else {
		DBGLOG(RSN, INFO, ("RXM: Drop unprotected Mgmt frame\n"));
		DBGLOG(RSN, INFO,
		       ("RXM: (MAC RX Done) RX (u2StatusFlag=0x%x) (ucKIdxSecMode=0x%x) (ucWlanIdx=0x%x)\n",
			prSwRfb->prRxStatus->u2StatusFlag,
			prSwRfb->prRxStatus->ucTidSecMode,
			prSwRfb->prRxStatus->ucWlanIdx));
	}
}

/*                                                                            */
/* 
                                                              
 
                                                                      
 
              
*/
/*                                                                            */
VOID saaFsmRunEventAbort(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr)
{
	P_MSG_SAA_FSM_ABORT_T prSaaFsmAbortMsg;
	P_STA_RECORD_T prStaRec;


	ASSERT(prMsgHdr);

	prSaaFsmAbortMsg = (P_MSG_SAA_FSM_ABORT_T) prMsgHdr;
	prStaRec = prSaaFsmAbortMsg->prStaRec;

	ASSERT(prStaRec);
	if (!prStaRec) {
		cnmMemFree(prAdapter, prMsgHdr);
		return;
	}

	DBGLOG(SAA, LOUD, ("EVENT-ABORT: Stop SAA FSM.\n"));

	cnmMemFree(prAdapter, prMsgHdr);


	/*                                       */
	prStaRec->ucTxAuthAssocRetryCount = 0;

	/*                            */
	cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);

	if (prStaRec->eAuthAssocState != AA_STATE_IDLE) {
#if DBG
		DBGLOG(SAA, LOUD, ("EVENT-ABORT: Previous Auth/Assoc State == %s.\n",
				   apucDebugAAState[prStaRec->eAuthAssocState]));
#else
		DBGLOG(SAA, LOUD, ("EVENT-ABORT: Previous Auth/Assoc State == %d.\n",
				   prStaRec->eAuthAssocState));
#endif
	}
#if 0
	/*                                  */
	prStaRec->eAuthAssocState = AA_STATE_IDLE;
#else
	/*                  */
	cnmStaRecFree(prAdapter, prStaRec);
#endif

	return;
}				/*                              */


/*                                                                  */
#if 0
/*                                                                            */
/* 
                                                                
 
                                                             
 
                                                            
*/
/*                                                                            */
WLAN_STATUS joinFsmRunEventJoinTimeOut(IN P_ADAPTER_T prAdapter)
{
	P_JOIN_INFO_T prJoinInfo;
	P_STA_RECORD_T prStaRec;

	DEBUGFUNC("joinFsmRunEventJoinTimeOut");


	ASSERT(prAdapter);
	prJoinInfo = &prAdapter->rJoinInfo;

	DBGLOG(JOIN, EVENT, ("JOIN EVENT: JOIN TIMEOUT\n"));

	/*                                                      */
	prStaRec = staRecGetStaRecordByAddr(prAdapter, prJoinInfo->prBssDesc->aucBSSID);

	/*                                                       */
	ASSERT(prStaRec);

	/*                                                  */
	prStaRec->u2StatusCode = STATUS_CODE_JOIN_TIMEOUT;

	/*                        */
	prStaRec->ucJoinFailureCount++;

	/*                                       */
	prJoinInfo->ucTxAuthAssocRetryCount = 0;

	/*                                  */
	ARB_CANCEL_TIMER(prAdapter, prJoinInfo->rTxRequestTimer);

	ARB_CANCEL_TIMER(prAdapter, prJoinInfo->rRxResponseTimer);

	/*                                                  */
	if (prAdapter->eConnectionState == MEDIA_STATE_CONNECTED) {
		joinAdoptParametersFromCurrentBss(prAdapter);
	}

	/*                   */
	joinFsmSteps(prAdapter, JOIN_STATE_IDLE);

	return WLAN_STATUS_FAILURE;

}				/*                                     */

/*                                                                            */
/* 
                                                               
 
                                                             
 
                
*/
/*                                                                            */
VOID joinAdoptParametersFromPeerBss(IN P_ADAPTER_T prAdapter)
{
	P_JOIN_INFO_T prJoinInfo;
	P_BSS_DESC_T prBssDesc;

	DEBUGFUNC("joinAdoptParametersFromPeerBss");


	ASSERT(prAdapter);
	prJoinInfo = &prAdapter->rJoinInfo;
	prBssDesc = prJoinInfo->prBssDesc;

	/*                                */
	prAdapter->eCurrentPhyType = prBssDesc->ePhyType;

	DBGLOG(JOIN, INFO, ("Target BSS[%s]'s PhyType = %s\n",
			    prBssDesc->aucSSID,
			    (prBssDesc->ePhyType == PHY_TYPE_ERP_INDEX) ? "ERP" : "HR_DSSS"));


	/*                                               */
	DBGLOG(JOIN, INFO, ("Target BSS's Channel = %d, Band = %d\n",
			    prBssDesc->ucChannelNum, prBssDesc->eBand));

	nicSwitchChannel(prAdapter, prBssDesc->eBand, prBssDesc->ucChannelNum, 10);

	prJoinInfo->fgIsParameterAdopted = TRUE;

	return;
}				/*                                         */


/*                                                                            */
/* 
                                                                             
 
                                                             
 
                
*/
/*                                                                            */
VOID joinAdoptParametersFromCurrentBss(IN P_ADAPTER_T prAdapter)
{
	/*                                                   */
	P_BSS_INFO_T prBssInfo;


	ASSERT(prAdapter);
	prBssInfo = &prAdapter->rBssInfo;

	/*                                   */
	prAdapter->eCurrentPhyType = prBssInfo->ePhyType;

	/*                                                  */
	DBGLOG(JOIN, INFO, ("Current BSS's Channel = %d, Band = %d\n",
			    prBssInfo->ucChnl, prBssInfo->eBand));

	nicSwitchChannel(prAdapter, prBssInfo->eBand, prBssInfo->ucChnl, 10);
	return;
}				/*                                            */


/*                                                                            */
/* 
                                                                                  
                                         
 
                                                             
 
                
*/
/*                                                                            */
VOID joinComplete(IN P_ADAPTER_T prAdapter)
{
	P_JOIN_INFO_T prJoinInfo;
	P_BSS_DESC_T prBssDesc;
	P_PEER_BSS_INFO_T prPeerBssInfo;
	P_BSS_INFO_T prBssInfo;
	P_CONNECTION_SETTINGS_T prConnSettings;
	P_STA_RECORD_T prStaRec;
	P_TX_CTRL_T prTxCtrl;
#if CFG_SUPPORT_802_11D
	P_IE_COUNTRY_T prIECountry;
#endif

	DEBUGFUNC("joinComplete");


	ASSERT(prAdapter);
	prJoinInfo = &prAdapter->rJoinInfo;
	prBssDesc = prJoinInfo->prBssDesc;
	prPeerBssInfo = &prAdapter->rPeerBssInfo;
	prBssInfo = &prAdapter->rBssInfo;
	prConnSettings = &prAdapter->rConnSettings;
	prTxCtrl = &prAdapter->rTxCtrl;

/*                                                         */
	/*                                               */
	scanRemoveConnectionFlagOfBssDescByBssid(prAdapter, prBssInfo->aucBSSID);

	prBssDesc->fgIsConnected = TRUE;	/*                   */

	if (prBssDesc->fgIsHiddenSSID) {
		/*                                                                            
                                        
   */
		COPY_SSID(prBssDesc->aucSSID,
			  prBssDesc->ucSSIDLen,
			  prAdapter->rConnSettings.aucSSID, prAdapter->rConnSettings.ucSSIDLen);

		if (prBssDesc->ucSSIDLen) {
			prBssDesc->fgIsHiddenSSID = FALSE;
		}
#if DBG
		else {
			ASSERT(0);
		}
#endif				/*     */

		DBGLOG(JOIN, INFO, ("Hidden SSID! - Update SSID : %s\n", prBssDesc->aucSSID));
	}

/*                                         */
	/*                  */
	prBssInfo->ePhyType = prBssDesc->ePhyType;

	/*                  */
	prBssInfo->eBSSType = BSS_TYPE_INFRASTRUCTURE;

	/*               */
	COPY_MAC_ADDR(prBssInfo->aucBSSID, prBssDesc->aucBSSID);

	DBGLOG(JOIN, INFO, ("JOIN to BSSID: [" MACSTR "]\n", MAC2STR(prBssDesc->aucBSSID)));


	/*              */
	COPY_SSID(prBssInfo->aucSSID,
		  prBssInfo->ucSSIDLen, prBssDesc->aucSSID, prBssDesc->ucSSIDLen);

	/*                                     */
	prBssInfo->eBand = prBssDesc->eBand;
	prBssInfo->ucChnl = prBssDesc->ucChannelNum;

	/*                              */
	secFsmRunEventStart(prAdapter);
	prBssInfo->u4RsnSelectedPairwiseCipher = prBssDesc->u4RsnSelectedPairwiseCipher;
	prBssInfo->u4RsnSelectedGroupCipher = prBssDesc->u4RsnSelectedGroupCipher;
	prBssInfo->u4RsnSelectedAKMSuite = prBssDesc->u4RsnSelectedAKMSuite;

	if (secRsnKeyHandshakeEnabled()) {
		prBssInfo->fgIsWPAorWPA2Enabled = TRUE;
	} else {
		prBssInfo->fgIsWPAorWPA2Enabled = FALSE;
	}

	/*                          */
	prBssInfo->u2BeaconInterval = prBssDesc->u2BeaconInterval;

	/*                      */
	prBssInfo->ucDtimPeriod = prBssDesc->ucDTIMPeriod;

	/*                         */
	if ((prBssInfo->ePhyType == PHY_TYPE_ERP_INDEX) &&	/*                                */
	    (prBssDesc->fgIsERPPresent)) {

		prBssInfo->fgIsERPPresent = TRUE;
		prBssInfo->ucERP = prBssDesc->ucERP;	/*                              */
	} else {		/*                                                                                      */
		prBssInfo->fgIsERPPresent = FALSE;
		prBssInfo->ucERP = 0;
	}

#if CFG_SUPPORT_802_11D
	/*                                                  */
	if (prConnSettings->fgMultiDomainCapabilityEnabled) {
		DOMAIN_INFO_ENTRY rDomainInfo;
		if (domainGetDomainInfoByScanResult(prAdapter, &rDomainInfo)) {
			if (prBssDesc->prIECountry) {
				prIECountry = prBssDesc->prIECountry;

				domainParseCountryInfoElem(prIECountry, &prBssInfo->rDomainInfo);

				/*                                      */
				prBssInfo->fgIsCountryInfoPresent = TRUE;
				nicSetupOpChnlList(prAdapter, prBssInfo->rDomainInfo.u2CountryCode,
						   FALSE);
			} else {
				/*                                         */
				prBssInfo->fgIsCountryInfoPresent = TRUE;
				nicSetupOpChnlList(prAdapter, rDomainInfo.u2CountryCode, FALSE);
			}
		}
	}
#endif

	/*                                           */
	prBssInfo->rRcpi = prBssDesc->rRcpi;
	prBssInfo->rRssi = RCPI_TO_dBm(prBssInfo->rRcpi);
	GET_CURRENT_SYSTIME(&prBssInfo->rRssiLastUpdateTime);

	/*                                               */
	prBssInfo->u2CapInfo = prBssDesc->u2CapInfo;

	DBGLOG(JOIN, INFO,
	       ("prBssInfo-> fgIsERPPresent = %d, ucERP = %02x, rRcpi = %d, rRssi = %ld\n",
		prBssInfo->fgIsERPPresent, prBssInfo->ucERP, prBssInfo->rRcpi, prBssInfo->rRssi));


/*                                                              */
	/*                        */
	prBssInfo->u2AssocId = prPeerBssInfo->u2AssocId;

	/*                        */
	if (prAdapter->fgIsEnableWMM && (prPeerBssInfo->rWmmInfo.ucWmmFlag & WMM_FLAG_SUPPORT_WMM)) {

		prBssInfo->fgIsWmmAssoc = TRUE;
		prTxCtrl->rTxQForVoipAccess = TXQ_AC3;

		qosWmmInfoInit(&prBssInfo->rWmmInfo,
			       (prBssInfo->ePhyType == PHY_TYPE_HR_DSSS_INDEX) ? TRUE : FALSE);

		if (prPeerBssInfo->rWmmInfo.ucWmmFlag & WMM_FLAG_AC_PARAM_PRESENT) {
			kalMemCopy(&prBssInfo->rWmmInfo,
				   &prPeerBssInfo->rWmmInfo, sizeof(WMM_INFO_T));
		} else {
			kalMemCopy(&prBssInfo->rWmmInfo,
				   &prPeerBssInfo->rWmmInfo,
				   sizeof(WMM_INFO_T) -
				   sizeof(prPeerBssInfo->rWmmInfo.arWmmAcParams));
		}
	} else {
		prBssInfo->fgIsWmmAssoc = FALSE;
		prTxCtrl->rTxQForVoipAccess = TXQ_AC1;

		kalMemZero(&prBssInfo->rWmmInfo, sizeof(WMM_INFO_T));
	}


	/*                                                   */
	prBssInfo->u2OperationalRateSet = prPeerBssInfo->u2OperationalRateSet;
	prBssInfo->u2BSSBasicRateSet = prPeerBssInfo->u2BSSBasicRateSet;


	/*                        */
	if (prBssInfo->fgIsERPPresent) {

		/*                                     
                          
                                                                                       
                                                                                                              
                                                                 
                                                                                                              
                                                                 
                                                                            
                                                                             
                                                                                                                       
                                                                                         
   */
		if ((prPeerBssInfo->fgIsShortPreambleAllowed) && ((prConnSettings->ePreambleType == PREAMBLE_TYPE_SHORT) ||	/*                                      */
								  ((prConnSettings->ePreambleType ==
								    PREAMBLE_TYPE_AUTO)
								   && (prBssDesc->
								       u2CapInfo &
								       CAP_INFO_SHORT_PREAMBLE)))) {

			prBssInfo->fgIsShortPreambleAllowed = TRUE;

			if (prBssInfo->ucERP & ERP_INFO_BARKER_PREAMBLE_MODE) {
				prBssInfo->fgUseShortPreamble = FALSE;
			} else {
				prBssInfo->fgUseShortPreamble = TRUE;
			}
		} else {
			prBssInfo->fgIsShortPreambleAllowed = FALSE;
			prBssInfo->fgUseShortPreamble = FALSE;
		}
	} else {
		/*                                     
                          
                                                                       
                                                 
                                                 
                                                
                                                
                                           
   */

		prBssInfo->fgIsShortPreambleAllowed = prBssInfo->fgUseShortPreamble =
		    prPeerBssInfo->fgIsShortPreambleAllowed;
	}

	DBGLOG(JOIN, INFO,
	       ("prBssInfo->fgIsShortPreambleAllowed = %d, prBssInfo->fgUseShortPreamble = %d\n",
		prBssInfo->fgIsShortPreambleAllowed, prBssInfo->fgUseShortPreamble));


	/*                         */
	prBssInfo->fgUseShortSlotTime = prPeerBssInfo->fgUseShortSlotTime;	/*                            */

	DBGLOG(JOIN, INFO, ("prBssInfo->fgUseShortSlotTime = %d\n", prBssInfo->fgUseShortSlotTime));

	nicSetSlotTime(prAdapter,
		       prBssInfo->ePhyType,
		       ((prConnSettings->fgIsShortSlotTimeOptionEnable &&
			 prBssInfo->fgUseShortSlotTime) ? TRUE : FALSE));


	/*                                          */
	bssUpdateTxRateForControlFrame(prAdapter);


	/*                                                                                        */
	/*                                   *//*                                              */
	{

		if (prJoinInfo->ucCurrAuthAlgNum == AUTH_ALGORITHM_NUM_OPEN_SYSTEM) {
			prJoinInfo->ucRoamingAuthTypes |= AUTH_TYPE_OPEN_SYSTEM;
		} else if (prJoinInfo->ucCurrAuthAlgNum == AUTH_ALGORITHM_NUM_SHARED_KEY) {
			prJoinInfo->ucRoamingAuthTypes |= AUTH_TYPE_SHARED_KEY;
		}

		prBssInfo->ucRoamingAuthTypes = prJoinInfo->ucRoamingAuthTypes;


		/*                                                                        
                            
   */
		SET_EXPIRATION_TIME(prBssInfo->rRoamingStableExpirationTime,
				    SEC_TO_SYSTIME(ROAMING_STABLE_TIMEOUT_SEC));
	}


	/*                                                         */
#if CFG_TX_FRAGMENT
	txFragInfoUpdate(prAdapter);
#endif				/*                 */


/*                           */
	/*                                  */
	prStaRec = staRecGetStaRecordByAddr(prAdapter, prBssDesc->aucBSSID);

	if (prStaRec) {
		UINT_16 u2OperationalRateSet, u2DesiredRateSet;

		/*                          */
		u2OperationalRateSet = (rPhyAttributes[prBssInfo->ePhyType].u2SupportedRateSet &
					prBssInfo->u2OperationalRateSet);

		u2DesiredRateSet = (u2OperationalRateSet & prConnSettings->u2DesiredRateSet);
		if (u2DesiredRateSet) {
			prStaRec->u2DesiredRateSet = u2DesiredRateSet;
		} else {
			/*                                                                                   */
			prStaRec->u2DesiredRateSet = u2OperationalRateSet;
		}

		/*                                                 */
		if (!rateGetBestInitialRateIndex(prStaRec->u2DesiredRateSet,
						 prStaRec->rRcpi, &prStaRec->ucCurrRate1Index)) {

			if (!rateGetLowestRateIndexFromRateSet(prStaRec->u2DesiredRateSet,
							       &prStaRec->ucCurrRate1Index)) {
				ASSERT(0);
			}
		}

		DBGLOG(JOIN, INFO, ("prStaRec->ucCurrRate1Index = %d\n",
				    prStaRec->ucCurrRate1Index));

		/*                       */
		prStaRec->fgIsShortPreambleOptionEnable = prBssInfo->fgUseShortPreamble;

		/*                  */
		prStaRec->fgIsQoS = prBssInfo->fgIsWmmAssoc;
	}
#if DBG
	else {
		ASSERT(0);
	}
#endif				/*     */


/*                  */
	/*                                       */
	nicSetupBSS(prAdapter, prBssInfo);

	/*                            */
	if (nicSetHwBySta(prAdapter, prStaRec) == FALSE) {
		ASSERT(FALSE);
	}
	/*                                         */
#if CFG_TX_FRAGMENT
	if (prConnSettings->fgIsEnableTxAutoFragmentForBT) {
		txRateSetInitForBT(prAdapter, prStaRec);
	}
#endif				/*                 */

	/*                                                 */
	if (prBssInfo->fgIsWmmAssoc) {

#if CFG_TX_AGGREGATE_HW_FIFO
		nicTxAggregateTXQ(prAdapter, FALSE);
#endif				/*                          */

		qosUpdateWMMParametersAndAssignAllowedACI(prAdapter, &prBssInfo->rWmmInfo);
	} else {

#if CFG_TX_AGGREGATE_HW_FIFO
		nicTxAggregateTXQ(prAdapter, TRUE);
#endif				/*                          */

		nicTxNonQoSAssignDefaultAdmittedTXQ(prAdapter);

		nicTxNonQoSUpdateTXQParameters(prAdapter, prBssInfo->ePhyType);
	}

#if CFG_TX_STOP_WRITE_TX_FIFO_UNTIL_JOIN
	{
		prTxCtrl->fgBlockTxDuringJoin = FALSE;

#if !CFG_TX_AGGREGATE_HW_FIFO	/*                                         */
		nicTxFlushStopQueues(prAdapter, (UINT_8) TXQ_DATA_MASK, (UINT_8) NULL);
#endif				/*                          */

		nicTxRetransmitOfSendWaitQue(prAdapter);

		if (prTxCtrl->fgIsPacketInOsSendQueue) {
			nicTxRetransmitOfOsSendQue(prAdapter);
		}
#if CFG_SDIO_TX_ENHANCE
		halTxLeftClusteredMpdu(prAdapter);
#endif				/*                     */

	}
#endif				/*                                      */


/*                              */
	prAdapter->eConnectionState = MEDIA_STATE_CONNECTED;
	prAdapter->eConnectionStateIndicated = MEDIA_STATE_CONNECTED;

	if (prJoinInfo->fgIsReAssoc) {
		prAdapter->fgBypassPortCtrlForRoaming = TRUE;
	} else {
		prAdapter->fgBypassPortCtrlForRoaming = FALSE;
	}

	kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
				     WLAN_STATUS_MEDIA_CONNECT, (PVOID) NULL, 0);

	return;
}				/*                       */
#endif
