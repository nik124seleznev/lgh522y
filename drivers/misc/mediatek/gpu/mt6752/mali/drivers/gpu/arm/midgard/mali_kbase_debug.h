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





#ifndef _KBASE_DEBUG_H
#define _KBASE_DEBUG_H

#include <linux/bug.h>

/*                                                                                                                */
#define KBASE_DEBUG_SKIP_TRACE 0

/*                                                                             */
#define KBASE_DEBUG_SKIP_FUNCTION_NAME 0

/*                                                                                              */
#ifndef KBASE_DEBUG_DISABLE_ASSERTS
#ifdef CONFIG_MALI_DEBUG
#define KBASE_DEBUG_DISABLE_ASSERTS 0
#else
#define KBASE_DEBUG_DISABLE_ASSERTS 1
#endif
#endif				/*                             */

/*                                                                                      */
typedef void (kbase_debug_assert_hook) (void *);

typedef struct kbasep_debug_assert_cb {
	kbase_debug_assert_hook *func;
	void *param;
} kbasep_debug_assert_cb;

/* 
                                
                                                                                          
                                                             
 */
#if !KBASE_DEBUG_SKIP_TRACE
#define KBASEP_DEBUG_PRINT_TRACE \
		"In file: " __FILE__ " line: " CSTD_STR2(__LINE__)
#if !KBASE_DEBUG_SKIP_FUNCTION_NAME
#define KBASEP_DEBUG_PRINT_FUNCTION CSTD_FUNC
#else
#define KBASEP_DEBUG_PRINT_FUNCTION ""
#endif
#else
#define KBASEP_DEBUG_PRINT_TRACE ""
#endif

/* 
                                                     
                                                                                                 
                                                                      
                                                             
                                                         
                                                                     
 */
/*                                          */
#ifdef CONFIG_MALI_DEBUG
#define KBASEP_DEBUG_ASSERT_OUT(trace, function, ...)\
		do { \
			pr_err("Mali<ASSERT>: %s function:%s ", trace, function);\
			pr_err(__VA_ARGS__);\
			pr_err("\n");\
		} while (MALI_FALSE)
#else
#define KBASEP_DEBUG_ASSERT_OUT(trace, function, ...) CSTD_NOP()
#endif

#ifdef CONFIG_MALI_DEBUG
#define KBASE_CALL_ASSERT_HOOK() kbasep_debug_assert_call_hook();
#else
#define KBASE_CALL_ASSERT_HOOK() CSTD_NOP();
#endif

/* 
                                
                                                                                             
  
                                                                                         
  
                                 
 */
#define KBASE_DEBUG_ASSERT(expr) \
	KBASE_DEBUG_ASSERT_MSG(expr, #expr)

#if KBASE_DEBUG_DISABLE_ASSERTS
#define KBASE_DEBUG_ASSERT_MSG(expr, ...) CSTD_NOP()
#else
	/* 
                                          
                                                                                              
   
                                                                                          
   
                                  
                                                                                                          
  */
#define KBASE_DEBUG_ASSERT_MSG(expr, ...) \
		do { \
			if (!(expr)) { \
				KBASEP_DEBUG_ASSERT_OUT(KBASEP_DEBUG_PRINT_TRACE, KBASEP_DEBUG_PRINT_FUNCTION, __VA_ARGS__);\
				KBASE_CALL_ASSERT_HOOK();\
				BUG();\
			} \
		} while (MALI_FALSE)
#endif				/*                             */

/* 
                             
                                                               
  
                                               
 */
#ifdef CONFIG_MALI_DEBUG
#define KBASE_DEBUG_CODE(X) X
#else
#define KBASE_DEBUG_CODE(X) CSTD_NOP()
#endif				/*                   */

/*     */

/* 
                                               
  
                                                                             
                                                                     
  
                                               
  
                                                                     
                                                     
  
                                                                    
                                                                    
 */
void kbase_debug_assert_register_hook(kbase_debug_assert_hook *func, void *param);

/* 
                                                                                                
  
                                                                          
                                            
                                                                      
                                         
 */
void kbasep_debug_assert_call_hook(void);

#endif				/*                */
