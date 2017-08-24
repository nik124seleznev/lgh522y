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

#ifndef __fw_api_tx_h__
#define __fw_api_tx_h__

/* 
                                                          
                                                                        
                                                     
                                                                             
                                                  
                                            
                                                                         
                                          
                                                                 
                                                                       
                                                                     
                                                                              
                                                         
                                                                       
                                                                         
                                                     
                                                                            
                                                                   
                                                
                                                       
                                                                              
                                                               
                                                                       
                                             
                                                                          
                                                                          
                                                                       
                                                          
                                                                         
                                                          
                                      
                                                                    
                                                    
 */
enum iwl_tx_flags {
	TX_CMD_FLG_PROT_REQUIRE		= BIT(0),
	TX_CMD_FLG_ACK			= BIT(3),
	TX_CMD_FLG_STA_RATE		= BIT(4),
	TX_CMD_FLG_BA			= BIT(5),
	TX_CMD_FLG_BAR			= BIT(6),
	TX_CMD_FLG_TXOP_PROT		= BIT(7),
	TX_CMD_FLG_VHT_NDPA		= BIT(8),
	TX_CMD_FLG_HT_NDPA		= BIT(9),
	TX_CMD_FLG_CSI_FDBK2HOST	= BIT(10),
	TX_CMD_FLG_BT_DIS		= BIT(12),
	TX_CMD_FLG_SEQ_CTL		= BIT(13),
	TX_CMD_FLG_MORE_FRAG		= BIT(14),
	TX_CMD_FLG_NEXT_FRAME		= BIT(15),
	TX_CMD_FLG_TSF			= BIT(16),
	TX_CMD_FLG_CALIB		= BIT(17),
	TX_CMD_FLG_KEEP_SEQ_CTL		= BIT(18),
	TX_CMD_FLG_AGG_START		= BIT(19),
	TX_CMD_FLG_MH_PAD		= BIT(20),
	TX_CMD_FLG_RESP_TO_DRV		= BIT(21),
	TX_CMD_FLG_CCMP_AGG		= BIT(22),
	TX_CMD_FLG_TKIP_MIC_DONE	= BIT(23),
	TX_CMD_FLG_CTS_ONLY		= BIT(24),
	TX_CMD_FLG_DUR			= BIT(25),
	TX_CMD_FLG_FW_DROP		= BIT(26),
	TX_CMD_FLG_EXEC_PAPD		= BIT(27),
	TX_CMD_FLG_PAPD_TYPE		= BIT(28),
	TX_CMD_FLG_HCCA_CHUNK		= BIT(31)
}; /*                           */

/*
                              
 */
#define TX_CMD_SEC_WEP			0x01
#define TX_CMD_SEC_CCM			0x02
#define TX_CMD_SEC_TKIP			0x03
#define TX_CMD_SEC_WEP_KEY_IDX_POS	6
#define TX_CMD_SEC_WEP_KEY_IDX_MSK	0xc0
#define TX_CMD_SEC_KEY128		0x08

/*                                                                 */
/*
                             
  
                                             
                                 
                                       
                                     
                                         
                 
                         
                    
 */
#define TX_CMD_NEXT_FRAME_ACK_MSK		(0x8)
#define TX_CMD_NEXT_FRAME_STA_RATE_MSK		(0x10)
#define TX_CMD_NEXT_FRAME_BA_MSK		(0x20)
#define TX_CMD_NEXT_FRAME_IMM_BA_RSP_MSK	(0x40)
#define TX_CMD_NEXT_FRAME_FLAGS_MSK		(0xf8)
#define TX_CMD_NEXT_FRAME_STA_ID_MSK		(0xff00)
#define TX_CMD_NEXT_FRAME_STA_ID_POS		(8)
#define TX_CMD_NEXT_FRAME_RATE_MSK		(0xffff0000)
#define TX_CMD_NEXT_FRAME_RATE_POS		(16)

/*
                                                                       
 */
#define TX_CMD_LIFE_TIME_INFINITE	0xFFFFFFFF
#define TX_CMD_LIFE_TIME_DEFAULT	2000000 /*        */
#define TX_CMD_LIFE_TIME_PROBE_RESP	40000 /*       */
#define TX_CMD_LIFE_TIME_EXPIRED_FRAME	0

/*
                                                      
 */
#define IWL_TID_NON_QOS	IWL_MAX_TID_COUNT

/*
                                                                          
 */
#define IWL_DEFAULT_TX_RETRY			15
#define IWL_MGMT_DFAULT_RETRY_LIMIT		3
#define IWL_RTS_DFAULT_RETRY_LIMIT		60
#define IWL_BAR_DFAULT_RETRY_LIMIT		60
#define IWL_LOW_RETRY_LIMIT			7

/*                                                         */
/* 
                                              
                    
                                                       
                                                                         
                                                                   
                                         
                                                                           
                                     
                                                            
                                           
                                                                             
                                                                         
                     
                                                          
                                        
                                                                            
                                                             
                                                            
                                         
                                                          
                       
                                         
                                                                         
                             
  
                                                                   
                      
                                     
                                                 
                 
                                      
                                              
                                                               
                               
  
                                                                      
                               
 */
struct iwl_tx_cmd {
	__le16 len;
	__le16 next_frame_len;
	__le32 tx_flags;
	/*                          */
	u8 try_cnt;
	u8 btkill_cnt;
	__le16 reserved;
	__le32 rate_n_flags;
	u8 sta_id;
	u8 sec_ctl;
	u8 initial_rate_index;
	u8 reserved2;
	u8 key[16];
	__le16 next_frame_flags;
	__le16 reserved3;
	__le32 life_time;
	__le32 dram_lsb_ptr;
	u8 dram_msb_ptr;
	u8 rts_retry_limit;
	u8 data_retry_limit;
	u8 tid_tspec;
	__le16 pm_frame_timeout;
	__le16 driver_txop;
	u8 payload[0];
	struct ieee80211_hdr hdr[0];
} __packed; /*                    */

/*
                           
 */

/*
                                                                              
                      
                          
                             
                                 
                               
                                    
                                 
                                          
                               
                              
                            
                              
                                
                               
                           
                                
                            
                               
                               
                               
                                
                                 
                           
                                                                           
            
                                   
                
                     
                         
                           
                     
  
                               
                               
 */
enum iwl_tx_status {
	TX_STATUS_MSK = 0x000000ff,
	TX_STATUS_SUCCESS = 0x01,
	TX_STATUS_DIRECT_DONE = 0x02,
	/*             */
	TX_STATUS_POSTPONE_DELAY = 0x40,
	TX_STATUS_POSTPONE_FEW_BYTES = 0x41,
	TX_STATUS_POSTPONE_BT_PRIO = 0x42,
	TX_STATUS_POSTPONE_QUIET_PERIOD = 0x43,
	TX_STATUS_POSTPONE_CALC_TTAK = 0x44,
	/*          */
	TX_STATUS_FAIL_INTERNAL_CROSSED_RETRY = 0x81,
	TX_STATUS_FAIL_SHORT_LIMIT = 0x82,
	TX_STATUS_FAIL_LONG_LIMIT = 0x83,
	TX_STATUS_FAIL_UNDERRUN = 0x84,
	TX_STATUS_FAIL_DRAIN_FLOW = 0x85,
	TX_STATUS_FAIL_RFKILL_FLUSH = 0x86,
	TX_STATUS_FAIL_LIFE_EXPIRE = 0x87,
	TX_STATUS_FAIL_DEST_PS = 0x88,
	TX_STATUS_FAIL_HOST_ABORTED = 0x89,
	TX_STATUS_FAIL_BT_RETRY = 0x8a,
	TX_STATUS_FAIL_STA_INVALID = 0x8b,
	TX_STATUS_FAIL_FRAG_DROPPED = 0x8c,
	TX_STATUS_FAIL_TID_DISABLE = 0x8d,
	TX_STATUS_FAIL_FIFO_FLUSHED = 0x8e,
	TX_STATUS_FAIL_SMALL_CF_POLL = 0x8f,
	TX_STATUS_FAIL_FW_DROP = 0x90,
	TX_STATUS_FAIL_STA_COLOR_MISMATCH = 0x91,
	TX_STATUS_INTERNAL_ABORT = 0x92,
	TX_MODE_MSK = 0x00000f00,
	TX_MODE_NO_BURST = 0x00000000,
	TX_MODE_IN_BURST_SEQ = 0x00000100,
	TX_MODE_FIRST_IN_BURST = 0x00000200,
	TX_QUEUE_NUM_MSK = 0x0001f000,
	TX_NARROW_BW_MSK = 0x00060000,
	TX_NARROW_BW_1DIV2 = 0x00020000,
	TX_NARROW_BW_1DIV4 = 0x00040000,
	TX_NARROW_BW_1DIV8 = 0x00060000,
};

/*
                                                 
                            
                             
                          
                         
                           
                       
                               
                                   
                                   
                           
                                
                          
                         
                          
                                                                               
                                                                       
                                                                         
                                                       
                                                                              
             
  
                               
 */
enum iwl_tx_agg_status {
	AGG_TX_STATE_STATUS_MSK = 0x00fff,
	AGG_TX_STATE_TRANSMITTED = 0x000,
	AGG_TX_STATE_UNDERRUN = 0x001,
	AGG_TX_STATE_BT_PRIO = 0x002,
	AGG_TX_STATE_FEW_BYTES = 0x004,
	AGG_TX_STATE_ABORT = 0x008,
	AGG_TX_STATE_LAST_SENT_TTL = 0x010,
	AGG_TX_STATE_LAST_SENT_TRY_CNT = 0x020,
	AGG_TX_STATE_LAST_SENT_BT_KILL = 0x040,
	AGG_TX_STATE_SCD_QUERY = 0x080,
	AGG_TX_STATE_TEST_BAD_CRC32 = 0x0100,
	AGG_TX_STATE_RESPONSE = 0x1ff,
	AGG_TX_STATE_DUMP_TX = 0x200,
	AGG_TX_STATE_DELAY_TX = 0x400,
	AGG_TX_STATE_TRY_CNT_POS = 12,
	AGG_TX_STATE_TRY_CNT_MSK = 0xf << AGG_TX_STATE_TRY_CNT_POS,
};

#define AGG_TX_STATE_LAST_SENT_MSK  (AGG_TX_STATE_LAST_SENT_TTL| \
				     AGG_TX_STATE_LAST_SENT_TRY_CNT| \
				     AGG_TX_STATE_LAST_SENT_BT_KILL)

/*
                                                                               
                                                                              
                                                                            
 */
#define AGG_TX_STAT_FRAME_NOT_SENT (AGG_TX_STATE_FEW_BYTES | \
				    AGG_TX_STATE_ABORT | \
				    AGG_TX_STATE_SCD_QUERY)

/*
                             
  
                                                                           
                            
  
                                                                              
                                                                         
              
  
                                                                             
                                                                      
                                                                        
                                             
  
                                                                      
                                                                       
               
                                                                           
                                                                         
                           
 */

/* 
                                                          
                                  
                                                           
 */
struct agg_tx_status {
	__le16 status;
	__le16 sequence;
} __packed;

/*
                                           
                                
                                                         
                                
 */
#define TX_RES_INIT_RATE_INDEX_MSK 0x0f
#define TX_RES_RATE_TABLE_COLOR_MSK 0x70
#define TX_RES_INV_RATE_INDEX_MSK 0x80

#define IWL_MVM_TX_RES_GET_TID(_ra_tid) ((_ra_tid) & 0x0f)
#define IWL_MVM_TX_RES_GET_RA(_ra_tid) ((_ra_tid) >> 4)

/* 
                                                              
                      
                                                 
                                                                     
                                                        
                                                              
                                                                              
                                  
                                                                               
                                                             
           
                            
                                 
                                 
                                 
                                     
                                           
                                             
                                        
                           
                                             
                             
                                                  
                                                                          
                                      
  
                                                                
                                         
 */
struct iwl_mvm_tx_resp {
	u8 frame_count;
	u8 bt_kill_count;
	u8 failure_rts;
	u8 failure_frame;
	__le32 initial_rate;
	__le16 wireless_media_time;

	u8 pa_status;
	u8 pa_integ_res_a[3];
	u8 pa_integ_res_b[3];
	u8 pa_integ_res_c[3];
	__le16 measurement_req_id;
	__le16 reserved;

	__le32 tfd_info;
	__le16 seq_ctl;
	__le16 byte_cnt;
	u8 tlc_info;
	u8 ra_tid;
	__le16 frame_ctrl;

	struct agg_tx_status status;
} __packed; /*                    */

/* 
                                                           
                      
                                                   
                                                   
                                                                         
                           
            
                                                                
                                             
                                                           
                                             
                                                     
 */
struct iwl_mvm_ba_notif {
	__le32 sta_addr_lo32;
	__le16 sta_addr_hi16;
	__le16 reserved;

	u8 sta_id;
	u8 tid;
	__le16 seq_ctl;
	__le64 bitmap;
	__le16 scd_flow;
	__le16 scd_ssn;
	u8 txed;
	u8 txed_2_done;
	__le16 reserved1;
} __packed;

/*
                                                      
                                                        
                                                                          
        
                                                   
                                      
                                           
 */
struct iwl_mac_beacon_cmd {
	struct iwl_tx_cmd tx;
	__le32 template_id;
	__le32 tim_idx;
	__le32 tim_size;
	struct ieee80211_hdr frame[0];
} __packed;

struct iwl_beacon_notif {
	struct iwl_mvm_tx_resp beacon_notify_hdr;
	__le64 tsf;
	__le32 ibss_mgr_status;
} __packed;

/* 
                                                     
                                                              
                                
 */
enum iwl_dump_control {
	DUMP_TX_FIFO_FLUSH	= BIT(1),
};

/* 
                                                           
                                         
                            
                      
 */
struct iwl_tx_path_flush_cmd {
	__le32 queues_ctl;
	__le16 flush_ctl;
	__le16 reserved;
} __packed; /*                               */

/* 
                                                   
                                                         
  
                                                                              
                                                                            
                                                                             
                                                                               
                                                                             
                                                                            
                                                                              
                                                  
 */
static inline u32 iwl_mvm_get_scd_ssn(struct iwl_mvm_tx_resp *tx_resp)
{
	return le32_to_cpup((__le32 *)&tx_resp->status +
			    tx_resp->frame_count) & 0xfff;
}

#endif /*                 */
