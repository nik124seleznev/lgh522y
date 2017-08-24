/*
 * Copyright (C) 2010, 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                            
                                                                         
               
 */

#ifndef __MALI_OSK_SPECIFIC_H__
#define __MALI_OSK_SPECIFIC_H__

#include <asm/uaccess.h>
#include <linux/platform_device.h>
#include <linux/dmapool.h>
#include <linux/gfp.h>
#include <linux/hardirq.h>

#include "mali_osk_types.h"
#include "mali_kernel_linux.h"

#define MALI_STATIC_INLINE static inline
#define MALI_NON_STATIC_INLINE inline

typedef struct dma_pool * mali_dma_pool;


MALI_STATIC_INLINE mali_dma_pool mali_dma_pool_create(u32 size, u32 alignment, u32 boundary)
{
	return dma_pool_create("mali-dma", &mali_platform_device->dev, size, alignment, boundary);
}

MALI_STATIC_INLINE void mali_dma_pool_destroy(mali_dma_pool pool)
{
	dma_pool_destroy(pool);
}

MALI_STATIC_INLINE mali_io_address mali_dma_pool_alloc(mali_dma_pool pool, u32 *phys_addr)
{
	return dma_pool_alloc(pool, GFP_KERNEL, phys_addr);
}

MALI_STATIC_INLINE void mali_dma_pool_free(mali_dma_pool pool, void* virt_addr, u32 phys_addr)
{
	dma_pool_free(pool, virt_addr, phys_addr);
}


#if defined(MALI_ENABLE_CPU_CYCLES)
/*                                                                  
                                                                   
                                                                      
                                                                
                                                                     
                                                                    
                                             
                                                                     
                                                                     
                                                                   
                                                */
static inline unsigned int mali_get_cpu_cyclecount(void)
{
	unsigned int value;
	/*                                               */
	asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(value));
	return value;
}

void mali_init_cpu_time_counters(int reset, int enable_divide_by_64);
#endif


MALI_STATIC_INLINE u32 _mali_osk_copy_from_user(void *to, void *from, u32 n)
{
	return (u32)copy_from_user(to, from, (unsigned long)n);
}

MALI_STATIC_INLINE mali_bool _mali_osk_in_atomic(void)
{
	return in_atomic();
}

#define _mali_osk_put_user(x, ptr) put_user(x, ptr)

#endif /*                         */
