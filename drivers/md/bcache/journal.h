#ifndef _BCACHE_JOURNAL_H
#define _BCACHE_JOURNAL_H

/*
               
  
                                                                           
                                                                              
                                                                          
  
                                                                        
                                                               
  
                                                                                
                                                                              
                                                                         
                                                                      
                                                                          
                                                              
  
                                                                                
                                                                            
                                                                             
                                                                                
                                                                       
                                
  
                        
  
                                                                              
                                                                             
                     
  
                                                                             
                                                                   
                                                                           
                                                                               
                  
  
                                                                           
                                                                               
                                                                            
                                                                               
                                  
  
                                                                               
                                                                              
                                                                            
                           
  
                   
  
                                                                             
                                                                                
                                                                               
                                                                              
                                                         
  
                                                                           
                                                                            
                                                                          
                                                                             
                                    
  
                      
  
                                                                           
                                                                                
                                                                          
                                                                            
            
  
                                                                            
                                                                           
                                                           
 */

#define BCACHE_JSET_VERSION_UUIDv1	1
/*                           */
#define BCACHE_JSET_VERSION_UUID	1
#define BCACHE_JSET_VERSION		1

/*
                                      
                                                                          
                   
  
                                                                            
                       
  
                                         
 */
struct jset {
	uint64_t		csum;
	uint64_t		magic;
	uint64_t		seq;
	uint32_t		version;
	uint32_t		keys;

	uint64_t		last_seq;

	BKEY_PADDED(uuid_bucket);
	BKEY_PADDED(btree_root);
	uint16_t		btree_level;
	uint16_t		pad[3];

	uint64_t		prio_bucket[MAX_CACHES_PER_SET];

	union {
		struct bkey	start[0];
		uint64_t	d[0];
	};
};

/*
                                                                            
                            
 */
struct journal_replay {
	struct list_head	list;
	atomic_t		*pin;
	struct jset		j;
};

/*
                                                                        
                                              
 */
struct journal_write {
	struct jset		*data;
#define JSET_BITS		3

	struct cache_set	*c;
	struct closure_waitlist	wait;
	bool			need_write;
};

/*                              */
struct journal {
	spinlock_t		lock;
	/*                                                */
	struct closure_waitlist	wait;
	struct closure_with_timer io;

	/*                                                                   */
	unsigned		blocks_free;
	uint64_t		seq;
	DECLARE_FIFO(atomic_t, pin);

	BKEY_PADDED(key);

	struct journal_write	w[2], *cur;
};

/*
                                                                             
                        
 */
struct journal_device {
	/*
                                                                    
                                                                        
  */
	uint64_t		seq[SB_JOURNAL_BUCKETS];

	/*                                           */
	unsigned		cur_idx;

	/*                                                               */
	unsigned		last_idx;

	/*                                     */
	unsigned		discard_idx;

#define DISCARD_READY		0
#define DISCARD_IN_FLIGHT	1
#define DISCARD_DONE		2
	/*                                               */
	atomic_t		discard_in_flight;

	struct work_struct	discard_work;
	struct bio		discard_bio;
	struct bio_vec		discard_bv;

	/*                                             */
	struct bio		bio;
	struct bio_vec		bv[8];
};

#define journal_pin_cmp(c, l, r)				\
	(fifo_idx(&(c)->journal.pin, (l)->journal) >		\
	 fifo_idx(&(c)->journal.pin, (r)->journal))

#define JOURNAL_PIN	20000

#define journal_full(j)						\
	(!(j)->blocks_free || fifo_free(&(j)->pin) <= 1)

struct closure;
struct cache_set;
struct btree_op;

void bch_journal(struct closure *);
void bch_journal_next(struct journal *);
void bch_journal_mark(struct cache_set *, struct list_head *);
void bch_journal_meta(struct cache_set *, struct closure *);
int bch_journal_read(struct cache_set *, struct list_head *,
			struct btree_op *);
int bch_journal_replay(struct cache_set *, struct list_head *,
			  struct btree_op *);

void bch_journal_free(struct cache_set *);
int bch_journal_alloc(struct cache_set *);

#endif /*                   */
