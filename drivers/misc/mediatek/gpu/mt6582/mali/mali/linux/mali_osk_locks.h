/*
 * Copyright (C) 2010-2013 ARM Limited. All rights reserved.
 * 
 * This program is free software and is provided to you under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
 * 
 * A copy of the licence is included with the program, and can also be obtained from Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* 
                         
                                           
 */
#ifndef _MALI_OSK_LOCKS_H
#define _MALI_OSK_LOCKS_H

#include <linux/spinlock.h>
#include <linux/rwsem.h>
#include <linux/mutex.h>

#include <linux/slab.h>

#include "mali_osk_types.h"
#include "mali_kernel_common.h"

#ifdef _cplusplus
extern "C" {
#endif

	/*                                                                                         */
#ifdef DEBUG
	struct _mali_osk_lock_debug_s {
		u32 owner;
		_mali_osk_lock_flags_t orig_flags;
		_mali_osk_lock_order_t order;
		struct _mali_osk_lock_debug_s *next;
	};
#endif

	/*                           */
	struct _mali_osk_spinlock_s {
#ifdef DEBUG
		struct _mali_osk_lock_debug_s checker;
#endif
		spinlock_t spinlock;
	};

	/*                                                                                               */
	struct _mali_osk_spinlock_irq_s {
#ifdef DEBUG
		struct _mali_osk_lock_debug_s checker;
#endif

		spinlock_t spinlock;
		unsigned long flags;
	};

	/*                                   */
	struct _mali_osk_mutex_rw_s {
#ifdef DEBUG
		struct _mali_osk_lock_debug_s checker;
		_mali_osk_lock_mode_t mode;
#endif

		struct rw_semaphore rw_sema;
	};

	/*                                                                         */
	struct _mali_osk_mutex_s {
#ifdef DEBUG
		struct _mali_osk_lock_debug_s checker;
#endif
		struct mutex mutex;
	};

#ifdef DEBUG
	/*                                                                                                 
                                                                                                          
                                                                             */
	void _mali_osk_locks_debug_init(struct _mali_osk_lock_debug_s *checker, _mali_osk_lock_flags_t flags, _mali_osk_lock_order_t order);
	void _mali_osk_locks_debug_add(struct _mali_osk_lock_debug_s *checker);
	void _mali_osk_locks_debug_remove(struct _mali_osk_lock_debug_s *checker);

	/*                                                                              */
	static inline u32 _mali_osk_lock_get_owner(struct _mali_osk_lock_debug_s *lock)
	{
		return lock->owner;
	}
#else
#define _mali_osk_locks_debug_init(x, y, z) do {} while (0)
#define _mali_osk_locks_debug_add(x) do {} while (0)
#define _mali_osk_locks_debug_remove(x) do {} while (0)
#endif

	/*                                                                                                             */
	static inline _mali_osk_spinlock_t *_mali_osk_spinlock_init(_mali_osk_lock_flags_t flags, _mali_osk_lock_order_t order)
	{
		_mali_osk_spinlock_t *lock = NULL;

		lock = kmalloc(sizeof(_mali_osk_spinlock_t), GFP_KERNEL);
		if (NULL == lock) {
			return NULL;
		}
		spin_lock_init(&lock->spinlock);
				   
		#ifdef CONFIG_PROVE_LOCKING
         lockdep_skip_validate(&lock->spinlock.dep_map);
		#endif
		
		_mali_osk_locks_debug_init((struct _mali_osk_lock_debug_s *)lock, flags, order);
		return lock;
	}

	/*                         */
	static inline void  _mali_osk_spinlock_lock(_mali_osk_spinlock_t *lock)
	{
		BUG_ON(NULL == lock);
		spin_lock(&lock->spinlock);
		_mali_osk_locks_debug_add((struct _mali_osk_lock_debug_s *)lock);
	}

	/*                           */
	static inline void _mali_osk_spinlock_unlock(_mali_osk_spinlock_t *lock)
	{
		BUG_ON(NULL == lock);
		_mali_osk_locks_debug_remove((struct _mali_osk_lock_debug_s *)lock);
		spin_unlock(&lock->spinlock);
	}

	/*                                                                                    
                            */
	static inline void _mali_osk_spinlock_term(_mali_osk_spinlock_t *lock)
	{
		/*                       */
		BUG_ON(NULL == lock);

		/*                                                                                   */
		kfree(lock);
	}

	/*                                                                                            
                                                                             */
	static inline _mali_osk_spinlock_irq_t *_mali_osk_spinlock_irq_init(_mali_osk_lock_flags_t flags, _mali_osk_lock_order_t order)
	{
		_mali_osk_spinlock_irq_t *lock = NULL;
		lock = kmalloc(sizeof(_mali_osk_spinlock_irq_t), GFP_KERNEL);

		if (NULL == lock) {
			return NULL;
		}

		lock->flags = 0;
		spin_lock_init(&lock->spinlock);
		
		#ifdef CONFIG_PROVE_LOCKING
         lockdep_skip_validate(&lock->spinlock.dep_map);
		#endif
				
		_mali_osk_locks_debug_init((struct _mali_osk_lock_debug_s *)lock, flags, order);
		return lock;
	}

	/*                                                     */
	static inline void _mali_osk_spinlock_irq_lock(_mali_osk_spinlock_irq_t *lock)
	{
		unsigned long tmp_flags;

		BUG_ON(NULL == lock);
		spin_lock_irqsave(&lock->spinlock, tmp_flags);
		lock->flags = tmp_flags;
		_mali_osk_locks_debug_add((struct _mali_osk_lock_debug_s *)lock);
	}

	/*                                                     */
	static inline void _mali_osk_spinlock_irq_unlock(_mali_osk_spinlock_irq_t *lock)
	{
		BUG_ON(NULL == lock);
		_mali_osk_locks_debug_remove((struct _mali_osk_lock_debug_s *)lock);
		spin_unlock_irqrestore(&lock->spinlock, lock->flags);
	}

	/*                                                                                      
                                */
	static inline void _mali_osk_spinlock_irq_term(_mali_osk_spinlock_irq_t *lock)
	{
		/*                       */
		BUG_ON(NULL == lock);

		/*                                                                                   */
		kfree(lock);
	}

	/*                                                                               
                                                                                            */
	static inline _mali_osk_mutex_rw_t *_mali_osk_mutex_rw_init(_mali_osk_lock_flags_t flags, _mali_osk_lock_order_t order)
	{
		_mali_osk_mutex_rw_t *lock = NULL;

		lock = kmalloc(sizeof(_mali_osk_mutex_rw_t), GFP_KERNEL);

		if (NULL == lock) {
			return NULL;
		}

		init_rwsem(&lock->rw_sema);
		_mali_osk_locks_debug_init((struct _mali_osk_lock_debug_s *)lock, flags, order);
		return lock;
	}

	/*                                                                                       
                                                                                 */
	static inline void _mali_osk_mutex_rw_wait(_mali_osk_mutex_rw_t *lock, _mali_osk_lock_mode_t mode)
	{
		BUG_ON(NULL == lock);
		BUG_ON(!(_MALI_OSK_LOCKMODE_RO == mode || _MALI_OSK_LOCKMODE_RW == mode));

		if (mode == _MALI_OSK_LOCKMODE_RO) {
			down_read(&lock->rw_sema);
		} else {
			down_write(&lock->rw_sema);
		}

#ifdef DEBUG
		if (mode == _MALI_OSK_LOCKMODE_RW) {
			lock->mode = mode;
		} else { /*                               */
			lock->mode = mode;
		}
		_mali_osk_locks_debug_add((struct _mali_osk_lock_debug_s *)lock);
#endif
	}

	/*                                                                                */
	static inline void  _mali_osk_mutex_rw_signal(_mali_osk_mutex_rw_t *lock, _mali_osk_lock_mode_t mode)
	{
		BUG_ON(NULL == lock);
		BUG_ON(!(_MALI_OSK_LOCKMODE_RO == mode || _MALI_OSK_LOCKMODE_RW == mode));
#ifdef DEBUG
		/*                                                                */
		if (mode == _MALI_OSK_LOCKMODE_RW) {
			_mali_osk_locks_debug_remove((struct _mali_osk_lock_debug_s *)lock);
			/*                            */
			lock->checker.owner = 0;
		}
#endif

		if (mode == _MALI_OSK_LOCKMODE_RO) {
			up_read(&lock->rw_sema);
		} else {
			up_write(&lock->rw_sema);
		}
	}

	/*                                                                             
                            */
	static inline void _mali_osk_mutex_rw_term(_mali_osk_mutex_rw_t *lock)
	{
		/*                       */
		BUG_ON(NULL == lock);

		/*                                                                                   */
		kfree(lock);
	}

	/*                                                                                                 
                                                                                              */
	static inline _mali_osk_mutex_t *_mali_osk_mutex_init(_mali_osk_lock_flags_t flags, _mali_osk_lock_order_t order)
	{
		_mali_osk_mutex_t *lock = NULL;

		lock = kmalloc(sizeof(_mali_osk_mutex_t), GFP_KERNEL);

		if (NULL == lock) {
			return NULL;
		}
		mutex_init(&lock->mutex);

		_mali_osk_locks_debug_init((struct _mali_osk_lock_debug_s *)lock, flags, order);
		return lock;
	}

	/*                                                                      */
	static inline _mali_osk_errcode_t _mali_osk_mutex_wait_interruptible(_mali_osk_mutex_t *lock)
	{
		_mali_osk_errcode_t err = _MALI_OSK_ERR_OK;

		BUG_ON(NULL == lock);

		if (mutex_lock_interruptible(&lock->mutex)) {
			printk(KERN_WARNING "Mali: Can not lock mutex\n");
			err = _MALI_OSK_ERR_RESTARTSYSCALL;
		}

		_mali_osk_locks_debug_add((struct _mali_osk_lock_debug_s *)lock);
		return err;
	}

	/*                                                                                          */
	static inline void _mali_osk_mutex_signal_interruptible(_mali_osk_mutex_t *lock)
	{
		BUG_ON(NULL == lock);
		_mali_osk_locks_debug_remove((struct _mali_osk_lock_debug_s *)lock);
		mutex_unlock(&lock->mutex);
	}

	/*                                                                                               */
	static inline void _mali_osk_mutex_wait(_mali_osk_mutex_t *lock)
	{
		BUG_ON(NULL == lock);
		mutex_lock(&lock->mutex);
		_mali_osk_locks_debug_add((struct _mali_osk_lock_debug_s *)lock);
	}

	/*                                                                            */
	static inline void _mali_osk_mutex_signal(_mali_osk_mutex_t *lock)
	{
		BUG_ON(NULL == lock);
		_mali_osk_locks_debug_remove((struct _mali_osk_lock_debug_s *)lock);
		mutex_unlock(&lock->mutex);
	}

	/*                                                     */
	static inline void _mali_osk_mutex_term(_mali_osk_mutex_t *lock)
	{
		/*                       */
		BUG_ON(NULL == lock);

		/*                                                                                   */
		kfree(lock);
	}

#ifdef _cplusplus
}
#endif

#endif
