/*
 * This file is part of wl12xx
 *
 * Copyright (C) 1998-2009, 2011 Texas Instruments. All rights reserved.
 * Copyright (C) 2008-2010 Nokia Corporation
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

#ifndef __WL12XX_ACX_H__
#define __WL12XX_ACX_H__

#include "../wlcore/wlcore.h"
#include "../wlcore/acx.h"

#define WL12XX_ACX_ALL_EVENTS_VECTOR	(WL1271_ACX_INTR_WATCHDOG      | \
					WL1271_ACX_INTR_INIT_COMPLETE | \
					WL1271_ACX_INTR_EVENT_A       | \
					WL1271_ACX_INTR_EVENT_B       | \
					WL1271_ACX_INTR_CMD_COMPLETE  | \
					WL1271_ACX_INTR_HW_AVAILABLE  | \
					WL1271_ACX_INTR_DATA)

#define WL12XX_INTR_MASK		(WL1271_ACX_INTR_WATCHDOG      | \
					WL1271_ACX_INTR_EVENT_A       | \
					WL1271_ACX_INTR_EVENT_B       | \
					WL1271_ACX_INTR_HW_AVAILABLE  | \
					WL1271_ACX_INTR_DATA)

struct wl1271_acx_host_config_bitmap {
	struct acx_header header;

	__le32 host_cfg_bitmap;
} __packed;

struct wl12xx_acx_tx_statistics {
	__le32 internal_desc_overflow;
}  __packed;

struct wl12xx_acx_rx_statistics {
	__le32 out_of_mem;
	__le32 hdr_overflow;
	__le32 hw_stuck;
	__le32 dropped;
	__le32 fcs_err;
	__le32 xfr_hint_trig;
	__le32 path_reset;
	__le32 reset_counter;
} __packed;

struct wl12xx_acx_dma_statistics {
	__le32 rx_requested;
	__le32 rx_errors;
	__le32 tx_requested;
	__le32 tx_errors;
}  __packed;

struct wl12xx_acx_isr_statistics {
	/*                       */
	__le32 cmd_cmplt;

	/*          */
	__le32 fiqs;

	/*                                   */
	__le32 rx_headers;

	/*                                  */
	__le32 rx_completes;

	/*                                   */
	__le32 rx_mem_overflow;

	/*                                  */
	__le32 rx_rdys;

	/*          */
	__le32 irqs;

	/*                                 */
	__le32 tx_procs;

	/*                                      */
	__le32 decrypt_done;

	/*                              */
	__le32 dma0_done;

	/*                              */
	__le32 dma1_done;

	/*                                      */
	__le32 tx_exch_complete;

	/*                                 */
	__le32 commands;

	/*                                 */
	__le32 rx_procs;

	/*                                */
	__le32 hw_pm_mode_changes;

	/*                                     */
	__le32 host_acknowledges;

	/*                                */
	__le32 pci_pm;

	/*                                    */
	__le32 wakeups;

	/*                                  */
	__le32 low_rssi;
} __packed;

struct wl12xx_acx_wep_statistics {
	/*                             */
	__le32 addr_key_count;

	/*                         */
	__le32 default_key_count;

	__le32 reserved;

	/*                                                  */
	__le32 key_not_found;

	/*                                                */
	__le32 decrypt_fail;

	/*                       */
	__le32 packets;

	/*                        */
	__le32 interrupt;
} __packed;

#define ACX_MISSED_BEACONS_SPREAD 10

struct wl12xx_acx_pwr_statistics {
	/*                                                           */
	__le32 ps_enter;

	/*                                    */
	__le32 elp_enter;

	/*                                                     */
	__le32 missing_bcns;

	/*                                         */
	__le32 wake_on_host;

	/*                                    */
	__le32 wake_on_timer_exp;

	/*                                                             */
	__le32 tx_with_ps;

	/*                                                               */
	__le32 tx_without_ps;

	/*                                */
	__le32 rcvd_beacons;

	/*                                                      */
	__le32 power_save_off;

	/*                                            */
	__le16 enable_ps;

	/*
                                                                
               
  */
	__le16 disable_ps;

	/*
                                                            
            
  */
	__le32 fix_tsf_ps;

	/*                                                             
                                             
                                             
                                                    
                                                             
                                                               
       
                                                                      
 */
	__le32 cont_miss_bcns_spread[ACX_MISSED_BEACONS_SPREAD];

	/*                                     */
	__le32 rcvd_awake_beacons;
} __packed;

struct wl12xx_acx_mic_statistics {
	__le32 rx_pkts;
	__le32 calc_failure;
} __packed;

struct wl12xx_acx_aes_statistics {
	__le32 encrypt_fail;
	__le32 decrypt_fail;
	__le32 encrypt_packets;
	__le32 decrypt_packets;
	__le32 encrypt_interrupt;
	__le32 decrypt_interrupt;
} __packed;

struct wl12xx_acx_event_statistics {
	__le32 heart_beat;
	__le32 calibration;
	__le32 rx_mismatch;
	__le32 rx_mem_empty;
	__le32 rx_pool;
	__le32 oom_late;
	__le32 phy_transmit_error;
	__le32 tx_stuck;
} __packed;

struct wl12xx_acx_ps_statistics {
	__le32 pspoll_timeouts;
	__le32 upsd_timeouts;
	__le32 upsd_max_sptime;
	__le32 upsd_max_apturn;
	__le32 pspoll_max_apturn;
	__le32 pspoll_utilization;
	__le32 upsd_utilization;
} __packed;

struct wl12xx_acx_rxpipe_statistics {
	__le32 rx_prep_beacon_drop;
	__le32 descr_host_int_trig_rx_data;
	__le32 beacon_buffer_thres_host_int_trig_rx_data;
	__le32 missed_beacon_host_int_trig_rx_data;
	__le32 tx_xfr_host_int_trig_rx_data;
} __packed;

struct wl12xx_acx_statistics {
	struct acx_header header;

	struct wl12xx_acx_tx_statistics tx;
	struct wl12xx_acx_rx_statistics rx;
	struct wl12xx_acx_dma_statistics dma;
	struct wl12xx_acx_isr_statistics isr;
	struct wl12xx_acx_wep_statistics wep;
	struct wl12xx_acx_pwr_statistics pwr;
	struct wl12xx_acx_aes_statistics aes;
	struct wl12xx_acx_mic_statistics mic;
	struct wl12xx_acx_event_statistics event;
	struct wl12xx_acx_ps_statistics ps;
	struct wl12xx_acx_rxpipe_statistics rxpipe;
} __packed;

int wl1271_acx_host_if_cfg_bitmap(struct wl1271 *wl, u32 host_cfg_bitmap);

#endif /*                  */
