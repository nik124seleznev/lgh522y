/*
 * Copyright (C) 2012-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __MALI_DMA_H__
#define __MALI_DMA_H__

#include "mali_osk.h"
#include "mali_osk_mali.h"
#include "mali_hw_core.h"

#define MALI_DMA_CMD_BUF_SIZE 1024

typedef struct mali_dma_cmd_buf {
	u32 *virt_addr;           /*                                 */
	u32 phys_addr;            /*                                      */
	u32 size;                 /*                                              */
} mali_dma_cmd_buf;

/*                              
  
                                                                       
                             
  
                                                         
                                                 
 */
struct mali_dma_core *mali_dma_create(_mali_osk_resource_t *resource);

/*                        
  
                                                                             
                                                                             
                                    
  
                                        
 */
void mali_dma_delete(struct mali_dma_core *dma);

/*                                                         
  
                                             
 */
struct mali_dma_core *mali_dma_get_global_dma_core(void);

/* 
                                              
  
                                            
                                                  
                                                                   
                                                      
 */
_mali_osk_errcode_t mali_dma_start(struct mali_dma_core* dma, mali_dma_cmd_buf *buf);

/* 
                              
  
                        
                                        
                                        
 */
MALI_STATIC_INLINE u32 mali_dma_command_write(struct mali_hw_core *core, u32 reg, u32 n)
{
	u32 core_offset = core->phys_offset;

	MALI_DEBUG_ASSERT(reg < 0x2000);
	MALI_DEBUG_ASSERT(n < 0x800);
	MALI_DEBUG_ASSERT(core_offset < 0x30000);
	MALI_DEBUG_ASSERT(0 == ((core_offset + reg) & ~0x7FFFF));

	return (n << 20) | (core_offset + reg);
}

/* 
                                                 
  
                                           
                                
                                                  
                                       
                                               
 */
MALI_STATIC_INLINE void mali_dma_write_array(mali_dma_cmd_buf *buf, struct mali_hw_core *core,
        u32 reg, u32 *data, u32 count)
{
	MALI_DEBUG_ASSERT((buf->size + 1 + count ) < MALI_DMA_CMD_BUF_SIZE / 4);

	buf->virt_addr[buf->size++] = mali_dma_command_write(core, reg, count);

	_mali_osk_memcpy(buf->virt_addr + buf->size, data, count * sizeof(*buf->virt_addr));

	buf->size += count;
}

/* 
                                                             
  
                                           
                                
                                                  
                                       
                                               
                                                                    
 */
MALI_STATIC_INLINE void mali_dma_write_array_conditional(mali_dma_cmd_buf *buf, struct mali_hw_core *core,
        u32 reg, u32 *data, u32 count, const u32 *ref)
{
	/*                                                                   
                        */
	mali_dma_write_array(buf, core, reg, data, count);
}

/* 
                                                                    
  
                                                                 
  
                                           
                                
                                                  
                                       
                                                                    
 */
MALI_STATIC_INLINE void mali_dma_write_conditional(mali_dma_cmd_buf *buf, struct mali_hw_core *core,
        u32 reg, u32 data, const u32 ref)
{
	/*                                                 */
	if (data == ref) return;

	buf->virt_addr[buf->size++] = mali_dma_command_write(core, reg, 1);

	buf->virt_addr[buf->size++] = data;

	MALI_DEBUG_ASSERT(buf->size < MALI_DMA_CMD_BUF_SIZE / 4);
}

/* 
                                                        
  
                                           
                                
                                                  
                                       
 */
MALI_STATIC_INLINE void mali_dma_write(mali_dma_cmd_buf *buf, struct mali_hw_core *core,
                                       u32 reg, u32 data)
{
	buf->virt_addr[buf->size++] = mali_dma_command_write(core, reg, 1);

	buf->virt_addr[buf->size++] = data;

	MALI_DEBUG_ASSERT(buf->size < MALI_DMA_CMD_BUF_SIZE / 4);
}

/* 
                                            
  
                                                 
  
                                             
                                                         
 */
_mali_osk_errcode_t mali_dma_get_cmd_buf(mali_dma_cmd_buf *buf);

/* 
                                                        
  
                                           
                                                              
 */
MALI_STATIC_INLINE mali_bool mali_dma_cmd_buf_is_valid(mali_dma_cmd_buf *buf)
{
	return NULL != buf->virt_addr;
}

/* 
                                     
  
                                                     
 */
void mali_dma_put_cmd_buf(mali_dma_cmd_buf *buf);

#endif /*                */
