/*
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * This file is released under the GPL.
 */

#ifndef _LINUX_DM_TRANSACTION_MANAGER_H
#define _LINUX_DM_TRANSACTION_MANAGER_H

#include "dm-block-manager.h"

struct dm_transaction_manager;
struct dm_space_map;

/*                                                                */

/*
                                                                          
                                                                         
  
                                                             
 */

void dm_tm_destroy(struct dm_transaction_manager *tm);

/*
                                                                           
                                                                      
                                                                          
                                                                      
                                                                         
                                                                              
                                                                           
             
 */
struct dm_transaction_manager *dm_tm_create_non_blocking_clone(struct dm_transaction_manager *real);

/*
                                
  
                                                                       
                                                      
  
                                                                          
                                                                         
                                                                        
                                                                          
                            
 */
int dm_tm_pre_commit(struct dm_transaction_manager *tm);
int dm_tm_commit(struct dm_transaction_manager *tm, struct dm_block *superblock);

/*
                                                                   
 */

/*
                                                                           
                                                                         
                                                                           
                                                                        
          
                                                         
 */
int dm_tm_new_block(struct dm_transaction_manager *tm,
		    struct dm_block_validator *v,
		    struct dm_block **result);

/*
                                                                            
                                                                         
                                                                    
                                                                        
                                                                        
                                                                           
                                                  
  
                                                                        
                                                                         
                 
  
                                                                       
                                
 */
int dm_tm_shadow_block(struct dm_transaction_manager *tm, dm_block_t orig,
		       struct dm_block_validator *v,
		       struct dm_block **result, int *inc_children);

/*
                                                                          
                                      
 */
int dm_tm_read_lock(struct dm_transaction_manager *tm, dm_block_t b,
		    struct dm_block_validator *v,
		    struct dm_block **result);

int dm_tm_unlock(struct dm_transaction_manager *tm, struct dm_block *b);

/*
                                                                  
 */
void dm_tm_inc(struct dm_transaction_manager *tm, dm_block_t b);

void dm_tm_dec(struct dm_transaction_manager *tm, dm_block_t b);

int dm_tm_ref(struct dm_transaction_manager *tm, dm_block_t b,
	      uint32_t *result);

struct dm_block_manager *dm_tm_get_bm(struct dm_transaction_manager *tm);

/*
                                                                         
                                                             
  
                                                                      
                                                  
  
                                                                       
                     
 */
int dm_tm_create_with_sm(struct dm_block_manager *bm, dm_block_t sb_location,
			 struct dm_transaction_manager **tm,
			 struct dm_space_map **sm);

int dm_tm_open_with_sm(struct dm_block_manager *bm, dm_block_t sb_location,
		       void *sm_root, size_t root_len,
		       struct dm_transaction_manager **tm,
		       struct dm_space_map **sm);

#endif	/*                                 */
