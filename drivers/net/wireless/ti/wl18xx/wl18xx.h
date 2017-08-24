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

#ifndef __WL18XX_PRIV_H__
#define __WL18XX_PRIV_H__

#include "conf.h"

/*                                */
#define WL18XX_CHIP_VER		8
#define WL18XX_IFTYPE_VER	5
#define WL18XX_MAJOR_VER	WLCORE_FW_VER_IGNORE
#define WL18XX_SUBTYPE_VER	WLCORE_FW_VER_IGNORE
#define WL18XX_MINOR_VER	39

#define WL18XX_CMD_MAX_SIZE          740

#define WL18XX_AGGR_BUFFER_SIZE		(13 * PAGE_SIZE)

#define WL18XX_NUM_TX_DESCRIPTORS 32
#define WL18XX_NUM_RX_DESCRIPTORS 32

#define WL18XX_NUM_MAC_ADDRESSES 3

#define WL18XX_RX_BA_MAX_SESSIONS 5

struct wl18xx_priv {
	/*                                   */
	u8 cmd_buf[WL18XX_CMD_MAX_SIZE];

	struct wl18xx_priv_conf conf;

	/*                                      */
	u8 last_fw_rls_idx;

	/*                                                 */
	int extra_spare_key_count;
};

#define WL18XX_FW_MAX_TX_STATUS_DESC 33

struct wl18xx_fw_status_priv {
	/*
                                                       
                         
  */
	u8 fw_release_idx;

	/*
                                                      
                                     
  */
	u8 released_tx_desc[WL18XX_FW_MAX_TX_STATUS_DESC];

	/*                                                                 
                                                      
  */
	__le32 link_suspend_bitmap;

	/*                                           
                             
  */
	u8 tx_ac_threshold;

	/*                                                */
	u8 tx_ps_threshold;

	/*                                                   */
	u8 tx_suspend_threshold;

	/*                                                                
                                         
  */
	u8 tx_slow_link_prio_threshold;

	/*                                                                
                                         
  */
	u8 tx_fast_link_prio_threshold;

	/*                                                                
                                                  
  */
	u8 tx_slow_stop_threshold;

	/*                                                                
                                                  
  */
	u8 tx_fast_stop_threshold;

	u8 padding[3];
};

#define WL18XX_PHY_VERSION_MAX_LEN 20

struct wl18xx_static_data_priv {
	char phy_version[WL18XX_PHY_VERSION_MAX_LEN];
};

struct wl18xx_clk_cfg {
	u32 n;
	u32 m;
	u32 p;
	u32 q;
	bool swallow;
};

enum {
	CLOCK_CONFIG_16_2_M	= 1,
	CLOCK_CONFIG_16_368_M,
	CLOCK_CONFIG_16_8_M,
	CLOCK_CONFIG_19_2_M,
	CLOCK_CONFIG_26_M,
	CLOCK_CONFIG_32_736_M,
	CLOCK_CONFIG_33_6_M,
	CLOCK_CONFIG_38_468_M,
	CLOCK_CONFIG_52_M,

	NUM_CLOCK_CONFIGS,
};

#endif /*                   */
