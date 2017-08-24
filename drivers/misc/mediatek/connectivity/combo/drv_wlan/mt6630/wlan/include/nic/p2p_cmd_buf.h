/*
       
*/

/*                         
                                                                    
  
                                                                                            
                        
 */



/*
                        
  
                        
       
  
                        
                                                                                         
                                                    
  
                      
                                                             
                                                  
                                                                      
 
                       
      
                                    
 
                  
                                                                                                                      
          
                                                            
                                                                                   
*/

#ifndef _P2P_CMD_BUF_H
#define _P2P_CMD_BUF_H

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

/*                                                                              
                                                              
                                                                                 
 */

/*                                                              */
/*                                                              */
/*                                                              */
WLAN_STATUS
wlanoidSendSetQueryP2PCmd(IN P_ADAPTER_T prAdapter, IN UINT_8 ucCID, IN UINT_8 ucBssIdx, IN BOOLEAN fgSetQuery, IN
			  BOOLEAN fgNeedResp, IN BOOLEAN fgIsOid, IN PFN_CMD_DONE_HANDLER pfCmdDoneHandler, IN
			  PFN_CMD_TIMEOUT_HANDLER
			  pfCmdTimeoutHandler, IN UINT_32 u4SetQueryInfoLen, IN PUINT_8 pucInfoBuffer, OUT PVOID
			  pvSetQueryBuffer, IN UINT_32
			  u4SetQueryBufferLen);




#endif				/*                */
