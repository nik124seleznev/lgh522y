/*
                                                                                       
*/

/*                  
                                                                                 

                                                                         
*/



/*
                   
  
                   
                                                                                               
                                         
                                                        
  
                   
  
                                                                               
  
                   
                                                    
                           
  
                   
                                                    
                      
                                                                      
                                                           
                                         
                                          
  
                   
                                                    
                                     
  
                                                     
  
                   
                                                    
                                                    
  
                         
                                                                       
  
                                                           
  
                         
                                                           
                                                                             
  
                         
                                                               
                                  
  
                       
                                                                
                                         
  
                       
                                                                
  
*/


#ifndef _AA_FSM_H
#define _AA_FSM_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
/*                                                                */
#define TX_AUTHENTICATION_RETRY_TIMEOUT_TU          100	/*     */

/*                                                             */
#define TX_ASSOCIATION_RETRY_TIMEOUT_TU             100	/*     */

/*                                                                    */
#define DOT11_AUTHENTICATION_RESPONSE_TIMEOUT_TU    512	/*     */

/*                                                                 */
#define DOT11_ASSOCIATION_RESPONSE_TIMEOUT_TU       512	/*     */

/*                                                     */
#define JOIN_FAILURE_TIMEOUT_BEACON_INTERVAL        20	/*                                      */

/*                                       */
#define JOIN_RETRY_INTERVAL_SEC                     10	/*         */

/*                                                */
#define JOIN_MAX_RETRY_FAILURE_COUNT                2	/*       */

/*                                                                              
                                                 
                                                                                
*/
typedef enum _ENUM_AA_STATE_T {
	AA_STATE_IDLE = 0,
	SAA_STATE_SEND_AUTH1,
	SAA_STATE_WAIT_AUTH2,
	SAA_STATE_SEND_AUTH3,
	SAA_STATE_WAIT_AUTH4,
	SAA_STATE_SEND_ASSOC1,
	SAA_STATE_WAIT_ASSOC2,
	AAA_STATE_SEND_AUTH2,
	AAA_STATE_SEND_AUTH4,	/*                                                           */
	AAA_STATE_SEND_ASSOC2,
	AA_STATE_RESOURCE,	/*                                                      */
	AA_STATE_NUM
} ENUM_AA_STATE_T;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/
/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
saaFsmSteps(IN P_ADAPTER_T prAdapter,
	    IN P_STA_RECORD_T prStaRec,
	    IN ENUM_AA_STATE_T eNextState, IN P_SW_RFB_T prRetainedSwRfb);

WLAN_STATUS
saaFsmSendEventJoinComplete(IN P_ADAPTER_T prAdapter,
			    WLAN_STATUS rJoinStatus, P_STA_RECORD_T prStaRec, P_SW_RFB_T prSwRfb);

VOID saaFsmRunEventStart(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

WLAN_STATUS
saaFsmRunEventTxDone(IN P_ADAPTER_T prAdapter,
		     IN P_MSDU_INFO_T prMsduInfo, IN ENUM_TX_RESULT_CODE_T rTxDoneStatus);

VOID saaFsmRunEventTxReqTimeOut(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

VOID saaFsmRunEventRxRespTimeOut(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

VOID saaFsmRunEventRxAuth(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

WLAN_STATUS saaFsmRunEventRxAssoc(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

WLAN_STATUS saaFsmRunEventRxDeauth(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

WLAN_STATUS saaFsmRunEventRxDisassoc(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

VOID saaFsmRunEventAbort(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID aaaFsmRunEventRxAuth(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

WLAN_STATUS aaaFsmRunEventRxAssoc(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

WLAN_STATUS
aaaFsmRunEventTxDone(IN P_ADAPTER_T prAdapter,
		     IN P_MSDU_INFO_T prMsduInfo, IN ENUM_TX_RESULT_CODE_T rTxDoneStatus);

/*                                                                              
                                                
                                                                                
*/

#endif				/*           */
