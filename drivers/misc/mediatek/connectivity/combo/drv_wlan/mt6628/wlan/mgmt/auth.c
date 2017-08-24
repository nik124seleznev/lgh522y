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
APPEND_IE_ENTRY_T txAuthIETable[] = {
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_CHALLENGE_TEXT), authAddIEChallengeText}
};

HANDLE_IE_ENTRY_T rxAuthIETable[] = {
	{ELEM_ID_CHALLENGE_TEXT, authHandleIEChallengeText}
};

/*                                                                              
                                                   
                                                                                
*/

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
__KAL_INLINE__ VOID
authComposeAuthFrameHeaderAndFF(IN PUINT_8 pucBuffer,
				IN UINT_8 aucPeerMACAddress[],
				IN UINT_8 aucMACAddress[],
				IN UINT_16 u2AuthAlgNum,
				IN UINT_16 u2TransactionSeqNum, IN UINT_16 u2StatusCode)
{
	P_WLAN_AUTH_FRAME_T prAuthFrame;
	UINT_16 u2FrameCtrl;


	ASSERT(pucBuffer);
	ASSERT(aucPeerMACAddress);
	ASSERT(aucMACAddress);

	prAuthFrame = (P_WLAN_AUTH_FRAME_T) pucBuffer;

	/*                                                             */
	/*                               */
	u2FrameCtrl = MAC_FRAME_AUTH;

	/*                                                                  
                                    
  */
	if ((u2AuthAlgNum == AUTH_ALGORITHM_NUM_SHARED_KEY) &&
	    (u2TransactionSeqNum == AUTH_TRANSACTION_SEQ_3)) {

		u2FrameCtrl |= MASK_FC_PROTECTED_FRAME;	/*                                                      */
	}
	/*                                                            */
	prAuthFrame->u2FrameCtrl = u2FrameCtrl;	/*                                */

	/*                                      */
	COPY_MAC_ADDR(prAuthFrame->aucDestAddr, aucPeerMACAddress);

	/*                                         */
	COPY_MAC_ADDR(prAuthFrame->aucSrcAddr, aucMACAddress);

	switch (u2TransactionSeqNum) {
	case AUTH_TRANSACTION_SEQ_1:
	case AUTH_TRANSACTION_SEQ_3:

		/*                                         */
		COPY_MAC_ADDR(prAuthFrame->aucBSSID, aucPeerMACAddress);
		break;

	case AUTH_TRANSACTION_SEQ_2:
	case AUTH_TRANSACTION_SEQ_4:

		/*                                          */
		COPY_MAC_ADDR(prAuthFrame->aucBSSID, aucMACAddress);
		break;

	default:
		ASSERT(0);
	}

	/*                              */
	prAuthFrame->u2SeqCtrl = 0;


	/*                                                                              */
	/*                                                 */
	/*                                                              */
	prAuthFrame->u2AuthAlgNum = u2AuthAlgNum;	/*                                */

	/*                                                            */
	/*                                                                         */
	prAuthFrame->u2AuthTransSeqNo = u2TransactionSeqNum;	/*                                */

	/*                             */
	/*                                                              */
	prAuthFrame->u2StatusCode = u2StatusCode;	/*                                */

	return;
}				/*                                          */


/*                                                                            */
/* 
                                                                                
 
                                                                
 
                
*/
/*                                                                            */
VOID authAddIEChallengeText(IN P_ADAPTER_T prAdapter, IN OUT P_MSDU_INFO_T prMsduInfo)
{
	P_WLAN_AUTH_FRAME_T prAuthFrame;
	P_STA_RECORD_T prStaRec;
	UINT_16 u2TransactionSeqNum;


	ASSERT(prMsduInfo);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	if (!prStaRec) {
		return;
	}

	ASSERT(prStaRec);

	/*                                                                     */
	prAuthFrame = (P_WLAN_AUTH_FRAME_T) prMsduInfo->prPacket;

	WLAN_GET_FIELD_16(&prAuthFrame->u2AuthTransSeqNo, &u2TransactionSeqNum)

	    /*                                        */
	    if ((u2TransactionSeqNum == AUTH_TRANSACTION_SEQ_3) &&
		(prStaRec->ucAuthAlgNum == AUTH_ALGORITHM_NUM_SHARED_KEY) &&
		(prStaRec->prChallengeText != NULL)) {

		COPY_IE(((UINT_32) (prMsduInfo->prPacket) + prMsduInfo->u2FrameLength),
			(prStaRec->prChallengeText));

		prMsduInfo->u2FrameLength += IE_SIZE(prStaRec->prChallengeText);
	}

	return;

}				/*                                 */


#if !CFG_SUPPORT_AAA
/*                                                                            */
/* 
                                                          
 
                                                               
                                                               
 
                                                                          
                                                                    
*/
/*                                                                            */
WLAN_STATUS
authSendAuthFrame(IN P_ADAPTER_T prAdapter,
		  IN P_STA_RECORD_T prStaRec, IN UINT_16 u2TransactionSeqNum)
{
	P_MSDU_INFO_T prMsduInfo;
	P_BSS_INFO_T prBssInfo;
	UINT_16 u2EstimatedFrameLen;
	UINT_16 u2EstimatedExtraIELen;
	UINT_16 u2PayloadLen;
	UINT_32 i;


	DBGLOG(SAA, LOUD, ("Send Auth Frame\n"));

	ASSERT(prStaRec);

	/*                                                      */
	/*                                                       */
	u2EstimatedFrameLen = (MAC_TX_RESERVED_FIELD +
			       WLAN_MAC_MGMT_HEADER_LEN +
			       AUTH_ALGORITHM_NUM_FIELD_LEN +
			       AUTH_TRANSACTION_SEQENCE_NUM_FIELD_LEN + STATUS_CODE_FIELD_LEN);

	/*                   */
	u2EstimatedExtraIELen = 0;

	for (i = 0; i < sizeof(txAuthIETable) / sizeof(APPEND_IE_ENTRY_T); i++) {
		u2EstimatedExtraIELen += txAuthIETable[i].u2EstimatedIELen;
	}

	u2EstimatedFrameLen += u2EstimatedExtraIELen;

	/*                        */
	if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
		DBGLOG(SAA, WARN, ("No PKT_INFO_T for sending Auth Frame.\n"));
		return WLAN_STATUS_RESOURCES;
	}
	/*                                                                                    */
	ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);
	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

	/*                                      */
	authComposeAuthFrameHeaderAndFF((PUINT_8)
					((UINT_32) (prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD),
					prStaRec->aucMacAddr, prBssInfo->aucOwnMacAddr,
					prStaRec->ucAuthAlgNum, u2TransactionSeqNum,
					STATUS_CODE_RESERVED);

	u2PayloadLen = (AUTH_ALGORITHM_NUM_FIELD_LEN +
			AUTH_TRANSACTION_SEQENCE_NUM_FIELD_LEN + STATUS_CODE_FIELD_LEN);

	/*                                         */
	prMsduInfo->eSrc = TX_PACKET_MGMT;
	prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
	prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
	prMsduInfo->ucNetworkType = prStaRec->ucNetTypeIndex;
	prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
	prMsduInfo->fgIs802_1x = FALSE;
	prMsduInfo->fgIs802_11 = TRUE;
	prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen;
	prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
	prMsduInfo->pfTxDoneHandler = saaFsmRunEventTxDone;
	prMsduInfo->fgIsBasicRate = TRUE;

	/*                                  */
	for (i = 0; i < sizeof(txAuthIETable) / sizeof(APPEND_IE_ENTRY_T); i++) {
		if (txAuthIETable[i].pfnAppendIE) {
			txAuthIETable[i].pfnAppendIE(prAdapter, prMsduInfo);
		}
	}

	/*                                                                      */

	/*                                                      */
	nicTxEnqueueMsdu(prAdapter, prMsduInfo);

	return WLAN_STATUS_SUCCESS;
}				/*                            */

#else

/*                                                                            */
/* 
                                                          
 
                                                               
                                                               
 
                                                                          
                                                                    
*/
/*                                                                            */
WLAN_STATUS
authSendAuthFrame(IN P_ADAPTER_T prAdapter,
		  IN P_STA_RECORD_T prStaRec,
		  IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex,
		  IN P_SW_RFB_T prFalseAuthSwRfb,
		  IN UINT_16 u2TransactionSeqNum, IN UINT_16 u2StatusCode)
{
	PUINT_8 pucReceiveAddr;
	PUINT_8 pucTransmitAddr;
	P_MSDU_INFO_T prMsduInfo;
	P_BSS_INFO_T prBssInfo;
	/*                         */
	/*                                                                   */
	PFN_TX_DONE_HANDLER pfTxDoneHandler = (PFN_TX_DONE_HANDLER) NULL;
	UINT_16 u2EstimatedFrameLen;
	UINT_16 u2EstimatedExtraIELen;
	UINT_16 u2PayloadLen;
	UINT_16 ucAuthAlgNum;
	UINT_32 i;


	DBGLOG(SAA, LOUD, ("Send Auth Frame %d, Status Code = %d\n",
			   u2TransactionSeqNum, u2StatusCode));

	/*                                                      */
	/*                                                       */
	u2EstimatedFrameLen = (MAC_TX_RESERVED_FIELD +
			       WLAN_MAC_MGMT_HEADER_LEN +
			       AUTH_ALGORITHM_NUM_FIELD_LEN +
			       AUTH_TRANSACTION_SEQENCE_NUM_FIELD_LEN + STATUS_CODE_FIELD_LEN);

	/*                   */
	u2EstimatedExtraIELen = 0;

	for (i = 0; i < sizeof(txAuthIETable) / sizeof(APPEND_IE_ENTRY_T); i++) {
		u2EstimatedExtraIELen += txAuthIETable[i].u2EstimatedIELen;
	}

	u2EstimatedFrameLen += u2EstimatedExtraIELen;

	/*                        */
	if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
		DBGLOG(SAA, WARN, ("No PKT_INFO_T for sending Auth Frame.\n"));
		return WLAN_STATUS_RESOURCES;
	}
	/*                                                                                    */
	if (prStaRec) {
		ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

		prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

		pucTransmitAddr = prBssInfo->aucOwnMacAddr;

		pucReceiveAddr = prStaRec->aucMacAddr;

		ucAuthAlgNum = prStaRec->ucAuthAlgNum;

		switch (u2TransactionSeqNum) {
		case AUTH_TRANSACTION_SEQ_1:
		case AUTH_TRANSACTION_SEQ_3:
			pfTxDoneHandler = saaFsmRunEventTxDone;
			break;

		case AUTH_TRANSACTION_SEQ_2:
		case AUTH_TRANSACTION_SEQ_4:
			pfTxDoneHandler = aaaFsmRunEventTxDone;
			break;
		}

	} else {		/*                       */
		P_WLAN_AUTH_FRAME_T prFalseAuthFrame;


		ASSERT(prFalseAuthSwRfb);
		prFalseAuthFrame = (P_WLAN_AUTH_FRAME_T) prFalseAuthSwRfb->pvHeader;

		ASSERT(u2StatusCode != STATUS_CODE_SUCCESSFUL);

		pucTransmitAddr = prFalseAuthFrame->aucDestAddr;

		pucReceiveAddr = prFalseAuthFrame->aucSrcAddr;

		ucAuthAlgNum = prFalseAuthFrame->u2AuthAlgNum;

		u2TransactionSeqNum = (prFalseAuthFrame->u2AuthTransSeqNo + 1);
	}

	/*                                      */
	authComposeAuthFrameHeaderAndFF((PUINT_8)
					((UINT_32) (prMsduInfo->prPacket) + MAC_TX_RESERVED_FIELD),
					pucReceiveAddr, pucTransmitAddr, ucAuthAlgNum,
					u2TransactionSeqNum, u2StatusCode);

	u2PayloadLen = (AUTH_ALGORITHM_NUM_FIELD_LEN +
			AUTH_TRANSACTION_SEQENCE_NUM_FIELD_LEN + STATUS_CODE_FIELD_LEN);

	/*                                         */
	prMsduInfo->eSrc = TX_PACKET_MGMT;
	prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
	if (prStaRec) {
		prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
	} else {
		prMsduInfo->ucStaRecIndex = STA_REC_INDEX_NOT_FOUND;	/*                  */
	}
	prMsduInfo->ucNetworkType = (UINT_8) eNetTypeIndex;
	prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
	prMsduInfo->fgIs802_1x = FALSE;
	prMsduInfo->fgIs802_11 = TRUE;
	prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen;
	prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
	prMsduInfo->pfTxDoneHandler = pfTxDoneHandler;
	prMsduInfo->fgIsBasicRate = TRUE;

	/*                                  */
	for (i = 0; i < sizeof(txAuthIETable) / sizeof(APPEND_IE_ENTRY_T); i++) {
		if (txAuthIETable[i].pfnAppendIE) {
			txAuthIETable[i].pfnAppendIE(prAdapter, prMsduInfo);
		}
	}

	/*                                                                      */

	/*                                                      */
	nicTxEnqueueMsdu(prAdapter, prMsduInfo);

	return WLAN_STATUS_SUCCESS;
}				/*                            */

#endif				/*                 */


/*                                                                            */
/* 
                                                                                        
                  
 
                                                          
                                                               
 
                                                                                        
                                                                   
*/
/*                                                                            */
WLAN_STATUS
authCheckTxAuthFrame(IN P_ADAPTER_T prAdapter,
		     IN P_MSDU_INFO_T prMsduInfo, IN UINT_16 u2TransactionSeqNum)
{
	P_WLAN_AUTH_FRAME_T prAuthFrame;
	P_STA_RECORD_T prStaRec;
	UINT_16 u2TxFrameCtrl;
	UINT_16 u2TxAuthAlgNum;
	UINT_16 u2TxTransactionSeqNum;


	ASSERT(prMsduInfo);

	prAuthFrame = (P_WLAN_AUTH_FRAME_T) (prMsduInfo->prPacket);
	ASSERT(prAuthFrame);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
	ASSERT(prStaRec);

	if (!prStaRec) {
		return WLAN_STATUS_INVALID_PACKET;
	}
	/*                                                              */
	u2TxFrameCtrl = prAuthFrame->u2FrameCtrl;	/*                                */
	u2TxFrameCtrl &= MASK_FRAME_TYPE;
	if (u2TxFrameCtrl != MAC_FRAME_AUTH) {
		return WLAN_STATUS_FAILURE;
	}
	/*                                                                */
	u2TxAuthAlgNum = prAuthFrame->u2AuthAlgNum;	/*                                */
	if (u2TxAuthAlgNum != (UINT_16) (prStaRec->ucAuthAlgNum)) {
		return WLAN_STATUS_FAILURE;
	}
	/*                                                                           */
	u2TxTransactionSeqNum = prAuthFrame->u2AuthTransSeqNo;	/*                                */
	if (u2TxTransactionSeqNum != u2TransactionSeqNum) {
		return WLAN_STATUS_FAILURE;
	}

	return WLAN_STATUS_SUCCESS;

}				/*                               */


/*                                                                            */
/* 
                                                                                
                                                                            
 
                                                                  
 
                                                                       
*/
/*                                                                            */
WLAN_STATUS authCheckRxAuthFrameTransSeq(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_WLAN_AUTH_FRAME_T prAuthFrame;
	UINT_16 u2RxTransactionSeqNum;


	ASSERT(prSwRfb);

	/*                                        */
	prAuthFrame = (P_WLAN_AUTH_FRAME_T) prSwRfb->pvHeader;

	/*                                                 */
	if ((prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) < (AUTH_ALGORITHM_NUM_FIELD_LEN +
							     AUTH_TRANSACTION_SEQENCE_NUM_FIELD_LEN
							     + STATUS_CODE_FIELD_LEN)) {
		ASSERT(0);
		return WLAN_STATUS_SUCCESS;
	}
	/*                                                            */
	/*                                                                            */
	u2RxTransactionSeqNum = prAuthFrame->u2AuthTransSeqNo;	/*                                */

	switch (u2RxTransactionSeqNum) {
	case AUTH_TRANSACTION_SEQ_2:
	case AUTH_TRANSACTION_SEQ_4:
		saaFsmRunEventRxAuth(prAdapter, prSwRfb);
		break;

	case AUTH_TRANSACTION_SEQ_1:
	case AUTH_TRANSACTION_SEQ_3:
#if CFG_SUPPORT_AAA
		aaaFsmRunEventRxAuth(prAdapter, prSwRfb);
#endif				/*                 */
		break;

	default:
		DBGLOG(SAA, WARN,
		       ("Strange Authentication Packet: Auth Trans Seq No = %d, Error Status Code = %d\n",
			u2RxTransactionSeqNum, prAuthFrame->u2StatusCode));
		break;
	}

	return WLAN_STATUS_SUCCESS;

}				/*                                       */


/*                                                                            */
/* 
                                                                               
                             
 
                                                                     
                                                               
                                                                                         
 
                                                                                        
                                                                   
*/
/*                                                                            */
WLAN_STATUS
authCheckRxAuthFrameStatus(IN P_ADAPTER_T prAdapter,
			   IN P_SW_RFB_T prSwRfb,
			   IN UINT_16 u2TransactionSeqNum, OUT PUINT_16 pu2StatusCode)
{
	P_STA_RECORD_T prStaRec;
	P_WLAN_AUTH_FRAME_T prAuthFrame;
	UINT_16 u2RxAuthAlgNum;
	UINT_16 u2RxTransactionSeqNum;
	/*                                                           */


	ASSERT(prSwRfb);
	ASSERT(pu2StatusCode);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
	ASSERT(prStaRec);

	if (!prStaRec) {
		return WLAN_STATUS_INVALID_PACKET;
	}
	/*                                        */
	prAuthFrame = (P_WLAN_AUTH_FRAME_T) prSwRfb->pvHeader;

	/*                                                            */
	/*                                                                 */
	u2RxAuthAlgNum = prAuthFrame->u2AuthAlgNum;	/*                                */
	if (u2RxAuthAlgNum != (UINT_16) prStaRec->ucAuthAlgNum) {
		DBGLOG(SAA, LOUD, ("Discard Auth frame with auth type = %d, current = %d\n",
				   u2RxAuthAlgNum, prStaRec->ucAuthAlgNum));
		return WLAN_STATUS_FAILURE;
	}
	/*                                                                            */
	u2RxTransactionSeqNum = prAuthFrame->u2AuthTransSeqNo;	/*                                */
	if (u2RxTransactionSeqNum != u2TransactionSeqNum) {
		DBGLOG(SAA, LOUD, ("Discard Auth frame with Transaction Seq No = %d\n",
				   u2RxTransactionSeqNum));
		return WLAN_STATUS_FAILURE;
	}
	/*                           */
	/*                                                                 */
	/*                                  */
	*pu2StatusCode = prAuthFrame->u2StatusCode;	/*                                */

	return WLAN_STATUS_SUCCESS;

}				/*                                     */


/*                                                                            */
/* 
                                                                                      
 
                                                                     
                                                                  
 
                
*/
/*                                                                            */
VOID authHandleIEChallengeText(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb, P_IE_HDR_T prIEHdr)
{
	P_WLAN_AUTH_FRAME_T prAuthFrame;
	P_STA_RECORD_T prStaRec;
	UINT_16 u2TransactionSeqNum;


	ASSERT(prSwRfb);
	ASSERT(prIEHdr);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
	ASSERT(prStaRec);

	if (!prStaRec) {
		return;
	}

	/*                                                                     */
	prAuthFrame = (P_WLAN_AUTH_FRAME_T) prSwRfb->pvHeader;

	/*                                                                         */
	u2TransactionSeqNum = prAuthFrame->u2AuthTransSeqNo;	/*                                */

	/*                                        */
	if ((u2TransactionSeqNum == AUTH_TRANSACTION_SEQ_2) &&
	    (prStaRec->ucAuthAlgNum == AUTH_ALGORITHM_NUM_SHARED_KEY)) {

		/*                                    */
		if (prStaRec->prChallengeText) {
			ASSERT(0);
			cnmMemFree(prAdapter, prStaRec->prChallengeText);
			prStaRec->prChallengeText = (P_IE_CHALLENGE_TEXT_T) NULL;
		}

		if ((prStaRec->prChallengeText =
		     cnmMemAlloc(prAdapter, RAM_TYPE_MSG, IE_SIZE(prIEHdr))) == NULL) {
			return;
		}

		/*                                                                                */
		COPY_IE(prStaRec->prChallengeText, prIEHdr);
	}

	return;

}				/*                                 */


/*                                                                            */
/* 
                                                                                
 
                                                                 
 
                                                                   
*/
/*                                                                            */
WLAN_STATUS authProcessRxAuth2_Auth4Frame(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb)
{
	P_WLAN_AUTH_FRAME_T prAuthFrame;
	PUINT_8 pucIEsBuffer;
	UINT_16 u2IEsLen;
	UINT_16 u2Offset;
	UINT_8 ucIEID;
	UINT_32 i;


	ASSERT(prSwRfb);

	prAuthFrame = (P_WLAN_AUTH_FRAME_T) prSwRfb->pvHeader;

	pucIEsBuffer = &prAuthFrame->aucInfoElem[0];
	u2IEsLen = (prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) -
	    (AUTH_ALGORITHM_NUM_FIELD_LEN +
	     AUTH_TRANSACTION_SEQENCE_NUM_FIELD_LEN + STATUS_CODE_FIELD_LEN);

	IE_FOR_EACH(pucIEsBuffer, u2IEsLen, u2Offset) {
		ucIEID = IE_ID(pucIEsBuffer);

		for (i = 0; i < (sizeof(rxAuthIETable) / sizeof(HANDLE_IE_ENTRY_T)); i++) {

			if (ucIEID == rxAuthIETable[i].ucElemID) {
				rxAuthIETable[i].pfnHandleIE(prAdapter, prSwRfb,
							     (P_IE_HDR_T) pucIEsBuffer);
			}
		}
	}

	return WLAN_STATUS_SUCCESS;

}				/*                                        */


/*                                                                            */
/* 
                                                              
 
                                                                
                                                           
                                                          
                                               
 
                
*/
/*                                                                            */
__KAL_INLINE__ VOID
authComposeDeauthFrameHeaderAndFF(IN PUINT_8 pucBuffer,
				  IN UINT_8 aucPeerMACAddress[],
				  IN UINT_8 aucMACAddress[],
				  IN UINT_8 aucBssid[], IN UINT_16 u2ReasonCode)
{
	P_WLAN_DEAUTH_FRAME_T prDeauthFrame;
	UINT_16 u2FrameCtrl;

	ASSERT(pucBuffer);
	ASSERT(aucPeerMACAddress);
	ASSERT(aucMACAddress);
	ASSERT(aucBssid);

	prDeauthFrame = (P_WLAN_DEAUTH_FRAME_T) pucBuffer;

	/*                                                               */
	/*                               */
	u2FrameCtrl = MAC_FRAME_DEAUTH;

	/*                                                              */
	prDeauthFrame->u2FrameCtrl = u2FrameCtrl;	/*                                */

	/*                                      */
	COPY_MAC_ADDR(prDeauthFrame->aucDestAddr, aucPeerMACAddress);

	/*                                         */
	COPY_MAC_ADDR(prDeauthFrame->aucSrcAddr, aucMACAddress);

	/*                                         */
	COPY_MAC_ADDR(prDeauthFrame->aucBSSID, aucBssid);

	/*                                                                                    */
	prDeauthFrame->u2SeqCtrl = 0;

	/*                                                                              */
	/*                             */
	/*                                                                */
	prDeauthFrame->u2ReasonCode = u2ReasonCode;	/*                                */

	return;
}				/*                                            */



/*                                                                            */
/* 
                                                            
 
                                                           
                                                                             
                                                                           
                                                          
 
                                                                          
                                                                    
                                                                             
*/
/*                                                                            */
WLAN_STATUS
authSendDeauthFrame(IN P_ADAPTER_T prAdapter,
		    IN P_STA_RECORD_T prStaRec,
		    IN P_SW_RFB_T prClassErrSwRfb,
		    IN UINT_16 u2ReasonCode, IN PFN_TX_DONE_HANDLER pfTxDoneHandler)
{
	P_WLAN_MAC_HEADER_A4_T prWlanMacHeader = NULL;
	PUINT_8 pucReceiveAddr;
	PUINT_8 pucTransmitAddr;
	PUINT_8 pucBssid = NULL;

	ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex = NETWORK_TYPE_AIS_INDEX;
	P_MSDU_INFO_T prMsduInfo;
	UINT_16 u2EstimatedFrameLen;
	UINT_16 u2RxFrameCtrl;
	P_BSS_INFO_T prBssInfo;

	P_DEAUTH_INFO_T prDeauthInfo;
	OS_SYSTIME rCurrentTime;
	INT_32 i4NewEntryIndex, i;
	UINT_8 ucStaRecIdx = STA_REC_INDEX_NOT_FOUND;

#if CFG_ENABLE_WIFI_DIRECT
	UINT_8 aucBMC[] = BC_MAC_ADDR;
#endif

	/*                                                                                
         
  */
	/*                                        */
	if (prClassErrSwRfb) {
		BOOLEAN fgIsAbleToSendDeauth = FALSE;

		prWlanMacHeader = (P_WLAN_MAC_HEADER_A4_T) prClassErrSwRfb->pvHeader;

		/*                                                                   */
		u2RxFrameCtrl = prWlanMacHeader->u2FrameCtrl;	/*                                */

		/*                                                                            */
		if ((prWlanMacHeader->u2FrameCtrl & MASK_TO_DS_FROM_DS) == 0) {
			return WLAN_STATUS_FAILURE;
		}

		/*                                                   */
		for (i = NETWORK_TYPE_AIS_INDEX; i < NETWORK_TYPE_INDEX_NUM; i++) {
			prBssInfo = &(prAdapter->rWifiVar.arBssInfo[i]);

			if (IS_NET_ACTIVE(prAdapter, i) &&
			    (EQUAL_MAC_ADDR(prWlanMacHeader->aucAddr1, prBssInfo->aucOwnMacAddr))) {
				{
					fgIsAbleToSendDeauth = TRUE;
					eNetTypeIndex = (ENUM_NETWORK_TYPE_INDEX_T) i;
					break;
				}
			}
		}

		if (!fgIsAbleToSendDeauth) {
			return WLAN_STATUS_FAILURE;
		}

		pucReceiveAddr = prWlanMacHeader->aucAddr2;

	} else if (prStaRec) {

		pucReceiveAddr = prStaRec->aucMacAddr;
	} else {
#if CFG_ENABLE_WIFI_DIRECT
		pucReceiveAddr = aucBMC;
#else
		return WLAN_STATUS_FAILURE;
#endif
	}

	/*                                                                        */
	GET_CURRENT_SYSTIME(&rCurrentTime);

	i4NewEntryIndex = -1;
	for (i = 0; i < MAX_DEAUTH_INFO_COUNT; i++) {
		prDeauthInfo = &(prAdapter->rWifiVar.arDeauthInfo[i]);


		/*                                                               
                              
   */
		if (CHECK_FOR_TIMEOUT(rCurrentTime,
				      prDeauthInfo->rLastSendTime,
				      MSEC_TO_SYSTIME(MIN_DEAUTH_INTERVAL_MSEC))) {

			i4NewEntryIndex = i;
		} else if (EQUAL_MAC_ADDR(pucReceiveAddr, prDeauthInfo->aucRxAddr) &&
			   (!pfTxDoneHandler)) {

			return WLAN_STATUS_FAILURE;
		}
	}

	/*                           */
	if (i4NewEntryIndex > 0) {

		prDeauthInfo = &(prAdapter->rWifiVar.arDeauthInfo[i4NewEntryIndex]);

		COPY_MAC_ADDR(prDeauthInfo->aucRxAddr, pucReceiveAddr);
		prDeauthInfo->rLastSendTime = rCurrentTime;
	} else {
		/*                                                                 
                                          
   */
		DBGLOG(SAA, WARN, ("No unused DEAUTH_INFO_T !\n"));
	}

	/*                                                        */
	/*                                                                   */
	u2EstimatedFrameLen = (MAC_TX_RESERVED_FIELD +
			       WLAN_MAC_MGMT_HEADER_LEN + REASON_CODE_FIELD_LEN);

	/*                        */
	if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
		DBGLOG(SAA, WARN, ("No PKT_INFO_T for sending Deauth Request.\n"));
		return WLAN_STATUS_RESOURCES;
	}
	/*                                               */
	if (prClassErrSwRfb) {

		/*                                        */
		pucTransmitAddr = prWlanMacHeader->aucAddr1;

		switch (prWlanMacHeader->u2FrameCtrl & MASK_TO_DS_FROM_DS) {

		case MASK_FC_FROM_DS:
			/*                                           */
			pucBssid = prWlanMacHeader->aucAddr2;
			break;

		case MASK_FC_TO_DS:
			/*                                           */
			pucBssid = prWlanMacHeader->aucAddr1;
			break;

		case MASK_TO_DS_FROM_DS:
			/*                                                          
                                                   
    */
			pucBssid = prWlanMacHeader->aucAddr2;
			break;

			/*            */
		}

	} else if (prStaRec) {
		eNetTypeIndex = prStaRec->ucNetTypeIndex;

		prBssInfo = &(prAdapter->rWifiVar.arBssInfo[eNetTypeIndex]);

		pucTransmitAddr = prBssInfo->aucOwnMacAddr;

		pucBssid = prBssInfo->aucBSSID;
	}
#if CFG_ENABLE_WIFI_DIRECT
	else {
		if (prAdapter->fgIsP2PRegistered) {
			prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX]);

			ucStaRecIdx = STA_REC_INDEX_BMCAST;

			pucTransmitAddr = prBssInfo->aucOwnMacAddr;

			pucBssid = prBssInfo->aucBSSID;

			eNetTypeIndex = NETWORK_TYPE_P2P_INDEX;
		} else {
			return WLAN_STATUS_FAILURE;
		}
	}

#endif

	/*                                                                   */ */
	authComposeDeauthFrameHeaderAndFF((PUINT_8)
					  ((UINT_32) (prMsduInfo->prPacket) +
					   MAC_TX_RESERVED_FIELD), pucReceiveAddr, pucTransmitAddr,
					  pucBssid, u2ReasonCode);

#if CFG_SUPPORT_802_11W
	if (rsnCheckBipKeyInstalled(prAdapter, prStaRec)) {
		P_WLAN_DEAUTH_FRAME_T prDeauthFrame;

		prDeauthFrame =
		    (P_WLAN_DEAUTH_FRAME_T) (PUINT_8) ((UINT_32) (prMsduInfo->prPacket) +
						       MAC_TX_RESERVED_FIELD);

		prDeauthFrame->u2FrameCtrl |= MASK_FC_PROTECTED_FRAME;
		DBGLOG(TX, WARN, ("authSendDeauthFrame with protection\n"));
	}
#endif

	/*                                         */
	prMsduInfo->eSrc = TX_PACKET_MGMT;
	prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
	prMsduInfo->ucStaRecIndex = ((prStaRec == NULL) ? ucStaRecIdx : prStaRec->ucIndex);
	prMsduInfo->ucNetworkType = (UINT_8) eNetTypeIndex;
	prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
	prMsduInfo->fgIs802_1x = FALSE;
	prMsduInfo->fgIs802_11 = TRUE;
	prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + REASON_CODE_FIELD_LEN;
	prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
	prMsduInfo->pfTxDoneHandler = pfTxDoneHandler;
	prMsduInfo->fgIsBasicRate = TRUE;

	/*                                                       */
	nicTxEnqueueMsdu(prAdapter, prMsduInfo);

	return WLAN_STATUS_SUCCESS;
}				/*                              */



/*                                                                            */
/* 
                                                                                 
                                       
 
                                                                 
                                           
                                                                                       
 
                                                                                        
                                                                   
*/
/*                                                                            */
WLAN_STATUS
authProcessRxDeauthFrame(IN P_SW_RFB_T prSwRfb, IN UINT_8 aucBSSID[], OUT PUINT_16 pu2ReasonCode)
{
	P_WLAN_DEAUTH_FRAME_T prDeauthFrame;
	UINT_16 u2RxReasonCode;


	ASSERT(prSwRfb);
	ASSERT(aucBSSID);
	ASSERT(pu2ReasonCode);

	/*                                          */
	prDeauthFrame = (P_WLAN_DEAUTH_FRAME_T) prSwRfb->pvHeader;

	/*                                                   */
#if 0				/*                        */
	WLAN_GET_FIELD_16(&prDeauthFrame->u2FrameCtrl, &u2RxFrameCtrl)
	    u2RxFrameCtrl &= MASK_FRAME_TYPE;
	if (u2RxFrameCtrl != MAC_FRAME_DEAUTH) {
		return WLAN_STATUS_FAILURE;
	}
#endif

	if ((prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) < REASON_CODE_FIELD_LEN) {
		ASSERT(0);
		return WLAN_STATUS_FAILURE;
	}

	/*                                                        */
	if (UNEQUAL_MAC_ADDR(prDeauthFrame->aucBSSID, aucBSSID)) {
		DBGLOG(SAA, LOUD, ("Ignore Deauth Frame from other BSS [" MACSTR "]\n",
				   MAC2STR(prDeauthFrame->aucSrcAddr)));
		return WLAN_STATUS_FAILURE;
	}
	/*                                                              */
	WLAN_GET_FIELD_16(&prDeauthFrame->u2ReasonCode, &u2RxReasonCode);
	*pu2ReasonCode = u2RxReasonCode;

	return WLAN_STATUS_SUCCESS;

}				/*                                   */


/*                                                                            */
/* 
                                                                                
 
                                                                     
                                                         
                                                                                  
                                                                               
                                                       
 
                                                                   
                                                         
*/
/*                                                                            */
WLAN_STATUS
authProcessRxAuth1Frame(IN P_ADAPTER_T prAdapter,
			IN P_SW_RFB_T prSwRfb,
			IN UINT_8 aucExpectedBSSID[],
			IN UINT_16 u2ExpectedAuthAlgNum,
			IN UINT_16 u2ExpectedTransSeqNum, OUT PUINT_16 pu2ReturnStatusCode)
{
	P_WLAN_AUTH_FRAME_T prAuthFrame;
	UINT_16 u2ReturnStatusCode = STATUS_CODE_SUCCESSFUL;


	ASSERT(prSwRfb);
	ASSERT(aucExpectedBSSID);
	ASSERT(pu2ReturnStatusCode);

	/*                                        */
	prAuthFrame = (P_WLAN_AUTH_FRAME_T) prSwRfb->pvHeader;

	/*                       */
	if (UNEQUAL_MAC_ADDR(prAuthFrame->aucBSSID, aucExpectedBSSID)) {
		return WLAN_STATUS_FAILURE;	/*                        */
	}

	/*                                               */
	if (prAuthFrame->aucSrcAddr[0] & BIT(0)) {
		DBGLOG(P2P, WARN, ("Invalid STA MAC with MC/BC bit set: "MACSTR"\n",
			MAC2STR(prAuthFrame->aucSrcAddr)));
		return WLAN_STATUS_FAILURE;
	}

	/*                                                            */
	if (prAuthFrame->u2AuthAlgNum != u2ExpectedAuthAlgNum) {
		u2ReturnStatusCode = STATUS_CODE_AUTH_ALGORITHM_NOT_SUPPORTED;
	}

	if (prAuthFrame->u2AuthTransSeqNo != u2ExpectedTransSeqNum) {
		u2ReturnStatusCode = STATUS_CODE_AUTH_OUT_OF_SEQ;
	}

	*pu2ReturnStatusCode = u2ReturnStatusCode;

	return WLAN_STATUS_SUCCESS;

}				/*                                  */
