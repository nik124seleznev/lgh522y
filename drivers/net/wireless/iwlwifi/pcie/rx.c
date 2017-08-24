/******************************************************************************
 *
 * Copyright(c) 2003 - 2013 Intel Corporation. All rights reserved.
 *
 * Portions of this file are derived from the ipw3945 project, as well
 * as portions of the ieee80211 subsystem header files.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 *  Intel Linux Wireless <ilw@linux.intel.com>
 * Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
 *
 *****************************************************************************/
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/gfp.h>

#include "iwl-prph.h"
#include "iwl-io.h"
#include "internal.h"
#include "iwl-op-mode.h"

/*                                                                             
  
                    
  
                                                                              */

/*
                         
  
                                                                           
                                                                             
                                                                            
                                                                   
                                       
  
                   
                                                                           
  
                                                                             
                                                                            
             
                                                                      
  
                                                                            
                                                                             
  
                                                                        
                
  
                                                                               
                                                           
  
                                                                                
                                                                           
                                                                            
                                               
  
                                              
                                                                       
                                                                               
                                        
                                                                           
                                                                        
                                                   
                                                                           
                                                                          
                                                                          
                                                                          
                                                                          
                                                                           
  
  
                   
  
                                               
                                                                              
                                                  
                                                                          
                                                                           
                                                                               
                                                                            
  
                          
                                                                           
                                                                          
                                                                            
                                                                            
                                    
      
  
 */

/*
                                                                  
 */
static int iwl_rxq_space(const struct iwl_rxq *q)
{
	int s = q->read - q->write;
	if (s <= 0)
		s += RX_QUEUE_SIZE;
	/*                                                      */
	s -= 2;
	if (s < 0)
		s = 0;
	return s;
}

/*
                                                                          
 */
static inline __le32 iwl_pcie_dma_addr2rbd_ptr(dma_addr_t dma_addr)
{
	return cpu_to_le32((u32)(dma_addr >> 8));
}

/*
                                      
 */
int iwl_pcie_rx_stop(struct iwl_trans *trans)
{
	iwl_write_direct32(trans, FH_MEM_RCSR_CHNL0_CONFIG_REG, 0);
	return iwl_poll_direct_bit(trans, FH_MEM_RSSR_RX_STATUS_REG,
				   FH_RSSR_CHNL0_RX_STATUS_CHNL_IDLE, 1000);
}

/*
                                                                      
 */
static void iwl_pcie_rxq_inc_wr_ptr(struct iwl_trans *trans, struct iwl_rxq *q)
{
	unsigned long flags;
	u32 reg;

	spin_lock_irqsave(&q->lock, flags);

	if (q->need_update == 0)
		goto exit_unlock;

	if (trans->cfg->base_params->shadow_reg_enable) {
		/*                         */
		/*                                */
		q->write_actual = (q->write & ~0x7);
		iwl_write32(trans, FH_RSCSR_CHNL0_WPTR, q->write_actual);
	} else {
		struct iwl_trans_pcie *trans_pcie =
			IWL_TRANS_GET_PCIE_TRANS(trans);

		/*                                                      */
		if (test_bit(STATUS_TPOWER_PMI, &trans_pcie->status)) {
			reg = iwl_read32(trans, CSR_UCODE_DRV_GP1);

			if (reg & CSR_UCODE_DRV_GP1_BIT_MAC_SLEEP) {
				IWL_DEBUG_INFO(trans,
					"Rx queue requesting wakeup,"
					" GP1 = 0x%x\n", reg);
				iwl_set_bit(trans, CSR_GP_CNTRL,
					CSR_GP_CNTRL_REG_FLAG_MAC_ACCESS_REQ);
				goto exit_unlock;
			}

			q->write_actual = (q->write & ~0x7);
			iwl_write_direct32(trans, FH_RSCSR_CHNL0_WPTR,
					q->write_actual);

		/*                                    */
		} else {
			/*                                */
			q->write_actual = (q->write & ~0x7);
			iwl_write_direct32(trans, FH_RSCSR_CHNL0_WPTR,
				q->write_actual);
		}
	}
	q->need_update = 0;

 exit_unlock:
	spin_unlock_irqrestore(&q->lock, flags);
}

/*
                                                                 
  
                                                                
                                                                 
                                   
  
                                                                         
                                                                       
                 
 */
static void iwl_pcie_rxq_restock(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_rxq *rxq = &trans_pcie->rxq;
	struct iwl_rx_mem_buffer *rxb;
	unsigned long flags;

	/*
                                                                   
                                                                       
                                                                     
                                                                    
                                                              
                                                                
  */
	if (!test_bit(STATUS_DEVICE_ENABLED, &trans_pcie->status))
		return;

	spin_lock_irqsave(&rxq->lock, flags);
	while ((iwl_rxq_space(rxq) > 0) && (rxq->free_count)) {
		/*                                        */
		rxb = rxq->queue[rxq->write];
		BUG_ON(rxb && rxb->page);

		/*                                                */
		rxb = list_first_entry(&rxq->rx_free, struct iwl_rx_mem_buffer,
				       list);
		list_del(&rxb->list);

		/*                                                    */
		rxq->bd[rxq->write] = iwl_pcie_dma_addr2rbd_ptr(rxb->page_dma);
		rxq->queue[rxq->write] = rxb;
		rxq->write = (rxq->write + 1) & RX_QUEUE_MASK;
		rxq->free_count--;
	}
	spin_unlock_irqrestore(&rxq->lock, flags);
	/*                                                              
              */
	if (rxq->free_count <= RX_LOW_WATERMARK)
		schedule_work(&trans_pcie->rx_replenish);

	/*                                                                   
                                                        */
	if (rxq->write_actual != (rxq->write & ~0x7)) {
		spin_lock_irqsave(&rxq->lock, flags);
		rxq->need_update = 1;
		spin_unlock_irqrestore(&rxq->lock, flags);
		iwl_pcie_rxq_inc_wr_ptr(trans, rxq);
	}
}

/*
                                                             
  
                                                                               
                                                                             
                                                                              
                                                                                
                     
 */
static void iwl_pcie_rxq_alloc_rbs(struct iwl_trans *trans, gfp_t priority)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_rxq *rxq = &trans_pcie->rxq;
	struct iwl_rx_mem_buffer *rxb;
	struct page *page;
	unsigned long flags;
	gfp_t gfp_mask = priority;

	while (1) {
		spin_lock_irqsave(&rxq->lock, flags);
		if (list_empty(&rxq->rx_used)) {
			spin_unlock_irqrestore(&rxq->lock, flags);
			return;
		}
		spin_unlock_irqrestore(&rxq->lock, flags);

		if (rxq->free_count > RX_LOW_WATERMARK)
			gfp_mask |= __GFP_NOWARN;

		if (trans_pcie->rx_page_order > 0)
			gfp_mask |= __GFP_COMP;

		/*                            */
		page = alloc_pages(gfp_mask, trans_pcie->rx_page_order);
		if (!page) {
			if (net_ratelimit())
				IWL_DEBUG_INFO(trans, "alloc_pages failed, "
					   "order: %d\n",
					   trans_pcie->rx_page_order);

			if ((rxq->free_count <= RX_LOW_WATERMARK) &&
			    net_ratelimit())
				IWL_CRIT(trans, "Failed to alloc_pages with %s."
					 "Only %u free buffers remaining.\n",
					 priority == GFP_ATOMIC ?
					 "GFP_ATOMIC" : "GFP_KERNEL",
					 rxq->free_count);
			/*                                                   
                                                   
                                              */
			return;
		}

		spin_lock_irqsave(&rxq->lock, flags);

		if (list_empty(&rxq->rx_used)) {
			spin_unlock_irqrestore(&rxq->lock, flags);
			__free_pages(page, trans_pcie->rx_page_order);
			return;
		}
		rxb = list_first_entry(&rxq->rx_used, struct iwl_rx_mem_buffer,
				       list);
		list_del(&rxb->list);
		spin_unlock_irqrestore(&rxq->lock, flags);

		BUG_ON(rxb->page);
		rxb->page = page;
		/*                                */
		rxb->page_dma =
			dma_map_page(trans->dev, page, 0,
				     PAGE_SIZE << trans_pcie->rx_page_order,
				     DMA_FROM_DEVICE);
		if (dma_mapping_error(trans->dev, rxb->page_dma)) {
			rxb->page = NULL;
			spin_lock_irqsave(&rxq->lock, flags);
			list_add(&rxb->list, &rxq->rx_used);
			spin_unlock_irqrestore(&rxq->lock, flags);
			__free_pages(page, trans_pcie->rx_page_order);
			return;
		}
		/*                                          */
		BUG_ON(rxb->page_dma & ~DMA_BIT_MASK(36));
		/*                            */
		BUG_ON(rxb->page_dma & DMA_BIT_MASK(8));

		spin_lock_irqsave(&rxq->lock, flags);

		list_add_tail(&rxb->list, &rxq->rx_free);
		rxq->free_count++;

		spin_unlock_irqrestore(&rxq->lock, flags);
	}
}

static void iwl_pcie_rxq_free_rbs(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_rxq *rxq = &trans_pcie->rxq;
	int i;

	/*                                                     */
	for (i = 0; i < RX_FREE_BUFFERS + RX_QUEUE_SIZE; i++) {
		/*                                                             
                                                               */
		if (rxq->pool[i].page != NULL) {
			dma_unmap_page(trans->dev, rxq->pool[i].page_dma,
				       PAGE_SIZE << trans_pcie->rx_page_order,
				       DMA_FROM_DEVICE);
			__free_pages(rxq->pool[i].page,
				     trans_pcie->rx_page_order);
			rxq->pool[i].page = NULL;
		}
		list_add_tail(&rxq->pool[i].list, &rxq->rx_used);
	}
}

/*
                                                                        
  
                                                            
  
                                                      
                                                                             
 */
static void iwl_pcie_rx_replenish(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	unsigned long flags;

	iwl_pcie_rxq_alloc_rbs(trans, GFP_KERNEL);

	spin_lock_irqsave(&trans_pcie->irq_lock, flags);
	iwl_pcie_rxq_restock(trans);
	spin_unlock_irqrestore(&trans_pcie->irq_lock, flags);
}

static void iwl_pcie_rx_replenish_now(struct iwl_trans *trans)
{
	iwl_pcie_rxq_alloc_rbs(trans, GFP_ATOMIC);

	iwl_pcie_rxq_restock(trans);
}

static void iwl_pcie_rx_replenish_work(struct work_struct *data)
{
	struct iwl_trans_pcie *trans_pcie =
	    container_of(data, struct iwl_trans_pcie, rx_replenish);

	iwl_pcie_rx_replenish(trans_pcie->trans);
}

static int iwl_pcie_rx_alloc(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_rxq *rxq = &trans_pcie->rxq;
	struct device *dev = trans->dev;

	memset(&trans_pcie->rxq, 0, sizeof(trans_pcie->rxq));

	spin_lock_init(&rxq->lock);

	if (WARN_ON(rxq->bd || rxq->rb_stts))
		return -EINVAL;

	/*                                                                */
	rxq->bd = dma_zalloc_coherent(dev, sizeof(__le32) * RX_QUEUE_SIZE,
				      &rxq->bd_dma, GFP_KERNEL);
	if (!rxq->bd)
		goto err_bd;

	/*                                                       */
	rxq->rb_stts = dma_zalloc_coherent(dev, sizeof(*rxq->rb_stts),
					   &rxq->rb_stts_dma, GFP_KERNEL);
	if (!rxq->rb_stts)
		goto err_rb_stts;

	return 0;

err_rb_stts:
	dma_free_coherent(dev, sizeof(__le32) * RX_QUEUE_SIZE,
			  rxq->bd, rxq->bd_dma);
	rxq->bd_dma = 0;
	rxq->bd = NULL;
err_bd:
	return -ENOMEM;
}

static void iwl_pcie_rx_hw_init(struct iwl_trans *trans, struct iwl_rxq *rxq)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	u32 rb_size;
	const u32 rfdnlog = RX_QUEUE_SIZE_LOG; /*          */

	if (trans_pcie->rx_buf_size_8k)
		rb_size = FH_RCSR_RX_CONFIG_REG_VAL_RB_SIZE_8K;
	else
		rb_size = FH_RCSR_RX_CONFIG_REG_VAL_RB_SIZE_4K;

	/*             */
	iwl_write_direct32(trans, FH_MEM_RCSR_CHNL0_CONFIG_REG, 0);
	/*                          */
	iwl_write_direct32(trans, FH_MEM_RCSR_CHNL0_RBDCB_WPTR, 0);
	iwl_write_direct32(trans, FH_MEM_RCSR_CHNL0_FLUSH_RB_REQ, 0);
	iwl_write_direct32(trans, FH_RSCSR_CHNL0_RDPTR, 0);

	/*                                     */
	iwl_write_direct32(trans, FH_RSCSR_CHNL0_RBDCB_WPTR_REG, 0);

	/*                                                       */
	iwl_write_direct32(trans, FH_RSCSR_CHNL0_RBDCB_BASE_REG,
			   (u32)(rxq->bd_dma >> 8));

	/*                                                   */
	iwl_write_direct32(trans, FH_RSCSR_CHNL0_STTS_WPTR_REG,
			   rxq->rb_stts_dma >> 4);

	/*              
                                                                 
                                                
                                 
                          
                   
            
  */
	iwl_write_direct32(trans, FH_MEM_RCSR_CHNL0_CONFIG_REG,
			   FH_RCSR_RX_CONFIG_CHNL_EN_ENABLE_VAL |
			   FH_RCSR_CHNL0_RX_IGNORE_RXF_EMPTY |
			   FH_RCSR_CHNL0_RX_CONFIG_IRQ_DEST_INT_HOST_VAL |
			   rb_size|
			   (RX_RB_TIMEOUT << FH_RCSR_RX_CONFIG_REG_IRQ_RBTH_POS)|
			   (rfdnlog << FH_RCSR_RX_CONFIG_RBDCB_SIZE_POS));

	/*                                                        */
	iwl_write8(trans, CSR_INT_COALESCING, IWL_HOST_INT_TIMEOUT_DEF);

	/*                                                   */
	if (trans->cfg->host_interrupt_operation_mode)
		iwl_set_bit(trans, CSR_INT_COALESCING, IWL_HOST_INT_OPER_MODE);
}

int iwl_pcie_rx_init(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_rxq *rxq = &trans_pcie->rxq;
	int i, err;
	unsigned long flags;

	if (!rxq->bd) {
		err = iwl_pcie_rx_alloc(trans);
		if (err)
			return err;
	}

	spin_lock_irqsave(&rxq->lock, flags);
	INIT_LIST_HEAD(&rxq->rx_free);
	INIT_LIST_HEAD(&rxq->rx_used);

	INIT_WORK(&trans_pcie->rx_replenish,
		  iwl_pcie_rx_replenish_work);

	iwl_pcie_rxq_free_rbs(trans);

	for (i = 0; i < RX_QUEUE_SIZE; i++)
		rxq->queue[i] = NULL;

	/*                                                                
                                                  */
	rxq->read = rxq->write = 0;
	rxq->write_actual = 0;
	rxq->free_count = 0;
	memset(rxq->rb_stts, 0, sizeof(*rxq->rb_stts));
	spin_unlock_irqrestore(&rxq->lock, flags);

	iwl_pcie_rx_replenish(trans);

	iwl_pcie_rx_hw_init(trans, rxq);

	spin_lock_irqsave(&trans_pcie->irq_lock, flags);
	rxq->need_update = 1;
	iwl_pcie_rxq_inc_wr_ptr(trans, rxq);
	spin_unlock_irqrestore(&trans_pcie->irq_lock, flags);

	return 0;
}

void iwl_pcie_rx_free(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_rxq *rxq = &trans_pcie->rxq;
	unsigned long flags;

	/*                                                             
             */
	if (!rxq->bd) {
		IWL_DEBUG_INFO(trans, "Free NULL rx context\n");
		return;
	}

	cancel_work_sync(&trans_pcie->rx_replenish);

	spin_lock_irqsave(&rxq->lock, flags);
	iwl_pcie_rxq_free_rbs(trans);
	spin_unlock_irqrestore(&rxq->lock, flags);

	dma_free_coherent(trans->dev, sizeof(__le32) * RX_QUEUE_SIZE,
			  rxq->bd, rxq->bd_dma);
	rxq->bd_dma = 0;
	rxq->bd = NULL;

	if (rxq->rb_stts)
		dma_free_coherent(trans->dev,
				  sizeof(struct iwl_rb_status),
				  rxq->rb_stts, rxq->rb_stts_dma);
	else
		IWL_DEBUG_INFO(trans, "Free rxq->rb_stts which is NULL\n");
	rxq->rb_stts_dma = 0;
	rxq->rb_stts = NULL;
}

static void iwl_pcie_rx_handle_rb(struct iwl_trans *trans,
				struct iwl_rx_mem_buffer *rxb)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_rxq *rxq = &trans_pcie->rxq;
	struct iwl_txq *txq = &trans_pcie->txq[trans_pcie->cmd_queue];
	unsigned long flags;
	bool page_stolen = false;
	int max_len = PAGE_SIZE << trans_pcie->rx_page_order;
	u32 offset = 0;

	if (WARN_ON(!rxb))
		return;

	dma_unmap_page(trans->dev, rxb->page_dma, max_len, DMA_FROM_DEVICE);

	while (offset + sizeof(u32) + sizeof(struct iwl_cmd_header) < max_len) {
		struct iwl_rx_packet *pkt;
		struct iwl_device_cmd *cmd;
		u16 sequence;
		bool reclaim;
		int index, cmd_index, err, len;
		struct iwl_rx_cmd_buffer rxcb = {
			._offset = offset,
			._rx_page_order = trans_pcie->rx_page_order,
			._page = rxb->page,
			._page_stolen = false,
			.truesize = max_len,
		};

		pkt = rxb_addr(&rxcb);

		if (pkt->len_n_flags == cpu_to_le32(FH_RSCSR_FRAME_INVALID))
			break;

		IWL_DEBUG_RX(trans, "cmd at offset %d: %s (0x%.2x)\n",
			rxcb._offset, get_cmd_string(trans_pcie, pkt->hdr.cmd),
			pkt->hdr.cmd);

		len = le32_to_cpu(pkt->len_n_flags) & FH_RSCSR_FRAME_SIZE_MSK;
		len += sizeof(u32); /*                         */
		trace_iwlwifi_dev_rx(trans->dev, trans, pkt, len);
		trace_iwlwifi_dev_rx_data(trans->dev, trans, pkt, len);

		/*                                                           
                                        
                                                         
                                             
                                                           
                                                            */
		reclaim = !(pkt->hdr.sequence & SEQ_RX_FRAME);
		if (reclaim) {
			int i;

			for (i = 0; i < trans_pcie->n_no_reclaim_cmds; i++) {
				if (trans_pcie->no_reclaim_cmds[i] ==
							pkt->hdr.cmd) {
					reclaim = false;
					break;
				}
			}
		}

		sequence = le16_to_cpu(pkt->hdr.sequence);
		index = SEQ_TO_INDEX(sequence);
		cmd_index = get_cmd_index(&txq->q, index);

		if (reclaim)
			cmd = txq->entries[cmd_index].cmd;
		else
			cmd = NULL;

		err = iwl_op_mode_rx(trans->op_mode, &rxcb, cmd);

		if (reclaim) {
			kfree(txq->entries[cmd_index].free_buf);
			txq->entries[cmd_index].free_buf = NULL;
		}

		/*
                                                          
                                                          
   */

		if (reclaim) {
			/*                                                     
                                          
                          
                                             */
			if (!rxcb._page_stolen)
				iwl_pcie_hcmd_complete(trans, &rxcb, err);
			else
				IWL_WARN(trans, "Claim null rxb?\n");
		}

		page_stolen |= rxcb._page_stolen;
		offset += ALIGN(len, FH_RSCSR_FRAME_ALIGN);
	}

	/*                                               */
	if (page_stolen) {
		__free_pages(rxb->page, trans_pcie->rx_page_order);
		rxb->page = NULL;
	}

	/*                                                         
                                                          
                                  */
	spin_lock_irqsave(&rxq->lock, flags);
	if (rxb->page != NULL) {
		rxb->page_dma =
			dma_map_page(trans->dev, rxb->page, 0,
				     PAGE_SIZE << trans_pcie->rx_page_order,
				     DMA_FROM_DEVICE);
		if (dma_mapping_error(trans->dev, rxb->page_dma)) {
			/*
                                           
                                              
                          
    */
			__free_pages(rxb->page, trans_pcie->rx_page_order);
			rxb->page = NULL;
			list_add_tail(&rxb->list, &rxq->rx_used);
		} else {
			list_add_tail(&rxb->list, &rxq->rx_free);
			rxq->free_count++;
		}
	} else
		list_add_tail(&rxb->list, &rxq->rx_used);
	spin_unlock_irqrestore(&rxq->lock, flags);
}

/*
                                                                           
 */
static void iwl_pcie_rx_handle(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct iwl_rxq *rxq = &trans_pcie->rxq;
	u32 r, i;
	u8 fill_rx = 0;
	u32 count = 8;
	int total_empty;

	/*                                                                 
                                                                      */
	r = le16_to_cpu(ACCESS_ONCE(rxq->rb_stts->closed_rb_num)) & 0x0FFF;
	i = rxq->read;

	/*                                           */
	if (i == r)
		IWL_DEBUG_RX(trans, "HW = SW = %d\n", r);

	/*                                                             */
	total_empty = r - rxq->write_actual;
	if (total_empty < 0)
		total_empty += RX_QUEUE_SIZE;

	if (total_empty > (RX_QUEUE_SIZE / 2))
		fill_rx = 1;

	while (i != r) {
		struct iwl_rx_mem_buffer *rxb;

		rxb = rxq->queue[i];
		rxq->queue[i] = NULL;

		IWL_DEBUG_RX(trans, "rxbuf: HW = %d, SW = %d (%p)\n",
			     r, i, rxb);
		iwl_pcie_rx_handle_rb(trans, rxb);

		i = (i + 1) & RX_QUEUE_MASK;
		/*                                     
                                                */
		if (fill_rx) {
			count++;
			if (count >= 8) {
				rxq->read = i;
				iwl_pcie_rx_replenish_now(trans);
				count = 0;
			}
		}
	}

	/*                     */
	rxq->read = i;
	if (fill_rx)
		iwl_pcie_rx_replenish_now(trans);
	else
		iwl_pcie_rxq_restock(trans);
}

/*
                                                                            
 */
static void iwl_pcie_irq_handle_error(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	/*                                              */
	if (trans->cfg->internal_wimax_coex &&
	    (!(iwl_read_prph(trans, APMG_CLK_CTRL_REG) &
			     APMS_CLK_VAL_MRB_FUNC_MODE) ||
	     (iwl_read_prph(trans, APMG_PS_CTRL_REG) &
			    APMG_PS_CTRL_VAL_RESET_REQ))) {
		clear_bit(STATUS_HCMD_ACTIVE, &trans_pcie->status);
		iwl_op_mode_wimax_active(trans->op_mode);
		wake_up(&trans_pcie->wait_command_queue);
		return;
	}

	iwl_pcie_dump_csr(trans);
	iwl_pcie_dump_fh(trans, NULL);

	set_bit(STATUS_FW_ERROR, &trans_pcie->status);
	clear_bit(STATUS_HCMD_ACTIVE, &trans_pcie->status);
	wake_up(&trans_pcie->wait_command_queue);

	local_bh_disable();
	iwl_op_mode_nic_error(trans->op_mode);
	local_bh_enable();
}

irqreturn_t iwl_pcie_irq_handler(int irq, void *dev_id)
{
	struct iwl_trans *trans = dev_id;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	struct isr_statistics *isr_stats = &trans_pcie->isr_stats;
	u32 inta = 0;
	u32 handled = 0;
	unsigned long flags;
	u32 i;
#ifdef CONFIG_IWLWIFI_DEBUG
	u32 inta_mask;
#endif

	lock_map_acquire(&trans->sync_cmd_lockdep_map);

	spin_lock_irqsave(&trans_pcie->irq_lock, flags);

	/*                                          
                                                                      
  */
	/*                                                                 
                                                                    
                                                                   
                                                                     
                                                                    
                                                                    
                                               
  */
	iwl_write32(trans, CSR_INT,
		    trans_pcie->inta | ~trans_pcie->inta_mask);

	inta = trans_pcie->inta;

#ifdef CONFIG_IWLWIFI_DEBUG
	if (iwl_have_debug_level(IWL_DL_ISR)) {
		/*                */
		inta_mask = iwl_read32(trans, CSR_INT_MASK);
		IWL_DEBUG_ISR(trans, "inta 0x%08x, enabled 0x%08x\n",
			      inta, inta_mask);
	}
#endif

	/*                                                                    */
	trans_pcie->inta = 0;

	spin_unlock_irqrestore(&trans_pcie->irq_lock, flags);

	/*                                                  */
	if (inta & CSR_INT_BIT_HW_ERR) {
		IWL_ERR(trans, "Hardware error detected.  Restarting.\n");

		/*                                            */
		iwl_disable_interrupts(trans);

		isr_stats->hw++;
		iwl_pcie_irq_handle_error(trans);

		handled |= CSR_INT_BIT_HW_ERR;

		goto out;
	}

#ifdef CONFIG_IWLWIFI_DEBUG
	if (iwl_have_debug_level(IWL_DL_ISR)) {
		/*                                                            */
		if (inta & CSR_INT_BIT_SCD) {
			IWL_DEBUG_ISR(trans, "Scheduler finished to transmit "
				      "the frame/frames.\n");
			isr_stats->sch++;
		}

		/*                                                           */
		if (inta & CSR_INT_BIT_ALIVE) {
			IWL_DEBUG_ISR(trans, "Alive interrupt\n");
			isr_stats->alive++;
		}
	}
#endif
	/*                                                 */
	inta &= ~(CSR_INT_BIT_SCD | CSR_INT_BIT_ALIVE);

	/*                           */
	if (inta & CSR_INT_BIT_RF_KILL) {
		bool hw_rfkill;

		hw_rfkill = iwl_is_rfkill_set(trans);
		IWL_WARN(trans, "RF_KILL bit toggled to %s.\n",
			 hw_rfkill ? "disable radio" : "enable radio");

		isr_stats->rfkill++;

		iwl_op_mode_hw_rf_kill(trans->op_mode, hw_rfkill);
		if (hw_rfkill) {
			set_bit(STATUS_RFKILL, &trans_pcie->status);
			if (test_and_clear_bit(STATUS_HCMD_ACTIVE,
					       &trans_pcie->status))
				IWL_DEBUG_RF_KILL(trans,
						  "Rfkill while SYNC HCMD in flight\n");
			wake_up(&trans_pcie->wait_command_queue);
		} else {
			clear_bit(STATUS_RFKILL, &trans_pcie->status);
		}

		handled |= CSR_INT_BIT_RF_KILL;
	}

	/*                                     */
	if (inta & CSR_INT_BIT_CT_KILL) {
		IWL_ERR(trans, "Microcode CT kill error detected.\n");
		isr_stats->ctkill++;
		handled |= CSR_INT_BIT_CT_KILL;
	}

	/*                         */
	if (inta & CSR_INT_BIT_SW_ERR) {
		IWL_ERR(trans, "Microcode SW error detected. "
			" Restarting 0x%X.\n", inta);
		isr_stats->sw++;
		iwl_pcie_irq_handle_error(trans);
		handled |= CSR_INT_BIT_SW_ERR;
	}

	/*                                       */
	if (inta & CSR_INT_BIT_WAKEUP) {
		IWL_DEBUG_ISR(trans, "Wakeup interrupt\n");
		iwl_pcie_rxq_inc_wr_ptr(trans, &trans_pcie->rxq);
		for (i = 0; i < trans->cfg->base_params->num_of_queues; i++)
			iwl_pcie_txq_inc_wr_ptr(trans, &trans_pcie->txq[i]);

		isr_stats->wakeup++;

		handled |= CSR_INT_BIT_WAKEUP;
	}

	/*                                                             
                                                           
                                              */
	if (inta & (CSR_INT_BIT_FH_RX | CSR_INT_BIT_SW_RX |
		    CSR_INT_BIT_RX_PERIODIC)) {
		IWL_DEBUG_ISR(trans, "Rx interrupt\n");
		if (inta & (CSR_INT_BIT_FH_RX | CSR_INT_BIT_SW_RX)) {
			handled |= (CSR_INT_BIT_FH_RX | CSR_INT_BIT_SW_RX);
			iwl_write32(trans, CSR_FH_INT_STATUS,
					CSR_FH_INT_RX_MASK);
		}
		if (inta & CSR_INT_BIT_RX_PERIODIC) {
			handled |= CSR_INT_BIT_RX_PERIODIC;
			iwl_write32(trans,
				CSR_INT, CSR_INT_BIT_RX_PERIODIC);
		}
		/*                                                          
                
                                                      
                     
                                                           
                       
                                                                  
                                                       
                                                             
   */

		/*                                                           */
		iwl_write8(trans, CSR_INT_PERIODIC_REG,
			    CSR_INT_PERIODIC_DIS);

		iwl_pcie_rx_handle(trans);

		/*
                                                            
                                                               
                                                            
                                                              
                                                          
   */
		if (inta & (CSR_INT_BIT_FH_RX | CSR_INT_BIT_SW_RX))
			iwl_write8(trans, CSR_INT_PERIODIC_REG,
				   CSR_INT_PERIODIC_ENA);

		isr_stats->rx++;
	}

	/*                                                      */
	if (inta & CSR_INT_BIT_FH_TX) {
		iwl_write32(trans, CSR_FH_INT_STATUS, CSR_FH_INT_TX_MASK);
		IWL_DEBUG_ISR(trans, "uCode load interrupt\n");
		isr_stats->tx++;
		handled |= CSR_INT_BIT_FH_TX;
		/*                                                       */
		trans_pcie->ucode_write_complete = true;
		wake_up(&trans_pcie->ucode_write_waitq);
	}

	if (inta & ~handled) {
		IWL_ERR(trans, "Unhandled INTA bits 0x%08x\n", inta & ~handled);
		isr_stats->unhandled++;
	}

	if (inta & ~(trans_pcie->inta_mask)) {
		IWL_WARN(trans, "Disabled INTA bits 0x%08x were pending\n",
			 inta & ~trans_pcie->inta_mask);
	}

	/*                          */
	/*                                   */
	if (test_bit(STATUS_INT_ENABLED, &trans_pcie->status))
		iwl_enable_interrupts(trans);
	/*                                  */
	else if (handled & CSR_INT_BIT_RF_KILL)
		iwl_enable_rfkill_int(trans);

out:
	lock_map_release(&trans->sync_cmd_lockdep_map);
	return IRQ_HANDLED;
}

/*                                                                             
  
                
  
                                                                              */

/*                                          */
#define ICT_SHIFT	12
#define ICT_SIZE	(1 << ICT_SHIFT)
#define ICT_COUNT	(ICT_SIZE / sizeof(u32))

/*                 */
void iwl_pcie_free_ict(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	if (trans_pcie->ict_tbl) {
		dma_free_coherent(trans->dev, ICT_SIZE,
				  trans_pcie->ict_tbl,
				  trans_pcie->ict_tbl_dma);
		trans_pcie->ict_tbl = NULL;
		trans_pcie->ict_tbl_dma = 0;
	}
}

/*
                                                      
                     
                                                      
 */
int iwl_pcie_alloc_ict(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	trans_pcie->ict_tbl =
		dma_alloc_coherent(trans->dev, ICT_SIZE,
				   &trans_pcie->ict_tbl_dma,
				   GFP_KERNEL);
	if (!trans_pcie->ict_tbl)
		return -ENOMEM;

	/*                                                             */
	if (WARN_ON(trans_pcie->ict_tbl_dma & (ICT_SIZE - 1))) {
		iwl_pcie_free_ict(trans);
		return -EINVAL;
	}

	IWL_DEBUG_ISR(trans, "ict dma addr %Lx\n",
		      (unsigned long long)trans_pcie->ict_tbl_dma);

	IWL_DEBUG_ISR(trans, "ict vir addr %p\n", trans_pcie->ict_tbl);

	/*                                */
	memset(trans_pcie->ict_tbl, 0, ICT_SIZE);
	trans_pcie->ict_index = 0;

	/*                           */
	trans_pcie->inta_mask |= CSR_INT_BIT_RX_PERIODIC;
	return 0;
}

/*                                                              
                                                                
 */
void iwl_pcie_reset_ict(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	u32 val;
	unsigned long flags;

	if (!trans_pcie->ict_tbl)
		return;

	spin_lock_irqsave(&trans_pcie->irq_lock, flags);
	iwl_disable_interrupts(trans);

	memset(trans_pcie->ict_tbl, 0, ICT_SIZE);

	val = trans_pcie->ict_tbl_dma >> ICT_SHIFT;

	val |= CSR_DRAM_INT_TBL_ENABLE;
	val |= CSR_DRAM_INIT_TBL_WRAP_CHECK;

	IWL_DEBUG_ISR(trans, "CSR_DRAM_INT_TBL_REG =0x%x\n", val);

	iwl_write32(trans, CSR_DRAM_INT_TBL_REG, val);
	trans_pcie->use_ict = true;
	trans_pcie->ict_index = 0;
	iwl_write32(trans, CSR_INT, trans_pcie->inta_mask);
	iwl_enable_interrupts(trans);
	spin_unlock_irqrestore(&trans_pcie->irq_lock, flags);
}

/*                                                  */
void iwl_pcie_disable_ict(struct iwl_trans *trans)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	unsigned long flags;

	spin_lock_irqsave(&trans_pcie->irq_lock, flags);
	trans_pcie->use_ict = false;
	spin_unlock_irqrestore(&trans_pcie->irq_lock, flags);
}

/*                                                                 */
static irqreturn_t iwl_pcie_isr(int irq, void *data)
{
	struct iwl_trans *trans = data;
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);
	u32 inta, inta_mask;
#ifdef CONFIG_IWLWIFI_DEBUG
	u32 inta_fh;
#endif

	lockdep_assert_held(&trans_pcie->irq_lock);

	trace_iwlwifi_dev_irq(trans->dev);

	/*                                                    
                                                              
                                                                        
                                                                       */
	inta_mask = iwl_read32(trans, CSR_INT_MASK);
	iwl_write32(trans, CSR_INT_MASK, 0x00000000);

	/*                                              */
	inta = iwl_read32(trans, CSR_INT);

	if (inta & (~inta_mask)) {
		IWL_DEBUG_ISR(trans,
			      "We got a masked interrupt (0x%08x)...Ack and ignore\n",
			      inta & (~inta_mask));
		iwl_write32(trans, CSR_INT, inta & (~inta_mask));
		inta &= inta_mask;
	}

	/*                                                       
                                                      
                                                       */
	if (!inta) {
		IWL_DEBUG_ISR(trans, "Ignore interrupt, inta == 0\n");
		goto none;
	}

	if ((inta == 0xFFFFFFFF) || ((inta & 0xFFFFFFF0) == 0xa5a5a5a0)) {
		/*                                                   
                  */
		IWL_WARN(trans, "HARDWARE GONE?? INTA == 0x%08x\n", inta);
		return IRQ_HANDLED;
	}

#ifdef CONFIG_IWLWIFI_DEBUG
	if (iwl_have_debug_level(IWL_DL_ISR)) {
		inta_fh = iwl_read32(trans, CSR_FH_INT_STATUS);
		IWL_DEBUG_ISR(trans, "ISR inta 0x%08x, enabled 0x%08x, "
			      "fh 0x%08x\n", inta, inta_mask, inta_fh);
	}
#endif

	trans_pcie->inta |= inta;
	/*                                                       */
	if (likely(inta))
		return IRQ_WAKE_THREAD;
	else if (test_bit(STATUS_INT_ENABLED, &trans_pcie->status) &&
		 !trans_pcie->inta)
		iwl_enable_interrupts(trans);
	return IRQ_HANDLED;

none:
	/*                                                                    */
	/*                                                              */
	if (test_bit(STATUS_INT_ENABLED, &trans_pcie->status) &&
	    !trans_pcie->inta)
		iwl_enable_interrupts(trans);

	return IRQ_NONE;
}

/*                                                                   
                                                                            
                                                                          
                                                                         
                                                                           
                                                                              
             
 */
irqreturn_t iwl_pcie_isr_ict(int irq, void *data)
{
	struct iwl_trans *trans = data;
	struct iwl_trans_pcie *trans_pcie;
	u32 inta, inta_mask;
	u32 val = 0;
	u32 read;
	unsigned long flags;

	if (!trans)
		return IRQ_NONE;

	trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	spin_lock_irqsave(&trans_pcie->irq_lock, flags);

	/*                                  
                         
  */
	if (unlikely(!trans_pcie->use_ict)) {
		irqreturn_t ret = iwl_pcie_isr(irq, data);
		spin_unlock_irqrestore(&trans_pcie->irq_lock, flags);
		return ret;
	}

	trace_iwlwifi_dev_irq(trans->dev);

	/*                                                    
                                                           
                                                                     
                                                                      
  */
	inta_mask = iwl_read32(trans, CSR_INT_MASK);
	iwl_write32(trans, CSR_INT_MASK, 0x00000000);

	/*                                                       
                                                      
                                                       */
	read = le32_to_cpu(trans_pcie->ict_tbl[trans_pcie->ict_index]);
	trace_iwlwifi_dev_ict_read(trans->dev, trans_pcie->ict_index, read);
	if (!read) {
		IWL_DEBUG_ISR(trans, "Ignore interrupt, inta == 0\n");
		goto none;
	}

	/*
                                                                   
                                      
  */
	do {
		val |= read;
		IWL_DEBUG_ISR(trans, "ICT index %d value 0x%08X\n",
				trans_pcie->ict_index, read);
		trans_pcie->ict_tbl[trans_pcie->ict_index] = 0;
		trans_pcie->ict_index =
			iwl_queue_inc_wrap(trans_pcie->ict_index, ICT_COUNT);

		read = le32_to_cpu(trans_pcie->ict_tbl[trans_pcie->ict_index]);
		trace_iwlwifi_dev_ict_read(trans->dev, trans_pcie->ict_index,
					   read);
	} while (read);

	/*                                               */
	if (val == 0xffffffff)
		val = 0;

	/*
                                                                 
                                                                   
                                                                      
                                                             
                                                              
  */
	if (val & 0xC0000)
		val |= 0x8000;

	inta = (0xff & val) | ((0xff00 & val) << 16);
	IWL_DEBUG_ISR(trans, "ISR inta 0x%08x, enabled 0x%08x ict 0x%08x\n",
		      inta, inta_mask, val);

	inta &= trans_pcie->inta_mask;
	trans_pcie->inta |= inta;

	/*                                                               */
	if (likely(inta)) {
		spin_unlock_irqrestore(&trans_pcie->irq_lock, flags);
		return IRQ_WAKE_THREAD;
	} else if (test_bit(STATUS_INT_ENABLED, &trans_pcie->status) &&
		 !trans_pcie->inta) {
		/*                                                    
                                                              
                            
   */
		iwl_enable_interrupts(trans);
	}

	spin_unlock_irqrestore(&trans_pcie->irq_lock, flags);
	return IRQ_HANDLED;

 none:
	/*                                                                   
                                      
  */
	if (test_bit(STATUS_INT_ENABLED, &trans_pcie->status) &&
	    !trans_pcie->inta)
		iwl_enable_interrupts(trans);

	spin_unlock_irqrestore(&trans_pcie->irq_lock, flags);
	return IRQ_NONE;
}
