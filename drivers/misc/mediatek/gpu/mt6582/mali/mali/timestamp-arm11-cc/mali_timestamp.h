/*
 * Copyright (C) 2010-2011, 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_TIMESTAMP_H__
#define __MALI_TIMESTAMP_H__

#include "mali_osk.h"

MALI_STATIC_INLINE _mali_osk_errcode_t _mali_timestamp_reset(void)
{
	/*
                                     
  */

	u32 mask = (1 << 0) | /*                           */
	           (0 << 1) | /*                                   */
	           (1 << 2) | /*                                         */
	           (0 << 3) | /*                                                                    */
	           (0 << 4) | /*                                   */
	           (0 << 5) | /*                                   */
	           (0 << 6) | /*                                */
	           (0 << 8) | /*                                                               */
	           (0 << 9) | /*                                                               */
	           (1 << 10); /*                                                                     */

	__asm__ __volatile__ ("MCR    p15, 0, %0, c15, c12, 0" : : "r" (mask) );

	return _MALI_OSK_ERR_OK;
}

MALI_STATIC_INLINE u64 _mali_timestamp_get(void)
{
	u32 result;

	/*                              */
	__asm__ __volatile__ ("MRC    p15, 0, %0, c15, c12, 1" : "=r" (result));

	return (u64)result;
}

#endif /*                      */