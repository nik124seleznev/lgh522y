/*
 * Network device features.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#ifndef _LINUX_NETDEV_FEATURES_H
#define _LINUX_NETDEV_FEATURES_H

#include <linux/types.h>

typedef u64 netdev_features_t;

enum {
	NETIF_F_SG_BIT,			/*                    */
	NETIF_F_IP_CSUM_BIT,		/*                                 */
	__UNUSED_NETIF_F_1,
	NETIF_F_HW_CSUM_BIT,		/*                               */
	NETIF_F_IPV6_CSUM_BIT,		/*                                */
	NETIF_F_HIGHDMA_BIT,		/*                         */
	NETIF_F_FRAGLIST_BIT,		/*                    */
	NETIF_F_HW_VLAN_CTAG_TX_BIT,	/*                                    */
	NETIF_F_HW_VLAN_CTAG_RX_BIT,	/*                                   */
	NETIF_F_HW_VLAN_CTAG_FILTER_BIT,/*                                 */
	NETIF_F_VLAN_CHALLENGED_BIT,	/*                                   */
	NETIF_F_GSO_BIT,		/*                      */
	NETIF_F_LLTX_BIT,		/*                                  */
					/*                                */
	NETIF_F_NETNS_LOCAL_BIT,	/*                                    */
	NETIF_F_GRO_BIT,		/*                         */
	NETIF_F_LRO_BIT,		/*                       */

	/**/NETIF_F_GSO_SHIFT,		/*                                  */
	NETIF_F_TSO_BIT			/*                        */
		= NETIF_F_GSO_SHIFT,
	NETIF_F_UFO_BIT,		/*                         */
	NETIF_F_GSO_ROBUST_BIT,		/*                     */
	NETIF_F_TSO_ECN_BIT,		/*                     */
	NETIF_F_TSO6_BIT,		/*                        */
	NETIF_F_FSO_BIT,		/*                       */
	NETIF_F_GSO_GRE_BIT,		/*                  */
	NETIF_F_GSO_UDP_TUNNEL_BIT,	/*                         */
	/**/NETIF_F_GSO_LAST =		/*                        */
		NETIF_F_GSO_UDP_TUNNEL_BIT,

	NETIF_F_FCOE_CRC_BIT,		/*            */
	NETIF_F_SCTP_CSUM_BIT,		/*                       */
	NETIF_F_FCOE_MTU_BIT,		/*                                  */
	NETIF_F_NTUPLE_BIT,		/*                           */
	NETIF_F_RXHASH_BIT,		/*                         */
	NETIF_F_RXCSUM_BIT,		/*                              */
	NETIF_F_NOCACHE_COPY_BIT,	/*                           */
	NETIF_F_LOOPBACK_BIT,		/*                 */
	NETIF_F_RXFCS_BIT,		/*                            */
	NETIF_F_RXALL_BIT,		/*                            */
	NETIF_F_HW_VLAN_STAG_TX_BIT,	/*                                    */
	NETIF_F_HW_VLAN_STAG_RX_BIT,	/*                                   */
	NETIF_F_HW_VLAN_STAG_FILTER_BIT,/*                                 */

	/*
                                                           
                                                             
                                                             
                                                    
  */

	/**/NETDEV_FEATURE_COUNT
};

/*                             */
#define __NETIF_F_BIT(bit)	((netdev_features_t)1 << (bit))
#define __NETIF_F(name)		__NETIF_F_BIT(NETIF_F_##name##_BIT)

#define NETIF_F_FCOE_CRC	__NETIF_F(FCOE_CRC)
#define NETIF_F_FCOE_MTU	__NETIF_F(FCOE_MTU)
#define NETIF_F_FRAGLIST	__NETIF_F(FRAGLIST)
#define NETIF_F_FSO		__NETIF_F(FSO)
#define NETIF_F_GRO		__NETIF_F(GRO)
#define NETIF_F_GSO		__NETIF_F(GSO)
#define NETIF_F_GSO_ROBUST	__NETIF_F(GSO_ROBUST)
#define NETIF_F_HIGHDMA		__NETIF_F(HIGHDMA)
#define NETIF_F_HW_CSUM		__NETIF_F(HW_CSUM)
#define NETIF_F_HW_VLAN_CTAG_FILTER __NETIF_F(HW_VLAN_CTAG_FILTER)
#define NETIF_F_HW_VLAN_CTAG_RX	__NETIF_F(HW_VLAN_CTAG_RX)
#define NETIF_F_HW_VLAN_CTAG_TX	__NETIF_F(HW_VLAN_CTAG_TX)
#define NETIF_F_IP_CSUM		__NETIF_F(IP_CSUM)
#define NETIF_F_IPV6_CSUM	__NETIF_F(IPV6_CSUM)
#define NETIF_F_LLTX		__NETIF_F(LLTX)
#define NETIF_F_LOOPBACK	__NETIF_F(LOOPBACK)
#define NETIF_F_LRO		__NETIF_F(LRO)
#define NETIF_F_NETNS_LOCAL	__NETIF_F(NETNS_LOCAL)
#define NETIF_F_NOCACHE_COPY	__NETIF_F(NOCACHE_COPY)
#define NETIF_F_NTUPLE		__NETIF_F(NTUPLE)
#define NETIF_F_RXCSUM		__NETIF_F(RXCSUM)
#define NETIF_F_RXHASH		__NETIF_F(RXHASH)
#define NETIF_F_SCTP_CSUM	__NETIF_F(SCTP_CSUM)
#define NETIF_F_SG		__NETIF_F(SG)
#define NETIF_F_TSO6		__NETIF_F(TSO6)
#define NETIF_F_TSO_ECN		__NETIF_F(TSO_ECN)
#define NETIF_F_TSO		__NETIF_F(TSO)
#define NETIF_F_UFO		__NETIF_F(UFO)
#define NETIF_F_VLAN_CHALLENGED	__NETIF_F(VLAN_CHALLENGED)
#define NETIF_F_RXFCS		__NETIF_F(RXFCS)
#define NETIF_F_RXALL		__NETIF_F(RXALL)
#define NETIF_F_GSO_GRE		__NETIF_F(GSO_GRE)
#define NETIF_F_GSO_UDP_TUNNEL	__NETIF_F(GSO_UDP_TUNNEL)
#define NETIF_F_HW_VLAN_STAG_FILTER __NETIF_F(HW_VLAN_STAG_FILTER)
#define NETIF_F_HW_VLAN_STAG_RX	__NETIF_F(HW_VLAN_STAG_RX)
#define NETIF_F_HW_VLAN_STAG_TX	__NETIF_F(HW_VLAN_STAG_TX)

/*                                      */
/*                                                 */
#define NETIF_F_NEVER_CHANGE	(NETIF_F_VLAN_CHALLENGED | \
				 NETIF_F_LLTX | NETIF_F_NETNS_LOCAL)

/*                                                    */
#define NETIF_F_ETHTOOL_BITS	((__NETIF_F_BIT(NETDEV_FEATURE_COUNT - 1) | \
		(__NETIF_F_BIT(NETDEV_FEATURE_COUNT - 1) - 1)) & \
		~NETIF_F_NEVER_CHANGE)

/*                                   */
#define NETIF_F_GSO_MASK	(__NETIF_F_BIT(NETIF_F_GSO_LAST + 1) - \
		__NETIF_F_BIT(NETIF_F_GSO_SHIFT))

/*                                           */
#define NETIF_F_GSO_SOFTWARE	(NETIF_F_TSO | NETIF_F_TSO_ECN | \
				 NETIF_F_TSO6 | NETIF_F_UFO)

#define NETIF_F_GEN_CSUM	NETIF_F_HW_CSUM
#define NETIF_F_V4_CSUM		(NETIF_F_GEN_CSUM | NETIF_F_IP_CSUM)
#define NETIF_F_V6_CSUM		(NETIF_F_GEN_CSUM | NETIF_F_IPV6_CSUM)
#define NETIF_F_ALL_CSUM	(NETIF_F_V4_CSUM | NETIF_F_V6_CSUM)

#define NETIF_F_ALL_TSO 	(NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_TSO_ECN)

#define NETIF_F_ALL_FCOE	(NETIF_F_FCOE_CRC | NETIF_F_FCOE_MTU | \
				 NETIF_F_FSO)

/*
                                                                 
                                        
 */
#define NETIF_F_ONE_FOR_ALL	(NETIF_F_GSO_SOFTWARE | NETIF_F_GSO_ROBUST | \
				 NETIF_F_SG | NETIF_F_HIGHDMA |		\
				 NETIF_F_FRAGLIST | NETIF_F_VLAN_CHALLENGED)
/*
                                                                       
                                        
 */
#define NETIF_F_ALL_FOR_ALL	(NETIF_F_NOCACHE_COPY | NETIF_F_FSO)

/*                                                           */
#define NETIF_F_SOFT_FEATURES	(NETIF_F_GSO | NETIF_F_GRO)

#endif	/*                          */