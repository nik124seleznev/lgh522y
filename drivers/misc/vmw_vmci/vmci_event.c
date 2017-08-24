/*
 * VMware VMCI Driver
 *
 * Copyright (C) 2012 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2 and no later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include <linux/vmw_vmci_defs.h>
#include <linux/vmw_vmci_api.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include "vmci_driver.h"
#include "vmci_event.h"

#define EVENT_MAGIC 0xEABE0000
#define VMCI_EVENT_MAX_ATTEMPTS 10

struct vmci_subscription {
	u32 id;
	u32 event;
	vmci_event_cb callback;
	void *callback_data;
	struct list_head node;	/*                            */
};

static struct list_head subscriber_array[VMCI_EVENT_MAX];
static DEFINE_MUTEX(subscriber_mutex);

int __init vmci_event_init(void)
{
	int i;

	for (i = 0; i < VMCI_EVENT_MAX; i++)
		INIT_LIST_HEAD(&subscriber_array[i]);

	return VMCI_SUCCESS;
}

void vmci_event_exit(void)
{
	int e;

	/*                             */
	for (e = 0; e < VMCI_EVENT_MAX; e++) {
		struct vmci_subscription *cur, *p2;
		list_for_each_entry_safe(cur, p2, &subscriber_array[e], node) {

			/*
                                                 
                                                 
                                  
    */
			pr_warn("Unexpected free events occurring\n");
			list_del(&cur->node);
			kfree(cur);
		}
	}
}

/*
                                                
 */
static struct vmci_subscription *event_find(u32 sub_id)
{
	int e;

	for (e = 0; e < VMCI_EVENT_MAX; e++) {
		struct vmci_subscription *cur;
		list_for_each_entry(cur, &subscriber_array[e], node) {
			if (cur->id == sub_id)
				return cur;
		}
	}
	return NULL;
}

/*
                                                   
                                                        
 */
static void event_deliver(struct vmci_event_msg *event_msg)
{
	struct vmci_subscription *cur;
	struct list_head *subscriber_list;

	rcu_read_lock();
	subscriber_list = &subscriber_array[event_msg->event_data.event];
	list_for_each_entry_rcu(cur, subscriber_list, node) {
		cur->callback(cur->id, &event_msg->event_data,
			      cur->callback_data);
	}
	rcu_read_unlock();
}

/*
                                                             
                               
 */
int vmci_event_dispatch(struct vmci_datagram *msg)
{
	struct vmci_event_msg *event_msg = (struct vmci_event_msg *)msg;

	if (msg->payload_size < sizeof(u32) ||
	    msg->payload_size > sizeof(struct vmci_event_data_max))
		return VMCI_ERROR_INVALID_ARGS;

	if (!VMCI_EVENT_VALID(event_msg->event_data.event))
		return VMCI_ERROR_EVENT_UNKNOWN;

	event_deliver(event_msg);
	return VMCI_SUCCESS;
}

/*
                                                       
                                          
                                                      
                                                     
                                                                 
                                        
  
                                                                   
                                                                
 */
int vmci_event_subscribe(u32 event,
			 vmci_event_cb callback,
			 void *callback_data,
			 u32 *new_subscription_id)
{
	struct vmci_subscription *sub;
	int attempts;
	int retval;
	bool have_new_id = false;

	if (!new_subscription_id) {
		pr_devel("%s: Invalid subscription (NULL)\n", __func__);
		return VMCI_ERROR_INVALID_ARGS;
	}

	if (!VMCI_EVENT_VALID(event) || !callback) {
		pr_devel("%s: Failed to subscribe to event (type=%d) (callback=%p) (data=%p)\n",
			 __func__, event, callback, callback_data);
		return VMCI_ERROR_INVALID_ARGS;
	}

	sub = kzalloc(sizeof(*sub), GFP_KERNEL);
	if (!sub)
		return VMCI_ERROR_NO_MEM;

	sub->id = VMCI_EVENT_MAX;
	sub->event = event;
	sub->callback = callback;
	sub->callback_data = callback_data;
	INIT_LIST_HEAD(&sub->node);

	mutex_lock(&subscriber_mutex);

	/*                                            */
	for (attempts = 0; attempts < VMCI_EVENT_MAX_ATTEMPTS; attempts++) {
		static u32 subscription_id;
		/*
                                                
                                      
   */

		/*                        */
		if (!event_find(++subscription_id)) {
			sub->id = subscription_id;
			have_new_id = true;
			break;
		}
	}

	if (have_new_id) {
		list_add_rcu(&sub->node, &subscriber_array[event]);
		retval = VMCI_SUCCESS;
	} else {
		retval = VMCI_ERROR_NO_RESOURCES;
	}

	mutex_unlock(&subscriber_mutex);

	*new_subscription_id = sub->id;
	return retval;
}
EXPORT_SYMBOL_GPL(vmci_event_subscribe);

/*
                                                        
                                                                       
  
                                                                   
                                                    
 */
int vmci_event_unsubscribe(u32 sub_id)
{
	struct vmci_subscription *s;

	mutex_lock(&subscriber_mutex);
	s = event_find(sub_id);
	if (s)
		list_del_rcu(&s->node);
	mutex_unlock(&subscriber_mutex);

	if (!s)
		return VMCI_ERROR_NOT_FOUND;

	synchronize_rcu();
	kfree(s);

	return VMCI_SUCCESS;
}
EXPORT_SYMBOL_GPL(vmci_event_unsubscribe);
