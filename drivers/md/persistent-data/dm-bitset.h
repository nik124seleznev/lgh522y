/*
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * This file is released under the GPL.
 */
#ifndef _LINUX_DM_BITSET_H
#define _LINUX_DM_BITSET_H

#include "dm-array.h"

/*                                                                */

/*
                                                                          
                                                                           
                        
  
                                                                          
                                                                         
                                                                        
                                                                          
                                                           
  
                              

               
  
                                                                       
                                                                           
                                                                       
                            
  
                                                                           
                                                                         
                                                                         
                                                                      
  
                                                                        
                                                                        
                                                                   
                                                                          
                                    
  
                                                                       
                                           
  
                                              
  
                                         
  
                                             
  
                                           
  
                                                              
  
                                                                          
                                                                       
                                                                           
                                                 
 */

/*
                                                                         
                                                  
 */
struct dm_disk_bitset {
	struct dm_array_info array_info;

	uint32_t current_index;
	uint64_t current_bits;

	bool current_index_set:1;
};

/*
                                                                          
                                           
  
                                                                    
                                         
 */
void dm_disk_bitset_init(struct dm_transaction_manager *tm,
			 struct dm_disk_bitset *info);

/*
                                       
  
                              
                                                      
 */
int dm_bitset_empty(struct dm_disk_bitset *info, dm_block_t *new_root);

/*
                     
  
                              
                                                 
                                                        
                                                                 
                                             
                                                      
 */
int dm_bitset_resize(struct dm_disk_bitset *info, dm_block_t old_root,
		     uint32_t old_nr_entries, uint32_t new_nr_entries,
		     bool default_value, dm_block_t *new_root);

/*
                    
 */
int dm_bitset_del(struct dm_disk_bitset *info, dm_block_t root);

/*
             
  
                              
                                      
                        
                                                      
  
                                                           
 */
int dm_bitset_set_bit(struct dm_disk_bitset *info, dm_block_t root,
		      uint32_t index, dm_block_t *new_root);

/*
                
  
                              
                                      
                        
                                                      
  
                                                           
 */
int dm_bitset_clear_bit(struct dm_disk_bitset *info, dm_block_t root,
			uint32_t index, dm_block_t *new_root);

/*
               
  
                              
                                      
                        
                                                                                            
                                      
  
                                                           
 */
int dm_bitset_test_bit(struct dm_disk_bitset *info, dm_block_t root,
		       uint32_t index, dm_block_t *new_root, bool *result);

/*
                                    
  
                              
                                      
                                                      
 */
int dm_bitset_flush(struct dm_disk_bitset *info, dm_block_t root,
		    dm_block_t *new_root);

/*                                                                */

#endif /*                    */
