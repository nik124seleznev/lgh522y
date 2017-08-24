/*
 * cgroup_freezer.c -  control group freezer subsystem
 *
 * Copyright IBM Corporation, 2007
 *
 * Author : Cedric Le Goater <clg@fr.ibm.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/export.h>
#include <linux/slab.h>
#include <linux/cgroup.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/freezer.h>
#include <linux/seq_file.h>

/*
                                                                        
                                                                         
                                                                          
                                                                        
                                       
 */
enum freezer_state_flags {
	CGROUP_FREEZER_ONLINE	= (1 << 0), /*                         */
	CGROUP_FREEZING_SELF	= (1 << 1), /*                          */
	CGROUP_FREEZING_PARENT	= (1 << 2), /*                                */
	CGROUP_FROZEN		= (1 << 3), /*                                 */

	/*                             */
	CGROUP_FREEZING		= CGROUP_FREEZING_SELF | CGROUP_FREEZING_PARENT,
};

struct freezer {
	struct cgroup_subsys_state	css;
	unsigned int			state;
	spinlock_t			lock;
};

static inline struct freezer *cgroup_freezer(struct cgroup *cgroup)
{
	return container_of(cgroup_subsys_state(cgroup, freezer_subsys_id),
			    struct freezer, css);
}

static inline struct freezer *task_freezer(struct task_struct *task)
{
	return container_of(task_subsys_state(task, freezer_subsys_id),
			    struct freezer, css);
}

static struct freezer *parent_freezer(struct freezer *freezer)
{
	struct cgroup *pcg = freezer->css.cgroup->parent;

	if (pcg)
		return cgroup_freezer(pcg);
	return NULL;
}

bool cgroup_freezing(struct task_struct *task)
{
	bool ret;

	rcu_read_lock();
	ret = task_freezer(task)->state & CGROUP_FREEZING;
	rcu_read_unlock();

	return ret;
}

/*
                                                                     
                           
 */
static const char *freezer_state_strs(unsigned int state)
{
	if (state & CGROUP_FROZEN)
		return "FROZEN";
	if (state & CGROUP_FREEZING)
		return "FREEZING";
	return "THAWED";
};

struct cgroup_subsys freezer_subsys;

static struct cgroup_subsys_state *freezer_css_alloc(struct cgroup *cgroup)
{
	struct freezer *freezer;

	freezer = kzalloc(sizeof(struct freezer), GFP_KERNEL);
	if (!freezer)
		return ERR_PTR(-ENOMEM);

	spin_lock_init(&freezer->lock);
	return &freezer->css;
}

/* 
                                                           
                                
  
                                                                       
                                                              
                 
 */
static int freezer_css_online(struct cgroup *cgroup)
{
	struct freezer *freezer = cgroup_freezer(cgroup);
	struct freezer *parent = parent_freezer(freezer);

	/*
                                                               
                                                               
                                                              
  */
	if (parent)
		spin_lock_irq(&parent->lock);
	spin_lock_nested(&freezer->lock, SINGLE_DEPTH_NESTING);

	freezer->state |= CGROUP_FREEZER_ONLINE;

	if (parent && (parent->state & CGROUP_FREEZING)) {
		freezer->state |= CGROUP_FREEZING_PARENT | CGROUP_FROZEN;
		atomic_inc(&system_freezing_cnt);
	}

	spin_unlock(&freezer->lock);
	if (parent)
		spin_unlock_irq(&parent->lock);

	return 0;
}

/* 
                                                        
                                  
  
                                                                           
                         
 */
static void freezer_css_offline(struct cgroup *cgroup)
{
	struct freezer *freezer = cgroup_freezer(cgroup);

	spin_lock_irq(&freezer->lock);

	if (freezer->state & CGROUP_FREEZING)
		atomic_dec(&system_freezing_cnt);

	freezer->state = 0;

	spin_unlock_irq(&freezer->lock);
}

static void freezer_css_free(struct cgroup *cgroup)
{
	kfree(cgroup_freezer(cgroup));
}

/*
                                                                           
                                                                       
                                
  
                                                                
                                                                       
                                                                       
 */
static void freezer_attach(struct cgroup *new_cgrp, struct cgroup_taskset *tset)
{
	struct freezer *freezer = cgroup_freezer(new_cgrp);
	struct task_struct *task;
	bool clear_frozen = false;

	spin_lock_irq(&freezer->lock);

	/*
                                                                 
                                                                  
                                                                  
                        
   
                                                              
                                                                    
                                                                   
  */
	cgroup_taskset_for_each(task, new_cgrp, tset) {
		if (!(freezer->state & CGROUP_FREEZING)) {
			__thaw_task(task);
		} else {
			freeze_task(task);
			freezer->state &= ~CGROUP_FROZEN;
			clear_frozen = true;
		}
	}

	spin_unlock_irq(&freezer->lock);

	/*
                                                        
                                                                  
                                                                  
                                                                
                
  */
	while (clear_frozen && (freezer = parent_freezer(freezer))) {
		spin_lock_irq(&freezer->lock);
		freezer->state &= ~CGROUP_FROZEN;
		clear_frozen = freezer->state & CGROUP_FREEZING;
		spin_unlock_irq(&freezer->lock);
	}
}

static void freezer_fork(struct task_struct *task)
{
	struct freezer *freezer;

	rcu_read_lock();
	freezer = task_freezer(task);

	/*
                                                        
                    
  */
	if (!freezer->css.cgroup->parent)
		goto out;

	spin_lock_irq(&freezer->lock);
	if (freezer->state & CGROUP_FREEZING)
		freeze_task(task);
	spin_unlock_irq(&freezer->lock);
out:
	rcu_read_unlock();
}

/* 
                                                               
                              
  
                                                                        
                                                                           
                                                                           
                                                     
  
                                                                   
                                                                         
  
                                                                     
                                                                          
                                                          
 */
static void update_if_frozen(struct cgroup *cgroup)
{
	struct freezer *freezer = cgroup_freezer(cgroup);
	struct cgroup *pos;
	struct cgroup_iter it;
	struct task_struct *task;

	WARN_ON_ONCE(!rcu_read_lock_held());

	spin_lock_irq(&freezer->lock);

	if (!(freezer->state & CGROUP_FREEZING) ||
	    (freezer->state & CGROUP_FROZEN))
		goto out_unlock;

	/*                                 */
	cgroup_for_each_child(pos, cgroup) {
		struct freezer *child = cgroup_freezer(pos);

		if ((child->state & CGROUP_FREEZER_ONLINE) &&
		    !(child->state & CGROUP_FROZEN))
			goto out_unlock;
	}

	/*                       */
	cgroup_iter_start(cgroup, &it);

	while ((task = cgroup_iter_next(cgroup, &it))) {
		if (freezing(task)) {
			/*
                                                   
                                                 
                                                    
                                 
    */
			if (!frozen(task) && !freezer_should_skip(task))
				goto out_iter_end;
		}
	}

	freezer->state |= CGROUP_FROZEN;
out_iter_end:
	cgroup_iter_end(cgroup, &it);
out_unlock:
	spin_unlock_irq(&freezer->lock);
}

static int freezer_read(struct cgroup *cgroup, struct cftype *cft,
			struct seq_file *m)
{
	struct cgroup *pos;

	rcu_read_lock();

	/*                         */
	cgroup_for_each_descendant_post(pos, cgroup)
		update_if_frozen(pos);
	update_if_frozen(cgroup);

	rcu_read_unlock();

	seq_puts(m, freezer_state_strs(cgroup_freezer(cgroup)->state));
	seq_putc(m, '\n');
	return 0;
}

static void freeze_cgroup(struct freezer *freezer)
{
	struct cgroup *cgroup = freezer->css.cgroup;
	struct cgroup_iter it;
	struct task_struct *task;

	cgroup_iter_start(cgroup, &it);
	while ((task = cgroup_iter_next(cgroup, &it)))
		freeze_task(task);
	cgroup_iter_end(cgroup, &it);
}

static void unfreeze_cgroup(struct freezer *freezer)
{
	struct cgroup *cgroup = freezer->css.cgroup;
	struct cgroup_iter it;
	struct task_struct *task;

	cgroup_iter_start(cgroup, &it);
	while ((task = cgroup_iter_next(cgroup, &it)))
		__thaw_task(task);
	cgroup_iter_end(cgroup, &it);
}

/* 
                                                                      
                                             
                                         
                                                 
  
                                                                   
                                    
 */
static void freezer_apply_state(struct freezer *freezer, bool freeze,
				unsigned int state)
{
	/*                                                                */
	lockdep_assert_held(&freezer->lock);

	if (!(freezer->state & CGROUP_FREEZER_ONLINE))
		return;

	if (freeze) {
		if (!(freezer->state & CGROUP_FREEZING))
			atomic_inc(&system_freezing_cnt);
		freezer->state |= state;
		freeze_cgroup(freezer);
	} else {
		bool was_freezing = freezer->state & CGROUP_FREEZING;

		freezer->state &= ~state;

		if (!(freezer->state & CGROUP_FREEZING)) {
			if (was_freezing)
				atomic_dec(&system_freezing_cnt);
			freezer->state &= ~CGROUP_FROZEN;
			unfreeze_cgroup(freezer);
		}
	}
}

/* 
                                                                       
                                
                                     
  
                                                                    
                                                            
 */
static void freezer_change_state(struct freezer *freezer, bool freeze)
{
	struct cgroup *pos;

	/*                 */
	spin_lock_irq(&freezer->lock);
	freezer_apply_state(freezer, freeze, CGROUP_FREEZING_SELF);
	spin_unlock_irq(&freezer->lock);

	/*
                                                            
                                                                 
                           
  */
	rcu_read_lock();
	cgroup_for_each_descendant_pre(pos, freezer->css.cgroup) {
		struct freezer *pos_f = cgroup_freezer(pos);
		struct freezer *parent = parent_freezer(pos_f);

		/*
                                                             
                                                             
                                                
   */
		spin_lock_irq(&pos_f->lock);
		freezer_apply_state(pos_f, parent->state & CGROUP_FREEZING,
				    CGROUP_FREEZING_PARENT);
		spin_unlock_irq(&pos_f->lock);
	}
	rcu_read_unlock();
}

static int freezer_write(struct cgroup *cgroup, struct cftype *cft,
			 const char *buffer)
{
	bool freeze;

	if (strcmp(buffer, freezer_state_strs(0)) == 0)
		freeze = false;
	else if (strcmp(buffer, freezer_state_strs(CGROUP_FROZEN)) == 0)
		freeze = true;
	else
		return -EINVAL;

	freezer_change_state(cgroup_freezer(cgroup), freeze);
	return 0;
}

static u64 freezer_self_freezing_read(struct cgroup *cgroup, struct cftype *cft)
{
	struct freezer *freezer = cgroup_freezer(cgroup);

	return (bool)(freezer->state & CGROUP_FREEZING_SELF);
}

static u64 freezer_parent_freezing_read(struct cgroup *cgroup, struct cftype *cft)
{
	struct freezer *freezer = cgroup_freezer(cgroup);

	return (bool)(freezer->state & CGROUP_FREEZING_PARENT);
}

static struct cftype files[] = {
	{
		.name = "state",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_seq_string = freezer_read,
		.write_string = freezer_write,
	},
	{
		.name = "self_freezing",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_u64 = freezer_self_freezing_read,
	},
	{
		.name = "parent_freezing",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_u64 = freezer_parent_freezing_read,
	},
	{ }	/*           */
};

struct cgroup_subsys freezer_subsys = {
	.name		= "freezer",
	.css_alloc	= freezer_css_alloc,
	.css_online	= freezer_css_online,
	.css_offline	= freezer_css_offline,
	.css_free	= freezer_css_free,
	.subsys_id	= freezer_subsys_id,
	.attach		= freezer_attach,
	.fork		= freezer_fork,
	.base_cftypes	= files,
};
