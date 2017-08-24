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
 *
 * File: wmgr.h
 *
 * Purpose:
 *
 * Author: lyndon chen
 *
 * Date: Jan 2, 2003
 *
 * Functions:
 *
 * Revision History:
 *
 */

#ifndef __WMGR_H__
#define __WMGR_H__

#include "80211mgr.h"
#include "80211hdr.h"
#include "wcmd.h"
#include "bssdb.h"
#include "wpa2.h"
#include "card.h"

//          
#define PROBE_DELAY                  100  //     
#define SWITCH_CHANNEL_DELAY         200 //     
#define WLAN_SCAN_MINITIME           25   //     
#define WLAN_SCAN_MAXTIME            100  //     
#define TRIVIAL_SYNC_DIFFERENCE      0    //     
#define DEFAULT_IBSS_BI              100  //     

#define WCMD_ACTIVE_SCAN_TIME   20 //    
#define WCMD_PASSIVE_SCAN_TIME  100 //    

#define DEFAULT_MSDU_LIFETIME           512  //   
#define DEFAULT_MSDU_LIFETIME_RES_64us  8000 //     

#define DEFAULT_MGN_LIFETIME            8    //   
#define DEFAULT_MGN_LIFETIME_RES_64us   125  //     

#define MAKE_BEACON_RESERVED            10  //    

#define TIM_MULTICAST_MASK           0x01
#define TIM_BITMAPOFFSET_MASK        0xFE
#define DEFAULT_DTIM_PERIOD             1

#define AP_LONG_RETRY_LIMIT             4

#define DEFAULT_IBSS_CHANNEL            6  //    

//                                                      
#define timer_expire(timer, next_tick) mod_timer(&timer, RUN_AT(next_tick))

typedef void (*TimerFunction)(unsigned long);

//                

typedef u8 NDIS_802_11_MAC_ADDRESS[ETH_ALEN];
typedef struct _NDIS_802_11_AI_REQFI
{
	u16 Capabilities;
	u16 ListenInterval;
    NDIS_802_11_MAC_ADDRESS  CurrentAPAddress;
} NDIS_802_11_AI_REQFI, *PNDIS_802_11_AI_REQFI;

typedef struct _NDIS_802_11_AI_RESFI
{
	u16 Capabilities;
	u16 StatusCode;
	u16 AssociationId;
} NDIS_802_11_AI_RESFI, *PNDIS_802_11_AI_RESFI;

typedef struct _NDIS_802_11_ASSOCIATION_INFORMATION
{
	u32 Length;
	u16 AvailableRequestFixedIEs;
	NDIS_802_11_AI_REQFI RequestFixedIEs;
	u32 RequestIELength;
	u32 OffsetRequestIEs;
	u16 AvailableResponseFixedIEs;
	NDIS_802_11_AI_RESFI ResponseFixedIEs;
	u32 ResponseIELength;
	u32 OffsetResponseIEs;
} NDIS_802_11_ASSOCIATION_INFORMATION, *PNDIS_802_11_ASSOCIATION_INFORMATION;

typedef struct tagSAssocInfo {
	NDIS_802_11_ASSOCIATION_INFORMATION AssocInfo;
	u8 abyIEs[WLAN_BEACON_FR_MAXLEN+WLAN_BEACON_FR_MAXLEN];
	/*                                                        */
	u32 RequestIELength;
	u8 abyReqIEs[WLAN_BEACON_FR_MAXLEN];
} SAssocInfo, *PSAssocInfo;

typedef enum tagWMAC_AUTHENTICATION_MODE {

    WMAC_AUTH_OPEN,
    WMAC_AUTH_SHAREKEY,
    WMAC_AUTH_AUTO,
    WMAC_AUTH_WPA,
    WMAC_AUTH_WPAPSK,
    WMAC_AUTH_WPANONE,
    WMAC_AUTH_WPA2,
    WMAC_AUTH_WPA2PSK,
    WMAC_AUTH_MAX       //                                        
} WMAC_AUTHENTICATION_MODE, *PWMAC_AUTHENTICATION_MODE;

//                              

typedef enum tagWMAC_CONFIG_MODE {
    WMAC_CONFIG_ESS_STA,
    WMAC_CONFIG_IBSS_STA,
    WMAC_CONFIG_AUTO,
    WMAC_CONFIG_AP

} WMAC_CONFIG_MODE, *PWMAC_CONFIG_MODE;

typedef enum tagWMAC_SCAN_TYPE {

    WMAC_SCAN_ACTIVE,
    WMAC_SCAN_PASSIVE,
    WMAC_SCAN_HYBRID

} WMAC_SCAN_TYPE, *PWMAC_SCAN_TYPE;

typedef enum tagWMAC_SCAN_STATE {

    WMAC_NO_SCANNING,
    WMAC_IS_SCANNING,
    WMAC_IS_PROBEPENDING

} WMAC_SCAN_STATE, *PWMAC_SCAN_STATE;

//       
//                                                
//                                                               
//                                                                           
//                                                                             
//                                                          
//                                                 
//                                                       
//                                              

typedef enum tagWMAC_BSS_STATE {

    WMAC_STATE_IDLE,
    WMAC_STATE_STARTED,
    WMAC_STATE_JOINTED,
    WMAC_STATE_AUTHPENDING,
    WMAC_STATE_AUTH,
    WMAC_STATE_ASSOCPENDING,
    WMAC_STATE_ASSOC

} WMAC_BSS_STATE, *PWMAC_BSS_STATE;

//                           
typedef enum tagWMAC_CURRENT_MODE {

    WMAC_MODE_STANDBY,
    WMAC_MODE_ESS_STA,
    WMAC_MODE_IBSS_STA,
    WMAC_MODE_ESS_AP

} WMAC_CURRENT_MODE, *PWMAC_CURRENT_MODE;

typedef enum tagWMAC_POWER_MODE {

    WMAC_POWER_CAM,
    WMAC_POWER_FAST,
    WMAC_POWER_MAX

} WMAC_POWER_MODE, *PWMAC_POWER_MODE;

/*                                 */
struct vnt_tx_mgmt {
	PUWLAN_80211HDR p80211Header;
	u32 cbMPDULen;
	u32 cbPayloadLen;
};

/*                                 */
struct vnt_rx_mgmt {
	PUWLAN_80211HDR p80211Header;
	u64 qwLocalTSF;
	u32 cbMPDULen;
	u32 cbPayloadLen;
	u32 uRSSI;
	u8 bySQ;
	u8 byRxRate;
	u8 byRxChannel;
};

struct vnt_manager {
	void *pAdapter;

	/*             */
	u8  abyMACAddr[WLAN_ADDR_LEN];

	/*                    */
	WMAC_CONFIG_MODE eConfigMode; /*                       */

	CARD_PHY_TYPE eCurrentPHYMode;

	/*                           */
	WMAC_CURRENT_MODE eCurrMode; /*                             */
	WMAC_BSS_STATE eCurrState; /*                       */
	WMAC_BSS_STATE eLastState; /*                    */

	PKnownBSS pCurrBSS;
	u8 byCSSGK;
	u8 byCSSPK;

	int bCurrBSSIDFilterOn;

	/*                    */
	u32 uCurrChannel;
	u8 abyCurrSuppRates[WLAN_IEHDR_LEN + WLAN_RATES_MAXLEN + 1];
	u8 abyCurrExtSuppRates[WLAN_IEHDR_LEN + WLAN_RATES_MAXLEN + 1];
	u8 abyCurrSSID[WLAN_IEHDR_LEN + WLAN_SSID_MAXLEN + 1];
	u8 abyCurrBSSID[WLAN_BSSID_LEN];
	u16 wCurrCapInfo;
	u16 wCurrAID;
	u32 uRSSITrigger;
	u16 wCurrATIMWindow;
	u16 wCurrBeaconPeriod;
	int bIsDS;
	u8 byERPContext;

	CMD_STATE eCommandState;
	u32 uScanChannel;

	/*                          */
	u8 abyDesireSSID[WLAN_IEHDR_LEN + WLAN_SSID_MAXLEN + 1];
	u8 abyDesireBSSID[WLAN_BSSID_LEN];

	/*                                 */
	u8 abyAdHocSSID[WLAN_IEHDR_LEN + WLAN_SSID_MAXLEN + 1];

	/*                                */
	u16 wIBSSBeaconPeriod;
	u16 wIBSSATIMWindow;
	u32 uIBSSChannel;
	u8 abyIBSSSuppRates[WLAN_IEHDR_LEN + WLAN_RATES_MAXLEN + 1];
	u8 byAPBBType;
	u8 abyWPAIE[MAX_WPA_IE_LEN];
	u16 wWPAIELen;

	u32 uAssocCount;
	int bMoreData;

	/*                 */
	WMAC_SCAN_STATE eScanState;
	WMAC_SCAN_TYPE eScanType;
	u32 uScanStartCh;
	u32 uScanEndCh;
	u16 wScanSteps;
	u32 uScanBSSType;
	/*                     */
	u8 abyScanSSID[WLAN_IEHDR_LEN + WLAN_SSID_MAXLEN + 1];
	u8 abyScanBSSID[WLAN_BSSID_LEN];

	/*         */
	WMAC_AUTHENTICATION_MODE eAuthenMode;
	int bShareKeyAlgorithm;
	u8 abyChallenge[WLAN_CHALLENGE_LEN];
	int bPrivacyInvoked;

	/*                            */
	int bInTIM;
	int bMulticastTIM;
	u8 byDTIMCount;
	u8 byDTIMPeriod;

	/*                         */
	WMAC_POWER_MODE ePSMode;
	u16 wListenInterval;
	u16 wCountToWakeUp;
	int bInTIMWake;
	u8 *pbyPSPacketPool;
	u8 byPSPacketPool[sizeof(struct vnt_tx_mgmt)
		+ WLAN_NULLDATA_FR_MAXLEN];
	int bRxBeaconInTBTTWake;
	u8 abyPSTxMap[MAX_NODE_NUM + 1];

	/*                            */
	u32 uCmdBusy;
	u32 uCmdHostAPBusy;

	/*                        */
	u8 *pbyMgmtPacketPool;
	u8 byMgmtPacketPool[sizeof(struct vnt_tx_mgmt)
		+ WLAN_A3FR_MAXLEN];

	/*                           */
	struct timer_list sTimerSecondCallback;

	/*                                       */
	struct vnt_rx_mgmt sRxPacket;

	/*                                         */
	KnownBSS sBSSList[MAX_BSS_NUM];
	/*                         */
	KnownBSS pSameBSS[6];
	int Cisco_cckm;
	u8 Roam_dbm;

	/*                          */
	/*                                                    */
	/*                                                              */
	KnownNodeDB sNodeDBTable[MAX_NODE_NUM + 1];

	/*                  */
	SPMKIDCache gsPMKIDCache;
	int bRoaming;

	/*                */
	SAssocInfo sAssocInfo;

	/*             */
	int b11hEnable;
	int bSwitchChannel;
	u8 byNewChannel;
	PWLAN_IE_MEASURE_REP    pCurrMeasureEIDRep;
	u32 uLengthOfRepEIDs;
	u8 abyCurrentMSRReq[sizeof(struct vnt_tx_mgmt)
		+ WLAN_A3FR_MAXLEN];
	u8 abyCurrentMSRRep[sizeof(struct vnt_tx_mgmt)
		+ WLAN_A3FR_MAXLEN];
	u8 abyIECountry[WLAN_A3FR_MAXLEN];
	u8 abyIBSSDFSOwner[6];
	u8 byIBSSDFSRecovery;

	struct sk_buff skb;

};

void vMgrObjectInit(struct vnt_private *pDevice);

void vMgrAssocBeginSta(struct vnt_private *pDevice,
		struct vnt_manager *, PCMD_STATUS pStatus);

void vMgrReAssocBeginSta(struct vnt_private *pDevice,
		struct vnt_manager *, PCMD_STATUS pStatus);

void vMgrDisassocBeginSta(struct vnt_private *pDevice,
	struct vnt_manager *, u8 *abyDestAddress, u16 wReason,
	PCMD_STATUS pStatus);

void vMgrAuthenBeginSta(struct vnt_private *pDevice,
	struct vnt_manager *, PCMD_STATUS pStatus);

void vMgrCreateOwnIBSS(struct vnt_private *pDevice,
	PCMD_STATUS pStatus);

void vMgrJoinBSSBegin(struct vnt_private *pDevice,
	PCMD_STATUS pStatus);

void vMgrRxManagePacket(struct vnt_private *pDevice,
	struct vnt_manager *, struct vnt_rx_mgmt *);

/*
    
              
                           
                        
      
*/

void vMgrDeAuthenBeginSta(struct vnt_private *pDevice,
	struct vnt_manager *, u8 *abyDestAddress, u16 wReason,
	PCMD_STATUS pStatus);

int bMgrPrepareBeaconToSend(struct vnt_private *pDevice,
	struct vnt_manager *);

int bAdd_PMKID_Candidate(struct vnt_private *pDevice,
	u8 *pbyBSSID, PSRSNCapObject psRSNCapObj);

void vFlush_PMKID_Candidate(struct vnt_private *pDevice);

#endif /*            */
