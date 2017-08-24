/*
 * Copyright (C) 2011 STRATO AG
 * written by Arne Jansen <sensille@gmx.net>
 * Distributed under the GNU GPL license version 2.
 *
 */

#ifndef __ULIST__
#define __ULIST__

#include <linux/list.h>
#include <linux/rbtree.h>

/*
                                                                       
                                                                    
                  
                                                                 
  
                                                                
                                                                        
                                     
 */

/*
                                                              
 */
#define ULIST_SIZE 16

struct ulist_iterator {
	int i;
};

/*
                      
 */
struct ulist_node {
	u64 val;		/*                */
	u64 aux;		/*                                          */
	struct rb_node rb_node;	/*                         */
};

struct ulist {
	/*
                                     
  */
	unsigned long nnodes;

	/*
                                            
  */
	unsigned long nodes_alloced;

	/*
                                                                   
                                                                        
                                                            
  */
	struct ulist_node *nodes;

	struct rb_root root;

	/*
                                                         
  */
	struct ulist_node int_nodes[ULIST_SIZE];
};

void ulist_init(struct ulist *ulist);
void ulist_fini(struct ulist *ulist);
void ulist_reinit(struct ulist *ulist);
struct ulist *ulist_alloc(gfp_t gfp_mask);
void ulist_free(struct ulist *ulist);
int ulist_add(struct ulist *ulist, u64 val, u64 aux, gfp_t gfp_mask);
int ulist_add_merge(struct ulist *ulist, u64 val, u64 aux,
		    u64 *old_aux, gfp_t gfp_mask);
struct ulist_node *ulist_next(struct ulist *ulist,
			      struct ulist_iterator *uiter);

#define ULIST_ITER_INIT(uiter) ((uiter)->i = 0)

#endif
