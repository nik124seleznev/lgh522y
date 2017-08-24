/*
 * Copyright (C) 2010-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_OSK_PROFILING_H__
#define __MALI_OSK_PROFILING_H__

#if defined(CONFIG_MALI400_PROFILING) && defined (CONFIG_TRACEPOINTS)

#include "mali_linux_trace.h"
#include "mali_profiling_events.h"
#include "mali_profiling_gator_api.h"

#define MALI_PROFILING_MAX_BUFFER_ENTRIES 1048576

#define MALI_PROFILING_NO_HW_COUNTER = ((u32)-1)

/*                                                               
      */

/* 
                                   
                                                          
 */
_mali_osk_errcode_t _mali_osk_profiling_init(mali_bool auto_start);

/*
                                  
 */
void _mali_osk_profiling_term(void);

/* 
                                 
  
                                                                      
                                                                                              
  
                                                                                                      
                                                          
 */
_mali_osk_errcode_t _mali_osk_profiling_start(u32 * limit);

/* 
                         
  
                                           
                                                                      
                                                                       
                                                                      
                                                                       
                                                                      
                                                          
 */
/*                                */
#define _mali_osk_profiling_add_event(event_id, data0, data1, data2, data3, data4) trace_mali_timeline_event((event_id), (data0), (data1), (data2), (data3), (data4))

/* 
                                   
  
                                           
                                         
 */

/*                                */
#define _mali_osk_profiling_report_hw_counter(counter_id, value) trace_mali_hw_counter(counter_id, value)

/* 
                     
  
                                          
 */
void _mali_osk_profiling_report_sw_counters(u32 *counters);

/* 
                                
  
                                                      
                                                          
 */
_mali_osk_errcode_t _mali_osk_profiling_stop(u32 * count);

/* 
                                                       
  
                                                               
 */
u32 _mali_osk_profiling_get_count(void);

/* 
                    
  
                                                                                                        
                                                                              
                                                                            
                                                                             
                                                          
 */
_mali_osk_errcode_t _mali_osk_profiling_get_event(u32 index, u64* timestamp, u32* event_id, u32 data[5]);

/* 
                             
  
                                                      
  
                                                          
 */
_mali_osk_errcode_t _mali_osk_profiling_clear(void);

/* 
                                                         
  
                                                                                     
 */
mali_bool _mali_osk_profiling_is_recording(void);

/* 
                                                     
  
                                                                     
 */
mali_bool _mali_osk_profiling_have_recording(void);

/*     */ /*                               */

#else /*                                                                   */

/*                                                  */

#define _mali_osk_profiling_add_event(event_id, data0, data1, data2, data3, data4)

#endif /*                                                                   */

#endif /*                          */


