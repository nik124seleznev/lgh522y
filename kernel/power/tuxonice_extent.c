/*
 * kernel/power/tuxonice_extent.c
 *
 * Copyright (C) 2003-2010 Nigel Cunningham (nigel at tuxonice net)
 *
 * Distributed under GPLv2.
 *
 * These functions encapsulate the manipulation of storage metadata.
 */

#include <linux/suspend.h>
#include "tuxonice_modules.h"
#include "tuxonice_extent.h"
#include "tuxonice_alloc.h"
#include "tuxonice_ui.h"
#include "tuxonice.h"

/* 
                                        
  
                                    
  */
static struct hibernate_extent *toi_get_extent(void)
{
	return (struct hibernate_extent *)toi_kzalloc(2,
						      sizeof(struct hibernate_extent),
						      TOI_ATOMIC_GFP);
}

/* 
                                                       
                         
  */
void toi_put_extent_chain(struct hibernate_extent_chain *chain)
{
	struct hibernate_extent *this;

	this = chain->first;

	while (this) {
		struct hibernate_extent *next = this->next;
		toi_kfree(2, this, sizeof(*this));
		chain->num_extents--;
		this = next;
	}

	chain->first = NULL;
	chain->last_touched = NULL;
	chain->current_extent = NULL;
	chain->size = 0;
}
EXPORT_SYMBOL_GPL(toi_put_extent_chain);

/* 
                                                               
                                                    
                                                     
                                                
  
                                                                   
  */
int toi_add_to_extent_chain(struct hibernate_extent_chain *chain,
			    unsigned long start, unsigned long end)
{
	struct hibernate_extent *new_ext = NULL, *cur_ext = NULL;

	toi_message(TOI_IO, TOI_VERBOSE, 0,
		    "Adding extent %lu-%lu to chain %p.\n", start, end, chain);

	/*                                   */
	if (chain->last_touched && chain->last_touched->start < start)
		cur_ext = chain->last_touched;
	else if (chain->first && chain->first->start < start)
		cur_ext = chain->first;

	if (cur_ext) {
		while (cur_ext->next && cur_ext->next->start < start)
			cur_ext = cur_ext->next;

		if (cur_ext->end == (start - 1)) {
			struct hibernate_extent *next_ext = cur_ext->next;
			cur_ext->end = end;

			/*                               */
			if (next_ext && cur_ext->end + 1 == next_ext->start) {
				cur_ext->end = next_ext->end;
				cur_ext->next = next_ext->next;
				toi_kfree(2, next_ext, sizeof(*next_ext));
				chain->num_extents--;
			}

			chain->last_touched = cur_ext;
			chain->size += (end - start + 1);

			return 0;
		}
	}

	new_ext = toi_get_extent();
	if (!new_ext) {
		printk(KERN_INFO "Error unable to append a new extent to the " "chain.\n");
		return -ENOMEM;
	}

	chain->num_extents++;
	chain->size += (end - start + 1);
	new_ext->start = start;
	new_ext->end = end;

	chain->last_touched = new_ext;

	if (cur_ext) {
		new_ext->next = cur_ext->next;
		cur_ext->next = new_ext;
	} else {
		if (chain->first)
			new_ext->next = chain->first;
		chain->first = new_ext;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(toi_add_to_extent_chain);
