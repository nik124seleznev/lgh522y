#ifndef __LINUX_MROUTE_H
#define __LINUX_MROUTE_H

#include <linux/in.h>
#include <linux/pim.h>
#include <net/sock.h>
#include <uapi/linux/mroute.h>

#ifdef CONFIG_IP_MROUTE
static inline int ip_mroute_opt(int opt)
{
	return (opt >= MRT_BASE) && (opt <= MRT_MAX);
}
#else
static inline int ip_mroute_opt(int opt)
{
	return 0;
}
#endif

#ifdef CONFIG_IP_MROUTE
extern int ip_mroute_setsockopt(struct sock *, int, char __user *, unsigned int);
extern int ip_mroute_getsockopt(struct sock *, int, char __user *, int __user *);
extern int ipmr_ioctl(struct sock *sk, int cmd, void __user *arg);
extern int ipmr_compat_ioctl(struct sock *sk, unsigned int cmd, void __user *arg);
extern int ip_mr_init(void);
#else
static inline
int ip_mroute_setsockopt(struct sock *sock,
			 int optname, char __user *optval, unsigned int optlen)
{
	return -ENOPROTOOPT;
}

static inline
int ip_mroute_getsockopt(struct sock *sock,
			 int optname, char __user *optval, int __user *optlen)
{
	return -ENOPROTOOPT;
}

static inline
int ipmr_ioctl(struct sock *sk, int cmd, void __user *arg)
{
	return -ENOIOCTLCMD;
}

static inline int ip_mr_init(void)
{
	return 0;
}
#endif

struct vif_device {
	struct net_device 	*dev;			/*                     */
	unsigned long	bytes_in,bytes_out;
	unsigned long	pkt_in,pkt_out;		/*               */
	unsigned long	rate_limit;		/*                       */
	unsigned char	threshold;		/*                 */
	unsigned short	flags;			/*                 */
	__be32		local,remote;		/*                              */
	int		link;			/*                          */
};

#define VIFF_STATIC 0x8000

struct mfc_cache {
	struct list_head list;
	__be32 mfc_mcastgrp;			/*                             */
	__be32 mfc_origin;			/*                    */
	vifi_t mfc_parent;			/*                   */
	int mfc_flags;				/*                */

	union {
		struct {
			unsigned long expires;
			struct sk_buff_head unresolved;	/*                     */
		} unres;
		struct {
			unsigned long last_assert;
			int minvif;
			int maxvif;
			unsigned long bytes;
			unsigned long pkt;
			unsigned long wrong_if;
			unsigned char ttls[MAXVIFS];	/*                 */
		} res;
	} mfc_un;
	struct rcu_head	rcu;
};

#define MFC_STATIC		1
#define MFC_NOTIFY		2

#define MFC_LINES		64

#ifdef __BIG_ENDIAN
#define MFC_HASH(a,b)	(((((__force u32)(__be32)a)>>24)^(((__force u32)(__be32)b)>>26))&(MFC_LINES-1))
#else
#define MFC_HASH(a,b)	((((__force u32)(__be32)a)^(((__force u32)(__be32)b)>>2))&(MFC_LINES-1))
#endif		

struct rtmsg;
extern int ipmr_get_route(struct net *net, struct sk_buff *skb,
			  __be32 saddr, __be32 daddr,
			  struct rtmsg *rtm, int nowait);
#endif
