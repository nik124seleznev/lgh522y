#ifndef __LINUX_RTNETLINK_H
#define __LINUX_RTNETLINK_H


#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <uapi/linux/rtnetlink.h>

extern int rtnetlink_send(struct sk_buff *skb, struct net *net, u32 pid, u32 group, int echo);
extern int rtnl_unicast(struct sk_buff *skb, struct net *net, u32 pid);
extern void rtnl_notify(struct sk_buff *skb, struct net *net, u32 pid,
			u32 group, struct nlmsghdr *nlh, gfp_t flags);
extern void rtnl_set_sk_err(struct net *net, u32 group, int error);
extern int rtnetlink_put_metrics(struct sk_buff *skb, u32 *metrics);
extern int rtnl_put_cacheinfo(struct sk_buff *skb, struct dst_entry *dst,
			      u32 id, long expires, u32 error);

extern void rtmsg_ifinfo(int type, struct net_device *dev, unsigned change);

/*                                                                         */
extern void rtnl_lock(void);
extern void rtnl_unlock(void);
extern int rtnl_trylock(void);
extern int rtnl_is_locked(void);
#ifdef CONFIG_PROVE_LOCKING
extern int lockdep_rtnl_is_held(void);
#endif /*                             */

/* 
                                                             
                                                  
  
                                                                          
                                                                        
 */
#define rcu_dereference_rtnl(p)					\
	rcu_dereference_check(p, lockdep_rtnl_is_held())

/* 
                                                                          
                                                  
  
                                                                    
                                                                     
                     
 */
#define rtnl_dereference(p)					\
	rcu_dereference_protected(p, lockdep_rtnl_is_held())

static inline struct netdev_queue *dev_ingress_queue(struct net_device *dev)
{
	return rtnl_dereference(dev->ingress_queue);
}

extern struct netdev_queue *dev_ingress_queue_create(struct net_device *dev);

extern void rtnetlink_init(void);
extern void __rtnl_unlock(void);

#define ASSERT_RTNL() do { \
	if (unlikely(!rtnl_is_locked())) { \
		printk(KERN_ERR "RTNL: assertion failed at %s (%d)\n", \
		       __FILE__,  __LINE__); \
		dump_stack(); \
	} \
} while(0)

extern int ndo_dflt_fdb_dump(struct sk_buff *skb,
			     struct netlink_callback *cb,
			     struct net_device *dev,
			     int idx);
extern int ndo_dflt_fdb_add(struct ndmsg *ndm,
			    struct nlattr *tb[],
			    struct net_device *dev,
			    const unsigned char *addr,
			     u16 flags);
extern int ndo_dflt_fdb_del(struct ndmsg *ndm,
			    struct nlattr *tb[],
			    struct net_device *dev,
			    const unsigned char *addr);

extern int ndo_dflt_bridge_getlink(struct sk_buff *skb, u32 pid, u32 seq,
				   struct net_device *dev, u16 mode);
#endif	/*                     */
