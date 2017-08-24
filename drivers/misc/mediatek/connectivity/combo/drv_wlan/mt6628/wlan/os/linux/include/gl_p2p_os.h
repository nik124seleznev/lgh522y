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

	/*               */
	struct net_device *prDevHandler;

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
	UINT_8 aucVenderIE[1024];	/*                                  */
	UINT_16 u2VenderIELen;
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

	/*                           */
	PARAM_MAC_ADDRESS aucblackMACList[8];
	UINT_8 ucMaxClients;

};


BOOLEAN p2pRegisterToWlan(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pUnregisterToWlan(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pLaunch(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pRemove(P_GLUE_INFO_T prGlueInfo);

VOID p2pSetMode(IN BOOLEAN fgIsAPMOde);


BOOLEAN glRegisterP2P(P_GLUE_INFO_T prGlueInfo, const char *prDevName, BOOLEAN fgIsApMode);

BOOLEAN glUnregisterP2P(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2pNetRegister(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgIsRtnlLockAcquired);

BOOLEAN p2pNetUnregister(P_GLUE_INFO_T prGlueInfo, BOOLEAN fgIsRtnlLockAcquired);

BOOLEAN p2pStopImmediate(P_GLUE_INFO_T prGlueInfo);

BOOLEAN p2PFreeInfo(P_GLUE_INFO_T prGlueInfo);


#endif
