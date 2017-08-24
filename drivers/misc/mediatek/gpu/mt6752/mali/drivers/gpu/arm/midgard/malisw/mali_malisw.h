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





#ifndef _MALISW_H_
#define _MALISW_H_

/* 
                      
                                                   
 */

/* 
                                                       
     
 */

#include <stddef.h>

#include "mali_stdtypes.h"

/*                                                                                  */
#define CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type,member)))

/*                                                                            */
#define NELEMS(s)       (sizeof(s)/sizeof((s)[0]))

/* 
                                   
                                             
                
 */
#define MIN(x,y) CSTD_MIN(x,y)

/* 
                                    
                                             
                
 */
#define MAX(x,y) CSTD_MAX(x,y)

/* 
                                                       
                                             
                  
 */
#define CLAMP( x, min, max ) CSTD_CLAMP( x, min, max )

/* 
                                                                       
                                                                            
                                                                           
                                                                     
 */
#define PTR_TO_U64( x ) CSTD_PTR_TO_U64( x )

/* 
                                        
                                                                                                     
     
 */
#define MALI_IMPORT CSTD_LINK_IMPORT
#define MALI_EXPORT CSTD_LINK_EXPORT
#define MALI_IMPL   CSTD_LINK_IMPL
#if defined(CONFIG_MALI_DEBUG) || !MALI_CUSTOMER_RELEASE
#define MALI_LOCAL  CSTD_LINK_EXPORT
#else
#define MALI_LOCAL  CSTD_LINK_LOCAL
#endif

/*                                               
  
                                                                                                               
                        
 */
#ifndef MALI_API
#define MALI_API MALI_IMPORT
#endif
/*     */

/*                                 
     
  
                                                                                                                        
                                                                     
  
                                                          
  
        
                               
  
                                  
                                         
  
                                           
    
                   
    
           
  
                                       
  
        
                          
                                  
  
                                           
  
         
           
  
                            
  
        
                                                 
  
                      
    
              
    
           
 */

/*                                                           
  
                                                                                                                    
                                                                        
 */
#if 1 == MALI_UNIT_TEST
#define MALI_TESTABLE_STATIC_IMPL MALI_IMPL
#else
#define MALI_TESTABLE_STATIC_IMPL static
#endif

/*                                                      
  
                                                                        
 */
#define MALI_TESTABLE_STATIC_API MALI_API
/*     */

/*                                
     
  
                                                                                                                        
                                                                     
  
                                                          
  
        
                               
  
                                  
                            
  
                                          
    
                   
    
           
  
                          
  
        
                                  
  
                                          
  
           
  
                            
  
        
                                    
  
                      
    
              
    
           
 */

/*                                                          
  
                                                                                                                   
            
 */
#ifdef CONFIG_MALI_DEBUG
#define MALI_TESTABLE_LOCAL_IMPL MALI_IMPL
#else
#define MALI_TESTABLE_LOCAL_IMPL MALI_LOCAL
#endif /*                   */

/*                                                     
  
                                                                                                                   
            
 */
#ifdef CONFIG_MALI_DEBUG
#define MALI_TESTABLE_LOCAL_API MALI_API
#else
#define MALI_TESTABLE_LOCAL_API MALI_LOCAL
#endif /*                   */
/*     */

/* 
                                                                
                             
 */
#define REINTERPRET_CAST(type) CSTD_REINTERPRET_CAST(type)

/* 
                                                                
                       
 */
#define CONST_CAST(type) (type) CSTD_CONST_CAST(type)

/* 
                                                                                        
                        
 */
#define STATIC_CAST(type) (type) CSTD_STATIC_CAST(type)


/*     */

#endif /*            */
