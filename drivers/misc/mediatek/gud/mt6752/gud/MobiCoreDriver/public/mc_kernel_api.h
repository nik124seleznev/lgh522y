/*
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
/*
                                                    
 */
#ifndef _MC_KERNEL_API_H_
#define _MC_KERNEL_API_H_

struct mc_instance;

/*
                                                                  
  
                                                                     
 */
struct mc_instance *mobicore_open(void);

/*
                                                          
                                
  
                              
 */
int mobicore_release(struct mc_instance *instance);

/*
                                                  
                                                         
                                                  
                              
                                                
  
                  
 */
int mobicore_allocate_wsm(struct mc_instance *instance,
			  unsigned long requested_size, uint32_t *handle,
			  void **virt_kernel_addr);

/*
                                                                           
                                                         
                                 
  
                  
 */
int mobicore_free_wsm(struct mc_instance *instance, uint32_t handle);

/*
                                                                          
                                                         
                                                                
                                  
                          
  
                        
 */
int mobicore_map_vmem(struct mc_instance *instance, void *addr,
		      uint32_t len, uint32_t *handle);

/*
                                                                      
                                                         
                          
  
                        
 */
int mobicore_unmap_vmem(struct mc_instance *instance, uint32_t handle);

/*
                                                      
  
                                                              
 */
bool mobicore_sleep_ready(void);


#endif /*                   */
