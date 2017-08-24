#ifndef _BCACHE_BTREE_H
#define _BCACHE_BTREE_H

/*
             
  
                                                                               
                                                                             
         
  
                                                                                
                                                                                
                                                                                
                                                                             
                                                                           
  
                                                                       
                                                                             
                                                                              
                                                                                
                                                                              
                                                                  
  
                                                                          
                                                                          
                                   
  
               
  
                                                                               
                                                        
  
                                                                               
                                                                                
                                                                           
                                              
  
                                                                              
                                                                              
                                                                          
                                                                               
                                                   
  
                                                                                
                                                                              
                                                                            
                                                                                
                                                                              
                                                                      
  
            
  
                                                                                
        
  
                                                                               
                                                                     
  
                                                                                
                                                                                
                                                                                
                                                                              
  
                                                                      
                                                                               
                                                                             
                                                                                
                                                                        
            
  
                                                                              
                                                                          
  
           
  
                                                                              
                                                                             
                 
  
                                                                              
                                                                             
                                                                         
  
                                                                               
                                                                                
                                                                            
        
  
                                                                               
                                                                              
                                                                           
                                                                              
                                                                             
                      
  
                                                                                
                                                                              
                                                  
 */

#include "bset.h"
#include "debug.h"

struct btree_write {
	struct closure		*owner;
	atomic_t		*journal;

	/*                                                                     
                                                              
                                                                 
                                                                      
                                     
  */
	int			prio_blocked;
};

struct btree {
	/*                       */
	struct hlist_node	hash;

	/*                                 */
	BKEY_PADDED(key);

	/*                                                     */
	unsigned long		accessed;
	unsigned long		seq;
	struct rw_semaphore	lock;
	struct cache_set	*c;

	unsigned long		flags;
	uint16_t		written;	/*                       */
	uint8_t			level;
	uint8_t			nsets;
	uint8_t			page_order;

	/*
                                                                        
   
                                                                         
                                                                      
                                                                      
  */
	struct bset_tree	sets[MAX_BSETS];

	/*                                                   */
	struct closure_with_waitlist	io;

	/*                                                             */
	int			prio_blocked;

	struct list_head	list;
	struct delayed_work	work;

	uint64_t		io_start_time;
	struct btree_write	writes[2];
	struct bio		*bio;
};

#define BTREE_FLAG(flag)						\
static inline bool btree_node_ ## flag(struct btree *b)			\
{	return test_bit(BTREE_NODE_ ## flag, &b->flags); }		\
									\
static inline void set_btree_node_ ## flag(struct btree *b)		\
{	set_bit(BTREE_NODE_ ## flag, &b->flags); }			\

enum btree_flags {
	BTREE_NODE_read_done,
	BTREE_NODE_io_error,
	BTREE_NODE_dirty,
	BTREE_NODE_write_idx,
};

BTREE_FLAG(read_done);
BTREE_FLAG(io_error);
BTREE_FLAG(dirty);
BTREE_FLAG(write_idx);

static inline struct btree_write *btree_current_write(struct btree *b)
{
	return b->writes + btree_node_write_idx(b);
}

static inline struct btree_write *btree_prev_write(struct btree *b)
{
	return b->writes + (btree_node_write_idx(b) ^ 1);
}

static inline unsigned bset_offset(struct btree *b, struct bset *i)
{
	return (((size_t) i) - ((size_t) b->sets->data)) >> 9;
}

static inline struct bset *write_block(struct btree *b)
{
	return ((void *) b->sets[0].data) + b->written * block_bytes(b->c);
}

static inline bool bset_written(struct btree *b, struct bset_tree *t)
{
	return t->data < write_block(b);
}

static inline bool bkey_written(struct btree *b, struct bkey *k)
{
	return k < write_block(b)->start;
}

static inline void set_gc_sectors(struct cache_set *c)
{
	atomic_set(&c->sectors_to_gc, c->sb.bucket_size * c->nbuckets / 8);
}

static inline bool bch_ptr_invalid(struct btree *b, const struct bkey *k)
{
	return __bch_ptr_invalid(b->c, b->level, k);
}

static inline struct bkey *bch_btree_iter_init(struct btree *b,
					       struct btree_iter *iter,
					       struct bkey *search)
{
	return __bch_btree_iter_init(b, iter, search, b->sets);
}

/*                */

#define for_each_cached_btree(b, c, iter)				\
	for (iter = 0;							\
	     iter < ARRAY_SIZE((c)->bucket_hash);			\
	     iter++)							\
		hlist_for_each_entry_rcu((b), (c)->bucket_hash + iter, hash)

#define for_each_key_filter(b, k, iter, filter)				\
	for (bch_btree_iter_init((b), (iter), NULL);			\
	     ((k) = bch_btree_iter_next_filter((iter), b, filter));)

#define for_each_key(b, k, iter)					\
	for (bch_btree_iter_init((b), (iter), NULL);			\
	     ((k) = bch_btree_iter_next(iter));)

/*                          */

struct btree_op {
	struct closure		cl;
	struct cache_set	*c;

	/*                                     */
	atomic_t		*journal;

	/*                                   */
	struct bio		*cache_bio;

	unsigned		inode;

	uint16_t		write_prio;

	/*                                                  */
	short			lock;

	/*                      */
	enum {
		BTREE_INSERT,
		BTREE_REPLACE
	} type:8;

	unsigned		csum:1;
	unsigned		skip:1;
	unsigned		flush_journal:1;

	unsigned		insert_data_done:1;
	unsigned		lookup_done:1;
	unsigned		insert_collision:1;

	/*                                                             */

	/*                     */
	struct keylist		keys;
	BKEY_PADDED(replace);
};

void bch_btree_op_init_stack(struct btree_op *);

static inline void rw_lock(bool w, struct btree *b, int level)
{
	w ? down_write_nested(&b->lock, level + 1)
	  : down_read_nested(&b->lock, level + 1);
	if (w)
		b->seq++;
}

static inline void rw_unlock(bool w, struct btree *b)
{
#ifdef CONFIG_BCACHE_EDEBUG
	unsigned i;

	if (w &&
	    b->key.ptr[0] &&
	    btree_node_read_done(b))
		for (i = 0; i <= b->nsets; i++)
			bch_check_key_order(b, b->sets[i].data);
#endif

	if (w)
		b->seq++;
	(w ? up_write : up_read)(&b->lock);
}

#define insert_lock(s, b)	((b)->level <= (s)->lock)

/*
                                                                             
                                                                             
                                                
  
                                                                               
                                                                             
                                                                             
                                                   
 */

/* 
                                                    
                                                              
                          
                         
                                   
 */
#define btree(fn, key, b, op, ...)					\
({									\
	int _r, l = (b)->level - 1;					\
	bool _w = l <= (op)->lock;					\
	struct btree *_b = bch_btree_node_get((b)->c, key, l, op);	\
	if (!IS_ERR(_b)) {						\
		_r = bch_btree_ ## fn(_b, op, ##__VA_ARGS__);		\
		rw_unlock(_w, _b);					\
	} else								\
		_r = PTR_ERR(_b);					\
	_r;								\
})

/* 
                                                        
                                                              
                 
                                   
 */
#define btree_root(fn, c, op, ...)					\
({									\
	int _r = -EINTR;						\
	do {								\
		struct btree *_b = (c)->root;				\
		bool _w = insert_lock(op, _b);				\
		rw_lock(_w, _b, _b->level);				\
		if (_b == (c)->root &&					\
		    _w == insert_lock(op, _b))				\
			_r = bch_btree_ ## fn(_b, op, ##__VA_ARGS__);	\
		rw_unlock(_w, _b);					\
		bch_cannibalize_unlock(c, &(op)->cl);		\
	} while (_r == -EINTR);						\
									\
	_r;								\
})

static inline bool should_split(struct btree *b)
{
	struct bset *i = write_block(b);
	return b->written >= btree_blocks(b) ||
		(i->seq == b->sets[0].data->seq &&
		 b->written + __set_blocks(i, i->keys + 15, b->c)
		 > btree_blocks(b));
}

void bch_btree_read_done(struct closure *);
void bch_btree_read(struct btree *);
void bch_btree_write(struct btree *b, bool now, struct btree_op *op);

void bch_cannibalize_unlock(struct cache_set *, struct closure *);
void bch_btree_set_root(struct btree *);
struct btree *bch_btree_node_alloc(struct cache_set *, int, struct closure *);
struct btree *bch_btree_node_get(struct cache_set *, struct bkey *,
				int, struct btree_op *);

bool bch_btree_insert_keys(struct btree *, struct btree_op *);
bool bch_btree_insert_check_key(struct btree *, struct btree_op *,
				   struct bio *);
int bch_btree_insert(struct btree_op *, struct cache_set *);

int bch_btree_search_recurse(struct btree *, struct btree_op *);

void bch_queue_gc(struct cache_set *);
size_t bch_btree_gc_finish(struct cache_set *);
void bch_moving_gc(struct closure *);
int bch_btree_check(struct cache_set *, struct btree_op *);
uint8_t __bch_btree_mark_key(struct cache_set *, int, struct bkey *);

void bch_keybuf_init(struct keybuf *, keybuf_pred_fn *);
void bch_refill_keybuf(struct cache_set *, struct keybuf *, struct bkey *);
bool bch_keybuf_check_overlapping(struct keybuf *, struct bkey *,
				  struct bkey *);
void bch_keybuf_del(struct keybuf *, struct keybuf_key *);
struct keybuf_key *bch_keybuf_next(struct keybuf *);
struct keybuf_key *bch_keybuf_next_rescan(struct cache_set *,
					  struct keybuf *, struct bkey *);

#endif
