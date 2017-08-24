/*
 * MUSB OTG peripheral driver ep0 handling
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

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>

#include "musb_core.h"
#include <linux/mu3d/hal/mu3d_hal_osal.h>
#include <linux/mu3d/hal/mu3d_hal_usb_drv.h>
#include <linux/mu3d/hal/mu3d_hal_hw.h>

/*                                        */
#define	next_ep0_request(musb)	next_in_request(&(musb)->endpoints[0])

/*
                                                                           
                                      
  
                                                                       
                                                                        
                                                                          
                                                        
 */

static char *decode_ep0stage(u8 stage)
{
	switch (stage) {
	case MUSB_EP0_STAGE_IDLE:	return "idle";
	case MUSB_EP0_STAGE_SETUP:	return "setup";
	case MUSB_EP0_STAGE_TX:		return "in";
	case MUSB_EP0_STAGE_RX:		return "out";
	case MUSB_EP0_STAGE_ACKWAIT:	return "wait";
	case MUSB_EP0_STAGE_STATUSIN:	return "in/status";
	case MUSB_EP0_STAGE_STATUSOUT:	return "out/status";
	default:			return "?";
	}
}


static int
forward_to_driver(struct musb *musb, const struct usb_ctrlrequest *ctrlrequest)
__releases(musb->lock)
__acquires(musb->lock)
{
	int usb_state = 0;
	int retval;

	os_printk(K_DEBUG, "%s\n", __func__);
	if (!musb->gadget_driver)
		return -EOPNOTSUPP;
	spin_unlock(&musb->lock);
	retval = musb->gadget_driver->setup(&musb->g, ctrlrequest);
	os_printk(K_DEBUG, "%s retval=%d\n", __func__, retval);

	if(ctrlrequest->bRequest == USB_REQ_SET_CONFIGURATION){
        if (ctrlrequest->wValue & 0xff)
	        usb_state = USB_CONFIGURED;
        else
	        usb_state = USB_UNCONFIGURED;
	    musb_sync_with_bat(musb,usb_state); /*                        */
	}
	spin_lock(&musb->lock);
	return retval;
}


/*                                     
                                         
 */
static int service_tx_status_request(
	struct musb *musb,
	const struct usb_ctrlrequest *ctrlrequest)
{
	int handled = 1, maxp;
	u8 result[2], epnum = 0;
	const u8 recip = ctrlrequest->bRequestType & USB_RECIP_MASK;

	os_printk(K_DEBUG, "%s\n", __func__);

	result[1] = 0;

	switch (recip) {
	case USB_RECIP_DEVICE:
		result[0] = musb->is_self_powered << USB_DEVICE_SELF_POWERED;
		result[0] |= musb->may_wakeup << USB_DEVICE_REMOTE_WAKEUP;
		//               
		if (musb->g.speed == USB_SPEED_SUPER)
		{
			result[0] |= musb->g.pwr_params.bU1Enabled << USB_DEV_STAT_U1_ENABLED;
			result[0] |= musb->g.pwr_params.bU2Enabled << USB_DEV_STAT_U2_ENABLED;
		}

		if (musb->g.is_otg) {
			result[0] |= musb->g.b_hnp_enable
				<< USB_DEVICE_B_HNP_ENABLE;
			result[0] |= musb->g.a_alt_hnp_support
				<< USB_DEVICE_A_ALT_HNP_SUPPORT;
			result[0] |= musb->g.a_hnp_support
				<< USB_DEVICE_A_HNP_SUPPORT;
		}
		os_printk(K_DEBUG, "%s result=%x, U1=%x, U2=%x\n", __func__, result[0], musb->g.pwr_params.bU1Enabled, musb->g.pwr_params.bU2Enabled);
		break;

	case USB_RECIP_INTERFACE:
		result[0] = 0;
		break;

	case USB_RECIP_ENDPOINT: {
		int		is_in;
		struct musb_ep	*ep;
		u32		tmp;

		epnum = (u8) ctrlrequest->wIndex;
		if (!epnum) {
			result[0] = 0;
			break;
		}

		is_in = epnum & USB_DIR_IN;
		if (is_in) {
			epnum &= 0x0f;
			ep = &musb->endpoints[epnum].ep_in;
		} else {
			ep = &musb->endpoints[epnum].ep_out;
		}

		if (epnum >= MUSB_C_NUM_EPS || !ep->desc) {
			handled = -EINVAL;
			break;
		}

		if (is_in)
		{
            tmp = (USB_ReadCsr32(U3D_TX1CSR0, epnum) & TX_SENDSTALL) || (USB_ReadCsr32(U3D_TX1CSR0, epnum) & TX_SENTSTALL);
            //                                                       
		}
		else
		{
			tmp = (USB_ReadCsr32(U3D_RX1CSR0, epnum) & RX_SENDSTALL) || (USB_ReadCsr32(U3D_RX1CSR0, epnum) & RX_SENTSTALL);
		}

		result[0] = tmp ? 1 : 0;
		} break;

	default:
		/*                                 */
		handled = 0;
		break;
	}

	/*                                       */
	if (handled > 0) {
		u16	len = le16_to_cpu(ctrlrequest->wLength);

		if (len > 2)
			len = 2;
		maxp = musb->endpoints->max_packet_sz_tx;
		mu3d_hal_write_fifo( 0,len,result, maxp);
	}

	return handled;
}

/*
                                                                            
                                                                         
                            
  
                                            
                 
                                    
  
                                         
 */
static int
service_in_request(struct musb *musb, const struct usb_ctrlrequest *ctrlrequest)
{
	int handled = 0;	/*             */

	os_printk(K_DEBUG, "%s\n", __func__);

	if ((ctrlrequest->bRequestType & USB_TYPE_MASK)
			== USB_TYPE_STANDARD) {
		switch (ctrlrequest->bRequest) {
		case USB_REQ_GET_STATUS:
			os_printk(K_DEBUG, "USB_REQ_GET_STATUS\n");
			handled = service_tx_status_request(musb,
					ctrlrequest);
			break;

		/*                          */

		default:
			break;
		}
	}
	return handled;
}

/*
                                         
 */
static void musb_g_ep0_giveback(struct musb *musb, struct usb_request *req)
{
	os_printk(K_DEBUG, "%s\n", __func__);

	musb_g_giveback(&musb->endpoints[0].ep_in, req, 0);
}

/*
                                                               
 */
static inline void musb_try_b_hnp_enable(struct musb *musb)
{
	u32		devctl;

	dev_dbg(musb->controller, "HNP: Setting HR\n");
	devctl = os_readl(U3D_DEVICE_CONTROL);
	//                                                                                                                                              
}

/*
                                                                     
                    
                                                                 
                                        
                                                                  
                                                            
  
                                         
 */
static int
service_zero_data_request(struct musb *musb,
		struct usb_ctrlrequest *ctrlrequest)
__releases(musb->lock)
__acquires(musb->lock)
{
	int handled = -EINVAL;
	const u8 recip = ctrlrequest->bRequestType & USB_RECIP_MASK;
#ifdef CONFIG_USBIF_COMPLIANCE
	unsigned int tmp;
#endif

	os_printk(K_DEBUG, "%s\n", __func__);

	/*                                                                 */
	if ((ctrlrequest->bRequestType & USB_TYPE_MASK)
			== USB_TYPE_STANDARD) {
		switch (ctrlrequest->bRequest) {
		case USB_REQ_SET_ISOCH_DELAY:
			handled = 1;
			break;
		case USB_REQ_SET_ADDRESS:
			/*                                  */
			musb->set_address = true;
			musb->address = (u8) (ctrlrequest->wValue & 0x7f);
			handled = 1;
			break;

		case USB_REQ_CLEAR_FEATURE:
			switch (recip) {
			case USB_RECIP_DEVICE:
				switch (ctrlrequest->wValue)
				{
					case USB_DEVICE_REMOTE_WAKEUP:
						musb->may_wakeup = 0;
						handled = 1;
						break;
					case USB_DEVICE_U1_ENABLE:
					case USB_DEVICE_U2_ENABLE:
						//               
						if (musb->g.speed == USB_SPEED_SUPER) {
							//                                                 
							handled = forward_to_driver(musb, ctrlrequest);

#ifdef CONFIG_USBIF_COMPLIANCE
							if (handled >= 0) {
								tmp = (ctrlrequest->wValue == USB_DEVICE_U1_ENABLE) ?
								SW_U1_ACCEPT_ENABLE : SW_U2_ACCEPT_ENABLE;

								os_printk(K_INFO, "%s CLEAR_FEATURE reg=%x, tmp=%x\n",
									__func__, os_readl(U3D_LINK_POWER_CONTROL), tmp);

								os_clrmsk(U3D_LINK_POWER_CONTROL, tmp);

								os_printk(K_INFO, "%s CLEAR_FEATURE reg=%x\n",
									__func__, os_readl(U3D_LINK_POWER_CONTROL));

								if (ctrlrequest->wValue == USB_DEVICE_U1_ENABLE)
									musb->g.pwr_params.bU1Enabled = 0;
								else
									musb->g.pwr_params.bU2Enabled = 0;
							} else {
								os_printk(K_ERR, "[COM] composite driver can not handle this control request!\n");
							}
#else
							/*
                                                     
                                                                
        */
							handled = -EINVAL;

#endif /*       */
						}
						break;
					default:
						handled = -EINVAL;
						break;
				}
				break;
			case USB_RECIP_INTERFACE:
				//               
				if ((ctrlrequest->wValue == USB_INTRF_FUNC_SUSPEND)
					&& (musb->g.speed == USB_SPEED_SUPER))
					//                                                 
					handled = forward_to_driver(musb, ctrlrequest);
				break;
			case USB_RECIP_ENDPOINT:{
				const u8		epnum =
					ctrlrequest->wIndex & 0x0f;
				struct musb_ep		*musb_ep;
				struct musb_hw_ep	*ep;
				struct musb_request	*request;
				int			is_in;
				u32			csr;

				if (epnum == 0 || epnum >= MUSB_C_NUM_EPS ||
				    ctrlrequest->wValue != USB_ENDPOINT_HALT)
					break;

				ep = musb->endpoints + epnum;
				is_in = ctrlrequest->wIndex & USB_DIR_IN;
				if (is_in)
					musb_ep = &ep->ep_in;
				else
					musb_ep = &ep->ep_out;
				if (!musb_ep->desc)
					break;

				handled = 1;
				/*                                      */
				if (musb_ep->wedged)
					break;

				if (is_in) {//  
					csr = USB_ReadCsr32(U3D_TX1CSR0, epnum) & TX_W1C_BITS;
					csr = (csr & (~TX_SENDSTALL)) | TX_SENTSTALL;
					//                            

					USB_WriteCsr32(U3D_TX1CSR0, epnum, csr);

					os_printk(K_DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&& clear tx stall --> write csr[%d] 0x%04x. new CSR is: 0x%04x\n", epnum, csr, USB_ReadCsr32(U3D_TX1CSR0, epnum));

					os_writel(U3D_EP_RST, os_readl(U3D_EP_RST) | (BIT16<<epnum));//           
					os_writel(U3D_EP_RST, os_readl(U3D_EP_RST) & ~(BIT16<<epnum));//                 
					os_printk(K_DEBUG, "RST TX%d\n", epnum);

					/*                                                                                                      */

					//                                      
					//                                    

				} else {
					csr = USB_ReadCsr32(U3D_RX1CSR0, epnum) & RX_W1C_BITS;
					csr = (csr & (~RX_SENDSTALL)) | RX_SENTSTALL;

					USB_WriteCsr32(U3D_RX1CSR0, epnum, csr);

					os_printk(K_DEBUG, "&&&&&&&&&&&&&&&&&&&&&&&&&& clear rx stall --> write csr[%d] 0x%04x. new CSR is: 0x%04x\n", epnum, csr, USB_ReadCsr32(U3D_RX1CSR0, epnum) );

					os_writel(U3D_EP_RST, os_readl(U3D_EP_RST) | (1 << epnum));//           
					os_writel(U3D_EP_RST, os_readl(U3D_EP_RST) & (~(1 << epnum)));//                 
					os_printk(K_DEBUG, "RST RX%d\n", epnum);
					/*                                                                                                      */

					//                                      
					//                                    
				}

				/*                                            */
				request = next_request(musb_ep);
				if (!musb_ep->busy && request) {
					dev_dbg(musb->controller, "restarting the request\n");
					musb_ep_restart(musb, request);
				}

				/*                  */
				} break;
			default:
				/*                                 */
				handled = 0;
				break;
			}
			break;

		case USB_REQ_SET_FEATURE:
			switch (recip) {
			case USB_RECIP_DEVICE:
				handled = 1;
				switch (ctrlrequest->wValue) {
				case USB_DEVICE_REMOTE_WAKEUP:
					musb->may_wakeup = 1;
					break;
				case USB_DEVICE_TEST_MODE:
					if (musb->g.speed != USB_SPEED_HIGH)
						goto stall;
					if (ctrlrequest->wIndex & 0xff)
						goto stall;

					switch (ctrlrequest->wIndex >> 8) {
					case 1:
						pr_debug("TEST_J\n");
						/*        */
						musb->test_mode_nr =
							TEST_J_MODE;
						break;
					case 2:
						/*        */
						pr_debug("TEST_K\n");
						musb->test_mode_nr =
							TEST_K_MODE;
						break;
					case 3:
						/*              */
						pr_debug("TEST_SE0_NAK\n");
						musb->test_mode_nr =
							TEST_SE0_NAK_MODE;
						break;
					case 4:
						/*             */
						pr_debug("TEST_PACKET\n");
						musb->test_mode_nr =
							TEST_PACKET_MODE;
						break;

					case 0xc0:
						/*               */
						pr_debug("TEST_FORCE_HS\n");
						musb->test_mode_nr =
							FORCE_HS;
						break;
					case 0xc1:
						/*               */
						pr_debug("TEST_FORCE_FS\n");
						musb->test_mode_nr =
							FORCE_FS;
						break;
					case 0xc2:
						/*                  */
						pr_debug("TEST_FIFO_ACCESS\n");
						musb->test_mode_nr =
							FIFO_ACCESS;
						break;
					case 0xc3:
						/*                 */
						pr_debug("TEST_FORCE_HOST\n");
						musb->test_mode_nr =
							FORCE_HOST;
						break;
					default:
						goto stall;
					}

					/*                           */
					if (handled > 0)
						musb->test_mode = true;
					break;
				case USB_DEVICE_B_HNP_ENABLE:
					if (!musb->g.is_otg)
						goto stall;
					musb->g.b_hnp_enable = 1;
					musb_try_b_hnp_enable(musb);
					break;
				case USB_DEVICE_A_HNP_SUPPORT:
					if (!musb->g.is_otg)
						goto stall;
					musb->g.a_hnp_support = 1;
					break;
				case USB_DEVICE_A_ALT_HNP_SUPPORT:
					if (!musb->g.is_otg)
						goto stall;
					musb->g.a_alt_hnp_support = 1;
					break;
				case USB_DEVICE_U1_ENABLE:
				case USB_DEVICE_U2_ENABLE:
					//               
					if (musb->g.speed == USB_SPEED_SUPER)
					{
						//                                                 
						handled = forward_to_driver(musb, ctrlrequest);

#ifdef CONFIG_USBIF_COMPLIANCE
						if (handled >= 0) {

							tmp = (ctrlrequest->wValue == USB_DEVICE_U1_ENABLE) ?
								(SW_U1_ACCEPT_ENABLE) :
								(SW_U2_ACCEPT_ENABLE);

							os_printk(K_INFO, "%s SET_FEATURE %s handled=%d val=%x\n",
								__func__,
								(ctrlrequest->wValue == USB_DEVICE_U1_ENABLE)?"U1":"U2",
								handled, tmp);

							os_setmsk(U3D_LINK_POWER_CONTROL, tmp);

							os_printk(K_INFO, "%s SET_FEATURE reg=%x\n",
  								__func__, os_readl(U3D_LINK_POWER_CONTROL));

							if (ctrlrequest->wValue == USB_DEVICE_U1_ENABLE)
								musb->g.pwr_params.bU1Enabled = 1;
							else
								musb->g.pwr_params.bU2Enabled = 1;
						} else {
							os_printk(K_ERR, "[COM] composite driver can not handle this control request!\n");
						}
#else
						/*
                                                    
                                                               
       */
						handled = -EINVAL;

#endif /*       */
					}
					break;
				case USB_DEVICE_DEBUG_MODE:
					handled = 0;
					break;
stall:
				default:
					handled = -EINVAL;
					break;
				}
				break;

			case USB_RECIP_INTERFACE:
				//               
				if ((ctrlrequest->wValue == USB_INTRF_FUNC_SUSPEND)
					&& (musb->g.speed == USB_SPEED_SUPER)) {
					//                                                 
					//                                               

					os_printk(K_DEBUG, "wIndex=%x\n", ctrlrequest->wIndex);

					if(ctrlrequest->wIndex & USB_INTRF_FUNC_SUSPEND_LP)
						os_printk(K_DEBUG, "USB_INTRF_FUNC_SUSPEND_LP\n");
					else if(ctrlrequest->wIndex & USB_INTRF_FUNC_SUSPEND_RW)
						os_printk(K_DEBUG, "USB_INTRF_FUNC_SUSPEND_RW\n");
				}
				os_printk(K_DEBUG, "USB_RECIP_INTERFACE handled=%d\n", handled);

				/*                                                                          */
				handled = 1;

				break;

			case USB_RECIP_ENDPOINT:{
				const u8		epnum =
					ctrlrequest->wIndex & 0x0f;
				struct musb_ep		*musb_ep;
				struct musb_hw_ep	*ep;
				int			is_in;
				u32			csr;

				if (epnum == 0 || epnum >= MUSB_C_NUM_EPS ||
				    ctrlrequest->wValue	!= USB_ENDPOINT_HALT)
					break;

				ep = musb->endpoints + epnum;
				is_in = ctrlrequest->wIndex & USB_DIR_IN;
				if (is_in)
					musb_ep = &ep->ep_in;
				else
					musb_ep = &ep->ep_out;
				if (!musb_ep->desc)
					break;

				if (is_in) {//  
					csr = USB_ReadCsr32(U3D_TX1CSR0, epnum);

					if (!(csr & TX_FIFOEMPTY))
					{
						/*

                                                
                                 
                                                                
                                                                                                                
                                                                    
       
                   
       

      */

						os_writel(U3D_EP_RST, os_readl(U3D_EP_RST) | (BIT16<<epnum));//           
						os_writel(U3D_EP_RST, os_readl(U3D_EP_RST) & ~(BIT16<<epnum));//                 

					}


					csr &= TX_W1C_BITS;
					csr |= TX_SENDSTALL;
					os_printk(K_DEBUG, "@@@@@@@@@@@@@@@@@ EP%d(IN/TX) SEND_STALL\n", epnum);

					//                                              

					USB_WriteCsr32(U3D_TX1CSR0, epnum, csr);
				} else {
					csr = USB_ReadCsr32(U3D_RX1CSR0, epnum);
					csr &= RX_W1C_BITS;
					csr |= RX_SENDSTALL;

					os_printk(K_DEBUG, "@@@@@@@@@@@@@@@@@ EP%d(OUT/RX) SEND_STALL\n", epnum);
					//                                   
					USB_WriteCsr32(U3D_RX1CSR0, epnum, csr);
				}

				/*                  */
				handled = 1;
				} break;

			default:
				/*                                 */
				handled = 0;
				break;
			}
			break;
		default:
			/*                                 */
			handled = 0;
		}
	} else
		handled = 0;
	return handled;
}

/*                              
                                         
 */
static void ep0_rxstate(struct musb *musb)
{
	struct musb_request	*request;
	struct usb_request	*req;
	u32 csr;
	u16 count = 0;

	os_printk(K_DEBUG, "%s\n", __func__);

	/*                                     */
	csr = os_readl(U3D_EP0CSR) & EP0_W1C_BITS;

	request = next_ep0_request(musb);
	req = &request->request;

	/*                                                            
                                                               
  */
	if (req) {
		void *buf = req->buf + req->actual;
		unsigned	len = req->length - req->actual;

#ifdef AUTOCLEAR
		if(!(os_readl(U3D_EP0CSR) & EP0_AUTOCLEAR)){
			os_writel(U3D_EP0CSR, os_readl(U3D_EP0CSR)|EP0_AUTOCLEAR);
		}
#endif
		/*                 */
		count = os_readl(U3D_RXCOUNT0);
		if (count > len) {
			req->status = -EOVERFLOW;
			count = len;
		}
		musb_read_fifo(&musb->endpoints[0], count, buf);
		req->actual += count;
		csr |= EP0_RXPKTRDY;
		/*                                                                                        */
		if (count < 64 || req->actual == req->length) {
			//                                          
			musb->ep0_state = MUSB_EP0_IDLE; //                                                         
			os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_STATUSIN then MUSB_EP0_IDLE\n");

			csr |= EP0_DATAEND;
		} else
			req = NULL;
	} else {
		csr |= EP0_RXPKTRDY | EP0_SENDSTALL;
		os_printk(K_DEBUG, "@@@@@@@@@@@@@@@ SENDSTALL\n");
	}

	/*                                                         
                                             
  */
	if (req) {
		musb->ackpend = csr;
		musb_g_ep0_giveback(musb, req);
		if (!musb->ackpend)
			return;
		musb->ackpend = 0;
	}
	os_writel(U3D_EP0CSR, csr);
}

/*
                                                                    
                          
  
                                         
 */
static void ep0_txstate(struct musb *musb)
{
	struct musb_request	*req = next_ep0_request(musb);
	struct usb_request	*request;

	u32			csr = EP0_TXPKTRDY;
	u8			*fifo_src;
	u16			fifo_count;
	u32 		maxp;

	os_printk(K_DEBUG, "%s\n", __func__);

	if (!req) {
		/*             */
		//                                                                           
		return;
	}

	maxp = musb->endpoints->max_packet_sz_tx;

	request = &req->request;

	/*               */
	fifo_src = (u8 *) request->buf + request->actual;
	fifo_count = min((unsigned) maxp,
		request->length - request->actual);
	musb_write_fifo(&musb->endpoints[0], fifo_count, fifo_src);

	os_printk(K_DEBUG, "%s act=%d, len=%d, cnt=%d, maxp=%d zero=%d\n", \
		__func__, request->actual, request->length, fifo_count, maxp, request->zero);

	/*
                                                           
                
                             
         
                                                                    
                               
          
                                                          
                               
  */
	/*                  */
	if(request->actual == request->length) {
		if (request->zero && (request->length % maxp == 0) && (request->length / maxp > 0)) {
			/*
                                                                  
                                                                  
                                                                  
                                                               
    */
			request->zero = 0;
			os_printk(K_DEBUG, "%s Send a padding ZLP!!!!\n", __func__);
			request = NULL;
		} else {
			musb->ep0_state = MUSB_EP0_STAGE_STATUSOUT;
			csr |= EP0_DATAEND;
		}
	} else {
		/*                                                                */
		request->actual += fifo_count;
		request = NULL;
	}

	/*                                                            
                                                                 
                                                                  
                                                                
  */
	if (request) {
		musb->ackpend = csr;
		musb_g_ep0_giveback(musb, request);
		if (!musb->ackpend)
			return;
		musb->ackpend = 0;
	}

	os_printk(K_DEBUG, "%s csr=%x\n", __func__, csr);

	/*                                                  */
	os_writel(U3D_EP0CSR, os_readl(U3D_EP0CSR) | csr);
}

/*
                                                                  
                                     
  
                                          
 */
static void
musb_read_setup(struct musb *musb, struct usb_ctrlrequest *req)
{
	struct musb_request	*r;
	u32 csr = 0;

	os_printk(K_DEBUG, "%s\n", __func__);

	csr = os_readl(U3D_EP0CSR) & EP0_W1C_BITS; //         
	if(!(os_readl(U3D_EP0CSR) & EP0_AUTOCLEAR)){
		os_writel(U3D_EP0CSR, os_readl(U3D_EP0CSR)|EP0_AUTOCLEAR);
	}
	mu3d_hal_read_fifo(0,(u8 *)req);

	/*                                                          
                                                             
  */
	//                                                                 
	os_printk(K_DEBUG, "SETUP req%02x.%02x v%04x i%04x l%d\n",
		req->bRequestType,
		req->bRequest,
		le16_to_cpu(req->wValue),
		le16_to_cpu(req->wIndex),
		le16_to_cpu(req->wLength));

	/*                                 */
	r = next_ep0_request(musb);
	if (r)
		musb_g_ep0_giveback(musb, &r->request);

	/*                                                            
                                                                  
                                                    
   
                                                              
                                                            
                      
  */
	musb->set_address = false;
	if (req->wLength == 0) {
		musb->ep0_state = MUSB_EP0_STAGE_ACKWAIT;
		os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_ACKWAIT\n");
	} else if (req->bRequestType & USB_DIR_IN) {
		musb->ep0_state = MUSB_EP0_STAGE_TX;
		os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_TX\n");
		os_writel(U3D_EP0CSR, csr | EP0_SETUPPKTRDY | EP0_DPHTX);
		musb->ackpend = 0;
	} else {
		os_writel(U3D_EP0CSR, (csr | EP0_SETUPPKTRDY) & (~EP0_DPHTX)); //                                        
		musb->ackpend = 0;
		musb->ep0_state = MUSB_EP0_STAGE_RX;
		os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_RX\n");
	}
}


/*
                                  
  
                                                               
 */
irqreturn_t musb_g_ep0_irq(struct musb *musb)
{
	u32		csr;
	u16		len;
	irqreturn_t	retval = IRQ_NONE;

	csr = os_readl(U3D_EP0CSR);
	len = (u16)os_readl(U3D_RXCOUNT0);

	os_printk(K_DEBUG, "%s csr=0x%X\n", __func__, csr);

	//                                                                         
	//           
	//                                
	//                                    

	//                                
	//   
	//                                                      
	//                                           
	//    
	//                    
	// 

	/*                                               */
	if (csr & EP0_SENTSTALL) {
		os_writel(U3D_EP0CSR, (csr & EP0_W1C_BITS) | EP0_SENTSTALL); //                    

		if (os_readl(U3D_EP0CSR) & EP0_TXPKTRDY) //                                      
		{
			#if 0
			u32 csr0 = 0;

			csr0 = os_readl(U3D_EP0CSR);
			csr0 &= EP0_W1C_BITS; //                    
			csr0 |= CSR0_FLUSHFIFO;
			os_writel(U3D_EP0CSR, csr0);

			os_printk(K_DEBUG, "waiting for flushfifo.....");
			while(os_readl(U3D_EP0CSR) & CSR0_FLUSHFIFO) //                        
			{
				cpu_relax();
			}
			os_printk(K_DEBUG, "done.\n");
			#else
			//              
			os_setmsk(U3D_EP_RST, EP0_RST);
			os_clrmsk(U3D_EP_RST, EP0_RST);
			#endif
		}
		retval = IRQ_HANDLED;
		musb->ep0_state = MUSB_EP0_STAGE_IDLE;
		csr = os_readl(U3D_EP0CSR);
		os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_IDLE. now csr is 0x%04x\n", csr);
	}

/*                                                */
#ifdef NEVER
	/*                       */
	if (csr & CSR0_SETUPEND) {
		os_writel(U3D_EP0CSR, (os_readl(U3D_EP0CSR) & EP0_W1C_BITS) | CSR0_SERVICESETUPEND);
		retval = IRQ_HANDLED;
		/*                                        */
		switch (musb->ep0_state) {
		case MUSB_EP0_STAGE_TX:
			musb->ep0_state = MUSB_EP0_STAGE_STATUSOUT;
			os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_STATUSOUT\n");
			break;
		case MUSB_EP0_STAGE_RX:
			musb->ep0_state = MUSB_EP0_STAGE_STATUSIN;
			os_printk(K_DEBUG, 	"----- ep0 state: MUSB_EP0_STAGE_STATUSIN\n");
			break;
		default:
			ERR("SetupEnd came in a wrong ep0stage %s\n",
			decode_ep0stage(musb->ep0_state));
		}
		csr = os_readl(U3D_EP0CSR);
		/*                                    */
	}
#endif /*       */

	os_printk(K_DEBUG, "ep0_state=%d\n", musb->ep0_state);

	/*                                                              
                                                                
                                                            
  */
	switch (musb->ep0_state) {

	case MUSB_EP0_STAGE_TX:
		/*                          */
		if ((csr & EP0_FIFOFULL) == 0) {
			os_printk(K_DEBUG, "csr & EP0_FIFOFULL\n");
			ep0_txstate(musb);
			retval = IRQ_HANDLED;
		}
		break;

	case MUSB_EP0_STAGE_RX:
		/*                     */
		if (csr & EP0_RXPKTRDY) {
			ep0_rxstate(musb);
			retval = IRQ_HANDLED;
		}
		break;

	case MUSB_EP0_STAGE_STATUSIN:
/*                                                                                                                                           */
#if 0
		/*                                                   */

		/*                                                 
                                                     
                                                     
                                          
   */
		if (musb->set_address) {
			musb->set_address = false;
			os_writel(U3D_DEVICE_CONF, os_readl(U3D_DEVICE_CONF) | (musb->address << DEV_ADDR_OFST));
		}

		/*                                           */
		else if (musb->test_mode) {
			dev_dbg(musb->controller, "entering TESTMODE\n");

			printk("entering TESTMODE 1\n");

			if (TEST_PACKET_MODE == musb->test_mode_nr)
				musb_load_testpacket(musb);

			os_writel(U3D_USB2_TEST_MODE, musb->test_mode_nr);
		}
#endif
		/*             */

	case MUSB_EP0_STAGE_STATUSOUT:
		/*                                              */
		{
			struct musb_request	*req;

			req = next_ep0_request(musb);
			if (req)
				musb_g_ep0_giveback(musb, &req->request);
		}

		/*
                                                       
                                                             
   */
		if (csr & EP0_SETUPPKTRDY) //                                                
			goto setup;

		retval = IRQ_HANDLED;
		musb->ep0_state = MUSB_EP0_STAGE_IDLE;
		os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_IDLE\n");
		break;

	case MUSB_EP0_STAGE_IDLE:
		/*
                                                           
                                                              
                                                               
                                                             
   */
		retval = IRQ_HANDLED;

		/*                                                                */
		//                 
		if (!(csr & EP0_SETUPPKTRDY))
		{
		    os_printk(K_DEBUG, "break from MUSB_EP0_STAGE_IDLE\n");
			break; //                            
		}
		//                

		musb->ep0_state = MUSB_EP0_STAGE_SETUP;
                os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_SETUP\n");
		/*             */

	case MUSB_EP0_STAGE_SETUP:
setup:
		if (csr & EP0_SETUPPKTRDY) {
			struct usb_ctrlrequest	setup;
			int			handled = 0;

			if (len != 8) {
				ERR("SETUP packet len %d != 8 ?\n", len);
				break;
			}
			musb_read_setup(musb, &setup);
			retval = IRQ_HANDLED;

			/*                                       */
			if (unlikely(musb->g.speed == USB_SPEED_UNKNOWN)) {
				/*                                 */
/*                                                     
             

                                              
                    
                        
                                          
                                     
                                       

*/
			}

			switch (musb->ep0_state) {

			/*                                               
                                                   
                                                   
                                               
    */
			case MUSB_EP0_STAGE_ACKWAIT:
				os_printk(K_DEBUG, "&&&&&&&&&&&&&&&&& process MUSB_EP0_STAGE_ACKWAIT\n");
				handled = service_zero_data_request(
						musb, &setup);

				/*
                                              
                                               
                                                 
                                               
     */
				if(1) //                                                                       
				{
					//                                                 
					if (musb->set_address) {
						musb->set_address = false;
						//                                              
						os_printk(K_INFO, "Set address to 0x%08x...\n", musb->address);
						os_writel(U3D_DEVICE_CONF, os_readl(U3D_DEVICE_CONF) | (musb->address << DEV_ADDR_OFST));
					}
					else if (musb->test_mode) {
						os_printk(K_DEBUG, "entering TESTMODE 2\n");

						if (TEST_PACKET_MODE == musb->test_mode_nr)
							musb_load_testpacket(musb);

						//                                 
						//                      

						//                                                     
						os_writel(U3D_EP0CSR, (os_readl(U3D_EP0CSR) & EP0_W1C_BITS) | musb->ackpend | EP0_DATAEND | EP0_SETUPPKTRDY);
						musb->ep0_state = MUSB_EP0_STAGE_IDLE;

						while((os_readl(U3D_EP0CSR) & EP0_DATAEND) != 0)//                                             
						{
							mdelay(1); //                                 
						}

						os_writel(U3D_USB2_TEST_MODE, musb->test_mode_nr);

						return retval;
					}

					/*                                              */
					{
						struct usb_request	*request;
						struct musb_request *req;

						req = next_ep0_request(musb);
						if (req)
						{
						    request = &(req->request);
							os_printk(K_DEBUG, "&&&&&&&&&&&&&&&&& next_ep0_request\n");
							musb_g_ep0_giveback(musb, request);
						}
						else
						{
							//                                                                        
						}
					}
				}

                musb->ackpend |= EP0_DATAEND | EP0_SETUPPKTRDY; //                                        

				/*                                 */
				if (handled > 0)
				{
					musb->ep0_state = MUSB_EP0_STAGE_IDLE; //                                                                                   
					os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_IDLE\n");
				}

				break;

			/*                                              
                                                    
                             
    */
			case MUSB_EP0_STAGE_TX:
				handled = service_in_request(musb, &setup);
				if (handled > 0) {
					os_printk(K_DEBUG, "handled MUSB_EP0_STAGE_TX.\n");

					while(os_readl(U3D_EP0CSR) & EP0_FIFOFULL)//                                   
					{
						cpu_relax();
					}

					musb->ackpend |= EP0_DATAEND;
					musb->ep0_state = MUSB_EP0_STAGE_STATUSOUT;
					os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_STATUSOUT (%s:%d)\n", __func__, __LINE__ );

					if(1){ //                                                                                                  
						/*                                              */
						{
							struct usb_request	*request;
							struct musb_request *req;

							req = next_ep0_request(musb);
							if (req){
								request = &(req->request);
								musb_g_ep0_giveback(musb, request);
							}
						}

						/*
                                                           
                                                                 
       */

						retval = IRQ_HANDLED;
						musb->ep0_state = MUSB_EP0_STAGE_IDLE;
						os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_IDLE (%s:%d)\n", __func__, __LINE__ );
					}	 //                                                                                                  

				}
				else
				{
					//                                                   
				}
				break;

			/*                                             */
			default:		/*                   */
				break;
			}

			//                                                                
			os_printk(K_DEBUG, "handled %d, csr %04x, ep0stage %s\n",
				handled, csr,
				decode_ep0stage(musb->ep0_state));

			/*                                              
                                                    
                           
    */
			if (handled < 0)
				goto stall;
			else if (handled > 0)
				goto finish;

			handled = forward_to_driver(musb, &setup);
			if (handled < 0) {
stall:
				os_printk(K_INFO, "stall (%d)\n", handled);
				//                               

				if (os_readl(U3D_EP0CSR) & EP0_TXPKTRDY) //                                      
				{
					#if 0
					u32 csr0 = 0;

					csr0 = os_readl(U3D_EP0CSR);
					csr0 &= EP0_W1C_BITS; //                    
					csr0 |= CSR0_FLUSHFIFO;
					os_writel(U3D_EP0CSR, csr0);

					os_printk(K_DEBUG, "waiting for flushfifo.....");
					while(os_readl(U3D_EP0CSR) & CSR0_FLUSHFIFO) //                        
					{
						cpu_relax();
					}
					os_printk(K_DEBUG, "done.\n");
					#else
					//              
					os_setmsk(U3D_EP_RST, EP0_RST);
					os_clrmsk(U3D_EP_RST, EP0_RST);
					#endif
				}


				if(musb->ackpend & EP0_DATAEND)
				{
					os_printk(K_DEBUG, "Do not send dataend due to stall.\n");
					musb->ackpend &= ~EP0_DATAEND;
				}

				musb->ackpend |= EP0_SENDSTALL;
				musb->ep0_state = MUSB_EP0_STAGE_IDLE;
				os_printk(K_INFO, "@@@@@@@@@@@@@@@ SENDSTALL\n");
				os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_IDLE\n");
finish:
                os_writel(U3D_EP0CSR, (os_readl(U3D_EP0CSR) & EP0_W1C_BITS) | musb->ackpend);
                musb->ackpend = 0;
			}
		}
		break;

	case MUSB_EP0_STAGE_ACKWAIT:
		/*                                                       
                                               
   */
		retval = IRQ_HANDLED;
		break;

	default:
		/*                */
		WARN_ON(1);
		os_printk(K_INFO, "@@@@@@@@@@@@@@@ SENDSTALL\n");

		//                               

		if (os_readl(U3D_EP0CSR) & EP0_TXPKTRDY) //                                      
		{
			#if 0
			u32 csr0 = 0;

			csr0 = os_readl(U3D_EP0CSR);
			csr0 &= EP0_W1C_BITS; //                    
			csr0 |= CSR0_FLUSHFIFO;
			os_writel(U3D_EP0CSR, csr0);

			os_printk(K_DEBUG, "waiting for flushfifo.....");
			while(os_readl(U3D_EP0CSR) & CSR0_FLUSHFIFO) //                        
			{
				cpu_relax();
			}
			os_printk(K_DEBUG, "done.\n");
			#else
			//              
			os_setmsk(U3D_EP_RST, EP0_RST);
			os_clrmsk(U3D_EP_RST, EP0_RST);
			#endif
		}


		os_writel(U3D_EP0CSR, (os_readl(U3D_EP0CSR) & EP0_W1C_BITS) | EP0_SENDSTALL);


		musb->ep0_state = MUSB_EP0_STAGE_IDLE;
		break;
	}

	return retval;
}


static int
musb_g_ep0_enable(struct usb_ep *ep, const struct usb_endpoint_descriptor *desc)
{
	/*                */
	return -EINVAL;
}

static int musb_g_ep0_disable(struct usb_ep *e)
{
	/*                */
	return -EINVAL;
}

static int
musb_g_ep0_queue(struct usb_ep *e, struct usb_request *r, gfp_t gfp_flags)
{
	struct musb_ep		*ep;
	struct musb_request	*req;
	struct musb		*musb;
	int			status;
	unsigned long		lockflags;

	if (!e || !r)
		return -EINVAL;

	os_printk(K_DEBUG, "%s\n", __func__);

	ep = to_musb_ep(e);
	musb = ep->musb;

	req = to_musb_request(r);
	req->musb = musb;
	req->request.actual = 0;
	req->request.status = -EINPROGRESS;
	req->tx = ep->is_in;

	spin_lock_irqsave(&musb->lock, lockflags);

	if (!list_empty(&ep->req_list)) {
		status = -EBUSY;
		goto cleanup;
	}

	switch (musb->ep0_state) {
	case MUSB_EP0_STAGE_RX:		/*                  */
	case MUSB_EP0_STAGE_TX:		/*                 */
	case MUSB_EP0_STAGE_ACKWAIT:	/*                  */
		status = 0;
		break;
	default:
		dev_dbg(musb->controller, "ep0 request queued in state %d\n",
				musb->ep0_state);
		status = -EINVAL;
		goto cleanup;
	}

	/*                         */
	list_add_tail(&req->list, &ep->req_list);

	//                                                          
	os_printk(K_DEBUG, "queue to %s (%s), length=%d\n",
			ep->name, ep->is_in ? "IN/TX" : "OUT/RX",
			req->request.length);

	/*                                            */
	if (musb->ep0_state == MUSB_EP0_STAGE_TX)
		ep0_txstate(musb);

	/*                                          */
	else if (musb->ep0_state == MUSB_EP0_STAGE_ACKWAIT) {
		if (req->request.length)
			status = -EINVAL;
		else {
			musb->ep0_state = MUSB_EP0_STAGE_STATUSIN;
            os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_STATUSIN\n");

			os_writel(U3D_EP0CSR, (os_readl(U3D_EP0CSR) & EP0_W1C_BITS) | musb->ackpend | EP0_DATAEND);

            musb->ackpend = 0;
			musb_g_ep0_giveback(ep->musb, r);
		}

	/*                                                     
                                                       
                                     
  */
	} else if (musb->ackpend) {
		os_writel(U3D_EP0CSR, (os_readl(U3D_EP0CSR) & EP0_W1C_BITS) | musb->ackpend);
		musb->ackpend = 0;
	}

cleanup:
	spin_unlock_irqrestore(&musb->lock, lockflags);
	return status;
}

static int musb_g_ep0_dequeue(struct usb_ep *ep, struct usb_request *req)
{
	/*                            */
	return -EINVAL;
}

static int musb_g_ep0_halt(struct usb_ep *e, int value)
{
	struct musb_ep		*ep;
	struct musb		*musb;
	void __iomem		*base;
	unsigned long		flags;
	int			status;
	u32			csr;

	os_printk(K_INFO, "%s\n", __func__);

	if (!e || !value)
		return -EINVAL;

	ep = to_musb_ep(e);
	musb = ep->musb;
	base = musb->mregs;
	status = 0;

	spin_lock_irqsave(&musb->lock, flags);

	if (!list_empty(&ep->req_list)) {
		status = -EBUSY;
		goto cleanup;
	}

	csr = musb->ackpend;

	switch (musb->ep0_state) {

	/*                                                             
                                                       
  */
	case MUSB_EP0_STAGE_TX:		/*                 */
	case MUSB_EP0_STAGE_ACKWAIT:	/*                            */
	case MUSB_EP0_STAGE_RX:		/*                  */
		csr = os_readl(U3D_EP0CSR);
		/*             */

	/*                                                               
                                                      
  */
	case MUSB_EP0_STAGE_STATUSIN:	/*                    */
	case MUSB_EP0_STAGE_STATUSOUT:	/*                   */

		if (os_readl(U3D_EP0CSR) & EP0_TXPKTRDY) //                                      
		{
			#if 0
			u32 csr0 = 0;

			csr0 = os_readl(U3D_EP0CSR);
			csr0 &= EP0_W1C_BITS; //                    
			csr0 |= CSR0_FLUSHFIFO;
			os_writel(U3D_EP0CSR, csr0);

			os_printk(K_DEBUG, "waiting for flushfifo.....");
			while(os_readl(U3D_EP0CSR) & CSR0_FLUSHFIFO) //                        
			{
				cpu_relax();
			}
			os_printk(K_DEBUG, "done.\n");
			#else
			//              
			os_setmsk(U3D_EP_RST, EP0_RST);
			os_clrmsk(U3D_EP_RST, EP0_RST);
			#endif
		}

		csr = (csr & EP0_W1C_BITS) | EP0_SENDSTALL;
		os_printk(K_INFO, "@@@@@@@@@@@@@@@ SENDSTALL\n");
		os_writel(U3D_EP0CSR, csr);
		musb->ep0_state = MUSB_EP0_STAGE_IDLE;
		os_printk(K_DEBUG, "----- ep0 state: MUSB_EP0_STAGE_IDLE\n");
		musb->ackpend = 0;
		break;
	default:
		dev_dbg(musb->controller, "ep0 can't halt in state %d\n", musb->ep0_state);
		status = -EINVAL;
	}

cleanup:
	spin_unlock_irqrestore(&musb->lock, flags);
	return status;
}

const struct usb_ep_ops musb_g_ep0_ops = {
	.enable		= musb_g_ep0_enable,
	.disable	= musb_g_ep0_disable,
	.alloc_request	= musb_alloc_request,
	.free_request	= musb_free_request,
	.queue		= musb_g_ep0_queue,
	.dequeue	= musb_g_ep0_dequeue,
	.set_halt	= musb_g_ep0_halt,
};
