/*
 * Copyright (C) 2012 Red Hat. All rights reserved.
 *
 * This file is released under the GPL.
 */

#ifndef DM_CACHE_POLICY_H
#define DM_CACHE_POLICY_H

#include "dm-cache-block-types.h"

#include <linux/device-mapper.h>

/*                                                                */

/*                                                                      
                              
 */

/*
                                                                           
                                   
  
                                                                           
                                                                           
  
              
                                                                  
  
               
                                                             
  
              
                                                                          
                               
  
                                               
                                               
                                  
                                            
  
                  
                                                                       
                                                                           
                                                           
  
                                               
                                                                        
                
                              
                          
                                      
  
                                                                    
                                                                          
                                                                           
                                                                        
                                                                           
                                   
  
                                                                          
                                                                          
                                
 */
enum policy_operation {
	POLICY_HIT,
	POLICY_MISS,
	POLICY_NEW,
	POLICY_REPLACE
};

/*
                                                          
 */
struct policy_result {
	enum policy_operation op;
	dm_oblock_t old_oblock;	/*                */
	dm_cblock_t cblock;	/*                                        */
};

typedef int (*policy_walk_fn)(void *context, dm_cblock_t cblock,
			      dm_oblock_t oblock, uint32_t hint);

/*
                                                                           
                                                                         
                            
 */
struct dm_cache_policy {

	/*
                                                                  
          
  */

	/*
                         
  */
	void (*destroy)(struct dm_cache_policy *p);

	/*
                            
   
                                                       
   
                                                                  
                                                                    
   
                                                                   
                                                                    
                                                              
                                      
   
                                                                  
                                                                 
                                                                  
                                           
                                                   
                                                      
   
                                                        
  */
	int (*map)(struct dm_cache_policy *p, dm_oblock_t oblock,
		   bool can_block, bool can_migrate, bool discarded_oblock,
		   struct bio *bio, struct policy_result *result);

	/*
                                                                
                                                               
   
                   
   
                                                               
                                    
  */
	int (*lookup)(struct dm_cache_policy *p, dm_oblock_t oblock, dm_cblock_t *cblock);

	/*
                                                   
  */
	void (*set_dirty)(struct dm_cache_policy *p, dm_oblock_t oblock);
	void (*clear_dirty)(struct dm_cache_policy *p, dm_oblock_t oblock);

	/*
                                                                
                                                     
  */
	int (*load_mapping)(struct dm_cache_policy *p, dm_oblock_t oblock,
			    dm_cblock_t cblock, uint32_t hint, bool hint_valid);

	int (*walk_mappings)(struct dm_cache_policy *p, policy_walk_fn fn,
			     void *context);

	/*
                                                                  
                      
  */
	void (*remove_mapping)(struct dm_cache_policy *p, dm_oblock_t oblock);
	void (*force_mapping)(struct dm_cache_policy *p, dm_oblock_t current_oblock,
			      dm_oblock_t new_oblock);

	int (*writeback_work)(struct dm_cache_policy *p, dm_oblock_t *oblock, dm_cblock_t *cblock);


	/*
                          
  */
	dm_cblock_t (*residency)(struct dm_cache_policy *p);

	/*
                                                                  
                                                               
                                                                    
                                                                   
                                                               
  */
	void (*tick)(struct dm_cache_policy *p);

	/*
                  
  */
	int (*emit_config_values)(struct dm_cache_policy *p,
				  char *result, unsigned maxlen);
	int (*set_config_value)(struct dm_cache_policy *p,
				const char *key, const char *value);

	/*
                                                                  
  */
	void *private;
};

/*                                                                */

/*
                                                               
 */
#define CACHE_POLICY_NAME_SIZE 16
#define CACHE_POLICY_VERSION_SIZE 3

struct dm_cache_policy_type {
	/*                                    */
	struct list_head list;

	/*
                                                                  
                                                              
  */
	char name[CACHE_POLICY_NAME_SIZE];
	unsigned version[CACHE_POLICY_VERSION_SIZE];

	/*
                                                        
                                                               
                                   
  */
	size_t hint_size;

	struct module *owner;
	struct dm_cache_policy *(*create)(dm_cblock_t cache_size,
					  sector_t origin_size,
					  sector_t block_size);
};

int dm_cache_policy_register(struct dm_cache_policy_type *type);
void dm_cache_policy_unregister(struct dm_cache_policy_type *type);

/*                                                                */

#endif	/*                   */
