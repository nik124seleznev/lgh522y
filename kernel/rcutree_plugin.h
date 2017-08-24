/*
 * Read-Copy Update mechanism for mutual exclusion (tree-based version)
 * Internal non-public definitions that provide either classic
 * or preemptible semantics.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright Red Hat, 2009
 * Copyright IBM Corporation, 2009
 *
 * Author: Ingo Molnar <mingo@elte.hu>
 *	   Paul E. McKenney <paulmck@linux.vnet.ibm.com>
 */

#include <linux/delay.h>
#include <linux/gfp.h>
#include <linux/oom.h>
#include <linux/smpboot.h>
#include <linux/tick.h>

#define RCU_KTHREAD_PRIO 1

#ifdef CONFIG_RCU_BOOST
#define RCU_BOOST_PRIO CONFIG_RCU_BOOST_PRIO
#else
#define RCU_BOOST_PRIO RCU_KTHREAD_PRIO
#endif

#ifdef CONFIG_RCU_NOCB_CPU
static cpumask_var_t rcu_nocb_mask; /*                                   */
static bool have_rcu_nocb_mask;	    /*                              */
static bool __read_mostly rcu_nocb_poll;    /*                              */
static char __initdata nocb_buf[NR_CPUS * 5];
#endif /*                            */

/*
                                                                      
                                                                        
                           
 */
static void __init rcu_bootup_announce_oddness(void)
{
#ifdef CONFIG_RCU_TRACE
	printk(KERN_INFO "\tRCU debugfs-based tracing is enabled.\n");
#endif
#if (defined(CONFIG_64BIT) && CONFIG_RCU_FANOUT != 64) || (!defined(CONFIG_64BIT) && CONFIG_RCU_FANOUT != 32)
	printk(KERN_INFO "\tCONFIG_RCU_FANOUT set to non-default value of %d\n",
	       CONFIG_RCU_FANOUT);
#endif
#ifdef CONFIG_RCU_FANOUT_EXACT
	printk(KERN_INFO "\tHierarchical RCU autobalancing is disabled.\n");
#endif
#ifdef CONFIG_RCU_FAST_NO_HZ
	printk(KERN_INFO
	       "\tRCU dyntick-idle grace-period acceleration is enabled.\n");
#endif
#ifdef CONFIG_PROVE_RCU
	printk(KERN_INFO "\tRCU lockdep checking is enabled.\n");
#endif
#ifdef CONFIG_RCU_TORTURE_TEST_RUNNABLE
	printk(KERN_INFO "\tRCU torture testing starts during boot.\n");
#endif
#if defined(CONFIG_TREE_PREEMPT_RCU) && !defined(CONFIG_RCU_CPU_STALL_VERBOSE)
	printk(KERN_INFO "\tDump stacks of tasks blocking RCU-preempt GP.\n");
#endif
#if defined(CONFIG_RCU_CPU_STALL_INFO)
	printk(KERN_INFO "\tAdditional per-CPU info printed with stalls.\n");
#endif
#if NUM_RCU_LVL_4 != 0
	printk(KERN_INFO "\tFour-level hierarchy is enabled.\n");
#endif
	if (rcu_fanout_leaf != CONFIG_RCU_FANOUT_LEAF)
		printk(KERN_INFO "\tExperimental boot-time adjustment of leaf fanout to %d.\n", rcu_fanout_leaf);
	if (nr_cpu_ids != NR_CPUS)
		printk(KERN_INFO "\tRCU restricting CPUs from NR_CPUS=%d to nr_cpu_ids=%d.\n", NR_CPUS, nr_cpu_ids);
#ifdef CONFIG_RCU_NOCB_CPU
#ifndef CONFIG_RCU_NOCB_CPU_NONE
	if (!have_rcu_nocb_mask) {
		zalloc_cpumask_var(&rcu_nocb_mask, GFP_KERNEL);
		have_rcu_nocb_mask = true;
	}
#ifdef CONFIG_RCU_NOCB_CPU_ZERO
	pr_info("\tExperimental no-CBs CPU 0\n");
	cpumask_set_cpu(0, rcu_nocb_mask);
#endif /*                                 */
#ifdef CONFIG_RCU_NOCB_CPU_ALL
	pr_info("\tExperimental no-CBs for all CPUs\n");
	cpumask_setall(rcu_nocb_mask);
#endif /*                                */
#endif /*                                  */
	if (have_rcu_nocb_mask) {
		cpulist_scnprintf(nocb_buf, sizeof(nocb_buf), rcu_nocb_mask);
		pr_info("\tExperimental no-CBs CPUs: %s.\n", nocb_buf);
		if (rcu_nocb_poll)
			pr_info("\tExperimental polled no-CBs CPUs.\n");
	}
#endif /*                            */
}

#ifdef CONFIG_TREE_PREEMPT_RCU

struct rcu_state rcu_preempt_state =
	RCU_STATE_INITIALIZER(rcu_preempt, 'p', call_rcu);
DEFINE_PER_CPU(struct rcu_data, rcu_preempt_data);
static struct rcu_state *rcu_state = &rcu_preempt_state;

static int rcu_preempted_readers_exp(struct rcu_node *rnp);

/*
                                       
 */
static void __init rcu_bootup_announce(void)
{
	printk(KERN_INFO "Preemptible hierarchical RCU implementation.\n");
	rcu_bootup_announce_oddness();
}

/*
                                                              
                            
 */
long rcu_batches_completed_preempt(void)
{
	return rcu_preempt_state.completed;
}
EXPORT_SYMBOL_GPL(rcu_batches_completed_preempt);

/*
                                                                         
 */
long rcu_batches_completed(void)
{
	return rcu_batches_completed_preempt();
}
EXPORT_SYMBOL_GPL(rcu_batches_completed);

/*
                                               
 */
void rcu_force_quiescent_state(void)
{
	force_quiescent_state(&rcu_preempt_state);
}
EXPORT_SYMBOL_GPL(rcu_force_quiescent_state);

/*
                                                                        
                                                                     
                                                                        
                                              
  
                                                                  
                                                          
                             
 */
static void rcu_preempt_qs(int cpu)
{
	struct rcu_data *rdp = &per_cpu(rcu_preempt_data, cpu);

	if (rdp->passed_quiesce == 0)
		trace_rcu_grace_period("rcu_preempt", rdp->gpnum, "cpuqs");
	rdp->passed_quiesce = 1;
	current->rcu_read_unlock_special &= ~RCU_READ_UNLOCK_NEED_QS;
}

/*
                                                                    
                                                                   
                                                                    
                                                                   
                                                                     
                                                                       
                                                                    
                                                                  
                              
  
                                  
 */
static void rcu_preempt_note_context_switch(int cpu)
{
	struct task_struct *t = current;
	unsigned long flags;
	struct rcu_data *rdp;
	struct rcu_node *rnp;

	if (t->rcu_read_lock_nesting > 0 &&
	    (t->rcu_read_unlock_special & RCU_READ_UNLOCK_BLOCKED) == 0) {

		/*                                                         */
		rdp = per_cpu_ptr(rcu_preempt_state.rda, cpu);
		rnp = rdp->mynode;
		raw_spin_lock_irqsave(&rnp->lock, flags);
		t->rcu_read_unlock_special |= RCU_READ_UNLOCK_BLOCKED;
		t->rcu_blocked_node = rnp;

		/*
                                                       
                                                       
                                                       
                                                       
                                                           
                                                      
                                                          
                                                        
                                                      
                                                      
                                                        
                                                          
              
    
                                                       
             
   */
		WARN_ON_ONCE((rdp->grpmask & rnp->qsmaskinit) == 0);
		WARN_ON_ONCE(!list_empty(&t->rcu_node_entry));
		if ((rnp->qsmask & rdp->grpmask) && rnp->gp_tasks != NULL) {
			list_add(&t->rcu_node_entry, rnp->gp_tasks->prev);
			rnp->gp_tasks = &t->rcu_node_entry;
#ifdef CONFIG_RCU_BOOST
			if (rnp->boost_tasks != NULL)
				rnp->boost_tasks = rnp->gp_tasks;
#endif /*                         */
		} else {
			list_add(&t->rcu_node_entry, &rnp->blkd_tasks);
			if (rnp->qsmask & rdp->grpmask)
				rnp->gp_tasks = &t->rcu_node_entry;
		}
		trace_rcu_preempt_task(rdp->rsp->name,
				       t->pid,
				       (rnp->qsmask & rdp->grpmask)
				       ? rnp->gpnum
				       : rnp->gpnum + 1);
		raw_spin_unlock_irqrestore(&rnp->lock, flags);
	} else if (t->rcu_read_lock_nesting < 0 &&
		   t->rcu_read_unlock_special) {

		/*
                                                         
                                                         
   */
		rcu_read_unlock_special(t);
	}

	/*
                                                              
                                                             
                                                            
                                                                 
                                                                  
                                                               
                                                             
  */
	local_irq_save(flags);
	rcu_preempt_qs(cpu);
	local_irq_restore(flags);
}

/*
                                                                    
                                                                        
                                              
 */
static int rcu_preempt_blocked_readers_cgp(struct rcu_node *rnp)
{
	return rnp->gp_tasks != NULL;
}

/*
                                                                     
                                                                         
                                                                       
                                                                        
            
 */
static void rcu_report_unblock_qs_rnp(struct rcu_node *rnp, unsigned long flags)
	__releases(rnp->lock)
{
	unsigned long mask;
	struct rcu_node *rnp_p;

	if (rnp->qsmask != 0 || rcu_preempt_blocked_readers_cgp(rnp)) {
		raw_spin_unlock_irqrestore(&rnp->lock, flags);
		return;  /*                                   */
	}

	rnp_p = rnp->parent;
	if (rnp_p == NULL) {
		/*
                                                   
                                                    
                        
   */
		rcu_report_qs_rsp(&rcu_preempt_state, flags);
		return;
	}

	/*                                      */
	mask = rnp->grpmask;
	raw_spin_unlock(&rnp->lock);	/*                       */
	raw_spin_lock(&rnp_p->lock);	/*                        */
	rcu_report_qs_rnp(mask, &rcu_preempt_state, rnp_p, flags);
}

/*
                                                                 
                                            
 */
static struct list_head *rcu_next_node_entry(struct task_struct *t,
					     struct rcu_node *rnp)
{
	struct list_head *np;

	np = t->rcu_node_entry.next;
	if (np == &rnp->blkd_tasks)
		np = NULL;
	return np;
}

/*
                                                                    
                                                                   
                              
 */
void rcu_read_unlock_special(struct task_struct *t)
{
	int empty;
	int empty_exp;
	int empty_exp_now;
	unsigned long flags;
	struct list_head *np;
#ifdef CONFIG_RCU_BOOST
	struct rt_mutex *rbmp = NULL;
#endif /*                         */
	struct rcu_node *rnp;
	int special;

	/*                                                               */
	if (in_nmi())
		return;

	local_irq_save(flags);

	/*
                                                                 
                                     
  */
	special = t->rcu_read_unlock_special;
	if (special & RCU_READ_UNLOCK_NEED_QS) {
		rcu_preempt_qs(smp_processor_id());
	}

	/*                                     */
	if (in_irq() || in_serving_softirq()) {
		local_irq_restore(flags);
		return;
	}

	/*                                                            */
	if (special & RCU_READ_UNLOCK_BLOCKED) {
		t->rcu_read_unlock_special &= ~RCU_READ_UNLOCK_BLOCKED;

		/*
                                                       
                                                       
                                                        
   */
		for (;;) {
			rnp = t->rcu_blocked_node;
			raw_spin_lock(&rnp->lock);  /*                        */
			if (rnp == t->rcu_blocked_node)
				break;
			raw_spin_unlock(&rnp->lock); /*                       */
		}
		empty = !rcu_preempt_blocked_readers_cgp(rnp);
		empty_exp = !rcu_preempted_readers_exp(rnp);
		smp_mb(); /*                                                */
		np = rcu_next_node_entry(t, rnp);
		list_del_init(&t->rcu_node_entry);
		t->rcu_blocked_node = NULL;
		trace_rcu_unlock_preempted_task("rcu_preempt",
						rnp->gpnum, t->pid);
		if (&t->rcu_node_entry == rnp->gp_tasks)
			rnp->gp_tasks = np;
		if (&t->rcu_node_entry == rnp->exp_tasks)
			rnp->exp_tasks = np;
#ifdef CONFIG_RCU_BOOST
		if (&t->rcu_node_entry == rnp->boost_tasks)
			rnp->boost_tasks = np;
		/*                                                           */
		if (t->rcu_boost_mutex) {
			rbmp = t->rcu_boost_mutex;
			t->rcu_boost_mutex = NULL;
		}
#endif /*                         */

		/*
                                                          
                                                               
                                                              
                                                       
   */
		empty_exp_now = !rcu_preempted_readers_exp(rnp);
		if (!empty && !rcu_preempt_blocked_readers_cgp(rnp)) {
			trace_rcu_quiescent_state_report("preempt_rcu",
							 rnp->gpnum,
							 0, rnp->qsmask,
							 rnp->level,
							 rnp->grplo,
							 rnp->grphi,
							 !!rnp->gp_tasks);
			rcu_report_unblock_qs_rnp(rnp, flags);
		} else {
			raw_spin_unlock_irqrestore(&rnp->lock, flags);
		}

#ifdef CONFIG_RCU_BOOST
		/*                             */
		if (rbmp)
			rt_mutex_unlock(rbmp);
#endif /*                         */

		/*
                                                      
                                                      
   */
		if (!empty_exp && empty_exp_now)
			rcu_report_exp_rnp(&rcu_preempt_state, rnp, true);
	} else {
		local_irq_restore(flags);
	}
}

#ifdef CONFIG_RCU_CPU_STALL_VERBOSE

/*
                                                                   
                                                    
 */
static void rcu_print_detail_task_stall_rnp(struct rcu_node *rnp)
{
	unsigned long flags;
	struct task_struct *t;

	raw_spin_lock_irqsave(&rnp->lock, flags);
	if (!rcu_preempt_blocked_readers_cgp(rnp)) {
		raw_spin_unlock_irqrestore(&rnp->lock, flags);
		return;
	}
	t = list_entry(rnp->gp_tasks,
		       struct task_struct, rcu_node_entry);
	list_for_each_entry_continue(t, &rnp->blkd_tasks, rcu_node_entry)
		sched_show_task(t);
	raw_spin_unlock_irqrestore(&rnp->lock, flags);
}

/*
                                                                   
                
 */
static void rcu_print_detail_task_stall(struct rcu_state *rsp)
{
	struct rcu_node *rnp = rcu_get_root(rsp);

	rcu_print_detail_task_stall_rnp(rnp);
	rcu_for_each_leaf_node(rsp, rnp)
		rcu_print_detail_task_stall_rnp(rnp);
}

#else /*                                     */

static void rcu_print_detail_task_stall(struct rcu_state *rsp)
{
}

#endif /*                                           */

#ifdef CONFIG_RCU_CPU_STALL_INFO

static void rcu_print_task_stall_begin(struct rcu_node *rnp)
{
	printk(KERN_ERR "\tTasks blocked on level-%d rcu_node (CPUs %d-%d):",
	       rnp->level, rnp->grplo, rnp->grphi);
}

static void rcu_print_task_stall_end(void)
{
	printk(KERN_CONT "\n");
}

#else /*                                  */

static void rcu_print_task_stall_begin(struct rcu_node *rnp)
{
}

static void rcu_print_task_stall_end(void)
{
}

#endif /*                                        */

/*
                                                                       
                                          
 */
static int rcu_print_task_stall(struct rcu_node *rnp)
{
	struct task_struct *t;
	int ndetected = 0;

	if (!rcu_preempt_blocked_readers_cgp(rnp))
		return 0;
	rcu_print_task_stall_begin(rnp);
	t = list_entry(rnp->gp_tasks,
		       struct task_struct, rcu_node_entry);
	list_for_each_entry_continue(t, &rnp->blkd_tasks, rcu_node_entry) {
		printk(KERN_CONT " P%d", t->pid);
		ndetected++;
	}
	rcu_print_task_stall_end();
	return ndetected;
}

/*
                                                                     
                                                                    
                                                                    
                                                                     
                              
  
                                                                   
                                                                          
 */
static void rcu_preempt_check_blocked_tasks(struct rcu_node *rnp)
{
	WARN_ON_ONCE(rcu_preempt_blocked_readers_cgp(rnp));
	if (!list_empty(&rnp->blkd_tasks))
		rnp->gp_tasks = rnp->blkd_tasks.next;
	WARN_ON_ONCE(rnp->qsmask);
}

#ifdef CONFIG_HOTPLUG_CPU

/*
                                                                      
                                                                  
                                                                  
                                                                
                                                                     
                                                                  
          
  
                                                                      
                                              
  
                                                     
 */
static int rcu_preempt_offline_tasks(struct rcu_state *rsp,
				     struct rcu_node *rnp,
				     struct rcu_data *rdp)
{
	struct list_head *lp;
	struct list_head *lp_root;
	int retval = 0;
	struct rcu_node *rnp_root = rcu_get_root(rsp);
	struct task_struct *t;

	if (rnp == rnp_root) {
		WARN_ONCE(1, "Last CPU thought to be offlined?");
		return 0;  /*                                            */
	}

	/*                                                   */
	WARN_ON_ONCE(rnp != rdp->mynode);

	/*
                                                               
                                                            
                                                                   
                                                                
                                                                  
                                                                   
             
  */
	if (rcu_preempt_blocked_readers_cgp(rnp) && rnp->qsmask == 0)
		retval |= RCU_OFL_TASKS_NORM_GP;
	if (rcu_preempted_readers_exp(rnp))
		retval |= RCU_OFL_TASKS_EXP_GP;
	lp = &rnp->blkd_tasks;
	lp_root = &rnp_root->blkd_tasks;
	while (!list_empty(lp)) {
		t = list_entry(lp->next, typeof(*t), rcu_node_entry);
		raw_spin_lock(&rnp_root->lock); /*                       */
		list_del(&t->rcu_node_entry);
		t->rcu_blocked_node = rnp_root;
		list_add(&t->rcu_node_entry, lp_root);
		if (&t->rcu_node_entry == rnp->gp_tasks)
			rnp_root->gp_tasks = rnp->gp_tasks;
		if (&t->rcu_node_entry == rnp->exp_tasks)
			rnp_root->exp_tasks = rnp->exp_tasks;
#ifdef CONFIG_RCU_BOOST
		if (&t->rcu_node_entry == rnp->boost_tasks)
			rnp_root->boost_tasks = rnp->boost_tasks;
#endif /*                         */
		raw_spin_unlock(&rnp_root->lock); /*                     */
	}

	rnp->gp_tasks = NULL;
	rnp->exp_tasks = NULL;
#ifdef CONFIG_RCU_BOOST
	rnp->boost_tasks = NULL;
	/*
                                                              
                                                             
                 
  */
	raw_spin_lock(&rnp_root->lock); /*                       */
	if (rnp_root->boost_tasks != NULL &&
	    rnp_root->boost_tasks != rnp_root->gp_tasks &&
	    rnp_root->boost_tasks != rnp_root->exp_tasks)
		rnp_root->boost_tasks = rnp_root->gp_tasks;
	raw_spin_unlock(&rnp_root->lock); /*                     */
#endif /*                         */

	return retval;
}

#endif /*                           */

/*
                                                                         
                                                                      
                              
  
                                 
 */
static void rcu_preempt_check_callbacks(int cpu)
{
	struct task_struct *t = current;

	if (t->rcu_read_lock_nesting == 0) {
		rcu_preempt_qs(cpu);
		return;
	}
	if (t->rcu_read_lock_nesting > 0 &&
	    per_cpu(rcu_preempt_data, cpu).qs_pending)
		t->rcu_read_unlock_special |= RCU_READ_UNLOCK_NEED_QS;
}

#ifdef CONFIG_RCU_BOOST

static void rcu_preempt_do_callbacks(void)
{
	rcu_do_batch(&rcu_preempt_state, &__get_cpu_var(rcu_preempt_data));
}

#endif /*                         */

/*
                                                                        
 */
void call_rcu(struct rcu_head *head, void (*func)(struct rcu_head *rcu))
{
	__call_rcu(head, func, &rcu_preempt_state, -1, 0);
}
EXPORT_SYMBOL_GPL(call_rcu);

/*
                                                                  
                                                                    
                                                                
                                                                
                                                  
 */
void kfree_call_rcu(struct rcu_head *head,
		    void (*func)(struct rcu_head *rcu))
{
	__call_rcu(head, func, &rcu_preempt_state, -1, 1);
}
EXPORT_SYMBOL_GPL(kfree_call_rcu);

/* 
                                                           
  
                                                                 
                                                                       
                                                                   
                                                                         
                                                                         
                                                                      
                                                                         
  
                                                                           
                                 
 */
void synchronize_rcu(void)
{
	rcu_lockdep_assert(!lock_is_held(&rcu_bh_lock_map) &&
			   !lock_is_held(&rcu_lock_map) &&
			   !lock_is_held(&rcu_sched_lock_map),
			   "Illegal synchronize_rcu() in RCU read-side critical section");
	if (!rcu_scheduler_active)
		return;
	if (rcu_expedited)
		synchronize_rcu_expedited();
	else
		wait_rcu_gp(call_rcu);
}
EXPORT_SYMBOL_GPL(synchronize_rcu);

static DECLARE_WAIT_QUEUE_HEAD(sync_rcu_preempt_exp_wq);
static unsigned long sync_rcu_preempt_exp_count;
static DEFINE_MUTEX(sync_rcu_preempt_exp_mutex);

/*
                                                                   
                                                                        
                                                                     
                                          
 */
static int rcu_preempted_readers_exp(struct rcu_node *rnp)
{
	return rnp->exp_tasks != NULL;
}

/*
                                                                        
                                                                        
                                                                        
                                                                      
                                                   
  
                                               
 */
static int sync_rcu_preempt_exp_done(struct rcu_node *rnp)
{
	return !rcu_preempted_readers_exp(rnp) &&
	       ACCESS_ONCE(rnp->expmask) == 0;
}

/*
                                                                        
                                                                            
                                                                            
                                                                              
                                                                       
                
  
                                                                     
                                               
  
                                               
 */
static void rcu_report_exp_rnp(struct rcu_state *rsp, struct rcu_node *rnp,
			       bool wake)
{
	unsigned long flags;
	unsigned long mask;

	raw_spin_lock_irqsave(&rnp->lock, flags);
	for (;;) {
		if (!sync_rcu_preempt_exp_done(rnp)) {
			raw_spin_unlock_irqrestore(&rnp->lock, flags);
			break;
		}
		if (rnp->parent == NULL) {
			raw_spin_unlock_irqrestore(&rnp->lock, flags);
			if (wake)
				wake_up(&sync_rcu_preempt_exp_wq);
			break;
		}
		mask = rnp->grpmask;
		raw_spin_unlock(&rnp->lock); /*                      */
		rnp = rnp->parent;
		raw_spin_lock(&rnp->lock); /*                       */
		rnp->expmask &= ~mask;
	}
}

/*
                                                                          
                                                                           
                                              
  
                                                               
                          
 */
static void
sync_rcu_preempt_exp_init(struct rcu_state *rsp, struct rcu_node *rnp)
{
	unsigned long flags;
	int must_wait = 0;

	raw_spin_lock_irqsave(&rnp->lock, flags);
	if (list_empty(&rnp->blkd_tasks)) {
		raw_spin_unlock_irqrestore(&rnp->lock, flags);
	} else {
		rnp->exp_tasks = rnp->blkd_tasks.next;
		rcu_initiate_boost(rnp, flags);  /*                    */
		must_wait = 1;
	}
	if (!must_wait)
		rcu_report_exp_rnp(rsp, rnp, false); /*                  */
}

/* 
                                                           
  
                                                                    
                                                                           
                                                                         
                                                                         
                                                               
                                                                   
                                                                     
                                    
  
                                                                       
                                                                           
                                                                         
                                             
 */
void synchronize_rcu_expedited(void)
{
	unsigned long flags;
	struct rcu_node *rnp;
	struct rcu_state *rsp = &rcu_preempt_state;
	unsigned long snap;
	int trycount = 0;

	smp_mb(); /*                                                  */
	snap = ACCESS_ONCE(sync_rcu_preempt_exp_count) + 1;
	smp_mb(); /*                                                  */

	/*
                                                                  
                                                                
                                                              
                                                                 
                                                               
                                          
  */
	get_online_cpus();

	/*
                                                               
                                                              
                                              
  */
	while (!mutex_trylock(&sync_rcu_preempt_exp_mutex)) {
		if (ULONG_CMP_LT(snap,
		    ACCESS_ONCE(sync_rcu_preempt_exp_count))) {
			put_online_cpus();
			goto mb_ret; /*                             */
		}
		if (trycount++ < 10) {
			udelay(trycount * num_online_cpus());
		} else {
			put_online_cpus();
			wait_rcu_gp(call_rcu);
			return;
		}
	}
	if (ULONG_CMP_LT(snap, ACCESS_ONCE(sync_rcu_preempt_exp_count))) {
		put_online_cpus();
		goto unlock_mb_ret; /*                             */
	}

	/*                                                */
	synchronize_sched_expedited();

	/*                                                            */
	rcu_for_each_nonleaf_node_breadth_first(rsp, rnp) {
		raw_spin_lock_irqsave(&rnp->lock, flags);
		rnp->expmask = rnp->qsmaskinit;
		raw_spin_unlock_irqrestore(&rnp->lock, flags);
	}

	/*                                               */
	rcu_for_each_leaf_node(rsp, rnp)
		sync_rcu_preempt_exp_init(rsp, rnp);
	if (NUM_RCU_NODES > 1)
		sync_rcu_preempt_exp_init(rsp, rcu_get_root(rsp));

	put_online_cpus();

	/*                                                   */
	rnp = rcu_get_root(rsp);
	wait_event(sync_rcu_preempt_exp_wq,
		   sync_rcu_preempt_exp_done(rnp));

	/*                    */
	smp_mb(); /*                                                    */
	ACCESS_ONCE(sync_rcu_preempt_exp_count)++;
unlock_mb_ret:
	mutex_unlock(&sync_rcu_preempt_exp_mutex);
mb_ret:
	smp_mb(); /*                                                   */
}
EXPORT_SYMBOL_GPL(synchronize_rcu_expedited);

/* 
                                                                        
  
                                                                             
                                                                           
                                                                   
                                                                            
 */
void rcu_barrier(void)
{
	_rcu_barrier(&rcu_preempt_state);
}
EXPORT_SYMBOL_GPL(rcu_barrier);

/*
                                                 
 */
static void __init __rcu_init_preempt(void)
{
	rcu_init_one(&rcu_preempt_state, &rcu_preempt_data);
}

#else /*                                */

static struct rcu_state *rcu_state = &rcu_sched_state;

/*
                                       
 */
static void __init rcu_bootup_announce(void)
{
	printk(KERN_INFO "Hierarchical RCU implementation.\n");
	rcu_bootup_announce_oddness();
}

/*
                                                                         
 */
long rcu_batches_completed(void)
{
	return rcu_batches_completed_sched();
}
EXPORT_SYMBOL_GPL(rcu_batches_completed);

/*
                                                                          
                                      
 */
void rcu_force_quiescent_state(void)
{
	rcu_sched_force_quiescent_state();
}
EXPORT_SYMBOL_GPL(rcu_force_quiescent_state);

/*
                                                                     
                                  
 */
static void rcu_preempt_note_context_switch(int cpu)
{
}

/*
                                                                        
               
 */
static int rcu_preempt_blocked_readers_cgp(struct rcu_node *rnp)
{
	return 0;
}

#ifdef CONFIG_HOTPLUG_CPU

/*                                                               */
static void rcu_report_unblock_qs_rnp(struct rcu_node *rnp, unsigned long flags)
{
	raw_spin_unlock_irqrestore(&rnp->lock, flags);
}

#endif /*                           */

/*
                                                                     
                                                        
 */
static void rcu_print_detail_task_stall(struct rcu_state *rsp)
{
}

/*
                                                                     
                                                        
 */
static int rcu_print_task_stall(struct rcu_node *rnp)
{
	return 0;
}

/*
                                                                        
                                                                     
                       
 */
static void rcu_preempt_check_blocked_tasks(struct rcu_node *rnp)
{
	WARN_ON_ONCE(rnp->qsmask);
}

#ifdef CONFIG_HOTPLUG_CPU

/*
                                                                    
                                                                      
                                                                         
                
 */
static int rcu_preempt_offline_tasks(struct rcu_state *rsp,
				     struct rcu_node *rnp,
				     struct rcu_data *rdp)
{
	return 0;
}

#endif /*                           */

/*
                                                                     
            
 */
static void rcu_preempt_check_callbacks(int cpu)
{
}

/*
                                                                  
                                                                    
                                                                
                                                                
                                                  
  
                                                                 
 */
void kfree_call_rcu(struct rcu_head *head,
		    void (*func)(struct rcu_head *rcu))
{
	__call_rcu(head, func, &rcu_sched_state, -1, 1);
}
EXPORT_SYMBOL_GPL(kfree_call_rcu);

/*
                                                                    
                                                                
 */
void synchronize_rcu_expedited(void)
{
	synchronize_sched_expedited();
}
EXPORT_SYMBOL_GPL(synchronize_rcu_expedited);

#ifdef CONFIG_HOTPLUG_CPU

/*
                                                                     
                                                                      
                               
 */
static void rcu_report_exp_rnp(struct rcu_state *rsp, struct rcu_node *rnp,
			       bool wake)
{
}

#endif /*                           */

/*
                                                                
                                        
 */
void rcu_barrier(void)
{
	rcu_barrier_sched();
}
EXPORT_SYMBOL_GPL(rcu_barrier);

/*
                                                                      
 */
static void __init __rcu_init_preempt(void)
{
}

#endif /*                                      */

#ifdef CONFIG_RCU_BOOST

#include "rtmutex_common.h"

#ifdef CONFIG_RCU_TRACE

static void rcu_initiate_boost_trace(struct rcu_node *rnp)
{
	if (list_empty(&rnp->blkd_tasks))
		rnp->n_balk_blkd_tasks++;
	else if (rnp->exp_tasks == NULL && rnp->gp_tasks == NULL)
		rnp->n_balk_exp_gp_tasks++;
	else if (rnp->gp_tasks != NULL && rnp->boost_tasks != NULL)
		rnp->n_balk_boost_tasks++;
	else if (rnp->gp_tasks != NULL && rnp->qsmask != 0)
		rnp->n_balk_notblocked++;
	else if (rnp->gp_tasks != NULL &&
		 ULONG_CMP_LT(jiffies, rnp->boost_time))
		rnp->n_balk_notyet++;
	else
		rnp->n_balk_nos++;
}

#else /*                         */

static void rcu_initiate_boost_trace(struct rcu_node *rnp)
{
}

#endif /*                               */

static void rcu_wake_cond(struct task_struct *t, int status)
{
	/*
                                                     
                        
  */
	if (status != RCU_KTHREAD_YIELDING || is_idle_task(current))
		wake_up_process(t);
}

/*
                                                                       
                                                                  
                     
  
                                                               
                                                           
 */
static int rcu_boost(struct rcu_node *rnp)
{
	unsigned long flags;
	struct rt_mutex mtx;
	struct task_struct *t;
	struct list_head *tb;

	if (rnp->exp_tasks == NULL && rnp->boost_tasks == NULL)
		return 0;  /*                        */

	raw_spin_lock_irqsave(&rnp->lock, flags);

	/*
                                                         
                                                                  
  */
	if (rnp->exp_tasks == NULL && rnp->boost_tasks == NULL) {
		raw_spin_unlock_irqrestore(&rnp->lock, flags);
		return 0;
	}

	/*
                                                                
                                                                
                                                                  
                                                        
  */
	if (rnp->exp_tasks != NULL) {
		tb = rnp->exp_tasks;
		rnp->n_exp_boosts++;
	} else {
		tb = rnp->boost_tasks;
		rnp->n_normal_boosts++;
	}
	rnp->n_tasks_boosted++;

	/*
                                                                
                                                                 
                                                                 
                                                             
                                                             
                                                                 
   
                                                                 
                                                               
                                                               
                                                            
                                                               
                                                             
            
  */
	t = container_of(tb, struct task_struct, rcu_node_entry);
	rt_mutex_init_proxy_locked(&mtx, t);
	t->rcu_boost_mutex = &mtx;
	raw_spin_unlock_irqrestore(&rnp->lock, flags);
	rt_mutex_lock(&mtx);  /*                                        */
	rt_mutex_unlock(&mtx);  /*                     */

	return ACCESS_ONCE(rnp->exp_tasks) != NULL ||
	       ACCESS_ONCE(rnp->boost_tasks) != NULL;
}

/*
                                                                    
                 
 */
static int rcu_boost_kthread(void *arg)
{
	struct rcu_node *rnp = (struct rcu_node *)arg;
	int spincnt = 0;
	int more2boost;

	trace_rcu_utilization("Start boost kthread@init");
	for (;;) {
		rnp->boost_kthread_status = RCU_KTHREAD_WAITING;
		trace_rcu_utilization("End boost kthread@rcu_wait");
		rcu_wait(rnp->boost_tasks || rnp->exp_tasks);
		trace_rcu_utilization("Start boost kthread@rcu_wait");
		rnp->boost_kthread_status = RCU_KTHREAD_RUNNING;
		more2boost = rcu_boost(rnp);
		if (more2boost)
			spincnt++;
		else
			spincnt = 0;
		if (spincnt > 10) {
			rnp->boost_kthread_status = RCU_KTHREAD_YIELDING;
			trace_rcu_utilization("End boost kthread@rcu_yield");
			schedule_timeout_interruptible(2);
			trace_rcu_utilization("Start boost kthread@rcu_yield");
			spincnt = 0;
		}
	}
	/*            */
	trace_rcu_utilization("End boost kthread@notreached");
	return 0;
}

/*
                                                                    
                                                                       
                                                                  
                                                  
  
                                                                
                                                                  
                       
 */
static void rcu_initiate_boost(struct rcu_node *rnp, unsigned long flags)
{
	struct task_struct *t;

	if (!rcu_preempt_blocked_readers_cgp(rnp) && rnp->exp_tasks == NULL) {
		rnp->n_balk_exp_gp_tasks++;
		raw_spin_unlock_irqrestore(&rnp->lock, flags);
		return;
	}
	if (rnp->exp_tasks != NULL ||
	    (rnp->gp_tasks != NULL &&
	     rnp->boost_tasks == NULL &&
	     rnp->qsmask == 0 &&
	     ULONG_CMP_GE(jiffies, rnp->boost_time))) {
		if (rnp->exp_tasks == NULL)
			rnp->boost_tasks = rnp->gp_tasks;
		raw_spin_unlock_irqrestore(&rnp->lock, flags);
		t = rnp->boost_kthread_task;
		if (t)
			rcu_wake_cond(t, rnp->boost_kthread_status);
	} else {
		rcu_initiate_boost_trace(rnp);
		raw_spin_unlock_irqrestore(&rnp->lock, flags);
	}
}

/*
                                                       
 */
static void invoke_rcu_callbacks_kthread(void)
{
	unsigned long flags;

	local_irq_save(flags);
	__this_cpu_write(rcu_cpu_has_work, 1);
	if (__this_cpu_read(rcu_cpu_kthread_task) != NULL &&
	    current != __this_cpu_read(rcu_cpu_kthread_task)) {
		rcu_wake_cond(__this_cpu_read(rcu_cpu_kthread_task),
			      __this_cpu_read(rcu_cpu_kthread_status));
	}
	local_irq_restore(flags);
}

/*
                                                        
                                        
 */
static bool rcu_is_callbacks_kthread(void)
{
	return __get_cpu_var(rcu_cpu_kthread_task) == current;
}

#define RCU_BOOST_DELAY_JIFFIES DIV_ROUND_UP(CONFIG_RCU_BOOST_DELAY * HZ, 1000)

/*
                                                                    
 */
static void rcu_preempt_boost_start_gp(struct rcu_node *rnp)
{
	rnp->boost_time = jiffies + RCU_BOOST_DELAY_JIFFIES;
}

/*
                                                                     
                                                                   
                                                          
 */
static int __cpuinit rcu_spawn_one_boost_kthread(struct rcu_state *rsp,
						 struct rcu_node *rnp)
{
	int rnp_index = rnp - &rsp->node[0];
	unsigned long flags;
	struct sched_param sp;
	struct task_struct *t;

	if (&rcu_preempt_state != rsp)
		return 0;

	if (!rcu_scheduler_fully_active || rnp->qsmaskinit == 0)
		return 0;

	rsp->boost = 1;
	if (rnp->boost_kthread_task != NULL)
		return 0;
	t = kthread_create(rcu_boost_kthread, (void *)rnp,
			   "rcub/%d", rnp_index);
	if (IS_ERR(t))
		return PTR_ERR(t);
	raw_spin_lock_irqsave(&rnp->lock, flags);
	rnp->boost_kthread_task = t;
	raw_spin_unlock_irqrestore(&rnp->lock, flags);
	sp.sched_priority = RCU_BOOST_PRIO;
	sched_setscheduler_nocheck(t, SCHED_FIFO, &sp);
	wake_up_process(t); /*                                    */
	return 0;
}

static void rcu_kthread_do_work(void)
{
	rcu_do_batch(&rcu_sched_state, &__get_cpu_var(rcu_sched_data));
	rcu_do_batch(&rcu_bh_state, &__get_cpu_var(rcu_bh_data));
	rcu_preempt_do_callbacks();
}

static void rcu_cpu_kthread_setup(unsigned int cpu)
{
	struct sched_param sp;

	sp.sched_priority = RCU_KTHREAD_PRIO;
	sched_setscheduler_nocheck(current, SCHED_FIFO, &sp);
}

static void rcu_cpu_kthread_park(unsigned int cpu)
{
	per_cpu(rcu_cpu_kthread_status, cpu) = RCU_KTHREAD_OFFCPU;
}

static int rcu_cpu_kthread_should_run(unsigned int cpu)
{
	return __get_cpu_var(rcu_cpu_has_work);
}

/*
                                                                       
                                                                    
                                 
 */
static void rcu_cpu_kthread(unsigned int cpu)
{
	unsigned int *statusp = &__get_cpu_var(rcu_cpu_kthread_status);
	char work, *workp = &__get_cpu_var(rcu_cpu_has_work);
	int spincnt;

	for (spincnt = 0; spincnt < 10; spincnt++) {
		trace_rcu_utilization("Start CPU kthread@rcu_wait");
		local_bh_disable();
		*statusp = RCU_KTHREAD_RUNNING;
		this_cpu_inc(rcu_cpu_kthread_loops);
		local_irq_disable();
		work = *workp;
		*workp = 0;
		local_irq_enable();
		if (work)
			rcu_kthread_do_work();
		local_bh_enable();
		if (*workp == 0) {
			trace_rcu_utilization("End CPU kthread@rcu_wait");
			*statusp = RCU_KTHREAD_WAITING;
			return;
		}
	}
	*statusp = RCU_KTHREAD_YIELDING;
	trace_rcu_utilization("Start CPU kthread@rcu_yield");
	schedule_timeout_interruptible(2);
	trace_rcu_utilization("End CPU kthread@rcu_yield");
	*statusp = RCU_KTHREAD_WAITING;
}

/*
                                                                     
                                                                     
                                                        
  
                                                                       
                                                                   
                                                          
 */
static void rcu_boost_kthread_setaffinity(struct rcu_node *rnp, int outgoingcpu)
{
	struct task_struct *t = rnp->boost_kthread_task;
	unsigned long mask = rnp->qsmaskinit;
	cpumask_var_t cm;
	int cpu;

	if (!t)
		return;
	if (!zalloc_cpumask_var(&cm, GFP_KERNEL))
		return;
	for (cpu = rnp->grplo; cpu <= rnp->grphi; cpu++, mask >>= 1)
		if ((mask & 0x1) && cpu != outgoingcpu)
			cpumask_set_cpu(cpu, cm);
	if (cpumask_weight(cm) == 0) {
		cpumask_setall(cm);
		for (cpu = rnp->grplo; cpu <= rnp->grphi; cpu++)
			cpumask_clear_cpu(cpu, cm);
		WARN_ON_ONCE(cpumask_weight(cm) == 0);
	}
	set_cpus_allowed_ptr(t, cm);
	free_cpumask_var(cm);
}

static struct smp_hotplug_thread rcu_cpu_thread_spec = {
	.store			= &rcu_cpu_kthread_task,
	.thread_should_run	= rcu_cpu_kthread_should_run,
	.thread_fn		= rcu_cpu_kthread,
	.thread_comm		= "rcuc/%u",
	.setup			= rcu_cpu_kthread_setup,
	.park			= rcu_cpu_kthread_park,
};

/*
                                                                    
 */
static int __init rcu_spawn_kthreads(void)
{
	struct rcu_node *rnp;
	int cpu;

	rcu_scheduler_fully_active = 1;
	for_each_possible_cpu(cpu)
		per_cpu(rcu_cpu_has_work, cpu) = 0;
	BUG_ON(smpboot_register_percpu_thread(&rcu_cpu_thread_spec));
	rnp = rcu_get_root(rcu_state);
	(void)rcu_spawn_one_boost_kthread(rcu_state, rnp);
	if (NUM_RCU_NODES > 1) {
		rcu_for_each_leaf_node(rcu_state, rnp)
			(void)rcu_spawn_one_boost_kthread(rcu_state, rnp);
	}
	return 0;
}
early_initcall(rcu_spawn_kthreads);

static void __cpuinit rcu_prepare_kthreads(int cpu)
{
	struct rcu_data *rdp = per_cpu_ptr(rcu_state->rda, cpu);
	struct rcu_node *rnp = rdp->mynode;

	/*                                                               */
	if (rcu_scheduler_fully_active)
		(void)rcu_spawn_one_boost_kthread(rcu_state, rnp);
}

#else /*                         */

static void rcu_initiate_boost(struct rcu_node *rnp, unsigned long flags)
{
	raw_spin_unlock_irqrestore(&rnp->lock, flags);
}

static void invoke_rcu_callbacks_kthread(void)
{
	WARN_ON_ONCE(1);
}

static bool rcu_is_callbacks_kthread(void)
{
	return false;
}

static void rcu_preempt_boost_start_gp(struct rcu_node *rnp)
{
}

static void rcu_boost_kthread_setaffinity(struct rcu_node *rnp, int outgoingcpu)
{
}

static int __init rcu_scheduler_really_started(void)
{
	rcu_scheduler_fully_active = 1;
	return 0;
}
early_initcall(rcu_scheduler_really_started);

static void __cpuinit rcu_prepare_kthreads(int cpu)
{
}

#endif /*                               */

#if !defined(CONFIG_RCU_FAST_NO_HZ)

/*
                                                                   
                                                                       
                                                                         
                                     
  
                                                                        
                     
 */
int rcu_needs_cpu(int cpu, unsigned long *delta_jiffies)
{
	*delta_jiffies = ULONG_MAX;
	return rcu_cpu_has_callbacks(cpu, NULL);
}

/*
                                                                  
            
 */
static void rcu_cleanup_after_idle(int cpu)
{
}

/*
                                                                               
              
 */
static void rcu_prepare_for_idle(int cpu)
{
}

/*
                                                                      
                                          
 */
static void rcu_idle_count_callbacks_posted(void)
{
}

#else /*                                     */

/*
                                                                    
                                                                      
                                                                
                                                             
  
                                                                       
  
                                                                        
                                                                  
                                                                        
                                                                     
                                                                      
                                                                    
                                                  
                                                                   
                                                             
                                                                 
  
                                                                       
                                                                          
                                                             
 */
#define RCU_IDLE_GP_DELAY 4		/*                           */
#define RCU_IDLE_LAZY_GP_DELAY (6 * HZ)	/*                      */

static int rcu_idle_gp_delay = RCU_IDLE_GP_DELAY;
module_param(rcu_idle_gp_delay, int, 0644);
static int rcu_idle_lazy_gp_delay = RCU_IDLE_LAZY_GP_DELAY;
module_param(rcu_idle_lazy_gp_delay, int, 0644);

extern int tick_nohz_enabled;

/*
                                                                      
                                                                         
               
 */
static bool rcu_try_advance_all_cbs(void)
{
	bool cbs_ready = false;
	struct rcu_data *rdp;
	struct rcu_node *rnp;
	struct rcu_state *rsp;

	for_each_rcu_flavor(rsp) {
		rdp = this_cpu_ptr(rsp->rda);
		rnp = rdp->mynode;

		/*
                                                    
                                                  
                                       
   */
		if (rdp->completed != rnp->completed &&
		    rdp->nxttail[RCU_DONE_TAIL] != rdp->nxttail[RCU_NEXT_TAIL])
			rcu_process_gp_end(rsp, rdp);

		if (cpu_has_callbacks_ready_to_invoke(rdp))
			cbs_ready = true;
	}
	return cbs_ready;
}

/*
                                                                         
                                                                       
                                                                       
             
  
                                            
 */
int rcu_needs_cpu(int cpu, unsigned long *dj)
{
	struct rcu_dynticks *rdtp = &per_cpu(rcu_dynticks, cpu);

	/*                                                        */
	rdtp->nonlazy_posted_snap = rdtp->nonlazy_posted;

	/*                                            */
	if (!rcu_cpu_has_callbacks(cpu, &rdtp->all_lazy)) {
		*dj = ULONG_MAX;
		return 0;
	}

	/*                               */
	if (rcu_try_advance_all_cbs()) {
		/*                                                     */
		invoke_rcu_core();
		return 1;
	}
	rdtp->last_accelerate = jiffies;

	/*                                                       */
	if (!rdtp->all_lazy) {
		*dj = round_up(rcu_idle_gp_delay + jiffies,
			       rcu_idle_gp_delay) - jiffies;
	} else {
		*dj = round_jiffies(rcu_idle_lazy_gp_delay + jiffies) - jiffies;
	}
	return 0;
}

/*
                                                                        
                                                                        
                                                                      
                                                                       
                                                                        
                                  
  
                                            
 */
static void rcu_prepare_for_idle(int cpu)
{
	struct rcu_data *rdp;
	struct rcu_dynticks *rdtp = &per_cpu(rcu_dynticks, cpu);
	struct rcu_node *rnp;
	struct rcu_state *rsp;
	int tne;

	/*                                                 */
	tne = ACCESS_ONCE(tick_nohz_enabled);
	if (tne != rdtp->tick_nohz_enabled_snap) {
		if (rcu_cpu_has_callbacks(cpu, NULL))
			invoke_rcu_core(); /*                           */
		rdtp->tick_nohz_enabled_snap = tne;
		return;
	}
	if (!tne)
		return;

	/*                                                     */
	if (rcu_is_nocb_cpu(cpu))
		return;

	/*
                                                            
                                                                   
                                      
  */
	if (rdtp->all_lazy &&
	    rdtp->nonlazy_posted != rdtp->nonlazy_posted_snap) {
		invoke_rcu_core();
		return;
	}

	/*
                                                             
                          
  */
	if (rdtp->last_accelerate == jiffies)
		return;
	rdtp->last_accelerate = jiffies;
	for_each_rcu_flavor(rsp) {
		rdp = per_cpu_ptr(rsp->rda, cpu);
		if (!*rdp->nxttail[RCU_DONE_TAIL])
			continue;
		rnp = rdp->mynode;
		raw_spin_lock(&rnp->lock); /*                        */
		rcu_accelerate_cbs(rsp, rnp, rdp);
		raw_spin_unlock(&rnp->lock); /*                       */
	}
}

/*
                                                                      
                                                                    
                                                          
 */
static void rcu_cleanup_after_idle(int cpu)
{
	struct rcu_data *rdp;
	struct rcu_state *rsp;

	if (rcu_is_nocb_cpu(cpu))
		return;
	rcu_try_advance_all_cbs();
	for_each_rcu_flavor(rsp) {
		rdp = per_cpu_ptr(rsp->rda, cpu);
		if (cpu_has_callbacks_ready_to_invoke(rdp))
			invoke_rcu_core();
	}
}

/*
                                                                  
                                                                         
                                                                       
                                                                      
                                                                       
                                                                
 */
static void rcu_idle_count_callbacks_posted(void)
{
	__this_cpu_add(rcu_dynticks.nonlazy_posted, 1);
}

/*
                                                    
 */
static atomic_t oom_callback_count;
static DECLARE_WAIT_QUEUE_HEAD(oom_callback_wq);

/*
                                                                      
                                  
 */
static void rcu_oom_callback(struct rcu_head *rhp)
{
	if (atomic_dec_and_test(&oom_callback_count))
		wake_up(&oom_callback_wq);
}

/*
                                                                  
                                                                   
                                                                    
                                                                     
                                        
 */
static void rcu_oom_notify_cpu(void *unused)
{
	struct rcu_state *rsp;
	struct rcu_data *rdp;

	for_each_rcu_flavor(rsp) {
		rdp = __this_cpu_ptr(rsp->rda);
		if (rdp->qlen_lazy != 0) {
			atomic_inc(&oom_callback_count);
			rsp->call(&rdp->oom_head, rcu_oom_callback);
		}
	}
}

/*
                                                                  
                                                                
                                                                          
                                                                        
                                                     
 */
static int rcu_oom_notify(struct notifier_block *self,
			  unsigned long notused, void *nfreed)
{
	int cpu;

	/*                                                       */
	wait_event(oom_callback_wq, atomic_read(&oom_callback_count) == 0);

	/*
                                                               
                                                          
  */
	atomic_set(&oom_callback_count, 1);

	get_online_cpus();
	for_each_online_cpu(cpu) {
		smp_call_function_single(cpu, rcu_oom_notify_cpu, NULL, 1);
		cond_resched();
	}
	put_online_cpus();

	/*                                                          */
	atomic_dec(&oom_callback_count);

	return NOTIFY_OK;
}

static struct notifier_block rcu_oom_nb = {
	.notifier_call = rcu_oom_notify
};

static int __init rcu_register_oom_notifier(void)
{
	register_oom_notifier(&rcu_oom_nb);
	return 0;
}
early_initcall(rcu_register_oom_notifier);

#endif /*                                           */

#ifdef CONFIG_RCU_CPU_STALL_INFO

#ifdef CONFIG_RCU_FAST_NO_HZ

static void print_cpu_stall_fast_no_hz(char *cp, int cpu)
{
	struct rcu_dynticks *rdtp = &per_cpu(rcu_dynticks, cpu);
	unsigned long nlpd = rdtp->nonlazy_posted - rdtp->nonlazy_posted_snap;

	sprintf(cp, "last_accelerate: %04lx/%04lx, nonlazy_posted: %ld, %c%c",
		rdtp->last_accelerate & 0xffff, jiffies & 0xffff,
		ulong2long(nlpd),
		rdtp->all_lazy ? 'L' : '.',
		rdtp->tick_nohz_enabled_snap ? '.' : 'D');
}

#else /*                              */

static void print_cpu_stall_fast_no_hz(char *cp, int cpu)
{
	*cp = '\0';
}

#endif /*                                    */

/*                               */
static void print_cpu_stall_info_begin(void)
{
	printk(KERN_CONT "\n");
}

/*
                                                                  
  
                                                                
                                                                 
                                                                 
                                                                
                                                                
                                      
  
                                                                      
 */
static void print_cpu_stall_info(struct rcu_state *rsp, int cpu)
{
	char fast_no_hz[72];
	struct rcu_data *rdp = per_cpu_ptr(rsp->rda, cpu);
	struct rcu_dynticks *rdtp = rdp->dynticks;
	char *ticks_title;
	unsigned long ticks_value;

	if (rsp->gpnum == rdp->gpnum) {
		ticks_title = "ticks this GP";
		ticks_value = rdp->ticks_this_gp;
	} else {
		ticks_title = "GPs behind";
		ticks_value = rsp->gpnum - rdp->gpnum;
	}
	print_cpu_stall_fast_no_hz(fast_no_hz, cpu);
	printk(KERN_ERR "\t%d: (%lu %s) idle=%03x/%llx/%d softirq=%u/%u %s\n",
	       cpu, ticks_value, ticks_title,
	       atomic_read(&rdtp->dynticks) & 0xfff,
	       rdtp->dynticks_nesting, rdtp->dynticks_nmi_nesting,
	       rdp->softirq_snap, kstat_softirqs_cpu(RCU_SOFTIRQ, cpu),
	       fast_no_hz);
}

/*                                */
static void print_cpu_stall_info_end(void)
{
	printk(KERN_ERR "\t");
}

/*                                              */
static void zero_cpu_stall_ticks(struct rcu_data *rdp)
{
	rdp->ticks_this_gp = 0;
	rdp->softirq_snap = kstat_softirqs_cpu(RCU_SOFTIRQ, smp_processor_id());
}

/*                                                   */
static void increment_cpu_stall_ticks(void)
{
	struct rcu_state *rsp;

	for_each_rcu_flavor(rsp)
		__this_cpu_ptr(rsp->rda)->ticks_this_gp++;
}

#else /*                                  */

static void print_cpu_stall_info_begin(void)
{
	printk(KERN_CONT " {");
}

static void print_cpu_stall_info(struct rcu_state *rsp, int cpu)
{
	printk(KERN_CONT " %d", cpu);
}

static void print_cpu_stall_info_end(void)
{
	printk(KERN_CONT "} ");
}

static void zero_cpu_stall_ticks(struct rcu_data *rdp)
{
}

static void increment_cpu_stall_ticks(void)
{
}

#endif /*                                        */

#ifdef CONFIG_RCU_NOCB_CPU

/*
                                                                       
                                                                   
                                                                       
                                                                 
                                                                   
                                                                          
                                                                    
                                                                    
                                      
  
                                                                        
                                                                     
                                            
  
                                                                    
                                                                  
                                                               
 */


/*                                                                        */
static int __init rcu_nocb_setup(char *str)
{
	alloc_bootmem_cpumask_var(&rcu_nocb_mask);
	have_rcu_nocb_mask = true;
	cpulist_parse(str, rcu_nocb_mask);
	return 1;
}
__setup("rcu_nocbs=", rcu_nocb_setup);

static int __init parse_rcu_nocb_poll(char *arg)
{
	rcu_nocb_poll = 1;
	return 0;
}
early_param("rcu_nocb_poll", parse_rcu_nocb_poll);

/*
                                                
  
                                                                     
                                                              
 */
static int rcu_nocb_needs_gp(struct rcu_state *rsp)
{
	struct rcu_node *rnp = rcu_get_root(rsp);

	return rnp->need_future_gp[(ACCESS_ONCE(rnp->completed) + 1) & 0x1];
}

/*
                                                                        
                
 */
static void rcu_nocb_gp_cleanup(struct rcu_state *rsp, struct rcu_node *rnp)
{
	wake_up_all(&rnp->nocb_gp_wq[rnp->completed & 0x1]);
}

/*
                                                           
                                                                   
                                                                
                                                                 
                                                               
                                                               
 */
static void rcu_nocb_gp_set(struct rcu_node *rnp, int nrq)
{
	rnp->need_future_gp[(rnp->completed + 1) & 0x1] += nrq;
}

static void rcu_init_one_nocb(struct rcu_node *rnp)
{
	init_waitqueue_head(&rnp->nocb_gp_wq[0]);
	init_waitqueue_head(&rnp->nocb_gp_wq[1]);
}

/*                                     */
bool rcu_is_nocb_cpu(int cpu)
{
	if (have_rcu_nocb_mask)
		return cpumask_test_cpu(cpu, rcu_nocb_mask);
	return false;
}

/*
                                                                         
                                                                    
                                                                        
                                                   
  
                                                                     
 */
static void __call_rcu_nocb_enqueue(struct rcu_data *rdp,
				    struct rcu_head *rhp,
				    struct rcu_head **rhtp,
				    int rhcount, int rhcount_lazy)
{
	int len;
	struct rcu_head **old_rhpp;
	struct task_struct *t;

	/*                                                          */
	old_rhpp = xchg(&rdp->nocb_tail, rhtp);
	ACCESS_ONCE(*old_rhpp) = rhp;
	atomic_long_add(rhcount, &rdp->nocb_q_count);
	atomic_long_add(rhcount_lazy, &rdp->nocb_q_count_lazy);

	/*                                                                  */
	t = ACCESS_ONCE(rdp->nocb_kthread);
	if (rcu_nocb_poll | !t)
		return;
	len = atomic_long_read(&rdp->nocb_q_count);
	if (old_rhpp == &rdp->nocb_head) {
		wake_up(&rdp->nocb_wq); /*                                 */
		rdp->qlen_last_fqs_check = 0;
	} else if (len > rdp->qlen_last_fqs_check + qhimark) {
		wake_up_process(t); /*                                  */
		rdp->qlen_last_fqs_check = LONG_MAX / 2;
	}
	return;
}

/*
                                                                        
                                                                   
                                                                    
                 
  
                                                                       
                              
 */
static bool __call_rcu_nocb(struct rcu_data *rdp, struct rcu_head *rhp,
			    bool lazy)
{

	if (!rcu_is_nocb_cpu(rdp->cpu))
		return 0;
	__call_rcu_nocb_enqueue(rdp, rhp, &rhp->next, 1, lazy);
	if (__is_kfree_rcu_offset((unsigned long)rhp->func))
		trace_rcu_kfree_callback(rdp->rsp->name, rhp,
					 (unsigned long)rhp->func,
					 rdp->qlen_lazy, rdp->qlen);
	else
		trace_rcu_callback(rdp->rsp->name, rhp,
				   rdp->qlen_lazy, rdp->qlen);
	return 1;
}

/*
                                                                   
                    
 */
static bool __maybe_unused rcu_nocb_adopt_orphan_cbs(struct rcu_state *rsp,
						     struct rcu_data *rdp)
{
	long ql = rsp->qlen;
	long qll = rsp->qlen_lazy;

	/*                                                                    */
	if (!rcu_is_nocb_cpu(smp_processor_id()))
		return 0;
	rsp->qlen = 0;
	rsp->qlen_lazy = 0;

	/*                                                                   */
	if (rsp->orphan_donelist != NULL) {
		__call_rcu_nocb_enqueue(rdp, rsp->orphan_donelist,
					rsp->orphan_donetail, ql, qll);
		ql = qll = 0;
		rsp->orphan_donelist = NULL;
		rsp->orphan_donetail = &rsp->orphan_donelist;
	}
	if (rsp->orphan_nxtlist != NULL) {
		__call_rcu_nocb_enqueue(rdp, rsp->orphan_nxtlist,
					rsp->orphan_nxttail, ql, qll);
		ql = qll = 0;
		rsp->orphan_nxtlist = NULL;
		rsp->orphan_nxttail = &rsp->orphan_nxtlist;
	}
	return 1;
}

/*
                                                                 
                                             
 */
static void rcu_nocb_wait_gp(struct rcu_data *rdp)
{
	unsigned long c;
	bool d;
	unsigned long flags;
	struct rcu_node *rnp = rdp->mynode;

	raw_spin_lock_irqsave(&rnp->lock, flags);
	c = rcu_start_future_gp(rnp, rdp);
	raw_spin_unlock_irqrestore(&rnp->lock, flags);

	/*
                                                                    
                        
  */
	trace_rcu_future_gp(rnp, rdp, c, "StartWait");
	for (;;) {
		wait_event_interruptible(
			rnp->nocb_gp_wq[c & 0x1],
			(d = ULONG_CMP_GE(ACCESS_ONCE(rnp->completed), c)));
		if (likely(d))
			break;
		flush_signals(current);
		trace_rcu_future_gp(rnp, rdp, c, "ResumeWait");
	}
	trace_rcu_future_gp(rnp, rdp, c, "EndWait");
	smp_mb(); /*                                                 */
}

/*
                                                                        
                                                    
 */
static int rcu_nocb_kthread(void *arg)
{
	int c, cl;
	struct rcu_head *list;
	struct rcu_head *next;
	struct rcu_head **tail;
	struct rcu_data *rdp = arg;

	/*                                                            */
	for (;;) {
		/*                                                   */
		if (!rcu_nocb_poll)
			wait_event_interruptible(rdp->nocb_wq, rdp->nocb_head);
		list = ACCESS_ONCE(rdp->nocb_head);
		if (!list) {
			schedule_timeout_interruptible(1);
			flush_signals(current);
			continue;
		}

		/*
                                                      
                                  
   */
		ACCESS_ONCE(rdp->nocb_head) = NULL;
		tail = xchg(&rdp->nocb_tail, &rdp->nocb_head);
		c = atomic_long_xchg(&rdp->nocb_q_count, 0);
		cl = atomic_long_xchg(&rdp->nocb_q_count_lazy, 0);
		ACCESS_ONCE(rdp->nocb_p_count) += c;
		ACCESS_ONCE(rdp->nocb_p_count_lazy) += cl;
		rcu_nocb_wait_gp(rdp);

		/*                                                          */
		trace_rcu_batch_start(rdp->rsp->name, cl, c, -1);
		c = cl = 0;
		while (list) {
			next = list->next;
			/*                                            */
			while (next == NULL && &list->next != tail) {
				schedule_timeout_interruptible(1);
				next = list->next;
			}
			debug_rcu_head_unqueue(list);
			local_bh_disable();
			if (__rcu_reclaim(rdp->rsp->name, list))
				cl++;
			c++;
			local_bh_enable();
			list = next;
		}
		trace_rcu_batch_end(rdp->rsp->name, c, !!list, 0, 0, 1);
		ACCESS_ONCE(rdp->nocb_p_count) -= c;
		ACCESS_ONCE(rdp->nocb_p_count_lazy) -= cl;
		rdp->n_nocbs_invoked += c;
	}
	return 0;
}

/*                                                    */
static void __init rcu_boot_init_nocb_percpu_data(struct rcu_data *rdp)
{
	rdp->nocb_tail = &rdp->nocb_head;
	init_waitqueue_head(&rdp->nocb_wq);
}

/*                                                           */
static void __init rcu_spawn_nocb_kthreads(struct rcu_state *rsp)
{
	int cpu;
	struct rcu_data *rdp;
	struct task_struct *t;

	if (rcu_nocb_mask == NULL)
		return;
	for_each_cpu(cpu, rcu_nocb_mask) {
		rdp = per_cpu_ptr(rsp->rda, cpu);
		t = kthread_run(rcu_nocb_kthread, rdp,
				"rcuo%c/%d", rsp->abbr, cpu);
		BUG_ON(IS_ERR(t));
		ACCESS_ONCE(rdp->nocb_kthread) = t;
	}
}

/*                                                              */
static bool init_nocb_callback_list(struct rcu_data *rdp)
{
	if (rcu_nocb_mask == NULL ||
	    !cpumask_test_cpu(rdp->cpu, rcu_nocb_mask))
		return false;
	rdp->nxttail[RCU_NEXT_TAIL] = NULL;
	return true;
}

#else /*                            */

static int rcu_nocb_needs_gp(struct rcu_state *rsp)
{
	return 0;
}

static void rcu_nocb_gp_cleanup(struct rcu_state *rsp, struct rcu_node *rnp)
{
}

static void rcu_nocb_gp_set(struct rcu_node *rnp, int nrq)
{
}

static void rcu_init_one_nocb(struct rcu_node *rnp)
{
}

static bool __call_rcu_nocb(struct rcu_data *rdp, struct rcu_head *rhp,
			    bool lazy)
{
	return 0;
}

static bool __maybe_unused rcu_nocb_adopt_orphan_cbs(struct rcu_state *rsp,
						     struct rcu_data *rdp)
{
	return 0;
}

static void __init rcu_boot_init_nocb_percpu_data(struct rcu_data *rdp)
{
}

static void __init rcu_spawn_nocb_kthreads(struct rcu_state *rsp)
{
}

static bool init_nocb_callback_list(struct rcu_data *rdp)
{
	return false;
}

#endif /*                                  */

/*
                                                                      
                                                                        
                                                                      
                                                                         
                                                                           
                                                                      
                                                               
 */
static void rcu_kick_nohz_cpu(int cpu)
{
#ifdef CONFIG_NO_HZ_FULL
	if (tick_nohz_full_cpu(cpu))
		smp_send_reschedule(cpu);
#endif /*                          */
}
