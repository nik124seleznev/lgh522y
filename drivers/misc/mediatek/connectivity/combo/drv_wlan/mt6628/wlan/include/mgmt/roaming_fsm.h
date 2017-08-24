/*
       
*/

/*                         
                                                         

                                                 
*/



/*
                        
  
                          
                                                                                 
                        
  
                          
                                                                                 
                                                           
  
                          
                                                                              
                                                   
  
                          
                                                         
                                                                                    
  
                          
                                                             
                  
                                                                        
                                                                   
                                                                             
                                                       
  
                          
                                                             
                  
                                                                        
                                                                   
                                                                             
                                                       
  
*/

#ifndef _ROAMING_FSM_H
#define _ROAMING_FSM_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
/*                                                            */
#define ROAMING_DISCOVERY_TIMEOUT_SEC               5	/*          */

/*                                                              */
/*                                                                              
                                                 
                                                                                
*/
typedef enum _ENUM_ROAMING_FAIL_REASON_T {
	ROAMING_FAIL_REASON_CONNLIMIT = 0,
	ROAMING_FAIL_REASON_NOCANDIDATE,
	ROAMING_FAIL_REASON_NUM
} ENUM_ROAMING_FAIL_REASON_T;

/*                                               */
typedef enum _ENUM_ROAMING_EVENT_T {
	ROAMING_EVENT_START = 0,
	ROAMING_EVENT_DISCOVERY,
	ROAMING_EVENT_ROAM,
	ROAMING_EVENT_FAIL,
	ROAMING_EVENT_ABORT,
	ROAMING_EVENT_NUM
} ENUM_ROAMING_EVENT_T;

typedef struct _ROAMING_PARAM_T {
	UINT_16 u2Event;
	UINT_16 u2Data;
} ROAMING_PARAM_T, *P_ROAMING_PARAM_T;

 /**/ typedef enum _ENUM_ROAMING_STATE_T {
	ROAMING_STATE_IDLE = 0,
	ROAMING_STATE_DECISION,
	ROAMING_STATE_DISCOVERY,
	ROAMING_STATE_ROAM,
	ROAMING_STATE_NUM
} ENUM_ROAMING_STATE_T;

typedef struct _ROAMING_INFO_T {
	BOOLEAN fgIsEnableRoaming;

	ENUM_ROAMING_STATE_T eCurrentState;

	OS_SYSTIME rRoamingDiscoveryUpdateTime;

} ROAMING_INFO_T, *P_ROAMING_INFO_T;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

#if CFG_SUPPORT_ROAMING
#define IS_ROAMING_ACTIVE(prAdapter) \
	(prAdapter->rWifiVar.rRoamingInfo.eCurrentState == ROAMING_STATE_ROAM)
#else
#define IS_ROAMING_ACTIVE(prAdapter) FALSE
#endif				/*                     */

/*                                                                              
                                                            
                                                                                
*/
VOID roamingFsmInit(IN P_ADAPTER_T prAdapter);

VOID roamingFsmUninit(IN P_ADAPTER_T prAdapter);

VOID roamingFsmSendCmd(IN P_ADAPTER_T prAdapter, IN P_ROAMING_PARAM_T prParam);

VOID roamingFsmScanResultsUpdate(IN P_ADAPTER_T prAdapter);

VOID roamingFsmSteps(IN P_ADAPTER_T prAdapter, IN ENUM_ROAMING_STATE_T eNextState);

VOID roamingFsmRunEventStart(IN P_ADAPTER_T prAdapter);

VOID roamingFsmRunEventDiscovery(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Param);

VOID roamingFsmRunEventRoam(IN P_ADAPTER_T prAdapter);

VOID roamingFsmRunEventFail(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Reason);

VOID roamingFsmRunEventAbort(IN P_ADAPTER_T prAdapter);

WLAN_STATUS roamingFsmProcessEvent(IN P_ADAPTER_T prAdapter, IN P_ROAMING_PARAM_T prParam);

/*                                                                              
                                                
                                                                                
*/

#endif				/*                */
