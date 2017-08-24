/*
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_MEMORY_TYPES_H__
#define __MALI_MEMORY_TYPES_H__

#if defined(CONFIG_MALI400_UMP)
#include "ump_kernel_interface.h"
#endif

typedef u32 mali_address_t;

typedef enum mali_mem_type {
	MALI_MEM_OS,
	MALI_MEM_EXTERNAL,
	MALI_MEM_DMA_BUF,
	MALI_MEM_UMP,
	MALI_MEM_BLOCK,
} mali_mem_type;

typedef struct mali_mem_os_mem {
	struct list_head pages;
	u32 count;
} mali_mem_os_mem;

typedef struct mali_mem_dma_buf {
#if defined(CONFIG_DMA_SHARED_BUFFER)
	struct mali_dma_buf_attachment *attachment;
#endif
} mali_mem_dma_buf;

typedef struct mali_mem_external {
	dma_addr_t phys;
	u32 size;
} mali_mem_external;

typedef struct mali_mem_ump {
#if defined(CONFIG_MALI400_UMP)
	ump_dd_handle handle;
#endif
} mali_mem_ump;

typedef struct block_allocator_allocation {
	/*                                            */
	struct block_info *last_allocated;
	u32 mapping_length;
	struct block_allocator *info;
} block_allocator_allocation;

typedef struct mali_mem_block_mem {
	block_allocator_allocation mem;
} mali_mem_block_mem;

typedef struct mali_mem_virt_mali_mapping {
	mali_address_t addr; /*                      */
	u32 properties;      /*                                            */
} mali_mem_virt_mali_mapping;

typedef struct mali_mem_virt_cpu_mapping {
	void __user *addr;
	u32 ref;
} mali_mem_virt_cpu_mapping;

#define MALI_MEM_ALLOCATION_VALID_MAGIC 0xdeda110c
#define MALI_MEM_ALLOCATION_FREED_MAGIC 0x10101010

typedef struct mali_mem_allocation {
	MALI_DEBUG_CODE(u32 magic);
	mali_mem_type type;                /*                  */
	int id;                            /*                                                */

	u32 size;                          /*                          */
	u32 flags;                         /*                             */

	struct mali_session_data *session; /*                                               */

	/*                         */
	union {
		mali_mem_os_mem os_mem;       /*               */
		mali_mem_external ext_mem;    /*                     */
		mali_mem_dma_buf dma_buf;     /*                    */
		mali_mem_ump ump_mem;         /*                */
		mali_mem_block_mem block_mem; /*                  */
	};

	mali_mem_virt_cpu_mapping cpu_mapping; /*               */
	mali_mem_virt_mali_mapping mali_mapping; /*                */
} mali_mem_allocation;

#define MALI_MEM_FLAG_MALI_GUARD_PAGE (1 << 0)
#define MALI_MEM_FLAG_DONT_CPU_MAP    (1 << 1)

#endif /*                       */
