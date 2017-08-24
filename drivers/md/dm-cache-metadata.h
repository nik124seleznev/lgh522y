/*
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * This file is released under the GPL.
 */

#ifndef DM_CACHE_METADATA_H
#define DM_CACHE_METADATA_H

#include "dm-cache-block-types.h"
#include "dm-cache-policy-internal.h"

/*                                                                */

#define DM_CACHE_METADATA_BLOCK_SIZE 4096

/*                                */
/*
                                                    
  
                                                                      
                                                                  
 */
#define DM_CACHE_METADATA_MAX_SECTORS (255 * (1 << 14) * (DM_CACHE_METADATA_BLOCK_SIZE / (1 << SECTOR_SHIFT)))

/*
                                                         
 */
#define DM_CACHE_METADATA_MAX_SECTORS_WARNING (16 * (1024 * 1024 * 1024 >> SECTOR_SHIFT))

/*                                                                */

/*
                                       
  
                                                                        
                                                           
  
                                                                     
                                                                    
             
  
                                                                   
                                                                      
                                          
  
                                                                 
                   
 */
#define DM_CACHE_FEATURE_COMPAT_SUPP	  0UL
#define DM_CACHE_FEATURE_COMPAT_RO_SUPP	  0UL
#define DM_CACHE_FEATURE_INCOMPAT_SUPP	  0UL

/*
                                                   
                                 
 */
struct dm_cache_metadata *dm_cache_metadata_open(struct block_device *bdev,
						 sector_t data_block_size,
						 bool may_format_device,
						 size_t policy_hint_size);

void dm_cache_metadata_close(struct dm_cache_metadata *cmd);

/*
                                                                        
                                                                     
                           
 */
int dm_cache_resize(struct dm_cache_metadata *cmd, dm_cblock_t new_cache_size);
dm_cblock_t dm_cache_size(struct dm_cache_metadata *cmd);

int dm_cache_discard_bitset_resize(struct dm_cache_metadata *cmd,
				   sector_t discard_block_size,
				   dm_dblock_t new_nr_entries);

typedef int (*load_discard_fn)(void *context, sector_t discard_block_size,
			       dm_dblock_t dblock, bool discarded);
int dm_cache_load_discards(struct dm_cache_metadata *cmd,
			   load_discard_fn fn, void *context);

int dm_cache_set_discard(struct dm_cache_metadata *cmd, dm_dblock_t dblock, bool discard);

int dm_cache_remove_mapping(struct dm_cache_metadata *cmd, dm_cblock_t cblock);
int dm_cache_insert_mapping(struct dm_cache_metadata *cmd, dm_cblock_t cblock, dm_oblock_t oblock);
int dm_cache_changed_this_transaction(struct dm_cache_metadata *cmd);

typedef int (*load_mapping_fn)(void *context, dm_oblock_t oblock,
			       dm_cblock_t cblock, bool dirty,
			       uint32_t hint, bool hint_valid);
int dm_cache_load_mappings(struct dm_cache_metadata *cmd,
			   struct dm_cache_policy *policy,
			   load_mapping_fn fn,
			   void *context);

int dm_cache_set_dirty(struct dm_cache_metadata *cmd, dm_cblock_t cblock, bool dirty);

struct dm_cache_statistics {
	uint32_t read_hits;
	uint32_t read_misses;
	uint32_t write_hits;
	uint32_t write_misses;
};

void dm_cache_metadata_get_stats(struct dm_cache_metadata *cmd,
				 struct dm_cache_statistics *stats);
void dm_cache_metadata_set_stats(struct dm_cache_metadata *cmd,
				 struct dm_cache_statistics *stats);

int dm_cache_commit(struct dm_cache_metadata *cmd, bool clean_shutdown);

int dm_cache_get_free_metadata_block_count(struct dm_cache_metadata *cmd,
					   dm_block_t *result);

int dm_cache_get_metadata_dev_size(struct dm_cache_metadata *cmd,
				   dm_block_t *result);

void dm_cache_dump(struct dm_cache_metadata *cmd);

/*
                                                                         
                                                                   
                                                                          
                      
  
                                                                  
                                                                        
                                                                           
                                                                
 */

int dm_cache_begin_hints(struct dm_cache_metadata *cmd, struct dm_cache_policy *p);

/*
                                                                     
 */
int dm_cache_save_hint(struct dm_cache_metadata *cmd,
		       dm_cblock_t cblock, uint32_t hint);

/*                                                                */

#endif /*                     */
