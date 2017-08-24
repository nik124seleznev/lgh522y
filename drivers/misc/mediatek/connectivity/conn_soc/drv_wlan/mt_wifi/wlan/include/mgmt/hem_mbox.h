/*
                                                                                         
*/

/*                    
          

*/



/*
                     
   
                        
                                                                
                                              
   
                        
       
                      
  
                      
       
                                              
  
                   
                                                                                                                                
                                                                                                                                             
  
                        
                                                                                                                                                                                                     
                          
  
                   
                                                             
                                          
  
                      
                                                                                                    
                                                                       
  
                      
                                                                                  
                                       
  
                      
       
                           
  
                         
       
                                                                     
  
                   
       
                             
  
                          
       
                                                            
  
                         
       
                  
  
                   
       
                                              
  
                        
       
                                                            
  
                        
       
                                       
  
                        
       
                                         
  
                   
       
                                             
  
                        
       
                                       
  
                   
  
                                                      
                                                                                                
  
                   
  
                                                                               
  
                      
                                                            
                                                        
  
                   
                                                    
                                                        
  
                   
                                                    
                                                                  
  
                      
                                                            
                                          
  
                      
                                                            
                                                              
  
                   
                                                    
                                                                 
  
                   
                                                    
                      
  
                   
                                                    
                                     
  
                                                     
  
                   
                                                    
                                        
  
                   
                                                    
                        
  
                   
                                                    
                                  
  
                         
                                                               
                                                 
  
                         
                                             
                                       
  
                         
                                             
                                      
  
                      
                                                               
                         
  
                         
                                                          
                 
  
                         
                                                                       
  
                                                                       
  
                      
                                                               
                           
  
                         
                                                   
                                                                                  
  
                         
                                                               
                                  
  
                      
                                                                
                                      
  
                      
                                                      
                                    
  
                       
                                                                
                      
  
                       
                                                                
                              
  
                       
                                                                
                       
  
                      
                                                                
                  
  
                       
                                                                
  
*/

#ifndef _HEM_MBOX_H
#define _HEM_MBOX_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/
/*             */
typedef enum _ENUM_MSG_ID_T {
    MID_MNY_CNM_CH_REQ,                 /*                                             */
    MID_MNY_CNM_CH_ABORT,               /*                                                    */

    MID_CNM_AIS_CH_GRANT,               /*                                               */
    MID_CNM_P2P_CH_GRANT,               /*                                               */
    MID_CNM_BOW_CH_GRANT,               /*                                               */

    /*                                                  */
    /*                                                  */
    /*                                                  */
    MID_AIS_SCN_SCAN_REQ,               /*                                  */
    MID_AIS_SCN_SCAN_REQ_V2,            /*                                                             */
    MID_AIS_SCN_SCAN_CANCEL,            /*                                    */
    MID_P2P_SCN_SCAN_REQ,               /*                                  */
    MID_P2P_SCN_SCAN_REQ_V2,            /*                                                             */
    MID_P2P_SCN_SCAN_CANCEL,            /*                                    */
    MID_BOW_SCN_SCAN_REQ,               /*                                  */
    MID_BOW_SCN_SCAN_REQ_V2,            /*                                                             */
    MID_BOW_SCN_SCAN_CANCEL,            /*                                    */
    MID_RLM_SCN_SCAN_REQ,               /*                                              */
    MID_RLM_SCN_SCAN_REQ_V2,            /*                                                                         */
    MID_RLM_SCN_SCAN_CANCEL,            /*                                               */
    MID_SCN_AIS_SCAN_DONE,              /*                                    */
    MID_SCN_P2P_SCAN_DONE,              /*                                    */
    MID_SCN_BOW_SCAN_DONE,              /*                                    */
    MID_SCN_RLM_SCAN_DONE,              /*                                                */

    /*                                                  */
    /*                                                  */
    /*                                                  */
    MID_OID_AIS_FSM_JOIN_REQ,           /*                               */
    MID_OID_AIS_FSM_ABORT,              /*                                */
    MID_AIS_SAA_FSM_START,              /*                                                            */
    MID_AIS_SAA_FSM_ABORT,              /*                                                            */
    MID_SAA_AIS_JOIN_COMPLETE,          /*                                             */

#if CFG_ENABLE_BT_OVER_WIFI
    /*                                                  */
    /*                                                  */
    /*                                                  */
    MID_BOW_SAA_FSM_START,              /*                                                            */
    MID_BOW_SAA_FSM_ABORT,              /*                                                            */
    MID_SAA_BOW_JOIN_COMPLETE,          /*                                             */
#endif

#if CFG_ENABLE_WIFI_DIRECT
    /*                                                  */
    /*                                                  */
    /*                                                  */
    MID_P2P_SAA_FSM_START,              /*                                                            */
    MID_P2P_SAA_FSM_ABORT,              /*                                                            */
    MID_SAA_P2P_JOIN_COMPLETE,          /*                                             */

    MID_MNY_P2P_FUN_SWITCH,             /*                 */
    MID_MNY_P2P_DEVICE_DISCOVERY,       /*                         */
    MID_MNY_P2P_CONNECTION_REQ,         /*                     */
    MID_MNY_P2P_CONNECTION_ABORT,       /*                                                   */
    MID_MNY_P2P_BEACON_UPDATE,
    MID_MNY_P2P_STOP_AP,
    MID_MNY_P2P_CHNL_REQ,
    MID_MNY_P2P_CHNL_ABORT,
    MID_MNY_P2P_MGMT_TX,
    MID_MNY_P2P_GROUP_DISSOLVE,
    MID_MNY_P2P_MGMT_FRAME_REGISTER,
    MID_MNY_P2P_NET_DEV_REGISTER,
    MID_MNY_P2P_START_AP,
    MID_MNY_P2P_MGMT_FRAME_UPDATE,
#if CFG_SUPPORT_WFD
    MID_MNY_P2P_WFD_CFG_UPDATE,
#endif    
#endif

#if CFG_SUPPORT_ADHOC
    MID_SCN_AIS_FOUND_IBSS,             /*                                                                    */
#endif /*                   */

    MID_SAA_AIS_FSM_ABORT,              /*                                                               */

    /*                                                  */
    /*                                                  */
    /*                                                  */
    MID_MNY_AIS_REMAIN_ON_CHANNEL,
    MID_MNY_AIS_CANCEL_REMAIN_ON_CHANNEL,
    MID_MNY_AIS_MGMT_TX,

    MID_TOTAL_NUM
} ENUM_MSG_ID_T, *P_ENUM_MSG_ID_T;

/*                                    */
struct _MSG_HDR_T {
    LINK_ENTRY_T    rLinkEntry;
    ENUM_MSG_ID_T   eMsgId;
};

typedef VOID (*PFN_MSG_HNDL_FUNC)(P_ADAPTER_T, P_MSG_HDR_T);

typedef struct _MSG_HNDL_ENTRY {
    ENUM_MSG_ID_T       eMsgId;
    PFN_MSG_HNDL_FUNC   pfMsgHndl;
} MSG_HNDL_ENTRY_T, *P_MSG_HNDL_ENTRY_T;

typedef enum _EUNM_MSG_SEND_METHOD_T {
    MSG_SEND_METHOD_BUF = 0,    /*                                        
                                                                     */
    MSG_SEND_METHOD_UNBUF       /*                                           
                                                                     */
} EUNM_MSG_SEND_METHOD_T, *P_EUNM_MSG_SEND_METHOD_T;


typedef enum _ENUM_MBOX_ID_T {
    MBOX_ID_0 = 0,
    MBOX_ID_TOTAL_NUM
} ENUM_MBOX_ID_T, *P_ENUM_MBOX_ID_T;

/*                          */
typedef struct _MBOX_T {
    LINK_T          rLinkHead;
} MBOX_T, *P_MBOX_T;

typedef struct _MSG_SAA_FSM_START_T {
    MSG_HDR_T       rMsgHdr;    /*                          */
    UINT_8          ucSeqNum;
    P_STA_RECORD_T  prStaRec;
} MSG_SAA_FSM_START_T, *P_MSG_SAA_FSM_START_T;

typedef struct _MSG_SAA_FSM_COMP_T {
    MSG_HDR_T       rMsgHdr;        /*                          */
    UINT_8          ucSeqNum;
    WLAN_STATUS     rJoinStatus;
    P_STA_RECORD_T  prStaRec;
    P_SW_RFB_T      prSwRfb;
} MSG_SAA_FSM_COMP_T, *P_MSG_SAA_FSM_COMP_T;

typedef struct _MSG_SAA_FSM_ABORT_T {
    MSG_HDR_T       rMsgHdr;    /*                          */
    UINT_8          ucSeqNum;
    P_STA_RECORD_T  prStaRec;
} MSG_SAA_FSM_ABORT_T, *P_MSG_SAA_FSM_ABORT_T;

typedef struct _MSG_CONNECTION_ABORT_T {
    MSG_HDR_T       rMsgHdr;    /*                          */
    UINT_8          ucNetTypeIndex;
} MSG_CONNECTION_ABORT_T, *P_MSG_CONNECTION_ABORT_T;

typedef struct _MSG_REMAIN_ON_CHANNEL_T {
    MSG_HDR_T       rMsgHdr;    /*                          */
    ENUM_BAND_T     eBand;
    ENUM_CHNL_EXT_T eSco;
    UINT_8          ucChannelNum;
    UINT_32         u4DurationMs;
    UINT_64         u8Cookie;
} MSG_REMAIN_ON_CHANNEL_T, *P_MSG_REMAIN_ON_CHANNEL_T;

typedef struct _MSG_CANCEL_REMAIN_ON_CHANNEL_T {
    MSG_HDR_T       rMsgHdr;    /*                          */
    UINT_64         u8Cookie;
} MSG_CANCEL_REMAIN_ON_CHANNEL_T, *P_MSG_CANCEL_REMAIN_ON_CHANNEL_T;

typedef struct _MSG_MGMT_TX_REQUEST_T {
    MSG_HDR_T       rMsgHdr;
    P_MSDU_INFO_T   prMgmtMsduInfo;
    UINT_64         u8Cookie;                   /*                 */
    BOOLEAN         fgNoneCckRate;
    BOOLEAN         fgIsWaitRsp;
} MSG_MGMT_TX_REQUEST_T, *P_MSG_MGMT_TX_REQUEST_T;

/*                             */
typedef MSG_SAA_FSM_START_T MSG_JOIN_REQ_T, *P_MSG_JOIN_REQ_T;
typedef MSG_SAA_FSM_COMP_T MSG_JOIN_COMP_T, *P_MSG_JOIN_COMP_T;
typedef MSG_SAA_FSM_ABORT_T MSG_JOIN_ABORT_T, *P_MSG_JOIN_ABORT_T;


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/
VOID
mboxSetup (
    IN P_ADAPTER_T  prAdapter,
    IN ENUM_MBOX_ID_T  eMboxId
    );


VOID
mboxSendMsg (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_MBOX_ID_T eMboxId,
    IN P_MSG_HDR_T prMsg,
    IN EUNM_MSG_SEND_METHOD_T eMethod
    );

VOID
mboxRcvAllMsg (
    IN P_ADAPTER_T prAdapter,
    IN ENUM_MBOX_ID_T eMboxId
    );

VOID
mboxInitialize (
    IN P_ADAPTER_T prAdapter
    );

VOID
mboxDestroy (
    IN P_ADAPTER_T prAdapter
    );

VOID
mboxDummy (
    IN P_ADAPTER_T prAdapter,
    P_MSG_HDR_T prMsgHdr
    );

/*                                                                              
                                                
                                                                                
*/

#endif /*             */


