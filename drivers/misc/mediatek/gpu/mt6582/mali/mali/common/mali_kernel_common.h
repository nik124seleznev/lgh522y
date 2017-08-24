/*
 * Copyright (C) 2010, 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_KERNEL_COMMON_H__
#define __MALI_KERNEL_COMMON_H__

#include "mtk_mali_kernel.h" /*                                */
#include "mali_osk.h"

/*                                              */
#ifndef DEBUG
#if defined(_DEBUG)
#define DEBUG
#endif
#endif

/*                                                                                   
                                                                                             
                                                                                   
                                                                                                 
                                                                                     
                                                                                   
                                                                                           
  
                                                                                
  
                                                                         
  
                                                     
                                                                     
                                                                      
                                                                        
 */

/* 
                                                                                    
                                                                                    
                                         
 
                                                                 
 
                                        
*/
#define MALI_ERROR(error_code) return (error_code)

/* 
                                           
                                                                  
  
                 
 */
#define MALI_SUCCESS MALI_ERROR(_MALI_OSK_ERR_OK)

/* 
                                                                                         
                                                                                              
                      
  
                                                                                                 
                                                                                    
  
                                          
 */
#define MALI_CHECK(condition, error_code) do { if(!(condition)) MALI_ERROR(error_code); } while(0)

/* 
                                                                                              
                                                                                            
                                                                                     
                                                                              
                                                                                                 
                                                                     
 */
#define MALI_CHECK_NO_ERROR(expression) \
    do { _mali_osk_errcode_t _check_no_error_result=(expression); \
         if(_check_no_error_result != _MALI_OSK_ERR_OK) \
         MALI_ERROR(_check_no_error_result); \
    } while(0)

/* 
                                                 
 */
#define MALI_CHECK_NON_NULL(pointer, error_code) MALI_CHECK( ((pointer)!=NULL), (error_code) )

/* 
                                                                                           
                                                                                            
                                                                                           
                                
  
                                                                                           
                                                
 */
#define MALI_CHECK_GOTO(condition, label) do { if(!(condition)) goto label; } while(0)

/* 
                                                                                        
                                             
 */
#define MALI_IGNORE(x) x=x

#define MALI_PRINTF(args) _mali_osk_dbgmsg args;

#define MALI_PRINT_ERROR(args) do{ \
	MALI_PRINTF(("Mali: ERR: %s\n" ,__FILE__)); \
	MALI_PRINTF(("           %s()%4d\n           ", __FUNCTION__, __LINE__)) ; \
	MALI_PRINTF(args); \
	MALI_PRINTF(("\n"));   \
	MTKMALI_DumpRegister();\
	} while(0)

#define MALI_PRINT(args) do{ \
	MALI_PRINTF(("Mali: ")); \
	MALI_PRINTF(args); \
	} while (0)

#ifdef DEBUG
#ifndef mali_debug_level
extern int mali_debug_level;
#endif

#define MALI_DEBUG_CODE(code) code
#define MALI_DEBUG_PRINT(level, args)  do { \
	if((level) <=  mali_debug_level)\
        {MALI_PRINTF(("Mali<" #level ">: ")); MALI_PRINTF(args); } \
	} while (0)

#define MALI_DEBUG_PRINT_ERROR(args) MALI_PRINT_ERROR(args)

#define MALI_DEBUG_PRINT_IF(level,condition,args)  \
	if((condition)&&((level) <=  mali_debug_level))\
        {MALI_PRINTF(("Mali<" #level ">: ")); MALI_PRINTF(args); }

#define MALI_DEBUG_PRINT_ELSE(level, args)\
	else if((level) <=  mali_debug_level)\
    { MALI_PRINTF(("Mali<" #level ">: ")); MALI_PRINTF(args); }

/* 
                                                                         
                                                                          
                                             
 */
#define MALI_DEBUG_PRINT_ASSERT(condition, args) do  {if( !(condition)) { MALI_PRINT_ERROR(args); _mali_osk_break(); } } while(0)
#define MALI_DEBUG_ASSERT_POINTER(pointer) do  {if( (pointer)== NULL) {MALI_PRINT_ERROR(("NULL pointer " #pointer)); _mali_osk_break();} } while(0)
#define MALI_DEBUG_ASSERT(condition) do  {if( !(condition)) {MALI_PRINT_ERROR(("ASSERT failed: " #condition )); _mali_osk_break();} } while(0)

#else /*       */

#define MALI_DEBUG_CODE(code)
#define MALI_DEBUG_PRINT(string,args) do {} while(0)
#define MALI_DEBUG_PRINT_ERROR(args) do {} while(0)
#define MALI_DEBUG_PRINT_IF(level,condition,args) do {} while(0)
#define MALI_DEBUG_PRINT_ELSE(level,condition,args) do {} while(0)
#define MALI_DEBUG_PRINT_ASSERT(condition,args) do {} while(0)
#define MALI_DEBUG_ASSERT_POINTER(pointer) do {} while(0)
#define MALI_DEBUG_ASSERT(condition) do {} while(0)

#endif /*       */

/* 
                                                                                   
                                                                                 
                                                                                  
             
 */
#ifndef __user
#define __user
#endif

#endif /*                          */
