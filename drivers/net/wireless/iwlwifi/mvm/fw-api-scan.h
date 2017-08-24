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

#ifndef __fw_api_scan_h__
#define __fw_api_scan_h__

#include "fw-api.h"

/*                                         */

/*                                       */
#define SCAN_CHANNEL_TYPE_ACTIVE	BIT(0)
#define SCAN_CHANNEL_NARROW_BAND	BIT(22)

/*                                                      */
#define PROBE_OPTION_MAX		20

/* 
                                                                  
                                                           
                                  
                           
                                                                  
                                                                      
                                            
                                 
                                                                   
                                                             
                                                      
                                                             
  
                    
                       
                                                              
                                         
                                     
                                                             
                                                                           
                                  
  
                                                                        
                                                                
                                                        
                                                                          
                                 
                                                               
                                   
                                  
 */
struct iwl_scan_channel {
	__le32 type;
	__le16 channel;
	__le16 iteration_count;
	__le32 iteration_interval;
	__le16 active_dwell;
	__le16 passive_dwell;
} __packed; /*                                  */

/* 
                                                                 
  
                                                  
                                                           
                                                                     
                                                      
 */
struct iwl_ssid_ie {
	u8 id;
	u8 len;
	u8 ssid[IEEE80211_MAX_SSID_LEN];
} __packed; /*                                 */

/* 
                                                
                            
                                         
                                   
                                    
                              
 */
enum iwl_scan_flags {
	SCAN_FLAGS_PERIODIC_SCAN		= BIT(0),
	SCAN_FLAGS_P2P_PUBLIC_ACTION_FRAME_TX	= BIT(1),
	SCAN_FLAGS_DELAYED_SCAN_LOWBAND		= BIT(2),
	SCAN_FLAGS_DELAYED_SCAN_HIGHBAND	= BIT(3),
	SCAN_FLAGS_FRAGMENTED_SCAN		= BIT(4),
};

/* 
                                                   
                     
                         
                 
                      
                     
                        
                               
 */
enum iwl_scan_type {
	SCAN_TYPE_FORCED		= 0,
	SCAN_TYPE_BACKGROUND		= 1,
	SCAN_TYPE_OS			= 2,
	SCAN_TYPE_ROAMING		= 3,
	SCAN_TYPE_ACTION		= 4,
	SCAN_TYPE_DISCOVERY		= 5,
	SCAN_TYPE_DISCOVERY_FORCED	= 6,
}; /*                            */

/*                                    */
#define MAX_NUM_SCAN_CHANNELS 0x24

/* 
                                             
                              
                                
                                            
                                                                              
                                                                           
                                                                      
                                                     
                                                                             
                                      
                                                           
                                                                           
                                                                
                       
                                                           
                          
                               
                                                             
                           
                                  
                            
                                             
 */
struct iwl_scan_cmd {
	__le16 len;
	u8 scan_flags;
	u8 channel_count;
	__le16 quiet_time;
	__le16 quiet_plcp_th;
	__le16 passive2active;
	__le16 rxchain_sel_flags;
	__le32 max_out_time;
	__le32 suspend_time;
	/*                         */
	__le32 rxon_flags;
	__le32 filter_flags;
	struct iwl_tx_cmd tx_cmd;
	struct iwl_ssid_ie direct_scan[PROBE_OPTION_MAX];
	__le32 type;
	__le32 repeats;

	/*
                                                  
   
                                                                     
                                                               
                                                             
                                    
   
                                        
   
                                                                  
                                                                       
                                                                      
                                   
  */
	u8 data[0];
} __packed; /*                                     */

/*                                                                        */
#define SCAN_RESPONSE_OK	0x1
#define SCAN_RESPONSE_ERROR	0x2

/*
                        
                                                                            
                                                                        
 */
#define SCAN_ABORT_POSSIBLE	0x1
#define SCAN_ABORT_IGNORED	0x2 /*                  */

/*                              */
#define  SCAN_OWNER_STATUS 0x1
#define  MEASURE_OWNER_STATUS 0x2

/* 
                                                                     
                                     
                                            
                                              
                                        
                                       
                            
                                 
                                     
                                      
                                 
 */
struct iwl_scan_start_notif {
	__le32 tsf_low;
	__le32 tsf_high;
	__le32 beacon_timer;
	u8 channel;
	u8 band;
	u8 reserved[2];
	__le32 status;
} __packed; /*                            */

/*                                                       */
#define SCAN_PROBE_STATUS_OK		0
#define SCAN_PROBE_STATUS_TX_FAILED	BIT(0)
/*                                        */
#define SCAN_PROBE_STATUS_FAIL_TTL	BIT(1)
#define SCAN_PROBE_STATUS_FAIL_BT	BIT(2)

/*                                                   */
#define SCAN_RESULTS_STATISTICS 1

/* 
                                                                               
                                                     
                                                    
                                                                
                                                                      
                                                            
                                                                  
                                                                          
                                                                   
                                                       
                                                                             
                  
                                                                               
            
                                                                      
*/
enum iwl_scan_complete_status {
	SCAN_COMP_STATUS_OK = 0x1,
	SCAN_COMP_STATUS_ABORT = 0x2,
	SCAN_COMP_STATUS_ERR_SLEEP = 0x3,
	SCAN_COMP_STATUS_ERR_CHAN_TIMEOUT = 0x4,
	SCAN_COMP_STATUS_ERR_PROBE = 0x5,
	SCAN_COMP_STATUS_ERR_WAKEUP = 0x6,
	SCAN_COMP_STATUS_ERR_ANTENNAS = 0x7,
	SCAN_COMP_STATUS_ERR_INTERNAL = 0x8,
	SCAN_COMP_STATUS_ERR_COEX = 0x9,
	SCAN_COMP_STATUS_P2P_ACTION_OK = 0xA,
	SCAN_COMP_STATUS_ITERATION_END = 0x0B,
	SCAN_COMP_STATUS_ERR_ALLOC_TE = 0x0C,
};

/* 
                                                               
                                       
                                               
                                      
                                                                         
                                                                             
                                                 
                                                    
 */
struct iwl_scan_results_notif {
	u8 channel;
	u8 band;
	u8 probe_status;
	u8 num_probe_not_sent;
	__le32 duration;
	__le32 statistics[SCAN_RESULTS_STATISTICS];
} __packed; /*                             */

/* 
                                                                           
                                        
                                                                              
                                     
                               
                                               
                                            
                                              
                                                                        
 */
struct iwl_scan_complete_notif {
	u8 scanned_channels;
	u8 status;
	u8 bt_status;
	u8 last_channel;
	__le32 tsf_low;
	__le32 tsf_high;
	struct iwl_scan_results_notif results[MAX_NUM_SCAN_CHANNELS];
} __packed; /*                               */

/*              */
#define IWL_MAX_SCAN_CHANNELS		40
#define IWL_SCAN_MAX_BLACKLIST_LEN	64
#define IWL_SCAN_MAX_PROFILES		11
#define SCAN_OFFLOAD_PROBE_REQ_SIZE	512

/*                                                       */
#define IWL_SCHED_SCAN_WATCHDOG cpu_to_le16(15000)

#define IWL_GOOD_CRC_TH_DEFAULT cpu_to_le16(1)
#define CAN_ABORT_STATUS 1

#define IWL_FULL_SCAN_MULTIPLIER 5
#define IWL_FAST_SCHED_SCAN_ITERATIONS 3

/* 
                                                                    
                                        
                                           
                                                             
                                                         
                                                      
                             
                                                          
                                                                        
                      
                            
                                                               
                                                       
                                       
                                                                   
 */
struct iwl_scan_offload_cmd {
	__le16 len;
	u8 scan_flags;
	u8 channel_count;
	__le16 quiet_time;
	__le16 quiet_plcp_th;
	__le16 good_CRC_th;
	__le16 rx_chain;
	__le32 max_out_time;
	__le32 suspend_time;
	/*                         */
	__le32 flags;
	__le32 filter_flags;
	struct iwl_tx_cmd tx_cmd[2];
	/*                                 */
	struct iwl_ssid_ie direct_scan[PROBE_OPTION_MAX];
	__le32 scan_type;
	__le32 rep_count;
} __packed;

enum iwl_scan_offload_channel_flags {
	IWL_SCAN_OFFLOAD_CHANNEL_ACTIVE		= BIT(0),
	IWL_SCAN_OFFLOAD_CHANNEL_NARROW		= BIT(22),
	IWL_SCAN_OFFLOAD_CHANNEL_FULL		= BIT(24),
	IWL_SCAN_OFFLOAD_CHANNEL_PARTIAL	= BIT(25),
};

/* 
                                            
                                                            
                                       
                                      
                                                   
                   
                      
                                            
                                                  
                                                                   
                                                              
 */
struct iwl_scan_channel_cfg {
	__le32 type[IWL_MAX_SCAN_CHANNELS];
	__le16 channel_number[IWL_MAX_SCAN_CHANNELS];
	__le16 iter_count[IWL_MAX_SCAN_CHANNELS];
	__le32 iter_interval[IWL_MAX_SCAN_CHANNELS];
	u8 dwell_time[IWL_MAX_SCAN_CHANNELS][2];
} __packed;

/* 
                                                   
                                      
                                           
                                              
 */
struct iwl_scan_offload_cfg {
	struct iwl_scan_offload_cmd scan_cmd;
	struct iwl_scan_channel_cfg channel_cfg;
	u8 data[0];
} __packed;

/* 
                                                        
                                    
                                               
 */
struct iwl_scan_offload_blacklist {
	u8 ssid[ETH_ALEN];
	u8 reported_rssi;
	u8 reserved;
} __packed;

enum iwl_scan_offload_network_type {
	IWL_NETWORK_TYPE_BSS	= 1,
	IWL_NETWORK_TYPE_IBSS	= 2,
	IWL_NETWORK_TYPE_ANY	= 3,
};

enum iwl_scan_offload_band_selection {
	IWL_SCAN_OFFLOAD_SELECT_2_4	= 0x4,
	IWL_SCAN_OFFLOAD_SELECT_5_2	= 0x8,
	IWL_SCAN_OFFLOAD_SELECT_ANY	= 0xc,
};

/* 
                                                    
                                                 
                                                          
                                                        
                                                    
                                                        
 */
struct iwl_scan_offload_profile {
	u8 ssid_index;
	u8 unicast_cipher;
	u8 auth_alg;
	u8 network_type;
	u8 band_selection;
	u8 reserved[3];
} __packed;

/* 
                                                                       
                                                      
                                           
                                      
                                             
 */
struct iwl_scan_offload_profile_cfg {
	struct iwl_scan_offload_blacklist blacklist[IWL_SCAN_MAX_BLACKLIST_LEN];
	struct iwl_scan_offload_profile profiles[IWL_SCAN_MAX_PROFILES];
	u8 blacklist_len;
	u8 num_profiles;
	u8 reserved[2];
} __packed;

/* 
                                                       
                                                 
                                       
                                                                
 */
struct iwl_scan_offload_schedule {
	u16 delay;
	u8 iterations;
	u8 full_scan_mul;
} __packed;

/*
                         
  
                                                                                
             
                                                                             
                                                                               
             
 */
enum iwl_scan_offload_flags {
	IWL_SCAN_OFFLOAD_FLAG_FILTER_SSID	= BIT(0),
	IWL_SCAN_OFFLOAD_FLAG_CACHED_CHANNEL	= BIT(2),
	IWL_SCAN_OFFLOAD_FLAG_ENERGY_SCAN	= BIT(3),
};

/* 
                                                      
                                                 
                                           
                                                    
                                                                    
 */
struct iwl_scan_offload_req {
	__le16 flags;
	__le16 watchdog;
	__le16 delay;
	__le16 reserved;
	struct iwl_scan_offload_schedule schedule_line[2];
} __packed;

enum iwl_scan_offload_compleate_status {
	IWL_SCAN_OFFLOAD_COMPLETED	= 1,
	IWL_SCAN_OFFLOAD_ABORTED	= 2,
};

/* 
                                                                    
                                                                      
                                                                           
                                                    
 */
struct iwl_scan_offload_complete {
	u8 last_schedule_line;
	u8 last_schedule_iteration;
	u8 status;
	u8 reserved;
} __packed;

#endif
