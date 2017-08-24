/*
                                                                    
            
 */

#include "sched.h"
#if defined(CONFIG_HMP_TRACER) || \
	defined(CONFIG_MT_RT_SCHED_CRIT) || defined(CONFIG_MT_RT_SCHED_NOTICE)
#include <trace/events/sched.h>
#endif

#include <linux/slab.h>

int sched_rr_timeslice = RR_TIMESLICE;

static int do_sched_rt_period_timer(struct rt_bandwidth *rt_b, int overrun);

struct rt_bandwidth def_rt_bandwidth;

static enum hrtimer_restart sched_rt_period_timer(struct hrtimer *timer)
{
	struct rt_bandwidth *rt_b =
		container_of(timer, struct rt_bandwidth, rt_period_timer);
	ktime_t now;
	int overrun;
	int idle = 0;

	for (;;) {
		now = hrtimer_cb_get_time(timer);
		overrun = hrtimer_forward(timer, now, rt_b->rt_period);

		if (!overrun)
			break;

		idle = do_sched_rt_period_timer(rt_b, overrun);
	}

	return idle ? HRTIMER_NORESTART : HRTIMER_RESTART;
}

void init_rt_bandwidth(struct rt_bandwidth *rt_b, u64 period, u64 runtime)
{
	rt_b->rt_period = ns_to_ktime(period);
	rt_b->rt_runtime = runtime;

	raw_spin_lock_init(&rt_b->rt_runtime_lock);

	hrtimer_init(&rt_b->rt_period_timer,
			CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	rt_b->rt_period_timer.function = sched_rt_period_timer;
}

static void start_rt_bandwidth(struct rt_bandwidth *rt_b)
{
	if (!rt_bandwidth_enabled() || rt_b->rt_runtime == RUNTIME_INF)
		return;

	if (hrtimer_active(&rt_b->rt_period_timer))
		return;

	raw_spin_lock(&rt_b->rt_runtime_lock);
	start_bandwidth_timer(&rt_b->rt_period_timer, rt_b->rt_period);
	raw_spin_unlock(&rt_b->rt_runtime_lock);
}

#ifdef CONFIG_PROVE_LOCKING 
DEFINE_RAW_SPINLOCK(rt_rq_runtime_spinlock);
#define MAX_SPIN_KEY 10
DEFINE_PER_CPU(struct lock_class_key, spin_key[MAX_SPIN_KEY]);
DEFINE_PER_CPU(int, spin_key_idx);
#endif
void init_rt_rq(struct rt_rq *rt_rq, struct rq *rq)
{
	struct rt_prio_array *array;
	int i;
#ifdef CONFIG_PROVE_LOCKING 
	int cpu, idx;
#endif

	array = &rt_rq->active;
	for (i = 0; i < MAX_RT_PRIO; i++) {
		INIT_LIST_HEAD(array->queue + i);
		__clear_bit(i, array->bitmap);
	}
	/*                          */
	__set_bit(MAX_RT_PRIO, array->bitmap);

#if defined CONFIG_SMP
	rt_rq->highest_prio.curr = MAX_RT_PRIO;
	rt_rq->highest_prio.next = MAX_RT_PRIO;
	rt_rq->rt_nr_migratory = 0;
	rt_rq->overloaded = 0;
	plist_head_init(&rt_rq->pushable_tasks);
#endif

	rt_rq->rt_time = 0;
	rt_rq->rt_throttled = 0;
	rt_rq->rt_runtime = 0;
	/*                                                                                 */
	rt_rq->rt_disable_borrow = 0;
#ifdef CONFIG_PROVE_LOCKING 
	raw_spin_lock(&rt_rq_runtime_spinlock);
	cpu = rq->cpu;
	idx = per_cpu(spin_key_idx, cpu);
#endif
	raw_spin_lock_init(&rt_rq->rt_runtime_lock);
#ifdef CONFIG_PROVE_LOCKING 
	lockdep_set_class(&rt_rq->rt_runtime_lock, &per_cpu(spin_key[idx], cpu));
	per_cpu(spin_key_idx, cpu)++;
	BUG_ON(per_cpu(spin_key_idx, cpu) >= MAX_SPIN_KEY);
	raw_spin_unlock(&rt_rq_runtime_spinlock);
#endif
}

#ifdef CONFIG_RT_GROUP_SCHED
static void destroy_rt_bandwidth(struct rt_bandwidth *rt_b)
{
	hrtimer_cancel(&rt_b->rt_period_timer);
}

#define rt_entity_is_task(rt_se) (!(rt_se)->my_q)

static inline struct task_struct *rt_task_of(struct sched_rt_entity *rt_se)
{
#ifdef CONFIG_SCHED_DEBUG
	WARN_ON_ONCE(!rt_entity_is_task(rt_se));
#endif
	return container_of(rt_se, struct task_struct, rt);
}

static inline struct rq *rq_of_rt_rq(struct rt_rq *rt_rq)
{
	return rt_rq->rq;
}

static inline struct rt_rq *rt_rq_of_se(struct sched_rt_entity *rt_se)
{
	return rt_se->rt_rq;
}

void free_rt_sched_group(struct task_group *tg)
{
	int i;

	if (tg->rt_se)
		destroy_rt_bandwidth(&tg->rt_bandwidth);

	for_each_possible_cpu(i) {
		if (tg->rt_rq)
			kfree(tg->rt_rq[i]);
		if (tg->rt_se)
			kfree(tg->rt_se[i]);
	}

	kfree(tg->rt_rq);
	kfree(tg->rt_se);
}

void init_tg_rt_entry(struct task_group *tg, struct rt_rq *rt_rq,
		struct sched_rt_entity *rt_se, int cpu,
		struct sched_rt_entity *parent)
{
	struct rq *rq = cpu_rq(cpu);

	rt_rq->highest_prio.curr = MAX_RT_PRIO;
	rt_rq->rt_nr_boosted = 0;
	rt_rq->rq = rq;
	rt_rq->tg = tg;

	tg->rt_rq[cpu] = rt_rq;
	tg->rt_se[cpu] = rt_se;

	if (!rt_se)
		return;

	if (!parent)
		rt_se->rt_rq = &rq->rt;
	else
		rt_se->rt_rq = parent->my_q;

	rt_se->my_q = rt_rq;
	rt_se->parent = parent;
	INIT_LIST_HEAD(&rt_se->run_list);
}

int alloc_rt_sched_group(struct task_group *tg, struct task_group *parent)
{
	struct rt_rq *rt_rq;
	struct sched_rt_entity *rt_se;
	int i;

	tg->rt_rq = kzalloc(sizeof(rt_rq) * nr_cpu_ids, GFP_KERNEL);
	if (!tg->rt_rq)
		goto err;
	tg->rt_se = kzalloc(sizeof(rt_se) * nr_cpu_ids, GFP_KERNEL);
	if (!tg->rt_se)
		goto err;

	init_rt_bandwidth(&tg->rt_bandwidth,
			ktime_to_ns(def_rt_bandwidth.rt_period), 0);

	for_each_possible_cpu(i) {
		rt_rq = kzalloc_node(sizeof(struct rt_rq),
				     GFP_KERNEL, cpu_to_node(i));
		if (!rt_rq)
			goto err;

		rt_se = kzalloc_node(sizeof(struct sched_rt_entity),
				     GFP_KERNEL, cpu_to_node(i));
		if (!rt_se)
			goto err_free_rq;

		init_rt_rq(rt_rq, cpu_rq(i));
		rt_rq->rt_runtime = tg->rt_bandwidth.rt_runtime;
		init_tg_rt_entry(tg, rt_rq, rt_se, i, parent->rt_se[i]);
	}

	return 1;

err_free_rq:
	kfree(rt_rq);
err:
	return 0;
}

#else /*                       */

#define rt_entity_is_task(rt_se) (1)

static inline struct task_struct *rt_task_of(struct sched_rt_entity *rt_se)
{
	return container_of(rt_se, struct task_struct, rt);
}

static inline struct rq *rq_of_rt_rq(struct rt_rq *rt_rq)
{
	return container_of(rt_rq, struct rq, rt);
}

static inline struct rt_rq *rt_rq_of_se(struct sched_rt_entity *rt_se)
{
	struct task_struct *p = rt_task_of(rt_se);
	struct rq *rq = task_rq(p);

	return &rq->rt;
}

void free_rt_sched_group(struct task_group *tg) { }

int alloc_rt_sched_group(struct task_group *tg, struct task_group *parent)
{
	return 1;
}
#endif /*                       */

#if defined(CONFIG_MT_RT_SCHED) || defined(CONFIG_MT_RT_SCHED_LOG)
extern struct cpumask hmp_fast_cpu_mask;
extern struct cpumask hmp_slow_cpu_mask;
#endif 

#ifdef CONFIG_SMP

static inline int rt_overloaded(struct rq *rq)
{
	return atomic_read(&rq->rd->rto_count);
}

static inline void rt_set_overload(struct rq *rq)
{
	if (!rq->online)
		return;

	cpumask_set_cpu(rq->cpu, rq->rd->rto_mask);
	/*
                                               
                                                    
                                                      
                                                  
                
  */
	wmb();
	atomic_inc(&rq->rd->rto_count);
}

static inline void rt_clear_overload(struct rq *rq)
{
	if (!rq->online)
		return;

	/*                                      */
	atomic_dec(&rq->rd->rto_count);
	cpumask_clear_cpu(rq->cpu, rq->rd->rto_mask);
}

#ifdef CONFIG_MT_RT_SCHED
#if 0
static inline int rt_overloaded_in_big(struct rq *rq)
{
	cpumask_var_t new_mask;
	cpumask_and(new_mask, rq->rd->rto_mask, &hmp_fast_cpu_mask);
#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("rt_overloaded_in_big %lu:%lu:%lu",
		new_mask->bits[0], rq->rd->rto_mask->bits[0], hmp_fast_cpu_mask.bits[0]);
		
#endif
	if (cpumask_empty(new_mask))
		return 0;

	return 1;
}
#endif 
static inline int has_rt_task_in_little(void)
{
	int cpu;
	struct rq *rq;
	
	for_each_cpu(cpu, &hmp_slow_cpu_mask){
		if (!cpu_online(cpu))
			continue;

		rq = cpu_rq(cpu);
		if(rq->rt.rt_nr_running >= 1)
			return 1;
	}	

	return 0;				
}
#endif

static void update_rt_migration(struct rt_rq *rt_rq)
{
	if (rt_rq->rt_nr_migratory && rt_rq->rt_nr_total > 1) {
		if (!rt_rq->overloaded) {
			rt_set_overload(rq_of_rt_rq(rt_rq));
			rt_rq->overloaded = 1;
		}
	} else if (rt_rq->overloaded) {
		rt_clear_overload(rq_of_rt_rq(rt_rq));
		rt_rq->overloaded = 0;
	}
}

static void inc_rt_migration(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq)
{
	struct task_struct *p;

	if (!rt_entity_is_task(rt_se))
		return;

	p = rt_task_of(rt_se);
	rt_rq = &rq_of_rt_rq(rt_rq)->rt;

	rt_rq->rt_nr_total++;
	if (p->nr_cpus_allowed > 1)
		rt_rq->rt_nr_migratory++;

	update_rt_migration(rt_rq);
}

static void dec_rt_migration(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq)
{
	struct task_struct *p;

	if (!rt_entity_is_task(rt_se))
		return;

	p = rt_task_of(rt_se);
	rt_rq = &rq_of_rt_rq(rt_rq)->rt;

	rt_rq->rt_nr_total--;
	if (p->nr_cpus_allowed > 1)
		rt_rq->rt_nr_migratory--;

	update_rt_migration(rt_rq);
}

static inline int has_pushable_tasks(struct rq *rq)
{
	return !plist_head_empty(&rq->rt.pushable_tasks);
}

static void enqueue_pushable_task(struct rq *rq, struct task_struct *p)
{
	plist_del(&p->pushable_tasks, &rq->rt.pushable_tasks);
	plist_node_init(&p->pushable_tasks, p->prio);
	plist_add(&p->pushable_tasks, &rq->rt.pushable_tasks);

	/*                                       */
	if (p->prio < rq->rt.highest_prio.next)
		rq->rt.highest_prio.next = p->prio;
}

static void dequeue_pushable_task(struct rq *rq, struct task_struct *p)
{
	plist_del(&p->pushable_tasks, &rq->rt.pushable_tasks);

	/*                                           */
	if (has_pushable_tasks(rq)) {
		p = plist_first_entry(&rq->rt.pushable_tasks,
				      struct task_struct, pushable_tasks);
		rq->rt.highest_prio.next = p->prio;
	} else
		rq->rt.highest_prio.next = MAX_RT_PRIO;
}

#else

static inline void enqueue_pushable_task(struct rq *rq, struct task_struct *p)
{
}

static inline void dequeue_pushable_task(struct rq *rq, struct task_struct *p)
{
}

static inline
void inc_rt_migration(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq)
{
}

static inline
void dec_rt_migration(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq)
{
}

#endif /*            */

static inline int on_rt_rq(struct sched_rt_entity *rt_se)
{
	return !list_empty(&rt_se->run_list);
}

#ifdef CONFIG_RT_GROUP_SCHED

static inline u64 sched_rt_runtime(struct rt_rq *rt_rq)
{
	if (!rt_rq->tg)
		return RUNTIME_INF;

	return rt_rq->rt_runtime;
}

static inline u64 sched_rt_period(struct rt_rq *rt_rq)
{
	return ktime_to_ns(rt_rq->tg->rt_bandwidth.rt_period);
}

typedef struct task_group *rt_rq_iter_t;

static inline struct task_group *next_task_group(struct task_group *tg)
{
	do {
		tg = list_entry_rcu(tg->list.next,
			typeof(struct task_group), list);
	} while (&tg->list != &task_groups && task_group_is_autogroup(tg));

	if (&tg->list == &task_groups)
		tg = NULL;

	return tg;
}

#define for_each_rt_rq(rt_rq, iter, rq)					\
	for (iter = container_of(&task_groups, typeof(*iter), list);	\
		(iter = next_task_group(iter)) &&			\
		(rt_rq = iter->rt_rq[cpu_of(rq)]);)

static inline void list_add_leaf_rt_rq(struct rt_rq *rt_rq)
{
	list_add_rcu(&rt_rq->leaf_rt_rq_list,
			&rq_of_rt_rq(rt_rq)->leaf_rt_rq_list);
}

static inline void list_del_leaf_rt_rq(struct rt_rq *rt_rq)
{
	list_del_rcu(&rt_rq->leaf_rt_rq_list);
}

#define for_each_leaf_rt_rq(rt_rq, rq) \
	list_for_each_entry_rcu(rt_rq, &rq->leaf_rt_rq_list, leaf_rt_rq_list)

#define for_each_sched_rt_entity(rt_se) \
	for (; rt_se; rt_se = rt_se->parent)

static inline struct rt_rq *group_rt_rq(struct sched_rt_entity *rt_se)
{
	return rt_se->my_q;
}

static void enqueue_rt_entity(struct sched_rt_entity *rt_se, bool head);
static void dequeue_rt_entity(struct sched_rt_entity *rt_se);

static void sched_rt_rq_enqueue(struct rt_rq *rt_rq)
{
	struct task_struct *curr = rq_of_rt_rq(rt_rq)->curr;
	struct sched_rt_entity *rt_se;

	int cpu = cpu_of(rq_of_rt_rq(rt_rq));

	rt_se = rt_rq->tg->rt_se[cpu];

	if (rt_rq->rt_nr_running) {
		if (rt_se && !on_rt_rq(rt_se))
			enqueue_rt_entity(rt_se, false);
		if (rt_rq->highest_prio.curr < curr->prio)
			resched_task(curr);
	}
}

static void sched_rt_rq_dequeue(struct rt_rq *rt_rq)
{
	struct sched_rt_entity *rt_se;
	int cpu = cpu_of(rq_of_rt_rq(rt_rq));

	rt_se = rt_rq->tg->rt_se[cpu];

	if (rt_se && on_rt_rq(rt_se))
		dequeue_rt_entity(rt_se);
}

static inline int rt_rq_throttled(struct rt_rq *rt_rq)
{
	return rt_rq->rt_throttled && !rt_rq->rt_nr_boosted;
}

static int rt_se_boosted(struct sched_rt_entity *rt_se)
{
	struct rt_rq *rt_rq = group_rt_rq(rt_se);
	struct task_struct *p;

	if (rt_rq)
		return !!rt_rq->rt_nr_boosted;

	p = rt_task_of(rt_se);
	return p->prio != p->normal_prio;
}

#ifdef CONFIG_SMP
static inline const struct cpumask *sched_rt_period_mask(void)
{
	return cpu_rq(smp_processor_id())->rd->span;
}
#else
static inline const struct cpumask *sched_rt_period_mask(void)
{
	return cpu_online_mask;
}
#endif

static inline
struct rt_rq *sched_rt_period_rt_rq(struct rt_bandwidth *rt_b, int cpu)
{
	return container_of(rt_b, struct task_group, rt_bandwidth)->rt_rq[cpu];
}

static inline struct rt_bandwidth *sched_rt_bandwidth(struct rt_rq *rt_rq)
{
	return &rt_rq->tg->rt_bandwidth;
}

void unthrottle_offline_rt_rqs(struct rq *rq) {
	struct rt_rq *rt_rq;

	for_each_leaf_rt_rq(rt_rq, rq) {
		/*
                                                             
                                    
   */
		if (rt_rq_throttled(rt_rq)){
			rt_rq->rt_throttled = 0;
			printk(KERN_ERR "sched: RT throttling inactivated\n");
		}
	}
}

#else /*                        */

static inline u64 sched_rt_runtime(struct rt_rq *rt_rq)
{
	return rt_rq->rt_runtime;
}

static inline u64 sched_rt_period(struct rt_rq *rt_rq)
{
	return ktime_to_ns(def_rt_bandwidth.rt_period);
}

typedef struct rt_rq *rt_rq_iter_t;

#define for_each_rt_rq(rt_rq, iter, rq) \
	for ((void) iter, rt_rq = &rq->rt; rt_rq; rt_rq = NULL)

static inline void list_add_leaf_rt_rq(struct rt_rq *rt_rq)
{
}

static inline void list_del_leaf_rt_rq(struct rt_rq *rt_rq)
{
}

#define for_each_leaf_rt_rq(rt_rq, rq) \
	for (rt_rq = &rq->rt; rt_rq; rt_rq = NULL)

#define for_each_sched_rt_entity(rt_se) \
	for (; rt_se; rt_se = NULL)

static inline struct rt_rq *group_rt_rq(struct sched_rt_entity *rt_se)
{
	return NULL;
}

static inline void sched_rt_rq_enqueue(struct rt_rq *rt_rq)
{
	if (rt_rq->rt_nr_running)
		resched_task(rq_of_rt_rq(rt_rq)->curr);
}

static inline void sched_rt_rq_dequeue(struct rt_rq *rt_rq)
{
}

static inline int rt_rq_throttled(struct rt_rq *rt_rq)
{
	return rt_rq->rt_throttled;
}

static inline const struct cpumask *sched_rt_period_mask(void)
{
	return cpu_online_mask;
}

static inline
struct rt_rq *sched_rt_period_rt_rq(struct rt_bandwidth *rt_b, int cpu)
{
	return &cpu_rq(cpu)->rt;
}

static inline struct rt_bandwidth *sched_rt_bandwidth(struct rt_rq *rt_rq)
{
	return &def_rt_bandwidth;
}

void unthrottle_offline_rt_rqs(struct rq *rq) { }

#endif /*                       */

#ifdef CONFIG_SMP
/*
                                                                        
 */
//                        
static int do_balance_runtime(struct rt_rq *rt_rq)
{
	struct rt_bandwidth *rt_b = sched_rt_bandwidth(rt_rq);
	struct root_domain *rd = rq_of_rt_rq(rt_rq)->rd;
	int i, weight, more = 0;
	u64 rt_period;

	weight = cpumask_weight(rd->span);

	raw_spin_lock(&rt_b->rt_runtime_lock);
	raw_spin_lock(&rt_rq->rt_runtime_lock);
	
	if (rt_rq->rt_disable_borrow ==1){
		raw_spin_unlock(&rt_rq->rt_runtime_lock);
		raw_spin_unlock(&rt_b->rt_runtime_lock);
		return 0;
	}
	rt_period = ktime_to_ns(rt_b->rt_period);

#ifdef MTK_DEBUG_CGROUP
	printk(KERN_EMERG " do_balance_runtime curr_cpu=%d, dst_cpu=%d, span=%lu\n",
		smp_processor_id(), rt_rq->rq->cpu, rd->span->bits[0]);
#endif
	for_each_cpu(i, rd->span) {
		struct rt_rq *iter = sched_rt_period_rt_rq(rt_b, i);
		s64 diff;

		if (iter == rt_rq)
			continue;

		/*                                             */
		//                                        
#ifdef MTK_DEBUG_CGROUP
		printk(KERN_EMERG " do_balance_runtime get lock cpu=%d\n", i);
#endif
		if(!raw_spin_trylock(&iter->rt_runtime_lock)){
#ifdef MTK_DEBUG_CGROUP
			printk(KERN_EMERG " do_balance_runtime try lock fail cpu=%d\n", i);
#endif
			continue;
		}
		/*
                                                                 
                                                              
                                                     
   */
		if (iter->rt_disable_borrow ==1)
			goto next;	
		if (iter->rt_runtime == RUNTIME_INF)
			goto next;

		/*
                                                           
                                             
   */
		diff = iter->rt_runtime - iter->rt_time;

#ifdef MTK_DEBUG_CGROUP
		printk(KERN_EMERG "borrow, dst_cpu=%d, src_cpu=%d, src_cpu2=%d, src_addr=%x, dst_addr=%x,dst->rt_runtime=%llu, src->rt_runtime=%llu, diff=%lld, span=%lu\n",
			rt_rq->rq->cpu, i, iter->rq->cpu, iter, 
			rt_rq, rt_rq->rt_runtime, iter->rt_runtime, diff, rd->span->bits[0]);
#endif
		if (diff > 0) {
			diff = div_u64((u64)diff, weight);
			if (rt_rq->rt_runtime + diff > rt_period)
				diff = rt_period - rt_rq->rt_runtime;
			iter->rt_runtime -= diff;
			rt_rq->rt_runtime += diff;
			more = 1;
#ifdef MTK_DEBUG_CGROUP
			printk(KERN_EMERG "borrow successfully, dst_cpu=%d, src_cpu=%d, src_cpu2=%d, src_addr=%x, dst_addr=%x,dst->rt_runtime=%llu, src->rt_runtime=%llu, diff=%lld, span=%lu\n",
				rt_rq->rq->cpu, i, iter->rq->cpu, iter, 
				rt_rq, rt_rq->rt_runtime, iter->rt_runtime, diff, rd->span->bits[0]);
#endif
			if (rt_rq->rt_runtime == rt_period) {
				raw_spin_unlock(&iter->rt_runtime_lock);
				break;
			}
		}
next:
		raw_spin_unlock(&iter->rt_runtime_lock);
	}
	raw_spin_unlock(&rt_rq->rt_runtime_lock);
	raw_spin_unlock(&rt_b->rt_runtime_lock);

	return more;
}

/*
                                                                       
 */
static void __disable_runtime(struct rq *rq)
{
	struct root_domain *rd = rq->rd;
	rt_rq_iter_t iter;
	struct rt_rq *rt_rq;

	if (unlikely(!scheduler_running))
		return;

	for_each_rt_rq(rt_rq, iter, rq) {
		struct rt_bandwidth *rt_b = sched_rt_bandwidth(rt_rq);
		s64 want;
		int i;

		raw_spin_lock(&rt_b->rt_runtime_lock);
		raw_spin_lock(&rt_rq->rt_runtime_lock);
		/*                                   */
		rt_rq->rt_disable_borrow = 1;
		/*
                                                              
                                                             
                                                     
   */
#ifdef MTK_DEBUG_CGROUP
		printk(KERN_EMERG "0. disable_runtime, cpu=%d, rd->span=%lu, rt_rq_addr=%x, rt_rq->rt_runtime=%llu, rt_b->rt_runtime=%llu\n",
			rt_rq->rq->cpu, rd->span->bits[0], 
			rt_rq, rt_rq->rt_runtime, rt_b->rt_runtime);
#endif
		if (rt_rq->rt_runtime == RUNTIME_INF ||
				rt_rq->rt_runtime == rt_b->rt_runtime)
			goto balanced;
		raw_spin_unlock(&rt_rq->rt_runtime_lock);

		/*
                                                              
                                                           
                                     
   */
		want = rt_b->rt_runtime - rt_rq->rt_runtime;

		/*
                                                 
   */
		for_each_cpu(i, rd->span) {
			struct rt_rq *iter = sched_rt_period_rt_rq(rt_b, i);
			s64 diff;

#ifdef MTK_DEBUG_CGROUP
			printk(KERN_EMERG "0. disable_runtime, cpu=%d,rt_b->rt_runtime=%llu, rt_rq->rt_runtime=%llu, want=%lld, rd->span=%lu\n",  
				rt_rq->rq->cpu, rt_b->rt_runtime, rt_rq->rt_runtime, want, rd->span->bits[0]);
#endif

			/*
                                                         
    */
			if (iter == rt_rq || iter->rt_runtime == RUNTIME_INF || iter->rt_disable_borrow){
#ifdef MTK_DEBUG_CGROUP
				printk(KERN_EMERG "1. disable_runtime, cpu=%d, %llu\n",
					i, iter->rt_runtime);
#endif
				continue;
			}

			raw_spin_lock(&iter->rt_runtime_lock);
#ifdef MTK_DEBUG_CGROUP
			printk(KERN_EMERG "2-1. disable_runtime cpu=%d, want=%lld, iter->rt_runtime=%llu\n",
				i, want, iter->rt_runtime);
#endif
			if (want > 0) {
				diff = min_t(s64, iter->rt_runtime, want);
				iter->rt_runtime -= diff;
				want -= diff;
#ifdef MTK_DEBUG_CGROUP
				printk(KERN_EMERG "2. disable_runtime, rt_runtime=%llu, diff=%lld, want=%lld\n",
					iter->rt_runtime, diff, want);
#endif
			} else {
				iter->rt_runtime -= want;
				want -= want;
#ifdef MTK_DEBUG_CGROUP
				printk(KERN_EMERG "3. disable_runtime, rt_runtime=%llu, want=%lld\n", iter->rt_runtime, want);
#endif
			}
			raw_spin_unlock(&iter->rt_runtime_lock);

			if (!want)
				break;
		}

		raw_spin_lock(&rt_rq->rt_runtime_lock);
		/*
                                                             
                              
   */
		if(want){
#ifdef MTK_DEBUG_CGROUP
			printk(KERN_EMERG "4. disable_runtime, want=%lld, rt_rq->rt_runtime=%llu\n",
				want, rt_rq->rt_runtime);
			{
			struct rt_rq *iter = sched_rt_period_rt_rq(rt_b, 0);
			printk(KERN_EMERG "4-0. disable_runtime %llu\n", iter->rt_runtime);
			iter = sched_rt_period_rt_rq(rt_b, 1);
			printk(KERN_EMERG "4-1. disable_runtime %llu\n", iter->rt_runtime);
			iter = sched_rt_period_rt_rq(rt_b, 2);
			printk(KERN_EMERG "4-2. disable_runtime %llu\n", iter->rt_runtime);
			iter = sched_rt_period_rt_rq(rt_b, 3);
			printk(KERN_EMERG "4-3. disable_runtime %llu\n", iter->rt_runtime);
			}
#endif
			
			BUG_ON(want);
		}
balanced:
		/*
                                                           
                                                          
   */
		//                                                                          
		//                                
		rt_rq->rt_runtime = rt_b->rt_runtime;
		rt_rq->rt_throttled = 0;
#ifdef MTK_DEBUG_CGROUP
		{
		struct rt_rq *iter = sched_rt_period_rt_rq(rt_b, 0);
		printk(KERN_EMERG "5-0. disable_runtime %llu\n", iter->rt_runtime);
		iter = sched_rt_period_rt_rq(rt_b, 1);
		printk(KERN_EMERG "5-1. disable_runtime %llu\n", iter->rt_runtime);
		iter = sched_rt_period_rt_rq(rt_b, 2);
		printk(KERN_EMERG "5-2. disable_runtime %llu\n", iter->rt_runtime);
		iter = sched_rt_period_rt_rq(rt_b, 3);
		printk(KERN_EMERG "5-3. disable_runtime %llu\n", iter->rt_runtime);
		}
#endif
		raw_spin_unlock(&rt_rq->rt_runtime_lock);
		raw_spin_unlock(&rt_b->rt_runtime_lock);
#ifdef MTK_DEBUG_CGROUP
		printk(KERN_ERR "disable_runtime after: rt_rq->rt_runtime=%llu rq_rt->rt_throttled=%d\n",
			rt_rq->rt_runtime, rt_rq->rt_throttled);
#endif
	}

#ifdef CONFIG_MT_RT_SCHED_CRIT
	trace_sched_rt_crit(rq->cpu, rq->rt.rt_throttled);
#endif
}

static void disable_runtime(struct rq *rq)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&rq->lock, flags);
	__disable_runtime(rq);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}

static void __enable_runtime(struct rq *rq)
{
	rt_rq_iter_t iter;
	struct rt_rq *rt_rq;

	if (unlikely(!scheduler_running))
		return;

	/*
                                            
  */
	for_each_rt_rq(rt_rq, iter, rq) {
		struct rt_bandwidth *rt_b = sched_rt_bandwidth(rt_rq);

		raw_spin_lock(&rt_b->rt_runtime_lock);
		raw_spin_lock(&rt_rq->rt_runtime_lock);
		if (rt_rq->rt_disable_borrow ){
			#ifdef MTK_DEBUG_CGROUP
			printk(KERN_EMERG "enable_runtime %d \n", rq->cpu);
			#endif
			rt_rq->rt_runtime = rt_b->rt_runtime;
			rt_rq->rt_time = 0;
			rt_rq->rt_throttled = 0;
			rt_rq->rt_disable_borrow = 0;
		}
		raw_spin_unlock(&rt_rq->rt_runtime_lock);
		raw_spin_unlock(&rt_b->rt_runtime_lock);
	}

#ifdef CONFIG_MT_RT_SCHED_CRIT
	trace_sched_rt_crit(rq->cpu, rq->rt.rt_throttled);
#endif
}

static void enable_runtime(struct rq *rq)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&rq->lock, flags);
	__enable_runtime(rq);
	raw_spin_unlock_irqrestore(&rq->lock, flags);
}

int update_runtime(struct notifier_block *nfb, unsigned long action, void *hcpu)
{
	int cpu = (int)(long)hcpu;

	switch (action) {
	case CPU_DOWN_PREPARE:
	case CPU_DOWN_PREPARE_FROZEN:
		disable_runtime(cpu_rq(cpu));
		return NOTIFY_OK;

	case CPU_DOWN_FAILED:
	case CPU_DOWN_FAILED_FROZEN:
	case CPU_ONLINE:
	case CPU_ONLINE_FROZEN:
		enable_runtime(cpu_rq(cpu));
		return NOTIFY_OK;

	default:
		return NOTIFY_DONE;
	}
}

static int balance_runtime(struct rt_rq *rt_rq)
{
	int more = 0;

	if (!sched_feat(RT_RUNTIME_SHARE))
		return more;

	if (rt_rq->rt_time > rt_rq->rt_runtime) {
		raw_spin_unlock(&rt_rq->rt_runtime_lock);
		more = do_balance_runtime(rt_rq);
		raw_spin_lock(&rt_rq->rt_runtime_lock);
	}

	return more;
}
#else /*             */
static inline int balance_runtime(struct rt_rq *rt_rq)
{
	return 0;
}
#endif /*            */

static int do_sched_rt_period_timer(struct rt_bandwidth *rt_b, int overrun)
{
	int i, idle = 1, throttled = 0;
	const struct cpumask *span;

	span = sched_rt_period_mask();
#ifdef CONFIG_RT_GROUP_SCHED
	/*
                                                                 
                                                                
                                                                
                                                              
                                                                 
                                                               
                                                                 
  */
	if (rt_b == &root_task_group.rt_bandwidth)
		span = cpu_online_mask;
#endif

#ifdef MTK_DEBUG_CGROUP
	printk(KERN_EMERG " do_sched_rt_period_timer curr_cpu=%d \n", smp_processor_id());
#endif
	for_each_cpu(i, span) {
		int enqueue = 0;
		struct rt_rq *rt_rq = sched_rt_period_rt_rq(rt_b, i);
		struct rq *rq = rq_of_rt_rq(rt_rq);

		raw_spin_lock(&rq->lock);
		if (rt_rq->rt_time) {
			u64 runtime;
			u64 runtime_pre, rt_time_pre;

			raw_spin_lock(&rt_rq->rt_runtime_lock);
			if (rt_rq->rt_throttled) {
				runtime_pre = rt_rq->rt_runtime;
				balance_runtime(rt_rq);
				rt_time_pre = rt_rq->rt_time;
			}
			runtime = rt_rq->rt_runtime;
			rt_rq->rt_time -= min(rt_rq->rt_time, overrun*runtime);
			if (rt_rq->rt_throttled) {
				printk_deferred("sched: cpu=%d, [%llu -> %llu]"
					     " -= min(%llu, %d*[%llu -> %llu])"
					     "\n", i, rt_time_pre,
					     rt_rq->rt_time, rt_time_pre,
					     overrun, runtime_pre, runtime);
			}
			if (rt_rq->rt_throttled && rt_rq->rt_time < runtime) {
				printk_deferred("sched: RT throttling inactivated"
					     " cpu=%d\n", i);
				rt_rq->rt_throttled = 0;
#ifdef CONFIG_MT_RT_SCHED_CRIT
				trace_sched_rt_crit(rq_cpu(rq), rq->rt.rt_throttled);
#endif

				enqueue = 1;

				/*
                                                
                                                 
     */
				if (rt_rq->rt_nr_running && rq->curr == rq->idle)
					rq->skip_clock_update = -1;
			}
			if (rt_rq->rt_time || rt_rq->rt_nr_running)
				idle = 0;
			raw_spin_unlock(&rt_rq->rt_runtime_lock);
		} else if (rt_rq->rt_nr_running) {
			idle = 0;
			if (!rt_rq_throttled(rt_rq))
				enqueue = 1;
		}
		if (rt_rq->rt_throttled)
			throttled = 1;

		if (enqueue)
			sched_rt_rq_enqueue(rt_rq);
		raw_spin_unlock(&rq->lock);
	}

	if (!throttled && (!rt_bandwidth_enabled() || rt_b->rt_runtime == RUNTIME_INF))
		return 1;

	return idle;
}

static inline int rt_se_prio(struct sched_rt_entity *rt_se)
{
#ifdef CONFIG_RT_GROUP_SCHED
	struct rt_rq *rt_rq = group_rt_rq(rt_se);

	if (rt_rq)
		return rt_rq->highest_prio.curr;
#endif

	return rt_task_of(rt_se)->prio;
}
DEFINE_PER_CPU(u64, exec_delta_time);
DEFINE_PER_CPU(u64, clock_task);
DEFINE_PER_CPU(u64, exec_start);
static int sched_rt_runtime_exceeded(struct rt_rq *rt_rq)
{
	u64 runtime = sched_rt_runtime(rt_rq);
	u64 runtime_pre;

	if (rt_rq->rt_throttled)
		return rt_rq_throttled(rt_rq);

	if (runtime >= sched_rt_period(rt_rq))
		return 0;

	runtime_pre = runtime;
	balance_runtime(rt_rq);
	runtime = sched_rt_runtime(rt_rq);
	if (runtime == RUNTIME_INF)
		return 0;

	if (rt_rq->rt_time > runtime) {
		struct rt_bandwidth *rt_b = sched_rt_bandwidth(rt_rq);
		int cpu = rq_cpu(rt_rq->rq);

		printk_deferred("sched: cpu=%d rt_time %llu <-> runtime"
			     " [%llu -> %llu], exec_delta_time[%llu]"
			     ", clock_task[%llu], exec_start[%llu]\n",
			     cpu, rt_rq->rt_time, runtime_pre, runtime,
			     per_cpu(exec_delta_time, cpu),
			     per_cpu(clock_task, cpu),
			     per_cpu(exec_start, cpu));
		/*
                                                                 
                                          
   */
		/*                                       */
		if (likely(rt_b->rt_runtime)) {
		//                          

			rt_rq->rt_throttled = 1;

		//             
		//              
				printk_deferred("sched: RT throttling activated cpu=%d\n",
					cpu);
		//  
#ifdef CONFIG_MT_RT_SCHED_CRIT
			trace_sched_rt_crit(cpu, rt_rq->rt_throttled);
#endif

		} else {
			/*
                                             
                                                         
                        
    */
			rt_rq->rt_time = 0;
		}

		if (rt_rq_throttled(rt_rq)) {
			sched_rt_rq_dequeue(rt_rq);
			return 1;
		}
	}

	return 0;
}

/*
                                                                        
                                   
 */
static void update_curr_rt(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	struct sched_rt_entity *rt_se = &curr->rt;
	struct rt_rq *rt_rq = rt_rq_of_se(rt_se);
	u64 delta_exec;
	int cpu = rq_cpu(rq);

	if (curr->sched_class != &rt_sched_class)
		return;

	delta_exec = rq->clock_task - curr->se.exec_start;
	if (unlikely((s64)delta_exec <= 0))
		return;

	schedstat_set(curr->se.statistics.exec_max,
		      max(curr->se.statistics.exec_max, delta_exec));
	per_cpu(exec_delta_time, cpu) = delta_exec;
	per_cpu(clock_task, cpu) = rq->clock_task;
	per_cpu(exec_start, cpu) = curr->se.exec_start;
	curr->se.sum_exec_runtime += delta_exec;
	account_group_exec_runtime(curr, delta_exec);

	curr->se.exec_start = rq->clock_task;
	cpuacct_charge(curr, delta_exec);

	sched_rt_avg_update(rq, delta_exec);

	if (!rt_bandwidth_enabled())
		return;

	for_each_sched_rt_entity(rt_se) {
		rt_rq = rt_rq_of_se(rt_se);

		if (sched_rt_runtime(rt_rq) != RUNTIME_INF) {
			raw_spin_lock(&rt_rq->rt_runtime_lock);
			rt_rq->rt_time += delta_exec;
			if (sched_rt_runtime_exceeded(rt_rq))
				resched_task(curr);
			raw_spin_unlock(&rt_rq->rt_runtime_lock);
		}
	}
}

#if defined CONFIG_SMP

static void
inc_rt_prio_smp(struct rt_rq *rt_rq, int prio, int prev_prio)
{
	struct rq *rq = rq_of_rt_rq(rt_rq);

#ifdef CONFIG_RT_GROUP_SCHED
	/*
                                                      
  */
	if (&rq->rt != rt_rq)
		return;
#endif
	if (rq->online && prio < prev_prio)
		cpupri_set(&rq->rd->cpupri, rq->cpu, prio);
}

static void
dec_rt_prio_smp(struct rt_rq *rt_rq, int prio, int prev_prio)
{
	struct rq *rq = rq_of_rt_rq(rt_rq);

#ifdef CONFIG_RT_GROUP_SCHED
	/*
                                                      
  */
	if (&rq->rt != rt_rq)
		return;
#endif
	if (rq->online && rt_rq->highest_prio.curr != prev_prio)
		cpupri_set(&rq->rd->cpupri, rq->cpu, rt_rq->highest_prio.curr);
}

#else /*            */

static inline
void inc_rt_prio_smp(struct rt_rq *rt_rq, int prio, int prev_prio) {}
static inline
void dec_rt_prio_smp(struct rt_rq *rt_rq, int prio, int prev_prio) {}

#endif /*            */

#if defined CONFIG_SMP || defined CONFIG_RT_GROUP_SCHED
static void
inc_rt_prio(struct rt_rq *rt_rq, int prio)
{
	int prev_prio = rt_rq->highest_prio.curr;

	if (prio < prev_prio)
		rt_rq->highest_prio.curr = prio;

	inc_rt_prio_smp(rt_rq, prio, prev_prio);
}

static void
dec_rt_prio(struct rt_rq *rt_rq, int prio)
{
	int prev_prio = rt_rq->highest_prio.curr;

	if (rt_rq->rt_nr_running) {

		WARN_ON(prio < prev_prio);

		/*
                                                       
                                         
   */
		if (prio == prev_prio) {
			struct rt_prio_array *array = &rt_rq->active;

			rt_rq->highest_prio.curr =
				sched_find_first_bit(array->bitmap);
		}

	} else
		rt_rq->highest_prio.curr = MAX_RT_PRIO;

	dec_rt_prio_smp(rt_rq, prio, prev_prio);
}

#else

static inline void inc_rt_prio(struct rt_rq *rt_rq, int prio) {}
static inline void dec_rt_prio(struct rt_rq *rt_rq, int prio) {}

#endif /*                                     */

#ifdef CONFIG_RT_GROUP_SCHED

static void
inc_rt_group(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq)
{
	if (rt_se_boosted(rt_se))
		rt_rq->rt_nr_boosted++;

	if (rt_rq->tg)
		start_rt_bandwidth(&rt_rq->tg->rt_bandwidth);
}

static void
dec_rt_group(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq)
{
	if (rt_se_boosted(rt_se))
		rt_rq->rt_nr_boosted--;

	WARN_ON(!rt_rq->rt_nr_running && rt_rq->rt_nr_boosted);
}

#else /*                       */

static void
inc_rt_group(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq)
{
	start_rt_bandwidth(&def_rt_bandwidth);
}

static inline
void dec_rt_group(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq) {}

#endif /*                       */

static inline
void inc_rt_tasks(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq)
{
	int prio = rt_se_prio(rt_se);

	WARN_ON(!rt_prio(prio));
	rt_rq->rt_nr_running++;

	inc_rt_prio(rt_rq, prio);
	inc_rt_migration(rt_se, rt_rq);
	inc_rt_group(rt_se, rt_rq);
}

static inline
void dec_rt_tasks(struct sched_rt_entity *rt_se, struct rt_rq *rt_rq)
{
	WARN_ON(!rt_prio(rt_se_prio(rt_se)));
	WARN_ON(!rt_rq->rt_nr_running);
	rt_rq->rt_nr_running--;

	dec_rt_prio(rt_rq, rt_se_prio(rt_se));
	dec_rt_migration(rt_se, rt_rq);
	dec_rt_group(rt_se, rt_rq);
}

static void __enqueue_rt_entity(struct sched_rt_entity *rt_se, bool head)
{
	struct rt_rq *rt_rq = rt_rq_of_se(rt_se);
	struct rt_prio_array *array = &rt_rq->active;
	struct rt_rq *group_rq = group_rt_rq(rt_se);
	struct list_head *queue = array->queue + rt_se_prio(rt_se);

	/*
                                                            
                                                                
                                                              
                   
  */
//                                                                         
	if (group_rq && ( !group_rq->rt_nr_running))
		return;

	if (!rt_rq->rt_nr_running)
		list_add_leaf_rt_rq(rt_rq);

	if (head)
		list_add(&rt_se->run_list, queue);
	else
		list_add_tail(&rt_se->run_list, queue);
	__set_bit(rt_se_prio(rt_se), array->bitmap);

	inc_rt_tasks(rt_se, rt_rq);
}

static void __dequeue_rt_entity(struct sched_rt_entity *rt_se)
{
	struct rt_rq *rt_rq = rt_rq_of_se(rt_se);
	struct rt_prio_array *array = &rt_rq->active;

	list_del_init(&rt_se->run_list);
	if (list_empty(array->queue + rt_se_prio(rt_se)))
		__clear_bit(rt_se_prio(rt_se), array->bitmap);

	dec_rt_tasks(rt_se, rt_rq);
	if (!rt_rq->rt_nr_running)
		list_del_leaf_rt_rq(rt_rq);
}

/*
                                                          
                                              
 */
static void dequeue_rt_stack(struct sched_rt_entity *rt_se)
{
	struct sched_rt_entity *back = NULL;

	for_each_sched_rt_entity(rt_se) {
		rt_se->back = back;
		back = rt_se;
	}

	for (rt_se = back; rt_se; rt_se = rt_se->back) {
		if (on_rt_rq(rt_se))
			__dequeue_rt_entity(rt_se);
	}
}

static void enqueue_rt_entity(struct sched_rt_entity *rt_se, bool head)
{
	dequeue_rt_stack(rt_se);
	for_each_sched_rt_entity(rt_se)
		__enqueue_rt_entity(rt_se, head);
}

static void dequeue_rt_entity(struct sched_rt_entity *rt_se)
{
	dequeue_rt_stack(rt_se);

	for_each_sched_rt_entity(rt_se) {
		struct rt_rq *rt_rq = group_rt_rq(rt_se);

		if (rt_rq && rt_rq->rt_nr_running)
			__enqueue_rt_entity(rt_se, false);
	}
}

/*
                                                   
 */
static void
enqueue_task_rt(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_rt_entity *rt_se = &p->rt;

	if (flags & ENQUEUE_WAKEUP)
		rt_se->timeout = 0;

	enqueue_rt_entity(rt_se, flags & ENQUEUE_HEAD);

	if (!task_current(rq, p) && p->nr_cpus_allowed > 1)
		enqueue_pushable_task(rq, p);

	inc_nr_running(rq);
}

static void dequeue_task_rt(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_rt_entity *rt_se = &p->rt;

	update_curr_rt(rq);
	dequeue_rt_entity(rt_se);

	dequeue_pushable_task(rq, p);

	dec_nr_running(rq);
}

/*
                                                                          
                               
 */
static void
requeue_rt_entity(struct rt_rq *rt_rq, struct sched_rt_entity *rt_se, int head)
{
	if (on_rt_rq(rt_se)) {
		struct rt_prio_array *array = &rt_rq->active;
		struct list_head *queue = array->queue + rt_se_prio(rt_se);

		if (head)
			list_move(&rt_se->run_list, queue);
		else
			list_move_tail(&rt_se->run_list, queue);
	}
}

static void requeue_task_rt(struct rq *rq, struct task_struct *p, int head)
{
	struct sched_rt_entity *rt_se = &p->rt;
	struct rt_rq *rt_rq;

	for_each_sched_rt_entity(rt_se) {
		rt_rq = rt_rq_of_se(rt_se);
		requeue_rt_entity(rt_rq, rt_se, head);
	}
}

static void yield_task_rt(struct rq *rq)
{
	requeue_task_rt(rq, rq->curr, 0);
}

#ifdef CONFIG_SMP
static int find_lowest_rq(struct task_struct *task);

static int
select_task_rq_rt(struct task_struct *p, int sd_flag, int flags)
{
	struct task_struct *curr;
	struct rq *rq;
	int cpu;

	cpu = task_cpu(p);

	if (p->nr_cpus_allowed == 1)
		goto out;

	/*                                                     */
	if (sd_flag != SD_BALANCE_WAKE && sd_flag != SD_BALANCE_FORK)
		goto out;

	rq = cpu_rq(cpu);

	rcu_read_lock();
	curr = ACCESS_ONCE(rq->curr); /*                 */

	/*
                                                            
                                                        
                                                 
                            
   
                                                        
                                                            
                                                           
                                                           
                                                          
                                                         
         
   
                                                                
   
                                                         
                                                          
   
                                                                 
                             
  */
#ifdef CONFIG_MT_RT_SCHED_INFO
	if(curr){
		mt_rt_printf("0. select_task_rq_rt cpu=%d p=%d:%s:%d:%d curr=%d:%s:%d:%d", 
			cpu, p->pid, p->comm, p->prio, p->nr_cpus_allowed, 
			curr->pid, curr->comm, curr->prio, curr->nr_cpus_allowed);
	}else{
		mt_rt_printf("0. select_task_rq_rt cpu=%d curr=%d:%s:%d", 
			cpu, p->pid, p->comm, p->prio);
	}
#endif

#ifdef CONFIG_MT_RT_SCHED
	/*                                                  */
	if ( (p->nr_cpus_allowed > 1) ){
#else
	if (curr &&
            unlikely(rt_task(curr)) &&
	    (curr->nr_cpus_allowed < 2 || curr->prio <= p->prio)
		&& (p->nr_cpus_allowed > 1)) {
#endif
		int target = find_lowest_rq(p);

		if (target != -1)
			cpu = target;

#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("1. select_task_rq_rt %d:%d:%s", cpu, p->pid, p->comm);
#endif		
	}
	rcu_read_unlock();

out:
	return cpu;
}

static void check_preempt_equal_prio(struct rq *rq, struct task_struct *p)
{
	if (rq->curr->nr_cpus_allowed == 1)
		return;

	if (p->nr_cpus_allowed != 1
	    && cpupri_find(&rq->rd->cpupri, p, NULL))
		return;

	if (!cpupri_find(&rq->rd->cpupri, rq->curr, NULL))
		return;

	/*
                                                  
                                                   
                                 
  */
	requeue_task_rt(rq, p, 1);
	resched_task(rq->curr);
}

#endif /*            */

/*
                                                              
 */
static void check_preempt_curr_rt(struct rq *rq, struct task_struct *p, int flags)
{
#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("check_preempt_curr_rt %d:%d:%s", p->prio, rq->curr->prio, p->comm);
#endif
	if (p->prio < rq->curr->prio) {
		resched_task(rq->curr);
		return;
	}

#ifdef CONFIG_SMP
	/*
       
   
                                                                   
                                                                        
                                                      
   
                                                                     
                                                                  
                                                                      
         
  */
	if (p->prio == rq->curr->prio && !test_tsk_need_resched(rq->curr))
		check_preempt_equal_prio(rq, p);
#endif
}

#ifdef CONFIG_MT_RT_SCHED
/*                                                   */
static struct sched_rt_entity *pick_next_rt_entity(struct rq *rq,
						   struct rt_rq *rt_rq)
{
	struct rt_prio_array *array = &rt_rq->active;
	struct sched_rt_entity *next = NULL;
	struct sched_rt_entity *rt_se;
	int idx;

	idx = sched_find_first_bit(array->bitmap);
	BUG_ON(idx >= MAX_RT_PRIO);

next_idx:
	list_for_each_entry(rt_se, array->queue + idx, run_list) {
		struct task_struct *p;

		if (!rt_entity_is_task(rt_se)){
			next = rt_se;
			break;
		}

		p = rt_task_of(rt_se);
		if ( (!cpu_online(rq->cpu)) || (!test_tsk_need_released(p))) {
			next = rt_se;
			break;
		}else{
#ifdef CONFIG_MT_RT_SCHED_INFO
			mt_rt_printf("1. pick_next_rt_entity bypass %d %s", p->pid, p->comm);
#endif
		}
	}
	if (!next) {
		idx = find_next_bit(array->bitmap, MAX_RT_PRIO, idx+1);
		if (idx < MAX_RT_PRIO)
			goto next_idx;
	}

	return next;
}

#else
static struct sched_rt_entity *pick_next_rt_entity(struct rq *rq,
						   struct rt_rq *rt_rq)
{
	struct rt_prio_array *array = &rt_rq->active;
	struct sched_rt_entity *next = NULL;
	struct list_head *queue;
	int idx;

	idx = sched_find_first_bit(array->bitmap);
	BUG_ON(idx >= MAX_RT_PRIO);

	queue = array->queue + idx;
	next = list_entry(queue->next, struct sched_rt_entity, run_list);

	return next;
}
#endif

static struct task_struct *_pick_next_task_rt(struct rq *rq)
{
	struct sched_rt_entity *rt_se;
	struct task_struct *p;
	struct rt_rq *rt_rq;

	rt_rq = &rq->rt;

	if (!rt_rq->rt_nr_running)
		return NULL;

	if (rt_rq_throttled(rt_rq)){
		/*                              */
		struct rt_prio_array *array = &rt_rq->active;
		int idx = 0, prio = MAX_RT_PRIO- 1 - idx;  //            

		if( test_bit(idx, array->bitmap)){
			list_for_each_entry(rt_se, array->queue + idx, run_list){
				p = rt_task_of(rt_se);
				if( (p->rt_priority == prio) && (0 == strncmp(p->comm, "wdtk", 4)) ){
					p->se.exec_start = rq->clock_task;
					printk(KERN_WARNING "sched: unthrottle %s\n", p->comm);
					return p;
				}
			}
		}
		return NULL;
	}

	do {
		rt_se = pick_next_rt_entity(rq, rt_rq);
#ifdef CONFIG_MT_RT_SCHED
		if(!rt_se){
#ifdef CONFIG_MT_RT_SCHED_INFO
			mt_rt_printf("_pick_next_task_rt %d:%s:%d:%d:%d", 
				rq->curr->pid, rq->curr->comm, rq->curr->prio,
				test_tsk_need_released(rq->curr), rt_rq->rt_nr_running);
#endif
			return NULL;	
		}
#endif
		BUG_ON(!rt_se);
		rt_rq = group_rt_rq(rt_se);
	} while (rt_rq);

	p = rt_task_of(rt_se);
	p->se.exec_start = rq->clock_task;

	return p;
}

static struct task_struct *pick_next_task_rt(struct rq *rq)
{
	struct task_struct *p = _pick_next_task_rt(rq);

	/*                                                */
	if (p)
		dequeue_pushable_task(rq, p);

#ifdef CONFIG_SMP
	/*
                                                                
                                                
  */
	rq->post_schedule = has_pushable_tasks(rq);
#endif

	return p;
}

static void put_prev_task_rt(struct rq *rq, struct task_struct *p)
{
	update_curr_rt(rq);

	/*
                                                           
                         
  */
#ifdef CONFIG_MT_RT_SCHED
	if (on_rt_rq(&p->rt) && p->nr_cpus_allowed > 1 && !test_tsk_need_released(p))
		enqueue_pushable_task(rq, p);
#else
	if (on_rt_rq(&p->rt) && p->nr_cpus_allowed > 1)
		enqueue_pushable_task(rq, p);
#endif
}

#ifdef CONFIG_SMP

/*                                 */
#define RT_MAX_TRIES 3

static int pick_rt_task(struct rq *rq, struct task_struct *p, int cpu)
{
	if (!task_running(rq, p) &&
	    cpumask_test_cpu(cpu, tsk_cpus_allowed(p)))
		return 1;
	return 0;
}

/*                                                   */
static struct task_struct *pick_next_highest_task_rt(struct rq *rq, int cpu)
{
	struct task_struct *next = NULL;
	struct sched_rt_entity *rt_se;
	struct rt_prio_array *array;
	struct rt_rq *rt_rq;
	int idx;

	for_each_leaf_rt_rq(rt_rq, rq) {
		array = &rt_rq->active;
		idx = sched_find_first_bit(array->bitmap);
next_idx:
		if (idx >= MAX_RT_PRIO)
			continue;
		if (next && next->prio <= idx)
			continue;
		list_for_each_entry(rt_se, array->queue + idx, run_list) {
			struct task_struct *p;

			if (!rt_entity_is_task(rt_se))
				continue;

			p = rt_task_of(rt_se);
			if (pick_rt_task(rq, p, cpu)) {
				next = p;
				break;
			}
		}
		if (!next) {
			idx = find_next_bit(array->bitmap, MAX_RT_PRIO, idx+1);
			goto next_idx;
		}
	}

	return next;
}

static DEFINE_PER_CPU(cpumask_var_t, local_cpu_mask);

#ifdef CONFIG_MT_RT_SCHED
static int test_has_highest_prio(int this_cpu)
{
	int cpu, highest_prio;
	struct rq *this_rq = cpu_rq(this_cpu), *rq;
	int prio = this_rq->curr->prio;

#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("0. test_has_highest_prio %d:%d:%s:%d %lu",
		this_cpu, this_rq->curr->pid, this_rq->curr->comm, prio, tsk_cpus_allowed(this_rq->curr)->bits[0]);
#endif
	if (prio >= MAX_RT_PRIO){
#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("test_has_highest_prio false %d:%d:%s:%d",
			this_cpu, this_rq->curr->pid, this_rq->curr->comm, prio);
#endif
		return 0;
	}
	
	for_each_cpu(cpu, &hmp_fast_cpu_mask) {
		if(!cpu_online(cpu))
			continue;

                if (!cpumask_test_cpu(cpu, tsk_cpus_allowed(this_rq->curr)))
                        continue;

		rq = cpu_rq(cpu);

		if(rq->rt.rt_nr_running == 0){
#ifdef CONFIG_MT_RT_SCHED_NOTICE
			mt_rt_printf( "test_has_highest_prio true %d",
				cpu);
#endif
			return 1;
		}

		highest_prio = rq->rt.highest_prio.curr;

#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf( "1. test_has_highest_prio %d:%d %d",
			cpu, highest_prio, prio);
#endif
		/*                                                               */
		if(prio < highest_prio){
#ifdef CONFIG_MT_RT_SCHED_NOTICE
			mt_rt_printf("test_has_highest_prio true %d:%d:%d",
				cpu, highest_prio, prio);
#endif
			return 1;
		}
	}

#ifdef CONFIG_MT_RT_SCHED_NOTICE
	mt_rt_printf("test_has_highest_prio false %d:%d:%s:%d",
		this_cpu, this_rq->curr->pid, this_rq->curr->comm, prio);
#endif

	return 0;
}

static void release_task_ipi(void *data)
{
#ifdef CONFIG_MT_RT_SCHED_NOTICE	
	int target_cpu = (int)(long) data;
#endif	
	int cpu = smp_processor_id();
	struct rq *rq = cpu_rq(cpu);

#ifdef CONFIG_MT_RT_SCHED_NOTICE
	mt_rt_printf("1. release_task_ipi %d %lu %d", 
		cpu, hmp_slow_cpu_mask.bits[0], target_cpu);
#endif

	/*                                    */
	if (!cpumask_test_cpu(cpu, &hmp_slow_cpu_mask))
		return;

	/*                                           */
	if ( !test_has_highest_prio(cpu)){
#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("3. release_task_ipi false");
#endif
		return;
	}

#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("set_tsk_need_release %d:%s:%d", rq->curr->pid, rq->curr->comm, rq->curr->prio);
#endif
	set_tsk_need_released(rq->curr);
	set_tsk_need_resched(rq->curr);
}

static DEFINE_PER_CPU(int, mt_need_released);
static int find_highest_prio_in_LITTLE(struct rq *this_rq, int pull)
{
	int cpu, prio, this_cpu = this_rq->cpu, highest_prio;
	struct rq *rq = NULL;
	struct cpumask *lowest_mask = __get_cpu_var(local_cpu_mask);

	highest_prio = MAX_RT_PRIO;
	cpumask_clear(lowest_mask);

#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("0. find_highest_prio_in_LITTLE %lu %d:%d %d",
		hmp_slow_cpu_mask.bits[0],
		this_rq->cpu, this_rq->rt.highest_prio.curr,
		pull);
#endif
	for_each_cpu(cpu, &hmp_slow_cpu_mask){
#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("1. find_highest_prio_in_LITTLE %d %d", cpu, cpu_online(cpu));
#endif
		if (!cpu_online(cpu))
			continue;

		rq = cpu_rq(cpu);
		if(rq->rt.rt_nr_running == 0)
			continue;

		prio = rq->rt.highest_prio.curr;

#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("2. find_highest_prio_in_LITTLE %d %d %d %lu", 
			cpu, prio, highest_prio, tsk_cpus_allowed(rq->curr)->bits[0]);
#endif

		/*                                                                          
                
   */
		if (prio >= this_rq->rt.highest_prio.curr )
			continue;

		/*                                                                              */
		if (prio > highest_prio)
			continue;

		/*                    */
		if (!cpumask_test_cpu(this_rq->cpu, tsk_cpus_allowed(rq->curr)))
			continue;

		if (prio < highest_prio){
			if ( 0 == pull ){
#ifdef CONFIG_MT_RT_SCHED_INFO
				mt_rt_printf("3. find_highest_prio_in_LITTLE find");
#endif
				return 1;
			}

			highest_prio = prio;
			cpumask_clear(lowest_mask);
		}

		cpumask_set_cpu(cpu, lowest_mask);

#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("2. find_highest_prio_in_LITTLE %d:%d %d %lu",
			cpu, prio, highest_prio, lowest_mask->bits[0]);
#endif
	}

	if (cpumask_empty(lowest_mask)){
#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("3. find_highest_prio_in_LITTLE not find");
#endif
		return 0;
	}

	raw_spin_unlock_irq(&this_rq->lock);
	per_cpu(mt_need_released, this_cpu) = 1;
	for_each_cpu (cpu, lowest_mask) {
		rq = cpu_rq(cpu);
#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("4. find_highest_prio_in_LITTLE %d %d", 
			cpu, rq->rt.highest_prio.curr);
#endif
		if (highest_prio == rq->rt.highest_prio.curr) {
			/*                  */
#if defined (CONFIG_MT_RT_SCHED_NOTICE)
			mt_rt_printf("send ipi release to cpu=%d prio=%d", 
				cpu, rq->rt.highest_prio.curr);
#endif
			/*                                               */
			smp_call_function_single(cpu, release_task_ipi, (void *)this_cpu, 0);
		 	break;
		}
	}

	raw_spin_lock_irq(&this_rq->lock);
	return 1;
}

static int find_lowest_rq_in_big(struct task_struct *task, struct cpumask *lowest_mask)
{
	int i, lowest_prio = 0;
	struct rq *rq = NULL;

	cpumask_clear(lowest_mask);
#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("0. find_lowest_rq_in_big %lu %d:%s:%d",
		(unsigned long)hmp_fast_cpu_mask.bits[0],
		task->pid, task->comm, task->prio);
#endif

	for_each_cpu(i, &hmp_fast_cpu_mask){
		int prio;

		if (!cpu_online(i))
			continue;

		rq = cpu_rq(i);
		prio = rq->rt.highest_prio.curr;

#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("1. find_lowest_rq_in_big %d:%d %d:%lu",
			i, prio,
			lowest_prio, (unsigned long)lowest_mask->bits[0]);
#endif

		/*                                                          
                                         
   */
		if ((prio < lowest_prio) || (prio <= task->prio))
			continue;

		if (!cpumask_test_cpu(i, tsk_cpus_allowed(task)))
			continue;

		/*                                        */
		if (prio > lowest_prio){
			lowest_prio = prio;
			cpumask_clear(lowest_mask);
		}

		cpumask_set_cpu(i, lowest_mask);
	}

	if (cpumask_empty(lowest_mask)){
#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("2. find_lowest_rq_in_big not find");
#endif
		return 0;
	}

#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("3. find_lowest_rq_in_big find %d:%s:%d %d:%lu",
		task->pid, task->comm, task->prio, 
		lowest_prio, (unsigned long)lowest_mask->bits[0]);
#endif
	return 1;
}

static int find_lowest_rq_in_LITTLE(struct task_struct *task, struct cpumask *lowest_mask)
{
	int i, lowest_prio = 0;
	struct rq *rq = NULL;

	cpumask_clear(lowest_mask);
#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("0. find_lowest_rq_in_LITTLE %lu %d:%s:%d", 
		(unsigned long)hmp_slow_cpu_mask.bits[0],
		task->pid, task->comm, task->prio);
#endif

	for_each_cpu(i, &hmp_slow_cpu_mask){
		int prio;

		if (!cpu_online(i))
			continue;

		rq = cpu_rq(i);
		prio = rq->rt.highest_prio.curr;

#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("1. find_lowest_rq_in_LITTLE %d:%d %d:%lu",
			i, prio,
			lowest_prio, (unsigned long)lowest_mask->bits[0]);
#endif

                /*                                                          
                                                       
   */
		if ((prio < lowest_prio) || (prio <= task->prio))
			continue;

		if (!cpumask_test_cpu(i, tsk_cpus_allowed(task)))
			continue;

		/*                                        */
		if (prio > lowest_prio){
			lowest_prio = prio;
			cpumask_clear(lowest_mask);
		}

		cpumask_set_cpu(i, lowest_mask);
	}

	if (cpumask_empty(lowest_mask)){
#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("2. find_lowest_rq_in_LITTLE not find");
#endif
		return 0;
	}

#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("3. find_lowest_rq_in_LITTLE find %d:%s:%d %d:%lu",
		task->pid, task->comm, task->prio,
		lowest_prio, (unsigned long)lowest_mask->bits[0]);
#endif

	return 1;
}

#endif

static int find_lowest_rq(struct task_struct *task)
{
	struct sched_domain *sd;
	struct cpumask *lowest_mask = __get_cpu_var(local_cpu_mask);
	int this_cpu = smp_processor_id();
	int cpu      = task_cpu(task);

#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("0. find_lowest_rq lowest_mask=%lu, task->nr_cpus_allowed=%d", 
		lowest_mask->bits[0], task->nr_cpus_allowed );
#endif
	/*                                         */
	if (unlikely(!lowest_mask))
		return -1;

	if (task->nr_cpus_allowed == 1)
		return -1; /*                           */

#ifdef CONFIG_MT_RT_SCHED
	if (!find_lowest_rq_in_big(task, lowest_mask)){
		if (!find_lowest_rq_in_LITTLE(task, lowest_mask)){
			return -1; /*                  */
		}
	}
#else
	if (!cpupri_find(&task_rq(task)->rd->cpupri, task, lowest_mask))
		return -1; /*                  */
#endif

#ifdef CONFIG_MT_RT_SCHED_NOTICE
	mt_rt_printf("find_lowest_rq %d:%s:%d %lu", 
		task->pid, task->comm, task->prio,
		lowest_mask->bits[0]);
#endif

	/*
                                                               
                                                              
                                                    
   
                                                              
                                                 
  */
	if (cpumask_test_cpu(cpu, lowest_mask))
		return cpu;

	/*
                                                               
                                                             
  */
	if (!cpumask_test_cpu(this_cpu, lowest_mask))
		this_cpu = -1; /*                                       */

	rcu_read_lock();
	for_each_domain(cpu, sd) {
		if (sd->flags & SD_WAKE_AFFINE) {
			int best_cpu;

			/*
                                             
                       
    */
			if (this_cpu != -1 &&
			    cpumask_test_cpu(this_cpu, sched_domain_span(sd))) {
				rcu_read_unlock();
				return this_cpu;
			}

			best_cpu = cpumask_first_and(lowest_mask,
						     sched_domain_span(sd));
			if (best_cpu < nr_cpu_ids) {
				rcu_read_unlock();
				return best_cpu;
			}
		}
	}
	rcu_read_unlock();

	/*
                                                            
                                                                     
              
  */
	if (this_cpu != -1)
		return this_cpu;

	cpu = cpumask_any(lowest_mask);
	if (cpu < nr_cpu_ids)
		return cpu;
	return -1;
}

/*                           */
static struct rq *find_lock_lowest_rq(struct task_struct *task, struct rq *rq)
{
	struct rq *lowest_rq = NULL;
	int tries;
	int cpu;

	for (tries = 0; tries < RT_MAX_TRIES; tries++) {
		cpu = find_lowest_rq(task);

		if ((cpu == -1) || (cpu == rq->cpu))
			break;

		lowest_rq = cpu_rq(cpu);

		/*                                                 */
		if (double_lock_balance(rq, lowest_rq)) {
			/*
                                        
                                    
                                                   
                                                        
    */

#ifdef CONFIG_MT_RT_SCHED_INFO
			mt_rt_printf("1. find_lock_lowest_rq %d %d %d %s", 
				lowest_rq->cpu,  rq->cpu, task->pid, task->comm);
#endif
			if (unlikely(task_rq(task) != rq ||
				     !cpumask_test_cpu(lowest_rq->cpu,
						       tsk_cpus_allowed(task)) ||
				     task_running(rq, task) ||
				     !task->on_rq)) {

				double_unlock_balance(rq, lowest_rq);
				lowest_rq = NULL;
				break;
			}
		}

		/*                                      */
		if (lowest_rq->rt.highest_prio.curr > task->prio)
			break;

		/*           */
		double_unlock_balance(rq, lowest_rq);
		lowest_rq = NULL;
	}

	return lowest_rq;
}

static struct task_struct *pick_next_pushable_task(struct rq *rq)
{
	struct task_struct *p;

	if (!has_pushable_tasks(rq))
		return NULL;

	p = plist_first_entry(&rq->rt.pushable_tasks,
			      struct task_struct, pushable_tasks);

	BUG_ON(rq->cpu != task_cpu(p));
	BUG_ON(task_current(rq, p));
	BUG_ON(p->nr_cpus_allowed <= 1);

	BUG_ON(!p->on_rq);
	BUG_ON(!rt_task(p));

	return p;
}

#ifdef CONFIG_MT_RT_SCHED
/*                           */
/*                             */
static struct rq *find_lock_lowest_rq_mtk(struct task_struct *task, struct rq *rq)
{
	struct rq *lowest_rq = NULL;
	int cpu;

	cpu = find_lowest_rq(task);

	if ((cpu == -1) || (cpu == rq->cpu))
		return NULL;

	lowest_rq = cpu_rq(cpu);

	/*                                                 */
	if (double_lock_balance(rq, lowest_rq)) {
		/*
                                       
                                   
                                                  
                                                       
   */
#ifdef CONFIG_MT_RT_SCHED_INFO		 
		mt_rt_printf("1. find_lock_lowest_rq_mtk %d %d %d %s", 
			lowest_rq->cpu,  rq->cpu, task->pid, task->comm);
#endif		
		if (unlikely(task_rq(task) != rq ||
			     !cpumask_test_cpu(lowest_rq->cpu,
					       tsk_cpus_allowed(task)) ||
			     task_running(rq, task) ||
			     !task->on_rq)) {
			double_unlock_balance(rq, lowest_rq);
			return NULL;
		}
	}

	/*                                      */
	if (lowest_rq->rt.highest_prio.curr > task->prio){
		return lowest_rq;
	}

	double_unlock_balance(rq, lowest_rq);
	return NULL;
}
#endif


#ifdef CONFIG_MT_RT_SCHED
/*                      */
int push_need_released_rt_task(struct rq *rq, struct task_struct *p)
{
	struct rq *lowest_rq;
	int ret = 0;

	if (!p)
		return 0;

#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("0. push_need_released_task %d:%s %d:%s",
		p->pid, p->comm, rq->curr->pid, rq->curr->comm);
#endif

	if (unlikely(p == rq->curr)) {
		WARN_ON(1);
		return 0;
	}

	/*                          */
	get_task_struct(p);

	/*                                           */
	lowest_rq = find_lock_lowest_rq_mtk(p, rq);
	if (!lowest_rq) {
#ifdef CONFIG_MT_RT_SCHED_NOTICE
		mt_rt_printf("1. push_need_released_task fail %d:%s:%d %d",
			p->pid, p->comm, p->prio, rq->curr->prio);
#endif	
		put_task_struct(p);

		if (likely(p->prio < rq->curr->prio)) {
			resched_task(rq->curr);
		}else{
#ifdef CONFIG_MT_RT_SCHED_NOTICE
			mt_rt_printf("1. push_need_released_task fail %d:%s:%d %d",
				p->pid, p->comm, p->prio, rq->curr->prio);
#if 0
			printk(KERN_ALERT "[sched] push_need_released_task fail %d:%s:%d %d\n",
				p->pid, p->comm, p->prio, rq->curr->prio);
			WARN_ON(1);
#endif
#endif	
		}
		return 0;
	}

#ifdef CONFIG_MT_RT_SCHED_NOTICE
	mt_rt_printf("push_need_released_task task=%d:%s cpu=%d",
		p->pid, p->comm, lowest_rq->cpu);
#endif	

	deactivate_task(rq, p, 0);
	set_task_cpu(p, lowest_rq->cpu);
	activate_task(lowest_rq, p, 0);
	ret = 1;

	resched_task(lowest_rq->curr);

	double_unlock_balance(rq, lowest_rq);

	put_task_struct(p);

	return ret;
}
#endif

/*
                                                               
                                                                
                      
 */
static int push_rt_task(struct rq *rq)
{
	struct task_struct *next_task;
	struct rq *lowest_rq;
	int ret = 0;

	if (!rq->rt.overloaded)
		return 0;

	next_task = pick_next_pushable_task(rq);
	if (!next_task)
		return 0;

retry:
	if (unlikely(next_task == rq->curr)) {
		WARN_ON(1);
		return 0;
	}

	/*
                                                  
                                                    
                            
  */
	if (unlikely(next_task->prio < rq->curr->prio)) {
		resched_task(rq->curr);
		return 0;
	}

	/*                          */
	get_task_struct(next_task);

	/*                                           */
	lowest_rq = find_lock_lowest_rq(next_task, rq);
	if (!lowest_rq) {
		struct task_struct *task;
		/*
                                          
                                                   
    
                                                            
                                                           
             
   */
		task = pick_next_pushable_task(rq);
		if (task_cpu(next_task) == rq->cpu && task == next_task) {
			/*
                                                     
                                                      
                                                      
                                              
    */
			goto out;
		}

		if (!task)
			/*                          */
			goto out;

		/*
                                      
   */
		put_task_struct(next_task);
		next_task = task;
		goto retry;
	}

	deactivate_task(rq, next_task, 0);
	set_task_cpu(next_task, lowest_rq->cpu);
	activate_task(lowest_rq, next_task, 0);
	ret = 1;

	resched_task(lowest_rq->curr);

	double_unlock_balance(rq, lowest_rq);

out:
	put_task_struct(next_task);

	return ret;
}

static void push_rt_tasks(struct rq *rq)
{
	/*                                                 */
	while (push_rt_task(rq))
		;
}

#ifdef CONFIG_MT_RT_SCHED
/*                      */
static int pick_next_highest_task(struct rq *this_rq){
	int this_cpu = this_rq->cpu, ret = 0, cpu;
	struct task_struct *p;
	struct rq *src_rq;

	for_each_cpu(cpu, this_rq->rd->rto_mask) {
		if (this_cpu == cpu)
			continue;

		src_rq = cpu_rq(cpu);

		/*
                                                             
                                                              
                                                         
                                                           
                                                     
   */
		if (src_rq->rt.highest_prio.next >=
		    this_rq->rt.highest_prio.curr)
			continue;

		/*
                                              
                                               
                  
   */
		double_lock_balance(this_rq, src_rq);

		/*
                                       
   */
		if (src_rq->rt.rt_nr_running <= 1)
			goto skip;

		p = pick_next_highest_task_rt(src_rq, this_cpu);

		/*
                                        
                              
   */
		if (p && (p->prio < this_rq->rt.highest_prio.curr)) {
			WARN_ON(p == src_rq->curr);
			WARN_ON(!p->on_rq);

			/*
                                                   
                                               
                                                  
                                            
                                           
                                   
    */
			if (p->prio < src_rq->curr->prio)
				goto skip;

			ret = 1;

			deactivate_task(src_rq, p, 0);
			set_task_cpu(p, this_cpu);
			activate_task(this_rq, p, 0);
			/*
                                          
                                           
                                          
                   
    */
#ifdef CONFIG_MT_RT_SCHED_INFO
			mt_rt_printf("pick_next_highest_task %d:%d %d %d:%s:%d\n",
				this_rq->cpu, this_rq->rt.highest_prio.curr,
				src_rq->cpu,
				p->pid, p->comm, p->prio);
#endif
		}
skip:
		double_unlock_balance(this_rq, src_rq);
	}

	return ret;
}

void mt_check_rt_policy(struct rq *this_rq)
{
	int this_cpu = this_rq->cpu;
	if ( cpumask_test_cpu(this_cpu, &hmp_fast_cpu_mask) ){
		if ( !per_cpu(mt_need_released, this_cpu) )
			return;
#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("0. mt_check_rt_policy %d %d %s", 
			this_cpu, this_rq->curr->pid, this_rq->curr->comm );
#endif

		if ( find_highest_prio_in_LITTLE(this_rq, 0) ){
			set_tsk_need_resched(this_rq->curr);
#ifdef CONFIG_MT_RT_SCHED_INFO
			mt_rt_printf("1. mt_check_rt_policy %d %d %s", 
				this_cpu, this_rq->curr->pid, this_rq->curr->comm );
#endif
		}else{
			per_cpu(mt_need_released, this_cpu)=0;
#ifdef CONFIG_MT_RT_SCHED_INFO
			mt_rt_printf("2. mt_check_rt_policy %d", this_cpu);
#endif
		}
	}
}

int mt_post_schedule(struct rq *rq)
{
	int this_cpu = rq->cpu, ret = 0;
	unsigned long flags;

	raw_spin_lock_irqsave(&rq->lock, flags);
	if ( cpumask_test_cpu(this_cpu, &hmp_fast_cpu_mask) ) {
		if ( has_rt_task_in_little() )
			ret = find_highest_prio_in_LITTLE(rq, 1);
	}
	raw_spin_unlock_irqrestore(&rq->lock, flags);

	return ret;
}
#endif

#ifdef CONFIG_MT_RT_SCHED
int pull_rt_task(struct rq *this_rq)
#else
static int pull_rt_task(struct rq *this_rq)
#endif
{
#if defined(CONFIG_MT_RT_SCHED_INFO) || !defined(CONFIG_MT_RT_SCHED)	
	int this_cpu = this_rq->cpu;
#endif	
	int ret = 0;
#ifndef CONFIG_MT_RT_SCHED
	int cpu;
	struct task_struct *p;
	struct rq *src_rq;
#endif

#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("0. pull_rt_task %d %d %lu",
		rt_overloaded(this_rq), this_cpu, hmp_fast_cpu_mask.bits[0]);
#endif

#ifdef CONFIG_MT_RT_SCHED
	if (likely(!rt_overloaded(this_rq)))
		return 0;
	ret = pick_next_highest_task(this_rq);
#else
	if (likely(!rt_overloaded(this_rq)))
		return 0;

#ifdef CONFIG_MT_RT_SCHED_INFO
	mt_rt_printf("1. pull_rt_task %lu ",
		this_rq->rd->rto_mask->bits[0]);
#endif
	for_each_cpu(cpu, this_rq->rd->rto_mask) {
		if (this_cpu == cpu)
			continue;

		src_rq = cpu_rq(cpu);

		/*
                                                             
                                                              
                                                         
                                                           
                                                     
   */
#ifdef CONFIG_MT_RT_SCHED_INFO
		mt_rt_printf("2. pull_rt_task %d %d ",
			src_rq->rt.highest_prio.next, this_rq->rt.highest_prio.curr);
#endif
		if (src_rq->rt.highest_prio.next >=
		    this_rq->rt.highest_prio.curr)
			continue;

		/*
                                              
                                               
                  
   */
		double_lock_balance(this_rq, src_rq);

		/*
                                       
   */
		if (src_rq->rt.rt_nr_running <= 1)
			goto skip;

		p = pick_next_highest_task_rt(src_rq, this_cpu);

		/*
                                        
                              
   */
		if (p && (p->prio < this_rq->rt.highest_prio.curr)) {
			WARN_ON(p == src_rq->curr);
			WARN_ON(!p->on_rq);

			/*
                                                   
                                               
                                                  
                                            
                                           
                                   
    */
			if (p->prio < src_rq->curr->prio)
				goto skip;

			ret = 1;

			deactivate_task(src_rq, p, 0);
			set_task_cpu(p, this_cpu);
			activate_task(this_rq, p, 0);
			/*
                                          
                                           
                                          
                   
    */
		}
skip:
		double_unlock_balance(this_rq, src_rq);
	}
#endif

	return ret;
}

static void pre_schedule_rt(struct rq *rq, struct task_struct *prev)
{
	/*                                                      */
	if (rq->rt.highest_prio.curr > prev->prio)
		pull_rt_task(rq);
}

static void post_schedule_rt(struct rq *rq)
{
	push_rt_tasks(rq);
}

/*
                                                                           
                             
 */
static void task_woken_rt(struct rq *rq, struct task_struct *p)
{
	if (!task_running(rq, p) &&
	    !test_tsk_need_resched(rq->curr) &&
	    has_pushable_tasks(rq) &&
	    p->nr_cpus_allowed > 1 &&
	    rt_task(rq->curr) &&
	    (rq->curr->nr_cpus_allowed < 2 ||
	     rq->curr->prio <= p->prio))
		push_rt_tasks(rq);
}

static void set_cpus_allowed_rt(struct task_struct *p,
				const struct cpumask *new_mask)
{
	struct rq *rq;
	int weight;

	BUG_ON(!rt_task(p));

	if (!p->on_rq)
		return;

	weight = cpumask_weight(new_mask);

	/*
                                                                
                       
  */
	if ((p->nr_cpus_allowed > 1) == (weight > 1))
		return;

	rq = task_rq(p);

	/*
                                                                
  */
	if (weight <= 1) {
		if (!task_current(rq, p))
			dequeue_pushable_task(rq, p);
		BUG_ON(!rq->rt.rt_nr_migratory);
		rq->rt.rt_nr_migratory--;
	} else {
		if (!task_current(rq, p))
			enqueue_pushable_task(rq, p);
		rq->rt.rt_nr_migratory++;
	}

	update_rt_migration(&rq->rt);
}

/*                          */
static void rq_online_rt(struct rq *rq)
{
	if (rq->rt.overloaded)
		rt_set_overload(rq);

	__enable_runtime(rq);

	cpupri_set(&rq->rd->cpupri, rq->cpu, rq->rt.highest_prio.curr);
}

/*                          */
static void rq_offline_rt(struct rq *rq)
{
	if (rq->rt.overloaded)
		rt_clear_overload(rq);

	__disable_runtime(rq);

	cpupri_set(&rq->rd->cpupri, rq->cpu, CPUPRI_INVALID);
}

/*
                                                                  
                                                            
 */
static void switched_from_rt(struct rq *rq, struct task_struct *p)
{
	/*
                                                       
                                                        
                                                 
                                                 
        
  */
	if (!p->on_rq || rq->rt.rt_nr_running)
		return;

	if (pull_rt_task(rq))
		resched_task(rq->curr);
}

void init_sched_rt_class(void)
{
	unsigned int i;

	for_each_possible_cpu(i) {
		zalloc_cpumask_var_node(&per_cpu(local_cpu_mask, i),
					GFP_KERNEL, cpu_to_node(i));
	}
}
#endif /*            */

/*
                                                            
                                                         
                   
 */
static void switched_to_rt(struct rq *rq, struct task_struct *p)
{
	int check_resched = 1;

	/*
                                                   
                                                    
                                                    
                                                   
                                                 
  */
	if (p->on_rq && rq->curr != p) {
#ifdef CONFIG_SMP
		if (rq->rt.overloaded && push_rt_task(rq) &&
		    /*                                       */
		    rq != task_rq(p))
			check_resched = 0;
#endif /*            */
		if (check_resched && p->prio < rq->curr->prio)
			resched_task(rq->curr);
	}
}

/*
                                                   
                                 
 */
static void
prio_changed_rt(struct rq *rq, struct task_struct *p, int oldprio)
{
	if (!p->on_rq)
		return;

	if (rq->curr == p) {
#ifdef CONFIG_SMP
		/*
                                                
                                             
   */
		if (oldprio < p->prio)
			pull_rt_task(rq);
		/*
                                                     
                                                  
                                                 
                                                        
   */
		if (p->prio > rq->rt.highest_prio.curr && rq->curr == p)
			resched_task(p);
#else
		/*                                       */
		if (oldprio < p->prio)
			resched_task(p);
#endif /*            */
	} else {
		/*
                                           
                                          
                     
   */
		if (p->prio < rq->curr->prio)
			resched_task(rq->curr);
	}
}

static void watchdog(struct rq *rq, struct task_struct *p)
{
	unsigned long soft, hard;

	/*                                                                 */
	soft = task_rlimit(p, RLIMIT_RTTIME);
	hard = task_rlimit_max(p, RLIMIT_RTTIME);

	if (soft != RLIM_INFINITY) {
		unsigned long next;

		if (p->rt.watchdog_stamp != jiffies) {
			p->rt.timeout++;
			p->rt.watchdog_stamp = jiffies;
		}

		next = DIV_ROUND_UP(min(soft, hard), USEC_PER_SEC/HZ);
		if (p->rt.timeout > next)
			p->cputime_expires.sched_exp = p->se.sum_exec_runtime;
	}
}

static void task_tick_rt(struct rq *rq, struct task_struct *p, int queued)
{
	struct sched_rt_entity *rt_se = &p->rt;

	update_curr_rt(rq);

	watchdog(rq, p);

	/*
                                                         
                                  
  */
	if (p->policy != SCHED_RR)
		return;

	if (--p->rt.time_slice)
		return;

	p->rt.time_slice = sched_rr_timeslice;

	/*
                                                                        
                             
  */
	for_each_sched_rt_entity(rt_se) {
		if (rt_se->run_list.prev != rt_se->run_list.next) {
			requeue_task_rt(rq, p, 0);
			set_tsk_need_resched(p);
			return;
		}
	}
}

static void set_curr_task_rt(struct rq *rq)
{
	struct task_struct *p = rq->curr;

	p->se.exec_start = rq->clock_task;

	/*                                                */
	dequeue_pushable_task(rq, p);
}

static unsigned int get_rr_interval_rt(struct rq *rq, struct task_struct *task)
{
	/*
                                        
  */
	if (task->policy == SCHED_RR)
		return sched_rr_timeslice;
	else
		return 0;
}

const struct sched_class rt_sched_class = {
	.next			= &fair_sched_class,
	.enqueue_task		= enqueue_task_rt,
	.dequeue_task		= dequeue_task_rt,
	.yield_task		= yield_task_rt,

	.check_preempt_curr	= check_preempt_curr_rt,

	.pick_next_task		= pick_next_task_rt,
	.put_prev_task		= put_prev_task_rt,

#ifdef CONFIG_SMP
	.select_task_rq		= select_task_rq_rt,

	.set_cpus_allowed       = set_cpus_allowed_rt,
	.rq_online              = rq_online_rt,
	.rq_offline             = rq_offline_rt,
	.pre_schedule		= pre_schedule_rt,
	.post_schedule		= post_schedule_rt,
	.task_woken		= task_woken_rt,
	.switched_from		= switched_from_rt,
#endif

	.set_curr_task          = set_curr_task_rt,
	.task_tick		= task_tick_rt,

	.get_rr_interval	= get_rr_interval_rt,

	.prio_changed		= prio_changed_rt,
	.switched_to		= switched_to_rt,
};

#ifdef CONFIG_SCHED_DEBUG
extern void print_rt_rq(struct seq_file *m, int cpu, struct rt_rq *rt_rq);

void print_rt_stats(struct seq_file *m, int cpu)
{
	rt_rq_iter_t iter;
	struct rt_rq *rt_rq;

	rcu_read_lock();
	for_each_rt_rq(rt_rq, iter, cpu_rq(cpu))
		print_rt_rq(m, cpu, rt_rq);
	rcu_read_unlock();
}
#endif /*                    */
