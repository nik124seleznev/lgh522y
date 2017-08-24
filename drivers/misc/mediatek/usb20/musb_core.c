/*
 * MUSB OTG driver core code
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
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

/*
                                                               
  
                                                                   
                                                                  
                                                                     
                                         
  
                                                                   
                                                                  
                                                                    
                                        
  
  
                                                                    
                                                                      
                                                                       
                           
  
                                                                       
                                                         
  
                                                                      
                                                                      
                                                                    
                                                    
  
                                                                  
                                                                  
                                                                  
                                                                   
                         
  
                                                                       
  
                                                                      
                                                                    
                                                                        
                                                                         
  
                                                                    
                                                                   
                                                                   
                                                                       
                                                                
 */

/*
                                                     
                                             
                                                           
                                                           
                                            
  
                                                             
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/kobject.h>
#include <linux/prefetch.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/idr.h>
#include <linux/dma-mapping.h>

#include <linux/musb/musb_core.h>
#include <linux/musb/musbhsdma.h>
#ifdef CONFIG_OF
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_i2c.h>
#include <linux/musb/mtk_musb.h>
#endif

#include <mach/board_lge.h>
#include <mach/mt_boot.h>

DEFINE_SPINLOCK(usb_io_lock);
unsigned musb_debug = 0;
unsigned musb_uart_debug = 0;
struct musb *mtk_musb = NULL;
unsigned musb_speed = 1;
u32 usb_irq_number = 0; //                   
bool mtk_usb_power = false;

struct timeval writeTime;
struct timeval interruptTime;
//                         
extern struct musb_hdrc_platform_data usb_data;

static const struct of_device_id apusb_of_ids[] = {
       { .compatible = "mediatek,USB0", },
       {},
};
//                       

module_param_named(speed, musb_speed, uint, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "USB speed configuration. default = 1, high speed");
module_param_named(debug, musb_debug, uint, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug, "Debug message level. Default = 0");
module_param_named(dbg_uart, musb_uart_debug, uint, S_IRUGO | S_IWUSR);

#define TA_WAIT_BCON(m) max_t(int, (m)->a_wait_bcon, OTG_TIME_A_WAIT_BCON)

#define DRIVER_AUTHOR "Mentor Graphics, Texas Instruments, Nokia"
#define DRIVER_DESC "Inventra Dual-Role USB Controller Driver"

#define MUSB_VERSION "6.0"

#define DRIVER_INFO DRIVER_DESC ", v" MUSB_VERSION

#define MUSB_DRIVER_NAME "musb-hdrc"
const char musb_driver_name[] = MUSB_DRIVER_NAME;

static DEFINE_IDA(musb_ida);

MODULE_DESCRIPTION(DRIVER_INFO);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" MUSB_DRIVER_NAME);

void dumpTime(writeFunc_enum func, int epnum)
{
#if 0
	struct timeval tv;
	int diffWrite = 0;
	int diffInterrupt = 0;

	if((func == funcWriteb)||(func == funcWritew))
	{
		do_gettimeofday(&tv);
		diffWrite = tv.tv_sec - writeTime.tv_sec;
		if(diffWrite > 10)
		{
			DBG(0,"Write Operation (%d) seconds\n", diffWrite);
			writeTime = tv;
		}
	}

	if(func == funcInterrupt)
	{
		do_gettimeofday(&tv);
		diffInterrupt = tv.tv_sec - interruptTime.tv_sec;
		if(diffInterrupt > 10)
		{
			DBG(0,"Interrupt Operation (%d) seconds\n", diffInterrupt);
			interruptTime = tv;
		}
	}
#endif
}

/*                                                                         */

static inline struct musb *dev_to_musb(struct device *dev)
{
	return dev_get_drvdata(dev);
}

/*                                                                         */

int musb_get_id(struct device *dev, gfp_t gfp_mask)
{
	int ret;
	int id;

	ret = ida_pre_get(&musb_ida, gfp_mask);
	if (!ret) {
		dev_err(dev, "failed to reserve resource for id\n");
		return -ENOMEM;
	}

	ret = ida_get_new(&musb_ida, &id);
	if (ret < 0) {
		dev_err(dev, "failed to allocate a new id\n");
		return ret;
	}

	return id;
}
EXPORT_SYMBOL_GPL(musb_get_id);

void musb_put_id(struct device *dev, int id)
{

	dev_dbg(dev, "removing id %d\n", id);
	ida_remove(&musb_ida, id);
}
EXPORT_SYMBOL_GPL(musb_put_id);

#if 0 //                        
static int musb_ulpi_read(struct usb_phy *phy, u32 offset)
{
	void __iomem *addr = phy->io_priv;
	int	i = 0;
	u8	r;
	u8	power;
	int	ret;

	pm_runtime_get_sync(phy->io_dev);

	/*                                                    */
	power = musb_readb(addr, MUSB_POWER);
	power &= ~MUSB_POWER_SUSPENDM;
	musb_writeb(addr, MUSB_POWER, power);

	/*                                                     
                                                               
  */

	musb_writeb(addr, MUSB_ULPI_REG_ADDR, (u8)offset);
	musb_writeb(addr, MUSB_ULPI_REG_CONTROL,
			MUSB_ULPI_REG_REQ | MUSB_ULPI_RDN_WR);

	while (!(musb_readb(addr, MUSB_ULPI_REG_CONTROL)
				& MUSB_ULPI_REG_CMPLT)) {
		i++;
		if (i == 10000) {
			ret = -ETIMEDOUT;
			goto out;
		}

	}
	r = musb_readb(addr, MUSB_ULPI_REG_CONTROL);
	r &= ~MUSB_ULPI_REG_CMPLT;
	musb_writeb(addr, MUSB_ULPI_REG_CONTROL, r);

	ret = musb_readb(addr, MUSB_ULPI_REG_DATA);

out:
	pm_runtime_put(phy->io_dev);

	return ret;
}

static int musb_ulpi_write(struct usb_phy *phy, u32 offset, u32 data)
{
	void __iomem *addr = phy->io_priv;
	int	i = 0;
	u8	r = 0;
	u8	power;
	int	ret = 0;

	pm_runtime_get_sync(phy->io_dev);

	/*                                                    */
	power = musb_readb(addr, MUSB_POWER);
	power &= ~MUSB_POWER_SUSPENDM;
	musb_writeb(addr, MUSB_POWER, power);

	musb_writeb(addr, MUSB_ULPI_REG_ADDR, (u8)offset);
	musb_writeb(addr, MUSB_ULPI_REG_DATA, (u8)data);
	musb_writeb(addr, MUSB_ULPI_REG_CONTROL, MUSB_ULPI_REG_REQ);

	while (!(musb_readb(addr, MUSB_ULPI_REG_CONTROL)
				& MUSB_ULPI_REG_CMPLT)) {
		i++;
		if (i == 10000) {
			ret = -ETIMEDOUT;
			goto out;
		}
	}

	r = musb_readb(addr, MUSB_ULPI_REG_CONTROL);
	r &= ~MUSB_ULPI_REG_CMPLT;
	musb_writeb(addr, MUSB_ULPI_REG_CONTROL, r);

out:
	pm_runtime_put(phy->io_dev);

	return ret;
}
#else
#define musb_ulpi_read		NULL
#define musb_ulpi_write		NULL
#endif

static struct usb_phy_io_ops musb_ulpi_access = {
	.read = musb_ulpi_read,
	.write = musb_ulpi_write,
};

/*                                                                         */



/*
                          
 */
void musb_write_fifo(struct musb_hw_ep *hw_ep, u16 len, const u8 *src)
{
    void __iomem *fifo;

    if(mtk_musb->is_host)
	    fifo = hw_ep->fifo;
    else
	    fifo = mtk_musb->mregs + MUSB_FIFO_OFFSET(hw_ep->ep_in.current_epnum);

    if (unlikely(len == 0))
	return;

	prefetch((u8 *)src);

	DBG(4, "%cX ep%d fifo %p count %d buf %p\n",
			'T', hw_ep->epnum, fifo, len, src);

	/*                                      */
	if (likely((0x01 & (unsigned long) src) == 0)) {
		u16	index = 0;

		/*                                           */
		if ((0x02 & (unsigned long) src) == 0) {
			if (len >= 4) {
				iowrite32_rep(fifo, src + index, len >> 2);
				index += len & ~0x03;
			}
			if (len & 0x02) {
				musb_writew(fifo, 0, *(u16 *)&src[index]);
				index += 2;
			}
		} else {
			if (len >= 2) {
				iowrite16_rep(fifo, src + index, len >> 1);
				index += len & ~0x01;
			}
		}
		if (len & 0x01)
			musb_writeb(fifo, 0, src[index]);
	} else  {
		/*              */
		iowrite8_rep(fifo, src, len);
	}
}

/*
                            
 */
void musb_read_fifo(struct musb_hw_ep *hw_ep, u16 len, u8 *dst)
{
	void __iomem *fifo;

        if(mtk_musb->is_host)
            fifo = hw_ep->fifo;
        else
	    fifo = mtk_musb->mregs + MUSB_FIFO_OFFSET(hw_ep->ep_out.current_epnum);


	if (unlikely(len == 0))
		return;

	DBG(4, "%cX ep%d fifo %p count %d buf %p\n",
			'R', hw_ep->epnum, fifo, len, dst);

	/*                                       */
	if (likely((0x01 & (unsigned long) dst) == 0)) {
		u16	index = 0;

		/*                                                */
		if ((0x02 & (unsigned long) dst) == 0) {
			if (len >= 4) {
				ioread32_rep(fifo, dst, len >> 2);
				index = len & ~0x03;
			}
			if (len & 0x02) {
				*(u16 *)&dst[index] = musb_readw(fifo, 0);
				index += 2;
			}
		} else {
			if (len >= 2) {
				ioread16_rep(fifo, dst, len >> 1);
				index = len & ~0x01;
			}
		}
		if (len & 0x01)
			dst[index] = musb_readb(fifo, 0);
	} else  {
		/*              */
		ioread8_rep(fifo, dst, len);
	}
}



/*                                                                         */

/*                                                   */
static const u8 musb_test_packet[53] = {
	/*                                   */

	/*             */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*             */
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	/*             */
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
	/*                   */
	0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	/*             */
	0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd,
	/*                  */
	0xfc, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e

	/*                                */
};

void musb_load_testpacket(struct musb *musb)
{
	void __iomem	*regs = musb->endpoints[0].regs;

	musb_ep_select(musb->mregs, 0);
	musb_write_fifo(musb->control_ep,
			sizeof(musb_test_packet), musb_test_packet);
	musb_writew(regs, MUSB_CSR0, MUSB_CSR0_TXPKTRDY);
}

/*                                                                         */

/*
                                                
 */
static void musb_otg_timer_func(unsigned long data)
{
	struct musb	*musb = (struct musb *)data;
	unsigned long	flags;

	spin_lock_irqsave(&musb->lock, flags);
	switch (musb->xceiv->state) {
	case OTG_STATE_B_WAIT_ACON:
		DBG(2, "HNP: b_wait_acon timeout; back to b_peripheral\n");
		musb_g_disconnect(musb);
		musb->xceiv->state = OTG_STATE_B_PERIPHERAL;
		musb->is_active = 0;
		break;
	case OTG_STATE_A_SUSPEND:
	case OTG_STATE_A_WAIT_BCON:
		DBG(2, "HNP: %s timeout\n",
			otg_state_string(musb->xceiv->state));
		musb_platform_set_vbus(musb, 0);
		musb->xceiv->state = OTG_STATE_A_WAIT_VFALL;
		break;
	default:
		DBG(2, "HNP: Unhandled mode %s\n",
			otg_state_string(musb->xceiv->state));
	}
	musb->ignore_disconnect = 0;
	spin_unlock_irqrestore(&musb->lock, flags);
}

/*         
                                              
 
              
                  
                                
                     
                                         

                                               
             
                                  
                           
                                     
        
                                                

                       
                     
                                  
                                          

                              
                                       
                            
     

                           
                        
              
                                        
    
   
  
 
*/

/*
                                                              
 */
void musb_hnp_stop(struct musb *musb)
{
	struct usb_hcd	*hcd = musb_to_hcd(musb);
	void __iomem	*mbase = musb->mregs;
	u8	reg;

	DBG(2, "HNP: stop from %s\n", otg_state_string(musb->xceiv->state));

	switch (musb->xceiv->state) {
	case OTG_STATE_A_PERIPHERAL:
		musb_g_disconnect(musb);
		DBG(2, "HNP: back to %s\n",
			otg_state_string(musb->xceiv->state));
		break;
	case OTG_STATE_B_HOST:
		DBG(2, "HNP: Disabling HR\n");
		hcd->self.is_b_host = 0;
		musb->xceiv->state = OTG_STATE_B_PERIPHERAL;
		MUSB_DEV_MODE(musb);
		reg = musb_readb(mbase, MUSB_POWER);
		reg |= MUSB_POWER_SUSPENDM;
		musb_writeb(mbase, MUSB_POWER, reg);
		/*                                      */
		break;
	default:
		DBG(2, "HNP: Stopping in unknown state %s\n",
			otg_state_string(musb->xceiv->state));
	}

	/*
                                                                   
                                                                      
           
  */
	musb->port1_status &= ~(USB_PORT_STAT_C_CONNECTION << 16);
}

/*
                                                               
                                                        
                                                                
                                                    
  
                               
                                   
                
               
 */

static irqreturn_t musb_stage0_irq(struct musb *musb, u8 int_usb,
				u8 devctl)
{
	struct usb_otg *otg = musb->xceiv->otg;
	irqreturn_t handled = IRQ_NONE;

	DBG(2, "<== DevCtl=%02x, int_usb=0x%x\n", devctl,
		int_usb);

	USB_LOGGER(MUSB_STAGE0_IRQ, MUSB_STAGE0_IRQ, power, devctl, int_usb);

	/*                                                      
                                                     
                                                         
  */
	if (int_usb & MUSB_INTR_RESUME) {
		handled = IRQ_HANDLED;
		DBG(2, "RESUME (%s)\n", otg_state_string(musb->xceiv->state));

		if (devctl & MUSB_DEVCTL_HM) {
			void __iomem *mbase = musb->mregs;
			u8 power;

			switch (musb->xceiv->state) {
			case OTG_STATE_A_SUSPEND:
				/*                                     
                                 
     */

				power = musb_readb(musb->mregs, MUSB_POWER);
				if (power & MUSB_POWER_SUSPENDM) {
					/*          */
					musb->int_usb &= ~MUSB_INTR_SUSPEND;
					DBG(2, "Spurious SUSPENDM\n");
					break;
				}

				power &= ~MUSB_POWER_SUSPENDM;
				musb_writeb(mbase, MUSB_POWER,
						power | MUSB_POWER_RESUME);

				musb->port1_status |=
						(USB_PORT_STAT_C_SUSPEND << 16)
						| MUSB_PORT_STAT_RESUME;
				musb->rh_timer = jiffies
						+ msecs_to_jiffies(20);

				musb->xceiv->state = OTG_STATE_A_HOST;
				musb->is_active = 1;
				usb_hcd_resume_root_hub(musb_to_hcd(musb));
				break;
			case OTG_STATE_B_WAIT_ACON:
				musb->xceiv->state = OTG_STATE_B_PERIPHERAL;
				musb->is_active = 1;
				MUSB_DEV_MODE(musb);
				break;
			default:
				WARNING("bogus %s RESUME (%s)\n",
					"host",
					otg_state_string(musb->xceiv->state));
			}
		} else {
			switch (musb->xceiv->state) {
			case OTG_STATE_A_SUSPEND:
				/*                                 */
				musb->xceiv->state = OTG_STATE_A_HOST;
				usb_hcd_resume_root_hub(musb_to_hcd(musb));
				break;
			case OTG_STATE_B_WAIT_ACON:
			case OTG_STATE_B_PERIPHERAL:
				/*                                    
                                  
     */
				if ((devctl & MUSB_DEVCTL_VBUS)
						!= (3 << MUSB_DEVCTL_VBUS_SHIFT)
						) {
					musb->int_usb |= MUSB_INTR_DISCONNECT;
					musb->int_usb &= ~MUSB_INTR_SUSPEND;
					break;
				}
				musb_g_resume(musb);
				break;
			case OTG_STATE_B_IDLE:
				musb->int_usb &= ~MUSB_INTR_SUSPEND;
				break;
			default:
				WARNING("bogus %s RESUME (%s)\n",
					"peripheral",
					otg_state_string(musb->xceiv->state));
			}
		}
	}

	/*                                       */
	if (int_usb & MUSB_INTR_SESSREQ) {
		//                                   

		if ((devctl & MUSB_DEVCTL_VBUS) == MUSB_DEVCTL_VBUS
				&& (devctl & MUSB_DEVCTL_BDEVICE)) {
			DBG(2, "SessReq while on B state\n");
			return IRQ_HANDLED;
		}

		DBG(0, "SESSION_REQUEST (%s)\n",
			otg_state_string(musb->xceiv->state));

		/*                                                       
                                                   
                                                      
                               
                           
                                                       
   */

		/*                                     */
		/*                                    */
		#if 0
		musb_writeb(mbase, MUSB_DEVCTL, MUSB_DEVCTL_SESSION);
		musb->ep0_stage = MUSB_EP0_START;
		musb->xceiv->state = OTG_STATE_A_IDLE;
		MUSB_HST_MODE(musb);
		musb_platform_set_vbus(musb, 1);
		#endif

		handled = IRQ_HANDLED;
	}

	if (int_usb & MUSB_INTR_VBUSERROR) {
		int	ignore = 0;

		DBG(0, "MUSB_INTR_VBUSERROR (%s)\n",
			otg_state_string(musb->xceiv->state));

		/*                                                     
                                                          
                                                        
                                                            
                                           
    
                                                            
                                                            
                                                          
    
                                                          
                                                    
    
                                                         
                                             
   */
		DBG(0, "VBUSERROR\n");
		switch (musb->xceiv->state) {
		case OTG_STATE_A_HOST:
			/*                                             
                                                
                                                 
                                                    
                                                 
    */
		case OTG_STATE_A_WAIT_BCON:
		case OTG_STATE_A_WAIT_VRISE:
			if (musb->vbuserr_retry) {
				musb->vbuserr_retry--;
				ignore = 1;
			/*                                                                                              
                                                                                                                                   
   */
			#if 1
				musb_session_restart(musb);
			#else
				devctl |= MUSB_DEVCTL_SESSION;
				musb_writeb(mbase, MUSB_DEVCTL, devctl);
			#endif

			} else {
				musb->port1_status |=
					  USB_PORT_STAT_OVERCURRENT
					| (USB_PORT_STAT_C_OVERCURRENT << 16);
			}
			break;
		default:
			break;
		}

		DBG(2, "VBUS_ERROR in %s (%02x, %s), retry #%d, port1 %08x\n",
				otg_state_string(musb->xceiv->state),
				devctl,
				({ char *s;
				switch (devctl & MUSB_DEVCTL_VBUS) {
				case 0 << MUSB_DEVCTL_VBUS_SHIFT:
					s = "<SessEnd"; break;
				case 1 << MUSB_DEVCTL_VBUS_SHIFT:
					s = "<AValid"; break;
				case 2 << MUSB_DEVCTL_VBUS_SHIFT:
					s = "<VBusValid"; break;
				/*                                   */
				default:
					s = "VALID"; break;
				}; s; }),
				VBUSERR_RETRY_COUNT - musb->vbuserr_retry,
				musb->port1_status);

		/*                                                  */
		if (!ignore) {
			if(is_switch_charger()) {
			    DBG(0, "too many VBUS error, restart power on sequence for switching charger!\n");
			    schedule_delayed_work(&musb->id_pin_work, 400*HZ/1000);
			} else {
			    musb_platform_set_vbus(musb, 0);
			    DBG(0, "too many VBUS error, turn it off!\n");
			}
		}
		handled = IRQ_HANDLED;
	}

	if (int_usb & MUSB_INTR_SUSPEND) {
		DBG(0, "SUSPEND (%s) devctl %02x\n",
			otg_state_string(musb->xceiv->state), devctl);
		handled = IRQ_HANDLED;

		switch (musb->xceiv->state) {
		case OTG_STATE_A_PERIPHERAL:
			/*                                                 
                                                        
     
                                                      
                                                    
                                           
    */
			musb_hnp_stop(musb);
			usb_hcd_resume_root_hub(musb_to_hcd(musb));
			musb_root_disconnect(musb);
			musb_platform_try_idle(musb, jiffies
					+ msecs_to_jiffies(musb->a_wait_bcon
						? : OTG_TIME_A_WAIT_BCON));

			break;
		case OTG_STATE_B_IDLE:
			if (!musb->is_active)
				break;
		case OTG_STATE_B_PERIPHERAL:
			musb_g_suspend(musb);
			musb->is_active = otg->gadget->b_hnp_enable;
			if (musb->is_active) {
				musb->xceiv->state = OTG_STATE_B_WAIT_ACON;
				DBG(2, "HNP: Setting timer for b_ase0_brst\n");
				mod_timer(&musb->otg_timer, jiffies
					+ msecs_to_jiffies(
							OTG_TIME_B_ASE0_BRST));
			}
			break;
		case OTG_STATE_A_WAIT_BCON:
			if (musb->a_wait_bcon != 0)
				musb_platform_try_idle(musb, jiffies
					+ msecs_to_jiffies(musb->a_wait_bcon));
			break;
		case OTG_STATE_A_HOST:
			musb->xceiv->state = OTG_STATE_A_SUSPEND;
			musb->is_active = otg->host->b_hnp_enable;
			break;
		case OTG_STATE_B_HOST:
			/*                                              */
			DBG(2, "REVISIT: SUSPEND as B_HOST\n");
			break;
		default:
			/*                     */
			musb->is_active = 0;
			break;
		}
	}

	if (int_usb & MUSB_INTR_CONNECT) {
		struct usb_hcd *hcd = musb_to_hcd(musb);

		DBG(0, "MUSB_INTR_CONNECT (%s)\n",
			otg_state_string(musb->xceiv->state));

		handled = IRQ_HANDLED;
		musb->is_active = 1;

		musb->ep0_stage = MUSB_EP0_START;

		/*                                                     */
		if (is_peripheral_active(musb)) {
			/*                                    */
		}
#if 0
		musb->intrtxe = musb->epmask;
		musb_writew(musb->mregs, MUSB_INTRTXE, musb->intrtxe);
		musb->intrrxe = musb->epmask & 0xfffe;
		musb_writew(musb->mregs, MUSB_INTRRXE, musb->intrrxe);
		musb_writeb(musb->mregs, MUSB_INTRUSBE, 0xf7);
#endif
		musb->port1_status &= ~(USB_PORT_STAT_LOW_SPEED
					|USB_PORT_STAT_HIGH_SPEED
					|USB_PORT_STAT_ENABLE
					);
		musb->port1_status |= USB_PORT_STAT_CONNECTION
					|(USB_PORT_STAT_C_CONNECTION << 16);

		/*                                                      */
		if (devctl & MUSB_DEVCTL_LSDEV)
			musb->port1_status |= USB_PORT_STAT_LOW_SPEED;

		/*                                        */
		switch (musb->xceiv->state) {
		case OTG_STATE_B_PERIPHERAL:
			if (int_usb & MUSB_INTR_SUSPEND) {
				DBG(2, "HNP: SUSPEND+CONNECT, now b_host\n");
				int_usb &= ~MUSB_INTR_SUSPEND;
				goto b_host;
			} else
				DBG(2, "CONNECT as b_peripheral???\n");
			break;
		case OTG_STATE_B_WAIT_ACON:
			DBG(2, "HNP: CONNECT, now b_host\n");
b_host:
			musb->xceiv->state = OTG_STATE_B_HOST;
			hcd->self.is_b_host = 1;
			musb->ignore_disconnect = 0;
			del_timer(&musb->otg_timer);
			break;
		default:
			if ((devctl & MUSB_DEVCTL_VBUS)
					== (3 << MUSB_DEVCTL_VBUS_SHIFT)) {
				musb->xceiv->state = OTG_STATE_A_HOST;
				hcd->self.is_b_host = 0;
			}
			break;
		}

		/*                   */
		MUSB_HST_MODE(musb);
		if (hcd->status_urb)
			usb_hcd_poll_rh_status(hcd);
		else
			usb_hcd_resume_root_hub(hcd);

		DBG(0, "CONNECT (%s) devctl %02x\n",
				otg_state_string(musb->xceiv->state), devctl);
	}

	if ((int_usb & MUSB_INTR_DISCONNECT) && !musb->ignore_disconnect) {
		DBG(0, "DISCONNECT (%s) as %s, devctl %02x\n",
				otg_state_string(musb->xceiv->state),
				MUSB_MODE(musb), devctl);
		handled = IRQ_HANDLED;
		musb->is_active = 0;

		switch (musb->xceiv->state) {
		case OTG_STATE_A_HOST:
		case OTG_STATE_A_SUSPEND:
			usb_hcd_resume_root_hub(musb_to_hcd(musb));
			musb_root_disconnect(musb);
			if (musb->a_wait_bcon != 0)
				musb_platform_try_idle(musb, jiffies
					+ msecs_to_jiffies(musb->a_wait_bcon));
			break;
		case OTG_STATE_B_HOST:
			/*                                           
                                                 
                                                 
                                            
    */
			musb_root_disconnect(musb);
			musb_to_hcd(musb)->self.is_b_host = 0;
			musb->xceiv->state = OTG_STATE_B_PERIPHERAL;
			MUSB_DEV_MODE(musb);
			musb_g_disconnect(musb);
			break;
		case OTG_STATE_A_PERIPHERAL:
			musb_hnp_stop(musb);
			musb_root_disconnect(musb);
			/*             */
		case OTG_STATE_B_WAIT_ACON:
			/*             */
		case OTG_STATE_B_PERIPHERAL:
		case OTG_STATE_B_IDLE:
			musb_g_disconnect(musb);
			break;
		default:
			WARNING("unhandled DISCONNECT transition (%s)\n",
				otg_state_string(musb->xceiv->state));
			break;
		}
	}

	/*                                                             
                                                          
  */
	if (int_usb & MUSB_INTR_RESET) {
		handled = IRQ_HANDLED;

		DBG(0, "MUSB_INTR_RESET (%s)\n",
			otg_state_string(musb->xceiv->state));

		if ((devctl & MUSB_DEVCTL_HM) != 0) {
			/*
                                                  
                                                          
                                                        
                                                       
                       
    */
			DBG(0, "Babble \n");
			if (devctl & (MUSB_DEVCTL_FSDEV | MUSB_DEVCTL_LSDEV))
				DBG(2, "BABBLE devctl: %02x\n", devctl);
			else {
				ERR("Stopping host session -- babble\n");
				musb_writeb(musb->mregs, MUSB_DEVCTL, 0);
			}
		} else {
			DBG(2, "BUS RESET as %s\n",
				otg_state_string(musb->xceiv->state));
			switch (musb->xceiv->state) {
			case OTG_STATE_A_SUSPEND:
				/*                                        
                                                 
                                                
     */
				musb->ignore_disconnect = 1;
				musb_g_reset(musb);
				/*             */
			case OTG_STATE_A_WAIT_BCON:	/*                  */
				/*                                  */
				DBG(2, "HNP: in %s, %d msec timeout\n",
					otg_state_string(musb->xceiv->state),
					TA_WAIT_BCON(musb));
				mod_timer(&musb->otg_timer, jiffies
					+ msecs_to_jiffies(TA_WAIT_BCON(musb)));
				break;
			case OTG_STATE_A_PERIPHERAL:
				musb->ignore_disconnect = 0;
				del_timer(&musb->otg_timer);
				musb_g_reset(musb);
				break;
			case OTG_STATE_B_WAIT_ACON:
				DBG(2, "HNP: RESET (%s), to b_peripheral\n",
					otg_state_string(musb->xceiv->state));
				musb->xceiv->state = OTG_STATE_B_PERIPHERAL;
				musb_g_reset(musb);
				break;
			case OTG_STATE_B_IDLE:
				musb->xceiv->state = OTG_STATE_B_PERIPHERAL;
				/*             */
			case OTG_STATE_B_PERIPHERAL:
				musb_g_reset(musb);
				break;
			default:
				DBG(0, "Unhandled BUS RESET as %s\n",
					otg_state_string(musb->xceiv->state));
			}
		}
	}

#if 0
/*                                                                  
                                                                 
                                         
  
                                                                  
                                                     
  
                                                                    
                                                                   
                                
 */
	if (int_usb & MUSB_INTR_SOF) {
		void __iomem *mbase = musb->mregs;
		struct musb_hw_ep	*ep;
		u8 epnum;
		u16 frame;

		DBG(2, "START_OF_FRAME\n");
		handled = IRQ_HANDLED;

		/*                                                           */
		frame = musb_readw(mbase, MUSB_FRAME);
		ep = musb->endpoints;
		for (epnum = 1; (epnum < musb->nr_endpoints)
					&& (musb->epmask >= (1 << epnum));
				epnum++, ep++) {
			/*
                                               
                                         
    */
			if (ep->dwWaitFrame >= frame) {
				ep->dwWaitFrame = 0;
				pr_debug("SOF --> periodic TX%s on %d\n",
					ep->tx_channel ? " DMA" : "",
					epnum);
				if (!ep->tx_channel)
					musb_h_tx_start(musb, epnum);
				else
					cppi_hostdma_start(musb, epnum);
			}
		}		/*                 */
	}
#endif

	schedule_work(&musb->irq_work);

	return handled;
}

/*                                                                         */

/*
                                                           
*/
void musb_start(struct musb *musb)
{
	void __iomem	*regs = musb->mregs;
	int vbusdet_retry = 5;
	usb_cable_type bootmode_cable;
	unsigned high_speed = 0;

	u8  intrusbe;

	bootmode_cable = lge_get_board_cable();

	DBG(0, "start, is_host=%d is_active=%d\n", musb->is_host, musb->is_active);
	if(musb->is_active) {
		if(musb->is_host) {
			DBG(0, "we are host now, add more interrupt devctl=%x\n", musb_readb(mtk_musb->mregs,MUSB_DEVCTL));
			musb->intrtxe = 0xffff;
			musb_writew(regs, MUSB_INTRTXE, musb->intrtxe);
			musb->intrrxe = 0xfffe;
			musb_writew(regs, MUSB_INTRRXE, musb->intrrxe);
			musb_writeb(regs,MUSB_INTRUSBE,0xf7);
			return;
		}
	}
	musb_platform_enable(musb);
	musb_generic_disable(musb);

	intrusbe= musb_readb(regs, MUSB_INTRUSBE);
	if (musb->is_host){
		musb->intrtxe = 0xffff;
		musb_writew(regs, MUSB_INTRTXE, musb->intrtxe);
		musb->intrrxe = 0xfffe;
		musb_writew(regs, MUSB_INTRRXE, musb->intrrxe);
		intrusbe = 0xf7;

		while(!musb_platform_get_vbus_status(musb)) {
			mdelay(100);
			if(vbusdet_retry--<=1) {
				DBG(0, "VBUS detection fail!\n");
				break;
			}
		}

	} else if(!musb->is_host){
		intrusbe |= MUSB_INTR_RESET; //                                  
	}

	musb_writeb(regs,MUSB_INTRUSBE,intrusbe);

	//                                                                                               
	if (lge_get_laf_mode()== 1) //        
		high_speed = 1;
	else if (g_meta_com_type == META_USB_COM)
		high_speed = 0;
	else if (bootmode_cable == LT_CABLE_56K)  //                             
		high_speed = 0;
	else
		high_speed = 1;
	//                                   

	if (musb_speed && high_speed) {
		/*                                                 */
		musb_writeb(regs, MUSB_POWER, MUSB_POWER_SOFTCONN
				| MUSB_POWER_HSENAB
				/*                       */
				| MUSB_POWER_ENSUSPEND);
	} else {
		/*                                                 */
		musb_writeb(regs, MUSB_POWER, MUSB_POWER_SOFTCONN
				/*                       */
				| MUSB_POWER_ENSUSPEND);
	}
	musb->is_active = 1;
}

void musb_generic_disable(struct musb *musb)
{
	void __iomem	*mbase = musb->mregs;

	/*                    */
	musb_writeb(mbase, MUSB_INTRUSBE, 0);
	musb->intrtxe = 0;
	musb_writew(mbase, MUSB_INTRTXE, 0);
	musb->intrrxe = 0;
	musb_writew(mbase, MUSB_INTRRXE, 0);

	/*     */
	//                                   

	/*                           */
	musb_writew(musb->mregs, MUSB_INTRRX, 0xFFFF);
	musb_writew(musb->mregs, MUSB_INTRTX, 0xFFFF);
	musb_writeb(musb->mregs, MUSB_INTRUSB, 0xEF);
}

static void gadget_stop(struct musb *musb)
{
	u8 power;

	power = musb_readb(musb->mregs, MUSB_POWER);
	power &= ~MUSB_POWER_SOFTCONN;
	musb_writeb(musb->mregs, MUSB_POWER, power);

	/*                      */
	if (musb->g.speed != USB_SPEED_UNKNOWN) {
		if (musb->gadget_driver && musb->gadget_driver->disconnect) {
			musb->gadget_driver->disconnect(&musb->g);
		}
		musb->g.speed = USB_SPEED_UNKNOWN;
	}
}

/*
                                                 
                           
                                     
                                       
                                                        
 */
void musb_stop(struct musb *musb)
{
	/*                        */
	musb_generic_disable(musb);
	gadget_stop(musb);
	musb_platform_disable(musb);
	musb->is_active = 0;
	DBG(0, "HDRC disabled\n");

	/*      
                                                            
                                               
                                                            
                                                                
          
  */
	musb_platform_try_idle(musb, 0);
}

static void musb_shutdown(struct platform_device *pdev)
{
	struct musb	*musb = dev_to_musb(&pdev->dev);
	unsigned long	flags;
	DBG(0, "shut down\n");

	pm_runtime_get_sync(musb->controller);

	/*                            */

	spin_lock_irqsave(&musb->lock, flags);
	musb_generic_disable(musb);
	musb_platform_disable(musb);
	spin_unlock_irqrestore(&musb->lock, flags);
#ifdef CONFIG_USB_MTK_HDRC_HCD
    if (musb->is_host)
    {
        printk(KERN_ERR"%s, line %d. \n", __func__, __LINE__);
        musb_platform_set_vbus(mtk_musb, 0);
    }
#endif
	musb_writeb(musb->mregs, MUSB_DEVCTL, 0);
	musb_platform_exit(musb);

	pm_runtime_put(musb->controller);
	/*                  */
}
/*
                                                                 
                                              
  
                                                  
 */
static int fifo_setup(struct musb *musb, struct musb_hw_ep  *hw_ep,
		const struct musb_fifo_cfg *cfg, u16 offset)
{
//                                   
	int	size = 0;
	u16	maxpacket = cfg->maxpacket;
	u16	c_off = offset >> 3;
	u8	c_size;

	/*                                                */

	size = ffs(max(maxpacket, (u16) 8)) - 1;
	maxpacket = 1 << size;

	c_size = size - 3;
/*                                    
                                                     
                     
                            
                                                   
                     */

	/*                    */
//                                              

	switch (cfg->style) {
	case MUSB_FIFO_TX:
		DBG(0,"Tx ep %d fifo size is %d fifo address is %x\n",hw_ep->epnum,maxpacket,c_off);
//                                     
//                                     
		hw_ep->tx_double_buffered = !!(c_size & MUSB_FIFOSZ_DPB);
		hw_ep->max_packet_sz_tx = maxpacket;
		hw_ep->ep_in.fifo_size= maxpacket;
		hw_ep->ep_in.fifo_mode= cfg->mode;
		break;
	case MUSB_FIFO_RX:
		DBG(0,"Rx ep %d fifo size is %d fifo address is %x\n",hw_ep->epnum,maxpacket,c_off);
//                                     
//                                     
		hw_ep->rx_double_buffered = !!(c_size & MUSB_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;
		hw_ep->ep_out.fifo_size= maxpacket;
		hw_ep->ep_out.fifo_mode= cfg->mode;
		break;
	case MUSB_FIFO_RXTX:
//                                     
//                                     
		hw_ep->rx_double_buffered = !!(c_size & MUSB_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;

//                                     
//                                     
		hw_ep->tx_double_buffered = hw_ep->rx_double_buffered;
		hw_ep->max_packet_sz_tx = maxpacket;

		hw_ep->is_shared_fifo = true;
		hw_ep->ep_in.fifo_size= maxpacket;
		hw_ep->ep_out.fifo_size= maxpacket;
		hw_ep->ep_in.fifo_mode= cfg->mode;
		hw_ep->ep_out.fifo_mode= cfg->mode;
		break;
	}

	/*                                                        
                          
  */

	hw_ep->ep_mode = cfg->ep_mode; //                                    

	/*                                                        
                          
  */
	musb->epmask |= (1 << hw_ep->epnum);

	return offset + (maxpacket << ((c_size & MUSB_FIFOSZ_DPB) ? 1 : 0));
}

static struct musb_fifo_cfg ep0_cfg = {
	.style = MUSB_FIFO_RXTX, .maxpacket = 64, .ep_mode = EP_CONT,
};

static int ep_config_from_table(struct musb *musb)
{
	const struct musb_fifo_cfg	*cfg;
	unsigned		i, n;
	int			offset;
	struct musb_hw_ep	*hw_ep = musb->endpoints;

	if (musb->fifo_cfg) {
		cfg = musb->fifo_cfg;
		n = musb->fifo_cfg_size;
	}else
	 	return -EINVAL;
	offset = fifo_setup(musb, hw_ep, &ep0_cfg, 0);
	/*                    */

	/*                                                          
                                                                    
  */

	for (i = 0; i < n; i++){
		u8	epn = cfg->hw_ep_num;

		if (epn >= MUSB_C_NUM_EPS) {
			DBG(0, "%s: invalid ep %d\n",
					musb_driver_name, epn);
			return -EINVAL;
		}
		offset = fifo_setup(musb, hw_ep + epn, cfg++, offset);
		if (offset < 0) {
			DBG(0, "%s: mem overrun, ep %d\n",
					musb_driver_name, epn);
			return offset;
		}

		epn++;
		musb->nr_endpoints = max(epn, musb->nr_endpoints);
	}

	DBG(2,"%s: %d/%d max ep, %d/%d memory\n",
			musb_driver_name,
			n + 1, musb->config->num_eps * 2 - 1,
			offset, (1 << (musb->config->ram_bits + 2)));

	return 0;
}

static int fifo_setup_for_host(struct musb *musb, struct musb_hw_ep  *hw_ep,
		const struct musb_fifo_cfg *cfg, u16 offset)
{
	void __iomem	*mbase = musb->mregs;
	int	size = 0;
	u16	maxpacket = cfg->maxpacket;
	u16	c_off = offset >> 3;
	u8	c_size;

	/*                                                */
    DBG(4,"++,hw_ep->epnum=%d\n",hw_ep->epnum);
	size = ffs(max(maxpacket, (u16) 8)) - 1;
	maxpacket = 1 << size;

	c_size = size - 3;
	if (cfg->mode == MUSB_BUF_DOUBLE) {
		if ((offset + (maxpacket << 1)) >(musb->fifo_size))
				return -EMSGSIZE;
		c_size |= MUSB_FIFOSZ_DPB;
	}else if ((offset + maxpacket) >(musb->fifo_size))
				return -EMSGSIZE;

	/*                    */
	musb_writeb(mbase, MUSB_INDEX, hw_ep->epnum);

	switch (cfg->style) {
	case MUSB_FIFO_TX:
		DBG(4,"Tx ep %d fifo size is %d fifo address is %x\n",hw_ep->epnum,c_size,c_off);
		musb_write_txfifosz(mbase, c_size);
		musb_write_txfifoadd(mbase, c_off);
		hw_ep->tx_double_buffered = !!(c_size & MUSB_FIFOSZ_DPB);
		hw_ep->max_packet_sz_tx = maxpacket;
		break;
	case MUSB_FIFO_RX:
		DBG(4,"Rx ep %d fifo size is %d fifo address is %x\n",hw_ep->epnum,c_size,c_off);
		musb_write_rxfifosz(mbase, c_size);
		musb_write_rxfifoadd(mbase, c_off);
		hw_ep->rx_double_buffered = !!(c_size & MUSB_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;
		break;
	case MUSB_FIFO_RXTX:
		musb_write_txfifosz(mbase, c_size);
		musb_write_txfifoadd(mbase, c_off);
		hw_ep->rx_double_buffered = !!(c_size & MUSB_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;

		musb_write_rxfifosz(mbase, c_size);
		musb_write_rxfifoadd(mbase, c_off);
		hw_ep->tx_double_buffered = hw_ep->rx_double_buffered;
		hw_ep->max_packet_sz_tx = maxpacket;

		hw_ep->is_shared_fifo = true;
		break;
	}

	hw_ep->ep_mode = cfg->ep_mode; //                                    

	/*                                                        
                          
  */
	musb->epmask |= (1 << hw_ep->epnum);

	return offset + (maxpacket << ((c_size & MUSB_FIFOSZ_DPB) ? 1 : 0));
}

int ep_config_from_table_for_host(struct musb *musb)
{
	const struct musb_fifo_cfg	*cfg;
	unsigned		i, n;
	int			offset;
	struct musb_hw_ep	*hw_ep = musb->endpoints;
	if (musb->fifo_cfg_host) {
		cfg = musb->fifo_cfg_host;
		n = musb->fifo_cfg_host_size;
	}else{
	 	return -EINVAL;
       }
	offset = fifo_setup_for_host(musb, hw_ep, &ep0_cfg, 0);
	/*                    */

	/*                                                          
                                                                    
  */

	for (i = 0; i < n; i++){
		u8	epn = cfg->hw_ep_num;

		if (epn >= musb->config->num_eps) {
			DBG(0,"%s: invalid ep %d\n",
					musb_driver_name, epn);
			return -EINVAL;
		}
		offset = fifo_setup_for_host(musb, hw_ep + epn, cfg++, offset);
		if (offset < 0) {
			DBG(0,"%s: mem overrun, ep %d\n",
					musb_driver_name, epn);
			return offset;
		}
		epn++;
		musb->nr_endpoints = max(epn, musb->nr_endpoints);
	}

	DBG(2, "%s: %d/%d max ep, %d/%d memory\n",
			musb_driver_name,
			n + 1, musb->config->num_eps * 2 - 1,
			offset, (1 << (musb->config->ram_bits + 2)));

	return 0;
}


/*
                                                       
                             
 */
static int ep_config_from_hw(struct musb *musb)
{
	u8 epnum = 0;
	struct musb_hw_ep *hw_ep;
	void __iomem *mbase = musb->mregs;
	int ret = 0;

	DBG(2, "<== static silicon ep config\n");

	/*                                  */

	for (epnum = 1; epnum < musb->config->num_eps; epnum++) {
		musb_ep_select(mbase, epnum);
		hw_ep = musb->endpoints + epnum;

		ret = musb_read_fifosize(musb, hw_ep, epnum);
		if (ret < 0)
			break;

		/*                                             */

		/*                                 */
		if (hw_ep->max_packet_sz_tx < 512
				|| hw_ep->max_packet_sz_rx < 512)
			continue;

		/*                                                     
                                            
   */
		if (musb->bulk_ep)
			continue;
		musb->bulk_ep = hw_ep;
	}

	if (!musb->bulk_ep) {
		pr_debug("%s: missing bulk\n", musb_driver_name);
		return -EINVAL;
	}

	return 0;
}

enum { MUSB_CONTROLLER_MHDRC, MUSB_CONTROLLER_HDRC, };

/*                                                            
                                                         
 */
static int musb_core_init(u16 musb_type, struct musb *musb)
{
	u8 reg;
	char *type;
	char aInfo[90], aRevision[32], aDate[12];
	void __iomem	*mbase = musb->mregs;
	int		status = 0;
	int		i;

	/*                                             */
	reg = musb_read_configdata(mbase);

	strcpy(aInfo, (reg & MUSB_CONFIGDATA_UTMIDW) ? "UTMI-16" : "UTMI-8");
	if (reg & MUSB_CONFIGDATA_DYNFIFO) {
		strcat(aInfo, ", dyn FIFOs");
		musb->dyn_fifo = true;
	}
	if (reg & MUSB_CONFIGDATA_MPRXE) {
		strcat(aInfo, ", bulk combine");
		musb->bulk_combine = true;
	}
	if (reg & MUSB_CONFIGDATA_MPTXE) {
		strcat(aInfo, ", bulk split");
		musb->bulk_split = true;
	}
	if (reg & MUSB_CONFIGDATA_HBRXE) {
		strcat(aInfo, ", HB-ISO Rx");
		musb->hb_iso_rx = true;
	}
	if (reg & MUSB_CONFIGDATA_HBTXE) {
		strcat(aInfo, ", HB-ISO Tx");
		musb->hb_iso_tx = true;
	}
	if (reg & MUSB_CONFIGDATA_SOFTCONE)
		strcat(aInfo, ", SoftConn");

	DBG(0,"%s: ConfigData=0x%02x (%s)\n",
			musb_driver_name, reg, aInfo);

	aDate[0] = 0;
	if (MUSB_CONTROLLER_MHDRC == musb_type) {
		musb->is_multipoint = 1;
		type = "M";
	} else {
		musb->is_multipoint = 0;
		type = "";
#ifndef	CONFIG_USB_OTG_BLACKLIST_HUB
		printk(KERN_ERR
			"%s: kernel must blacklist external hubs\n",
			musb_driver_name);
#endif
	}

	/*                  */
	musb->hwvers = musb_read_hwvers(mbase);
	snprintf(aRevision, 32, "%d.%d%s", MUSB_HWVERS_MAJOR(musb->hwvers),
		MUSB_HWVERS_MINOR(musb->hwvers),
		(musb->hwvers & MUSB_HWVERS_RC) ? "RC" : "");
	printk(KERN_DEBUG "%s: %sHDRC RTL version %s %s\n",
			musb_driver_name, type, aRevision, aDate);

	/*               */
	musb_configure_ep0(musb);

	/*                                 */
	musb->nr_endpoints = 1;
	musb->epmask = 1;


	if (musb->dyn_fifo) {
		status = ep_config_from_table(musb);
		DBG(0, "ep_config_from_table %d\n",status);
	} else {
		status = ep_config_from_hw(musb);
		DBG(0, "ep_config_from_hw %d\n",status);
	}


	if (status < 0)
		return status;

	/*                                        */
	for (i = 0; i < musb->nr_endpoints; i++) {
		struct musb_hw_ep	*hw_ep = musb->endpoints + i;

		hw_ep->fifo = MUSB_FIFO_OFFSET(i) + mbase;
		hw_ep->regs = MUSB_EP_OFFSET(i, 0) + mbase;
#ifdef CONFIG_USB_MTK_HDRC_HCD
		hw_ep->rx_reinit = 1;
		hw_ep->tx_reinit = 1;
#endif
		if (hw_ep->max_packet_sz_tx) {
			DBG(0,
				"%s: hw_ep %d%s, %smax %d\n",
				musb_driver_name, i,
				hw_ep->is_shared_fifo ? "shared" : "tx",
				hw_ep->tx_double_buffered
					? "doublebuffer, " : "",
				hw_ep->max_packet_sz_tx);
		}
		if (hw_ep->max_packet_sz_rx && !hw_ep->is_shared_fifo) {
			DBG(0,
				"%s: hw_ep %d%s, %smax %d\n",
				musb_driver_name, i,
				"rx",
				hw_ep->rx_double_buffered
					? "doublebuffer, " : "",
				hw_ep->max_packet_sz_rx);
		}
		if (!(hw_ep->max_packet_sz_tx || hw_ep->max_packet_sz_rx))
			DBG(0, "hw_ep %d not configured\n", i);
	}
	DBG(0,"musb_core_init end\n");
	return 0;
}



/*
                                                                          
                                                                        
                                                              
  
                                                         
 */
irqreturn_t musb_interrupt(struct musb *musb)
{
	irqreturn_t	retval = IRQ_NONE;
	u8		devctl;
	int		ep_num;
	u32		reg;

	devctl = musb_readb(musb->mregs, MUSB_DEVCTL);

	DBG(1, "** IRQ %s usb%04x tx%04x rx%04x\n",
		(devctl & MUSB_DEVCTL_HM) ? "host" : "peripheral",
		musb->int_usb, musb->int_tx, musb->int_rx);

	USB_LOGGER( MUSB_INTERRUPT, MUSB_INTERRUPT, \
		(musb->is_host) ? "Host" : "Dev", musb->int_usb, \
		musb->int_tx, musb->int_rx);

	dumpTime(funcInterrupt, 0);

	/*                                                          
                                           
  */
	if (musb->int_usb)
		retval |= musb_stage0_irq(musb, musb->int_usb,
				devctl);

	/*                                     */

	/*                         */
	if (musb->int_tx & 1) {
		if (devctl & MUSB_DEVCTL_HM)
			retval |= musb_h_ep0_irq(musb);
		else
			retval |= musb_g_ep0_irq(musb);
	}

	/*                      */
	reg = musb->int_rx >> 1;
	ep_num = 1;
	while (reg) {
		if (reg & 1) {
			/*                                      */
			/*                                       */
			retval = IRQ_HANDLED;
			if (devctl & MUSB_DEVCTL_HM)
				musb_host_rx(musb, ep_num);
			else
				musb_g_rx(musb, ep_num);
			}

		reg >>= 1;
		ep_num++;
	}

	/*                      */
	reg = musb->int_tx >> 1;
	ep_num = 1;
	while (reg) {
		if (reg & 1) {
			/*                                      */
			/*                                        */
			retval = IRQ_HANDLED;
			if (devctl & MUSB_DEVCTL_HM)
					musb_host_tx(musb, ep_num);
			else
					musb_g_tx(musb, ep_num);
			}
		reg >>= 1;
		ep_num++;
	}

	return retval;
}
EXPORT_SYMBOL_GPL(musb_interrupt);

#ifndef CONFIG_MUSB_PIO_ONLY
static bool use_dma = 1;

/*                              */
module_param(use_dma, bool, 0);
MODULE_PARM_DESC(use_dma, "enable/disable use of DMA");

void musb_dma_completion(struct musb *musb, u8 epnum, u8 transmit)
{
	u8	devctl = musb_readb(musb->mregs, MUSB_DEVCTL);
        DBG(1,"musb_dma_completion %d   tx %d\n", epnum, transmit);

	/*                                          */

	if (!epnum) {
			/*            */
			if (devctl & MUSB_DEVCTL_HM)
				musb_h_ep0_irq(musb);
			else
				musb_g_ep0_irq(musb);
	} else {
		/*                 */
		if (transmit) {
			if (devctl & MUSB_DEVCTL_HM)
					musb_host_tx(musb, epnum);
			else
					musb_g_tx(musb, epnum);
		} else {
			/*         */
			if (devctl & MUSB_DEVCTL_HM)
					musb_host_rx(musb, epnum);
			else
					musb_g_rx(musb, epnum);
			}
		}
}
EXPORT_SYMBOL_GPL(musb_dma_completion);

#else
#define use_dma			0
#endif

/*                                                                         */
#undef CONFIG_SYSFS
#ifdef CONFIG_SYSFS

static ssize_t
musb_mode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct musb *musb = dev_to_musb(dev);
	unsigned long flags;
	int ret = -EINVAL;

	spin_lock_irqsave(&musb->lock, flags);
	ret = sprintf(buf, "%s\n", otg_state_string(musb->xceiv->state));
	spin_unlock_irqrestore(&musb->lock, flags);

	return ret;
}

static ssize_t
musb_mode_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t n)
{
	struct musb	*musb = dev_to_musb(dev);
	unsigned long	flags;
	int		status;

	spin_lock_irqsave(&musb->lock, flags);
	if (sysfs_streq(buf, "host"))
		status = musb_platform_set_mode(musb, MUSB_HOST);
	else if (sysfs_streq(buf, "peripheral"))
		status = musb_platform_set_mode(musb, MUSB_PERIPHERAL);
	else if (sysfs_streq(buf, "otg"))
		status = musb_platform_set_mode(musb, MUSB_OTG);
	else
		status = -EINVAL;
	spin_unlock_irqrestore(&musb->lock, flags);

	return (status == 0) ? n : status;
}
static DEVICE_ATTR(mode, 0644, musb_mode_show, musb_mode_store);

static ssize_t
musb_vbus_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t n)
{
	struct musb	*musb = dev_to_musb(dev);
	unsigned long	flags;
	unsigned long	val;

	if (sscanf(buf, "%lu", &val) < 1) {
		DBG(0, "Invalid VBUS timeout ms value\n");
		return -EINVAL;
	}

	spin_lock_irqsave(&musb->lock, flags);
	/*                                                      */
	musb->a_wait_bcon = val ? max_t(int, val, OTG_TIME_A_WAIT_BCON) : 0 ;
	if (musb->xceiv->state == OTG_STATE_A_WAIT_BCON)
		musb->is_active = 0;
	musb_platform_try_idle(musb, jiffies + msecs_to_jiffies(val));
	spin_unlock_irqrestore(&musb->lock, flags);

	return n;
}

static ssize_t
musb_vbus_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct musb	*musb = dev_to_musb(dev);
	unsigned long	flags;
	unsigned long	val;
	int		vbus;

	spin_lock_irqsave(&musb->lock, flags);
	val = musb->a_wait_bcon;
	/*                                                         
                                     
  */
	vbus = musb_platform_get_vbus_status(musb);
	spin_unlock_irqrestore(&musb->lock, flags);

	return sprintf(buf, "Vbus %s, timeout %lu msec\n",
			vbus ? "on" : "off", val);
}
static DEVICE_ATTR(vbus, 0644, musb_vbus_show, musb_vbus_store);

/*                                                                      
                                                                      
 */
static ssize_t
musb_srp_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t n)
{
	struct musb	*musb = dev_to_musb(dev);
	unsigned short	srp;

	if (sscanf(buf, "%hu", &srp) != 1
			|| (srp != 1)) {
		DBG(0, "SRP: Value must be 1\n");
		return -EINVAL;
	}

	if (srp == 1)
		musb_g_wakeup(musb);

	return n;
}
static DEVICE_ATTR(srp, 0644, NULL, musb_srp_store);

static struct attribute *musb_attributes[] = {
	&dev_attr_mode.attr,
	&dev_attr_vbus.attr,
	&dev_attr_srp.attr,
	NULL
};

static const struct attribute_group musb_attr_group = {
	.attrs = musb_attributes,
};

#endif	/*       */

/*                                                */
static void musb_irq_work(struct work_struct *data)
{
	struct musb *musb = container_of(data, struct musb, irq_work);

	if (musb->xceiv->state != musb->xceiv_old_state) {
		musb->xceiv_old_state = musb->xceiv->state;
		sysfs_notify(&musb->controller->kobj, NULL, "mode");
	}
}

/*                                                                           
               
 */

static struct musb *allocate_instance(struct device *dev,
		struct musb_hdrc_config *config, void __iomem *mbase)
{
	struct musb		*musb;
	struct musb_hw_ep	*ep;
	int			epnum;
	struct usb_hcd	*hcd;

	hcd = usb_create_hcd(&musb_hc_driver, dev, dev_name(dev));
	if (!hcd)
		return NULL;
	/*                                                                  */

	musb = hcd_to_musb(hcd);
	INIT_LIST_HEAD(&musb->control);
	INIT_LIST_HEAD(&musb->in_bulk);
	INIT_LIST_HEAD(&musb->out_bulk);

	hcd->uses_new_polling = 1;
	hcd->has_tt = 1;

	musb->vbuserr_retry = VBUSERR_RETRY_COUNT;
	musb->a_wait_bcon = OTG_TIME_A_WAIT_BCON;
	dev_set_drvdata(dev, musb);
	musb->mregs = mbase;
	musb->ctrl_base = mbase;
	musb->nIrq = -ENODEV;
	musb->config = config;
    musb->is_ready = false;
    musb->in_ipo_off = false;

	BUG_ON(musb->config->num_eps > MUSB_C_NUM_EPS);
	for (epnum = 0, ep = musb->endpoints;
			epnum < musb->config->num_eps;
			epnum++, ep++) {
		ep->musb = musb;
		ep->epnum = epnum;
	}

	musb->controller = dev;

	return musb;
}

static void musb_free(struct musb *musb)
{
	/*                                                              
                                                                   
                                                 
  */

#ifdef CONFIG_SYSFS
	sysfs_remove_group(&musb->controller->kobj, &musb_attr_group);
#endif

	if (musb->nIrq >= 0) {
		if (musb->irq_wake)
			disable_irq_wake(musb->nIrq);
		free_irq(musb->nIrq, musb);
	}
	if (is_dma_capable() && musb->dma_controller) {
		struct dma_controller	*c = musb->dma_controller;

		(void) c->stop(c);
		dma_controller_destroy(c);
	}

	usb_put_hcd(musb_to_hcd(musb));
}

/*
                                                 
  
                                              
                    
                                                  
                                                  
 */
 #ifdef CONFIG_OF
static int
musb_init_controller(struct device *dev, int nIrq, void __iomem *ctrl, void __iomem *ctrlp)
#else
static int
musb_init_controller(struct device *dev, int nIrq, void __iomem *ctrl)
#endif
{
	int			status;
	struct musb		*musb;
	struct musb_hdrc_platform_data *plat = dev->platform_data;
	struct usb_hcd		*hcd;

	/*                                                          
                                                           
  */
	if (!plat) {
		DBG(0, "no platform_data?\n");
		status = -ENODEV;
		goto fail0;
	}

	/*          */
	musb = allocate_instance(dev, plat->config, ctrl);
	if (!musb) {
		status = -ENOMEM;
		goto fail0;
	}

	mtk_musb = musb;
	sema_init(&musb->musb_lock, 1);
	pm_runtime_use_autosuspend(musb->controller);
	pm_runtime_set_autosuspend_delay(musb->controller, 200);
	pm_runtime_enable(musb->controller);

	spin_lock_init(&musb->lock);
	musb->board_set_power = plat->set_power;
	musb->min_power = plat->min_power;
	musb->ops = plat->platform_ops;
    musb->nIrq = nIrq;
	/*                               
                           
                          
                                               
                                                         
                           
   
                                                            
                                                             
                                                              
                                                          
  */
	status = musb_platform_init(musb);
    //                                                 
#ifdef CONFIG_OF
    musb->xceiv->io_priv = ctrlp;
#endif
   musb_platform_enable(musb);
    //                  
	if (status < 0)
		goto fail1;

	if (!musb->isr) {
		status = -ENODEV;
		goto fail2;
	}

	if (!musb->xceiv->io_ops) {
		musb->xceiv->io_dev = musb->controller;
		//                                   
		musb->xceiv->io_ops = &musb_ulpi_access;
	}

	pm_runtime_get_sync(musb->controller);

#ifndef CONFIG_MUSB_PIO_ONLY
	if (use_dma && dev->dma_mask) {
		struct dma_controller	*c;

		c = dma_controller_create(musb, musb->mregs);
		musb->dma_controller = c;
		if (c)
			(void) c->start(c);
	}
#endif
	/*                                                    */
	if (!is_dma_capable() || !musb->dma_controller)
		dev->dma_mask = NULL;

	/*                                                       */
	musb_generic_disable(musb);
	musb_platform_disable(musb);

	/*                                                     */
	status = musb_core_init(plat->config->multipoint
			? MUSB_CONTROLLER_MHDRC
			: MUSB_CONTROLLER_HDRC, musb);
	if (status < 0)
		goto fail3;

	setup_timer(&musb->otg_timer, musb_otg_timer_func, (unsigned long) musb);

	/*                                       */
	INIT_WORK(&musb->irq_work, musb_irq_work);
    printk("musb irq number : %d", musb->nIrq);
	/*                   */
	if (request_irq(musb->nIrq, musb->isr, IRQF_TRIGGER_LOW, dev_name(dev), musb)) {
		DBG(0, "request_irq %d failed!\n", musb->nIrq);
		status = -ENODEV;
		goto fail3;
	}
#if 0
	musb->nIrq = nIrq;
	/*                                      */
	if (enable_irq_wake(nIrq) == 0) {
		musb->irq_wake = 1;
		device_init_wakeup(dev, 1);
	} else {
		musb->irq_wake = 0;
	}
#endif
	/*                            */
	hcd = musb_to_hcd(musb);
	otg_set_host(musb->xceiv->otg, &hcd->self);
	hcd->self.otg_port = 1;
	musb->xceiv->otg->host = &hcd->self;
	hcd->power_budget = 2 * (plat->power ? : 250);

		/*                                              */
	if (plat->extvbus) {
		u8 busctl = musb_read_ulpi_buscontrol(musb->mregs);
		busctl |= MUSB_ULPI_USE_EXTVBUS;
		musb_write_ulpi_buscontrol(musb->mregs, busctl);
	}

	MUSB_DEV_MODE(musb);
	musb->xceiv->otg->default_a = 0;
	musb->xceiv->state = OTG_STATE_B_IDLE;

		status = musb_gadget_setup(musb);

	/*                          */
	musb_platform_disable(musb);

	if (status < 0)
		goto fail3;

	status = musb_init_debugfs(musb);
	if (status < 0)
		goto fail4;

#ifdef CONFIG_SYSFS
	status = sysfs_create_group(&musb->controller->kobj, &musb_attr_group);
	if (status)
		goto fail5;
#endif

	pm_runtime_put(musb->controller);

	return 0;

#ifdef CONFIG_SYSFS
fail5:
	musb_exit_debugfs(musb);
#endif

fail4:
	musb_gadget_cleanup(musb);

fail3:
	pm_runtime_put_sync(musb->controller);

fail2:
	if (musb->irq_wake)
		device_init_wakeup(dev, 0);
	musb_platform_exit(musb);

fail1:
	DBG(0, "musb_init_controller failed with status %d\n", status);

	musb_free(musb);

fail0:

	return status;

}

/*                                                                         */

/*                                                                 
                                                          
 */
static int musb_probe(struct platform_device *pdev)
{
	struct device	*dev = &pdev->dev;
	int		irq = 0;
	int		status;
    void __iomem	*base;
    //                              
	/*                         */
#ifdef CONFIG_OF
    void __iomem	*pbase;
    u32 musb_temp_id = 0;
    unsigned long usb_mac_base;
    unsigned long usb_phy_base;

     pr_info("musb probe\n");
     pdev->dev.of_node = of_find_compatible_node(NULL,NULL,"mediatek,USB0");
     if(pdev->dev.of_node == NULL){
          pr_info("USB get node failed\n");
     }
        base = of_iomap(pdev->dev.of_node, 0);
        usb_irq_number = irq_of_parse_and_map(pdev->dev.of_node, 0);
    pbase = of_iomap(pdev->dev.of_node, 1);

    usb_mac_base = (unsigned long)base;
    usb_phy_base = (unsigned long)pbase;
    irq = usb_irq_number;
    pr_info("musb probe reg: 0x%lx ,0x%lx , irq: 0x%d \n", usb_mac_base, usb_phy_base, usb_irq_number);
#endif
#ifdef CONFIG_OF
status = musb_init_controller(dev, irq, base, pbase);
#else
	base = (void *)USB_BASE;
status = musb_init_controller(dev, irq, base);
#endif


#if 0
	if (status < 0)
		iounmap(base);
#endif

    #ifdef CONFIG_OF
    usb_mac_base = (unsigned long)mtk_musb->xceiv->io_priv;
    pr_info("musb core probe done base 0x%lx\n", usb_mac_base);
    #endif
	return status;
}

static int musb_remove(struct platform_device *pdev)
{
	struct device	*dev = &pdev->dev;
	struct musb	*musb = dev_to_musb(dev);
	/*                                            */

	/*                           
                                          
                                                                      
                                                                
  */
	musb_exit_debugfs(musb);
	musb_shutdown(pdev);

	musb_free(musb);

	/*                     */

	device_init_wakeup(dev, 0);

#ifndef CONFIG_MUSB_PIO_ONLY
	dma_set_mask(dev, *dev->parent->dma_mask);
#endif
	return 0;
}

#ifdef CONFIG_PM

static void musb_save_context(struct musb *musb)
{
	int i;
	void __iomem *musb_base = musb->mregs;
	void __iomem *epio;

	musb->context.power = musb_readb(musb_base, MUSB_POWER);
	musb->context.intrusbe = musb_readb(musb_base, MUSB_INTRUSBE);
	musb->context.index = musb_readb(musb_base, MUSB_INDEX);
	musb->context.devctl = musb_readb(musb_base, MUSB_DEVCTL);

	musb->context.l1_int = musb_readl(musb_base, USB_L1INTM);

	for (i = 0; i < musb->config->num_eps; ++i) {
		struct musb_hw_ep	*hw_ep;

		hw_ep = &musb->endpoints[i];
		if (!hw_ep)
			continue;

		epio = hw_ep->regs;
		if (!epio)
			continue;

		musb_writeb(musb_base, MUSB_INDEX, i);
		musb->context.index_regs[i].txmaxp =
			musb_readw(epio, MUSB_TXMAXP);
		musb->context.index_regs[i].txcsr =
			musb_readw(epio, MUSB_TXCSR);
		musb->context.index_regs[i].rxmaxp =
			musb_readw(epio, MUSB_RXMAXP);
		musb->context.index_regs[i].rxcsr =
			musb_readw(epio, MUSB_RXCSR);

		if (musb->dyn_fifo) {
			musb->context.index_regs[i].txfifoadd =
					musb_read_txfifoadd(musb_base);
			musb->context.index_regs[i].rxfifoadd =
					musb_read_rxfifoadd(musb_base);
			musb->context.index_regs[i].txfifosz =
					musb_read_txfifosz(musb_base);
			musb->context.index_regs[i].rxfifosz =
					musb_read_rxfifosz(musb_base);
		}
	}
}

static void musb_restore_context(struct musb *musb)
{
	int i;
	void __iomem *musb_base = musb->mregs;
	void __iomem *epio;

	musb_writeb(musb_base, MUSB_POWER, musb->context.power);
	musb_writew(musb_base, MUSB_INTRTXE, musb->intrtxe);
	musb_writew(musb_base, MUSB_INTRRXE, musb->intrrxe);
	musb_writeb(musb_base, MUSB_INTRUSBE, musb->context.intrusbe);
	musb_writeb(musb_base, MUSB_DEVCTL, musb->context.devctl);

	for (i = 0; i < musb->config->num_eps; ++i) {
		struct musb_hw_ep	*hw_ep;

		hw_ep = &musb->endpoints[i];
		if (!hw_ep)
			continue;

		epio = hw_ep->regs;
		if (!epio)
			continue;

		musb_writeb(musb_base, MUSB_INDEX, i);
		musb_writew(epio, MUSB_TXMAXP,
			musb->context.index_regs[i].txmaxp);
		musb_writew(epio, MUSB_TXCSR,
			musb->context.index_regs[i].txcsr);
		musb_writew(epio, MUSB_RXMAXP,
			musb->context.index_regs[i].rxmaxp);
		musb_writew(epio, MUSB_RXCSR,
			musb->context.index_regs[i].rxcsr);

		if (musb->dyn_fifo) {
			musb_write_txfifosz(musb_base,
				musb->context.index_regs[i].txfifosz);
			musb_write_rxfifosz(musb_base,
				musb->context.index_regs[i].rxfifosz);
			musb_write_txfifoadd(musb_base,
				musb->context.index_regs[i].txfifoadd);
			musb_write_rxfifoadd(musb_base,
				musb->context.index_regs[i].rxfifoadd);
		}
	}

	musb_writeb(musb_base, MUSB_INDEX, musb->context.index);

	/*                             
                                                      
  */
	musb_writel(musb->mregs, MUSB_HSDMA_INTR, 0xFF | (0xFF << DMA_INTR_UNMASK_SET_OFFSET));

	musb_writel(musb_base, USB_L1INTM, musb->context.l1_int);
}

static int musb_suspend_noirq(struct device *dev)
{
	struct musb	*musb = dev_to_musb(dev);
	/*                    */

	/*                                */
	/*                                      */

	/*                                                 */
	mtk_usb_power = true;
	usb_enable_clock(true);

	musb_save_context(musb);
	usb_phy_context_save();

	/*                                                */
	usb_enable_clock(false);
	mtk_usb_power = false;

	/*                                           */
	return 0;
}


static int musb_resume_noirq(struct device *dev)
{
	struct musb	*musb = dev_to_musb(dev);

	/*                                                 */
	mtk_usb_power = true;
	usb_enable_clock(true);

	musb_restore_context(musb);
    usb_phy_context_restore();
	/*                                                */
	usb_enable_clock(false);
	mtk_usb_power = false;

	return 0;
}

static const struct dev_pm_ops musb_dev_pm_ops = {
	.suspend_noirq		= musb_suspend_noirq,
	.resume_noirq	= musb_resume_noirq,
};

#define MUSB_DEV_PM_OPS (&musb_dev_pm_ops)
#else
#define	MUSB_DEV_PM_OPS	NULL
#endif

static struct platform_driver musb_driver = {
	.driver = {
		.name		= (char *)musb_driver_name,
		.bus		= &platform_bus_type,
		.of_match_table = apusb_of_ids,
		.owner		= THIS_MODULE,
		.pm		= MUSB_DEV_PM_OPS,
	},
	.probe		= musb_probe,
	.remove		= musb_remove,
	.shutdown	= musb_shutdown,
};

/*                                                                         */

static int __init musb_init(void)
{
	if (usb_disabled())
		return 0;

	pr_info("%s: version " MUSB_VERSION ", ?dma?, otg (peripheral+host)\n",
		musb_driver_name);
	return platform_driver_register(&musb_driver);
}
module_init(musb_init);

static void __exit musb_cleanup(void)
{
	platform_driver_unregister(&musb_driver);
}
module_exit(musb_cleanup);
