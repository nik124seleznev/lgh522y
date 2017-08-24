/*
  Interval Trees
  (C) 2012  Michel Lespinasse <walken@google.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  include/linux/interval_tree_generic.h
*/

#include <linux/rbtree_augmented.h>

/*
                                           
  
                                                     
                                                           
                                             
                                                                           
                                                
                                               
                                
                                                            
  
                                                                   
                                          
 */

#define INTERVAL_TREE_DEFINE(ITSTRUCT, ITRB, ITTYPE, ITSUBTREE,		      \
			     ITSTART, ITLAST, ITSTATIC, ITPREFIX)	      \
									      \
/*                                                  */			      \
									      \
static inline ITTYPE ITPREFIX ## _compute_subtree_last(ITSTRUCT *node)	      \
{									      \
	ITTYPE max = ITLAST(node), subtree_last;			      \
	if (node->ITRB.rb_left) {					      \
		subtree_last = rb_entry(node->ITRB.rb_left,		      \
					ITSTRUCT, ITRB)->ITSUBTREE;	      \
		if (max < subtree_last)					      \
			max = subtree_last;				      \
	}								      \
	if (node->ITRB.rb_right) {					      \
		subtree_last = rb_entry(node->ITRB.rb_right,		      \
					ITSTRUCT, ITRB)->ITSUBTREE;	      \
		if (max < subtree_last)					      \
			max = subtree_last;				      \
	}								      \
	return max;							      \
}									      \
									      \
RB_DECLARE_CALLBACKS(static, ITPREFIX ## _augment, ITSTRUCT, ITRB,	      \
		     ITTYPE, ITSUBTREE, ITPREFIX ## _compute_subtree_last)    \
									      \
/*                                              */			      \
									      \
ITSTATIC void ITPREFIX ## _insert(ITSTRUCT *node, struct rb_root *root)	      \
{									      \
	struct rb_node **link = &root->rb_node, *rb_parent = NULL;	      \
	ITTYPE start = ITSTART(node), last = ITLAST(node);		      \
	ITSTRUCT *parent;						      \
									      \
	while (*link) {							      \
		rb_parent = *link;					      \
		parent = rb_entry(rb_parent, ITSTRUCT, ITRB);		      \
		if (parent->ITSUBTREE < last)				      \
			parent->ITSUBTREE = last;			      \
		if (start < ITSTART(parent))				      \
			link = &parent->ITRB.rb_left;			      \
		else							      \
			link = &parent->ITRB.rb_right;			      \
	}								      \
									      \
	node->ITSUBTREE = last;						      \
	rb_link_node(&node->ITRB, rb_parent, link);			      \
	rb_insert_augmented(&node->ITRB, root, &ITPREFIX ## _augment);	      \
}									      \
									      \
ITSTATIC void ITPREFIX ## _remove(ITSTRUCT *node, struct rb_root *root)	      \
{									      \
	rb_erase_augmented(&node->ITRB, root, &ITPREFIX ## _augment);	      \
}									      \
									      \
/*                
                                                             
                  
                                                                    
                                             
                      
                                             
 */									      \
									      \
static ITSTRUCT *							      \
ITPREFIX ## _subtree_search(ITSTRUCT *node, ITTYPE start, ITTYPE last)	      \
{									      \
	while (true) {							      \
		/*              
                                                      
                                                             
   */							      \
		if (node->ITRB.rb_left) {				      \
			ITSTRUCT *left = rb_entry(node->ITRB.rb_left,	      \
						  ITSTRUCT, ITRB);	      \
			if (start <= left->ITSUBTREE) {			      \
				/*            
                                                   
                                                   
                                                   
                                                   
                                                   
                                                   
     */					      \
				node = left;				      \
				continue;				      \
			}						      \
		}							      \
		if (ITSTART(node) <= last) {		/*       */	      \
			if (start <= ITLAST(node))	/*       */	      \
				return node;	/*                        */  \
			if (node->ITRB.rb_right) {			      \
				node = rb_entry(node->ITRB.rb_right,	      \
						ITSTRUCT, ITRB);	      \
				if (start <= node->ITSUBTREE)		      \
					continue;			      \
			}						      \
		}							      \
		return NULL;	/*          */				      \
	}								      \
}									      \
									      \
ITSTATIC ITSTRUCT *							      \
ITPREFIX ## _iter_first(struct rb_root *root, ITTYPE start, ITTYPE last)      \
{									      \
	ITSTRUCT *node;							      \
									      \
	if (!root->rb_node)						      \
		return NULL;						      \
	node = rb_entry(root->rb_node, ITSTRUCT, ITRB);			      \
	if (node->ITSUBTREE < start)					      \
		return NULL;						      \
	return ITPREFIX ## _subtree_search(node, start, last);		      \
}									      \
									      \
ITSTATIC ITSTRUCT *							      \
ITPREFIX ## _iter_next(ITSTRUCT *node, ITTYPE start, ITTYPE last)	      \
{									      \
	struct rb_node *rb = node->ITRB.rb_right, *prev;		      \
									      \
	while (true) {							      \
		/*              
                                 
                                             
                                           
                  
                                                     
   */							      \
		if (rb) {						      \
			ITSTRUCT *right = rb_entry(rb, ITSTRUCT, ITRB);	      \
			if (start <= right->ITSUBTREE)			      \
				return ITPREFIX ## _subtree_search(right,     \
								start, last); \
		}							      \
									      \
		/*                                                         */ \
		do {							      \
			rb = rb_parent(&node->ITRB);			      \
			if (!rb)					      \
				return NULL;				      \
			prev = &node->ITRB;				      \
			node = rb_entry(rb, ITSTRUCT, ITRB);		      \
			rb = node->ITRB.rb_right;			      \
		} while (prev == rb);					      \
									      \
		/*                                           */		      \
		if (last < ITSTART(node))		/*        */	      \
			return NULL;					      \
		else if (start <= ITLAST(node))		/*       */	      \
			return node;					      \
	}								      \
}
