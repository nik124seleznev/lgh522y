#ifndef _LINUX_CGROUP_H
#define _LINUX_CGROUP_H
/*
 *  cgroup interface
 *
 *  Copyright (C) 2003 BULL SA
 *  Copyright (C) 2004-2006 Silicon Graphics, Inc.
 *
 */

#include <linux/sched.h>
#include <linux/cpumask.h>
#include <linux/nodemask.h>
#include <linux/rcupdate.h>
#include <linux/rculist.h>
#include <linux/cgroupstats.h>
#include <linux/prio_heap.h>
#include <linux/rwsem.h>
#include <linux/idr.h>
#include <linux/workqueue.h>
#include <linux/xattr.h>
#include <linux/fs.h>

#ifdef CONFIG_CGROUPS

struct cgroupfs_root;
struct cgroup_subsys;
struct inode;
struct cgroup;
struct css_id;
struct eventfd_ctx;

extern int cgroup_init_early(void);
extern int cgroup_init(void);
extern void cgroup_fork(struct task_struct *p);
extern void cgroup_post_fork(struct task_struct *p);
extern void cgroup_exit(struct task_struct *p, int run_callbacks);
extern int cgroupstats_build(struct cgroupstats *stats,
				struct dentry *dentry);
extern int cgroup_load_subsys(struct cgroup_subsys *ss);
extern void cgroup_unload_subsys(struct cgroup_subsys *ss);

extern int proc_cgroup_show(struct seq_file *, void *);

/*
                                                   
  
                                                              
 */
#define SUBSYS(_x) _x ## _subsys_id,
enum cgroup_subsys_id {
#define IS_SUBSYS_ENABLED(option) IS_BUILTIN(option)
#include <linux/cgroup_subsys.h>
#undef IS_SUBSYS_ENABLED
	CGROUP_BUILTIN_SUBSYS_COUNT,

	__CGROUP_SUBSYS_TEMP_PLACEHOLDER = CGROUP_BUILTIN_SUBSYS_COUNT - 1,

#define IS_SUBSYS_ENABLED(option) IS_MODULE(option)
#include <linux/cgroup_subsys.h>
#undef IS_SUBSYS_ENABLED
	CGROUP_SUBSYS_COUNT,
};
#undef SUBSYS

/*                                                          */
struct cgroup_subsys_state {
	/*
                                                         
                                                     
                       
  */
	struct cgroup *cgroup;

	/*
                                                             
                                                   
                               
  */

	atomic_t refcnt;

	unsigned long flags;
	/*                              */
	struct css_id __rcu *id;

	/*                                                   */
	struct work_struct dput_work;
};

/*                                                */
enum {
	CSS_ROOT	= (1 << 0), /*                                       */
	CSS_ONLINE	= (1 << 1), /*                                            */
};

/*                                                        */
static inline void __css_get(struct cgroup_subsys_state *css, int count)
{
	atomic_add(count, &css->refcnt);
}

/*
                                                                
                                
                                                 
                                    
 */

static inline void css_get(struct cgroup_subsys_state *css)
{
	/*                                                 */
	if (!(css->flags & CSS_ROOT))
		__css_get(css, 1);
}

/*
                                                                  
                                                                      
                              
 */

extern bool __css_tryget(struct cgroup_subsys_state *css);
static inline bool css_tryget(struct cgroup_subsys_state *css)
{
	if (css->flags & CSS_ROOT)
		return true;
	return __css_tryget(css);
}

/*
                                                             
                            
 */

extern void __css_put(struct cgroup_subsys_state *css);
static inline void css_put(struct cgroup_subsys_state *css)
{
	if (!(css->flags & CSS_ROOT))
		__css_put(css);
}

/*                                   */
enum {
	/*                       */
	CGRP_REMOVED,
	/*
                                                              
                                                         
  */
	CGRP_RELEASABLE,
	/*                                                           */
	CGRP_NOTIFY_ON_RELEASE,
	/*
                                                              
                                                              
                                                         
  */
	CGRP_CPUSET_CLONE_CHILDREN,
	/*                                                           */
	CGRP_SANE_BEHAVIOR,
};

struct cgroup_name {
	struct rcu_head rcu_head;
	char name[];
};

struct cgroup {
	unsigned long flags;		/*                                */

	/*
                                                          
                                                          
  */
	atomic_t count;

	int id;				/*                               */

	/*
                                                              
                                                          
  */
	struct list_head sibling;	/*                      */
	struct list_head children;	/*             */
	struct list_head files;		/*          */

	struct cgroup *parent;		/*           */
	struct dentry *dentry;		/*                                */

	/*
                                                             
                                                 
   
                                                              
                                                           
                                      
   
                                                             
                       
  */
	struct cgroup_name __rcu *name;

	/*                                                */
	struct cgroup_subsys_state *subsys[CGROUP_SUBSYS_COUNT];

	struct cgroupfs_root *root;

	/*
                                                     
                                                   
  */
	struct list_head css_sets;

	struct list_head allcg_node;	/*                           */
	struct list_head cft_q_node;	/*                           */

	/*
                                                    
                                                            
                     
  */
	struct list_head release_list;

	/*
                                                                      
                                  
  */
	struct list_head pidlists;
	struct mutex pidlist_mutex;

	/*                            */
	struct rcu_head rcu_head;
	struct work_struct free_work;

	/*                                                */
	struct list_head event_list;
	spinlock_t event_list_lock;

	/*                  */
	struct simple_xattrs xattrs;
};

#define MAX_CGROUP_ROOT_NAMELEN 64

/*                      */
enum {
	/*
                                                                  
                                                                   
                                                                    
                                                                 
                                    
   
                                                              
                                                                  
                                                                
                                                               
                                                                    
         
   
                                                                  
   
                                                                   
                                                               
   
                                                                
            
   
                            
   
                                                                   
                              
   
                                       
   
                                                                    
                               
  */
	CGRP_ROOT_SANE_BEHAVIOR	= (1 << 0),

	CGRP_ROOT_NOPREFIX	= (1 << 1), /*                                         */
	CGRP_ROOT_XATTR		= (1 << 2), /*                              */
};

/*
                                                                        
                                                                     
                                                                    
 */
struct cgroupfs_root {
	struct super_block *sb;

	/*
                                                             
             
  */
	unsigned long subsys_mask;

	/*                               */
	int hierarchy_id;

	/*                                                                */
	unsigned long actual_subsys_mask;

	/*                                                */
	struct list_head subsys_list;

	/*                                    */
	struct cgroup top_cgroup;

	/*                                                            */
	int number_of_cgroups;

	/*                                               */
	struct list_head root_list;

	/*                                                  */
	struct list_head allcg_list;

	/*                          */
	unsigned long flags;

	/*                                   */
	struct ida cgroup_ida;

	/*                                            */
	char release_agent_path[PATH_MAX];

	/*                                            */
	char name[MAX_CGROUP_ROOT_NAMELEN];
};

/*
                                                        
                                                                   
                                                                   
                                                                     
                  
 */

struct css_set {

	/*                 */
	atomic_t refcount;

	/*
                                                           
                                   
  */
	struct hlist_node hlist;

	/*
                                                    
                                    
  */
	struct list_head tasks;

	/*
                                                      
                                                      
                
  */
	struct list_head cg_links;

	/*
                                                               
                                                           
                                                                      
                      
  */
	struct cgroup_subsys_state *subsys[CGROUP_SUBSYS_COUNT];

	/*                            */
	struct rcu_head rcu_head;
};

/*
                                                                     
                
 */

struct cgroup_map_cb {
	int (*fill)(struct cgroup_map_cb *cb, const char *key, u64 value);
	void *state;
};

/*
                                                              
  
                                  
                                                            
                                                         
 */

/*               */
#define CFTYPE_ONLY_ON_ROOT	(1U << 0)	/*                        */
#define CFTYPE_NOT_ON_ROOT	(1U << 1)	/*                         */
#define CFTYPE_INSANE		(1U << 2)	/*                               */

#define MAX_CFTYPE_NAME		64

struct cftype {
	/*
                                                             
                                                                  
                        
  */
	char name[MAX_CFTYPE_NAME];
	int private;
	/*
                                                               
                                
  */
	umode_t mode;

	/*
                                                              
                                             
  */
	size_t max_write_len;

	/*                */
	unsigned int flags;

	int (*open)(struct inode *inode, struct file *file);
	ssize_t (*read)(struct cgroup *cgrp, struct cftype *cft,
			struct file *file,
			char __user *buf, size_t nbytes, loff_t *ppos);
	/*
                                                               
                                             
  */
	u64 (*read_u64)(struct cgroup *cgrp, struct cftype *cft);
	/*
                                                
  */
	s64 (*read_s64)(struct cgroup *cgrp, struct cftype *cft);
	/*
                                                      
                                                           
                                                              
                           
  */
	int (*read_map)(struct cgroup *cont, struct cftype *cft,
			struct cgroup_map_cb *cb);
	/*
                                                              
                  
  */
	int (*read_seq_string)(struct cgroup *cont, struct cftype *cft,
			       struct seq_file *m);

	ssize_t (*write)(struct cgroup *cgrp, struct cftype *cft,
			 struct file *file,
			 const char __user *buf, size_t nbytes, loff_t *ppos);

	/*
                                                              
                                                        
                                                          
  */
	int (*write_u64)(struct cgroup *cgrp, struct cftype *cft, u64 val);
	/*
                                                  
  */
	int (*write_s64)(struct cgroup *cgrp, struct cftype *cft, s64 val);

	/*
                                                         
                                                         
                                
  */
	int (*write_string)(struct cgroup *cgrp, struct cftype *cft,
			    const char *buffer);
	/*
                                                            
                                                              
                                                          
                               
  */
	int (*trigger)(struct cgroup *cgrp, unsigned int event);

	int (*release)(struct inode *inode, struct file *file);

	/*
                                                               
                                                             
                                                                
                                                 
  */
	int (*register_event)(struct cgroup *cgrp, struct cftype *cft,
			struct eventfd_ctx *eventfd, const char *args);
	/*
                                                             
                                             
                                                          
                               
  */
	void (*unregister_event)(struct cgroup *cgrp, struct cftype *cft,
			struct eventfd_ctx *eventfd);
};

/*
                                                                           
                                                                     
                                  
 */
struct cftype_set {
	struct list_head		node;	/*                            */
	struct cftype			*cfts;
};

struct cgroup_scanner {
	struct cgroup *cg;
	int (*test_task)(struct task_struct *p, struct cgroup_scanner *scan);
	void (*process_task)(struct task_struct *p,
			struct cgroup_scanner *scan);
	struct ptr_heap *heap;
	void *data;
};

/*
                                                                   
                                                         
 */
static inline bool cgroup_sane_behavior(const struct cgroup *cgrp)
{
	return cgrp->root->flags & CGRP_ROOT_SANE_BEHAVIOR;
}

/*                                    */
static inline const char *cgroup_name(const struct cgroup *cgrp)
{
	return rcu_dereference(cgrp->name)->name;
}

int cgroup_add_cftypes(struct cgroup_subsys *ss, struct cftype *cfts);
int cgroup_rm_cftypes(struct cgroup_subsys *ss, struct cftype *cfts);

int cgroup_is_removed(const struct cgroup *cgrp);
bool cgroup_is_descendant(struct cgroup *cgrp, struct cgroup *ancestor);

int cgroup_path(const struct cgroup *cgrp, char *buf, int buflen);

int cgroup_task_count(const struct cgroup *cgrp);

/*
                                                                           
           
 */
struct cgroup_taskset;
struct task_struct *cgroup_taskset_first(struct cgroup_taskset *tset);
struct task_struct *cgroup_taskset_next(struct cgroup_taskset *tset);
struct cgroup *cgroup_taskset_cur_cgroup(struct cgroup_taskset *tset);
int cgroup_taskset_size(struct cgroup_taskset *tset);

/* 
                                                   
                         
                                                                               
                            
 */
#define cgroup_taskset_for_each(task, skip_cgrp, tset)			\
	for ((task) = cgroup_taskset_first((tset)); (task);		\
	     (task) = cgroup_taskset_next((tset)))			\
		if (!(skip_cgrp) ||					\
		    cgroup_taskset_cur_cgroup((tset)) != (skip_cgrp))

/*
                                
                                                    
 */

struct cgroup_subsys {
	struct cgroup_subsys_state *(*css_alloc)(struct cgroup *cgrp);
	int (*css_online)(struct cgroup *cgrp);
	void (*css_offline)(struct cgroup *cgrp);
	void (*css_free)(struct cgroup *cgrp);

	int (*allow_attach)(struct cgroup *cgrp, struct cgroup_taskset *tset);
	int (*can_attach)(struct cgroup *cgrp, struct cgroup_taskset *tset);
	void (*cancel_attach)(struct cgroup *cgrp, struct cgroup_taskset *tset);
	void (*attach)(struct cgroup *cgrp, struct cgroup_taskset *tset);
	void (*fork)(struct task_struct *task);
	void (*exit)(struct cgroup *cgrp, struct cgroup *old_cgrp,
		     struct task_struct *task);
	void (*bind)(struct cgroup *root);

	int subsys_id;
	int disabled;
	int early_init;
	/*
                                                                         
                                       
  */
	bool use_id;

	/*
                                                        
                                                                  
                                                                    
                                                             
                                                          
   
                                                                    
                                                              
                                                            
                                       
  */
	bool broken_hierarchy;
	bool warned_broken_hierarchy;

#define MAX_CGROUP_TYPE_NAMELEN 32
	const char *name;

	/*
                                                        
                              
  */
	struct cgroupfs_root *root;
	struct list_head sibling;
	/*                          */
	struct idr idr;
	spinlock_t id_lock;

	/*                     */
	struct list_head cftsets;

	/*                                                               */
	struct cftype *base_cftypes;
	struct cftype_set base_cftset;

	/*                                              */
	struct module *module;
};

#define SUBSYS(_x) extern struct cgroup_subsys _x ## _subsys;
#define IS_SUBSYS_ENABLED(option) IS_BUILTIN(option)
#include <linux/cgroup_subsys.h>
#undef IS_SUBSYS_ENABLED
#undef SUBSYS

static inline struct cgroup_subsys_state *cgroup_subsys_state(
	struct cgroup *cgrp, int subsys_id)
{
	return cgrp->subsys[subsys_id];
}

/* 
                                                                            
                                        
                                                                           
  
                                                                          
                                                                        
                                                                         
                                                                        
  
                                                                           
                                                            
 */
#ifdef CONFIG_PROVE_RCU
extern struct mutex cgroup_mutex;
#define task_css_set_check(task, __c)					\
	rcu_dereference_check((task)->cgroups,				\
		lockdep_is_held(&(task)->alloc_lock) ||			\
		lockdep_is_held(&cgroup_mutex) || (__c))
#else
#define task_css_set_check(task, __c)					\
	rcu_dereference((task)->cgroups)
#endif

/* 
                                                                                
                         
                                      
                                                                           
  
                                                                        
                                                              
 */
#define task_subsys_state_check(task, subsys_id, __c)			\
	task_css_set_check((task), (__c))->subsys[(subsys_id)]

/* 
                                         
                                        
  
                            
 */
static inline struct css_set *task_css_set(struct task_struct *task)
{
	return task_css_set_check(task, false);
}

/* 
                                                    
                         
                                      
  
                                 
 */
static inline struct cgroup_subsys_state *
task_subsys_state(struct task_struct *task, int subsys_id)
{
	return task_subsys_state_check(task, subsys_id, false);
}

static inline struct cgroup* task_cgroup(struct task_struct *task,
					       int subsys_id)
{
	return task_subsys_state(task, subsys_id)->cgroup;
}

/* 
                                                               
                                               
                                         
  
                                                                           
                                                                      
                                                                         
                                                    
  
                                                                           
                                                                       
                                                     
 */
#define cgroup_for_each_child(pos, cgroup)				\
	list_for_each_entry_rcu(pos, &(cgroup)->children, sibling)

struct cgroup *cgroup_next_descendant_pre(struct cgroup *pos,
					  struct cgroup *cgroup);
struct cgroup *cgroup_rightmost_descendant(struct cgroup *pos);

/* 
                                                                            
                                               
                                            
  
                                                                        
                                                                        
                                                                      
                                                                
  
                                                                           
                                                                   
                                                                    
                                                     
  
                                                                          
                                  
  
                   
    
                                
                                    
               
    
  
                         
    
              
                        
                
  
                                                
              
                                                             
                
    
    
  
                                                                          
                                                                          
                                                                        
                                                                        
                                                                         
                                                                         
                                                                    
          
  
                                                                          
                                                               
  
                                                                       
                                                                      
              
 */
#define cgroup_for_each_descendant_pre(pos, cgroup)			\
	for (pos = cgroup_next_descendant_pre(NULL, (cgroup)); (pos);	\
	     pos = cgroup_next_descendant_pre((pos), (cgroup)))

struct cgroup *cgroup_next_descendant_post(struct cgroup *pos,
					   struct cgroup *cgroup);

/* 
                                                                              
                                               
                                            
  
                                                                      
                                                                           
                                                             
 */
#define cgroup_for_each_descendant_post(pos, cgroup)			\
	for (pos = cgroup_next_descendant_post(NULL, (cgroup)); (pos);	\
	     pos = cgroup_next_descendant_post((pos), (cgroup)))

/*                                                     */
struct cgroup_iter {
	struct list_head *cg_link;
	struct list_head *task;
};

/*
                                           
  
                                                      
  
                                                               
                                                         
  
                                                     
  
                                                                  
                                                                   
                                                                     
            
 */
void cgroup_iter_start(struct cgroup *cgrp, struct cgroup_iter *it);
struct task_struct *cgroup_iter_next(struct cgroup *cgrp,
					struct cgroup_iter *it);
void cgroup_iter_end(struct cgroup *cgrp, struct cgroup_iter *it);
int cgroup_scan_tasks(struct cgroup_scanner *scan);
int cgroup_attach_task_all(struct task_struct *from, struct task_struct *);
int cgroup_transfer_tasks(struct cgroup *to, struct cgroup *from);

/*
                                                                             
                                                                               
                                                                 
                                                                        
                                                          
  
                                                                           
                                                            
                                                                            
                                                               
 */

/*
                                                                 
                       
 */
void free_css_id(struct cgroup_subsys *ss, struct cgroup_subsys_state *css);

/*                                               */

struct cgroup_subsys_state *css_lookup(struct cgroup_subsys *ss, int id);

/*                                        */
bool css_is_ancestor(struct cgroup_subsys_state *cg,
		     const struct cgroup_subsys_state *root);

/*                         */
unsigned short css_id(struct cgroup_subsys_state *css);
unsigned short css_depth(struct cgroup_subsys_state *css);
struct cgroup_subsys_state *cgroup_css_from_dir(struct file *f, int id);

#else /*                 */

static inline int cgroup_init_early(void) { return 0; }
static inline int cgroup_init(void) { return 0; }
static inline void cgroup_fork(struct task_struct *p) {}
static inline void cgroup_post_fork(struct task_struct *p) {}
static inline void cgroup_exit(struct task_struct *p, int callbacks) {}

static inline void cgroup_lock(void) {}
static inline void cgroup_unlock(void) {}
static inline int cgroupstats_build(struct cgroupstats *stats,
					struct dentry *dentry)
{
	return -EINVAL;
}

/*                            */
static inline int cgroup_attach_task_all(struct task_struct *from,
					 struct task_struct *t)
{
	return 0;
}

#endif /*                 */

#endif /*                 */
