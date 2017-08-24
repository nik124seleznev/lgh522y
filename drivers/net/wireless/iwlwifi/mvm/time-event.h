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

#ifndef __time_event_h__
#define __time_event_h__

#include "fw-api.h"

#include "mvm.h"

/* 
                                 
  
                                                                              
                                                                        
                                                                          
                                                                               
                                                                      
  
                                                                               
                                                                            
                                                                        
                                                                             
 */

 /* 
                               
  
                                                                               
                                                                               
                     
                                             
                                                                           
                                   
                                                      
  
                                                                            
               
                                     
                                       
                                      
                                     
                          
                                               
         
 */

/* 
                                 
  
                                                                         
                                                                            
                        
 */

#define IWL_MVM_TE_SESSION_PROTECTION_MAX_TIME_MS 500
#define IWL_MVM_TE_SESSION_PROTECTION_MIN_TIME_MS 400

/* 
                                                                   
                          
                                                              
                                                
                                                                          
                             
  
                                                                               
                                                                           
                                                                              
                                         
                                                                              
                                                                             
 */
void iwl_mvm_protect_session(struct iwl_mvm *mvm,
			     struct ieee80211_vif *vif,
			     u32 duration, u32 min_duration);

/* 
                                                                   
                          
                                                              
  
                                                                        
                                                                           
                                                           
                               
 */
void iwl_mvm_stop_session_protection(struct iwl_mvm *mvm,
				      struct ieee80211_vif *vif);

/*
                                                                  
 */
int iwl_mvm_rx_time_event_notif(struct iwl_mvm *mvm,
				struct iwl_rx_cmd_buffer *rxb,
				struct iwl_device_cmd *cmd);

/* 
                                                                              
                          
                                                                            
                                                 
                                                                           
                                    
                                            
  
                                                                  
                                                                             
                                                                           
                                                                               
                                                                            
                                                                         
                                      
 */
int iwl_mvm_start_p2p_roc(struct iwl_mvm *mvm, struct ieee80211_vif *vif,
			  int duration, enum ieee80211_roc_type type);

/* 
                                                                            
                          
  
                                                              
                                                                         
                                                                     
 */
void iwl_mvm_stop_p2p_roc(struct iwl_mvm *mvm);

/* 
                                                                         
                          
                                                
                                                                    
  
                                                                        
                                                                      
             
 */
void iwl_mvm_remove_time_event(struct iwl_mvm *mvm,
			       struct iwl_mvm_vif *mvmvif,
			       struct iwl_mvm_time_event_data *te_data);

/* 
                                                      
                          
                                          
  
                                                                   
                                 
 */
void iwl_mvm_te_clear_data(struct iwl_mvm *mvm,
			   struct iwl_mvm_time_event_data *te_data);

void iwl_mvm_roc_done_wk(struct work_struct *wk);

#endif /*                  */
