#ifndef __NET_FRAG_H__
#define __NET_FRAG_H__

#include <linux/percpu_counter.h>

struct netns_frags {
	int			nqueues;
	struct list_head	lru_list;
	spinlock_t		lru_lock;

	/*                                                       
                                                          
  */
	struct percpu_counter   mem ____cacheline_aligned_in_smp;

	/*         */
	int			timeout;
	int			high_thresh;
	int			low_thresh;
};

struct inet_frag_queue {
	spinlock_t		lock;
	struct timer_list	timer;      /*                              */
	struct list_head	lru_list;   /*                 */
	struct hlist_node	list;
	atomic_t		refcnt;
	struct sk_buff		*fragments; /*                            */
	struct sk_buff		*fragments_tail;
	ktime_t			stamp;
	int			len;        /*                               */
	int			meat;
	__u8			last_in;    /*                             */

#define INET_FRAG_COMPLETE	4
#define INET_FRAG_FIRST_IN	2
#define INET_FRAG_LAST_IN	1

	u16			max_size;

	struct netns_frags	*net;
};

#define INETFRAGS_HASHSZ	1024

/*          
                                                              
                                                           
                             
 */
#define INETFRAGS_MAXDEPTH		128

struct inet_frag_bucket {
	struct hlist_head	chain;
	spinlock_t		chain_lock;
};

struct inet_frags {
	struct inet_frag_bucket	hash[INETFRAGS_HASHSZ];
	/*                                                          
                                                               
                                              
  */
	rwlock_t		lock ____cacheline_aligned_in_smp;
	int			secret_interval;
	struct timer_list	secret_timer;
	u32			rnd;
	int			qsize;

	unsigned int		(*hashfn)(struct inet_frag_queue *);
	bool			(*match)(struct inet_frag_queue *q, void *arg);
	void			(*constructor)(struct inet_frag_queue *q,
						void *arg);
	void			(*destructor)(struct inet_frag_queue *);
	void			(*skb_free)(struct sk_buff *);
	void			(*frag_expire)(unsigned long data);
};

void inet_frags_init(struct inet_frags *);
void inet_frags_fini(struct inet_frags *);

void inet_frags_init_net(struct netns_frags *nf);
void inet_frags_exit_net(struct netns_frags *nf, struct inet_frags *f);

void inet_frag_kill(struct inet_frag_queue *q, struct inet_frags *f);
void inet_frag_destroy(struct inet_frag_queue *q,
				struct inet_frags *f, int *work);
int inet_frag_evictor(struct netns_frags *nf, struct inet_frags *f, bool force);
struct inet_frag_queue *inet_frag_find(struct netns_frags *nf,
		struct inet_frags *f, void *key, unsigned int hash)
	__releases(&f->lock);
void inet_frag_maybe_warn_overflow(struct inet_frag_queue *q,
				   const char *prefix);

static inline void inet_frag_put(struct inet_frag_queue *q, struct inet_frags *f)
{
	if (atomic_dec_and_test(&q->refcnt))
		inet_frag_destroy(q, f, NULL);
}

/*                            */

/*                                                                    
                                
                                            
                                                                          
 */
static unsigned int frag_percpu_counter_batch = 130000;

static inline int frag_mem_limit(struct netns_frags *nf)
{
	return percpu_counter_read(&nf->mem);
}

static inline void sub_frag_mem_limit(struct inet_frag_queue *q, int i)
{
	__percpu_counter_add(&q->net->mem, -i, frag_percpu_counter_batch);
}

static inline void add_frag_mem_limit(struct inet_frag_queue *q, int i)
{
	__percpu_counter_add(&q->net->mem, i, frag_percpu_counter_batch);
}

static inline void init_frag_mem_limit(struct netns_frags *nf)
{
	percpu_counter_init(&nf->mem, 0);
}

static inline int sum_frag_mem_limit(struct netns_frags *nf)
{
	int res;

	local_bh_disable();
	res = percpu_counter_sum_positive(&nf->mem);
	local_bh_enable();

	return res;
}

static inline void inet_frag_lru_move(struct inet_frag_queue *q)
{
	spin_lock(&q->net->lru_lock);
	if (!list_empty(&q->lru_list))
		list_move_tail(&q->lru_list, &q->net->lru_list);
	spin_unlock(&q->net->lru_lock);
}

static inline void inet_frag_lru_del(struct inet_frag_queue *q)
{
	spin_lock(&q->net->lru_lock);
	list_del_init(&q->lru_list);
	q->net->nqueues--;
	spin_unlock(&q->net->lru_lock);
}

static inline void inet_frag_lru_add(struct netns_frags *nf,
				     struct inet_frag_queue *q)
{
	spin_lock(&nf->lru_lock);
	list_add_tail(&q->lru_list, &nf->lru_list);
	q->net->nqueues++;
	spin_unlock(&nf->lru_lock);
}

/*                   
                                                                                
                                                                            
 */
#define	IPFRAG_ECN_NOT_ECT	0x01 /*                          */
#define	IPFRAG_ECN_ECT_1	0x02 /*                        */
#define	IPFRAG_ECN_ECT_0	0x04 /*                        */
#define	IPFRAG_ECN_CE		0x08 /*                     */

extern const u8 ip_frag_ecn_table[16];

#endif