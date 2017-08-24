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

/*
  
                                                                     
            
 */

/*
  
                                                                     
                                                               
                                                                 
                                                                 
                                                                    
                                                                    
                                                                      
            
  
                                                         
  
                                                                  
                                                                     
                                                                  
                                                                   
                                                                     
                                                                     
                                                              
                                                                  
                                                                  
                 
  
                                               
  
                                                               
                                                                 
                                                  
                                                                  
                                                                  
                                                                     
                                                            
  
                                                               
                                                                      
                                                                   
                                                                         
                                                                
                                                                    
                                                                    
                                                                    
  
                                                      
  
                                                                   
                                                                   
                                                                      
                                                              
             
  
                                                                 
                                                                     
                                                                      
                                                                   
                                                              
                                                                
                                                                  
                                                                     
                                                
  
                                                       
  
                                                                      
                                                                     
                                                                     
                                                                     
                                                                      
                                                                    
                                              
  
                                           
  
                                                                     
                                                                      
                                                                     
                                                                  
                                                         
                                                                     
                                                                   
                                                                     
                                                                     
  
                                              
  
                                                                   
                                                                   
                                                                      
                                                                     
                                                               
                                                                      
                                                    
  
                                             
                                                                     
                                                                
                                                                    
                                                                  
                                                                   
                                         
                                   
  
                                                                      
  
                                                            
  
                                                                    
  
                                                     
  
                          
  
                                                                     
  
                                                                  
                   
 */
#ifndef _GXIO_TRIO_H_
#define _GXIO_TRIO_H_

#include <linux/types.h>

#include <gxio/common.h>
#include <gxio/dma_queue.h>

#include <arch/trio_constants.h>
#include <arch/trio.h>
#include <arch/trio_pcie_intfc.h>
#include <arch/trio_pcie_rc.h>
#include <arch/trio_shm.h>
#include <hv/drv_trio_intf.h>
#include <hv/iorpc.h>

/*                                                          */
typedef struct {

	/*                                                            */
	int fd;

	/*                                                    */
	char *mmio_base_mac;

	/*                                                                   
                                            */
	char *mmio_base_pio_cfg[TILEGX_TRIO_PCIES];

#ifdef USE_SHARED_PCIE_CONFIG_REGION
	/*                                                       */
	int pio_cfg_index;
#else
	/*                                                        */
	int pio_cfg_index[TILEGX_TRIO_PCIES];
#endif

	/*                                                          */
	char *mmio_push_dma[TRIO_NUM_PUSH_DMA_RINGS];

	/*                                                          */
	char *mmio_pull_dma[TRIO_NUM_PUSH_DMA_RINGS];

	/*                       */
	unsigned int asid;

} gxio_trio_context_t;

/*                                          */
typedef TRIO_DMA_DESC_t gxio_trio_dma_desc_t;

/*                                                      */
typedef struct {

	/*                                                   */
	__gxio_dma_queue_t dma_queue;

	/*                   */
	gxio_trio_dma_desc_t *dma_descs;

	/*                                  */
	unsigned long mask_num_entries;

	/*                                      */
	unsigned int log2_num_entries;

} gxio_trio_dma_queue_t;

/*                           
  
                                                                    
                                            
  
                                                       
                                                   
 */
extern int gxio_trio_init(gxio_trio_context_t *context,
			  unsigned int trio_index);

/*                                                    */
#define GXIO_ASID_NULL -1

/*                                                                  */
typedef enum gxio_trio_order_mode_e {
	/*                                                                 */
	GXIO_TRIO_ORDER_MODE_UNORDERED =
		TRIO_MAP_MEM_SETUP__ORDER_MODE_VAL_UNORDERED,
	/*                                                                   */
	GXIO_TRIO_ORDER_MODE_STRICT =
		TRIO_MAP_MEM_SETUP__ORDER_MODE_VAL_STRICT,
	/*                                                      
                                     */
	GXIO_TRIO_ORDER_MODE_OBEY_PACKET =
		TRIO_MAP_MEM_SETUP__ORDER_MODE_VAL_REL_ORD
} gxio_trio_order_mode_t;

/*                                    
  
                                              
                                                                            
                                                                   
                                                  
                                                                      
          
                                                                 
                         
                                                              
                                                           
                                                                
                                                            
 */
extern int gxio_trio_init_memory_map(gxio_trio_context_t *context,
				     unsigned int map, void *target_mem,
				     size_t target_size, unsigned int asid,
				     unsigned int mac, uint64_t bus_address,
				     gxio_trio_order_mode_t order_mode);

/*                                                            */
enum gxio_trio_alloc_flags_e {
	GXIO_TRIO_ALLOC_FIXED = HV_TRIO_ALLOC_FIXED,
};

/*                                                            */
enum gxio_trio_mem_flags_e {
	/*                                                                */
	GXIO_TRIO_MEM_FLAG_NT_HINT = IORPC_MEM_BUFFER_FLAG_NT_HINT,

	/*                                                    */
	GXIO_TRIO_MEM_FLAG_IO_PIN = IORPC_MEM_BUFFER_FLAG_IO_PIN,
};

/*                                                           
           */
#define GXIO_TRIO_FLAG_TRAFFIC_CLASS(N) HV_TRIO_FLAG_TC(N)

/*                                                            
            */
#define GXIO_TRIO_FLAG_VFUNC(N) HV_TRIO_FLAG_VFUNC(N)

/*                                                                
                                                                   
                                                                  */

/*                                                               
                                                                     
                                                                   
                              
  
                                              
                                         
                                                                   
                               
                                                                         
                                                                        
            
 */
extern int gxio_trio_alloc_asids(gxio_trio_context_t *context,
				 unsigned int count, unsigned int first,
				 unsigned int flags);

#endif /*                 */
