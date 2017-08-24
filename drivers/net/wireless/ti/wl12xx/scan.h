/*
 * This file is part of wl12xx
 *
 * Copyright (C) 2012 Texas Instruments. All rights reserved.
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

#ifndef __WL12XX_SCAN_H__
#define __WL12XX_SCAN_H__

#include "../wlcore/wlcore.h"
#include "../wlcore/cmd.h"
#include "../wlcore/scan.h"

#define WL12XX_MAX_CHANNELS_5GHZ 23

struct basic_scan_params {
	/*                                       */
	__le16 scan_options;
	u8 role_id;
	/*                                                  */
	u8 n_ch;
	/*                                                          
                                   */
	u8 n_probe_reqs;
	u8 tid_trigger;
	u8 ssid_len;
	u8 use_ssid_list;

	/*                                       */
	__le32 tx_rate;

	u8 ssid[IEEE80211_MAX_SSID_LEN];
	/*              */
	u8 band;

	u8 scan_tag;
	u8 padding2[2];
} __packed;

struct basic_scan_channel_params {
	/*                                                                */
	__le32 min_duration;
	__le32 max_duration;
	__le32 bssid_lsb;
	__le16 bssid_msb;
	u8 early_termination;
	u8 tx_power_att;
	u8 channel;
	/*                       */
	u8 dfs_candidate;
	u8 activity_detected;
	u8 pad;
} __packed;

struct wl1271_cmd_scan {
	struct wl1271_cmd_header header;

	struct basic_scan_params params;
	struct basic_scan_channel_params channels[WL1271_SCAN_MAX_CHANNELS];

	/*                 */
	u8 addr[ETH_ALEN];
	u8 padding[2];
} __packed;

struct wl1271_cmd_sched_scan_config {
	struct wl1271_cmd_header header;

	__le32 intervals[SCAN_MAX_CYCLE_INTERVALS];

	s8 rssi_threshold; /*                        */
	s8 snr_threshold;  /*                       */

	u8 cycles;       /*                               */
	u8 report_after; /*                                                 */
	u8 terminate;    /*                               */

	u8 tag;
	u8 bss_type; /*               */
	u8 filter_type;

	u8 ssid_len;     /*                               */
	u8 ssid[IEEE80211_MAX_SSID_LEN];

	u8 n_probe_reqs; /*                                       */

	u8 passive[SCAN_MAX_BANDS];
	u8 active[SCAN_MAX_BANDS];

	u8 dfs;

	u8 n_pactive_ch; /*                                                    
                           */
	u8 role_id;
	u8 padding[1];
	struct conn_scan_ch_params channels_2[MAX_CHANNELS_2GHZ];
	struct conn_scan_ch_params channels_5[WL12XX_MAX_CHANNELS_5GHZ];
	struct conn_scan_ch_params channels_4[MAX_CHANNELS_4GHZ];
} __packed;

struct wl1271_cmd_sched_scan_start {
	struct wl1271_cmd_header header;

	u8 tag;
	u8 role_id;
	u8 padding[2];
} __packed;

struct wl1271_cmd_sched_scan_stop {
	struct wl1271_cmd_header header;

	u8 tag;
	u8 role_id;
	u8 padding[2];
} __packed;

int wl12xx_scan_start(struct wl1271 *wl, struct wl12xx_vif *wlvif,
		      struct cfg80211_scan_request *req);
int wl12xx_scan_stop(struct wl1271 *wl, struct wl12xx_vif *wlvif);
void wl12xx_scan_completed(struct wl1271 *wl, struct wl12xx_vif *wlvif);
int wl12xx_sched_scan_start(struct wl1271 *wl, struct wl12xx_vif  *wlvif,
			    struct cfg80211_sched_scan_request *req,
			    struct ieee80211_sched_scan_ies *ies);
void wl12xx_scan_sched_scan_stop(struct wl1271 *wl,  struct wl12xx_vif *wlvif);
#endif
