/*
 * Copyright (C) 2010-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                   
                                                                 
 */

#ifndef __MALI_UKK_H__
#define __MALI_UKK_H__

#include "mali_osk.h"
#include "mali_uk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
                                                      
  
     
 */

/* 
                                                           
  
                                                                             
                                                                             
                                                                          
                                             
                                  
                                                                        
                                                                           
                                                                            
                                                 
                                                                     
                                                                           
                                                                       
                                                                               
                                                                         
                                                                         
                                                                           
                                                                           
                                                                            
                                                                           
          
                                                                       
                           
                                                                            
                                                
                                                                                                       
    
                                               
    
           
                                                                       
                                                                         
                  
  
                                                                             
                                                                           
                                                                      
                                                                                
                                                                     
                            
  
                                            
                                                                            
                                                                             
                                                                               
                                         
                 
    
                 
                           
                                       
           
  
                                                                          
                                                             
                                                                      
              
                                                                       
                                                                            
                               
                                                                      
                                                   
                                                                    
                                                                 
  
  
                               
  
                                                                       
                                                                              
                                             
                                                                           
                                            
                                                                              
                                                                              
                                                          
                                                                               
                                                                    
                                                                            
                                                                      
                                                                        
                                                                               
                                                                             
                                                                          
                                                                              
                                                                               
                                                                         
                                                                     
                                                                             
                                                                       
                  
  
                                                                            
                                                                         
                                                                            
                                                                             
                                                                     
                                                 
                                                                 
                        
                                                                             
                                                                              
                                                                            
                                                                        
                                                                     
                                                                         
                     
                                                                            
                                                                             
                   
                                                                             
                                                                        
                                                                      
  
                                                                 
                                                                               
                                                                              
                                                                  
                                                                        
                              
  
     
 */


/*                                                   
  
                                                                                          
  
                                                                                                                   
                                                                                                                               
  
                                                                                                            
  
                                                                     
                                                                          
                                                                              
                                           
  
                                                                                                            
                                                                                                      
                                                                                                                        
                                                                                                                     
                                                                      
  
                                                                           
                                                                               
                                                                       
                                                                    
                                                                       
                                              
  
      */

/*                                               
  
                                                                                
  
                                                                       
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_open( void **context );

/*                                         
  
                                                                                           
  
                                                                
  
                                                            
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_close( void **context );

/*     */ /*                            */


/*                                    
  
                                                          
                                                       
                                                                        
                                                  
  
      */

/*                                      
  
                                                                                         
  
                                                                               
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_wait_for_notification( _mali_uk_wait_for_notification_s *args );

/*                                                                           
  
                                                                           
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_post_notification( _mali_uk_post_notification_s *args );

/*                                                                         
  
                                                                         
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_get_api_version( _mali_uk_get_api_version_s *args );

/*                                                                    
  
                                                                           
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_get_user_settings(_mali_uk_get_user_settings_s *args);

/*                                                                 
  
                                                                          
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_get_user_setting(_mali_uk_get_user_setting_s *args);

/*                                                                
  
                                                                               
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_request_high_priority(_mali_uk_request_high_priority_s *args);

/*     */ /*                         */


/*                                        
  
                                                                                  
  
                                                                       
                                                        
                                                                                                    
                                
                                                                           
  
                                 
                                    
  
      */

/*                                                      
  
                                                                   
  
                                                                                                                   
                                                                  
  
                                                                                                                    
                  
                                                                                                                     
                                                                                                             
                                                                                                                  
                                                                             
                                                                               
                                                                            
                                                                             
                                                                              
                                                                            
                                                                             
                               
  
                                                                       
                                                                          
                                                     
  
                                                                             
                                                                            
                                  
  
                                                                  
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_mem_mmap( _mali_uk_mem_mmap_s *args );

/*                                                        
  
                                                                                                                
                             
  
                                                                    
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_mem_munmap( _mali_uk_mem_munmap_s *args );

/*                                                                        
                                                                                      
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_query_mmu_page_table_dump_size( _mali_uk_query_mmu_page_table_dump_size_s *args );
/*                              
                                                                           
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_dump_mmu_page_table( _mali_uk_dump_mmu_page_table_s * args );

/*                                                                            
                                                                      
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_mem_write_safe( _mali_uk_mem_write_safe_s *args );

/*                                                              
                                                                        
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_map_external_mem( _mali_uk_map_external_mem_s *args );

/*                                                                
                                                                          
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_unmap_external_mem( _mali_uk_unmap_external_mem_s *args );

#if defined(CONFIG_MALI400_UMP)
/*                                 
                                                                      
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_attach_ump_mem( _mali_uk_attach_ump_mem_s *args );
/*                                   
                                                                       
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_release_ump_mem( _mali_uk_release_ump_mem_s *args );
#endif /*                    */

/*                                                                           
             
  
                                                                             
                                                                         
                    
  
                                                                             
                                           
  
                                                                      
                                                                           
                                                                           
                                                        
  
                                                                             
                                                                        
                                                                           
           
  
                                                                        
                                                                            
                                                                      
                                                                       
                                                                             
                                                                            
        
  
                                                                               
                                                                  
                                                                           
                                                                               
                                                                              
               
  
                                                                               
                            
  
                                                                       
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_va_to_mali_pa( _mali_uk_va_to_mali_pa_s * args );

/*     */ /*                           */


/*                                                
  
                                                                                                   
                                                  
                                            
                                           
  
      */

/*                                                                    
  
                                                                                                    
                              
  
                                                                                                                      
                                                                                                                      
                                                                                                      
                              
  
                                                                        
  
                                                
                                                                                                                       
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_pp_start_job( void *ctx, _mali_uk_pp_start_job_s *uargs );

/* 
                                                                                            
  
                                                                                    
  
                                                
                                                                                                                              
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_pp_and_gp_start_job( void *ctx, _mali_uk_pp_and_gp_start_job_s *uargs );

/*                                                                
  
                                                                                
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_get_pp_number_of_cores( _mali_uk_get_pp_number_of_cores_s *args );

/*                                                                                   
  
                                                                                                           
                               
  
                                                                             
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_get_pp_core_version( _mali_uk_get_pp_core_version_s *args );

/*                                                    
  
                                                                             
 */
void _mali_ukk_pp_job_disable_wb(_mali_uk_pp_disable_wb_s *args);


/*     */ /*                       */


/*                                              
  
                                                                                                    
                                                
                                                    
                                         
  
      */

/*                                                                  
  
                                                                                                    
                              
  
                                                                                                                      
                                                                                                              
                                                                                                      
                              
  
                                                                        
  
                                                
                                                                                                                       
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_gp_start_job( void *ctx, _mali_uk_gp_start_job_s *uargs );

/*                                                               
  
                                                                                
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_get_gp_number_of_cores( _mali_uk_get_gp_number_of_cores_s *args );

/*                                                                                 
  
                                                                                                        
                               
  
                                                                             
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_get_gp_core_version( _mali_uk_get_gp_core_version_s *args );

/*                                                         
  
                                                                              
                                                                                          
  
                                                                             
                                                                                            
 */
_mali_osk_errcode_t _mali_ukk_gp_suspend_response( _mali_uk_gp_suspend_response_s *args );

/*     */ /*                       */

#if defined(CONFIG_MALI400_PROFILING)
/*                                                              
      */

/*                                          
  
                                                                         
 */
_mali_osk_errcode_t _mali_ukk_profiling_start(_mali_uk_profiling_start_s *args);

/*                                       
  
                                                                             
 */
_mali_osk_errcode_t _mali_ukk_profiling_add_event(_mali_uk_profiling_add_event_s *args);

/*                                         
  
                                                                        
 */
_mali_osk_errcode_t _mali_ukk_profiling_stop(_mali_uk_profiling_stop_s *args);

/*                                             
  
                                                                             
 */
_mali_osk_errcode_t _mali_ukk_profiling_get_event(_mali_uk_profiling_get_event_s *args);

/*                                         
  
                                                                         
 */
_mali_osk_errcode_t _mali_ukk_profiling_clear(_mali_uk_profiling_clear_s *args);

/*     */ /*                              */
#endif

/*                                                       
      */

/*                                        
  
                                                                               
                                                                            
                                         
  
                                                                            
 */
_mali_osk_errcode_t _mali_ukk_vsync_event_report(_mali_uk_vsync_event_report_s *args);

/*     */ /*                          */

/*                                                                     
      */

/*                                  
  
                                                                     
 */
_mali_osk_errcode_t _mali_ukk_sw_counters_report(_mali_uk_sw_counters_report_s *args);

/*     */ /*                                    */

/*     */ /*                   */

/*     */ /*                  */

u32 _mali_ukk_report_memory_usage(void);

u32 _mali_ukk_utilization_gp_pp(void);

u32 _mali_ukk_utilization_gp(void);

u32 _mali_ukk_utilization_pp(void);

#ifdef __cplusplus
}
#endif

#endif /*                */
