/*
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

#ifndef WIL6210_TXRX_H
#define WIL6210_TXRX_H

#define BUF_SW_OWNED    (1)
#define BUF_HW_OWNED    (0)

/*                        */
#define RX_BUF_LEN      (2048)
#define TX_BUF_LEN      (2048)
/*                                            */
#define WIL6210_RTAP_SIZE (128)

/*            */
/*
                           
            
                                        
                             
                           
                               
                                     
                                   
                           
                           
                        
                           
                           
                              
            
                          
                             
                           
                           
                           
                               
                           
                              
                            
                             
                           
                              
            
                                    
                           
                                     
                                       
                                 
                            
            
                            
 */
struct vring_tx_mac {
	u32 d[3];
	u32 ucode_cmd;
} __packed;

/*                */
#define MAC_CFG_DESC_TX_0_LIFETIME_EXPIRY_VALUE_POS 0
#define MAC_CFG_DESC_TX_0_LIFETIME_EXPIRY_VALUE_LEN 10
#define MAC_CFG_DESC_TX_0_LIFETIME_EXPIRY_VALUE_MSK 0x3FF

#define MAC_CFG_DESC_TX_0_INTERRUP_EN_POS 10
#define MAC_CFG_DESC_TX_0_INTERRUP_EN_LEN 1
#define MAC_CFG_DESC_TX_0_INTERRUP_EN_MSK 0x400

#define MAC_CFG_DESC_TX_0_STATUS_EN_POS 11
#define MAC_CFG_DESC_TX_0_STATUS_EN_LEN 1
#define MAC_CFG_DESC_TX_0_STATUS_EN_MSK 0x800

#define MAC_CFG_DESC_TX_0_TXSS_OVERRIDE_POS 12
#define MAC_CFG_DESC_TX_0_TXSS_OVERRIDE_LEN 2
#define MAC_CFG_DESC_TX_0_TXSS_OVERRIDE_MSK 0x3000

#define MAC_CFG_DESC_TX_0_TIMESTAMP_INSERTION_POS 14
#define MAC_CFG_DESC_TX_0_TIMESTAMP_INSERTION_LEN 1
#define MAC_CFG_DESC_TX_0_TIMESTAMP_INSERTION_MSK 0x4000

#define MAC_CFG_DESC_TX_0_DURATION_PRESERVE_POS 15
#define MAC_CFG_DESC_TX_0_DURATION_PRESERVE_LEN 1
#define MAC_CFG_DESC_TX_0_DURATION_PRESERVE_MSK 0x8000

#define MAC_CFG_DESC_TX_0_MCS_INDEX_POS 22
#define MAC_CFG_DESC_TX_0_MCS_INDEX_LEN 5
#define MAC_CFG_DESC_TX_0_MCS_INDEX_MSK 0x7C00000

#define MAC_CFG_DESC_TX_0_MCS_EN_POS 27
#define MAC_CFG_DESC_TX_0_MCS_EN_LEN 1
#define MAC_CFG_DESC_TX_0_MCS_EN_MSK 0x8000000

#define MAC_CFG_DESC_TX_0_SN_PRESERVED_POS 31
#define MAC_CFG_DESC_TX_0_SN_PRESERVED_LEN 1
#define MAC_CFG_DESC_TX_0_SN_PRESERVED_MSK 0x80000000

/*                */
#define MAC_CFG_DESC_TX_1_PKT_MODE_POS 0
#define MAC_CFG_DESC_TX_1_PKT_MODE_LEN 4
#define MAC_CFG_DESC_TX_1_PKT_MODE_MSK 0xF

#define MAC_CFG_DESC_TX_1_PKT_MODE_EN_POS 4
#define MAC_CFG_DESC_TX_1_PKT_MODE_EN_LEN 1
#define MAC_CFG_DESC_TX_1_PKT_MODE_EN_MSK 0x10

#define MAC_CFG_DESC_TX_1_ACK_POLICY_EN_POS 15
#define MAC_CFG_DESC_TX_1_ACK_POLICY_EN_LEN 1
#define MAC_CFG_DESC_TX_1_ACK_POLICY_EN_MSK 0x8000

#define MAC_CFG_DESC_TX_1_DST_INDEX_POS 16
#define MAC_CFG_DESC_TX_1_DST_INDEX_LEN 4
#define MAC_CFG_DESC_TX_1_DST_INDEX_MSK 0xF0000

#define MAC_CFG_DESC_TX_1_DST_INDEX_EN_POS 20
#define MAC_CFG_DESC_TX_1_DST_INDEX_EN_LEN 1
#define MAC_CFG_DESC_TX_1_DST_INDEX_EN_MSK 0x100000

#define MAC_CFG_DESC_TX_1_ACK_POLICY_POS 21
#define MAC_CFG_DESC_TX_1_ACK_POLICY_LEN 2
#define MAC_CFG_DESC_TX_1_ACK_POLICY_MSK 0x600000

#define MAC_CFG_DESC_TX_1_LIFETIME_EN_POS 23
#define MAC_CFG_DESC_TX_1_LIFETIME_EN_LEN 1
#define MAC_CFG_DESC_TX_1_LIFETIME_EN_MSK 0x800000

#define MAC_CFG_DESC_TX_1_MAX_RETRY_POS 24
#define MAC_CFG_DESC_TX_1_MAX_RETRY_LEN 7
#define MAC_CFG_DESC_TX_1_MAX_RETRY_MSK 0x7F000000

#define MAC_CFG_DESC_TX_1_MAX_RETRY_EN_POS 31
#define MAC_CFG_DESC_TX_1_MAX_RETRY_EN_LEN 1
#define MAC_CFG_DESC_TX_1_MAX_RETRY_EN_MSK 0x80000000

/*                */
#define MAC_CFG_DESC_TX_2_NUM_OF_DESCRIPTORS_POS 0
#define MAC_CFG_DESC_TX_2_NUM_OF_DESCRIPTORS_LEN 8
#define MAC_CFG_DESC_TX_2_NUM_OF_DESCRIPTORS_MSK 0xFF

#define MAC_CFG_DESC_TX_2_RESERVED_POS 8
#define MAC_CFG_DESC_TX_2_RESERVED_LEN 10
#define MAC_CFG_DESC_TX_2_RESERVED_MSK 0x3FF00

#define MAC_CFG_DESC_TX_2_L2_TRANSLATION_TYPE_POS 18
#define MAC_CFG_DESC_TX_2_L2_TRANSLATION_TYPE_LEN 2
#define MAC_CFG_DESC_TX_2_L2_TRANSLATION_TYPE_MSK 0xC0000

#define MAC_CFG_DESC_TX_2_SNAP_HDR_INSERTION_EN_POS 20
#define MAC_CFG_DESC_TX_2_SNAP_HDR_INSERTION_EN_LEN 1
#define MAC_CFG_DESC_TX_2_SNAP_HDR_INSERTION_EN_MSK 0x100000

#define MAC_CFG_DESC_TX_2_VLAN_REMOVAL_EN_POS 21
#define MAC_CFG_DESC_TX_2_VLAN_REMOVAL_EN_LEN 1
#define MAC_CFG_DESC_TX_2_VLAN_REMOVAL_EN_MSK 0x200000

/*                */
#define MAC_CFG_DESC_TX_3_UCODE_CMD_POS 0
#define MAC_CFG_DESC_TX_3_UCODE_CMD_LEN 32
#define MAC_CFG_DESC_TX_3_UCODE_CMD_MSK 0xFFFFFFFF

/*                */
#define DMA_CFG_DESC_TX_0_L4_LENGTH_POS 0
#define DMA_CFG_DESC_TX_0_L4_LENGTH_LEN 8
#define DMA_CFG_DESC_TX_0_L4_LENGTH_MSK 0xFF

#define DMA_CFG_DESC_TX_0_CMD_EOP_POS 8
#define DMA_CFG_DESC_TX_0_CMD_EOP_LEN 1
#define DMA_CFG_DESC_TX_0_CMD_EOP_MSK 0x100

#define DMA_CFG_DESC_TX_0_CMD_DMA_IT_POS 10
#define DMA_CFG_DESC_TX_0_CMD_DMA_IT_LEN 1
#define DMA_CFG_DESC_TX_0_CMD_DMA_IT_MSK 0x400

#define DMA_CFG_DESC_TX_0_SEGMENT_BUF_DETAILS_POS 11
#define DMA_CFG_DESC_TX_0_SEGMENT_BUF_DETAILS_LEN 2
#define DMA_CFG_DESC_TX_0_SEGMENT_BUF_DETAILS_MSK 0x1800

#define DMA_CFG_DESC_TX_0_TCP_SEG_EN_POS 13
#define DMA_CFG_DESC_TX_0_TCP_SEG_EN_LEN 1
#define DMA_CFG_DESC_TX_0_TCP_SEG_EN_MSK 0x2000

#define DMA_CFG_DESC_TX_0_IPV4_CHECKSUM_EN_POS 14
#define DMA_CFG_DESC_TX_0_IPV4_CHECKSUM_EN_LEN 1
#define DMA_CFG_DESC_TX_0_IPV4_CHECKSUM_EN_MSK 0x4000

#define DMA_CFG_DESC_TX_0_TCP_UDP_CHECKSUM_EN_POS 15
#define DMA_CFG_DESC_TX_0_TCP_UDP_CHECKSUM_EN_LEN 1
#define DMA_CFG_DESC_TX_0_TCP_UDP_CHECKSUM_EN_MSK 0x8000

#define DMA_CFG_DESC_TX_0_QID_POS 16
#define DMA_CFG_DESC_TX_0_QID_LEN 5
#define DMA_CFG_DESC_TX_0_QID_MSK 0x1F0000

#define DMA_CFG_DESC_TX_0_PSEUDO_HEADER_CALC_EN_POS 21
#define DMA_CFG_DESC_TX_0_PSEUDO_HEADER_CALC_EN_LEN 1
#define DMA_CFG_DESC_TX_0_PSEUDO_HEADER_CALC_EN_MSK 0x200000

#define DMA_CFG_DESC_TX_0_L4_TYPE_POS 30
#define DMA_CFG_DESC_TX_0_L4_TYPE_LEN 2
#define DMA_CFG_DESC_TX_0_L4_TYPE_MSK 0xC0000000


#define TX_DMA_STATUS_DU         BIT(0)

struct vring_tx_dma {
	u32 d0;
	u32 addr_low;
	u16 addr_high;
	u8  ip_length;
	u8  b11;       /*                                */
	u8  error;     /*                            */
	u8  status;    /*                         */
	u16 length;
} __packed;

/*
                           
            
                                              
                                                                        
                                                                           
                          
                                                                     
                                                                  
                                                          
                                                                         
                                                                
                                           
            
                        
                        
                                
                          
                         
                                                 
                                              
                                                   
                           
                           
                          
                                                                   
                                               
                     
                         
                          
            
                                                                  
                                                                          
                                                      
                                                                 
                                              
                                                           
                                                               
                                   
                                            
            
                                              
 */
struct vring_rx_mac {
	u32 d0;
	u32 d1;
	u16 w4;
	u16 pn_15_0;
	u32 pn_47_16;
} __packed;

/*
                           
            
                                          
                          
                            
                          
                                  
                                                                         
            
                                                          
            
                                                            
                           
                            
                            
            
                   
                    
                    
                     
                       
                    
                               
                               
                                   
                          
                   
  
 */

#define RX_DMA_D0_CMD_DMA_IT     BIT(10)

#define RX_DMA_STATUS_DU         BIT(0)
#define RX_DMA_STATUS_ERROR      BIT(2)
#define RX_DMA_STATUS_PHY_INFO   BIT(6)

struct vring_rx_dma {
	u32 d0;
	u32 addr_low;
	u16 addr_high;
	u8  ip_length;
	u8  b11;
	u8  error;
	u8  status;
	u16 length;
} __packed;

struct vring_tx_desc {
	struct vring_tx_mac mac;
	struct vring_tx_dma dma;
} __packed;

struct vring_rx_desc {
	struct vring_rx_mac mac;
	struct vring_rx_dma dma;
} __packed;

union vring_desc {
	struct vring_tx_desc tx;
	struct vring_rx_desc rx;
} __packed;

static inline int wil_rxdesc_tid(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 0, 3);
}

static inline int wil_rxdesc_cid(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 4, 6);
}

static inline int wil_rxdesc_mid(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 8, 9);
}

static inline int wil_rxdesc_ftype(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 10, 11);
}

static inline int wil_rxdesc_subtype(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 12, 15);
}

static inline int wil_rxdesc_seq(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 16, 27);
}

static inline int wil_rxdesc_ext_subtype(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d0, 28, 31);
}

static inline int wil_rxdesc_ds_bits(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d1, 8, 9);
}

static inline int wil_rxdesc_mcs(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->mac.d1, 21, 24);
}

static inline int wil_rxdesc_phy_length(struct vring_rx_desc *d)
{
	return WIL_GET_BITS(d->dma.d0, 16, 29);
}

static inline struct vring_rx_desc *wil_skb_rxdesc(struct sk_buff *skb)
{
	return (void *)skb->cb;
}

#endif /*                */
