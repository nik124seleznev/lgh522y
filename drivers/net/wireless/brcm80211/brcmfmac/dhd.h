/*
 * Copyright (c) 2010 Broadcom Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*               
                 
 */

#ifndef _BRCMF_H_
#define _BRCMF_H_

#define BRCMF_VERSION_STR		"4.218.248.5"

#include "fweh.h"

/*                                                                              
                                                   
                                                                              */
#define BRCMF_C_GET_VERSION			1
#define BRCMF_C_UP				2
#define BRCMF_C_DOWN				3
#define BRCMF_C_SET_PROMISC			10
#define BRCMF_C_GET_RATE			12
#define BRCMF_C_GET_INFRA			19
#define BRCMF_C_SET_INFRA			20
#define BRCMF_C_GET_AUTH			21
#define BRCMF_C_SET_AUTH			22
#define BRCMF_C_GET_BSSID			23
#define BRCMF_C_GET_SSID			25
#define BRCMF_C_SET_SSID			26
#define BRCMF_C_TERMINATED			28
#define BRCMF_C_GET_CHANNEL			29
#define BRCMF_C_SET_CHANNEL			30
#define BRCMF_C_GET_SRL				31
#define BRCMF_C_SET_SRL				32
#define BRCMF_C_GET_LRL				33
#define BRCMF_C_SET_LRL				34
#define BRCMF_C_GET_RADIO			37
#define BRCMF_C_SET_RADIO			38
#define BRCMF_C_GET_PHYTYPE			39
#define BRCMF_C_SET_KEY				45
#define BRCMF_C_SET_PASSIVE_SCAN		49
#define BRCMF_C_SCAN				50
#define BRCMF_C_SCAN_RESULTS			51
#define BRCMF_C_DISASSOC			52
#define BRCMF_C_REASSOC				53
#define BRCMF_C_SET_ROAM_TRIGGER		55
#define BRCMF_C_SET_ROAM_DELTA			57
#define BRCMF_C_GET_BCNPRD			75
#define BRCMF_C_SET_BCNPRD			76
#define BRCMF_C_GET_DTIMPRD			77
#define BRCMF_C_SET_DTIMPRD			78
#define BRCMF_C_SET_COUNTRY			84
#define BRCMF_C_GET_PM				85
#define BRCMF_C_SET_PM				86
#define BRCMF_C_GET_CURR_RATESET		114
#define BRCMF_C_GET_AP				117
#define BRCMF_C_SET_AP				118
#define BRCMF_C_GET_RSSI			127
#define BRCMF_C_GET_WSEC			133
#define BRCMF_C_SET_WSEC			134
#define BRCMF_C_GET_PHY_NOISE			135
#define BRCMF_C_GET_BSS_INFO			136
#define BRCMF_C_GET_BANDLIST			140
#define BRCMF_C_SET_SCB_TIMEOUT			158
#define BRCMF_C_GET_PHYLIST			180
#define BRCMF_C_SET_SCAN_CHANNEL_TIME		185
#define BRCMF_C_SET_SCAN_UNASSOC_TIME		187
#define BRCMF_C_SCB_DEAUTHENTICATE_FOR_REASON	201
#define BRCMF_C_GET_VALID_CHANNELS		217
#define BRCMF_C_GET_KEY_PRIMARY			235
#define BRCMF_C_SET_KEY_PRIMARY			236
#define BRCMF_C_SET_SCAN_PASSIVE_TIME		258
#define BRCMF_C_GET_VAR				262
#define BRCMF_C_SET_VAR				263

/*                                        */
#define	WLC_PHY_TYPE_A		0
#define	WLC_PHY_TYPE_B		1
#define	WLC_PHY_TYPE_G		2
#define	WLC_PHY_TYPE_N		4
#define	WLC_PHY_TYPE_LP		5
#define	WLC_PHY_TYPE_SSN	6
#define	WLC_PHY_TYPE_HT		7
#define	WLC_PHY_TYPE_LCN	8
#define	WLC_PHY_TYPE_NULL	0xf

#define BRCMF_EVENTING_MASK_LEN	16

#define TOE_TX_CSUM_OL		0x00000001
#define TOE_RX_CSUM_OL		0x00000002

#define	BRCMF_BSS_INFO_VERSION	109 /*                                      */

/*                                                               */
#define BRCMF_SCAN_PARAMS_FIXED_SIZE 64

/*                                  */
#define BRCMF_SCAN_PARAMS_COUNT_MASK 0x0000ffff
#define BRCMF_SCAN_PARAMS_NSSID_SHIFT 16

/*                     */
#define BRCMF_PRIMARY_KEY	(1 << 1)

/*                                    */
#define BRCMF_MAX_IFS	16

#define DOT11_BSSTYPE_ANY			2
#define DOT11_MAX_DEFAULT_KEYS	4

#define BRCMF_ESCAN_REQ_VERSION 1

#define WLC_BSS_RSSI_ON_CHANNEL		0x0002

#define BRCMF_MAXRATES_IN_SET		16	/*                           */
#define BRCMF_STA_ASSOC			0x10		/*            */

#define BRCMF_E_STATUS_SUCCESS			0
#define BRCMF_E_STATUS_FAIL			1
#define BRCMF_E_STATUS_TIMEOUT			2
#define BRCMF_E_STATUS_NO_NETWORKS		3
#define BRCMF_E_STATUS_ABORT			4
#define BRCMF_E_STATUS_NO_ACK			5
#define BRCMF_E_STATUS_UNSOLICITED		6
#define BRCMF_E_STATUS_ATTEMPT			7
#define BRCMF_E_STATUS_PARTIAL			8
#define BRCMF_E_STATUS_NEWSCAN			9
#define BRCMF_E_STATUS_NEWASSOC			10
#define BRCMF_E_STATUS_11HQUIET			11
#define BRCMF_E_STATUS_SUPPRESS			12
#define BRCMF_E_STATUS_NOCHANS			13
#define BRCMF_E_STATUS_CS_ABORT			15
#define BRCMF_E_STATUS_ERROR			16

#define BRCMF_E_REASON_INITIAL_ASSOC		0
#define BRCMF_E_REASON_LOW_RSSI			1
#define BRCMF_E_REASON_DEAUTH			2
#define BRCMF_E_REASON_DISASSOC			3
#define BRCMF_E_REASON_BCNS_LOST		4
#define BRCMF_E_REASON_MINTXRATE		9
#define BRCMF_E_REASON_TXFAIL			10

#define BRCMF_E_REASON_LINK_BSSCFG_DIS		4
#define BRCMF_E_REASON_FAST_ROAM_FAILED		5
#define BRCMF_E_REASON_DIRECTED_ROAM		6
#define BRCMF_E_REASON_TSPEC_REJECTED		7
#define BRCMF_E_REASON_BETTER_AP		8

#define BRCMF_E_PRUNE_ENCR_MISMATCH		1
#define BRCMF_E_PRUNE_BCAST_BSSID		2
#define BRCMF_E_PRUNE_MAC_DENY			3
#define BRCMF_E_PRUNE_MAC_NA			4
#define BRCMF_E_PRUNE_REG_PASSV			5
#define BRCMF_E_PRUNE_SPCT_MGMT			6
#define BRCMF_E_PRUNE_RADAR			7
#define BRCMF_E_RSN_MISMATCH			8
#define BRCMF_E_PRUNE_NO_COMMON_RATES		9
#define BRCMF_E_PRUNE_BASIC_RATES		10
#define BRCMF_E_PRUNE_CIPHER_NA			12
#define BRCMF_E_PRUNE_KNOWN_STA			13
#define BRCMF_E_PRUNE_WDS_PEER			15
#define BRCMF_E_PRUNE_QBSS_LOAD			16
#define BRCMF_E_PRUNE_HOME_AP			17

#define BRCMF_E_SUP_OTHER			0
#define BRCMF_E_SUP_DECRYPT_KEY_DATA		1
#define BRCMF_E_SUP_BAD_UCAST_WEP128		2
#define BRCMF_E_SUP_BAD_UCAST_WEP40		3
#define BRCMF_E_SUP_UNSUP_KEY_LEN		4
#define BRCMF_E_SUP_PW_KEY_CIPHER		5
#define BRCMF_E_SUP_MSG3_TOO_MANY_IE		6
#define BRCMF_E_SUP_MSG3_IE_MISMATCH		7
#define BRCMF_E_SUP_NO_INSTALL_FLAG		8
#define BRCMF_E_SUP_MSG3_NO_GTK			9
#define BRCMF_E_SUP_GRP_KEY_CIPHER		10
#define BRCMF_E_SUP_GRP_MSG1_NO_GTK		11
#define BRCMF_E_SUP_GTK_DECRYPT_FAIL		12
#define BRCMF_E_SUP_SEND_FAIL			13
#define BRCMF_E_SUP_DEAUTH			14

#define BRCMF_E_IF_ADD				1
#define BRCMF_E_IF_DEL				2
#define BRCMF_E_IF_CHANGE			3

#define BRCMF_E_IF_ROLE_STA			0
#define BRCMF_E_IF_ROLE_AP			1
#define BRCMF_E_IF_ROLE_WDS			2

#define BRCMF_E_LINK_BCN_LOSS			1
#define BRCMF_E_LINK_DISASSOC			2
#define BRCMF_E_LINK_ASSOC_REC			3
#define BRCMF_E_LINK_BSSCFG_DIS			4

/*                                               
 */
#define BRCMF_DCMD_SMLEN	256
#define BRCMF_DCMD_MEDLEN	1536
#define BRCMF_DCMD_MAXLEN	8192

/*                                                                        
                                                                               
                                                                  
 */
struct brcmf_pkt_filter_pattern_le {
	/*
                                                            
                                                        
  */
	__le32 offset;
	/*                                                     */
	__le32 size_bytes;
	/*
                                                                   
                                     
  */
	u8 mask_and_pattern[1];
};

/*                                                                   */
struct brcmf_pkt_filter_le {
	__le32 id;		/*                                     */
	__le32 type;		/*                                       */
	__le32 negate_match;	/*                                     */
	union {			/*                    */
		struct brcmf_pkt_filter_pattern_le pattern; /*                */
	} u;
};

/*                                      */
struct brcmf_pkt_filter_enable_le {
	__le32 id;		/*                  */
	__le32 enable;		/*                     */
};

/*                   
                                                                             
                                                                     
 */
struct brcmf_bss_info_le {
	__le32 version;		/*               */
	__le32 length;		/*                                    
                                            
     */
	u8 BSSID[ETH_ALEN];
	__le16 beacon_period;	/*                 */
	__le16 capability;	/*                        */
	u8 SSID_len;
	u8 SSID[32];
	struct {
		__le32 count;   /*                     */
		u8 rates[16]; /*                                              */
	} rateset;		/*                 */
	__le16 chanspec;	/*                  */
	__le16 atim_window;	/*                 */
	u8 dtim_period;	/*             */
	__le16 RSSI;		/*                                  */
	s8 phy_noise;		/*                */

	u8 n_cap;		/*                        */
	/*                                               */
	__le32 nbss_cap;
	u8 ctl_ch;		/*                                    */
	__le32 reserved32[1];	/*                                          */
	u8 flags;		/*       */
	u8 reserved[3];	/*                                          */
	u8 basic_mcs[MCSSET_LEN];	/*                              */

	__le16 ie_offset;	/*                                           */
	__le32 ie_length;	/*                                     */
	__le16 SNR;		/*                                       */
	/*                     */
	/*                                      */
};

struct brcm_rateset_le {
	/*                     */
	__le32 count;
	/*                                              */
	u8 rates[BRCMF_MAXRATES_IN_SET];
};

struct brcmf_ssid {
	u32 SSID_len;
	unsigned char SSID[32];
};

struct brcmf_ssid_le {
	__le32 SSID_len;
	unsigned char SSID[32];
};

struct brcmf_scan_params_le {
	struct brcmf_ssid_le ssid_le;	/*                  */
	u8 bssid[ETH_ALEN];	/*                */
	s8 bss_type;		/*              
                                                   
     */
	u8 scan_type;	/*                      */
	__le32 nprobes;	  /*                                              */
	__le32 active_time;	/*                                           
                      
     */
	__le32 passive_time;	/*                                       
                           
     */
	__le32 home_time;	/*                                   
                                         
     */
	__le32 channel_num;	/*                                        
      
                                       
                                             
                          
      
                                                
                                                   
                                                 
                                          
                                  
      
                                                
                                                    
                                           
     */
	__le16 channel_list[1];	/*                   */
};

struct brcmf_scan_results {
	u32 buflen;
	u32 version;
	u32 count;
	struct brcmf_bss_info_le bss_info_le[];
};

struct brcmf_escan_params_le {
	__le32 version;
	__le16 action;
	__le16 sync_id;
	struct brcmf_scan_params_le params_le;
};

struct brcmf_escan_result_le {
	__le32 buflen;
	__le32 version;
	__le16 sync_id;
	__le16 bss_count;
	struct brcmf_bss_info_le bss_info_le;
};

#define WL_ESCAN_RESULTS_FIXED_SIZE (sizeof(struct brcmf_escan_result_le) - \
	sizeof(struct brcmf_bss_info_le))

/*                                                              */
struct brcmf_assoc_params_le {
	/*                                   */
	u8 bssid[ETH_ALEN];
	/*                                                           
                  */
	__le32 chanspec_num;
	/*                   */
	__le16 chanspec_list[1];
};

/*                                                                 */
struct brcmf_join_params {
	struct brcmf_ssid_le ssid_le;
	struct brcmf_assoc_params_le params_le;
};

/*                               */
struct brcmf_join_scan_params_le {
	u8 scan_type;		/*                                       */
	__le32 nprobes;		/*                                          */
	__le32 active_time;	/*                                           
                      
     */
	__le32 passive_time;	/*                                       
                           
     */
	__le32 home_time;	/*                                        
                                    
     */
};

/*                      */
struct brcmf_ext_join_params_le {
	struct brcmf_ssid_le ssid_le;	/*                        */
	struct brcmf_join_scan_params_le scan_le;
	struct brcmf_assoc_params_le assoc_le;
};

struct brcmf_wsec_key {
	u32 index;		/*           */
	u32 len;		/*            */
	u8 data[WLAN_MAX_KEY_LEN];	/*          */
	u32 pad_1[18];
	u32 algo;	/*                                              */
	u32 flags;	/*            */
	u32 pad_2[3];
	u32 iv_initialized;	/*                                  */
	u32 pad_3;
	/*       */
	struct {
		u32 hi;	/*                     */
		u16 lo;	/*                     */
	} rxiv;
	u32 pad_4[2];
	u8 ea[ETH_ALEN];	/*             */
};

/*
                                                                              
 */
struct brcmf_wsec_key_le {
	__le32 index;		/*           */
	__le32 len;		/*            */
	u8 data[WLAN_MAX_KEY_LEN];	/*          */
	__le32 pad_1[18];
	__le32 algo;	/*                                              */
	__le32 flags;	/*            */
	__le32 pad_2[3];
	__le32 iv_initialized;	/*                                  */
	__le32 pad_3;
	/*       */
	struct {
		__le32 hi;	/*                     */
		__le16 lo;	/*                     */
	} rxiv;
	__le32 pad_4[2];
	u8 ea[ETH_ALEN];	/*             */
};

/*                                     */
struct brcmf_scb_val_le {
	__le32 val;
	u8 ea[ETH_ALEN];
};

/*                  */
struct brcmf_channel_info_le {
	__le32 hw_channel;
	__le32 target_channel;
	__le32 scan_channel;
};

struct brcmf_sta_info_le {
	__le16	ver;		/*                        */
	__le16	len;		/*                                   */
	__le16	cap;		/*                               */
	__le32	flags;		/*                     */
	__le32	idle;		/*                                   */
	u8	ea[ETH_ALEN];		/*                 */
	__le32	count;			/*                     */
	u8	rates[BRCMF_MAXRATES_IN_SET];	/*                        */
						/*                       */
	__le32	in;		/*                                  */
	__le32	listen_interval_inms; /*                                   */
	__le32	tx_pkts;	/*                          */
	__le32	tx_failures;	/*                     */
	__le32	rx_ucast_pkts;	/*                               */
	__le32	rx_mcast_pkts;	/*                                 */
	__le32	tx_rate;	/*                                  */
	__le32	rx_rate;	/*                                  */
	__le32	rx_decrypt_succeeds;	/*                                    */
	__le32	rx_decrypt_failures;	/*                              */
};

struct brcmf_chanspec_list {
	__le32	count;		/*              */
	__le32	element[1];	/*                             */
};

/*
                     
                        
                        
 */
struct brcmf_rx_mgmt_data {
	__be16	version;
	__be16	chanspec;
	__be32	rssi;
	__be32	mactime;
	__be32	rate;
};

/*                                */
struct brcmf_dcmd {
	uint cmd;		/*                              */
	void *buf;		/*                        */
	uint len;		/*                       */
	u8 set;			/*                               */
	uint used;		/*                                  */
	uint needed;		/*                         */
};

/*                                                */
struct brcmf_proto;	/*                                    */
struct brcmf_cfg80211_dev; /*                      */
struct brcmf_fws_info; /*                          */

/*                                                  */
struct brcmf_pub {
	/*                 */
	struct brcmf_bus *bus_if;
	struct brcmf_proto *prot;
	struct brcmf_cfg80211_info *config;

	/*                      */
	uint hdrlen;		/*                                         */
	uint rxsz;		/*                                      */
	u8 wme_dp;		/*                      */

	/*                   */
	unsigned long drv_version;	/*                                   */
	u8 mac[ETH_ALEN];		/*                                  */

	/*                                       */
	unsigned long tx_multicast;

	struct brcmf_if *iflist[BRCMF_MAX_IFS];

	struct mutex proto_block;
	unsigned char proto_buf[BRCMF_DCMD_MAXLEN];

	struct brcmf_fweh_info fweh;

	bool fw_signals;
	struct brcmf_fws_info *fws;
	spinlock_t fws_spinlock;
#ifdef DEBUG
	struct dentry *dbgfs_dir;
#endif
};

struct brcmf_if_event {
	u8 ifidx;
	u8 action;
	u8 flags;
	u8 bssidx;
	u8 role;
};

/*                      */
struct brcmf_cfg80211_vif;
struct brcmf_fws_mac_descriptor;

/* 
                                                                  
  
                                   
                                                         
                                      
                                     
 */
enum brcmf_netif_stop_reason {
	BRCMF_NETIF_STOP_REASON_FWS_FC = 1,
	BRCMF_NETIF_STOP_REASON_BLOCK_BUS = 2
};

/* 
                                                   
  
                                               
                                                           
                                    
                                                 
                                                           
                                                             
                                                                
                                              
                                                        
                                   
                                                                     
                                                               
                                                         
 */
struct brcmf_if {
	struct brcmf_pub *drvr;
	struct brcmf_cfg80211_vif *vif;
	struct net_device *ndev;
	struct net_device_stats stats;
	struct work_struct setmacaddr_work;
	struct work_struct multicast_work;
	struct brcmf_fws_mac_descriptor *fws_desc;
	int ifidx;
	s32 bssidx;
	u8 mac_addr[ETH_ALEN];
	u8 netif_stop;
	atomic_t pend_8021x_cnt;
	wait_queue_head_t pend_8021x_wait;
};


extern int brcmf_netdev_wait_pend8021x(struct net_device *ndev);

/*                                  */
extern char *brcmf_ifname(struct brcmf_pub *drvr, int idx);

/*              */
extern int brcmf_proto_cdc_query_dcmd(struct brcmf_pub *drvr, int ifidx,
				       uint cmd, void *buf, uint len);
extern int brcmf_proto_cdc_set_dcmd(struct brcmf_pub *drvr, int ifidx, uint cmd,
				    void *buf, uint len);

/*                                           */
extern int brcmf_proto_hdrpull(struct brcmf_pub *drvr, bool do_fws, u8 *ifidx,
			       struct sk_buff *rxp);

extern int brcmf_net_attach(struct brcmf_if *ifp, bool rtnl_locked);
extern struct brcmf_if *brcmf_add_if(struct brcmf_pub *drvr, s32 bssidx,
				     s32 ifidx, char *name, u8 *mac_addr);
extern void brcmf_del_if(struct brcmf_pub *drvr, s32 bssidx);
void brcmf_txflowblock_if(struct brcmf_if *ifp,
			  enum brcmf_netif_stop_reason reason, bool state);
extern u32 brcmf_get_chip_info(struct brcmf_if *ifp);
extern void brcmf_txfinalize(struct brcmf_pub *drvr, struct sk_buff *txp,
			     bool success);

#endif				/*           */
