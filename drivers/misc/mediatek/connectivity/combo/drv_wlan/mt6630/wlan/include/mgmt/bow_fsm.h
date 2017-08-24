/*
                                                                                        
*/

/*                   
                                                                             

                                                                     
*/

/*
                    
  
                      
                                                             
                                                  
                                                                      
  
                      
                                                                                      
                                   
  
                         
                                                 
                                  
  
                         
                                                 
                                                                                                    
  
                         
                                                 
                                               
  
                         
       
                              
  
                         
       
                               
 */

#ifndef _BOW_FSM_H
#define _BOW_FSM_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

#define BOW_BG_SCAN_INTERVAL_MIN_SEC        2	/*                          */
#define BOW_BG_SCAN_INTERVAL_MAX_SEC        2	/*              */

#define BOW_DELAY_TIME_OF_DISCONNECT_SEC    10

#define BOW_BEACON_TIMEOUT_COUNT_STARTING   10
#define BOW_BEACON_TIMEOUT_GUARD_TIME_SEC   1	/*        */

#define BOW_BEACON_MAX_TIMEOUT_TU           100
#define BOW_BEACON_MIN_TIMEOUT_TU           5
#define BOW_BEACON_MAX_TIMEOUT_VALID        TRUE
#define BOW_BEACON_MIN_TIMEOUT_VALID        TRUE

#define BOW_BMC_MAX_TIMEOUT_TU              100
#define BOW_BMC_MIN_TIMEOUT_TU              5
#define BOW_BMC_MAX_TIMEOUT_VALID           TRUE
#define BOW_BMC_MIN_TIMEOUT_VALID           TRUE

#define BOW_JOIN_CH_GRANT_THRESHOLD         10
#define BOW_JOIN_CH_REQUEST_INTERVAL        2000

/*                                                                              
                                                 
                                                                                
*/

typedef enum _ENUM_BOW_STATE_T {
	BOW_STATE_IDLE = 0,
	BOW_STATE_SEARCH,
	BOW_STATE_SCAN,
	BOW_STATE_ONLINE_SCAN,
	BOW_STATE_LOOKING_FOR,
	BOW_STATE_WAIT_FOR_NEXT_SCAN,
	BOW_STATE_REQ_CHANNEL_JOIN,
	BOW_STATE_REQ_CHANNEL_ALONE,
	BOW_STATE_REQ_CHANNEL_MERGE,
	BOW_STATE_JOIN,
	BOW_STATE_IBSS_ALONE,
	BOW_STATE_IBSS_MERGE,
	BOW_STATE_NORMAL_TR,
	BOW_STATE_NUM
} ENUM_BOW_STATE_T;

typedef struct _BOW_FSM_INFO_T {
	/*                   */
	BOOLEAN fgIsChannelRequested;
	BOOLEAN fgIsChannelGranted;
	UINT_32 u4ChGrantedInterval;

	UINT_8 ucPrimaryChannel;
	ENUM_BAND_T eBand;
	UINT_16 u2BeaconInterval;

	P_STA_RECORD_T prTargetStaRec;
	P_BSS_DESC_T prTargetBssDesc;	/*                 */

	UINT_8 aucPeerAddress[6];
	UINT_8 ucBssIndex;	/*                                    */
	UINT_8 ucRole;		/*                        */
	UINT_8 ucAvailableAuthTypes;	/*                                */

	BOOLEAN fgSupportQoS;

	/*                                       */
	UINT_8 ucSeqNumOfChReq;
	UINT_8 ucSeqNumOfReqMsg;
	UINT_8 ucSeqNumOfScnMsg;
	UINT_8 ucSeqNumOfScanReq;
	UINT_8 ucSeqNumOfCancelMsg;

	/*       */
	TIMER_T rStartingBeaconTimer;	/*                                                                */
	TIMER_T rChGrantedTimer;

	/*                 */
	TIMER_T rIndicationOfDisconnectTimer;



} BOW_FSM_INFO_T, *P_BOW_FSM_INFO_T;


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

#define bowChangeMediaState(_prBssInfo, _eNewMediaState) \
	    (_prBssInfo->eConnectionState = (_eNewMediaState));
	    /*                                                                                                */

/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/



#endif
