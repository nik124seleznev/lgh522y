/*
                                                                                        
*/

/*                     
                                                                       
                                                    
*/



/*
                    
  
                      
       
                                                               
  
                          
                                                                                     
                                              
  
                        
       
                                                                      
  
                   
                                                                                                              
                                                                             
  
                      
                                                                                                    
    
  
                        
                                        

                                              


                                                  
  
                        
                                        
                                              

                           
  
                                              
  
                          
                                                               
                                                    
                                                       
  
                      
                                                                                      
                                   
                                                       
  
                      
                                                                                                                            
                                                             
  
                   
                                                                                                                            
                                                     
                                                                      
  
                         
                                                                         
                                  
  
                         
       
                                                                     
  
                   
  
                                                               
  
                   
  
                                                                               
  
                      
                                                            
                                                 
  
                      
                                                            
                                  
  
                   
                                                
                                           
  
                        
                                                    
                                                
  
                      
                                                            
                                                              
  
                        
                                                    
                          
  
                   
                                                    
                         
  
                   
                                                    
                                                                 
  
                   
                                                    
                                     
  
                                                     
  
                   
                                                    
                        
  
                   
                                                    
                                                                                 
  
                          
                                            
                                             
                                  
                                                                    
                                                                             
  
                        
                                                               
                                                             
  
                      
                                                               
                                              
  
                         
                                                               
                                                                       
  
                         
                                                               
                                                                                             
  
                        
                                                               
                                         
  
                         
                                                               
                                                 
  
                        
                                                               
                                                    
  
                          
                                            
                                            
  
                          
                                            
                                          
  
                         
                                             
                                                                         
  
                         
                                             
                                       
  
                         
                                                             
                                                                             
  
                         
                                             
                                          
                                    
  
                      
                                                               
                        
  
                         
                                                          
                 
  
                        
                                          
    
  
                        
                                                               
                              
  
                   
                                             
                                       
  
                         
                                                           
                                          
  
                      
                                                               
                                                    
  
                   
                                                                                            
                                                                         
  
                   
                                                                                            
                                                                                                                            
  
                         
                                                   
                                             
  
                         
                                                   
                                          
  
                         
                                                   
                                           
  
                         
                                                   
                                                     
  
                      
                                                               
                                      
  
                         
                                                   
                          
  
                         
                                                   
                                                                  
  
                         
                                                           
                                                                             
  
                         
                                                   
                               
  
                   
                                                                          
                                        
  
                         
                                                               
                                  
  
                   
                                             
                                                   
                                                                        
                                                       
  
                   
                                             
                                                   
                                                            
  
                   
                                             
                                                                                                                 
                                                                                                                
                                             
  
                        
                                          
                                                                      
  
                         
                                                               
                                                                                       
                      
                         
                                           
  
                        
                                                               
    
  
                   
                                             
                                                                     
  
                      
                                                               
                                                                                                         
  
                      
                                                               
                                     
  
                      
                                                               
                            
  
                      
                                                               
    
  
                          
                                                               
    
  
                       
                                                      
                                                 
  
                       
                                                                
                                  
  
                      
                                                                
                           
  
                      
                                              
                                                      
  
                      
                                                                           
                                                              
  
                      
                                                                           
                                                         
  
                      
                                                                
                                                                  
  
                      
                                                                           
                             
  
                       
                                                                
                                               
  
                       
                                                      
                                                      
  
                       
                                                      
                                    
  
                       
                                                      
                  
  
                       
                                                      
                               
  
                       
                                                                           
                                   
  
                       
                                                                           
                                              
  
                       
                                                                           
                                       
  
                       
                                                                
                                           
  
                       
                                                      
                                                                 
  
                       
                                                      
                                                 
  
                       
                                                                
                                                          
  
                      
                                                                
                                                                   
  
                      
                                                                
  
  
                      
                                                                
  
  
                      
                                                                
                 
  
                       
                                                                
                                                
  
                       
                                                                
  
  
                       
                                                                
  
  
                       
                                                                
                   
  
                       
                                                                
                                 
  
                       
                                                                
  
  
                      
                                                      
                                                                       
  
                      
                                                                
  
  
*/

#ifndef _CNM_MEM_H
#define _CNM_MEM_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

#ifndef POWER_OF_2
#define POWER_OF_2(n)                           BIT(n)
#endif

/*                                                       */
#define MGT_BUF_BLOCK_SIZE_IN_POWER_OF_2        7 /*                           */
#define MSG_BUF_BLOCK_SIZE_IN_POWER_OF_2        5 /*                          */

/*                                         */
#define MGT_BUF_BLOCK_SIZE                      POWER_OF_2(MGT_BUF_BLOCK_SIZE_IN_POWER_OF_2)
#define MSG_BUF_BLOCK_SIZE                      POWER_OF_2(MSG_BUF_BLOCK_SIZE_IN_POWER_OF_2)

/*                                                  */
#define MGT_BUF_BLOCKS_SIZE(n)                  ((UINT_32)(n) << MGT_BUF_BLOCK_SIZE_IN_POWER_OF_2)
#define MSG_BUF_BLOCKS_SIZE(n)                  ((UINT_32)(n) << MSG_BUF_BLOCK_SIZE_IN_POWER_OF_2)

/*                                   */
#define MAX_NUM_OF_BUF_BLOCKS                   32 /*             */

/*                                         */
#define MGT_BUFFER_SIZE                         (MAX_NUM_OF_BUF_BLOCKS * MGT_BUF_BLOCK_SIZE)
#define MSG_BUFFER_SIZE                         (MAX_NUM_OF_BUF_BLOCKS * MSG_BUF_BLOCK_SIZE)


/*                             */
#define STA_REC_INDEX_BMCAST        0xFF
#define STA_REC_INDEX_NOT_FOUND     0xFE
#define STA_WAIT_QUEUE_NUM          5       /*                                              */
#define SC_CACHE_INDEX_NUM          5       /*                                              */


/*                         */
#ifdef CFG_ENABLE_WIFI_DIRECT
/*                      */
#define WPS_ATTRI_MAX_LEN_DEVICE_NAME               32  /*        */
#define P2P_GC_MAX_CACHED_SEC_DEV_TYPE_COUNT        8 /*                          */
#endif


/*                                                                              
                                                 
                                                                                
*/
#if ((MAX_NUM_OF_BUF_BLOCKS > 32) || (MAX_NUM_OF_BUF_BLOCKS <= 0))
    #error > #define MAX_NUM_OF_MGT_BUF_BLOCKS : Out of boundary !
#elif MAX_NUM_OF_BUF_BLOCKS > 16
    typedef UINT_32 BUF_BITMAP;
#elif MAX_NUM_OF_BUF_BLOCKS > 8
    typedef UINT_16 BUF_BITMAP;
#else
    typedef UINT_8 BUF_BITMAP;
#endif /*                           */


/*                                               */
typedef struct _BUF_INFO_T {
    PUINT_8     pucBuf;

#if CFG_DBG_MGT_BUF
    UINT_32     u4AllocCount;
    UINT_32     u4FreeCount;
    UINT_32     u4AllocNullCount;
#endif /*                 */

    BUF_BITMAP  rFreeBlocksBitmap;
    UINT_8      aucAllocatedBlockNum[MAX_NUM_OF_BUF_BLOCKS];
} BUF_INFO_T, *P_BUF_INFO_T;


/*                                   
                                        
                                    
 */
typedef enum _ENUM_RAM_TYPE_T {
    RAM_TYPE_MSG = 0,
    RAM_TYPE_BUF
} ENUM_RAM_TYPE_T, P_ENUM_RAM_TYPE_T;

typedef enum _ENUM_BUFFER_SOURCE_T {
    BUFFER_SOURCE_HIF_TX0 =0,
    BUFFER_SOURCE_HIF_TX1,
    BUFFER_SOURCE_MAC_RX,
    BUFFER_SOURCE_MNG,
    BUFFER_SOURCE_BCN,
    BUFFER_SOURCE_NUM
} ENUM_BUFFER_SOURCE_T, *P_ENUM_BUFFER_SOURCE_T;


typedef enum _ENUM_SEC_STATE_T {
    SEC_STATE_INIT,
    SEC_STATE_INITIATOR_PORT_BLOCKED,
    SEC_STATE_RESPONDER_PORT_BLOCKED,
    SEC_STATE_CHECK_OK,
    SEC_STATE_SEND_EAPOL,
    SEC_STATE_SEND_DEAUTH,
    SEC_STATE_COUNTERMEASURE,
    SEC_STATE_NUM
} ENUM_SEC_STATE_T;

typedef struct _TSPEC_ENTRY_T {
    UINT_8  ucStatus;
    UINT_8  ucToken; /*                                        */
    UINT_16 u2MediumTime;
    UINT_32 u4TsInfo;
    //                               
    /*                                         */
} TSPEC_ENTRY_T, *P_TSPEC_ENTRY_T, TSPEC_TABLE_ENTRY_T, *P_TSPEC_TABLE_ENTRY_T;

typedef struct _SEC_INFO_T {

    ENUM_SEC_STATE_T        ePreviousState;
    ENUM_SEC_STATE_T        eCurrentState;

    BOOLEAN                    fg2nd1xSend;
    BOOLEAN                    fgKeyStored;

    UINT_8                  aucStoredKey[64];

    BOOLEAN                    fgAllowOnly1x;
} SEC_INFO_T, *P_SEC_INFO_T;

#define MAX_NUM_CONCURRENT_FRAGMENTED_MSDUS     3

#define UPDATE_BSS_RSSI_INTERVAL_SEC            3 //        

/*                                */
typedef struct _FRAG_INFO_T {
    UINT_16     u2NextFragSeqCtrl;
    PUINT_8     pucNextFragStart;
    P_SW_RFB_T  pr1stFrag;
    OS_SYSTIME  rReceiveLifetimeLimit; /*                                  */
} FRAG_INFO_T, *P_FRAG_INFO_T;


typedef struct _STAT_CNT_INFO_FW_T {
	UINT32		u4NumOfTx; /*                                  */
	UINT32		u4NumOfTxOK; /*                                  */
	UINT32		u4NumOfTxRetry; /*                                     */
	UINT32		u4TxDoneAirTimeMax; /*                          */

	UINT32		u4NumOfPtiRspTxOk; /*                                  */
	UINT32		u4NumOfPtiRspTxErr; /*                                     */

	UINT32		u4NumOfTxErr; /*                                     */

	UINT32		u4NumOfRx; /*                            */
	UINT32		u4NumOfPtiRspRx; /*                       */

#define STAT_CNT_INFO_TX_ERR_FLUSHED				0x00000001
#define STAT_CNT_INFO_TX_ERR_AGE_TIMEOUT			0x00000002
#define STAT_CNT_INFO_TX_ERR_MPDU					0x00000004
#define STAT_CNT_INFO_TX_ERR_RTS					0x00000010
#define STAT_CNT_INFO_TX_ERR_LIFETIME				0x00000020
#define STAT_CNT_INFO_TX_ERR_UNKNOWN				0x80000000
	UINT32		u4TxErrBitmap; /*               */

#define STAT_CNT_INFO_MAX_TX_RATE_OK_HIS_NUM		10 /*               */
	UINT8		aucTxRateOkHis[STAT_CNT_INFO_MAX_TX_RATE_OK_HIS_NUM][2];
	UINT32		u4TxRateOkHisId;

#define STAT_CNT_INFO_MAX_RATE_ID					(32) /*              */
	UINT32		aucTxRateMap[STAT_CNT_INFO_MAX_RATE_ID];
	UINT32		aucRxRateMap[STAT_CNT_INFO_MAX_RATE_ID];

	UINT8		aucStateHis[100][3]; /*               */
	UINT32		u4StateHisId; /*            */
} STAT_CNT_INFO_FW_T;

typedef struct _STAT_CNT_INFO_DRV_T {

	UINT32		u4NumOfTxFromOs; /*                                */
	UINT32		u4NumOfTxQueFull; /*                                             */
	UINT32		u4NumOfTxToFw; /*                                    */

	STAT_CNT_INFO_FW_T rFw;
} STAT_CNT_INFO_DRV_T;

/*                             */
struct _STA_RECORD_T {
    LINK_ENTRY_T            rLinkEntry;
    UINT_8                  ucIndex;                    /*                                   */

    BOOLEAN                 fgIsInUse;                  /*                                         */
    UINT_8                  aucMacAddr[MAC_ADDR_LEN];   /*             */

    /*         */
    ENUM_AA_STATE_T         eAuthAssocState;        /*                                   */
    UINT_8                  ucAuthAssocReqSeqNum;

    ENUM_STA_TYPE_T         eStaType;               /*                                 
                                                                                        
                                                     */

    UINT_8                  ucNetTypeIndex;         /*                           */

    UINT_8                  ucStaState;             /*             */

    UINT_8                  ucPhyTypeSet;           /*                                    
                                                                                             
                                                     */
    UINT_8                  ucDesiredPhyTypeSet;    /*                                            
                                                                           
                                                     */
    BOOLEAN                 fgHasBasicPhyType;      /*                                          
                                                                                                
                                                                                                 
                                                     */
    UINT_8                  ucNonHTBasicPhyType;    /*                                     
                                                                           
                                                     */

    UINT_16                 u2CapInfo;              /*                                                                      
                                                                                                                        
                                                     */
    UINT_16                 u2AssocId;              /*                                                         
                                                                                                   
                                                     */

    UINT_16                 u2ListenInterval;       /*                               */

    UINT_16                 u2DesiredNonHTRateSet;  /*                                   
                                                                                            
                                                     */

    UINT_16                 u2OperationalRateSet;   /*                                  */
    UINT_16                 u2BSSBasicRateSet;      /*                            */

    BOOLEAN                 fgIsMerging;            /*                                                  */

    BOOLEAN                 fgDiagnoseConnection;   /*                                                   
                                                                                                */

    /*                                                                                          */
    /*                                                                                          */
    /*                                                                                          */
    /*                                                                                          */
    UINT_8                  ucMcsSet;               /*                             */
    BOOLEAN                 fgSupMcs32;             /*                                */
    UINT_16                 u2HtCapInfo;            /*                                */
    UINT_8                  ucAmpduParam;           /*                                      */
    UINT_16                 u2HtExtendedCap;        /*                                    */
    UINT_32                 u4TxBeamformingCap;     /*                                       */
    UINT_8                  ucAselCap;              /*                             */

    UINT_8                  ucRCPI;                 /*              */

    UINT_8                  ucDTIMPeriod;           /*                                      
                                                                                      
                                                                       
                                                     */
    UINT_8                  ucAuthAlgNum;           /*                                                                           */
    BOOLEAN                 fgIsReAssoc;            /*                                                                   */

    UINT_8                  ucTxAuthAssocRetryCount;/*                                                             */
    UINT_8                  ucTxAuthAssocRetryLimit;/*                                                             */

    UINT_16                 u2StatusCode;       /*                          */
    UINT_16                 u2ReasonCode;       /*                                  */


    P_IE_CHALLENGE_TEXT_T   prChallengeText;    /*                                                                                       */

    TIMER_T                 rTxReqDoneOrRxRespTimer;/*                                                     
                                                                                                        
                                                     */

    /*                                                                                          */
    /*                                                                                                                                   */
    /*                                                                                          */
    BOOLEAN                 fgSetPwrMgtBit;         /*                                                            
                                                                                                  
                                                     */

    BOOLEAN                 fgIsInPS;               /*                                                  
                                                                                                */

    BOOLEAN                 fgIsInPsPollSP;             /*                                                                 
                                                                                         
                                                         */

    BOOLEAN                 fgIsInTriggerSP;            /*                                                                       
                                                                                          
                                                         */

    UINT_8                  ucBmpDeliveryAC;        /*                                */

    UINT_8                  ucBmpTriggerAC;         /*                                */

    UINT_8                  ucUapsdSp;  /*               */

    /*                                                                                          */

    BOOLEAN                 fgIsRtsEnabled;

    OS_SYSTIME              rUpdateTime;        /*                                               */

    OS_SYSTIME              rLastJoinTime;      /*                                             */

    UINT_8                  ucJoinFailureCount; /*                             */

    LINK_T                  arStaWaitQueue[STA_WAIT_QUEUE_NUM];     /*                                               */

    UINT_16                 au2CachedSeqCtrl[TID_NUM + 1];          /*                                                                                 */

#if 0
    /*     */
    P_RX_BA_ENTRY_T         aprRxBaTable[TID_NUM];

    /*     */
    P_TX_BA_ENTRY_T         aprTxBaTable[TID_NUM];
#endif

    FRAG_INFO_T             rFragInfo[MAX_NUM_CONCURRENT_FRAGMENTED_MSDUS];

    SEC_INFO_T              rSecInfo; /*                            */

    BOOLEAN                 fgPortBlock; /*                              */

    BOOLEAN                 fgTransmitKeyExist; /*                                */

    UINT_8                  ucWTEntry;

    BOOLEAN                 fgTxAmpduEn; /*                               */
    BOOLEAN                 fgRxAmpduEn; /*                               */

    PUINT_8                 pucAssocReqIe;
    UINT_16                 u2AssocReqIeLen;
    /*                                                                                          */
    /*                                                                                          */
    /*                                                                                          */
    BOOLEAN                 fgIsQoS;             /*                                                       */
    BOOLEAN                 fgIsWmmSupported;    /*                                                        */
    BOOLEAN                 fgIsUapsdSupported;  /*                                                    */

    /*                                                                                          */
    /*                                                                                          */
    /*                                                                                          */
#if CFG_ENABLE_WIFI_DIRECT
    UINT_8                  u2DevNameLen;
    UINT_8                  aucDevName[WPS_ATTRI_MAX_LEN_DEVICE_NAME];

    UINT_8                  aucDevAddr[MAC_ADDR_LEN];   /*                    */

    UINT_16                 u2ConfigMethods;

    UINT_8                  ucDeviceCap;

    UINT_8                  ucSecondaryDevTypeCount;

    DEVICE_TYPE_T           rPrimaryDevTypeBE;

    DEVICE_TYPE_T           arSecondaryDevTypeBE[P2P_GC_MAX_CACHED_SEC_DEV_TYPE_COUNT];
#endif /*                 */


    /*                                                                                          */
    /*                                                                                         */
    /*                                                                                          */

    UINT_8                  ucFreeQuota;   /*                                                                                           */
    //                                                                                            
    UINT_8                  ucFreeQuotaForDelivery;
    UINT_8                  ucFreeQuotaForNonDelivery;
#if CFG_ENABLE_PKT_LIFETIME_PROFILE && CFG_ENABLE_PER_STA_STATISTICS
    UINT_32                 u4TotalTxPktsNumber;
    UINT_32                 u4TotalTxPktsTime;
	UINT_32                 u4MaxTxPktsTime;
    UINT_32                 u4ThresholdCounter;
#endif

#if 1
    /*                                                                                          */
    /*                                                                                          */
    /*                                                                                          */
    /*                                           */
    BOOLEAN fgIsValid;

    /*                                                                */
    QUE_T   arTxQueue[NUM_OF_PER_STA_TX_QUEUES];

    /*                                           */
    //               

    /*                                                                                */
    UINT_8  ucPsSessionID;

    BOOLEAN fgIsAp;

    /*                                   */
    P_RX_BA_ENTRY_T  aprRxReorderParamRefTbl[CFG_RX_MAX_BA_TID_NUM];
#endif

#if CFG_SUPPORT_802_11V_TIMING_MEASUREMENT
    TIMINGMSMT_PARAM_T rWNMTimingMsmt;
#endif

#if (CFG_SUPPORT_TDLS == 1)
	BOOLEAN fgTdlsIsProhibited; /*                               */
	BOOLEAN fgTdlsIsChSwProhibited; /*                                     */

	BOOLEAN flgTdlsIsInitiator; /*                                 */
	IE_HT_CAP_T rTdlsHtCap; /*                                     */
	BOOLEAN fgTdlsInSecurityMode; /*                     */
	PARAM_KEY_T rTdlsKeyTemp; /*                                   */

#define TDLS_SETUP_TIMEOUT_SEC			5 /*              */
	OS_SYSTIME rTdlsSetupStartTime; /*                                 */

	OS_SYSTIME rTdlsTxQuotaEmptyTime; /*                         */

	STAT_CNT_INFO_DRV_T rTdlsStatistics;
#endif /*                  */

#if (CFG_SUPPORT_STATISTICS == 1)
#define STATS_ENV_TIMEOUT_SEC				10 /*              */
	OS_SYSTIME rStatsEnvTxPeriodLastTime;

#define STATS_ENV_TX_CNT_REPORT_TRIGGER		2500 /*       */
#define STATS_ENV_TX_CNT_REPORT_TRIGGER_SEC	5 /*              */
	OS_SYSTIME rStatsEnvTxLastTime;
	UINT32 u4StatsEnvTxCnt;

	UINT32 u4NumOfNoTxQuota;

	UINT32 u4RxReorderFallAheadCnt;
	UINT32 u4RxReorderFallBehindCnt;
	UINT32 u4RxReorderHoleCnt;

	UINT32 u4StatsRxPassToOsCnt;

	/*                                  */
#define STATS_STAY_INT_BYTE_THRESHOLD		500
	UINT32 u4StayIntMaxRx[3], u4StayIntMinRx[3], u4StayIntAvgRx[3];
#endif /*                        */
};

#if 0
/*                      */
/*                                */
typedef struct _MSDU_INFO_T {

  //                                                                             

    LINK_ENTRY_T    rLinkEntry;
    PUINT_8         pucBuffer;          /*                                  */

    UINT_8          ucBufferSource;     /*                                       */
    UINT_8          ucNetworkTypeIndex; /*                                                           */
    UINT_8          ucTC;               /*                                                                   */
    UINT_8          ucTID;              /*                        */

    BOOLEAN         fgIs802_11Frame;    /*                              */
    UINT_8          ucMacHeaderLength;
    UINT_16         u2PayloadLength;
    PUINT_8         pucMacHeader;       /*                */
    PUINT_8         pucPayload;         /*                */

    OS_SYSTIME      rArrivalTime;       /*                      */
    P_STA_RECORD_T  prStaRec;

#if CFG_PROFILE_BUFFER_TRACING
    ENUM_BUFFER_ACTIVITY_TYPE_T eActivity[2];
    UINT_32                     rActivityTime[2];
#endif
#if DBG && CFG_BUFFER_FREE_CHK
    BOOLEAN         fgBufferInSource;
#endif

    UINT_8          ucControlFlag;      /*                                                 */

    //                                                                   
    /*                                   */

    BOOLEAN     fgIs1xFrame;                /*                              */

    /*                                                              */
    BOOLEAN     fgIsTxFailed;               /*                                     */

    PFN_TX_DONE_HANDLER     pfTxDoneHandler;

    UINT_64     u8TimeStamp; /*                         */

    /*                                                       */
    UINT_8      ucPsForwardingType;         /*                                                */
    UINT_8      ucPsSessionID;              /*                                                    */

    /*                                */
    UINT_8      ucMacTxQueIdx;              /*                                     */
    BOOLEAN     fgNoAck;                    /*                                                   */
    BOOLEAN     fgBIP;                      /*                                            */
    UINT_8      ucFragTotalCount;
    UINT_8      ucFragFinishedCount;
    UINT_16     u2FragThreshold;     /*                                                   */
    BOOLEAN     fgFixedRate;                /*                                       */
    UINT_8      ucFixedRateCode;            /*                                     */
    UINT_8      ucFixedRateRetryLimit;      /*                                           */
    BOOLEAN     fgIsBmcQueueEnd;            /*                                              */

    /*                              */
    UINT_16     u2PalLLH;                   /*                */
    //                  
    UINT_16     u2ACLSeq;                   /*                                           */

    /*                                         */
    BOOLEAN     fgIsSnAssigned;
    UINT_16     u2SequenceNumber;          /*                                                     */

} MSDU_INFO_T, *P_MSDU_INFO_T;
#endif

#if 0
/*          */
typedef struct _SW_RFB_T {

    //                                                                            

    LINK_ENTRY_T    rLinkEntry;
    PUINT_8         pucBuffer;          /*                                  */

    UINT_8          ucBufferSource;     /*                                       */
    UINT_8          ucNetworkTypeIndex; /*                                                           */
    UINT_8          ucTC;               /*                                                                   */
    UINT_8          ucTID;              /*                        */

    BOOLEAN         fgIs802_11Frame;    /*                              */
    UINT_8          ucMacHeaderLength;
    UINT_16         u2PayloadLength;
    PUINT_8         pucMacHeader;       /*                */
    PUINT_8         pucPayload;         /*                */

    OS_SYSTIME      rArrivalTime;       /*                      */
    P_STA_RECORD_T  prStaRec;

#if CFG_PROFILE_BUFFER_TRACING
    ENUM_BUFFER_ACTIVITY_TYPE_T eActivity[2];
    UINT_32                     rActivityTime[2];
#endif
#if DBG && CFG_BUFFER_FREE_CHK
    BOOLEAN         fgBufferInSource;
#endif

    UINT_8          ucControlFlag;      /*                                                 */

    //                                                                   

    /*                                                                     */
    PUINT_8     pucHifRxPacket;             /*                                                  */
    UINT_16     u2HifRxPacketLength;
    UINT_8      ucHeaderOffset;
    UINT_8      ucHifRxPortIndex;

    UINT_16     u2SequenceControl;
    BOOLEAN     fgIsA4Frame;                /*                                                                    */
    BOOLEAN     fgIsBAR;
    BOOLEAN     fgIsQoSData;
    BOOLEAN     fgIsAmsduSubframe;  /*                                 */

    /*                     */
    BOOLEAN     fgTUChecksumCheckRequired;
    BOOLEAN     fgIPChecksumCheckRequired;
    UINT_8      ucEtherTypeOffset;

} SW_RFB_T, *P_SW_RFB_T;
#endif


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

P_MSDU_INFO_T
cnmMgtPktAlloc (
    IN P_ADAPTER_T  prAdapter,
    IN UINT_32      u4Length
    );

VOID
cnmMgtPktFree (
    IN P_ADAPTER_T      prAdapter,
    IN P_MSDU_INFO_T    prMsduInfo
    );

VOID
cnmMemInit (
    IN P_ADAPTER_T  prAdapter
    );

PVOID
cnmMemAlloc (
    IN P_ADAPTER_T      prAdapter,
    IN ENUM_RAM_TYPE_T  eRamType,
    IN UINT_32          u4Length
    );

VOID
cnmMemFree (
    IN P_ADAPTER_T  prAdapter,
    IN PVOID pvMemory
    );

VOID
cnmStaRecInit (
    IN P_ADAPTER_T  prAdapter
    );

VOID
cnmStaRecUninit (
    IN P_ADAPTER_T  prAdapter
    );

P_STA_RECORD_T
cnmStaRecAlloc (
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucNetTypeIndex
    );

VOID
cnmStaRecFree (
    IN P_ADAPTER_T      prAdapter,
    IN P_STA_RECORD_T   prStaRec,
    IN BOOLEAN          fgSyncToChip
    );

VOID
cnmStaFreeAllStaByNetType (
    P_ADAPTER_T                 prAdapter,
    ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIndex,
    BOOLEAN                     fgSyncToChip
    );

P_STA_RECORD_T
cnmGetStaRecByIndex (
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucIndex
    );

P_STA_RECORD_T
cnmGetStaRecByAddress (
    IN P_ADAPTER_T  prAdapter,
    IN UINT_8       ucNetTypeIndex,
    IN UINT_8       aucPeerMACAddress[]
    );

VOID
cnmStaRecResetStatus (
    IN P_ADAPTER_T                  prAdapter,
    IN ENUM_NETWORK_TYPE_INDEX_T    eNetTypeIndex
    );

VOID
cnmStaRecChangeState (
    IN P_ADAPTER_T          prAdapter,
    IN OUT P_STA_RECORD_T   prStaRec,
    IN UINT_8               ucNewState
    );

P_STA_RECORD_T
cnmStaTheTypeGet (
    P_ADAPTER_T                 prAdapter,
    ENUM_NETWORK_TYPE_INDEX_T   eNetTypeIndex,
    ENUM_STA_TYPE_T				eStaType,
	UINT32						*pu4StartIdx
    );

/*                                                                              
                                                
                                                                                
*/
#ifndef _lint
/*                                                                               
                                                     
                                                        
 */
__KAL_INLINE__ VOID
cnmMemDataTypeCheck (
    VOID
    )
{
#if 0
    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,rLinkEntry) == 0);

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,rLinkEntry) == OFFSET_OF(SW_RFB_T,rLinkEntry));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,pucBuffer) == OFFSET_OF(SW_RFB_T,pucBuffer));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,ucBufferSource) ==
        OFFSET_OF(SW_RFB_T,ucBufferSource));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,pucMacHeader) ==
        OFFSET_OF(SW_RFB_T,pucMacHeader));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,ucMacHeaderLength) ==
        OFFSET_OF(SW_RFB_T,ucMacHeaderLength));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,pucPayload) ==
        OFFSET_OF(SW_RFB_T,pucPayload));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,u2PayloadLength) ==
        OFFSET_OF(SW_RFB_T,u2PayloadLength));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,prStaRec) ==
        OFFSET_OF(SW_RFB_T,prStaRec));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,ucNetworkTypeIndex) ==
        OFFSET_OF(SW_RFB_T,ucNetworkTypeIndex));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,ucTID) ==
        OFFSET_OF(SW_RFB_T,ucTID));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,fgIs802_11Frame) ==
        OFFSET_OF(SW_RFB_T,fgIs802_11Frame));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,ucControlFlag) ==
        OFFSET_OF(SW_RFB_T,ucControlFlag));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,rArrivalTime) ==
        OFFSET_OF(SW_RFB_T,rArrivalTime));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,ucTC) ==
        OFFSET_OF(SW_RFB_T,ucTC));

#if CFG_PROFILE_BUFFER_TRACING
    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,eActivity[0]) ==
        OFFSET_OF(SW_RFB_T,eActivity[0]));

    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,rActivityTime[0]) ==
        OFFSET_OF(SW_RFB_T,rActivityTime[0]));
#endif

#if DBG && CFG_BUFFER_FREE_CHK
    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(MSDU_INFO_T,fgBufferInSource) ==
        OFFSET_OF(SW_RFB_T,fgBufferInSource));
#endif


    DATA_STRUC_INSPECTING_ASSERT(
        OFFSET_OF(STA_RECORD_T,rLinkEntry) == 0);

    return;
#endif
}
#endif /*       */

#endif /*            */


