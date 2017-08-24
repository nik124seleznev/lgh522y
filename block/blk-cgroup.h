#ifndef _BLK_CGROUP_H
#define _BLK_CGROUP_H
/*
 * Common Block IO controller cgroup interface
 *
 * Based on ideas and code from CFQ, CFS and BFQ:
 * Copyright (C) 2003 Jens Axboe <axboe@kernel.dk>
 *
 * Copyright (C) 2008 Fabio Checconi <fabio@gandalf.sssup.it>
 *		      Paolo Valente <paolo.valente@unimore.it>
 *
 * Copyright (C) 2009 Vivek Goyal <vgoyal@redhat.com>
 * 	              Nauman Rafique <nauman@google.com>
 */

#include <linux/cgroup.h>
#include <linux/u64_stats_sync.h>
#include <linux/seq_file.h>
#include <linux/radix-tree.h>
#include <linux/blkdev.h>

/*                                */
#define THROTL_IOPS_MAX		UINT_MAX

/*                                              */
#define CFQ_WEIGHT_MIN		10
#define CFQ_WEIGHT_MAX		1000
#define CFQ_WEIGHT_DEFAULT	500

#ifdef CONFIG_BLK_CGROUP

enum blkg_rwstat_type {
	BLKG_RWSTAT_READ,
	BLKG_RWSTAT_WRITE,
	BLKG_RWSTAT_SYNC,
	BLKG_RWSTAT_ASYNC,

	BLKG_RWSTAT_NR,
	BLKG_RWSTAT_TOTAL = BLKG_RWSTAT_NR,
};

struct blkcg_gq;

struct blkcg {
	struct cgroup_subsys_state	css;
	spinlock_t			lock;

	struct radix_tree_root		blkg_tree;
	struct blkcg_gq			*blkg_hint;
	struct hlist_head		blkg_list;

	/*                                                           */
	uint64_t			id;

	/*                                   */
	unsigned int			cfq_weight;	/*                */
	unsigned int			cfq_leaf_weight;
};

struct blkg_stat {
	struct u64_stats_sync		syncp;
	uint64_t			cnt;
};

struct blkg_rwstat {
	struct u64_stats_sync		syncp;
	uint64_t			cnt[BLKG_RWSTAT_NR];
};

/*
                                                                        
                                                                         
                                  
  
                                                                       
                                                        
                                                                    
                                                            
  
                                                                   
                                                  
 */
struct blkg_policy_data {
	/*                                                        */
	struct blkcg_gq			*blkg;
	int				plid;

	/*                               */
	struct list_head		alloc_node;
};

/*                                                      */
struct blkcg_gq {
	/*                                         */
	struct request_queue		*q;
	struct list_head		q_node;
	struct hlist_node		blkcg_node;
	struct blkcg			*blkcg;

	/*                                                                 */
	struct blkcg_gq			*parent;

	/*                                               */
	struct request_list		rl;

	/*                 */
	int				refcnt;

	/*                                                          */
	bool				online;

	struct blkg_policy_data		*pd[BLKCG_MAX_POLS];

	struct rcu_head			rcu_head;
};

typedef void (blkcg_pol_init_pd_fn)(struct blkcg_gq *blkg);
typedef void (blkcg_pol_online_pd_fn)(struct blkcg_gq *blkg);
typedef void (blkcg_pol_offline_pd_fn)(struct blkcg_gq *blkg);
typedef void (blkcg_pol_exit_pd_fn)(struct blkcg_gq *blkg);
typedef void (blkcg_pol_reset_pd_stats_fn)(struct blkcg_gq *blkg);

struct blkcg_policy {
	int				plid;
	/*                                   */
	size_t				pd_size;
	/*                             */
	struct cftype			*cftypes;

	/*            */
	blkcg_pol_init_pd_fn		*pd_init_fn;
	blkcg_pol_online_pd_fn		*pd_online_fn;
	blkcg_pol_offline_pd_fn		*pd_offline_fn;
	blkcg_pol_exit_pd_fn		*pd_exit_fn;
	blkcg_pol_reset_pd_stats_fn	*pd_reset_stats_fn;
};

extern struct blkcg blkcg_root;

struct blkcg_gq *blkg_lookup(struct blkcg *blkcg, struct request_queue *q);
struct blkcg_gq *blkg_lookup_create(struct blkcg *blkcg,
				    struct request_queue *q);
int blkcg_init_queue(struct request_queue *q);
void blkcg_drain_queue(struct request_queue *q);
void blkcg_exit_queue(struct request_queue *q);

/*                                      */
int blkcg_policy_register(struct blkcg_policy *pol);
void blkcg_policy_unregister(struct blkcg_policy *pol);
int blkcg_activate_policy(struct request_queue *q,
			  const struct blkcg_policy *pol);
void blkcg_deactivate_policy(struct request_queue *q,
			     const struct blkcg_policy *pol);

void blkcg_print_blkgs(struct seq_file *sf, struct blkcg *blkcg,
		       u64 (*prfill)(struct seq_file *,
				     struct blkg_policy_data *, int),
		       const struct blkcg_policy *pol, int data,
		       bool show_total);
u64 __blkg_prfill_u64(struct seq_file *sf, struct blkg_policy_data *pd, u64 v);
u64 __blkg_prfill_rwstat(struct seq_file *sf, struct blkg_policy_data *pd,
			 const struct blkg_rwstat *rwstat);
u64 blkg_prfill_stat(struct seq_file *sf, struct blkg_policy_data *pd, int off);
u64 blkg_prfill_rwstat(struct seq_file *sf, struct blkg_policy_data *pd,
		       int off);

u64 blkg_stat_recursive_sum(struct blkg_policy_data *pd, int off);
struct blkg_rwstat blkg_rwstat_recursive_sum(struct blkg_policy_data *pd,
					     int off);

struct blkg_conf_ctx {
	struct gendisk			*disk;
	struct blkcg_gq			*blkg;
	u64				v;
};

int blkg_conf_prep(struct blkcg *blkcg, const struct blkcg_policy *pol,
		   const char *input, struct blkg_conf_ctx *ctx);
void blkg_conf_finish(struct blkg_conf_ctx *ctx);


static inline struct blkcg *cgroup_to_blkcg(struct cgroup *cgroup)
{
	return container_of(cgroup_subsys_state(cgroup, blkio_subsys_id),
			    struct blkcg, css);
}

static inline struct blkcg *task_blkcg(struct task_struct *tsk)
{
	return container_of(task_subsys_state(tsk, blkio_subsys_id),
			    struct blkcg, css);
}

static inline struct blkcg *bio_blkcg(struct bio *bio)
{
	if (bio && bio->bi_css)
		return container_of(bio->bi_css, struct blkcg, css);
	return task_blkcg(current);
}

/* 
                                           
                            
  
                                                             
 */
static inline struct blkcg *blkcg_parent(struct blkcg *blkcg)
{
	struct cgroup *pcg = blkcg->css.cgroup->parent;

	return pcg ? cgroup_to_blkcg(pcg) : NULL;
}

/* 
                                          
                          
                           
  
                                                                      
 */
static inline struct blkg_policy_data *blkg_to_pd(struct blkcg_gq *blkg,
						  struct blkcg_policy *pol)
{
	return blkg ? blkg->pd[pol->plid] : NULL;
}

/* 
                                                               
                                       
  
                                                                        
 */
static inline struct blkcg_gq *pd_to_blkg(struct blkg_policy_data *pd)
{
	return pd ? pd->blkg : NULL;
}

/* 
                                         
                          
                      
                                
  
                                                    
 */
static inline int blkg_path(struct blkcg_gq *blkg, char *buf, int buflen)
{
	int ret;

	ret = cgroup_path(blkg->blkcg->css.cgroup, buf, buflen);
	if (ret)
		strncpy(buf, "<unavailable>", buflen);
	return ret;
}

/* 
                                  
                     
  
                                                                     
 */
static inline void blkg_get(struct blkcg_gq *blkg)
{
	lockdep_assert_held(blkg->q->queue_lock);
	WARN_ON_ONCE(!blkg->refcnt);
	blkg->refcnt++;
}

void __blkg_release(struct blkcg_gq *blkg);

/* 
                                  
                     
  
                                           
 */
static inline void blkg_put(struct blkcg_gq *blkg)
{
	lockdep_assert_held(blkg->q->queue_lock);
	WARN_ON_ONCE(blkg->refcnt <= 0);
	if (!--blkg->refcnt)
		__blkg_release(blkg);
}

/* 
                                       
                                
                                                                           
  
                                                                        
                                                                          
                                                                     
                
 */
static inline struct request_list *blk_get_rl(struct request_queue *q,
					      struct bio *bio)
{
	struct blkcg *blkcg;
	struct blkcg_gq *blkg;

	rcu_read_lock();

	blkcg = bio_blkcg(bio);

	/*                                                          */
	if (blkcg == &blkcg_root)
		goto root_rl;

	/*
                                                                    
                                                                
                          
  */
	blkg = blkg_lookup_create(blkcg, q);
	if (unlikely(IS_ERR(blkg)))
		goto root_rl;

	blkg_get(blkg);
	rcu_read_unlock();
	return &blkg->rl;
root_rl:
	rcu_read_unlock();
	return &q->root_rl;
}

/* 
                                
                           
  
                                                                      
              
 */
static inline void blk_put_rl(struct request_list *rl)
{
	/*                               */
	if (rl->blkg && rl->blkg->blkcg != &blkcg_root)
		blkg_put(rl->blkg);
}

/* 
                                                          
                           
                           
  
                                                                     
                              
 */
static inline void blk_rq_set_rl(struct request *rq, struct request_list *rl)
{
	rq->rl = rl;
}

/* 
                                                          
                           
  
                                                 
 */
static inline struct request_list *blk_rq_rl(struct request *rq)
{
	return rq->rl;
}

struct request_list *__blk_queue_next_rl(struct request_list *rl,
					 struct request_queue *q);
/* 
                                                                               
  
                                   
 */
#define blk_queue_for_each_rl(rl, q)	\
	for ((rl) = &(q)->root_rl; (rl); (rl) = __blk_queue_next_rl((rl), (q)))

/* 
                                             
                          
                     
  
                                                                           
                 
 */
static inline void blkg_stat_add(struct blkg_stat *stat, uint64_t val)
{
	u64_stats_update_begin(&stat->syncp);
	stat->cnt += val;
	u64_stats_update_end(&stat->syncp);
}

/* 
                                                         
                           
  
                                                                        
                                                  
 */
static inline uint64_t blkg_stat_read(struct blkg_stat *stat)
{
	unsigned int start;
	uint64_t v;

	do {
		start = u64_stats_fetch_begin_bh(&stat->syncp);
		v = stat->cnt;
	} while (u64_stats_fetch_retry_bh(&stat->syncp, start));

	return v;
}

/* 
                                      
                            
 */
static inline void blkg_stat_reset(struct blkg_stat *stat)
{
	stat->cnt = 0;
}

/* 
                                                   
                                 
                    
  
                            
 */
static inline void blkg_stat_merge(struct blkg_stat *to, struct blkg_stat *from)
{
	blkg_stat_add(to, blkg_stat_read(from));
}

/* 
                                                 
                              
                                
                     
  
                                                                       
                                                                  
 */
static inline void blkg_rwstat_add(struct blkg_rwstat *rwstat,
				   int rw, uint64_t val)
{
	u64_stats_update_begin(&rwstat->syncp);

	if (rw & REQ_WRITE)
		rwstat->cnt[BLKG_RWSTAT_WRITE] += val;
	else
		rwstat->cnt[BLKG_RWSTAT_READ] += val;
	if (rw & REQ_SYNC)
		rwstat->cnt[BLKG_RWSTAT_SYNC] += val;
	else
		rwstat->cnt[BLKG_RWSTAT_ASYNC] += val;

	u64_stats_update_end(&rwstat->syncp);
}

/* 
                                                              
                               
  
                                                                          
                                                                        
                 
 */
static inline struct blkg_rwstat blkg_rwstat_read(struct blkg_rwstat *rwstat)
{
	unsigned int start;
	struct blkg_rwstat tmp;

	do {
		start = u64_stats_fetch_begin_bh(&rwstat->syncp);
		tmp = *rwstat;
	} while (u64_stats_fetch_retry_bh(&rwstat->syncp, start));

	return tmp;
}

/* 
                                                            
                               
  
                                                                          
                                                                       
             
 */
static inline uint64_t blkg_rwstat_total(struct blkg_rwstat *rwstat)
{
	struct blkg_rwstat tmp = blkg_rwstat_read(rwstat);

	return tmp.cnt[BLKG_RWSTAT_READ] + tmp.cnt[BLKG_RWSTAT_WRITE];
}

/* 
                                          
                                
 */
static inline void blkg_rwstat_reset(struct blkg_rwstat *rwstat)
{
	memset(rwstat->cnt, 0, sizeof(rwstat->cnt));
}

/* 
                                                       
                                   
                    
  
                             
 */
static inline void blkg_rwstat_merge(struct blkg_rwstat *to,
				     struct blkg_rwstat *from)
{
	struct blkg_rwstat v = blkg_rwstat_read(from);
	int i;

	u64_stats_update_begin(&to->syncp);
	for (i = 0; i < BLKG_RWSTAT_NR; i++)
		to->cnt[i] += v.cnt[i];
	u64_stats_update_end(&to->syncp);
}

#else	/*                   */

struct cgroup;
struct blkcg;

struct blkg_policy_data {
};

struct blkcg_gq {
};

struct blkcg_policy {
};

static inline struct blkcg_gq *blkg_lookup(struct blkcg *blkcg, void *key) { return NULL; }
static inline int blkcg_init_queue(struct request_queue *q) { return 0; }
static inline void blkcg_drain_queue(struct request_queue *q) { }
static inline void blkcg_exit_queue(struct request_queue *q) { }
static inline int blkcg_policy_register(struct blkcg_policy *pol) { return 0; }
static inline void blkcg_policy_unregister(struct blkcg_policy *pol) { }
static inline int blkcg_activate_policy(struct request_queue *q,
					const struct blkcg_policy *pol) { return 0; }
static inline void blkcg_deactivate_policy(struct request_queue *q,
					   const struct blkcg_policy *pol) { }

static inline struct blkcg *cgroup_to_blkcg(struct cgroup *cgroup) { return NULL; }
static inline struct blkcg *bio_blkcg(struct bio *bio) { return NULL; }

static inline struct blkg_policy_data *blkg_to_pd(struct blkcg_gq *blkg,
						  struct blkcg_policy *pol) { return NULL; }
static inline struct blkcg_gq *pd_to_blkg(struct blkg_policy_data *pd) { return NULL; }
static inline char *blkg_path(struct blkcg_gq *blkg) { return NULL; }
static inline void blkg_get(struct blkcg_gq *blkg) { }
static inline void blkg_put(struct blkcg_gq *blkg) { }

static inline struct request_list *blk_get_rl(struct request_queue *q,
					      struct bio *bio) { return &q->root_rl; }
static inline void blk_put_rl(struct request_list *rl) { }
static inline void blk_rq_set_rl(struct request *rq, struct request_list *rl) { }
static inline struct request_list *blk_rq_rl(struct request *rq) { return &rq->q->root_rl; }

#define blk_queue_for_each_rl(rl, q)	\
	for ((rl) = &(q)->root_rl; (rl); (rl) = NULL)

#endif	/*                   */
#endif	/*               */
