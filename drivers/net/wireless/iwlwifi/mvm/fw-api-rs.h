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

#ifndef __fw_api_rs_h__
#define __fw_api_rs_h__

#include "fw-api-mac.h"

/*
                              
                                                            
 */
enum {
	IWL_RATE_1M_INDEX = 0,
	IWL_FIRST_CCK_RATE = IWL_RATE_1M_INDEX,
	IWL_RATE_2M_INDEX,
	IWL_RATE_5M_INDEX,
	IWL_RATE_11M_INDEX,
	IWL_LAST_CCK_RATE = IWL_RATE_11M_INDEX,
	IWL_RATE_6M_INDEX,
	IWL_FIRST_OFDM_RATE = IWL_RATE_6M_INDEX,
	IWL_RATE_9M_INDEX,
	IWL_RATE_12M_INDEX,
	IWL_RATE_18M_INDEX,
	IWL_RATE_24M_INDEX,
	IWL_RATE_36M_INDEX,
	IWL_RATE_48M_INDEX,
	IWL_RATE_54M_INDEX,
	IWL_LAST_NON_HT_RATE = IWL_RATE_54M_INDEX,
	IWL_RATE_60M_INDEX,
	IWL_LAST_OFDM_RATE = IWL_RATE_60M_INDEX,
	IWL_RATE_COUNT_LEGACY = IWL_LAST_NON_HT_RATE + 1,
	IWL_RATE_COUNT,
};

#define IWL_RATE_BIT_MSK(r) BIT(IWL_RATE_##r##M_INDEX)

/*                                                       */
enum {
	IWL_RATE_6M_PLCP  = 13,
	IWL_RATE_9M_PLCP  = 15,
	IWL_RATE_12M_PLCP = 5,
	IWL_RATE_18M_PLCP = 7,
	IWL_RATE_24M_PLCP = 9,
	IWL_RATE_36M_PLCP = 11,
	IWL_RATE_48M_PLCP = 1,
	IWL_RATE_54M_PLCP = 3,
	IWL_RATE_1M_PLCP  = 10,
	IWL_RATE_2M_PLCP  = 20,
	IWL_RATE_5M_PLCP  = 55,
	IWL_RATE_11M_PLCP = 110,
};

/*
                          
  
                                                                             
                                                                              
                     
  
                                   
                                             
                                         
                                             
                                       
                                             
                                       
                                            
 */

/*                                                */
#define RATE_MCS_HT_POS 8
#define RATE_MCS_HT_MSK (1 << RATE_MCS_HT_POS)

/*                                                                            */
#define RATE_MCS_CCK_POS 9
#define RATE_MCS_CCK_MSK (1 << RATE_MCS_CCK_POS)

/*                                                       */
#define RATE_MCS_VHT_POS 26
#define RATE_MCS_VHT_MSK (1 << RATE_MCS_VHT_POS)


/*
                                                
  
                       
                     
                     
                     
                     
                     
                     
                     
                     
                                  
                                
                                  
                                                                        
                       
  
                                                                     
                                                                    
                                                                     
                                                                  
 */
#define RATE_HT_MCS_RATE_CODE_MSK	0x7

/*                                      */
#define RATE_HT_MCS_GF_POS		10
#define RATE_HT_MCS_GF_MSK		(1 << RATE_HT_MCS_GF_POS)

#define RATE_HT_MCS_INDEX_MSK		0x3f

/*
                                                      
  
                       
                                
                                  
                                
                                  
 */

/*                                        */
#define RATE_VHT_MCS_RATE_CODE_MSK	0xf
#define RATE_VHT_MCS_NSS_POS		4
#define RATE_VHT_MCS_NSS_MSK		(3 << RATE_VHT_MCS_NSS_POS)

/*
                                       
  
                       
                       
                       
                       
                       
                       
                       
                       
                   
  
                                       
                                             
  
                      
                      
                        
                       
               
 */
#define RATE_LEGACY_RATE_MSK 0xff


/*
                                                         
                                                         
 */
#define RATE_MCS_CHAN_WIDTH_POS		11
#define RATE_MCS_CHAN_WIDTH_MSK		(3 << RATE_MCS_CHAN_WIDTH_POS)
#define RATE_MCS_CHAN_WIDTH_20		(0 << RATE_MCS_CHAN_WIDTH_POS)
#define RATE_MCS_CHAN_WIDTH_40		(1 << RATE_MCS_CHAN_WIDTH_POS)
#define RATE_MCS_CHAN_WIDTH_80		(2 << RATE_MCS_CHAN_WIDTH_POS)
#define RATE_MCS_CHAN_WIDTH_160		(3 << RATE_MCS_CHAN_WIDTH_POS)

/*                                                                       */
#define RATE_MCS_SGI_POS		13
#define RATE_MCS_SGI_MSK		(1 << RATE_MCS_SGI_POS)

/*                                                              */
#define RATE_MCS_ANT_POS		14
#define RATE_MCS_ANT_A_MSK		(1 << RATE_MCS_ANT_POS)
#define RATE_MCS_ANT_B_MSK		(2 << RATE_MCS_ANT_POS)
#define RATE_MCS_ANT_C_MSK		(4 << RATE_MCS_ANT_POS)
#define RATE_MCS_ANT_AB_MSK		(RATE_MCS_ANT_A_MSK | \
					 RATE_MCS_ANT_B_MSK)
#define RATE_MCS_ANT_ABC_MSK		(RATE_MCS_ANT_AB_MSK | \
					 RATE_MCS_ANT_C_MSK)
#define RATE_MCS_ANT_MSK		RATE_MCS_ANT_ABC_MSK
#define RATE_MCS_ANT_NUM 3

/*                             */
#define RATE_MCS_STBC_POS		17
#define RATE_MCS_STBC_MSK		(1 << RATE_MCS_STBC_POS)

/*                                                       */
#define RATE_MCS_BF_POS			19
#define RATE_MCS_BF_MSK			(1 << RATE_MCS_BF_POS)

/*                                       */
#define RATE_MCS_ZLF_POS		20
#define RATE_MCS_ZLF_MSK		(1 << RATE_MCS_ZLF_POS)

/*                                                                    */
#define RATE_MCS_DUP_POS		24
#define RATE_MCS_DUP_MSK		(3 << RATE_MCS_DUP_POS)

/*                                             */
#define RATE_MCS_LDPC_POS		27
#define RATE_MCS_LDPC_MSK		(1 << RATE_MCS_LDPC_POS)


/*                          */

/*                                                     */
#define  LQ_MAX_RETRY_NUM 16

/*                                                        */
#define  LQ_FLAG_SET_STA_TLC_RTS_MSK	BIT(0)

/* 
                                           
                             
                     
                                   
                                                                      
                 
                                                                      
                      
                                                                         
                                                                 
                                                                       
                                                 
                                                                       
                                                                 
                                    
                                                           
              
                                                
                                                      
                                                                   
                                                                
                                                      
 */
struct iwl_lq_cmd {
	u8 sta_id;
	u8 reserved1;
	u16 control;
	/*                                      */
	u8 flags;
	u8 mimo_delim;
	u8 single_stream_ant_msk;
	u8 dual_stream_ant_msk;
	u8 initial_rate_index[AC_NUM];
	/*                                  */
	__le16 agg_time_limit;
	u8 agg_disable_start_th;
	u8 agg_frame_cnt_limit;
	__le32 reserved2;
	__le32 rs_table[LQ_MAX_RETRY_NUM];
	__le32 bf_params;
}; /*                              */
#endif /*                 */
