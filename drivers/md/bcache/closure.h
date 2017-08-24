#ifndef _LINUX_CLOSURE_H
#define _LINUX_CLOSURE_H

#include <linux/llist.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

/*
                                                                                
                                                                              
         
  
                     
  
                                                                          
                                                                              
                                      
  
                                                                        
                                                                               
                           
  
                                                                                
                                                                            
                                                                               
                   
  
                              
                                               
  
                                                                               
                                                 
  
                                                                                
                                
  
                                                                                
                                                                                
                                                                               
  
                                        
    
                   
    
  
                    
  
              
                   
                              
                    
  
                   
                   
                              
                    
  
                                                  
  
                                                                                
                                                                            
                                                                             
                                                                              
                   
  
                                                                        
                                                            
  
                                                                             
                                                                                
                                                                          
                                                                        
                                                              
  
                                                                               
  
                                
                                           
                              
  
                                                                                
                                                                                
                                                        
  
                                                                 
                                                                        
                                                                             
                                      
  
                                                                           
                                                                               
                                                                         
  
                                                                         
                                                                              
                                                     
                                                                   
  
                                                                                
                                                                               
                                                                         
                                                                                
                                                                                
                  
  
           
  
                                                                               
                            
  
                                                                                
                                                                            
                                 
  
                               
  
                                                                             
                                                                              
                        
  
                                                                               
                                                                              
                                                       
  
           
  
                                                                           
                                                                        
                                                                          
                                                
  
                                                                                
                                                                            
                                                                                
                                                                               
                                                                                
                                                                               
                                                      
  
                                                                              
                            
  
                           
                     
  
                                                                               
                                                                 
  
                                               
  
                                          
                                 
  
                                                                              
                                                                                
                                                                               
                             
  
                                                                           
          
                             
  
                                                                                
                                                                         
                                                                              
                                                                            
                                      
  
                                                                           
                                                                          
                                                                           
                                     
 */

struct closure;
typedef void (closure_fn) (struct closure *);

struct closure_waitlist {
	struct llist_head	list;
};

enum closure_type {
	TYPE_closure				= 0,
	TYPE_closure_with_waitlist		= 1,
	TYPE_closure_with_timer			= 2,
	TYPE_closure_with_waitlist_and_timer	= 3,
	MAX_CLOSURE_TYPE			= 3,
};

enum closure_state {
	/*
                                                                      
                          
   
                                                                         
                                                                      
                          
   
                                                                         
                                                                        
                                                            
   
                                                                         
                                                                     
                                                                       
   
                                                          
   
                                                           
                                                                       
                                                                    
                                                                     
                                                                        
                                                                     
                                                    
   
                                                                   
                              
  */

	CLOSURE_BITS_START	= (1 << 19),
	CLOSURE_DESTRUCTOR	= (1 << 19),
	CLOSURE_BLOCKING	= (1 << 21),
	CLOSURE_WAITING		= (1 << 23),
	CLOSURE_SLEEPING	= (1 << 25),
	CLOSURE_TIMER		= (1 << 27),
	CLOSURE_RUNNING		= (1 << 29),
	CLOSURE_STACK		= (1 << 31),
};

#define CLOSURE_GUARD_MASK					\
	((CLOSURE_DESTRUCTOR|CLOSURE_BLOCKING|CLOSURE_WAITING|	\
	  CLOSURE_SLEEPING|CLOSURE_TIMER|CLOSURE_RUNNING|CLOSURE_STACK) << 1)

#define CLOSURE_REMAINING_MASK		(CLOSURE_BITS_START - 1)
#define CLOSURE_REMAINING_INITIALIZER	(1|CLOSURE_RUNNING)

struct closure {
	union {
		struct {
			struct workqueue_struct *wq;
			struct task_struct	*task;
			struct llist_node	list;
			closure_fn		*fn;
		};
		struct work_struct	work;
	};

	struct closure		*parent;

	atomic_t		remaining;

	enum closure_type	type;

#ifdef CONFIG_BCACHE_CLOSURES_DEBUG
#define CLOSURE_MAGIC_DEAD	0xc054dead
#define CLOSURE_MAGIC_ALIVE	0xc054a11e

	unsigned		magic;
	struct list_head	all;
	unsigned long		ip;
	unsigned long		waiting_on;
#endif
};

struct closure_with_waitlist {
	struct closure		cl;
	struct closure_waitlist	wait;
};

struct closure_with_timer {
	struct closure		cl;
	struct timer_list	timer;
};

struct closure_with_waitlist_and_timer {
	struct closure		cl;
	struct closure_waitlist	wait;
	struct timer_list	timer;
};

extern unsigned invalid_closure_type(void);

#define __CLOSURE_TYPE(cl, _t)						\
	  __builtin_types_compatible_p(typeof(cl), struct _t)		\
		? TYPE_ ## _t :						\

#define __closure_type(cl)						\
(									\
	__CLOSURE_TYPE(cl, closure)					\
	__CLOSURE_TYPE(cl, closure_with_waitlist)			\
	__CLOSURE_TYPE(cl, closure_with_timer)				\
	__CLOSURE_TYPE(cl, closure_with_waitlist_and_timer)		\
	invalid_closure_type()						\
)

void closure_sub(struct closure *cl, int v);
void closure_put(struct closure *cl);
void closure_queue(struct closure *cl);
void __closure_wake_up(struct closure_waitlist *list);
bool closure_wait(struct closure_waitlist *list, struct closure *cl);
void closure_sync(struct closure *cl);

bool closure_trylock(struct closure *cl, struct closure *parent);
void __closure_lock(struct closure *cl, struct closure *parent,
		    struct closure_waitlist *wait_list);

void do_closure_timer_init(struct closure *cl);
bool __closure_delay(struct closure *cl, unsigned long delay,
		     struct timer_list *timer);
void __closure_flush(struct closure *cl, struct timer_list *timer);
void __closure_flush_sync(struct closure *cl, struct timer_list *timer);

#ifdef CONFIG_BCACHE_CLOSURES_DEBUG

void closure_debug_init(void);
void closure_debug_create(struct closure *cl);
void closure_debug_destroy(struct closure *cl);

#else

static inline void closure_debug_init(void) {}
static inline void closure_debug_create(struct closure *cl) {}
static inline void closure_debug_destroy(struct closure *cl) {}

#endif

static inline void closure_set_ip(struct closure *cl)
{
#ifdef CONFIG_BCACHE_CLOSURES_DEBUG
	cl->ip = _THIS_IP_;
#endif
}

static inline void closure_set_ret_ip(struct closure *cl)
{
#ifdef CONFIG_BCACHE_CLOSURES_DEBUG
	cl->ip = _RET_IP_;
#endif
}

static inline void closure_get(struct closure *cl)
{
#ifdef CONFIG_BCACHE_CLOSURES_DEBUG
	BUG_ON((atomic_inc_return(&cl->remaining) &
		CLOSURE_REMAINING_MASK) <= 1);
#else
	atomic_inc(&cl->remaining);
#endif
}

static inline void closure_set_stopped(struct closure *cl)
{
	atomic_sub(CLOSURE_RUNNING, &cl->remaining);
}

static inline bool closure_is_stopped(struct closure *cl)
{
	return !(atomic_read(&cl->remaining) & CLOSURE_RUNNING);
}

static inline bool closure_is_unlocked(struct closure *cl)
{
	return atomic_read(&cl->remaining) == -1;
}

static inline void do_closure_init(struct closure *cl, struct closure *parent,
				   bool running)
{
	switch (cl->type) {
	case TYPE_closure_with_timer:
	case TYPE_closure_with_waitlist_and_timer:
		do_closure_timer_init(cl);
	default:
		break;
	}

	cl->parent = parent;
	if (parent)
		closure_get(parent);

	if (running) {
		closure_debug_create(cl);
		atomic_set(&cl->remaining, CLOSURE_REMAINING_INITIALIZER);
	} else
		atomic_set(&cl->remaining, -1);

	closure_set_ip(cl);
}

/*
                                                                                
                                                                           
            
 */
#define __to_internal_closure(cl)				\
({								\
	BUILD_BUG_ON(__closure_type(*cl) > MAX_CLOSURE_TYPE);	\
	(struct closure *) cl;					\
})

#define closure_init_type(cl, parent, running)			\
do {								\
	struct closure *_cl = __to_internal_closure(cl);	\
	_cl->type = __closure_type(*(cl));			\
	do_closure_init(_cl, parent, running);			\
} while (0)

/* 
                                                                 
  
                                                                             
 */
#define __closure_init(cl, parent)				\
	closure_init_type(cl, parent, true)

/* 
                                                                   
                              
                                                                            
                          
 */
#define closure_init(cl, parent)				\
do {								\
	memset((cl), 0, sizeof(*(cl)));				\
	__closure_init(cl, parent);				\
} while (0)

static inline void closure_init_stack(struct closure *cl)
{
	memset(cl, 0, sizeof(struct closure));
	atomic_set(&cl->remaining, CLOSURE_REMAINING_INITIALIZER|
		   CLOSURE_BLOCKING|CLOSURE_STACK);
}

/* 
                                                                        
                              
  
                                                                           
                                                     
 */
#define closure_init_unlocked(cl)				\
do {								\
	memset((cl), 0, sizeof(*(cl)));				\
	closure_init_type(cl, NULL, false);			\
} while (0)

/* 
                                                  
                            
                                           
  
                                                                            
                                            
  
                                                                              
                                                                             
            
 */
#define closure_lock(cl, parent)				\
	__closure_lock(__to_internal_closure(cl), parent, &(cl)->wait)

/* 
                                                 
                                    
                               
  
                                                                                
                                                                      
                                                                   
 */
#define closure_delay(cl, delay)			\
	__closure_delay(__to_internal_closure(cl), delay, &(cl)->timer)

#define closure_flush(cl)				\
	__closure_flush(__to_internal_closure(cl), &(cl)->timer)

#define closure_flush_sync(cl)				\
	__closure_flush_sync(__to_internal_closure(cl), &(cl)->timer)

static inline void __closure_end_sleep(struct closure *cl)
{
	__set_current_state(TASK_RUNNING);

	if (atomic_read(&cl->remaining) & CLOSURE_SLEEPING)
		atomic_sub(CLOSURE_SLEEPING, &cl->remaining);
}

static inline void __closure_start_sleep(struct closure *cl)
{
	closure_set_ip(cl);
	cl->task = current;
	set_current_state(TASK_UNINTERRUPTIBLE);

	if (!(atomic_read(&cl->remaining) & CLOSURE_SLEEPING))
		atomic_add(CLOSURE_SLEEPING, &cl->remaining);
}

/* 
                                                                        
  
                                                                              
                                                       
 */
static inline bool closure_blocking(struct closure *cl)
{
	return atomic_read(&cl->remaining) & CLOSURE_BLOCKING;
}

/* 
                                                           
  
                                                                              
                                                       
  
                                                                          
 */
static inline void set_closure_blocking(struct closure *cl)
{
	if (!closure_blocking(cl))
		atomic_add(CLOSURE_BLOCKING, &cl->remaining);
}

/*
                                                                          
 */
static inline void clear_closure_blocking(struct closure *cl)
{
	if (closure_blocking(cl))
		atomic_sub(CLOSURE_BLOCKING, &cl->remaining);
}

/* 
                                                           
 */
static inline void closure_wake_up(struct closure_waitlist *list)
{
	smp_mb();
	__closure_wake_up(list);
}

/*
                                                                                
                    
  
                                                                        
                                                                                
                                                                              
                                                                            
                                              
  
                                                                          
                                                                               
                                                                              
                                  
  
                                                            
                                                                   
                                                                           
                                                                                
       
 */
#define __closure_wait_event(list, cl, condition, _block)		\
({									\
	bool block = _block;						\
	typeof(condition) ret;						\
									\
	while (1) {							\
		ret = (condition);					\
		if (ret) {						\
			__closure_wake_up(list);			\
			if (block)					\
				closure_sync(cl);			\
									\
			break;						\
		}							\
									\
		if (block)						\
			__closure_start_sleep(cl);			\
									\
		if (!closure_wait(list, cl)) {				\
			if (!block)					\
				break;					\
									\
			schedule();					\
		}							\
	}								\
									\
	ret;								\
})

/* 
                                                                               
                                  
                                              
                                                       
  
                                                                               
                                    
  
                                                                       
                                                                            
                                                                               
                                                                             
  
                                                                           
         
  
                                                                               
                                   
 */
#define closure_wait_event(list, cl, condition)				\
	__closure_wait_event(list, cl, condition, closure_blocking(cl))

#define closure_wait_event_async(list, cl, condition)			\
	__closure_wait_event(list, cl, condition, false)

#define closure_wait_event_sync(list, cl, condition)			\
	__closure_wait_event(list, cl, condition, true)

static inline void set_closure_fn(struct closure *cl, closure_fn *fn,
				  struct workqueue_struct *wq)
{
	BUG_ON(object_is_on_stack(cl));
	closure_set_ip(cl);
	cl->fn = fn;
	cl->wq = wq;
	/*                                       */
	smp_mb__before_atomic_dec();
}

#define continue_at(_cl, _fn, _wq)					\
do {									\
	set_closure_fn(_cl, _fn, _wq);					\
	closure_sub(_cl, CLOSURE_RUNNING + 1);				\
	return;								\
} while (0)

#define closure_return(_cl)	continue_at((_cl), NULL, NULL)

#define continue_at_nobarrier(_cl, _fn, _wq)				\
do {									\
	set_closure_fn(_cl, _fn, _wq);					\
	closure_queue(cl);						\
	return;								\
} while (0)

#define closure_return_with_destructor(_cl, _destructor)		\
do {									\
	set_closure_fn(_cl, _destructor, NULL);				\
	closure_sub(_cl, CLOSURE_RUNNING - CLOSURE_DESTRUCTOR + 1);	\
	return;								\
} while (0)

static inline void closure_call(struct closure *cl, closure_fn fn,
				struct workqueue_struct *wq,
				struct closure *parent)
{
	closure_init(cl, parent);
	continue_at_nobarrier(cl, fn, wq);
}

static inline void closure_trylock_call(struct closure *cl, closure_fn fn,
					struct workqueue_struct *wq,
					struct closure *parent)
{
	if (closure_trylock(cl, parent))
		continue_at_nobarrier(cl, fn, wq);
}

#endif /*                  */
