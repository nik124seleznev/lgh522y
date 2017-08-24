/*
 * bcache stats code
 *
 * Copyright 2012 Google, Inc.
 */

#include "bcache.h"
#include "stats.h"
#include "btree.h"
#include "request.h"
#include "sysfs.h"

/*
                                                                               
                    
  
                                                                      
                                                                           
                                                                               
               
  
                                                                            
                                                   
  
                          
  
                                                                             
                                                                              
                                                                           
                                             
  
                                                                      
                                                                           
 */

static const unsigned DAY_RESCALE		= 288;
static const unsigned HOUR_RESCALE		= 12;
static const unsigned FIVE_MINUTE_RESCALE	= 1;
static const unsigned accounting_delay		= (HZ * 300) / 22;
static const unsigned accounting_weight		= 32;

/*                       */

read_attribute(cache_hits);
read_attribute(cache_misses);
read_attribute(cache_bypass_hits);
read_attribute(cache_bypass_misses);
read_attribute(cache_hit_ratio);
read_attribute(cache_readaheads);
read_attribute(cache_miss_collisions);
read_attribute(bypassed);

SHOW(bch_stats)
{
	struct cache_stats *s =
		container_of(kobj, struct cache_stats, kobj);
#define var(stat)		(s->stat >> 16)
	var_print(cache_hits);
	var_print(cache_misses);
	var_print(cache_bypass_hits);
	var_print(cache_bypass_misses);

	sysfs_print(cache_hit_ratio,
		    DIV_SAFE(var(cache_hits) * 100,
			     var(cache_hits) + var(cache_misses)));

	var_print(cache_readaheads);
	var_print(cache_miss_collisions);
	sysfs_hprint(bypassed,	var(sectors_bypassed) << 9);
#undef var
	return 0;
}

STORE(bch_stats)
{
	return size;
}

static void bch_stats_release(struct kobject *k)
{
}

static struct attribute *bch_stats_files[] = {
	&sysfs_cache_hits,
	&sysfs_cache_misses,
	&sysfs_cache_bypass_hits,
	&sysfs_cache_bypass_misses,
	&sysfs_cache_hit_ratio,
	&sysfs_cache_readaheads,
	&sysfs_cache_miss_collisions,
	&sysfs_bypassed,
	NULL
};
static KTYPE(bch_stats);

int bch_cache_accounting_add_kobjs(struct cache_accounting *acc,
				   struct kobject *parent)
{
	int ret = kobject_add(&acc->total.kobj, parent,
			      "stats_total");
	ret = ret ?: kobject_add(&acc->five_minute.kobj, parent,
				 "stats_five_minute");
	ret = ret ?: kobject_add(&acc->hour.kobj, parent,
				 "stats_hour");
	ret = ret ?: kobject_add(&acc->day.kobj, parent,
				 "stats_day");
	return ret;
}

void bch_cache_accounting_clear(struct cache_accounting *acc)
{
	memset(&acc->total.cache_hits,
	       0,
	       sizeof(unsigned long) * 7);
}

void bch_cache_accounting_destroy(struct cache_accounting *acc)
{
	kobject_put(&acc->total.kobj);
	kobject_put(&acc->five_minute.kobj);
	kobject_put(&acc->hour.kobj);
	kobject_put(&acc->day.kobj);

	atomic_set(&acc->closing, 1);
	if (del_timer_sync(&acc->timer))
		closure_return(&acc->cl);
}

/*              */

static void scale_stat(unsigned long *stat)
{
	*stat =  ewma_add(*stat, 0, accounting_weight, 0);
}

static void scale_stats(struct cache_stats *stats, unsigned long rescale_at)
{
	if (++stats->rescale == rescale_at) {
		stats->rescale = 0;
		scale_stat(&stats->cache_hits);
		scale_stat(&stats->cache_misses);
		scale_stat(&stats->cache_bypass_hits);
		scale_stat(&stats->cache_bypass_misses);
		scale_stat(&stats->cache_readaheads);
		scale_stat(&stats->cache_miss_collisions);
		scale_stat(&stats->sectors_bypassed);
	}
}

static void scale_accounting(unsigned long data)
{
	struct cache_accounting *acc = (struct cache_accounting *) data;

#define move_stat(name) do {						\
	unsigned t = atomic_xchg(&acc->collector.name, 0);		\
	t <<= 16;							\
	acc->five_minute.name += t;					\
	acc->hour.name += t;						\
	acc->day.name += t;						\
	acc->total.name += t;						\
} while (0)

	move_stat(cache_hits);
	move_stat(cache_misses);
	move_stat(cache_bypass_hits);
	move_stat(cache_bypass_misses);
	move_stat(cache_readaheads);
	move_stat(cache_miss_collisions);
	move_stat(sectors_bypassed);

	scale_stats(&acc->total, 0);
	scale_stats(&acc->day, DAY_RESCALE);
	scale_stats(&acc->hour, HOUR_RESCALE);
	scale_stats(&acc->five_minute, FIVE_MINUTE_RESCALE);

	acc->timer.expires += accounting_delay;

	if (!atomic_read(&acc->closing))
		add_timer(&acc->timer);
	else
		closure_return(&acc->cl);
}

static void mark_cache_stats(struct cache_stat_collector *stats,
			     bool hit, bool bypass)
{
	if (!bypass)
		if (hit)
			atomic_inc(&stats->cache_hits);
		else
			atomic_inc(&stats->cache_misses);
	else
		if (hit)
			atomic_inc(&stats->cache_bypass_hits);
		else
			atomic_inc(&stats->cache_bypass_misses);
}

void bch_mark_cache_accounting(struct search *s, bool hit, bool bypass)
{
	struct cached_dev *dc = container_of(s->d, struct cached_dev, disk);
	mark_cache_stats(&dc->accounting.collector, hit, bypass);
	mark_cache_stats(&s->op.c->accounting.collector, hit, bypass);
#ifdef CONFIG_CGROUP_BCACHE
	mark_cache_stats(&(bch_bio_to_cgroup(s->orig_bio)->stats), hit, bypass);
#endif
}

void bch_mark_cache_readahead(struct search *s)
{
	struct cached_dev *dc = container_of(s->d, struct cached_dev, disk);
	atomic_inc(&dc->accounting.collector.cache_readaheads);
	atomic_inc(&s->op.c->accounting.collector.cache_readaheads);
}

void bch_mark_cache_miss_collision(struct search *s)
{
	struct cached_dev *dc = container_of(s->d, struct cached_dev, disk);
	atomic_inc(&dc->accounting.collector.cache_miss_collisions);
	atomic_inc(&s->op.c->accounting.collector.cache_miss_collisions);
}

void bch_mark_sectors_bypassed(struct search *s, int sectors)
{
	struct cached_dev *dc = container_of(s->d, struct cached_dev, disk);
	atomic_add(sectors, &dc->accounting.collector.sectors_bypassed);
	atomic_add(sectors, &s->op.c->accounting.collector.sectors_bypassed);
}

void bch_cache_accounting_init(struct cache_accounting *acc,
			       struct closure *parent)
{
	kobject_init(&acc->total.kobj,		&bch_stats_ktype);
	kobject_init(&acc->five_minute.kobj,	&bch_stats_ktype);
	kobject_init(&acc->hour.kobj,		&bch_stats_ktype);
	kobject_init(&acc->day.kobj,		&bch_stats_ktype);

	closure_init(&acc->cl, parent);
	init_timer(&acc->timer);
	acc->timer.expires	= jiffies + accounting_delay;
	acc->timer.data		= (unsigned long) acc;
	acc->timer.function	= scale_accounting;
	add_timer(&acc->timer);
}