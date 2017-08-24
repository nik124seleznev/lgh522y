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

#ifndef _GXIO_MPIPE_H_
#define _GXIO_MPIPE_H_

/*
  
                                                                      
             
 */

#include <gxio/common.h>
#include <gxio/dma_queue.h>

#include <linux/time.h>

#include <arch/mpipe_def.h>
#include <arch/mpipe_shm.h>

#include <hv/drv_mpipe_intf.h>
#include <hv/iorpc.h>

/*
  
                                                                    
                                                           
                                                                      
                                                            
                                                                 
                                                                     
                                                              
                         
  
                                                                
                                                                 
                                                                      
                                                               
                                                                 
                                                             
                                                             
                                                              
  
                                                                   
                                                                     
                                                                    
                                                              
                       
  
                                                           
  
                                                                     
                                                               
                                                                 
                                                                     
                                                                      
                        
  
                                                                   
                                                                  
                                                         
  
                                                   
  
                                                                     
                                                                    
                                                                  
                                                                     
                                                                     
                                                                   
  
                                                                   
                                                                      
                                                                    
                                                                  
                                                                 
                                                                 
                                                                  
                                                               
                                             
  
                                                   
  
                                                                    
                                                                    
                                                                 
                                                                    
                                                                      
                                                                   
                                                                   
                                                             
                                                                    
                                                                     
                                                                      
                                                                    
                              
  
                                                                     
                                                                  
                                                                   
                                                                 
                                                        
                                             
  
                                                                  
  
                                                                   
                                                                      
                                                                     
                                                                  
                                                                   
                                                                   
                                                                      
                                                               
                                                            
                                                                      
                                                                    
                
  
                                                               
                                                             
                                                                  
                                           
  
                                                                  
                                                                      
        
  
                                                                     
                                                                      
                                                                      
                                                                  
                                                                   
                                                 
  
                                       
  
                                                                
                                                              
                                                                 
                                                                   
                                                                   
                                                                   
                                                 
  
                                               
  
                                                                
                                                                   
                                                                  
                                                                  
                                                       
                            
  
                                                             
  
                                                                    
                        
  
                                                                    
                                                                 
                                                       
  
                                                               
                                     
  
                                               
  
                               
  
                                                     
  
                                         
  
                                                                      
                                                                  
  
                                                  
  
                                                   
  
                                             
 */

/*                                                            */
enum gxio_mpipe_alloc_flags_e {
	/*                                                               */
	GXIO_MPIPE_ALLOC_FIXED = HV_MPIPE_ALLOC_FIXED,
};

/*                                                            */
enum gxio_mpipe_mem_flags_e {
	/*                                                                */
	GXIO_MPIPE_MEM_FLAG_NT_HINT = IORPC_MEM_BUFFER_FLAG_NT_HINT,

	/*                                                    */
	GXIO_MPIPE_MEM_FLAG_IO_PIN = IORPC_MEM_BUFFER_FLAG_IO_PIN,
};

/*                                                                
                                                                    
 */
typedef MPIPE_PDESC_t gxio_mpipe_idesc_t;

/*                                                                 
                                                                    
                                                                    
                                                                 
 */
typedef MPIPE_EDMA_DESC_t gxio_mpipe_edesc_t;

/*                                    
  
                                                                     
                      
  
                                                                      
                                                                    
                           
  
                                                                
  
                                             
 */
static inline unsigned char *gxio_mpipe_idesc_get_va(gxio_mpipe_idesc_t *idesc)
{
	return (unsigned char *)(long)idesc->va;
}

/*                                     
  
                                                                    
                   
  
                                                                
  
                                             
  
                                         
                                                      
 */
static inline unsigned int gxio_mpipe_idesc_get_xfer_size(gxio_mpipe_idesc_t
							  *idesc)
{
	return idesc->l2_size;
}

/*                                     
  
                                                                    
  
                                                                  
  
                                                                    
                                                                 
                                                                     
                                  
  
                                                                  
                                                       
  
                                             
 */
static inline uint8_t gxio_mpipe_idesc_get_l2_offset(gxio_mpipe_idesc_t *idesc)
{
	return (idesc->custom1 >> 32) & 0xFF;
}

/*                                    
  
                                                
                                    
  
                                             
 */
static inline unsigned char *gxio_mpipe_idesc_get_l2_start(gxio_mpipe_idesc_t
							   *idesc)
{
	unsigned char *va = gxio_mpipe_idesc_get_va(idesc);
	return va + gxio_mpipe_idesc_get_l2_offset(idesc);
}

/*                                     
  
                                                        
                                    
  
                                             
 */
static inline unsigned int gxio_mpipe_idesc_get_l2_length(gxio_mpipe_idesc_t
							  *idesc)
{
	unsigned int xfer_size = idesc->l2_size;
	return xfer_size - gxio_mpipe_idesc_get_l2_offset(idesc);
}

/*                                                           */
typedef struct {

	/*                                                            */
	int fd;

	/*                                                     */
	char *mmio_cfg_base;

	/*                                                            */
	char *mmio_fast_base;

	/*                                  */
	gxio_mpipe_rules_stacks_t __stacks;

} gxio_mpipe_context_t;

/*                                                                       */
typedef gxio_mpipe_context_t gxio_mpipe_info_context_t;

/*                             
  
                                                                      
                                        
  
                                                   
                                                                           
            
 */
extern int gxio_mpipe_init(gxio_mpipe_context_t *context,
			   unsigned int mpipe_instance);

/*                          
  
                                                                     
                                        
  
                                                                   
                                                              
  
                                                 
 */
extern int gxio_mpipe_destroy(gxio_mpipe_context_t *context);

/*                                                                
                                                                   
                                                                  */

/*                                 
  
                                                        
  
                                               
                                          
                                                                             
                       
                                                          
                                                    
                                                         
 */
extern int gxio_mpipe_alloc_buffer_stacks(gxio_mpipe_context_t *context,
					  unsigned int count,
					  unsigned int first,
					  unsigned int flags);

/*                                                 */
typedef enum {
	/*                              */
	GXIO_MPIPE_BUFFER_SIZE_128 = MPIPE_BSM_INIT_DAT_1__SIZE_VAL_BSZ_128,
	/*                              */
	GXIO_MPIPE_BUFFER_SIZE_256 = MPIPE_BSM_INIT_DAT_1__SIZE_VAL_BSZ_256,
	/*                              */
	GXIO_MPIPE_BUFFER_SIZE_512 = MPIPE_BSM_INIT_DAT_1__SIZE_VAL_BSZ_512,
	/*                               */
	GXIO_MPIPE_BUFFER_SIZE_1024 = MPIPE_BSM_INIT_DAT_1__SIZE_VAL_BSZ_1024,
	/*                               */
	GXIO_MPIPE_BUFFER_SIZE_1664 = MPIPE_BSM_INIT_DAT_1__SIZE_VAL_BSZ_1664,
	/*                               */
	GXIO_MPIPE_BUFFER_SIZE_4096 = MPIPE_BSM_INIT_DAT_1__SIZE_VAL_BSZ_4096,
	/*                                */
	GXIO_MPIPE_BUFFER_SIZE_10368 =
		MPIPE_BSM_INIT_DAT_1__SIZE_VAL_BSZ_10368,
	/*                                */
	GXIO_MPIPE_BUFFER_SIZE_16384 = MPIPE_BSM_INIT_DAT_1__SIZE_VAL_BSZ_16384
} gxio_mpipe_buffer_size_enum_t;

/*                                                         */
extern gxio_mpipe_buffer_size_enum_t
gxio_mpipe_buffer_size_to_buffer_size_enum(size_t size);

/*                                                         */
extern size_t
gxio_mpipe_buffer_size_enum_to_buffer_size(gxio_mpipe_buffer_size_enum_t
					   buffer_size_enum);

/*                                                                  
                                                           
                                  
 */
extern size_t gxio_mpipe_calc_buffer_stack_bytes(unsigned long buffers);

/*                                                                   
                                                                     
                                                                    
                                                                    
                                                                    
                                                                      
                           
  
                                               
                                       
                                                                       
              
                                                                   
                                                        
                                                          
                                                          
                                                                 
                                                                    
                                
 */
extern int gxio_mpipe_init_buffer_stack(gxio_mpipe_context_t *context,
					unsigned int stack,
					gxio_mpipe_buffer_size_enum_t
					buffer_size_enum, void *mem,
					size_t mem_size,
					unsigned int mem_flags);

/*                                                          
  
                                                                   
                                                                     
                                                                  
                                  
  
                                               
                                       
                                                             
 */
static inline void gxio_mpipe_push_buffer(gxio_mpipe_context_t *context,
					  unsigned int stack, void *buffer)
{
	MPIPE_BSM_REGION_ADDR_t offset = { {0} };
	MPIPE_BSM_REGION_VAL_t val = { {0} };

	/*
                                                                    
                            
  */
	offset.region =
		MPIPE_MMIO_ADDR__REGION_VAL_BSM -
		MPIPE_MMIO_ADDR__REGION_VAL_IDMA;
	offset.stack = stack;

#if __SIZEOF_POINTER__ == 4
	val.va = ((ulong) buffer) >> MPIPE_BSM_REGION_VAL__VA_SHIFT;
#else
	val.va = ((long)buffer) >> MPIPE_BSM_REGION_VAL__VA_SHIFT;
#endif

	__gxio_mmio_write(context->mmio_fast_base + offset.word, val.word);
}

/*                                                           
  
                                               
                                       
                                                     
 */
static inline void *gxio_mpipe_pop_buffer(gxio_mpipe_context_t *context,
					  unsigned int stack)
{
	MPIPE_BSM_REGION_ADDR_t offset = { {0} };

	/*
                                                                    
                            
  */
	offset.region =
		MPIPE_MMIO_ADDR__REGION_VAL_BSM -
		MPIPE_MMIO_ADDR__REGION_VAL_IDMA;
	offset.stack = stack;

	while (1) {
		/*
                                                    
                                              
                                              
   */
		MPIPE_BSM_REGION_VAL_t val;
		val.word =
			__gxio_mmio_read(context->mmio_fast_base +
					 offset.word);

		/*
                                                           
                                                                
   */
		if (val.c != MPIPE_EDMA_DESC_WORD1__C_VAL_NOT_RDY)
			return (void *)((unsigned long)val.
					va << MPIPE_BSM_REGION_VAL__VA_SHIFT);
	}
}

/*                                                                
                                                                   
                                                                  */

/*                              
  
                                                        
  
                                                                     
                                                                 
  
                                               
                                              
                                                                         
                               
                                                          
                                                        
                                                       
 */
extern int gxio_mpipe_alloc_notif_rings(gxio_mpipe_context_t *context,
					unsigned int count, unsigned int first,
					unsigned int flags);

/*                                                         
  
                                               
                                   
                                                                   
                                                  
                                                                  
                                               
                                                          
  
                                                           
                                           
 */
extern int gxio_mpipe_init_notif_ring(gxio_mpipe_context_t *context,
				      unsigned int ring,
				      void *mem, size_t mem_size,
				      unsigned int mem_flags);

/*                                                                  
                                                                      
                                                                         
  
                                               
                                                  
                                                  
                                                                       
                                                                            
                        
                                   
                                                                      
 */
extern int gxio_mpipe_request_notif_ring_interrupt(gxio_mpipe_context_t
						   *context, int x, int y,
						   int i, int e,
						   unsigned int ring);

/*                                                   
  
                                               
                                   
                                                                      
 */
extern int gxio_mpipe_enable_notif_ring_interrupt(gxio_mpipe_context_t
						  *context, unsigned int ring);

/*                                                  
                                               
                            
                               
                      
                                                     
 */
extern int gxio_mpipe_register_client_memory(gxio_mpipe_context_t *context,
					     unsigned int iotlb, HV_PTE pte,
					     unsigned int flags);

/*                                                                
                                                                   
                                                                  */

/*                               
  
                                                        
  
                                               
                                               
                                                                          
                               
                                                          
                                                         
                                                        
 */
extern int gxio_mpipe_alloc_notif_groups(gxio_mpipe_context_t *context,
					 unsigned int count,
					 unsigned int first,
					 unsigned int flags);

/*                                                              
                                                               
                                                                     
                                             
 */
static inline void
gxio_mpipe_notif_group_add_ring(gxio_mpipe_notif_group_bits_t *bits, int ring)
{
	bits->ring_mask[ring / 64] |= (1ull << (ring % 64));
}

/*                                                              
                                                                  
                                                                    
                                                                  
 */
extern int gxio_mpipe_init_notif_group(gxio_mpipe_context_t *context,
				       unsigned int group,
				       gxio_mpipe_notif_group_bits_t bits);

/*                                                                
                                                                   
                                                                  */

/*                                         
  
                                                        
  
                                                                  
                                                                 
  
                                                                     
                                                                    
                                                                    
                                  
  
                                               
                                           
                                                                              
                       
                                                          
                                                     
                                                   
 */
extern int gxio_mpipe_alloc_buckets(gxio_mpipe_context_t *context,
				    unsigned int count, unsigned int first,
				    unsigned int flags);

/*                                                 
                                             
  
                                                                        
                                                                     
                                                                      
                                                                      
                                                                     
                                                                     
                                                               
 */
typedef enum {
	/*                                                             
                                                                   
                                                                     
              
  */
	GXIO_MPIPE_BUCKET_DYNAMIC_FLOW_AFFINITY =
		MPIPE_LBL_INIT_DAT_BSTS_TBL__MODE_VAL_DFA,

	/*                                                          
  */
	GXIO_MPIPE_BUCKET_STATIC_FLOW_AFFINITY =
		MPIPE_LBL_INIT_DAT_BSTS_TBL__MODE_VAL_FIXED,

	/*                                                               
                                                         
  */
	GXIO_MPIPE_BUCKET_ROUND_ROBIN =
		MPIPE_LBL_INIT_DAT_BSTS_TBL__MODE_VAL_ALWAYS_PICK,

	/*                                                             
                                                                   
                                                                
                                            
  */
	GXIO_MPIPE_BUCKET_STICKY_FLOW_LOCALITY =
		MPIPE_LBL_INIT_DAT_BSTS_TBL__MODE_VAL_STICKY,

	/*                                                             
                                                                    
                                                                  
                                                                  
                                                                   
  */
	GXIO_MPIPE_BUCKET_PREFER_FLOW_LOCALITY =
		MPIPE_LBL_INIT_DAT_BSTS_TBL__MODE_VAL_STICKY_RAND,

} gxio_mpipe_bucket_mode_t;

/*                                                          
                                                                   
                                                            
                                                                   
                                         
  
                                               
                                                
                                                                         
                                                                
 */
extern int gxio_mpipe_init_bucket(gxio_mpipe_context_t *context,
				  unsigned int bucket,
				  gxio_mpipe_bucket_info_t bucket_info);

/*                                                                 
                                                                    
  
                                                        
  
                                                                    
                                                
  
                                                                      
                                                                     
                                       
  
                                               
                          
                              
                                        
                                  
                                            
                                       
  
                                                     
                                       
                                              
 */
extern int gxio_mpipe_init_notif_group_and_buckets(gxio_mpipe_context_t
						   *context,
						   unsigned int group,
						   unsigned int ring,
						   unsigned int num_rings,
						   unsigned int bucket,
						   unsigned int num_buckets,
						   gxio_mpipe_bucket_mode_t
						   mode);

/*                                             
  
                                               
                                          
                                   
                                                
 */
static inline void gxio_mpipe_credit(gxio_mpipe_context_t *context,
				     int ring, int bucket, unsigned int count)
{
	/*                                                                  */

	MPIPE_IDMA_RELEASE_REGION_ADDR_t offset = { {0} };
	MPIPE_IDMA_RELEASE_REGION_VAL_t val = { {0} };

	/*
                                                                    
                            
  */
	offset.region =
		MPIPE_MMIO_ADDR__REGION_VAL_IDMA -
		MPIPE_MMIO_ADDR__REGION_VAL_IDMA;
	offset.ring = ring;
	offset.bucket = bucket;
	offset.ring_enable = (ring >= 0);
	offset.bucket_enable = (bucket >= 0);
	val.count = count;

	__gxio_mmio_write(context->mmio_fast_base + offset.word, val.word);
}

/*                                                                
                                                                   
                                                                  */

/*                              
  
                                                        
  
                                               
                                              
                                                                         
                               
                                                          
                                                        
                                                      
 */
extern int gxio_mpipe_alloc_edma_rings(gxio_mpipe_context_t *context,
				       unsigned int count, unsigned int first,
				       unsigned int flags);

/*                                                          
  
                                               
                                   
                                                                       
                                                   
                                                                   
                                                  
                                                                   
                                                             
                                                          
  
                                                          
                                           
 */
extern int gxio_mpipe_init_edma_ring(gxio_mpipe_context_t *context,
				     unsigned int ring, unsigned int channel,
				     void *mem, size_t mem_size,
				     unsigned int mem_flags);

/*                                                                
                                                                   
                                                                  */

/*
  
                                                                     
  
                                                                     
                                                                      
                                                                    
                                                                   
                                                                   
                                                                      
                       
  
                                                            
  
                                                                 
                                                                  
                                                                  
                                                                  
                                                                    
                                                          
  
                                                               
                                      
  
                                                                   
                                                                      
                                                                      
                                                                      
                                                                    
                                                                    
                                                                      
                                           
  
                                                              
                         
  
                                                             
                                                                    
                                                                    
  
                                                              
                                                                    
                                                                      
                                        
  
                                                                     
                                                                      
                                                                      
                                                                     
                                                                  
                  
  
                                                            
  
                                                                  
                                                        
  
                                                              
                                                                 
                                                
  
                                                                      
                                                                    
                                                 
 */

/*                                            */
typedef struct {

	/*              */
	gxio_mpipe_context_t *context;

	/*                   */
	gxio_mpipe_rules_list_t list;

} gxio_mpipe_rules_t;

/*                                            
  
                                                                     
                                         
  
                                         
                                               
 */
extern void gxio_mpipe_rules_init(gxio_mpipe_rules_t *rules,
				  gxio_mpipe_context_t *context);

/*                                              
  
                                                                      
                      
  
                                                         
                                                                    
             
                                                                       
                                                              
                                                                   
                                                                  
                                                                 
                                                                     
                                                             
                                                    
                                                             
 */
extern int gxio_mpipe_rules_begin(gxio_mpipe_rules_t *rules,
				  unsigned int bucket,
				  unsigned int num_buckets,
				  gxio_mpipe_rules_stacks_t *stacks);

/*                                      
  
                                                               
                                
                                                             
 */
extern int gxio_mpipe_rules_set_headroom(gxio_mpipe_rules_t *rules,
					 uint8_t headroom);

/*                                                                 
                                                                     
  
                                                                           
                                                                          
                                                 
  
                                                               
                                     
                                                             
 */
extern int gxio_mpipe_rules_add_channel(gxio_mpipe_rules_t *rules,
					unsigned int channel);

/*              
  
                                                                     
                                                                          
  
                                                                       
                                                                     
                          
  
                                     
                                                             
 */
extern int gxio_mpipe_rules_commit(gxio_mpipe_rules_t *rules);

/*                                                                
                                                                   
                                                                  */

/*
  
                                                                   
                                    
  
                                                                  
                                                                  
                                                                   
                                                                    
                                                                       
                                                                     
                            
  
                                                                     
                                                                     
                                                           
                                                                      
                                                                      
                                                                     
                                                               
                                                       
                                                         
                                                             
  
                                                         
  
                                                                   
                                                                    
                                                  
                                                                 
                                                                       
                                                                   
                                                            
  
                                                                   
                                                                    
                                                    
                                                                      
                                                                      
                                                                      
                                                                      
                                                                      
                                                                      
                                                  
  
                                                        
  
                                                              
                                                     
                                                              
                                                                      
                                                                      
                 
  
                                                              
                                                                      
                                                                 
                                                                 
                                                                  
                                                                    
                                                     
  
                                                                      
                                                                   
                                                                  
                                                                  
                                                               
                                                        
                                                                     
                                                                 
                                                     
  
                                                                   
                                                                 
                                                                      
                                                                      
                                                                   
                                                                     
                                                                    
                                                     
  
                                                              
                                                                
                                                                      
                                                                
                                                                    
                                                                   
                                                                 
                                  
 */

/*                                                                   
 */
typedef struct {

	/*              */
	gxio_mpipe_context_t *context;

	/*                       */
	gxio_mpipe_idesc_t *idescs;

	/*                        */
	unsigned long num_entries;

	/*                                  */
	unsigned long mask_num_entries;

	/*                                      */
	unsigned long log2_num_entries;

	/*                 */
	unsigned int head;

	/*                   */
	unsigned int ring;

#ifdef __BIG_ENDIAN__
	/*                                    */
	unsigned int swapped;
#endif

} gxio_mpipe_iqueue_t;

/*                        
  
                                                                       
 */
extern int gxio_mpipe_iqueue_init(gxio_mpipe_iqueue_t *iqueue,
				  gxio_mpipe_context_t *context,
				  unsigned int ring,
				  void *mem, size_t mem_size,
				  unsigned int mem_flags);

/*                                            
  
                                                                
  
                                                                           
                                                      
 */
static inline void gxio_mpipe_iqueue_advance(gxio_mpipe_iqueue_t *iqueue,
					     int count)
{
	/*                           */
	int head = iqueue->head + count;
	iqueue->head =
		(head & iqueue->mask_num_entries) +
		(head >> iqueue->log2_num_entries);

#ifdef __BIG_ENDIAN__
	/*                              */
	iqueue->swapped -= count;
#endif
}

/*                                                           
  
                                                                      
  
                                                                      
                                                               
  
                                                                     
                                                                     
                                                               
                               
  
                                                                      
                                     
  
                                                                
  
                                                                           
                                                   
 */
static inline void gxio_mpipe_iqueue_release(gxio_mpipe_iqueue_t *iqueue,
					     gxio_mpipe_idesc_t *idesc)
{
	gxio_mpipe_credit(iqueue->context, iqueue->ring, idesc->bucket_id, 1);
}

/*                                   
  
                                                                  
                                                                   
                                                                   
                                                                   
  
                                                                     
                                                  
  
                                                                   
                                                                   
                                                                    
                                                                 
  
                                                                 
                                                                  
                                                                
                                                   
  
                                                                           
                                                   
 */
static inline void gxio_mpipe_iqueue_consume(gxio_mpipe_iqueue_t *iqueue,
					     gxio_mpipe_idesc_t *idesc)
{
	gxio_mpipe_iqueue_advance(iqueue, 1);
	gxio_mpipe_iqueue_release(iqueue, idesc);
}

/*                                                            
  
                                                                   
                                                                     
                                                                    
                                                                  
                                                
  
                                                                   
                                                                 
  
                                                                           
                                                             
                                                                   
                                                                
 */
static inline int gxio_mpipe_iqueue_try_peek(gxio_mpipe_iqueue_t *iqueue,
					     gxio_mpipe_idesc_t **idesc_ref)
{
	gxio_mpipe_idesc_t *next;

	uint64_t head = iqueue->head;
	uint64_t tail = __gxio_mmio_read(iqueue->idescs);

	/*                    */
	uint64_t avail =
		(tail >= head) ? (tail - head) : (iqueue->num_entries - head);

	if (avail == 0) {
		*idesc_ref = NULL;
		return GXIO_MPIPE_ERR_IQUEUE_EMPTY;
	}

	next = &iqueue->idescs[head];

	/*                         */
	__insn_prefetch(next);

#ifdef __BIG_ENDIAN__
	/*                                            */
	{
		int i, j;
		for (i = iqueue->swapped; i < avail; i++) {
			for (j = 0; j < 8; j++)
				next[i].words[j] =
					__builtin_bswap64(next[i].words[j]);
		}
		iqueue->swapped = avail;
	}
#endif

	*idesc_ref = next;

	return avail;
}

/*                                                      
  
                                                                        
                                                                      
  
                                                                           
                                    
 */
static inline void gxio_mpipe_iqueue_drop(gxio_mpipe_iqueue_t *iqueue,
					  gxio_mpipe_idesc_t *idesc)
{
	/*                                    */

	if (!idesc->be) {
		unsigned char *va = gxio_mpipe_idesc_get_va(idesc);
		gxio_mpipe_push_buffer(iqueue->context, idesc->stack_idx, va);
	}
}

/*                                                                
                                                                   
                                                                  */

/*                                                      */
typedef struct {

	/*                                                   */
	__gxio_dma_queue_t dma_queue;

	/*                   */
	gxio_mpipe_edesc_t *edescs;

	/*                                  */
	unsigned long mask_num_entries;

	/*                                      */
	unsigned long log2_num_entries;

} gxio_mpipe_equeue_t;

/*                        
  
                                                                      
 */
extern int gxio_mpipe_equeue_init(gxio_mpipe_equeue_t *equeue,
				  gxio_mpipe_context_t *context,
				  unsigned int edma_ring_id,
				  unsigned int channel,
				  void *mem, unsigned int mem_size,
				  unsigned int mem_flags);

/*                                     
  
                                                                 
  
                                                                     
                                                                
                                   
  
                                                                          
                                                            
                                                                        
 */
static inline int64_t gxio_mpipe_equeue_reserve(gxio_mpipe_equeue_t *equeue,
						unsigned int num)
{
	return __gxio_dma_queue_reserve_aux(&equeue->dma_queue, num, true);
}

/*                                                  
  
                                                                 
  
                                                                     
                                                                    
                                   
  
                                                                          
                                                            
                                                                        
 */
static inline int64_t gxio_mpipe_equeue_try_reserve(gxio_mpipe_equeue_t
						    *equeue, unsigned int num)
{
	return __gxio_dma_queue_reserve_aux(&equeue->dma_queue, num, false);
}

/*                          
  
                                                                 
  
                                                                
                                                                     
                                                            
  
                                                                          
                                                              
                                                             
 */
static inline int64_t gxio_mpipe_equeue_reserve_fast(gxio_mpipe_equeue_t
						     *equeue, unsigned int num)
{
	return __gxio_dma_queue_reserve(&equeue->dma_queue, num, true, false);
}

/*                                       
  
                                                                 
  
                                                                    
                                                                     
                                                            
  
                                                                          
                                                              
                                                             
 */
static inline int64_t gxio_mpipe_equeue_try_reserve_fast(gxio_mpipe_equeue_t
							 *equeue,
							 unsigned int num)
{
	return __gxio_dma_queue_reserve(&equeue->dma_queue, num, false, false);
}

/*
                                                                 
                                                                  
 */

static inline void gxio_mpipe_equeue_put_at_aux(gxio_mpipe_equeue_t *equeue,
						uint_reg_t ew[2],
						unsigned long slot)
{
	unsigned long edma_slot = slot & equeue->mask_num_entries;
	gxio_mpipe_edesc_t *edesc_p = &equeue->edescs[edma_slot];

	/*
                                                                    
                                                                     
  */
	ew[0] |= !((slot >> equeue->log2_num_entries) & 1);

	/*
                                                                    
                                                                   
                                                                    
  */
	__gxio_mmio_write64(&edesc_p->words[1], ew[1]);
	__gxio_mmio_write64(&edesc_p->words[0], ew[0]);
}

/*                                            
  
                                                                     
                                                                      
                                                                      
                                                                   
                                                    
  
                                                              
                                                                
                                           
                                    
  
                                                                   
                                                                      
                                                                    
                                                    
  
                                                                          
                                                   
                                                                    
 */
static inline void gxio_mpipe_equeue_put_at(gxio_mpipe_equeue_t *equeue,
					    gxio_mpipe_edesc_t edesc,
					    unsigned long slot)
{
	gxio_mpipe_equeue_put_at_aux(equeue, edesc.words, slot);
}

/*                                             
  
                                       
                                                                   
  
                                                                          
                                                   
                        
 */
static inline int gxio_mpipe_equeue_put(gxio_mpipe_equeue_t *equeue,
					gxio_mpipe_edesc_t edesc)
{
	int64_t slot = gxio_mpipe_equeue_reserve_fast(equeue, 1);
	if (slot < 0)
		return (int)slot;

	gxio_mpipe_equeue_put_at(equeue, edesc, slot);

	return 0;
}

/*                                                                  
  
                                                                       
  
                                                                      
                                                                    
                                                                     
                                                                      
                                                                    
                          
  
                                                                          
 */
static inline void gxio_mpipe_equeue_flush(gxio_mpipe_equeue_t *equeue)
{
	/*                                                                */
	MPIPE_EDMA_POST_REGION_VAL_t val = { {0} };
	/*                          */
	__insn_flushwb();
	__gxio_mmio_write(equeue->dma_queue.post_region_addr, val.word);
}

/*                                               
  
                                                                     
                                                                     
                                        
  
                                                                          
                                                                
                                                                        
                                                                          
                                                  
                                                       
 */
static inline int gxio_mpipe_equeue_is_complete(gxio_mpipe_equeue_t *equeue,
						int64_t completion_slot,
						int update)
{
	return __gxio_dma_queue_is_complete(&equeue->dma_queue,
					    completion_slot, update);
}

/*                                                                
                                                                   
                                                                  */

/*
  
                                                                     
                             
  
                                                 
  
                                                                      
                                                                             
                                                      
  
                                                                         
                                                                           
                                                                        
                               
  
                                                                           
                                                                              
                                                                            
  
                                                                               
                                                                         
                                                           
  
                                                                          
                                                                        
                                                                         
                                                                        
  
                                                                         
                                                                         
                                                                           
                                                                          
                        
  
                                            
  
                                                                        
                                                                          
                                                                   
                                                                
                                                                   
                                                                        
                                                                         
                                                                           
                                                                             
                                                                           
                                                       
  
                                                                      
                                                                       
                                                                        
                                                                      
                              
  
                                              
  
                                                                          
                                      
  
                                                                        
                 
  
                                                                            
                                                                        
                                                                      
                                                      
  
                                                                      
                                                                        
                        
  
                                                                       
                                                                      
                                                                        
                                                                           
                                                                        
                                                             
  
                                                              
                                                                
                                                                   
                                                                     
                                                                   
                                                                     
                                                                       
                          
  
                                                                           
                                                                           
                                                                         
                                                                          
                                                                          
                                                                         
                                            
  
                                                                    
                                                                    
                                                                          
                                                                         
                                                                        
                                                                           
                                                                           
  
                                                                         
                                                                    
                                                                            
                                                                          
                                                                          
                                                                
  
                                                                       
                                                                         
                                                                       
                                                                     
                                                                        
                                                                           
                                                                     
                                                                      
                                                                         
                                                                          
                                                                 
 */

/*                                                          */
typedef struct {
	/*                            */
	gxio_mpipe_context_t *context;

	/*                                       */
	uint8_t channel;

	/*                                  */
	uint8_t mac;
} gxio_mpipe_link_t;

/*                                                                     
  
                                                                          
                                                                          
                                                                       
                                                                     
                                                            
                                                                          
                                                            
                                                                      
                                                       
                                                                          
                                                                       
                                                                        
        
 */
extern int gxio_mpipe_link_enumerate_mac(int index, char *link_name,
					 uint8_t *mac_addr);

/*                    
  
                                                                           
                                                                         
                                                                          
                                                                           
                                                                         
                                                                               
  
                                                                     
                                    
                                               
                                     
                                                                            
                                                                           
  
 */
extern int gxio_mpipe_link_open(gxio_mpipe_link_t *link,
				gxio_mpipe_context_t *context,
				const char *link_name, unsigned int flags);

/*                     
  
                                                                       
                                                                        
                                                                
  
                                                                       
                                            
                                                                           
  
 */
extern int gxio_mpipe_link_close(gxio_mpipe_link_t *link);

/*                                
  
                                                        
                                           
 */
static inline int gxio_mpipe_link_channel(gxio_mpipe_link_t *link)
{
	return link->channel;
}

//                                                                 
//                                                                 
//                                                                 

/*                                                        
  
                                               
                                                             
                                                                        
         
 */
extern int gxio_mpipe_get_timestamp(gxio_mpipe_context_t *context,
				    struct timespec *ts);

/*                            
  
                                               
                                                               
                                                                        
         
 */
extern int gxio_mpipe_set_timestamp(gxio_mpipe_context_t *context,
				    const struct timespec *ts);

/*                               
  
                                               
                                                               
                                                            
                       
                                                                        
         
 */
extern int gxio_mpipe_adjust_timestamp(gxio_mpipe_context_t *context,
				       int64_t delta);

#endif /*                 */
