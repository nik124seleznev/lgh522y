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

/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/
#if DBG
UINT_8 aucDebugModule[DBG_MODULE_NUM];
UINT_32 u4DebugModule = 0;
#endif				/*     */

/*                                                                              
                                                   
                                                                                
*/
#ifdef NDIS51_MINIPORT
static UINT_8 mpNdisMajorVersion = 5;
static UINT_8 mpNdisMinorVersion = 1;
#else
static UINT_8 mpNdisMajorVersion = 5;
static UINT_8 mpNdisMinorVersion;
#endif

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/
BOOLEAN mpCheckForHang(NDIS_HANDLE miniportAdapterContext);

VOID
mpIsr(OUT PBOOLEAN interruptRecognized_p,
      OUT PBOOLEAN queueMiniportHandleInterrupt_p, IN NDIS_HANDLE miniportAdapterContext);

VOID mpHandleInterrupt(IN NDIS_HANDLE miniportAdapterContext);

NDIS_STATUS
mpInitialize(OUT PNDIS_STATUS prOpenErrorStatus,
	     OUT PUINT prSelectedMediumIndex,
	     IN PNDIS_MEDIUM prMediumArray,
	     IN UINT u4MediumArraySize,
	     IN NDIS_HANDLE rMiniportAdapterHandle, IN NDIS_HANDLE rWrapperConfigurationContext);

VOID
mpSendPackets(IN NDIS_HANDLE miniportAdapterContext,
	      IN PPNDIS_PACKET packetArray_p, IN UINT numberOfPackets);

VOID mpReturnPacket(IN NDIS_HANDLE miniportAdapterContext, IN PNDIS_PACKET prPacket);

NDIS_STATUS mpReset(OUT PBOOLEAN addressingReset_p, IN NDIS_HANDLE miniportAdapterContext);

VOID mpHalt(IN NDIS_HANDLE miniportAdapterContext);

NDIS_STATUS
mpQueryInformation(IN NDIS_HANDLE miniportAdapterContext,
		   IN NDIS_OID oid,
		   IN PVOID pvInfomationBuffer,
		   IN UINT_32 u4InformationBufferLength,
		   OUT PUINT_32 pu4ByteWritten, OUT PUINT_32 pu4ByteNeeded);

NDIS_STATUS
mpSetInformation(IN NDIS_HANDLE miniportAdapterContext,
		 IN NDIS_OID oid,
		 IN PVOID pvInfomationBuffer,
		 IN UINT_32 u4InformationBufferLength,
		 OUT PUINT_32 pu4ByteRead, OUT PUINT_32 pu4ByteNeeded);

VOID mpShutdown(IN PVOID shutdownContext);

P_GLUE_INFO_T windowsCreateGlue(NDIS_HANDLE rMiniportAdapterHandle, UINT_16 u2NdisVersion);

VOID mpFreeAdapterObject(IN P_GLUE_INFO_T prGlueInfo);

NDIS_STATUS
windowsReadRegistryParameters(IN P_GLUE_INFO_T prGlueInfo,
			      IN NDIS_HANDLE wrapperConfigurationContext);

UINT_32 windowsInitRxPacketPool(P_GLUE_INFO_T prGlueInfo, UINT_32 u4NumPkt, UINT_32 u4MaxPktSz);

#if DBG
BOOLEAN reqCheckOrderOfSupportedOids(IN PVOID pvAdapter);
#endif

#ifdef NDIS51_MINIPORT
VOID
mpPnPEventNotify(IN NDIS_HANDLE miniportAdapterContext,
		 IN NDIS_DEVICE_PNP_EVENT pnpEvent,
		 IN PVOID informationBuffer_p, IN UINT_32 informationBufferLength);

VOID mpShutdown(IN PVOID shutdownContext);
#endif				/*     */


/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
                                                                               
                                                       
 
                                                                               
                                                             
                                                                            
                                                            
 
                                                                 
                                                                          
                        
*/
/*                                                                            */
NTSTATUS DriverEntry(IN PDRIVER_OBJECT driverObject, IN PUNICODE_STRING registryPath)
{
	NDIS_STATUS status;
	NDIS_HANDLE ndisWrapperHandle;
	NDIS_MINIPORT_CHARACTERISTICS mpChar;
#if DBG
	INT i;
#endif

	DEBUGFUNC("DriverEntry");

#if DBG
	/*                                       */
	for (i = 0; i < DBG_MODULE_NUM; i++) {
		aucDebugModule[i] = DBG_CLASS_ERROR |
		    DBG_CLASS_WARN | DBG_CLASS_STATE | DBG_CLASS_EVENT;
	}
	aucDebugModule[DBG_INIT_IDX] |= DBG_CLASS_TRACE | DBG_CLASS_INFO;
/*                                                                 */
	aucDebugModule[DBG_RX_IDX] |= DBG_CLASS_TRACE | DBG_CLASS_INFO;
	aucDebugModule[DBG_RFTEST_IDX] |= DBG_CLASS_TRACE | DBG_CLASS_INFO;

	aucDebugModule[DBG_REQ_IDX] &= ~DBG_CLASS_WARN;

	aucDebugModule[DBG_EMU_IDX] |= DBG_CLASS_TRACE | DBG_CLASS_INFO;
/*                                                */
	aucDebugModule[DBG_TX_IDX] &= ~DBG_CLASS_EVENT;
	aucDebugModule[DBG_RX_IDX] &= ~DBG_CLASS_EVENT;

#endif				/*     */

	DBGLOG(INIT, TRACE, ("\n"));
	DBGLOG(INIT, TRACE, ("DriverEntry: Driver object @0x%p\n", driverObject));

	/*                                                                    */
	NdisMInitializeWrapper(&ndisWrapperHandle, driverObject, registryPath, NULL);

	if (ndisWrapperHandle == NULL) {
		status = NDIS_STATUS_FAILURE;

		DBGLOG(INIT, ERROR, ("Init wrapper ==> FAILED (status=0x%x)\n", status));
		return status;
	}

	NdisZeroMemory(&mpChar, sizeof(NDIS_MINIPORT_CHARACTERISTICS));

	/*                                                        
                           */
	mpChar.MajorNdisVersion = mpNdisMajorVersion;
	mpChar.MinorNdisVersion = mpNdisMinorVersion;
	mpChar.CheckForHangHandler = NULL;	/*                 */
	mpChar.DisableInterruptHandler = NULL;
	mpChar.EnableInterruptHandler = NULL;
	mpChar.HaltHandler = mpHalt;
	mpChar.HandleInterruptHandler = mpHandleInterrupt;
	mpChar.InitializeHandler = mpInitialize;
	mpChar.ISRHandler = mpIsr;
	mpChar.QueryInformationHandler = mpQueryInformation;
	/*                                       */
	mpChar.ResetHandler = mpReset;
	mpChar.SetInformationHandler = mpSetInformation;
	mpChar.SendHandler = NULL;
	mpChar.SendPacketsHandler = mpSendPackets;
	mpChar.ReturnPacketHandler = mpReturnPacket;
	mpChar.TransferDataHandler = NULL;
	mpChar.AllocateCompleteHandler = NULL;
#ifdef NDIS51_MINIPORT
	mpChar.CancelSendPacketsHandler = NULL;
	/*                                                       */
	mpChar.PnPEventNotifyHandler = mpPnPEventNotify;
	mpChar.AdapterShutdownHandler = mpShutdown;
#endif

	/*                                                                    
                                                  */
	status = NdisMRegisterMiniport(ndisWrapperHandle,
				       &mpChar, sizeof(NDIS_MINIPORT_CHARACTERISTICS));

	DBGLOG(INIT, TRACE, ("NdisMRegisterMiniport (NDIS %d.%d) returns 0x%x\n",
			     mpNdisMajorVersion, mpNdisMinorVersion, status));

#ifndef _WIN64
#ifdef NDIS51_MINIPORT
	/*                                                                       
                                              */
	if (status == NDIS_STATUS_BAD_VERSION) {
		mpNdisMinorVersion = 0;
		mpChar.MinorNdisVersion = 0;
		/*                                                   */
		status = NdisMRegisterMiniport(ndisWrapperHandle, &mpChar,
					       sizeof(NDIS50_MINIPORT_CHARACTERISTICS));

		DBGLOG(INIT, TRACE, ("NdisMRegisterMiniport (NDIS %d.%d) returns 0x%x\n",
				     mpNdisMajorVersion, mpNdisMinorVersion, status));
	}
#endif
#endif

	if (status != NDIS_STATUS_SUCCESS) {
		DBGLOG(INIT, ERROR, ("Register NDIS %d.%d miniport ==> FAILED (status=0x%x)\n",
				     mpNdisMajorVersion, mpNdisMinorVersion, status));

		NdisTerminateWrapper(ndisWrapperHandle, NULL);

		return status;
	}

	return STATUS_SUCCESS;
}				/*             */

/*                                                                            */
/* 
                                                                             
                                                                              
                                                      
 
                                                                    
                                                                    
                                                                    
 
              
*/
/*                                                                            */
VOID
mpIsr(OUT PBOOLEAN interruptRecognized_p,
      OUT PBOOLEAN queueMiniportHandleInterrupt_p, IN NDIS_HANDLE miniportAdapterContext)
{
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T) miniportAdapterContext;

	if (wlanISR(prGlueInfo->prAdapter, TRUE)) {
		*interruptRecognized_p = TRUE;
		*queueMiniportHandleInterrupt_p = TRUE;
	} else {
		*interruptRecognized_p = FALSE;
		*queueMiniportHandleInterrupt_p = FALSE;
	}

}				/*       */

/*                                                                            */
/* 
                                                                           
                                                                            
                                             
 
                                                                  
 
              
*/
/*                                                                            */
VOID mpHandleInterrupt(IN NDIS_HANDLE miniportAdapterContext)
{
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T) miniportAdapterContext;

	wlanIST(prGlueInfo->prAdapter);

}				/*                   */


/*                                                                            */
/* 
                                                                             
                                                                           
                                                                             
                                                                          
             
 
                                                                    
                                                                    
                                                                    
                                                                    
                                                                    
                                                                    
 
                                       
                                                      
*/
/*                                                                            */
NDIS_STATUS
mpInitialize(OUT PNDIS_STATUS prOpenErrorStatus,
	     OUT PUINT prSelectedMediumIndex,
	     IN PNDIS_MEDIUM prMediumArray,
	     IN UINT u4MediumArraySize,
	     IN NDIS_HANDLE rMiniportAdapterHandle, IN NDIS_HANDLE rWrapperConfigurationContext)
{
	P_ADAPTER_T prAdapter = NULL;
	P_GLUE_INFO_T prGlueInfo = NULL;
	UINT i;
	NDIS_STATUS status;
#if DBG
	CHAR companyName[] = NIC_VENDOR;
	CHAR productName[] = NIC_PRODUCT_NAME;
	CHAR driverVersion[] = NIC_DRIVER_VERSION_STRING;
#endif
	UINT_8 desc[] = NIC_DRIVER_NAME;
	PVOID pvFwImageMapFile = NULL;
	NDIS_HANDLE rFileHandleFwImg = NULL;
	NDIS_STRING rFileWifiRam;
	UINT_32 u4FwImageFileLength = 0;
#if defined(WINDOWS_DDK)
	NTSTATUS rStatus = 0;
#endif


	DEBUGFUNC("MPInitialize");
	DBGLOG(INIT, TRACE, ("\n"));
	DBGPRINTF("MPInitialize() / Current IRQL = %d\n", KeGetCurrentIrql());

	DBGLOG(INIT, TRACE, ("%s\n", productName));
	DBGLOG(INIT, TRACE, ("(C) Copyright 2002-2007 %s\n", companyName));

	DBGLOG(INIT, TRACE, ("Version %s (NDIS 5.1/5.0 Checked Build)\n", driverVersion));

	DBGLOG(INIT, TRACE, ("***** BUILD TIME: %s %s *****\n", __DATE__, __TIME__));
	DBGLOG(INIT, TRACE, ("***** Current Platform: NDIS %d.%d *****\n",
			     mpNdisMajorVersion, mpNdisMinorVersion));

	do {
		/*                                 */
		for (i = 0; i < u4MediumArraySize; i++) {
			if (prMediumArray[i] == NIC_MEDIA_TYPE) {
				break;
			}
		}

		if (i == u4MediumArraySize) {
			DBGLOG(INIT, ERROR, ("Supported media type ==> Not found\n"));
			status = NDIS_STATUS_UNSUPPORTED_MEDIA;
			break;
		}

		/*                          */
		*prSelectedMediumIndex = i;

		/*                         */
		prGlueInfo = windowsCreateGlue(rMiniportAdapterHandle,
					       (UINT_16) ((UINT_16) mpNdisMajorVersion * 0x100 +
							  (UINT_16) mpNdisMinorVersion));
		if (prGlueInfo == NULL) {
			status = WLAN_STATUS_FAILURE;
			break;
		}


		prGlueInfo->ucDriverDescLen = (UINT_8) strlen(desc) + 1;
		if (prGlueInfo->ucDriverDescLen >= sizeof(prGlueInfo->aucDriverDesc)) {
			prGlueInfo->ucDriverDescLen = sizeof(prGlueInfo->aucDriverDesc);
		}
		strncpy(prGlueInfo->aucDriverDesc, desc, prGlueInfo->ucDriverDescLen);
		prGlueInfo->aucDriverDesc[prGlueInfo->ucDriverDescLen - 1] = '\0';


		prGlueInfo->eParamMediaStateIndicated = PARAM_MEDIA_STATE_DISCONNECTED;
		prGlueInfo->fgIsCardRemoved = FALSE;

#if CFG_SUPPORT_SDIO_READ_WRITE_PATTERN
		/*                                            */
		prGlueInfo->fgEnSdioTestPattern = FALSE;
		prGlueInfo->fgIsSdioTestInitialized = FALSE;
#endif

		/*                         */
		prAdapter = wlanAdapterCreate(prGlueInfo);
		if (prAdapter == NULL) {
			status = WLAN_STATUS_FAILURE;
			break;
		}

		DBGLOG(INIT, TRACE, ("Adapter structure pointer @0x%p\n", prAdapter));

		/*                                            */
		prGlueInfo->prAdapter = prAdapter;

		/*                               */
		kalMemZero(&(prGlueInfo->rRegInfo), sizeof(REG_INFO_T));
		status = windowsReadRegistryParameters(prGlueInfo, rWrapperConfigurationContext);
		DBGPRINTF("windowsReadRegistryParameters() = %08x\n", status);
		if (status != NDIS_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR,
			       ("Read registry parameters FAILED (status=0x%x)\n", status));
			break;
		}
		DBGLOG(INIT, TRACE, ("Read registry parameters -- OK\n"));

		/*                                              
                                                                    
                                                         
                          
                                     */
		NdisMSetAttributesEx(rMiniportAdapterHandle,
				     (NDIS_HANDLE) prGlueInfo,
				     0, (ULONG) NIC_ATTRIBUTE, NIC_INTERFACE_TYPE);
		DBGLOG(INIT, TRACE, ("Set attributes -- OK\n"));

		/*                                          */
		kalOsTimerInitialize(prGlueInfo, kalTimeoutHandler);

		/*                     */
		for (i = 0; i < SPIN_LOCK_NUM; i++) {
			NdisAllocateSpinLock(&(prGlueInfo->arSpinLock[i]));
		}
		GLUE_SET_FLAG(prGlueInfo, GLUE_FLAG_SPIN_LOCK);

		/*                                                   */
		status = windowsInitRxPacketPool(prGlueInfo,
						 CFG_RX_MAX_PKT_NUM, CFG_RX_MAX_PKT_SIZE);
		DBGPRINTF("windowsInitRxPacketPool() = %08x\n", status);

		if (status != NDIS_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR, ("Allocate Rx packet pool ==> FAILED (status=0x%x)\n",
					     status));
			break;
		}

		DBGLOG(INIT, TRACE, ("Init packet pool -- OK\n"));

		NdisInitializeEvent(&prGlueInfo->rTxReqEvent);

		/*                                                              */
		status = windowsFindAdapter(prGlueInfo, rWrapperConfigurationContext);
		DBGPRINTF("windowsFindAdapter() = %08x\n", status);

		if (status != NDIS_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR, ("Cannot find the adapter\n"));
			break;
		}
		DBGLOG(INIT, TRACE, ("Adapter Found -- OK\n"));


		/*                                                             
                                                                
            */
		if (prGlueInfo->u2NdisVersion == 0x500) {
			NdisMRegisterAdapterShutdownHandler(prGlueInfo->rMiniportAdapterHandle,
							    (PVOID) prAdapter,
							    (ADAPTER_SHUTDOWN_HANDLER) mpShutdown);
		}

		/*                                   */
		windowsRegisterIsrt(prGlueInfo);
		DBGLOG(INIT, TRACE, ("Register interrupt handler -- OK\n"));

#if CFG_ENABLE_FW_DOWNLOAD
		/*                                             */
#if defined(MT6620) && CFG_MULTI_ECOVER_SUPPORT
		if (wlanGetEcoVersion(prAdapter) >= 6) {
			NdisInitUnicodeString(&rFileWifiRam,
					      prGlueInfo->rRegInfo.aucFwImgFilenameE6);
		} else {
			NdisInitUnicodeString(&rFileWifiRam, prGlueInfo->rRegInfo.aucFwImgFilename);
		}
#else
		NdisInitUnicodeString(&rFileWifiRam, prGlueInfo->rRegInfo.aucFwImgFilename);
#endif
		if (!imageFileMapping
		    (rFileWifiRam, &rFileHandleFwImg, &pvFwImageMapFile, &u4FwImageFileLength)) {
			DBGLOG(INIT, ERROR, ("Fail to load FW image from file!\n"));
		}
#endif

		/*               */
		status = wlanAdapterStart((PVOID) prAdapter,
					  &prGlueInfo->rRegInfo,
					  pvFwImageMapFile, u4FwImageFileLength);
		DBGPRINTF("wlanAdapterStart() = %08x\n", status);

#if CFG_ENABLE_FW_DOWNLOAD
		/*                               */
		imageFileUnMapping(rFileHandleFwImg, pvFwImageMapFile);
#endif

		if (status != NDIS_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR, ("Start adapter ==> FAILED (status=0x%x)\n", status));
			break;
		}

		/*                                              */
		/*                                                                           */
		KeInitializeEvent(&prGlueInfo->rHifInfo.rOidReqEvent, SynchronizationEvent, FALSE);

		/*                                 */
#if defined(WINDOWS_DDK)
		KAL_CREATE_THREAD(prGlueInfo->hTxService, kalTxServiceThread, prGlueInfo,
				  &prGlueInfo->pvKThread);
#else
		KAL_CREATE_THREAD(prGlueInfo->hTxService, kalTxServiceThread, prGlueInfo);
#endif
		if (prGlueInfo->hTxService) {
			DBGLOG(INIT, TRACE, ("Thread has been created successfully\n"));

			KeWaitForSingleObject(&prGlueInfo->rHifInfo.rOidReqEvent, Executive, KernelMode, FALSE, NULL);	/*                    */
		} else {
			DBGLOG(INIT, WARN, ("CreateThread Failed\n"));
			break;
		}
		DBGLOG(INIT, TRACE, ("...done\n"));

		GLUE_SET_FLAG(prGlueInfo, GLUE_FLAG_WLAN_PROBE);
		DBGLOG(INIT, TRACE, ("Start adapter -- OK\n"));

#if DBG
		/*                                    */
		reqCheckOrderOfSupportedOids(prAdapter);
#endif


	} while (FALSE);

	DBGPRINTF("MPInitialize Completed: %08X (%d)\n", status, prGlueInfo->fgIsCardRemoved);

	if (prAdapter && status != NDIS_STATUS_SUCCESS) {
		/*                               */
		GLUE_DEC_REF_CNT(prGlueInfo->exitRefCount);
		mpFreeAdapterObject(prGlueInfo);
		return status;
	}

	return status;
}				/*              */


/*                                                                            */
/* 
                                                                               
                                         
 
                                                           
                                                           
 
              
*/
/*                                                                            */
VOID mpReturnPacket(IN NDIS_HANDLE miniportAdapterContext, IN PNDIS_PACKET prPacket)
{
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T) miniportAdapterContext;
	KIRQL currentIrql;
	P_QUE_ENTRY_T prQueEentry;

	DEBUGFUNC("mpReturnPacket");

	GLUE_SPIN_LOCK_DECLARATION();


	ASSERT(miniportAdapterContext);
	ASSERT(prPacket);

	/*                            */

	currentIrql = KeGetCurrentIrql();

	prQueEentry = (P_QUE_ENTRY_T) MP_GET_PKT_MR(prPacket);

	GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_RX_RETURN_QUE);
	QUEUE_INSERT_TAIL(&prGlueInfo->rReturnQueue, prQueEentry);
	GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_RX_RETURN_QUE);

	/*                                 */
	ASSERT(prGlueInfo->i4RxPendingFrameNum > 0);
	InterlockedDecrement(&prGlueInfo->i4RxPendingFrameNum);

	GLUE_SET_EVENT(prGlueInfo);

	ASSERT(currentIrql == KeGetCurrentIrql());

	return;
}				/*                         */


/*                                                                            */
/* 
                                                                        
                                                    
 
                                                           
                                                           
                                                           
 
              
*/
/*                                                                            */
VOID
mpSendPackets(IN NDIS_HANDLE miniportAdapterContext,
	      IN PPNDIS_PACKET packetArray_p, IN UINT numberOfPackets)
{
	P_GLUE_INFO_T prGlueInfo;
	PNDIS_PACKET prNdisPacket;
	P_QUE_ENTRY_T prQueueEntry;
	P_QUE_T prTxQue;
	UINT i;
	UINT_8 ucBssIndex;

	GLUE_SPIN_LOCK_DECLARATION();

	DEBUGFUNC("mpSendPackets");

	ASSERT(miniportAdapterContext);
	prGlueInfo = (P_GLUE_INFO_T) miniportAdapterContext;
	prTxQue = &prGlueInfo->rTxQueue;

	ucBssIndex = wlanGetAisBssIndex(prGlueInfo->prAdapter);

	for (i = 0; i < numberOfPackets; i++) {
		prNdisPacket = packetArray_p[i];

		if (prGlueInfo->fgIsCardRemoved) {
			NdisMSendComplete(prGlueInfo->rMiniportAdapterHandle,
					  prNdisPacket, NDIS_STATUS_NOT_ACCEPTED);
			continue;
		}
		if (wlanQueryTestMode(prGlueInfo->prAdapter) == TRUE) {
			NdisMSendComplete(prGlueInfo->rMiniportAdapterHandle,
					  prNdisPacket, NDIS_STATUS_NETWORK_UNREACHABLE);
			continue;
		}

		NDIS_SET_PACKET_STATUS(prNdisPacket, NDIS_STATUS_PENDING);

		GLUE_CLEAR_PKT_RSVD(prNdisPacket);

		GLUE_SET_PKT_BSS_IDX(prNdisPacket, ucBssIndex);

		if (wlanProcessSecurityFrame(prGlueInfo->prAdapter, (P_NATIVE_PACKET) prNdisPacket)
		    == FALSE) {
			prQueueEntry = (P_QUE_ENTRY_T) GLUE_GET_PKT_QUEUE_ENTRY(prNdisPacket);

			GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);
			QUEUE_INSERT_TAIL(prTxQue, prQueueEntry);
			GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_TX_QUE);

			GLUE_INC_REF_CNT(prGlueInfo->i4TxPendingFrameNum);
		} else {
			GLUE_INC_REF_CNT(prGlueInfo->i4TxPendingSecurityFrameNum);
		}
	}

	/*           */
	NdisSetEvent(&prGlueInfo->rTxReqEvent);

	return;

}				/*               */


/*                                                                            */
/* 
                                                                         
                                               
 
                                                           
 
              
*/
/*                                                                            */
VOID mpHalt(IN NDIS_HANDLE miniportAdapterContext)
{
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T) miniportAdapterContext;

	DEBUGFUNC("MPHalt");

	INITLOG(("\n"));
	DBGPRINTF("MPHalt (Card Removed: %d)\n", prGlueInfo->fgIsCardRemoved);

	/*                                  */
	GLUE_SET_FLAG(prGlueInfo, GLUE_FLAG_HALT);

	kalCancelTimer(prGlueInfo);

#if defined(WINDOWS_DDK)
	KAL_KILL_THREAD(&prGlueInfo->rTxReqEvent, prGlueInfo->pvKThread);
	prGlueInfo->hTxService = NULL;
#else
	/*                                         */
	NdisSetEvent(&prGlueInfo->rTxReqEvent);

	DBGLOG(INIT, TRACE, ("Notify TxServiceThread to terminate it\n"));

	if (prGlueInfo->hTxService) {
		DBGLOG(INIT, TRACE,
		       ("KAL_WAIT_FOR_SINGLE_OBJECT (0x%x)\n", prGlueInfo->hTxService));
		KAL_WAIT_FOR_SINGLE_OBJECT(prGlueInfo->hTxService);

		KAL_CLOSE_HANDLE(prGlueInfo->hTxService);
		prGlueInfo->hTxService = NULL;
	}
#endif

#if defined(WINDOWS_CE)
	NdisFreeEvent(&prGlueInfo->rTxReqEvent);
#endif

	while (prGlueInfo->i4RxPendingFrameNum > 0) {
		/*                                                     */
		/*                                                                               */
		NdisMSleep(100000);	/*                                   */
	}

	wlanAdapterStop(prGlueInfo->prAdapter);
	DBGLOG(INIT, TRACE, ("wlanAdapterStop\n"));


	/*                                                                        
         */
#ifdef NDIS51_MINIPORT
	if (prGlueInfo->u2NdisVersion == 0x500)
#endif
	{
		NdisMDeregisterAdapterShutdownHandler(prGlueInfo->rMiniportAdapterHandle);

		INITLOG(("Shutdown handler deregistered\n"));
	}
#if defined(WINDOWS_DDK) && defined(_HIF_SDIO)
	/*                                                                        
                                                                         
                                                                         
                                             
  */
	if (prGlueInfo->rHifInfo.dx.BusInterface.InterfaceDereference &&
	    prGlueInfo->rHifInfo.dx.busInited) {
		(prGlueInfo->rHifInfo.dx.BusInterface.InterfaceDereference)
		    (prGlueInfo->rHifInfo.dx.BusInterface.Context);
		RtlZeroMemory(&prGlueInfo->rHifInfo.dx.BusInterface,
			      sizeof(SDBUS_INTERFACE_STANDARD));
		prGlueInfo->rHifInfo.dx.busInited = FALSE;
	}
#endif

	/*                                                            
                */
	mpFreeAdapterObject((PVOID) prGlueInfo);

	INITLOG(("Halt handler -- Completed\n"));

}				/*        */

/*                                                                            */
/* 
                                                                     
                                                   
                                 
 
                                                               
                                                               
 
                             
*/
/*                                                                            */
NDIS_STATUS mpReset(OUT PBOOLEAN addressingReset_p, IN NDIS_HANDLE miniportAdapterContext)
{
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T) miniportAdapterContext;


	ASSERT(prGlueInfo);

	DBGLOG(INIT, ERROR, ("mpReset()\n"));

	if ((prGlueInfo->i4OidPendingCount)
	    || (prGlueInfo->i4TxPendingFrameNum)
	    || (prGlueInfo->i4TxPendingSecurityFrameNum)) {

		/*                                  */
		GLUE_SET_FLAG(prGlueInfo, GLUE_FLAG_RESET);

		/*                                         */
		GLUE_SET_EVENT(prGlueInfo);

		return NDIS_STATUS_PENDING;
	} else {
		return NDIS_STATUS_SUCCESS;
	}

}				/*                  */

/*                                                                            */
/* 
                                                                               
                                   
 
                                                         
 
              
*/
/*                                                                            */
VOID windowsDestroyPacketPool(P_GLUE_INFO_T prGlueInfo)
{
	UINT_32 i;
	PNDIS_PACKET prPktDscr;	/*                   */

	DEBUGFUNC("windowsDestroyPacketPool");

	/*                         */
	if (GLUE_TEST_FLAG(prGlueInfo, GLUE_FLAG_PKT_DESCR)) {

		for (i = 0; i < prGlueInfo->u4PktPoolSz; i++) {
			prPktDscr = getPoolPacket(prGlueInfo);

			if (prPktDscr == NULL) {
				ASSERT(prPktDscr);
				break;
			}
			NdisFreePacket(prPktDscr);
		}
		GLUE_CLEAR_FLAG(prGlueInfo, GLUE_FLAG_PKT_DESCR);
	}
#if DBG
	do {
		UINT_32 count;

		/*                                 */
		count = NdisPacketPoolUsage(prGlueInfo->hPktPool);
		ASSERT(count == 0);
	} while (0);
#endif

	/*                             */
	if (GLUE_TEST_FLAG(prGlueInfo, GLUE_FLAG_BUF_POOL)) {
		NdisFreeBufferPool(prGlueInfo->hBufPool);
		GLUE_CLEAR_FLAG(prGlueInfo, GLUE_FLAG_BUF_POOL);
	}

	/*                             */
	if (GLUE_TEST_FLAG(prGlueInfo, GLUE_FLAG_PKT_POOL)) {
		NdisFreePacketPool(prGlueInfo->hPktPool);
		GLUE_CLEAR_FLAG(prGlueInfo, GLUE_FLAG_PKT_POOL);
	}

	/*                          */
	if (GLUE_TEST_FLAG(prGlueInfo, GLUE_FLAG_PAYLOAD_POOL)) {
		kalMemFree(prGlueInfo->pucPayloadPool, VIR_MEM_TYPE, prGlueInfo->u4PayloadPoolSz);
		GLUE_CLEAR_FLAG(prGlueInfo, GLUE_FLAG_PAYLOAD_POOL);
	}

	return;
}				/*                          */

/*                                                                            */
/* 
                                                                   
                                      
 
                                                                   
 
              
*/
/*                                                                            */
VOID windowsDestroyGlue(IN P_GLUE_INFO_T prGlueInfo)
{
	if (prGlueInfo) {
		NdisFreeMemory(prGlueInfo, sizeof(GLUE_INFO_T), 0);
	}

	return;
}				/*                    */



/*                                                                            */
/* 
                                                           
                                                                
                       
 
                                                       
 
              
*/
/*                                                                            */
VOID mpFreeAdapterObject(IN P_GLUE_INFO_T prGlueInfo)
{
	UINT i;

	DEBUGFUNC("mpFreeAdapterObject");

	INITLOG(("\n"));

	if (prGlueInfo == NULL) {
		return;
	}

	if (GLUE_TEST_FLAG(prGlueInfo, GLUE_FLAG_WLAN_PROBE)) {
		GLUE_CLEAR_FLAG(prGlueInfo, GLUE_FLAG_WLAN_PROBE);
		INITLOG(("Hardware stopped\n"));
	}

	/*                              */
	windowsUnregisterIsrt(prGlueInfo);

#if CFG_SDIO_PATHRU_MODE
	/*                                       */
	sdioEnablePathruMode(prGlueInfo, FALSE);
	/*                                         */
	sdioDeinitPathruMode(prGlueInfo);
#endif

#if defined(_HIF_SDIO) && defined(WINDOWS_CE)
	sdioBusDeinit(prGlueInfo);
#endif

	windowsUMapFreeRegister(prGlueInfo);

#if defined(WINDOWS_DDK) && defined(_HIF_SDIO)
	/*                                                                        
                                                                         
                                                                         
                                             
  */
	if (prGlueInfo->rHifInfo.dx.BusInterface.InterfaceDereference &&
	    prGlueInfo->rHifInfo.dx.busInited) {
		(prGlueInfo->rHifInfo.dx.BusInterface.InterfaceDereference)
		    (prGlueInfo->rHifInfo.dx.BusInterface.Context);
		RtlZeroMemory(&prGlueInfo->rHifInfo.dx.BusInterface,
			      sizeof(SDBUS_INTERFACE_STANDARD));
		prGlueInfo->rHifInfo.dx.busInited = FALSE;
	}
#endif

	/*                                                                    
                              
  */
	if (prGlueInfo->pvPktDescrHead) {
		windowsDestroyPacketPool(prGlueInfo);
	}

	if (GLUE_TEST_FLAG(prGlueInfo, GLUE_FLAG_SPIN_LOCK)) {
		for (i = 0; i < SPIN_LOCK_NUM; i++) {
			NdisFreeSpinLock(&prGlueInfo->arSpinLock[i]);
		}
	}
	INITLOG(("Spin lock freed\n"));

	wlanAdapterDestroy(prGlueInfo->prAdapter);
	prGlueInfo->prAdapter = NULL;

	windowsDestroyGlue(prGlueInfo);

}				/*                     */



/*                                                                            */
/* 
                                                                       
                   
 
                                                                   
                                                                                
                                                           
 
                                                            
*/
/*                                                                            */
P_GLUE_INFO_T windowsCreateGlue(IN NDIS_HANDLE rMiniportAdapterHandle, IN UINT_16 u2NdisVersion)
{
	NDIS_STATUS status;
	P_GLUE_INFO_T prGlueInfo;

	DEBUGFUNC("windowsCreateGlue");

	/*                                                          */
	status = NdisAllocateMemoryWithTag((PVOID *) &prGlueInfo,
					   sizeof(GLUE_INFO_T), NIC_MEM_TAG);
	if (status != NDIS_STATUS_SUCCESS) {
		return NULL;
	}

	NdisZeroMemory(prGlueInfo, sizeof(GLUE_INFO_T));

	/*                                     */
	prGlueInfo->rMiniportAdapterHandle = rMiniportAdapterHandle;

	GLUE_INC_REF_CNT(prGlueInfo->exitRefCount);

	prGlueInfo->u2NdisVersion = u2NdisVersion;

	/*                                               
                                        */
	prGlueInfo->rNdisAssocInfo.Length = sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);
	prGlueInfo->rNdisAssocInfo.OffsetRequestIEs = sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);
	prGlueInfo->rNdisAssocInfo.OffsetResponseIEs = sizeof(NDIS_802_11_ASSOCIATION_INFORMATION);

	return prGlueInfo;
}				/*                   */



/*                                                                            */
/* 
                                                               
                                                                                 
                                        
 
                                                          
                                               
                                                                 
 
                           
                        
*/
/*                                                                            */
UINT_32 windowsInitRxPacketPool(P_GLUE_INFO_T prGlueInfo, UINT_32 u4NumPkt, UINT_32 u4MaxPktSz)
{
	UINT_32 u4TotalPayloadBufSz;	/*                           */
	PVOID prBufVirAddr;	/*                 */
	PUINT_8 prPktBufStartAddr;
	PNDIS_PACKET prPktDescriptor;
	NDIS_STATUS status;
	UINT_32 i;

	DEBUGFUNC("windowsInitRxPacketPool");

	/*                                                                        
                                                                     
                           */
#if DBG
	if (IS_NOT_ALIGN_4(u4MaxPktSz)) {
		/*                                                      */
		ASSERT(FALSE);
	}
#endif
	u4TotalPayloadBufSz = u4NumPkt * u4MaxPktSz;

	prBufVirAddr = kalMemAlloc(u4TotalPayloadBufSz, VIR_MEM_TYPE);

	if (prBufVirAddr == NULL) {
		DBGLOG(INIT, ERROR, ("Could not allocate the Rx payload buffer: %d (bytes)\n",
				     u4TotalPayloadBufSz));
		return ERRLOG_OUT_OF_SHARED_MEMORY;
	}

	prGlueInfo->pucPayloadPool = prBufVirAddr;
	prGlueInfo->u4PayloadPoolSz = u4TotalPayloadBufSz;

	GLUE_SET_FLAG(prGlueInfo, GLUE_FLAG_PAYLOAD_POOL);

	/*                                 
                                                                 
                                             
                                                            
                                                                  
            */
	NdisAllocatePacketPoolEx(&status,
				 &prGlueInfo->hPktPool,
				 (UINT) u4NumPkt, (UINT) u4NumPkt, 4 * sizeof(PVOID));

	if (status != NDIS_STATUS_SUCCESS) {
		DBGLOG(INIT, ERROR,
		       ("Could not allocate a Rx packet pool of %d descriptors (max. %d)\n",
			u4NumPkt, u4NumPkt));
		return ERRLOG_OUT_OF_PACKET_POOL;
	}

	GLUE_SET_FLAG(prGlueInfo, GLUE_FLAG_PKT_POOL);

	DBGLOG(INIT, TRACE, ("Allocate Rx packet pool of %d descriptors\n", u4NumPkt));

	/*                                                                       
                */
	prPktBufStartAddr = (PUINT_8) prBufVirAddr;
	for (i = 0; i < u4NumPkt; i++) {
		NdisAllocatePacket(&status, &prPktDescriptor, prGlueInfo->hPktPool);

		if (status != NDIS_STATUS_SUCCESS) {
			break;
		}

		putPoolPacket(prGlueInfo, prPktDescriptor, prPktBufStartAddr);

		prPktBufStartAddr += u4MaxPktSz;
	}

	if (i == 0) {
		DBGLOG(INIT, ERROR, ("Fail to allocate any packet descriptor\n"));
		return ERRLOG_OUT_OF_PACKET_POOL;
	}

	if (i != u4NumPkt) {
		DBGLOG(INIT, WARN, ("Only %d packet descriptors are allocated. (%d should be "
				    "allocated)\n", i, u4NumPkt));
	}

	prGlueInfo->u4PktPoolSz = i;

	GLUE_SET_FLAG(prGlueInfo, GLUE_FLAG_PKT_DESCR);

	DBGLOG(INIT, TRACE, ("Allocate %d descriptors\n", prGlueInfo->u4PktPoolSz));

	/*                                 
                                                          
                                     */
	NdisAllocateBufferPool(&status, &prGlueInfo->hBufPool, (UINT) u4NumPkt);

	if (status != NDIS_STATUS_SUCCESS) {
		DBGLOG(INIT, ERROR,
		       ("Could not allocate buffer descriptor pool of %d descriptors\n", u4NumPkt));
		return ERRLOG_OUT_OF_BUFFER_POOL;
	}

	GLUE_SET_FLAG(prGlueInfo, GLUE_FLAG_BUF_POOL);

	DBGLOG(INIT, TRACE, ("Allocate buffer descriptor pool of %d descriptors\n", u4NumPkt));

	return 0;
}				/*                         */


/*                                                                            */
/* 
                                                                
 
                                                     
                                                       
                                                                            
                                                      
 
              
 
                                                                             
                                
 
                                                                                
                           
*/
/*                                                                            */
VOID putPoolPacket(IN P_GLUE_INFO_T prGlueInfo, IN PNDIS_PACKET prPktDscr, IN PVOID pvPayloadBuf)
{
	PNDIS_PACKET *pprPkt;	/*                                   */
	PVOID *ppvBuf;		/*                                               */

	ASSERT(prPktDscr);

	pprPkt =
	    (PNDIS_PACKET *) &prPktDscr->
	    MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, prNextPkt)];
	*pprPkt = prGlueInfo->pvPktDescrHead;

	if (pvPayloadBuf) {
		ppvBuf =
		    (PVOID *) &prPktDscr->MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, pvBuf)];
		*ppvBuf = pvPayloadBuf;

		ASSERT((UINT_32) pvPayloadBuf >= (UINT_32) prGlueInfo->pucPayloadPool);
		ASSERT((UINT_32) pvPayloadBuf <
		       (UINT_32) (prGlueInfo->pucPayloadPool + prGlueInfo->u4PayloadPoolSz));
	}
#if DBG
	else {
		UINT_32 buffer_addr =
		    *(PUINT_32) & prPktDscr->
		    MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, pvBuf)];
		ASSERT(buffer_addr >= (UINT_32) prGlueInfo->pucPayloadPool);
		ASSERT(buffer_addr <
		       (UINT_32) (prGlueInfo->pucPayloadPool + prGlueInfo->u4PayloadPoolSz));
	}
#endif
	prGlueInfo->pvPktDescrHead = prPktDscr;
	prGlueInfo->u4PktDescrFreeNum++;

}				/*               */

/*                                                                            */
/* 
                                                                            
 
                                                       
 
                                                       
                                                               
*/
/*                                                                            */
PNDIS_PACKET getPoolPacket(IN P_GLUE_INFO_T prGlueInfo)
{
	PNDIS_PACKET prPktDscr;
	PNDIS_PACKET *pprPkt;


	prPktDscr = (PNDIS_PACKET) prGlueInfo->pvPktDescrHead;
	if (prPktDscr) {
		/*                      */
		pprPkt =
		    (PNDIS_PACKET *) &prPktDscr->
		    MiniportReservedEx[OFFSET_OF(PKT_INFO_RESERVED, prNextPkt)];

		ASSERT(prGlueInfo->u4PktDescrFreeNum >= 1);
		prGlueInfo->pvPktDescrHead = *pprPkt;
		prGlueInfo->u4PktDescrFreeNum--;
	}

	return prPktDscr;
}				/*               */




#ifdef NDIS51_MINIPORT
/*                                                                            */
/* 
        
 
        
 
              
*/
/*                                                                            */
VOID
mpPnPEventNotify(IN NDIS_HANDLE miniportAdapterContext,
		 IN NDIS_DEVICE_PNP_EVENT pnpEvent,
		 IN PVOID informationBuffer_p, IN UINT_32 informationBufferLength)
{
	P_GLUE_INFO_T prGlueInfo = (P_GLUE_INFO_T) miniportAdapterContext;
	P_QUE_ENTRY_T prQueueEntry;
	P_QUE_T prTxQue;
	PNDIS_PACKET prNdisPacket;

	DEBUGFUNC("MPPnPEventNotify");

	INITLOG(("\n"));

	prTxQue = &prGlueInfo->rTxQueue;

	switch (pnpEvent) {
	case NdisDevicePnPEventQueryRemoved:
		INITLOG(("NdisDevicePnPEventQueryRemoved\n"));
		break;

	case NdisDevicePnPEventRemoved:
		INITLOG(("NdisDevicePnPEventRemoved\n"));
		break;

	case NdisDevicePnPEventSurpriseRemoved:
		DBGPRINTF("NdisDevicePnPEventSurpriseRemoved\n");
		prGlueInfo->fgIsCardRemoved = TRUE;

		/*                     */
		if (prGlueInfo->i4OidPendingCount) {
			wlanReleasePendingOid(prGlueInfo->prAdapter, 0);
		}
		/*                   */
		if (prGlueInfo->i4TxPendingFrameNum) {
			kalFlushPendingTxPackets(prGlueInfo);
		}
		/*                                */
		if (prGlueInfo->i4TxPendingSecurityFrameNum) {
			kalClearSecurityFrames(prGlueInfo);
		}

		wlanCardEjected(prGlueInfo->prAdapter);
		INITLOG(("NdisDevicePnPEventSurpriseRemoved\n"));
		DBGPRINTF("NdisDevicePnPEventSurpriseRemoved -- Completed.\n");
		break;

	case NdisDevicePnPEventQueryStopped:
		INITLOG(("NdisDevicePnPEventQueryStopped\n"));
		break;

	case NdisDevicePnPEventStopped:
		INITLOG(("NdisDevicePnPEventStopped\n"));
		break;

	case NdisDevicePnPEventPowerProfileChanged:
#if DBG
		switch (*((PUINT_32) informationBuffer_p)) {
		case 0:	/*                         */
			INITLOG(("NdisDevicePnPEventPowerProfileChanged: NdisPowerProfileBattery\n"));
			break;

		case 1:	/*                          */
			INITLOG(("NdisDevicePnPEventPowerProfileChanged: NdisPowerProfileAcOnline\n"));
			break;

		default:
			INITLOG(("unknown power profile mode: %d\n",
				 *((PUINT_32) informationBuffer_p)));
		}
#endif

		break;

	default:
		INITLOG(("unknown PnP event 0x%x\n", pnpEvent));
	}



}				/*                  */

/*                                                                            */
/* 
        
 
        
 
              
*/
/*                                                                            */
VOID mpShutdown(IN PVOID shutdownContext)
{
}				/*            */

#endif				/*                 */
