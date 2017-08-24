/*
            
*/

/*                  
          

*/



/*
                 
  
                   
                                                                                                                                  
                                                                                                                                    
  
                   
                                                                                                              
                            
                                                                  

  
                   
                                                                                                                                
                                                                                                                                             
  
                   
                                                                                                                                                               
                                                                                     
  
                      
                                                                           
                                     
  
                   
                                                                                                                                   
                                                                
  
                        
                                                                                          
                     
  
                   
       
                                                             
  
                         
       
                                                                     
  
                         
       
                                                 
  
                   
       
                             
  
                   
       
                                              
  
                        
       
                                                                               
  
                        
       
                                             
                                                                                                               
  
                        
       
                                         
  
                        
       
                                       
  
                        
  
                                                                 
  
                        
  
                                            
  
                        
  
                                                     
  
                   
  
                                                      
                                                                         
  
                   
  
                                                      
                                                              
  
                   
  
                                                      
                           
  
                   
  
                                                                                                     
                                                                            
                                                                                                                                   
  
                   
  
                                                                               
  
                   
                                                    
                                      
  
                   
                                                    
                                                                  
  
                   
                                                    
                                               
                                                            
  
                        
                                                    
                                         
  
                   
                                                    
                         
  
                   
                                                    
                                             
  
                   
                                                    
                                                                 
  
                   
                                                    
                      
  
                   
                                                    
                                     
  
                                                     
  
                   
                                                    
                                                    
  
                         
                                                               
                                                 
  
                         
                                                          
  
                              
  
                         
                                                          
                 
  
                         
                                                                       
  
                                                                          
  
                         
                                                           
                                                               
  
                   
                                                                           
                                                                                                            
  
                         
                                                           
                                                                                                    
  
                         
                                                           
                                                                             
  
                   
                                                                          
                                        
  
                         
                                                               
                                                                
  
                                                                                             
  
                      
                                                                           
                              
  
                      
                                                                           
                                    
  
                      
                                                                
                                                         
  
                       
                                                                
                                                                        
  
                       
                                                                
                                          
  
                       
                                                                
                                                             
  
                       
                                                                           
                                       
  
                       
                                                                           
                                                
  
                       
                                                                
                                  
  
                      
                                                                
                                          
  
                       
                                                                
                               
  
*/

#ifndef _SCAN_H
#define _SCAN_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
/*                                   */
#define SCN_MAX_BUFFER_SIZE                 (CFG_MAX_NUM_BSS_LIST * ALIGN_4(sizeof(BSS_DESC_T)))

#define SCN_RM_POLICY_EXCLUDE_CONNECTED     BIT(0) //                                             
#define SCN_RM_POLICY_TIMEOUT               BIT(1) //                       
#define SCN_RM_POLICY_OLDEST_HIDDEN         BIT(2) //                                       
#define SCN_RM_POLICY_SMART_WEAKEST         BIT(3) /*                                              
                                                                                                                       
                                                                                  
                                                    */
#define SCN_RM_POLICY_ENTIRE                BIT(4) //                          

#define SCN_BSS_DESC_SAME_SSID_THRESHOLD    3 /*                                      
                                                                                                
                                                                                       
                                               */

/*                                                                                 */
#define SCN_BSS_DESC_REMOVE_TIMEOUT_SEC     15 //        
                                              /*                                
                                                                                                 
                                               */




#define SCN_PROBE_DELAY_MSEC                0

#define SCN_ADHOC_BSS_DESC_TIMEOUT_SEC      5 //        

/*                                                                            */
/*                                                                            */
/*                                                                            */
#define SCAN_REQ_SSID_WILDCARD              BIT(0)
#define SCAN_REQ_SSID_P2P_WILDCARD          BIT(1)
#define SCAN_REQ_SSID_SPECIFIED             BIT(2)


/*                                                                            */
/*                                                                            */
/*                                                                            */
#define SCN_SSID_MAX_NUM                        4


/*                                                                              
                                                 
                                                                                
*/
typedef enum _ENUM_SCAN_TYPE_T {
    SCAN_TYPE_PASSIVE_SCAN = 0,
    SCAN_TYPE_ACTIVE_SCAN,
    SCAN_TYPE_NUM
} ENUM_SCAN_TYPE_T, *P_ENUM_SCAN_TYPE_T;

typedef enum _ENUM_SCAN_STATE_T {
    SCAN_STATE_IDLE = 0,
    SCAN_STATE_SCANNING,
    SCAN_STATE_NUM
} ENUM_SCAN_STATE_T;

typedef enum _ENUM_SCAN_CHANNEL_T {
    SCAN_CHANNEL_FULL = 0,
    SCAN_CHANNEL_2G4,
    SCAN_CHANNEL_5G,
    SCAN_CHANNEL_P2P_SOCIAL,
    SCAN_CHANNEL_SPECIFIED,
    SCAN_CHANNEL_NUM
} ENUM_SCAN_CHANNEL, *P_ENUM_SCAN_CHANNEL;

typedef struct _MSG_SCN_FSM_T {
    MSG_HDR_T       rMsgHdr;        /*                          */
    UINT_32         u4Dummy;
} MSG_SCN_FSM_T, *P_MSG_SCN_FSM_T;



/*                                                                            */
/*                                                                            */
/*                                                                            */
struct _BSS_DESC_T {
    LINK_ENTRY_T            rLinkEntry;

    UINT_8                  aucBSSID[MAC_ADDR_LEN];
    UINT_8                  aucSrcAddr[MAC_ADDR_LEN]; /*                                               */

    BOOLEAN                 fgIsConnecting; /*                                       
                                                                                      
                                                                                
                                             */
    BOOLEAN                 fgIsConnected; /*                                              
                                                                                      
                                            */

    BOOLEAN                 fgIsHiddenSSID; /*                                       
                                                                            
                                             */
    UINT_8                  ucSSIDLen;
    UINT_8                  aucSSID[ELEM_MAX_LEN_SSID];

    OS_SYSTIME              rUpdateTime;

    ENUM_BSS_TYPE_T         eBSSType;

    UINT_16                 u2CapInfo;

    UINT_16                 u2BeaconInterval;
    UINT_16                 u2ATIMWindow;

    UINT_16                 u2OperationalRateSet;
    UINT_16                 u2BSSBasicRateSet;
    BOOLEAN                 fgIsUnknownBssBasicRate;

    BOOLEAN                 fgIsERPPresent;
    BOOLEAN                 fgIsHTPresent;

    UINT_8                  ucPhyTypeSet;           /*                                    */

    UINT_8                  ucChannelNum;

    ENUM_CHNL_EXT_T         eSco;                   /*                                         
                                                                                                      */
    ENUM_BAND_T             eBand;

    UINT_8                  ucDTIMPeriod;

    BOOLEAN                 fgIsLargerTSF; /*                                                                 */

    UINT_8                  ucRCPI;

    UINT_8                  ucWmmFlag; /*                                              */

    /*                                                               
                                                                           
                                                                             
                                                               */
    UINT_32                 u4RsnSelectedGroupCipher;
    UINT_32                 u4RsnSelectedPairwiseCipher;
    UINT_32                 u4RsnSelectedAKMSuite;

    UINT_16                 u2RsnCap;

    RSN_INFO_T              rRSNInfo;
    RSN_INFO_T              rWPAInfo;
#if 1//                
    WAPI_INFO_T             rIEWAPI;
    BOOLEAN                    fgIEWAPI;
#endif
    BOOLEAN                    fgIERSN;
    BOOLEAN                    fgIEWPA;

    /*                                                */
    /*                                                 
                                                                */
    UINT_8                  ucEncLevel;

#if CFG_ENABLE_WIFI_DIRECT
    BOOLEAN               fgIsP2PPresent;
	BOOLEAN					fgIsP2PReport; /*                             */
    P_P2P_DEVICE_DESC_T  prP2pDesc;

    UINT_8                  aucIntendIfAddr[MAC_ADDR_LEN]; /*                                               */
//                                                                                               
//                                                                                                  

    LINK_T rP2pDeviceList;

//                             

    /*    
                             
                                                
                           
                                                        
                                          
                                         
      */
#endif

    BOOLEAN                 fgIsIEOverflow; /*                                                          */
	UINT_16                 u2RawLength; /*                               */
    UINT_16                 u2IELength; /*                              */

    ULARGE_INTEGER          u8TimeStamp; /*                                                        */
	UINT_8                  aucRawBuf[CFG_RAW_BUFFER_SIZE];
    UINT_8                  aucIEBuf[CFG_IE_BUFFER_SIZE];

#ifdef CFG_AIS_SUPPORT_REJ_CNT_AVOID
#define SCN_BSS_DESC_REJ_ERR_CNT_TIMEOUT_SEC	60
	OS_SYSTIME				ucBssErrTime;
#define SCN_BSS_DESC_REJ_ERR_CNT				1
	UINT_8					ucBssErrCnt;
	UINT_16					ucBssErrCntTotal;
#endif /*                               */
};


typedef struct _SCAN_PARAM_T { /*                  */
    /*                   */
    ENUM_SCAN_TYPE_T            eScanType;

    /*              */
    ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIndex;

    /*                     */
    UINT_8                      ucSSIDType;
    UINT_8                      ucSSIDNum;

    /*                          */
    UINT_8                      ucSpecifiedSSIDLen[SCN_SSID_MAX_NUM];

    /*                */
    UINT_8                      aucSpecifiedSSID[SCN_SSID_MAX_NUM][ELEM_MAX_LEN_SSID];

#if CFG_ENABLE_WIFI_DIRECT
    BOOLEAN                     fgFindSpecificDev;                  /*                         */
    UINT_8                      aucDiscoverDevAddr[MAC_ADDR_LEN];
    BOOLEAN                     fgIsDevType;
    P2P_DEVICE_TYPE_T           rDiscoverDevType;

    UINT_16                     u2PassiveListenInterval;
    //                                 
#endif /*                 */

    BOOLEAN                     fgIsObssScan;
    BOOLEAN                     fgIsScanV2;

    /*                */
    UINT_16                     u2ProbeDelayTime;

    /*                     */
    ENUM_SCAN_CHANNEL   eScanChannel;
    UINT_8              ucChannelListNum;
    RF_CHANNEL_INFO_T   arChnlInfoList[MAXIMUM_OPERATION_CHANNEL_LIST];

    /*                      */
    UINT_8              ucSeqNum;

    /*                     */
    UINT_16             u2IELen;
    UINT_8              aucIE[MAX_IE_LENGTH];

} SCAN_PARAM_T, *P_SCAN_PARAM_T;

typedef struct _SCAN_INFO_T {
    ENUM_SCAN_STATE_T       eCurrentState;  /*                                      */

    OS_SYSTIME              rLastScanCompletedTime;

    SCAN_PARAM_T            rScanParam;

    UINT_32                 u4NumOfBssDesc;

    UINT_8                  aucScanBuffer[SCN_MAX_BUFFER_SIZE];

    LINK_T                  rBSSDescList;

    LINK_T                  rFreeBSSDescList;

    LINK_T                  rPendingMsgList;

    /*                          */
    BOOLEAN                 fgIsSparseChannelValid;
    RF_CHANNEL_INFO_T       rSparseChannel;

} SCAN_INFO_T, *P_SCAN_INFO_T;


/*                           */
typedef struct _MSG_SCN_SCAN_REQ_T {
    MSG_HDR_T           rMsgHdr;        /*                          */
    UINT_8              ucSeqNum;
    UINT_8              ucNetTypeIndex;
    ENUM_SCAN_TYPE_T    eScanType;
    UINT_8              ucSSIDType;     /*                                                         */
    UINT_8              ucSSIDLength;
    UINT_8              aucSSID[PARAM_MAX_LEN_SSID];
#if CFG_ENABLE_WIFI_DIRECT
    UINT_16             u2ChannelDwellTime;  /*                */
#endif
    ENUM_SCAN_CHANNEL   eScanChannel;
    UINT_8              ucChannelListNum;
    RF_CHANNEL_INFO_T   arChnlInfoList[MAXIMUM_OPERATION_CHANNEL_LIST];
    UINT_16             u2IELen;
    UINT_8              aucIE[MAX_IE_LENGTH];
} MSG_SCN_SCAN_REQ, *P_MSG_SCN_SCAN_REQ;

typedef struct _MSG_SCN_SCAN_REQ_V2_T {
    MSG_HDR_T           rMsgHdr;        /*                          */
    UINT_8              ucSeqNum;
    UINT_8              ucNetTypeIndex;
    ENUM_SCAN_TYPE_T    eScanType;
    UINT_8              ucSSIDType;     /*                                                         */
    UINT_8              ucSSIDNum;
    P_PARAM_SSID_T      prSsid;
    UINT_16             u2ProbeDelay;
    UINT_16             u2ChannelDwellTime;  /*                */
    ENUM_SCAN_CHANNEL   eScanChannel;
    UINT_8              ucChannelListNum;
    RF_CHANNEL_INFO_T   arChnlInfoList[MAXIMUM_OPERATION_CHANNEL_LIST];
    UINT_16             u2IELen;
    UINT_8              aucIE[MAX_IE_LENGTH];
} MSG_SCN_SCAN_REQ_V2, *P_MSG_SCN_SCAN_REQ_V2;


typedef struct _MSG_SCN_SCAN_CANCEL_T {
    MSG_HDR_T           rMsgHdr;        /*                          */
    UINT_8              ucSeqNum;
    UINT_8              ucNetTypeIndex;
#if CFG_ENABLE_WIFI_DIRECT
    BOOLEAN           fgIsChannelExt;
#endif
} MSG_SCN_SCAN_CANCEL, *P_MSG_SCN_SCAN_CANCEL;

/*                           */
typedef enum _ENUM_SCAN_STATUS_T {
    SCAN_STATUS_DONE = 0,
    SCAN_STATUS_CANCELLED,
    SCAN_STATUS_FAIL,
    SCAN_STATUS_BUSY,
    SCAN_STATUS_NUM
} ENUM_SCAN_STATUS, *P_ENUM_SCAN_STATUS;

typedef struct _MSG_SCN_SCAN_DONE_T {
    MSG_HDR_T           rMsgHdr;        /*                          */
    UINT_8              ucSeqNum;
    UINT_8              ucNetTypeIndex;
    ENUM_SCAN_STATUS    eScanStatus;
} MSG_SCN_SCAN_DONE, *P_MSG_SCN_SCAN_DONE;

#if CFG_SUPPORT_AGPS_ASSIST
typedef enum {
	AGPS_PHY_A,
	AGPS_PHY_B,
	AGPS_PHY_G,
}AP_PHY_TYPE;

typedef struct _AGPS_AP_INFO_T{
    UINT_8 aucBSSID[6]; 
    INT_16 i2ApRssi; //         
    UINT_16 u2Channel;   //      
    AP_PHY_TYPE ePhyType;
} AGPS_AP_INFO_T, *P_AGPS_AP_INFO_T;

typedef struct _AGPS_AP_LIST_T {
	UINT_8 ucNum;
	AGPS_AP_INFO_T arApInfo[32];
} AGPS_AP_LIST_T, *P_AGPS_AP_LIST_T;
#endif

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
scnInit (
    IN P_ADAPTER_T prAdapter
    );

VOID
scnUninit (
    IN P_ADAPTER_T prAdapter
    );

/*                 */
P_BSS_DESC_T
scanSearchBssDescByBssid (
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
    );

P_BSS_DESC_T
scanSearchBssDescByBssidAndSsid (
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[],
    IN BOOLEAN fgCheckSsid,
    IN P_PARAM_SSID_T prSsid
    );

P_BSS_DESC_T
scanSearchBssDescByTA (
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucSrcAddr[]
    );

P_BSS_DESC_T
scanSearchBssDescByTAAndSsid (
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucSrcAddr[],
    IN BOOLEAN fgCheckSsid,
    IN P_PARAM_SSID_T prSsid
    );

#if CFG_SUPPORT_HOTSPOT_2_0
P_BSS_DESC_T
scanSearchBssDescByBssidAndLatestUpdateTime (
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
    );
#endif


/*                               */
P_BSS_DESC_T
scanSearchExistingBssDesc (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BSS_TYPE_T eBSSType,
    IN UINT_8 aucBSSID[],
    IN UINT_8 aucSrcAddr[]
    );

P_BSS_DESC_T
scanSearchExistingBssDescWithSsid (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BSS_TYPE_T eBSSType,
    IN UINT_8 aucBSSID[],
    IN UINT_8 aucSrcAddr[],
    IN BOOLEAN fgCheckSsid,
    IN P_PARAM_SSID_T prSsid
    );


/*                     */
P_BSS_DESC_T
scanAllocateBssDesc (
    IN P_ADAPTER_T prAdapter
    );

/*                  */
VOID
scanRemoveBssDescsByPolicy (
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4RemovePolicy
    );

VOID
scanRemoveBssDescByBssid (
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
    );

VOID
scanRemoveBssDescByBandAndNetwork (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BAND_T eBand,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex
    );

/*                       */
VOID
scanRemoveConnFlagOfBssDescByBssid (
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 aucBSSID[]
    );

#if 0
/*                    */
P_BSS_DESC_T
scanAddToInternalScanResult (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSWRfb,
    IN P_BSS_DESC_T prBssDesc
    );
#endif

/*                                  */
P_BSS_DESC_T
scanAddToBssDesc (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSwRfb
    );

WLAN_STATUS
scanProcessBeaconAndProbeResp (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T prSWRfb
    );

VOID
scanBuildProbeReqFrameCommonIEs (
    IN P_MSDU_INFO_T    prMsduInfo,
    IN PUINT_8          pucDesiredSsid,
    IN UINT_32          u4DesiredSsidLen,
    IN UINT_16          u2SupportedRateSet
    );

WLAN_STATUS
scanSendProbeReqFrames (
    IN P_ADAPTER_T prAdapter,
    IN P_SCAN_PARAM_T prScanParam
    );

VOID
scanUpdateBssDescForSearch (
    IN P_ADAPTER_T prAdapter,
    IN P_BSS_DESC_T prBssDesc
    );

P_BSS_DESC_T
scanSearchBssDescByPolicy (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex
    );

WLAN_STATUS
scanAddScanResult (
    IN P_ADAPTER_T prAdapter,
    IN P_BSS_DESC_T prBssDesc,
    IN P_SW_RFB_T prSwRfb
    );

VOID
scanReportBss2Cfg80211 (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_BSS_TYPE_T         eBSSType,
    IN P_BSS_DESC_T SpecificprBssDesc
    );

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
scnFsmSteps (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_SCAN_STATE_T eNextState
    );

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
scnSendScanReq (
    IN P_ADAPTER_T prAdapter
    );

VOID
scnSendScanReqV2 (
    IN P_ADAPTER_T prAdapter
    );

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
scnEventScanDone(
    IN P_ADAPTER_T          prAdapter,
    IN P_EVENT_SCAN_DONE    prScanDone
    );

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
scnFsmMsgStart (
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
    );

VOID
scnFsmMsgAbort (
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_HDR_T prMsgHdr
    );

VOID
scnFsmHandleScanMsg (
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_SCN_SCAN_REQ prScanReqMsg
    );

VOID
scnFsmHandleScanMsgV2 (
    IN P_ADAPTER_T prAdapter,
    IN P_MSG_SCN_SCAN_REQ_V2 prScanReqMsg
    );

VOID
scnFsmRemovePendingMsg (
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucSeqNum,
    IN UINT_8       ucNetTypeIndex
    );

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
scnFsmGenerateScanDoneMsg (
    IN P_ADAPTER_T          prAdapter,
    IN UINT_8               ucSeqNum,
    IN UINT_8               ucNetTypeIndex,
    IN ENUM_SCAN_STATUS     eScanStatus
    );

/*                                                                            */
/*                                                                            */
/*                                                                            */
BOOLEAN
scnQuerySparseChannel (
    IN P_ADAPTER_T      prAdapter,
    P_ENUM_BAND_T       prSparseBand,
    PUINT_8             pucSparseChannel
    );


#endif /*         */

#if CFG_SUPPORT_AGPS_ASSIST
VOID scanReportScanResultToAgps(P_ADAPTER_T prAdapter);
#endif
