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

#ifndef __MALI_OSK_MALI_H__
#define __MALI_OSK_MALI_H__

#include <linux/mali/mali_utgard.h>
#include <mali_osk.h>

#ifdef __cplusplus
extern "C" {
#endif

/*                                     
      */

/*                                                       
 */
struct _mali_osk_device_data {
	/*                                        */
	u32 dedicated_mem_start;
	u32 dedicated_mem_size;

	/*                   */
	u32 shared_mem_size;

	/*                                                             */
	u32 fb_start;
	u32 fb_size;

	/*                           */
	int max_job_runtime;

	/*                                                           */
	u32 utilization_interval;

	/*                                                             */
	void (*utilization_callback)(struct mali_gpu_utilization_data *data);

	/*
                          
                                                                            
                                                                      
                                                      
                                                                           
  */
	u32 pmu_switch_delay;

	/*                                                              
                                                              
  */
	u16 pmu_domain_config[12];

	/*                                                                                                       */
	int (*set_freq_callback)(unsigned int mhz);
};

/*                                  
  
                                                   
                                                                    
                                                                                             
 */
_mali_osk_errcode_t _mali_osk_resource_find(u32 addr, _mali_osk_resource_t *res);


/*                                      
  
                                                                                          
 */
u32 _mali_osk_resource_base_address(void);

/*                                            
  
                                                          
 */
_mali_osk_errcode_t _mali_osk_device_data_get(struct _mali_osk_device_data *data);

/*                                                                           
  
                                                             
 */
mali_bool _mali_osk_shared_interrupts(void);

/*     */ /*                                   */

/*                                        
      */

/*                                                                                                     
  
                                                                            
                           
                                      
                                          
 */
u32 _mali_osk_mem_write_safe(void *dest, const void *src, u32 size);

/*     */ /*                                      */


#ifdef __cplusplus
}
#endif

#endif /*                     */
