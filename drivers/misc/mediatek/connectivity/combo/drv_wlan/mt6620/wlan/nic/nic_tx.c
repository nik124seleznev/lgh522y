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
/*                                                                            */
/* 
                                                                               
                                                     
 
                                                     
 
                
*/
/*                                                                            */
VOID nicTxInitialize(IN P_ADAPTER_T prAdapter)
{
	P_TX_CTRL_T prTxCtrl;
	PUINT_8 pucMemHandle;
	P_MSDU_INFO_T prMsduInfo;
	UINT_32 i;
	KAL_SPIN_LOCK_DECLARATION();

	DEBUGFUNC("nicTxInitialize");

	ASSERT(prAdapter);
	prTxCtrl = &prAdapter->rTxCtrl;

	/*                                                        */
	nicTxResetResource(prAdapter);

#if CFG_SDIO_TX_AGG
	prTxCtrl->pucTxCoalescingBufPtr = prAdapter->pucCoalescingBufCached;
#endif				/*                 */

	/*                                                         */
	QUEUE_INITIALIZE(&prTxCtrl->rFreeMsduInfoList);

	pucMemHandle = prTxCtrl->pucTxCached;
	for (i = 0; i < CFG_TX_MAX_PKT_NUM; i++) {
		prMsduInfo = (P_MSDU_INFO_T) pucMemHandle;
		kalMemZero(prMsduInfo, sizeof(MSDU_INFO_T));

		KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
		QUEUE_INSERT_TAIL(&prTxCtrl->rFreeMsduInfoList, (P_QUE_ENTRY_T) prMsduInfo);
		KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);

		pucMemHandle += ALIGN_4(sizeof(MSDU_INFO_T));
	}

	ASSERT(prTxCtrl->rFreeMsduInfoList.u4NumElem == CFG_TX_MAX_PKT_NUM);
	/*                                                                          */
	ASSERT((UINT_32) (pucMemHandle - prTxCtrl->pucTxCached) == prTxCtrl->u4TxCachedSize);

	QUEUE_INITIALIZE(&prTxCtrl->rTxMgmtTxingQueue);
	prTxCtrl->i4TxMgmtPendingNum = 0;

#if CFG_HIF_STATISTICS
	prTxCtrl->u4TotalTxAccessNum = 0;
	prTxCtrl->u4TotalTxPacketNum = 0;
#endif

	prTxCtrl->i4PendingFwdFrameCount = 0;

	qmInit(prAdapter);

	TX_RESET_ALL_CNTS(prTxCtrl);

	return;
}				/*                          */


/*                                                                            */
/* 
                                                                                    
                                                                                       
                                                                                 
 
                                                                     
                                                              
 
                                                                        
                                                          
*/
/*                                                                            */
WLAN_STATUS nicTxAcquireResource(IN P_ADAPTER_T prAdapter, IN UINT_8 ucTC)
{
	P_TX_CTRL_T prTxCtrl;
	WLAN_STATUS u4Status = WLAN_STATUS_RESOURCES;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);
	prTxCtrl = &prAdapter->rTxCtrl;


	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

/*                                                                                                                           */

	if (prTxCtrl->rTc.aucFreeBufferCount[ucTC]) {

		prTxCtrl->rTc.aucFreeBufferCount[ucTC]--;

		DBGLOG(TX, EVENT, ("Acquire: TC = %d aucFreeBufferCount = %d\n",
				   ucTC, prTxCtrl->rTc.aucFreeBufferCount[ucTC]));

		u4Status = WLAN_STATUS_SUCCESS;
	}
	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

	return u4Status;

}				/*                                       */


/*                                                                            */
/* 
                                                                                    
                                                                                      
                                                           
 
                                                         
                                                  
 
                                                      
                                                          
*/
/*                                                                            */
WLAN_STATUS nicTxPollingResource(IN P_ADAPTER_T prAdapter, IN UINT_8 ucTC)
{
	P_TX_CTRL_T prTxCtrl;
	WLAN_STATUS u4Status = WLAN_STATUS_FAILURE;
	INT_32 i = NIC_TX_RESOURCE_POLLING_TIMEOUT;
	UINT_32 au4WTSR[2];

	ASSERT(prAdapter);
	prTxCtrl = &prAdapter->rTxCtrl;

	if (ucTC >= TC_NUM) {
		return WLAN_STATUS_FAILURE;
	}

	if (prTxCtrl->rTc.aucFreeBufferCount[ucTC] > 0) {
		return WLAN_STATUS_SUCCESS;
	}

	while (i-- > 0) {
		HAL_READ_TX_RELEASED_COUNT(prAdapter, au4WTSR);

		if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE || fgIsBusAccessFailed == TRUE) {
			u4Status = WLAN_STATUS_FAILURE;
			break;
		} else if (nicTxReleaseResource(prAdapter, (PUINT_8) au4WTSR)) {
			if (prTxCtrl->rTc.aucFreeBufferCount[ucTC] > 0) {
				u4Status = WLAN_STATUS_SUCCESS;
				break;
			} else {
				kalMsleep(NIC_TX_RESOURCE_POLLING_DELAY_MSEC);
			}
		} else {
			kalMsleep(NIC_TX_RESOURCE_POLLING_DELAY_MSEC);
		}
	}

#if DBG
	{
		INT_32 i4Times = NIC_TX_RESOURCE_POLLING_TIMEOUT - (i + 1);

		if (i4Times) {
			DBGLOG(TX, TRACE, ("Polling MCR_WTSR delay %d times, %d msec\n",
					   i4Times,
					   (i4Times * NIC_TX_RESOURCE_POLLING_DELAY_MSEC)));
		}
	}
#endif				/*     */

	return u4Status;

}				/*                               */


/*                                                                            */
/* 
                                                                                    
                                                                                 
                                                   
 
                                                                     
                                                      
 
                
*/
/*                                                                            */
BOOLEAN nicTxReleaseResource(IN P_ADAPTER_T prAdapter, IN UINT_8 * aucTxRlsCnt)
{
	PUINT_32 pu4Tmp = (PUINT_32) aucTxRlsCnt;
	P_TX_CTRL_T prTxCtrl;
	BOOLEAN bStatus = FALSE;
	UINT_32 i;

	KAL_SPIN_LOCK_DECLARATION();


	ASSERT(prAdapter);
	prTxCtrl = &prAdapter->rTxCtrl;

	if (pu4Tmp[0] | pu4Tmp[1]) {

		KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);
		for (i = 0; i < TC_NUM; i++) {
			prTxCtrl->rTc.aucFreeBufferCount[i] += aucTxRlsCnt[i];

			if ((i == 1) || (i == 5)) {
				DBGLOG(TX, EVENT, ("Release: i = %d aucFreeBufferCount = %d\n",
						   i, prTxCtrl->rTc.aucFreeBufferCount[i]));
			}
		}
		KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);
#if 0
		for (i = 0; i < TC_NUM; i++) {
			DBGLOG(INIT, TRACE,
			       ("aucFreeBufferCount[%d]: %d, aucMaxNumOfBuffer[%d]: %d\n", i,
				prTxCtrl->rTc.aucFreeBufferCount[i], i,
				prTxCtrl->rTc.aucMaxNumOfBuffer[i]));
		}
		DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[0]=%d\n",
			 prTxCtrl->rTc.aucFreeBufferCount[0]);
		DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[1]=%d\n",
			 prTxCtrl->rTc.aucFreeBufferCount[1]);
		DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[2]=%d\n",
			 prTxCtrl->rTc.aucFreeBufferCount[2]);
		DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[3]=%d\n",
			 prTxCtrl->rTc.aucFreeBufferCount[3]);
		DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[4]=%d\n",
			 prTxCtrl->rTc.aucFreeBufferCount[4]);
		DbgPrint("prTxCtrl->rTc.aucFreeBufferCount[5]=%d\n",
			 prTxCtrl->rTc.aucFreeBufferCount[5]);
#endif
		ASSERT(prTxCtrl->rTc.aucFreeBufferCount[TC0_INDEX] <=
		       prTxCtrl->rTc.aucMaxNumOfBuffer[TC0_INDEX]);
		ASSERT(prTxCtrl->rTc.aucFreeBufferCount[TC1_INDEX] <=
		       prTxCtrl->rTc.aucMaxNumOfBuffer[TC1_INDEX]);
		ASSERT(prTxCtrl->rTc.aucFreeBufferCount[TC2_INDEX] <=
		       prTxCtrl->rTc.aucMaxNumOfBuffer[TC2_INDEX]);
		ASSERT(prTxCtrl->rTc.aucFreeBufferCount[TC3_INDEX] <=
		       prTxCtrl->rTc.aucMaxNumOfBuffer[TC3_INDEX]);
		ASSERT(prTxCtrl->rTc.aucFreeBufferCount[TC4_INDEX] <=
		       prTxCtrl->rTc.aucMaxNumOfBuffer[TC4_INDEX]);
		ASSERT(prTxCtrl->rTc.aucFreeBufferCount[TC5_INDEX] <=
		       prTxCtrl->rTc.aucMaxNumOfBuffer[TC5_INDEX]);
		bStatus = TRUE;
	}

	return bStatus;
}				/*                               */


/*                                                                            */
/* 
                                                   
 
                                                                     
 
                             
*/
/*                                                                            */
WLAN_STATUS nicTxResetResource(IN P_ADAPTER_T prAdapter)
{
	P_TX_CTRL_T prTxCtrl;

	KAL_SPIN_LOCK_DECLARATION();

	DEBUGFUNC("nicTxResetResource");

	ASSERT(prAdapter);
	prTxCtrl = &prAdapter->rTxCtrl;

	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC0_INDEX] = NIC_TX_BUFF_COUNT_TC0;
	prTxCtrl->rTc.aucFreeBufferCount[TC0_INDEX] = NIC_TX_BUFF_COUNT_TC0;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC1_INDEX] = NIC_TX_BUFF_COUNT_TC1;
	prTxCtrl->rTc.aucFreeBufferCount[TC1_INDEX] = NIC_TX_BUFF_COUNT_TC1;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC2_INDEX] = NIC_TX_BUFF_COUNT_TC2;
	prTxCtrl->rTc.aucFreeBufferCount[TC2_INDEX] = NIC_TX_BUFF_COUNT_TC2;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC3_INDEX] = NIC_TX_BUFF_COUNT_TC3;
	prTxCtrl->rTc.aucFreeBufferCount[TC3_INDEX] = NIC_TX_BUFF_COUNT_TC3;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC4_INDEX] = NIC_TX_BUFF_COUNT_TC4;
	prTxCtrl->rTc.aucFreeBufferCount[TC4_INDEX] = NIC_TX_BUFF_COUNT_TC4;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC5_INDEX] = NIC_TX_BUFF_COUNT_TC5;
	prTxCtrl->rTc.aucFreeBufferCount[TC5_INDEX] = NIC_TX_BUFF_COUNT_TC5;

	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

	return WLAN_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                                                    
                                                                                 
                                                          
 
                                                         
                                                  
 
                                                             
*/
/*                                                                            */
UINT_8 nicTxGetResource(IN P_ADAPTER_T prAdapter, IN UINT_8 ucTC)
{
	P_TX_CTRL_T prTxCtrl;

	ASSERT(prAdapter);
	prTxCtrl = &prAdapter->rTxCtrl;

	ASSERT(prTxCtrl);

	if (ucTC >= TC_NUM) {
		return 0;
	} else {
		return prTxCtrl->rTc.aucFreeBufferCount[ucTC];
	}
}


/*                                                                            */
/* 
                                                               
                              
 
                                                                 
                                                            
 
                                              
                                                
*/
/*                                                                            */
WLAN_STATUS nicTxMsduInfoList(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfoListHead)
{
	P_MSDU_INFO_T prMsduInfo, prNextMsduInfo;
	QUE_T qDataPort0, qDataPort1;
	WLAN_STATUS status;

	ASSERT(prAdapter);
	ASSERT(prMsduInfoListHead);

	prMsduInfo = prMsduInfoListHead;

	QUEUE_INITIALIZE(&qDataPort0);
	QUEUE_INITIALIZE(&qDataPort1);

	/*                                                                   */
	while (prMsduInfo) {
		prNextMsduInfo = (P_MSDU_INFO_T) QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T) prMsduInfo);
#if DBG && 0
		LOG_FUNC
		    ("nicTxMsduInfoList Acquire TC %d net %u mac len %u len %u Type %u 1x %u 11 %u\n",
		     prMsduInfo->ucTC, prMsduInfo->ucNetworkType, prMsduInfo->ucMacHeaderLength,
		     prMsduInfo->u2FrameLength, prMsduInfo->ucPacketType, prMsduInfo->fgIs802_1x,
		     prMsduInfo->fgIs802_11);

		LOG_FUNC("Dest Mac: " MACSTR "\n", MAC2STR(prMsduInfo->aucEthDestAddr));
#endif

		switch (prMsduInfo->ucTC) {
		case TC0_INDEX:
		case TC1_INDEX:
		case TC2_INDEX:
		case TC3_INDEX:
		case TC5_INDEX:	/*                                  */
			QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T) prMsduInfo) = NULL;
			QUEUE_INSERT_TAIL(&qDataPort0, (P_QUE_ENTRY_T) prMsduInfo);
			status = nicTxAcquireResource(prAdapter, prMsduInfo->ucTC);
			ASSERT(status == WLAN_STATUS_SUCCESS)

			    break;

		case TC4_INDEX:	/*                           */
			QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T) prMsduInfo) = NULL;
			QUEUE_INSERT_TAIL(&qDataPort1, (P_QUE_ENTRY_T) prMsduInfo);

			status = nicTxAcquireResource(prAdapter, prMsduInfo->ucTC);
			ASSERT(status == WLAN_STATUS_SUCCESS)

			    break;

		default:
			ASSERT(0);
			break;
		}

		prMsduInfo = prNextMsduInfo;
	}

	if (qDataPort0.u4NumElem > 0) {
		nicTxMsduQueue(prAdapter, 0, &qDataPort0);
	}

	if (qDataPort1.u4NumElem > 0) {
		nicTxMsduQueue(prAdapter, 1, &qDataPort1);
	}

	return WLAN_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                                     
 
                                                                 
                                           
                                                            
 
                                              
                                                
*/
/*                                                                            */
WLAN_STATUS nicTxMsduQueue(IN P_ADAPTER_T prAdapter, UINT_8 ucPortIdx, P_QUE_T prQue)
{
	P_MSDU_INFO_T prMsduInfo, prNextMsduInfo;
	HIF_TX_HEADER_T rHwTxHeader;
	P_NATIVE_PACKET prNativePacket;
	UINT_16 u2OverallBufferLength;
	UINT_8 ucEtherTypeOffsetInWord;
	PUINT_8 pucOutputBuf = (PUINT_8) NULL;	/*                                          */
	UINT_32 u4TxHdrSize;
	UINT_32 u4ValidBufSize;
	UINT_32 u4TotalLength;
	P_TX_CTRL_T prTxCtrl;
	QUE_T rFreeQueue;
#if CFG_TCP_IP_CHKSUM_OFFLOAD
	UINT_8 ucChksumFlag;
#endif

	ASSERT(prAdapter);
	ASSERT(ucPortIdx < 2);
	ASSERT(prQue);

	prTxCtrl = &prAdapter->rTxCtrl;
	u4ValidBufSize = prAdapter->u4CoalescingBufCachedSize;

#if CFG_HIF_STATISTICS
	prTxCtrl->u4TotalTxAccessNum++;
	prTxCtrl->u4TotalTxPacketNum += prQue->u4NumElem;
#endif

	QUEUE_INITIALIZE(&rFreeQueue);

	if (prQue->u4NumElem > 0) {
		prMsduInfo = (P_MSDU_INFO_T) QUEUE_GET_HEAD(prQue);
		pucOutputBuf = prTxCtrl->pucTxCoalescingBufPtr;
		u4TotalLength = 0;

		while (prMsduInfo) {

			kalMemZero(&rHwTxHeader, sizeof(rHwTxHeader));

			prNativePacket = prMsduInfo->prPacket;

			ASSERT(prNativePacket);

			u4TxHdrSize = TX_HDR_SIZE;

			u2OverallBufferLength = ((prMsduInfo->u2FrameLength + TX_HDR_SIZE) &
						 (UINT_16) HIF_TX_HDR_TX_BYTE_COUNT_MASK);

			rHwTxHeader.u2TxByteCount_UserPriority = u2OverallBufferLength;
			rHwTxHeader.u2TxByteCount_UserPriority |=
			    ((UINT_16) prMsduInfo->
			     ucUserPriority << HIF_TX_HDR_USER_PRIORITY_OFFSET);

			if (prMsduInfo->fgIs802_11) {
				ucEtherTypeOffsetInWord =
				    (TX_HDR_SIZE + prMsduInfo->ucMacHeaderLength +
				     prMsduInfo->ucLlcLength) >> 1;
			} else {
				ucEtherTypeOffsetInWord =
				    ((ETHER_HEADER_LEN - ETHER_TYPE_LEN) + TX_HDR_SIZE) >> 1;
			}

			rHwTxHeader.ucEtherTypeOffset =
			    ucEtherTypeOffsetInWord & HIF_TX_HDR_ETHER_TYPE_OFFSET_MASK;

			rHwTxHeader.ucResource_PktType_CSflags =
			    (prMsduInfo->ucTC) << HIF_TX_HDR_RESOURCE_OFFSET;
			rHwTxHeader.ucResource_PktType_CSflags |=
			    (UINT_8) (((prMsduInfo->
					ucPacketType) << HIF_TX_HDR_PACKET_TYPE_OFFSET) &
				      (HIF_TX_HDR_PACKET_TYPE_MASK));

#if CFG_TCP_IP_CHKSUM_OFFLOAD
			if (prMsduInfo->eSrc == TX_PACKET_OS
			    || prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
				if (prAdapter->u4CSUMFlags &
				    (CSUM_OFFLOAD_EN_TX_TCP |
				     CSUM_OFFLOAD_EN_TX_UDP | CSUM_OFFLOAD_EN_TX_IP)) {
					kalQueryTxChksumOffloadParam(prNativePacket, &ucChksumFlag);

					if (ucChksumFlag & TX_CS_IP_GEN) {
						rHwTxHeader.ucResource_PktType_CSflags |=
						    (UINT_8) HIF_TX_HDR_IP_CSUM;
					}

					if (ucChksumFlag & TX_CS_TCP_UDP_GEN) {
						rHwTxHeader.ucResource_PktType_CSflags |=
						    (UINT_8) HIF_TX_HDR_TCP_CSUM;
					}
				}
			}
#endif				/*                           */

			rHwTxHeader.u2LLH = prMsduInfo->u2PalLLH;
			rHwTxHeader.ucStaRecIdx = prMsduInfo->ucStaRecIndex;
			rHwTxHeader.ucForwardingType_SessionID_Reserved =
			    (prMsduInfo->
			     ucPsForwardingType) | ((prMsduInfo->
						     ucPsSessionID) <<
						    HIF_TX_HDR_PS_SESSION_ID_OFFSET)
			    | ((prMsduInfo->fgIsBurstEnd) ? HIF_TX_HDR_BURST_END_MASK : 0);

			rHwTxHeader.ucWlanHeaderLength =
			    (prMsduInfo->ucMacHeaderLength & HIF_TX_HDR_WLAN_HEADER_LEN_MASK);
			rHwTxHeader.ucPktFormtId_Flags =
			    (prMsduInfo->ucFormatID & HIF_TX_HDR_FORMAT_ID_MASK)
			    | ((prMsduInfo->ucNetworkType << HIF_TX_HDR_NETWORK_TYPE_OFFSET) &
			       HIF_TX_HDR_NETWORK_TYPE_MASK)
			    | ((prMsduInfo->fgIs802_1x << HIF_TX_HDR_FLAG_1X_FRAME_OFFSET) &
			       HIF_TX_HDR_FLAG_1X_FRAME_MASK)
			    | ((prMsduInfo->fgIs802_11 << HIF_TX_HDR_FLAG_802_11_FORMAT_OFFSET) &
			       HIF_TX_HDR_FLAG_802_11_FORMAT_MASK);

			rHwTxHeader.u2SeqNo = prMsduInfo->u2AclSN;

			if (prMsduInfo->pfTxDoneHandler) {
				rHwTxHeader.ucPacketSeqNo = prMsduInfo->ucTxSeqNum;
				rHwTxHeader.ucAck_BIP_BasicRate = HIF_TX_HDR_NEED_ACK;
			} else {
				rHwTxHeader.ucPacketSeqNo = 0;
				rHwTxHeader.ucAck_BIP_BasicRate = 0;
			}

			if (prMsduInfo->fgIsBIP) {
				rHwTxHeader.ucAck_BIP_BasicRate |= HIF_TX_HDR_BIP;
			}

			if (prMsduInfo->fgIsBasicRate) {
				rHwTxHeader.ucAck_BIP_BasicRate |= HIF_TX_HDR_BASIC_RATE;
			}
#if CFG_SDIO_TX_AGG
			/*                             */
			kalMemCopy(pucOutputBuf + u4TotalLength, &rHwTxHeader, u4TxHdrSize);
			u4TotalLength += u4TxHdrSize;

			if (prMsduInfo->eSrc == TX_PACKET_OS
			    || prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
				kalCopyFrame(prAdapter->prGlueInfo,
					     prNativePacket, pucOutputBuf + u4TotalLength);
			} else if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
				kalMemCopy(pucOutputBuf + u4TotalLength,
					   prNativePacket, prMsduInfo->u2FrameLength);
			} else {
				ASSERT(0);
			}

			u4TotalLength += ALIGN_4(prMsduInfo->u2FrameLength);

#else
			kalMemCopy(pucOutputBuf, &rHwTxHeader, u4TxHdrSize);

			/*                 */
			if (prMsduInfo->eSrc == TX_PACKET_OS
			    || prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
				kalCopyFrame(prAdapter->prGlueInfo,
					     prNativePacket, pucOutputBuf + u4TxHdrSize);
			} else if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
				kalMemCopy(pucOutputBuf + u4TxHdrSize,
					   prNativePacket, prMsduInfo->u2FrameLength);
			} else {
				ASSERT(0);
			}

			ASSERT(u2OverallBufferLength <= u4ValidBufSize);

			HAL_WRITE_TX_PORT(prAdapter,
					  ucPortIdx,
					  (UINT_32) u2OverallBufferLength,
					  (PUINT_8) pucOutputBuf, u4ValidBufSize);

			/*                  */
#endif
			prNextMsduInfo = (P_MSDU_INFO_T)
			    QUEUE_GET_NEXT_ENTRY(&prMsduInfo->rQueEntry);

			if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
				GLUE_DEC_REF_CNT(prTxCtrl->i4TxMgmtPendingNum);

				if (prMsduInfo->pfTxDoneHandler == NULL) {
					cnmMgtPktFree(prAdapter, prMsduInfo);
				} else {
					KAL_SPIN_LOCK_DECLARATION();
					DBGLOG(INIT, TRACE,
					       ("Wait TxSeqNum:%d\n", prMsduInfo->ucTxSeqNum));
					KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
					QUEUE_INSERT_TAIL(&(prTxCtrl->rTxMgmtTxingQueue),
							  (P_QUE_ENTRY_T) prMsduInfo);
					KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
				}
			} else {
				/*                                            */
				QUEUE_INSERT_TAIL(&rFreeQueue, (P_QUE_ENTRY_T) prMsduInfo);

				if (prMsduInfo->eSrc == TX_PACKET_OS) {
					kalSendComplete(prAdapter->prGlueInfo,
							prNativePacket, WLAN_STATUS_SUCCESS);
				} else if (prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
					GLUE_DEC_REF_CNT(prTxCtrl->i4PendingFwdFrameCount);
				}
			}

			prMsduInfo = prNextMsduInfo;
		}

#if CFG_SDIO_TX_AGG
		ASSERT(u4TotalLength <= u4ValidBufSize);

#if CFG_DBG_GPIO_PINS
		{
			/*                  */
			mtk_wcn_stp_debug_gpio_assert(IDX_TX_PORT_WRITE, DBG_TIE_LOW);
			kalUdelay(1);
			mtk_wcn_stp_debug_gpio_assert(IDX_TX_PORT_WRITE, DBG_TIE_HIGH);
		}
#endif

		/*                        */
		HAL_WRITE_TX_PORT(prAdapter,
				  ucPortIdx, u4TotalLength, (PUINT_8) pucOutputBuf, u4ValidBufSize);
#endif

		/*        */
		nicTxReturnMsduInfo(prAdapter, (P_MSDU_INFO_T) QUEUE_GET_HEAD(&rFreeQueue));
	}

	return WLAN_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                                    
 
                                                         
                                             
                                                  
 
                                              
                                                
*/
/*                                                                            */
WLAN_STATUS nicTxCmd(IN P_ADAPTER_T prAdapter, IN P_CMD_INFO_T prCmdInfo, IN UINT_8 ucTC)
{
	P_WIFI_CMD_T prWifiCmd;
	UINT_16 u2OverallBufferLength;
	PUINT_8 pucOutputBuf = (PUINT_8) NULL;	/*                                          */
	UINT_8 ucPortIdx;
	HIF_TX_HEADER_T rHwTxHeader;
	P_NATIVE_PACKET prNativePacket;
	UINT_8 ucEtherTypeOffsetInWord;
	P_MSDU_INFO_T prMsduInfo;
	P_TX_CTRL_T prTxCtrl;

	KAL_SPIN_LOCK_DECLARATION();


	ASSERT(prAdapter);
	ASSERT(prCmdInfo);

	prTxCtrl = &prAdapter->rTxCtrl;
	pucOutputBuf = prTxCtrl->pucTxCoalescingBufPtr;

	/*                      */
	if (ucTC != TC4_INDEX) {
		ucPortIdx = 0;
	} else {
		/*                                                                    */
		ucPortIdx = 1;
	}

	if (prCmdInfo->eCmdType == COMMAND_TYPE_SECURITY_FRAME) {
		/*                           */
		kalMemZero(&rHwTxHeader, sizeof(rHwTxHeader));

		prNativePacket = prCmdInfo->prPacket;

		ASSERT(prNativePacket);

		u2OverallBufferLength = TFCB_FRAME_PAD_TO_DW((prCmdInfo->u2InfoBufLen + TX_HDR_SIZE)
							     & (UINT_16)
							     HIF_TX_HDR_TX_BYTE_COUNT_MASK);

		rHwTxHeader.u2TxByteCount_UserPriority = ((prCmdInfo->u2InfoBufLen + TX_HDR_SIZE)
							  & (UINT_16)
							  HIF_TX_HDR_TX_BYTE_COUNT_MASK);
		ucEtherTypeOffsetInWord = ((ETHER_HEADER_LEN - ETHER_TYPE_LEN) + TX_HDR_SIZE) >> 1;

		rHwTxHeader.ucEtherTypeOffset =
		    ucEtherTypeOffsetInWord & HIF_TX_HDR_ETHER_TYPE_OFFSET_MASK;

		rHwTxHeader.ucResource_PktType_CSflags = (ucTC << HIF_TX_HDR_RESOURCE_OFFSET);

		rHwTxHeader.ucStaRecIdx = prCmdInfo->ucStaRecIndex;
		rHwTxHeader.ucForwardingType_SessionID_Reserved = HIF_TX_HDR_BURST_END_MASK;

		rHwTxHeader.ucWlanHeaderLength = (ETH_HLEN & HIF_TX_HDR_WLAN_HEADER_LEN_MASK);
		rHwTxHeader.ucPktFormtId_Flags =
		    (((UINT_8) (prCmdInfo->eNetworkType) << HIF_TX_HDR_NETWORK_TYPE_OFFSET) &
		     HIF_TX_HDR_NETWORK_TYPE_MASK)
		    | ((1 << HIF_TX_HDR_FLAG_1X_FRAME_OFFSET) & HIF_TX_HDR_FLAG_1X_FRAME_MASK);

		rHwTxHeader.u2SeqNo = 0;
		rHwTxHeader.ucPacketSeqNo = 0;
		rHwTxHeader.ucAck_BIP_BasicRate = 0;

		/*                          */
		kalMemCopy((PVOID) & pucOutputBuf[0], (PVOID) & rHwTxHeader, TX_HDR_SIZE);

		/*                          */
		kalCopyFrame(prAdapter->prGlueInfo, prNativePacket, pucOutputBuf + TX_HDR_SIZE);
	} else if (prCmdInfo->eCmdType == COMMAND_TYPE_MANAGEMENT_FRAME) {
		prMsduInfo = (P_MSDU_INFO_T) prCmdInfo->prPacket;

		ASSERT(prMsduInfo->fgIs802_11 == TRUE);
		ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

		/*                           */
		kalMemZero(&rHwTxHeader, sizeof(rHwTxHeader));

		u2OverallBufferLength = ((prMsduInfo->u2FrameLength + TX_HDR_SIZE) &
					 (UINT_16) HIF_TX_HDR_TX_BYTE_COUNT_MASK);

		rHwTxHeader.u2TxByteCount_UserPriority = u2OverallBufferLength;
		rHwTxHeader.u2TxByteCount_UserPriority |=
		    ((UINT_16) prMsduInfo->ucUserPriority << HIF_TX_HDR_USER_PRIORITY_OFFSET);

		ucEtherTypeOffsetInWord =
		    (TX_HDR_SIZE + prMsduInfo->ucMacHeaderLength + prMsduInfo->ucLlcLength) >> 1;

		rHwTxHeader.ucEtherTypeOffset =
		    ucEtherTypeOffsetInWord & HIF_TX_HDR_ETHER_TYPE_OFFSET_MASK;

		rHwTxHeader.ucResource_PktType_CSflags =
		    (prMsduInfo->ucTC) << HIF_TX_HDR_RESOURCE_OFFSET;
		rHwTxHeader.ucResource_PktType_CSflags |=
		    (UINT_8) (((prMsduInfo->
				ucPacketType) << HIF_TX_HDR_PACKET_TYPE_OFFSET) &
			      (HIF_TX_HDR_PACKET_TYPE_MASK));

		rHwTxHeader.u2LLH = prMsduInfo->u2PalLLH;
		rHwTxHeader.ucStaRecIdx = prMsduInfo->ucStaRecIndex;
		rHwTxHeader.ucForwardingType_SessionID_Reserved =
		    (prMsduInfo->
		     ucPsForwardingType) | ((prMsduInfo->
					     ucPsSessionID) << HIF_TX_HDR_PS_SESSION_ID_OFFSET)
		    | ((prMsduInfo->fgIsBurstEnd) ? HIF_TX_HDR_BURST_END_MASK : 0);

		rHwTxHeader.ucWlanHeaderLength =
		    (prMsduInfo->ucMacHeaderLength & HIF_TX_HDR_WLAN_HEADER_LEN_MASK);
		rHwTxHeader.ucPktFormtId_Flags =
		    (prMsduInfo->ucFormatID & HIF_TX_HDR_FORMAT_ID_MASK)
		    | ((prMsduInfo->ucNetworkType << HIF_TX_HDR_NETWORK_TYPE_OFFSET) &
		       HIF_TX_HDR_NETWORK_TYPE_MASK)
		    | ((prMsduInfo->fgIs802_1x << HIF_TX_HDR_FLAG_1X_FRAME_OFFSET) &
		       HIF_TX_HDR_FLAG_1X_FRAME_MASK)
		    | ((prMsduInfo->fgIs802_11 << HIF_TX_HDR_FLAG_802_11_FORMAT_OFFSET) &
		       HIF_TX_HDR_FLAG_802_11_FORMAT_MASK);

		rHwTxHeader.u2SeqNo = prMsduInfo->u2AclSN;

		if (prMsduInfo->pfTxDoneHandler) {
			rHwTxHeader.ucPacketSeqNo = prMsduInfo->ucTxSeqNum;
			rHwTxHeader.ucAck_BIP_BasicRate = HIF_TX_HDR_NEED_ACK;
		} else {
			rHwTxHeader.ucPacketSeqNo = 0;
			rHwTxHeader.ucAck_BIP_BasicRate = 0;
		}

		if (prMsduInfo->fgIsBIP) {
			rHwTxHeader.ucAck_BIP_BasicRate |= HIF_TX_HDR_BIP;
		}

		if (prMsduInfo->fgIsBasicRate) {
			rHwTxHeader.ucAck_BIP_BasicRate |= HIF_TX_HDR_BASIC_RATE;
		}
		/*                          */
		kalMemCopy((PVOID) & pucOutputBuf[0], (PVOID) & rHwTxHeader, TX_HDR_SIZE);

		/*                     */
		kalMemCopy(pucOutputBuf + TX_HDR_SIZE,
			   prMsduInfo->prPacket, prMsduInfo->u2FrameLength);

		/*                                      */
		GLUE_DEC_REF_CNT(prTxCtrl->i4TxMgmtPendingNum);

		if (prMsduInfo->pfTxDoneHandler == NULL) {
			cnmMgtPktFree(prAdapter, prMsduInfo);
		} else {

			DBGLOG(INIT, TRACE, ("Wait Cmd TxSeqNum:%d\n", prMsduInfo->ucTxSeqNum));

			KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
			QUEUE_INSERT_TAIL(&(prTxCtrl->rTxMgmtTxingQueue),
					  (P_QUE_ENTRY_T) prMsduInfo);
			KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
		}
	} else {
		prWifiCmd = (P_WIFI_CMD_T) prCmdInfo->pucInfoBuffer;

		/*                                                                  */
		u2OverallBufferLength = TFCB_FRAME_PAD_TO_DW((prCmdInfo->
							      u2InfoBufLen) & (UINT_16)
							     HIF_TX_HDR_TX_BYTE_COUNT_MASK);

		prWifiCmd->u2TxByteCount_UserPriority = u2OverallBufferLength;
		prWifiCmd->ucEtherTypeOffset = 0;
		prWifiCmd->ucResource_PktType_CSflags = (ucTC << HIF_TX_HDR_RESOURCE_OFFSET)
		    | (UINT_8) ((HIF_TX_PKT_TYPE_CMD << HIF_TX_HDR_PACKET_TYPE_OFFSET) &
				(HIF_TX_HDR_PACKET_TYPE_MASK));


		/*                                                                         */
		kalMemCopy((PVOID) & pucOutputBuf[0],
			   (PVOID) prCmdInfo->pucInfoBuffer, prCmdInfo->u2InfoBufLen);

		ASSERT(u2OverallBufferLength <= prAdapter->u4CoalescingBufCachedSize);
	}

	/*                              */
	HAL_WRITE_TX_PORT(prAdapter,
			  ucPortIdx,
			  (UINT_32) u2OverallBufferLength,
			  (PUINT_8) pucOutputBuf, (UINT_32) prAdapter->u4CoalescingBufCachedSize);

	return WLAN_STATUS_SUCCESS;
}				/*                   */


/*                                                                            */
/* 
                                                                                 
                                                       
 
                                                     
 
                
*/
/*                                                                            */
VOID nicTxRelease(IN P_ADAPTER_T prAdapter)
{
	P_TX_CTRL_T prTxCtrl;
	P_MSDU_INFO_T prMsduInfo;

	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);

	prTxCtrl = &prAdapter->rTxCtrl;

	nicTxFlush(prAdapter);

	/*                                           */
	do {
		KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);
		QUEUE_REMOVE_HEAD(&prTxCtrl->rTxMgmtTxingQueue, prMsduInfo, P_MSDU_INFO_T);
		KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TXING_MGMT_LIST);

		if (prMsduInfo) {
			/*                                                     */
			ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);
			ASSERT(prMsduInfo->pfTxDoneHandler != NULL);

			/*                     */
			prMsduInfo->pfTxDoneHandler(prAdapter, prMsduInfo, TX_RESULT_LIFE_TIMEOUT);

			cnmMgtPktFree(prAdapter, prMsduInfo);
		} else {
			break;
		}
	} while (TRUE);

	return;
}				/*                       */


/*                                                                            */
/* 
                                                                            
                                 
 
                                                     
 
                
*/
/*                                                                            */
VOID nicProcessTxInterrupt(IN P_ADAPTER_T prAdapter)
{
	P_TX_CTRL_T prTxCtrl;
#if CFG_SDIO_INTR_ENHANCE
	P_SDIO_CTRL_T prSDIOCtrl;
#else
	UINT_32 au4TxCount[2];
#endif				/*                       */

	ASSERT(prAdapter);

	prTxCtrl = &prAdapter->rTxCtrl;
	ASSERT(prTxCtrl);

	/*                   */
#if CFG_SDIO_INTR_ENHANCE

	prSDIOCtrl = prAdapter->prSDIOCtrl;
#if DBG
	/*                                                        */
#endif

	nicTxReleaseResource(prAdapter, (PUINT_8) &prSDIOCtrl->rTxInfo);
	kalMemZero(&prSDIOCtrl->rTxInfo, sizeof(prSDIOCtrl->rTxInfo));

#else

	HAL_MCR_RD(prAdapter, MCR_WTSR0, &au4TxCount[0]);
	HAL_MCR_RD(prAdapter, MCR_WTSR1, &au4TxCount[1]);
	DBGLOG(EMU, TRACE, ("MCR_WTSR0: 0x%x, MCR_WTSR1: 0x%x\n", au4TxCount[0], au4TxCount[1]));

	nicTxReleaseResource(prAdapter, (PUINT_8) au4TxCount);

#endif				/*                       */

	nicTxAdjustTcq(prAdapter);

	/*                         */
	if (kalGetTxPendingCmdCount(prAdapter->prGlueInfo) > 0
	    || wlanGetTxPendingFrameCount(prAdapter) > 0) {
		kalSetEvent(prAdapter->prGlueInfo);
	}

	return;
}				/*                                */


/*                                                                            */
/* 
                                                                
 
                                                                 
                                                            
 
                
*/
/*                                                                            */
VOID nicTxFreeMsduInfoPacket(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfoListHead)
{
	P_NATIVE_PACKET prNativePacket;
	P_MSDU_INFO_T prMsduInfo = prMsduInfoListHead;
	P_TX_CTRL_T prTxCtrl;


	ASSERT(prAdapter);
	ASSERT(prMsduInfoListHead);

	prTxCtrl = &prAdapter->rTxCtrl;

	while (prMsduInfo) {
		prNativePacket = prMsduInfo->prPacket;

		if (prMsduInfo->eSrc == TX_PACKET_OS) {
			kalSendComplete(prAdapter->prGlueInfo, prNativePacket, WLAN_STATUS_FAILURE);
		} else if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
			if (prMsduInfo->pfTxDoneHandler) {
				prMsduInfo->pfTxDoneHandler(prAdapter, prMsduInfo,
							    TX_RESULT_DROPPED_IN_DRIVER);
			}
			cnmMemFree(prAdapter, prNativePacket);
		} else if (prMsduInfo->eSrc == TX_PACKET_FORWARDING) {
			GLUE_DEC_REF_CNT(prTxCtrl->i4PendingFwdFrameCount);
		}

		prMsduInfo = (P_MSDU_INFO_T) QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T) prMsduInfo);
	}

	return;
}


/*                                                                            */
/* 
                                                                                         
 
                                                                 
                                                            
 
                
*/
/*                                                                            */
VOID nicTxReturnMsduInfo(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfoListHead)
{
	P_TX_CTRL_T prTxCtrl;
	P_MSDU_INFO_T prMsduInfo = prMsduInfoListHead, prNextMsduInfo;

	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);

	prTxCtrl = &prAdapter->rTxCtrl;
	ASSERT(prTxCtrl);

	while (prMsduInfo) {
		prNextMsduInfo = (P_MSDU_INFO_T) QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T) prMsduInfo);

		switch (prMsduInfo->eSrc) {
		case TX_PACKET_FORWARDING:
			wlanReturnPacket(prAdapter, prMsduInfo->prPacket);
			break;
		case TX_PACKET_OS:
		case TX_PACKET_OS_OID:
		case TX_PACKET_MGMT:
		default:
			break;
		}

		KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
		QUEUE_INSERT_TAIL(&prTxCtrl->rFreeMsduInfoList, (P_QUE_ENTRY_T) prMsduInfo);
		KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
		prMsduInfo = prNextMsduInfo;
	};

	return;
}



/*                                                                            */
/* 
                                                                
 
                                                                 
                                             
                                               
 
                                                  
                                                       
*/
/*                                                                            */
BOOLEAN
nicTxFillMsduInfo(IN P_ADAPTER_T prAdapter,
		  IN P_MSDU_INFO_T prMsduInfo, IN P_NATIVE_PACKET prPacket)
{
	P_GLUE_INFO_T prGlueInfo;
	UINT_8 ucPriorityParam;
	UINT_8 ucMacHeaderLen;
	UINT_8 aucEthDestAddr[PARAM_MAC_ADDR_LEN];
	BOOLEAN fgIs1x = FALSE;
	BOOLEAN fgIsPAL = FALSE;
	UINT_32 u4PacketLen;
	ULONG u4SysTime;
	UINT_8 ucNetworkType;


	ASSERT(prAdapter);

	prGlueInfo = prAdapter->prGlueInfo;
	ASSERT(prGlueInfo);

	if (kalQoSFrameClassifierAndPacketInfo(prGlueInfo,
					       prPacket,
					       &ucPriorityParam,
					       &u4PacketLen,
					       aucEthDestAddr,
					       &fgIs1x, &fgIsPAL, &ucNetworkType) == FALSE) {
		return FALSE;
	}

	/*                                      */
	GLUE_SET_PKT_TID(prPacket, ucPriorityParam);

	if (fgIs1x) {
		GLUE_SET_PKT_FLAG_1X(prPacket);
	}

	if (fgIsPAL) {
		GLUE_SET_PKT_FLAG_PAL(prPacket);
	}

	ucMacHeaderLen = ETH_HLEN;

	/*                                 */
	GLUE_SET_PKT_HEADER_LEN(prPacket, ucMacHeaderLen);

	/*                                */
	GLUE_SET_PKT_FRAME_LEN(prPacket, (UINT_16) u4PacketLen);

	/*                                */
	u4SysTime = (OS_SYSTIME) kalGetTimeTick();
	GLUE_SET_PKT_ARRIVAL_TIME(prPacket, u4SysTime);

	prMsduInfo->prPacket = prPacket;
	prMsduInfo->fgIs802_1x = fgIs1x;
	prMsduInfo->fgIs802_11 = FALSE;
	prMsduInfo->ucNetworkType = ucNetworkType;
	prMsduInfo->ucUserPriority = ucPriorityParam;
	prMsduInfo->ucMacHeaderLength = ucMacHeaderLen;
	prMsduInfo->u2FrameLength = (UINT_16) u4PacketLen;
	COPY_MAC_ADDR(prMsduInfo->aucEthDestAddr, aucEthDestAddr);

	return TRUE;
}


/*                                                                            */
/* 
                                                                                      
 
                                                                 
 
                                                    
*/
/*                                                                            */
WLAN_STATUS nicTxAdjustTcq(IN P_ADAPTER_T prAdapter)
{
	UINT_32 u4Num;
	TX_TCQ_ADJUST_T rTcqAdjust;
	P_TX_CTRL_T prTxCtrl;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);

	prTxCtrl = &prAdapter->rTxCtrl;
	ASSERT(prTxCtrl);

	qmAdjustTcQuotas(prAdapter, &rTcqAdjust, &prTxCtrl->rTc);
	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

	for (u4Num = 0; u4Num < TC_NUM; u4Num++) {
		prTxCtrl->rTc.aucFreeBufferCount[u4Num] += rTcqAdjust.acVariation[u4Num];
		prTxCtrl->rTc.aucMaxNumOfBuffer[u4Num] += rTcqAdjust.acVariation[u4Num];

		ASSERT(prTxCtrl->rTc.aucFreeBufferCount[u4Num] >= 0);
		ASSERT(prTxCtrl->rTc.aucMaxNumOfBuffer[u4Num] >= 0);
	}

	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_RESOURCE);

	return WLAN_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                                 
 
                                                                 
 
                                                    
*/
/*                                                                            */

WLAN_STATUS nicTxFlush(IN P_ADAPTER_T prAdapter)
{
	P_MSDU_INFO_T prMsduInfo;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);

	/*                                                                   */
	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);
	prMsduInfo = qmFlushTxQueues(prAdapter);
	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);

	if (prMsduInfo != NULL) {
		nicTxFreeMsduInfoPacket(prAdapter, prMsduInfo);
		nicTxReturnMsduInfo(prAdapter, prMsduInfo);
	}

	return WLAN_STATUS_SUCCESS;
}


#if CFG_ENABLE_FW_DOWNLOAD
/*                                                                            */
/* 
                                                                    
                                                    
 
                                                                                       
 
                                                         
                                             
                                                  
 
                                              
                                                
*/
/*                                                                            */
WLAN_STATUS nicTxInitCmd(IN P_ADAPTER_T prAdapter, IN P_CMD_INFO_T prCmdInfo, IN UINT_8 ucTC)
{
	P_INIT_HIF_TX_HEADER_T prInitTxHeader;
	UINT_16 u2OverallBufferLength;
	PUINT_8 pucOutputBuf = (PUINT_8) NULL;	/*                                          */
	UINT_32 ucPortIdx;
	P_TX_CTRL_T prTxCtrl;

	ASSERT(prAdapter);
	ASSERT(prCmdInfo);
	ASSERT(ucTC == TC0_INDEX);

	prTxCtrl = &prAdapter->rTxCtrl;
	pucOutputBuf = prTxCtrl->pucTxCoalescingBufPtr;
	prInitTxHeader = (P_INIT_HIF_TX_HEADER_T) prCmdInfo->pucInfoBuffer;

	/*                                                                  */
	u2OverallBufferLength = TFCB_FRAME_PAD_TO_DW((prCmdInfo->
						      u2InfoBufLen) & (UINT_16)
						     HIF_TX_HDR_TX_BYTE_COUNT_MASK);

	prInitTxHeader->u2TxByteCount = u2OverallBufferLength;
	prInitTxHeader->ucEtherTypeOffset = 0;
	prInitTxHeader->ucCSflags = 0;

	/*                      */
	if (ucTC != TC4_INDEX) {
		ucPortIdx = 0;
	} else {		/*                                  */
		ucPortIdx = 1;
	}

	/*                                                                         */
	kalMemCopy((PVOID) & pucOutputBuf[0],
		   (PVOID) prCmdInfo->pucInfoBuffer, prCmdInfo->u2InfoBufLen);

	ASSERT(u2OverallBufferLength <= prAdapter->u4CoalescingBufCachedSize);

	/*                              */
	HAL_WRITE_TX_PORT(prAdapter,
			  ucPortIdx,
			  (UINT_32) u2OverallBufferLength,
			  (PUINT_8) pucOutputBuf, (UINT_32) prAdapter->u4CoalescingBufCachedSize);

	return WLAN_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                                                
                              
 
                                                         
 
                                                           
*/
/*                                                                            */
WLAN_STATUS nicTxInitResetResource(IN P_ADAPTER_T prAdapter)
{
	P_TX_CTRL_T prTxCtrl;

	DEBUGFUNC("nicTxInitResetResource");

	ASSERT(prAdapter);
	prTxCtrl = &prAdapter->rTxCtrl;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC0_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC0;
	prTxCtrl->rTc.aucFreeBufferCount[TC0_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC0;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC1_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC1;
	prTxCtrl->rTc.aucFreeBufferCount[TC1_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC1;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC2_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC2;
	prTxCtrl->rTc.aucFreeBufferCount[TC2_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC2;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC3_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC3;
	prTxCtrl->rTc.aucFreeBufferCount[TC3_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC3;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC4_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC4;
	prTxCtrl->rTc.aucFreeBufferCount[TC4_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC4;

	prTxCtrl->rTc.aucMaxNumOfBuffer[TC5_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC5;
	prTxCtrl->rTc.aucFreeBufferCount[TC5_INDEX] = NIC_TX_INIT_BUFF_COUNT_TC5;

	return WLAN_STATUS_SUCCESS;

}

#endif


/*                                                                            */
/* 
                                                                  
                         
 
                                                         
                                       
 
                                                           
*/
/*                                                                            */
WLAN_STATUS nicTxEnqueueMsdu(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo)
{
	P_TX_CTRL_T prTxCtrl;
	P_MSDU_INFO_T prNextMsduInfo, prRetMsduInfo, prMsduInfoHead;
	QUE_T qDataPort0, qDataPort1;
	P_CMD_INFO_T prCmdInfo;
	WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);

	prTxCtrl = &prAdapter->rTxCtrl;
	ASSERT(prTxCtrl);

	QUEUE_INITIALIZE(&qDataPort0);
	QUEUE_INITIALIZE(&qDataPort1);

	/*                                                  */
	while (prMsduInfo) {
		prNextMsduInfo = (P_MSDU_INFO_T) QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T) prMsduInfo);

		QUEUE_GET_NEXT_ENTRY((P_QUE_ENTRY_T) prMsduInfo) = NULL;

		if (prMsduInfo->eSrc == TX_PACKET_MGMT) {
			/*                           */
			prMsduInfo->ucTC = TC4_INDEX;

			QUEUE_INSERT_TAIL(&qDataPort1, (P_QUE_ENTRY_T) prMsduInfo);
		} else {
			QUEUE_INSERT_TAIL(&qDataPort0, (P_QUE_ENTRY_T) prMsduInfo);
		}

		prMsduInfo = prNextMsduInfo;
	}

	if (qDataPort0.u4NumElem) {
		/*            */
		KAL_SPIN_LOCK_DECLARATION();
		KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);
		prRetMsduInfo =
		    qmEnqueueTxPackets(prAdapter, (P_MSDU_INFO_T) QUEUE_GET_HEAD(&qDataPort0));
		KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_QM_TX_QUEUE);

		/*                                  */
		if (prRetMsduInfo != NULL) {	/*                   */
			nicTxFreeMsduInfoPacket(prAdapter, prRetMsduInfo);
			nicTxReturnMsduInfo(prAdapter, prRetMsduInfo);
		}
	}

	if (qDataPort1.u4NumElem) {
		prMsduInfoHead = (P_MSDU_INFO_T) QUEUE_GET_HEAD(&qDataPort1);

		if (qDataPort1.u4NumElem > nicTxGetFreeCmdCount(prAdapter)) {
			/*                                    */
			u4Status = WLAN_STATUS_FAILURE;

			/*                */
			while (prMsduInfoHead) {
				prNextMsduInfo =
				    (P_MSDU_INFO_T) QUEUE_GET_NEXT_ENTRY(&prMsduInfoHead->
									 rQueEntry);

				if (prMsduInfoHead->pfTxDoneHandler != NULL) {
					prMsduInfoHead->pfTxDoneHandler(prAdapter, prMsduInfoHead,
									TX_RESULT_DROPPED_IN_DRIVER);
				}


				cnmMgtPktFree(prAdapter, prMsduInfoHead);

				prMsduInfoHead = prNextMsduInfo;
			}
		} else {
			/*                       */
			while (prMsduInfoHead) {
				prNextMsduInfo =
				    (P_MSDU_INFO_T) QUEUE_GET_NEXT_ENTRY(&prMsduInfoHead->
									 rQueEntry);

				KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);
				QUEUE_REMOVE_HEAD(&prAdapter->rFreeCmdList, prCmdInfo,
						  P_CMD_INFO_T);
				KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_CMD_RESOURCE);

				if (prCmdInfo) {
					GLUE_INC_REF_CNT(prTxCtrl->i4TxMgmtPendingNum);

					kalMemZero(prCmdInfo, sizeof(CMD_INFO_T));

					prCmdInfo->eCmdType = COMMAND_TYPE_MANAGEMENT_FRAME;
					prCmdInfo->u2InfoBufLen = prMsduInfoHead->u2FrameLength;
					prCmdInfo->pucInfoBuffer = NULL;
					prCmdInfo->prPacket = (P_NATIVE_PACKET) prMsduInfoHead;
					prCmdInfo->ucStaRecIndex = prMsduInfoHead->ucStaRecIndex;
					prCmdInfo->eNetworkType = prMsduInfoHead->ucNetworkType;
					prCmdInfo->pfCmdDoneHandler = NULL;
					prCmdInfo->pfCmdTimeoutHandler = NULL;
					prCmdInfo->fgIsOid = FALSE;
					prCmdInfo->fgSetQuery = TRUE;
					prCmdInfo->fgNeedResp = FALSE;

					kalEnqueueCommand(prAdapter->prGlueInfo,
							  (P_QUE_ENTRY_T) prCmdInfo);
				} else {
					/*                                                              */
					ASSERT(0);

					u4Status = WLAN_STATUS_FAILURE;
					cnmMgtPktFree(prAdapter, prMsduInfoHead);
				}

				prMsduInfoHead = prNextMsduInfo;
			}
		}
	}

	/*                                     */
	if (prTxCtrl->i4TxMgmtPendingNum > 0
	    || kalGetTxPendingFrameCount(prAdapter->prGlueInfo) > 0) {
		kalSetEvent(prAdapter->prGlueInfo);
	}

	return u4Status;
}


/*                                                                            */
/* 
                                                               
 
                                                         
 
         
*/
/*                                                                            */
UINT_32 nicTxGetFreeCmdCount(IN P_ADAPTER_T prAdapter)
{
	ASSERT(prAdapter);

	return prAdapter->rFreeCmdList.u4NumElem;
}
