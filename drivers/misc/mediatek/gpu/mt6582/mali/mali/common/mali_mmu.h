/*
 * Copyright (C) 2010-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_MMU_H__
#define __MALI_MMU_H__

#include "mali_osk.h"
#include "mali_mmu_page_directory.h"
#include "mali_hw_core.h"

/*                                       */
struct mali_group;

/* 
                       
                                            
                                                                          
 */
typedef enum mali_mmu_register {
	MALI_MMU_REGISTER_DTE_ADDR = 0x0000, /*                                  */
	MALI_MMU_REGISTER_STATUS = 0x0004, /*                     */
	MALI_MMU_REGISTER_COMMAND = 0x0008, /*                                             */
	MALI_MMU_REGISTER_PAGE_FAULT_ADDR = 0x000C, /*                                          */
	MALI_MMU_REGISTER_ZAP_ONE_LINE = 0x010, /*                                                                */
	MALI_MMU_REGISTER_INT_RAWSTAT = 0x0014, /*                                                */
	MALI_MMU_REGISTER_INT_CLEAR = 0x0018, /*                                                            */
	MALI_MMU_REGISTER_INT_MASK = 0x001C, /*                                      */
	MALI_MMU_REGISTER_INT_STATUS = 0x0020 /*                                      */
} mali_mmu_register;

/* 
                              
                                          
                                                
                                                       
                      
 */
typedef enum mali_mmu_interrupt {
	MALI_MMU_INTERRUPT_PAGE_FAULT = 0x01, /*                        */
	MALI_MMU_INTERRUPT_READ_BUS_ERROR = 0x02 /*                            */
} mali_mmu_interrupt;

typedef enum mali_mmu_status_bits {
	MALI_MMU_STATUS_BIT_PAGING_ENABLED      = 1 << 0,
	MALI_MMU_STATUS_BIT_PAGE_FAULT_ACTIVE   = 1 << 1,
	MALI_MMU_STATUS_BIT_STALL_ACTIVE        = 1 << 2,
	MALI_MMU_STATUS_BIT_IDLE                = 1 << 3,
	MALI_MMU_STATUS_BIT_REPLAY_BUFFER_EMPTY = 1 << 4,
	MALI_MMU_STATUS_BIT_PAGE_FAULT_IS_WRITE = 1 << 5,
	MALI_MMU_STATUS_BIT_STALL_NOT_ACTIVE    = 1 << 31,
} mali_mmu_status_bits;

/* 
                               
                                          
                                                          
 */
struct mali_mmu_core {
	struct mali_hw_core hw_core; /*                           */
	_mali_osk_irq_t *irq;        /*               */
};

_mali_osk_errcode_t mali_mmu_initialize(void);

void mali_mmu_terminate(void);

struct mali_mmu_core *mali_mmu_create(_mali_osk_resource_t *resource, struct mali_group *group, mali_bool is_virtual);
void mali_mmu_delete(struct mali_mmu_core *mmu);

_mali_osk_errcode_t mali_mmu_reset(struct mali_mmu_core *mmu);
mali_bool mali_mmu_zap_tlb(struct mali_mmu_core *mmu);
void mali_mmu_zap_tlb_without_stall(struct mali_mmu_core *mmu);
void mali_mmu_invalidate_page(struct mali_mmu_core *mmu, u32 mali_address);

void mali_mmu_activate_page_directory(struct mali_mmu_core* mmu, struct mali_page_directory *pagedir);
void mali_mmu_activate_empty_page_directory(struct mali_mmu_core* mmu);
void mali_mmu_activate_fault_flush_page_directory(struct mali_mmu_core* mmu);

void mali_mmu_page_fault_done(struct mali_mmu_core *mmu);

/*                                        */
MALI_STATIC_INLINE u32 mali_mmu_get_int_status(struct mali_mmu_core *mmu)
{
	return mali_hw_core_register_read(&mmu->hw_core, MALI_MMU_REGISTER_INT_STATUS);
}

MALI_STATIC_INLINE u32 mali_mmu_get_rawstat(struct mali_mmu_core *mmu)
{
	return mali_hw_core_register_read(&mmu->hw_core, MALI_MMU_REGISTER_INT_RAWSTAT);
}

MALI_STATIC_INLINE void mali_mmu_mask_all_interrupts(struct mali_mmu_core *mmu)
{
	mali_hw_core_register_write(&mmu->hw_core, MALI_MMU_REGISTER_INT_MASK, 0);
}

MALI_STATIC_INLINE u32 mali_mmu_get_status(struct mali_mmu_core *mmu)
{
	return mali_hw_core_register_read(&mmu->hw_core, MALI_MMU_REGISTER_STATUS);
}

MALI_STATIC_INLINE u32 mali_mmu_get_page_fault_addr(struct mali_mmu_core *mmu)
{
	return mali_hw_core_register_read(&mmu->hw_core, MALI_MMU_REGISTER_PAGE_FAULT_ADDR);
}

#endif /*                */
