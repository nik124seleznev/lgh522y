/*
 * ncm.c -- NCM gadget driver
 *
 * Copyright (C) 2010 Nokia Corporation
 * Contact: Yauheni Kaliuta <yauheni.kaliuta@nokia.com>
 *
 * The driver borrows from ether.c which is:
 *
 * Copyright (C) 2003-2005,2008 David Brownell
 * Copyright (C) 2003-2004 Robert Schwebel, Benedikt Spranger
 * Copyright (C) 2008 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*               */
/*                       */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb/composite.h>

#include "u_ether.h"

#define DRIVER_DESC		"NCM Gadget"

/*                                                                         */

/*
                                                                    
                                                                     
                                                                      
                                                                   
                                                                  
 */
#include "f_ncm.c"
#include "u_ether.c"

/*                                                                         */

/*                                                                     
                                                               
 */

/*                                                             
                                                          
 */
#define CDC_VENDOR_NUM		0x0525	/*         */
#define CDC_PRODUCT_NUM		0xa4a1	/*                           */

/*                                                                         */
USB_GADGET_COMPOSITE_OPTIONS();

static struct usb_device_descriptor device_desc = {
	.bLength =		sizeof device_desc,
	.bDescriptorType =	USB_DT_DEVICE,

	.bcdUSB =		cpu_to_le16 (0x0200),

	.bDeviceClass =		USB_CLASS_COMM,
	.bDeviceSubClass =	0,
	.bDeviceProtocol =	0,
	/*                                */

	/*                                                                
                                                                
                                            
  */
	.idVendor =		cpu_to_le16 (CDC_VENDOR_NUM),
	.idProduct =		cpu_to_le16 (CDC_PRODUCT_NUM),
	/*                          */
	/*                          */
	/*                     */
	/*                  */
	.bNumConfigurations =	1,
};

static struct usb_otg_descriptor otg_descriptor = {
	.bLength =		sizeof otg_descriptor,
	.bDescriptorType =	USB_DT_OTG,

	/*                                                
                                    
  */
	.bmAttributes =		USB_OTG_SRP | USB_OTG_HNP,
};

static const struct usb_descriptor_header *otg_desc[] = {
	(struct usb_descriptor_header *) &otg_descriptor,
	NULL,
};

/*                                     */
static struct usb_string strings_dev[] = {
	[USB_GADGET_MANUFACTURER_IDX].s = "",
	[USB_GADGET_PRODUCT_IDX].s = DRIVER_DESC,
	[USB_GADGET_SERIAL_IDX].s = "",
	{  } /*             */
};

static struct usb_gadget_strings stringtab_dev = {
	.language	= 0x0409,	/*       */
	.strings	= strings_dev,
};

static struct usb_gadget_strings *dev_strings[] = {
	&stringtab_dev,
	NULL,
};

struct eth_dev *the_dev;
static u8 hostaddr[ETH_ALEN];

/*                                                                         */

static int __init ncm_do_config(struct usb_configuration *c)
{
	/*                                                         */

	if (gadget_is_otg(c->cdev->gadget)) {
		c->descriptors = otg_desc;
		c->bmAttributes |= USB_CONFIG_ATT_WAKEUP;
	}

	return ncm_bind_config(c, hostaddr, the_dev);
}

static struct usb_configuration ncm_config_driver = {
	/*                      */
	.label			= "CDC Ethernet (NCM)",
	.bConfigurationValue	= 1,
	/*                           */
	.bmAttributes		= USB_CONFIG_ATT_SELFPOWER,
};

/*                                                                         */

static int __init gncm_bind(struct usb_composite_dev *cdev)
{
	struct usb_gadget	*gadget = cdev->gadget;
	int			status;

	/*                           */
	the_dev = gether_setup(cdev->gadget, hostaddr);
	if (IS_ERR(the_dev))
		return PTR_ERR(the_dev);

	/*                                                        
                                                         
  */

	status = usb_string_ids_tab(cdev, strings_dev);
	if (status < 0)
		goto fail;
	device_desc.iManufacturer = strings_dev[USB_GADGET_MANUFACTURER_IDX].id;
	device_desc.iProduct = strings_dev[USB_GADGET_PRODUCT_IDX].id;

	status = usb_add_config(cdev, &ncm_config_driver,
				ncm_do_config);
	if (status < 0)
		goto fail;

	usb_composite_overwrite_options(cdev, &coverwrite);
	dev_info(&gadget->dev, "%s\n", DRIVER_DESC);

	return 0;

fail:
	gether_cleanup(the_dev);
	return status;
}

static int __exit gncm_unbind(struct usb_composite_dev *cdev)
{
	gether_cleanup(the_dev);
	return 0;
}

static __refdata struct usb_composite_driver ncm_driver = {
	.name		= "g_ncm",
	.dev		= &device_desc,
	.strings	= dev_strings,
	.max_speed	= USB_SPEED_HIGH,
	.bind		= gncm_bind,
	.unbind		= __exit_p(gncm_unbind),
};

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("Yauheni Kaliuta");
MODULE_LICENSE("GPL");

static int __init init(void)
{
	return usb_composite_probe(&ncm_driver);
}
module_init(init);

static void __exit cleanup(void)
{
	usb_composite_unregister(&ncm_driver);
}
module_exit(cleanup);
