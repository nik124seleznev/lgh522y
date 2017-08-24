#ifndef _BCACHE_BSET_H
#define _BCACHE_BSET_H

/*
         
  
                                                                               
                       
  
                                                                           
                 
  
                                                                          
                                                                              
                                                                      
  
                                                                               
                                                                          
                                                                               
                                                                             
           
  
                                                                       
                                                                          
                                                                              
  
                                                                                
                                                                           
                                                                   
  
         
  
                                                                               
                                                                                
                   
  
                                                                                
                                            
  
                                                                             
                                                                            
                              
  
                  
  
                                                                              
                                                                      
  
                                                                              
                                                                              
                                                                      
  
                          
  
                                                                              
                                                                              
                                                                               
                                                                 
  
                                                                               
                                                                                
                                                                          
                                           
  
                                                                            
                                                                               
                        
  
                                                                           
                                                                            
                                                                            
                                                                             
                                                                     
                                           
  
                                                                          
                                                                            
                                                                                
                             
  
                                                                                
                                                                                
                                                                           
  
                                                                              
                                                                                
                                                                                
                                                                              
                                    
  
                                                                        
                                                                               
                                                                              
                                                                    
                                                                                
                            
  
                                                                             
                                                                             
      
  
                                                                               
                                                                             
                                                                        
                                                                             
                                                               
  
                                                                                
                                                                                
                 
  
                                                                              
                                                                            
                                                                              
                                                                              
                                                                              
                                                                               
                                                     
  
                                                                          
                                                                              
                                                                             
                                                                         
  
                                                                              
                                                                               
                                                                              
                                                              
  
                                                                            
                                                                          
                                                                               
                                                                                
                                                                          
                                                                            
  
                                                                           
                                                                                
                                                                            
                                          
 */

/*                                */

struct btree_iter {
	size_t size, used;
	struct btree_iter_set {
		struct bkey *k, *end;
	} data[MAX_BSETS];
};

struct bset_tree {
	/*
                                                          
                                                               
                                                             
           
  */

	/*                                        */
	unsigned	size;

	/*                                                   */
	unsigned	extra;

	/*                                 */
	struct bkey	end;
	struct bkey_float *tree;

	/*
                                                            
                                              
   
                                                                
                                                                 
         
  */
	uint8_t		*prev;

	/*                                                         */
	struct bset	*data;
};

static __always_inline int64_t bkey_cmp(const struct bkey *l,
					const struct bkey *r)
{
	return unlikely(KEY_INODE(l) != KEY_INODE(r))
		? (int64_t) KEY_INODE(l) - (int64_t) KEY_INODE(r)
		: (int64_t) KEY_OFFSET(l) - (int64_t) KEY_OFFSET(r);
}

static inline size_t bkey_u64s(const struct bkey *k)
{
	BUG_ON(KEY_CSUM(k) > 1);
	return 2 + KEY_PTRS(k) + (KEY_CSUM(k) ? 1 : 0);
}

static inline size_t bkey_bytes(const struct bkey *k)
{
	return bkey_u64s(k) * sizeof(uint64_t);
}

static inline void bkey_copy(struct bkey *dest, const struct bkey *src)
{
	memcpy(dest, src, bkey_bytes(src));
}

static inline void bkey_copy_key(struct bkey *dest, const struct bkey *src)
{
	if (!src)
		src = &KEY(0, 0, 0);

	SET_KEY_INODE(dest, KEY_INODE(src));
	SET_KEY_OFFSET(dest, KEY_OFFSET(src));
}

static inline struct bkey *bkey_next(const struct bkey *k)
{
	uint64_t *d = (void *) k;
	return (struct bkey *) (d + bkey_u64s(k));
}

/*          */

struct keylist {
	struct bkey		*top;
	union {
		uint64_t		*list;
		struct bkey		*bottom;
	};

	/*                                                    */
#define KEYLIST_INLINE		16
	uint64_t		d[KEYLIST_INLINE];
};

static inline void bch_keylist_init(struct keylist *l)
{
	l->top = (void *) (l->list = l->d);
}

static inline void bch_keylist_push(struct keylist *l)
{
	l->top = bkey_next(l->top);
}

static inline void bch_keylist_add(struct keylist *l, struct bkey *k)
{
	bkey_copy(l->top, k);
	bch_keylist_push(l);
}

static inline bool bch_keylist_empty(struct keylist *l)
{
	return l->top == (void *) l->list;
}

static inline void bch_keylist_free(struct keylist *l)
{
	if (l->list != l->d)
		kfree(l->list);
}

void bch_keylist_copy(struct keylist *, struct keylist *);
struct bkey *bch_keylist_pop(struct keylist *);
int bch_keylist_realloc(struct keylist *, int, struct cache_set *);

void bch_bkey_copy_single_ptr(struct bkey *, const struct bkey *,
			      unsigned);
bool __bch_cut_front(const struct bkey *, struct bkey *);
bool __bch_cut_back(const struct bkey *, struct bkey *);

static inline bool bch_cut_front(const struct bkey *where, struct bkey *k)
{
	BUG_ON(bkey_cmp(where, k) > 0);
	return __bch_cut_front(where, k);
}

static inline bool bch_cut_back(const struct bkey *where, struct bkey *k)
{
	BUG_ON(bkey_cmp(where, &START_KEY(k)) < 0);
	return __bch_cut_back(where, k);
}

const char *bch_ptr_status(struct cache_set *, const struct bkey *);
bool __bch_ptr_invalid(struct cache_set *, int level, const struct bkey *);
bool bch_ptr_bad(struct btree *, const struct bkey *);

static inline uint8_t gen_after(uint8_t a, uint8_t b)
{
	uint8_t r = a - b;
	return r > 128U ? 0 : r;
}

static inline uint8_t ptr_stale(struct cache_set *c, const struct bkey *k,
				unsigned i)
{
	return gen_after(PTR_BUCKET(c, k, i)->gen, PTR_GEN(k, i));
}

static inline bool ptr_available(struct cache_set *c, const struct bkey *k,
				 unsigned i)
{
	return (PTR_DEV(k, i) < MAX_CACHES_PER_SET) && PTR_CACHE(c, k, i);
}


typedef bool (*ptr_filter_fn)(struct btree *, const struct bkey *);

struct bkey *bch_next_recurse_key(struct btree *, struct bkey *);
struct bkey *bch_btree_iter_next(struct btree_iter *);
struct bkey *bch_btree_iter_next_filter(struct btree_iter *,
					struct btree *, ptr_filter_fn);

void bch_btree_iter_push(struct btree_iter *, struct bkey *, struct bkey *);
struct bkey *__bch_btree_iter_init(struct btree *, struct btree_iter *,
				   struct bkey *, struct bset_tree *);

/*                */
#define BKEY_MID_BITS		3
#define BKEY_EXPONENT_BITS	7
#define BKEY_MANTISSA_BITS	22
#define BKEY_MANTISSA_MASK	((1 << BKEY_MANTISSA_BITS) - 1)

struct bkey_float {
	unsigned	exponent:BKEY_EXPONENT_BITS;
	unsigned	m:BKEY_MID_BITS;
	unsigned	mantissa:BKEY_MANTISSA_BITS;
} __packed;

/*
                                                                                
                                                                             
                        
  
                                                                             
                                                                              
                                                             
  
                                                                            
                                                                               
                                                                              
                                
 */

#define BSET_CACHELINE		128
#define bset_tree_space(b)	(btree_data_space(b) / BSET_CACHELINE)

#define bset_tree_bytes(b)	(bset_tree_space(b) * sizeof(struct bkey_float))
#define bset_prev_bytes(b)	(bset_tree_space(b) * sizeof(uint8_t))

void bch_bset_init_next(struct btree *);

void bch_bset_fix_invalidated_key(struct btree *, struct bkey *);
void bch_bset_fix_lookup_table(struct btree *, struct bkey *);

struct bkey *__bch_bset_search(struct btree *, struct bset_tree *,
			   const struct bkey *);

static inline struct bkey *bch_bset_search(struct btree *b, struct bset_tree *t,
					   const struct bkey *search)
{
	return search ? __bch_bset_search(b, t, search) : t->data->start;
}

bool bch_bkey_try_merge(struct btree *, struct bkey *, struct bkey *);
void bch_btree_sort_lazy(struct btree *);
void bch_btree_sort_into(struct btree *, struct btree *);
void bch_btree_sort_and_fix_extents(struct btree *, struct btree_iter *);
void bch_btree_sort_partial(struct btree *, unsigned);

static inline void bch_btree_sort(struct btree *b)
{
	bch_btree_sort_partial(b, 0);
}

int bch_bset_print_stats(struct cache_set *, char *);

#endif
