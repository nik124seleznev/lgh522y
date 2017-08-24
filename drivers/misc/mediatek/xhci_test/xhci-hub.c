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

#include <asm/unaligned.h>

#include "xhci.h"
#include "mtk-usb-hcd.h"

static void xhci_hub_descriptor(struct xhci_hcd *xhci,
		struct usb_hub_descriptor *desc)
{
	int ports;
	u16 temp;

	ports = HCS_MAX_PORTS(xhci->hcs_params1);

	/*                                                                    */
	desc->bDescriptorType = 0x29;
	desc->bPwrOn2PwrGood = 10;	/*                                  */
	desc->bHubContrCurrent = 0;

	desc->bNbrPorts = ports;
	temp = 1 + (ports / 8);
	desc->bDescLength = 7 + 2 * temp;

	/*                                                          */
	//                                           
	//                                                 

	/*                                      */
	/*                                    */
	temp = 0;
	/*                                                             */
	if (HCC_PPC(xhci->hcc_params))
		temp |= 0x0001;
	else
		temp |= 0x0002;
	/*                                                      */
	/*                                                    */
	temp |= 0x0008;
	/*                                 */
	/*                             */
	desc->wHubCharacteristics = (__force __u16) cpu_to_le16(temp);
}

unsigned int mtktest_xhci_port_speed(unsigned int port_status)
{
	if (DEV_LOWSPEED(port_status)){
		xhci_dbg(xhci, "speed low\n");
		return USB_PORT_STAT_LOW_SPEED;
	}
	if (DEV_HIGHSPEED(port_status)){
		xhci_dbg(xhci, "speed high\n");
		return USB_PORT_STAT_HIGH_SPEED;
	}
	if (DEV_SUPERSPEED(port_status)){
		xhci_dbg(xhci, "speed super\n");
		return USB_PORT_STAT_SUPER_SPEED;
	}
	/*
                                                                         
                                                                       
                                   
  */
	xhci_dbg(xhci, "speed none\n");
	return 0;
}

/*
                                                                       
                             
                                                                         
                                                                     
                                                                      
 */
#define	XHCI_PORT_RO	((1<<0) | (1<<3) | (0xf<<10) | (1<<30))
/*
                                                                           
                            
                                                                       
 */
#define XHCI_PORT_RWS	((0xf<<5) | (1<<9) | (0x3<<14) | (0x7<<25))
/*
                                                                          
                     
 */
#define	XHCI_PORT_RW1S	((1<<4))
/*
                                                                            
                                     
                           
                                                                                        
                                                 
 */
#define XHCI_PORT_RW1CS	((1<<1) | (0x7f<<17))

#define XHCI_PORT_RW1C (0x7f<<17)
/*
                                                                            
             
 */
#define	XHCI_PORT_RW	((1<<16))
/*
                                                                           
                    
 */
#define	XHCI_PORT_RZ	((1<<2) | (1<<24) | (0xf<<28))

/*
                                                                             
                                                                            
                    
                                                          
                                                                 
                                                                              
 */
u32 mtktest_xhci_port_state_to_neutral(u32 state)
{
	/*                                      */
	return (state & XHCI_PORT_RO) | (state & XHCI_PORT_RWS);
}


u32 mtktest_xhci_port_state_to_clear_change(u32 state){
	return (state & XHCI_PORT_RO) | (state & XHCI_PORT_RWS) | (state & XHCI_PORT_RW1C);
}

static void xhci_disable_port(struct xhci_hcd *xhci, u16 wIndex,
		u32 __iomem *addr, u32 port_status)
{
	/*                             */
	xhci_writel(xhci, port_status | PORT_PE, addr);
	port_status = xhci_readl(xhci, addr);
	xhci_dbg(xhci, "disable port, actual port %d status  = 0x%x\n",
			wIndex, port_status);
}

static void xhci_clear_port_change_bit(struct xhci_hcd *xhci, u16 wValue,
		u16 wIndex, u32 __iomem *addr, u32 port_status)
{
	char *port_change_bit;
	u32 status;

	switch (wValue) {
	case USB_PORT_FEAT_C_RESET:
		status = PORT_RC;
		port_change_bit = "reset";
		break;
	case USB_PORT_FEAT_C_CONNECTION:
		status = PORT_CSC;
		port_change_bit = "connect";
		break;
	case USB_PORT_FEAT_C_OVER_CURRENT:
		status = PORT_OCC;
		port_change_bit = "over-current";
		break;
	case USB_PORT_FEAT_C_ENABLE:
		status = PORT_PEC;
		port_change_bit = "enable/disable";
		break;
	default:
		/*                     */
		return;
	}
	/*                                      */
	xhci_writel(xhci, port_status | status, addr);
	port_status = xhci_readl(xhci, addr);
	xhci_dbg(xhci, "clear port %s change, actual port %d status  = 0x%x\n",
			port_change_bit, wIndex, port_status);
}

int mtktest_xhci_hub_control(struct usb_hcd *hcd, u16 typeReq, u16 wValue,
		u16 wIndex, char *buf, u16 wLength)
{
	struct xhci_hcd	*xhci = hcd_to_xhci(hcd);
	int ports;
	unsigned long flags;
	u32 temp, status;
	int retval = 0;
	u32 __iomem *addr;

	ports = HCS_MAX_PORTS(xhci->hcs_params1);

	spin_lock_irqsave(&xhci->lock, flags);
	switch (typeReq) {
	case GetHubStatus:
		/*                                                 */
		memset(buf, 0, 4);
		break;
	case GetHubDescriptor:
		xhci_hub_descriptor(xhci, (struct usb_hub_descriptor *) buf);
		break;
	case GetPortStatus:
		if (!wIndex || wIndex > ports)
			goto error;
		wIndex--;
		status = 0;
		addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(wIndex & 0xff);
		temp = xhci_readl(xhci, addr);
		xhci_dbg(xhci, "get port status, actual port %d status  = 0x%x\n", wIndex, temp);

		printk("[OTG_H] mtktest_xhci_hub_control, port %d status  = 0x%x\n", wIndex, temp);

		/*                  */
		if (temp & PORT_CSC)
			status |= USB_PORT_STAT_C_CONNECTION << 16;
		if (temp & PORT_PEC)
			status |= USB_PORT_STAT_C_ENABLE << 16;
		if ((temp & PORT_OCC))
			status |= USB_PORT_STAT_C_OVERCURRENT << 16;
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
		xhci_dbg(xhci, "Get port status returned 0x%x\n", status);
		put_unaligned(cpu_to_le32(status), (__le32 *) buf);
		break;
	case SetPortFeature:
		wIndex &= 0xff;
		if (!wIndex || wIndex > ports)
			goto error;
		wIndex--;
		addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(wIndex & 0xff);
		temp = xhci_readl(xhci, addr);
		temp = mtktest_xhci_port_state_to_neutral(temp);
		switch (wValue) {
		case USB_PORT_FEAT_POWER:
			/*
                                                            
                                                            
                                                         
                                
    */
			xhci_writel(xhci, temp | PORT_POWER, addr);

			temp = xhci_readl(xhci, addr);
			xhci_dbg(xhci, "set port power, actual port %d status  = 0x%x\n", wIndex, temp);
			break;
		case USB_PORT_FEAT_RESET:
			temp = (temp | PORT_RESET);
			xhci_writel(xhci, temp, addr);

			temp = xhci_readl(xhci, addr);
			xhci_dbg(xhci, "set port reset, actual port %d status  = 0x%x\n", wIndex, temp);
			break;
		default:
			goto error;
		}
		temp = xhci_readl(xhci, addr); /*                           */
		break;
	case ClearPortFeature:
		if (!wIndex || wIndex > ports)
			goto error;
		wIndex--;
		addr = &xhci->op_regs->port_status_base +
			NUM_PORT_REGS*(wIndex & 0xff);
		temp = xhci_readl(xhci, addr);
		temp = mtktest_xhci_port_state_to_neutral(temp);
		switch (wValue) {
		case USB_PORT_FEAT_C_RESET:
		case USB_PORT_FEAT_C_CONNECTION:
		case USB_PORT_FEAT_C_OVER_CURRENT:
		case USB_PORT_FEAT_C_ENABLE:
			xhci_clear_port_change_bit(xhci, wValue, wIndex,
					addr, temp);
			break;
		case USB_PORT_FEAT_ENABLE:
			xhci_disable_port(xhci, wIndex, addr, temp);
			break;
		default:
			goto error;
		}
		break;
	default:
error:
		/*                  */
		retval = -EPIPE;
	}
	spin_unlock_irqrestore(&xhci->lock, flags);
	return retval;
}

/*
                                                                         
                                                  
                                                   
  
                                                                            
                                                                         
 */
int mtktest_xhci_hub_status_data(struct usb_hcd *hcd, char *buf)
{
	unsigned long flags;
	u32 temp, status;
	int i, retval;
	struct xhci_hcd	*xhci = hcd_to_xhci(hcd);
	int ports;
	u32 __iomem *addr;

	ports = HCS_MAX_PORTS(xhci->hcs_params1);

	/*                              */
	retval = (ports + 8) / 8;
	memset(buf, 0, retval);
	status = 0;

	spin_lock_irqsave(&xhci->lock, flags);
	/*                                                                  */
	for (i = 0; i < ports; i++) {
		addr = &xhci->op_regs->port_status_base +
			NUM_PORT_REGS*i;
		temp = xhci_readl(xhci, addr);
		if (temp & (PORT_CSC | PORT_PEC | PORT_OCC)) {
			buf[(i + 1) / 8] |= 1 << (i + 1) % 8;
			status = 1;
		}
	}
	spin_unlock_irqrestore(&xhci->lock, flags);
	return status ? retval : 0;
}
