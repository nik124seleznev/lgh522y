/*
 * Copyright 2012 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

#include <linux/io.h>
#include <linux/atomic.h>
#include <linux/module.h>
#include <gxio/dma_queue.h>

/*                                     */
#define wait_for_value(val)                             \
  __asm__ __volatile__("move %0, %0" :: "r"(val))

/*                             */
#define DMA_QUEUE_INDEX_MASK ((1 << 16) - 1)

/*
                                     
                                                                      
                                                                       
                                                                   
 */
typedef union {
	struct {
#ifndef __BIG_ENDIAN__
		uint64_t ring_idx:16;
		uint64_t count:16;
		uint64_t gen:1;
		uint64_t __reserved:31;
#else
		uint64_t __reserved:31;
		uint64_t gen:1;
		uint64_t count:16;
		uint64_t ring_idx:16;
#endif
	};
	uint64_t word;
} __gxio_ring_t;

void __gxio_dma_queue_init(__gxio_dma_queue_t *dma_queue,
			   void *post_region_addr, unsigned int num_entries)
{
	/*
                                                                   
                              
  */
	int64_t credits = (num_entries < 65536) ? num_entries : 65535;

	memset(dma_queue, 0, sizeof(*dma_queue));

	dma_queue->post_region_addr = post_region_addr;
	dma_queue->hw_complete_count = 0;
	dma_queue->credits_and_next_index = credits << DMA_QUEUE_CREDIT_SHIFT;
}

EXPORT_SYMBOL_GPL(__gxio_dma_queue_init);

void __gxio_dma_queue_update_credits(__gxio_dma_queue_t *dma_queue)
{
	__gxio_ring_t val;
	uint64_t count;
	uint64_t delta;
	uint64_t new_count;

	/*
                                                                   
                                                                 
                            
  */
	uint64_t orig_hw_complete_count =
		cmpxchg(&dma_queue->hw_complete_count,
			-1, -1);

	/*                                                             */
	wait_for_value(orig_hw_complete_count);

	/*                                                             */
	val.word = __gxio_mmio_read(dma_queue->post_region_addr);
	count = val.count;

	/*
                                                                 
                                                                  
                                  
  */
	delta = (count - orig_hw_complete_count) & 0xffff;
	if (delta == 0)
		return;

	/*
                                                                    
                                                            
                                                                    
                    
  */
	new_count = orig_hw_complete_count + delta;
	if (cmpxchg(&dma_queue->hw_complete_count,
		    orig_hw_complete_count,
		    new_count) != orig_hw_complete_count)
		return;

	/*
                                                                
                                           
  */
	__insn_fetchadd(&dma_queue->credits_and_next_index,
			(delta << DMA_QUEUE_CREDIT_SHIFT));
}

EXPORT_SYMBOL_GPL(__gxio_dma_queue_update_credits);

/*
                                                               
                                                                     
                                                             
 */
int64_t __gxio_dma_queue_wait_for_credits(__gxio_dma_queue_t *dma_queue,
					  int64_t modifier)
{
	int backoff = 16;
	int64_t old;

	do {
		int i;
		/*                                             */
		for (i = backoff; i > 0; i--)
			__insn_mfspr(SPR_PASS);

		/*                      */
		__gxio_dma_queue_update_credits(dma_queue);
		old = __insn_fetchaddgez(&dma_queue->credits_and_next_index,
					 modifier);

		/*                                                           */
		if (backoff < 256)
			backoff *= 2;
	} while (old + modifier < 0);

	return old;
}

EXPORT_SYMBOL_GPL(__gxio_dma_queue_wait_for_credits);

int64_t __gxio_dma_queue_reserve_aux(__gxio_dma_queue_t *dma_queue,
				     unsigned int num, int wait)
{
	return __gxio_dma_queue_reserve(dma_queue, num, wait != 0, true);
}

EXPORT_SYMBOL_GPL(__gxio_dma_queue_reserve_aux);

int __gxio_dma_queue_is_complete(__gxio_dma_queue_t *dma_queue,
				 int64_t completion_slot, int update)
{
	if (update) {
		if (ACCESS_ONCE(dma_queue->hw_complete_count) >
		    completion_slot)
			return 1;

		__gxio_dma_queue_update_credits(dma_queue);
	}

	return ACCESS_ONCE(dma_queue->hw_complete_count) > completion_slot;
}

EXPORT_SYMBOL_GPL(__gxio_dma_queue_is_complete);
