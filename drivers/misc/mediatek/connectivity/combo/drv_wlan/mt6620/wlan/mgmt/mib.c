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
NON_HT_PHY_ATTRIBUTE_T rNonHTPhyAttributes[] = {
	{RATE_SET_HR_DSSS, TRUE, FALSE}
	,			/*                               */
	{RATE_SET_ERP, TRUE, TRUE}
	,			/*                           */
	{RATE_SET_ERP_P2P, TRUE, TRUE}
	,			/*                               */
	{RATE_SET_OFDM, FALSE, FALSE}
	,			/*                            */
};

NON_HT_ADHOC_MODE_ATTRIBUTE_T rNonHTAdHocModeAttributes[AD_HOC_MODE_NUM] = {
	{PHY_TYPE_HR_DSSS_INDEX, BASIC_RATE_SET_HR_DSSS}
	,			/*                        */
	{PHY_TYPE_ERP_INDEX, BASIC_RATE_SET_HR_DSSS_ERP}
	,			/*                               */
	{PHY_TYPE_ERP_INDEX, BASIC_RATE_SET_ERP}
	,			/*                        */
	{PHY_TYPE_OFDM_INDEX, BASIC_RATE_SET_OFDM}
	,			/*                        */
};

NON_HT_AP_MODE_ATTRIBUTE_T rNonHTApModeAttributes[AP_MODE_NUM] = {
	{PHY_TYPE_HR_DSSS_INDEX, BASIC_RATE_SET_HR_DSSS}
	,			/*                    */
	{PHY_TYPE_ERP_INDEX, BASIC_RATE_SET_HR_DSSS_ERP}
	,			/*                           */
	{PHY_TYPE_ERP_INDEX, BASIC_RATE_SET_ERP}
	,			/*                    */
	{PHY_TYPE_ERP_P2P_INDEX, BASIC_RATE_SET_ERP_P2P}
	,			/*                        */
	{PHY_TYPE_OFDM_INDEX, BASIC_RATE_SET_OFDM}
	,			/*                    */
};

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
