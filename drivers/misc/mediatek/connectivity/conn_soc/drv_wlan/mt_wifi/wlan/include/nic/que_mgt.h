/*
                                                                                       
*/

/*                     
                                               

                                                                            
                                                                     
                                                                             
*/



/*
                    
  
                   
                                                                                             
                                   
  
                        
                                                                                    
                                                       
  
                        
                                                           
                                     
  
                        
                                                           
                              
  
                        
                                                                                           
                                                     
  
                        
                                                           
                                                   
  
                        
                                                       
                
  
                        
                                                                           
                                        
  
                        
                                                                   
                               
  
                        
                                                                                               
                                                        
                                                            
  
                        
                                         

                                              


                                                  
                                                                        
  
                        
                                         

                                              


                           
   
                                              
  
                          
                                                               
                                                    
                                                       
  
                   
                                                                                                                                 
                                         
  
                         
                                                                         
                                  
  
                        
       
                                        
  
                          
  
                                                
  
                        
  
                                        
                                      
                                         
  
                        
  
                              
                                      
  
                        
  
                
                                                                                        
  
                        
  
                                                      
  
                   
  
                                                                               
  
                        
                                        
                                  
  
                   
                                                    
                                                                    
  
                        
                                        
                                              
  
                        
                                        
                                 
  
                      
                                                            
                                                              
  
                   
                                                    
                                                                                 
  
                         
                                                
                                   
  
                         
                                                          
                                       
  
                           
                                             
                                
  
                         
                                                          
                                                                   
  
                         
                                                          
                                                                                        
  
                         
                                                          
                                               
  
                         
                                                          
                                          
  
                         
                                                          
                                                           
  
                         
                                                          
                                                               
  
                         
                                                          
                                             
                                                                           
                                                  
                                                                           
                                                        
                                                                           
                   
  
*/

#ifndef _QUE_MGT_H
#define _QUE_MGT_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

/*                        */
#define QM_BURST_END_INFO_ENABLED       1  /*                                                         */
#define QM_FORWARDING_FAIRNESS          1  /*                                                  */
#define QM_ADAPTIVE_TC_RESOURCE_CTRL    1  /*                                              */
#define QM_PRINT_TC_RESOURCE_CTRL       0  /*                                            */
#define QM_RX_WIN_SSN_AUTO_ADVANCING    1  /*                                                                      */
#define QM_RX_INIT_FALL_BEHIND_PASS     1  /*                                                                                    */
#define QM_TC_RESOURCE_EMPTY_COUNTER    1  /*                                              */
/*            */

/*
                                                   

                                                                   
                                                            
                              
                                             

                                                                        
                                                         
                                      
*/
#define QM_INIT_TIME_TO_UPDATE_QUE_LEN  60  /*                                                        */
#define QM_INIT_TIME_TO_UPDATE_QUE_LEN_MIN 5

#define QM_INIT_TIME_TO_ADJUST_TC_RSC   3   /*                                                             */
#define QM_QUE_LEN_MOVING_AVE_FACTOR    3   /*                              */

#define QM_MIN_RESERVED_TC0_RESOURCE    1
#define QM_MIN_RESERVED_TC1_RESOURCE    1
#define QM_MIN_RESERVED_TC2_RESOURCE    1
#define QM_MIN_RESERVED_TC3_RESOURCE    1
#define QM_MIN_RESERVED_TC4_RESOURCE    2   /*                                    */
#define QM_MIN_RESERVED_TC5_RESOURCE    1

#if defined(MT6620)

#define QM_GUARANTEED_TC0_RESOURCE      4
#define QM_GUARANTEED_TC1_RESOURCE      4
#define QM_GUARANTEED_TC2_RESOURCE      9
#define QM_GUARANTEED_TC3_RESOURCE      11
#define QM_GUARANTEED_TC4_RESOURCE      2   /*                                    */
#define QM_GUARANTEED_TC5_RESOURCE      4

#elif defined(MT5931) 

#define QM_GUARANTEED_TC0_RESOURCE      4
#define QM_GUARANTEED_TC1_RESOURCE      4
#define QM_GUARANTEED_TC2_RESOURCE      4 
#define QM_GUARANTEED_TC3_RESOURCE      4
#define QM_GUARANTEED_TC4_RESOURCE      2   /*                                    */
#define QM_GUARANTEED_TC5_RESOURCE      2

#elif defined(MT6628)

#define QM_GUARANTEED_TC0_RESOURCE      4
#define QM_GUARANTEED_TC1_RESOURCE      4
#define QM_GUARANTEED_TC2_RESOURCE      6
#define QM_GUARANTEED_TC3_RESOURCE      6
#define QM_GUARANTEED_TC4_RESOURCE      2   /*                                    */
#define QM_GUARANTEED_TC5_RESOURCE      4

#else
#error
#endif



#define QM_EXTRA_RESERVED_RESOURCE_WHEN_BUSY    0

#define QM_TOTAL_TC_RESOURCE            (\
        NIC_TX_BUFF_COUNT_TC0 + NIC_TX_BUFF_COUNT_TC1 +\
        NIC_TX_BUFF_COUNT_TC2 + NIC_TX_BUFF_COUNT_TC3 +\
        NIC_TX_BUFF_COUNT_TC5)
#define QM_AVERAGE_TC_RESOURCE          6

/*                                                                */
/*                                                                  */
#define QM_INITIAL_RESIDUAL_TC_RESOURCE  (QM_TOTAL_TC_RESOURCE - \
                                         (QM_GUARANTEED_TC0_RESOURCE +\
                                          QM_GUARANTEED_TC1_RESOURCE +\
                                          QM_GUARANTEED_TC2_RESOURCE +\
                                          QM_GUARANTEED_TC3_RESOURCE +\
                                          QM_GUARANTEED_TC5_RESOURCE \
                                          ))

/*                                                               */
#define QM_OPERATING_NETWORK_TYPE   NETWORK_TYPE_AIS

#define QM_TEST_MODE                        0
#define QM_TEST_TRIGGER_TX_COUNT            50
#define QM_TEST_STA_REC_DETERMINATION       0
#define QM_TEST_STA_REC_DEACTIVATION        0
#define QM_TEST_FAIR_FORWARDING             0

#define QM_DEBUG_COUNTER                    0

/*                                                                */
/*                             */
#define NUM_OF_PER_STA_TX_QUEUES    5
#define NUM_OF_PER_TYPE_TX_QUEUES   1

/*                                                                      */
#define STA_REC_INDEX_BMCAST        0xFF
#define STA_REC_INDEX_NOT_FOUND     0xFE

/*                */
#define TX_QUEUE_INDEX_BMCAST       0
#define TX_QUEUE_INDEX_NO_STA_REC   0
#define TX_QUEUE_INDEX_AC0          0
#define TX_QUEUE_INDEX_AC1          1
#define TX_QUEUE_INDEX_AC2          2
#define TX_QUEUE_INDEX_AC3          3
#define TX_QUEUE_INDEX_802_1X       4
#define TX_QUEUE_INDEX_NON_QOS      1


//             
/*           */
#define WMM_FLAG_SUPPORT_WMM                BIT(0)
#define WMM_FLAG_SUPPORT_WMMSA              BIT(1)
#define WMM_FLAG_AC_PARAM_PRESENT           BIT(2)
#define WMM_FLAG_SUPPORT_UAPSD              BIT(3)

/*                                       */
#define ACM_FLAG_ADM_NOT_REQUIRED           0
#define ACM_FLAG_ADM_GRANTED                BIT(0)
#define ACM_FLAG_ADM_REQUIRED               BIT(1)

/*                        */
#define AC_FLAG_TRIGGER_ENABLED             BIT(1)
#define AC_FLAG_DELIVERY_ENABLED            BIT(2)

/*                                   */
#define ELEM_MAX_LEN_WMM_INFO               7

/*                                 */
#define ELEM_MAX_LEN_WMM_PARAM              24

/*                              */
#define WMM_QOS_INFO_PARAM_SET_CNT          BITS(0,3) /*            */
#define WMM_QOS_INFO_UAPSD                  BIT(7)

#define WMM_QOS_INFO_VO_UAPSD               BIT(0) /*                    */
#define WMM_QOS_INFO_VI_UAPSD               BIT(1)
#define WMM_QOS_INFO_BK_UAPSD               BIT(2)
#define WMM_QOS_INFO_BE_UAPSD               BIT(3)
#define WMM_QOS_INFO_MAX_SP_LEN_MASK        BITS(5,6)
#define WMM_QOS_INFO_MAX_SP_ALL             0
#define WMM_QOS_INFO_MAX_SP_2               BIT(5)
#define WMM_QOS_INFO_MAX_SP_4               BIT(6)
#define WMM_QOS_INFO_MAX_SP_6               BITS(5,6)

/*                                        */
#define WMM_MAX_SP_LENGTH_ALL               0
#define WMM_MAX_SP_LENGTH_2                 2
#define WMM_MAX_SP_LENGTH_4                 4
#define WMM_MAX_SP_LENGTH_6                 6


/*                               */
/*                                     */
#define WMM_ACIAIFSN_AIFSN                  BITS(0,3)
#define WMM_ACIAIFSN_ACM                    BIT(4)
#define WMM_ACIAIFSN_ACI                    BITS(5,6)
#define WMM_ACIAIFSN_ACI_OFFSET             5

/*                              */
#define WMM_ACI_AC_BE                       0
#define WMM_ACI_AC_BK                       BIT(5)
#define WMM_ACI_AC_VI                       BIT(6)
#define WMM_ACI_AC_VO                       BITS(5,6)

#define WMM_ACI(_AC)                        (_AC << WMM_ACIAIFSN_ACI_OFFSET)

/*                                        */
#define WMM_ECW_WMIN_MASK                   BITS(0,3)
#define WMM_ECW_WMAX_MASK                   BITS(4,7)
#define WMM_ECW_WMAX_OFFSET                 4

#define TXM_DEFAULT_FLUSH_QUEUE_GUARD_TIME              0   /*             */

#define QM_RX_BA_ENTRY_MISS_TIMEOUT_MS      (1000)

/*                                                                              
                                                 
                                                                                
*/

enum {
    QM_DBG_CNT_00=0,
    QM_DBG_CNT_01,
    QM_DBG_CNT_02,
    QM_DBG_CNT_03,
    QM_DBG_CNT_04,
    QM_DBG_CNT_05,
    QM_DBG_CNT_06,
    QM_DBG_CNT_07,
    QM_DBG_CNT_08,
    QM_DBG_CNT_09,
    QM_DBG_CNT_10,
    QM_DBG_CNT_11,
    QM_DBG_CNT_12,
    QM_DBG_CNT_13,
    QM_DBG_CNT_14,
    QM_DBG_CNT_15,
    QM_DBG_CNT_16,
    QM_DBG_CNT_17,
    QM_DBG_CNT_18,
    QM_DBG_CNT_19,
    QM_DBG_CNT_20,
    QM_DBG_CNT_21,
    QM_DBG_CNT_22,
    QM_DBG_CNT_23,
    QM_DBG_CNT_24,
    QM_DBG_CNT_25,
    QM_DBG_CNT_26,
    QM_DBG_CNT_27,
    QM_DBG_CNT_28,
    QM_DBG_CNT_29,
    QM_DBG_CNT_30,
    QM_DBG_CNT_31,
    QM_DBG_CNT_NUM
};




/*                 */
typedef enum _ENUM_MAC_TX_QUEUE_INDEX_T {
    MAC_TX_QUEUE_AC0_INDEX = 0,
    MAC_TX_QUEUE_AC1_INDEX,
    MAC_TX_QUEUE_AC2_INDEX,
    MAC_TX_QUEUE_AC3_INDEX,
    MAC_TX_QUEUE_AC4_INDEX,
    MAC_TX_QUEUE_AC5_INDEX,
    MAC_TX_QUEUE_AC6_INDEX,
    MAC_TX_QUEUE_BCN_INDEX,
    MAC_TX_QUEUE_BMC_INDEX,
    MAC_TX_QUEUE_NUM
} ENUM_MAC_TX_QUEUE_INDEX_T;

typedef struct _RX_BA_ENTRY_T {
    BOOLEAN                 fgIsValid;
    QUE_T                   rReOrderQue;
    UINT_16                 u2WinStart;
    UINT_16                 u2WinEnd;
    UINT_16                 u2WinSize;

    /*                                     */
    UINT_8                  ucStaRecIdx;
    UINT_8                  ucTid;

    BOOLEAN                 fgIsWaitingForPktWithSsn;

    //                                       
    //                                        
    //                                      
} RX_BA_ENTRY_T, *P_RX_BA_ENTRY_T;

/*                                                                                  */
typedef struct _MAILBOX_MSG_T{
    UINT_32 u4Msg[2];   /*                                                  */
} MAILBOX_MSG_T, *P_MAILBOX_MSG_T;


/*                                            */
typedef struct _TC_RESOURCE_CTRL_T {
    /*                         */
    UINT_32 au4AverageQueLen[TC_NUM - 1];
} TC_RESOURCE_CTRL_T, *P_TC_RESOURCE_CTRL_T;

typedef struct _QUE_MGT_T{      /*                               */

    /*                                                    */
    QUE_T arTxQueue[NUM_OF_PER_TYPE_TX_QUEUES];

#if 0
    /*                   */
    UINT_8  arRemainingTxOppt[NUM_OF_PER_STA_TX_QUEUES];
    UINT_8  arCurrentTxStaIndex[NUM_OF_PER_STA_TX_QUEUES];

#endif

    /*                             */
    RX_BA_ENTRY_T  arRxBaTable[CFG_NUM_OF_RX_BA_AGREEMENTS];

    /*                                                                             */
    UINT_8 ucRxBaCount;

#if QM_TEST_MODE
    UINT_32 u4PktCount;
    P_ADAPTER_T prAdapter;

#if QM_TEST_FAIR_FORWARDING
    UINT_32 u4CurrentStaRecIndexToEnqueue;
#endif

#endif


#if QM_FORWARDING_FAIRNESS
    /*                                                           */
    UINT_32 au4ForwardCount[NUM_OF_PER_STA_TX_QUEUES];

    /*                                                    */
    UINT_32 au4HeadStaRecIndex [NUM_OF_PER_STA_TX_QUEUES];
#endif

#if QM_ADAPTIVE_TC_RESOURCE_CTRL
    UINT_32 au4AverageQueLen[TC_NUM];
    UINT_32 au4CurrentTcResource[TC_NUM];
    UINT_32 au4MinReservedTcResource[TC_NUM]; /*                                                       */
    UINT_32 au4GuaranteedTcResource[TC_NUM]; /*                                                    */

    UINT_32 u4TimeToAdjustTcResource;
    UINT_32 u4TimeToUpdateQueLen;
	UINT_32 u4TxNumOfVi, u4TxNumOfVo; /*                         */

    /*                                                                                                         
                                                             */
    BOOLEAN fgTcResourcePostAnnealing;

#endif

#if QM_DEBUG_COUNTER
    UINT_32 au4QmDebugCounters[QM_DBG_CNT_NUM];
#endif
#if QM_TC_RESOURCE_EMPTY_COUNTER
    UINT_32 au4QmTcResourceEmptyCounter[NET_TYPE_NUM][TC_NUM];
#endif
} QUE_MGT_T, *P_QUE_MGT_T;



typedef struct _EVENT_RX_ADDBA_T {
    /*              */
    UINT_16     u2Length;
    UINT_16     u2Reserved1;    /*                                           */
    UINT_8      ucEID;
    UINT_8      ucSeqNum;
    UINT_8      aucReserved2[2];

    /*                                              */
    UINT_8      ucStaRecIdx;

    /*                                                   */
    UINT_8      ucDialogToken;              /*                                   */
    UINT_16     u2BAParameterSet;           /*                             */
    UINT_16     u2BATimeoutValue;
    UINT_16     u2BAStartSeqCtrl;           /*     */

} EVENT_RX_ADDBA_T, *P_EVENT_RX_ADDBA_T;

typedef struct _EVENT_RX_DELBA_T {
    /*              */
	UINT_16     u2Length;
	UINT_16     u2Reserved1;    /*                                           */
	UINT_8		ucEID;
	UINT_8      ucSeqNum;
	UINT_8		aucReserved2[2];

    /*                                              */
    UINT_8      ucStaRecIdx;
    UINT_8      ucTid;
} EVENT_RX_DELBA_T, *P_EVENT_RX_DELBA_T;


typedef struct _EVENT_BSS_ABSENCE_PRESENCE_T {
    /*              */
    UINT_16     u2Length;
    UINT_16     u2Reserved1;    /*                                           */
    UINT_8		ucEID;
    UINT_8      ucSeqNum;
    UINT_8		aucReserved2[2];

    /*            */
    UINT_8      ucNetTypeIdx;
    BOOLEAN     fgIsAbsent;
    UINT_8      ucBssFreeQuota;
    UINT_8      aucReserved[1];
} EVENT_BSS_ABSENCE_PRESENCE_T, *P_EVENT_BSS_ABSENCE_PRESENCE_T;


typedef struct _EVENT_STA_CHANGE_PS_MODE_T {
    /*              */
    UINT_16     u2Length;
    UINT_16     u2Reserved1;    /*                                           */
    UINT_8		ucEID;
    UINT_8      ucSeqNum;
    UINT_8		aucReserved2[2];

    /*            */
    UINT_8      ucStaRecIdx;
    BOOLEAN     fgIsInPs;
    UINT_8      ucUpdateMode;
    UINT_8      ucFreeQuota; 
} EVENT_STA_CHANGE_PS_MODE_T, *P_EVENT_STA_CHANGE_PS_MODE_T;

/*                                       */
/*                                                           */
typedef struct _EVENT_STA_UPDATE_FREE_QUOTA_T {
    /*              */
    UINT_16     u2Length;
    UINT_16     u2Reserved1;    /*                                           */
    UINT_8      ucEID;
    UINT_8      ucSeqNum;
    UINT_8      aucReserved2[2];

    /*            */
    UINT_8      ucStaRecIdx;
    UINT_8      ucUpdateMode;
    UINT_8      ucFreeQuota;
    UINT_8      aucReserved[1];
} EVENT_STA_UPDATE_FREE_QUOTA_T, *P_EVENT_STA_UPDATE_FREE_QUOTA_T;




/*                                   */
typedef struct _IE_WMM_INFO_T {
    UINT_8      ucId;                   /*            */
    UINT_8      ucLength;               /*        */
    UINT_8      aucOui[3];              /*     */
    UINT_8      ucOuiType;              /*          */
    UINT_8      ucOuiSubtype;           /*             */
    UINT_8      ucVersion;              /*         */
    UINT_8      ucQosInfo;              /*                */
    UINT_8      ucDummy[3];             /*                */
} IE_WMM_INFO_T, *P_IE_WMM_INFO_T;

/*                                 */
typedef struct _IE_WMM_PARAM_T {
    UINT_8      ucId;                   /*            */
    UINT_8      ucLength;               /*        */

    /*         */
    UINT_8      aucOui[3];              /*     */
    UINT_8      ucOuiType;              /*          */
    UINT_8      ucOuiSubtype;           /*             */
    UINT_8      ucVersion;              /*         */

    /*             */
    UINT_8      ucQosInfo;              /*                */
    UINT_8      ucReserved;

    /*               */
    UINT_8      ucAciAifsn_BE;
    UINT_8      ucEcw_BE;
    UINT_8      aucTxopLimit_BE[2];

    UINT_8      ucAciAifsn_BG;
    UINT_8      ucEcw_BG;
    UINT_8      aucTxopLimit_BG[2];

    UINT_8      ucAciAifsn_VI;
    UINT_8      ucEcw_VI;
    UINT_8      aucTxopLimit_VI[2];

    UINT_8      ucAciAifsn_VO;
    UINT_8      ucEcw_VO;
    UINT_8      aucTxopLimit_VO[2];

} IE_WMM_PARAM_T, *P_IE_WMM_PARAM_T;

typedef struct _IE_WMM_TSPEC_T {
    UINT_8      ucId;                   /*            */
    UINT_8      ucLength;               /*        */
    UINT_8      aucOui[3];              /*     */
    UINT_8      ucOuiType;              /*          */
    UINT_8      ucOuiSubtype;           /*             */
    UINT_8      ucVersion;              /*         */
    /*                */
    UINT_8      aucTsInfo[3];           /*         */
    UINT_8      aucTspecBodyPart[1];    /*                                                     */
} IE_WMM_TSPEC_T, *P_IE_WMM_TSPEC_T;

typedef struct _IE_WMM_HDR_T {
    UINT_8      ucId;                   /*            */
    UINT_8      ucLength;               /*        */
    UINT_8      aucOui[3];              /*     */
    UINT_8      ucOuiType;              /*          */
    UINT_8      ucOuiSubtype;           /*             */
    UINT_8      ucVersion;              /*         */
    UINT_8      aucBody[1];             /*         */
} IE_WMM_HDR_T, *P_IE_WMM_HDR_T;


typedef struct _AC_QUE_PARMS_T{
    UINT_16     u2CWmin;            /*         */
    UINT_16     u2CWmax;            /*         */
    UINT_16     u2TxopLimit;        /*              */
    UINT_16     u2Aifsn;            /*         */
    UINT_8      ucGuradTime;   /*                             */
    BOOLEAN     fgIsACMSet;
} AC_QUE_PARMS_T, *P_AC_QUE_PARMS_T;

/*                    */
typedef enum _ENUM_WMM_ACI_T {
    WMM_AC_BE_INDEX = 0,
    WMM_AC_BK_INDEX,
    WMM_AC_VI_INDEX,
    WMM_AC_VO_INDEX,
    WMM_AC_INDEX_NUM
} ENUM_WMM_ACI_T, *P_ENUM_WMM_ACI_T;


/*                              */
typedef enum _ENUM_FRAME_ACTION_T {
    FRAME_ACTION_DROP_PKT = 0,
    FRAME_ACTION_QUEUE_PKT,
    FRAME_ACTION_TX_PKT,
    FRAME_ACTION_NUM
} ENUM_FRAME_ACTION_T;


typedef enum _ENUM_FRAME_TYPE_IN_CMD_Q_T {
    FRAME_TYPE_802_1X = 0,
    FRAME_TYPE_MMPDU,
    FRAME_TYEP_NUM
} ENUM_FRAME_TYPE_IN_CMD_Q_T;

typedef enum _ENUM_FREE_QUOTA_MODET_T {
    FREE_QUOTA_UPDATE_MODE_INIT = 0,
    FREE_QUOTA_UPDATE_MODE_OVERWRITE,
    FREE_QUOTA_UPDATE_MODE_INCREASE,
    FREE_QUOTA_UPDATE_MODE_DECREASE
} ENUM_FREE_QUOTA_MODET_T, *P_ENUM_FREE_QUOTA_MODET_T;



typedef struct _CMD_UPDATE_WMM_PARMS_T {
    AC_QUE_PARMS_T          arACQueParms[AC_NUM];
    UINT_8                  ucNetTypeIndex;
    UINT_8                  fgIsQBSS;
    UINT_8                  aucReserved[2];
} CMD_UPDATE_WMM_PARMS_T, *P_CMD_UPDATE_WMM_PARMS_T;


typedef struct _CMD_TX_AMPDU_T {
    BOOLEAN          fgEnable;
    UINT_8           aucReserved[3];
} CMD_TX_AMPDU_T, *P_CMD_TX_AMPDU_T;


typedef struct _CMD_ADDBA_REJECT {
    BOOLEAN          fgEnable;
    UINT_8           aucReserved[3];
} CMD_ADDBA_REJECT_T, *P_CMD_ADDBA_REJECT_T;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

#define QM_TX_SET_NEXT_MSDU_INFO(_prMsduInfoPreceding, _prMsduInfoNext) \
    ((((_prMsduInfoPreceding)->rQueEntry).prNext) = (P_QUE_ENTRY_T)(_prMsduInfoNext))

#define QM_TX_SET_NEXT_SW_RFB(_prSwRfbPreceding, _prSwRfbNext) \
    ((((_prSwRfbPreceding)->rQueEntry).prNext) = (P_QUE_ENTRY_T)(_prSwRfbNext))


#define QM_TX_GET_NEXT_MSDU_INFO(_prMsduInfo) \
    ((P_MSDU_INFO_T)(((_prMsduInfo)->rQueEntry).prNext))

#define QM_RX_SET_NEXT_SW_RFB(_prSwRfbPreceding, _prSwRfbNext) \
    ((((_prSwRfbPreceding)->rQueEntry).prNext) = (P_QUE_ENTRY_T)(_prSwRfbNext))

#define QM_RX_GET_NEXT_SW_RFB(_prSwRfb) \
    ((P_SW_RFB_T)(((_prSwRfb)->rQueEntry).prNext))

#if 0
#define QM_GET_STA_REC_PTR_FROM_INDEX(_prAdapter, _ucIndex) \
    ((((_ucIndex) != STA_REC_INDEX_BMCAST) && ((_ucIndex)!= STA_REC_INDEX_NOT_FOUND)) ?\
    &(_prAdapter->arStaRec[_ucIndex]): NULL)
#endif

#define QM_GET_STA_REC_PTR_FROM_INDEX(_prAdapter, _ucIndex) \
    cnmGetStaRecByIndex(_prAdapter,_ucIndex)


#define QM_TX_SET_MSDU_INFO_FOR_DATA_PACKET(\
                                        _prMsduInfo,\
                                        _ucTC,\
                                        _ucPacketType,\
                                        _ucFormatID,\
                                        _fgIs802_1x,\
                                        _fgIs802_11,\
                                        _u2PalLLH,\
                                        _u2AclSN,\
                                        _ucPsForwardingType,\
                                        _ucPsSessionID\
                                        ) \
{\
    ASSERT(_prMsduInfo);\
    (_prMsduInfo)->ucTC                 = (_ucTC);\
    (_prMsduInfo)->ucPacketType         = (_ucPacketType);\
    (_prMsduInfo)->ucFormatID           = (_ucFormatID);\
    (_prMsduInfo)->fgIs802_1x           = (_fgIs802_1x);\
    (_prMsduInfo)->fgIs802_11           = (_fgIs802_11);\
    (_prMsduInfo)->u2PalLLH             = (_u2PalLLH);\
    (_prMsduInfo)->u2AclSN              = (_u2AclSN);\
    (_prMsduInfo)->ucPsForwardingType   = (_ucPsForwardingType);\
    (_prMsduInfo)->ucPsSessionID        = (_ucPsSessionID);\
    (_prMsduInfo)->fgIsBurstEnd         = (FALSE);\
}

#define QM_INIT_STA_REC(\
    _prStaRec,\
    _fgIsValid,\
    _fgIsQoS,\
    _pucMacAddr\
    )\
{\
    ASSERT(_prStaRec);\
    (_prStaRec)->fgIsValid      = (_fgIsValid);\
    (_prStaRec)->fgIsQoS        = (_fgIsQoS);\
    (_prStaRec)->fgIsInPS         = FALSE; \
    (_prStaRec)->ucPsSessionID  = 0xFF;\
    COPY_MAC_ADDR((_prStaRec)->aucMacAddr,(_pucMacAddr));\
}

#if QM_ADAPTIVE_TC_RESOURCE_CTRL
#define QM_GET_TX_QUEUE_LEN(_prAdapter, _u4QueIdx) ((_prAdapter->rQM.au4AverageQueLen[(_u4QueIdx)] >> QM_QUE_LEN_MOVING_AVE_FACTOR))
#endif


#define WMM_IE_OUI_TYPE(fp)      (((P_IE_WMM_HDR_T)(fp))->ucOuiType)
#define WMM_IE_OUI_SUBTYPE(fp)   (((P_IE_WMM_HDR_T)(fp))->ucOuiSubtype)
#define WMM_IE_OUI(fp)           (((P_IE_WMM_HDR_T)(fp))->aucOui)

#if QM_DEBUG_COUNTER
#define QM_DBG_CNT_INC(_prQM, _index) { (_prQM)->au4QmDebugCounters[(_index)]++; }
#else
#define QM_DBG_CNT_INC(_prQM, _index) {}
#endif



/*                                                                              
                                                             
                                                                                
*/
/*                                                                            */
/*                                                                            */
/*                                                                            */

VOID
qmInit(
    IN P_ADAPTER_T  prAdapter
    );

#if QM_TEST_MODE
VOID
qmTestCases(
    IN P_ADAPTER_T  prAdapter
    );
#endif

VOID
qmActivateStaRec(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T  prStaRec
    );

VOID
qmDeactivateStaRec(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4StaRecIdx
    );


/*                                                                            */
/*                                                                            */
/*                                                                            */

P_MSDU_INFO_T
qmFlushTxQueues(
    IN P_ADAPTER_T prAdapter
    );

P_MSDU_INFO_T
qmFlushStaTxQueues(
    IN P_ADAPTER_T prAdapter,
    IN UINT_32 u4StaRecIdx
    );

P_MSDU_INFO_T
qmEnqueueTxPackets(
    IN P_ADAPTER_T  prAdapter,
    IN P_MSDU_INFO_T prMsduInfoListHead
    );

P_MSDU_INFO_T
qmDequeueTxPackets(
    IN P_ADAPTER_T   prAdapter,
    IN P_TX_TCQ_STATUS_T prTcqStatus
    );

VOID
qmAdjustTcQuotas (
    IN P_ADAPTER_T  prAdapter,
    OUT P_TX_TCQ_ADJUST_T prTcqAdjust,
    IN P_TX_TCQ_STATUS_T prTcqStatus
    );


#if QM_ADAPTIVE_TC_RESOURCE_CTRL
VOID
qmReassignTcResource(
    IN P_ADAPTER_T   prAdapter
    );

VOID
qmUpdateAverageTxQueLen(
    IN P_ADAPTER_T   prAdapter
    );
#endif


/*                                                                            */
/*                                                                            */
/*                                                                            */

VOID
qmInitRxQueues(
    IN P_ADAPTER_T   prAdapter
    );

P_SW_RFB_T
qmFlushRxQueues(
    IN P_ADAPTER_T  prAdapter
    );

P_SW_RFB_T
qmHandleRxPackets(
    IN P_ADAPTER_T   prAdapter,
    IN P_SW_RFB_T prSwRfbListHead
    );

VOID
qmProcessPktWithReordering(
    IN P_ADAPTER_T   prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT P_QUE_T prReturnedQue
    );

VOID
qmProcessBarFrame(
    IN P_ADAPTER_T   prAdapter,
    IN P_SW_RFB_T prSwRfb,
    OUT P_QUE_T prReturnedQue
    );

VOID
qmInsertFallWithinReorderPkt(
    IN P_SW_RFB_T prSwRfb,
    IN P_RX_BA_ENTRY_T prReorderQueParm,
    OUT P_QUE_T prReturnedQue
    );

VOID
qmInsertFallAheadReorderPkt(
    IN P_SW_RFB_T prSwRfb,
    IN P_RX_BA_ENTRY_T prReorderQueParm,
    OUT P_QUE_T prReturnedQue
    );

BOOLEAN
qmPopOutDueToFallWithin(
    IN P_RX_BA_ENTRY_T prReorderQueParm,
    OUT P_QUE_T prReturnedQue
    );

VOID
qmPopOutDueToFallAhead(
    IN P_RX_BA_ENTRY_T prReorderQueParm,
    OUT P_QUE_T prReturnedQue
    );


VOID
qmHandleMailboxRxMessage(
    IN MAILBOX_MSG_T prMailboxRxMsg
    );

BOOLEAN
qmCompareSnIsLessThan(
    IN UINT_32 u4SnLess,
    IN UINT_32 u4SnGreater
    );

VOID
qmHandleEventRxAddBa(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
    );

VOID
qmHandleEventRxDelBa(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
    );

P_RX_BA_ENTRY_T
qmLookupRxBaEntry(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 ucStaRecIdx,
    IN UINT_8 ucTid
    );

BOOLEAN
qmAddRxBaEntry(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 ucStaRecIdx,
    IN UINT_8 ucTid,
    IN UINT_16 u2WinStart,
    IN UINT_16 u2WinSize
    );


VOID
qmDelRxBaEntry(
    IN P_ADAPTER_T prAdapter,
    IN UINT_8 ucStaRecIdx,
    IN UINT_8 ucTid,
    IN BOOLEAN fgFlushToHost
    );


VOID
mqmProcessAssocRsp (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb,
    IN PUINT_8     pucIE,
    IN UINT_16     u2IELength
    );

VOID
mqmParseEdcaParameters (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb,
    IN PUINT_8     pucIE,
    IN UINT_16     u2IELength,
    IN BOOLEAN     fgForceOverride
    );

VOID
mqmFillAcQueParam(
    IN  P_IE_WMM_PARAM_T prIeWmmParam,
    IN  UINT_32 u4AcOffset,
    OUT P_AC_QUE_PARMS_T prAcQueParams
    );

VOID
mqmProcessScanResult(
    IN P_ADAPTER_T prAdapter,
    IN P_BSS_DESC_T prScanResult,
    OUT P_STA_RECORD_T prStaRec
    );


/*                                              */
UINT_8
qmGetStaRecIdx(
    IN P_ADAPTER_T                  prAdapter,
    IN PUINT_8                      pucEthDestAddr,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetworkType
    );

UINT_32
mqmGenerateWmmInfoIEByParam (
	BOOLEAN					fgSupportUAPSD,
	UINT_8					ucBmpDeliveryAC,
	UINT_8					ucBmpTriggerAC,
	UINT_8					ucUapsdSp,
    UINT_8					*pOutBuf
    );

VOID
mqmGenerateWmmInfoIE (
    IN P_ADAPTER_T          prAdapter,
    IN P_MSDU_INFO_T        prMsduInfo
    );

UINT_32
mqmGenerateWmmParamIEByParam (
	P_ADAPTER_T				prAdapter,
	P_BSS_INFO_T			prBssInfo,
    UINT_8					*pOutBuf
    );

VOID
mqmGenerateWmmParamIE (
    IN P_ADAPTER_T          prAdapter,
    IN P_MSDU_INFO_T        prMsduInfo
    );


ENUM_FRAME_ACTION_T
qmGetFrameAction(
    IN P_ADAPTER_T                  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetworkType,
    IN UINT_8                       ucStaRecIdx,
    IN P_MSDU_INFO_T                prMsduInfo,
    IN ENUM_FRAME_TYPE_IN_CMD_Q_T   eFrameType
);

VOID
qmHandleEventBssAbsencePresence(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
    );

VOID
qmHandleEventStaChangePsMode(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
    );

VOID
mqmProcessAssocReq (
    IN P_ADAPTER_T prAdapter,
    IN P_SW_RFB_T  prSwRfb,
    IN PUINT_8     pucIE,
    IN UINT_16     u2IELength
    );

VOID
qmHandleEventStaUpdateFreeQuota(
    IN P_ADAPTER_T prAdapter,
    IN P_WIFI_EVENT_T prEvent
    );

    
VOID
qmUpdateFreeQuota(
    IN P_ADAPTER_T prAdapter,
    IN P_STA_RECORD_T prStaRec,
    IN UINT_8 ucUpdateMode, 
    IN UINT_8 ucFreeQuota,
	IN UINT_8 ucNumOfTxDone
    );

VOID
qmFreeAllByNetType(
    IN P_ADAPTER_T prAdapter, 
    IN ENUM_NETWORK_TYPE_INDEX_T eNetworkTypeIdx
    );

UINT_32
qmGetRxReorderQueuedBufferCount(
	IN P_ADAPTER_T  prAdapter
	);
   
/*                                                                              
                                                
                                                                                
*/

#endif /*            */
