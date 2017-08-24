/*
 * drivers/usb/core/otg_whitelist.h
 *
 * Copyright (C) 2004 Texas Instruments
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
                                                                       
                                                           
  
                                                                       
 */

static struct usb_device_id whitelist_table [] = {

/*                                              */
{ USB_DEVICE_INFO(USB_CLASS_HUB, 0, 0), },
{ USB_DEVICE_INFO(USB_CLASS_HUB, 0, 1), },
#ifdef CONFIG_USBIF_COMPLIANCE
{ USB_DEVICE_INFO(USB_CLASS_MASS_STORAGE, 0, 0), },
{ USB_DEVICE_INFO(USB_CLASS_HID, 0, 0), },
{ USB_DEVICE_INFO(0, 0, 0), },
#endif

#ifdef	CONFIG_USB_PRINTER		/*                             */
/*                                                                 
                                               
 */
{ USB_DEVICE_INFO(7, 1, 1) },
{ USB_DEVICE_INFO(7, 1, 2) },
{ USB_DEVICE_INFO(7, 1, 3) },
#endif

#ifdef	CONFIG_USB_NET_CDCETHER
/*                               */
{ USB_DEVICE(0x0525, 0xa4a1), },
/*                                       */
{ USB_DEVICE(0x0525, 0xa4a2), },
#endif

#if	defined(CONFIG_USB_TEST) || defined(CONFIG_USB_TEST_MODULE)
/*                          */
{ USB_DEVICE(0x0525, 0xa4a0), },
#endif

{ }	/*                   */
};

static int is_targeted(struct usb_device *dev)
{
	struct usb_device_id	*id = whitelist_table;

	/*                                     */
#ifndef CONFIG_USBIF_COMPLIANCE		
	if (!dev->bus->otg_port)
		return 1;
#endif	

	/*                                                          */
	if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
	     le16_to_cpu(dev->descriptor.idProduct) == 0xbadd))
		return 0;

#ifdef CONFIG_USBIF_COMPLIANCE		
    /*
                          
    */

    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0200))
            return 1;

    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0201))
            return 0;

    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1a0a &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0202))
            return 0;	
    /*
             
    */

    /*            */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x17EF &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x6019))
            return 1;

    /*                             */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x045E &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0084))
            return 1;

    /*                   */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x046D &&
        le16_to_cpu(dev->descriptor.idProduct) == 0xC05A))
            return 1;

    /*                        */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x05A4 &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x9862))
            return 1;


    /*
                       
    */
    /*                     */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x0951 &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x1643))
            return 1;
    
    /*                                     */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x0951 &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x1665))
            return 1;

    /*                          */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x0781 &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x5571))
            return 1;

    /*               */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x13FE &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x3100))
            return 1;

    /*             */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x090C &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x1000))
            return 1;

    /*            */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x0D7D &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0100))
            return 1;

    /*                                   */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x1908 &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0225))
            return 1;

    /*                                     */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x0bb4 &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0001))
            return 1;

    /*               */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x08EC &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0008))
            return 1;

    /*               */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x0781 &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x5406))
            return 1;            

    /*
              
    */    
    /*                */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x03EB &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0902))
            return 1;

    /*                */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x058F &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x9254))
            return 1;

    /*                    */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x0409 &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0059))
            return 1;

    /*                              */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x050d &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x0233))
            return 1;

    /*                               */
    if ((le16_to_cpu(dev->descriptor.idVendor) == 0x05A4 &&
        le16_to_cpu(dev->descriptor.idProduct) == 0x9837))
            return 1;

	/*               */
	if ((le16_to_cpu(dev->descriptor.idVendor) == 0x14CD &&
		le16_to_cpu(dev->descriptor.idProduct) == 0x8601))
			return 1;


#else
	/*                                                      
                                                        
  */
	for (id = whitelist_table; id->match_flags; id++) {
		if ((id->match_flags & USB_DEVICE_ID_MATCH_VENDOR) &&
		    id->idVendor != le16_to_cpu(dev->descriptor.idVendor))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_PRODUCT) &&
		    id->idProduct != le16_to_cpu(dev->descriptor.idProduct))
			continue;

		/*                                                        
                                       */
		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_LO) &&
		    (id->bcdDevice_lo > le16_to_cpu(dev->descriptor.bcdDevice)))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_HI) &&
		    (id->bcdDevice_hi < le16_to_cpu(dev->descriptor.bcdDevice)))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_CLASS) &&
		    (id->bDeviceClass != dev->descriptor.bDeviceClass))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_SUBCLASS) &&
		    (id->bDeviceSubClass != dev->descriptor.bDeviceSubClass))
			continue;

		if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_PROTOCOL) &&
		    (id->bDeviceProtocol != dev->descriptor.bDeviceProtocol))
			continue;

		return 1;
	}
#endif
	/*                                   */


	/*                                                                  */
	dev_err(&dev->dev, "device v%04x p%04x is not supported\n",
		le16_to_cpu(dev->descriptor.idVendor),
		le16_to_cpu(dev->descriptor.idProduct));
#ifdef	CONFIG_USB_OTG_WHITELIST
	return 0;
#else
	return 1;
#endif
}

