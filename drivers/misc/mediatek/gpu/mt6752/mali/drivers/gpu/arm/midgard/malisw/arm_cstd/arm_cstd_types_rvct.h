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





#ifndef _ARM_CSTD_TYPES_RVCT_H_
#define _ARM_CSTD_TYPES_RVCT_H_

/*                                                                             
                 
                                                                             */
#include <stddef.h>
#include <limits.h>

#if 199901L <= __STDC_VERSION__
	#include <inttypes.h>
#else
	typedef unsigned char           uint8_t;
	typedef signed char             int8_t;
	typedef unsigned short          uint16_t;
	typedef signed short            int16_t;
	typedef unsigned int            uint32_t;
	typedef signed int              int32_t;
	typedef unsigned __int64        uint64_t;
	typedef signed __int64          int64_t;
	typedef ptrdiff_t               intptr_t;
	typedef size_t                  uintptr_t;
#endif

typedef uint32_t                    bool_t;

#if !defined(TRUE)
	#define TRUE                ((bool_t)1)
#endif

#if !defined(FALSE)
	#define FALSE               ((bool_t)0)
#endif

/*                                                                             
         
                                                                             */
/* 
                                       
     
 */

/* 
           
                   
                                                                             
                                                                    
                                              
 */
#define ASM                     __asm

/* 
              
                   
                                                                             
                                                                          
          
 */
#define INLINE                  __inline

/* 
                    
                   
                                                                               
                                                                               
                                                                           
                                          
 */
#define FORCE_INLINE            __forceinline

/* 
                    
                   
                                                                              
                                                                         
                                                                      
                                                                    
 */
#define NEVER_INLINE            __declspec(noinline)

/* 
            
                   
                                                                             
                                                                               
 */
#define PURE                    __pure

/* 
              
                   
                                                                              
                                                                              
  
        
                                       
  
                           
          
     
  
                                      
         
           
 */
#define PACKED                  __packed

/* 
                 
                   
                                                                          
                                                                         
                                                                             
              
  
                                                                              
                                               
 */
#define UNALIGNED               __packed

/* 
                
                   
                                                                             
                                                                             
            
  
                                                                             
                                                                         
                                                                     
                        
 */
#define RESTRICT                __restrict

/* 
                    
                   
                                                                            
                                                                        
                                                                             
  
                                                                            
                                                                           
                                                                      
            
  
        
                                    
           
  */
#define CHECK_RESULT

/* 
                 
                                                                              
                                                                          
                                                                          
                                   
  
                                                                           
                                                                         
                                                                        
                                                                         
                                                                           
                                       
 */
#define CSTD_FUNC            __FUNCTION__

/* 
     
 */

#endif /*                               */
