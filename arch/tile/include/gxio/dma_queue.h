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

#ifndef _GXIO_DMA_QUEUE_H_
#define _GXIO_DMA_QUEUE_H_

/*
                                                           
 */

#include <gxio/common.h>

/*                                               */
#define DMA_QUEUE_CREDIT_SHIFT 32

/*
                                                                   
                                                            
                                                                
 */
typedef struct {
	/*
                                              
                                                             
                                                                  
                                                            
  */
	void *post_region_addr;

	/*
                                                              
              
  */
	uint64_t hw_complete_count __attribute__ ((aligned(64)));

	/*
                                                                 
                                           
  */
	int64_t credits_and_next_index;

} __gxio_dma_queue_t;

/*                         */
extern void __gxio_dma_queue_init(__gxio_dma_queue_t *dma_queue,
				  void *post_region_addr,
				  unsigned int num_entries);

/*
                                                                     
                                                                      
                                                                   
                                                
 */
extern void __gxio_dma_queue_update_credits(__gxio_dma_queue_t *dma_queue);

/*                                       */
extern int64_t __gxio_dma_queue_wait_for_credits(__gxio_dma_queue_t *dma_queue,
						 int64_t modifier);

/*                                                                   
                                                                       
                                            
 */
static inline int64_t __gxio_dma_queue_reserve(__gxio_dma_queue_t *dma_queue,
					       unsigned int num, bool wait,
					       bool completion)
{
	uint64_t slot;

	/*
                                                             
                                                                  
                                                                    
                              
  */
	int64_t modifier = (((int64_t)(-num)) << DMA_QUEUE_CREDIT_SHIFT) | num;
	int64_t old =
		__insn_fetchaddgez(&dma_queue->credits_and_next_index,
				   modifier);

	if (unlikely(old + modifier < 0)) {
		/*
                                                                
                                                             
   */
		__gxio_dma_queue_update_credits(dma_queue);
		old = __insn_fetchaddgez(&dma_queue->credits_and_next_index,
					 modifier);
		if (old + modifier < 0) {
			if (wait)
				old = __gxio_dma_queue_wait_for_credits
					(dma_queue, modifier);
			else
				return GXIO_ERR_DMA_CREDITS;
		}
	}

	/*                                              */
	slot = (old & 0xffffff);

	if (completion) {
		/*
                                                             
                                                               
                                                                   
                                                                  
                                                                
                                                                 
                                                    
   */
		uint64_t complete;
		complete = ACCESS_ONCE(dma_queue->hw_complete_count);
		slot |= (complete & 0xffffffffff000000);
		if (slot < complete)
			slot += 0x1000000;
	}

	/*
                                                                  
                                                                    
                                                          
  */
	if (unlikely(((old + num) & 0xff) < num)) {
		__gxio_dma_queue_update_credits(dma_queue);

		/*                                                        */
#ifdef __BIG_ENDIAN__
		*(((uint8_t *)&dma_queue->credits_and_next_index) + 4) = 0;
#else
		*(((uint8_t *)&dma_queue->credits_and_next_index) + 3) = 0;
#endif
	}

	return slot;
}

/*                                                      */
extern int64_t __gxio_dma_queue_reserve_aux(__gxio_dma_queue_t *dma_queue,
					    unsigned int num, int wait);

/*                                                            
  
                                                                 
                                                                 
 */
extern int __gxio_dma_queue_is_complete(__gxio_dma_queue_t *dma_queue,
					int64_t completion_slot, int update);

#endif /*                     */
