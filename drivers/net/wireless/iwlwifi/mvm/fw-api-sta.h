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

#ifndef __fw_api_sta_h__
#define __fw_api_sta_h__

/* 
                                                          
                                
                                
                                          
                                           
                                          
                                                   
                                                                
                                  
                                         
                       
                        
                            
                                                     
                                                                      
                                                                        
                                                                   
                                                                       
                                                                        
                                                                          
                                                                   
                                               
 */
enum iwl_sta_flags {
	STA_FLG_REDUCED_TX_PWR_CTRL	= BIT(3),
	STA_FLG_REDUCED_TX_PWR_DATA	= BIT(6),

	STA_FLG_FLG_ANT_A		= (1 << 4),
	STA_FLG_FLG_ANT_B		= (2 << 4),
	STA_FLG_FLG_ANT_MSK		= (STA_FLG_FLG_ANT_A |
					   STA_FLG_FLG_ANT_B),

	STA_FLG_PS			= BIT(8),
	STA_FLG_INVALID			= BIT(9),
	STA_FLG_DLP_EN			= BIT(10),
	STA_FLG_SET_ALL_KEYS		= BIT(11),
	STA_FLG_DRAIN_FLOW		= BIT(12),
	STA_FLG_PAN			= BIT(13),
	STA_FLG_CLASS_AUTH		= BIT(14),
	STA_FLG_CLASS_ASSOC		= BIT(15),
	STA_FLG_RTS_MIMO_PROT		= BIT(17),

	STA_FLG_MAX_AGG_SIZE_SHIFT	= 19,
	STA_FLG_MAX_AGG_SIZE_8K		= (0 << STA_FLG_MAX_AGG_SIZE_SHIFT),
	STA_FLG_MAX_AGG_SIZE_16K	= (1 << STA_FLG_MAX_AGG_SIZE_SHIFT),
	STA_FLG_MAX_AGG_SIZE_32K	= (2 << STA_FLG_MAX_AGG_SIZE_SHIFT),
	STA_FLG_MAX_AGG_SIZE_64K	= (3 << STA_FLG_MAX_AGG_SIZE_SHIFT),
	STA_FLG_MAX_AGG_SIZE_128K	= (4 << STA_FLG_MAX_AGG_SIZE_SHIFT),
	STA_FLG_MAX_AGG_SIZE_256K	= (5 << STA_FLG_MAX_AGG_SIZE_SHIFT),
	STA_FLG_MAX_AGG_SIZE_512K	= (6 << STA_FLG_MAX_AGG_SIZE_SHIFT),
	STA_FLG_MAX_AGG_SIZE_1024K	= (7 << STA_FLG_MAX_AGG_SIZE_SHIFT),
	STA_FLG_MAX_AGG_SIZE_MSK	= (7 << STA_FLG_MAX_AGG_SIZE_SHIFT),

	STA_FLG_AGG_MPDU_DENS_SHIFT	= 23,
	STA_FLG_AGG_MPDU_DENS_2US	= (4 << STA_FLG_AGG_MPDU_DENS_SHIFT),
	STA_FLG_AGG_MPDU_DENS_4US	= (5 << STA_FLG_AGG_MPDU_DENS_SHIFT),
	STA_FLG_AGG_MPDU_DENS_8US	= (6 << STA_FLG_AGG_MPDU_DENS_SHIFT),
	STA_FLG_AGG_MPDU_DENS_16US	= (7 << STA_FLG_AGG_MPDU_DENS_SHIFT),
	STA_FLG_AGG_MPDU_DENS_MSK	= (7 << STA_FLG_AGG_MPDU_DENS_SHIFT),

	STA_FLG_FAT_EN_20MHZ		= (0 << 26),
	STA_FLG_FAT_EN_40MHZ		= (1 << 26),
	STA_FLG_FAT_EN_80MHZ		= (2 << 26),
	STA_FLG_FAT_EN_160MHZ		= (3 << 26),
	STA_FLG_FAT_EN_MSK		= (3 << 26),

	STA_FLG_MIMO_EN_SISO		= (0 << 28),
	STA_FLG_MIMO_EN_MIMO2		= (1 << 28),
	STA_FLG_MIMO_EN_MIMO3		= (2 << 28),
	STA_FLG_MIMO_EN_MSK		= (3 << 28),
};

/* 
                                                                 
                                                     
                                                                               
                                     
                                               
                                     
                                                     
                                           
                                                            
 */
enum iwl_sta_key_flag {
	STA_KEY_FLG_NO_ENC		= (0 << 0),
	STA_KEY_FLG_WEP			= (1 << 0),
	STA_KEY_FLG_CCM			= (2 << 0),
	STA_KEY_FLG_TKIP		= (3 << 0),
	STA_KEY_FLG_CMAC		= (6 << 0),
	STA_KEY_FLG_ENC_UNKNOWN		= (7 << 0),
	STA_KEY_FLG_EN_MSK		= (7 << 0),

	STA_KEY_FLG_WEP_KEY_MAP		= BIT(3),
	STA_KEY_FLG_KEYID_POS		 = 8,
	STA_KEY_FLG_KEYID_MSK		= (3 << STA_KEY_FLG_KEYID_POS),
	STA_KEY_NOT_VALID		= BIT(11),
	STA_KEY_FLG_WEP_13BYTES		= BIT(12),
	STA_KEY_MULTICAST		= BIT(14),
	STA_KEY_MFP			= BIT(15),
};

/* 
                                                                            
                                              
                                                                    
                              
                                                                      
                                                                            
                                                                           
                       
                                                             
 */
enum iwl_sta_modify_flag {
	STA_MODIFY_KEY				= BIT(0),
	STA_MODIFY_TID_DISABLE_TX		= BIT(1),
	STA_MODIFY_TX_RATE			= BIT(2),
	STA_MODIFY_ADD_BA_TID			= BIT(3),
	STA_MODIFY_REMOVE_BA_TID		= BIT(4),
	STA_MODIFY_SLEEPING_STA_TX_COUNT	= BIT(5),
	STA_MODIFY_PROT_TH			= BIT(6),
	STA_MODIFY_QUEUES			= BIT(7),
};

#define STA_MODE_MODIFY	1

/* 
                                                         
                          
                            
                          
 */
enum iwl_sta_sleep_flag {
	STA_SLEEP_STATE_AWAKE	= 0,
	STA_SLEEP_STATE_PS_POLL	= BIT(0),
	STA_SLEEP_STATE_UAPSD	= BIT(1),
};

/*                                   */
#define STA_ID_SEED		(0x0f)
#define STA_ID_POS		(0)
#define STA_ID_MSK		(STA_ID_SEED << STA_ID_POS)

#define STA_COLOR_SEED		(0x7)
#define STA_COLOR_POS		(4)
#define STA_COLOR_MSK		(STA_COLOR_SEED << STA_COLOR_POS)

#define STA_ID_N_COLOR_GET_COLOR(id_n_color) \
	(((id_n_color) & STA_COLOR_MSK) >> STA_COLOR_POS)
#define STA_ID_N_COLOR_GET_ID(id_n_color)    \
	(((id_n_color) & STA_ID_MSK) >> STA_ID_POS)

#define STA_KEY_MAX_NUM (16)
#define STA_KEY_IDX_INVALID (0xff)
#define STA_KEY_MAX_DATA_KEY_NUM (4)
#define IWL_MAX_GLOBAL_KEYS (4)
#define STA_KEY_LEN_WEP40 (5)
#define STA_KEY_LEN_WEP104 (13)

/* 
                                           
                                     
                                                       
                                                  
                                                
                                       
                                                              
                                                             
                                                             
 */
struct iwl_mvm_keyinfo {
	__le16 key_flags;
	u8 tkip_rx_tsc_byte2;
	u8 reserved1;
	__le16 tkip_rx_ttak[5];
	u8 key_offset;
	u8 reserved2;
	u8 key[16];
	__le64 tx_secur_seq_cnt;
	__le64 hw_tkip_mic_rx_key;
	__le64 hw_tkip_mic_tx_key;
} __packed;

/* 
                                                                                
                           
                                                      
                                                                             
                                                                              
       
                                                           
                                         
                                                     
                                                                           
                                       
                                 
                                         
                                                          
                                                                          
                                                                        
                                                                     
                                                             
                        
                                                                           
                                                  
                                                                              
                        
                                                                              
                                                                       
                                  
                                                   
                                                                            
            
                                         
                                                  
  
                                                                              
                                                                        
                                                              
  
                                                                         
                                          
 */
struct iwl_mvm_add_sta_cmd {
	u8 add_modify;
	u8 unicast_tx_key_id;
	u8 multicast_tx_key_id;
	u8 reserved1;
	__le32 mac_id_n_color;
	u8 addr[ETH_ALEN];
	__le16 reserved2;
	u8 sta_id;
	u8 modify_mask;
	__le16 reserved3;
	struct iwl_mvm_keyinfo key;
	__le32 station_flags;
	__le32 station_flags_msk;
	__le16 tid_disable_tx;
	__le16 reserved4;
	u8 add_immediate_ba_tid;
	u8 remove_immediate_ba_tid;
	__le16 add_immediate_ba_ssn;
	__le16 sleep_tx_count;
	__le16 sleep_state_flags;
	__le16 assoc_id;
	__le16 beamform_flags;
	__le32 tfd_queue_msk;
} __packed; /*                         */

/* 
                                                                              
                                                        
                                                                     
                                                                
                                                                              
                 
 */
enum iwl_mvm_add_sta_rsp_status {
	ADD_STA_SUCCESS			= 0x1,
	ADD_STA_STATIONS_OVERLOAD	= 0x2,
	ADD_STA_IMMEDIATE_BA_FAILURE	= 0x4,
	ADD_STA_MODIFY_NON_EXISTING_STA	= 0x8,
};

/* 
                                                                               
                        
                                                       
 */
struct iwl_mvm_rm_sta_cmd {
	u8 sta_id;
	u8 reserved[3];
} __packed; /*                            */

/* 
                                    
                            
                                 
         
                       
                    
                                        
           
                                                           
 */
struct iwl_mvm_mgmt_mcast_key_cmd {
	__le32 ctrl_flags;
	u8 IGTK[16];
	u8 K1[16];
	u8 K2[16];
	__le32 key_id;
	__le32 sta_id;
	__le64 receive_seq_cnt;
} __packed; /*                                        */

struct iwl_mvm_wep_key {
	u8 key_index;
	u8 key_offset;
	__le16 reserved1;
	u8 key_size;
	u8 reserved2[3];
	u8 key[16];
} __packed;

struct iwl_mvm_wep_key_cmd {
	__le32 mac_id_n_color;
	u8 num_keys;
	u8 decryption_type;
	u8 flags;
	u8 reserved;
	struct iwl_mvm_wep_key wep_key[0];
} __packed; /*                                  */


#endif /*                  */
