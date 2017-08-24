/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005-2006, Devicescape Software, Inc.
 * Copyright 2006-2007	Jiri Benc <jbenc@suse.cz>
 * Copyright 2007	Johannes Berg <johannes@sipsolutions.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Transmit and frame generation functions.
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/bitmap.h>
#include <linux/rcupdate.h>
#include <linux/export.h>
#include <net/net_namespace.h>
#include <net/ieee80211_radiotap.h>
#include <net/cfg80211.h>
#include <net/mac80211.h>
#include <asm/unaligned.h>

#include "ieee80211_i.h"
#include "driver-ops.h"
#include "led.h"
#include "mesh.h"
#include "wep.h"
#include "wpa.h"
#include "wme.h"
#include "rate.h"

/*            */

static __le16 ieee80211_duration(struct ieee80211_tx_data *tx,
				 struct sk_buff *skb, int group_addr,
				 int next_frag_len)
{
	int rate, mrate, erp, dur, i;
	struct ieee80211_rate *txrate;
	struct ieee80211_local *local = tx->local;
	struct ieee80211_supported_band *sband;
	struct ieee80211_hdr *hdr;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);

	/*                        */
	if (tx->rate.flags & IEEE80211_TX_RC_MCS)
		return 0;

	/*         */
	if (WARN_ON_ONCE(tx->rate.idx < 0))
		return 0;

	sband = local->hw.wiphy->bands[info->band];
	txrate = &sband->bitrates[tx->rate.idx];

	erp = txrate->flags & IEEE80211_RATE_ERP_G;

	/*
                                   
                       
                               
                                  
                                                                    
                                   
                                                                    
                                                          
   
                     
                                                                        
                                                                       
                                                                        
                                                                    
                     
  */
	hdr = (struct ieee80211_hdr *)skb->data;
	if (ieee80211_is_ctl(hdr->frame_control)) {
		/*                                                     
                                                            
                                                               
    
                                                              
                                                      
                                                         
                                             
                                                          
                                                             
                                                             
                    
                                     
   */
		return 0;
	}

	/*           */
	if (0 /*                           */)
		return cpu_to_le16(32768);

	if (group_addr) /*                                           */
		return 0;

	/*                                
                                                     
                                                                     
                                                                
                                                                    
                                                                        
                                                                       
                                           
                                                                       
  */
	rate = -1;
	/*                                          */
	mrate = sband->bitrates[0].bitrate;
	for (i = 0; i < sband->n_bitrates; i++) {
		struct ieee80211_rate *r = &sband->bitrates[i];

		if (r->bitrate > txrate->bitrate)
			break;

		if (tx->sdata->vif.bss_conf.basic_rates & BIT(i))
			rate = r->bitrate;

		switch (sband->band) {
		case IEEE80211_BAND_2GHZ: {
			u32 flag;
			if (tx->sdata->flags & IEEE80211_SDATA_OPERATING_GMODE)
				flag = IEEE80211_RATE_MANDATORY_G;
			else
				flag = IEEE80211_RATE_MANDATORY_B;
			if (r->flags & flag)
				mrate = r->bitrate;
			break;
		}
		case IEEE80211_BAND_5GHZ:
			if (r->flags & IEEE80211_RATE_MANDATORY_A)
				mrate = r->bitrate;
			break;
		case IEEE80211_BAND_60GHZ:
			/*                            */
		case IEEE80211_NUM_BANDS:
			WARN_ON(1);
			break;
		}
	}
	if (rate == -1) {
		/*                                                             
              */
		rate = mrate;
	}

	/*                                                           */
	if (ieee80211_is_data_qos(hdr->frame_control) &&
	    *(ieee80211_get_qos_ctl(hdr)) & IEEE80211_QOS_CTL_ACK_POLICY_NOACK)
		dur = 0;
	else
		/*                            
                                                             
                        */
		dur = ieee80211_frame_duration(sband->band, 10, rate, erp,
				tx->sdata->vif.bss_conf.use_short_preamble);

	if (next_frag_len) {
		/*                                                            
                                                   */
		dur *= 2; /*            */
		/*               */
		dur += ieee80211_frame_duration(sband->band, next_frag_len,
				txrate->bitrate, erp,
				tx->sdata->vif.bss_conf.use_short_preamble);
	}

	return cpu_to_le16(dur);
}

/*             */
static ieee80211_tx_result debug_noinline
ieee80211_tx_h_dynamic_ps(struct ieee80211_tx_data *tx)
{
	struct ieee80211_local *local = tx->local;
	struct ieee80211_if_managed *ifmgd;

	/*                                   */
	if (!(local->hw.flags & IEEE80211_HW_SUPPORTS_PS))
		return TX_CONTINUE;

	/*                                  */
	if (local->hw.flags & IEEE80211_HW_SUPPORTS_DYNAMIC_PS)
		return TX_CONTINUE;

	/*                             */
	if (local->hw.conf.dynamic_ps_timeout <= 0)
		return TX_CONTINUE;

	/*                                          */
	if (local->scanning)
		return TX_CONTINUE;

	if (!local->ps_sdata)
		return TX_CONTINUE;

	/*                                    */
	if (local->quiescing)
		return TX_CONTINUE;

	/*                                              */
	if (tx->sdata->vif.type != NL80211_IFTYPE_STATION)
		return TX_CONTINUE;

	ifmgd = &tx->sdata->u.mgd;

	/*
                                                                  
                                                                   
                                                                    
                                                            
                                                                   
                                                                
                     
   
                                                                  
                                                                   
                       
  */
	if ((ifmgd->flags & IEEE80211_STA_UAPSD_ENABLED) &&
	    (ifmgd->uapsd_queues & IEEE80211_WMM_IE_STA_QOSINFO_AC_VO) &&
	    skb_get_queue_mapping(tx->skb) == IEEE80211_AC_VO)
		return TX_CONTINUE;

	if (local->hw.conf.flags & IEEE80211_CONF_PS) {
		ieee80211_stop_queues_by_reason(&local->hw,
						IEEE80211_MAX_QUEUE_MAP,
						IEEE80211_QUEUE_STOP_REASON_PS);
		ifmgd->flags &= ~IEEE80211_STA_NULLFUNC_ACKED;
		ieee80211_queue_work(&local->hw,
				     &local->dynamic_ps_disable_work);
	}

	/*                                                    */
	if (!ifmgd->associated)
		return TX_CONTINUE;

	mod_timer(&local->dynamic_ps_timer, jiffies +
		  msecs_to_jiffies(local->hw.conf.dynamic_ps_timeout));

	return TX_CONTINUE;
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_check_assoc(struct ieee80211_tx_data *tx)
{

	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	bool assoc = false;

	if (unlikely(info->flags & IEEE80211_TX_CTL_INJECTED))
		return TX_CONTINUE;

	if (unlikely(test_bit(SCAN_SW_SCANNING, &tx->local->scanning)) &&
	    test_bit(SDATA_STATE_OFFCHANNEL, &tx->sdata->state) &&
	    !ieee80211_is_probe_req(hdr->frame_control) &&
	    !ieee80211_is_nullfunc(hdr->frame_control))
		/*
                                                           
                                                           
                                                             
                                                  
                                                 
                                        
                                     
    
                                                                       
   */
		return TX_DROP;

	if (tx->sdata->vif.type == NL80211_IFTYPE_WDS)
		return TX_CONTINUE;

	if (tx->sdata->vif.type == NL80211_IFTYPE_MESH_POINT)
		return TX_CONTINUE;

	if (tx->flags & IEEE80211_TX_PS_BUFFERED)
		return TX_CONTINUE;

	if (tx->sta)
		assoc = test_sta_flag(tx->sta, WLAN_STA_ASSOC);

	if (likely(tx->flags & IEEE80211_TX_UNICAST)) {
		if (unlikely(!assoc &&
			     ieee80211_is_data(hdr->frame_control))) {
#ifdef CONFIG_MAC80211_VERBOSE_DEBUG
			sdata_info(tx->sdata,
				   "dropped data frame to not associated station %pM\n",
				   hdr->addr1);
#endif
			I802_DEBUG_INC(tx->local->tx_handlers_drop_not_assoc);
			return TX_DROP;
		}
	} else if (unlikely(tx->sdata->vif.type == NL80211_IFTYPE_AP &&
			    ieee80211_is_data(hdr->frame_control) &&
			    !atomic_read(&tx->sdata->u.ap.num_mcast_sta))) {
		/*
                                                   
            
   */
		return TX_DROP;
	}

	return TX_CONTINUE;
}

/*                                                                             
                                                                             
                                                                               
                                                                  */
static void purge_old_ps_buffers(struct ieee80211_local *local)
{
	int total = 0, purged = 0;
	struct sk_buff *skb;
	struct ieee80211_sub_if_data *sdata;
	struct sta_info *sta;

	list_for_each_entry_rcu(sdata, &local->interfaces, list) {
		struct ps_data *ps;

		if (sdata->vif.type == NL80211_IFTYPE_AP)
			ps = &sdata->u.ap.ps;
		else if (ieee80211_vif_is_mesh(&sdata->vif))
			ps = &sdata->u.mesh.ps;
		else
			continue;

		skb = skb_dequeue(&ps->bc_buf);
		if (skb) {
			purged++;
			dev_kfree_skb(skb);
		}
		total += skb_queue_len(&ps->bc_buf);
	}

	/*
                                                             
                              
  */
	list_for_each_entry_rcu(sta, &local->sta_list, list) {
		int ac;

		for (ac = IEEE80211_AC_BK; ac >= IEEE80211_AC_VO; ac--) {
			skb = skb_dequeue(&sta->ps_tx_buf[ac]);
			total += skb_queue_len(&sta->ps_tx_buf[ac]);
			if (skb) {
				purged++;
				ieee80211_free_txskb(&local->hw, skb);
				break;
			}
		}
	}

	local->total_ps_buffered = total;
	ps_dbg_hw(&local->hw, "PS buffers full - purged %d frames\n", purged);
}

static ieee80211_tx_result
ieee80211_tx_h_multicast_ps_buf(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;
	struct ps_data *ps;

	/*
                             
   
                                                                 
                                                             
                                              
  */

	/*                                                      */
	if (tx->sdata->vif.type == NL80211_IFTYPE_AP ||
	    tx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN) {
		if (!tx->sdata->bss)
			return TX_CONTINUE;

		ps = &tx->sdata->bss->ps;
	} else if (ieee80211_vif_is_mesh(&tx->sdata->vif)) {
		ps = &tx->sdata->u.mesh.ps;
	} else {
		return TX_CONTINUE;
	}


	/*                                 */
	if (ieee80211_has_order(hdr->frame_control))
		return TX_CONTINUE;

	if (ieee80211_is_probe_req(hdr->frame_control))
		return TX_CONTINUE;

	/*                        */
	if (!atomic_read(&ps->num_sta_ps))
		return TX_CONTINUE;

	info->flags |= IEEE80211_TX_CTL_SEND_AFTER_DTIM;
	if (tx->local->hw.flags & IEEE80211_HW_QUEUE_CONTROL)
		info->hw_queue = tx->sdata->vif.cab_queue;

	/*                                         */
	if (!(tx->local->hw.flags & IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING))
		return TX_CONTINUE;

	/*                      */
	if (tx->local->total_ps_buffered >= TOTAL_MAX_TX_BUFFER)
		purge_old_ps_buffers(tx->local);

	if (skb_queue_len(&ps->bc_buf) >= AP_MAX_BC_BUFFER) {
		ps_dbg(tx->sdata,
		       "BC TX buffer full - dropping the oldest frame\n");
		dev_kfree_skb(skb_dequeue(&ps->bc_buf));
	} else
		tx->local->total_ps_buffered++;

	skb_queue_tail(&ps->bc_buf, tx->skb);

	return TX_QUEUED;
}

static int ieee80211_use_mfp(__le16 fc, struct sta_info *sta,
			     struct sk_buff *skb)
{
	if (!ieee80211_is_mgmt(fc))
		return 0;

	if (sta == NULL || !test_sta_flag(sta, WLAN_STA_MFP))
		return 0;

	if (!ieee80211_is_robust_mgmt_frame((struct ieee80211_hdr *)
					    skb->data))
		return 0;

	return 1;
}

static ieee80211_tx_result
ieee80211_tx_h_unicast_ps_buf(struct ieee80211_tx_data *tx)
{
	struct sta_info *sta = tx->sta;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;
	struct ieee80211_local *local = tx->local;

	if (unlikely(!sta))
		return TX_CONTINUE;

	if (unlikely((test_sta_flag(sta, WLAN_STA_PS_STA) ||
		      test_sta_flag(sta, WLAN_STA_PS_DRIVER)) &&
		     !(info->flags & IEEE80211_TX_CTL_NO_PS_BUFFER))) {
		int ac = skb_get_queue_mapping(tx->skb);

		/*                                                        */
		if (ieee80211_is_mgmt(hdr->frame_control) &&
		    !ieee80211_is_deauth(hdr->frame_control) &&
		    !ieee80211_is_disassoc(hdr->frame_control) &&
		    !ieee80211_is_action(hdr->frame_control)) {
			info->flags |= IEEE80211_TX_CTL_NO_PS_BUFFER;
			return TX_CONTINUE;
		}

		ps_dbg(sta->sdata, "STA %pM aid %d: PS buffer for AC %d\n",
		       sta->sta.addr, sta->sta.aid, ac);
		if (tx->local->total_ps_buffered >= TOTAL_MAX_TX_BUFFER)
			purge_old_ps_buffers(tx->local);

		/*                                           */
		spin_lock(&sta->ps_lock);
		/*
                                                                  
                                                               
                             
   */
		if (!test_sta_flag(sta, WLAN_STA_PS_STA) &&
		    !test_sta_flag(sta, WLAN_STA_PS_DRIVER)) {
			spin_unlock(&sta->ps_lock);
			return TX_CONTINUE;
		}

		if (skb_queue_len(&sta->ps_tx_buf[ac]) >= STA_MAX_TX_BUFFER) {
			struct sk_buff *old = skb_dequeue(&sta->ps_tx_buf[ac]);
			ps_dbg(tx->sdata,
			       "STA %pM TX buffer for AC %d full - dropping oldest frame\n",
			       sta->sta.addr, ac);
			ieee80211_free_txskb(&local->hw, old);
		} else
			tx->local->total_ps_buffered++;

		info->control.jiffies = jiffies;
		info->control.vif = &tx->sdata->vif;
		info->flags |= IEEE80211_TX_INTFL_NEED_TXPROCESSING;
		skb_queue_tail(&sta->ps_tx_buf[ac], tx->skb);
		spin_unlock(&sta->ps_lock);

		if (!timer_pending(&local->sta_cleanup))
			mod_timer(&local->sta_cleanup,
				  round_jiffies(jiffies +
						STA_INFO_CLEANUP_INTERVAL));

		/*
                                                   
                                    
   */
		sta_info_recalc_tim(sta);

		return TX_QUEUED;
	} else if (unlikely(test_sta_flag(sta, WLAN_STA_PS_STA))) {
		ps_dbg(tx->sdata,
		       "STA %pM in PS mode, but polling/in SP -> send frame\n",
		       sta->sta.addr);
	}

	return TX_CONTINUE;
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_ps_buf(struct ieee80211_tx_data *tx)
{
	if (unlikely(tx->flags & IEEE80211_TX_PS_BUFFERED))
		return TX_CONTINUE;
	if (tx->flags & IEEE80211_TX_UNICAST)
		return ieee80211_tx_h_unicast_ps_buf(tx);
	else
		return ieee80211_tx_h_multicast_ps_buf(tx);
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_check_control_port_protocol(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);

	if (unlikely(tx->sdata->control_port_protocol == tx->skb->protocol &&
		     tx->sdata->control_port_no_encrypt))
		info->flags |= IEEE80211_TX_INTFL_DONT_ENCRYPT;

	return TX_CONTINUE;
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_select_key(struct ieee80211_tx_data *tx)
{
	struct ieee80211_key *key;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;

	if (unlikely(info->flags & IEEE80211_TX_INTFL_DONT_ENCRYPT))
		tx->key = NULL;
	else if (tx->sta && (key = rcu_dereference(tx->sta->ptk)))
		tx->key = key;
	else if (ieee80211_is_mgmt(hdr->frame_control) &&
		 is_multicast_ether_addr(hdr->addr1) &&
		 ieee80211_is_robust_mgmt_frame(hdr) &&
		 (key = rcu_dereference(tx->sdata->default_mgmt_key)))
		tx->key = key;
	else if (is_multicast_ether_addr(hdr->addr1) &&
		 (key = rcu_dereference(tx->sdata->default_multicast_key)))
		tx->key = key;
	else if (!is_multicast_ether_addr(hdr->addr1) &&
		 (key = rcu_dereference(tx->sdata->default_unicast_key)))
		tx->key = key;
	else if (info->flags & IEEE80211_TX_CTL_INJECTED)
		tx->key = NULL;
	else if (!tx->sdata->drop_unencrypted)
		tx->key = NULL;
	else if (tx->skb->protocol == tx->sdata->control_port_protocol)
		tx->key = NULL;
	else if (ieee80211_is_robust_mgmt_frame(hdr) &&
		 !(ieee80211_is_action(hdr->frame_control) &&
		   tx->sta && test_sta_flag(tx->sta, WLAN_STA_MFP)))
		tx->key = NULL;
	else if (ieee80211_is_mgmt(hdr->frame_control) &&
		 !ieee80211_is_robust_mgmt_frame(hdr))
		tx->key = NULL;
	else {
		I802_DEBUG_INC(tx->local->tx_handlers_drop_unencrypted);
		return TX_DROP;
	}

	if (tx->key) {
		bool skip_hw = false;

		tx->key->tx_rx_count++;
		/*                                 */

		switch (tx->key->conf.cipher) {
		case WLAN_CIPHER_SUITE_WEP40:
		case WLAN_CIPHER_SUITE_WEP104:
		case WLAN_CIPHER_SUITE_TKIP:
			if (!ieee80211_is_data_present(hdr->frame_control))
				tx->key = NULL;
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			if (!ieee80211_is_data_present(hdr->frame_control) &&
			    !ieee80211_use_mfp(hdr->frame_control, tx->sta,
					       tx->skb))
				tx->key = NULL;
			else
				skip_hw = (tx->key->conf.flags &
					   IEEE80211_KEY_FLAG_SW_MGMT_TX) &&
					ieee80211_is_mgmt(hdr->frame_control);
			break;
		case WLAN_CIPHER_SUITE_AES_CMAC:
			if (!ieee80211_is_mgmt(hdr->frame_control))
				tx->key = NULL;
			break;
		}

		if (unlikely(tx->key && tx->key->flags & KEY_FLAG_TAINTED &&
			     !ieee80211_is_deauth(hdr->frame_control)))
			return TX_DROP;

		if (!skip_hw && tx->key &&
		    tx->key->flags & KEY_FLAG_UPLOADED_TO_HARDWARE)
			info->control.hw_key = &tx->key->conf;
	}

	return TX_CONTINUE;
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_rate_ctrl(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (void *)tx->skb->data;
	struct ieee80211_supported_band *sband;
	u32 len;
	struct ieee80211_tx_rate_control txrc;
	struct ieee80211_sta_rates *ratetbl = NULL;
	bool assoc = false;

	memset(&txrc, 0, sizeof(txrc));

	sband = tx->local->hw.wiphy->bands[info->band];

	len = min_t(u32, tx->skb->len + FCS_LEN,
			 tx->local->hw.wiphy->frag_threshold);

	/*                                                       */
	txrc.hw = &tx->local->hw;
	txrc.sband = sband;
	txrc.bss_conf = &tx->sdata->vif.bss_conf;
	txrc.skb = tx->skb;
	txrc.reported_rate.idx = -1;
	txrc.rate_idx_mask = tx->sdata->rc_rateidx_mask[info->band];
	if (txrc.rate_idx_mask == (1 << sband->n_bitrates) - 1)
		txrc.max_rate_idx = -1;
	else
		txrc.max_rate_idx = fls(txrc.rate_idx_mask) - 1;

	if (tx->sdata->rc_has_mcs_mask[info->band])
		txrc.rate_idx_mcs_mask =
			tx->sdata->rc_rateidx_mcs_mask[info->band];

	txrc.bss = (tx->sdata->vif.type == NL80211_IFTYPE_AP ||
		    tx->sdata->vif.type == NL80211_IFTYPE_MESH_POINT ||
		    tx->sdata->vif.type == NL80211_IFTYPE_ADHOC);

	/*                                  */
	if (len > tx->local->hw.wiphy->rts_threshold) {
		txrc.rts = true;
	}

	info->control.use_rts = txrc.rts;
	info->control.use_cts_prot = tx->sdata->vif.bss_conf.use_cts_prot;

	/*
                                                            
                                                            
                                                           
                                
  */
	if (tx->sdata->vif.bss_conf.use_short_preamble &&
	    (ieee80211_is_data(hdr->frame_control) ||
	     (tx->sta && test_sta_flag(tx->sta, WLAN_STA_SHORT_PREAMBLE))))
		txrc.short_preamble = true;

	info->control.short_preamble = txrc.short_preamble;

	if (tx->sta)
		assoc = test_sta_flag(tx->sta, WLAN_STA_ASSOC);

	/*
                                                               
                                            
  */
	if (WARN(test_bit(SCAN_SW_SCANNING, &tx->local->scanning) && assoc &&
		 !rate_usable_index_exists(sband, &tx->sta->sta),
		 "%s: Dropped data frame as no usable bitrate found while "
		 "scanning and associated. Target station: "
		 "%pM on %d GHz band\n",
		 tx->sdata->name, hdr->addr1,
		 info->band ? 5 : 2))
		return TX_DROP;

	/*
                                                                    
                                                
  */
	rate_control_get_rate(tx->sdata, tx->sta, &txrc);

	if (tx->sta && !info->control.skip_table)
		ratetbl = rcu_dereference(tx->sta->sta.rates);

	if (unlikely(info->control.rates[0].idx < 0)) {
		if (ratetbl) {
			struct ieee80211_tx_rate rate = {
				.idx = ratetbl->rate[0].idx,
				.flags = ratetbl->rate[0].flags,
				.count = ratetbl->rate[0].count
			};

			if (ratetbl->rate[0].idx < 0)
				return TX_DROP;

			tx->rate = rate;
		} else {
			return TX_DROP;
		}
	} else {
		tx->rate = info->control.rates[0];
	}

	if (txrc.reported_rate.idx < 0) {
		txrc.reported_rate = tx->rate;
		if (tx->sta && ieee80211_is_data(hdr->frame_control))
			tx->sta->last_tx_rate = txrc.reported_rate;
	} else if (tx->sta)
		tx->sta->last_tx_rate = txrc.reported_rate;

	if (ratetbl)
		return TX_CONTINUE;

	if (unlikely(!info->control.rates[0].count))
		info->control.rates[0].count = 1;

	if (WARN_ON_ONCE((info->control.rates[0].count > 1) &&
			 (info->flags & IEEE80211_TX_CTL_NO_ACK)))
		info->control.rates[0].count = 1;

	return TX_CONTINUE;
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_sequence(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)tx->skb->data;
	u16 *seq;
	u8 *qc;
	int tid;

	/*
                                                     
                                                    
                                                       
  */
	if (unlikely(info->control.vif->type == NL80211_IFTYPE_MONITOR))
		return TX_CONTINUE;

	if (unlikely(ieee80211_is_ctl(hdr->frame_control)))
		return TX_CONTINUE;

	if (ieee80211_hdrlen(hdr->frame_control) < 24)
		return TX_CONTINUE;

	if (ieee80211_is_qos_nullfunc(hdr->frame_control))
		return TX_CONTINUE;

	/*
                                                          
                                                           
            
  */
	if (!ieee80211_is_data_qos(hdr->frame_control)) {
		/*                                      */
		info->flags |= IEEE80211_TX_CTL_ASSIGN_SEQ;
		/*                                                 */
		hdr->seq_ctrl = cpu_to_le16(tx->sdata->sequence_number);
		tx->sdata->sequence_number += 0x10;
		return TX_CONTINUE;
	}

	/*
                                                                
                                                                 
                                             
  */
	if (!tx->sta)
		return TX_CONTINUE;

	/*                                           */

	qc = ieee80211_get_qos_ctl(hdr);
	tid = *qc & IEEE80211_QOS_CTL_TID_MASK;
	seq = &tx->sta->tid_seq[tid];

	hdr->seq_ctrl = cpu_to_le16(*seq);

	/*                               */
	*seq = (*seq + 0x10) & IEEE80211_SCTL_SEQ;

	return TX_CONTINUE;
}

static int ieee80211_fragment(struct ieee80211_tx_data *tx,
			      struct sk_buff *skb, int hdrlen,
			      int frag_threshold)
{
	struct ieee80211_local *local = tx->local;
	struct ieee80211_tx_info *info;
	struct sk_buff *tmp;
	int per_fragm = frag_threshold - hdrlen - FCS_LEN;
	int pos = hdrlen + per_fragm;
	int rem = skb->len - hdrlen - per_fragm;

	if (WARN_ON(rem < 0))
		return -EINVAL;

	/*                                                     */

	while (rem) {
		int fraglen = per_fragm;

		if (fraglen > rem)
			fraglen = rem;
		rem -= fraglen;
		tmp = dev_alloc_skb(local->tx_headroom +
				    frag_threshold +
				    IEEE80211_ENCRYPT_HEADROOM +
				    IEEE80211_ENCRYPT_TAILROOM);
		if (!tmp)
			return -ENOMEM;

		__skb_queue_tail(&tx->skbs, tmp);

		skb_reserve(tmp, local->tx_headroom +
				 IEEE80211_ENCRYPT_HEADROOM);
		/*                          */
		memcpy(tmp->cb, skb->cb, sizeof(tmp->cb));

		info = IEEE80211_SKB_CB(tmp);
		info->flags &= ~(IEEE80211_TX_CTL_CLEAR_PS_FILT |
				 IEEE80211_TX_CTL_FIRST_FRAGMENT);

		if (rem)
			info->flags |= IEEE80211_TX_CTL_MORE_FRAMES;

		skb_copy_queue_mapping(tmp, skb);
		tmp->priority = skb->priority;
		tmp->dev = skb->dev;

		/*                      */
		memcpy(skb_put(tmp, hdrlen), skb->data, hdrlen);
		memcpy(skb_put(tmp, fraglen), skb->data + pos, fraglen);

		pos += fraglen;
	}

	/*                                */
	skb_trim(skb, hdrlen + per_fragm);
	return 0;
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_fragment(struct ieee80211_tx_data *tx)
{
	struct sk_buff *skb = tx->skb;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_hdr *hdr = (void *)skb->data;
	int frag_threshold = tx->local->hw.wiphy->frag_threshold;
	int hdrlen;
	int fragnum;

	/*                                                   */
	__skb_queue_tail(&tx->skbs, skb);
	tx->skb = NULL;

	if (info->flags & IEEE80211_TX_CTL_DONTFRAG)
		return TX_CONTINUE;

	if (tx->local->ops->set_frag_threshold)
		return TX_CONTINUE;

	/*
                                                               
                                                              
                                                             
  */
	if (WARN_ON(info->flags & IEEE80211_TX_CTL_AMPDU))
		return TX_DROP;

	hdrlen = ieee80211_hdrlen(hdr->frame_control);

	/*                                         */
	if (WARN_ON(skb->len + FCS_LEN <= frag_threshold))
		return TX_DROP;

	/*
                                                                    
                                                              
                                                                    
                                                                   
                                                                  
                                 
  */
	if (ieee80211_fragment(tx, skb, hdrlen, frag_threshold))
		return TX_DROP;

	/*                                        */
	fragnum = 0;

	skb_queue_walk(&tx->skbs, skb) {
		const __le16 morefrags = cpu_to_le16(IEEE80211_FCTL_MOREFRAGS);

		hdr = (void *)skb->data;
		info = IEEE80211_SKB_CB(skb);

		if (!skb_queue_is_last(&tx->skbs, skb)) {
			hdr->frame_control |= morefrags;
			/*
                                                       
                                                       
    */
			info->control.rates[1].idx = -1;
			info->control.rates[2].idx = -1;
			info->control.rates[3].idx = -1;
			BUILD_BUG_ON(IEEE80211_TX_MAX_RATES != 4);
			info->flags &= ~IEEE80211_TX_CTL_RATE_CTRL_PROBE;
		} else {
			hdr->frame_control &= ~morefrags;
		}
		hdr->seq_ctrl |= cpu_to_le16(fragnum & IEEE80211_SCTL_FRAG);
		fragnum++;
	}

	return TX_CONTINUE;
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_stats(struct ieee80211_tx_data *tx)
{
	struct sk_buff *skb;
	int ac = -1;

	if (!tx->sta)
		return TX_CONTINUE;

	skb_queue_walk(&tx->skbs, skb) {
		ac = skb_get_queue_mapping(skb);
		tx->sta->tx_fragments++;
		tx->sta->tx_bytes[ac] += skb->len;
	}
	if (ac >= 0)
		tx->sta->tx_packets[ac]++;

	return TX_CONTINUE;
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_encrypt(struct ieee80211_tx_data *tx)
{
	if (!tx->key)
		return TX_CONTINUE;

	switch (tx->key->conf.cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		return ieee80211_crypto_wep_encrypt(tx);
	case WLAN_CIPHER_SUITE_TKIP:
		return ieee80211_crypto_tkip_encrypt(tx);
	case WLAN_CIPHER_SUITE_CCMP:
		return ieee80211_crypto_ccmp_encrypt(tx);
	case WLAN_CIPHER_SUITE_AES_CMAC:
		return ieee80211_crypto_aes_cmac_encrypt(tx);
	default:
		return ieee80211_crypto_hw_encrypt(tx);
	}

	return TX_DROP;
}

static ieee80211_tx_result debug_noinline
ieee80211_tx_h_calculate_duration(struct ieee80211_tx_data *tx)
{
	struct sk_buff *skb;
	struct ieee80211_hdr *hdr;
	int next_len;
	bool group_addr;

	skb_queue_walk(&tx->skbs, skb) {
		hdr = (void *) skb->data;
		if (unlikely(ieee80211_is_pspoll(hdr->frame_control)))
			break; /*                        */
		if (!skb_queue_is_last(&tx->skbs, skb)) {
			struct sk_buff *next = skb_queue_next(&tx->skbs, skb);
			next_len = next->len;
		} else
			next_len = 0;
		group_addr = is_multicast_ether_addr(hdr->addr1);

		hdr->duration_id =
			ieee80211_duration(tx, skb, group_addr, next_len);
	}

	return TX_CONTINUE;
}

/*                      */

static bool ieee80211_tx_prep_agg(struct ieee80211_tx_data *tx,
				  struct sk_buff *skb,
				  struct ieee80211_tx_info *info,
				  struct tid_ampdu_tx *tid_tx,
				  int tid)
{
	bool queued = false;
	bool reset_agg_timer = false;
	struct sk_buff *purge_skb = NULL;

	if (test_bit(HT_AGG_STATE_OPERATIONAL, &tid_tx->state)) {
		info->flags |= IEEE80211_TX_CTL_AMPDU;
		reset_agg_timer = true;
	} else if (test_bit(HT_AGG_STATE_WANT_START, &tid_tx->state)) {
		/*
                                                         
                                              
   */
	} else {
		spin_lock(&tx->sta->lock);
		/*
                                                  
    
                                                         
                                                         
                                                       
                                                          
                                                       
                                                       
    
                                                        
                                                        
                                                       
                                                       
                                                       
                                                        
                              
   */
		tid_tx = rcu_dereference_protected_tid_tx(tx->sta, tid);

		if (!tid_tx) {
			/*                                     */
		} else if (test_bit(HT_AGG_STATE_OPERATIONAL, &tid_tx->state)) {
			info->flags |= IEEE80211_TX_CTL_AMPDU;
			reset_agg_timer = true;
		} else {
			queued = true;
			info->control.vif = &tx->sdata->vif;
			info->flags |= IEEE80211_TX_INTFL_NEED_TXPROCESSING;
			__skb_queue_tail(&tid_tx->pending, skb);
			if (skb_queue_len(&tid_tx->pending) > STA_MAX_TX_BUFFER)
				purge_skb = __skb_dequeue(&tid_tx->pending);
		}
		spin_unlock(&tx->sta->lock);

		if (purge_skb)
			ieee80211_free_txskb(&tx->local->hw, purge_skb);
	}

	/*                     */
	if (reset_agg_timer && tid_tx->timeout)
		tid_tx->last_tx = jiffies;

	return queued;
}

/*
                  
 */
static ieee80211_tx_result
ieee80211_tx_prepare(struct ieee80211_sub_if_data *sdata,
		     struct ieee80211_tx_data *tx,
		     struct sk_buff *skb)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_hdr *hdr;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	int tid;
	u8 *qc;

	memset(tx, 0, sizeof(*tx));
	tx->skb = skb;
	tx->local = local;
	tx->sdata = sdata;
	__skb_queue_head_init(&tx->skbs);

	/*
                                                          
                                                          
        
  */
	info->flags &= ~IEEE80211_TX_INTFL_NEED_TXPROCESSING;

	hdr = (struct ieee80211_hdr *) skb->data;

	if (sdata->vif.type == NL80211_IFTYPE_AP_VLAN) {
		tx->sta = rcu_dereference(sdata->u.vlan.sta);
		if (!tx->sta && sdata->dev->ieee80211_ptr->use_4addr)
			return TX_DROP;
	} else if (info->flags & (IEEE80211_TX_CTL_INJECTED |
				  IEEE80211_TX_INTFL_NL80211_FRAME_TX) ||
		   tx->sdata->control_port_protocol == tx->skb->protocol) {
		tx->sta = sta_info_get_bss(sdata, hdr->addr1);
	}
	if (!tx->sta)
		tx->sta = sta_info_get(sdata, hdr->addr1);

	if (tx->sta && ieee80211_is_data_qos(hdr->frame_control) &&
	    !ieee80211_is_qos_nullfunc(hdr->frame_control) &&
	    (local->hw.flags & IEEE80211_HW_AMPDU_AGGREGATION) &&
	    !(local->hw.flags & IEEE80211_HW_TX_AMPDU_SETUP_IN_HW)) {
		struct tid_ampdu_tx *tid_tx;

		qc = ieee80211_get_qos_ctl(hdr);
		tid = *qc & IEEE80211_QOS_CTL_TID_MASK;

		tid_tx = rcu_dereference(tx->sta->ampdu_mlme.tid_tx[tid]);
		if (tid_tx) {
			bool queued;

			queued = ieee80211_tx_prep_agg(tx, skb, info,
						       tid_tx, tid);

			if (unlikely(queued))
				return TX_QUEUED;
		}
	}

	if (is_multicast_ether_addr(hdr->addr1)) {
		tx->flags &= ~IEEE80211_TX_UNICAST;
		info->flags |= IEEE80211_TX_CTL_NO_ACK;
	} else
		tx->flags |= IEEE80211_TX_UNICAST;

	if (!(info->flags & IEEE80211_TX_CTL_DONTFRAG)) {
		if (!(tx->flags & IEEE80211_TX_UNICAST) ||
		    skb->len + FCS_LEN <= local->hw.wiphy->frag_threshold ||
		    info->flags & IEEE80211_TX_CTL_AMPDU)
			info->flags |= IEEE80211_TX_CTL_DONTFRAG;
	}

	if (!tx->sta)
		info->flags |= IEEE80211_TX_CTL_CLEAR_PS_FILT;
	else if (test_and_clear_sta_flag(tx->sta, WLAN_STA_CLEAR_PS_FILT))
		info->flags |= IEEE80211_TX_CTL_CLEAR_PS_FILT;

	info->flags |= IEEE80211_TX_CTL_FIRST_FRAGMENT;

	return TX_CONTINUE;
}

static bool ieee80211_tx_frags(struct ieee80211_local *local,
			       struct ieee80211_vif *vif,
			       struct ieee80211_sta *sta,
			       struct sk_buff_head *skbs,
			       bool txpending)
{
	struct ieee80211_tx_control control;
	struct sk_buff *skb, *tmp;
	unsigned long flags;

	skb_queue_walk_safe(skbs, skb, tmp) {
		struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
		int q = info->hw_queue;

#ifdef CONFIG_MAC80211_VERBOSE_DEBUG
		if (WARN_ON_ONCE(q >= local->hw.queues)) {
			__skb_unlink(skb, skbs);
			ieee80211_free_txskb(&local->hw, skb);
			continue;
		}
#endif

		spin_lock_irqsave(&local->queue_stop_reason_lock, flags);
		if (local->queue_stop_reasons[q] ||
		    (!txpending && !skb_queue_empty(&local->pending[q]))) {
			if (unlikely(info->flags &
				     IEEE80211_TX_INTFL_OFFCHAN_TX_OK)) {
				if (local->queue_stop_reasons[q] &
				    ~BIT(IEEE80211_QUEUE_STOP_REASON_OFFCHANNEL)) {
					/*
                                         
                                        
                                         
                   
      */
					spin_unlock_irqrestore(
						&local->queue_stop_reason_lock,
						flags);
					ieee80211_purge_tx_queue(&local->hw,
								 skbs);
					return true;
				}
			} else {

				/*
                                                  
                                             
                                             
     */
				if (txpending)
					skb_queue_splice_init(skbs,
							      &local->pending[q]);
				else
					skb_queue_splice_tail_init(skbs,
								   &local->pending[q]);

				spin_unlock_irqrestore(&local->queue_stop_reason_lock,
						       flags);
				return false;
			}
		}
		spin_unlock_irqrestore(&local->queue_stop_reason_lock, flags);

		info->control.vif = vif;
		control.sta = sta;

		__skb_unlink(skb, skbs);
		drv_tx(local, &control, skb);
	}

	return true;
}

/*
                                                                             
 */
static bool __ieee80211_tx(struct ieee80211_local *local,
			   struct sk_buff_head *skbs, int led_len,
			   struct sta_info *sta, bool txpending)
{
	struct ieee80211_tx_info *info;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_vif *vif;
	struct ieee80211_sta *pubsta;
	struct sk_buff *skb;
	bool result = true;
	__le16 fc;

	if (WARN_ON(skb_queue_empty(skbs)))
		return true;

	skb = skb_peek(skbs);
	fc = ((struct ieee80211_hdr *)skb->data)->frame_control;
	info = IEEE80211_SKB_CB(skb);
	sdata = vif_to_sdata(info->control.vif);
	if (sta && !sta->uploaded)
		sta = NULL;

	if (sta)
		pubsta = &sta->sta;
	else
		pubsta = NULL;

	switch (sdata->vif.type) {
	case NL80211_IFTYPE_MONITOR:
		sdata = rcu_dereference(local->monitor_sdata);
		if (sdata) {
			vif = &sdata->vif;
			info->hw_queue =
				vif->hw_queue[skb_get_queue_mapping(skb)];
		} else if (local->hw.flags & IEEE80211_HW_QUEUE_CONTROL) {
			dev_kfree_skb(skb);
			return true;
		} else
			vif = NULL;
		break;
	case NL80211_IFTYPE_AP_VLAN:
		sdata = container_of(sdata->bss,
				     struct ieee80211_sub_if_data, u.ap);
		/*              */
	default:
		vif = &sdata->vif;
		break;
	}

	result = ieee80211_tx_frags(local, vif, pubsta, skbs,
				    txpending);

	ieee80211_tpt_led_trig_tx(local, fc, led_len);
	ieee80211_led_tx(local, 1);

	WARN_ON_ONCE(!skb_queue_empty(skbs));

	return result;
}

/*
                                                              
                               
 */
static int invoke_tx_handlers(struct ieee80211_tx_data *tx)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx->skb);
	ieee80211_tx_result res = TX_DROP;

#define CALL_TXH(txh) \
	do {				\
		res = txh(tx);		\
		if (res != TX_CONTINUE)	\
			goto txh_done;	\
	} while (0)

	CALL_TXH(ieee80211_tx_h_dynamic_ps);
	CALL_TXH(ieee80211_tx_h_check_assoc);
	CALL_TXH(ieee80211_tx_h_ps_buf);
	CALL_TXH(ieee80211_tx_h_check_control_port_protocol);
	CALL_TXH(ieee80211_tx_h_select_key);
	if (!(tx->local->hw.flags & IEEE80211_HW_HAS_RATE_CONTROL))
		CALL_TXH(ieee80211_tx_h_rate_ctrl);

	if (unlikely(info->flags & IEEE80211_TX_INTFL_RETRANSMISSION)) {
		__skb_queue_tail(&tx->skbs, tx->skb);
		tx->skb = NULL;
		goto txh_done;
	}

	CALL_TXH(ieee80211_tx_h_michael_mic_add);
	CALL_TXH(ieee80211_tx_h_sequence);
	CALL_TXH(ieee80211_tx_h_fragment);
	/*                                                                 */
	CALL_TXH(ieee80211_tx_h_stats);
	CALL_TXH(ieee80211_tx_h_encrypt);
	if (!(tx->local->hw.flags & IEEE80211_HW_HAS_RATE_CONTROL))
		CALL_TXH(ieee80211_tx_h_calculate_duration);
#undef CALL_TXH

 txh_done:
	if (unlikely(res == TX_DROP)) {
		I802_DEBUG_INC(tx->local->tx_handlers_drop);
		if (tx->skb)
			ieee80211_free_txskb(&tx->local->hw, tx->skb);
		else
			ieee80211_purge_tx_queue(&tx->local->hw, &tx->skbs);
		return -1;
	} else if (unlikely(res == TX_QUEUED)) {
		I802_DEBUG_INC(tx->local->tx_handlers_queued);
		return -1;
	}

	return 0;
}

/*
                                                                             
 */
static bool ieee80211_tx(struct ieee80211_sub_if_data *sdata,
			 struct sk_buff *skb, bool txpending,
			 enum ieee80211_band band)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_tx_data tx;
	ieee80211_tx_result res_prepare;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	bool result = true;
	int led_len;

	if (unlikely(skb->len < 10)) {
		dev_kfree_skb(skb);
		return true;
	}

	/*                */
	led_len = skb->len;
	res_prepare = ieee80211_tx_prepare(sdata, &tx, skb);

	if (unlikely(res_prepare == TX_DROP)) {
		ieee80211_free_txskb(&local->hw, skb);
		return true;
	} else if (unlikely(res_prepare == TX_QUEUED)) {
		return true;
	}

	info->band = band;

	/*                             */
	if (!(info->flags & IEEE80211_TX_CTL_TX_OFFCHAN) ||
	    !(local->hw.flags & IEEE80211_HW_QUEUE_CONTROL))
		info->hw_queue =
			sdata->vif.hw_queue[skb_get_queue_mapping(skb)];

	if (!invoke_tx_handlers(&tx))
		result = __ieee80211_tx(local, &tx.skbs, led_len,
					tx.sta, txpending);

	return result;
}

/*                      */

static int ieee80211_skb_resize(struct ieee80211_sub_if_data *sdata,
				struct sk_buff *skb,
				int head_need, bool may_encrypt)
{
	struct ieee80211_local *local = sdata->local;
	int tail_need = 0;

	if (may_encrypt && sdata->crypto_tx_tailroom_needed_cnt) {
		tail_need = IEEE80211_ENCRYPT_TAILROOM;
		tail_need -= skb_tailroom(skb);
		tail_need = max_t(int, tail_need, 0);
	}

	if (skb_cloned(skb))
		I802_DEBUG_INC(local->tx_expand_skb_head_cloned);
	else if (head_need || tail_need)
		I802_DEBUG_INC(local->tx_expand_skb_head);
	else
		return 0;

	if (pskb_expand_head(skb, head_need, tail_need, GFP_ATOMIC)) {
		wiphy_debug(local->hw.wiphy,
			    "failed to reallocate TX buffer\n");
		return -ENOMEM;
	}

	return 0;
}

void ieee80211_xmit(struct ieee80211_sub_if_data *sdata, struct sk_buff *skb,
		    enum ieee80211_band band)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	int headroom;
	bool may_encrypt;

	may_encrypt = !(info->flags & IEEE80211_TX_INTFL_DONT_ENCRYPT);

	headroom = local->tx_headroom;
	if (may_encrypt)
		headroom += IEEE80211_ENCRYPT_HEADROOM;
	headroom -= skb_headroom(skb);
	headroom = max_t(int, 0, headroom);

	if (ieee80211_skb_resize(sdata, skb, headroom, may_encrypt)) {
		ieee80211_free_txskb(&local->hw, skb);
		return;
	}

	hdr = (struct ieee80211_hdr *) skb->data;
	info->control.vif = &sdata->vif;

	if (ieee80211_vif_is_mesh(&sdata->vif)) {
		if (ieee80211_is_data(hdr->frame_control) &&
		    is_unicast_ether_addr(hdr->addr1)) {
			if (mesh_nexthop_resolve(sdata, skb))
				return; /*                        */
		} else {
			ieee80211_mps_set_frame_flags(sdata, NULL, hdr);
		}
	}

	ieee80211_set_qos_hdr(sdata, skb);
	ieee80211_tx(sdata, skb, false, band);
}

static bool ieee80211_parse_tx_radiotap(struct sk_buff *skb)
{
	struct ieee80211_radiotap_iterator iterator;
	struct ieee80211_radiotap_header *rthdr =
		(struct ieee80211_radiotap_header *) skb->data;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	int ret = ieee80211_radiotap_iterator_init(&iterator, rthdr, skb->len,
						   NULL);
	u16 txflags;

	info->flags |= IEEE80211_TX_INTFL_DONT_ENCRYPT |
		       IEEE80211_TX_CTL_DONTFRAG;

	/*
                                            
                                                                  
                                         
  */

	while (!ret) {
		ret = ieee80211_radiotap_iterator_next(&iterator);

		if (ret)
			continue;

		/*                                              */
		switch (iterator.this_arg_index) {
		/*
                                                            
                                                            
                                                            
                                                            
  */
		case IEEE80211_RADIOTAP_FLAGS:
			if (*iterator.this_arg & IEEE80211_RADIOTAP_F_FCS) {
				/*
                                               
                                                  
                                                 
                                              
                      
     */
				if (skb->len < (iterator._max_length + FCS_LEN))
					return false;

				skb_trim(skb, skb->len - FCS_LEN);
			}
			if (*iterator.this_arg & IEEE80211_RADIOTAP_F_WEP)
				info->flags &= ~IEEE80211_TX_INTFL_DONT_ENCRYPT;
			if (*iterator.this_arg & IEEE80211_RADIOTAP_F_FRAG)
				info->flags &= ~IEEE80211_TX_CTL_DONTFRAG;
			break;

		case IEEE80211_RADIOTAP_TX_FLAGS:
			txflags = get_unaligned_le16(iterator.this_arg);
			if (txflags & IEEE80211_RADIOTAP_F_TX_NOACK)
				info->flags |= IEEE80211_TX_CTL_NO_ACK;
			break;

		/*
                           
                                                    
                                  
   */

		default:
			break;
		}
	}

	if (ret != -ENOENT) /*                                           */
		return false;

	/*
                              
                                                    
                             
  */
	skb_pull(skb, iterator._max_length);

	return true;
}

netdev_tx_t ieee80211_monitor_start_xmit(struct sk_buff *skb,
					 struct net_device *dev)
{
	struct ieee80211_local *local = wdev_priv(dev->ieee80211_ptr);
	struct ieee80211_chanctx_conf *chanctx_conf;
	struct ieee80211_channel *chan;
	struct ieee80211_radiotap_header *prthdr =
		(struct ieee80211_radiotap_header *)skb->data;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_hdr *hdr;
	struct ieee80211_sub_if_data *tmp_sdata, *sdata;
	u16 len_rthdr;
	int hdrlen;

	/*                                                          */
	if (unlikely(skb->len < sizeof(struct ieee80211_radiotap_header)))
		goto fail; /*                                */

	/*                                                          */
	if (unlikely(prthdr->it_version))
		goto fail; /*                             */

	/*                                                               */
	len_rthdr = ieee80211_get_radiotap_len(skb->data);

	/*                                                               */
	if (unlikely(skb->len < len_rthdr))
		goto fail; /*                                            */

	/*
                                                   
                                                    
                                               
                     
  */
	skb_set_mac_header(skb, len_rthdr);
	/*
                                                           
                                                                     
  */
	skb_set_network_header(skb, len_rthdr);
	skb_set_transport_header(skb, len_rthdr);

	if (skb->len < len_rthdr + 2)
		goto fail;

	hdr = (struct ieee80211_hdr *)(skb->data + len_rthdr);
	hdrlen = ieee80211_hdrlen(hdr->frame_control);

	if (skb->len < len_rthdr + hdrlen)
		goto fail;

	/*
                                                                  
                             
  */
	if (ieee80211_is_data(hdr->frame_control) &&
	    skb->len >= len_rthdr + hdrlen + sizeof(rfc1042_header) + 2) {
		u8 *payload = (u8 *)hdr + hdrlen;

		if (ether_addr_equal(payload, rfc1042_header))
			skb->protocol = cpu_to_be16((payload[6] << 8) |
						    payload[7]);
	}

	memset(info, 0, sizeof(*info));

	info->flags = IEEE80211_TX_CTL_REQ_TX_STATUS |
		      IEEE80211_TX_CTL_INJECTED;

	/*                                                  */
	if (!ieee80211_parse_tx_radiotap(skb))
		goto fail;

	rcu_read_lock();

	/*
                                                                 
                                                     
                                                                
                                                                
                                                              
                                                 
  */
	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	list_for_each_entry_rcu(tmp_sdata, &local->interfaces, list) {
		if (!ieee80211_sdata_running(tmp_sdata))
			continue;
		if (tmp_sdata->vif.type == NL80211_IFTYPE_MONITOR ||
		    tmp_sdata->vif.type == NL80211_IFTYPE_AP_VLAN ||
		    tmp_sdata->vif.type == NL80211_IFTYPE_WDS)
			continue;
		if (ether_addr_equal(tmp_sdata->vif.addr, hdr->addr2)) {
			sdata = tmp_sdata;
			break;
		}
	}

	chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
	if (!chanctx_conf) {
		tmp_sdata = rcu_dereference(local->monitor_sdata);
		if (tmp_sdata)
			chanctx_conf =
				rcu_dereference(tmp_sdata->vif.chanctx_conf);
	}

	if (chanctx_conf)
		chan = chanctx_conf->def.chan;
	else if (!local->use_chanctx)
		chan = local->_oper_chandef.chan;
	else
		goto fail_rcu;

	/*
                                                              
                                                                        
                                                                 
                                                               
                                                                 
                                                                
                                                                  
         
   
                                                                 
                                                                 
                                                                
                                                               
                                                
  */
	if ((chan->flags & (IEEE80211_CHAN_NO_IBSS | IEEE80211_CHAN_RADAR |
			    IEEE80211_CHAN_PASSIVE_SCAN)))
		goto fail_rcu;

	ieee80211_xmit(sdata, skb, chan->band);
	rcu_read_unlock();

	return NETDEV_TX_OK;

fail_rcu:
	rcu_read_unlock();
fail:
	dev_kfree_skb(skb);
	return NETDEV_TX_OK; /*                                */
}

/* 
                                                                           
                                                  
                          
                           
  
                                                                               
                                                                               
        
  
                                                                              
                                                                           
                                                                             
                                           
 */
netdev_tx_t ieee80211_subif_start_xmit(struct sk_buff *skb,
				    struct net_device *dev)
{
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_tx_info *info;
	int head_need;
	u16 ethertype, hdrlen,  meshhdrlen = 0;
	__le16 fc;
	struct ieee80211_hdr hdr;
	struct ieee80211s_hdr mesh_hdr __maybe_unused;
	struct mesh_path __maybe_unused *mppath = NULL, *mpath = NULL;
	const u8 *encaps_data;
	int encaps_len, skip_header_bytes;
	int nh_pos, h_pos;
	struct sta_info *sta = NULL;
	bool wme_sta = false, authorized = false, tdls_auth = false;
	bool tdls_direct = false;
	bool multicast;
	u32 info_flags = 0;
	u16 info_id = 0;
	struct ieee80211_chanctx_conf *chanctx_conf;
	struct ieee80211_sub_if_data *ap_sdata;
	enum ieee80211_band band;

	if (unlikely(skb->len < ETH_HLEN))
		goto fail;

	/*                                                          
                    */
	ethertype = (skb->data[12] << 8) | skb->data[13];
	fc = cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_DATA);

	rcu_read_lock();

	switch (sdata->vif.type) {
	case NL80211_IFTYPE_AP_VLAN:
		sta = rcu_dereference(sdata->u.vlan.sta);
		if (sta) {
			fc |= cpu_to_le16(IEEE80211_FCTL_FROMDS | IEEE80211_FCTL_TODS);
			/*             */
			memcpy(hdr.addr1, sta->sta.addr, ETH_ALEN);
			memcpy(hdr.addr2, sdata->vif.addr, ETH_ALEN);
			memcpy(hdr.addr3, skb->data, ETH_ALEN);
			memcpy(hdr.addr4, skb->data + ETH_ALEN, ETH_ALEN);
			hdrlen = 30;
			authorized = test_sta_flag(sta, WLAN_STA_AUTHORIZED);
			wme_sta = test_sta_flag(sta, WLAN_STA_WME);
		}
		ap_sdata = container_of(sdata->bss, struct ieee80211_sub_if_data,
					u.ap);
		chanctx_conf = rcu_dereference(ap_sdata->vif.chanctx_conf);
		if (!chanctx_conf)
			goto fail_rcu;
		band = chanctx_conf->def.chan->band;
		if (sta)
			break;
		/*              */
	case NL80211_IFTYPE_AP:
		if (sdata->vif.type == NL80211_IFTYPE_AP)
			chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
		if (!chanctx_conf)
			goto fail_rcu;
		fc |= cpu_to_le16(IEEE80211_FCTL_FROMDS);
		/*             */
		memcpy(hdr.addr1, skb->data, ETH_ALEN);
		memcpy(hdr.addr2, sdata->vif.addr, ETH_ALEN);
		memcpy(hdr.addr3, skb->data + ETH_ALEN, ETH_ALEN);
		hdrlen = 24;
		band = chanctx_conf->def.chan->band;
		break;
	case NL80211_IFTYPE_WDS:
		fc |= cpu_to_le16(IEEE80211_FCTL_FROMDS | IEEE80211_FCTL_TODS);
		/*             */
		memcpy(hdr.addr1, sdata->u.wds.remote_addr, ETH_ALEN);
		memcpy(hdr.addr2, sdata->vif.addr, ETH_ALEN);
		memcpy(hdr.addr3, skb->data, ETH_ALEN);
		memcpy(hdr.addr4, skb->data + ETH_ALEN, ETH_ALEN);
		hdrlen = 30;
		/*
                                                               
                                                            
   */
		band = local->hw.conf.chandef.chan->band;
		break;
#ifdef CONFIG_MAC80211_MESH
	case NL80211_IFTYPE_MESH_POINT:
		if (!sdata->u.mesh.mshcfg.dot11MeshTTL) {
			/*                                       */
			sdata->u.mesh.mshstats.dropped_frames_ttl++;
			goto fail_rcu;
		}

		if (!is_multicast_ether_addr(skb->data)) {
			struct sta_info *next_hop;
			bool mpp_lookup = true;

			mpath = mesh_path_lookup(sdata, skb->data);
			if (mpath) {
				mpp_lookup = false;
				next_hop = rcu_dereference(mpath->next_hop);
				if (!next_hop ||
				    !(mpath->flags & (MESH_PATH_ACTIVE |
						      MESH_PATH_RESOLVING)))
					mpp_lookup = true;
			}

			if (mpp_lookup)
				mppath = mpp_path_lookup(sdata, skb->data);

			if (mppath && mpath)
				mesh_path_del(mpath->sdata, mpath->dst);
		}

		/*
                                                 
                                                    
                                                      
                                  
   */
		if (ether_addr_equal(sdata->vif.addr, skb->data + ETH_ALEN) &&
		    !(mppath && !ether_addr_equal(mppath->mpp, skb->data))) {
			hdrlen = ieee80211_fill_mesh_addresses(&hdr, &fc,
					skb->data, skb->data + ETH_ALEN);
			meshhdrlen = ieee80211_new_mesh_header(sdata, &mesh_hdr,
							       NULL, NULL);
		} else {
			/*                                    
                                                      
                                                       
                                                    
                                                        
                                                       */
			const u8 *mesh_da = skb->data;

			if (mppath)
				mesh_da = mppath->mpp;
			else if (mpath)
				mesh_da = mpath->dst;

			hdrlen = ieee80211_fill_mesh_addresses(&hdr, &fc,
					mesh_da, sdata->vif.addr);
			if (is_multicast_ether_addr(mesh_da))
				/*                 */
				meshhdrlen = ieee80211_new_mesh_header(
						sdata, &mesh_hdr,
						skb->data + ETH_ALEN, NULL);
			else
				/*                        */
				meshhdrlen = ieee80211_new_mesh_header(
						sdata, &mesh_hdr, skb->data,
						skb->data + ETH_ALEN);

		}
		chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
		if (!chanctx_conf)
			goto fail_rcu;
		band = chanctx_conf->def.chan->band;
		break;
#endif
	case NL80211_IFTYPE_STATION:
		if (sdata->wdev.wiphy->flags & WIPHY_FLAG_SUPPORTS_TDLS) {
			bool tdls_peer = false;

			sta = sta_info_get(sdata, skb->data);
			if (sta) {
				authorized = test_sta_flag(sta,
							WLAN_STA_AUTHORIZED);
				wme_sta = test_sta_flag(sta, WLAN_STA_WME);
				tdls_peer = test_sta_flag(sta,
							 WLAN_STA_TDLS_PEER);
				tdls_auth = test_sta_flag(sta,
						WLAN_STA_TDLS_PEER_AUTH);
			}

			/*
                                                  
                                                  
                                   
    */
			tdls_direct = tdls_peer && (tdls_auth ||
				 !(ethertype == ETH_P_TDLS && skb->len > 14 &&
				   skb->data[14] == WLAN_TDLS_SNAP_RFTYPE));
		}

		if (tdls_direct) {
			/*                                              */
			if (!tdls_auth)
				goto fail_rcu;

			/*             */
			memcpy(hdr.addr1, skb->data, ETH_ALEN);
			memcpy(hdr.addr2, skb->data + ETH_ALEN, ETH_ALEN);
			memcpy(hdr.addr3, sdata->u.mgd.bssid, ETH_ALEN);
			hdrlen = 24;
		}  else if (sdata->u.mgd.use_4addr &&
			    cpu_to_be16(ethertype) != sdata->control_port_protocol) {
			fc |= cpu_to_le16(IEEE80211_FCTL_FROMDS |
					  IEEE80211_FCTL_TODS);
			/*             */
			memcpy(hdr.addr1, sdata->u.mgd.bssid, ETH_ALEN);
			memcpy(hdr.addr2, sdata->vif.addr, ETH_ALEN);
			memcpy(hdr.addr3, skb->data, ETH_ALEN);
			memcpy(hdr.addr4, skb->data + ETH_ALEN, ETH_ALEN);
			hdrlen = 30;
		} else {
			fc |= cpu_to_le16(IEEE80211_FCTL_TODS);
			/*             */
			memcpy(hdr.addr1, sdata->u.mgd.bssid, ETH_ALEN);
			memcpy(hdr.addr2, skb->data + ETH_ALEN, ETH_ALEN);
			memcpy(hdr.addr3, skb->data, ETH_ALEN);
			hdrlen = 24;
		}
		chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
		if (!chanctx_conf)
			goto fail_rcu;
		band = chanctx_conf->def.chan->band;
		break;
	case NL80211_IFTYPE_ADHOC:
		/*             */
		memcpy(hdr.addr1, skb->data, ETH_ALEN);
		memcpy(hdr.addr2, skb->data + ETH_ALEN, ETH_ALEN);
		memcpy(hdr.addr3, sdata->u.ibss.bssid, ETH_ALEN);
		hdrlen = 24;
		chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
		if (!chanctx_conf)
			goto fail_rcu;
		band = chanctx_conf->def.chan->band;
		break;
	default:
		goto fail_rcu;
	}

	/*
                                                     
                                                       
               
  */
	multicast = is_multicast_ether_addr(hdr.addr1);
	if (!multicast) {
		sta = sta_info_get(sdata, hdr.addr1);
		if (sta) {
			authorized = test_sta_flag(sta, WLAN_STA_AUTHORIZED);
			wme_sta = test_sta_flag(sta, WLAN_STA_WME);
		}
	}

	/*                                                  */
	if (ieee80211_vif_is_mesh(&sdata->vif))
		wme_sta = true;

	/*                                                       */
	if (wme_sta && local->hw.queues >= IEEE80211_NUM_ACS) {
		fc |= cpu_to_le16(IEEE80211_STYPE_QOS_DATA);
		hdrlen += 2;
	}

	/*
                                                                
                                        
  */
	if (unlikely(!ieee80211_vif_is_mesh(&sdata->vif) &&
		     !is_multicast_ether_addr(hdr.addr1) && !authorized &&
		     (cpu_to_be16(ethertype) != sdata->control_port_protocol ||
		      !ether_addr_equal(sdata->vif.addr, skb->data + ETH_ALEN)))) {
#ifdef CONFIG_MAC80211_VERBOSE_DEBUG
		net_info_ratelimited("%s: dropped frame to %pM (unauthorized port)\n",
				    dev->name, hdr.addr1);
#endif

		I802_DEBUG_INC(local->tx_handlers_drop_unauth_port);

		goto fail_rcu;
	}

	if (unlikely(!multicast && skb->sk &&
		     skb_shinfo(skb)->tx_flags & SKBTX_WIFI_STATUS)) {
		struct sk_buff *orig_skb = skb;

		skb = skb_clone(skb, GFP_ATOMIC);
		if (skb) {
			unsigned long flags;
			int id;

			spin_lock_irqsave(&local->ack_status_lock, flags);
			id = idr_alloc(&local->ack_status_frames, orig_skb,
				       1, 0x10000, GFP_ATOMIC);
			spin_unlock_irqrestore(&local->ack_status_lock, flags);

			if (id >= 0) {
				info_id = id;
				info_flags |= IEEE80211_TX_CTL_REQ_TX_STATUS;
			} else if (skb_shared(skb)) {
				kfree_skb(orig_skb);
			} else {
				kfree_skb(skb);
				skb = orig_skb;
			}
		} else {
			/*                                      */
			skb = orig_skb;
		}
	}

	/*
                                                        
  */
	if (skb_shared(skb)) {
		struct sk_buff *tmp_skb = skb;

		/*                                                */
		WARN_ON(info_id);

		skb = skb_clone(skb, GFP_ATOMIC);
		kfree_skb(tmp_skb);

		if (!skb)
			goto fail_rcu;
	}

	hdr.frame_control = fc;
	hdr.duration_id = 0;
	hdr.seq_ctrl = 0;

	skip_header_bytes = ETH_HLEN;
	if (ethertype == ETH_P_AARP || ethertype == ETH_P_IPX) {
		encaps_data = bridge_tunnel_header;
		encaps_len = sizeof(bridge_tunnel_header);
		skip_header_bytes -= 2;
	} else if (ethertype >= ETH_P_802_3_MIN) {
		encaps_data = rfc1042_header;
		encaps_len = sizeof(rfc1042_header);
		skip_header_bytes -= 2;
	} else {
		encaps_data = NULL;
		encaps_len = 0;
	}

	nh_pos = skb_network_header(skb) - skb->data;
	h_pos = skb_transport_header(skb) - skb->data;

	skb_pull(skb, skip_header_bytes);
	nh_pos -= skip_header_bytes;
	h_pos -= skip_header_bytes;

	head_need = hdrlen + encaps_len + meshhdrlen - skb_headroom(skb);

	/*
                                                                
                                                               
                                                                
                                                              
                                                      
   
                                                                 
                                                                    
                                                       
  */

	if (head_need > 0 || skb_cloned(skb)) {
		head_need += IEEE80211_ENCRYPT_HEADROOM;
		head_need += local->tx_headroom;
		head_need = max_t(int, 0, head_need);
		if (ieee80211_skb_resize(sdata, skb, head_need, true)) {
			ieee80211_free_txskb(&local->hw, skb);
			skb = NULL;
			goto fail_rcu;
		}
	}

	if (encaps_data) {
		memcpy(skb_push(skb, encaps_len), encaps_data, encaps_len);
		nh_pos += encaps_len;
		h_pos += encaps_len;
	}

#ifdef CONFIG_MAC80211_MESH
	if (meshhdrlen > 0) {
		memcpy(skb_push(skb, meshhdrlen), &mesh_hdr, meshhdrlen);
		nh_pos += meshhdrlen;
		h_pos += meshhdrlen;
	}
#endif

	if (ieee80211_is_data_qos(fc)) {
		__le16 *qos_control;

		qos_control = (__le16*) skb_push(skb, 2);
		memcpy(skb_push(skb, hdrlen - 2), &hdr, hdrlen - 2);
		/*
                                                               
                                                         
   */
		*qos_control = 0;
	} else
		memcpy(skb_push(skb, hdrlen), &hdr, hdrlen);

	nh_pos += hdrlen;
	h_pos += hdrlen;

	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;

	/*                                                                 
                                                                     
                                           */
	skb_set_mac_header(skb, 0);
	skb_set_network_header(skb, nh_pos);
	skb_set_transport_header(skb, h_pos);

	info = IEEE80211_SKB_CB(skb);
	memset(info, 0, sizeof(*info));

	dev->trans_start = jiffies;

	info->flags = info_flags;
	info->ack_frame_id = info_id;

	ieee80211_xmit(sdata, skb, band);
	rcu_read_unlock();

	return NETDEV_TX_OK;

 fail_rcu:
	rcu_read_unlock();
 fail:
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}


/*
                                                                  
                                                      
 */
void ieee80211_clear_tx_pending(struct ieee80211_local *local)
{
	struct sk_buff *skb;
	int i;

	for (i = 0; i < local->hw.queues; i++) {
		while ((skb = skb_dequeue(&local->pending[i])) != NULL)
			ieee80211_free_txskb(&local->hw, skb);
	}
}

/*
                                                                             
                                                                              
                       
 */
static bool ieee80211_tx_pending_skb(struct ieee80211_local *local,
				     struct sk_buff *skb)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_sub_if_data *sdata;
	struct sta_info *sta;
	struct ieee80211_hdr *hdr;
	bool result;
	struct ieee80211_chanctx_conf *chanctx_conf;

	sdata = vif_to_sdata(info->control.vif);

	if (info->flags & IEEE80211_TX_INTFL_NEED_TXPROCESSING) {
		chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
		if (unlikely(!chanctx_conf)) {
			dev_kfree_skb(skb);
			return true;
		}
		result = ieee80211_tx(sdata, skb, true,
				      chanctx_conf->def.chan->band);
	} else {
		struct sk_buff_head skbs;

		__skb_queue_head_init(&skbs);
		__skb_queue_tail(&skbs, skb);

		hdr = (struct ieee80211_hdr *)skb->data;
		sta = sta_info_get(sdata, hdr->addr1);

		result = __ieee80211_tx(local, &skbs, skb->len, sta, true);
	}

	return result;
}

/*
                                                     
 */
void ieee80211_tx_pending(unsigned long data)
{
	struct ieee80211_local *local = (struct ieee80211_local *)data;
	unsigned long flags;
	int i;
	bool txok;

	rcu_read_lock();

	spin_lock_irqsave(&local->queue_stop_reason_lock, flags);
	for (i = 0; i < local->hw.queues; i++) {
		/*
                                                               
                                                                 
   */
		if (local->queue_stop_reasons[i] ||
		    skb_queue_empty(&local->pending[i]))
			continue;

		while (!skb_queue_empty(&local->pending[i])) {
			struct sk_buff *skb = __skb_dequeue(&local->pending[i]);
			struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);

			if (WARN_ON(!info->control.vif)) {
				ieee80211_free_txskb(&local->hw, skb);
				continue;
			}

			spin_unlock_irqrestore(&local->queue_stop_reason_lock,
						flags);

			txok = ieee80211_tx_pending_skb(local, skb);
			spin_lock_irqsave(&local->queue_stop_reason_lock,
					  flags);
			if (!txok)
				break;
		}

		if (skb_queue_empty(&local->pending[i]))
			ieee80211_propagate_queue_wake(local, i);
	}
	spin_unlock_irqrestore(&local->queue_stop_reason_lock, flags);

	rcu_read_unlock();
}

/*                                             */

static void __ieee80211_beacon_add_tim(struct ieee80211_sub_if_data *sdata,
				       struct ps_data *ps, struct sk_buff *skb)
{
	u8 *pos, *tim;
	int aid0 = 0;
	int i, have_bits = 0, n1, n2;

	/*                                                                 
          */
	if (atomic_read(&ps->num_sta_ps) > 0)
		/*                                     
                            */
		have_bits = !bitmap_empty((unsigned long*)ps->tim,
					  IEEE80211_MAX_AID+1);

	if (ps->dtim_count == 0)
		ps->dtim_count = sdata->vif.bss_conf.dtim_period - 1;
	else
		ps->dtim_count--;

	tim = pos = (u8 *) skb_put(skb, 6);
	*pos++ = WLAN_EID_TIM;
	*pos++ = 4;
	*pos++ = ps->dtim_count;
	*pos++ = sdata->vif.bss_conf.dtim_period;

	if (ps->dtim_count == 0 && !skb_queue_empty(&ps->bc_buf))
		aid0 = 1;

	ps->dtim_bc_mc = aid0 == 1;

	if (have_bits) {
		/*                                                            
                                                                
                                      */
		n1 = 0;
		for (i = 0; i < IEEE80211_MAX_TIM_LEN; i++) {
			if (ps->tim[i]) {
				n1 = i & 0xfe;
				break;
			}
		}
		n2 = n1;
		for (i = IEEE80211_MAX_TIM_LEN - 1; i >= n1; i--) {
			if (ps->tim[i]) {
				n2 = i;
				break;
			}
		}

		/*                */
		*pos++ = n1 | aid0;
		/*                  */
		skb_put(skb, n2 - n1);
		memcpy(pos, ps->tim + n1, n2 - n1 + 1);

		tim[1] = n2 - n1 + 4;
	} else {
		*pos++ = aid0; /*                */
		*pos++ = 0; /*                  */
	}
}

static int ieee80211_beacon_add_tim(struct ieee80211_sub_if_data *sdata,
				    struct ps_data *ps, struct sk_buff *skb)
{
	struct ieee80211_local *local = sdata->local;

	/*
                                                          
                                                         
                                                         
                                                         
                                                 
  */
	if (local->tim_in_locked_section) {
		__ieee80211_beacon_add_tim(sdata, ps, skb);
	} else {
		spin_lock_bh(&local->tim_lock);
		__ieee80211_beacon_add_tim(sdata, ps, skb);
		spin_unlock_bh(&local->tim_lock);
	}

	return 0;
}

struct sk_buff *ieee80211_beacon_get_tim(struct ieee80211_hw *hw,
					 struct ieee80211_vif *vif,
					 u16 *tim_offset, u16 *tim_length)
{
	struct ieee80211_local *local = hw_to_local(hw);
	struct sk_buff *skb = NULL;
	struct ieee80211_tx_info *info;
	struct ieee80211_sub_if_data *sdata = NULL;
	enum ieee80211_band band;
	struct ieee80211_tx_rate_control txrc;
	struct ieee80211_chanctx_conf *chanctx_conf;

	rcu_read_lock();

	sdata = vif_to_sdata(vif);
	chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);

	if (!ieee80211_sdata_running(sdata) || !chanctx_conf)
		goto out;

	if (tim_offset)
		*tim_offset = 0;
	if (tim_length)
		*tim_length = 0;

	if (sdata->vif.type == NL80211_IFTYPE_AP) {
		struct ieee80211_if_ap *ap = &sdata->u.ap;
		struct beacon_data *beacon = rcu_dereference(ap->beacon);

		if (beacon) {
			/*
                            
                                        
    */
			skb = dev_alloc_skb(local->tx_headroom +
					    beacon->head_len +
					    beacon->tail_len + 256);
			if (!skb)
				goto out;

			skb_reserve(skb, local->tx_headroom);
			memcpy(skb_put(skb, beacon->head_len), beacon->head,
			       beacon->head_len);

			ieee80211_beacon_add_tim(sdata, &ap->ps, skb);

			if (tim_offset)
				*tim_offset = beacon->head_len;
			if (tim_length)
				*tim_length = skb->len - beacon->head_len;

			if (beacon->tail)
				memcpy(skb_put(skb, beacon->tail_len),
				       beacon->tail, beacon->tail_len);
		} else
			goto out;
	} else if (sdata->vif.type == NL80211_IFTYPE_ADHOC) {
		struct ieee80211_if_ibss *ifibss = &sdata->u.ibss;
		struct ieee80211_hdr *hdr;
		struct beacon_data *presp = rcu_dereference(ifibss->presp);

		if (!presp)
			goto out;

		skb = dev_alloc_skb(local->tx_headroom + presp->head_len);
		if (!skb)
			goto out;
		skb_reserve(skb, local->tx_headroom);
		memcpy(skb_put(skb, presp->head_len), presp->head,
		       presp->head_len);

		hdr = (struct ieee80211_hdr *) skb->data;
		hdr->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
						 IEEE80211_STYPE_BEACON);
	} else if (ieee80211_vif_is_mesh(&sdata->vif)) {
		struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
		struct beacon_data *bcn = rcu_dereference(ifmsh->beacon);

		if (!bcn)
			goto out;

		if (ifmsh->sync_ops)
			ifmsh->sync_ops->adjust_tbtt(
						sdata);

		skb = dev_alloc_skb(local->tx_headroom +
				    bcn->head_len +
				    256 + /*        */
				    bcn->tail_len);
		if (!skb)
			goto out;
		skb_reserve(skb, local->tx_headroom);
		memcpy(skb_put(skb, bcn->head_len), bcn->head, bcn->head_len);
		ieee80211_beacon_add_tim(sdata, &ifmsh->ps, skb);
		memcpy(skb_put(skb, bcn->tail_len), bcn->tail, bcn->tail_len);
	} else {
		WARN_ON(1);
		goto out;
	}

	band = chanctx_conf->def.chan->band;

	info = IEEE80211_SKB_CB(skb);

	info->flags |= IEEE80211_TX_INTFL_DONT_ENCRYPT;
	info->flags |= IEEE80211_TX_CTL_NO_ACK;
	info->band = band;

	memset(&txrc, 0, sizeof(txrc));
	txrc.hw = hw;
	txrc.sband = local->hw.wiphy->bands[band];
	txrc.bss_conf = &sdata->vif.bss_conf;
	txrc.skb = skb;
	txrc.reported_rate.idx = -1;
	txrc.rate_idx_mask = sdata->rc_rateidx_mask[band];
	if (txrc.rate_idx_mask == (1 << txrc.sband->n_bitrates) - 1)
		txrc.max_rate_idx = -1;
	else
		txrc.max_rate_idx = fls(txrc.rate_idx_mask) - 1;
	txrc.bss = true;
	rate_control_get_rate(sdata, NULL, &txrc);

	info->control.vif = vif;

	info->flags |= IEEE80211_TX_CTL_CLEAR_PS_FILT |
			IEEE80211_TX_CTL_ASSIGN_SEQ |
			IEEE80211_TX_CTL_FIRST_FRAGMENT;
 out:
	rcu_read_unlock();
	return skb;
}
EXPORT_SYMBOL(ieee80211_beacon_get_tim);

struct sk_buff *ieee80211_proberesp_get(struct ieee80211_hw *hw,
					struct ieee80211_vif *vif)
{
	struct ieee80211_if_ap *ap = NULL;
	struct sk_buff *skb = NULL;
	struct probe_resp *presp = NULL;
	struct ieee80211_hdr *hdr;
	struct ieee80211_sub_if_data *sdata = vif_to_sdata(vif);

	if (sdata->vif.type != NL80211_IFTYPE_AP)
		return NULL;

	rcu_read_lock();

	ap = &sdata->u.ap;
	presp = rcu_dereference(ap->probe_resp);
	if (!presp)
		goto out;

	skb = dev_alloc_skb(presp->len);
	if (!skb)
		goto out;

	memcpy(skb_put(skb, presp->len), presp->data, presp->len);

	hdr = (struct ieee80211_hdr *) skb->data;
	memset(hdr->addr1, 0, sizeof(hdr->addr1));

out:
	rcu_read_unlock();
	return skb;
}
EXPORT_SYMBOL(ieee80211_proberesp_get);

struct sk_buff *ieee80211_pspoll_get(struct ieee80211_hw *hw,
				     struct ieee80211_vif *vif)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_managed *ifmgd;
	struct ieee80211_pspoll *pspoll;
	struct ieee80211_local *local;
	struct sk_buff *skb;

	if (WARN_ON(vif->type != NL80211_IFTYPE_STATION))
		return NULL;

	sdata = vif_to_sdata(vif);
	ifmgd = &sdata->u.mgd;
	local = sdata->local;

	skb = dev_alloc_skb(local->hw.extra_tx_headroom + sizeof(*pspoll));
	if (!skb)
		return NULL;

	skb_reserve(skb, local->hw.extra_tx_headroom);

	pspoll = (struct ieee80211_pspoll *) skb_put(skb, sizeof(*pspoll));
	memset(pspoll, 0, sizeof(*pspoll));
	pspoll->frame_control = cpu_to_le16(IEEE80211_FTYPE_CTL |
					    IEEE80211_STYPE_PSPOLL);
	pspoll->aid = cpu_to_le16(ifmgd->aid);

	/*                                               */
	pspoll->aid |= cpu_to_le16(1 << 15 | 1 << 14);

	memcpy(pspoll->bssid, ifmgd->bssid, ETH_ALEN);
	memcpy(pspoll->ta, vif->addr, ETH_ALEN);

	return skb;
}
EXPORT_SYMBOL(ieee80211_pspoll_get);

struct sk_buff *ieee80211_nullfunc_get(struct ieee80211_hw *hw,
				       struct ieee80211_vif *vif)
{
	struct ieee80211_hdr_3addr *nullfunc;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_managed *ifmgd;
	struct ieee80211_local *local;
	struct sk_buff *skb;

	if (WARN_ON(vif->type != NL80211_IFTYPE_STATION))
		return NULL;

	sdata = vif_to_sdata(vif);
	ifmgd = &sdata->u.mgd;
	local = sdata->local;

	skb = dev_alloc_skb(local->hw.extra_tx_headroom + sizeof(*nullfunc));
	if (!skb)
		return NULL;

	skb_reserve(skb, local->hw.extra_tx_headroom);

	nullfunc = (struct ieee80211_hdr_3addr *) skb_put(skb,
							  sizeof(*nullfunc));
	memset(nullfunc, 0, sizeof(*nullfunc));
	nullfunc->frame_control = cpu_to_le16(IEEE80211_FTYPE_DATA |
					      IEEE80211_STYPE_NULLFUNC |
					      IEEE80211_FCTL_TODS);
	memcpy(nullfunc->addr1, ifmgd->bssid, ETH_ALEN);
	memcpy(nullfunc->addr2, vif->addr, ETH_ALEN);
	memcpy(nullfunc->addr3, ifmgd->bssid, ETH_ALEN);

	return skb;
}
EXPORT_SYMBOL(ieee80211_nullfunc_get);

struct sk_buff *ieee80211_probereq_get(struct ieee80211_hw *hw,
				       struct ieee80211_vif *vif,
				       const u8 *ssid, size_t ssid_len,
				       size_t tailroom)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_local *local;
	struct ieee80211_hdr_3addr *hdr;
	struct sk_buff *skb;
	size_t ie_ssid_len;
	u8 *pos;

	sdata = vif_to_sdata(vif);
	local = sdata->local;
	ie_ssid_len = 2 + ssid_len;

	skb = dev_alloc_skb(local->hw.extra_tx_headroom + sizeof(*hdr) +
			    ie_ssid_len + tailroom);
	if (!skb)
		return NULL;

	skb_reserve(skb, local->hw.extra_tx_headroom);

	hdr = (struct ieee80211_hdr_3addr *) skb_put(skb, sizeof(*hdr));
	memset(hdr, 0, sizeof(*hdr));
	hdr->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
					 IEEE80211_STYPE_PROBE_REQ);
	eth_broadcast_addr(hdr->addr1);
	memcpy(hdr->addr2, vif->addr, ETH_ALEN);
	eth_broadcast_addr(hdr->addr3);

	pos = skb_put(skb, ie_ssid_len);
	*pos++ = WLAN_EID_SSID;
	*pos++ = ssid_len;
	if (ssid_len)
		memcpy(pos, ssid, ssid_len);
	pos += ssid_len;

	return skb;
}
EXPORT_SYMBOL(ieee80211_probereq_get);

void ieee80211_rts_get(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
		       const void *frame, size_t frame_len,
		       const struct ieee80211_tx_info *frame_txctl,
		       struct ieee80211_rts *rts)
{
	const struct ieee80211_hdr *hdr = frame;

	rts->frame_control =
	    cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_RTS);
	rts->duration = ieee80211_rts_duration(hw, vif, frame_len,
					       frame_txctl);
	memcpy(rts->ra, hdr->addr1, sizeof(rts->ra));
	memcpy(rts->ta, hdr->addr2, sizeof(rts->ta));
}
EXPORT_SYMBOL(ieee80211_rts_get);

void ieee80211_ctstoself_get(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
			     const void *frame, size_t frame_len,
			     const struct ieee80211_tx_info *frame_txctl,
			     struct ieee80211_cts *cts)
{
	const struct ieee80211_hdr *hdr = frame;

	cts->frame_control =
	    cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CTS);
	cts->duration = ieee80211_ctstoself_duration(hw, vif,
						     frame_len, frame_txctl);
	memcpy(cts->ra, hdr->addr1, sizeof(cts->ra));
}
EXPORT_SYMBOL(ieee80211_ctstoself_get);

struct sk_buff *
ieee80211_get_buffered_bc(struct ieee80211_hw *hw,
			  struct ieee80211_vif *vif)
{
	struct ieee80211_local *local = hw_to_local(hw);
	struct sk_buff *skb = NULL;
	struct ieee80211_tx_data tx;
	struct ieee80211_sub_if_data *sdata;
	struct ps_data *ps;
	struct ieee80211_tx_info *info;
	struct ieee80211_chanctx_conf *chanctx_conf;

	sdata = vif_to_sdata(vif);

	rcu_read_lock();
	chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);

	if (!chanctx_conf)
		goto out;

	if (sdata->vif.type == NL80211_IFTYPE_AP) {
		struct beacon_data *beacon =
				rcu_dereference(sdata->u.ap.beacon);

		if (!beacon || !beacon->head)
			goto out;

		ps = &sdata->u.ap.ps;
	} else if (ieee80211_vif_is_mesh(&sdata->vif)) {
		ps = &sdata->u.mesh.ps;
	} else {
		goto out;
	}

	if (ps->dtim_count != 0 || !ps->dtim_bc_mc)
		goto out; /*                                            */

	while (1) {
		skb = skb_dequeue(&ps->bc_buf);
		if (!skb)
			goto out;
		local->total_ps_buffered--;

		if (!skb_queue_empty(&ps->bc_buf) && skb->len >= 2) {
			struct ieee80211_hdr *hdr =
				(struct ieee80211_hdr *) skb->data;
			/*                                                 
                                                      
           */
			hdr->frame_control |=
				cpu_to_le16(IEEE80211_FCTL_MOREDATA);
		}

		if (sdata->vif.type == NL80211_IFTYPE_AP)
			sdata = IEEE80211_DEV_TO_SUB_IF(skb->dev);
		if (!ieee80211_tx_prepare(sdata, &tx, skb))
			break;
		dev_kfree_skb_any(skb);
	}

	info = IEEE80211_SKB_CB(skb);

	tx.flags |= IEEE80211_TX_PS_BUFFERED;
	info->band = chanctx_conf->def.chan->band;

	if (invoke_tx_handlers(&tx))
		skb = NULL;
 out:
	rcu_read_unlock();

	return skb;
}
EXPORT_SYMBOL(ieee80211_get_buffered_bc);

void __ieee80211_tx_skb_tid_band(struct ieee80211_sub_if_data *sdata,
				 struct sk_buff *skb, int tid,
				 enum ieee80211_band band)
{
	int ac = ieee802_1d_to_ac[tid & 7];

	skb_set_mac_header(skb, 0);
	skb_set_network_header(skb, 0);
	skb_set_transport_header(skb, 0);

	skb_set_queue_mapping(skb, ac);
	skb->priority = tid;

	skb->dev = sdata->dev;

	/*
                                                              
                                                            
                                                             
  */
	local_bh_disable();
	ieee80211_xmit(sdata, skb, band);
	local_bh_enable();
}
