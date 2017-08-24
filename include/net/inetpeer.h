/*
                                                              
  
                                             
 */

#ifndef _NET_INETPEER_H
#define _NET_INETPEER_H

#include <linux/types.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/spinlock.h>
#include <linux/rtnetlink.h>
#include <net/ipv6.h>
#include <linux/atomic.h>

struct inetpeer_addr_base {
	union {
		__be32			a4;
		__be32			a6[4];
	};
};

struct inetpeer_addr {
	struct inetpeer_addr_base	addr;
	__u16				family;
};

struct inet_peer {
	/*                                                              */
	struct inet_peer __rcu	*avl_left, *avl_right;
	struct inetpeer_addr	daddr;
	__u32			avl_height;

	u32			metrics[RTAX_MAX];
	u32			rate_tokens;	/*                        */
	unsigned long		rate_last;
	union {
		struct list_head	gc_list;
		struct rcu_head     gc_rcu;
	};
	/*
                                                                         
                         
                                                                   
  */
	union {
		struct {
			atomic_t			rid;		/*                        */
		};
		struct rcu_head         rcu;
		struct inet_peer	*gc_next;
	};

	/*                                              */
	__u32			dtime;	/*                                                */
	atomic_t		refcnt;
};

struct inet_peer_base {
	struct inet_peer __rcu	*root;
	seqlock_t		lock;
	u32			flush_seq;
	int			total;
};

#define INETPEER_BASE_BIT	0x1UL

static inline struct inet_peer *inetpeer_ptr(unsigned long val)
{
	BUG_ON(val & INETPEER_BASE_BIT);
	return (struct inet_peer *) val;
}

static inline struct inet_peer_base *inetpeer_base_ptr(unsigned long val)
{
	if (!(val & INETPEER_BASE_BIT))
		return NULL;
	val &= ~INETPEER_BASE_BIT;
	return (struct inet_peer_base *) val;
}

static inline bool inetpeer_ptr_is_peer(unsigned long val)
{
	return !(val & INETPEER_BASE_BIT);
}

static inline void __inetpeer_ptr_set_peer(unsigned long *val, struct inet_peer *peer)
{
	/*                                          */
	*val = (unsigned long) peer;
}

static inline bool inetpeer_ptr_set_peer(unsigned long *ptr, struct inet_peer *peer)
{
	unsigned long val = (unsigned long) peer;
	unsigned long orig = *ptr;

	if (!(orig & INETPEER_BASE_BIT) ||
	    cmpxchg(ptr, orig, val) != orig)
		return false;
	return true;
}

static inline void inetpeer_init_ptr(unsigned long *ptr, struct inet_peer_base *base)
{
	*ptr = (unsigned long) base | INETPEER_BASE_BIT;
}

static inline void inetpeer_transfer_peer(unsigned long *to, unsigned long *from)
{
	unsigned long val = *from;

	*to = val;
	if (inetpeer_ptr_is_peer(val)) {
		struct inet_peer *peer = inetpeer_ptr(val);
		atomic_inc(&peer->refcnt);
	}
}

extern void inet_peer_base_init(struct inet_peer_base *);

void			inet_initpeers(void) __init;

#define INETPEER_METRICS_NEW	(~(u32) 0)

static inline bool inet_metrics_new(const struct inet_peer *p)
{
	return p->metrics[RTAX_LOCK-1] == INETPEER_METRICS_NEW;
}

/*                                                       */
struct inet_peer *inet_getpeer(struct inet_peer_base *base,
			       const struct inetpeer_addr *daddr,
			       int create);

static inline struct inet_peer *inet_getpeer_v4(struct inet_peer_base *base,
						__be32 v4daddr,
						int create)
{
	struct inetpeer_addr daddr;

	daddr.addr.a4 = v4daddr;
	daddr.family = AF_INET;
	return inet_getpeer(base, &daddr, create);
}

static inline struct inet_peer *inet_getpeer_v6(struct inet_peer_base *base,
						const struct in6_addr *v6daddr,
						int create)
{
	struct inetpeer_addr daddr;

	*(struct in6_addr *)daddr.addr.a6 = *v6daddr;
	daddr.family = AF_INET6;
	return inet_getpeer(base, &daddr, create);
}

/*                                          */
extern void inet_putpeer(struct inet_peer *p);
extern bool inet_peer_xrlim_allow(struct inet_peer *peer, int timeout);

extern void inetpeer_invalidate_tree(struct inet_peer_base *);
extern void inetpeer_invalidate_family(int family);

/*
                                                           
                                                    
 */
static inline void inet_peer_refcheck(const struct inet_peer *p)
{
	WARN_ON_ONCE(atomic_read(&p->refcnt) <= 0);
}

#endif /*                 */