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
APPEND_VAR_IE_ENTRY_T txAssocReqIETable[] = {
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_HT_CAP), NULL, rlmReqGenerateHtCapIE}
	,			/*    */
#if CFG_SUPPORT_WPS2
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_WSC), NULL, rsnGenerateWSCIE}
	,			/*     */
#endif
#if CFG_SUPPORT_WAPI
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_WAPI), NULL, wapiGenerateWAPIIE}
	,			/*    */
#endif
#if CFG_RSN_MIGRATION
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_WPA), NULL, rsnGenerateWPAIE}
	,			/*     */
#endif
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_EXT_CAP), NULL, rlmReqGenerateExtCapIE}
	,			/*     */
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_WMM_INFO), NULL, mqmGenerateWmmInfoIE}
	,			/*     */
#if CFG_RSN_MIGRATION
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_RSN), NULL, rsnGenerateRSNIE}
	,			/*    */
#endif
};

#if CFG_SUPPORT_AAA
VERIFY_IE_ENTRY_T rxAssocReqIETable[] = {
	{ELEM_ID_RESERVED, NULL}	/*     */
};


APPEND_VAR_IE_ENTRY_T txAssocRespIETable[] = {
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_ERP), NULL, rlmRspGenerateErpIE}
	,			/*    */
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_HT_CAP), NULL, rlmRspGenerateHtCapIE}
	,			/*    */
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_HT_OP), NULL, rlmRspGenerateHtOpIE}
	,			/*    */
#if CFG_ENABLE_WIFI_DIRECT
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_OBSS_SCAN), NULL, rlmRspGenerateObssScanIE}
	,			/*    */
	{(0), p2pFuncCalculateP2p_IELenForAssocRsp, p2pFuncGenerateP2p_IEForAssocRsp}
	,			/*     */
#if CFG_SUPPORT_WFD
	{(0), wfdFuncCalculateWfdIELenForAssocRsp, wfdFuncGenerateWfdIEForAssocRsp}
	,			/*     */
#endif
#endif
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_EXT_CAP), NULL, rlmRspGenerateExtCapIE}
	,			/*     */
	{(ELEM_HDR_LEN + ELEM_MAX_LEN_WMM_PARAM), NULL, mqmGenerateWmmParamIE}
	,			/*     */

	{(0), p2pFuncCalculateWSC_IELenForAssocRsp, p2pFuncGenerateWSC_IEForAssocRsp}	/*     */

};
#endif

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
__KAL_INLINE__ UINT_16
assocBuildCapabilityInfo(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec)
{
	UINT_32 u4NonHTPhyType;
	UINT_16 u2CapInfo;


	ASSERT(prStaRec);


	/*                                    */
	u2CapInfo = CAP_INFO_ESS;
	u2CapInfo |= CAP_CF_STA_NOT_POLLABLE;

	if (prStaRec->u2CapInfo & CAP_INFO_PRIVACY) {
		u2CapInfo |= CAP_INFO_PRIVACY;
	}


	/*         */
	if (prStaRec->fgHasBasicPhyType) {
		u4NonHTPhyType = prStaRec->ucNonHTBasicPhyType;

		if ((rNonHTPhyAttributes[u4NonHTPhyType].fgIsShortPreambleOptionImplemented) && ((prAdapter->rWifiVar.ePreambleType == PREAMBLE_TYPE_SHORT) ||	/*                                      */
												 ((prAdapter->rWifiVar.ePreambleType == PREAMBLE_TYPE_AUTO) && (prStaRec->u2CapInfo & CAP_INFO_SHORT_PREAMBLE)))) {

			/*                                                                      
                                                                         
                                       
    */
			u2CapInfo |= CAP_INFO_SHORT_PREAMBLE;
		}

		if (rNonHTPhyAttributes[u4NonHTPhyType].fgIsShortSlotTimeOptionImplemented &&
		    prAdapter->rWifiVar.fgIsShortSlotTimeOptionEnable) {
			u2CapInfo |= CAP_INFO_SHORT_SLOT_TIME;
		}
	}

	DBGLOG(SAA, LOUD, ("ASSOC REQ: Compose Capability = 0x%04x for Target BSS [" MACSTR "].\n",
			   u2CapInfo, MAC2STR(prStaRec->aucMacAddr)));


	return u2CapInfo;

}				/*                                   */


/*                                                                            */
/* 
                                                                                     
                       
 
                                                                
 
                
*/
/*                                                                            */
__KAL_INLINE__ VOID
assocBuildReAssocReqFrameCommonIEs(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo)
{
	P_CONNECTION_SETTINGS_T prConnSettings;
	P_STA_RECORD_T prStaRec;
	PUINT_8 pucBuffer;
	UINT_16 u2SupportedRateSet;
	UINT_8 aucAllSupportedRates[RATE_NUM] = { 0 };
	UINT_8 ucAllSupportedRatesLen;
	UINT_8 ucSupRatesLen;
	UINT_8 ucExtSupRatesLen;

	prConnSettings = &(prAdapter->rWifiVar.rConnSettings);
	ASSERT(prMsduInfo);
	ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
	ASSERT(prStaRec);

	if (!prStaRec) {
		return;
	}

	pucBuffer = (PUINT_8) ((UINT_32) prMsduInfo->prPacket +
			       (UINT_32) prMsduInfo->u2FrameLength);
	ASSERT(pucBuffer);

	if (IS_STA_IN_AIS(prStaRec)) {

		/*                        */
		SSID_IE(pucBuffer)->ucId = ELEM_ID_SSID;

		/*                                                                       
                                                                               
   */

		COPY_SSID(SSID_IE(pucBuffer)->aucSSID,
			  SSID_IE(pucBuffer)->ucLength,
			  prConnSettings->aucSSID, prConnSettings->ucSSIDLen);

		prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
		pucBuffer += IE_SIZE(pucBuffer);

	}
#if CFG_ENABLE_WIFI_DIRECT
	else if ((prAdapter->fgIsP2PRegistered) && (IS_STA_IN_P2P(prStaRec))) {
		pucBuffer = p2pBuildReAssocReqFrameCommonIEs(prAdapter, prMsduInfo, pucBuffer);
	}
#endif
#if CFG_ENABLE_BT_OVER_WIFI
	else if (IS_STA_IN_BOW(prStaRec)) {

		SSID_IE(pucBuffer)->ucId = ELEM_ID_SSID;

		/*                                                                       
                                                                               
   */

		COPY_SSID(SSID_IE(pucBuffer)->aucSSID,
			  SSID_IE(pucBuffer)->ucLength,
			  prConnSettings->aucSSID, prConnSettings->ucSSIDLen);

		prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
		pucBuffer += IE_SIZE(pucBuffer);
	}
#endif

	else {
		/*                                */
	}

	/*                                                             
                                                                        
                                   
                                                                      
                                                                           
  */
	/*                                                                */
	/*                                                          */

	if (prStaRec->fgHasBasicPhyType) {
		UINT_32 u4NonHTPhyType;


		u4NonHTPhyType = prStaRec->ucNonHTBasicPhyType;

		u2SupportedRateSet = (prStaRec->u2OperationalRateSet &
				      rNonHTPhyAttributes[u4NonHTPhyType].u2SupportedRateSet);

		ASSERT(u2SupportedRateSet);

		if (!u2SupportedRateSet) {
			u2SupportedRateSet = rNonHTPhyAttributes[u4NonHTPhyType].u2SupportedRateSet;
		}

		/*                                                                                  */

		rateGetDataRatesFromRateSet(u2SupportedRateSet,
					    0, aucAllSupportedRates, &ucAllSupportedRatesLen);

		ucSupRatesLen = ((ucAllSupportedRatesLen > ELEM_MAX_LEN_SUP_RATES) ?
				 ELEM_MAX_LEN_SUP_RATES : ucAllSupportedRatesLen);

		ucExtSupRatesLen = ucAllSupportedRatesLen - ucSupRatesLen;


		/*                                   */
		if (ucSupRatesLen) {
			SUP_RATES_IE(pucBuffer)->ucId = ELEM_ID_SUP_RATES;
			SUP_RATES_IE(pucBuffer)->ucLength = ucSupRatesLen;
			kalMemCopy(SUP_RATES_IE(pucBuffer)->aucSupportedRates,
				   aucAllSupportedRates, ucSupRatesLen);

			prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
			pucBuffer += IE_SIZE(pucBuffer);
		}


		/*                                            */
		if (ucExtSupRatesLen) {

			EXT_SUP_RATES_IE(pucBuffer)->ucId = ELEM_ID_EXTENDED_SUP_RATES;
			EXT_SUP_RATES_IE(pucBuffer)->ucLength = ucExtSupRatesLen;

			kalMemCopy(EXT_SUP_RATES_IE(pucBuffer)->aucExtSupportedRates,
				   &aucAllSupportedRates[ucSupRatesLen], ucExtSupRatesLen);

			prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
			pucBuffer += IE_SIZE(pucBuffer);
		}
	}

	return;
}				/*                                             */


/*                                                                            */
/* 
                                                                                
                         
 
                                                               
                                                                
                                                          
                                                                                  
 
                
*/
/*                                                                            */
__KAL_INLINE__ VOID
assocComposeReAssocReqFrameHeaderAndFF(IN P_ADAPTER_T prAdapter,
				       IN P_STA_RECORD_T prStaRec,
				       IN PUINT_8 pucBuffer,
				       IN UINT_8 aucMACAddress[], IN OUT PUINT_16 pu2PayloadLen)
{
	P_WLAN_ASSOC_REQ_FRAME_T prAssocFrame;
	BOOLEAN fgIsReAssoc;

	UINT_16 u2FrameCtrl;
	UINT_16 u2CapInfo;
	UINT_16 u2ListenInterval;


	ASSERT(prStaRec);
	ASSERT(pucBuffer);
	ASSERT(aucMACAddress);
	ASSERT(pu2PayloadLen);

	prAssocFrame = (P_WLAN_ASSOC_REQ_FRAME_T) pucBuffer;
	fgIsReAssoc = prStaRec->fgIsReAssoc;

	/*                                                                       */
	/*                               */
	if (fgIsReAssoc) {
		u2FrameCtrl = MAC_FRAME_REASSOC_REQ;
	} else {
		u2FrameCtrl = MAC_FRAME_ASSOC_REQ;
	}
	WLAN_SET_FIELD_16(&prAssocFrame->u2FrameCtrl, u2FrameCtrl);

	/*                                      */
	COPY_MAC_ADDR(prAssocFrame->aucDestAddr, prStaRec->aucMacAddr);

	/*                                         */
	COPY_MAC_ADDR(prAssocFrame->aucSrcAddr, aucMACAddress);

	/*                                         */
	COPY_MAC_ADDR(prAssocFrame->aucBSSID, prStaRec->aucMacAddr);

	/*                                                                                    */
	prAssocFrame->u2SeqCtrl = 0;


	/*                                                                                               */
	u2CapInfo = assocBuildCapabilityInfo(prAdapter, prStaRec);

	/*                                        */
	WLAN_SET_FIELD_16(&prAssocFrame->u2CapInfo, u2CapInfo);


	/*                                                                        
                                             */
	if (prStaRec->ucDTIMPeriod) {
		u2ListenInterval = prStaRec->ucDTIMPeriod * DEFAULT_LISTEN_INTERVAL_BY_DTIM_PERIOD;
	} else {
		DBGLOG(SAA, TRACE, ("Use default listen interval\n"));
		u2ListenInterval = DEFAULT_LISTEN_INTERVAL;
	}
	prStaRec->u2ListenInterval = u2ListenInterval;

	/*                                 */
	WLAN_SET_FIELD_16(&prAssocFrame->u2ListenInterval, u2ListenInterval);


	/*                                                                              */
	/*                                    */
	if (prStaRec->fgIsReAssoc) {
		if (IS_STA_IN_AIS(prStaRec)) {

			P_AIS_BSS_INFO_T prAisBssInfo =
			    &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
			P_WLAN_REASSOC_REQ_FRAME_T prReAssocFrame =
			    (P_WLAN_REASSOC_REQ_FRAME_T) prAssocFrame;

			COPY_MAC_ADDR(prReAssocFrame->aucCurrentAPAddr, prAisBssInfo->aucBSSID);
		} else {
			ASSERT(0);	/*                                                  */
		}

		*pu2PayloadLen = (CAP_INFO_FIELD_LEN +
				  LISTEN_INTERVAL_FIELD_LEN + CURR_AP_ADDR_FIELD_LEN);
	} else {
		*pu2PayloadLen = (CAP_INFO_FIELD_LEN + LISTEN_INTERVAL_FIELD_LEN);
	}

	return;
}				/*                                      */


/*                                                                            */
/* 
                                                                  
 
                                                           
 
                                                                          
                                                                    
*/
/*                                                                            */
WLAN_STATUS assocSendReAssocReqFrame(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec)
{
	P_MSDU_INFO_T prMsduInfo;
	P_BSS_INFO_T prBssInfo;

	UINT_16 u2PayloadLen;
	UINT_16 u2EstimatedFrameLen;
	UINT_16 u2EstimatedExtraIELen;
	BOOLEAN fgIsReAssoc;
	UINT_32 i;


	ASSERT(prStaRec);

	/*                                                      */
	fgIsReAssoc = prStaRec->fgIsReAssoc;

	/*                                                                          */
	if (fgIsReAssoc) {
		u2EstimatedFrameLen = MAC_TX_RESERVED_FIELD +
		    WLAN_MAC_MGMT_HEADER_LEN +
		    CAP_INFO_FIELD_LEN +
		    LISTEN_INTERVAL_FIELD_LEN +
		    CURR_AP_ADDR_FIELD_LEN +
		    (ELEM_HDR_LEN + ELEM_MAX_LEN_SSID) +
		    (ELEM_HDR_LEN + ELEM_MAX_LEN_SUP_RATES) +
		    (ELEM_HDR_LEN + (RATE_NUM - ELEM_MAX_LEN_SUP_RATES));
	} else {
		u2EstimatedFrameLen = MAC_TX_RESERVED_FIELD +
		    WLAN_MAC_MGMT_HEADER_LEN +
		    CAP_INFO_FIELD_LEN +
		    LISTEN_INTERVAL_FIELD_LEN +
		    (ELEM_HDR_LEN + ELEM_MAX_LEN_SSID) +
		    (ELEM_HDR_LEN + ELEM_MAX_LEN_SUP_RATES) +
		    (ELEM_HDR_LEN + (RATE_NUM - ELEM_MAX_LEN_SUP_RATES));
	}

	/*                   */
	u2EstimatedExtraIELen = 0;

#if CFG_ENABLE_WIFI_DIRECT_CFG_80211 && CFG_ENABLE_WIFI_DIRECT
	if (prStaRec->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX) {
		if ((prAdapter->fgIsP2PRegistered)) {
			u2EstimatedExtraIELen = p2pCalculate_IEForAssocReq(prAdapter,
									   prStaRec->ucNetTypeIndex,
									   prStaRec);
		} else {
			DBGLOG(P2P, TRACE, ("Function Linker Lost.\n"));
			ASSERT(FALSE);
		}
	} else {
		for (i = 0; i < sizeof(txAssocReqIETable) / sizeof(APPEND_VAR_IE_ENTRY_T); i++) {
			if (txAssocReqIETable[i].u2EstimatedFixedIELen != 0) {
				u2EstimatedExtraIELen += txAssocReqIETable[i].u2EstimatedFixedIELen;
			} else {
				u2EstimatedExtraIELen +=
				    (UINT_16) txAssocReqIETable[i].
				    pfnCalculateVariableIELen(prAdapter, prStaRec->ucNetTypeIndex,
							      prStaRec);
			}
		}
	}
#else
	for (i = 0; i < sizeof(txAssocReqIETable) / sizeof(APPEND_VAR_IE_ENTRY_T); i++) {
		if (txAssocReqIETable[i].u2EstimatedFixedIELen != 0) {
			u2EstimatedExtraIELen += txAssocReqIETable[i].u2EstimatedFixedIELen;
		} else {
			u2EstimatedExtraIELen +=
			    (UINT_16) txAssocReqIETable[i].pfnCalculateVariableIELen(prAdapter,
										     prStaRec->
										     ucNetTypeIndex,
										     prStaRec);
		}
	}
#endif

	u2EstimatedFrameLen += u2EstimatedExtraIELen;

	/*                        */
	if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
		DBGLOG(SAA, WARN, ("No PKT_INFO_T for sending (Re)Assoc Request.\n"));
		return WLAN_STATUS_RESOURCES;
	}
	/*                                                                                     */
	ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);
	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);


	/*                                */
	assocComposeReAssocReqFrameHeaderAndFF(prAdapter,
					       prStaRec,
					       (PUINT_8) ((UINT_32) (prMsduInfo->prPacket) +
							  MAC_TX_RESERVED_FIELD),
					       prBssInfo->aucOwnMacAddr, &u2PayloadLen);

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

	/*                                                                           */
	assocBuildReAssocReqFrameCommonIEs(prAdapter, prMsduInfo);


	/*                                  */
#if CFG_ENABLE_WIFI_DIRECT_CFG_80211 && CFG_ENABLE_WIFI_DIRECT
	if (prStaRec->ucNetTypeIndex == NETWORK_TYPE_P2P_INDEX) {
		if ((prAdapter->fgIsP2PRegistered)) {
			p2pGenerate_IEForAssocReq(prAdapter, prMsduInfo);
		} else {
			DBGLOG(P2P, TRACE, ("Function Linker Lost.\n"));
			ASSERT(FALSE);
		}
	} else {
		/*           */
		for (i = 0; i < sizeof(txAssocReqIETable) / sizeof(APPEND_VAR_IE_ENTRY_T); i++) {
			if (txAssocReqIETable[i].pfnAppendIE) {
				txAssocReqIETable[i].pfnAppendIE(prAdapter, prMsduInfo);
			}
		}
	}
#else
	/*           */
	for (i = 0; i < sizeof(txAssocReqIETable) / sizeof(APPEND_VAR_IE_ENTRY_T); i++) {
		if (txAssocReqIETable[i].pfnAppendIE) {
			txAssocReqIETable[i].pfnAppendIE(prAdapter, prMsduInfo);
		}
	}
#endif

	/*                                                      */
	if (IS_STA_IN_AIS(prStaRec)) {
		P_WLAN_ASSOC_REQ_FRAME_T prAssocFrame;

		prAssocFrame =
		    (P_WLAN_ASSOC_REQ_FRAME_T) ((UINT_32) (prMsduInfo->prPacket) +
						MAC_TX_RESERVED_FIELD);

#if CFG_RSN_MIGRATION
		kalUpdateReAssocReqInfo(prAdapter->prGlueInfo,
					(PUINT_8) &prAssocFrame->u2CapInfo,
					prMsduInfo->u2FrameLength - offsetof(WLAN_ASSOC_REQ_FRAME_T,
									     u2CapInfo),
					fgIsReAssoc);
#endif
	}
#if CFG_ENABLE_WIFI_DIRECT
	if ((prAdapter->fgIsP2PRegistered) && (IS_STA_IN_P2P(prStaRec))) {
		P_WLAN_ASSOC_REQ_FRAME_T prAssocFrame;

		prAssocFrame =
		    (P_WLAN_ASSOC_REQ_FRAME_T) ((UINT_32) (prMsduInfo->prPacket) +
						MAC_TX_RESERVED_FIELD);


		kalP2PUpdateAssocInfo(prAdapter->prGlueInfo,
				      (PUINT_8) &prAssocFrame->u2CapInfo,
				      prMsduInfo->u2FrameLength - offsetof(WLAN_ASSOC_REQ_FRAME_T,
									   u2CapInfo), fgIsReAssoc);
	}
#endif

	/*                                                                      */

	/*                                                                     */
	nicTxEnqueueMsdu(prAdapter, prMsduInfo);

	return WLAN_STATUS_SUCCESS;
}				/*                                   */


/*                                                                            */
/* 
                                                                                   
                            
 
                                                      
 
                                                                                        
                                                                   
*/
/*                                                                            */
WLAN_STATUS assocCheckTxReAssocReqFrame(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo)
{
	P_WLAN_ASSOC_REQ_FRAME_T prAssocReqFrame;
	P_STA_RECORD_T prStaRec;
	UINT_16 u2TxFrameCtrl;


	ASSERT(prMsduInfo);
	ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

	prAssocReqFrame = (P_WLAN_ASSOC_REQ_FRAME_T) (prMsduInfo->prPacket);
	ASSERT(prAssocReqFrame);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
	ASSERT(prStaRec);

	if (!prStaRec) {
		return WLAN_STATUS_INVALID_PACKET;
	}
	/*                                                                  */
	u2TxFrameCtrl = prAssocReqFrame->u2FrameCtrl;	/*                                */
	u2TxFrameCtrl &= MASK_FRAME_TYPE;
	if (prStaRec->fgIsReAssoc) {
		if (u2TxFrameCtrl != MAC_FRAME_REASSOC_REQ) {
			return WLAN_STATUS_FAILURE;
		}
	} else {
		if (u2TxFrameCtrl != MAC_FRAME_ASSOC_REQ) {
			return WLAN_STATUS_FAILURE;
		}
	}

	return WLAN_STATUS_SUCCESS;

}				/*                                      */


/*                                                                            */
/* 
                                                                                    
                            
 
                                                      
 
                                                                                        
                                                                   
*/
/*                                                                            */
WLAN_STATUS assocCheckTxReAssocRespFrame(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo)
{
	P_WLAN_ASSOC_RSP_FRAME_T prAssocRspFrame;
	P_STA_RECORD_T prStaRec;
	UINT_16 u2TxFrameCtrl;


	ASSERT(prMsduInfo);
	ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

	prAssocRspFrame = (P_WLAN_ASSOC_RSP_FRAME_T) (prMsduInfo->prPacket);
	ASSERT(prAssocRspFrame);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
	ASSERT(prStaRec);

	if (!prStaRec) {
		return WLAN_STATUS_INVALID_PACKET;
	}
	/*                                                               */
	u2TxFrameCtrl = prAssocRspFrame->u2FrameCtrl;	/*                                */
	u2TxFrameCtrl &= MASK_FRAME_TYPE;
	if (prStaRec->fgIsReAssoc) {
		if (u2TxFrameCtrl != MAC_FRAME_REASSOC_RSP) {
			return WLAN_STATUS_FAILURE;
		}
	} else {
		if (u2TxFrameCtrl != MAC_FRAME_ASSOC_RSP) {
			return WLAN_STATUS_FAILURE;
		}
	}

	return WLAN_STATUS_SUCCESS;

}				/*                                       */


/*                                                                            */
/* 
                                                                                    
                         
 
                                                                     
                                                                                         
 
                                                                                        
                                                                   
*/
/*                                                                            */
WLAN_STATUS
assocCheckRxReAssocRspFrameStatus(IN P_ADAPTER_T prAdapter,
				  IN P_SW_RFB_T prSwRfb, OUT PUINT_16 pu2StatusCode)
{
	P_STA_RECORD_T prStaRec;
	P_WLAN_ASSOC_RSP_FRAME_T prAssocRspFrame;
	UINT_16 u2RxFrameCtrl;
	UINT_16 u2RxCapInfo;
	UINT_16 u2RxStatusCode;
	UINT_16 u2RxAssocId;


	ASSERT(prSwRfb);
	ASSERT(pu2StatusCode);

	if ((prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) < (CAP_INFO_FIELD_LEN +
							     STATUS_CODE_FIELD_LEN +
							     AID_FIELD_LEN)) {
		ASSERT(0);
		return WLAN_STATUS_FAILURE;
	}

	DBGLOG(SAA, LOUD,
	       ("prSwRfb->u2PayloadLength = %d\n", prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen));

	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);
	ASSERT(prStaRec);

	if (!prStaRec) {
		return WLAN_STATUS_INVALID_PACKET;
	}
	/*                                             */
	prAssocRspFrame = (P_WLAN_ASSOC_RSP_FRAME_T) prSwRfb->pvHeader;

	/*                                                      */
	/*                                                                   */
	u2RxFrameCtrl = prAssocRspFrame->u2FrameCtrl;	/*                                */
	u2RxFrameCtrl &= MASK_FRAME_TYPE;
	if (prStaRec->fgIsReAssoc) {
		if (u2RxFrameCtrl != MAC_FRAME_REASSOC_RSP) {
			return WLAN_STATUS_FAILURE;
		}
	} else {
		if (u2RxFrameCtrl != MAC_FRAME_ASSOC_RSP) {
			return WLAN_STATUS_FAILURE;
		}
	}

	/*                                                                 */
	/*                                                               */
	u2RxCapInfo = prAssocRspFrame->u2CapInfo;	/*                                */

	/*                                                                     */
	u2RxStatusCode = prAssocRspFrame->u2StatusCode;	/*                                */

	/*                      */
	/*                                                                          
                                                                      
                                                                 
  */
	if (u2RxStatusCode == STATUS_CODE_SUCCESSFUL) {
#if CFG_SUPPORT_WAPI
		if (prAdapter->rWifiVar.rConnSettings.fgWapiMode) {
			/*                                                                       */
			/*                                                                              */
			DBGLOG(SEC, TRACE,
			       ("Workaround the WAPI AP allow the customer to use WZC to join\n"));
		} else
#endif
#if CFG_ENABLE_WIFI_DIRECT
		if (prAdapter->fgIsP2PRegistered && 1) {
			/*                   */
		} else
#endif
		{
		}

#if CFG_STRICT_CHECK_CAPINFO_PRIVACY
		if ((prStaRec->u2CapInfo & CAP_INFO_PRIVACY) ^ (u2RxCapInfo & CAP_INFO_PRIVACY)) {
			u2RxStatusCode = STATUS_CODE_CAP_NOT_SUPPORTED;
		}
#endif
	}

	if (u2RxStatusCode == STATUS_CODE_SUCCESSFUL) {
#if CFG_RSN_MIGRATION
		/*                                                              
                                         */
		kalUpdateReAssocRspInfo(prAdapter->prGlueInfo,
					(PUINT_8) &prAssocRspFrame->u2CapInfo,
					(UINT_32) (prSwRfb->u2PacketLen));
#endif
	}
	/*                                    */
	if (u2RxStatusCode == STATUS_CODE_SUCCESSFUL) {
		prStaRec->u2CapInfo = u2RxCapInfo;

		/*                                                               */
		u2RxAssocId = prAssocRspFrame->u2AssocId;	/*                                */

		/*                                                       
                                                             
                                     
                  
                                                      
                                                         
                                             
   */
		if ((u2RxAssocId & BIT(6)) &&
		    (u2RxAssocId & BIT(7)) && !(u2RxAssocId & BITS(8, 15))) {
			prStaRec->u2AssocId = u2RxAssocId & ~BITS(6, 7);
		} else {
			prStaRec->u2AssocId = u2RxAssocId & ~AID_MSB;
#if CFG_SUPPORT_802_11W
			if (prStaRec->ucNetTypeIndex == NETWORK_TYPE_AIS_INDEX) {
				P_AIS_SPECIFIC_BSS_INFO_T prBssSpecInfo;

				prBssSpecInfo = &prAdapter->rWifiVar.rAisSpecificBssInfo;
				ASSERT(prBssSpecInfo);

				prBssSpecInfo->ucSaQueryTimedOut = 0;
			}
#endif
		}
	}
#if CFG_SUPPORT_802_11W
	if (u2RxStatusCode == STATUS_CODE_AUTH_ALGORITHM_NOT_SUPPORTED) {
		DBGLOG(SAA, INFO, ("AP rejected due the authentication algorithm not support\n"));
	} else if (u2RxStatusCode == STATUS_CODE_ASSOC_REJECTED_TEMPORARILY) {
		PUINT_8 pucIE, pucTime;
		UINT_16 u2IELength;
		UINT_16 u2Offset = 0;

		u2IELength = prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen;
		pucIE = (PUINT_8) ((UINT_32) prSwRfb->pvHeader + prSwRfb->u2HeaderLen);

		IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
			if (ELEM_ID_TIMEOUT_INTERVAL == IE_ID(pucIE) && IE_LEN(pucIE) == 5) {
				pucTime = ((P_IE_HDR_T) pucIE)->aucInfo;
				if (pucTime[0] == ACTION_SA_TIMEOUT_ASSOC_COMEBACK) {
					UINT_32 tu;
					WLAN_GET_FIELD_32(pucTime + 1, &tu);
					DBGLOG(SAA, INFO,
					       ("AP rejected association temporarily; comeback duration %u TU "
						"(%u ms)\n", tu, TU_TO_MSEC(tu)));
					if (tu > TX_ASSOCIATION_RETRY_TIMEOUT_TU) {
						DBGLOG(SAA, INFO,
						       ("Update timer based on comeback duration\n"));
						/*                                        */
					}
				}
				break;
			}
		}		/*                    */
	}
#endif
	*pu2StatusCode = u2RxStatusCode;

	return WLAN_STATUS_SUCCESS;

}				/*                                            */


/*                                                                            */
/* 
                                                            
 
                                                               
                                                                
                                                     
                                                                
 
                
*/
/*                                                                            */
__KAL_INLINE__ VOID
assocComposeDisassocFrame(IN P_STA_RECORD_T prStaRec,
			  IN PUINT_8 pucBuffer, IN UINT_8 aucMACAddress[], IN UINT_16 u2ReasonCode)
{
	P_WLAN_DISASSOC_FRAME_T prDisAssocFrame;
	UINT_16 u2FrameCtrl;

	ASSERT(pucBuffer);
	ASSERT(pucBuffer);
	ASSERT(aucMACAddress);

	prDisAssocFrame = (P_WLAN_DISASSOC_FRAME_T) pucBuffer;

	/*                                                              */
	/*                               */
	u2FrameCtrl = MAC_FRAME_DISASSOC;

	WLAN_SET_FIELD_16(&prDisAssocFrame->u2FrameCtrl, u2FrameCtrl);

	/*                                      */
	COPY_MAC_ADDR(prDisAssocFrame->aucDestAddr, prStaRec->aucMacAddr);

	/*                                         */
	COPY_MAC_ADDR(prDisAssocFrame->aucSrcAddr, aucMACAddress);

	/*                                         */
	COPY_MAC_ADDR(prDisAssocFrame->aucBSSID, prStaRec->aucMacAddr);

	/*                                                                                    */
	prDisAssocFrame->u2SeqCtrl = 0;

	/*                                                                              */
	/*                             */
	WLAN_SET_FIELD_16(&prDisAssocFrame->u2ReasonCode, u2ReasonCode);

	return;
}				/*                                    */


/*                                                                            */
/* 
                                                         
 
                                                           
                                                            
 
                                                                          
                                                                    
*/
/*                                                                            */
WLAN_STATUS
assocSendDisAssocFrame(IN P_ADAPTER_T prAdapter,
		       IN P_STA_RECORD_T prStaRec, IN UINT_16 u2ReasonCode)
{
	PUINT_8 pucMacAddress;
	P_MSDU_INFO_T prMsduInfo;
	UINT_16 u2PayloadLen;
	UINT_16 u2EstimatedFrameLen;
	/*                                         */


	ASSERT(prStaRec);

	/*                                                      */
	/*                                                                   */
	u2EstimatedFrameLen = MAC_TX_RESERVED_FIELD +
	    WLAN_MAC_MGMT_HEADER_LEN + REASON_CODE_FIELD_LEN;

	/*                        */
	if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
		DBGLOG(SAA, WARN, ("No PKT_INFO_T for sending DisAssoc.\n"));
		return WLAN_STATUS_RESOURCES;
	}
	/*                                                                             */
	ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

	pucMacAddress = prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex].aucOwnMacAddr;

	/*                                */
	assocComposeDisassocFrame(prStaRec,
				  (PUINT_8) ((UINT_32) (prMsduInfo->prPacket) +
					     MAC_TX_RESERVED_FIELD), pucMacAddress, u2ReasonCode);

#if CFG_SUPPORT_802_11W
	if (rsnCheckBipKeyInstalled(prAdapter, prStaRec)) {
		P_WLAN_DISASSOC_FRAME_T prDisassocFrame;

		prDisassocFrame =
		    (P_WLAN_DEAUTH_FRAME_T) (PUINT_8) ((UINT_32) (prMsduInfo->prPacket) +
						       MAC_TX_RESERVED_FIELD);

		prDisassocFrame->u2FrameCtrl |= MASK_FC_PROTECTED_FRAME;
		DBGLOG(TX, WARN, ("assocSendDisAssocFrame with protection\n"));
	}
#endif

	u2PayloadLen = REASON_CODE_FIELD_LEN;

	/*                                         */
	ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

	prMsduInfo->eSrc = TX_PACKET_MGMT;
	prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
	prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
	prMsduInfo->ucNetworkType = prStaRec->ucNetTypeIndex;
	prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
	prMsduInfo->fgIs802_1x = FALSE;
	prMsduInfo->fgIs802_11 = TRUE;
	prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen;
	prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
	prMsduInfo->pfTxDoneHandler = NULL;
	prMsduInfo->fgIsBasicRate = TRUE;

	/*                                                                     */
	nicTxEnqueueMsdu(prAdapter, prMsduInfo);

	return WLAN_STATUS_SUCCESS;
}				/*                                 */


/*                                                                            */
/* 
                                                                               
                                       
 
                                                                 
                                           
                                                                                       
 
                                                                                        
                                                                   
*/
/*                                                                            */
WLAN_STATUS
assocProcessRxDisassocFrame(IN P_ADAPTER_T prAdapter,
			    IN P_SW_RFB_T prSwRfb, IN UINT_8 aucBSSID[], OUT PUINT_16 pu2ReasonCode)
{
	P_WLAN_DISASSOC_FRAME_T prDisassocFrame;
	UINT_16 u2RxReasonCode;


	ASSERT(prSwRfb);
	ASSERT(aucBSSID);
	ASSERT(pu2ReasonCode);

	/*                                        */
	prDisassocFrame = (P_WLAN_DISASSOC_FRAME_T) prSwRfb->pvHeader;

	/*                                                 */
	if ((prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) < REASON_CODE_FIELD_LEN) {
		ASSERT(0);
		return WLAN_STATUS_FAILURE;
	}

	/*                                                          */
	if (UNEQUAL_MAC_ADDR(prDisassocFrame->aucBSSID, aucBSSID)) {
		DBGLOG(SAA, LOUD, ("Ignore Disassoc Frame from other BSS [" MACSTR "]\n",
				   MAC2STR(prDisassocFrame->aucSrcAddr)));
		return WLAN_STATUS_FAILURE;
	}
	/*                                                              */
	WLAN_GET_FIELD_16(&prDisassocFrame->u2ReasonCode, &u2RxReasonCode);
	*pu2ReasonCode = u2RxReasonCode;

	return WLAN_STATUS_SUCCESS;

}				/*                                      */


#if CFG_SUPPORT_AAA
/*                                                                            */
/* 
                                                                                
                                  
 
                                                                 
                                                                 
                                                                                                     
 
                                                                                        
                                                                   
*/
/*                                                                            */
WLAN_STATUS
assocProcessRxAssocReqFrame(IN P_ADAPTER_T prAdapter,
			    IN P_SW_RFB_T prSwRfb, OUT PUINT_16 pu2StatusCode)
{
	P_WLAN_ASSOC_REQ_FRAME_T prAssocReqFrame;
	P_STA_RECORD_T prStaRec;
	P_BSS_INFO_T prBssInfo;
	P_IE_SSID_T prIeSsid = (P_IE_SSID_T) NULL;
	P_RSN_INFO_ELEM_T prIeRsn = (P_RSN_INFO_ELEM_T) NULL;
	P_IE_SUPPORTED_RATE_T prIeSupportedRate = (P_IE_SUPPORTED_RATE_T) NULL;
	P_IE_EXT_SUPPORTED_RATE_T prIeExtSupportedRate = (P_IE_EXT_SUPPORTED_RATE_T) NULL;
	PUINT_8 pucIE, pucIEStart;
	UINT_16 u2IELength;
	UINT_16 u2Offset = 0;
	UINT_16 u2StatusCode = STATUS_CODE_SUCCESSFUL;
	UINT_16 u2RxFrameCtrl;
	UINT_16 u2BSSBasicRateSet;
	BOOLEAN fgIsUnknownBssBasicRate;
	UINT_32 i;


	ASSERT(prAdapter);
	ASSERT(prSwRfb);
	ASSERT(pu2StatusCode);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prSwRfb->ucStaRecIdx);

	if (prStaRec == NULL) {
		return WLAN_STATUS_FAILURE;
	}
	/*                                         */
	prAssocReqFrame = (P_WLAN_ASSOC_REQ_FRAME_T) prSwRfb->pvHeader;

	/*                                                  */
	if ((prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) <
	    (CAP_INFO_FIELD_LEN + LISTEN_INTERVAL_FIELD_LEN)) {
		ASSERT(0);
		return WLAN_STATUS_FAILURE;
	}

	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

	/*                                                          */
	if (UNEQUAL_MAC_ADDR(prAssocReqFrame->aucBSSID, prBssInfo->aucBSSID)) {
		return WLAN_STATUS_FAILURE;	/*                        */
	}
	/*                                                                   */
	u2RxFrameCtrl = prAssocReqFrame->u2FrameCtrl;	/*                                */
	u2RxFrameCtrl &= MASK_FRAME_TYPE;
	if (MAC_FRAME_REASSOC_REQ == u2RxFrameCtrl) {
		prStaRec->fgIsReAssoc = TRUE;

		u2IELength = (prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) -
		    (UINT_16) (OFFSET_OF(WLAN_REASSOC_REQ_FRAME_T, aucInfoElem[0]) -
			       WLAN_MAC_MGMT_HEADER_LEN);

		pucIEStart = pucIE =
		    ((P_WLAN_REASSOC_REQ_FRAME_T) (prSwRfb->pvHeader))->aucInfoElem;
	} else {
		prStaRec->fgIsReAssoc = FALSE;

		u2IELength = (prSwRfb->u2PacketLen - prSwRfb->u2HeaderLen) -
		    (UINT_16) (OFFSET_OF(WLAN_ASSOC_REQ_FRAME_T, aucInfoElem[0]) -
			       WLAN_MAC_MGMT_HEADER_LEN);

		pucIEStart = pucIE = prAssocReqFrame->aucInfoElem;
	}


	/*                                                       */
	prStaRec->u2CapInfo = prAssocReqFrame->u2CapInfo;

#if CFG_ENABLE_WIFI_DIRECT && CFG_ENABLE_HOTSPOT_PRIVACY_CHECK
	if (prAdapter->fgIsP2PRegistered && IS_STA_IN_P2P(prStaRec)) {
		if (((prStaRec->u2CapInfo & CAP_INFO_PRIVACY) &&
		     !kalP2PGetCipher(prAdapter->prGlueInfo))) {
			u2StatusCode = STATUS_CODE_CAP_NOT_SUPPORTED;
			DBGLOG(RSN, TRACE, ("STA Assoc req privacy bit check fail\n"));
			return WLAN_STATUS_SUCCESS;
		}
	}
#endif

	prStaRec->u2ListenInterval = prAssocReqFrame->u2ListenInterval;
	prStaRec->ucPhyTypeSet = 0;

	/*                                   */
	prStaRec->eStaType = STA_TYPE_LEGACY_CLIENT;

	/*                                             */
	IE_FOR_EACH(pucIE, u2IELength, u2Offset) {
		switch (IE_ID(pucIE)) {
		case ELEM_ID_SSID:
			if ((!prIeSsid) &&	/*                            */
			    (IE_LEN(pucIE) <= ELEM_MAX_LEN_SSID)) {
				prIeSsid = (P_IE_SSID_T) pucIE;
			}
			break;

		case ELEM_ID_SUP_RATES:
			if ((!prIeSupportedRate) && (IE_LEN(pucIE) <= RATE_NUM)) {
				prIeSupportedRate = SUP_RATES_IE(pucIE);
			}
			break;

		case ELEM_ID_EXTENDED_SUP_RATES:
			if (!prIeExtSupportedRate)
				prIeExtSupportedRate = EXT_SUP_RATES_IE(pucIE);
			break;
		case ELEM_ID_HT_CAP:
			prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_HT;
			kalMemCopy(&prStaRec->u2HtCapInfo, &(HT_CAP_IE(pucIE)->u2HtCapInfo), 2);
			break;
		case ELEM_ID_RSN:
#if CFG_ENABLE_WIFI_DIRECT && CFG_ENABLE_HOTSPOT_PRIVACY_CHECK
			if (prAdapter->fgIsP2PRegistered && IS_STA_IN_P2P(prStaRec)) {
				prIeRsn = RSN_IE(pucIE);
				rsnParserCheckForRSNCCMPPSK(prAdapter, prIeRsn, &u2StatusCode);
				if (u2StatusCode != STATUS_CODE_SUCCESSFUL) {
					*pu2StatusCode = u2StatusCode;
					return WLAN_STATUS_SUCCESS;
				}
			}
#endif
			break;
		case ELEM_ID_VENDOR:
#if CFG_ENABLE_WIFI_DIRECT
			{
				if ((prAdapter->fgIsP2PRegistered)) {
					UINT_8 ucOuiType = 0;

					p2pFuncParseCheckForP2PInfoElem(prAdapter, pucIE,
									&ucOuiType);

					if (ucOuiType == VENDOR_OUI_TYPE_P2P) {
						DBGLOG(P2P, TRACE,
						       ("Target Client is a P2P group client\n"));
						prStaRec->eStaType = STA_TYPE_P2P_GC;
					}
				}
			}
#endif
			break;
		default:
			for (i = 0; i < (sizeof(rxAssocReqIETable) / sizeof(VERIFY_IE_ENTRY_T));
			     i++) {

				if ((IE_ID(pucIE)) == rxAssocReqIETable[i].ucElemID) {
					rxAssocReqIETable[i].pfnVarifyIE(prAdapter, prSwRfb,
									 (P_IE_HDR_T) pucIE,
									 &u2StatusCode);

					if (u2StatusCode != STATUS_CODE_SUCCESSFUL) {
						*pu2StatusCode = u2StatusCode;
						return WLAN_STATUS_SUCCESS;
					}
				}
			}

			break;
		}
	}			/*                    */

	/*                                                  */
	mqmProcessAssocReq(prAdapter, prSwRfb, pucIEStart, u2IELength);

	do {
		if (prIeSsid) {
			if (UNEQUAL_SSID(prBssInfo->aucSSID, prBssInfo->ucSSIDLen,
					 prIeSsid->aucSSID, prIeSsid->ucLength)) {

				u2StatusCode = STATUS_CODE_UNSPECIFIED_FAILURE;
				break;
			}
		} else {
			u2StatusCode = STATUS_CODE_UNSPECIFIED_FAILURE;
			break;
		}

		prStaRec->u2OperationalRateSet = 0;
		prStaRec->u2BSSBasicRateSet = 0;

		if (prIeSupportedRate || prIeExtSupportedRate) {
			rateGetRateSetFromIEs(prIeSupportedRate, prIeExtSupportedRate, &prStaRec->u2OperationalRateSet, &u2BSSBasicRateSet,	/*                      */
					      &fgIsUnknownBssBasicRate);

			if ((prBssInfo->u2BSSBasicRateSet & prStaRec->u2OperationalRateSet) !=
			    prBssInfo->u2BSSBasicRateSet) {

				u2StatusCode = STATUS_CODE_ASSOC_DENIED_RATE_NOT_SUPPORTED;
				break;
			}

			/*                                                 */

			prStaRec->u2BSSBasicRateSet = prBssInfo->u2BSSBasicRateSet;

			prStaRec->u2DesiredNonHTRateSet =
			    (prStaRec->u2OperationalRateSet & RATE_SET_ALL_ABG);

			if (BAND_2G4 == HIF_RX_HDR_GET_RF_BAND(prSwRfb->prHifRxHdr)) {
#if 0				/*                        */
				/*                      */
				if (!(u2BSSBasicRateSet & RATE_SET_BIT_HT_PHY)) {

					if (prStaRec->u2OperationalRateSet & RATE_SET_OFDM) {
						prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_ERP;
					}


					if (!(u2BSSBasicRateSet & RATE_SET_OFDM)) {
						if (prStaRec->
						    u2OperationalRateSet & RATE_SET_HR_DSSS) {
							prStaRec->ucPhyTypeSet |=
							    PHY_TYPE_BIT_HR_DSSS;
						}

					}

				}
#else
				if (prStaRec->u2OperationalRateSet & RATE_SET_OFDM) {
					prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_ERP;
				}
				if (prStaRec->u2OperationalRateSet & RATE_SET_HR_DSSS) {
					prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_HR_DSSS;
				}
#endif
			} else {	/*                                */
#if 0				/*                        */
				if (!(u2BSSBasicRateSet & RATE_SET_BIT_HT_PHY)) {

					prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_OFDM;
				}
				ASSERT((prStaRec->u2OperationalRateSet & RATE_SET_HR_DSSS) == 0);
#else
				if (prStaRec->u2OperationalRateSet & RATE_SET_OFDM) {
					prStaRec->ucPhyTypeSet |= PHY_TYPE_BIT_OFDM;
				}
#endif
			}

		} else {
			ASSERT(0);
			u2StatusCode = STATUS_CODE_ASSOC_DENIED_RATE_NOT_SUPPORTED;
			break;
		}

#if CFG_ENABLE_WIFI_DIRECT && CFG_ENABLE_HOTSPOT_PRIVACY_CHECK
		if (prAdapter->fgIsP2PRegistered && IS_STA_IN_P2P(prStaRec)) {
			if (prIeRsn) {
				if (!kalP2PGetCipher(prAdapter->prGlueInfo)) {
					u2StatusCode = STATUS_CODE_CIPHER_SUITE_REJECTED;
					break;
				}
			} else {
				prStaRec->rSecInfo.fgAllowOnly1x = FALSE;
				if (kalP2PGetCipher(prAdapter->prGlueInfo)) {
					/*               */
					prStaRec->rSecInfo.fgAllowOnly1x = TRUE;
					break;
				}
			}
		}
#endif

	} while (FALSE);

#if CFG_ENABLE_WIFI_DIRECT
	if (prAdapter->fgIsP2PRegistered && IS_STA_IN_P2P(prStaRec)) {
#if 1				/*     */
		{
			PUINT_8 cp = (PUINT_8) &prAssocReqFrame->u2CapInfo;
			P_UINT_8 prNewAssocReqIe = NULL;

			if (u2IELength) {
				prNewAssocReqIe = kalMemAlloc(u2IELength, VIR_MEM_TYPE);
				if (NULL == prNewAssocReqIe) {
					DBGLOG(AIS, WARN,
					       ("allocate memory for (Re)assocReqIe fail,IELength=%d!\n",
						u2IELength));
					u2StatusCode = STATUS_CODE_INVALID_INFO_ELEMENT;
					return WLAN_STATUS_FAILURE;
					/*                                                                                          
                                                                                                    */
				}
			}
			if (prStaRec->fgIsReAssoc)
				cp += 10;
			else
				cp += 4;
			if (prStaRec->pucAssocReqIe) {
				kalMemFree(prStaRec->pucAssocReqIe, VIR_MEM_TYPE,
					   prStaRec->u2AssocReqIeLen);
				prStaRec->pucAssocReqIe = NULL;
			}
			prStaRec->u2AssocReqIeLen = u2IELength;
			if (u2IELength) {
				prStaRec->pucAssocReqIe = prNewAssocReqIe;
				kalMemCopy(prStaRec->pucAssocReqIe, cp, u2IELength);
			}
		}
#endif
		kalP2PUpdateAssocInfo(prAdapter->prGlueInfo, (PUINT_8) &prAssocReqFrame->u2CapInfo,
				      u2IELength + (prStaRec->fgIsReAssoc ? 10 : 4),
				      prStaRec->fgIsReAssoc);
	}
#endif

	*pu2StatusCode = u2StatusCode;

	return WLAN_STATUS_SUCCESS;

}				/*                                      */


/*                                                                            */
/* 
                                                                                     
                        
 
                                                                
                                                      
 
                
*/
/*                                                                            */
__KAL_INLINE__ VOID
assocBuildReAssocRespFrameCommonIEs(IN P_ADAPTER_T prAdapter,
				    IN P_MSDU_INFO_T prMsduInfo, IN P_BSS_INFO_T prBssInfo)
{
	PUINT_8 pucBuffer;
	P_STA_RECORD_T prStaRec;
	UINT_8 ucSupRatesLen;
	UINT_8 ucExtSupRatesLen;


	ASSERT(prMsduInfo);
	ASSERT(prMsduInfo->eSrc == TX_PACKET_MGMT);

	prStaRec = cnmGetStaRecByIndex(prAdapter, prMsduInfo->ucStaRecIndex);
	ASSERT(prStaRec);


	pucBuffer = (PUINT_8) ((UINT_32) prMsduInfo->prPacket +
			       (UINT_32) prMsduInfo->u2FrameLength);
	ASSERT(pucBuffer);

	if (prBssInfo->ucAllSupportedRatesLen > ELEM_MAX_LEN_SUP_RATES) {

		ucSupRatesLen = ELEM_MAX_LEN_SUP_RATES;
		ucExtSupRatesLen = prBssInfo->ucAllSupportedRatesLen - ELEM_MAX_LEN_SUP_RATES;
	} else {
		ucSupRatesLen = prBssInfo->ucAllSupportedRatesLen;
		ucExtSupRatesLen = 0;
	}

	/*                                   */
	if (ucSupRatesLen) {
		SUP_RATES_IE(pucBuffer)->ucId = ELEM_ID_SUP_RATES;
		SUP_RATES_IE(pucBuffer)->ucLength = ucSupRatesLen;
		kalMemCopy(SUP_RATES_IE(pucBuffer)->aucSupportedRates,
			   prBssInfo->aucAllSupportedRates, ucSupRatesLen);

		prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
		pucBuffer += IE_SIZE(pucBuffer);
	}


	/*                                            */
	if (ucExtSupRatesLen) {

		EXT_SUP_RATES_IE(pucBuffer)->ucId = ELEM_ID_EXTENDED_SUP_RATES;
		EXT_SUP_RATES_IE(pucBuffer)->ucLength = ucExtSupRatesLen;

		kalMemCopy(EXT_SUP_RATES_IE(pucBuffer)->aucExtSupportedRates,
			   &prBssInfo->aucAllSupportedRates[ucSupRatesLen], ucExtSupRatesLen);

		prMsduInfo->u2FrameLength += IE_SIZE(pucBuffer);
	}

	return;
}				/*                                              */


/*                                                                            */
/* 
                                                                      
 
                                                               
                                                                
                                                
                                                                    
                                                                                  
 
                
*/
/*                                                                            */
__KAL_INLINE__ VOID
assocComposeReAssocRespFrameHeaderAndFF(IN P_STA_RECORD_T prStaRec,
					IN PUINT_8 pucBuffer,
					IN UINT_8 aucBSSID[],
					IN UINT_16 u2CapInfo, IN OUT PUINT_16 pu2PayloadLen)
{
	P_WLAN_ASSOC_RSP_FRAME_T prAssocRspFrame;
	BOOLEAN fgIsReAssoc;

	UINT_16 u2FrameCtrl;


	ASSERT(prStaRec);
	ASSERT(pucBuffer);
	ASSERT(aucBSSID);
	ASSERT(pu2PayloadLen);

	prAssocRspFrame = (P_WLAN_ASSOC_RSP_FRAME_T) pucBuffer;
	fgIsReAssoc = prStaRec->fgIsReAssoc;

	/*                                                                       */
	/*                               */
	if (fgIsReAssoc) {
		u2FrameCtrl = MAC_FRAME_REASSOC_RSP;
	} else {
		u2FrameCtrl = MAC_FRAME_ASSOC_RSP;
	}
	/*                                                             */
	prAssocRspFrame->u2FrameCtrl = u2FrameCtrl;	/*                                */

	/*                                            */
	COPY_MAC_ADDR(prAssocRspFrame->aucDestAddr, prStaRec->aucMacAddr);

	/*                                       */
	COPY_MAC_ADDR(prAssocRspFrame->aucSrcAddr, aucBSSID);

	/*                                          */
	COPY_MAC_ADDR(prAssocRspFrame->aucBSSID, aucBSSID);

	/*                                                                                    */
	prAssocRspFrame->u2SeqCtrl = 0;


	/*                                                                                               */
	/*                                        */
	/*                                                         */
	prAssocRspFrame->u2CapInfo = u2CapInfo;	/*                                */

	/*                                                                         */
	prAssocRspFrame->u2StatusCode = prStaRec->u2StatusCode;	/*                                */

	/*                                                                                            */
	prAssocRspFrame->u2AssocId = ((prStaRec->u2AssocId & AID_MASK) | AID_MSB);	/*                                */

	*pu2PayloadLen = (CAP_INFO_FIELD_LEN + STATUS_CODE_FIELD_LEN + AID_FIELD_LEN);

	return;
}				/*                                                  */


/*                                                                            */
/* 
                                                               
 
                                                           
 
                                                                          
                                                                    
*/
/*                                                                            */
WLAN_STATUS assocSendReAssocRespFrame(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec)
{
	P_BSS_INFO_T prBssInfo;
	P_MSDU_INFO_T prMsduInfo;

	UINT_16 u2PayloadLen;
	UINT_16 u2EstimatedFrameLen;
	UINT_16 u2EstimatedExtraIELen;
	BOOLEAN fgIsReAssoc;
	UINT_32 i;


	ASSERT(prStaRec);


	/*                                                      */
	fgIsReAssoc = prStaRec->fgIsReAssoc;

	/*                                                                          */
	u2EstimatedFrameLen = MAC_TX_RESERVED_FIELD +
	    WLAN_MAC_MGMT_HEADER_LEN +
	    CAP_INFO_FIELD_LEN +
	    STATUS_CODE_FIELD_LEN +
	    AID_FIELD_LEN +
	    (ELEM_HDR_LEN + ELEM_MAX_LEN_SUP_RATES) +
	    (ELEM_HDR_LEN + (RATE_NUM - ELEM_MAX_LEN_SUP_RATES));

	/*                   */
	u2EstimatedExtraIELen = 0;

	for (i = 0; i < sizeof(txAssocRespIETable) / sizeof(APPEND_VAR_IE_ENTRY_T); i++) {
		if (txAssocRespIETable[i].u2EstimatedFixedIELen != 0) {
			u2EstimatedExtraIELen += txAssocRespIETable[i].u2EstimatedFixedIELen;
		} else if (txAssocRespIETable[i].pfnCalculateVariableIELen != NULL) {
			u2EstimatedExtraIELen +=
			    (UINT_16) txAssocRespIETable[i].pfnCalculateVariableIELen(prAdapter,
										      prStaRec->
										      ucNetTypeIndex,
										      prStaRec);
		}

	}

	u2EstimatedFrameLen += u2EstimatedExtraIELen;

	/*                        */
	if ((prMsduInfo = cnmMgtPktAlloc(prAdapter, u2EstimatedFrameLen)) == NULL) {
		DBGLOG(AAA, WARN, ("No PKT_INFO_T for sending (Re)Assoc Response.\n"));
		return WLAN_STATUS_RESOURCES;
	}
	/*                                                                                     */
	ASSERT(prStaRec->ucNetTypeIndex != NETWORK_TYPE_AIS_INDEX);
	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);

	/*                                */
	assocComposeReAssocRespFrameHeaderAndFF(prStaRec,
						(PUINT_8) ((UINT_32) (prMsduInfo->prPacket) +
							   MAC_TX_RESERVED_FIELD),
						prBssInfo->aucBSSID, prBssInfo->u2CapInfo,
						&u2PayloadLen);

	/*                                         */
	ASSERT(prStaRec->ucNetTypeIndex < NETWORK_TYPE_INDEX_NUM);

	prMsduInfo->eSrc = TX_PACKET_MGMT;
	prMsduInfo->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
	prMsduInfo->ucStaRecIndex = prStaRec->ucIndex;
	prMsduInfo->ucNetworkType = prStaRec->ucNetTypeIndex;
	prMsduInfo->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
	prMsduInfo->fgIs802_1x = FALSE;
	prMsduInfo->fgIs802_11 = TRUE;
	prMsduInfo->u2FrameLength = WLAN_MAC_MGMT_HEADER_LEN + u2PayloadLen;
	prMsduInfo->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
	prMsduInfo->pfTxDoneHandler = aaaFsmRunEventTxDone;
	prMsduInfo->fgIsBasicRate = TRUE;

	/*                                                                           */
	assocBuildReAssocRespFrameCommonIEs(prAdapter, prMsduInfo, prBssInfo);


	/*                                  */

	/*           */
	for (i = 0; i < sizeof(txAssocRespIETable) / sizeof(APPEND_VAR_IE_ENTRY_T); i++) {
		if (txAssocRespIETable[i].pfnAppendIE) {
			txAssocRespIETable[i].pfnAppendIE(prAdapter, prMsduInfo);
		}
	}

	/*                                                                      */

	/*                                                                     */
	nicTxEnqueueMsdu(prAdapter, prMsduInfo);

	return WLAN_STATUS_SUCCESS;

}				/*                                    */
#endif				/*                 */
