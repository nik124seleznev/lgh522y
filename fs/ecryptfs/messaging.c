/**
 * eCryptfs: Linux filesystem encryption layer
 *
 * Copyright (C) 2004-2008 International Business Machines Corp.
 *   Author(s): Michael A. Halcrow <mhalcrow@us.ibm.com>
 *		Tyler Hicks <tyhicks@ou.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/user_namespace.h>
#include <linux/nsproxy.h>
#include "ecryptfs_kernel.h"

static LIST_HEAD(ecryptfs_msg_ctx_free_list);
static LIST_HEAD(ecryptfs_msg_ctx_alloc_list);
static struct mutex ecryptfs_msg_ctx_lists_mux;

static struct hlist_head *ecryptfs_daemon_hash;
struct mutex ecryptfs_daemon_hash_mux;
static int ecryptfs_hash_bits;
#define ecryptfs_current_euid_hash(uid) \
	hash_long((unsigned long)from_kuid(&init_user_ns, current_euid()), ecryptfs_hash_bits)

static u32 ecryptfs_msg_counter;
static struct ecryptfs_msg_ctx *ecryptfs_msg_ctx_arr;

/* 
                                
                                                             
  
                                                                    
                                                                      
                                                               
                                                                   
        
 */
static int ecryptfs_acquire_free_msg_ctx(struct ecryptfs_msg_ctx **msg_ctx)
{
	struct list_head *p;
	int rc;

	if (list_empty(&ecryptfs_msg_ctx_free_list)) {
		printk(KERN_WARNING "%s: The eCryptfs free "
		       "context list is empty.  It may be helpful to "
		       "specify the ecryptfs_message_buf_len "
		       "parameter to be greater than the current "
		       "value of [%d]\n", __func__, ecryptfs_message_buf_len);
		rc = -ENOMEM;
		goto out;
	}
	list_for_each(p, &ecryptfs_msg_ctx_free_list) {
		*msg_ctx = list_entry(p, struct ecryptfs_msg_ctx, node);
		if (mutex_trylock(&(*msg_ctx)->mux)) {
			(*msg_ctx)->task = current;
			rc = 0;
			goto out;
		}
	}
	rc = -ENOMEM;
out:
	return rc;
}

/* 
                                 
                                                                     
  
                                                       
 */
static void ecryptfs_msg_ctx_free_to_alloc(struct ecryptfs_msg_ctx *msg_ctx)
{
	list_move(&msg_ctx->node, &ecryptfs_msg_ctx_alloc_list);
	msg_ctx->state = ECRYPTFS_MSG_CTX_STATE_PENDING;
	msg_ctx->counter = ++ecryptfs_msg_counter;
}

/* 
                                 
                                                                     
  
                                                       
 */
void ecryptfs_msg_ctx_alloc_to_free(struct ecryptfs_msg_ctx *msg_ctx)
{
	list_move(&(msg_ctx->node), &ecryptfs_msg_ctx_free_list);
	kfree(msg_ctx->msg);
	msg_ctx->msg = NULL;
	msg_ctx->state = ECRYPTFS_MSG_CTX_STATE_FREE;
}

/* 
                               
                                                                         
  
                                                     
  
                                                          
  
                                                                      
 */
int ecryptfs_find_daemon_by_euid(struct ecryptfs_daemon **daemon)
{
	int rc;

	hlist_for_each_entry(*daemon,
			    &ecryptfs_daemon_hash[ecryptfs_current_euid_hash()],
			    euid_chain) {
		if (uid_eq((*daemon)->file->f_cred->euid, current_euid())) {
			rc = 0;
			goto out;
		}
	}
	rc = -EINVAL;
out:
	return rc;
}

/* 
                                                                    
                                                           
                                              
  
                                                      
                                  
  
                                              
 */
int
ecryptfs_spawn_daemon(struct ecryptfs_daemon **daemon, struct file *file)
{
	int rc = 0;

	(*daemon) = kzalloc(sizeof(**daemon), GFP_KERNEL);
	if (!(*daemon)) {
		rc = -ENOMEM;
		printk(KERN_ERR "%s: Failed to allocate [%zd] bytes of "
		       "GFP_KERNEL memory\n", __func__, sizeof(**daemon));
		goto out;
	}
	(*daemon)->file = file;
	mutex_init(&(*daemon)->mux);
	INIT_LIST_HEAD(&(*daemon)->msg_ctx_out_queue);
	init_waitqueue_head(&(*daemon)->wait);
	(*daemon)->num_queued_msg_ctx = 0;
	hlist_add_head(&(*daemon)->euid_chain,
		       &ecryptfs_daemon_hash[ecryptfs_current_euid_hash()]);
out:
	return rc;
}

/* 
                                                       
  
                                                      
                                                     
 */
int ecryptfs_exorcise_daemon(struct ecryptfs_daemon *daemon)
{
	struct ecryptfs_msg_ctx *msg_ctx, *msg_ctx_tmp;
	int rc = 0;

	mutex_lock(&daemon->mux);
	if ((daemon->flags & ECRYPTFS_DAEMON_IN_READ)
	    || (daemon->flags & ECRYPTFS_DAEMON_IN_POLL)) {
		rc = -EBUSY;
		mutex_unlock(&daemon->mux);
		goto out;
	}
	list_for_each_entry_safe(msg_ctx, msg_ctx_tmp,
				 &daemon->msg_ctx_out_queue, daemon_out_list) {
		list_del(&msg_ctx->daemon_out_list);
		daemon->num_queued_msg_ctx--;
		printk(KERN_WARNING "%s: Warning: dropping message that is in "
		       "the out queue of a dying daemon\n", __func__);
		ecryptfs_msg_ctx_alloc_to_free(msg_ctx);
	}
	hlist_del(&daemon->euid_chain);
	mutex_unlock(&daemon->mux);
	kzfree(daemon);
out:
	return rc;
}

/* 
                           
                                                                      
                                          
                                                                    
                                                                 
                 
  
                                                                     
                                                                  
                                                                      
                                                                   
                                                                    
                                                             
                                                                    
                                                               
                                                                     
                                                                   
                    
  
                                              
 */
int ecryptfs_process_response(struct ecryptfs_daemon *daemon,
			      struct ecryptfs_message *msg, u32 seq)
{
	struct ecryptfs_msg_ctx *msg_ctx;
	size_t msg_size;
	int rc;

	if (msg->index >= ecryptfs_message_buf_len) {
		rc = -EINVAL;
		printk(KERN_ERR "%s: Attempt to reference "
		       "context buffer at index [%d]; maximum "
		       "allowable is [%d]\n", __func__, msg->index,
		       (ecryptfs_message_buf_len - 1));
		goto out;
	}
	msg_ctx = &ecryptfs_msg_ctx_arr[msg->index];
	mutex_lock(&msg_ctx->mux);
	if (msg_ctx->state != ECRYPTFS_MSG_CTX_STATE_PENDING) {
		rc = -EINVAL;
		printk(KERN_WARNING "%s: Desired context element is not "
		       "pending a response\n", __func__);
		goto unlock;
	} else if (msg_ctx->counter != seq) {
		rc = -EINVAL;
		printk(KERN_WARNING "%s: Invalid message sequence; "
		       "expected [%d]; received [%d]\n", __func__,
		       msg_ctx->counter, seq);
		goto unlock;
	}
	msg_size = (sizeof(*msg) + msg->data_len);
	msg_ctx->msg = kmalloc(msg_size, GFP_KERNEL);
	if (!msg_ctx->msg) {
		rc = -ENOMEM;
		printk(KERN_ERR "%s: Failed to allocate [%zd] bytes of "
		       "GFP_KERNEL memory\n", __func__, msg_size);
		goto unlock;
	}
	memcpy(msg_ctx->msg, msg, msg_size);
	msg_ctx->state = ECRYPTFS_MSG_CTX_STATE_DONE;
	wake_up_process(msg_ctx->task);
	rc = 0;
unlock:
	mutex_unlock(&msg_ctx->mux);
out:
	return rc;
}

/* 
                               
                          
                                
                                                       
  
                                                     
  
                                              
 */
static int
ecryptfs_send_message_locked(char *data, int data_len, u8 msg_type,
			     struct ecryptfs_msg_ctx **msg_ctx)
{
	struct ecryptfs_daemon *daemon;
	int rc;

	rc = ecryptfs_find_daemon_by_euid(&daemon);
	if (rc) {
		rc = -ENOTCONN;
		goto out;
	}
	mutex_lock(&ecryptfs_msg_ctx_lists_mux);
	rc = ecryptfs_acquire_free_msg_ctx(msg_ctx);
	if (rc) {
		mutex_unlock(&ecryptfs_msg_ctx_lists_mux);
		printk(KERN_WARNING "%s: Could not claim a free "
		       "context element\n", __func__);
		goto out;
	}
	ecryptfs_msg_ctx_free_to_alloc(*msg_ctx);
	mutex_unlock(&(*msg_ctx)->mux);
	mutex_unlock(&ecryptfs_msg_ctx_lists_mux);
	rc = ecryptfs_send_miscdev(data, data_len, *msg_ctx, msg_type, 0,
				   daemon);
	if (rc)
		printk(KERN_ERR "%s: Error attempting to send message to "
		       "userspace daemon; rc = [%d]\n", __func__, rc);
out:
	return rc;
}

/* 
                        
                          
                                
                                                       
  
                                  
  
                                              
 */
int ecryptfs_send_message(char *data, int data_len,
			  struct ecryptfs_msg_ctx **msg_ctx)
{
	int rc;

	mutex_lock(&ecryptfs_daemon_hash_mux);
	rc = ecryptfs_send_message_locked(data, data_len, ECRYPTFS_MSG_REQUEST,
					  msg_ctx);
	mutex_unlock(&ecryptfs_daemon_hash_mux);
	return rc;
}

/* 
                             
                                                                 
                                                                
  
                                                                      
                                                             
                                                                
                                                                     
                                                  
 */
int ecryptfs_wait_for_response(struct ecryptfs_msg_ctx *msg_ctx,
			       struct ecryptfs_message **msg)
{
	signed long timeout = ecryptfs_message_wait_timeout * HZ;
	int rc = 0;

sleep:
	timeout = schedule_timeout_interruptible(timeout);
	mutex_lock(&ecryptfs_msg_ctx_lists_mux);
	mutex_lock(&msg_ctx->mux);
	if (msg_ctx->state != ECRYPTFS_MSG_CTX_STATE_DONE) {
		if (timeout) {
			mutex_unlock(&msg_ctx->mux);
			mutex_unlock(&ecryptfs_msg_ctx_lists_mux);
			goto sleep;
		}
		rc = -ENOMSG;
	} else {
		*msg = msg_ctx->msg;
		msg_ctx->msg = NULL;
	}
	ecryptfs_msg_ctx_alloc_to_free(msg_ctx);
	mutex_unlock(&msg_ctx->mux);
	mutex_unlock(&ecryptfs_msg_ctx_lists_mux);
	return rc;
}

int __init ecryptfs_init_messaging(void)
{
	int i;
	int rc = 0;

	if (ecryptfs_number_of_users > ECRYPTFS_MAX_NUM_USERS) {
		ecryptfs_number_of_users = ECRYPTFS_MAX_NUM_USERS;
		printk(KERN_WARNING "%s: Specified number of users is "
		       "too large, defaulting to [%d] users\n", __func__,
		       ecryptfs_number_of_users);
	}
	mutex_init(&ecryptfs_daemon_hash_mux);
	mutex_lock(&ecryptfs_daemon_hash_mux);
	ecryptfs_hash_bits = 1;
	while (ecryptfs_number_of_users >> ecryptfs_hash_bits)
		ecryptfs_hash_bits++;
	ecryptfs_daemon_hash = kmalloc((sizeof(struct hlist_head)
					* (1 << ecryptfs_hash_bits)),
				       GFP_KERNEL);
	if (!ecryptfs_daemon_hash) {
		rc = -ENOMEM;
		printk(KERN_ERR "%s: Failed to allocate memory\n", __func__);
		mutex_unlock(&ecryptfs_daemon_hash_mux);
		goto out;
	}
	for (i = 0; i < (1 << ecryptfs_hash_bits); i++)
		INIT_HLIST_HEAD(&ecryptfs_daemon_hash[i]);
	mutex_unlock(&ecryptfs_daemon_hash_mux);
	ecryptfs_msg_ctx_arr = kmalloc((sizeof(struct ecryptfs_msg_ctx)
					* ecryptfs_message_buf_len),
				       GFP_KERNEL);
	if (!ecryptfs_msg_ctx_arr) {
		rc = -ENOMEM;
		printk(KERN_ERR "%s: Failed to allocate memory\n", __func__);
		goto out;
	}
	mutex_init(&ecryptfs_msg_ctx_lists_mux);
	mutex_lock(&ecryptfs_msg_ctx_lists_mux);
	ecryptfs_msg_counter = 0;
	for (i = 0; i < ecryptfs_message_buf_len; i++) {
		INIT_LIST_HEAD(&ecryptfs_msg_ctx_arr[i].node);
		INIT_LIST_HEAD(&ecryptfs_msg_ctx_arr[i].daemon_out_list);
		mutex_init(&ecryptfs_msg_ctx_arr[i].mux);
		mutex_lock(&ecryptfs_msg_ctx_arr[i].mux);
		ecryptfs_msg_ctx_arr[i].index = i;
		ecryptfs_msg_ctx_arr[i].state = ECRYPTFS_MSG_CTX_STATE_FREE;
		ecryptfs_msg_ctx_arr[i].counter = 0;
		ecryptfs_msg_ctx_arr[i].task = NULL;
		ecryptfs_msg_ctx_arr[i].msg = NULL;
		list_add_tail(&ecryptfs_msg_ctx_arr[i].node,
			      &ecryptfs_msg_ctx_free_list);
		mutex_unlock(&ecryptfs_msg_ctx_arr[i].mux);
	}
	mutex_unlock(&ecryptfs_msg_ctx_lists_mux);
	rc = ecryptfs_init_ecryptfs_miscdev();
	if (rc)
		ecryptfs_release_messaging();
out:
	return rc;
}

void ecryptfs_release_messaging(void)
{
	if (ecryptfs_msg_ctx_arr) {
		int i;

		mutex_lock(&ecryptfs_msg_ctx_lists_mux);
		for (i = 0; i < ecryptfs_message_buf_len; i++) {
			mutex_lock(&ecryptfs_msg_ctx_arr[i].mux);
			if (ecryptfs_msg_ctx_arr[i].msg)
				kfree(ecryptfs_msg_ctx_arr[i].msg);
			mutex_unlock(&ecryptfs_msg_ctx_arr[i].mux);
		}
		kfree(ecryptfs_msg_ctx_arr);
		mutex_unlock(&ecryptfs_msg_ctx_lists_mux);
	}
	if (ecryptfs_daemon_hash) {
		struct ecryptfs_daemon *daemon;
		int i;

		mutex_lock(&ecryptfs_daemon_hash_mux);
		for (i = 0; i < (1 << ecryptfs_hash_bits); i++) {
			int rc;

			hlist_for_each_entry(daemon,
					     &ecryptfs_daemon_hash[i],
					     euid_chain) {
				rc = ecryptfs_exorcise_daemon(daemon);
				if (rc)
					printk(KERN_ERR "%s: Error whilst "
					       "attempting to destroy daemon; "
					       "rc = [%d]. Dazed and confused, "
					       "but trying to continue.\n",
					       __func__, rc);
			}
		}
		kfree(ecryptfs_daemon_hash);
		mutex_unlock(&ecryptfs_daemon_hash_mux);
	}
	ecryptfs_destroy_ecryptfs_miscdev();
	return;
}
