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
#if CFG_SUPPORT_WAPI

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
VOID wapiGenerateWAPIIE(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo)
{
	PUINT_8 pucBuffer;

	ASSERT(prAdapter);
	ASSERT(prMsduInfo);

	if (prMsduInfo->ucNetworkType != NETWORK_TYPE_AIS_INDEX)
		return;

	pucBuffer = (PUINT_8) ((UINT_32) prMsduInfo->prPacket +
			       (UINT_32) prMsduInfo->u2FrameLength);

	/*                            */
	if (/*                           */ prAdapter->prGlueInfo->u2WapiAssocInfoIESz) {
		kalMemCopy(pucBuffer, &prAdapter->prGlueInfo->aucWapiAssocInfoIEs,
			   prAdapter->prGlueInfo->u2WapiAssocInfoIESz);
		prMsduInfo->u2FrameLength += prAdapter->prGlueInfo->u2WapiAssocInfoIESz;
	}

}


/*                                                                            */
/* 
                                                 
 
                                              
                                                                            
                                                           
 
                          
                        
*/
/*                                                                            */
BOOLEAN wapiParseWapiIE(IN P_WAPI_INFO_ELEM_T prInfoElem, OUT P_WAPI_INFO_T prWapiInfo)
{
	UINT_32 i;
	INT_32 u4RemainWapiIeLen;
	UINT_16 u2Version;
	UINT_16 u2Cap = 0;
	UINT_32 u4GroupSuite = WAPI_CIPHER_SUITE_WPI;
	UINT_16 u2PairSuiteCount = 0;
	UINT_16 u2AuthSuiteCount = 0;
	PUCHAR pucPairSuite = NULL;
	PUCHAR pucAuthSuite = NULL;
	PUCHAR cp;

	DEBUGFUNC("wapiParseWapiIE");

	ASSERT(prInfoElem);
	ASSERT(prWapiInfo);

	/*                                   */
	if (prInfoElem->ucLength < 6) {
		DBGLOG(SEC, TRACE,
		       ("WAPI IE length too short (length=%d)\n", prInfoElem->ucLength));
		return FALSE;
	}

	/*                                                           */
	WLAN_GET_FIELD_16(&prInfoElem->u2Version, &u2Version);
	if (u2Version != 1) {
		DBGLOG(SEC, TRACE, ("Unsupported WAPI IE version: %d\n", u2Version));
		return FALSE;
	}

	cp = (PUCHAR) &prInfoElem->u2AuthKeyMgtSuiteCount;
	u4RemainWapiIeLen = (INT_32) prInfoElem->ucLength - 2;

	do {
		if (u4RemainWapiIeLen == 0) {
			break;
		}

		/*
                     
                                      
                     
                                      
                     
                      */

		/*                                                               
            */
		if (u4RemainWapiIeLen < 2) {
			DBGLOG(SEC, TRACE,
			       ("Fail to parse WAPI IE in auth & key mgt suite count (IE len: %d)\n",
				prInfoElem->ucLength));
			return FALSE;
		}

		WLAN_GET_FIELD_16(cp, &u2AuthSuiteCount);
		cp += 2;
		u4RemainWapiIeLen -= 2;

		/*                                                              
            */
		i = (UINT_32) u2AuthSuiteCount * 4;
		if (u4RemainWapiIeLen < (INT_32) i) {
			DBGLOG(SEC, TRACE,
			       ("Fail to parse WAPI IE in auth & key mgt suite list (IE len: %d)\n",
				prInfoElem->ucLength));
			return FALSE;
		}

		pucAuthSuite = cp;

		cp += i;
		u4RemainWapiIeLen -= (INT_32) i;

		if (u4RemainWapiIeLen == 0) {
			break;
		}

		/*                                                  */
		if (u4RemainWapiIeLen < 2) {
			DBGLOG(SEC, TRACE,
			       ("Fail to parse WAPI IE in pairwise cipher suite count (IE len: %d)\n",
				prInfoElem->ucLength));
			return FALSE;
		}

		WLAN_GET_FIELD_16(cp, &u2PairSuiteCount);
		cp += 2;
		u4RemainWapiIeLen -= 2;

		/*                                                 */
		i = (UINT_32) u2PairSuiteCount * 4;
		if (u4RemainWapiIeLen < (INT_32) i) {
			DBGLOG(SEC, TRACE,
			       ("Fail to parse WAPI IE in pairwise cipher suite list (IE len: %d)\n",
				prInfoElem->ucLength));
			return FALSE;
		}

		pucPairSuite = cp;

		cp += i;
		u4RemainWapiIeLen -= (INT_32) i;

		/*                                         */
		if (u4RemainWapiIeLen < 4) {
			DBGLOG(SEC, TRACE,
			       ("Fail to parse WAPI IE in group cipher suite (IE len: %d)\n",
				prInfoElem->ucLength));
			return FALSE;
		}

		WLAN_GET_FIELD_32(cp, &u4GroupSuite);
		cp += 4;
		u4RemainWapiIeLen -= 4;

		/*                                      */
		if (u4RemainWapiIeLen < 2) {
			DBGLOG(SEC, TRACE,
			       ("Fail to parse WAPI IE in WAPI capabilities (IE len: %d)\n",
				prInfoElem->ucLength));
			return FALSE;
		}

		WLAN_GET_FIELD_16(cp, &u2Cap);
		u4RemainWapiIeLen -= 2;

		/*                     */
	} while (FALSE);

	/*                                        */

	prWapiInfo->ucElemId = ELEM_ID_WAPI;

	prWapiInfo->u2Version = u2Version;

	prWapiInfo->u4GroupKeyCipherSuite = u4GroupSuite;

	DBGLOG(SEC, LOUD, ("WAPI: version %d, group key cipher suite %02x-%02x-%02x-%02x\n",
			   u2Version, (UCHAR) (u4GroupSuite & 0x000000FF),
			   (UCHAR) ((u4GroupSuite >> 8) & 0x000000FF),
			   (UCHAR) ((u4GroupSuite >> 16) & 0x000000FF),
			   (UCHAR) ((u4GroupSuite >> 24) & 0x000000FF)));

	if (pucPairSuite) {
		/*                                                                  */
		if (u2PairSuiteCount > MAX_NUM_SUPPORTED_WAPI_CIPHER_SUITES) {
			u2PairSuiteCount = MAX_NUM_SUPPORTED_WAPI_CIPHER_SUITES;
		}

		prWapiInfo->u4PairwiseKeyCipherSuiteCount = (UINT_32) u2PairSuiteCount;

		for (i = 0; i < (UINT_32) u2PairSuiteCount; i++) {
			WLAN_GET_FIELD_32(pucPairSuite, &prWapiInfo->au4PairwiseKeyCipherSuite[i]);
			pucPairSuite += 4;

			DBGLOG(SEC, LOUD,
			       ("WAPI: pairwise key cipher suite [%d]: %02x-%02x-%02x-%02x\n",
				(UINT_8) i,
				(UCHAR) (prWapiInfo->au4PairwiseKeyCipherSuite[i] & 0x000000FF),
				(UCHAR) ((prWapiInfo->
					  au4PairwiseKeyCipherSuite[i] >> 8) & 0x000000FF),
				(UCHAR) ((prWapiInfo->
					  au4PairwiseKeyCipherSuite[i] >> 16) & 0x000000FF),
				(UCHAR) ((prWapiInfo->
					  au4PairwiseKeyCipherSuite[i] >> 24) & 0x000000FF)));
		}
	} else {
		/*                                                                     
                                                  */
		prWapiInfo->u4PairwiseKeyCipherSuiteCount = 1;
		prWapiInfo->au4PairwiseKeyCipherSuite[0] = WAPI_CIPHER_SUITE_WPI;

		DBGLOG(SEC, LOUD,
		       ("WAPI: pairwise key cipher suite: %02x-%02x-%02x-%02x (default)\n",
			(UCHAR) (prWapiInfo->au4PairwiseKeyCipherSuite[0] & 0x000000FF),
			(UCHAR) ((prWapiInfo->au4PairwiseKeyCipherSuite[0] >> 8) & 0x000000FF),
			(UCHAR) ((prWapiInfo->au4PairwiseKeyCipherSuite[0] >> 16) & 0x000000FF),
			(UCHAR) ((prWapiInfo->au4PairwiseKeyCipherSuite[0] >> 24) & 0x000000FF)));
	}

	if (pucAuthSuite) {
		/*                                                                   
                 */
		if (u2AuthSuiteCount > MAX_NUM_SUPPORTED_WAPI_AKM_SUITES) {
			u2AuthSuiteCount = MAX_NUM_SUPPORTED_WAPI_AKM_SUITES;
		}

		prWapiInfo->u4AuthKeyMgtSuiteCount = (UINT_32) u2AuthSuiteCount;

		for (i = 0; i < (UINT_32) u2AuthSuiteCount; i++) {
			WLAN_GET_FIELD_32(pucAuthSuite, &prWapiInfo->au4AuthKeyMgtSuite[i]);
			pucAuthSuite += 4;

			DBGLOG(SEC, LOUD, ("WAPI: AKM suite [%d]: %02x-%02x-%02x-%02x\n",
					   (UINT_8) i,
					   (UCHAR) (prWapiInfo->au4AuthKeyMgtSuite[i] & 0x000000FF),
					   (UCHAR) ((prWapiInfo->
						     au4AuthKeyMgtSuite[i] >> 8) & 0x000000FF),
					   (UCHAR) ((prWapiInfo->
						     au4AuthKeyMgtSuite[i] >> 16) & 0x000000FF),
					   (UCHAR) ((prWapiInfo->
						     au4AuthKeyMgtSuite[i] >> 24) & 0x000000FF)));
		}
	} else {
		/*                                                                   
                                                         */
		prWapiInfo->u4AuthKeyMgtSuiteCount = 1;
		prWapiInfo->au4AuthKeyMgtSuite[0] = WAPI_AKM_SUITE_802_1X;

		DBGLOG(SEC, LOUD, ("WAPI: AKM suite: %02x-%02x-%02x-%02x (default)\n",
				   (UCHAR) (prWapiInfo->au4AuthKeyMgtSuite[0] & 0x000000FF),
				   (UCHAR) ((prWapiInfo->au4AuthKeyMgtSuite[0] >> 8) & 0x000000FF),
				   (UCHAR) ((prWapiInfo->au4AuthKeyMgtSuite[0] >> 16) & 0x000000FF),
				   (UCHAR) ((prWapiInfo->
					     au4AuthKeyMgtSuite[0] >> 24) & 0x000000FF)));
	}

	prWapiInfo->u2WapiCap = u2Cap;
	DBGLOG(SEC, LOUD, ("WAPI: cap: 0x%04x\n", prWapiInfo->u2WapiCap));

	return TRUE;
}				/*                 */


/*                                                                            */
/* 
                                                                                 
 
                                                                
                                                  
 
                                                               
                                                                          
                                                     
                                                                        
                                                                     
 
                                                                              
*/
/*                                                                            */
BOOLEAN wapiPerformPolicySelection(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBss)
{
	UINT_32 i;
	UINT_32 u4PairwiseCipher = 0;
	UINT_32 u4GroupCipher = 0;
	UINT_32 u4AkmSuite = 0;
	P_WAPI_INFO_T prBssWapiInfo;
	P_WLAN_INFO_T prWlanInfo;

	DEBUGFUNC("wapiPerformPolicySelection");

	ASSERT(prBss);

	/*                                                                                          */
	prWlanInfo = &prAdapter->rWlanInfo;

	if (prBss->fgIEWAPI) {
		prBssWapiInfo = &prBss->rIEWAPI;
	} else {
		if (prAdapter->rWifiVar.rConnSettings.fgWapiMode == FALSE) {
			DBGLOG(SEC, TRACE, ("-- No Protected BSS\n"));
			return TRUE;
		} else {
			DBGLOG(SEC, TRACE, ("WAPI Information Element does not exist.\n"));
			return FALSE;
		}
	}

	/*                               */
	for (i = 0; i < prBssWapiInfo->u4PairwiseKeyCipherSuiteCount; i++) {
		if (prBssWapiInfo->au4PairwiseKeyCipherSuite[i] ==
		    prAdapter->rWifiVar.rConnSettings.u4WapiSelectedPairwiseCipher) {
			u4PairwiseCipher = prBssWapiInfo->au4PairwiseKeyCipherSuite[i];
		}
	}
	if (prBssWapiInfo->u4GroupKeyCipherSuite ==
	    prAdapter->rWifiVar.rConnSettings.u4WapiSelectedGroupCipher)
		u4GroupCipher = prBssWapiInfo->u4GroupKeyCipherSuite;

	/*                   */
	/*                                                                      
                                                */
	if (u4PairwiseCipher == 0 || u4GroupCipher == 0) {
		DBGLOG(SEC, TRACE, ("Failed to select pairwise/group cipher (0x%08lx/0x%08lx)\n",
				    u4PairwiseCipher, u4GroupCipher));
		return FALSE;
	}

	/*            */
	/*                                                                     
                                                             */
	/*                                                      */
	for (i = 0; i < prBssWapiInfo->u4AuthKeyMgtSuiteCount; i++) {
		if (prBssWapiInfo->au4AuthKeyMgtSuite[i] ==
		    prAdapter->rWifiVar.rConnSettings.u4WapiSelectedAKMSuite) {
			u4AkmSuite = prBssWapiInfo->au4AuthKeyMgtSuite[i];
			break;
		}
	}

	if (u4AkmSuite == 0) {
		DBGLOG(SEC, TRACE, ("Cannot support any AKM suites\n"));
		return FALSE;
	}

	DBGLOG(SEC, TRACE,
	       ("Selected pairwise/group cipher: %02x-%02x-%02x-%02x/%02x-%02x-%02x-%02x\n",
		(UINT_8) (u4PairwiseCipher & 0x000000FF),
		(UINT_8) ((u4PairwiseCipher >> 8) & 0x000000FF),
		(UINT_8) ((u4PairwiseCipher >> 16) & 0x000000FF),
		(UINT_8) ((u4PairwiseCipher >> 24) & 0x000000FF),
		(UINT_8) (u4GroupCipher & 0x000000FF), (UINT_8) ((u4GroupCipher >> 8) & 0x000000FF),
		(UINT_8) ((u4GroupCipher >> 16) & 0x000000FF),
		(UINT_8) ((u4GroupCipher >> 24) & 0x000000FF)));

	DBGLOG(SEC, TRACE, ("Selected AKM suite: %02x-%02x-%02x-%02x\n",
			    (UINT_8) (u4AkmSuite & 0x000000FF),
			    (UINT_8) ((u4AkmSuite >> 8) & 0x000000FF),
			    (UINT_8) ((u4AkmSuite >> 16) & 0x000000FF),
			    (UINT_8) ((u4AkmSuite >> 24) & 0x000000FF)));

	return TRUE;
}				/*                            */

#if 0
/*                                                                            */
/* 
                                                                                     
 
                                                
                                                    
 
                          
                        
*/
/*                                                                            */
BOOLEAN wapiUpdateTxKeyIdx(IN P_STA_RECORD_T prStaRec, IN UINT_8 ucWlanIdx)
{
	UINT_8 ucKeyId;

	if ((ucWlanIdx & BITS(0, 3)) == CIPHER_SUITE_WPI) {

		ucKeyId = ((ucWlanIdx & BITS(4, 5)) >> 4);

		if (ucKeyId != g_prWifiVar->rAisSpecificBssInfo.ucWpiActivedPWKey) {
			DBGLOG(RSN, STATE,
			       ("Change wapi key index from %d->%d\n",
				g_prWifiVar->rAisSpecificBssInfo.ucWpiActivedPWKey, ucKeyId));
			g_prWifiVar->rAisSpecificBssInfo.ucWpiActivedPWKey = ucKeyId;

			prStaRec->ucWTEntry =
			    (ucKeyId ==
			     WTBL_AIS_BSSID_WAPI_IDX_0) ? WTBL_AIS_BSSID_WAPI_IDX_0 :
			    WTBL_AIS_BSSID_WAPI_IDX_1;
		}
	}
}
#endif
#endif
