/*
 * Copyright (c) 2012 Broadcom Corporation
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
#ifndef WL_CFGP2P_H_
#define WL_CFGP2P_H_

#include <net/cfg80211.h>

struct brcmf_cfg80211_info;

/* 
                                                            
  
                                                           
                                                                       
                                                                     
                                               
 */
enum p2p_bss_type {
	P2PAPI_BSSCFG_PRIMARY, /*                                 */
	P2PAPI_BSSCFG_DEVICE, /*                                              */
	P2PAPI_BSSCFG_CONNECTION, /*                                        */
	P2PAPI_BSSCFG_MAX
};

/* 
                                                         
  
                                           
                     
 */
struct p2p_bss {
	struct brcmf_cfg80211_vif *vif;
	void *private_data;
};

/* 
                                                      
  
                                                                 
                                      
                                                                         
                                                                         
                                                                             
                                                                    
                                                                
                                                              
                                                                       
                                                                               
                                                                         
                                                                               
                                                                          
 */
enum brcmf_p2p_status {
	BRCMF_P2P_STATUS_ENABLED,
	BRCMF_P2P_STATUS_IF_ADD,
	BRCMF_P2P_STATUS_IF_DEL,
	BRCMF_P2P_STATUS_IF_DELETING,
	BRCMF_P2P_STATUS_IF_CHANGING,
	BRCMF_P2P_STATUS_IF_CHANGED,
	BRCMF_P2P_STATUS_ACTION_TX_COMPLETED,
	BRCMF_P2P_STATUS_ACTION_TX_NOACK,
	BRCMF_P2P_STATUS_GO_NEG_PHASE,
	BRCMF_P2P_STATUS_DISCOVER_LISTEN,
	BRCMF_P2P_STATUS_SENDING_ACT_FRAME,
	BRCMF_P2P_STATUS_WAITING_NEXT_AF_LISTEN,
	BRCMF_P2P_STATUS_WAITING_NEXT_ACT_FRAME,
	BRCMF_P2P_STATUS_FINDING_COMMON_CHANNEL
};

/* 
                                                     
  
                                                 
                                              
                                        
                               
                                        
                                              
                                                  
                                                           
 */
struct afx_hdl {
	struct work_struct afx_work;
	struct completion act_frm_scan;
	bool is_active;
	s32 peer_chan;
	bool is_listen;
	u16 my_listen_chan;
	u16 peer_listen_chan;
	u8 tx_dst_addr[ETH_ALEN];
};

/* 
                                                           
  
                                                    
                                                      
                                 
                                    
                                         
                                                                  
                          
                                                                      
                                                                
                                                                      
                                                   
                                                              
                                              
                                                  
                                                                
                                              
                                                               
                                                           
 */
struct brcmf_p2p_info {
	struct brcmf_cfg80211_info *cfg;
	unsigned long status;
	u8 dev_addr[ETH_ALEN];
	u8 int_addr[ETH_ALEN];
	struct p2p_bss bss_idx[P2PAPI_BSSCFG_MAX];
	struct timer_list listen_timer;
	struct brcmf_ssid ssid;
	u8 listen_channel;
	struct ieee80211_channel remain_on_channel;
	u32 remain_on_channel_cookie;
	u8 next_af_subtype;
	struct completion send_af_done;
	struct afx_hdl afx_hdl;
	u32 af_sent_channel;
	unsigned long af_tx_sent_jiffies;
	struct completion wait_next_af;
	bool gon_req_action;
	bool block_gon_req_tx;
};

s32 brcmf_p2p_attach(struct brcmf_cfg80211_info *cfg);
void brcmf_p2p_detach(struct brcmf_p2p_info *p2p);
struct wireless_dev *brcmf_p2p_add_vif(struct wiphy *wiphy, const char *name,
				       enum nl80211_iftype type, u32 *flags,
				       struct vif_params *params);
int brcmf_p2p_del_vif(struct wiphy *wiphy, struct wireless_dev *wdev);
int brcmf_p2p_ifchange(struct brcmf_cfg80211_info *cfg,
		       enum brcmf_fil_p2p_if_types if_type);
int brcmf_p2p_start_device(struct wiphy *wiphy, struct wireless_dev *wdev);
void brcmf_p2p_stop_device(struct wiphy *wiphy, struct wireless_dev *wdev);
int brcmf_p2p_scan_prep(struct wiphy *wiphy,
			struct cfg80211_scan_request *request,
			struct brcmf_cfg80211_vif *vif);
int brcmf_p2p_remain_on_channel(struct wiphy *wiphy, struct wireless_dev *wdev,
				struct ieee80211_channel *channel,
				unsigned int duration, u64 *cookie);
int brcmf_p2p_notify_listen_complete(struct brcmf_if *ifp,
				     const struct brcmf_event_msg *e,
				     void *data);
void brcmf_p2p_cancel_remain_on_channel(struct brcmf_if *ifp);
int brcmf_p2p_notify_action_frame_rx(struct brcmf_if *ifp,
				     const struct brcmf_event_msg *e,
				     void *data);
int brcmf_p2p_notify_action_tx_complete(struct brcmf_if *ifp,
					const struct brcmf_event_msg *e,
					void *data);
bool brcmf_p2p_send_action_frame(struct brcmf_cfg80211_info *cfg,
				 struct net_device *ndev,
				 struct brcmf_fil_af_params_le *af_params);
bool brcmf_p2p_scan_finding_common_channel(struct brcmf_cfg80211_info *cfg,
					   struct brcmf_bss_info_le *bi);
s32 brcmf_p2p_notify_rx_mgmt_p2p_probereq(struct brcmf_if *ifp,
					  const struct brcmf_event_msg *e,
					  void *data);
#endif /*              */
