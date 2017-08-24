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





#ifndef _MALISW_STDTYPES_H_
#define _MALISW_STDTYPES_H_

/* 
                        
                                                                  
 */

/* 
                     
     
 */

/* 
                                                         
  
                           
 */

/* 
                              
     
 */

#include "arm_cstd/arm_cstd.h"

/* 
                      
                                                          
     
 */
/*                                                                  */
#if 0 == CSTD_OS_LINUX_KERNEL
	typedef uint64_t u64;
	typedef uint32_t u32;
	typedef uint16_t u16;
	typedef uint8_t  u8;
	
	typedef int64_t  s64;
	typedef int32_t  s32;
	typedef int16_t  s16;
	typedef int8_t   s8;
#endif

typedef double   f64;
typedef float    f32;
typedef u16      f16;

typedef u32      mali_fixed16_16;
/*    */

/* 
                       
                                                                          
                                                                
     
 */
typedef bool_t     mali_bool;
typedef u8         mali_bool8;

#define MALI_FALSE FALSE
#define MALI_TRUE  TRUE
/*    */

/* 
                                
                                               
     
 */
#ifndef U64_MAX
#define U64_MAX  UINT64_MAX
#endif

#ifndef U32_MAX
#define U32_MAX  UINT32_MAX
#endif

#ifndef U16_MAX
#define U16_MAX  UINT16_MAX
#endif

#ifndef U8_MAX
#define U8_MAX  UINT8_MAX
#endif

#ifndef S64_MAX
#define S64_MAX  INT64_MAX
#endif

#ifndef S64_MIN
#define S64_MIN  INT64_MIN
#endif

#ifndef S32_MAX
#define S32_MAX  INT32_MAX
#endif

#ifndef S32_MIN
#define S32_MIN  INT32_MIN
#endif

#ifndef S16_MAX
#define S16_MAX  INT16_MAX
#endif

#ifndef S16_MIN
#define S16_MIN  INT16_MIN
#endif

#ifndef S8_MAX
#define S8_MAX   INT8_MAX
#endif

#ifndef S8_MIN
#define S8_MIN   INT8_MIN
#endif

/*    */

/* 
                          
                                                                      
     
 */
typedef u64      mali_addr64;
typedef u32      mali_addr32;
typedef u64      mali_size64;
typedef s64      mali_offset64;
/*                                                                                                   */
/*    */

/* 
                         
                                                    
                                                                               
                                                
     
 */
typedef enum
{
	/* 
                                                   
                                          
      
  */
	MALI_ERROR_NONE = 0,
	MALI_ERROR_OUT_OF_GPU_MEMORY,
	MALI_ERROR_OUT_OF_MEMORY,
	MALI_ERROR_FUNCTION_FAILED,
	/*    */
	/* 
                                                                             
                                                                                        
                                     
      
  */
	MALI_ERROR_EGLP_BAD_ACCESS,
	MALI_ERROR_EGLP_BAD_PARAMETER,
	/*    */
	/* 
                                          
                                                                                                         
                                                                                                          
                                            
                                                                                                    
      
  */
	MALI_ERROR_MCLP_DEVICE_NOT_FOUND,
	MALI_ERROR_MCLP_DEVICE_NOT_AVAILABLE,
	MALI_ERROR_MCLP_COMPILER_NOT_AVAILABLE,
	MALI_ERROR_MCLP_MEM_OBJECT_ALLOCATION_FAILURE,
	MALI_ERROR_MCLP_PROFILING_INFO_NOT_AVAILABLE,
	MALI_ERROR_MCLP_MEM_COPY_OVERLAP,
	MALI_ERROR_MCLP_IMAGE_FORMAT_MISMATCH,
	MALI_ERROR_MCLP_IMAGE_FORMAT_NOT_SUPPORTED,
	MALI_ERROR_MCLP_BUILD_PROGRAM_FAILURE,
	MALI_ERROR_MCLP_MAP_FAILURE,
	MALI_ERROR_MCLP_MISALIGNED_SUB_BUFFER_OFFSET,
	MALI_ERROR_MCLP_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST,
	MALI_ERROR_MCLP_INVALID_VALUE,
	MALI_ERROR_MCLP_INVALID_DEVICE_TYPE,
	MALI_ERROR_MCLP_INVALID_PLATFORM,
	MALI_ERROR_MCLP_INVALID_DEVICE,
	MALI_ERROR_MCLP_INVALID_CONTEXT,
	MALI_ERROR_MCLP_INVALID_QUEUE_PROPERTIES,
	MALI_ERROR_MCLP_INVALID_COMMAND_QUEUE,
	MALI_ERROR_MCLP_INVALID_HOST_PTR,
	MALI_ERROR_MCLP_INVALID_MEM_OBJECT,
	MALI_ERROR_MCLP_INVALID_IMAGE_FORMAT_DESCRIPTOR,
	MALI_ERROR_MCLP_INVALID_IMAGE_SIZE,
	MALI_ERROR_MCLP_INVALID_SAMPLER,
	MALI_ERROR_MCLP_INVALID_BINARY,
	MALI_ERROR_MCLP_INVALID_BUILD_OPTIONS,
	MALI_ERROR_MCLP_INVALID_PROGRAM,
	MALI_ERROR_MCLP_INVALID_PROGRAM_EXECUTABLE,
	MALI_ERROR_MCLP_INVALID_KERNEL_NAME,
	MALI_ERROR_MCLP_INVALID_KERNEL_DEFINITION,
	MALI_ERROR_MCLP_INVALID_KERNEL,
	MALI_ERROR_MCLP_INVALID_ARG_INDEX,
	MALI_ERROR_MCLP_INVALID_ARG_VALUE,
	MALI_ERROR_MCLP_INVALID_ARG_SIZE,
	MALI_ERROR_MCLP_INVALID_KERNEL_ARGS,
	MALI_ERROR_MCLP_INVALID_WORK_DIMENSION,
	MALI_ERROR_MCLP_INVALID_WORK_GROUP_SIZE,
	MALI_ERROR_MCLP_INVALID_WORK_ITEM_SIZE,
	MALI_ERROR_MCLP_INVALID_GLOBAL_OFFSET,
	MALI_ERROR_MCLP_INVALID_EVENT_WAIT_LIST,
	MALI_ERROR_MCLP_INVALID_EVENT,
	MALI_ERROR_MCLP_INVALID_OPERATION,
	MALI_ERROR_MCLP_INVALID_GL_OBJECT,
	MALI_ERROR_MCLP_INVALID_BUFFER_SIZE,
	MALI_ERROR_MCLP_INVALID_MIP_LEVEL,
	MALI_ERROR_MCLP_INVALID_GLOBAL_WORK_SIZE,
	MALI_ERROR_MCLP_INVALID_GL_SHAREGROUP_REFERENCE_KHR,
	MALI_ERROR_MCLP_INVALID_EGL_OBJECT,
	/*    */
	/* 
                                          
                                                                                                          
                                               
                                                                                                      
      
  */
	MALI_ERROR_BASEP_INVALID_FUNCTION,
	/*    */
	/*                                                                                                            
                                                                                                 
  */
	MALI_ERROR_RESOURCE_IN_USE,

	/* 
                                      
   
                                                                                                   
                                                                        
  */
	MALI_ERROR_STRIDE_TOO_BIG

} mali_error;
/*    */

/*    */

/*    */

#endif /*                     */
