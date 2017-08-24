/*
 * drivers/staging/android/ion/ion.h
 *
 * Copyright (C) 2011 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _LINUX_ION_H
#define _LINUX_ION_H

#include <linux/types.h>
struct ion_handle_debug;

#include "../uapi/ion.h"

struct ion_handle;
struct ion_device;
struct ion_heap;
struct ion_mapper;
struct ion_client;
struct ion_buffer;

/*                                                             
                                                                     
                                                                          
                                                  */
#define ion_phys_addr_t unsigned long

/* 
                                                                  
                                                  
                                                                    
                                                                 
                                                                
                                 
                                                               
                                                 
                                                              
                                                 
  
                              
 */
struct ion_platform_heap {
	enum ion_heap_type type;
	unsigned int id;
	const char *name;
	ion_phys_addr_t base;
	size_t size;
	ion_phys_addr_t align;
	void *priv;
};

/* 
                                                                            
                                          
                                            
  
                                                                                
 */
struct ion_platform_data {
	int nr;
	struct ion_platform_heap *heaps;
};

/* 
                                                             
                                                                
        
  
                                                                
                                                               
                        
 */
void ion_reserve(struct ion_platform_data *data);

/* 
                                                          
                               
                                                               
                             
 */
struct ion_client *ion_client_create(struct ion_device *dev,
				     const char *name);

/* 
                                                               
                      
  
                                                            
                             
 */
void ion_client_destroy(struct ion_client *client);

/* 
                                  
                       
                                
                                                                   
                                             
                                                                          
                                                        
       
                                                                
                                                          
                       
  
                                                                       
                          
 */
struct ion_handle *ion_alloc(struct ion_client *client, size_t len,
			     size_t align, unsigned int heap_id_mask,
			     unsigned int flags);

/* 
                           
                      
                              
  
                            
 */
void ion_free(struct ion_client *client, struct ion_handle *handle);

/* 
                                                              
                      
                      
                                         
                                       
  
                                                                    
                                                             
                                                                
                                                                    
                                                                
                                                             
                                                           
                       
 */
int ion_phys(struct ion_client *client, struct ion_handle *handle,
	     ion_phys_addr_t *addr, size_t *len);

/* 
                                                       
                      
                      
  
                                                
                           
 */
struct sg_table *ion_sg_table(struct ion_client *client,
			      struct ion_handle *handle);

/* 
                                                       
                      
                         
  
                                                                        
                                      
 */
void *ion_map_kernel(struct ion_client *client, struct ion_handle *handle);

/* 
                                                             
                      
                           
 */
void ion_unmap_kernel(struct ion_client *client, struct ion_handle *handle);

/* 
                                                
                      
                      
 */
struct dma_buf *ion_share_dma_buf(struct ion_client *client,
						struct ion_handle *handle);

/* 
                                                                    
                      
                      
 */
int ion_share_dma_buf_fd(struct ion_client *client, struct ion_handle *handle);

/* 
                                                                              
                      
                       
  
                                                                            
                                                                         
                                                                           
 */
struct ion_handle *ion_import_dma_buf(struct ion_client *client, int fd);

#endif /*              */
