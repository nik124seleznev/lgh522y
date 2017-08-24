/*
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * This file is released under the GPL.
 */
#ifndef _LINUX_DM_ARRAY_H
#define _LINUX_DM_ARRAY_H

#include "dm-btree.h"

/*                                                                */

/*
                                                                       
                                                                          
                                                                         
                                              
  
                                                                           
                                
  
                                                                      
                                                                     
                                                                          
                                                            
  
                                                                        
                                                                         
                                        
  
               
  
                                                                     
                                                                          
                                                                         
               
  
                                                                           
                                                                        
                                                                         
                                                                    
  
                                                                       
                                                                        
                                                                   
                                                                          
                                    
  
                                             
  
                                                           
  
                                                         
  
                                                                        
                                                               
  
                                                                        
                                                                       
                                                                          
                                                 
 */

/*
                                                                         
                       
 */
struct dm_array_info {
	struct dm_transaction_manager *tm;
	struct dm_btree_value_type value_type;
	struct dm_btree_info btree_info;
};

/*
                                                                         
                                           
  
                                        
                                                                       
                                    
 */
void dm_array_info_init(struct dm_array_info *info,
			struct dm_transaction_manager *tm,
			struct dm_btree_value_type *vt);

/*
                                      
  
                             
                                                                
 */
int dm_array_empty(struct dm_array_info *info, dm_block_t *root);

/*
                     
  
                             
                                             
                                                                   
                       
                                                    
                                                                          
                                                      
  
                                                                         
                                                                          
              
 */
int dm_array_resize(struct dm_array_info *info, dm_block_t root,
		    uint32_t old_size, uint32_t new_size,
		    const void *value, dm_block_t *new_root)
	__dm_written_to_disk(value);

/*
                                                                            
                              
 */
int dm_array_del(struct dm_array_info *info, dm_block_t root);

/*
                              
  
                             
                                 
                      
                                                                      
  
                                                           
 */
int dm_array_get_value(struct dm_array_info *info, dm_block_t root,
		       uint32_t index, void *value);

/*
                             
  
                             
                                 
                      
                                                                           
                                                                      
                                
  
                                                                     
               
  
                                                           
 */
int dm_array_set_value(struct dm_array_info *info, dm_block_t root,
		       uint32_t index, const void *value, dm_block_t *new_root)
	__dm_written_to_disk(value);

/*
                                            
  
                             
                                 
                                     
                                   
 */
int dm_array_walk(struct dm_array_info *info, dm_block_t root,
		  int (*fn)(void *context, uint64_t key, void *leaf),
		  void *context);

/*                                                                */

#endif	/*                   */
