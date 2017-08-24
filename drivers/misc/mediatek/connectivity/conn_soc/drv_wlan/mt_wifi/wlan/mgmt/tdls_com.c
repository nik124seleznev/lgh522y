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
 
#if (CFG_SUPPORT_TDLS == 1)
#include "tdls.h"
 
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
UINT_32
TdlsFrameGeneralIeAppend(
	ADAPTER_T							*prAdapter,
	STA_RECORD_T						*prStaRec,
	UINT_16								u2StatusCode,
	UINT_8								*pPkt
	)
{
	GLUE_INFO_T *prGlueInfo;
    BSS_INFO_T *prBssInfo;
	PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
	UINT_32 u4NonHTPhyType;
    UINT_16 u2SupportedRateSet;
    UINT_8 aucAllSupportedRates[RATE_NUM] = {0};
    UINT_8 ucAllSupportedRatesLen;
    UINT_8 ucSupRatesLen;
    UINT_8 ucExtSupRatesLen;
	UINT_32 u4PktLen, u4IeLen;
	BOOLEAN fg40mAllowed;


	/*                                                   */

	/*      */
	prGlueInfo = (GLUE_INFO_T *)prAdapter->prGlueInfo;
	prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
    prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
	u4PktLen = 0;

	/*                                                  */
	/*                                 */
	if (prStaRec != NULL)
		u4NonHTPhyType = prStaRec->ucNonHTBasicPhyType;
	else
		u4NonHTPhyType = PHY_TYPE_ERP_INDEX; /*         */

	u2SupportedRateSet = rNonHTPhyAttributes[u4NonHTPhyType].u2SupportedRateSet;

	if (prStaRec != NULL)
	{
		u2SupportedRateSet &= prStaRec->u2OperationalRateSet;

		if (u2SupportedRateSet == 0)
			u2SupportedRateSet = rNonHTPhyAttributes[u4NonHTPhyType].u2SupportedRateSet;
	}

	rateGetDataRatesFromRateSet(u2SupportedRateSet,
								prBssInfo->u2BSSBasicRateSet,
								aucAllSupportedRates,
								&ucAllSupportedRatesLen);
	
	ucSupRatesLen = ((ucAllSupportedRatesLen > ELEM_MAX_LEN_SUP_RATES) ?
					 ELEM_MAX_LEN_SUP_RATES : ucAllSupportedRatesLen);
	
	ucExtSupRatesLen = ucAllSupportedRatesLen - ucSupRatesLen;
		
	if (ucSupRatesLen) {
		SUP_RATES_IE(pPkt)->ucId = ELEM_ID_SUP_RATES;
		SUP_RATES_IE(pPkt)->ucLength = ucSupRatesLen;
		kalMemCopy(SUP_RATES_IE(pPkt)->aucSupportedRates,
				   aucAllSupportedRates,
				   ucSupRatesLen);
	
		u4IeLen = IE_SIZE(pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}

	/*                                                     */
	if (ucExtSupRatesLen) {
	
		EXT_SUP_RATES_IE(pPkt)->ucId = ELEM_ID_EXTENDED_SUP_RATES;
		EXT_SUP_RATES_IE(pPkt)->ucLength = ucExtSupRatesLen;
	
		kalMemCopy(EXT_SUP_RATES_IE(pPkt)->aucExtSupportedRates,
				   &aucAllSupportedRates[ucSupRatesLen],
				   ucExtSupRatesLen);
	
		u4IeLen = IE_SIZE(pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}

	/*                                                     */
	/*
                                                                                   
                                       
 */
	if (u2StatusCode == 0)
	{
		SUPPORTED_CHANNELS_IE(pPkt)->ucId = ELEM_ID_SUP_CHS;
		SUPPORTED_CHANNELS_IE(pPkt)->ucLength = 2;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[0] = 1;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[1] = 11;

#if CFG_SUPPORT_DFS
		if(prAdapter->fgEnable5GBand == TRUE) {
			/*            */
			SUPPORTED_CHANNELS_IE(pPkt)->ucLength = 10;
			SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[2] = 36;
			SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[3] = 4;
			SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[4] = 52;
			SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[5] = 4;
			SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[6] = 149;
			SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[7] = 4;
			SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[8] = 165;
			SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[9] = 4;
		}
#endif /*                 */

		u4IeLen = IE_SIZE(pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}

	/*                                                   */

	/*                                */
//                                                                          

	/*
                                                                             
                                                                    

                                                

                                      
 */
	if (!prStaRec || (prStaRec->ucPhyTypeSet & PHY_TYPE_SET_802_11N))
	{
		/*                                                           */
#if 0 /*                */
		if (prAdapter->rWifiVar.rConnSettings.uc2G4BandwidthMode == CONFIG_BW_20M)
			fg40mAllowed = FALSE;
		else
#endif
			fg40mAllowed = TRUE;

		u4IeLen = rlmFillHtCapIEByParams(\
							fg40mAllowed,
							prAdapter->rWifiVar.rConnSettings.fgRxShortGIDisabled,
							prAdapter->rWifiVar.u8SupportRxSgi20,
							prAdapter->rWifiVar.u8SupportRxSgi40,
							prAdapter->rWifiVar.u8SupportRxGf,
							prBssInfo->eCurrentOPMode,
							pPkt);

		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}

	/*                                                   */

	/*                */
//                                      

	{
		/*                                         */
		u4IeLen = mqmGenerateWmmInfoIEByParam(\
							TRUE /*                                  */,
							0xf /*                                  */,
							0xf /*                                 */,
							WMM_MAX_SP_LENGTH_ALL /*                            */,
							pPkt);

		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}

	return u4PktLen;
}


/*                                                                            */
/* 
                                                                                                    
 
                                                         
                                                             
                                                          
                                     
                                       
                                     
                                                                          
                                                
 
                        
*/
/*                                                                            */
TDLS_STATUS
TdlsDataFrameSend(
	ADAPTER_T							*prAdapter,
	STA_RECORD_T						*prStaRec,
	UINT_8								*pPeerMac,
	UINT_8								ucActionCode,
	UINT_8								ucDialogToken,
	UINT_16								u2StatusCode,
	UINT_8								*pAppendIe,
	UINT_32								AppendIeLen
	)
{
#define LR_TDLS_FME_FIELD_FILL(__Len) \
	pPkt += __Len; \
	u4PktLen += __Len;
	
	GLUE_INFO_T *prGlueInfo;
    BSS_INFO_T *prBssInfo;
	PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
	struct sk_buff *prMsduInfo;
	MSDU_INFO_T *prMsduInfoMgmt;
	UINT8 *pPkt, *pucInitiator, *pucResponder;
	UINT32 u4PktLen, u4IeLen;
	UINT16 u2CapInfo;
//                    


	prGlueInfo = (GLUE_INFO_T *)prAdapter->prGlueInfo;

	DBGLOG(TDLS, INFO, ("<tdls_fme> %s: 2040=%d\n",
		__FUNCTION__, prGlueInfo->rTdlsLink.fgIs2040Sup));

	/*              */
	if (prStaRec != NULL)
	{
		if (prStaRec->ucNetTypeIndex >= NETWORK_TYPE_INDEX_NUM)
		{
			DBGLOG(TDLS, ERROR,
				("<tdls_cmd> %s: net index %d fail\n",
				__FUNCTION__, prStaRec->ucNetTypeIndex));
			return TDLS_STATUS_FAILURE;
		}

		prBssInfo = &(prAdapter->rWifiVar.arBssInfo[prStaRec->ucNetTypeIndex]);
	}
	else
	{
		/*                                      */
		prBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);
	}

	/*                      */
    prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
	u4PktLen = 0;
	prMsduInfo = NULL;
	prMsduInfoMgmt = NULL;

	/*                       */
	if (ucActionCode != TDLS_FRM_ACTION_DISCOVERY_RESPONSE)
	{
		/*
                                                                                      
                                                                      
  */

		/*                                   */
		prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
		if (prMsduInfo == NULL)
		{
			DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: allocate pkt fail\n",
				__FUNCTION__));
			return TDLS_STATUS_RESOURCES;
		}
		
		prMsduInfo->dev = prGlueInfo->prDevHandler;
		if (prMsduInfo->dev == NULL)
		{
			DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: MsduInfo->dev == NULL\n",
				__FUNCTION__));
			kalPacketFree(prGlueInfo, prMsduInfo);
			return TDLS_STATUS_FAILURE;
		}

		/*                 */
//                        
		kalMemCopy(pPkt, pPeerMac, TDLS_FME_MAC_ADDR_LEN);
		LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
		kalMemCopy(pPkt, prBssInfo->aucOwnMacAddr, TDLS_FME_MAC_ADDR_LEN);
		LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
		*(UINT_16 *)pPkt = htons(TDLS_FRM_PROT_TYPE);
		LR_TDLS_FME_FIELD_FILL(2);

		/*                 */
		*pPkt = TDLS_FRM_PAYLOAD_TYPE;
		LR_TDLS_FME_FIELD_FILL(1);

		/*                                   */
		*pPkt = TDLS_FRM_CATEGORY;
		LR_TDLS_FME_FIELD_FILL(1);
	}
	else
	{
		/*                    */
		WLAN_MAC_HEADER_T *prHdr;

		prMsduInfoMgmt = (MSDU_INFO_T *)\
			cnmMgtPktAlloc(prAdapter, PUBLIC_ACTION_MAX_LEN);
		if (prMsduInfoMgmt == NULL)
		{
			DBGLOG(TDLS, ERROR, ("<tdls_cmd> %s: allocate mgmt pkt fail\n",
				__FUNCTION__));
			return TDLS_STATUS_RESOURCES;
		}

		pPkt = (UINT8 *)prMsduInfoMgmt->prPacket;
		prHdr = (WLAN_MAC_HEADER_T *)pPkt;

		/*                  */
		prHdr->u2FrameCtrl = MAC_FRAME_ACTION;
		prHdr->u2DurationID = 0;
		kalMemCopy(prHdr->aucAddr1, pPeerMac, TDLS_FME_MAC_ADDR_LEN);
		kalMemCopy(prHdr->aucAddr2, prBssInfo->aucOwnMacAddr, TDLS_FME_MAC_ADDR_LEN);
		kalMemCopy(prHdr->aucAddr3, prBssInfo->aucBSSID, TDLS_FME_MAC_ADDR_LEN);
		prHdr->u2SeqCtrl = 0;
		LR_TDLS_FME_FIELD_FILL(sizeof(WLAN_MAC_HEADER_T));

		/*                                */
		*pPkt = CATEGORY_PUBLIC_ACTION;
		LR_TDLS_FME_FIELD_FILL(1);
	}

	/*                                 */
	*pPkt = ucActionCode;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                  */
	switch(ucActionCode)
	{
		case TDLS_FRM_ACTION_SETUP_RSP:
		case TDLS_FRM_ACTION_CONFIRM:
		case TDLS_FRM_ACTION_TEARDOWN:
			WLAN_SET_FIELD_16(pPkt, u2StatusCode);
			LR_TDLS_FME_FIELD_FILL(2);
			break;
	}

	/*                                       */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN)
	{
		*pPkt = ucDialogToken;
		LR_TDLS_FME_FIELD_FILL(1);
	}

	/*               */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN)
	{
		/*
             
  
                
                         
                     
                  
                          

                        
                  
  
        
       
                   
  */
		if (ucActionCode != TDLS_FRM_ACTION_CONFIRM)
		{
			/*                                                                         */
			u2CapInfo = assocBuildCapabilityInfo(prAdapter, prStaRec);
			WLAN_SET_FIELD_16(pPkt, u2CapInfo);
			LR_TDLS_FME_FIELD_FILL(2);

			/*                       */
			/*
                                                                       
                             

                                                              
                          
   */
			u4IeLen = TdlsFrameGeneralIeAppend(prAdapter, prStaRec,	u2StatusCode, pPkt);
			LR_TDLS_FME_FIELD_FILL(u4IeLen);

			/*                                                    */
			EXT_CAP_IE(pPkt)->ucId = ELEM_ID_EXTENDED_CAP;
			EXT_CAP_IE(pPkt)->ucLength = 5;

			EXT_CAP_IE(pPkt)->aucCapabilities[0] = 0x00; /*             */
			EXT_CAP_IE(pPkt)->aucCapabilities[1] = 0x00; /*              */
			EXT_CAP_IE(pPkt)->aucCapabilities[2] = 0x00; /*               */
			EXT_CAP_IE(pPkt)->aucCapabilities[3] = 0x00; /*               */
			EXT_CAP_IE(pPkt)->aucCapabilities[4] = 0x00; /*               */

		//                                             
				EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((28-24));
		//                                              
				EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((30-24));
		//                                       
				EXT_CAP_IE(pPkt)->aucCapabilities[4] |= BIT((37-32));

			u4IeLen = IE_SIZE(pPkt);
			LR_TDLS_FME_FIELD_FILL(u4IeLen);
		}
		else
		{
			/*                                            */
			if (prAdapter->rWifiVar.fgSupportQoS)
			{
				u4IeLen = mqmGenerateWmmParamIEByParam(\
									prAdapter, prBssInfo, pPkt);

				LR_TDLS_FME_FIELD_FILL(u4IeLen);
			}
		}
	}

	/*                                            */
	/*
                                                                       
 */
//                                                                     
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN)
	{
		/*
                                                                     
                                                                       
                                                                
             
 
                                                                         
                                                                     
                             
 
                                                                       
                                                            
  */
		BSS_20_40_COEXIST_IE(pPkt)->ucId = ELEM_ID_20_40_BSS_COEXISTENCE;
		BSS_20_40_COEXIST_IE(pPkt)->ucLength = 1;
		BSS_20_40_COEXIST_IE(pPkt)->ucData = 0x01;
		LR_TDLS_FME_FIELD_FILL(3);
	}

	/*                                           */
//                                              
//                                 

	/*                                              */
	/*                                                                              */
	/*
                                                                                       
                                                        
 */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = ELEM_LEN_LINK_IDENTIFIER;

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);

	switch(ucActionCode)
	{
		case TDLS_FRM_ACTION_SETUP_REQ:
		case TDLS_FRM_ACTION_CONFIRM:
		default:
			/*                  */
			pucInitiator = prBssInfo->aucOwnMacAddr;
			pucResponder = pPeerMac;

			if (prStaRec != NULL)
				prStaRec->flgTdlsIsInitiator = TRUE;
			break;

		case TDLS_FRM_ACTION_SETUP_RSP:
		case TDLS_FRM_ACTION_DISCOVERY_RESPONSE:
			/*                   */
			pucInitiator = pPeerMac;
			pucResponder = prBssInfo->aucOwnMacAddr;

			if (prStaRec != NULL)
				prStaRec->flgTdlsIsInitiator = FALSE;
			break;

		case TDLS_FRM_ACTION_TEARDOWN:
			if (prStaRec != NULL)
			{
				if (prStaRec->flgTdlsIsInitiator == TRUE)
				{
					/*                  */
					pucInitiator = prBssInfo->aucOwnMacAddr;
					pucResponder = pPeerMac;
				}
				else
				{
					/*                   */
					pucInitiator = pPeerMac;
					pucResponder = prBssInfo->aucOwnMacAddr;
				}
			}
			else
			{
				/*                   */
				pucInitiator = pPeerMac;
				pucResponder = prBssInfo->aucOwnMacAddr;
			}
			break;
	}

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, pucInitiator, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, pucResponder, 6);

	u4IeLen = IE_SIZE(pPkt);
	LR_TDLS_FME_FIELD_FILL(u4IeLen);

	/*                        */
	/*
                                   
                                                                                    
                                      

                                                                                   
 */
//                                                                       
	if (pAppendIe != NULL)
	{
		if ((ucActionCode != TDLS_FRM_ACTION_TEARDOWN) ||
			((ucActionCode == TDLS_FRM_ACTION_TEARDOWN) &&
			(prStaRec != NULL) &&
			(prStaRec->fgTdlsInSecurityMode == TRUE)))
		{
			kalMemCopy(pPkt, pAppendIe, AppendIeLen);
			LR_TDLS_FME_FIELD_FILL(AppendIeLen);
		}
	}

    /*                                          */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN)
	{
	    /*                                                                                */
		u4IeLen = rlmDomainSupOperatingClassIeFill(pPkt);
		LR_TDLS_FME_FIELD_FILL(u4IeLen);
	}

	/*                                       */
	if (ucActionCode != TDLS_FRM_ACTION_DISCOVERY_RESPONSE)
	{
#if 0
		/*
                                                                   
                                                               
  */

#if 1
        /*                                                                           */
        if (ucActionCode == TDLS_FRM_ACTION_SETUP_REQ)
        {
#if 0
			/*          */
            char buffer[] = { 0x31, 0x04,
            0x01, 0x08, 0x02, 0x04, 0x0b, 0x16, 0xc, 0x12, 0x18, 0x24,
            0x32, 0x04, 0x30, 0x48, 0x60, 0x6c,
			0x24, 0x0a, 0x01, 0x0b, 0x24, 0x04, 0x34, 0x04, 0x95, 0x04, 0xa5, 0x01,
			0x2d, 0x1a, 0x72, 0x11, 0x03, 0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00,
			0xdd, 0x07, 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x0f,
			0x7f, 0x05, 0x00, 0x00, 0x00, 0x50, 0x20,
			0x48, 0x01, 0x01,
			0x65, 0x12, 0x00, 0x0c, 0x43, 0x31, 0x35, 0x97, 0x00, 0x11, 0x22, 0x33,
				0x44, 0x05, 0x00, 0x22, 0x58, 0x00, 0xcc, 0x0f,
			0x3b, 0x0d, 0x0c, 0x01, 0x02, 0x03, 0x05, 0x16, 0x17, 0x19,
				0x1b, 0x1c, 0x1e, 0x20, 0x21,
			0x07, 0x06, 0x55, 0x53, 0x20, 0x01, 0x0b, 0x1e
			};
#endif

#if 1
			/*        */ /*                 */
            char buffer[] = { 0x21, 0x04,
            0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x12, 0x24, 0x48, 0x6c,
            0x07, 0x06, 0x55, 0x53, 0x20, 0xdd, 0x20, 0x00,
            0x32, 0x04, 0x0c, 0x18, 0x30, 0x60,
            0x24, 0x06, 0x01, 0x0b, 0x24, 0x08, 0x95, 0x04,
            0x7f, 0x05, 0x01, 0x00, 0x00, 0x50, 0x20,
            0x3b, 0x10, 0x20, 0x01, 0x02, 0x03, 0x04, 0x0c, 0x16, 0x17, 0x18, 0x19,
                0x1b, 0x1c, 0x1d, 0x1e, 0x20, 0x21,
            0x2d, 0x1a, 0x6e, 0x00, 0x17, 0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x48, 0x01, 0x01,
            0x65, 0x12, 0x00, 0x0c, 0x43, 0x44, 0x0b, 0x1a, 0x00, 0x11, 0x22, 0x33,
                0x44, 0x05, 0x00, 0x22, 0x58, 0x00, 0xcc, 0x0f,
            0xdd, 0x07, 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x0f };
#endif
#if 0
			/*      */
            char buffer[] = { 0x01, 0x01,
            0x01, 0x04, 0x02, 0x04, 0x0b, 0x16,
			0x24, 0x02, 0x01, 0x0d,
			0x7f, 0x05, 0x00, 0x00, 0x00, 0x50, 0xff,
			0x2d, 0x1a, 0x61, 0x01, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
			0x38, 0x05, 0x02, 0xc0, 0xa8, 0x00, 0x00,
            0x48, 0x01, 0x01,
			0x3b, 0x0d, 0x0c, 0x01, 0x02, 0x03, 0x05, 0x16, 0x17, 0x19,
				0x1b, 0x1c, 0x1d, 0x1e, 0x20, 0x21,
			0x07, 0x06, 0x55, 0x53, 0x20, 0x01, 0x0b, 0x1e,
            0x65, 0x12, 0x00, 0x0c, 0x43, 0x44, 0x0b, 0x1a, 0x00, 0x11, 0x22, 0x33,
				0x44, 0x05, 0x00, 0x22, 0x58, 0x00, 0xcc, 0x0f,
			0xdd, 0x18, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01, 0x80, 0x01, 0x00, 0x00,
				0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60,
				0x00, 0x00, 0x00,
			0xbf, 0x0c, 0x30, 0x01, 0x80, 0x03, 0xfe, 0xff, 0x00, 0x00, 0xfe, 0xff,
				0x00, 0x00
			};
#endif

            pPktTemp += 18;
            memcpy(pPktTemp, buffer, sizeof(buffer));
            u4PktLen = 18 + sizeof(buffer);
        }
#endif

#if 1
		if (ucActionCode == TDLS_FRM_ACTION_CONFIRM)
		{
			/*                                            */
#if 1
			/*        */
            char buffer[] = { 0x00,
            0x01, 0x2d, 0x1a, 0x6e, 0x00, 0x17, 0xff, 0x00, 0x00, 0x00,
            	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x65, 0x12, 0x00, 0x0c, 0x43, 0x44, 0x0b, 0x1a, 0x00, 0x11, 0x22, 0x33,
                0x44, 0x05, 0x00, 0x22, 0x58, 0x00, 0xcc, 0x0f,
			0xdd, 0x18, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01, 0x0f, 0x00, 0x03,
				0xa4, 0x00, 0x00, 0x27, 0xa4, 0x00, 0x00, 0x42, 0x43, 0x5e, 0x00,
				0x62, 0x32, 0x2f, 0x00
			};
#endif

#if 0
			/*      */
			char buffer[] = { 0x00,
			0x01,
			0x38, 0x05, 0x02, 0xc0, 0xa8, 0x00, 0x00,
			0x48, 0x01, 0x01,
			0x3b, 0x0d, 0x0c, 0x01, 0x02, 0x03, 0x05, 0x16, 0x17, 0x19,
				0x1b, 0x1c, 0x1d, 0x1e, 0x20, 0x21,
			0x07, 0x06, 0x55, 0x53, 0x20, 0x01, 0x0b, 0x1e,
			0x65, 0x12, 0x00, 0x0c, 0x43, 0x44, 0x0b, 0x1a, 0x00, 0x11, 0x22, 0x33,
				0x44, 0x05, 0x00, 0x22, 0x58, 0x00, 0xcc, 0x0f,
			0xdd, 0x07, 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x00,
			0xdd, 0x18, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01, 0x80, 0x3f, 0x00, 0x00,
				0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60,
				0x00, 0x00, 0x00
			};
#endif

#if 0
			/*         */
			char buffer[] = { 0x00,
			0x01,
			0xdd, 0x18, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x01, 0x0f, 0x6b, 0x00, 0x00,
				0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60,
				0x00, 0x00, 0x00
			0x65, 0x12, 0x00, 0x0c, 0x43, 0x31, 0x35, 0x97, 0x00, 0x11, 0x22, 0x33,
				0x44, 0x05, 0x00, 0x22, 0x58, 0x00, 0xcc, 0x0f,
			0x38, 0x0d, 0x0c, 0x01, 0x02, 0x03, 0x05, 0x16, 0x17, 0x19, 0x1b,
				0x1c, 0x1e, 0x20, 0x21,
			0x07, 0x06, 0x55, 0x53, 0x20, 0x01, 0x0b, 0x1e
			};
#endif

            pPktTemp += 18;
            memcpy(pPktTemp, buffer, sizeof(buffer));
            u4PktLen = 18 + sizeof(buffer);
        }
#endif

#else

#if 0
        /*                       */
        if (ucActionCode == TDLS_FRM_ACTION_SETUP_REQ)
        {
            char buffer[] = { 0x01, 0x04,
			0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x12, 0x24, 0x48, 0x6c,
			0x07, 0x06, 0x55, 0x53, 0x20, 0xdd, 0x20, 0x00,
            0x32, 0x04, 0x30, 0x48, 0x60, 0x6c,
			0x24, 0x0a, 0x01, 0x0b, 0x24, 0x04, 0x34, 0x04, 0x95, 0x04, 0xa5, 0x01,
			0x2d, 0x1a, 0x72, 0x11, 0x03, 0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
			0xdd, 0x07, 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x0f,
			0x7f, 0x05, 0x00, 0x00, 0x00, 0x50, 0x20,
            0x48, 0x01, 0x01,
            0x65, 0x12, 0x00, 0x0c, 0x43, 0x44, 0x0b, 0x1a, 0x00, 0x11, 0x22, 0x33,
				0x44, 0x05, 0x00, 0x22, 0x58, 0x00, 0xcc, 0x0f,
			0x3b, 0x0d, 0x0c, 0x01, 0x02, 0x03, 0x05, 0x16, 0x17, 0x19,
				0x1b, 0x1c, 0x1e, 0x20, 0x21
			};

            pPktTemp += 18;
            memcpy(pPktTemp, buffer, sizeof(buffer));
            u4PktLen = 18 + sizeof(buffer);
        }
#endif
#endif /*   */

		/*                         */
		prMsduInfo->len = u4PktLen;
		dumpMemory8(ANDROID_LOG_INFO, prMsduInfo->data, u4PktLen);

		wlanHardStartXmit(prMsduInfo, prMsduInfo->dev);
	}
	else
	{
		/*
                                                                                  
                                                                  
                                                                                 
                                                                                 
                                                                         
                                                                       
  */
		prMsduInfoMgmt->ucPacketType = HIF_TX_PACKET_TYPE_MGMT;
		prMsduInfoMgmt->ucStaRecIndex = prBssInfo->prStaRecOfAP->ucIndex;
		prMsduInfoMgmt->ucNetworkType = prBssInfo->ucNetTypeIndex;
		prMsduInfoMgmt->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
		prMsduInfoMgmt->fgIs802_1x = FALSE;
		prMsduInfoMgmt->fgIs802_11 = TRUE;
		prMsduInfoMgmt->u2FrameLength = u4PktLen;
		prMsduInfoMgmt->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
		prMsduInfoMgmt->pfTxDoneHandler = NULL;
		prMsduInfoMgmt->fgIsBasicRate = TRUE; /*                */

		/*                       */
		nicTxEnqueueMsdu(prAdapter, prMsduInfoMgmt);
	}

	return TDLS_STATUS_SUCCESS;
}


#endif /*                  */
 
 /*                   */

