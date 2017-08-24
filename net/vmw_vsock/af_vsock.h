/*
 * VMware vSockets Driver
 *
 * Copyright (C) 2007-2013 VMware, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation version 2 and no later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef __AF_VSOCK_H__
#define __AF_VSOCK_H__

#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/vm_sockets.h>

#include "vsock_addr.h"

#define LAST_RESERVED_PORT 1023

#define vsock_sk(__sk)    ((struct vsock_sock *)__sk)
#define sk_vsock(__vsk)   (&(__vsk)->sk)

struct vsock_sock {
	/*                              */
	struct sock sk;
	struct sockaddr_vm local_addr;
	struct sockaddr_vm remote_addr;
	/*                                                             */
	struct list_head bound_table;
	struct list_head connected_table;
	/*                                                                  
                                                   
  */
	bool trusted;
	bool cached_peer_allow_dgram;	/*                               
                    
      */
	u32 cached_peer;  /*                                             */
	const struct cred *owner;
	/*                            */
	long connect_timeout;
	/*                                       */
	struct sock *listener;
	/*                                                                    
                                                                     
                                                                     
                                                                       
                                                                       
                                                                      
                           
  */
	struct list_head pending_links;
	struct list_head accept_queue;
	bool rejected;
	struct delayed_work dwork;
	u32 peer_shutdown;
	bool sent_request;
	bool ignore_connecting_rst;

	/*                       */
	void *trans;
};

s64 vsock_stream_has_data(struct vsock_sock *vsk);
s64 vsock_stream_has_space(struct vsock_sock *vsk);
void vsock_pending_work(struct work_struct *work);
struct sock *__vsock_create(struct net *net,
			    struct socket *sock,
			    struct sock *parent,
			    gfp_t priority, unsigned short type);

/*                 */

struct vsock_transport_recv_notify_data {
	u64 data1; /*                    */
	u64 data2; /*                    */
	bool notify_on_block;
};

struct vsock_transport_send_notify_data {
	u64 data1; /*                    */
	u64 data2; /*                    */
};

struct vsock_transport {
	/*                              */
	int (*init)(struct vsock_sock *, struct vsock_sock *);
	void (*destruct)(struct vsock_sock *);
	void (*release)(struct vsock_sock *);

	/*              */
	int (*connect)(struct vsock_sock *);

	/*        */
	int (*dgram_bind)(struct vsock_sock *, struct sockaddr_vm *);
	int (*dgram_dequeue)(struct kiocb *kiocb, struct vsock_sock *vsk,
			     struct msghdr *msg, size_t len, int flags);
	int (*dgram_enqueue)(struct vsock_sock *, struct sockaddr_vm *,
			     struct iovec *, size_t len);
	bool (*dgram_allow)(u32 cid, u32 port);

	/*         */
	/*                     */
	ssize_t (*stream_dequeue)(struct vsock_sock *, struct iovec *,
				  size_t len, int flags);
	ssize_t (*stream_enqueue)(struct vsock_sock *, struct iovec *,
				  size_t len);
	s64 (*stream_has_data)(struct vsock_sock *);
	s64 (*stream_has_space)(struct vsock_sock *);
	u64 (*stream_rcvhiwat)(struct vsock_sock *);
	bool (*stream_is_active)(struct vsock_sock *);
	bool (*stream_allow)(u32 cid, u32 port);

	/*               */
	int (*notify_poll_in)(struct vsock_sock *, size_t, bool *);
	int (*notify_poll_out)(struct vsock_sock *, size_t, bool *);
	int (*notify_recv_init)(struct vsock_sock *, size_t,
		struct vsock_transport_recv_notify_data *);
	int (*notify_recv_pre_block)(struct vsock_sock *, size_t,
		struct vsock_transport_recv_notify_data *);
	int (*notify_recv_pre_dequeue)(struct vsock_sock *, size_t,
		struct vsock_transport_recv_notify_data *);
	int (*notify_recv_post_dequeue)(struct vsock_sock *, size_t,
		ssize_t, bool, struct vsock_transport_recv_notify_data *);
	int (*notify_send_init)(struct vsock_sock *,
		struct vsock_transport_send_notify_data *);
	int (*notify_send_pre_block)(struct vsock_sock *,
		struct vsock_transport_send_notify_data *);
	int (*notify_send_pre_enqueue)(struct vsock_sock *,
		struct vsock_transport_send_notify_data *);
	int (*notify_send_post_enqueue)(struct vsock_sock *, ssize_t,
		struct vsock_transport_send_notify_data *);

	/*           */
	int (*shutdown)(struct vsock_sock *, int);

	/*               */
	void (*set_buffer_size)(struct vsock_sock *, u64);
	void (*set_min_buffer_size)(struct vsock_sock *, u64);
	void (*set_max_buffer_size)(struct vsock_sock *, u64);
	u64 (*get_buffer_size)(struct vsock_sock *);
	u64 (*get_min_buffer_size)(struct vsock_sock *);
	u64 (*get_max_buffer_size)(struct vsock_sock *);

	/*             */
	u32 (*get_local_cid)(void);
};

/*            */

int vsock_core_init(const struct vsock_transport *t);
void vsock_core_exit(void);

/*             */

void vsock_release_pending(struct sock *pending);
void vsock_add_pending(struct sock *listener, struct sock *pending);
void vsock_remove_pending(struct sock *listener, struct sock *pending);
void vsock_enqueue_accept(struct sock *listener, struct sock *connected);
void vsock_insert_connected(struct vsock_sock *vsk);
void vsock_remove_bound(struct vsock_sock *vsk);
void vsock_remove_connected(struct vsock_sock *vsk);
struct sock *vsock_find_bound_socket(struct sockaddr_vm *addr);
struct sock *vsock_find_connected_socket(struct sockaddr_vm *src,
					 struct sockaddr_vm *dst);
void vsock_for_each_connected_socket(void (*fn)(struct sock *sk));

#endif /*                */
