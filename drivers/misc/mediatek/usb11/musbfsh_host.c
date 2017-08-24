/*
 * MUSB OTG driver host support
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
 * Copyright (C) 2008-2009 MontaVista Software, Inc. <source@mvista.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/list.h>

#include "musbfsh_core.h"
#include "musbfsh_host.h"
#include "musbfsh_dma.h"
#include "usb.h"

#ifdef MTK_USB_RUNTIME_SUPPORT
#include <cust_eint.h>
extern void mt_eint_unmask(unsigned int line);
extern void mt_eint_mask(unsigned int line);
#endif

/*                             
  
                                                                       
                                                            
  
                                         
                                                    
                                                       
                                                                            
                                                                           
                                                                     
  
                                                                
                                                                   
                                                             
                                                               
  
                                                             
  
                                                                     
                                                           
  
                                                            
  
                       
  
                                                                        
                                                                        
                                                                       
                                                                      
                                                                       
                                                                        
 */


/*
                          
  
                                                                           
                                                                          
                                                                         
                    
  
                                                                            
                                                                            
                                                                             
                                                      
 */


static void musbfsh_ep_program(struct musbfsh *musbfsh, u8 epnum,
			struct urb *urb, int is_out,
			u8 *buf, u32 offset, u32 len);

/*
                                                
 */
static void musbfsh_h_tx_flush_fifo(struct musbfsh_hw_ep *ep)
{
	void __iomem	*epio = ep->regs;
	u16		csr;
	u16		lastcsr = 0;
	int		retries = 1000;
	
	INFO("musbfsh_h_tx_flush_fifo++\r\n");
	csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
	while (csr & MUSBFSH_TXCSR_FIFONOTEMPTY) {
		if (csr != lastcsr)
			INFO("Host TX FIFONOTEMPTY csr: %02x\n", csr);
		lastcsr = csr;
		csr &= ~MUSBFSH_TXCSR_TXPKTRDY;
		csr |= MUSBFSH_TXCSR_FLUSHFIFO;
		musbfsh_writew(epio, MUSBFSH_TXCSR, csr);
		csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
		if (retries-- < 1) {			
			WARNING("Could not flush host TX%d fifo: csr: %04x\n", ep->epnum, csr);
			return;
		}
		mdelay(1);
	}
}

static void musbfsh_h_ep0_flush_fifo(struct musbfsh_hw_ep *ep)
{
	void __iomem	*epio = ep->regs;
	u16		csr;
	int		retries = 5;
	
	INFO("musbfsh_h_ep0_flush_fifo++\r\n");
	/*                                 */
	do {
		csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
		if (!(csr & (MUSBFSH_CSR0_TXPKTRDY | MUSBFSH_CSR0_RXPKTRDY)))
			break;
		musbfsh_writew(epio, MUSBFSH_TXCSR, MUSBFSH_CSR0_FLUSHFIFO);
		csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
		udelay(10);
	} while (--retries);

	if(!retries) {
		WARNING("Could not flush host TX%d fifo: csr: %04x\n",
			ep->epnum, csr);
	}

	/*                                 */
	musbfsh_writew(epio, MUSBFSH_TXCSR, 0);
}

/*
                                                                      
                       
 */
static inline void musbfsh_h_tx_start(struct musbfsh_hw_ep *ep)
{
	u16	txcsr;
	
	INFO("musbfsh_h_tx_start++\r\n");
	/*                                                       */
	if (ep->epnum) {
		txcsr = musbfsh_readw(ep->regs, MUSBFSH_TXCSR);
		INFO("txcsr=0x%x for ep%d\n", txcsr, ep->epnum);
		txcsr |= MUSBFSH_TXCSR_TXPKTRDY | MUSBFSH_TXCSR_H_WZC_BITS;
		musbfsh_writew(ep->regs, MUSBFSH_TXCSR, txcsr);
		txcsr = musbfsh_readw(ep->regs, MUSBFSH_TXCSR);
		INFO("txcsr=0x%x for ep%d\n", txcsr, ep->epnum);
	} else {
		txcsr = musbfsh_readw(ep->regs, MUSBFSH_CSR0);
		INFO("txcsr=0x%x for ep%d\n", txcsr, ep->epnum);
		txcsr = MUSBFSH_CSR0_H_SETUPPKT | MUSBFSH_CSR0_TXPKTRDY;
		musbfsh_writew(ep->regs, MUSBFSH_CSR0, txcsr);
		txcsr = musbfsh_readw(ep->regs, MUSBFSH_TXCSR);
		INFO("txcsr=0x%x for ep%d\n", txcsr, ep->epnum);
	}

}

static void musbfsh_ep_set_qh(struct musbfsh_hw_ep *ep, int is_in, struct musbfsh_qh *qh)
{
	if (is_in != 0 || ep->is_shared_fifo)
		ep->in_qh  = qh;
	if (is_in == 0 || ep->is_shared_fifo)
		ep->out_qh = qh;
}

static struct musbfsh_qh *musbfsh_ep_get_qh(struct musbfsh_hw_ep *ep, int is_in)
{
	INFO("musbfsh_ep_get_qh++,hw_ep%d,is_in=%d\r\n",ep->epnum,is_in);
	return is_in ? ep->in_qh : ep->out_qh;
}

/*
                                                    
                                       
  
                                           
 */
static void
musbfsh_start_urb(struct musbfsh *musbfsh, int is_in, struct musbfsh_qh *qh)
{
	u32			len;
	struct urb		*urb = next_urb(qh);
	void			*buf = urb->transfer_buffer;
	u32			offset = 0;
	struct musbfsh_hw_ep	*hw_ep = qh->hw_ep;
	unsigned		pipe = urb->pipe;
	u8			address = usb_pipedevice(pipe);
	int			epnum = hw_ep->epnum;
	
	INFO("musbfsh_start_urb++, address=%d,hw_ep->epnum=%d,urb_ep_addr:0x%x\r\n",address,epnum,urb->ep->desc.bEndpointAddress);
	//                                                                                                                                                 

	/*                              */
	qh->offset = 0; //                                 
	qh->segsize = 0;

	/*                             */
	switch (qh->type) {
	case USB_ENDPOINT_XFER_CONTROL: //              
		/*                                           */
		is_in = 0; //                                                   
		musbfsh->ep0_stage = MUSBFSH_EP0_START;
		buf = urb->setup_packet; //                     
		len = 8;
        break;
	default:		/*                 */
		/*                                             */
		buf = urb->transfer_buffer + urb->actual_length; //                                           
		len = urb->transfer_buffer_length - urb->actual_length;
	}
	INFO( "qh %p urb %p dev%d ep%d %s %s, hw_ep %d, %p/%d\n",
			qh, urb, address, qh->epnum,
			is_in ? "in" : "out",
			({char *s; switch (qh->type) {
			case USB_ENDPOINT_XFER_CONTROL:	s = "-ctl"; break;
			case USB_ENDPOINT_XFER_BULK:	s = "-bulk"; break;
			default:			s = "-intr"; break;
			}; s; }),
			epnum, buf + offset, len);
	/*                    */
	musbfsh_ep_set_qh(hw_ep, is_in, qh);
	musbfsh_ep_program(musbfsh, epnum, urb, !is_in, buf, offset, len); //                                                           

	/*                                                       */
	if (is_in) //                                                                       
		return;

	INFO( "Start TX%d %s\n", epnum,hw_ep->tx_channel ? "dma" : "pio");
	if (!hw_ep->tx_channel)
		musbfsh_h_tx_start(hw_ep); //                                                                                 
	return;
}

/*                                                        */
static void musbfsh_giveback(struct musbfsh *musbfsh, struct urb *urb, int status)
__releases(musbfsh->lock)
__acquires(musbfsh->lock)
{
	INFO("musbfsh_giveback++,complete %p %pF (%d), dev%d ep%d%s, %d/%d\n",
			urb, urb->complete, status,
			usb_pipedevice(urb->pipe),
			usb_pipeendpoint(urb->pipe),
			usb_pipein(urb->pipe) ? "in" : "out",
			urb->actual_length, urb->transfer_buffer_length
			);
	
	//                                                                                                                                  

	usb_hcd_unlink_urb_from_ep(musbfsh_to_hcd(musbfsh), urb);
	spin_unlock(&musbfsh->lock);
	usb_hcd_giveback_urb(musbfsh_to_hcd(musbfsh), urb, status);
	spin_lock(&musbfsh->lock);
}

/*                                   */
static inline void musbfsh_save_toggle(struct musbfsh_qh *qh, int is_in,
				    struct urb *urb)
{
	struct musbfsh *musbfsh = qh->hw_ep->musbfsh;
	u8 epnum = qh->hw_ep->epnum;
	int toggle;
	
	INFO("musbfsh_save_toggle++\r\n");
	/*
                                                    
                                    
  */
	if (is_in){
	    toggle = musbfsh_readl(musbfsh->mregs,MUSBFSH_RXTOG);
	    INFO("toggle_IN=0x%x\n",toggle);
	    }
	else{
	    toggle = musbfsh_readl(musbfsh->mregs,MUSBFSH_TXTOG);
	    INFO("toggle_OUT=0x%x\n",toggle);
	    }
	
	if(toggle & (1<<epnum))
	    usb_settoggle(urb->dev, qh->epnum, !is_in, 1);
	else
	    usb_settoggle(urb->dev, qh->epnum, !is_in, 0);
}

static inline void musbfsh_set_toggle(struct musbfsh_qh *qh, int is_in,
				    struct urb *urb)
{
    struct musbfsh *musbfsh = qh->hw_ep->musbfsh;
    u8 epnum = qh->hw_ep->epnum;
    int toggle;
	
    INFO("musbfsh_set_toggle++:qh->hw_ep->epnum %d, qh->epnum %d\n",qh->hw_ep->epnum,qh->epnum);

    toggle = usb_gettoggle (urb->dev, qh->epnum, !is_in);
    if(is_in){
        INFO("qh->dev->toggle[IN]=0x%x\n",qh->dev->toggle[!is_in]);
        musbfsh_writel(musbfsh->mregs, MUSBFSH_RXTOG, (((1<<epnum)<<16)|(toggle<<epnum)));
        musbfsh_writel(musbfsh->mregs, MUSBFSH_RXTOG, (toggle<<epnum));
        }
    else{
        INFO("qh->dev->toggle[OUT]=0x%x\n",qh->dev->toggle[!is_in]);
        musbfsh_writel(musbfsh->mregs, MUSBFSH_TXTOG, (((1<<epnum)<<16)|(toggle<<epnum)));
        musbfsh_writel(musbfsh->mregs, MUSBFSH_TXTOG, (toggle<<epnum));
        }
}

/*
                                                                           
                                                                           
                                                                        
  
                                                         
 */
static void musbfsh_advance_schedule(struct musbfsh *musbfsh, struct urb *urb,
				  struct musbfsh_hw_ep *hw_ep, int is_in)
{
	struct musbfsh_qh		*qh = musbfsh_ep_get_qh(hw_ep, is_in); //               
	struct musbfsh_hw_ep	*ep = qh->hw_ep;
	int			ready = qh->is_ready;
	int			status;
	
	INFO("musbfsh_advance_schedule++\r\n");
	status = (urb->status == -EINPROGRESS) ? 0 : urb->status;

	/*                                   */
	switch (qh->type) {
	case USB_ENDPOINT_XFER_BULK:
	case USB_ENDPOINT_XFER_INT:
		musbfsh_save_toggle(qh, is_in, urb); //                                                  
		break;
	}

	qh->is_ready = 0;
	musbfsh_giveback(musbfsh, urb, status);
	qh->is_ready = ready;

	/*                                                                     
                                                  */
#if 0
	/*                                                           
                                                       
  */
	 if((unsigned int)&qh->hep->urb_list<0xc0000000)
	 {
		 printk(KERN_ERR"hank:%s(%d)urb=0x%x,qh=0x%x,qh->hep=0x%x,&qh->hep->urb_list=0x%x\n",__FUNCTION__,__LINE__,
				 (unsigned int)urb,
				 (unsigned int)qh,
				 (unsigned int)qh->hep,
				 (unsigned int)&qh->hep->urb_list);
		 return ;
	 }
#endif
	if (list_empty(&qh->hep->urb_list)) { //                                                      
		struct list_head	*head;

		if (is_in)
			ep->rx_reinit = 1;
		else
			ep->tx_reinit = 1;

		/*                                 */
#ifdef CONFIG_MTK_DT_USB_SUPPORT
		mark_qh_activity(qh->epnum, ep->epnum, is_in, 1);
#endif
		musbfsh_ep_set_qh(ep, is_in, NULL);
		qh->hep->hcpriv = NULL;

		switch (qh->type) {

		case USB_ENDPOINT_XFER_CONTROL:
		case USB_ENDPOINT_XFER_BULK:
			/*                                                
                                                   
    */
			if (qh->mux == 1) {
				head = qh->ring.prev;
				list_del(&qh->ring);
				kfree(qh);
				qh = first_qh(head);
				break;
			}
		case USB_ENDPOINT_XFER_INT:
			/*                                           
                                                 
                                                
    */
			kfree(qh);
			qh = NULL;
			break;
		}
	}

	if (qh != NULL && qh->is_ready) {
		INFO("... next ep%d %cX urb %p\n",
		    hw_ep->epnum, is_in ? 'R' : 'T', next_urb(qh));
		musbfsh_start_urb(musbfsh, is_in, qh);
	}
}

static u16 musbfsh_h_flush_rxfifo(struct musbfsh_hw_ep *hw_ep, u16 csr)
{
	/*                                         
                                
                                                    
  */
	INFO("musbfsh_h_flush_rxfifo++\r\n");
	csr |= MUSBFSH_RXCSR_FLUSHFIFO | MUSBFSH_RXCSR_RXPKTRDY;
	csr &= ~(MUSBFSH_RXCSR_H_REQPKT
		| MUSBFSH_RXCSR_H_AUTOREQ
		| MUSBFSH_RXCSR_AUTOCLEAR);

	/*                                    */
	musbfsh_writew(hw_ep->regs, MUSBFSH_RXCSR, csr);
	musbfsh_writew(hw_ep->regs, MUSBFSH_RXCSR, csr);

	/*                   */
	return musbfsh_readw(hw_ep->regs, MUSBFSH_RXCSR);
}

/*
                                       
 */
static bool
musbfsh_host_packet_rx(struct musbfsh *musbfsh, struct urb *urb, u8 epnum)//       
{
	u16			rx_count;
	u8			*buf;
	u16			csr;
	bool			done = false;
	u32			length;
	int			do_flush = 0;
	struct musbfsh_hw_ep	*hw_ep = musbfsh->endpoints + epnum;
	void __iomem		*epio = hw_ep->regs;
	struct musbfsh_qh		*qh = hw_ep->in_qh;
	void			*buffer = urb->transfer_buffer;
	
	/*                                  */
	rx_count = musbfsh_readw(epio, MUSBFSH_RXCOUNT);
	INFO("musbfsh_host_packet_rx++:real RX%d count %d, buffer %p len %d/%d\n", epnum, rx_count,
			urb->transfer_buffer, qh->offset,
			urb->transfer_buffer_length);
	/*             */
	/*           */
	buf = buffer + qh->offset;
	length = urb->transfer_buffer_length - qh->offset;
	if (rx_count > length) {
		if (urb->status == -EINPROGRESS)
			urb->status = -EOVERFLOW;
		WARNING("** OVERFLOW %d into %d\n", rx_count, length);
		do_flush = 1;
	} else
		length = rx_count;
	urb->actual_length += length;
	qh->offset += length;

	/*                    */
	done = (urb->actual_length == urb->transfer_buffer_length)
		|| (rx_count < qh->maxpacket)
		|| (urb->status != -EINPROGRESS);
	if (done
			&& (urb->status == -EINPROGRESS)
			&& (urb->transfer_flags & URB_SHORT_NOT_OK)
			&& (urb->actual_length
				< urb->transfer_buffer_length))
		urb->status = -EREMOTEIO;

	musbfsh_read_fifo(hw_ep, length, buf);

	csr = musbfsh_readw(epio, MUSBFSH_RXCSR);
	csr |= MUSBFSH_RXCSR_H_WZC_BITS;
	if (unlikely(do_flush))
		musbfsh_h_flush_rxfifo(hw_ep, csr);
	else {
		/*                                             */
		csr &= ~(MUSBFSH_RXCSR_RXPKTRDY | MUSBFSH_RXCSR_H_REQPKT);
		if (!done)
			csr |= MUSBFSH_RXCSR_H_REQPKT;
		musbfsh_writew(epio, MUSBFSH_RXCSR, csr);
	}

	return done;
}

/*                                                              
                                                                    
                                                
  
                                                                    
                                                                  
                                                   
 */
static void
musbfsh_rx_reinit(struct musbfsh *musbfsh, struct musbfsh_qh *qh, struct musbfsh_hw_ep *ep)
{
	u16	csr;
	
	INFO("musbfsh_rx_reinit++\r\n");
	/*                                                            
                                                               
                                                            
  */

	/*                                         */
	if (ep->is_shared_fifo) {
		csr = musbfsh_readw(ep->regs, MUSBFSH_TXCSR);
		if (csr & MUSBFSH_TXCSR_MODE) {
			musbfsh_h_tx_flush_fifo(ep);
			csr = musbfsh_readw(ep->regs, MUSBFSH_TXCSR);
			musbfsh_writew(ep->regs, MUSBFSH_TXCSR,
				    csr | MUSBFSH_TXCSR_FRCDATATOG);
		}

		/*
                                                           
                                                           
   */
		if (csr & MUSBFSH_TXCSR_DMAMODE)
			musbfsh_writew(ep->regs, MUSBFSH_TXCSR, MUSBFSH_TXCSR_DMAMODE);
		musbfsh_writew(ep->regs, MUSBFSH_TXCSR, 0);

	/*                                           */
	} else {
		csr = musbfsh_readw(ep->regs, MUSBFSH_RXCSR);
		if (csr & MUSBFSH_RXCSR_RXPKTRDY)
			INFO("musbfsh::rx%d, packet/%d ready?\n", ep->epnum,
				musbfsh_readw(ep->regs, MUSBFSH_RXCOUNT));

        musbfsh_h_flush_rxfifo(ep, 0);
	}

	/*                                                */
	if(musbfsh->is_multipoint) {
		musbfsh_write_rxfunaddr(musbfsh->mregs, ep->epnum, qh->addr_reg);
		musbfsh_write_rxhubaddr(musbfsh->mregs, ep->epnum, qh->h_addr_reg);
		musbfsh_write_rxhubport(musbfsh->mregs, ep->epnum, qh->h_port_reg);
	} else {
		musbfsh_writeb(musbfsh->mregs, MUSBFSH_FADDR, qh->addr_reg);
	}

	/*                                                */
	musbfsh_writeb(ep->regs, MUSBFSH_RXTYPE, qh->type_reg);
	musbfsh_writeb(ep->regs, MUSBFSH_RXINTERVAL, qh->intv_reg);
    
	musbfsh_writew(ep->regs, MUSBFSH_RXMAXP, qh->maxpacket);

	ep->rx_reinit = 0;
}

static bool musbfsh_tx_dma_program(struct dma_controller *dma,
		struct musbfsh_hw_ep *hw_ep, struct musbfsh_qh *qh,
		struct urb *urb, u32 offset, u32 length)
{
	struct dma_channel	*channel = hw_ep->tx_channel;
	void __iomem		*epio = hw_ep->regs;
	u16			pkt_size = qh->maxpacket;
	u16			csr;
	u8			mode;
	
	INFO("musbfsh_tx_dma_program++\r\n");
	if (length > channel->max_len)
		length = channel->max_len;

	csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
	if (length > pkt_size) {
		INFO("musbfsh_tx_dma_program,mode 1\r\n");
		mode = 1;
		csr |= MUSBFSH_TXCSR_DMAMODE | MUSBFSH_TXCSR_DMAENAB;
		csr |= MUSBFSH_TXCSR_AUTOSET;
	} else {
		INFO("musbfsh_tx_dma_program,mode 0\r\n");
		mode = 0;
		csr &= ~(MUSBFSH_TXCSR_AUTOSET | MUSBFSH_TXCSR_DMAMODE);
		csr |= MUSBFSH_TXCSR_DMAENAB; /*                            */
	}
	channel->desired_mode = mode;
	INFO("musbfsh_tx_dma_program,txcsr=0x%x\r\n",csr);
	musbfsh_writew(epio, MUSBFSH_TXCSR, csr);//                                           
	qh->segsize = length;

	/*
                                                          
                
  */
	wmb();

	if (!dma->channel_program(channel, pkt_size, mode,
			urb->transfer_dma + offset, length)) {
		dma->channel_release(channel);//                                           
		hw_ep->tx_channel = NULL;

		csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
		csr &= ~(MUSBFSH_TXCSR_AUTOSET | MUSBFSH_TXCSR_DMAENAB);
		musbfsh_writew(epio, MUSBFSH_TXCSR, csr | MUSBFSH_TXCSR_H_WZC_BITS);
		return false;
	}
	return true;
}

/*
                                                
                                              
 */
static void musbfsh_ep_program(struct musbfsh *musbfsh, u8 epnum,//                                    
			struct urb *urb, int is_out,//                                                          
			u8 *buf, u32 offset, u32 len)
{
	struct dma_controller	*dma_controller;
	struct dma_channel	*dma_channel;
	void __iomem		*mbase = musbfsh->mregs;
	struct musbfsh_hw_ep	*hw_ep = musbfsh->endpoints + epnum;
	void __iomem		*epio = hw_ep->regs;
	struct musbfsh_qh		*qh = musbfsh_ep_get_qh(hw_ep, !is_out);//                                                
	u16			packet_sz = qh->maxpacket;
	
	INFO("musbfsh_ep_program++:%s hw%d urb %p spd%d dev%d ep%d%s "
				"h_addr%02x h_port%02x bytes %d\n",
			is_out ? "-->" : "<--",
			epnum, urb, urb->dev->speed,
			qh->addr_reg, qh->epnum, is_out ? "out" : "in",
			qh->h_addr_reg, qh->h_port_reg,
			len);
	
	musbfsh_ep_select(mbase, epnum);//                                                     

	/*                    */
	/*                                                                               
                                                                                    
                                         */
	dma_controller = musbfsh->dma_controller;
	if (is_dma_capable() && epnum && dma_controller) {//                                                   
		INFO("Using DMA epnum%d\n", epnum);
		dma_channel = is_out ? hw_ep->tx_channel : hw_ep->rx_channel;//                            
		if (!dma_channel) {//                                                           
			dma_channel = dma_controller->channel_alloc(
					dma_controller, hw_ep, is_out);//                                                             
	            if(dma_channel){
					INFO("Got a DMA channel for ep%d\n", epnum);
	                if (is_out)
	    				hw_ep->tx_channel = dma_channel;
	    			else
	    				hw_ep->rx_channel = dma_channel;
	            } else {
					WARNING("DMA channel alloc fail for ep%d\n", epnum);
				}
		}
	} else {
		INFO("Using PIO for ep%d\n", epnum);
		dma_channel = NULL;
	}

	/*                                                            */

	/*                                 */
	if (is_out) {
		u16	csr;
		u16	int_txe;
		u16	load_count;

		csr = musbfsh_readw(epio, MUSBFSH_TXCSR);

		/*                                    */
		int_txe = musbfsh_readw(mbase, MUSBFSH_INTRTXE);
		musbfsh_writew(mbase, MUSBFSH_INTRTXE, int_txe & ~(1 << epnum));

		/*                                */
		if (epnum) {//           
			/*                                  */
			musbfsh_h_tx_flush_fifo(hw_ep);

			/*
                                                    
                                                      
                         
    */
			csr &= ~(MUSBFSH_TXCSR_H_NAKTIMEOUT
					| MUSBFSH_TXCSR_AUTOSET
					| MUSBFSH_TXCSR_DMAENAB
					| MUSBFSH_TXCSR_FRCDATATOG
					| MUSBFSH_TXCSR_H_RXSTALL
					| MUSBFSH_TXCSR_H_ERROR
					| MUSBFSH_TXCSR_TXPKTRDY
					);

            musbfsh_set_toggle (qh, !is_out, urb);//                         
            
			musbfsh_writew(epio, MUSBFSH_TXCSR, csr);
			/*                                         */
			csr &= ~MUSBFSH_TXCSR_DMAMODE;
			musbfsh_writew(epio, MUSBFSH_TXCSR, csr);
			csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
		} else {
			/*                        */
			musbfsh_h_ep0_flush_fifo(hw_ep);
		}
		
		/*                                                */
		if(musbfsh->is_multipoint) {
			musbfsh_write_txfunaddr(mbase, epnum, qh->addr_reg);
           		musbfsh_write_txhubaddr(mbase, epnum, qh->h_addr_reg);
           		musbfsh_write_txhubport(mbase, epnum, qh->h_port_reg);
			INFO("set address! h_port_reg 0x%x h_addr_reg 0x%x\n", qh->h_port_reg, qh->h_addr_reg);
		} else {
			musbfsh_writeb(mbase, MUSBFSH_FADDR, qh->addr_reg);//                                              
			INFO("set address! 0x%x\n", qh->addr_reg);
		}

		/*                                     */
		if (epnum) {
			musbfsh_writeb(epio, MUSBFSH_TXTYPE, qh->type_reg);//                                         
			musbfsh_writew(epio, MUSBFSH_TXMAXP, qh->maxpacket);
			musbfsh_writeb(epio, MUSBFSH_TXINTERVAL, qh->intv_reg);
		} else {//   
			musbfsh_writeb(epio, MUSBFSH_NAKLIMIT0, qh->intv_reg);
            if (musbfsh->is_multipoint)
				musbfsh_writeb(epio, MUSBFSH_TYPE0, qh->type_reg);
		}
		load_count = min((u32) packet_sz, len);
        
		//                      
		if (dma_channel && musbfsh_tx_dma_program(dma_controller,
					hw_ep, qh, urb, offset, len))
			load_count = 0;

		if (load_count) {//                    
			/*                  */
			qh->segsize = load_count;
			musbfsh_write_fifo(hw_ep, load_count, buf);
		}

		/*                     */
		musbfsh_writew(mbase, MUSBFSH_INTRTXE, int_txe);//                                                   

	/*            */
	} else {
		u16	csr;

		if (hw_ep->rx_reinit) {
			musbfsh_rx_reinit(musbfsh, qh, hw_ep);
            musbfsh_set_toggle (qh, !is_out, urb);//                         
			csr = 0;
			if (qh->type == USB_ENDPOINT_XFER_INT)
				csr |= MUSBFSH_RXCSR_DISNYET;//                                   

		} else {//       
			csr = musbfsh_readw(hw_ep->regs, MUSBFSH_RXCSR);

			if (csr & (MUSBFSH_RXCSR_RXPKTRDY
					| MUSBFSH_RXCSR_DMAENAB
					| MUSBFSH_RXCSR_H_REQPKT))
				ERR("broken !rx_reinit, ep%d csr %04x\n",
						hw_ep->epnum, csr);

			/*                                             */
			csr &= MUSBFSH_RXCSR_DISNYET;
		}

		/*                 */
		
		csr |= MUSBFSH_RXCSR_H_REQPKT;//                          
		INFO("RXCSR%d := %04x\n", epnum, csr);
		musbfsh_writew(hw_ep->regs, MUSBFSH_RXCSR, csr);
		csr = musbfsh_readw(hw_ep->regs, MUSBFSH_RXCSR);
	}
}


/*
                                              
                                                          
 */
static bool musbfsh_h_ep0_continue(struct musbfsh *musbfsh, u16 len, struct urb *urb)
{
	bool			 more = false;
	u8			*fifo_dest = NULL;
	u16			fifo_count = 0;
	struct musbfsh_hw_ep	*hw_ep = musbfsh->control_ep;
	struct musbfsh_qh		*qh = hw_ep->in_qh;
	struct usb_ctrlrequest	*request;
	
	INFO("musbfsh_h_ep0_continue++\r\n");
	switch (musbfsh->ep0_stage) {
	case MUSBFSH_EP0_IN:
		fifo_dest = urb->transfer_buffer + urb->actual_length;//                                         
		fifo_count = min_t(size_t, len, urb->transfer_buffer_length -
				   urb->actual_length);
		if (fifo_count < len)//                                    
			urb->status = -EOVERFLOW;

		musbfsh_read_fifo(hw_ep, fifo_count, fifo_dest);//                   

		urb->actual_length += fifo_count;//                         
		if (len < qh->maxpacket) {//                                                           
			/*                                     
                                  
                                    
    */
		} else if (urb->actual_length <
				urb->transfer_buffer_length)
			more = true;
		break;
	case MUSBFSH_EP0_START:
		request = (struct usb_ctrlrequest *) urb->setup_packet;

		if (!request->wLength) {
			INFO( "start no-DATA\n");
			break;
		} else if (request->bRequestType & USB_DIR_IN) {
			INFO( "start IN-DATA\n");
			musbfsh->ep0_stage = MUSBFSH_EP0_IN;
			more = true;
			break;//                        
		} else {
			INFO( "start OUT-DATA\n");
			musbfsh->ep0_stage = MUSBFSH_EP0_OUT;
			more = true;
			//                                   
		}
		/*             */
	case MUSBFSH_EP0_OUT:
		fifo_count = min_t(size_t, qh->maxpacket,
				   urb->transfer_buffer_length -
				   urb->actual_length);
		if (fifo_count) {
			fifo_dest = (u8 *) (urb->transfer_buffer
					+ urb->actual_length);
			INFO("Sending %d byte%s to ep0 fifo %p\n",
					fifo_count,
					(fifo_count == 1) ? "" : "s",
					fifo_dest);
			musbfsh_write_fifo(hw_ep, fifo_count, fifo_dest);

			urb->actual_length += fifo_count;
			more = true;
		}
		break;
	default:
		ERR("bogus ep0 stage %d\n", musbfsh->ep0_stage);
		break;
	}

	return more;
}

/*
                                                                     
                            
  
                                   
 */
irqreturn_t musbfsh_h_ep0_irq(struct musbfsh *musbfsh)
{
	struct urb		*urb;
	u16			csr, len;
	int			status = 0;
	void __iomem		*mbase = musbfsh->mregs;
	struct musbfsh_hw_ep	*hw_ep = musbfsh->control_ep;
	void __iomem		*epio = hw_ep->regs;
	struct musbfsh_qh		*qh = hw_ep->in_qh;
	bool			complete = false;
	irqreturn_t		retval = IRQ_NONE;
	
	INFO("musbfsh_h_ep0_irq++\r\n");
	/*                              */
	urb = next_urb(qh);

	musbfsh_ep_select(mbase, 0);
	csr = musbfsh_readw(epio, MUSBFSH_CSR0);
	len = (csr & MUSBFSH_CSR0_RXPKTRDY)
			? musbfsh_readb(epio, MUSBFSH_COUNT0)
			: 0;

	WARNING( "<== csr0 %04x, qh %p, count %d, urb %p, stage %d\n",
		csr, qh, len, urb, musbfsh->ep0_stage);

	/*                                          */
	if (MUSBFSH_EP0_STATUS == musbfsh->ep0_stage) {
		retval = IRQ_HANDLED;
		complete = true;
	}

	/*                */
	if (csr & MUSBFSH_CSR0_H_RXSTALL) {
		WARNING("STALLING ENDPOINT\n");
		status = -EPIPE;

	} else if (csr & MUSBFSH_CSR0_H_ERROR) {
		WARNING("no response, csr0 %04x\n", csr);
		status = -EPROTO;

	} else if (csr & MUSBFSH_CSR0_H_NAKTIMEOUT) {
		WARNING("control NAK timeout\n");

		/*                                                       
                                                        
                                                          
                           
    
                                                  
                                                    
   */
		musbfsh_writew(epio, MUSBFSH_CSR0, 0);
		retval = IRQ_HANDLED;
	}

	if (status) {//                                                        
		INFO("aborting\n");
		retval = IRQ_HANDLED;
		if (urb)
			urb->status = status;
		complete = true;

		/*                                               */
		if (csr & MUSBFSH_CSR0_H_REQPKT) {
			csr &= ~MUSBFSH_CSR0_H_REQPKT;
			musbfsh_writew(epio, MUSBFSH_CSR0, csr);
			csr &= ~MUSBFSH_CSR0_H_NAKTIMEOUT;
			musbfsh_writew(epio, MUSBFSH_CSR0, csr);
		} else {
			musbfsh_h_ep0_flush_fifo(hw_ep);
		}

		musbfsh_writeb(epio, MUSBFSH_NAKLIMIT0, 0);

		/*          */
		musbfsh_writew(epio, MUSBFSH_CSR0, 0);
	}

	if (unlikely(!urb)) {
		/*                                                        
                          */
		ERR("no URB for end 0\n");

		musbfsh_h_ep0_flush_fifo(hw_ep);
		goto done;
	}

	if (!complete) {//                    
		/*                                        */
		if (musbfsh_h_ep0_continue(musbfsh, len, urb)) {
			/*                       */
			//                                                                   
			csr = (MUSBFSH_EP0_IN == musbfsh->ep0_stage)
				?  MUSBFSH_CSR0_H_REQPKT : MUSBFSH_CSR0_TXPKTRDY;
		} else {
			/*                                              */
			if (usb_pipeout(urb->pipe)
					|| !urb->transfer_buffer_length)//                                                                                      
				csr = MUSBFSH_CSR0_H_STATUSPKT
					| MUSBFSH_CSR0_H_REQPKT;
			else
				csr = MUSBFSH_CSR0_H_STATUSPKT
					| MUSBFSH_CSR0_TXPKTRDY;

			/*                   */
			musbfsh->ep0_stage = MUSBFSH_EP0_STATUS;

			INFO("ep0 STATUS, csr %04x\n", csr);

		}
		musbfsh_writew(epio, MUSBFSH_CSR0, csr);
		retval = IRQ_HANDLED;
	} else
		musbfsh->ep0_stage = MUSBFSH_EP0_IDLE;

	/*                                 */
	if (complete)
	{
		//          
		musbfsh_advance_schedule(musbfsh, urb, hw_ep, 1);
	}
done:
	return retval;
}

/*                                                      
              
                                        
                                               

                                         
                                                         
                              
                                           
                           
*/

/*                                                               */
void musbfsh_host_tx(struct musbfsh *musbfsh, u8 epnum)//           
{
	int			pipe;
	bool			done = false;
	u16			tx_csr;
	size_t			length = 0;
	size_t			offset = 0;
	struct musbfsh_hw_ep	*hw_ep = musbfsh->endpoints+epnum;
	void __iomem		*epio = hw_ep->regs;
	struct musbfsh_qh		*qh = hw_ep->out_qh;
	struct urb		*urb = next_urb(qh);//                               
	u32			status = 0;//                                                            
	void __iomem		*mbase = musbfsh->mregs;
	struct dma_channel	*dma;
	bool			transfer_pending = false;
	
	INFO("musbfsh_host_tx++,real ep=%d\r\n",epnum);
	musbfsh_ep_select(mbase, epnum);
	tx_csr = musbfsh_readw(epio, MUSBFSH_TXCSR);

	/*                                                */
	if (!urb) {
		WARNING("extra TX%d ready, csr %04x\n", epnum, tx_csr);
		return;
	}

	pipe = urb->pipe;
	dma = is_dma_capable() ? hw_ep->tx_channel : NULL;
	INFO("OUT/TX%d end, csr %04x%s\n", epnum, tx_csr,
			dma ? ", dma" : "pio");

	/*                  */
	if (tx_csr & MUSBFSH_TXCSR_H_RXSTALL) {
		/*                                */
		WARNING("TX end %d stall\n", epnum);

		/*                          */
		status = -EPIPE;

	} else if (tx_csr & MUSBFSH_TXCSR_H_ERROR) {
		/*                                          */
		WARNING("TX 3strikes on ep=%d\n", epnum);

		status = -ETIMEDOUT;

	} else if (tx_csr & MUSBFSH_TXCSR_H_NAKTIMEOUT) {
		WARNING("TX end=%d device not responding\n", epnum);

		/*                                                       
                                                         
                                                       
                                               
    
                                                           
                                                    
   */
		musbfsh_ep_select(mbase, epnum);
		musbfsh_writew(epio, MUSBFSH_TXCSR,
				MUSBFSH_TXCSR_H_WZC_BITS
				| MUSBFSH_TXCSR_TXPKTRDY);
		return;
	}

	if (status) {//                                                    
		if (dma_channel_status(dma) == MUSBFSH_DMA_STATUS_BUSY) {
			dma->status = MUSBFSH_DMA_STATUS_CORE_ABORT;
			(void) musbfsh->dma_controller->channel_abort(dma);
		}

		/*                                                    
                                                        
   */
		musbfsh_h_tx_flush_fifo(hw_ep);
		tx_csr &= ~(MUSBFSH_TXCSR_AUTOSET
				| MUSBFSH_TXCSR_DMAENAB
				| MUSBFSH_TXCSR_H_ERROR
				| MUSBFSH_TXCSR_H_RXSTALL
				| MUSBFSH_TXCSR_H_NAKTIMEOUT
				);

		musbfsh_ep_select(mbase, epnum);
		musbfsh_writew(epio, MUSBFSH_TXCSR, tx_csr);
		/*                                         */
		musbfsh_writew(epio, MUSBFSH_TXCSR, tx_csr);
		musbfsh_writeb(epio, MUSBFSH_TXINTERVAL, 0);

		done = true;
	}

	/*                  */
	if (dma_channel_status(dma) == MUSBFSH_DMA_STATUS_BUSY) {
		WARNING("extra TX%d ready, csr %04x\n", epnum, tx_csr);
		return;
	}

	if (is_dma_capable() && dma && !status) {
		/*
                                                             
                                                              
                                                           
                                                             
                                                            
                               
   */
		 if (tx_csr & MUSBFSH_TXCSR_DMAMODE) {
			/*
                                                     
                                                    
                                                     
                                                        
                                                        
                                                       
                               
     
                                                         
                                                          
                                                           
                                                         
                                                        
                            
    */
			tx_csr &= musbfsh_readw(epio, MUSBFSH_TXCSR);
			if (tx_csr & MUSBFSH_TXCSR_TXPKTRDY) {
				tx_csr &= ~(MUSBFSH_TXCSR_DMAENAB |
					    MUSBFSH_TXCSR_TXPKTRDY);
				musbfsh_writew(epio, MUSBFSH_TXCSR,
					    tx_csr | MUSBFSH_TXCSR_H_WZC_BITS);
			}
			tx_csr &= ~(MUSBFSH_TXCSR_DMAMODE |
				    MUSBFSH_TXCSR_TXPKTRDY);
			musbfsh_writew(epio, MUSBFSH_TXCSR,
				    tx_csr | MUSBFSH_TXCSR_H_WZC_BITS);

			/*
                                                       
                                                       
                                                          
                                                          
    */
			tx_csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
		}

		/*
                                                                 
                                                                 
                                                               
                                                               
                     
   */
		if (tx_csr & (MUSBFSH_TXCSR_FIFONOTEMPTY | MUSBFSH_TXCSR_TXPKTRDY)) {
			INFO("DMA complete but packet still in FIFO, "
			    "CSR %04x\n", tx_csr);
			return;
		}
	}
    
    if (!status || dma) {
		if (dma)
			length = dma->actual_len;
		else
			length = qh->segsize;
		qh->offset += length;

		if (dma && urb->transfer_buffer_length == qh->offset) {
			done = true;
		} else {
			/*                                          */
			if (qh->segsize < qh->maxpacket)//                   
				done = true;
			else if (qh->offset == urb->transfer_buffer_length
					&& !(urb->transfer_flags
						& URB_ZERO_PACKET))
				done = true;
			if (!done) {
				offset = qh->offset;
				length = urb->transfer_buffer_length - offset;				
				transfer_pending = true;
			}
		}
	}

	/*                                                            
                                                
  */
	if (urb->status != -EINPROGRESS) {
		done = true;
		if (status == 0)
			status = urb->status;
	}

	if (done) {
		/*            */
		urb->status = status;
		urb->actual_length = qh->offset;
		musbfsh_advance_schedule(musbfsh, urb, hw_ep, USB_DIR_OUT);
		return;
	} else if (transfer_pending && dma) {
		if (musbfsh_tx_dma_program(musbfsh->dma_controller, hw_ep, qh, urb,
				offset, length)) {
			return;
		}
	}  else	if (tx_csr & MUSBFSH_TXCSR_DMAENAB) {
		WARNING("not complete, but DMA enabled?\n");
		return;
	}

	/*
                                       
   
                                                               
                                                                 
                                                         
  */
	if (length > qh->maxpacket)
		length = qh->maxpacket;
	/*                                         */
	usb_hcd_unmap_urb_for_dma(musbfsh_to_hcd(musbfsh), urb);
	musbfsh_write_fifo(hw_ep, length, urb->transfer_buffer + offset);
	qh->segsize = length;

	musbfsh_ep_select(mbase, epnum);
	musbfsh_writew(epio, MUSBFSH_TXCSR,
			MUSBFSH_TXCSR_H_WZC_BITS | MUSBFSH_TXCSR_TXPKTRDY);
}


/*                                                     
              
                                       
                                                  
                      
                               
                                     
                                               
                                                          
                                                          
                             
         
                            
  
                     
                                                              
                            
                                                                        
                                                                        
                                       
  
          
                                                 
                                                                      
                                                                  
                                
                                                                   
                                                                
                                                                      
  
                                                               
                                                                 
                                     
 */

/*                                                                  
                                                  
 */
static void musbfsh_bulk_rx_nak_timeout(struct musbfsh *musbfsh, struct musbfsh_hw_ep *ep)
{
	struct dma_channel	*dma;
	struct urb		*urb;
	void __iomem		*mbase = musbfsh->mregs;
	void __iomem		*epio = ep->regs;
	struct musbfsh_qh		*cur_qh, *next_qh;
	u16			rx_csr;
	
	INFO("musbfsh_bulk_rx_nak_timeout++\r\n");
	musbfsh_ep_select(mbase, ep->epnum);
	dma = is_dma_capable() ? ep->rx_channel : NULL;

	/*                       */
	rx_csr = musbfsh_readw(epio, MUSBFSH_RXCSR);
	rx_csr |= MUSBFSH_RXCSR_H_WZC_BITS;
	rx_csr &= ~MUSBFSH_RXCSR_DATAERROR;
	musbfsh_writew(epio, MUSBFSH_RXCSR, rx_csr);

	cur_qh = first_qh(&musbfsh->in_bulk);
	if (cur_qh) {
		urb = next_urb(cur_qh);
		if (dma_channel_status(dma) == MUSBFSH_DMA_STATUS_BUSY) {
			dma->status = MUSBFSH_DMA_STATUS_CORE_ABORT;
			musbfsh->dma_controller->channel_abort(dma);
			urb->actual_length += dma->actual_len;
			dma->actual_len = 0L;
		}
		musbfsh_save_toggle(cur_qh, 1, urb);

		/*                             */
		list_move_tail(&cur_qh->ring, &musbfsh->in_bulk);

		/*                                       */
		next_qh = first_qh(&musbfsh->in_bulk);

		/*                                        */
		ep->rx_reinit = 1;
		//                                
		musbfsh_start_urb(musbfsh, 1, next_qh);
	}
}

/*
                                                                           
                                        
 */
void musbfsh_host_rx(struct musbfsh *musbfsh, u8 epnum)
{
	struct urb		*urb;
	struct musbfsh_hw_ep	*hw_ep = musbfsh->endpoints+epnum;
	void __iomem		*epio = hw_ep->regs;
	struct musbfsh_qh		*qh = hw_ep->in_qh;
	size_t			xfer_len;
	void __iomem		*mbase = musbfsh->mregs;
	int			pipe;
	u16			rx_csr, val;
	bool			done = false;
	u32			status;
	struct dma_channel	*dma;
    
	INFO("musbfsh_host_rx++,real ep=%d\r\n",epnum);
	musbfsh_ep_select(mbase, epnum);

	urb = next_urb(qh);//           
	dma = is_dma_capable() ? hw_ep->rx_channel : NULL;
	status = 0;
	xfer_len = 0;

	rx_csr = musbfsh_readw(epio, MUSBFSH_RXCSR);
	val = rx_csr;

	if (unlikely(!urb)) {
		/*                                                      
                                                           
                                       
   */
		WARNING("BOGUS RX%d ready, csr %04x, count %d\n", epnum, val,
			musbfsh_readw(epio, MUSBFSH_RXCOUNT));
		musbfsh_h_flush_rxfifo(hw_ep, 0);
		return;
	}

	pipe = urb->pipe;

	INFO("<==real hw %d rxcsr %04x, urb actual %d (+dma %zu)\n",
		epnum, rx_csr, urb->actual_length,
		dma ? dma->actual_len : 0);

	/*                                                          
                 */
	if (rx_csr & MUSBFSH_RXCSR_H_RXSTALL) {
		WARNING("RX end %d STALL\n", epnum);
		rx_csr &= ~MUSBFSH_RXCSR_H_RXSTALL;
        musbfsh_writew(epio, MUSBFSH_RXCSR, rx_csr);
		/*                          */
		status = -EPIPE;

	} else if (rx_csr & MUSBFSH_RXCSR_H_ERROR) {
		WARNING("end %d RX proto error\n", epnum);

		status = -EPROTO;
		musbfsh_writeb(epio, MUSBFSH_RXINTERVAL, 0);

	} else if (rx_csr & MUSBFSH_RXCSR_DATAERROR) {
		INFO("RX end %d NAK timeout\n", epnum); //                             

		/*                                              
                                                  
                                                
   
                                                  
                                                
                                     
  */
		if (usb_pipebulk(urb->pipe)
				&& qh->mux == 1
				&& !list_is_singular(&musbfsh->in_bulk)) {
			musbfsh_bulk_rx_nak_timeout(musbfsh, hw_ep);
			return;
		}
		musbfsh_ep_select(mbase, epnum);
		rx_csr |= MUSBFSH_RXCSR_H_WZC_BITS;
		rx_csr &= ~MUSBFSH_RXCSR_DATAERROR;
		musbfsh_writew(epio, MUSBFSH_RXCSR, rx_csr);
		goto finish;	
	} else if (rx_csr & MUSBFSH_RXCSR_INCOMPRX) {
		WARNING("end %d high bandwidth incomplete ISO packet RX\n", epnum);
		status = -EPROTO;
	}

	/*                           */
	if (status) {
		/*                                         */
		if (dma_channel_status(dma) == MUSBFSH_DMA_STATUS_BUSY) {
			dma->status = MUSBFSH_DMA_STATUS_CORE_ABORT;
			(void) musbfsh->dma_controller->channel_abort(dma);
			xfer_len = dma->actual_len;
		}
        musbfsh_h_flush_rxfifo(hw_ep, 0);
		musbfsh_writeb(epio, MUSBFSH_RXINTERVAL, 0);

#ifdef CONFIG_MTK_DT_USB_SUPPORT
		extern	int musbfsh_connect_flag;	
		if(!musbfsh_connect_flag){
			MYDBG("err(%d) after disc\n", status);
			return;
		}
#endif

		done = true;
		goto finish;
	}

	if (unlikely(dma_channel_status(dma) == MUSBFSH_DMA_STATUS_BUSY)) {
		/*                                                          */
		ERR("RX%d dma busy, csr %04x\n", epnum, rx_csr);
		goto finish;
	}

	/*                                                                
                                                                   
                                                      
  */

	/*                                                           */

	if (dma && (rx_csr & MUSBFSH_RXCSR_DMAENAB)) {//                                                           
		xfer_len = dma->actual_len;

		val &= ~(MUSBFSH_RXCSR_DMAENAB
			| MUSBFSH_RXCSR_H_AUTOREQ
			| MUSBFSH_RXCSR_AUTOCLEAR
			| MUSBFSH_RXCSR_RXPKTRDY);//                
		musbfsh_writew(hw_ep->regs, MUSBFSH_RXCSR, val);


	/*                                                    */
	done = (urb->actual_length + xfer_len >=
			urb->transfer_buffer_length
		|| dma->actual_len < qh->maxpacket);
		

	/*                                                */
	if (!done) {
		val |= MUSBFSH_RXCSR_H_REQPKT;
		musbfsh_writew(epio, MUSBFSH_RXCSR,
			MUSBFSH_RXCSR_H_WZC_BITS | val);
	}

	INFO( "ep %d dma %s, rxcsr %04x, rxcount %d\n", epnum,
		done ? "off" : "reset",
		musbfsh_readw(epio, MUSBFSH_RXCSR),
		musbfsh_readw(epio, MUSBFSH_RXCOUNT));
	} else if (urb->status == -EINPROGRESS) {
		/*                                                       */
		if (unlikely(!(rx_csr & MUSBFSH_RXCSR_RXPKTRDY))) {
			status = -EPROTO;
			ERR("Rx interrupt with no errors or packet!\n");

			/*                                             */

/*            */
			/*                                              */
			musbfsh_ep_select(mbase, epnum);
			val &= ~MUSBFSH_RXCSR_H_REQPKT;
			musbfsh_writew(epio, MUSBFSH_RXCSR, val);
			goto finish;
		}

		/*                             */
		if (dma) {
			struct dma_controller	*c;
			u16			rx_count;
			int			ret, length;
			dma_addr_t		buf;

			rx_count = musbfsh_readw(epio, MUSBFSH_RXCOUNT);

			INFO("RX%d count %d, buffer 0x%x len %d/%d\n",
					epnum, rx_count,
					(unsigned int)urb->transfer_dma
					+ urb->actual_length,
					qh->offset,
					urb->transfer_buffer_length);

			c = musbfsh->dma_controller;

			length = rx_count;
			buf = urb->transfer_dma + urb->actual_length;
			dma->desired_mode = 0;

#ifdef USE_MODE1
			/*                                        
                                                
    */
			if ((urb->transfer_flags &
						URB_SHORT_NOT_OK)
				&& (urb->transfer_buffer_length -
						urb->actual_length)
					> qh->maxpacket)
				dma->desired_mode = 1;
			if (rx_count < hw_ep->max_packet_sz_rx) {
				length = rx_count;
				dma->desired_mode = 0;
			} else {
				length = urb->transfer_buffer_length;
			}
#endif
			
			/*                              
                                                                        
                                                                
     
              
                                                                           
                                                                          
                                                                          
                                                                          
                                                                         
                                                                          
                                                                            
                                                                   
                                                                          
                                                                          
    */
			
			val = musbfsh_readw(epio, MUSBFSH_RXCSR);
			val &= ~MUSBFSH_RXCSR_H_REQPKT;
						
			if (dma->desired_mode == 0)
				val &= ~MUSBFSH_RXCSR_H_AUTOREQ;
			else
				val |= MUSBFSH_RXCSR_H_AUTOREQ;
			
			val |= MUSBFSH_RXCSR_DMAENAB;
			
			musbfsh_writew(epio, MUSBFSH_RXCSR,
				MUSBFSH_RXCSR_H_WZC_BITS | val);

			/*                                    
                                        
                       
    */
			ret = c->channel_program(
				dma, qh->maxpacket,//                                                                                             
				dma->desired_mode, buf, length);

			if (!ret) {
				c->channel_release(dma);
				hw_ep->rx_channel = NULL;
				dma = NULL;
				/*                   */
			}
		}

		if (!dma) {			
			/*                                         */
			usb_hcd_unmap_urb_for_dma(musbfsh_to_hcd(musbfsh), urb);
			done = musbfsh_host_packet_rx(musbfsh, urb, epnum);
			INFO("read %spacket\n", done ? "last " : "");
		}
	}

finish:
	urb->actual_length += xfer_len;
	qh->offset += xfer_len;
	if (done) {
		if (urb->status == -EINPROGRESS)
			urb->status = status;
		musbfsh_advance_schedule(musbfsh, urb, hw_ep, USB_DIR_IN);
	}
}

/*                                                                    
                                                                    
                                                                   
                          
 */
static int musbfsh_schedule(
	struct musbfsh		*musbfsh,
	struct musbfsh_qh		*qh,
	int			is_in)
{
	int			idle;
	int			best_diff;
	int			best_end, epnum;
	struct musbfsh_hw_ep	*hw_ep = NULL;
	struct list_head	*head = NULL;
	
	INFO("musbfsh_schedule++,qh->epnum=%d,is_in=%d\r\n",qh->epnum,(unsigned int)is_in);
	/*                                         */
	if (qh->type == USB_ENDPOINT_XFER_CONTROL) {
		head = &musbfsh->control;
		hw_ep = musbfsh->control_ep;
		goto success;
	}

	/*                                                       */

	/*
                                                               
                                                      
   
                                                               
                        
  */
	best_diff = 4096;
	best_end = -1;

	for (epnum = 1, hw_ep = musbfsh->endpoints + 1;
			epnum < musbfsh->nr_endpoints;
			epnum++, hw_ep++) {
		int	diff;

		if (musbfsh_ep_get_qh(hw_ep, is_in) != NULL)
			continue;

		if (hw_ep == musbfsh->bulk_ep)
			continue;

		if (is_in)
			diff = hw_ep->max_packet_sz_rx;
		else
			diff = hw_ep->max_packet_sz_tx;
		diff -= qh->maxpacket;

		if (diff >= 0 && best_diff > diff) {
			hw_ep = musbfsh->endpoints + epnum;

			best_diff = diff;
			best_end = epnum;
		}
	}
	/*                                              */
	if (best_end < 0 && qh->type == USB_ENDPOINT_XFER_BULK) {
		hw_ep = musbfsh->bulk_ep;
		if (is_in)
			head = &musbfsh->in_bulk;
		else
			head = &musbfsh->out_bulk;

		/*                                                         
                                                                
                                                           
                                                        
                                                              
                                      
   */
		if (is_in && qh->dev)
			qh->intv_reg =
				(USB_SPEED_HIGH == qh->dev->speed) ? 8 : 4;
		goto success;
	} else if (best_end < 0) {
		return -ENOSPC;
	}

	idle = 1;
	qh->mux = 0;
	hw_ep = musbfsh->endpoints + best_end;
	INFO("qh %p periodic slot %d\n", qh, best_end);
success:
	if (head) {
#ifdef CONFIG_MTK_DT_USB_SUPPORT
		MYDBG("head!=NULL\n");
#endif
		idle = list_empty(head);
		list_add_tail(&qh->ring, head);
		qh->mux = 1;
	}
	qh->hw_ep = hw_ep;
	qh->hep->hcpriv = qh;
	if (idle)//                                                  
	{
#ifdef CONFIG_MTK_DT_USB_SUPPORT
		mark_qh_activity(qh->epnum, hw_ep->epnum, is_in, 0);
#endif
		musbfsh_start_urb(musbfsh, is_in, qh);
	}
	return 0;
}

static int musbfsh_urb_enqueue(
	struct usb_hcd			*hcd,
	struct urb			*urb,
	gfp_t				mem_flags)
{
	unsigned long			flags;
	struct musbfsh			*musbfsh = hcd_to_musbfsh(hcd);
	struct usb_host_endpoint	*hep = urb->ep;
	struct musbfsh_qh			*qh;
	struct usb_endpoint_descriptor	*epd = &hep->desc;
	int				ret;
	unsigned			type_reg;
	unsigned			interval;
	
	INFO("musbfsh_urb_enqueue++:urb addr=0x%p\r\n",urb);


	//                                                                                                                         
#if 1 //                                                                                                                  
	if(usb_endpoint_num(epd)==0)
		urb->transfer_flags &= ~URB_DMA_MAP_SINGLE;
#endif
	spin_lock_irqsave(&musbfsh->lock, flags);
	ret = usb_hcd_link_urb_to_ep(hcd, urb); //                                              
	qh = ret ? NULL : hep->hcpriv;
	if (qh)
		urb->hcpriv = qh;
	spin_unlock_irqrestore(&musbfsh->lock, flags);

	/*                                                            
                                                              
                             
   
                                                                  
                                                                    
                                                          
  */
	if (qh || ret)
		return ret;

	/*                                                               
                                                                     
   
                                                              
                                                         
  */
	qh = kzalloc(sizeof *qh, mem_flags);
	if (!qh) {
		spin_lock_irqsave(&musbfsh->lock, flags);
		usb_hcd_unlink_urb_from_ep(hcd, urb);
		spin_unlock_irqrestore(&musbfsh->lock, flags);
		return -ENOMEM;
	}

	qh->hep = hep;
	qh->dev = urb->dev;
	INIT_LIST_HEAD(&qh->ring);
	qh->is_ready = 1;

	qh->maxpacket = le16_to_cpu(epd->wMaxPacketSize);
	qh->type = usb_endpoint_type(epd);
	INFO("desc type=%d\r\n",qh->type);

	qh->epnum = usb_endpoint_num(epd);
	INFO("desc epnum=%d\r\n",qh->epnum);

	/*                                                    */
	qh->addr_reg = (u8) usb_pipedevice(urb->pipe);
	INFO("desc pipe=0x%x, desc devnum=%d\r\n", urb->pipe, urb->dev->devnum);

	/*                                         */
	type_reg = (qh->type << 4) | qh->epnum;
	switch (urb->dev->speed) {
	case USB_SPEED_LOW:
		type_reg |= 0xc0;
		break;
	case USB_SPEED_FULL:
		type_reg |= 0x80;
		break;
	default:
		type_reg |= 0x40;
	}
	qh->type_reg = type_reg;

	/*                                           */
	switch (qh->type) {
	case USB_ENDPOINT_XFER_INT:
		/*
                                              
                                              
   */
		if (urb->dev->speed <= USB_SPEED_FULL) {
			interval = max_t(u8, epd->bInterval, 1);
			break;
		}
	default:
		/*                                                           
                                                             
                      
    
                                                             
    
                                                               
                                                              
                                                              
                                                             
                                          
    
                                                               
   */
		interval = 0;
	}
	qh->intv_reg = interval;
	
	/*                                                 */
	if(musbfsh->is_multipoint) {
		struct usb_device	*parent = urb->dev->parent;
		
		if (parent != hcd->self.root_hub) {
			qh->h_addr_reg = (u8) parent->devnum;
			
			/*                          */
			if (urb->dev->tt) {
				qh->h_port_reg = (u8) urb->dev->ttport;
				if (urb->dev->tt->hub)
					qh->h_addr_reg =
						(u8) urb->dev->tt->hub->devnum;
				if (urb->dev->tt->multi)
					qh->h_addr_reg |= 0x80;
			}
		}
		INFO("addr_reg=0x%x,h_addr_reg=0x%x,h_port_reg=0x%x",qh->addr_reg,qh->h_addr_reg,qh->h_port_reg);
	}

	/*                                                                   
                                                                     
                                               
  */
	spin_lock_irqsave(&musbfsh->lock, flags);
	if (hep->hcpriv) {
		/*                                                         
                                       
   */		
		kfree(qh);
		qh = NULL;
		ret = 0;
	} else
	{
		ret = musbfsh_schedule(musbfsh, qh,
				epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK);
		//                                                                                            
	}

	if (ret == 0) {
		urb->hcpriv = qh;
		/*                                                        
                                                               
   */
	}
	spin_unlock_irqrestore(&musbfsh->lock, flags);

	if (ret != 0) {
		spin_lock_irqsave(&musbfsh->lock, flags);
		usb_hcd_unlink_urb_from_ep(hcd, urb);
		spin_unlock_irqrestore(&musbfsh->lock, flags);
		kfree(qh);
	}
	return ret;
}

/*
                                                           
                                              
                                                                      
 */
static int musbfsh_cleanup_urb(struct urb *urb, struct musbfsh_qh *qh)
{
	struct musbfsh_hw_ep	*ep = qh->hw_ep;
	void __iomem		*epio = ep->regs;
	unsigned		hw_end = ep->epnum;
	void __iomem		*regs = ep->musbfsh->mregs;
	int			is_in = usb_pipein(urb->pipe);
	int			status = 0;
	u16			csr;
	
	INFO("musbfsh_cleanup_urb++\r\n");
	musbfsh_ep_select(regs, hw_end);

	if (is_dma_capable()) {
		struct dma_channel	*dma;

		dma = is_in ? ep->rx_channel : ep->tx_channel;
		if (dma) {
			status = ep->musbfsh->dma_controller->channel_abort(dma);
			WARNING("abort %cX%d DMA for urb %p --> %d\n",
				is_in ? 'R' : 'T', ep->epnum,
				urb, status);
			urb->actual_length += dma->actual_len;
		}
	}

	/*                                                        */
	if (is_in) {
		/*                            */
		csr = musbfsh_h_flush_rxfifo(ep, 0);

		/*                                                     
                                                    
                                                 
   */
	} else if (ep->epnum) {
		musbfsh_h_tx_flush_fifo(ep);
		csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
		csr &= ~(MUSBFSH_TXCSR_AUTOSET
			| MUSBFSH_TXCSR_DMAENAB
			| MUSBFSH_TXCSR_H_RXSTALL
			| MUSBFSH_TXCSR_H_NAKTIMEOUT
			| MUSBFSH_TXCSR_H_ERROR
			| MUSBFSH_TXCSR_TXPKTRDY);
		musbfsh_writew(epio, MUSBFSH_TXCSR, csr);
		/*                                         */
		musbfsh_writew(epio, MUSBFSH_TXCSR, csr);
		/*                       */
		csr = musbfsh_readw(epio, MUSBFSH_TXCSR);
	} else  {
		musbfsh_h_ep0_flush_fifo(ep);
	}
	if (status == 0)
		musbfsh_advance_schedule(ep->musbfsh, urb, ep, is_in);
	return status;
}

static int musbfsh_urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status)
{
	struct musbfsh		*musbfsh = hcd_to_musbfsh(hcd);
	struct musbfsh_qh		*qh;
	unsigned long		flags;
	int			is_in  = usb_pipein(urb->pipe);
	int			ret;
	
	INFO("urb=%p, dev%d ep%d%s\n", urb,
			usb_pipedevice(urb->pipe),
			usb_pipeendpoint(urb->pipe),
			is_in ? "in" : "out");

	spin_lock_irqsave(&musbfsh->lock, flags);
	ret = usb_hcd_check_unlink_urb(hcd, urb, status);
	if (ret)
		goto done;

	qh = urb->hcpriv;
	if (!qh)
		goto done;

	/*
                                                                 
                                                                
                                                                    
                                                                 
   
                                                                    
                                                                   
                                                
   
                                                          
  */
	if (!qh->is_ready
			|| urb->urb_list.prev != &qh->hep->urb_list
			|| musbfsh_ep_get_qh(qh->hw_ep, is_in) != qh) {
		int	ready = qh->is_ready;

		qh->is_ready = 0;
		musbfsh_giveback(musbfsh, urb, 0);
		qh->is_ready = ready;

		/*                                                       
                                                   
   */
		if (ready && list_empty(&qh->hep->urb_list)) {
			qh->hep->hcpriv = NULL;
			list_del(&qh->ring);
			kfree(qh);
		}
	} else
		ret = musbfsh_cleanup_urb(urb, qh);
done:
	spin_unlock_irqrestore(&musbfsh->lock, flags);
	return ret;
}

/*                     */
static void
musbfsh_h_disable(struct usb_hcd *hcd, struct usb_host_endpoint *hep)
{
	u8			is_in = hep->desc.bEndpointAddress & USB_DIR_IN;
	unsigned long		flags;
	struct musbfsh		*musbfsh = hcd_to_musbfsh(hcd);
	struct musbfsh_qh		*qh;
	struct urb		*urb;
	
	WARNING("musbfsh_h_disable++:ep:0x%x\r\n",hep->desc.bEndpointAddress);	
	spin_lock_irqsave(&musbfsh->lock, flags);

	qh = hep->hcpriv;
	if (qh == NULL){
		MYDBG("qh == NULL\n");
		goto exit;
	}

	/*                                                        */

	/*                                                */
	qh->is_ready = 0;
	if (musbfsh_ep_get_qh(qh->hw_ep, is_in) == qh) {
		urb = next_urb(qh);
		
		/*                                                                     
                                                   */

		/*                                                                                   */
#if 1	
		if(!virt_addr_valid(urb))
		{
			MYDBG("urb(%p) addr error\n", urb);
			goto exit;
		}
#endif
		/*                                         */
		if (!urb->unlinked)
			urb->status = -ESHUTDOWN;

		/*         */
		musbfsh_cleanup_urb(urb, qh);

		/*                                             
                                                     
   */
		while (!list_empty(&hep->urb_list)) {
			urb = next_urb(qh);
			urb->status = -ESHUTDOWN;
			musbfsh_advance_schedule(musbfsh, urb, qh->hw_ep, is_in);
		}
	} else {
		/*                                                
                                                     
                                               
   */
		while (!list_empty(&hep->urb_list))
			musbfsh_giveback(musbfsh, next_urb(qh), -ESHUTDOWN);

		hep->hcpriv = NULL;
		list_del(&qh->ring);
		kfree(qh);
	}
exit:
	spin_unlock_irqrestore(&musbfsh->lock, flags);
}

static int musbfsh_h_get_frame_number(struct usb_hcd *hcd)
{
	struct musbfsh	*musbfsh = hcd_to_musbfsh(hcd);
	
	return musbfsh_readw(musbfsh->mregs, MUSBFSH_FRAME);
}

static int musbfsh_h_start(struct usb_hcd *hcd)
{
	struct musbfsh	*musbfsh = hcd_to_musbfsh(hcd);
	
	INFO("musbfsh_h_start++\r\n");
	/*                                                          
                                                   
  */
	hcd->state = HC_STATE_RUNNING;
	musbfsh->port1_status = 0;
	return 0;
}

static void musbfsh_h_stop(struct usb_hcd *hcd)
{
	INFO("musbfsh_h_stop++\r\n");
	musbfsh_stop(hcd_to_musbfsh(hcd));
	hcd->state = HC_STATE_HALT;
}

//                               
static int musbfsh_bus_suspend(struct usb_hcd *hcd)
{
    struct musbfsh	*musbfsh = hcd_to_musbfsh(hcd);
    unsigned char power = musbfsh_readb(musbfsh->mregs,MUSBFSH_POWER);
	
    WARNING("musbfsh_bus_suspend++,power=0x%x\r\n",power);
#ifdef CONFIG_MTK_DT_USB_SUPPORT
#if defined(CONFIG_PM_RUNTIME) 
	usb11_plat_suspend();
#endif
#endif

#ifdef CONFIG_MTK_DT_USB_SUPPORT
#if defined(CONFIG_PM_RUNTIME) && defined(USB11_REMOTE_IRQ_NON_AUTO_MASK)
	enable_remote_wake_up();	
#endif
#endif

#ifdef MTK_USB_RUNTIME_SUPPORT
	//                                                                                                
	mt_eint_unmask(CUST_EINT_MT6280_USB_WAKEUP_NUM);
#endif
#if 0 //                                                                                         
    power |= MUSBFSH_POWER_SUSPENDM|MUSBFSH_POWER_ENSUSPEND;
    musbfsh_writeb(musbfsh->mregs,MUSBFSH_POWER,power);
    mdelay(15);
#endif
    return 0;
}

//                              
static int musbfsh_bus_resume(struct usb_hcd *hcd)
{
	/*                                   */
    struct musbfsh	*musbfsh = hcd_to_musbfsh(hcd);
    unsigned char power; 
	
#ifdef CONFIG_MTK_DT_USB_SUPPORT
#if defined(CONFIG_PM_RUNTIME) 
	usb11_plat_resume();
	return 0;
#endif
#endif

#ifdef MTK_USB_RUNTIME_SUPPORT
	mt_eint_mask(CUST_EINT_MT6280_USB_WAKEUP_NUM);
#endif
	power = musbfsh_readb(musbfsh->mregs,MUSBFSH_POWER);
    WARNING("musbfsh_bus_resume++,power=0x%x\r\n",power);
#if 0 //                                                                                        
    power |= MUSBFSH_POWER_RESUME;
    power &= ~MUSBFSH_POWER_SUSPENDM;
    musbfsh_writeb(musbfsh->mregs,MUSBFSH_POWER,power);
    mdelay(30);
    power &= ~MUSBFSH_POWER_RESUME;
    musbfsh_writeb(musbfsh->mregs,MUSBFSH_POWER,power);
#endif
	return 0;
}

struct hc_driver musbfsh_hc_driver = {
	.description		= "musbfsh-hcd",
	.product_desc		= "MUSBFSH HDRC host driver",
	.hcd_priv_size		= sizeof(struct musbfsh),
	.flags			= HCD_USB2 | HCD_MEMORY,

	/*                                                         
                                                             
  */

	.start			= musbfsh_h_start,
	.stop			= musbfsh_h_stop,

	.get_frame_number	= musbfsh_h_get_frame_number,

	.urb_enqueue		= musbfsh_urb_enqueue,
	.urb_dequeue		= musbfsh_urb_dequeue,
	.endpoint_disable	= musbfsh_h_disable,

	.hub_status_data	= musbfsh_hub_status_data,
	.hub_control		= musbfsh_hub_control,
	.bus_suspend		= musbfsh_bus_suspend,
	.bus_resume		= musbfsh_bus_resume,
	/*                           */
	/*                         */
};
