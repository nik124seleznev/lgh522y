/*
                                                                                        
*/

/*                   
                                                                             

                                                                     
*/



/*
                    
  
                        
                                                                                       
                                                              
                                                                         
                 
                                                   
                                              
  
                   
                                                                                                                                                                
                                                       
                                                                                                                                      
                                                              
  
                   
                                                                                                          
                                                                                                                
  
                          
                                                         
                                                                                    
  
                   
                                                                                                                                                       
                                                  
  
                          
                                                             
                  
                                                                        
                                                                   
                                                                             
                                                       
  
                   
                                                                                                  
                                                                                
                                                                                                
  
                   
                                                                                     
                                                       
  
                   
                                                                                                                                                                                                      
                                                                                                   
                                          
                                                                          
  
                   
       
                                                               
                                                               
  
                         
       
                                                                     
  
                   
       
                                                                                                                                             
  
                         
       
                                                                   
  
                   
       
                                                                   
  
                   
       
                                
  
                   
       
                                                                                
                                                                                  
                                                                                   
  
                   
  
                                                                                                                                 
  
                   
  
                                                                                   
                                                                                    
  
                   
  
                                                 
  
                   
  
                                                                   
  
                   
  
                                                       
                                                             
  
                   
  
                                                      
                                
  
                   
  
                                                      
                                             
  
                   
  
                                                                                                     
                                                                            
                                                                                                                                   
  
                   
  
                                                                               
  
                   
                                                    
                                                        
  
                   
                                                    
                                                                  
  
                   
                                                    
                                     
  
                                                     
  
                   
                                                    
                                        
  
                   
                                                    
                                                    
  
                         
                                                               
                                                 
  
                   
                                             
                                                        
  
                         
                                                             
                             
                                                      
  
                         
                                                          
                 
  
                         
                                                                       
  
                                                              
  
                         
                                                           
                                                                                
  
                         
                                                           
                                    
  
                         
                                                           
                                                                             
  
                         
                                                               
                                  
  
                         
                                                               
                                                                
                                                                 
  
                      
                                                                
                                   
  
                       
                                                                
                                              
  
                       
                                                                
                                           
  
                       
                                                                
                                         
  
                       
                                                                
                                         
  
                       
                                                                
  
*/

#ifndef _AIS_FSM_H
#define _AIS_FSM_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
#define AIS_BG_SCAN_INTERVAL_MIN_SEC        2	/*                          */
#define AIS_BG_SCAN_INTERVAL_MAX_SEC        2	/*              */

#define AIS_DELAY_TIME_OF_DISCONNECT_SEC    2

#define AIS_IBSS_ALONE_TIMEOUT_SEC          20	/*         */

#define AIS_BEACON_TIMEOUT_COUNT_ADHOC      30
#define AIS_BEACON_TIMEOUT_COUNT_INFRA      10
#define AIS_BEACON_TIMEOUT_GUARD_TIME_SEC   1	/*        */

#define AIS_BEACON_MAX_TIMEOUT_TU           100
#define AIS_BEACON_MIN_TIMEOUT_TU           5
#define AIS_BEACON_MAX_TIMEOUT_VALID        TRUE
#define AIS_BEACON_MIN_TIMEOUT_VALID        TRUE

#define AIS_BMC_MAX_TIMEOUT_TU              100
#define AIS_BMC_MIN_TIMEOUT_TU              5
#define AIS_BMC_MAX_TIMEOUT_VALID           TRUE
#define AIS_BMC_MIN_TIMEOUT_VALID           TRUE

#define AIS_JOIN_CH_GRANT_THRESHOLD         10
#define AIS_JOIN_CH_REQUEST_INTERVAL        2000

/*                                                                              
                                                 
                                                                                
*/
typedef enum _ENUM_AIS_STATE_T {
	AIS_STATE_IDLE = 0,
	AIS_STATE_SEARCH,
	AIS_STATE_SCAN,
	AIS_STATE_ONLINE_SCAN,
	AIS_STATE_LOOKING_FOR,
	AIS_STATE_WAIT_FOR_NEXT_SCAN,
	AIS_STATE_REQ_CHANNEL_JOIN,
	AIS_STATE_JOIN,
	AIS_STATE_IBSS_ALONE,
	AIS_STATE_IBSS_MERGE,
	AIS_STATE_NORMAL_TR,
	AIS_STATE_DISCONNECTING,
	AIS_STATE_NUM
} ENUM_AIS_STATE_T;


typedef struct _MSG_AIS_ABORT_T {
	MSG_HDR_T rMsgHdr;	/*                          */
	UINT_8 ucReasonOfDisconnect;
	BOOLEAN fgDelayIndication;
} MSG_AIS_ABORT_T, *P_MSG_AIS_ABORT_T;


typedef struct _MSG_AIS_IBSS_PEER_FOUND_T {
	MSG_HDR_T rMsgHdr;	/*                          */
	UINT_8 ucNetTypeIndex;
	BOOLEAN fgIsMergeIn;	/*                                  */
	P_STA_RECORD_T prStaRec;
} MSG_AIS_IBSS_PEER_FOUND_T, *P_MSG_AIS_IBSS_PEER_FOUND_T;

typedef enum _ENUM_AIS_REQUEST_TYPE_T {
	AIS_REQUEST_SCAN,
	AIS_REQUEST_RECONNECT,
	AIS_REQUEST_ROAMING_SEARCH,
	AIS_REQUEST_ROAMING_CONNECT,
	AIS_REQUEST_NUM
} ENUM_AIS_REQUEST_TYPE_T;

typedef struct _AIS_REQ_HDR_T {
	LINK_ENTRY_T rLinkEntry;
	ENUM_AIS_REQUEST_TYPE_T eReqType;
} AIS_REQ_HDR_T, *P_AIS_REQ_HDR_T;


typedef struct _AIS_FSM_INFO_T {
	ENUM_AIS_STATE_T ePreviousState;
	ENUM_AIS_STATE_T eCurrentState;

	BOOLEAN fgTryScan;

	BOOLEAN fgIsInfraChannelFinished;
	BOOLEAN fgIsChannelRequested;
	BOOLEAN fgIsChannelGranted;

#if CFG_SUPPORT_ROAMING
	BOOLEAN fgIsRoamingScanPending;
#endif				/*                     */

	UINT_8 ucAvailableAuthTypes;	/*                                */

	P_BSS_DESC_T prTargetBssDesc;	/*                 */

	P_STA_RECORD_T prTargetStaRec;	/*                */

	UINT_32 u4SleepInterval;

	TIMER_T rBGScanTimer;

	TIMER_T rIbssAloneTimer;

	TIMER_T rIndicationOfDisconnectTimer;

	TIMER_T rJoinTimeoutTimer;

	UINT_8 ucSeqNumOfReqMsg;
	UINT_8 ucSeqNumOfChReq;
	UINT_8 ucSeqNumOfScanReq;

	UINT_32 u4ChGrantedInterval;

	UINT_8 ucConnTrialCount;

	UINT_8 ucScanSSIDLen;
	UINT_8 aucScanSSID[ELEM_MAX_LEN_SSID];

	UINT_32 u4ScanIELength;
	UINT_8 aucScanIEBuf[MAX_IE_LENGTH];

	/*                      */
	LINK_T rPendingReqList;

} AIS_FSM_INFO_T, *P_AIS_FSM_INFO_T;


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define aisChangeMediaState(_prAdapter, _eNewMediaState) \
	    (_prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX].eConnectionState = (_eNewMediaState));


/*                                                                              
                                                            
                                                                                
*/
VOID aisInitializeConnectionSettings(IN P_ADAPTER_T prAdapter, IN P_REG_INFO_T prRegInfo);

VOID aisFsmInit(IN P_ADAPTER_T prAdapter);

VOID aisFsmUninit(IN P_ADAPTER_T prAdapter);

VOID aisFsmStateInit_JOIN(IN P_ADAPTER_T prAdapter, P_BSS_DESC_T prBssDesc);

BOOLEAN aisFsmStateInit_RetryJOIN(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

VOID aisFsmStateInit_IBSS_ALONE(IN P_ADAPTER_T prAdapter);

VOID aisFsmStateInit_IBSS_MERGE(IN P_ADAPTER_T prAdapter, P_BSS_DESC_T prBssDesc);

VOID
aisFsmStateAbort(IN P_ADAPTER_T prAdapter, UINT_8 ucReasonOfDisconnect, BOOLEAN fgDelayIndication);

VOID aisFsmStateAbort_JOIN(IN P_ADAPTER_T prAdapter);

VOID aisFsmStateAbort_SCAN(IN P_ADAPTER_T prAdapter);

VOID aisFsmStateAbort_NORMAL_TR(IN P_ADAPTER_T prAdapter);

VOID aisFsmStateAbort_IBSS(IN P_ADAPTER_T prAdapter);

VOID aisFsmSteps(IN P_ADAPTER_T prAdapter, ENUM_AIS_STATE_T eNextState);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID aisFsmRunEventScanDone(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID aisFsmRunEventAbort(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID aisFsmRunEventJoinComplete(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID aisFsmRunEventFoundIBSSPeer(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID aisFsmCreateIBSS(IN P_ADAPTER_T prAdapter);

VOID aisFsmMergeIBSS(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID aisFsmRunEventChGrant(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);


/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID aisFsmReleaseCh(IN P_ADAPTER_T prAdapter);


/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
aisIndicationOfMediaStateToHost(IN P_ADAPTER_T prAdapter,
				ENUM_PARAM_MEDIA_STATE_T eConnectionState,
				BOOLEAN fgDelayIndication);

VOID aisPostponedEventOfDisconnTimeout(IN P_ADAPTER_T prAdapter, UINT_32 u4Param);

VOID
aisUpdateBssInfoForJOIN(IN P_ADAPTER_T prAdapter,
			P_STA_RECORD_T prStaRec, P_SW_RFB_T prAssocRspSwRfb);

VOID aisUpdateBssInfoForCreateIBSS(IN P_ADAPTER_T prAdapter);

VOID aisUpdateBssInfoForMergeIBSS(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

BOOLEAN
aisValidateProbeReq(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb, OUT PUINT_32 pu4ControlFlags);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID aisFsmDisconnect(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgDelayIndication);


/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID aisBssBeaconTimeout(IN P_ADAPTER_T prAdapter);

WLAN_STATUS
aisDeauthXmitComplete(IN P_ADAPTER_T prAdapter,
		      IN P_MSDU_INFO_T prMsduInfo, IN ENUM_TX_RESULT_CODE_T rTxDoneStatus);

#if CFG_SUPPORT_ROAMING
VOID aisFsmRunEventRoamingDiscovery(IN P_ADAPTER_T prAdapter, UINT_32 u4ReqScan);

ENUM_AIS_STATE_T aisFsmRoamingScanResultsUpdate(IN P_ADAPTER_T prAdapter);

VOID aisFsmRoamingDisconnectPrevAP(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prTargetStaRec);

VOID
aisUpdateBssInfoForRoamingAP(IN P_ADAPTER_T prAdapter,
			     IN P_STA_RECORD_T prStaRec, IN P_SW_RFB_T prAssocRspSwRfb);
#endif				/*                    */

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID aisFsmRunEventBGSleepTimeOut(IN P_ADAPTER_T prAdapter, UINT_32 u4Param);

VOID aisFsmRunEventIbssAloneTimeOut(IN P_ADAPTER_T prAdapter, UINT_32 u4Param);

VOID aisFsmRunEventJoinTimeout(IN P_ADAPTER_T prAdapter, UINT_32 u4Param);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
aisFsmScanRequest(IN P_ADAPTER_T prAdapter,
		  IN P_PARAM_SSID_T prSsid, IN PUINT_8 pucIe, IN UINT_32 u4IeLength);

/*                                                                            */
/*                                                                            */
/*                                                                            */
BOOLEAN
aisFsmIsRequestPending(IN P_ADAPTER_T prAdapter,
		       IN ENUM_AIS_REQUEST_TYPE_T eReqType, IN BOOLEAN bRemove);

P_AIS_REQ_HDR_T aisFsmGetNextRequest(IN P_ADAPTER_T prAdapter);

BOOLEAN aisFsmInsertRequest(IN P_ADAPTER_T prAdapter, IN ENUM_AIS_REQUEST_TYPE_T eReqType);

VOID aisFsmFlushRequest(IN P_ADAPTER_T prAdapter);


#if defined(CFG_TEST_MGMT_FSM) && (CFG_TEST_MGMT_FSM != 0)
VOID aisTest(VOID);
#endif				/*                   */
/*                                                                              
                                                
                                                                                
*/

#endif				/*            */
