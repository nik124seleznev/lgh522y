/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





#if !defined(_TRACE_MALI_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_MALI_H

#include <linux/stringify.h>
#include <linux/tracepoint.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM mali
#define TRACE_SYSTEM_STRING __stringify(TRACE_SYSTEM)
#define TRACE_INCLUDE_FILE mali_linux_trace

#define MALI_JOB_SLOTS_EVENT_CHANGED

/* 
                                                             
                                                                                                           
 */
TRACE_EVENT(mali_job_slots_event, TP_PROTO(unsigned int event_id, unsigned int tgid, unsigned int pid, unsigned char job_id), TP_ARGS(event_id, tgid, pid, job_id), TP_STRUCT__entry(__field(unsigned int, event_id)
																		       __field(unsigned int, tgid)
																		       __field(unsigned int, pid)
																		       __field(unsigned char, job_id)
	    ), TP_fast_assign(__entry->event_id = event_id; __entry->tgid = tgid; __entry->pid = pid; __entry->job_id = job_id;), TP_printk("event=%u tgid=%u pid=%u job_id=%u", __entry->event_id, __entry->tgid, __entry->pid, __entry->job_id)
	);

/* 
                                                    
                                                           
                                                                                  
 */
TRACE_EVENT(mali_pm_status, TP_PROTO(unsigned int event_id, unsigned long long value), TP_ARGS(event_id, value), TP_STRUCT__entry(__field(unsigned int, event_id)
																  __field(unsigned long long, value)
	    ), TP_fast_assign(__entry->event_id = event_id;), TP_printk("event %u = %llu", __entry->event_id, __entry->value)
	);

/* 
                                                      
                                                           
                                                         
 */
TRACE_EVENT(mali_pm_power_on, TP_PROTO(unsigned int event_id, unsigned long long value), TP_ARGS(event_id, value), TP_STRUCT__entry(__field(unsigned int, event_id)
																    __field(unsigned long long, value)
	    ), TP_fast_assign(__entry->event_id = event_id;), TP_printk("event %u = %llu", __entry->event_id, __entry->value)
	);

/* 
                                                       
                                                           
                                                           
 */
TRACE_EVENT(mali_pm_power_off, TP_PROTO(unsigned int event_id, unsigned long long value), TP_ARGS(event_id, value), TP_STRUCT__entry(__field(unsigned int, event_id)
																     __field(unsigned long long, value)
	    ), TP_fast_assign(__entry->event_id = event_id;), TP_printk("event %u = %llu", __entry->event_id, __entry->value)
	);

/* 
                                                               
                                                                    
                                       
                                          
 */
TRACE_EVENT(mali_page_fault_insert_pages, TP_PROTO(int event_id, unsigned long value), TP_ARGS(event_id, value), TP_STRUCT__entry(__field(int, event_id)
																  __field(unsigned long, value)
	    ), TP_fast_assign(__entry->event_id = event_id;), TP_printk("event %d = %lu", __entry->event_id, __entry->value)
	);

/* 
                                                                       
                                                             
                                       
 */
TRACE_EVENT(mali_mmu_as_in_use, TP_PROTO(int event_id), TP_ARGS(event_id), TP_STRUCT__entry(__field(int, event_id)
	    ), TP_fast_assign(__entry->event_id = event_id;), TP_printk("event=%d", __entry->event_id)
	);

/* 
                                                                            
                                                                     
                                       
 */
TRACE_EVENT(mali_mmu_as_released, TP_PROTO(int event_id), TP_ARGS(event_id), TP_STRUCT__entry(__field(int, event_id)
	    ), TP_fast_assign(__entry->event_id = event_id;), TP_printk("event=%d", __entry->event_id)
	);

/* 
                                                                     
                                                                  
                                                                  
                                                                                
 */
TRACE_EVENT(mali_total_alloc_pages_change, TP_PROTO(long long int event_id), TP_ARGS(event_id), TP_STRUCT__entry(__field(long long int, event_id)
	    ), TP_fast_assign(__entry->event_id = event_id;), TP_printk("event=%lld", __entry->event_id)
	);

/* 
                                       
                        
 */
TRACE_EVENT(mali_sw_counter, TP_PROTO(unsigned int event_id, signed long long value), TP_ARGS(event_id, value), TP_STRUCT__entry(__field(int, event_id)
																 __field(long long, value)
	    ), TP_fast_assign(__entry->event_id = event_id;), TP_printk("event %d = %lld", __entry->event_id, __entry->value)
	);

#endif				/*                */

#undef TRACE_INCLUDE_PATH
#undef linux
#define TRACE_INCLUDE_PATH .

/*                                      */
#include <trace/define_trace.h>
