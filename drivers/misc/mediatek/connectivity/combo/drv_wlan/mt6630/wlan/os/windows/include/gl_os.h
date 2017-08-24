/*
                                                                                            
*/

/*                 
                                                             

                                                                                  
                                                                            
                                                                                  
                        
*/



/*
                  
  
                   
                                                                       
                                                    
  
                   
                                                                                     
                                  
                                                                  
  
                      
                                                         
    
  
                      
                                                         
                                                  
  
                      
                                                         
                                                        
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                                                     
                                                                        
  
                           
                                                                                
    
  
                   
                                                                         
                                                     
  
                   
                                                                                                                                                         
                                               
                                                                   
                                                                                                       
                                                                           
                                            
                                                      
  
                         
       
                                                                     
  
                         
       
                                                 
  
                   
       
                                                                                                              
  
                   
       
                                                                          
                                                   
  
                   
  
                        
  
                   
  
                                                                               
  
                   
                                                    
                                                                                                                        
  
                   
                                                    
                                                                           
  
                         
                                                
                                   
  
                   
                                                               
                                                                    
  
                   
                                                          
                                                                                
                                                                       
  
                   
                                                          
                                                  
  
                   
                                                          
                                                                                                      
  
                   
                                                          
                                              
  
                   
                                                          
                                                                       
                                                              
                                                              
  
                   
                                                               
                                             
  
                   
                                                          
                                                                              
                                                                                                       
  
                   
                                                          
    
  
                   
                                                          
                                             
                                                                                                  
                                                                                                                                     
                                                                               
  
                   
                                                          
                                                                                                                 
  
                   
                                                          
                                   
                                     
                                             
                                           
                                             
                                          
  
                   
                                                          
                                         
  
                   
                                                          
                                                                  
  
                   
                                                          
                                                 
                                                                            
              
                                                                            
                                                
                                                                            
                                                           
                                                                            
                         
                                                                            
                                                                                                        
                                                                            
                         
                                                                            
                                                                            
                                                                            
                            
                                                                            
                  
                                                                           
                                                                             
                                                                           
                                                                      
                                                                           
                                              
                                                                           
                                                  
                                                                           
                                     
                                                                           
                                   
                                                                           
                      
                                                                           
                    
  
*/


#ifndef _GL_OS_H
#define _GL_OS_H

/*                                                                              
                                                     
                                                                                
*/


/*                                                                              
                                                          
                                                                                
*/
#include "config.h"

LINT_EXT_HEADER_BEGIN
#include <ndis.h>
    LINT_EXT_HEADER_END
#include "version.h"
#include "gl_typedef.h"
#include "typedef.h"
#include "queue.h"
#include "debug.h"
#include "CFG_Wifi_File.h"
#include "gl_kal.h"
    LINT_EXT_HEADER_BEGIN
#include "hif.h"
    LINT_EXT_HEADER_END
#include "wlan_lib.h"
#include "gl_req.h"
#include "wlan_oid.h"
extern BOOLEAN fgIsBusAccessFailed;

/*                                                                              
                                                
                                                                                
*/

/*                             */
#define GLUE_FLAG_WLAN_PROBE                    0x00000001	/*                                 */
#define GLUE_FLAG_MAP_REGISTER                  0x00000002	/*                                 */
#define GLUE_FLAG_INTERRUPT_IN_USE              0x00000004	/*                                 */
#define GLUE_FLAG_PAYLOAD_POOL                  0x00000008	/*                                 */
#define GLUE_FLAG_PKT_POOL                      0x00000010	/*                                 */
#define GLUE_FLAG_PKT_DESCR                     0x00000020	/*                                 */
#define GLUE_FLAG_BUF_POOL                      0x00000040	/*                                 */
#define GLUE_FLAG_SPIN_LOCK                     0x00000080	/*                                 */
#define GLUE_FLAG_HALT                          0x00000100	/*                                 */
#define GLUE_FLAG_RESET                         0x00000200	/*                                 */
#define GLUE_FLAG_TIMEOUT                       0x00000400	/*                                 */


/*                 */
/*                                                    */
#define ERRLOG_READ_PCI_SLOT_FAILED             0x00000101L
#define ERRLOG_WRITE_PCI_SLOT_FAILED            0x00000102L
#define ERRLOG_VENDOR_DEVICE_NOMATCH            0x00000103L

/*                                                   */
#define ERRLOG_BUS_MASTER_DISABLED              0x00000201L
#define ERRLOG_SET_LATENCY_TIMER_FAILED         0x00000202L

/*                                                            */
#define ERRLOG_INVALID_SPEED_DUPLEX             0x00000301L
#define ERRLOG_SET_SECONDARY_FAILED             0x00000302L

/*                                                   */
#define ERRLOG_OUT_OF_MEMORY                    0x00000401L
#define ERRLOG_OUT_OF_SHARED_MEMORY             0x00000402L
#define ERRLOG_OUT_OF_MAP_REGISTERS             0x00000403L
#define ERRLOG_OUT_OF_BUFFER_POOL               0x00000404L
#define ERRLOG_OUT_OF_NDIS_BUFFER               0x00000405L
#define ERRLOG_OUT_OF_PACKET_POOL               0x00000406L
#define ERRLOG_OUT_OF_NDIS_PACKET               0x00000407L
#define ERRLOG_OUT_OF_LOOKASIDE_MEMORY          0x00000408L

/*                                                   */
#define ERRLOG_UNSUPPORTED_CHIP_ID_REV          0x00000501L
#define ERRLOG_UNSUPPORTED_RF_CTRL_MODE         0x00000502L
#define ERRLOG_SELFTEST_FAILED                  0x00000503L
#define ERRLOG_INITIALIZE_ADAPTER               0x00000504L
#define ERRLOG_REMOVE_MINIPORT                  0x00000505L
#define ERRLOG_TFIFO_UNDERFLOW                  0x00000506L
#define ERRLOG_MMI_FATAL_ERROR                  0x00000507L
#define ERRLOG_BCI_FATAL_ERROR                  0x00000508L

/*                                                    */
#define ERRLOG_MAP_IO_SPACE                     0x00000601L
#define ERRLOG_QUERY_ADAPTER_RESOURCES          0x00000602L
#define ERRLOG_NO_IO_RESOURCE                   0x00000603L
#define ERRLOG_NO_INTERRUPT_RESOURCE            0x00000604L
#define ERRLOG_NO_MEMORY_RESOURCE               0x00000605L
#define ERRLOG_REG_IO_PORT_RANGE                0x00000606L


#define NUM_SUPPORTED_OIDS      (sizeof(arWlanOidReqTable) / sizeof(WLAN_REQ_ENTRY))
#define NUM_REG_PARAMS          (sizeof(arWlanRegTable) / sizeof(WLAN_REG_ENTRY_T))


#define ETHERNET_HEADER_SZ                      14
#define ETHERNET_MAX_PKT_SZ                     1514

#define MAX_ARRAY_SEND_PACKETS                  8

#define SOURCE_PORT_LEN                         2
/*                                       */
#define LOOK_AHEAD_LEN                          (ETHER_HEADER_LEN + IPV4_HDR_LEN + SOURCE_PORT_LEN)

#if 0				/*                           */
#define ETH_HLEN                                14
#define ETH_TYPE_LEN_LEN                        2
#define ETH_TYPE_LEN_OFFSET                     12
#define ETH_P_IP                                0x0800
#define ETH_P_1X                                0x888E
#define ETH_P_PRE_1X                            0x88C7

#if CFG_SUPPORT_WAPI
#define ETH_WPI_1X                              0x88B4
#endif

#define IPVERSION                               4
#define IP_HEADER_LEN                           20

#define IPVH_VERSION_OFFSET                     4	/*                   */
#define IPVH_VERSION_MASK                       0xF0
#define IPTOS_PREC_OFFSET                       5
#define IPTOS_PREC_MASK                         0xE0

#define USER_PRIORITY_DEFAULT                   0


/*                */
#define ETH_LLC_OFFSET                          (ETH_HLEN)
#define ETH_LLC_LEN                             3
#define ETH_LLC_DSAP_SNAP                       0xAA
#define ETH_LLC_SSAP_SNAP                       0xAA
#define ETH_LLC_CONTROL_UNNUMBERED_INFORMATION  0x03

/*                */
#define ETH_SNAP_OFFSET                         (ETH_HLEN + ETH_LLC_LEN)
#define ETH_SNAP_LEN                            5
#define ETH_SNAP_BT_SIG_OUI_0                   0x00
#define ETH_SNAP_BT_SIG_OUI_1                   0x19
#define ETH_SNAP_BT_SIG_OUI_2                   0x58

#define BOW_PROTOCOL_ID_SECURITY_FRAME          0x0003
#endif
/*                                                                              
                                                 
                                                                                
*/
LINT_EXT_HEADER_BEGIN
    typedef WLAN_STATUS(*PFN_OID_HANDLER_FUNC_REQ) (IN PVOID prAdapter,
						    IN OUT PVOID pvBuf,
						    IN UINT_32 u4BufLen,
						    OUT PUINT_32 pu4OutInfoLen);

typedef enum _ENUM_OID_METHOD_T {
	ENUM_OID_GLUE_ONLY,
	ENUM_OID_GLUE_EXTENSION,
	ENUM_OID_DRIVER_CORE
} ENUM_OID_METHOD_T, *P_ENUM_OID_METHOD_T;

typedef enum _ENUM_NET_PORT_IDX_T {
	NET_PORT_WLAN_IDX = 0,
	NET_PORT_P2P0_IDX,
	NET_PORT_P2P1_IDX,
	NET_PORT_P2P2_IDX,
	NET_PORT_BOW_IDX,
	NET_PORT_NUM
} ENUM_NET_PORT_IDX_T;

typedef enum _ENUM_NET_DEV_IDX_T {
	NET_DEV_WLAN_IDX = 0,
	NET_DEV_P2P_IDX,
	NET_DEV_BOW_IDX,
	NET_DEV_NUM
} ENUM_NET_DEV_IDX_T;

/*                                */
typedef struct _WLAN_REQ_ENTRY {
	NDIS_OID rOid;		/*     */
	PUINT_8 pucOidName;	/*               */
	BOOLEAN fgQryBufLenChecking;
	BOOLEAN fgSetBufLenChecking;
	ENUM_OID_METHOD_T eOidMethod;
	UINT_32 u4InfoBufLen;
	PFN_OID_HANDLER_FUNC_REQ pfOidQueryHandler;
	PFN_OID_HANDLER_FUNC_REQ pfOidSetHandler;
} WLAN_REQ_ENTRY, *P_WLAN_REQ_ENTRY;


#if CFG_TCP_IP_CHKSUM_OFFLOAD
/*                                          */
typedef struct _NIC_TASK_OFFLOAD {
	UINT_32 ChecksumOffload:1;
	UINT_32 LargeSendOffload:1;
	UINT_32 IpSecOffload:1;

} NIC_TASK_OFFLOAD;


/*                               */
typedef struct _NIC_CHECKSUM_OFFLOAD {
	UINT_32 DoXmitTcpChecksum:1;
	UINT_32 DoRcvTcpChecksum:1;
	UINT_32 DoXmitUdpChecksum:1;
	UINT_32 DoRcvUdpChecksum:1;
	UINT_32 DoXmitIpChecksum:1;
	UINT_32 DoRcvIpChecksum:1;

} NIC_CHECKSUM_OFFLOAD;
#endif				/*                           */

typedef enum _ENUM_RSSI_TRIGGER_TYPE {
	ENUM_RSSI_TRIGGER_NONE,
	ENUM_RSSI_TRIGGER_GREATER,
	ENUM_RSSI_TRIGGER_LESS,
	ENUM_RSSI_TRIGGER_TRIGGERED,
	ENUM_RSSI_TRIGGER_NUM
} ENUM_RSSI_TRIGGER_TYPE;

struct _GLUE_INFO_T {
	NDIS_HANDLE rMiniportAdapterHandle;	/*               */

	NDIS_WORK_ITEM rWorkItem;

	UINT_32 ucEmuWorkItemId;

	/*                                       */
	UINT_8 ucDriverDescLen;
	UINT_8 aucDriverDesc[80];	/*                               */

#if CFG_TCP_IP_CHKSUM_OFFLOAD
	/*                             */
	NIC_TASK_OFFLOAD rNicTaskOffload;
	NIC_CHECKSUM_OFFLOAD rNicChecksumOffload;
	NDIS_ENCAPSULATION_FORMAT rEncapsulationFormat;
#endif

	UINT_16 u2NdisVersion;
	INT_32 exitRefCount;
	UINT_32 u4Flags;

	/*                             */
	NDIS_HANDLE hPktPool;	/*                          */
	NDIS_HANDLE hBufPool;	/*                          */
	PUINT_8 pucPayloadPool;	/*                                             */
	UINT_32 u4PayloadPoolSz;	/*                                                 */
	PVOID pvPktDescrHead;	/*                                               */
	UINT_32 u4PktDescrFreeNum;	/*                                                           */
	UINT_32 u4PktPoolSz;	/*                                                       */

	/*                        */
	HANDLE hTxService;	/*                         */
	NDIS_EVENT rTxReqEvent;	/*                            */
#if defined(WINDOWS_DDK)
	PKTHREAD pvKThread;
#endif

	QUE_T rTxQueue;
	QUE_T rReturnQueue;
	QUE_T rCmdQueue;

	/*                                        */
	NDIS_SPIN_LOCK arSpinLock[SPIN_LOCK_NUM];


	/*                                                                 
                                                   
  */
	LONG i4TxPendingFrameNum;
	LONG i4TxPendingSecurityFrameNum;

	LONG i4RxPendingFrameNum;

	/*                               */
	UINT_8 aucNetInterfaceToBssIdx[NET_PORT_NUM];

	BOOLEAN fgIsCardRemoved;

	/*                                    */
	/*                                    */
	GL_HIF_INFO_T rHifInfo;

	NDIS_802_11_ASSOCIATION_INFORMATION rNdisAssocInfo;
	UINT_8 aucNdisAssocInfoIEs[600];

	/*                                                                       */
	P_ADAPTER_T prAdapter;

	/*                       */
	ENUM_PARAM_MEDIA_STATE_T eParamMediaStateIndicated;

	/*               */
	REG_INFO_T rRegInfo;

	/*                                       */
	NET_INTERFACE_INFO_T arNetInterfaceInfo[NET_DEV_NUM];
	P_NET_INTERFACE_INFO_T aprBssIdxToNetInterfaceInfo[HW_BSSID_NUM];

	/*             */
	BOOLEAN fgSetOid;
	PVOID pvInformationBuffer;
	UINT_32 u4InformationBufferLength;
	PUINT_32 pu4BytesReadOrWritten;
	PUINT_32 pu4BytesNeeded;
	PVOID pvOidEntry;
	BOOLEAN fgIsGlueExtension;

#if CFG_SUPPORT_WAPI
	/*                                                  */
	UINT_8 aucWapiAssocInfoIEs[42];
	UINT_16 u2WapiAssocInfoIESz;
#endif

	LONG i4OidPendingCount;

	/*               */
	PVOID pvTimerFunc;
	NDIS_MINIPORT_TIMER rMasterTimer;

	BOOLEAN fgWpsActive;
	UINT_8 aucWSCIE[500];	/*              */
	UINT_16 u2WSCIELen;
	UINT_8 aucWSCAssocInfoIE[200];	/*              */
	UINT_16 u2WSCAssocInfoIELen;

	SET_TXPWR_CTRL_T rTxPwr;

#if CFG_SUPPORT_SDIO_READ_WRITE_PATTERN
	BOOLEAN fgEnSdioTestPattern;
	BOOLEAN fgSdioReadWriteMode;
	BOOLEAN fgIsSdioTestInitialized;
	UINT_8 aucSdioTestBuffer[256];
#endif

};


#define MAX_MINIPORT_NAME_PATH                  256

/*                                */
typedef struct _MINIPORT_INSTANCE_INFO {
	WCHAR MiniportName[MAX_MINIPORT_NAME_PATH];
	WCHAR MiniportInstance[MAX_MINIPORT_NAME_PATH];
	WCHAR RegPath[MAX_MINIPORT_NAME_PATH];
	WCHAR ActiveKeyPath[MAX_MINIPORT_NAME_PATH];
	ULONG InstanceNumber;
} MINIPORT_INSTANCE_INFO, *PMINIPORT_INSTANCE_INFO;


/*                                                               */
typedef struct _PKT_INFO_RESERVED_T {
	union {
		/*                 */
		struct {
			PVOID pvPacket;	/*                                          */
			UINT_8 ucTid;
			UINT_8 ucTC;
			UINT_16 u2PktLen;
			PUINT_8 pucDA;
		};
		/*                 */
		struct {
			PNDIS_PACKET prNextPkt;	/*                       */
			PVOID pvBuf;	/*                */
		};
	};
} PKT_INFO_RESERVED, *P_PKT_INFO_RESERVED;

#define PKT_INFO_RESERVED_TID_MASK      BITS(0, 3)
#define PKT_INFO_RESERVED_FLAG_P2P      BIT(4)
#define PKT_INFO_RESERVED_FLAG_PAL      BIT(5)
#define PKT_INFO_RESERVED_FLAG_1X       BIT(6)
#define PKT_INFO_RESERVED_FLAG_802_11   BIT(7)

typedef struct _MEDIA_STREAMING_INDICATIONS_T {
	NDIS_802_11_STATUS_TYPE StatusType;
	NDIS_802_11_MEDIA_STREAM_MODE MediaStreamMode;
} MEDIA_STREAMING_INDICATIONS_T, *P_MEDIA_STREAMING_INDICATIONS_T;

#define PKT_INFO_RESERVED_FLAG_802_3    BIT(0)
#define PKT_INFO_RESERVED_FLAG_VLAN     BIT(1)


/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
/*                                                                            */
/*                                                                            */
/*                                                                            */
#define GLUE_SPIN_LOCK_DECLARATION()

#define GLUE_ACQUIRE_SPIN_LOCK(_prGlueInfo, _rLockCategory)  \
	{ \
	    if (_rLockCategory < SPIN_LOCK_NUM) { \
		NdisAcquireSpinLock(&((_prGlueInfo)->arSpinLock[_rLockCategory])); \
	    } \
	}

#define GLUE_RELEASE_SPIN_LOCK(_prGlueInfo, _rLockCategory)  \
	{ \
	    if (_rLockCategory < SPIN_LOCK_NUM) { \
		NdisReleaseSpinLock(&((_prGlueInfo)->arSpinLock[_rLockCategory])); \
	    } \
	}

/*                                                                            */
/*                                                                            */
/*                                                                            */
#define GLUE_WAIT_EVENT(_prGlueInfo)        NdisWaitEvent(&((_prGlueInfo)->rTxReqEvent), 0)

#define GLUE_RESET_EVENT(_prGlueInfo)       NdisResetEvent(&((_prGlueInfo)->rTxReqEvent))

#define GLUE_SET_EVENT(_prGlueInfo)         NdisSetEvent(&((_prGlueInfo)->rTxReqEvent))


/*                            */
#define NIC_MEM_TAG                             ((ULONG) 'MCPI')

/*            */
#define NIC_MEDIA_TYPE                          NdisMedium802_3

/*                                                                 */
#define GLUE_GET_REG_OFFSET(_f)        (OFFSET_OF(GLUE_INFO_T, rRegInfo) + OFFSET_OF(REG_INFO_T, _f))

/*                                           */
#define MP_GET_PKT_MR(_p)               (&(_p)->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, pvPacket)])
#define MP_GET_MR_PKT(_p)               (PNDIS_PACKET) CONTAINING_RECORD(_p, \
					    NDIS_PACKET, MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, pvPacket)])
#define MP_SET_PKT_TID(_p, _pri)        ((_p)->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, ucTid)] = (_pri))
#define MP_GET_PKT_TID(_p)              ((_p)->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, ucTid)])

#define MP_SET_PKT_TC(_p, _tc)          ((_p)->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, ucTC)] = (_tc))
#define MP_GET_PKT_TC(_p)               ((_p)->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, ucTC)])

#define MP_SET_PKT_PKTLEN(_p, _pktLen)  (*((PUINT_16)&((_p)->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, u2PktLen)])) = (_pktLen))
#define MP_GET_PKT_PKTLEN(_p)           (*((PUINT_16)&((_p)->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, u2PktLen)])))

#define MP_SET_PKT_DA_PTR(_p, _DA)      (*((PUINT_16)&((_p)->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, pucDA)])) = (_DA))
#define MP_GET_PKT_DA_PTR(_p)           (*((PUINT_16)&((_p)->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, pucDA)])))



/*                                                      */
#define GLUE_SET_FLAG(_M, _F)           ((_M)->u4Flags |= (_F))
#define GLUE_CLEAR_FLAG(_M, _F)         ((_M)->u4Flags &= ~(_F))
#define GLUE_TEST_FLAG(_M, _F)          ((_M)->u4Flags & (_F))
#define GLUE_TEST_FLAGS(_M, _F)         (((_M)->u4Flags & (_F)) == (_F))

#define GLUE_INC_REF_CNT(_refCount)     NdisInterlockedIncrement(&(_refCount))
#define GLUE_DEC_REF_CNT(_refCount)     NdisInterlockedDecrement(&(_refCount))
#define GLUE_GET_REF_CNT(_refCount)     (_refCount)

/*                                                                            */
/*                                                                            */
/*                                                                            */
#define GLUE_SPIN_LOCK_DECLARATION()

/*       */
/*                                                                            */
/*                                                                            */
/*                                                                            */
#define GLUE_CLEAR_PKT_RSVD(_p) \
	    { \
	     *((PUINT_32) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[0])) = 0; \
	     *((PUINT_32) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) = 0; \
	     *((PUINT_32) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[8])) = 0; \
	     *((PUINT_32) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[12])) = 0; \
	    }

#define GLUE_GET_PKT_QUEUE_ENTRY(_p)    \
	    (&(((PNDIS_PACKET)(_p))->MiniportReservedEx[0]))

#define GLUE_GET_PKT_DESCRIPTOR(_prQueueEntry)  \
	    (CONTAINING_RECORD(_prQueueEntry, NDIS_PACKET, MiniportReservedEx[0]))

#define GLUE_SET_PKT_TID(_p, _tid)  \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) |= \
	     ((UINT_8)((_tid) & PKT_INFO_RESERVED_TID_MASK)))

#define GLUE_SET_PKT_FLAG_802_11(_p)  \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) |= \
	     PKT_INFO_RESERVED_FLAG_802_11)

#define GLUE_SET_PKT_FLAG_1X(_p)  \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) |= \
	     PKT_INFO_RESERVED_FLAG_1X)

#define GLUE_SET_PKT_FLAG_PAL(_p)  \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) |= \
	     PKT_INFO_RESERVED_FLAG_PAL)

#define GLUE_SET_PKT_FLAG_P2P(_p)  \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) |= \
	     PKT_INFO_RESERVED_FLAG_P2P)


#define GLUE_GET_PKT_TID(_p)        \
	    ((*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) ) & \
	     PKT_INFO_RESERVED_TID_MASK)

#define GLUE_GET_PKT_IS_802_11(_p)      \
	    ((*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) ) & \
	     PKT_INFO_RESERVED_FLAG_802_11)

#define GLUE_GET_PKT_IS_1X(_p)          \
	    ((*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) ) & \
	     PKT_INFO_RESERVED_FLAG_1X)

#define GLUE_GET_PKT_IS_PAL(_p)         \
	    ((*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) ) & \
	     PKT_INFO_RESERVED_FLAG_PAL)

#define GLUE_GET_PKT_IS_P2P(_p)         \
	    ((*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[4])) ) & \
	     PKT_INFO_RESERVED_FLAG_P2P)


#define GLUE_SET_PKT_HEADER_LEN(_p, _ucMacHeaderLen)    \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[5])) = \
	     (_ucMacHeaderLen))

#define GLUE_GET_PKT_HEADER_LEN(_p) \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[5])) )


#define GLUE_SET_PKT_FRAME_LEN(_p, _u2FrameLen) \
            (*((PUINT_16) & (((PNDIS_PACKET)(_p))->MiniportReservedEx[6])) = \
	     (_u2FrameLen))

#define GLUE_GET_PKT_FRAME_LEN(_p)    \
            (*((PUINT_16) & (((PNDIS_PACKET)(_p))->MiniportReservedEx[6])) )


#define GLUE_SET_PKT_ARRIVAL_TIME(_p, _rSysTime) \
            (*((POS_SYSTIME) & (((PNDIS_PACKET)(_p))->MiniportReservedEx[8])) = \
	     (OS_SYSTIME)(_rSysTime))

#define GLUE_GET_PKT_ARRIVAL_TIME(_p)    \
            (*((POS_SYSTIME) & (((PNDIS_PACKET)(_p))->MiniportReservedEx[8])) )

#define GLUE_SET_PKT_BSS_IDX(_p, _ucBssIndex) \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[12])) = \
	     (UINT_8)(_ucBssIndex))

#define GLUE_GET_PKT_BSS_IDX(_p)    \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[12])) )

#define GLUE_SET_PKT_FLAG_802_3(_p) \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[13])) |= \
	     PKT_INFO_RESERVED_FLAG_802_3)

#define GLUE_GET_PKT_IS_802_3(_p) \
	    ((*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[13])) ) & \
	     PKT_INFO_RESERVED_FLAG_802_3)

#define GLUE_SET_PKT_FLAG_VLAN_EXIST(_p)  \
	    (*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[13])) |= \
	     PKT_INFO_RESERVED_FLAG_VLAN)

#define GLUE_GET_PKT_IS_VLAN_EXIST(_p)  \
	    ((*((PUINT_8) &(((PNDIS_PACKET)(_p))->MiniportReservedEx[13])) ) & \
	     PKT_INFO_RESERVED_FLAG_VLAN)


/*                                                                              
                                                            
                                                                                
*/
NDIS_STATUS
windowsFindAdapter(IN P_GLUE_INFO_T prGlueInfo, IN NDIS_HANDLE rWrapperConfigurationContext);

NDIS_STATUS windowsRegisterIsrt(IN P_GLUE_INFO_T prGlueInfo);

PNDIS_PACKET getPoolPacket(IN P_GLUE_INFO_T prGlueInfo);

VOID putPoolPacket(IN P_GLUE_INFO_T prGlueInfo, IN PNDIS_PACKET prPktDscr, IN PVOID pvPayloadBuf);

BOOLEAN
windowsGetPacketInfo(IN P_GLUE_INFO_T prGlueInfo,
		     IN PNDIS_PACKET prPacket,
		     OUT PUINT_32 pu4PktLen,
		     OUT PUINT_8 pucUserPriority, OUT PUINT_8 pucTC, OUT PUINT_8 aucEthDestAddr);

NDIS_STATUS windowsUnregisterIsrt(IN P_GLUE_INFO_T prGlueInfo);

NDIS_STATUS windowsUMapFreeRegister(IN P_GLUE_INFO_T prGlueInfo);

/*                                                                              
                                                
                                                                                
*/
LINT_EXT_HEADER_END
/*                                                                                          
 */
#ifdef _lint
VOID NdisAcquireSpinLock(IN PVOID SpinLock);

VOID NdisReleaseSpinLock(IN PVOID SpinLock);

VOID NdisSetEvent(IN PVOID Event);

BOOLEAN NdisWaitEvent(IN PVOID Event, IN UINT_32 MsToWait);

VOID NdisResetEvent(IN PVOID Event);
#endif				/*       */


#endif				/*          */
