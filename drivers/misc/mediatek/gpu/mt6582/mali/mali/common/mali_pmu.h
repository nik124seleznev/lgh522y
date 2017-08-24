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

#ifndef __MALI_PMU_H__
#define __MALI_PMU_H__

#include "mali_osk.h"

#define MALI_GP_DOMAIN_INDEX	0
#define MALI_PP0_DOMAIN_INDEX	1
#define MALI_PP1_DOMAIN_INDEX	2
#define MALI_PP2_DOMAIN_INDEX	3
#define MALI_PP3_DOMAIN_INDEX	4
#define MALI_PP4_DOMAIN_INDEX	5
#define MALI_PP5_DOMAIN_INDEX	6
#define MALI_PP6_DOMAIN_INDEX	7
#define MALI_PP7_DOMAIN_INDEX	8
#define MALI_L20_DOMAIN_INDEX	9
#define MALI_L21_DOMAIN_INDEX	10
#define MALI_L22_DOMAIN_INDEX	11

#define MALI_MAX_NUMBER_OF_DOMAINS	12

/*                                                              */
extern u16 mali_pmu_global_domain_config[];

static inline u16 mali_pmu_get_domain_mask(u32 index)
{
	MALI_DEBUG_ASSERT(MALI_MAX_NUMBER_OF_DOMAINS > index);

	return mali_pmu_global_domain_config[index];
}

static inline void mali_pmu_set_domain_mask(u32 index, u16 value)
{
	MALI_DEBUG_ASSERT(MALI_MAX_NUMBER_OF_DOMAINS > index);

	mali_pmu_global_domain_config[index] = value;
}

static inline void mali_pmu_copy_domain_mask(void *src, u32 len)
{
	_mali_osk_memcpy(mali_pmu_global_domain_config, src, len);
}

struct mali_pmu_core;

/*                                   
  
                                                                                                  
  
                                                         
                                                                          
                                                                                 
                                                              
 */
struct mali_pmu_core *mali_pmu_create(_mali_osk_resource_t *resource);

/*                                        
  
                                                                         
  
                                                  
 */
void mali_pmu_delete(struct mali_pmu_core *pmu);

/*                       
  
                                                 
                                                          
 */
_mali_osk_errcode_t mali_pmu_reset(struct mali_pmu_core *pmu);

/*                                                    
  
                                                                              
                                                                               
                                         
  
                                                      
                                                                
                                                                                       
 */
_mali_osk_errcode_t mali_pmu_power_down(struct mali_pmu_core *pmu, u32 mask);

/*                                                  
  
                                                                            
                                                                               
                                           
  
                                                    
                                                              
                                                                                       
 */
_mali_osk_errcode_t mali_pmu_power_up(struct mali_pmu_core *pmu, u32 mask);

/*                                                    
  
                                 
  
                                                      
                                                                                       
 */
_mali_osk_errcode_t mali_pmu_power_down_all(struct mali_pmu_core *pmu);

/*                                                  
  
                               
  
                                                    
                                                                                       
 */
_mali_osk_errcode_t mali_pmu_power_up_all(struct mali_pmu_core *pmu);

/*                                                    
  
                                                         
 */
struct mali_pmu_core *mali_pmu_get_global_pmu_core(void);

#endif /*                */
