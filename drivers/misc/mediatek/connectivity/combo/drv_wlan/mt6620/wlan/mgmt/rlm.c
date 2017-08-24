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
static VOID rlmFillHtCapIE(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, P_MSDU_INFO_T prMsduInfo);

static VOID
rlmFillExtCapIE(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, P_MSDU_INFO_T prMsduInfo);

static VOID rlmFillHtOpIE(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, P_MSDU_INFO_T prMsduInfo);

static UINT_8
rlmRecIeInfoForClient(P_ADAPTER_T prAdapter,
		      P_BSS_INFO_T prBssInfo, PUINT_8 pucIE, UINT_16 u2IELength);

static BOOLEAN
rlmRecBcnFromNeighborForClient(P_ADAPTER_T prAdapter,
			       P_BSS_INFO_T prBssInfo,
			       P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength);

static BOOLEAN
rlmRecBcnInfoForClient(P_ADAPTER_T prAdapter,
		       P_BSS_INFO_T prBssInfo,
		       P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength);

static VOID rlmBssReset(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmFsmEventInit(P_ADAPTER_T prAdapter)
{
	ASSERT(prAdapter);

	/*                                                             
                                  
  */

	/*                     */
	rlmObssInit(prAdapter);
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmFsmEventUninit(P_ADAPTER_T prAdapter)
{
	P_BSS_INFO_T prBssInfo;
	UINT_8 ucNetIdx;

	ASSERT(prAdapter);

	RLM_NET_FOR_EACH(ucNetIdx) {
		prBssInfo = &prAdapter->rWifiVar.arBssInfo[ucNetIdx];
		ASSERT(prBssInfo);

		/*                                           
                                        
   */
		rlmBssReset(prAdapter, prBssInfo);
	}
}

/*                                                                            */
/* 
                                               
 
            
 
              
*/
/*                                                                            */
VOID rlmReqGenerateHtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo)
{
	P_BSS_INFO_T prBssInfo;
	P_STA_RECORD_T prStaRec;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);

	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prMsduInfo->ucNetworkType];
	ASSERT(prBssInfo);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	if ((prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11N) &&
	    (!prStaRec || (prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N))) {

		rlmFillHtCapIE(prAdapter, prBssInfo, prMsduInfo);
	}
}

/*                                                                            */
/* 
                                               
 
            
 
              
*/
/*                                                                            */
VOID rlmReqGenerateExtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo)
{
	P_BSS_INFO_T prBssInfo;
	P_STA_RECORD_T prStaRec;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);

	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prMsduInfo->ucNetworkType];
	ASSERT(prBssInfo);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	if ((prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11N) &&
	    (!prStaRec || (prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N))) {

		rlmFillExtCapIE(prAdapter, prBssInfo, prMsduInfo);
	}
}

/*                                                                            */
/* 
                                                               
 
            
 
              
*/
/*                                                                            */
VOID rlmRspGenerateHtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo)
{
	P_BSS_INFO_T prBssInfo;
	P_STA_RECORD_T prStaRec;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);
	ASSERT(IS_NET_ACTIVE(prAdapter, prMsduInfo->ucNetworkType));

	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prMsduInfo->ucNetworkType];
	ASSERT(prBssInfo);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	if (RLM_NET_IS_11N(prBssInfo) &&
	    (!prStaRec || (prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N))) {

		rlmFillHtCapIE(prAdapter, prBssInfo, prMsduInfo);
	}
}

/*                                                                            */
/* 
                                                               
 
            
 
              
*/
/*                                                                            */
VOID rlmRspGenerateExtCapIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo)
{
	P_BSS_INFO_T prBssInfo;
	P_STA_RECORD_T prStaRec;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);
	ASSERT(IS_NET_ACTIVE(prAdapter, prMsduInfo->ucNetworkType));

	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prMsduInfo->ucNetworkType];
	ASSERT(prBssInfo);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	if (RLM_NET_IS_11N(prBssInfo) &&
	    (!prStaRec || (prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N))) {

		rlmFillExtCapIE(prAdapter, prBssInfo, prMsduInfo);
	}
}

/*                                                                            */
/* 
                                                               
 
            
 
              
*/
/*                                                                            */
VOID rlmRspGenerateHtOpIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo)
{
	P_BSS_INFO_T prBssInfo;
	P_STA_RECORD_T prStaRec;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);
	ASSERT(IS_NET_ACTIVE(prAdapter, prMsduInfo->ucNetworkType));

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prMsduInfo->ucNetworkType];
	ASSERT(prBssInfo);

	if (RLM_NET_IS_11N(prBssInfo) &&
	    (!prStaRec || (prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N))) {

		rlmFillHtOpIE(prAdapter, prBssInfo, prMsduInfo);
	}
}

/*                                                                            */
/* 
                                                               
 
            
 
              
*/
/*                                                                            */
VOID rlmRspGenerateErpIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo)
{
	P_BSS_INFO_T prBssInfo;
	P_STA_RECORD_T prStaRec;
	P_IE_ERP_T prErpIe;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);
	ASSERT(IS_NET_ACTIVE(prAdapter, prMsduInfo->ucNetworkType));

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prMsduInfo->ucNetworkType];
	ASSERT(prBssInfo);

	if (RLM_NET_IS_11GN(prBssInfo) && prBssInfo->eBand == BAND_2G4 &&
	    (!prStaRec || (prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11GN))) {
		prErpIe = (P_IE_ERP_T)
		    (((PUINT_8) prMsduInfo->prPacket) + prMsduInfo->u2FrameLength);

		/*            */
		prErpIe->ucId = ELEM_ID_ERP_INFO;
		prErpIe->ucLength = 1;

		prErpIe->ucERP = prBssInfo->fgObssErpProtectMode ? ERP_INFO_USE_PROTECTION : 0;

		if (prBssInfo->fgErpProtectMode) {
			prErpIe->ucERP |= (ERP_INFO_NON_ERP_PRESENT | ERP_INFO_USE_PROTECTION);
		}

		/*                        */
		if (!prBssInfo->fgUseShortPreamble) {
			prErpIe->ucERP |= ERP_INFO_BARKER_PREAMBLE_MODE;
		}

		ASSERT(IE_SIZE(prErpIe) <= (ELEM_HDR_LEN + ELEM_MAX_LEN_ERP));

		prMsduInfo->u2FrameLength += IE_SIZE(prErpIe);
	}
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
static VOID rlmFillHtCapIE(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, P_MSDU_INFO_T prMsduInfo)
{
	P_IE_HT_CAP_T prHtCap;
	P_SUP_MCS_SET_FIELD prSupMcsSet;
	BOOLEAN fg40mAllowed;

	ASSERT(prAdapter);
	ASSERT(prBssInfo);
	ASSERT(prMsduInfo);

	fg40mAllowed = prBssInfo->fgAssoc40mBwAllowed;

	prHtCap = (P_IE_HT_CAP_T)
	    (((PUINT_8) prMsduInfo->prPacket) + prMsduInfo->u2FrameLength);

	/*                        */
	prHtCap->ucId = ELEM_ID_HT_CAP;
	prHtCap->ucLength = sizeof(IE_HT_CAP_T) - ELEM_HDR_LEN;

	prHtCap->u2HtCapInfo = HT_CAP_INFO_DEFAULT_VAL;
	if (!fg40mAllowed) {
		prHtCap->u2HtCapInfo &= ~(HT_CAP_INFO_SUP_CHNL_WIDTH |
					  HT_CAP_INFO_SHORT_GI_40M | HT_CAP_INFO_DSSS_CCK_IN_40M);
	}
	if (prAdapter->rWifiVar.rConnSettings.fgRxShortGIDisabled) {
		prHtCap->u2HtCapInfo &= ~(HT_CAP_INFO_SHORT_GI_20M | HT_CAP_INFO_SHORT_GI_40M);
	}

	prHtCap->ucAmpduParam = AMPDU_PARAM_DEFAULT_VAL;

	prSupMcsSet = &prHtCap->rSupMcsSet;
	kalMemZero((PVOID) & prSupMcsSet->aucRxMcsBitmask[0], SUP_MCS_RX_BITMASK_OCTET_NUM);

	prSupMcsSet->aucRxMcsBitmask[0] = BITS(0, 7);

	if (fg40mAllowed) {
		prSupMcsSet->aucRxMcsBitmask[32 / 8] = BIT(0);	/*       */
	}
	prSupMcsSet->u2RxHighestSupportedRate = SUP_MCS_RX_DEFAULT_HIGHEST_RATE;
	prSupMcsSet->u4TxRateInfo = SUP_MCS_TX_DEFAULT_VAL;

	prHtCap->u2HtExtendedCap = HT_EXT_CAP_DEFAULT_VAL;
	if (!fg40mAllowed || prBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE) {
		prHtCap->u2HtExtendedCap &= ~(HT_EXT_CAP_PCO | HT_EXT_CAP_PCO_TRANS_TIME_NONE);
	}

	prHtCap->u4TxBeamformingCap = TX_BEAMFORMING_CAP_DEFAULT_VAL;

	prHtCap->ucAselCap = ASEL_CAP_DEFAULT_VAL;


	ASSERT(IE_SIZE(prHtCap) <= (ELEM_HDR_LEN + ELEM_MAX_LEN_HT_CAP));

	prMsduInfo->u2FrameLength += IE_SIZE(prHtCap);
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
static VOID rlmFillExtCapIE(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, P_MSDU_INFO_T prMsduInfo)
{
	P_EXT_CAP_T prExtCap;
	BOOLEAN fg40mAllowed;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);

	fg40mAllowed = prBssInfo->fgAssoc40mBwAllowed;

	/*                              */
	prExtCap = (P_EXT_CAP_T)
	    (((PUINT_8) prMsduInfo->prPacket) + prMsduInfo->u2FrameLength);

	prExtCap->ucId = ELEM_ID_EXTENDED_CAP;
	prExtCap->ucLength = 1;
	prExtCap->aucCapabilities[0] = ELEM_EXT_CAP_DEFAULT_VAL;

	if (!fg40mAllowed) {
		prExtCap->aucCapabilities[0] &= ~ELEM_EXT_CAP_20_40_COEXIST_SUPPORT;
	}

	if (prBssInfo->eCurrentOPMode != OP_MODE_INFRASTRUCTURE) {
		prExtCap->aucCapabilities[0] &= ~ELEM_EXT_CAP_PSMP_CAP;
	}

	ASSERT(IE_SIZE(prExtCap) <= (ELEM_HDR_LEN + ELEM_MAX_LEN_EXT_CAP));

	prMsduInfo->u2FrameLength += IE_SIZE(prExtCap);
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
static VOID rlmFillHtOpIE(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, P_MSDU_INFO_T prMsduInfo)
{
	P_IE_HT_OP_T prHtOp;
	UINT_16 i;

	ASSERT(prAdapter);
	ASSERT(prBssInfo);
	ASSERT(prMsduInfo);

	prHtOp = (P_IE_HT_OP_T)
	    (((PUINT_8) prMsduInfo->prPacket) + prMsduInfo->u2FrameLength);

	/*                     */
	prHtOp->ucId = ELEM_ID_HT_OP;
	prHtOp->ucLength = sizeof(IE_HT_OP_T) - ELEM_HDR_LEN;

	/*                                                  */
	prHtOp->ucPrimaryChannel = prBssInfo->ucPrimaryChannel;
	prHtOp->ucInfo1 = prBssInfo->ucHtOpInfo1;

	/*                                 */
	if (prBssInfo->eHtProtectMode == HT_PROTECT_MODE_NON_HT) {
		prHtOp->u2Info2 = (UINT_8) HT_PROTECT_MODE_NON_HT;
	} else if (prBssInfo->eObssHtProtectMode == HT_PROTECT_MODE_NON_MEMBER) {
		prHtOp->u2Info2 = (UINT_8) HT_PROTECT_MODE_NON_MEMBER;
	} else {
		/*                                                         */
		prHtOp->u2Info2 = (UINT_8) prBssInfo->eHtProtectMode;
	}

	if (prBssInfo->eGfOperationMode != GF_MODE_NORMAL) {
		/*                                                
                                                
   */
		prHtOp->u2Info2 |= HT_OP_INFO2_NON_GF_HT_STA_PRESENT;
	}

	if (0 /*                     */  &&
	    prBssInfo->eObssHtProtectMode == HT_PROTECT_MODE_NON_MEMBER) {
		/*                                                              
                                                           
   */
		prHtOp->u2Info2 |= HT_OP_INFO2_OBSS_NON_HT_STA_PRESENT;
	}

	prHtOp->u2Info3 = prBssInfo->u2HtOpInfo3;	/*                          */

	/*                                      */
	for (i = 0; i < 16; i++) {
		prHtOp->aucBasicMcsSet[i] = 0;
	}

	ASSERT(IE_SIZE(prHtOp) <= (ELEM_HDR_LEN + ELEM_MAX_LEN_HT_OP));

	prMsduInfo->u2FrameLength += IE_SIZE(prHtOp);
}

/*                                                                            */
/* 
                                                                               
                                          
 
            
 
              
*/
/*                                                                            */
static UINT_8
rlmRecIeInfoForClient(P_ADAPTER_T prAdapter,
		      P_BSS_INFO_T prBssInfo, PUINT_8 pucIE, UINT_16 u2IELength)
{
	UINT_16 u2Offset;
	P_STA_RECORD_T prStaRec;
	P_IE_HT_CAP_T prHtCap;
	P_IE_HT_OP_T prHtOp;
	P_IE_OBSS_SCAN_PARAM_T prObssScnParam;
	UINT_8 ucERP, ucPrimaryChannel;
#if CFG_SUPPORT_QUIET && 0
	BOOLEAN fgHasQuietIE = FALSE;
#endif

	ASSERT(prAdapter);
	ASSERT(prBssInfo);
	ASSERT(pucIE);

	prStaRec = prBssInfo->prStaRecOfAP;
	ASSERT(prStaRec);
	if (!prStaRec) {
		return 0;
	}

	prBssInfo->fgUseShortPreamble = prBssInfo->fgIsShortPreambleAllowed;
	ucPrimaryChannel = 0;
	prObssScnParam = NULL;

	/*                                                              
                                                                  
                                                                           
                                                          
  */
	IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
		switch (IE_ID(pucIE)) {
		case ELEM_ID_HT_CAP:
			if (!RLM_NET_IS_11N(prBssInfo) ||
			    IE_LEN(pucIE) != (sizeof(IE_HT_CAP_T) - 2)) {
				break;
			}
			prHtCap = (P_IE_HT_CAP_T) pucIE;
			prStaRec->ucMcsSet = prHtCap->rSupMcsSet.aucRxMcsBitmask[0];
			prStaRec->fgSupMcs32 =
			    (prHtCap->rSupMcsSet.aucRxMcsBitmask[32 / 8] & BIT(0)) ? TRUE : FALSE;

			prStaRec->u2HtCapInfo = prHtCap->u2HtCapInfo;
			prStaRec->ucAmpduParam = prHtCap->ucAmpduParam;
			prStaRec->u2HtExtendedCap = prHtCap->u2HtExtendedCap;
			prStaRec->u4TxBeamformingCap = prHtCap->u4TxBeamformingCap;
			prStaRec->ucAselCap = prHtCap->ucAselCap;
			break;

		case ELEM_ID_HT_OP:
			if (!RLM_NET_IS_11N(prBssInfo) || IE_LEN(pucIE) != (sizeof(IE_HT_OP_T) - 2)) {
				break;
			}
			prHtOp = (P_IE_HT_OP_T) pucIE;
			/*                                                           
                                                          
    */
			if (ucPrimaryChannel == 0) {
				ucPrimaryChannel = prHtOp->ucPrimaryChannel;
			}
			prBssInfo->ucHtOpInfo1 = prHtOp->ucInfo1;
			prBssInfo->u2HtOpInfo2 = prHtOp->u2Info2;
			prBssInfo->u2HtOpInfo3 = prHtOp->u2Info3;

			if (!prBssInfo->fg40mBwAllowed) {
				prBssInfo->ucHtOpInfo1 &=
				    ~(HT_OP_INFO1_SCO | HT_OP_INFO1_STA_CHNL_WIDTH);
			}

			if ((prBssInfo->ucHtOpInfo1 & HT_OP_INFO1_SCO) != CHNL_EXT_RES) {
				prBssInfo->eBssSCO = (ENUM_CHNL_EXT_T)
				    (prBssInfo->ucHtOpInfo1 & HT_OP_INFO1_SCO);
			}

			prBssInfo->eHtProtectMode = (ENUM_HT_PROTECT_MODE_T)
			    (prBssInfo->u2HtOpInfo2 & HT_OP_INFO2_HT_PROTECTION);

			/*                                    */
			if ((prBssInfo->u2HtOpInfo2 & HT_OP_INFO2_OBSS_NON_HT_STA_PRESENT)
			    && 0 /*                           */) {
				prBssInfo->eGfOperationMode = GF_MODE_DISALLOWED;
			} else if (prBssInfo->u2HtOpInfo2 & HT_OP_INFO2_NON_GF_HT_STA_PRESENT) {
				prBssInfo->eGfOperationMode = GF_MODE_PROTECT;
			} else {
				prBssInfo->eGfOperationMode = GF_MODE_NORMAL;
			}

			prBssInfo->eRifsOperationMode =
			    (prBssInfo->ucHtOpInfo1 & HT_OP_INFO1_RIFS_MODE) ?
			    RIFS_MODE_NORMAL : RIFS_MODE_DISALLOWED;

			break;

		case ELEM_ID_20_40_BSS_COEXISTENCE:
			if (!RLM_NET_IS_11N(prBssInfo)) {
				break;
			}
			/*                                                     */
			break;

		case ELEM_ID_OBSS_SCAN_PARAMS:
			if (!RLM_NET_IS_11N(prBssInfo) ||
			    IE_LEN(pucIE) != (sizeof(IE_OBSS_SCAN_PARAM_T) - 2)) {
				break;
			}
			/*                                  */
			prObssScnParam = (P_IE_OBSS_SCAN_PARAM_T) pucIE;
			break;

		case ELEM_ID_EXTENDED_CAP:
			if (!RLM_NET_IS_11N(prBssInfo)) {
				break;
			}
			/*                                                             */
			break;

		case ELEM_ID_ERP_INFO:
			if (IE_LEN(pucIE) != (sizeof(IE_ERP_T) - 2) || prBssInfo->eBand != BAND_2G4) {
				break;
			}
			ucERP = ERP_INFO_IE(pucIE)->ucERP;
			prBssInfo->fgErpProtectMode =
			    (ucERP & ERP_INFO_USE_PROTECTION) ? TRUE : FALSE;

			if (ucERP & ERP_INFO_BARKER_PREAMBLE_MODE) {
				prBssInfo->fgUseShortPreamble = FALSE;
			}
			break;

		case ELEM_ID_DS_PARAM_SET:
			if (IE_LEN(pucIE) == ELEM_MAX_LEN_DS_PARAMETER_SET) {
				ucPrimaryChannel = DS_PARAM_IE(pucIE)->ucCurrChnl;
			}
			break;

#if CFG_SUPPORT_QUIET && 0
			/*                                                              
                                                                      
    */
		case ELEM_ID_QUIET:
			rrmQuietHandleQuietIE(prBssInfo, (P_IE_QUIET_T) pucIE);
			fgHasQuietIE = TRUE;
			break;
#endif
		default:
			break;
		}		/*               */
	}			/*                    */

	/*                                                                
                                                         
  */
	if ((prBssInfo->eBand == BAND_2G4 && ucPrimaryChannel > 14) ||
	    (prBssInfo->eBand != BAND_2G4 && (ucPrimaryChannel >= 200 || ucPrimaryChannel <= 14))) {
		ucPrimaryChannel = 0;
	}
#if CFG_SUPPORT_QUIET && 0
	if (!fgHasQuietIE) {
		rrmQuietIeNotExist(prAdapter, prBssInfo);
	}
#endif

	/*                                        */
	if (!prAdapter->fgEnOnlineScan || !prObssScnParam ||
	    !(prStaRec->u2HtCapInfo & HT_CAP_INFO_SUP_CHNL_WIDTH) ||
	    prBssInfo->eBand != BAND_2G4 || !prBssInfo->fg40mBwAllowed) {

		/*                                                  */
		prBssInfo->u2ObssScanInterval = 0;
	} else {
		if (prObssScnParam->u2TriggerScanInterval < OBSS_SCAN_MIN_INTERVAL) {
			prObssScnParam->u2TriggerScanInterval = OBSS_SCAN_MIN_INTERVAL;
		}
		if (prBssInfo->u2ObssScanInterval != prObssScnParam->u2TriggerScanInterval) {

			prBssInfo->u2ObssScanInterval = prObssScnParam->u2TriggerScanInterval;

			/*                                      */
			cnmTimerStartTimer(prAdapter, &prBssInfo->rObssScanTimer,
					   prBssInfo->u2ObssScanInterval * MSEC_PER_SEC);
		}
	}

	return ucPrimaryChannel;
}

/*                                                                            */
/* 
                       
 
            
 
              
*/
/*                                                                            */
static BOOLEAN
rlmRecBcnFromNeighborForClient(P_ADAPTER_T prAdapter,
			       P_BSS_INFO_T prBssInfo,
			       P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength)
{
	UINT_16 u2Offset, i;
	UINT_8 ucPriChannel, ucSecChannel;
	ENUM_CHNL_EXT_T eSCO;
	BOOLEAN fgHtBss, fg20mReq;

	ASSERT(prAdapter);
	ASSERT(prBssInfo && prSwRfb);
	ASSERT(pucIE);

	/*                                                     */
	ucPriChannel = 0;
	eSCO = CHNL_EXT_SCN;

	fgHtBss = FALSE;
	fg20mReq = FALSE;

	IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
		switch (IE_ID(pucIE)) {
		case ELEM_ID_HT_CAP:
			{
				P_IE_HT_CAP_T prHtCap;

				if (IE_LEN(pucIE) != (sizeof(IE_HT_CAP_T) - 2)) {
					break;
				}

				prHtCap = (P_IE_HT_CAP_T) pucIE;
				if (prHtCap->u2HtCapInfo & HT_CAP_INFO_40M_INTOLERANT) {
					fg20mReq = TRUE;
				}
				fgHtBss = TRUE;
				break;
			}
		case ELEM_ID_HT_OP:
			{
				P_IE_HT_OP_T prHtOp;

				if (IE_LEN(pucIE) != (sizeof(IE_HT_OP_T) - 2)) {
					break;
				}

				prHtOp = (P_IE_HT_OP_T) pucIE;
				/*                                                           
                                                           
     */
				if (ucPriChannel == 0) {
					ucPriChannel = prHtOp->ucPrimaryChannel;
				}

				if ((prHtOp->ucInfo1 & HT_OP_INFO1_SCO) != CHNL_EXT_RES) {
					eSCO =
					    (ENUM_CHNL_EXT_T) (prHtOp->ucInfo1 & HT_OP_INFO1_SCO);
				}
				break;
			}
		case ELEM_ID_20_40_BSS_COEXISTENCE:
			{
				P_IE_20_40_COEXIST_T prCoexist;

				if (IE_LEN(pucIE) != (sizeof(IE_20_40_COEXIST_T) - 2)) {
					break;
				}

				prCoexist = (P_IE_20_40_COEXIST_T) pucIE;
				if (prCoexist->ucData & BSS_COEXIST_40M_INTOLERANT) {
					fg20mReq = TRUE;
				}
				break;
			}
		case ELEM_ID_DS_PARAM_SET:
			if (IE_LEN(pucIE) != (sizeof(IE_DS_PARAM_SET_T) - 2)) {
				break;
			}
			ucPriChannel = DS_PARAM_IE(pucIE)->ucCurrChnl;
			break;

		default:
			break;
		}
	}


	/*                                                                        
                                                                    
                 
  */
	if (HIF_RX_HDR_GET_RF_BAND(prSwRfb->prHifRxHdr) != BAND_2G4) {
		return FALSE;
	}

	if (ucPriChannel == 0 || ucPriChannel > 14) {
		ucPriChannel = HIF_RX_HDR_GET_CHNL_NUM(prSwRfb->prHifRxHdr);
	}

	if (fgHtBss) {
		ASSERT(prBssInfo->auc2G_PriChnlList[0] <= CHNL_LIST_SZ_2G);
		for (i = 1; i <= prBssInfo->auc2G_PriChnlList[0] && i <= CHNL_LIST_SZ_2G; i++) {
			if (prBssInfo->auc2G_PriChnlList[i] == ucPriChannel) {
				break;
			}
		}
		if ((i > prBssInfo->auc2G_PriChnlList[0]) && (i <= CHNL_LIST_SZ_2G)) {
			prBssInfo->auc2G_PriChnlList[i] = ucPriChannel;
			prBssInfo->auc2G_PriChnlList[0]++;
		}

		/*                          */
		if (eSCO != CHNL_EXT_SCN) {
			ucSecChannel = (eSCO == CHNL_EXT_SCA) ?
			    (ucPriChannel + 4) : (ucPriChannel - 4);

			ASSERT(prBssInfo->auc2G_SecChnlList[0] <= CHNL_LIST_SZ_2G);
			for (i = 1; i <= prBssInfo->auc2G_SecChnlList[0] &&
			     i <= CHNL_LIST_SZ_2G; i++) {
				if (prBssInfo->auc2G_SecChnlList[i] == ucSecChannel) {
					break;
				}
			}
			if ((i > prBssInfo->auc2G_SecChnlList[0]) && (i <= CHNL_LIST_SZ_2G)) {
				prBssInfo->auc2G_SecChnlList[i] = ucSecChannel;
				prBssInfo->auc2G_SecChnlList[0]++;
			}
		}

		/*                                       */
		if (fg20mReq) {
			ASSERT(prBssInfo->auc2G_20mReqChnlList[0] <= CHNL_LIST_SZ_2G);
			for (i = 1; i <= prBssInfo->auc2G_20mReqChnlList[0] &&
			     i <= CHNL_LIST_SZ_2G; i++) {
				if (prBssInfo->auc2G_20mReqChnlList[i] == ucPriChannel) {
					break;
				}
			}
			if ((i > prBssInfo->auc2G_20mReqChnlList[0]) && (i <= CHNL_LIST_SZ_2G)) {
				prBssInfo->auc2G_20mReqChnlList[i] = ucPriChannel;
				prBssInfo->auc2G_20mReqChnlList[0]++;
			}
		}
	} else {
		/*                            */
		ASSERT(prBssInfo->auc2G_NonHtChnlList[0] <= CHNL_LIST_SZ_2G);
		for (i = 1; i <= prBssInfo->auc2G_NonHtChnlList[0] && i <= CHNL_LIST_SZ_2G; i++) {
			if (prBssInfo->auc2G_NonHtChnlList[i] == ucPriChannel) {
				break;
			}
		}
		if ((i > prBssInfo->auc2G_NonHtChnlList[0]) && (i <= CHNL_LIST_SZ_2G)) {
			prBssInfo->auc2G_NonHtChnlList[i] = ucPriChannel;
			prBssInfo->auc2G_NonHtChnlList[0]++;
		}

	}

	return FALSE;
}


/*                                                                            */
/* 
                       
 
            
 
              
*/
/*                                                                            */
static BOOLEAN
rlmRecBcnInfoForClient(P_ADAPTER_T prAdapter,
		       P_BSS_INFO_T prBssInfo,
		       P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength)
{
	ASSERT(prAdapter);
	ASSERT(prBssInfo && prSwRfb);
	ASSERT(pucIE);

#if 0				/*                        */
	/*                                                              
                                                                       
                          
                                                                           
                                                                           
                                                                
  */
	if (IS_SCAN_ACTIVE()
	    /*                                                              */
	    ) {
		return FALSE;
	}
#endif

	/*                            */
	prBssInfo->u2CapInfo = ((P_WLAN_BEACON_FRAME_T) (prSwRfb->pvHeader))->u2CapInfo;
	prBssInfo->fgUseShortSlotTime =
	    (prBssInfo->u2CapInfo & CAP_INFO_SHORT_SLOT_TIME) ? TRUE : FALSE;

	rlmRecIeInfoForClient(prAdapter, prBssInfo, pucIE, u2IELength);

	return TRUE;
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmProcessBcn(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength)
{
	P_BSS_INFO_T prBssInfo;
	BOOLEAN fgNewParameter;
	UINT_8 ucNetIdx;

	ASSERT(prAdapter);
	ASSERT(prSwRfb);
	ASSERT(pucIE);

	fgNewParameter = FALSE;

	/*                                                                 
                                                                    
                         
                                                             
                                                                       
                                                                        
  */
	RLM_NET_FOR_EACH_NO_BOW(ucNetIdx) {
		prBssInfo = &prAdapter->rWifiVar.arBssInfo[ucNetIdx];
		ASSERT(prBssInfo);

		if (IS_BSS_ACTIVE(prBssInfo)) {
			if (prBssInfo->eCurrentOPMode == OP_MODE_INFRASTRUCTURE &&
			    prBssInfo->eConnectionState == PARAM_MEDIA_STATE_CONNECTED) {
				/*                             */
				if (EQUAL_MAC_ADDR(prBssInfo->aucBSSID, ((P_WLAN_MAC_MGMT_HEADER_T)
									 (prSwRfb->pvHeader))->
						   aucBSSID)) {

					fgNewParameter = rlmRecBcnInfoForClient(prAdapter,
										prBssInfo, prSwRfb,
										pucIE, u2IELength);
				} else {
					fgNewParameter = rlmRecBcnFromNeighborForClient(prAdapter,
											prBssInfo,
											prSwRfb,
											pucIE,
											u2IELength);
				}
			}
#if CFG_ENABLE_WIFI_DIRECT
			else if (prAdapter->fgIsP2PRegistered &&
				 (prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT ||
				  prBssInfo->eCurrentOPMode == OP_MODE_P2P_DEVICE)) {
				/*                                                   */
				rlmRecBcnFromNeighborForClient(prAdapter,
							       prBssInfo, prSwRfb, pucIE,
							       u2IELength);
			}
#endif
			else if (prBssInfo->eCurrentOPMode == OP_MODE_IBSS) {
				/*               */
			}

			/*                                  */
			if (fgNewParameter) {
				rlmSyncOperationParams(prAdapter, prBssInfo);
				fgNewParameter = FALSE;
			}
		}		/*                        */
	}			/*                                */
}

/*                                                                            */
/* 
                                                                              
 
            
 
              
*/
/*                                                                            */
VOID
rlmProcessAssocRsp(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength)
{
	P_BSS_INFO_T prBssInfo;
	P_STA_RECORD_T prStaRec;
	UINT_8 ucPriChannel;

	ASSERT(prAdapter);
	ASSERT(prSwRfb);
	ASSERT(pucIE);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
	ASSERT(prStaRec);
	if (!prStaRec) {
		return;
	}
	ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex];
	ASSERT(prStaRec == prBssInfo->prStaRecOfAP);

	/*                                                                    
                                               
  */
	rlmBssReset(prAdapter, prBssInfo);

	prBssInfo->fgUseShortSlotTime =
	    (prBssInfo->u2CapInfo & CAP_INFO_SHORT_SLOT_TIME) ? TRUE : FALSE;

	if ((ucPriChannel = rlmRecIeInfoForClient(prAdapter, prBssInfo, pucIE, u2IELength)) > 0) {
		prBssInfo->ucPrimaryChannel = ucPriChannel;
	}

	if (!RLM_NET_IS_11N(prBssInfo) || !(prStaRec->u2HtCapInfo & HT_CAP_INFO_SUP_CHNL_WIDTH)) {
		prBssInfo->fg40mBwAllowed = FALSE;
	}

	/*                                                                       
                                      
                                                                         
  */
}

/*                                                                            */
/* 
                                                                              
 
            
 
              
*/
/*                                                                            */
VOID rlmFillSyncCmdParam(P_CMD_SET_BSS_RLM_PARAM_T prCmdBody, P_BSS_INFO_T prBssInfo)
{
	ASSERT(prCmdBody && prBssInfo);
	if (!prCmdBody || !prBssInfo) {
		return;
	}

	prCmdBody->ucNetTypeIndex = prBssInfo->ucNetTypeIndex;
	prCmdBody->ucRfBand = (UINT_8) prBssInfo->eBand;
	prCmdBody->ucPrimaryChannel = prBssInfo->ucPrimaryChannel;
	prCmdBody->ucRfSco = (UINT_8) prBssInfo->eBssSCO;
	prCmdBody->ucErpProtectMode = (UINT_8) prBssInfo->fgErpProtectMode;
	prCmdBody->ucHtProtectMode = (UINT_8) prBssInfo->eHtProtectMode;
	prCmdBody->ucGfOperationMode = (UINT_8) prBssInfo->eGfOperationMode;
	prCmdBody->ucTxRifsMode = (UINT_8) prBssInfo->eRifsOperationMode;
	prCmdBody->u2HtOpInfo3 = prBssInfo->u2HtOpInfo3;
	prCmdBody->u2HtOpInfo2 = prBssInfo->u2HtOpInfo2;
	prCmdBody->ucHtOpInfo1 = prBssInfo->ucHtOpInfo1;
	prCmdBody->ucUseShortPreamble = prBssInfo->fgUseShortPreamble;
	prCmdBody->ucUseShortSlotTime = prBssInfo->fgUseShortSlotTime;
	prCmdBody->ucCheckId = 0x72;

	if (RLM_NET_PARAM_VALID(prBssInfo)) {
		DBGLOG(RLM, INFO, ("N=%d b=%d c=%d s=%d e=%d h=%d I=0x%02x l=%d p=%d\n",
				   prCmdBody->ucNetTypeIndex, prCmdBody->ucRfBand,
				   prCmdBody->ucPrimaryChannel, prCmdBody->ucRfSco,
				   prCmdBody->ucErpProtectMode, prCmdBody->ucHtProtectMode,
				   prCmdBody->ucHtOpInfo1, prCmdBody->ucUseShortSlotTime,
				   prCmdBody->ucUseShortPreamble));
	} else {
		DBGLOG(RLM, INFO, ("N=%d closed\n", prCmdBody->ucNetTypeIndex));
	}
}

/*                                                                            */
/* 
                                                                       
                                                                            
                               
 
            
 
              
*/
/*                                                                            */
VOID rlmSyncOperationParams(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo)
{
	P_CMD_SET_BSS_RLM_PARAM_T prCmdBody;
	WLAN_STATUS rStatus;

	ASSERT(prAdapter);
	ASSERT(prBssInfo);

	prCmdBody = (P_CMD_SET_BSS_RLM_PARAM_T)
	    cnmMemAlloc(prAdapter, RAM_TYPE_BUF, sizeof(CMD_SET_BSS_RLM_PARAM_T));
	ASSERT(prCmdBody);

	/*                         */
	if (!prCmdBody) {
		DBGLOG(RLM, WARN, ("No buf for sync RLM params (Net=%d)\n",
				   prBssInfo->ucNetTypeIndex));
		return;
	}

	rlmFillSyncCmdParam(prCmdBody, prBssInfo);

	rStatus = wlanSendSetQueryCmd(prAdapter,	/*           */
				      CMD_ID_SET_BSS_RLM_PARAM,	/*       */
				      TRUE,	/*            */
				      FALSE,	/*            */
				      FALSE,	/*         */
				      NULL,	/*                  */
				      NULL,	/*                     */
				      sizeof(CMD_SET_BSS_RLM_PARAM_T),	/*                   */
				      (PUINT_8) prCmdBody,	/*               */
				      NULL,	/*                  */
				      0	/*                     */
	    );

	ASSERT(rStatus == WLAN_STATUS_PENDING);

	cnmMemFree(prAdapter, prCmdBody);
}

#if CFG_SUPPORT_AAA
/*                                                                            */
/* 
                                                                              
 
            
 
              
*/
/*                                                                            */
VOID
rlmProcessAssocReq(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb, PUINT_8 pucIE, UINT_16 u2IELength)
{
	P_BSS_INFO_T prBssInfo;
	P_STA_RECORD_T prStaRec;
	UINT_16 u2Offset;
	P_IE_HT_CAP_T prHtCap;

	ASSERT(prAdapter);
	ASSERT(prSwRfb);
	ASSERT(pucIE);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
	ASSERT(prStaRec);
	if (!prStaRec) {
		return;
	}
	ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

	prBssInfo = &prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex];

	IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
		switch (IE_ID(pucIE)) {
		case ELEM_ID_HT_CAP:
			if (!RLM_NET_IS_11N(prBssInfo) ||
			    IE_LEN(pucIE) != (sizeof(IE_HT_CAP_T) - 2)) {
				break;
			}
			prHtCap = (P_IE_HT_CAP_T) pucIE;
			prStaRec->ucMcsSet = prHtCap->rSupMcsSet.aucRxMcsBitmask[0];
			prStaRec->fgSupMcs32 =
			    (prHtCap->rSupMcsSet.aucRxMcsBitmask[32 / 8] & BIT(0)) ? TRUE : FALSE;

			prStaRec->u2HtCapInfo = prHtCap->u2HtCapInfo;
			prStaRec->ucAmpduParam = prHtCap->ucAmpduParam;
			prStaRec->u2HtExtendedCap = prHtCap->u2HtExtendedCap;
			prStaRec->u4TxBeamformingCap = prHtCap->u4TxBeamformingCap;
			prStaRec->ucAselCap = prHtCap->ucAselCap;
			break;

		default:
			break;
		}		/*               */
	}			/*                    */
}
#endif				/*                 */

/*                                                                            */
/* 
                                        
 
            
 
              
*/
/*                                                                            */
VOID rlmBssInitForAPandIbss(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo)
{
	ASSERT(prAdapter);
	ASSERT(prBssInfo);

#if CFG_ENABLE_WIFI_DIRECT
	if (prAdapter->fgIsP2PRegistered && prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT) {

		rlmBssInitForAP(prAdapter, prBssInfo);
	}
#endif
}

/*                                                                            */
/* 
                                        
 
            
 
              
*/
/*                                                                            */
VOID rlmBssAborted(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo)
{
	ASSERT(prAdapter);
	ASSERT(prBssInfo);

	rlmBssReset(prAdapter, prBssInfo);

	prBssInfo->fg40mBwAllowed = FALSE;
	prBssInfo->fgAssoc40mBwAllowed = FALSE;

	/*                                                      
                                    
  */
}

/*                                                                            */
/* 
                                             
 
            
 
              
*/
/*                                                                            */
static VOID rlmBssReset(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo)
{
	ASSERT(prAdapter);
	ASSERT(prBssInfo);

	/*                       */
	prBssInfo->ucHtOpInfo1 = 0;	/*                      */
	prBssInfo->u2HtOpInfo2 = 0;
	prBssInfo->u2HtOpInfo3 = 0;

	prBssInfo->eBssSCO = 0;
	prBssInfo->fgErpProtectMode = 0;
	prBssInfo->eHtProtectMode = 0;
	prBssInfo->eGfOperationMode = 0;
	prBssInfo->eRifsOperationMode = 0;

	/*                         */
	prBssInfo->auc2G_20mReqChnlList[0] = 0;
	prBssInfo->auc2G_NonHtChnlList[0] = 0;
	prBssInfo->auc2G_PriChnlList[0] = 0;
	prBssInfo->auc2G_SecChnlList[0] = 0;
	prBssInfo->auc5G_20mReqChnlList[0] = 0;
	prBssInfo->auc5G_NonHtChnlList[0] = 0;
	prBssInfo->auc5G_PriChnlList[0] = 0;
	prBssInfo->auc5G_SecChnlList[0] = 0;

	/*                                     */
	cnmTimerStopTimer(prAdapter, &prBssInfo->rObssScanTimer);
	prBssInfo->u2ObssScanInterval = 0;

	prBssInfo->fgObssErpProtectMode = 0;	/*         */
	prBssInfo->eObssHtProtectMode = 0;	/*         */
	prBssInfo->eObssGfOperationMode = 0;	/*         */
	prBssInfo->fgObssRifsOperationMode = 0;	/*         */
	prBssInfo->fgObssActionForcedTo20M = 0;	/*         */
	prBssInfo->fgObssBeaconForcedTo20M = 0;	/*         */
}
