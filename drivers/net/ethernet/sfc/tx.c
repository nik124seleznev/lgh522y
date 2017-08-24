/****************************************************************************
 * Driver for Solarflare Solarstorm network controllers and boards
 * Copyright 2005-2006 Fen Systems Ltd.
 * Copyright 2005-2010 Solarflare Communications Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, incorporated herein by reference.
 */

#include <linux/pci.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/ipv6.h>
#include <linux/slab.h>
#include <net/ipv6.h>
#include <linux/if_ether.h>
#include <linux/highmem.h>
#include "net_driver.h"
#include "efx.h"
#include "nic.h"
#include "workarounds.h"

static void efx_dequeue_buffer(struct efx_tx_queue *tx_queue,
			       struct efx_tx_buffer *buffer,
			       unsigned int *pkts_compl,
			       unsigned int *bytes_compl)
{
	if (buffer->unmap_len) {
		struct device *dma_dev = &tx_queue->efx->pci_dev->dev;
		dma_addr_t unmap_addr = (buffer->dma_addr + buffer->len -
					 buffer->unmap_len);
		if (buffer->flags & EFX_TX_BUF_MAP_SINGLE)
			dma_unmap_single(dma_dev, unmap_addr, buffer->unmap_len,
					 DMA_TO_DEVICE);
		else
			dma_unmap_page(dma_dev, unmap_addr, buffer->unmap_len,
				       DMA_TO_DEVICE);
		buffer->unmap_len = 0;
	}

	if (buffer->flags & EFX_TX_BUF_SKB) {
		(*pkts_compl)++;
		(*bytes_compl) += buffer->skb->len;
		dev_kfree_skb_any((struct sk_buff *) buffer->skb);
		netif_vdbg(tx_queue->efx, tx_done, tx_queue->efx->net_dev,
			   "TX queue %d transmission id %x complete\n",
			   tx_queue->queue, tx_queue->read_count);
	} else if (buffer->flags & EFX_TX_BUF_HEAP) {
		kfree(buffer->heap_buf);
	}

	buffer->len = 0;
	buffer->flags = 0;
}

static int efx_enqueue_skb_tso(struct efx_tx_queue *tx_queue,
			       struct sk_buff *skb);

static inline unsigned
efx_max_tx_len(struct efx_nic *efx, dma_addr_t dma_addr)
{
	/*                                                     
                                                         
                                                               
                                                          
                                                         
  */
	unsigned len = (~dma_addr & (EFX_PAGE_SIZE - 1)) + 1;

	/*                                                 */
	if (EFX_WORKAROUND_5391(efx) && (dma_addr & 0xf))
		len = min_t(unsigned, len, 512 - (dma_addr & 0xf));

	return len;
}

unsigned int efx_tx_max_skb_descs(struct efx_nic *efx)
{
	/*                                                            
                                                          
  */
	unsigned int max_descs = EFX_TSO_MAX_SEGS * 2 + MAX_SKB_FRAGS;

	/*                                                            */
	if (EFX_WORKAROUND_5391(efx))
		max_descs += EFX_TSO_MAX_SEGS;

	/*                                                               */
	if (PAGE_SIZE > EFX_PAGE_SIZE)
		max_descs += max_t(unsigned int, MAX_SKB_FRAGS,
				   DIV_ROUND_UP(GSO_MAX_SIZE, EFX_PAGE_SIZE));

	return max_descs;
}

/*                                                                    */
static struct efx_tx_queue *efx_tx_queue_partner(struct efx_tx_queue *tx_queue)
{
	if (tx_queue->queue & EFX_TXQ_TYPE_OFFLOAD)
		return tx_queue - EFX_TXQ_TYPE_OFFLOAD;
	else
		return tx_queue + EFX_TXQ_TYPE_OFFLOAD;
}

static void efx_tx_maybe_stop_queue(struct efx_tx_queue *txq1)
{
	/*                                                               */
	struct efx_tx_queue *txq2 = efx_tx_queue_partner(txq1);
	struct efx_nic *efx = txq1->efx;
	unsigned int fill_level;

	fill_level = max(txq1->insert_count - txq1->old_read_count,
			 txq2->insert_count - txq2->old_read_count);
	if (likely(fill_level < efx->txq_stop_thresh))
		return;

	/*                                                         
                                                          
                                                         
                                                
   
                                                         
                                                              
                                                              
                                                             
                                                           
                                                        
                                                           
           
  */
	netif_tx_stop_queue(txq1->core_txq);
	smp_mb();
	txq1->old_read_count = ACCESS_ONCE(txq1->read_count);
	txq2->old_read_count = ACCESS_ONCE(txq2->read_count);

	fill_level = max(txq1->insert_count - txq1->old_read_count,
			 txq2->insert_count - txq2->old_read_count);
	EFX_BUG_ON_PARANOID(fill_level >= efx->txq_entries);
	if (likely(fill_level < efx->txq_stop_thresh)) {
		smp_mb();
		if (likely(!efx->loopback_selftest))
			netif_tx_start_queue(txq1->core_txq);
	}
}

/*
                                    
  
                                                                      
                                                                   
                                                
  
                                                                   
                                                                     
  
                                                                   
                                                          
  
                        
                                                       
 */
netdev_tx_t efx_enqueue_skb(struct efx_tx_queue *tx_queue, struct sk_buff *skb)
{
	struct efx_nic *efx = tx_queue->efx;
	struct device *dma_dev = &efx->pci_dev->dev;
	struct efx_tx_buffer *buffer;
	skb_frag_t *fragment;
	unsigned int len, unmap_len = 0, insert_ptr;
	dma_addr_t dma_addr, unmap_addr = 0;
	unsigned int dma_len;
	unsigned short dma_flags;
	int i = 0;

	EFX_BUG_ON_PARANOID(tx_queue->write_count != tx_queue->insert_count);

	if (skb_shinfo(skb)->gso_size)
		return efx_enqueue_skb_tso(tx_queue, skb);

	/*                                  */
	len = skb_headlen(skb);

	/*                  */
	if (EFX_WORKAROUND_15592(efx) && skb->len <= 32) {
		EFX_BUG_ON_PARANOID(skb->data_len);
		len = 32 + 1;
		if (skb_pad(skb, len - skb->len))
			return NETDEV_TX_OK;
	}

	/*                                                          
                                                        
           
  */
	dma_flags = EFX_TX_BUF_MAP_SINGLE;
	dma_addr = dma_map_single(dma_dev, skb->data, len, PCI_DMA_TODEVICE);

	/*                       */
	while (1) {
		if (unlikely(dma_mapping_error(dma_dev, dma_addr)))
			goto dma_err;

		/*                                                   
                */
		unmap_len = len;
		unmap_addr = dma_addr;

		/*                                                  */
		do {
			insert_ptr = tx_queue->insert_count & tx_queue->ptr_mask;
			buffer = &tx_queue->buffer[insert_ptr];
			EFX_BUG_ON_PARANOID(buffer->flags);
			EFX_BUG_ON_PARANOID(buffer->len);
			EFX_BUG_ON_PARANOID(buffer->unmap_len);

			dma_len = efx_max_tx_len(efx, dma_addr);
			if (likely(dma_len >= len))
				dma_len = len;

			/*                                */
			buffer->len = dma_len;
			buffer->dma_addr = dma_addr;
			buffer->flags = EFX_TX_BUF_CONT;
			len -= dma_len;
			dma_addr += dma_len;
			++tx_queue->insert_count;
		} while (len);

		/*                                                         */
		buffer->flags = EFX_TX_BUF_CONT | dma_flags;
		buffer->unmap_len = unmap_len;
		unmap_len = 0;

		/*                                       */
		if (i >= skb_shinfo(skb)->nr_frags)
			break;
		fragment = &skb_shinfo(skb)->frags[i];
		len = skb_frag_size(fragment);
		i++;
		/*             */
		dma_flags = 0;
		dma_addr = skb_frag_dma_map(dma_dev, fragment, 0, len,
					    DMA_TO_DEVICE);
	}

	/*                                                   */
	buffer->skb = skb;
	buffer->flags = EFX_TX_BUF_SKB | dma_flags;

	netdev_tx_sent_queue(tx_queue->core_txq, skb->len);

	/*                      */
	efx_nic_push_buffers(tx_queue);

	efx_tx_maybe_stop_queue(tx_queue);

	return NETDEV_TX_OK;

 dma_err:
	netif_err(efx, tx_err, efx->net_dev,
		  " TX queue %d could not map skb with %d bytes %d "
		  "fragments for DMA\n", tx_queue->queue, skb->len,
		  skb_shinfo(skb)->nr_frags + 1);

	/*                                                            */
	dev_kfree_skb_any(skb);

	/*                                                               */
	while (tx_queue->insert_count != tx_queue->write_count) {
		unsigned int pkts_compl = 0, bytes_compl = 0;
		--tx_queue->insert_count;
		insert_ptr = tx_queue->insert_count & tx_queue->ptr_mask;
		buffer = &tx_queue->buffer[insert_ptr];
		efx_dequeue_buffer(tx_queue, buffer, &pkts_compl, &bytes_compl);
	}

	/*                                                   */
	if (unmap_len) {
		if (dma_flags & EFX_TX_BUF_MAP_SINGLE)
			dma_unmap_single(dma_dev, unmap_addr, unmap_len,
					 DMA_TO_DEVICE);
		else
			dma_unmap_page(dma_dev, unmap_addr, unmap_len,
				       DMA_TO_DEVICE);
	}

	return NETDEV_TX_OK;
}

/*                                 
  
                                                                  
                   
 */
static void efx_dequeue_buffers(struct efx_tx_queue *tx_queue,
				unsigned int index,
				unsigned int *pkts_compl,
				unsigned int *bytes_compl)
{
	struct efx_nic *efx = tx_queue->efx;
	unsigned int stop_index, read_ptr;

	stop_index = (index + 1) & tx_queue->ptr_mask;
	read_ptr = tx_queue->read_count & tx_queue->ptr_mask;

	while (read_ptr != stop_index) {
		struct efx_tx_buffer *buffer = &tx_queue->buffer[read_ptr];
		if (unlikely(buffer->len == 0)) {
			netif_err(efx, tx_err, efx->net_dev,
				  "TX queue %d spurious TX completion id %x\n",
				  tx_queue->queue, read_ptr);
			efx_schedule_reset(efx, RESET_TYPE_TX_SKIP);
			return;
		}

		efx_dequeue_buffer(tx_queue, buffer, pkts_compl, bytes_compl);

		++tx_queue->read_count;
		read_ptr = tx_queue->read_count & tx_queue->ptr_mask;
	}
}

/*                                                            
                                                                     
                                                                      
                                               
  
                         
                                                                      
                      
 */
netdev_tx_t efx_hard_start_xmit(struct sk_buff *skb,
				struct net_device *net_dev)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	struct efx_tx_queue *tx_queue;
	unsigned index, type;

	EFX_WARN_ON_PARANOID(!netif_device_present(net_dev));

	/*                    */
	if (unlikely(efx_xmit_with_hwtstamp(skb)) &&
	    unlikely(efx_ptp_is_ptp_tx(efx, skb))) {
		return efx_ptp_tx(efx, skb);
	}

	index = skb_get_queue_mapping(skb);
	type = skb->ip_summed == CHECKSUM_PARTIAL ? EFX_TXQ_TYPE_OFFLOAD : 0;
	if (index >= efx->n_tx_channels) {
		index -= efx->n_tx_channels;
		type |= EFX_TXQ_TYPE_HIGHPRI;
	}
	tx_queue = efx_get_tx_queue(efx, index, type);

	return efx_enqueue_skb(tx_queue, skb);
}

void efx_init_tx_queue_core_txq(struct efx_tx_queue *tx_queue)
{
	struct efx_nic *efx = tx_queue->efx;

	/*                                                          */
	tx_queue->core_txq =
		netdev_get_tx_queue(efx->net_dev,
				    tx_queue->queue / EFX_TXQ_TYPES +
				    ((tx_queue->queue & EFX_TXQ_TYPE_HIGHPRI) ?
				     efx->n_tx_channels : 0));
}

int efx_setup_tc(struct net_device *net_dev, u8 num_tc)
{
	struct efx_nic *efx = netdev_priv(net_dev);
	struct efx_channel *channel;
	struct efx_tx_queue *tx_queue;
	unsigned tc;
	int rc;

	if (efx_nic_rev(efx) < EFX_REV_FALCON_B0 || num_tc > EFX_MAX_TX_TC)
		return -EINVAL;

	if (num_tc == net_dev->num_tc)
		return 0;

	for (tc = 0; tc < num_tc; tc++) {
		net_dev->tc_to_txq[tc].offset = tc * efx->n_tx_channels;
		net_dev->tc_to_txq[tc].count = efx->n_tx_channels;
	}

	if (num_tc > net_dev->num_tc) {
		/*                                              */
		efx_for_each_channel(channel, efx) {
			efx_for_each_possible_channel_tx_queue(tx_queue,
							       channel) {
				if (!(tx_queue->queue & EFX_TXQ_TYPE_HIGHPRI))
					continue;
				if (!tx_queue->buffer) {
					rc = efx_probe_tx_queue(tx_queue);
					if (rc)
						return rc;
				}
				if (!tx_queue->initialised)
					efx_init_tx_queue(tx_queue);
				efx_init_tx_queue_core_txq(tx_queue);
			}
		}
	} else {
		/*                                                  */
		net_dev->num_tc = num_tc;
	}

	rc = netif_set_real_num_tx_queues(net_dev,
					  max_t(int, num_tc, 1) *
					  efx->n_tx_channels);
	if (rc)
		return rc;

	/*                                                     
                                                         
                                                           
                              
  */

	net_dev->num_tc = num_tc;
	return 0;
}

void efx_xmit_done(struct efx_tx_queue *tx_queue, unsigned int index)
{
	unsigned fill_level;
	struct efx_nic *efx = tx_queue->efx;
	struct efx_tx_queue *txq2;
	unsigned int pkts_compl = 0, bytes_compl = 0;

	EFX_BUG_ON_PARANOID(index > tx_queue->ptr_mask);

	efx_dequeue_buffers(tx_queue, index, &pkts_compl, &bytes_compl);
	netdev_tx_completed_queue(tx_queue->core_txq, pkts_compl, bytes_compl);

	/*                                                        
                                                    
                                                           
  */
	smp_mb();
	if (unlikely(netif_tx_queue_stopped(tx_queue->core_txq)) &&
	    likely(efx->port_enabled) &&
	    likely(netif_device_present(efx->net_dev))) {
		txq2 = efx_tx_queue_partner(tx_queue);
		fill_level = max(tx_queue->insert_count - tx_queue->read_count,
				 txq2->insert_count - txq2->read_count);
		if (fill_level <= efx->txq_wake_thresh)
			netif_tx_wake_queue(tx_queue->core_txq);
	}

	/*                                               */
	if ((int)(tx_queue->read_count - tx_queue->old_write_count) >= 0) {
		tx_queue->old_write_count = ACCESS_ONCE(tx_queue->write_count);
		if (tx_queue->read_count == tx_queue->old_write_count) {
			smp_mb();
			tx_queue->empty_read_count =
				tx_queue->read_count | EFX_EMPTY_COUNT_VALID;
		}
	}
}

/*                                                              
                           
 */
#define TSOH_STD_SIZE	128
#define TSOH_PER_PAGE	(PAGE_SIZE / TSOH_STD_SIZE)

/*                                                                    
                                                               
                                          
 */
static unsigned int efx_tsoh_page_count(struct efx_tx_queue *tx_queue)
{
	return DIV_ROUND_UP(tx_queue->ptr_mask + 1, 2 * TSOH_PER_PAGE);
}

int efx_probe_tx_queue(struct efx_tx_queue *tx_queue)
{
	struct efx_nic *efx = tx_queue->efx;
	unsigned int entries;
	int rc;

	/*                                               */
	entries = max(roundup_pow_of_two(efx->txq_entries), EFX_MIN_DMAQ_SIZE);
	EFX_BUG_ON_PARANOID(entries > EFX_MAX_DMAQ_SIZE);
	tx_queue->ptr_mask = entries - 1;

	netif_dbg(efx, probe, efx->net_dev,
		  "creating TX queue %d size %#x mask %#x\n",
		  tx_queue->queue, efx->txq_entries, tx_queue->ptr_mask);

	/*                        */
	tx_queue->buffer = kcalloc(entries, sizeof(*tx_queue->buffer),
				   GFP_KERNEL);
	if (!tx_queue->buffer)
		return -ENOMEM;

	if (tx_queue->queue & EFX_TXQ_TYPE_OFFLOAD) {
		tx_queue->tsoh_page =
			kcalloc(efx_tsoh_page_count(tx_queue),
				sizeof(tx_queue->tsoh_page[0]), GFP_KERNEL);
		if (!tx_queue->tsoh_page) {
			rc = -ENOMEM;
			goto fail1;
		}
	}

	/*                        */
	rc = efx_nic_probe_tx(tx_queue);
	if (rc)
		goto fail2;

	return 0;

fail2:
	kfree(tx_queue->tsoh_page);
	tx_queue->tsoh_page = NULL;
fail1:
	kfree(tx_queue->buffer);
	tx_queue->buffer = NULL;
	return rc;
}

void efx_init_tx_queue(struct efx_tx_queue *tx_queue)
{
	netif_dbg(tx_queue->efx, drv, tx_queue->efx->net_dev,
		  "initialising TX queue %d\n", tx_queue->queue);

	tx_queue->insert_count = 0;
	tx_queue->write_count = 0;
	tx_queue->old_write_count = 0;
	tx_queue->read_count = 0;
	tx_queue->old_read_count = 0;
	tx_queue->empty_read_count = 0 | EFX_EMPTY_COUNT_VALID;

	/*                           */
	efx_nic_init_tx(tx_queue);

	tx_queue->initialised = true;
}

void efx_release_tx_buffers(struct efx_tx_queue *tx_queue)
{
	struct efx_tx_buffer *buffer;

	if (!tx_queue->buffer)
		return;

	/*                                   */
	while (tx_queue->read_count != tx_queue->write_count) {
		unsigned int pkts_compl = 0, bytes_compl = 0;
		buffer = &tx_queue->buffer[tx_queue->read_count & tx_queue->ptr_mask];
		efx_dequeue_buffer(tx_queue, buffer, &pkts_compl, &bytes_compl);

		++tx_queue->read_count;
	}
	netdev_tx_reset_queue(tx_queue->core_txq);
}

void efx_fini_tx_queue(struct efx_tx_queue *tx_queue)
{
	if (!tx_queue->initialised)
		return;

	netif_dbg(tx_queue->efx, drv, tx_queue->efx->net_dev,
		  "shutting down TX queue %d\n", tx_queue->queue);

	tx_queue->initialised = false;

	/*                                        */
	efx_nic_fini_tx(tx_queue);

	efx_release_tx_buffers(tx_queue);
}

void efx_remove_tx_queue(struct efx_tx_queue *tx_queue)
{
	int i;

	if (!tx_queue->buffer)
		return;

	netif_dbg(tx_queue->efx, drv, tx_queue->efx->net_dev,
		  "destroying TX queue %d\n", tx_queue->queue);
	efx_nic_remove_tx(tx_queue);

	if (tx_queue->tsoh_page) {
		for (i = 0; i < efx_tsoh_page_count(tx_queue); i++)
			efx_nic_free_buffer(tx_queue->efx,
					    &tx_queue->tsoh_page[i]);
		kfree(tx_queue->tsoh_page);
		tx_queue->tsoh_page = NULL;
	}

	kfree(tx_queue->buffer);
	tx_queue->buffer = NULL;
}


/*                                   
  
                                                                       
                       
  
                                        
 */

/*                                                              
                           
 */
#ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
#define TSOH_OFFSET	0
#else
#define TSOH_OFFSET	NET_IP_ALIGN
#endif

#define PTR_DIFF(p1, p2)  ((u8 *)(p1) - (u8 *)(p2))

/* 
                                          
                                                
                                   
                                         
                                                   
                                             
                                                    
                                     
                                           
                                                                            
                                                      
                               
                                 
                                         
                                                                     
  
                                                                          
                                                      
 */
struct tso_state {
	/*                 */
	unsigned out_len;
	unsigned seqnum;
	unsigned ipv4_id;
	unsigned packet_space;

	/*                */
	dma_addr_t dma_addr;
	unsigned in_len;
	unsigned unmap_len;
	dma_addr_t unmap_addr;
	unsigned short dma_flags;

	__be16 protocol;
	unsigned int ip_off;
	unsigned int tcp_off;
	unsigned header_len;
	unsigned int ip_base_len;
};


/*
                                                                        
                                                                            
 */
static __be16 efx_tso_check_protocol(struct sk_buff *skb)
{
	__be16 protocol = skb->protocol;

	EFX_BUG_ON_PARANOID(((struct ethhdr *)skb->data)->h_proto !=
			    protocol);
	if (protocol == htons(ETH_P_8021Q)) {
		struct vlan_ethhdr *veh = (struct vlan_ethhdr *)skb->data;
		protocol = veh->h_vlan_encapsulated_proto;
	}

	if (protocol == htons(ETH_P_IP)) {
		EFX_BUG_ON_PARANOID(ip_hdr(skb)->protocol != IPPROTO_TCP);
	} else {
		EFX_BUG_ON_PARANOID(protocol != htons(ETH_P_IPV6));
		EFX_BUG_ON_PARANOID(ipv6_hdr(skb)->nexthdr != NEXTHDR_TCP);
	}
	EFX_BUG_ON_PARANOID((PTR_DIFF(tcp_hdr(skb), skb->data)
			     + (tcp_hdr(skb)->doff << 2u)) >
			    skb_headlen(skb));

	return protocol;
}

static u8 *efx_tsoh_get_buffer(struct efx_tx_queue *tx_queue,
			       struct efx_tx_buffer *buffer, unsigned int len)
{
	u8 *result;

	EFX_BUG_ON_PARANOID(buffer->len);
	EFX_BUG_ON_PARANOID(buffer->flags);
	EFX_BUG_ON_PARANOID(buffer->unmap_len);

	if (likely(len <= TSOH_STD_SIZE - TSOH_OFFSET)) {
		unsigned index =
			(tx_queue->insert_count & tx_queue->ptr_mask) / 2;
		struct efx_buffer *page_buf =
			&tx_queue->tsoh_page[index / TSOH_PER_PAGE];
		unsigned offset =
			TSOH_STD_SIZE * (index % TSOH_PER_PAGE) + TSOH_OFFSET;

		if (unlikely(!page_buf->addr) &&
		    efx_nic_alloc_buffer(tx_queue->efx, page_buf, PAGE_SIZE))
			return NULL;

		result = (u8 *)page_buf->addr + offset;
		buffer->dma_addr = page_buf->dma_addr + offset;
		buffer->flags = EFX_TX_BUF_CONT;
	} else {
		tx_queue->tso_long_headers++;

		buffer->heap_buf = kmalloc(TSOH_OFFSET + len, GFP_ATOMIC);
		if (unlikely(!buffer->heap_buf))
			return NULL;
		result = (u8 *)buffer->heap_buf + TSOH_OFFSET;
		buffer->flags = EFX_TX_BUF_CONT | EFX_TX_BUF_HEAP;
	}

	buffer->len = len;

	return result;
}

/* 
                                                           
                           
                                      
                            
                                                          
  
                                      
 */
static void efx_tx_queue_insert(struct efx_tx_queue *tx_queue,
				dma_addr_t dma_addr, unsigned len,
				struct efx_tx_buffer **final_buffer)
{
	struct efx_tx_buffer *buffer;
	struct efx_nic *efx = tx_queue->efx;
	unsigned dma_len, insert_ptr;

	EFX_BUG_ON_PARANOID(len <= 0);

	while (1) {
		insert_ptr = tx_queue->insert_count & tx_queue->ptr_mask;
		buffer = &tx_queue->buffer[insert_ptr];
		++tx_queue->insert_count;

		EFX_BUG_ON_PARANOID(tx_queue->insert_count -
				    tx_queue->read_count >=
				    efx->txq_entries);

		EFX_BUG_ON_PARANOID(buffer->len);
		EFX_BUG_ON_PARANOID(buffer->unmap_len);
		EFX_BUG_ON_PARANOID(buffer->flags);

		buffer->dma_addr = dma_addr;

		dma_len = efx_max_tx_len(efx, dma_addr);

		/*                                             */
		if (dma_len >= len)
			break;

		buffer->len = dma_len;
		buffer->flags = EFX_TX_BUF_CONT;
		dma_addr += dma_len;
		len -= dma_len;
	}

	EFX_BUG_ON_PARANOID(!len);
	buffer->len = len;
	*final_buffer = buffer;
}


/*
                                      
  
                                                                          
                                                                       
                                                       
 */
static int efx_tso_put_header(struct efx_tx_queue *tx_queue,
			      struct efx_tx_buffer *buffer, u8 *header)
{
	if (unlikely(buffer->flags & EFX_TX_BUF_HEAP)) {
		buffer->dma_addr = dma_map_single(&tx_queue->efx->pci_dev->dev,
						  header, buffer->len,
						  DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(&tx_queue->efx->pci_dev->dev,
					       buffer->dma_addr))) {
			kfree(buffer->heap_buf);
			buffer->len = 0;
			buffer->flags = 0;
			return -ENOMEM;
		}
		buffer->unmap_len = buffer->len;
		buffer->flags |= EFX_TX_BUF_MAP_SINGLE;
	}

	++tx_queue->insert_count;
	return 0;
}


/*                                                                   
                   
 */
static void efx_enqueue_unwind(struct efx_tx_queue *tx_queue)
{
	struct efx_tx_buffer *buffer;

	/*                                                               */
	while (tx_queue->insert_count != tx_queue->write_count) {
		--tx_queue->insert_count;
		buffer = &tx_queue->buffer[tx_queue->insert_count &
					   tx_queue->ptr_mask];
		efx_dequeue_buffer(tx_queue, buffer, NULL, NULL);
	}
}


/*                                            */
static void tso_start(struct tso_state *st, const struct sk_buff *skb)
{
	st->ip_off = skb_network_header(skb) - skb->data;
	st->tcp_off = skb_transport_header(skb) - skb->data;
	st->header_len = st->tcp_off + (tcp_hdr(skb)->doff << 2u);
	if (st->protocol == htons(ETH_P_IP)) {
		st->ip_base_len = st->header_len - st->ip_off;
		st->ipv4_id = ntohs(ip_hdr(skb)->id);
	} else {
		st->ip_base_len = st->header_len - st->tcp_off;
		st->ipv4_id = 0;
	}
	st->seqnum = ntohl(tcp_hdr(skb)->seq);

	EFX_BUG_ON_PARANOID(tcp_hdr(skb)->urg);
	EFX_BUG_ON_PARANOID(tcp_hdr(skb)->syn);
	EFX_BUG_ON_PARANOID(tcp_hdr(skb)->rst);

	st->out_len = skb->len - st->header_len;
	st->unmap_len = 0;
	st->dma_flags = 0;
}

static int tso_get_fragment(struct tso_state *st, struct efx_nic *efx,
			    skb_frag_t *frag)
{
	st->unmap_addr = skb_frag_dma_map(&efx->pci_dev->dev, frag, 0,
					  skb_frag_size(frag), DMA_TO_DEVICE);
	if (likely(!dma_mapping_error(&efx->pci_dev->dev, st->unmap_addr))) {
		st->dma_flags = 0;
		st->unmap_len = skb_frag_size(frag);
		st->in_len = skb_frag_size(frag);
		st->dma_addr = st->unmap_addr;
		return 0;
	}
	return -ENOMEM;
}

static int tso_get_head_fragment(struct tso_state *st, struct efx_nic *efx,
				 const struct sk_buff *skb)
{
	int hl = st->header_len;
	int len = skb_headlen(skb) - hl;

	st->unmap_addr = dma_map_single(&efx->pci_dev->dev, skb->data + hl,
					len, DMA_TO_DEVICE);
	if (likely(!dma_mapping_error(&efx->pci_dev->dev, st->unmap_addr))) {
		st->dma_flags = EFX_TX_BUF_MAP_SINGLE;
		st->unmap_len = len;
		st->in_len = len;
		st->dma_addr = st->unmap_addr;
		return 0;
	}
	return -ENOMEM;
}


/* 
                                                                            
                           
                       
                   
  
                                                                    
                                
 */
static void tso_fill_packet_with_fragment(struct efx_tx_queue *tx_queue,
					  const struct sk_buff *skb,
					  struct tso_state *st)
{
	struct efx_tx_buffer *buffer;
	int n;

	if (st->in_len == 0)
		return;
	if (st->packet_space == 0)
		return;

	EFX_BUG_ON_PARANOID(st->in_len <= 0);
	EFX_BUG_ON_PARANOID(st->packet_space <= 0);

	n = min(st->in_len, st->packet_space);

	st->packet_space -= n;
	st->out_len -= n;
	st->in_len -= n;

	efx_tx_queue_insert(tx_queue, st->dma_addr, n, &buffer);

	if (st->out_len == 0) {
		/*                               */
		buffer->skb = skb;
		buffer->flags = EFX_TX_BUF_SKB;
	} else if (st->packet_space != 0) {
		buffer->flags = EFX_TX_BUF_CONT;
	}

	if (st->in_len == 0) {
		/*                                       */
		buffer->unmap_len = st->unmap_len;
		buffer->flags |= st->dma_flags;
		st->unmap_len = 0;
	}

	st->dma_addr += n;
}


/* 
                                                                              
                           
                       
                   
  
                                                                     
                                                  
 */
static int tso_start_new_packet(struct efx_tx_queue *tx_queue,
				const struct sk_buff *skb,
				struct tso_state *st)
{
	struct efx_tx_buffer *buffer =
		&tx_queue->buffer[tx_queue->insert_count & tx_queue->ptr_mask];
	struct tcphdr *tsoh_th;
	unsigned ip_length;
	u8 *header;
	int rc;

	/*                                                 */
	header = efx_tsoh_get_buffer(tx_queue, buffer, st->header_len);
	if (!header)
		return -ENOMEM;

	tsoh_th = (struct tcphdr *)(header + st->tcp_off);

	/*                              */
	memcpy(header, skb->data, st->header_len);

	tsoh_th->seq = htonl(st->seqnum);
	st->seqnum += skb_shinfo(skb)->gso_size;
	if (st->out_len > skb_shinfo(skb)->gso_size) {
		/*                                            */
		st->packet_space = skb_shinfo(skb)->gso_size;
		tsoh_th->fin = 0;
		tsoh_th->psh = 0;
	} else {
		/*                                                */
		st->packet_space = st->out_len;
		tsoh_th->fin = tcp_hdr(skb)->fin;
		tsoh_th->psh = tcp_hdr(skb)->psh;
	}
	ip_length = st->ip_base_len + st->packet_space;

	if (st->protocol == htons(ETH_P_IP)) {
		struct iphdr *tsoh_iph = (struct iphdr *)(header + st->ip_off);

		tsoh_iph->tot_len = htons(ip_length);

		/*                                                               */
		tsoh_iph->id = htons(st->ipv4_id);
		st->ipv4_id++;
	} else {
		struct ipv6hdr *tsoh_iph =
			(struct ipv6hdr *)(header + st->ip_off);

		tsoh_iph->payload_len = htons(ip_length);
	}

	rc = efx_tso_put_header(tx_queue, buffer, header);
	if (unlikely(rc))
		return rc;

	++tx_queue->tso_packets;

	return 0;
}


/* 
                                                                 
                           
                       
  
                                                                
  
                                                                   
                                                                 
                 
 */
static int efx_enqueue_skb_tso(struct efx_tx_queue *tx_queue,
			       struct sk_buff *skb)
{
	struct efx_nic *efx = tx_queue->efx;
	int frag_i, rc;
	struct tso_state state;

	/*                                              */
	state.protocol = efx_tso_check_protocol(skb);

	EFX_BUG_ON_PARANOID(tx_queue->write_count != tx_queue->insert_count);

	tso_start(&state, skb);

	/*                                                              
                                    
  */
	if (skb_headlen(skb) == state.header_len) {
		/*                                  */
		EFX_BUG_ON_PARANOID(skb_shinfo(skb)->nr_frags < 1);
		frag_i = 0;
		rc = tso_get_fragment(&state, efx,
				      skb_shinfo(skb)->frags + frag_i);
		if (rc)
			goto mem_err;
	} else {
		rc = tso_get_head_fragment(&state, efx, skb);
		if (rc)
			goto mem_err;
		frag_i = -1;
	}

	if (tso_start_new_packet(tx_queue, skb, &state) < 0)
		goto mem_err;

	while (1) {
		tso_fill_packet_with_fragment(tx_queue, skb, &state);

		/*                              */
		if (state.in_len == 0) {
			if (++frag_i >= skb_shinfo(skb)->nr_frags)
				/*                         */
				break;
			rc = tso_get_fragment(&state, efx,
					      skb_shinfo(skb)->frags + frag_i);
			if (rc)
				goto mem_err;
		}

		/*                      */
		if (state.packet_space == 0 &&
		    tso_start_new_packet(tx_queue, skb, &state) < 0)
			goto mem_err;
	}

	netdev_tx_sent_queue(tx_queue->core_txq, skb->len);

	/*                      */
	efx_nic_push_buffers(tx_queue);

	efx_tx_maybe_stop_queue(tx_queue);

	tx_queue->tso_bursts++;
	return NETDEV_TX_OK;

 mem_err:
	netif_err(efx, tx_err, efx->net_dev,
		  "Out of memory for TSO headers, or DMA mapping error\n");
	dev_kfree_skb_any(skb);

	/*                                                            */
	if (state.unmap_len) {
		if (state.dma_flags & EFX_TX_BUF_MAP_SINGLE)
			dma_unmap_single(&efx->pci_dev->dev, state.unmap_addr,
					 state.unmap_len, DMA_TO_DEVICE);
		else
			dma_unmap_page(&efx->pci_dev->dev, state.unmap_addr,
				       state.unmap_len, DMA_TO_DEVICE);
	}

	efx_enqueue_unwind(tx_queue);
	return NETDEV_TX_OK;
}
