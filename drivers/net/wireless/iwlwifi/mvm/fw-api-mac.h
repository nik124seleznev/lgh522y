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

#ifndef __fw_api_mac_h__
#define __fw_api_mac_h__

/*
                                          
                                           
 */
#define MAC_INDEX_AUX		4
#define MAC_INDEX_MIN_DRIVER	0
#define NUM_MAC_INDEX_DRIVER	MAC_INDEX_AUX

enum iwl_ac {
	AC_BK,
	AC_BE,
	AC_VI,
	AC_VO,
	AC_NUM,
};

/* 
                                                    
                                                                           
                                      
                                        
                                              
                                                       
                                            
 */
enum iwl_mac_protection_flags {
	MAC_PROT_FLG_TGG_PROTECT	= BIT(3),
	MAC_PROT_FLG_HT_PROT		= BIT(23),
	MAC_PROT_FLG_FAT_PROT		= BIT(24),
	MAC_PROT_FLG_SELF_CTS_EN	= BIT(30),
};

#define MAC_FLG_SHORT_SLOT		BIT(4)
#define MAC_FLG_SHORT_PREAMBLE		BIT(5)

/* 
                                           
                                                
                                             
                                              
                                  
                          
                                              
                                      
                                   
                          
                       
                                             
 */
enum iwl_mac_types {
	FW_MAC_TYPE_FIRST = 1,
	FW_MAC_TYPE_AUX = FW_MAC_TYPE_FIRST,
	FW_MAC_TYPE_LISTENER,
	FW_MAC_TYPE_PIBSS,
	FW_MAC_TYPE_IBSS,
	FW_MAC_TYPE_BSS_STA,
	FW_MAC_TYPE_P2P_DEVICE,
	FW_MAC_TYPE_P2P_STA,
	FW_MAC_TYPE_GO,
	FW_MAC_TYPE_TEST,
	FW_MAC_TYPE_MAX = FW_MAC_TYPE_TEST
}; /*                              */

/* 
                                    
                       
                       
                       
                       
                                               
 */
enum iwl_tsf_id {
	TSF_ID_A = 0,
	TSF_ID_B = 1,
	TSF_ID_C = 2,
	TSF_ID_D = 3,
	NUM_TSF_IDS = 4,
}; /*                    */

/* 
                                                                 
                                                    
                                           
                             
                            
                                           
                                         
                                             
                                       
 */
struct iwl_mac_data_ap {
	__le32 beacon_time;
	__le64 beacon_tsf;
	__le32 bi;
	__le32 bi_reciprocal;
	__le32 dtim_interval;
	__le32 dtim_reciprocal;
	__le32 mcast_qid;
	__le32 beacon_template;
} __packed; /*                         */

/* 
                                                                     
                                                    
                                           
                             
                            
 */
struct iwl_mac_data_ibss {
	__le32 beacon_time;
	__le64 beacon_tsf;
	__le32 bi;
	__le32 bi_reciprocal;
} __packed; /*                           */

/* 
                                                                       
                                                 
                                               
                                      
                                                              
                                                              
                                                                       
                                                                           
                                                                         
                                                             
 */
struct iwl_mac_data_sta {
	__le32 is_assoc;
	__le32 dtim_time;
	__le64 dtim_tsf;
	__le32 bi;
	__le32 bi_reciprocal;
	__le32 dtim_interval;
	__le32 dtim_reciprocal;
	__le32 listen_interval;
	__le32 assoc_id;
	__le32 assoc_beacon_arrive_time;
} __packed; /*                          */

/* 
                                                                     
                                                    
                                                                              
                                          
                                                          
 */
struct iwl_mac_data_go {
	struct iwl_mac_data_ap ap;
	__le32 ctwin;
	__le32 opp_ps_enabled;
} __packed; /*                         */

/* 
                                                                              
                                                      
                                                                              
                                          
 */
struct iwl_mac_data_p2p_sta {
	struct iwl_mac_data_sta sta;
	__le32 ctwin;
} __packed; /*                              */

/* 
                                                      
                                                                            
 */
struct iwl_mac_data_pibss {
	__le32 stats_interval;
} __packed; /*                            */

/*
                                                                          
           
                                                                              
                                                                       
                                                                           
                                                                         
                                                                        
            
 */
struct iwl_mac_data_p2p_dev {
	__le32 is_disc_extended;
} __packed; /*                               */

/* 
                                                       
                                                 
                                                          
                             
                                                  
                                                        
                                                              
                                                                
                                    
                                                         
                                                            
                                                            
 */
enum iwl_mac_filter_flags {
	MAC_FILTER_IN_PROMISC		= BIT(0),
	MAC_FILTER_IN_CONTROL_AND_MGMT	= BIT(1),
	MAC_FILTER_ACCEPT_GRP		= BIT(2),
	MAC_FILTER_DIS_DECRYPT		= BIT(3),
	MAC_FILTER_DIS_GRP_DECRYPT	= BIT(4),
	MAC_FILTER_IN_BEACON		= BIT(6),
	MAC_FILTER_OUT_BCAST		= BIT(8),
	MAC_FILTER_IN_CRC32		= BIT(11),
	MAC_FILTER_IN_PROBE_REQUEST	= BIT(12),
};

/* 
                                     
                              
                                  
                            
  
 */
enum iwl_mac_qos_flags {
	MAC_QOS_FLG_UPDATE_EDCA	= BIT(0),
	MAC_QOS_FLG_TGN		= BIT(1),
	MAC_QOS_FLG_TXOP_TYPE	= BIT(4),
};

/* 
                                                            
                                                               
                                                              
                                                             
                                                              
                                                                   
                                                                    
                                                  
                                                                         
  
                                                                          
                              
  
                                                                              
                                                                           
                              
 */
struct iwl_ac_qos {
	__le16 cw_min;
	__le16 cw_max;
	u8 aifsn;
	u8 fifos_mask;
	__le16 edca_txop;
} __packed; /*                    */

/* 
                                                                       
                             
                                         
                                                      
                                  
                                         
                          
                     
                                            
                                              
                                                        
                                                                     
                                                          
                                             
                                 
                                                 
                                                                     
 */
struct iwl_mac_ctx_cmd {
	/*                              */
	__le32 id_and_color;
	__le32 action;
	/*                                     */
	__le32 mac_type;
	__le32 tsf_id;
	u8 node_addr[6];
	__le16 reserved_for_node_addr;
	u8 bssid_addr[6];
	__le16 reserved_for_bssid_addr;
	__le32 cck_rates;
	__le32 ofdm_rates;
	__le32 protection_flags;
	__le32 cck_short_preamble;
	__le32 short_slot;
	__le32 filter_flags;
	/*                           */
	__le32 qos_flags;
	struct iwl_ac_qos ac[AC_NUM+1];
	/*                               */
	union {
		struct iwl_mac_data_ap ap;
		struct iwl_mac_data_go go;
		struct iwl_mac_data_sta sta;
		struct iwl_mac_data_p2p_sta p2p_sta;
		struct iwl_mac_data_p2p_dev p2p_dev;
		struct iwl_mac_data_pibss pibss;
		struct iwl_mac_data_ibss ibss;
	};
} __packed; /*                             */

static inline u32 iwl_mvm_reciprocal(u32 v)
{
	if (!v)
		return 0;
	return 0xFFFFFFFF / v;
}

#endif /*                  */
