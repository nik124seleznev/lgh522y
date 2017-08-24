/*
                                                                                    
*/

/*                      
                                                                       

                       
*/



/*
                     
  
                        
       
                                                                
  
                   
                                                                                                                                 
                                                                                                                                    
  
                   
                                                                                                     
                                                                        
  
                   
                                                                                                          
                                     
  
                   
                                                                                                          
                                                  
  
                   
                                                                                          
                                            
  
                          
                                                                                 
                                                
  
                      
                                                                                            
                                                                  
  
                      
                                                                                            
    
  
                         
                                                                 
                  
  
                         
                                                                 
                              
  
                   
                                                                                               
                                                                                        
  
                        
                                                                                                                             
                                  
  
                           
                                                                 
                                                          
  
                         
                                                                                                                    
                                  
  
                   
                                                                                       
                                                                                      
  
                   
                                                                         
                                                            
  
                   
                                                                    
                                                
                                                                             
  
                   
                                                                                                            
                                            
  
                          
                                                             
                  
                                                                        
                                                                   
                                                                             
                                                       
  
                          
                                                                                          
                                                                
  
                   
                                                                                                             
                                                               
                                                                          
                                                                                   
  
                   
                                                                                                                                                            
                                                                                  
  
                          
                                                                                                                        
                                                             
  
                   
                                                                                                                                                         
                                               
                                                                   
                                                                                                       
                                                                           
                                            
                                                      
  
                        
                                                                                                         
                                       
  
                   
                                                                                                                                                              
                                                  
  
                   
                                                                                      
                                                                       
  
                   
                                                                         
                                                                                           
  
                   
                                                                         
                                                                         
  
                   
                                                                                 
                                                           
  
                   
                                                                                             
                                                                                
  
                   
                                                                                    
                                                                                        
  
                   
                                                                                 
                                     
  
                        
       
                                             
                                                                                          
  
                          
       
    
  
                   
  
                                                       
                                                             
  
                   
  
                                                                                                 
                                                                                                   
                                                                                                                         
  
                   
  
                                                                               
  
                   
                                                    
                                                                                                                        
  
                   
                                                    
                                     
  
                   
                                                    
                        
  
                   
                                                    
                      
                                                                      
                                                           
                                         
                                          
  
                   
                                                    
                               
  
                         
                                                
                                   
  
                   
                                                          
                                                                          
                                          
                                                         
  
                   
                                                          
                                                        
  
                   
                                                          
                                                                                                      
  
                   
                                                          
                                               
                                            
  
                   
                                                          
                                                     
  
                   
                                                          
                                                        
  
                   
                                                          
                                                                 
  
                   
                                                          
                                                              
  
                   
                                                          
                                                            
  
                   
                                                          
                                          
  
                   
                                                          
                                     
                                                                                          
  
                   
                                                          
                                                                              
                                                                      
                                                                            
                                          
                                                                                     
                                                                    
  
                   
                                                          
                                             
  
                   
                                                          
                                             
                                                                          
                                                                                                             
                                                       
                                                                            
                        
                                                                           
                               
                                                                           
                               
                                                                           
                           
                                                                           
                                      
                                                                           
                                                         
                                                                           
                                                       
                                                                           
                      
                                                                           
                    
  
*/

#ifndef _WLAN_LIB_H
#define _WLAN_LIB_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "CFG_Wifi_File.h"
#include "rlm_domain.h"
#include "wlan_typedef.h"

/*                                                                              
                                                
                                                                                
*/

#define MAX_NUM_GROUP_ADDR                      32	/*                               */



#define TX_CS_TCP_UDP_GEN        BIT(1)
#define TX_CS_IP_GEN             BIT(0)


#define CSUM_OFFLOAD_EN_TX_TCP      BIT(0)
#define CSUM_OFFLOAD_EN_TX_UDP      BIT(1)
#define CSUM_OFFLOAD_EN_TX_IP       BIT(2)
#define CSUM_OFFLOAD_EN_RX_TCP      BIT(3)
#define CSUM_OFFLOAD_EN_RX_UDP      BIT(4)
#define CSUM_OFFLOAD_EN_RX_IPv4     BIT(5)
#define CSUM_OFFLOAD_EN_RX_IPv6     BIT(6)
#define CSUM_OFFLOAD_EN_TX_MASK     BITS(0, 2)
#define CSUM_OFFLOAD_EN_ALL         BITS(0, 6)

/*                       */
#define RX_CS_TYPE_UDP           BIT(7)
#define RX_CS_TYPE_TCP           BIT(6)
#define RX_CS_TYPE_IPv6          BIT(5)
#define RX_CS_TYPE_IPv4          BIT(4)

#define RX_CS_STATUS_UDP         BIT(3)
#define RX_CS_STATUS_TCP         BIT(2)
#define RX_CS_STATUS_IP          BIT(0)

#define CSUM_NOT_SUPPORTED      0x0

#define TXPWR_USE_PDSLOPE 0

/*                              */
#define NVRAM_ERROR_VERSION_MISMATCH        BIT(1)
#define NVRAM_ERROR_INVALID_TXPWR           BIT(2)
#define NVRAM_ERROR_INVALID_DPD             BIT(3)
#define NVRAM_ERROR_INVALID_MAC_ADDR        BIT(4)

#define NUM_TC_RESOURCE_TO_STATISTICS       4

/*                                                                              
                                                 
                                                                                
*/
typedef WLAN_STATUS(*PFN_OID_HANDLER_FUNC) (IN P_ADAPTER_T prAdapter,
					    IN PVOID pvBuf,
					    IN UINT_32 u4BufLen, OUT PUINT_32 pu4OutInfoLen);

typedef enum _ENUM_CSUM_TYPE_T {
	CSUM_TYPE_IPV4,
	CSUM_TYPE_IPV6,
	CSUM_TYPE_TCP,
	CSUM_TYPE_UDP,
	CSUM_TYPE_NUM
} ENUM_CSUM_TYPE_T, *P_ENUM_CSUM_TYPE_T;

typedef enum _ENUM_CSUM_RESULT_T {
	CSUM_RES_NONE,
	CSUM_RES_SUCCESS,
	CSUM_RES_FAILED,
	CSUM_RES_NUM
} ENUM_CSUM_RESULT_T, *P_ENUM_CSUM_RESULT_T;

typedef enum _ENUM_PHY_MODE_T {
	ENUM_PHY_2G4_CCK,
	ENUM_PHY_2G4_OFDM_BPSK,
	ENUM_PHY_2G4_OFDM_QPSK,
	ENUM_PHY_2G4_OFDM_16QAM,
	ENUM_PHY_2G4_OFDM_48M,
	ENUM_PHY_2G4_OFDM_54M,
	ENUM_PHY_2G4_HT20_BPSK,
	ENUM_PHY_2G4_HT20_QPSK,
	ENUM_PHY_2G4_HT20_16QAM,
	ENUM_PHY_2G4_HT20_MCS5,
	ENUM_PHY_2G4_HT20_MCS6,
	ENUM_PHY_2G4_HT20_MCS7,
	ENUM_PHY_2G4_HT40_BPSK,
	ENUM_PHY_2G4_HT40_QPSK,
	ENUM_PHY_2G4_HT40_16QAM,
	ENUM_PHY_2G4_HT40_MCS5,
	ENUM_PHY_2G4_HT40_MCS6,
	ENUM_PHY_2G4_HT40_MCS7,
	ENUM_PHY_5G_OFDM_BPSK,
	ENUM_PHY_5G_OFDM_QPSK,
	ENUM_PHY_5G_OFDM_16QAM,
	ENUM_PHY_5G_OFDM_48M,
	ENUM_PHY_5G_OFDM_54M,
	ENUM_PHY_5G_HT20_BPSK,
	ENUM_PHY_5G_HT20_QPSK,
	ENUM_PHY_5G_HT20_16QAM,
	ENUM_PHY_5G_HT20_MCS5,
	ENUM_PHY_5G_HT20_MCS6,
	ENUM_PHY_5G_HT20_MCS7,
	ENUM_PHY_5G_HT40_BPSK,
	ENUM_PHY_5G_HT40_QPSK,
	ENUM_PHY_5G_HT40_16QAM,
	ENUM_PHY_5G_HT40_MCS5,
	ENUM_PHY_5G_HT40_MCS6,
	ENUM_PHY_5G_HT40_MCS7,
	ENUM_PHY_MODE_NUM
} ENUM_PHY_MODE_T, *P_ENUM_PHY_MODE_T;

typedef enum _ENUM_POWER_SAVE_POLL_MODE_T {
	ENUM_POWER_SAVE_POLL_DISABLE,
	ENUM_POWER_SAVE_POLL_LEGACY_NULL,
	ENUM_POWER_SAVE_POLL_QOS_NULL,
	ENUM_POWER_SAVE_POLL_NUM
} ENUM_POWER_SAVE_POLL_MODE_T, *P_ENUM_POWER_SAVE_POLL_MODE_T;

typedef enum _ENUM_AC_TYPE_T {
	ENUM_AC_TYPE_AC0,
	ENUM_AC_TYPE_AC1,
	ENUM_AC_TYPE_AC2,
	ENUM_AC_TYPE_AC3,
	ENUM_AC_TYPE_AC4,
	ENUM_AC_TYPE_AC5,
	ENUM_AC_TYPE_AC6,
	ENUM_AC_TYPE_BMC,
	ENUM_AC_TYPE_NUM
} ENUM_AC_TYPE_T, *P_ENUM_AC_TYPE_T;

typedef enum _ENUM_ADV_AC_TYPE_T {
	ENUM_ADV_AC_TYPE_RX_NSW,
	ENUM_ADV_AC_TYPE_RX_PTA,
	ENUM_ADV_AC_TYPE_RX_SP,
	ENUM_ADV_AC_TYPE_TX_PTA,
	ENUM_ADV_AC_TYPE_TX_RSP,
	ENUM_ADV_AC_TYPE_NUM
} ENUM_ADV_AC_TYPE_T, *P_ENUM_ADV_AC_TYPE_T;

typedef enum _ENUM_REG_CH_MAP_T {
	REG_CH_MAP_COUNTRY_CODE,
	REG_CH_MAP_TBL_IDX,
	REG_CH_MAP_CUSTOMIZED,
	REG_CH_MAP_NUM
} ENUM_REG_CH_MAP_T, *P_ENUM_REG_CH_MAP_T;

typedef struct _SET_TXPWR_CTRL_T {
	INT_8 c2GLegacyStaPwrOffset;	/*                          */
	INT_8 c2GHotspotPwrOffset;
	INT_8 c2GP2pPwrOffset;
	INT_8 c2GBowPwrOffset;
	INT_8 c5GLegacyStaPwrOffset;	/*                          */
	INT_8 c5GHotspotPwrOffset;
	INT_8 c5GP2pPwrOffset;
	INT_8 c5GBowPwrOffset;
	UINT_8 ucConcurrencePolicy;	/*                                 
                           
                                     
                                     */
	INT_8 acReserved1[3];	/*              */

	/*                                           */
	INT_8 acTxPwrLimit2G[14];	/*                            */
	INT_8 acTxPwrLimit5G[4];	/*          */
	INT_8 acReserved2[2];	/*              */
} SET_TXPWR_CTRL_T, *P_SET_TXPWR_CTRL_T;

/*                                                         */
typedef struct _REG_INFO_T {
	UINT_32 u4SdBlockSize;	/*                 */
	UINT_32 u4SdBusWidth;	/*                        */
	UINT_32 u4SdClockRate;	/*                                  */
	UINT_32 u4StartAddress;	/*                                    */
	UINT_32 u4LoadAddress;	/*                                */
	UINT_16 aucFwImgFilename[65];	/*                   */
	UINT_16 aucFwImgFilenameE6[65];	/*                          */
	UINT_32 u4StartFreq;	/*                                                     */
	UINT_32 u4AdhocMode;	/*                                                            */
	UINT_32 u4RddStartFreq;
	UINT_32 u4RddStopFreq;
	UINT_32 u4RddTestMode;
	UINT_32 u4RddShutFreq;
	UINT_32 u4RddDfs;
	INT_32 i4HighRssiThreshold;
	INT_32 i4MediumRssiThreshold;
	INT_32 i4LowRssiThreshold;
	INT_32 au4TxPriorityTag[ENUM_AC_TYPE_NUM];
	INT_32 au4RxPriorityTag[ENUM_AC_TYPE_NUM];
	INT_32 au4AdvPriorityTag[ENUM_ADV_AC_TYPE_NUM];
	UINT_32 u4FastPSPoll;
	UINT_32 u4PTA;		/*                       */
	UINT_32 u4TXLimit;	/*                       */
	UINT_32 u4SilenceWindow;	/*                            */
	UINT_32 u4TXLimitThreshold;	/*                             */
	UINT_32 u4PowerMode;
	UINT_32 fgEnArpFilter;
	UINT_32 u4PsCurrentMeasureEn;
	UINT_32 u4UapsdAcBmp;
	UINT_32 u4MaxSpLen;
	UINT_32 fgDisOnlineScan;	/*                                                      */
	UINT_32 fgDisBcnLostDetection;	/*                                                      */
	UINT_32 u4FixedRate;	/*                                    */
	UINT_32 u4ArSysParam0;
	UINT_32 u4ArSysParam1;
	UINT_32 u4ArSysParam2;
	UINT_32 u4ArSysParam3;
	UINT_32 fgDisRoaming;	/*                            */

	/*                         */
	UINT_8 aucMacAddr[6];
	UINT_16 au2CountryCode[4];	/*                                                          */
	TX_PWR_PARAM_T rTxPwr;
	UINT_8 aucEFUSE[144];
	UINT_8 ucTxPwrValid;
	UINT_8 ucSupport5GBand;
	UINT_8 fg2G4BandEdgePwrUsed;
	INT_8 cBandEdgeMaxPwrCCK;
	INT_8 cBandEdgeMaxPwrOFDM20;
	INT_8 cBandEdgeMaxPwrOFDM40;
	ENUM_REG_CH_MAP_T eRegChannelListMap;
	UINT_8 ucRegChannelListIndex;
	DOMAIN_INFO_ENTRY rDomainInfo;
	/*                       */

	/*                                 */
	UINT_8 uc2G4BwFixed20M;
	UINT_8 uc5GBwFixed20M;
	UINT_8 ucEnable5GBand;
	/*                               */

} REG_INFO_T, *P_REG_INFO_T;

/*                              */
typedef struct _FWDL_SECTION_INFO_T {
	UINT_32 u4Offset;
	UINT_32 u4Reserved;
	UINT_32 u4Length;
	UINT_32 u4DestAddr;
} FWDL_SECTION_INFO_T, *P_FWDL_SECTION_INFO_T;

typedef struct _FIRMWARE_DIVIDED_DOWNLOAD_T {
	UINT_32 u4Signature;
	UINT_32 u4CRC;		/*                                               */
	UINT_32 u4NumOfEntries;
	UINT_32 u4Reserved;
	FWDL_SECTION_INFO_T arSection[];
} FIRMWARE_DIVIDED_DOWNLOAD_T, *P_FIRMWARE_DIVIDED_DOWNLOAD_T;

typedef struct _PARAM_MCR_RW_STRUC_T {
	UINT_32 u4McrOffset;
	UINT_32 u4McrData;
} PARAM_MCR_RW_STRUC_T, *P_PARAM_MCR_RW_STRUC_T;

typedef struct _PARAM_GET_STA_STATISTICS {
	/*                   */
	UINT_8 aucMacAddr[MAC_ADDR_LEN];

	UINT_32 u4Flag;

	/*             */
	UINT_32 u4TxTotalCount;
	UINT_32 u4TxExceedThresholdCount;

	UINT_32 u4TxMaxTime;
	UINT_32 u4TxAverageProcessTime;

	UINT_32 au4TcResourceEmptyCount[NUM_TC_RESOURCE_TO_STATISTICS];
	UINT_32 au4TcQueLen[NUM_TC_RESOURCE_TO_STATISTICS];

	/*         */
	UINT_8 ucPer;		/*           */
	UINT_8 ucRcpi;
	UINT_32 u4PhyMode;
	UINT_16 u2LinkSpeed;	/*                   */

	UINT_32 u4TxFailCount;
	UINT_32 u4TxLifeTimeoutCount;

	UINT_32 u4TxAverageAirTime;

	/*                                   */
	UINT_32 au4TcAverageQueLen[NUM_TC_RESOURCE_TO_STATISTICS];
	UINT_32 au4TcCurrentQueLen[NUM_TC_RESOURCE_TO_STATISTICS];

	/*                 */
	UINT_8 au4Reserved[32];
} PARAM_GET_STA_STA_STATISTICS, *P_PARAM_GET_STA_STATISTICS;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define BUILD_SIGN(ch0, ch1, ch2, ch3) \
    ((UINT_32)(UINT_8)(ch0) | ((UINT_32)(UINT_8)(ch1) << 8) |   \
     ((UINT_32)(UINT_8)(ch2) << 16) | ((UINT_32)(UINT_8)(ch3) << 24))

#define MTK_WIFI_SIGNATURE BUILD_SIGN('M', 'T', 'K', 'W')

/*                                                                              
                                                             
                                                                                
*/

P_ADAPTER_T wlanAdapterCreate(IN P_GLUE_INFO_T prGlueInfo);

VOID wlanAdapterDestroy(IN P_ADAPTER_T prAdapter);

VOID wlanCardEjected(IN P_ADAPTER_T prAdapter);

VOID wlanIST(IN P_ADAPTER_T prAdapter);

BOOL wlanISR(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgGlobalIntrCtrl);

WLAN_STATUS wlanProcessCommandQueue(IN P_ADAPTER_T prAdapter, IN P_QUE_T prCmdQue);

WLAN_STATUS wlanSendCommand(IN P_ADAPTER_T prAdapter, IN P_CMD_INFO_T prCmdInfo);

VOID wlanReleaseCommand(IN P_ADAPTER_T prAdapter, IN P_CMD_INFO_T prCmdInfo);

VOID wlanReleasePendingOid(IN P_ADAPTER_T prAdapter, IN UINT_32 u4Data);

VOID
wlanReleasePendingCMDbyNetwork(IN P_ADAPTER_T prAdapter, IN ENUM_NETWORK_TYPE_INDEX_T eNetworkType);

VOID wlanReturnPacket(IN P_ADAPTER_T prAdapter, IN PVOID pvPacket);

WLAN_STATUS
wlanQueryInformation(IN P_ADAPTER_T prAdapter,
		     IN PFN_OID_HANDLER_FUNC pfOidQryHandler,
		     IN PVOID pvInfoBuf, IN UINT_32 u4InfoBufLen, OUT PUINT_32 pu4QryInfoLen);

WLAN_STATUS
wlanSetInformation(IN P_ADAPTER_T prAdapter,
		   IN PFN_OID_HANDLER_FUNC pfOidSetHandler,
		   IN PVOID pvInfoBuf, IN UINT_32 u4InfoBufLen, OUT PUINT_32 pu4SetInfoLen);


WLAN_STATUS
wlanAdapterStart(IN P_ADAPTER_T prAdapter,
		 IN P_REG_INFO_T prRegInfo,
		 IN PVOID pvFwImageMapFile, IN UINT_32 u4FwImageFileLength);

WLAN_STATUS wlanAdapterStop(IN P_ADAPTER_T prAdapter);

#if CFG_SUPPORT_WAPI
BOOLEAN wlanQueryWapiMode(IN P_ADAPTER_T prAdapter);
#endif

VOID wlanReturnRxPacket(IN PVOID pvAdapter, IN PVOID pvPacket);

VOID wlanRxSetBroadcast(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgEnableBroadcast);

BOOLEAN wlanIsHandlerNeedHwAccess(IN PFN_OID_HANDLER_FUNC pfnOidHandler, IN BOOLEAN fgSetInfo);

VOID wlanSetPromiscuousMode(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgEnablePromiscuousMode);

#if CFG_ENABLE_FW_DOWNLOAD
#if CFG_ENABLE_FW_DOWNLOAD_AGGREGATION
WLAN_STATUS
wlanImageSectionDownloadAggregated(IN P_ADAPTER_T prAdapter,
				   IN UINT_32 u4DestAddr,
				   IN UINT_32 u4ImgSecSize, IN PUINT_8 pucImgSecBuf);
#endif

WLAN_STATUS
wlanImageSectionDownload(IN P_ADAPTER_T prAdapter,
			 IN UINT_32 u4DestAddr, IN UINT_32 u4ImgSecSize, IN PUINT_8 pucImgSecBuf);

#if !CFG_ENABLE_FW_DOWNLOAD_ACK
WLAN_STATUS wlanImageQueryStatus(IN P_ADAPTER_T prAdapter);
#else
WLAN_STATUS wlanImageSectionDownloadStatus(IN P_ADAPTER_T prAdapter, IN UINT_8 ucCmdSeqNum);
#endif

WLAN_STATUS
wlanConfigWifiFunc(IN P_ADAPTER_T prAdapter, IN BOOLEAN fgEnable, IN UINT_32 u4StartAddress);

UINT_32 wlanCRC32(PUINT_8 buf, UINT_32 len);

#endif

WLAN_STATUS wlanSendNicPowerCtrlCmd(IN P_ADAPTER_T prAdapter, IN UINT_8 ucPowerMode);

BOOLEAN wlanIsHandlerAllowedInRFTest(IN PFN_OID_HANDLER_FUNC pfnOidHandler, IN BOOLEAN fgSetInfo);

WLAN_STATUS wlanProcessQueuedSwRfb(IN P_ADAPTER_T prAdapter, IN P_SW_RFB_T prSwRfbListHead);

WLAN_STATUS
wlanProcessQueuedMsduInfo(IN P_ADAPTER_T prAdapter, IN P_MSDU_INFO_T prMsduInfoListHead);

BOOLEAN wlanoidTimeoutCheck(IN P_ADAPTER_T prAdapter, IN PFN_OID_HANDLER_FUNC pfnOidHandler);

VOID wlanoidClearTimeoutCheck(IN P_ADAPTER_T prAdapter);

WLAN_STATUS wlanUpdateNetworkAddress(IN P_ADAPTER_T prAdapter);

BOOLEAN wlanQueryTestMode(IN P_ADAPTER_T prAdapter);

/*                         */
BOOLEAN wlanProcessSecurityFrame(IN P_ADAPTER_T prAdapter, IN P_NATIVE_PACKET prPacket);

VOID
wlanSecurityFrameTxDone(IN P_ADAPTER_T prAdapter,
			IN P_CMD_INFO_T prCmdInfo, IN PUINT_8 pucEventBuf);

VOID wlanSecurityFrameTxTimeout(IN P_ADAPTER_T prAdapter, IN P_CMD_INFO_T prCmdInfo);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID wlanClearScanningResult(IN P_ADAPTER_T prAdapter);

VOID wlanClearBssInScanningResult(IN P_ADAPTER_T prAdapter, IN PUINT_8 arBSSID);

#if CFG_TEST_WIFI_DIRECT_GO
VOID wlanEnableP2pFunction(IN P_ADAPTER_T prAdapter);

VOID wlanEnableATGO(IN P_ADAPTER_T prAdapter);
#endif


/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS wlanQueryPermanentAddress(IN P_ADAPTER_T prAdapter);


/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS wlanQueryNicCapability(IN P_ADAPTER_T prAdapter);


/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS wlanQueryCompileFlags(IN P_ADAPTER_T prAdapter);

/*                                                                            */
/*                                                                    */
/*                                                                            */
WLAN_STATUS wlanQueryPdMcr(IN P_ADAPTER_T prAdapter, IN P_PARAM_MCR_RW_STRUC_T prMcrRdInfo);
/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS wlanLoadManufactureData(IN P_ADAPTER_T prAdapter, IN P_REG_INFO_T prRegInfo);


/*                                                                            */
/*                                                                            */
/*                                                                            */
BOOLEAN wlanResetMediaStreamMode(IN P_ADAPTER_T prAdapter);


/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS wlanTimerTimeoutCheck(IN P_ADAPTER_T prAdapter);


/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS wlanProcessMboxMessage(IN P_ADAPTER_T prAdapter);


/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS wlanEnqueueTxPacket(IN P_ADAPTER_T prAdapter, IN P_NATIVE_PACKET prNativePacket);

WLAN_STATUS wlanFlushTxPendingPackets(IN P_ADAPTER_T prAdapter);

WLAN_STATUS wlanTxPendingPackets(IN P_ADAPTER_T prAdapter, IN OUT PBOOLEAN pfgHwAccess);


/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS wlanAcquirePowerControl(IN P_ADAPTER_T prAdapter);

WLAN_STATUS wlanReleasePowerControl(IN P_ADAPTER_T prAdapter);


/*                                                                            */
/*                                                                            */
/*                                                                            */
UINT_32 wlanGetTxPendingFrameCount(IN P_ADAPTER_T prAdapter);


/*                                                                            */
/*                                                                            */
/*                                                                            */
ENUM_ACPI_STATE_T wlanGetAcpiState(IN P_ADAPTER_T prAdapter);

VOID wlanSetAcpiState(IN P_ADAPTER_T prAdapter, IN ENUM_ACPI_STATE_T ePowerState);

#if CFG_SUPPORT_OSC_SETTING && defined(MT5931)
WLAN_STATUS wlanSetMcuOscStableTime(IN P_ADAPTER_T prAdapter, IN UINT_16 u2OscStableTime);
#endif

VOID wlanDefTxPowerCfg(IN P_ADAPTER_T prAdapter);

/*                                                                            */
/*                                                                            */
/*                                                                            */
UINT_8 wlanGetEcoVersion(IN P_ADAPTER_T prAdapter);

/*                                                                            */
/*                                                                            */
/*                                                                            */
VOID
wlanSetPreferBandByNetwork(IN P_ADAPTER_T prAdapter,
			   IN ENUM_BAND_T eBand, IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex);

/*                                                                            */
/*                                                                            */
/*                                                                            */
UINT_8
wlanGetChannelNumberByNetwork(IN P_ADAPTER_T prAdapter, IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex);

/*                                                                            */
/*                                                                            */
/*                                                                            */
P_BSS_DESC_T
wlanGetTargetBssDescByNetwork(IN P_ADAPTER_T prAdapter, IN ENUM_NETWORK_TYPE_INDEX_T eNetTypeIndex);

/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS wlanCheckSystemConfiguration(IN P_ADAPTER_T prAdapter);

/*                                                                            */
/*                                                                            */
/*                                                                            */
WLAN_STATUS
wlanoidQueryStaStatistics(IN P_ADAPTER_T prAdapter,
			  IN PVOID pvQueryBuffer,
			  IN UINT_32 u4QueryBufferLen, OUT PUINT_32 pu4QueryInfoLen);

#endif				/*             */
