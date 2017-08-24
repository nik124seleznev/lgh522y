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
/*                               */
const UINT_8 aucDataRate[] = {
	RATE_1M,		/*                   */
	RATE_2M,		/*               */
	RATE_5_5M,		/*                 */
	RATE_11M,		/*                */
	RATE_22M,		/*                */
	RATE_33M,		/*                */
	RATE_6M,		/*               */
	RATE_9M,		/*               */
	RATE_12M,		/*                */
	RATE_18M,		/*                */
	RATE_24M,		/*                */
	RATE_36M,		/*                */
	RATE_48M,		/*                */
	RATE_54M,		/*                */
	RATE_HT_PHY		/*                   */
};

static const UINT_8 aucDefaultAckCtsRateIndex[RATE_NUM] = {
	RATE_1M_INDEX,		/*                   */
	RATE_2M_INDEX,		/*               */
	RATE_5_5M_INDEX,	/*                 */
	RATE_11M_INDEX,		/*                */
	RATE_1M_INDEX,		/*                                */
	RATE_1M_INDEX,		/*                                */
	RATE_6M_INDEX,		/*               */
	RATE_6M_INDEX,		/*               */
	RATE_12M_INDEX,		/*                */
	RATE_12M_INDEX,		/*                */
	RATE_24M_INDEX,		/*                */
	RATE_24M_INDEX,		/*                */
	RATE_24M_INDEX,		/*                */
	RATE_24M_INDEX		/*                */
};

const BOOLEAN afgIsOFDMRate[RATE_NUM] = {
	FALSE,			/*                   */
	FALSE,			/*               */
	FALSE,			/*                 */
	FALSE,			/*                */
	FALSE,			/*                                */
	FALSE,			/*                                */
	TRUE,			/*               */
	TRUE,			/*               */
	TRUE,			/*                */
	TRUE,			/*                */
	TRUE,			/*                */
	TRUE,			/*                */
	TRUE,			/*                */
	TRUE			/*                */
};

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
VOID
rateGetRateSetFromIEs(IN P_IE_SUPPORTED_RATE_T prIeSupportedRate,
		      IN P_IE_EXT_SUPPORTED_RATE_T prIeExtSupportedRate,
		      OUT PUINT_16 pu2OperationalRateSet,
		      OUT PUINT_16 pu2BSSBasicRateSet, OUT PBOOLEAN pfgIsUnknownBSSBasicRate)
{
	UINT_16 u2OperationalRateSet = 0;
	UINT_16 u2BSSBasicRateSet = 0;
	BOOLEAN fgIsUnknownBSSBasicRate = FALSE;
	UINT_8 ucRate;
	UINT_32 i, j;


	ASSERT(pu2OperationalRateSet);
	ASSERT(pu2BSSBasicRateSet);
	ASSERT(pfgIsUnknownBSSBasicRate);

	if (prIeSupportedRate) {
		/*                                                                
                                                                 
                                              
   */
		/*                                                                */
		ASSERT(prIeSupportedRate->ucLength <= RATE_NUM);

		for (i = 0; i < prIeSupportedRate->ucLength; i++) {
			ucRate = prIeSupportedRate->aucSupportedRates[i] & RATE_MASK;

			/*                             */
			for (j = 0; j < sizeof(aucDataRate) / sizeof(UINT_8); j++) {
				if (ucRate == aucDataRate[j]) {
					u2OperationalRateSet |= BIT(j);

					if (prIeSupportedRate->
					    aucSupportedRates[i] & RATE_BASIC_BIT) {
						u2BSSBasicRateSet |= BIT(j);
					}

					break;
				}
			}

			if ((j == sizeof(aucDataRate) / sizeof(UINT_8)) &&
			    (prIeSupportedRate->aucSupportedRates[i] & RATE_BASIC_BIT)) {
				fgIsUnknownBSSBasicRate = TRUE;	/*                                           */
			}
		}
	}


	if (prIeExtSupportedRate) {
		/*                                                                            */

		for (i = 0; i < prIeExtSupportedRate->ucLength; i++) {
			ucRate = prIeExtSupportedRate->aucExtSupportedRates[i] & RATE_MASK;

			/*                             */
			for (j = 0; j < sizeof(aucDataRate) / sizeof(UINT_8); j++) {
				if (ucRate == aucDataRate[j]) {
					u2OperationalRateSet |= BIT(j);

					if (prIeExtSupportedRate->
					    aucExtSupportedRates[i] & RATE_BASIC_BIT) {
						u2BSSBasicRateSet |= BIT(j);
					}

					break;
				}
			}

			if ((j == sizeof(aucDataRate) / sizeof(UINT_8)) &&
			    (prIeExtSupportedRate->aucExtSupportedRates[i] & RATE_BASIC_BIT)) {
				fgIsUnknownBSSBasicRate = TRUE;	/*                                           */
			}
		}
	}

	*pu2OperationalRateSet = u2OperationalRateSet;
	*pu2BSSBasicRateSet = u2BSSBasicRateSet;
	*pfgIsUnknownBSSBasicRate = fgIsUnknownBSSBasicRate;

	return;

}				/*                                */


/*                                                                            */
/* 
                                                                                 
                                                   
 
                                                        
                                                  
                                                                   
                                                                          
 
                
*/
/*                                                                            */
VOID
rateGetDataRatesFromRateSet(IN UINT_16 u2OperationalRateSet,
			    IN UINT_16 u2BSSBasicRateSet,
			    OUT PUINT_8 pucDataRates, OUT PUINT_8 pucDataRatesLen)
{
	UINT_32 i, j;


	ASSERT(pucDataRates);
	ASSERT(pucDataRatesLen);

	ASSERT(u2BSSBasicRateSet == (u2OperationalRateSet & u2BSSBasicRateSet));

	for (i = RATE_1M_INDEX, j = 0; i < RATE_NUM; i++) {
		if (u2OperationalRateSet & BIT(i)) {

			*(pucDataRates + j) = aucDataRate[i];

			if (u2BSSBasicRateSet & BIT(i)) {
				*(pucDataRates + j) |= RATE_BASIC_BIT;
			}

			j++;
		}
	}

	*pucDataRatesLen = (UINT_8) j;

	return;

}				/*                                      */


/*                                                                            */
/* 
                                                  
 
                                            
                                                                               
 
                                            
                                                
*/
/*                                                                            */
BOOLEAN rateGetHighestRateIndexFromRateSet(IN UINT_16 u2RateSet, OUT PUINT_8 pucHighestRateIndex)
{
	INT_32 i;


	ASSERT(pucHighestRateIndex);

	for (i = RATE_54M_INDEX; i >= RATE_1M_INDEX; i--) {
		if (u2RateSet & BIT(i)) {
			*pucHighestRateIndex = (UINT_8) i;
			return TRUE;
		}
	}

	return FALSE;

}				/*                                             */


/*                                                                            */
/* 
                                                 
 
                                            
                                                                              
 
                                           
                                               
*/
/*                                                                            */
BOOLEAN rateGetLowestRateIndexFromRateSet(IN UINT_16 u2RateSet, OUT PUINT_8 pucLowestRateIndex)
{
	UINT_32 i;

	ASSERT(pucLowestRateIndex);

	for (i = RATE_1M_INDEX; i <= RATE_54M_INDEX; i++) {
		if (u2RateSet & BIT(i)) {
			*pucLowestRateIndex = (UINT_8) i;
			return TRUE;
		}
	}

	return FALSE;

}				/*                                            */


#if 0				/*                            */
/*                                                                            */
/* 
                                                      
 
                                                         
                                                          
                                                       
 
                
*/
/*                                                                            */
VOID
rateGetRateSetFromDataRates(IN PUINT_8 pucDataRates,
			    IN UINT_8 ucDataRatesLen, OUT PUINT_16 pu2RateSet)
{
	UINT_16 u2RateSet = 0;
	UINT_8 ucRate;
	UINT_32 i, j;


	ASSERT(pucDataRates);
	ASSERT(pu2RateSet);

	if (pucDataRates) {
		for (i = 0; i < ucDataRatesLen; i++) {
			ucRate = pucDataRates[i] & RATE_MASK;

			/*                             */
			for (j = 0; j < sizeof(aucDataRate) / sizeof(UINT_8); j++) {
				if (ucRate == aucDataRate[j]) {
					u2RateSet |= BIT(j);
					break;
				}
			}
		}
	}

	*pu2RateSet = u2RateSet;

	return;

}				/*                                      */


/*                                                                            */
/* 
                                                                                   
                                    
 
                                                        
                                                  
                                                                           
 
                
*/
/*                                                                            */
VOID
rateSetAckCtsDataRatesFromRateSet(IN UINT_16 u2OperationalRateSet,
				  IN UINT_16 u2BSSBasicRateSet, IN OUT UINT_8 aucAckCtsRateIndex[]
    )
{
	INT_32 i, j;


	ASSERT(aucAckCtsRateIndex);
	ASSERT(u2BSSBasicRateSet == (u2OperationalRateSet & u2BSSBasicRateSet));

	/*                                     */
	kalMemCopy(aucAckCtsRateIndex, (PVOID) aucDefaultAckCtsRateIndex,
		   sizeof(aucDefaultAckCtsRateIndex));


	for (i = RATE_54M_INDEX; i >= RATE_1M_INDEX; i--) {
		if (u2OperationalRateSet & BIT(i)) {
			for (j = i; j >= RATE_1M_INDEX; j--) {
				if (u2BSSBasicRateSet & BIT(j)) {
					if ((afgIsOFDMRate[i] && afgIsOFDMRate[j]) || (!afgIsOFDMRate[i] && !afgIsOFDMRate[j]))	/*                                                */
						aucAckCtsRateIndex[i] = (UINT_8) j;
					break;
				}
			}

			/*                                                                      
                        
                                                                               
                                                                       
                                                              
                                                                                   
                                            
    */
			if (j < RATE_1M_INDEX) {	/*                                                */
				if (!(BIT(aucAckCtsRateIndex[i]) & u2OperationalRateSet)) {
					aucAckCtsRateIndex[i] = (UINT_8) i;
				}
			}
		}
	}

	return;

}				/*                                            */


/*                                                                            */
/* 
                                                                                
 
                                            
                                                                  
                                                                               
 
                                            
                                                
*/
/*                                                                            */
BOOLEAN
rateGetBestInitialRateIndex(IN UINT_16 u2RateSet, IN RCPI rRcpi, OUT PUINT_8 pucInitialRateIndex)
{
	UINT_16 u2InitRateSet;
	INT_32 i;


	ASSERT(pucInitialRateIndex);

	DBGLOG(MGT, TRACE, ("rRcpi = %d\n", rRcpi));

	if (rRcpi >= RCPI_100) {	/*             */
		u2InitRateSet = INITIAL_RATE_SET(RCPI_100);
	} else if (rRcpi >= RCPI_80) {	/*               */
		u2InitRateSet = INITIAL_RATE_SET(RCPI_80);
	} else if (rRcpi >= RCPI_60) {	/*             */
		u2InitRateSet = INITIAL_RATE_SET(RCPI_60);
	} else {		/*              */
		/*                                                                        
                                                                            
                                                 
   */
		return FALSE;
	}

	u2RateSet &= u2InitRateSet;

	for (i = RATE_54M_INDEX; i >= RATE_1M_INDEX; i--) {
		if (u2RateSet & BIT(i)) {
			*pucInitialRateIndex = (UINT_8) i;
			return TRUE;
		}
	}

	return FALSE;

}				/*                                      */
#endif
