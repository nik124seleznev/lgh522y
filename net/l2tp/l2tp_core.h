/*
 * L2TP internal definitions.
 *
 * Copyright (c) 2008,2009 Katalix Systems Ltd
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _L2TP_CORE_H_
#define _L2TP_CORE_H_

/*                          */
#define L2TP_TUNNEL_MAGIC	0x42114DDA
#define L2TP_SESSION_MAGIC	0x0C04EB7D

/*                                     */
#define L2TP_HASH_BITS	4
#define L2TP_HASH_SIZE	(1 << L2TP_HASH_BITS)

/*                                      */
#define L2TP_HASH_BITS_2	8
#define L2TP_HASH_SIZE_2	(1 << L2TP_HASH_BITS_2)

/*                                                      */
enum {
	L2TP_MSG_DEBUG		= (1 << 0),	/*                  
                      */
	L2TP_MSG_CONTROL	= (1 << 1),	/*                   
                   */
	L2TP_MSG_SEQ		= (1 << 2),	/*                  */
	L2TP_MSG_DATA		= (1 << 3),	/*              */
};

struct sk_buff;

struct l2tp_stats {
	atomic_long_t		tx_packets;
	atomic_long_t		tx_bytes;
	atomic_long_t		tx_errors;
	atomic_long_t		rx_packets;
	atomic_long_t		rx_bytes;
	atomic_long_t		rx_seq_discards;
	atomic_long_t		rx_oos_packets;
	atomic_long_t		rx_errors;
	atomic_long_t		rx_cookie_discards;
};

struct l2tp_tunnel;

/*                                                                
                                      
 */
struct l2tp_session_cfg {
	enum l2tp_pwtype	pw_type;
	unsigned int		data_seq:2;	/*                      
                                 
                 
       */
	unsigned int		recv_seq:1;	/*                            
                           */
	unsigned int		send_seq:1;	/*                           
                  */
	unsigned int		lns_mode:1;	/*                           
                               
                         */
	int			debug;		/*                         
                    */
	u16			vlan_id;	/*                      */
	u16			offset;		/*                   */
	u16			l2specific_len;	/*                         */
	u16			l2specific_type; /*                       */
	u8			cookie[8];	/*                 */
	int			cookie_len;	/*                 */
	u8			peer_cookie[8];	/*               */
	int			peer_cookie_len; /*                 */
	int			reorder_timeout; /*                           
                       */
	int			mtu;
	int			mru;
	char			*ifname;
};

struct l2tp_session {
	int			magic;		/*          
                            */

	struct l2tp_tunnel	*tunnel;	/*                       
                 */
	u32			session_id;
	u32			peer_session_id;
	u8			cookie[8];
	int			cookie_len;
	u8			peer_cookie[8];
	int			peer_cookie_len;
	u16			offset;		/*                               
                              */
	u16			l2specific_len;
	u16			l2specific_type;
	u16			hdr_len;
	u32			nr;		/*                            */
	u32			ns;		/*                         */
	struct sk_buff_head	reorder_q;	/*                       */
	struct hlist_node	hlist;		/*                */
	atomic_t		ref_count;

	char			name[32];	/*             */
	char			ifname[IFNAMSIZ];
	unsigned int		data_seq:2;	/*                      
                                 
                 
       */
	unsigned int		recv_seq:1;	/*                            
                           */
	unsigned int		send_seq:1;	/*                           
                  */
	unsigned int		lns_mode:1;	/*                           
                               
                         */
	int			debug;		/*                         
                    */
	int			reorder_timeout; /*                           
                       */
	int			reorder_skip;	/*                        */
	int			mtu;
	int			mru;
	enum l2tp_pwtype	pwtype;
	struct l2tp_stats	stats;
	struct hlist_node	global_hlist;	/*                       */

	int (*build_header)(struct l2tp_session *session, void *buf);
	void (*recv_skb)(struct l2tp_session *session, struct sk_buff *skb, int data_len);
	void (*session_close)(struct l2tp_session *session);
	void (*ref)(struct l2tp_session *session);
	void (*deref)(struct l2tp_session *session);
#if defined(CONFIG_L2TP_DEBUGFS) || defined(CONFIG_L2TP_DEBUGFS_MODULE)
	void (*show)(struct seq_file *m, void *priv);
#endif
	uint8_t			priv[0];	/*              */
};

/*                                                                   
                                                 
 */
struct l2tp_tunnel_cfg {
	int			debug;		/*                         
                    */
	enum l2tp_encap_type	encap;

	/*                                      */
	struct in_addr		local_ip;
	struct in_addr		peer_ip;
#if IS_ENABLED(CONFIG_IPV6)
	struct in6_addr		*local_ip6;
	struct in6_addr		*peer_ip6;
#endif
	u16			local_udp_port;
	u16			peer_udp_port;
	unsigned int		use_udp_checksums:1;
};

struct l2tp_tunnel {
	int			magic;		/*                             */
	struct rcu_head rcu;
	rwlock_t		hlist_lock;	/*                       */
	struct hlist_head	session_hlist[L2TP_HASH_SIZE];
						/*                         
                      */
	u32			tunnel_id;
	u32			peer_tunnel_id;
	int			version;	/*                      */

	char			name[20];	/*             */
	int			debug;		/*                         
                    */
	enum l2tp_encap_type	encap;
	struct l2tp_stats	stats;

	struct list_head	list;		/*                            */
	struct net		*l2tp_net;	/*                      */

	atomic_t		ref_count;
#ifdef CONFIG_DEBUG_FS
	void (*show)(struct seq_file *m, void *arg);
#endif
	int (*recv_payload_hook)(struct sk_buff *skb);
	void (*old_sk_destruct)(struct sock *);
	struct sock		*sock;		/*               */
	int			fd;		/*                            
                                  */
#if IS_ENABLED(CONFIG_IPV6)
	bool			v4mapped;
#endif

	struct work_struct	del_work;

	uint8_t			priv[0];	/*              */
};

struct l2tp_nl_cmd_ops {
	int (*session_create)(struct net *net, u32 tunnel_id, u32 session_id, u32 peer_session_id, struct l2tp_session_cfg *cfg);
	int (*session_delete)(struct l2tp_session *session);
};

static inline void *l2tp_tunnel_priv(struct l2tp_tunnel *tunnel)
{
	return &tunnel->priv[0];
}

static inline void *l2tp_session_priv(struct l2tp_session *session)
{
	return &session->priv[0];
}

static inline struct l2tp_tunnel *l2tp_sock_to_tunnel(struct sock *sk)
{
	struct l2tp_tunnel *tunnel;

	if (sk == NULL)
		return NULL;

	sock_hold(sk);
	tunnel = (struct l2tp_tunnel *)(sk->sk_user_data);
	if (tunnel == NULL) {
		sock_put(sk);
		goto out;
	}

	BUG_ON(tunnel->magic != L2TP_TUNNEL_MAGIC);

out:
	return tunnel;
}

extern struct sock *l2tp_tunnel_sock_lookup(struct l2tp_tunnel *tunnel);
extern void l2tp_tunnel_sock_put(struct sock *sk);
extern struct l2tp_session *l2tp_session_find(struct net *net, struct l2tp_tunnel *tunnel, u32 session_id);
extern struct l2tp_session *l2tp_session_find_nth(struct l2tp_tunnel *tunnel, int nth);
extern struct l2tp_session *l2tp_session_find_by_ifname(struct net *net, char *ifname);
extern struct l2tp_tunnel *l2tp_tunnel_find(struct net *net, u32 tunnel_id);
extern struct l2tp_tunnel *l2tp_tunnel_find_nth(struct net *net, int nth);

extern int l2tp_tunnel_create(struct net *net, int fd, int version, u32 tunnel_id, u32 peer_tunnel_id, struct l2tp_tunnel_cfg *cfg, struct l2tp_tunnel **tunnelp);
extern void l2tp_tunnel_closeall(struct l2tp_tunnel *tunnel);
extern int l2tp_tunnel_delete(struct l2tp_tunnel *tunnel);
extern struct l2tp_session *l2tp_session_create(int priv_size, struct l2tp_tunnel *tunnel, u32 session_id, u32 peer_session_id, struct l2tp_session_cfg *cfg);
extern void __l2tp_session_unhash(struct l2tp_session *session);
extern int l2tp_session_delete(struct l2tp_session *session);
extern void l2tp_session_free(struct l2tp_session *session);
extern void l2tp_recv_common(struct l2tp_session *session, struct sk_buff *skb, unsigned char *ptr, unsigned char *optr, u16 hdrflags, int length, int (*payload_hook)(struct sk_buff *skb));
extern int l2tp_session_queue_purge(struct l2tp_session *session);
extern int l2tp_udp_encap_recv(struct sock *sk, struct sk_buff *skb);

extern int l2tp_xmit_skb(struct l2tp_session *session, struct sk_buff *skb, int hdr_len);

extern int l2tp_nl_register_ops(enum l2tp_pwtype pw_type, const struct l2tp_nl_cmd_ops *ops);
extern void l2tp_nl_unregister_ops(enum l2tp_pwtype pw_type);

/*                                                                    
                
 */
static inline void l2tp_session_inc_refcount_1(struct l2tp_session *session)
{
	atomic_inc(&session->ref_count);
}

static inline void l2tp_session_dec_refcount_1(struct l2tp_session *session)
{
	if (atomic_dec_and_test(&session->ref_count))
		l2tp_session_free(session);
}

#ifdef L2TP_REFCNT_DEBUG
#define l2tp_session_inc_refcount(_s)					\
do {									\
	pr_debug("l2tp_session_inc_refcount: %s:%d %s: cnt=%d\n",	\
		 __func__, __LINE__, (_s)->name,			\
		 atomic_read(&_s->ref_count));				\
	l2tp_session_inc_refcount_1(_s);				\
} while (0)
#define l2tp_session_dec_refcount(_s)					\
do {									\
	pr_debug("l2tp_session_dec_refcount: %s:%d %s: cnt=%d\n",	\
		 __func__, __LINE__, (_s)->name,			\
		 atomic_read(&_s->ref_count));				\
	l2tp_session_dec_refcount_1(_s);				\
} while (0)
#else
#define l2tp_session_inc_refcount(s) l2tp_session_inc_refcount_1(s)
#define l2tp_session_dec_refcount(s) l2tp_session_dec_refcount_1(s)
#endif

#define l2tp_printk(ptr, type, func, fmt, ...)				\
do {									\
	if (((ptr)->debug) & (type))					\
		func(fmt, ##__VA_ARGS__);				\
} while (0)

#define l2tp_warn(ptr, type, fmt, ...)					\
	l2tp_printk(ptr, type, pr_warn, fmt, ##__VA_ARGS__)
#define l2tp_info(ptr, type, fmt, ...)					\
	l2tp_printk(ptr, type, pr_info, fmt, ##__VA_ARGS__)
#define l2tp_dbg(ptr, type, fmt, ...)					\
	l2tp_printk(ptr, type, pr_debug, fmt, ##__VA_ARGS__)

#endif /*               */
