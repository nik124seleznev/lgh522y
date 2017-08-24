/*
 * usb/gadget/config.c -- simplify building config descriptors
 *
 * Copyright (C) 2003 David Brownell
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/device.h>

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/composite.h>

/* 
                                                        
                            
                       
                                                                  
  
                                                                
                                                                
                                                                  
                                                                     
                                            
 */
int
usb_descriptor_fillbuf(void *buf, unsigned buflen,
		const struct usb_descriptor_header **src)
{
	u8	*dest = buf;

	if (!src)
		return -EINVAL;

	/*                                                  */
	for (; NULL != *src; src++) {
		unsigned		len = (*src)->bLength;

		if (len > buflen)
			return -EINVAL;
		memcpy(dest, *src, len);
		buflen -= len;
		dest += len;
	}
	return dest - (u8 *)buf;
}
EXPORT_SYMBOL_GPL(usb_descriptor_fillbuf);

/* 
                                                                     
                                                                     
                                                  
                                                                           
                                                                      
                                                           
                                                                    
                                                                   
  
                                                                          
                                                                      
                                                                         
                                                                        
                                              
  
                                                                      
                                                                      
                                                                          
 */
int usb_gadget_config_buf(
	const struct usb_config_descriptor	*config,
	void					*buf,
	unsigned				length,
	const struct usb_descriptor_header	**desc
)
{
	struct usb_config_descriptor		*cp = buf;
	int					len;

	/*                         */
	if (length < USB_DT_CONFIG_SIZE || !desc)
		return -EINVAL;
	*cp = *config;

	/*                                          */
	len = usb_descriptor_fillbuf(USB_DT_CONFIG_SIZE + (u8*)buf,
			length - USB_DT_CONFIG_SIZE, desc);
	if (len < 0)
		return len;
	len += USB_DT_CONFIG_SIZE;
	if (len > 0xffff)
		return -EINVAL;

	/*                                */
	cp->bLength = USB_DT_CONFIG_SIZE;
	cp->bDescriptorType = USB_DT_CONFIG;
	cp->wTotalLength = cpu_to_le16(len);
	cp->bmAttributes |= USB_CONFIG_ATT_ONE;
	return len;
}
EXPORT_SYMBOL_GPL(usb_gadget_config_buf);

/* 
                                                          
                                       
                                                
  
                                                                     
                                                                     
                                                                 
                                                                    
                                                                  
                                          
 */
struct usb_descriptor_header **
usb_copy_descriptors(struct usb_descriptor_header **src)
{
	struct usb_descriptor_header **tmp;
	unsigned bytes;
	unsigned n_desc;
	void *mem;
	struct usb_descriptor_header **ret;

	/*                                                         */
	for (bytes = 0, n_desc = 0, tmp = src; *tmp; tmp++, n_desc++)
		bytes += (*tmp)->bLength;
	bytes += (n_desc + 1) * sizeof(*tmp);

	mem = kmalloc(bytes, GFP_KERNEL);
	if (!mem)
		return NULL;

	/*                                    
                                            
                    
  */
	tmp = mem;
	ret = mem;
	mem += (n_desc + 1) * sizeof(*tmp);
	while (*src) {
		memcpy(mem, *src, (*src)->bLength);
		*tmp = mem;
		tmp++;
		mem += (*src)->bLength;
		src++;
	}
	*tmp = NULL;

	return ret;
}
EXPORT_SYMBOL_GPL(usb_copy_descriptors);

int usb_assign_descriptors(struct usb_function *f,
		struct usb_descriptor_header **fs,
		struct usb_descriptor_header **hs,
		struct usb_descriptor_header **ss)
{
	struct usb_gadget *g = f->config->cdev->gadget;

	if (fs) {
		f->fs_descriptors = usb_copy_descriptors(fs);
		if (!f->fs_descriptors)
			goto err;
	}
	if (hs && gadget_is_dualspeed(g)) {
		f->hs_descriptors = usb_copy_descriptors(hs);
		if (!f->hs_descriptors)
			goto err;
	}
	if (ss && gadget_is_superspeed(g)) {
		f->ss_descriptors = usb_copy_descriptors(ss);
		if (!f->ss_descriptors)
			goto err;
	}
	return 0;
err:
	usb_free_all_descriptors(f);
	return -ENOMEM;
}
EXPORT_SYMBOL_GPL(usb_assign_descriptors);

void usb_free_all_descriptors(struct usb_function *f)
{
	usb_free_descriptors(f->fs_descriptors);
	usb_free_descriptors(f->hs_descriptors);
	usb_free_descriptors(f->ss_descriptors);
}
EXPORT_SYMBOL_GPL(usb_free_all_descriptors);
