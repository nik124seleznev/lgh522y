/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * File: bssdb.c
 *
 * Purpose: Handles the Basic Service Set & Node Database functions
 *
 * Functions:
 *      BSSpSearchBSSList - Search known BSS list for Desire SSID or BSSID
 *      BSSvClearBSSList - Clear BSS List
 *      BSSbInsertToBSSList - Insert a BSS set into known BSS list
 *      BSSbUpdateToBSSList - Update BSS set in known BSS list
 *      BSSbIsSTAInNodeDB - Search Node DB table to find the index of matched DstAddr
 *      BSSvCreateOneNode - Allocate an Node for Node DB
 *      BSSvUpdateAPNode - Update AP Node content in Index 0 of KnownNodeDB
 *      BSSvSecondCallBack - One second timer callback function to update Node DB info & AP link status
 *      BSSvUpdateNodeTxCounter - Update Tx attemps, Tx failure counter in Node DB for auto-fall back rate control
 *
 * Revision History:
 *
 * Author: Lyndon Chen
 *
 * Date: July 17, 2002
 *
 */

#include "tmacro.h"
#include "tether.h"
#include "device.h"
#include "80211hdr.h"
#include "bssdb.h"
#include "wmgr.h"
#include "datarate.h"
#include "desc.h"
#include "wcmd.h"
#include "wpa.h"
#include "baseband.h"
#include "rf.h"
#include "card.h"
#include "mac.h"
#include "wpa2.h"
#include "control.h"
#include "rndis.h"
#include "iowpa.h"

static int          msglevel                =MSG_LEVEL_INFO;
//                                                             

const u16             awHWRetry0[5][5] = {
                                            {RATE_18M, RATE_18M, RATE_12M, RATE_12M, RATE_12M},
                                            {RATE_24M, RATE_24M, RATE_18M, RATE_12M, RATE_12M},
                                            {RATE_36M, RATE_36M, RATE_24M, RATE_18M, RATE_18M},
                                            {RATE_48M, RATE_48M, RATE_36M, RATE_24M, RATE_24M},
                                            {RATE_54M, RATE_54M, RATE_48M, RATE_36M, RATE_36M}
                                           };
const u16             awHWRetry1[5][5] = {
                                            {RATE_18M, RATE_18M, RATE_12M, RATE_6M, RATE_6M},
                                            {RATE_24M, RATE_24M, RATE_18M, RATE_6M, RATE_6M},
                                            {RATE_36M, RATE_36M, RATE_24M, RATE_12M, RATE_12M},
                                            {RATE_48M, RATE_48M, RATE_24M, RATE_12M, RATE_12M},
                                            {RATE_54M, RATE_54M, RATE_36M, RATE_18M, RATE_18M}
                                           };

static void s_vCheckSensitivity(struct vnt_private *pDevice);
static void s_vCheckPreEDThreshold(struct vnt_private *pDevice);
static void s_uCalculateLinkQual(struct vnt_private *pDevice);

/* 
  
                       
                                                     
  
                
                             
  
 */

PKnownBSS BSSpSearchBSSList(struct vnt_private *pDevice,
		u8 *pbyDesireBSSID, u8 *pbyDesireSSID,
		CARD_PHY_TYPE ePhyType)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	u8 *pbyBSSID = NULL;
	PWLAN_IE_SSID pSSID = NULL;
	PKnownBSS pCurrBSS = NULL;
	PKnownBSS pSelect = NULL;
	u8 ZeroBSSID[WLAN_BSSID_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	int ii = 0;
	int jj = 0;

    if (pbyDesireBSSID != NULL) {
		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO
			"BSSpSearchBSSList BSSID[%pM]\n", pbyDesireBSSID);
	if ((!is_broadcast_ether_addr(pbyDesireBSSID)) &&
	     (memcmp(pbyDesireBSSID, ZeroBSSID, 6)!= 0)){
            pbyBSSID = pbyDesireBSSID;
        }
    }
    if (pbyDesireSSID != NULL) {
        if (((PWLAN_IE_SSID)pbyDesireSSID)->len != 0) {
            pSSID = (PWLAN_IE_SSID) pbyDesireSSID;
        }
    }

    if ((pbyBSSID != NULL)&&(pDevice->bRoaming == false)) {
        //                  
        for (ii = 0; ii <MAX_BSS_NUM; ii++) {
            pCurrBSS = &(pMgmt->sBSSList[ii]);

	   pCurrBSS->bSelected = false;

            if ((pCurrBSS->bActive) &&
                (pCurrBSS->bSelected == false)) {
		    if (!compare_ether_addr(pCurrBSS->abyBSSID, pbyBSSID)) {
                    if (pSSID != NULL) {
                        //             
                        if ( !memcmp(pSSID->abySSID,
                            ((PWLAN_IE_SSID)pCurrBSS->abySSID)->abySSID,
                            pSSID->len)) {
                            if ((pMgmt->eConfigMode == WMAC_CONFIG_AUTO) ||
                                ((pMgmt->eConfigMode == WMAC_CONFIG_IBSS_STA) && WLAN_GET_CAP_INFO_IBSS(pCurrBSS->wCapInfo)) ||
                                ((pMgmt->eConfigMode == WMAC_CONFIG_ESS_STA) && WLAN_GET_CAP_INFO_ESS(pCurrBSS->wCapInfo))
                                ) {
                                pCurrBSS->bSelected = true;
                                return(pCurrBSS);
                            }
                        }
                    } else {
                        if ((pMgmt->eConfigMode == WMAC_CONFIG_AUTO) ||
                            ((pMgmt->eConfigMode == WMAC_CONFIG_IBSS_STA) && WLAN_GET_CAP_INFO_IBSS(pCurrBSS->wCapInfo)) ||
                            ((pMgmt->eConfigMode == WMAC_CONFIG_ESS_STA) && WLAN_GET_CAP_INFO_ESS(pCurrBSS->wCapInfo))
                            ) {
                            pCurrBSS->bSelected = true;
                            return(pCurrBSS);
                        }
                    }
                }
            }
        }
    } else {
        //             
        for (ii = 0; ii <MAX_BSS_NUM; ii++) {
            pCurrBSS = &(pMgmt->sBSSList[ii]);

           //                            
         //                             
         //                                        

	  pCurrBSS->bSelected = false;
          if (pCurrBSS->bActive) {

                if (pSSID != NULL) {
                    //             
                    if (memcmp(pSSID->abySSID,
                        ((PWLAN_IE_SSID)pCurrBSS->abySSID)->abySSID,
                        pSSID->len) ||
                        (pSSID->len != ((PWLAN_IE_SSID)pCurrBSS->abySSID)->len)) {
                        //                             
                        continue;
                      }
                }
                if (((pMgmt->eConfigMode == WMAC_CONFIG_IBSS_STA) && WLAN_GET_CAP_INFO_ESS(pCurrBSS->wCapInfo)) ||
                    ((pMgmt->eConfigMode == WMAC_CONFIG_ESS_STA) && WLAN_GET_CAP_INFO_IBSS(pCurrBSS->wCapInfo))
                    ){
                    //                             
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"BSS type mismatch.... Config[%d] BSS[0x%04x]\n", pMgmt->eConfigMode, pCurrBSS->wCapInfo);
                    continue;
                }

                if (ePhyType != PHY_TYPE_AUTO) {
                    if (((ePhyType == PHY_TYPE_11A) && (PHY_TYPE_11A != pCurrBSS->eNetworkTypeInUse)) ||
                        ((ePhyType != PHY_TYPE_11A) && (PHY_TYPE_11A == pCurrBSS->eNetworkTypeInUse))) {
                        //                                
                        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Physical type mismatch.... ePhyType[%d] BSS[%d]\n", ePhyType, pCurrBSS->eNetworkTypeInUse);
                        continue;
                    }
                }

        pMgmt->pSameBSS[jj].uChannel = pCurrBSS->uChannel;
		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO
			"BSSpSearchBSSList pSelect1[%pM]\n",
			pCurrBSS->abyBSSID);
        jj++;

                if (pSelect == NULL) {
                    pSelect = pCurrBSS;
                } else {
                    //                                           
                    if (pCurrBSS->uRSSI < pSelect->uRSSI) {
                        pSelect = pCurrBSS;
                    }
                }
            }
        }

pDevice->bSameBSSMaxNum = jj;

        if (pSelect != NULL) {
            pSelect->bSelected = true;
                        if (pDevice->bRoaming == false)  {
	//                          
			memcpy(pbyDesireSSID,pCurrBSS->abySSID,WLAN_IEHDR_LEN + WLAN_SSID_MAXLEN + 1) ;
                                                }

            return(pSelect);
        }
    }
    return(NULL);

}

/* 
  
                       
                    
  
                
           
  
 */

void BSSvClearBSSList(struct vnt_private *pDevice, int bKeepCurrBSSID)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	int ii;

    for (ii = 0; ii < MAX_BSS_NUM; ii++) {
        if (bKeepCurrBSSID) {
            if (pMgmt->sBSSList[ii].bActive &&
		!compare_ether_addr(pMgmt->sBSSList[ii].abyBSSID,
				    pMgmt->abyCurrBSSID)) {
 //                                                                                              
 //                                                                                      
 //                                                
               //                        
                continue;
            }
        }

	pMgmt->sBSSList[ii].bActive = false;
        memset(&pMgmt->sBSSList[ii], 0, sizeof(KnownBSS));
    }
    BSSvClearAnyBSSJoinRecord(pDevice);
}

/* 
  
                       
                                                
  
                
                    
  
 */
PKnownBSS BSSpAddrIsInBSSList(struct vnt_private *pDevice,
	u8 *abyBSSID, PWLAN_IE_SSID pSSID)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	PKnownBSS pBSSList = NULL;
	int ii;

    for (ii = 0; ii < MAX_BSS_NUM; ii++) {
        pBSSList = &(pMgmt->sBSSList[ii]);
        if (pBSSList->bActive) {
		if (!compare_ether_addr(pBSSList->abyBSSID, abyBSSID)) {
                if (pSSID->len == ((PWLAN_IE_SSID)pBSSList->abySSID)->len){
                    if (memcmp(pSSID->abySSID,
                            ((PWLAN_IE_SSID)pBSSList->abySSID)->abySSID,
                            pSSID->len) == 0)
                        return pBSSList;
                }
            }
        }
    }

    return NULL;
};

/* 
  
                       
                                          
  
                
                      
  
 */

int BSSbInsertToBSSList(struct vnt_private *pDevice,
			u8 *abyBSSIDAddr,
			u64 qwTimestamp,
			u16 wBeaconInterval,
			u16 wCapInfo,
			u8 byCurrChannel,
			PWLAN_IE_SSID pSSID,
			PWLAN_IE_SUPP_RATES pSuppRates,
			PWLAN_IE_SUPP_RATES pExtSuppRates,
			PERPObject psERP,
			PWLAN_IE_RSN pRSN,
			PWLAN_IE_RSN_EXT pRSNWPA,
			PWLAN_IE_COUNTRY pIE_Country,
			PWLAN_IE_QUIET pIE_Quiet,
			u32 uIELength,
			u8 *pbyIEs,
			void *pRxPacketContext)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	struct vnt_rx_mgmt *pRxPacket =
		(struct vnt_rx_mgmt *)pRxPacketContext;
	PKnownBSS pBSSList = NULL;
	unsigned int ii;
	bool bParsingQuiet = false;

    pBSSList = (PKnownBSS)&(pMgmt->sBSSList[0]);

    for (ii = 0; ii < MAX_BSS_NUM; ii++) {
        pBSSList = (PKnownBSS)&(pMgmt->sBSSList[ii]);
        if (!pBSSList->bActive)
                break;
    }

    if (ii == MAX_BSS_NUM){
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "Get free KnowBSS node failed.\n");
        return false;
    }
    //                  
    pBSSList->bActive = true;
    memcpy( pBSSList->abyBSSID, abyBSSIDAddr, WLAN_BSSID_LEN);
	pBSSList->qwBSSTimestamp = cpu_to_le64(qwTimestamp);
    pBSSList->wBeaconInterval = cpu_to_le16(wBeaconInterval);
    pBSSList->wCapInfo = cpu_to_le16(wCapInfo);
    pBSSList->uClearCount = 0;

    if (pSSID->len > WLAN_SSID_MAXLEN)
        pSSID->len = WLAN_SSID_MAXLEN;
    memcpy( pBSSList->abySSID, pSSID, pSSID->len + WLAN_IEHDR_LEN);

    pBSSList->uChannel = byCurrChannel;

    if (pSuppRates->len > WLAN_RATES_MAXLEN)
        pSuppRates->len = WLAN_RATES_MAXLEN;
    memcpy( pBSSList->abySuppRates, pSuppRates, pSuppRates->len + WLAN_IEHDR_LEN);

    if (pExtSuppRates != NULL) {
        if (pExtSuppRates->len > WLAN_RATES_MAXLEN)
            pExtSuppRates->len = WLAN_RATES_MAXLEN;
        memcpy(pBSSList->abyExtSuppRates, pExtSuppRates, pExtSuppRates->len + WLAN_IEHDR_LEN);
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"BSSbInsertToBSSList: pExtSuppRates->len = %d\n", pExtSuppRates->len);

    } else {
        memset(pBSSList->abyExtSuppRates, 0, WLAN_IEHDR_LEN + WLAN_RATES_MAXLEN + 1);
    }
    pBSSList->sERP.byERP = psERP->byERP;
    pBSSList->sERP.bERPExist = psERP->bERPExist;

    //                            
    if (pBSSList->uChannel > CB_MAX_CHANNEL_24G) {
        pBSSList->eNetworkTypeInUse = PHY_TYPE_11A;
    } else {
        if (pBSSList->sERP.bERPExist == true) {
            pBSSList->eNetworkTypeInUse = PHY_TYPE_11G;
        } else {
            pBSSList->eNetworkTypeInUse = PHY_TYPE_11B;
        }
    }

    pBSSList->byRxRate = pRxPacket->byRxRate;
    pBSSList->qwLocalTSF = pRxPacket->qwLocalTSF;
    pBSSList->uRSSI = pRxPacket->uRSSI;
    pBSSList->bySQ = pRxPacket->bySQ;

   if ((pMgmt->eCurrMode == WMAC_MODE_ESS_STA) &&
        (pMgmt->eCurrState == WMAC_STATE_ASSOC)) {
        //               
        if (pBSSList == pMgmt->pCurrBSS) {
            bParsingQuiet = true;
        }
    }

    WPA_ClearRSN(pBSSList);

    if (pRSNWPA != NULL) {
	unsigned int uLen = pRSNWPA->len + 2;

	if (uLen <= (uIELength -
		     (unsigned int) (u32) ((u8 *) pRSNWPA - pbyIEs))) {
		pBSSList->wWPALen = uLen;
		memcpy(pBSSList->byWPAIE, pRSNWPA, uLen);
		WPA_ParseRSN(pBSSList, pRSNWPA);
	}
    }

    WPA2_ClearRSN(pBSSList);

    if (pRSN != NULL) {
	unsigned int uLen = pRSN->len + 2;

	if (uLen <= (uIELength -
		     (unsigned int) (u32) ((u8 *) pRSN - pbyIEs))) {
		pBSSList->wRSNLen = uLen;
		memcpy(pBSSList->byRSNIE, pRSN, uLen);
		WPA2vParseRSN(pBSSList, pRSN);
	}
    }

    if ((pMgmt->eAuthenMode == WMAC_AUTH_WPA2) || (pBSSList->bWPA2Valid == true)) {

        PSKeyItem  pTransmitKey = NULL;
        bool       bIs802_1x = false;

        for (ii = 0; ii < pBSSList->wAKMSSAuthCount; ii ++) {
            if (pBSSList->abyAKMSSAuthType[ii] == WLAN_11i_AKMSS_802_1X) {
                bIs802_1x = true;
                break;
            }
        }
        if ((bIs802_1x == true) && (pSSID->len == ((PWLAN_IE_SSID)pMgmt->abyDesireSSID)->len) &&
            ( !memcmp(pSSID->abySSID, ((PWLAN_IE_SSID)pMgmt->abyDesireSSID)->abySSID, pSSID->len))) {

		bAdd_PMKID_Candidate((void *) pDevice,
				     pBSSList->abyBSSID,
				     &pBSSList->sRSNCapObj);

            if ((pDevice->bLinkPass == true) && (pMgmt->eCurrState == WMAC_STATE_ASSOC)) {
                if ((KeybGetTransmitKey(&(pDevice->sKey), pDevice->abyBSSID, PAIRWISE_KEY, &pTransmitKey) == true) ||
                    (KeybGetTransmitKey(&(pDevice->sKey), pDevice->abyBSSID, GROUP_KEY, &pTransmitKey) == true)) {
                    pDevice->gsPMKIDCandidate.StatusType = Ndis802_11StatusType_PMKID_CandidateList;
                    pDevice->gsPMKIDCandidate.Version = 1;

                }

            }
        }
    }

    if (pDevice->bUpdateBBVGA) {
        //                               
        pBSSList->byRSSIStatCnt = 0;
        RFvRSSITodBm(pDevice, (u8)(pRxPacket->uRSSI), &pBSSList->ldBmMAX);
        pBSSList->ldBmAverage[0] = pBSSList->ldBmMAX;
        pBSSList->ldBmAverRange = pBSSList->ldBmMAX;
        for (ii = 1; ii < RSSI_STAT_COUNT; ii++)
            pBSSList->ldBmAverage[ii] = 0;
    }

    pBSSList->uIELength = uIELength;
    if (pBSSList->uIELength > WLAN_BEACON_FR_MAXLEN)
        pBSSList->uIELength = WLAN_BEACON_FR_MAXLEN;
    memcpy(pBSSList->abyIEs, pbyIEs, pBSSList->uIELength);

    return true;
}

/* 
  
                       
                                      
  
                
                      
  
 */
//                             

int BSSbUpdateToBSSList(struct vnt_private *pDevice,
			u64 qwTimestamp,
			u16 wBeaconInterval,
			u16 wCapInfo,
			u8 byCurrChannel,
			int bChannelHit,
			PWLAN_IE_SSID pSSID,
			PWLAN_IE_SUPP_RATES pSuppRates,
			PWLAN_IE_SUPP_RATES pExtSuppRates,
			PERPObject psERP,
			PWLAN_IE_RSN pRSN,
			PWLAN_IE_RSN_EXT pRSNWPA,
			PWLAN_IE_COUNTRY pIE_Country,
			PWLAN_IE_QUIET pIE_Quiet,
			PKnownBSS pBSSList,
			u32 uIELength,
			u8 *pbyIEs,
			void *pRxPacketContext)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	struct vnt_rx_mgmt *pRxPacket =
		(struct vnt_rx_mgmt *)pRxPacketContext;
	int ii, jj;
	signed long ldBm, ldBmSum;
	bool bParsingQuiet = false;

    if (pBSSList == NULL)
        return false;

	pBSSList->qwBSSTimestamp = cpu_to_le64(qwTimestamp);

    pBSSList->wBeaconInterval = cpu_to_le16(wBeaconInterval);
    pBSSList->wCapInfo = cpu_to_le16(wCapInfo);
    pBSSList->uClearCount = 0;
    pBSSList->uChannel = byCurrChannel;

    if (pSSID->len > WLAN_SSID_MAXLEN)
        pSSID->len = WLAN_SSID_MAXLEN;

    if ((pSSID->len != 0) && (pSSID->abySSID[0] != 0))
        memcpy(pBSSList->abySSID, pSSID, pSSID->len + WLAN_IEHDR_LEN);
    memcpy(pBSSList->abySuppRates, pSuppRates,pSuppRates->len + WLAN_IEHDR_LEN);

    if (pExtSuppRates != NULL) {
        memcpy(pBSSList->abyExtSuppRates, pExtSuppRates,pExtSuppRates->len + WLAN_IEHDR_LEN);
    } else {
        memset(pBSSList->abyExtSuppRates, 0, WLAN_IEHDR_LEN + WLAN_RATES_MAXLEN + 1);
    }
    pBSSList->sERP.byERP = psERP->byERP;
    pBSSList->sERP.bERPExist = psERP->bERPExist;

    //                            
    if (pBSSList->uChannel > CB_MAX_CHANNEL_24G) {
        pBSSList->eNetworkTypeInUse = PHY_TYPE_11A;
    } else {
        if (pBSSList->sERP.bERPExist == true) {
            pBSSList->eNetworkTypeInUse = PHY_TYPE_11G;
        } else {
            pBSSList->eNetworkTypeInUse = PHY_TYPE_11B;
        }
    }

    pBSSList->byRxRate = pRxPacket->byRxRate;
    pBSSList->qwLocalTSF = pRxPacket->qwLocalTSF;
    if(bChannelHit)
        pBSSList->uRSSI = pRxPacket->uRSSI;
    pBSSList->bySQ = pRxPacket->bySQ;

   if ((pMgmt->eCurrMode == WMAC_MODE_ESS_STA) &&
        (pMgmt->eCurrState == WMAC_STATE_ASSOC)) {
        //               
        if (pBSSList == pMgmt->pCurrBSS) {
            bParsingQuiet = true;
        }
    }

   WPA_ClearRSN(pBSSList);         //           

   if (pRSNWPA != NULL) {
	unsigned int uLen = pRSNWPA->len + 2;
	if (uLen <= (uIELength -
		     (unsigned int) (u32) ((u8 *) pRSNWPA - pbyIEs))) {
		pBSSList->wWPALen = uLen;
		memcpy(pBSSList->byWPAIE, pRSNWPA, uLen);
		WPA_ParseRSN(pBSSList, pRSNWPA);
	}
   }

   WPA2_ClearRSN(pBSSList);  //           

    if (pRSN != NULL) {
	unsigned int uLen = pRSN->len + 2;
	if (uLen <= (uIELength -
			(unsigned int) (u32) ((u8 *) pRSN - pbyIEs))) {
		pBSSList->wRSNLen = uLen;
		memcpy(pBSSList->byRSNIE, pRSN, uLen);
		WPA2vParseRSN(pBSSList, pRSN);
	}
    }

    if (pRxPacket->uRSSI != 0) {
        RFvRSSITodBm(pDevice, (u8)(pRxPacket->uRSSI), &ldBm);
        //                               
        pBSSList->byRSSIStatCnt++;
        pBSSList->byRSSIStatCnt %= RSSI_STAT_COUNT;
        pBSSList->ldBmAverage[pBSSList->byRSSIStatCnt] = ldBm;
        ldBmSum = 0;
	for (ii = 0, jj = 0; ii < RSSI_STAT_COUNT; ii++) {
		if (pBSSList->ldBmAverage[ii] != 0) {
			pBSSList->ldBmMAX =
				max(pBSSList->ldBmAverage[ii], ldBm);
			ldBmSum +=
				pBSSList->ldBmAverage[ii];
			jj++;
		}
        }
        pBSSList->ldBmAverRange = ldBmSum /jj;
    }

    pBSSList->uIELength = uIELength;
    if (pBSSList->uIELength > WLAN_BEACON_FR_MAXLEN)
        pBSSList->uIELength = WLAN_BEACON_FR_MAXLEN;
    memcpy(pBSSList->abyIEs, pbyIEs, pBSSList->uIELength);

    return true;
}

/* 
  
                       
                                                               
  
                
          
  
 */

int BSSbIsSTAInNodeDB(struct vnt_private *pDevice,
		u8 *abyDstAddr, u32 *puNodeIndex)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	unsigned int ii;

    //                               
    for (ii = 1; ii < (MAX_NODE_NUM + 1); ii++) {
        if (pMgmt->sNodeDBTable[ii].bActive) {
		if (!compare_ether_addr(abyDstAddr,
					pMgmt->sNodeDBTable[ii].abyMACAddr)) {
                *puNodeIndex = ii;
                return true;
            }
        }
    }

   return false;
};

/* 
  
                       
                                                          
                                               
  
                
          
  
 */
void BSSvCreateOneNode(struct vnt_private *pDevice, u32 *puNodeIndex)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	int            ii;
	u32 BigestCount = 0;
	u32 SelectIndex;
	struct sk_buff  *skb;

    //                                             
    //                                                        
    SelectIndex = 1;
    for (ii = 1; ii < (MAX_NODE_NUM + 1); ii++) {
        if (pMgmt->sNodeDBTable[ii].bActive) {
            if (pMgmt->sNodeDBTable[ii].uInActiveCount > BigestCount) {
                BigestCount = pMgmt->sNodeDBTable[ii].uInActiveCount;
                SelectIndex = ii;
            }
        }
        else {
            break;
        }
    }

    //                                                          
    if ( ii == (MAX_NODE_NUM + 1)) {
        *puNodeIndex = SelectIndex;
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "Replace inactive node = %d\n", SelectIndex);
        //                
        if (pMgmt->sNodeDBTable[*puNodeIndex].sTxPSQueue.next != NULL) {
      	    while ((skb = skb_dequeue(&pMgmt->sNodeDBTable[*puNodeIndex].sTxPSQueue)) != NULL)
            dev_kfree_skb(skb);
        }
    }
    else {
        *puNodeIndex = ii;
    }

    memset(&pMgmt->sNodeDBTable[*puNodeIndex], 0, sizeof(KnownNodeDB));
    pMgmt->sNodeDBTable[*puNodeIndex].bActive = true;
    pMgmt->sNodeDBTable[*puNodeIndex].uRatePollTimeout = FALLBACK_POLL_SECOND;
    //                     
    skb_queue_head_init(&pMgmt->sNodeDBTable[*puNodeIndex].sTxPSQueue);
    pMgmt->sNodeDBTable[*puNodeIndex].byAuthSequence = 0;
    pMgmt->sNodeDBTable[*puNodeIndex].wEnQueueCnt = 0;
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "Create node index = %d\n", ii);
};

/* 
  
                       
                              
  
  
                
          
  
 */

void BSSvRemoveOneNode(struct vnt_private *pDevice, u32 uNodeIndex)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	u8 byMask[8] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};
	struct sk_buff  *skb;

    while ((skb = skb_dequeue(&pMgmt->sNodeDBTable[uNodeIndex].sTxPSQueue)) != NULL)
            dev_kfree_skb(skb);
    //              
    memset(&pMgmt->sNodeDBTable[uNodeIndex], 0, sizeof(KnownNodeDB));
    //                 
    pMgmt->abyPSTxMap[pMgmt->sNodeDBTable[uNodeIndex].wAID >> 3] &=  ~byMask[pMgmt->sNodeDBTable[uNodeIndex].wAID & 7];
};
/* 
  
                       
                                                      
  
  
                
          
  
 */

void BSSvUpdateAPNode(struct vnt_private *pDevice, u16 *pwCapInfo,
	PWLAN_IE_SUPP_RATES pSuppRates, PWLAN_IE_SUPP_RATES pExtSuppRates)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	u32 uRateLen = WLAN_RATES_MAXLEN;

    memset(&pMgmt->sNodeDBTable[0], 0, sizeof(KnownNodeDB));

    pMgmt->sNodeDBTable[0].bActive = true;
    if (pDevice->byBBType == BB_TYPE_11B) {
        uRateLen = WLAN_RATES_MAXLEN_11B;
    }
    pMgmt->abyCurrSuppRates[1] = RATEuSetIE((PWLAN_IE_SUPP_RATES)pSuppRates,
                                            (PWLAN_IE_SUPP_RATES)pMgmt->abyCurrSuppRates,
                                            uRateLen);
    pMgmt->abyCurrExtSuppRates[1] = RATEuSetIE((PWLAN_IE_SUPP_RATES)pExtSuppRates,
                                            (PWLAN_IE_SUPP_RATES)pMgmt->abyCurrExtSuppRates,
                                            uRateLen);
    RATEvParseMaxRate((void *) pDevice,
                       (PWLAN_IE_SUPP_RATES)pMgmt->abyCurrSuppRates,
                       (PWLAN_IE_SUPP_RATES)pMgmt->abyCurrExtSuppRates,
                       true,
                       &(pMgmt->sNodeDBTable[0].wMaxBasicRate),
                       &(pMgmt->sNodeDBTable[0].wMaxSuppRate),
                       &(pMgmt->sNodeDBTable[0].wSuppRate),
                       &(pMgmt->sNodeDBTable[0].byTopCCKBasicRate),
                       &(pMgmt->sNodeDBTable[0].byTopOFDMBasicRate)
                      );
    memcpy(pMgmt->sNodeDBTable[0].abyMACAddr, pMgmt->abyCurrBSSID, WLAN_ADDR_LEN);
    pMgmt->sNodeDBTable[0].wTxDataRate = pMgmt->sNodeDBTable[0].wMaxSuppRate;
    pMgmt->sNodeDBTable[0].bShortPreamble = WLAN_GET_CAP_INFO_SHORTPREAMBLE(*pwCapInfo);
    pMgmt->sNodeDBTable[0].uRatePollTimeout = FALLBACK_POLL_SECOND;
    //                                        
    //                    
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pMgmt->sNodeDBTable[0].wTxDataRate = %d \n", pMgmt->sNodeDBTable[0].wTxDataRate);

};

/* 
  
                       
                                                          
  
  
                
          
  
 */

void BSSvAddMulticastNode(struct vnt_private *pDevice)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;

    if (!pDevice->bEnableHostWEP)
        memset(&pMgmt->sNodeDBTable[0], 0, sizeof(KnownNodeDB));
    memset(pMgmt->sNodeDBTable[0].abyMACAddr, 0xff, WLAN_ADDR_LEN);
    pMgmt->sNodeDBTable[0].bActive = true;
    pMgmt->sNodeDBTable[0].bPSEnable = false;
    skb_queue_head_init(&pMgmt->sNodeDBTable[0].sTxPSQueue);
    RATEvParseMaxRate((void *) pDevice,
                      (PWLAN_IE_SUPP_RATES)pMgmt->abyCurrSuppRates,
                      (PWLAN_IE_SUPP_RATES)pMgmt->abyCurrExtSuppRates,
                      true,
                      &(pMgmt->sNodeDBTable[0].wMaxBasicRate),
                      &(pMgmt->sNodeDBTable[0].wMaxSuppRate),
                       &(pMgmt->sNodeDBTable[0].wSuppRate),
                      &(pMgmt->sNodeDBTable[0].byTopCCKBasicRate),
                      &(pMgmt->sNodeDBTable[0].byTopOFDMBasicRate)
                     );
    pMgmt->sNodeDBTable[0].wTxDataRate = pMgmt->sNodeDBTable[0].wMaxBasicRate;
    pMgmt->sNodeDBTable[0].uRatePollTimeout = FALLBACK_POLL_SECOND;

};

/* 
  
                       
  
  
                                                                     
  
  
                
           
  
 */

void BSSvSecondCallBack(struct vnt_private *pDevice)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	int ii;
	PWLAN_IE_SSID pItemSSID, pCurrSSID;
	u32 uSleepySTACnt = 0;
	u32 uNonShortSlotSTACnt = 0;
	u32 uLongPreambleSTACnt = 0;

    spin_lock_irq(&pDevice->lock);

    pDevice->uAssocCount = 0;

    //                          
    if ( pDevice->bEnablePSMode == true ) {
        pDevice->ulPSModeWaitTx++;
        if ( pDevice->ulPSModeWaitTx >= 2 ) {
            pDevice->ulPSModeWaitTx = 0;
            pDevice->bPSModeTxBurst = false;
        }
    }

    pDevice->byERPFlag &=
        ~(WLAN_SET_ERP_BARKER_MODE(1) | WLAN_SET_ERP_NONERP_PRESENT(1));

    if (pDevice->wUseProtectCntDown > 0) {
        pDevice->wUseProtectCntDown --;
    }
    else {
        //                     
        pDevice->byERPFlag &= ~(WLAN_SET_ERP_USE_PROTECTION(1));
    }

if(pDevice->byReAssocCount > 0) {
       pDevice->byReAssocCount++;
   if((pDevice->byReAssocCount > 10) && (pDevice->bLinkPass != true)) {  //              
                     printk("Re-association timeout!!!\n");
		   pDevice->byReAssocCount = 0;
                    //                                         
                        {
                  	union iwreq_data  wrqu;
                  	memset(&wrqu, 0, sizeof (wrqu));
                          wrqu.ap_addr.sa_family = ARPHRD_ETHER;
                  	PRINT_K("wireless_send_event--->SIOCGIWAP(disassociated)\n");
                  	wireless_send_event(pDevice->dev, SIOCGIWAP, &wrqu, NULL);
                       }
     }
   else if(pDevice->bLinkPass == true)
   	pDevice->byReAssocCount = 0;
}

 pMgmt->eLastState = pMgmt->eCurrState ;

	s_uCalculateLinkQual(pDevice);

    for (ii = 0; ii < (MAX_NODE_NUM + 1); ii++) {

        if (pMgmt->sNodeDBTable[ii].bActive) {
            //                             
            pMgmt->sNodeDBTable[ii].uInActiveCount++;

            if (ii > 0) {
                if (pMgmt->sNodeDBTable[ii].uInActiveCount > MAX_INACTIVE_COUNT) {
                    BSSvRemoveOneNode(pDevice, ii);
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO
                        "Inactive timeout [%d] sec, STA index = [%d] remove\n", MAX_INACTIVE_COUNT, ii);
                    continue;
                }

                if (pMgmt->sNodeDBTable[ii].eNodeState >= NODE_ASSOC) {

                    pDevice->uAssocCount++;

                    //                       
                    if (pMgmt->sNodeDBTable[ii].uInActiveCount < ERP_RECOVER_COUNT) {
                        if (!pMgmt->sNodeDBTable[ii].bShortPreamble) {
                            pDevice->byERPFlag |= WLAN_SET_ERP_BARKER_MODE(1);
                            uLongPreambleSTACnt ++;
                        }
                        if (!pMgmt->sNodeDBTable[ii].bERPExist) {
                            pDevice->byERPFlag |= WLAN_SET_ERP_NONERP_PRESENT(1);
                            pDevice->byERPFlag |= WLAN_SET_ERP_USE_PROTECTION(1);
                        }
                        if (!pMgmt->sNodeDBTable[ii].bShortSlotTime)
                            uNonShortSlotSTACnt++;
                    }
                }

                //                            
                if (pMgmt->sNodeDBTable[ii].bPSEnable)
                    uSleepySTACnt++;

            }

            //                    
            if (!pDevice->bFixRate) {
                if (ii > 0) {
                    //                                       
			RATEvTxRateFallBack((void *)pDevice,
					    &(pMgmt->sNodeDBTable[ii]));
                }
                else {
                    //                                                
			if (pMgmt->eCurrMode == WMAC_MODE_ESS_STA)
				RATEvTxRateFallBack((void *)pDevice,
						    &(pMgmt->sNodeDBTable[ii]));
                }

            }

            //                          
            if (pMgmt->sNodeDBTable[ii].wEnQueueCnt != 0) {
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "Index= %d, Queue = %d pending \n",
                           ii, pMgmt->sNodeDBTable[ii].wEnQueueCnt);
                if ((ii >0) && (pMgmt->sNodeDBTable[ii].wEnQueueCnt > 15)) {
                    BSSvRemoveOneNode(pDevice, ii);
                    DBG_PRT(MSG_LEVEL_NOTICE, KERN_INFO "Pending many queues PS STA Index = %d remove \n", ii);
                    continue;
                }
            }
        }

    }

    if ((pMgmt->eCurrMode == WMAC_MODE_ESS_AP) && (pDevice->byBBType == BB_TYPE_11G)) {

        //                    
        if (WLAN_GET_ERP_USE_PROTECTION(pDevice->byERPFlag)) {
            if (!pDevice->bProtectMode) {
                MACvEnableProtectMD(pDevice);
                pDevice->bProtectMode = true;
            }
        }
        else {
            if (pDevice->bProtectMode) {
                MACvDisableProtectMD(pDevice);
                pDevice->bProtectMode = false;
            }
        }
        //                       

        if (uNonShortSlotSTACnt > 0) {
            if (pDevice->bShortSlotTime) {
                pDevice->bShortSlotTime = false;
                BBvSetShortSlotTime(pDevice);
		vUpdateIFS((void *)pDevice);
            }
        }
        else {
            if (!pDevice->bShortSlotTime) {
                pDevice->bShortSlotTime = true;
                BBvSetShortSlotTime(pDevice);
		vUpdateIFS((void *)pDevice);
            }
        }

        //                                 

        if (uLongPreambleSTACnt > 0) {
            if (!pDevice->bBarkerPreambleMd) {
                MACvEnableBarkerPreambleMd(pDevice);
                pDevice->bBarkerPreambleMd = true;
            }
        }
        else {
            if (pDevice->bBarkerPreambleMd) {
                MACvDisableBarkerPreambleMd(pDevice);
                pDevice->bBarkerPreambleMd = false;
            }
        }

    }

    //                                                           
    if (pMgmt->eCurrMode == WMAC_MODE_ESS_AP) {
        if (uSleepySTACnt > 0)
            pMgmt->sNodeDBTable[0].bPSEnable = true;
        else
            pMgmt->sNodeDBTable[0].bPSEnable = false;
    }

    pItemSSID = (PWLAN_IE_SSID)pMgmt->abyDesireSSID;
    pCurrSSID = (PWLAN_IE_SSID)pMgmt->abyCurrSSID;

    if ((pMgmt->eCurrMode == WMAC_MODE_STANDBY) ||
        (pMgmt->eCurrMode == WMAC_MODE_ESS_STA)) {

        if (pMgmt->sNodeDBTable[0].bActive) { //               

            if (pDevice->bUpdateBBVGA) {
		s_vCheckSensitivity(pDevice);
		s_vCheckPreEDThreshold(pDevice);
            }

    	    if ((pMgmt->sNodeDBTable[0].uInActiveCount >= (LOST_BEACON_COUNT/2)) &&
    	        (pDevice->byBBVGACurrent != pDevice->abyBBVGA[0]) ) {
    	        pDevice->byBBVGANew = pDevice->abyBBVGA[0];
		bScheduleCommand((void *) pDevice,
				 WLAN_CMD_CHANGE_BBSENSITIVITY,
				 NULL);
    	    }

        	if (pMgmt->sNodeDBTable[0].uInActiveCount >= LOST_BEACON_COUNT) {
                pMgmt->sNodeDBTable[0].bActive = false;
                pMgmt->eCurrMode = WMAC_MODE_STANDBY;
                pMgmt->eCurrState = WMAC_STATE_IDLE;
                netif_stop_queue(pDevice->dev);
                pDevice->bLinkPass = false;
                ControlvMaskByte(pDevice,MESSAGE_REQUEST_MACREG,MAC_REG_PAPEDELAY,LEDSTS_STS,LEDSTS_SLOW);
                pDevice->bRoaming = true;
                pDevice->bIsRoaming = false;

                DBG_PRT(MSG_LEVEL_NOTICE, KERN_INFO "Lost AP beacon [%d] sec, disconnected !\n", pMgmt->sNodeDBTable[0].uInActiveCount);
		/*                                           */
      {
	union iwreq_data  wrqu;
	memset(&wrqu, 0, sizeof (wrqu));
        wrqu.ap_addr.sa_family = ARPHRD_ETHER;
	PRINT_K("wireless_send_event--->SIOCGIWAP(disassociated)\n");
	wireless_send_event(pDevice->dev, SIOCGIWAP, &wrqu, NULL);
     }
            }
        }
        else if (pItemSSID->len != 0) {
//         
      if ((pDevice->bEnableRoaming == true)&&(!(pMgmt->Cisco_cckm))) {
DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "bRoaming %d, !\n", pDevice->bRoaming );
DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "bIsRoaming %d, !\n", pDevice->bIsRoaming );
          if ((pDevice->bRoaming == true)&&(pDevice->bIsRoaming == true)){
	    	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "Fast   Roaming ...\n");
		BSSvClearBSSList((void *) pDevice, pDevice->bLinkPass);
		bScheduleCommand((void *) pDevice,
				 WLAN_CMD_BSSID_SCAN,
				 pMgmt->abyDesireSSID);
		bScheduleCommand((void *) pDevice,
				 WLAN_CMD_SSID,
				 pMgmt->abyDesireSSID);
                pDevice->uAutoReConnectTime = 0;
                pDevice->uIsroamingTime = 0;
                pDevice->bRoaming = false;
          }
      else if ((pDevice->bRoaming == false)&&(pDevice->bIsRoaming == true)) {
                            pDevice->uIsroamingTime++;
       if (pDevice->uIsroamingTime >= 20)
            pDevice->bIsRoaming = false;
         }

   }
else {
            if (pDevice->uAutoReConnectTime < 10) {
                pDevice->uAutoReConnectTime++;
                //                                                   
                if(pDevice->bWPASuppWextEnabled ==true)
		 pDevice->uAutoReConnectTime = 0;
            }
            else {
	    //                                               
	      if(pDevice->bWPADEVUp)
	          pDevice->eEncryptionStatus = pDevice->eOldEncryptionStatus;

                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "Roaming ...\n");
		BSSvClearBSSList((void *) pDevice, pDevice->bLinkPass);
		pMgmt->eScanType = WMAC_SCAN_ACTIVE;
		bScheduleCommand((void *) pDevice,
				 WLAN_CMD_BSSID_SCAN,
				 pMgmt->abyDesireSSID);
		bScheduleCommand((void *) pDevice,
				 WLAN_CMD_SSID,
				 pMgmt->abyDesireSSID);
                pDevice->uAutoReConnectTime = 0;
            }
        }
    }
    }

    if (pMgmt->eCurrMode == WMAC_MODE_IBSS_STA) {
        //                                                         
        if ((pMgmt->eCurrState == WMAC_STATE_STARTED) && (pCurrSSID->len == 0)) {
            if (pDevice->uAutoReConnectTime < 10) {
                pDevice->uAutoReConnectTime++;
            }
            else {
                DBG_PRT(MSG_LEVEL_NOTICE, KERN_INFO "Adhoc re-scanning ...\n");
	       pMgmt->eScanType = WMAC_SCAN_ACTIVE;
		bScheduleCommand((void *) pDevice, WLAN_CMD_BSSID_SCAN, NULL);
		bScheduleCommand((void *) pDevice, WLAN_CMD_SSID, NULL);
                pDevice->uAutoReConnectTime = 0;
            };
        }
        if (pMgmt->eCurrState == WMAC_STATE_JOINTED) {

		if (pDevice->bUpdateBBVGA) {
			s_vCheckSensitivity(pDevice);
			s_vCheckPreEDThreshold(pDevice);
		}
        	if (pMgmt->sNodeDBTable[0].uInActiveCount >=ADHOC_LOST_BEACON_COUNT) {
        	    DBG_PRT(MSG_LEVEL_NOTICE, KERN_INFO "Lost other STA beacon [%d] sec, started !\n", pMgmt->sNodeDBTable[0].uInActiveCount);
                pMgmt->sNodeDBTable[0].uInActiveCount = 0;
                pMgmt->eCurrState = WMAC_STATE_STARTED;
                netif_stop_queue(pDevice->dev);
                pDevice->bLinkPass = false;
                ControlvMaskByte(pDevice,MESSAGE_REQUEST_MACREG,MAC_REG_PAPEDELAY,LEDSTS_STS,LEDSTS_SLOW);
            }
        }
    }

    if (pDevice->bLinkPass == true) {
        if (netif_queue_stopped(pDevice->dev))
            netif_wake_queue(pDevice->dev);
    }

    spin_unlock_irq(&pDevice->lock);

    pMgmt->sTimerSecondCallback.expires = RUN_AT(HZ);
    add_timer(&pMgmt->sTimerSecondCallback);
}

/* 
  
                       
  
  
                                                    
  
  
                
           
  
 */

void BSSvUpdateNodeTxCounter(struct vnt_private *pDevice,
	PSStatCounter pStatistic, u8 byTSR, u8 byPktNO)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	u32 uNodeIndex = 0;
	u8 byTxRetry;
	u16 wRate;
	u16 wFallBackRate = RATE_1M;
	u8 byFallBack;
	int ii;
	u8 *pbyDestAddr;
	u8 byPktNum;
	u16 wFIFOCtl;

    byPktNum = (byPktNO & 0x0F) >> 4;
    byTxRetry = (byTSR & 0xF0) >> 4;
    wRate = (u16) (byPktNO & 0xF0) >> 4;
    wFIFOCtl = pStatistic->abyTxPktInfo[byPktNum].wFIFOCtl;
    pbyDestAddr = (u8 *) &( pStatistic->abyTxPktInfo[byPktNum].abyDestAddr[0]);

    if (wFIFOCtl & FIFOCTL_AUTO_FB_0) {
        byFallBack = AUTO_FB_0;
    } else if (wFIFOCtl & FIFOCTL_AUTO_FB_1) {
        byFallBack = AUTO_FB_1;
    } else {
        byFallBack = AUTO_FB_NONE;
    }

    //                                 
    if (wFIFOCtl & FIFOCTL_NEEDACK) {
        if (pMgmt->eCurrMode == WMAC_MODE_ESS_STA) {
            pMgmt->sNodeDBTable[0].uTxAttempts += 1;
            if ( !(byTSR & (TSR_TMO | TSR_RETRYTMO))) {
                //                                               
                pMgmt->sNodeDBTable[0].uTxOk[MAX_RATE]++;
                if ( (byFallBack == AUTO_FB_NONE) ||
                     (wRate < RATE_18M) ) {
                    wFallBackRate = wRate;
                } else if (byFallBack == AUTO_FB_0) {
                    if (byTxRetry < 5)
                        wFallBackRate = awHWRetry0[wRate-RATE_18M][byTxRetry];
                    else
                        wFallBackRate = awHWRetry0[wRate-RATE_18M][4];
                } else if (byFallBack == AUTO_FB_1) {
                    if (byTxRetry < 5)
                        wFallBackRate = awHWRetry1[wRate-RATE_18M][byTxRetry];
                    else
                        wFallBackRate = awHWRetry1[wRate-RATE_18M][4];
                }
                pMgmt->sNodeDBTable[0].uTxOk[wFallBackRate]++;
            } else {
                pMgmt->sNodeDBTable[0].uTxFailures ++;
            }
            pMgmt->sNodeDBTable[0].uTxRetry += byTxRetry;
            if (byTxRetry != 0) {
                pMgmt->sNodeDBTable[0].uTxFail[MAX_RATE]+=byTxRetry;
                if ( (byFallBack == AUTO_FB_NONE) ||
                     (wRate < RATE_18M) ) {
                    pMgmt->sNodeDBTable[0].uTxFail[wRate]+=byTxRetry;
                } else if (byFallBack == AUTO_FB_0) {
			for (ii = 0; ii < byTxRetry; ii++) {
				if (ii < 5)
					wFallBackRate =
						awHWRetry0[wRate-RATE_18M][ii];
				else
					wFallBackRate =
						awHWRetry0[wRate-RATE_18M][4];
				pMgmt->sNodeDBTable[0].uTxFail[wFallBackRate]++;
			}
                } else if (byFallBack == AUTO_FB_1) {
			for (ii = 0; ii < byTxRetry; ii++) {
				if (ii < 5)
					wFallBackRate =
						awHWRetry1[wRate-RATE_18M][ii];
				else
					wFallBackRate =
						awHWRetry1[wRate-RATE_18M][4];
				pMgmt->sNodeDBTable[0].uTxFail[wFallBackRate]++;
			}
                }
            }
        }

	if ((pMgmt->eCurrMode == WMAC_MODE_IBSS_STA) ||
            (pMgmt->eCurrMode == WMAC_MODE_ESS_AP)) {

		if (BSSbIsSTAInNodeDB((void *) pDevice,
				      pbyDestAddr,
				      &uNodeIndex)) {
			pMgmt->sNodeDBTable[uNodeIndex].uTxAttempts += 1;
                if ( !(byTSR & (TSR_TMO | TSR_RETRYTMO))) {
                    //                                               
                    pMgmt->sNodeDBTable[uNodeIndex].uTxOk[MAX_RATE]++;
                    if ( (byFallBack == AUTO_FB_NONE) ||
                         (wRate < RATE_18M) ) {
                        wFallBackRate = wRate;
                    } else if (byFallBack == AUTO_FB_0) {
                        if (byTxRetry < 5)
                            wFallBackRate = awHWRetry0[wRate-RATE_18M][byTxRetry];
                        else
                            wFallBackRate = awHWRetry0[wRate-RATE_18M][4];
                    } else if (byFallBack == AUTO_FB_1) {
                        if (byTxRetry < 5)
                            wFallBackRate = awHWRetry1[wRate-RATE_18M][byTxRetry];
                        else
                            wFallBackRate = awHWRetry1[wRate-RATE_18M][4];
                    }
                    pMgmt->sNodeDBTable[uNodeIndex].uTxOk[wFallBackRate]++;
                } else {
                    pMgmt->sNodeDBTable[uNodeIndex].uTxFailures ++;
                }
                pMgmt->sNodeDBTable[uNodeIndex].uTxRetry += byTxRetry;
                if (byTxRetry != 0) {
                    pMgmt->sNodeDBTable[uNodeIndex].uTxFail[MAX_RATE]+=byTxRetry;
                    if ( (byFallBack == AUTO_FB_NONE) ||
                         (wRate < RATE_18M) ) {
                        pMgmt->sNodeDBTable[uNodeIndex].uTxFail[wRate]+=byTxRetry;
                    } else if (byFallBack == AUTO_FB_0) {
			for (ii = 0; ii < byTxRetry; ii++) {
				if (ii < 5)
					wFallBackRate =
						awHWRetry0[wRate-RATE_18M][ii];
				else
					wFallBackRate =
						awHWRetry0[wRate-RATE_18M][4];
				pMgmt->sNodeDBTable[uNodeIndex].uTxFail[wFallBackRate]++;
                        }
                    } else if (byFallBack == AUTO_FB_1) {
		      for (ii = 0; ii < byTxRetry; ii++) {
			if (ii < 5)
                                wFallBackRate = awHWRetry1[wRate-RATE_18M][ii];
			else
                                wFallBackRate = awHWRetry1[wRate-RATE_18M][4];
			pMgmt->sNodeDBTable[uNodeIndex].uTxFail[wFallBackRate]++;
		      }
                    }
                }
            }
        }
    }
}

/* 
  
                       
                                   
  
  
              
       
                                                    
                                              
        
            
  
                
           
  
 */

void BSSvClearNodeDBTable(struct vnt_private *pDevice, u32 uStartIndex)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	struct sk_buff  *skb;
	int ii;

    for (ii = uStartIndex; ii < (MAX_NODE_NUM + 1); ii++) {
        if (pMgmt->sNodeDBTable[ii].bActive) {
            //                                     
            if (pMgmt->sNodeDBTable[ii].sTxPSQueue.next != NULL) {
                while ((skb = skb_dequeue(&pMgmt->sNodeDBTable[ii].sTxPSQueue)) != NULL){
                        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "PS skb != NULL %d\n", ii);
                        dev_kfree_skb(skb);
                }
            }
            memset(&pMgmt->sNodeDBTable[ii], 0, sizeof(KnownNodeDB));
        }
    }
};

static void s_vCheckSensitivity(struct vnt_private *pDevice)
{
	PKnownBSS pBSSList = NULL;
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	int ii;

    if ((pMgmt->eCurrState == WMAC_STATE_ASSOC) ||
        ((pMgmt->eCurrMode == WMAC_MODE_IBSS_STA) && (pMgmt->eCurrState == WMAC_STATE_JOINTED))) {
        pBSSList = BSSpAddrIsInBSSList(pDevice, pMgmt->abyCurrBSSID, (PWLAN_IE_SSID)pMgmt->abyCurrSSID);
        if (pBSSList != NULL) {
		/*                                          */
		signed long    LocalldBmAverage = 0;
		signed long    uNumofdBm = 0;
            for (ii = 0; ii < RSSI_STAT_COUNT; ii++) {
                if (pBSSList->ldBmAverage[ii] != 0) {
                    uNumofdBm ++;
                    LocalldBmAverage += pBSSList->ldBmAverage[ii];
                }
            }
            if (uNumofdBm > 0) {
                LocalldBmAverage = LocalldBmAverage/uNumofdBm;
                for (ii=0;ii<BB_VGA_LEVEL;ii++) {
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"LocalldBmAverage:%ld, %ld %02x\n", LocalldBmAverage, pDevice->ldBmThreshold[ii], pDevice->abyBBVGA[ii]);
                    if (LocalldBmAverage < pDevice->ldBmThreshold[ii]) {
                	    pDevice->byBBVGANew = pDevice->abyBBVGA[ii];
                        break;
                    }
                }
                if (pDevice->byBBVGANew != pDevice->byBBVGACurrent) {
                    pDevice->uBBVGADiffCount++;
                    if (pDevice->uBBVGADiffCount >= BB_VGA_CHANGE_THRESHOLD)
			bScheduleCommand(pDevice,
					 WLAN_CMD_CHANGE_BBSENSITIVITY,
					 NULL);
                } else {
                    pDevice->uBBVGADiffCount = 0;
                }
            }
        }
    }
}

static void s_uCalculateLinkQual(struct vnt_private *pDevice)
{
	unsigned long TxOkRatio, TxCnt;
	unsigned long RxOkRatio, RxCnt;
	unsigned long RssiRatio;
	long ldBm;

TxCnt = pDevice->scStatistic.TxNoRetryOkCount +
	      pDevice->scStatistic.TxRetryOkCount +
	      pDevice->scStatistic.TxFailCount;
RxCnt = pDevice->scStatistic.RxFcsErrCnt +
	      pDevice->scStatistic.RxOkCnt;
TxOkRatio = (TxCnt < 6) ? 4000:((pDevice->scStatistic.TxNoRetryOkCount * 4000) / TxCnt);
RxOkRatio = (RxCnt < 6) ? 2000:((pDevice->scStatistic.RxOkCnt * 2000) / RxCnt);
//                   
if(pDevice->bLinkPass !=true)
{
   pDevice->scStatistic.LinkQuality = 0;
   pDevice->scStatistic.SignalStren = 0;
}
else
{
   RFvRSSITodBm(pDevice, (u8)(pDevice->uCurrRSSI), &ldBm);
   if(-ldBm < 50)  {
   	RssiRatio = 4000;
     }
   else if(-ldBm > 90) {
   	RssiRatio = 0;
     }
   else {
   	RssiRatio = (40-(-ldBm-50))*4000/40;
     }
   pDevice->scStatistic.SignalStren = RssiRatio/40;
   pDevice->scStatistic.LinkQuality = (RssiRatio+TxOkRatio+RxOkRatio)/100;
}
   pDevice->scStatistic.RxFcsErrCnt = 0;
   pDevice->scStatistic.RxOkCnt = 0;
   pDevice->scStatistic.TxFailCount = 0;
   pDevice->scStatistic.TxNoRetryOkCount = 0;
   pDevice->scStatistic.TxRetryOkCount = 0;
}

void BSSvClearAnyBSSJoinRecord(struct vnt_private *pDevice)
{
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;
	int ii;

	for (ii = 0; ii < MAX_BSS_NUM; ii++)
		pMgmt->sBSSList[ii].bSelected = false;

	return;
}

static void s_vCheckPreEDThreshold(struct vnt_private *pDevice)
{
	PKnownBSS pBSSList = NULL;
	struct vnt_manager *pMgmt = &pDevice->vnt_mgmt;

    if ((pMgmt->eCurrState == WMAC_STATE_ASSOC) ||
        ((pMgmt->eCurrMode == WMAC_MODE_IBSS_STA) && (pMgmt->eCurrState == WMAC_STATE_JOINTED))) {
        pBSSList = BSSpAddrIsInBSSList(pDevice, pMgmt->abyCurrBSSID, (PWLAN_IE_SSID)pMgmt->abyCurrSSID);
        if (pBSSList != NULL) {
            pDevice->byBBPreEDRSSI = (u8) (~(pBSSList->ldBmAverRange) + 1);
            BBvUpdatePreEDThreshold(pDevice, false);
        }
    }
}

