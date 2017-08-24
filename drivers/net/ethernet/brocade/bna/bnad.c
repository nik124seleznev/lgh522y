/*
 * Linux network driver for Brocade Converged Network Adapter.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (GPL) Version 2 as
 * published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */
/*
 * Copyright (c) 2005-2010 Brocade Communications Systems, Inc.
 * All rights reserved
 * www.brocade.com
 */
#include <linux/bitops.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/in.h>
#include <linux/ethtool.h>
#include <linux/if_vlan.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/prefetch.h>
#include <linux/module.h>

#include "bnad.h"
#include "bna.h"
#include "cna.h"

static DEFINE_MUTEX(bnad_fwimg_mutex);

/*
                
 */
static uint bnad_msix_disable;
module_param(bnad_msix_disable, uint, 0444);
MODULE_PARM_DESC(bnad_msix_disable, "Disable MSIX mode");

static uint bnad_ioc_auto_recover = 1;
module_param(bnad_ioc_auto_recover, uint, 0444);
MODULE_PARM_DESC(bnad_ioc_auto_recover, "Enable / Disable auto recovery");

static uint bna_debugfs_enable = 1;
module_param(bna_debugfs_enable, uint, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(bna_debugfs_enable, "Enables debugfs feature, default=1,"
		 " Range[false:0|true:1]");

/*
                   
 */
u32 bnad_rxqs_per_cq = 2;
static u32 bna_id;
static struct mutex bnad_list_mutex;
static LIST_HEAD(bnad_list);
static const u8 bnad_bcast_addr[] =  {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

/*
               
 */
#define BNAD_GET_MBOX_IRQ(_bnad)				\
	(((_bnad)->cfg_flags & BNAD_CF_MSIX) ?			\
	 ((_bnad)->msix_table[BNAD_MAILBOX_MSIX_INDEX].vector) : \
	 ((_bnad)->pcidev->irq))

#define BNAD_FILL_UNMAPQ_MEM_REQ(_res_info, _num, _size)	\
do {								\
	(_res_info)->res_type = BNA_RES_T_MEM;			\
	(_res_info)->res_u.mem_info.mem_type = BNA_MEM_T_KVA;	\
	(_res_info)->res_u.mem_info.num = (_num);		\
	(_res_info)->res_u.mem_info.len = (_size);		\
} while (0)

static void
bnad_add_to_list(struct bnad *bnad)
{
	mutex_lock(&bnad_list_mutex);
	list_add_tail(&bnad->list_entry, &bnad_list);
	bnad->id = bna_id++;
	mutex_unlock(&bnad_list_mutex);
}

static void
bnad_remove_from_list(struct bnad *bnad)
{
	mutex_lock(&bnad_list_mutex);
	list_del(&bnad->list_entry);
	mutex_unlock(&bnad_list_mutex);
}

/*
                                                        
 */
static void
bnad_cq_cleanup(struct bnad *bnad, struct bna_ccb *ccb)
{
	struct bna_cq_entry *cmpl;
	int i;

	for (i = 0; i < ccb->q_depth; i++) {
		cmpl = &((struct bna_cq_entry *)ccb->sw_q)[i];
		cmpl->valid = 0;
	}
}

/*                       */


/*                                                                */
static u32
bnad_tx_buff_unmap(struct bnad *bnad,
			      struct bnad_tx_unmap *unmap_q,
			      u32 q_depth, u32 index)
{
	struct bnad_tx_unmap *unmap;
	struct sk_buff *skb;
	int vector, nvecs;

	unmap = &unmap_q[index];
	nvecs = unmap->nvecs;

	skb = unmap->skb;
	unmap->skb = NULL;
	unmap->nvecs = 0;
	dma_unmap_single(&bnad->pcidev->dev,
		dma_unmap_addr(&unmap->vectors[0], dma_addr),
		skb_headlen(skb), DMA_TO_DEVICE);
	dma_unmap_addr_set(&unmap->vectors[0], dma_addr, 0);
	nvecs--;

	vector = 0;
	while (nvecs) {
		vector++;
		if (vector == BFI_TX_MAX_VECTORS_PER_WI) {
			vector = 0;
			BNA_QE_INDX_INC(index, q_depth);
			unmap = &unmap_q[index];
		}

		dma_unmap_page(&bnad->pcidev->dev,
			dma_unmap_addr(&unmap->vectors[vector], dma_addr),
			skb_shinfo(skb)->frags[nvecs].size, DMA_TO_DEVICE);
		dma_unmap_addr_set(&unmap->vectors[vector], dma_addr, 0);
		nvecs--;
	}

	BNA_QE_INDX_INC(index, q_depth);

	return index;
}

/*
                            
                                                  
                                  
 */
static void
bnad_txq_cleanup(struct bnad *bnad, struct bna_tcb *tcb)
{
	struct bnad_tx_unmap *unmap_q = tcb->unmap_q;
	struct sk_buff *skb;
	int i;

	for (i = 0; i < tcb->q_depth; i++) {
		skb = unmap_q[i].skb;
		if (!skb)
			continue;
		bnad_tx_buff_unmap(bnad, unmap_q, tcb->q_depth, i);

		dev_kfree_skb_any(skb);
	}
}

/*
                                                           
                                        
                          
 */
static u32
bnad_txcmpl_process(struct bnad *bnad, struct bna_tcb *tcb)
{
	u32 sent_packets = 0, sent_bytes = 0;
	u32 wis, unmap_wis, hw_cons, cons, q_depth;
	struct bnad_tx_unmap *unmap_q = tcb->unmap_q;
	struct bnad_tx_unmap *unmap;
	struct sk_buff *skb;

	/*                              */
	if (!test_bit(BNAD_TXQ_TX_STARTED, &tcb->flags))
		return 0;

	hw_cons = *(tcb->hw_consumer_index);
	cons = tcb->consumer_index;
	q_depth = tcb->q_depth;

	wis = BNA_Q_INDEX_CHANGE(cons, hw_cons, q_depth);
	BUG_ON(!(wis <= BNA_QE_IN_USE_CNT(tcb, tcb->q_depth)));

	while (wis) {
		unmap = &unmap_q[cons];

		skb = unmap->skb;

		sent_packets++;
		sent_bytes += skb->len;

		unmap_wis = BNA_TXQ_WI_NEEDED(unmap->nvecs);
		wis -= unmap_wis;

		cons = bnad_tx_buff_unmap(bnad, unmap_q, q_depth, cons);
		dev_kfree_skb_any(skb);
	}

	/*                           */
	tcb->consumer_index = hw_cons;

	tcb->txq->tx_packets += sent_packets;
	tcb->txq->tx_bytes += sent_bytes;

	return sent_packets;
}

static u32
bnad_tx_complete(struct bnad *bnad, struct bna_tcb *tcb)
{
	struct net_device *netdev = bnad->netdev;
	u32 sent = 0;

	if (test_and_set_bit(BNAD_TXQ_FREE_SENT, &tcb->flags))
		return 0;

	sent = bnad_txcmpl_process(bnad, tcb);
	if (sent) {
		if (netif_queue_stopped(netdev) &&
		    netif_carrier_ok(netdev) &&
		    BNA_QE_FREE_CNT(tcb, tcb->q_depth) >=
				    BNAD_NETIF_WAKE_THRESHOLD) {
			if (test_bit(BNAD_TXQ_TX_STARTED, &tcb->flags)) {
				netif_wake_queue(netdev);
				BNAD_UPDATE_CTR(bnad, netif_queue_wakeup);
			}
		}
	}

	if (likely(test_bit(BNAD_TXQ_TX_STARTED, &tcb->flags)))
		bna_ib_ack(tcb->i_dbell, sent);

	smp_mb__before_clear_bit();
	clear_bit(BNAD_TXQ_FREE_SENT, &tcb->flags);

	return sent;
}

/*                            */
static irqreturn_t
bnad_msix_tx(int irq, void *data)
{
	struct bna_tcb *tcb = (struct bna_tcb *)data;
	struct bnad *bnad = tcb->bnad;

	bnad_tx_complete(bnad, tcb);

	return IRQ_HANDLED;
}

static inline void
bnad_rxq_alloc_uninit(struct bnad *bnad, struct bna_rcb *rcb)
{
	struct bnad_rx_unmap_q *unmap_q = rcb->unmap_q;

	unmap_q->reuse_pi = -1;
	unmap_q->alloc_order = -1;
	unmap_q->map_size = 0;
	unmap_q->type = BNAD_RXBUF_NONE;
}

/*                                                              */
static int
bnad_rxq_alloc_init(struct bnad *bnad, struct bna_rcb *rcb)
{
	struct bnad_rx_unmap_q *unmap_q = rcb->unmap_q;
	int mtu, order;

	bnad_rxq_alloc_uninit(bnad, rcb);

	mtu = bna_enet_mtu_get(&bnad->bna.enet);
	order = get_order(mtu);

	if (bna_is_small_rxq(rcb->id)) {
		unmap_q->alloc_order = 0;
		unmap_q->map_size = rcb->rxq->buffer_size;
	} else {
		unmap_q->alloc_order = order;
		unmap_q->map_size =
			(rcb->rxq->buffer_size > 2048) ?
			PAGE_SIZE << order : 2048;
	}

	BUG_ON(((PAGE_SIZE << order) % unmap_q->map_size));

	unmap_q->type = BNAD_RXBUF_PAGE;

	return 0;
}

static inline void
bnad_rxq_cleanup_page(struct bnad *bnad, struct bnad_rx_unmap *unmap)
{
	if (!unmap->page)
		return;

	dma_unmap_page(&bnad->pcidev->dev,
			dma_unmap_addr(&unmap->vector, dma_addr),
			unmap->vector.len, DMA_FROM_DEVICE);
	put_page(unmap->page);
	unmap->page = NULL;
	dma_unmap_addr_set(&unmap->vector, dma_addr, 0);
	unmap->vector.len = 0;
}

static inline void
bnad_rxq_cleanup_skb(struct bnad *bnad, struct bnad_rx_unmap *unmap)
{
	if (!unmap->skb)
		return;

	dma_unmap_single(&bnad->pcidev->dev,
			dma_unmap_addr(&unmap->vector, dma_addr),
			unmap->vector.len, DMA_FROM_DEVICE);
	dev_kfree_skb_any(unmap->skb);
	unmap->skb = NULL;
	dma_unmap_addr_set(&unmap->vector, dma_addr, 0);
	unmap->vector.len = 0;
}

static void
bnad_rxq_cleanup(struct bnad *bnad, struct bna_rcb *rcb)
{
	struct bnad_rx_unmap_q *unmap_q = rcb->unmap_q;
	int i;

	for (i = 0; i < rcb->q_depth; i++) {
		struct bnad_rx_unmap *unmap = &unmap_q->unmap[i];

		if (BNAD_RXBUF_IS_PAGE(unmap_q->type))
			bnad_rxq_cleanup_page(bnad, unmap);
		else
			bnad_rxq_cleanup_skb(bnad, unmap);
	}
	bnad_rxq_alloc_uninit(bnad, rcb);
}

static u32
bnad_rxq_refill_page(struct bnad *bnad, struct bna_rcb *rcb, u32 nalloc)
{
	u32 alloced, prod, q_depth;
	struct bnad_rx_unmap_q *unmap_q = rcb->unmap_q;
	struct bnad_rx_unmap *unmap, *prev;
	struct bna_rxq_entry *rxent;
	struct page *page;
	u32 page_offset, alloc_size;
	dma_addr_t dma_addr;

	prod = rcb->producer_index;
	q_depth = rcb->q_depth;

	alloc_size = PAGE_SIZE << unmap_q->alloc_order;
	alloced = 0;

	while (nalloc--) {
		unmap = &unmap_q->unmap[prod];

		if (unmap_q->reuse_pi < 0) {
			page = alloc_pages(GFP_ATOMIC | __GFP_COMP,
					unmap_q->alloc_order);
			page_offset = 0;
		} else {
			prev = &unmap_q->unmap[unmap_q->reuse_pi];
			page = prev->page;
			page_offset = prev->page_offset + unmap_q->map_size;
			get_page(page);
		}

		if (unlikely(!page)) {
			BNAD_UPDATE_CTR(bnad, rxbuf_alloc_failed);
			rcb->rxq->rxbuf_alloc_failed++;
			goto finishing;
		}

		dma_addr = dma_map_page(&bnad->pcidev->dev, page, page_offset,
				unmap_q->map_size, DMA_FROM_DEVICE);

		unmap->page = page;
		unmap->page_offset = page_offset;
		dma_unmap_addr_set(&unmap->vector, dma_addr, dma_addr);
		unmap->vector.len = unmap_q->map_size;
		page_offset += unmap_q->map_size;

		if (page_offset < alloc_size)
			unmap_q->reuse_pi = prod;
		else
			unmap_q->reuse_pi = -1;

		rxent = &((struct bna_rxq_entry *)rcb->sw_q)[prod];
		BNA_SET_DMA_ADDR(dma_addr, &rxent->host_addr);
		BNA_QE_INDX_INC(prod, q_depth);
		alloced++;
	}

finishing:
	if (likely(alloced)) {
		rcb->producer_index = prod;
		smp_mb();
		if (likely(test_bit(BNAD_RXQ_POST_OK, &rcb->flags)))
			bna_rxq_prod_indx_doorbell(rcb);
	}

	return alloced;
}

static u32
bnad_rxq_refill_skb(struct bnad *bnad, struct bna_rcb *rcb, u32 nalloc)
{
	u32 alloced, prod, q_depth, buff_sz;
	struct bnad_rx_unmap_q *unmap_q = rcb->unmap_q;
	struct bnad_rx_unmap *unmap;
	struct bna_rxq_entry *rxent;
	struct sk_buff *skb;
	dma_addr_t dma_addr;

	buff_sz = rcb->rxq->buffer_size;
	prod = rcb->producer_index;
	q_depth = rcb->q_depth;

	alloced = 0;
	while (nalloc--) {
		unmap = &unmap_q->unmap[prod];

		skb = netdev_alloc_skb_ip_align(bnad->netdev, buff_sz);

		if (unlikely(!skb)) {
			BNAD_UPDATE_CTR(bnad, rxbuf_alloc_failed);
			rcb->rxq->rxbuf_alloc_failed++;
			goto finishing;
		}
		dma_addr = dma_map_single(&bnad->pcidev->dev, skb->data,
					  buff_sz, DMA_FROM_DEVICE);

		unmap->skb = skb;
		dma_unmap_addr_set(&unmap->vector, dma_addr, dma_addr);
		unmap->vector.len = buff_sz;

		rxent = &((struct bna_rxq_entry *)rcb->sw_q)[prod];
		BNA_SET_DMA_ADDR(dma_addr, &rxent->host_addr);
		BNA_QE_INDX_INC(prod, q_depth);
		alloced++;
	}

finishing:
	if (likely(alloced)) {
		rcb->producer_index = prod;
		smp_mb();
		if (likely(test_bit(BNAD_RXQ_POST_OK, &rcb->flags)))
			bna_rxq_prod_indx_doorbell(rcb);
	}

	return alloced;
}

static inline void
bnad_rxq_post(struct bnad *bnad, struct bna_rcb *rcb)
{
	struct bnad_rx_unmap_q *unmap_q = rcb->unmap_q;
	u32 to_alloc;

	to_alloc = BNA_QE_FREE_CNT(rcb, rcb->q_depth);
	if (!(to_alloc >> BNAD_RXQ_REFILL_THRESHOLD_SHIFT))
		return;

	if (BNAD_RXBUF_IS_PAGE(unmap_q->type))
		bnad_rxq_refill_page(bnad, rcb, to_alloc);
	else
		bnad_rxq_refill_skb(bnad, rcb, to_alloc);
}

#define flags_cksum_prot_mask (BNA_CQ_EF_IPV4 | BNA_CQ_EF_L3_CKSUM_OK | \
					BNA_CQ_EF_IPV6 | \
					BNA_CQ_EF_TCP | BNA_CQ_EF_UDP | \
					BNA_CQ_EF_L4_CKSUM_OK)

#define flags_tcp4 (BNA_CQ_EF_IPV4 | BNA_CQ_EF_L3_CKSUM_OK | \
				BNA_CQ_EF_TCP | BNA_CQ_EF_L4_CKSUM_OK)
#define flags_tcp6 (BNA_CQ_EF_IPV6 | \
				BNA_CQ_EF_TCP | BNA_CQ_EF_L4_CKSUM_OK)
#define flags_udp4 (BNA_CQ_EF_IPV4 | BNA_CQ_EF_L3_CKSUM_OK | \
				BNA_CQ_EF_UDP | BNA_CQ_EF_L4_CKSUM_OK)
#define flags_udp6 (BNA_CQ_EF_IPV6 | \
				BNA_CQ_EF_UDP | BNA_CQ_EF_L4_CKSUM_OK)

static inline struct sk_buff *
bnad_cq_prepare_skb(struct bnad_rx_ctrl *rx_ctrl,
		struct bnad_rx_unmap_q *unmap_q,
		struct bnad_rx_unmap *unmap,
		u32 length, u32 flags)
{
	struct bnad *bnad = rx_ctrl->bnad;
	struct sk_buff *skb;

	if (BNAD_RXBUF_IS_PAGE(unmap_q->type)) {
		skb = napi_get_frags(&rx_ctrl->napi);
		if (unlikely(!skb))
			return NULL;

		dma_unmap_page(&bnad->pcidev->dev,
				dma_unmap_addr(&unmap->vector, dma_addr),
				unmap->vector.len, DMA_FROM_DEVICE);
		skb_fill_page_desc(skb, skb_shinfo(skb)->nr_frags,
				unmap->page, unmap->page_offset, length);
		skb->len += length;
		skb->data_len += length;
		skb->truesize += length;

		unmap->page = NULL;
		unmap->vector.len = 0;

		return skb;
	}

	skb = unmap->skb;
	BUG_ON(!skb);

	dma_unmap_single(&bnad->pcidev->dev,
			dma_unmap_addr(&unmap->vector, dma_addr),
			unmap->vector.len, DMA_FROM_DEVICE);

	skb_put(skb, length);

	skb->protocol = eth_type_trans(skb, bnad->netdev);

	unmap->skb = NULL;
	unmap->vector.len = 0;
	return skb;
}

static u32
bnad_cq_process(struct bnad *bnad, struct bna_ccb *ccb, int budget)
{
	struct bna_cq_entry *cq, *cmpl;
	struct bna_rcb *rcb = NULL;
	struct bnad_rx_unmap_q *unmap_q;
	struct bnad_rx_unmap *unmap;
	struct sk_buff *skb;
	struct bna_pkt_rate *pkt_rt = &ccb->pkt_rate;
	struct bnad_rx_ctrl *rx_ctrl = ccb->ctrl;
	u32 packets = 0, length = 0, flags, masked_flags;

	prefetch(bnad->netdev);

	cq = ccb->sw_q;
	cmpl = &cq[ccb->producer_index];

	while (cmpl->valid && (packets < budget)) {
		packets++;
		flags = ntohl(cmpl->flags);
		length = ntohs(cmpl->length);
		BNA_UPDATE_PKT_CNT(pkt_rt, ntohs(cmpl->length));

		if (bna_is_small_rxq(cmpl->rxq_id))
			rcb = ccb->rcb[1];
		else
			rcb = ccb->rcb[0];

		unmap_q = rcb->unmap_q;
		unmap = &unmap_q->unmap[rcb->consumer_index];

		if (unlikely(flags & (BNA_CQ_EF_MAC_ERROR |
					BNA_CQ_EF_FCS_ERROR |
					BNA_CQ_EF_TOO_LONG))) {
			if (BNAD_RXBUF_IS_PAGE(unmap_q->type))
				bnad_rxq_cleanup_page(bnad, unmap);
			else
				bnad_rxq_cleanup_skb(bnad, unmap);

			rcb->rxq->rx_packets_with_error++;
			goto next;
		}

		skb = bnad_cq_prepare_skb(ccb->ctrl, unmap_q, unmap,
				length, flags);

		if (unlikely(!skb))
			break;

		masked_flags = flags & flags_cksum_prot_mask;

		if (likely
		    ((bnad->netdev->features & NETIF_F_RXCSUM) &&
		     ((masked_flags == flags_tcp4) ||
		      (masked_flags == flags_udp4) ||
		      (masked_flags == flags_tcp6) ||
		      (masked_flags == flags_udp6))))
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		else
			skb_checksum_none_assert(skb);

		rcb->rxq->rx_packets++;
		rcb->rxq->rx_bytes += length;

		if (flags & BNA_CQ_EF_VLAN)
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), ntohs(cmpl->vlan_tag));

		if (BNAD_RXBUF_IS_PAGE(unmap_q->type))
			napi_gro_frags(&rx_ctrl->napi);
		else
			netif_receive_skb(skb);

next:
		cmpl->valid = 0;
		BNA_QE_INDX_INC(rcb->consumer_index, rcb->q_depth);
		BNA_QE_INDX_INC(ccb->producer_index, ccb->q_depth);
		cmpl = &cq[ccb->producer_index];
	}

	napi_gro_flush(&rx_ctrl->napi, false);
	if (likely(test_bit(BNAD_RXQ_STARTED, &ccb->rcb[0]->flags)))
		bna_ib_ack_disable_irq(ccb->i_dbell, packets);

	bnad_rxq_post(bnad, ccb->rcb[0]);
	if (ccb->rcb[1])
		bnad_rxq_post(bnad, ccb->rcb[1]);

	return packets;
}

static void
bnad_netif_rx_schedule_poll(struct bnad *bnad, struct bna_ccb *ccb)
{
	struct bnad_rx_ctrl *rx_ctrl = (struct bnad_rx_ctrl *)(ccb->ctrl);
	struct napi_struct *napi = &rx_ctrl->napi;

	if (likely(napi_schedule_prep(napi))) {
		__napi_schedule(napi);
		rx_ctrl->rx_schedule++;
	}
}

/*                      */
static irqreturn_t
bnad_msix_rx(int irq, void *data)
{
	struct bna_ccb *ccb = (struct bna_ccb *)data;

	if (ccb) {
		((struct bnad_rx_ctrl *)(ccb->ctrl))->rx_intr_ctr++;
		bnad_netif_rx_schedule_poll(ccb->bnad, ccb);
	}

	return IRQ_HANDLED;
}

/*                    */

/*                         */
static irqreturn_t
bnad_msix_mbox_handler(int irq, void *data)
{
	u32 intr_status;
	unsigned long flags;
	struct bnad *bnad = (struct bnad *)data;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (unlikely(test_bit(BNAD_RF_MBOX_IRQ_DISABLED, &bnad->run_flags))) {
		spin_unlock_irqrestore(&bnad->bna_lock, flags);
		return IRQ_HANDLED;
	}

	bna_intr_status_get(&bnad->bna, intr_status);

	if (BNA_IS_MBOX_ERR_INTR(&bnad->bna, intr_status))
		bna_mbox_handler(&bnad->bna, intr_status);

	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	return IRQ_HANDLED;
}

static irqreturn_t
bnad_isr(int irq, void *data)
{
	int i, j;
	u32 intr_status;
	unsigned long flags;
	struct bnad *bnad = (struct bnad *)data;
	struct bnad_rx_info *rx_info;
	struct bnad_rx_ctrl *rx_ctrl;
	struct bna_tcb *tcb = NULL;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (unlikely(test_bit(BNAD_RF_MBOX_IRQ_DISABLED, &bnad->run_flags))) {
		spin_unlock_irqrestore(&bnad->bna_lock, flags);
		return IRQ_NONE;
	}

	bna_intr_status_get(&bnad->bna, intr_status);

	if (unlikely(!intr_status)) {
		spin_unlock_irqrestore(&bnad->bna_lock, flags);
		return IRQ_NONE;
	}

	if (BNA_IS_MBOX_ERR_INTR(&bnad->bna, intr_status))
		bna_mbox_handler(&bnad->bna, intr_status);

	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	if (!BNA_IS_INTX_DATA_INTR(intr_status))
		return IRQ_HANDLED;

	/*                         */
	/*               */
	for (i = 0; i < bnad->num_tx; i++) {
		for (j = 0; j < bnad->num_txq_per_tx; j++) {
			tcb = bnad->tx_info[i].tcb[j];
			if (tcb && test_bit(BNAD_TXQ_TX_STARTED, &tcb->flags))
				bnad_tx_complete(bnad, bnad->tx_info[i].tcb[j]);
		}
	}
	/*               */
	for (i = 0; i < bnad->num_rx; i++) {
		rx_info = &bnad->rx_info[i];
		if (!rx_info->rx)
			continue;
		for (j = 0; j < bnad->num_rxp_per_rx; j++) {
			rx_ctrl = &rx_info->rx_ctrl[j];
			if (rx_ctrl->ccb)
				bnad_netif_rx_schedule_poll(bnad,
							    rx_ctrl->ccb);
		}
	}
	return IRQ_HANDLED;
}

/*
                                         
                                                
 */
static void
bnad_enable_mbox_irq(struct bnad *bnad)
{
	clear_bit(BNAD_RF_MBOX_IRQ_DISABLED, &bnad->run_flags);

	BNAD_UPDATE_CTR(bnad, mbox_intr_enabled);
}

/*
                                           
                          
 */
static void
bnad_disable_mbox_irq(struct bnad *bnad)
{
	set_bit(BNAD_RF_MBOX_IRQ_DISABLED, &bnad->run_flags);

	BNAD_UPDATE_CTR(bnad, mbox_intr_disabled);
}

static void
bnad_set_netdev_perm_addr(struct bnad *bnad)
{
	struct net_device *netdev = bnad->netdev;

	memcpy(netdev->perm_addr, &bnad->perm_addr, netdev->addr_len);
	if (is_zero_ether_addr(netdev->dev_addr))
		memcpy(netdev->dev_addr, &bnad->perm_addr, netdev->addr_len);
}

/*                       */

/*           */
void
bnad_cb_mbox_intr_enable(struct bnad *bnad)
{
	bnad_enable_mbox_irq(bnad);
}

void
bnad_cb_mbox_intr_disable(struct bnad *bnad)
{
	bnad_disable_mbox_irq(bnad);
}

void
bnad_cb_ioceth_ready(struct bnad *bnad)
{
	bnad->bnad_completions.ioc_comp_status = BNA_CB_SUCCESS;
	complete(&bnad->bnad_completions.ioc_comp);
}

void
bnad_cb_ioceth_failed(struct bnad *bnad)
{
	bnad->bnad_completions.ioc_comp_status = BNA_CB_FAIL;
	complete(&bnad->bnad_completions.ioc_comp);
}

void
bnad_cb_ioceth_disabled(struct bnad *bnad)
{
	bnad->bnad_completions.ioc_comp_status = BNA_CB_SUCCESS;
	complete(&bnad->bnad_completions.ioc_comp);
}

static void
bnad_cb_enet_disabled(void *arg)
{
	struct bnad *bnad = (struct bnad *)arg;

	netif_carrier_off(bnad->netdev);
	complete(&bnad->bnad_completions.enet_comp);
}

void
bnad_cb_ethport_link_status(struct bnad *bnad,
			enum bna_link_status link_status)
{
	bool link_up = false;

	link_up = (link_status == BNA_LINK_UP) || (link_status == BNA_CEE_UP);

	if (link_status == BNA_CEE_UP) {
		if (!test_bit(BNAD_RF_CEE_RUNNING, &bnad->run_flags))
			BNAD_UPDATE_CTR(bnad, cee_toggle);
		set_bit(BNAD_RF_CEE_RUNNING, &bnad->run_flags);
	} else {
		if (test_bit(BNAD_RF_CEE_RUNNING, &bnad->run_flags))
			BNAD_UPDATE_CTR(bnad, cee_toggle);
		clear_bit(BNAD_RF_CEE_RUNNING, &bnad->run_flags);
	}

	if (link_up) {
		if (!netif_carrier_ok(bnad->netdev)) {
			uint tx_id, tcb_id;
			printk(KERN_WARNING "bna: %s link up\n",
				bnad->netdev->name);
			netif_carrier_on(bnad->netdev);
			BNAD_UPDATE_CTR(bnad, link_toggle);
			for (tx_id = 0; tx_id < bnad->num_tx; tx_id++) {
				for (tcb_id = 0; tcb_id < bnad->num_txq_per_tx;
				      tcb_id++) {
					struct bna_tcb *tcb =
					bnad->tx_info[tx_id].tcb[tcb_id];
					u32 txq_id;
					if (!tcb)
						continue;

					txq_id = tcb->id;

					if (test_bit(BNAD_TXQ_TX_STARTED,
						     &tcb->flags)) {
						/*
                           
                           */
						printk(KERN_INFO "bna: %s %d "
						      "TXQ_STARTED\n",
						       bnad->netdev->name,
						       txq_id);
						netif_wake_subqueue(
								bnad->netdev,
								txq_id);
						BNAD_UPDATE_CTR(bnad,
							netif_queue_wakeup);
					} else {
						netif_stop_subqueue(
								bnad->netdev,
								txq_id);
						BNAD_UPDATE_CTR(bnad,
							netif_queue_stop);
					}
				}
			}
		}
	} else {
		if (netif_carrier_ok(bnad->netdev)) {
			printk(KERN_WARNING "bna: %s link down\n",
				bnad->netdev->name);
			netif_carrier_off(bnad->netdev);
			BNAD_UPDATE_CTR(bnad, link_toggle);
		}
	}
}

static void
bnad_cb_tx_disabled(void *arg, struct bna_tx *tx)
{
	struct bnad *bnad = (struct bnad *)arg;

	complete(&bnad->bnad_completions.tx_comp);
}

static void
bnad_cb_tcb_setup(struct bnad *bnad, struct bna_tcb *tcb)
{
	struct bnad_tx_info *tx_info =
			(struct bnad_tx_info *)tcb->txq->tx->priv;

	tcb->priv = tcb;
	tx_info->tcb[tcb->id] = tcb;
}

static void
bnad_cb_tcb_destroy(struct bnad *bnad, struct bna_tcb *tcb)
{
	struct bnad_tx_info *tx_info =
			(struct bnad_tx_info *)tcb->txq->tx->priv;

	tx_info->tcb[tcb->id] = NULL;
	tcb->priv = NULL;
}

static void
bnad_cb_ccb_setup(struct bnad *bnad, struct bna_ccb *ccb)
{
	struct bnad_rx_info *rx_info =
			(struct bnad_rx_info *)ccb->cq->rx->priv;

	rx_info->rx_ctrl[ccb->id].ccb = ccb;
	ccb->ctrl = &rx_info->rx_ctrl[ccb->id];
}

static void
bnad_cb_ccb_destroy(struct bnad *bnad, struct bna_ccb *ccb)
{
	struct bnad_rx_info *rx_info =
			(struct bnad_rx_info *)ccb->cq->rx->priv;

	rx_info->rx_ctrl[ccb->id].ccb = NULL;
}

static void
bnad_cb_tx_stall(struct bnad *bnad, struct bna_tx *tx)
{
	struct bnad_tx_info *tx_info =
			(struct bnad_tx_info *)tx->priv;
	struct bna_tcb *tcb;
	u32 txq_id;
	int i;

	for (i = 0; i < BNAD_MAX_TXQ_PER_TX; i++) {
		tcb = tx_info->tcb[i];
		if (!tcb)
			continue;
		txq_id = tcb->id;
		clear_bit(BNAD_TXQ_TX_STARTED, &tcb->flags);
		netif_stop_subqueue(bnad->netdev, txq_id);
		printk(KERN_INFO "bna: %s %d TXQ_STOPPED\n",
			bnad->netdev->name, txq_id);
	}
}

static void
bnad_cb_tx_resume(struct bnad *bnad, struct bna_tx *tx)
{
	struct bnad_tx_info *tx_info = (struct bnad_tx_info *)tx->priv;
	struct bna_tcb *tcb;
	u32 txq_id;
	int i;

	for (i = 0; i < BNAD_MAX_TXQ_PER_TX; i++) {
		tcb = tx_info->tcb[i];
		if (!tcb)
			continue;
		txq_id = tcb->id;

		BUG_ON(test_bit(BNAD_TXQ_TX_STARTED, &tcb->flags));
		set_bit(BNAD_TXQ_TX_STARTED, &tcb->flags);
		BUG_ON(*(tcb->hw_consumer_index) != 0);

		if (netif_carrier_ok(bnad->netdev)) {
			printk(KERN_INFO "bna: %s %d TXQ_STARTED\n",
				bnad->netdev->name, txq_id);
			netif_wake_subqueue(bnad->netdev, txq_id);
			BNAD_UPDATE_CTR(bnad, netif_queue_wakeup);
		}
	}

	/*
                                                   
                                                      
               
  */
	if (is_zero_ether_addr(&bnad->perm_addr.mac[0])) {
		bna_enet_perm_mac_get(&bnad->bna.enet, &bnad->perm_addr);
		bnad_set_netdev_perm_addr(bnad);
	}
}

/*
                                                                     
 */
static void
bnad_tx_cleanup(struct delayed_work *work)
{
	struct bnad_tx_info *tx_info =
		container_of(work, struct bnad_tx_info, tx_cleanup_work);
	struct bnad *bnad = NULL;
	struct bna_tcb *tcb;
	unsigned long flags;
	u32 i, pending = 0;

	for (i = 0; i < BNAD_MAX_TXQ_PER_TX; i++) {
		tcb = tx_info->tcb[i];
		if (!tcb)
			continue;

		bnad = tcb->bnad;

		if (test_and_set_bit(BNAD_TXQ_FREE_SENT, &tcb->flags)) {
			pending++;
			continue;
		}

		bnad_txq_cleanup(bnad, tcb);

		smp_mb__before_clear_bit();
		clear_bit(BNAD_TXQ_FREE_SENT, &tcb->flags);
	}

	if (pending) {
		queue_delayed_work(bnad->work_q, &tx_info->tx_cleanup_work,
			msecs_to_jiffies(1));
		return;
	}

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_tx_cleanup_complete(tx_info->tx);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

static void
bnad_cb_tx_cleanup(struct bnad *bnad, struct bna_tx *tx)
{
	struct bnad_tx_info *tx_info = (struct bnad_tx_info *)tx->priv;
	struct bna_tcb *tcb;
	int i;

	for (i = 0; i < BNAD_MAX_TXQ_PER_TX; i++) {
		tcb = tx_info->tcb[i];
		if (!tcb)
			continue;
	}

	queue_delayed_work(bnad->work_q, &tx_info->tx_cleanup_work, 0);
}

static void
bnad_cb_rx_stall(struct bnad *bnad, struct bna_rx *rx)
{
	struct bnad_rx_info *rx_info = (struct bnad_rx_info *)rx->priv;
	struct bna_ccb *ccb;
	struct bnad_rx_ctrl *rx_ctrl;
	int i;

	for (i = 0; i < BNAD_MAX_RXP_PER_RX; i++) {
		rx_ctrl = &rx_info->rx_ctrl[i];
		ccb = rx_ctrl->ccb;
		if (!ccb)
			continue;

		clear_bit(BNAD_RXQ_POST_OK, &ccb->rcb[0]->flags);

		if (ccb->rcb[1])
			clear_bit(BNAD_RXQ_POST_OK, &ccb->rcb[1]->flags);
	}
}

/*
                                                                     
 */
static void
bnad_rx_cleanup(void *work)
{
	struct bnad_rx_info *rx_info =
		container_of(work, struct bnad_rx_info, rx_cleanup_work);
	struct bnad_rx_ctrl *rx_ctrl;
	struct bnad *bnad = NULL;
	unsigned long flags;
	u32 i;

	for (i = 0; i < BNAD_MAX_RXP_PER_RX; i++) {
		rx_ctrl = &rx_info->rx_ctrl[i];

		if (!rx_ctrl->ccb)
			continue;

		bnad = rx_ctrl->ccb->bnad;

		/*
                                          
                                         
   */
		napi_disable(&rx_ctrl->napi);

		bnad_cq_cleanup(bnad, rx_ctrl->ccb);
		bnad_rxq_cleanup(bnad, rx_ctrl->ccb->rcb[0]);
		if (rx_ctrl->ccb->rcb[1])
			bnad_rxq_cleanup(bnad, rx_ctrl->ccb->rcb[1]);
	}

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_rx_cleanup_complete(rx_info->rx);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

static void
bnad_cb_rx_cleanup(struct bnad *bnad, struct bna_rx *rx)
{
	struct bnad_rx_info *rx_info = (struct bnad_rx_info *)rx->priv;
	struct bna_ccb *ccb;
	struct bnad_rx_ctrl *rx_ctrl;
	int i;

	for (i = 0; i < BNAD_MAX_RXP_PER_RX; i++) {
		rx_ctrl = &rx_info->rx_ctrl[i];
		ccb = rx_ctrl->ccb;
		if (!ccb)
			continue;

		clear_bit(BNAD_RXQ_STARTED, &ccb->rcb[0]->flags);

		if (ccb->rcb[1])
			clear_bit(BNAD_RXQ_STARTED, &ccb->rcb[1]->flags);
	}

	queue_work(bnad->work_q, &rx_info->rx_cleanup_work);
}

static void
bnad_cb_rx_post(struct bnad *bnad, struct bna_rx *rx)
{
	struct bnad_rx_info *rx_info = (struct bnad_rx_info *)rx->priv;
	struct bna_ccb *ccb;
	struct bna_rcb *rcb;
	struct bnad_rx_ctrl *rx_ctrl;
	int i, j;

	for (i = 0; i < BNAD_MAX_RXP_PER_RX; i++) {
		rx_ctrl = &rx_info->rx_ctrl[i];
		ccb = rx_ctrl->ccb;
		if (!ccb)
			continue;

		napi_enable(&rx_ctrl->napi);

		for (j = 0; j < BNAD_MAX_RXQ_PER_RXP; j++) {
			rcb = ccb->rcb[j];
			if (!rcb)
				continue;

			bnad_rxq_alloc_init(bnad, rcb);
			set_bit(BNAD_RXQ_STARTED, &rcb->flags);
			set_bit(BNAD_RXQ_POST_OK, &rcb->flags);
			bnad_rxq_post(bnad, rcb);
		}
	}
}

static void
bnad_cb_rx_disabled(void *arg, struct bna_rx *rx)
{
	struct bnad *bnad = (struct bnad *)arg;

	complete(&bnad->bnad_completions.rx_comp);
}

static void
bnad_cb_rx_mcast_add(struct bnad *bnad, struct bna_rx *rx)
{
	bnad->bnad_completions.mcast_comp_status = BNA_CB_SUCCESS;
	complete(&bnad->bnad_completions.mcast_comp);
}

void
bnad_cb_stats_get(struct bnad *bnad, enum bna_cb_status status,
		       struct bna_stats *stats)
{
	if (status == BNA_CB_SUCCESS)
		BNAD_UPDATE_CTR(bnad, hw_stats_updates);

	if (!netif_running(bnad->netdev) ||
		!test_bit(BNAD_RF_STATS_TIMER_RUNNING, &bnad->run_flags))
		return;

	mod_timer(&bnad->stats_timer,
		  jiffies + msecs_to_jiffies(BNAD_STATS_TIMER_FREQ));
}

static void
bnad_cb_enet_mtu_set(struct bnad *bnad)
{
	bnad->bnad_completions.mtu_comp_status = BNA_CB_SUCCESS;
	complete(&bnad->bnad_completions.mtu_comp);
}

void
bnad_cb_completion(void *arg, enum bfa_status status)
{
	struct bnad_iocmd_comp *iocmd_comp =
			(struct bnad_iocmd_comp *)arg;

	iocmd_comp->comp_status = (u32) status;
	complete(&iocmd_comp->comp);
}

/*                                     */

static void
bnad_mem_free(struct bnad *bnad,
	      struct bna_mem_info *mem_info)
{
	int i;
	dma_addr_t dma_pa;

	if (mem_info->mdl == NULL)
		return;

	for (i = 0; i < mem_info->num; i++) {
		if (mem_info->mdl[i].kva != NULL) {
			if (mem_info->mem_type == BNA_MEM_T_DMA) {
				BNA_GET_DMA_ADDR(&(mem_info->mdl[i].dma),
						dma_pa);
				dma_free_coherent(&bnad->pcidev->dev,
						  mem_info->mdl[i].len,
						  mem_info->mdl[i].kva, dma_pa);
			} else
				kfree(mem_info->mdl[i].kva);
		}
	}
	kfree(mem_info->mdl);
	mem_info->mdl = NULL;
}

static int
bnad_mem_alloc(struct bnad *bnad,
	       struct bna_mem_info *mem_info)
{
	int i;
	dma_addr_t dma_pa;

	if ((mem_info->num == 0) || (mem_info->len == 0)) {
		mem_info->mdl = NULL;
		return 0;
	}

	mem_info->mdl = kcalloc(mem_info->num, sizeof(struct bna_mem_descr),
				GFP_KERNEL);
	if (mem_info->mdl == NULL)
		return -ENOMEM;

	if (mem_info->mem_type == BNA_MEM_T_DMA) {
		for (i = 0; i < mem_info->num; i++) {
			mem_info->mdl[i].len = mem_info->len;
			mem_info->mdl[i].kva =
				dma_alloc_coherent(&bnad->pcidev->dev,
						   mem_info->len, &dma_pa,
						   GFP_KERNEL);
			if (mem_info->mdl[i].kva == NULL)
				goto err_return;

			BNA_SET_DMA_ADDR(dma_pa,
					 &(mem_info->mdl[i].dma));
		}
	} else {
		for (i = 0; i < mem_info->num; i++) {
			mem_info->mdl[i].len = mem_info->len;
			mem_info->mdl[i].kva = kzalloc(mem_info->len,
							GFP_KERNEL);
			if (mem_info->mdl[i].kva == NULL)
				goto err_return;
		}
	}

	return 0;

err_return:
	bnad_mem_free(bnad, mem_info);
	return -ENOMEM;
}

/*                      */
static void
bnad_mbox_irq_free(struct bnad *bnad)
{
	int irq;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bnad_disable_mbox_irq(bnad);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	irq = BNAD_GET_MBOX_IRQ(bnad);
	free_irq(irq, bnad);
}

/*
                                                  
                                                            
           
 */
static int
bnad_mbox_irq_alloc(struct bnad *bnad)
{
	int		err = 0;
	unsigned long	irq_flags, flags;
	u32	irq;
	irq_handler_t	irq_handler;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (bnad->cfg_flags & BNAD_CF_MSIX) {
		irq_handler = (irq_handler_t)bnad_msix_mbox_handler;
		irq = bnad->msix_table[BNAD_MAILBOX_MSIX_INDEX].vector;
		irq_flags = 0;
	} else {
		irq_handler = (irq_handler_t)bnad_isr;
		irq = bnad->pcidev->irq;
		irq_flags = IRQF_SHARED;
	}

	spin_unlock_irqrestore(&bnad->bna_lock, flags);
	sprintf(bnad->mbox_irq_name, "%s", BNAD_NAME);

	/*
                                                          
                                                            
  */
	set_bit(BNAD_RF_MBOX_IRQ_DISABLED, &bnad->run_flags);

	BNAD_UPDATE_CTR(bnad, mbox_intr_disabled);

	err = request_irq(irq, irq_handler, irq_flags,
			  bnad->mbox_irq_name, bnad);

	return err;
}

static void
bnad_txrx_irq_free(struct bnad *bnad, struct bna_intr_info *intr_info)
{
	kfree(intr_info->idl);
	intr_info->idl = NULL;
}

/*                                                            */
static int
bnad_txrx_irq_alloc(struct bnad *bnad, enum bnad_intr_source src,
		    u32 txrx_id, struct bna_intr_info *intr_info)
{
	int i, vector_start = 0;
	u32 cfg_flags;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	cfg_flags = bnad->cfg_flags;
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	if (cfg_flags & BNAD_CF_MSIX) {
		intr_info->intr_type = BNA_INTR_T_MSIX;
		intr_info->idl = kcalloc(intr_info->num,
					sizeof(struct bna_intr_descr),
					GFP_KERNEL);
		if (!intr_info->idl)
			return -ENOMEM;

		switch (src) {
		case BNAD_INTR_TX:
			vector_start = BNAD_MAILBOX_MSIX_VECTORS + txrx_id;
			break;

		case BNAD_INTR_RX:
			vector_start = BNAD_MAILBOX_MSIX_VECTORS +
					(bnad->num_tx * bnad->num_txq_per_tx) +
					txrx_id;
			break;

		default:
			BUG();
		}

		for (i = 0; i < intr_info->num; i++)
			intr_info->idl[i].vector = vector_start + i;
	} else {
		intr_info->intr_type = BNA_INTR_T_INTX;
		intr_info->num = 1;
		intr_info->idl = kcalloc(intr_info->num,
					sizeof(struct bna_intr_descr),
					GFP_KERNEL);
		if (!intr_info->idl)
			return -ENOMEM;

		switch (src) {
		case BNAD_INTR_TX:
			intr_info->idl[0].vector = BNAD_INTX_TX_IB_BITMASK;
			break;

		case BNAD_INTR_RX:
			intr_info->idl[0].vector = BNAD_INTX_RX_IB_BITMASK;
			break;
		}
	}
	return 0;
}

/*                                     
                                                
 */
static void
bnad_tx_msix_unregister(struct bnad *bnad, struct bnad_tx_info *tx_info,
			int num_txqs)
{
	int i;
	int vector_num;

	for (i = 0; i < num_txqs; i++) {
		if (tx_info->tcb[i] == NULL)
			continue;

		vector_num = tx_info->tcb[i]->intr_vector;
		free_irq(bnad->msix_table[vector_num].vector, tx_info->tcb[i]);
	}
}

/*                                     
                                                                 
 */
static int
bnad_tx_msix_register(struct bnad *bnad, struct bnad_tx_info *tx_info,
			u32 tx_id, int num_txqs)
{
	int i;
	int err;
	int vector_num;

	for (i = 0; i < num_txqs; i++) {
		vector_num = tx_info->tcb[i]->intr_vector;
		sprintf(tx_info->tcb[i]->name, "%s TXQ %d", bnad->netdev->name,
				tx_id + tx_info->tcb[i]->id);
		err = request_irq(bnad->msix_table[vector_num].vector,
				  (irq_handler_t)bnad_msix_tx, 0,
				  tx_info->tcb[i]->name,
				  tx_info->tcb[i]);
		if (err)
			goto err_return;
	}

	return 0;

err_return:
	if (i > 0)
		bnad_tx_msix_unregister(bnad, tx_info, (i - 1));
	return -1;
}

/*                                     
                                                
 */
static void
bnad_rx_msix_unregister(struct bnad *bnad, struct bnad_rx_info *rx_info,
			int num_rxps)
{
	int i;
	int vector_num;

	for (i = 0; i < num_rxps; i++) {
		if (rx_info->rx_ctrl[i].ccb == NULL)
			continue;

		vector_num = rx_info->rx_ctrl[i].ccb->intr_vector;
		free_irq(bnad->msix_table[vector_num].vector,
			 rx_info->rx_ctrl[i].ccb);
	}
}

/*                                     
                                                                 
 */
static int
bnad_rx_msix_register(struct bnad *bnad, struct bnad_rx_info *rx_info,
			u32 rx_id, int num_rxps)
{
	int i;
	int err;
	int vector_num;

	for (i = 0; i < num_rxps; i++) {
		vector_num = rx_info->rx_ctrl[i].ccb->intr_vector;
		sprintf(rx_info->rx_ctrl[i].ccb->name, "%s CQ %d",
			bnad->netdev->name,
			rx_id + rx_info->rx_ctrl[i].ccb->id);
		err = request_irq(bnad->msix_table[vector_num].vector,
				  (irq_handler_t)bnad_msix_rx, 0,
				  rx_info->rx_ctrl[i].ccb->name,
				  rx_info->rx_ctrl[i].ccb);
		if (err)
			goto err_return;
	}

	return 0;

err_return:
	if (i > 0)
		bnad_rx_msix_unregister(bnad, rx_info, (i - 1));
	return -1;
}

/*                          */
static void
bnad_tx_res_free(struct bnad *bnad, struct bna_res_info *res_info)
{
	int i;

	for (i = 0; i < BNA_TX_RES_T_MAX; i++) {
		if (res_info[i].res_type == BNA_RES_T_MEM)
			bnad_mem_free(bnad, &res_info[i].res_u.mem_info);
		else if (res_info[i].res_type == BNA_RES_T_INTR)
			bnad_txrx_irq_free(bnad, &res_info[i].res_u.intr_info);
	}
}

/*                                                        */
static int
bnad_tx_res_alloc(struct bnad *bnad, struct bna_res_info *res_info,
		  u32 tx_id)
{
	int i, err = 0;

	for (i = 0; i < BNA_TX_RES_T_MAX; i++) {
		if (res_info[i].res_type == BNA_RES_T_MEM)
			err = bnad_mem_alloc(bnad,
					&res_info[i].res_u.mem_info);
		else if (res_info[i].res_type == BNA_RES_T_INTR)
			err = bnad_txrx_irq_alloc(bnad, BNAD_INTR_TX, tx_id,
					&res_info[i].res_u.intr_info);
		if (err)
			goto err_return;
	}
	return 0;

err_return:
	bnad_tx_res_free(bnad, res_info);
	return err;
}

/*                          */
static void
bnad_rx_res_free(struct bnad *bnad, struct bna_res_info *res_info)
{
	int i;

	for (i = 0; i < BNA_RX_RES_T_MAX; i++) {
		if (res_info[i].res_type == BNA_RES_T_MEM)
			bnad_mem_free(bnad, &res_info[i].res_u.mem_info);
		else if (res_info[i].res_type == BNA_RES_T_INTR)
			bnad_txrx_irq_free(bnad, &res_info[i].res_u.intr_info);
	}
}

/*                                                        */
static int
bnad_rx_res_alloc(struct bnad *bnad, struct bna_res_info *res_info,
		  uint rx_id)
{
	int i, err = 0;

	/*                                                    */
	for (i = 0; i < BNA_RX_RES_T_MAX; i++) {
		if (res_info[i].res_type == BNA_RES_T_MEM)
			err = bnad_mem_alloc(bnad,
					&res_info[i].res_u.mem_info);
		else if (res_info[i].res_type == BNA_RES_T_INTR)
			err = bnad_txrx_irq_alloc(bnad, BNAD_INTR_RX, rx_id,
					&res_info[i].res_u.intr_info);
		if (err)
			goto err_return;
	}
	return 0;

err_return:
	bnad_rx_res_free(bnad, res_info);
	return err;
}

/*                 */
/*              */
static void
bnad_ioc_timeout(unsigned long data)
{
	struct bnad *bnad = (struct bnad *)data;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bfa_nw_ioc_timeout((void *) &bnad->bna.ioceth.ioc);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

static void
bnad_ioc_hb_check(unsigned long data)
{
	struct bnad *bnad = (struct bnad *)data;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bfa_nw_ioc_hb_check((void *) &bnad->bna.ioceth.ioc);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

static void
bnad_iocpf_timeout(unsigned long data)
{
	struct bnad *bnad = (struct bnad *)data;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bfa_nw_iocpf_timeout((void *) &bnad->bna.ioceth.ioc);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

static void
bnad_iocpf_sem_timeout(unsigned long data)
{
	struct bnad *bnad = (struct bnad *)data;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bfa_nw_iocpf_sem_timeout((void *) &bnad->bna.ioceth.ioc);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

/*
                                                           
                                                             
                   
                       
                
                     
              
 */

/*                                       */
static void
bnad_dim_timeout(unsigned long data)
{
	struct bnad *bnad = (struct bnad *)data;
	struct bnad_rx_info *rx_info;
	struct bnad_rx_ctrl *rx_ctrl;
	int i, j;
	unsigned long flags;

	if (!netif_carrier_ok(bnad->netdev))
		return;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	for (i = 0; i < bnad->num_rx; i++) {
		rx_info = &bnad->rx_info[i];
		if (!rx_info->rx)
			continue;
		for (j = 0; j < bnad->num_rxp_per_rx; j++) {
			rx_ctrl = &rx_info->rx_ctrl[j];
			if (!rx_ctrl->ccb)
				continue;
			bna_rx_dim_update(rx_ctrl->ccb);
		}
	}

	/*                                                          */
	if (test_bit(BNAD_RF_DIM_TIMER_RUNNING, &bnad->run_flags))
		mod_timer(&bnad->dim_timer,
			  jiffies + msecs_to_jiffies(BNAD_DIM_TIMER_FREQ));
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

/*                      */
static void
bnad_stats_timeout(unsigned long data)
{
	struct bnad *bnad = (struct bnad *)data;
	unsigned long flags;

	if (!netif_running(bnad->netdev) ||
		!test_bit(BNAD_RF_STATS_TIMER_RUNNING, &bnad->run_flags))
		return;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_hw_stats_get(&bnad->bna);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

/*
                       
                                  
 */
void
bnad_dim_timer_start(struct bnad *bnad)
{
	if (bnad->cfg_flags & BNAD_CF_DIM_ENABLED &&
	    !test_bit(BNAD_RF_DIM_TIMER_RUNNING, &bnad->run_flags)) {
		setup_timer(&bnad->dim_timer, bnad_dim_timeout,
			    (unsigned long)bnad);
		set_bit(BNAD_RF_DIM_TIMER_RUNNING, &bnad->run_flags);
		mod_timer(&bnad->dim_timer,
			  jiffies + msecs_to_jiffies(BNAD_DIM_TIMER_FREQ));
	}
}

/*
                              
                                                 
 */
static void
bnad_stats_timer_start(struct bnad *bnad)
{
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (!test_and_set_bit(BNAD_RF_STATS_TIMER_RUNNING, &bnad->run_flags)) {
		setup_timer(&bnad->stats_timer, bnad_stats_timeout,
			    (unsigned long)bnad);
		mod_timer(&bnad->stats_timer,
			  jiffies + msecs_to_jiffies(BNAD_STATS_TIMER_FREQ));
	}
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

/*
                        
                                                 
 */
static void
bnad_stats_timer_stop(struct bnad *bnad)
{
	int to_del = 0;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (test_and_clear_bit(BNAD_RF_STATS_TIMER_RUNNING, &bnad->run_flags))
		to_del = 1;
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
	if (to_del)
		del_timer_sync(&bnad->stats_timer);
}

/*           */

static void
bnad_netdev_mc_list_get(struct net_device *netdev, u8 *mc_list)
{
	int i = 1; /*                               */
	struct netdev_hw_addr *mc_addr;

	netdev_for_each_mc_addr(mc_addr, netdev) {
		memcpy(&mc_list[i * ETH_ALEN], &mc_addr->addr[0],
							ETH_ALEN);
		i++;
	}
}

static int
bnad_napi_poll_rx(struct napi_struct *napi, int budget)
{
	struct bnad_rx_ctrl *rx_ctrl =
		container_of(napi, struct bnad_rx_ctrl, napi);
	struct bnad *bnad = rx_ctrl->bnad;
	int rcvd = 0;

	rx_ctrl->rx_poll_ctr++;

	if (!netif_carrier_ok(bnad->netdev))
		goto poll_exit;

	rcvd = bnad_cq_process(bnad, rx_ctrl->ccb, budget);
	if (rcvd >= budget)
		return rcvd;

poll_exit:
	napi_complete(napi);

	rx_ctrl->rx_complete++;

	if (rx_ctrl->ccb)
		bnad_enable_rx_irq_unsafe(rx_ctrl->ccb);

	return rcvd;
}

#define BNAD_NAPI_POLL_QUOTA		64
static void
bnad_napi_add(struct bnad *bnad, u32 rx_id)
{
	struct bnad_rx_ctrl *rx_ctrl;
	int i;

	/*                          */
	for (i = 0; i <	bnad->num_rxp_per_rx; i++) {
		rx_ctrl = &bnad->rx_info[rx_id].rx_ctrl[i];
		netif_napi_add(bnad->netdev, &rx_ctrl->napi,
			       bnad_napi_poll_rx, BNAD_NAPI_POLL_QUOTA);
	}
}

static void
bnad_napi_delete(struct bnad *bnad, u32 rx_id)
{
	int i;

	/*                                 */
	for (i = 0; i < bnad->num_rxp_per_rx; i++)
		netif_napi_del(&bnad->rx_info[rx_id].rx_ctrl[i].napi);
}

/*                                    */
void
bnad_destroy_tx(struct bnad *bnad, u32 tx_id)
{
	struct bnad_tx_info *tx_info = &bnad->tx_info[tx_id];
	struct bna_res_info *res_info = &bnad->tx_res_info[tx_id].res_info[0];
	unsigned long flags;

	if (!tx_info->tx)
		return;

	init_completion(&bnad->bnad_completions.tx_comp);
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_tx_disable(tx_info->tx, BNA_HARD_CLEANUP, bnad_cb_tx_disabled);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
	wait_for_completion(&bnad->bnad_completions.tx_comp);

	if (tx_info->tcb[0]->intr_type == BNA_INTR_T_MSIX)
		bnad_tx_msix_unregister(bnad, tx_info,
			bnad->num_txq_per_tx);

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_tx_destroy(tx_info->tx);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	tx_info->tx = NULL;
	tx_info->tx_id = 0;

	bnad_tx_res_free(bnad, res_info);
}

/*                                    */
int
bnad_setup_tx(struct bnad *bnad, u32 tx_id)
{
	int err;
	struct bnad_tx_info *tx_info = &bnad->tx_info[tx_id];
	struct bna_res_info *res_info = &bnad->tx_res_info[tx_id].res_info[0];
	struct bna_intr_info *intr_info =
			&res_info[BNA_TX_RES_INTR_T_TXCMPL].res_u.intr_info;
	struct bna_tx_config *tx_config = &bnad->tx_config[tx_id];
	static const struct bna_tx_event_cbfn tx_cbfn = {
		.tcb_setup_cbfn = bnad_cb_tcb_setup,
		.tcb_destroy_cbfn = bnad_cb_tcb_destroy,
		.tx_stall_cbfn = bnad_cb_tx_stall,
		.tx_resume_cbfn = bnad_cb_tx_resume,
		.tx_cleanup_cbfn = bnad_cb_tx_cleanup,
	};

	struct bna_tx *tx;
	unsigned long flags;

	tx_info->tx_id = tx_id;

	/*                                        */
	tx_config->num_txq = bnad->num_txq_per_tx;
	tx_config->txq_depth = bnad->txq_depth;
	tx_config->tx_type = BNA_TX_T_REGULAR;
	tx_config->coalescing_timeo = bnad->tx_coalescing_timeo;

	/*                                                  */
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_tx_res_req(bnad->num_txq_per_tx,
		bnad->txq_depth, res_info);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	/*                                  */
	BNAD_FILL_UNMAPQ_MEM_REQ(&res_info[BNA_TX_RES_MEM_T_UNMAPQ],
			bnad->num_txq_per_tx, (sizeof(struct bnad_tx_unmap) *
			bnad->txq_depth));

	/*                    */
	err = bnad_tx_res_alloc(bnad, res_info, tx_id);
	if (err)
		return err;

	/*                                                               */
	spin_lock_irqsave(&bnad->bna_lock, flags);
	tx = bna_tx_create(&bnad->bna, bnad, tx_config, &tx_cbfn, res_info,
			tx_info);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
	if (!tx)
		goto err_return;
	tx_info->tx = tx;

	INIT_DELAYED_WORK(&tx_info->tx_cleanup_work,
			(work_func_t)bnad_tx_cleanup);

	/*                                */
	if (intr_info->intr_type == BNA_INTR_T_MSIX) {
		err = bnad_tx_msix_register(bnad, tx_info,
			tx_id, bnad->num_txq_per_tx);
		if (err)
			goto err_return;
	}

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_tx_enable(tx);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	return 0;

err_return:
	bnad_tx_res_free(bnad, res_info);
	return err;
}

/*                                       */
/*                                */
static void
bnad_init_rx_config(struct bnad *bnad, struct bna_rx_config *rx_config)
{
	rx_config->rx_type = BNA_RX_T_REGULAR;
	rx_config->num_paths = bnad->num_rxp_per_rx;
	rx_config->coalescing_timeo = bnad->rx_coalescing_timeo;

	if (bnad->num_rxp_per_rx > 1) {
		rx_config->rss_status = BNA_STATUS_T_ENABLED;
		rx_config->rss_config.hash_type =
				(BFI_ENET_RSS_IPV6 |
				 BFI_ENET_RSS_IPV6_TCP |
				 BFI_ENET_RSS_IPV4 |
				 BFI_ENET_RSS_IPV4_TCP);
		rx_config->rss_config.hash_mask =
				bnad->num_rxp_per_rx - 1;
		get_random_bytes(rx_config->rss_config.toeplitz_hash_key,
			sizeof(rx_config->rss_config.toeplitz_hash_key));
	} else {
		rx_config->rss_status = BNA_STATUS_T_DISABLED;
		memset(&rx_config->rss_config, 0,
		       sizeof(rx_config->rss_config));
	}
	rx_config->rxp_type = BNA_RXP_SLR;
	rx_config->q_depth = bnad->rxq_depth;

	rx_config->small_buff_size = BFI_SMALL_RXBUF_SIZE;

	rx_config->vlan_strip_status = BNA_STATUS_T_ENABLED;
}

static void
bnad_rx_ctrl_init(struct bnad *bnad, u32 rx_id)
{
	struct bnad_rx_info *rx_info = &bnad->rx_info[rx_id];
	int i;

	for (i = 0; i < bnad->num_rxp_per_rx; i++)
		rx_info->rx_ctrl[i].bnad = bnad;
}

/*                                                */
void
bnad_destroy_rx(struct bnad *bnad, u32 rx_id)
{
	struct bnad_rx_info *rx_info = &bnad->rx_info[rx_id];
	struct bna_rx_config *rx_config = &bnad->rx_config[rx_id];
	struct bna_res_info *res_info = &bnad->rx_res_info[rx_id].res_info[0];
	unsigned long flags;
	int to_del = 0;

	if (!rx_info->rx)
		return;

	if (0 == rx_id) {
		spin_lock_irqsave(&bnad->bna_lock, flags);
		if (bnad->cfg_flags & BNAD_CF_DIM_ENABLED &&
		    test_bit(BNAD_RF_DIM_TIMER_RUNNING, &bnad->run_flags)) {
			clear_bit(BNAD_RF_DIM_TIMER_RUNNING, &bnad->run_flags);
			to_del = 1;
		}
		spin_unlock_irqrestore(&bnad->bna_lock, flags);
		if (to_del)
			del_timer_sync(&bnad->dim_timer);
	}

	init_completion(&bnad->bnad_completions.rx_comp);
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_rx_disable(rx_info->rx, BNA_HARD_CLEANUP, bnad_cb_rx_disabled);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
	wait_for_completion(&bnad->bnad_completions.rx_comp);

	if (rx_info->rx_ctrl[0].ccb->intr_type == BNA_INTR_T_MSIX)
		bnad_rx_msix_unregister(bnad, rx_info, rx_config->num_paths);

	bnad_napi_delete(bnad, rx_id);

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_rx_destroy(rx_info->rx);

	rx_info->rx = NULL;
	rx_info->rx_id = 0;
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	bnad_rx_res_free(bnad, res_info);
}

/*                                                */
int
bnad_setup_rx(struct bnad *bnad, u32 rx_id)
{
	int err;
	struct bnad_rx_info *rx_info = &bnad->rx_info[rx_id];
	struct bna_res_info *res_info = &bnad->rx_res_info[rx_id].res_info[0];
	struct bna_intr_info *intr_info =
			&res_info[BNA_RX_RES_T_INTR].res_u.intr_info;
	struct bna_rx_config *rx_config = &bnad->rx_config[rx_id];
	static const struct bna_rx_event_cbfn rx_cbfn = {
		.rcb_setup_cbfn = NULL,
		.rcb_destroy_cbfn = NULL,
		.ccb_setup_cbfn = bnad_cb_ccb_setup,
		.ccb_destroy_cbfn = bnad_cb_ccb_destroy,
		.rx_stall_cbfn = bnad_cb_rx_stall,
		.rx_cleanup_cbfn = bnad_cb_rx_cleanup,
		.rx_post_cbfn = bnad_cb_rx_post,
	};
	struct bna_rx *rx;
	unsigned long flags;

	rx_info->rx_id = rx_id;

	/*                                        */
	bnad_init_rx_config(bnad, rx_config);

	/*                                                  */
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_rx_res_req(rx_config, res_info);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	/*                                  */
	BNAD_FILL_UNMAPQ_MEM_REQ(&res_info[BNA_RX_RES_MEM_T_UNMAPQ],
			rx_config->num_paths +
			((rx_config->rxp_type == BNA_RXP_SINGLE) ?
			 0 : rx_config->num_paths),
			((bnad->rxq_depth * sizeof(struct bnad_rx_unmap)) +
			 sizeof(struct bnad_rx_unmap_q)));

	/*                   */
	err = bnad_rx_res_alloc(bnad, res_info, rx_id);
	if (err)
		return err;

	bnad_rx_ctrl_init(bnad, rx_id);

	/*                                                               */
	spin_lock_irqsave(&bnad->bna_lock, flags);
	rx = bna_rx_create(&bnad->bna, bnad, rx_config, &rx_cbfn, res_info,
			rx_info);
	if (!rx) {
		err = -ENOMEM;
		spin_unlock_irqrestore(&bnad->bna_lock, flags);
		goto err_return;
	}
	rx_info->rx = rx;
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	INIT_WORK(&rx_info->rx_cleanup_work,
			(work_func_t)(bnad_rx_cleanup));

	/*
                                                        
                                                           
  */
	bnad_napi_add(bnad, rx_id);

	/*                                */
	if (intr_info->intr_type == BNA_INTR_T_MSIX) {
		err = bnad_rx_msix_register(bnad, rx_info, rx_id,
						rx_config->num_paths);
		if (err)
			goto err_return;
	}

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (0 == rx_id) {
		/*                                            */
		if (bnad->cfg_flags & BNAD_CF_DIM_ENABLED)
			bna_rx_dim_reconfig(&bnad->bna, bna_napi_dim_vector);

		/*                                              */
		bna_rx_vlanfilter_enable(rx);

		/*                     */
		bnad_dim_timer_start(bnad);
	}

	bna_rx_enable(rx);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	return 0;

err_return:
	bnad_destroy_rx(bnad, rx_id);
	return err;
}

/*                                             */
void
bnad_tx_coalescing_timeo_set(struct bnad *bnad)
{
	struct bnad_tx_info *tx_info;

	tx_info = &bnad->tx_info[0];
	if (!tx_info->tx)
		return;

	bna_tx_coalescing_timeo_set(tx_info->tx, bnad->tx_coalescing_timeo);
}

/*                                             */
void
bnad_rx_coalescing_timeo_set(struct bnad *bnad)
{
	struct bnad_rx_info *rx_info;
	int	i;

	for (i = 0; i < bnad->num_rx; i++) {
		rx_info = &bnad->rx_info[i];
		if (!rx_info->rx)
			continue;
		bna_rx_coalescing_timeo_set(rx_info->rx,
				bnad->rx_coalescing_timeo);
	}
}

/*
                                  
 */
int
bnad_mac_addr_set_locked(struct bnad *bnad, u8 *mac_addr)
{
	int ret;

	if (!is_valid_ether_addr(mac_addr))
		return -EADDRNOTAVAIL;

	/*                                                      */
	if (!bnad->rx_info[0].rx)
		return 0;

	ret = bna_rx_ucast_set(bnad->rx_info[0].rx, mac_addr, NULL);
	if (ret != BNA_CB_SUCCESS)
		return -EADDRNOTAVAIL;

	return 0;
}

/*                                      */
int
bnad_enable_default_bcast(struct bnad *bnad)
{
	struct bnad_rx_info *rx_info = &bnad->rx_info[0];
	int ret;
	unsigned long flags;

	init_completion(&bnad->bnad_completions.mcast_comp);

	spin_lock_irqsave(&bnad->bna_lock, flags);
	ret = bna_rx_mcast_add(rx_info->rx, (u8 *)bnad_bcast_addr,
				bnad_cb_rx_mcast_add);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	if (ret == BNA_CB_SUCCESS)
		wait_for_completion(&bnad->bnad_completions.mcast_comp);
	else
		return -ENODEV;

	if (bnad->bnad_completions.mcast_comp_status != BNA_CB_SUCCESS)
		return -ENODEV;

	return 0;
}

/*                                                */
void
bnad_restore_vlans(struct bnad *bnad, u32 rx_id)
{
	u16 vid;
	unsigned long flags;

	for_each_set_bit(vid, bnad->active_vlans, VLAN_N_VID) {
		spin_lock_irqsave(&bnad->bna_lock, flags);
		bna_rx_vlan_add(bnad->rx_info[rx_id].rx, vid);
		spin_unlock_irqrestore(&bnad->bna_lock, flags);
	}
}

/*                      */
void
bnad_netdev_qstats_fill(struct bnad *bnad, struct rtnl_link_stats64 *stats)
{
	int i, j;

	for (i = 0; i < bnad->num_rx; i++) {
		for (j = 0; j < bnad->num_rxp_per_rx; j++) {
			if (bnad->rx_info[i].rx_ctrl[j].ccb) {
				stats->rx_packets += bnad->rx_info[i].
				rx_ctrl[j].ccb->rcb[0]->rxq->rx_packets;
				stats->rx_bytes += bnad->rx_info[i].
					rx_ctrl[j].ccb->rcb[0]->rxq->rx_bytes;
				if (bnad->rx_info[i].rx_ctrl[j].ccb->rcb[1] &&
					bnad->rx_info[i].rx_ctrl[j].ccb->
					rcb[1]->rxq) {
					stats->rx_packets +=
						bnad->rx_info[i].rx_ctrl[j].
						ccb->rcb[1]->rxq->rx_packets;
					stats->rx_bytes +=
						bnad->rx_info[i].rx_ctrl[j].
						ccb->rcb[1]->rxq->rx_bytes;
				}
			}
		}
	}
	for (i = 0; i < bnad->num_tx; i++) {
		for (j = 0; j < bnad->num_txq_per_tx; j++) {
			if (bnad->tx_info[i].tcb[j]) {
				stats->tx_packets +=
				bnad->tx_info[i].tcb[j]->txq->tx_packets;
				stats->tx_bytes +=
					bnad->tx_info[i].tcb[j]->txq->tx_bytes;
			}
		}
	}
}

/*
                                         
 */
void
bnad_netdev_hwstats_fill(struct bnad *bnad, struct rtnl_link_stats64 *stats)
{
	struct bfi_enet_stats_mac *mac_stats;
	u32 bmap;
	int i;

	mac_stats = &bnad->stats.bna_stats->hw_stats.mac_stats;
	stats->rx_errors =
		mac_stats->rx_fcs_error + mac_stats->rx_alignment_error +
		mac_stats->rx_frame_length_error + mac_stats->rx_code_error +
		mac_stats->rx_undersize;
	stats->tx_errors = mac_stats->tx_fcs_error +
					mac_stats->tx_undersize;
	stats->rx_dropped = mac_stats->rx_drop;
	stats->tx_dropped = mac_stats->tx_drop;
	stats->multicast = mac_stats->rx_multicast;
	stats->collisions = mac_stats->tx_total_collision;

	stats->rx_length_errors = mac_stats->rx_frame_length_error;

	/*                                  */

	stats->rx_crc_errors = mac_stats->rx_fcs_error;
	stats->rx_frame_errors = mac_stats->rx_alignment_error;
	/*                     */
	bmap = bna_rx_rid_mask(&bnad->bna);
	for (i = 0; bmap; i++) {
		if (bmap & 1) {
			stats->rx_fifo_errors +=
				bnad->stats.bna_stats->
					hw_stats.rxf_stats[i].frame_drops;
			break;
		}
		bmap >>= 1;
	}
}

static void
bnad_mbox_irq_sync(struct bnad *bnad)
{
	u32 irq;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (bnad->cfg_flags & BNAD_CF_MSIX)
		irq = bnad->msix_table[BNAD_MAILBOX_MSIX_INDEX].vector;
	else
		irq = bnad->pcidev->irq;
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	synchronize_irq(irq);
}

/*                                                */
static int
bnad_tso_prepare(struct bnad *bnad, struct sk_buff *skb)
{
	int err;

	if (skb_header_cloned(skb)) {
		err = pskb_expand_head(skb, 0, 0, GFP_ATOMIC);
		if (err) {
			BNAD_UPDATE_CTR(bnad, tso_err);
			return err;
		}
	}

	/*
                                                                    
                               
  */
	if (skb->protocol == htons(ETH_P_IP)) {
		struct iphdr *iph = ip_hdr(skb);

		/*                          */
		iph->tot_len = 0;
		iph->check = 0;

		tcp_hdr(skb)->check =
			~csum_tcpudp_magic(iph->saddr, iph->daddr, 0,
					   IPPROTO_TCP, 0);
		BNAD_UPDATE_CTR(bnad, tso4);
	} else {
		struct ipv6hdr *ipv6h = ipv6_hdr(skb);

		ipv6h->payload_len = 0;
		tcp_hdr(skb)->check =
			~csum_ipv6_magic(&ipv6h->saddr, &ipv6h->daddr, 0,
					 IPPROTO_TCP, 0);
		BNAD_UPDATE_CTR(bnad, tso6);
	}

	return 0;
}

/*
                                             
                                                        
          
 */
static void
bnad_q_num_init(struct bnad *bnad)
{
	int rxps;

	rxps = min((uint)num_online_cpus(),
			(uint)(BNAD_MAX_RX * BNAD_MAX_RXP_PER_RX));

	if (!(bnad->cfg_flags & BNAD_CF_MSIX))
		rxps = 1;	/*      */

	bnad->num_rx = 1;
	bnad->num_tx = 1;
	bnad->num_rxp_per_rx = rxps;
	bnad->num_txq_per_tx = BNAD_TXQ_NUM;
}

/*
                                                        
                                                           
                                  
                                                            
 */
static void
bnad_q_num_adjust(struct bnad *bnad, int msix_vectors, int temp)
{
	bnad->num_txq_per_tx = 1;
	if ((msix_vectors >= (bnad->num_tx * bnad->num_txq_per_tx)  +
	     bnad_rxqs_per_cq + BNAD_MAILBOX_MSIX_VECTORS) &&
	    (bnad->cfg_flags & BNAD_CF_MSIX)) {
		bnad->num_rxp_per_rx = msix_vectors -
			(bnad->num_tx * bnad->num_txq_per_tx) -
			BNAD_MAILBOX_MSIX_VECTORS;
	} else
		bnad->num_rxp_per_rx = 1;
}

/*                         */
static int
bnad_ioceth_disable(struct bnad *bnad)
{
	unsigned long flags;
	int err = 0;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	init_completion(&bnad->bnad_completions.ioc_comp);
	bna_ioceth_disable(&bnad->bna.ioceth, BNA_HARD_CLEANUP);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	wait_for_completion_timeout(&bnad->bnad_completions.ioc_comp,
		msecs_to_jiffies(BNAD_IOCETH_TIMEOUT));

	err = bnad->bnad_completions.ioc_comp_status;
	return err;
}

static int
bnad_ioceth_enable(struct bnad *bnad)
{
	int err = 0;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	init_completion(&bnad->bnad_completions.ioc_comp);
	bnad->bnad_completions.ioc_comp_status = BNA_CB_WAITING;
	bna_ioceth_enable(&bnad->bna.ioceth);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	wait_for_completion_timeout(&bnad->bnad_completions.ioc_comp,
		msecs_to_jiffies(BNAD_IOCETH_TIMEOUT));

	err = bnad->bnad_completions.ioc_comp_status;

	return err;
}

/*                    */
static void
bnad_res_free(struct bnad *bnad, struct bna_res_info *res_info,
		u32 res_val_max)
{
	int i;

	for (i = 0; i < res_val_max; i++)
		bnad_mem_free(bnad, &res_info[i].res_u.mem_info);
}

/*                                                  */
static int
bnad_res_alloc(struct bnad *bnad, struct bna_res_info *res_info,
		u32 res_val_max)
{
	int i, err;

	for (i = 0; i < res_val_max; i++) {
		err = bnad_mem_alloc(bnad, &res_info[i].res_u.mem_info);
		if (err)
			goto err_return;
	}
	return 0;

err_return:
	bnad_res_free(bnad, res_info, res_val_max);
	return err;
}

/*                            */
static void
bnad_enable_msix(struct bnad *bnad)
{
	int i, ret;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (!(bnad->cfg_flags & BNAD_CF_MSIX)) {
		spin_unlock_irqrestore(&bnad->bna_lock, flags);
		return;
	}
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	if (bnad->msix_table)
		return;

	bnad->msix_table =
		kcalloc(bnad->msix_num, sizeof(struct msix_entry), GFP_KERNEL);

	if (!bnad->msix_table)
		goto intx_mode;

	for (i = 0; i < bnad->msix_num; i++)
		bnad->msix_table[i].entry = i;

	ret = pci_enable_msix(bnad->pcidev, bnad->msix_table, bnad->msix_num);
	if (ret > 0) {
		/*                           */
		pr_warn("BNA: %d MSI-X vectors allocated < %d requested\n",
			ret, bnad->msix_num);

		spin_lock_irqsave(&bnad->bna_lock, flags);
		/*                               */
		bnad_q_num_adjust(bnad, (ret - BNAD_MAILBOX_MSIX_VECTORS) / 2,
			(ret - BNAD_MAILBOX_MSIX_VECTORS) / 2);
		spin_unlock_irqrestore(&bnad->bna_lock, flags);

		bnad->msix_num = BNAD_NUM_TXQ + BNAD_NUM_RXP +
			 BNAD_MAILBOX_MSIX_VECTORS;

		if (bnad->msix_num > ret)
			goto intx_mode;

		/*                                     */
		/*                                  */
		ret = pci_enable_msix(bnad->pcidev, bnad->msix_table,
				      bnad->msix_num);
		if (ret)
			goto intx_mode;

	} else if (ret < 0)
		goto intx_mode;

	pci_intx(bnad->pcidev, 0);

	return;

intx_mode:
	pr_warn("BNA: MSI-X enable failed - operating in INTx mode\n");

	kfree(bnad->msix_table);
	bnad->msix_table = NULL;
	bnad->msix_num = 0;
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bnad->cfg_flags &= ~BNAD_CF_MSIX;
	bnad_q_num_init(bnad);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

static void
bnad_disable_msix(struct bnad *bnad)
{
	u32 cfg_flags;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	cfg_flags = bnad->cfg_flags;
	if (bnad->cfg_flags & BNAD_CF_MSIX)
		bnad->cfg_flags &= ~BNAD_CF_MSIX;
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	if (cfg_flags & BNAD_CF_MSIX) {
		pci_disable_msix(bnad->pcidev);
		kfree(bnad->msix_table);
		bnad->msix_table = NULL;
	}
}

/*                     */
static int
bnad_open(struct net_device *netdev)
{
	int err;
	struct bnad *bnad = netdev_priv(netdev);
	struct bna_pause_config pause_config;
	int mtu;
	unsigned long flags;

	mutex_lock(&bnad->conf_mutex);

	/*    */
	err = bnad_setup_tx(bnad, 0);
	if (err)
		goto err_return;

	/*    */
	err = bnad_setup_rx(bnad, 0);
	if (err)
		goto cleanup_tx;

	/*      */
	pause_config.tx_pause = 0;
	pause_config.rx_pause = 0;

	mtu = ETH_HLEN + VLAN_HLEN + bnad->netdev->mtu + ETH_FCS_LEN;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_enet_mtu_set(&bnad->bna.enet, mtu, NULL);
	bna_enet_pause_config(&bnad->bna.enet, &pause_config, NULL);
	bna_enet_enable(&bnad->bna.enet);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	/*                  */
	bnad_enable_default_bcast(bnad);

	/*                       */
	bnad_restore_vlans(bnad, 0);

	/*                       */
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bnad_mac_addr_set_locked(bnad, netdev->dev_addr);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	/*                       */
	bnad_stats_timer_start(bnad);

	mutex_unlock(&bnad->conf_mutex);

	return 0;

cleanup_tx:
	bnad_destroy_tx(bnad, 0);

err_return:
	mutex_unlock(&bnad->conf_mutex);
	return err;
}

static int
bnad_stop(struct net_device *netdev)
{
	struct bnad *bnad = netdev_priv(netdev);
	unsigned long flags;

	mutex_lock(&bnad->conf_mutex);

	/*                      */
	bnad_stats_timer_stop(bnad);

	init_completion(&bnad->bnad_completions.enet_comp);

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_enet_disable(&bnad->bna.enet, BNA_HARD_CLEANUP,
			bnad_cb_enet_disabled);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	wait_for_completion(&bnad->bnad_completions.enet_comp);

	bnad_destroy_tx(bnad, 0);
	bnad_destroy_rx(bnad, 0);

	/*                         */
	bnad_mbox_irq_sync(bnad);

	mutex_unlock(&bnad->conf_mutex);

	return 0;
}

/*    */
/*                       */
static int
bnad_txq_wi_prepare(struct bnad *bnad, struct bna_tcb *tcb,
		    struct sk_buff *skb, struct bna_txq_entry *txqent)
{
	u16 flags = 0;
	u32 gso_size;
	u16 vlan_tag = 0;

	if (vlan_tx_tag_present(skb)) {
		vlan_tag = (u16)vlan_tx_tag_get(skb);
		flags |= (BNA_TXQ_WI_CF_INS_PRIO | BNA_TXQ_WI_CF_INS_VLAN);
	}
	if (test_bit(BNAD_RF_CEE_RUNNING, &bnad->run_flags)) {
		vlan_tag = ((tcb->priority & 0x7) << VLAN_PRIO_SHIFT)
				| (vlan_tag & 0x1fff);
		flags |= (BNA_TXQ_WI_CF_INS_PRIO | BNA_TXQ_WI_CF_INS_VLAN);
	}
	txqent->hdr.wi.vlan_tag = htons(vlan_tag);

	if (skb_is_gso(skb)) {
		gso_size = skb_shinfo(skb)->gso_size;
		if (unlikely(gso_size > bnad->netdev->mtu)) {
			BNAD_UPDATE_CTR(bnad, tx_skb_mss_too_long);
			return -EINVAL;
		}
		if (unlikely((gso_size + skb_transport_offset(skb) +
			      tcp_hdrlen(skb)) >= skb->len)) {
			txqent->hdr.wi.opcode =
				__constant_htons(BNA_TXQ_WI_SEND);
			txqent->hdr.wi.lso_mss = 0;
			BNAD_UPDATE_CTR(bnad, tx_skb_tso_too_short);
		} else {
			txqent->hdr.wi.opcode =
				__constant_htons(BNA_TXQ_WI_SEND_LSO);
			txqent->hdr.wi.lso_mss = htons(gso_size);
		}

		if (bnad_tso_prepare(bnad, skb)) {
			BNAD_UPDATE_CTR(bnad, tx_skb_tso_prepare);
			return -EINVAL;
		}

		flags |= (BNA_TXQ_WI_CF_IP_CKSUM | BNA_TXQ_WI_CF_TCP_CKSUM);
		txqent->hdr.wi.l4_hdr_size_n_offset =
			htons(BNA_TXQ_WI_L4_HDR_N_OFFSET(
			tcp_hdrlen(skb) >> 2, skb_transport_offset(skb)));
	} else  {
		txqent->hdr.wi.opcode =	__constant_htons(BNA_TXQ_WI_SEND);
		txqent->hdr.wi.lso_mss = 0;

		if (unlikely(skb->len > (bnad->netdev->mtu + ETH_HLEN))) {
			BNAD_UPDATE_CTR(bnad, tx_skb_non_tso_too_long);
			return -EINVAL;
		}

		if (skb->ip_summed == CHECKSUM_PARTIAL) {
			u8 proto = 0;

			if (skb->protocol == __constant_htons(ETH_P_IP))
				proto = ip_hdr(skb)->protocol;
#ifdef NETIF_F_IPV6_CSUM
			else if (skb->protocol ==
				 __constant_htons(ETH_P_IPV6)) {
				/*                                     */
				proto = ipv6_hdr(skb)->nexthdr;
			}
#endif
			if (proto == IPPROTO_TCP) {
				flags |= BNA_TXQ_WI_CF_TCP_CKSUM;
				txqent->hdr.wi.l4_hdr_size_n_offset =
					htons(BNA_TXQ_WI_L4_HDR_N_OFFSET
					      (0, skb_transport_offset(skb)));

				BNAD_UPDATE_CTR(bnad, tcpcsum_offload);

				if (unlikely(skb_headlen(skb) <
					    skb_transport_offset(skb) +
				    tcp_hdrlen(skb))) {
					BNAD_UPDATE_CTR(bnad, tx_skb_tcp_hdr);
					return -EINVAL;
				}
			} else if (proto == IPPROTO_UDP) {
				flags |= BNA_TXQ_WI_CF_UDP_CKSUM;
				txqent->hdr.wi.l4_hdr_size_n_offset =
					htons(BNA_TXQ_WI_L4_HDR_N_OFFSET
					      (0, skb_transport_offset(skb)));

				BNAD_UPDATE_CTR(bnad, udpcsum_offload);
				if (unlikely(skb_headlen(skb) <
					    skb_transport_offset(skb) +
				    sizeof(struct udphdr))) {
					BNAD_UPDATE_CTR(bnad, tx_skb_udp_hdr);
					return -EINVAL;
				}
			} else {

				BNAD_UPDATE_CTR(bnad, tx_skb_csum_err);
				return -EINVAL;
			}
		} else
			txqent->hdr.wi.l4_hdr_size_n_offset = 0;
	}

	txqent->hdr.wi.flags = htons(flags);
	txqent->hdr.wi.frame_length = htonl(skb->len);

	return 0;
}

/*
                                                    
                                             
 */
static netdev_tx_t
bnad_start_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	struct bnad *bnad = netdev_priv(netdev);
	u32 txq_id = 0;
	struct bna_tcb *tcb = NULL;
	struct bnad_tx_unmap *unmap_q, *unmap, *head_unmap;
	u32		prod, q_depth, vect_id;
	u32		wis, vectors, len;
	int		i;
	dma_addr_t		dma_addr;
	struct bna_txq_entry *txqent;

	len = skb_headlen(skb);

	/*                           */

	if (unlikely(skb->len <= ETH_HLEN)) {
		dev_kfree_skb(skb);
		BNAD_UPDATE_CTR(bnad, tx_skb_too_short);
		return NETDEV_TX_OK;
	}
	if (unlikely(len > BFI_TX_MAX_DATA_PER_VECTOR)) {
		dev_kfree_skb(skb);
		BNAD_UPDATE_CTR(bnad, tx_skb_headlen_zero);
		return NETDEV_TX_OK;
	}
	if (unlikely(len == 0)) {
		dev_kfree_skb(skb);
		BNAD_UPDATE_CTR(bnad, tx_skb_headlen_zero);
		return NETDEV_TX_OK;
	}

	tcb = bnad->tx_info[0].tcb[txq_id];
	q_depth = tcb->q_depth;
	prod = tcb->producer_index;

	unmap_q = tcb->unmap_q;

	/*
                                                                    
                                            
  */
	if (unlikely(!test_bit(BNAD_TXQ_TX_STARTED, &tcb->flags))) {
		dev_kfree_skb(skb);
		BNAD_UPDATE_CTR(bnad, tx_skb_stopping);
		return NETDEV_TX_OK;
	}

	vectors = 1 + skb_shinfo(skb)->nr_frags;
	wis = BNA_TXQ_WI_NEEDED(vectors);	/*                         */

	if (unlikely(vectors > BFI_TX_MAX_VECTORS_PER_PKT)) {
		dev_kfree_skb(skb);
		BNAD_UPDATE_CTR(bnad, tx_skb_max_vectors);
		return NETDEV_TX_OK;
	}

	/*                                   */
	if (unlikely(wis > BNA_QE_FREE_CNT(tcb, q_depth))) {
		if ((*tcb->hw_consumer_index != tcb->consumer_index) &&
		    !test_and_set_bit(BNAD_TXQ_FREE_SENT, &tcb->flags)) {
			u32 sent;
			sent = bnad_txcmpl_process(bnad, tcb);
			if (likely(test_bit(BNAD_TXQ_TX_STARTED, &tcb->flags)))
				bna_ib_ack(tcb->i_dbell, sent);
			smp_mb__before_clear_bit();
			clear_bit(BNAD_TXQ_FREE_SENT, &tcb->flags);
		} else {
			netif_stop_queue(netdev);
			BNAD_UPDATE_CTR(bnad, netif_queue_stop);
		}

		smp_mb();
		/*
                                                    
                                                   
                                                         
   */
		if (likely(wis > BNA_QE_FREE_CNT(tcb, q_depth))) {
			BNAD_UPDATE_CTR(bnad, netif_queue_stop);
			return NETDEV_TX_BUSY;
		} else {
			netif_wake_queue(netdev);
			BNAD_UPDATE_CTR(bnad, netif_queue_wakeup);
		}
	}

	txqent = &((struct bna_txq_entry *)tcb->sw_q)[prod];
	head_unmap = &unmap_q[prod];

	/*                                                         */
	if (bnad_txq_wi_prepare(bnad, tcb, skb, txqent)) {
		dev_kfree_skb(skb);
		return NETDEV_TX_OK;
	}
	txqent->hdr.wi.reserved = 0;
	txqent->hdr.wi.num_vectors = vectors;

	head_unmap->skb = skb;
	head_unmap->nvecs = 0;

	/*                     */
	unmap = head_unmap;
	dma_addr = dma_map_single(&bnad->pcidev->dev, skb->data,
				  len, DMA_TO_DEVICE);
	BNA_SET_DMA_ADDR(dma_addr, &txqent->vector[0].host_addr);
	txqent->vector[0].length = htons(len);
	dma_unmap_addr_set(&unmap->vectors[0], dma_addr, dma_addr);
	head_unmap->nvecs++;

	for (i = 0, vect_id = 0; i < vectors - 1; i++) {
		const struct skb_frag_struct *frag = &skb_shinfo(skb)->frags[i];
		u16		size = skb_frag_size(frag);

		if (unlikely(size == 0)) {
			/*                                                  */
			bnad_tx_buff_unmap(bnad, unmap_q, q_depth,
				tcb->producer_index);
			dev_kfree_skb(skb);
			BNAD_UPDATE_CTR(bnad, tx_skb_frag_zero);
			return NETDEV_TX_OK;
		}

		len += size;

		vect_id++;
		if (vect_id == BFI_TX_MAX_VECTORS_PER_WI) {
			vect_id = 0;
			BNA_QE_INDX_INC(prod, q_depth);
			txqent = &((struct bna_txq_entry *)tcb->sw_q)[prod];
			txqent->hdr.wi_ext.opcode =
				__constant_htons(BNA_TXQ_WI_EXTENSION);
			unmap = &unmap_q[prod];
		}

		dma_addr = skb_frag_dma_map(&bnad->pcidev->dev, frag,
					    0, size, DMA_TO_DEVICE);
		BNA_SET_DMA_ADDR(dma_addr, &txqent->vector[vect_id].host_addr);
		txqent->vector[vect_id].length = htons(size);
		dma_unmap_addr_set(&unmap->vectors[vect_id], dma_addr,
						dma_addr);
		head_unmap->nvecs++;
	}

	if (unlikely(len != skb->len)) {
		/*                                                  */
		bnad_tx_buff_unmap(bnad, unmap_q, q_depth, tcb->producer_index);
		dev_kfree_skb(skb);
		BNAD_UPDATE_CTR(bnad, tx_skb_len_mismatch);
		return NETDEV_TX_OK;
	}

	BNA_QE_INDX_INC(prod, q_depth);
	tcb->producer_index = prod;

	smp_mb();

	if (unlikely(!test_bit(BNAD_TXQ_TX_STARTED, &tcb->flags)))
		return NETDEV_TX_OK;

	bna_txq_prod_indx_doorbell(tcb);
	smp_mb();

	return NETDEV_TX_OK;
}

/*
                                                                   
                                         
 */
static struct rtnl_link_stats64 *
bnad_get_stats64(struct net_device *netdev, struct rtnl_link_stats64 *stats)
{
	struct bnad *bnad = netdev_priv(netdev);
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);

	bnad_netdev_qstats_fill(bnad, stats);
	bnad_netdev_hwstats_fill(bnad, stats);

	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	return stats;
}

void
bnad_set_rx_mode(struct net_device *netdev)
{
	struct bnad *bnad = netdev_priv(netdev);
	u32	new_mask, valid_mask;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);

	new_mask = valid_mask = 0;

	if (netdev->flags & IFF_PROMISC) {
		if (!(bnad->cfg_flags & BNAD_CF_PROMISC)) {
			new_mask = BNAD_RXMODE_PROMISC_DEFAULT;
			valid_mask = BNAD_RXMODE_PROMISC_DEFAULT;
			bnad->cfg_flags |= BNAD_CF_PROMISC;
		}
	} else {
		if (bnad->cfg_flags & BNAD_CF_PROMISC) {
			new_mask = ~BNAD_RXMODE_PROMISC_DEFAULT;
			valid_mask = BNAD_RXMODE_PROMISC_DEFAULT;
			bnad->cfg_flags &= ~BNAD_CF_PROMISC;
		}
	}

	if (netdev->flags & IFF_ALLMULTI) {
		if (!(bnad->cfg_flags & BNAD_CF_ALLMULTI)) {
			new_mask |= BNA_RXMODE_ALLMULTI;
			valid_mask |= BNA_RXMODE_ALLMULTI;
			bnad->cfg_flags |= BNAD_CF_ALLMULTI;
		}
	} else {
		if (bnad->cfg_flags & BNAD_CF_ALLMULTI) {
			new_mask &= ~BNA_RXMODE_ALLMULTI;
			valid_mask |= BNA_RXMODE_ALLMULTI;
			bnad->cfg_flags &= ~BNAD_CF_ALLMULTI;
		}
	}

	if (bnad->rx_info[0].rx == NULL)
		goto unlock;

	bna_rx_mode_set(bnad->rx_info[0].rx, new_mask, valid_mask, NULL);

	if (!netdev_mc_empty(netdev)) {
		u8 *mcaddr_list;
		int mc_count = netdev_mc_count(netdev);

		/*                                     */
		mcaddr_list =
			kzalloc((mc_count + 1) * ETH_ALEN,
				GFP_ATOMIC);
		if (!mcaddr_list)
			goto unlock;

		memcpy(&mcaddr_list[0], &bnad_bcast_addr[0], ETH_ALEN);

		/*                               */
		bnad_netdev_mc_list_get(netdev, mcaddr_list);

		bna_rx_mcast_listset(bnad->rx_info[0].rx, mc_count + 1,
					mcaddr_list, NULL);

		/*                                                  */
		kfree(mcaddr_list);
	}
unlock:
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
}

/*
                                                  
                                                       
                             
 */
static int
bnad_set_mac_address(struct net_device *netdev, void *mac_addr)
{
	int err;
	struct bnad *bnad = netdev_priv(netdev);
	struct sockaddr *sa = (struct sockaddr *)mac_addr;
	unsigned long flags;

	spin_lock_irqsave(&bnad->bna_lock, flags);

	err = bnad_mac_addr_set_locked(bnad, sa->sa_data);

	if (!err)
		memcpy(netdev->dev_addr, sa->sa_data, netdev->addr_len);

	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	return err;
}

static int
bnad_mtu_set(struct bnad *bnad, int mtu)
{
	unsigned long flags;

	init_completion(&bnad->bnad_completions.mtu_comp);

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_enet_mtu_set(&bnad->bna.enet, mtu, bnad_cb_enet_mtu_set);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	wait_for_completion(&bnad->bnad_completions.mtu_comp);

	return bnad->bnad_completions.mtu_comp_status;
}

static int
bnad_change_mtu(struct net_device *netdev, int new_mtu)
{
	int err, mtu = netdev->mtu;
	struct bnad *bnad = netdev_priv(netdev);

	if (new_mtu + ETH_HLEN < ETH_ZLEN || new_mtu > BNAD_JUMBO_MTU)
		return -EINVAL;

	mutex_lock(&bnad->conf_mutex);

	netdev->mtu = new_mtu;

	mtu = ETH_HLEN + VLAN_HLEN + new_mtu + ETH_FCS_LEN;
	err = bnad_mtu_set(bnad, mtu);
	if (err)
		err = -EBUSY;

	mutex_unlock(&bnad->conf_mutex);
	return err;
}

static int
bnad_vlan_rx_add_vid(struct net_device *netdev, __be16 proto, u16 vid)
{
	struct bnad *bnad = netdev_priv(netdev);
	unsigned long flags;

	if (!bnad->rx_info[0].rx)
		return 0;

	mutex_lock(&bnad->conf_mutex);

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_rx_vlan_add(bnad->rx_info[0].rx, vid);
	set_bit(vid, bnad->active_vlans);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	mutex_unlock(&bnad->conf_mutex);

	return 0;
}

static int
bnad_vlan_rx_kill_vid(struct net_device *netdev, __be16 proto, u16 vid)
{
	struct bnad *bnad = netdev_priv(netdev);
	unsigned long flags;

	if (!bnad->rx_info[0].rx)
		return 0;

	mutex_lock(&bnad->conf_mutex);

	spin_lock_irqsave(&bnad->bna_lock, flags);
	clear_bit(vid, bnad->active_vlans);
	bna_rx_vlan_del(bnad->rx_info[0].rx, vid);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	mutex_unlock(&bnad->conf_mutex);

	return 0;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void
bnad_netpoll(struct net_device *netdev)
{
	struct bnad *bnad = netdev_priv(netdev);
	struct bnad_rx_info *rx_info;
	struct bnad_rx_ctrl *rx_ctrl;
	u32 curr_mask;
	int i, j;

	if (!(bnad->cfg_flags & BNAD_CF_MSIX)) {
		bna_intx_disable(&bnad->bna, curr_mask);
		bnad_isr(bnad->pcidev->irq, netdev);
		bna_intx_enable(&bnad->bna, curr_mask);
	} else {
		/*
                                                            
                                           
   */

		/*               */
		for (i = 0; i < bnad->num_rx; i++) {
			rx_info = &bnad->rx_info[i];
			if (!rx_info->rx)
				continue;
			for (j = 0; j < bnad->num_rxp_per_rx; j++) {
				rx_ctrl = &rx_info->rx_ctrl[j];
				if (rx_ctrl->ccb)
					bnad_netif_rx_schedule_poll(bnad,
							    rx_ctrl->ccb);
			}
		}
	}
}
#endif

static const struct net_device_ops bnad_netdev_ops = {
	.ndo_open		= bnad_open,
	.ndo_stop		= bnad_stop,
	.ndo_start_xmit		= bnad_start_xmit,
	.ndo_get_stats64		= bnad_get_stats64,
	.ndo_set_rx_mode	= bnad_set_rx_mode,
	.ndo_validate_addr      = eth_validate_addr,
	.ndo_set_mac_address    = bnad_set_mac_address,
	.ndo_change_mtu		= bnad_change_mtu,
	.ndo_vlan_rx_add_vid    = bnad_vlan_rx_add_vid,
	.ndo_vlan_rx_kill_vid   = bnad_vlan_rx_kill_vid,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller    = bnad_netpoll
#endif
};

static void
bnad_netdev_init(struct bnad *bnad, bool using_dac)
{
	struct net_device *netdev = bnad->netdev;

	netdev->hw_features = NETIF_F_SG | NETIF_F_RXCSUM |
		NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
		NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_HW_VLAN_CTAG_TX;

	netdev->vlan_features = NETIF_F_SG | NETIF_F_HIGHDMA |
		NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
		NETIF_F_TSO | NETIF_F_TSO6;

	netdev->features |= netdev->hw_features |
		NETIF_F_HW_VLAN_CTAG_RX | NETIF_F_HW_VLAN_CTAG_FILTER;

	if (using_dac)
		netdev->features |= NETIF_F_HIGHDMA;

	netdev->mem_start = bnad->mmio_start;
	netdev->mem_end = bnad->mmio_start + bnad->mmio_len - 1;

	netdev->netdev_ops = &bnad_netdev_ops;
	bnad_set_ethtool_ops(netdev);
}

/*
                                   
                                     
                                                
                            
 */
static int
bnad_init(struct bnad *bnad,
	  struct pci_dev *pdev, struct net_device *netdev)
{
	unsigned long flags;

	SET_NETDEV_DEV(netdev, &pdev->dev);
	pci_set_drvdata(pdev, netdev);

	bnad->netdev = netdev;
	bnad->pcidev = pdev;
	bnad->mmio_start = pci_resource_start(pdev, 0);
	bnad->mmio_len = pci_resource_len(pdev, 0);
	bnad->bar0 = ioremap_nocache(bnad->mmio_start, bnad->mmio_len);
	if (!bnad->bar0) {
		dev_err(&pdev->dev, "ioremap for bar0 failed\n");
		pci_set_drvdata(pdev, NULL);
		return -ENOMEM;
	}
	pr_info("bar0 mapped to %p, len %llu\n", bnad->bar0,
	       (unsigned long long) bnad->mmio_len);

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (!bnad_msix_disable)
		bnad->cfg_flags = BNAD_CF_MSIX;

	bnad->cfg_flags |= BNAD_CF_DIM_ENABLED;

	bnad_q_num_init(bnad);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	bnad->msix_num = (bnad->num_tx * bnad->num_txq_per_tx) +
		(bnad->num_rx * bnad->num_rxp_per_rx) +
			 BNAD_MAILBOX_MSIX_VECTORS;

	bnad->txq_depth = BNAD_TXQ_DEPTH;
	bnad->rxq_depth = BNAD_RXQ_DEPTH;

	bnad->tx_coalescing_timeo = BFI_TX_COALESCING_TIMEO;
	bnad->rx_coalescing_timeo = BFI_RX_COALESCING_TIMEO;

	sprintf(bnad->wq_name, "%s_wq_%d", BNAD_NAME, bnad->id);
	bnad->work_q = create_singlethread_workqueue(bnad->wq_name);
	if (!bnad->work_q) {
		iounmap(bnad->bar0);
		return -ENOMEM;
	}

	return 0;
}

/*
                                         
                                              
                                           
 */
static void
bnad_uninit(struct bnad *bnad)
{
	if (bnad->work_q) {
		flush_workqueue(bnad->work_q);
		destroy_workqueue(bnad->work_q);
		bnad->work_q = NULL;
	}

	if (bnad->bar0)
		iounmap(bnad->bar0);
	pci_set_drvdata(bnad->pcidev, NULL);
}

/*
                   
                                                       
                             
                                               
 */
static void
bnad_lock_init(struct bnad *bnad)
{
	spin_lock_init(&bnad->bna_lock);
	mutex_init(&bnad->conf_mutex);
	mutex_init(&bnad_list_mutex);
}

static void
bnad_lock_uninit(struct bnad *bnad)
{
	mutex_destroy(&bnad->conf_mutex);
	mutex_destroy(&bnad_list_mutex);
}

/*                    */
static int
bnad_pci_init(struct bnad *bnad,
	      struct pci_dev *pdev, bool *using_dac)
{
	int err;

	err = pci_enable_device(pdev);
	if (err)
		return err;
	err = pci_request_regions(pdev, BNAD_NAME);
	if (err)
		goto disable_device;
	if (!dma_set_mask(&pdev->dev, DMA_BIT_MASK(64)) &&
	    !dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(64))) {
		*using_dac = true;
	} else {
		err = dma_set_mask(&pdev->dev, DMA_BIT_MASK(32));
		if (err) {
			err = dma_set_coherent_mask(&pdev->dev,
						    DMA_BIT_MASK(32));
			if (err)
				goto release_regions;
		}
		*using_dac = false;
	}
	pci_set_master(pdev);
	return 0;

release_regions:
	pci_release_regions(pdev);
disable_device:
	pci_disable_device(pdev);

	return err;
}

static void
bnad_pci_uninit(struct pci_dev *pdev)
{
	pci_release_regions(pdev);
	pci_disable_device(pdev);
}

static int
bnad_pci_probe(struct pci_dev *pdev,
		const struct pci_device_id *pcidev_id)
{
	bool	using_dac;
	int	err;
	struct bnad *bnad;
	struct bna *bna;
	struct net_device *netdev;
	struct bfa_pcidev pcidev_info;
	unsigned long flags;

	pr_info("bnad_pci_probe : (0x%p, 0x%p) PCI Func : (%d)\n",
	       pdev, pcidev_id, PCI_FUNC(pdev->devfn));

	mutex_lock(&bnad_fwimg_mutex);
	if (!cna_get_firmware_buf(pdev)) {
		mutex_unlock(&bnad_fwimg_mutex);
		pr_warn("Failed to load Firmware Image!\n");
		return -ENODEV;
	}
	mutex_unlock(&bnad_fwimg_mutex);

	/*
                                                     
                       
  */
	netdev = alloc_etherdev(sizeof(struct bnad));
	if (!netdev) {
		err = -ENOMEM;
		return err;
	}
	bnad = netdev_priv(netdev);
	bnad_lock_init(bnad);
	bnad_add_to_list(bnad);

	mutex_lock(&bnad->conf_mutex);
	/*
                      
                                         
                           
  */
	using_dac = false;
	err = bnad_pci_init(bnad, pdev, &using_dac);
	if (err)
		goto unlock_mutex;

	/*
                             
                                           
  */
	err = bnad_init(bnad, pdev, netdev);
	if (err)
		goto pci_uninit;

	/*                                                 */
	bnad_netdev_init(bnad, using_dac);

	/*                        */
	netif_carrier_off(netdev);

	/*                                      */
	if (bna_debugfs_enable)
		bnad_debugfs_init(bnad);

	/*                                   */
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_res_req(&bnad->res_info[0]);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	/*                             */
	err = bnad_res_alloc(bnad, &bnad->res_info[0], BNA_RES_T_MAX);
	if (err)
		goto drv_uninit;

	bna = &bnad->bna;

	/*                                  */
	pcidev_info.pci_slot = PCI_SLOT(bnad->pcidev->devfn);
	pcidev_info.pci_func = PCI_FUNC(bnad->pcidev->devfn);
	pcidev_info.device_id = bnad->pcidev->device;
	pcidev_info.pci_bar_kva = bnad->bar0;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_init(bna, bnad, &pcidev_info, &bnad->res_info[0]);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	bnad->stats.bna_stats = &bna->stats;

	bnad_enable_msix(bnad);
	err = bnad_mbox_irq_alloc(bnad);
	if (err)
		goto res_free;

	/*               */
	setup_timer(&bnad->bna.ioceth.ioc.ioc_timer, bnad_ioc_timeout,
				((unsigned long)bnad));
	setup_timer(&bnad->bna.ioceth.ioc.hb_timer, bnad_ioc_hb_check,
				((unsigned long)bnad));
	setup_timer(&bnad->bna.ioceth.ioc.iocpf_timer, bnad_iocpf_timeout,
				((unsigned long)bnad));
	setup_timer(&bnad->bna.ioceth.ioc.sem_timer, bnad_iocpf_sem_timeout,
				((unsigned long)bnad));

	/*                                        */
	mod_timer(&bnad->bna.ioceth.ioc.iocpf_timer,
		  jiffies + msecs_to_jiffies(BNA_IOC_TIMER_FREQ));

	/*
                  
                                                   
                                 
  */
	err = bnad_ioceth_enable(bnad);
	if (err) {
		pr_err("BNA: Initialization failed err=%d\n",
		       err);
		goto probe_success;
	}

	spin_lock_irqsave(&bnad->bna_lock, flags);
	if (bna_num_txq_set(bna, BNAD_NUM_TXQ + 1) ||
		bna_num_rxp_set(bna, BNAD_NUM_RXP + 1)) {
		bnad_q_num_adjust(bnad, bna_attr(bna)->num_txq - 1,
			bna_attr(bna)->num_rxp - 1);
		if (bna_num_txq_set(bna, BNAD_NUM_TXQ + 1) ||
			bna_num_rxp_set(bna, BNAD_NUM_RXP + 1))
			err = -EIO;
	}
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
	if (err)
		goto disable_ioceth;

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_mod_res_req(&bnad->bna, &bnad->mod_res_info[0]);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	err = bnad_res_alloc(bnad, &bnad->mod_res_info[0], BNA_MOD_RES_T_MAX);
	if (err) {
		err = -EIO;
		goto disable_ioceth;
	}

	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_mod_init(&bnad->bna, &bnad->mod_res_info[0]);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	/*                      */
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_enet_perm_mac_get(&bna->enet, &bnad->perm_addr);
	bnad_set_netdev_perm_addr(bnad);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	mutex_unlock(&bnad->conf_mutex);

	/*                                          */
	err = register_netdev(netdev);
	if (err) {
		pr_err("BNA : Registering with netdev failed\n");
		goto probe_uninit;
	}
	set_bit(BNAD_RF_NETDEV_REGISTERED, &bnad->run_flags);

	return 0;

probe_success:
	mutex_unlock(&bnad->conf_mutex);
	return 0;

probe_uninit:
	mutex_lock(&bnad->conf_mutex);
	bnad_res_free(bnad, &bnad->mod_res_info[0], BNA_MOD_RES_T_MAX);
disable_ioceth:
	bnad_ioceth_disable(bnad);
	del_timer_sync(&bnad->bna.ioceth.ioc.ioc_timer);
	del_timer_sync(&bnad->bna.ioceth.ioc.sem_timer);
	del_timer_sync(&bnad->bna.ioceth.ioc.hb_timer);
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_uninit(bna);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);
	bnad_mbox_irq_free(bnad);
	bnad_disable_msix(bnad);
res_free:
	bnad_res_free(bnad, &bnad->res_info[0], BNA_RES_T_MAX);
drv_uninit:
	/*                                       */
	kfree(bnad->regdata);
	bnad_debugfs_uninit(bnad);
	bnad_uninit(bnad);
pci_uninit:
	bnad_pci_uninit(pdev);
unlock_mutex:
	mutex_unlock(&bnad->conf_mutex);
	bnad_remove_from_list(bnad);
	bnad_lock_uninit(bnad);
	free_netdev(netdev);
	return err;
}

static void
bnad_pci_remove(struct pci_dev *pdev)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct bnad *bnad;
	struct bna *bna;
	unsigned long flags;

	if (!netdev)
		return;

	pr_info("%s bnad_pci_remove\n", netdev->name);
	bnad = netdev_priv(netdev);
	bna = &bnad->bna;

	if (test_and_clear_bit(BNAD_RF_NETDEV_REGISTERED, &bnad->run_flags))
		unregister_netdev(netdev);

	mutex_lock(&bnad->conf_mutex);
	bnad_ioceth_disable(bnad);
	del_timer_sync(&bnad->bna.ioceth.ioc.ioc_timer);
	del_timer_sync(&bnad->bna.ioceth.ioc.sem_timer);
	del_timer_sync(&bnad->bna.ioceth.ioc.hb_timer);
	spin_lock_irqsave(&bnad->bna_lock, flags);
	bna_uninit(bna);
	spin_unlock_irqrestore(&bnad->bna_lock, flags);

	bnad_res_free(bnad, &bnad->mod_res_info[0], BNA_MOD_RES_T_MAX);
	bnad_res_free(bnad, &bnad->res_info[0], BNA_RES_T_MAX);
	bnad_mbox_irq_free(bnad);
	bnad_disable_msix(bnad);
	bnad_pci_uninit(pdev);
	mutex_unlock(&bnad->conf_mutex);
	bnad_remove_from_list(bnad);
	bnad_lock_uninit(bnad);
	/*                                       */
	kfree(bnad->regdata);
	bnad_debugfs_uninit(bnad);
	bnad_uninit(bnad);
	free_netdev(netdev);
}

static DEFINE_PCI_DEVICE_TABLE(bnad_pci_id_table) = {
	{
		PCI_DEVICE(PCI_VENDOR_ID_BROCADE,
			PCI_DEVICE_ID_BROCADE_CT),
		.class = PCI_CLASS_NETWORK_ETHERNET << 8,
		.class_mask =  0xffff00
	},
	{
		PCI_DEVICE(PCI_VENDOR_ID_BROCADE,
			BFA_PCI_DEVICE_ID_CT2),
		.class = PCI_CLASS_NETWORK_ETHERNET << 8,
		.class_mask =  0xffff00
	},
	{0,  },
};

MODULE_DEVICE_TABLE(pci, bnad_pci_id_table);

static struct pci_driver bnad_pci_driver = {
	.name = BNAD_NAME,
	.id_table = bnad_pci_id_table,
	.probe = bnad_pci_probe,
	.remove = bnad_pci_remove,
};

static int __init
bnad_module_init(void)
{
	int err;

	pr_info("Brocade 10G Ethernet driver - version: %s\n",
			BNAD_VERSION);

	bfa_nw_ioc_auto_recover(bnad_ioc_auto_recover);

	err = pci_register_driver(&bnad_pci_driver);
	if (err < 0) {
		pr_err("bna : PCI registration failed in module init "
		       "(%d)\n", err);
		return err;
	}

	return 0;
}

static void __exit
bnad_module_exit(void)
{
	pci_unregister_driver(&bnad_pci_driver);
	release_firmware(bfi_fw);
}

module_init(bnad_module_init);
module_exit(bnad_module_exit);

MODULE_AUTHOR("Brocade");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Brocade 10G PCIe Ethernet driver");
MODULE_VERSION(BNAD_VERSION);
MODULE_FIRMWARE(CNA_FW_FILE_CT);
MODULE_FIRMWARE(CNA_FW_FILE_CT2);