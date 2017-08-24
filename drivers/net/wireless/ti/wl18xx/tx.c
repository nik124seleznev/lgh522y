/*
 * This file is part of wl18xx
 *
 * Copyright (C) 2011 Texas Instruments Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "../wlcore/wlcore.h"
#include "../wlcore/cmd.h"
#include "../wlcore/debug.h"
#include "../wlcore/acx.h"
#include "../wlcore/tx.h"

#include "wl18xx.h"
#include "tx.h"

static
void wl18xx_get_last_tx_rate(struct wl1271 *wl, struct ieee80211_vif *vif,
			     struct ieee80211_tx_rate *rate)
{
	u8 fw_rate = wl->fw_status_2->counters.tx_last_rate;

	if (fw_rate > CONF_HW_RATE_INDEX_MAX) {
		wl1271_error("last Tx rate invalid: %d", fw_rate);
		rate->idx = 0;
		rate->flags = 0;
		return;
	}

	if (fw_rate <= CONF_HW_RATE_INDEX_54MBPS) {
		rate->idx = fw_rate;
		rate->flags = 0;
	} else {
		rate->flags = IEEE80211_TX_RC_MCS;
		rate->idx = fw_rate - CONF_HW_RATE_INDEX_MCS0;

		/*                                            */
		if (fw_rate >= CONF_HW_RATE_INDEX_MCS7_SGI)
			(rate->idx)--;
		if (fw_rate == CONF_HW_RATE_INDEX_MCS15_SGI)
			(rate->idx)--;

		/*                                               */
		if (fw_rate == CONF_HW_RATE_INDEX_MCS7_SGI ||
		    fw_rate == CONF_HW_RATE_INDEX_MCS15_SGI)
			rate->flags |= IEEE80211_TX_RC_SHORT_GI;

		if (fw_rate > CONF_HW_RATE_INDEX_MCS7_SGI && vif) {
			struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
			if (wlvif->channel_type == NL80211_CHAN_HT40MINUS ||
			    wlvif->channel_type == NL80211_CHAN_HT40PLUS) {
				/*                                 */
				rate->idx -= 8;
				rate->flags |= IEEE80211_TX_RC_40_MHZ_WIDTH;
			}
		}
	}
}

static void wl18xx_tx_complete_packet(struct wl1271 *wl, u8 tx_stat_byte)
{
	struct ieee80211_tx_info *info;
	struct sk_buff *skb;
	int id = tx_stat_byte & WL18XX_TX_STATUS_DESC_ID_MASK;
	bool tx_success;

	/*                       */
	if (unlikely(id >= wl->num_tx_desc || wl->tx_frames[id] == NULL)) {
		wl1271_warning("illegal id in tx completion: %d", id);
		return;
	}

	/*                                 */
	tx_success = !(tx_stat_byte & BIT(WL18XX_TX_STATUS_STAT_BIT_IDX));

	skb = wl->tx_frames[id];
	info = IEEE80211_SKB_CB(skb);

	if (wl12xx_is_dummy_packet(wl, skb)) {
		wl1271_free_tx_id(wl, id);
		return;
	}

	/*                           */
	if (tx_success && !(info->flags & IEEE80211_TX_CTL_NO_ACK))
		info->flags |= IEEE80211_TX_STAT_ACK;
	/*
                                                                    
                               
  */
	wl18xx_get_last_tx_rate(wl, info->control.vif, &info->status.rates[0]);

	info->status.rates[0].count = 1; /*                       */
	info->status.ack_signal = -1;

	if (!tx_success)
		wl->stats.retry_count++;

	/*
                                                            
                                                             
  */

	/*                                   */
	skb_pull(skb, sizeof(struct wl1271_tx_hw_descr));

	/*                                     */
	if ((wl->quirks & WLCORE_QUIRK_TKIP_HEADER_SPACE) &&
	    info->control.hw_key &&
	    info->control.hw_key->cipher == WLAN_CIPHER_SUITE_TKIP) {
		int hdrlen = ieee80211_get_hdrlen_from_skb(skb);
		memmove(skb->data + WL1271_EXTRA_SPACE_TKIP, skb->data, hdrlen);
		skb_pull(skb, WL1271_EXTRA_SPACE_TKIP);
	}

	wl1271_debug(DEBUG_TX, "tx status id %u skb 0x%p success %d",
		     id, skb, tx_success);

	/*                                */
	skb_queue_tail(&wl->deferred_tx_queue, skb);
	queue_work(wl->freezable_wq, &wl->netstack_work);
	wl1271_free_tx_id(wl, id);
}

void wl18xx_tx_immediate_complete(struct wl1271 *wl)
{
	struct wl18xx_fw_status_priv *status_priv =
		(struct wl18xx_fw_status_priv *)wl->fw_status_2->priv;
	struct wl18xx_priv *priv = wl->priv;
	u8 i;

	/*                    */
	if (priv->last_fw_rls_idx == status_priv->fw_release_idx)
		return;

	/*                      */
	wl1271_debug(DEBUG_TX, "last released desc = %d, current idx = %d",
		     priv->last_fw_rls_idx, status_priv->fw_release_idx);

	if (status_priv->fw_release_idx >= WL18XX_FW_MAX_TX_STATUS_DESC) {
		wl1271_error("invalid desc release index %d",
			     status_priv->fw_release_idx);
		WARN_ON(1);
		return;
	}

	for (i = priv->last_fw_rls_idx;
	     i != status_priv->fw_release_idx;
	     i = (i + 1) % WL18XX_FW_MAX_TX_STATUS_DESC) {
		wl18xx_tx_complete_packet(wl,
			status_priv->released_tx_desc[i]);

		wl->tx_results_count++;
	}

	priv->last_fw_rls_idx = status_priv->fw_release_idx;
}
