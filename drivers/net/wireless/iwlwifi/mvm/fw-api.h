/******************************************************************************
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2012 - 2013 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110,
 * USA
 *
 * The full GNU General Public License is included in this distribution
 * in the file called COPYING.
 *
 * Contact Information:
 *  Intel Linux Wireless <ilw@linux.intel.com>
 * Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
 *
 * BSD LICENSE
 *
 * Copyright(c) 2012 - 2013 Intel Corporation. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

#ifndef __fw_api_h__
#define __fw_api_h__

#include "fw-api-rs.h"
#include "fw-api-tx.h"
#include "fw-api-sta.h"
#include "fw-api-mac.h"
#include "fw-api-power.h"
#include "fw-api-d3.h"
#include "fw-api-bt-coex.h"

/*                                 */
enum {
	IWL_MVM_OFFCHANNEL_QUEUE = 8,
	IWL_MVM_CMD_QUEUE = 9,
	IWL_MVM_AUX_QUEUE = 15,
	IWL_MVM_FIRST_AGG_QUEUE = 16,
	IWL_MVM_NUM_QUEUES = 20,
	IWL_MVM_LAST_AGG_QUEUE = IWL_MVM_NUM_QUEUES - 1,
	IWL_MVM_CMD_FIFO = 7
};

#define IWL_MVM_STATION_COUNT	16

/*          */
enum {
	MVM_ALIVE = 0x1,
	REPLY_ERROR = 0x2,

	INIT_COMPLETE_NOTIF = 0x4,

	/*                      */
	PHY_CONTEXT_CMD = 0x8,
	DBG_CFG = 0x9,

	/*               */
	ADD_STA = 0x18,
	REMOVE_STA = 0x19,

	/*    */
	TX_CMD = 0x1c,
	TXPATH_FLUSH = 0x1e,
	MGMT_MCAST_KEY = 0x1f,

	/*            */
	WEP_KEY = 0x20,

	/*                          */
	MAC_CONTEXT_CMD = 0x28,
	TIME_EVENT_CMD = 0x29, /*                       */
	TIME_EVENT_NOTIFICATION = 0x2a,
	BINDING_CONTEXT_CMD = 0x2b,
	TIME_QUOTA_CMD = 0x2c,

	LQ_CMD = 0x4e,

	/*             */
	TEMPERATURE_NOTIFICATION = 0x62,
	CALIBRATION_CFG_CMD = 0x65,
	CALIBRATION_RES_NOTIFICATION = 0x66,
	CALIBRATION_COMPLETE_NOTIFICATION = 0x67,
	RADIO_VERSION_NOTIFICATION = 0x68,

	/*              */
	SCAN_OFFLOAD_REQUEST_CMD = 0x51,
	SCAN_OFFLOAD_ABORT_CMD = 0x52,
	SCAN_OFFLOAD_COMPLETE = 0x6D,
	SCAN_OFFLOAD_UPDATE_PROFILES_CMD = 0x6E,
	SCAN_OFFLOAD_CONFIG_CMD = 0x6f,

	/*     */
	PHY_CONFIGURATION_CMD = 0x6a,
	CALIB_RES_NOTIF_PHY_DB = 0x6b,
	/*                    */

	/*       */
	POWER_TABLE_CMD = 0x77,

	/*          */
	SCAN_REQUEST_CMD = 0x80,
	SCAN_ABORT_CMD = 0x81,
	SCAN_START_NOTIFICATION = 0x82,
	SCAN_RESULTS_NOTIFICATION = 0x83,
	SCAN_COMPLETE_NOTIFICATION = 0x84,

	/*     */
	NVM_ACCESS_CMD = 0x88,

	SET_CALIB_DEFAULT_CMD = 0x8e,

	BEACON_NOTIFICATION = 0x90,
	BEACON_TEMPLATE_CMD = 0x91,
	TX_ANT_CONFIGURATION_CMD = 0x98,
	BT_CONFIG = 0x9b,
	STATISTICS_NOTIFICATION = 0x9d,

	/*                                    */
	CARD_STATE_CMD = 0xa0,
	CARD_STATE_NOTIFICATION = 0xa1,

	REPLY_RX_PHY_CMD = 0xc0,
	REPLY_RX_MPDU_CMD = 0xc1,
	BA_NOTIF = 0xc5,

	/*         */
	BT_COEX_PRIO_TABLE = 0xcc,
	BT_COEX_PROT_ENV = 0xcd,
	BT_PROFILE_NOTIFICATION = 0xce,

	REPLY_DEBUG_CMD = 0xf0,
	DEBUG_LOG_MSG = 0xf7,

	MCAST_FILTER_CMD = 0xd0,

	/*                           */
	D3_CONFIG_CMD = 0xd3,
	PROT_OFFLOAD_CONFIG_CMD = 0xd4,
	OFFLOADS_QUERY_CMD = 0xd5,
	REMOTE_WAKE_CONFIG_CMD = 0xd6,

	/*                          */
	WOWLAN_PATTERNS = 0xe0,
	WOWLAN_CONFIGURATION = 0xe1,
	WOWLAN_TSC_RSC_PARAM = 0xe2,
	WOWLAN_TKIP_PARAM = 0xe3,
	WOWLAN_KEK_KCK_MATERIAL = 0xe4,
	WOWLAN_GET_STATUSES = 0xe5,
	WOWLAN_TX_POWER_PER_DB = 0xe6,

	/*                   */
	NET_DETECT_CONFIG_CMD = 0x54,
	NET_DETECT_PROFILES_QUERY_CMD = 0x56,
	NET_DETECT_PROFILES_CMD = 0x57,
	NET_DETECT_HOTSPOTS_CMD = 0x58,
	NET_DETECT_HOTSPOTS_QUERY_CMD = 0x59,

	REPLY_MAX = 0xff,
};

/* 
                                                                      
                                                             
 */
struct iwl_cmd_response {
	__le32 status;
};

/*
                            
                                      
 */
struct iwl_tx_ant_cfg_cmd {
	__le32 valid;
} __packed;

/*
                              
                                                 
                                                                       
                  
                                                                        
                   
 */
struct iwl_calib_ctrl {
	__le32 flow_trigger;
	__le32 event_trigger;
} __packed;

/*                                                                       
                                                            
 */
enum iwl_calib_cfg {
	IWL_CALIB_CFG_XTAL_IDX			= BIT(0),
	IWL_CALIB_CFG_TEMPERATURE_IDX		= BIT(1),
	IWL_CALIB_CFG_VOLTAGE_READ_IDX		= BIT(2),
	IWL_CALIB_CFG_PAPD_IDX			= BIT(3),
	IWL_CALIB_CFG_TX_PWR_IDX		= BIT(4),
	IWL_CALIB_CFG_DC_IDX			= BIT(5),
	IWL_CALIB_CFG_BB_FILTER_IDX		= BIT(6),
	IWL_CALIB_CFG_LO_LEAKAGE_IDX		= BIT(7),
	IWL_CALIB_CFG_TX_IQ_IDX			= BIT(8),
	IWL_CALIB_CFG_TX_IQ_SKEW_IDX		= BIT(9),
	IWL_CALIB_CFG_RX_IQ_IDX			= BIT(10),
	IWL_CALIB_CFG_RX_IQ_SKEW_IDX		= BIT(11),
	IWL_CALIB_CFG_SENSITIVITY_IDX		= BIT(12),
	IWL_CALIB_CFG_CHAIN_NOISE_IDX		= BIT(13),
	IWL_CALIB_CFG_DISCONNECTED_ANT_IDX	= BIT(14),
	IWL_CALIB_CFG_ANT_COUPLING_IDX		= BIT(15),
	IWL_CALIB_CFG_DAC_IDX			= BIT(16),
	IWL_CALIB_CFG_ABS_IDX			= BIT(17),
	IWL_CALIB_CFG_AGC_IDX			= BIT(18),
};

/*
                             
 */
struct iwl_phy_cfg_cmd {
	__le32	phy_cfg;
	struct iwl_calib_ctrl calib_control;
} __packed;

#define PHY_CFG_RADIO_TYPE	(BIT(0) | BIT(1))
#define PHY_CFG_RADIO_STEP	(BIT(2) | BIT(3))
#define PHY_CFG_RADIO_DASH	(BIT(4) | BIT(5))
#define PHY_CFG_PRODUCT_NUMBER	(BIT(6) | BIT(7))
#define PHY_CFG_TX_CHAIN_A	BIT(8)
#define PHY_CFG_TX_CHAIN_B	BIT(9)
#define PHY_CFG_TX_CHAIN_C	BIT(10)
#define PHY_CFG_RX_CHAIN_A	BIT(12)
#define PHY_CFG_RX_CHAIN_B	BIT(13)
#define PHY_CFG_RX_CHAIN_C	BIT(14)


/*                              */
enum {
	NVM_ACCESS_TARGET_CACHE = 0,
	NVM_ACCESS_TARGET_OTP = 1,
	NVM_ACCESS_TARGET_EEPROM = 2,
};

/*                                  */
enum {
	NVM_SECTION_TYPE_HW = 0,
	NVM_SECTION_TYPE_SW,
	NVM_SECTION_TYPE_PAPD,
	NVM_SECTION_TYPE_BT,
	NVM_SECTION_TYPE_CALIBRATION,
	NVM_SECTION_TYPE_PRODUCTION,
	NVM_SECTION_TYPE_POST_FCS_CALIB,
	NVM_NUM_OF_SECTIONS,
};

/* 
                                                                             
                                
                               
                            
                                            
                                   
                                                                  
 */
struct iwl_nvm_access_cmd {
	u8 op_code;
	u8 target;
	__le16 type;
	__le16 offset;
	__le16 length;
	u8 data[];
} __packed; /*                            */

/* 
                                                               
                                            
                                                         
                            
                                         
                                                               
 */
struct iwl_nvm_access_resp {
	__le16 offset;
	__le16 length;
	__le16 type;
	__le16 status;
	u8 data[];
} __packed; /*                                 */

/*               */

/*                                */
#define ALIVE_RESP_UCODE_OK	BIT(0)
#define ALIVE_RESP_RFKILL	BIT(1)

/*                                */
enum {
	FW_TYPE_HW = 0,
	FW_TYPE_PROT = 1,
	FW_TYPE_AP = 2,
	FW_TYPE_WOWLAN = 3,
	FW_TYPE_TIMING = 4,
	FW_TYPE_WIPAN = 5
};

/*                                   */
enum {
	FW_SUBTYPE_FULL_FEATURE = 0,
	FW_SUBTYPE_BOOTSRAP = 1, /*           */
	FW_SUBTYPE_REDUCED = 2,
	FW_SUBTYPE_ALIVE_ONLY = 3,
	FW_SUBTYPE_WOWLAN = 4,
	FW_SUBTYPE_AP_SUBTYPE = 5,
	FW_SUBTYPE_WIPAN = 6,
	FW_SUBTYPE_INITIALIZE = 9
};

#define IWL_ALIVE_STATUS_ERR 0xDEAD
#define IWL_ALIVE_STATUS_OK 0xCAFE

#define IWL_ALIVE_FLG_RFKILL	BIT(0)

struct mvm_alive_resp {
	__le16 status;
	__le16 flags;
	u8 ucode_minor;
	u8 ucode_major;
	__le16 id;
	u8 api_minor;
	u8 api_major;
	u8 ver_subtype;
	u8 ver_type;
	u8 mac;
	u8 opt;
	__le16 reserved2;
	__le32 timestamp;
	__le32 error_event_table_ptr;	/*                            */
	__le32 log_event_table_ptr;	/*                            */
	__le32 cpu_register_ptr;
	__le32 dbgm_config_ptr;
	__le32 alive_counter_ptr;
	__le32 scd_base_ptr;		/*                      */
} __packed; /*                       */

/*                             */
enum {
	FW_ERR_UNKNOWN_CMD = 0x0,
	FW_ERR_INVALID_CMD_PARAM = 0x1,
	FW_ERR_SERVICE = 0x2,
	FW_ERR_ARC_MEMORY = 0x3,
	FW_ERR_ARC_CODE = 0x4,
	FW_ERR_WATCH_DOG = 0x5,
	FW_ERR_WEP_GRP_KEY_INDX = 0x10,
	FW_ERR_WEP_KEY_SIZE = 0x11,
	FW_ERR_OBSOLETE_FUNC = 0x12,
	FW_ERR_UNEXPECTED = 0xFE,
	FW_ERR_FATAL = 0xFF
};

/* 
                                              
                        
                               
                                                      
                                                             
                                                                      
                              
                            
 */
struct iwl_error_resp {
	__le32 error_type;
	u8 cmd_id;
	u8 reserved1;
	__le16 bad_cmd_seq_num;
	__le32 error_service;
	__le64 timestamp;
} __packed;


/*                                          */

#define MAX_MACS_IN_BINDING	(3)
#define MAX_BINDINGS		(4)
#define AUX_BINDING_INDEX	(3)
#define MAX_PHYS		(4)

/*                                                     */
#define FW_CTXT_ID_POS	  (0)
#define FW_CTXT_ID_MSK	  (0xff << FW_CTXT_ID_POS)
#define FW_CTXT_COLOR_POS (8)
#define FW_CTXT_COLOR_MSK (0xff << FW_CTXT_COLOR_POS)
#define FW_CTXT_INVALID	  (0xffffffff)

#define FW_CMD_ID_AND_COLOR(_id, _color) ((_id << FW_CTXT_ID_POS) |\
					  (_color << FW_CTXT_COLOR_POS))

/*                                             */
enum {
	FW_CTXT_ACTION_STUB = 0,
	FW_CTXT_ACTION_ADD,
	FW_CTXT_ACTION_MODIFY,
	FW_CTXT_ACTION_REMOVE,
	FW_CTXT_ACTION_NUM
}; /*                                   */

/*             */

/*                                         */
enum iwl_time_event_type {
	/*                    */
	TE_BSS_STA_AGGRESSIVE_ASSOC,
	TE_BSS_STA_ASSOC,
	TE_BSS_EAP_DHCP_PROT,
	TE_BSS_QUIET_PERIOD,

	/*                   */
	TE_P2P_DEVICE_DISCOVERABLE,
	TE_P2P_DEVICE_LISTEN,
	TE_P2P_DEVICE_ACTION_SCAN,
	TE_P2P_DEVICE_FULL_SCAN,

	/*                   */
	TE_P2P_CLIENT_AGGRESSIVE_ASSOC,
	TE_P2P_CLIENT_ASSOC,
	TE_P2P_CLIENT_QUIET_PERIOD,

	/*               */
	TE_P2P_GO_ASSOC_PROT,
	TE_P2P_GO_REPETITIVE_NOA,
	TE_P2P_GO_CT_WINDOW,

	/*                  */
	TE_WIDI_TX_SYNC,

	TE_MAX
}; /*                            */

/*                                                                     */
enum {
	TE_INDEPENDENT		= 0,
	TE_DEP_OTHER		= 1,
	TE_DEP_TSF		= 2,
	TE_EVENT_SOCIOPATHIC	= 4,
}; /*                                         */
/*
                                                    
                                                                                
                                                                       
                                                                            
                                      
  
                                   
                                                                          
                                                                     
                                                  
                                                 
                                                                        
                                                                   
                                                  
                                                
 */
enum {
	TE_NOTIF_NONE = 0,
	TE_NOTIF_HOST_EVENT_START = 0x1,
	TE_NOTIF_HOST_EVENT_END = 0x2,
	TE_NOTIF_INTERNAL_EVENT_START = 0x4,
	TE_NOTIF_INTERNAL_EVENT_END = 0x8,
	TE_NOTIF_HOST_FRAG_START = 0x10,
	TE_NOTIF_HOST_FRAG_END = 0x20,
	TE_NOTIF_INTERNAL_FRAG_START = 0x40,
	TE_NOTIF_INTERNAL_FRAG_END = 0x80
}; /*                              */

/*
                                                                 
                                                                        
                                    
                                                                      
                                        
                                                                               
                           
  
                                                                              
                                                                        
             
 */
enum {
	TE_FRAG_NONE = 0,
	TE_FRAG_SINGLE = 1,
	TE_FRAG_DUAL = 2,
	TE_FRAG_ENDLESS = 0xffffffff
};

/*                                                 */
#define TE_REPEAT_ENDLESS	(0xffffffff)
/*                                                                    */
#define TE_REPEAT_MAX_MSK	(0x0fffffff)
/*                                                                        */
#define TE_FRAG_MAX_MSK		(0x0fffffff)

/* 
                                                      
                            
                                                  
                                                      
                                                             
                                                                
                                                                    
                             
                                                     
                                                                 
                                                                
                                                 
                                        
                                     
                                                                
                                                               
                                                                      
                                                                           
                                                                
 */
struct iwl_time_event_cmd {
	/*                              */
	__le32 id_and_color;
	__le32 action;
	__le32 id;
	/*                                 */
	__le32 apply_time;
	__le32 max_delay;
	__le32 dep_policy;
	__le32 depends_on;
	__le32 is_present;
	__le32 max_frags;
	__le32 interval;
	__le32 interval_reciprocal;
	__le32 duration;
	__le32 repeat;
	__le32 notify;
} __packed; /*                                */

/* 
                                                                        
                                                              
                           
                                                                          
                                                  
 */
struct iwl_time_event_resp {
	__le32 status;
	__le32 id;
	__le32 unique_id;
	__le32 id_and_color;
} __packed; /*                                */

/* 
                                                                       
                                     
                                      
                                   
                                                 
                                                  
                                                 
                                                             
 */
struct iwl_time_event_notif {
	__le32 timestamp;
	__le32 session_id;
	__le32 unique_id;
	__le32 id_and_color;
	__le32 action;
	__le32 status;
} __packed; /*                                 */


/*                         */

/* 
                                                
                                 
                                                      
                                                      
                                                                
                                                      
 */
struct iwl_binding_cmd {
	/*                              */
	__le32 id_and_color;
	__le32 action;
	/*                          */
	__le32 macs[MAX_MACS_IN_BINDING];
	__le32 phy;
} __packed; /*                         */

/*                                                              */
#define IWL_MVM_MAX_QUOTA 128

/* 
                                                                       
                                                      
                                                                          
                                                              
                                                          
 */
struct iwl_time_quota_data {
	__le32 id_and_color;
	__le32 quota;
	__le32 max_duration;
} __packed; /*                             */

/* 
                                                                           
                            
                                   
 */
struct iwl_time_quota_cmd {
	struct iwl_time_quota_data quotas[MAX_BINDINGS];
} __packed; /*                                       */


/*             */

/*                 */
#define PHY_BAND_5  (0)
#define PHY_BAND_24 (1)

/*                                                       */
#define PHY_VHT_CHANNEL_MODE20	(0x0)
#define PHY_VHT_CHANNEL_MODE40	(0x1)
#define PHY_VHT_CHANNEL_MODE80	(0x2)
#define PHY_VHT_CHANNEL_MODE160	(0x3)

/*
                            
                                                           
                                                                              
                                                                             
                                                
                                           
                                                   
                                                           
                                                                           
                                                                       
 */
#define PHY_VHT_CTRL_POS_1_BELOW  (0x0)
#define PHY_VHT_CTRL_POS_2_BELOW  (0x1)
#define PHY_VHT_CTRL_POS_3_BELOW  (0x2)
#define PHY_VHT_CTRL_POS_4_BELOW  (0x3)
#define PHY_VHT_CTRL_POS_1_ABOVE  (0x4)
#define PHY_VHT_CTRL_POS_2_ABOVE  (0x5)
#define PHY_VHT_CTRL_POS_3_ABOVE  (0x6)
#define PHY_VHT_CTRL_POS_4_ABOVE  (0x7)

/*
                    
                           
                                     
                                         
 */
struct iwl_fw_channel_info {
	u8 band;
	u8 channel;
	u8 width;
	u8 ctrl_pos;
} __packed;

#define PHY_RX_CHAIN_DRIVER_FORCE_POS	(0)
#define PHY_RX_CHAIN_DRIVER_FORCE_MSK \
	(0x1 << PHY_RX_CHAIN_DRIVER_FORCE_POS)
#define PHY_RX_CHAIN_VALID_POS		(1)
#define PHY_RX_CHAIN_VALID_MSK \
	(0x7 << PHY_RX_CHAIN_VALID_POS)
#define PHY_RX_CHAIN_FORCE_SEL_POS	(4)
#define PHY_RX_CHAIN_FORCE_SEL_MSK \
	(0x7 << PHY_RX_CHAIN_FORCE_SEL_POS)
#define PHY_RX_CHAIN_FORCE_MIMO_SEL_POS	(7)
#define PHY_RX_CHAIN_FORCE_MIMO_SEL_MSK \
	(0x7 << PHY_RX_CHAIN_FORCE_MIMO_SEL_POS)
#define PHY_RX_CHAIN_CNT_POS		(10)
#define PHY_RX_CHAIN_CNT_MSK \
	(0x3 << PHY_RX_CHAIN_CNT_POS)
#define PHY_RX_CHAIN_MIMO_CNT_POS	(12)
#define PHY_RX_CHAIN_MIMO_CNT_MSK \
	(0x3 << PHY_RX_CHAIN_MIMO_CNT_POS)
#define PHY_RX_CHAIN_MIMO_FORCE_POS	(14)
#define PHY_RX_CHAIN_MIMO_FORCE_MSK \
	(0x1 << PHY_RX_CHAIN_MIMO_FORCE_POS)

/*                                                             */
#define NUM_PHY_CTX	3

/*                                      */
/* 
                                                         
                            
                                                      
                                                      
                                                           
                                                   
                       
                 
                     
                     
                         
                           
 */
struct iwl_phy_context_cmd {
	/*                              */
	__le32 id_and_color;
	__le32 action;
	/*                              */
	__le32 apply_time;
	__le32 tx_param_color;
	struct iwl_fw_channel_info ci;
	__le32 txchain_info;
	__le32 rxchain_info;
	__le32 acquisition_data;
	__le32 dsp_cfg_flags;
} __packed; /*                           */

#define IWL_RX_INFO_PHY_CNT 8
#define IWL_RX_INFO_AGC_IDX 1
#define IWL_RX_INFO_RSSI_AB_IDX 2
#define IWL_OFDM_AGC_A_MSK 0x0000007f
#define IWL_OFDM_AGC_A_POS 0
#define IWL_OFDM_AGC_B_MSK 0x00003f80
#define IWL_OFDM_AGC_B_POS 7
#define IWL_OFDM_AGC_CODE_MSK 0x3fe00000
#define IWL_OFDM_AGC_CODE_POS 20
#define IWL_OFDM_RSSI_INBAND_A_MSK 0x00ff
#define IWL_OFDM_RSSI_A_POS 0
#define IWL_OFDM_RSSI_ALLBAND_A_MSK 0xff00
#define IWL_OFDM_RSSI_ALLBAND_A_POS 8
#define IWL_OFDM_RSSI_INBAND_B_MSK 0xff0000
#define IWL_OFDM_RSSI_B_POS 16
#define IWL_OFDM_RSSI_ALLBAND_B_MSK 0xff000000
#define IWL_OFDM_RSSI_ALLBAND_B_POS 24

/* 
                                    
                            
                                                             
                                                     
                                             
              
                                         
                                 
                                            
                                                       
                           
                                                               
                            
                                  
                                                                           
              
                                                                     
  
                                                                          
                                     
 */
struct iwl_rx_phy_info {
	u8 non_cfg_phy_cnt;
	u8 cfg_phy_cnt;
	u8 stat_id;
	u8 reserved1;
	__le32 system_timestamp;
	__le64 timestamp;
	__le32 beacon_time_stamp;
	__le16 phy_flags;
	__le16 channel;
	__le32 non_cfg_phy[IWL_RX_INFO_PHY_CNT];
	__le32 rate_n_flags;
	__le32 byte_count;
	__le16 mac_active_msk;
	__le16 frame_time;
} __packed;

struct iwl_rx_mpdu_res_start {
	__le16 byte_count;
	__le16 reserved;
} __packed;

/* 
                                                              
                                                                         
                             
                                                                        
                                 
                                                                      
                                                                 
                                                       
                                                        
                                                        
 */
enum iwl_rx_phy_flags {
	RX_RES_PHY_FLAGS_BAND_24	= BIT(0),
	RX_RES_PHY_FLAGS_MOD_CCK	= BIT(1),
	RX_RES_PHY_FLAGS_SHORT_PREAMBLE	= BIT(2),
	RX_RES_PHY_FLAGS_NARROW_BAND	= BIT(3),
	RX_RES_PHY_FLAGS_ANTENNA	= (0x7 << 4),
	RX_RES_PHY_FLAGS_ANTENNA_POS	= 4,
	RX_RES_PHY_FLAGS_AGG		= BIT(7),
	RX_RES_PHY_FLAGS_OFDM_HT	= BIT(8),
	RX_RES_PHY_FLAGS_OFDM_GF	= BIT(9),
	RX_RES_PHY_FLAGS_OFDM_VHT	= BIT(10),
};

/* 
                                                            
                                          
                                                            
                                     
                                 
                                    
                                                                           
                                                                         
                 
                                            
                                                                         
                                                                        
                                               
                                                              
                                                                     
                                                                     
                                                                       
                                                                               
                                                                    
                                                                       
                                                                           
                                             
                                      
                                      
                                                                              
                                      
                                  
                                
                                     
                                      
 */
enum iwl_mvm_rx_status {
	RX_MPDU_RES_STATUS_CRC_OK			= BIT(0),
	RX_MPDU_RES_STATUS_OVERRUN_OK			= BIT(1),
	RX_MPDU_RES_STATUS_SRC_STA_FOUND		= BIT(2),
	RX_MPDU_RES_STATUS_KEY_VALID			= BIT(3),
	RX_MPDU_RES_STATUS_KEY_PARAM_OK			= BIT(4),
	RX_MPDU_RES_STATUS_ICV_OK			= BIT(5),
	RX_MPDU_RES_STATUS_MIC_OK			= BIT(6),
	RX_MPDU_RES_STATUS_TTAK_OK			= BIT(7),
	RX_MPDU_RES_STATUS_MNG_FRAME_REPLAY_ERR		= BIT(7),
	RX_MPDU_RES_STATUS_SEC_NO_ENC			= (0 << 8),
	RX_MPDU_RES_STATUS_SEC_WEP_ENC			= (1 << 8),
	RX_MPDU_RES_STATUS_SEC_CCM_ENC			= (2 << 8),
	RX_MPDU_RES_STATUS_SEC_TKIP_ENC			= (3 << 8),
	RX_MPDU_RES_STATUS_SEC_CCM_CMAC_ENC		= (6 << 8),
	RX_MPDU_RES_STATUS_SEC_ENC_ERR			= (7 << 8),
	RX_MPDU_RES_STATUS_SEC_ENC_MSK			= (7 << 8),
	RX_MPDU_RES_STATUS_DEC_DONE			= BIT(11),
	RX_MPDU_RES_STATUS_PROTECT_FRAME_BIT_CMP	= BIT(12),
	RX_MPDU_RES_STATUS_EXT_IV_BIT_CMP		= BIT(13),
	RX_MPDU_RES_STATUS_KEY_ID_CMP_BIT		= BIT(14),
	RX_MPDU_RES_STATUS_ROBUST_MNG_FRAME		= BIT(15),
	RX_MPDU_RES_STATUS_HASH_INDEX_MSK		= (0x3F0000),
	RX_MPDU_RES_STATUS_STA_ID_MSK			= (0x1f000000),
	RX_MPDU_RES_STATUS_RRF_KILL			= BIT(29),
	RX_MPDU_RES_STATUS_FILTERING_MSK		= (0xc00000),
	RX_MPDU_RES_STATUS2_FILTERING_MSK		= (0xc0000000),
};

/* 
                                                                    
                                        
                 
               
               
 */
struct iwl_radio_version_notif {
	__le32 radio_flavor;
	__le32 radio_step;
	__le32 radio_dash;
} __packed; /*                                    */

enum iwl_card_state_flags {
	CARD_ENABLED		= 0x00,
	HW_CARD_DISABLED	= 0x01,
	SW_CARD_DISABLED	= 0x02,
	CT_KILL_CARD_DISABLED	= 0x04,
	HALT_CARD_DISABLED	= 0x08,
	CARD_DISABLED_MSK	= 0x0f,
	CARD_IS_RX_ON		= 0x10,
};

/* 
                                                                    
                                     
                                
 */
struct iwl_card_state_notif {
	__le32 flags;
} __packed; /*                             */

/* 
                                                                        
                                   
                                                 
                   
                                                     
 */
struct iwl_set_calib_default_cmd {
	__le16 calib_index;
	__le16 length;
	u8 data[0];
} __packed; /*                             */

#define MAX_PORT_ID_NUM	2

/* 
                                                            
                                                                            
                                                                           
                                                            
                                                                   
                            
                                               
                                                  
                                     
                                                       
                                                                  
 */
struct iwl_mcast_filter_cmd {
	u8 filter_own;
	u8 port_id;
	u8 count;
	u8 pass_all;
	u8 bssid[6];
	u8 reserved[2];
	u8 addr_list[0];
} __packed; /*                                 */

#endif /*              */
