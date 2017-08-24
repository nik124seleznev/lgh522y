/*
 * VLAN		An implementation of 802.1Q VLAN tagging.
 *
 * Authors:	Ben Greear <greearb@candelatech.com>
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 */
#ifndef _LINUX_IF_VLAN_H_
#define _LINUX_IF_VLAN_H_

#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/rtnetlink.h>
#include <linux/bug.h>
#include <uapi/linux/if_vlan.h>

#define VLAN_HLEN	4		/*                                      
                                            
      */
#define VLAN_ETH_HLEN	18		/*                          */
#define VLAN_ETH_ZLEN	64		/*                               */

/*
                                                                      
 */
#define VLAN_ETH_DATA_LEN	1500	/*                         */
#define VLAN_ETH_FRAME_LEN	1518	/*                               */

/*
                                 
                                     
                                                    
 */
struct vlan_hdr {
	__be16	h_vlan_TCI;
	__be16	h_vlan_encapsulated_proto;
};

/* 
                                                                
                                        
                                     
                                   
                                    
                                                    
 */
struct vlan_ethhdr {
	unsigned char	h_dest[ETH_ALEN];
	unsigned char	h_source[ETH_ALEN];
	__be16		h_vlan_proto;
	__be16		h_vlan_TCI;
	__be16		h_vlan_encapsulated_proto;
};

#include <linux/skbuff.h>

static inline struct vlan_ethhdr *vlan_eth_hdr(const struct sk_buff *skb)
{
	return (struct vlan_ethhdr *)skb_mac_header(skb);
}

#define VLAN_PRIO_MASK		0xe000 /*                     */
#define VLAN_PRIO_SHIFT		13
#define VLAN_CFI_MASK		0x1000 /*                            */
#define VLAN_TAG_PRESENT	VLAN_CFI_MASK
#define VLAN_VID_MASK		0x0fff /*                 */
#define VLAN_N_VID		4096

/*                   */
extern void vlan_ioctl_set(int (*hook)(struct net *, void __user *));

static inline int is_vlan_dev(struct net_device *dev)
{
        return dev->priv_flags & IFF_802_1Q_VLAN;
}

#define vlan_tx_tag_present(__skb)	((__skb)->vlan_tci & VLAN_TAG_PRESENT)
#define vlan_tx_tag_get(__skb)		((__skb)->vlan_tci & ~VLAN_TAG_PRESENT)
#define vlan_tx_tag_get_id(__skb)	((__skb)->vlan_tci & VLAN_VID_MASK)

#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)

extern struct net_device *__vlan_find_dev_deep(struct net_device *real_dev,
					       __be16 vlan_proto, u16 vlan_id);
extern struct net_device *vlan_dev_real_dev(const struct net_device *dev);
extern u16 vlan_dev_vlan_id(const struct net_device *dev);

extern bool vlan_do_receive(struct sk_buff **skb);
extern struct sk_buff *vlan_untag(struct sk_buff *skb);

extern int vlan_vid_add(struct net_device *dev, __be16 proto, u16 vid);
extern void vlan_vid_del(struct net_device *dev, __be16 proto, u16 vid);

extern int vlan_vids_add_by_dev(struct net_device *dev,
				const struct net_device *by_dev);
extern void vlan_vids_del_by_dev(struct net_device *dev,
				 const struct net_device *by_dev);

extern bool vlan_uses_dev(const struct net_device *dev);
#else
static inline struct net_device *
__vlan_find_dev_deep(struct net_device *real_dev,
		     __be16 vlan_proto, u16 vlan_id)
{
	return NULL;
}

static inline struct net_device *vlan_dev_real_dev(const struct net_device *dev)
{
	BUG();
	return NULL;
}

static inline u16 vlan_dev_vlan_id(const struct net_device *dev)
{
	BUG();
	return 0;
}

static inline bool vlan_do_receive(struct sk_buff **skb)
{
	return false;
}

static inline struct sk_buff *vlan_untag(struct sk_buff *skb)
{
	return skb;
}

static inline int vlan_vid_add(struct net_device *dev, __be16 proto, u16 vid)
{
	return 0;
}

static inline void vlan_vid_del(struct net_device *dev, __be16 proto, u16 vid)
{
}

static inline int vlan_vids_add_by_dev(struct net_device *dev,
				       const struct net_device *by_dev)
{
	return 0;
}

static inline void vlan_vids_del_by_dev(struct net_device *dev,
					const struct net_device *by_dev)
{
}

static inline bool vlan_uses_dev(const struct net_device *dev)
{
	return false;
}
#endif

static inline bool vlan_hw_offload_capable(netdev_features_t features,
					   __be16 proto)
{
	if (proto == htons(ETH_P_8021Q) && features & NETIF_F_HW_VLAN_CTAG_TX)
		return true;
	if (proto == htons(ETH_P_8021AD) && features & NETIF_F_HW_VLAN_STAG_TX)
		return true;
	return false;
}

/* 
                                               
                      
                                           
                                
  
                                                        
                                                                     
  
                                                                              
                                                        
  
                                                                             
 */
static inline struct sk_buff *vlan_insert_tag(struct sk_buff *skb,
					      __be16 vlan_proto, u16 vlan_tci)
{
	struct vlan_ethhdr *veth;

	if (skb_cow_head(skb, VLAN_HLEN) < 0) {
		kfree_skb(skb);
		return NULL;
	}
	veth = (struct vlan_ethhdr *)skb_push(skb, VLAN_HLEN);

	/*                                                            */
	memmove(skb->data, skb->data + VLAN_HLEN, 2 * ETH_ALEN);
	skb->mac_header -= VLAN_HLEN;

	/*                          */
	veth->h_vlan_proto = vlan_proto;

	/*              */
	veth->h_vlan_TCI = htons(vlan_tci);

	return skb;
}

/* 
                                              
                      
                                
  
                                                        
                                                                     
  
                                                                              
                                                        
 */
static inline struct sk_buff *__vlan_put_tag(struct sk_buff *skb,
					     __be16 vlan_proto, u16 vlan_tci)
{
	skb = vlan_insert_tag(skb, vlan_proto, vlan_tci);
	if (skb)
		skb->protocol = vlan_proto;
	return skb;
}

/* 
                                                               
                      
                                           
                                
  
                                                                      
 */
static inline struct sk_buff *__vlan_hwaccel_put_tag(struct sk_buff *skb,
						     __be16 vlan_proto,
						     u16 vlan_tci)
{
	skb->vlan_proto = vlan_proto;
	skb->vlan_tci = VLAN_TAG_PRESENT | vlan_tci;
	return skb;
}

#define HAVE_VLAN_PUT_TAG

/* 
                                                               
                      
                                
  
                                                            
                             
 */
static inline struct sk_buff *vlan_put_tag(struct sk_buff *skb,
					   __be16 vlan_proto, u16 vlan_tci)
{
	if (vlan_hw_offload_capable(skb->dev->features, vlan_proto)) {
		return __vlan_hwaccel_put_tag(skb, vlan_proto, vlan_tci);
	} else {
		return __vlan_put_tag(skb, vlan_proto, vlan_tci);
	}
}

/* 
                                                               
                        
                                   
  
                                               
 */
static inline int __vlan_get_tag(const struct sk_buff *skb, u16 *vlan_tci)
{
	struct vlan_ethhdr *veth = (struct vlan_ethhdr *)skb->data;

	if (veth->h_vlan_proto != htons(ETH_P_8021Q) &&
	    veth->h_vlan_proto != htons(ETH_P_8021AD))
		return -EINVAL;

	*vlan_tci = ntohs(veth->h_vlan_TCI);
	return 0;
}

/* 
                                                                 
                        
                                   
  
                                                       
 */
static inline int __vlan_hwaccel_get_tag(const struct sk_buff *skb,
					 u16 *vlan_tci)
{
	if (vlan_tx_tag_present(skb)) {
		*vlan_tci = vlan_tx_tag_get(skb);
		return 0;
	} else {
		*vlan_tci = 0;
		return -EINVAL;
	}
}

#define HAVE_VLAN_GET_TAG

/* 
                                              
                        
                                   
  
                                              
 */
static inline int vlan_get_tag(const struct sk_buff *skb, u16 *vlan_tci)
{
	if (skb->dev->features & NETIF_F_HW_VLAN_CTAG_TX) {
		return __vlan_hwaccel_get_tag(skb, vlan_tci);
	} else {
		return __vlan_get_tag(skb, vlan_tci);
	}
}

/* 
                                              
                        
  
                                                                   
                                                             
 */
static inline __be16 vlan_get_protocol(const struct sk_buff *skb)
{
	__be16 protocol = 0;

	if (vlan_tx_tag_present(skb) ||
	     skb->protocol != cpu_to_be16(ETH_P_8021Q))
		protocol = skb->protocol;
	else {
		__be16 proto, *protop;
		protop = skb_header_pointer(skb, offsetof(struct vlan_ethhdr,
						h_vlan_encapsulated_proto),
						sizeof(proto), &proto);
		if (likely(protop))
			protocol = *protop;
	}

	return protocol;
}

static inline void vlan_set_encap_proto(struct sk_buff *skb,
					struct vlan_hdr *vhdr)
{
	__be16 proto;
	unsigned short *rawp;

	/*
                                                                      
                               
  */

	proto = vhdr->h_vlan_encapsulated_proto;
	if (ntohs(proto) >= ETH_P_802_3_MIN) {
		skb->protocol = proto;
		return;
	}

	rawp = (unsigned short *)(vhdr + 1);
	if (*rawp == 0xFFFF)
		/*
                                                           
                                                               
                                                            
                                                                
                           
   */
		skb->protocol = htons(ETH_P_802_3);
	else
		/*
                   
   */
		skb->protocol = htons(ETH_P_802_2);
}
#endif /*                      */
