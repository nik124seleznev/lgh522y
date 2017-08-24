/*
                                
*/

/*                      
          

*/




/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "net/cfg80211.h"
#include "precomp.h"
#include "gl_wext.h"

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
BOOLEAN
kalP2pFuncGetChannelType(IN ENUM_CHNL_EXT_T rChnlSco, OUT enum nl80211_channel_type *channel_type);


struct ieee80211_channel *kalP2pFuncGetChannelEntry(IN P_GL_P2P_INFO_T prP2pInfo,
						    IN P_RF_CHANNEL_INFO_T prChannelInfo);


/*                                                                              
                                                
                                                                                
*/

/*                                                                            */
/* 
                                                       
 
            
                      
                     
         
                                 
*/
/*                                                                            */
ENUM_PARAM_MEDIA_STATE_T kalP2PGetState(IN P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);

	return prGlueInfo->prP2PInfo->eState;
}				/*                         */


/*                                                                            */
/* 
                                       
 
            
                      
                        
                          
                            
         
                
*/
/*                                                                            */
VOID
kalP2PUpdateAssocInfo(IN P_GLUE_INFO_T prGlueInfo,
		      IN PUINT_8 pucFrameBody,
		      IN UINT_32 u4FrameBodyLen, IN BOOLEAN fgReassocRequest)
{
	union iwreq_data wrqu;
	unsigned char *pucExtraInfo = NULL;
	unsigned char *pucDesiredIE = NULL;
/*                                     */
	PUINT_8 cp;

	memset(&wrqu, 0, sizeof(wrqu));

	if (fgReassocRequest) {
		if (u4FrameBodyLen < 15) {
			/*
                                                                        
    */
			return;
		}
	} else {
		if (u4FrameBodyLen < 9) {
			/*
                                                                        
    */
			return;
		}
	}

	cp = pucFrameBody;

	if (fgReassocRequest) {
		/*                                */
		/*                         */
		/*                      */
		cp += 10;
		u4FrameBodyLen -= 10;
	} else {
		/*                                */
		/*                         */
		cp += 4;
		u4FrameBodyLen -= 4;
	}

	/*                                                         */
	if (wextSrchDesiredWPSIE(cp, u4FrameBodyLen, 0xDD, &pucDesiredIE)) {
		/*                                     */
		/*              */
	} else if (wextSrchDesiredWPAIE(cp, u4FrameBodyLen, 0x30, &pucDesiredIE)) {
		/*                                     */
		/*              */
	} else if (wextSrchDesiredWPAIE(cp, u4FrameBodyLen, 0xDD, &pucDesiredIE)) {
		/*                                     */
		/*              */
	} else {
		/*                                      */
		goto skip_indicate_event;
	}

	/*                                        */
	pucExtraInfo = pucDesiredIE;
	wrqu.data.length = pucDesiredIE[1] + 2;

	/*                          */
	wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler, IWEVASSOCREQIE, &wrqu,
			    pucExtraInfo);

skip_indicate_event:
	return;

}


/*                                                                            */
/* 
                                                
 
            
                      
                     
                     
         
                
*/
/*                                                                            */
VOID
kalP2PSetState(IN P_GLUE_INFO_T prGlueInfo,
	       IN ENUM_PARAM_MEDIA_STATE_T eState, IN PARAM_MAC_ADDRESS rPeerAddr, IN UINT_8 ucRole)
{
	union iwreq_data evt;
	UINT_8 aucBuffer[IW_CUSTOM_MAX];

	ASSERT(prGlueInfo);

	memset(&evt, 0, sizeof(evt));

	if (eState == PARAM_MEDIA_STATE_CONNECTED) {
		prGlueInfo->prP2PInfo->eState = PARAM_MEDIA_STATE_CONNECTED;

		snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_STA_CONNECT=" MACSTR,
			 MAC2STR(rPeerAddr));
		evt.data.length = strlen(aucBuffer);

		/*                             */
		wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler,
				    IWEVCUSTOM, &evt, aucBuffer);

	} else if (eState == PARAM_MEDIA_STATE_DISCONNECTED) {
		snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_STA_DISCONNECT=" MACSTR,
			 MAC2STR(rPeerAddr));
		evt.data.length = strlen(aucBuffer);

		/*                             */
		wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler,
				    IWEVCUSTOM, &evt, aucBuffer);
	} else {
		ASSERT(0);
	}

	return;
}				/*                         */


/*                                                                            */
/* 
                                                     
 
            
                      
 
         
                          
*/
/*                                                                            */
UINT_32 kalP2PGetFreqInKHz(IN P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);

	return prGlueInfo->prP2PInfo->u4FreqInKHz;
}				/*                             */


/*                                                                            */
/* 
                                              
 
            
                      
 
         
                         
                           
                          
*/
/*                                                                            */
UINT_8 kalP2PGetRole(IN P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);

	return prGlueInfo->prP2PInfo->ucRole;
}				/*                        */


/*                                                                            */
/* 
                                 
 
            
                      
                    
                                 
                            
                  
                                 
                                   
                                  
 
         
                
*/
/*                                                                            */
#if 1
VOID kalP2PSetRole(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucRole)
{
	ASSERT(prGlueInfo);
	ASSERT(ucRole <= 2);

	prGlueInfo->prP2PInfo->ucRole = ucRole;
	/*                      */
	return;
}				/*                        */

#else
VOID
kalP2PSetRole(IN P_GLUE_INFO_T prGlueInfo,
	      IN UINT_8 ucResult, IN PUINT_8 pucSSID, IN UINT_8 ucSSIDLen, IN UINT_8 ucRole)
{
	union iwreq_data evt;
	UINT_8 aucBuffer[IW_CUSTOM_MAX];

	ASSERT(prGlueInfo);
	ASSERT(ucRole <= 2);

	memset(&evt, 0, sizeof(evt));

	if (ucResult == 0) {
		prGlueInfo->prP2PInfo->ucRole = ucRole;
	}

	if (pucSSID)
		snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_FORMATION_RST=%d%d%d%c%c", ucResult,
			 ucRole, 1 /*                    */ , pucSSID[7], pucSSID[8]);
	else
		snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_FORMATION_RST=%d%d%d%c%c", ucResult,
			 ucRole, 1 /*                    */ , '0', '0');

	evt.data.length = strlen(aucBuffer);

	/*              */
	/*                                                              */

	/*                             */
	wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler, IWEVCUSTOM, &evt, aucBuffer);

	return;
}				/*                        */

#endif
/*                                                                            */
/* 
                                  
 
            
                      
                    
 
         
                
*/
/*                                                                            */
VOID kalP2PSetCipher(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Cipher)
{
	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	prGlueInfo->prP2PInfo->u4CipherPairwise = u4Cipher;

	return;
}


/*                                                                            */
/* 
                                                     
 
            
                      
 
         
                                
                                 
*/
/*                                                                            */
BOOLEAN kalP2PGetCipher(IN P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	if (prGlueInfo->prP2PInfo->u4CipherPairwise == IW_AUTH_CIPHER_CCMP)
		return TRUE;

	if (prGlueInfo->prP2PInfo->u4CipherPairwise == IW_AUTH_CIPHER_TKIP)
		return TRUE;

	if (prGlueInfo->prP2PInfo->u4CipherPairwise == IW_AUTH_CIPHER_WEP40)
		return TRUE;

	return FALSE;
}

BOOLEAN kalP2PGetCcmpCipher(IN P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	if (prGlueInfo->prP2PInfo->u4CipherPairwise == IW_AUTH_CIPHER_CCMP)
		return TRUE;

	if (prGlueInfo->prP2PInfo->u4CipherPairwise == IW_AUTH_CIPHER_TKIP)
		return FALSE;

	return FALSE;
}


BOOLEAN kalP2PGetTkipCipher(IN P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	if (prGlueInfo->prP2PInfo->u4CipherPairwise == IW_AUTH_CIPHER_CCMP)
		return FALSE;

	if (prGlueInfo->prP2PInfo->u4CipherPairwise == IW_AUTH_CIPHER_TKIP)
		return TRUE;

	return FALSE;
}

/*                                                                            */
/* 
                                 
 
            
                      
 
         
*/
/*                                                                            */
VOID kalP2PSetWscMode(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucWscMode)
{
	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	prGlueInfo->prP2PInfo->ucWSCRunning = ucWscMode;
}


/*                                                                            */
/* 
                                 
 
            
                      
 
         
*/
/*                                                                            */
UINT_8 kalP2PGetWscMode(IN P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	return prGlueInfo->prP2PInfo->ucWSCRunning;
}


/*                                                                            */
/* 
                                 
 
            
                      
                                                                    
 
         
                             
*/
/*                                                                            */
UINT_16 kalP2PCalWSC_IELen(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucType)
{
	ASSERT(prGlueInfo);

	ASSERT(ucType < 3);

	return prGlueInfo->prP2PInfo->u2WSCIELen[ucType];
}


/*                                                                            */
/* 
                                                       
 
            
                      
 
         
                             
*/
/*                                                                            */
VOID kalP2PGenWSC_IE(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucType, IN PUINT_8 pucBuffer)
{
	P_GL_P2P_INFO_T prGlP2pInfo = (P_GL_P2P_INFO_T) NULL;

	do {
		if ((prGlueInfo == NULL) || (ucType >= 3) || (pucBuffer == NULL)) {
			break;
		}


		prGlP2pInfo = prGlueInfo->prP2PInfo;

		kalMemCopy(pucBuffer, prGlP2pInfo->aucWSCIE[ucType],
			   prGlP2pInfo->u2WSCIELen[ucType]);

	} while (FALSE);

	return;
}


VOID
kalP2PUpdateWSC_IE(IN P_GLUE_INFO_T prGlueInfo,
		   IN UINT_8 ucType, IN PUINT_8 pucBuffer, IN UINT_16 u2BufferLength)
{
	P_GL_P2P_INFO_T prGlP2pInfo = (P_GL_P2P_INFO_T) NULL;

	do {
		if ((prGlueInfo == NULL) ||
		    (ucType >= 3) || ((u2BufferLength > 0) && (pucBuffer == NULL))) {
			break;
		}


		if (u2BufferLength > 400) {
			DBGLOG(P2P, ERROR,
			       ("Buffer length is not enough, GLUE only 400 bytes but %d received\n",
				u2BufferLength));
			ASSERT(FALSE);
			break;
		}


		prGlP2pInfo = prGlueInfo->prP2PInfo;

		kalMemCopy(prGlP2pInfo->aucWSCIE[ucType], pucBuffer, u2BufferLength);

		prGlP2pInfo->u2WSCIELen[ucType] = u2BufferLength;


	} while (FALSE);

	return;
}				/*                    */



/*                                                                            */
/* 
                                                                      
 
                                              
 
              
*/
/*                                                                            */
VOID kalP2PIndicateConnReq(IN P_GLUE_INFO_T prGlueInfo, IN PUINT_8 pucDevName, IN INT_32 u4NameLength, IN PARAM_MAC_ADDRESS rPeerAddr, IN UINT_8 ucDevType,	/*                               */
			   IN INT_32 i4ConfigMethod, IN INT_32 i4ActiveConfigMethod)
{
	union iwreq_data evt;
	UINT_8 aucBuffer[IW_CUSTOM_MAX];

	ASSERT(prGlueInfo);

	/*                                                                      */
	prGlueInfo->prP2PInfo->u4ConnReqNameLength = u4NameLength > 32 ? 32 : u4NameLength;
	kalMemCopy(prGlueInfo->prP2PInfo->aucConnReqDevName,
		   pucDevName, prGlueInfo->prP2PInfo->u4ConnReqNameLength);
	COPY_MAC_ADDR(prGlueInfo->prP2PInfo->rConnReqPeerAddr, rPeerAddr);
	prGlueInfo->prP2PInfo->ucConnReqDevType = ucDevType;
	prGlueInfo->prP2PInfo->i4ConnReqConfigMethod = i4ConfigMethod;
	prGlueInfo->prP2PInfo->i4ConnReqActiveConfigMethod = i4ActiveConfigMethod;

	/*                         */
	memset(&evt, 0, sizeof(evt));

	snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_DVC_REQ");
	evt.data.length = strlen(aucBuffer);

	/*                              */
	wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler, IWEVCUSTOM, &evt, aucBuffer);

	return;
}				/*                                */


/*                                                                            */
/* 
                                                                                         
 
                                              
                                                                        
 
              
*/
/*                                                                            */
VOID
kalP2PInvitationIndication(IN P_GLUE_INFO_T prGlueInfo,
			   IN P_P2P_DEVICE_DESC_T prP2pDevDesc,
			   IN PUINT_8 pucSsid,
			   IN UINT_8 ucSsidLen,
			   IN UINT_8 ucOperatingChnl,
			   IN UINT_8 ucInvitationType, IN PUINT_8 pucGroupBssid)
{
#if 1
	union iwreq_data evt;
	UINT_8 aucBuffer[IW_CUSTOM_MAX];

	ASSERT(prGlueInfo);

	/*                                                             */
	prGlueInfo->prP2PInfo->u4ConnReqNameLength =
	    (UINT_32) ((prP2pDevDesc->u2NameLength > 32) ? 32 : prP2pDevDesc->u2NameLength);
	kalMemCopy(prGlueInfo->prP2PInfo->aucConnReqDevName, prP2pDevDesc->aucName,
		   prGlueInfo->prP2PInfo->u4ConnReqNameLength);
	COPY_MAC_ADDR(prGlueInfo->prP2PInfo->rConnReqPeerAddr, prP2pDevDesc->aucDeviceAddr);
	COPY_MAC_ADDR(prGlueInfo->prP2PInfo->rConnReqGroupAddr, pucGroupBssid);
	prGlueInfo->prP2PInfo->i4ConnReqConfigMethod = (INT_32) (prP2pDevDesc->u2ConfigMethod);
	prGlueInfo->prP2PInfo->ucOperatingChnl = ucOperatingChnl;
	prGlueInfo->prP2PInfo->ucInvitationType = ucInvitationType;

	/*                         */
	memset(&evt, 0, sizeof(evt));

	snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_INV_INDICATE");
	evt.data.length = strlen(aucBuffer);

	/*                              */
	wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler, IWEVCUSTOM, &evt, aucBuffer);
	return;

#else
	P_MSG_P2P_CONNECTION_REQUEST_T prP2pConnReq = (P_MSG_P2P_CONNECTION_REQUEST_T) NULL;
	P_P2P_SPECIFIC_BSS_INFO_T prP2pSpecificBssInfo = (P_P2P_SPECIFIC_BSS_INFO_T) NULL;
	P_P2P_CONNECTION_SETTINGS_T prP2pConnSettings = (P_P2P_CONNECTION_SETTINGS_T) NULL;

	do {
		ASSERT_BREAK((prGlueInfo != NULL) && (prP2pDevDesc != NULL));


		/*                     */

		prP2pSpecificBssInfo = prGlueInfo->prAdapter->rWifiVar.prP2pSpecificBssInfo;
		prP2pConnSettings = prGlueInfo->prAdapter->rWifiVar.prP2PConnSettings;

		prP2pConnReq = (P_MSG_P2P_CONNECTION_REQUEST_T) cnmMemAlloc(prGlueInfo->prAdapter,
									    RAM_TYPE_MSG,
									    sizeof
									    (MSG_P2P_CONNECTION_REQUEST_T));

		if (prP2pConnReq == NULL) {
			break;
		}


		kalMemZero(prP2pConnReq, sizeof(MSG_P2P_CONNECTION_REQUEST_T));

		prP2pConnReq->rMsgHdr.eMsgId = MID_MNY_P2P_CONNECTION_REQ;

		prP2pConnReq->eFormationPolicy = ENUM_P2P_FORMATION_POLICY_AUTO;

		COPY_MAC_ADDR(prP2pConnReq->aucDeviceID, prP2pDevDesc->aucDeviceAddr);

		prP2pConnReq->u2ConfigMethod = prP2pDevDesc->u2ConfigMethod;

		if (ucInvitationType == P2P_INVITATION_TYPE_INVITATION) {
			prP2pConnReq->fgIsPersistentGroup = FALSE;
			prP2pConnReq->fgIsTobeGO = FALSE;

		}

		else if (ucInvitationType == P2P_INVITATION_TYPE_REINVOKE) {
			DBGLOG(P2P, TRACE, ("Re-invoke Persistent Group\n"));
			prP2pConnReq->fgIsPersistentGroup = TRUE;
			prP2pConnReq->fgIsTobeGO =
			    (prGlueInfo->prP2PInfo->ucRole == 2) ? TRUE : FALSE;

		}


		p2pFsmRunEventDeviceDiscoveryAbort(prGlueInfo->prAdapter, NULL);

		if (ucOperatingChnl != 0) {
			prP2pSpecificBssInfo->ucPreferredChannel = ucOperatingChnl;
		}

		if ((ucSsidLen < 32) && (pucSsid != NULL)) {
			COPY_SSID(prP2pConnSettings->aucSSID,
				  prP2pConnSettings->ucSSIDLen, pucSsid, ucSsidLen);
		}

		mboxSendMsg(prGlueInfo->prAdapter,
			    MBOX_ID_0, (P_MSG_HDR_T) prP2pConnReq, MSG_SEND_METHOD_BUF);



	} while (FALSE);

	/*           */
	/*                             */

	return;
#endif

}				/*                            */


/*                                                                            */
/* 
                                                                       
 
                                              
 
              
*/
/*                                                                            */
VOID kalP2PInvitationStatus(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4InvStatus)
{
	union iwreq_data evt;
	UINT_8 aucBuffer[IW_CUSTOM_MAX];

	ASSERT(prGlueInfo);

	/*                                                             */
	prGlueInfo->prP2PInfo->u4InvStatus = u4InvStatus;

	/*                         */
	memset(&evt, 0, sizeof(evt));

	snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_INV_STATUS");
	evt.data.length = strlen(aucBuffer);

	/*                              */
	wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler, IWEVCUSTOM, &evt, aucBuffer);

	return;
}				/*                        */

/*                                                                            */
/* 
                                                                                         
 
                                              
 
              
*/
/*                                                                            */
VOID
kalP2PIndicateSDRequest(IN P_GLUE_INFO_T prGlueInfo,
			IN PARAM_MAC_ADDRESS rPeerAddr, IN UINT_8 ucSeqNum)
{
	union iwreq_data evt;
	UINT_8 aucBuffer[IW_CUSTOM_MAX];

	ASSERT(prGlueInfo);

	memset(&evt, 0, sizeof(evt));

	snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_SD_REQ %d", ucSeqNum);
	evt.data.length = strlen(aucBuffer);

	/*                             */
	wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler, IWEVCUSTOM, &evt, aucBuffer);

	return;
}				/*                                  */


/*                                                                            */
/* 
                                                                       
                            
 
                                              
 
              
*/
/*                                                                            */
void
kalP2PIndicateSDResponse(IN P_GLUE_INFO_T prGlueInfo,
			 IN PARAM_MAC_ADDRESS rPeerAddr, IN UINT_8 ucSeqNum)
{
	union iwreq_data evt;
	UINT_8 aucBuffer[IW_CUSTOM_MAX];

	ASSERT(prGlueInfo);

	memset(&evt, 0, sizeof(evt));

	snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_SD_RESP %d", ucSeqNum);
	evt.data.length = strlen(aucBuffer);

	/*                             */
	wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler, IWEVCUSTOM, &evt, aucBuffer);

	return;
}				/*                                   */


/*                                                                            */
/* 
                                                                      
                            
 
                                              
                                                    
                                          
 
              
*/
/*                                                                            */
VOID kalP2PIndicateTXDone(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucSeqNum, IN UINT_8 ucStatus)
{
	union iwreq_data evt;
	UINT_8 aucBuffer[IW_CUSTOM_MAX];

	ASSERT(prGlueInfo);

	memset(&evt, 0, sizeof(evt));

	snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_SD_XMITTED: %d %d", ucSeqNum, ucStatus);
	evt.data.length = strlen(aucBuffer);

	/*                             */
	wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler, IWEVCUSTOM, &evt, aucBuffer);

	return;
}				/*                                   */


struct net_device *kalP2PGetDevHdlr(P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);
	return prGlueInfo->prP2PInfo->prDevHandler;
}

#if CFG_SUPPORT_ANTI_PIRACY
/*                                                                            */
/* 
        
 
                                            
 
              
*/
/*                                                                            */
VOID kalP2PIndicateSecCheckRsp(IN P_GLUE_INFO_T prGlueInfo, IN PUINT_8 pucRsp, IN UINT_16 u2RspLen)
{
	union iwreq_data evt;
	UINT_8 aucBuffer[IW_CUSTOM_MAX];

	ASSERT(prGlueInfo);

	memset(&evt, 0, sizeof(evt));
	snprintf(aucBuffer, IW_CUSTOM_MAX - 1, "P2P_SEC_CHECK_RSP=");

	kalMemCopy(prGlueInfo->prP2PInfo->aucSecCheckRsp, pucRsp, u2RspLen);
	evt.data.length = strlen(aucBuffer);

#if DBG
	DBGLOG_MEM8(SEC, LOUD, prGlueInfo->prP2PInfo->aucSecCheckRsp, u2RspLen);
#endif
	/*                             */
	wireless_send_event(prGlueInfo->prP2PInfo->prDevHandler, IWEVCUSTOM, &evt, aucBuffer);
	return;
}				/*                                */
#endif


/*                                                                            */
/* 
        
 
                                            
 
              
*/
/*                                                                            */
VOID
kalGetChnlList(IN P_GLUE_INFO_T prGlueInfo,
	       IN ENUM_BAND_T eSpecificBand,
	       IN UINT_8 ucMaxChannelNum,
	       IN PUINT_8 pucNumOfChannel, IN P_RF_CHANNEL_INFO_T paucChannelList)
{
	rlmDomainGetChnlList(prGlueInfo->prAdapter,
			     eSpecificBand, ucMaxChannelNum, pucNumOfChannel, paucChannelList);
}				/*                */

/*                                                                                       */

VOID
kalP2PIndicateChannelReady(IN P_GLUE_INFO_T prGlueInfo,
			   IN UINT_64 u8SeqNum,
			   IN UINT_32 u4ChannelNum,
			   IN ENUM_BAND_T eBand, IN ENUM_CHNL_EXT_T eSco, IN UINT_32 u4Duration)
{
	struct ieee80211_channel *prIEEE80211ChnlStruct = (struct ieee80211_channel *)NULL;
	RF_CHANNEL_INFO_T rChannelInfo;
	enum nl80211_channel_type eChnlType = NL80211_CHAN_NO_HT;

	do {
		if (prGlueInfo == NULL) {
			break;
		}


		kalMemZero(&rChannelInfo, sizeof(RF_CHANNEL_INFO_T));

		rChannelInfo.ucChannelNum = u4ChannelNum;
		rChannelInfo.eBand = eBand;

		prIEEE80211ChnlStruct =
		    kalP2pFuncGetChannelEntry(prGlueInfo->prP2PInfo, &rChannelInfo);

		kalP2pFuncGetChannelType(eSco, &eChnlType);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
		cfg80211_ready_on_channel(&prGlueInfo->prP2PInfo->wdev,	/*                      */
					  u8SeqNum,	/*             */
					  prIEEE80211ChnlStruct,	/*                                  */
					  u4Duration,	/*                        */
					  GFP_KERNEL);	/*           */ /*                  */
#else
		cfg80211_ready_on_channel(prGlueInfo->prP2PInfo->prDevHandler,	/*                          */
					  u8SeqNum,	/*             */
					  prIEEE80211ChnlStruct,	/*                                  */
					  eChnlType,	/*                                         */
					  u4Duration,	/*                        */
					  GFP_KERNEL);	/*           */ /*                  */
#endif
	} while (FALSE);

}				/*                            */

VOID
kalP2PIndicateChannelExpired(IN P_GLUE_INFO_T prGlueInfo,
			     IN UINT_64 u8SeqNum,
			     IN UINT_32 u4ChannelNum, IN ENUM_BAND_T eBand, IN ENUM_CHNL_EXT_T eSco)
{

	P_GL_P2P_INFO_T prGlueP2pInfo = (P_GL_P2P_INFO_T) NULL;
	struct ieee80211_channel *prIEEE80211ChnlStruct = (struct ieee80211_channel *)NULL;
	enum nl80211_channel_type eChnlType = NL80211_CHAN_NO_HT;
	RF_CHANNEL_INFO_T rRfChannelInfo;

	do {
		if (prGlueInfo == NULL) {
			ASSERT(FALSE);
			break;
		}

		prGlueP2pInfo = prGlueInfo->prP2PInfo;

		if (prGlueP2pInfo == NULL) {
			ASSERT(FALSE);
			break;
		}


		DBGLOG(P2P, TRACE, ("kalP2PIndicateChannelExpired\n"));

		rRfChannelInfo.eBand = eBand;
		rRfChannelInfo.ucChannelNum = u4ChannelNum;

		prIEEE80211ChnlStruct = kalP2pFuncGetChannelEntry(prGlueP2pInfo, &rRfChannelInfo);


		kalP2pFuncGetChannelType(eSco, &eChnlType);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
		cfg80211_remain_on_channel_expired(&prGlueP2pInfo->wdev,	/*                      */
						   u8SeqNum, prIEEE80211ChnlStruct, GFP_KERNEL);
#else
		cfg80211_remain_on_channel_expired(prGlueP2pInfo->prDevHandler,	/*                          */
						   u8SeqNum,
						   prIEEE80211ChnlStruct, eChnlType, GFP_KERNEL);
#endif
	} while (FALSE);

}				/*                              */

VOID
kalP2PIndicateScanDone(IN P_GLUE_INFO_T prGlueInfo, IN UINT_8 ucRoleIndex, IN BOOLEAN fgIsAbort)
{
	P_GL_P2P_INFO_T prGlueP2pInfo = (P_GL_P2P_INFO_T) NULL;
	struct cfg80211_scan_request *prScanRequest = NULL;
	GLUE_SPIN_LOCK_DECLARATION();

	do {
		if (prGlueInfo == NULL) {

			ASSERT(FALSE);
			break;
		}

		prGlueP2pInfo = prGlueInfo->prP2PInfo;

		if (prGlueP2pInfo == NULL) {
			ASSERT(FALSE);
			break;
		}

		DBGLOG(INIT, INFO, ("[p2p] scan complete %p\n", prGlueP2pInfo->prScanRequest));

		GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_NET_DEV);

		if (prGlueP2pInfo->prScanRequest != NULL) {
			prScanRequest = prGlueP2pInfo->prScanRequest;
			prGlueP2pInfo->prScanRequest = NULL;
		}
		GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_NET_DEV);

		if (prScanRequest != NULL) {

			/*                                                                */
			scanReportBss2Cfg80211(prGlueInfo->prAdapter, BSS_TYPE_P2P_DEVICE, NULL);

			DBGLOG(INIT, INFO, ("DBG:p2p_cfg_scan_done\n"));
			cfg80211_scan_done(prScanRequest, fgIsAbort);
		}

	} while (FALSE);


}				/*                        */

VOID
kalP2PIndicateBssInfo(IN P_GLUE_INFO_T prGlueInfo,
		      IN PUINT_8 pucFrameBuf,
		      IN UINT_32 u4BufLen,
		      IN P_RF_CHANNEL_INFO_T prChannelInfo, IN INT_32 i4SignalStrength)
{
	P_GL_P2P_INFO_T prGlueP2pInfo = (P_GL_P2P_INFO_T) NULL;
	struct ieee80211_channel *prChannelEntry = (struct ieee80211_channel *)NULL;
	struct ieee80211_mgmt *prBcnProbeRspFrame = (struct ieee80211_mgmt *)pucFrameBuf;
	struct cfg80211_bss *prCfg80211Bss = (struct cfg80211_bss *)NULL;

	do {
		if ((prGlueInfo == NULL) || (pucFrameBuf == NULL) || (prChannelInfo == NULL)) {
			ASSERT(FALSE);
			break;
		}

		prGlueP2pInfo = prGlueInfo->prP2PInfo;

		if (prGlueP2pInfo == NULL) {
			ASSERT(FALSE);
			break;
		}


		prChannelEntry = kalP2pFuncGetChannelEntry(prGlueP2pInfo, prChannelInfo);

		if (prChannelEntry == NULL) {
			DBGLOG(P2P, TRACE, ("Unknown channel info\n"));
			break;
		}

		/*                                                                                             */

		prCfg80211Bss = cfg80211_inform_bss_frame(prGlueP2pInfo->wdev.wiphy,	/*                       */
							  prChannelEntry,
							  prBcnProbeRspFrame,
							  u4BufLen, i4SignalStrength, GFP_KERNEL);


		/*                        */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
		cfg80211_put_bss(prGlueP2pInfo->wdev.wiphy, prCfg80211Bss);
#else
		cfg80211_put_bss(prCfg80211Bss);
#endif

	} while (FALSE);

	return;

}				/*                       */

VOID
kalP2PIndicateMgmtTxStatus(IN P_GLUE_INFO_T prGlueInfo,
			   IN P_MSDU_INFO_T prMsduInfo, IN BOOLEAN fgIsAck)
{
	P_GL_P2P_INFO_T prGlueP2pInfo = (P_GL_P2P_INFO_T) NULL;
	PUINT_64 pu8GlCookie = (PUINT_64) NULL;
	struct net_device *prNetdevice = (struct net_device *)NULL;

	do {
		if ((prGlueInfo == NULL) || (prMsduInfo == NULL)) {
			DBGLOG(P2P, WARN,
			       ("Unexpected pointer PARAM. 0x%lx, 0x%lx.\n", prGlueInfo,
				prMsduInfo));
			ASSERT(FALSE);
			break;
		}

		prGlueP2pInfo = prGlueInfo->prP2PInfo;

		pu8GlCookie =
		    (PUINT_64) ((ULONG) prMsduInfo->prPacket +
				(ULONG) prMsduInfo->u2FrameLength + MAC_TX_RESERVED_FIELD);

		if (prMsduInfo->ucBssIndex == P2P_DEV_BSS_INDEX) {
			prNetdevice = prGlueP2pInfo->prDevHandler;
		} else {
			P_BSS_INFO_T prP2pBssInfo =
			    GET_BSS_INFO_BY_INDEX(prGlueInfo->prAdapter, prMsduInfo->ucBssIndex);

			prNetdevice = prGlueP2pInfo->aprRoleHandler[prP2pBssInfo->u4PrivateData];
		}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
		cfg80211_mgmt_tx_status(&prGlueP2pInfo->wdev,	/*                          */
#else
		cfg80211_mgmt_tx_status(prGlueP2pInfo->prDevHandler,	/*                          */
#endif				/*                    */
					*pu8GlCookie,
					(PUINT_8) ((ULONG) prMsduInfo->prPacket +
						   MAC_TX_RESERVED_FIELD),
					prMsduInfo->u2FrameLength, fgIsAck, GFP_KERNEL);

	} while (FALSE);

}				/*                            */

VOID
kalP2PIndicateRxMgmtFrame(IN P_GLUE_INFO_T prGlueInfo,
			  IN P_SW_RFB_T prSwRfb, IN BOOLEAN fgIsDevInterface, IN UINT_8 ucRoleIdx)
{
#define DBG_P2P_MGMT_FRAME_INDICATION 1
	P_GL_P2P_INFO_T prGlueP2pInfo = (P_GL_P2P_INFO_T) NULL;
	INT_32 i4Freq = 0;
	UINT_8 ucChnlNum = 0;
#if DBG_P2P_MGMT_FRAME_INDICATION
	P_WLAN_MAC_HEADER_T prWlanHeader = (P_WLAN_MAC_HEADER_T) NULL;
#endif


	do {
		if ((prGlueInfo == NULL) || (prSwRfb == NULL)) {
			ASSERT(FALSE);
			break;
		}

		prGlueP2pInfo = prGlueInfo->prP2PInfo;

		/*                                              */
		/*                                                   */
		/*                                                  */

		ucChnlNum = HAL_RX_STATUS_GET_CHNL_NUM(prSwRfb->prRxStatus);

#if DBG_P2P_MGMT_FRAME_INDICATION

		prWlanHeader = (P_WLAN_MAC_HEADER_T) prSwRfb->pvHeader;

		switch (prWlanHeader->u2FrameCtrl) {
		case MAC_FRAME_PROBE_REQ:
			DBGLOG(P2P, TRACE, ("RX Probe Req at channel %d ", ucChnlNum));
			break;
		case MAC_FRAME_PROBE_RSP:
			DBGLOG(P2P, TRACE, ("RX Probe Rsp at channel %d ", ucChnlNum));
			break;
		case MAC_FRAME_ACTION:
			DBGLOG(P2P, TRACE, ("RX Action frame at channel %d ", ucChnlNum));
			break;
		default:
			DBGLOG(P2P, TRACE,
			       ("RX Packet:%d at channel %d ", prWlanHeader->u2FrameCtrl,
				ucChnlNum));
			break;
		}

		DBGLOG(P2P, TRACE, ("from: " MACSTR "\n", MAC2STR(prWlanHeader->aucAddr2)));
#endif
		i4Freq = nicChannelNum2Freq(ucChnlNum) / 1000;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0)
		cfg80211_rx_mgmt(&prGlueP2pInfo->wdev,	/*                          */
#else
		cfg80211_rx_mgmt((fgIsDevInterface) ? prGlueP2pInfo->prDevHandler : prGlueP2pInfo->aprRoleHandler[ucRoleIdx],	/*                          */
#endif				/*                    */
				 i4Freq,
				 RCPI_TO_dBm(HAL_RX_STATUS_GET_RCPI(prSwRfb->prRxStatusGroup3)),
				 prSwRfb->pvHeader, prSwRfb->u2PacketLen, GFP_ATOMIC);
#else
		cfg80211_rx_mgmt((fgIsDevInterface) ? prGlueP2pInfo->prDevHandler : prGlueP2pInfo->aprRoleHandler[ucRoleIdx],	/*                          */
				 i4Freq, prSwRfb->pvHeader, prSwRfb->u2PacketLen, GFP_ATOMIC);
#endif

	} while (FALSE);

}				/*                           */

VOID
kalP2PGCIndicateConnectionStatus(IN P_GLUE_INFO_T prGlueInfo,
				 IN UINT_8 ucRoleIndex,
				 IN P_P2P_CONNECTION_REQ_INFO_T prP2pConnInfo,
				 IN PUINT_8 pucRxIEBuf,
				 IN UINT_16 u2RxIELen, IN UINT_16 u2StatusReason)
{
	P_GL_P2P_INFO_T prGlueP2pInfo = (P_GL_P2P_INFO_T) NULL;

	do {
		if (prGlueInfo == NULL) {
			ASSERT(FALSE);
			break;
		}

		prGlueP2pInfo = prGlueInfo->prP2PInfo;

		if (prP2pConnInfo) {
			cfg80211_connect_result(prGlueP2pInfo->aprRoleHandler[ucRoleIndex],	/*                          */
						prP2pConnInfo->aucBssid, prP2pConnInfo->aucIEBuf, prP2pConnInfo->u4BufLength, pucRxIEBuf, u2RxIELen, u2StatusReason, GFP_KERNEL);	/*           *//*                  */

			prP2pConnInfo->eConnRequest = P2P_CONNECTION_TYPE_IDLE;
		} else {
			/*                                          */
			cfg80211_disconnected(prGlueP2pInfo->aprRoleHandler[ucRoleIndex],	/*                          */
					      u2StatusReason, pucRxIEBuf, u2RxIELen, GFP_KERNEL);
		}

	} while (FALSE);


}				/*                                  */


VOID
kalP2PGOStationUpdate(IN P_GLUE_INFO_T prGlueInfo,
		      IN UINT_8 ucRoleIndex, IN P_STA_RECORD_T prCliStaRec, IN BOOLEAN fgIsNew)
{
	P_GL_P2P_INFO_T prP2pGlueInfo = (P_GL_P2P_INFO_T) NULL;

	do {
		if ((prGlueInfo == NULL) || (prCliStaRec == NULL) || (ucRoleIndex >= 2)) {
			break;
		}

		prP2pGlueInfo = prGlueInfo->prP2PInfo;

		if (fgIsNew) {
			struct station_info rStationInfo;

			kalMemZero(&rStationInfo, sizeof(rStationInfo));

			rStationInfo.filled = STATION_INFO_ASSOC_REQ_IES;
			rStationInfo.generation = ++prP2pGlueInfo->i4Generation;

			rStationInfo.assoc_req_ies = prCliStaRec->pucAssocReqIe;
			rStationInfo.assoc_req_ies_len = prCliStaRec->u2AssocReqIeLen;

			cfg80211_new_sta(prGlueInfo->prP2PInfo->aprRoleHandler[ucRoleIndex],	/*                          */
					 prCliStaRec->aucMacAddr, &rStationInfo, GFP_KERNEL);
		} else {
			++prP2pGlueInfo->i4Generation;

			cfg80211_del_sta(prGlueInfo->prP2PInfo->aprRoleHandler[ucRoleIndex],	/*                          */
					 prCliStaRec->aucMacAddr, GFP_KERNEL);
		}


	} while (FALSE);

	return;

}				/*                       */




BOOLEAN
kalP2pFuncGetChannelType(IN ENUM_CHNL_EXT_T rChnlSco, OUT enum nl80211_channel_type *channel_type)
{
	BOOLEAN fgIsValid = FALSE;

	do {
		if (channel_type) {

			switch (rChnlSco) {
			case CHNL_EXT_SCN:
				*channel_type = NL80211_CHAN_NO_HT;
				break;
			case CHNL_EXT_SCA:
				*channel_type = NL80211_CHAN_HT40MINUS;
				break;
			case CHNL_EXT_SCB:
				*channel_type = NL80211_CHAN_HT40PLUS;
				break;
			default:
				ASSERT(FALSE);
				*channel_type = NL80211_CHAN_NO_HT;
				break;
			}

		}

		fgIsValid = TRUE;
	} while (FALSE);

	return fgIsValid;
}				/*                          */




struct ieee80211_channel *kalP2pFuncGetChannelEntry(IN P_GL_P2P_INFO_T prP2pInfo,
						    IN P_RF_CHANNEL_INFO_T prChannelInfo)
{
	struct ieee80211_channel *prTargetChannelEntry = (struct ieee80211_channel *)NULL;
	UINT_32 u4TblSize = 0, u4Idx = 0;

	do {
		if ((prP2pInfo == NULL) || (prChannelInfo == NULL)) {
			break;
		}


		switch (prChannelInfo->eBand) {
		case BAND_2G4:
			prTargetChannelEntry =
			    prP2pInfo->wdev.wiphy->bands[IEEE80211_BAND_2GHZ]->channels;
			u4TblSize = prP2pInfo->wdev.wiphy->bands[IEEE80211_BAND_2GHZ]->n_channels;
			break;
		case BAND_5G:
			prTargetChannelEntry =
			    prP2pInfo->wdev.wiphy->bands[IEEE80211_BAND_5GHZ]->channels;
			u4TblSize = prP2pInfo->wdev.wiphy->bands[IEEE80211_BAND_5GHZ]->n_channels;
			break;
		default:
			break;
		}


		if (prTargetChannelEntry == NULL) {
			break;
		}



		for (u4Idx = 0; u4Idx < u4TblSize; u4Idx++, prTargetChannelEntry++) {
			if (prTargetChannelEntry->hw_value == prChannelInfo->ucChannelNum) {
				break;
			}

		}


		if (u4Idx == u4TblSize) {
			prTargetChannelEntry = NULL;
			break;
		}


	} while (FALSE);

	return prTargetChannelEntry;
}				/*                           */

#if CFG_SUPPORT_HOTSPOT_WPS_MANAGER

/*                                                                            */
/* 
                                         
 
            
                      
 
         
*/
/*                                                                            */
BOOLEAN
kalP2PSetBlackList(IN P_GLUE_INFO_T prGlueInfo, IN PARAM_MAC_ADDRESS rbssid, IN BOOLEAN fgIsblock)
{
	UINT_8 aucNullAddr[] = NULL_MAC_ADDR;
	BOOLEAN fgIsValid = FALSE;
	UINT_32 i;

	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	if (fgIsblock) {
		for (i = 0; i < P2P_MAXIMUM_CLIENT_COUNT; i++) {
			if (UNEQUAL_MAC_ADDR(rbssid, aucNullAddr)) {
				if (UNEQUAL_MAC_ADDR
				    (&(prGlueInfo->prP2PInfo->aucblackMACList[i]), rbssid)) {
					if (EQUAL_MAC_ADDR
					    (&(prGlueInfo->prP2PInfo->aucblackMACList[i]),
					     aucNullAddr)) {
						COPY_MAC_ADDR(&
							      (prGlueInfo->prP2PInfo->aucblackMACList[i]), rbssid);
						fgIsValid = FALSE;
						return fgIsValid;
					}
				}
			}
		}
	} else {
		for (i = 0; i < P2P_MAXIMUM_CLIENT_COUNT; i++) {
			if (EQUAL_MAC_ADDR(&(prGlueInfo->prP2PInfo->aucblackMACList[i]), rbssid)) {
				COPY_MAC_ADDR(&(prGlueInfo->prP2PInfo->aucblackMACList[i]),
					      aucNullAddr);
				fgIsValid = FALSE;
				return fgIsValid;
			}
		}
	}

	return fgIsValid;

}


/*                                                                            */
/* 
                                             
 
            
                      
 
         
*/
/*                                                                            */
BOOLEAN kalP2PCmpBlackList(IN P_GLUE_INFO_T prGlueInfo, IN PARAM_MAC_ADDRESS rbssid)
{
	UINT_8 aucNullAddr[] = NULL_MAC_ADDR;
	BOOLEAN fgIsExsit = FALSE;
	UINT_32 i;

	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	for (i = 0; i < P2P_MAXIMUM_CLIENT_COUNT; i++) {
		if (UNEQUAL_MAC_ADDR(rbssid, aucNullAddr)) {
			if (EQUAL_MAC_ADDR(&(prGlueInfo->prP2PInfo->aucblackMACList[i]), rbssid)) {
				fgIsExsit = TRUE;
				return fgIsExsit;
			}
		}
	}

	return fgIsExsit;

}


/*                                                                            */
/* 
                                             
 
            
                      
 
         
*/
/*                                                                            */
VOID kalP2PSetMaxClients(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4MaxClient)
{
	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	if (u4MaxClient == 0 || prGlueInfo->prP2PInfo->ucMaxClients >= P2P_MAXIMUM_CLIENT_COUNT)
		prGlueInfo->prP2PInfo->ucMaxClients = P2P_MAXIMUM_CLIENT_COUNT;
	else
		prGlueInfo->prP2PInfo->ucMaxClients = u4MaxClient;
}


/*                                                                            */
/* 
                                             
 
            
                      
 
         
*/
/*                                                                            */
BOOLEAN kalP2PMaxClients(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4NumClient)
{
	ASSERT(prGlueInfo);
	ASSERT(prGlueInfo->prP2PInfo);

	if (prGlueInfo->prP2PInfo->ucMaxClients) {
		if ((UINT_8) u4NumClient > prGlueInfo->prP2PInfo->ucMaxClients)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

#endif