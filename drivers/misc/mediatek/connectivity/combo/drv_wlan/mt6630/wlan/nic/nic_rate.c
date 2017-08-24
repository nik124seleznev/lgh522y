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

const UINT_16 au2RateCCKLong[CCK_RATE_NUM] = {
	RATE_CCK_1M_LONG,	/*                   */
	RATE_CCK_2M_LONG,	/*               */
	RATE_CCK_5_5M_LONG,	/*                 */
	RATE_CCK_11M_LONG	/*                */
};

const UINT_16 au2RateCCKShort[CCK_RATE_NUM] = {
	RATE_CCK_1M_LONG,	/*                   */
	RATE_CCK_2M_SHORT,	/*               */
	RATE_CCK_5_5M_SHORT,	/*                 */
	RATE_CCK_11M_SHORT	/*                */
};

const UINT_16 au2RateOFDM[OFDM_RATE_NUM] = {
	RATE_OFDM_6M,		/*               */
	RATE_OFDM_9M,		/*               */
	RATE_OFDM_12M,		/*                */
	RATE_OFDM_18M,		/*                */
	RATE_OFDM_24M,		/*                */
	RATE_OFDM_36M,		/*                */
	RATE_OFDM_48M,		/*                */
	RATE_OFDM_54M		/*                */
};


const UINT_16 au2RateHTMixed[HT_RATE_NUM] = {
	RATE_MM_MCS_32,		/*                   */
	RATE_MM_MCS_0,		/*                  */
	RATE_MM_MCS_1,		/*                  */
	RATE_MM_MCS_2,		/*                  */
	RATE_MM_MCS_3,		/*                  */
	RATE_MM_MCS_4,		/*                  */
	RATE_MM_MCS_5,		/*                  */
	RATE_MM_MCS_6,		/*                  */
	RATE_MM_MCS_7		/*                  */
};


const UINT_16 au2RateHTGreenField[HT_RATE_NUM] = {
	RATE_GF_MCS_32,		/*                   */
	RATE_GF_MCS_0,		/*                  */
	RATE_GF_MCS_1,		/*                  */
	RATE_GF_MCS_2,		/*                  */
	RATE_GF_MCS_3,		/*                  */
	RATE_GF_MCS_4,		/*                  */
	RATE_GF_MCS_5,		/*                  */
	RATE_GF_MCS_6,		/*                  */
	RATE_GF_MCS_7,		/*                  */
};


const UINT_16 au2RateVHT[VHT_RATE_NUM] = {
	RATE_VHT_MCS_0,		/*                  */
	RATE_VHT_MCS_1,		/*                  */
	RATE_VHT_MCS_2,		/*                  */
	RATE_VHT_MCS_3,		/*                  */
	RATE_VHT_MCS_4,		/*                  */
	RATE_VHT_MCS_5,		/*                  */
	RATE_VHT_MCS_6,		/*                  */
	RATE_VHT_MCS_7,		/*                  */
	RATE_VHT_MCS_8,		/*                  */
	RATE_VHT_MCS_9		/*                  */
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

WLAN_STATUS
nicRateIndex2RateCode(IN UINT_8 ucPreambleOption, IN UINT_8 ucRateIndex, OUT PUINT_16 pu2RateCode)
{
	switch (ucPreambleOption) {
	case PREAMBLE_DEFAULT_LONG_NONE:
		if (ucRateIndex >= CCK_RATE_NUM) {
			return WLAN_STATUS_INVALID_DATA;
		}
		*pu2RateCode = au2RateCCKLong[ucRateIndex];
		break;

	case PREAMBLE_OPTION_SHORT:
		if (ucRateIndex >= CCK_RATE_NUM) {
			return WLAN_STATUS_INVALID_DATA;
		}
		*pu2RateCode = au2RateCCKShort[ucRateIndex];
		break;

	case PREAMBLE_OFDM_MODE:
		if (ucRateIndex >= OFDM_RATE_NUM) {
			return WLAN_STATUS_INVALID_DATA;
		}
		*pu2RateCode = au2RateOFDM[ucRateIndex];
		break;

	case PREAMBLE_HT_MIXED_MODE:
		if (ucRateIndex >= HT_RATE_NUM) {
			return WLAN_STATUS_INVALID_DATA;
		}
		*pu2RateCode = au2RateHTMixed[ucRateIndex];
		break;

	case PREAMBLE_HT_GREEN_FIELD:
		if (ucRateIndex >= HT_RATE_NUM) {
			return WLAN_STATUS_INVALID_DATA;
		}
		*pu2RateCode = au2RateHTGreenField[ucRateIndex];
		break;

	case PREAMBLE_VHT_FIELD:
		if (ucRateIndex >= VHT_RATE_NUM) {
			return WLAN_STATUS_INVALID_DATA;
		}
		*pu2RateCode = au2RateVHT[ucRateIndex];
		break;

	default:
		return WLAN_STATUS_INVALID_DATA;
	}

	return WLAN_STATUS_SUCCESS;
}
