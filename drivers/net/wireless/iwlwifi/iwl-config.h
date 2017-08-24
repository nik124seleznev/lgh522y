/******************************************************************************
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2007 - 2013 Intel Corporation. All rights reserved.
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
 * Copyright(c) 2005 - 2013 Intel Corporation. All rights reserved.
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
#ifndef __IWL_CONFIG_H__
#define __IWL_CONFIG_H__

#include <linux/types.h>
#include <net/mac80211.h>


enum iwl_device_family {
	IWL_DEVICE_FAMILY_UNDEFINED,
	IWL_DEVICE_FAMILY_1000,
	IWL_DEVICE_FAMILY_100,
	IWL_DEVICE_FAMILY_2000,
	IWL_DEVICE_FAMILY_2030,
	IWL_DEVICE_FAMILY_105,
	IWL_DEVICE_FAMILY_135,
	IWL_DEVICE_FAMILY_5000,
	IWL_DEVICE_FAMILY_5150,
	IWL_DEVICE_FAMILY_6000,
	IWL_DEVICE_FAMILY_6000i,
	IWL_DEVICE_FAMILY_6005,
	IWL_DEVICE_FAMILY_6030,
	IWL_DEVICE_FAMILY_6050,
	IWL_DEVICE_FAMILY_6150,
	IWL_DEVICE_FAMILY_7000,
};

/*
           
                                          
                                                         
                    
                     
                                                                  
                                   
 */
enum iwl_led_mode {
	IWL_LED_DEFAULT,
	IWL_LED_RF_STATE,
	IWL_LED_BLINK,
	IWL_LED_DISABLE,
};

/*
                                                                      
                                                        
 */
#define IWL_MAX_PLCP_ERR_THRESHOLD_MIN		1
#define IWL_MAX_PLCP_ERR_THRESHOLD_DEF		50
#define IWL_MAX_PLCP_ERR_LONG_THRESHOLD_DEF	100
#define IWL_MAX_PLCP_ERR_EXT_LONG_THRESHOLD_DEF	200
#define IWL_MAX_PLCP_ERR_THRESHOLD_MAX		255
#define IWL_MAX_PLCP_ERR_THRESHOLD_DISABLE	0

/*                                     */
#define IWL_WATCHDOG_DISABLED	0
#define IWL_DEF_WD_TIMEOUT	2000
#define IWL_LONG_WD_TIMEOUT	10000
#define IWL_MAX_WD_TIMEOUT	120000

/*                              */
#define	ANT_NONE	0x0
#define	ANT_A		BIT(0)
#define	ANT_B		BIT(1)
#define ANT_C		BIT(2)
#define	ANT_AB		(ANT_A | ANT_B)
#define	ANT_AC		(ANT_A | ANT_C)
#define ANT_BC		(ANT_B | ANT_C)
#define ANT_ABC		(ANT_A | ANT_B | ANT_C)


/*
                                          
                                                     
                                                                        
                                                         
                                                 
                                                                          
                                                          
                                                        
                                                                   
                                                              
                                                                          
                                          
                                                                             
                                                 
                                                                        
                                             
 */
struct iwl_base_params {
	int eeprom_size;
	int num_of_queues;	/*                   */
	/*                         */
	u32 pll_cfg_val;

	const u16 max_ll_items;
	const bool shadow_ram_support;
	u16 led_compensation;
	bool adv_thermal_throttle;
	bool support_ct_kill_exit;
	u8 plcp_delta_threshold;
	s32 chain_noise_scale;
	unsigned int wd_timeout;
	u32 max_event_log_size;
	const bool shadow_reg_enable;
	const bool hd_v2;
	const bool no_idle_support;
};

/*
                                                    
                                                         
                                                  
                                                         
                                                                    
 */
struct iwl_bt_params {
	bool advanced_bt_coexist;
	u8 bt_init_traffic_load;
	u32 bt_prio_boost;
	u16 agg_time_limit;
	bool bt_sco_disable;
	bool bt_session_2;
};

/*
                                                                  
                                                                           
 */
struct iwl_ht_params {
	enum ieee80211_smps_mode smps_mode;
	const bool ht_greenfield_support; /*                     */
	bool use_rts_for_aggregation;
	u8 ht40_bands;
};

/*
                                         
 */
#define EEPROM_REG_BAND_1_CHANNELS		0x08
#define EEPROM_REG_BAND_2_CHANNELS		0x26
#define EEPROM_REG_BAND_3_CHANNELS		0x42
#define EEPROM_REG_BAND_4_CHANNELS		0x5C
#define EEPROM_REG_BAND_5_CHANNELS		0x74
#define EEPROM_REG_BAND_24_HT40_CHANNELS	0x82
#define EEPROM_REG_BAND_52_HT40_CHANNELS	0x92
#define EEPROM_6000_REG_BAND_24_HT40_CHANNELS	0x80
#define EEPROM_REGULATORY_BAND_NO_HT40		0

struct iwl_eeprom_params {
	const u8 regulatory_bands[7];
	bool enhanced_txpower;
};

/* 
                 
                                    
                                                                        
                                                                   
                                                     
                                                                    
                                                                    
                                            
                                                                   
                                                            
                                                            
                                        
                                       
                        
                                          
                               
                                            
                                     
                                       
                                                                          
                                                                     
                         
                                                 
                                    
                                                                
                                                         
                                                                
                                                                        
           
  
                                                                         
                                                                      
                                        
 */
struct iwl_cfg {
	/*                                                                */
	const char *name;
	const char *fw_name_pre;
	const unsigned int ucode_api_max;
	const unsigned int ucode_api_ok;
	const unsigned int ucode_api_min;
	const enum iwl_device_family device_family;
	const u32 max_data_size;
	const u32 max_inst_size;
	u8   valid_tx_ant;
	u8   valid_rx_ant;
	u16  nvm_ver;
	u16  nvm_calib_ver;
	/*                                                    */
	const struct iwl_base_params *base_params;
	/*                                                */
	const struct iwl_ht_params *ht_params;
	const struct iwl_bt_params *bt_params;
	const struct iwl_eeprom_params *eeprom_params;
	const bool need_temp_offset_calib; /*                     */
	const bool no_xtal_calib;
	enum iwl_led_mode led_mode;
	const bool adv_pm;
	const bool rx_with_siso_diversity;
	const bool internal_wimax_coex;
	const bool temp_offset_v2;
	const bool host_interrupt_operation_mode;
};

/*
                                                            
 */
extern const struct iwl_cfg iwl5300_agn_cfg;
extern const struct iwl_cfg iwl5100_agn_cfg;
extern const struct iwl_cfg iwl5350_agn_cfg;
extern const struct iwl_cfg iwl5100_bgn_cfg;
extern const struct iwl_cfg iwl5100_abg_cfg;
extern const struct iwl_cfg iwl5150_agn_cfg;
extern const struct iwl_cfg iwl5150_abg_cfg;
extern const struct iwl_cfg iwl6005_2agn_cfg;
extern const struct iwl_cfg iwl6005_2abg_cfg;
extern const struct iwl_cfg iwl6005_2bg_cfg;
extern const struct iwl_cfg iwl6005_2agn_sff_cfg;
extern const struct iwl_cfg iwl6005_2agn_d_cfg;
extern const struct iwl_cfg iwl6005_2agn_mow1_cfg;
extern const struct iwl_cfg iwl6005_2agn_mow2_cfg;
extern const struct iwl_cfg iwl1030_bgn_cfg;
extern const struct iwl_cfg iwl1030_bg_cfg;
extern const struct iwl_cfg iwl6030_2agn_cfg;
extern const struct iwl_cfg iwl6030_2abg_cfg;
extern const struct iwl_cfg iwl6030_2bgn_cfg;
extern const struct iwl_cfg iwl6030_2bg_cfg;
extern const struct iwl_cfg iwl6000i_2agn_cfg;
extern const struct iwl_cfg iwl6000i_2abg_cfg;
extern const struct iwl_cfg iwl6000i_2bg_cfg;
extern const struct iwl_cfg iwl6000_3agn_cfg;
extern const struct iwl_cfg iwl6050_2agn_cfg;
extern const struct iwl_cfg iwl6050_2abg_cfg;
extern const struct iwl_cfg iwl6150_bgn_cfg;
extern const struct iwl_cfg iwl6150_bg_cfg;
extern const struct iwl_cfg iwl1000_bgn_cfg;
extern const struct iwl_cfg iwl1000_bg_cfg;
extern const struct iwl_cfg iwl100_bgn_cfg;
extern const struct iwl_cfg iwl100_bg_cfg;
extern const struct iwl_cfg iwl130_bgn_cfg;
extern const struct iwl_cfg iwl130_bg_cfg;
extern const struct iwl_cfg iwl2000_2bgn_cfg;
extern const struct iwl_cfg iwl2000_2bgn_d_cfg;
extern const struct iwl_cfg iwl2030_2bgn_cfg;
extern const struct iwl_cfg iwl6035_2agn_cfg;
extern const struct iwl_cfg iwl6035_2agn_sff_cfg;
extern const struct iwl_cfg iwl105_bgn_cfg;
extern const struct iwl_cfg iwl105_bgn_d_cfg;
extern const struct iwl_cfg iwl135_bgn_cfg;
extern const struct iwl_cfg iwl7260_2ac_cfg;
extern const struct iwl_cfg iwl7260_2n_cfg;
extern const struct iwl_cfg iwl7260_n_cfg;
extern const struct iwl_cfg iwl3160_2ac_cfg;
extern const struct iwl_cfg iwl3160_2n_cfg;
extern const struct iwl_cfg iwl3160_n_cfg;

#endif /*                  */
