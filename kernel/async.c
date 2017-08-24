/*
 * async.c: Asynchronous function calls for boot performance
 *
 * (C) Copyright 2009 Intel Corporation
 * Author: Arjan van de Ven <arjan@linux.intel.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */


/*

                             

                                                                   
                                                                     
                                      

                                                                
                                                           
                                                          
                                                                     
                                                                     

                                                                      
                                                                      
                                                   

                                                                          
                                  

                                                                         
                                                       
                                                                   
                                                                        
                                                                            
                      

                                                                      
                                                                          
                                                                
                                                                            
                                                                         
                                                 

*/

#include <linux/async.h>
#include <linux/atomic.h>
#include <linux/ktime.h>
#include <linux/export.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#include "workqueue_internal.h"

static async_cookie_t next_cookie = 1;

#define MAX_WORK		32768
#define ASYNC_COOKIE_MAX	ULLONG_MAX	/*                 */

static LIST_HEAD(async_global_pending);	/*                                  */
static ASYNC_DOMAIN(async_dfl_domain);
static DEFINE_SPINLOCK(async_lock);

struct async_entry {
	struct list_head	domain_list;
	struct list_head	global_list;
	struct work_struct	work;
	async_cookie_t		cookie;
	async_func_t		func;
	void			*data;
	struct async_domain	*domain;
};

static DECLARE_WAIT_QUEUE_HEAD(async_done);

static atomic_t entry_count;

static async_cookie_t lowest_in_progress(struct async_domain *domain)
{
	struct list_head *pending;
	async_cookie_t ret = ASYNC_COOKIE_MAX;
	unsigned long flags;

	spin_lock_irqsave(&async_lock, flags);

	if (domain)
		pending = &domain->pending;
	else
		pending = &async_global_pending;

	if (!list_empty(pending))
		ret = list_first_entry(pending, struct async_entry,
				       domain_list)->cookie;

	spin_unlock_irqrestore(&async_lock, flags);
	return ret;
}

/*
                                          
 */
static void async_run_entry_fn(struct work_struct *work)
{
	struct async_entry *entry =
		container_of(work, struct async_entry, work);
	unsigned long flags;
	ktime_t uninitialized_var(calltime), delta, rettime;

	/*                             */
	if (initcall_debug && system_state == SYSTEM_BOOTING) {
		printk(KERN_DEBUG "calling  %lli_%pF @ %i\n",
			(long long)entry->cookie,
			entry->func, task_pid_nr(current));
		calltime = ktime_get();
	}
	entry->func(entry->data, entry->cookie);
	if (initcall_debug && system_state == SYSTEM_BOOTING) {
		rettime = ktime_get();
		delta = ktime_sub(rettime, calltime);
		printk(KERN_DEBUG "initcall %lli_%pF returned 0 after %lld usecs\n",
			(long long)entry->cookie,
			entry->func,
			(long long)ktime_to_ns(delta) >> 10);
	}

	/*                                        */
	spin_lock_irqsave(&async_lock, flags);
	list_del_init(&entry->domain_list);
	list_del_init(&entry->global_list);

	/*                   */
	kfree(entry);
	atomic_dec(&entry_count);

	spin_unlock_irqrestore(&async_lock, flags);

	/*                        */
	wake_up(&async_done);
}

static async_cookie_t __async_schedule(async_func_t func, void *data, struct async_domain *domain)
{
	struct async_entry *entry;
	unsigned long flags;
	async_cookie_t newcookie;

	/*                       */
	entry = kzalloc(sizeof(struct async_entry), GFP_ATOMIC);

	/*
                                                      
                                              
  */
	if (!entry || atomic_read(&entry_count) > MAX_WORK) {
		kfree(entry);
		spin_lock_irqsave(&async_lock, flags);
		newcookie = next_cookie++;
		spin_unlock_irqrestore(&async_lock, flags);

		/*                                   */
		func(data, newcookie);
		return newcookie;
	}
	INIT_LIST_HEAD(&entry->domain_list);
	INIT_LIST_HEAD(&entry->global_list);
	INIT_WORK(&entry->work, async_run_entry_fn);
	entry->func = func;
	entry->data = data;
	entry->domain = domain;

	spin_lock_irqsave(&async_lock, flags);

	/*                           */
	newcookie = entry->cookie = next_cookie++;

	list_add_tail(&entry->domain_list, &domain->pending);
	if (domain->registered)
		list_add_tail(&entry->global_list, &async_global_pending);

	atomic_inc(&entry_count);
	spin_unlock_irqrestore(&async_lock, flags);

	/*                                                                  */
	current->flags |= PF_USED_ASYNC;

	/*                        */
	queue_work(system_unbound_wq, &entry->work);

	return newcookie;
}

/* 
                                                                  
                                            
                                              
  
                                                                      
                                                                        
 */
async_cookie_t async_schedule(async_func_t func, void *data)
{
	return __async_schedule(func, data, &async_dfl_domain);
}
EXPORT_SYMBOL_GPL(async_schedule);

/* 
                                                                                                 
                                            
                                              
                      
  
                                                                      
                                                                       
                                                                        
                                                                        
                                                    
 */
async_cookie_t async_schedule_domain(async_func_t func, void *data,
				     struct async_domain *domain)
{
	return __async_schedule(func, data, domain);
}
EXPORT_SYMBOL_GPL(async_schedule_domain);

/* 
                                                                       
  
                                                                            
 */
void async_synchronize_full(void)
{
	async_synchronize_full_domain(NULL);
}
EXPORT_SYMBOL_GPL(async_synchronize_full);

/* 
                                                                            
                                                                            
  
                                                                         
                                                        
  
                                                             
 */
void async_unregister_domain(struct async_domain *domain)
{
	spin_lock_irq(&async_lock);
	WARN_ON(!domain->registered || !list_empty(&domain->pending));
	domain->registered = 0;
	spin_unlock_irq(&async_lock);
}
EXPORT_SYMBOL_GPL(async_unregister_domain);

/* 
                                                                                                
                                     
  
                                                                    
                                                              
 */
void async_synchronize_full_domain(struct async_domain *domain)
{
	async_synchronize_cookie_domain(ASYNC_COOKIE_MAX, domain);
}
EXPORT_SYMBOL_GPL(async_synchronize_full_domain);

/* 
                                                                                                                              
                                               
                                                                        
  
                                                                    
                                                                         
                  
 */
void async_synchronize_cookie_domain(async_cookie_t cookie, struct async_domain *domain)
{
	ktime_t uninitialized_var(starttime), delta, endtime;

	if (initcall_debug && system_state == SYSTEM_BOOTING) {
		printk(KERN_DEBUG "async_waiting @ %i\n", task_pid_nr(current));
		starttime = ktime_get();
	}

	wait_event(async_done, lowest_in_progress(domain) >= cookie);

	if (initcall_debug && system_state == SYSTEM_BOOTING) {
		endtime = ktime_get();
		delta = ktime_sub(endtime, starttime);

		printk(KERN_DEBUG "async_continuing @ %i after %lli usec\n",
			task_pid_nr(current),
			(long long)ktime_to_ns(delta) >> 10);
	}
}
EXPORT_SYMBOL_GPL(async_synchronize_cookie_domain);

/* 
                                                                                               
                                               
  
                                                                             
                  
 */
void async_synchronize_cookie(async_cookie_t cookie)
{
	async_synchronize_cookie_domain(cookie, &async_dfl_domain);
}
EXPORT_SYMBOL_GPL(async_synchronize_cookie);

/* 
                                                       
  
                                                     
 */
bool current_is_async(void)
{
	struct worker *worker = current_wq_worker();

	return worker && worker->current_func == async_run_entry_fn;
}
