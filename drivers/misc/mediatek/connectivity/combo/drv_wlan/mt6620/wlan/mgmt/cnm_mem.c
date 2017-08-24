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
static VOID
cnmStaRecHandleEventPkt(P_ADAPTER_T prAdapter, P_CMD_INFO_T prCmdInfo, PUINT_8 pucEventBuf);

static VOID cnmStaSendUpdateCmd(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec, BOOLEAN fgNeedResp);

static VOID cnmStaSendRemoveCmd(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec);

/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
P_MSDU_INFO_T cnmMgtPktAlloc(P_ADAPTER_T prAdapter, UINT_32 u4Length)
{
	P_MSDU_INFO_T prMsduInfo;
	P_QUE_T prQueList;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);
	prQueList = &prAdapter->rTxCtrl.rFreeMsduInfoList;

	/*                        */
	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
	QUEUE_REMOVE_HEAD(prQueList, prMsduInfo, P_MSDU_INFO_T);
	KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);

	if (prMsduInfo) {
		prMsduInfo->prPacket = cnmMemAlloc(prAdapter, RAM_TYPE_BUF, u4Length);
		prMsduInfo->eSrc = TX_PACKET_MGMT;

		if (prMsduInfo->prPacket == NULL) {
			KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
			QUEUE_INSERT_TAIL(prQueList, &prMsduInfo->rQueEntry);
			KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
			prMsduInfo = NULL;
		}
	}
#if DBG
	if (prMsduInfo == NULL) {
		DBGLOG(MEM, WARN, ("\n"));
		DBGLOG(MEM, WARN, ("MgtDesc#=%ld\n", prQueList->u4NumElem));

#if CFG_DBG_MGT_BUF
		DBGLOG(MEM, WARN, ("rMgtBufInfo: alloc#=%ld, free#=%ld, null#=%ld\n",
				   prAdapter->rMgtBufInfo.u4AllocCount,
				   prAdapter->rMgtBufInfo.u4FreeCount,
				   prAdapter->rMgtBufInfo.u4AllocNullCount));
#endif

		DBGLOG(MEM, WARN, ("\n"));
	}
#endif

	return prMsduInfo;
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID cnmMgtPktFree(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo)
{
	P_QUE_T prQueList;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);

	prQueList = &prAdapter->rTxCtrl.rFreeMsduInfoList;

	ASSERT(prMsduInfo->prPacket);
	if (prMsduInfo->prPacket) {
		cnmMemFree(prAdapter, prMsduInfo->prPacket);
		prMsduInfo->prPacket = NULL;
	}

	KAL_ACQUIRE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
	QUEUE_INSERT_TAIL(prQueList, &prMsduInfo->rQueEntry)
	    KAL_RELEASE_SPIN_LOCK(prAdapter, SPIN_LOCK_TX_MSDU_INFO_LIST);
}

/*                                                                            */
/* 
                                                                   
 
               
 
                
*/
/*                                                                            */
VOID cnmMemInit(P_ADAPTER_T prAdapter)
{
	P_BUF_INFO_T prBufInfo;

	/*                                   */
	prBufInfo = &prAdapter->rMgtBufInfo;
	kalMemZero(prBufInfo, sizeof(prAdapter->rMgtBufInfo));
	prBufInfo->pucBuf = prAdapter->pucMgtBufCached;

	/*                                                 */
	prBufInfo->rFreeBlocksBitmap = (BUF_BITMAP) BITS(0, MAX_NUM_OF_BUF_BLOCKS - 1);


	/*                                */
	prBufInfo = &prAdapter->rMsgBufInfo;
	kalMemZero(prBufInfo, sizeof(prAdapter->rMsgBufInfo));
	prBufInfo->pucBuf = &prAdapter->aucMsgBuf[0];

	/*                                                 */
	prBufInfo->rFreeBlocksBitmap = (BUF_BITMAP) BITS(0, MAX_NUM_OF_BUF_BLOCKS - 1);

	return;

}				/*                     */


/*                                                                            */
/* 
                                       
 
                                            
                                                                     
                                                             
 
                                                                    
                                         
*/
/*                                                                            */
PVOID cnmMemAlloc(IN P_ADAPTER_T prAdapter, IN ENUM_RAM_TYPE_T eRamType, IN UINT_32 u4Length)
{
	P_BUF_INFO_T prBufInfo;
	BUF_BITMAP rRequiredBitmap;
	UINT_32 u4BlockNum;
	UINT_32 i, u4BlkSzInPower;
	PVOID pvMemory;
	KAL_SPIN_LOCK_DECLARATION();

	ASSERT(prAdapter);
	ASSERT(u4Length);

	if (eRamType == RAM_TYPE_MSG && u4Length <= 256) {
		prBufInfo = &prAdapter->rMsgBufInfo;
		u4BlkSzInPower = MSG_BUF_BLOCK_SIZE_IN_POWER_OF_2;

		u4Length += (MSG_BUF_BLOCK_SIZE - 1);
		u4BlockNum = u4Length >> MSG_BUF_BLOCK_SIZE_IN_POWER_OF_2;

		ASSERT(u4BlockNum <= MAX_NUM_OF_BUF_BLOCKS);
	} else {
		eRamType = RAM_TYPE_BUF;

		prBufInfo = &prAdapter->rMgtBufInfo;
		u4BlkSzInPower = MGT_BUF_BLOCK_SIZE_IN_POWER_OF_2;

		u4Length += (MGT_BUF_BLOCK_SIZE - 1);
		u4BlockNum = u4Length >> MGT_BUF_BLOCK_SIZE_IN_POWER_OF_2;

		ASSERT(u4BlockNum <= MAX_NUM_OF_BUF_BLOCKS);
	}

#if CFG_DBG_MGT_BUF
	prBufInfo->u4AllocCount++;
#endif

	KAL_ACQUIRE_SPIN_LOCK(prAdapter,
			      eRamType == RAM_TYPE_MSG ? SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

	if ((u4BlockNum > 0) && (u4BlockNum <= MAX_NUM_OF_BUF_BLOCKS)) {

		/*                                          */
		rRequiredBitmap = BITS(0, u4BlockNum - 1);

		for (i = 0; i <= (MAX_NUM_OF_BUF_BLOCKS - u4BlockNum); i++) {

			/*                              */
			if ((prBufInfo->rFreeBlocksBitmap & rRequiredBitmap)
			    == rRequiredBitmap) {

				/*                                                     */
				prBufInfo->rFreeBlocksBitmap &= ~rRequiredBitmap;

				/*                                    */
				prBufInfo->aucAllocatedBlockNum[i] = (UINT_8) u4BlockNum;

				KAL_RELEASE_SPIN_LOCK(prAdapter,
						      eRamType == RAM_TYPE_MSG ?
						      SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

				/*                                              */
				return (PVOID) (prBufInfo->pucBuf + (i << u4BlkSzInPower));

			}

			rRequiredBitmap <<= 1;
		}
	}
#ifdef LINUX
	pvMemory = (PVOID) kalMemAlloc(u4Length, VIR_MEM_TYPE);
#else
	pvMemory = (PVOID) NULL;
#endif

#if CFG_DBG_MGT_BUF
	prBufInfo->u4AllocNullCount++;

	if (pvMemory) {
		prAdapter->u4MemAllocDynamicCount++;
	}
#endif

	KAL_RELEASE_SPIN_LOCK(prAdapter,
			      eRamType == RAM_TYPE_MSG ? SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

	return pvMemory;

}				/*                      */


/*                                                                            */
/* 
                                               
 
                                                              
 
                
*/
/*                                                                            */
VOID cnmMemFree(IN P_ADAPTER_T prAdapter, IN PVOID pvMemory)
{
	P_BUF_INFO_T prBufInfo;
	UINT_32 u4BlockIndex;
	BUF_BITMAP rAllocatedBlocksBitmap;
	ENUM_RAM_TYPE_T eRamType;
	KAL_SPIN_LOCK_DECLARATION();


	ASSERT(prAdapter);
	ASSERT(pvMemory);
	if (!pvMemory) {
		return;
	}

	/*                                    */
	if (((UINT_32) pvMemory >= (UINT_32) &prAdapter->aucMsgBuf[0]) &&
	    ((UINT_32) pvMemory <= (UINT_32) &prAdapter->aucMsgBuf[MSG_BUFFER_SIZE - 1])) {

		prBufInfo = &prAdapter->rMsgBufInfo;
		u4BlockIndex = ((UINT_32) pvMemory - (UINT_32) prBufInfo->pucBuf)
		    >> MSG_BUF_BLOCK_SIZE_IN_POWER_OF_2;
		ASSERT(u4BlockIndex < MAX_NUM_OF_BUF_BLOCKS);
		eRamType = RAM_TYPE_MSG;
	} else if (((UINT_32) pvMemory >= (UINT_32) prAdapter->pucMgtBufCached) &&
		   ((UINT_32) pvMemory <=
		    ((UINT_32) prAdapter->pucMgtBufCached + MGT_BUFFER_SIZE - 1))) {
		prBufInfo = &prAdapter->rMgtBufInfo;
		u4BlockIndex = ((UINT_32) pvMemory - (UINT_32) prBufInfo->pucBuf)
		    >> MGT_BUF_BLOCK_SIZE_IN_POWER_OF_2;
		ASSERT(u4BlockIndex < MAX_NUM_OF_BUF_BLOCKS);
		eRamType = RAM_TYPE_BUF;
	} else {
#ifdef LINUX
		/*                                                       */
		kalMemFree(pvMemory, VIR_MEM_TYPE, 0);
#else
		/*                                                              */
		ASSERT(0);
#endif

#if CFG_DBG_MGT_BUF
		prAdapter->u4MemFreeDynamicCount++;
#endif
		return;
	}

	KAL_ACQUIRE_SPIN_LOCK(prAdapter,
			      eRamType == RAM_TYPE_MSG ? SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

#if CFG_DBG_MGT_BUF
	prBufInfo->u4FreeCount++;
#endif

	/*                                          */
	ASSERT(prBufInfo->aucAllocatedBlockNum[u4BlockIndex] > 0);

	rAllocatedBlocksBitmap = BITS(0, prBufInfo->aucAllocatedBlockNum[u4BlockIndex] - 1);
	rAllocatedBlocksBitmap <<= u4BlockIndex;

	/*                                                 */
	prBufInfo->aucAllocatedBlockNum[u4BlockIndex] = 0;

	/*                                                */
	prBufInfo->rFreeBlocksBitmap |= rAllocatedBlocksBitmap;

	KAL_RELEASE_SPIN_LOCK(prAdapter,
			      eRamType == RAM_TYPE_MSG ? SPIN_LOCK_MSG_BUF : SPIN_LOCK_MGT_BUF);

	return;

}				/*                     */


/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID cnmStaRecInit(P_ADAPTER_T prAdapter)
{
	P_STA_RECORD_T prStaRec;
	UINT_16 i;

	for (i = 0; i < CFG_STA_REC_NUM; i++) {
		prStaRec = &prAdapter->arStaRec[i];

		prStaRec->ucIndex = (UINT_8) i;
		prStaRec->fgIsInUse = FALSE;
	}
}


/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID cnmStaRecUninit(IN P_ADAPTER_T prAdapter)
{
	P_STA_RECORD_T prStaRec;
	UINT_16 i;

	for (i = 0; i < CFG_STA_REC_NUM; i++) {
		prStaRec = &prAdapter->arStaRec[i];

		if (prStaRec->fgIsInUse) {
			cnmStaRecFree(prAdapter, prStaRec, FALSE);
		}
	}
}


/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
P_STA_RECORD_T cnmStaRecAlloc(P_ADAPTER_T prAdapter, UINT_8 ucNetTypeIndex)
{
	P_STA_RECORD_T prStaRec;
	UINT_16 i, k;

	ASSERT(prAdapter);

	for (i = 0; i < CFG_STA_REC_NUM; i++) {
		prStaRec = &prAdapter->arStaRec[i];

		if (!prStaRec->fgIsInUse) {
	    /*                                   */
			kalMemZero(prStaRec, sizeof(STA_RECORD_T));
			prStaRec->ucIndex = (UINT_8) i;
			prStaRec->ucNetTypeIndex = ucNetTypeIndex;
			prStaRec->fgIsInUse = TRUE;

			if (prStaRec->pucAssocReqIe) {
				kalMemFree(prStaRec->pucAssocReqIe, VIR_MEM_TYPE,
					   prStaRec->u2AssocReqIeLen);
				prStaRec->pucAssocReqIe = NULL;
				prStaRec->u2AssocReqIeLen = 0;
			}

			/*                                                  */
			for (k = 0; k < TID_NUM + 1; k++) {
				prStaRec->au2CachedSeqCtrl[k] = 0xFFFF;
			}

			/*                                    */
			for (k = 0; k < STA_WAIT_QUEUE_NUM; k++) {
				LINK_INITIALIZE(&prStaRec->arStaWaitQueue[k]);
			}

			/*                            */
			prStaRec->fgTxAmpduEn = TRUE;
			prStaRec->fgRxAmpduEn = TRUE;

			for (k = 0; k < NUM_OF_PER_STA_TX_QUEUES; k++) {
				QUEUE_INITIALIZE(&prStaRec->arTxQueue[k]);
			}

			break;
		}
	}

	return (i < CFG_STA_REC_NUM) ? prStaRec : NULL;
}


/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID cnmStaRecFree(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec, BOOLEAN fgSyncToChip)
{
	ASSERT(prAdapter);
	ASSERT(prStaRec);

	/*                                                          */
	cnmTimerStopTimer(prAdapter, &prStaRec->rTxReqDoneOrRxRespTimer);
	prStaRec->fgTransmitKeyExist = FALSE;
	prStaRec->fgSetPwrMgtBit = FALSE;

	if (prStaRec->pucAssocReqIe) {
		kalMemFree(prStaRec->pucAssocReqIe, VIR_MEM_TYPE, prStaRec->u2AssocReqIeLen);
		prStaRec->pucAssocReqIe = NULL;
		prStaRec->u2AssocReqIeLen = 0;
	}

	qmDeactivateStaRec(prAdapter, prStaRec->ucIndex);

	if (fgSyncToChip) {
		cnmStaSendRemoveCmd(prAdapter, prStaRec);
	}

	prStaRec->fgIsInUse = FALSE;

	return;
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID
cnmStaFreeAllStaByNetType(P_ADAPTER_T prAdapter,
			  ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex, BOOLEAN fgSyncToChip)
{
	P_STA_RECORD_T prStaRec;
	UINT_16 i;

	for (i = 0; i < CFG_STA_REC_NUM; i++) {
		prStaRec = (P_STA_RECORD_T) &prAdapter->arStaRec[i];

		if (prStaRec->fgIsInUse && prStaRec->ucNetTypeIndex == (UINT_8) eNetTypeIndex) {

			cnmStaRecFree(prAdapter, prStaRec, fgSyncToChip);
		}
	}			/*                 */
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
P_STA_RECORD_T cnmGetStaRecByIndex(P_ADAPTER_T prAdapter, UINT_8 ucIndex)
{
	P_STA_RECORD_T prStaRec;

	ASSERT(prAdapter);

	prStaRec = (ucIndex < CFG_STA_REC_NUM) ? &prAdapter->arStaRec[ucIndex] : NULL;

	if (prStaRec && prStaRec->fgIsInUse == FALSE) {
		prStaRec = NULL;
	}

	return prStaRec;
}

/*                                                                            */
/* 
                                                          
 
                                                        
 
                                                                 
*/
/*                                                                            */
P_STA_RECORD_T
cnmGetStaRecByAddress(P_ADAPTER_T prAdapter, UINT_8 ucNetTypeIndex, PUINT_8 pucPeerMacAddr)
{
	P_STA_RECORD_T prStaRec;
	UINT_16 i;

	ASSERT(prAdapter);
	ASSERT(pucPeerMacAddr);

	for (i = 0; i < CFG_STA_REC_NUM; i++) {
		prStaRec = &prAdapter->arStaRec[i];

		if (prStaRec->fgIsInUse &&
		    prStaRec->ucNetTypeIndex == ucNetTypeIndex &&
		    EQUAL_MAC_ADDR(prStaRec->aucMacAddr, pucPeerMacAddr)) {
			break;
		}
	}

	return (i < CFG_STA_REC_NUM) ? prStaRec : NULL;
}

/*                                                                            */
/* 
                                                                               
                                   
 
                                                
 
                  
*/
/*                                                                            */
VOID cnmStaRecResetStatus(P_ADAPTER_T prAdapter, ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex)
{
	cnmStaFreeAllStaByNetType(prAdapter, eNetTypeIndex, FALSE);

#if 0
	P_STA_RECORD_T prStaRec;
	UINT_16 i;

	ASSERT(prAdapter);

	for (i = 0; i < CFG_STA_REC_NUM; i++) {
		prStaRec = &prAdapter->arStaRec[i];

		if (prStaRec->fgIsInUse) {
			if ((NETWORK_TYPE_AIS_INDEX == eNetTypeIndex) &&
			    IS_STA_IN_AIS(prStaRec->eStaType)) {

				prStaRec->u2StatusCode = STATUS_CODE_SUCCESSFUL;
				prStaRec->u2ReasonCode = REASON_CODE_RESERVED;
				prStaRec->ucJoinFailureCount = 0;
				prStaRec->fgTransmitKeyExist = FALSE;

				prStaRec->fgSetPwrMgtBit = FALSE;
			}

			/*                              */
		}
	}

	return;
#endif
}

/*                                                                            */
/* 
                                                                             
                                                                     
 
                                                       
                                                
 
                
*/
/*                                                                            */
VOID cnmStaRecChangeState(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec, UINT_8 ucNewState)
{
	BOOLEAN fgNeedResp;

	ASSERT(prAdapter);
	ASSERT(prStaRec);
	ASSERT(prStaRec->fgIsInUse);

	/*                                                    
                                                                   
  */
	if ((ucNewState == STA_STATE_2 && prStaRec->ucStaState != STA_STATE_3) ||
	    (ucNewState == STA_STATE_1 && prStaRec->ucStaState == STA_STATE_2)) {
		prStaRec->ucStaState = ucNewState;
		return;
	}

	fgNeedResp = FALSE;
	if (ucNewState == STA_STATE_3) {
		secFsmEventStart(prAdapter, prStaRec);
		if (ucNewState != prStaRec->ucStaState) {
			fgNeedResp = TRUE;
		}
	} else {
		if (ucNewState != prStaRec->ucStaState && prStaRec->ucStaState == STA_STATE_3) {
			qmDeactivateStaRec(prAdapter, prStaRec->ucIndex);
		}
		fgNeedResp = FALSE;
	}
	prStaRec->ucStaState = ucNewState;

	cnmStaSendUpdateCmd(prAdapter, prStaRec, fgNeedResp);

#if CFG_ENABLE_WIFI_DIRECT
	/*                                                                      
                                                 
                                                  
  */
	if (prAdapter->fgIsP2PRegistered && (IS_STA_IN_P2P(prStaRec))) {
		P_BSS_INFO_T prBssInfo;

		prBssInfo = &prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex];

		if (prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT) {
			rlmUpdateParamsForAP(prAdapter, prBssInfo, FALSE);
		}
	}
#endif
	return;
}

/*                                                                            */
/* 
        
 
            
 
                
*/
/*                                                                            */
static VOID
cnmStaRecHandleEventPkt(P_ADAPTER_T prAdapter, P_CMD_INFO_T prCmdInfo, PUINT_8 pucEventBuf)
{
	P_EVENT_ACTIVATE_STA_REC_T prEventContent;
	P_STA_RECORD_T prStaRec;

	prEventContent = (P_EVENT_ACTIVATE_STA_REC_T) pucEventBuf;
	prStaRec = cnmGetStaRecByIndex(prAdapter, prEventContent->ucStaRecIdx);

	if (prStaRec && prStaRec->ucStaState == STA_STATE_3 &&
	    !kalMemCmp(&prStaRec->aucMacAddr[0], &prEventContent->aucMacAddr[0], MAC_ADDR_LEN)) {

		qmActivateStaRec(prAdapter, prStaRec);
	}

}

/*                                                                            */
/* 
        
 
            
 
                
*/
/*                                                                            */
static VOID cnmStaSendUpdateCmd(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec, BOOLEAN fgNeedResp)
{
	P_CMD_UPDATE_STA_RECORD_T prCmdContent;
	WLAN_STATUS rStatus;

	ASSERT(prAdapter);
	ASSERT(prStaRec);
	ASSERT(prStaRec->fgIsInUse);

	/*                                                                       
                                                                   
                          
  */

	/*                                                            
                                                          
  */
	prCmdContent = cnmMemAlloc(prAdapter, RAM_TYPE_BUF, sizeof(CMD_UPDATE_STA_RECORD_T));
	ASSERT(prCmdContent);

	/*                         */
	if (!prCmdContent) {
		return;
	}

	prCmdContent->ucIndex = prStaRec->ucIndex;
	prCmdContent->ucStaType = (UINT_8) prStaRec->eStaType;
	kalMemCopy(&prCmdContent->aucMacAddr[0], &prStaRec->aucMacAddr[0], MAC_ADDR_LEN);
	prCmdContent->u2AssocId = prStaRec->u2AssocId;
	prCmdContent->u2ListenInterval = prStaRec->u2ListenInterval;
	prCmdContent->ucNetTypeIndex = prStaRec->ucNetTypeIndex;

	prCmdContent->ucDesiredPhyTypeSet = prStaRec->ucDesiredPhyTypeSet;
	prCmdContent->u2DesiredNonHTRateSet = prStaRec->u2DesiredNonHTRateSet;
	prCmdContent->u2BSSBasicRateSet = prStaRec->u2BSSBasicRateSet;
	prCmdContent->ucMcsSet = prStaRec->ucMcsSet;
	prCmdContent->ucSupMcs32 = (UINT_8) prStaRec->fgSupMcs32;
	prCmdContent->u2HtCapInfo = prStaRec->u2HtCapInfo;
	prCmdContent->ucNeedResp = (UINT_8) fgNeedResp;

#if !CFG_SLT_SUPPORT
	if (prAdapter->rWifiVar.eRateSetting != FIXED_RATE_NONE) {
		/*                             */
		nicUpdateRateParams(prAdapter,
				    prAdapter->rWifiVar.eRateSetting,
				    &(prCmdContent->ucDesiredPhyTypeSet),
				    &(prCmdContent->u2DesiredNonHTRateSet),
				    &(prCmdContent->u2BSSBasicRateSet),
				    &(prCmdContent->ucMcsSet),
				    &(prCmdContent->ucSupMcs32), &(prCmdContent->u2HtCapInfo));
	}
#endif

	prCmdContent->ucIsQoS = prStaRec->fgIsQoS;
	prCmdContent->ucIsUapsdSupported = prStaRec->fgIsUapsdSupported;
	prCmdContent->ucStaState = prStaRec->ucStaState;

	prCmdContent->ucAmpduParam = prStaRec->ucAmpduParam;
	prCmdContent->u2HtExtendedCap = prStaRec->u2HtExtendedCap;
	prCmdContent->u4TxBeamformingCap = prStaRec->u4TxBeamformingCap;
	prCmdContent->ucAselCap = prStaRec->ucAselCap;
	prCmdContent->ucRCPI = prStaRec->ucRCPI;

	prCmdContent->ucUapsdAc = prStaRec->ucBmpTriggerAC | (prStaRec->ucBmpDeliveryAC << 4);
	prCmdContent->ucUapsdSp = prStaRec->ucUapsdSp;

	rStatus = wlanSendSetQueryCmd(prAdapter,	/*           */
				      CMD_ID_UPDATE_STA_RECORD,	/*       */
				      TRUE,	/*            */
				      fgNeedResp,	/*            */
				      FALSE,	/*         */
				      fgNeedResp ? cnmStaRecHandleEventPkt : NULL, NULL,	/*                     */
				      sizeof(CMD_UPDATE_STA_RECORD_T),	/*                   */
				      (PUINT_8) prCmdContent,	/*               */
				      NULL,	/*                  */
				      0	/*                     */
	    );

	ASSERT(rStatus == WLAN_STATUS_PENDING);

	cnmMemFree(prAdapter, prCmdContent);
}

/*                                                                            */
/* 
        
 
            
 
                
*/
/*                                                                            */
static VOID cnmStaSendRemoveCmd(P_ADAPTER_T prAdapter, P_STA_RECORD_T prStaRec)
{
	CMD_REMOVE_STA_RECORD_T rCmdContent;
	WLAN_STATUS rStatus;

	ASSERT(prAdapter);
	ASSERT(prStaRec);

	rCmdContent.ucIndex = prStaRec->ucIndex;
	kalMemCopy(&rCmdContent.aucMacAddr[0], &prStaRec->aucMacAddr[0], MAC_ADDR_LEN);

	rStatus = wlanSendSetQueryCmd(prAdapter,	/*           */
				      CMD_ID_REMOVE_STA_RECORD,	/*       */
				      TRUE,	/*            */
				      FALSE,	/*            */
				      FALSE,	/*         */
				      NULL,	/*                  */
				      NULL,	/*                     */
				      sizeof(CMD_REMOVE_STA_RECORD_T),	/*                   */
				      (PUINT_8) &rCmdContent,	/*               */
				      NULL,	/*                  */
				      0	/*                     */
	    );

	ASSERT(rStatus == WLAN_STATUS_PENDING);
}
