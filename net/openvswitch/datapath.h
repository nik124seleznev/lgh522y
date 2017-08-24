/*
 * Copyright (c) 2007-2012 Nicira, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef DATAPATH_H
#define DATAPATH_H 1

#include <asm/page.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/u64_stats_sync.h>

#include "flow.h"
#include "vport.h"

#define DP_MAX_PORTS           USHRT_MAX
#define DP_VPORT_HASH_BUCKETS  1024

#define SAMPLE_ACTION_DEPTH 3

/* 
                                                                            
            
                                                                            
                  
                                                                            
                                                                           
                                 
                                                                            
                                                                            
                                 
 */
struct dp_stats_percpu {
	u64 n_hit;
	u64 n_missed;
	u64 n_lost;
	struct u64_stats_sync sync;
};

/* 
                                                             
                                                    
                                            
                                                               
                                                                               
                     
                                              
                                    
  
                                                                              
                       
 */
struct datapath {
	struct rcu_head rcu;
	struct list_head list_node;

	/*             */
	struct flow_table __rcu *table;

	/*               */
	struct hlist_head *ports;

	/*        */
	struct dp_stats_percpu __percpu *stats_percpu;

#ifdef CONFIG_NET_NS
	/*                        */
	struct net *net;
#endif
};

/* 
                                         
                                                                         
 */
struct ovs_skb_cb {
	struct sw_flow		*flow;
};
#define OVS_CB(skb) ((struct ovs_skb_cb *)(skb)->cb)

/* 
                                                                            
                                  
                                                        
                                                                             
                             
                                                                          
                                                                       
           
 */
struct dp_upcall_info {
	u8 cmd;
	const struct sw_flow_key *key;
	const struct nlattr *userdata;
	u32 portid;
};

/* 
                                                   
                                                          
                           
 */
struct ovs_net {
	struct list_head dps;
	struct work_struct dp_notify_work;
};

extern int ovs_net_id;
void ovs_lock(void);
void ovs_unlock(void);

#ifdef CONFIG_LOCKDEP
int lockdep_ovsl_is_held(void);
#else
#define lockdep_ovsl_is_held()	1
#endif

#define ASSERT_OVSL()		WARN_ON(unlikely(!lockdep_ovsl_is_held()))
#define ovsl_dereference(p)					\
	rcu_dereference_protected(p, lockdep_ovsl_is_held())

static inline struct net *ovs_dp_get_net(struct datapath *dp)
{
	return read_pnet(&dp->net);
}

static inline void ovs_dp_set_net(struct datapath *dp, struct net *net)
{
	write_pnet(&dp->net, net);
}

struct vport *ovs_lookup_vport(const struct datapath *dp, u16 port_no);

static inline struct vport *ovs_vport_rcu(const struct datapath *dp, int port_no)
{
	WARN_ON_ONCE(!rcu_read_lock_held());
	return ovs_lookup_vport(dp, port_no);
}

static inline struct vport *ovs_vport_ovsl_rcu(const struct datapath *dp, int port_no)
{
	WARN_ON_ONCE(!rcu_read_lock_held() && !lockdep_ovsl_is_held());
	return ovs_lookup_vport(dp, port_no);
}

static inline struct vport *ovs_vport_ovsl(const struct datapath *dp, int port_no)
{
	ASSERT_OVSL();
	return ovs_lookup_vport(dp, port_no);
}

extern struct notifier_block ovs_dp_device_notifier;
extern struct genl_multicast_group ovs_dp_vport_multicast_group;

void ovs_dp_process_received_packet(struct vport *, struct sk_buff *);
void ovs_dp_detach_port(struct vport *);
int ovs_dp_upcall(struct datapath *, struct sk_buff *,
		  const struct dp_upcall_info *);

const char *ovs_dp_name(const struct datapath *dp);
struct sk_buff *ovs_vport_cmd_build_info(struct vport *, u32 pid, u32 seq,
					 u8 cmd);

int ovs_execute_actions(struct datapath *dp, struct sk_buff *skb);
void ovs_dp_notify_wq(struct work_struct *work);
#endif /*            */
