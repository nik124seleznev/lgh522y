/*
                                                                                             
*/

/*                  
                                                                                  

                                                                                           
*/



/*
                   
  
                        
                                                                                  
          
  
                                                                                                
  
                                                                                    
  
                        
                                                                                  
                               
  
                        
                                                
                      
  
                   
                                                                                              
                                            
                                                                   
                                                   
  
                   
                                                                       
                                                    
  
                   
                                                                                     
                                  
                                                                  
  
                      
                                                         
                                                  
  
                   
                                                                                              
                                        
  
                      
                                                         
                                                        
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                                
    
  
                      
                                                                      
                                       
  
                        
                                                                                                                                                                                                     
                          
  
                   
                                                                                                                                                    
                                                                
  
                      
                                                                           
                                     
  
                   
                                                                                                                                                    
                                                                                               
                                                       
  
                   
                                                                                                                                    
                                                                                                      
  
                   
                                                                                                                                                         
                                                                          
                                                                         
  
                                                                                                
  
                   
                                                                                                                                    
                                                                                                                       
  
                   
                                                                                                                                                                    
                                                                                     
  
                   
                                                                                                                                                                    
                                                
  
                        
                                                                                      
                                    
  
                   
                                                                                                                                                   
                                                                                                             
  
                   
                                                                                                                                  
                                                                                                                          
  
                        
       
                                               
  
                   
                                                                                                                            
                                                                                                                                                  
                                                                                                                    
  
                   
                                                                                                                      
                                                                                       
  
                   
                                                                                                                                                                                                      
                                                                                                   
                                          
                                                                          
  
                   
                                                                                                                                                              
                                                  
  
                   
                                                                                 
                          
                                                                                                     
                                                         
  
                   
                                                                                     
                                     
  
                   
                                                                                     
                                          
  
                   
                                                                                                                                 
                                         
  
                   
       
                                                                            
  
                   
                                                                                                                            
                                                                      
  
                   
       
                                       
  
                   
       
                                                                 
  
                   
       
                                                                                                
  
                         
       
                                                                     
  
                   
       
                                                                     
  
                   
       
                                                                                          
  
                   
       
                                                
  
                   
       
                                                             
  
                   
  
                                                
                           
  
                   
  
                                                  
  
                   
  
                                                                               
  
                   
                                                    
                                                                                                                        
  
                   
                                                    
                                     
  
                      
                                                            
                                                              
  
                   
                                                    
                      
  
                   
                                                    
                      
                                                                      
                                                           
                                         
                                          
  
                   
                                                    
                               
  
                   
                                                    
                
  
                         
                                                
                                   
  
                   
                                                          
                                     
  
                   
                                                                     
                                                              
  
                   
                                                                     
                                                            
  
                   
                                                                     
                                         
  
                   
                                                               
                                                                       
                                                    
  
                   
                                                               
                                     
  
                   
                                                          
                                                            
                                                                        
  
                   
                                                          
                                                                      
                                                                                                     
  
                   
                                                               
                                           
                                                                                                                                          
                                                                                                                        
                                                                                                                               
  
                   
                                                          
                                                 
  
                   
                                                          
                                                                                   
                                               
  
                   
                                                          
                                                                                
                                                                    
  
                   
                                                          
                                                                                       
  
                   
                                                          
                                                                   
  
                   
                                                          
                                                                                                    
  
                   
                                                          
                                                                                                      
  
                   
                                                          
                                               
                                                           
  
                   
                                                          
                                            
                                                                                                                                     
  
                   
                                                          
                                                                                                    
                                                                                           
  
                   
                                                          
                                                                   
  
                   
                                                          
                                                                                                                   
  
                   
                                                          
                                                                                   
                                                                                     
  
                   
                                                          
                                                                  
  
                   
                                                          
                                                             
                                                                   
  
                   
                                                          
                                                                              
                                                                                                    
                                                                            
                     
                                                                            
                                       
  
                                                                                                                                                                                   
  
                                                                            
                                                 
                                                                            
                                                                            
                         
                                                                            
                                           
                                                                            
                                               
                                                                            
                                                
                                                                            
                                       
                                                                            
                     
                                                                           
                                                                                                       
                                                                           
                          
                                                                           
                                    
                                                                           
                                                                              
                                                                           
                                              
                                                                           
                                                                                    
                                                                           
                      
                                                                           
                    
  
*/

#ifndef _GL_KAL_H
#define _GL_KAL_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "config.h"
#include "gl_typedef.h"
#include "gl_os.h"
#include "nic/mac.h"
#include "nic/wlan_def.h"
#include "wlan_lib.h"
#include "wlan_oid.h"
#include "queue.h"


#if CFG_ENABLE_BT_OVER_WIFI
#include "nic/bow.h"
#endif

/*                                                                              
                                                
                                                                                
*/
#define USEC_PER_MSEC   (1000)

#define KAL_BSS_NUM     2
#define KAL_P2P_NUM     1

/*                                                                              
                                                 
                                                                                
*/
typedef enum _ENUM_SPIN_LOCK_CATEGORY_E {
	SPIN_LOCK_RX_QUE = 0,
	SPIN_LOCK_RX_RETURN_QUE,
	SPIN_LOCK_TX_QUE,
	SPIN_LOCK_TX_RESOURCE,
	SPIN_LOCK_TX_MSDU_INFO_LIST,
	SPIN_LOCK_TXING_MGMT_LIST,
	SPIN_LOCK_TX_SEQ_NUM,
	SPIN_LOCK_QM_TX_QUEUE,
	SPIN_LOCK_CMD_QUE,
	SPIN_LOCK_CMD_RESOURCE,
	SPIN_LOCK_CMD_PENDING,
	SPIN_LOCK_CMD_SEQ_NUM,
#if defined(_HIF_SPI)
	SPIN_LOCK_SPI_ACCESS,
#endif
	SPIN_LOCK_MGT_BUF,
	SPIN_LOCK_MSG_BUF,
	SPIN_LOCK_STA_REC,

	SPIN_LOCK_MAILBOX,
	SPIN_LOCK_TIMER,

	SPIN_LOCK_NUM
} ENUM_SPIN_LOCK_CATEGORY_E;

/*                                   */
typedef struct _EVENT_ASSOC_INFO {
	UINT_8 ucAssocReq;	/*                                  */
	UINT_8 ucReassoc;	/*                            */
	UINT_16 u2Length;
	PUINT_8 pucIe;
} EVENT_ASSOC_INFO, *P_EVENT_ASSOC_INFO;

typedef enum _ENUM_KAL_NETWORK_TYPE_INDEX_T {
	KAL_NETWORK_TYPE_AIS_INDEX = 0,
#if CFG_ENABLE_WIFI_DIRECT
	KAL_NETWORK_TYPE_P2P_INDEX,
#endif
#if CFG_ENABLE_BT_OVER_WIFI
	KAL_NETWORK_TYPE_BOW_INDEX,
#endif
	KAL_NETWORK_TYPE_INDEX_NUM
} ENUM_KAL_NETWORK_TYPE_INDEX_T;

typedef enum _ENUM_KAL_MEM_ALLOCATION_TYPE_E {
	PHY_MEM_TYPE,		/*                       */
	VIR_MEM_TYPE,		/*                     */
	MEM_TYPE_NUM
} ENUM_KAL_MEM_ALLOCATION_TYPE;

/*                                     */
typedef UINT_32 KAL_WAKE_LOCK_T, *P_KAL_WAKE_LOCK_T;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
/*                                                                            */
/*                                                                            */
/*                                                                            */
#if defined(WINDOWS_DDK)
#define KAL_CREATE_THREAD(pvThreadHandle, pfnThreadProc, pvArg, ppvKThread) \
    DBGLOG(INIT, TRACE, ("KAL_CREATE_THREAD\n")); \
    PsCreateSystemThread(&pvThreadHandle, \
			 (ACCESS_MASK)0, \
			 NULL, \
			 (HANDLE) 0, \
			 NULL, \
			 pfnThreadProc, \
			 pvArg); \
    ObReferenceObjectByHandle(pvThreadHandle, THREAD_ALL_ACCESS, NULL, KernelMode, \
			    (PVOID *) ppvKThread, NULL);

#define KAL_KILL_THREAD(pvEvent, pKThread) \
	NdisSetEvent(pvEvent); \
        DBGLOG(INIT, TRACE, ("Notify TxServiceThread to terminate it\n")); \
	if (pKThread) { \
	    KeWaitForSingleObject(pKThread, \
				Executive, \
				KernelMode, \
				FALSE, \
				NULL); \
	    ObDereferenceObject(pKThread); \
	    /*                              */\
	}



#define KAL_WAIT_FOR_SINGLE_OBJECT(pvThreadHandle) \
    KeWaitForSingleObject(pvThreadHandle, \
			Executive, \
			KernelMode, \
			FALSE, \
			NULL);

#define KAL_CLOSE_HANDLE(pvThreadHandle) \
    ObDereferenceObject(pvThreadHandle);

#endif

#if defined(WINDOWS_CE)
#define KAL_CREATE_THREAD(pvThreadHandle, pfnThreadProc, pvArg) \
				/*                                                                 */
CreateThread(NULL, 0, pfnThreadProc, pvArg, 0, NULL)
#define KAL_WAIT_FOR_SINGLE_OBJECT(pvThreadHandle) \
    WaitForSingleObject(pvThreadHandle, INFINITE);
#define KAL_CLOSE_HANDLE(pvThreadHandle) \
    CloseHandle(pvThreadHandle);
#endif
/*                                                                            */
/*                                                                            */
/*                                                                            */
#define KAL_SPIN_LOCK_DECLARATION() \
	    GLUE_SPIN_LOCK_DECLARATION()
#define KAL_ACQUIRE_SPIN_LOCK(_prAdapter, _rLockCategory)   \
	    GLUE_ACQUIRE_SPIN_LOCK((_prAdapter)->prGlueInfo, _rLockCategory)
#define KAL_RELEASE_SPIN_LOCK(_prAdapter, _rLockCategory)   \
	    GLUE_RELEASE_SPIN_LOCK((_prAdapter)->prGlueInfo, _rLockCategory)
/*                                                                            */
/*                                                                            */
/*                                                                            */
#define KAL_WAIT_EVENT(_prAdapter)          GLUE_WAIT_EVENT((_prAdapter)->prGlueInfo)
#define KAL_RESET_EVENT(_prAdapter)         GLUE_RESET_EVENT((_prAdapter)->prGlueInfo)
#define KAL_SET_EVENT(_prAdapter)           GLUE_SET_EVENT((_prAdapter)->prGlueInfo)
/*                                                                            */
/*                                                                            */
/*                                                                            */
#define KAL_GET_PKT_QUEUE_ENTRY(_p)             GLUE_GET_PKT_QUEUE_ENTRY(_p)
#define KAL_GET_PKT_DESCRIPTOR(_prQueueEntry)   GLUE_GET_PKT_DESCRIPTOR(_prQueueEntry)
#define KAL_GET_PKT_TID(_p)                     GLUE_GET_PKT_TID(_p)
#define KAL_GET_PKT_IS_802_11(_p)               GLUE_GET_PKT_IS_802_11(_p)
#define KAL_GET_PKT_IS_1X(_p)                   GLUE_GET_PKT_IS_1X(_p)
#define KAL_GET_PKT_IS_PAL(_p)                  GLUE_GET_PKT_IS_PAL(_p)
#define KAL_GET_PKT_HEADER_LEN(_p)              GLUE_GET_PKT_HEADER_LEN(_p)
#define KAL_GET_PKT_FRAME_LEN(_p)               GLUE_GET_PKT_FRAME_LEN(_p)
#define KAL_GET_PKT_ARRIVAL_TIME(_p)            GLUE_GET_PKT_ARRIVAL_TIME(_p)
/*                                                                            */
/*                                                                            */
/*                                                                            */
#define KAL_WAKE_LOCK_INIT(_prAdapter, _prWakeLock, _pcName)
#define KAL_WAKE_LOCK_DESTROY(_prAdapter, _prWakeLock)
#define KAL_WAKE_LOCK(_prAdapter, _prWakeLock)
#define KAL_WAKE_LOCK_TIMEOUT(_prAdapter, _prWakeLock, _u4Timeout)
#define KAL_WAKE_UNLOCK(_prAdapter, _prWakeLock)
/*                                                                      */
#ifdef _lint
VOID NdisZeroMemory(IN PVOID Destination, IN ULONG Length);

VOID NdisMoveMemory(OUT PVOID Destination, IN PVOID Source, IN ULONG Length);

VOID NdisFillMemory(IN PVOID Destination, IN ULONG Length, IN UCHAR Fill);

ULONG NdisEqualMemory(PVOID Source1, PVOID Source2, ULONG Length);
#endif				/*       */


/*                                       */
#define kalMemCopy(pvDst, pvSrc, u4Size)            NdisMoveMemory(pvDst, pvSrc, u4Size)

/*                                         */
#define kalMemSet(pvAddr, ucPattern, u4Size)        NdisFillMemory(pvAddr, u4Size, ucPattern)

/*                                                
                                    
 */
#define kalMemCmp(pvAddr1, pvAddr2, u4Size)         !NdisEqualMemory(pvAddr1, pvAddr2, u4Size)

/*                             */
#define kalMemZero(pvAddr, u4Size)                  NdisZeroMemory(pvAddr, u4Size)


#if CFG_SDIO_TX_AGG
#define kalGetSDIOWriteBlkSize(prGlueInfo)          prGlueInfo->rHifInfo.sdHostBlockCap.WriteBlockSize
#define kalGetSDIOWriteBlkBitSize(prGlueInfo)       prGlueInfo->rHifInfo.WBlkBitSize
#else
#define kalGetSDIOWriteBlkSize(prGlueInfo)
#define kalGetSDIOWriteBlkBitSize(prGlueInfo)
#endif

#if defined(_HIF_SDIO) && defined(WINDOWS_CE)
/*                                     */
#define kalDevSetPowerState(prGlueInfo, ePowerMode) sdioSetPowerState(prGlueInfo, ePowerMode)
#else
#define kalDevSetPowerState(prGlueInfo, ePowerMode)
#endif



/*                                                                           
                                                                     
                                                                         
         
 */
#define OS_DEBUG_MSG                0
#define FILE_DEBUG_MSG              1
#define MSG_DEBUG_MSG               2	/*                  */

/*                                         */
#define LOG_METHOD                  OS_DEBUG_MSG

#ifdef WINDOWS_CE
#define UNICODE_MESSAGE         1
#else
#define UNICODE_MESSAGE         0
#endif

#if !DBG
static inline void CREATE_LOG_FILE(void)
{
};

static inline void kalPrint(PUINT_8 dbgstr, ...)
{
};

#elif (LOG_METHOD == FILE_DEBUG_MSG) || (LOG_METHOD == MSG_DEBUG_MSG)
#define CREATE_LOG_FILE()       dbgFileCreate()
#define kalPrint                dbgLogWr
#else				/*                */
#if UNICODE_MESSAGE
#define CREATE_LOG_FILE()
#define kalPrint                dbgLogWr2
#else				/*                 */
#define CREATE_LOG_FILE()
#define kalPrint                DbgPrint
#endif				/*                  */
#endif				/*            */

#define kalBreakPoint()             DbgBreakPoint()

/*                                                                 */
#define SPRINTF(buf, arg)           kalPrint arg

#define USEC_TO_SYSTIME(_usec)      ((_usec) / USEC_PER_MSEC)
#define MSEC_TO_SYSTIME(_msec)      (_msec)

#define SYSTIME_TO_MSEC(_systime)   (_systime)

#define kalIndicateBssInfo(prGlueInfo, pucFrameBuf, u4BufLen, ucChannelNum, i4SignalStrength)


/*                                                                              
                                                            
                                                                                
*/
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                    */
PVOID kalMemAlloc(IN UINT_32 u4Size, IN ENUM_KAL_MEM_ALLOCATION_TYPE eMemType);

/*                                                   */
OS_SYSTIME kalGetTimeTick(VOID);

/*                                          */
UINT_64 kalGetNanoTick(VOID);

VOID kalCopyFrame(IN P_GLUE_INFO_T prGlueInfo, IN PVOID pvPacket, IN PUINT_8 pucDestBuffer);

PVOID kalPacketAlloc(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Size, OUT PUINT_8 *ppucData);

VOID kalPacketFree(IN P_GLUE_INFO_T prGlueInfo, IN PVOID pvPacket);

WLAN_STATUS
kalProcessRxPacket(IN P_GLUE_INFO_T prGlueInfo,
		   IN PVOID pvPacket,
		   IN PUINT_8 pucPacketStart,
		   IN UINT_32 u4PacketLen, IN BOOL fgIsRetain, IN ENUM_CSUM_RESULT_T aeCSUM[]
    );

WLAN_STATUS kalRxIndicatePkts(IN P_GLUE_INFO_T prGlueInfo, IN PVOID apvPkts[], IN UINT_32 ucPktNum);

VOID
kalIndicateStatusAndComplete(IN P_GLUE_INFO_T prGlueInfo,
			     IN WLAN_STATUS eStatus, IN PVOID pvBuf, IN UINT_32 u4BufLen);

VOID
kalUpdateReAssocReqInfo(IN P_GLUE_INFO_T prGlueInfo,
			IN PUINT_8 pucFrameBody,
			IN UINT_32 u4FrameBodyLen, IN BOOLEAN fgReassocRequest);

VOID
kalUpdateReAssocRspInfo(IN P_GLUE_INFO_T prGlueInfo,
			IN PUINT_8 pucFrameBody, IN UINT_32 u4FrameBodyLen);

#if CFG_TCP_IP_CHKSUM_OFFLOAD
VOID kalQueryTxChksumOffloadParam(IN PVOID pvPacket, OUT PUINT_8 pucFlag);


VOID kalUpdateRxCSUMOffloadParam(IN PVOID pvPacket, IN ENUM_CSUM_RESULT_T aeCSUM[]
    );
#endif				/*                           */


VOID kalSendComplete(IN P_GLUE_INFO_T prGlueInfo, IN PVOID pvPacket, IN WLAN_STATUS rStatus);

VOID kalHandleAssocInfo(IN P_GLUE_INFO_T prGlueInfo, IN P_EVENT_ASSOC_INFO prAssocInfo);


/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                     */
VOID kalMemFree(IN PVOID pvAddr, IN ENUM_KAL_MEM_ALLOCATION_TYPE eMemType, IN UINT_32 u4Size);

/*                                             */
VOID kalUdelay(IN UINT_32 u4MicroSec);

/*                                             */
VOID kalMdelay(IN UINT_32 u4MilliSec);

VOID kalMsleep(IN UINT_32 u4MilliSec);

VOID kalSetEvent(IN P_GLUE_INFO_T prGlueInfo);

VOID kalInterruptDone(IN P_GLUE_INFO_T prGlueInfo);

WLAN_STATUS kalTxServiceThread(IN PVOID pvGlueContext);

#if 0
WLAN_STATUS kalDownloadPatch(IN PNDIS_STRING FileName);
#endif

VOID
kalOidComplete(IN P_GLUE_INFO_T prGlueInfo,
	       IN BOOLEAN fgSetQuery, IN UINT_32 u4SetQueryInfoLen, IN WLAN_STATUS rOidStatus);

BOOLEAN
kalGetEthDestAddr(IN P_GLUE_INFO_T prGlueInfo,
		  IN P_NATIVE_PACKET prPacket, OUT PUINT_8 pucEthDestAddr);

BOOL
kalQoSFrameClassifierAndPacketInfo(IN P_GLUE_INFO_T prGlueInfo,
				   IN P_NATIVE_PACKET prPacket,
				   OUT PUINT_8 pucPriorityParam,
				   OUT PUINT_32 pu4PacketLen,
				   OUT PUINT_8 pucEthDestAddr,
				   OUT PBOOLEAN pfgIs1X,
				   OUT PBOOLEAN pfgIsPAL,
				   OUT PBOOLEAN pfgIs802_3, OUT PBOOLEAN pfgIsVlanExists);


VOID kalUpdateMACAddress(IN P_GLUE_INFO_T prGlueInfo, IN PUINT_8 pucMacAddr);


UINT_32 kalWriteToFile(const PUINT_8 pucPath, BOOLEAN fgDoAppend, PUINT_8 pucData, UINT_32 u4Size);

UINT_32
kal_sprintf_ddk(const PUINT_8 pucPath,
		UINT_32 u4size, const char *fmt1, const char *fmt2, const char *fmt3);

UINT_32 kal_sprintf_done_ddk(const PUINT_8 pucPath, UINT_32 u4size);


#ifdef WINDOWS_CE
/*                                                                            */
/*                                                                            */
/*                                                                            */
#if DBG
#if (LOG_METHOD == FILE_DEBUG_MSG) || (LOG_METHOD == MSG_DEBUG_MSG)

NDIS_STATUS dbgFileCreate(VOID);

NDIS_STATUS dbgLogWr(IN PINT_8 debugStr, IN ...
    );

#elif UNICODE_MESSAGE

NDIS_STATUS dbgLogWr2(IN PINT_8 debugStr, IN ...
    );

#endif
#endif				/*     */
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
BOOL kalDevRegRead(P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, OUT PUINT_32 pu4Value);

BOOL kalDevRegWrite(P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Register, IN UINT_32 u4Value);

BOOL
kalDevPortRead(IN P_GLUE_INFO_T prGlueInfo,
	       IN UINT_32 u4Port,
	       IN UINT_32 u4Len, OUT PUINT_8 pucBuf, IN UINT_32 u4ValidOutBufSize);

BOOL
kalDevPortWrite(IN P_GLUE_INFO_T prGlueInfo,
		IN UINT_32 u4Port,
		IN UINT_32 u4Len, IN PUINT_8 pucBuf, IN UINT_32 u4ValidInBufSize);

BOOL kalDevWriteWithSdioCmd52(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Addr, IN UINT_8 ucData);

BOOL
kalDevReadAfterWriteWithSdioCmd52(IN P_GLUE_INFO_T prGlueInfo,
				  IN UINT_32 u4Addr,
				  IN OUT PUINT_8 pucRwBuffer, IN UINT_32 u4RwBufLen);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
kalTimerEvent(IN PVOID systemSpecific1,
	      IN PVOID miniportAdapterContext, IN PVOID systemSpecific2, IN PVOID systemSpecific3);

VOID kalOsTimerInitialize(IN P_GLUE_INFO_T prGlueInfo, IN PVOID prTimerHandler);

BOOLEAN kalSetTimer(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Interval);

BOOLEAN kalCancelTimer(IN P_GLUE_INFO_T prGlueInfo);

VOID kalTimeoutHandler(IN P_GLUE_INFO_T prGlueInfo);


/*                                                                            */
/*                                                                            */
/*                                                                            */
PVOID
kalFirmwareImageMapping(IN P_GLUE_INFO_T prGlueInfo,
			OUT PPVOID ppvMapFileBuf, OUT PUINT_32 pu4FileLength);

VOID
kalFirmwareImageUnmapping(IN P_GLUE_INFO_T prGlueInfo, IN PVOID prFwHandle, IN PVOID pvMapFileBuf);


/*                                                                            */
/*                                                                            */
/*                                                                            */
BOOLEAN kalIsCardRemoved(IN P_GLUE_INFO_T prGlueInfo);


/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID kalFlushPendingTxPackets(IN P_GLUE_INFO_T prGlueInfo);

UINT_32 kalGetTxPendingFrameCount(IN P_GLUE_INFO_T prGlueInfo);

UINT_32 kalGetTxPendingCmdCount(IN P_GLUE_INFO_T prGlueInfo);


/*                                                                            */
/*                                                                            */
/*                                                                            */
ENUM_PARAM_MEDIA_STATE_T kalGetMediaStateIndicated(IN P_GLUE_INFO_T prGlueInfo);


VOID
kalSetMediaStateIndicated(IN P_GLUE_INFO_T prGlueInfo,
			  IN ENUM_PARAM_MEDIA_STATE_T eParamMediaStateIndicate);


/*                                                                            */
/*                                                                            */
/*                                                                            */
UINT_32 kalGetFwLoadAddress(IN P_GLUE_INFO_T prGlueInfo);

UINT_32 kalGetFwStartAddress(IN P_GLUE_INFO_T prGlueInfo);


/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID kalOidCmdClearance(IN P_GLUE_INFO_T prGlueInfo);

VOID kalOidClearance(IN P_GLUE_INFO_T prGlueInfo);

VOID kalEnqueueCommand(IN P_GLUE_INFO_T prGlueInfo, IN P_QUE_ENTRY_T prQueueEntry);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID kalClearSecurityFrames(IN P_GLUE_INFO_T prGlueInfo);

VOID kalClearSecurityFramesByBssIdx(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucBssIndex);

VOID
kalSecurityFrameSendComplete(IN P_GLUE_INFO_T prGlueInfo,
			     IN PVOID pvPacket, IN WLAN_STATUS rStatus);


/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID kalClearMgmtFrames(IN P_GLUE_INFO_T prGlueInfo);


/*                                                                            */
/*                                                                            */
/*                                                                            */
UINT_32 kalRandomNumber(VOID);


/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
kalScanDone(IN P_GLUE_INFO_T prGlueInfo,
	    IN ENUM_KAL_NETWORK_TYPE_INDEX_T eNetTypeIdx, IN WLAN_STATUS rStatus);

#if CFG_ENABLE_BT_OVER_WIFI
/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID kalIndicateBOWEvent(IN P_GLUE_INFO_T prGlueInfo, IN P_AMPC_EVENT prEvent);

ENUM_BOW_DEVICE_STATE kalGetBowState(IN P_GLUE_INFO_T prGlueInfo, IN PARAM_MAC_ADDRESS rPeerAddr);

VOID
kalSetBowState(IN P_GLUE_INFO_T prGlueInfo,
	       IN ENUM_BOW_DEVICE_STATE eBowState, IN PARAM_MAC_ADDRESS rPeerAddr);

ENUM_BOW_DEVICE_STATE kalGetBowGlobalState(IN P_GLUE_INFO_T prGlueInfo);

UINT_32 kalGetBowFreqInKHz(IN P_GLUE_INFO_T prGlueInfo);

UINT_8 kalGetBowRole(IN P_GLUE_INFO_T prGlueInfo, IN PARAM_MAC_ADDRESS rPeerAddr);

VOID kalSetBowRole(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucRole, IN PARAM_MAC_ADDRESS rPeerAddr);

UINT_32 kalGetBowAvailablePhysicalLinkCount(IN P_GLUE_INFO_T prGlueInfo);
#endif

#if CFG_ENABLE_WIFI_DIRECT
/*                                                                            */
/*                                                                            */
/*                                                                            */
ENUM_PARAM_MEDIA_STATE_T kalP2PGetState(IN P_GLUE_INFO_T prGlueInfo);

VOID
kalP2PSetState(IN P_GLUE_INFO_T prGlueInfo,
	       IN ENUM_PARAM_MEDIA_STATE_T eState,
	       IN PARAM_MAC_ADDRESS rPeerAddr, IN UINT_8 ucRole);

VOID
kalP2PUpdateAssocInfo(IN P_GLUE_INFO_T prGlueInfo,
		      IN PUINT_8 pucFrameBody,
		      IN UINT_32 u4FrameBodyLen, IN BOOLEAN fgReassocRequest);

UINT_32 kalP2PGetFreqInKHz(IN P_GLUE_INFO_T prGlueInfo);

UINT_8 kalP2PGetRole(IN P_GLUE_INFO_T prGlueInfo);

VOID
kalP2PSetRole(IN P_GLUE_INFO_T prGlueInfo,
	      IN UINT_8 ucResult, IN PUINT_8 pucSSID, IN UINT_8 ucSSIDLen, IN UINT_8 ucRole);

BOOLEAN kalP2PIndicateFound(IN P_GLUE_INFO_T prGlueInfo);

VOID kalP2PIndicateConnReq(IN P_GLUE_INFO_T prGlueInfo, IN PUINT_8 pucDevName, IN INT_32 u4NameLength, IN PARAM_MAC_ADDRESS rPeerAddr, IN UINT_8 ucDevType,	/*                               */
			   IN INT_32 i4ConfigMethod, IN INT_32 i4ActiveConfigMethod);

VOID
kalP2PInvitationIndication(IN P_GLUE_INFO_T prGlueInfo,
			   IN P_P2P_DEVICE_DESC_T prP2pDevDesc,
			   IN PUINT_8 pucSsid,
			   IN UINT_8 ucSsidLen,
			   IN UINT_8 ucOperatingChnl, IN UINT_8 ucInvitationType);

VOID kalP2PSetCipher(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Cipher);

BOOLEAN kalP2PGetCipher(IN P_GLUE_INFO_T prGlueInfo);

UINT_16 kalP2PCalWSC_IELen(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucType);

VOID kalP2PGenWSC_IE(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucType, IN PUINT_8 pucBuffer);
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
BOOLEAN kalIsConfigurationExist(IN P_GLUE_INFO_T prGlueInfo);

P_REG_INFO_T kalGetConfiguration(IN P_GLUE_INFO_T prGlueInfo);

VOID
kalGetConfigurationVersion(IN P_GLUE_INFO_T prGlueInfo,
			   OUT PUINT_16 pu2Part1CfgOwnVersion,
			   OUT PUINT_16 pu2Part1CfgPeerVersion,
			   OUT PUINT_16 pu2Part2CfgOwnVersion, OUT PUINT_16 pu2Part2CfgPeerVersion);

BOOLEAN kalCfgDataRead16(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Offset, OUT PUINT_16 pu2Data);

BOOLEAN kalCfgDataWrite16(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Offset, IN UINT_16 u2Data);

BOOLEAN
kalRetrieveNetworkAddress(IN P_GLUE_INFO_T prGlueInfo, IN OUT PARAM_MAC_ADDRESS * prMacAddr);

/*                                                                            */
/*                                                                    */
/*                                                                            */
BOOLEAN kalWSCGetActiveState(IN P_GLUE_INFO_T prGlueInfo);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
kalUpdateRSSI(IN P_GLUE_INFO_T prGlueInfo,
	      IN ENUM_KAL_NETWORK_TYPE_INDEX_T eNetTypeIdx, IN INT_8 cRssi, IN INT_8 cLinkQuality);

/*                                                                            */
/*                                                                            */
/*                                                                            */
PVOID kalAllocateIOBuffer(IN UINT_32 u4AllocSize);

VOID kalReleaseIOBuffer(IN PVOID pvAddr, IN UINT_32 u4Size);


/*                                                                            */
/*                                                                            */
/*                                                                            */
#if CFG_CHIP_RESET_SUPPORT
VOID glSendResetRequest(VOID);

BOOLEAN kalIsResetting(VOID);

#endif

#if CFG_SUPPORT_SDIO_READ_WRITE_PATTERN
/*                                                                            */
/*                                                                            */
/*                                                                            */
BOOLEAN kalSetSdioTestPattern(IN P_GLUE_INFO_T prGlueInfo, IN BOOLEAN fgEn, IN BOOLEAN fgRead);
#endif


/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
kalReadyOnChannel(IN P_GLUE_INFO_T prGlueInfo,
		  IN UINT_64 u8Cookie,
		  IN ENUM_BAND_T eBand,
		  IN ENUM_CHNL_EXT_T eSco, IN UINT_8 ucChannelNum, IN UINT_32 u4DurationMs);

VOID
kalRemainOnChannelExpired(IN P_GLUE_INFO_T prGlueInfo,
			  IN UINT_64 u8Cookie,
			  IN ENUM_BAND_T eBand, IN ENUM_CHNL_EXT_T eSco, IN UINT_8 ucChannelNum);

VOID
kalIndicateMgmtTxStatus(IN P_GLUE_INFO_T prGlueInfo,
			IN UINT_64 u8Cookie,
			IN BOOLEAN fgIsAck, IN PUINT_8 pucFrameBuf, IN UINT_32 u4FrameLen);

VOID kalIndicateRxMgmtFrame(IN P_GLUE_INFO_T prGlueInfo, IN P_SW_RFB_T prSwRfb);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID kalSchedScanResults(IN P_GLUE_INFO_T prGlueInfo);

VOID kalSchedScanStopped(IN P_GLUE_INFO_T prGlueInfo);

/*                                                                              
                                                
                                                                                
*/

#endif				/*           */
