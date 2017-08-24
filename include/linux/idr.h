/*
 * include/linux/idr.h
 * 
 * 2002-10-18  written by Jim Houston jim.houston@ccur.com
 *	Copyright (C) 2002 by Concurrent Computer Corporation
 *	Distributed under the GNU GPL license version 2.
 *
 * Small id to pointer translation service avoiding fixed sized
 * tables.
 */

#ifndef __IDR_H__
#define __IDR_H__

#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/init.h>
#include <linux/rcupdate.h>

/*
                                                                       
                                                                        
                                                                           
               
 */
#define IDR_BITS 8
#define IDR_SIZE (1 << IDR_BITS)
#define IDR_MASK ((1 << IDR_BITS)-1)

struct idr_layer {
	int			prefix;	/*                                 */
	DECLARE_BITMAP(bitmap, IDR_SIZE); /*                               */
	struct idr_layer __rcu	*ary[1<<IDR_BITS];
	int			count;	/*                              */
	int			layer;	/*                    */
	struct rcu_head		rcu_head;
};

struct idr {
	struct idr_layer __rcu	*hint;	/*                               */
	struct idr_layer __rcu	*top;
	struct idr_layer	*id_free;
	int			layers;	/*                                   */
	int			id_free_cnt;
	int			cur;	/*                                   */
	spinlock_t		lock;
};

#define IDR_INIT(name)							\
{									\
	.lock			= __SPIN_LOCK_UNLOCKED(name.lock),	\
}
#define DEFINE_IDR(name)	struct idr name = IDR_INIT(name)

/* 
                
                                                 
  
                                                                         
                                                                         
                                                                          
                
  
                                                                      
                                                                          
                                                                          
                                                                             
                                                                        
           
 */

/*
                          
 */

void *idr_find_slowpath(struct idr *idp, int id);
void idr_preload(gfp_t gfp_mask);
int idr_alloc(struct idr *idp, void *ptr, int start, int end, gfp_t gfp_mask);
int idr_alloc_cyclic(struct idr *idr, void *ptr, int start, int end, gfp_t gfp_mask);
int idr_for_each(struct idr *idp,
		 int (*fn)(int id, void *p, void *data), void *data);
void *idr_get_next(struct idr *idp, int *nextid);
void *idr_replace(struct idr *idp, void *ptr, int id);
void idr_remove(struct idr *idp, int id);
void idr_free(struct idr *idp, int id);
void idr_destroy(struct idr *idp);
void idr_init(struct idr *idp);

/* 
                                                                   
  
                                                                  
                                            
 */
static inline void idr_preload_end(void)
{
	preempt_enable();
}

/* 
                                         
                   
                  
  
                                                                        
                                                                
                 
  
                                                                         
                                            
 */
static inline void *idr_find(struct idr *idr, int id)
{
	struct idr_layer *hint = rcu_dereference_raw(idr->hint);

	if (hint && (id & ~IDR_MASK) == hint->prefix)
		return rcu_dereference_raw(hint->ary[id & IDR_MASK]);

	return idr_find_slowpath(idr, id);
}

/* 
                                                                      
                       
                                        
                           
  
                                                                    
                                                                      
                                         
 */
#define idr_for_each_entry(idp, entry, id)			\
	for (id = 0; ((entry) = idr_get_next(idp, &(id))) != NULL; ++id)

/*
                                                                   
                                           
 */
int __idr_pre_get(struct idr *idp, gfp_t gfp_mask);
int __idr_get_new_above(struct idr *idp, void *ptr, int starting_id, int *id);
void __idr_remove_all(struct idr *idp);

/* 
                                                     
                   
                                     
  
                                                         
                                               
 */
static inline int __deprecated idr_pre_get(struct idr *idp, gfp_t gfp_mask)
{
	return __idr_pre_get(idp, gfp_mask);
}

/* 
                                                                          
                   
                                                
                                      
                                       
  
                                                         
                                               
 */
static inline int __deprecated idr_get_new_above(struct idr *idp, void *ptr,
						 int starting_id, int *id)
{
	return __idr_get_new_above(idp, ptr, starting_id, id);
}

/* 
                                       
                   
                                                
                                       
  
                                                         
                                               
 */
static inline int __deprecated idr_get_new(struct idr *idp, void *ptr, int *id)
{
	return __idr_get_new_above(idp, ptr, 0, id);
}

/* 
                                                          
                   
  
                                                                     
                                  
 */
static inline void __deprecated idr_remove_all(struct idr *idp)
{
	__idr_remove_all(idp);
}

/*
                                                                
                           
  
                                                               
                                                                  
 */
#define IDA_CHUNK_SIZE		128	/*                     */
#define IDA_BITMAP_LONGS	(IDA_CHUNK_SIZE / sizeof(long) - 1)
#define IDA_BITMAP_BITS 	(IDA_BITMAP_LONGS * sizeof(long) * 8)

struct ida_bitmap {
	long			nr_busy;
	unsigned long		bitmap[IDA_BITMAP_LONGS];
};

struct ida {
	struct idr		idr;
	struct ida_bitmap	*free_bitmap;
};

#define IDA_INIT(name)		{ .idr = IDR_INIT((name).idr), .free_bitmap = NULL, }
#define DEFINE_IDA(name)	struct ida name = IDA_INIT(name)

int ida_pre_get(struct ida *ida, gfp_t gfp_mask);
int ida_get_new_above(struct ida *ida, int starting_id, int *p_id);
void ida_remove(struct ida *ida, int id);
void ida_destroy(struct ida *ida);
void ida_init(struct ida *ida);

int ida_simple_get(struct ida *ida, unsigned int start, unsigned int end,
		   gfp_t gfp_mask);
void ida_simple_remove(struct ida *ida, unsigned int id);

/* 
                                
                   
                                         
  
                                                                     
 */
static inline int ida_get_new(struct ida *ida, int *p_id)
{
	return ida_get_new_above(ida, 0, p_id);
}

void __init idr_init_cache(void);

#endif /*           */
