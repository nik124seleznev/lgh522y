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


VOID
scanP2pProcessBeaconAndProbeResp(IN P_ADAPTER_T prAdapter,
				 IN P_SW_RFB_T prSwRfb,
				 IN P_WLAN_STATUS prStatus,
				 IN P_BSS_DESC_T prBssDesc,
				 IN P_WLAN_BEACON_FRAME_T prWlanBeaconFrame)
{
	if (prBssDesc->fgIsP2PPresent) {
		if ((prBssDesc->fgIsConnected) &&	/*                   */
		    ((prWlanBeaconFrame->u2FrameCtrl & MASK_FRAME_TYPE) == MAC_FRAME_BEACON)	/*           */
		    ) {
			UINT_32 u4Idx = 0;
			P_BSS_INFO_T prP2pBssInfo = (P_BSS_INFO_T) NULL;

			for (u4Idx = 0; u4Idx < BSS_INFO_NUM; u4Idx++) {
				/*                    */
				/*              */
				prP2pBssInfo = GET_BSS_INFO_BY_INDEX(prAdapter, (UINT_8) u4Idx);

				if (!IS_BSS_ACTIVE(prP2pBssInfo)) {
					continue;
				}

				if ((prP2pBssInfo->eNetworkType != NETWORK_TYPE_P2P) ||
				    (UNEQUAL_MAC_ADDR(prP2pBssInfo->aucBSSID, prBssDesc->aucBSSID)
				     ||
				     (!EQUAL_SSID
				      (prP2pBssInfo->aucSSID, prP2pBssInfo->ucSSIDLen,
				       prBssDesc->aucSSID, prBssDesc->ucSSIDLen)))) {
					continue;
				}

				if ((prP2pBssInfo->eCurrentOPMode == OP_MODE_INFRASTRUCTURE) &&	/*        */
				    (prP2pBssInfo->eConnectionState == PARAM_MEDIA_STATE_CONNECTED) &&	/*           */
				    (!prP2pBssInfo->ucDTIMPeriod)) {	/*             */
					prP2pBssInfo->ucDTIMPeriod = prBssDesc->ucDTIMPeriod;
					nicPmIndicateBssConnected(prAdapter,
								  prP2pBssInfo->ucBssIndex);
				}

			}

		}

		do {
			RF_CHANNEL_INFO_T rChannelInfo;

			ASSERT_BREAK((prSwRfb != NULL) && (prBssDesc != NULL));

			if (((prWlanBeaconFrame->u2FrameCtrl & MASK_FRAME_TYPE) !=
			     MAC_FRAME_PROBE_RSP)) {
				/*                                                 */
				/*                                               */
				break;
			}

			rChannelInfo.ucChannelNum = prBssDesc->ucChannelNum;
			rChannelInfo.eBand = prBssDesc->eBand;
			prBssDesc->fgIsP2PReport = TRUE;

			DBGLOG(P2P, INFO,
			       ("indicate %s [%d]\n", prBssDesc->aucSSID, prBssDesc->ucChannelNum));

			kalP2PIndicateBssInfo(prAdapter->prGlueInfo,
					      (PUINT_8) prSwRfb->pvHeader,
					      (UINT_32) prSwRfb->u2PacketLen,
					      &rChannelInfo, RCPI_TO_dBm(prBssDesc->ucRCPI));


		} while (FALSE);
	}
}

VOID scnEventReturnChannel(IN P_ADAPTER_T prAdapter, IN UINT_8 ucScnSeqNum)
{

	CMD_SCAN_CANCEL rCmdScanCancel;

	/*                                        */
	rCmdScanCancel.ucSeqNum = ucScnSeqNum;
	rCmdScanCancel.ucIsExtChannel = (UINT_8) FALSE;

	wlanSendSetQueryCmd(prAdapter,
			    CMD_ID_SCAN_CANCEL,
			    TRUE,
			    FALSE,
			    FALSE,
			    NULL,
			    NULL, sizeof(CMD_SCAN_CANCEL), (PUINT_8) & rCmdScanCancel, NULL, 0);

	return;
}				/*                       */

VOID scanRemoveAllP2pBssDesc(IN P_ADAPTER_T prAdapter)
{
	P_LINK_T prBSSDescList;
	P_BSS_DESC_T prBssDesc;
	P_BSS_DESC_T prBSSDescNext;

	ASSERT(prAdapter);

	prBSSDescList = &(prAdapter->rWifiVar.rScanInfo.rBSSDescList);

	/*                                                */
	LINK_FOR_EACH_ENTRY_SAFE(prBssDesc, prBSSDescNext, prBSSDescList, rLinkEntry, BSS_DESC_T) {
		scanRemoveP2pBssDesc(prAdapter, prBssDesc);
	}
}				/*                         */

VOID scanRemoveP2pBssDesc(IN P_ADAPTER_T prAdapter, IN P_BSS_DESC_T prBssDesc)
{

	return;
}				/*                      */


P_BSS_DESC_T
scanP2pSearchDesc(IN P_ADAPTER_T prAdapter, IN P_P2P_CONNECTION_REQ_INFO_T prConnReqInfo)
{
	P_BSS_DESC_T prCandidateBssDesc = (P_BSS_DESC_T) NULL, prBssDesc = (P_BSS_DESC_T) NULL;
	P_LINK_T prBssDescList = (P_LINK_T) NULL;

	do {
		if ((prAdapter == NULL) || (prConnReqInfo == NULL)) {
			break;
		}


		prBssDescList = &(prAdapter->rWifiVar.rScanInfo.rBSSDescList);

		DBGLOG(P2P, LOUD,
		       ("Connecting to BSSID: " MACSTR "\n", MAC2STR(prConnReqInfo->aucBssid)));
		DBGLOG(P2P, LOUD,
		       ("Connecting to SSID:%s, length:%d\n", prConnReqInfo->rSsidStruct.aucSsid,
			prConnReqInfo->rSsidStruct.ucSsidLen));

		LINK_FOR_EACH_ENTRY(prBssDesc, prBssDescList, rLinkEntry, BSS_DESC_T) {
			DBGLOG(P2P, LOUD,
			       ("Checking BSS: " MACSTR "\n", MAC2STR(prBssDesc->aucBSSID)));

			if (prBssDesc->eBSSType != BSS_TYPE_INFRASTRUCTURE) {
				DBGLOG(P2P, LOUD, ("Ignore mismatch BSS type.\n"));
				continue;
			}


			if (UNEQUAL_MAC_ADDR(prBssDesc->aucBSSID, prConnReqInfo->aucBssid)) {
				DBGLOG(P2P, LOUD, ("Ignore mismatch BSSID.\n"));
				continue;
			}


			/*                                                                  */
			if (UNEQUAL_SSID(prConnReqInfo->rSsidStruct.aucSsid,
					 prConnReqInfo->rSsidStruct.ucSsidLen,
					 prBssDesc->aucSSID, prBssDesc->ucSSIDLen)) {

				DBGLOG(P2P, TRACE,
				       ("Connecting to BSSID: " MACSTR "\n",
					MAC2STR(prConnReqInfo->aucBssid)));
				DBGLOG(P2P, TRACE,
				       ("Connecting to SSID:%s, length:%d\n",
					prConnReqInfo->rSsidStruct.aucSsid,
					prConnReqInfo->rSsidStruct.ucSsidLen));
				DBGLOG(P2P, TRACE,
				       ("Checking SSID:%s, length:%d\n", prBssDesc->aucSSID,
					prBssDesc->ucSSIDLen));
				DBGLOG(P2P, TRACE, ("Ignore mismatch SSID, (But BSSID match).\n"));
				//                                         
				continue;
			}

			if (!prBssDesc->fgIsP2PPresent) {
				DBGLOG(P2P, ERROR,
				       ("SSID, BSSID, BSSTYPE match, but no P2P IE present.\n"));
				continue;
			}

			/*                 */
			prCandidateBssDesc = prBssDesc;
			break;
		}



	} while (FALSE);

	return prCandidateBssDesc;
}				/*                   */
