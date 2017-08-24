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
#include "config.h"
#include "gl_os.h"

/*                                                                              
                                                
                                                                                
*/
#define MIN_ASSOC_REQ_BODY_LEN          9
#define MIN_REASSOC_REQ_BODY_LEN        15
#define MIN_REASSOC_RESP_BODY_LEN       6

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

/*                                                                              
                                                
                                                                                
*/



/*                                                                            */
/* 
                                     
 
                                                    
                                        
 
                                                        
*/
/*                                                                            */
PVOID kalMemAlloc(IN UINT_32 u4Size, IN ENUM_KAL_MEM_ALLOCATION_TYPE eMemType)
{
	PVOID pvAddr;

	if (NdisAllocateMemoryWithTag(&pvAddr, u4Size, NIC_MEM_TAG) == NDIS_STATUS_SUCCESS) {
		return pvAddr;
	}

	return NULL;
}				/*             */


/*                                                                            */
/* 
                                                                                
                                             
 
              
 
                                              
*/
/*                                                                            */
OS_SYSTIME kalGetTimeTick(VOID)
{
	ULONG u4SystemUpTime;

	NdisGetSystemUpTime(&u4SystemUpTime);

	return (OS_SYSTIME) u4SystemUpTime;
}				/*                */


/*                                                                            */
/* 
                                                                            
                                       
 
              
 
                                                          
*/
/*                                                                            */
UINT_64 kalGetNanoTick(VOID)
{
	LARGE_INTEGER rSystemTime;

	NdisGetCurrentSystemTime(&rSystemTime);

	return rSystemTime.QuadPart;
}				/*                */


/*                                                                            */
/* 
                                                                              
                                                                                
 
                                                      
                                                        
                                                                  
 
              
*/
/*                                                                            */
VOID kalCopyFrame(IN P_GLUE_INFO_T prGlueInfo, IN PVOID pvPacket, OUT PUINT_8 pucDestBuffer)
{
	PNDIS_PACKET prNdisPacket;
	PNDIS_BUFFER prNdisBuffer, prNextNdisBuffer;
	UINT u4PacketLen;
	UINT u4BytesToCopy;
	PVOID pvMbuf;
	UINT u4MbufLength;
	UINT u4BytesCopied;

	ASSERT(pvPacket);

	prNdisPacket = (PNDIS_PACKET) pvPacket;
	NdisQueryPacket(prNdisPacket, NULL, NULL, &prNdisBuffer, &u4PacketLen);

	u4BytesToCopy = u4PacketLen;
	u4BytesCopied = 0;

	while (u4BytesToCopy != 0) {

#ifdef NDIS51_MINIPORT
		NdisQueryBufferSafe(prNdisBuffer, &pvMbuf, &u4MbufLength, HighPagePriority);
#else
		NdisQueryBuffer(prNdisBuffer, &pvMbuf, &u4MbufLength);
#endif				/*                 */

		if (pvMbuf == (PVOID) NULL) {
			ASSERT(pvMbuf);
			break;
		}

		NdisMoveMemory((PVOID) pucDestBuffer, pvMbuf, u4MbufLength);

		u4BytesToCopy -= u4MbufLength;
		u4BytesCopied += u4MbufLength;
		pucDestBuffer += u4MbufLength;

		NdisGetNextBuffer(prNdisBuffer, &prNextNdisBuffer);
		prNdisBuffer = prNextNdisBuffer;
	}

	ASSERT(u4BytesCopied == u4PacketLen);

	return;
}

/*                                                                            */
/* 
                                                                                
 
                                                  
                                                              
                                                       
 
                                      
*/
/*                                                                            */
PVOID kalPacketAlloc(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Size, OUT PPUINT_8 ppucData)
{
	PNDIS_PACKET packet_p;

	packet_p = getPoolPacket(prGlueInfo);

	/*                                                          
                               
  */
	if (packet_p != NULL) {
		*ppucData =
		    *(PUCHAR *) &packet_p->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, pvBuf)];
		NDIS_SET_PACKET_HEADER_SIZE(packet_p, 14);	/*                   */
	}

	return packet_p;
}				/*                */

/*                                                                            */
/* 
                                                                              
 
                                                      
                                                        
 
              
*/
/*                                                                            */
VOID kalPacketFree(IN P_GLUE_INFO_T prGlueInfo, IN PVOID pvPacket)
{
	PNDIS_PACKET prNdisPacket;
	PNDIS_BUFFER prNdisBuf;


	ASSERT(prGlueInfo);
	ASSERT(pvPacket);

	prNdisPacket = (PNDIS_PACKET) pvPacket;

	do {
		NdisUnchainBufferAtBack(prNdisPacket, &prNdisBuf);

		if (prNdisBuf) {
			NdisFreeBuffer(prNdisBuf);
		} else {
			break;
		}

	} while (TRUE);

	/*                                               */
	NdisReinitializePacket(prNdisPacket);

#if CETK_NDIS_PERFORMANCE_WORKAROUND
	{
		PUINT_32 ptr;
		ptr = (PUINT_32) prNdisPacket->ProtocolReserved;
		*ptr = 0;
	}
#endif

	putPoolPacket(prGlueInfo, prNdisPacket, NULL);

}				/*               */


/*                                                                            */
/* 
                                                          
 
                                                      
                                                        
                                                                        
                                          
                                                     
                                                               
 
                              
                              
 
*/
/*                                                                            */
WLAN_STATUS
kalProcessRxPacket(IN P_GLUE_INFO_T prGlueInfo,
		   IN PVOID pvPacket,
		   IN PUINT_8 pucPacketStart,
		   IN UINT_32 u4PacketLen, IN BOOL fgIsRetain, IN ENUM_CSUM_RESULT_T aerCSUM[]
    )
{
	PNDIS_PACKET prNdisPacket;
	PNDIS_BUFFER prNdisBuf;
	NDIS_STATUS rStatus;


	ASSERT(prGlueInfo);
	ASSERT(pvPacket);
	ASSERT(pucPacketStart);

	prNdisPacket = (PNDIS_PACKET) pvPacket;

	NdisAllocateBuffer(&rStatus,
			   &prNdisBuf,
			   prGlueInfo->hBufPool, (PVOID) pucPacketStart, (UINT_32) u4PacketLen);

	if (rStatus != NDIS_STATUS_SUCCESS) {
		ASSERT(0);
		return WLAN_STATUS_FAILURE;
	}

	NdisChainBufferAtBack(prNdisPacket, prNdisBuf);

	if (fgIsRetain) {
		/*                                                        
                                                        
                                                           
                                                
                                          */
		NDIS_SET_PACKET_STATUS(prNdisPacket, NDIS_STATUS_RESOURCES);
	} else {

		/*                                                         
                                    */
		NDIS_SET_PACKET_STATUS(prNdisPacket, NDIS_STATUS_SUCCESS);
	}

#if CFG_TCP_IP_CHKSUM_OFFLOAD
	kalUpdateRxCSUMOffloadParam(pvPacket, aerCSUM);
#endif

	return WLAN_STATUS_SUCCESS;
}


/*                                                                            */
/* 
                                                                         
                             
 
                                                  
                                                    
                                                         
 
                                      
                                      
 
*/
/*                                                                            */
WLAN_STATUS kalRxIndicatePkts(IN P_GLUE_INFO_T prGlueInfo, IN PVOID apvPkts[], IN UINT_32 ucPktNum)
{
	NDIS_STATUS arStatus[CFG_RX_MAX_PKT_NUM];
	UINT_32 u4Idx;

	for (u4Idx = 0; u4Idx < ucPktNum; u4Idx++) {
		UINT_32 i, pivot;
		PVOID pvTmp;

		if (NDIS_GET_PACKET_STATUS((PNDIS_PACKET) apvPkts[u4Idx]) == NDIS_STATUS_RESOURCES) {
			pivot = u4Idx;
			for (i = u4Idx + 1; i < ucPktNum; i++) {
				if (NDIS_GET_PACKET_STATUS((PNDIS_PACKET) apvPkts[i]) !=
				    NDIS_STATUS_RESOURCES) {
					pvTmp = apvPkts[pivot];
					apvPkts[pivot] = apvPkts[i];
					apvPkts[i] = pvTmp;
					pivot++;
				}
			}
			break;
		}
	}

	for (u4Idx = 0; u4Idx < ucPktNum; u4Idx++) {
		arStatus[u4Idx] = NDIS_GET_PACKET_STATUS((PNDIS_PACKET) apvPkts[u4Idx]);
		if (arStatus[u4Idx] == NDIS_STATUS_SUCCESS) {
			/*                                                                           */
			InterlockedIncrement(&prGlueInfo->i4RxPendingFrameNum);
		}
	}

	NdisMIndicateReceivePacket(prGlueInfo->rMiniportAdapterHandle,
				   (PPNDIS_PACKET) apvPkts, (UINT) ucPktNum);

	for (u4Idx = 0; u4Idx < ucPktNum; u4Idx++) {

		/*                            */
		if (arStatus[u4Idx] != NDIS_STATUS_SUCCESS) {
			PNDIS_PACKET prNdisPacket = (PNDIS_PACKET) apvPkts[u4Idx];
			PNDIS_BUFFER prNdisBuf = (PNDIS_BUFFER) NULL;

			ASSERT(prNdisPacket);

			NdisUnchainBufferAtBack(prNdisPacket, &prNdisBuf);

			if (prNdisBuf) {
				NdisFreeBuffer(prNdisBuf);
			}
#if DBG
			else {
				ASSERT(0);
			}
#endif				/*     */

			/*                                               */
			NdisReinitializePacket(prNdisPacket);

#if CETK_NDIS_PERFORMANCE_WORKAROUND
			{
				PUINT_32 pu4Dummy;
				pu4Dummy = (PUINT_32) prNdisPacket->ProtocolReserved;
				*pu4Dummy = 0;
			}
#endif				/*                                  */

		}
	}

	return WLAN_STATUS_SUCCESS;
}				/*                    */


/*                                                                            */
/* 
                                                                               
 
                                                  
                                                                          
                                                             
                                                                             
                                                                                 
                                                                              
 
              
 
*/
/*                                                                            */
VOID
kalIndicateStatusAndComplete(IN P_GLUE_INFO_T prGlueInfo,
			     IN WLAN_STATUS eStatus, IN PVOID pvBuf, IN UINT_32 u4BufLen)
{
	ASSERT(prGlueInfo);

	/*                                                         */
	NdisMIndicateStatus(prGlueInfo->rMiniportAdapterHandle,
			    (NDIS_STATUS) eStatus, (PVOID) pvBuf, u4BufLen);

	/*                                                                       */
	NdisMIndicateStatusComplete(prGlueInfo->rMiniportAdapterHandle);

	if (eStatus == WLAN_STATUS_MEDIA_CONNECT || eStatus == WLAN_STATUS_MEDIA_DISCONNECT) {

		if (eStatus == WLAN_STATUS_MEDIA_CONNECT) {
			prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_CONNECTED;
		} else if (eStatus == WLAN_STATUS_MEDIA_DISCONNECT) {
			prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
		}

		if (wlanResetMediaStreamMode(prGlueInfo->prAdapter) == TRUE) {
			MEDIA_STREAMING_INDICATIONS_T rMediaStreamIndication;

			/*                                               */
			rMediaStreamIndication.StatusType = Ndis802_11StatusType_MediaStreamMode;
			rMediaStreamIndication.MediaStreamMode = Ndis802_11MediaStreamOff;

			NdisMIndicateStatus(prGlueInfo->rMiniportAdapterHandle,
					    (NDIS_STATUS) WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
					    (PVOID) &rMediaStreamIndication,
					    sizeof(MEDIA_STREAMING_INDICATIONS_T));
		}
	}
}				/*                              */


/*                                                                            */
/* 
                                                             
                                                               
                                                
 
                                                        
                                                                               
                                                    
                                                                  
                                                         
                                                                       
 
                
*/
/*                                                                            */
VOID
kalUpdateReAssocReqInfo(IN P_GLUE_INFO_T prGlueInfo,
			IN PUINT_8 pucFrameBody,
			IN UINT_32 u4FrameBodyLen, IN BOOLEAN fgReassocRequest)
{
	PUINT_8 cp;
	PNDIS_802_11_ASSOCIATION_INFORMATION prNdisAssocInfo;

	if (fgReassocRequest) {
		ASSERT(u4FrameBodyLen >= MIN_REASSOC_REQ_BODY_LEN);

		if (u4FrameBodyLen < MIN_REASSOC_REQ_BODY_LEN) {
			return;
		}
	} else {
		ASSERT(u4FrameBodyLen >= MIN_ASSOC_REQ_BODY_LEN);

		if (u4FrameBodyLen < MIN_ASSOC_REQ_BODY_LEN) {
			return;
		}
	}

	prNdisAssocInfo = &prGlueInfo->rNdisAssocInfo;

	cp = pucFrameBody;

	/*                                        */
	if (fgReassocRequest) {
		prNdisAssocInfo->AvailableRequestFixedIEs =
		    NDIS_802_11_AI_REQFI_CAPABILITIES |
		    NDIS_802_11_AI_REQFI_LISTENINTERVAL | NDIS_802_11_AI_REQFI_CURRENTAPADDRESS;
	} else {
		prNdisAssocInfo->AvailableRequestFixedIEs =
		    NDIS_802_11_AI_REQFI_CAPABILITIES | NDIS_802_11_AI_REQFI_LISTENINTERVAL;
	}

	kalMemCopy(&prNdisAssocInfo->RequestFixedIEs.Capabilities, cp, 2);
	cp += 2;
	u4FrameBodyLen -= 2;

	kalMemCopy(&prNdisAssocInfo->RequestFixedIEs.ListenInterval, cp, 2);
	cp += 2;
	u4FrameBodyLen -= 2;

	if (fgReassocRequest) {
		kalMemCopy(&prNdisAssocInfo->RequestFixedIEs.CurrentAPAddress, cp, 6);
		cp += 6;
		u4FrameBodyLen -= 6;
	} else {
		kalMemZero(&prNdisAssocInfo->RequestFixedIEs.CurrentAPAddress, 6);
	}

	/*                                                  */
	prNdisAssocInfo->RequestIELength = u4FrameBodyLen;
	prNdisAssocInfo->OffsetRequestIEs = sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);

	kalMemCopy(prGlueInfo->aucNdisAssocInfoIEs, cp, u4FrameBodyLen);

	/*                                                                      
                 */
	prNdisAssocInfo->AvailableResponseFixedIEs = 0;
	prNdisAssocInfo->ResponseFixedIEs.Capabilities = 0;
	prNdisAssocInfo->ResponseFixedIEs.StatusCode = 0;
	prNdisAssocInfo->ResponseFixedIEs.AssociationId = 0;
	prNdisAssocInfo->ResponseIELength = 0;
	prNdisAssocInfo->OffsetResponseIEs =
	    sizeof(NDIS_802_11_ASSOCIATION_INFORMATION) + u4FrameBodyLen;

}				/*                         */


/*                                                                            */
/* 
                                                             
                                                                
                                                
 
                                                      
                                                                              
                                                    
                                                                 
                                                         
 
                
*/
/*                                                                            */
VOID
kalUpdateReAssocRspInfo(IN P_GLUE_INFO_T prGlueInfo,
			IN PUINT_8 pucFrameBody, IN UINT_32 u4FrameBodyLen)
{
	PUINT_8 cp;
	PNDIS_802_11_ASSOCIATION_INFORMATION prNdisAssocInfo;
	UINT_32 u4AvailableAssocRespIEBufLen;

	ASSERT(u4FrameBodyLen >= MIN_REASSOC_RESP_BODY_LEN);

	if (u4FrameBodyLen < MIN_REASSOC_RESP_BODY_LEN) {
		return;
	}

	prNdisAssocInfo = &prGlueInfo->rNdisAssocInfo;

	cp = pucFrameBody;

	/*                                        */
	prNdisAssocInfo->AvailableResponseFixedIEs =
	    NDIS_802_11_AI_RESFI_CAPABILITIES |
	    NDIS_802_11_AI_RESFI_STATUSCODE | NDIS_802_11_AI_RESFI_ASSOCIATIONID;

	kalMemCopy(&prNdisAssocInfo->ResponseFixedIEs.Capabilities, cp, 2);
	cp += 2;

	kalMemCopy(&prNdisAssocInfo->ResponseFixedIEs.StatusCode, cp, 2);
	cp += 2;

	kalMemCopy(&prNdisAssocInfo->ResponseFixedIEs.AssociationId, cp, 2);
	cp += 2;

	u4FrameBodyLen -= 6;

	/*                                                  */
	u4AvailableAssocRespIEBufLen = (sizeof(prGlueInfo->aucNdisAssocInfoIEs) >
					prNdisAssocInfo->RequestIELength) ?
	    sizeof(prGlueInfo->aucNdisAssocInfoIEs) - prNdisAssocInfo->RequestIELength : 0;

	if (u4FrameBodyLen > u4AvailableAssocRespIEBufLen) {
		ASSERT(u4FrameBodyLen <= u4AvailableAssocRespIEBufLen);
		u4FrameBodyLen = u4AvailableAssocRespIEBufLen;
	}

	prNdisAssocInfo->ResponseIELength = u4FrameBodyLen;
	prNdisAssocInfo->OffsetResponseIEs =
	    sizeof(NDIS_802_11_ASSOCIATION_INFORMATION) + prNdisAssocInfo->RequestIELength;

	if (u4FrameBodyLen) {
#if BUILD_WMM
		UINT_32 len = 0;
		UINT_8 elemLen = 0, elemId = 0, wifiElemId = 221;
		UINT_8 wifiOuiWMM[] = { 0x00, 0x50, 0xF2, 0x02 };
#endif

		kalMemCopy(&prGlueInfo->aucNdisAssocInfoIEs[prNdisAssocInfo->RequestIELength],
			   cp, u4FrameBodyLen);

#if BUILD_WMM
		prGlueInfo->supportWMM = FALSE;
		while (len < u4FrameBodyLen) {
			elemId = *cp;
			elemLen = *(cp + 1);

			if (elemId == wifiElemId && (elemLen > sizeof(wifiOuiWMM))) {
				if (kalMemCmp(cp + 2, wifiOuiWMM, sizeof(wifiOuiWMM)) == 0) {
					prGlueInfo->supportWMM = TRUE;
#if DBG
					/*                       */
#endif
					break;
				}
			}

			len += elemLen + 2;
			cp += elemLen + 2;
		}
#endif
	}
}				/*                         */


#if CFG_TCP_IP_CHKSUM_OFFLOAD
/*                                                                            */
/* 
                                                                        
 
                                                    
                                                            
 
                
 
*/
/*                                                                            */
VOID kalQueryTxChksumOffloadParam(IN PVOID pvPacket, OUT PUINT_8 pucFlag)
{
	PNDIS_PACKET prPacket = (PNDIS_PACKET) pvPacket;
	NDIS_TCP_IP_CHECKSUM_PACKET_INFO rChecksumPktInfo;
	UINT_8 ucFlag = 0;

	if (NDIS_GET_PACKET_PROTOCOL_TYPE(prPacket) == NDIS_PROTOCOL_ID_TCP_IP) {

		rChecksumPktInfo.Value = (UINT_32)
		    NDIS_PER_PACKET_INFO_FROM_PACKET(prPacket, TcpIpChecksumPacketInfo);

		/*                                                            */
		if (rChecksumPktInfo.Transmit.NdisPacketChecksumV4 ||
		    rChecksumPktInfo.Transmit.NdisPacketChecksumV6) {

			/*                                              */
			if (rChecksumPktInfo.Transmit.NdisPacketIpChecksum) {
				ucFlag |= TX_CS_IP_GEN;
			}

			if (rChecksumPktInfo.Transmit.NdisPacketTcpChecksum ||
			    rChecksumPktInfo.Transmit.NdisPacketUdpChecksum) {
				ucFlag |= TX_CS_TCP_UDP_GEN;
			}
		}
	}
	*pucFlag = ucFlag;

}				/*                            */


/*                                                                            */
/* 
                                                                                   
 
                                                   
                                                          
 
                
 
*/
/*                                                                            */
VOID kalUpdateRxCSUMOffloadParam(IN PVOID pvPacket, IN ENUM_CSUM_RESULT_T aeCSUM[]
    )
{
	PNDIS_PACKET prPacket = (PNDIS_PACKET) pvPacket;
	NDIS_TCP_IP_CHECKSUM_PACKET_INFO rChecksumPktInfo;
	PNDIS_PACKET_EXTENSION prPktExt;

	prPktExt = NDIS_PACKET_EXTENSION_FROM_PACKET(prPacket);

	/*                                   */
	rChecksumPktInfo.Value = 0;

	if (aeCSUM[CSUM_TYPE_IPV4] != CSUM_RES_NONE) {
		if (aeCSUM[CSUM_TYPE_IPV4] != CSUM_RES_SUCCESS) {
			rChecksumPktInfo.Receive.NdisPacketIpChecksumFailed = TRUE;
		} else {
			rChecksumPktInfo.Receive.NdisPacketIpChecksumSucceeded = TRUE;
		}
	} else if (aeCSUM[CSUM_TYPE_IPV6] != CSUM_RES_NONE) {
		if (aeCSUM[CSUM_TYPE_IPV6] != CSUM_RES_SUCCESS) {
			rChecksumPktInfo.Receive.NdisPacketIpChecksumFailed = TRUE;
		} else {
			rChecksumPktInfo.Receive.NdisPacketIpChecksumSucceeded = TRUE;
		}
	}

	if (aeCSUM[CSUM_TYPE_TCP] != CSUM_RES_NONE) {
		if (aeCSUM[CSUM_TYPE_TCP] != CSUM_RES_SUCCESS) {
			rChecksumPktInfo.Receive.NdisPacketTcpChecksumFailed = TRUE;
		} else {
			rChecksumPktInfo.Receive.NdisPacketTcpChecksumSucceeded = TRUE;
		}
	} else if (aeCSUM[CSUM_TYPE_UDP] != CSUM_RES_NONE) {
		if (aeCSUM[CSUM_TYPE_UDP] != CSUM_RES_SUCCESS) {
			rChecksumPktInfo.Receive.NdisPacketUdpChecksumFailed = TRUE;
		} else {
			rChecksumPktInfo.Receive.NdisPacketUdpChecksumSucceeded = TRUE;
		}
	}

	prPktExt->NdisPacketInfo[TcpIpChecksumPacketInfo] = (PVOID) rChecksumPktInfo.Value;

}
#endif				/*                           */


/*                                                                            */
/* 
                                                                               
                           
 
                                                      
                                                
                                                      
 
              
*/
/*                                                                            */
VOID kalSendComplete(IN P_GLUE_INFO_T prGlueInfo, IN PVOID pvPacket, IN WLAN_STATUS rStatus)
{
	ASSERT(pvPacket);

	/*                                                      
                       
  */
	NdisMSendComplete(prGlueInfo->rMiniportAdapterHandle,
			  (PNDIS_PACKET) pvPacket, NDIS_STATUS_SUCCESS);
	/*                         */

	GLUE_DEC_REF_CNT(prGlueInfo->i4TxPendingFrameNum);

	return;
}


/*                                                                            */
/* 
                                                                               
                           
 
                                                      
                                                
                                                      
 
              
*/
/*                                                                            */
VOID
kalSecurityFrameSendComplete(IN P_GLUE_INFO_T prGlueInfo, IN PVOID pvPacket, IN WLAN_STATUS rStatus)
{
	ASSERT(pvPacket);

	/*                                                      
                       
  */
	NdisMSendComplete(prGlueInfo->rMiniportAdapterHandle,
			  (PNDIS_PACKET) pvPacket, NDIS_STATUS_SUCCESS);
	/*                         */

	GLUE_DEC_REF_CNT(prGlueInfo->i4TxPendingSecurityFrameNum);

	return;
}


/*                                                                            */
/* 
                                                                         
                           
 
                                                      
                                                             
 
              
*/
/*                                                                            */
VOID kalHandleAssocInfo(IN P_GLUE_INFO_T prGlueInfo, IN P_EVENT_ASSOC_INFO prAssocInfo)
{
	UINT_16 u2FrameBodyLen;

	ASSERT(prGlueInfo);

	u2FrameBodyLen = prAssocInfo->u2Length;

	if (prAssocInfo->ucAssocReq) {
		PUINT_8 cp;
		PNDIS_802_11_ASSOCIATION_INFORMATION prNdisAssocInfo;

#if 0
		if (prAssocInfo->ucReassoc) {
			ASSERT(u2FrameBodyLen >= MIN_REASSOC_REQ_BODY_LEN);

			if (u2FrameBodyLen < MIN_REASSOC_REQ_BODY_LEN) {
				return;
			}
		} else {
			ASSERT(u2FrameBodyLen >= MIN_ASSOC_REQ_BODY_LEN);

			if (u2FrameBodyLen < MIN_ASSOC_REQ_BODY_LEN) {
				return;
			}
		}
#endif
		prNdisAssocInfo = &prGlueInfo->rNdisAssocInfo;

		cp = (PUINT_8) &prAssocInfo->pucIe;

		/*                                        */
		if (prAssocInfo->ucReassoc) {
			prNdisAssocInfo->AvailableRequestFixedIEs =
			    NDIS_802_11_AI_REQFI_CAPABILITIES |
			    NDIS_802_11_AI_REQFI_LISTENINTERVAL |
			    NDIS_802_11_AI_REQFI_CURRENTAPADDRESS;
		} else {
			prNdisAssocInfo->AvailableRequestFixedIEs =
			    NDIS_802_11_AI_REQFI_CAPABILITIES | NDIS_802_11_AI_REQFI_LISTENINTERVAL;
		}
		kalMemCopy(&prNdisAssocInfo->RequestFixedIEs.Capabilities, cp, 2);
		cp += 2;
		u2FrameBodyLen -= 2;

		kalMemCopy(&prNdisAssocInfo->RequestFixedIEs.ListenInterval, cp, 2);
		cp += 2;
		u2FrameBodyLen -= 2;
		if (prAssocInfo->ucReassoc) {
			kalMemCopy(&prNdisAssocInfo->RequestFixedIEs.CurrentAPAddress, cp, 6);
			cp += 6;
			u2FrameBodyLen -= 6;
		} else {
			kalMemZero(&prNdisAssocInfo->RequestFixedIEs.CurrentAPAddress, 6);
		}

		/*                                                  */
		prNdisAssocInfo->RequestIELength = u2FrameBodyLen;
		prNdisAssocInfo->OffsetRequestIEs = sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);

		kalMemCopy(prGlueInfo->aucNdisAssocInfoIEs, cp, u2FrameBodyLen);

		/*                                                                      
                  */
		prNdisAssocInfo->AvailableResponseFixedIEs = 0;
		prNdisAssocInfo->ResponseFixedIEs.Capabilities = 0;
		prNdisAssocInfo->ResponseFixedIEs.StatusCode = 0;
		prNdisAssocInfo->ResponseFixedIEs.AssociationId = 0;
		prNdisAssocInfo->ResponseIELength = 0;
		prNdisAssocInfo->OffsetResponseIEs =
		    sizeof(NDIS_802_11_ASSOCIATION_INFORMATION) + u2FrameBodyLen;

	} else {

		PUINT_8 cp;
		PNDIS_802_11_ASSOCIATION_INFORMATION prNdisAssocInfo;
		UINT_16 u2AvailableAssocRespIEBufLen;

#if 0
		ASSERT(u2FrameBodyLen >= MIN_REASSOC_RESP_BODY_LEN);

		if (u2FrameBodyLen < MIN_REASSOC_RESP_BODY_LEN) {
			return;
		}
#endif
		prNdisAssocInfo = &prGlueInfo->rNdisAssocInfo;

		cp = (PUINT_8) &prAssocInfo->pucIe;

		/*                                        */
		prNdisAssocInfo->AvailableResponseFixedIEs =
		    NDIS_802_11_AI_RESFI_CAPABILITIES |
		    NDIS_802_11_AI_RESFI_STATUSCODE | NDIS_802_11_AI_RESFI_ASSOCIATIONID;

		kalMemCopy(&prNdisAssocInfo->ResponseFixedIEs.Capabilities, cp, 2);
		cp += 2;

		kalMemCopy(&prNdisAssocInfo->ResponseFixedIEs.StatusCode, cp, 2);
		cp += 2;

		kalMemCopy(&prNdisAssocInfo->ResponseFixedIEs.AssociationId, cp, 2);
		cp += 2;

		u2FrameBodyLen -= 6;

		/*                                                  */
		u2AvailableAssocRespIEBufLen = (sizeof(prGlueInfo->aucNdisAssocInfoIEs) >
						prNdisAssocInfo->RequestIELength) ?
		    (UINT_16) (sizeof(prGlueInfo->aucNdisAssocInfoIEs) -
			       prNdisAssocInfo->RequestIELength) : 0;

		if (u2FrameBodyLen > u2AvailableAssocRespIEBufLen) {
			ASSERT(u2FrameBodyLen <= u2AvailableAssocRespIEBufLen);
			u2FrameBodyLen = u2AvailableAssocRespIEBufLen;
		}

		prNdisAssocInfo->ResponseIELength = u2FrameBodyLen;
		prNdisAssocInfo->OffsetResponseIEs =
		    sizeof(NDIS_802_11_ASSOCIATION_INFORMATION) + prNdisAssocInfo->RequestIELength;

		if (u2FrameBodyLen) {
			kalMemCopy(&prGlueInfo->
				   aucNdisAssocInfoIEs[prNdisAssocInfo->RequestIELength], cp,
				   u2FrameBodyLen);
		}
	}
}
