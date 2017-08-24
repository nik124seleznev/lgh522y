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
                                                  
                                                                                
*/ extern BOOLEAN fgIsResetting;

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
	}
}

/*                                                                            */
/* 
                                                                 
 
                                                        
 
                
*/
/*                                                                            */
BOOLEAN nicpmSetDriverOwn(IN P_ADAPTER_T prAdapter)
{
#define LP_OWN_BACK_TOTAL_DELAY_MS      8192	/*                  */
#define LP_OWN_BACK_LOOP_DELAY_MS       1	/*                  */
#define LP_OWN_BACK_CLR_OWN_ITERATION   256	/*                  */

	BOOLEAN fgStatus = TRUE;
	UINT_32 i, u4CurrTick, u4RegValue = 0;

	ASSERT(prAdapter);

	if (prAdapter->fgIsFwOwn == FALSE)
		return fgStatus;

	u4CurrTick = kalGetTimeTick();
	i = 0;
	while (1) {
		HAL_MCR_RD(prAdapter, MCR_WHLPCR, &u4RegValue);

		if (u4RegValue & WHLPCR_FW_OWN_REQ_SET) {
			prAdapter->fgIsFwOwn = FALSE;
			break;
		} else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
			   || fgIsBusAccessFailed == TRUE
			   || (kalGetTimeTick() - u4CurrTick) > LP_OWN_BACK_TOTAL_DELAY_MS
			   || fgIsResetting == TRUE) {
			/*                                                                                  */
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
	UINT_8 aucTxCount[8];
	UINT_32 i;
#if CFG_ENABLE_FW_DOWNLOAD
	UINT_32 u4FwImgLength, u4FwLoadAddr, u4ImgSecSize;
	PVOID prFwMappingHandle;
	PVOID pvFwImageMapFile = NULL;
#if CFG_ENABLE_FW_DIVIDED_DOWNLOAD
	UINT_32 j;
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

#if defined(MT6620) || defined(MT6628)
		/*                                                  */
		ACQUIRE_POWER_CONTROL_FROM_PM(prAdapter);
#if !CFG_ENABLE_FULL_PM
		nicpmSetDriverOwn(prAdapter);
#endif

		/*                           */
		if ((u4Status = nicInitializeAdapter(prAdapter)) != WLAN_STATUS_SUCCESS) {
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
#if defined(MT6620) || defined(MT6628)
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
				for (i = 0; i < prFwHead->u4NumOfEntries; i++) {
#if CFG_ENABLE_FW_DOWNLOAD_AGGREGATION
					if (wlanImageSectionDownloadAggregated(prAdapter,
									       prFwHead->
									       arSection[i].
									       u4DestAddr,
									       prFwHead->
									       arSection[i].
									       u4Length,
									       (PUINT_8)
									       pvFwImageMapFile +
									       prFwHead->
									       arSection[i].
									       u4Offset) !=
					    WLAN_STATUS_SUCCESS) {
						DBGLOG(INIT, ERROR,
						       ("Firmware scatter download failed!\n"));
						u4Status = WLAN_STATUS_FAILURE;
					}
#else
					for (j = 0; j < prFwHead->arSection[i].u4Length;
					     j += CMD_PKT_SIZE_FOR_IMAGE) {
						if (j + CMD_PKT_SIZE_FOR_IMAGE <
						    prFwHead->arSection[i].u4Length)
							u4ImgSecSize = CMD_PKT_SIZE_FOR_IMAGE;
						else
							u4ImgSecSize =
							    prFwHead->arSection[i].u4Length - j;

						if (wlanImageSectionDownload(prAdapter,
									     prFwHead->arSection[i].
									     u4DestAddr + j,
									     u4ImgSecSize,
									     (PUINT_8)
									     pvFwImageMapFile +
									     prFwHead->arSection[i].
									     u4Offset + j) !=
						    WLAN_STATUS_SUCCESS) {
							DBGLOG(INIT, ERROR,
							       ("Firmware scatter download failed!\n"));
							u4Status = WLAN_STATUS_FAILURE;
							break;
						}
					}
#endif
					/*                                              */
					if (u4Status == WLAN_STATUS_FAILURE) {
						break;
					}
				}
			} else
#endif
#if CFG_ENABLE_FW_DOWNLOAD_AGGREGATION
			if (wlanImageSectionDownloadAggregated(prAdapter,
								       u4FwLoadAddr,
								       u4FwImgLength,
								       (PUINT_8)
								       pvFwImageMapFile) !=
				    WLAN_STATUS_SUCCESS) {
				DBGLOG(INIT, ERROR, ("Firmware scatter download failed!\n"));
				u4Status = WLAN_STATUS_FAILURE;
			}
#else
				for (i = 0; i < u4FwImgLength; i += CMD_PKT_SIZE_FOR_IMAGE) {
					if (i + CMD_PKT_SIZE_FOR_IMAGE < u4FwImgLength)
						u4ImgSecSize = CMD_PKT_SIZE_FOR_IMAGE;
					else
						u4ImgSecSize = u4FwImgLength - i;

					if (wlanImageSectionDownload(prAdapter,
								     u4FwLoadAddr + i,
								     u4ImgSecSize,
								     (PUINT_8) pvFwImageMapFile +
								     i) != WLAN_STATUS_SUCCESS) {
						DBGLOG(INIT, ERROR,
						       ("wlanImageSectionDownload failed!\n"));
						u4Status = WLAN_STATUS_FAILURE;
						break;
					}
				}
#endif

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

#elif defined(MT5931)
		if (pvFwImageMapFile) {
			DBGLOG(INIT, TRACE,
			       ("Download Address: 0x%08X\n",
				kalGetFwLoadAddress(prAdapter->prGlueInfo)));
			DBGLOG(INIT, TRACE, ("Firmware Length:  0x%08X\n", u4FwImgLength));

			do {
				/*                                    */
				HAL_MCR_WR(prAdapter, MCR_WMCSR, WMCSR_CHIP_RST);
				HAL_MCR_WR(prAdapter, MCR_WMCSR, 0);

				/*                       */
				i = 0;
				while (1) {
					HAL_MCR_RD(prAdapter, MCR_WMCSR, &u4Value);

					if (u4Value & WMCSR_INI_RDY) {
						DBGLOG(INIT, TRACE, ("INIT-RDY detected\n"));
						break;
					} else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
						   || fgIsBusAccessFailed == TRUE) {
						u4Status = WLAN_STATUS_FAILURE;
						break;
					} else if (i >= CFG_RESPONSE_POLLING_TIMEOUT) {
						DBGLOG(INIT, ERROR,
						       ("Waiting for Init Ready bit: Timeout\n"));
						u4Status = WLAN_STATUS_FAILURE;
						break;
					} else {
						i++;
						kalMsleep(10);
					}
				}

				/*                   */
				HAL_MCR_WR(prAdapter, MCR_FWCFG, u4FwImgLength >> 6);

				/*                    */
				HAL_MCR_WR(prAdapter, MCR_WMCSR, WMCSR_FWDLEN);

				/*                      */
				i = 0;
				while (1) {
					HAL_MCR_RD(prAdapter, MCR_WMCSR, &u4Value);

					if (u4Value & WMCSR_PLLRDY) {
						DBGLOG(INIT, TRACE, ("PLL-RDY detected\n"));
						break;
					} else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
						   || fgIsBusAccessFailed == TRUE) {
						u4Status = WLAN_STATUS_FAILURE;
						break;
					} else if (i >= CFG_RESPONSE_POLLING_TIMEOUT) {
						DBGLOG(INIT, ERROR,
						       ("Waiting for PLL Ready bit: Timeout\n"));
						u4Status = WLAN_STATUS_FAILURE;
						break;
					} else {
						i++;
						kalMsleep(10);
					}
				}

				/*                                           */
				HAL_MCR_WR(prAdapter, MCR_FWDLSR, FWDLSR_FWDL_MODE);

				/*                          */
				u4FwLoadAddr = kalGetFwLoadAddress(prAdapter->prGlueInfo);
				HAL_MCR_WR(prAdapter, MCR_FWDLDSAR, u4FwLoadAddr);

				/*                    */
				for (i = 0; i < u4FwImgLength; i += CMD_PKT_SIZE_FOR_IMAGE) {
					if (i + CMD_PKT_SIZE_FOR_IMAGE < u4FwImgLength)
						u4ImgSecSize = CMD_PKT_SIZE_FOR_IMAGE;
					else
						u4ImgSecSize = u4FwImgLength - i;

					if (wlanImageSectionDownload(prAdapter,
								     u4FwLoadAddr + i,
								     u4ImgSecSize,
								     (PUINT_8) pvFwImageMapFile +
								     i) != WLAN_STATUS_SUCCESS) {
						DBGLOG(INIT, ERROR,
						       ("Firmware scatter download failed!\n"));
						u4Status = WLAN_STATUS_FAILURE;
						break;
					}
				}

				/*                                   */
				i = 0;
				while (1) {
					HAL_MCR_RD(prAdapter, MCR_WMCSR, &u4Value);

					if (u4Value & WMCSR_DL_OK) {
						DBGLOG(INIT, TRACE, ("DL_OK detected\n"));
						break;
					} else if (kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
						   || fgIsBusAccessFailed == TRUE
						   || (u4Value & WMCSR_DL_FAIL)) {
						DBGLOG(INIT, ERROR,
						       ("DL_FAIL detected: 0x%08X\n", u4Value));
						u4Status = WLAN_STATUS_FAILURE;
						break;
					} else if (i >= CFG_RESPONSE_POLLING_TIMEOUT) {
						DBGLOG(INIT, ERROR,
						       ("Waiting for DL_OK/DL_FAIL bit: Timeout\n"));
						u4Status = WLAN_STATUS_FAILURE;
						break;
					} else {
						i++;
						kalMsleep(10);
					}
				}

				/*                                   */
				HAL_MCR_WR(prAdapter, MCR_FWDLSR, 0);

			} while (FALSE);
		} else {
			DBGLOG(INIT, ERROR, ("No Firmware found!\n"));
			u4Status = WLAN_STATUS_FAILURE;
			break;
		}

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

#if defined(MT5931)
		/*                */
		ACQUIRE_POWER_CONTROL_FROM_PM(prAdapter);
#if !CFG_ENABLE_FULL_PM
		nicpmSetDriverOwn(prAdapter);
#endif

		/*                           */
		if ((u4Status = nicInitializeAdapter(prAdapter)) != WLAN_STATUS_SUCCESS) {
			DBGLOG(INIT, ERROR, ("nicInitializeAdapter failed!\n"));
			u4Status = WLAN_STATUS_FAILURE;
			break;
		}
#endif

		if (u4Status == WLAN_STATUS_SUCCESS) {
			/*                            */
			HAL_READ_INTR_STATUS(prAdapter, 4, (PUINT_8) & u4WHISR);
			if (HAL_IS_TX_DONE_INTR(u4WHISR)) {
				HAL_READ_TX_RELEASED_COUNT(prAdapter, aucTxCount);
			}

			/*                                            */
			nicTxResetResource(prAdapter);

			/*                      */
			nicEnableInterrupt(prAdapter);

			/*                              */
			wlanUpdateNetworkAddress(prAdapter);

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
	nicTxRelease(prAdapter);

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
