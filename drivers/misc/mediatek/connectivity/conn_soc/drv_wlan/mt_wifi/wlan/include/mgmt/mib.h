/*
                                                                                    
*/

/*              
                                                                           
                                                     
*/



/*
                
  
                   
                                                                                        
                                          
  
                   
   
                         
  
                   
  
                                                                               
  
                   
                                                    
                                                    
  
                         
                                                           
                                                                             
  
                       
                                                                
  
*/

#ifndef _MIB_H
#define _MIB_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
/*                                                                                 */
typedef struct _DOT11_AUTHENTICATION_ALGORITHMS_ENTRY {
    BOOLEAN     dot11AuthenticationAlgorithmsEnable;    /*                                      */
} DOT11_AUTHENTICATION_ALGORITHMS_ENTRY, *P_DOT11_AUTHENTICATION_ALGORITHMS_ENTRY;

/*                                                                                             */
typedef struct _DOT11_RSNA_CONFIG_PAIRWISE_CIPHERS_ENTRY
{
    UINT_32     dot11RSNAConfigPairwiseCipher;          /*                                       */
    BOOLEAN     dot11RSNAConfigPairwiseCipherEnabled;   /*                                       */
} DOT11_RSNA_CONFIG_PAIRWISE_CIPHERS_ENTRY, *P_DOT11_RSNA_CONFIG_PAIRWISE_CIPHERS_ENTRY;

/*                                                                                                       */
typedef struct _DOT11_RSNA_CONFIG_AUTHENTICATION_SUITES_ENTRY
{
    UINT_32     dot11RSNAConfigAuthenticationSuite;         /*                                            */
    BOOLEAN     dot11RSNAConfigAuthenticationSuiteEnabled;  /*                                            */
} DOT11_RSNA_CONFIG_AUTHENTICATION_SUITES_ENTRY, *P_DOT11_RSNA_CONFIG_AUTHENTICATION_SUITES_ENTRY;

/*                                            */
typedef struct _IEEE_802_11_MIB_T {
    /*                                                           */
    UINT_8      dot11WEPDefaultKeyID;                   /*                     */
    BOOLEAN     dot11TranmitKeyAvailable;
    UINT_32     dot11WEPICVErrorCount;                  /*                     */
    UINT_32     dot11WEPExcludedCount;                  /*                     */

    /*                                                           */
    UINT_32     dot11RSNAConfigGroupCipher;             /*                        */

    /*                                                           */
    DOT11_RSNA_CONFIG_PAIRWISE_CIPHERS_ENTRY dot11RSNAConfigPairwiseCiphersTable[MAX_NUM_SUPPORTED_CIPHER_SUITES];

    /*                                                            */
    DOT11_RSNA_CONFIG_AUTHENTICATION_SUITES_ENTRY dot11RSNAConfigAuthenticationSuitesTable[MAX_NUM_SUPPORTED_AKM_SUITES];

#if 0 //            
    BOOLEAN            fgWapiKeyInstalled;
    PARAM_WPI_KEY_T    rWapiPairwiseKey[2];
    BOOLEAN            fgPairwiseKeyUsed[2];
    UINT_8             ucWpiActivedPWKey; /*                              */
    PARAM_WPI_KEY_T    rWapiGroupKey[2];
    BOOLEAN            fgGroupKeyUsed[2];
#endif
} IEEE_802_11_MIB_T, *P_IEEE_802_11_MIB_T;

/*                                                                            */
typedef const struct _NON_HT_PHY_ATTRIBUTE_T {
    UINT_16 u2SupportedRateSet;

    BOOLEAN fgIsShortPreambleOptionImplemented;

    BOOLEAN fgIsShortSlotTimeOptionImplemented;

} NON_HT_PHY_ATTRIBUTE_T, *P_NON_HT_PHY_ATTRIBUTE_T;

typedef const struct _NON_HT_ADHOC_MODE_ATTRIBUTE_T {

    ENUM_PHY_TYPE_INDEX_T ePhyTypeIndex;

    UINT_16 u2BSSBasicRateSet;

} NON_HT_ADHOC_MODE_ATTRIBUTE_T, *P_NON_HT_ADHOC_MODE_ATTRIBUTE_T;

typedef NON_HT_ADHOC_MODE_ATTRIBUTE_T NON_HT_AP_MODE_ATTRIBUTE_T;

/*                                                                              
                                                  
                                                                                
*/
extern NON_HT_PHY_ATTRIBUTE_T rNonHTPhyAttributes[];
extern NON_HT_ADHOC_MODE_ATTRIBUTE_T rNonHTAdHocModeAttributes[];
extern NON_HT_AP_MODE_ATTRIBUTE_T rNonHTApModeAttributes[];

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

#endif /*        */

