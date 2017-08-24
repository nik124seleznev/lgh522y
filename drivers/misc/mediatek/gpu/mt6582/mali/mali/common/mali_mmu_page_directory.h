/*
 * Copyright (C) 2011-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_MMU_PAGE_DIRECTORY_H__
#define __MALI_MMU_PAGE_DIRECTORY_H__

#include "mali_osk.h"

/* 
                               
 */
#define MALI_MMU_PAGE_SIZE 0x1000

/*
                                                            
 */
#define MALI_MMU_VIRTUAL_PAGE_SIZE 0x400000 /*       */

/* 
                                    
                                                             
 */
#define MALI_MMU_PDE_ENTRY(address) (((address)>>22) & 0x03FF)

/* 
                                
                                                         
 */
#define MALI_MMU_PTE_ENTRY(address) (((address)>>12) & 0x03FF)

/* 
                                                   
 */
#define MALI_MMU_ENTRY_ADDRESS(value) ((value) & 0xFFFFFC00)

#define MALI_INVALID_PAGE ((u32)(~0))

/* 
  
 */
typedef enum mali_mmu_entry_flags {
	MALI_MMU_FLAGS_PRESENT = 0x01,
	MALI_MMU_FLAGS_READ_PERMISSION = 0x02,
	MALI_MMU_FLAGS_WRITE_PERMISSION = 0x04,
	MALI_MMU_FLAGS_OVERRIDE_CACHE  = 0x8,
	MALI_MMU_FLAGS_WRITE_CACHEABLE  = 0x10,
	MALI_MMU_FLAGS_WRITE_ALLOCATE  = 0x20,
	MALI_MMU_FLAGS_WRITE_BUFFERABLE  = 0x40,
	MALI_MMU_FLAGS_READ_CACHEABLE  = 0x80,
	MALI_MMU_FLAGS_READ_ALLOCATE  = 0x100,
	MALI_MMU_FLAGS_MASK = 0x1FF,
} mali_mmu_entry_flags;


#define MALI_MMU_FLAGS_FORCE_GP_READ_ALLOCATE ( \
MALI_MMU_FLAGS_PRESENT | \
	MALI_MMU_FLAGS_READ_PERMISSION |  \
	MALI_MMU_FLAGS_WRITE_PERMISSION | \
	MALI_MMU_FLAGS_OVERRIDE_CACHE | \
	MALI_MMU_FLAGS_WRITE_CACHEABLE | \
	MALI_MMU_FLAGS_WRITE_BUFFERABLE | \
	MALI_MMU_FLAGS_READ_CACHEABLE | \
	MALI_MMU_FLAGS_READ_ALLOCATE )

#define MALI_MMU_FLAGS_DEFAULT ( \
	MALI_MMU_FLAGS_PRESENT | \
	MALI_MMU_FLAGS_READ_PERMISSION |  \
	MALI_MMU_FLAGS_WRITE_PERMISSION )


struct mali_page_directory {
	u32 page_directory; /*                                                           */
	mali_io_address page_directory_mapped; /*                                                                                       */

	mali_io_address page_entries_mapped[1024]; /*                                                                                                         */
	u32   page_entries_usage_count[1024]; /*                                                                                             */
};

/*                                                                                       */
_mali_osk_errcode_t mali_mmu_pagedir_map(struct mali_page_directory *pagedir, u32 mali_address, u32 size);
_mali_osk_errcode_t mali_mmu_pagedir_unmap(struct mali_page_directory *pagedir, u32 mali_address, u32 size);

/*                                                                                           */
void mali_mmu_pagedir_update(struct mali_page_directory *pagedir, u32 mali_address, u32 phys_address, u32 size, u32 cache_settings);

u32 mali_page_directory_get_phys_address(struct mali_page_directory *pagedir, u32 index);

u32 mali_allocate_empty_page(mali_io_address *virtual);
void mali_free_empty_page(u32 address, mali_io_address virtual);
_mali_osk_errcode_t mali_create_fault_flush_pages(u32 *page_directory, mali_io_address *page_directory_mapping,
        u32 *page_table, mali_io_address *page_table_mapping,
        u32 *data_page, mali_io_address *data_page_mapping);
void mali_destroy_fault_flush_pages(u32 *page_directory, mali_io_address *page_directory_mapping,
                                    u32 *page_table, mali_io_address *page_table_mapping,
                                    u32 *data_page, mali_io_address *data_page_mapping);

struct mali_page_directory *mali_mmu_pagedir_alloc(void);
void mali_mmu_pagedir_free(struct mali_page_directory *pagedir);

#endif /*                               */
