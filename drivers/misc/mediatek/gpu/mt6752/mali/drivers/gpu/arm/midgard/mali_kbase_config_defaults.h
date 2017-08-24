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



/* 
                                     
  
                                            
  
 */

#ifndef _KBASE_CONFIG_DEFAULTS_H_
#define _KBASE_CONFIG_DEFAULTS_H_

/*                                            */
#include <mali_kbase_config_platform.h>

/* 
                                                              
                                                               
                                                          
                                        
  
                                          
 */
#define DEFAULT_IRQ_THROTTLE_TIME_US 20

/*                               */

/* 
                                                     
 */
/*      */
#define DEFAULT_JS_SCHEDULING_TICK_NS 50000000u

/* 
                                                                           
  
                                                                             
             
 */
/*                                        */
#define DEFAULT_JS_SOFT_STOP_TICKS 2

/* 
                                                                              
 */
/*                                        */
#define DEFAULT_JS_SOFT_STOP_TICKS_CL 1

/* 
                                                                          
 */
/*                                                                     
                             
 */
#define DEFAULT_JS_HARD_STOP_TICKS_SS_HW_ISSUE_8408 24
/*                                        */
#define DEFAULT_JS_HARD_STOP_TICKS_SS 4

/* 
                                                                              
 */
/*                                        */
#define DEFAULT_JS_HARD_STOP_TICKS_CL 2

/* 
                                                                          
                 
 */
/*                 */
#define DEFAULT_JS_HARD_STOP_TICKS_NSS 1200

/* 
                                                                     
                         
 */
/*                                                                         
                             
 */
#define DEFAULT_JS_RESET_TICKS_SS_HW_ISSUE_8408 36
/*                               */
#define DEFAULT_JS_RESET_TICKS_SS 6

/* 
                                                                     
                             
 */
/*                               */
#define DEFAULT_JS_RESET_TICKS_CL 4

/* 
                                                                     
                                         
 */
/*                    */
#define DEFAULT_JS_RESET_TICKS_NSS 1202

/* 
                                                               
                                               
 */
#define DEFAULT_JS_RESET_TIMEOUT_MS 3000

/* 
                                                                        
  
                                                                        
                                    
  
                                                                           
                        
 */
/*                                                                         
                          
 */
#define DEFAULT_JS_CTX_TIMESLICE_NS 50000000

/* 
                                                                          
                   
  
                                                                       
                                                                           
                                                                     
                                                                           
                                         
  
                                                                      
                                                                   
                                          
 */
#define DEFAULT_JS_CFS_CTX_RUNTIME_INIT_SLICES 1

/* 
                                                                       
                                                         
  
                                                                      
                                      
  
                                                                         
                                                                        
                                                                            
                                                                  
              
  
                                                                          
                                                                           
                                                 
  
                                                                          
                                                                          
 */
#define DEFAULT_JS_CFS_CTX_RUNTIME_MIN_SLICES 2

/* 
                                                                     
                                  
 
                                                         
 
                                                                         
              
 
                                                                   
                                                                 
             
                                                                  
          
 
                                                                    
                                                              
            
*/
#define DEFAULT_SECURE_BUT_LOSS_OF_PERFORMANCE MALI_FALSE

enum {
	/* 
                                                     
  */
	KBASE_AID_32 = 0x0,

	/* 
                                                       
                                                                 
  */
	KBASE_AID_16 = 0x3,

	/* 
                                                          
                                                                 
  */
	KBASE_AID_8  = 0x2,

	/* 
                                                          
                                                                 
  */
	KBASE_AID_4  = 0x1
};

/* 
                                                           
  
                                     
                                                    
                                           
                                          
                                          
                                                                       
                              
 */
#define DEFAULT_ARID_LIMIT KBASE_AID_32

/* 
                                                        
  
                                     
                                                    
                                           
                                          
                                          
                                                                       
                              
 */
#define DEFAULT_AWID_LIMIT KBASE_AID_32

/* 
                                                           
 */
#define DEFAULT_ALTERNATIVE_HWC MALI_FALSE

/*                             */

/*                                */

/*              */
#define DEFAULT_PM_DVFS_FREQ 25

/* 
                                                   
 */
/*       */
#define DEFAULT_PM_GPU_POWEROFF_TICK_NS 400000

/* 
                                                                       
 */
/*           */
#define DEFAULT_PM_POWEROFF_TICK_SHADER 2

/* 
                                                             
 */
#define DEFAULT_PM_POWEROFF_TICK_GPU 2         /*           */

/*                                */


/* 
                                                                      
                                                         
 */
#define DEFAULT_UMP_GPU_DEVICE_SHIFT UMP_DEVICE_Z_SHIFT

/* 
                                                      
                                                         
 */
#define DEFAULT_CPU_SPEED_FUNC \
	((uintptr_t)kbase_cpuprops_get_default_clock_speed)

#endif /*                           */

