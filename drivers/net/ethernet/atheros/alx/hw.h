/*
 * Copyright (c) 2013 Johannes Berg <johannes@sipsolutions.net>
 *
 *  This file is free software: you may copy, redistribute and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * Copyright (c) 2012 Qualcomm Atheros, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef ALX_HW_H_
#define ALX_HW_H_
#include <linux/types.h>
#include <linux/mdio.h>
#include <linux/pci.h>
#include "reg.h"

/*                                                     
  
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
  
                                                       
  
                                                                 
                                                                     
                                                                         
                                                                          
                             
                                                                   
                                    
  
                         
  
                                             
                                             
                                             
                                                                 
                                                          
                                             
                                             
                                             
                                                                       
                                                                      
                                                                     
                                                                     
                                                                          
                                                                        
                                                            
                                                                
                                                            
                                                                    
                                              
                                              
                                              
                                              
                                              
                                                            
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                                              
 */
struct alx_txd {
	__le16 len;
	__le16 vlan_tag;
	__le32 word1;
	union {
		__le64 addr;
		struct {
			__le32 pkt_len;
			__le32 resvd;
		} l;
	} adrl;
} __packed;

/*            */
#define TPD_CXSUMSTART_MASK		0x00FF
#define TPD_CXSUMSTART_SHIFT		0
#define TPD_L4HDROFFSET_MASK		0x00FF
#define TPD_L4HDROFFSET_SHIFT		0
#define TPD_CXSUM_EN_MASK		0x0001
#define TPD_CXSUM_EN_SHIFT		8
#define TPD_IP_XSUM_MASK		0x0001
#define TPD_IP_XSUM_SHIFT		9
#define TPD_TCP_XSUM_MASK		0x0001
#define TPD_TCP_XSUM_SHIFT		10
#define TPD_UDP_XSUM_MASK		0x0001
#define TPD_UDP_XSUM_SHIFT		11
#define TPD_LSO_EN_MASK			0x0001
#define TPD_LSO_EN_SHIFT		12
#define TPD_LSO_V2_MASK			0x0001
#define TPD_LSO_V2_SHIFT		13
#define TPD_VLTAGGED_MASK		0x0001
#define TPD_VLTAGGED_SHIFT		14
#define TPD_INS_VLTAG_MASK		0x0001
#define TPD_INS_VLTAG_SHIFT		15
#define TPD_IPV4_MASK			0x0001
#define TPD_IPV4_SHIFT			16
#define TPD_ETHTYPE_MASK		0x0001
#define TPD_ETHTYPE_SHIFT		17
#define TPD_CXSUMOFFSET_MASK		0x00FF
#define TPD_CXSUMOFFSET_SHIFT		18
#define TPD_MSS_MASK			0x1FFF
#define TPD_MSS_SHIFT			18
#define TPD_EOP_MASK			0x0001
#define TPD_EOP_SHIFT			31

#define DESC_GET(_x, _name) ((_x) >> _name##SHIFT & _name##MASK)

/*                         */
struct alx_rfd {
	__le64 addr;		/*                               
                                     
                               
     */
} __packed;

/*                                                    
  
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
                                        
  
                                                          
  
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                                           
                                                          
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                              
                                                              
                                                           
                                              
                                              
                                              
                                                           
                                              
                                                                    
                                              
                                              
                                                                     
                                                           
                                              
  
                    
  
         
         
         
         
         
         
         
                                        
                        
         
         
         
         
         
                                     
                                
                        
         
                                
         
                                  
                           
                                  
                             
                        
                                                          
                             
                             
                                         
                          
                                                                            
                                                                    
 */
struct alx_rrd {
	__le32 word0;
	__le32 rss_hash;
	__le32 word2;
	__le32 word3;
} __packed;

/*            */
#define RRD_XSUM_MASK		0xFFFF
#define RRD_XSUM_SHIFT		0
#define RRD_NOR_MASK		0x000F
#define RRD_NOR_SHIFT		16
#define RRD_SI_MASK		0x0FFF
#define RRD_SI_SHIFT		20

/*            */
#define RRD_VLTAG_MASK		0xFFFF
#define RRD_VLTAG_SHIFT		0
#define RRD_PID_MASK		0x00FF
#define RRD_PID_SHIFT		16
/*               */
#define RRD_PID_NONIP		0
/*            */
#define RRD_PID_IPV4		1
/*          */
#define RRD_PID_IPV6TCP		2
/*          */
#define RRD_PID_IPV4TCP		3
/*          */
#define RRD_PID_IPV6UDP		4
/*          */
#define RRD_PID_IPV4UDP		5
/*            */
#define RRD_PID_IPV6		6
/*             */
#define RRD_PID_LLDP		7
/*             */
#define RRD_PID_1588		8
#define RRD_RSSQ_MASK		0x0007
#define RRD_RSSQ_SHIFT		25
#define RRD_RSSALG_MASK		0x000F
#define RRD_RSSALG_SHIFT	28
#define RRD_RSSALG_TCPV6	0x1
#define RRD_RSSALG_IPV6		0x2
#define RRD_RSSALG_TCPV4	0x4
#define RRD_RSSALG_IPV4		0x8

/*            */
#define RRD_PKTLEN_MASK		0x3FFF
#define RRD_PKTLEN_SHIFT	0
#define RRD_ERR_L4_MASK		0x0001
#define RRD_ERR_L4_SHIFT	14
#define RRD_ERR_IPV4_MASK	0x0001
#define RRD_ERR_IPV4_SHIFT	15
#define RRD_VLTAGGED_MASK	0x0001
#define RRD_VLTAGGED_SHIFT	16
#define RRD_OLD_PID_MASK	0x0007
#define RRD_OLD_PID_SHIFT	17
#define RRD_ERR_RES_MASK	0x0001
#define RRD_ERR_RES_SHIFT	20
#define RRD_ERR_FCS_MASK	0x0001
#define RRD_ERR_FCS_SHIFT	21
#define RRD_ERR_FAE_MASK	0x0001
#define RRD_ERR_FAE_SHIFT	22
#define RRD_ERR_TRUNC_MASK	0x0001
#define RRD_ERR_TRUNC_SHIFT	23
#define RRD_ERR_RUNT_MASK	0x0001
#define RRD_ERR_RUNT_SHIFT	24
#define RRD_ERR_ICMP_MASK	0x0001
#define RRD_ERR_ICMP_SHIFT	25
#define RRD_BCAST_MASK		0x0001
#define RRD_BCAST_SHIFT		26
#define RRD_MCAST_MASK		0x0001
#define RRD_MCAST_SHIFT		27
#define RRD_ETHTYPE_MASK	0x0001
#define RRD_ETHTYPE_SHIFT	28
#define RRD_ERR_FIFOV_MASK	0x0001
#define RRD_ERR_FIFOV_SHIFT	29
#define RRD_ERR_LEN_MASK	0x0001
#define RRD_ERR_LEN_SHIFT	30
#define RRD_UPDATED_MASK	0x0001
#define RRD_UPDATED_SHIFT	31


#define ALX_MAX_SETUP_LNK_CYCLE	50

/*                 */
#define ALX_FC_RX		0x01
#define ALX_FC_TX		0x02
#define ALX_FC_ANEG		0x04

/*                   */
#define ALX_SLEEP_WOL_PHY	0x00000001
#define ALX_SLEEP_WOL_MAGIC	0x00000002
#define ALX_SLEEP_CIFS		0x00000004
#define ALX_SLEEP_ACTIVE	(ALX_SLEEP_WOL_PHY | \
				 ALX_SLEEP_WOL_MAGIC | \
				 ALX_SLEEP_CIFS)

/*                   */
#define ALX_RSS_HASH_TYPE_IPV4		0x1
#define ALX_RSS_HASH_TYPE_IPV4_TCP	0x2
#define ALX_RSS_HASH_TYPE_IPV6		0x4
#define ALX_RSS_HASH_TYPE_IPV6_TCP	0x8
#define ALX_RSS_HASH_TYPE_ALL		(ALX_RSS_HASH_TYPE_IPV4 | \
					 ALX_RSS_HASH_TYPE_IPV4_TCP | \
					 ALX_RSS_HASH_TYPE_IPV6 | \
					 ALX_RSS_HASH_TYPE_IPV6_TCP)
#define ALX_DEF_RXBUF_SIZE	1536
#define ALX_MAX_JUMBO_PKT_SIZE	(9*1024)
#define ALX_MAX_TSO_PKT_SIZE	(7*1024)
#define ALX_MAX_FRAME_SIZE	ALX_MAX_JUMBO_PKT_SIZE
#define ALX_MIN_FRAME_SIZE	68
#define ALX_RAW_MTU(_mtu)	(_mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN)

#define ALX_MAX_RX_QUEUES	8
#define ALX_MAX_TX_QUEUES	4
#define ALX_MAX_HANDLED_INTRS	5

#define ALX_ISR_MISC		(ALX_ISR_PCIE_LNKDOWN | \
				 ALX_ISR_DMAW | \
				 ALX_ISR_DMAR | \
				 ALX_ISR_SMB | \
				 ALX_ISR_MANU | \
				 ALX_ISR_TIMER)

#define ALX_ISR_FATAL		(ALX_ISR_PCIE_LNKDOWN | \
				 ALX_ISR_DMAW | ALX_ISR_DMAR)

#define ALX_ISR_ALERT		(ALX_ISR_RXF_OV | \
				 ALX_ISR_TXF_UR | \
				 ALX_ISR_RFD_UR)

#define ALX_ISR_ALL_QUEUES	(ALX_ISR_TX_Q0 | \
				 ALX_ISR_TX_Q1 | \
				 ALX_ISR_TX_Q2 | \
				 ALX_ISR_TX_Q3 | \
				 ALX_ISR_RX_Q0 | \
				 ALX_ISR_RX_Q1 | \
				 ALX_ISR_RX_Q2 | \
				 ALX_ISR_RX_Q3 | \
				 ALX_ISR_RX_Q4 | \
				 ALX_ISR_RX_Q5 | \
				 ALX_ISR_RX_Q6 | \
				 ALX_ISR_RX_Q7)

/*                                    */
#define ALX_MAX_MSIX_INTRS	16

#define ALX_GET_FIELD(_data, _field)					\
	(((_data) >> _field ## _SHIFT) & _field ## _MASK)

#define ALX_SET_FIELD(_data, _field, _value)	do {			\
		(_data) &= ~(_field ## _MASK << _field ## _SHIFT);	\
		(_data) |= ((_value) & _field ## _MASK) << _field ## _SHIFT;\
	} while (0)

struct alx_hw {
	struct pci_dev *pdev;
	u8 __iomem *hw_addr;

	/*                              */
	u8 mac_addr[ETH_ALEN];
	u8 perm_addr[ETH_ALEN];

	u16 mtu;
	u16 imt;
	u8 dma_chnl;
	u8 max_dma_chnl;
	/*                           */
	u32 ith_tpd;
	u32 rx_ctrl;
	u32 mc_hash[2];

	u32 smb_timer;
	/*                                                   */
	int link_speed;

	/*                                             */
	u32 adv_cfg;
	u8 flowctrl;

	u32 sleep_ctrl;

	spinlock_t mdio_lock;
	struct mdio_if_info mdio;
	u16 phy_id[2];

	/*                     */
	bool lnk_patch;
};

static inline int alx_hw_revision(struct alx_hw *hw)
{
	return hw->pdev->revision >> ALX_PCI_REVID_SHIFT;
}

static inline bool alx_hw_with_cr(struct alx_hw *hw)
{
	return hw->pdev->revision & 1;
}

static inline bool alx_hw_giga(struct alx_hw *hw)
{
	return hw->pdev->device & 1;
}

static inline void alx_write_mem8(struct alx_hw *hw, u32 reg, u8 val)
{
	writeb(val, hw->hw_addr + reg);
}

static inline void alx_write_mem16(struct alx_hw *hw, u32 reg, u16 val)
{
	writew(val, hw->hw_addr + reg);
}

static inline u16 alx_read_mem16(struct alx_hw *hw, u32 reg)
{
	return readw(hw->hw_addr + reg);
}

static inline void alx_write_mem32(struct alx_hw *hw, u32 reg, u32 val)
{
	writel(val, hw->hw_addr + reg);
}

static inline u32 alx_read_mem32(struct alx_hw *hw, u32 reg)
{
	return readl(hw->hw_addr + reg);
}

static inline void alx_post_write(struct alx_hw *hw)
{
	readl(hw->hw_addr);
}

int alx_get_perm_macaddr(struct alx_hw *hw, u8 *addr);
void alx_reset_phy(struct alx_hw *hw);
void alx_reset_pcie(struct alx_hw *hw);
void alx_enable_aspm(struct alx_hw *hw, bool l0s_en, bool l1_en);
int alx_setup_speed_duplex(struct alx_hw *hw, u32 ethadv, u8 flowctrl);
void alx_post_phy_link(struct alx_hw *hw);
int alx_pre_suspend(struct alx_hw *hw, int speed);
int alx_read_phy_reg(struct alx_hw *hw, u16 reg, u16 *phy_data);
int alx_write_phy_reg(struct alx_hw *hw, u16 reg, u16 phy_data);
int alx_read_phy_ext(struct alx_hw *hw, u8 dev, u16 reg, u16 *pdata);
int alx_write_phy_ext(struct alx_hw *hw, u8 dev, u16 reg, u16 data);
int alx_get_phy_link(struct alx_hw *hw, int *speed);
int alx_clear_phy_intr(struct alx_hw *hw);
int alx_config_wol(struct alx_hw *hw);
void alx_cfg_mac_flowcontrol(struct alx_hw *hw, u8 fc);
void alx_start_mac(struct alx_hw *hw);
int alx_reset_mac(struct alx_hw *hw);
void alx_set_macaddr(struct alx_hw *hw, const u8 *addr);
bool alx_phy_configured(struct alx_hw *hw);
void alx_configure_basic(struct alx_hw *hw);
void alx_disable_rss(struct alx_hw *hw);
int alx_select_powersaving_speed(struct alx_hw *hw, int *speed);
bool alx_get_phy_info(struct alx_hw *hw);

#endif
