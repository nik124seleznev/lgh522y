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
 * File: device.h
 *
 * Purpose: MAC Data structure
 *
 * Author: Tevin Chen
 *
 * Date: Mar 17, 1997
 *
 */

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/etherdevice.h>
#include <linux/suspend.h>
#include <linux/if_arp.h>
#include <linux/wireless.h>
#include <net/iw_handler.h>
#include <net/cfg80211.h>
#include <linux/timer.h>
#include <linux/usb.h>

#ifdef SIOCETHTOOL
#define DEVICE_ETHTOOL_IOCTL_SUPPORT
#include <linux/ethtool.h>
#else
#undef DEVICE_ETHTOOL_IOCTL_SUPPORT
#endif

/*                                                   */
#define RT_INSMOD_EVENT_FLAG                             0x0101
#define RT_UPDEV_EVENT_FLAG                               0x0102
#define RT_DISCONNECTED_EVENT_FLAG               0x0103
#define RT_WPACONNECTED_EVENT_FLAG             0x0104
#define RT_DOWNDEV_EVENT_FLAG                        0x0105
#define RT_RMMOD_EVENT_FLAG                              0x0106

/*
                  
 */

#include "device_cfg.h"
#include "80211hdr.h"
#include "tether.h"
#include "wmgr.h"
#include "wcmd.h"
#include "mib.h"
#include "srom.h"
#include "rc4.h"
#include "desc.h"
#include "key.h"
#include "card.h"

#define VNT_USB_VENDOR_ID                     0x160a
#define VNT_USB_PRODUCT_ID                    0x3184

#define MAC_MAX_CONTEXT_REG     (256+128)

#define MAX_MULTICAST_ADDRESS_NUM       32
#define MULTICAST_ADDRESS_LIST_SIZE     (MAX_MULTICAST_ADDRESS_NUM * ETH_ALEN)

#define DUPLICATE_RX_CACHE_LENGTH       5

#define NUM_KEY_ENTRY                   11

#define TX_WEP_NONE                     0
#define TX_WEP_OTF                      1
#define TX_WEP_SW                       2
#define TX_WEP_SWOTP                    3
#define TX_WEP_OTPSW                    4
#define TX_WEP_SW232                    5

#define KEYSEL_WEP40                    0
#define KEYSEL_WEP104                   1
#define KEYSEL_TKIP                     2
#define KEYSEL_CCMP                     3

#define AUTO_FB_NONE            0
#define AUTO_FB_0               1
#define AUTO_FB_1               2

#define FB_RATE0                0
#define FB_RATE1                1

/*              */
#define ANT_A                   0
#define ANT_B                   1
#define ANT_DIVERSITY           2
#define ANT_RXD_TXA             3
#define ANT_RXD_TXB             4
#define ANT_UNKNOWN             0xFF
#define ANT_TXA                 0
#define ANT_TXB                 1
#define ANT_RXA                 2
#define ANT_RXB                 3

#define MAXCHECKHANGCNT         4

/*             */
#define TX_PKT_UNI              0x00
#define TX_PKT_MULTI            0x01
#define TX_PKT_BROAD            0x02

#define BB_VGA_LEVEL            4
#define BB_VGA_CHANGE_THRESHOLD 3

#ifndef RUN_AT
#define RUN_AT(x)                       (jiffies+(x))
#endif

/*             */
#define RESERV_AC0DMA                   4

#define PRIVATE_Message                 0

#define DBG_PRT(l, p, args...) { if (l <= msglevel) printk(p, ##args); }
#define PRINT_K(p, args...) { if (PRIVATE_Message) printk(p, ##args); }

typedef enum __device_msg_level {
	MSG_LEVEL_ERR = 0,            /*                                   */
	MSG_LEVEL_NOTICE = 1,         /*                                  */
	MSG_LEVEL_INFO = 2,           /*                 */
	MSG_LEVEL_VERBOSE = 3,        /*                                */
	MSG_LEVEL_DEBUG = 4           /*                         */
} DEVICE_MSG_LEVEL, *PDEVICE_MSG_LEVEL;

typedef enum __device_init_type {
	DEVICE_INIT_COLD = 0,       /*           */
	DEVICE_INIT_RESET,          /*                                     */
	DEVICE_INIT_DXPL            /*                          */
} DEVICE_INIT_TYPE, *PDEVICE_INIT_TYPE;

/*     */

/*
                                       
 */
typedef enum _CONTEXT_TYPE {
    CONTEXT_DATA_PACKET = 1,
    CONTEXT_MGMT_PACKET
} CONTEXT_TYPE;

/*                             */
typedef struct _RCB
{
	void *Next;
	signed long Ref;
	void *pDevice;
	struct urb *pUrb;
	struct vnt_rx_mgmt sMngPacket;
	struct sk_buff *skb;
	int bBoolInUse;

} RCB, *PRCB;

/*                             */
typedef struct _USB_SEND_CONTEXT {
    void *pDevice;
    struct sk_buff *pPacket;
    struct urb      *pUrb;
    unsigned int            uBufLen;
    CONTEXT_TYPE    Type;
    struct ethhdr sEthHeader;
    void *Next;
    bool            bBoolInUse;
    unsigned char           Data[MAX_TOTAL_SIZE_WITH_ALL_HEADERS];
} USB_SEND_CONTEXT, *PUSB_SEND_CONTEXT;

/*                                                                        */
typedef struct _DEFAULT_CONFIG {
	signed int    ZoneType;
	signed int    eConfigMode;
	signed int    eAuthenMode;        /*              */
	signed int    bShareKeyAlgorithm; /*                               */
	signed int    keyidx;             /*              */
	signed int    eEncryptionStatus;
} DEFAULT_CONFIG, *PDEFAULT_CONFIG;

/*
                                                   
 */
typedef struct {
    unsigned int            uDataLen;
    u8 *           pDataBuf;
  /*                   */
    bool            bInUse;
} INT_BUFFER, *PINT_BUFFER;

/*                   */
typedef enum _VIA_BB_TYPE
{
    BB_TYPE_11A = 0,
    BB_TYPE_11B,
    BB_TYPE_11G
} VIA_BB_TYPE, *PVIA_BB_TYPE;

/*                                                                         */
typedef enum _VIA_PKT_TYPE
{
    PK_TYPE_11A = 0,
    PK_TYPE_11B,
    PK_TYPE_11GB,
    PK_TYPE_11GA
} VIA_PKT_TYPE, *PVIA_PKT_TYPE;

/*                */

typedef enum __DEVICE_NDIS_STATUS {
    STATUS_SUCCESS = 0,
    STATUS_FAILURE,
    STATUS_RESOURCES,
    STATUS_PENDING,
} DEVICE_NDIS_STATUS, *PDEVICE_NDIS_STATUS;

#define MAX_BSSIDINFO_4_PMKID   16
#define MAX_PMKIDLIST           5
/*                                          */
#define NDIS_802_11_PMKID_CANDIDATE_PREAUTH_ENABLED	0x01

/*                  */
typedef unsigned char   NDIS_802_11_PMKID_VALUE[16];

typedef enum _NDIS_802_11_WEP_STATUS
{
    Ndis802_11WEPEnabled,
    Ndis802_11Encryption1Enabled = Ndis802_11WEPEnabled,
    Ndis802_11WEPDisabled,
    Ndis802_11EncryptionDisabled = Ndis802_11WEPDisabled,
    Ndis802_11WEPKeyAbsent,
    Ndis802_11Encryption1KeyAbsent = Ndis802_11WEPKeyAbsent,
    Ndis802_11WEPNotSupported,
    Ndis802_11EncryptionNotSupported = Ndis802_11WEPNotSupported,
    Ndis802_11Encryption2Enabled,
    Ndis802_11Encryption2KeyAbsent,
    Ndis802_11Encryption3Enabled,
    Ndis802_11Encryption3KeyAbsent
} NDIS_802_11_WEP_STATUS, *PNDIS_802_11_WEP_STATUS,
  NDIS_802_11_ENCRYPTION_STATUS, *PNDIS_802_11_ENCRYPTION_STATUS;

typedef enum _NDIS_802_11_STATUS_TYPE
{
	Ndis802_11StatusType_Authentication,
	Ndis802_11StatusType_MediaStreamMode,
	Ndis802_11StatusType_PMKID_CandidateList,
	Ndis802_11StatusTypeMax, /*                                         */
} NDIS_802_11_STATUS_TYPE, *PNDIS_802_11_STATUS_TYPE;

/*                                           */
typedef struct _PMKID_CANDIDATE {
    NDIS_802_11_MAC_ADDRESS BSSID;
    unsigned long Flags;
} PMKID_CANDIDATE, *PPMKID_CANDIDATE;

typedef struct _BSSID_INFO
{
    NDIS_802_11_MAC_ADDRESS BSSID;
    NDIS_802_11_PMKID_VALUE PMKID;
} BSSID_INFO, *PBSSID_INFO;

typedef struct tagSPMKID {
    unsigned long Length;
    unsigned long BSSIDInfoCount;
    BSSID_INFO BSSIDInfo[MAX_BSSIDINFO_4_PMKID];
} SPMKID, *PSPMKID;

typedef struct tagSPMKIDCandidateEvent {
    NDIS_802_11_STATUS_TYPE     StatusType;
	unsigned long Version;       /*                          */
	unsigned long NumCandidates; /*                         */
    PMKID_CANDIDATE CandidateList[MAX_PMKIDLIST];
} SPMKIDCandidateEvent, *PSPMKIDCandidateEvent;

/*                   */
#define MAX_QUIET_COUNT     8

typedef struct tagSQuietControl {
    bool        bEnable;
    u32       dwStartTime;
    u8        byPeriod;
    u16        wDuration;
} SQuietControl, *PSQuietControl;

/*                                             */
typedef struct tagSCacheEntry{
    u16        wFmSequence;
    u8        abyAddr2[ETH_ALEN];
    u16        wFrameCtl;
} SCacheEntry, *PSCacheEntry;

typedef struct tagSCache{
/*                                                                          
                          
 */
	unsigned int uInPtr; /*                   */
    SCacheEntry     asCacheEntry[DUPLICATE_RX_CACHE_LENGTH];
} SCache, *PSCache;

#define CB_MAX_RX_FRAG                 64
/*
                                                                              
 */
typedef struct tagSDeFragControlBlock
{
    u16            wSequence;
    u16            wFragNum;
    u8            abyAddr2[ETH_ALEN];
	unsigned int            uLifetime;
    struct sk_buff* skb;
    u8 *           pbyRxBuffer;
    unsigned int            cbFrameLength;
    bool            bInUse;
} SDeFragControlBlock, *PSDeFragControlBlock;

/*                   */
#define     DEVICE_FLAGS_UNPLUG          0x00000001UL
#define     DEVICE_FLAGS_PREAMBLE_TYPE   0x00000002UL
#define     DEVICE_FLAGS_OP_MODE         0x00000004UL
#define     DEVICE_FLAGS_PS_MODE         0x00000008UL
#define		DEVICE_FLAGS_80211h_MODE	 0x00000010UL

/*                         */
#define     DEVICE_FLAGS_OPENED          0x00010000UL
#define     DEVICE_FLAGS_WOL_ENABLED     0x00080000UL
/*                        */
#define     DEVICE_FLAGS_TX_ALIGN        0x01000000UL
#define     DEVICE_FLAGS_HAVE_CAM        0x02000000UL
#define     DEVICE_FLAGS_FLOW_CTRL       0x04000000UL

/*                      */
#define     DEVICE_LINK_FAIL             0x00000001UL
#define     DEVICE_SPEED_10              0x00000002UL
#define     DEVICE_SPEED_100             0x00000004UL
#define     DEVICE_SPEED_1000            0x00000008UL
#define     DEVICE_DUPLEX_FULL           0x00000010UL
#define     DEVICE_AUTONEG_ENABLE        0x00000020UL
#define     DEVICE_FORCED_BY_EEPROM      0x00000040UL
/*                             */
#define     DEVICE_LINK_CHANGE           0x00000001UL

typedef struct __device_opt {
	int nRxDescs0;  /*                            */
	int nTxDescs0;  /*                                     */
	int rts_thresh; /*               */
    int         frag_thresh;
    int         OpMode;
    int         data_rate;
    int         channel_num;
    int         short_retry;
    int         long_retry;
    int         bbp_type;
    u32         flags;
} OPTIONS, *POPTIONS;

struct vnt_private {
	/*        */
	struct usb_device *usb;
	struct net_device *dev;
	struct net_device_stats stats;

	OPTIONS sOpts;

	struct tasklet_struct CmdWorkItem;
	struct tasklet_struct EventWorkItem;
	struct tasklet_struct ReadWorkItem;
	struct tasklet_struct RxMngWorkItem;

	u32 rx_buf_sz;
	int multicast_limit;
	u8 byRxMode;

	spinlock_t lock;

	u32 rx_bytes;

	u8 byRevId;

	u32 flags;
	unsigned long Flags;

	SCache sDupRxCache;

	SDeFragControlBlock sRxDFCB[CB_MAX_RX_FRAG];
	u32 cbDFCB;
	u32 cbFreeDFCB;
	u32 uCurrentDFCBIdx;

	/*     */
	struct urb *pControlURB;
	struct urb *pInterruptURB;
	struct usb_ctrlrequest sUsbCtlRequest;
	u32 int_interval;

	/*                                                   */
	PRCB pRCBMem;
	PRCB apRCB[CB_MAX_RX_DESC];
	u32 cbRD;
	PRCB FirstRecvFreeList;
	PRCB LastRecvFreeList;
	u32 NumRecvFreeList;
	PRCB FirstRecvMngList;
	PRCB LastRecvMngList;
	u32 NumRecvMngList;
	int bIsRxWorkItemQueued;
	int bIsRxMngWorkItemQueued;
	unsigned long ulRcvRefCount; /*                                     */

	/*                                                    */
	PUSB_SEND_CONTEXT apTD[CB_MAX_TX_DESC];
	u32 cbTD;

	/*                                                        */
	INT_BUFFER intBuf;
	int fKillEventPollingThread;
	int bEventAvailable;

	/*                                          */
	DEFAULT_CONFIG config_file;

	/*                   */
	unsigned long ulBulkInPosted;
	unsigned long ulBulkInError;
	unsigned long ulBulkInContCRCError;
	unsigned long ulBulkInBytesRead;

	unsigned long ulBulkOutPosted;
	unsigned long ulBulkOutError;
	unsigned long ulBulkOutContCRCError;
	unsigned long ulBulkOutBytesWrite;

	unsigned long ulIntInPosted;
	unsigned long ulIntInError;
	unsigned long ulIntInContCRCError;
	unsigned long ulIntInBytesRead;

	/*                 */
	u16 wFirmwareVersion;
	u8 byLocalID;
	u8 byRFType;
	u8 byBBRxConf;

	u8 byZoneType;
	int bZoneRegExist;

	u8 byOriginalZonetype;

	int bLinkPass; /*                         */
	u8 abyCurrentNetAddr[ETH_ALEN];
	u8 abyPermanentNetAddr[ETH_ALEN];

	int bExistSWNetAddr;

	/*                    */
	SStatCounter scStatistic;
	/*                */
	SDot11Counters s802_11Counter;

	/*                                  */
	unsigned long packetsReceived;
	unsigned long packetsReceivedDropped;
	unsigned long packetsReceivedOverflow;
	unsigned long packetsSent;
	unsigned long packetsSentDropped;
	unsigned long SendContextsInUse;
	unsigned long RcvBuffersInUse;

	/*                   */
	struct vnt_manager vnt_mgmt;

	u64 qwCurrTSF;
	u32 cbBulkInMax;
	int bPSRxBeacon;

	/*                     */
	u32 uCurrRSSI;
	u8 byCurrSQ;

	/*                   */
	int bTxRxAntInv;
	u32 dwRxAntennaSel;
	u32 dwTxAntennaSel;
	u8 byAntennaCount;
	u8 byRxAntennaMode;
	u8 byTxAntennaMode;
	u8 byRadioCtl;
	u8 bHWRadioOff;

	/*                                     */
	struct timer_list TimerSQ3Tmax1;
	struct timer_list TimerSQ3Tmax2;
	struct timer_list TimerSQ3Tmax3;

	int bDiversityRegCtlON;
	int bDiversityEnable;
	unsigned long ulDiversityNValue;
	unsigned long ulDiversityMValue;
	u8 byTMax;
	u8 byTMax2;
	u8 byTMax3;
	unsigned long ulSQ3TH;

	unsigned long uDiversityCnt;
	u8 byAntennaState;
	unsigned long ulRatio_State0;
	unsigned long ulRatio_State1;
	unsigned long ulSQ3_State0;
	unsigned long ulSQ3_State1;

	unsigned long aulSQ3Val[MAX_RATE];
	unsigned long aulPktNum[MAX_RATE];

	/*          */
	u32 uSIFS;  /*              */
	u32 uDIFS;  /*              */
	u32 uEIFS;  /*              */
	u32 uSlot;  /*                  */
	u32 uCwMin; /*               */
	u32 uCwMax; /*                        */

	/*               */
	u8  bySIFS;
	u8  byDIFS;
	u8  byEIFS;
	u8  bySlot;
	u8  byCWMaxMin;

	/*      */
	VIA_BB_TYPE byBBType; /*                      */
	VIA_PKT_TYPE byPacketType; /*                           */
	u16 wBasicRate;
	u8 byACKRate;
	u8 byTopOFDMBasicRate;
	u8 byTopCCKBasicRate;

	u32 dwAotoRateTxOkCnt;
	u32 dwAotoRateTxFailCnt;
	u32 dwErrorRateThreshold[13];
	u32 dwTPTable[MAX_RATE];
	u8 abyEEPROM[EEP_MAX_CONTEXT_SIZE];  /*              */

	u8 byMinChannel;
	u8 byMaxChannel;
	u32 uConnectionRate;

	u8 byPreambleType;
	u8 byShortPreamble;
	/*               */
	u8 eConfigPHYMode;

	/*                    */
	u8 byCCKPwr;
	u8 byOFDMPwrG;
	u8 byOFDMPwrA;
	u8 byCurPwr;
	u8 abyCCKPwrTbl[14];
	u8 abyOFDMPwrTbl[14];
	u8 abyOFDMAPwrTbl[42];

	u16 wCurrentRate;
	u16 wRTSThreshold;
	u16 wFragmentationThreshold;
	u8 byShortRetryLimit;
	u8 byLongRetryLimit;
	CARD_OP_MODE eOPMode;
	int bBSSIDFilter;
	u16 wMaxTransmitMSDULifetime;
	u8 abyBSSID[ETH_ALEN];
	u8 abyDesireBSSID[ETH_ALEN];

	u16 wCTSDuration;       /*                           */
	u16 wACKDuration;
	u16 wRTSTransmitLen;
	u8 byRTSServiceField;
	u8 byRTSSignalField;

	u32 dwMaxReceiveLifetime;  /*                         */

	int bCCK;
	int bEncryptionEnable;
	int bLongHeader;
	int bSoftwareGenCrcErr;
	int bShortSlotTime;
	int bProtectMode;
	int bNonERPPresent;
	int bBarkerPreambleMd;

	u8 byERPFlag;
	u16 wUseProtectCntDown;

	int bRadioControlOff;
	int bRadioOff;

	/*            */
	int bEnablePSMode;
	u16 wListenInterval;
	int bPWBitOn;
	WMAC_POWER_MODE ePSMode;
	unsigned long ulPSModeWaitTx;
	int bPSModeTxBurst;

	/*                 */
	u16 wSeqCounter;
	int bBeaconBufReady;
	int bBeaconSent;
	int bFixRate;
	u8 byCurrentCh;
	u32 uScanTime;

	CMD_STATE eCommandState;

	CMD_CODE eCommand;
	int bBeaconTx;
	u8 byScanBBType;

	int bStopBeacon;
	int bStopDataPkt;
	int bStopTx0Pkt;
	u32 uAutoReConnectTime;
	u32 uIsroamingTime;

	/*                */

	CMD_ITEM eCmdQueue[CMD_Q_SIZE];
	u32 uCmdDequeueIdx;
	u32 uCmdEnqueueIdx;
	u32 cbFreeCmdQueue;
	int bCmdRunning;
	int bCmdClear;
	int bNeedRadioOFF;

	int bEnableRoaming;
	int bIsRoaming;
	int bFastRoaming;
	u8 bSameBSSMaxNum;
	u8 bSameBSSCurNum;
	int bRoaming;
	int b11hEable;
	unsigned long ulTxPower;

	/*            */
	NDIS_802_11_WEP_STATUS eEncryptionStatus;
	int  bTransmitKey;
	NDIS_802_11_WEP_STATUS eOldEncryptionStatus;
	SKeyManagement sKey;
	u32 dwIVCounter;

	RC4Ext SBox;
	u8 abyPRNG[WLAN_WEPMAX_KEYLEN+3];
	u8 byKeyIndex;

	int bAES;

	u32 uKeyLength;
	u8 abyKey[WLAN_WEP232_KEYLEN];

	/*             */
	u32 uAssocCount;
	int bMoreData;

	/*     */
	int bGrpAckPolicy;

	u8 byAutoFBCtrl;

	int bTxMICFail;
	int bRxMICFail;

	/*                                     */
	int bUpdateBBVGA;
	u32 uBBVGADiffCount;
	u8 byBBVGANew;
	u8 byBBVGACurrent;
	u8 abyBBVGA[BB_VGA_LEVEL];
	signed long ldBmThreshold[BB_VGA_LEVEL];

	u8 byBBPreEDRSSI;
	u8 byBBPreEDIndex;

	int bRadioCmd;
	u32 dwDiagRefCount;

	/*                */
	u8  byFOETuning;

	/*                        */
	u8  byAutoPwrTunning;

	/*                       */
	u8 byBBCR4d;
	u8 byBBCRc9;
	u8 byBBCR88;
	u8 byBBCR09;

	/*               */
	struct timer_list sTimerCommand;

	struct timer_list sTimerTxData;
	unsigned long nTxDataTimeCout;
	int fTxDataInSleep;
	int IsTxDataTrigger;

	int fWPA_Authened; /*                                         */
	u8 byReAssocCount;
	u8 byLinkWaitCount;

	struct ethhdr sTxEthHeader;
	struct ethhdr sRxEthHeader;
	u8 abyBroadcastAddr[ETH_ALEN];
	u8 abySNAP_RFC1042[ETH_ALEN];
	u8 abySNAP_Bridgetunnel[ETH_ALEN];

	/*                                */
	SPMKID gsPMKID;
	SPMKIDCandidateEvent gsPMKIDCandidate;

	/*             */
	int b11hEnable;

	int bChannelSwitch;
	u8 byNewChannel;
	u8 byChannelSwitchCount;

	/*                       */
	int bWPADEVUp;
	int bwextstep0;
	int bwextstep1;
	int bwextstep2;
	int bwextstep3;
	int bWPASuppWextEnabled;

	/*                                                */
	int bEnableHostapd;
	int bEnable8021x;
	int bEnableHostWEP;
	struct net_device *apdev;
	int (*tx_80211)(struct sk_buff *skb, struct net_device *dev);

	u32 uChannel;

	struct iw_statistics wstats; /*                */

	int bCommit;

};

#define EnqueueRCB(_Head, _Tail, _RCB)                  \
{                                                       \
    if (!_Head) {                                       \
        _Head = _RCB;                                   \
    }                                                   \
    else {                                              \
        _Tail->Next = _RCB;                             \
    }                                                   \
    _RCB->Next = NULL;                                  \
    _Tail = _RCB;                                       \
}

#define DequeueRCB(Head, Tail)                          \
{                                                       \
    PRCB   RCB = Head;                                  \
    if (!RCB->Next) {                                   \
        Tail = NULL;                                    \
    }                                                   \
    Head = RCB->Next;                                   \
}

#define ADD_ONE_WITH_WRAP_AROUND(uVar, uModulo) {   \
    if ((uVar) >= ((uModulo) - 1))                  \
        (uVar) = 0;                                 \
    else                                            \
        (uVar)++;                                   \
}

#define fMP_RESET_IN_PROGRESS               0x00000001
#define fMP_DISCONNECTED                    0x00000002
#define fMP_HALT_IN_PROGRESS                0x00000004
#define fMP_SURPRISE_REMOVED                0x00000008
#define fMP_RECV_LOOKASIDE                  0x00000010
#define fMP_INIT_IN_PROGRESS                0x00000020
#define fMP_SEND_SIDE_RESOURCE_ALLOCATED    0x00000040
#define fMP_RECV_SIDE_RESOURCE_ALLOCATED    0x00000080
#define fMP_POST_READS                      0x00000100
#define fMP_POST_WRITES                     0x00000200
#define fMP_CONTROL_READS                   0x00000400
#define fMP_CONTROL_WRITES                  0x00000800

#define MP_SET_FLAG(_M, _F)             ((_M)->Flags |= (_F))
#define MP_CLEAR_FLAG(_M, _F)            ((_M)->Flags &= ~(_F))
#define MP_TEST_FLAGS(_M, _F)            (((_M)->Flags & (_F)) == (_F))

#define MP_IS_READY(_M)        (((_M)->Flags & \
                                 (fMP_DISCONNECTED | fMP_RESET_IN_PROGRESS | fMP_HALT_IN_PROGRESS | fMP_INIT_IN_PROGRESS | fMP_SURPRISE_REMOVED)) == 0)

int device_alloc_frag_buf(struct vnt_private *, PSDeFragControlBlock pDeF);

#endif
