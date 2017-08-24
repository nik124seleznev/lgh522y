/*
                   
*/

/*               
                                                 
*/


/*
                  
  
                      
       
                   
 */

/*                                                                              
                                  
                                                                                 
 */
 
/*                                                                              
                                           
                                                                                 
 */
 extern UINT_64 u8DrvOwnStart, u8DrvOwnEnd;
 extern UINT32 u4DrvOwnMax;

/*                                                                              
                        
                                                                                
*/

/*                             */
typedef enum _STATS_CMD_CORE_ID {
	STATS_CORE_CMD_ENV_REQUEST = 0x00
} STATS_CMD_CORE_ID;

typedef enum _STATS_EVENT_HOST_ID {
	STATS_HOST_EVENT_ENV_REPORT = 0x00
} STATS_EVENT_HOST_ID;

/*                                                                              
                                                 
                                                                                
*/
typedef struct _STATS_CMD_CORE_T {

	UINT32					u4Command; /*                   */

	UINT8					ucStaRecIdx;
	UINT8					ucReserved[3];

	UINT32					u4Reserved[4];

#define STATS_CMD_CORE_RESERVED_SIZE					50
	union{
		UINT8 Reserved[STATS_CMD_CORE_RESERVED_SIZE];
	} Content;

} STATS_CMD_CORE_T;

typedef struct _STATS_INFO_ENV_T {

	BOOLEAN fgIsUsed; /*            */

	/*                                            */
	BOOLEAN fgTxIsRtsUsed; /*                                */
	BOOLEAN fgTxIsRtsEverUsed; /*                           */
	BOOLEAN fgTxIsCtsSelfUsed; /*                       */

#define STATS_INFO_TX_PARAM_HW_BW40_OFFSET			0
#define STATS_INFO_TX_PARAM_HW_SHORT_GI20_OFFSET	1
#define STATS_INFO_TX_PARAM_HW_SHORT_GI40_OFFSET	2
#define STATS_INFO_TX_PARAM_USE_BW40_OFFSET			3
#define STATS_INFO_TX_PARAM_USE_SHORT_GI_OFFSET		4
#define STATS_INFO_TX_PARAM_NO_ACK_OFFSET			5
	UINT_8	ucTxParam;

	UINT_8  ucStaRecIdx;
	UINT_8  ucReserved1[2];

	UINT32 u4TxDataCntAll; /*                          */
	UINT32 u4TxDataCntOK; /*                          */
	UINT32 u4TxDataCntErr; /*                           */

	/*                                                                 */
	UINT32 u4TxDataCntErrType[6]; /*                                              */

	UINT_8 ucTxRate1NonHTMax;
	UINT_8 ucTxRate1HTMax;
	UINT32 u4TxRateCntNonHT[16]; /*              */
	UINT32 u4TxRateCntHT[16]; /*              */

	UINT_8 ucTxAggBitmap; /*                            */
	UINT_8 ucTxPeerAggMaxSize;


	/*                                            */
	BOOLEAN fgRxIsRtsUsed; /*                                   */
	BOOLEAN fgRxIsRtsEverUsed; /*                              */

	UINT_8 ucRcvRcpi;
	UINT_8 ucHwChanNum;
	BOOLEAN fgRxIsShortGI;
	UINT_8 ucReserved2[1];

	UINT32 u4RxDataCntAll; /*                          */
	UINT32 u4RxDataCntErr; /*                    */
	UINT32 u4RxRateCnt[3][16]; /*                                                */

	UINT_8 ucRxAggBitmap; /*                            */
	UINT_8 ucRxAggMaxSize;

#define STATS_INFO_PHY_MODE_CCK					0
#define STATS_INFO_PHY_MODE_OFDM				1
#define STATS_INFO_PHY_MODE_HT					2
#define STATS_INFO_PHY_MODE_VHT					3
	UINT_8 ucBssSupPhyMode; /*                           */

	UINT_8 ucVersion; /*                                            */


	/*                                               */
#define STATS_AIR_DELAY_INT						500 /*          */

	/*                                    */
	/*                                */
	UINT32 u4StayIntMaxH2M[3], u4StayIntMinH2M[3], u4StayIntAvgH2M[3];

	/*                                       */
	/*                                  */
	UINT32 u4AirDelayMax[3], u4AirDelayMin[3], u4AirDelayAvg[3];

	/*                                        */
	/*                                */
	UINT32 u4StayIntMax[3], u4StayIntMin[3], u4StayIntAvg[3];
	UINT32 u4StayIntMaxSysTime[3];

	/*                                          */
	/*                                */
	UINT32 u4StayIntMaxD2T[3], u4StayIntMinD2T[3], u4StayIntAvgD2T[3];

	/*                                              */
	/*                                                                           */
#define STATS_STAY_INT_CONST					1 /*     */
#define STATS_STAY_INT_CONST_2					5
#define STATS_STAY_INT_CONST_3					10
#define STATS_STAY_INT_CONST_4					15
#define STATS_STAY_INT_CONST_NUM				4
	UINT32 u4StayIntByConst[STATS_STAY_INT_CONST_NUM];

	/*
                                                  
                                                                     
 */
#define STATS_STAY_INT_NUM						4
	UINT32 u4StayIntMaxPast;
	UINT32 u4StayIntCnt[STATS_STAY_INT_NUM+1];

	/*                                            */
	/*                                                                              */
#define STATS_STAY_INT_D2H_CONST				10 /*      */
#define STATS_STAY_INT_D2H_CONST_2				20
#define STATS_STAY_INT_D2H_CONST_3				30
#define STATS_STAY_INT_D2H_CONST_4				40
#define STATS_STAY_INT_D2H_CONST_NUM			4
	UINT32 u4StayIntD2HByConst[STATS_STAY_INT_D2H_CONST_NUM];


	/*                                */
	UINT32 u4StayIntMaxRx[3], u4StayIntMinRx[3], u4StayIntAvgRx[3];


	/*                                                */
	UINT32 u4NumOfChanChange; /*                            */
	UINT32 u4NumOfRetryCnt; /*                      */
	UINT32 u4ChanIdleCnt; /*                                    */
	UINT32 u4PsIntMax;	/*                                */
	UINT_8 ucNumOfPsChange; /*                              */
	UINT_8 ucReserved3[3];

	UINT32 u4ReportSysTime; /*                      */
	UINT32 u4RxDataCntOk; /*                       */

	UINT_8 ucReserved_rate[4];
} STATS_INFO_ENV_T;


/*                                                                              
                                          
                                                                                
*/
#if (CFG_SUPPORT_STATISTICS == 1)

#define STATS_ENV_REPORT_DETECT				statsEnvReportDetect

#define STATS_RX_REORDER_FALL_AHEAD_INC(__StaRec__)							\
	(__StaRec__)->u4RxReorderFallAheadCnt ++;

#define STATS_RX_REORDER_FALL_BEHIND_INC(__StaRec__)						\
	(__StaRec__)->u4RxReorderFallBehindCnt ++;

#define STATS_RX_REORDER_HOLE_INC(__StaRec__)								\
	(__StaRec__)->u4RxReorderHoleCnt ++;

#define STATS_RX_ARRIVE_TIME_RECORD(__SwRfb__)								\
	(__SwRfb__)->rRxTime = StatsEnvTimeGet();

#define STATS_RX_PASS2OS_INC				StatsEnvRxDone

#define STATS_TX_TIME_ARRIVE(__Skb__)										\
do {																		\
	UINT_64 __SysTime;														\
	__SysTime = StatsEnvTimeGet(); /*    */									\
	GLUE_SET_PKT_XTIME(__Skb__, __SysTime);									\
} while(FALSE);

#define STATS_TX_TIME_TO_HIF				StatsEnvTxTime2Hif

#define STATS_DRIVER_OWN_RESET()											\
	u4DrvOwnMax = 0;
#define STATS_DRIVER_OWN_START_RECORD()										\
	u8DrvOwnStart = StatsEnvTimeGet();
#define STATS_DRIVER_OWN_END_RECORD()										\
	u8DrvOwnEnd = StatsEnvTimeGet();
#define STATS_DRIVER_OWN_STOP()												\
do {																		\
	UINT32 __Diff;															\
	__Diff = (UINT32)(u8DrvOwnEnd - u8DrvOwnStart);							\
	if (__Diff > u4DrvOwnMax)												\
		u4DrvOwnMax = __Diff;												\
} while(FALSE);

#else

#define STATS_ENV_REPORT_DETECT(__Adapter__, __StaRecIndex__)

#define STATS_RX_REORDER_FALL_AHEAD_INC(__StaRec__)
#define STATS_RX_REORDER_FALL_BEHIND_INC(__StaRec__)
#define STATS_RX_REORDER_HOLE_INC(__StaRec__)
#define STATS_RX_PASS2OS_INC(__StaRec__, __SwRfb__)
#define STATS_TX_TIME_ARRIVE(__Skb__)
#define STATS_TX_TIME_TO_HIF(__MsduInfo__, __HwTxHeader__)

#define STATS_DRIVER_OWN_RESET()
#define STATS_DRIVER_OWN_START_RECORD()
#define STATS_DRIVER_OWN_END_RECORD()
#define STATS_DRIVER_OWN_STOP()
#endif /*                        */

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                              
                                                                                
*/

/*                                                                              
                                       
                                                                                
*/

/*                                                                              
                                     
                                                                                
*/

VOID
statsEnvReportDetect(
	ADAPTER_T							*prAdapter,
	UINT8								ucStaRecIndex
	);

VOID
StatsEnvRxDone(
	STA_RECORD_T						*prStaRec,
	SW_RFB_T							*prSwRfb
	);

UINT_64
StatsEnvTimeGet(
	VOID
	);

VOID
StatsEnvTxTime2Hif(
	MSDU_INFO_T							*prMsduInfo,
	HIF_TX_HEADER_T						*prHwTxHeader
	);

VOID
statsEventHandle(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	);

/*                */


