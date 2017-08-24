
/*
 * This file is part of wl1271
 *
 * Copyright (C) 2008-2010 Nokia Corporation
 *
 * Contact: Luciano Coelho <luciano.coelho@nokia.com>
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

#include <linux/module.h>
#include <linux/firmware.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/crc32.h>
#include <linux/etherdevice.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/wl12xx.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

#include "wlcore.h"
#include "debug.h"
#include "wl12xx_80211.h"
#include "io.h"
#include "event.h"
#include "tx.h"
#include "rx.h"
#include "ps.h"
#include "init.h"
#include "debugfs.h"
#include "cmd.h"
#include "boot.h"
#include "testmode.h"
#include "scan.h"
#include "hw_ops.h"

#define WL1271_BOOT_RETRIES 3

#define WL1271_BOOT_RETRIES 3

static char *fwlog_param;
static int bug_on_recovery = -1;
static int no_recovery     = -1;

static void __wl1271_op_remove_interface(struct wl1271 *wl,
					 struct ieee80211_vif *vif,
					 bool reset_tx_queues);
static void wlcore_op_stop_locked(struct wl1271 *wl);
static void wl1271_free_ap_keys(struct wl1271 *wl, struct wl12xx_vif *wlvif);

static int wl12xx_set_authorized(struct wl1271 *wl,
				 struct wl12xx_vif *wlvif)
{
	int ret;

	if (WARN_ON(wlvif->bss_type != BSS_TYPE_STA_BSS))
		return -EINVAL;

	if (!test_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags))
		return 0;

	if (test_and_set_bit(WLVIF_FLAG_STA_STATE_SENT, &wlvif->flags))
		return 0;

	ret = wl12xx_cmd_set_peer_state(wl, wlvif, wlvif->sta.hlid);
	if (ret < 0)
		return ret;

	wl1271_info("Association completed.");
	return 0;
}

static void wl1271_reg_notify(struct wiphy *wiphy,
			      struct regulatory_request *request)
{
	struct ieee80211_supported_band *band;
	struct ieee80211_channel *ch;
	int i;
	struct ieee80211_hw *hw = wiphy_to_ieee80211_hw(wiphy);
	struct wl1271 *wl = hw->priv;

	band = wiphy->bands[IEEE80211_BAND_5GHZ];
	for (i = 0; i < band->n_channels; i++) {
		ch = &band->channels[i];
		if (ch->flags & IEEE80211_CHAN_DISABLED)
			continue;

		if (ch->flags & IEEE80211_CHAN_RADAR)
			ch->flags |= IEEE80211_CHAN_NO_IBSS |
				     IEEE80211_CHAN_PASSIVE_SCAN;

	}

	wlcore_regdomain_config(wl);
}

static int wl1271_set_rx_streaming(struct wl1271 *wl, struct wl12xx_vif *wlvif,
				   bool enable)
{
	int ret = 0;

	/*                          */
	ret = wl1271_acx_ps_rx_streaming(wl, wlvif, enable);
	if (ret < 0)
		goto out;

	if (enable)
		set_bit(WLVIF_FLAG_RX_STREAMING_STARTED, &wlvif->flags);
	else
		clear_bit(WLVIF_FLAG_RX_STREAMING_STARTED, &wlvif->flags);
out:
	return ret;
}

/*
                                                               
                                                      
 */
int wl1271_recalc_rx_streaming(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	int ret = 0;
	int period = wl->conf.rx_streaming.interval;

	/*                                               */
	if (!test_bit(WLVIF_FLAG_RX_STREAMING_STARTED, &wlvif->flags))
		goto out;

	/*                                                       */
	if (period &&
	    test_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags) &&
	    (wl->conf.rx_streaming.always ||
	     test_bit(WL1271_FLAG_SOFT_GEMINI, &wl->flags)))
		ret = wl1271_set_rx_streaming(wl, wlvif, true);
	else {
		ret = wl1271_set_rx_streaming(wl, wlvif, false);
		/*                                                */
		del_timer_sync(&wlvif->rx_streaming_timer);
	}
out:
	return ret;
}

static void wl1271_rx_streaming_enable_work(struct work_struct *work)
{
	int ret;
	struct wl12xx_vif *wlvif = container_of(work, struct wl12xx_vif,
						rx_streaming_enable_work);
	struct wl1271 *wl = wlvif->wl;

	mutex_lock(&wl->mutex);

	if (test_bit(WLVIF_FLAG_RX_STREAMING_STARTED, &wlvif->flags) ||
	    !test_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags) ||
	    (!wl->conf.rx_streaming.always &&
	     !test_bit(WL1271_FLAG_SOFT_GEMINI, &wl->flags)))
		goto out;

	if (!wl->conf.rx_streaming.interval)
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wl1271_set_rx_streaming(wl, wlvif, true);
	if (ret < 0)
		goto out_sleep;

	/*                                       */
	mod_timer(&wlvif->rx_streaming_timer,
		  jiffies + msecs_to_jiffies(wl->conf.rx_streaming.duration));

out_sleep:
	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);
}

static void wl1271_rx_streaming_disable_work(struct work_struct *work)
{
	int ret;
	struct wl12xx_vif *wlvif = container_of(work, struct wl12xx_vif,
						rx_streaming_disable_work);
	struct wl1271 *wl = wlvif->wl;

	mutex_lock(&wl->mutex);

	if (!test_bit(WLVIF_FLAG_RX_STREAMING_STARTED, &wlvif->flags))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wl1271_set_rx_streaming(wl, wlvif, false);
	if (ret)
		goto out_sleep;

out_sleep:
	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);
}

static void wl1271_rx_streaming_timer(unsigned long data)
{
	struct wl12xx_vif *wlvif = (struct wl12xx_vif *)data;
	struct wl1271 *wl = wlvif->wl;
	ieee80211_queue_work(wl->hw, &wlvif->rx_streaming_disable_work);
}

/*                         */
void wl12xx_rearm_tx_watchdog_locked(struct wl1271 *wl)
{
	/*                                                 */
	if (wl->tx_allocated_blocks == 0)
		return;

	cancel_delayed_work(&wl->tx_watchdog_work);
	ieee80211_queue_delayed_work(wl->hw, &wl->tx_watchdog_work,
		msecs_to_jiffies(wl->conf.tx.tx_watchdog_timeout));
}

static void wl12xx_tx_watchdog_work(struct work_struct *work)
{
	struct delayed_work *dwork;
	struct wl1271 *wl;

	dwork = container_of(work, struct delayed_work, work);
	wl = container_of(dwork, struct wl1271, tx_watchdog_work);

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	/*                                                */
	if (unlikely(wl->tx_allocated_blocks == 0))
		goto out;

	/*
                                                                
                                                  
  */
	if (find_first_bit(wl->roc_map, WL12XX_MAX_ROLES) < WL12XX_MAX_ROLES) {
		wl1271_debug(DEBUG_TX, "No Tx (in FW) for %d ms due to ROC",
			     wl->conf.tx.tx_watchdog_timeout);
		wl12xx_rearm_tx_watchdog_locked(wl);
		goto out;
	}

	/*
                                                                 
        
  */
	if (wl->scan.state != WL1271_SCAN_STATE_IDLE) {
		wl1271_debug(DEBUG_TX, "No Tx (in FW) for %d ms due to scan",
			     wl->conf.tx.tx_watchdog_timeout);
		wl12xx_rearm_tx_watchdog_locked(wl);
		goto out;
	}

	/*
                                                                 
                                                                  
                                                                    
                                          
 */
	if (wl->active_sta_count) {
		wl1271_debug(DEBUG_TX, "No Tx (in FW) for %d ms. AP has "
			     " %d stations",
			      wl->conf.tx.tx_watchdog_timeout,
			      wl->active_sta_count);
		wl12xx_rearm_tx_watchdog_locked(wl);
		goto out;
	}

	wl1271_error("Tx stuck (in FW) for %d ms. Starting recovery",
		     wl->conf.tx.tx_watchdog_timeout);
	wl12xx_queue_recovery_work(wl);

out:
	mutex_unlock(&wl->mutex);
}

static void wlcore_adjust_conf(struct wl1271 *wl)
{
	/*                                                         */

	if (fwlog_param) {
		if (!strcmp(fwlog_param, "continuous")) {
			wl->conf.fwlog.mode = WL12XX_FWLOG_CONTINUOUS;
		} else if (!strcmp(fwlog_param, "ondemand")) {
			wl->conf.fwlog.mode = WL12XX_FWLOG_ON_DEMAND;
		} else if (!strcmp(fwlog_param, "dbgpins")) {
			wl->conf.fwlog.mode = WL12XX_FWLOG_CONTINUOUS;
			wl->conf.fwlog.output = WL12XX_FWLOG_OUTPUT_DBG_PINS;
		} else if (!strcmp(fwlog_param, "disable")) {
			wl->conf.fwlog.mem_blocks = 0;
			wl->conf.fwlog.output = WL12XX_FWLOG_OUTPUT_NONE;
		} else {
			wl1271_error("Unknown fwlog parameter %s", fwlog_param);
		}
	}

	if (bug_on_recovery != -1)
		wl->conf.recovery.bug_on_recovery = (u8) bug_on_recovery;

	if (no_recovery != -1)
		wl->conf.recovery.no_recovery = (u8) no_recovery;
}

static void wl12xx_irq_ps_regulate_link(struct wl1271 *wl,
					struct wl12xx_vif *wlvif,
					u8 hlid, u8 tx_pkts)
{
	bool fw_ps;

	fw_ps = test_bit(hlid, (unsigned long *)&wl->ap_fw_ps_map);

	/*
                                                                   
                                         
  */
	if (!fw_ps || tx_pkts < WL1271_PS_STA_MAX_PACKETS)
		wl12xx_ps_link_end(wl, wlvif, hlid);

	/*
                                                                      
                                                                 
                                                   
                                                                        
                                                                    
                                                                     
                                     
  */
	else if (wl->active_link_count > 3 && fw_ps &&
		 tx_pkts >= WL1271_PS_STA_MAX_PACKETS)
		wl12xx_ps_link_start(wl, wlvif, hlid, true);
}

static void wl12xx_irq_update_links_status(struct wl1271 *wl,
					   struct wl12xx_vif *wlvif,
					   struct wl_fw_status_2 *status)
{
	u32 cur_fw_ps_map;
	u8 hlid;

	cur_fw_ps_map = le32_to_cpu(status->link_ps_bitmap);
	if (wl->ap_fw_ps_map != cur_fw_ps_map) {
		wl1271_debug(DEBUG_PSM,
			     "link ps prev 0x%x cur 0x%x changed 0x%x",
			     wl->ap_fw_ps_map, cur_fw_ps_map,
			     wl->ap_fw_ps_map ^ cur_fw_ps_map);

		wl->ap_fw_ps_map = cur_fw_ps_map;
	}

	for_each_set_bit(hlid, wlvif->ap.sta_hlid_map, WL12XX_MAX_LINKS)
		wl12xx_irq_ps_regulate_link(wl, wlvif, hlid,
					    wl->links[hlid].allocated_pkts);
}

static int wlcore_fw_status(struct wl1271 *wl,
			    struct wl_fw_status_1 *status_1,
			    struct wl_fw_status_2 *status_2)
{
	struct wl12xx_vif *wlvif;
	struct timespec ts;
	u32 old_tx_blk_count = wl->tx_blocks_available;
	int avail, freed_blocks;
	int i;
	size_t status_len;
	int ret;
	struct wl1271_link *lnk;

	status_len = WLCORE_FW_STATUS_1_LEN(wl->num_rx_desc) +
		sizeof(*status_2) + wl->fw_status_priv_len;

	ret = wlcore_raw_read_data(wl, REG_RAW_FW_STATUS_ADDR, status_1,
				   status_len, false);
	if (ret < 0)
		return ret;

	wl1271_debug(DEBUG_IRQ, "intr: 0x%x (fw_rx_counter = %d, "
		     "drv_rx_counter = %d, tx_results_counter = %d)",
		     status_1->intr,
		     status_1->fw_rx_counter,
		     status_1->drv_rx_counter,
		     status_1->tx_results_counter);

	for (i = 0; i < NUM_TX_QUEUES; i++) {
		/*                                              */
		wl->tx_allocated_pkts[i] -=
				(status_2->counters.tx_released_pkts[i] -
				wl->tx_pkts_freed[i]) & 0xff;

		wl->tx_pkts_freed[i] = status_2->counters.tx_released_pkts[i];
	}


	for_each_set_bit(i, wl->links_map, WL12XX_MAX_LINKS) {
		u8 diff;
		lnk = &wl->links[i];

		/*                                              */
		diff = (status_2->counters.tx_lnk_free_pkts[i] -
		       lnk->prev_freed_pkts) & 0xff;

		if (diff == 0)
			continue;

		lnk->allocated_pkts -= diff;
		lnk->prev_freed_pkts = status_2->counters.tx_lnk_free_pkts[i];

		/*                                        */
		lnk->total_freed_pkts += diff;
	}

	/*                                             */
	if (likely(wl->tx_blocks_freed <=
		   le32_to_cpu(status_2->total_released_blks)))
		freed_blocks = le32_to_cpu(status_2->total_released_blks) -
			       wl->tx_blocks_freed;
	else
		freed_blocks = 0x100000000LL - wl->tx_blocks_freed +
			       le32_to_cpu(status_2->total_released_blks);

	wl->tx_blocks_freed = le32_to_cpu(status_2->total_released_blks);

	wl->tx_allocated_blocks -= freed_blocks;

	/*
                                
                                                               
                                                             
  */
	if (freed_blocks) {
		if (wl->tx_allocated_blocks)
			wl12xx_rearm_tx_watchdog_locked(wl);
		else
			cancel_delayed_work(&wl->tx_watchdog_work);
	}

	avail = le32_to_cpu(status_2->tx_total) - wl->tx_allocated_blocks;

	/*
                                                               
                                                                
                                                               
                                                               
                                                               
                                                             
  */
	wl->tx_blocks_available = max((int)wl->tx_blocks_available,
				      avail);

	/*                                                            */
	if (wl->tx_blocks_available > old_tx_blk_count)
		clear_bit(WL1271_FLAG_FW_TX_BUSY, &wl->flags);

	/*                                                                 */
	wl12xx_for_each_wlvif_ap(wl, wlvif) {
		wl12xx_irq_update_links_status(wl, wlvif, status_2);
	}

	/*                                     */
	getnstimeofday(&ts);
	wl->time_offset = (timespec_to_ns(&ts) >> 10) -
		(s64)le32_to_cpu(status_2->fw_localtime);

	wl->fw_fast_lnk_map = le32_to_cpu(status_2->link_fast_bitmap);

	return 0;
}

static void wl1271_flush_deferred_work(struct wl1271 *wl)
{
	struct sk_buff *skb;

	/*                                               */
	while ((skb = skb_dequeue(&wl->deferred_rx_queue)))
		ieee80211_rx_ni(wl->hw, skb);

	/*                                       */
	while ((skb = skb_dequeue(&wl->deferred_tx_queue)))
		ieee80211_tx_status_ni(wl->hw, skb);
}

static void wl1271_netstack_work(struct work_struct *work)
{
	struct wl1271 *wl =
		container_of(work, struct wl1271, netstack_work);

	do {
		wl1271_flush_deferred_work(wl);
	} while (skb_queue_len(&wl->deferred_rx_queue));
}

#define WL1271_IRQ_MAX_LOOPS 256

static int wlcore_irq_locked(struct wl1271 *wl)
{
	int ret = 0;
	u32 intr;
	int loopcount = WL1271_IRQ_MAX_LOOPS;
	bool done = false;
	unsigned int defer_count;
	unsigned long flags;

	/*
                                                                    
                                                                        
  */
	if (wl->platform_quirks & WL12XX_PLATFORM_QUIRK_EDGE_IRQ)
		loopcount = 1;

	wl1271_debug(DEBUG_IRQ, "IRQ work");

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	while (!done && loopcount--) {
		/*
                                                              
                                                            
                                                        
   */
		clear_bit(WL1271_FLAG_IRQ_RUNNING, &wl->flags);
		smp_mb__after_clear_bit();

		ret = wlcore_fw_status(wl, wl->fw_status_1, wl->fw_status_2);
		if (ret < 0)
			goto out;

		wlcore_hw_tx_immediate_compl(wl);

		intr = le32_to_cpu(wl->fw_status_1->intr);
		intr &= WLCORE_ALL_INTR_MASK;
		if (!intr) {
			done = true;
			continue;
		}

		if (unlikely(intr & WL1271_ACX_INTR_WATCHDOG)) {
			wl1271_error("HW watchdog interrupt received! starting recovery.");
			wl->watchdog_recovery = true;
			ret = -EIO;

			/*                                                  */
			goto out;
		}

		if (unlikely(intr & WL1271_ACX_SW_INTR_WATCHDOG)) {
			wl1271_error("SW watchdog interrupt received! "
				     "starting recovery.");
			wl->watchdog_recovery = true;
			ret = -EIO;

			/*                                                  */
			goto out;
		}

		if (likely(intr & WL1271_ACX_INTR_DATA)) {
			wl1271_debug(DEBUG_IRQ, "WL1271_ACX_INTR_DATA");

			ret = wlcore_rx(wl, wl->fw_status_1);
			if (ret < 0)
				goto out;

			/*                                   */
			spin_lock_irqsave(&wl->wl_lock, flags);
			if (!test_bit(WL1271_FLAG_FW_TX_BUSY, &wl->flags) &&
			    wl1271_tx_total_queue_count(wl) > 0) {
				spin_unlock_irqrestore(&wl->wl_lock, flags);
				/*
                                                   
                                       
     */
				ret = wlcore_tx_work_locked(wl);
				if (ret < 0)
					goto out;
			} else {
				spin_unlock_irqrestore(&wl->wl_lock, flags);
			}

			/*                      */
			ret = wlcore_hw_tx_delayed_compl(wl);
			if (ret < 0)
				goto out;

			/*                                                  */
			defer_count = skb_queue_len(&wl->deferred_tx_queue) +
				      skb_queue_len(&wl->deferred_rx_queue);
			if (defer_count > WL1271_DEFERRED_QUEUE_LIMIT)
				wl1271_flush_deferred_work(wl);
		}

		if (intr & WL1271_ACX_INTR_EVENT_A) {
			wl1271_debug(DEBUG_IRQ, "WL1271_ACX_INTR_EVENT_A");
			ret = wl1271_event_handle(wl, 0);
			if (ret < 0)
				goto out;
		}

		if (intr & WL1271_ACX_INTR_EVENT_B) {
			wl1271_debug(DEBUG_IRQ, "WL1271_ACX_INTR_EVENT_B");
			ret = wl1271_event_handle(wl, 1);
			if (ret < 0)
				goto out;
		}

		if (intr & WL1271_ACX_INTR_INIT_COMPLETE)
			wl1271_debug(DEBUG_IRQ,
				     "WL1271_ACX_INTR_INIT_COMPLETE");

		if (intr & WL1271_ACX_INTR_HW_AVAILABLE)
			wl1271_debug(DEBUG_IRQ, "WL1271_ACX_INTR_HW_AVAILABLE");
	}

	wl1271_ps_elp_sleep(wl);

out:
	return ret;
}

static irqreturn_t wlcore_irq(int irq, void *cookie)
{
	int ret;
	unsigned long flags;
	struct wl1271 *wl = cookie;

	/*                             */
	spin_lock_irqsave(&wl->wl_lock, flags);
	set_bit(WL1271_FLAG_IRQ_RUNNING, &wl->flags);
	if (wl->elp_compl) {
		complete(wl->elp_compl);
		wl->elp_compl = NULL;
	}

	if (test_bit(WL1271_FLAG_SUSPENDED, &wl->flags)) {
		/*                                                    */
		set_bit(WL1271_FLAG_PENDING_WORK, &wl->flags);
		wl1271_debug(DEBUG_IRQ, "should not enqueue work");
		disable_irq_nosync(wl->irq);
		pm_wakeup_event(wl->dev, 0);
		spin_unlock_irqrestore(&wl->wl_lock, flags);
		return IRQ_HANDLED;
	}
	spin_unlock_irqrestore(&wl->wl_lock, flags);

	/*                                                */
	set_bit(WL1271_FLAG_TX_PENDING, &wl->flags);
	cancel_work_sync(&wl->tx_work);

	mutex_lock(&wl->mutex);

	ret = wlcore_irq_locked(wl);
	if (ret)
		wl12xx_queue_recovery_work(wl);

	spin_lock_irqsave(&wl->wl_lock, flags);
	/*                                                */
	clear_bit(WL1271_FLAG_TX_PENDING, &wl->flags);
	if (!test_bit(WL1271_FLAG_FW_TX_BUSY, &wl->flags) &&
	    wl1271_tx_total_queue_count(wl) > 0)
		ieee80211_queue_work(wl->hw, &wl->tx_work);
	spin_unlock_irqrestore(&wl->wl_lock, flags);

	mutex_unlock(&wl->mutex);

	return IRQ_HANDLED;
}

struct vif_counter_data {
	u8 counter;

	struct ieee80211_vif *cur_vif;
	bool cur_vif_running;
};

static void wl12xx_vif_count_iter(void *data, u8 *mac,
				  struct ieee80211_vif *vif)
{
	struct vif_counter_data *counter = data;

	counter->counter++;
	if (counter->cur_vif == vif)
		counter->cur_vif_running = true;
}

/*                                                      */
static void wl12xx_get_vif_count(struct ieee80211_hw *hw,
			       struct ieee80211_vif *cur_vif,
			       struct vif_counter_data *data)
{
	memset(data, 0, sizeof(*data));
	data->cur_vif = cur_vif;

	ieee80211_iterate_active_interfaces(hw, IEEE80211_IFACE_ITER_RESUME_ALL,
					    wl12xx_vif_count_iter, data);
}

static int wl12xx_fetch_firmware(struct wl1271 *wl, bool plt)
{
	const struct firmware *fw;
	const char *fw_name;
	enum wl12xx_fw_type fw_type;
	int ret;

	if (plt) {
		fw_type = WL12XX_FW_TYPE_PLT;
		fw_name = wl->plt_fw_name;
	} else {
		/*
                                                      
                                                               
   */
		if (wl->last_vif_count > 1 && wl->mr_fw_name) {
			fw_type = WL12XX_FW_TYPE_MULTI;
			fw_name = wl->mr_fw_name;
		} else {
			fw_type = WL12XX_FW_TYPE_NORMAL;
			fw_name = wl->sr_fw_name;
		}
	}

	if (wl->fw_type == fw_type)
		return 0;

	wl1271_debug(DEBUG_BOOT, "booting firmware %s", fw_name);

	ret = request_firmware(&fw, fw_name, wl->dev);

	if (ret < 0) {
		wl1271_error("could not get firmware %s: %d", fw_name, ret);
		return ret;
	}

	if (fw->size % 4) {
		wl1271_error("firmware size is not multiple of 32 bits: %zu",
			     fw->size);
		ret = -EILSEQ;
		goto out;
	}

	vfree(wl->fw);
	wl->fw_type = WL12XX_FW_TYPE_NONE;
	wl->fw_len = fw->size;
	wl->fw = vmalloc(wl->fw_len);

	if (!wl->fw) {
		wl1271_error("could not allocate memory for the firmware");
		ret = -ENOMEM;
		goto out;
	}

	memcpy(wl->fw, fw->data, wl->fw_len);
	ret = 0;
	wl->fw_type = fw_type;
out:
	release_firmware(fw);

	return ret;
}

void wl12xx_queue_recovery_work(struct wl1271 *wl)
{
	WARN_ON(!test_bit(WL1271_FLAG_INTENDED_FW_RECOVERY, &wl->flags));

	/*                            */
	if (wl->state == WLCORE_STATE_ON) {
		wl->state = WLCORE_STATE_RESTARTING;
		set_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS, &wl->flags);
		wlcore_disable_interrupts_nosync(wl);
		ieee80211_queue_work(wl->hw, &wl->recovery_work);
	}
}

size_t wl12xx_copy_fwlog(struct wl1271 *wl, u8 *memblock, size_t maxlen)
{
	size_t len = 0;

	/*                                                           */
	while (len < maxlen) {
		if (memblock[len] == 0)
			break;
		if (len + memblock[len] + 1 > maxlen)
			break;
		len += memblock[len] + 1;
	}

	/*                               */
	len = min(len, (size_t)(PAGE_SIZE - wl->fwlog_size));

	/*                                                         */
	memcpy(wl->fwlog + wl->fwlog_size, memblock, len);
	wl->fwlog_size += len;

	return len;
}

#define WLCORE_FW_LOG_END 0x2000000

static void wl12xx_read_fwlog_panic(struct wl1271 *wl)
{
	u32 addr;
	u32 offset;
	u32 end_of_log;
	u8 *block;
	int ret;

	if ((wl->quirks & WLCORE_QUIRK_FWLOG_NOT_IMPLEMENTED) ||
	    (wl->conf.fwlog.mem_blocks == 0))
		return;

	wl1271_info("Reading FW panic log");

	block = kmalloc(WL12XX_HW_BLOCK_SIZE, GFP_KERNEL);
	if (!block)
		return;

	/*
                                                            
                                                              
                                          
  */
	if (wl1271_ps_elp_wakeup(wl))
		goto out;
	if (!wl->watchdog_recovery &&
	    wl->conf.fwlog.output != WL12XX_FWLOG_OUTPUT_DBG_PINS)
		wl12xx_cmd_stop_fwlog(wl);

	/*                                     */
	ret = wlcore_fw_status(wl, wl->fw_status_1, wl->fw_status_2);
	if (ret < 0)
		goto out;

	addr = le32_to_cpu(wl->fw_status_2->log_start_addr);
	if (!addr)
		goto out;

	if (wl->conf.fwlog.mode == WL12XX_FWLOG_CONTINUOUS) {
		offset = sizeof(addr) + sizeof(struct wl1271_rx_descriptor);
		end_of_log = WLCORE_FW_LOG_END;
	} else {
		offset = sizeof(addr);
		end_of_log = addr;
	}

	/*                                        */
	do {
		memset(block, 0, WL12XX_HW_BLOCK_SIZE);
		ret = wlcore_read_hwaddr(wl, addr, block, WL12XX_HW_BLOCK_SIZE,
					 false);
		if (ret < 0)
			goto out;

		/*
                                                               
                                                               
                                                          
                                                                 
   */
		addr = le32_to_cpup((__le32 *)block);
		if (!wl12xx_copy_fwlog(wl, block + offset,
				       WL12XX_HW_BLOCK_SIZE - offset))
			break;
	} while (addr && (addr != end_of_log));

	wake_up_interruptible(&wl->fwlog_waitq);

out:
	kfree(block);
}

static void wlcore_print_recovery(struct wl1271 *wl)
{
	u32 pc = 0;
	u32 hint_sts = 0;
	int ret;

	wl1271_info("Hardware recovery in progress. FW ver: %s",
		    wl->chip.fw_ver_str);

	/*                                                        */
	ret = wlcore_set_partition(wl, &wl->ptable[PART_BOOT]);
	if (ret < 0)
		return;

	ret = wlcore_read_reg(wl, REG_PC_ON_RECOVERY, &pc);
	if (ret < 0)
		return;

	ret = wlcore_read_reg(wl, REG_INTERRUPT_NO_CLEAR, &hint_sts);
	if (ret < 0)
		return;

	wl1271_info("pc: 0x%x, hint_sts: 0x%08x count: %d",
				pc, hint_sts, ++wl->recovery_count);

	wlcore_set_partition(wl, &wl->ptable[PART_WORK]);
}


static void wl1271_recovery_work(struct work_struct *work)
{
	struct wl1271 *wl =
		container_of(work, struct wl1271, recovery_work);
	struct wl12xx_vif *wlvif;
	struct ieee80211_vif *vif;

	mutex_lock(&wl->mutex);

	if (wl->state == WLCORE_STATE_OFF || wl->plt)
		goto out_unlock;

	if (!test_bit(WL1271_FLAG_INTENDED_FW_RECOVERY, &wl->flags)) {
		wl12xx_read_fwlog_panic(wl);
		wlcore_print_recovery(wl);
	}

	BUG_ON(wl->conf.recovery.bug_on_recovery &&
	       !test_bit(WL1271_FLAG_INTENDED_FW_RECOVERY, &wl->flags));

	if (wl->conf.recovery.no_recovery) {
		wl1271_info("No recovery (chosen on module load). Fw will remain stuck.");
		goto out_unlock;
	}

	/*                                       */
	wlcore_stop_queues(wl, WLCORE_QUEUE_STOP_REASON_FW_RESTART);

	/*                    */
	while (!list_empty(&wl->wlvif_list)) {
		wlvif = list_first_entry(&wl->wlvif_list,
				       struct wl12xx_vif, list);
		vif = wl12xx_wlvif_to_vif(wlvif);
		__wl1271_op_remove_interface(wl, vif, false);
	}

	wlcore_op_stop_locked(wl);

	ieee80211_restart_hw(wl->hw);

	/*
                                                                      
                      
  */
	wlcore_wake_queues(wl, WLCORE_QUEUE_STOP_REASON_FW_RESTART);

out_unlock:
	wl->watchdog_recovery = false;
	clear_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS, &wl->flags);
	mutex_unlock(&wl->mutex);
}

static int wlcore_fw_wakeup(struct wl1271 *wl)
{
	return wlcore_raw_write32(wl, HW_ACCESS_ELP_CTRL_REG, ELPCTRL_WAKE_UP);
}

static int wl1271_setup(struct wl1271 *wl)
{
	wl->fw_status_1 = kmalloc(WLCORE_FW_STATUS_1_LEN(wl->num_rx_desc) +
				  sizeof(*wl->fw_status_2) +
				  wl->fw_status_priv_len, GFP_KERNEL);
	if (!wl->fw_status_1)
		return -ENOMEM;

	wl->fw_status_2 = (struct wl_fw_status_2 *)
				(((u8 *) wl->fw_status_1) +
				WLCORE_FW_STATUS_1_LEN(wl->num_rx_desc));

	wl->tx_res_if = kmalloc(sizeof(*wl->tx_res_if), GFP_KERNEL);
	if (!wl->tx_res_if) {
		kfree(wl->fw_status_1);
		return -ENOMEM;
	}

	return 0;
}

static int wl12xx_set_power_on(struct wl1271 *wl)
{
	int ret;

	msleep(WL1271_PRE_POWER_ON_SLEEP);
	ret = wl1271_power_on(wl);
	if (ret < 0)
		goto out;
	msleep(WL1271_POWER_ON_SLEEP);
	wl1271_io_reset(wl);
	wl1271_io_init(wl);

	ret = wlcore_set_partition(wl, &wl->ptable[PART_BOOT]);
	if (ret < 0)
		goto fail;

	/*                    */
	ret = wlcore_fw_wakeup(wl);
	if (ret < 0)
		goto fail;

out:
	return ret;

fail:
	wl1271_power_off(wl);
	return ret;
}

static int wl12xx_chip_wakeup(struct wl1271 *wl, bool plt)
{
	int ret = 0;

	ret = wl12xx_set_power_on(wl);
	if (ret < 0)
		goto out;

	/*
                                                           
                                                             
                                                               
                                                         
                                                            
               
   
                                                            
                                               
  */
	if (!wl1271_set_block_size(wl))
		wl->quirks &= ~WLCORE_QUIRK_TX_BLOCKSIZE_ALIGN;

	/*                                                              */

	ret = wl1271_setup(wl);
	if (ret < 0)
		goto out;

	ret = wl12xx_fetch_firmware(wl, plt);
	if (ret < 0)
		goto out;

out:
	return ret;
}

int wl1271_plt_start(struct wl1271 *wl, const enum plt_mode plt_mode)
{
	int retries = WL1271_BOOT_RETRIES;
	struct wiphy *wiphy = wl->hw->wiphy;

	static const char* const PLT_MODE[] = {
		"PLT_OFF",
		"PLT_ON",
		"PLT_FEM_DETECT"
	};

	int ret;

	mutex_lock(&wl->mutex);

	wl1271_notice("power up");

	if (wl->state != WLCORE_STATE_OFF) {
		wl1271_error("cannot go into PLT state because not "
			     "in off state: %d", wl->state);
		ret = -EBUSY;
		goto out;
	}

	/*                                                      */
	wl->plt = true;
	wl->plt_mode = plt_mode;

	while (retries) {
		retries--;
		ret = wl12xx_chip_wakeup(wl, true);
		if (ret < 0)
			goto power_off;

		ret = wl->ops->plt_init(wl);
		if (ret < 0)
			goto power_off;

		wl->state = WLCORE_STATE_ON;
		wl1271_notice("firmware booted in PLT mode %s (%s)",
			      PLT_MODE[plt_mode],
			      wl->chip.fw_ver_str);

		/*                                           */
		wiphy->hw_version = wl->chip.id;
		strncpy(wiphy->fw_version, wl->chip.fw_ver_str,
			sizeof(wiphy->fw_version));

		goto out;

power_off:
		wl1271_power_off(wl);
	}

	wl->plt = false;
	wl->plt_mode = PLT_OFF;

	wl1271_error("firmware boot in PLT mode failed despite %d retries",
		     WL1271_BOOT_RETRIES);
out:
	mutex_unlock(&wl->mutex);

	return ret;
}

int wl1271_plt_stop(struct wl1271 *wl)
{
	int ret = 0;

	wl1271_notice("power down");

	/*
                                                                
                                                                     
                                 
  */
	wlcore_disable_interrupts(wl);
	mutex_lock(&wl->mutex);
	if (!wl->plt) {
		mutex_unlock(&wl->mutex);

		/*
                                                              
                                                             
                                                             
   */
		wlcore_enable_interrupts(wl);

		wl1271_error("cannot power down because not in PLT "
			     "state: %d", wl->state);
		ret = -EBUSY;
		goto out;
	}

	mutex_unlock(&wl->mutex);

	wl1271_flush_deferred_work(wl);
	cancel_work_sync(&wl->netstack_work);
	cancel_work_sync(&wl->recovery_work);
	cancel_delayed_work_sync(&wl->elp_work);
	cancel_delayed_work_sync(&wl->tx_watchdog_work);

	mutex_lock(&wl->mutex);
	wl1271_power_off(wl);
	wl->flags = 0;
	wl->sleep_auth = WL1271_PSM_ILLEGAL;
	wl->state = WLCORE_STATE_OFF;
	wl->plt = false;
	wl->plt_mode = PLT_OFF;
	wl->rx_counter = 0;
	mutex_unlock(&wl->mutex);

out:
	return ret;
}

static void wl1271_op_tx(struct ieee80211_hw *hw,
			 struct ieee80211_tx_control *control,
			 struct sk_buff *skb)
{
	struct wl1271 *wl = hw->priv;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_vif *vif = info->control.vif;
	struct wl12xx_vif *wlvif = NULL;
	unsigned long flags;
	int q, mapping;
	u8 hlid;

	if (!vif) {
		wl1271_debug(DEBUG_TX, "DROP skb with no vif");
		ieee80211_free_txskb(hw, skb);
		return;
	}

	wlvif = wl12xx_vif_to_data(vif);
	mapping = skb_get_queue_mapping(skb);
	q = wl1271_tx_get_queue(mapping);

	hlid = wl12xx_tx_get_hlid(wl, wlvif, skb, control->sta);

	spin_lock_irqsave(&wl->wl_lock, flags);

	/*
                                                                  
                                                                  
                                
  */
	if (hlid == WL12XX_INVALID_LINK_ID ||
	    (!test_bit(hlid, wlvif->links_map)) ||
	     (wlcore_is_queue_stopped_locked(wl, wlvif, q) &&
	      !wlcore_is_queue_stopped_by_reason_locked(wl, wlvif, q,
			WLCORE_QUEUE_STOP_REASON_WATERMARK))) {
		wl1271_debug(DEBUG_TX, "DROP skb hlid %d q %d", hlid, q);
		ieee80211_free_txskb(hw, skb);
		goto out;
	}

	wl1271_debug(DEBUG_TX, "queue skb hlid %d q %d len %d",
		     hlid, q, skb->len);
	skb_queue_tail(&wl->links[hlid].tx_queue[q], skb);

	wl->tx_queue_count[q]++;
	wlvif->tx_queue_count[q]++;

	/*
                                                                  
                                                          
  */
	if (wlvif->tx_queue_count[q] >= WL1271_TX_QUEUE_HIGH_WATERMARK &&
	    !wlcore_is_queue_stopped_by_reason_locked(wl, wlvif, q,
					WLCORE_QUEUE_STOP_REASON_WATERMARK)) {
		wl1271_debug(DEBUG_TX, "op_tx: stopping queues for q %d", q);
		wlcore_stop_queue_locked(wl, wlvif, q,
					 WLCORE_QUEUE_STOP_REASON_WATERMARK);
	}

	/*
                                                                 
                                                     
  */

	if (!test_bit(WL1271_FLAG_FW_TX_BUSY, &wl->flags) &&
	    !test_bit(WL1271_FLAG_TX_PENDING, &wl->flags))
		ieee80211_queue_work(wl->hw, &wl->tx_work);

out:
	spin_unlock_irqrestore(&wl->wl_lock, flags);
}

int wl1271_tx_dummy_packet(struct wl1271 *wl)
{
	unsigned long flags;
	int q;

	/*                                                               */
	if (test_bit(WL1271_FLAG_DUMMY_PACKET_PENDING, &wl->flags))
		return 0;

	q = wl1271_tx_get_queue(skb_get_queue_mapping(wl->dummy_packet));

	spin_lock_irqsave(&wl->wl_lock, flags);
	set_bit(WL1271_FLAG_DUMMY_PACKET_PENDING, &wl->flags);
	wl->tx_queue_count[q]++;
	spin_unlock_irqrestore(&wl->wl_lock, flags);

	/*                                                                  */
	if (!test_bit(WL1271_FLAG_FW_TX_BUSY, &wl->flags))
		return wlcore_tx_work_locked(wl);

	/*
                                                                   
                              
  */
	return 0;
}

/*
                                                                          
                                                                             
                                                   
 */
#define TOTAL_TX_DUMMY_PACKET_SIZE (ALIGN(1400, 512))

static struct sk_buff *wl12xx_alloc_dummy_packet(struct wl1271 *wl)
{
	struct sk_buff *skb;
	struct ieee80211_hdr_3addr *hdr;
	unsigned int dummy_packet_size;

	dummy_packet_size = TOTAL_TX_DUMMY_PACKET_SIZE -
			    sizeof(struct wl1271_tx_hw_descr) - sizeof(*hdr);

	skb = dev_alloc_skb(TOTAL_TX_DUMMY_PACKET_SIZE);
	if (!skb) {
		wl1271_warning("Failed to allocate a dummy packet skb");
		return NULL;
	}

	skb_reserve(skb, sizeof(struct wl1271_tx_hw_descr));

	hdr = (struct ieee80211_hdr_3addr *) skb_put(skb, sizeof(*hdr));
	memset(hdr, 0, sizeof(*hdr));
	hdr->frame_control = cpu_to_le16(IEEE80211_FTYPE_DATA |
					 IEEE80211_STYPE_NULLFUNC |
					 IEEE80211_FCTL_TODS);

	memset(skb_put(skb, dummy_packet_size), 0, dummy_packet_size);

	/*                                                */
	skb->priority = WL1271_TID_MGMT;

	/*                                          */
	skb_set_queue_mapping(skb, 0);
	memset(IEEE80211_SKB_CB(skb), 0, sizeof(struct ieee80211_tx_info));

	return skb;
}


#ifdef CONFIG_PM
static int
wl1271_validate_wowlan_pattern(struct cfg80211_wowlan_trig_pkt_pattern *p)
{
	int num_fields = 0, in_field = 0, fields_size = 0;
	int i, pattern_len = 0;

	if (!p->mask) {
		wl1271_warning("No mask in WoWLAN pattern");
		return -EINVAL;
	}

	/*
                                                                        
                                                                    
                                                                    
                                                        
                                                                 
                                                                   
                                     
  */
	for (i = 0; i < p->pattern_len; i++) {
		if (test_bit(i, (unsigned long *)p->mask)) {
			if (!in_field) {
				in_field = 1;
				pattern_len = 1;
			} else {
				if (i == WL1271_RX_FILTER_ETH_HEADER_SIZE) {
					num_fields++;
					fields_size += pattern_len +
						RX_FILTER_FIELD_OVERHEAD;
					pattern_len = 1;
				} else
					pattern_len++;
			}
		} else {
			if (in_field) {
				in_field = 0;
				fields_size += pattern_len +
					RX_FILTER_FIELD_OVERHEAD;
				num_fields++;
			}
		}
	}

	if (in_field) {
		fields_size += pattern_len + RX_FILTER_FIELD_OVERHEAD;
		num_fields++;
	}

	if (num_fields > WL1271_RX_FILTER_MAX_FIELDS) {
		wl1271_warning("RX Filter too complex. Too many segments");
		return -EINVAL;
	}

	if (fields_size > WL1271_RX_FILTER_MAX_FIELDS_SIZE) {
		wl1271_warning("RX filter pattern is too big");
		return -E2BIG;
	}

	return 0;
}

struct wl12xx_rx_filter *wl1271_rx_filter_alloc(void)
{
	return kzalloc(sizeof(struct wl12xx_rx_filter), GFP_KERNEL);
}

void wl1271_rx_filter_free(struct wl12xx_rx_filter *filter)
{
	int i;

	if (filter == NULL)
		return;

	for (i = 0; i < filter->num_fields; i++)
		kfree(filter->fields[i].pattern);

	kfree(filter);
}

int wl1271_rx_filter_alloc_field(struct wl12xx_rx_filter *filter,
				 u16 offset, u8 flags,
				 u8 *pattern, u8 len)
{
	struct wl12xx_rx_filter_field *field;

	if (filter->num_fields == WL1271_RX_FILTER_MAX_FIELDS) {
		wl1271_warning("Max fields per RX filter. can't alloc another");
		return -EINVAL;
	}

	field = &filter->fields[filter->num_fields];

	field->pattern = kzalloc(len, GFP_KERNEL);
	if (!field->pattern) {
		wl1271_warning("Failed to allocate RX filter pattern");
		return -ENOMEM;
	}

	filter->num_fields++;

	field->offset = cpu_to_le16(offset);
	field->flags = flags;
	field->len = len;
	memcpy(field->pattern, pattern, len);

	return 0;
}

int wl1271_rx_filter_get_fields_size(struct wl12xx_rx_filter *filter)
{
	int i, fields_size = 0;

	for (i = 0; i < filter->num_fields; i++)
		fields_size += filter->fields[i].len +
			sizeof(struct wl12xx_rx_filter_field) -
			sizeof(u8 *);

	return fields_size;
}

void wl1271_rx_filter_flatten_fields(struct wl12xx_rx_filter *filter,
				    u8 *buf)
{
	int i;
	struct wl12xx_rx_filter_field *field;

	for (i = 0; i < filter->num_fields; i++) {
		field = (struct wl12xx_rx_filter_field *)buf;

		field->offset = filter->fields[i].offset;
		field->flags = filter->fields[i].flags;
		field->len = filter->fields[i].len;

		memcpy(&field->pattern, filter->fields[i].pattern, field->len);
		buf += sizeof(struct wl12xx_rx_filter_field) -
			sizeof(u8 *) + field->len;
	}
}

/*
                                            
                                                 
 */
static int wl1271_convert_wowlan_pattern_to_rx_filter(
	struct cfg80211_wowlan_trig_pkt_pattern *p,
	struct wl12xx_rx_filter **f)
{
	int i, j, ret = 0;
	struct wl12xx_rx_filter *filter;
	u16 offset;
	u8 flags, len;

	filter = wl1271_rx_filter_alloc();
	if (!filter) {
		wl1271_warning("Failed to alloc rx filter");
		ret = -ENOMEM;
		goto err;
	}

	i = 0;
	while (i < p->pattern_len) {
		if (!test_bit(i, (unsigned long *)p->mask)) {
			i++;
			continue;
		}

		for (j = i; j < p->pattern_len; j++) {
			if (!test_bit(j, (unsigned long *)p->mask))
				break;

			if (i < WL1271_RX_FILTER_ETH_HEADER_SIZE &&
			    j >= WL1271_RX_FILTER_ETH_HEADER_SIZE)
				break;
		}

		if (i < WL1271_RX_FILTER_ETH_HEADER_SIZE) {
			offset = i;
			flags = WL1271_RX_FILTER_FLAG_ETHERNET_HEADER;
		} else {
			offset = i - WL1271_RX_FILTER_ETH_HEADER_SIZE;
			flags = WL1271_RX_FILTER_FLAG_IP_HEADER;
		}

		len = j - i;

		ret = wl1271_rx_filter_alloc_field(filter,
						   offset,
						   flags,
						   &p->pattern[i], len);
		if (ret)
			goto err;

		i = j;
	}

	filter->action = FILTER_SIGNAL;

	*f = filter;
	return 0;

err:
	wl1271_rx_filter_free(filter);
	*f = NULL;

	return ret;
}

static int wl1271_configure_wowlan(struct wl1271 *wl,
				   struct cfg80211_wowlan *wow)
{
	int i, ret;

	if (!wow || wow->any || !wow->n_patterns) {
		ret = wl1271_acx_default_rx_filter_enable(wl, 0,
							  FILTER_SIGNAL);
		if (ret)
			goto out;

		ret = wl1271_rx_filter_clear_all(wl);
		if (ret)
			goto out;

		return 0;
	}

	if (WARN_ON(wow->n_patterns > WL1271_MAX_RX_FILTERS))
		return -EINVAL;

	/*                                                                 */
	for (i = 0; i < wow->n_patterns; i++) {
		ret = wl1271_validate_wowlan_pattern(&wow->patterns[i]);
		if (ret) {
			wl1271_warning("Bad wowlan pattern %d", i);
			return ret;
		}
	}

	ret = wl1271_acx_default_rx_filter_enable(wl, 0, FILTER_SIGNAL);
	if (ret)
		goto out;

	ret = wl1271_rx_filter_clear_all(wl);
	if (ret)
		goto out;

	/*                                        */
	for (i = 0; i < wow->n_patterns; i++) {
		struct cfg80211_wowlan_trig_pkt_pattern *p;
		struct wl12xx_rx_filter *filter = NULL;

		p = &wow->patterns[i];

		ret = wl1271_convert_wowlan_pattern_to_rx_filter(p, &filter);
		if (ret) {
			wl1271_warning("Failed to create an RX filter from "
				       "wowlan pattern %d", i);
			goto out;
		}

		ret = wl1271_rx_filter_enable(wl, i, 1, filter);

		wl1271_rx_filter_free(filter);
		if (ret)
			goto out;
	}

	ret = wl1271_acx_default_rx_filter_enable(wl, 1, FILTER_DROP);

out:
	return ret;
}

static int wl1271_configure_suspend_sta(struct wl1271 *wl,
					struct wl12xx_vif *wlvif,
					struct cfg80211_wowlan *wow)
{
	int ret = 0;

	if (!test_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wl1271_configure_wowlan(wl, wow);
	if (ret < 0)
		goto out_sleep;

	if ((wl->conf.conn.suspend_wake_up_event ==
	     wl->conf.conn.wake_up_event) &&
	    (wl->conf.conn.suspend_listen_interval ==
	     wl->conf.conn.listen_interval))
		goto out_sleep;

	ret = wl1271_acx_wake_up_conditions(wl, wlvif,
				    wl->conf.conn.suspend_wake_up_event,
				    wl->conf.conn.suspend_listen_interval);

	if (ret < 0)
		wl1271_error("suspend: set wake up conditions failed: %d", ret);

out_sleep:
	wl1271_ps_elp_sleep(wl);
out:
	return ret;

}

static int wl1271_configure_suspend_ap(struct wl1271 *wl,
				       struct wl12xx_vif *wlvif)
{
	int ret = 0;

	if (!test_bit(WLVIF_FLAG_AP_STARTED, &wlvif->flags))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wl1271_acx_beacon_filter_opt(wl, wlvif, true);

	wl1271_ps_elp_sleep(wl);
out:
	return ret;

}

static int wl1271_configure_suspend(struct wl1271 *wl,
				    struct wl12xx_vif *wlvif,
				    struct cfg80211_wowlan *wow)
{
	if (wlvif->bss_type == BSS_TYPE_STA_BSS)
		return wl1271_configure_suspend_sta(wl, wlvif, wow);
	if (wlvif->bss_type == BSS_TYPE_AP_BSS)
		return wl1271_configure_suspend_ap(wl, wlvif);
	return 0;
}

static void wl1271_configure_resume(struct wl1271 *wl,
				    struct wl12xx_vif *wlvif)
{
	int ret = 0;
	bool is_ap = wlvif->bss_type == BSS_TYPE_AP_BSS;
	bool is_sta = wlvif->bss_type == BSS_TYPE_STA_BSS;

	if ((!is_ap) && (!is_sta))
		return;

	if (is_sta && !test_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags))
		return;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		return;

	if (is_sta) {
		wl1271_configure_wowlan(wl, NULL);

		if ((wl->conf.conn.suspend_wake_up_event ==
		     wl->conf.conn.wake_up_event) &&
		    (wl->conf.conn.suspend_listen_interval ==
		     wl->conf.conn.listen_interval))
			goto out_sleep;

		ret = wl1271_acx_wake_up_conditions(wl, wlvif,
				    wl->conf.conn.wake_up_event,
				    wl->conf.conn.listen_interval);

		if (ret < 0)
			wl1271_error("resume: wake up conditions failed: %d",
				     ret);

	} else if (is_ap) {
		ret = wl1271_acx_beacon_filter_opt(wl, wlvif, false);
	}

out_sleep:
	wl1271_ps_elp_sleep(wl);
}

static int wl1271_op_suspend(struct ieee80211_hw *hw,
			    struct cfg80211_wowlan *wow)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif;
	int ret;

	wl1271_debug(DEBUG_MAC80211, "mac80211 suspend wow=%d", !!wow);
	WARN_ON(!wow);

	/*                                                   */
	if (test_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS, &wl->flags)) {
		wl1271_warning("postponing suspend to perform recovery");
		return -EBUSY;
	}

	wl1271_tx_flush(wl);

	mutex_lock(&wl->mutex);
	wl->wow_enabled = true;
	wl12xx_for_each_wlvif(wl, wlvif) {
		ret = wl1271_configure_suspend(wl, wlvif, wow);
		if (ret < 0) {
			mutex_unlock(&wl->mutex);
			wl1271_warning("couldn't prepare device to suspend");
			return ret;
		}
	}
	mutex_unlock(&wl->mutex);
	/*                          */
	wl1271_debug(DEBUG_MAC80211, "flushing remaining works");

	/*
                                                      
                    
  */
	wlcore_disable_interrupts(wl);

	/*
                                                             
                                                          
  */
	set_bit(WL1271_FLAG_SUSPENDED, &wl->flags);

	wlcore_enable_interrupts(wl);
	flush_work(&wl->tx_work);
	flush_delayed_work(&wl->elp_work);

	return 0;
}

static int wl1271_op_resume(struct ieee80211_hw *hw)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif;
	unsigned long flags;
	bool run_irq_work = false, pending_recovery;
	int ret;

	wl1271_debug(DEBUG_MAC80211, "mac80211 resume wow=%d",
		     wl->wow_enabled);
	WARN_ON(!wl->wow_enabled);

	/*
                                                               
                            
  */
	spin_lock_irqsave(&wl->wl_lock, flags);
	clear_bit(WL1271_FLAG_SUSPENDED, &wl->flags);
	if (test_and_clear_bit(WL1271_FLAG_PENDING_WORK, &wl->flags))
		run_irq_work = true;
	spin_unlock_irqrestore(&wl->wl_lock, flags);

	mutex_lock(&wl->mutex);

	/*                                                          */
	pending_recovery = test_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS,
				    &wl->flags);

	if (run_irq_work) {
		wl1271_debug(DEBUG_MAC80211,
			     "run postponed irq_work directly");

		/*                                             */
		if (!pending_recovery) {
			ret = wlcore_irq_locked(wl);
			if (ret)
				wl12xx_queue_recovery_work(wl);
		}

		wlcore_enable_interrupts(wl);
	}

	if (pending_recovery) {
		wl1271_warning("queuing forgotten recovery on resume");
		ieee80211_queue_work(wl->hw, &wl->recovery_work);
		goto out;
	}

	wl12xx_for_each_wlvif(wl, wlvif) {
		wl1271_configure_resume(wl, wlvif);
	}

out:
	wl->wow_enabled = false;
	mutex_unlock(&wl->mutex);

	return 0;
}
#endif

static int wl1271_op_start(struct ieee80211_hw *hw)
{
	wl1271_debug(DEBUG_MAC80211, "mac80211 start");

	/*
                                                        
                                                                
                                                                
                                                                
                               
   
                                                                   
                                                            
  */

	return 0;
}

static void wlcore_op_stop_locked(struct wl1271 *wl)
{
	int i;

	if (wl->state == WLCORE_STATE_OFF) {
		if (test_and_clear_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS,
					&wl->flags))
			wlcore_enable_interrupts(wl);

		return;
	}

	/*
                                                                     
                                         
  */
	wl->state = WLCORE_STATE_OFF;

	/*
                                                                       
                                            
  */
	wlcore_disable_interrupts_nosync(wl);

	mutex_unlock(&wl->mutex);

	wlcore_synchronize_interrupts(wl);
	if (!test_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS, &wl->flags))
		cancel_work_sync(&wl->recovery_work);
	wl1271_flush_deferred_work(wl);
	cancel_delayed_work_sync(&wl->scan_complete_work);
	cancel_work_sync(&wl->netstack_work);
	cancel_work_sync(&wl->tx_work);
	cancel_delayed_work_sync(&wl->elp_work);
	cancel_delayed_work_sync(&wl->tx_watchdog_work);

	/*                                                             */
	mutex_lock(&wl->mutex);
	wl12xx_tx_reset(wl);

	wl1271_power_off(wl);
	/*
                                                                       
                                                                 
                                                     
  */
	if (test_and_clear_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS, &wl->flags))
		wlcore_enable_interrupts(wl);

	wl->band = IEEE80211_BAND_2GHZ;

	wl->rx_counter = 0;
	wl->power_level = WL1271_DEFAULT_POWER_LEVEL;
	wl->channel_type = NL80211_CHAN_NO_HT;
	wl->tx_blocks_available = 0;
	wl->tx_allocated_blocks = 0;
	wl->tx_results_count = 0;
	wl->tx_packets_count = 0;
	wl->time_offset = 0;
	wl->ap_fw_ps_map = 0;
	wl->ap_ps_map = 0;
	wl->sleep_auth = WL1271_PSM_ILLEGAL;
	memset(wl->roles_map, 0, sizeof(wl->roles_map));
	memset(wl->links_map, 0, sizeof(wl->links_map));
	memset(wl->roc_map, 0, sizeof(wl->roc_map));
	memset(wl->session_ids, 0, sizeof(wl->session_ids));
	wl->active_sta_count = 0;
	wl->active_link_count = 0;

	/*                                     */
	wl->links[WL12XX_SYSTEM_HLID].allocated_pkts = 0;
	wl->links[WL12XX_SYSTEM_HLID].prev_freed_pkts = 0;
	__set_bit(WL12XX_SYSTEM_HLID, wl->links_map);

	/*
                                                                    
                                                                     
                                                       
  */
	wl->flags = 0;

	wl->tx_blocks_freed = 0;

	for (i = 0; i < NUM_TX_QUEUES; i++) {
		wl->tx_pkts_freed[i] = 0;
		wl->tx_allocated_pkts[i] = 0;
	}

	wl1271_debugfs_reset(wl);

	kfree(wl->fw_status_1);
	wl->fw_status_1 = NULL;
	wl->fw_status_2 = NULL;
	kfree(wl->tx_res_if);
	wl->tx_res_if = NULL;
	kfree(wl->target_mem_map);
	wl->target_mem_map = NULL;

	/*
                                                     
                                                    
  */
	memset(wl->reg_ch_conf_last, 0, sizeof(wl->reg_ch_conf_last));
}

static void wlcore_op_stop(struct ieee80211_hw *hw)
{
	struct wl1271 *wl = hw->priv;

	wl1271_debug(DEBUG_MAC80211, "mac80211 stop");

	mutex_lock(&wl->mutex);

	wlcore_op_stop_locked(wl);

	mutex_unlock(&wl->mutex);
}

static void wlcore_channel_switch_work(struct work_struct *work)
{
	struct delayed_work *dwork;
	struct wl1271 *wl;
	struct ieee80211_vif *vif;
	struct wl12xx_vif *wlvif;
	int ret;

	dwork = container_of(work, struct delayed_work, work);
	wlvif = container_of(dwork, struct wl12xx_vif, channel_switch_work);
	wl = wlvif->wl;

	wl1271_info("channel switch failed (role_id: %d).", wlvif->role_id);

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	/*                                           */
	if (!test_and_clear_bit(WLVIF_FLAG_CS_PROGRESS, &wlvif->flags))
		goto out;

	vif = wl12xx_wlvif_to_vif(wlvif);
	ieee80211_chswitch_done(vif, false);

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	wl12xx_cmd_stop_channel_switch(wl, wlvif);

	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);
}

static void wlcore_connection_loss_work(struct work_struct *work)
{
	struct delayed_work *dwork;
	struct wl1271 *wl;
	struct ieee80211_vif *vif;
	struct wl12xx_vif *wlvif;

	dwork = container_of(work, struct delayed_work, work);
	wlvif = container_of(dwork, struct wl12xx_vif, connection_loss_work);
	wl = wlvif->wl;

	wl1271_info("Connection loss work (role_id: %d).", wlvif->role_id);

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	/*                               */
	if (!test_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags))
		goto out;

	vif = wl12xx_wlvif_to_vif(wlvif);
	ieee80211_connection_loss(vif);
out:
	mutex_unlock(&wl->mutex);
}

static int wl12xx_allocate_rate_policy(struct wl1271 *wl, u8 *idx)
{
	u8 policy = find_first_zero_bit(wl->rate_policies_map,
					WL12XX_MAX_RATE_POLICIES);
	if (policy >= WL12XX_MAX_RATE_POLICIES)
		return -EBUSY;

	__set_bit(policy, wl->rate_policies_map);
	*idx = policy;
	return 0;
}

static void wl12xx_free_rate_policy(struct wl1271 *wl, u8 *idx)
{
	if (WARN_ON(*idx >= WL12XX_MAX_RATE_POLICIES))
		return;

	__clear_bit(*idx, wl->rate_policies_map);
	*idx = WL12XX_MAX_RATE_POLICIES;
}

static int wlcore_allocate_klv_template(struct wl1271 *wl, u8 *idx)
{
	u8 policy = find_first_zero_bit(wl->klv_templates_map,
					WLCORE_MAX_KLV_TEMPLATES);
	if (policy >= WLCORE_MAX_KLV_TEMPLATES)
		return -EBUSY;

	__set_bit(policy, wl->klv_templates_map);
	*idx = policy;
	return 0;
}

static void wlcore_free_klv_template(struct wl1271 *wl, u8 *idx)
{
	if (WARN_ON(*idx >= WLCORE_MAX_KLV_TEMPLATES))
		return;

	__clear_bit(*idx, wl->klv_templates_map);
	*idx = WLCORE_MAX_KLV_TEMPLATES;
}

static u8 wl12xx_get_role_type(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	switch (wlvif->bss_type) {
	case BSS_TYPE_AP_BSS:
		if (wlvif->p2p)
			return WL1271_ROLE_P2P_GO;
		else
			return WL1271_ROLE_AP;

	case BSS_TYPE_STA_BSS:
		if (wlvif->p2p)
			return WL1271_ROLE_P2P_CL;
		else
			return WL1271_ROLE_STA;

	case BSS_TYPE_IBSS:
		return WL1271_ROLE_IBSS;

	default:
		wl1271_error("invalid bss_type: %d", wlvif->bss_type);
	}
	return WL12XX_INVALID_ROLE_TYPE;
}

static int wl12xx_init_vif_data(struct wl1271 *wl, struct ieee80211_vif *vif)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int i;

	/*                                          */
	memset(wlvif, 0, offsetof(struct wl12xx_vif, persistent));

	switch (ieee80211_vif_type_p2p(vif)) {
	case NL80211_IFTYPE_P2P_CLIENT:
		wlvif->p2p = 1;
		/*              */
	case NL80211_IFTYPE_STATION:
		wlvif->bss_type = BSS_TYPE_STA_BSS;
		break;
	case NL80211_IFTYPE_ADHOC:
		wlvif->bss_type = BSS_TYPE_IBSS;
		break;
	case NL80211_IFTYPE_P2P_GO:
		wlvif->p2p = 1;
		/*              */
	case NL80211_IFTYPE_AP:
		wlvif->bss_type = BSS_TYPE_AP_BSS;
		break;
	default:
		wlvif->bss_type = MAX_BSS_TYPE;
		return -EOPNOTSUPP;
	}

	wlvif->role_id = WL12XX_INVALID_ROLE_ID;
	wlvif->dev_role_id = WL12XX_INVALID_ROLE_ID;
	wlvif->dev_hlid = WL12XX_INVALID_LINK_ID;

	if (wlvif->bss_type == BSS_TYPE_STA_BSS ||
	    wlvif->bss_type == BSS_TYPE_IBSS) {
		/*                    */
		wlvif->sta.hlid = WL12XX_INVALID_LINK_ID;
		wl12xx_allocate_rate_policy(wl, &wlvif->sta.basic_rate_idx);
		wl12xx_allocate_rate_policy(wl, &wlvif->sta.ap_rate_idx);
		wl12xx_allocate_rate_policy(wl, &wlvif->sta.p2p_rate_idx);
		wlcore_allocate_klv_template(wl, &wlvif->sta.klv_template_id);
		wlvif->basic_rate_set = CONF_TX_RATE_MASK_BASIC;
		wlvif->basic_rate = CONF_TX_RATE_MASK_BASIC;
		wlvif->rate_set = CONF_TX_RATE_MASK_BASIC;
	} else {
		/*              */
		wlvif->ap.bcast_hlid = WL12XX_INVALID_LINK_ID;
		wlvif->ap.global_hlid = WL12XX_INVALID_LINK_ID;
		wl12xx_allocate_rate_policy(wl, &wlvif->ap.mgmt_rate_idx);
		wl12xx_allocate_rate_policy(wl, &wlvif->ap.bcast_rate_idx);
		for (i = 0; i < CONF_TX_MAX_AC_COUNT; i++)
			wl12xx_allocate_rate_policy(wl,
						&wlvif->ap.ucast_rate_idx[i]);
		wlvif->basic_rate_set = CONF_TX_ENABLED_RATES;
		/*
                                           
                                                       
                                            
  */
		wlvif->basic_rate = CONF_TX_ENABLED_RATES;
		/*                                                    */
		wlvif->rate_set = CONF_TX_ENABLED_RATES;
	}

	wlvif->bitrate_masks[IEEE80211_BAND_2GHZ] = wl->conf.tx.basic_rate;
	wlvif->bitrate_masks[IEEE80211_BAND_5GHZ] = wl->conf.tx.basic_rate_5;
	wlvif->beacon_int = WL1271_DEFAULT_BEACON_INT;

	/*
                                                                 
                                                              
  */
	wlvif->band = wl->band;
	wlvif->channel = wl->channel;
	wlvif->power_level = wl->power_level;
	wlvif->channel_type = wl->channel_type;

	INIT_WORK(&wlvif->rx_streaming_enable_work,
		  wl1271_rx_streaming_enable_work);
	INIT_WORK(&wlvif->rx_streaming_disable_work,
		  wl1271_rx_streaming_disable_work);
	INIT_DELAYED_WORK(&wlvif->channel_switch_work,
			  wlcore_channel_switch_work);
	INIT_DELAYED_WORK(&wlvif->connection_loss_work,
			  wlcore_connection_loss_work);
	INIT_LIST_HEAD(&wlvif->list);

	setup_timer(&wlvif->rx_streaming_timer, wl1271_rx_streaming_timer,
		    (unsigned long) wlvif);
	return 0;
}

static int wl12xx_init_fw(struct wl1271 *wl)
{
	int retries = WL1271_BOOT_RETRIES;
	bool booted = false;
	struct wiphy *wiphy = wl->hw->wiphy;
	int ret;

	while (retries) {
		retries--;
		ret = wl12xx_chip_wakeup(wl, false);
		if (ret < 0)
			goto power_off;

		ret = wl->ops->boot(wl);
		if (ret < 0)
			goto power_off;

		ret = wl1271_hw_init(wl);
		if (ret < 0)
			goto irq_disable;

		booted = true;
		break;

irq_disable:
		mutex_unlock(&wl->mutex);
		/*                                                 
                                                           
                                                           
                                                          
                                                         
                                                        
                                                            */
		wlcore_disable_interrupts(wl);
		wl1271_flush_deferred_work(wl);
		cancel_work_sync(&wl->netstack_work);
		mutex_lock(&wl->mutex);
power_off:
		wl1271_power_off(wl);
	}

	if (!booted) {
		wl1271_error("firmware boot failed despite %d retries",
			     WL1271_BOOT_RETRIES);
		goto out;
	}

	wl1271_info("firmware booted (%s)", wl->chip.fw_ver_str);

	/*                                           */
	wiphy->hw_version = wl->chip.id;
	strncpy(wiphy->fw_version, wl->chip.fw_ver_str,
		sizeof(wiphy->fw_version));

	/*
                                                                   
                                 
  */
	if (!wl->enable_11a)
		wiphy->bands[IEEE80211_BAND_5GHZ]->n_channels = 0;

	wl1271_debug(DEBUG_MAC80211, "11a is %ssupported",
		     wl->enable_11a ? "" : "not ");

	wl->state = WLCORE_STATE_ON;
out:
	return ret;
}

static bool wl12xx_dev_role_started(struct wl12xx_vif *wlvif)
{
	return wlvif->dev_hlid != WL12XX_INVALID_LINK_ID;
}

/*
                                                         
                                                              
                                                               
                                                                   
                 
 */
static bool wl12xx_need_fw_change(struct wl1271 *wl,
				  struct vif_counter_data vif_counter_data,
				  bool add)
{
	enum wl12xx_fw_type current_fw = wl->fw_type;
	u8 vif_count = vif_counter_data.counter;

	if (test_bit(WL1271_FLAG_VIF_CHANGE_IN_PROGRESS, &wl->flags))
		return false;

	/*                                             */
	if (add && !vif_counter_data.cur_vif_running)
		vif_count++;

	wl->last_vif_count = vif_count;

	/*                                            */
	if (wl->state == WLCORE_STATE_OFF)
		return false;

	/*                                              */
	if (!wl->mr_fw_name)
		return false;

	if (vif_count > 1 && current_fw == WL12XX_FW_TYPE_NORMAL)
		return true;
	if (vif_count <= 1 && current_fw == WL12XX_FW_TYPE_MULTI)
		return true;

	return false;
}

/*
                                                                  
                                                             
 */
static void wl12xx_force_active_psm(struct wl1271 *wl)
{
	struct wl12xx_vif *wlvif;

	wl12xx_for_each_wlvif_sta(wl, wlvif) {
		wl1271_ps_set_mode(wl, wlvif, STATION_POWER_SAVE_MODE);
	}
}

struct wlcore_hw_queue_iter_data {
	unsigned long hw_queue_map[BITS_TO_LONGS(WLCORE_NUM_MAC_ADDRESSES)];
	/*             */
	struct ieee80211_vif *vif;
	/*                                         */
	bool cur_running;
};

static void wlcore_hw_queue_iter(void *data, u8 *mac,
				 struct ieee80211_vif *vif)
{
	struct wlcore_hw_queue_iter_data *iter_data = data;

	if (WARN_ON_ONCE(vif->hw_queue[0] == IEEE80211_INVAL_HW_QUEUE))
		return;

	if (iter_data->cur_running || vif == iter_data->vif) {
		iter_data->cur_running = true;
		return;
	}

	__set_bit(vif->hw_queue[0] / NUM_TX_QUEUES, iter_data->hw_queue_map);
}

static int wlcore_allocate_hw_queue_base(struct wl1271 *wl,
					 struct wl12xx_vif *wlvif)
{
	struct ieee80211_vif *vif = wl12xx_wlvif_to_vif(wlvif);
	struct wlcore_hw_queue_iter_data iter_data = {};
	int i, q_base;

	iter_data.vif = vif;

	/*                                          */
	ieee80211_iterate_active_interfaces_atomic(wl->hw,
					IEEE80211_IFACE_ITER_RESUME_ALL,
					wlcore_hw_queue_iter, &iter_data);

	/*                                                                  */
	if (iter_data.cur_running) {
		wlvif->hw_queue_base = vif->hw_queue[0];
		wl1271_debug(DEBUG_MAC80211,
			     "using pre-allocated hw queue base %d",
			     wlvif->hw_queue_base);

		/*                                        */
		goto adjust_cab_queue;
	}

	q_base = find_first_zero_bit(iter_data.hw_queue_map,
				     WLCORE_NUM_MAC_ADDRESSES);
	if (q_base >= WLCORE_NUM_MAC_ADDRESSES)
		return -EBUSY;

	wlvif->hw_queue_base = q_base * NUM_TX_QUEUES;
	wl1271_debug(DEBUG_MAC80211, "allocating hw queue base: %d",
		     wlvif->hw_queue_base);

	for (i = 0; i < NUM_TX_QUEUES; i++) {
		wl->queue_stop_reasons[wlvif->hw_queue_base + i] = 0;
		/*                                */
		vif->hw_queue[i] = wlvif->hw_queue_base + i;
	}

adjust_cab_queue:
	/*                                                           */
	if (wlvif->bss_type == BSS_TYPE_AP_BSS)
		vif->cab_queue = NUM_TX_QUEUES * WLCORE_NUM_MAC_ADDRESSES +
				 wlvif->hw_queue_base / NUM_TX_QUEUES;
	else
		vif->cab_queue = IEEE80211_INVAL_HW_QUEUE;

	return 0;
}

static int wl1271_op_add_interface(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	struct vif_counter_data vif_count;
	int ret = 0;
	u8 role_type;

	vif->driver_flags |= IEEE80211_VIF_BEACON_FILTER |
			     IEEE80211_VIF_SUPPORTS_CQM_RSSI;

	wl1271_debug(DEBUG_MAC80211, "mac80211 add interface type %d mac %pM",
		     ieee80211_vif_type_p2p(vif), vif->addr);

	wl12xx_get_vif_count(hw, vif, &vif_count);

	mutex_lock(&wl->mutex);
	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out_unlock;

	/*
                                                                  
                                                                
                                
  */
	if (test_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS, &wl->flags) ||
	    test_bit(WLVIF_FLAG_INITIALIZED, &wlvif->flags)) {
		ret = -EBUSY;
		goto out;
	}


	ret = wl12xx_init_vif_data(wl, vif);
	if (ret < 0)
		goto out;

	wlvif->wl = wl;
	role_type = wl12xx_get_role_type(wl, wlvif);
	if (role_type == WL12XX_INVALID_ROLE_TYPE) {
		ret = -EINVAL;
		goto out;
	}

	ret = wlcore_allocate_hw_queue_base(wl, wlvif);
	if (ret < 0)
		goto out;

	if (wl12xx_need_fw_change(wl, vif_count, true)) {
		wl12xx_force_active_psm(wl);
		set_bit(WL1271_FLAG_INTENDED_FW_RECOVERY, &wl->flags);
		mutex_unlock(&wl->mutex);
		wl1271_recovery_work(&wl->recovery_work);
		return 0;
	}

	/*
                                                             
                                                    
  */
	if (wl->state == WLCORE_STATE_OFF) {
		/*
                                                    
                            
   */
		memcpy(wl->addresses[0].addr, vif->addr, ETH_ALEN);

		ret = wl12xx_init_fw(wl);
		if (ret < 0)
			goto out;
	}

	ret = wl12xx_cmd_role_enable(wl, vif->addr,
				     role_type, &wlvif->role_id);
	if (ret < 0)
		goto out;

	ret = wl1271_init_vif_specific(wl, vif);
	if (ret < 0)
		goto out;

	list_add(&wlvif->list, &wl->wlvif_list);
	set_bit(WLVIF_FLAG_INITIALIZED, &wlvif->flags);

	if (wlvif->bss_type == BSS_TYPE_AP_BSS)
		wl->ap_count++;
	else
		wl->sta_count++;
out:
	wl1271_ps_elp_sleep(wl);
out_unlock:
	mutex_unlock(&wl->mutex);

	return ret;
}

static void __wl1271_op_remove_interface(struct wl1271 *wl,
					 struct ieee80211_vif *vif,
					 bool reset_tx_queues)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int i, ret;
	bool is_ap = (wlvif->bss_type == BSS_TYPE_AP_BSS);

	wl1271_debug(DEBUG_MAC80211, "mac80211 remove interface");

	if (!test_and_clear_bit(WLVIF_FLAG_INITIALIZED, &wlvif->flags))
		return;

	/*                                                     */
	if (wl->state == WLCORE_STATE_OFF)
		return;

	wl1271_info("down");

	if (wl->scan.state != WL1271_SCAN_STATE_IDLE &&
	    wl->scan_wlvif == wlvif) {
		/*
                                                        
                                                              
   */
		wl12xx_rearm_tx_watchdog_locked(wl);

		wl->scan.state = WL1271_SCAN_STATE_IDLE;
		memset(wl->scan.scanned_ch, 0, sizeof(wl->scan.scanned_ch));
		wl->scan_wlvif = NULL;
		wl->scan.req = NULL;
		ieee80211_scan_completed(wl->hw, true);
	}

	if (wl->sched_vif == wlvif) {
		ieee80211_sched_scan_stopped(wl->hw);
		wl->sched_vif = NULL;
	}

	if (wl->roc_vif == vif) {
		wl->roc_vif = NULL;
		ieee80211_remain_on_channel_expired(wl->hw);
	}

	if (!test_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS, &wl->flags)) {
		/*                      */
		ret = wl1271_ps_elp_wakeup(wl);
		if (ret < 0)
			goto deinit;

		if (wlvif->bss_type == BSS_TYPE_STA_BSS ||
		    wlvif->bss_type == BSS_TYPE_IBSS) {
			if (wl12xx_dev_role_started(wlvif))
				wl12xx_stop_dev(wl, wlvif);
		}

		ret = wl12xx_cmd_role_disable(wl, &wlvif->role_id);
		if (ret < 0)
			goto deinit;

		wl1271_ps_elp_sleep(wl);
	}
deinit:
	wl12xx_tx_reset_wlvif(wl, wlvif);

	/*                                      */
	wlvif->dev_hlid = WL12XX_INVALID_LINK_ID;

	if (wlvif->bss_type == BSS_TYPE_STA_BSS ||
	    wlvif->bss_type == BSS_TYPE_IBSS) {
		wlvif->sta.hlid = WL12XX_INVALID_LINK_ID;
		wl12xx_free_rate_policy(wl, &wlvif->sta.basic_rate_idx);
		wl12xx_free_rate_policy(wl, &wlvif->sta.ap_rate_idx);
		wl12xx_free_rate_policy(wl, &wlvif->sta.p2p_rate_idx);
		wlcore_free_klv_template(wl, &wlvif->sta.klv_template_id);
	} else {
		wlvif->ap.bcast_hlid = WL12XX_INVALID_LINK_ID;
		wlvif->ap.global_hlid = WL12XX_INVALID_LINK_ID;
		wl12xx_free_rate_policy(wl, &wlvif->ap.mgmt_rate_idx);
		wl12xx_free_rate_policy(wl, &wlvif->ap.bcast_rate_idx);
		for (i = 0; i < CONF_TX_MAX_AC_COUNT; i++)
			wl12xx_free_rate_policy(wl,
						&wlvif->ap.ucast_rate_idx[i]);
		wl1271_free_ap_keys(wl, wlvif);
	}

	dev_kfree_skb(wlvif->probereq);
	wlvif->probereq = NULL;
	if (wl->last_wlvif == wlvif)
		wl->last_wlvif = NULL;
	list_del(&wlvif->list);
	memset(wlvif->ap.sta_hlid_map, 0, sizeof(wlvif->ap.sta_hlid_map));
	wlvif->role_id = WL12XX_INVALID_ROLE_ID;
	wlvif->dev_role_id = WL12XX_INVALID_ROLE_ID;

	if (is_ap)
		wl->ap_count--;
	else
		wl->sta_count--;

	/*
                                                                       
                                         
  */
	if (test_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS, &wl->flags) &&
	    !test_bit(WL1271_FLAG_INTENDED_FW_RECOVERY, &wl->flags))
		goto unlock;

	if (wl->ap_count == 0 && is_ap && wl->sta_count) {
		u8 sta_auth = wl->conf.conn.sta_sleep_auth;
		/*                                          */
		if (sta_auth != WL1271_PSM_ILLEGAL)
			wl1271_acx_sleep_auth(wl, sta_auth);
		/*                                */
		else
			wl1271_acx_sleep_auth(wl, WL1271_PSM_ELP);
	}

unlock:
	mutex_unlock(&wl->mutex);

	del_timer_sync(&wlvif->rx_streaming_timer);
	cancel_work_sync(&wlvif->rx_streaming_enable_work);
	cancel_work_sync(&wlvif->rx_streaming_disable_work);
	cancel_delayed_work_sync(&wlvif->connection_loss_work);

	mutex_lock(&wl->mutex);
}

static void wl1271_op_remove_interface(struct ieee80211_hw *hw,
				       struct ieee80211_vif *vif)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	struct wl12xx_vif *iter;
	struct vif_counter_data vif_count;

	wl12xx_get_vif_count(hw, vif, &vif_count);
	mutex_lock(&wl->mutex);

	if (wl->state == WLCORE_STATE_OFF ||
	    !test_bit(WLVIF_FLAG_INITIALIZED, &wlvif->flags))
		goto out;

	/*
                                                                
                                                 
  */
	wl12xx_for_each_wlvif(wl, iter) {
		if (iter != wlvif)
			continue;

		__wl1271_op_remove_interface(wl, vif, true);
		break;
	}
	WARN_ON(iter != wlvif);
	if (wl12xx_need_fw_change(wl, vif_count, false)) {
		wl12xx_force_active_psm(wl);
		set_bit(WL1271_FLAG_INTENDED_FW_RECOVERY, &wl->flags);
		wl12xx_queue_recovery_work(wl);
	}
out:
	mutex_unlock(&wl->mutex);
}

static int wl12xx_op_change_interface(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif,
				      enum nl80211_iftype new_type, bool p2p)
{
	struct wl1271 *wl = hw->priv;
	int ret;

	set_bit(WL1271_FLAG_VIF_CHANGE_IN_PROGRESS, &wl->flags);
	wl1271_op_remove_interface(hw, vif);

	vif->type = new_type;
	vif->p2p = p2p;
	ret = wl1271_op_add_interface(hw, vif);

	clear_bit(WL1271_FLAG_VIF_CHANGE_IN_PROGRESS, &wl->flags);
	return ret;
}

static int wlcore_join(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	int ret;
	bool is_ibss = (wlvif->bss_type == BSS_TYPE_IBSS);

	/*
                                                                 
                                                                      
                                                                 
                                                                 
                                                                
                                                              
                                          
  */
	if (test_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags))
		wl1271_info("JOIN while associated.");

	/*                       */
	wlvif->encryption_type = KEY_NONE;

	if (is_ibss)
		ret = wl12xx_cmd_role_start_ibss(wl, wlvif);
	else {
		if (wl->quirks & WLCORE_QUIRK_START_STA_FAILS) {
			/*
                                                    
                                                    
                                                
                                    
                                     
    */
			wl12xx_cmd_role_start_sta(wl, wlvif);
			wl12xx_cmd_role_stop_sta(wl, wlvif);
		}

		ret = wl12xx_cmd_role_start_sta(wl, wlvif);
	}

	return ret;
}

static int wl1271_ssid_set(struct wl12xx_vif *wlvif, struct sk_buff *skb,
			    int offset)
{
	u8 ssid_len;
	const u8 *ptr = cfg80211_find_ie(WLAN_EID_SSID, skb->data + offset,
					 skb->len - offset);

	if (!ptr) {
		wl1271_error("No SSID in IEs!");
		return -ENOENT;
	}

	ssid_len = ptr[1];
	if (ssid_len > IEEE80211_MAX_SSID_LEN) {
		wl1271_error("SSID is too long!");
		return -EINVAL;
	}

	wlvif->ssid_len = ssid_len;
	memcpy(wlvif->ssid, ptr+2, ssid_len);
	return 0;
}

static int wlcore_set_ssid(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	struct ieee80211_vif *vif = wl12xx_wlvif_to_vif(wlvif);
	struct sk_buff *skb;
	int ieoffset;

	/*                                                                  */
	if (wlvif->bss_type != BSS_TYPE_STA_BSS)
		return -EINVAL;

	skb = ieee80211_ap_probereq_get(wl->hw, vif);
	if (!skb)
		return -EINVAL;

	ieoffset = offsetof(struct ieee80211_mgmt,
			    u.probe_req.variable);
	wl1271_ssid_set(wlvif, skb, ieoffset);
	dev_kfree_skb(skb);

	return 0;
}

static int wlcore_set_assoc(struct wl1271 *wl, struct wl12xx_vif *wlvif,
			    struct ieee80211_bss_conf *bss_conf,
			    u32 sta_rate_set)
{
	int ieoffset;
	int ret;

	wlvif->aid = bss_conf->aid;
	wlvif->channel_type = cfg80211_get_chandef_type(&bss_conf->chandef);
	wlvif->beacon_int = bss_conf->beacon_int;
	wlvif->wmm_enabled = bss_conf->qos;

	set_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags);

	/*
                                            
                                                    
                                                 
                          
  */
	ret = wl1271_cmd_build_ps_poll(wl, wlvif, wlvif->aid);
	if (ret < 0)
		return ret;

	/*
                                                      
  */
	dev_kfree_skb(wlvif->probereq);
	wlvif->probereq = wl1271_cmd_build_ap_probe_req(wl,
							wlvif,
							NULL);
	ieoffset = offsetof(struct ieee80211_mgmt,
			    u.probe_req.variable);
	wl1271_ssid_set(wlvif, wlvif->probereq, ieoffset);

	/*                                          */
	ret = wl1271_acx_conn_monit_params(wl, wlvif, true);
	if (ret < 0)
		return ret;

	/*
                                                                        
                                                                        
                                                                      
                                      
  */
	ret = wl1271_acx_keep_alive_mode(wl, wlvif, true);
	if (ret < 0)
		return ret;

	ret = wl1271_acx_aid(wl, wlvif, wlvif->aid);
	if (ret < 0)
		return ret;

	ret = wl12xx_cmd_build_klv_null_data(wl, wlvif);
	if (ret < 0)
		return ret;

	ret = wl1271_acx_keep_alive_config(wl, wlvif,
					   wlvif->sta.klv_template_id,
					   ACX_KEEP_ALIVE_TPL_VALID);
	if (ret < 0)
		return ret;

	/*
                                                                    
                                                                   
  */
	ret = wl1271_ps_set_mode(wl, wlvif, STATION_ACTIVE_MODE);
	if (ret < 0)
		return ret;

	if (sta_rate_set) {
		wlvif->rate_set =
			wl1271_tx_enabled_rates_get(wl,
						    sta_rate_set,
						    wlvif->band);
		ret = wl1271_acx_sta_rate_policies(wl, wlvif);
		if (ret < 0)
			return ret;
	}

	return ret;
}

static int wlcore_unset_assoc(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	int ret;
	bool sta = wlvif->bss_type == BSS_TYPE_STA_BSS;

	/*                                         */
	if (sta &&
	    !test_and_clear_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags))
		return false;

	/*                                */
	if (!sta &&
	    test_and_clear_bit(WLVIF_FLAG_IBSS_JOINED, &wlvif->flags))
		return false;

	if (sta) {
		/*                                  */
		wlvif->aid = 0;

		/*                             */
		dev_kfree_skb(wlvif->probereq);
		wlvif->probereq = NULL;

		/*                                     */
		ret = wl1271_acx_conn_monit_params(wl, wlvif, false);
		if (ret < 0)
			return ret;

		/*                                */
		ret = wl1271_acx_keep_alive_mode(wl, wlvif, false);
		if (ret < 0)
			return ret;
	}

	if (test_and_clear_bit(WLVIF_FLAG_CS_PROGRESS, &wlvif->flags)) {
		struct ieee80211_vif *vif = wl12xx_wlvif_to_vif(wlvif);

		wl12xx_cmd_stop_channel_switch(wl, wlvif);
		ieee80211_chswitch_done(vif, false);
		cancel_delayed_work(&wlvif->channel_switch_work);
	}

	/*                                */
	wl1271_acx_keep_alive_config(wl, wlvif,
				     wlvif->sta.klv_template_id,
				     ACX_KEEP_ALIVE_TPL_INVALID);

	return 0;
}

static void wl1271_set_band_rate(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	wlvif->basic_rate_set = wlvif->bitrate_masks[wlvif->band];
	wlvif->rate_set = wlvif->basic_rate_set;
}

static int wl12xx_config_vif(struct wl1271 *wl, struct wl12xx_vif *wlvif,
			     struct ieee80211_conf *conf, u32 changed)
{
	int ret;

	if (conf->power_level != wlvif->power_level) {
		ret = wl1271_acx_tx_power(wl, wlvif, conf->power_level);
		if (ret < 0)
			return ret;

		wlvif->power_level = conf->power_level;
	}

	return 0;
}

static int wl1271_op_config(struct ieee80211_hw *hw, u32 changed)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif;
	struct ieee80211_conf *conf = &hw->conf;
	int ret = 0;

	wl1271_debug(DEBUG_MAC80211, "mac80211 config psm %s power %d %s"
		     " changed 0x%x",
		     conf->flags & IEEE80211_CONF_PS ? "on" : "off",
		     conf->power_level,
		     conf->flags & IEEE80211_CONF_IDLE ? "idle" : "in use",
			 changed);

	mutex_lock(&wl->mutex);

	if (changed & IEEE80211_CONF_CHANGE_POWER)
		wl->power_level = conf->power_level;

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	/*                          */
	wl12xx_for_each_wlvif(wl, wlvif) {
		ret = wl12xx_config_vif(wl, wlvif, conf, changed);
		if (ret < 0)
			goto out_sleep;
	}

out_sleep:
	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);

	return ret;
}

struct wl1271_filter_params {
	bool enabled;
	int mc_list_length;
	u8 mc_list[ACX_MC_ADDRESS_GROUP_MAX][ETH_ALEN];
};

static u64 wl1271_op_prepare_multicast(struct ieee80211_hw *hw,
				       struct netdev_hw_addr_list *mc_list)
{
	struct wl1271_filter_params *fp;
	struct netdev_hw_addr *ha;

	fp = kzalloc(sizeof(*fp), GFP_ATOMIC);
	if (!fp) {
		wl1271_error("Out of memory setting filters.");
		return 0;
	}

	/*                                       */
	fp->mc_list_length = 0;
	if (netdev_hw_addr_list_count(mc_list) > ACX_MC_ADDRESS_GROUP_MAX) {
		fp->enabled = false;
	} else {
		fp->enabled = true;
		netdev_hw_addr_list_for_each(ha, mc_list) {
			memcpy(fp->mc_list[fp->mc_list_length],
					ha->addr, ETH_ALEN);
			fp->mc_list_length++;
		}
	}

	return (u64)(unsigned long)fp;
}

#define WL1271_SUPPORTED_FILTERS (FIF_PROMISC_IN_BSS | \
				  FIF_ALLMULTI | \
				  FIF_FCSFAIL | \
				  FIF_BCN_PRBRESP_PROMISC | \
				  FIF_CONTROL | \
				  FIF_OTHER_BSS)

static void wl1271_op_configure_filter(struct ieee80211_hw *hw,
				       unsigned int changed,
				       unsigned int *total, u64 multicast)
{
	struct wl1271_filter_params *fp = (void *)(unsigned long)multicast;
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif;

	int ret;

	wl1271_debug(DEBUG_MAC80211, "mac80211 configure filter changed %x"
		     " total %x", changed, *total);

	mutex_lock(&wl->mutex);

	*total &= WL1271_SUPPORTED_FILTERS;
	changed &= WL1271_SUPPORTED_FILTERS;

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	wl12xx_for_each_wlvif(wl, wlvif) {
		if (wlvif->bss_type != BSS_TYPE_AP_BSS) {
			if (*total & FIF_ALLMULTI)
				ret = wl1271_acx_group_address_tbl(wl, wlvif,
								   false,
								   NULL, 0);
			else if (fp)
				ret = wl1271_acx_group_address_tbl(wl, wlvif,
							fp->enabled,
							fp->mc_list,
							fp->mc_list_length);
			if (ret < 0)
				goto out_sleep;
		}
	}

	/*
                                                                    
                                                                
          
  */

out_sleep:
	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);
	kfree(fp);
}

static int wl1271_record_ap_key(struct wl1271 *wl, struct wl12xx_vif *wlvif,
				u8 id, u8 key_type, u8 key_size,
				const u8 *key, u8 hlid, u32 tx_seq_32,
				u16 tx_seq_16)
{
	struct wl1271_ap_key *ap_key;
	int i;

	wl1271_debug(DEBUG_CRYPT, "record ap key id %d", (int)id);

	if (key_size > MAX_KEY_SIZE)
		return -EINVAL;

	/*
                                                                    
                    
  */
	for (i = 0; i < MAX_NUM_KEYS; i++) {
		if (wlvif->ap.recorded_keys[i] == NULL)
			break;

		if (wlvif->ap.recorded_keys[i]->id == id) {
			wl1271_warning("trying to record key replacement");
			return -EINVAL;
		}
	}

	if (i == MAX_NUM_KEYS)
		return -EBUSY;

	ap_key = kzalloc(sizeof(*ap_key), GFP_KERNEL);
	if (!ap_key)
		return -ENOMEM;

	ap_key->id = id;
	ap_key->key_type = key_type;
	ap_key->key_size = key_size;
	memcpy(ap_key->key, key, key_size);
	ap_key->hlid = hlid;
	ap_key->tx_seq_32 = tx_seq_32;
	ap_key->tx_seq_16 = tx_seq_16;

	wlvif->ap.recorded_keys[i] = ap_key;
	return 0;
}

static void wl1271_free_ap_keys(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	int i;

	for (i = 0; i < MAX_NUM_KEYS; i++) {
		kfree(wlvif->ap.recorded_keys[i]);
		wlvif->ap.recorded_keys[i] = NULL;
	}
}

static int wl1271_ap_init_hwenc(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	int i, ret = 0;
	struct wl1271_ap_key *key;
	bool wep_key_added = false;

	for (i = 0; i < MAX_NUM_KEYS; i++) {
		u8 hlid;
		if (wlvif->ap.recorded_keys[i] == NULL)
			break;

		key = wlvif->ap.recorded_keys[i];
		hlid = key->hlid;
		if (hlid == WL12XX_INVALID_LINK_ID)
			hlid = wlvif->ap.bcast_hlid;

		ret = wl1271_cmd_set_ap_key(wl, wlvif, KEY_ADD_OR_REPLACE,
					    key->id, key->key_type,
					    key->key_size, key->key,
					    hlid, key->tx_seq_32,
					    key->tx_seq_16);
		if (ret < 0)
			goto out;

		if (key->key_type == KEY_WEP)
			wep_key_added = true;
	}

	if (wep_key_added) {
		ret = wl12xx_cmd_set_default_wep_key(wl, wlvif->default_key,
						     wlvif->ap.bcast_hlid);
		if (ret < 0)
			goto out;
	}

out:
	wl1271_free_ap_keys(wl, wlvif);
	return ret;
}

static int wl1271_set_key(struct wl1271 *wl, struct wl12xx_vif *wlvif,
		       u16 action, u8 id, u8 key_type,
		       u8 key_size, const u8 *key, u32 tx_seq_32,
		       u16 tx_seq_16, struct ieee80211_sta *sta)
{
	int ret;
	bool is_ap = (wlvif->bss_type == BSS_TYPE_AP_BSS);

	if (is_ap) {
		struct wl1271_station *wl_sta;
		u8 hlid;

		if (sta) {
			wl_sta = (struct wl1271_station *)sta->drv_priv;
			hlid = wl_sta->hlid;
		} else {
			hlid = wlvif->ap.bcast_hlid;
		}

		if (!test_bit(WLVIF_FLAG_AP_STARTED, &wlvif->flags)) {
			/*
                                                        
                                           
    */
			if (action != KEY_ADD_OR_REPLACE)
				return 0;

			ret = wl1271_record_ap_key(wl, wlvif, id,
					     key_type, key_size,
					     key, hlid, tx_seq_32,
					     tx_seq_16);
		} else {
			ret = wl1271_cmd_set_ap_key(wl, wlvif, action,
					     id, key_type, key_size,
					     key, hlid, tx_seq_32,
					     tx_seq_16);
		}

		if (ret < 0)
			return ret;
	} else {
		const u8 *addr;
		static const u8 bcast_addr[ETH_ALEN] = {
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff
		};

		addr = sta ? sta->addr : bcast_addr;

		if (is_zero_ether_addr(addr)) {
			/*                                    */
			return -EOPNOTSUPP;
		}

		/*                                                        
                                                               
                                                           
                       */
		if (action == KEY_REMOVE && !is_broadcast_ether_addr(addr))
			return 0;

		/*                                              */
		if (action == KEY_REMOVE &&
		    wlvif->sta.hlid == WL12XX_INVALID_LINK_ID)
			return 0;

		ret = wl1271_cmd_set_sta_key(wl, wlvif, action,
					     id, key_type, key_size,
					     key, addr, tx_seq_32,
					     tx_seq_16);
		if (ret < 0)
			return ret;

		/*                                                          */
		if (key_type == KEY_WEP) {
			ret = wl12xx_cmd_set_default_wep_key(wl,
							wlvif->default_key,
							wlvif->sta.hlid);
			if (ret < 0)
				return ret;
		}
	}

	return 0;
}

static int wlcore_op_set_key(struct ieee80211_hw *hw, enum set_key_cmd cmd,
			     struct ieee80211_vif *vif,
			     struct ieee80211_sta *sta,
			     struct ieee80211_key_conf *key_conf)
{
	struct wl1271 *wl = hw->priv;
	int ret;
	bool might_change_spare =
		key_conf->cipher == WL1271_CIPHER_SUITE_GEM ||
		key_conf->cipher == WLAN_CIPHER_SUITE_TKIP;

	if (might_change_spare) {
		/*
                                                             
                                           
   */
		wlcore_stop_queues(wl, WLCORE_QUEUE_STOP_REASON_SPARE_BLK);
		wl1271_tx_flush(wl);
	}

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON)) {
		ret = -EAGAIN;
		goto out_wake_queues;
	}

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out_wake_queues;

	ret = wlcore_hw_set_key(wl, cmd, vif, sta, key_conf);

	wl1271_ps_elp_sleep(wl);

out_wake_queues:
	if (might_change_spare)
		wlcore_wake_queues(wl, WLCORE_QUEUE_STOP_REASON_SPARE_BLK);

	mutex_unlock(&wl->mutex);

	return ret;
}

int wlcore_set_key(struct wl1271 *wl, enum set_key_cmd cmd,
		   struct ieee80211_vif *vif,
		   struct ieee80211_sta *sta,
		   struct ieee80211_key_conf *key_conf)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int ret;
	u32 tx_seq_32 = 0;
	u16 tx_seq_16 = 0;
	u8 key_type;
	u8 hlid;

	wl1271_debug(DEBUG_MAC80211, "mac80211 set key");

	wl1271_debug(DEBUG_CRYPT, "CMD: 0x%x sta: %p", cmd, sta);
	wl1271_debug(DEBUG_CRYPT, "Key: algo:0x%x, id:%d, len:%d flags 0x%x",
		     key_conf->cipher, key_conf->keyidx,
		     key_conf->keylen, key_conf->flags);
	wl1271_dump(DEBUG_CRYPT, "KEY: ", key_conf->key, key_conf->keylen);

	if (wlvif->bss_type == BSS_TYPE_AP_BSS)
		if (sta) {
			struct wl1271_station *wl_sta = (void *)sta->drv_priv;
			hlid = wl_sta->hlid;
		} else {
			hlid = wlvif->ap.bcast_hlid;
		}
	else
		hlid = wlvif->sta.hlid;

	if (hlid != WL12XX_INVALID_LINK_ID) {
		u64 tx_seq = wl->links[hlid].total_freed_pkts;
		tx_seq_32 = WL1271_TX_SECURITY_HI32(tx_seq);
		tx_seq_16 = WL1271_TX_SECURITY_LO16(tx_seq);
	}

	switch (key_conf->cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		key_type = KEY_WEP;

		key_conf->hw_key_idx = key_conf->keyidx;
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		key_type = KEY_TKIP;
		key_conf->hw_key_idx = key_conf->keyidx;
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		key_type = KEY_AES;
		key_conf->flags |= IEEE80211_KEY_FLAG_PUT_IV_SPACE;
		break;
	case WL1271_CIPHER_SUITE_GEM:
		key_type = KEY_GEM;
		break;
	default:
		wl1271_error("Unknown key algo 0x%x", key_conf->cipher);

		return -EOPNOTSUPP;
	}

	switch (cmd) {
	case SET_KEY:
		ret = wl1271_set_key(wl, wlvif, KEY_ADD_OR_REPLACE,
				 key_conf->keyidx, key_type,
				 key_conf->keylen, key_conf->key,
				 tx_seq_32, tx_seq_16, sta);
		if (ret < 0) {
			wl1271_error("Could not add or replace key");
			return ret;
		}

		/*
                                                          
                                    
   */
		if (wlvif->bss_type == BSS_TYPE_STA_BSS &&
		    (sta || key_type == KEY_WEP) &&
		    wlvif->encryption_type != key_type) {
			wlvif->encryption_type = key_type;
			ret = wl1271_cmd_build_arp_rsp(wl, wlvif);
			if (ret < 0) {
				wl1271_warning("build arp rsp failed: %d", ret);
				return ret;
			}
		}
		break;

	case DISABLE_KEY:
		ret = wl1271_set_key(wl, wlvif, KEY_REMOVE,
				     key_conf->keyidx, key_type,
				     key_conf->keylen, key_conf->key,
				     0, 0, sta);
		if (ret < 0) {
			wl1271_error("Could not remove key");
			return ret;
		}
		break;

	default:
		wl1271_error("Unsupported key cmd 0x%x", cmd);
		return -EOPNOTSUPP;
	}

	return ret;
}
EXPORT_SYMBOL_GPL(wlcore_set_key);

void wlcore_regdomain_config(struct wl1271 *wl)
{
	int ret;

	if (!(wl->quirks & WLCORE_QUIRK_REGDOMAIN_CONF))
		return;

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wlcore_cmd_regdomain_config_locked(wl);
	if (ret < 0) {
		wl12xx_queue_recovery_work(wl);
		goto out;
	}

	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);
}

static int wl1271_op_hw_scan(struct ieee80211_hw *hw,
			     struct ieee80211_vif *vif,
			     struct cfg80211_scan_request *req)
{
	struct wl1271 *wl = hw->priv;
	int ret;
	u8 *ssid = NULL;
	size_t len = 0;

	wl1271_debug(DEBUG_MAC80211, "mac80211 hw scan");

	if (req->n_ssids) {
		ssid = req->ssids[0].ssid;
		len = req->ssids[0].ssid_len;
	}

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON)) {
		/*
                                                              
                                                               
                             
   */
		ret = -EAGAIN;
		goto out;
	}

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	/*                                  */
	if (find_first_bit(wl->roc_map, WL12XX_MAX_ROLES) < WL12XX_MAX_ROLES) {
		/*                                */
		ret = -EBUSY;
		goto out_sleep;
	}

	ret = wlcore_scan(hw->priv, vif, ssid, len, req);
out_sleep:
	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);

	return ret;
}

static void wl1271_op_cancel_hw_scan(struct ieee80211_hw *hw,
				     struct ieee80211_vif *vif)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int ret;

	wl1271_debug(DEBUG_MAC80211, "mac80211 cancel hw scan");

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	if (wl->scan.state == WL1271_SCAN_STATE_IDLE)
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	if (wl->scan.state != WL1271_SCAN_STATE_DONE) {
		ret = wl->ops->scan_stop(wl, wlvif);
		if (ret < 0)
			goto out_sleep;
	}

	/*
                                                       
                                                             
  */
	wl12xx_rearm_tx_watchdog_locked(wl);

	wl->scan.state = WL1271_SCAN_STATE_IDLE;
	memset(wl->scan.scanned_ch, 0, sizeof(wl->scan.scanned_ch));
	wl->scan_wlvif = NULL;
	wl->scan.req = NULL;
	ieee80211_scan_completed(wl->hw, true);

out_sleep:
	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);

	cancel_delayed_work_sync(&wl->scan_complete_work);
}

static int wl1271_op_sched_scan_start(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif,
				      struct cfg80211_sched_scan_request *req,
				      struct ieee80211_sched_scan_ies *ies)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int ret;

	wl1271_debug(DEBUG_MAC80211, "wl1271_op_sched_scan_start");

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON)) {
		ret = -EAGAIN;
		goto out;
	}

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wl->ops->sched_scan_start(wl, wlvif, req, ies);
	if (ret < 0)
		goto out_sleep;

	wl->sched_vif = wlvif;

out_sleep:
	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);
	return ret;
}

static void wl1271_op_sched_scan_stop(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int ret;

	wl1271_debug(DEBUG_MAC80211, "wl1271_op_sched_scan_stop");

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	wl->ops->sched_scan_stop(wl, wlvif);

	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);
}

static int wl1271_op_set_frag_threshold(struct ieee80211_hw *hw, u32 value)
{
	struct wl1271 *wl = hw->priv;
	int ret = 0;

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON)) {
		ret = -EAGAIN;
		goto out;
	}

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wl1271_acx_frag_threshold(wl, value);
	if (ret < 0)
		wl1271_warning("wl1271_op_set_frag_threshold failed: %d", ret);

	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);

	return ret;
}

static int wl1271_op_set_rts_threshold(struct ieee80211_hw *hw, u32 value)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif;
	int ret = 0;

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON)) {
		ret = -EAGAIN;
		goto out;
	}

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	wl12xx_for_each_wlvif(wl, wlvif) {
		ret = wl1271_acx_rts_threshold(wl, wlvif, value);
		if (ret < 0)
			wl1271_warning("set rts threshold failed: %d", ret);
	}
	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);

	return ret;
}

static void wl12xx_remove_ie(struct sk_buff *skb, u8 eid, int ieoffset)
{
	int len;
	const u8 *next, *end = skb->data + skb->len;
	u8 *ie = (u8 *)cfg80211_find_ie(eid, skb->data + ieoffset,
					skb->len - ieoffset);
	if (!ie)
		return;
	len = ie[1] + 2;
	next = ie + len;
	memmove(ie, next, end - next);
	skb_trim(skb, skb->len - len);
}

static void wl12xx_remove_vendor_ie(struct sk_buff *skb,
					    unsigned int oui, u8 oui_type,
					    int ieoffset)
{
	int len;
	const u8 *next, *end = skb->data + skb->len;
	u8 *ie = (u8 *)cfg80211_find_vendor_ie(oui, oui_type,
					       skb->data + ieoffset,
					       skb->len - ieoffset);
	if (!ie)
		return;
	len = ie[1] + 2;
	next = ie + len;
	memmove(ie, next, end - next);
	skb_trim(skb, skb->len - len);
}

static int wl1271_ap_set_probe_resp_tmpl(struct wl1271 *wl, u32 rates,
					 struct ieee80211_vif *vif)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	struct sk_buff *skb;
	int ret;

	skb = ieee80211_proberesp_get(wl->hw, vif);
	if (!skb)
		return -EOPNOTSUPP;

	ret = wl1271_cmd_template_set(wl, wlvif->role_id,
				      CMD_TEMPL_AP_PROBE_RESPONSE,
				      skb->data,
				      skb->len, 0,
				      rates);
	dev_kfree_skb(skb);

	if (ret < 0)
		goto out;

	wl1271_debug(DEBUG_AP, "probe response updated");
	set_bit(WLVIF_FLAG_AP_PROBE_RESP_SET, &wlvif->flags);

out:
	return ret;
}

static int wl1271_ap_set_probe_resp_tmpl_legacy(struct wl1271 *wl,
					     struct ieee80211_vif *vif,
					     u8 *probe_rsp_data,
					     size_t probe_rsp_len,
					     u32 rates)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	struct ieee80211_bss_conf *bss_conf = &vif->bss_conf;
	u8 probe_rsp_templ[WL1271_CMD_TEMPL_MAX_SIZE];
	int ssid_ie_offset, ie_offset, templ_len;
	const u8 *ptr;

	/*                                                               */
	if (wlvif->ssid_len > 0)
		return wl1271_cmd_template_set(wl, wlvif->role_id,
					       CMD_TEMPL_AP_PROBE_RESPONSE,
					       probe_rsp_data,
					       probe_rsp_len, 0,
					       rates);

	if (probe_rsp_len + bss_conf->ssid_len > WL1271_CMD_TEMPL_MAX_SIZE) {
		wl1271_error("probe_rsp template too big");
		return -EINVAL;
	}

	/*                                */
	ie_offset = offsetof(struct ieee80211_mgmt, u.probe_resp.variable);

	ptr = cfg80211_find_ie(WLAN_EID_SSID, probe_rsp_data + ie_offset,
			       probe_rsp_len - ie_offset);
	if (!ptr) {
		wl1271_error("No SSID in beacon!");
		return -EINVAL;
	}

	ssid_ie_offset = ptr - probe_rsp_data;
	ptr += (ptr[1] + 2);

	memcpy(probe_rsp_templ, probe_rsp_data, ssid_ie_offset);

	/*                           */
	probe_rsp_templ[ssid_ie_offset] = WLAN_EID_SSID;
	probe_rsp_templ[ssid_ie_offset + 1] = bss_conf->ssid_len;
	memcpy(probe_rsp_templ + ssid_ie_offset + 2,
	       bss_conf->ssid, bss_conf->ssid_len);
	templ_len = ssid_ie_offset + 2 + bss_conf->ssid_len;

	memcpy(probe_rsp_templ + ssid_ie_offset + 2 + bss_conf->ssid_len,
	       ptr, probe_rsp_len - (ptr - probe_rsp_data));
	templ_len += probe_rsp_len - (ptr - probe_rsp_data);

	return wl1271_cmd_template_set(wl, wlvif->role_id,
				       CMD_TEMPL_AP_PROBE_RESPONSE,
				       probe_rsp_templ,
				       templ_len, 0,
				       rates);
}

static int wl1271_bss_erp_info_changed(struct wl1271 *wl,
				       struct ieee80211_vif *vif,
				       struct ieee80211_bss_conf *bss_conf,
				       u32 changed)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int ret = 0;

	if (changed & BSS_CHANGED_ERP_SLOT) {
		if (bss_conf->use_short_slot)
			ret = wl1271_acx_slot(wl, wlvif, SLOT_TIME_SHORT);
		else
			ret = wl1271_acx_slot(wl, wlvif, SLOT_TIME_LONG);
		if (ret < 0) {
			wl1271_warning("Set slot time failed %d", ret);
			goto out;
		}
	}

	if (changed & BSS_CHANGED_ERP_PREAMBLE) {
		if (bss_conf->use_short_preamble)
			wl1271_acx_set_preamble(wl, wlvif, ACX_PREAMBLE_SHORT);
		else
			wl1271_acx_set_preamble(wl, wlvif, ACX_PREAMBLE_LONG);
	}

	if (changed & BSS_CHANGED_ERP_CTS_PROT) {
		if (bss_conf->use_cts_prot)
			ret = wl1271_acx_cts_protect(wl, wlvif,
						     CTSPROTECT_ENABLE);
		else
			ret = wl1271_acx_cts_protect(wl, wlvif,
						     CTSPROTECT_DISABLE);
		if (ret < 0) {
			wl1271_warning("Set ctsprotect failed %d", ret);
			goto out;
		}
	}

out:
	return ret;
}

static int wlcore_set_beacon_template(struct wl1271 *wl,
				      struct ieee80211_vif *vif,
				      bool is_ap)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	struct ieee80211_hdr *hdr;
	u32 min_rate;
	int ret;
	int ieoffset = offsetof(struct ieee80211_mgmt,
				u.beacon.variable);
	struct sk_buff *beacon = ieee80211_beacon_get(wl->hw, vif);
	u16 tmpl_id;

	if (!beacon) {
		ret = -EINVAL;
		goto out;
	}

	wl1271_debug(DEBUG_MASTER, "beacon updated");

	ret = wl1271_ssid_set(wlvif, beacon, ieoffset);
	if (ret < 0) {
		dev_kfree_skb(beacon);
		goto out;
	}
	min_rate = wl1271_tx_min_rate_get(wl, wlvif->basic_rate_set);
	tmpl_id = is_ap ? CMD_TEMPL_AP_BEACON :
		CMD_TEMPL_BEACON;
	ret = wl1271_cmd_template_set(wl, wlvif->role_id, tmpl_id,
				      beacon->data,
				      beacon->len, 0,
				      min_rate);
	if (ret < 0) {
		dev_kfree_skb(beacon);
		goto out;
	}

	wlvif->wmm_enabled =
		cfg80211_find_vendor_ie(WLAN_OUI_MICROSOFT,
					WLAN_OUI_TYPE_MICROSOFT_WMM,
					beacon->data + ieoffset,
					beacon->len - ieoffset);

	/*
                                                              
                                           
  */
	if (test_bit(WLVIF_FLAG_AP_PROBE_RESP_SET, &wlvif->flags))
		goto end_bcn;

	/*                                   */
	wl12xx_remove_ie(beacon, WLAN_EID_TIM, ieoffset);

	/*
                                      
                                                       
                                                          
                                                     
                                                         
                                     
  */
	wl12xx_remove_vendor_ie(beacon, WLAN_OUI_WFA,
				WLAN_OUI_TYPE_WFA_P2P, ieoffset);

	hdr = (struct ieee80211_hdr *) beacon->data;
	hdr->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
					 IEEE80211_STYPE_PROBE_RESP);
	if (is_ap)
		ret = wl1271_ap_set_probe_resp_tmpl_legacy(wl, vif,
							   beacon->data,
							   beacon->len,
							   min_rate);
	else
		ret = wl1271_cmd_template_set(wl, wlvif->role_id,
					      CMD_TEMPL_PROBE_RESPONSE,
					      beacon->data,
					      beacon->len, 0,
					      min_rate);
end_bcn:
	dev_kfree_skb(beacon);
	if (ret < 0)
		goto out;

out:
	return ret;
}

static int wl1271_bss_beacon_info_changed(struct wl1271 *wl,
					  struct ieee80211_vif *vif,
					  struct ieee80211_bss_conf *bss_conf,
					  u32 changed)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	bool is_ap = (wlvif->bss_type == BSS_TYPE_AP_BSS);
	int ret = 0;

	if (changed & BSS_CHANGED_BEACON_INT) {
		wl1271_debug(DEBUG_MASTER, "beacon interval updated: %d",
			bss_conf->beacon_int);

		wlvif->beacon_int = bss_conf->beacon_int;
	}

	if ((changed & BSS_CHANGED_AP_PROBE_RESP) && is_ap) {
		u32 rate = wl1271_tx_min_rate_get(wl, wlvif->basic_rate_set);

		wl1271_ap_set_probe_resp_tmpl(wl, rate, vif);
	}

	if (changed & BSS_CHANGED_BEACON) {
		ret = wlcore_set_beacon_template(wl, vif, is_ap);
		if (ret < 0)
			goto out;
	}

out:
	if (ret != 0)
		wl1271_error("beacon info change failed: %d", ret);
	return ret;
}

/*                 */
static void wl1271_bss_info_changed_ap(struct wl1271 *wl,
				       struct ieee80211_vif *vif,
				       struct ieee80211_bss_conf *bss_conf,
				       u32 changed)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int ret = 0;

	if (changed & BSS_CHANGED_BASIC_RATES) {
		u32 rates = bss_conf->basic_rates;

		wlvif->basic_rate_set = wl1271_tx_enabled_rates_get(wl, rates,
								 wlvif->band);
		wlvif->basic_rate = wl1271_tx_min_rate_get(wl,
							wlvif->basic_rate_set);

		ret = wl1271_init_ap_rates(wl, wlvif);
		if (ret < 0) {
			wl1271_error("AP rate policy change failed %d", ret);
			goto out;
		}

		ret = wl1271_ap_init_templates(wl, vif);
		if (ret < 0)
			goto out;

		ret = wl1271_ap_set_probe_resp_tmpl(wl, wlvif->basic_rate, vif);
		if (ret < 0)
			goto out;

		ret = wlcore_set_beacon_template(wl, vif, true);
		if (ret < 0)
			goto out;
	}

	ret = wl1271_bss_beacon_info_changed(wl, vif, bss_conf, changed);
	if (ret < 0)
		goto out;

	if (changed & BSS_CHANGED_BEACON_ENABLED) {
		if (bss_conf->enable_beacon) {
			if (!test_bit(WLVIF_FLAG_AP_STARTED, &wlvif->flags)) {
				ret = wl12xx_cmd_role_start_ap(wl, wlvif);
				if (ret < 0)
					goto out;

				ret = wl1271_ap_init_hwenc(wl, wlvif);
				if (ret < 0)
					goto out;

				set_bit(WLVIF_FLAG_AP_STARTED, &wlvif->flags);
				wl1271_debug(DEBUG_AP, "started AP");
			}
		} else {
			if (test_bit(WLVIF_FLAG_AP_STARTED, &wlvif->flags)) {
				ret = wl12xx_cmd_role_stop_ap(wl, wlvif);
				if (ret < 0)
					goto out;

				clear_bit(WLVIF_FLAG_AP_STARTED, &wlvif->flags);
				clear_bit(WLVIF_FLAG_AP_PROBE_RESP_SET,
					  &wlvif->flags);
				wl1271_debug(DEBUG_AP, "stopped AP");
			}
		}
	}

	ret = wl1271_bss_erp_info_changed(wl, vif, bss_conf, changed);
	if (ret < 0)
		goto out;

	/*                              */
	if ((changed & BSS_CHANGED_HT) &&
	    (bss_conf->chandef.width != NL80211_CHAN_WIDTH_20_NOHT)) {
		ret = wl1271_acx_set_ht_information(wl, wlvif,
					bss_conf->ht_operation_mode);
		if (ret < 0) {
			wl1271_warning("Set ht information failed %d", ret);
			goto out;
		}
	}

out:
	return;
}

static int wlcore_set_bssid(struct wl1271 *wl, struct wl12xx_vif *wlvif,
			    struct ieee80211_bss_conf *bss_conf,
			    u32 sta_rate_set)
{
	u32 rates;
	int ret;

	wl1271_debug(DEBUG_MAC80211,
	     "changed_bssid: %pM, aid: %d, bcn_int: %d, brates: 0x%x sta_rate_set: 0x%x",
	     bss_conf->bssid, bss_conf->aid,
	     bss_conf->beacon_int,
	     bss_conf->basic_rates, sta_rate_set);

	wlvif->beacon_int = bss_conf->beacon_int;
	rates = bss_conf->basic_rates;
	wlvif->basic_rate_set =
		wl1271_tx_enabled_rates_get(wl, rates,
					    wlvif->band);
	wlvif->basic_rate =
		wl1271_tx_min_rate_get(wl,
				       wlvif->basic_rate_set);

	if (sta_rate_set)
		wlvif->rate_set =
			wl1271_tx_enabled_rates_get(wl,
						sta_rate_set,
						wlvif->band);

	/*                                                */
	if (wl->sched_vif == wlvif)
		wl->ops->sched_scan_stop(wl, wlvif);

	ret = wl1271_acx_sta_rate_policies(wl, wlvif);
	if (ret < 0)
		return ret;

	ret = wl12xx_cmd_build_null_data(wl, wlvif);
	if (ret < 0)
		return ret;

	ret = wl1271_build_qos_null_data(wl, wl12xx_wlvif_to_vif(wlvif));
	if (ret < 0)
		return ret;

	wlcore_set_ssid(wl, wlvif);

	set_bit(WLVIF_FLAG_IN_USE, &wlvif->flags);

	return 0;
}

static int wlcore_clear_bssid(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	int ret;

	/*                                                   */
	wl1271_set_band_rate(wl, wlvif);
	wlvif->basic_rate = wl1271_tx_min_rate_get(wl, wlvif->basic_rate_set);

	ret = wl1271_acx_sta_rate_policies(wl, wlvif);
	if (ret < 0)
		return ret;

	if (wlvif->bss_type == BSS_TYPE_STA_BSS &&
	    test_bit(WLVIF_FLAG_IN_USE, &wlvif->flags)) {
		ret = wl12xx_cmd_role_stop_sta(wl, wlvif);
		if (ret < 0)
			return ret;
	}

	clear_bit(WLVIF_FLAG_IN_USE, &wlvif->flags);
	return 0;
}
/*                       */
static void wl1271_bss_info_changed_sta(struct wl1271 *wl,
					struct ieee80211_vif *vif,
					struct ieee80211_bss_conf *bss_conf,
					u32 changed)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	bool do_join = false;
	bool is_ibss = (wlvif->bss_type == BSS_TYPE_IBSS);
	bool ibss_joined = false;
	u32 sta_rate_set = 0;
	int ret;
	struct ieee80211_sta *sta;
	bool sta_exists = false;
	struct ieee80211_sta_ht_cap sta_ht_cap;

	if (is_ibss) {
		ret = wl1271_bss_beacon_info_changed(wl, vif, bss_conf,
						     changed);
		if (ret < 0)
			goto out;
	}

	if (changed & BSS_CHANGED_IBSS) {
		if (bss_conf->ibss_joined) {
			set_bit(WLVIF_FLAG_IBSS_JOINED, &wlvif->flags);
			ibss_joined = true;
		} else {
			wlcore_unset_assoc(wl, wlvif);
			wl12xx_cmd_role_stop_sta(wl, wlvif);
		}
	}

	if ((changed & BSS_CHANGED_BEACON_INT) && ibss_joined)
		do_join = true;

	/*                                             */
	if ((changed & BSS_CHANGED_BEACON) && ibss_joined)
		do_join = true;

	if ((changed & BSS_CHANGED_BEACON_ENABLED) && ibss_joined) {
		wl1271_debug(DEBUG_ADHOC, "ad-hoc beaconing: %s",
			     bss_conf->enable_beacon ? "enabled" : "disabled");

		do_join = true;
	}

	if (changed & BSS_CHANGED_CQM) {
		bool enable = false;
		if (bss_conf->cqm_rssi_thold)
			enable = true;
		ret = wl1271_acx_rssi_snr_trigger(wl, wlvif, enable,
						  bss_conf->cqm_rssi_thold,
						  bss_conf->cqm_rssi_hyst);
		if (ret < 0)
			goto out;
		wlvif->rssi_thold = bss_conf->cqm_rssi_thold;
	}

	if (changed & (BSS_CHANGED_BSSID | BSS_CHANGED_HT |
		       BSS_CHANGED_ASSOC)) {
		rcu_read_lock();
		sta = ieee80211_find_sta(vif, bss_conf->bssid);
		if (sta) {
			u8 *rx_mask = sta->ht_cap.mcs.rx_mask;

			/*                               */
			sta_rate_set = sta->supp_rates[wlvif->band];
			if (sta->ht_cap.ht_supported)
				sta_rate_set |=
					(rx_mask[0] << HW_HT_RATES_OFFSET) |
					(rx_mask[1] << HW_MIMO_RATES_OFFSET);
			sta_ht_cap = sta->ht_cap;
			sta_exists = true;
		}

		rcu_read_unlock();
	}

	if (changed & BSS_CHANGED_BSSID) {
		if (!is_zero_ether_addr(bss_conf->bssid)) {
			ret = wlcore_set_bssid(wl, wlvif, bss_conf,
					       sta_rate_set);
			if (ret < 0)
				goto out;

			/*                                              */
			do_join = true;
		} else {
			ret = wlcore_clear_bssid(wl, wlvif);
			if (ret < 0)
				goto out;
		}
	}

	if (changed & BSS_CHANGED_IBSS) {
		wl1271_debug(DEBUG_ADHOC, "ibss_joined: %d",
			     bss_conf->ibss_joined);

		if (bss_conf->ibss_joined) {
			u32 rates = bss_conf->basic_rates;
			wlvif->basic_rate_set =
				wl1271_tx_enabled_rates_get(wl, rates,
							    wlvif->band);
			wlvif->basic_rate =
				wl1271_tx_min_rate_get(wl,
						       wlvif->basic_rate_set);

			/*                                  */
			wlvif->rate_set = CONF_TX_IBSS_DEFAULT_RATES;
			ret = wl1271_acx_sta_rate_policies(wl, wlvif);
			if (ret < 0)
				goto out;
		}
	}

	ret = wl1271_bss_erp_info_changed(wl, vif, bss_conf, changed);
	if (ret < 0)
		goto out;

	if (do_join) {
		ret = wlcore_join(wl, wlvif);
		if (ret < 0) {
			wl1271_warning("cmd join failed %d", ret);
			goto out;
		}
	}

	if (changed & BSS_CHANGED_ASSOC) {
		if (bss_conf->assoc) {
			ret = wlcore_set_assoc(wl, wlvif, bss_conf,
					       sta_rate_set);
			if (ret < 0)
				goto out;

			if (test_bit(WLVIF_FLAG_STA_AUTHORIZED, &wlvif->flags))
				wl12xx_set_authorized(wl, wlvif);
		} else {
			wlcore_unset_assoc(wl, wlvif);
		}
	}

	if (changed & BSS_CHANGED_PS) {
		if ((bss_conf->ps) &&
		    test_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags) &&
		    !test_bit(WLVIF_FLAG_IN_PS, &wlvif->flags)) {
			int ps_mode;
			char *ps_mode_str;

			if (wl->conf.conn.forced_ps) {
				ps_mode = STATION_POWER_SAVE_MODE;
				ps_mode_str = "forced";
			} else {
				ps_mode = STATION_AUTO_PS_MODE;
				ps_mode_str = "auto";
			}

			wl1271_debug(DEBUG_PSM, "%s ps enabled", ps_mode_str);

			ret = wl1271_ps_set_mode(wl, wlvif, ps_mode);
			if (ret < 0)
				wl1271_warning("enter %s ps failed %d",
					       ps_mode_str, ret);
		} else if (!bss_conf->ps &&
			   test_bit(WLVIF_FLAG_IN_PS, &wlvif->flags)) {
			wl1271_debug(DEBUG_PSM, "auto ps disabled");

			ret = wl1271_ps_set_mode(wl, wlvif,
						 STATION_ACTIVE_MODE);
			if (ret < 0)
				wl1271_warning("exit auto ps failed %d", ret);
		}
	}

	/*                                                     */
	if (sta_exists &&
	    (changed & BSS_CHANGED_HT)) {
		bool enabled =
			bss_conf->chandef.width != NL80211_CHAN_WIDTH_20_NOHT;

		ret = wlcore_hw_set_peer_cap(wl,
					     &sta_ht_cap,
					     enabled,
					     wlvif->rate_set,
					     wlvif->sta.hlid);
		if (ret < 0) {
			wl1271_warning("Set ht cap failed %d", ret);
			goto out;

		}

		if (enabled) {
			ret = wl1271_acx_set_ht_information(wl, wlvif,
						bss_conf->ht_operation_mode);
			if (ret < 0) {
				wl1271_warning("Set ht information failed %d",
					       ret);
				goto out;
			}
		}
	}

	/*                                        */
	if ((changed & BSS_CHANGED_ARP_FILTER) ||
	    (!is_ibss && (changed & BSS_CHANGED_QOS))) {
		__be32 addr = bss_conf->arp_addr_list[0];
		wlvif->sta.qos = bss_conf->qos;
		WARN_ON(wlvif->bss_type != BSS_TYPE_STA_BSS);

		if (bss_conf->arp_addr_cnt == 1 && bss_conf->assoc) {
			wlvif->ip_addr = addr;
			/*
                                                        
                                                        
                                                   
                                                    
    */
			ret = wl1271_cmd_build_arp_rsp(wl, wlvif);
			if (ret < 0) {
				wl1271_warning("build arp rsp failed: %d", ret);
				goto out;
			}

			ret = wl1271_acx_arp_ip_filter(wl, wlvif,
				(ACX_ARP_FILTER_ARP_FILTERING |
				 ACX_ARP_FILTER_AUTO_ARP),
				addr);
		} else {
			wlvif->ip_addr = 0;
			ret = wl1271_acx_arp_ip_filter(wl, wlvif, 0, addr);
		}

		if (ret < 0)
			goto out;
	}

out:
	return;
}

static void wl1271_op_bss_info_changed(struct ieee80211_hw *hw,
				       struct ieee80211_vif *vif,
				       struct ieee80211_bss_conf *bss_conf,
				       u32 changed)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	bool is_ap = (wlvif->bss_type == BSS_TYPE_AP_BSS);
	int ret;

	wl1271_debug(DEBUG_MAC80211, "mac80211 bss info role %d changed 0x%x",
		     wlvif->role_id, (int)changed);

	/*
                                                                 
                 
  */
	if (!is_ap && (changed & BSS_CHANGED_ASSOC))
		cancel_delayed_work_sync(&wlvif->connection_loss_work);

	if (is_ap && (changed & BSS_CHANGED_BEACON_ENABLED) &&
	    !bss_conf->enable_beacon)
		wl1271_tx_flush(wl);

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	if (unlikely(!test_bit(WLVIF_FLAG_INITIALIZED, &wlvif->flags)))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	if (is_ap)
		wl1271_bss_info_changed_ap(wl, vif, bss_conf, changed);
	else
		wl1271_bss_info_changed_sta(wl, vif, bss_conf, changed);

	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);
}

static int wlcore_op_add_chanctx(struct ieee80211_hw *hw,
				 struct ieee80211_chanctx_conf *ctx)
{
	wl1271_debug(DEBUG_MAC80211, "mac80211 add chanctx %d (type %d)",
		     ieee80211_frequency_to_channel(ctx->def.chan->center_freq),
		     cfg80211_get_chandef_type(&ctx->def));
	return 0;
}

static void wlcore_op_remove_chanctx(struct ieee80211_hw *hw,
				     struct ieee80211_chanctx_conf *ctx)
{
	wl1271_debug(DEBUG_MAC80211, "mac80211 remove chanctx %d (type %d)",
		     ieee80211_frequency_to_channel(ctx->def.chan->center_freq),
		     cfg80211_get_chandef_type(&ctx->def));
}

static void wlcore_op_change_chanctx(struct ieee80211_hw *hw,
				     struct ieee80211_chanctx_conf *ctx,
				     u32 changed)
{
	wl1271_debug(DEBUG_MAC80211,
		     "mac80211 change chanctx %d (type %d) changed 0x%x",
		     ieee80211_frequency_to_channel(ctx->def.chan->center_freq),
		     cfg80211_get_chandef_type(&ctx->def), changed);
}

static int wlcore_op_assign_vif_chanctx(struct ieee80211_hw *hw,
					struct ieee80211_vif *vif,
					struct ieee80211_chanctx_conf *ctx)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int channel = ieee80211_frequency_to_channel(
		ctx->def.chan->center_freq);

	wl1271_debug(DEBUG_MAC80211,
		     "mac80211 assign chanctx (role %d) %d (type %d)",
		     wlvif->role_id, channel, cfg80211_get_chandef_type(&ctx->def));

	mutex_lock(&wl->mutex);

	wlvif->band = ctx->def.chan->band;
	wlvif->channel = channel;
	wlvif->channel_type = cfg80211_get_chandef_type(&ctx->def);

	/*                                            */
	wl1271_set_band_rate(wl, wlvif);

	mutex_unlock(&wl->mutex);

	return 0;
}

static void wlcore_op_unassign_vif_chanctx(struct ieee80211_hw *hw,
					   struct ieee80211_vif *vif,
					   struct ieee80211_chanctx_conf *ctx)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);

	wl1271_debug(DEBUG_MAC80211,
		     "mac80211 unassign chanctx (role %d) %d (type %d)",
		     wlvif->role_id,
		     ieee80211_frequency_to_channel(ctx->def.chan->center_freq),
		     cfg80211_get_chandef_type(&ctx->def));

	wl1271_tx_flush(wl);
}

static int wl1271_op_conf_tx(struct ieee80211_hw *hw,
			     struct ieee80211_vif *vif, u16 queue,
			     const struct ieee80211_tx_queue_params *params)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	u8 ps_scheme;
	int ret = 0;

	mutex_lock(&wl->mutex);

	wl1271_debug(DEBUG_MAC80211, "mac80211 conf tx %d", queue);

	if (params->uapsd)
		ps_scheme = CONF_PS_SCHEME_UPSD_TRIGGER;
	else
		ps_scheme = CONF_PS_SCHEME_LEGACY;

	if (!test_bit(WLVIF_FLAG_INITIALIZED, &wlvif->flags))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	/*
                                                        
              
  */
	ret = wl1271_acx_ac_cfg(wl, wlvif, wl1271_tx_get_queue(queue),
				params->cw_min, params->cw_max,
				params->aifs, params->txop << 5);
	if (ret < 0)
		goto out_sleep;

	ret = wl1271_acx_tid_cfg(wl, wlvif, wl1271_tx_get_queue(queue),
				 CONF_CHANNEL_TYPE_EDCF,
				 wl1271_tx_get_queue(queue),
				 ps_scheme, CONF_ACK_POLICY_LEGACY,
				 0, 0);

out_sleep:
	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);

	return ret;
}

static u64 wl1271_op_get_tsf(struct ieee80211_hw *hw,
			     struct ieee80211_vif *vif)
{

	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	u64 mactime = ULLONG_MAX;
	int ret;

	wl1271_debug(DEBUG_MAC80211, "mac80211 get tsf");

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wl12xx_acx_tsf_info(wl, wlvif, &mactime);
	if (ret < 0)
		goto out_sleep;

out_sleep:
	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);
	return mactime;
}

static int wl1271_op_get_survey(struct ieee80211_hw *hw, int idx,
				struct survey_info *survey)
{
	struct ieee80211_conf *conf = &hw->conf;

	if (idx != 0)
		return -ENOENT;

	survey->channel = conf->chandef.chan;
	survey->filled = 0;
	return 0;
}

static int wl1271_allocate_sta(struct wl1271 *wl,
			     struct wl12xx_vif *wlvif,
			     struct ieee80211_sta *sta)
{
	struct wl1271_station *wl_sta;
	int ret;


	if (wl->active_sta_count >= AP_MAX_STATIONS) {
		wl1271_warning("could not allocate HLID - too much stations");
		return -EBUSY;
	}

	wl_sta = (struct wl1271_station *)sta->drv_priv;
	ret = wl12xx_allocate_link(wl, wlvif, &wl_sta->hlid);
	if (ret < 0) {
		wl1271_warning("could not allocate HLID - too many links");
		return -EBUSY;
	}

	/*                                                             */
	wl->links[wl_sta->hlid].total_freed_pkts = wl_sta->total_freed_pkts;

	set_bit(wl_sta->hlid, wlvif->ap.sta_hlid_map);
	memcpy(wl->links[wl_sta->hlid].addr, sta->addr, ETH_ALEN);
	wl->active_sta_count++;
	return 0;
}

void wl1271_free_sta(struct wl1271 *wl, struct wl12xx_vif *wlvif, u8 hlid)
{
	struct wl1271_station *wl_sta;
	struct ieee80211_sta *sta;
	struct ieee80211_vif *vif = wl12xx_wlvif_to_vif(wlvif);

	if (!test_bit(hlid, wlvif->ap.sta_hlid_map))
		return;

	clear_bit(hlid, wlvif->ap.sta_hlid_map);
	__clear_bit(hlid, &wl->ap_ps_map);
	__clear_bit(hlid, (unsigned long *)&wl->ap_fw_ps_map);

	/*
                                                              
                               
  */
	rcu_read_lock();
	sta = ieee80211_find_sta(vif, wl->links[hlid].addr);
	if (sta) {
		wl_sta = (void *)sta->drv_priv;
		wl_sta->total_freed_pkts = wl->links[hlid].total_freed_pkts;

		/*
                                                                
                                                                 
   */
		if (test_bit(WL1271_FLAG_RECOVERY_IN_PROGRESS, &wl->flags))
			wl_sta->total_freed_pkts +=
					WL1271_TX_SQN_POST_RECOVERY_PADDING;
	}
	rcu_read_unlock();

	wl12xx_free_link(wl, wlvif, &hlid);
	wl->active_sta_count--;

	/*
                                                                    
                                                             
  */
	if (wl->active_sta_count == 0)
		wl12xx_rearm_tx_watchdog_locked(wl);
}

static int wl12xx_sta_add(struct wl1271 *wl,
			  struct wl12xx_vif *wlvif,
			  struct ieee80211_sta *sta)
{
	struct wl1271_station *wl_sta;
	int ret = 0;
	u8 hlid;

	wl1271_debug(DEBUG_MAC80211, "mac80211 add sta %d", (int)sta->aid);

	ret = wl1271_allocate_sta(wl, wlvif, sta);
	if (ret < 0)
		return ret;

	wl_sta = (struct wl1271_station *)sta->drv_priv;
	hlid = wl_sta->hlid;

	ret = wl12xx_cmd_add_peer(wl, wlvif, sta, hlid);
	if (ret < 0)
		wl1271_free_sta(wl, wlvif, hlid);

	return ret;
}

static int wl12xx_sta_remove(struct wl1271 *wl,
			     struct wl12xx_vif *wlvif,
			     struct ieee80211_sta *sta)
{
	struct wl1271_station *wl_sta;
	int ret = 0, id;

	wl1271_debug(DEBUG_MAC80211, "mac80211 remove sta %d", (int)sta->aid);

	wl_sta = (struct wl1271_station *)sta->drv_priv;
	id = wl_sta->hlid;
	if (WARN_ON(!test_bit(id, wlvif->ap.sta_hlid_map)))
		return -EINVAL;

	ret = wl12xx_cmd_remove_peer(wl, wl_sta->hlid);
	if (ret < 0)
		return ret;

	wl1271_free_sta(wl, wlvif, wl_sta->hlid);
	return ret;
}

static void wlcore_roc_if_possible(struct wl1271 *wl,
				   struct wl12xx_vif *wlvif)
{
	if (find_first_bit(wl->roc_map,
			   WL12XX_MAX_ROLES) < WL12XX_MAX_ROLES)
		return;

	if (WARN_ON(wlvif->role_id == WL12XX_INVALID_ROLE_ID))
		return;

	wl12xx_roc(wl, wlvif, wlvif->role_id, wlvif->band, wlvif->channel);
}

static void wlcore_update_inconn_sta(struct wl1271 *wl,
				     struct wl12xx_vif *wlvif,
				     struct wl1271_station *wl_sta,
				     bool in_connection)
{
	if (in_connection) {
		if (WARN_ON(wl_sta->in_connection))
			return;
		wl_sta->in_connection = true;
		if (!wlvif->inconn_count++)
			wlcore_roc_if_possible(wl, wlvif);
	} else {
		if (!wl_sta->in_connection)
			return;

		wl_sta->in_connection = false;
		wlvif->inconn_count--;
		if (WARN_ON(wlvif->inconn_count < 0))
			return;

		if (!wlvif->inconn_count)
			if (test_bit(wlvif->role_id, wl->roc_map))
				wl12xx_croc(wl, wlvif->role_id);
	}
}

static int wl12xx_update_sta_state(struct wl1271 *wl,
				   struct wl12xx_vif *wlvif,
				   struct ieee80211_sta *sta,
				   enum ieee80211_sta_state old_state,
				   enum ieee80211_sta_state new_state)
{
	struct wl1271_station *wl_sta;
	bool is_ap = wlvif->bss_type == BSS_TYPE_AP_BSS;
	bool is_sta = wlvif->bss_type == BSS_TYPE_STA_BSS;
	int ret;

	wl_sta = (struct wl1271_station *)sta->drv_priv;

	/*                       */
	if (is_ap &&
	    old_state == IEEE80211_STA_NOTEXIST &&
	    new_state == IEEE80211_STA_NONE) {
		ret = wl12xx_sta_add(wl, wlvif, sta);
		if (ret)
			return ret;

		wlcore_update_inconn_sta(wl, wlvif, wl_sta, true);
	}

	/*                          */
	if (is_ap &&
	    old_state == IEEE80211_STA_NONE &&
	    new_state == IEEE80211_STA_NOTEXIST) {
		/*               */
		wl12xx_sta_remove(wl, wlvif, sta);

		wlcore_update_inconn_sta(wl, wlvif, wl_sta, false);
	}

	/*                             */
	if (is_ap &&
	    new_state == IEEE80211_STA_AUTHORIZED) {
		ret = wl12xx_cmd_set_peer_state(wl, wlvif, wl_sta->hlid);
		if (ret < 0)
			return ret;

		ret = wl1271_acx_set_ht_capabilities(wl, &sta->ht_cap, true,
						     wl_sta->hlid);
		if (ret)
			return ret;

		wlcore_update_inconn_sta(wl, wlvif, wl_sta, false);
	}

	/*                   */
	if (is_sta &&
	    new_state == IEEE80211_STA_AUTHORIZED) {
		set_bit(WLVIF_FLAG_STA_AUTHORIZED, &wlvif->flags);
		ret = wl12xx_set_authorized(wl, wlvif);
		if (ret)
			return ret;
	}

	if (is_sta &&
	    old_state == IEEE80211_STA_AUTHORIZED &&
	    new_state == IEEE80211_STA_ASSOC) {
		clear_bit(WLVIF_FLAG_STA_AUTHORIZED, &wlvif->flags);
		clear_bit(WLVIF_FLAG_STA_STATE_SENT, &wlvif->flags);
	}

	/*                                        */
	if (is_sta &&
	    (new_state == IEEE80211_STA_AUTHORIZED ||
	     new_state == IEEE80211_STA_NOTEXIST)) {
		if (test_bit(wlvif->role_id, wl->roc_map))
			wl12xx_croc(wl, wlvif->role_id);
	}

	if (is_sta &&
	    old_state == IEEE80211_STA_NOTEXIST &&
	    new_state == IEEE80211_STA_NONE) {
		if (find_first_bit(wl->roc_map,
				   WL12XX_MAX_ROLES) >= WL12XX_MAX_ROLES) {
			WARN_ON(wlvif->role_id == WL12XX_INVALID_ROLE_ID);
			wl12xx_roc(wl, wlvif, wlvif->role_id,
				   wlvif->band, wlvif->channel);
		}
	}
	return 0;
}

static int wl12xx_op_sta_state(struct ieee80211_hw *hw,
			       struct ieee80211_vif *vif,
			       struct ieee80211_sta *sta,
			       enum ieee80211_sta_state old_state,
			       enum ieee80211_sta_state new_state)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int ret;

	wl1271_debug(DEBUG_MAC80211, "mac80211 sta %d state=%d->%d",
		     sta->aid, old_state, new_state);

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON)) {
		ret = -EBUSY;
		goto out;
	}

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wl12xx_update_sta_state(wl, wlvif, sta, old_state, new_state);

	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);
	if (new_state < old_state)
		return 0;
	return ret;
}

static int wl1271_op_ampdu_action(struct ieee80211_hw *hw,
				  struct ieee80211_vif *vif,
				  enum ieee80211_ampdu_mlme_action action,
				  struct ieee80211_sta *sta, u16 tid, u16 *ssn,
				  u8 buf_size)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int ret;
	u8 hlid, *ba_bitmap;

	wl1271_debug(DEBUG_MAC80211, "mac80211 ampdu action %d tid %d", action,
		     tid);

	/*                                                     */
	if (WARN_ON(tid > 0xFF))
		return -ENOTSUPP;

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON)) {
		ret = -EAGAIN;
		goto out;
	}

	if (wlvif->bss_type == BSS_TYPE_STA_BSS) {
		hlid = wlvif->sta.hlid;
	} else if (wlvif->bss_type == BSS_TYPE_AP_BSS) {
		struct wl1271_station *wl_sta;

		wl_sta = (struct wl1271_station *)sta->drv_priv;
		hlid = wl_sta->hlid;
	} else {
		ret = -EINVAL;
		goto out;
	}

	ba_bitmap = &wl->links[hlid].ba_bitmap;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	wl1271_debug(DEBUG_MAC80211, "mac80211 ampdu: Rx tid %d action %d",
		     tid, action);

	switch (action) {
	case IEEE80211_AMPDU_RX_START:
		if (!wlvif->ba_support || !wlvif->ba_allowed) {
			ret = -ENOTSUPP;
			break;
		}

		if (wl->ba_rx_session_count >= wl->ba_rx_session_count_max) {
			ret = -EBUSY;
			wl1271_error("exceeded max RX BA sessions");
			break;
		}

		if (*ba_bitmap & BIT(tid)) {
			ret = -EINVAL;
			wl1271_error("cannot enable RX BA session on active "
				     "tid: %d", tid);
			break;
		}

		ret = wl12xx_acx_set_ba_receiver_session(wl, tid, *ssn, true,
							 hlid);
		if (!ret) {
			*ba_bitmap |= BIT(tid);
			wl->ba_rx_session_count++;
		}
		break;

	case IEEE80211_AMPDU_RX_STOP:
		if (!(*ba_bitmap & BIT(tid))) {
			/*
                                                       
                                                   
    */
			wl1271_debug(DEBUG_MAC80211,
				     "no active RX BA session on tid: %d",
				     tid);
			ret = 0;
			break;
		}

		ret = wl12xx_acx_set_ba_receiver_session(wl, tid, 0, false,
							 hlid);
		if (!ret) {
			*ba_bitmap &= ~BIT(tid);
			wl->ba_rx_session_count--;
		}
		break;

	/*
                                                            
                                                           
  */
	case IEEE80211_AMPDU_TX_START:
	case IEEE80211_AMPDU_TX_STOP_CONT:
	case IEEE80211_AMPDU_TX_STOP_FLUSH:
	case IEEE80211_AMPDU_TX_STOP_FLUSH_CONT:
	case IEEE80211_AMPDU_TX_OPERATIONAL:
		ret = -EINVAL;
		break;

	default:
		wl1271_error("Incorrect ampdu action id=%x\n", action);
		ret = -EINVAL;
	}

	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);

	return ret;
}

static int wl12xx_set_bitrate_mask(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif,
				   const struct cfg80211_bitrate_mask *mask)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	struct wl1271 *wl = hw->priv;
	int i, ret = 0;

	wl1271_debug(DEBUG_MAC80211, "mac80211 set_bitrate_mask 0x%x 0x%x",
		mask->control[NL80211_BAND_2GHZ].legacy,
		mask->control[NL80211_BAND_5GHZ].legacy);

	mutex_lock(&wl->mutex);

	for (i = 0; i < WLCORE_NUM_BANDS; i++)
		wlvif->bitrate_masks[i] =
			wl1271_tx_enabled_rates_get(wl,
						    mask->control[i].legacy,
						    i);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	if (wlvif->bss_type == BSS_TYPE_STA_BSS &&
	    !test_bit(WLVIF_FLAG_STA_ASSOCIATED, &wlvif->flags)) {

		ret = wl1271_ps_elp_wakeup(wl);
		if (ret < 0)
			goto out;

		wl1271_set_band_rate(wl, wlvif);
		wlvif->basic_rate =
			wl1271_tx_min_rate_get(wl, wlvif->basic_rate_set);
		ret = wl1271_acx_sta_rate_policies(wl, wlvif);

		wl1271_ps_elp_sleep(wl);
	}
out:
	mutex_unlock(&wl->mutex);

	return ret;
}

static void wl12xx_op_channel_switch(struct ieee80211_hw *hw,
				     struct ieee80211_channel_switch *ch_switch)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif;
	int ret;

	wl1271_debug(DEBUG_MAC80211, "mac80211 channel switch");

	wl1271_tx_flush(wl);

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state == WLCORE_STATE_OFF)) {
		wl12xx_for_each_wlvif_sta(wl, wlvif) {
			struct ieee80211_vif *vif = wl12xx_wlvif_to_vif(wlvif);
			ieee80211_chswitch_done(vif, false);
		}
		goto out;
	} else if (unlikely(wl->state != WLCORE_STATE_ON)) {
		goto out;
	}

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	/*                                            */
	wl12xx_for_each_wlvif_sta(wl, wlvif) {
		unsigned long delay_usec;

		ret = wl->ops->channel_switch(wl, wlvif, ch_switch);
		if (ret)
			goto out_sleep;

		set_bit(WLVIF_FLAG_CS_PROGRESS, &wlvif->flags);

		/*                                                      */
		delay_usec = ieee80211_tu_to_usec(wlvif->beacon_int) *
			     ch_switch->count;
		ieee80211_queue_delayed_work(hw, &wlvif->channel_switch_work,
				usecs_to_jiffies(delay_usec) +
				msecs_to_jiffies(5000));
	}

out_sleep:
	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);
}

static void wlcore_op_flush(struct ieee80211_hw *hw, u32 queues, bool drop)
{
	struct wl1271 *wl = hw->priv;

	wl1271_tx_flush(wl);
}

static int wlcore_op_remain_on_channel(struct ieee80211_hw *hw,
				       struct ieee80211_vif *vif,
				       struct ieee80211_channel *chan,
				       int duration,
				       enum ieee80211_roc_type type)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	struct wl1271 *wl = hw->priv;
	int channel, ret = 0;

	channel = ieee80211_frequency_to_channel(chan->center_freq);

	wl1271_debug(DEBUG_MAC80211, "mac80211 roc %d (%d)",
		     channel, wlvif->role_id);

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	/*                                        */
	if (WARN_ON(wl->roc_vif ||
		    find_first_bit(wl->roc_map,
				   WL12XX_MAX_ROLES) < WL12XX_MAX_ROLES)) {
		ret = -EBUSY;
		goto out;
	}

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = wl12xx_start_dev(wl, wlvif, chan->band, channel);
	if (ret < 0)
		goto out_sleep;

	wl->roc_vif = vif;
	ieee80211_queue_delayed_work(hw, &wl->roc_complete_work,
				     msecs_to_jiffies(duration));
out_sleep:
	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);
	return ret;
}

static int __wlcore_roc_completed(struct wl1271 *wl)
{
	struct wl12xx_vif *wlvif;
	int ret;

	/*                   */
	if (unlikely(!wl->roc_vif))
		return 0;

	wlvif = wl12xx_vif_to_data(wl->roc_vif);

	if (!test_bit(WLVIF_FLAG_INITIALIZED, &wlvif->flags))
		return -EBUSY;

	ret = wl12xx_stop_dev(wl, wlvif);
	if (ret < 0)
		return ret;

	wl->roc_vif = NULL;

	return 0;
}

static int wlcore_roc_completed(struct wl1271 *wl)
{
	int ret;

	wl1271_debug(DEBUG_MAC80211, "roc complete");

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON)) {
		ret = -EBUSY;
		goto out;
	}

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	ret = __wlcore_roc_completed(wl);

	wl1271_ps_elp_sleep(wl);
out:
	mutex_unlock(&wl->mutex);

	return ret;
}

static void wlcore_roc_complete_work(struct work_struct *work)
{
	struct delayed_work *dwork;
	struct wl1271 *wl;
	int ret;

	dwork = container_of(work, struct delayed_work, work);
	wl = container_of(dwork, struct wl1271, roc_complete_work);

	ret = wlcore_roc_completed(wl);
	if (!ret)
		ieee80211_remain_on_channel_expired(wl->hw);
}

static int wlcore_op_cancel_remain_on_channel(struct ieee80211_hw *hw)
{
	struct wl1271 *wl = hw->priv;

	wl1271_debug(DEBUG_MAC80211, "mac80211 croc");

	/*               */
	wl1271_tx_flush(wl);

	/*
                                                            
                                                    
  */
	cancel_delayed_work_sync(&wl->roc_complete_work);
	wlcore_roc_completed(wl);

	return 0;
}

static void wlcore_op_sta_rc_update(struct ieee80211_hw *hw,
				    struct ieee80211_vif *vif,
				    struct ieee80211_sta *sta,
				    u32 changed)
{
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	struct wl1271 *wl = hw->priv;

	wlcore_hw_sta_rc_update(wl, wlvif, sta, changed);
}

static int wlcore_op_get_rssi(struct ieee80211_hw *hw,
			       struct ieee80211_vif *vif,
			       struct ieee80211_sta *sta,
			       s8 *rssi_dbm)
{
	struct wl1271 *wl = hw->priv;
	struct wl12xx_vif *wlvif = wl12xx_vif_to_data(vif);
	int ret = 0;

	wl1271_debug(DEBUG_MAC80211, "mac80211 get_rssi");

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out_sleep;

	ret = wlcore_acx_average_rssi(wl, wlvif, rssi_dbm);
	if (ret < 0)
		goto out_sleep;

out_sleep:
	wl1271_ps_elp_sleep(wl);

out:
	mutex_unlock(&wl->mutex);

	return ret;
}

static bool wl1271_tx_frames_pending(struct ieee80211_hw *hw)
{
	struct wl1271 *wl = hw->priv;
	bool ret = false;

	mutex_lock(&wl->mutex);

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	/*                                                             */
	ret = (wl1271_tx_total_queue_count(wl) > 0) || (wl->tx_frames_cnt > 0);
out:
	mutex_unlock(&wl->mutex);

	return ret;
}

/*                                         */
static struct ieee80211_rate wl1271_rates[] = {
	{ .bitrate = 10,
	  .hw_value = CONF_HW_BIT_RATE_1MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_1MBPS, },
	{ .bitrate = 20,
	  .hw_value = CONF_HW_BIT_RATE_2MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_2MBPS,
	  .flags = IEEE80211_RATE_SHORT_PREAMBLE },
	{ .bitrate = 55,
	  .hw_value = CONF_HW_BIT_RATE_5_5MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_5_5MBPS,
	  .flags = IEEE80211_RATE_SHORT_PREAMBLE },
	{ .bitrate = 110,
	  .hw_value = CONF_HW_BIT_RATE_11MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_11MBPS,
	  .flags = IEEE80211_RATE_SHORT_PREAMBLE },
	{ .bitrate = 60,
	  .hw_value = CONF_HW_BIT_RATE_6MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_6MBPS, },
	{ .bitrate = 90,
	  .hw_value = CONF_HW_BIT_RATE_9MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_9MBPS, },
	{ .bitrate = 120,
	  .hw_value = CONF_HW_BIT_RATE_12MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_12MBPS, },
	{ .bitrate = 180,
	  .hw_value = CONF_HW_BIT_RATE_18MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_18MBPS, },
	{ .bitrate = 240,
	  .hw_value = CONF_HW_BIT_RATE_24MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_24MBPS, },
	{ .bitrate = 360,
	 .hw_value = CONF_HW_BIT_RATE_36MBPS,
	 .hw_value_short = CONF_HW_BIT_RATE_36MBPS, },
	{ .bitrate = 480,
	  .hw_value = CONF_HW_BIT_RATE_48MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_48MBPS, },
	{ .bitrate = 540,
	  .hw_value = CONF_HW_BIT_RATE_54MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_54MBPS, },
};

/*                                         */
static struct ieee80211_channel wl1271_channels[] = {
	{ .hw_value = 1, .center_freq = 2412, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 2, .center_freq = 2417, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 3, .center_freq = 2422, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 4, .center_freq = 2427, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 5, .center_freq = 2432, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 6, .center_freq = 2437, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 7, .center_freq = 2442, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 8, .center_freq = 2447, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 9, .center_freq = 2452, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 10, .center_freq = 2457, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 11, .center_freq = 2462, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 12, .center_freq = 2467, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 13, .center_freq = 2472, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 14, .center_freq = 2484, .max_power = WLCORE_MAX_TXPWR },
};

/*                                         */
static struct ieee80211_supported_band wl1271_band_2ghz = {
	.channels = wl1271_channels,
	.n_channels = ARRAY_SIZE(wl1271_channels),
	.bitrates = wl1271_rates,
	.n_bitrates = ARRAY_SIZE(wl1271_rates),
};

/*                             */
static struct ieee80211_rate wl1271_rates_5ghz[] = {
	{ .bitrate = 60,
	  .hw_value = CONF_HW_BIT_RATE_6MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_6MBPS, },
	{ .bitrate = 90,
	  .hw_value = CONF_HW_BIT_RATE_9MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_9MBPS, },
	{ .bitrate = 120,
	  .hw_value = CONF_HW_BIT_RATE_12MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_12MBPS, },
	{ .bitrate = 180,
	  .hw_value = CONF_HW_BIT_RATE_18MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_18MBPS, },
	{ .bitrate = 240,
	  .hw_value = CONF_HW_BIT_RATE_24MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_24MBPS, },
	{ .bitrate = 360,
	 .hw_value = CONF_HW_BIT_RATE_36MBPS,
	 .hw_value_short = CONF_HW_BIT_RATE_36MBPS, },
	{ .bitrate = 480,
	  .hw_value = CONF_HW_BIT_RATE_48MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_48MBPS, },
	{ .bitrate = 540,
	  .hw_value = CONF_HW_BIT_RATE_54MBPS,
	  .hw_value_short = CONF_HW_BIT_RATE_54MBPS, },
};

/*                                */
static struct ieee80211_channel wl1271_channels_5ghz[] = {
	{ .hw_value = 7, .center_freq = 5035, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 8, .center_freq = 5040, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 9, .center_freq = 5045, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 11, .center_freq = 5055, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 12, .center_freq = 5060, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 16, .center_freq = 5080, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 34, .center_freq = 5170, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 36, .center_freq = 5180, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 38, .center_freq = 5190, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 40, .center_freq = 5200, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 42, .center_freq = 5210, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 44, .center_freq = 5220, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 46, .center_freq = 5230, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 48, .center_freq = 5240, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 52, .center_freq = 5260, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 56, .center_freq = 5280, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 60, .center_freq = 5300, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 64, .center_freq = 5320, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 100, .center_freq = 5500, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 104, .center_freq = 5520, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 108, .center_freq = 5540, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 112, .center_freq = 5560, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 116, .center_freq = 5580, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 120, .center_freq = 5600, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 124, .center_freq = 5620, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 128, .center_freq = 5640, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 132, .center_freq = 5660, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 136, .center_freq = 5680, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 140, .center_freq = 5700, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 149, .center_freq = 5745, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 153, .center_freq = 5765, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 157, .center_freq = 5785, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 161, .center_freq = 5805, .max_power = WLCORE_MAX_TXPWR },
	{ .hw_value = 165, .center_freq = 5825, .max_power = WLCORE_MAX_TXPWR },
};

static struct ieee80211_supported_band wl1271_band_5ghz = {
	.channels = wl1271_channels_5ghz,
	.n_channels = ARRAY_SIZE(wl1271_channels_5ghz),
	.bitrates = wl1271_rates_5ghz,
	.n_bitrates = ARRAY_SIZE(wl1271_rates_5ghz),
};

static const struct ieee80211_ops wl1271_ops = {
	.start = wl1271_op_start,
	.stop = wlcore_op_stop,
	.add_interface = wl1271_op_add_interface,
	.remove_interface = wl1271_op_remove_interface,
	.change_interface = wl12xx_op_change_interface,
#ifdef CONFIG_PM
	.suspend = wl1271_op_suspend,
	.resume = wl1271_op_resume,
#endif
	.config = wl1271_op_config,
	.prepare_multicast = wl1271_op_prepare_multicast,
	.configure_filter = wl1271_op_configure_filter,
	.tx = wl1271_op_tx,
	.set_key = wlcore_op_set_key,
	.hw_scan = wl1271_op_hw_scan,
	.cancel_hw_scan = wl1271_op_cancel_hw_scan,
	.sched_scan_start = wl1271_op_sched_scan_start,
	.sched_scan_stop = wl1271_op_sched_scan_stop,
	.bss_info_changed = wl1271_op_bss_info_changed,
	.set_frag_threshold = wl1271_op_set_frag_threshold,
	.set_rts_threshold = wl1271_op_set_rts_threshold,
	.conf_tx = wl1271_op_conf_tx,
	.get_tsf = wl1271_op_get_tsf,
	.get_survey = wl1271_op_get_survey,
	.sta_state = wl12xx_op_sta_state,
	.ampdu_action = wl1271_op_ampdu_action,
	.tx_frames_pending = wl1271_tx_frames_pending,
	.set_bitrate_mask = wl12xx_set_bitrate_mask,
	.channel_switch = wl12xx_op_channel_switch,
	.flush = wlcore_op_flush,
	.remain_on_channel = wlcore_op_remain_on_channel,
	.cancel_remain_on_channel = wlcore_op_cancel_remain_on_channel,
	.add_chanctx = wlcore_op_add_chanctx,
	.remove_chanctx = wlcore_op_remove_chanctx,
	.change_chanctx = wlcore_op_change_chanctx,
	.assign_vif_chanctx = wlcore_op_assign_vif_chanctx,
	.unassign_vif_chanctx = wlcore_op_unassign_vif_chanctx,
	.sta_rc_update = wlcore_op_sta_rc_update,
	.get_rssi = wlcore_op_get_rssi,
	CFG80211_TESTMODE_CMD(wl1271_tm_cmd)
};


u8 wlcore_rate_to_idx(struct wl1271 *wl, u8 rate, enum ieee80211_band band)
{
	u8 idx;

	BUG_ON(band >= 2);

	if (unlikely(rate >= wl->hw_tx_rate_tbl_size)) {
		wl1271_error("Illegal RX rate from HW: %d", rate);
		return 0;
	}

	idx = wl->band_rate_to_idx[band][rate];
	if (unlikely(idx == CONF_HW_RXTX_RATE_UNSUPPORTED)) {
		wl1271_error("Unsupported RX rate from HW: %d", rate);
		return 0;
	}

	return idx;
}

static ssize_t wl1271_sysfs_show_bt_coex_state(struct device *dev,
					       struct device_attribute *attr,
					       char *buf)
{
	struct wl1271 *wl = dev_get_drvdata(dev);
	ssize_t len;

	len = PAGE_SIZE;

	mutex_lock(&wl->mutex);
	len = snprintf(buf, len, "%d\n\n0 - off\n1 - on\n",
		       wl->sg_enabled);
	mutex_unlock(&wl->mutex);

	return len;

}

static ssize_t wl1271_sysfs_store_bt_coex_state(struct device *dev,
						struct device_attribute *attr,
						const char *buf, size_t count)
{
	struct wl1271 *wl = dev_get_drvdata(dev);
	unsigned long res;
	int ret;

	ret = kstrtoul(buf, 10, &res);
	if (ret < 0) {
		wl1271_warning("incorrect value written to bt_coex_mode");
		return count;
	}

	mutex_lock(&wl->mutex);

	res = !!res;

	if (res == wl->sg_enabled)
		goto out;

	wl->sg_enabled = res;

	if (unlikely(wl->state != WLCORE_STATE_ON))
		goto out;

	ret = wl1271_ps_elp_wakeup(wl);
	if (ret < 0)
		goto out;

	wl1271_acx_sg_enable(wl, wl->sg_enabled);
	wl1271_ps_elp_sleep(wl);

 out:
	mutex_unlock(&wl->mutex);
	return count;
}

static DEVICE_ATTR(bt_coex_state, S_IRUGO | S_IWUSR,
		   wl1271_sysfs_show_bt_coex_state,
		   wl1271_sysfs_store_bt_coex_state);

static ssize_t wl1271_sysfs_show_hw_pg_ver(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	struct wl1271 *wl = dev_get_drvdata(dev);
	ssize_t len;

	len = PAGE_SIZE;

	mutex_lock(&wl->mutex);
	if (wl->hw_pg_ver >= 0)
		len = snprintf(buf, len, "%d\n", wl->hw_pg_ver);
	else
		len = snprintf(buf, len, "n/a\n");
	mutex_unlock(&wl->mutex);

	return len;
}

static DEVICE_ATTR(hw_pg_ver, S_IRUGO,
		   wl1271_sysfs_show_hw_pg_ver, NULL);

static ssize_t wl1271_sysfs_read_fwlog(struct file *filp, struct kobject *kobj,
				       struct bin_attribute *bin_attr,
				       char *buffer, loff_t pos, size_t count)
{
	struct device *dev = container_of(kobj, struct device, kobj);
	struct wl1271 *wl = dev_get_drvdata(dev);
	ssize_t len;
	int ret;

	ret = mutex_lock_interruptible(&wl->mutex);
	if (ret < 0)
		return -ERESTARTSYS;

	/*                                                             */
	while (wl->fwlog_size == 0) {
		DEFINE_WAIT(wait);

		prepare_to_wait_exclusive(&wl->fwlog_waitq,
					  &wait,
					  TASK_INTERRUPTIBLE);

		if (wl->fwlog_size != 0) {
			finish_wait(&wl->fwlog_waitq, &wait);
			break;
		}

		mutex_unlock(&wl->mutex);

		schedule();
		finish_wait(&wl->fwlog_waitq, &wait);

		if (signal_pending(current))
			return -ERESTARTSYS;

		ret = mutex_lock_interruptible(&wl->mutex);
		if (ret < 0)
			return -ERESTARTSYS;
	}

	/*                                   */
	if (wl->fwlog_size < 0) {
		mutex_unlock(&wl->mutex);
		return 0;
	}

	/*                                                                  */
	len = min(count, (size_t)wl->fwlog_size);
	wl->fwlog_size -= len;
	memcpy(buffer, wl->fwlog, len);

	/*                            */
	memmove(wl->fwlog, wl->fwlog + len, wl->fwlog_size);

	mutex_unlock(&wl->mutex);

	return len;
}

static struct bin_attribute fwlog_attr = {
	.attr = {.name = "fwlog", .mode = S_IRUSR},
	.read = wl1271_sysfs_read_fwlog,
};

static void wl12xx_derive_mac_addresses(struct wl1271 *wl, u32 oui, u32 nic)
{
	int i;

	wl1271_debug(DEBUG_PROBE, "base address: oui %06x nic %06x",
		     oui, nic);

	if (nic + WLCORE_NUM_MAC_ADDRESSES - wl->num_mac_addr > 0xffffff)
		wl1271_warning("NIC part of the MAC address wraps around!");

	for (i = 0; i < wl->num_mac_addr; i++) {
		wl->addresses[i].addr[0] = (u8)(oui >> 16);
		wl->addresses[i].addr[1] = (u8)(oui >> 8);
		wl->addresses[i].addr[2] = (u8) oui;
		wl->addresses[i].addr[3] = (u8)(nic >> 16);
		wl->addresses[i].addr[4] = (u8)(nic >> 8);
		wl->addresses[i].addr[5] = (u8) nic;
		nic++;
	}

	/*                                         */
	WARN_ON(wl->num_mac_addr + 1 < WLCORE_NUM_MAC_ADDRESSES);

	/*
                                                          
                     
  */
	if (wl->num_mac_addr < WLCORE_NUM_MAC_ADDRESSES) {
		int idx = WLCORE_NUM_MAC_ADDRESSES - 1;
		memcpy(&wl->addresses[idx], &wl->addresses[0],
		       sizeof(wl->addresses[0]));
		/*         */
		wl->addresses[idx].addr[2] |= BIT(1);
	}

	wl->hw->wiphy->n_addresses = WLCORE_NUM_MAC_ADDRESSES;
	wl->hw->wiphy->addresses = wl->addresses;
}

static int wl12xx_get_hw_info(struct wl1271 *wl)
{
	int ret;

	ret = wl12xx_set_power_on(wl);
	if (ret < 0)
		return ret;

	ret = wlcore_read_reg(wl, REG_CHIP_ID_B, &wl->chip.id);
	if (ret < 0)
		goto out;

	wl->fuse_oui_addr = 0;
	wl->fuse_nic_addr = 0;

	ret = wl->ops->get_pg_ver(wl, &wl->hw_pg_ver);
	if (ret < 0)
		goto out;

	if (wl->ops->get_mac)
		ret = wl->ops->get_mac(wl);

out:
	wl1271_power_off(wl);
	return ret;
}

static int wl1271_register_hw(struct wl1271 *wl)
{
	int ret;
	u32 oui_addr = 0, nic_addr = 0;

	if (wl->mac80211_registered)
		return 0;

	if (wl->nvs_len >= 12) {
		/*                                                 
                                                      
                                            
   */
		u8 *nvs_ptr = (u8 *)wl->nvs;

		oui_addr =
			(nvs_ptr[11] << 16) + (nvs_ptr[10] << 8) + nvs_ptr[6];
		nic_addr =
			(nvs_ptr[5] << 16) + (nvs_ptr[4] << 8) + nvs_ptr[3];
	}

	/*                                                          */
	if (oui_addr == 0 && nic_addr == 0) {
		oui_addr = wl->fuse_oui_addr;
		/*                                                           */
		nic_addr = wl->fuse_nic_addr + 1;
	}

	wl12xx_derive_mac_addresses(wl, oui_addr, nic_addr);

	ret = ieee80211_register_hw(wl->hw);
	if (ret < 0) {
		wl1271_error("unable to register mac80211 hw: %d", ret);
		goto out;
	}

	wl->mac80211_registered = true;

	wl1271_debugfs_init(wl);

	wl1271_notice("loaded");

out:
	return ret;
}

static void wl1271_unregister_hw(struct wl1271 *wl)
{
	if (wl->plt)
		wl1271_plt_stop(wl);

	ieee80211_unregister_hw(wl->hw);
	wl->mac80211_registered = false;

}

static const struct ieee80211_iface_limit wlcore_iface_limits[] = {
	{
		.max = 3,
		.types = BIT(NL80211_IFTYPE_STATION),
	},
	{
		.max = 1,
		.types = BIT(NL80211_IFTYPE_AP) |
			 BIT(NL80211_IFTYPE_P2P_GO) |
			 BIT(NL80211_IFTYPE_P2P_CLIENT),
	},
};

static struct ieee80211_iface_combination
wlcore_iface_combinations[] = {
	{
	  .max_interfaces = 3,
	  .limits = wlcore_iface_limits,
	  .n_limits = ARRAY_SIZE(wlcore_iface_limits),
	},
};

static int wl1271_init_ieee80211(struct wl1271 *wl)
{
	int i;
	static const u32 cipher_suites[] = {
		WLAN_CIPHER_SUITE_WEP40,
		WLAN_CIPHER_SUITE_WEP104,
		WLAN_CIPHER_SUITE_TKIP,
		WLAN_CIPHER_SUITE_CCMP,
		WL1271_CIPHER_SUITE_GEM,
	};

	/*                          */
	wl->hw->extra_tx_headroom = sizeof(struct wl1271_tx_hw_descr);

	if (wl->quirks & WLCORE_QUIRK_TKIP_HEADER_SPACE)
		wl->hw->extra_tx_headroom += WL1271_EXTRA_SPACE_TKIP;

	/*         */
	/*                            */
	wl->hw->channel_change_time = 10000;
	wl->hw->max_listen_interval = wl->conf.conn.max_listen_interval;

	wl->hw->flags = IEEE80211_HW_SIGNAL_DBM |
		IEEE80211_HW_SUPPORTS_PS |
		IEEE80211_HW_SUPPORTS_DYNAMIC_PS |
		IEEE80211_HW_SUPPORTS_UAPSD |
		IEEE80211_HW_HAS_RATE_CONTROL |
		IEEE80211_HW_CONNECTION_MONITOR |
		IEEE80211_HW_REPORTS_TX_ACK_STATUS |
		IEEE80211_HW_SPECTRUM_MGMT |
		IEEE80211_HW_AP_LINK_PS |
		IEEE80211_HW_AMPDU_AGGREGATION |
		IEEE80211_HW_TX_AMPDU_SETUP_IN_HW |
		IEEE80211_HW_QUEUE_CONTROL;

	wl->hw->wiphy->cipher_suites = cipher_suites;
	wl->hw->wiphy->n_cipher_suites = ARRAY_SIZE(cipher_suites);

	wl->hw->wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
		BIT(NL80211_IFTYPE_ADHOC) | BIT(NL80211_IFTYPE_AP) |
		BIT(NL80211_IFTYPE_P2P_CLIENT) | BIT(NL80211_IFTYPE_P2P_GO);
	wl->hw->wiphy->max_scan_ssids = 1;
	wl->hw->wiphy->max_sched_scan_ssids = 16;
	wl->hw->wiphy->max_match_sets = 16;
	/*
                                                                  
                                                                 
                                        
  */
	wl->hw->wiphy->max_scan_ie_len = WL1271_CMD_TEMPL_MAX_SIZE -
			sizeof(struct ieee80211_header);

	wl->hw->wiphy->max_sched_scan_ie_len = WL1271_CMD_TEMPL_MAX_SIZE -
		sizeof(struct ieee80211_header);

	wl->hw->wiphy->max_remain_on_channel_duration = 5000;

	wl->hw->wiphy->flags |= WIPHY_FLAG_AP_UAPSD |
				WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL;

	/*                                                          */
	BUILD_BUG_ON(ARRAY_SIZE(wl1271_channels) +
		     ARRAY_SIZE(wl1271_channels_5ghz) >
		     WL1271_MAX_CHANNELS);
	/*
                                              
                                                   
 */
	for (i = 0; i < ARRAY_SIZE(wl1271_channels); i++) {
		wl1271_band_2ghz.channels[i].flags = 0;
		wl1271_band_2ghz.channels[i].max_power = WLCORE_MAX_TXPWR;
		wl1271_band_2ghz.channels[i].max_antenna_gain = 0;
	}

	for (i = 0; i < ARRAY_SIZE(wl1271_channels_5ghz); i++) {
		wl1271_band_5ghz.channels[i].flags = 0;
		wl1271_band_5ghz.channels[i].max_power = WLCORE_MAX_TXPWR;
		wl1271_band_5ghz.channels[i].max_antenna_gain = 0;
	}

	/*
                                                               
                                      
  */
	memcpy(&wl->bands[IEEE80211_BAND_2GHZ], &wl1271_band_2ghz,
	       sizeof(wl1271_band_2ghz));
	memcpy(&wl->bands[IEEE80211_BAND_2GHZ].ht_cap,
	       &wl->ht_cap[IEEE80211_BAND_2GHZ],
	       sizeof(*wl->ht_cap));
	memcpy(&wl->bands[IEEE80211_BAND_5GHZ], &wl1271_band_5ghz,
	       sizeof(wl1271_band_5ghz));
	memcpy(&wl->bands[IEEE80211_BAND_5GHZ].ht_cap,
	       &wl->ht_cap[IEEE80211_BAND_5GHZ],
	       sizeof(*wl->ht_cap));

	wl->hw->wiphy->bands[IEEE80211_BAND_2GHZ] =
		&wl->bands[IEEE80211_BAND_2GHZ];
	wl->hw->wiphy->bands[IEEE80211_BAND_5GHZ] =
		&wl->bands[IEEE80211_BAND_5GHZ];

	/*
                                               
                                                        
  */
	wl->hw->queues = (NUM_TX_QUEUES + 1) * WLCORE_NUM_MAC_ADDRESSES + 1;

	/*                                        */
	wl->hw->offchannel_tx_hw_queue = wl->hw->queues - 1;
	wl->hw->max_rates = 1;

	wl->hw->wiphy->reg_notifier = wl1271_reg_notify;

	/*                                          */
	wl->hw->wiphy->flags |= WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD;
	wl->hw->wiphy->probe_resp_offload =
		NL80211_PROBE_RESP_OFFLOAD_SUPPORT_WPS |
		NL80211_PROBE_RESP_OFFLOAD_SUPPORT_WPS2 |
		NL80211_PROBE_RESP_OFFLOAD_SUPPORT_P2P;

	/*                                */
	wlcore_iface_combinations[0].num_different_channels = wl->num_channels;
	wl->hw->wiphy->iface_combinations = wlcore_iface_combinations;
	wl->hw->wiphy->n_iface_combinations =
		ARRAY_SIZE(wlcore_iface_combinations);

	SET_IEEE80211_DEV(wl->hw, wl->dev);

	wl->hw->sta_data_size = sizeof(struct wl1271_station);
	wl->hw->vif_data_size = sizeof(struct wl12xx_vif);

	wl->hw->max_rx_aggregation_subframes = wl->conf.ht.rx_ba_win_size;

	return 0;
}

#define WL1271_DEFAULT_CHANNEL 0

struct ieee80211_hw *wlcore_alloc_hw(size_t priv_size, u32 aggr_buf_size,
				     u32 mbox_size)
{
	struct ieee80211_hw *hw;
	struct wl1271 *wl;
	int i, j, ret;
	unsigned int order;

	BUILD_BUG_ON(AP_MAX_STATIONS > WL12XX_MAX_LINKS);

	hw = ieee80211_alloc_hw(sizeof(*wl), &wl1271_ops);
	if (!hw) {
		wl1271_error("could not alloc ieee80211_hw");
		ret = -ENOMEM;
		goto err_hw_alloc;
	}

	wl = hw->priv;
	memset(wl, 0, sizeof(*wl));

	wl->priv = kzalloc(priv_size, GFP_KERNEL);
	if (!wl->priv) {
		wl1271_error("could not alloc wl priv");
		ret = -ENOMEM;
		goto err_priv_alloc;
	}

	INIT_LIST_HEAD(&wl->wlvif_list);

	wl->hw = hw;

	for (i = 0; i < NUM_TX_QUEUES; i++)
		for (j = 0; j < WL12XX_MAX_LINKS; j++)
			skb_queue_head_init(&wl->links[j].tx_queue[i]);

	skb_queue_head_init(&wl->deferred_rx_queue);
	skb_queue_head_init(&wl->deferred_tx_queue);

	INIT_DELAYED_WORK(&wl->elp_work, wl1271_elp_work);
	INIT_WORK(&wl->netstack_work, wl1271_netstack_work);
	INIT_WORK(&wl->tx_work, wl1271_tx_work);
	INIT_WORK(&wl->recovery_work, wl1271_recovery_work);
	INIT_DELAYED_WORK(&wl->scan_complete_work, wl1271_scan_complete_work);
	INIT_DELAYED_WORK(&wl->roc_complete_work, wlcore_roc_complete_work);
	INIT_DELAYED_WORK(&wl->tx_watchdog_work, wl12xx_tx_watchdog_work);

	wl->freezable_wq = create_freezable_workqueue("wl12xx_wq");
	if (!wl->freezable_wq) {
		ret = -ENOMEM;
		goto err_hw;
	}

	wl->channel = WL1271_DEFAULT_CHANNEL;
	wl->rx_counter = 0;
	wl->power_level = WL1271_DEFAULT_POWER_LEVEL;
	wl->band = IEEE80211_BAND_2GHZ;
	wl->channel_type = NL80211_CHAN_NO_HT;
	wl->flags = 0;
	wl->sg_enabled = true;
	wl->sleep_auth = WL1271_PSM_ILLEGAL;
	wl->recovery_count = 0;
	wl->hw_pg_ver = -1;
	wl->ap_ps_map = 0;
	wl->ap_fw_ps_map = 0;
	wl->quirks = 0;
	wl->platform_quirks = 0;
	wl->system_hlid = WL12XX_SYSTEM_HLID;
	wl->active_sta_count = 0;
	wl->active_link_count = 0;
	wl->fwlog_size = 0;
	init_waitqueue_head(&wl->fwlog_waitq);

	/*                                     */
	__set_bit(WL12XX_SYSTEM_HLID, wl->links_map);

	memset(wl->tx_frames_map, 0, sizeof(wl->tx_frames_map));
	for (i = 0; i < wl->num_tx_desc; i++)
		wl->tx_frames[i] = NULL;

	spin_lock_init(&wl->wl_lock);

	wl->state = WLCORE_STATE_OFF;
	wl->fw_type = WL12XX_FW_TYPE_NONE;
	mutex_init(&wl->mutex);
	mutex_init(&wl->flush_mutex);
	init_completion(&wl->nvs_loading_complete);

	order = get_order(aggr_buf_size);
	wl->aggr_buf = (u8 *)__get_free_pages(GFP_KERNEL, order);
	if (!wl->aggr_buf) {
		ret = -ENOMEM;
		goto err_wq;
	}
	wl->aggr_buf_size = aggr_buf_size;

	wl->dummy_packet = wl12xx_alloc_dummy_packet(wl);
	if (!wl->dummy_packet) {
		ret = -ENOMEM;
		goto err_aggr;
	}

	/*                                  */
	wl->fwlog = (u8 *)get_zeroed_page(GFP_KERNEL);
	if (!wl->fwlog) {
		ret = -ENOMEM;
		goto err_dummy_packet;
	}

	wl->mbox_size = mbox_size;
	wl->mbox = kmalloc(wl->mbox_size, GFP_KERNEL | GFP_DMA);
	if (!wl->mbox) {
		ret = -ENOMEM;
		goto err_fwlog;
	}

	wl->buffer_32 = kmalloc(sizeof(*wl->buffer_32), GFP_KERNEL);
	if (!wl->buffer_32) {
		ret = -ENOMEM;
		goto err_mbox;
	}

	return hw;

err_mbox:
	kfree(wl->mbox);

err_fwlog:
	free_page((unsigned long)wl->fwlog);

err_dummy_packet:
	dev_kfree_skb(wl->dummy_packet);

err_aggr:
	free_pages((unsigned long)wl->aggr_buf, order);

err_wq:
	destroy_workqueue(wl->freezable_wq);

err_hw:
	wl1271_debugfs_exit(wl);
	kfree(wl->priv);

err_priv_alloc:
	ieee80211_free_hw(hw);

err_hw_alloc:

	return ERR_PTR(ret);
}
EXPORT_SYMBOL_GPL(wlcore_alloc_hw);

int wlcore_free_hw(struct wl1271 *wl)
{
	/*                           */
	mutex_lock(&wl->mutex);
	wl->fwlog_size = -1;
	wake_up_interruptible_all(&wl->fwlog_waitq);
	mutex_unlock(&wl->mutex);

	device_remove_bin_file(wl->dev, &fwlog_attr);

	device_remove_file(wl->dev, &dev_attr_hw_pg_ver);

	device_remove_file(wl->dev, &dev_attr_bt_coex_state);
	kfree(wl->buffer_32);
	kfree(wl->mbox);
	free_page((unsigned long)wl->fwlog);
	dev_kfree_skb(wl->dummy_packet);
	free_pages((unsigned long)wl->aggr_buf, get_order(wl->aggr_buf_size));

	wl1271_debugfs_exit(wl);

	vfree(wl->fw);
	wl->fw = NULL;
	wl->fw_type = WL12XX_FW_TYPE_NONE;
	kfree(wl->nvs);
	wl->nvs = NULL;

	kfree(wl->fw_status_1);
	kfree(wl->tx_res_if);
	destroy_workqueue(wl->freezable_wq);

	kfree(wl->priv);
	ieee80211_free_hw(wl->hw);

	return 0;
}
EXPORT_SYMBOL_GPL(wlcore_free_hw);

static void wlcore_nvs_cb(const struct firmware *fw, void *context)
{
	struct wl1271 *wl = context;
	struct platform_device *pdev = wl->pdev;
	struct wlcore_platdev_data *pdev_data = pdev->dev.platform_data;
	struct wl12xx_platform_data *pdata = pdev_data->pdata;
	unsigned long irqflags;
	int ret;

	if (fw) {
		wl->nvs = kmemdup(fw->data, fw->size, GFP_KERNEL);
		if (!wl->nvs) {
			wl1271_error("Could not allocate nvs data");
			goto out;
		}
		wl->nvs_len = fw->size;
	} else {
		wl1271_debug(DEBUG_BOOT, "Could not get nvs file %s",
			     WL12XX_NVS_NAME);
		wl->nvs = NULL;
		wl->nvs_len = 0;
	}

	ret = wl->ops->setup(wl);
	if (ret < 0)
		goto out_free_nvs;

	BUG_ON(wl->num_tx_desc > WLCORE_MAX_TX_DESCRIPTORS);

	/*                                              */
	wlcore_adjust_conf(wl);

	wl->irq = platform_get_irq(pdev, 0);
	wl->platform_quirks = pdata->platform_quirks;
	wl->if_ops = pdev_data->if_ops;

	if (wl->platform_quirks & WL12XX_PLATFORM_QUIRK_EDGE_IRQ)
		irqflags = IRQF_TRIGGER_RISING;
	else
		irqflags = IRQF_TRIGGER_HIGH | IRQF_ONESHOT;

	ret = request_threaded_irq(wl->irq, NULL, wlcore_irq,
				   irqflags, pdev->name, wl);
	if (ret < 0) {
		wl1271_error("request_irq() failed: %d", ret);
		goto out_free_nvs;
	}

#ifdef CONFIG_PM
	ret = enable_irq_wake(wl->irq);
	if (!ret) {
		wl->irq_wake_enabled = true;
		device_init_wakeup(wl->dev, 1);
		if (pdata->pwr_in_suspend) {
			wl->hw->wiphy->wowlan.flags = WIPHY_WOWLAN_ANY;
			wl->hw->wiphy->wowlan.n_patterns =
				WL1271_MAX_RX_FILTERS;
			wl->hw->wiphy->wowlan.pattern_min_len = 1;
			wl->hw->wiphy->wowlan.pattern_max_len =
				WL1271_RX_FILTER_MAX_PATTERN_SIZE;
		}
	}
#endif
	disable_irq(wl->irq);

	ret = wl12xx_get_hw_info(wl);
	if (ret < 0) {
		wl1271_error("couldn't get hw info");
		goto out_irq;
	}

	ret = wl->ops->identify_chip(wl);
	if (ret < 0)
		goto out_irq;

	ret = wl1271_init_ieee80211(wl);
	if (ret)
		goto out_irq;

	ret = wl1271_register_hw(wl);
	if (ret)
		goto out_irq;

	/*                                            */
	ret = device_create_file(wl->dev, &dev_attr_bt_coex_state);
	if (ret < 0) {
		wl1271_error("failed to create sysfs file bt_coex_state");
		goto out_unreg;
	}

	/*                                        */
	ret = device_create_file(wl->dev, &dev_attr_hw_pg_ver);
	if (ret < 0) {
		wl1271_error("failed to create sysfs file hw_pg_ver");
		goto out_bt_coex_state;
	}

	/*                                  */
	ret = device_create_bin_file(wl->dev, &fwlog_attr);
	if (ret < 0) {
		wl1271_error("failed to create sysfs file fwlog");
		goto out_hw_pg_ver;
	}

	wl->initialized = true;
	goto out;

out_hw_pg_ver:
	device_remove_file(wl->dev, &dev_attr_hw_pg_ver);

out_bt_coex_state:
	device_remove_file(wl->dev, &dev_attr_bt_coex_state);

out_unreg:
	wl1271_unregister_hw(wl);

out_irq:
	free_irq(wl->irq, wl);

out_free_nvs:
	kfree(wl->nvs);

out:
	release_firmware(fw);
	complete_all(&wl->nvs_loading_complete);
}

int wlcore_probe(struct wl1271 *wl, struct platform_device *pdev)
{
	int ret;

	if (!wl->ops || !wl->ptable)
		return -EINVAL;

	wl->dev = &pdev->dev;
	wl->pdev = pdev;
	platform_set_drvdata(pdev, wl);

	ret = request_firmware_nowait(THIS_MODULE, FW_ACTION_HOTPLUG,
				      WL12XX_NVS_NAME, &pdev->dev, GFP_KERNEL,
				      wl, wlcore_nvs_cb);
	if (ret < 0) {
		wl1271_error("request_firmware_nowait failed: %d", ret);
		complete_all(&wl->nvs_loading_complete);
	}

	return ret;
}
EXPORT_SYMBOL_GPL(wlcore_probe);

int wlcore_remove(struct platform_device *pdev)
{
	struct wl1271 *wl = platform_get_drvdata(pdev);

	wait_for_completion(&wl->nvs_loading_complete);
	if (!wl->initialized)
		return 0;

	if (wl->irq_wake_enabled) {
		device_init_wakeup(wl->dev, 0);
		disable_irq_wake(wl->irq);
	}
	wl1271_unregister_hw(wl);
	free_irq(wl->irq, wl);
	wlcore_free_hw(wl);

	return 0;
}
EXPORT_SYMBOL_GPL(wlcore_remove);

u32 wl12xx_debug_level = DEBUG_NONE;
EXPORT_SYMBOL_GPL(wl12xx_debug_level);
module_param_named(debug_level, wl12xx_debug_level, uint, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(debug_level, "wl12xx debugging level");

module_param_named(fwlog, fwlog_param, charp, 0);
MODULE_PARM_DESC(fwlog,
		 "FW logger options: continuous, ondemand, dbgpins or disable");

module_param(bug_on_recovery, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(bug_on_recovery, "BUG() on fw recovery");

module_param(no_recovery, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(no_recovery, "Prevent HW recovery. FW will remain stuck.");

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Luciano Coelho <coelho@ti.com>");
MODULE_AUTHOR("Juuso Oikarinen <juuso.oikarinen@nokia.com>");
MODULE_FIRMWARE(WL12XX_NVS_NAME);