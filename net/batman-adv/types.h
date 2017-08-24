/* Copyright (C) 2007-2013 B.A.T.M.A.N. contributors:
 *
 * Marek Lindner, Simon Wunderlich
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

#ifndef _NET_BATMAN_ADV_TYPES_H_
#define _NET_BATMAN_ADV_TYPES_H_

#include "packet.h"
#include "bitarray.h"
#include <linux/kernel.h>

/* 
                                                              
 */
#define BATADV_HEADER_LEN \
	(ETH_HLEN + max(sizeof(struct batadv_unicast_packet), \
			sizeof(struct batadv_bcast_packet)))

#ifdef CONFIG_BATMAN_ADV_DAT

/*                                                                            
                                          
  
                                                          
 */
#define batadv_dat_addr_t uint16_t

#endif /*                       */

/* 
                                                                            
                                           
                                                 
                                                              
 */
struct batadv_hard_iface_bat_iv {
	unsigned char *ogm_buff;
	int ogm_buff_len;
	atomic_t ogm_seqno;
};

/* 
                                                                
                                          
                                          
                                                     
                                      
                                                                    
                                                                   
                                                   
                                                                                
                                 
                                                                          
                                                      
                                                      
                                                                    
 */
struct batadv_hard_iface {
	struct list_head list;
	int16_t if_num;
	char if_status;
	struct net_device *net_dev;
	atomic_t frag_seqno;
	struct kobject *hardif_obj;
	atomic_t refcount;
	struct packet_type batman_adv_ptype;
	struct net_device *soft_iface;
	struct rcu_head rcu;
	struct batadv_hard_iface_bat_iv bat_iv;
	struct work_struct cleanup_work;
};

/* 
                                                                              
                                     
                                                 
                                                               
                                                                        
                                                                        
                                                            
                                              
                                                                
                                                                     
                                                                   
                                            
                                       
                                                         
                                        
                                                                    
                                                                            
             
                                                            
                                                                   
                                                                                
                                                            
                                                        
                                         
                                                                               
                                                               
                     
                                                                          
                                                                          
                                                                 
                                                                         
            
                                                                        
                                                     
                                                             
                                                           
                                                         
                                                                   
                                                      
                                         
                                                   
                                                      
                                                    
                                                          
                                                
                                                  
 */
struct batadv_orig_node {
	uint8_t orig[ETH_ALEN];
	uint8_t primary_addr[ETH_ALEN];
	struct batadv_neigh_node __rcu *router; /*                       */
#ifdef CONFIG_BATMAN_ADV_DAT
	batadv_dat_addr_t dat_addr;
#endif
	unsigned long *bcast_own;
	uint8_t *bcast_own_sum;
	unsigned long last_seen;
	unsigned long bcast_seqno_reset;
	unsigned long batman_seqno_reset;
	uint8_t gw_flags;
	uint8_t flags;
	atomic_t last_ttvn;
	uint16_t tt_crc;
	unsigned char *tt_buff;
	int16_t tt_buff_len;
	spinlock_t tt_buff_lock; /*                                */
	atomic_t tt_size;
	bool tt_initialised;
	uint32_t last_real_seqno;
	uint8_t last_ttl;
	DECLARE_BITMAP(bcast_bits, BATADV_TQ_LOCAL_WINDOW_SIZE);
	uint32_t last_bcast_seqno;
	struct hlist_head neigh_list;
	struct list_head frag_list;
	unsigned long last_frag_packet;
	/*                                                             */
	spinlock_t neigh_list_lock;
	struct hlist_node hash_entry;
	struct batadv_priv *bat_priv;
	/*                                                 
                                                         
  */
	spinlock_t ogm_cnt_lock;
	/*                                                          */
	spinlock_t bcast_seqno_lock;
	atomic_t bond_candidates;
	struct list_head bond_list;
	atomic_t refcount;
	struct rcu_head rcu;
#ifdef CONFIG_BATMAN_ADV_NC
	struct list_head in_coding_list;
	struct list_head out_coding_list;
	spinlock_t in_coding_list_lock; /*                         */
	spinlock_t out_coding_list_lock; /*                          */
#endif
};

/* 
                                                                              
                                            
                                                 
                                                  
                                                   
                                                      
 */
struct batadv_gw_node {
	struct hlist_node list;
	struct batadv_orig_node *orig_node;
	unsigned long deleted;
	atomic_t refcount;
	struct rcu_head rcu;
};

/* 
                                                                
                                                    
                                   
                                                                   
                               
                                                                     
                                                    
                                                           
                                                              
                                                                              
                                                 
                                                  
                                                 
                                                   
                                                      
                                                   
                                                      
 */
struct batadv_neigh_node {
	struct hlist_node list;
	uint8_t addr[ETH_ALEN];
	uint8_t tq_recv[BATADV_TQ_GLOBAL_WINDOW_SIZE];
	uint8_t tq_index;
	uint8_t tq_avg;
	uint8_t last_ttl;
	struct list_head bonding_list;
	unsigned long last_seen;
	DECLARE_BITMAP(real_bits, BATADV_TQ_LOCAL_WINDOW_SIZE);
	uint8_t real_packet_count;
	struct batadv_orig_node *orig_node;
	struct batadv_hard_iface *if_incoming;
	spinlock_t lq_update_lock; /*                             */
	atomic_t refcount;
	struct rcu_head rcu;
};

/* 
                                                                              
                                                                     
                                            
                                                          
 */
#ifdef CONFIG_BATMAN_ADV_BLA
struct batadv_bcast_duplist_entry {
	uint8_t orig[ETH_ALEN];
	__be32 crc;
	unsigned long entrytime;
};
#endif

/* 
                                                      
                                                             
                                                                  
                                                                              
                                                          
                                                               
                                                                
                                                                     
                                                                           
                                                                                
                                                                        
                                                                             
                                                                       
                                                                    
                                                                         
                                                                      
                                                                         
                                                                      
                                                                     
                                                                  
                                                                     
                                                                  
                                                                              
           
                                                                      
                                                                           
                                                                          
                                                                               
                                                                            
                                                                        
                                                                             
                                                                          
           
                                                                             
         
                                              
 */
enum batadv_counters {
	BATADV_CNT_TX,
	BATADV_CNT_TX_BYTES,
	BATADV_CNT_TX_DROPPED,
	BATADV_CNT_RX,
	BATADV_CNT_RX_BYTES,
	BATADV_CNT_FORWARD,
	BATADV_CNT_FORWARD_BYTES,
	BATADV_CNT_MGMT_TX,
	BATADV_CNT_MGMT_TX_BYTES,
	BATADV_CNT_MGMT_RX,
	BATADV_CNT_MGMT_RX_BYTES,
	BATADV_CNT_TT_REQUEST_TX,
	BATADV_CNT_TT_REQUEST_RX,
	BATADV_CNT_TT_RESPONSE_TX,
	BATADV_CNT_TT_RESPONSE_RX,
	BATADV_CNT_TT_ROAM_ADV_TX,
	BATADV_CNT_TT_ROAM_ADV_RX,
#ifdef CONFIG_BATMAN_ADV_DAT
	BATADV_CNT_DAT_GET_TX,
	BATADV_CNT_DAT_GET_RX,
	BATADV_CNT_DAT_PUT_TX,
	BATADV_CNT_DAT_PUT_RX,
	BATADV_CNT_DAT_CACHED_REPLY_TX,
#endif
#ifdef CONFIG_BATMAN_ADV_NC
	BATADV_CNT_NC_CODE,
	BATADV_CNT_NC_CODE_BYTES,
	BATADV_CNT_NC_RECODE,
	BATADV_CNT_NC_RECODE_BYTES,
	BATADV_CNT_NC_BUFFER,
	BATADV_CNT_NC_DECODE,
	BATADV_CNT_NC_DECODE_BYTES,
	BATADV_CNT_NC_DECODE_FAILED,
	BATADV_CNT_NC_SNIFFED,
#endif
	BATADV_CNT_NUM,
};

/* 
                                                                    
                                        
                                                                          
                                                               
                                                                       
                                                  
                                                    
                                                      
                                                                          
                                                    
                                                   
                                           
                                             
                                                              
                                                                               
                                                             
                                                                           
                                                                            
                                                                
 */
struct batadv_priv_tt {
	atomic_t vn;
	atomic_t ogm_append_cnt;
	atomic_t local_changes;
	struct list_head changes_list;
	struct batadv_hashtable *local_hash;
	struct batadv_hashtable *global_hash;
	struct list_head req_list;
	struct list_head roam_list;
	spinlock_t changes_list_lock; /*                  */
	spinlock_t req_list_lock; /*                   */
	spinlock_t roam_list_lock; /*                    */
	atomic_t local_entry_num;
	uint16_t local_crc;
	unsigned char *last_changeset;
	int16_t last_changeset_len;
	/*                                              */
	spinlock_t last_changeset_lock;
	struct delayed_work work;
};

/* 
                                                                          
                                                  
                                                                      
                                                                       
                                                                           
                          
                                                                             
                                                                          
                                                   
                                                                   
 */
#ifdef CONFIG_BATMAN_ADV_BLA
struct batadv_priv_bla {
	atomic_t num_requests;
	struct batadv_hashtable *claim_hash;
	struct batadv_hashtable *backbone_hash;
	struct batadv_bcast_duplist_entry bcast_duplist[BATADV_DUPLIST_SIZE];
	int bcast_duplist_curr;
	/*                                             */
	spinlock_t bcast_duplist_lock;
	struct batadv_bla_claim_dst claim_dest;
	struct delayed_work work;
};
#endif

/* 
                                               
                                                  
                                              
                                             
                                                       
                                       
 */
#ifdef CONFIG_BATMAN_ADV_DEBUG
struct batadv_priv_debug_log {
	char log_buff[BATADV_LOG_BUF_LEN];
	unsigned long log_start;
	unsigned long log_end;
	spinlock_t lock; /*                                          */
	wait_queue_head_t queue_wait;
};
#endif

/* 
                                                          
                                         
                                                
                                                       
                                                                   
 */
struct batadv_priv_gw {
	struct hlist_head list;
	spinlock_t list_lock; /*                            */
	struct batadv_gw_node __rcu *curr_gw;  /*                       */
	atomic_t reselect;
};

/* 
                                                       
                                                      
                                                                        
                                             
                                                 
                                                         
                                                               
 */
struct batadv_priv_vis {
	struct list_head send_list;
	struct batadv_hashtable *hash;
	spinlock_t hash_lock; /*               */
	spinlock_t list_lock; /*                             */
	struct delayed_work work;
	struct batadv_vis_info *my_info;
};

/* 
                                                               
                          
                                                    
                                                    
 */
#ifdef CONFIG_BATMAN_ADV_DAT
struct batadv_priv_dat {
	batadv_dat_addr_t addr;
	struct batadv_hashtable *hash;
	struct delayed_work work;
};
#endif

/* 
                                                                         
                                              
                                                                      
                                                                     
                                                                          
                                                                     
                                                                     
                                                                         
                                                                         
                                                                                
                          
                                                                                
                                                                        
                                                                        
                            
 */
struct batadv_priv_nc {
	struct delayed_work work;
	struct dentry *debug_dir;
	u8 min_tq;
	u32 max_fwd_delay;
	u32 max_buffer_time;
	unsigned long timestamp_fwd_flush;
	unsigned long timestamp_sniffed_purge;
	struct batadv_hashtable *coding_hash;
	struct batadv_hashtable *decoding_hash;
};

/* 
                                               
                                                                         
                                                                  
                                                                  
                                                                                
                                                                       
                                                               
                                                                           
                                                                 
                                                                           
           
                                                                           
           
                                                                         
                                                                           
                                                                     
                                                                         
                                                         
                                                                                
                                                          
                                                           
                                                                         
                                                           
                                                                    
                                                 
                                                         
                                                                 
                                                                         
                                                                   
                                                     
                                                         
                                                             
                                                                    
                                                                          
                                 
                                                               
                                    
                                                  
                    
                              
                 
                                   
                                                                     
                                       
 */
struct batadv_priv {
	atomic_t mesh_state;
	struct net_device *soft_iface;
	struct net_device_stats stats;
	uint64_t __percpu *bat_counters; /*                  */
	atomic_t aggregated_ogms;
	atomic_t bonding;
	atomic_t fragmentation;
	atomic_t ap_isolation;
#ifdef CONFIG_BATMAN_ADV_BLA
	atomic_t bridge_loop_avoidance;
#endif
#ifdef CONFIG_BATMAN_ADV_DAT
	atomic_t distributed_arp_table;
#endif
	atomic_t vis_mode;
	atomic_t gw_mode;
	atomic_t gw_sel_class;
	atomic_t gw_bandwidth;
	atomic_t orig_interval;
	atomic_t hop_penalty;
#ifdef CONFIG_BATMAN_ADV_DEBUG
	atomic_t log_level;
#endif
	atomic_t bcast_seqno;
	atomic_t bcast_queue_left;
	atomic_t batman_queue_left;
	char num_ifaces;
	struct kobject *mesh_obj;
	struct dentry *debug_dir;
	struct hlist_head forw_bat_list;
	struct hlist_head forw_bcast_list;
	struct batadv_hashtable *orig_hash;
	spinlock_t forw_bat_list_lock; /*                        */
	spinlock_t forw_bcast_list_lock; /*                          */
	struct delayed_work orig_work;
	struct work_struct cleanup_work;
	struct batadv_hard_iface __rcu *primary_if;  /*                       */
	struct batadv_algo_ops *bat_algo_ops;
#ifdef CONFIG_BATMAN_ADV_BLA
	struct batadv_priv_bla bla;
#endif
#ifdef CONFIG_BATMAN_ADV_DEBUG
	struct batadv_priv_debug_log *debug_log;
#endif
	struct batadv_priv_gw gw;
	struct batadv_priv_tt tt;
	struct batadv_priv_vis vis;
#ifdef CONFIG_BATMAN_ADV_DAT
	struct batadv_priv_dat dat;
#endif
#ifdef CONFIG_BATMAN_ADV_NC
	atomic_t network_coding;
	struct batadv_priv_nc nc;
#endif /*                      */
};

/* 
                                                               
                                                                        
                                                                 
                                                                 
                                                       
                                          
                                                          
 */
struct batadv_socket_client {
	struct list_head queue_list;
	unsigned int queue_len;
	unsigned char index;
	spinlock_t lock; /*                                        */
	wait_queue_head_t queue_wait;
	struct batadv_priv *bat_priv;
};

/* 
                                                                     
                                                        
                                            
                                   
 */
struct batadv_socket_packet {
	struct list_head list;
	size_t icmp_len;
	struct batadv_icmp_packet_rr icmp_packet;
};

/* 
                                                                          
                                                                            
                                             
                                                             
                                                                    
                                                    
                                                                               
                                                                     
                                                                          
                                                                            
            
                                       
                                                   
                                                      
 */
#ifdef CONFIG_BATMAN_ADV_BLA
struct batadv_bla_backbone_gw {
	uint8_t orig[ETH_ALEN];
	short vid;
	struct hlist_node hash_entry;
	struct batadv_priv *bat_priv;
	unsigned long lasttime;
	atomic_t wait_periods;
	atomic_t request_sent;
	uint16_t crc;
	atomic_t refcount;
	struct rcu_head rcu;
};

/* 
                                                              
                                                
                                            
                                                                       
                                                       
                                                          
                                                   
                                                      
 */
struct batadv_bla_claim {
	uint8_t addr[ETH_ALEN];
	short vid;
	struct batadv_bla_backbone_gw *backbone_gw;
	unsigned long lasttime;
	struct hlist_node hash_entry;
	struct rcu_head rcu;
	atomic_t refcount;
};
#endif

/* 
                                                                   
                                        
                                                                
                               
                                                                    
                                                                
                                                   
                                                      
 */
struct batadv_tt_common_entry {
	uint8_t addr[ETH_ALEN];
	struct hlist_node hash_entry;
	uint16_t flags;
	unsigned long added_at;
	atomic_t refcount;
	struct rcu_head rcu;
};

/* 
                                                                    
                                          
                                                                
 */
struct batadv_tt_local_entry {
	struct batadv_tt_common_entry common;
	unsigned long last_seen;
};

/* 
                                                                      
                                          
                                                                 
                                        
                                                 
 */
struct batadv_tt_global_entry {
	struct batadv_tt_common_entry common;
	struct hlist_head orig_list;
	spinlock_t list_lock;	/*                    */
	unsigned long roam_at;
};

/* 
                                                                            
                                                                   
                                                                          
                                                         
                                                   
                                                      
 */
struct batadv_tt_orig_list_entry {
	struct batadv_orig_node *orig_node;
	uint8_t ttvn;
	struct hlist_node list;
	atomic_t refcount;
	struct rcu_head rcu;
};

/* 
                                                                  
                                                    
                                                        
 */
struct batadv_tt_change_node {
	struct list_head list;
	struct batadv_tt_change change;
};

/* 
                                                                              
                                                                        
                                                           
                                                
 */
struct batadv_tt_req_node {
	uint8_t addr[ETH_ALEN];
	unsigned long issued_at;
	struct list_head list;
};

/* 
                                                   
                                                        
                                                                        
                                                      
                                                                     
                                                 
 */
struct batadv_tt_roam_node {
	uint8_t addr[ETH_ALEN];
	atomic_t counter;
	unsigned long first_time;
	struct list_head list;
};

/* 
                                              
                                                     
                                
                                                      
                                                      
                                                        
                                                            
 */
struct batadv_nc_node {
	struct list_head list;
	uint8_t addr[ETH_ALEN];
	atomic_t refcount;
	struct rcu_head rcu;
	struct batadv_orig_node *orig_node;
	unsigned long last_seen;
};

/* 
                                              
                                                           
                                                      
                                                      
                                                       
                                                 
                                            
                                           
                                                     
 */
struct batadv_nc_path {
	struct hlist_node hash_entry;
	struct rcu_head rcu;
	atomic_t refcount;
	struct list_head packet_list;
	spinlock_t packet_list_lock; /*                      */
	uint8_t next_hop[ETH_ALEN];
	uint8_t prev_hop[ETH_ALEN];
	unsigned long last_valid;
};

/* 
                                                                       
                    
                                                     
                                         
                                                                          
                                                            
                                                      
                                                          
 */
struct batadv_nc_packet {
	struct list_head list;
	__be32 packet_id;
	unsigned long timestamp;
	struct batadv_neigh_node *neigh_node;
	struct sk_buff *skb;
	struct batadv_nc_path *nc_path;
};

/* 
                                                                               
                                                
                                                                               
                                     
 */
struct batadv_skb_cb {
	bool decoded;
};

/* 
                                                                               
                                                        
                                                               
                                                                              
            
                                  
                                                                   
                                                                   
                                                        
                                                             
                                                                        
                    
 */
struct batadv_forw_packet {
	struct hlist_node list;
	unsigned long send_time;
	uint8_t own;
	struct sk_buff *skb;
	uint16_t packet_len;
	uint32_t direct_link_flags;
	uint8_t num_packets;
	struct delayed_work delayed_work;
	struct batadv_hard_iface *if_incoming;
};

/* 
                                                                     
                                            
                                          
                              
 */
struct batadv_frag_packet_list_entry {
	struct list_head list;
	uint16_t seqno;
	struct sk_buff *skb;
};

/* 
                                                          
                                                                 
                                                                               
                                                                           
                            
                                              
                                                   
                                                    
                                                             
                                       
 */
struct batadv_vis_info {
	unsigned long first_seen;
	struct list_head recv_list;
	struct list_head send_list;
	struct kref refcount;
	struct hlist_node hash_entry;
	struct batadv_priv *bat_priv;
	struct sk_buff *skb_packet;
} __packed;

/* 
                                                                   
                                                            
                                                                           
                                                                      
 */
struct batadv_vis_info_entry {
	uint8_t  src[ETH_ALEN];
	uint8_t  dest[ETH_ALEN];
	uint8_t  quality;
} __packed;

/* 
                                                                              
                                                  
                                                                           
 */
struct batadv_vis_recvlist_node {
	struct list_head list;
	uint8_t mac[ETH_ALEN];
};

/* 
                                                                           
                                      
                                                            
                                      
  
                                                                
                                                                 
                    
 */
struct batadv_vis_if_list_entry {
	uint8_t addr[ETH_ALEN];
	bool primary;
	struct hlist_node list;
};

/* 
                                                    
                                            
                               
                                                                      
                                                                           
                                                                             
                                    
                                                                              
                                                                   
                                    
 */
struct batadv_algo_ops {
	struct hlist_node list;
	char *name;
	int (*bat_iface_enable)(struct batadv_hard_iface *hard_iface);
	void (*bat_iface_disable)(struct batadv_hard_iface *hard_iface);
	void (*bat_iface_update_mac)(struct batadv_hard_iface *hard_iface);
	void (*bat_primary_iface_set)(struct batadv_hard_iface *hard_iface);
	void (*bat_ogm_schedule)(struct batadv_hard_iface *hard_iface);
	void (*bat_ogm_emit)(struct batadv_forw_packet *forw_packet);
};

/* 
                                                                               
                                                                
                                                    
                                                           
                                                                        
                                                    
                                                   
                                                      
 */
struct batadv_dat_entry {
	__be32 ip;
	uint8_t mac_addr[ETH_ALEN];
	unsigned long last_update;
	struct hlist_node hash_entry;
	atomic_t refcount;
	struct rcu_head rcu;
};

/* 
                                                                         
                                                                          
                                     
                                
                                                                            
                                                 
 */
struct batadv_dat_candidate {
	int type;
	struct batadv_orig_node *orig_node;
};

#endif /*                          */
