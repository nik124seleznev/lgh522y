#ifndef _BCACHE_H
#define _BCACHE_H

/*
                                      
  
                                                                           
  
                                                                             
                                                                               
                                                                                
                                                                         
  
                                                                            
                                                                 
  
                                                                               
                                                                              
                                                                                
                                                                               
                                                       
  
                                                                       
  
                                                                           
                                                                          
                                                                        
                                                                    
                                                 
  
                                                                              
                                                         
  
                      
  
                                                                       
                                                                           
                                                                                
      
  
                                                                                
                                                                                
                     
  
                                                                             
                                                                              
                                                                               
                                              
  
                                                                              
                                                                              
                                                                              
                                    
  
                                                                              
                                                                               
                                                                                
                                                                              
            
  
                                                                               
                                            
  
             
  
                                                   
  
                                                                    
  
                                                                                
                                                                            
                           
  
                                                                            
                                                                                
                                                                        
                            
  
                                                                                
                                            
  
                                                                           
                                                                             
             
  
                                                                              
                                          
  
                                                                              
                                                                               
                                          
  
                                                                               
                                                                               
                                                                                
                                
  
                                                                    
                                                                              
                                                                             
                                                    
  
                                                                            
                                                                                
                             
  
               
  
                                                                               
                                                                       
  
                                                                                
                                                                           
                                                                           
                                                                       
  
                                                                              
                        
  
                                                                               
                                                                           
                                                                  
  
                                                                              
                                                                                
                                                                            
                                                                                
                                                                               
                                                                             
            
  
                                                                     
                                                                       
                      
  
                      
  
                                                                       
                                                                             
                                                                  
  
                                                                               
                                                                                
                                                                            
  
                                                                 
                                                                                
                                                                              
                                                                                
  
               
  
                                                                        
                                                                               
                                                                             
                                                                       
               
  
                                                                                
                                                                            
                                                                                
                                                                               
                                                                               
                                                                               
                                                                          
  
                                                                                
                                                                                
                                                                            
                    
  
                                                                           
                                                                             
                                                                               
                                                                             
 */

#define pr_fmt(fmt) "bcache: %s() " fmt "\n", __func__

#include <linux/bio.h>
#include <linux/blktrace_api.h>
#include <linux/kobject.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/rbtree.h>
#include <linux/rwsem.h>
#include <linux/types.h>
#include <linux/workqueue.h>

#include "util.h"
#include "closure.h"

struct bucket {
	atomic_t	pin;
	uint16_t	prio;
	uint8_t		gen;
	uint8_t		disk_gen;
	uint8_t		last_gc; /*                                   */
	uint8_t		gc_gen;
	uint16_t	gc_mark;
};

/*
                                                                              
                                                          
 */

BITMASK(GC_MARK,	 struct bucket, gc_mark, 0, 2);
#define GC_MARK_RECLAIMABLE	0
#define GC_MARK_DIRTY		1
#define GC_MARK_METADATA	2
BITMASK(GC_SECTORS_USED, struct bucket, gc_mark, 2, 14);

struct bkey {
	uint64_t	high;
	uint64_t	low;
	uint64_t	ptr[];
};

/*                                  */
#define BKEY_PAD		8

#define BKEY_PADDED(key)					\
	union { struct bkey key; uint64_t key ## _pad[BKEY_PAD]; }

/*                        
                            
                                                   
                                               
                                             
 */
#define BCACHE_SB_VERSION_CDEV			0
#define BCACHE_SB_VERSION_BDEV			1
#define BCACHE_SB_VERSION_CDEV_WITH_UUID	3
#define BCACHE_SB_VERSION_BDEV_WITH_OFFSET	4
#define BCACHE_SB_MAX_VERSION			4

#define SB_SECTOR		8
#define SB_SIZE			4096
#define SB_LABEL_SIZE		32
#define SB_JOURNAL_BUCKETS	256U
/*                                                       */
#define MAX_CACHES_PER_SET	8

#define BDEV_DATA_START_DEFAULT	16	/*         */

struct cache_sb {
	uint64_t		csum;
	uint64_t		offset;	/*                                  */
	uint64_t		version;

	uint8_t			magic[16];

	uint8_t			uuid[16];
	union {
		uint8_t		set_uuid[16];
		uint64_t	set_magic;
	};
	uint8_t			label[SB_LABEL_SIZE];

	uint64_t		flags;
	uint64_t		seq;
	uint64_t		pad[8];

	union {
	struct {
		/*               */
		uint64_t	nbuckets;	/*             */

		uint16_t	block_size;	/*         */
		uint16_t	bucket_size;	/*         */

		uint16_t	nr_in_set;
		uint16_t	nr_this_dev;
	};
	struct {
		/*                 */
		uint64_t	data_offset;

		/*
                                                              
                                                             
                                                      
   */
	};
	};

	uint32_t		last_mount;	/*        */

	uint16_t		first_bucket;
	union {
		uint16_t	njournal_buckets;
		uint16_t	keys;
	};
	uint64_t		d[SB_JOURNAL_BUCKETS];	/*                 */
};

BITMASK(CACHE_SYNC,		struct cache_sb, flags, 0, 1);
BITMASK(CACHE_DISCARD,		struct cache_sb, flags, 1, 1);
BITMASK(CACHE_REPLACEMENT,	struct cache_sb, flags, 2, 3);
#define CACHE_REPLACEMENT_LRU	0U
#define CACHE_REPLACEMENT_FIFO	1U
#define CACHE_REPLACEMENT_RANDOM 2U

BITMASK(BDEV_CACHE_MODE,	struct cache_sb, flags, 0, 4);
#define CACHE_MODE_WRITETHROUGH	0U
#define CACHE_MODE_WRITEBACK	1U
#define CACHE_MODE_WRITEAROUND	2U
#define CACHE_MODE_NONE		3U
BITMASK(BDEV_STATE,		struct cache_sb, flags, 61, 2);
#define BDEV_STATE_NONE		0U
#define BDEV_STATE_CLEAN	1U
#define BDEV_STATE_DIRTY	2U
#define BDEV_STATE_STALE	3U

/*                                                 
 */
#define BCACHE_BSET_VERSION	1

/*
                                                                              
                                                
 */
struct bset {
	uint64_t		csum;
	uint64_t		magic;
	uint64_t		seq;
	uint32_t		version;
	uint32_t		keys;

	union {
		struct bkey	start[0];
		uint64_t	d[0];
	};
};

/*
                                                                             
        
 */
struct prio_set {
	uint64_t		csum;
	uint64_t		magic;
	uint64_t		seq;
	uint32_t		version;
	uint32_t		pad;

	uint64_t		next_bucket;

	struct bucket_disk {
		uint16_t	prio;
		uint8_t		gen;
	} __attribute((packed)) data[];
};

struct uuid_entry {
	union {
		struct {
			uint8_t		uuid[16];
			uint8_t		label[32];
			uint32_t	first_reg;
			uint32_t	last_reg;
			uint32_t	invalidated;

			uint32_t	flags;
			/*                            */
			uint64_t	sectors;
		};

		uint8_t	pad[128];
	};
};

BITMASK(UUID_FLASH_ONLY,	struct uuid_entry, flags, 0, 1);

#include "journal.h"
#include "stats.h"
struct search;
struct btree;
struct keybuf;

struct keybuf_key {
	struct rb_node		node;
	BKEY_PADDED(key);
	void			*private;
};

typedef bool (keybuf_pred_fn)(struct keybuf *, struct bkey *);

struct keybuf {
	keybuf_pred_fn		*key_predicate;

	struct bkey		last_scanned;
	spinlock_t		lock;

	/*
                                                                      
                                                                       
         
  */
	struct bkey		start;
	struct bkey		end;

	struct rb_root		keys;

#define KEYBUF_NR		100
	DECLARE_ARRAY_ALLOCATOR(struct keybuf_key, freelist, KEYBUF_NR);
};

struct bio_split_pool {
	struct bio_set		*bio_split;
	mempool_t		*bio_split_hook;
};

struct bio_split_hook {
	struct closure		cl;
	struct bio_split_pool	*p;
	struct bio		*bio;
	bio_end_io_t		*bi_end_io;
	void			*bi_private;
};

struct bcache_device {
	struct closure		cl;

	struct kobject		kobj;

	struct cache_set	*c;
	unsigned		id;
#define BCACHEDEVNAME_SIZE	12
	char			name[BCACHEDEVNAME_SIZE];

	struct gendisk		*disk;

	/*                           */
	atomic_t		closing;

	/*                                                          */
	atomic_t		detaching;
	int			flush_done;

	atomic_long_t		sectors_dirty;
	unsigned long		sectors_dirty_gc;
	unsigned long		sectors_dirty_last;
	long			sectors_dirty_derivative;

	mempool_t		*unaligned_bvec;
	struct bio_set		*bio_split;

	unsigned		data_csum:1;

	int (*cache_miss)(struct btree *, struct search *,
			  struct bio *, unsigned);
	int (*ioctl) (struct bcache_device *, fmode_t, unsigned, unsigned long);

	struct bio_split_pool	bio_split_hook;
};

struct io {
	/*                                                  */
	struct hlist_node	hash;
	struct list_head	lru;

	unsigned long		jiffies;
	unsigned		sequential;
	sector_t		last;
};

struct cached_dev {
	struct list_head	list;
	struct bcache_device	disk;
	struct block_device	*bdev;

	struct cache_sb		sb;
	struct bio		sb_bio;
	struct bio_vec		sb_bv[1];
	struct closure_with_waitlist sb_write;

	/*                                                               */
	atomic_t		count;
	struct work_struct	detach;

	/*
                                                                      
                  
  */
	atomic_t		running;

	/*
                                                                      
                                                          
  */
	struct rw_semaphore	writeback_lock;

	/*
                                                                        
                                                                
                                                   
  */
	atomic_t		has_dirty;

	struct bch_ratelimit	writeback_rate;
	struct delayed_work	writeback_rate_update;

	/*
                                                                     
                  
  */
	sector_t		last_read;

	/*                                          */
	struct semaphore	in_flight;
	struct closure_with_timer writeback;

	struct keybuf		writeback_keys;

	/*                            */
#define RECENT_IO_BITS	7
#define RECENT_IO	(1 << RECENT_IO_BITS)
	struct io		io[RECENT_IO];
	struct hlist_head	io_hash[RECENT_IO + 1];
	struct list_head	io_lru;
	spinlock_t		io_lock;

	struct cache_accounting	accounting;

	/*                                        */
	unsigned		sequential_cutoff;
	unsigned		readahead;

	unsigned		sequential_merge:1;
	unsigned		verify:1;

	unsigned		writeback_metadata:1;
	unsigned		writeback_running:1;
	unsigned char		writeback_percent;
	unsigned		writeback_delay;

	int			writeback_rate_change;
	int64_t			writeback_rate_derivative;
	uint64_t		writeback_rate_target;

	unsigned		writeback_rate_update_seconds;
	unsigned		writeback_rate_d_term;
	unsigned		writeback_rate_p_term_inverse;
	unsigned		writeback_rate_d_smooth;
};

enum alloc_watermarks {
	WATERMARK_PRIO,
	WATERMARK_METADATA,
	WATERMARK_MOVINGGC,
	WATERMARK_NONE,
	WATERMARK_MAX
};

struct cache {
	struct cache_set	*set;
	struct cache_sb		sb;
	struct bio		sb_bio;
	struct bio_vec		sb_bv[1];

	struct kobject		kobj;
	struct block_device	*bdev;

	unsigned		watermark[WATERMARK_MAX];

	struct closure		alloc;
	struct workqueue_struct	*alloc_workqueue;

	struct closure		prio;
	struct prio_set		*disk_buckets;

	/*
                                                                        
                                                                   
                                                                       
                                                                   
                                      
  */
	uint64_t		*prio_buckets;
	uint64_t		*prio_last_buckets;

	/*
                                           
   
                                                                      
                                                                     
                                                                      
                                                                         
                   
   
                                                                  
                                                                    
                                                                   
  */
	DECLARE_FIFO(long, free);
	DECLARE_FIFO(long, free_inc);
	DECLARE_FIFO(long, unused);

	size_t			fifo_last_bucket;

	/*                   */
	struct bucket		*buckets;

	DECLARE_HEAP(struct bucket *, heap);

	/*
                                                                        
                                                 
  */
	uint8_t			need_save_prio;
	unsigned		gc_move_threshold;

	/*
                                                                         
                                                                      
       
  */
	unsigned		invalidate_needs_gc:1;

	bool			discard; /*             */

	/*
                                                                         
                                                                      
                                                                        
                            
  */
	atomic_t		discards_in_flight;
	struct list_head	discards;

	struct journal_device	journal;

	/*                                        */
#define IO_ERROR_SHIFT		20
	atomic_t		io_errors;
	atomic_t		io_count;

	atomic_long_t		meta_sectors_written;
	atomic_long_t		btree_sectors_written;
	atomic_long_t		sectors_written;

	struct bio_split_pool	bio_split_hook;
};

struct gc_stat {
	size_t			nodes;
	size_t			key_bytes;

	size_t			nkeys;
	uint64_t		data;	/*         */
	uint64_t		dirty;	/*         */
	unsigned		in_use; /*         */
};

/*
                                                                             
  
                                                                              
                                                                              
                                 
  
                                                                                
                                                                              
                        
  
                                                                               
                         
 */
#define CACHE_SET_UNREGISTERING		0
#define	CACHE_SET_STOPPING		1
#define	CACHE_SET_STOPPING_2		2

struct cache_set {
	struct closure		cl;

	struct list_head	list;
	struct kobject		kobj;
	struct kobject		internal;
	struct dentry		*debug;
	struct cache_accounting accounting;

	unsigned long		flags;

	struct cache_sb		sb;

	struct cache		*cache[MAX_CACHES_PER_SET];
	struct cache		*cache_by_alloc[MAX_CACHES_PER_SET];
	int			caches_loaded;

	struct bcache_device	**devices;
	struct list_head	cached_devs;
	uint64_t		cached_dev_sectors;
	struct closure		caching;

	struct closure_with_waitlist sb_write;

	mempool_t		*search;
	mempool_t		*bio_meta;
	struct bio_set		*bio_split;

	/*                     */
	struct shrinker		shrink;

	/*                          */
	wait_queue_head_t	alloc_wait;

	/*                                                     */
	struct mutex		bucket_lock;

	/*                               */
	unsigned short		bucket_bits;

	/*                              */
	unsigned short		block_bits;

	/*
                                                                     
               
  */
	unsigned		btree_pages;

	/*
                                                                        
                                                                      
   
                                                                        
                                                                      
                                                                   
                                                                       
                                                               
                        
   
                                                                         
                                                                       
                                                                     
                                                  
  */
	struct list_head	btree_cache;
	struct list_head	btree_cache_freeable;
	struct list_head	btree_cache_freed;

	/*                                                                */
	unsigned		bucket_cache_used;

	/*
                                                               
                                                                        
                                                                         
                                                                
                                                                     
                                                         
  */
	struct closure		*try_harder;
	struct closure_waitlist	try_wait;
	uint64_t		try_harder_start;

	/*
                                                                     
                                                                 
                                                                      
                                                                        
                                                          
   
                                                                      
            
  */
	atomic_t		prio_blocked;
	struct closure_waitlist	bucket_wait;

	/*
                                                                    
                                                                 
  */
	atomic_t		rescale;
	/*
                                                                       
                                                                      
                                                                     
                           
  */
	uint16_t		min_prio;

	/*
                                                                         
                                      
  */
	uint8_t			need_gc;
	struct gc_stat		gc_stats;
	size_t			nbuckets;

	struct closure_with_waitlist gc;
	/*                                    */
	struct bkey		gc_done;

	/*
                                                                         
                                                                 
  */
	int			gc_mark_valid;

	/*                                                           */
	atomic_t		sectors_to_gc;

	struct closure		moving_gc;
	struct closure_waitlist	moving_gc_wait;
	struct keybuf		moving_gc_keys;
	/*                                    */
	atomic_t		in_flight;

	struct btree		*root;

#ifdef CONFIG_BCACHE_DEBUG
	struct btree		*verify_data;
	struct mutex		verify_lock;
#endif

	unsigned		nr_uuids;
	struct uuid_entry	*uuids;
	BKEY_PADDED(uuid_bucket);
	struct closure_with_waitlist uuid_write;

	/*
                                                                         
                                                                         
  */
	struct mutex		fill_lock;
	struct btree_iter	*fill_iter;

	/*
                                                                      
                   
  */
	struct mutex		sort_lock;
	struct bset		*sort;

	/*                                                 */
	struct list_head	data_buckets;
	spinlock_t		data_bucket_lock;

	struct journal		journal;

#define CONGESTED_MAX		1024
	unsigned		congested_last_us;
	atomic_t		congested;

	/*                                        */
	unsigned		congested_read_threshold_us;
	unsigned		congested_write_threshold_us;

	spinlock_t		sort_time_lock;
	struct time_stats	sort_time;
	struct time_stats	btree_gc_time;
	struct time_stats	btree_split_time;
	spinlock_t		btree_read_time_lock;
	struct time_stats	btree_read_time;
	struct time_stats	try_harder_time;

	atomic_long_t		cache_read_races;
	atomic_long_t		writeback_keys_done;
	atomic_long_t		writeback_keys_failed;
	unsigned		error_limit;
	unsigned		error_decay;
	unsigned short		journal_delay_ms;
	unsigned		verify:1;
	unsigned		key_merging_disabled:1;
	unsigned		gc_always_rewrite:1;
	unsigned		shrinker_disabled:1;
	unsigned		copy_gc_enabled:1;

#define BUCKET_HASH_BITS	12
	struct hlist_head	bucket_hash[1 << BUCKET_HASH_BITS];
};

static inline bool key_merging_disabled(struct cache_set *c)
{
#ifdef CONFIG_BCACHE_DEBUG
	return c->key_merging_disabled;
#else
	return 0;
#endif
}

static inline bool SB_IS_BDEV(const struct cache_sb *sb)
{
	return sb->version == BCACHE_SB_VERSION_BDEV
		|| sb->version == BCACHE_SB_VERSION_BDEV_WITH_OFFSET;
}

struct bbio {
	unsigned		submit_time_us;
	union {
		struct bkey	key;
		uint64_t	_pad[3];
		/*
                                                                  
                                                              
   */
	};
	struct bio		bio;
};

static inline unsigned local_clock_us(void)
{
	return local_clock() >> 10;
}

#define MAX_BSETS		4U

#define BTREE_PRIO		USHRT_MAX
#define INITIAL_PRIO		32768

#define btree_bytes(c)		((c)->btree_pages * PAGE_SIZE)
#define btree_blocks(b)							\
	((unsigned) (KEY_SIZE(&b->key) >> (b)->c->block_bits))

#define btree_default_blocks(c)						\
	((unsigned) ((PAGE_SECTORS * (c)->btree_pages) >> (c)->block_bits))

#define bucket_pages(c)		((c)->sb.bucket_size / PAGE_SECTORS)
#define bucket_bytes(c)		((c)->sb.bucket_size << 9)
#define block_bytes(c)		((c)->sb.block_size << 9)

#define __set_bytes(i, k)	(sizeof(*(i)) + (k) * sizeof(uint64_t))
#define set_bytes(i)		__set_bytes(i, i->keys)

#define __set_blocks(i, k, c)	DIV_ROUND_UP(__set_bytes(i, k), block_bytes(c))
#define set_blocks(i, c)	__set_blocks(i, (i)->keys, c)

#define node(i, j)		((struct bkey *) ((i)->d + (j)))
#define end(i)			node(i, (i)->keys)

#define index(i, b)							\
	((size_t) (((void *) i - (void *) (b)->sets[0].data) /		\
		   block_bytes(b->c)))

#define btree_data_space(b)	(PAGE_SIZE << (b)->page_order)

#define prios_per_bucket(c)				\
	((bucket_bytes(c) - sizeof(struct prio_set)) /	\
	 sizeof(struct bucket_disk))
#define prio_buckets(c)					\
	DIV_ROUND_UP((size_t) (c)->sb.nbuckets, prios_per_bucket(c))

#define JSET_MAGIC		0x245235c1a3625032ULL
#define PSET_MAGIC		0x6750e15f87337f91ULL
#define BSET_MAGIC		0x90135c78b99e07f5ULL

#define jset_magic(c)		((c)->sb.set_magic ^ JSET_MAGIC)
#define pset_magic(c)		((c)->sb.set_magic ^ PSET_MAGIC)
#define bset_magic(c)		((c)->sb.set_magic ^ BSET_MAGIC)

/*                                       */

#define KEY_FIELD(name, field, offset, size)				\
	BITMASK(name, struct bkey, field, offset, size)

#define PTR_FIELD(name, offset, size)					\
	static inline uint64_t name(const struct bkey *k, unsigned i)	\
	{ return (k->ptr[i] >> offset) & ~(((uint64_t) ~0) << size); }	\
									\
	static inline void SET_##name(struct bkey *k, unsigned i, uint64_t v)\
	{								\
		k->ptr[i] &= ~(~((uint64_t) ~0 << size) << offset);	\
		k->ptr[i] |= v << offset;				\
	}

KEY_FIELD(KEY_PTRS,	high, 60, 3)
KEY_FIELD(HEADER_SIZE,	high, 58, 2)
KEY_FIELD(KEY_CSUM,	high, 56, 2)
KEY_FIELD(KEY_PINNED,	high, 55, 1)
KEY_FIELD(KEY_DIRTY,	high, 36, 1)

KEY_FIELD(KEY_SIZE,	high, 20, 16)
KEY_FIELD(KEY_INODE,	high, 0,  20)

/*                                                                      */

static inline uint64_t KEY_OFFSET(const struct bkey *k)
{
	return k->low;
}

static inline void SET_KEY_OFFSET(struct bkey *k, uint64_t v)
{
	k->low = v;
}

PTR_FIELD(PTR_DEV,		51, 12)
PTR_FIELD(PTR_OFFSET,		8,  43)
PTR_FIELD(PTR_GEN,		0,  8)

#define PTR_CHECK_DEV		((1 << 12) - 1)

#define PTR(gen, offset, dev)						\
	((((uint64_t) dev) << 51) | ((uint64_t) offset) << 8 | gen)

static inline size_t sector_to_bucket(struct cache_set *c, sector_t s)
{
	return s >> c->bucket_bits;
}

static inline sector_t bucket_to_sector(struct cache_set *c, size_t b)
{
	return ((sector_t) b) << c->bucket_bits;
}

static inline sector_t bucket_remainder(struct cache_set *c, sector_t s)
{
	return s & (c->sb.bucket_size - 1);
}

static inline struct cache *PTR_CACHE(struct cache_set *c,
				      const struct bkey *k,
				      unsigned ptr)
{
	return c->cache[PTR_DEV(k, ptr)];
}

static inline size_t PTR_BUCKET_NR(struct cache_set *c,
				   const struct bkey *k,
				   unsigned ptr)
{
	return sector_to_bucket(c, PTR_OFFSET(k, ptr));
}

static inline struct bucket *PTR_BUCKET(struct cache_set *c,
					const struct bkey *k,
					unsigned ptr)
{
	return PTR_CACHE(c, k, ptr)->buckets + PTR_BUCKET_NR(c, k, ptr);
}

/*                  */

/*
                                                                      
                                                                     
                                      
 */
#define KEY(dev, sector, len)						\
((struct bkey) {							\
	.high = (1ULL << 63) | ((uint64_t) (len) << 20) | (dev),	\
	.low = (sector)							\
})

static inline void bkey_init(struct bkey *k)
{
	*k = KEY(0, 0, 0);
}

#define KEY_START(k)		(KEY_OFFSET(k) - KEY_SIZE(k))
#define START_KEY(k)		KEY(KEY_INODE(k), KEY_START(k), 0)
#define MAX_KEY			KEY(~(~0 << 20), ((uint64_t) ~0) >> 1, 0)
#define ZERO_KEY		KEY(0, 0, 0)

/*
                                                                               
                                                                    
 */
#define csum_set(i)							\
	bch_crc64(((void *) (i)) + sizeof(uint64_t),			\
	      ((void *) end(i)) - (((void *) (i)) + sizeof(uint64_t)))

/*                       */

#define btree_bug(b, ...)						\
do {									\
	if (bch_cache_set_error((b)->c, __VA_ARGS__))			\
		dump_stack();						\
} while (0)

#define cache_bug(c, ...)						\
do {									\
	if (bch_cache_set_error(c, __VA_ARGS__))			\
		dump_stack();						\
} while (0)

#define btree_bug_on(cond, b, ...)					\
do {									\
	if (cond)							\
		btree_bug(b, __VA_ARGS__);				\
} while (0)

#define cache_bug_on(cond, c, ...)					\
do {									\
	if (cond)							\
		cache_bug(c, __VA_ARGS__);				\
} while (0)

#define cache_set_err_on(cond, c, ...)					\
do {									\
	if (cond)							\
		bch_cache_set_error(c, __VA_ARGS__);			\
} while (0)

/*                */

#define for_each_cache(ca, cs, iter)					\
	for (iter = 0; ca = cs->cache[iter], iter < (cs)->sb.nr_in_set; iter++)

#define for_each_bucket(b, ca)						\
	for (b = (ca)->buckets + (ca)->sb.first_bucket;			\
	     b < (ca)->buckets + (ca)->sb.nbuckets; b++)

static inline void __bkey_put(struct cache_set *c, struct bkey *k)
{
	unsigned i;

	for (i = 0; i < KEY_PTRS(k); i++)
		atomic_dec_bug(&PTR_BUCKET(c, k, i)->pin);
}

/*                 */

#define blktrace_msg(c, fmt, ...)					\
do {									\
	struct request_queue *q = bdev_get_queue(c->bdev);		\
	if (q)								\
		blk_add_trace_msg(q, fmt, ##__VA_ARGS__);		\
} while (0)

#define blktrace_msg_all(s, fmt, ...)					\
do {									\
	struct cache *_c;						\
	unsigned i;							\
	for_each_cache(_c, (s), i)					\
		blktrace_msg(_c, fmt, ##__VA_ARGS__);			\
} while (0)

static inline void cached_dev_put(struct cached_dev *dc)
{
	if (atomic_dec_and_test(&dc->count))
		schedule_work(&dc->detach);
}

static inline bool cached_dev_get(struct cached_dev *dc)
{
	if (!atomic_inc_not_zero(&dc->count))
		return false;

	/*                                         */
	smp_mb__after_atomic_inc();
	return true;
}

/*
                                                                              
                                                                         
  
                                                                               
                                                                  
 */

static inline uint8_t bucket_gc_gen(struct bucket *b)
{
	return b->gen - b->last_gc;
}

static inline uint8_t bucket_disk_gen(struct bucket *b)
{
	return b->gen - b->disk_gen;
}

#define BUCKET_GC_GEN_MAX	96U
#define BUCKET_DISK_GEN_MAX	64U

#define kobj_attribute_write(n, fn)					\
	static struct kobj_attribute ksysfs_##n = __ATTR(n, S_IWUSR, NULL, fn)

#define kobj_attribute_rw(n, show, store)				\
	static struct kobj_attribute ksysfs_##n =			\
		__ATTR(n, S_IWUSR|S_IRUSR, show, store)

/*                      */

void bch_writeback_queue(struct cached_dev *);
void bch_writeback_add(struct cached_dev *, unsigned);

void bch_count_io_errors(struct cache *, int, const char *);
void bch_bbio_count_io_errors(struct cache_set *, struct bio *,
			      int, const char *);
void bch_bbio_endio(struct cache_set *, struct bio *, int, const char *);
void bch_bbio_free(struct bio *, struct cache_set *);
struct bio *bch_bbio_alloc(struct cache_set *);

struct bio *bch_bio_split(struct bio *, int, gfp_t, struct bio_set *);
void bch_generic_make_request(struct bio *, struct bio_split_pool *);
void __bch_submit_bbio(struct bio *, struct cache_set *);
void bch_submit_bbio(struct bio *, struct cache_set *, struct bkey *, unsigned);

uint8_t bch_inc_gen(struct cache *, struct bucket *);
void bch_rescale_priorities(struct cache_set *, int);
bool bch_bucket_add_unused(struct cache *, struct bucket *);
void bch_allocator_thread(struct closure *);

long bch_bucket_alloc(struct cache *, unsigned, struct closure *);
void bch_bucket_free(struct cache_set *, struct bkey *);

int __bch_bucket_alloc_set(struct cache_set *, unsigned,
			   struct bkey *, int, struct closure *);
int bch_bucket_alloc_set(struct cache_set *, unsigned,
			 struct bkey *, int, struct closure *);

__printf(2, 3)
bool bch_cache_set_error(struct cache_set *, const char *, ...);

void bch_prio_write(struct cache *);
void bch_write_bdev_super(struct cached_dev *, struct closure *);

extern struct workqueue_struct *bcache_wq, *bch_gc_wq;
extern const char * const bch_cache_modes[];
extern struct mutex bch_register_lock;
extern struct list_head bch_cache_sets;

extern struct kobj_type bch_cached_dev_ktype;
extern struct kobj_type bch_flash_dev_ktype;
extern struct kobj_type bch_cache_set_ktype;
extern struct kobj_type bch_cache_set_internal_ktype;
extern struct kobj_type bch_cache_ktype;

void bch_cached_dev_release(struct kobject *);
void bch_flash_dev_release(struct kobject *);
void bch_cache_set_release(struct kobject *);
void bch_cache_release(struct kobject *);

int bch_uuid_write(struct cache_set *);
void bcache_write_super(struct cache_set *);

int bch_flash_dev_create(struct cache_set *c, uint64_t size);

int bch_cached_dev_attach(struct cached_dev *, struct cache_set *);
void bch_cached_dev_detach(struct cached_dev *);
void bch_cached_dev_run(struct cached_dev *);
void bcache_device_stop(struct bcache_device *);

void bch_cache_set_unregister(struct cache_set *);
void bch_cache_set_stop(struct cache_set *);

struct cache_set *bch_cache_set_alloc(struct cache_sb *);
void bch_btree_cache_free(struct cache_set *);
int bch_btree_cache_alloc(struct cache_set *);
void bch_cached_dev_writeback_init(struct cached_dev *);
void bch_moving_init_cache_set(struct cache_set *);

void bch_cache_allocator_exit(struct cache *ca);
int bch_cache_allocator_init(struct cache *ca);

void bch_debug_exit(void);
int bch_debug_init(struct kobject *);
void bch_writeback_exit(void);
int bch_writeback_init(void);
void bch_request_exit(void);
int bch_request_init(void);
void bch_btree_exit(void);
int bch_btree_init(void);

#endif /*           */
