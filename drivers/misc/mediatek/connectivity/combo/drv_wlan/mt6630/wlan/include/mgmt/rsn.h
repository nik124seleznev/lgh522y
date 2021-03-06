/*
                                                                                    
*/

/*               
                                                                               
*/



/*
                
  
                   
                                                                             
                                            
  
                   
                                                                             
                                                   
                                                  
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                             
                                                   
  
                   
                                                                                                                                       
                                                                    
                               
  
                           
                                                                                
    
  
                   
                                                                               
                                                          
  
                   
                                                                                             
                                       
  
                   
                                                                                     
                                     
  
                   
       
                                                
  
                   
       
                        
  
                   
       
                                                         
  
                   
  
                                                                               
  
                   
                                                
                                  
  
                   
                                                                                            
                                                                                                                            
  
                   
                                                                          
                                        
  
                      
                                                                           
                                                        
  
                      
                                                                           
                                      
  
                      
                                                                           
                                         
  
                       
                                                                           
                                                      
  
                       
                                                                           
                                                   
  
                       
                                                                           
                             
  
                       
                                                                           
  
  
*/

#ifndef _RSN_H
#define _RSN_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
/*                                                    */
#define RSN_CIPHER_SUITE_USE_GROUP_KEY  0x00AC0F00
#define RSN_CIPHER_SUITE_WEP40          0x01AC0F00
#define RSN_CIPHER_SUITE_TKIP           0x02AC0F00
#define RSN_CIPHER_SUITE_CCMP           0x04AC0F00
#define RSN_CIPHER_SUITE_WEP104         0x05AC0F00
#if CFG_SUPPORT_802_11W
#define RSN_CIPHER_SUITE_AES_128_CMAC   0x06AC0F00
#endif

#define WPA_CIPHER_SUITE_NONE           0x00F25000
#define WPA_CIPHER_SUITE_WEP40          0x01F25000
#define WPA_CIPHER_SUITE_TKIP           0x02F25000
#define WPA_CIPHER_SUITE_CCMP           0x04F25000
#define WPA_CIPHER_SUITE_WEP104         0x05F25000

/*                                                                               */
#define RSN_AKM_SUITE_NONE              0x00AC0F00
#define RSN_AKM_SUITE_802_1X            0x01AC0F00
#define RSN_AKM_SUITE_PSK               0x02AC0F00
#if CFG_SUPPORT_802_11W
#define RSN_AKM_SUITE_802_1X_SHA256     0x05AC0F00
#define RSN_AKM_SUITE_PSK_SHA256        0x06AC0F00
#endif

#define WPA_AKM_SUITE_NONE              0x00F25000
#define WPA_AKM_SUITE_802_1X            0x01F25000
#define WPA_AKM_SUITE_PSK               0x02F25000

#define ELEM_ID_RSN_LEN_FIXED           20	/*                                      */

#define ELEM_ID_WPA_LEN_FIXED           22	/*                                      */

#define MASK_RSNIE_CAP_PREAUTH          BIT(0)

#define GET_SELECTOR_TYPE(x)           ((UINT_8)(((x) >> 24) & 0x000000FF))
#define SET_SELECTOR_TYPE(x, y)         x = (((x) & 0x00FFFFFF) | (((UINT_32)(y) << 24) & 0xFF000000))

#define AUTH_CIPHER_CCMP                0x00000008

/*                     */
#define CIPHER_FLAG_NONE                        0x00000000
#define CIPHER_FLAG_WEP40                       0x00000001	/*       */
#define CIPHER_FLAG_TKIP                        0x00000002	/*       */
#define CIPHER_FLAG_CCMP                        0x00000008	/*       */
#define CIPHER_FLAG_WEP104                      0x00000010	/*       */
#define CIPHER_FLAG_WEP128                      0x00000020	/*       */

#define WAIT_TIME_IND_PMKID_CANDICATE_SEC       6	/*         */
#define TKIP_COUNTERMEASURE_SEC                 60	/*         */

#if CFG_SUPPORT_802_11W
#define RSN_AUTH_MFP_DISABLED   0	/*              */
#define RSN_AUTH_MFP_OPTIONAL   1	/*              */
#define RSN_AUTH_MFP_REQUIRED   2	/*              */
#endif

/*                                                                              
                                                 
                                                                                
*/

/*                                          */
#define EVENT_PMKID_CANDIDATE_PREAUTH_ENABLED   0x01


#define CONTROL_FLAG_UC_MGMT_NO_ENC             BIT(5)

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define RSN_IE(fp)              ((P_RSN_INFO_ELEM_T) fp)
#define WPA_IE(fp)              ((P_WPA_INFO_ELEM_T) fp)

#define ELEM_MAX_LEN_ASSOC_RSP_WSC_IE          (32 - ELEM_HDR_LEN)


/*                                                                              
                                                            
                                                                                
*/
BOOLEAN
rsnParseRsnIE(IN P_ADAPTER_T prAdapter,
	      IN P_RSN_INFO_ELEM_T prInfoElem, OUT P_RSN_INFO_T prRsnInfo);

BOOLEAN
rsnParseWpaIE(IN P_ADAPTER_T prAdapter,
	      IN P_WPA_INFO_ELEM_T prInfoElem, OUT P_RSN_INFO_T prWpaInfo);

BOOLEAN
rsnSearchSupportedCipher(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Cipher, OUT PUINT_32 pu4Index);

BOOLEAN rsnIsSuitableBSS(IN P_ADAPTER_T prAdapter, IN P_RSN_INFO_T prBssRsnInfo);

BOOLEAN rsnSearchAKMSuite(IN P_ADAPTER_T prAdapter, IN UINT_32 u4AkmSuite, OUT PUINT_32 pu4Index);

BOOLEAN rsnPerformPolicySelection(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBss);

VOID rsnGenerateWpaNoneIE(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);

VOID rsnGenerateWPAIE(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);

VOID rsnGenerateRSNIE(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);

BOOLEAN
rsnParseCheckForWFAInfoElem(IN P_ADAPTER_T prAdapter,
			    IN PUINT_8 pucBuf,
			    OUT PUINT_8 pucOuiType, OUT PUINT_16 pu2SubTypeVersion);

#if CFG_SUPPORT_AAA
void
rsnParserCheckForRSNCCMPPSK(P_ADAPTER_T prAdapter, P_RSN_INFO_ELEM_T prIe, PUINT_16 pu2StatusCode);
#endif

VOID
rsnTkipHandleMICFailure(IN P_ADAPTER_T prAdapter,
			IN P_STA_RECORD_T prSta, IN BOOLEAN fgErrorKeyType);

VOID rsnSelectPmkidCandidateList(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBssDesc);

VOID rsnUpdatePmkidCandidateList(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBssDesc);

BOOLEAN
rsnSearchPmkidEntry(IN P_ADAPTER_T prAdapter, IN PUINT_8 pucBssid, OUT PUINT_32 pu4EntryIndex);

BOOLEAN rsnCheckPmkidCandicate(IN P_ADAPTER_T prAdapter);

VOID rsnCheckPmkidCache(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBss);

VOID rsnGeneratePmkidIndication(IN P_ADAPTER_T prAdapter);

VOID rsnIndicatePmkidCand(IN P_ADAPTER_T prAdapter, IN ULONG ulParamPtr);
#if CFG_SUPPORT_WPS2
VOID rsnGenerateWSCIE(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);
#endif

#if CFG_SUPPORT_802_11W
UINT_32 rsnCheckBipKeyInstalled(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

UINT_8 rsnCheckSaQueryTimeout(IN P_ADAPTER_T prAdapter);

void rsnStartSaQueryTimer(IN P_ADAPTER_T prAdapter, IN ULONG ulParamPtr);

void rsnStartSaQuery(IN P_ADAPTER_T prAdapter);

void rsnStopSaQuery(IN P_ADAPTER_T prAdapter);

void rsnSaQueryRequest(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

void rsnSaQueryAction(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);
#endif

#if CFG_SUPPORT_AAA
VOID rsnGenerateWSCIEForAssocRsp(P_ADAPTER_T prAdapter, P_MSDU_INFO_T prMsduInfo);
#endif

/*                                                                              
                                                
                                                                                
*/

#endif				/*        */
