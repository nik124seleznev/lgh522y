/*
                                                                            
*/

/*                     
                                            

                       
*/





#ifndef _P2P_NIC_H
#define _P2P_NIC_H

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


VOID
nicP2pMediaStateChange(IN P_ADAPTER_T prAdapter,
		       IN ENUM_NETWORK_TYPE_INDEX_T eNetworkType,
		       IN P_EVENT_CONNECTION_STATUS prConnectionStatus);

VOID
nicRxAddP2pDevice(IN P_ADAPTER_T prAdapter,
		  IN P_EVENT_P2P_DEV_DISCOVER_RESULT_T prP2pResult,
		  IN PUINT_8 pucRxIEBuf, IN UINT_16 u2RxIELength);

#endif
