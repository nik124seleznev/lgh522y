/*
 * Copyright (C) 2011-2012 Red Hat, Inc.
 *
 * This file is released under the GPL.
 */

#ifndef DM_BIO_PRISON_H
#define DM_BIO_PRISON_H

#include "persistent-data/dm-block-manager.h" /*                       */
#include "dm-thin-metadata.h" /*                       */

#include <linux/list.h>
#include <linux/bio.h>

/*                                                                */

/*
                                                                           
                                                                          
                                                                     
                                                   
 */
struct dm_bio_prison;

/*                                       */
struct dm_cell_key {
	int virtual;
	dm_thin_id dev;
	dm_block_t block;
};

/*
                                                                        
              
 */
struct dm_bio_prison_cell {
	struct hlist_node list;
	struct dm_cell_key key;
	struct bio *holder;
	struct bio_list bios;
};

struct dm_bio_prison *dm_bio_prison_create(unsigned nr_cells);
void dm_bio_prison_destroy(struct dm_bio_prison *prison);

/*
                                                                       
                                                                         
  
                                                                           
                                             
 */
struct dm_bio_prison_cell *dm_bio_prison_alloc_cell(struct dm_bio_prison *prison,
						    gfp_t gfp);
void dm_bio_prison_free_cell(struct dm_bio_prison *prison,
			     struct dm_bio_prison_cell *cell);

/*
                                                  
  
                                                                          
                  
 */
int dm_get_cell(struct dm_bio_prison *prison,
		struct dm_cell_key *key,
		struct dm_bio_prison_cell *cell_prealloc,
		struct dm_bio_prison_cell **cell_result);

/*
                                                                        
  
                                                                          
 */
int dm_bio_detain(struct dm_bio_prison *prison,
		  struct dm_cell_key *key,
		  struct bio *inmate,
		  struct dm_bio_prison_cell *cell_prealloc,
		  struct dm_bio_prison_cell **cell_result);

void dm_cell_release(struct dm_bio_prison *prison,
		     struct dm_bio_prison_cell *cell,
		     struct bio_list *bios);
void dm_cell_release_no_holder(struct dm_bio_prison *prison,
			       struct dm_bio_prison_cell *cell,
			       struct bio_list *inmates);
void dm_cell_error(struct dm_bio_prison *prison,
		   struct dm_bio_prison_cell *cell);

/*                                                                */

/*
                                                                           
                                                                         
                                                                          
                                                                         
 */

struct dm_deferred_set;
struct dm_deferred_entry;

struct dm_deferred_set *dm_deferred_set_create(void);
void dm_deferred_set_destroy(struct dm_deferred_set *ds);

struct dm_deferred_entry *dm_deferred_entry_inc(struct dm_deferred_set *ds);
void dm_deferred_entry_dec(struct dm_deferred_entry *entry, struct list_head *head);
int dm_deferred_set_add_work(struct dm_deferred_set *ds, struct list_head *work);

/*                                                                */

#endif
