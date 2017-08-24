/*
                        
*/

/*                     
          

*/




/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

#include "precomp.h"
#include "rlm.h"


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
VOID rlmBssInitForAP(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo)
{
	ENUM_BAND_T eBand;
	UINT_8 ucChannel, i;
	ENUM_CHNL_EXT_T eSCO;

	ASSERT(prAdapter);
	ASSERT(prBssInfo);

	if (prBssInfo->eCurrentOPMode != OP_MODE_ACCESS_POINT) {
		return;
	}

	/*                                                                      
                                                        
  */
	prBssInfo->fg40mBwAllowed = FALSE;
	prBssInfo->fgAssoc40mBwAllowed = FALSE;
	prBssInfo->eBssSCO = CHNL_EXT_SCN;

    /*                                    
                                                                                    
                                     
     */
    if (cnmBss40mBwPermitted(prAdapter, prBssInfo->ucBssIndex)) {
		/*                                                                
                                                                  
                                            
   */
		if (cnmPreferredChannel(prAdapter, &eBand, &ucChannel, &eSCO) &&
		    eSCO != CHNL_EXT_SCN && ucChannel == prBssInfo->ucPrimaryChannel &&
		    eBand == prBssInfo->eBand) {
			prBssInfo->eBssSCO = eSCO;
		} else {
			prBssInfo->eBssSCO = rlmDecideScoForAP(prAdapter, prBssInfo);
		}

		if (prBssInfo->eBssSCO != CHNL_EXT_SCN) {
			prBssInfo->fg40mBwAllowed = TRUE;
			prBssInfo->fgAssoc40mBwAllowed = TRUE;

			prBssInfo->ucHtOpInfo1 = (UINT_8)
			    (((UINT_32) prBssInfo->eBssSCO) | HT_OP_INFO1_STA_CHNL_WIDTH);

			rlmUpdateBwByChListForAP(prAdapter, prBssInfo);
		}
	}

	/*                                          */
    if(prBssInfo->ucPhyTypeSet & PHY_TYPE_BIT_VHT) {
    	for(i = 0 ; i < 8 ; i++ ){
            prBssInfo->u2VhtBasicMcsSet |= BITS(2*i,(2*i+1));
    	}
    	prBssInfo->u2VhtBasicMcsSet &= (VHT_CAP_INFO_MCS_MAP_MCS9 << VHT_CAP_INFO_MCS_1SS_OFFSET);
        
        if(cnmGetBssMaxBw(prAdapter, prBssInfo->ucBssIndex) < MAX_BW_80MHZ || prBssInfo->eBand == BAND_2G4) {
            prBssInfo->ucVhtChannelWidth = VHT_OP_CHANNEL_WIDTH_20_40;
            prBssInfo->ucVhtChannelFrequencyS1 = 0;
            prBssInfo->ucVhtChannelFrequencyS2 = 0;
        }
        else if(cnmGetBssMaxBw(prAdapter, prBssInfo->ucBssIndex) == MAX_BW_80MHZ) {        
            prBssInfo->ucVhtChannelWidth = VHT_OP_CHANNEL_WIDTH_80;
            prBssInfo->ucVhtChannelFrequencyS1 = nicGetVhtS1(prBssInfo->ucPrimaryChannel);
            prBssInfo->ucVhtChannelFrequencyS2 = 0;
        }
        else {
            //                               
        }
    }
    else {
        prBssInfo->ucVhtChannelWidth = VHT_OP_CHANNEL_WIDTH_20_40;
        prBssInfo->ucVhtChannelFrequencyS1 = 0;
        prBssInfo->ucVhtChannelFrequencyS2 = 0;
    }



    DBGLOG(RLM, INFO, ("WLAN AP SCO=%d BW=%d S1=%d S2=%d CH=%d Band=%d\n", 
        prBssInfo->eBssSCO,
        prBssInfo->ucVhtChannelWidth,
        prBssInfo->ucVhtChannelFrequencyS1,
        prBssInfo->ucVhtChannelFrequencyS2,
        prBssInfo->ucVhtChannelWidth,
        prBssInfo->eBand
        ));
    
}

/*                                                                            */
/* 
                                                               
 
            
 
              
*/
/*                                                                            */
VOID rlmRspGenerateObssScanIE(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo)
{
	P_BSS_INFO_T prBssInfo;
	P_IE_OBSS_SCAN_PARAM_T prObssScanIe;
	P_STA_RECORD_T prStaRec = (P_STA_RECORD_T) NULL;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);

	prBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, prMsduInfo->ucBssIndex);
	if (!prBssInfo) {
		return;
	}

	if (!IS_BSS_ACTIVE(prBssInfo)) {
		return;
	}

	if (RLM_NET_IS_11N(prBssInfo) &&	/*                                        */
	    prBssInfo->eCurrentOPMode == OP_MODE_ACCESS_POINT &&
	    (!prStaRec || (prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N)) &&
	    prBssInfo->eBand == BAND_2G4 && prBssInfo->eBssSCO != CHNL_EXT_SCN) {

		prObssScanIe = (P_IE_OBSS_SCAN_PARAM_T)
		    (((PUINT_8) prMsduInfo->prPacket) + prMsduInfo->u2FrameLength);

		/*                              */
		prObssScanIe->ucId = ELEM_ID_OBSS_SCAN_PARAMS;
		prObssScanIe->ucLength = sizeof(IE_OBSS_SCAN_PARAM_T) - ELEM_HDR_LEN;

		prObssScanIe->u2ScanPassiveDwell = dot11OBSSScanPassiveDwell;
		prObssScanIe->u2ScanActiveDwell = dot11OBSSScanActiveDwell;
		prObssScanIe->u2TriggerScanInterval = dot11BSSWidthTriggerScanInterval;
		prObssScanIe->u2ScanPassiveTotalPerChnl = dot11OBSSScanPassiveTotalPerChannel;
		prObssScanIe->u2ScanActiveTotalPerChnl = dot11OBSSScanActiveTotalPerChannel;
		prObssScanIe->u2WidthTransDelayFactor = dot11BSSWidthChannelTransitionDelayFactor;
		prObssScanIe->u2ScanActivityThres = dot11OBSSScanActivityThreshold;

		ASSERT(IE_SIZE(prObssScanIe) <= (ELEM_HDR_LEN + ELEM_MAX_LEN_OBSS_SCAN));

		prMsduInfo->u2FrameLength += IE_SIZE(prObssScanIe);
	}
}

/*                                                                            */
/* 
                
 
            
 
              
*/
/*                                                                            */
BOOLEAN rlmUpdateBwByChListForAP(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo)
{
	UINT_8 ucLevel;
	BOOLEAN fgBwChange;

	ASSERT(prAdapter);
	ASSERT(prBssInfo);

	fgBwChange = FALSE;

	if (prBssInfo->eBssSCO == CHNL_EXT_SCN) {
		return fgBwChange;
	}

	ucLevel = rlmObssChnlLevel(prBssInfo, prBssInfo->eBand,
				   prBssInfo->ucPrimaryChannel, prBssInfo->eBssSCO);

	if (ucLevel == CHNL_LEVEL0) {
		/*                                                                   */
		prBssInfo->fgObssActionForcedTo20M = TRUE;

		if (prBssInfo->ucHtOpInfo1 != (UINT_8) CHNL_EXT_SCN) {
			prBssInfo->ucHtOpInfo1 = (UINT_8) CHNL_EXT_SCN;
			fgBwChange = TRUE;
		}

		cnmTimerStartTimer(prAdapter, &prBssInfo->rObssScanTimer,
				   OBSS_20_40M_TIMEOUT * MSEC_PER_SEC);
	}

	/*                            */
	prBssInfo->auc2G_20mReqChnlList[0] = 0;
	prBssInfo->auc2G_NonHtChnlList[0] = 0;
	prBssInfo->auc2G_PriChnlList[0] = 0;
	prBssInfo->auc2G_SecChnlList[0] = 0;
	prBssInfo->auc5G_20mReqChnlList[0] = 0;
	prBssInfo->auc5G_NonHtChnlList[0] = 0;
	prBssInfo->auc5G_PriChnlList[0] = 0;
	prBssInfo->auc5G_SecChnlList[0] = 0;

	return fgBwChange;
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmProcessPublicAction(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb)
{
	P_ACTION_20_40_COEXIST_FRAME prRxFrame;
	P_IE_20_40_COEXIST_T prCoexist;
	P_IE_INTOLERANT_CHNL_REPORT_T prChnlReport;
	P_BSS_INFO_T prBssInfo;
	P_STA_RECORD_T prStaRec;
	PUINT_8 pucIE;
	UINT_16 u2IELength, u2Offset;
	UINT_8 i, j;

	ASSERT(prAdapter);
	ASSERT(prSwRfb);

	prRxFrame = (P_ACTION_20_40_COEXIST_FRAME) prSwRfb->pvHeader;
	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);

	if (prRxFrame->ucAction != ACTION_PUBLIC_20_40_COEXIST || !prStaRec || prStaRec->ucStaState != STA_STATE_3 || prSwRfb->u2PacketLen < (WLAN_MAC_MGMT_HEADER_LEN + 5) || prSwRfb->prStaRec->ucBssIndex !=	/*                                                    */
	    prStaRec->ucBssIndex) {
		return;
	}

	prBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, prStaRec->ucBssIndex);
	ASSERT(prBssInfo);

	if (!IS_BSS_ACTIVE(prBssInfo) ||
	    prBssInfo->eCurrentOPMode != OP_MODE_ACCESS_POINT ||
	    prBssInfo->eBssSCO == CHNL_EXT_SCN) {
		return;
	}

	prCoexist = &prRxFrame->rBssCoexist;
	if (prCoexist->ucData & (BSS_COEXIST_40M_INTOLERANT | BSS_COEXIST_20M_REQ)) {
		ASSERT(prBssInfo->auc2G_20mReqChnlList[0] <= CHNL_LIST_SZ_2G);
		for (i = 1; i <= prBssInfo->auc2G_20mReqChnlList[0] && i <= CHNL_LIST_SZ_2G; i++) {
			if (prBssInfo->auc2G_20mReqChnlList[i] == prBssInfo->ucPrimaryChannel) {
				break;
			}
		}
		if ((i > prBssInfo->auc2G_20mReqChnlList[0]) && (i <= CHNL_LIST_SZ_2G)) {
			prBssInfo->auc2G_20mReqChnlList[i] = prBssInfo->ucPrimaryChannel;
			prBssInfo->auc2G_20mReqChnlList[0]++;
		}
	}

	/*                                      */
	pucIE = (PUINT_8) &prRxFrame->rChnlReport;
	u2IELength = prSwRfb->u2PacketLen - (WLAN_MAC_MGMT_HEADER_LEN + 5);

	IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
		switch (IE_ID(pucIE)) {
		case ELEM_ID_20_40_INTOLERANT_CHNL_REPORT:
			prChnlReport = (P_IE_INTOLERANT_CHNL_REPORT_T) pucIE;

			if (prChnlReport->ucLength <= 1) {
				break;
			}

			/*                                                          */

			for (j = 0; j < prChnlReport->ucLength - 1; j++) {
				/*                            */
				ASSERT(prBssInfo->auc2G_NonHtChnlList[0] <= CHNL_LIST_SZ_2G);
				for (i = 1; i <= prBssInfo->auc2G_NonHtChnlList[0] &&
				     i <= CHNL_LIST_SZ_2G; i++) {
					if (prBssInfo->auc2G_NonHtChnlList[i] ==
					    prChnlReport->aucChannelList[j]) {
						break;
					}
				}
				if ((i > prBssInfo->auc2G_NonHtChnlList[0]) &&
				    (i <= CHNL_LIST_SZ_2G)) {
					prBssInfo->auc2G_NonHtChnlList[i] =
					    prChnlReport->aucChannelList[j];
					prBssInfo->auc2G_NonHtChnlList[0]++;
				}
			}
			break;

		default:
			break;
		}
	}			/*                    */

	if (rlmUpdateBwByChListForAP(prAdapter, prBssInfo)) {
		bssUpdateBeaconContent(prAdapter, prBssInfo->ucBssIndex);
		rlmSyncOperationParams(prAdapter, prBssInfo);
	}

	/*                                                      */
	if (prCoexist->ucData & BSS_COEXIST_OBSS_SCAN_EXEMPTION_REQ) {
		rlmObssScanExemptionRsp(prAdapter, prBssInfo, prSwRfb);
	}
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmProcessHtAction(P_ADAPTER_T prAdapter, P_SW_RFB_T prSwRfb)
{
	P_ACTION_NOTIFY_CHNL_WIDTH_FRAME prRxFrame;
	P_STA_RECORD_T prStaRec;

	ASSERT(prAdapter);
	ASSERT(prSwRfb);

	prRxFrame = (P_ACTION_NOTIFY_CHNL_WIDTH_FRAME) prSwRfb->pvHeader;
	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);

	if (prRxFrame->ucAction != ACTION_HT_NOTIFY_CHANNEL_WIDTH ||
	    !prStaRec || prStaRec->ucStaState != STA_STATE_3 ||
	    prSwRfb->u2PacketLen < sizeof(ACTION_NOTIFY_CHNL_WIDTH_FRAME)) {
		return;
	}

	/*                                                          
                                                                         
                                              
  */
	if (prRxFrame->ucChannelWidth == 0) {
		prStaRec->u2HtCapInfo &= ~HT_CAP_INFO_SUP_CHNL_WIDTH;
	} else if (prRxFrame->ucChannelWidth == 1) {
		prStaRec->u2HtCapInfo |= HT_CAP_INFO_SUP_CHNL_WIDTH;
	}
	cnmStaRecChangeState(prAdapter, prStaRec, STA_STATE_3);
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
VOID rlmHandleObssStatusEventPkt(P_ADAPTER_T prAdapter, P_EVENT_AP_OBSS_STATUS_T prObssStatus)
{
	P_BSS_INFO_T prBssInfo;

	ASSERT(prAdapter);
	ASSERT(prObssStatus);
	ASSERT(prObssStatus->ucBssIndex < MAX_BSS_INDEX);

	prBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, prObssStatus->ucBssIndex);

	if (prBssInfo->eCurrentOPMode != OP_MODE_ACCESS_POINT) {
		return;
	}

	prBssInfo->fgObssErpProtectMode = (BOOLEAN) prObssStatus->ucObssErpProtectMode;
	prBssInfo->eObssHtProtectMode = (ENUM_HT_PROTECT_MODE_T) prObssStatus->ucObssHtProtectMode;
	prBssInfo->eObssGfOperationMode = (ENUM_GF_MODE_T) prObssStatus->ucObssGfOperationMode;
	prBssInfo->fgObssRifsOperationMode = (BOOLEAN) prObssStatus->ucObssRifsOperationMode;
	prBssInfo->fgObssBeaconForcedTo20M = (BOOLEAN) prObssStatus->ucObssBeaconForcedTo20M;

	/*                                            */
	rlmUpdateParamsForAP(prAdapter, prBssInfo, TRUE);
}


/*                                                                            */
/* 
                                                          
 
            
 
              
*/
/*                                                                            */
VOID rlmUpdateParamsForAP(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo, BOOLEAN fgUpdateBeacon)
{
	P_LINK_T prStaList;
	P_STA_RECORD_T prStaRec;
	BOOLEAN fgErpProtectMode, fgSta40mIntolerant;
	BOOLEAN fgUseShortPreamble, fgUseShortSlotTime;
	ENUM_HT_PROTECT_MODE_T eHtProtectMode;
	ENUM_GF_MODE_T eGfOperationMode;
	UINT_8 ucHtOpInfo1;

	ASSERT(prAdapter);
	ASSERT(prBssInfo);

	if (!IS_BSS_ACTIVE(prBssInfo) || prBssInfo->eCurrentOPMode != OP_MODE_ACCESS_POINT) {
		return;
	}

	fgErpProtectMode = FALSE;
	eHtProtectMode = HT_PROTECT_MODE_NONE;
	eGfOperationMode = GF_MODE_NORMAL;
	fgSta40mIntolerant = FALSE;
	fgUseShortPreamble = prBssInfo->fgIsShortPreambleAllowed;
	fgUseShortSlotTime = TRUE;
	ucHtOpInfo1 = (UINT_8) CHNL_EXT_SCN;

	prStaList = &prBssInfo->rStaRecOfClientList;

	LINK_FOR_EACH_ENTRY(prStaRec, prStaList, rLinkEntry, STA_RECORD_T) {
		ASSERT(prStaRec);
		if (prStaRec->fgIsInUse && prStaRec->ucStaState == STA_STATE_3 &&
		    prStaRec->ucBssIndex == prBssInfo->ucBssIndex) {
			if (!(prStaRec->ucPhyTypeSet &
			      (PHY_TYPE_SET_802_11GN | PHY_TYPE_SET_802_11A))) {
				/*                                         */
				fgErpProtectMode = TRUE;
			}

			if (!(prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N)) {
				/*                   */
				eHtProtectMode = HT_PROTECT_MODE_NON_HT;
			} else if (!(prStaRec->u2HtCapInfo & HT_CAP_INFO_SUP_CHNL_WIDTH)) {
				/*            */
				if (eHtProtectMode == HT_PROTECT_MODE_NONE) {
					eHtProtectMode = HT_PROTECT_MODE_20M;
				}
			}

			if (!(prStaRec->u2HtCapInfo & HT_CAP_INFO_HT_GF)) {
				eGfOperationMode = GF_MODE_PROTECT;
			}

			if (!(prStaRec->u2CapInfo & CAP_INFO_SHORT_PREAMBLE)) {
				fgUseShortPreamble = FALSE;
			}

			if (!(prStaRec->u2CapInfo & CAP_INFO_SHORT_SLOT_TIME)) {
				fgUseShortSlotTime = FALSE;
			}

			if (prStaRec->u2HtCapInfo & HT_CAP_INFO_40M_INTOLERANT) {
				fgSta40mIntolerant = TRUE;
			}
		}
	}			/*                            */

	/*                                                                  */
	if (prBssInfo->eBssSCO != CHNL_EXT_SCN) {
		if (/*                             */ !fgSta40mIntolerant &&
		    !prBssInfo->fgObssActionForcedTo20M && !prBssInfo->fgObssBeaconForcedTo20M) {

			ucHtOpInfo1 = (UINT_8)
			    (((UINT_32) prBssInfo->eBssSCO) | HT_OP_INFO1_STA_CHNL_WIDTH);
		}
	}

	/*                                           */
	if (prBssInfo->fgErpProtectMode != fgErpProtectMode ||
	    prBssInfo->eHtProtectMode != eHtProtectMode ||
	    prBssInfo->eGfOperationMode != eGfOperationMode ||
	    prBssInfo->ucHtOpInfo1 != ucHtOpInfo1 ||
	    prBssInfo->fgUseShortPreamble != fgUseShortPreamble ||
	    prBssInfo->fgUseShortSlotTime != fgUseShortSlotTime) {

		prBssInfo->fgErpProtectMode = fgErpProtectMode;
		prBssInfo->eHtProtectMode = eHtProtectMode;
		prBssInfo->eGfOperationMode = eGfOperationMode;
		prBssInfo->ucHtOpInfo1 = ucHtOpInfo1;
		prBssInfo->fgUseShortPreamble = fgUseShortPreamble;
		prBssInfo->fgUseShortSlotTime = fgUseShortSlotTime;

		if (fgUseShortSlotTime) {
			prBssInfo->u2CapInfo |= CAP_INFO_SHORT_SLOT_TIME;
		} else {
			prBssInfo->u2CapInfo &= ~CAP_INFO_SHORT_SLOT_TIME;
		}

		rlmSyncOperationParams(prAdapter, prBssInfo);
		fgUpdateBeacon = TRUE;
	}

	/*                                                        */
	if (fgUpdateBeacon) {
		bssUpdateBeaconContent(prAdapter, prBssInfo->ucBssIndex);
	}
}


/*                                                                            */
/* 
                                                                 
                                                                                           
                                                                                         
 
                                            
 
                                                  
*/
/*                                                                            */
VOID rlmFuncInitialChannelList(IN P_ADAPTER_T prAdapter)
{
	P_P2P_CONNECTION_SETTINGS_T prP2pConnSetting = (P_P2P_CONNECTION_SETTINGS_T) NULL;
	P_DOMAIN_INFO_ENTRY prDomainInfoEntry = (P_DOMAIN_INFO_ENTRY) NULL;
	P_DOMAIN_SUBBAND_INFO prDomainSubBand = (P_DOMAIN_SUBBAND_INFO) NULL;
	P_CHANNEL_ENTRY_FIELD_T prChannelEntryField = (P_CHANNEL_ENTRY_FIELD_T) NULL;
	UINT_32 u4Idx = 0, u4IdxII = 0;
	UINT_8 ucBufferSize = P2P_MAX_SUPPORTED_CHANNEL_LIST_SIZE;
#if 0
	UINT_8 ucSocialChnlSupport = 0, ucAutoChnl = 0;
#endif

	do {
		ASSERT_BREAK(prAdapter != NULL);

		prP2pConnSetting = prAdapter->rWifiVar.prP2PConnSettings;
#if 0
		ucAutoChnl = prP2pConnSetting->ucOperatingChnl;
#endif

		prDomainInfoEntry = rlmDomainGetDomainInfo(prAdapter);

		ASSERT_BREAK((prDomainInfoEntry != NULL) && (prP2pConnSetting != NULL));

		prChannelEntryField =
		    (P_CHANNEL_ENTRY_FIELD_T) prP2pConnSetting->aucChannelEntriesField;

		for (u4Idx = 0; u4Idx < MAX_SUBBAND_NUM; u4Idx++) {
			prDomainSubBand = &prDomainInfoEntry->rSubBand[u4Idx];


			if (((prDomainSubBand->ucBand == BAND_5G) && (!prAdapter->fgEnable5GBand))
			    || (prDomainSubBand->ucBand == BAND_NULL)) {
				continue;
			}


			if (ucBufferSize <
			    (P2P_ATTRI_LEN_CHANNEL_ENTRY + prDomainSubBand->ucNumChannels)) {
				/*                                                         */
				break;	/*     */
			}

			prChannelEntryField->ucRegulatoryClass = prDomainSubBand->ucRegClass;
			prChannelEntryField->ucNumberOfChannels = prDomainSubBand->ucNumChannels;

			for (u4IdxII = 0; u4IdxII < prDomainSubBand->ucNumChannels; u4IdxII++) {
				prChannelEntryField->aucChannelList[u4IdxII] =
				    prDomainSubBand->ucFirstChannelNum +
				    (u4IdxII * prDomainSubBand->ucChannelSpan);

#if 0
				switch (prChannelEntryField->aucChannelList[u4IdxII]) {
				case 1:
					ucSocialChnlSupport = 1;
					break;
				case 6:
					ucSocialChnlSupport = 6;
					break;
				case 11:
					ucSocialChnlSupport = 11;
					break;
				default:
					break;
				}

#endif
			}

			if (ucBufferSize >=
			    (P2P_ATTRI_LEN_CHANNEL_ENTRY +
			     prChannelEntryField->ucNumberOfChannels)) {
				ucBufferSize -=
				    (P2P_ATTRI_LEN_CHANNEL_ENTRY +
				     prChannelEntryField->ucNumberOfChannels);
			} else {
				break;
			}


			prChannelEntryField =
			    (P_CHANNEL_ENTRY_FIELD_T) ((ULONG) prChannelEntryField +
						       P2P_ATTRI_LEN_CHANNEL_ENTRY +
						       (ULONG) prChannelEntryField->
						       ucNumberOfChannels);

		}

#if 0
		if (prP2pConnSetting->ucListenChnl == 0) {
			prP2pConnSetting->ucListenChnl = P2P_DEFAULT_LISTEN_CHANNEL;

			if (ucSocialChnlSupport != 0) {
				/*                                
                                      
     */
				prP2pConnSetting->ucListenChnl = ucSocialChnlSupport;
			}
		}
#endif

		/*                       */
		/*                              
                                             
                                                                     
   */

		prP2pConnSetting->ucRfChannelListSize =
		    P2P_MAX_SUPPORTED_CHANNEL_LIST_SIZE - ucBufferSize;

#if 0
		if (prP2pConnSetting->ucOperatingChnl == 0) {	/*                               */

			if (scnQuerySparseChannel(prAdapter, NULL, &ucAutoChnl)) {
				break;	/*       */
			}

			ucBufferSize = prP2pConnSetting->ucRfChannelListSize;

			prChannelEntryField =
			    (P_CHANNEL_ENTRY_FIELD_T) prP2pConnSetting->aucChannelEntriesField;

			while (ucBufferSize != 0) {
				if (prChannelEntryField->ucNumberOfChannels != 0) {
					ucAutoChnl = prChannelEntryField->aucChannelList[0];
					break;	/*       */
				}

				else {
					prChannelEntryField =
					    (P_CHANNEL_ENTRY_FIELD_T) ((UINT_32) prChannelEntryField
								       +
								       P2P_ATTRI_LEN_CHANNEL_ENTRY +
								       (UINT_32)
								       prChannelEntryField->
								       ucNumberOfChannels);

					ucBufferSize -=
					    (P2P_ATTRI_LEN_CHANNEL_ENTRY +
					     prChannelEntryField->ucNumberOfChannels);
				}

			}



		}
#endif
		/*                                                                
                                                                                       
   */

		/*                       */
		/*                                                                                                 
                                                                                
   */

	} while (FALSE);

#if 0
	prP2pConnSetting->ucOperatingChnl = ucAutoChnl;
#endif
	return;
}				/*                           */

/*                                                                            */
/* 
                                                                                                   
 
                                            
 
                                                  
*/
/*                                                                            */
VOID
rlmFuncCommonChannelList(IN P_ADAPTER_T prAdapter,
			 IN P_CHANNEL_ENTRY_FIELD_T prChannelEntryII, IN UINT_8 ucChannelListSize)
{
	P_P2P_CONNECTION_SETTINGS_T prP2pConnSetting = (P_P2P_CONNECTION_SETTINGS_T) NULL;
	P_CHANNEL_ENTRY_FIELD_T prChannelEntryI =
	    (P_CHANNEL_ENTRY_FIELD_T) NULL, prChannelEntryIII = (P_CHANNEL_ENTRY_FIELD_T) NULL;
	UINT_8 aucCommonChannelList[P2P_MAX_SUPPORTED_CHANNEL_LIST_SIZE];
	UINT_8 ucOriChnlSize = 0, ucNewChnlSize = 0;


	do {

		ASSERT_BREAK(prAdapter != NULL);

		prP2pConnSetting = prAdapter->rWifiVar.prP2PConnSettings;

		prChannelEntryIII = (P_CHANNEL_ENTRY_FIELD_T) aucCommonChannelList;

		while (ucChannelListSize > 0) {

			prChannelEntryI =
			    (P_CHANNEL_ENTRY_FIELD_T) prP2pConnSetting->aucChannelEntriesField;
			ucOriChnlSize = prP2pConnSetting->ucRfChannelListSize;

			while (ucOriChnlSize > 0) {
				if (prChannelEntryI->ucRegulatoryClass ==
				    prChannelEntryII->ucRegulatoryClass) {
					prChannelEntryIII->ucRegulatoryClass =
					    prChannelEntryI->ucRegulatoryClass;
					/*                                                                                          */
					kalMemCopy(prChannelEntryIII->aucChannelList,
						   prChannelEntryII->aucChannelList,
						   prChannelEntryII->ucNumberOfChannels);
					prChannelEntryIII->ucNumberOfChannels =
					    prChannelEntryII->ucNumberOfChannels;

					ucNewChnlSize +=
					    P2P_ATTRI_LEN_CHANNEL_ENTRY +
					    prChannelEntryIII->ucNumberOfChannels;

					prChannelEntryIII =
					    (P_CHANNEL_ENTRY_FIELD_T) ((ULONG) prChannelEntryIII +
								       P2P_ATTRI_LEN_CHANNEL_ENTRY +
								       (ULONG) prChannelEntryIII->
								       ucNumberOfChannels);
				}

				ucOriChnlSize -=
				    (P2P_ATTRI_LEN_CHANNEL_ENTRY +
				     prChannelEntryI->ucNumberOfChannels);

				prChannelEntryI =
				    (P_CHANNEL_ENTRY_FIELD_T) ((ULONG) prChannelEntryI +
							       P2P_ATTRI_LEN_CHANNEL_ENTRY +
							       (ULONG) prChannelEntryI->
							       ucNumberOfChannels);


			}


			ucChannelListSize -=
			    (P2P_ATTRI_LEN_CHANNEL_ENTRY + prChannelEntryII->ucNumberOfChannels);

			prChannelEntryII = (P_CHANNEL_ENTRY_FIELD_T) ((ULONG) prChannelEntryII +
								      P2P_ATTRI_LEN_CHANNEL_ENTRY +
								      (ULONG) prChannelEntryII->
								      ucNumberOfChannels);


		}


		kalMemCopy(prP2pConnSetting->aucChannelEntriesField, aucCommonChannelList,
			   ucNewChnlSize);
		prP2pConnSetting->ucRfChannelListSize = ucNewChnlSize;

	} while (FALSE);

	return;
}				/*                          */


/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
UINT_8 rlmFuncFindOperatingClass(IN P_ADAPTER_T prAdapter, IN UINT_8 ucChannelNum)
{
	UINT_8 ucRegulatoryClass = 0, ucBufferSize = 0;
	P_P2P_CONNECTION_SETTINGS_T prP2pConnSetting = (P_P2P_CONNECTION_SETTINGS_T) NULL;
	P_CHANNEL_ENTRY_FIELD_T prChannelEntryField = (P_CHANNEL_ENTRY_FIELD_T) NULL;
	UINT_32 u4Idx = 0;

	do {
		ASSERT_BREAK(prAdapter != NULL);

		prP2pConnSetting = prAdapter->rWifiVar.prP2PConnSettings;
		ucBufferSize = prP2pConnSetting->ucRfChannelListSize;
		prChannelEntryField =
		    (P_CHANNEL_ENTRY_FIELD_T) prP2pConnSetting->aucChannelEntriesField;

		while (ucBufferSize != 0) {

			for (u4Idx = 0; u4Idx < prChannelEntryField->ucNumberOfChannels; u4Idx++) {
				if (prChannelEntryField->aucChannelList[u4Idx] == ucChannelNum) {
					ucRegulatoryClass = prChannelEntryField->ucRegulatoryClass;
					break;
				}

			}


			if (ucRegulatoryClass != 0) {
				break;	/*       */
			} else {
				prChannelEntryField =
				    (P_CHANNEL_ENTRY_FIELD_T) ((ULONG) prChannelEntryField +
							       P2P_ATTRI_LEN_CHANNEL_ENTRY +
							       (ULONG) prChannelEntryField->
							       ucNumberOfChannels);

				ucBufferSize -=
				    (P2P_ATTRI_LEN_CHANNEL_ENTRY +
				     prChannelEntryField->ucNumberOfChannels);
			}

		}


	} while (FALSE);

	return ucRegulatoryClass;
}				/*                           */


/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
BOOLEAN
rlmFuncFindAvailableChannel(IN P_ADAPTER_T prAdapter,
			    IN UINT_8 ucCheckChnl,
			    IN PUINT_8 pucSuggestChannel,
			    IN BOOLEAN fgIsSocialChannel, IN BOOLEAN fgIsDefaultChannel)
{
	BOOLEAN fgIsResultAvailable = FALSE;
	P_CHANNEL_ENTRY_FIELD_T prChannelEntry = (P_CHANNEL_ENTRY_FIELD_T) NULL;
	P_P2P_CONNECTION_SETTINGS_T prP2pConnSetting = (P_P2P_CONNECTION_SETTINGS_T) NULL;
	UINT_8 ucBufferSize = 0, ucIdx = 0, ucChannelSelected = 0;

	do {
		ASSERT_BREAK(prAdapter != NULL);

		if (fgIsDefaultChannel) {
			ucChannelSelected = P2P_DEFAULT_LISTEN_CHANNEL;
		}


		prP2pConnSetting = prAdapter->rWifiVar.prP2PConnSettings;
		ucBufferSize = prP2pConnSetting->ucRfChannelListSize;
		prChannelEntry = (P_CHANNEL_ENTRY_FIELD_T) prP2pConnSetting->aucChannelEntriesField;

		while ((ucBufferSize != 0) && (!fgIsResultAvailable)) {

			for (ucIdx = 0; ucIdx < prChannelEntry->ucNumberOfChannels; ucIdx++) {
				if ((!fgIsSocialChannel) ||
				    (prChannelEntry->aucChannelList[ucIdx] == 1) ||
				    (prChannelEntry->aucChannelList[ucIdx] == 6) ||
				    (prChannelEntry->aucChannelList[ucIdx] == 11)) {

					if (prChannelEntry->aucChannelList[ucIdx] <= 11) {
						/*       */
						ucChannelSelected =
						    prChannelEntry->aucChannelList[ucIdx];
					} else if ((prChannelEntry->aucChannelList[ucIdx] < 52)
						   && (prChannelEntry->aucChannelList[ucIdx] >
						       14)) {
						/*            */
						ucChannelSelected =
						    prChannelEntry->aucChannelList[ucIdx];
					}

					if (ucChannelSelected == ucCheckChnl) {
						fgIsResultAvailable = TRUE;
						break;
					}
				}

			}


			ucBufferSize -=
			    (P2P_ATTRI_LEN_CHANNEL_ENTRY + prChannelEntry->ucNumberOfChannels);

			prChannelEntry = (P_CHANNEL_ENTRY_FIELD_T) ((ULONG) prChannelEntry +
								    P2P_ATTRI_LEN_CHANNEL_ENTRY +
								    (ULONG) prChannelEntry->
								    ucNumberOfChannels);

		}



		if ((!fgIsResultAvailable) && (pucSuggestChannel != NULL)) {
			DBGLOG(P2P, TRACE,
			       ("The request channel %d is not available, sugguested channel:%d\n",
				ucCheckChnl, ucChannelSelected));
			/*                            */
			*pucSuggestChannel = ucChannelSelected;
		}


	} while (FALSE);

	return fgIsResultAvailable;
}

/*                                                                            */
/* 
        
 
            
 
              
*/
/*                                                                            */
ENUM_CHNL_EXT_T rlmDecideScoForAP(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo)
{
	P_DOMAIN_SUBBAND_INFO prSubband;
	P_DOMAIN_INFO_ENTRY prDomainInfo;
	UINT_8 ucSecondChannel, i, j;
	ENUM_CHNL_EXT_T eSCO; 
    ENUM_CHNL_EXT_T eTempSCO;

	eSCO = CHNL_EXT_SCN;
    eTempSCO = CHNL_EXT_SCN;

	if (prBssInfo->eBand == BAND_2G4) {
		if (prBssInfo->ucPrimaryChannel != 14) {
			eSCO = (prBssInfo->ucPrimaryChannel > 7) ? CHNL_EXT_SCB : CHNL_EXT_SCA;
		}
	} else {
		prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
		ASSERT(prDomainInfo);

		for (i = 0; i < MAX_SUBBAND_NUM; i++) {
			prSubband = &prDomainInfo->rSubBand[i];
			if (prSubband->ucBand == prBssInfo->eBand) {
				for (j = 0; j < prSubband->ucNumChannels; j++) {
					if ((prSubband->ucFirstChannelNum +
					     j * prSubband->ucChannelSpan)
					    == prBssInfo->ucPrimaryChannel) {
						eSCO = (j & 1) ? CHNL_EXT_SCB : CHNL_EXT_SCA;
						break;
					}
				}

				if (j < prSubband->ucNumChannels) {
					break;	/*       */
				}
			}
		}
	}

	/*                                                           */
	if (eSCO != CHNL_EXT_SCN) {
		ucSecondChannel = (eSCO == CHNL_EXT_SCA) ?
		    (prBssInfo->ucPrimaryChannel + 4) : (prBssInfo->ucPrimaryChannel - 4);

		if (!rlmDomainIsLegalChannel(prAdapter, prBssInfo->eBand, ucSecondChannel)) {
			eSCO = CHNL_EXT_SCN;
		}
	}

    /*                                    */
    if(IS_BSS_P2P(prBssInfo)) {
        /*         */
        if(p2pFuncIsAPMode(prAdapter->rWifiVar.prP2PConnSettings)) {
            if(prAdapter->rWifiVar.ucApSco == CHNL_EXT_SCA || 
               prAdapter->rWifiVar.ucApSco == CHNL_EXT_SCB) {
                eTempSCO = (ENUM_CHNL_EXT_T)prAdapter->rWifiVar.ucApSco;
            }
        }
        /*          */
        else { 
            if(prAdapter->rWifiVar.ucP2pGoSco == CHNL_EXT_SCA || 
               prAdapter->rWifiVar.ucP2pGoSco == CHNL_EXT_SCB) {
                eTempSCO = (ENUM_CHNL_EXT_T)prAdapter->rWifiVar.ucP2pGoSco;
            }
        }

        /*                                                                 */
        if(eTempSCO != CHNL_EXT_SCN) {
            ucSecondChannel = (eTempSCO == CHNL_EXT_SCA) ?
                (prBssInfo->ucPrimaryChannel+ 4) : (prBssInfo->ucPrimaryChannel- 4);        
            if(rlmDomainIsLegalChannel(prAdapter, prBssInfo->eBand, ucSecondChannel)) {
                eSCO = eTempSCO;
            }
        }
    } 

	return eSCO;
}