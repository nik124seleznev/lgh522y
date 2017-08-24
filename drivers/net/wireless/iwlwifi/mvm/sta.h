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

#ifndef __sta_h__
#define __sta_h__

#include <linux/spinlock.h>
#include <net/mac80211.h>
#include <linux/wait.h>

#include "iwl-trans.h" /*                       */
#include "fw-api.h" /*                       */
#include "rs.h"

struct iwl_mvm;

/* 
                                    
  
                                                                              
                                                                           
                                                                              
                                                                         
                                                                             
                                            
  
                                                                          
                                                                          
                                                                             
                                                                        
                                                                      
  
                                                                  
                                                                       
                                                                             
                                                                           
                                                                              
                                                                    
 */

/* 
                               
  
                                                                              
                                                                            
                                                                         
         
  
                                                                          
                                                                          
                                                                           
                                                                              
                                                                             
                                                         
  
                                                                            
                                                                
                                                                         
                                                                    
  
                                                                           
                                                                            
                                         
 */

/* 
                                         
  
                                                                 
                                                                      
                                                       
                                                                             
                                                                    
                                                                            
                                              
                                                                          
                                                                       
                                                                         
                                                                       
           
  
                                                                          
           
 */

/* 
                                              
  
                                                                           
                                                                    
                                                                                
                                                                             
                                                                            
                                                                            
                                                                    
 */

/* 
                                       
  
                                                                             
                                                                              
                                                                               
      
                                                                               
                                                                             
                                                                                
                                                                              
                                                                              
                                                                             
                                                                               
                                                                            
                                                                               
                                                          
                           
 */

/* 
                                  
  
                                                                             
                                                                           
                                                                             
                                                        
                                                                            
                                                                          
                                                                            
                                                                               
                                                                            
                                                                             
                                                                       
               
 */

/* 
                    
  
                                                                    
                                                                   
                                          
                                                                            
                                                                           
                                  
                                                                          
                                                                        
                                                                               
                                                                               
                                                                               
                                                                   
                                      
                                                                   
                                                                           
                                               
  
                       
 */

/* 
                         
  
                                                                   
                                              
  
                                        
                                                                       
                                         
                                                                            
                                                      
                                                                            
                                                      
 */
enum iwl_mvm_agg_state {
	IWL_AGG_OFF = 0,
	IWL_AGG_STARTING,
	IWL_AGG_ON,
	IWL_EMPTYING_HW_QUEUE_ADDBA,
	IWL_EMPTYING_HW_QUEUE_DELBA,
};

/* 
                                                               
                                                    
                                                                            
                                           
                                                                            
                                                                        
                                                               
                                           
                                                                             
                                                                      
                                                                       
                                                       
                                                      
 */
struct iwl_mvm_tid_data {
	u16 seq_number;
	u16 next_reclaimed;
	/*                            */
	u32 rate_n_flags;
	enum iwl_mvm_agg_state state;
	u16 txq_id;
	u16 ssn;
	bool wait_for_ba;
};

/* 
                                                                 
                                                                               
                                                     
                                                             
                                                                             
       
                                                                        
                                                                    
                                                                             
                                                  
                                                      
  
                                                                             
                                                                       
         
  
 */
struct iwl_mvm_sta {
	u32 sta_id;
	u32 tfd_queue_msk;
	u32 mac_id_n_color;
	u16 tid_disable_agg;
	u8 max_agg_bufsize;
	bool bt_reduced_txpower;
	spinlock_t lock;
	struct iwl_mvm_tid_data tid_data[IWL_MAX_TID_COUNT];
	struct iwl_lq_sta lq_sta;
	struct ieee80211_vif *vif;

#ifdef CONFIG_PM_SLEEP
	u16 last_seq_ctl;
#endif
};

/* 
                                                                               
             
                                                                               
                                                     
 */
struct iwl_mvm_int_sta {
	u32 sta_id;
	u32 tfd_queue_msk;
};

int iwl_mvm_sta_send_to_fw(struct iwl_mvm *mvm, struct ieee80211_sta *sta,
			   bool update);
int iwl_mvm_add_sta(struct iwl_mvm *mvm,
		    struct ieee80211_vif *vif,
		    struct ieee80211_sta *sta);
int iwl_mvm_update_sta(struct iwl_mvm *mvm,
		       struct ieee80211_vif *vif,
		       struct ieee80211_sta *sta);
int iwl_mvm_rm_sta(struct iwl_mvm *mvm,
		   struct ieee80211_vif *vif,
		   struct ieee80211_sta *sta);
int iwl_mvm_rm_sta_id(struct iwl_mvm *mvm,
		      struct ieee80211_vif *vif,
		      u8 sta_id);
int iwl_mvm_set_sta_key(struct iwl_mvm *mvm,
			struct ieee80211_vif *vif,
			struct ieee80211_sta *sta,
			struct ieee80211_key_conf *key,
			bool have_key_offset);
int iwl_mvm_remove_sta_key(struct iwl_mvm *mvm,
			   struct ieee80211_vif *vif,
			   struct ieee80211_sta *sta,
			   struct ieee80211_key_conf *keyconf);

void iwl_mvm_update_tkip_key(struct iwl_mvm *mvm,
			     struct ieee80211_vif *vif,
			     struct ieee80211_key_conf *keyconf,
			     struct ieee80211_sta *sta, u32 iv32,
			     u16 *phase1key);

/*       */
int iwl_mvm_sta_rx_agg(struct iwl_mvm *mvm, struct ieee80211_sta *sta,
		       int tid, u16 ssn, bool start);
int iwl_mvm_sta_tx_agg_start(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			struct ieee80211_sta *sta, u16 tid, u16 *ssn);
int iwl_mvm_sta_tx_agg_oper(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			struct ieee80211_sta *sta, u16 tid, u8 buf_size);
int iwl_mvm_sta_tx_agg_stop(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			    struct ieee80211_sta *sta, u16 tid);
int iwl_mvm_sta_tx_agg_flush(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			    struct ieee80211_sta *sta, u16 tid);

int iwl_mvm_add_aux_sta(struct iwl_mvm *mvm);
int iwl_mvm_allocate_int_sta(struct iwl_mvm *mvm, struct iwl_mvm_int_sta *sta,
			     u32 qmask);
void iwl_mvm_dealloc_int_sta(struct iwl_mvm *mvm,
			     struct iwl_mvm_int_sta *sta);
int iwl_mvm_send_bcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			   struct iwl_mvm_int_sta *bsta);
int iwl_mvm_send_rm_bcast_sta(struct iwl_mvm *mvm,
			      struct iwl_mvm_int_sta *bsta);
int iwl_mvm_add_bcast_sta(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			  struct iwl_mvm_int_sta *bsta);
int iwl_mvm_rm_bcast_sta(struct iwl_mvm *mvm, struct iwl_mvm_int_sta *bsta);
void iwl_mvm_sta_drained_wk(struct work_struct *wk);
void iwl_mvm_sta_modify_ps_wake(struct iwl_mvm *mvm,
				struct ieee80211_sta *sta);
void iwl_mvm_sta_modify_sleep_tx_count(struct iwl_mvm *mvm,
				       struct ieee80211_sta *sta,
				       enum ieee80211_frame_release_type reason,
				       u16 cnt);
int iwl_mvm_drain_sta(struct iwl_mvm *mvm, struct iwl_mvm_sta *mvmsta,
		      bool drain);

#endif /*           */
