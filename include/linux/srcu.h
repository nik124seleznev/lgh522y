/*
 * Sleepable Read-Copy Update mechanism for mutual exclusion
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
 * Copyright (C) IBM Corporation, 2006
 * Copyright (C) Fujitsu, 2012
 *
 * Author: Paul McKenney <paulmck@us.ibm.com>
 *	   Lai Jiangshan <laijs@cn.fujitsu.com>
 *
 * For detailed explanation of Read-Copy Update mechanism see -
 * 		Documentation/RCU/ *.txt
 *
 */

#ifndef _LINUX_SRCU_H
#define _LINUX_SRCU_H

#include <linux/mutex.h>
#include <linux/rcupdate.h>
#include <linux/workqueue.h>

struct srcu_struct_array {
	unsigned long c[2];
	unsigned long seq[2];
};

struct rcu_batch {
	struct rcu_head *head, **tail;
};

#define RCU_BATCH_INIT(name) { NULL, &(name.head) }

struct srcu_struct {
	unsigned completed;
	struct srcu_struct_array __percpu *per_cpu_ref;
	spinlock_t queue_lock; /*                                  */
	bool running;
	/*                       */
	struct rcu_batch batch_queue;
	/*                                          */
	struct rcu_batch batch_check0;
	/*                                                       */
	struct rcu_batch batch_check1;
	struct rcu_batch batch_done;
	struct delayed_work work;
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif /*                                */
};

#ifdef CONFIG_DEBUG_LOCK_ALLOC

int __init_srcu_struct(struct srcu_struct *sp, const char *name,
		       struct lock_class_key *key);

#define init_srcu_struct(sp) \
({ \
	static struct lock_class_key __srcu_key; \
	\
	__init_srcu_struct((sp), #sp, &__srcu_key); \
})

#define __SRCU_DEP_MAP_INIT(srcu_name)	.dep_map = { .name = #srcu_name },
#else /*                                */

int init_srcu_struct(struct srcu_struct *sp);

#define __SRCU_DEP_MAP_INIT(srcu_name)
#endif /*                                      */

void process_srcu(struct work_struct *work);

#define __SRCU_STRUCT_INIT(name)					\
	{								\
		.completed = -300,					\
		.per_cpu_ref = &name##_srcu_array,			\
		.queue_lock = __SPIN_LOCK_UNLOCKED(name.queue_lock),	\
		.running = false,					\
		.batch_queue = RCU_BATCH_INIT(name.batch_queue),	\
		.batch_check0 = RCU_BATCH_INIT(name.batch_check0),	\
		.batch_check1 = RCU_BATCH_INIT(name.batch_check1),	\
		.batch_done = RCU_BATCH_INIT(name.batch_done),		\
		.work = __DELAYED_WORK_INITIALIZER(name.work, process_srcu, 0),\
		__SRCU_DEP_MAP_INIT(name)				\
	}

/*
                                               
                                                                  
 */
#define DEFINE_SRCU(name)						\
	static DEFINE_PER_CPU(struct srcu_struct_array, name##_srcu_array);\
	struct srcu_struct name = __SRCU_STRUCT_INIT(name);

#define DEFINE_STATIC_SRCU(name)					\
	static DEFINE_PER_CPU(struct srcu_struct_array, name##_srcu_array);\
	static struct srcu_struct name = __SRCU_STRUCT_INIT(name);

/* 
                                                                           
                                         
                                                              
                                                            
  
                                                                    
                                                                   
                                                                     
                                                                     
                                                                      
                                                                    
                                         
  
                                                                           
                              
 */
void call_srcu(struct srcu_struct *sp, struct rcu_head *head,
		void (*func)(struct rcu_head *head));

void cleanup_srcu_struct(struct srcu_struct *sp);
int __srcu_read_lock(struct srcu_struct *sp) __acquires(sp);
void __srcu_read_unlock(struct srcu_struct *sp, int idx) __releases(sp);
void synchronize_srcu(struct srcu_struct *sp);
void synchronize_srcu_expedited(struct srcu_struct *sp);
long srcu_batches_completed(struct srcu_struct *sp);
void srcu_barrier(struct srcu_struct *sp);

#ifdef CONFIG_DEBUG_LOCK_ALLOC

/* 
                                                                        
  
                                                                         
                                                                      
                                                                          
                   
  
                                                                            
                                 
  
                                                                 
                                                            
                                                            
 */
static inline int srcu_read_lock_held(struct srcu_struct *sp)
{
	if (!debug_lockdep_rcu_enabled())
		return 1;
	return lock_is_held(&sp->dep_map);
}

#else /*                                */

static inline int srcu_read_lock_held(struct srcu_struct *sp)
{
	return 1;
}

#endif /*                                      */

/* 
                                                                                
                                                               
                                                                  
                                                    
                                             
  
                                                                     
                                                                            
                                                                          
                           
 */
#define srcu_dereference_check(p, sp, c) \
	__rcu_dereference_check((p), srcu_read_lock_held(sp) || (c), __rcu)

/* 
                                                                          
                                                               
                                                                  
                                                    
  
                                                                 
                                                                 
                                               
 */
#define srcu_dereference(p, sp) srcu_dereference_check((p), (sp), 0)

/* 
                                                                          
                                                        
  
                                                                      
                                                              
                                                                
                                                                 
                                                                   
                                                                     
                                
  
                                                                      
                                                                  
                                                                        
                                  
 */
static inline int srcu_read_lock(struct srcu_struct *sp) __acquires(sp)
{
	int retval = __srcu_read_lock(sp);

	rcu_lock_acquire(&(sp)->dep_map);
	return retval;
}

/* 
                                                                               
                                                          
                                                          
  
                                           
 */
static inline void srcu_read_unlock(struct srcu_struct *sp, int idx)
	__releases(sp)
{
	rcu_lock_release(&(sp)->dep_map);
	__srcu_read_unlock(sp, idx);
}

/* 
                                                                              
                                                        
  
                                                                          
                                                                       
                                                                        
                                                                        
                                                            
  
                                                                           
                                                                     
                                                                       
                                                        
 */
static inline int srcu_read_lock_raw(struct srcu_struct *sp)
{
	unsigned long flags;
	int ret;

	local_irq_save(flags);
	ret =  __srcu_read_lock(sp);
	local_irq_restore(flags);
	return ret;
}

/* 
                                                                             
                                                          
                                                              
  
                                                                        
                                             
 */
static inline void srcu_read_unlock_raw(struct srcu_struct *sp, int idx)
{
	unsigned long flags;

	local_irq_save(flags);
	__srcu_read_unlock(sp, idx);
	local_irq_restore(flags);
}

#endif
