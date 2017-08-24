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





#ifndef _ARM_CSTD_TYPES_GCC_H_
#define _ARM_CSTD_TYPES_GCC_H_

/*                                                                             
                 
                                                                             */
/*                                                                */
/*                                                          */
#if 1 == CSTD_OS_LINUX_KERNEL
	#include <linux/kernel.h>
	#include <linux/types.h>
	#include <linux/stddef.h>
	#include <linux/version.h>

	/*                                                                 */
	/*                                                          */
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
		typedef unsigned long   uintptr_t;
	#endif
	typedef long                intptr_t;

#else
	#include <stdint.h>
	#include <stddef.h>
	#include <limits.h>
#endif

typedef uint32_t                bool_t;

#if !defined(TRUE)
	#define TRUE                ((bool_t)1)
#endif

#if !defined(FALSE)
	#define FALSE               ((bool_t)0)
#endif

/*                                                                             
         
                                                                             */
/*                                                        */
#define ASM                     __asm__

#define INLINE                  __inline__

#define FORCE_INLINE            __attribute__((__always_inline__)) __inline__

#define NEVER_INLINE            __attribute__((__noinline__))

#define PURE                    __attribute__((__pure__))

#define PACKED                  __attribute__((__packed__))

/*                                                                           
                                                */

#define RESTRICT                __restrict__

/*                                                               */
#if 0 == CSTD_TOOLCHAIN_RVCT_GCC_MODE
	#define CHECK_RESULT        __attribute__((__warn_unused_result__))
#else
	#define CHECK_RESULT
#endif

/*                                                                     */
#if (0 == CSTD_TOOLCHAIN_RVCT_GCC_MODE)
	#define CSTD_FUNC           __func__
#else
	#define CSTD_FUNC           __FUNCTION__
#endif

#endif /*                              */
