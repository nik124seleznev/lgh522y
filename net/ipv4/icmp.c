/*
 *	NET3:	Implementation of the ICMP protocol layer.
 *
 *		Alan Cox, <alan@lxorguk.ukuu.org.uk>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 *	Some of the function names and the icmp unreach table for this
 *	module were derived from [icmp.c 1.0.11 06/02/93] by
 *	Ross Biro, Fred N. van Kempen, Mark Evans, Alan Cox, Gerhard Koerting.
 *	Other than that this module is a complete rewrite.
 *
 *	Fixes:
 *	Clemens Fruhwirth	:	introduce global icmp rate limiting
 *					with icmp type masking ability instead
 *					of broken per type icmp timeouts.
 *		Mike Shaver	:	RFC1122 checks.
 *		Alan Cox	:	Multicast ping reply as self.
 *		Alan Cox	:	Fix atomicity lockup in ip_build_xmit
 *					call.
 *		Alan Cox	:	Added 216,128 byte paths to the MTU
 *					code.
 *		Martin Mares	:	RFC1812 checks.
 *		Martin Mares	:	Can be configured to follow redirects
 *					if acting as a router _without_ a
 *					routing protocol (RFC 1812).
 *		Martin Mares	:	Echo requests may be configured to
 *					be ignored (RFC 1812).
 *		Martin Mares	:	Limitation of ICMP error message
 *					transmit rate (RFC 1812).
 *		Martin Mares	:	TOS and Precedence set correctly
 *					(RFC 1812).
 *		Martin Mares	:	Now copying as much data from the
 *					original packet as we can without
 *					exceeding 576 bytes (RFC 1812).
 *	Willy Konynenberg	:	Transparent proxying support.
 *		Keith Owens	:	RFC1191 correction for 4.2BSD based
 *					path MTU bug.
 *		Thomas Quinot	:	ICMP Dest Unreach codes up to 15 are
 *					valid (RFC 1812).
 *		Andi Kleen	:	Check all packet lengths properly
 *					and moved all kfree_skb() up to
 *					icmp_rcv.
 *		Andi Kleen	:	Move the rate limit bookkeeping
 *					into the dest entry and use a token
 *					bucket filter (thanks to ANK). Make
 *					the rates sysctl configurable.
 *		Yu Tianli	:	Fixed two ugly bugs in icmp_send
 *					- IP option length was accounted wrongly
 *					- ICMP header length was not accounted
 *					  at all.
 *              Tristan Greaves :       Added sysctl option to ignore bogus
 *              			broadcast responses from broken routers.
 *
 * To Fix:
 *
 *	- Should use skb_pull() instead of all the manual checking.
 *	  This would also greatly simply some upper layer error handlers. --AK
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/types.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/fcntl.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/string.h>
#include <linux/netfilter_ipv4.h>
#include <linux/slab.h>
#include <net/snmp.h>
#include <net/ip.h>
#include <net/route.h>
#include <net/protocol.h>
#include <net/icmp.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/raw.h>
#include <net/ping.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <net/checksum.h>
#include <net/xfrm.h>
#include <net/inet_common.h>
#include <net/ip_fib.h>

/*
                             
 */

struct icmp_bxm {
	struct sk_buff *skb;
	int offset;
	int data_len;

	struct {
		struct icmphdr icmph;
		__be32	       times[3];
	} data;
	int head_len;
	struct ip_options_data replyopts;
};

/*                                                         */
/*                                                                                                            */

const struct icmp_err icmp_err_convert[] = {
	{
		.errno = ENETUNREACH,	/*                  */
		.fatal = 0,
	},
	{
		.errno = EHOSTUNREACH,	/*                   */
		.fatal = 0,
	},
	{
		.errno = ENOPROTOOPT	/*                   */,
		.fatal = 1,
	},
	{
		.errno = ECONNREFUSED,	/*                   */
		.fatal = 1,
	},
	{
		.errno = EMSGSIZE,	/*                  */
		.fatal = 0,
	},
	{
		.errno = EOPNOTSUPP,	/*                */
		.fatal = 0,
	},
	{
		.errno = ENETUNREACH,	/*                  */
		.fatal = 1,
	},
	{
		.errno = EHOSTDOWN,	/*                   */
		.fatal = 1,
	},
	{
		.errno = ENONET,	/*                    */
		.fatal = 1,
	},
	{
		.errno = ENETUNREACH,	/*              */
		.fatal = 1,
	},
	{
		.errno = EHOSTUNREACH,	/*               */
		.fatal = 1,
	},
	{
		.errno = ENETUNREACH,	/*                  */
		.fatal = 0,
	},
	{
		.errno = EHOSTUNREACH,	/*                   */
		.fatal = 0,
	},
	{
		.errno = EHOSTUNREACH,	/*                   */
		.fatal = 1,
	},
	{
		.errno = EHOSTUNREACH,	/*                     */
		.fatal = 1,
	},
	{
		.errno = EHOSTUNREACH,	/*                  */
		.fatal = 1,
	},
};
EXPORT_SYMBOL(icmp_err_convert);

/*
                                                                
 */

struct icmp_control {
	void (*handler)(struct sk_buff *skb);
	short   error;		/*                                          */
};

static const struct icmp_control icmp_pointers[NR_ICMP_TYPES+1];

/*
                                                                      
                                                                   
                                                         
  
                                          
 */
static struct sock *icmp_sk(struct net *net)
{
	return net->ipv4.icmp_sk[smp_processor_id()];
}

static inline struct sock *icmp_xmit_lock(struct net *net)
{
	struct sock *sk;

	local_bh_disable();

	sk = icmp_sk(net);

	if (unlikely(!spin_trylock(&sk->sk_lock.slock))) {
		/*                                             
                                                    
   */
		local_bh_enable();
		return NULL;
	}
	return sk;
}

static inline void icmp_xmit_unlock(struct sock *sk)
{
	spin_unlock_bh(&sk->sk_lock.slock);
}

/*
                      
 */

static inline bool icmpv4_xrlim_allow(struct net *net, struct rtable *rt,
				      struct flowi4 *fl4, int type, int code)
{
	struct dst_entry *dst = &rt->dst;
	bool rc = true;

	if (type > NR_ICMP_TYPES)
		goto out;

	/*                             */
	if (type == ICMP_DEST_UNREACH && code == ICMP_FRAG_NEEDED)
		goto out;

	/*                           */
	if (dst->dev && (dst->dev->flags&IFF_LOOPBACK))
		goto out;

	/*                                            */
	if ((1 << type) & net->ipv4.sysctl_icmp_ratemask) {
		struct inet_peer *peer = inet_getpeer_v4(net->ipv4.peers, fl4->daddr, 1);
		rc = inet_peer_xrlim_allow(peer,
					   net->ipv4.sysctl_icmp_ratelimit);
		if (peer)
			inet_putpeer(peer);
	}
out:
	return rc;
}

/*
                                                                      
 */
void icmp_out_count(struct net *net, unsigned char type)
{
	ICMPMSGOUT_INC_STATS(net, type);
	ICMP_INC_STATS(net, ICMP_MIB_OUTMSGS);
}

/*
                                                                         
            
 */
static int icmp_glue_bits(void *from, char *to, int offset, int len, int odd,
			  struct sk_buff *skb)
{
	struct icmp_bxm *icmp_param = (struct icmp_bxm *)from;
	__wsum csum;

	csum = skb_copy_and_csum_bits(icmp_param->skb,
				      icmp_param->offset + offset,
				      to, len, 0);

	skb->csum = csum_block_add(skb->csum, csum, odd);
	if (icmp_pointers[icmp_param->data.icmph.type].error)
		nf_ct_attach(skb, icmp_param->skb);
	return 0;
}

static void icmp_push_reply(struct icmp_bxm *icmp_param,
			    struct flowi4 *fl4,
			    struct ipcm_cookie *ipc, struct rtable **rt)
{
	struct sock *sk;
	struct sk_buff *skb;

	sk = icmp_sk(dev_net((*rt)->dst.dev));
	if (ip_append_data(sk, fl4, icmp_glue_bits, icmp_param,
			   icmp_param->data_len+icmp_param->head_len,
			   icmp_param->head_len,
			   ipc, rt, MSG_DONTWAIT) < 0) {
		ICMP_INC_STATS_BH(sock_net(sk), ICMP_MIB_OUTERRORS);
		ip_flush_pending_frames(sk);
	} else if ((skb = skb_peek(&sk->sk_write_queue)) != NULL) {
		struct icmphdr *icmph = icmp_hdr(skb);
		__wsum csum = 0;
		struct sk_buff *skb1;

		skb_queue_walk(&sk->sk_write_queue, skb1) {
			csum = csum_add(csum, skb1->csum);
		}
		csum = csum_partial_copy_nocheck((void *)&icmp_param->data,
						 (char *)icmph,
						 icmp_param->head_len, csum);
		icmph->checksum = csum_fold(csum);
		skb->ip_summed = CHECKSUM_NONE;
		ip_push_pending_frames(sk, fl4);
	}
}

/*
                                                        
 */

static void icmp_reply(struct icmp_bxm *icmp_param, struct sk_buff *skb)
{
	struct ipcm_cookie ipc;
	struct rtable *rt = skb_rtable(skb);
	struct net *net = dev_net(rt->dst.dev);
	struct flowi4 fl4;
	struct sock *sk;
	struct inet_sock *inet;
	__be32 daddr, saddr;
	u32 mark = IP4_REPLY_MARK(net, skb->mark);

	if (ip_options_echo(&icmp_param->replyopts.opt.opt, skb))
		return;

	sk = icmp_xmit_lock(net);
	if (sk == NULL)
		return;
	inet = inet_sk(sk);

	icmp_param->data.icmph.checksum = 0;

	inet->tos = ip_hdr(skb)->tos;
	sk->sk_mark = mark;
	daddr = ipc.addr = ip_hdr(skb)->saddr;
	saddr = fib_compute_spec_dst(skb);
	ipc.opt = NULL;
	ipc.tx_flags = 0;
	if (icmp_param->replyopts.opt.opt.optlen) {
		ipc.opt = &icmp_param->replyopts.opt;
		if (ipc.opt->opt.srr)
			daddr = icmp_param->replyopts.opt.opt.faddr;
	}
	memset(&fl4, 0, sizeof(fl4));
	fl4.daddr = daddr;
	fl4.saddr = saddr;
	fl4.flowi4_mark = mark;
	fl4.flowi4_tos = RT_TOS(ip_hdr(skb)->tos);
	fl4.flowi4_proto = IPPROTO_ICMP;
	security_skb_classify_flow(skb, flowi4_to_flowi(&fl4));
	rt = ip_route_output_key(net, &fl4);
	if (IS_ERR(rt))
		goto out_unlock;
	if (icmpv4_xrlim_allow(net, rt, &fl4, icmp_param->data.icmph.type,
			       icmp_param->data.icmph.code))
		icmp_push_reply(icmp_param, &fl4, &ipc, &rt);
	ip_rt_put(rt);
out_unlock:
	icmp_xmit_unlock(sk);
}

static struct rtable *icmp_route_lookup(struct net *net,
					struct flowi4 *fl4,
					struct sk_buff *skb_in,
					const struct iphdr *iph,
					__be32 saddr, u8 tos, u32 mark,
					int type, int code,
					struct icmp_bxm *param)
{
	struct rtable *rt, *rt2;
	struct flowi4 fl4_dec;
	int err;

	memset(fl4, 0, sizeof(*fl4));
	fl4->daddr = (param->replyopts.opt.opt.srr ?
		      param->replyopts.opt.opt.faddr : iph->saddr);
	fl4->saddr = saddr;
	fl4->flowi4_mark = mark;
	fl4->flowi4_tos = RT_TOS(tos);
	fl4->flowi4_proto = IPPROTO_ICMP;
	fl4->fl4_icmp_type = type;
	fl4->fl4_icmp_code = code;
	security_skb_classify_flow(skb_in, flowi4_to_flowi(fl4));
	rt = __ip_route_output_key(net, fl4);
	if (IS_ERR(rt))
		return rt;

	/*                                                      */
	rt2 = rt;

	rt = (struct rtable *) xfrm_lookup(net, &rt->dst,
					   flowi4_to_flowi(fl4), NULL, 0);
	if (!IS_ERR(rt)) {
		if (rt != rt2)
			return rt;
	} else if (PTR_ERR(rt) == -EPERM) {
		rt = NULL;
	} else
		return rt;

	err = xfrm_decode_session_reverse(skb_in, flowi4_to_flowi(&fl4_dec), AF_INET);
	if (err)
		goto relookup_failed;

	if (inet_addr_type(net, fl4_dec.saddr) == RTN_LOCAL) {
		rt2 = __ip_route_output_key(net, &fl4_dec);
		if (IS_ERR(rt2))
			err = PTR_ERR(rt2);
	} else {
		struct flowi4 fl4_2 = {};
		unsigned long orefdst;

		fl4_2.daddr = fl4_dec.saddr;
		rt2 = ip_route_output_key(net, &fl4_2);
		if (IS_ERR(rt2)) {
			err = PTR_ERR(rt2);
			goto relookup_failed;
		}
		/*      */
		orefdst = skb_in->_skb_refdst; /*                 */
		err = ip_route_input(skb_in, fl4_dec.daddr, fl4_dec.saddr,
				     RT_TOS(tos), rt2->dst.dev);

		dst_release(&rt2->dst);
		rt2 = skb_rtable(skb_in);
		skb_in->_skb_refdst = orefdst; /*                    */
	}

	if (err)
		goto relookup_failed;

	rt2 = (struct rtable *) xfrm_lookup(net, &rt2->dst,
					    flowi4_to_flowi(&fl4_dec), NULL,
					    XFRM_LOOKUP_ICMP);
	if (!IS_ERR(rt2)) {
		dst_release(&rt->dst);
		memcpy(fl4, &fl4_dec, sizeof(*fl4));
		rt = rt2;
	} else if (PTR_ERR(rt2) == -EPERM) {
		if (rt)
			dst_release(&rt->dst);
		return rt2;
	} else {
		err = PTR_ERR(rt2);
		goto relookup_failed;
	}
	return rt;

relookup_failed:
	if (rt)
		return rt;
	return ERR_PTR(err);
}

/*
                                                  
  
                                                                          
                            
                                             
                                                        
                                                         
                                           
 */

void icmp_send(struct sk_buff *skb_in, int type, int code, __be32 info)
{
	struct iphdr *iph;
	int room;
	struct icmp_bxm icmp_param;
	struct rtable *rt = skb_rtable(skb_in);
	struct ipcm_cookie ipc;
	struct flowi4 fl4;
	__be32 saddr;
	u8  tos;
	u32 mark;
	struct net *net;
	struct sock *sk;

	if (!rt)
		goto out;
	net = dev_net(rt->dst.dev);

	/*
                                                                    
                                                                 
              
  */
	iph = ip_hdr(skb_in);

	if ((u8 *)iph < skb_in->head ||
	    (skb_in->network_header + sizeof(*iph)) > skb_in->tail)
		goto out;

	/*
                                              
  */
	if (skb_in->pkt_type != PACKET_HOST)
		goto out;

	/*
                                   
  */
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		goto out;

	/*
                                                           
                        
  */
	if (iph->frag_off & htons(IP_OFFSET))
		goto out;

	/*
                                                                   
  */
	if (icmp_pointers[type].error) {
		/*
                                                    
               
   */
		if (iph->protocol == IPPROTO_ICMP) {
			u8 _inner_type, *itp;

			itp = skb_header_pointer(skb_in,
						 skb_network_header(skb_in) +
						 (iph->ihl << 2) +
						 offsetof(struct icmphdr,
							  type) -
						 skb_in->data,
						 sizeof(_inner_type),
						 &_inner_type);
			if (itp == NULL)
				goto out;

			/*
                                                    
                                                      
    */
			if (*itp > NR_ICMP_TYPES ||
			    icmp_pointers[*itp].error)
				goto out;
		}
	}

	sk = icmp_xmit_lock(net);
	if (sk == NULL)
		return;

	/*
                                         
  */

	saddr = iph->daddr;
	if (!(rt->rt_flags & RTCF_LOCAL)) {
		struct net_device *dev = NULL;

		rcu_read_lock();
		if (rt_is_input_route(rt) &&
		    net->ipv4.sysctl_icmp_errors_use_inbound_ifaddr)
			dev = dev_get_by_index_rcu(net, inet_iif(skb_in));

		if (dev)
			saddr = inet_select_addr(dev, 0, RT_SCOPE_LINK);
		else
			saddr = 0;
		rcu_read_unlock();
	}

	tos = icmp_pointers[type].error ? ((iph->tos & IPTOS_TOS_MASK) |
					   IPTOS_PREC_INTERNETCONTROL) :
					  iph->tos;
	mark = IP4_REPLY_MARK(net, skb_in->mark);

	if (ip_options_echo(&icmp_param.replyopts.opt.opt, skb_in))
		goto out_unlock;


	/*
                                 
  */

	icmp_param.data.icmph.type	 = type;
	icmp_param.data.icmph.code	 = code;
	icmp_param.data.icmph.un.gateway = info;
	icmp_param.data.icmph.checksum	 = 0;
	icmp_param.skb	  = skb_in;
	icmp_param.offset = skb_network_offset(skb_in);
	inet_sk(sk)->tos = tos;
	sk->sk_mark = mark;
	ipc.addr = iph->saddr;
	ipc.opt = &icmp_param.replyopts.opt;
	ipc.tx_flags = 0;

	rt = icmp_route_lookup(net, &fl4, skb_in, iph, saddr, tos, mark,
			       type, code, &icmp_param);
	if (IS_ERR(rt))
		goto out_unlock;

	if (!icmpv4_xrlim_allow(net, rt, &fl4, type, code))
		goto ende;

	/*                                                                */

	room = dst_mtu(&rt->dst);
	if (room > 576)
		room = 576;
	room -= sizeof(struct iphdr) + icmp_param.replyopts.opt.opt.optlen;
	room -= sizeof(struct icmphdr);

	icmp_param.data_len = skb_in->len - icmp_param.offset;
	if (icmp_param.data_len > room)
		icmp_param.data_len = room;
	icmp_param.head_len = sizeof(struct icmphdr);

	icmp_push_reply(&icmp_param, &fl4, &ipc, &rt);
ende:
	ip_rt_put(rt);
out_unlock:
	icmp_xmit_unlock(sk);
out:;
}
EXPORT_SYMBOL(icmp_send);


static void icmp_socket_deliver(struct sk_buff *skb, u32 info)
{
	const struct iphdr *iph = (const struct iphdr *) skb->data;
	const struct net_protocol *ipprot;
	int protocol = iph->protocol;

	/*                                                   
                                                 
  */
	if (!pskb_may_pull(skb, iph->ihl * 4 + 8))
		return;

	raw_icmp_error(skb, protocol, info);

	rcu_read_lock();
	ipprot = rcu_dereference(inet_protos[protocol]);
	if (ipprot && ipprot->err_handler)
		ipprot->err_handler(skb, info);
	rcu_read_unlock();
}

/*
                                                               
 */

static void icmp_unreach(struct sk_buff *skb)
{
	const struct iphdr *iph;
	struct icmphdr *icmph;
	struct net *net;
	u32 info = 0;

	net = dev_net(skb_dst(skb)->dev);

	/*
                       
                                                      
                                                        
  */

	if (!pskb_may_pull(skb, sizeof(struct iphdr)))
		goto out_err;

	icmph = icmp_hdr(skb);
	iph   = (const struct iphdr *)skb->data;

	if (iph->ihl < 5) /*                       */
		goto out_err;

	if (icmph->type == ICMP_DEST_UNREACH) {
		switch (icmph->code & 15) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			break;
		case ICMP_FRAG_NEEDED:
			if (ipv4_config.no_pmtu_disc) {
				LIMIT_NETDEBUG(KERN_INFO pr_fmt("%pI4: fragmentation needed and DF set\n"),
					       &iph->daddr);
			} else {
				info = ntohs(icmph->un.frag.mtu);
			}
			break;
		case ICMP_SR_FAILED:
			LIMIT_NETDEBUG(KERN_INFO pr_fmt("%pI4: Source Route Failed\n"),
				       &iph->daddr);
			break;
		default:
			break;
		}
		if (icmph->code > NR_ICMP_UNREACH)
			goto out;
	} else if (icmph->type == ICMP_PARAMETERPROB)
		info = ntohl(icmph->un.gateway) >> 24;

	/*
                                
   
                                                                
              
                                                            
                       
                                                             
                       
  */

	/*
                                                                   
                                                                
                                                                  
                                          
  */

	if (!net->ipv4.sysctl_icmp_ignore_bogus_error_responses &&
	    inet_addr_type(net, iph->daddr) == RTN_BROADCAST) {
		net_warn_ratelimited("%pI4 sent an invalid ICMP type %u, code %u error to a broadcast: %pI4 on %s\n",
				     &ip_hdr(skb)->saddr,
				     icmph->type, icmph->code,
				     &iph->daddr, skb->dev->name);
		goto out;
	}

	icmp_socket_deliver(skb, info);

out:
	return;
out_err:
	ICMP_INC_STATS_BH(net, ICMP_MIB_INERRORS);
	goto out;
}


/*
                        
 */

static void icmp_redirect(struct sk_buff *skb)
{
	if (skb->len < sizeof(struct iphdr)) {
		ICMP_INC_STATS_BH(dev_net(skb->dev), ICMP_MIB_INERRORS);
		return;
	}

	if (!pskb_may_pull(skb, sizeof(struct iphdr)))
		return;

	icmp_socket_deliver(skb, icmp_hdr(skb)->un.gateway);
}

/*
                                      
  
                                                                    
               
                                                                   
                            
                                                                      
                                           
                                                                   
 */

static void icmp_echo(struct sk_buff *skb)
{
	struct net *net;

	net = dev_net(skb_dst(skb)->dev);
	if (!net->ipv4.sysctl_icmp_echo_ignore_all) {
		struct icmp_bxm icmp_param;

		icmp_param.data.icmph	   = *icmp_hdr(skb);
		icmp_param.data.icmph.type = ICMP_ECHOREPLY;
		icmp_param.skb		   = skb;
		icmp_param.offset	   = 0;
		icmp_param.data_len	   = skb->len;
		icmp_param.head_len	   = sizeof(struct icmphdr);
		icmp_reply(&icmp_param, skb);
	}
}

/*
                                  
                                                           
                                                         
                                        
                                       
 */
static void icmp_timestamp(struct sk_buff *skb)
{
	struct timespec tv;
	struct icmp_bxm icmp_param;
	/*
              
  */
	if (skb->len < 4)
		goto out_err;

	/*
                                                     
  */
	getnstimeofday(&tv);
	icmp_param.data.times[1] = htonl((tv.tv_sec % 86400) * MSEC_PER_SEC +
					 tv.tv_nsec / NSEC_PER_MSEC);
	icmp_param.data.times[2] = icmp_param.data.times[1];
	if (skb_copy_bits(skb, 0, &icmp_param.data.times[0], 4))
		BUG();
	icmp_param.data.icmph	   = *icmp_hdr(skb);
	icmp_param.data.icmph.type = ICMP_TIMESTAMPREPLY;
	icmp_param.data.icmph.code = 0;
	icmp_param.skb		   = skb;
	icmp_param.offset	   = 0;
	icmp_param.data_len	   = 0;
	icmp_param.head_len	   = sizeof(struct icmphdr) + 12;
	icmp_reply(&icmp_param, skb);
out:
	return;
out_err:
	ICMP_INC_STATS_BH(dev_net(skb_dst(skb)->dev), ICMP_MIB_INERRORS);
	goto out;
}

static void icmp_discard(struct sk_buff *skb)
{
}

/*
                                   
 */
int icmp_rcv(struct sk_buff *skb)
{
	struct icmphdr *icmph;
	struct rtable *rt = skb_rtable(skb);
	struct net *net = dev_net(rt->dst.dev);
	/*                           */
	struct sock *ping_sk = NULL;
	
	if (!xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb)) {
		struct sec_path *sp = skb_sec_path(skb);
		int nh;

		if (!(sp && sp->xvec[sp->len - 1]->props.flags &
				 XFRM_STATE_ICMP))
			goto drop;

		if (!pskb_may_pull(skb, sizeof(*icmph) + sizeof(struct iphdr)))
			goto drop;

		nh = skb_network_offset(skb);
		skb_set_network_header(skb, sizeof(*icmph));

		if (!xfrm4_policy_check_reverse(NULL, XFRM_POLICY_IN, skb))
			goto drop;

		skb_set_network_header(skb, nh);
	}

	ICMP_INC_STATS_BH(net, ICMP_MIB_INMSGS);

	switch (skb->ip_summed) {
	case CHECKSUM_COMPLETE:
		if (!csum_fold(skb->csum))
			break;
		/*              */
	case CHECKSUM_NONE:
		skb->csum = 0;
		if (__skb_checksum_complete(skb))
			goto csum_error;
	}

	if (!pskb_pull(skb, sizeof(*icmph)))
		goto error;

	icmph = icmp_hdr(skb);

	ICMPMSGIN_INC_STATS_BH(net, icmph->type);
	/*
                                                                   
   
                                                                 
                 
  */
	if (icmph->type > NR_ICMP_TYPES)
		goto error;


	/*
                          
  */

	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST)) {
		/*
                                                       
                                                           
                                                        
                                           
   */
		if ((icmph->type == ICMP_ECHO ||
		     icmph->type == ICMP_TIMESTAMP) &&
		    net->ipv4.sysctl_icmp_echo_ignore_broadcasts) {
			goto error;
		}
		if (icmph->type != ICMP_ECHO &&
		    icmph->type != ICMP_TIMESTAMP &&
		    icmph->type != ICMP_ADDRESS &&
		    icmph->type != ICMP_ADDRESSREPLY) {
			goto error;
		}
	}

	icmp_pointers[icmph->type].handler(skb);
	
	if(icmph->type == ICMP_ECHOREPLY && skb->sk != NULL ){
	    ping_sk = skb->sk;
 	}
 	
drop:
	if(ping_sk){
	    kfree_skb(skb);
	    sock_put(ping_sk);
	} else {
	    kfree_skb(skb);
	}
	return 0;
csum_error:
	ICMP_INC_STATS_BH(net, ICMP_MIB_CSUMERRORS);
error:
	ICMP_INC_STATS_BH(net, ICMP_MIB_INERRORS);
	goto drop;
}

void icmp_err(struct sk_buff *skb, u32 info)
{
	struct iphdr *iph = (struct iphdr *)skb->data;
	int offset = iph->ihl<<2;
	struct icmphdr *icmph = (struct icmphdr *)(skb->data + offset);
	int type = icmp_hdr(skb)->type;
	int code = icmp_hdr(skb)->code;
	struct net *net = dev_net(skb->dev);

	/*
                                                       
                                                       
  */
	if (icmph->type != ICMP_ECHOREPLY) {
		ping_err(skb, offset, info);
		return;
	}

	if (type == ICMP_DEST_UNREACH && code == ICMP_FRAG_NEEDED)
		ipv4_update_pmtu(skb, net, info, 0, 0, IPPROTO_ICMP, 0);
	else if (type == ICMP_REDIRECT)
		ipv4_redirect(skb, net, 0, 0, IPPROTO_ICMP, 0);
}

/*
                                                      
 */
static const struct icmp_control icmp_pointers[NR_ICMP_TYPES + 1] = {
	[ICMP_ECHOREPLY] = {
		.handler = ping_rcv,
	},
	[1] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[2] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[ICMP_DEST_UNREACH] = {
		.handler = icmp_unreach,
		.error = 1,
	},
	[ICMP_SOURCE_QUENCH] = {
		.handler = icmp_unreach,
		.error = 1,
	},
	[ICMP_REDIRECT] = {
		.handler = icmp_redirect,
		.error = 1,
	},
	[6] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[7] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[ICMP_ECHO] = {
		.handler = icmp_echo,
	},
	[9] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[10] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[ICMP_TIME_EXCEEDED] = {
		.handler = icmp_unreach,
		.error = 1,
	},
	[ICMP_PARAMETERPROB] = {
		.handler = icmp_unreach,
		.error = 1,
	},
	[ICMP_TIMESTAMP] = {
		.handler = icmp_timestamp,
	},
	[ICMP_TIMESTAMPREPLY] = {
		.handler = icmp_discard,
	},
	[ICMP_INFO_REQUEST] = {
		.handler = icmp_discard,
	},
	[ICMP_INFO_REPLY] = {
		.handler = icmp_discard,
	},
	[ICMP_ADDRESS] = {
		.handler = icmp_discard,
	},
	[ICMP_ADDRESSREPLY] = {
		.handler = icmp_discard,
	},
};

static void __net_exit icmp_sk_exit(struct net *net)
{
	int i;

	for_each_possible_cpu(i)
		inet_ctl_sock_destroy(net->ipv4.icmp_sk[i]);
	kfree(net->ipv4.icmp_sk);
	net->ipv4.icmp_sk = NULL;
}

static int __net_init icmp_sk_init(struct net *net)
{
	int i, err;

	net->ipv4.icmp_sk =
		kzalloc(nr_cpu_ids * sizeof(struct sock *), GFP_KERNEL);
	if (net->ipv4.icmp_sk == NULL)
		return -ENOMEM;

	for_each_possible_cpu(i) {
		struct sock *sk;

		err = inet_ctl_sock_create(&sk, PF_INET,
					   SOCK_RAW, IPPROTO_ICMP, net);
		if (err < 0)
			goto fail;

		net->ipv4.icmp_sk[i] = sk;

		/*                                               
                                             
   */
		sk->sk_sndbuf =	2 * SKB_TRUESIZE(64 * 1024);

		/*
                         
   */
		sock_set_flag(sk, SOCK_USE_WRITE_QUEUE);
		inet_sk(sk)->pmtudisc = IP_PMTUDISC_DONT;
	}

	/*                                      */
	net->ipv4.sysctl_icmp_echo_ignore_all = 0;
	net->ipv4.sysctl_icmp_echo_ignore_broadcasts = 1;

	/*                                                       */
	net->ipv4.sysctl_icmp_ignore_bogus_error_responses = 1;

	/*
                                    
   
                                                                
                                                               
                              
   
            
                                            
                                              
  */

	net->ipv4.sysctl_icmp_ratelimit = 1 * HZ;
	net->ipv4.sysctl_icmp_ratemask = 0x1818;
	net->ipv4.sysctl_icmp_errors_use_inbound_ifaddr = 0;

	return 0;

fail:
	for_each_possible_cpu(i)
		inet_ctl_sock_destroy(net->ipv4.icmp_sk[i]);
	kfree(net->ipv4.icmp_sk);
	return err;
}

static struct pernet_operations __net_initdata icmp_sk_ops = {
       .init = icmp_sk_init,
       .exit = icmp_sk_exit,
};

int __init icmp_init(void)
{
	return register_pernet_subsys(&icmp_sk_ops);
}