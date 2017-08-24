#ifndef _NET_IP6_TUNNEL_H
#define _NET_IP6_TUNNEL_H

#include <linux/ipv6.h>
#include <linux/netdevice.h>
#include <linux/if_tunnel.h>
#include <linux/ip6_tunnel.h>

#define IP6TUNNEL_ERR_TIMEO (30*HZ)

/*                            */
#define IP6_TNL_F_CAP_XMIT 0x10000
/*                              */
#define IP6_TNL_F_CAP_RCV 0x20000
/*                                            */
#define IP6_TNL_F_CAP_PER_PACKET 0x40000

struct __ip6_tnl_parm {
	char name[IFNAMSIZ];	/*                       */
	int link;		/*                                    */
	__u8 proto;		/*                 */
	__u8 encap_limit;	/*                                */
	__u8 hop_limit;		/*                      */
	__be32 flowinfo;	/*                                        */
	__u32 flags;		/*              */
	struct in6_addr laddr;	/*                                */
	struct in6_addr raddr;	/*                                 */

	__be16			i_flags;
	__be16			o_flags;
	__be32			i_key;
	__be32			o_key;
};

/*             */
struct ip6_tnl {
	struct ip6_tnl __rcu *next;	/*                     */
	struct net_device *dev;	/*                                       */
	struct __ip6_tnl_parm parms;	/*                                 */
	struct flowi fl;	/*                         */
	struct dst_entry *dst_cache;    /*            */
	u32 dst_cookie;

	int err_count;
	unsigned long err_time;

	/*                               */
	__u32 i_seqno;	/*                     */
	__u32 o_seqno;	/*                       */
	int hlen;       /*                                 */
	int mlink;
};

/*                                                   */

struct ipv6_tlv_tnl_enc_lim {
	__u8 type;		/*                              */
	__u8 length;		/*                              */
	__u8 encap_limit;	/*                              */
} __packed;

struct dst_entry *ip6_tnl_dst_check(struct ip6_tnl *t);
void ip6_tnl_dst_reset(struct ip6_tnl *t);
void ip6_tnl_dst_store(struct ip6_tnl *t, struct dst_entry *dst);
int ip6_tnl_rcv_ctl(struct ip6_tnl *t, const struct in6_addr *laddr,
		const struct in6_addr *raddr);
int ip6_tnl_xmit_ctl(struct ip6_tnl *t);
__u16 ip6_tnl_parse_tlv_enc_lim(struct sk_buff *skb, __u8 *raw);
__u32 ip6_tnl_get_cap(struct ip6_tnl *t, const struct in6_addr *laddr,
			     const struct in6_addr *raddr);

static inline void ip6tunnel_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct net_device_stats *stats = &dev->stats;
	int pkt_len, err;

	nf_reset(skb);
	pkt_len = skb->len;
	err = ip6_local_out(skb);

	if (net_xmit_eval(err) == 0) {
		struct pcpu_tstats *tstats = this_cpu_ptr(dev->tstats);
		u64_stats_update_begin(&tstats->syncp);
		tstats->tx_bytes += pkt_len;
		tstats->tx_packets++;
		u64_stats_update_end(&tstats->syncp);
	} else {
		stats->tx_errors++;
		stats->tx_aborted_errors++;
	}
}
#endif
