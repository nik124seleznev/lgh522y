/*
 * Copyright (C) 2007-2013 ST-Ericsson
 * License terms: GNU General Public License (GPL) version 2
 * DMA driver for COH 901 318
 * Author: Per Friden <per.friden@stericsson.com>
 */

#ifndef COH901318_H
#define COH901318_H

#define MAX_DMA_PACKET_SIZE_SHIFT 11
#define MAX_DMA_PACKET_SIZE (1 << MAX_DMA_PACKET_SIZE_SHIFT)

struct device;

struct coh901318_pool {
	spinlock_t lock;
	struct dma_pool *dmapool;
	struct device *dev;

#ifdef CONFIG_DEBUG_FS
	int debugfs_pool_counter;
#endif
};

/* 
                                                   
                                      
                                     
                                          
                                            
                                                                        
                                                                      
 */
struct coh901318_lli {
	u32 control;
	dma_addr_t src_addr;
	dma_addr_t dst_addr;
	dma_addr_t link_addr;

	void *virt_link_addr;
	dma_addr_t phy_this;
};

/* 
                                                          
                     
                   
                                        
                                     
                                           
 */
int coh901318_pool_create(struct coh901318_pool *pool,
			  struct device *dev,
			  size_t lli_nbr, size_t align);

/* 
                                                   
                     
                                           
 */
int coh901318_pool_destroy(struct coh901318_pool *pool);

/* 
                                                  
  
                     
                       
                                              
 */
struct coh901318_lli *
coh901318_lli_alloc(struct coh901318_pool *pool,
		    unsigned int len);

/* 
                                                                   
                     
                                             
 */
void coh901318_lli_free(struct coh901318_pool *pool,
			struct coh901318_lli **lli);

/* 
                                                                  
                     
                      
                    
                       
                            
                                      
                                    
                                                                   
 */
int
coh901318_lli_fill_memcpy(struct coh901318_pool *pool,
			  struct coh901318_lli *lli,
			  dma_addr_t src, unsigned int size,
			  dma_addr_t dst, u32 ctrl_chained, u32 ctrl_last);

/* 
                                                                           
                     
                      
                        
                       
                                 
                                      
                                    
                                                  
                                                                   
 */
int
coh901318_lli_fill_single(struct coh901318_pool *pool,
			  struct coh901318_lli *lli,
			  dma_addr_t buf, unsigned int size,
			  dma_addr_t dev_addr, u32 ctrl_chained, u32 ctrl_last,
			  enum dma_transfer_direction dir);

/* 
                                                                                 
                     
                      
                           
                                  
                                 
                                      
                            
                                    
                                                  
                                              
                                                                   
 */
int
coh901318_lli_fill_sg(struct coh901318_pool *pool,
		      struct coh901318_lli *lli,
		      struct scatterlist *sg, unsigned int nents,
		      dma_addr_t dev_addr, u32 ctrl_chained,
		      u32 ctrl, u32 ctrl_last,
		      enum dma_transfer_direction dir, u32 ctrl_irq_mask);

#endif /*             */
