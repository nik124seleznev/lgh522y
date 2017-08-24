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

/*                                                                              
                                                
                                                                                
*/
#if 0
/*                                                                            */
/* 
                                                     
 
                                                                    
                                                           
                                                       

              
*/
/*                                                                            */
WLAN_STATUS
aaaFsmSendEventJoinComplete (
    WLAN_STATUS rJoinStatus,
    P_STA_RECORD_T prStaRec,
    P_SW_RFB_T prSwRfb
    )
{
    P_MSG_SAA_JOIN_COMP_T prJoinCompMsg;


    ASSERT(prStaRec);

    prJoinCompMsg = cnmMemAlloc(RAM_TYPE_TCM, sizeof(MSG_SAA_JOIN_COMP_T));
    if (!prJoinCompMsg) {
        return WLAN_STATUS_RESOURCES;
    }

    if (IS_STA_IN_AIS(prStaRec)) {
        prJoinCompMsg->rMsgHdr.eMsgId = MID_SAA_AIS_JOIN_COMPLETE;
    }
    else if (IS_STA_IN_P2P(prStaRec)) {
        prJoinCompMsg->rMsgHdr.eMsgId = MID_SAA_P2P_JOIN_COMPLETE;
    }
    else if (IS_STA_IN_BOW(prStaRec)) {
        prJoinCompMsg->rMsgHdr.eMsgId = MID_SAA_BOW_JOIN_COMPLETE;
    }
    else {
        ASSERT(0);
    }

    prJoinCompMsg->rJoinStatus = rJoinStatus;
    prJoinCompMsg->prStaRec = prStaRec;
    prJoinCompMsg->prSwRfb = prSwRfb;

    mboxSendMsg(MBOX_ID_0,
                (P_MSG_HDR_T)prJoinCompMsg,
                MSG_SEND_METHOD_BUF);

    return WLAN_STATUS_SUCCESS;

} /*                                      */

/*                                                                            */
/* 
                                                              
 
                                                                     
 
              
*/
/*                                                                            */
VOID
aaaFsmRunEventStart (
    IN P_MSG_HDR_T prMsgHdr
    )
{
    P_MSG_SAA_JOIN_REQ_T prJoinReqMsg;
    P_STA_RECORD_T prStaRec;
    P_AIS_BSS_INFO_T prAisBssInfo;


    ASSERT(prMsgHdr);

    prJoinReqMsg = (P_MSG_SAA_JOIN_REQ_T)prMsgHdr;
    prStaRec = prJoinReqMsg->prStaRec;

    ASSERT(prStaRec);

    DBGLOG(SAA, LOUD, ("EVENT-START: Trigger SAA FSM\n"));

    cnmMemFree(prMsgHdr);

    //                                   
    if (!IS_AP_STA(prStaRec->eStaType)) {

        DBGLOG(SAA, ERROR, ("EVENT-START: STA Type - %d was not supported.\n", prStaRec->eStaType));

        /*                                                        */
        saaFsmSendEventJoinComplete(WLAN_STATUS_FAILURE, prStaRec, NULL);

        return;
    }

    //                                                  
    if (prStaRec->eAuthAssocState != AA_STATE_IDLE) {
        DBGLOG(SAA, ERROR, ("EVENT-START: Reentry of SAA Module.\n"));
        prStaRec->eAuthAssocState = AA_STATE_IDLE;
    }

    //                                
    /*                                            */
    prStaRec->u2StatusCode = STATUS_CODE_SUCCESSFUL;

    /*                              */
    GET_CURRENT_SYSTIME(&prStaRec->rLastJoinTime);

    prStaRec->ucTxAuthAssocRetryCount = 0;

    if (prStaRec->prChallengeText) {
        cnmMemFree(prStaRec->prChallengeText);
        prStaRec->prChallengeText = (P_IE_CHALLENGE_TEXT_T)NULL;
    }

    cnmTimerStopTimer(&prStaRec->rTxReqDoneOrRxRespTimer);

    prStaRec->ucStaState = STA_STATE_1;

    /*                    */
    saaFsmSteps(prStaRec, SAA_STATE_SEND_AUTH1, (P_SW_RFB_T)NULL);

    return;
} /*                              */
#endif


#if CFG_SUPPORT_AAA
/*                                                                            */
/* 
                                                                      
                         
 
                                                                 
                                                                  
 
                
*/
/*                                                                            */
VOID
aaaFsmRunEventRxAuth (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
    )
{
    P_BSS_INFO_T prBssInfo;
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;
    UINT_16 u2StatusCode;
    BOOLEAN fgReplyAuth = FALSE;
    ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex;


    ASSERT(prAdapter);

    do {


        //                                  
#if CFG_ENABLE_WIFI_DIRECT
        if(prAdapter->fgIsP2PRegistered){
            prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);

            if (prBssInfo->fgIsNetActive) {

                //                                                            
                if (WLAN_STATUS_SUCCESS ==
                    authProcessRxAuth1Frame(prAdapter,
                                            prSwRfb,
                                            prBssInfo->aucBSSID,
                                            AUTH_ALGORITHM_NUM_OPEN_SYSTEM,
                                            AUTH_TRANSACTION_SEQ_1,
                                            &u2StatusCode)) {

                    if (STATUS_CODE_SUCCESSFUL == u2StatusCode) {
                        //                                                           
                        fgReplyAuth = p2pFuncValidateAuth(
                                            prAdapter,
                                            prSwRfb,
                                            &prStaRec,
                                            &u2StatusCode);
                    }
                    else {
                        fgReplyAuth = TRUE;
                    }
                    eNetTypeIndex = NETWORK_TYPE_P2P_INDEX;
                    break;
                }
            }
        }
#endif /*                        */

        //                                  
#if CFG_ENABLE_BT_OVER_WIFI
        {
            prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_BOW_INDEX]);

            if ((prBssInfo->fgIsNetActive) &&
            (OP_MODE_BOW == prBssInfo->eCurrentOPMode)) {

                //                                                            
                /*                         */
                if (WLAN_STATUS_SUCCESS ==
                    authProcessRxAuth1Frame(prAdapter,
                                            prSwRfb,
                                            prBssInfo->aucBSSID,
                                            AUTH_ALGORITHM_NUM_OPEN_SYSTEM,
                                            AUTH_TRANSACTION_SEQ_1,
                                            &u2StatusCode)) {

                    if (STATUS_CODE_SUCCESSFUL == u2StatusCode) {

                        //                                                           
                        fgReplyAuth = bowValidateAuth(prAdapter, prSwRfb, &prStaRec, &u2StatusCode);

                    }
                    else {

                        fgReplyAuth = TRUE;
                    }
                    eNetTypeIndex = NETWORK_TYPE_BOW_INDEX;
                    /*                                                     */
                    break;
                }
            }
        }
#endif /*                         */

        return;
    } while (FALSE);

    if(prStaRec) {
        /*             */
        prStaRec->ucRCPI = prSwRfb->prHifRxHdr->ucRcpi;
    }

    //                                                                    
    if (fgReplyAuth) {

        if (prStaRec) {

            if (u2StatusCode == STATUS_CODE_SUCCESSFUL) {
                if (prStaRec->eAuthAssocState != AA_STATE_IDLE) {
                    DBGLOG(AAA, WARN, ("Previous AuthAssocState (%d) != IDLE.\n",
                        prStaRec->eAuthAssocState));
                }

                prStaRec->eAuthAssocState = AAA_STATE_SEND_AUTH2;
            }
            else {
                prStaRec->eAuthAssocState = AA_STATE_IDLE;

                /*                                */
                cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);
            }

            /*                              */
            GET_CURRENT_SYSTIME(&prStaRec->rUpdateTime);

            /*                                            */
            prStaRec->u2StatusCode = u2StatusCode;

            prStaRec->ucAuthAlgNum = AUTH_ALGORITHM_NUM_OPEN_SYSTEM;
        }
        else {
            /*                                                                            */
            ASSERT(!(u2StatusCode == STATUS_CODE_SUCCESSFUL));
        }

        /*                                        */
        //                 
        authSendAuthFrame(prAdapter,
                        prStaRec,
                        eNetTypeIndex,
                        prSwRfb,
                        AUTH_TRANSACTION_SEQ_2,
                        u2StatusCode);

    }

    return;
} /*                               */


/*                                                                            */
/* 
                                                                                 
                         
 
                                                                 
                                                                  
 
                                                             
*/
/*                                                                            */
WLAN_STATUS
aaaFsmRunEventRxAssoc (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
    )
{
    P_BSS_INFO_T prBssInfo;
    P_STA_RECORD_T prStaRec = (P_STA_RECORD_T)NULL;
    UINT_16 u2StatusCode = STATUS_CODE_RESERVED;
    BOOLEAN fgReplyAssocResp = FALSE;


    ASSERT(prAdapter);

    do {

        //                                                              
        prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);

        /*                                              */
        if ((!prStaRec) || (!prStaRec->fgIsInUse)) {
            ASSERT(0); //                     
            break;
        }

        if (!IS_CLIENT_STA(prStaRec)) {
            break;
        }

        if (prStaRec->ucStaState == STA_STATE_3) {
            /*                 */
        }
        else if ((prStaRec->ucStaState == STA_STATE_2) &&
                 (prStaRec->eAuthAssocState == AAA_STATE_SEND_AUTH2)) {
            /*             */
        }
        else {
            DBGLOG(AAA, WARN, ("Previous AuthAssocState (%d) != SEND_AUTH2.\n",
                prStaRec->eAuthAssocState));
            break;
        }

        /*             */
        prStaRec->ucRCPI = prSwRfb->prHifRxHdr->ucRcpi;

        //                                  
#if CFG_ENABLE_WIFI_DIRECT
        if ((prAdapter->fgIsP2PRegistered) && (IS_STA_IN_P2P(prStaRec))) {

            prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);

            if (prBssInfo->fgIsNetActive) {

                //                                                    
                /*                         */
                if (WLAN_STATUS_SUCCESS ==
                    assocProcessRxAssocReqFrame(prAdapter,
                                                prSwRfb,
                                                &u2StatusCode)) {

                    if (STATUS_CODE_SUCCESSFUL == u2StatusCode) {
                        //                                                                 
                        fgReplyAssocResp = p2pFuncValidateAssocReq(
                                                prAdapter,
                                                prSwRfb,
                                                (PUINT_16)&u2StatusCode);
                    }
                    else {
                        fgReplyAssocResp = TRUE;
                    }

                    break;
                }
            }
        }
#endif /*                        */

        //                                  
#if CFG_ENABLE_BT_OVER_WIFI
        if (IS_STA_IN_BOW(prStaRec)) {

            prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_BOW_INDEX]);

            if ((prBssInfo->fgIsNetActive) &&
                (OP_MODE_BOW == prBssInfo->eCurrentOPMode)){

                //                                                            
                /*                         */
                if (WLAN_STATUS_SUCCESS ==
                    assocProcessRxAssocReqFrame(prAdapter,
                                                prSwRfb,
                                                &u2StatusCode)) {

                    if (STATUS_CODE_SUCCESSFUL == u2StatusCode) {

                        //                                                           
                        fgReplyAssocResp = bowValidateAssocReq(prAdapter, prSwRfb, &u2StatusCode);

                    }
                    else {

                        fgReplyAssocResp = TRUE;
                    }

                    /*                                                     */
                    break;
                }
            }
        }
#endif /*                         */

        return WLAN_STATUS_SUCCESS; //                        
    } while (FALSE);


    //                                                    
    if (fgReplyAssocResp) {
        UINT_16     u2IELength;
        PUINT_8     pucIE;

        if ((((P_WLAN_ASSOC_REQ_FRAME_T)(prSwRfb->pvHeader))->u2FrameCtrl & MASK_FRAME_TYPE) ==
            MAC_FRAME_REASSOC_REQ) {

            u2IELength = prSwRfb->u2PacketLen -
                (UINT_16)OFFSET_OF(WLAN_REASSOC_REQ_FRAME_T, aucInfoElem[0]);

            pucIE = ((P_WLAN_REASSOC_REQ_FRAME_T)(prSwRfb->pvHeader))->aucInfoElem;
        }
        else {
            u2IELength = prSwRfb->u2PacketLen -
                (UINT_16)OFFSET_OF(WLAN_ASSOC_REQ_FRAME_T, aucInfoElem[0]);

            pucIE = ((P_WLAN_ASSOC_REQ_FRAME_T)(prSwRfb->pvHeader))->aucInfoElem;
        }

        rlmProcessAssocReq(prAdapter, prSwRfb, pucIE, u2IELength);

        //                             
        if (u2StatusCode == STATUS_CODE_SUCCESSFUL) {

#if CFG_ENABLE_WIFI_DIRECT
            if ((prAdapter->fgIsP2PRegistered) && (IS_STA_IN_P2P(prStaRec))) {
                if (p2pRunEventAAAComplete(prAdapter, prStaRec) == WLAN_STATUS_SUCCESS) {
                    prStaRec->u2AssocId = bssAssignAssocID(prStaRec);
                    //                                          
                    prStaRec->eAuthAssocState = AAA_STATE_SEND_ASSOC2; //                         

                    /*                                                                */
                    //                                                       
                }
                else {
                    /*                   */
                    u2StatusCode = STATUS_CODE_REQ_DECLINED;

                    prStaRec->u2AssocId = 0; /*                       */

                    /*                                                                           */
                    prStaRec->eAuthAssocState = AAA_STATE_SEND_AUTH2;

                    /*                                                          */
                    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_2);
                }
            }
#endif

#if CFG_ENABLE_BT_OVER_WIFI
            if ((IS_STA_IN_BOW(prStaRec))) {

//                                                                                             
                prStaRec->u2AssocId = bssAssignAssocID(prStaRec);
                prStaRec->eAuthAssocState = AAA_STATE_SEND_ASSOC2; //                         

                /*                                                                */
                //                                                       
            }
#if 0
                else {
                    /*                   */
                    u2StatusCode = STATUS_CODE_REQ_DECLINED;

                    prStaRec->u2AssocId = 0; /*                       */

                    /*                                                                           */
                    prStaRec->eAuthAssocState = AAA_STATE_SEND_AUTH2;

                    /*                                                          */
                    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_2);
                }
            }
#endif
#endif
        }
        else {
            prStaRec->u2AssocId = 0; /*                       */

            /*                                                                           */
            prStaRec->eAuthAssocState = AAA_STATE_SEND_AUTH2;

            /*                                                          */
            cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_2);
        }

        /*                              */
        GET_CURRENT_SYSTIME(&prStaRec->rUpdateTime);

        /*                                            */
        prStaRec->u2StatusCode = u2StatusCode;

        /*                                        */
        //                         
        assocSendReAssocRespFrame(prAdapter, prStaRec);

    }

    return WLAN_STATUS_SUCCESS;

} /*                                */


/*                                                                            */
/* 
                                                                           
 
                                                             
                                                       
                                                                               
 
                             
*/
/*                                                                            */
WLAN_STATUS
aaaFsmRunEventTxDone (
    IN P_ADAPTER_T              prAdapter,
    IN P_MSDU_INFO_T            prMsduInfo,
    IN ENUM_TX_RESULT_CODE_T    rTxDoneStatus
    )
{
    P_STA_RECORD_T prStaRec;
    P_BSS_INFO_T prBssInfo;


    ASSERT(prAdapter);
    ASSERT(prMsduInfo);

    DBGLOG(AAA, LOUD, ("EVENT-TX DONE: Current Time = %lu\n",
		(unsigned long)kalGetTimeTick()));

    prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

    if ((!prStaRec) || (!prStaRec->fgIsInUse)) {
        return WLAN_STATUS_SUCCESS; /*                                            */
    }

    ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

    prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

    switch (prStaRec->eAuthAssocState) {
    case AAA_STATE_SEND_AUTH2:
        {
            /*                                                                    */
            if (authCheckTxAuthFrame(prAdapter,
                                    prMsduInfo,
                                    AUTH_TRANSACTION_SEQ_2) != WLAN_STATUS_SUCCESS) {
                break;
            }

            if (STATUS_CODE_SUCCESSFUL == prStaRec->u2StatusCode) {
                if (TX_RESULT_SUCCESS == rTxDoneStatus) {

                    /*                                           */
                    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_2);
                }
                else {

                    prStaRec->eAuthAssocState = AA_STATE_IDLE;

                    /*                                */
                    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_1);

#if CFG_ENABLE_WIFI_DIRECT
                    if ((prAdapter->fgIsP2PRegistered) &&
                        (IS_STA_IN_P2P(prStaRec))) {
                        p2pRunEventAAATxFail(prAdapter, prStaRec);
                    }
#endif /*                        */

#if CFG_ENABLE_BT_OVER_WIFI
                    if (IS_STA_IN_BOW(prStaRec)) {
                        bowRunEventAAATxFail(prAdapter, prStaRec);
                    }
#endif /*                         */
                }

            }
            /*                                                                            */

        }
        break;

    case AAA_STATE_SEND_ASSOC2:
        {
            /*                                                                     */
            if (assocCheckTxReAssocRespFrame(prAdapter, prMsduInfo) != WLAN_STATUS_SUCCESS) {
                break;
            }

            if (STATUS_CODE_SUCCESSFUL == prStaRec->u2StatusCode) {
                if (TX_RESULT_SUCCESS == rTxDoneStatus) {

                    prStaRec->eAuthAssocState = AA_STATE_IDLE;

                    /*                                           */
#if CFG_ENABLE_WIFI_DIRECT
                    if ((prAdapter->fgIsP2PRegistered) &&
                        (IS_STA_IN_P2P(prStaRec))) {
                        p2pRunEventAAASuccess(prAdapter, prStaRec);
                    }
#endif /*                        */

#if CFG_ENABLE_BT_OVER_WIFI

                    if (IS_STA_IN_BOW(prStaRec))
                        {
                        bowRunEventAAAComplete(prAdapter, prStaRec);
                    }
#endif /*                         */

                }
                else {

                    prStaRec->eAuthAssocState = AAA_STATE_SEND_AUTH2;

                    /*                                */
                    cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_2);

#if CFG_ENABLE_WIFI_DIRECT
                    if ((prAdapter->fgIsP2PRegistered) &&
                        (IS_STA_IN_P2P(prStaRec))) {
                        p2pRunEventAAATxFail(prAdapter, prStaRec);
                    }
#endif /*                        */

#if CFG_ENABLE_BT_OVER_WIFI
                    if (IS_STA_IN_BOW(prStaRec)) {
                        bowRunEventAAATxFail(prAdapter, prStaRec);
                    }
#endif /*                         */

                }
            }
            /*                                                                            */
        }
        break;

    default:
        break; /*                    */
    }


    return WLAN_STATUS_SUCCESS;

} /*                               */
#endif /*                 */


#if 0 /*                                                            */
/*                                                                            */
/* 
                                                         
 
                                                         
 
              
*/
/*                                                                            */
VOID
saaFsmRunEventAbort (
    IN P_MSG_HDR_T prMsgHdr
    )
{
    P_JOIN_INFO_T prJoinInfo;
    P_STA_RECORD_T prStaRec;

    DEBUGFUNC("joinFsmRunEventAbort");


    ASSERT(prAdapter);
    prJoinInfo = &prAdapter->rJoinInfo;

    DBGLOG(JOIN, EVENT, ("JOIN EVENT: ABORT\n"));


    /*                                                                          */
    ASSERT(prJoinInfo->prBssDesc);

    //                                               
    /*                                       */
    prJoinInfo->ucTxAuthAssocRetryCount = 0;

    /*                                */
    ARB_CANCEL_TIMER(prAdapter,
                     prJoinInfo->rTxRequestTimer);

    ARB_CANCEL_TIMER(prAdapter,
                     prJoinInfo->rRxResponseTimer);

    ARB_CANCEL_TIMER(prAdapter,
                     prJoinInfo->rJoinTimer);

    //                                                     
    /*                                                      */
    prStaRec = staRecGetStaRecordByAddr(prAdapter,
                                        prJoinInfo->prBssDesc->aucBSSID);
    if (prStaRec) {

        /*                                      */
        prStaRec->ucStaState = STA_STATE_1;
    }
#if DBG
    else {
        ASSERT(0); /*                                                                                 */
    }
#endif /*     */

    //                        
    joinFsmSteps(prAdapter, JOIN_STATE_IDLE);

    //                                                                 
    /*                  
                                                        
     */
    if (prAdapter->eConnectionState == MEDIA_STATE_CONNECTED) {
        joinAdoptParametersFromCurrentBss(prAdapter);
    }

    return;
} /*                               */
#endif


/*                                                                  */
#if 0
/*                                                                            */
/* 
                                                                
 
                                                             
 
                                                            
*/
/*                                                                            */
WLAN_STATUS
joinFsmRunEventJoinTimeOut (
    IN P_ADAPTER_T  prAdapter
    )
{
    P_JOIN_INFO_T prJoinInfo;
    P_STA_RECORD_T prStaRec;

    DEBUGFUNC("joinFsmRunEventJoinTimeOut");


    ASSERT(prAdapter);
    prJoinInfo = &prAdapter->rJoinInfo;

    DBGLOG(JOIN, EVENT, ("JOIN EVENT: JOIN TIMEOUT\n"));

    /*                                                      */
    prStaRec = staRecGetStaRecordByAddr(prAdapter,
                                        prJoinInfo->prBssDesc->aucBSSID);

    /*                                                       */
    ASSERT(prStaRec);

    /*                                                  */
    prStaRec->u2StatusCode = STATUS_CODE_JOIN_TIMEOUT;

    /*                        */
    prStaRec->ucJoinFailureCount++;

    /*                                       */
    prJoinInfo->ucTxAuthAssocRetryCount = 0;

    /*                                  */
    ARB_CANCEL_TIMER(prAdapter,
                     prJoinInfo->rTxRequestTimer);

    ARB_CANCEL_TIMER(prAdapter,
                     prJoinInfo->rRxResponseTimer);

    /*                                                  */
    if (prAdapter->eConnectionState == MEDIA_STATE_CONNECTED) {
        joinAdoptParametersFromCurrentBss(prAdapter);
    }

    /*                   */
    joinFsmSteps(prAdapter, JOIN_STATE_IDLE);

    return WLAN_STATUS_FAILURE;

} /*                                     */

/*                                                                            */
/* 
                                                               
 
                                                             
 
                
*/
/*                                                                            */
VOID
joinAdoptParametersFromPeerBss (
    IN P_ADAPTER_T prAdapter
    )
{
    P_JOIN_INFO_T prJoinInfo;
    P_BSS_DESC_T prBssDesc;

    DEBUGFUNC("joinAdoptParametersFromPeerBss");


    ASSERT(prAdapter);
    prJoinInfo = &prAdapter->rJoinInfo;
    prBssDesc = prJoinInfo->prBssDesc;

    //                              
    prAdapter->eCurrentPhyType = prBssDesc->ePhyType;

    DBGLOG(JOIN, INFO, ("Target BSS[%s]'s PhyType = %s\n",
        prBssDesc->aucSSID, (prBssDesc->ePhyType == PHY_TYPE_ERP_INDEX) ? "ERP" : "HR_DSSS"));


    //                                             
    DBGLOG(JOIN, INFO, ("Target BSS's Channel = %d, Band = %d\n",
        prBssDesc->ucChannelNum, prBssDesc->eBand));

    nicSwitchChannel(prAdapter,
                     prBssDesc->eBand,
                     prBssDesc->ucChannelNum,
                     10);

    prJoinInfo->fgIsParameterAdopted = TRUE;

    return;
} /*                                         */


/*                                                                            */
/* 
                                                                             
 
                                                             
 
                
*/
/*                                                                            */
VOID
joinAdoptParametersFromCurrentBss (
    IN P_ADAPTER_T prAdapter
    )
{
    //                                                 
    P_BSS_INFO_T prBssInfo;


    ASSERT(prAdapter);
    prBssInfo = &prAdapter->rBssInfo;

    //                                 
    prAdapter->eCurrentPhyType = prBssInfo->ePhyType;

    //                                                
    DBGLOG(JOIN, INFO, ("Current BSS's Channel = %d, Band = %d\n",
        prBssInfo->ucChnl, prBssInfo->eBand));

    nicSwitchChannel(prAdapter,
                     prBssInfo->eBand,
                     prBssInfo->ucChnl,
                     10);
    return;
} /*                                            */


/*                                                                            */
/* 
                                                                                  
                                         
 
                                                             
 
                
*/
/*                                                                            */
VOID
joinComplete (
    IN P_ADAPTER_T prAdapter
    )
{
    P_JOIN_INFO_T prJoinInfo;
    P_BSS_DESC_T prBssDesc;
    P_PEER_BSS_INFO_T prPeerBssInfo;
    P_BSS_INFO_T prBssInfo;
    P_CONNECTION_SETTINGS_T prConnSettings;
    P_STA_RECORD_T prStaRec;
    P_TX_CTRL_T prTxCtrl;
#if CFG_SUPPORT_802_11D
    P_IE_COUNTRY_T          prIECountry;
#endif

    DEBUGFUNC("joinComplete");


    ASSERT(prAdapter);
    prJoinInfo = &prAdapter->rJoinInfo;
    prBssDesc = prJoinInfo->prBssDesc;
    prPeerBssInfo = &prAdapter->rPeerBssInfo;
    prBssInfo = &prAdapter->rBssInfo;
    prConnSettings = &prAdapter->rConnSettings;
    prTxCtrl = &prAdapter->rTxCtrl;

//                                                       
    /*                                               */
    scanRemoveConnectionFlagOfBssDescByBssid(prAdapter, prBssInfo->aucBSSID);

    prBssDesc->fgIsConnected = TRUE; /*                   */

    if (prBssDesc->fgIsHiddenSSID) {
        /*                                                                            
                                              
         */
        COPY_SSID(prBssDesc->aucSSID,
                  prBssDesc->ucSSIDLen,
                  prAdapter->rConnSettings.aucSSID,
                  prAdapter->rConnSettings.ucSSIDLen);

        if (prBssDesc->ucSSIDLen) {
            prBssDesc->fgIsHiddenSSID = FALSE;
        }
#if DBG
        else {
            ASSERT(0);
        }
#endif /*     */

        DBGLOG(JOIN, INFO, ("Hidden SSID! - Update SSID : %s\n", prBssDesc->aucSSID));
    }


//                                       
    //                
    prBssInfo->ePhyType = prBssDesc->ePhyType;

    //                
    prBssInfo->eBSSType = BSS_TYPE_INFRASTRUCTURE;

    //             
    COPY_MAC_ADDR(prBssInfo->aucBSSID, prBssDesc->aucBSSID);

    DBGLOG(JOIN, INFO, ("JOIN to BSSID: ["MACSTR"]\n", MAC2STR(prBssDesc->aucBSSID)));


    //            
    COPY_SSID(prBssInfo->aucSSID,
              prBssInfo->ucSSIDLen,
              prBssDesc->aucSSID,
              prBssDesc->ucSSIDLen);

    //                                   
    prBssInfo->eBand = prBssDesc->eBand;
    prBssInfo->ucChnl = prBssDesc->ucChannelNum;

    //                            
    secFsmRunEventStart(prAdapter);
    prBssInfo->u4RsnSelectedPairwiseCipher = prBssDesc->u4RsnSelectedPairwiseCipher;
    prBssInfo->u4RsnSelectedGroupCipher = prBssDesc->u4RsnSelectedGroupCipher;
    prBssInfo->u4RsnSelectedAKMSuite = prBssDesc->u4RsnSelectedAKMSuite;

    if (secRsnKeyHandshakeEnabled()) {
        prBssInfo->fgIsWPAorWPA2Enabled = TRUE;
    }
    else {
        prBssInfo->fgIsWPAorWPA2Enabled = FALSE;
    }

    //                        
    prBssInfo->u2BeaconInterval = prBssDesc->u2BeaconInterval;

    //                    
    prBssInfo->ucDtimPeriod = prBssDesc->ucDTIMPeriod;

    //                       
    if ((prBssInfo->ePhyType == PHY_TYPE_ERP_INDEX) && //                               
        (prBssDesc->fgIsERPPresent)) {

        prBssInfo->fgIsERPPresent = TRUE;
        prBssInfo->ucERP = prBssDesc->ucERP; /*                              */
    }
    else { /*                                                                                      */
        prBssInfo->fgIsERPPresent = FALSE;
        prBssInfo->ucERP = 0;
    }

#if CFG_SUPPORT_802_11D
    //                                                
    if (prConnSettings->fgMultiDomainCapabilityEnabled) {
        DOMAIN_INFO_ENTRY   rDomainInfo;
        if (domainGetDomainInfoByScanResult(prAdapter, &rDomainInfo)) {
            if (prBssDesc->prIECountry) {
                prIECountry = prBssDesc->prIECountry;

                domainParseCountryInfoElem(prIECountry, &prBssInfo->rDomainInfo);

                /*                                      */
                prBssInfo->fgIsCountryInfoPresent = TRUE;
                nicSetupOpChnlList(prAdapter, prBssInfo->rDomainInfo.u2CountryCode, FALSE);
            } else {
                /*                                         */
                prBssInfo->fgIsCountryInfoPresent = TRUE;
                nicSetupOpChnlList(prAdapter, rDomainInfo.u2CountryCode, FALSE);
            }
        }
    }
#endif

    //                                         
    prBssInfo->rRcpi = prBssDesc->rRcpi;
    prBssInfo->rRssi = RCPI_TO_dBm(prBssInfo->rRcpi);
    GET_CURRENT_SYSTIME(&prBssInfo->rRssiLastUpdateTime);

    //                                             
    prBssInfo->u2CapInfo = prBssDesc->u2CapInfo;

    DBGLOG(JOIN, INFO, ("prBssInfo-> fgIsERPPresent = %d, ucERP = %02x, rRcpi = %d, rRssi = %ld\n",
        prBssInfo->fgIsERPPresent, prBssInfo->ucERP, prBssInfo->rRcpi, prBssInfo->rRssi));


//                                                            
    //                      
    prBssInfo->u2AssocId = prPeerBssInfo->u2AssocId;

    //                      
    if (prAdapter->fgIsEnableWMM &&
        (prPeerBssInfo->rWmmInfo.ucWmmFlag & WMM_FLAG_SUPPORT_WMM)) {

        prBssInfo->fgIsWmmAssoc = TRUE;
        prTxCtrl->rTxQForVoipAccess = TXQ_AC3;

        qosWmmInfoInit(&prBssInfo->rWmmInfo, (prBssInfo->ePhyType == PHY_TYPE_HR_DSSS_INDEX) ? TRUE : FALSE);

        if (prPeerBssInfo->rWmmInfo.ucWmmFlag & WMM_FLAG_AC_PARAM_PRESENT) {
            kalMemCopy(&prBssInfo->rWmmInfo,
                       &prPeerBssInfo->rWmmInfo,
                       sizeof(WMM_INFO_T));
        }
        else {
            kalMemCopy(&prBssInfo->rWmmInfo,
                       &prPeerBssInfo->rWmmInfo,
                       sizeof(WMM_INFO_T) - sizeof(prPeerBssInfo->rWmmInfo.arWmmAcParams));
        }
    }
    else {
        prBssInfo->fgIsWmmAssoc = FALSE;
        prTxCtrl->rTxQForVoipAccess = TXQ_AC1;

        kalMemZero(&prBssInfo->rWmmInfo, sizeof(WMM_INFO_T));
    }


    //                                                 
    prBssInfo->u2OperationalRateSet = prPeerBssInfo->u2OperationalRateSet;
    prBssInfo->u2BSSBasicRateSet = prPeerBssInfo->u2BSSBasicRateSet;


    //                      
    if (prBssInfo->fgIsERPPresent) {

        /*                                     
                                
                                                                                             
                                                                                                                    
                                                                       
                                                                                                                    
                                                                       
                                                                                  
                                                                                   
                                                                                                                             
                                                                                               
         */
        if ((prPeerBssInfo->fgIsShortPreambleAllowed) &&
            ((prConnSettings->ePreambleType == PREAMBLE_TYPE_SHORT) || /*                                      */
             ((prConnSettings->ePreambleType == PREAMBLE_TYPE_AUTO) &&
              (prBssDesc->u2CapInfo & CAP_INFO_SHORT_PREAMBLE)))) {

            prBssInfo->fgIsShortPreambleAllowed = TRUE;

            if (prBssInfo->ucERP & ERP_INFO_BARKER_PREAMBLE_MODE) {
                prBssInfo->fgUseShortPreamble = FALSE;
            }
            else {
                prBssInfo->fgUseShortPreamble = TRUE;
            }
        }
        else {
            prBssInfo->fgIsShortPreambleAllowed = FALSE;
            prBssInfo->fgUseShortPreamble = FALSE;
        }
    }
    else {
        /*                                     
                                
                                                                             
                                                       
                                                       
                                                      
                                                      
                                                 
         */

        prBssInfo->fgIsShortPreambleAllowed = prBssInfo->fgUseShortPreamble =
            prPeerBssInfo->fgIsShortPreambleAllowed;
    }

    DBGLOG(JOIN, INFO, ("prBssInfo->fgIsShortPreambleAllowed = %d, prBssInfo->fgUseShortPreamble = %d\n",
        prBssInfo->fgIsShortPreambleAllowed, prBssInfo->fgUseShortPreamble));


    //                       
    prBssInfo->fgUseShortSlotTime =
        prPeerBssInfo->fgUseShortSlotTime; /*                            */

    DBGLOG(JOIN, INFO, ("prBssInfo->fgUseShortSlotTime = %d\n",
        prBssInfo->fgUseShortSlotTime));

    nicSetSlotTime(prAdapter,
                   prBssInfo->ePhyType,
                   ((prConnSettings->fgIsShortSlotTimeOptionEnable &&
                     prBssInfo->fgUseShortSlotTime) ? TRUE : FALSE));


    //                                        
    bssUpdateTxRateForControlFrame(prAdapter);


    //                                                                                      
    //                                                                                    
    {

        if (prJoinInfo->ucCurrAuthAlgNum == AUTH_ALGORITHM_NUM_OPEN_SYSTEM) {
            prJoinInfo->ucRoamingAuthTypes |= AUTH_TYPE_OPEN_SYSTEM;
        }
        else if (prJoinInfo->ucCurrAuthAlgNum == AUTH_ALGORITHM_NUM_SHARED_KEY) {
            prJoinInfo->ucRoamingAuthTypes |= AUTH_TYPE_SHARED_KEY;
        }

        prBssInfo->ucRoamingAuthTypes = prJoinInfo->ucRoamingAuthTypes;


        /*                                                                        
                                  
         */
        SET_EXPIRATION_TIME(prBssInfo->rRoamingStableExpirationTime,
            SEC_TO_SYSTIME(ROAMING_STABLE_TIMEOUT_SEC));
    }


    //                                                       
#if CFG_TX_FRAGMENT
    txFragInfoUpdate(prAdapter);
#endif /*                 */


//                         
    /*                                  */
    prStaRec = staRecGetStaRecordByAddr(prAdapter,
                                        prBssDesc->aucBSSID);

    if (prStaRec) {
        UINT_16 u2OperationalRateSet, u2DesiredRateSet;

        //                        
        u2OperationalRateSet = (rPhyAttributes[prBssInfo->ePhyType].u2SupportedRateSet &
                                prBssInfo->u2OperationalRateSet);

        u2DesiredRateSet = (u2OperationalRateSet & prConnSettings->u2DesiredRateSet);
        if (u2DesiredRateSet) {
            prStaRec->u2DesiredRateSet = u2DesiredRateSet;
        }
        else {
            /*                                                                                   */
            prStaRec->u2DesiredRateSet = u2OperationalRateSet;
        }

        /*                                                 */
        if (!rateGetBestInitialRateIndex(prStaRec->u2DesiredRateSet,
                                         prStaRec->rRcpi,
                                         &prStaRec->ucCurrRate1Index)) {

            if (!rateGetLowestRateIndexFromRateSet(prStaRec->u2DesiredRateSet,
                                                   &prStaRec->ucCurrRate1Index)) {
                ASSERT(0);
            }
        }

        DBGLOG(JOIN, INFO, ("prStaRec->ucCurrRate1Index = %d\n",
            prStaRec->ucCurrRate1Index));

        //                     
        prStaRec->fgIsShortPreambleOptionEnable =
            prBssInfo->fgUseShortPreamble;

        //                
        prStaRec->fgIsQoS = prBssInfo->fgIsWmmAssoc;
    }
#if DBG
    else {
        ASSERT(0);
    }
#endif /*     */


//                
    //                                     
    nicSetupBSS(prAdapter, prBssInfo);

    //                          
    if (nicSetHwBySta(prAdapter, prStaRec) == FALSE) {
        ASSERT(FALSE);
    }

    //                                       
#if CFG_TX_FRAGMENT
    if (prConnSettings->fgIsEnableTxAutoFragmentForBT) {
        txRateSetInitForBT(prAdapter, prStaRec);
    }
#endif /*                 */

    //                                               
    if (prBssInfo->fgIsWmmAssoc) {

#if CFG_TX_AGGREGATE_HW_FIFO
        nicTxAggregateTXQ(prAdapter, FALSE);
#endif /*                          */

        qosUpdateWMMParametersAndAssignAllowedACI(prAdapter, &prBssInfo->rWmmInfo);
    }
    else {

#if CFG_TX_AGGREGATE_HW_FIFO
        nicTxAggregateTXQ(prAdapter, TRUE);
#endif /*                          */

        nicTxNonQoSAssignDefaultAdmittedTXQ(prAdapter);

        nicTxNonQoSUpdateTXQParameters(prAdapter,
                                       prBssInfo->ePhyType);
    }

#if CFG_TX_STOP_WRITE_TX_FIFO_UNTIL_JOIN
    {
        prTxCtrl->fgBlockTxDuringJoin = FALSE;

    #if !CFG_TX_AGGREGATE_HW_FIFO /*                                         */
        nicTxFlushStopQueues(prAdapter, (UINT_8)TXQ_DATA_MASK, (UINT_8)NULL);
    #endif /*                          */

        nicTxRetransmitOfSendWaitQue(prAdapter);

        if (prTxCtrl->fgIsPacketInOsSendQueue) {
            nicTxRetransmitOfOsSendQue(prAdapter);
        }

    #if CFG_SDIO_TX_ENHANCE
        halTxLeftClusteredMpdu(prAdapter);
    #endif /*                     */

    }
#endif /*                                      */


//                            
    prAdapter->eConnectionState = MEDIA_STATE_CONNECTED;
    prAdapter->eConnectionStateIndicated = MEDIA_STATE_CONNECTED;

    if (prJoinInfo->fgIsReAssoc) {
        prAdapter->fgBypassPortCtrlForRoaming = TRUE;
    }
    else {
        prAdapter->fgBypassPortCtrlForRoaming = FALSE;
    }

    kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
        WLAN_STATUS_MEDIA_CONNECT,
        (PVOID)NULL,
        0);

    return;
} /*                       */
#endif

