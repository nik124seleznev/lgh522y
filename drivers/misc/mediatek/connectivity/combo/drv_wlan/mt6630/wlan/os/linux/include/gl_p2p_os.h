/*
       
                                                                                    
*/

/*                     
                                                                 

                                                                                  
                                                                            
                                                                                  
                        
*/





#ifndef _GL_P2P_OS_H
#define _GL_P2P_OS_H


/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                      
                                                                                
*/
#if CFG_ENABLE_WIFI_DIRECT && CFG_ENABLE_WIFI_DIRECT_CFG_80211
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32)
#if  LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31)
extern const struct net_device_ops p2p_netdev_ops;
#endif
#endif
#endif

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


struct _GL_P2P_INFO_T {

	/*                             */
	struct net_device *prDevHandler;

	struct net_device *aprRoleHandler[KAL_P2P_NUM];

	UINT_8 ucRoleInterfaceNum;

#if CFG_ENABLE_WIFI_DIRECT_CFG_80211
	/*          */
	struct wireless_dev wdev;

	struct cfg80211_scan_request *prScanRequest;

	UINT_64 u8Cookie;

	/*                                     */
	INT_32 i4Generation;

	UINT_32 u4OsMgmtFrameFilter;

#endif

	/*                   */
	struct net_device_stats rNetDevStats;

	/*                      */
	/*                          */
	/*                                             */
	UINT_32 u4FreqInKHz;	/*           */
	UINT_8 ucRole;		/*                                                */
	UINT_8 ucIntent;	/*             */
	UINT_8 ucScanMode;	/*                                                    */

	ENUM_PARAM_MEDIA_STATE_T eState;
	UINT_32 u4PacketFilter;
	PARAM_MAC_ADDRESS aucMCAddrList[MAX_NUM_GROUP_ADDR];

	/*                                       */
	UINT_8 aucConnReqDevName[32];
	INT_32 u4ConnReqNameLength;
	PARAM_MAC_ADDRESS rConnReqPeerAddr;
	PARAM_MAC_ADDRESS rConnReqGroupAddr;	/*                       */
	UINT_8 ucConnReqDevType;
	INT_32 i4ConnReqConfigMethod;
	INT_32 i4ConnReqActiveConfigMethod;

	UINT_32 u4CipherPairwise;
	UINT_8 ucWSCRunning;

	UINT_8 aucWSCIE[3][400];	/*                                                     */
	UINT_16 u2WSCIELen[3];

#if CFG_SUPPORT_WFD
	UINT_8 aucWFDIE[400];	/*                                                     */
	UINT_16 u2WFDIELen;
/*                                                */ /*                                  */
/*                                            */
#endif

	UINT_8 ucOperatingChnl;
	UINT_8 ucInvitationType;

	UINT_32 u4InvStatus;

	/*                           */
	UINT_8 aucOidBuf[4096];

#if 1				/*                         */
	UINT_8 aucSecCheck[256];
	UINT_8 aucSecCheckRsp[256];
#endif

#if CFG_SUPPORT_HOTSPOT_WPS_MANAGER
	/*                           */
	PARAM_MAC_ADDRESS aucblackMACList[10]; //                                                                                                                            
	UINT_8 ucMaxClients;
#endif

#if CFG_SUPPORT_HOTSPOT_OPTIMIZATION
	BOOLEAN fgEnableHotspotOptimization;
	UINT_32 u4PsLevel;
#endif
};


#if CONFIG_NL80211_TESTMODE
typedef struct _NL80211_DRIVER_TEST_PRE_PARAMS {
	UINT_16 idx_mode;
	UINT_16 idx;
	UINT_32 value;
} NL80211_DRIVER_TEST_PRE_PARAMS, *P_NL80211_DRIVER_TEST_PRE_PARAMS;


typedef struct _NL80211_DRIVER_TEST_PARAMS {
	UINT_32 index;
	UINT_32 buflen;
} NL80211_DRIVER_TEST_PARAMS, *P_NL80211_DRIVER_TEST_PARAMS;


/*          */
typedef struct _NL80211_DRIVER_P2P_SIGMA_PARAMS {
	NL80211_DRIVER_TEST_PARAMS hdr;
	UINT_32 idx;
	UINT_32 value;
} NL80211_DRIVER_P2P_SIGMA_PARAMS, *P_NL80211_DRIVER_P2P_SIGMA_PARAMS;


/*                           */
typedef struct _NL80211_DRIVER_hotspot_block_PARAMS {
	NL80211_DRIVER_TEST_PARAMS hdr;
	UINT_8 ucblocked;
	UINT_8 aucBssid[MAC_ADDR_LEN];
} NL80211_DRIVER_hotspot_block_PARAMS, *P_NL80211_DRIVER_hotspot_block_PARAMS;

#if CFG_SUPPORT_WFD
typedef struct _NL80211_DRIVER_WFD_PARAMS {
	NL80211_DRIVER_TEST_PARAMS hdr;
	UINT_32 WfdCmdType;
	UINT_8 WfdEnable;
	UINT_8 WfdCoupleSinkStatus;
	UINT_8 WfdSessionAvailable;
	UINT_8 WfdSigmaMode;
	UINT_16 WfdDevInfo;
	UINT_16 WfdControlPort;
	UINT_16 WfdMaximumTp;
	UINT_16 WfdExtendCap;
	UINT_8 WfdCoupleSinkAddress[MAC_ADDR_LEN];
	UINT_8 WfdAssociatedBssid[MAC_ADDR_LEN];
	UINT_8 WfdVideoIp[4];
	UINT_8 WfdAudioIp[4];
	UINT_16 WfdVideoPort;
	UINT_16 WfdAudioPort;
	UINT_32 WfdFlag;
	UINT_32 WfdPolicy;
	UINT_32 WfdState;
	UINT_8 WfdSessionInformationIE[24 * 8];	/*                               */
	UINT_16 WfdSessionInformationIELen;
	UINT_8 aucReserved1[2];
	UINT_8 aucWfdPrimarySinkMac[MAC_ADDR_LEN];
	UINT_8 aucWfdSecondarySinkMac[MAC_ADDR_LEN];
	UINT_32 WfdAdvanceFlag;
	/*                  */
	UINT_8 aucWfdLocalIp[4];
	UINT_16 WfdLifetimeAc2;	/*              */
	UINT_16 WfdLifetimeAc3;	/*              */
	UINT_16 WfdCounterThreshold;	/*            */
	UINT_8 aucReserved2[54];
	/*                  */
	UINT_8 aucReserved3[64];
	/*                  */
	UINT_8 aucReserved4[64];
} NL80211_DRIVER_WFD_PARAMS, *P_NL80211_DRIVER_WFD_PARAMS;
#endif
#endif

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

BOOLEAN p2pRegisterToWlan(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pUnregisterToWlan(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pLaunch(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pRemove(P_GLUE_INFO_T prGlueInfo);

VOID p2pSetMode(IN BOOLEAN fgIsAPMOde);


BOOLEAN glRegisterP2P(P_GLUE_INFO_T prGlueInfo, const char *prDevName, BOOLEAN fgIsApMode);

BOOLEAN glUnregisterP2P(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pNetRegister(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgIsRtnlLockAcquired);

BOOLEAN p2pNetUnregister(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgIsRtnlLockAcquired);

BOOLEAN p2PFreeInfo(P_GLUE_INFO_T prGlueInfo);

VOID p2pSetSuspendMode(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgEnable);

#endif
