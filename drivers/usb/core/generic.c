/*
 * drivers/usb/generic.c - generic driver for USB devices (not interfaces)
 *
 * (C) Copyright 2005 Greg Kroah-Hartman <gregkh@suse.de>
 *
 * based on drivers/usb/usb.c which had the following copyrights:
 *	(C) Copyright Linus Torvalds 1999
 *	(C) Copyright Johannes Erdfelt 1999-2001
 *	(C) Copyright Andreas Gal 1999
 *	(C) Copyright Gregory P. Smith 1999
 *	(C) Copyright Deti Fliegl 1999 (new USB architecture)
 *	(C) Copyright Randy Dunlap 2000
 *	(C) Copyright David Brownell 2000-2004
 *	(C) Copyright Yggdrasil Computing, Inc. 2000
 *		(usb_device_id matching changes by Adam J. Richter)
 *	(C) Copyright Greg Kroah-Hartman 2002-2003
 *
 */

#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include "usb.h"

#if defined(CONFIG_USBIF_COMPLIANCE) && defined(CONFIG_USB_XHCI_HCD)
extern int usbif_u3h_send_event(char* event) ;
#endif

static inline const char *plural(int n)
{
	return (n == 1 ? "" : "s");
}

static int is_rndis(struct usb_interface_descriptor *desc)
{
	return desc->bInterfaceClass == USB_CLASS_COMM
		&& desc->bInterfaceSubClass == 2
		&& desc->bInterfaceProtocol == 0xff;
}

static int is_activesync(struct usb_interface_descriptor *desc)
{
	return desc->bInterfaceClass == USB_CLASS_MISC
		&& desc->bInterfaceSubClass == 1
		&& desc->bInterfaceProtocol == 1;
}

int usb_choose_configuration(struct usb_device *udev)
{
	int i;
	int num_configs;
	int insufficient_power = 0;
	struct usb_host_config *c, *best;

	if (usb_device_is_owned(udev))
		return 0;

	best = NULL;
	c = udev->config;
	num_configs = udev->descriptor.bNumConfigurations;
	for (i = 0; i < num_configs; (i++, c++)) {
		struct usb_interface_descriptor	*desc = NULL;

		/*                                                */
		if (c->desc.bNumInterfaces > 0)
			desc = &c->intf_cache[0]->altsetting->desc;

		/*
                                                             
                                                             
                                                           
                                                              
                                                            
                                                           
                                                           
                                                              
    
                                                          
                                                            
                                                                 
                                                               
                               
    
                                                            
                                                              
                            
   */
#if 0
		/*                                                        */
		if (bus_powered && (c->desc.bmAttributes &
					USB_CONFIG_ATT_SELFPOWER))
			continue;
#endif

		/*
                                                           
                                                        
                                                         
                                                         
                                  
    
                                                       
                                                          
              
   */

		/*                                                 */
		if (usb_get_max_power(udev, c) > udev->bus_mA) {
			insufficient_power++;
			MYDBG("insufficient available bus power to %d intf, the intf class is 0x%x \n", desc->bInterfaceNumber , desc->bInterfaceClass);
			continue;
		}

		/*                                                              
                                                                  
                                                            
                                                  
   */
		if (i == 0 && num_configs > 1 && desc &&
				(is_rndis(desc) || is_activesync(desc))) {
#if !defined(CONFIG_USB_NET_RNDIS_HOST) && !defined(CONFIG_USB_NET_RNDIS_HOST_MODULE)
			continue;
#else
			best = c;
#endif
		}

		/*                                                       
                                                        
                                                        
                                    */
		else if (udev->descriptor.bDeviceClass !=
						USB_CLASS_VENDOR_SPEC &&
				(desc && desc->bInterfaceClass !=
						USB_CLASS_VENDOR_SPEC)) {
			best = c;
			break;
		}

		/*                                                  
                           */
		else if (!best)
			best = c;
	}

	if (insufficient_power > 0)
		dev_info(&udev->dev, "rejected %d configuration%s "
			"due to insufficient available bus power\n",
			insufficient_power, plural(insufficient_power));

	if (best) {
		i = best->desc.bConfigurationValue;
		dev_dbg(&udev->dev,
			"configuration #%d chosen from %d choice%s\n",
			i, num_configs, plural(num_configs));
	} else {
		i = -1;
		dev_warn(&udev->dev,
			"no configuration chosen from %d choice%s\n",
			num_configs, plural(num_configs));
#if defined(CONFIG_USBIF_COMPLIANCE) && defined(CONFIG_USB_XHCI_HCD)
		if ((insufficient_power>0) && (insufficient_power==num_configs)){
			MYDBG("insufficient available bus power to all intf\n");
			usbif_u3h_send_event("DEV_OVER_CURRENT");
		}
#endif
	}
	return i;
}

static int generic_probe(struct usb_device *udev)
{
	int err, c;

	/*                                                                 
                                                                 
  */
	if (udev->authorized == 0)
		dev_err(&udev->dev, "Device is not authorized for usage\n");
	else {
		c = usb_choose_configuration(udev);
		if (c >= 0) {
			err = usb_set_configuration(udev, c);
			if (err && err != -ENODEV) {
				dev_err(&udev->dev, "can't set config #%d, error %d\n",
					c, err);
				/*                                             
                                 */
			}
		}
	}
	/*                                           */
	usb_notify_add_device(udev);

	return 0;
}

static void generic_disconnect(struct usb_device *udev)
{
	usb_notify_remove_device(udev);

	/*                                                           
                           */
	if (udev->actconfig)
		usb_set_configuration(udev, -1);
}

#ifdef	CONFIG_PM

static int generic_suspend(struct usb_device *udev, pm_message_t msg)
{
	int rc;

  	MYDBG("udev : %lu", (unsigned long)udev);

	/*                                                        
                                                   
                                                      
                                                             
  */
	if (!udev->parent)
	{
		MYDBG("");
		rc = hcd_bus_suspend(udev, msg);
	}

	/*                                                                  */
	else if (msg.event == PM_EVENT_FREEZE || msg.event == PM_EVENT_PRETHAW)
	{
		MYDBG("");
		rc = 0;
	}
	else
	{
		MYDBG("");
		rc = usb_port_suspend(udev, msg);
	}

	return rc;
}

static int generic_resume(struct usb_device *udev, pm_message_t msg)
{
	int rc;
	MYDBG("udev : %lu", (unsigned long)udev);

	/*                                                             
                                                           
                                                     
                                                            
  */
	if (!udev->parent)
	{
		MYDBG("udev : %lu", (unsigned long)udev);
		rc = hcd_bus_resume(udev, msg);
	}
	else
	{
		MYDBG("udev : %lu", (unsigned long)udev);
		rc = usb_port_resume(udev, msg);
	}
	return rc;
}

#endif	/*           */

struct usb_device_driver usb_generic_driver = {
	.name =	"usb",
	.probe = generic_probe,
	.disconnect = generic_disconnect,
#ifdef	CONFIG_PM
	.suspend = generic_suspend,
	.resume = generic_resume,
#endif
	.supports_autosuspend = 1,
};
