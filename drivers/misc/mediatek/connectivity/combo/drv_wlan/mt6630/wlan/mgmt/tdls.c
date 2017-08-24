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

#if CFG_SUPPORT_TDLS
#include "tdls.h"
#include "gl_cfg80211.h"
#include "queue.h"


/*                                                                              
                        
                                                                                
*/
	/*                               */
/*                               */

/*                                                                              
                                                
                                                                                
*/


/*                                                                              
                              
                                                                                
*/

static BOOLEAN fgIsPtiTimeoutSkip = FALSE;


/*                                                                              
                                       
                                                                                
*/


#define ELEM_ID_LINK_IDENTIFIER_LENGTH 16
	
#define 	TDLS_KEY_TIMEOUT_INTERVAL 43200

#define    UNREACH_ABLE 25
#define TDLS_REASON_CODE_UNREACHABLE  25
#define TDLS_REASON_CODE_UNSPECIFIED  26

#define WLAN_REASON_TDLS_TEARDOWN_UNREACHABLE 25
#define WLAN_REASON_TDLS_TEARDOWN_UNSPECIFIED 26

UINT_8 g_arTdlsLink[MAXNUM_TDLS_PEER] = {
    0,
    0,
    0,
    0
};


/*                                                                            */
/* 
                                                                     
 
                                                        
             
             
                                                         
 
                             
                                    
*/
/*                                                                            */
UINT_32
TdlsexLinkMgt(P_ADAPTER_T prAdapter,
	      PVOID pvSetBuffer, UINT_32 u4SetBufferLen, PUINT_32 pu4SetInfoLen)
{
	/*                                                      */
	
	STA_RECORD_T *prStaRec;
	P_BSS_INFO_T  prBssInfo;
	TDLS_CMD_LINK_MGT_T *prCmd;
	
	prCmd = (TDLS_CMD_LINK_MGT_T *) pvSetBuffer;
	prBssInfo = prAdapter->prAisBssInfo;


	/*                                        */

#if 1
	/*          */
	if (prBssInfo->eCurrentOPMode == OP_MODE_INFRASTRUCTURE) {
		prStaRec = prBssInfo->prStaRecOfAP;
		if (prStaRec == NULL)
			return 0;
	} else {
		return -EINVAL;
	}
#endif

	prStaRec = prBssInfo->prStaRecOfAP;

	switch (prCmd->ucActionCode) {
		
		case TDLS_FRM_ACTION_DISCOVERY_REQ:
		/*                                                        */
		if (prStaRec == NULL) {
				return 0;
			}
		if (TdlsDataFrameSend_DISCOVERY_REQ(prAdapter,
							prStaRec,
							prCmd->aucPeer,
							prCmd->ucActionCode,
							prCmd->ucDialogToken,
							prCmd->u2StatusCode,
						    (UINT_8 *) (prCmd->aucSecBuf),
							prCmd->u4SecBufLen) != TDLS_STATUS_SUCCESS) {
				return -1;
			}

			
			break;

		case TDLS_FRM_ACTION_SETUP_REQ:
		/*                                                    */
		if (prStaRec == NULL) {
				return 0;
			}
		prStaRec =
		    cnmGetTdlsPeerByAddress(prAdapter, prAdapter->prAisBssInfo->ucBssIndex,
					    prCmd->aucPeer);
			g_arTdlsLink[prStaRec->ucTdlsIndex] = 0;
		if (TdlsDataFrameSend_SETUP_REQ(prAdapter,
							prStaRec,
							prCmd->aucPeer,
							prCmd->ucActionCode,
							prCmd->ucDialogToken,
							prCmd->u2StatusCode,
						(UINT_8 *) (prCmd->aucSecBuf),
							prCmd->u4SecBufLen) != TDLS_STATUS_SUCCESS) {
				return -1;
			}

			break;

		case TDLS_FRM_ACTION_SETUP_RSP:

            /*                                                                                                                        */
		/*                              */
		if (prBssInfo->fgTdlsIsProhibited)
                return 0;                 
            
		/*                                                    */
		if (TdlsDataFrameSend_SETUP_RSP(prAdapter,
							prStaRec,
							prCmd->aucPeer,
							prCmd->ucActionCode,
							prCmd->ucDialogToken,
							prCmd->u2StatusCode,
						(UINT_8 *) (prCmd->aucSecBuf),
							prCmd->u4SecBufLen) != TDLS_STATUS_SUCCESS) {
				return -1;
			}

			break;

		case TDLS_FRM_ACTION_DISCOVERY_RSP:	
		/*                                                        */
		if (TdlsDataFrameSend_DISCOVERY_RSP(prAdapter,
							prStaRec,
							prCmd->aucPeer,
							prCmd->ucActionCode,
							prCmd->ucDialogToken,
							prCmd->u2StatusCode,
						    (UINT_8 *) (prCmd->aucSecBuf),
							prCmd->u4SecBufLen) != TDLS_STATUS_SUCCESS) {
				return -1;
			}

			break;

		case TDLS_FRM_ACTION_CONFIRM:
		/*                                                  */
		if (TdlsDataFrameSend_CONFIRM(prAdapter,
							prStaRec,
							prCmd->aucPeer,
							prCmd->ucActionCode,
							prCmd->ucDialogToken,
							prCmd->u2StatusCode,
					      (UINT_8 *) (prCmd->aucSecBuf),
							prCmd->u4SecBufLen) != TDLS_STATUS_SUCCESS) {	
				return -1;
			}
			break;
			
		case TDLS_FRM_ACTION_TEARDOWN:


		prStaRec =
		    cnmGetTdlsPeerByAddress(prAdapter, prAdapter->prAisBssInfo->ucBssIndex,
					    prCmd->aucPeer);
		if (prCmd->u2StatusCode == TDLS_REASON_CODE_UNREACHABLE) {
			/*                                                                       */
                g_arTdlsLink[prStaRec->ucTdlsIndex] = 0;
            }
		/*                                                   */
		if (TdlsDataFrameSend_TearDown(prAdapter,
							prStaRec,
							prCmd->aucPeer,
							prCmd->ucActionCode,
							prCmd->ucDialogToken,
							prCmd->u2StatusCode,
					       (UINT_8 *) (prCmd->aucSecBuf),
							prCmd->u4SecBufLen) != TDLS_STATUS_SUCCESS) {	
			/*                                               */
				return -1;
			}
			break;

			default:
		/*                                  */
			return -EINVAL;
	}

	return 0;

}


/*                                                                            */
/* 
                                                                    
 
                                                        
             
             
                                                         
 
                             
                                    
*/
/*                                                                            */
UINT_32
TdlsexLinkOper(P_ADAPTER_T prAdapter,
	       PVOID pvSetBuffer, UINT_32 u4SetBufferLen, PUINT_32 pu4SetInfoLen)
{
	/*                              */

	/*                                                      */
    UINT_16         i;
	STA_RECORD_T *prStaRec;

	TDLS_CMD_LINK_OPER_T *prCmd;
	prCmd = (TDLS_CMD_LINK_OPER_T *) pvSetBuffer;

	switch (prCmd->oper) {

		case TDLS_ENABLE_LINK: 
            


		for (i = 0; i < MAXNUM_TDLS_PEER; i++) {
			if (!g_arTdlsLink[i]) {
					g_arTdlsLink[i]	= 1;
				prStaRec =
				    cnmGetTdlsPeerByAddress(prAdapter,
							    prAdapter->prAisBssInfo->ucBssIndex,
							    prCmd->aucPeerMac);
					prStaRec->ucTdlsIndex = i;
					break;
				}	
			}
            
		/*                                      */
			break;
		case TDLS_DISABLE_LINK:	 
 
		prStaRec =
		    cnmGetTdlsPeerByAddress(prAdapter, prAdapter->prAisBssInfo->ucBssIndex,
					    prCmd->aucPeerMac);

		/*                                                          */
			g_arTdlsLink[prStaRec->ucTdlsIndex] = 0;
		if (IS_DLS_STA(prStaRec))
				cnmStaRecFree(prAdapter, prStaRec);
            
			break;
		default:
			return 0;
	}

	
	return 0;
}


/*                                                                            */
/* 
                                                      
 
                                                        
             
 
                       
*/
/*                                                                            */
UINT_32 TdlsFrameGeneralIeAppend(ADAPTER_T *prAdapter, STA_RECORD_T *prStaRec, UINT_8 *pPkt)
{
	GLUE_INFO_T *prGlueInfo;
    BSS_INFO_T *prBssInfo;
	PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
	UINT_32 u4NonHTPhyType;
    UINT_16 u2SupportedRateSet;
	UINT_8 aucAllSupportedRates[RATE_NUM_SW] = { 0 };	/*                                   */
    UINT_8 ucAllSupportedRatesLen;
    UINT_8 ucSupRatesLen;
    UINT_8 ucExtSupRatesLen;
	UINT_32 u4PktLen, u4IeLen;




	/*      */
	prGlueInfo = (GLUE_INFO_T *) prAdapter->prGlueInfo;
	prBssInfo = prAdapter->prAisBssInfo;	/*          */
	
    prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
	u4PktLen = 0;

	/*                                                  */
	/*                                 */
	u4NonHTPhyType = prStaRec->ucNonHTBasicPhyType;
	u2SupportedRateSet = rNonHTPhyAttributes[u4NonHTPhyType].u2SupportedRateSet;
	rateGetDataRatesFromRateSet(u2SupportedRateSet,
				    0, aucAllSupportedRates, &ucAllSupportedRatesLen);
	
	ucSupRatesLen = ((ucAllSupportedRatesLen > ELEM_MAX_LEN_SUP_RATES) ?
					 ELEM_MAX_LEN_SUP_RATES : ucAllSupportedRatesLen);


	ucExtSupRatesLen = ucAllSupportedRatesLen - ucSupRatesLen;
		
	if (ucSupRatesLen) {
		SUP_RATES_IE(pPkt)->ucId = ELEM_ID_SUP_RATES;
		SUP_RATES_IE(pPkt)->ucLength = ucSupRatesLen;
		kalMemCopy(SUP_RATES_IE(pPkt)->aucSupportedRates,
			   aucAllSupportedRates, ucSupRatesLen);
	
		u4IeLen = IE_SIZE(pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}

	/*                                                     */
	if (ucExtSupRatesLen) {
	
		EXT_SUP_RATES_IE(pPkt)->ucId = ELEM_ID_EXTENDED_SUP_RATES;
		EXT_SUP_RATES_IE(pPkt)->ucLength = ucExtSupRatesLen;
	
		kalMemCopy(EXT_SUP_RATES_IE(pPkt)->aucExtSupportedRates,
			   &aucAllSupportedRates[ucSupRatesLen], ucExtSupRatesLen);
	
		u4IeLen = IE_SIZE(pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}

	/*                                                     */
	SUPPORTED_CHANNELS_IE(pPkt)->ucId = ELEM_ID_SUP_CHS;
	SUPPORTED_CHANNELS_IE(pPkt)->ucLength = 2;
	SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[0] = 1;
	SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[1] = 13;

	if (prAdapter->fgEnable5GBand == TRUE) {
		SUPPORTED_CHANNELS_IE(pPkt)->ucLength = 10;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[2] = 36;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[3] = 4;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[4] = 52;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[5] = 4;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[6] = 149;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[7] = 4;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[8] = 165;
		SUPPORTED_CHANNELS_IE(pPkt)->ucChannelNum[9] = 1;
	}


	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;

	return u4PktLen;
}


 /*                                                                              
                                       
                                                                                 
 */

/* 
                                                              
 
                                                        
             
             
                                                         
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS
TdlsDataFrameSend_TearDown(ADAPTER_T *prAdapter,
	STA_RECORD_T						*prStaRec,
	UINT_8								*pPeerMac,
	UINT_8								ucActionCode,
	UINT_8								ucDialogToken,
			   UINT_16 u2StatusCode, UINT_8 *pAppendIe, UINT_32 AppendIeLen)
{

	GLUE_INFO_T *prGlueInfo;
    BSS_INFO_T *prBssInfo;
	PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;
	UINT_16 ReasonCode;


	/*                      */
	prGlueInfo = (GLUE_INFO_T *) prAdapter->prGlueInfo;
	prBssInfo = prAdapter->prAisBssInfo;	/*          */

	
    prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
	u4PktLen = 0;

    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
        return TDLS_STATUS_RESOURCES;
    }

    prMsduInfo->dev = prGlueInfo->prDevHandler;
	if (prMsduInfo->dev == NULL) {
        kalPacketFree(prGlueInfo, prMsduInfo);
        return TDLS_STATUS_FAIL;
    }

	/*                       */
	/*                 */
	kalMemCopy(pPkt, pPeerMac, TDLS_FME_MAC_ADDR_LEN);
	pPkt += TDLS_FME_MAC_ADDR_LEN;
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	pPkt += TDLS_FME_MAC_ADDR_LEN;
	*(UINT_16 *) pPkt = htons(TDLS_FRM_PROT_TYPE);
	pPkt += 2;
	u4PktLen += TDLS_FME_MAC_ADDR_LEN * 2 + 2;

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	pPkt++;
	u4PktLen++;

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	pPkt++;
	u4PktLen++;

	/*                                 */
	*pPkt = ucActionCode;
	pPkt++;
	u4PktLen++;

	/*                                  */
	
    
	ReasonCode = u2StatusCode;

	/*                                                 */

    
	kalMemCopy(pPkt, &ReasonCode, 2);
	pPkt = pPkt + 2;
	u4PktLen = u4PktLen + 2;

	if (pAppendIe != NULL) {
		if ((ucActionCode != TDLS_FRM_ACTION_TEARDOWN) ||
		    ((ucActionCode == TDLS_FRM_ACTION_TEARDOWN) && (prStaRec != NULL))) {
			kalMemCopy(pPkt, pAppendIe, AppendIeLen);
			LR_TDLS_FME_FIELD_FILL(AppendIeLen);
		}
	}

    /*                                          */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
	    /*                                                                                */
		u4IeLen = rlmDomainSupOperatingClassIeFill(pPkt);
		LR_TDLS_FME_FIELD_FILL(u4IeLen);
	}


	/*                                                   */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = 18;

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, prAdapter->rMyMacAddr, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, pPeerMac, 6);

	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;

	/*                         */
	prMsduInfo->len = u4PktLen;

	/*                                    */
	/*                                              */
	/*   */


	/*                                             */
	
	/*                        */
	wlanHardStartXmit(prMsduInfo, prMsduInfo->dev);

	return TDLS_STATUS_SUCCESS;
}


/* 
                                                              
 
                                                        
             
             
                                                         
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS			/*             */
TdlsDataFrameSend_SETUP_REQ(ADAPTER_T *prAdapter,
	STA_RECORD_T						*prStaRec,
	UINT_8								*pPeerMac,
	UINT_8								ucActionCode,
	UINT_8								ucDialogToken,
			    UINT_16 u2StatusCode, UINT_8 *pAppendIe, UINT_32 AppendIeLen)
{


	GLUE_INFO_T *prGlueInfo;
    BSS_INFO_T *prBssInfo;
	PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;
	BOOLEAN fg40mAllowed;
	UINT_16 u2CapInfo;



	/*                      */
	prGlueInfo = (GLUE_INFO_T *) prAdapter->prGlueInfo;
	prBssInfo = prAdapter->prAisBssInfo;	/*          */

	
	
    prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
	u4PktLen = 0;

    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
        return TDLS_STATUS_RESOURCES;
    }

    prMsduInfo->dev = prGlueInfo->prDevHandler;
    if (prMsduInfo->dev == NULL) {
        kalPacketFree(prGlueInfo, prMsduInfo);
        return TDLS_STATUS_FAIL;
    }

	/*                       */
	/*                 */
	kalMemCopy(pPkt, pPeerMac, TDLS_FME_MAC_ADDR_LEN);
	pPkt += TDLS_FME_MAC_ADDR_LEN;
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	pPkt += TDLS_FME_MAC_ADDR_LEN;
	*(UINT_16 *) pPkt = htons(TDLS_FRM_PROT_TYPE);
	pPkt += 2;
	u4PktLen += TDLS_FME_MAC_ADDR_LEN * 2 + 2;

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	pPkt++;
	u4PktLen++;

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	pPkt++;
	u4PktLen++;

	/*                                 */
	*pPkt = ucActionCode;
	pPkt++;
	u4PktLen++;

	/*                                       */
	*pPkt = ucDialogToken;
	pPkt++;
	u4PktLen++;

	/*                                     */
	u2CapInfo = assocBuildCapabilityInfo(prAdapter, prStaRec);
	WLAN_SET_FIELD_16(pPkt, u2CapInfo);
	pPkt = pPkt + 2;
	u4PktLen = u4PktLen + 2;

	/*                       */
	u4IeLen = TdlsFrameGeneralIeAppend(prAdapter, prStaRec, pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;

	/*                     */
	kalMemCopy(pPkt, pAppendIe, AppendIeLen);
	pPkt += AppendIeLen;
	u4PktLen += AppendIeLen;

	/*                                                         */
	EXT_CAP_IE(pPkt)->ucId = ELEM_ID_EXTENDED_CAP;
	EXT_CAP_IE(pPkt)->ucLength = 5;
/*         */
	EXT_CAP_IE(pPkt)->aucCapabilities[0] = 0x00; /*             */
	EXT_CAP_IE(pPkt)->aucCapabilities[1] = 0x00; /*              */
	EXT_CAP_IE(pPkt)->aucCapabilities[2] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[3] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[4] = 0xFF; /*               */


	EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((28 - 24));
	EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((30 - 24));
	EXT_CAP_IE(pPkt)->aucCapabilities[4] |= BIT((37 - 32));

	/*                                              */ /*               */

	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;


	/*                              */
	HT_CAP_IE(pPkt)->ucId = ELEM_ID_HT_CAP;
	HT_CAP_IE(pPkt)->ucLength = 26;


	/*                                                   */	
	if ((prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11N)) {
		/*                                                           */
		if (cnmBss40mBwPermitted(prAdapter, prBssInfo->ucBssIndex))
			fg40mAllowed = TRUE;
		else
			fg40mAllowed = FALSE;
		
		/*           */ /*                       */
		u4IeLen = rlmFillHtCapIEByAdapter(prAdapter, prBssInfo, pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
		/*                                 */
	}
/*       */

	/*                                                                       */
	TIMEOUT_INTERVAL_IE(pPkt)->ucId = ELEM_ID_TIMEOUT_INTERVAL;
	TIMEOUT_INTERVAL_IE(pPkt)->ucLength = 5;

	TIMEOUT_INTERVAL_IE(pPkt)->ucType = 2;	/*                                        */
	TIMEOUT_INTERVAL_IE(pPkt)->u4Value = TDLS_KEY_TIMEOUT_INTERVAL;	/*                          */

	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;

	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
		/*
                                                                     
                                                                       
                                                                
             
 
                                                                         
                                                                     
                             
 
                                                                       
                                                            
  */
		BSS_20_40_COEXIST_IE(pPkt)->ucId = ELEM_ID_20_40_BSS_COEXISTENCE;
		BSS_20_40_COEXIST_IE(pPkt)->ucLength = 1;
		BSS_20_40_COEXIST_IE(pPkt)->ucData = 0x01;
		LR_TDLS_FME_FIELD_FILL(3);
	}

	if (pAppendIe != NULL) {
		if ((ucActionCode != TDLS_FRM_ACTION_TEARDOWN) ||
		    ((ucActionCode == TDLS_FRM_ACTION_TEARDOWN) && (prStaRec != NULL))) {
			kalMemCopy(pPkt, pAppendIe, AppendIeLen);
			LR_TDLS_FME_FIELD_FILL(AppendIeLen);
		}
	}

    /*                                          */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
	    /*                                                                                */
		u4IeLen = rlmDomainSupOperatingClassIeFill(pPkt);
		LR_TDLS_FME_FIELD_FILL(u4IeLen);
	}

	/*                                                   */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = 18;

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, prAdapter->rMyMacAddr, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, pPeerMac, 6);

	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;





	/*                                                   */

	/*                  */
	if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
		
		/*            */ /*                       */
		u4IeLen = mqmGenerateWmmInfoIEByStaRec(prAdapter, prBssInfo, prStaRec, pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}


	if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
		u4IeLen = mqmGenerateWmmParamIEByParam(prAdapter, prBssInfo, pPkt);
    
        LR_TDLS_FME_FIELD_FILL(u4IeLen);
    }

#if CFG_SUPPORT_802_11AC
	if (prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11AC) {
		/*            */ /*                       */
		u4IeLen = rlmFillVhtCapIEByAdapter(prAdapter, prBssInfo, pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}
#endif

	/*                         */
	prMsduInfo->len = u4PktLen;


	printk("\n\n\n	wlanHardStartXmit\n\n\n");

	/*                        */
	wlanHardStartXmit(prMsduInfo, prMsduInfo->dev);
	/*            */
	return TDLS_STATUS_SUCCESS;
}

WLAN_STATUS
TdlsDataFrameSend_SETUP_RSP(ADAPTER_T *prAdapter,
	STA_RECORD_T						*prStaRec,
	UINT_8								*pPeerMac,
	UINT_8								ucActionCode,
	UINT_8								ucDialogToken,
			    UINT_16 u2StatusCode, UINT_8 *pAppendIe, UINT_32 AppendIeLen)
{
	
	GLUE_INFO_T *prGlueInfo;
	BSS_INFO_T *prBssInfo;
	PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;
	UINT_16 u2CapInfo;
	UINT_16 StatusCode;	
	BOOLEAN fg40mAllowed;

	
	/*                      */
	prGlueInfo = (GLUE_INFO_T *) prAdapter->prGlueInfo;
	prBssInfo = prAdapter->prAisBssInfo;	/*          */
	prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
	u4PktLen = 0;

	prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
	if (prMsduInfo == NULL) {
		return TDLS_STATUS_RESOURCES;
	}

	prMsduInfo->dev = prGlueInfo->prDevHandler;
	if (prMsduInfo->dev == NULL) {
		kalPacketFree(prGlueInfo, prMsduInfo);
		return TDLS_STATUS_FAIL;
	}

	/*                       */
	/*                 */
	kalMemCopy(pPkt, pPeerMac, TDLS_FME_MAC_ADDR_LEN);
	pPkt += TDLS_FME_MAC_ADDR_LEN;
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	pPkt += TDLS_FME_MAC_ADDR_LEN;
	*(UINT_16 *) pPkt = htons(TDLS_FRM_PROT_TYPE);
	pPkt += 2;
	u4PktLen += TDLS_FME_MAC_ADDR_LEN * 2 + 2;

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	pPkt++;
	u4PktLen++;

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	pPkt++;
	u4PktLen++;
	
	/*                                 */
	*pPkt = ucActionCode;
	pPkt++;
	u4PktLen++;


	/*                                  */
	StatusCode = u2StatusCode;
	kalMemCopy(pPkt, &StatusCode, 2);
	pPkt = pPkt + 2;
	u4PktLen = u4PktLen + 2;


	
	/*                                       */
	*pPkt = ucDialogToken;
	pPkt++;
	u4PktLen++;
	
	/*                                     */
	u2CapInfo = assocBuildCapabilityInfo(prAdapter, prStaRec);
	WLAN_SET_FIELD_16(pPkt, u2CapInfo);
	pPkt = pPkt + 2;
	u4PktLen = u4PktLen + 2;

	
	/*                       */
	u4IeLen = TdlsFrameGeneralIeAppend(prAdapter, prStaRec, pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;
	
	/*                     */
	kalMemCopy(pPkt, pAppendIe, AppendIeLen);
	pPkt += AppendIeLen;
	u4PktLen += AppendIeLen;
	
	/*                                                         */
	EXT_CAP_IE(pPkt)->ucId = ELEM_ID_EXTENDED_CAP;
	EXT_CAP_IE(pPkt)->ucLength = 5;
	
	EXT_CAP_IE(pPkt)->aucCapabilities[0] = 0x00; /*             */
	EXT_CAP_IE(pPkt)->aucCapabilities[1] = 0x00; /*              */
	EXT_CAP_IE(pPkt)->aucCapabilities[2] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[3] = 0x00; /*               */
	EXT_CAP_IE(pPkt)->aucCapabilities[4] = 0xFF; /*               */
	
	EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((28 - 24));

	EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((30 - 24));
	EXT_CAP_IE(pPkt)->aucCapabilities[4] |= BIT((37 - 32));
	/*                                              */ /*               */
	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;
	
	/*                         */
	HT_CAP_IE(pPkt)->ucId = ELEM_ID_HT_CAP;
	HT_CAP_IE(pPkt)->ucLength = 26;

	
	/*                                                   */
	if ((prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11N)) {
		/*                                                           */
		if (cnmBss40mBwPermitted(prAdapter, prBssInfo->ucBssIndex))
			fg40mAllowed = TRUE;
		else
			fg40mAllowed = FALSE;
		
		/*           */ /*                       */
		u4IeLen = rlmFillHtCapIEByAdapter(prAdapter, prBssInfo, pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;	
	}
		
	/*                                                                       */
	TIMEOUT_INTERVAL_IE(pPkt)->ucId = ELEM_ID_TIMEOUT_INTERVAL;
	TIMEOUT_INTERVAL_IE(pPkt)->ucLength = 5;
	TIMEOUT_INTERVAL_IE(pPkt)->ucType = 2;	/*                                        */
	TIMEOUT_INTERVAL_IE(pPkt)->u4Value = TDLS_KEY_TIMEOUT_INTERVAL;	/*                          */
	
	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;

	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
		/*
                                                                     
                                                                       
                                                                
             
 
                                                                         
                                                                     
                             
 
                                                                       
                                                            
  */
		BSS_20_40_COEXIST_IE(pPkt)->ucId = ELEM_ID_20_40_BSS_COEXISTENCE;
		BSS_20_40_COEXIST_IE(pPkt)->ucLength = 1;
		BSS_20_40_COEXIST_IE(pPkt)->ucData = 0x01;
		LR_TDLS_FME_FIELD_FILL(3);
	}

	if (pAppendIe != NULL) {
		if ((ucActionCode != TDLS_FRM_ACTION_TEARDOWN) ||
		    ((ucActionCode == TDLS_FRM_ACTION_TEARDOWN) && (prStaRec != NULL))) {
			kalMemCopy(pPkt, pAppendIe, AppendIeLen);
			LR_TDLS_FME_FIELD_FILL(AppendIeLen);
		}
	}

    /*                                          */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
	    /*                                                                                */
		u4IeLen = rlmDomainSupOperatingClassIeFill(pPkt);
		LR_TDLS_FME_FIELD_FILL(u4IeLen);
	}

    
	/*                                                   */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = 18;

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, pPeerMac, 6);	/*                       */
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, prAdapter->rMyMacAddr, 6);	/*          */
	
	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;


	/*                  */
	/*                  */
	if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
		
		/*            */ /*                       */
		u4IeLen = mqmGenerateWmmInfoIEByStaRec(prAdapter, prBssInfo, prStaRec, pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}


	if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
		u4IeLen = mqmGenerateWmmParamIEByParam(prAdapter, prBssInfo, pPkt);
    
        LR_TDLS_FME_FIELD_FILL(u4IeLen);
    }

#if CFG_SUPPORT_802_11AC
		if (prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11AC) {
		/*            */ /*                       */
			u4IeLen = rlmFillVhtCapIEByAdapter(prAdapter, prBssInfo, pPkt);
			pPkt += u4IeLen;
			u4PktLen += u4IeLen;
		}
#endif

	
	/*                         */
	prMsduInfo->len = u4PktLen;
	
	/*                        */
	wlanHardStartXmit(prMsduInfo, prMsduInfo->dev);
	
		return TDLS_STATUS_SUCCESS;
}


WLAN_STATUS
TdlsDataFrameSend_CONFIRM(ADAPTER_T *prAdapter,
	STA_RECORD_T						*prStaRec,
	UINT_8								*pPeerMac,
	UINT_8								ucActionCode,
	UINT_8								ucDialogToken,
			  UINT_16 u2StatusCode, UINT_8 *pAppendIe, UINT_32 AppendIeLen)
{

	GLUE_INFO_T *prGlueInfo;
    BSS_INFO_T *prBssInfo;
	PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen, u4IeLen;
	UINT_16 StatusCode;

	/*                      */
	prGlueInfo = (GLUE_INFO_T *) prAdapter->prGlueInfo;
	prBssInfo = prAdapter->prAisBssInfo;	/*          */

    prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
	u4PktLen = 0;

    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
        return TDLS_STATUS_RESOURCES;
    }

    prMsduInfo->dev = prGlueInfo->prDevHandler;
	if (prMsduInfo->dev == NULL) {
        kalPacketFree(prGlueInfo, prMsduInfo);
        return TDLS_STATUS_FAIL;
    }

	/*                       */
	/*                 */
	kalMemCopy(pPkt, pPeerMac, TDLS_FME_MAC_ADDR_LEN);
	pPkt += TDLS_FME_MAC_ADDR_LEN;
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	pPkt += TDLS_FME_MAC_ADDR_LEN;
	*(UINT_16 *) pPkt = htons(TDLS_FRM_PROT_TYPE);
	pPkt += 2;
	u4PktLen += TDLS_FME_MAC_ADDR_LEN * 2 + 2;

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	pPkt++;
	u4PktLen++;

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	pPkt++;
	u4PktLen++;

	/*                                 */
	*pPkt = ucActionCode;
	pPkt++;
	u4PktLen++;

	/*                                  */
	
	StatusCode = u2StatusCode;	/*                                  */
	kalMemCopy(pPkt, &StatusCode, 2);
	pPkt = pPkt + 2;
	u4PktLen = u4PktLen + 2;


	/*                                       */
	*pPkt = ucDialogToken;
	pPkt++;
	u4PktLen++;

	/*                     */
	kalMemCopy(pPkt, pAppendIe, AppendIeLen);
	pPkt += AppendIeLen;
	u4PktLen += AppendIeLen;


	/*                                                                       */
	TIMEOUT_INTERVAL_IE(pPkt)->ucId = ELEM_ID_TIMEOUT_INTERVAL;
	TIMEOUT_INTERVAL_IE(pPkt)->ucLength = 5;
	
	TIMEOUT_INTERVAL_IE(pPkt)->ucType = 2;
	TIMEOUT_INTERVAL_IE(pPkt)->u4Value = TDLS_KEY_TIMEOUT_INTERVAL;	/*                          */
	
	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;

	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
		/*
                                                                     
                                                                       
                                                                
             
 
                                                                         
                                                                     
                             
 
                                                                       
                                                            
  */
		BSS_20_40_COEXIST_IE(pPkt)->ucId = ELEM_ID_20_40_BSS_COEXISTENCE;
		BSS_20_40_COEXIST_IE(pPkt)->ucLength = 1;
		BSS_20_40_COEXIST_IE(pPkt)->ucData = 0x01;
		LR_TDLS_FME_FIELD_FILL(3);
	}

	if (pAppendIe != NULL) {
		if ((ucActionCode != TDLS_FRM_ACTION_TEARDOWN) ||
		    ((ucActionCode == TDLS_FRM_ACTION_TEARDOWN) && (prStaRec != NULL))) {
			kalMemCopy(pPkt, pAppendIe, AppendIeLen);
			LR_TDLS_FME_FIELD_FILL(AppendIeLen);
		}
	}

    /*                                          */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
	    /*                                                                                */
		u4IeLen = rlmDomainSupOperatingClassIeFill(pPkt);
		LR_TDLS_FME_FIELD_FILL(u4IeLen);
	}



	/*                                                   */
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
	TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = 18;

	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, prAdapter->rMyMacAddr, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, pPeerMac, 6);

	u4IeLen = IE_SIZE(pPkt);
	pPkt += u4IeLen;
	u4PktLen += u4IeLen;

	/*                  */
	if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
		
		/*            */ /*                       */
		u4IeLen = mqmGenerateWmmInfoIEByStaRec(prAdapter, prBssInfo, prStaRec, pPkt);
		pPkt += u4IeLen;
		u4PktLen += u4IeLen;
	}


	if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
		u4IeLen = mqmGenerateWmmParamIEByParam(prAdapter, prBssInfo, pPkt);
    
        LR_TDLS_FME_FIELD_FILL(u4IeLen);
    }


	/*                         */
	prMsduInfo->len = u4PktLen;

	/*                        */
	wlanHardStartXmit(prMsduInfo, prMsduInfo->dev);

	return TDLS_STATUS_SUCCESS;
}



/*
                                                              
 
                                                        
             
             
                                                         
 
                             
                                    
*/
/*                                                                            */
WLAN_STATUS			/*             */
TdlsDataFrameSend_DISCOVERY_REQ(ADAPTER_T *prAdapter,
	STA_RECORD_T						*prStaRec,
	UINT_8								*pPeerMac,
	UINT_8								ucActionCode,
	UINT_8								ucDialogToken,
				UINT_16 u2StatusCode, UINT_8 *pAppendIe, UINT_32 AppendIeLen)
{
#define LR_TDLS_FME_FIELD_FILL(__Len) \
		pPkt += __Len; \
		u4PktLen += __Len;
		
		GLUE_INFO_T *prGlueInfo;
		BSS_INFO_T *prBssInfo;
		PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
		struct sk_buff *prMsduInfo;
		MSDU_INFO_T *prMsduInfoMgmt;
	UINT_8 *pPkt, *pucInitiator, *pucResponder;
	UINT_32 u4PktLen, u4IeLen;
	UINT_16 u2CapInfo;
	
	
	prGlueInfo = (GLUE_INFO_T *) prAdapter->prGlueInfo;
	
		if (prStaRec != NULL) {
		prBssInfo = prAdapter->prAisBssInfo;	/*          */

	} else {
			return TDLS_STATUS_FAIL;
		}



		/*                      */
		prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
		u4PktLen = 0;
		prMsduInfo = NULL;
		prMsduInfoMgmt = NULL;
	
		/*                       */
	if (ucActionCode != TDLS_FRM_ACTION_DISCOVERY_RSP) {
			/*                                   */
			prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
		if (prMsduInfo == NULL) {

				return TDLS_STATUS_RESOURCES;
			}
			
			prMsduInfo->dev = prGlueInfo->prDevHandler;
		if (prMsduInfo->dev == NULL) {

				kalPacketFree(prGlueInfo, prMsduInfo);
				return TDLS_STATUS_FAIL;
			}
	
			/*                 */
			kalMemCopy(pPkt, pPeerMac, TDLS_FME_MAC_ADDR_LEN);
			LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
			kalMemCopy(pPkt, prBssInfo->aucOwnMacAddr, TDLS_FME_MAC_ADDR_LEN);
			LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
		*(UINT_16 *) pPkt = htons(TDLS_FRM_PROT_TYPE);
			LR_TDLS_FME_FIELD_FILL(2);
	
			/*                 */
			*pPkt = TDLS_FRM_PAYLOAD_TYPE;
			LR_TDLS_FME_FIELD_FILL(1);
	
			/*                                   */
			*pPkt = TDLS_FRM_CATEGORY;
			LR_TDLS_FME_FIELD_FILL(1);
	} else {
			WLAN_MAC_HEADER_T *prHdr;
	
		prMsduInfoMgmt = (MSDU_INFO_T *)
				cnmMgtPktAlloc(prAdapter, PUBLIC_ACTION_MAX_LEN);
		if (prMsduInfoMgmt == NULL) {

				return TDLS_STATUS_RESOURCES;
			}
	
		pPkt = (UINT_8 *) prMsduInfoMgmt->prPacket;
		prHdr = (WLAN_MAC_HEADER_T *) pPkt;
	
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
	switch (ucActionCode) {
			case TDLS_FRM_ACTION_SETUP_RSP:
			case TDLS_FRM_ACTION_CONFIRM:
			case TDLS_FRM_ACTION_TEARDOWN:
				WLAN_SET_FIELD_16(pPkt, u2StatusCode);
				LR_TDLS_FME_FIELD_FILL(2);
				break;
		}
	
		/*                                       */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
			*pPkt = ucDialogToken;
			LR_TDLS_FME_FIELD_FILL(1);
		}
	
		/*               */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
			/*
              
   
                 
                          
                      
                   
                           
 
                         
                   
   
         
        
                    
   */
		if (ucActionCode != TDLS_FRM_ACTION_CONFIRM) {
				/*                                                                         */
				u2CapInfo = assocBuildCapabilityInfo(prAdapter, prStaRec);
				WLAN_SET_FIELD_16(pPkt, u2CapInfo);
				LR_TDLS_FME_FIELD_FILL(2);
	
				/*                       */
				/*
                                                                        
                              
 
                                                               
                           
    */
				u4IeLen = TdlsFrameGeneralIeAppend(prAdapter, prStaRec, pPkt);
				LR_TDLS_FME_FIELD_FILL(u4IeLen);
	
				/*                                                    */
				EXT_CAP_IE(pPkt)->ucId = ELEM_ID_EXTENDED_CAP;
				EXT_CAP_IE(pPkt)->ucLength = 5;
	
				EXT_CAP_IE(pPkt)->aucCapabilities[0] = 0x00; /*             */
				EXT_CAP_IE(pPkt)->aucCapabilities[1] = 0x00; /*              */
				EXT_CAP_IE(pPkt)->aucCapabilities[2] = 0x00; /*               */
				EXT_CAP_IE(pPkt)->aucCapabilities[3] = 0x00; /*               */
				EXT_CAP_IE(pPkt)->aucCapabilities[4] = 0x00; /*               */
	
			/*                                              */
			EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((28 - 24));
			/*                                               */
			EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((30 - 24));
			/*                                        */
			EXT_CAP_IE(pPkt)->aucCapabilities[4] |= BIT((37 - 32));
	
				u4IeLen = IE_SIZE(pPkt);
				LR_TDLS_FME_FIELD_FILL(u4IeLen);
		} else {
				/*                                              */
			if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
					
				/*            */ /*                       */
				u4IeLen =
				    mqmGenerateWmmInfoIEByStaRec(prAdapter, prBssInfo, prStaRec,
								 pPkt);
					pPkt += u4IeLen;
					u4PktLen += u4IeLen;
				}

			}
		}
	
		/*                                            */
		/*
                                                                        
  */
#if 0
	if (prGlueInfo->fgTdlsIs2040Supported == TRUE) {
			/*
                                                                      
                                                                        
                                                                 
              
  
                                                                          
                                                                      
                              
  
                                                                        
                                                             
   */
			BSS_20_40_COEXIST_IE(pPkt)->ucId = ELEM_ID_20_40_BSS_COEXISTENCE;
			BSS_20_40_COEXIST_IE(pPkt)->ucLength = 1;
			BSS_20_40_COEXIST_IE(pPkt)->ucData = 0x01;
			LR_TDLS_FME_FIELD_FILL(3);
		}
#endif
		/*                                           */
	/*                                               */
	/*                                  */
	
		/*                                              */
		/*                                                                             */
		TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
		TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = 18;
	
		kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	
	switch (ucActionCode) {
			case TDLS_FRM_ACTION_SETUP_REQ:
			case TDLS_FRM_ACTION_CONFIRM:
			case TDLS_FRM_ACTION_DISCOVERY_RSP:
			default:
				/*                  */
				pucInitiator = prBssInfo->aucOwnMacAddr;
				pucResponder = pPeerMac;
	
				if (prStaRec != NULL)
					prStaRec->flgTdlsIsInitiator = TRUE;
				break;
	
			case TDLS_FRM_ACTION_SETUP_RSP:
				/*                   */
				pucInitiator = pPeerMac;
				pucResponder = prBssInfo->aucOwnMacAddr;
	
				if (prStaRec != NULL)
					prStaRec->flgTdlsIsInitiator = FALSE;
				break;
	
			case TDLS_FRM_ACTION_TEARDOWN:
		if (prStaRec != NULL) {
			if (prStaRec->flgTdlsIsInitiator == TRUE) {
						/*                  */
						pucInitiator = prBssInfo->aucOwnMacAddr;
						pucResponder = pPeerMac;
			} else {
						/*                   */
						pucInitiator = pPeerMac;
						pucResponder = prBssInfo->aucOwnMacAddr;
					}
		} else {
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
	if ((ucActionCode != TDLS_FRM_ACTION_TEARDOWN) && (pAppendIe != NULL)) {
			kalMemCopy(pPkt, pAppendIe, AppendIeLen);
			LR_TDLS_FME_FIELD_FILL(AppendIeLen);
		}
	
		/*                                       */
	if (ucActionCode != TDLS_FRM_ACTION_DISCOVERY_RSP) {
			/*                         */
			prMsduInfo->len = u4PktLen;
	
			wlanHardStartXmit(prMsduInfo, prMsduInfo->dev);
	} else {
			prMsduInfoMgmt->ucPacketType = TX_PACKET_TYPE_MGMT;
			prMsduInfoMgmt->ucStaRecIndex = prBssInfo->prStaRecOfAP->ucIndex;
			prMsduInfoMgmt->ucBssIndex = prBssInfo->ucBssIndex;
			prMsduInfoMgmt->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
			prMsduInfoMgmt->fgIs802_1x = FALSE;
			prMsduInfoMgmt->fgIs802_11 = TRUE;
			prMsduInfoMgmt->u2FrameLength = u4PktLen;
			prMsduInfoMgmt->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
			prMsduInfoMgmt->pfTxDoneHandler = NULL;
	
			
			/*                       */
			nicTxEnqueueMsdu(prAdapter, prMsduInfoMgmt);
		}
	
		return TDLS_STATUS_SUCCESS;
}




WLAN_STATUS
TdlsDataFrameSend_DISCOVERY_RSP(ADAPTER_T *prAdapter,
	STA_RECORD_T						*prStaRec,
	UINT_8								*pPeerMac,
	UINT_8								ucActionCode,
	UINT_8								ucDialogToken,
				UINT_16 u2StatusCode, UINT_8 *pAppendIe, UINT_32 AppendIeLen)
{
#define LR_TDLS_FME_FIELD_FILL(__Len) \
		pPkt += __Len; \
		u4PktLen += __Len;
		
		GLUE_INFO_T *prGlueInfo;
		BSS_INFO_T *prBssInfo;
		PM_PROFILE_SETUP_INFO_T *prPmProfSetupInfo;
		struct sk_buff *prMsduInfo;
		MSDU_INFO_T *prMsduInfoMgmt;
	UINT_8 *pPkt, *pucInitiator, *pucResponder;
	UINT_32 u4PktLen, u4IeLen;
	UINT_16 u2CapInfo;
	
	
	prGlueInfo = (GLUE_INFO_T *) prAdapter->prGlueInfo;
	
	
		/*              */
		if (prStaRec != NULL) {
		prBssInfo = prAdapter->prAisBssInfo;	/*          */

	} else {
			return TDLS_STATUS_FAIL;
		}

	
		/*                      */
		prPmProfSetupInfo = &prBssInfo->rPmProfSetupInfo;
		u4PktLen = 0;
		prMsduInfo = NULL;
		prMsduInfoMgmt = NULL;
	
		/*                       */
	if (ucActionCode != TDLS_FRM_ACTION_DISCOVERY_RSP) {
			/*                                   */
			prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
		if (prMsduInfo == NULL) {

				return TDLS_STATUS_RESOURCES;
			}
			
			prMsduInfo->dev = prGlueInfo->prDevHandler;
		if (prMsduInfo->dev == NULL) {

				kalPacketFree(prGlueInfo, prMsduInfo);
				return TDLS_STATUS_FAIL;
			}
	
			/*                 */
			kalMemCopy(pPkt, pPeerMac, TDLS_FME_MAC_ADDR_LEN);
			LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
			kalMemCopy(pPkt, prBssInfo->aucOwnMacAddr, TDLS_FME_MAC_ADDR_LEN);
			LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
		*(UINT_16 *) pPkt = htons(TDLS_FRM_PROT_TYPE);
			LR_TDLS_FME_FIELD_FILL(2);
	
			/*                 */
			*pPkt = TDLS_FRM_PAYLOAD_TYPE;
			LR_TDLS_FME_FIELD_FILL(1);
	
			/*                                   */
			*pPkt = TDLS_FRM_CATEGORY;
			LR_TDLS_FME_FIELD_FILL(1);
	} else {
			WLAN_MAC_HEADER_T *prHdr;
	
		prMsduInfoMgmt = (MSDU_INFO_T *)
				cnmMgtPktAlloc(prAdapter, PUBLIC_ACTION_MAX_LEN);
		if (prMsduInfoMgmt == NULL) {

				return TDLS_STATUS_RESOURCES;
			}
	
		pPkt = (UINT_8 *) prMsduInfoMgmt->prPacket;
		prHdr = (WLAN_MAC_HEADER_T *) pPkt;
	
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
	switch (ucActionCode) {
			case TDLS_FRM_ACTION_SETUP_RSP:
			case TDLS_FRM_ACTION_CONFIRM:
			case TDLS_FRM_ACTION_TEARDOWN:
				WLAN_SET_FIELD_16(pPkt, u2StatusCode);
				LR_TDLS_FME_FIELD_FILL(2);
				break;
		}
	
		/*                                       */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
			*pPkt = ucDialogToken;
			LR_TDLS_FME_FIELD_FILL(1);
		}
	
		/*               */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
			/*
              
   
                 
                          
                      
                   
                           
 
                         
                   
   
         
        
                    
   */
		if (ucActionCode != TDLS_FRM_ACTION_CONFIRM) {
				/*                                                                         */
				u2CapInfo = assocBuildCapabilityInfo(prAdapter, prStaRec);
				WLAN_SET_FIELD_16(pPkt, u2CapInfo);
				LR_TDLS_FME_FIELD_FILL(2);
	
				/*                       */
				/*
                                                                        
                              
 
                                                               
                           
    */
				u4IeLen = TdlsFrameGeneralIeAppend(prAdapter, prStaRec, pPkt);
				LR_TDLS_FME_FIELD_FILL(u4IeLen);
	
				/*                                                    */
				EXT_CAP_IE(pPkt)->ucId = ELEM_ID_EXTENDED_CAP;
				EXT_CAP_IE(pPkt)->ucLength = 5;
	
				EXT_CAP_IE(pPkt)->aucCapabilities[0] = 0x00; /*             */
				EXT_CAP_IE(pPkt)->aucCapabilities[1] = 0x00; /*              */
				EXT_CAP_IE(pPkt)->aucCapabilities[2] = 0x00; /*               */
				EXT_CAP_IE(pPkt)->aucCapabilities[3] = 0x00; /*               */
				EXT_CAP_IE(pPkt)->aucCapabilities[4] = 0x00; /*               */
	
			/*                                              */
			EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((28 - 24));
			/*                                               */
			EXT_CAP_IE(pPkt)->aucCapabilities[3] |= BIT((30 - 24));
			/*                                        */
			EXT_CAP_IE(pPkt)->aucCapabilities[4] |= BIT((37 - 32));
	
				u4IeLen = IE_SIZE(pPkt);
				LR_TDLS_FME_FIELD_FILL(u4IeLen);
		} else {
				/*                                              */
			if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
					
				/*            */ /*                       */
				u4IeLen =
				    mqmGenerateWmmInfoIEByStaRec(prAdapter, prBssInfo, prStaRec,
								 pPkt);
					pPkt += u4IeLen;
					u4PktLen += u4IeLen;
				}

			}
		}
	
		/*                                            */
		/*
                                                                        
  */
#if 0
	if (prGlueInfo->fgTdlsIs2040Supported == TRUE) {
			/*
                                                                      
                                                                        
                                                                 
              
  
                                                                          
                                                                      
                              
  
                                                                        
                                                             
   */
			BSS_20_40_COEXIST_IE(pPkt)->ucId = ELEM_ID_20_40_BSS_COEXISTENCE;
			BSS_20_40_COEXIST_IE(pPkt)->ucLength = 1;
			BSS_20_40_COEXIST_IE(pPkt)->ucData = 0x01;
			LR_TDLS_FME_FIELD_FILL(3);
		}
#endif
		/*                                           */
	/*                                               */
	/*                                  */
	
		/*                                              */
		/*                                                                             */
		TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
		TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = 18;
	
		kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	
	switch (ucActionCode) {
			case TDLS_FRM_ACTION_SETUP_REQ:
			case TDLS_FRM_ACTION_CONFIRM:
			default:
				/*                  */
				pucInitiator = prBssInfo->aucOwnMacAddr;
				pucResponder = pPeerMac;
	
				if (prStaRec != NULL)
					prStaRec->flgTdlsIsInitiator = TRUE;
				break;
			case TDLS_FRM_ACTION_DISCOVERY_RSP:	
			case TDLS_FRM_ACTION_SETUP_RSP:
				/*                   */
				pucInitiator = pPeerMac;
				pucResponder = prBssInfo->aucOwnMacAddr;
	
				if (prStaRec != NULL)
					prStaRec->flgTdlsIsInitiator = FALSE;
				break;
	
			case TDLS_FRM_ACTION_TEARDOWN:
		if (prStaRec != NULL) {
			if (prStaRec->flgTdlsIsInitiator == TRUE) {
						/*                  */
						pucInitiator = prBssInfo->aucOwnMacAddr;
						pucResponder = pPeerMac;
			} else {
						/*                   */
						pucInitiator = pPeerMac;
						pucResponder = prBssInfo->aucOwnMacAddr;
					}
		} else {
					/*                   */
					pucInitiator = pPeerMac;
					pucResponder = prBssInfo->aucOwnMacAddr;
				}
				break;
		}
	
        /*                                                                       */
        TIMEOUT_INTERVAL_IE(pPkt)->ucId = ELEM_ID_TIMEOUT_INTERVAL;
        TIMEOUT_INTERVAL_IE(pPkt)->ucLength = 5;
	TIMEOUT_INTERVAL_IE(pPkt)->ucType = 2;	/*                                        */
	TIMEOUT_INTERVAL_IE(pPkt)->u4Value = TDLS_KEY_TIMEOUT_INTERVAL;	/*                          */
        
        u4IeLen = IE_SIZE(pPkt);
        pPkt += u4IeLen;
        u4PktLen += u4IeLen;
    
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
            /*
                                                                                  
                                                                                    
                                                                             
                          
        
                                                                                      
                                                                                  
                                          
        
                                                                                    
                                                                         
            */
            BSS_20_40_COEXIST_IE(pPkt)->ucId = ELEM_ID_20_40_BSS_COEXISTENCE;
            BSS_20_40_COEXIST_IE(pPkt)->ucLength = 1;
            BSS_20_40_COEXIST_IE(pPkt)->ucData = 0x01;
            LR_TDLS_FME_FIELD_FILL(3);
        }
    
	if (pAppendIe != NULL) {

            kalMemCopy(pPkt, pAppendIe, AppendIeLen);
            LR_TDLS_FME_FIELD_FILL(AppendIeLen);

        }
    
        /*                                          */
	if (ucActionCode != TDLS_FRM_ACTION_TEARDOWN) {
            /*                                                                                */
            u4IeLen = rlmDomainSupOperatingClassIeFill(pPkt);
            LR_TDLS_FME_FIELD_FILL(u4IeLen);
        }
    
        
        /*                                                   */
        TDLS_LINK_IDENTIFIER_IE(pPkt)->ucId = ELEM_ID_LINK_IDENTIFIER;
        TDLS_LINK_IDENTIFIER_IE(pPkt)->ucLength = 18;
    
        kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aBSSID, prBssInfo->aucBSSID, 6);
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aInitiator, pPeerMac, 6);	/*                       */
	kalMemCopy(TDLS_LINK_IDENTIFIER_IE(pPkt)->aResponder, prAdapter->rMyMacAddr, 6);	/*          */
        
        u4IeLen = IE_SIZE(pPkt);
        pPkt += u4IeLen;
        u4PktLen += u4IeLen;
    
    
	/*                  */
	/*                  */
	if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
            
		/*            */ /*                       */
		u4IeLen = mqmGenerateWmmInfoIEByStaRec(prAdapter, prBssInfo, prStaRec, pPkt);
            pPkt += u4IeLen;
            u4PktLen += u4IeLen;
        }
    
    
	if (IS_FEATURE_ENABLED(prAdapter->rWifiVar.ucQoS)) {
		u4IeLen = mqmGenerateWmmParamIEByParam(prAdapter, prBssInfo, pPkt);
        
            LR_TDLS_FME_FIELD_FILL(u4IeLen);
        }
    
#if CFG_SUPPORT_802_11AC
            if (prAdapter->rWifiVar.ucAvailablePhyTypeSet & PHY_TYPE_SET_802_11AC) {
		/*            */ /*                       */
                u4IeLen = rlmFillVhtCapIEByAdapter(prAdapter, prBssInfo, pPkt);
                pPkt += u4IeLen;
                u4PktLen += u4IeLen;
            }
#endif

	
		/*                        */
	if ((ucActionCode != TDLS_FRM_ACTION_TEARDOWN) && (pAppendIe != NULL)) {
			kalMemCopy(pPkt, pAppendIe, AppendIeLen);
			LR_TDLS_FME_FIELD_FILL(AppendIeLen);
		}
	

			prMsduInfoMgmt->ucPacketType = TX_PACKET_TYPE_MGMT;
			prMsduInfoMgmt->ucStaRecIndex = prBssInfo->prStaRecOfAP->ucIndex;
			prMsduInfoMgmt->ucBssIndex = prBssInfo->ucBssIndex;
			prMsduInfoMgmt->ucMacHeaderLength = WLAN_MAC_MGMT_HEADER_LEN;
			prMsduInfoMgmt->fgIs802_1x = FALSE;
			prMsduInfoMgmt->fgIs802_11 = TRUE;
			prMsduInfoMgmt->u2FrameLength = u4PktLen;
			prMsduInfoMgmt->ucTxSeqNum = nicIncreaseTxSeqNum(prAdapter);
			prMsduInfoMgmt->pfTxDoneHandler = NULL;

			
			/*                       */
			nicTxEnqueueMsdu(prAdapter, prMsduInfoMgmt);

	
		return TDLS_STATUS_SUCCESS;
}


/*                                                                            */
/*                                                                  
 
                                                         
                                                            
                                                
                  
 
              
*/
/*                                                                            */
VOID TdlsexEventHandle(GLUE_INFO_T *prGlueInfo, UINT_8 *prInBuf, UINT_32 u4InBufLen)
{
	UINT_32 u4EventId;

	printk("TdlsexEventHandle\n");

	/*              */
	if ((prGlueInfo == NULL) || (prInBuf == NULL))
		return; /*                   */

	/*        */
	u4EventId = *(UINT_32 *) prInBuf;
	u4InBufLen -= 4;

	switch (u4EventId) {
		case TDLS_HOST_EVENT_TEAR_DOWN:
		printk("TDLS_HOST_EVENT_TEAR_DOWN\n");
		TdlsEventTearDown(prGlueInfo, prInBuf + 4, u4InBufLen);
			break;

		case TDLS_HOST_EVENT_TX_DONE:

			break;
	}
}


/*                                                                            */
/*                                                 
 
                                                         
                                                                               
                                                
                  
 
              
 
*/
/*                                                                            */
VOID TdlsEventTearDown(GLUE_INFO_T *prGlueInfo, UINT_8 *prInBuf, UINT_32 u4InBufLen)
{
	STA_RECORD_T *prStaRec;
	UINT_16 u2ReasonCode;
	UINT_32 u4TearDownSubId;
	UINT_8 *pMac, aucZeroMac[6];



	/*      */
	u4TearDownSubId = *(UINT_32 *) prInBuf;
	kalMemZero(aucZeroMac, sizeof(aucZeroMac));
	pMac = aucZeroMac;

	prStaRec = cnmGetStaRecByIndex(prGlueInfo->prAdapter, *(prInBuf + 4));
	if (prStaRec != NULL)
		pMac = prStaRec->aucMacAddr;

	/*        */

	/*              */
	if (prStaRec == NULL)
		return;

	if (fgIsPtiTimeoutSkip == TRUE) {
		/*                        */
		if (u4TearDownSubId == TDLS_HOST_EVENT_TD_PTI_TIMEOUT) {
			return;
		}
	}



	if (u4TearDownSubId == TDLS_HOST_EVENT_TD_PTI_TIMEOUT) {
		printk("TDLS_HOST_EVENT_TD_PTI_TIMEOUT TDLS_REASON_CODE_UNSPECIFIED\n");
		u2ReasonCode = TDLS_REASON_CODE_UNSPECIFIED;
        
        cfg80211_tdls_oper_request(prGlueInfo->prDevHandler,
					   prStaRec->aucMacAddr, NL80211_TDLS_TEARDOWN,
					   WLAN_REASON_TDLS_TEARDOWN_UNREACHABLE, GFP_ATOMIC);
	}	
    
	if (u4TearDownSubId == TDLS_HOST_EVENT_TD_AGE_TIMEOUT) {
		printk("TDLS_HOST_EVENT_TD_AGE_TIMEOUT TDLS_REASON_CODE_UNREACHABLE\n");
		u2ReasonCode = TDLS_REASON_CODE_UNREACHABLE;

        cfg80211_tdls_oper_request(prGlueInfo->prDevHandler,
					   prStaRec->aucMacAddr, NL80211_TDLS_TEARDOWN,
					   WLAN_REASON_TDLS_TEARDOWN_UNREACHABLE, GFP_ATOMIC);

	}	
 

	printk("\n\n u2ReasonCode = %u\n\n", u2ReasonCode);

    
	/*
                                                                                      
                                                                           
 */

	/*                                                                                        */
	

}

#if 0

/*                                                                            */
/*                                                                    
 
                                                        
                                                           
                                                
                  
 
              
 
*/
/*                                                                            */
VOID
tdls_oper_request(struct net_device *dev, const u8 *peer, u16 oper, u16 reason_code, gfp_t gfp)
{
    GLUE_INFO_T *prGlueInfo;
	ADAPTER_T *prAdapter;
	struct sk_buff *prMsduInfo;
	UINT_8 *pPkt;
	UINT_32 u4PktLen;


	/*              */
	if ((dev == NULL) || (peer == NULL))
		return; /*                   */

	/*      */
	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(dev));
	prAdapter = prGlueInfo->prAdapter;
	u4PktLen = 0;

	/*                      */
    prMsduInfo = kalPacketAlloc(prGlueInfo, 1600, &pPkt);
    if (prMsduInfo == NULL) {
        return;
    }
    prMsduInfo->dev = dev;

	/*                       */
	/*                 */
	kalMemCopy(pPkt, prAdapter->rMyMacAddr, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	kalMemCopy(pPkt, peer, TDLS_FME_MAC_ADDR_LEN);
	LR_TDLS_FME_FIELD_FILL(TDLS_FME_MAC_ADDR_LEN);
	*(UINT_16 *) pPkt = htons(TDLS_FRM_PROT_TYPE);
	LR_TDLS_FME_FIELD_FILL(2);

	/*                 */
	*pPkt = TDLS_FRM_PAYLOAD_TYPE;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                   */
	*pPkt = TDLS_FRM_CATEGORY;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                 */
	*pPkt = TDLS_FRM_ACTION_EVENT_TEAR_DOWN_TO_SUPPLICANT;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                    */
	*pPkt = oper;
	LR_TDLS_FME_FIELD_FILL(1);

	/*                                      */
	*pPkt = reason_code;
	*(pPkt + 1) = 0x00;
	LR_TDLS_FME_FIELD_FILL(2);

	/*                                   */
	kalMemCopy(pPkt, peer, 6);
	LR_TDLS_FME_FIELD_FILL(6);

	/*                         */
	prMsduInfo->len = u4PktLen;

	/*            */
	TdlsCmdTestRxIndicatePkts(prGlueInfo, prMsduInfo);
}



/*                                                                            */
/* 
                                                                   
 
                                                         
                                                     
 
              
 
*/
/*                                                                            */
VOID TdlsCmdTestRxIndicatePkts(GLUE_INFO_T *prGlueInfo, struct sk_buff *prSkb)
{
    struct net_device *prNetDev;


	/*      */
	prNetDev = prGlueInfo->prDevHandler;
	prGlueInfo->rNetDevStats.rx_bytes += prSkb->len;
	prGlueInfo->rNetDevStats.rx_packets++;

	/*                     */
	prNetDev->last_rx = jiffies;
	prSkb->protocol = eth_type_trans(prSkb, prNetDev);
	prSkb->dev = prNetDev;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
	if (!in_interrupt()) {
        netif_rx_ni(prSkb); /*                               */
	} else {
        netif_rx(prSkb);
    }
#else
    netif_rx(prSkb);
#endif
}
#endif

VOID TdlsBssExtCapParse(P_STA_RECORD_T prStaRec, P_UINT_8 pucIE)
{
	UINT_8 *pucIeExtCap;


	/*              */
	if ((prStaRec == NULL) || (pucIE == NULL))
		return;

	if (IE_ID(pucIE) != ELEM_ID_EXTENDED_CAP)
		return;

	/*
             

                         
                                                                                   
                                                                                         
          
 */
	if (IE_LEN(pucIE) < 5)
		return; /*                        */

	/*      */
	prStaRec->fgTdlsIsProhibited = FALSE;
	prStaRec->fgTdlsIsChSwProhibited = FALSE;

	/*       */
	pucIeExtCap = pucIE + 2;
	pucIeExtCap += 4; /*                                 */

	if ((*pucIeExtCap) && BIT(38 - 32))
		prStaRec->fgTdlsIsProhibited = TRUE;
	if ((*pucIeExtCap) && BIT(39 - 32))
		prStaRec->fgTdlsIsChSwProhibited = TRUE;


}







/*                                                                            */
/* 
                                                      
 
            
 
              
*/
/*                                                                            */
WLAN_STATUS
TdlsSendChSwControlCmd(P_ADAPTER_T prAdapter,
		       PVOID pvSetBuffer, UINT_32 u4SetBufferLen, PUINT_32 pu4SetInfoLen)
{

    CMD_TDLS_CH_SW_T rCmdTdlsChSwCtrl;


    ASSERT(prAdapter);


	/*                              */
    kalMemZero(&rCmdTdlsChSwCtrl, sizeof(CMD_TDLS_CH_SW_T));

    rCmdTdlsChSwCtrl.fgIsTDLSChSwProhibit = prAdapter->prAisBssInfo->fgTdlsIsChSwProhibited;

    wlanSendSetQueryCmd(prAdapter,
            CMD_ID_SET_TDLS_CH_SW,
            TRUE,
            FALSE,
            FALSE,
            NULL,
			    NULL, sizeof(CMD_TDLS_CH_SW_T), (PUINT_8) & rCmdTdlsChSwCtrl, NULL, 0);
    return TDLS_STATUS_SUCCESS;      
}





#endif /*                  */

/*               */
