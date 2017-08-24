/*
                                                                              
*/

/*                   
                                                                             
  
                                                                     
 */



/*
                    
  
                        
       
  
                        
                                                                                         
                                                              
  
                        
                                                                                         
                                     
  
                        
                                                                                         
                     
  
                      
                                                             
                                              
  
                      
                                                             
                                                  
                                                                      
  
                        
       
                                 
  
                        
       
                                                         
                                  
  
                        
       
                             
  
                        
       
                  
  
                        
       
                      
 
                       
      
               
 
                       
      
                                            
 
                                             
 
                       
      
                               
 
                       
      
                                
 
                       
                                                                                             
                                                   
 
                       
                                                                                                               
                         
                                                                     
                                           
 
                       
                                                         
                        
 
                       
                                                         
                
 
 
                       
      
                            
                                                                       
 
                       
                                                                          
                                                                            
 
                  
                                                                                               
                                                
 
                       
      
                                         
 
                       
                                                                                
                            
 
                       
                                                                                                                     
       
                          
 
                       
                                                                                                         
                                                     
 
                       
                                                                                                        
                    
 
                       
                                                                                                        
                                                     
 
                       
                                                                                         
                                
 
                       
                                                                     
                                
 
                       
                                                                                           
                                       
 
                       
                                                                                           
                      
 
                       
                                                                                           
                                                           
 
                       
                                                                                      
                                        
 
                       
                                                                                                  
                                        
 
                       
      
                                            
 
                     
                                                                          
                               
 
                       
                                                                                              
                                     
 
                       
      
                               
 
                         
                                                                                        
                               
 
                       
      
                                
                                        
                              
 
                       
      
                                     
 
                       
                                                                                               
                             
 
                       
                                                                             
                                                                                   
 
                       
                                                                                  
                                                                                   
                                                                                                                  
 
                       
                                                                                                  
                                  
 
                       
      
              
 
                       
                                                                                                  
   
 
                  
                                                                            
                                    
 
                  
                                                                            
                                                            
 
                       
                                                                                                   
                                                
 
                       
                                                 
                                          
 
                                                    
 
                  
                                                                                                            
                                                                           
 
                       
                                                                       
                             
 
                  
                                                                                                            
                                                                           
 
                       
                                                                                                                  
                             
 
                       
                                                                                                                  
                                                   
 
                       
                                                 
 
                                          
 
                                                
 
                       
                                                                                                                  
                                                     
 
                       
                                                                               
                         
 
                       
                                                 
 
                                          
 
                                 
 
                     
                                                                        
                                                                        
 
                         
                                                                                         
                                                        
 
                       
                                                                                         
                                  
 
                       
                                                 
                                          
 
                    
 
                       
                                                 
                                          
 
                         
 
                       
                                                                                           
                                                                                                                     
                 
 
                       
      
                                              
 
                       
      
                                                                    
 
                       
      
                                              
 
                       
      
                                                                                      
 
                       
      
                                                                   
 
                         
                                                              
                                 
 
                       
      
                                              
 
                  
                                                                                                                        
                                        
                          
 
                  
                                                                    
                                     
 
                  
                                                                        
                                              
 
                  
                                                                                
                                          
 
                  
                                                                                    
                                         
 
                        
                                                                                
                                                           
 
                  
      
                                     
 
                  
      
                                                                
 
                        
      
                                                                    
 
                       
      
                                               
 
                        
      
                                                                            
 
                       
      
                                         
                              
 
                       
      
                                                                    
 
                        
      
                      
 
                        
      
                                                                  
 
                       
      
                                           
 
                       
      
                                                                     
 
                       
      
                                          
                          
                           
             
 
                       
      
                                        
 
                         
      
                                                            
 
                       
      
                                      
 
                       
 
                             
 
                  
 
                                                              
 
                       
 
                                           
 
                       
 
                             
 
                  
 
                                                                              
 
                       
                                                   
                                  
 
                       
                                                   
                                                      
 
                        
                                                         
                         
 
                        
                                                          
                                
 
                        
                                                          
                                            
 
                        
                                                          
                                                                            
*/

#ifndef _P2P_FSM_H
#define _P2P_FSM_H


/*                                                                              
                                                      
                                                                                 
 */
#define CID52_53_54         0


/*                                                                              
                                                           
                                                                                 
 */

/*                                                                              
                                                 
                                                                                 
 */





/*                                                                              
                                                  
                                                                                 
 */
typedef enum _ENUM_P2P_STATE_T {
	P2P_STATE_IDLE = 0,
	P2P_STATE_SCAN,
	P2P_STATE_AP_CHANNEL_DETECT,
	P2P_STATE_REQING_CHANNEL,
	P2P_STATE_CHNL_ON_HAND,	/*                                            */
	P2P_STATE_GC_JOIN,	/*                                                               */
	P2P_STATE_NUM
} ENUM_P2P_STATE_T, *P_ENUM_P2P_STATE_T;


struct _P2P_FSM_INFO_T {
	/*                */
	ENUM_P2P_STATE_T		ePreviousState;
	ENUM_P2P_STATE_T		eCurrentState;

	/*                  */
	P2P_CHNL_REQ_INFO_T		rChnlReqInfo;

	/*               */
	P2P_SCAN_REQ_INFO_T		rScanReqInfo;

	/*                     */
	P2P_CONNECTION_REQ_INFO_T	rConnReqInfo;

	/*                  */
	P2P_MGMT_TX_REQ_INFO_T		rMgmtTxInfo;

	/*                 */
	P2P_BEACON_UPDATE_INFO_T	rBcnContentInfo;

	/*                         */
	P2P_PROBE_RSP_UPDATE_INFO_T	rProbeRspContentInfo;

	/*                    */
	P2P_ASSOC_RSP_UPDATE_INFO_T	rAssocRspContentInfo;

	/*                  */
	P2P_JOIN_INFO_T			rJoinInfo;

	/*           */
	TIMER_T				rP2pFsmTimeoutTimer;


	/*                */
	P_BSS_DESC_T			prTargetBss;

	/*                        */
	BOOLEAN				fgIsConnectionRequested;

	BOOLEAN				fgIsApMode;

	/*                         */
	UINT_32				u4GrantInterval;

	/*                               */
	UINT_32				u4P2pPacketFilter;

	/*                                                                               */
	/*                  */
	LINK_T				rMsgEventQueue;
};


/*                                             */








/*                                                                              
                                                             
                                                                                 
 */
VOID
p2pFsmStateTransition(IN P_ADAPTER_T prAdapter, IN P_P2P_FSM_INFO_T prP2pFsmInfo, IN ENUM_P2P_STATE_T eNextState);



VOID p2pFsmRunEventAbort(IN P_ADAPTER_T prAdapter, IN P_P2P_FSM_INFO_T prP2pFsmInfo);


VOID p2pFsmRunEventScanRequest(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventMgmtFrameTx(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventStartAP(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);



VOID p2pFsmRunEventBeaconUpdate(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventStopAP(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventChannelRequest(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);


VOID p2pFsmRunEventChannelAbort(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);


VOID p2pFsmRunEventDissolve(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);





WLAN_STATUS
p2pFsmRunEventMgmtFrameTxDone(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo, IN ENUM_TX_RESULT_CODE_T
			      rTxDoneStatus);


VOID p2pFsmRunEventMgmtFrameRegister(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);




/*                                                               */ */
#define P2P_WILDCARD_SSID                           "DIRECT-"
#define P2P_WILDCARD_SSID_LEN                       7
#define P2P_GROUP_ID_LEN                            9

#define P2P_DRIVER_VERSION                          2	/*                     */

#define P2P_DEFAULT_DEV_NAME                        "Wireless Client"
#define P2P_DEFAULT_DEV_NAME_LEN                    15
#define P2P_DEFAULT_PRIMARY_CATEGORY_ID             10
#define P2P_DEFAULT_PRIMARY_SUB_CATEGORY_ID         5
#define P2P_DEFAULT_CONFIG_METHOD                   (WPS_ATTRI_CFG_METHOD_PUSH_BUTTON | WPS_ATTRI_CFG_METHOD_KEYPAD | \
						     WPS_ATTRI_CFG_METHOD_DISPLAY)

#define P2P_MAX_SUPPORTED_SEC_DEV_TYPE_COUNT        0	/*                          */
#define P2P_MAX_SUPPORTED_CHANNEL_LIST_COUNT         13

#define P2P_MAX_SUPPORTED_CHANNEL_LIST_SIZE            51	/*                      */

#define P2P_GC_MAX_CACHED_SEC_DEV_TYPE_COUNT        8		/*                          */






#define P2P_CTWINDOW_DEFAULT                        25	/*                  */



/*                            */
#define P2P_MAX_DISCOVERABLE_INTERVAL    8		/*      */
#define P2P_MIN_DISCOVERABLE_INTERVAL    5		/*   */

#define P2P_LISTEN_SCAN_UNIT                     100	/*    */

/*                             */
#define P2P_SERACH_STATE_PERIOD_MS              1000	/*             */

#define P2P_GO_CHANNEL_STAY_INTERVAL             1000

#define P2P_GO_NEGO_TIMEOUT_MS                          500
#define P2P_CONNECTION_TIMEOUT_SEC                   120

#define P2P_INVITAION_TIMEOUT_MS                         500	/*                                  */
#define P2P_PROVISION_DISCOVERY_TIMEOUT_MS     500		/*                                           */

/*   */





/*                                                                                */ */
#define P2P_ATTRI_MAX_LEN_NOTICE_OF_ABSENCE                 (2 + (P2P_MAXIMUM_NOA_COUNT * (13)))	/*    */

#define P2P_ATTRI_MAX_LEN_P2P_DEV_INFO                      17 + (8 * (8)) + 36				/*    */
/*                                                                                           */ */
#define P2P_ATTRI_MAX_LEN_P2P_GROUP_ID                      38						/*    */
#define P2P_ATTRI_MAX_LEN_P2P_INTERFACE                     253	/*                     */ * /
#if CID52_53_54
#define P2P_ATTRI_MAX_LEN_OPERATING_CHANNEL                 5		/*    */
#else
#define P2P_ATTRI_MAX_LEN_OPERATING_CHANNEL                 5		/*    */
#endif
#define P2P_ATTRI_MAX_LEN_INVITATION_FLAGS                          1	/*    */



/*                                               */
#define P2P_REASON_SUCCESS                                  0
#define P2P_REASON_DISASSOCIATED_DUE_CROSS_CONNECTION       1
#define P2P_REASON_DISASSOCIATED_DUE_UNMANAGEABLE           2
#define P2P_REASON_DISASSOCIATED_DUE_NO_P2P_COEXIST_PARAM   3
#define P2P_REASON_DISASSOCIATED_DUE_MANAGEABLE             4


/*                                                  */
#define P2P_DEV_CAPABILITY_SERVICE_DISCOVERY                BIT(0)
#define P2P_DEV_CAPABILITY_CLIENT_DISCOVERABILITY           BIT(1)
#define P2P_DEV_CAPABILITY_CONCURRENT_OPERATION             BIT(2)
#define P2P_DEV_CAPABILITY_P2P_INFRA_MANAGED                BIT(3)
#define P2P_DEV_CAPABILITY_P2P_DEVICE_LIMIT                 BIT(4)
#define P2P_DEV_CAPABILITY_P2P_INVITATION_PROCEDURE         BIT(5)


/*                                                 */
#define P2P_GROUP_CAPABILITY_P2P_GROUP_OWNER                BIT(0)
#define P2P_GROUP_CAPABILITY_PERSISTENT_P2P_GROUP           BIT(1)
#define P2P_GROUP_CAPABILITY_P2P_GROUP_LIMIT                BIT(2)
#define P2P_GROUP_CAPABILITY_INTRA_BSS_DISTRIBUTION         BIT(3)
#define P2P_GROUP_CAPABILITY_CROSS_CONNECTION               BIT(4)
#define P2P_GROUP_CAPABILITY_PERSISTENT_RECONNECT           BIT(5)
#define P2P_GROUP_CAPABILITY_GROUP_FORMATION                BIT(6)

/*                                         */
#define P2P_GO_INTENT_TIE_BREAKER_FIELD                     BIT(0)
#define P2P_GO_INTENT_VALUE_MASK                            BITS(1, 7)
#define P2P_GO_INTENT_VALUE_OFFSET                          1

/*                                               */
#define P2P_DEVICE_MANAGEMENT                               BIT(0)

/*                                                        */
#define P2P_CTW_OPPPS_PARAM_OPPPS_FIELD                     BIT(7)
#define P2P_CTW_OPPPS_PARAM_CTWINDOW_MASK                   BITS(0, 6)


#define ELEM_MAX_LEN_P2P_FOR_PROBE_REQ                      \
	(P2P_OUI_TYPE_LEN + \
	 (P2P_ATTRI_HDR_LEN + P2P_ATTRI_MAX_LEN_P2P_CAPABILITY) + \
	 (P2P_ATTRI_HDR_LEN + P2P_ATTRI_MAX_LEN_P2P_DEV_ID) + \
	 (P2P_ATTRI_HDR_LEN + P2P_ATTRI_MAX_LEN_LISTEN_CHANNEL) + \
	 (P2P_ATTRI_HDR_LEN + P2P_ATTRI_MAX_LEN_OPERATING_CHANNEL))

#define ELEM_MAX_LEN_P2P_FOR_ASSOC_REQ                      \
	(P2P_OUI_TYPE_LEN + \
	 (P2P_ATTRI_HDR_LEN + P2P_ATTRI_MAX_LEN_P2P_CAPABILITY) + \
	 (P2P_ATTRI_HDR_LEN + P2P_ATTRI_MAX_LEN_EXT_LISTEN_TIMING) + \
	 (P2P_ATTRI_HDR_LEN + P2P_ATTRI_MAX_LEN_P2P_DEV_INFO))


/*                                          */
#define P2P_CLIENT_INFO_DESC_HDR_LEN                        1	/*                  */

/*                                            */
#define P2P_INVITATION_FLAGS_INVITATION_TYPE       BIT(0)
#define P2P_INVITATION_TYPE_INVITATION                      0
#define P2P_INVITATION_TYPE_REINVOKE                          1
/*   */
/*                                   */
#define WPS_ATTRI_ID_VERSION            0x104A
#define WPS_ATTRI_ID_CONFIGURATION_METHODS   0x1008
#define WPS_ATTRI_ID_DEVICE_PASSWORD    0x1012
#define WPS_ATTRI_ID_DEVICE_NAME        0x1011
#define WPS_ATTRI_ID_PRI_DEVICE_TYPE    0x1054
#define WPS_ATTRI_ID_SEC_DEVICE_TYPE    0x1055



#define WPS_ATTRI_MAX_LEN_DEVICE_NAME   32	/*        */

#define WPS_ATTRI_CFG_METHOD_USBA           BIT(0)
#define WPS_ATTRI_CFG_METHOD_ETHERNET       BIT(1)
#define WPS_ATTRI_CFG_METHOD_LABEL          BIT(2)
#define WPS_ATTRI_CFG_METHOD_DISPLAY        BIT(3)
#define WPS_ATTRI_CFG_METHOD_EXT_NFC        BIT(4)
#define WPS_ATTRI_CFG_METHOD_INT_NFC        BIT(5)
#define WPS_ATTRI_CFG_METHOD_NFC_IF         BIT(6)
#define WPS_ATTRI_CFG_METHOD_PUSH_BUTTON    BIT(7)
#define WPS_ATTRI_CFG_METHOD_KEYPAD         BIT(8)


#define P2P_FLAGS_PROVISION_COMPLETE                            0x00000001
#define P2P_FLAGS_PROVISION_DISCOVERY_COMPLETE        0x00000002
#define P2P_FLAGS_PROVISION_DISCOVERY_WAIT_RESPONSE 0x00000004
#define P2P_FLAGS_PROVISION_DISCOVERY_RESPONSE_WAIT  0x00000008
#define P2P_FLAGS_MASK_PROVISION                                    0x00000017
#define P2P_FLAGS_MASK_PROVISION_COMPLETE                   0x00000015
#define P2P_FLAGS_PROVISION_DISCOVERY_INDICATED        0x00000010
#define P2P_FLAGS_INVITATION_TOBE_GO                            0x00000100
#define P2P_FLAGS_INVITATION_TOBE_GC                            0x00000200
#define P2P_FLAGS_INVITATION_SUCCESS                            0x00000400
#define P2P_FLAGS_INVITATION_WAITING_TARGET                            0x00000800
#define P2P_FLAGS_MASK_INVITATION                                  0x00000F00
#define P2P_FLAGS_FORMATION_ON_GOING                          0x00010000
#define P2P_FLAGS_FORMATION_LOCAL_PWID_RDY              0x00020000
#define P2P_FLAGS_FORMATION_TARGET_PWID_RDY           0x00040000
#define P2P_FLAGS_FORMATION_COMPLETE                            0x00080000
#define P2P_FLAGS_MASK_FORMATION                                  0x000F0000
#define P2P_FLAGS_DEVICE_DISCOVER_REQ                        0x00100000
#define P2P_FLAGS_DEVICE_DISCOVER_DONE                       0x00200000
#define P2P_FLAGS_DEVICE_INVITATION_WAIT                      0x00400000
#define P2P_FLAGS_DEVICE_SERVICE_DISCOVER_WAIT         0x00800000
#define P2P_FLAGS_MASK_DEVICE_DISCOVER                        0x00F00000

#define P2P_FLAGS_DEVICE_FORMATION_REQUEST                 0x01000000


/*                          */
#define SET_FLAGS(_FlagsVar, _BitsToSet) \
	(_FlagsVar) = ((_FlagsVar) | (_BitsToSet))

#define TEST_FLAGS(_FlagsVar, _BitsToCheck) \
	(((_FlagsVar) & (_BitsToCheck)) == (_BitsToCheck))

#define CLEAR_FLAGS(_FlagsVar, _BitsToClear) \
	(_FlagsVar) &= ~(_BitsToClear)



#define CFG_DISABLE_WIFI_DIRECT_ENHANCEMENT_I     0

#define CFG_DISABLE_WIFI_DIRECT_ENHANCEMENT_II     0

#define CFG_DISABLE_WIFI_DIRECT_ENHANCEMENT_III     0

#define CFG_DISABLE_WIFI_DIRECT_ENHANCEMENT_IV     0

#define CFG_DISABLE_DELAY_PROVISION_DISCOVERY      0

#define CFG_CONNECTION_POLICY_2_0                            0

/*                    */
enum wps_dev_password_id {
	DEV_PW_DEFAULT = 0x0000,
	DEV_PW_USER_SPECIFIED = 0x0001,
	DEV_PW_MACHINE_SPECIFIED = 0x0002,
	DEV_PW_REKEY = 0x0003,
	DEV_PW_PUSHBUTTON = 0x0004,
	DEV_PW_REGISTRAR_SPECIFIED = 0x0005
};

/*                                                                              
                                                  
                                                                                 
 */
#if defined(WINDOWS_DDK) || defined(WINDOWS_CE)
#pragma pack(1)
#endif

/*   */

#if 0
/*                                   */
typedef struct _P2P_ATTRIBUTE_T {
	UINT_8	ucId;		/*              */
	UINT_16 u2Length;	/*        */
	UINT_8	aucBody[1];	/*            */
} __KAL_ATTRIB_PACKED__ P2P_ATTRIBUTE_T, ATTRIBUTE_HDR_T, *P_P2P_ATTRIBUTE_T, *P_ATTRIBUTE_HDR_T;
#endif




/*                                             */
typedef struct _P2P_ATTRI_REASON_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_8	ucMinorReasonCode;	/*                   */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_REASON_T, *P_P2P_ATTRI_REASON_T;


/*                                      */
typedef struct _P2P_ATTRI_CAPABILITY_T {
	UINT_8	ucId;		/*              */
	UINT_16 u2Length;	/*        */
	UINT_8	ucDeviceCap;	/*                          */
	UINT_8	ucGroupCap;	/*                         */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_CAPABILITY_T, *P_P2P_ATTRI_CAPABILITY_T;


/*                                     */
typedef struct _P2P_ATTRI_DEV_ID_T {
	UINT_8	ucId;				/*              */
	UINT_16 u2Length;			/*        */
	UINT_8	aucDevAddr[MAC_ADDR_LEN];	/*                    */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_DEV_ID_T, *P_P2P_ATTRI_DEV_ID_T;


/*                                          */
typedef struct _P2P_ATTRI_GO_INTENT_T {
	UINT_8	ucId;		/*              */
	UINT_16 u2Length;	/*        */
	UINT_8	ucGOIntent;	/*                    */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_GO_INTENT_T, *P_P2P_ATTRI_GO_INTENT_T;


/*                                             */
typedef struct _P2P_ATTRI_CFG_TIMEOUT_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_8	ucGOCfgTimeout;		/*                          */
	UINT_8	ucClientCfgTimeout;	/*                              */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_CFG_TIMEOUT_T, *P_P2P_ATTRI_CFG_TIMEOUT_T;


/*                                      */
typedef struct _P2P_ATTRI_LISTEN_CHANNEL_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_8	aucCountryString[3];	/*                */
	UINT_8	ucOperatingClass;	/*                                                       */
	UINT_8	ucChannelNumber;	/*                */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_LISTEN_CHANNEL_T, *P_P2P_ATTRI_LISTEN_CHANNEL_T;


/*                                       */
typedef struct _P2P_ATTRI_GROUP_BSSID_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_8	aucBssid[MAC_ADDR_LEN];	/*                 */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_GROUP_BSSID_T, *P_P2P_ATTRI_GROUP_BSSID_T;





/*                                                       */
typedef struct _P2P_ATTRI_INTENDED_IF_ADDR_T {
	UINT_8	ucId;				/*              */
	UINT_16 u2Length;			/*        */
	UINT_8	aucIfAddr[MAC_ADDR_LEN];	/*                       */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_INTENDED_IF_ADDR_T, *P_P2P_ATTRI_INTENDED_IF_ADDR_T;


/*                                          */
typedef struct _P2P_ATTRI_MANAGEABILITY_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_8	ucManageability;	/*                          */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_MANAGEABILITY_T, *P_P2P_ATTRI_MANAGEABILITY_T;


/*                                     */
typedef struct _P2P_ATTRI_CHANNEL_LIST_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_8	aucCountryString[3];	/*                */
	UINT_8	aucChannelEntry[1];	/*                    */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_CHANNEL_T, *P_P2P_ATTRI_CHANNEL_T;




/*                                          */
typedef struct _P2P_ATTRI_NOA_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_8	ucIndex;		/*       */
	UINT_8	ucCTWOppPSParam;	/*                               */
	UINT_8	aucNoADesc[1];		/*                */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_NOA_T, *P_P2P_ATTRI_NOA_T;

typedef struct _NOA_DESCRIPTOR_T {
	UINT_8	ucCountType;	/*            */
	UINT_32 u4Duration;	/*          */
	UINT_32 u4Interval;	/*          */
	UINT_32 u4StartTime;	/*            */
} __KAL_ATTRIB_PACKED__ NOA_DESCRIPTOR_T, *P_NOA_DESCRIPTOR_T;

typedef struct _P2P_ATTRI_DEV_INFO_T {
	UINT_8		ucId;				/*              */
	UINT_16		u2Length;			/*        */
	UINT_8		aucDevAddr[MAC_ADDR_LEN];	/*                    */
	UINT_16		u2ConfigMethodsBE;		/*               */
	DEVICE_TYPE_T	rPrimaryDevTypeBE;		/*                     */
	UINT_8		ucNumOfSecondaryDevType;	/*                                  */
	DEVICE_TYPE_T	arSecondaryDevTypeListBE[1];	/*                            */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_DEV_INFO_T, *P_P2P_ATTRI_DEV_INFO_T;

/*                                                */
typedef struct _DEVICE_NAME_TLV_T {
	UINT_16 u2Id;		/*                    */
	UINT_16 u2Length;	/*             */
	UINT_8	aucName[32]; /*             */ /*              */
} __KAL_ATTRIB_PACKED__ DEVICE_NAME_TLV_T, *P_DEVICE_NAME_TLV_T;


/*                                       */
typedef struct _P2P_CLIENT_INFO_DESC_T {
	UINT_8		ucLength;			/*        */
	UINT_8		aucDevAddr[MAC_ADDR_LEN];	/*                    */
	UINT_8		aucIfAddr[MAC_ADDR_LEN];	/*                       */
	UINT_8		ucDeviceCap;			/*                          */
	UINT_16		u2ConfigMethodsBE;		/*               */
	DEVICE_TYPE_T	rPrimaryDevTypeBE;		/*                     */
	UINT_8		ucNumOfSecondaryDevType;	/*                                  */
	DEVICE_TYPE_T	arSecondaryDevTypeListBE[1];	/*                            */
} __KAL_ATTRIB_PACKED__ P2P_CLIENT_INFO_DESC_T, *P_P2P_CLIENT_INFO_DESC_T;

typedef struct _P2P_ATTRI_GROUP_INFO_T {
	UINT_8			ucId;			/*              */
	UINT_16			u2Length;		/*        */
	P2P_CLIENT_INFO_DESC_T	arClientDesc[1];	/*                             */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_GROUP_INFO_T, *P_P2P_ATTRI_GROUP_INFO_T;


/*                                     */
typedef struct _P2P_ATTRI_GROUP_ID_T {
	UINT_8	ucId;				/*              */
	UINT_16 u2Length;			/*        */
	UINT_8	aucDevAddr[MAC_ADDR_LEN];	/*                    */
	UINT_8	aucSSID[ELEM_MAX_LEN_SSID];	/*      */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_GROUP_ID_T, *P_P2P_ATTRI_GROUP_ID_T;


/*                                      */
typedef struct _P2P_ATTRI_INTERFACE_T {
	UINT_8	ucId;				/*              */
	UINT_16 u2Length;			/*        */
	UINT_8	aucDevAddr[MAC_ADDR_LEN];	/*                    */
	UINT_8	ucIfAddrCount;			/*                             */
	UINT_8	aucIfAddrList[MAC_ADDR_LEN];	/*                            */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_INTERFACE_T, *P_P2P_ATTRI_INTERFACE_T;


/*                                          */
typedef struct _P2P_ATTRI_OPERATING_CHANNEL_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_8	aucCountryString[3];	/*                */
	UINT_8	ucOperatingClass;	/*                                                       */
	UINT_8	ucChannelNumber;	/*                */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_OPERATING_CHANNEL_T, *P_P2P_ATTRI_OPERATING_CHANNEL_T;

/*                                         */
typedef struct _P2P_ATTRI_INVITATION_FLAG_T {
	UINT_8	ucId;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_8	ucInviteFlagsBitmap;	/*                         */
} __KAL_ATTRIB_PACKED__ P2P_ATTRI_INVITATION_FLAG_T, *P_P2P_ATTRI_INVITATION_FLAG_T;





/*                  */
typedef struct _WSC_ATTRI_VERSION_T {
	UINT_16 u2Id;		/*              */
	UINT_16 u2Length;	/*        */
	UINT_8	ucVersion;	/*                    */
} __KAL_ATTRIB_PACKED__ WSC_ATTRI_VERSION_T, *P_WSC_ATTRI_VERSION_T;

typedef struct _WSC_ATTRI_DEVICE_PASSWORD_ID_T {
	UINT_16 u2Id;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_16 u2DevPasswordId;	/*                    */
} __KAL_ATTRIB_PACKED__ WSC_ATTRI_DEVICE_PASSWORD_ID_T, *P_WSC_ATTRI_DEVICE_PASSWORD_ID_T;


typedef struct _WSC_ATTRI_CONFIGURATION_METHOD_T {
	UINT_16 u2Id;			/*              */
	UINT_16 u2Length;		/*        */
	UINT_16 u2ConfigMethods;	/*                   */
} __KAL_ATTRIB_PACKED__ WSC_ATTRI_CONFIGURATION_METHOD_T, *P_WSC_ATTRI_CONFIGURATION_METHOD_T;



#if defined(WINDOWS_DDK) || defined(WINDOWS_CE)
#pragma pack()
#endif


/*                                                                        */
typedef UINT_32 (*PFN_APPEND_ATTRI_FUNC) (P_ADAPTER_T, BOOLEAN, PUINT_16, PUINT_8, UINT_16);

typedef VOID (*PFN_HANDLE_ATTRI_FUNC) (P_SW_RFB_T, P_P2P_ATTRIBUTE_T);

typedef VOID (*PFN_VERIFY_ATTRI_FUNC) (P_SW_RFB_T, P_P2P_ATTRIBUTE_T, PUINT_16);

typedef UINT_32 (*PFN_CALCULATE_VAR_ATTRI_LEN_FUNC) (P_ADAPTER_T, P_STA_RECORD_T);



typedef enum _ENUM_CONFIG_METHOD_SEL {
	ENUM_CONFIG_METHOD_SEL_AUTO,
	ENUM_CONFIG_METHOD_SEL_USER,
	ENUM_CONFIG_METHOD_SEL_NUM
} ENUM_CONFIG_METHOD_SEL, *P_ENUM_CONFIG_METHOD_SEL;

typedef enum _ENUM_P2P_FORMATION_POLICY {
	ENUM_P2P_FORMATION_POLICY_AUTO = 0,
	ENUM_P2P_FORMATION_POLICY_PASSIVE,	/*                                                               */
	ENUM_P2P_FORMATION_POLICY_NUM
} ENUM_P2P_FORMATION_POLICY, P_ENUM_P2P_FORMATION_POLICY;

typedef enum _ENUM_P2P_INVITATION_POLICY {
	ENUM_P2P_INVITATION_POLICY_USER = 0,
	ENUM_P2P_INVITATION_POLICY_ACCEPT_FIRST,
	ENUM_P2P_INVITATION_POLICY_DENY_ALL,
	ENUM_P2P_INVITATION_POLICY_NUM
} ENUM_P2P_INVITATION_POLICY, P_ENUM_P2P_INVITATION_POLICY;

/*                                                                       */ */
/*                                             */ */
struct _P2P_CONNECTION_SETTINGS_T {
	UINT_8				ucDevNameLen;
	UINT_8				aucDevName[WPS_ATTRI_MAX_LEN_DEVICE_NAME];

	DEVICE_TYPE_T			rPrimaryDevTypeBE;

	ENUM_P2P_FORMATION_POLICY	eFormationPolicy;	/*                   */

	/*                                            */
	UINT_16				u2ConfigMethodsSupport;	/*                           
                                           
         */
	ENUM_CONFIG_METHOD_SEL		eConfigMethodSelType;
	UINT_16				u2TargetConfigMethod;	/*                                            */
	UINT_16				u2LocalConfigMethod;	/*                             */
	BOOLEAN				fgIsPasswordIDRdy;
	/*                                            */

	UINT_8				ucClientConfigTimeout;
	UINT_8				ucGoConfigTimeout;

	UINT_8				ucSecondaryDevTypeCount;
#if P2P_MAX_SUPPORTED_SEC_DEV_TYPE_COUNT
	DEVICE_TYPE_T			arSecondaryDevTypeBE[P2P_MAX_SUPPORTED_SEC_DEV_TYPE_COUNT];
#endif


#if 0
	UINT_8				ucRfChannelListCount;
#if P2P_MAX_SUPPORTED_CHANNEL_LIST_COUNT
	UINT_8				aucChannelList[P2P_MAX_SUPPORTED_CHANNEL_LIST_COUNT];	/*                  
                                 
                       */

#endif
#else
	UINT_8				ucRfChannelListSize;
#if P2P_MAX_SUPPORTED_CHANNEL_LIST_SIZE
	UINT_8				aucChannelEntriesField[P2P_MAX_SUPPORTED_CHANNEL_LIST_SIZE];
#endif
#endif

	/*           */
	UINT_8				ucTieBreaker;
	UINT_8				ucGoIntent;

	/*                       */
	BOOLEAN				fgSupportServiceDiscovery;
	BOOLEAN				fgSupportClientDiscoverability;
	BOOLEAN				fgSupportConcurrentOperation;
	BOOLEAN				fgSupportInfraManaged;
	BOOLEAN				fgSupportInvitationProcedure;

	/*                      */
	BOOLEAN				fgSupportPersistentP2PGroup;
	BOOLEAN				fgSupportIntraBSSDistribution;
	BOOLEAN				fgSupportCrossConnection;
	BOOLEAN				fgSupportPersistentReconnect;

	BOOLEAN				fgP2pGroupLimit;

	BOOLEAN				fgSupportOppPS;
	UINT_16				u2CTWindow;

	BOOLEAN				fgIsScanReqIssued;
	BOOLEAN				fgIsServiceDiscoverIssued;


	/*                                                           */

	/*                              */
	BOOLEAN				fgIsDevId;
	BOOLEAN				fgIsDevType;

	/*                                  */
	ENUM_PARAM_AUTH_MODE_T		eAuthMode;

	/*     
                                                         
                                                                
  */
	UINT_8				aucSSID[ELEM_MAX_LEN_SSID];
	UINT_8				ucSSIDLen;

	/*                              */
	UINT_8				ucOperatingChnl;
	ENUM_BAND_T			eBand;

	/*                           */
	UINT_8				ucListenChnl;

	/*                                   */
	UINT_8				aucTargetDevAddr[MAC_ADDR_LEN];	/*                                             
                             */

#if CFG_ENABLE_WIFI_DIRECT
	P_P2P_DEVICE_DESC_T		prTargetP2pDesc;
#endif

	UINT_8				ucLastStatus;	/*                                                        
                */


#if !CFG_DISABLE_DELAY_PROVISION_DISCOVERY
	UINT_8				ucLastDialogToken;
	UINT_8				aucIndicateDevAddr[MAC_ADDR_LEN];
#endif

#if 0
	UINT_8				ucTargetRfChannelListCount;
#if P2P_MAX_SUPPORTED_CHANNEL_LIST_COUNT
	UINT_8				aucTargetChannelList[P2P_MAX_SUPPORTED_CHANNEL_LIST_COUNT];	/*        
                        
                         
                             
                  */
#endif
#endif
};




typedef enum _ENUM_P2P_IOCTL_T {
	P2P_IOCTL_IDLE = 0,
	P2P_IOCTL_DEV_DISCOVER,
	P2P_IOCTL_INVITATION_REQ,
	P2P_IOCTL_SERV_DISCOVER,
	P2P_IOCTL_WAITING,
	P2P_IOCTL_NUM
} ENUM_P2P_IOCTL_T;



/*                                                              */
typedef enum _ENUM_SERVICE_TX_TYPE_T {
	ENUM_SERVICE_TX_TYPE_BY_DA,
	ENUM_SERVICE_TX_TYPE_BY_CHNL,
	ENUM_SERVICE_TX_TYPE_NUM
} ENUM_SERVICE_TX_TYPE_T;


typedef struct _SERVICE_DISCOVERY_FRAME_DATA_T {
	QUE_ENTRY_T		rQueueEntry;
	P_MSDU_INFO_T		prSDFrame;
	ENUM_SERVICE_TX_TYPE_T	eServiceType;
	UINT_8			ucSeqNum;
	union {
		UINT_8	ucChannelNum;
		UINT_8	aucPeerAddr[MAC_ADDR_LEN];
	} uTypeData;
	BOOLEAN			fgIsTxDoneIndicate;
} SERVICE_DISCOVERY_FRAME_DATA_T, *P_SERVICE_DISCOVERY_FRAME_DATA_T;




struct _P2P_FSM_INFO_T_DEPRECATED {
	/*               */
	ENUM_P2P_STATE_T	eCurrentState;

	/*         */
	BOOLEAN			fgIsChannelRequested;










	ENUM_P2P_STATE_T			ePreviousState;

	ENUM_P2P_STATE_T			eReturnState;	/*                                                
                 */

	UINT_8					aucTargetIfAddr[PARAM_MAC_ADDR_LEN];
	P_BSS_DESC_T				prTargetBss;	/*                                                 
                    */

	P_STA_RECORD_T				prTargetStaRec;

	BOOLEAN					fgIsRsponseProbe;	/*                                       
                          */

	/*                                       */
	UINT_8					ucSeqNumOfReqMsg;	/*                                   */

	/*                   */
	UINT_8					ucSeqNumOfChReq;/*                                   */


	UINT_8					ucSeqNumOfScnMsg;	/*                                    */
	UINT_8					ucSeqNumOfCancelMsg;

	UINT_8					ucDialogToken;
	UINT_8					ucRxDialogToken;

	/*       */
	TIMER_T					rDeviceDiscoverTimer;	/*                                            
                              */
	TIMER_T					rOperationListenTimer;	/*                                         */
	TIMER_T					rFSMTimer;		/*                                             
           */

	TIMER_T					rRejoinTimer;		/*                                             
           */


	/*                               */
	BOOLEAN					fgIsConnectionRequested;

	/*                */
	ENUM_P2P_IOCTL_T			eP2pIOCTL;

	UINT_8					ucAvailableAuthTypes;	/*                                */

	/*                                 */
	QUE_T					rQueueGASRx;	/*                         */
	QUE_T					rQueueGASTx;	/*                  */
	P_SERVICE_DISCOVERY_FRAME_DATA_T	prSDRequest;
	UINT_8					ucVersionNum;	/*                                                */
	UINT_8					ucGlobalSeqNum;	/*                                  */
	/*                                 */

	/*                                 */
	UINT_8					aucTargetGroupID[PARAM_MAC_ADDR_LEN];
	UINT_16					u2TargetGroupSsidLen;
	UINT_8					aucTargetSsid[32];
	UINT_8					aucSearchingP2pDevice[PARAM_MAC_ADDR_LEN];
	UINT_8					ucDLToken;
	/*                                  */

	/*                         */
	UINT_32					u4Flags;

	/*                                 */
	BOOLEAN					fgIsApMode;


	/*                                  */
	ENUM_P2P_INVITATION_POLICY		eInvitationRspPolicy;
	/*                                  */
};



struct _P2P_SPECIFIC_BSS_INFO_T {
	/*                                  */
	UINT_16					u2SmallestAID;
	UINT_16					u2LargestAID;
	UINT_8					ucBitmapCtrl;
	/*                                                                           */

	/*                 */
	BOOLEAN					fgEnableOppPS;
	UINT_16					u2CTWindow;

	/*               */
	UINT_8					ucNoAIndex;
	UINT_8					ucNoATimingCount;	/*                      */
	NOA_TIMING_T				arNoATiming[P2P_MAXIMUM_NOA_COUNT];

	BOOLEAN					fgIsNoaAttrExisted;

	/*                */
	UINT_8					ucRegClass;		/*                               */
	UINT_8					ucListenChannel;	/*                                            
                           */

	UINT_8					ucPreferredChannel;	/*                                            
                                            */
	ENUM_CHNL_EXT_T				eRfSco;
	ENUM_BAND_T				eRfBand;

	/*                        */
	UINT_16					u2AvailabilityPeriod;
	UINT_16					u2AvailabilityInterval;


#if 0				/*                                               */
	UINT_16					u2IELenForBCN;
	UINT_8					aucBeaconIECache[P2P_MAXIMUM_ATTRIBUTES_CACHE_SIZE +
								 WPS_MAXIMUM_ATTRIBUTES_CACHE_SIZE];

/*                                             */
/*                                                                                                                    */

	UINT_16					u2IELenForAssocRsp;
	UINT_8					aucAssocRspIECache[P2P_MAXIMUM_ATTRIBUTES_CACHE_SIZE +
								   WPS_MAXIMUM_ATTRIBUTES_CACHE_SIZE];

#else
	UINT_16					u2AttributeLen;
	UINT_8					aucAttributesCache[P2P_MAXIMUM_ATTRIBUTES_CACHE_SIZE];

	UINT_16					u2WscAttributeLen;
	UINT_8					aucWscAttributesCache[WPS_MAXIMUM_ATTRIBUTES_CACHE_SIZE];
#endif
	UINT_8					aucGroupID[MAC_ADDR_LEN];
	UINT_16					u2GroupSsidLen;
	UINT_8					aucGroupSsid[ELEM_MAX_LEN_SSID];

	PARAM_CUSTOM_NOA_PARAM_STRUC_T		rNoaParam;
	PARAM_CUSTOM_OPPPS_PARAM_STRUC_T	rOppPsParam;

#if 0				/*           */
	UINT_16					u2WpaIeLen;
	UINT_8					aucWpaIeBuffer[ELEM_HDR_LEN + ELEM_MAX_LEN_WPA];
#endif
};







typedef struct _MSG_P2P_DEVICE_DISCOVER_T {
	MSG_HDR_T		rMsgHdr;		/*                          */
	UINT_32			u4DevDiscoverTime;	/*                                  */
	BOOLEAN			fgIsSpecificType;
#if CFG_ENABLE_WIFI_DIRECT
	P2P_DEVICE_TYPE_T	rTargetDeviceType;
#endif
	UINT_8			aucTargetDeviceID[MAC_ADDR_LEN];
} MSG_P2P_DEVICE_DISCOVER_T, *P_MSG_P2P_DEVICE_DISCOVER_T;



typedef struct _MSG_P2P_INVITATION_REQUEST_T {
	MSG_HDR_T	rMsgHdr;			/*                          */
	UINT_8		aucDeviceID[MAC_ADDR_LEN];	/*                                 */
} MSG_P2P_INVITATION_REQUEST_T, *P_MSG_P2P_INVITATION_REQUEST_T;

typedef struct _MSG_P2P_FUNCTION_SWITCH_T {
	MSG_HDR_T	rMsgHdr;/*                          */
	BOOLEAN		fgIsFuncOn;
} MSG_P2P_FUNCTION_SWITCH_T, *P_MSG_P2P_FUNCTION_SWITCH_T;

typedef struct _MSG_P2P_SERVICE_DISCOVERY_REQUEST_T {
	MSG_HDR_T	rMsgHdr;/*                          */
	UINT_8		aucDeviceID[MAC_ADDR_LEN];
	BOOLEAN		fgNeedTxDoneIndicate;
	UINT_8		ucSeqNum;
} MSG_P2P_SERVICE_DISCOVERY_REQUEST_T, *P_MSG_P2P_SERVICE_DISCOVERY_REQUEST_T;



/*                                                                              
                                                   
                                                                                 
 */

/*                                                                              
                                                    
                                                                                 
 */

/*                                                                              
                                              
                                                                                 
 */





/*                                                                              
                                                             
                                                                                 
 */

/*                     */
VOID
p2pStateInit_LISTEN(IN P_ADAPTER_T prAdapter, IN P_P2P_FSM_INFO_T prP2pFsmInfo, IN P_P2P_SPECIFIC_BSS_INFO_T
		    prSP2pBssInfo, IN UINT_8 ucListenChannel);

VOID p2pStateAbort_LISTEN(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsChannelExtenstion);

VOID p2pStateAbort_SEARCH_SCAN(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsChannelExtenstion);

VOID p2pStateAbort_GO_OPERATION(IN P_ADAPTER_T prAdapter);

VOID p2pStateAbort_GC_OPERATION(IN P_ADAPTER_T prAdapter);

VOID
p2pStateInit_CONFIGURATION(IN P_ADAPTER_T prAdapter, IN P_P2P_FSM_INFO_T prP2pFsmInfo, IN P_BSS_INFO_T prP2pBssInfo, IN
			   P_P2P_SPECIFIC_BSS_INFO_T prP2pSpecBssInfo);

VOID p2pStateAbort_CONFIGURATION(IN P_ADAPTER_T prAdapter);

VOID p2pStateInit_JOIN(IN P_ADAPTER_T prAdapter);

VOID p2pStateAbort_JOIN(IN P_ADAPTER_T prAdapter);

/*                           */


VOID p2pFuncInitGO(IN P_ADAPTER_T prAdapter);





VOID
p2pFuncDisconnect(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec, IN BOOLEAN fgSendDeauth, IN UINT_16
		  u2ReasonCode);



VOID p2pFuncRunEventProvisioningComplete(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

WLAN_STATUS
p2pFuncSetGroupID(IN P_ADAPTER_T prAdapter, IN PUINT_8 pucGroupID, IN PUINT_8 pucSsid, IN UINT_8 ucSsidLen);


WLAN_STATUS
p2pFuncSendDeviceDiscoverabilityReqFrame(IN P_ADAPTER_T prAdapter, IN UINT_8 aucDestAddr[], IN UINT_8 ucDialogToken);

WLAN_STATUS
p2pFuncSendDeviceDiscoverabilityRspFrame(IN P_ADAPTER_T prAdapter, IN UINT_8 aucDestAddr[], IN UINT_8 ucDialogToken);


UINT_8 p2pFuncGetVersionNumOfSD(IN P_ADAPTER_T prAdapter);

/*                     */
VOID p2pFsmRunEventConnectionRequest(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventDeviceDiscoveryRequest(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventDeviceDiscoveryAbort(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventRxGroupNegotiationReqFrame(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

WLAN_STATUS
p2pFsmRunEventGroupNegotiationRequestTxDone(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo, IN
					    ENUM_TX_RESULT_CODE_T rTxDoneStatus);

WLAN_STATUS
p2pFsmRunEventGroupNegotiationResponseTxDone(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo, IN
					     ENUM_TX_RESULT_CODE_T rTxDoneStatus);

WLAN_STATUS
p2pFsmRunEventGroupNegotiationConfirmTxDone(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo, IN
					    ENUM_TX_RESULT_CODE_T rTxDoneStatus);

WLAN_STATUS
p2pFsmRunEventProvisionDiscoveryRequestTxDone(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo, IN
					      ENUM_TX_RESULT_CODE_T rTxDoneStatus);

WLAN_STATUS
p2pFsmRunEventProvisionDiscoveryResponseTxDone(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo, IN
					       ENUM_TX_RESULT_CODE_T rTxDoneStatus);


WLAN_STATUS
p2pFsmRunEventInvitationRequestTxDone(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo, IN ENUM_TX_RESULT_CODE_T
				      rTxDoneStatus);



VOID
p2pFsmRunEventRxDeauthentication(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec, IN P_SW_RFB_T prSwRfb);

VOID
p2pFsmRunEventRxDisassociation(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec, IN P_SW_RFB_T prSwRfb);

VOID p2pFsmRunEventBeaconTimeout(IN P_ADAPTER_T prAdapter, IN P_BSS_INFO_T prP2pBssInfo);



WLAN_STATUS
p2pFsmRunEventDeauthTxDone(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo, IN ENUM_TX_RESULT_CODE_T
			   rTxDoneStatus);


#if 1
#endif


/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                 */


VOID p2pFsmRunEventConnectionAbort(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventConnectionTrigger(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);


VOID p2pFsmRunEventP2PFunctionSwitch(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventChGrant(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventJoinComplete(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID p2pFsmRunEventConnectionPause(IN P_ADAPTER_T prAdapter, IN P_MSG_HDR_T prMsgHdr);

VOID
p2pIndicationOfMediaStateToHost(IN P_ADAPTER_T prAdapter, IN ENUM_PARAM_MEDIA_STATE_T eConnectionState,	IN UINT_8
				aucTargetAddr[]);

VOID
p2pUpdateBssInfoForJOIN(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec, IN P_SW_RFB_T prAssocRspSwRfb);

/*                                 */


VOID p2pFsmInit(IN P_ADAPTER_T prAdapter);

VOID p2pFsmUninit(IN P_ADAPTER_T prAdapter);

VOID p2pFsmSteps(IN P_ADAPTER_T prAdapter, IN ENUM_P2P_STATE_T eNextState);

VOID p2pStartGO(IN P_ADAPTER_T prAdapter);

VOID p2pAssignSsid(IN PUINT_8 pucSsid, IN PUINT_8 pucSsidLen);



VOID p2pFsmRunEventIOReqTimeout(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Param);

VOID p2pFsmRunEventSearchPeriodTimeout(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Param);

VOID p2pFsmRunEventFsmTimeout(IN P_ADAPTER_T prAdapter, IN ULONG ulParamPtr);

VOID p2pFsmRunEventRejoinTimeout(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Parm);




/*                                                     */

/*                                                     */


#if CFG_TEST_WIFI_DIRECT_GO
VOID p2pTest(IN P_ADAPTER_T prAdapter);
#endif				/*                         */




VOID p2pGenerateP2P_IEForBeacon(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);

VOID p2pGenerateP2P_IEForAssocReq(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);

VOID p2pGenerateP2P_IEForAssocRsp(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);


VOID
p2pGenerateP2P_IEForProbeReq(IN P_ADAPTER_T prAdapter, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf, IN UINT_16 u2BufSize);




UINT_32
p2pCalculateP2P_IELenForBeacon(IN P_ADAPTER_T prAdapter, IN UINT_8 ucBssIndex, IN P_STA_RECORD_T prStaRec);

UINT_32
p2pCalculateP2P_IELenForAssocRsp(IN P_ADAPTER_T prAdapter, IN UINT_8 ucBssIndex, IN P_STA_RECORD_T prStaRec);


UINT_32
p2pCalculateP2P_IELenForProbeReq(IN P_ADAPTER_T prAdapter, IN UINT_8 ucBssIndex, IN P_STA_RECORD_T prStaRec);



VOID p2pGenerateWSC_IEForProbeResp(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfo);

VOID
p2pGenerateWSC_IEForProbeReq(IN P_ADAPTER_T prAdapter, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf, IN UINT_16 u2BufSize);



UINT_16 p2pCalculateWSC_IELenForProbeReq(IN P_ADAPTER_T prAdapter, IN UINT_8 ucBssIndex);

UINT_32
p2pCalculateWSC_IELenForProbeResp(IN P_ADAPTER_T prAdapter, IN UINT_8 ucBssIndex, IN P_STA_RECORD_T prStaRec);

UINT_32
p2pAppendAttriStatus(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf, IN
		     UINT_16 u2BufSize);



UINT_32
p2pAppendAttriCapability(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf,
			 IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriGoIntent(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf, IN
		       UINT_16 u2BufSize);

UINT_32
p2pAppendAttriCfgTimeout(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf,
			 IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriGroupBssid(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf,
			 IN UINT_16 u2BufSize);


UINT_32
p2pAppendAttriDeviceIDForBeacon(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8
				pucBuf, IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriDeviceIDForProbeReq(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8
				  pucBuf, IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriDeviceIDForDeviceDiscoveryReq(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset,
					    IN PUINT_8 pucBuf, IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriListenChannel(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8
			    pucBuf, IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriIntendP2pIfAddr(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8
			      pucBuf, IN UINT_16 u2BufSize);


UINT_32
p2pAppendAttriChannelList(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf,
			  IN UINT_16 u2BufSize);

UINT_32 p2pCalculateAttriLenChannelList(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

UINT_32
p2pAppendAttriNoA(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf, IN
		  UINT_16 u2BufSize);

UINT_32
p2pAppendAttriDeviceInfo(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf,
			 IN UINT_16 u2BufSize);

UINT_32 p2pCalculateAttriLenDeviceInfo(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

UINT_32
p2pAppendAttriGroupInfo(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf,
			IN UINT_16 u2BufSize);

UINT_32 p2pCalculateAttriLenGroupInfo(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);


UINT_32
p2pAppendAttriP2pGroupID(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf,
			 IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriOperatingChannel(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8
			       pucBuf, IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriInvitationFlag(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8
			     pucBuf, IN UINT_16 u2BufSize);


VOID
p2pGenerateWscIE(IN P_ADAPTER_T prAdapter, IN UINT_8 ucOuiType, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN
		 PUINT_8 pucBuf, IN UINT_16 u2BufSize, IN APPEND_VAR_ATTRI_ENTRY_T arAppendAttriTable[], IN UINT_32
		 u4AttriTableSize);

UINT_32
p2pAppendAttriWSCConfigMethod(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8
			      pucBuf, IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriWSCVersion(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN PUINT_8 pucBuf,
			 IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriWSCGONegReqDevPasswordId(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN
				       PUINT_8 pucBuf, IN UINT_16 u2BufSize);

UINT_32
p2pAppendAttriWSCGONegRspDevPasswordId(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgIsAssocFrame, IN PUINT_16 pu2Offset, IN
				       PUINT_8 pucBuf, IN UINT_16 u2BufSize);

WLAN_STATUS
p2pGetWscAttriList(IN P_ADAPTER_T prAdapter, IN UINT_8 ucOuiType, IN PUINT_8 pucIE, IN UINT_16 u2IELength, OUT PPUINT_8
		   ppucAttriList, OUT PUINT_16 pu2AttriListLen);

WLAN_STATUS
p2pGetAttriList(IN P_ADAPTER_T prAdapter, IN UINT_8 ucOuiType, IN PUINT_8 pucIE, IN UINT_16 u2IELength, OUT PPUINT_8
		ppucAttriList, OUT PUINT_16 pu2AttriListLen);

VOID p2pRunEventAAATxFail(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

WLAN_STATUS p2pRunEventAAASuccess(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);


WLAN_STATUS p2pRunEventAAAComplete(IN P_ADAPTER_T prAdapter, IN P_STA_RECORD_T prStaRec);

WLAN_STATUS p2pSendProbeResponseFrame(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

BOOLEAN p2pFsmRunEventRxProbeRequestFrame(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

VOID
p2pFsmRunEventRxProbeResponseFrame(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb, IN P_BSS_DESC_T prBssDesc);

WLAN_STATUS p2pRxPublicActionFrame(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

WLAN_STATUS p2pRxActionFrame(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

VOID p2pFsmRunEventRxGroupNegotiationRspFrame(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);

VOID p2pFsmRunEventRxGroupNegotiationCfmFrame(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfb);


#if 0				/*      */
BOOLEAN scanMatchFilterOfP2P(IN P_SW_RFB_T prSWRfb, IN PP_BSS_DESC_T pprBssDesc);
#endif				/*      */

VOID
p2pProcessEvent_UpdateNOAParam(IN P_ADAPTER_T prAdapter, UINT_8 ucNetTypeIndex, P_EVENT_UPDATE_NOA_PARAMS_T
			       prEventUpdateNoaParam);

VOID p2pFuncCompleteIOCTL(IN P_ADAPTER_T prAdapter, IN WLAN_STATUS rWlanStatus);

/*                                                                              
                                                 
                                                                                 
 */
#ifndef _lint
/*                                                                               
                                                     
                                                        
 */
static __KAL_INLINE__ VOID p2pDataTypeCheck(VOID)
{
	DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_P2P_T) == (2 + 4 + 1));	/*            */
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRIBUTE_T) == (3 + 1));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_STATUS_T) == (3 + 1));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_REASON_T) == (3 + 1));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_CAPABILITY_T) == (3 + 2));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_DEV_ID_T) == (3 + 6));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_GO_INTENT_T) == (3 + 1));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_CFG_TIMEOUT_T) == (3 + 2));
#if CID52_53_54
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_LISTEN_CHANNEL_T) == (3 + 5));
#else
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_LISTEN_CHANNEL_T) == (3 + 5));
#endif
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_GROUP_BSSID_T) == (3 + 6));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_EXT_LISTEN_TIMING_T) == (3 + 4));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_INTENDED_IF_ADDR_T) == (3 + 6));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_MANAGEABILITY_T) == (3 + 1));

	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_CHANNEL_T) == (3 + 4));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(CHANNEL_ENTRY_FIELD_T) == 3);
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_NOA_T) == (3 + 3));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(NOA_DESCRIPTOR_T) == 13);
	DATA_STRUC_INSPECTING_ASSERT(sizeof(DEVICE_TYPE_T) == 8);
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_DEV_INFO_T) == (3 + 6 + 2 + 8 + 1 + 8));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(DEVICE_NAME_TLV_T) == (4 + 32));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_CLIENT_INFO_DESC_T) ==
				     (1 + 6 + 6 + 1 + 2 + 8 + 1 + 8));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_GROUP_INFO_T) ==
				     (3 + (1 + 6 + 6 + 1 + 2 + 8 + 1 + 8)));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_GROUP_ID_T) == (3 + 38));
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_INTERFACE_T) == (3 + 13));
#if CID52_53_54
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_OPERATING_CHANNEL_T) == (3 + 5));
#else
	DATA_STRUC_INSPECTING_ASSERT(sizeof(P2P_ATTRI_OPERATING_CHANNEL_T) == (3 + 5));
#endif


	return;
}
#endif				/*       */

#endif				/*            */
