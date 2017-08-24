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
#include "precomp.h"

#if (CFG_SUPPORT_STATISTICS == 1)

/*                                                                              
                        
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
static void
statsInfoEnvDisplay(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	);

static WLAN_STATUS
statsInfoEnvRequest(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	);

/*                                                                              
                            
                                                                                
*/
UINT_64 u8DrvOwnStart, u8DrvOwnEnd;
UINT32 u4DrvOwnMax = 0;

/*                                                                              
                                       
                                                                                
*/

/*                                                                            */
/*                                                                
 
                                                         
                                                                               
                                                
                  
 
              
 
*/
/*                                                                            */
static void
statsInfoEnvDisplay(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	)
{
	P_ADAPTER_T prAdapter;
	STA_RECORD_T *prStaRec;
	UINT32 u4NumOfInfo, u4InfoId;
	UINT32 u4RxErrBitmap;
	STATS_INFO_ENV_T rStatsInfoEnv, *prInfo;

/*
                                                                   
                                                                          
                                                                             
                                                                                      

                                                                                                       
                                                                                                                                       
                                                      
                                                                                     
                                                                          
                                                             
                                                                                            
                                                                                                 

                                                                                                         
                                                                            

                                                                                     
                                     
                                                                                    
                                                                    
                                      

                                                                                      
                       
                                                                                       
                                               
                                                                                     
                                                    

                                                                                        
                                                                                                              

                                                                                         
                                       

                                                                                                   
                                                                   

                                                                                             
                                                          

                                                                                          
                                               

                                                                                       
                                                     

                                                                               
                                                                           

                                                                          
                                                                        

                                                                                                                      
                                                                                                 

                                                                                    
                                                                           
                                                                           
*/

	/*      */
	prAdapter = prGlueInfo->prAdapter;
	prInfo = &rStatsInfoEnv;

	/*       */
	u4NumOfInfo = *(UINT32 *)prInBuf;
	u4RxErrBitmap = *(UINT32 *)(prInBuf+4);

	/*       */
	for(u4InfoId=0; u4InfoId<u4NumOfInfo; u4InfoId++)
	{
		kalMemCopy(&rStatsInfoEnv, prInBuf+8, sizeof(STATS_INFO_ENV_T));

		prStaRec = cnmGetStaRecByIndex(prAdapter, rStatsInfoEnv.ucStaRecIdx);
		if (prStaRec == NULL)
			continue;

		DBGLOG(INIT, INFO, ("<stats> Display stats for ["MACSTR"]: %uB %ums\n",
			MAC2STR(prStaRec->aucMacAddr), (UINT32)sizeof(STATS_INFO_ENV_T),
			prInfo->u4ReportSysTime));
		DBGLOG(INIT, INFO, ("<stats> TPAM(0x%x) RTS(%u %u) BA(0x%x %u) "
			"OS(%u) OK(%u %u) ERR(%u %u %u %u %u %u %u)\n",
			prInfo->ucTxParam,
			prInfo->fgTxIsRtsUsed, prInfo->fgTxIsRtsEverUsed,
			prInfo->ucTxAggBitmap, prInfo->ucTxPeerAggMaxSize,
			(UINT32)prGlueInfo->rNetDevStats.tx_packets,
			prInfo->u4TxDataCntAll, prInfo->u4TxDataCntOK,
			prInfo->u4TxDataCntErr,	prInfo->u4TxDataCntErrType[0],
			prInfo->u4TxDataCntErrType[1], prInfo->u4TxDataCntErrType[2],
			prInfo->u4TxDataCntErrType[3], prInfo->u4TxDataCntErrType[4],
			prInfo->u4TxDataCntErrType[5]));

		DBGLOG(INIT, INFO, ("<stats> TRATE "
			"(%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u) "
			"(%u %u %u %u %u %u %u %u)\n",
			prInfo->u4TxRateCntNonHT[0], prInfo->u4TxRateCntNonHT[1],
			prInfo->u4TxRateCntNonHT[2], prInfo->u4TxRateCntNonHT[3],
			prInfo->u4TxRateCntNonHT[4], prInfo->u4TxRateCntNonHT[5],
			prInfo->u4TxRateCntNonHT[6], prInfo->u4TxRateCntNonHT[7],
			prInfo->u4TxRateCntNonHT[8], prInfo->u4TxRateCntNonHT[9],
			prInfo->u4TxRateCntNonHT[10], prInfo->u4TxRateCntNonHT[11],
			prInfo->u4TxRateCntNonHT[12], prInfo->u4TxRateCntNonHT[13],
			prInfo->u4TxRateCntNonHT[14], prInfo->u4TxRateCntNonHT[15],
			prInfo->u4TxRateCntHT[0], prInfo->u4TxRateCntHT[1],
			prInfo->u4TxRateCntHT[2], prInfo->u4TxRateCntHT[3],
			prInfo->u4TxRateCntHT[4], prInfo->u4TxRateCntHT[5],
			prInfo->u4TxRateCntHT[6], prInfo->u4TxRateCntHT[7]));

		DBGLOG(INIT, INFO, ("<stats> TREORDER (%u %u %u)\n",
			prStaRec->u4RxReorderFallAheadCnt,
			prStaRec->u4RxReorderFallBehindCnt,
			prStaRec->u4RxReorderHoleCnt));

		DBGLOG(INIT, INFO, ("<stats> RX(%u %u %u) BA(0x%x %u) OK(%u %u) ERR(%u)\n",
			prInfo->ucRcvRcpi, prInfo->ucHwChanNum, prInfo->fgRxIsShortGI,
			prInfo->ucRxAggBitmap, prInfo->ucRxAggMaxSize,
			prInfo->u4RxDataCntAll, prStaRec->u4StatsRxPassToOsCnt,
			prInfo->u4RxDataCntErr));

		DBGLOG(INIT, INFO, ("<stats> RCCK "
			"(%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u)\n",
			prInfo->u4RxRateCnt[0][0], prInfo->u4RxRateCnt[0][1],
			prInfo->u4RxRateCnt[0][2], prInfo->u4RxRateCnt[0][3],
			prInfo->u4RxRateCnt[0][4], prInfo->u4RxRateCnt[0][5],
			prInfo->u4RxRateCnt[0][6], prInfo->u4RxRateCnt[0][7],
			prInfo->u4RxRateCnt[0][8], prInfo->u4RxRateCnt[0][9],
			prInfo->u4RxRateCnt[0][10], prInfo->u4RxRateCnt[0][11],
			prInfo->u4RxRateCnt[0][12], prInfo->u4RxRateCnt[0][13],
			prInfo->u4RxRateCnt[0][14], prInfo->u4RxRateCnt[0][15]));
		DBGLOG(INIT, INFO, ("<stats> ROFDM "
			"(%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u)\n",
			prInfo->u4RxRateCnt[1][0], prInfo->u4RxRateCnt[1][1],
			prInfo->u4RxRateCnt[1][2], prInfo->u4RxRateCnt[1][3],
			prInfo->u4RxRateCnt[1][4], prInfo->u4RxRateCnt[1][5],
			prInfo->u4RxRateCnt[1][6], prInfo->u4RxRateCnt[1][7],
			prInfo->u4RxRateCnt[1][8], prInfo->u4RxRateCnt[1][9],
			prInfo->u4RxRateCnt[1][10], prInfo->u4RxRateCnt[1][11],
			prInfo->u4RxRateCnt[1][12], prInfo->u4RxRateCnt[1][13],
			prInfo->u4RxRateCnt[1][14], prInfo->u4RxRateCnt[1][15]));
		DBGLOG(INIT, INFO, ("<stats> RHT "
			"(%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u)\n",
			prInfo->u4RxRateCnt[2][0], prInfo->u4RxRateCnt[2][1],
			prInfo->u4RxRateCnt[2][2], prInfo->u4RxRateCnt[2][3],
			prInfo->u4RxRateCnt[2][4], prInfo->u4RxRateCnt[2][5],
			prInfo->u4RxRateCnt[2][6], prInfo->u4RxRateCnt[2][7],
			prInfo->u4RxRateCnt[2][8], prInfo->u4RxRateCnt[2][9],
			prInfo->u4RxRateCnt[2][10], prInfo->u4RxRateCnt[2][11],
			prInfo->u4RxRateCnt[2][12], prInfo->u4RxRateCnt[2][13],
			prInfo->u4RxRateCnt[2][14], prInfo->u4RxRateCnt[2][15]));

		/*                       */
		DBGLOG(INIT, INFO, ("<stats> StayIntH2M us (%u %u %u) (%u %u %u) (%u %u %u)\n",
			prInfo->u4StayIntMinH2M[0], prInfo->u4StayIntAvgH2M[0],
			prInfo->u4StayIntMaxH2M[0],
			prInfo->u4StayIntMinH2M[1], prInfo->u4StayIntAvgH2M[1],
			prInfo->u4StayIntMaxH2M[1],
			prInfo->u4StayIntMinH2M[2], prInfo->u4StayIntAvgH2M[2],
			prInfo->u4StayIntMaxH2M[2]));
		/*                          */
		DBGLOG(INIT, INFO, ("<stats> AirTime us (%u %u %u) (%u %u %u) (%u %u %u)\n",
			prInfo->u4AirDelayMin[0]<<5, prInfo->u4AirDelayAvg[0]<<5,
			prInfo->u4AirDelayMax[0]<<5,
			prInfo->u4AirDelayMin[1]<<5, prInfo->u4AirDelayAvg[1]<<5,
			prInfo->u4AirDelayMax[1]<<5,
			prInfo->u4AirDelayMin[2]<<5, prInfo->u4AirDelayAvg[2]<<5,
			prInfo->u4AirDelayMax[2]<<5));
		/*                          */
		DBGLOG(INIT, INFO, ("<stats> StayInt us (%u %u %u_%u) (%u %u %u_%u) (%u %u %u_%u)\n",
			prInfo->u4StayIntMin[0], prInfo->u4StayIntAvg[0],
			prInfo->u4StayIntMax[0], prInfo->u4StayIntMaxSysTime[0],
			prInfo->u4StayIntMin[1], prInfo->u4StayIntAvg[1],
			prInfo->u4StayIntMax[1], prInfo->u4StayIntMaxSysTime[1],
			prInfo->u4StayIntMin[2], prInfo->u4StayIntAvg[2],
			prInfo->u4StayIntMax[2], prInfo->u4StayIntMaxSysTime[2]));
		/*                             */
		DBGLOG(INIT, INFO, ("<stats> StayIntD2T us (%u %u %u) (%u %u %u) (%u %u %u)\n",
			prInfo->u4StayIntMinD2T[0], prInfo->u4StayIntAvgD2T[0],
			prInfo->u4StayIntMaxD2T[0],
			prInfo->u4StayIntMinD2T[1], prInfo->u4StayIntAvgD2T[1],
			prInfo->u4StayIntMaxD2T[1],
			prInfo->u4StayIntMinD2T[2], prInfo->u4StayIntAvgD2T[2],
			prInfo->u4StayIntMaxD2T[2]));

		/*                          */
		DBGLOG(INIT, INFO, ("<stats> StayIntR_M2H us (%u %u %u) (%u %u %u) (%u %u %u)\n",
			prInfo->u4StayIntMinRx[0], prInfo->u4StayIntAvgRx[0],
			prInfo->u4StayIntMaxRx[0],
			prInfo->u4StayIntMinRx[1], prInfo->u4StayIntAvgRx[1],
			prInfo->u4StayIntMaxRx[1],
			prInfo->u4StayIntMinRx[2], prInfo->u4StayIntAvgRx[2],
			prInfo->u4StayIntMaxRx[2]));
		/*                      */
		DBGLOG(INIT, INFO, ("<stats> StayIntR_H2D us (%u %u %u) (%u %u %u) (%u %u %u)\n",
			prStaRec->u4StayIntMinRx[0], prStaRec->u4StayIntAvgRx[0],
			prStaRec->u4StayIntMaxRx[0],
			prStaRec->u4StayIntMinRx[1], prStaRec->u4StayIntAvgRx[1],
			prStaRec->u4StayIntMaxRx[1],
			prStaRec->u4StayIntMinRx[2], prStaRec->u4StayIntAvgRx[2],
			prStaRec->u4StayIntMaxRx[2]));

		/*                                                    */
		DBGLOG(INIT, INFO, ("<stats> StayCntD2H unit:%dms (%d %d %d %d)\n",
			STATS_STAY_INT_D2H_CONST,
			prInfo->u4StayIntD2HByConst[0], prInfo->u4StayIntD2HByConst[1],
			prInfo->u4StayIntD2HByConst[2], prInfo->u4StayIntD2HByConst[3]));
		DBGLOG(INIT, INFO, ("<stats> StayCnt unit:%dms (%d %d %d %d)\n",
			STATS_STAY_INT_CONST,
			prInfo->u4StayIntByConst[0], prInfo->u4StayIntByConst[1],
			prInfo->u4StayIntByConst[2], prInfo->u4StayIntByConst[3]));
		DBGLOG(INIT, INFO, ("<stats> StayCnt (%d~%d:%d) (%d~%d:%d) "
			"(%d~%d:%d) (%d~%d:%d) (%d~:%d)\n",
			0, prInfo->u4StayIntMaxPast/4, prInfo->u4StayIntCnt[0],
			prInfo->u4StayIntMaxPast/4, prInfo->u4StayIntMaxPast/2, prInfo->u4StayIntCnt[1],
			prInfo->u4StayIntMaxPast/2, prInfo->u4StayIntMaxPast*3/4, prInfo->u4StayIntCnt[2],
			prInfo->u4StayIntMaxPast*3/4, prInfo->u4StayIntMaxPast, prInfo->u4StayIntCnt[3],
			prInfo->u4StayIntMaxPast, prInfo->u4StayIntCnt[4]));

		/*        */
		DBGLOG(INIT, INFO, ("<stats> OTHER (%u) (%u) (%u) (%u) (%u) (%ums) (%uus)\n",
			prInfo->u4ChanIdleCnt, prAdapter->ucScanTime,
			prInfo->u4NumOfChanChange, prStaRec->u4NumOfNoTxQuota,
			prInfo->ucNumOfPsChange, prInfo->u4PsIntMax,
			u4DrvOwnMax/1000));

		/*       */
		kalMemZero(prStaRec->u4StayIntMinRx, sizeof(prStaRec->u4StayIntMinRx));
		kalMemZero(prStaRec->u4StayIntAvgRx, sizeof(prStaRec->u4StayIntAvgRx));
		kalMemZero(prStaRec->u4StayIntMaxRx, sizeof(prStaRec->u4StayIntMaxRx));
		prStaRec->u4StatsRxPassToOsCnt = 0;
		prStaRec->u4RxReorderFallAheadCnt = 0;
		prStaRec->u4RxReorderFallBehindCnt = 0;
		prStaRec->u4RxReorderHoleCnt = 0;
	}

	STATS_DRIVER_OWN_RESET();
}


/*                                                                            */
/*                                                                            
 
                                                         
                                                                               
                                                         
                                                                            
                                                                             
                                                       
 
                        
 
*/
/*                                                                            */
static WLAN_STATUS
statsInfoEnvRequest(
	ADAPTER_T 	  						*prAdapter,
	VOID			  					*pvSetBuffer,
	UINT_32 		  					u4SetBufferLen,
	UINT_32		  						*pu4SetInfoLen
	)
{
	STATS_CMD_CORE_T *prCmdContent;
	WLAN_STATUS rStatus;


	/*                     */
	prCmdContent = (STATS_CMD_CORE_T *)pvSetBuffer;
	prCmdContent->u4Command = STATS_CORE_CMD_ENV_REQUEST;

	/*                  */
	rStatus = wlanSendSetQueryCmd (
		prAdapter,					/*           */
		CMD_ID_STATS,				/*       */
		TRUE,						/*            */
		FALSE, 			   			/*            */
		FALSE,						/*         */
		NULL,
		NULL,						/*                     */
		sizeof(STATS_CMD_CORE_T),	/*                   */
		(PUINT_8) prCmdContent, 	/*               */
		NULL,						/*                  */
		0							/*                     */
		);

	if (rStatus != WLAN_STATUS_PENDING)
	{
		DBGLOG(INIT, ERROR, ("%s wlanSendSetQueryCmd allocation fail!\n",
			__FUNCTION__));
		return WLAN_STATUS_RESOURCES;
	}

	DBGLOG(INIT, INFO, ("%s cmd ok.\n", __FUNCTION__));
	return WLAN_STATUS_SUCCESS;
}


/*                                                                              
                                     
                                                                                
*/

/*                                                                            */
/*                                                                
 
                                                         
                                                            
                                                
                  
 
              
*/
/*                                                                            */
VOID
statsEventHandle(
	GLUE_INFO_T							*prGlueInfo,
	UINT8								*prInBuf,
	UINT32 								u4InBufLen
	)
{
	UINT32 u4EventId;


	/*              */
//                   
//                                               

	if ((prGlueInfo == NULL) || (prInBuf == NULL))
		return; /*                   */

	/*        */
	u4EventId = *(UINT32 *)prInBuf;
	u4InBufLen -= 4;

//                   
//                                                              

	switch(u4EventId)
	{
		case STATS_HOST_EVENT_ENV_REPORT:
			statsInfoEnvDisplay(prGlueInfo, prInBuf+4, u4InBufLen);
			break;

		default:
			break;
	}
}


/*                                                                            */
/*                                                                                             
 
                                                         
                                            
                  
 
              
*/
/*                                                                            */
VOID
statsEnvReportDetect(
	ADAPTER_T							*prAdapter,
	UINT8								ucStaRecIndex
	)
{
	STA_RECORD_T *prStaRec;
	OS_SYSTIME rCurTime;
	STATS_CMD_CORE_T rCmd;


	prStaRec = cnmGetStaRecByIndex(prAdapter, ucStaRecIndex);
	if (prStaRec == NULL)
		return;

	prStaRec->u4StatsEnvTxCnt ++;
	GET_CURRENT_SYSTIME(&rCurTime);

	if (prStaRec->rStatsEnvTxPeriodLastTime == 0)
	{
		prStaRec->rStatsEnvTxLastTime = rCurTime;
		prStaRec->rStatsEnvTxPeriodLastTime = rCurTime;
		return;
	}

	if (prStaRec->u4StatsEnvTxCnt > STATS_ENV_TX_CNT_REPORT_TRIGGER)
	{
		if (CHECK_FOR_TIMEOUT(rCurTime, prStaRec->rStatsEnvTxLastTime,
			SEC_TO_SYSTIME(STATS_ENV_TX_CNT_REPORT_TRIGGER_SEC)))
		{
			rCmd.ucStaRecIdx = ucStaRecIndex;
			statsInfoEnvRequest(prAdapter, &rCmd, 0, NULL);

			prStaRec->rStatsEnvTxLastTime = rCurTime;
			prStaRec->rStatsEnvTxPeriodLastTime = rCurTime;
			prStaRec->u4StatsEnvTxCnt = 0;
			return;
		}
	}

	if (CHECK_FOR_TIMEOUT(rCurTime, prStaRec->rStatsEnvTxPeriodLastTime,
		SEC_TO_SYSTIME(STATS_ENV_TIMEOUT_SEC)))
	{
		rCmd.ucStaRecIdx = ucStaRecIndex;
		statsInfoEnvRequest(prAdapter, &rCmd, 0, NULL);

		prStaRec->rStatsEnvTxPeriodLastTime = rCurTime;
		return;
	}
}


/*                                                                            */
/*                                                   
 
                                                            
                                                    
                  
 
              
*/
/*                                                                            */
VOID
StatsEnvRxDone(
	STA_RECORD_T						*prStaRec,
	SW_RFB_T							*prSwRfb
	)
{
	UINT32 u4LenId;
	UINT32 u4CurTime, u4DifTime;


	/*              */
	if (prStaRec == NULL)
		return;

	/*                      */
	prStaRec->u4StatsRxPassToOsCnt ++;

	/*                         */
	u4LenId = 0;
	if ((0 <= prSwRfb->u2PacketLen) &&
		(prSwRfb->u2PacketLen < STATS_STAY_INT_BYTE_THRESHOLD))
	{
		u4LenId = 0;
	}
	else
	{
		if ((STATS_STAY_INT_BYTE_THRESHOLD <= prSwRfb->u2PacketLen) &&
			(prSwRfb->u2PacketLen < (STATS_STAY_INT_BYTE_THRESHOLD<<1)))
		{
			u4LenId = 1;
		}
		else
			u4LenId = 2;
	}

	/*                 */
	u4CurTime = kalGetTimeTick();

	if ((u4CurTime > prSwRfb->rRxTime) &&
		(prSwRfb->rRxTime != 0))
	{
		u4DifTime = u4CurTime - prSwRfb->rRxTime;

		if (prStaRec->u4StayIntMinRx[u4LenId] == 0) /*            */
			prStaRec->u4StayIntMinRx[u4LenId] = 0xffffffff;

		if (u4DifTime > prStaRec->u4StayIntMaxRx[u4LenId])
			prStaRec->u4StayIntMaxRx[u4LenId] = u4DifTime;
		else if (u4DifTime < prStaRec->u4StayIntMinRx[u4LenId])
			prStaRec->u4StayIntMinRx[u4LenId] = u4DifTime;

		prStaRec->u4StayIntAvgRx[u4LenId] += u4DifTime;
		if (prStaRec->u4StayIntAvgRx[u4LenId] != u4DifTime)
			prStaRec->u4StayIntAvgRx[u4LenId] >>= 1;
	}
}


/*                                                                            */
/*                                                   
 
                                                         
                                                            
                                                
                  
 
              
*/
/*                                                                            */
UINT_64
StatsEnvTimeGet(
	VOID
	)
{
	/*                                                                                */

//                   


//                      
//                                                  

	UINT_64 u8Clk;
//                       

	u8Clk = sched_clock(); /*                    */
//                                                                                

	return (UINT_64)u8Clk; /*             */ /*                   */
}


/*                                                                            */
/*                                                   
 
                                                         
                                                            
                                                
                  
 
              
*/
/*                                                                            */
VOID
StatsEnvTxTime2Hif(
	MSDU_INFO_T							*prMsduInfo,
	HIF_TX_HEADER_T						*prHwTxHeader
	)
{
	UINT_64 u8SysTime, u8SysTimeIn;
	UINT32 u4TimeDiff;


	u8SysTime = StatsEnvTimeGet();
	u8SysTimeIn = GLUE_GET_PKT_XTIME(prMsduInfo->prPacket);

//                                       
//                                                                                                

	if ((u8SysTimeIn > 0) && (u8SysTime > u8SysTimeIn))
	{
		u8SysTime = u8SysTime - u8SysTimeIn;
		u4TimeDiff = (UINT32)u8SysTime;
		u4TimeDiff = u4TimeDiff/1000; /*          */

		if (u4TimeDiff > 0xFFFF)
			*(UINT16 *)prHwTxHeader->aucReserved = (UINT16)0xFFFF; /*          */
		else
			*(UINT16 *)prHwTxHeader->aucReserved = (UINT16)u4TimeDiff;

//                                                 
	}
	else
	{
		prHwTxHeader->aucReserved[0] = 0;
		prHwTxHeader->aucReserved[1] = 0;
	}
}


#endif /*                        */

/*                */

