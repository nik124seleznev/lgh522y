/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005-2006, Devicescape Software, Inc.
 * Copyright 2006-2007	Jiri Benc <jbenc@suse.cz>
 * Copyright 2007-2010	Johannes Berg <johannes@sipsolutions.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/rcupdate.h>
#include <linux/export.h>
#include <net/mac80211.h>
#include <net/ieee80211_radiotap.h>
#include <asm/unaligned.h>

#include "ieee80211_i.h"
#include "driver-ops.h"
#include "led.h"
#include "mesh.h"
#include "wep.h"
#include "wpa.h"
#include "tkip.h"
#include "wme.h"
#include "rate.h"

/*
                         
  
                                                                 
                              
 */
static struct sk_buff *remove_monitor_info(struct ieee80211_local *local,
					   struct sk_buff *skb)
{
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);

	if (local->hw.flags & IEEE80211_HW_RX_INCLUDES_FCS) {
		if (likely(skb->len > FCS_LEN))
			__pskb_trim(skb, skb->len - FCS_LEN);
		else {
			/*            */
			WARN_ON(1);
			dev_kfree_skb(skb);
			return NULL;
		}
	}

	if (status->vendor_radiotap_len)
		__pskb_pull(skb, status->vendor_radiotap_len);

	return skb;
}

static inline int should_drop_frame(struct sk_buff *skb, int present_fcs_len)
{
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *hdr;

	hdr = (void *)(skb->data + status->vendor_radiotap_len);

	if (status->flag & (RX_FLAG_FAILED_FCS_CRC |
			    RX_FLAG_FAILED_PLCP_CRC |
			    RX_FLAG_AMPDU_IS_ZEROLEN))
		return 1;
	if (unlikely(skb->len < 16 + present_fcs_len +
				status->vendor_radiotap_len))
		return 1;
	if (ieee80211_is_ctl(hdr->frame_control) &&
	    !ieee80211_is_pspoll(hdr->frame_control) &&
	    !ieee80211_is_back_req(hdr->frame_control))
		return 1;
	return 0;
}

static int
ieee80211_rx_radiotap_space(struct ieee80211_local *local,
			    struct ieee80211_rx_status *status)
{
	int len;

	/*                       */
	len = sizeof(struct ieee80211_radiotap_header) + 9;

	/*                       */
	if (status->vendor_radiotap_len)
		len += 4;

	if (ieee80211_have_rx_timestamp(status)) {
		len = ALIGN(len, 8);
		len += 8;
	}
	if (local->hw.flags & IEEE80211_HW_SIGNAL_DBM)
		len += 1;

	/*                                   */
	len = ALIGN(len, 2);

	if (status->flag & RX_FLAG_HT) /*         */
		len += 3;

	if (status->flag & RX_FLAG_AMPDU_DETAILS) {
		len = ALIGN(len, 4);
		len += 8;
	}

	if (status->flag & RX_FLAG_VHT) {
		len = ALIGN(len, 2);
		len += 12;
	}

	if (status->vendor_radiotap_len) {
		if (WARN_ON_ONCE(status->vendor_radiotap_align == 0))
			status->vendor_radiotap_align = 1;
		/*                                         */
		len = ALIGN(len, 2);
		/*                                            */
		len += 6;
		/*                           */
		len = ALIGN(len, status->vendor_radiotap_align);
		/*                                       */
	}

	return len;
}

/*
                                                         
  
                                                                               
 */
static void
ieee80211_add_rx_radiotap_header(struct ieee80211_local *local,
				 struct sk_buff *skb,
				 struct ieee80211_rate *rate,
				 int rtap_len, bool has_fcs)
{
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_radiotap_header *rthdr;
	unsigned char *pos;
	u16 rx_flags = 0;
	int mpdulen;

	mpdulen = skb->len;
	if (!(has_fcs && (local->hw.flags & IEEE80211_HW_RX_INCLUDES_FCS)))
		mpdulen += FCS_LEN;

	rthdr = (struct ieee80211_radiotap_header *)skb_push(skb, rtap_len);
	memset(rthdr, 0, rtap_len);

	/*                                           */
	rthdr->it_present =
		cpu_to_le32((1 << IEEE80211_RADIOTAP_FLAGS) |
			    (1 << IEEE80211_RADIOTAP_CHANNEL) |
			    (1 << IEEE80211_RADIOTAP_ANTENNA) |
			    (1 << IEEE80211_RADIOTAP_RX_FLAGS));
	rthdr->it_len = cpu_to_le16(rtap_len + status->vendor_radiotap_len);

	pos = (unsigned char *)(rthdr + 1);

	if (status->vendor_radiotap_len) {
		rthdr->it_present |=
			cpu_to_le32(BIT(IEEE80211_RADIOTAP_VENDOR_NAMESPACE)) |
			cpu_to_le32(BIT(IEEE80211_RADIOTAP_EXT));
		put_unaligned_le32(status->vendor_radiotap_bitmap, pos);
		pos += 4;
	}

	/*                                                */

	/*                         */
	if (ieee80211_have_rx_timestamp(status)) {
		/*         */
		while ((pos - (u8 *)rthdr) & 7)
			*pos++ = 0;
		put_unaligned_le64(
			ieee80211_calculate_rx_timestamp(local, status,
							 mpdulen, 0),
			pos);
		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_TSFT);
		pos += 8;
	}

	/*                          */
	if (has_fcs && (local->hw.flags & IEEE80211_HW_RX_INCLUDES_FCS))
		*pos |= IEEE80211_RADIOTAP_F_FCS;
	if (status->flag & (RX_FLAG_FAILED_FCS_CRC | RX_FLAG_FAILED_PLCP_CRC))
		*pos |= IEEE80211_RADIOTAP_F_BADFCS;
	if (status->flag & RX_FLAG_SHORTPRE)
		*pos |= IEEE80211_RADIOTAP_F_SHORTPRE;
	pos++;

	/*                         */
	if (!rate || status->flag & (RX_FLAG_HT | RX_FLAG_VHT)) {
		/*
                                                       
                                                     
                                                    
                                                   
   */
		*pos = 0;
	} else {
		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_RATE);
		*pos = rate->bitrate / 5;
	}
	pos++;

	/*                            */
	put_unaligned_le16(status->freq, pos);
	pos += 2;
	if (status->band == IEEE80211_BAND_5GHZ)
		put_unaligned_le16(IEEE80211_CHAN_OFDM | IEEE80211_CHAN_5GHZ,
				   pos);
	else if (status->flag & (RX_FLAG_HT | RX_FLAG_VHT))
		put_unaligned_le16(IEEE80211_CHAN_DYN | IEEE80211_CHAN_2GHZ,
				   pos);
	else if (rate && rate->flags & IEEE80211_RATE_ERP_G)
		put_unaligned_le16(IEEE80211_CHAN_OFDM | IEEE80211_CHAN_2GHZ,
				   pos);
	else if (rate)
		put_unaligned_le16(IEEE80211_CHAN_CCK | IEEE80211_CHAN_2GHZ,
				   pos);
	else
		put_unaligned_le16(IEEE80211_CHAN_2GHZ, pos);
	pos += 2;

	/*                                  */
	if (local->hw.flags & IEEE80211_HW_SIGNAL_DBM &&
	    !(status->flag & RX_FLAG_NO_SIGNAL_VAL)) {
		*pos = status->signal;
		rthdr->it_present |=
			cpu_to_le32(1 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL);
		pos++;
	}

	/*                                            */

	/*                            */
	*pos = status->antenna;
	pos++;

	/*                                            */

	/*                             */
	/*                                                          */
	if ((pos - (u8 *)rthdr) & 1)
		*pos++ = 0;
	if (status->flag & RX_FLAG_FAILED_PLCP_CRC)
		rx_flags |= IEEE80211_RADIOTAP_F_RX_BADPLCP;
	put_unaligned_le16(rx_flags, pos);
	pos += 2;

	if (status->flag & RX_FLAG_HT) {
		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_MCS);
		*pos++ = local->hw.radiotap_mcs_details;
		*pos = 0;
		if (status->flag & RX_FLAG_SHORT_GI)
			*pos |= IEEE80211_RADIOTAP_MCS_SGI;
		if (status->flag & RX_FLAG_40MHZ)
			*pos |= IEEE80211_RADIOTAP_MCS_BW_40;
		if (status->flag & RX_FLAG_HT_GF)
			*pos |= IEEE80211_RADIOTAP_MCS_FMT_GF;
		pos++;
		*pos++ = status->rate_idx;
	}

	if (status->flag & RX_FLAG_AMPDU_DETAILS) {
		u16 flags = 0;

		/*                         */
		while ((pos - (u8 *)rthdr) & 3)
			pos++;
		rthdr->it_present |=
			cpu_to_le32(1 << IEEE80211_RADIOTAP_AMPDU_STATUS);
		put_unaligned_le32(status->ampdu_reference, pos);
		pos += 4;
		if (status->flag & RX_FLAG_AMPDU_REPORT_ZEROLEN)
			flags |= IEEE80211_RADIOTAP_AMPDU_REPORT_ZEROLEN;
		if (status->flag & RX_FLAG_AMPDU_IS_ZEROLEN)
			flags |= IEEE80211_RADIOTAP_AMPDU_IS_ZEROLEN;
		if (status->flag & RX_FLAG_AMPDU_LAST_KNOWN)
			flags |= IEEE80211_RADIOTAP_AMPDU_LAST_KNOWN;
		if (status->flag & RX_FLAG_AMPDU_IS_LAST)
			flags |= IEEE80211_RADIOTAP_AMPDU_IS_LAST;
		if (status->flag & RX_FLAG_AMPDU_DELIM_CRC_ERROR)
			flags |= IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_ERR;
		if (status->flag & RX_FLAG_AMPDU_DELIM_CRC_KNOWN)
			flags |= IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_KNOWN;
		put_unaligned_le16(flags, pos);
		pos += 2;
		if (status->flag & RX_FLAG_AMPDU_DELIM_CRC_KNOWN)
			*pos++ = status->ampdu_delimiter_crc;
		else
			*pos++ = 0;
		*pos++ = 0;
	}

	if (status->flag & RX_FLAG_VHT) {
		u16 known = local->hw.radiotap_vht_details;

		rthdr->it_present |= cpu_to_le32(1 << IEEE80211_RADIOTAP_VHT);
		/*                                    */
		if (status->flag & RX_FLAG_80P80MHZ)
			known &= ~IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH;
		put_unaligned_le16(known, pos);
		pos += 2;
		/*       */
		if (status->flag & RX_FLAG_SHORT_GI)
			*pos |= IEEE80211_RADIOTAP_VHT_FLAG_SGI;
		pos++;
		/*           */
		if (status->flag & RX_FLAG_80MHZ)
			*pos++ = 4;
		else if (status->flag & RX_FLAG_80P80MHZ)
			*pos++ = 0; /*                        */
		else if (status->flag & RX_FLAG_160MHZ)
			*pos++ = 11;
		else if (status->flag & RX_FLAG_40MHZ)
			*pos++ = 1;
		else /*        */
			*pos++ = 0;
		/*         */
		*pos = (status->rate_idx << 4) | status->vht_nss;
		pos += 4;
		/*              */
		pos++;
		/*          */
		pos++;
		/*             */
		pos += 2;
	}

	if (status->vendor_radiotap_len) {
		/*                                                          */
		if ((pos - (u8 *)rthdr) & 1)
			*pos++ = 0;
		*pos++ = status->vendor_radiotap_oui[0];
		*pos++ = status->vendor_radiotap_oui[1];
		*pos++ = status->vendor_radiotap_oui[2];
		*pos++ = status->vendor_radiotap_subns;
		put_unaligned_le16(status->vendor_radiotap_len, pos);
		pos += 2;
		/*                                       */
		while ((pos - (u8 *)rthdr) & (status->vendor_radiotap_align - 1))
			*pos++ = 0;
	}
}

/*
                                                                      
                                                                   
                                               
 */
static struct sk_buff *
ieee80211_rx_monitor(struct ieee80211_local *local, struct sk_buff *origskb,
		     struct ieee80211_rate *rate)
{
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(origskb);
	struct ieee80211_sub_if_data *sdata;
	int needed_headroom;
	struct sk_buff *skb, *skb2;
	struct net_device *prev_dev = NULL;
	int present_fcs_len = 0;

	/*
                                                        
                                                                
                                                          
   
                                                             
                                                   
  */

	if (local->hw.flags & IEEE80211_HW_RX_INCLUDES_FCS)
		present_fcs_len = FCS_LEN;

	/*                                                                    */
	if (!pskb_may_pull(origskb, 2 + status->vendor_radiotap_len)) {
		dev_kfree_skb(origskb);
		return NULL;
	}

	if (!local->monitors) {
		if (should_drop_frame(origskb, present_fcs_len)) {
			dev_kfree_skb(origskb);
			return NULL;
		}

		return remove_monitor_info(local, origskb);
	}

	/*                                                       */
	needed_headroom = ieee80211_rx_radiotap_space(local, status);

	if (should_drop_frame(origskb, present_fcs_len)) {
		/*                                           */
		skb = origskb;
		origskb = NULL;

		/*
                                                              
                                                            
                                                          
                                                              
                                                 
   */
		if (skb_headroom(skb) < needed_headroom &&
		    pskb_expand_head(skb, needed_headroom, 0, GFP_ATOMIC)) {
			dev_kfree_skb(skb);
			return NULL;
		}
	} else {
		/*
                                                            
                               
   */
		skb = skb_copy_expand(origskb, needed_headroom, 0, GFP_ATOMIC);

		origskb = remove_monitor_info(local, origskb);

		if (!skb)
			return origskb;
	}

	/*                              */
	ieee80211_add_rx_radiotap_header(local, skb, rate, needed_headroom,
					 true);

	skb_reset_mac_header(skb);
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	skb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = htons(ETH_P_802_2);

	list_for_each_entry_rcu(sdata, &local->interfaces, list) {
		if (sdata->vif.type != NL80211_IFTYPE_MONITOR)
			continue;

		if (sdata->u.mntr_flags & MONITOR_FLAG_COOK_FRAMES)
			continue;

		if (!ieee80211_sdata_running(sdata))
			continue;

		if (prev_dev) {
			skb2 = skb_clone(skb, GFP_ATOMIC);
			if (skb2) {
				skb2->dev = prev_dev;
				netif_receive_skb(skb2);
			}
		}

		prev_dev = sdata->dev;
		sdata->dev->stats.rx_packets++;
		sdata->dev->stats.rx_bytes += skb->len;
	}

	if (prev_dev) {
		skb->dev = prev_dev;
		netif_receive_skb(skb);
	} else
		dev_kfree_skb(skb);

	return origskb;
}

static void ieee80211_parse_qos(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);
	int tid, seqno_idx, security_idx;

	/*                                          */
	if (ieee80211_is_data_qos(hdr->frame_control)) {
		u8 *qc = ieee80211_get_qos_ctl(hdr);
		/*                       */
		tid = *qc & IEEE80211_QOS_CTL_TID_MASK;
		if (*qc & IEEE80211_QOS_CTL_A_MSDU_PRESENT)
			status->rx_flags |= IEEE80211_RX_AMSDU;

		seqno_idx = tid;
		security_idx = tid;
	} else {
		/*
                                                           
    
                                                     
                                                     
                                                      
                                                        
                               
    
                                               
   */
		seqno_idx = IEEE80211_NUM_TIDS;
		security_idx = 0;
		if (ieee80211_is_mgmt(hdr->frame_control))
			security_idx = IEEE80211_NUM_TIDS;
		tid = 0;
	}

	rx->seqno_idx = seqno_idx;
	rx->security_idx = security_idx;
	/*                                                                    
                                                     */
	rx->skb->priority = (tid > 7) ? 0 : tid;
}

/* 
                        
  
                                                                              
                
  
                                                                          
                                                                    
                                                                          
                                                                            
                                                                            
                                                                        
                                                                        
                       
  
                                                                              
                                                                             
                                                                               
                                                                         
                                                   
  
                                                                            
                                                                          
                                                              
 */
static void ieee80211_verify_alignment(struct ieee80211_rx_data *rx)
{
#ifdef CONFIG_MAC80211_VERBOSE_DEBUG
	WARN_ONCE((unsigned long)rx->skb->data & 1,
		  "unaligned packet at 0x%p\n", rx->skb->data);
#endif
}


/*             */

static int ieee80211_is_unicast_robust_mgmt_frame(struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;

	if (skb->len < 24 || is_multicast_ether_addr(hdr->addr1))
		return 0;

	return ieee80211_is_robust_mgmt_frame(hdr);
}


static int ieee80211_is_multicast_robust_mgmt_frame(struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;

	if (skb->len < 24 || !is_multicast_ether_addr(hdr->addr1))
		return 0;

	return ieee80211_is_robust_mgmt_frame(hdr);
}


/*                                                                       */
static int ieee80211_get_mmie_keyidx(struct sk_buff *skb)
{
	struct ieee80211_mgmt *hdr = (struct ieee80211_mgmt *) skb->data;
	struct ieee80211_mmie *mmie;

	if (skb->len < 24 + sizeof(*mmie) || !is_multicast_ether_addr(hdr->da))
		return -1;

	if (!ieee80211_is_robust_mgmt_frame((struct ieee80211_hdr *) hdr))
		return -1; /*                               */

	mmie = (struct ieee80211_mmie *)
		(skb->data + skb->len - sizeof(*mmie));
	if (mmie->element_id != WLAN_EID_MMIE ||
	    mmie->length != sizeof(*mmie) - 2)
		return -1;

	return le16_to_cpu(mmie->key_id);
}

static ieee80211_rx_result ieee80211_rx_mesh_check(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;
	char *dev_addr = rx->sdata->vif.addr;

	if (ieee80211_is_data(hdr->frame_control)) {
		if (is_multicast_ether_addr(hdr->addr1)) {
			if (ieee80211_has_tods(hdr->frame_control) ||
			    !ieee80211_has_fromds(hdr->frame_control))
				return RX_DROP_MONITOR;
			if (ether_addr_equal(hdr->addr3, dev_addr))
				return RX_DROP_MONITOR;
		} else {
			if (!ieee80211_has_a4(hdr->frame_control))
				return RX_DROP_MONITOR;
			if (ether_addr_equal(hdr->addr4, dev_addr))
				return RX_DROP_MONITOR;
		}
	}

	/*                                                                     
                                                        
  */

	if (!rx->sta || sta_plink_state(rx->sta) != NL80211_PLINK_ESTAB) {
		struct ieee80211_mgmt *mgmt;

		if (!ieee80211_is_mgmt(hdr->frame_control))
			return RX_DROP_MONITOR;

		if (ieee80211_is_action(hdr->frame_control)) {
			u8 category;

			/*                                     */
			if (rx->skb->len < IEEE80211_MIN_ACTION_SIZE)
				return RX_DROP_MONITOR;

			mgmt = (struct ieee80211_mgmt *)hdr;
			category = mgmt->u.action.category;
			if (category != WLAN_CATEGORY_MESH_ACTION &&
			    category != WLAN_CATEGORY_SELF_PROTECTED)
				return RX_DROP_MONITOR;
			return RX_CONTINUE;
		}

		if (ieee80211_is_probe_req(hdr->frame_control) ||
		    ieee80211_is_probe_resp(hdr->frame_control) ||
		    ieee80211_is_beacon(hdr->frame_control) ||
		    ieee80211_is_auth(hdr->frame_control))
			return RX_CONTINUE;

		return RX_DROP_MONITOR;
	}

	return RX_CONTINUE;
}

static void ieee80211_release_reorder_frame(struct ieee80211_sub_if_data *sdata,
					    struct tid_ampdu_rx *tid_agg_rx,
					    int index,
					    struct sk_buff_head *frames)
{
	struct sk_buff *skb = tid_agg_rx->reorder_buf[index];
	struct ieee80211_rx_status *status;

	lockdep_assert_held(&tid_agg_rx->reorder_lock);

	if (!skb)
		goto no_frame;

	/*                                                */
	tid_agg_rx->stored_mpdu_num--;
	tid_agg_rx->reorder_buf[index] = NULL;
	status = IEEE80211_SKB_RXCB(skb);
	status->rx_flags |= IEEE80211_RX_DEFERRED_RELEASE;
	__skb_queue_tail(frames, skb);

no_frame:
	tid_agg_rx->head_seq_num = ieee80211_sn_inc(tid_agg_rx->head_seq_num);
}

static void ieee80211_release_reorder_frames(struct ieee80211_sub_if_data *sdata,
					     struct tid_ampdu_rx *tid_agg_rx,
					     u16 head_seq_num,
					     struct sk_buff_head *frames)
{
	int index;

	lockdep_assert_held(&tid_agg_rx->reorder_lock);

	while (ieee80211_sn_less(tid_agg_rx->head_seq_num, head_seq_num)) {
		index = ieee80211_sn_sub(tid_agg_rx->head_seq_num,
					 tid_agg_rx->ssn) %
							tid_agg_rx->buf_size;
		ieee80211_release_reorder_frame(sdata, tid_agg_rx, index,
						frames);
	}
}

/*
                                                                               
                                                                         
                                                                            
                                                                             
                                                                     
  
                                              
 */
#define HT_RX_REORDER_BUF_TIMEOUT (HZ / 10)

static void ieee80211_sta_reorder_release(struct ieee80211_sub_if_data *sdata,
					  struct tid_ampdu_rx *tid_agg_rx,
					  struct sk_buff_head *frames)
{
	int index, j;

	lockdep_assert_held(&tid_agg_rx->reorder_lock);

	/*                                             */
	index = ieee80211_sn_sub(tid_agg_rx->head_seq_num,
				 tid_agg_rx->ssn) % tid_agg_rx->buf_size;
	if (!tid_agg_rx->reorder_buf[index] &&
	    tid_agg_rx->stored_mpdu_num) {
		/*
                                                           
                                                 
   */
		int skipped = 1;
		for (j = (index + 1) % tid_agg_rx->buf_size; j != index;
		     j = (j + 1) % tid_agg_rx->buf_size) {
			if (!tid_agg_rx->reorder_buf[j]) {
				skipped++;
				continue;
			}
			if (skipped &&
			    !time_after(jiffies, tid_agg_rx->reorder_time[j] +
					HT_RX_REORDER_BUF_TIMEOUT))
				goto set_release_timer;

			ht_dbg_ratelimited(sdata,
					   "release an RX reorder frame due to timeout on earlier frames\n");
			ieee80211_release_reorder_frame(sdata, tid_agg_rx, j,
							frames);

			/*
                                                         
    */
			tid_agg_rx->head_seq_num =
				(tid_agg_rx->head_seq_num +
				 skipped) & IEEE80211_SN_MASK;
			skipped = 0;
		}
	} else while (tid_agg_rx->reorder_buf[index]) {
		ieee80211_release_reorder_frame(sdata, tid_agg_rx, index,
						frames);
		index =	ieee80211_sn_sub(tid_agg_rx->head_seq_num,
					 tid_agg_rx->ssn) %
							tid_agg_rx->buf_size;
	}

	if (tid_agg_rx->stored_mpdu_num) {
		j = index = ieee80211_sn_sub(tid_agg_rx->head_seq_num,
					     tid_agg_rx->ssn) %
							tid_agg_rx->buf_size;

		for (; j != (index - 1) % tid_agg_rx->buf_size;
		     j = (j + 1) % tid_agg_rx->buf_size) {
			if (tid_agg_rx->reorder_buf[j])
				break;
		}

 set_release_timer:

		mod_timer(&tid_agg_rx->reorder_timer,
			  tid_agg_rx->reorder_time[j] + 1 +
			  HT_RX_REORDER_BUF_TIMEOUT);
	} else {
		del_timer(&tid_agg_rx->reorder_timer);
	}
}

/*
                                                           
                                                          
                                                         
 */
static bool ieee80211_sta_manage_reorder_buf(struct ieee80211_sub_if_data *sdata,
					     struct tid_ampdu_rx *tid_agg_rx,
					     struct sk_buff *skb,
					     struct sk_buff_head *frames)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	u16 sc = le16_to_cpu(hdr->seq_ctrl);
	u16 mpdu_seq_num = (sc & IEEE80211_SCTL_SEQ) >> 4;
	u16 head_seq_num, buf_size;
	int index;
	bool ret = true;

	spin_lock(&tid_agg_rx->reorder_lock);

	buf_size = tid_agg_rx->buf_size;
	head_seq_num = tid_agg_rx->head_seq_num;

	/*                                        */
	if (ieee80211_sn_less(mpdu_seq_num, head_seq_num)) {
		dev_kfree_skb(skb);
		goto out;
	}

	/*
                                                             
                                                                
  */
	if (!ieee80211_sn_less(mpdu_seq_num, head_seq_num + buf_size)) {
		head_seq_num = ieee80211_sn_inc(
				ieee80211_sn_sub(mpdu_seq_num, buf_size));
		/*                                               */
		ieee80211_release_reorder_frames(sdata, tid_agg_rx,
						 head_seq_num, frames);
	}

	/*                                                                   */

	index = ieee80211_sn_sub(mpdu_seq_num,
				 tid_agg_rx->ssn) % tid_agg_rx->buf_size;

	/*                                       */
	if (tid_agg_rx->reorder_buf[index]) {
		dev_kfree_skb(skb);
		goto out;
	}

	/*
                                                              
                                                               
                                                               
                                     
  */
	if (mpdu_seq_num == tid_agg_rx->head_seq_num &&
	    tid_agg_rx->stored_mpdu_num == 0) {
		tid_agg_rx->head_seq_num =
			ieee80211_sn_inc(tid_agg_rx->head_seq_num);
		ret = false;
		goto out;
	}

	/*                                        */
	tid_agg_rx->reorder_buf[index] = skb;
	tid_agg_rx->reorder_time[index] = jiffies;
	tid_agg_rx->stored_mpdu_num++;
	ieee80211_sta_reorder_release(sdata, tid_agg_rx, frames);

 out:
	spin_unlock(&tid_agg_rx->reorder_lock);
	return ret;
}

/*
                                                                
                                                                  
 */
static void ieee80211_rx_reorder_ampdu(struct ieee80211_rx_data *rx,
				       struct sk_buff_head *frames)
{
	struct sk_buff *skb = rx->skb;
	struct ieee80211_local *local = rx->local;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct sta_info *sta = rx->sta;
	struct tid_ampdu_rx *tid_agg_rx;
	u16 sc;
	u8 tid, ack_policy;

	if (!ieee80211_is_data_qos(hdr->frame_control) ||
	    is_multicast_ether_addr(hdr->addr1))
		goto dont_reorder;

	/*
                                              
                                                       
  */

	if (!sta)
		goto dont_reorder;

	ack_policy = *ieee80211_get_qos_ctl(hdr) &
		     IEEE80211_QOS_CTL_ACK_POLICY_MASK;
	tid = *ieee80211_get_qos_ctl(hdr) & IEEE80211_QOS_CTL_TID_MASK;

	tid_agg_rx = rcu_dereference(sta->ampdu_mlme.tid_rx[tid]);
	if (!tid_agg_rx)
		goto dont_reorder;

	/*                                   */
	if (unlikely(hdr->frame_control & cpu_to_le16(IEEE80211_STYPE_NULLFUNC)))
		goto dont_reorder;

	/*                          */
	if (ack_policy != IEEE80211_QOS_CTL_ACK_POLICY_BLOCKACK &&
	    ack_policy != IEEE80211_QOS_CTL_ACK_POLICY_NORMAL)
		goto dont_reorder;

	/*                                      */
	if (!(status->rx_flags & IEEE80211_RX_RA_MATCH))
		goto dont_reorder;

	/*                                                       */

	/*                     */
	if (tid_agg_rx->timeout)
		tid_agg_rx->last_rx = jiffies;

	/*                                                               */
	sc = le16_to_cpu(hdr->seq_ctrl);
	if (sc & IEEE80211_SCTL_FRAG) {
		skb->pkt_type = IEEE80211_SDATA_QUEUE_TYPE_FRAME;
		skb_queue_tail(&rx->sdata->skb_queue, skb);
		ieee80211_queue_work(&local->hw, &rx->sdata->work);
		return;
	}

	/*
                                                      
                                                     
                                                       
                                                       
                     
  */
	if (ieee80211_sta_manage_reorder_buf(rx->sdata, tid_agg_rx, skb,
					     frames))
		return;

 dont_reorder:
	__skb_queue_tail(frames, skb);
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_check(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);

	/*
                                         
                                                                   
  */
	if (rx->skb->len >= 24 && rx->sta &&
	    !ieee80211_is_ctl(hdr->frame_control) &&
	    !ieee80211_is_qos_nullfunc(hdr->frame_control) &&
	    !is_multicast_ether_addr(hdr->addr1)) {
		if (unlikely(ieee80211_has_retry(hdr->frame_control) &&
			     rx->sta->last_seq_ctrl[rx->seqno_idx] ==
			     hdr->seq_ctrl)) {
			if (status->rx_flags & IEEE80211_RX_RA_MATCH) {
				rx->local->dot11FrameDuplicateCount++;
				rx->sta->num_duplicates++;
			}
			return RX_DROP_UNUSABLE;
		} else
			rx->sta->last_seq_ctrl[rx->seqno_idx] = hdr->seq_ctrl;
	}

	if (unlikely(rx->skb->len < 16)) {
		I802_DEBUG_INC(rx->local->rx_handlers_drop_short);
		return RX_DROP_MONITOR;
	}

	/*                                                             
                          
   
                                                                   
                                                              
                                                               
                                                            
  */

	if (ieee80211_vif_is_mesh(&rx->sdata->vif))
		return ieee80211_rx_mesh_check(rx);

	if (unlikely((ieee80211_is_data(hdr->frame_control) ||
		      ieee80211_is_pspoll(hdr->frame_control)) &&
		     rx->sdata->vif.type != NL80211_IFTYPE_ADHOC &&
		     rx->sdata->vif.type != NL80211_IFTYPE_WDS &&
		     (!rx->sta || !test_sta_flag(rx->sta, WLAN_STA_ASSOC)))) {
		/*
                                                              
                                                              
                                                             
   */
		if (rx->sta && rx->sdata->vif.type == NL80211_IFTYPE_STATION &&
		    ieee80211_is_data_present(hdr->frame_control)) {
			unsigned int hdrlen;
			__be16 ethertype;

			hdrlen = ieee80211_hdrlen(hdr->frame_control);

			if (rx->skb->len < hdrlen + 8)
				return RX_DROP_MONITOR;

			skb_copy_bits(rx->skb, hdrlen + 6, &ethertype, 2);
			if (ethertype == rx->sdata->control_port_protocol)
				return RX_CONTINUE;
		}

		if (rx->sdata->vif.type == NL80211_IFTYPE_AP &&
		    cfg80211_rx_spurious_frame(rx->sdata->dev,
					       hdr->addr2,
					       GFP_ATOMIC))
			return RX_DROP_UNUSABLE;

		return RX_DROP_MONITOR;
	}

	return RX_CONTINUE;
}


static ieee80211_rx_result debug_noinline
ieee80211_rx_h_decrypt(struct ieee80211_rx_data *rx)
{
	struct sk_buff *skb = rx->skb;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	int keyidx;
	int hdrlen;
	ieee80211_rx_result result = RX_DROP_UNUSABLE;
	struct ieee80211_key *sta_ptk = NULL;
	int mmie_keyidx = -1;
	__le16 fc;

	/*
                     
   
                                 
                       
                                              
                          
                                             
   
                                                                  
                                                                 
                                                                 
                                                                  
                                                                   
                                                              
   
                                                                
                                                                  
                                             
   
                                                                    
                                                                   
                                                                  
                                                                 
             
  */

	/*
                                                                    
                                          
  */
	if (!(status->rx_flags & IEEE80211_RX_RA_MATCH))
		return RX_CONTINUE;

	/*                     */
	rx->key = NULL;

	if (rx->sta)
		sta_ptk = rcu_dereference(rx->sta->ptk);

	fc = hdr->frame_control;

	if (!ieee80211_has_protected(fc))
		mmie_keyidx = ieee80211_get_mmie_keyidx(rx->skb);

	if (!is_multicast_ether_addr(hdr->addr1) && sta_ptk) {
		rx->key = sta_ptk;
		if ((status->flag & RX_FLAG_DECRYPTED) &&
		    (status->flag & RX_FLAG_IV_STRIPPED))
			return RX_CONTINUE;
		/*                                                */
		if (!ieee80211_has_protected(fc))
			return RX_CONTINUE;
	} else if (mmie_keyidx >= 0) {
		/*                                                   */
		if ((status->flag & RX_FLAG_DECRYPTED) &&
		    (status->flag & RX_FLAG_IV_STRIPPED))
			return RX_CONTINUE;

		if (mmie_keyidx < NUM_DEFAULT_KEYS ||
		    mmie_keyidx >= NUM_DEFAULT_KEYS + NUM_DEFAULT_MGMT_KEYS)
			return RX_DROP_MONITOR; /*                       */
		if (rx->sta)
			rx->key = rcu_dereference(rx->sta->gtk[mmie_keyidx]);
		if (!rx->key)
			rx->key = rcu_dereference(rx->sdata->keys[mmie_keyidx]);
	} else if (!ieee80211_has_protected(fc)) {
		/*
                                                                 
                                                               
                                                              
                        
   */
		struct ieee80211_key *key = NULL;
		struct ieee80211_sub_if_data *sdata = rx->sdata;
		int i;

		if (ieee80211_is_mgmt(fc) &&
		    is_multicast_ether_addr(hdr->addr1) &&
		    (key = rcu_dereference(rx->sdata->default_mgmt_key)))
			rx->key = key;
		else {
			if (rx->sta) {
				for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
					key = rcu_dereference(rx->sta->gtk[i]);
					if (key)
						break;
				}
			}
			if (!key) {
				for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
					key = rcu_dereference(sdata->keys[i]);
					if (key)
						break;
				}
			}
			if (key)
				rx->key = key;
		}
		return RX_CONTINUE;
	} else {
		u8 keyid;
		/*
                                                     
                                                  
                                                   
                                         
                                                   
                                                     
                                           
   */
		if ((status->flag & RX_FLAG_DECRYPTED) &&
		    (status->flag & RX_FLAG_IV_STRIPPED))
			return RX_CONTINUE;

		hdrlen = ieee80211_hdrlen(fc);

		if (rx->skb->len < 8 + hdrlen)
			return RX_DROP_UNUSABLE; /*                   */

		/*
                                              
                                                     
   */
		skb_copy_bits(rx->skb, hdrlen + 3, &keyid, 1);
		keyidx = keyid >> 6;

		/*                                                  */
		if (is_multicast_ether_addr(hdr->addr1) && rx->sta)
			rx->key = rcu_dereference(rx->sta->gtk[keyidx]);

		/*                               */
		if (!rx->key) {
			rx->key = rcu_dereference(rx->sdata->keys[keyidx]);

			/*
                                                    
                                                    
                                                     
    */
			if (rx->key &&
			    rx->key->conf.cipher != WLAN_CIPHER_SUITE_WEP40 &&
			    rx->key->conf.cipher != WLAN_CIPHER_SUITE_WEP104 &&
			    !is_multicast_ether_addr(hdr->addr1))
				rx->key = NULL;
		}
	}

	if (rx->key) {
		if (unlikely(rx->key->flags & KEY_FLAG_TAINTED))
			return RX_DROP_MONITOR;

		rx->key->tx_rx_count++;
		/*                                 */
	} else {
		return RX_DROP_MONITOR;
	}

	switch (rx->key->conf.cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		result = ieee80211_crypto_wep_decrypt(rx);
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		result = ieee80211_crypto_tkip_decrypt(rx);
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		result = ieee80211_crypto_ccmp_decrypt(rx);
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
		result = ieee80211_crypto_aes_cmac_decrypt(rx);
		break;
	default:
		/*
                                                   
                                          
   */
		return RX_DROP_UNUSABLE;
	}

	/*                                                        */

	/*                                                        */
	status->flag |= RX_FLAG_DECRYPTED;

	return result;
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_check_more_data(struct ieee80211_rx_data *rx)
{
	struct ieee80211_local *local;
	struct ieee80211_hdr *hdr;
	struct sk_buff *skb;

	local = rx->local;
	skb = rx->skb;
	hdr = (struct ieee80211_hdr *) skb->data;

	if (!local->pspolling)
		return RX_CONTINUE;

	if (!ieee80211_has_fromds(hdr->frame_control))
		/*                     */
		return RX_CONTINUE;

	if (!ieee80211_is_data(hdr->frame_control))
		return RX_CONTINUE;

	if (!ieee80211_has_moredata(hdr->frame_control)) {
		/*                                       */
		local->pspolling = false;
		return RX_CONTINUE;
	}

	/*                                                             */
	ieee80211_send_pspoll(local, rx->sdata);

	return RX_CONTINUE;
}

static void sta_ps_start(struct sta_info *sta)
{
	struct ieee80211_sub_if_data *sdata = sta->sdata;
	struct ieee80211_local *local = sdata->local;
	struct ps_data *ps;

	if (sta->sdata->vif.type == NL80211_IFTYPE_AP ||
	    sta->sdata->vif.type == NL80211_IFTYPE_AP_VLAN)
		ps = &sdata->bss->ps;
	else
		return;

	atomic_inc(&ps->num_sta_ps);
	set_sta_flag(sta, WLAN_STA_PS_STA);
	if (!(local->hw.flags & IEEE80211_HW_AP_LINK_PS))
		drv_sta_notify(local, sdata, STA_NOTIFY_SLEEP, &sta->sta);
	ps_dbg(sdata, "STA %pM aid %d enters power save mode\n",
	       sta->sta.addr, sta->sta.aid);
}

static void sta_ps_end(struct sta_info *sta)
{
	ps_dbg(sta->sdata, "STA %pM aid %d exits power save mode\n",
	       sta->sta.addr, sta->sta.aid);

	if (test_sta_flag(sta, WLAN_STA_PS_DRIVER)) {
		ps_dbg(sta->sdata, "STA %pM aid %d driver-ps-blocked\n",
		       sta->sta.addr, sta->sta.aid);
		return;
	}

	ieee80211_sta_ps_deliver_wakeup(sta);
}

int ieee80211_sta_ps_transition(struct ieee80211_sta *sta, bool start)
{
	struct sta_info *sta_inf = container_of(sta, struct sta_info, sta);
	bool in_ps;

	WARN_ON(!(sta_inf->local->hw.flags & IEEE80211_HW_AP_LINK_PS));

	/*                                          */
	in_ps = test_sta_flag(sta_inf, WLAN_STA_PS_STA);
	if ((start && in_ps) || (!start && !in_ps))
		return -EINVAL;

	if (start)
		sta_ps_start(sta_inf);
	else
		sta_ps_end(sta_inf);

	return 0;
}
EXPORT_SYMBOL(ieee80211_sta_ps_transition);

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_uapsd_and_pspoll(struct ieee80211_rx_data *rx)
{
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_hdr *hdr = (void *)rx->skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);
	int tid, ac;

	if (!rx->sta || !(status->rx_flags & IEEE80211_RX_RA_MATCH))
		return RX_CONTINUE;

	if (sdata->vif.type != NL80211_IFTYPE_AP &&
	    sdata->vif.type != NL80211_IFTYPE_AP_VLAN)
		return RX_CONTINUE;

	/*
                                                                    
                                                                    
                                          
  */
	if (sdata->local->hw.flags & IEEE80211_HW_AP_LINK_PS)
		return RX_CONTINUE;

	/*
                                                             
                                                               
                                                        
  */
	if (!test_sta_flag(rx->sta, WLAN_STA_PS_STA))
		return RX_CONTINUE;

	if (unlikely(ieee80211_is_pspoll(hdr->frame_control))) {
		if (!test_sta_flag(rx->sta, WLAN_STA_SP)) {
			if (!test_sta_flag(rx->sta, WLAN_STA_PS_DRIVER))
				ieee80211_sta_ps_deliver_poll_response(rx->sta);
			else
				set_sta_flag(rx->sta, WLAN_STA_PSPOLL);
		}

		/*                                                              
                                */
		dev_kfree_skb(rx->skb);

		return RX_QUEUED;
	} else if (!ieee80211_has_morefrags(hdr->frame_control) &&
		   !(status->rx_flags & IEEE80211_RX_DEFERRED_RELEASE) &&
		   ieee80211_has_pm(hdr->frame_control) &&
		   (ieee80211_is_data_qos(hdr->frame_control) ||
		    ieee80211_is_qos_nullfunc(hdr->frame_control))) {
		tid = *ieee80211_get_qos_ctl(hdr) & IEEE80211_QOS_CTL_TID_MASK;
		ac = ieee802_1d_to_ac[tid & 7];

		/*
                                                  
    
                                                              
                                                            
                                                          
   */
		if (!(rx->sta->sta.uapsd_queues & BIT(ac)))
			return RX_CONTINUE;

		/*                                           */
		if (test_sta_flag(rx->sta, WLAN_STA_SP))
			return RX_CONTINUE;

		if (!test_sta_flag(rx->sta, WLAN_STA_PS_DRIVER))
			ieee80211_sta_ps_deliver_uapsd(rx->sta);
		else
			set_sta_flag(rx->sta, WLAN_STA_UAPSD);
	}

	return RX_CONTINUE;
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_sta_process(struct ieee80211_rx_data *rx)
{
	struct sta_info *sta = rx->sta;
	struct sk_buff *skb = rx->skb;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;

	if (!sta)
		return RX_CONTINUE;

	/*
                                                                  
                                                                 
                                                              
                                                                  
                                                              
                                        
  */
	if (rx->sdata->vif.type == NL80211_IFTYPE_ADHOC) {
		u8 *bssid = ieee80211_get_bssid(hdr, rx->skb->len,
						NL80211_IFTYPE_ADHOC);
		if (ether_addr_equal(bssid, rx->sdata->u.ibss.bssid) &&
		    test_sta_flag(sta, WLAN_STA_AUTHORIZED)) {
			sta->last_rx = jiffies;
			if (ieee80211_is_data(hdr->frame_control)) {
				sta->last_rx_rate_idx = status->rate_idx;
				sta->last_rx_rate_flag = status->flag;
				sta->last_rx_rate_vht_nss = status->vht_nss;
			}
		}
	} else if (!is_multicast_ether_addr(hdr->addr1)) {
		/*
                                                               
                                                          
   */
		sta->last_rx = jiffies;
		if (ieee80211_is_data(hdr->frame_control)) {
			sta->last_rx_rate_idx = status->rate_idx;
			sta->last_rx_rate_flag = status->flag;
			sta->last_rx_rate_vht_nss = status->vht_nss;
		}
	}

	if (!(status->rx_flags & IEEE80211_RX_RA_MATCH))
		return RX_CONTINUE;

	if (rx->sdata->vif.type == NL80211_IFTYPE_STATION)
		ieee80211_sta_rx_notify(rx->sdata, hdr);

	sta->rx_fragments++;
	sta->rx_bytes += rx->skb->len;
	if (!(status->flag & RX_FLAG_NO_SIGNAL_VAL)) {
		sta->last_signal = status->signal;
		ewma_add(&sta->avg_signal, -status->signal);
	}

	/*
                                                           
                      
  */
	if (!(sta->local->hw.flags & IEEE80211_HW_AP_LINK_PS) &&
	    !ieee80211_has_morefrags(hdr->frame_control) &&
	    !(status->rx_flags & IEEE80211_RX_DEFERRED_RELEASE) &&
	    (rx->sdata->vif.type == NL80211_IFTYPE_AP ||
	     rx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN)) {
		if (test_sta_flag(sta, WLAN_STA_PS_STA)) {
			/*
                                            
                                                
                                               
                                             
                                                  
                                      
    */
			if (ieee80211_is_data(hdr->frame_control) &&
			    !ieee80211_has_pm(hdr->frame_control))
				sta_ps_end(sta);
		} else {
			if (ieee80211_has_pm(hdr->frame_control))
				sta_ps_start(sta);
		}
	}

	/*                         */
	if (ieee80211_vif_is_mesh(&rx->sdata->vif))
		ieee80211_mps_rx_h_sta_process(sta, hdr);

	/*
                                                         
                                                       
  */
	if (ieee80211_is_nullfunc(hdr->frame_control) ||
	    ieee80211_is_qos_nullfunc(hdr->frame_control)) {
		I802_DEBUG_INC(rx->local->rx_handlers_drop_nullfunc);

		/*
                                                     
                                                     
                                                      
                                        
   */
		if (ieee80211_has_a4(hdr->frame_control) &&
		    (rx->sdata->vif.type == NL80211_IFTYPE_AP ||
		     (rx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN &&
		      !rx->sdata->u.vlan.sta))) {
			if (!test_and_set_sta_flag(sta, WLAN_STA_4ADDR_EVENT))
				cfg80211_rx_unexpected_4addr_frame(
					rx->sdata->dev, sta->sta.addr,
					GFP_ATOMIC);
			return RX_DROP_MONITOR;
		}
		/*
                                                 
                                       
   */
		sta->rx_packets++;
		dev_kfree_skb(rx->skb);
		return RX_QUEUED;
	}

	return RX_CONTINUE;
} /*                            */

static inline struct ieee80211_fragment_entry *
ieee80211_reassemble_add(struct ieee80211_sub_if_data *sdata,
			 unsigned int frag, unsigned int seq, int rx_queue,
			 struct sk_buff **skb)
{
	struct ieee80211_fragment_entry *entry;

	entry = &sdata->fragments[sdata->fragment_next++];
	if (sdata->fragment_next >= IEEE80211_FRAGMENT_MAX)
		sdata->fragment_next = 0;

	if (!skb_queue_empty(&entry->skb_list))
		__skb_queue_purge(&entry->skb_list);

	__skb_queue_tail(&entry->skb_list, *skb); /*                     */
	*skb = NULL;
	entry->first_frag_time = jiffies;
	entry->seq = seq;
	entry->rx_queue = rx_queue;
	entry->last_frag = frag;
	entry->ccmp = 0;
	entry->extra_len = 0;

	return entry;
}

static inline struct ieee80211_fragment_entry *
ieee80211_reassemble_find(struct ieee80211_sub_if_data *sdata,
			  unsigned int frag, unsigned int seq,
			  int rx_queue, struct ieee80211_hdr *hdr)
{
	struct ieee80211_fragment_entry *entry;
	int i, idx;

	idx = sdata->fragment_next;
	for (i = 0; i < IEEE80211_FRAGMENT_MAX; i++) {
		struct ieee80211_hdr *f_hdr;

		idx--;
		if (idx < 0)
			idx = IEEE80211_FRAGMENT_MAX - 1;

		entry = &sdata->fragments[idx];
		if (skb_queue_empty(&entry->skb_list) || entry->seq != seq ||
		    entry->rx_queue != rx_queue ||
		    entry->last_frag + 1 != frag)
			continue;

		f_hdr = (struct ieee80211_hdr *)entry->skb_list.next->data;

		/*
                                                                  
   */
		if (((hdr->frame_control ^ f_hdr->frame_control) &
		     cpu_to_le16(IEEE80211_FCTL_FTYPE)) ||
		    !ether_addr_equal(hdr->addr1, f_hdr->addr1) ||
		    !ether_addr_equal(hdr->addr2, f_hdr->addr2))
			continue;

		if (time_after(jiffies, entry->first_frag_time + 2 * HZ)) {
			__skb_queue_purge(&entry->skb_list);
			continue;
		}
		return entry;
	}

	return NULL;
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_defragment(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *hdr;
	u16 sc;
	__le16 fc;
	unsigned int frag, seq;
	struct ieee80211_fragment_entry *entry;
	struct sk_buff *skb;
	struct ieee80211_rx_status *status;

	hdr = (struct ieee80211_hdr *)rx->skb->data;
	fc = hdr->frame_control;

	if (ieee80211_is_ctl(fc))
		return RX_CONTINUE;

	sc = le16_to_cpu(hdr->seq_ctrl);
	frag = sc & IEEE80211_SCTL_FRAG;

	if (likely((!ieee80211_has_morefrags(fc) && frag == 0) ||
		   is_multicast_ether_addr(hdr->addr1))) {
		/*                */
		goto out;
	}
	I802_DEBUG_INC(rx->local->rx_handlers_fragments);

	if (skb_linearize(rx->skb))
		return RX_DROP_UNUSABLE;

	/*
                                                   
                                                            
                                    
  */
	hdr = (struct ieee80211_hdr *)rx->skb->data;
	seq = (sc & IEEE80211_SCTL_SEQ) >> 4;

	if (frag == 0) {
		/*                                            */
		entry = ieee80211_reassemble_add(rx->sdata, frag, seq,
						 rx->seqno_idx, &(rx->skb));
		if (rx->key && rx->key->conf.cipher == WLAN_CIPHER_SUITE_CCMP &&
		    ieee80211_has_protected(fc)) {
			int queue = rx->security_idx;
			/*                                                  
                                          */
			entry->ccmp = 1;
			memcpy(entry->last_pn,
			       rx->key->u.ccmp.rx_pn[queue],
			       CCMP_PN_LEN);
		}
		return RX_QUEUED;
	}

	/*                                                                 
                                                                      
  */
	entry = ieee80211_reassemble_find(rx->sdata, frag, seq,
					  rx->seqno_idx, hdr);
	if (!entry) {
		I802_DEBUG_INC(rx->local->rx_handlers_drop_defrag);
		return RX_DROP_MONITOR;
	}

	/*                                                             
                              */
	if (entry->ccmp) {
		int i;
		u8 pn[CCMP_PN_LEN], *rpn;
		int queue;
		if (!rx->key || rx->key->conf.cipher != WLAN_CIPHER_SUITE_CCMP)
			return RX_DROP_UNUSABLE;
		memcpy(pn, entry->last_pn, CCMP_PN_LEN);
		for (i = CCMP_PN_LEN - 1; i >= 0; i--) {
			pn[i]++;
			if (pn[i])
				break;
		}
		queue = rx->security_idx;
		rpn = rx->key->u.ccmp.rx_pn[queue];
		if (memcmp(pn, rpn, CCMP_PN_LEN))
			return RX_DROP_UNUSABLE;
		memcpy(entry->last_pn, pn, CCMP_PN_LEN);
	}

	skb_pull(rx->skb, ieee80211_hdrlen(fc));
	__skb_queue_tail(&entry->skb_list, rx->skb);
	entry->last_frag = frag;
	entry->extra_len += rx->skb->len;
	if (ieee80211_has_morefrags(fc)) {
		rx->skb = NULL;
		return RX_QUEUED;
	}

	rx->skb = __skb_dequeue(&entry->skb_list);
	if (skb_tailroom(rx->skb) < entry->extra_len) {
		I802_DEBUG_INC(rx->local->rx_expand_skb_head2);
		if (unlikely(pskb_expand_head(rx->skb, 0, entry->extra_len,
					      GFP_ATOMIC))) {
			I802_DEBUG_INC(rx->local->rx_handlers_drop_defrag);
			__skb_queue_purge(&entry->skb_list);
			return RX_DROP_UNUSABLE;
		}
	}
	while ((skb = __skb_dequeue(&entry->skb_list))) {
		memcpy(skb_put(rx->skb, skb->len), skb->data, skb->len);
		dev_kfree_skb(skb);
	}

	/*                                                      */
	status = IEEE80211_SKB_RXCB(rx->skb);
	status->rx_flags |= IEEE80211_RX_FRAGMENTED;

 out:
	if (rx->sta)
		rx->sta->rx_packets++;
	if (is_multicast_ether_addr(hdr->addr1))
		rx->local->dot11MulticastReceivedFrameCount++;
	else
		ieee80211_led_rx(rx->local);
	return RX_CONTINUE;
}

static int ieee80211_802_1x_port_control(struct ieee80211_rx_data *rx)
{
	if (unlikely(!rx->sta || !test_sta_flag(rx->sta, WLAN_STA_AUTHORIZED)))
		return -EACCES;

	return 0;
}

static int ieee80211_drop_unencrypted(struct ieee80211_rx_data *rx, __le16 fc)
{
	struct sk_buff *skb = rx->skb;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);

	/*
                                                       
                           
  */
	if (status->flag & RX_FLAG_DECRYPTED)
		return 0;

	/*                                        */
	if (unlikely(!ieee80211_has_protected(fc) &&
		     !ieee80211_is_nullfunc(fc) &&
		     ieee80211_is_data(fc) &&
		     (rx->key || rx->sdata->drop_unencrypted)))
		return -EACCES;

	return 0;
}

static int ieee80211_drop_unencrypted_mgmt(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);
	__le16 fc = hdr->frame_control;

	/*
                                                       
                           
  */
	if (status->flag & RX_FLAG_DECRYPTED)
		return 0;

	if (rx->sta && test_sta_flag(rx->sta, WLAN_STA_MFP)) {
		if (unlikely(!ieee80211_has_protected(fc) &&
			     ieee80211_is_unicast_robust_mgmt_frame(rx->skb) &&
			     rx->key)) {
			if (ieee80211_is_deauth(fc))
				cfg80211_send_unprot_deauth(rx->sdata->dev,
							    rx->skb->data,
							    rx->skb->len);
			else if (ieee80211_is_disassoc(fc))
				cfg80211_send_unprot_disassoc(rx->sdata->dev,
							      rx->skb->data,
							      rx->skb->len);
			return -EACCES;
		}
		/*                                                         */
		if (unlikely(ieee80211_is_multicast_robust_mgmt_frame(rx->skb) &&
			     ieee80211_get_mmie_keyidx(rx->skb) < 0)) {
			if (ieee80211_is_deauth(fc))
				cfg80211_send_unprot_deauth(rx->sdata->dev,
							    rx->skb->data,
							    rx->skb->len);
			else if (ieee80211_is_disassoc(fc))
				cfg80211_send_unprot_disassoc(rx->sdata->dev,
							      rx->skb->data,
							      rx->skb->len);
			return -EACCES;
		}
		/*
                                                           
                            
   */
		if (unlikely(ieee80211_is_action(fc) && !rx->key &&
			     ieee80211_is_robust_mgmt_frame(
				     (struct ieee80211_hdr *) rx->skb->data)))
			return -EACCES;
	}

	return 0;
}

static int
__ieee80211_data_to_8023(struct ieee80211_rx_data *rx, bool *port_control)
{
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;
	bool check_port_control = false;
	struct ethhdr *ehdr;
	int ret;

	*port_control = false;
	if (ieee80211_has_a4(hdr->frame_control) &&
	    sdata->vif.type == NL80211_IFTYPE_AP_VLAN && !sdata->u.vlan.sta)
		return -1;

	if (sdata->vif.type == NL80211_IFTYPE_STATION &&
	    !!sdata->u.mgd.use_4addr != !!ieee80211_has_a4(hdr->frame_control)) {

		if (!sdata->u.mgd.use_4addr)
			return -1;
		else
			check_port_control = true;
	}

	if (is_multicast_ether_addr(hdr->addr1) &&
	    sdata->vif.type == NL80211_IFTYPE_AP_VLAN && sdata->u.vlan.sta)
		return -1;

	ret = ieee80211_data_to_8023(rx->skb, sdata->vif.addr, sdata->vif.type);
	if (ret < 0)
		return ret;

	ehdr = (struct ethhdr *) rx->skb->data;
	if (ehdr->h_proto == rx->sdata->control_port_protocol)
		*port_control = true;
	else if (check_port_control)
		return -1;

	return 0;
}

/*
                                                        
 */
static bool ieee80211_frame_allowed(struct ieee80211_rx_data *rx, __le16 fc)
{
	static const u8 pae_group_addr[ETH_ALEN] __aligned(2)
		= { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x03 };
	struct ethhdr *ehdr = (struct ethhdr *) rx->skb->data;

	/*
                                                             
                                              
  */
	if (ehdr->h_proto == rx->sdata->control_port_protocol &&
	    (ether_addr_equal(ehdr->h_dest, rx->sdata->vif.addr) ||
	     ether_addr_equal(ehdr->h_dest, pae_group_addr)))
		return true;

	if (ieee80211_802_1x_port_control(rx) ||
	    ieee80211_drop_unencrypted(rx, fc))
		return false;

	return true;
}

/*
                                                        
 */
static void
ieee80211_deliver_skb(struct ieee80211_rx_data *rx)
{
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct net_device *dev = sdata->dev;
	struct sk_buff *skb, *xmit_skb;
	struct ethhdr *ehdr = (struct ethhdr *) rx->skb->data;
	struct sta_info *dsta;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);

	skb = rx->skb;
	xmit_skb = NULL;

	if ((sdata->vif.type == NL80211_IFTYPE_AP ||
	     sdata->vif.type == NL80211_IFTYPE_AP_VLAN) &&
	    !(sdata->flags & IEEE80211_SDATA_DONT_BRIDGE_PACKETS) &&
	    (status->rx_flags & IEEE80211_RX_RA_MATCH) &&
	    (sdata->vif.type != NL80211_IFTYPE_AP_VLAN || !sdata->u.vlan.sta)) {
		if (is_multicast_ether_addr(ehdr->h_dest)) {
			/*
                                                    
                                                     
    */
			xmit_skb = skb_copy(skb, GFP_ATOMIC);
			if (!xmit_skb)
				net_info_ratelimited("%s: failed to clone multicast frame\n",
						    dev->name);
		} else {
			dsta = sta_info_get(sdata, skb->data);
			if (dsta) {
				/*
                                               
                                                
                                                 
                 
     */
				xmit_skb = skb;
				skb = NULL;
			}
		}
	}

	if (skb) {
		int align __maybe_unused;

#ifndef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
		/*
                                                  
                                                
                                              
                                                  
                                                 
                                              
                        
   */
		align = ((unsigned long)(skb->data + sizeof(struct ethhdr))) & 3;
		if (align) {
			if (WARN_ON(skb_headroom(skb) < 3)) {
				dev_kfree_skb(skb);
				skb = NULL;
			} else {
				u8 *data = skb->data;
				size_t len = skb_headlen(skb);
				skb->data -= align;
				memmove(skb->data, data, len);
				skb_set_tail_pointer(skb, len);
			}
		}
#endif

		if (skb) {
			/*                        */
			skb->protocol = eth_type_trans(skb, dev);
			memset(skb->cb, 0, sizeof(skb->cb));
			netif_receive_skb(skb);
		}
	}

	if (xmit_skb) {
		/*
                                                           
                                                        
                                            
   */
		xmit_skb->priority += 256;
		xmit_skb->protocol = htons(ETH_P_802_3);
		skb_reset_network_header(xmit_skb);
		skb_reset_mac_header(xmit_skb);
		dev_queue_xmit(xmit_skb);
	}
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_amsdu(struct ieee80211_rx_data *rx)
{
	struct net_device *dev = rx->sdata->dev;
	struct sk_buff *skb = rx->skb;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	__le16 fc = hdr->frame_control;
	struct sk_buff_head frame_list;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);

	if (unlikely(!ieee80211_is_data(fc)))
		return RX_CONTINUE;

	if (unlikely(!ieee80211_is_data_present(fc)))
		return RX_DROP_MONITOR;

	if (!(status->rx_flags & IEEE80211_RX_AMSDU))
		return RX_CONTINUE;

	if (ieee80211_has_a4(hdr->frame_control) &&
	    rx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN &&
	    !rx->sdata->u.vlan.sta)
		return RX_DROP_UNUSABLE;

	if (is_multicast_ether_addr(hdr->addr1) &&
	    ((rx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN &&
	      rx->sdata->u.vlan.sta) ||
	     (rx->sdata->vif.type == NL80211_IFTYPE_STATION &&
	      rx->sdata->u.mgd.use_4addr)))
		return RX_DROP_UNUSABLE;

	skb->dev = dev;
	__skb_queue_head_init(&frame_list);

	if (skb_linearize(skb))
		return RX_DROP_UNUSABLE;

	ieee80211_amsdu_to_8023s(skb, &frame_list, dev->dev_addr,
				 rx->sdata->vif.type,
				 rx->local->hw.extra_tx_headroom, true);

	while (!skb_queue_empty(&frame_list)) {
		rx->skb = __skb_dequeue(&frame_list);

		if (!ieee80211_frame_allowed(rx, fc)) {
			dev_kfree_skb(rx->skb);
			continue;
		}
		dev->stats.rx_packets++;
		dev->stats.rx_bytes += rx->skb->len;

		ieee80211_deliver_skb(rx);
	}

	return RX_QUEUED;
}

#ifdef CONFIG_MAC80211_MESH
static ieee80211_rx_result
ieee80211_rx_h_mesh_fwding(struct ieee80211_rx_data *rx)
{
	struct ieee80211_hdr *fwd_hdr, *hdr;
	struct ieee80211_tx_info *info;
	struct ieee80211s_hdr *mesh_hdr;
	struct sk_buff *skb = rx->skb, *fwd_skb;
	struct ieee80211_local *local = rx->local;
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	__le16 reason = cpu_to_le16(WLAN_REASON_MESH_PATH_NOFORWARD);
	u16 q, hdrlen;

	hdr = (struct ieee80211_hdr *) skb->data;
	hdrlen = ieee80211_hdrlen(hdr->frame_control);

	/*                                                                   */
	if (!pskb_may_pull(rx->skb, hdrlen + 6))
		return RX_DROP_MONITOR;

	mesh_hdr = (struct ieee80211s_hdr *) (skb->data + hdrlen);

	/*                                                          */
	if (!pskb_may_pull(rx->skb,
			   hdrlen + ieee80211_get_mesh_hdrlen(mesh_hdr)))
		return RX_DROP_MONITOR;

	/*                 */
	hdr = (struct ieee80211_hdr *) skb->data;
	mesh_hdr = (struct ieee80211s_hdr *) (skb->data + hdrlen);

	/*                                */
	if (ieee80211_is_data(hdr->frame_control) &&
	    is_multicast_ether_addr(hdr->addr1) &&
	    mesh_rmc_check(rx->sdata, hdr->addr3, mesh_hdr))
		return RX_DROP_MONITOR;

	if (!ieee80211_is_data(hdr->frame_control) ||
	    !(status->rx_flags & IEEE80211_RX_RA_MATCH))
		return RX_CONTINUE;

	if (!mesh_hdr->ttl)
		return RX_DROP_MONITOR;

	if (mesh_hdr->flags & MESH_FLAGS_AE) {
		struct mesh_path *mppath;
		char *proxied_addr;
		char *mpp_addr;

		if (is_multicast_ether_addr(hdr->addr1)) {
			mpp_addr = hdr->addr3;
			proxied_addr = mesh_hdr->eaddr1;
		} else if (mesh_hdr->flags & MESH_FLAGS_AE_A5_A6) {
			/*                                                   */
			mpp_addr = hdr->addr4;
			proxied_addr = mesh_hdr->eaddr2;
		} else {
			return RX_DROP_MONITOR;
		}

		rcu_read_lock();
		mppath = mpp_path_lookup(sdata, proxied_addr);
		if (!mppath) {
			mpp_path_add(sdata, proxied_addr, mpp_addr);
		} else {
			spin_lock_bh(&mppath->state_lock);
			if (!ether_addr_equal(mppath->mpp, mpp_addr))
				memcpy(mppath->mpp, mpp_addr, ETH_ALEN);
			spin_unlock_bh(&mppath->state_lock);
		}
		rcu_read_unlock();
	}

	/*                                               */
	if (!is_multicast_ether_addr(hdr->addr1) &&
	    ether_addr_equal(sdata->vif.addr, hdr->addr3))
		return RX_CONTINUE;

	q = ieee80211_select_queue_80211(sdata, skb, hdr);
	if (ieee80211_queue_stopped(&local->hw, q)) {
		IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, dropped_frames_congestion);
		return RX_DROP_MONITOR;
	}
	skb_set_queue_mapping(skb, q);

	if (!--mesh_hdr->ttl) {
		IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, dropped_frames_ttl);
		goto out;
	}

	if (!ifmsh->mshcfg.dot11MeshForwarding)
		goto out;

	fwd_skb = skb_copy(skb, GFP_ATOMIC);
	if (!fwd_skb) {
		net_info_ratelimited("%s: failed to clone mesh frame\n",
				    sdata->name);
		goto out;
	}

	fwd_hdr =  (struct ieee80211_hdr *) fwd_skb->data;
	fwd_hdr->frame_control &= ~cpu_to_le16(IEEE80211_FCTL_RETRY);
	info = IEEE80211_SKB_CB(fwd_skb);
	memset(info, 0, sizeof(*info));
	info->flags |= IEEE80211_TX_INTFL_NEED_TXPROCESSING;
	info->control.vif = &rx->sdata->vif;
	info->control.jiffies = jiffies;
	if (is_multicast_ether_addr(fwd_hdr->addr1)) {
		IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, fwded_mcast);
		memcpy(fwd_hdr->addr2, sdata->vif.addr, ETH_ALEN);
		/*                                              */
		ieee80211_mps_set_frame_flags(sdata, NULL, fwd_hdr);
	} else if (!mesh_nexthop_lookup(sdata, fwd_skb)) {
		/*                                                      */
		IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, fwded_unicast);
	} else {
		/*                            */
		mesh_path_error_tx(sdata, ifmsh->mshcfg.element_ttl,
				   fwd_hdr->addr3, 0, reason, fwd_hdr->addr2);
		IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, dropped_frames_no_route);
		kfree_skb(fwd_skb);
		return RX_DROP_MONITOR;
	}

	IEEE80211_IFSTA_MESH_CTR_INC(ifmsh, fwded_frames);
	ieee80211_add_pending_skb(local, fwd_skb);
 out:
	if (is_multicast_ether_addr(hdr->addr1) ||
	    sdata->dev->flags & IFF_PROMISC)
		return RX_CONTINUE;
	else
		return RX_DROP_MONITOR;
}
#endif

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_data(struct ieee80211_rx_data *rx)
{
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_local *local = rx->local;
	struct net_device *dev = sdata->dev;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)rx->skb->data;
	__le16 fc = hdr->frame_control;
	bool port_control;
	int err;

	if (unlikely(!ieee80211_is_data(hdr->frame_control)))
		return RX_CONTINUE;

	if (unlikely(!ieee80211_is_data_present(hdr->frame_control)))
		return RX_DROP_MONITOR;

	/*
                                                                     
                                                     
  */
	if (ieee80211_has_a4(hdr->frame_control) &&
	    sdata->vif.type == NL80211_IFTYPE_AP) {
		if (rx->sta &&
		    !test_and_set_sta_flag(rx->sta, WLAN_STA_4ADDR_EVENT))
			cfg80211_rx_unexpected_4addr_frame(
				rx->sdata->dev, rx->sta->sta.addr, GFP_ATOMIC);
		return RX_DROP_MONITOR;
	}

	err = __ieee80211_data_to_8023(rx, &port_control);
	if (unlikely(err))
		return RX_DROP_UNUSABLE;

	if (!ieee80211_frame_allowed(rx, fc))
		return RX_DROP_MONITOR;

	if (rx->sdata->vif.type == NL80211_IFTYPE_AP_VLAN &&
	    unlikely(port_control) && sdata->bss) {
		sdata = container_of(sdata->bss, struct ieee80211_sub_if_data,
				     u.ap);
		dev = sdata->dev;
		rx->sdata = sdata;
	}

	rx->skb->dev = dev;

	dev->stats.rx_packets++;
	dev->stats.rx_bytes += rx->skb->len;

	if (local->ps_sdata && local->hw.conf.dynamic_ps_timeout > 0 &&
	    !is_multicast_ether_addr(
		    ((struct ethhdr *)rx->skb->data)->h_dest) &&
	    (!local->scanning &&
	     !test_bit(SDATA_STATE_OFFCHANNEL, &sdata->state))) {
			mod_timer(&local->dynamic_ps_timer, jiffies +
			 msecs_to_jiffies(local->hw.conf.dynamic_ps_timeout));
	}

	ieee80211_deliver_skb(rx);

	return RX_QUEUED;
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_ctrl(struct ieee80211_rx_data *rx, struct sk_buff_head *frames)
{
	struct sk_buff *skb = rx->skb;
	struct ieee80211_bar *bar = (struct ieee80211_bar *)skb->data;
	struct tid_ampdu_rx *tid_agg_rx;
	u16 start_seq_num;
	u16 tid;

	if (likely(!ieee80211_is_ctl(bar->frame_control)))
		return RX_CONTINUE;

	if (ieee80211_is_back_req(bar->frame_control)) {
		struct {
			__le16 control, start_seq_num;
		} __packed bar_data;

		if (!rx->sta)
			return RX_DROP_MONITOR;

		if (skb_copy_bits(skb, offsetof(struct ieee80211_bar, control),
				  &bar_data, sizeof(bar_data)))
			return RX_DROP_MONITOR;

		tid = le16_to_cpu(bar_data.control) >> 12;

		tid_agg_rx = rcu_dereference(rx->sta->ampdu_mlme.tid_rx[tid]);
		if (!tid_agg_rx)
			return RX_DROP_MONITOR;

		start_seq_num = le16_to_cpu(bar_data.start_seq_num) >> 4;

		/*                     */
		if (tid_agg_rx->timeout)
			mod_timer(&tid_agg_rx->session_timer,
				  TU_TO_EXP_TIME(tid_agg_rx->timeout));

		spin_lock(&tid_agg_rx->reorder_lock);
		/*                                          */
		ieee80211_release_reorder_frames(rx->sdata, tid_agg_rx,
						 start_seq_num, frames);
		spin_unlock(&tid_agg_rx->reorder_lock);

		kfree_skb(skb);
		return RX_QUEUED;
	}

	/*
                                                     
                                                  
                              
  */
	return RX_DROP_MONITOR;
}

static void ieee80211_process_sa_query_req(struct ieee80211_sub_if_data *sdata,
					   struct ieee80211_mgmt *mgmt,
					   size_t len)
{
	struct ieee80211_local *local = sdata->local;
	struct sk_buff *skb;
	struct ieee80211_mgmt *resp;

	if (!ether_addr_equal(mgmt->da, sdata->vif.addr)) {
		/*                            */
		return;
	}

	if (!ether_addr_equal(mgmt->sa, sdata->u.mgd.bssid) ||
	    !ether_addr_equal(mgmt->bssid, sdata->u.mgd.bssid)) {
		/*                                                */
		return;
	}

	if (len < 24 + 1 + sizeof(resp->u.action.u.sa_query)) {
		/*                                  */
		return;
	}

	skb = dev_alloc_skb(sizeof(*resp) + local->hw.extra_tx_headroom);
	if (skb == NULL)
		return;

	skb_reserve(skb, local->hw.extra_tx_headroom);
	resp = (struct ieee80211_mgmt *) skb_put(skb, 24);
	memset(resp, 0, 24);
	memcpy(resp->da, mgmt->sa, ETH_ALEN);
	memcpy(resp->sa, sdata->vif.addr, ETH_ALEN);
	memcpy(resp->bssid, sdata->u.mgd.bssid, ETH_ALEN);
	resp->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
					  IEEE80211_STYPE_ACTION);
	skb_put(skb, 1 + sizeof(resp->u.action.u.sa_query));
	resp->u.action.category = WLAN_CATEGORY_SA_QUERY;
	resp->u.action.u.sa_query.action = WLAN_ACTION_SA_QUERY_RESPONSE;
	memcpy(resp->u.action.u.sa_query.trans_id,
	       mgmt->u.action.u.sa_query.trans_id,
	       WLAN_SA_QUERY_TR_ID_LEN);

	ieee80211_tx_skb(sdata, skb);
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_mgmt_check(struct ieee80211_rx_data *rx)
{
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *) rx->skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);

	/*
                                                 
                                                
                                              
  */
	if (rx->skb->len < 24)
		return RX_DROP_MONITOR;

	if (!ieee80211_is_mgmt(mgmt->frame_control))
		return RX_DROP_MONITOR;

	if (rx->sdata->vif.type == NL80211_IFTYPE_AP &&
	    ieee80211_is_beacon(mgmt->frame_control) &&
	    !(rx->flags & IEEE80211_RX_BEACON_REPORTED)) {
		int sig = 0;

		if (rx->local->hw.flags & IEEE80211_HW_SIGNAL_DBM)
			sig = status->signal;

		cfg80211_report_obss_beacon(rx->local->hw.wiphy,
					    rx->skb->data, rx->skb->len,
					    status->freq, sig);
		rx->flags |= IEEE80211_RX_BEACON_REPORTED;
	}

	if (!(status->rx_flags & IEEE80211_RX_RA_MATCH))
		return RX_DROP_MONITOR;

	if (ieee80211_drop_unencrypted_mgmt(rx))
		return RX_DROP_UNUSABLE;

	return RX_CONTINUE;
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_action(struct ieee80211_rx_data *rx)
{
	struct ieee80211_local *local = rx->local;
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *) rx->skb->data;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);
	int len = rx->skb->len;

	if (!ieee80211_is_action(mgmt->frame_control))
		return RX_CONTINUE;

	/*                       */
	if (len < IEEE80211_MIN_ACTION_SIZE)
		return RX_DROP_UNUSABLE;

	if (!rx->sta && mgmt->u.action.category != WLAN_CATEGORY_PUBLIC &&
	    mgmt->u.action.category != WLAN_CATEGORY_SELF_PROTECTED)
		return RX_DROP_UNUSABLE;

	if (!(status->rx_flags & IEEE80211_RX_RA_MATCH))
		return RX_DROP_UNUSABLE;

	switch (mgmt->u.action.category) {
	case WLAN_CATEGORY_HT:
		/*                                                         */
		if (!rx->sta->sta.ht_cap.ht_supported)
			goto invalid;

		if (sdata->vif.type != NL80211_IFTYPE_STATION &&
		    sdata->vif.type != NL80211_IFTYPE_MESH_POINT &&
		    sdata->vif.type != NL80211_IFTYPE_AP_VLAN &&
		    sdata->vif.type != NL80211_IFTYPE_AP &&
		    sdata->vif.type != NL80211_IFTYPE_ADHOC)
			break;

		/*                                                    */
		if (len < IEEE80211_MIN_ACTION_SIZE + 2)
			goto invalid;

		switch (mgmt->u.action.u.ht_smps.action) {
		case WLAN_HT_ACTION_SMPS: {
			struct ieee80211_supported_band *sband;
			enum ieee80211_smps_mode smps_mode;

			/*                          */
			switch (mgmt->u.action.u.ht_smps.smps_control) {
			case WLAN_HT_SMPS_CONTROL_DISABLED:
				smps_mode = IEEE80211_SMPS_OFF;
				break;
			case WLAN_HT_SMPS_CONTROL_STATIC:
				smps_mode = IEEE80211_SMPS_STATIC;
				break;
			case WLAN_HT_SMPS_CONTROL_DYNAMIC:
				smps_mode = IEEE80211_SMPS_DYNAMIC;
				break;
			default:
				goto invalid;
			}

			/*                         */
			if (rx->sta->sta.smps_mode == smps_mode)
				goto handled;
			rx->sta->sta.smps_mode = smps_mode;

			sband = rx->local->hw.wiphy->bands[status->band];

			rate_control_rate_update(local, sband, rx->sta,
						 IEEE80211_RC_SMPS_CHANGED);
			goto handled;
		}
		case WLAN_HT_ACTION_NOTIFY_CHANWIDTH: {
			struct ieee80211_supported_band *sband;
			u8 chanwidth = mgmt->u.action.u.ht_notify_cw.chanwidth;
			enum ieee80211_sta_rx_bandwidth new_bw;

			/*                                                  */
			if (!(rx->sta->sta.ht_cap.cap &
					IEEE80211_HT_CAP_SUP_WIDTH_20_40))
				goto handled;

			if (chanwidth == IEEE80211_HT_CHANWIDTH_20MHZ)
				new_bw = IEEE80211_STA_RX_BW_20;
			else
				new_bw = ieee80211_sta_cur_vht_bw(rx->sta);

			if (rx->sta->sta.bandwidth == new_bw)
				goto handled;

			sband = rx->local->hw.wiphy->bands[status->band];

			rate_control_rate_update(local, sband, rx->sta,
						 IEEE80211_RC_BW_CHANGED);
			goto handled;
		}
		default:
			goto invalid;
		}

		break;
	case WLAN_CATEGORY_PUBLIC:
		if (len < IEEE80211_MIN_ACTION_SIZE + 1)
			goto invalid;
		if (sdata->vif.type != NL80211_IFTYPE_STATION)
			break;
		if (!rx->sta)
			break;
		if (!ether_addr_equal(mgmt->bssid, sdata->u.mgd.bssid))
			break;
		if (mgmt->u.action.u.ext_chan_switch.action_code !=
				WLAN_PUB_ACTION_EXT_CHANSW_ANN)
			break;
		if (len < offsetof(struct ieee80211_mgmt,
				   u.action.u.ext_chan_switch.variable))
			goto invalid;
		goto queue;
	case WLAN_CATEGORY_VHT:
		if (sdata->vif.type != NL80211_IFTYPE_STATION &&
		    sdata->vif.type != NL80211_IFTYPE_MESH_POINT &&
		    sdata->vif.type != NL80211_IFTYPE_AP_VLAN &&
		    sdata->vif.type != NL80211_IFTYPE_AP &&
		    sdata->vif.type != NL80211_IFTYPE_ADHOC)
			break;

		/*                               */
		if (len < IEEE80211_MIN_ACTION_SIZE + 1)
			goto invalid;

		switch (mgmt->u.action.u.vht_opmode_notif.action_code) {
		case WLAN_VHT_ACTION_OPMODE_NOTIF: {
			u8 opmode;

			/*                          */
			if (len < IEEE80211_MIN_ACTION_SIZE + 2)
				goto invalid;

			opmode = mgmt->u.action.u.vht_opmode_notif.operating_mode;

			ieee80211_vht_handle_opmode(rx->sdata, rx->sta,
						    opmode, status->band,
						    false);
			goto handled;
		}
		default:
			break;
		}
		break;
	case WLAN_CATEGORY_BACK:
		if (sdata->vif.type != NL80211_IFTYPE_STATION &&
		    sdata->vif.type != NL80211_IFTYPE_MESH_POINT &&
		    sdata->vif.type != NL80211_IFTYPE_AP_VLAN &&
		    sdata->vif.type != NL80211_IFTYPE_AP &&
		    sdata->vif.type != NL80211_IFTYPE_ADHOC)
			break;

		/*                               */
		if (len < IEEE80211_MIN_ACTION_SIZE + 1)
			break;

		switch (mgmt->u.action.u.addba_req.action_code) {
		case WLAN_ACTION_ADDBA_REQ:
			if (len < (IEEE80211_MIN_ACTION_SIZE +
				   sizeof(mgmt->u.action.u.addba_req)))
				goto invalid;
			break;
		case WLAN_ACTION_ADDBA_RESP:
			if (len < (IEEE80211_MIN_ACTION_SIZE +
				   sizeof(mgmt->u.action.u.addba_resp)))
				goto invalid;
			break;
		case WLAN_ACTION_DELBA:
			if (len < (IEEE80211_MIN_ACTION_SIZE +
				   sizeof(mgmt->u.action.u.delba)))
				goto invalid;
			break;
		default:
			goto invalid;
		}

		goto queue;
	case WLAN_CATEGORY_SPECTRUM_MGMT:
		if (status->band != IEEE80211_BAND_5GHZ)
			break;

		if (sdata->vif.type != NL80211_IFTYPE_STATION)
			break;

		/*                               */
		if (len < IEEE80211_MIN_ACTION_SIZE + 1)
			break;

		switch (mgmt->u.action.u.measurement.action_code) {
		case WLAN_ACTION_SPCT_MSR_REQ:
			if (len < (IEEE80211_MIN_ACTION_SIZE +
				   sizeof(mgmt->u.action.u.measurement)))
				break;
			ieee80211_process_measurement_req(sdata, mgmt, len);
			goto handled;
		case WLAN_ACTION_SPCT_CHL_SWITCH:
			if (sdata->vif.type != NL80211_IFTYPE_STATION)
				break;

			if (!ether_addr_equal(mgmt->bssid, sdata->u.mgd.bssid))
				break;

			goto queue;
		}
		break;
	case WLAN_CATEGORY_SA_QUERY:
		if (len < (IEEE80211_MIN_ACTION_SIZE +
			   sizeof(mgmt->u.action.u.sa_query)))
			break;

		switch (mgmt->u.action.u.sa_query.action) {
		case WLAN_ACTION_SA_QUERY_REQUEST:
			if (sdata->vif.type != NL80211_IFTYPE_STATION)
				break;
			ieee80211_process_sa_query_req(sdata, mgmt, len);
			goto handled;
		}
		break;
	case WLAN_CATEGORY_SELF_PROTECTED:
		if (len < (IEEE80211_MIN_ACTION_SIZE +
			   sizeof(mgmt->u.action.u.self_prot.action_code)))
			break;

		switch (mgmt->u.action.u.self_prot.action_code) {
		case WLAN_SP_MESH_PEERING_OPEN:
		case WLAN_SP_MESH_PEERING_CLOSE:
		case WLAN_SP_MESH_PEERING_CONFIRM:
			if (!ieee80211_vif_is_mesh(&sdata->vif))
				goto invalid;
			if (sdata->u.mesh.user_mpm)
				/*                              */
				break;
			goto queue;
		case WLAN_SP_MGK_INFORM:
		case WLAN_SP_MGK_ACK:
			if (!ieee80211_vif_is_mesh(&sdata->vif))
				goto invalid;
			break;
		}
		break;
	case WLAN_CATEGORY_MESH_ACTION:
		if (len < (IEEE80211_MIN_ACTION_SIZE +
			   sizeof(mgmt->u.action.u.mesh_action.action_code)))
			break;

		if (!ieee80211_vif_is_mesh(&sdata->vif))
			break;
		if (mesh_action_is_path_sel(mgmt) &&
		    !mesh_path_sel_is_hwmp(sdata))
			break;
		goto queue;
	}

	return RX_CONTINUE;

 invalid:
	status->rx_flags |= IEEE80211_RX_MALFORMED_ACTION_FRM;
	/*                                  */
	return RX_CONTINUE;

 handled:
	if (rx->sta)
		rx->sta->rx_packets++;
	dev_kfree_skb(rx->skb);
	return RX_QUEUED;

 queue:
	rx->skb->pkt_type = IEEE80211_SDATA_QUEUE_TYPE_FRAME;
	skb_queue_tail(&sdata->skb_queue, rx->skb);
	ieee80211_queue_work(&local->hw, &sdata->work);
	if (rx->sta)
		rx->sta->rx_packets++;
	return RX_QUEUED;
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_userspace_mgmt(struct ieee80211_rx_data *rx)
{
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);
	int sig = 0;

	/*                                                                  */
	if (status->rx_flags & IEEE80211_RX_MALFORMED_ACTION_FRM)
		return RX_CONTINUE;

	/*
                                                            
                                                            
                                                            
                                              
  */

	if (rx->local->hw.flags & IEEE80211_HW_SIGNAL_DBM)
		sig = status->signal;

	if (cfg80211_rx_mgmt(&rx->sdata->wdev, status->freq, sig,
			     rx->skb->data, rx->skb->len,
			     GFP_ATOMIC)) {
		if (rx->sta)
			rx->sta->rx_packets++;
		dev_kfree_skb(rx->skb);
		return RX_QUEUED;
	}

	return RX_CONTINUE;
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_action_return(struct ieee80211_rx_data *rx)
{
	struct ieee80211_local *local = rx->local;
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *) rx->skb->data;
	struct sk_buff *nskb;
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(rx->skb);

	if (!ieee80211_is_action(mgmt->frame_control))
		return RX_CONTINUE;

	/*
                                                               
                                                             
                                                                
                                                               
                       
                                                                 
                                                            
                                                
  */
	if (!(status->rx_flags & IEEE80211_RX_MALFORMED_ACTION_FRM) &&
	    (sdata->vif.type == NL80211_IFTYPE_AP ||
	     sdata->vif.type == NL80211_IFTYPE_AP_VLAN))
		return RX_DROP_MONITOR;

	if (is_multicast_ether_addr(mgmt->da))
		return RX_DROP_MONITOR;

	/*                                      */
	if (mgmt->u.action.category & 0x80)
		return RX_DROP_UNUSABLE;

	nskb = skb_copy_expand(rx->skb, local->hw.extra_tx_headroom, 0,
			       GFP_ATOMIC);
	if (nskb) {
		struct ieee80211_mgmt *nmgmt = (void *)nskb->data;

		nmgmt->u.action.category |= 0x80;
		memcpy(nmgmt->da, nmgmt->sa, ETH_ALEN);
		memcpy(nmgmt->sa, rx->sdata->vif.addr, ETH_ALEN);

		memset(nskb->cb, 0, sizeof(nskb->cb));

		if (rx->sdata->vif.type == NL80211_IFTYPE_P2P_DEVICE) {
			struct ieee80211_tx_info *info = IEEE80211_SKB_CB(nskb);

			info->flags = IEEE80211_TX_CTL_TX_OFFCHAN |
				      IEEE80211_TX_INTFL_OFFCHAN_TX_OK |
				      IEEE80211_TX_CTL_NO_CCK_RATE;
			if (local->hw.flags & IEEE80211_HW_QUEUE_CONTROL)
				info->hw_queue =
					local->hw.offchannel_tx_hw_queue;
		}

		__ieee80211_tx_skb_tid_band(rx->sdata, nskb, 7,
					    status->band);
	}
	dev_kfree_skb(rx->skb);
	return RX_QUEUED;
}

static ieee80211_rx_result debug_noinline
ieee80211_rx_h_mgmt(struct ieee80211_rx_data *rx)
{
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_mgmt *mgmt = (void *)rx->skb->data;
	__le16 stype;

	stype = mgmt->frame_control & cpu_to_le16(IEEE80211_FCTL_STYPE);

	if (!ieee80211_vif_is_mesh(&sdata->vif) &&
	    sdata->vif.type != NL80211_IFTYPE_ADHOC &&
	    sdata->vif.type != NL80211_IFTYPE_STATION)
		return RX_DROP_MONITOR;

	switch (stype) {
	case cpu_to_le16(IEEE80211_STYPE_AUTH):
	case cpu_to_le16(IEEE80211_STYPE_BEACON):
	case cpu_to_le16(IEEE80211_STYPE_PROBE_RESP):
		/*                                   */
		break;
	case cpu_to_le16(IEEE80211_STYPE_ASSOC_RESP):
	case cpu_to_le16(IEEE80211_STYPE_REASSOC_RESP):
	case cpu_to_le16(IEEE80211_STYPE_DEAUTH):
	case cpu_to_le16(IEEE80211_STYPE_DISASSOC):
		if (is_multicast_ether_addr(mgmt->da) &&
		    !is_broadcast_ether_addr(mgmt->da))
			return RX_DROP_MONITOR;

		/*                          */
		if (sdata->vif.type != NL80211_IFTYPE_STATION)
			return RX_DROP_MONITOR;
		break;
	case cpu_to_le16(IEEE80211_STYPE_PROBE_REQ):
		/*                                */
		if (sdata->vif.type != NL80211_IFTYPE_ADHOC &&
		    sdata->vif.type != NL80211_IFTYPE_MESH_POINT)
			return RX_DROP_MONITOR;
		break;
	default:
		return RX_DROP_MONITOR;
	}

	/*                                                */
	rx->skb->pkt_type = IEEE80211_SDATA_QUEUE_TYPE_FRAME;
	skb_queue_tail(&sdata->skb_queue, rx->skb);
	ieee80211_queue_work(&rx->local->hw, &sdata->work);
	if (rx->sta)
		rx->sta->rx_packets++;

	return RX_QUEUED;
}

/*                                   */
static void ieee80211_rx_cooked_monitor(struct ieee80211_rx_data *rx,
					struct ieee80211_rate *rate)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_local *local = rx->local;
	struct sk_buff *skb = rx->skb, *skb2;
	struct net_device *prev_dev = NULL;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	int needed_headroom;

	/*
                                                      
                                            
  */
	if (rx->flags & IEEE80211_RX_CMNTR)
		goto out_free_skb;
	rx->flags |= IEEE80211_RX_CMNTR;

	/*                                                              */
	if (!local->cooked_mntrs)
		goto out_free_skb;

	/*                                                       */
	needed_headroom = ieee80211_rx_radiotap_space(local, status);

	if (skb_headroom(skb) < needed_headroom &&
	    pskb_expand_head(skb, needed_headroom, 0, GFP_ATOMIC))
		goto out_free_skb;

	/*                              */
	ieee80211_add_rx_radiotap_header(local, skb, rate, needed_headroom,
					 false);

	skb_set_mac_header(skb, 0);
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	skb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = htons(ETH_P_802_2);

	list_for_each_entry_rcu(sdata, &local->interfaces, list) {
		if (!ieee80211_sdata_running(sdata))
			continue;

		if (sdata->vif.type != NL80211_IFTYPE_MONITOR ||
		    !(sdata->u.mntr_flags & MONITOR_FLAG_COOK_FRAMES))
			continue;

		if (prev_dev) {
			skb2 = skb_clone(skb, GFP_ATOMIC);
			if (skb2) {
				skb2->dev = prev_dev;
				netif_receive_skb(skb2);
			}
		}

		prev_dev = sdata->dev;
		sdata->dev->stats.rx_packets++;
		sdata->dev->stats.rx_bytes += skb->len;
	}

	if (prev_dev) {
		skb->dev = prev_dev;
		netif_receive_skb(skb);
		return;
	}

 out_free_skb:
	dev_kfree_skb(skb);
}

static void ieee80211_rx_handlers_result(struct ieee80211_rx_data *rx,
					 ieee80211_rx_result res)
{
	switch (res) {
	case RX_DROP_MONITOR:
		I802_DEBUG_INC(rx->sdata->local->rx_handlers_drop);
		if (rx->sta)
			rx->sta->rx_dropped++;
		/*              */
	case RX_CONTINUE: {
		struct ieee80211_rate *rate = NULL;
		struct ieee80211_supported_band *sband;
		struct ieee80211_rx_status *status;

		status = IEEE80211_SKB_RXCB((rx->skb));

		sband = rx->local->hw.wiphy->bands[status->band];
		if (!(status->flag & RX_FLAG_HT) &&
		    !(status->flag & RX_FLAG_VHT))
			rate = &sband->bitrates[status->rate_idx];

		ieee80211_rx_cooked_monitor(rx, rate);
		break;
		}
	case RX_DROP_UNUSABLE:
		I802_DEBUG_INC(rx->sdata->local->rx_handlers_drop);
		if (rx->sta)
			rx->sta->rx_dropped++;
		dev_kfree_skb(rx->skb);
		break;
	case RX_QUEUED:
		I802_DEBUG_INC(rx->sdata->local->rx_handlers_queued);
		break;
	}
}

static void ieee80211_rx_handlers(struct ieee80211_rx_data *rx,
				  struct sk_buff_head *frames)
{
	ieee80211_rx_result res = RX_DROP_MONITOR;
	struct sk_buff *skb;

#define CALL_RXH(rxh)			\
	do {				\
		res = rxh(rx);		\
		if (res != RX_CONTINUE)	\
			goto rxh_next;  \
	} while (0);

	spin_lock_bh(&rx->local->rx_path_lock);

	while ((skb = __skb_dequeue(frames))) {
		/*
                                                 
                                                  
                                   
   */
		rx->skb = skb;

		CALL_RXH(ieee80211_rx_h_decrypt)
		CALL_RXH(ieee80211_rx_h_check_more_data)
		CALL_RXH(ieee80211_rx_h_uapsd_and_pspoll)
		CALL_RXH(ieee80211_rx_h_sta_process)
		CALL_RXH(ieee80211_rx_h_defragment)
		CALL_RXH(ieee80211_rx_h_michael_mic_verify)
		/*                                                            */
#ifdef CONFIG_MAC80211_MESH
		if (ieee80211_vif_is_mesh(&rx->sdata->vif))
			CALL_RXH(ieee80211_rx_h_mesh_fwding);
#endif
		CALL_RXH(ieee80211_rx_h_amsdu)
		CALL_RXH(ieee80211_rx_h_data)

		/*                                      */
		res = ieee80211_rx_h_ctrl(rx, frames);
		if (res != RX_CONTINUE)
			goto rxh_next;

		CALL_RXH(ieee80211_rx_h_mgmt_check)
		CALL_RXH(ieee80211_rx_h_action)
		CALL_RXH(ieee80211_rx_h_userspace_mgmt)
		CALL_RXH(ieee80211_rx_h_action_return)
		CALL_RXH(ieee80211_rx_h_mgmt)

 rxh_next:
		ieee80211_rx_handlers_result(rx, res);

#undef CALL_RXH
	}

	spin_unlock_bh(&rx->local->rx_path_lock);
}

static void ieee80211_invoke_rx_handlers(struct ieee80211_rx_data *rx)
{
	struct sk_buff_head reorder_release;
	ieee80211_rx_result res = RX_DROP_MONITOR;

	__skb_queue_head_init(&reorder_release);

#define CALL_RXH(rxh)			\
	do {				\
		res = rxh(rx);		\
		if (res != RX_CONTINUE)	\
			goto rxh_next;  \
	} while (0);

	CALL_RXH(ieee80211_rx_h_check)

	ieee80211_rx_reorder_ampdu(rx, &reorder_release);

	ieee80211_rx_handlers(rx, &reorder_release);
	return;

 rxh_next:
	ieee80211_rx_handlers_result(rx, res);

#undef CALL_RXH
}

/*
                                                        
                                            
 */
void ieee80211_release_reorder_timeout(struct sta_info *sta, int tid)
{
	struct sk_buff_head frames;
	struct ieee80211_rx_data rx = {
		.sta = sta,
		.sdata = sta->sdata,
		.local = sta->local,
		/*                                      */
		.security_idx = tid,
		.seqno_idx = tid,
		.flags = 0,
	};
	struct tid_ampdu_rx *tid_agg_rx;

	tid_agg_rx = rcu_dereference(sta->ampdu_mlme.tid_rx[tid]);
	if (!tid_agg_rx)
		return;

	__skb_queue_head_init(&frames);

	spin_lock(&tid_agg_rx->reorder_lock);
	ieee80211_sta_reorder_release(sta->sdata, tid_agg_rx, &frames);
	spin_unlock(&tid_agg_rx->reorder_lock);

	ieee80211_rx_handlers(&rx, &frames);
}

/*                   */

static int prepare_for_handlers(struct ieee80211_rx_data *rx,
				struct ieee80211_hdr *hdr)
{
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct sk_buff *skb = rx->skb;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	u8 *bssid = ieee80211_get_bssid(hdr, skb->len, sdata->vif.type);
	int multicast = is_multicast_ether_addr(hdr->addr1);

	switch (sdata->vif.type) {
	case NL80211_IFTYPE_STATION:
		if (!bssid && !sdata->u.mgd.use_4addr)
			return 0;
		if (!multicast &&
		    !ether_addr_equal(sdata->vif.addr, hdr->addr1)) {
			if (!(sdata->dev->flags & IFF_PROMISC) ||
			    sdata->u.mgd.use_4addr)
				return 0;
			status->rx_flags &= ~IEEE80211_RX_RA_MATCH;
		}
		break;
	case NL80211_IFTYPE_ADHOC:
		if (!bssid)
			return 0;
		if (ether_addr_equal(sdata->vif.addr, hdr->addr2) ||
		    ether_addr_equal(sdata->u.ibss.bssid, hdr->addr2))
			return 0;
		if (ieee80211_is_beacon(hdr->frame_control)) {
			return 1;
		} else if (!ieee80211_bssid_match(bssid, sdata->u.ibss.bssid)) {
			return 0;
		} else if (!multicast &&
			   !ether_addr_equal(sdata->vif.addr, hdr->addr1)) {
			if (!(sdata->dev->flags & IFF_PROMISC))
				return 0;
			status->rx_flags &= ~IEEE80211_RX_RA_MATCH;
		} else if (!rx->sta) {
			int rate_idx;
			if (status->flag & (RX_FLAG_HT | RX_FLAG_VHT))
				rate_idx = 0; /*                    */
			else
				rate_idx = status->rate_idx;
			ieee80211_ibss_rx_no_sta(sdata, bssid, hdr->addr2,
						 BIT(rate_idx));
		}
		break;
	case NL80211_IFTYPE_MESH_POINT:
		if (!multicast &&
		    !ether_addr_equal(sdata->vif.addr, hdr->addr1)) {
			if (!(sdata->dev->flags & IFF_PROMISC))
				return 0;

			status->rx_flags &= ~IEEE80211_RX_RA_MATCH;
		}
		break;
	case NL80211_IFTYPE_AP_VLAN:
	case NL80211_IFTYPE_AP:
		if (!bssid) {
			if (!ether_addr_equal(sdata->vif.addr, hdr->addr1))
				return 0;
		} else if (!ieee80211_bssid_match(bssid, sdata->vif.addr)) {
			/*
                                               
                                               
                                              
                                         
    */
			if (!multicast &&
			    !ether_addr_equal(sdata->vif.addr, hdr->addr1))
				return 0;
			if (ieee80211_is_public_action(hdr, skb->len))
				return 1;
			if (!ieee80211_is_beacon(hdr->frame_control))
				return 0;
			status->rx_flags &= ~IEEE80211_RX_RA_MATCH;
		}
		break;
	case NL80211_IFTYPE_WDS:
		if (bssid || !ieee80211_is_data(hdr->frame_control))
			return 0;
		if (!ether_addr_equal(sdata->u.wds.remote_addr, hdr->addr2))
			return 0;
		break;
	case NL80211_IFTYPE_P2P_DEVICE:
		if (!ieee80211_is_public_action(hdr, skb->len) &&
		    !ieee80211_is_probe_req(hdr->frame_control) &&
		    !ieee80211_is_probe_resp(hdr->frame_control) &&
		    !ieee80211_is_beacon(hdr->frame_control))
			return 0;
		if (!ether_addr_equal(sdata->vif.addr, hdr->addr1) &&
		    !multicast)
			status->rx_flags &= ~IEEE80211_RX_RA_MATCH;
		break;
	default:
		/*                       */
		WARN_ON_ONCE(1);
		break;
	}

	return 1;
}

/*
                                               
                                                
                                               
                               
 */
static bool ieee80211_prepare_and_rx_handle(struct ieee80211_rx_data *rx,
					    struct sk_buff *skb, bool consume)
{
	struct ieee80211_local *local = rx->local;
	struct ieee80211_sub_if_data *sdata = rx->sdata;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *hdr = (void *)skb->data;
	int prepares;

	rx->skb = skb;
	status->rx_flags |= IEEE80211_RX_RA_MATCH;
	prepares = prepare_for_handlers(rx, hdr);

	if (!prepares)
		return false;

	if (!consume) {
		skb = skb_copy(skb, GFP_ATOMIC);
		if (!skb) {
			if (net_ratelimit())
				wiphy_debug(local->hw.wiphy,
					"failed to copy skb for %s\n",
					sdata->name);
			return true;
		}

		rx->skb = skb;
	}

	ieee80211_invoke_rx_handlers(rx);
	return true;
}

/*
                                                                        
                                           
 */
static void __ieee80211_rx_handle_packet(struct ieee80211_hw *hw,
					 struct sk_buff *skb)
{
	struct ieee80211_local *local = hw_to_local(hw);
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_hdr *hdr;
	__le16 fc;
	struct ieee80211_rx_data rx;
	struct ieee80211_sub_if_data *prev;
	struct sta_info *sta, *tmp, *prev_sta;
	int err = 0;

	fc = ((struct ieee80211_hdr *)skb->data)->frame_control;
	memset(&rx, 0, sizeof(rx));
	rx.skb = skb;
	rx.local = local;

	if (ieee80211_is_data(fc) || ieee80211_is_mgmt(fc))
		local->dot11ReceivedFragmentCount++;

	if (ieee80211_is_mgmt(fc)) {
		/*                                    */
		if (skb->len < ieee80211_hdrlen(fc))
			err = -ENOBUFS;
		else
			err = skb_linearize(skb);
	} else {
		err = !pskb_may_pull(skb, ieee80211_hdrlen(fc));
	}

	if (err) {
		dev_kfree_skb(skb);
		return;
	}

	hdr = (struct ieee80211_hdr *)skb->data;
	ieee80211_parse_qos(&rx);
	ieee80211_verify_alignment(&rx);

	if (unlikely(ieee80211_is_probe_resp(hdr->frame_control) ||
		     ieee80211_is_beacon(hdr->frame_control)))
		ieee80211_scan_rx(local, skb);

	if (ieee80211_is_data(fc)) {
		prev_sta = NULL;

		for_each_sta_info(local, hdr->addr2, sta, tmp) {
			if (!prev_sta) {
				prev_sta = sta;
				continue;
			}

			rx.sta = prev_sta;
			rx.sdata = prev_sta->sdata;
			ieee80211_prepare_and_rx_handle(&rx, skb, false);

			prev_sta = sta;
		}

		if (prev_sta) {
			rx.sta = prev_sta;
			rx.sdata = prev_sta->sdata;

			if (ieee80211_prepare_and_rx_handle(&rx, skb, true))
				return;
			goto out;
		}
	}

	prev = NULL;

	list_for_each_entry_rcu(sdata, &local->interfaces, list) {
		if (!ieee80211_sdata_running(sdata))
			continue;

		if (sdata->vif.type == NL80211_IFTYPE_MONITOR ||
		    sdata->vif.type == NL80211_IFTYPE_AP_VLAN)
			continue;

		/*
                                                      
                                                       
                                                    
   */

		if (!prev) {
			prev = sdata;
			continue;
		}

		rx.sta = sta_info_get_bss(prev, hdr->addr2);
		rx.sdata = prev;
		ieee80211_prepare_and_rx_handle(&rx, skb, false);

		prev = sdata;
	}

	if (prev) {
		rx.sta = sta_info_get_bss(prev, hdr->addr2);
		rx.sdata = prev;

		if (ieee80211_prepare_and_rx_handle(&rx, skb, true))
			return;
	}

 out:
	dev_kfree_skb(skb);
}

/*
                                                                               
                                             
 */
void ieee80211_rx(struct ieee80211_hw *hw, struct sk_buff *skb)
{
	struct ieee80211_local *local = hw_to_local(hw);
	struct ieee80211_rate *rate = NULL;
	struct ieee80211_supported_band *sband;
	struct ieee80211_rx_status *status = IEEE80211_SKB_RXCB(skb);

	WARN_ON_ONCE(softirq_count() == 0);

	if (WARN_ON(status->band >= IEEE80211_NUM_BANDS))
		goto drop;

	sband = local->hw.wiphy->bands[status->band];
	if (WARN_ON(!sband))
		goto drop;

	/*
                                                               
                                                                
                                                               
                                                                
                                
  */
	if (unlikely(local->quiescing || local->suspended))
		goto drop;

	/*                                                                  */
	if (unlikely(local->in_reconfig))
		goto drop;

	/*
                                                 
                               
  */
	if (WARN_ON(!local->started))
		goto drop;

	if (likely(!(status->flag & RX_FLAG_FAILED_PLCP_CRC))) {
		/*
                                                      
                                                     
   */

		if (status->flag & RX_FLAG_HT) {
			/*
                                                
                       
     
                                                                              
     
                                                   
                                                      
             
    */
			if (WARN(status->rate_idx > 76,
				 "Rate marked as an HT rate but passed "
				 "status->rate_idx is not "
				 "an MCS index [0-76]: %d (0x%02x)\n",
				 status->rate_idx,
				 status->rate_idx))
				goto drop;
		} else if (status->flag & RX_FLAG_VHT) {
			if (WARN_ONCE(status->rate_idx > 9 ||
				      !status->vht_nss ||
				      status->vht_nss > 8,
				      "Rate marked as a VHT rate but data is invalid: MCS: %d, NSS: %d\n",
				      status->rate_idx, status->vht_nss))
				goto drop;
		} else {
			if (WARN_ON(status->rate_idx >= sband->n_bitrates))
				goto drop;
			rate = &sband->bitrates[status->rate_idx];
		}
	}

	status->rx_flags = 0;

	/*
                                                                 
                                                                   
                      
  */
	rcu_read_lock();

	/*
                                                          
                                                         
                                 
                                                     
  */
	skb = ieee80211_rx_monitor(local, skb, rate);
	if (!skb) {
		rcu_read_unlock();
		return;
	}

	ieee80211_tpt_led_trig_rx(local,
			((struct ieee80211_hdr *)skb->data)->frame_control,
			skb->len);
	__ieee80211_rx_handle_packet(hw, skb);

	rcu_read_unlock();

	return;
 drop:
	kfree_skb(skb);
}
EXPORT_SYMBOL(ieee80211_rx);

/*                                                                     
                                                               */
void ieee80211_rx_irqsafe(struct ieee80211_hw *hw, struct sk_buff *skb)
{
	struct ieee80211_local *local = hw_to_local(hw);

	BUILD_BUG_ON(sizeof(struct ieee80211_rx_status) > sizeof(skb->cb));

	skb->pkt_type = IEEE80211_RX_MSG;
	skb_queue_tail(&local->skb_queue, skb);
	tasklet_schedule(&local->tasklet);
}
EXPORT_SYMBOL(ieee80211_rx_irqsafe);