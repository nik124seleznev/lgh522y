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

#include "mali_osk.h"
#include "mali_kernel_common.h"

#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

/* 
                                                    
                                                                         
                                                                          
                                                                
 */
struct _mali_osk_notification_queue_t_struct {
	spinlock_t mutex; /*                             */
	wait_queue_head_t receive_queue; /*                                                */
	struct list_head head; /*                                                 */
};

typedef struct _mali_osk_notification_wrapper_t_struct {
	struct list_head list;           /*                                 */
	_mali_osk_notification_t data;   /*                     */
} _mali_osk_notification_wrapper_t;

_mali_osk_notification_queue_t *_mali_osk_notification_queue_init( void )
{
	_mali_osk_notification_queue_t *	result;

	result = (_mali_osk_notification_queue_t *)kmalloc(sizeof(_mali_osk_notification_queue_t), GFP_KERNEL);
	if (NULL == result) return NULL;

	spin_lock_init(&result->mutex);
	init_waitqueue_head(&result->receive_queue);
	INIT_LIST_HEAD(&result->head);

	return result;
}

_mali_osk_notification_t *_mali_osk_notification_create( u32 type, u32 size )
{
	/*                                       */
	_mali_osk_notification_wrapper_t *notification;

	notification = (_mali_osk_notification_wrapper_t *)kmalloc( sizeof(_mali_osk_notification_wrapper_t) + size,
	               GFP_KERNEL | __GFP_HIGH | __GFP_REPEAT);
	if (NULL == notification) {
		MALI_DEBUG_PRINT(1, ("Failed to create a notification object\n"));
		return NULL;
	}

	/*               */
	INIT_LIST_HEAD(&notification->list);

	if (0 != size) {
		notification->data.result_buffer = ((u8*)notification) + sizeof(_mali_osk_notification_wrapper_t);
	} else {
		notification->data.result_buffer = NULL;
	}

	/*                                  */
	notification->data.notification_type = type;
	notification->data.result_buffer_size = size;

	/*        */
	return &(notification->data);
}

void _mali_osk_notification_delete( _mali_osk_notification_t *object )
{
	_mali_osk_notification_wrapper_t *notification;
	MALI_DEBUG_ASSERT_POINTER( object );

	notification = container_of( object, _mali_osk_notification_wrapper_t, data );

	/*                    */
	kfree(notification);
}

void _mali_osk_notification_queue_term( _mali_osk_notification_queue_t *queue )
{
	_mali_osk_notification_t *result;
	MALI_DEBUG_ASSERT_POINTER( queue );

	while (_MALI_OSK_ERR_OK == _mali_osk_notification_queue_dequeue(queue, &result)) {
		_mali_osk_notification_delete( result );
	}

	/*                                      */
	kfree(queue);
}
void _mali_osk_notification_queue_send( _mali_osk_notification_queue_t *queue, _mali_osk_notification_t *object )
{
#if defined(MALI_UPPER_HALF_SCHEDULING)
	unsigned long irq_flags;
#endif

	_mali_osk_notification_wrapper_t *notification;
	MALI_DEBUG_ASSERT_POINTER( queue );
	MALI_DEBUG_ASSERT_POINTER( object );

	notification = container_of( object, _mali_osk_notification_wrapper_t, data );

#if defined(MALI_UPPER_HALF_SCHEDULING)
	spin_lock_irqsave(&queue->mutex, irq_flags);
#else
	spin_lock(&queue->mutex);
#endif

	list_add_tail(&notification->list, &queue->head);

#if defined(MALI_UPPER_HALF_SCHEDULING)
	spin_unlock_irqrestore(&queue->mutex, irq_flags);
#else
	spin_unlock(&queue->mutex);
#endif

	/*                                           */
	wake_up(&queue->receive_queue);
}

_mali_osk_errcode_t _mali_osk_notification_queue_dequeue( _mali_osk_notification_queue_t *queue, _mali_osk_notification_t **result )
{
#if defined(MALI_UPPER_HALF_SCHEDULING)
	unsigned long irq_flags;
#endif

	_mali_osk_errcode_t ret = _MALI_OSK_ERR_ITEM_NOT_FOUND;
	_mali_osk_notification_wrapper_t *wrapper_object;

#if defined(MALI_UPPER_HALF_SCHEDULING)
	spin_lock_irqsave(&queue->mutex, irq_flags);
#else
	spin_lock(&queue->mutex);
#endif

	if (!list_empty(&queue->head)) {
		wrapper_object = list_entry(queue->head.next, _mali_osk_notification_wrapper_t, list);
		*result = &(wrapper_object->data);
		list_del_init(&wrapper_object->list);
		ret = _MALI_OSK_ERR_OK;
	}

#if defined(MALI_UPPER_HALF_SCHEDULING)
	spin_unlock_irqrestore(&queue->mutex, irq_flags);
#else
	spin_unlock(&queue->mutex);
#endif

	return ret;
}

_mali_osk_errcode_t _mali_osk_notification_queue_receive( _mali_osk_notification_queue_t *queue, _mali_osk_notification_t **result )
{
	/*             */
	MALI_DEBUG_ASSERT_POINTER( queue );
	MALI_DEBUG_ASSERT_POINTER( result );

	/*                */
	*result = NULL;

	if (wait_event_interruptible(queue->receive_queue,
	                             _MALI_OSK_ERR_OK == _mali_osk_notification_queue_dequeue(queue, result))) {
		return _MALI_OSK_ERR_RESTARTSYSCALL;
	}

	return _MALI_OSK_ERR_OK; /*        */
}
