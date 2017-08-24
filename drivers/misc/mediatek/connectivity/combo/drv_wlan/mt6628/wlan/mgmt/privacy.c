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

#if CFG_PRIVACY_MIGRATION

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
VOID secInit(IN P_ADAPTER_T prAdapter, IN UINT_8 ucNetTypeIdx)
{
	UINT_8 i;
	P_CONNECTION_SETTINGS_T prConnSettings;
	P_BSS_INFO_T prBssInfo;
	P_AIS_SPECIFIC_BSS_INFO_T prAisSpecBssInfo;

	DEBUGFUNC("secInit");

	ASSERT(prAdapter);

	prConnSettings = &prAdapter->rWifiVar.rConnSettings;
	prBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX];
	prAisSpecBssInfo = &prAdapter->rWifiVar.rAisSpecificBssInfo;

	prBssInfo->u4RsnSelectedGroupCipher = 0;
	prBssInfo->u4RsnSelectedPairwiseCipher = 0;
	prBssInfo->u4RsnSelectedAKMSuite = 0;

#if CFG_ENABLE_WIFI_DIRECT
	prBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX];

	prBssInfo->u4RsnSelectedGroupCipher = RSN_CIPHER_SUITE_CCMP;
	prBssInfo->u4RsnSelectedPairwiseCipher = RSN_CIPHER_SUITE_CCMP;
	prBssInfo->u4RsnSelectedAKMSuite = RSN_AKM_SUITE_PSK;
#endif

#if CFG_ENABLE_BT_OVER_WIFI
	prBssInfo = &prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_BOW_INDEX];

	prBssInfo->u4RsnSelectedGroupCipher = RSN_CIPHER_SUITE_CCMP;
	prBssInfo->u4RsnSelectedPairwiseCipher = RSN_CIPHER_SUITE_CCMP;
	prBssInfo->u4RsnSelectedAKMSuite = RSN_AKM_SUITE_PSK;
#endif

	prAdapter->rMib.dot11RSNAConfigPairwiseCiphersTable[0].dot11RSNAConfigPairwiseCipher =
	    WPA_CIPHER_SUITE_WEP40;
	prAdapter->rMib.dot11RSNAConfigPairwiseCiphersTable[1].dot11RSNAConfigPairwiseCipher =
	    WPA_CIPHER_SUITE_TKIP;
	prAdapter->rMib.dot11RSNAConfigPairwiseCiphersTable[2].dot11RSNAConfigPairwiseCipher =
	    WPA_CIPHER_SUITE_CCMP;
	prAdapter->rMib.dot11RSNAConfigPairwiseCiphersTable[3].dot11RSNAConfigPairwiseCipher =
	    WPA_CIPHER_SUITE_WEP104;

	prAdapter->rMib.dot11RSNAConfigPairwiseCiphersTable[4].dot11RSNAConfigPairwiseCipher =
	    RSN_CIPHER_SUITE_WEP40;
	prAdapter->rMib.dot11RSNAConfigPairwiseCiphersTable[5].dot11RSNAConfigPairwiseCipher =
	    RSN_CIPHER_SUITE_TKIP;
	prAdapter->rMib.dot11RSNAConfigPairwiseCiphersTable[6].dot11RSNAConfigPairwiseCipher =
	    RSN_CIPHER_SUITE_CCMP;
	prAdapter->rMib.dot11RSNAConfigPairwiseCiphersTable[7].dot11RSNAConfigPairwiseCipher =
	    RSN_CIPHER_SUITE_WEP104;

	for (i = 0; i < MAX_NUM_SUPPORTED_CIPHER_SUITES; i++) {
		prAdapter->rMib.dot11RSNAConfigPairwiseCiphersTable[i].
		    dot11RSNAConfigPairwiseCipherEnabled = FALSE;
	}

	prAdapter->rMib.dot11RSNAConfigAuthenticationSuitesTable[0].
	    dot11RSNAConfigAuthenticationSuite = WPA_AKM_SUITE_NONE;
	prAdapter->rMib.dot11RSNAConfigAuthenticationSuitesTable[1].
	    dot11RSNAConfigAuthenticationSuite = WPA_AKM_SUITE_802_1X;
	prAdapter->rMib.dot11RSNAConfigAuthenticationSuitesTable[2].
	    dot11RSNAConfigAuthenticationSuite = WPA_AKM_SUITE_PSK;
	prAdapter->rMib.dot11RSNAConfigAuthenticationSuitesTable[3].
	    dot11RSNAConfigAuthenticationSuite = RSN_AKM_SUITE_NONE;
	prAdapter->rMib.dot11RSNAConfigAuthenticationSuitesTable[4].
	    dot11RSNAConfigAuthenticationSuite = RSN_AKM_SUITE_802_1X;
	prAdapter->rMib.dot11RSNAConfigAuthenticationSuitesTable[5].
	    dot11RSNAConfigAuthenticationSuite = RSN_AKM_SUITE_PSK;

#if CFG_SUPPORT_802_11W
	prAdapter->rMib.dot11RSNAConfigAuthenticationSuitesTable[6].
	    dot11RSNAConfigAuthenticationSuite = RSN_AKM_SUITE_802_1X_SHA256;
	prAdapter->rMib.dot11RSNAConfigAuthenticationSuitesTable[7].
	    dot11RSNAConfigAuthenticationSuite = RSN_AKM_SUITE_PSK_SHA256;
#endif

	for (i = 0; i < MAX_NUM_SUPPORTED_AKM_SUITES; i++) {
		prAdapter->rMib.dot11RSNAConfigAuthenticationSuitesTable[i].
		    dot11RSNAConfigAuthenticationSuiteEnabled = FALSE;
	}

	secClearPmkid(prAdapter);

	cnmTimerInitTimer(prAdapter,
			  &prAisSpecBssInfo->rPreauthenticationTimer,
			  (PFN_MGMT_TIMEOUT_FUNC) rsnIndicatePmkidCand, (UINT_32) NULL);

#if CFG_SUPPORT_802_11W
	cnmTimerInitTimer(prAdapter,
			  &prAisSpecBssInfo->rSaQueryTimer,
			  (PFN_MGMT_TIMEOUT_FUNC) rsnStartSaQueryTimer, (UINT_32) NULL);
#endif

	prAisSpecBssInfo->fgCounterMeasure = FALSE;
	prAisSpecBssInfo->ucWEPDefaultKeyID = 0;


#if 0
	for (i = 0; i < WTBL_SIZE; i++) {
		g_prWifiVar->arWtbl[i].fgUsed = FALSE;
		g_prWifiVar->arWtbl[i].prSta = NULL;
		g_prWifiVar->arWtbl[i].ucNetTypeIdx = NETWORK_TYPE_INDEX_NUM;

	}
	nicPrivacyInitialize((UINT_8) NETWORK_TYPE_INDEX_NUM);
#endif
}				/*         */


/*                                                                            */
/* 
                                                                                
                     
 
                                                           
                                                 
 
                                          
*/
/*                                                                            */
BOOL secCheckClassError(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb, IN P_STA_RECORD_T prStaRec)
{
	ASSERT(prAdapter);
	ASSERT(prSwRfb);
	/*                   */

	/*                                                 */

	if ((prStaRec) && 1 /*                            */) {
		ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex = prStaRec->ucNetTypeIndex;

		if (IS_NET_ACTIVE(prAdapter, eNetTypeIndex)) {
			P_BSS_INFO_T prBssInfo;
			prBssInfo = &prAdapter->rWifiVar.arBssInfo[eNetTypeIndex];

			if ((STA_STATE_3 != prStaRec->ucStaState) &&
			    IS_BSS_ACTIVE(prBssInfo) && prBssInfo->fgIsNetAbsent == FALSE) {
				/*                                                    */

				if (WLAN_STATUS_SUCCESS == authSendDeauthFrame(prAdapter,
									       prStaRec,
									       NULL,
									       REASON_CODE_CLASS_3_ERR,
									       (PFN_TX_DONE_HANDLER)
									       NULL)) {

					DBGLOG(RSN, INFO,
					       ("Send Deauth to MAC:[" MACSTR
						"] for Rx Class 3 Error.\n",
						MAC2STR(prStaRec->aucMacAddr)));
				}

				return FALSE;
			}

			return secRxPortControlCheck(prAdapter, prSwRfb);
		}
	}

	return FALSE;
}				/*                             */


/*                                                                            */
/* 
                                                             
 
                                                        
                                      
                                         
 
              
 
*/
/*                                                                            */
VOID secSetPortBlocked(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prSta, IN BOOLEAN fgPortBlock)
{
	if (prSta == NULL)
		return;

	prSta->fgPortBlock = fgPortBlock;

	DBGLOG(RSN, TRACE,
	       ("The STA " MACSTR " port %s\n", MAC2STR(prSta->aucMacAddr),
		fgPortBlock == TRUE ? "BLOCK" : " OPEN"));
}


/*                                                                            */
/* 
                                                            
 
                                                        
                                      
                                          
 
                                             
 
*/
/*                                                                            */
BOOLEAN
secGetPortStatus(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prSta, OUT PBOOLEAN pfgPortStatus)
{
	if (prSta == NULL)
		return FALSE;

	*pfgPortStatus = prSta->fgPortBlock;

	return TRUE;
}


/*                                                                            */
/* 
                                                                               
 
                                                          
 
                                
                                                       
 
*/
/*                                                                            */
BOOL				/*                          */
secTxPortControlCheck(IN P_ADAPTER_T prAdapter,
		      IN P_MSDU_INFO_T prMsduInfo, IN P_STA_RECORD_T prStaRec)
{
	ASSERT(prAdapter);
	ASSERT(prMsduInfo);
	ASSERT(prStaRec);

	if (prStaRec) {

		/*        */
		if (prMsduInfo->fgIs802_1x)
			return TRUE;

		if (prStaRec->fgPortBlock == TRUE) {
			DBGLOG(INIT, TRACE, ("Drop Tx packet due Port Control!\n"));
			return FALSE;
		}
#if CFG_SUPPORT_WAPI
		if (prAdapter->rWifiVar.rConnSettings.fgWapiMode) {
			return TRUE;
		}
#endif
		if (IS_STA_IN_AIS(prStaRec)) {
			if (!prAdapter->rWifiVar.rAisSpecificBssInfo.fgTransmitKeyExist &&
			    (prAdapter->rWifiVar.rConnSettings.eEncStatus ==
			     ENUM_ENCRYPTION1_ENABLED)) {
				DBGLOG(INIT, TRACE, ("Drop Tx packet due the key is removed!!!\n"));
				return FALSE;
			}
		}
	}

	return TRUE;
}


/*                                                                            */
/* 
                                                                       
 
                                                       
                                  
 
                                
                                                       
*/
/*                                                                            */
BOOLEAN secRxPortControlCheck(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSWRfb)
{
	ASSERT(prSWRfb);

#if 0
	/*                                  */
	if (prSWRfb->prStaRec) {
		if (prSWRfb->prStaRec->fgPortBlock == TRUE) {
			if (1 /*                                   */  &&
			    (g_prWifiVar->rConnSettings.eAuthMode >= AUTH_MODE_WPA)) {
				/*                                                             */
				return TRUE;	/*                   */
			}
			/*                                                   */
			/*                                                              */
			/*               */
			/*   */
		}
	} else {
	}
#endif
	return TRUE;
}

/*                                                                            */
/* 
                                                          
 
                                                               
                                                      
 
              
*/
/*                                                                            */
VOID secSetCipherSuite(IN P_ADAPTER_T prAdapter, IN UINT_32 u4CipherSuitesFlags)
{
	UINT_32 i;
	P_DOT11_RSNA_CONFIG_PAIRWISE_CIPHERS_ENTRY prEntry;
	P_IEEE_802_11_MIB_T prMib;

	ASSERT(prAdapter);

	prMib = &prAdapter->rMib;

	ASSERT(prMib);

	if (u4CipherSuitesFlags == CIPHER_FLAG_NONE) {
		/*                                         */
		for (i = 0; i < MAX_NUM_SUPPORTED_CIPHER_SUITES; i++) {
			prMib->dot11RSNAConfigPairwiseCiphersTable[i].
			    dot11RSNAConfigPairwiseCipherEnabled = FALSE;
		}

		/*                                */
		prMib->dot11RSNAConfigGroupCipher = WPA_CIPHER_SUITE_NONE;

		return;
	}

	for (i = 0; i < MAX_NUM_SUPPORTED_CIPHER_SUITES; i++) {
		prEntry = &prMib->dot11RSNAConfigPairwiseCiphersTable[i];

		switch (prEntry->dot11RSNAConfigPairwiseCipher) {
		case WPA_CIPHER_SUITE_WEP40:
		case RSN_CIPHER_SUITE_WEP40:
			if (u4CipherSuitesFlags & CIPHER_FLAG_WEP40) {
				prEntry->dot11RSNAConfigPairwiseCipherEnabled = TRUE;
			} else {
				prEntry->dot11RSNAConfigPairwiseCipherEnabled = FALSE;
			}
			break;

		case WPA_CIPHER_SUITE_TKIP:
		case RSN_CIPHER_SUITE_TKIP:
			if (u4CipherSuitesFlags & CIPHER_FLAG_TKIP) {
				prEntry->dot11RSNAConfigPairwiseCipherEnabled = TRUE;
			} else {
				prEntry->dot11RSNAConfigPairwiseCipherEnabled = FALSE;
			}
			break;

		case WPA_CIPHER_SUITE_CCMP:
		case RSN_CIPHER_SUITE_CCMP:
			if (u4CipherSuitesFlags & CIPHER_FLAG_CCMP) {
				prEntry->dot11RSNAConfigPairwiseCipherEnabled = TRUE;
			} else {
				prEntry->dot11RSNAConfigPairwiseCipherEnabled = FALSE;
			}
			break;

		case WPA_CIPHER_SUITE_WEP104:
		case RSN_CIPHER_SUITE_WEP104:
			if (u4CipherSuitesFlags & CIPHER_FLAG_WEP104) {
				prEntry->dot11RSNAConfigPairwiseCipherEnabled = TRUE;
			} else {
				prEntry->dot11RSNAConfigPairwiseCipherEnabled = FALSE;
			}
			break;
		default:
			break;
		}
	}

	/*                                */
	if (rsnSearchSupportedCipher(prAdapter, WPA_CIPHER_SUITE_CCMP, &i)) {
		prMib->dot11RSNAConfigGroupCipher = WPA_CIPHER_SUITE_CCMP;
	} else if (rsnSearchSupportedCipher(prAdapter, WPA_CIPHER_SUITE_TKIP, &i)) {
		prMib->dot11RSNAConfigGroupCipher = WPA_CIPHER_SUITE_TKIP;
	} else if (rsnSearchSupportedCipher(prAdapter, WPA_CIPHER_SUITE_WEP104, &i)) {
		prMib->dot11RSNAConfigGroupCipher = WPA_CIPHER_SUITE_WEP104;
	} else if (rsnSearchSupportedCipher(prAdapter, WPA_CIPHER_SUITE_WEP40, &i)) {
		prMib->dot11RSNAConfigGroupCipher = WPA_CIPHER_SUITE_WEP40;
	} else {
		prMib->dot11RSNAConfigGroupCipher = WPA_CIPHER_SUITE_NONE;
	}

}				/*                   */


/*                                                                            */
/* 
                                                                 
 
                                                       
                                                          
                                             
                                               
 
                                
                                                       
 
*/
/*                                                                            */
BOOL
secProcessEAPOL(IN P_ADAPTER_T prAdapter,
		IN P_MSDU_INFO_T prMsduInfo,
		IN P_STA_RECORD_T prStaRec, IN PUINT_8 pucPayload, IN UINT_16 u2PayloadLen)
{
	P_EAPOL_KEY prEapol = (P_EAPOL_KEY) NULL;
	P_IEEE_802_1X_HDR pr1xHdr;
	UINT_16 u2KeyInfo;

	ASSERT(prMsduInfo);
	ASSERT(prStaRec);

	/*                                                      */
	ASSERT(prStaRec);

	if (prStaRec && IS_AP_STA(prStaRec)) {
		pr1xHdr = (P_IEEE_802_1X_HDR) pucPayload;
		if ((pr1xHdr->ucType == 3) /*           */ && ((u2PayloadLen - 4) >
							      sizeof(EAPOL_KEY))) {
			prEapol = (P_EAPOL_KEY) ((PUINT_32) (pucPayload + 4));
			WLAN_GET_FIELD_BE16(prEapol->aucKeyInfo, &u2KeyInfo);
			if ((prEapol->ucType == 254) && (u2KeyInfo & MASK_2ND_EAPOL)) {
				if (u2KeyInfo & WPA_KEY_INFO_SECURE) {
					/*                                              */
					/*                                                 */
				} else if (u2PayloadLen == 123 /*                 */) {
					DBGLOG(RSN, INFO, ("Tx 2nd EAPoL frame\r\n"));
					secFsmEvent2ndEapolTx(prAdapter, prStaRec);
				}
			}
		}
	}

	return TRUE;
}


/*                                                                            */
/* 
                                                                                    
 
                                                                  
                                                          
                                                      
 
           
*/
/*                                                                            */
VOID
secHandleTxDoneCallback(IN P_ADAPTER_T prAdapter,
			IN P_MSDU_INFO_T prMsduInfo,
			IN P_STA_RECORD_T prStaRec, IN WLAN_STATUS rStatus)
{
	PUINT_8 pucPayload;
	P_IEEE_802_1X_HDR pr1xHdr = (P_IEEE_802_1X_HDR) NULL;
	P_EAPOL_KEY prEapol = (P_EAPOL_KEY) NULL;
	UINT_16 u2KeyInfo;
	UINT_16 u2PayloadLen;

	DEBUGFUNC("secHandleTxDoneCallback");

	ASSERT(prMsduInfo);
	/*                                                                                                         */

	ASSERT(prStaRec);

	/*                                                                                                  */
	/*                              */
#if 1
	pucPayload = (PUINT_8) prMsduInfo->prPacket;
	ASSERT(pucPayload);

	u2PayloadLen = prMsduInfo->u2FrameLength;

	if (0 /*                         */) {

		if (prStaRec && IS_AP_STA(prStaRec)) {
			pr1xHdr = (P_IEEE_802_1X_HDR) (PUINT_32) (pucPayload + 8);
			if ((pr1xHdr->ucType == 3) /*           */ && ((u2PayloadLen - 4) >
								      sizeof(EAPOL_KEY))) {
				prEapol = (P_EAPOL_KEY) (PUINT_32) (pucPayload + 12);
				WLAN_GET_FIELD_BE16(prEapol->aucKeyInfo, &u2KeyInfo);
				if ((prEapol->ucType == 254) && (u2KeyInfo & MASK_2ND_EAPOL)) {
					if (prStaRec->rSecInfo.fg2nd1xSend == TRUE
					    && u2PayloadLen ==
					    107
					    /*             *//*                                 */)
					{
						DBGLOG(RSN, INFO, ("Tx 4th EAPoL frame\r\n"));
						secFsmEvent4ndEapolTxDone(prAdapter, prStaRec);
					} else if (prAdapter->rWifiVar.rAisSpecificBssInfo.
						   fgCheckEAPoLTxDone) {
						DBGLOG(RSN, INFO,
						       ("Tx EAPoL Error report frame\r\n"));
						/*                                                                   */
					}
				}
			}
		}

	}
#endif
}

/*                                                                            */
/* 
                                                                   
 
                                                       
 
              
*/
/*                                                                            */
VOID secClearPmkid(IN P_ADAPTER_T prAdapter)
{
	P_AIS_SPECIFIC_BSS_INFO_T prAisSpecBssInfo;

	DEBUGFUNC("secClearPmkid");

	prAisSpecBssInfo = &prAdapter->rWifiVar.rAisSpecificBssInfo;
	DBGLOG(RSN, TRACE, ("secClearPmkid\n"));
	prAisSpecBssInfo->u4PmkidCandicateCount = 0;
	prAisSpecBssInfo->u4PmkidCacheCount = 0;
	kalMemZero((PVOID) prAisSpecBssInfo->arPmkidCandicate,
		   sizeof(PMKID_CANDICATE_T) * CFG_MAX_PMKID_CACHE);
	kalMemZero((PVOID) prAisSpecBssInfo->arPmkidCache,
		   sizeof(PMKID_ENTRY_T) * CFG_MAX_PMKID_CACHE);
}


/*                                                                            */
/* 
                                                          
 
                                                       
 
                 
*/
/*                                                                            */
BOOLEAN secRsnKeyHandshakeEnabled(IN P_ADAPTER_T prAdapter)
{
	P_CONNECTION_SETTINGS_T prConnSettings;

	ASSERT(prAdapter);

	prConnSettings = &prAdapter->rWifiVar.rConnSettings;

	ASSERT(prConnSettings);

	ASSERT(prConnSettings->eEncStatus < ENUM_ENCRYPTION3_KEY_ABSENT);

	if (prConnSettings->eEncStatus == ENUM_ENCRYPTION_DISABLED) {
		return FALSE;
	}

	ASSERT(prConnSettings->eAuthMode < AUTH_MODE_NUM);
	if ((prConnSettings->eAuthMode >= AUTH_MODE_WPA) &&
	    (prConnSettings->eAuthMode != AUTH_MODE_WPA_NONE)) {
		return TRUE;
	}

	return FALSE;
}				/*                           */


/*                                                                            */
/* 
                                                          
 
                                                       
                                         
 
                                                    
                                                         
 
        
*/
/*                                                                            */
BOOLEAN secTransmitKeyExist(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prSta)
{
	ASSERT(prSta);

	if (prSta->fgTransmitKeyExist) {
		return TRUE;
	} else {
		return FALSE;
	}
}				/*                     */


/*                                                                            */
/* 
                                           
 
                                                       
 
                 
*/
/*                                                                            */
BOOLEAN secEnabledInAis(IN P_ADAPTER_T prAdapter)
{
	DEBUGFUNC("secEnabled");

	ASSERT(prAdapter->rWifiVar.rConnSettings.eEncStatus < ENUM_ENCRYPTION3_KEY_ABSENT);

	switch (prAdapter->rWifiVar.rConnSettings.eEncStatus) {
	case ENUM_ENCRYPTION_DISABLED:
		return FALSE;
	case ENUM_ENCRYPTION1_ENABLED:
	case ENUM_ENCRYPTION2_ENABLED:
	case ENUM_ENCRYPTION3_ENABLED:
		return TRUE;
	default:
		DBGLOG(RSN, TRACE, ("Unknown encryption setting %d\n",
				    prAdapter->rWifiVar.rConnSettings.eEncStatus));
		break;
	}
	return FALSE;
}				/*            */


/*                                                                            */
/* 
                                                                            
 
                                                       
                                                     
 
                                      
                                             
*/
/*                                                                            */
BOOLEAN
secIsProtectedFrame(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsdu, IN P_STA_RECORD_T prStaRec)
{
	ASSERT(prAdapter);

	ASSERT(prMsdu);

	ASSERT(prStaRec);
	/*                                                  */

	if (prStaRec == NULL) {
		if (prAdapter->rWifiVar.rAisSpecificBssInfo.fgTransmitKeyExist)
			return TRUE;
		return FALSE;	/*                */
	}

	/*        */
	if (0 /*                     */) {
		if (IS_STA_IN_AIS(prStaRec) &&
		    prAdapter->rWifiVar.rConnSettings.eAuthMode < AUTH_MODE_WPA) {
			DBGLOG(RSN, LOUD, ("For AIS Legacy 1x, always not encryped\n"));
			return FALSE;
		} else if (!prStaRec->fgTransmitKeyExist) {
			DBGLOG(RSN, LOUD, ("1x Not Protected.\n"));
			return FALSE;
		} else if (prStaRec->rSecInfo.fgKeyStored) {
			DBGLOG(RSN, LOUD, ("1x not Protected due key stored!\n"));
			return FALSE;
		} else {
			DBGLOG(RSN, LOUD, ("1x Protected.\n"));
			return TRUE;
		}
	} else {
		if (!prStaRec->fgTransmitKeyExist) {
			/*                           */
			if (prAdapter->rWifiVar.rConnSettings.eAuthMode < AUTH_MODE_WPA &&
			    prAdapter->rWifiVar.rAisSpecificBssInfo.fgTransmitKeyExist) {
				DBGLOG(RSN, LOUD, ("Protected\n"));
				return TRUE;
			}
		} else {
			DBGLOG(RSN, LOUD, ("Protected.\n"));
			return TRUE;
		}
	}

	/*                             */
	return FALSE;
}
#endif
