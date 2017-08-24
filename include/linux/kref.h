/*
 * kref.h - library routines for handling generic reference counted objects
 *
 * Copyright (C) 2004 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2004 IBM Corp.
 *
 * based on kobject.h which was:
 * Copyright (C) 2002-2003 Patrick Mochel <mochel@osdl.org>
 * Copyright (C) 2002-2003 Open Source Development Labs
 *
 * This file is released under the GPLv2.
 *
 */

#ifndef _KREF_H_
#define _KREF_H_

#include <linux/bug.h>
#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>

struct kref {
	atomic_t refcount;
};

/* 
                                 
                             
 */
static inline void kref_init(struct kref *kref)
{
	atomic_set(&kref->refcount, 1);
}

/* 
                                            
                 
 */
static inline void kref_get(struct kref *kref)
{
	/*                                                          
                                                                       
                                                      
  */
	WARN_ON_ONCE(atomic_inc_return(&kref->refcount) < 2);
}

/* 
                                                        
                 
                                          
                                                                           
                                                 
                                                                        
                                                                   
                                                                     
                                                                       
                    
  
                                                               
                                                                           
                                                                            
                                                                           
                     
 */
static inline int kref_sub(struct kref *kref, unsigned int count,
	     void (*release)(struct kref *kref))
{
	WARN_ON(release == NULL);

	if (atomic_sub_and_test((int) count, &kref->refcount)) {
		release(kref);
		return 1;
	}
	return 0;
}

/* 
                                            
                 
                                                                           
                                                 
                                                                        
                                                                   
                                                                     
                                                                       
                    
  
                                                    
                                                                           
                                                                            
                                                                           
                     
 */
static inline int kref_put(struct kref *kref, void (*release)(struct kref *kref))
{
	return kref_sub(kref, 1, release);
}

/* 
                                                             
                 
                                                                           
                                                 
                                                                        
                            
                                      
  
                                                                            
                                                                              
                                                                              
 */
static inline int kref_put_spinlock_irqsave(struct kref *kref,
		void (*release)(struct kref *kref),
		spinlock_t *lock)
{
	unsigned long flags;

	WARN_ON(release == NULL);
	if (atomic_add_unless(&kref->refcount, -1, 1))
		return 0;
	spin_lock_irqsave(lock, flags);
	if (atomic_dec_and_test(&kref->refcount)) {
		release(kref);
		local_irq_restore(flags);
		return 1;
	}
	spin_unlock_irqrestore(lock, flags);
	return 0;
}

static inline int kref_put_mutex(struct kref *kref,
				 void (*release)(struct kref *kref),
				 struct mutex *lock)
{
	WARN_ON(release == NULL);
	if (unlikely(!atomic_add_unless(&kref->refcount, -1, 1))) {
		mutex_lock(lock);
		if (unlikely(!atomic_dec_and_test(&kref->refcount))) {
			mutex_unlock(lock);
			return 0;
		}
		release(kref);
		return 1;
	}
	return 0;
}

/* 
                                                                          
                 
  
                                                                  
  
                                                                       
                                                                       
                                                               
                                                                 
                                                                           
                                                                       
                                                                             
                                                                          
                                                       
 */
static inline int __must_check kref_get_unless_zero(struct kref *kref)
{
	return atomic_add_unless(&kref->refcount, 1, 0);
}
#endif /*          */
