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
static const UINT_16 g_u2CountryGroup0[] = {
	COUNTRY_CODE_US, COUNTRY_CODE_BS, COUNTRY_CODE_BB, COUNTRY_CODE_BO, COUNTRY_CODE_DM,
	COUNTRY_CODE_DO, COUNTRY_CODE_HT, COUNTRY_CODE_PR, COUNTRY_CODE_TH, COUNTRY_CODE_TW,
	COUNTRY_CODE_AI, COUNTRY_CODE_AG, COUNTRY_CODE_AW, COUNTRY_CODE_BM, COUNTRY_CODE_CU,
	COUNTRY_CODE_GD, COUNTRY_CODE_GY, COUNTRY_CODE_JM, COUNTRY_CODE_KN, COUNTRY_CODE_LC,
	COUNTRY_CODE_VC, COUNTRY_CODE_TT, COUNTRY_CODE_SR
};

static const UINT_16 g_u2CountryGroup1[] = {
	COUNTRY_CODE_BR, COUNTRY_CODE_EC, COUNTRY_CODE_HK, COUNTRY_CODE_MX, COUNTRY_CODE_PE,
	COUNTRY_CODE_CR, COUNTRY_CODE_MD, COUNTRY_CODE_NI, COUNTRY_CODE_PZ
};
static const UINT_16 g_u2CountryGroup2[] = { COUNTRY_CODE_CO, COUNTRY_CODE_PY };
static const UINT_16 g_u2CountryGroup3[] = { COUNTRY_CODE_JP };

static const UINT_16 g_u2CountryGroup4[] = {
	COUNTRY_CODE_CN, COUNTRY_CODE_UY, COUNTRY_CODE_MA
};
static const UINT_16 g_u2CountryGroup5[] = { COUNTRY_CODE_AR };
static const UINT_16 g_u2CountryGroup6[] = { COUNTRY_CODE_AU, COUNTRY_CODE_NZ };
static const UINT_16 g_u2CountryGroup7[] = { COUNTRY_CODE_RU };

static const UINT_16 g_u2CountryGroup8[] = {
	COUNTRY_CODE_ID, COUNTRY_CODE_HN, COUNTRY_CODE_PG, COUNTRY_CODE_PK
};
static const UINT_16 g_u2CountryGroup9[] = { COUNTRY_CODE_CA };

static const UINT_16 g_u2CountryGroup10[] = {
	COUNTRY_CODE_CL, COUNTRY_CODE_IN, COUNTRY_CODE_SA, COUNTRY_CODE_SG, COUNTRY_CODE_VE,
	COUNTRY_CODE_MY, COUNTRY_CODE_VN, COUNTRY_CODE_EG
};
static const UINT_16 g_u2CountryGroup11[] = { COUNTRY_CODE_IL, COUNTRY_CODE_UA };
static const UINT_16 g_u2CountryGroup12[] = { COUNTRY_CODE_JO, COUNTRY_CODE_KW };
static const UINT_16 g_u2CountryGroup13[] = { COUNTRY_CODE_KR, COUNTRY_CODE_OM };

static const UINT_16 g_u2CountryGroup14[] = {
	COUNTRY_CODE_EU
	    /*                                                               
                                                                 
      */
	    /*                                                                     
                        */
};


DOMAIN_INFO_ENTRY arSupportedRegDomains[] = {
	{
	 (PUINT_16) g_u2CountryGroup0, sizeof(g_u2CountryGroup0) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 11, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 11, FALSE}
	  ,			/*                        */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 5, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup1, sizeof(g_u2CountryGroup1) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 11, FALSE}
	  ,			/*                        */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 5, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup2, sizeof(g_u2CountryGroup2) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 11, FALSE}
	  ,			/*                        */
	  {125, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                      */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup3, sizeof(g_u2CountryGroup3) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */
	  {82, BAND_2G4, CHNL_SPAN_5, 14, 1, FALSE}
	  ,

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 11, FALSE}
	  ,			/*                        */
	  {125, BAND_NULL, 0, 0, 0, FALSE}	/*                      */
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup4, sizeof(g_u2CountryGroup4) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                    */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 11, FALSE}
	  ,			/*                        */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 5, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup5, sizeof(g_u2CountryGroup5) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                    */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                   */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 5, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup6, sizeof(g_u2CountryGroup6) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 11, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 11, FALSE}
	  ,			/*                        */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 4, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup7, sizeof(g_u2CountryGroup7) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 11, FALSE}
	  ,			/*                        */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 4, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup8, sizeof(g_u2CountryGroup8) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                    */
	  {118, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                    */
	  {121, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                   */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 4, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup9, sizeof(g_u2CountryGroup9) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 11, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 5, FALSE}
	  ,			/*                                */
	  {121, BAND_5G, CHNL_SPAN_20, 132, 5, FALSE}
	  ,
	  {125, BAND_5G, CHNL_SPAN_20, 149, 5, FALSE}
	  ,			/*                           */
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup10, sizeof(g_u2CountryGroup10) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                   */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 5, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup11, sizeof(g_u2CountryGroup11) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                   */
	  {125, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                      */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup12, sizeof(g_u2CountryGroup12) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                    */
	  {121, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                   */
	  {125, BAND_NULL, 0, 0, 0, FALSE}
	  ,			/*                      */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 (PUINT_16) g_u2CountryGroup13, sizeof(g_u2CountryGroup13) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 8, FALSE}
	  ,			/*                        */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 5, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
	,
	{
	 /*                                              */
	 (PUINT_16) g_u2CountryGroup14, sizeof(g_u2CountryGroup14) / 2,
	 {
	  {81, BAND_2G4, CHNL_SPAN_5, 1, 13, FALSE}
	  ,			/*                 */

	  {115, BAND_5G, CHNL_SPAN_20, 36, 4, FALSE}
	  ,			/*                       */
	  {118, BAND_5G, CHNL_SPAN_20, 52, 4, FALSE}
	  ,			/*                       */
	  {121, BAND_5G, CHNL_SPAN_20, 100, 11, FALSE}
	  ,			/*                        */
	  {125, BAND_5G, CHNL_SPAN_20, 149, 7, FALSE}
	  ,			/*                           */
	  {0, BAND_NULL, 0, 0, 0, FALSE}
	  }
	 }
};

#if 0
COUNTRY_CH_SET_T arCountryChSets[] = {
	/*                                                                                           
                                  */
	{CH_SET_2G4_1_11, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_140, CH_SET_UNII_UPPER_149_165}
	,
	/*                                                 */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_140, CH_SET_UNII_UPPER_149_165}
	,
	/*                                           */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_140, CH_SET_UNII_UPPER_NA}
	,
	/*            */
	{CH_SET_2G4_1_14, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_140, CH_SET_UNII_UPPER_NA}
	,
	/*                             */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_NA, CH_SET_UNII_MID_NA,
	 CH_SET_UNII_WW_NA, CH_SET_UNII_UPPER_149_165}
	,
	/*                  */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_NA, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_NA, CH_SET_UNII_UPPER_149_165}
	,
	/*                               */
	{CH_SET_2G4_1_11, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_140, CH_SET_UNII_UPPER_149_161}
	,
	/*               */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_140, CH_SET_UNII_UPPER_149_161}
	,
	/*                  */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_NA, CH_SET_UNII_MID_NA,
	 CH_SET_UNII_WW_NA, CH_SET_UNII_UPPER_149_161}
	,
	/*               */
	{CH_SET_2G4_1_11, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_116_132_140, CH_SET_UNII_UPPER_149_165}
	,
	/*                                               */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_NA, CH_SET_UNII_UPPER_149_165}
	,
	/*                         */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_NA, CH_SET_UNII_UPPER_NA}
	,
	/*                        */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_NA,
	 CH_SET_UNII_WW_NA, CH_SET_UNII_UPPER_NA}
	,
	/*                     */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_128, CH_SET_UNII_UPPER_149_165}
	,
	/*                */
	{CH_SET_2G4_1_11, CH_SET_UNII_LOW_NA, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_140, CH_SET_UNII_UPPER_149_165}
	,
	/*                          */
	{CH_SET_2G4_1_13, CH_SET_UNII_LOW_36_48, CH_SET_UNII_MID_52_64,
	 CH_SET_UNII_WW_100_140, CH_SET_UNII_UPPER_149_173}
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
P_DOMAIN_INFO_ENTRY rlmDomainGetDomainInfo(P_ADAPTER_T prAdapter)
{
#define REG_DOMAIN_DEF_IDX          14	/*                   */
#define REG_DOMAIN_GROUP_NUM \
	(sizeof(arSupportedRegDomains) / sizeof(DOMAIN_INFO_ENTRY))

	UINT_16 i, j;
	P_DOMAIN_INFO_ENTRY prDomainInfo;
	P_REG_INFO_T prRegInfo;
	UINT_16 u2TargetCountryCode;

	ASSERT(prAdapter);

	if (prAdapter->prDomainInfo) {
		return prAdapter->prDomainInfo;
	}

	prRegInfo = &prAdapter->prGlueInfo->rRegInfo;

	DBGLOG(RLM, INFO, ("Domain: map=%d, idx=%d, code=0x%04x\n",
			   prRegInfo->eRegChannelListMap, prRegInfo->ucRegChannelListIndex,
			   prAdapter->rWifiVar.rConnSettings.u2CountryCode));

	if (prRegInfo->eRegChannelListMap == REG_CH_MAP_TBL_IDX &&
	    prRegInfo->ucRegChannelListIndex < REG_DOMAIN_GROUP_NUM) {
		prDomainInfo = &arSupportedRegDomains[prRegInfo->ucRegChannelListIndex];
		goto L_set_domain_info;
	} else if (prRegInfo->eRegChannelListMap == REG_CH_MAP_CUSTOMIZED) {
		prDomainInfo = &prRegInfo->rDomainInfo;
		goto L_set_domain_info;
	}

	u2TargetCountryCode = prAdapter->rWifiVar.rConnSettings.u2CountryCode;

	/*                                                       
                                          
  */
	for (i = 0; i < REG_DOMAIN_GROUP_NUM; i++) {
		prDomainInfo = &arSupportedRegDomains[i];

		ASSERT((prDomainInfo->u4CountryNum && prDomainInfo->pu2CountryGroup) ||
		       prDomainInfo->u4CountryNum == 0);

		for (j = 0; j < prDomainInfo->u4CountryNum; j++) {
			if (prDomainInfo->pu2CountryGroup[j] == u2TargetCountryCode) {
				break;
			}
		}
		if (j < prDomainInfo->u4CountryNum) {
			break;	/*       */
		}
	}

	DATA_STRUC_INSPECTING_ASSERT(REG_DOMAIN_DEF_IDX < REG_DOMAIN_GROUP_NUM);

	if (i >= REG_DOMAIN_GROUP_NUM) {
		prDomainInfo = &arSupportedRegDomains[REG_DOMAIN_DEF_IDX];
	}

 L_set_domain_info:

	prAdapter->prDomainInfo = prDomainInfo;
	return prDomainInfo;
}

/*                                                                            */
/* 
        
 
                                                                         
                                                                           
 
              
*/
/*                                                                            */
VOID
rlmDomainGetChnlList(P_ADAPTER_T prAdapter,
		     ENUM_BAND_T eSpecificBand,
		     UINT_8 ucMaxChannelNum,
		     PUINT_8 pucNumOfChannel, P_RF_CHANNEL_INFO_T paucChannelList)
{
	UINT_8 i, j, ucNum;
	P_DOMAIN_SUBBAND_INFO prSubband;
	P_DOMAIN_INFO_ENTRY prDomainInfo;

	ASSERT(prAdapter);
	ASSERT(paucChannelList);
	ASSERT(pucNumOfChannel);

	/*                                                        */
	prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
	ASSERT(prDomainInfo);

	ucNum = 0;
	for (i = 0; i < MAX_SUBBAND_NUM; i++) {
		prSubband = &prDomainInfo->rSubBand[i];

		if (prSubband->ucBand == BAND_NULL || prSubband->ucBand >= BAND_NUM ||
		    (prSubband->ucBand == BAND_5G && !prAdapter->fgEnable5GBand)) {
			continue;
		}

		if (eSpecificBand == BAND_NULL || prSubband->ucBand == eSpecificBand) {
			for (j = 0; j < prSubband->ucNumChannels; j++) {
				if (ucNum >= ucMaxChannelNum) {
					break;
				}
				paucChannelList[ucNum].eBand = prSubband->ucBand;
				paucChannelList[ucNum].ucChannelNum =
				    prSubband->ucFirstChannelNum + j * prSubband->ucChannelSpan;
				ucNum++;
			}
		}
	}

	*pucNumOfChannel = ucNum;
}

/*                                                                            */
/* 
        
 
            
 
                
*/
/*                                                                            */
VOID rlmDomainSendCmd(P_ADAPTER_T prAdapter, BOOLEAN fgIsOid)
{
	P_DOMAIN_INFO_ENTRY prDomainInfo;
	P_CMD_SET_DOMAIN_INFO_T prCmd;
	WLAN_STATUS rStatus;
	UINT_8 i;
	P_DOMAIN_SUBBAND_INFO prSubBand;

	prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
	ASSERT(prDomainInfo);

	prCmd = cnmMemAlloc(prAdapter, RAM_TYPE_BUF, sizeof(CMD_SET_DOMAIN_INFO_T));
	ASSERT(prCmd);

	/*                         */
	if (!prCmd) {
		DBGLOG(RLM, ERROR, ("Domain: no buf to send cmd\n"));
		return;
	}
	kalMemZero(prCmd, sizeof(CMD_SET_DOMAIN_INFO_T));

	prCmd->u2CountryCode = prAdapter->rWifiVar.rConnSettings.u2CountryCode;
	prCmd->u2Reserved = 0;
	prCmd->uc2G4Bandwidth = prAdapter->rWifiVar.rConnSettings.uc2G4BandwidthMode;
	prCmd->uc5GBandwidth = prAdapter->rWifiVar.rConnSettings.uc5GBandwidthMode;

	for (i = 0; i < 6; i++) {
		prSubBand = &prDomainInfo->rSubBand[i];

		prCmd->rSubBand[i].ucRegClass = prSubBand->ucRegClass;
		prCmd->rSubBand[i].ucBand = prSubBand->ucBand;

		if (prSubBand->ucBand != BAND_NULL && prSubBand->ucBand < BAND_NUM) {
			prCmd->rSubBand[i].ucChannelSpan = prSubBand->ucChannelSpan;
			prCmd->rSubBand[i].ucFirstChannelNum = prSubBand->ucFirstChannelNum;
			prCmd->rSubBand[i].ucNumChannels = prSubBand->ucNumChannels;
		}
	}

	/*                            */
	rStatus = wlanSendSetQueryCmd(prAdapter,	/*           */
				      CMD_ID_SET_DOMAIN_INFO,	/*       */
				      TRUE,	/*            */
				      FALSE,	/*            */
				      fgIsOid,	/*         */
				      NULL,	/*                  */
				      NULL,	/*                     */
				      sizeof(CMD_SET_DOMAIN_INFO_T),	/*                   */
				      (PUINT_8) prCmd,	/*               */
				      NULL,	/*                  */
				      0	/*                     */
	    );

	ASSERT(rStatus == WLAN_STATUS_PENDING);

	cnmMemFree(prAdapter, prCmd);
}

/*                                                                            */
/* 
        
 
                
 
                             
                                                             
*/
/*                                                                            */
BOOLEAN rlmDomainIsLegalChannel(P_ADAPTER_T prAdapter, ENUM_BAND_T eBand, UINT_8 ucChannel)
{
	UINT_8 i, j;
	P_DOMAIN_SUBBAND_INFO prSubband;
	P_DOMAIN_INFO_ENTRY prDomainInfo;

	prDomainInfo = rlmDomainGetDomainInfo(prAdapter);
	ASSERT(prDomainInfo);

	for (i = 0; i < MAX_SUBBAND_NUM; i++) {
		prSubband = &prDomainInfo->rSubBand[i];

		if (prSubband->ucBand == BAND_5G && !prAdapter->fgEnable5GBand) {
			continue;
		}

		if (prSubband->ucBand == eBand) {
			for (j = 0; j < prSubband->ucNumChannels; j++) {
				if ((prSubband->ucFirstChannelNum + j * prSubband->ucChannelSpan)
				    == ucChannel) {
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}
