/*
                                                                                         
*/

/*                      
          
*/



/*
                     
  
                      
                                                             
                                                  
                                                                      
  
                      
                                                                                                                             
                                                              
  
                      
                                                                                        
                                                        
  
                      
                                                                            
                                                         
  
                      
       
                                                     
  
                   
  
                                                                               
  
                      
                                                            
                                
  
                      
                                                               
                                                           
  
                      
                                                               
                                                       
  
                      
                                                               
                                 
  
                      
                                                               
                         
  
                      
                                                               
                          
  
                      
                                                               
                                           
  
                      
                                                               
                                          
*/

#ifndef _RLM_OBSS_H
#define _RLM_OBSS_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
#define CHNL_LIST_SZ_2G         14
#define CHNL_LIST_SZ_5G         14


#define CHNL_LEVEL0             0
#define CHNL_LEVEL1             1
#define CHNL_LEVEL2             2

#define AFFECTED_CHNL_OFFSET    5

#define OBSS_SCAN_MIN_INTERVAL  10	/*                */

#define PUBLIC_ACTION_MAX_LEN   200	/*                 */

/*             */
/*                                                         */
#define dot11OBSSScanPassiveDwell                   20
#define dot11OBSSScanActiveDwell                    10
#define dot11OBSSScanPassiveTotalPerChannel         200
#define dot11OBSSScanActiveTotalPerChannel          20
#define dot11BSSWidthTriggerScanInterval            300	/*           */
#define dot11BSSWidthChannelTransitionDelayFactor   5
#define dot11OBSSScanActivityThreshold              25

#define OBSS_20_40M_TIMEOUT     (dot11BSSWidthTriggerScanInterval + 10)

/*                                                                              
                                                 
                                                                                
*/

/*                          */
typedef enum _ENUM_SYS_PCO_PHASE_T {
	SYS_PCO_PHASE_DISABLED = 0,
	SYS_PCO_PHASE_20M,
	SYS_PCO_PHASE_40M
} ENUM_SYS_PCO_PHASE_T, *P_ENUM_SYS_PCO_PHASE_T;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/
VOID rlmObssInit(P_ADAPTER_T prAdapter);

VOID rlmObssScanDone(P_ADAPTER_T prAdapter, P_MSG_HDR_T prMsgHdr);

VOID rlmObssTriggerScan(P_ADAPTER_T prAdapter, P_BSS_INFO_T prBssInfo);

/*                                                                              
                                                
                                                                                
*/

#endif				/*             */
