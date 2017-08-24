/*
                                                                                     
*/

/*               
                                                                            
*/



/*
                 
  
                      
                                                             
                                                  
                                                                      
  
                   
  
    
  
                   
                                                                          
                                        
  
                      
                                                                           
                                                                         
  
                      
                                                                           
                                    
  
                       
                                                                           
  
                                                             
  
*/

#ifndef _WAPI_H
#define _WAPI_H

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
#define WAPI_CIPHER_SUITE_WPI           0x01721400	/*          */
#define WAPI_AKM_SUITE_802_1X           0x01721400	/*     */
#define WAPI_AKM_SUITE_PSK              0x02721400	/*         */

#define ELEM_ID_WAPI                    68	/*         */

#define WAPI_IE(fp)                     ((P_WAPI_INFO_ELEM_T) fp)


/*                                                                              
                                                            
                                                                                
*/

VOID wapiGenerateWAPIIE(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);

BOOLEAN wapiParseWapiIE(IN P_WAPI_INFO_ELEM_T prInfoElem, OUT P_WAPI_INFO_T prWapiInfo);

BOOLEAN wapiPerformPolicySelection(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBss);

/*         */
/*                      */
/*                                  */
/*                                  */
/*    */

/*                                                                              
                                                
                                                                                
*/
#endif
#endif				/*         */
