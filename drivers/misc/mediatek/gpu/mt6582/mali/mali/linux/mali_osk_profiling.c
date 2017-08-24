/*
 * Copyright (C) 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/module.h>

#include <mali_profiling_gator_api.h>
#include "mali_kernel_common.h"
#include "mali_osk.h"
#include "mali_ukk.h"
#include "mali_uk_types.h"
#include "mali_osk_profiling.h"
#include "mali_linux_trace.h"
#include "mali_gp.h"
#include "mali_pp.h"
#include "mali_pp_scheduler.h"
#include "mali_l2_cache.h"
#include "mali_user_settings_db.h"

_mali_osk_errcode_t _mali_osk_profiling_init(mali_bool auto_start)
{
	if (MALI_TRUE == auto_start) {
		mali_set_user_setting(_MALI_UK_USER_SETTING_SW_EVENTS_ENABLE, MALI_TRUE);
	}

	return _MALI_OSK_ERR_OK;
}

void _mali_osk_profiling_term(void)
{
	/*               */
}

_mali_osk_errcode_t _mali_osk_profiling_start(u32 * limit)
{
	/*               */
	return _MALI_OSK_ERR_OK;
}

_mali_osk_errcode_t _mali_osk_profiling_stop(u32 *count)
{
	/*               */
	return _MALI_OSK_ERR_OK;
}

u32 _mali_osk_profiling_get_count(void)
{
	return 0;
}

_mali_osk_errcode_t _mali_osk_profiling_get_event(u32 index, u64* timestamp, u32* event_id, u32 data[5])
{
	/*               */
	return _MALI_OSK_ERR_OK;
}

_mali_osk_errcode_t _mali_osk_profiling_clear(void)
{
	/*               */
	return _MALI_OSK_ERR_OK;
}

mali_bool _mali_osk_profiling_is_recording(void)
{
	return MALI_FALSE;
}

mali_bool _mali_osk_profiling_have_recording(void)
{
	return MALI_FALSE;
}

void _mali_osk_profiling_report_sw_counters(u32 *counters)
{
	trace_mali_sw_counters(_mali_osk_get_pid(), _mali_osk_get_tid(), NULL, counters);
}


_mali_osk_errcode_t _mali_ukk_profiling_start(_mali_uk_profiling_start_s *args)
{
	return _mali_osk_profiling_start(&args->limit);
}

_mali_osk_errcode_t _mali_ukk_profiling_add_event(_mali_uk_profiling_add_event_s *args)
{
	/*                                                                                                       */
	_mali_osk_profiling_add_event(args->event_id, _mali_osk_get_pid(), _mali_osk_get_tid(), args->data[2], args->data[3], args->data[4]);

	return _MALI_OSK_ERR_OK;
}

_mali_osk_errcode_t _mali_ukk_profiling_stop(_mali_uk_profiling_stop_s *args)
{
	return _mali_osk_profiling_stop(&args->count);
}

_mali_osk_errcode_t _mali_ukk_profiling_get_event(_mali_uk_profiling_get_event_s *args)
{
	return _mali_osk_profiling_get_event(args->index, &args->timestamp, &args->event_id, args->data);
}

_mali_osk_errcode_t _mali_ukk_profiling_clear(_mali_uk_profiling_clear_s *args)
{
	return _mali_osk_profiling_clear();
}

_mali_osk_errcode_t _mali_ukk_sw_counters_report(_mali_uk_sw_counters_report_s *args)
{
	_mali_osk_profiling_report_sw_counters(args->counters);
	return _MALI_OSK_ERR_OK;
}

/* 
                                        
  
                                    
                                                                                      
  
                                      
 */
int _mali_profiling_set_event(u32 counter_id, s32 event_id)
{
	if (COUNTER_VP_0_C0 == counter_id) {
		mali_gp_job_set_gp_counter_src0(event_id);
	} else if (COUNTER_VP_0_C1 == counter_id) {
		mali_gp_job_set_gp_counter_src1(event_id);
	} else if (COUNTER_FP_0_C0 <= counter_id && COUNTER_FP_7_C1 >= counter_id) {
		/*
                                               
    
                                                     
    
                                                                                    
                                                                                                   
                                    
    
                                                                                                       
                                                                                     
                                       
    
                                                                                                             
                                                                                                             
                                                                                                                   
    
                                                                                                      
    
                                                                                                    
                                                                                    
    
                                               
    
                                                            
                                                                                                
                                                                           
                                                                 
    
                                                                                        
    
                                                               
                                                                                               
    
                                                                                                     
                                    
                                                                                 
    
                                                                                                         
                                                                                   
                                                                                                     
    
                                                                                                          
                                    
    
                                                                                                       
                                                                                                                  
                                                                                                                 
                                                                                        
                                                                                                              
   */
		u32 sub_job = (counter_id - COUNTER_FP_0_C0) >> 1;
		u32 counter_src = (counter_id - COUNTER_FP_0_C0) & 1;
		if (0 == counter_src) {
			mali_pp_job_set_pp_counter_sub_job_src0(sub_job, event_id);
			if (0 == sub_job) {
				mali_pp_job_set_pp_counter_global_src0(event_id);
			}
		} else {
			mali_pp_job_set_pp_counter_sub_job_src1(sub_job, event_id);
			if (0 == sub_job) {
				mali_pp_job_set_pp_counter_global_src1(event_id);
			}
		}
	} else if (COUNTER_L2_0_C0 <= counter_id && COUNTER_L2_2_C1 >= counter_id) {
		u32 core_id = (counter_id - COUNTER_L2_0_C0) >> 1;
		struct mali_l2_cache_core* l2_cache_core = mali_l2_cache_core_get_glob_l2_core(core_id);

		if (NULL != l2_cache_core) {
			u32 counter_src = (counter_id - COUNTER_L2_0_C0) & 1;
			if (0 == counter_src) {
				mali_l2_cache_core_set_counter_src0(l2_cache_core, event_id);
			} else {
				mali_l2_cache_core_set_counter_src1(l2_cache_core, event_id);
			}
		}
	} else {
		return 0; /*                        */
	}

	return 1; /*         */
}

/* 
                                                                                     
                                                                                       
                                            
  
                                                                               
                                                              
                                                                                                     
 */
u32 _mali_profiling_get_l2_counters(_mali_profiling_l2_counter_values *values)
{
	struct mali_l2_cache_core *l2_cache;
	u32 l2_cores_num = mali_l2_cache_core_get_glob_num_l2_cores();
	u32 i;
	u32 ret = 0;

	MALI_DEBUG_ASSERT(l2_cores_num <= 3);

	for (i = 0; i < l2_cores_num; i++) {
		l2_cache = mali_l2_cache_core_get_glob_l2_core(i);

		if (NULL == l2_cache) {
			continue;
		}

		if (MALI_TRUE == mali_l2_cache_lock_power_state(l2_cache)) {
			/*                                                                              */
			mali_l2_cache_core_get_counter_values(l2_cache,
			                                      &values->cores[i].source0,
			                                      &values->cores[i].value0,
			                                      &values->cores[i].source1,
			                                      &values->cores[i].value1);
		} else {
			/*                                                            */
			ret |= (1 << i);
		}
		mali_l2_cache_unlock_power_state(l2_cache);
	}

	return ret;
}

/* 
                                                                                 
 */
void _mali_profiling_control(u32 action, u32 value)
{
	switch(action) {
	case FBDUMP_CONTROL_ENABLE:
		mali_set_user_setting(_MALI_UK_USER_SETTING_COLORBUFFER_CAPTURE_ENABLED, (value == 0 ? MALI_FALSE : MALI_TRUE));
		break;
	case FBDUMP_CONTROL_RATE:
		mali_set_user_setting(_MALI_UK_USER_SETTING_BUFFER_CAPTURE_N_FRAMES, value);
		break;
	case SW_COUNTER_ENABLE:
		mali_set_user_setting(_MALI_UK_USER_SETTING_SW_COUNTER_ENABLED, value);
		break;
	case FBDUMP_CONTROL_RESIZE_FACTOR:
		mali_set_user_setting(_MALI_UK_USER_SETTING_BUFFER_CAPTURE_RESIZE_FACTOR, value);
		break;
	default:
		break;	/*                              */
	}
}

/* 
                                           
 */
u32 _mali_profiling_get_api_version(void)
{
	return MALI_PROFILING_API_VERSION;
}

/* 
                                                             
                                      
*/
void _mali_profiling_get_mali_version(struct _mali_profiling_mali_version *values)
{
	values->mali_product_id = (u32)mali_kernel_core_get_product_id();
	values->mali_version_major = mali_kernel_core_get_gpu_major_version();
	values->mali_version_minor = mali_kernel_core_get_gpu_minor_version();
	values->num_of_l2_cores = mali_l2_cache_core_get_glob_num_l2_cores();
	values->num_of_fp_cores = mali_pp_scheduler_get_num_cores_total();
	values->num_of_vp_cores = 1;
}

EXPORT_SYMBOL(_mali_profiling_set_event);
EXPORT_SYMBOL(_mali_profiling_get_l2_counters);
EXPORT_SYMBOL(_mali_profiling_control);
EXPORT_SYMBOL(_mali_profiling_get_api_version);
EXPORT_SYMBOL(_mali_profiling_get_mali_version);
