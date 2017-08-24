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

VOID
nicCmdEventQueryMcrRead (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_PARAM_CUSTOM_MCR_RW_STRUC_T prMcrRdInfo;
    P_GLUE_INFO_T prGlueInfo;
    P_CMD_ACCESS_REG prCmdAccessReg;


    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //                               
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prCmdAccessReg = (P_CMD_ACCESS_REG)(pucEventBuf);

        u4QueryInfoLen = sizeof(PARAM_CUSTOM_MCR_RW_STRUC_T);

        prMcrRdInfo = (P_PARAM_CUSTOM_MCR_RW_STRUC_T) prCmdInfo->pvInformationBuffer;
        prMcrRdInfo->u4McrOffset = prCmdAccessReg->u4Address;
        prMcrRdInfo->u4McrData = prCmdAccessReg->u4Data;

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    return;

}


VOID
nicCmdEventQuerySwCtrlRead (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_PARAM_CUSTOM_SW_CTRL_STRUC_T prSwCtrlInfo;
    P_GLUE_INFO_T prGlueInfo;
    P_CMD_SW_DBG_CTRL_T prCmdSwCtrl;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //                               
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prCmdSwCtrl = (P_CMD_SW_DBG_CTRL_T)(pucEventBuf);

        u4QueryInfoLen = sizeof(PARAM_CUSTOM_SW_CTRL_STRUC_T);

        prSwCtrlInfo = (P_PARAM_CUSTOM_SW_CTRL_STRUC_T) prCmdInfo->pvInformationBuffer;
        prSwCtrlInfo->u4Id = prCmdSwCtrl->u4Id;
        prSwCtrlInfo->u4Data = prCmdSwCtrl->u4Data;

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    return;

}



VOID
nicCmdEventSetCommon (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    if (prCmdInfo->fgIsOid) {
        /*                              */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       prCmdInfo->u4InformationBufferLength,
                       WLAN_STATUS_SUCCESS);
    }

    return;
}

VOID
nicCmdEventSetDisassociate (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    if (prCmdInfo->fgIsOid) {
        /*                              */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       0,
                       WLAN_STATUS_SUCCESS);
    }

	DBGLOG(INIT, TRACE, ("DisByCmdE\n"));
    kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                    WLAN_STATUS_MEDIA_DISCONNECT,
                    NULL,
                    0);

#if !defined(LINUX)
    prAdapter->fgIsRadioOff = TRUE;
#endif

    return;
}

VOID
nicCmdEventSetIpAddress (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4Count;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    u4Count = (prCmdInfo->u4SetInfoLen - OFFSET_OF(CMD_SET_NETWORK_ADDRESS_LIST, arNetAddress))
        / sizeof(IPV4_NETWORK_ADDRESS) ;

    if (prCmdInfo->fgIsOid) {
        /*                              */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       OFFSET_OF(PARAM_NETWORK_ADDRESS_LIST, arAddress) + u4Count *
                                (OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress) + sizeof(PARAM_NETWORK_ADDRESS_IP)),
                       WLAN_STATUS_SUCCESS);
    }

    return;
}

VOID
nicCmdEventQueryRfTestATInfo(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_TEST_STATUS prTestStatus, prQueryBuffer;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prTestStatus = (P_EVENT_TEST_STATUS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prQueryBuffer = (P_EVENT_TEST_STATUS) prCmdInfo->pvInformationBuffer;

        kalMemCopy(prQueryBuffer, prTestStatus, sizeof(EVENT_TEST_STATUS));

        u4QueryInfoLen = sizeof(EVENT_TEST_STATUS);

        /*                                */
        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }

    return;
}

VOID
nicCmdEventQueryLinkQuality(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    PARAM_RSSI rRssi, *prRssi;
    P_EVENT_LINK_QUALITY prLinkQuality;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prLinkQuality = (P_EVENT_LINK_QUALITY)pucEventBuf;

    rRssi = (PARAM_RSSI)prLinkQuality->cRssi; //                                       
	DBGLOG(INIT, INFO, ("<rxm> %s: rRssi = %d\n", __FUNCTION__, rRssi));

    if(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX].eConnectionState == PARAM_MEDIA_STATE_CONNECTED) {
        if(rRssi > PARAM_WHQL_RSSI_MAX_DBM)
            rRssi = PARAM_WHQL_RSSI_MAX_DBM;
        else if(rRssi < PARAM_WHQL_RSSI_MIN_DBM)
            rRssi = PARAM_WHQL_RSSI_MIN_DBM;
    }
    else {
        rRssi = PARAM_WHQL_RSSI_MIN_DBM;
    }

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prRssi = (PARAM_RSSI *) prCmdInfo->pvInformationBuffer;

        kalMemCopy(prRssi, &rRssi, sizeof(PARAM_RSSI));
        u4QueryInfoLen = sizeof(PARAM_RSSI);

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


/*                                                                            */
/* 
                                                                        
 
                                                         
                                                           
                    
 
              
*/
/*                                                                            */
VOID
nicCmdEventQueryLinkSpeed(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_LINK_QUALITY prLinkQuality;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4LinkSpeed;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prLinkQuality = (P_EVENT_LINK_QUALITY)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        pu4LinkSpeed = (PUINT_32)(prCmdInfo->pvInformationBuffer);

        if(prLinkQuality->u2LinkSpeed == 0) {
            *pu4LinkSpeed = 10000; /*                      */
        }
        else {
            *pu4LinkSpeed = prLinkQuality->u2LinkSpeed * 5000;
        }

        u4QueryInfoLen = sizeof(UINT_32);

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryStatistics(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_PARAM_802_11_STATISTICS_STRUCT_T prStatistics;
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        u4QueryInfoLen = sizeof(PARAM_802_11_STATISTICS_STRUCT_T);
        prStatistics = (P_PARAM_802_11_STATISTICS_STRUCT_T) prCmdInfo->pvInformationBuffer;

        prStatistics->u4Length = sizeof(PARAM_802_11_STATISTICS_STRUCT_T);
        prStatistics->rTransmittedFragmentCount
            = prEventStatistics->rTransmittedFragmentCount;
        prStatistics->rMulticastTransmittedFrameCount
            = prEventStatistics->rMulticastTransmittedFrameCount;
        prStatistics->rFailedCount
            = prEventStatistics->rFailedCount;
        prStatistics->rRetryCount
            = prEventStatistics->rRetryCount;
        prStatistics->rMultipleRetryCount
            = prEventStatistics->rMultipleRetryCount;
        prStatistics->rRTSSuccessCount
            = prEventStatistics->rRTSSuccessCount;
        prStatistics->rRTSFailureCount
            = prEventStatistics->rRTSFailureCount;
        prStatistics->rACKFailureCount
            = prEventStatistics->rACKFailureCount;
        prStatistics->rFrameDuplicateCount
            = prEventStatistics->rFrameDuplicateCount;
        prStatistics->rReceivedFragmentCount
            = prEventStatistics->rReceivedFragmentCount;
        prStatistics->rMulticastReceivedFrameCount
            = prEventStatistics->rMulticastReceivedFrameCount;
        prStatistics->rFCSErrorCount
            = prEventStatistics->rFCSErrorCount;
        prStatistics->rTKIPLocalMICFailures.QuadPart
            = 0;
        prStatistics->rTKIPICVErrors.QuadPart
            = 0;
        prStatistics->rTKIPCounterMeasuresInvoked.QuadPart
            = 0;
        prStatistics->rTKIPReplays.QuadPart
            = 0;
        prStatistics->rCCMPFormatErrors.QuadPart
            = 0;
        prStatistics->rCCMPReplays.QuadPart
            = 0;
        prStatistics->rCCMPDecryptErrors.QuadPart
            = 0;
        prStatistics->rFourWayHandshakeFailures.QuadPart
            = 0;
        prStatistics->rWEPUndecryptableCount.QuadPart
            = 0;
        prStatistics->rWEPICVErrorCount.QuadPart
            = 0;
        prStatistics->rDecryptSuccessCount.QuadPart
            = 0;
        prStatistics->rDecryptFailureCount.QuadPart
            = 0;

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}

VOID
nicCmdEventEnterRfTest(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4WHISR = 0, u4Value = 0;
    UINT_8 aucTxCount[8];

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    //              
    prAdapter->fgTestMode = TRUE;

    //                                 
    if(kalGetMediaStateIndicated(prAdapter->prGlueInfo) != PARAM_MEDIA_STATE_DISCONNECTED) {
        kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                WLAN_STATUS_MEDIA_DISCONNECT,
                NULL,
                0);
    }

    //                     
    nicTxRelease(prAdapter);

    //                                               
    kalClearSecurityFrames(prAdapter->prGlueInfo);
    kalClearMgmtFrames(prAdapter->prGlueInfo);

    //                                                 
    kalFlushPendingTxPackets(prAdapter->prGlueInfo);

    //                            
    nicUninitMGMT(prAdapter);

    nicResetSystemService(prAdapter);
    nicInitMGMT(prAdapter, NULL);

    //                     
    HAL_INTR_DISABLE(prAdapter);

    //                                                           
    kalMsleep(500);
    while(1) {
        HAL_MCR_RD(prAdapter, MCR_WCIR, &u4Value);

        if (u4Value & WCIR_WLAN_READY) {
            break;
        }
        else if(kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                || fgIsBusAccessFailed == TRUE) {
            if (prCmdInfo->fgIsOid) {
                /*                              */
                kalOidComplete(prAdapter->prGlueInfo,
                        prCmdInfo->fgSetQuery,
                        prCmdInfo->u4SetInfoLen,
                        WLAN_STATUS_NOT_SUPPORTED);

            }
            return;
        }
        else
            kalMsleep(10);
    }

    //                          
    HAL_READ_INTR_STATUS(prAdapter, 4, (PUINT_8)&u4WHISR);
    if(HAL_IS_TX_DONE_INTR(u4WHISR)) {
        HAL_READ_TX_RELEASED_COUNT(prAdapter, aucTxCount);
    }

    //                    
    nicTxResetResource(prAdapter);

    //                       
    HAL_INTR_ENABLE(prAdapter);

    //                         
    if (prCmdInfo->fgIsOid) {
        /*                              */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       prCmdInfo->u4SetInfoLen,
                       WLAN_STATUS_SUCCESS);
    }

#if CFG_SUPPORT_NVRAM
    //                         
    wlanLoadManufactureData(prAdapter, kalGetConfiguration(prAdapter->prGlueInfo));
#endif

    return;
}

VOID
nicCmdEventLeaveRfTest(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4WHISR = 0, u4Value = 0;
    UINT_8 aucTxCount[8];

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    //                     
    HAL_INTR_DISABLE(prAdapter);

    //                                                          
    kalMsleep(500);
    while(1) {
        HAL_MCR_RD(prAdapter, MCR_WCIR, &u4Value);

        if (u4Value & WCIR_WLAN_READY) {
            break;
        }
        else if(kalIsCardRemoved(prAdapter->prGlueInfo) == TRUE
                || fgIsBusAccessFailed == TRUE) {
            if (prCmdInfo->fgIsOid) {
                /*                              */
                kalOidComplete(prAdapter->prGlueInfo,
                        prCmdInfo->fgSetQuery,
                        prCmdInfo->u4SetInfoLen,
                        WLAN_STATUS_NOT_SUPPORTED);

            }
            return;
        }
        else {
            kalMsleep(10);
        }
    }

    //                          
    HAL_READ_INTR_STATUS(prAdapter, 4, (PUINT_8)&u4WHISR);
    if(HAL_IS_TX_DONE_INTR(u4WHISR)) {
        HAL_READ_TX_RELEASED_COUNT(prAdapter, aucTxCount);
    }

    //                    
    nicTxResetResource(prAdapter);

    //                       
    HAL_INTR_ENABLE(prAdapter);

    //                            
    prAdapter->fgTestMode = FALSE;

    //                         
    if (prCmdInfo->fgIsOid) {
        /*                              */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       prCmdInfo->u4SetInfoLen,
                       WLAN_STATUS_SUCCESS);
    }

    /*                             */
    if(kalGetMediaStateIndicated(prAdapter->prGlueInfo) != PARAM_MEDIA_STATE_DISCONNECTED) {

        kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                WLAN_STATUS_MEDIA_DISCONNECT,
                NULL,
                0);

        prAdapter->rWlanInfo.u4SysTime = kalGetTimeTick();
    }

#if CFG_SUPPORT_NVRAM
    /*                          */
    wlanLoadManufactureData(prAdapter, kalGetConfiguration(prAdapter->prGlueInfo));
#endif

    /*                              */
    wlanUpdateNetworkAddress(prAdapter);

    return;
}

VOID
nicCmdEventQueryAddress(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_GLUE_INFO_T prGlueInfo;
    P_EVENT_BASIC_CONFIG prEventBasicConfig;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    prEventBasicConfig = (P_EVENT_BASIC_CONFIG)(pucEventBuf);

    //                
    kalMemCopy(&(prAdapter->rMyMacAddr), &(prEventBasicConfig->rMyMacAddr), MAC_ADDR_LEN);

    //                               
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        kalMemCopy(prCmdInfo->pvInformationBuffer, &(prEventBasicConfig->rMyMacAddr), MAC_ADDR_LEN);
        u4QueryInfoLen = MAC_ADDR_LEN;

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    //                                               
    COPY_MAC_ADDR(prAdapter->rWifiVar.aucMacAddress, prAdapter->rMyMacAddr);
    COPY_MAC_ADDR(prAdapter->rWifiVar.aucDeviceAddress, prAdapter->rMyMacAddr);
    prAdapter->rWifiVar.aucDeviceAddress[0] ^= MAC_ADDR_LOCAL_ADMIN;

    COPY_MAC_ADDR(prAdapter->rWifiVar.aucInterfaceAddress, prAdapter->rMyMacAddr);
    prAdapter->rWifiVar.aucInterfaceAddress[0] ^= MAC_ADDR_LOCAL_ADMIN;

    COPY_MAC_ADDR(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX].aucOwnMacAddr,
            prAdapter->rMyMacAddr);

#if CFG_ENABLE_WIFI_DIRECT
    if(prAdapter->fgIsP2PRegistered) {
        COPY_MAC_ADDR(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_P2P_INDEX].aucOwnMacAddr,
                prAdapter->rWifiVar.aucDeviceAddress);
    }
#endif

#if CFG_ENABLE_BT_OVER_WIFI
    COPY_MAC_ADDR(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_BOW_INDEX].aucOwnMacAddr,
            prAdapter->rWifiVar.aucDeviceAddress);
#endif

#if CFG_TEST_WIFI_DIRECT_GO
    if (prAdapter->rWifiVar.prP2pFsmInfo->eCurrentState == P2P_STATE_IDLE) {
        wlanEnableP2pFunction(prAdapter);

        wlanEnableATGO(prAdapter);
    }
#endif

    kalUpdateMACAddress(prAdapter->prGlueInfo, prAdapter->rWifiVar.aucMacAddress);

    return;
}

VOID
nicCmdEventQueryMcastAddr(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_GLUE_INFO_T prGlueInfo;
    P_EVENT_MAC_MCAST_ADDR prEventMacMcastAddr;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //                               
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEventMacMcastAddr = (P_EVENT_MAC_MCAST_ADDR)(pucEventBuf);

        u4QueryInfoLen = prEventMacMcastAddr->u4NumOfGroupAddr * MAC_ADDR_LEN;

        //                    
        if (prCmdInfo->u4InformationBufferLength < u4QueryInfoLen) {
            kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_BUFFER_TOO_SHORT);
        }
        else {
            kalMemCopy(prCmdInfo->pvInformationBuffer,
                    prEventMacMcastAddr->arAddress,
                    prEventMacMcastAddr->u4NumOfGroupAddr * MAC_ADDR_LEN);

            kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
        }
    }
}

VOID
nicCmdEventQueryEepromRead(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_PARAM_CUSTOM_EEPROM_RW_STRUC_T prEepromRdInfo;
    P_GLUE_INFO_T prGlueInfo;
    P_EVENT_ACCESS_EEPROM prEventAccessEeprom;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //                               
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEventAccessEeprom = (P_EVENT_ACCESS_EEPROM)(pucEventBuf);

        u4QueryInfoLen = sizeof(PARAM_CUSTOM_EEPROM_RW_STRUC_T);

        prEepromRdInfo = (P_PARAM_CUSTOM_EEPROM_RW_STRUC_T) prCmdInfo->pvInformationBuffer;
        prEepromRdInfo->ucEepromIndex = (UINT_8)(prEventAccessEeprom->u2Offset);
        prEepromRdInfo->u2EepromData = prEventAccessEeprom->u2Data;

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    return;

}


VOID
nicCmdEventSetMediaStreamMode(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    PARAM_MEDIA_STREAMING_INDICATION rParamMediaStreamIndication;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    if (prCmdInfo->fgIsOid) {
        /*                              */
        kalOidComplete(prAdapter->prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       prCmdInfo->u4SetInfoLen,
                       WLAN_STATUS_SUCCESS);
    }

    rParamMediaStreamIndication.rStatus.eStatusType =
        ENUM_STATUS_TYPE_MEDIA_STREAM_MODE;
    rParamMediaStreamIndication.eMediaStreamMode =
        prAdapter->rWlanInfo.eLinkAttr.ucMediaStreamMode == 0 ?
            ENUM_MEDIA_STREAM_OFF : ENUM_MEDIA_STREAM_ON;

    kalIndicateStatusAndComplete(prAdapter->prGlueInfo,
                WLAN_STATUS_MEDIA_SPECIFIC_INDICATION,
                (PVOID)&rParamMediaStreamIndication,
                sizeof(PARAM_MEDIA_STREAMING_INDICATION));
}


/*                      */
VOID
nicCmdEventQueryXmitOk(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rTransmittedFragmentCount.QuadPart;
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = prEventStatistics->rTransmittedFragmentCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvOk(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rReceivedFragmentCount.QuadPart;
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = prEventStatistics->rReceivedFragmentCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}

VOID
nicCmdEventQueryXmitError(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rFailedCount.QuadPart;
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = (UINT_64) prEventStatistics->rFailedCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvError(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rFCSErrorCount.QuadPart;
            //                                                                      
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = prEventStatistics->rFCSErrorCount.QuadPart;
            //                                                                      
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvNoBuffer(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = 0; //        
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = 0; //       
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvCrcError(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rFCSErrorCount.QuadPart;
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = prEventStatistics->rFCSErrorCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryRecvErrorAlignment(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) 0; //      
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = 0; //      
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryXmitOneCollision(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) (prEventStatistics->rMultipleRetryCount.QuadPart - prEventStatistics->rRetryCount.QuadPart);
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = (UINT_64) (prEventStatistics->rMultipleRetryCount.QuadPart - prEventStatistics->rRetryCount.QuadPart);
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryXmitMoreCollisions(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rMultipleRetryCount.QuadPart;
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = (UINT_64) prEventStatistics->rMultipleRetryCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


VOID
nicCmdEventQueryXmitMaxCollisions(
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    P_EVENT_STATISTICS prEventStatistics;
    P_GLUE_INFO_T prGlueInfo;
    UINT_32 u4QueryInfoLen;
    PUINT_32 pu4Data;
    PUINT_64 pu8Data;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);

    prEventStatistics = (P_EVENT_STATISTICS)pucEventBuf;

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;

        if(prCmdInfo->u4InformationBufferLength == sizeof(UINT_32)) {
            u4QueryInfoLen = sizeof(UINT_32);

            pu4Data = (PUINT_32) prCmdInfo->pvInformationBuffer;
            *pu4Data = (UINT_32) prEventStatistics->rFailedCount.QuadPart;
        }
        else {
            u4QueryInfoLen = sizeof(UINT_64);

            pu8Data = (PUINT_64) prCmdInfo->pvInformationBuffer;
            *pu8Data = (UINT_64) prEventStatistics->rFailedCount.QuadPart;
        }

        kalOidComplete(prGlueInfo,
                       prCmdInfo->fgSetQuery,
                       u4QueryInfoLen,
                       WLAN_STATUS_SUCCESS);
    }
}


/*                                                                            */
/* 
                                                                           
 
                                                             
                                                              
 
              
               
*/
/*                                                                            */
VOID
nicOidCmdTimeoutCommon (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
    )
{
    ASSERT(prAdapter);

    kalOidComplete(prAdapter->prGlueInfo,
            prCmdInfo->fgSetQuery,
            0,
            WLAN_STATUS_FAILURE);
}


/*                                                                            */
/* 
                                                           
 
                                                      
 
              
*/
/*                                                                            */
VOID
nicCmdTimeoutCommon (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
    )
{
    ASSERT(prAdapter);
}


/*                                                                            */
/* 
                                                                      
                                                                          
 
                                                             
                                                              
 
              
 
*/
/*                                                                            */
VOID
nicOidCmdEnterRFTestTimeout (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo
    )
{
    ASSERT(prAdapter);

    //                            
    nicTxRelease(prAdapter);

    //                                               
    kalClearSecurityFrames(prAdapter->prGlueInfo);
    kalClearMgmtFrames(prAdapter->prGlueInfo);

    //                                                 
    kalFlushPendingTxPackets(prAdapter->prGlueInfo);

    //                           
    kalOidComplete(prAdapter->prGlueInfo,
            prCmdInfo->fgSetQuery,
            0,
            WLAN_STATUS_FAILURE);
}


/*                                                                            */
/* 
                                                                 
                         
 
                                                             
                                                             
                                                  
 
              
 
*/
/*                                                                            */
VOID
nicCmdEventQueryMemDump (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_PARAM_CUSTOM_MEM_DUMP_STRUC_T prMemDumpInfo;
    P_GLUE_INFO_T prGlueInfo;
    P_EVENT_DUMP_MEM_T prEventDumpMem;
    static UINT_8 aucPath[256];
    static UINT_32 u4CurTimeTick;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //                               
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEventDumpMem = (P_EVENT_DUMP_MEM_T)(pucEventBuf);

        u4QueryInfoLen = sizeof(P_PARAM_CUSTOM_MEM_DUMP_STRUC_T);

        prMemDumpInfo = (P_PARAM_CUSTOM_MEM_DUMP_STRUC_T) prCmdInfo->pvInformationBuffer;
        prMemDumpInfo->u4Address = prEventDumpMem->u4Address;
        prMemDumpInfo->u4Length = prEventDumpMem->u4Length;
        prMemDumpInfo->u4RemainLength = prEventDumpMem->u4RemainLength;
        prMemDumpInfo->ucFragNum = prEventDumpMem->ucFragNum;

#if 0
        do{
            UINT_32 i = 0;
            printk("Rx dump address 0x%X, Length %d, FragNum %d, remain %d\n",
                prEventDumpMem->u4Address,
                prEventDumpMem->u4Length,
                prEventDumpMem->ucFragNum,
                prEventDumpMem->u4RemainLength);
#if 0
            for(i = 0; i < prEventDumpMem->u4Length; i++) {
                printk("%02X ", prEventDumpMem->aucBuffer[i]);
                if(i % 32 == 31) {
                    printk("\n");
                }
            }
#endif
        }while(FALSE);
#endif

        if(prEventDumpMem->ucFragNum == 1) {
        /*                               
                                                                                               
                */
            u4CurTimeTick = kalGetTimeTick();
            sprintf(aucPath, "/sdcard/dump_%d_0x%08X_%d.hex",
                u4CurTimeTick,
                prEventDumpMem->u4Address,
                prEventDumpMem->u4Length + prEventDumpMem->u4RemainLength);
            kalWriteToFile(aucPath, FALSE, &prEventDumpMem->aucBuffer[0], prEventDumpMem->u4Length);
        }
        else {
            /*                                            */
            kalWriteToFile(aucPath, TRUE, &prEventDumpMem->aucBuffer[0], prEventDumpMem->u4Length);
        }

        if(prEventDumpMem->u4RemainLength == 0 ||
           prEventDumpMem->u4Address == 0xFFFFFFFF) {
           /*                                                      */
           /*                           */
            *((PUINT_32)prCmdInfo->pvInformationBuffer) = u4CurTimeTick;
            kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
        }
        else {
            /*                                                           */
            wlanSendMemDumpCmd(
                prAdapter,
                prCmdInfo->pvInformationBuffer,
                prCmdInfo->u4InformationBufferLength);
        }
    }

    return;

}

#if CFG_SUPPORT_BUILD_DATE_CODE
/*                                                                            */
/* 
                                                                           
                           
 
                                                             
                                                              
                                                       
 
              
 
*/
/*                                                                            */
VOID
nicCmdEventBuildDateCode (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_EVENT_BUILD_DATE_CODE prEvent;
    P_GLUE_INFO_T prGlueInfo;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //                               
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEvent = (P_EVENT_BUILD_DATE_CODE)pucEventBuf;

        u4QueryInfoLen = sizeof(UINT_8) * 16;
        kalMemCopy(prCmdInfo->pvInformationBuffer, prEvent->aucDateCode, sizeof(UINT_8) * 16);

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    return;
}
#endif


/*                                                                            */
/* 
                                                                     
                           
 
                                                             
                                                              
                                                       
 
              
 
*/
/*                                                                            */
VOID
nicCmdEventQueryStaStatistics (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_EVENT_STA_STATISTICS_T prEvent;
    P_GLUE_INFO_T prGlueInfo;
    P_PARAM_GET_STA_STATISTICS prStaStatistics;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);
    ASSERT(prCmdInfo->pvInformationBuffer);

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEvent = (P_EVENT_STA_STATISTICS_T)pucEventBuf;
        prStaStatistics = (P_PARAM_GET_STA_STATISTICS)prCmdInfo->pvInformationBuffer;

        u4QueryInfoLen = sizeof(PARAM_GET_STA_STA_STATISTICS);

        /*                             */
        if(prEvent->u4Flags & BIT(0)) {
            prStaStatistics->ucPer = prEvent->ucPer;
            prStaStatistics->ucRcpi = prEvent->ucRcpi;
            prStaStatistics->u4PhyMode = prEvent->u4PhyMode;
            prStaStatistics->u2LinkSpeed = prEvent->u2LinkSpeed;
            
            prStaStatistics->u4TxFailCount = prEvent->u4TxFailCount;
            prStaStatistics->u4TxLifeTimeoutCount = prEvent->u4TxLifeTimeoutCount;

            if(prEvent->u4TxCount) {
                UINT_32 u4TxDoneAirTimeMs = USEC_TO_MSEC(prEvent->u4TxDoneAirTime * 32);
                
                prStaStatistics->u4TxAverageAirTime = (u4TxDoneAirTimeMs / prEvent->u4TxCount);
            }
            else {
                prStaStatistics->u4TxAverageAirTime = 0;
            }            
        }
        
        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }
    
}
#if CFG_AUTO_CHANNEL_SEL_SUPPORT

//                         

/*                                                                            */
/* 
                                                                     
                           
 
                                                             
                                                              
                                                       
 
              
 
*/
/*                                                                            */
VOID
nicCmdEventQueryChannelLoad (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_EVENT_CHN_LOAD_T prEvent;
    P_GLUE_INFO_T prGlueInfo;
    P_PARAM_GET_CHN_LOAD prChnLoad;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);
    ASSERT(prCmdInfo->pvInformationBuffer);

    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEvent = (P_EVENT_CHN_LOAD_T)pucEventBuf;//                             
        prChnLoad = (P_PARAM_GET_CHN_LOAD)prCmdInfo->pvInformationBuffer; //                                                    

        u4QueryInfoLen = sizeof(PARAM_GET_CHN_LOAD);

        /*                             */
        if(prEvent->u4Flags & BIT(0)) {
            prChnLoad->rEachChnLoad[0].ucChannel = prEvent->ucChannel;
            prChnLoad->rEachChnLoad[0].u2ChannelLoad = prEvent->u2ChannelLoad;
			printk("CHN[%d]=%d\n",prEvent->ucChannel,prEvent->u2ChannelLoad);
            
        }
        
        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }
    
}
VOID
nicCmdEventQueryLTESafeChn (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
	UINT_8 ucIdx=0;
    UINT_32 u4QueryInfoLen;
    P_EVENT_LTE_MODE_T prEvent;
    P_GLUE_INFO_T prGlueInfo;
    P_PARAM_GET_CHN_LOAD prLteSafeChnInfo;

    ASSERT(prAdapter);
    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);
    ASSERT(prCmdInfo->pvInformationBuffer);
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEvent = (P_EVENT_LTE_MODE_T)pucEventBuf;//                             

		prLteSafeChnInfo = (P_PARAM_GET_CHN_LOAD)prCmdInfo->pvInformationBuffer;

        u4QueryInfoLen = sizeof(PARAM_GET_CHN_LOAD);

        /*                             */
        if(prEvent->u4Flags & BIT(0)) {
			for(ucIdx=0;ucIdx<(NL80211_TESTMODE_AVAILABLE_CHAN_NUM-1);ucIdx++) {
            	prLteSafeChnInfo->rLteSafeChnList.au4SafeChannelBitmask[ucIdx]= prEvent->rLteSafeChn.au4SafeChannelBitmask[ucIdx];
			
				DBGLOG(P2P, INFO,("[Auto Channel]LTE safe channels [%d]=[%x]\n",ucIdx,(UINT32)prLteSafeChnInfo->rLteSafeChnList.au4SafeChannelBitmask[ucIdx]));
			}
        }
        
        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }
    
}
#endif
/*                                                                            */
/* 
                                                                 
                           
 
                                                             
                                                              
                                                       
 
              
 
*/
/*                                                                            */

VOID
nicCmdEventGetBSSInfo (
    IN P_ADAPTER_T  prAdapter,
    IN P_CMD_INFO_T prCmdInfo,
    IN PUINT_8      pucEventBuf
    )
{
    UINT_32 u4QueryInfoLen;
    P_EVENT_AIS_BSS_INFO_T prEvent;
    P_GLUE_INFO_T prGlueInfo;
    P_BSS_INFO_T prAisBssInfo;

    ASSERT(prAdapter);

    ASSERT(prCmdInfo);
    ASSERT(pucEventBuf);

    //                               
    if (prCmdInfo->fgIsOid) {
        prGlueInfo = prAdapter->prGlueInfo;
        prEvent = (P_EVENT_AIS_BSS_INFO_T)pucEventBuf;

        u4QueryInfoLen = sizeof(EVENT_AIS_BSS_INFO_T);
        kalMemCopy(prCmdInfo->pvInformationBuffer, prEvent, sizeof(EVENT_AIS_BSS_INFO_T));
        prAisBssInfo = &(prAdapter->rWifiVar.arBssInfo[NETWORK_TYPE_AIS_INDEX]);

    if(prEvent->eCurrentOPMode == OP_MODE_INFRASTRUCTURE){
        if (prEvent->eConnectionState != prAisBssInfo->eConnectionState) {
            DBGLOG(INIT, ERROR, ("driver[%d] & FW[%d] status didn't sync !!!\n",
                    prAisBssInfo->eConnectionState, prEvent->eCurrentOPMode));
            aisFsmStateAbort(prAdapter, DISCONNECT_REASON_CODE_RADIO_LOST, FALSE);
        }
    }

        kalOidComplete(prGlueInfo, prCmdInfo->fgSetQuery, u4QueryInfoLen, WLAN_STATUS_SUCCESS);
    }

    return;
}




