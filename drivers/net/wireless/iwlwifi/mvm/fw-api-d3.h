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
 *****************************************************************************/

#ifndef __fw_api_d3_h__
#define __fw_api_d3_h__

/* 
                                                     
                                                                
 */
enum iwl_d3_wakeup_flags {
	IWL_WAKEUP_D3_CONFIG_FW_ERROR = BIT(0),
}; /*                                      */

/* 
                                                                  
                                                
                                                             
                                                           
  
                                                       
 */
struct iwl_d3_manager_config {
	__le32 min_sleep_time;
	__le32 wakeup_flags;
	__le32 wakeup_host_timer;
} __packed; /*                               */


/*                                  */

/* 
                                                         
                                                 
                                                                  
 */
enum iwl_proto_offloads {
	IWL_D3_PROTO_OFFLOAD_ARP = BIT(0),
	IWL_D3_PROTO_OFFLOAD_NS = BIT(1),
};

#define IWL_PROTO_OFFLOAD_NUM_IPV6_ADDRS	2

/* 
                                                              
                         
                                                                  
                                                              
                                                   
                                                                  
                                                                     
                          
                                          
                                                           
 */
struct iwl_proto_offload_cmd {
	__le32 enabled;
	__be32 remote_ipv4_addr;
	__be32 host_ipv4_addr;
	u8 arp_mac_addr[ETH_ALEN];
	__le16 reserved1;

	u8 remote_ipv6_addr[16];
	u8 solicited_node_ipv6_addr[16];
	u8 target_ipv6_addr[IWL_PROTO_OFFLOAD_NUM_IPV6_ADDRS][16];
	u8 ndp_mac_addr[ETH_ALEN];
	__le16 reserved2;
} __packed; /*                                    */


/*
                  
 */
#define IWL_WOWLAN_MIN_PATTERN_LEN	16
#define IWL_WOWLAN_MAX_PATTERN_LEN	128

struct iwl_wowlan_pattern {
	u8 mask[IWL_WOWLAN_MAX_PATTERN_LEN / 8];
	u8 pattern[IWL_WOWLAN_MAX_PATTERN_LEN];
	u8 mask_size;
	u8 pattern_size;
	__le16 reserved;
} __packed; /*                            */

#define IWL_WOWLAN_MAX_PATTERNS	20

struct iwl_wowlan_patterns_cmd {
	__le32 n_patterns;
	struct iwl_wowlan_pattern patterns[];
} __packed; /*                                  */

enum iwl_wowlan_wakeup_filters {
	IWL_WOWLAN_WAKEUP_MAGIC_PACKET			= BIT(0),
	IWL_WOWLAN_WAKEUP_PATTERN_MATCH			= BIT(1),
	IWL_WOWLAN_WAKEUP_BEACON_MISS			= BIT(2),
	IWL_WOWLAN_WAKEUP_LINK_CHANGE			= BIT(3),
	IWL_WOWLAN_WAKEUP_GTK_REKEY_FAIL		= BIT(4),
	IWL_WOWLAN_WAKEUP_EAP_IDENT_REQ			= BIT(5),
	IWL_WOWLAN_WAKEUP_4WAY_HANDSHAKE		= BIT(6),
	IWL_WOWLAN_WAKEUP_ENABLE_NET_DETECT		= BIT(7),
	IWL_WOWLAN_WAKEUP_RF_KILL_DEASSERT		= BIT(8),
	IWL_WOWLAN_WAKEUP_REMOTE_LINK_LOSS		= BIT(9),
	IWL_WOWLAN_WAKEUP_REMOTE_SIGNATURE_TABLE	= BIT(10),
	/*                  */
	IWL_WOWLAN_WAKEUP_REMOTE_WAKEUP_PACKET		= BIT(12),
}; /*                                  */

struct iwl_wowlan_config_cmd {
	__le32 wakeup_filter;
	__le16 non_qos_seq;
	__le16 qos_seq[8];
	u8 wowlan_ba_teardown_tids;
	u8 is_11n_connection;
} __packed; /*                           */

/*
                        
 */
#define IWL_NUM_RSC	16

struct tkip_sc {
	__le16 iv16;
	__le16 pad;
	__le32 iv32;
} __packed; /*                     */

struct iwl_tkip_rsc_tsc {
	struct tkip_sc unicast_rsc[IWL_NUM_RSC];
	struct tkip_sc multicast_rsc[IWL_NUM_RSC];
	struct tkip_sc tsc;
} __packed; /*                          */

struct aes_sc {
	__le64 pn;
} __packed; /*                         */

struct iwl_aes_rsc_tsc {
	struct aes_sc unicast_rsc[IWL_NUM_RSC];
	struct aes_sc multicast_rsc[IWL_NUM_RSC];
	struct aes_sc tsc;
} __packed; /*                         */

union iwl_all_tsc_rsc {
	struct iwl_tkip_rsc_tsc tkip;
	struct iwl_aes_rsc_tsc aes;
}; /*                         */

struct iwl_wowlan_rsc_tsc_params_cmd {
	union iwl_all_tsc_rsc all_tsc_rsc;
} __packed; /*                         */

#define IWL_MIC_KEY_SIZE	8
struct iwl_mic_keys {
	u8 tx[IWL_MIC_KEY_SIZE];
	u8 rx_unicast[IWL_MIC_KEY_SIZE];
	u8 rx_mcast[IWL_MIC_KEY_SIZE];
} __packed; /*                      */

#define IWL_P1K_SIZE		5
struct iwl_p1k_cache {
	__le16 p1k[IWL_P1K_SIZE];
} __packed;

#define IWL_NUM_RX_P1K_CACHE	2

struct iwl_wowlan_tkip_params_cmd {
	struct iwl_mic_keys mic_keys;
	struct iwl_p1k_cache tx;
	struct iwl_p1k_cache rx_uni[IWL_NUM_RX_P1K_CACHE];
	struct iwl_p1k_cache rx_multi[IWL_NUM_RX_P1K_CACHE];
} __packed; /*                                 */

#define IWL_KCK_MAX_SIZE	32
#define IWL_KEK_MAX_SIZE	32

struct iwl_wowlan_kek_kck_material_cmd {
	u8	kck[IWL_KCK_MAX_SIZE];
	u8	kek[IWL_KEK_MAX_SIZE];
	__le16	kck_len;
	__le16	kek_len;
	__le64	replay_ctr;
} __packed; /*                              */

#define RF_KILL_INDICATOR_FOR_WOWLAN	0x87

enum iwl_wowlan_rekey_status {
	IWL_WOWLAN_REKEY_POST_REKEY = 0,
	IWL_WOWLAN_REKEY_WHILE_REKEY = 1,
}; /*                                 */

enum iwl_wowlan_wakeup_reason {
	IWL_WOWLAN_WAKEUP_BY_NON_WIRELESS			= 0,
	IWL_WOWLAN_WAKEUP_BY_MAGIC_PACKET			= BIT(0),
	IWL_WOWLAN_WAKEUP_BY_PATTERN				= BIT(1),
	IWL_WOWLAN_WAKEUP_BY_DISCONNECTION_ON_MISSED_BEACON	= BIT(2),
	IWL_WOWLAN_WAKEUP_BY_DISCONNECTION_ON_DEAUTH		= BIT(3),
	IWL_WOWLAN_WAKEUP_BY_GTK_REKEY_FAILURE			= BIT(4),
	IWL_WOWLAN_WAKEUP_BY_RFKILL_DEASSERTED			= BIT(5),
	IWL_WOWLAN_WAKEUP_BY_UCODE_ERROR			= BIT(6),
	IWL_WOWLAN_WAKEUP_BY_EAPOL_REQUEST			= BIT(7),
	IWL_WOWLAN_WAKEUP_BY_FOUR_WAY_HANDSHAKE			= BIT(8),
	IWL_WOWLAN_WAKEUP_BY_REM_WAKE_LINK_LOSS			= BIT(9),
	IWL_WOWLAN_WAKEUP_BY_REM_WAKE_SIGNATURE_TABLE		= BIT(10),
	/*                  */
	IWL_WOWLAN_WAKEUP_BY_REM_WAKE_WAKEUP_PACKET		= BIT(12),
}; /*                                   */

struct iwl_wowlan_status {
	__le64 replay_ctr;
	__le16 pattern_number;
	__le16 non_qos_seq_ctr;
	__le16 qos_seq_ctr[8];
	__le32 wakeup_reasons;
	__le32 rekey_status;
	__le32 num_of_gtk_rekeys;
	__le32 transmitted_ndps;
	__le32 received_beacons;
	__le32 wake_packet_length;
	__le32 wake_packet_bufsize;
	u8 wake_packet[]; /*                                           */
} __packed; /*                             */

#define IWL_WOWLAN_TCP_MAX_PACKET_LEN		64
#define IWL_WOWLAN_REMOTE_WAKE_MAX_PACKET_LEN	128
#define IWL_WOWLAN_REMOTE_WAKE_MAX_TOKENS	2048

struct iwl_tcp_packet_info {
	__le16 tcp_pseudo_header_checksum;
	__le16 tcp_payload_length;
} __packed; /*                             */

struct iwl_tcp_packet {
	struct iwl_tcp_packet_info info;
	u8 rx_mask[IWL_WOWLAN_MAX_PATTERN_LEN / 8];
	u8 data[IWL_WOWLAN_TCP_MAX_PACKET_LEN];
} __packed; /*                                 */

struct iwl_remote_wake_packet {
	struct iwl_tcp_packet_info info;
	u8 rx_mask[IWL_WOWLAN_MAX_PATTERN_LEN / 8];
	u8 data[IWL_WOWLAN_REMOTE_WAKE_MAX_PACKET_LEN];
} __packed; /*                                 */

struct iwl_wowlan_remote_wake_config {
	__le32 connection_max_time; /*        */
	/*                                 */
	u8 max_syn_retries;
	u8 max_data_retries;
	u8 tcp_syn_ack_timeout;
	u8 tcp_ack_timeout;

	struct iwl_tcp_packet syn_tx;
	struct iwl_tcp_packet synack_rx;
	struct iwl_tcp_packet keepalive_ack_rx;
	struct iwl_tcp_packet fin_tx;

	struct iwl_remote_wake_packet keepalive_tx;
	struct iwl_remote_wake_packet wake_rx;

	/*                                     */
	u8 sequence_number_offset;
	u8 sequence_number_length;
	u8 token_offset;
	u8 token_length;
	/*                                         */
	__le32 initial_sequence_number;
	__le16 keepalive_interval;
	__le16 num_tokens;
	u8 tokens[IWL_WOWLAN_REMOTE_WAKE_MAX_TOKENS];
} __packed; /*                                */

/*                     */

#endif /*                 */
