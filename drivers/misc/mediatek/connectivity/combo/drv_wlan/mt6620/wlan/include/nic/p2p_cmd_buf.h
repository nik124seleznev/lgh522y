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
wlanoidSendSetQueryP2PCmd(IN P_ADAPTER_T prAdapter,
			  UINT_8 ucCID,
			  BOOLEAN fgSetQuery,
			  BOOLEAN fgNeedResp,
			  BOOLEAN fgIsOid,
			  PFN_CMD_DONE_HANDLER pfCmdDoneHandler,
			  PFN_CMD_TIMEOUT_HANDLER pfCmdTimeoutHandler,
			  UINT_32 u4SetQueryInfoLen,
			  PUINT_8 pucInfoBuffer,
			  OUT PVOID pvSetQueryBuffer, IN UINT_32 u4SetQueryBufferLen);




#endif				/*                */
