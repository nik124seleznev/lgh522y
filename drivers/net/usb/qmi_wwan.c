/*
 * Copyright (c) 2012  Bjørn Mork <bjorn@mork.no>
 *
 * The probing code is heavily inspired by cdc_ether, which is:
 * Copyright (C) 2003-2005 by David Brownell
 * Copyright (C) 2006 by Ole Andre Vadla Ravnas (ActiveSync)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/etherdevice.h>
#include <linux/mii.h>
#include <linux/usb.h>
#include <linux/usb/cdc.h>
#include <linux/usb/usbnet.h>
#include <linux/usb/cdc-wdm.h>

/*                                                            
                                                                     
                                                                   
             
  
                                                                
                                                            
                                                                   
                                                                 
                                                           
                                                                  
  
                                                                 
                        
  
                                                                   
                                                                   
                                                                     
             
  
                                                                      
                                 
 */

/*                      */
struct qmi_wwan_state {
	struct usb_driver *subdriver;
	atomic_t pmcount;
	unsigned long unused;
	struct usb_interface *control;
	struct usb_interface *data;
};

/*                                            */
static const u8 default_modem_addr[ETH_ALEN] = {0x02, 0x50, 0xf3};

/*                                                           
  
                                                                     
                                                                
                                                                    
                                                              
  
                                                                   
                                                                    
                                                               
                                                                  
  
                                                                  
                                                                     
               
  
                                                                     
                                                                 
                                              
 */
static int qmi_wwan_rx_fixup(struct usbnet *dev, struct sk_buff *skb)
{
	__be16 proto;

	/*                                        */
	if (skb->len < dev->net->hard_header_len)
		return 0;

	switch (skb->data[0] & 0xf0) {
	case 0x40:
		proto = htons(ETH_P_IP);
		break;
	case 0x60:
		proto = htons(ETH_P_IPV6);
		break;
	case 0x00:
		if (is_multicast_ether_addr(skb->data))
			return 1;
		/*                                                   */
		skb_reset_mac_header(skb);
		goto fix_dest;
	default:
		/*                                                */
		return 1;
	}
	if (skb_headroom(skb) < ETH_HLEN)
		return 0;
	skb_push(skb, ETH_HLEN);
	skb_reset_mac_header(skb);
	eth_hdr(skb)->h_proto = proto;
	memset(eth_hdr(skb)->h_source, 0, ETH_ALEN);
fix_dest:
	memcpy(eth_hdr(skb)->h_dest, dev->net->dev_addr, ETH_ALEN);
	return 1;
}

/*                                                   */
static bool possibly_iphdr(const char *data)
{
	return (data[0] & 0xd0) == 0x40;
}

/*                                                          */
static int qmi_wwan_mac_addr(struct net_device *dev, void *p)
{
	int ret;
	struct sockaddr *addr = p;

	ret = eth_prepare_mac_addr_change(dev, p);
	if (ret < 0)
		return ret;
	if (possibly_iphdr(addr->sa_data))
		return -EADDRNOTAVAIL;
	eth_commit_mac_addr_change(dev, p);
	return 0;
}

static const struct net_device_ops qmi_wwan_netdev_ops = {
	.ndo_open		= usbnet_open,
	.ndo_stop		= usbnet_stop,
	.ndo_start_xmit		= usbnet_start_xmit,
	.ndo_tx_timeout		= usbnet_tx_timeout,
	.ndo_change_mtu		= usbnet_change_mtu,
	.ndo_set_mac_address	= qmi_wwan_mac_addr,
	.ndo_validate_addr	= eth_validate_addr,
};

/*                                                                                */
static int qmi_wwan_manage_power(struct usbnet *dev, int on)
{
	struct qmi_wwan_state *info = (void *)&dev->data;
	int rv = 0;

	dev_dbg(&dev->intf->dev, "%s() pmcount=%d, on=%d\n", __func__, atomic_read(&info->pmcount), on);

	if ((on && atomic_add_return(1, &info->pmcount) == 1) || (!on && atomic_dec_and_test(&info->pmcount))) {
		/*                                                                   */
		rv = usb_autopm_get_interface(dev->intf);
		if (rv < 0)
			goto err;
		dev->intf->needs_remote_wakeup = on;
		usb_autopm_put_interface(dev->intf);
	}
err:
	return rv;
}

static int qmi_wwan_cdc_wdm_manage_power(struct usb_interface *intf, int on)
{
	struct usbnet *dev = usb_get_intfdata(intf);

	/*                                   */
	if (!dev)
		return 0;
	return qmi_wwan_manage_power(dev, on);
}

/*                                                    */
static int qmi_wwan_register_subdriver(struct usbnet *dev)
{
	int rv;
	struct usb_driver *subdriver = NULL;
	struct qmi_wwan_state *info = (void *)&dev->data;

	/*                        */
	rv = usbnet_get_endpoints(dev, info->data);
	if (rv < 0)
		goto err;

	/*                                                      */
	if (info->control != info->data)
		dev->status = &info->control->cur_altsetting->endpoint[0];

	/*                                          */
	if (!dev->status) {
		rv = -EINVAL;
		goto err;
	}

	/*                                */
	atomic_set(&info->pmcount, 0);

	/*                    */
	subdriver = usb_cdc_wdm_register(info->control, &dev->status->desc, 4096, &qmi_wwan_cdc_wdm_manage_power);
	if (IS_ERR(subdriver)) {
		dev_err(&info->control->dev, "subdriver registration failed\n");
		rv = PTR_ERR(subdriver);
		goto err;
	}

	/*                                           */
	dev->status = NULL;

	/*                                                   */
	info->subdriver = subdriver;

err:
	return rv;
}

static int qmi_wwan_bind(struct usbnet *dev, struct usb_interface *intf)
{
	int status = -1;
	u8 *buf = intf->cur_altsetting->extra;
	int len = intf->cur_altsetting->extralen;
	struct usb_interface_descriptor *desc = &intf->cur_altsetting->desc;
	struct usb_cdc_union_desc *cdc_union = NULL;
	struct usb_cdc_ether_desc *cdc_ether = NULL;
	u32 found = 0;
	struct usb_driver *driver = driver_of(intf);
	struct qmi_wwan_state *info = (void *)&dev->data;

	BUILD_BUG_ON((sizeof(((struct usbnet *)0)->data) < sizeof(struct qmi_wwan_state)));

	/*                      */
	info->control = intf;
	info->data = intf;

	/*                                 */
	while (len > 3) {
		struct usb_descriptor_header *h = (void *)buf;

		/*                                  */
		if (h->bDescriptorType != USB_DT_CS_INTERFACE)
			goto next_desc;

		/*                                  */
		switch (buf[2]) {
		case USB_CDC_HEADER_TYPE:
			if (found & 1 << USB_CDC_HEADER_TYPE) {
				dev_dbg(&intf->dev, "extra CDC header\n");
				goto err;
			}
			if (h->bLength != sizeof(struct usb_cdc_header_desc)) {
				dev_dbg(&intf->dev, "CDC header len %u\n", h->bLength);
				goto err;
			}
			break;
		case USB_CDC_UNION_TYPE:
			if (found & 1 << USB_CDC_UNION_TYPE) {
				dev_dbg(&intf->dev, "extra CDC union\n");
				goto err;
			}
			if (h->bLength != sizeof(struct usb_cdc_union_desc)) {
				dev_dbg(&intf->dev, "CDC union len %u\n", h->bLength);
				goto err;
			}
			cdc_union = (struct usb_cdc_union_desc *)buf;
			break;
		case USB_CDC_ETHERNET_TYPE:
			if (found & 1 << USB_CDC_ETHERNET_TYPE) {
				dev_dbg(&intf->dev, "extra CDC ether\n");
				goto err;
			}
			if (h->bLength != sizeof(struct usb_cdc_ether_desc)) {
				dev_dbg(&intf->dev, "CDC ether len %u\n",  h->bLength);
				goto err;
			}
			cdc_ether = (struct usb_cdc_ether_desc *)buf;
			break;
		}

		/*
                                                                 
                                                                
                                   
   */
		if (buf[2] < 32)
			found |= 1 << buf[2];

next_desc:
		len -= h->bLength;
		buf += h->bLength;
	}

	/*                                                                  */
	if (cdc_union) {
		info->data = usb_ifnum_to_if(dev->udev, cdc_union->bSlaveInterface0);
		if (desc->bInterfaceNumber != cdc_union->bMasterInterface0 || !info->data) {
			dev_err(&intf->dev, "bogus CDC Union: master=%u, slave=%u\n",
				cdc_union->bMasterInterface0, cdc_union->bSlaveInterface0);
			goto err;
		}
	}

	/*                                                            */
	if (cdc_ether) {
		dev->hard_mtu = le16_to_cpu(cdc_ether->wMaxSegmentSize);
		usbnet_get_ethernet_addr(dev, cdc_ether->iMACAddress);
	}

	/*                                    */
	if (info->control != info->data) {
		status = usb_driver_claim_interface(driver, info->data, dev);
		if (status < 0)
			goto err;
	}

	status = qmi_wwan_register_subdriver(dev);
	if (status < 0 && info->control != info->data) {
		usb_set_intfdata(info->data, NULL);
		usb_driver_release_interface(driver, info->data);
	}

	/*                                                          
                                     
  */
	if (!compare_ether_addr(dev->net->dev_addr, default_modem_addr))
		eth_hw_addr_random(dev->net);

	/*                                                        */
	if (possibly_iphdr(dev->net->dev_addr)) {
		dev->net->dev_addr[0] |= 0x02;	/*                          */
		dev->net->dev_addr[0] &= 0xbf;	/*                */
	}
	dev->net->netdev_ops = &qmi_wwan_netdev_ops;
err:
	return status;
}

static void qmi_wwan_unbind(struct usbnet *dev, struct usb_interface *intf)
{
	struct qmi_wwan_state *info = (void *)&dev->data;
	struct usb_driver *driver = driver_of(intf);
	struct usb_interface *other;

	if (info->subdriver && info->subdriver->disconnect)
		info->subdriver->disconnect(info->control);

	/*                                                   */
	if (intf == info->control)
		other = info->data;
	else
		other = info->control;

	/*                    */
	if (other && intf != other) {
		usb_set_intfdata(other, NULL);
		usb_driver_release_interface(driver, other);
	}

	info->subdriver = NULL;
	info->data = NULL;
	info->control = NULL;
}

/*                                                            
                        
  
                                                                    
                                                                
 */
static int qmi_wwan_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct usbnet *dev = usb_get_intfdata(intf);
	struct qmi_wwan_state *info = (void *)&dev->data;
	int ret;

	/*
                                                                
                                                               
                                                    
  */
	ret = usbnet_suspend(intf, message);
	if (ret < 0)
		goto err;

	if (intf == info->control && info->subdriver && info->subdriver->suspend)
		ret = info->subdriver->suspend(intf, message);
	if (ret < 0)
		usbnet_resume(intf);
err:
	return ret;
}

static int qmi_wwan_resume(struct usb_interface *intf)
{
	struct usbnet *dev = usb_get_intfdata(intf);
	struct qmi_wwan_state *info = (void *)&dev->data;
	int ret = 0;
	bool callsub = (intf == info->control && info->subdriver && info->subdriver->resume);

	if (callsub)
		ret = info->subdriver->resume(intf);
	if (ret < 0)
		goto err;
	ret = usbnet_resume(intf);
	if (ret < 0 && callsub && info->subdriver->suspend)
		info->subdriver->suspend(intf, PMSG_SUSPEND);
err:
	return ret;
}

static const struct driver_info	qmi_wwan_info = {
	.description	= "WWAN/QMI device",
	.flags		= FLAG_WWAN,
	.bind		= qmi_wwan_bind,
	.unbind		= qmi_wwan_unbind,
	.manage_power	= qmi_wwan_manage_power,
	.rx_fixup       = qmi_wwan_rx_fixup,
};

#define HUAWEI_VENDOR_ID	0x12D1

/*                                                   */
#define QMI_FIXED_INTF(vend, prod, num) \
	USB_DEVICE_INTERFACE_NUMBER(vend, prod, num), \
	.driver_info = (unsigned long)&qmi_wwan_info

/*                                                                */
#define QMI_GOBI1K_DEVICE(vend, prod) \
	QMI_FIXED_INTF(vend, prod, 3)

/*                                                                     */
#define QMI_GOBI_DEVICE(vend, prod) \
	QMI_FIXED_INTF(vend, prod, 0)

static const struct usb_device_id products[] = {
	/*                                                        */
	{	/*                                                                          */
		USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, USB_CLASS_VENDOR_SPEC, 1, 9),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                                      */
		USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, USB_CLASS_VENDOR_SPEC, 1, 57),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                        */
		USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, USB_CLASS_VENDOR_SPEC, 0x01, 0x69),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},

	/*                                                          */
	{	/*                                                   */
		USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, USB_CLASS_VENDOR_SPEC, 1, 7),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                                         */
		USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, USB_CLASS_VENDOR_SPEC, 1, 17),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                  */
		USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, USB_CLASS_VENDOR_SPEC, 0x01, 0x37),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                   */
		USB_VENDOR_AND_INTERFACE_INFO(HUAWEI_VENDOR_ID, USB_CLASS_VENDOR_SPEC, 0x01, 0x67),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                */
		USB_VENDOR_AND_INTERFACE_INFO(0x106c, USB_CLASS_VENDOR_SPEC, 0xf0, 0xff),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                 */
		USB_VENDOR_AND_INTERFACE_INFO(0x106c, USB_CLASS_VENDOR_SPEC, 0xf1, 0xff),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                           */
		USB_DEVICE_AND_INTERFACE_INFO(0x1410, 0xb001,
		                              USB_CLASS_COMM,
		                              USB_CDC_SUBCLASS_ETHERNET,
		                              USB_CDC_PROTO_NONE),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*              */
		USB_DEVICE_AND_INTERFACE_INFO(0x1410, 0x9010,
		                              USB_CLASS_COMM,
		                              USB_CDC_SUBCLASS_ETHERNET,
		                              USB_CDC_PROTO_NONE),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                   */
		USB_DEVICE_AND_INTERFACE_INFO(0x413C, 0x8195,
					      USB_CLASS_COMM,
					      USB_CDC_SUBCLASS_ETHERNET,
					      USB_CDC_PROTO_NONE),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                      */
		USB_DEVICE_AND_INTERFACE_INFO(0x413C, 0x8196,
					      USB_CLASS_COMM,
					      USB_CDC_SUBCLASS_ETHERNET,
					      USB_CDC_PROTO_NONE),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*                                   */
		USB_DEVICE_AND_INTERFACE_INFO(0x413C, 0x819b,
					      USB_CLASS_COMM,
					      USB_CDC_SUBCLASS_ETHERNET,
					      USB_CDC_PROTO_NONE),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},
	{	/*         */
		USB_DEVICE_AND_INTERFACE_INFO(0x16d5, 0x650a,
					      USB_CLASS_COMM,
					      USB_CDC_SUBCLASS_ETHERNET,
					      USB_CDC_PROTO_NONE),
		.driver_info        = (unsigned long)&qmi_wwan_info,
	},

	/*                                                            */
	{QMI_FIXED_INTF(0x0408, 0xea42, 4)},	/*                       */
	{QMI_FIXED_INTF(0x05c6, 0x7000, 0)},
	{QMI_FIXED_INTF(0x05c6, 0x7001, 1)},
	{QMI_FIXED_INTF(0x05c6, 0x7002, 1)},
	{QMI_FIXED_INTF(0x05c6, 0x7101, 1)},
	{QMI_FIXED_INTF(0x05c6, 0x7101, 2)},
	{QMI_FIXED_INTF(0x05c6, 0x7101, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x7102, 1)},
	{QMI_FIXED_INTF(0x05c6, 0x7102, 2)},
	{QMI_FIXED_INTF(0x05c6, 0x7102, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x8000, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x8001, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9000, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9003, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9005, 2)},
	{QMI_FIXED_INTF(0x05c6, 0x900a, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x900b, 2)},
	{QMI_FIXED_INTF(0x05c6, 0x900c, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x900c, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x900c, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x900d, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x900f, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x900f, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x900f, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9010, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9010, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9011, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9011, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9021, 1)},
	{QMI_FIXED_INTF(0x05c6, 0x9022, 2)},
	{QMI_FIXED_INTF(0x05c6, 0x9025, 4)},	/*                                                 */
	{QMI_FIXED_INTF(0x05c6, 0x9026, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x902e, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9031, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9032, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9033, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9033, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9033, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9033, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9034, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9034, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9034, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9034, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9034, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9035, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9036, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9037, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9038, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x903b, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x903c, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x903d, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x903e, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9043, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9046, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9046, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9046, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9047, 2)},
	{QMI_FIXED_INTF(0x05c6, 0x9047, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9047, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9048, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9048, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9048, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9048, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9048, 8)},
	{QMI_FIXED_INTF(0x05c6, 0x904c, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x904c, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x904c, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x904c, 8)},
	{QMI_FIXED_INTF(0x05c6, 0x9050, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9052, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9053, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9053, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9054, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9054, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9055, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9055, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9055, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9055, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9055, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9056, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9062, 2)},
	{QMI_FIXED_INTF(0x05c6, 0x9062, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9062, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9062, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9062, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9062, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9062, 8)},
	{QMI_FIXED_INTF(0x05c6, 0x9062, 9)},
	{QMI_FIXED_INTF(0x05c6, 0x9064, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9065, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9065, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9066, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9066, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9067, 1)},
	{QMI_FIXED_INTF(0x05c6, 0x9068, 2)},
	{QMI_FIXED_INTF(0x05c6, 0x9068, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9068, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9068, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9068, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9068, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9069, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9069, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9069, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9069, 8)},
	{QMI_FIXED_INTF(0x05c6, 0x9070, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9070, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9075, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9076, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9076, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9076, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9076, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9076, 8)},
	{QMI_FIXED_INTF(0x05c6, 0x9077, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9077, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9077, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9077, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9078, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9079, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x9079, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9079, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9079, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9079, 8)},
	{QMI_FIXED_INTF(0x05c6, 0x9080, 5)},
	{QMI_FIXED_INTF(0x05c6, 0x9080, 6)},
	{QMI_FIXED_INTF(0x05c6, 0x9080, 7)},
	{QMI_FIXED_INTF(0x05c6, 0x9080, 8)},
	{QMI_FIXED_INTF(0x05c6, 0x9083, 3)},
	{QMI_FIXED_INTF(0x05c6, 0x9084, 4)},
	{QMI_FIXED_INTF(0x05c6, 0x920d, 0)},
	{QMI_FIXED_INTF(0x05c6, 0x920d, 5)},
	{QMI_FIXED_INTF(0x0846, 0x68a2, 8)},
	{QMI_FIXED_INTF(0x12d1, 0x140c, 1)},	/*             */
	{QMI_FIXED_INTF(0x12d1, 0x14ac, 1)},	/*              */
	{QMI_FIXED_INTF(0x16d8, 0x6003, 0)},	/*              */
	{QMI_FIXED_INTF(0x16d8, 0x6007, 0)},	/*                  */
	{QMI_FIXED_INTF(0x16d8, 0x6008, 0)},	/*                 */
	{QMI_FIXED_INTF(0x16d8, 0x6280, 0)},	/*                 */
	{QMI_FIXED_INTF(0x16d8, 0x7001, 0)},	/*                  */
	{QMI_FIXED_INTF(0x16d8, 0x7002, 0)},	/*              */
	{QMI_FIXED_INTF(0x16d8, 0x7003, 4)},	/*                  */
	{QMI_FIXED_INTF(0x16d8, 0x7004, 3)},	/*              */
	{QMI_FIXED_INTF(0x16d8, 0x7006, 5)},	/*                 */
	{QMI_FIXED_INTF(0x16d8, 0x700a, 4)},	/*                  */
	{QMI_FIXED_INTF(0x16d8, 0x7211, 0)},	/*                  */
	{QMI_FIXED_INTF(0x16d8, 0x7212, 0)},	/*              */
	{QMI_FIXED_INTF(0x16d8, 0x7213, 0)},	/*              */
	{QMI_FIXED_INTF(0x16d8, 0x7251, 1)},	/*              */
	{QMI_FIXED_INTF(0x16d8, 0x7252, 1)},	/*              */
	{QMI_FIXED_INTF(0x16d8, 0x7253, 1)},	/*              */
	{QMI_FIXED_INTF(0x19d2, 0x0002, 1)},
	{QMI_FIXED_INTF(0x19d2, 0x0012, 1)},
	{QMI_FIXED_INTF(0x19d2, 0x0017, 3)},
	{QMI_FIXED_INTF(0x19d2, 0x0019, 3)},	/*              */
	{QMI_FIXED_INTF(0x19d2, 0x0021, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x0025, 1)},
	{QMI_FIXED_INTF(0x19d2, 0x0031, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x0042, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x0049, 5)},
	{QMI_FIXED_INTF(0x19d2, 0x0052, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x0055, 1)},	/*                        */
	{QMI_FIXED_INTF(0x19d2, 0x0058, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x0063, 4)},	/*                        */
	{QMI_FIXED_INTF(0x19d2, 0x0104, 4)},	/*                        */
	{QMI_FIXED_INTF(0x19d2, 0x0113, 5)},
	{QMI_FIXED_INTF(0x19d2, 0x0118, 5)},
	{QMI_FIXED_INTF(0x19d2, 0x0121, 5)},
	{QMI_FIXED_INTF(0x19d2, 0x0123, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x0124, 5)},
	{QMI_FIXED_INTF(0x19d2, 0x0125, 6)},
	{QMI_FIXED_INTF(0x19d2, 0x0126, 5)},
	{QMI_FIXED_INTF(0x19d2, 0x0130, 1)},
	{QMI_FIXED_INTF(0x19d2, 0x0133, 3)},
	{QMI_FIXED_INTF(0x19d2, 0x0141, 5)},
	{QMI_FIXED_INTF(0x19d2, 0x0157, 5)},	/*           */
	{QMI_FIXED_INTF(0x19d2, 0x0158, 3)},
	{QMI_FIXED_INTF(0x19d2, 0x0167, 4)},	/*            */
	{QMI_FIXED_INTF(0x19d2, 0x0168, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x0176, 3)},
	{QMI_FIXED_INTF(0x19d2, 0x0178, 3)},
	{QMI_FIXED_INTF(0x19d2, 0x0191, 4)},	/*             */
	{QMI_FIXED_INTF(0x19d2, 0x0199, 1)},	/*            */
	{QMI_FIXED_INTF(0x19d2, 0x0200, 1)},
	{QMI_FIXED_INTF(0x19d2, 0x0257, 3)},	/*           */
	{QMI_FIXED_INTF(0x19d2, 0x0265, 4)},	/*                    */
	{QMI_FIXED_INTF(0x19d2, 0x0284, 4)},	/*           */
	{QMI_FIXED_INTF(0x19d2, 0x0326, 4)},	/*            */
	{QMI_FIXED_INTF(0x19d2, 0x0412, 4)},	/*                    */
	{QMI_FIXED_INTF(0x19d2, 0x1008, 4)},	/*                        */
	{QMI_FIXED_INTF(0x19d2, 0x1010, 4)},	/*                        */
	{QMI_FIXED_INTF(0x19d2, 0x1012, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x1018, 3)},	/*                        */
	{QMI_FIXED_INTF(0x19d2, 0x1021, 2)},
	{QMI_FIXED_INTF(0x19d2, 0x1245, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x1247, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x1252, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x1254, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x1255, 3)},
	{QMI_FIXED_INTF(0x19d2, 0x1255, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x1256, 4)},
	{QMI_FIXED_INTF(0x19d2, 0x1270, 5)},	/*           */
	{QMI_FIXED_INTF(0x19d2, 0x1401, 2)},
	{QMI_FIXED_INTF(0x19d2, 0x1402, 2)},	/*          */
	{QMI_FIXED_INTF(0x19d2, 0x1424, 2)},
	{QMI_FIXED_INTF(0x19d2, 0x1425, 2)},
	{QMI_FIXED_INTF(0x19d2, 0x1426, 2)},	/*          */
	{QMI_FIXED_INTF(0x19d2, 0x1428, 2)},	/*                       */
	{QMI_FIXED_INTF(0x19d2, 0x2002, 4)},	/*                        */
	{QMI_FIXED_INTF(0x0f3d, 0x68a2, 8)},    /*                        */
	{QMI_FIXED_INTF(0x114f, 0x68a2, 8)},    /*                        */
	{QMI_FIXED_INTF(0x1199, 0x68a2, 8)},	/*                                    */
	{QMI_FIXED_INTF(0x1199, 0x68a2, 19)},	/*                                    */
	{QMI_FIXED_INTF(0x1199, 0x68c0, 8)},	/*                        */
	{QMI_FIXED_INTF(0x1199, 0x68c0, 10)},	/*                        */
	{QMI_FIXED_INTF(0x1199, 0x68c0, 11)},	/*                        */
	{QMI_FIXED_INTF(0x1199, 0x901c, 8)},    /*                        */
	{QMI_FIXED_INTF(0x1199, 0x901f, 8)},    /*                        */
	{QMI_FIXED_INTF(0x1199, 0x9041, 8)},	/*                               */
	{QMI_FIXED_INTF(0x1199, 0x9051, 8)},	/*                      */
	{QMI_FIXED_INTF(0x1199, 0x9057, 8)},
	{QMI_FIXED_INTF(0x1bbb, 0x011e, 4)},	/*                                                         */
	{QMI_FIXED_INTF(0x1bbb, 0x0203, 2)},	/*                */
	{QMI_FIXED_INTF(0x2357, 0x0201, 4)},	/*                           */
	{QMI_FIXED_INTF(0x2357, 0x9000, 4)},	/*               */
	{QMI_FIXED_INTF(0x1bc7, 0x1200, 5)},	/*             */
	{QMI_FIXED_INTF(0x1bc7, 0x1201, 2)},	/*             */
	{QMI_FIXED_INTF(0x0b3c, 0xc000, 4)},	/*                      */
	{QMI_FIXED_INTF(0x0b3c, 0xc001, 4)},	/*                      */
	{QMI_FIXED_INTF(0x0b3c, 0xc002, 4)},	/*                      */
	{QMI_FIXED_INTF(0x0b3c, 0xc004, 6)},	/*                      */
	{QMI_FIXED_INTF(0x0b3c, 0xc005, 6)},	/*                      */
	{QMI_FIXED_INTF(0x0b3c, 0xc00a, 6)},	/*                      */
	{QMI_FIXED_INTF(0x0b3c, 0xc00b, 4)},	/*                      */
	{QMI_FIXED_INTF(0x1e2d, 0x0060, 4)},	/*                */
	{QMI_FIXED_INTF(0x1e2d, 0x0053, 4)},	/*                     */
	{QMI_FIXED_INTF(0x413c, 0x81a2, 8)},	/*                                                          */
	{QMI_FIXED_INTF(0x413c, 0x81a3, 8)},	/*                                                         */
	{QMI_FIXED_INTF(0x413c, 0x81a4, 8)},	/*                                                          */
	{QMI_FIXED_INTF(0x413c, 0x81a8, 8)},	/*                                                          */
	{QMI_FIXED_INTF(0x413c, 0x81a9, 8)},	/*                                                           */
	{QMI_FIXED_INTF(0x03f0, 0x581d, 4)},	/* HP lt4112 LTE/HSPA+ Gobi 4G Module (Huawei me906e) */

	/*                      */
	{QMI_GOBI1K_DEVICE(0x05c6, 0x9212)},	/*                        */
	{QMI_GOBI1K_DEVICE(0x03f0, 0x1f1d)},	/*                             */
	{QMI_GOBI1K_DEVICE(0x04da, 0x250d)},	/*                             */
	{QMI_GOBI1K_DEVICE(0x413c, 0x8172)},	/*                        */
	{QMI_GOBI1K_DEVICE(0x1410, 0xa001)},	/*                          */
	{QMI_GOBI1K_DEVICE(0x1410, 0xa002)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x1410, 0xa003)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x1410, 0xa004)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x1410, 0xa005)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x1410, 0xa006)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x1410, 0xa007)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x0b05, 0x1776)},	/*                        */
	{QMI_GOBI1K_DEVICE(0x19d2, 0xfff3)},	/*                        */
	{QMI_GOBI1K_DEVICE(0x05c6, 0x9001)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x05c6, 0x9002)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x05c6, 0x9202)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x05c6, 0x9203)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x05c6, 0x9222)},	/*                           */
	{QMI_GOBI1K_DEVICE(0x05c6, 0x9009)},	/*                           */

	/*                               */
	{QMI_GOBI_DEVICE(0x413c, 0x8186)},	/*                                            */
	{QMI_GOBI_DEVICE(0x413c, 0x8194)},	/*                          */
	{QMI_GOBI_DEVICE(0x05c6, 0x920b)},	/*                                */
	{QMI_GOBI_DEVICE(0x05c6, 0x9225)},	/*                                            */
	{QMI_GOBI_DEVICE(0x05c6, 0x9245)},	/*                                        */
	{QMI_GOBI_DEVICE(0x03f0, 0x251d)},	/*                                   */
	{QMI_GOBI_DEVICE(0x05c6, 0x9215)},	/*                                     */
	{QMI_GOBI_DEVICE(0x05c6, 0x9265)},	/*                                     */
	{QMI_GOBI_DEVICE(0x05c6, 0x9235)},	/*                                           */
	{QMI_GOBI_DEVICE(0x05c6, 0x9275)},	/*                                                  */
	{QMI_GOBI_DEVICE(0x0af0, 0x8120)},	/*                */
	{QMI_GOBI_DEVICE(0x1199, 0x68a5)},	/*                       */
	{QMI_GOBI_DEVICE(0x1199, 0x68a9)},	/*                       */
	{QMI_GOBI_DEVICE(0x1199, 0x9001)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x9002)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x9003)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x9004)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x9005)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x9006)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x9007)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x9008)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x9009)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x900a)},	/*                                                */
	{QMI_GOBI_DEVICE(0x1199, 0x9011)},	/*                                                 */
	{QMI_GOBI_DEVICE(0x16d8, 0x8002)},	/*                                        */
	{QMI_GOBI_DEVICE(0x05c6, 0x9205)},	/*                        */
	{QMI_GOBI_DEVICE(0x1199, 0x9013)},	/*                                                 */
	{QMI_GOBI_DEVICE(0x03f0, 0x371d)},	/*                                   */
	{QMI_GOBI_DEVICE(0x1199, 0x9015)},	/*                                        */
	{QMI_GOBI_DEVICE(0x1199, 0x9019)},	/*                                        */
	{QMI_GOBI_DEVICE(0x1199, 0x901b)},	/*                        */
	{QMI_GOBI_DEVICE(0x12d1, 0x14f1)},	/*                          */
	{QMI_GOBI_DEVICE(0x1410, 0xa021)},	/*                                               */

	{ }					/*     */
};
MODULE_DEVICE_TABLE(usb, products);

static int qmi_wwan_probe(struct usb_interface *intf, const struct usb_device_id *prod)
{
	struct usb_device_id *id = (struct usb_device_id *)prod;

	/*                                                        
                                                           
                                                         
                                              
  */
	if (!id->driver_info) {
		dev_dbg(&intf->dev, "setting defaults for dynamic device id\n");
		id->driver_info = (unsigned long)&qmi_wwan_info;
	}

	return usbnet_probe(intf, id);
}

static struct usb_driver qmi_wwan_driver = {
	.name		      = "qmi_wwan",
	.id_table	      = products,
	.probe		      = qmi_wwan_probe,
	.disconnect	      = usbnet_disconnect,
	.suspend	      = qmi_wwan_suspend,
	.resume		      =	qmi_wwan_resume,
	.reset_resume         = qmi_wwan_resume,
	.supports_autosuspend = 1,
	.disable_hub_initiated_lpm = 1,
};

module_usb_driver(qmi_wwan_driver);

MODULE_AUTHOR("Bjørn Mork <bjorn@mork.no>");
MODULE_DESCRIPTION("Qualcomm MSM Interface (QMI) WWAN driver");
MODULE_LICENSE("GPL");
