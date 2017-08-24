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

/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
                                                                
 
                                                        
 
                
*/
/*                                                                            */
VOID nicpmSetFWOwn(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgEnableGlobalInt)
{
	UINT_32 u4RegValue;

	ASSERT(prAdapter);

	if (prAdapter->fgIsFwOwn == TRUE) {
		return;
	} else {
		if (nicProcessIST(prAdapter) != WLAN_STATUS_NOT_INDICATING) {
			DBGLOG(INIT, STATE, ("FW OWN Failed due to pending INT\n"));
			/*                    */
			return;
		}
	}

	if (fgEnableGlobalInt) {
		prAdapter->fgIsIntEnableWithLPOwnSet = TRUE;
	} else {
		HAL_MCR_WR(prAdapter, MCR_WHLPCR, WHLPCR_FW_OWN_REQ_SET);

		HAL_MCR_RD(prAdapter, MCR_WHLPCR, &u4RegValue);
		if (u4RegValue & WHLPCR_FW_OWN_REQ_SET) {
			/*                                                                   */
			/*                             */
			HAL_MCR_WR(prAdapter, MCR_WHLPCR, WHLPCR_FW_OWN_REQ_CLR);

			return;
		}

		prAdapter->fgIsFwOwn = TRUE;

		DBGLOG(INIT, INFO, ("FW OWN\n"));
	}
}

VOID nicPmTriggerDriverOwn(IN P_ADAPTER_T prAdapter)
{
	UINT_32 u4RegValue = 0;
	
	HAL_MCR_RD(prAdapter, MCR_WHLPCR, &u4RegValue);

	if (u4RegValue & WHLPCR_FW_OWN_REQ_SET) {
		prAdapter->fgIsFwOwn = FALSE;
	}
	else {
		HAL_MCR_WR(prAdapter, MCR_WHLPCR, WHLPCR_FW_OWN_REQ_CLR);
	}
}


/*                                                                            */
/* 
                                                                 
 
                                                        
 
                
*/
/*                                                                            */
BOOLEAN nicpmSetDriverOwn(IN P_ADAPTER_T prAdapter)
{
#define LP_OWN_BACK_TOTAL_DELAY_MS      2048    /*                  */
#define LP_OWN_BACK_LOOP_DELAY_MS       1       /*                  */
#define LP_OWN_BACK_CLR_OWN_ITERATION   256     /*                  */
#define LP_OWN_BACK_FAILED_RETRY_CNT    5
#define LP_OWN_BACK_FAILED_LOG_SKIP_MS  2000
#define LP_OWN_BACK_FAILED_RESET_CNT    5

	BOOLEAN fgStatus = TRUE;
	UINT_32 i, u4CurrTick, u4RegValue = 0;
	BOOLEAN fgTimeout;

	ASSERT(prAdapter);

	if (prAdapter->fgIsFwOwn == FALSE)
		return fgStatus;

	DBGLOG(INIT, INFO, ("DRIVER OWN\n"));

	u4CurrTick = kalGetTimeTick();
	i = 0;
	while (1) {
		HAL_MCR_RD(prAdapter, MCR_WHLPCR, &u4RegValue);

		fgTimeout =
		    ((kalGetTimeTick() - u4CurrTick) > LP_OWN_BACK_TOTAL_DELAY_MS) ? TRUE : FALSE;

		if (u4RegValue & WHLPCR_FW_OWN_REQ_SET) {
			prAdapter->fgIsFwOwn = FALSE;
            prAdapter->u4OwnFailedCount = 0;
            prAdapter->u4OwnFailedLogCount = 0;
			break;
        }
        else if((i > LP_OWN_BACK_FAILED_RETRY_CNT) && 
            (kalIsCardRemoved(prAdapter->prGlueInfo) || fgIsBusAccessFailed || fgTimeout || wlanIsChipNoAck(prAdapter))) {

            if ((prAdapter->u4OwnFailedCount == 0) || 
                CHECK_FOR_TIMEOUT(u4CurrTick, prAdapter->rLastOwnFailedLogTime, MSEC_TO_SYSTIME(LP_OWN_BACK_FAILED_LOG_SKIP_MS))) {
                      
                DBGLOG(INIT, ERROR, ("LP cannot be own back, Timeout[%u](%ums), BusAccessError[%u], Reseting[%u], CardRemoved[%u] NoAck[%u] Cnt[%u]\n",
                    fgTimeout,
                    kalGetTimeTick() - u4CurrTick,
                    fgIsBusAccessFailed,
                    kalIsResetting(),
                    kalIsCardRemoved(prAdapter->prGlueInfo),
                    wlanIsChipNoAck(prAdapter), 
                    prAdapter->u4OwnFailedCount));

                DBGLOG(INIT, INFO, ("Skip LP own back failed log for next %ums\n", LP_OWN_BACK_FAILED_LOG_SKIP_MS));

                prAdapter->u4OwnFailedLogCount++;
                if(prAdapter->u4OwnFailedLogCount > LP_OWN_BACK_FAILED_RESET_CNT) {
                    /*               */
#if CFG_CHIP_RESET_SUPPORT                    
                    glResetTrigger(prAdapter);
#endif
                }
                GET_CURRENT_SYSTIME(&prAdapter->rLastOwnFailedLogTime);
            }
            
            prAdapter->u4OwnFailedCount++;
			fgStatus = FALSE;
			break;
		} else {
			if ((i & (LP_OWN_BACK_CLR_OWN_ITERATION - 1)) == 0) {
				/*                                                */
				HAL_MCR_WR(prAdapter, MCR_WHLPCR, WHLPCR_FW_OWN_REQ_CLR);
			}

			/*                                                */
			kalMsleep(LP_OWN_BACK_LOOP_DELAY_MS);
			i++;
		}
	}

	return fgStatus;
}

/*                                                                            */
/* 
                                                           
 
                                                        
 
                
*/
/*                                                                            */
BOOLEAN nicpmSetAcpiPowerD0(IN P_ADAPTER_T prAdapter)
{
	WLAN_STATUS u4Status = WLAN_STATUS_SUCCESS;
	UINT_32 u4Value = 0, u4WHISR = 0;
	UINT_16 au2TxCount[16];
	UINT_32 i;
#if CFG_ENABLE_FW_DOWNLOAD
	UINT_32 u4FwImgLength, u4FwLoadAddr;
	PVOID prFwMappingHandle;
	PVOID pvFwImageMapFile = NULL;
#if CFG_ENABLE_FW_DIVIDED_DOWNLOAD
	P_FIRMWARE_DIVIDED_DOWNLOAD_T prFwHead;
	BOOLEAN fgValidHead;
	const UINT_32 u4CRCOffset = offsetof(FIRMWARE_DIVIDED_DOWNLOAD_T, u4NumOfEntries);
#endif
#endif

	DEBUGFUNC("nicpmSetAcpiPowerD0");

	ASSERT(prAdapter);

	do {
		/*                                 */
		prAdapter->fgIsFwOwn = TRUE;
		prAdapter->fgWiFiInSleepyState = FALSE;
		prAdapter->rAcpiState = ACPI_STATE_D0;
		prAdapter->fgIsEnterD3ReqIssued = FALSE;

#if defined(MT6630)
		/*                                                  */
		ACQUIRE_POWER_CONTROL_FROM_PM(prAdapter);
#if !CFG_ENABLE_FULL_PM
		nicpmSetDriverOwn(prAdapter);
#endif

		/*                           */
		u4Status = nicInitializeAdapter(prAdapter);
		if (u4Status != WLAN_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR, ("nicInitializeAdapter failed!\n"));
			u4Status = WLAN_STATUS_FAILURE;
			break;
		}
#endif

#if CFG_ENABLE_FW_DOWNLOAD
		prFwMappingHandle =
		    kalFirmwareImageMapping(prAdapter->prGlueInfo, &pvFwImageMapFile,
					    &u4FwImgLength);
		if (!prFwMappingHandle) {
			DBGLOG(INIT, ERROR, ("Fail to load FW image from file!\n"));
			pvFwImageMapFile = NULL;
		}
#if defined(MT6630)
		if (pvFwImageMapFile) {
			/*                                                              */
			nicDisableInterrupt(prAdapter);

			/*                                                 */
			nicTxInitResetResource(prAdapter);

			/*                      */
			u4FwLoadAddr = kalGetFwLoadAddress(prAdapter->prGlueInfo);

#if CFG_ENABLE_FW_DIVIDED_DOWNLOAD
			/*                                                                        */
			prFwHead = (P_FIRMWARE_DIVIDED_DOWNLOAD_T) pvFwImageMapFile;

			if (prFwHead->u4Signature == MTK_WIFI_SIGNATURE &&
			    prFwHead->u4CRC == wlanCRC32((PUINT_8) pvFwImageMapFile + u4CRCOffset,
							 u4FwImgLength - u4CRCOffset)) {
				fgValidHead = TRUE;
			} else {
				fgValidHead = FALSE;
			}

			/*                                         */
			if (fgValidHead == TRUE) {
				wlanFwDvdDwnloadHandler(prAdapter, prFwHead, pvFwImageMapFile, &u4Status);
			} else
#endif
			{
				if (wlanImageSectionConfig(prAdapter,
							   u4FwLoadAddr,
							   u4FwImgLength,
							   TRUE) != WLAN_STATUS_SUCCESS) {
					DBGLOG(INIT, ERROR,
					       ("Firmware download configuration failed!\n"));

					u4Status = WLAN_STATUS_FAILURE;
					break;
				} else {
					wlanFwDwnloadHandler(prAdapter, u4FwImgLength, pvFwImageMapFile, &u4Status);
				}
			}
			/*               */
			if (u4Status != WLAN_STATUS_SUCCESS) {
				kalFirmwareImageUnmapping(prAdapter->prGlueInfo, prFwMappingHandle,
							  pvFwImageMapFile);
				break;
			}
#if !CFG_ENABLE_FW_DOWNLOAD_ACK
			/*                                                                    */
			if (wlanImageQueryStatus(prAdapter) != WLAN_STATUS_SUCCESS) {
				kalFirmwareImageUnmapping(prAdapter->prGlueInfo, prFwMappingHandle,
							  pvFwImageMapFile);
				u4Status = WLAN_STATUS_FAILURE;
				break;
			}
#endif

			kalFirmwareImageUnmapping(prAdapter->prGlueInfo, prFwMappingHandle,
						  pvFwImageMapFile);
		} else {
			u4Status = WLAN_STATUS_FAILURE;
			break;
		}

		/*                             */
#if CFG_OVERRIDE_FW_START_ADDRESS
		wlanConfigWifiFunc(prAdapter, TRUE, kalGetFwStartAddress(prAdapter->prGlueInfo));
#else
		wlanConfigWifiFunc(prAdapter, FALSE, 0);
#endif
#endif
#endif

		/*                                     */
		DBGLOG(INIT, TRACE, ("wlanAdapterStart(): Waiting for Ready bit..\n"));
		i = 0;
		while (1) {
			HAL_MCR_RD(prAdapter, MCR_WCIR, &u4Value);

			if (u4Value & WCIR_WLAN_READY) {
				DBGLOG(INIT, TRACE, ("Ready bit asserted\n"));
				break;
			} else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
				   || fgIsBusAccessFailed == TRUE) {
				u4Status = WLAN_STATUS_FAILURE;
				break;
			} else if (i >= CFG_RESPONSE_POLLING_TIMEOUT) {
				DBGLOG(INIT, ERROR, ("Waiting for Ready bit: Timeout\n"));
				u4Status = WLAN_STATUS_FAILURE;
				break;
			} else {
				i++;
				kalMsleep(10);
			}
		}

		if (u4Status == WLAN_STATUS_SUCCESS) {
			/*                            */
			HAL_READ_INTR_STATUS(prAdapter, 4, (PUINT_8) & u4WHISR);
			if (HAL_IS_TX_DONE_INTR(u4WHISR)) {
				HAL_READ_TX_RELEASED_COUNT(prAdapter, au2TxCount);
			}

			/*                                            */
			nicTxResetResource(prAdapter);

			/*                      */
			nicEnableInterrupt(prAdapter);

			/*                                */
			wlanUpdateBasicConfig(prAdapter);

			/*                           */
			nicApplyNetworkAddress(prAdapter);

			/*                                              */
			kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
						     WLAN_STATUS_MEDIA_DISCONNECT, NULL, 0);
		}

		RECLAIM_POWER_CONTROL_TO_PM(prAdapter, FALSE);

		/*                     */
		nicInitMGMT(prAdapter, NULL);

	} while (FALSE);

	if (u4Status != WLAN_STATUS_SUCCESS) {
		return FALSE;
	} else {
		return TRUE;
	}
}


/*                                                                            */
/* 
                                                           
 
                                                 
 
                
*/
/*                                                                            */
BOOLEAN nicpmSetAcpiPowerD3(IN P_ADAPTER_T prAdapter)
{
	UINT_32 i;

	ASSERT(prAdapter);

	/*                          */
	nicUninitMGMT(prAdapter);

	/*                      */
	nicDisableInterrupt(prAdapter);

	/*                                           */
	wlanSendNicPowerCtrlCmd(prAdapter, 1);

	/*                           */
	i = 0;
	while (i < CFG_IST_LOOP_COUNT && nicProcessIST(prAdapter) != WLAN_STATUS_NOT_INDICATING) {
		i++;
	};

	/*                      */
	nicTxRelease(prAdapter, TRUE);

	/*                                                */
	kalClearSecurityFrames(prAdapter->prGlueInfo);
	kalClearMgmtFrames(prAdapter->prGlueInfo);

	/*                                                  */
	kalFlushPendingTxPackets(prAdapter->prGlueInfo);

	/*                         */
	nicpmSetFWOwn(prAdapter, FALSE);

	/*                  */
	prAdapter->rAcpiState = ACPI_STATE_D3;

	return TRUE;
}


