/*
 * Copyright (C) 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_PP_SCHEDULER_H__
#define __MALI_PP_SCHEDULER_H__

#include "mali_osk.h"
#include "mali_pp_job.h"
#include "mali_group.h"
#include "linux/mali/mali_utgard.h"

/*                                                         
 */
_mali_osk_errcode_t mali_pp_scheduler_initialize(void);
void mali_pp_scheduler_terminate(void);

/*                                        
 */
void mali_pp_scheduler_populate(void);
void mali_pp_scheduler_depopulate(void);

/* 
                                
  
                                                       
  
                                                                                                    
                                                                               
  
                                                                                            
  
                                                                                            
  
                                                 
                                   
                                 
                                                                             
                                                                               
 */
void mali_pp_scheduler_job_done(struct mali_group *group, struct mali_pp_job *job, u32 sub_job, mali_bool success, mali_bool in_upper_half);

void mali_pp_scheduler_suspend(void);
void mali_pp_scheduler_resume(void);

/* 
                                                            
  
                                                                                                 
                                                              
  
                                           
 */
void mali_pp_scheduler_abort_session(struct mali_session_data *session);

/* 
                          
  
                                                                           
                                                                         
  
                                                             
                                                                            
                                                                
 */
void mali_pp_scheduler_reset_all_groups(void);

/* 
                                                      
  
                                                            
 */
void mali_pp_scheduler_zap_all_active(struct mali_session_data *session);

/* 
                                 
  
                                                                               
                                                                          
         
  
                                                                     
 */
struct mali_pp_core *mali_pp_scheduler_get_virtual_pp(void);

u32 mali_pp_scheduler_dump_state(char *buf, u32 size);

void mali_pp_scheduler_enable_group(struct mali_group *group);
void mali_pp_scheduler_disable_group(struct mali_group *group);

/* 
                                                        
  
                                                                                                     
  
                                                 
  
                                                                                                
        
 */
mali_scheduler_mask mali_pp_scheduler_activate_job(struct mali_pp_job *job);

/* 
                                             
 */
void mali_pp_scheduler_schedule(void);

int mali_pp_scheduler_set_perf_level(u32 cores, mali_bool override);

void mali_pp_scheduler_core_scaling_enable(void);
void mali_pp_scheduler_core_scaling_disable(void);
mali_bool mali_pp_scheduler_core_scaling_is_enabled(void);

u32 mali_pp_scheduler_get_num_cores_total(void);
u32 mali_pp_scheduler_get_num_cores_enabled(void);

/* 
                                                                                          
  
                                                                 
 */
u32 mali_pp_scheduler_get_num_cores_total(void);

#endif /*                         */
