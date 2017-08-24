/*
 * xHCI host controller driver
 *
 * Copyright (C) 2008 Intel Corp.
 *
 * Author: Sarah Sharp
 * Some code borrowed from the Linux EHCI driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
                             
                                                                
                                                                          
                                                             
                                                                              
  
                       
                                                                                
                                                                              
                                          
                                                                           
                                                                               
                                                                            
                                                                          
                                     
                                                                         
                                 
  
                   
                                                                              
                                                         
                                                                               
                                                         
  
                  
                                               
                                                                              
                                                                          
                   
                                                                            
                                                                    
                                                                            
  
                  
                                                                              
                                       
                                                                        
                                                                               
                                                                         
                                                                                
                                                                     
 */

#include <linux/scatterlist.h>
#include <linux/slab.h>
#include "xhci.h"
#include "mtk-test-lib.h"
#include <linux/dma-mapping.h>

/*
                                                                              
                      
 */
dma_addr_t mtktest_xhci_trb_virt_to_dma(struct xhci_segment *seg,
		union xhci_trb *trb)
{
	unsigned long segment_offset;

	if (!seg || !trb || trb < seg->trbs)
		return 0;
	/*                */
	segment_offset = trb - seg->trbs;
	if (segment_offset > TRBS_PER_SEGMENT)
		return 0;
	return seg->dma + (segment_offset * sizeof(*trb));
}

/*                                                         
                                                                        
 */
static inline bool last_trb_on_last_seg(struct xhci_hcd *xhci, struct xhci_ring *ring,
		struct xhci_segment *seg, union xhci_trb *trb)
{
	if (ring == xhci->event_ring)
		return (trb == &seg->trbs[TRBS_PER_SEGMENT]) &&
			(seg->next == xhci->event_ring->first_seg);
	else
		return trb->link.control & LINK_TOGGLE;
}

/*                                                                           
                                                                             
             
 */
static inline int last_trb(struct xhci_hcd *xhci, struct xhci_ring *ring,
		struct xhci_segment *seg, union xhci_trb *trb)
{
	if (ring == xhci->event_ring)
		return trb == &seg->trbs[TRBS_PER_SEGMENT];
	else
		return (trb->link.control & TRB_TYPE_BITMASK) == TRB_TYPE(TRB_LINK);
}

static inline int enqueue_is_link_trb(struct xhci_ring *ring)
{
	struct xhci_link_trb *link = &ring->enqueue->link;
	return ((link->control & TRB_TYPE_BITMASK) == TRB_TYPE(TRB_LINK));
}

/*                                                                              
                                                                               
                                               
 */
static void next_trb(struct xhci_hcd *xhci,
		struct xhci_ring *ring,
		struct xhci_segment **seg,
		union xhci_trb **trb)
{
	if (last_trb(xhci, ring, *seg, *trb)) {
		*seg = (*seg)->next;
		*trb = ((*seg)->trbs);
	} else {
		(*trb)++;
	}
}

/*
                                                                   
                                                                                
 */
void mtktest_inc_deq(struct xhci_hcd *xhci, struct xhci_ring *ring, bool consumer)
{
	union xhci_trb *next = ++(ring->dequeue);
	unsigned long long addr;

	ring->deq_updates++;
	/*                                                                      
                                                                   
  */
	while (last_trb(xhci, ring, ring->deq_seg, next)) {
		if (consumer && last_trb_on_last_seg(xhci, ring, ring->deq_seg, next)) {
			ring->cycle_state = (ring->cycle_state ? 0 : 1);
			if (!in_interrupt())
				xhci_dbg(xhci, "Toggle cycle state for ring %p = %i\n",
						ring,
						(unsigned int) ring->cycle_state);
		}
		ring->deq_seg = ring->deq_seg->next;
		ring->dequeue = ring->deq_seg->trbs;
		next = ring->dequeue;
	}
	addr = (unsigned long long) mtktest_xhci_trb_virt_to_dma(ring->deq_seg, ring->dequeue);
	if (ring == xhci->event_ring)
		xhci_dbg(xhci, "Event ring deq = 0x%llx (DMA)\n", addr);
	else if (ring == xhci->cmd_ring)
		xhci_dbg(xhci, "Command ring deq = 0x%llx (DMA)\n", addr);
	else
		xhci_dbg(xhci, "Ring deq = 0x%llx (DMA)\n", addr);
}

/*
                                                                   
                                                                                
  
                                                                          
                                                                            
                                                                            
                                                                         
  
                                                                            
                                                                               
                                                                              
                                                                        
  
                                                               
                        
 */
static void inc_enq(struct xhci_hcd *xhci, struct xhci_ring *ring,
		bool consumer, bool more_trbs_coming)
{
	u32 chain;
	union xhci_trb *next;
	unsigned long long addr;

	chain = ring->enqueue->generic.field[3] & TRB_CHAIN;
	next = ++(ring->enqueue);

	ring->enq_updates++;
	/*                                                                      
                                                                   
  */
	while (last_trb(xhci, ring, ring->enq_seg, next)) {
		if (!consumer) {
			if (ring != xhci->event_ring) {
				/*
                                                    
                                               
                                             
                                                  
                                                    
                                     
     */
				if (!chain && !more_trbs_coming)
					break;

				/*                                         
                                                   
                                                 
     */
				if (!xhci_link_trb_quirk(xhci)) {
					next->link.control &= ~TRB_CHAIN;
					next->link.control |= chain;
				}
				/*                                    */
				wmb();
				next->link.control ^= TRB_CYCLE;
			}
			/*                                                   */
			if (last_trb_on_last_seg(xhci, ring, ring->enq_seg, next)) {
				ring->cycle_state = (ring->cycle_state ? 0 : 1);
				if (!in_interrupt())
					xhci_dbg(xhci, "Toggle cycle state for ring %p = %i\n",
							ring,
							(unsigned int) ring->cycle_state);
			}
		}
		ring->enq_seg = ring->enq_seg->next;
		ring->enqueue = ring->enq_seg->trbs;
		next = ring->enqueue;
	}
	addr = (unsigned long long) mtktest_xhci_trb_virt_to_dma(ring->enq_seg, ring->enqueue);
	if (ring == xhci->event_ring)
		xhci_dbg(xhci, "Event ring enq = 0x%p, 0x%llx (DMA)\n", ring->enqueue, addr);
	else if (ring == xhci->cmd_ring)
		xhci_dbg(xhci, "Command ring enq = 0x%p, 0x%llx (DMA)\n", ring->enqueue, addr);
	else
		xhci_dbg(xhci, "Ring enq = 0x%p 0x%llx (DMA)\n", ring->enqueue, addr);
}

/*
                                                                           
         
                                                                              
                          
 */
static int room_on_ring(struct xhci_hcd *xhci, struct xhci_ring *ring,
		unsigned int num_trbs)
{
	int i;
	union xhci_trb *enq = ring->enqueue;
	struct xhci_segment *enq_seg = ring->enq_seg;
	struct xhci_segment *cur_seg;
	unsigned int left_on_ring;

	/*                                                        
                                              */
	while (last_trb(xhci, ring, enq_seg, enq)) {
		enq_seg = enq_seg->next;
		enq = enq_seg->trbs;
	}

	/*                        */
	if (enq == ring->dequeue) {
		/*                     */
		left_on_ring = TRBS_PER_SEGMENT - 1;
		for (cur_seg = enq_seg->next; cur_seg != enq_seg;
				cur_seg = cur_seg->next)
			left_on_ring += TRBS_PER_SEGMENT - 1;

		/*                                       */
		left_on_ring -= 1;
		if (num_trbs > left_on_ring) {
			xhci_warn(xhci, "Not enough room on ring; "
					"need %u TRBs, %u TRBs left\n",
					num_trbs, left_on_ring);
			return 0;
		}
		return 1;
	}
	/*                                                */
	for (i = 0; i <= num_trbs; ++i) {
		if (enq == ring->dequeue)
			return 0;
		enq++;
		while (last_trb(xhci, ring, enq_seg, enq)) {
			enq_seg = enq_seg->next;
			enq = enq_seg->trbs;
		}
	}
	return 1;
}

void mtktest_xhci_set_hc_event_deq(struct xhci_hcd *xhci)
{
	u64 temp;
	dma_addr_t deq;

	deq = mtktest_xhci_trb_virt_to_dma(xhci->event_ring->deq_seg,
			xhci->event_ring->dequeue);
	if (deq == 0 && !in_interrupt())
		xhci_warn(xhci, "WARN something wrong with SW event ring "
				"dequeue ptr.\n");
	/*                                      */
	temp = xhci_read_64(xhci, &xhci->ir_set->erst_dequeue);
	temp &= ERST_PTR_MASK;
	/*                                                
                                          
  */
	temp &= ~ERST_EHB;
	xhci_dbg(xhci, "// Write event ring dequeue pointer, preserving EHB bit\n");
	xhci_write_64(xhci, ((u64) deq & (u64) ~ERST_PTR_MASK) | temp,
			&xhci->ir_set->erst_dequeue);
}

/*                                                                       */
void mtktest_xhci_ring_cmd_db(struct xhci_hcd *xhci)
{
	u32 temp;

	xhci_dbg(xhci, "Ding dong!\n");
	temp = xhci_readl(xhci, &xhci->dba->doorbell[0]) & DB_MASK;
	xhci_writel(xhci, temp | DB_TARGET_HOST, &xhci->dba->doorbell[0]);
#if 0
	/*                         */
	xhci_readl(xhci, &xhci->dba->doorbell[0]);
#endif
}

static void ring_ep_doorbell(struct xhci_hcd *xhci,
		unsigned int slot_id,
		unsigned int ep_index,
		unsigned int stream_id)
{
	struct xhci_virt_ep *ep;
	unsigned int ep_state;
	u32 field;
	__u32 __iomem *db_addr = &xhci->dba->doorbell[slot_id];

	ep = &xhci->devs[slot_id]->eps[ep_index];
	ep_state = ep->ep_state;
	/*                                                               
                                                                    
                                                                      
                                                                      
                                                   
                                                                 
  */
	if (!(ep_state & EP_HALT_PENDING) && !(ep_state & SET_DEQ_PENDING)
			&& !(ep_state & EP_HALTED)) {
		field = xhci_readl(xhci, db_addr) & DB_MASK;
		field |= EPI_TO_DB(ep_index) | STREAM_ID_TO_DB(stream_id);
		xhci_writel(xhci, field, db_addr);
	}
}

/*                                                   */
static void ring_doorbell_for_active_rings(struct xhci_hcd *xhci,
		unsigned int slot_id,
		unsigned int ep_index)
{
	unsigned int stream_id;
	struct xhci_virt_ep *ep;

	ep = &xhci->devs[slot_id]->eps[ep_index];

	/*                                                     */
	if (!(ep->ep_state & EP_HAS_STREAMS)) {
		if (!(list_empty(&ep->ring->td_list)))
			ring_ep_doorbell(xhci, slot_id, ep_index, 0);
		return;
	}

	for (stream_id = 1; stream_id < ep->stream_info->num_streams;
			stream_id++) {
		struct xhci_stream_info *stream_info = ep->stream_info;
		if (!list_empty(&stream_info->stream_rings[stream_id]->td_list))
			ring_ep_doorbell(xhci, slot_id, ep_index, stream_id);
	}
}

/*
                                                                  
                                                                               
                                                                    
 */
static struct xhci_segment *find_trb_seg(
		struct xhci_segment *start_seg,
		union xhci_trb	*trb, int *cycle_state)
{
	struct xhci_segment *cur_seg = start_seg;
	struct xhci_generic_trb *generic_trb;

	while (cur_seg->trbs > trb ||
			&cur_seg->trbs[TRBS_PER_SEGMENT - 1] < trb) {
		generic_trb = &cur_seg->trbs[TRBS_PER_SEGMENT - 1].generic;
		if ((generic_trb->field[3] & TRB_TYPE_BITMASK) ==
				TRB_TYPE(TRB_LINK) &&
				(generic_trb->field[3] & LINK_TOGGLE))
			*cycle_state = ~(*cycle_state) & 0x1;
		cur_seg = cur_seg->next;
		if (cur_seg == start_seg)
			/*                                     */
			return NULL;
	}
	return cur_seg;
}

/*
                                                            
                                                                   
                                                          
                                                                    
  
                             
                                                                                
                                                                 
                                                                               
                                                  
                                                                               
                                                                      
 */
void mtktest_xhci_find_new_dequeue_state(struct xhci_hcd *xhci,
		unsigned int slot_id, unsigned int ep_index,
		unsigned int stream_id, struct xhci_td *cur_td,
		struct xhci_dequeue_state *state)
{
	struct xhci_virt_device *dev = xhci->devs[slot_id];
	struct xhci_ring *ep_ring;
	struct xhci_generic_trb *trb;
	struct xhci_ep_ctx *ep_ctx;
	dma_addr_t addr;

	ep_ring = mtktest_xhci_triad_to_transfer_ring(xhci, slot_id,
			ep_index, stream_id);
	if (!ep_ring) {
		xhci_warn(xhci, "WARN can't find new dequeue state "
				"for invalid stream ID %u.\n",
				stream_id);
		return;
	}
	state->new_cycle_state = 0;
	xhci_dbg(xhci, "Finding segment containing stopped TRB.\n");
	state->new_deq_seg = find_trb_seg(cur_td->start_seg,
			dev->eps[ep_index].stopped_trb,
			&state->new_cycle_state);
	if (!state->new_deq_seg)
		BUG();
	/*                                                                 */
	xhci_dbg(xhci, "Finding endpoint context\n");
	ep_ctx = mtktest_xhci_get_ep_ctx(xhci, dev->out_ctx, ep_index);
	state->new_cycle_state = 0x1 & ep_ctx->deq;

	state->new_deq_ptr = cur_td->last_trb;
	xhci_dbg(xhci, "Finding segment containing last TRB in TD.\n");
	state->new_deq_seg = find_trb_seg(state->new_deq_seg,
			state->new_deq_ptr,
			&state->new_cycle_state);
	if (!state->new_deq_seg)
		BUG();

	trb = &state->new_deq_ptr->generic;
	if ((trb->field[3] & TRB_TYPE_BITMASK) == TRB_TYPE(TRB_LINK) &&
				(trb->field[3] & LINK_TOGGLE))
		state->new_cycle_state = ~(state->new_cycle_state) & 0x1;
	next_trb(xhci, ep_ring, &state->new_deq_seg, &state->new_deq_ptr);

	/*                                                          */
	xhci_dbg(xhci, "New dequeue segment = %p (virtual)\n",
			state->new_deq_seg);
	addr = mtktest_xhci_trb_virt_to_dma(state->new_deq_seg, state->new_deq_ptr);
	xhci_dbg(xhci, "New dequeue pointer = 0x%llx (DMA)\n",
			(unsigned long long) addr);
	xhci_dbg(xhci, "Setting dequeue pointer in internal ring state.\n");
	ep_ring->dequeue = state->new_deq_ptr;
	ep_ring->deq_seg = state->new_deq_seg;
}

static void td_to_noop(struct xhci_hcd *xhci, struct xhci_ring *ep_ring,
		struct xhci_td *cur_td)
{
	struct xhci_segment *cur_seg;
	union xhci_trb *cur_trb;

	for (cur_seg = cur_td->start_seg, cur_trb = cur_td->first_trb;
			true;
			next_trb(xhci, ep_ring, &cur_seg, &cur_trb)) {
		if ((cur_trb->generic.field[3] & TRB_TYPE_BITMASK) ==
				TRB_TYPE(TRB_LINK)) {
			/*                                   
                                
    */
			cur_trb->generic.field[3] &= ~TRB_CHAIN;
			xhci_dbg(xhci, "Cancel (unchain) link TRB\n");
			xhci_dbg(xhci, "Address = %p (0x%llx dma); "
					"in seg %p (0x%llx dma)\n",
					cur_trb,
					(unsigned long long)mtktest_xhci_trb_virt_to_dma(cur_seg, cur_trb),
					cur_seg,
					(unsigned long long)cur_seg->dma);
		} else {
			cur_trb->generic.field[0] = 0;
			cur_trb->generic.field[1] = 0;
			cur_trb->generic.field[2] = 0;
			/*                                         */
			cur_trb->generic.field[3] &= TRB_CYCLE;
			cur_trb->generic.field[3] |= TRB_TYPE(TRB_TR_NOOP);
			xhci_dbg(xhci, "Cancel TRB %p (0x%llx dma) "
					"in seg %p (0x%llx dma)\n",
					cur_trb,
					(unsigned long long)mtktest_xhci_trb_virt_to_dma(cur_seg, cur_trb),
					cur_seg,
					(unsigned long long)cur_seg->dma);
		}
		if (cur_trb == cur_td->last_trb)
			break;
	}
}

static int queue_set_tr_deq(struct xhci_hcd *xhci, int slot_id,
		unsigned int ep_index, unsigned int stream_id,
		struct xhci_segment *deq_seg,
		union xhci_trb *deq_ptr, u32 cycle_state);

void mtktest_xhci_queue_new_dequeue_state(struct xhci_hcd *xhci,
		unsigned int slot_id, unsigned int ep_index,
		unsigned int stream_id,
		struct xhci_dequeue_state *deq_state)
{
	struct xhci_virt_ep *ep = &xhci->devs[slot_id]->eps[ep_index];

	xhci_dbg(xhci, "Set TR Deq Ptr cmd, new deq seg = %p (0x%llx dma), "
			"new deq ptr = %p (0x%llx dma), new cycle = %u\n",
			deq_state->new_deq_seg,
			(unsigned long long)deq_state->new_deq_seg->dma,
			deq_state->new_deq_ptr,
			(unsigned long long)mtktest_xhci_trb_virt_to_dma(deq_state->new_deq_seg, deq_state->new_deq_ptr),
			deq_state->new_cycle_state);
	queue_set_tr_deq(xhci, slot_id, ep_index, stream_id,
			deq_state->new_deq_seg,
			deq_state->new_deq_ptr,
			(u32) deq_state->new_cycle_state);
	/*                                                          
                                                                 
                                                               
                 
  */
	ep->ep_state |= SET_DEQ_PENDING;
}

static inline void xhci_stop_watchdog_timer_in_irq(struct xhci_hcd *xhci,
		struct xhci_virt_ep *ep)
{
	ep->ep_state &= ~EP_HALT_PENDING;
	/*                                                                    
                                                                         
                                                           
  */
	if (del_timer(&ep->stop_cmd_timer))
		ep->stop_cmds_pending--;
}

/*                                                          */
static void xhci_giveback_urb_in_irq(struct xhci_hcd *xhci,
		struct xhci_td *cur_td, int status, char *adjective)
{
	struct usb_hcd *hcd = xhci_to_hcd(xhci);

	cur_td->urb->hcpriv = NULL;
	usb_hcd_unlink_urb_from_ep(hcd, cur_td->urb);
	xhci_dbg(xhci, "Giveback %s URB %p\n", adjective, cur_td->urb);

	spin_unlock(&xhci->lock);
	usb_hcd_giveback_urb(hcd, cur_td->urb, status);
	kfree(cur_td);
	spin_lock(&xhci->lock);
	xhci_dbg(xhci, "%s URB given back\n", adjective);
}

/*
                                                                           
                                                                          
  
                                                                        
                                                                                
                                                    
                                                                                
                                                       
 */
static void handle_stopped_endpoint(struct xhci_hcd *xhci,
		union xhci_trb *trb)
{
	unsigned int slot_id;
	unsigned int ep_index;
	struct xhci_ring *ep_ring;
	struct xhci_virt_ep *ep;
	struct list_head *entry;
	struct xhci_td *cur_td = NULL;
	struct xhci_td *last_unlinked_td;

	struct xhci_dequeue_state deq_state;

	memset(&deq_state, 0, sizeof(deq_state));
	slot_id = TRB_TO_SLOT_ID(trb->generic.field[3]);
	ep_index = TRB_TO_EP_INDEX(trb->generic.field[3]);
	ep = &xhci->devs[slot_id]->eps[ep_index];

	if (list_empty(&ep->cancelled_td_list)) {
		xhci_stop_watchdog_timer_in_irq(xhci, ep);
		ring_doorbell_for_active_rings(xhci, slot_id, ep_index);
		return;
	}

	/*                                                               
                                                                        
                                                                        
                                              
  */
	list_for_each(entry, &ep->cancelled_td_list) {
		cur_td = list_entry(entry, struct xhci_td, cancelled_td_list);
		xhci_dbg(xhci, "Cancelling TD starting at %p, 0x%llx (dma).\n",
				cur_td->first_trb,
				(unsigned long long)mtktest_xhci_trb_virt_to_dma(cur_td->start_seg, cur_td->first_trb));
		ep_ring = mtktest_xhci_urb_to_transfer_ring(xhci, cur_td->urb);
		if (!ep_ring) {
			/*                                                 
                                                     
                                                         
                                                     
                                                         
                                                          
                                                           
                                                     
                                                
                 
    */
			xhci_warn(xhci, "WARN Cancelled URB %p "
					"has invalid stream ID %u.\n",
					cur_td->urb,
					cur_td->urb->stream_id);
			goto remove_finished_td;
		}
		/*
                                                               
                                                             
   */
		if (cur_td == ep->stopped_td)
			mtktest_xhci_find_new_dequeue_state(xhci, slot_id, ep_index,
					cur_td->urb->stream_id,
					cur_td, &deq_state);
		else
			td_to_noop(xhci, ep_ring, cur_td);
remove_finished_td:
		/*
                                                                  
                                                               
                                                    
   */
		list_del(&cur_td->td_list);
	}
	last_unlinked_td = cur_td;
	xhci_stop_watchdog_timer_in_irq(xhci, ep);

	/*                                                                 */
	if (deq_state.new_deq_ptr && deq_state.new_deq_seg) {
		mtktest_xhci_queue_new_dequeue_state(xhci,
				slot_id, ep_index,
				ep->stopped_td->urb->stream_id,
				&deq_state);
		mtktest_xhci_ring_cmd_db(xhci);
	} else {
		/*                                                            */
		ring_doorbell_for_active_rings(xhci, slot_id, ep_index);
	}
	ep->stopped_td = NULL;
	ep->stopped_trb = NULL;

	/*
                                                                 
                                                                        
                                                                 
                                                                     
  */
	do {
		cur_td = list_entry(ep->cancelled_td_list.next,
				struct xhci_td, cancelled_td_list);
		list_del(&cur_td->cancelled_td_list);

		/*                            */
		/*                                                            
                                                           
   */
		xhci_giveback_urb_in_irq(xhci, cur_td, 0, "cancelled");

		/*                                                            
             
   */
		if (xhci->xhc_state & XHCI_STATE_DYING)
			return;
	} while (cur_td != last_unlinked_td);

	/*                                                         */
}

/*                                                                            
                                                                               
                                                                              
                                                                              
                       
  
                                                                               
                                                                              
                                                                                
                                                                           
                                                                            
                                                                            
                       
  
                                                                            
                                                                            
                                                                                
                     
 */
void mtktest_xhci_stop_endpoint_command_watchdog(unsigned long arg)
{
	struct xhci_hcd *xhci;
	struct xhci_virt_ep *ep;
	struct xhci_virt_ep *temp_ep;
	struct xhci_ring *ring;
	struct xhci_td *cur_td;
	int ret, i, j;

	ep = (struct xhci_virt_ep *) arg;
	xhci = ep->xhci;

	spin_lock(&xhci->lock);

	ep->stop_cmds_pending--;
	if (xhci->xhc_state & XHCI_STATE_DYING) {
		xhci_dbg(xhci, "Stop EP timer ran, but another timer marked "
				"xHCI as DYING, exiting.\n");
		spin_unlock(&xhci->lock);
		return;
	}
	if (!(ep->stop_cmds_pending == 0 && (ep->ep_state & EP_HALT_PENDING))) {
		xhci_dbg(xhci, "Stop EP timer ran, but no command pending, "
				"exiting.\n");
		spin_unlock(&xhci->lock);
		return;
	}

	xhci_warn(xhci, "xHCI host not responding to stop endpoint command.\n");
	xhci_warn(xhci, "Assuming host is dying, halting host.\n");
	/*                                                                 
                     
  */
	xhci->xhc_state |= XHCI_STATE_DYING;
	/*                                                                  */
	mtktest_xhci_quiesce(xhci);
	spin_unlock(&xhci->lock);

	ret = mtktest_xhci_halt(xhci);

	spin_lock(&xhci->lock);
	if (ret < 0) {
		/*                                                             
                                                               
                                                         
                                                                
                                                          
                                                                
                              
   */
		xhci_warn(xhci, "Non-responsive xHCI host is not halting.\n");
		xhci_warn(xhci, "Completing active URBs anyway.\n");
		/*                                                          
                                                                 
                                                                  
                              
   */
	}
	for (i = 0; i < MAX_HC_SLOTS; i++) {
		if (!xhci->devs[i])
			continue;
		for (j = 0; j < 31; j++) {
			temp_ep = &xhci->devs[i]->eps[j];
			ring = temp_ep->ring;
			if (!ring)
				continue;
			xhci_dbg(xhci, "Killing URBs for slot ID %u, "
					"ep index %u\n", i, j);
			while (!list_empty(&ring->td_list)) {
				cur_td = list_first_entry(&ring->td_list,
						struct xhci_td,
						td_list);
				list_del(&cur_td->td_list);
				if (!list_empty(&cur_td->cancelled_td_list))
					list_del(&cur_td->cancelled_td_list);
				xhci_giveback_urb_in_irq(xhci, cur_td,
						-ESHUTDOWN, "killed");
			}
			while (!list_empty(&temp_ep->cancelled_td_list)) {
				cur_td = list_first_entry(
						&temp_ep->cancelled_td_list,
						struct xhci_td,
						cancelled_td_list);
				list_del(&cur_td->cancelled_td_list);
				xhci_giveback_urb_in_irq(xhci, cur_td,
						-ESHUTDOWN, "killed");
			}
		}
	}
	spin_unlock(&xhci->lock);
	xhci_to_hcd(xhci)->state = HC_STATE_HALT;
	xhci_dbg(xhci, "Calling usb_hc_died()\n");
	usb_hc_died(xhci_to_hcd(xhci));
	xhci_dbg(xhci, "xHCI host controller is dead.\n");
}

/*
                                                                            
                                                                                
                                                                              
                                                                       
                         
 */
static void handle_set_deq_completion(struct xhci_hcd *xhci,
		struct xhci_event_cmd *event,
		union xhci_trb *trb)
{
	unsigned int slot_id;
	unsigned int ep_index;
	unsigned int stream_id;
	struct xhci_ring *ep_ring;
	struct xhci_virt_device *dev;
	struct xhci_ep_ctx *ep_ctx;
	struct xhci_slot_ctx *slot_ctx;

	slot_id = TRB_TO_SLOT_ID(trb->generic.field[3]);
	ep_index = TRB_TO_EP_INDEX(trb->generic.field[3]);
	stream_id = TRB_TO_STREAM_ID(trb->generic.field[2]);
	dev = xhci->devs[slot_id];

	ep_ring = mtktest_xhci_stream_id_to_ring(dev, ep_index, stream_id);
	if (!ep_ring) {
		xhci_warn(xhci, "WARN Set TR deq ptr command for "
				"freed stream ID %u\n",
				stream_id);
		/*                  */
		dev->eps[ep_index].ep_state &= ~SET_DEQ_PENDING;
		return;
	}

	ep_ctx = mtktest_xhci_get_ep_ctx(xhci, dev->out_ctx, ep_index);
	slot_ctx = mtktest_xhci_get_slot_ctx(xhci, dev->out_ctx);

	if (GET_COMP_CODE(event->status) != COMP_SUCCESS) {
		unsigned int ep_state;
		unsigned int slot_state;

		switch (GET_COMP_CODE(event->status)) {
		case COMP_TRB_ERR:
			xhci_warn(xhci, "WARN Set TR Deq Ptr cmd invalid because "
					"of stream ID configuration\n");
			break;
		case COMP_CTX_STATE:
			xhci_warn(xhci, "WARN Set TR Deq Ptr cmd failed due "
					"to incorrect slot or ep state.\n");
			ep_state = ep_ctx->ep_info;
			ep_state &= EP_STATE_MASK;
			slot_state = slot_ctx->dev_state;
			slot_state = GET_SLOT_STATE(slot_state);
			xhci_dbg(xhci, "Slot state = %u, EP state = %u\n",
					slot_state, ep_state);
			break;
		case COMP_EBADSLT:
			xhci_warn(xhci, "WARN Set TR Deq Ptr cmd failed because "
					"slot %u was not enabled.\n", slot_id);
			break;
		default:
			xhci_warn(xhci, "WARN Set TR Deq Ptr cmd with unknown "
					"completion code of %u.\n",
					GET_COMP_CODE(event->status));
			break;
		}
		/*                                                           
                                                                  
                                                                 
                                                         
                                                    
   */
	} else {
		xhci_dbg(xhci, "Successful Set TR Deq Ptr cmd, deq = @%08llx\n",
				ep_ctx->deq);
		if (mtktest_xhci_trb_virt_to_dma(dev->eps[ep_index].queued_deq_seg,
					dev->eps[ep_index].queued_deq_ptr) ==
				(ep_ctx->deq & ~(EP_CTX_CYCLE_MASK))) {
			/*                                                      
                                  
    */
			ep_ring->deq_seg = dev->eps[ep_index].queued_deq_seg;
			ep_ring->dequeue = dev->eps[ep_index].queued_deq_ptr;
		} else {
			xhci_warn(xhci, "Mismatch between completed Set TR Deq "
					"Ptr command & xHCI internal state.\n");
			xhci_warn(xhci, "ep deq seg = %p, deq ptr = %p\n",
					dev->eps[ep_index].queued_deq_seg,
					dev->eps[ep_index].queued_deq_ptr);
		}
	}

	dev->eps[ep_index].ep_state &= ~SET_DEQ_PENDING;
	dev->eps[ep_index].queued_deq_seg = NULL;
	dev->eps[ep_index].queued_deq_ptr = NULL;
	/*                                     */
	ring_doorbell_for_active_rings(xhci, slot_id, ep_index);
}

static void handle_reset_ep_completion(struct xhci_hcd *xhci,
		struct xhci_event_cmd *event,
		union xhci_trb *trb)
{
	int slot_id;
	unsigned int ep_index;

	slot_id = TRB_TO_SLOT_ID(trb->generic.field[3]);
	ep_index = TRB_TO_EP_INDEX(trb->generic.field[3]);
	/*                                                           
                      
  */
	xhci_dbg(xhci, "Ignoring reset ep completion code of %u\n",
			(unsigned int) GET_COMP_CODE(event->status));

	/*                                                                    
                                                                      
                                                                   
  */
	if (xhci->quirks & XHCI_RESET_EP_QUIRK) {
		xhci_dbg(xhci, "Queueing configure endpoint command\n");
		mtktest_xhci_queue_configure_endpoint(xhci,
				xhci->devs[slot_id]->in_ctx->dma, slot_id,
				false);
		mtktest_xhci_ring_cmd_db(xhci);
	} else {
		/*                                                         */
		xhci->devs[slot_id]->eps[ep_index].ep_state &= ~EP_HALTED;
		ring_doorbell_for_active_rings(xhci, slot_id, ep_index);
	}
}

/*                                                                          
                                                                            
                                                           
 */
static int handle_cmd_in_cmd_wait_list(struct xhci_hcd *xhci,
		struct xhci_virt_device *virt_dev,
		struct xhci_event_cmd *event)
{
	struct xhci_command *command;

	if (list_empty(&virt_dev->cmd_list))
		return 0;

	command = list_entry(virt_dev->cmd_list.next,
			struct xhci_command, cmd_list);
	if (xhci->cmd_ring->dequeue != command->command_trb)
		return 0;

	command->status =
		GET_COMP_CODE(event->status);
	list_del(&command->cmd_list);
	if (command->completion)
		complete(command->completion);
	else
		mtktest_xhci_free_command(xhci, command);
	return 1;
}

/*
                                                                               
                                                                                
                                                                          
             
 */
struct xhci_segment *mtktest_trb_in_td(struct xhci_segment *start_seg,
		union xhci_trb	*start_trb,
		union xhci_trb	*end_trb,
		dma_addr_t	suspect_dma)
{
	dma_addr_t start_dma;
	dma_addr_t end_seg_dma;
	dma_addr_t end_trb_dma;
	struct xhci_segment *cur_seg;

	start_dma = mtktest_xhci_trb_virt_to_dma(start_seg, start_trb);
	cur_seg = start_seg;
	printk(KERN_DEBUG "start_dma 0x%p\n", (void *)(unsigned long)start_dma);
	printk(KERN_DEBUG "start_seg 0x%p\n", start_seg);
	printk(KERN_DEBUG "start_trb 0x%p\n", start_trb);
	printk(KERN_DEBUG "end_trb 0x%p\n", end_trb);
	printk(KERN_DEBUG "suspect_dma 0x%p\n", (void *)(unsigned long)suspect_dma);

	do {
		if (start_dma == 0){
			printk(KERN_DEBUG "return NULL 1\n");
			return NULL;
		}
		/*                                                          */
		end_seg_dma = mtktest_xhci_trb_virt_to_dma(cur_seg,
				&cur_seg->trbs[TRBS_PER_SEGMENT - 1]);
		/*                                                        */
		end_trb_dma = mtktest_xhci_trb_virt_to_dma(cur_seg, end_trb);
		printk(KERN_DEBUG "end_trb_dma 0x%p\n", (void *)(unsigned long)end_trb_dma);
		if (end_trb_dma > 0) {
			/*                                                           */
			if (start_dma <= end_trb_dma) {
				if (suspect_dma >= start_dma && suspect_dma <= end_trb_dma)
					return cur_seg;
			} else {
				/*                          
                                     
     */
				if ((suspect_dma >= start_dma &&
							suspect_dma <= end_seg_dma) ||
						(suspect_dma >= cur_seg->dma &&
						 suspect_dma <= end_trb_dma))
					return cur_seg;
			}
			printk(KERN_DEBUG "return NULL 2\n");
			return NULL;
		} else {
			/*                                          */
			if (suspect_dma >= start_dma && suspect_dma <= end_seg_dma)
				return cur_seg;
		}
		cur_seg = cur_seg->next;
		start_dma = mtktest_xhci_trb_virt_to_dma(cur_seg, &cur_seg->trbs[0]);
	} while (cur_seg != start_seg);
	printk(KERN_DEBUG "return NULL 3\n");
	return NULL;
}

static void xhci_cleanup_halted_endpoint(struct xhci_hcd *xhci,
		unsigned int slot_id, unsigned int ep_index,
		unsigned int stream_id,
		struct xhci_td *td, union xhci_trb *event_trb)
{
	struct xhci_virt_ep *ep = &xhci->devs[slot_id]->eps[ep_index];
	ep->ep_state |= EP_HALTED;
	ep->stopped_td = td;
	ep->stopped_trb = event_trb;
	ep->stopped_stream = stream_id;

	mtktest_xhci_queue_reset_ep(xhci, slot_id, ep_index);
	mtktest_xhci_cleanup_stalled_ring(xhci, td->urb->dev, ep_index);

	ep->stopped_td = NULL;
	ep->stopped_trb = NULL;
	ep->stopped_stream = 0;

	mtktest_xhci_ring_cmd_db(xhci);
}

/*                                                                       
                                                                              
                                                                                
                                                                               
                                         
 */
static int xhci_requires_manual_halt_cleanup(struct xhci_hcd *xhci,
		struct xhci_ep_ctx *ep_ctx,
		unsigned int trb_comp_code)
{
	xhci_dbg(xhci, "check required to cleanup halt ep\n");
	xhci_dbg(xhci, "ep_info 0x%x\n", ep_ctx->ep_info);
	/*                                                             */
	if (trb_comp_code == COMP_TX_ERR ||
			trb_comp_code == COMP_BABBLE ||
			trb_comp_code == COMP_SPLIT_ERR)
		/*                                               
                                                      
                                                          
                                                   
              
   */
		if ((ep_ctx->ep_info & EP_STATE_MASK) == EP_STATE_HALTED){
			xhci_dbg(xhci, "Required to cleanup halt ep, comp code(%d)\n", trb_comp_code);
		return 1;
	}

	return 0;
}

int mtktest_xhci_is_vendor_info_code(struct xhci_hcd *xhci, unsigned int trb_comp_code)
{
	if (trb_comp_code >= 224 && trb_comp_code <= 255) {
		/*                                                
                           
   */
		xhci_dbg(xhci, "Vendor defined info completion code %u\n",
				trb_comp_code);
		xhci_dbg(xhci, "Treating code as success.\n");
		return 1;
	}
	return 0;
}

/*
                                                        
                                         
 */
static int finish_td(struct xhci_hcd *xhci, struct xhci_td *td,
	union xhci_trb *event_trb, struct xhci_transfer_event *event,
	struct xhci_virt_ep *ep, int *status, bool skip)
{
	struct xhci_virt_device *xdev;
	struct xhci_ring *ep_ring;
	unsigned int slot_id;
	int ep_index;
	struct urb *urb = NULL;
	struct xhci_ep_ctx *ep_ctx;
	int ret = 0;
	struct urb_priv	*urb_priv;
	u32 trb_comp_code;

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p\n", td, td->last_trb ) ;
	
	slot_id = TRB_TO_SLOT_ID(event->flags);
	xdev = xhci->devs[slot_id];
	ep_index = TRB_TO_EP_ID(event->flags) - 1;
	ep_ring = mtktest_xhci_dma_to_transfer_ring(ep, event->buffer);

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p\n", td, td->last_trb ) ;
	ep_ctx = mtktest_xhci_get_ep_ctx(xhci, xdev->out_ctx, ep_index);

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p\n", td, td->last_trb ) ;
	trb_comp_code = GET_COMP_CODE(event->transfer_len);

	if (skip)
		goto td_cleanup;

	if (trb_comp_code == COMP_STOP_INVAL ||
			trb_comp_code == COMP_STOP) {
		/*                                                           
                                                                 
                                                                
   */
		ep->stopped_td = td;
		ep->stopped_trb = event_trb;
		xhci_dbg(xhci, "trb_comp_code call COMP_STALL or COMP_STOP_INVAL, %d\n",trb_comp_code);
		return 0;
	} else {
		if (trb_comp_code == COMP_STALL) {
			/*                                            
                                                     
                                                           
                                                         
                                                        
                                             
    */
			ep->stopped_td = td;
			ep->stopped_trb = event_trb;
			ep->stopped_stream = ep_ring->stream_id;

			xhci_dbg(xhci, "trb_comp_code call COMP_STALL\n");
		} else if (xhci_requires_manual_halt_cleanup(xhci,
					ep_ctx, trb_comp_code)) {
			/*                                                 
                                                           
                                                          
                             
    */
			xhci_dbg(xhci, "Need to cleanup halt ep, do it\n");
			xhci_cleanup_halted_endpoint(xhci,
					slot_id, ep_index, ep_ring->stream_id,
					td, event_trb);
		} else {
			xhci_dbg(xhci, "inc dequeue of ring: dequeue(0x%p), td(0x%p), last_trb(0x%p)\n", ep_ring->dequeue, td, td->last_trb);
			/*                             */
			while (ep_ring->dequeue != td->last_trb)
				mtktest_inc_deq(xhci, ep_ring, false);
			mtktest_inc_deq(xhci, ep_ring, false);
		}
td_cleanup:
	/*                                 */
	urb = td->urb;
	urb_priv = urb->hcpriv;

	/*                                                 
                                                             
                                                            
                                                                 
                      
  */
	if (urb->actual_length > urb->transfer_buffer_length) {
		xhci_warn(xhci, "URB transfer length is wrong, "
				"xHC issue? req. len = %u, "
				"act. len = %u\n",
				urb->transfer_buffer_length,
				urb->actual_length);
		urb->actual_length = 0;
		if (td->urb->transfer_flags & URB_SHORT_NOT_OK)
			*status = -EREMOTEIO;
		else
			*status = 0;
	}
#if 0
	spin_lock(&ep_ring->lock);
	list_del(&td->td_list);
	spin_unlock(&ep_ring->lock);
#endif
	xhci_dbg(xhci,"td(0x%p), prev(0x%p), next(0x%p)\n", td, td->td_list.prev, td->td_list.next);

	//              

	list_del(&td->td_list);

	xhci_dbg(xhci,"list_del done\n");
	/*                                                          */
	if (!list_empty(&td->cancelled_td_list))
		list_del(&td->cancelled_td_list);

	urb_priv->td_cnt++;
	/*                                                 */
	if (urb_priv->td_cnt == urb_priv->length)
		ret = 1;
    }	
    xhci_dbg(xhci,"finish done\n");
	return ret;
}


/*
                                                            
 */

#define FINISH_TD_CALL	(0xFFFE)
static int process_ctrl_td(struct xhci_hcd *xhci, struct xhci_td *td,
	union xhci_trb *event_trb, struct xhci_transfer_event *event,
	struct xhci_virt_ep *ep, int *status)
{
	struct xhci_virt_device *xdev;
	struct xhci_ring *ep_ring;
	unsigned int slot_id;
	int ep_index;
	struct xhci_ep_ctx *ep_ctx;
	u32 trb_comp_code;
	int ret ;

	xhci_dbg(xhci, "process_ctrl_td========>\n");

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	
	slot_id = TRB_TO_SLOT_ID(event->flags);
	xdev = xhci->devs[slot_id];
	ep_index = TRB_TO_EP_ID(event->flags) - 1;
	ep_ring = mtktest_xhci_dma_to_transfer_ring(ep, event->buffer);

	xhci_dbg(xhci, "ep_ring is 0x%p\n", ep_ring ) ;
	
	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	
	ep_ctx = mtktest_xhci_get_ep_ctx(xhci, xdev->out_ctx, ep_index);

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	trb_comp_code = GET_COMP_CODE(event->transfer_len);

	mtktest_xhci_debug_trb(xhci, xhci->event_ring->dequeue);

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	switch (trb_comp_code) {
	case COMP_SUCCESS:
		if (event_trb == ep_ring->dequeue) {
			xhci_warn(xhci, "WARN: Success on ctrl setup TRB "
					"without IOC set??\n");
			*status = -ESHUTDOWN;
		} else if (event_trb != td->last_trb) {
#if 1
			xhci_warn(xhci, "WARN: Success on ctrl data TRB "
					"without IOC set??\n");
#endif
			*status = -ESHUTDOWN;
		} else {
			xhci_dbg(xhci, "Successful control transfer!\n");
			*status = 0;
		}
		xhci_dbg(xhci, "end of COMP_SUCCESS\n");
		break;
	case COMP_SHORT_TX:
		xhci_warn(xhci, "WARN: short transfer on control ep\n");
		*status = 0;
		break;
	case COMP_STOP_INVAL:
	case COMP_STOP:
		ret = finish_td(xhci, td, event_trb, event, ep, status, false);
		xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;

		//                          
		//            
		return FINISH_TD_CALL ;
	default:
		xhci_dbg(xhci, "TRB error code %u, "
				"halted endpoint index = %u\n",
				trb_comp_code, ep_index);
		if (!xhci_requires_manual_halt_cleanup(xhci,
					ep_ctx, trb_comp_code))
		break;
	/*                   */
	case COMP_STALL:
		/*                                                  */
		if (event_trb != ep_ring->dequeue &&
				event_trb != td->last_trb)
			td->urb->actual_length =
				td->urb->transfer_buffer_length
				- TRB_LEN(event->transfer_len);
		else
			td->urb->actual_length = 0;

		xhci_cleanup_halted_endpoint(xhci,
			slot_id, ep_index, 0, td, event_trb);

		xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
		ret = finish_td(xhci, td, event_trb, event, ep, status, true);

		xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;

		//                          
		//            
		return FINISH_TD_CALL ;
		//         
	}

	xhci_dbg(xhci, "process_ctrl_td========>2\n");

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	/*
                                                                
                                                    
  */
	if (event_trb != ep_ring->dequeue) {
		/*                                    */
		if (event_trb == td->last_trb) {
			if (td->urb->actual_length != 0) {
				/*                                            
     */
				if ((*status == -EINPROGRESS || *status == 0) &&
						(td->urb->transfer_flags
						 & URB_SHORT_NOT_OK))
					/*                                
               */
					*status = -EREMOTEIO;
			} else {
				td->urb->actual_length =
					td->urb->transfer_buffer_length;
			}
			xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
		} else {
		/*                                         */
			if (trb_comp_code != COMP_STOP_INVAL) {
				/*                                            */
				td->urb->actual_length =
					td->urb->transfer_buffer_length -
					TRB_LEN(event->transfer_len);
				xhci_dbg(xhci, "Waiting for status "
						"stage event\n");

				xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
				return 0;
			}
		}
	}

	xhci_dbg(xhci, "process_ctrl_td========>3\n");

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	
	
	ret = finish_td(xhci, td, event_trb, event, ep, status, false);

	xhci_dbg(xhci, "process_ctrl_td <========\n");
	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
		//                          

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
		
	//           
	return FINISH_TD_CALL ;
	//                                                                
}

/*
                                                                       
 */
static int process_bulk_intr_td(struct xhci_hcd *xhci, struct xhci_td *td,
	union xhci_trb *event_trb, struct xhci_transfer_event *event,
	struct xhci_virt_ep *ep, int *status)
{
	struct xhci_ring *ep_ring;
	union xhci_trb *cur_trb;
	struct xhci_segment *cur_seg;
	u32 trb_comp_code;
	int ret ;

	ep_ring = mtktest_xhci_dma_to_transfer_ring(ep, event->buffer);
	trb_comp_code = GET_COMP_CODE(event->transfer_len);

	switch (trb_comp_code) {
	case COMP_SUCCESS:
		/*                                                  */
		if (event_trb != td->last_trb) {
			xhci_warn(xhci, "WARN Successful completion "
					"on short TX\n");
			if (td->urb->transfer_flags & URB_SHORT_NOT_OK)
				*status = -EREMOTEIO;
			else
				*status = 0;
		} else {
			if (usb_endpoint_xfer_bulk(&td->urb->ep->desc))
				xhci_dbg(xhci, "Successful bulk "
						"transfer!\n");
			else
				xhci_dbg(xhci, "Successful interrupt "
						"transfer!\n");
			*status = 0;
		}
		break;
	case COMP_SHORT_TX:
		if (td->urb->transfer_flags & URB_SHORT_NOT_OK)
			*status = -EREMOTEIO;
		else
			*status = 0;
		break;
	default:
		/*                              */
		break;
	}
	xhci_dbg(xhci,
			"ep %#x - asked for %d bytes, "
			"%d bytes untransferred\n",
			td->urb->ep->desc.bEndpointAddress,
			td->urb->transfer_buffer_length,
			TRB_LEN(event->transfer_len));
	/*                                                           */
	if (event_trb == td->last_trb) {
		if (TRB_LEN(event->transfer_len) != 0) {
			td->urb->actual_length =
				td->urb->transfer_buffer_length -
				TRB_LEN(event->transfer_len);
			if (td->urb->transfer_buffer_length <
					td->urb->actual_length) {
				xhci_warn(xhci, "HC gave bad length "
						"of %d bytes left\n",
						TRB_LEN(event->transfer_len));
				td->urb->actual_length = 0;
				if (td->urb->transfer_flags & URB_SHORT_NOT_OK)
					*status = -EREMOTEIO;
				else
					*status = 0;
			}
			/*                                             */
			if (*status == -EINPROGRESS) {
				if (td->urb->transfer_flags & URB_SHORT_NOT_OK)
					*status = -EREMOTEIO;
				else
					*status = 0;
			}
		} else {
			td->urb->actual_length =
				td->urb->transfer_buffer_length;
			/*                                        
                                    
    */
			if (*status == -EREMOTEIO)
				*status = 0;
		}
	} else {
		/*                                                     
                                                   
   */
		td->urb->actual_length = 0;
		for (cur_trb = ep_ring->dequeue, cur_seg = ep_ring->deq_seg;
				cur_trb != event_trb;
				next_trb(xhci, ep_ring, &cur_seg, &cur_trb)) {
			if ((cur_trb->generic.field[3] &
			 TRB_TYPE_BITMASK) != TRB_TYPE(TRB_TR_NOOP) &&
			    (cur_trb->generic.field[3] &
			 TRB_TYPE_BITMASK) != TRB_TYPE(TRB_LINK))
				td->urb->actual_length +=
					TRB_LEN(cur_trb->generic.field[2]);
		}
		/*                                                    
                                                        
   */
		if (trb_comp_code != COMP_STOP_INVAL)
			td->urb->actual_length +=
				TRB_LEN(cur_trb->generic.field[2]) -
				TRB_LEN(event->transfer_len);
	}

	ret = finish_td(xhci, td, event_trb, event, ep, status, false);
	//                          
	
	//            
	return FINISH_TD_CALL ;

}


/*
                                                                       
 */
int process_isoc_td(struct xhci_hcd *xhci, struct xhci_td *td,
	union xhci_trb *event_trb, struct xhci_transfer_event *event,
	struct xhci_virt_ep *ep, int *status)
{
	struct xhci_ring *ep_ring;
	struct urb_priv *urb_priv;
	int idx;
	int len = 0;
	int skip_td = 0;
	union xhci_trb *cur_trb;
	struct xhci_segment *cur_seg;
	u32 trb_comp_code;
	int ret ;

	ep_ring = mtktest_xhci_dma_to_transfer_ring(ep, event->buffer);
	trb_comp_code = GET_COMP_CODE(event->transfer_len);
	urb_priv = td->urb->hcpriv;
	idx = urb_priv->td_cnt;

	/*                        */
	switch (trb_comp_code) {
	case COMP_SUCCESS:
		td->urb->iso_frame_desc[idx].status = 0;
		xhci_dbg(xhci, "Successful isoc transfer, idx(%d)!\n", idx);
		break;
	case COMP_SHORT_TX:
		if (td->urb->transfer_flags & URB_SHORT_NOT_OK)
			td->urb->iso_frame_desc[idx].status =
				 -EREMOTEIO;
		else
			td->urb->iso_frame_desc[idx].status = 0;
		break;
	case COMP_BW_OVER:
		td->urb->iso_frame_desc[idx].status = -ECOMM;
		skip_td = 1;
		break;
	case COMP_BUFF_OVER:
	case COMP_BABBLE:
		td->urb->iso_frame_desc[idx].status = -EOVERFLOW;
		skip_td = 1;
		break;
	case COMP_STALL:
		td->urb->iso_frame_desc[idx].status = -EPROTO;
		skip_td = 1;
		break;
	case COMP_STOP:
	case COMP_STOP_INVAL:
		break;
	default:
		td->urb->iso_frame_desc[idx].status = -1;
		break;
	}

	if (trb_comp_code == COMP_SUCCESS || skip_td == 1) {
		td->urb->iso_frame_desc[idx].actual_length =
			td->urb->iso_frame_desc[idx].length;
		td->urb->actual_length +=
			td->urb->iso_frame_desc[idx].length;
	} else {
		for (cur_trb = ep_ring->dequeue,
		     cur_seg = ep_ring->deq_seg; cur_trb != event_trb;
		     next_trb(xhci, ep_ring, &cur_seg, &cur_trb)) {
			if ((cur_trb->generic.field[3] &
			 TRB_TYPE_BITMASK) != TRB_TYPE(TRB_TR_NOOP) &&
			    (cur_trb->generic.field[3] &
			 TRB_TYPE_BITMASK) != TRB_TYPE(TRB_LINK))
				len +=
				    TRB_LEN(cur_trb->generic.field[2]);
		}
		len += TRB_LEN(cur_trb->generic.field[2]) -
			TRB_LEN(event->transfer_len);

		if (trb_comp_code != COMP_STOP_INVAL) {
			td->urb->iso_frame_desc[idx].actual_length = len;
			td->urb->actual_length += len;
		}
	}

	if ((idx == urb_priv->length - 1) && *status == -EINPROGRESS){
		*status = 0;
		
	}

	ret = finish_td(xhci, td, event_trb, event, ep, status, false);
	
	//                          
	//            
	return FINISH_TD_CALL ;

}

static void handle_cmd_completion(struct xhci_hcd *xhci,
		struct xhci_event_cmd *event)
{
	int slot_id;
	u64 cmd_dma;
	dma_addr_t cmd_dequeue_dma;
	union xhci_trb *trb;
	struct xhci_input_control_ctx *ctrl_ctx;
	struct xhci_virt_device *virt_dev;
	unsigned int ep_index, ret;
	struct xhci_ring *ep_ring;
	unsigned int ep_state, port_id;
	struct usb_hcd *hcd = xhci_to_hcd(xhci);
	struct usb_device *udev, *rhdev;
	struct xhci_slot_ctx *slot_ctx;
	u64 temp_64;
	struct urb *urb;
	struct usb_ctrlrequest *dr;
	struct usb_config_descriptor *desc;
	int status;

	slot_id = TRB_TO_SLOT_ID(event->flags);
	cmd_dma = event->cmd_trb;
	cmd_dequeue_dma = mtktest_xhci_trb_virt_to_dma(xhci->cmd_ring->deq_seg,
			xhci->cmd_ring->dequeue);
	trb = xhci->cmd_ring->dequeue;
	/*                                                               */
	if (cmd_dequeue_dma == 0) {
		xhci->error_bitmask |= 1 << 4;
		return;
	}
	/*                                                                  */
	if (cmd_dma != (u64) cmd_dequeue_dma) {
		xhci->error_bitmask |= 1 << 5;
		return;
	}
	xhci_err(xhci, "CMD(%d), Complete Code(%d), slot_id(%d)\n",
        TRB_FIELD_TO_TYPE(xhci->cmd_ring->dequeue->generic.field[3]), GET_COMP_CODE(event->status), slot_id);

	switch (xhci->cmd_ring->dequeue->generic.field[3] & TRB_TYPE_BITMASK) {
	case TRB_TYPE(TRB_CMD_NOOP):
		break;
	case TRB_TYPE(TRB_ENABLE_SLOT):
		if (GET_COMP_CODE(event->status) == COMP_SUCCESS){
			xhci_err(xhci, "Enable slot\n");
			g_slot_id = slot_id;
			g_cmd_status = CMD_DONE;
		}
		else{
			g_slot_id = 0;
			g_cmd_status = CMD_FAIL;
		}
		break;
	case TRB_TYPE(TRB_DISABLE_SLOT):
		if (GET_COMP_CODE(event->status) == COMP_SUCCESS){
			xhci_err(xhci, "Disable slot\n");
			g_slot_id = slot_id;
			g_cmd_status = CMD_DONE;
		}
		break;
	case TRB_TYPE(TRB_ADDR_DEV):
		if(GET_COMP_CODE(event->status) == COMP_SUCCESS){
			xhci_err(xhci, "Address Device\n");
			g_cmd_status = CMD_DONE;
		}
		else if(GET_COMP_CODE(event->status) == COMP_CMD_ABORT){
			xhci_err(xhci, "Address Device Aborted\n");
			g_cmd_status = CMD_DONE;
		}
		else{
			g_cmd_status = CMD_FAIL;
		}
		break;
	case TRB_TYPE(TRB_CONFIG_EP):
		if(GET_COMP_CODE(event->status) == COMP_SUCCESS){
			xhci_err(xhci, "config endpoint success\n");
			g_cmd_status = CMD_DONE;
		}
		else{
			g_cmd_status = CMD_FAIL;
		}
		break;
	case TRB_TYPE(TRB_RESET_DEV):
		if(GET_COMP_CODE(event->status) == COMP_SUCCESS){
			xhci_err(xhci, "reset dev success\n");
			g_cmd_status = CMD_DONE;
		}
		else{
			xhci_dbg(xhci, "reset dev failed, code: %d\n", GET_COMP_CODE(event->status));
			g_cmd_status = CMD_FAIL;
		}
		break;
	case TRB_TYPE(TRB_STOP_RING):
		xhci_err(xhci, "TRB_STOP_RING\n");
		//                                                                      
		if((((int)event->cmd_trb) & 0xff0) != g_cmd_ring_pointer1 && (((int)event->cmd_trb) & 0xff0) != g_cmd_ring_pointer2){
			xhci_err(xhci, "[DBG] handle stop ep command pointer not equal to enqueued pointer, enqueue 0x%x , 0x%x, event refer 0x%x\n"
				, g_cmd_ring_pointer1, g_cmd_ring_pointer2, (((int)event->cmd_trb) & 0xff0));
			while(1);
		}
		if(GET_COMP_CODE(event->status) == COMP_SUCCESS){
					xhci_dbg(xhci, "stop ring success\n");
					g_cmd_status = CMD_DONE;
				}
				else{
					xhci_dbg(xhci, "stop ring failed, code: %d\n", GET_COMP_CODE(event->status));
					g_cmd_status = CMD_FAIL;
		}

		break;
	case TRB_TYPE(TRB_SET_DEQ):
		xhci_err(xhci, "TRB_SET_DEQ\n");
		if(GET_COMP_CODE(event->status) == COMP_SUCCESS){
				ep_index = TRB_TO_EP_INDEX(trb->generic.field[3]);
				virt_dev = xhci->devs[slot_id];
				virt_dev->eps[ep_index].ep_state &= ~SET_DEQ_PENDING;
				g_cmd_status = CMD_DONE;
			}
			else{
				xhci_dbg(xhci, "stop ring failed, code: %d\n", GET_COMP_CODE(event->status));
				g_cmd_status = CMD_FAIL;
		}
		break;
	case TRB_TYPE(TRB_EVAL_CONTEXT):
		xhci_err(xhci, "TRB_EVAL_CONTEXT\n");
		if(GET_COMP_CODE(event->status) == COMP_SUCCESS){
				g_cmd_status = CMD_DONE;
			}
			else{
				xhci_dbg(xhci, "eval context, code: %d\n", GET_COMP_CODE(event->status));
				g_cmd_status = CMD_FAIL;
		}
		break;
	case TRB_TYPE(TRB_RESET_EP):
		ep_index = TRB_TO_EP_INDEX(trb->generic.field[3]);
		xhci->devs[slot_id]->eps[ep_index].ep_state &= ~EP_HALTED;
		g_cmd_status = CMD_DONE;
		break;
	default:
		if(GET_COMP_CODE(event->status) == COMP_CMD_STOP){
			xhci_err(xhci, "command ring stopped\n");
			g_cmd_status = CMD_DONE;
			return;
		}
#if 0
		case TRB_TYPE(TRB_COMPLETION):
		if(GET_COMP_CODE(event->status) == COMP_CMD_STOP){
			xhci_dbg(xhci, "stop command ring\n");
			g_cmd_status = CMD_DONE;
		}
		else{
			xhci_dbg(xhci, "stop command failed, code: %d\n", GET_COMP_CODE(event->status));
			g_cmd_status = CMD_FAIL;
		}
		break;
#endif
		/*                                              */
		xhci->error_bitmask |= 1 << 6;
		g_cmd_status = CMD_FAIL;
		break;
	}
	mtktest_inc_deq(xhci, xhci->cmd_ring, false);
}

void rh_port_clear_change(struct xhci_hcd *xhci, int port_id, int port_temp){
	u32 temp;
	u32 __iomem *addr;
	port_id--;

	addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id & 0xff);
	//                                     
	temp = port_temp;
	xhci_dbg(xhci, "to clear port change, actual port %d status  = 0x%x\n", port_id, temp);
	temp = mtktest_xhci_port_state_to_clear_change(temp);
	xhci_writel(xhci, temp, addr);
	temp = xhci_readl(xhci, addr);
	xhci_dbg(xhci, "clear port change, actual port %d status  = 0x%x\n", port_id, temp);
}


int rh_get_port_status(struct xhci_hcd *xhci, int port_id){
	u32 temp, temp1,status;
	u32 __iomem *addr;

	port_id--;
	status = 0;

	addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id & 0xff);
	temp = xhci_readl(xhci, addr);

	printk(KERN_ERR "[OTG_H][rh_get_port_status] read port status 0x%x\n", temp);
	
	/*                  */
	if (temp & PORT_CSC)
		status |= USB_PORT_STAT_C_CONNECTION << 16;
	if (temp & PORT_PEC)
		status |= USB_PORT_STAT_C_ENABLE << 16;
	if ((temp & PORT_OCC))
		status |= USB_PORT_STAT_C_OVERCURRENT << 16;
	if ((temp & PORT_RC))
		status |= USB_PORT_STAT_C_RESET << 16;
	if ((temp & PORT_PLC))
		status |= USB_PORT_STAT_C_SUSPEND << 16;
	/*
                                                           
           
  */
	if (temp & PORT_CONNECT) {
		status |= USB_PORT_STAT_CONNECTION;
		status |= mtktest_xhci_port_speed(temp);
	}
	if (temp & PORT_PE)
		status |= USB_PORT_STAT_ENABLE;
	if (temp & PORT_OC)
		status |= USB_PORT_STAT_OVERCURRENT;
	if (temp & PORT_RESET)
		status |= USB_PORT_STAT_RESET;
	if (temp & PORT_POWER)
		status |= USB_PORT_STAT_POWER;
	temp1 = mtktest_xhci_port_state_to_neutral(temp);
//                               
//                               

	xhci_err(xhci, "port_id(%d), state(0x%x), neutral(0x%x), status(0x%x)\n", port_id, temp, temp1, status);
#if 0
	put_unaligned(cpu_to_le32(status), (__le32 *) buf);
#endif
	return status;
}
extern void mtktest_disableXhciAllPortPower(struct xhci_hcd *xhci);
extern void phy_hsrx_reset(void);
extern void phy_hsrx_set(void);
static void handle_port_status(struct xhci_hcd *xhci,
		union xhci_trb *event)
{
	u32 port_id, temp, temp1;
	int ret, port_status;
	struct usb_hcd *hcd = xhci_to_hcd(xhci);
	u32 __iomem *addr;
	int port_index;
	struct xhci_port *port;
#if TEST_OTG
	u32 temp2;
#endif
	/*                                                                    */
	if (GET_COMP_CODE(event->generic.field[2]) != COMP_SUCCESS) {
		xhci_warn(xhci, "WARN: xHC returned failed port status event\n");
		xhci->error_bitmask |= 1 << 8;
	}
	/*                                                                */
	port_id = GET_PORT_ID(event->generic.field[0]);
	port_index = get_port_index(port_id);
	xhci_dbg(xhci, "Port Status Change Event for port %d\n", port_id);
	xhci_dbg(xhci, "port_index: %d\n", port_index);
	if(port_index >= RH_PORT_NUM){
		xhci_err(xhci, "[ERROR] RH_PORT_NUM not enough\n");
		return;
	}
	port = rh_port[port_index];
	port->port_id = port_id;
	addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*((port_id - 1) & 0xff);
	temp = xhci_readl(xhci, addr);
	printk(KERN_ERR "[OTG_H] port_status change event port_status 0x%x\n", temp);
	port_status = rh_get_port_status(xhci, port_id);
	printk(KERN_ERR "port_status %x\n", port_status);
//                                     

	if(port_status & (USB_PORT_STAT_C_CONNECTION << 16)){
		
		if(port_status & USB_PORT_STAT_CONNECTION){
			xhci_err(xhci, "[OTG_H]connect port status event, connected, port_id is %d\n", port_id);
			//      
			phy_hsrx_set();
			g_port_id = port_id;
			g_port_connect = true;			
			port->port_status = CONNECTED;
			g_otg_wait_con = false;
			mb() ;
			printk(KERN_ERR "[OTG_H] handle_port_status, connected, g_port_connect is %d\n", g_port_connect);
			if(!(port_status & USB_PORT_STAT_SUPER_SPEED)){
				if(g_hs_block_reset){//          
#if TEST_OTG
					port->port_status = ENABLED;
#endif
				}
				else{
					//             
					mb() ;
					addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id-1 & 0xff);
					temp1 = xhci_readl(xhci, addr);
					temp1 = mtktest_xhci_port_state_to_neutral(temp);
					xhci_err(xhci, "Before Reset command, the port status is 0x%x\n", temp1);
					
					if(!g_otg_dev_B)
						mdelay(150);
					//            
															
					temp1 = (temp1 | PORT_RESET);
					xhci_dbg(xhci, "Reset command\n");
					xhci_err(xhci, "Reset command\n");
					xhci_writel(xhci, temp1, addr);
					port->port_status = RESET;
				}
			}
			else{
				if(port->port_reenabled == 1){
					port->port_reenabled = 2;
				}
				if(g_device_reconnect == 1)
					g_device_reconnect = 2;
				g_speed = USB_SPEED_SUPER;
				addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id-1 & 0xff);
				temp1 = xhci_readl(xhci, addr);
				if(((temp1 & PORT_RESET) == 0) && (temp1 & PORT_PE) && (PORT_PLS(temp1) == 0)){
					port->port_status = ENABLED;
					port->port_speed = USB_SPEED_SUPER;
					xhci_err(xhci, "port set: port_id %d, port_status %d, port_speed %d\n"
						, port->port_id, port->port_status, port->port_speed);
					g_port_reset = true;
					mb() ;
				}
				else{
					xhci_err(xhci, "Super speed port enabled failed!!\n");
					xhci_dbg(xhci, "temp & PORT_RESET 0x%x\n", (temp & PORT_RESET));
					xhci_dbg(xhci, "temp & PORT_PE 0x%x\n", (temp & PORT_PE));
					xhci_dbg(xhci, "temp & PORT_PLS 0x%x\n", (PORT_PLS(temp)));
					g_port_reset = false;
					mb() ;
				}
			}
		}
		else{	//               
			xhci_err(xhci, "[OTG_H]connect port status event, disconnected\n");
//                     
			port->port_speed = 0;
			port->port_status = DISCONNECTED;
			if(port->port_reenabled == 0){
				port->port_reenabled = 1;
			}
			g_port_connect = false;
			g_port_reset = false;
			if(g_device_reconnect == 0)
				g_device_reconnect = 1;

			mb() ;
			printk(KERN_ERR "[OTG_H] handle_port_status, disconnect, g_port_connect is %d\n", g_port_connect);
#if TEST_OTG
			temp2 = readl(SSUSB_OTG_STS);
			//                                                          
			//     
			//                                                        
			//            
			mb() ;
			if (g_otg_hnp_become_host){ //                             
				//                                      
				//                                                                                            
			    //                                                                               
				//                                                                                                            
			}
#endif
		}

		g_otg_csc = true;  //                                                                 
		mb() ;
		
	}
	if((port_status & (USB_PORT_STAT_C_RESET << 16)) && (port_status & (USB_PORT_STAT_CONNECTION)) && !(port_status & USB_PORT_STAT_SUPER_SPEED)){
		xhci_dbg(xhci, "Change Reset\n");
		if(!(port_status & USB_PORT_STAT_RESET)){
			if(port_status & USB_PORT_STAT_LOW_SPEED){
				port->port_speed = USB_SPEED_LOW;
				g_speed = USB_SPEED_LOW;
				xhci_dbg(xhci, "Change Reset, speed low\n");
			}
			else if(port_status & USB_PORT_STAT_HIGH_SPEED){
				port->port_speed = USB_SPEED_HIGH;
				g_speed = USB_SPEED_HIGH;
				xhci_dbg(xhci, "Change Reset, speed high\n");
			}
			else{
				port->port_speed = USB_SPEED_FULL;
				g_speed = USB_SPEED_FULL;
				xhci_dbg(xhci, "Change Reset, speed full\n");
			}
			port->port_status = ENABLED;
			xhci_err(xhci, "port_reenabled(%d)\n", port->port_reenabled);
			if(port->port_reenabled == 1){
				port->port_reenabled = 2;
			}
			if(g_device_reconnect == 1)
				g_device_reconnect = 2;
			g_port_reset = true;
			mb() ;
		}
		else{
			g_port_reset = false;
			mb() ;
		}
	}
	else if((port_status & (USB_PORT_STAT_C_RESET << 16)) && (port_status & (USB_PORT_STAT_CONNECTION))
		&& (port_status & USB_PORT_STAT_SUPER_SPEED)){
		port->port_status = ENABLED;
	}
	else if((port_status & (USB_PORT_STAT_C_RESET << 16)) && (!(port_status & (USB_PORT_STAT_CONNECTION)))){
		//                                                    
		printk(KERN_ERR "[OTG_H] USB_PORT_STAT_C_RESET and \n");
		if(g_otg_hnp_become_host && g_otg_dev_B){  //                                                
			if(!g_otg_wait_con){
				while(readl(SSUSB_OTG_STS) & SSUSB_XHCI_MAS_DMA_REQ){
					msleep(1);
					printk(KERN_ERR "[OTG_H] DMA in used\n");
				}
				writel(SSUSB_CHG_B_ROLE_B, SSUSB_OTG_STS);
				printk(KERN_ERR "[OTG_H]alert device done\n");
			}
		}
		g_port_connect = false;
		mb() ;
		printk(KERN_ERR "[OTG_H] handle_port_status, USB_PORT_STAT_C_RESET, g_port_connect is %d\n", g_port_connect);
	}
	if(port_status & (USB_PORT_STAT_C_SUSPEND << 16)){
		xhci_dbg(xhci, "port link status changed, wake up \n");
		//             
	}
	if(port_status & (USB_PORT_STAT_C_OVERCURRENT << 16)){
		xhci_err(xhci, "port over current changed\n");
		g_port_occ = true;
		mb() ;
	}
	if ((temp & PORT_PLC) && (temp & PORT_PLS_MASK) == XDEV_RESUME){
		xhci_dbg(xhci, "Change resume\n");
		g_port_resume = 1;
		mb() ;
	}
	if ((temp & PORT_PLC)){
		xhci_dbg(xhci, "Change PLS(%d)\n", (temp & PORT_PLS_MASK) >> 5);
		g_port_plc = 1;
		mb() ;
	}
	rh_port_clear_change(xhci, port_id, temp);
	/*                                                            */
	mtktest_inc_deq(xhci, xhci->event_ring, true);
	mtktest_xhci_set_hc_event_deq(xhci);
}


/*
                                                                          
                                                                   
                                                                            
 */
static int handle_tx_event(struct xhci_hcd *xhci,
		struct xhci_transfer_event *event)
{
	struct xhci_virt_device *xdev;
	struct xhci_virt_ep *ep;
	struct xhci_ring *ep_ring;
	unsigned int slot_id;
	int ep_index;
	struct xhci_td *td = NULL;
	dma_addr_t event_dma;
	struct xhci_segment *event_seg;
	union xhci_trb *event_trb;
	struct urb *urb = NULL;
	int status = -EINPROGRESS;
	struct xhci_ep_ctx *ep_ctx;
	u32 trb_comp_code;
	int i, ret;
	char *tmp;

	trb_comp_code = GET_COMP_CODE(event->transfer_len);
	xhci_dbg(xhci, "Complete code(%d)\n", trb_comp_code);
#if 1
	if(trb_comp_code == COMP_UNDERRUN || trb_comp_code == COMP_OVERRUN){
		if(trb_comp_code == COMP_UNDERRUN){
			//                                               
		}
		else if(trb_comp_code == COMP_OVERRUN){
			//                                              
		}
		goto cleanup;
	}
#endif
#if 0
	if (trb_comp_code == COMP_STOP_INVAL ||
			trb_comp_code == COMP_STOP) {
		/*                                                           
                                                                 
                                                                
   */
		g_cmd_status = CMD_DONE;
		goto cleanup;
	}
#endif
#if 1
	slot_id = TRB_TO_SLOT_ID(event->flags);
	xdev = xhci->devs[slot_id];
	if (!xdev) {
		xhci_err(xhci, "[ERROR] Transfer event pointed to bad slot\n");
		return -ENODEV;
	}

	/*                                                 */
	ep_index = TRB_TO_EP_ID(event->flags) - 1;
	xhci_dbg(xhci, "ep index = %d\n", ep_index);
	ep = &xdev->eps[ep_index];
	ep_ring = mtktest_xhci_dma_to_transfer_ring(ep, event->buffer);
	ep_ctx = mtktest_xhci_get_ep_ctx(xhci, xdev->out_ctx, ep_index);
	if (!ep_ring || (ep_ctx->ep_info & EP_STATE_MASK) == EP_STATE_DISABLED) {
		xhci_err(xhci, "[ERROR] Transfer event for disabled endpoint "
				"or incorrect stream ring\n");		
		return -ENODEV;
	}

	event_dma = event->buffer;
	/*                                                                 */
	xhci_dbg(xhci, "checking for list empty\n");
	if (list_empty(&ep_ring->td_list)) {
		if(!g_test_random_stop_ep){
			xhci_warn(xhci, "WARN Event TRB for slot %d ep %d with no TDs queued?\n",
					TRB_TO_SLOT_ID(event->flags), ep_index);
			xhci_warn(xhci, "Event TRB(0x%p: 0x%llx 0x%x 0x%x\n"
				, event, event->buffer, event->transfer_len, event->flags);

			xhci_dbg(xhci, "Event TRB with TRB type ID %u\n",
					(unsigned int) (event->flags & TRB_TYPE_BITMASK)>>10);
			mtktest_xhci_print_trb_offsets(xhci, (union xhci_trb *) event);
		}
		urb = NULL;
		goto cleanup;
	}	
	xhci_dbg(xhci, "getting list entry\n");
	td = list_entry(ep_ring->td_list.next, struct xhci_td, td_list);
	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;

	/*                                              */
	event_seg = mtktest_trb_in_td(ep_ring->deq_seg, ep_ring->dequeue,
			td->last_trb, event_dma);

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	
	if (!event_seg) {
		/*                        */
		xhci_err(xhci, "[ERROR] Transfer event TRB DMA ptr not part of current TD\n");		
		return -ESHUTDOWN;
	}
	event_trb = &event_seg->trbs[(event_dma - event_seg->dma) / sizeof(*event_trb)];
	xhci_dbg(xhci, "deg_seg_dma(0x%p)trb(0x%p), dequeue(0x%p), td(0x%p), last_trb(0x%p), event_seg_dma(0x%p), event_dma(0x%p)trb(0x%p)\n",
            (void *)(unsigned long)ep_ring->deq_seg->dma, ep_ring->deq_seg->trbs, ep_ring->dequeue, td, td->last_trb, (void *)(unsigned long)event_seg->dma, (void *)(unsigned long)event_dma, event_trb);

	xhci_dbg(xhci, "Event TRB with TRB type ID %u\n",
			(unsigned int) (event->flags & TRB_TYPE_BITMASK)>>10);
	xhci_dbg(xhci, "Offset 0x00 (buffer lo) = 0x%x\n",
			lower_32_bits(event->buffer));
	xhci_dbg(xhci, "Offset 0x04 (buffer hi) = 0x%x\n",
			upper_32_bits(event->buffer));
	xhci_dbg(xhci, "Offset 0x08 (transfer length) = 0x%x\n",
			(unsigned int) event->transfer_len);
	xhci_dbg(xhci, "Offset 0x0C (flags) = 0x%x\n",
			(unsigned int) event->flags);

	/*                             */
	trb_comp_code = GET_COMP_CODE(event->transfer_len);

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
//                                           
	switch (trb_comp_code) {
	/*                                                      
                 
  */
	case COMP_SUCCESS:
#if 0
		if(usb_endpoint_xfer_control(&td->urb->ep->desc) && g_con_is_enter){
			udelay(g_con_delay_us);
			f_port_set_pls(g_port_id, g_con_enter_ux);
			udelay(100);
			f_port_set_pls((int)g_port_id, 0);
		}
#endif
		if(!usb_endpoint_xfer_isoc(&td->urb->ep->desc)){
			td->urb->actual_length = td->urb->transfer_buffer_length - GET_TRANSFER_LENGTH(event->transfer_len);
			status = 0;
			//                    
			xhci_dbg(xhci, "urb buffer len(%d), event trb len(%d)\n",
                                        td->urb->transfer_buffer_length, GET_TRANSFER_LENGTH(event->transfer_len));
		}
		break;
	case COMP_SHORT_TX:
		if(!usb_endpoint_xfer_isoc(&td->urb->ep->desc)){
			td->urb->actual_length = td->urb->transfer_buffer_length - GET_TRANSFER_LENGTH(event->transfer_len);
			status = 0;
			//                    
		}
		break;
	case COMP_STOP:
		xhci_dbg(xhci, "Stopped on Transfer TRB\n");
		break;
	case COMP_STOP_INVAL:
		xhci_dbg(xhci, "Stopped on No-op or Link TRB\n");
		break;
	case COMP_STALL:
		xhci_warn(xhci, "WARN: Stalled endpoint\n");
		ep->ep_state |= EP_HALTED;
		status = -EPIPE;
		//                         
		break;
	case COMP_TRB_ERR:
		xhci_warn(xhci, "WARN: TRB error on endpoint\n");
		status = -EILSEQ;
		//                          
		break;
	case COMP_SPLIT_ERR:
	case COMP_TX_ERR:
		xhci_warn(xhci, "WARN: transfer error on endpoint\n");
		status = -EPROTO;
		//                          
		break;
	case COMP_BABBLE:
		xhci_warn(xhci, "WARN: babble error on endpoint, ep_idx %d\n", ep_index);
		status = -EOVERFLOW;
		//                             
		break;
	case COMP_DB_ERR:
		xhci_warn(xhci, "WARN: HC couldn't access mem fast enough\n");
		status = -ENOSR;
		//                         
		break;
	case COMP_BW_OVER:
		xhci_warn(xhci, "WARN: bandwidth overrun event on endpoint\n");
		break;
	case COMP_BUFF_OVER:
		xhci_warn(xhci, "WARN: buffer overrun event on endpoint\n");
		break;
	case COMP_UNDERRUN:
		/*
                                                        
                                                       
                                           
   */
		xhci_dbg(xhci, "underrun event on endpoint\n");
		if (!list_empty(&ep_ring->td_list))
			xhci_dbg(xhci, "Underrun Event for slot %d ep %d "
					"still with TDs queued?\n",
				TRB_TO_SLOT_ID(event->flags), ep_index);
		break;
	case COMP_OVERRUN:
		xhci_dbg(xhci, "overrun event on endpoint\n");
		if (!list_empty(&ep_ring->td_list))
			xhci_dbg(xhci, "Overrun Event for slot %d ep %d "
					"still with TDs queued?\n",
				TRB_TO_SLOT_ID(event->flags), ep_index);
		break;
	case COMP_MISSED_INT:
		/*
                                                              
                          
                                                             
                                                       
   */
		xhci_dbg(xhci, "Miss service interval error, set skip flag\n");
		break;
	default:
		if (mtktest_xhci_is_vendor_info_code(xhci, trb_comp_code)) {
			//                
			status = 0 ;
			break;
		}
		xhci_warn(xhci, "ERROR Unknown event condition, HC probably busted, comp_code %d\n", trb_comp_code);
		urb = NULL;
		return -ENODEV;
	}
	/*                                                    
            
  */
	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	
	if (usb_endpoint_xfer_control(&td->urb->ep->desc)){
		ret = process_ctrl_td(xhci, td, event_trb, event, ep,
					 &status);
		xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	}else if (usb_endpoint_xfer_isoc(&td->urb->ep->desc)){
		ret = process_isoc_td(xhci, td, event_trb, event, ep,
					 &status);
	}else{
		ret = process_bulk_intr_td(xhci, td, event_trb, event,
					 ep, &status);
	}				
#if 0
	/*                             */
	while (ep_ring->dequeue != td->last_trb)
		mtktest_inc_deq(xhci, ep_ring, false);
	mtktest_inc_deq(xhci, ep_ring, false);
#endif

#if 0
	urb = td->urb;
	list_del(&td->td_list);
	if (usb_endpoint_xfer_control(&urb->ep->desc) ||
		(trb_comp_code != COMP_STALL &&
			trb_comp_code != COMP_BABBLE)) {
		kfree(td);
	}
#endif
	#if 0
	//          
	list_del(&td->td_list);
	/*                                                          */
	if (!list_empty(&td->cancelled_td_list))
		list_del(&td->cancelled_td_list);
	/*                                                           
                                                              
                                                         
                       
  */
	if (usb_endpoint_xfer_control(&urb->ep->desc) ||
		(trb_comp_code != COMP_STALL &&
			trb_comp_code != COMP_BABBLE)) {
		kfree(td);
	}
	#endif
#if 0
	if(dev_list[0] && dev_list[0]->slot_id==slot_id){
		xhci_dbg(xhci, "dev 0 slot_id %d trans done\n", slot_id);
		g_trans_status1 = TRANS_DONE;
	}
	else if(dev_list[1] && dev_list[1]->slot_id==slot_id){
		xhci_dbg(xhci, "dev 1 slot_id %d trans done\n", slot_id);
		g_trans_status2 = TRANS_DONE;
	}
	else if(dev_list[2] && dev_list[2]->slot_id==slot_id){
		xhci_dbg(xhci, "dev 2 slot_id %d trans done\n", slot_id);
		g_trans_status3 = TRANS_DONE;
	}
	else if(dev_list[3] && dev_list[3]->slot_id==slot_id){
		xhci_dbg(xhci, "dev 3 slot_id %d trans done\n", slot_id);
		g_trans_status4 = TRANS_DONE;
	}
#endif
#if 0
	if(ep_index == 1 || ep_index == 2 || (dev_list[0] && dev_list[0]->slot_id==slot_id)){
		g_trans_status1 = TRANS_DONE;
	}
	else if(ep_index == 3 || ep_index == 4 || (dev_list[1] && dev_list[1]->slot_id==slot_id)){
		g_trans_status2 = TRANS_DONE;
	}
	else if(ep_index == 5 || ep_index == 6 || (dev_list[2] && dev_list[2]->slot_id==slot_id)){
		g_trans_status3 = TRANS_DONE;
	}
	else if(ep_index == 7 || ep_index == 8 || (dev_list[3] && dev_list[3]->slot_id==slot_id)){
		g_trans_status4 = TRANS_DONE;
	}
#endif

#endif
cleanup:
	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	mtktest_inc_deq(xhci, xhci->event_ring, true);
	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
	mtktest_xhci_set_hc_event_deq(xhci);
	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p, td->urb->status is %d\n", td, td->last_trb , td->urb->status) ;
		
	//                                                             
	if (ret == FINISH_TD_CALL){
		td->urb->status = status;  //                                                                                        
	}

	/*                                                             */
	return 0;
}

int mtktest_xhci_handle_event(struct xhci_hcd *xhci){
	union xhci_trb *event;
	struct xhci_generic_trb *generic_event;
	int update_ptrs = 1;
	int ret;
		
	event = xhci->event_ring->dequeue;

	printk(KERN_ERR "[OTG_H]mtktest_xhci_handle_event: %x\n", event->event_cmd.flags);
#if 1
	if(g_event_full){
		struct xhci_generic_trb *event_trb = &event->generic;
		if(GET_COMP_CODE(event_trb->field[2]) == COMP_ER_FULL){
			xhci_dbg(xhci, "Got event ring full\n");
			g_got_event_full = true;
		}
		else{
			xhci_dbg(xhci, "increase SW dequeue pointer\n");
			mtktest_inc_deq(xhci, xhci->event_ring, true);
			return 0;
		}
	}
#endif
	if((event->event_cmd.flags & TRB_TYPE_BITMASK) == TRB_TYPE(TRB_MFINDEX_WRAP)){
		g_mfindex_event++;
	}
	/*                                */
	if ((event->event_cmd.flags & TRB_CYCLE) !=
			xhci->event_ring->cycle_state) {
		xhci->error_bitmask |= 1 << 2;
		return 0;
	}
	xhci_dbg(xhci, "%s - OS owns TRB\n", __func__);

	xhci_dbg(xhci, "TRB TYPE(0x%x)\n", TRB_FIELD_TO_TYPE(event->event_cmd.flags));
	/*                                 */
	switch ((event->event_cmd.flags & TRB_TYPE_BITMASK)) {
	case TRB_TYPE(TRB_COMPLETION):
#if 1
		xhci_dbg(xhci, "%s - calling handle_cmd_completion\n", __func__);
		handle_cmd_completion(xhci, &event->event_cmd);
		xhci_dbg(xhci, "%s - returned from handle_cmd_completion\n", __func__);
#endif
		break;

	case TRB_TYPE(TRB_PORT_STATUS):
#if 1
		xhci_dbg(xhci, "%s - calling handle_port_status\n", __func__);
		handle_port_status(xhci, event);
		xhci_dbg(xhci, "%s - returned from handle_port_status\n", __func__);
		update_ptrs = 0;
#endif
		break;
	case TRB_TYPE(TRB_TRANSFER):
#if 1
		xhci_dbg(xhci, "%s - calling handle_tx_event\n", __func__);
		ret = handle_tx_event(xhci, &event->trans_event);
		xhci_dbg(xhci, "%s - returned from handle_tx_event\n", __func__);
		if (ret < 0)
			xhci->error_bitmask |= 1 << 9;
		else
			update_ptrs = 0;
#endif
		break;
	case TRB_TYPE(TRB_DEV_NOTE):
		xhci_dbg(xhci, "Got device notification packet\n");
		generic_event  = &event->generic;
		xhci_dbg(xhci, "fields 0x%x 0x%x 0x%x 0x%x\n"
			, generic_event->field[0], generic_event->field[1], generic_event->field[2], generic_event->field[3]);
		g_dev_notification = TRB_DEV_NOTE_TYEP(generic_event->field[0]);
		xhci_dbg(xhci, "notification type %d\n", g_dev_notification);
		g_dev_not_value = TRB_DEV_NOTE_VALUE_LO(generic_event->field[0])
			| (generic_event->field[1] << 32);
		xhci_dbg(xhci, "notification value %ld\n", g_dev_not_value);
		break;
	default:
		break;
#if 0
		if ((event->event_cmd.flags & TRB_TYPE_BITMASK) >= TRB_TYPE(48))
			handle_vendor_event(xhci, event);
		else
			xhci->error_bitmask |= 1 << 3;
#endif
	}
	/*                                                                      
                                                                         
  */
	if (update_ptrs) {
		/*                                             */
		mtktest_inc_deq(xhci, xhci->event_ring, true);
		mtktest_xhci_set_hc_event_deq(xhci);
	}
	/*                                         */
	//          
	//                                 
	return 1;
}


/*                                 */

/*
                                                 
                                                                      
  
                                                               
                        
 */
static void queue_trb(struct xhci_hcd *xhci, struct xhci_ring *ring,
		bool consumer, bool more_trbs_coming,
		u32 field1, u32 field2, u32 field3, u32 field4)
{
	struct xhci_generic_trb *trb;

	trb = &ring->enqueue->generic;
	trb->field[0] = field1;
	trb->field[1] = field2;
	trb->field[2] = field3;
	trb->field[3] = field4;
	xhci_dbg(xhci, "Dump TRB: 0x%x 0x%x 0x%x 0x%x\n", trb->field[0], trb->field[1], trb->field[2], trb->field[3]);
	inc_enq(xhci, ring, consumer, more_trbs_coming);
	
}

/*
                                                                                  
                                               
 */
static int prepare_ring(struct xhci_hcd *xhci, struct xhci_ring *ep_ring,
		u32 ep_state, unsigned int num_trbs, gfp_t mem_flags)
{
	/*                                                       */
	xhci_dbg(xhci, "Endpoint state = 0x%x\n", ep_state);
	switch (ep_state) {
	case EP_STATE_DISABLED:
		/*
                                                             
                                              
   */
		xhci_warn(xhci, "WARN urb submitted to disabled ep\n");
		return -ENOENT;
	case EP_STATE_ERROR:
		xhci_warn(xhci, "WARN waiting for error on ep to be cleared\n");
		/*                                                       */
		/*                                               */
		return -EINVAL;
	case EP_STATE_HALTED:
		xhci_dbg(xhci, "WARN halted endpoint, queueing URB anyway.\n");
	case EP_STATE_STOPPED:
	case EP_STATE_RUNNING:
		break;
	default:
		xhci_err(xhci, "[ERROR] unknown endpoint state for ep\n");
		/*
                                                                
                             
   */
		return -EINVAL;
	}
	if (!room_on_ring(xhci, ep_ring, num_trbs)) {
		/*                          */
		xhci_err(xhci, "[ERROR] no room on ep ring, num_trbs %d\n", num_trbs);
		return -ENOMEM;
	}

	if (enqueue_is_link_trb(ep_ring)) {
		struct xhci_ring *ring = ep_ring;
		union xhci_trb *next;

		xhci_dbg(xhci, "prepare_ring: pointing to link trb\n");
		next = ring->enqueue;

		while (last_trb(xhci, ring, ring->enq_seg, next)) {

			/*                                         
                          
    */
			if (!xhci_link_trb_quirk(xhci))
				next->link.control &= ~TRB_CHAIN;
			else
				next->link.control |= TRB_CHAIN;

			wmb();
			next->link.control ^= (u32) TRB_CYCLE;

			/*                                                   */
			if (last_trb_on_last_seg(xhci, ring, ring->enq_seg, next)) {
				ring->cycle_state = (ring->cycle_state ? 0 : 1);
				if (!in_interrupt()) {
					xhci_dbg(xhci, "queue_trb: Toggle cycle "
						"state for ring %p = %i\n",
						ring, (unsigned int)ring->cycle_state);
				}
			}
			ring->enq_seg = ring->enq_seg->next;
			ring->enqueue = ring->enq_seg->trbs;
			next = ring->enqueue;
		}
	}

	return 0;
}

static int prepare_transfer(struct xhci_hcd *xhci,
		struct xhci_virt_device *xdev,
		unsigned int ep_index,
		unsigned int stream_id,
		unsigned int num_trbs,
		struct urb *urb,
		unsigned int td_index,
		gfp_t mem_flags)
{
	int ret;
	struct urb_priv *urb_priv;
	struct xhci_td	*td;
	struct xhci_ring *ep_ring;
	struct xhci_ep_ctx *ep_ctx = mtktest_xhci_get_ep_ctx(xhci, xdev->out_ctx, ep_index);

	ep_ring = mtktest_xhci_stream_id_to_ring(xdev, ep_index, stream_id);
	if (!ep_ring) {
		xhci_dbg(xhci, "Can't prepare ring for bad stream ID %u\n",
				stream_id);
		return -EINVAL;
	}
	xhci_dbg(xhci, "prepare transfer EP[%d]\n", ep_index);
	ret = prepare_ring(xhci, ep_ring,
			ep_ctx->ep_info & EP_STATE_MASK,
			num_trbs, mem_flags);
	if (ret)
		return ret;

	urb_priv = urb->hcpriv;
	td = urb_priv->td[td_index];

	INIT_LIST_HEAD(&td->td_list);
	INIT_LIST_HEAD(&td->cancelled_td_list);

	td->urb = urb;
	list_add_tail(&td->td_list, &ep_ring->td_list);
	xhci_dbg(xhci, "td(0x%p), prev(0x%p), next(0x%p)\n", td, td->td_list.prev, td->td_list.next);
	td->start_seg = ep_ring->enq_seg;
	td->first_trb = ep_ring->enqueue;

	urb_priv->td[td_index] = td;

	return 0;
}

static unsigned int count_sg_trbs_needed(struct xhci_hcd *xhci, struct urb *urb)
{
	int num_sgs, num_trbs, running_total, temp, i;
	struct scatterlist *sg;

	sg = NULL;
	num_sgs = urb->num_sgs;
	temp = urb->transfer_buffer_length;

	xhci_dbg(xhci, "count sg list trbs: \n");
	num_trbs = 0;
	for_each_sg(urb->sg, sg, num_sgs, i) {
		unsigned int previous_total_trbs = num_trbs;
		unsigned int len = sg_dma_len(sg);

		/*                                                       */
		running_total = TRB_MAX_BUFF_SIZE -
			(sg_dma_address(sg) & ((1 << TRB_MAX_BUFF_SHIFT) - 1));
		if (running_total != 0)
			num_trbs++;

		/*                                                            */
		while (running_total < sg_dma_len(sg)) {
			num_trbs++;
			running_total += TRB_MAX_BUFF_SIZE;
		}
		xhci_dbg(xhci, " sg #%d: dma = %#llx, len = %#x (%d), num_trbs = %d\n",
				i, (unsigned long long)sg_dma_address(sg),
				len, len, num_trbs - previous_total_trbs);

		len = min_t(int, len, temp);
		temp -= len;
		if (temp == 0)
			break;
	}
	xhci_dbg(xhci, "\n");
	if (!in_interrupt())
		dev_dbg(&urb->dev->dev, "ep %#x - urb len = %d, sglist used, num_trbs = %d\n",
				urb->ep->desc.bEndpointAddress,
				urb->transfer_buffer_length,
				num_trbs);
	return num_trbs;
}

static void check_trb_math(struct urb *urb, int num_trbs, int running_total)
{
	if (num_trbs != 0)
		dev_dbg(&urb->dev->dev, "%s - ep %#x - Miscalculated number of "
				"TRBs, %d left\n", __func__,
				urb->ep->desc.bEndpointAddress, num_trbs);
	if (running_total != urb->transfer_buffer_length)
		dev_dbg(&urb->dev->dev, "%s - ep %#x - Miscalculated tx length, "
				"queued %#x (%d), asked for %#x (%d)\n",
				__func__,
				urb->ep->desc.bEndpointAddress,
				running_total, running_total,
				urb->transfer_buffer_length,
				urb->transfer_buffer_length);
}

static void giveback_first_trb(struct xhci_hcd *xhci, int slot_id,
		unsigned int ep_index, unsigned int stream_id, int start_cycle,
		struct xhci_generic_trb *start_trb, struct xhci_td *td)
{
	/*
                                                                      
                    
  */
	wmb();
	if (start_cycle)
		start_trb->field[3] |= start_cycle;
	else
		start_trb->field[3] &= ~0x1;
	ring_ep_doorbell(xhci, slot_id, ep_index, stream_id);
}

/*
                                                                         
                                                                           
                                                                       
            
 */
int mtktest_xhci_queue_intr_tx(struct xhci_hcd *xhci, gfp_t mem_flags,
		struct urb *urb, int slot_id, unsigned int ep_index)
{
	struct xhci_ep_ctx *ep_ctx = mtktest_xhci_get_ep_ctx(xhci,
			xhci->devs[slot_id]->out_ctx, ep_index);
	int xhci_interval;
	int ep_interval;

	xhci_interval = EP_INTERVAL_TO_UFRAMES(ep_ctx->ep_info);
	ep_interval = urb->interval;
	/*                        */
	if (urb->dev->speed == USB_SPEED_LOW ||
			urb->dev->speed == USB_SPEED_FULL)
		ep_interval *= 8;
	/*                                                                   
                                                        
  */
	if (xhci_interval != ep_interval) {
		if (!printk_ratelimit())
			dev_dbg(&urb->dev->dev, "Driver uses different interval"
					" (%d microframe%s) than xHCI "
					"(%d microframe%s)\n",
					ep_interval,
					ep_interval == 1 ? "" : "s",
					xhci_interval,
					xhci_interval == 1 ? "" : "s");
		urb->interval = xhci_interval;
		/*                                          */
		if (urb->dev->speed == USB_SPEED_LOW ||
				urb->dev->speed == USB_SPEED_FULL)
			urb->interval /= 8;
	}
	return mtktest_xhci_queue_bulk_tx(xhci, GFP_ATOMIC, urb, slot_id, ep_index);
}

/*
                                                                               
                       
                                                            
 */
static u32 xhci_td_remainder(unsigned int td_transfer_size, unsigned int td_running_total
	, unsigned int maxp, unsigned trb_buffer_length)
{
	u32 max = 31;
	int remainder, td_packet_count, packet_transferred;

	//                  
	//                                                    
	if(td_running_total + trb_buffer_length == td_transfer_size)
		return 0;

	//                                                     
	packet_transferred = (td_running_total /*                   */) / maxp;
	td_packet_count = DIV_ROUND_UP(td_transfer_size, maxp);
	remainder = td_packet_count - packet_transferred;

	if(remainder > max)
		return max << 17;
	else
		return remainder << 17;

}

static int queue_bulk_sg_tx(struct xhci_hcd *xhci, gfp_t mem_flags,
		struct urb *urb, int slot_id, unsigned int ep_index)
{
	struct xhci_ring *ep_ring;
	unsigned int num_trbs;
	struct urb_priv *urb_priv;
	struct xhci_td *td;
	struct scatterlist *sg;
	int num_sgs;
	int trb_buff_len, this_sg_len, running_total;
	bool first_trb;
	u64 addr;
	int max_packet;
	bool more_trbs_coming;
	bool zlp;
//                                                                               

	struct xhci_generic_trb *start_trb;
	int start_cycle;

	ep_ring = mtktest_xhci_urb_to_transfer_ring(xhci, urb);
	if (!ep_ring)
		return -EINVAL;

	num_trbs = count_sg_trbs_needed(xhci, urb);
	num_sgs = urb->num_sgs;

	trb_buff_len = prepare_transfer(xhci, xhci->devs[slot_id],
		ep_index, urb->stream_id, num_trbs, urb, 0, mem_flags);
	if (trb_buff_len < 0)
		return trb_buff_len;

	urb_priv = urb->hcpriv;
	td = urb_priv->td[0];

	zlp = false;
	/*                                                               */
	switch(urb->dev->speed){
		case USB_SPEED_SUPER:
			max_packet = urb->ep->desc.wMaxPacketSize;
			break;
		case USB_SPEED_HIGH:
		case USB_SPEED_FULL:
		case USB_SPEED_LOW:
			max_packet = urb->ep->desc.wMaxPacketSize & 0x7ff;
			break;
	}
	if((urb->transfer_flags & URB_ZERO_PACKET)
		&& ((urb->transfer_buffer_length % max_packet) == 0)){
		zlp = true;
	}
#if 0
	td_packet_count = urb->transfer_buffer_length/max_packet + (urb->transfer_buffer_length%max_packet > 0 ? 1 : 0);
	trb_tx_len_sum = 0;
	packet_transferred = 0;
#endif
	/*                                                               */

	/*
                                                                        
                                                                       
                                                                  
  */
	start_trb = &ep_ring->enqueue->generic;
	start_cycle = ep_ring->cycle_state;

	running_total = 0;
	/*
                                      
   
                                                                       
                                                                      
                                                                        
                                                                   
                                                
  */
	sg = urb->sg;
	addr = (u64) sg_dma_address(sg);
	this_sg_len = sg_dma_len(sg);
	trb_buff_len = TRB_MAX_BUFF_SIZE -
		(addr & ((1 << TRB_MAX_BUFF_SHIFT) - 1));
	trb_buff_len = min_t(int, trb_buff_len, this_sg_len);
	if (trb_buff_len > urb->transfer_buffer_length)
		trb_buff_len = urb->transfer_buffer_length;
	xhci_dbg(xhci, "First length to xfer from 1st sglist entry = %u\n",
			trb_buff_len);

	first_trb = true;
	/*                                               */
	do {
		u32 field = 0;
		u32 length_field = 0;
		u32 remainder = 0;

		/*                                                         */
		if (first_trb){
			first_trb = false;
			if (start_cycle == 0)
				field |= 0x1;
		}
		else
			field |= ep_ring->cycle_state;

		/*                                                             
                                                    
   */
		if (num_trbs > 1 || zlp) {
			field |= TRB_CHAIN;
		} else {
			/*                                                    */
			td->last_trb = ep_ring->enqueue;
			field |= TRB_IOC;
		}
		xhci_dbg(xhci, " sg entry: dma = %#x, len = %#x (%d), "
				"64KB boundary at %#x, end dma = %#x\n",
				(unsigned int) addr, trb_buff_len, trb_buff_len,
				(unsigned int) (addr + TRB_MAX_BUFF_SIZE) & ~(TRB_MAX_BUFF_SIZE - 1),
				(unsigned int) addr + trb_buff_len);
		if (TRB_MAX_BUFF_SIZE -
				(addr & ((1 << TRB_MAX_BUFF_SHIFT) - 1)) < trb_buff_len) {
			xhci_warn(xhci, "WARN: sg dma xfer crosses 64KB boundaries!\n");
			xhci_dbg(xhci, "Next boundary at %#x, end dma = %#x\n",
					(unsigned int) (addr + TRB_MAX_BUFF_SIZE) & ~(TRB_MAX_BUFF_SIZE - 1),
					(unsigned int) addr + trb_buff_len);
		}
		remainder = xhci_td_remainder(urb->transfer_buffer_length, running_total, max_packet, trb_buff_len);
		length_field = TRB_LEN(trb_buff_len) |
			remainder |
			TRB_INTR_TARGET(0);
		if (num_trbs > 1 || zlp)
			more_trbs_coming = true;
		else
			more_trbs_coming = false;
		xhci_dbg(xhci, "queue trb, len[%d], addr[0x%llx]\n", trb_buff_len, addr);
		queue_trb(xhci, ep_ring, false, more_trbs_coming,
				lower_32_bits(addr),
				upper_32_bits(addr),
				length_field,
				/*                                             
                                                  
                                                  
                                        
     */
				field | TRB_ISP | TRB_TYPE(TRB_NORMAL));
		--num_trbs;
		running_total += trb_buff_len;

		/*                                      
                                                         
   */
		this_sg_len -= trb_buff_len;
		if (this_sg_len == 0) {
			--num_sgs;
			if (num_sgs == 0)
				break;
			sg = sg_next(sg);
			addr = (u64) sg_dma_address(sg);
			this_sg_len = sg_dma_len(sg);
		} else {
			addr += trb_buff_len;
		}

		trb_buff_len = TRB_MAX_BUFF_SIZE -
			(addr & ((1 << TRB_MAX_BUFF_SHIFT) - 1));
		trb_buff_len = min_t(int, trb_buff_len, this_sg_len);
		if (running_total + trb_buff_len > urb->transfer_buffer_length)
			trb_buff_len =
				urb->transfer_buffer_length - running_total;
	} while (num_trbs > 0/*                                           */);
	if(zlp){
		u32 field = 0;
		u32 length_field = 0;
		length_field = TRB_LEN(0) |	TRB_INTR_TARGET(0);
		field |= ep_ring->cycle_state;
		field |= TRB_IOC;
		td->last_trb = ep_ring->enqueue;
		xhci_dbg(xhci, "queue trb, len[0x%x], addr[0x%llx]\n", length_field, addr);
		queue_trb(xhci, ep_ring, false, false,
				lower_32_bits(addr),
				upper_32_bits(addr),
				length_field,
				/*                                             
                                                  
                                                  
                                        
     */
				field | TRB_ISP | TRB_TYPE(TRB_NORMAL));
	}
	check_trb_math(urb, num_trbs, running_total);
	giveback_first_trb(xhci, slot_id, ep_index, urb->stream_id,
			start_cycle, start_trb, td);
	return 0;
}

/*                                                       */
int mtktest_xhci_queue_bulk_tx(struct xhci_hcd *xhci, gfp_t mem_flags,
		struct urb *urb, int slot_id, unsigned int ep_index)
{
	struct xhci_ring *ep_ring;
	struct urb_priv *urb_priv;
	struct xhci_td *td;
	int num_trbs;
	struct xhci_generic_trb *start_trb;
	bool first_trb;
	bool more_trbs_coming;
	int start_cycle;
	u32 field, length_field;
	int max_packet;
	int running_total, trb_buff_len, ret;
	u64 addr;

	if (urb->num_sgs)
		return queue_bulk_sg_tx(xhci, mem_flags, urb, slot_id, ep_index);

	ep_ring = mtktest_xhci_urb_to_transfer_ring(xhci, urb);
	if (!ep_ring){
		xhci_err(xhci, "mtktest_xhci_queue_bulk_tx, Get transfer ring failed\n");
		return -EINVAL;
	}
	num_trbs = 0;
	/*                                                               */
	running_total = TRB_MAX_BUFF_SIZE -
		(urb->transfer_dma & ((1 << TRB_MAX_BUFF_SHIFT) - 1));

	/*                                                              
                                                  
  */
	if (running_total != 0 || urb->transfer_buffer_length == 0)
		num_trbs++;
	/*                                                            */
	while (running_total < urb->transfer_buffer_length) {
		num_trbs++;
		running_total += TRB_MAX_BUFF_SIZE;
	}
#if 1
	/*                                                               */
	switch(urb->dev->speed){
		case USB_SPEED_SUPER:
			max_packet = urb->ep->desc.wMaxPacketSize;
			break;
		case USB_SPEED_HIGH:
		case USB_SPEED_FULL:
		case USB_SPEED_LOW:
			max_packet = urb->ep->desc.wMaxPacketSize & 0x7ff;
			break;
	}
	if((urb->transfer_flags & URB_ZERO_PACKET)
		&& ((urb->transfer_buffer_length % max_packet) == 0)){
		num_trbs++;
	}
	/*                                                               */
#endif


	if (!in_interrupt())
		dev_dbg(&urb->dev->dev, "ep %#x - urb len = %#x (%d), addr = %#llx, num_trbs = %d\n",
				urb->ep->desc.bEndpointAddress,
				urb->transfer_buffer_length,
				urb->transfer_buffer_length,
				(unsigned long long)urb->transfer_dma,
				num_trbs);

	ret = prepare_transfer(xhci, xhci->devs[slot_id],
		ep_index, urb->stream_id,
		num_trbs, urb, 0, mem_flags);
	if (ret < 0)
		return ret;

	urb_priv = urb->hcpriv;
	td = urb_priv->td[0];

	/*
                                                                        
                                                                       
                                                                  
  */
	start_trb = &ep_ring->enqueue->generic;
	start_cycle = ep_ring->cycle_state;

	running_total = 0;
	/*                                    */
	addr = (u64) urb->transfer_dma;
	trb_buff_len = TRB_MAX_BUFF_SIZE -
		(urb->transfer_dma & ((1 << TRB_MAX_BUFF_SHIFT) - 1));
	if (urb->transfer_buffer_length < trb_buff_len)
		trb_buff_len = urb->transfer_buffer_length;

	first_trb = true;

	/*                                               */
	do {
		u32 remainder = 0;
		field = 0;

		/*                                                         */
		if (first_trb){
			first_trb = false;
			if (start_cycle == 0)
					field |= 0x1;
			}
		else
			field |= ep_ring->cycle_state;

		/*                                                             
                                                    
   */
		if (num_trbs > 1) {
			field |= TRB_CHAIN;
		} else {
			/*                                                    */
			td->last_trb = ep_ring->enqueue;
			field |= TRB_IOC;
			if(g_is_bei){
				field |= TRB_BEI;
			}
		}
		remainder = xhci_td_remainder(urb->transfer_buffer_length, running_total, max_packet, trb_buff_len);
		length_field = TRB_LEN(trb_buff_len) |
			remainder |
			TRB_INTR_TARGET(0);
		if (num_trbs > 1)
			more_trbs_coming = true;
		else
			more_trbs_coming = false;
//                                                                         

		if(g_idt_transfer && !usb_endpoint_dir_in(&urb->ep->desc)){
			struct xhci_generic_trb *trb;
			u32 *idt_data;

			idt_data = urb->transfer_buffer;
			xhci_err(xhci, "idt_data: 0x%p\n", idt_data);
			trb = &ep_ring->enqueue->generic;
			trb->field[0] = *idt_data;
			idt_data++;
			trb->field[1] = *idt_data;
			trb->field[2] = length_field;
			trb->field[3] = field | TRB_ISP | TRB_TYPE(TRB_NORMAL) | TRB_IDT;
			xhci_dbg(xhci, "Dump TRB: 0x%x 0x%x 0x%x 0x%x\n", trb->field[0], trb->field[1], trb->field[2], trb->field[3]);
			inc_enq(xhci, ep_ring, false, more_trbs_coming);
		}
		else{
			queue_trb(xhci, ep_ring, false, more_trbs_coming,
					lower_32_bits(addr),
					upper_32_bits(addr),
					length_field,
					/*                                             
                                                   
                                                   
                                         
      */
					field | TRB_ISP | TRB_TYPE(TRB_NORMAL));
		}
		--num_trbs;
		running_total += trb_buff_len;

		/*                                    */
		addr += trb_buff_len;
		trb_buff_len = urb->transfer_buffer_length - running_total;
		if (trb_buff_len > TRB_MAX_BUFF_SIZE)
			trb_buff_len = TRB_MAX_BUFF_SIZE;
	} while (num_trbs > 0/*                                           */);

	check_trb_math(urb, num_trbs, running_total);
	if(g_td_to_noop){
		if (start_cycle)
			start_trb->field[3] |= start_cycle;
		else
			start_trb->field[3] &= ~0x1;
		td_to_noop(xhci, ep_ring, td);
		list_del(&td->td_list);
		return 0;
	}
	giveback_first_trb(xhci, slot_id, ep_index, urb->stream_id,
			start_cycle, start_trb, td);
	return 0;
}

static int count_isoc_trbs_needed(struct xhci_hcd *xhci,
		struct urb *urb, int i)
{
	int num_trbs = 0;
	u64 addr, td_len, running_total;

	addr = (u64) (urb->transfer_dma + urb->iso_frame_desc[i].offset);
	td_len = urb->iso_frame_desc[i].length;

	running_total = TRB_MAX_BUFF_SIZE -
			(addr & ((1 << TRB_MAX_BUFF_SHIFT) - 1));
	if (running_total != 0)
		num_trbs++;

	while (running_total < td_len) {
		num_trbs++;
		running_total += TRB_MAX_BUFF_SIZE;
	}

	return num_trbs;
}


/*                           */
static int xhci_queue_isoc_tx(struct xhci_hcd *xhci, gfp_t mem_flags,
		struct urb *urb, int slot_id, unsigned int ep_index)
{
	struct xhci_ring *ep_ring;
	struct urb_priv *urb_priv;
	struct xhci_td *td;
	int num_tds, trbs_per_td;
	struct xhci_generic_trb *start_trb;
	bool first_trb;
	int start_cycle;
	u32 field, length_field;
	int running_total, trb_buff_len, td_len, td_remain_len, ret;
	u64 start_addr, addr;
	int i, j;
	bool more_trbs_coming;
	int max_packet;
	int max_esit_payload;
	int frame_id;

	ep_ring = xhci->devs[slot_id]->eps[ep_index].ring;

	num_tds = urb->number_of_packets;
	if (num_tds < 1) {
		xhci_dbg(xhci, "Isoc URB with zero packets?\n");
		return -EINVAL;
	}

	if (!in_interrupt())
		dev_dbg(&urb->dev->dev, "ep %#x - urb len = %#x (%d),"
				" addr = %#llx, num_tds = %d\n",
				urb->ep->desc.bEndpointAddress,
				urb->transfer_buffer_length,
				urb->transfer_buffer_length,
				(unsigned long long)urb->transfer_dma,
				num_tds);

	start_addr = (u64) urb->transfer_dma;
	start_trb = &ep_ring->enqueue->generic;
	start_cycle = ep_ring->cycle_state;
	switch(urb->dev->speed){
		case USB_SPEED_SUPER:
			max_packet = urb->ep->desc.wMaxPacketSize;
			break;
		case USB_SPEED_HIGH:
		case USB_SPEED_FULL:
		case USB_SPEED_LOW:
			max_packet = urb->ep->desc.wMaxPacketSize & 0x7ff;
			break;
	}

	/*                                               */
	for (i = 0; i < num_tds; i++) {
		first_trb = true;

		running_total = 0;
		addr = start_addr + urb->iso_frame_desc[i].offset;
		td_len = urb->iso_frame_desc[i].length;
		td_remain_len = td_len;

		trbs_per_td = count_isoc_trbs_needed(xhci, urb, i);

		ret = prepare_transfer(xhci, xhci->devs[slot_id], ep_index,
				urb->stream_id, trbs_per_td, urb, i, mem_flags);
		if (ret < 0)
			return ret;

		urb_priv = urb->hcpriv;
		td = urb_priv->td[i];
		for (j = 0; j < trbs_per_td; j++) {
			u32 remainder = 0;
			field = 0;

			if (first_trb) {
				/*                    */
				field |= TRB_TYPE(TRB_ISOC);
				/*                            */
				if(g_iso_frame && i==0){
					frame_id = xhci_readl(xhci, &xhci->run_regs->microframe_index) >> 3;
					frame_id &= 0x7ff;
					frame_id--;
					if(frame_id <0){
						frame_id = 0x7ff;
					}
					field |= ((frame_id) << 20);
					xhci_err(xhci, "[DBG]start frame id = %d\n", frame_id);
				}
				else{
					field |= TRB_SIA;
				}
				if (i == 0) {
					if (start_cycle == 0)
						field |= 0x1;
				} else
					field |= ep_ring->cycle_state;
				first_trb = false;
			} else {
				/*                         */
				field |= TRB_TYPE(TRB_NORMAL);
				field |= ep_ring->cycle_state;
			}

			/*                                                    
                                                       
            
    */
			if (j < trbs_per_td - 1) {
				field |= TRB_CHAIN;
				more_trbs_coming = true;
			} else {
				td->last_trb = ep_ring->enqueue;
				field |= TRB_IOC;
				more_trbs_coming = false;
			}

			/*                      */
			trb_buff_len = TRB_MAX_BUFF_SIZE -
				(addr & ((1 << TRB_MAX_BUFF_SHIFT) - 1));
			if (trb_buff_len > td_remain_len)
				trb_buff_len = td_remain_len;

//                                                         
			remainder = xhci_td_remainder(td_len, running_total, max_packet, trb_buff_len);
			length_field = TRB_LEN(trb_buff_len) |
				remainder |
				TRB_INTR_TARGET(0);
			queue_trb(xhci, ep_ring, false, more_trbs_coming,
				lower_32_bits(addr),
				upper_32_bits(addr),
				length_field,
				/*                                             
                                                  
                                                  
                                        
     */
				field | TRB_ISP);
			running_total += trb_buff_len;

			addr += trb_buff_len;
			td_remain_len -= trb_buff_len;
		}

		/*                 */
		if (running_total != td_len) {
			xhci_err(xhci, "ISOC TD length unmatch\n");
			return -EINVAL;
		}
	}

	giveback_first_trb(xhci, slot_id, ep_index, urb->stream_id,
			start_cycle, start_trb, td);
	return 0;
}

/*
                                                                     
                                           
                                                                                   
                                      
                                                                           
 */
int mtktest_xhci_queue_isoc_tx_prepare(struct xhci_hcd *xhci, gfp_t mem_flags,
		struct urb *urb, int slot_id, unsigned int ep_index)
{
	struct xhci_virt_device *xdev;
	struct xhci_ring *ep_ring;
	struct xhci_ep_ctx *ep_ctx;
	int start_frame;
	int xhci_interval;
	int ep_interval;
	int num_tds, num_trbs, i;
	int ret;

	xdev = xhci->devs[slot_id];
	ep_ring = xdev->eps[ep_index].ring;
	ep_ctx = mtktest_xhci_get_ep_ctx(xhci, xdev->out_ctx, ep_index);

	num_trbs = 0;
	num_tds = urb->number_of_packets;
	for (i = 0; i < num_tds; i++)
		num_trbs += count_isoc_trbs_needed(xhci, urb, i);

	/*                                                                    
                                                                    
  */
	ret = prepare_ring(xhci, ep_ring, ep_ctx->ep_info & EP_STATE_MASK,
				num_trbs, mem_flags);
	if (ret)
		return ret;

	start_frame = xhci_readl(xhci, &xhci->run_regs->microframe_index);
	start_frame &= 0x3fff;

	urb->start_frame = start_frame;
	if (urb->dev->speed == USB_SPEED_LOW ||
			urb->dev->speed == USB_SPEED_FULL)
		urb->start_frame >>= 3;

	xhci_interval = EP_INTERVAL_TO_UFRAMES(ep_ctx->ep_info);
	ep_interval = urb->interval;
	/*                        */
	if (urb->dev->speed == USB_SPEED_LOW ||
			urb->dev->speed == USB_SPEED_FULL)
		ep_interval *= 8;
	/*                                                                   
                                                        
  */
	if (xhci_interval != ep_interval) {
		if (printk_ratelimit())
			dev_dbg(&urb->dev->dev, "Driver uses different interval"
					" (%d microframe%s) than xHCI "
					"(%d microframe%s)\n",
					ep_interval,
					ep_interval == 1 ? "" : "s",
					xhci_interval,
					xhci_interval == 1 ? "" : "s");
		urb->interval = xhci_interval;
		/*                                          */
		if (urb->dev->speed == USB_SPEED_LOW ||
				urb->dev->speed == USB_SPEED_FULL)
			urb->interval /= 8;
	}
	return xhci_queue_isoc_tx(xhci, GFP_ATOMIC, urb, slot_id, ep_index);
}

/*                                    */
int mtktest_xhci_queue_ctrl_tx(struct xhci_hcd *xhci, gfp_t mem_flags,
		struct urb *urb, int slot_id, unsigned int ep_index)
{
	struct xhci_ring *ep_ring;
	int num_trbs;
	int ret;
	struct usb_ctrlrequest *setup;
	struct xhci_generic_trb *start_trb;
	int start_cycle;
	u32 field, length_field;
	struct urb_priv *urb_priv;
	struct xhci_td *td;
	int max_packet;
	int remainder;
	u32 trt;
#if 0
	xhci_dbg(xhci, "urb->ep->desc->bLength 0x%x\n", urb->ep->desc.bLength);
	xhci_dbg(xhci, "urb->ep->desc->bDescriptorType 0x%x\n", urb->ep->desc.bDescriptorType);
	xhci_dbg(xhci, "urb->ep->desc->bEndpointAddress 0x%x\n", urb->ep->desc.bEndpointAddress);
	xhci_dbg(xhci, "urb->ep->desc->bmAttributes 0x%x\n", urb->ep->desc.bmAttributes);
	xhci_dbg(xhci, "urb->ep->desc->wMaxPacketSize 0x%x\n", urb->ep->desc.wMaxPacketSize);
	xhci_dbg(xhci, "urb->ep->desc->bInterval 0x%x\n", urb->ep->desc.bInterval);
	xhci_dbg(xhci, "urb->ep->desc->bRefresh 0x%x\n", urb->ep->desc.bRefresh);
	xhci_dbg(xhci, "urb->ep->desc->bSynchAddress 0x%x\n", urb->ep->desc.bSynchAddress);
	xhci_dbg(xhci, "urb->setup_packet 0x%x\n", urb->setup_packet);
#endif
	xhci_dbg(xhci, "mtktest_xhci_queue_ctrl_tx\n");
	
	ep_ring = mtktest_xhci_urb_to_transfer_ring(xhci, urb);
	if (!ep_ring)
		return -EINVAL;

	/*
                                                                       
                
  */
	if (!urb->setup_packet)
		return -EINVAL;

	if (!in_interrupt())
		xhci_dbg(xhci, "Queueing ctrl tx for slot id %d, ep %d\n",
				slot_id, ep_index);
	/*                               */
	num_trbs = 2;
	/*
                                                                         
                                                                   
                                                          
  */
	if (urb->transfer_buffer_length > 0)
		num_trbs++;
	ret = prepare_transfer(xhci, xhci->devs[slot_id],
			ep_index, urb->stream_id,
			num_trbs, urb, 0, mem_flags);
	if (ret < 0)
		return ret;

	urb_priv = urb->hcpriv;
	td = urb_priv->td[0];

	/*
                                                                        
                                                                       
                                                                  
  */
	start_trb = &ep_ring->enqueue->generic;
	start_cycle = ep_ring->cycle_state;
#if 0
	xhci_dbg(xhci, "start_trb 0x%x\n", &ep_ring->enqueue->generic);
	xhci_dbg(xhci, "start_cycle 0x%x\n", ep_ring->cycle_state);
#endif
	/*                                         */
	/*                                                                 */

	setup = (struct usb_ctrlrequest *) urb->setup_packet;
#if 0
	xhci_dbg(xhci, "setup->bRequestType 0x%x\n", setup->bRequestType);
	xhci_dbg(xhci, "setup->bRequest 0x%x\n", setup->bRequest);
	xhci_dbg(xhci, "setup->wValue 0x%x\n", setup->wValue);
	xhci_dbg(xhci, "setup->wIndex 0x%x\n", setup->wIndex);
	xhci_dbg(xhci, "setup->wLength 0x%x\n", setup->wLength);
#endif
	if(num_trbs ==2){
		trt = TRB_TRT(TRT_NO_DATA);
	}else if(setup->bRequestType & USB_DIR_IN){
		trt = TRB_TRT(TRT_IN_DATA);
	}else{
		trt = TRB_TRT(TRT_OUT_DATA);
	}
	field = 0;
	field |= TRB_IDT | TRB_TYPE(TRB_SETUP) | trt;
	if (start_cycle == 0)
		field |= 0x1;
	queue_trb(xhci, ep_ring, false, true,
			/*                                                         */
			setup->bRequestType | setup->bRequest << 8 | setup->wValue << 16,
			setup->wIndex | setup->wLength << 16,
			TRB_LEN(8) | TRB_INTR_TARGET(0),
			/*                           */
			field);
	/*                                  */
	field = 0;
//                                                                                                        
	length_field = TRB_LEN(urb->transfer_buffer_length) |
//             
		TRB_INTR_TARGET(0);
	if (urb->transfer_buffer_length > 0) {
		if (setup->bRequestType & USB_DIR_IN)
			field |= TRB_DIR_IN;
		queue_trb(xhci, ep_ring, false, true,
				lower_32_bits(urb->transfer_dma),
				upper_32_bits(urb->transfer_dma),
				length_field,
				/*                   */
				field | TRB_ISP | TRB_TYPE(TRB_DATA) | ep_ring->cycle_state);
	}
#if 1
	max_packet = urb->ep->desc.wMaxPacketSize;
	if((urb->transfer_flags & URB_ZERO_PACKET)
		&& ((urb->transfer_buffer_length % max_packet) == 0)){
		if (setup->bRequestType & USB_DIR_IN)
				field |= TRB_DIR_IN;
		queue_trb(xhci, ep_ring, false, true,
				lower_32_bits(urb->transfer_dma),
				upper_32_bits(urb->transfer_dma),
				0,
				/*                   */
				field | TRB_ISP | TRB_TYPE(TRB_DATA) | ep_ring->cycle_state);
	}
#endif
	/*                                                */	
	td->last_trb = ep_ring->enqueue;
	xhci_dbg(xhci, "set the td 0x%p 's last_trb 0x%p to 0x%p\n", td, td->last_trb , ep_ring->enqueue) ;	

	/*                                                                    */
	/*                                                              */
	if (urb->transfer_buffer_length > 0 && setup->bRequestType & USB_DIR_IN)
		field = 0;
	else
		field = TRB_DIR_IN;

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p\n", td, td->last_trb ) ;
	queue_trb(xhci, ep_ring, false, false,
			0,
			0,
			TRB_INTR_TARGET(0),
			/*                     */
			field | TRB_IOC | TRB_TYPE(TRB_STATUS) | ep_ring->cycle_state);

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p\n", td, td->last_trb ) ;
#if 1
	giveback_first_trb(xhci, slot_id, ep_index, 0,
			start_cycle, start_trb, td);

	xhci_dbg(xhci, "td is 0x%p, last_trb is 0x%p\n", td, td->last_trb ) ;
#endif
	return 0;
}

/*                                 */

/*                                                                 
                                                                           
                                                                         
                                                                                
                                                    
                                                                          
                                                                           
 */
static int queue_command(struct xhci_hcd *xhci, u32 field1, u32 field2,
		u32 field3, u32 field4, bool command_must_succeed)
{
	int reserved_trbs = xhci->cmd_ring_reserved_trbs;
	int ret;

	if (!command_must_succeed)
		reserved_trbs++;

	ret = prepare_ring(xhci, xhci->cmd_ring, EP_STATE_RUNNING,
			reserved_trbs, GFP_ATOMIC);
	if (ret < 0) {
		xhci_err(xhci, "[ERROR] No room for command on command ring\n");
		if (command_must_succeed)
			xhci_err(xhci, "[ERROR] Reserved TRB counting for "
					"unfailable commands failed.\n");
		return ret;
	}
	queue_trb(xhci, xhci->cmd_ring, false, false, field1, field2, field3,
			field4 | xhci->cmd_ring->cycle_state);
	return 0;
}

/*                                           */
static int queue_cmd_noop(struct xhci_hcd *xhci)
{
	return queue_command(xhci, 0, 0, 0, TRB_TYPE(TRB_CMD_NOOP), false);
}

/*
                                                                    
              
 */
void *mtktest_xhci_setup_one_noop(struct xhci_hcd *xhci)
{
	if (queue_cmd_noop(xhci) < 0)
		return NULL;
	xhci->noops_submitted++;
	return mtktest_xhci_ring_cmd_db;
}

/*
                                                                    
              
 */
void *mtktest_mtk_xhci_setup_one_noop(struct xhci_hcd *xhci)
{
	if (queue_cmd_noop(xhci) < 0)
		return NULL;
	mtktest_xhci_ring_cmd_db(xhci);
}


/*                                                            */
int mtktest_xhci_queue_slot_control(struct xhci_hcd *xhci, u32 trb_type, u32 slot_id)
{
	return queue_command(xhci, 0, 0, 0,
			TRB_TYPE(trb_type) | SLOT_ID_FOR_TRB(slot_id), false);
}

/*                                     */
int mtktest_xhci_queue_address_device(struct xhci_hcd *xhci, dma_addr_t in_ctx_ptr,
		u32 slot_id, char isBSR)
{

	if(isBSR){
		return queue_command(xhci, lower_32_bits(in_ctx_ptr),
			upper_32_bits(in_ctx_ptr), 0,
			TRB_TYPE(TRB_ADDR_DEV) | SLOT_ID_FOR_TRB(slot_id) | ADDRESS_TRB_BSR,
			false);
	}
	else{
		return queue_command(xhci, lower_32_bits(in_ctx_ptr),
				upper_32_bits(in_ctx_ptr), 0,
				TRB_TYPE(TRB_ADDR_DEV) | SLOT_ID_FOR_TRB(slot_id),
				false);
	}
}

int mtktest_xhci_queue_vendor_command(struct xhci_hcd *xhci,
		u32 field1, u32 field2, u32 field3, u32 field4)
{
	return queue_command(xhci, field1, field2, field3, field4, false);
}

/*                                  */
int mtktest_xhci_queue_reset_device(struct xhci_hcd *xhci, u32 slot_id)
{
	return queue_command(xhci, 0, 0, 0,
			TRB_TYPE(TRB_RESET_DEV) | SLOT_ID_FOR_TRB(slot_id),
			false);
}

/*                                        */
int mtktest_xhci_queue_configure_endpoint(struct xhci_hcd *xhci, dma_addr_t in_ctx_ptr,
		u32 slot_id, bool command_must_succeed)
{
	return queue_command(xhci, lower_32_bits(in_ctx_ptr),
			upper_32_bits(in_ctx_ptr), 0,
			TRB_TYPE(TRB_CONFIG_EP) | SLOT_ID_FOR_TRB(slot_id),
			command_must_succeed);
}

int mtktest_xhci_queue_deconfigure_endpoint(struct xhci_hcd *xhci, dma_addr_t in_ctx_ptr,
		u32 slot_id, bool command_must_succeed){
	return queue_command(xhci, lower_32_bits(in_ctx_ptr),
			upper_32_bits(in_ctx_ptr), 0,
			TRB_TYPE(TRB_CONFIG_EP) | SLOT_ID_FOR_TRB(slot_id) | CONFIG_EP_TRB_DC,
			command_must_succeed);
}

/*                                       */
int mtktest_xhci_queue_evaluate_context(struct xhci_hcd *xhci, dma_addr_t in_ctx_ptr,
		u32 slot_id)
{
	return queue_command(xhci, lower_32_bits(in_ctx_ptr),
			upper_32_bits(in_ctx_ptr), 0,
			TRB_TYPE(TRB_EVAL_CONTEXT) | SLOT_ID_FOR_TRB(slot_id),
			false);
}

int mtktest_xhci_queue_stop_endpoint(struct xhci_hcd *xhci, int slot_id,
		unsigned int ep_index)
{
	u32 trb_slot_id = SLOT_ID_FOR_TRB(slot_id);
	u32 trb_ep_index = EP_ID_FOR_TRB(ep_index);
	u32 type = TRB_TYPE(TRB_STOP_RING);
	//                                                                                       
	if(ep_index == 1){
		if(TRB_FIELD_TO_TYPE(xhci->cmd_ring->enqueue->generic.field[3]) == TRB_LINK){
			g_cmd_ring_pointer1 = (((int)xhci->cmd_ring->enqueue->link.segment_ptr) & 0xff0);
		}
		else{
			g_cmd_ring_pointer1 = ((((int)(unsigned long)xhci->cmd_ring->enqueue) & 0xff0));
		}
	}
	else if(ep_index == 2){
		if(TRB_FIELD_TO_TYPE(xhci->cmd_ring->enqueue->generic.field[3]) == TRB_LINK){
			g_cmd_ring_pointer2 = (((int)xhci->cmd_ring->enqueue->link.segment_ptr) & 0xff0);
		}
		else{
			g_cmd_ring_pointer2 = ((((int)(unsigned long)xhci->cmd_ring->enqueue) & 0xff0));
		}
	}
	return queue_command(xhci, 0, 0, 0,
			trb_slot_id | trb_ep_index | type, false);
}

/*                                           
                                                                   
 */
static int queue_set_tr_deq(struct xhci_hcd *xhci, int slot_id,
		unsigned int ep_index, unsigned int stream_id,
		struct xhci_segment *deq_seg,
		union xhci_trb *deq_ptr, u32 cycle_state)
{
	dma_addr_t addr;
	u32 trb_slot_id = SLOT_ID_FOR_TRB(slot_id);
	u32 trb_ep_index = EP_ID_FOR_TRB(ep_index);
	u32 trb_stream_id = STREAM_ID_FOR_TRB(stream_id);
	u32 type = TRB_TYPE(TRB_SET_DEQ);
	struct xhci_virt_ep *ep;

	addr = mtktest_xhci_trb_virt_to_dma(deq_seg, deq_ptr);
	if (addr == 0) {
		xhci_warn(xhci, "WARN Cannot submit Set TR Deq Ptr\n");
		xhci_warn(xhci, "WARN deq seg = %p, deq pt = %p\n",
				deq_seg, deq_ptr);
		return 0;
	}
	ep = &xhci->devs[slot_id]->eps[ep_index];
	if ((ep->ep_state & SET_DEQ_PENDING)) {
		xhci_warn(xhci, "WARN Cannot submit Set TR Deq Ptr\n");
		xhci_warn(xhci, "A Set TR Deq Ptr command is pending.\n");
		return 0;
	}
	ep->queued_deq_seg = deq_seg;
	ep->queued_deq_ptr = deq_ptr;
	return queue_command(xhci, lower_32_bits(addr) | cycle_state,
			upper_32_bits(addr), trb_stream_id,
			trb_slot_id | trb_ep_index | type, false);
}

int mtktest_xhci_queue_reset_ep(struct xhci_hcd *xhci, int slot_id,
		unsigned int ep_index)
{
	u32 trb_slot_id = SLOT_ID_FOR_TRB(slot_id);
	u32 trb_ep_index = EP_ID_FOR_TRB(ep_index);
	u32 type = TRB_TYPE(TRB_RESET_EP);

	return queue_command(xhci, 0, 0, 0, trb_slot_id | trb_ep_index | type,
			false);
}
