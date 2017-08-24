/*
 *  HID driver for UC-Logic devices not fully compliant with HID standard
 *
 *  Copyright (c) 2010 Nikolai Kondrashov
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <linux/usb.h>

#include "hid-ids.h"

/*
                                                                       
                                                                       
                                                                       
                                                                       
 */

/*                                                    */
#define WPXXXXU_RDESC_ORIG_SIZE	212

/*                                 */
static __u8 wp4030u_rdesc_fixed[] = {
	0x05, 0x0D,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x09,         /*                                      */
	0x09, 0x20,         /*                                      */
	0xA0,               /*                                      */
	0x75, 0x01,         /*                                      */
	0x09, 0x42,         /*                                      */
	0x09, 0x44,         /*                                      */
	0x09, 0x46,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x05,         /*                                      */
	0x81, 0x01,         /*                                      */
	0x75, 0x10,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x14,               /*                                      */
	0xA4,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x55, 0xFD,         /*                                      */
	0x65, 0x13,         /*                                      */
	0x34,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x46, 0xA0, 0x0F,   /*                                      */
	0x26, 0xFF, 0x7F,   /*                                      */
	0x81, 0x02,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x46, 0xB8, 0x0B,   /*                                      */
	0x26, 0xFF, 0x7F,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xB4,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x26, 0xFF, 0x03,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xC0,               /*                                      */
	0xC0                /*                                      */
};

/*                                 */
static __u8 wp5540u_rdesc_fixed[] = {
	0x05, 0x0D,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x09,         /*                                      */
	0x09, 0x20,         /*                                      */
	0xA0,               /*                                      */
	0x75, 0x01,         /*                                      */
	0x09, 0x42,         /*                                      */
	0x09, 0x44,         /*                                      */
	0x09, 0x46,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x05,         /*                                      */
	0x81, 0x01,         /*                                      */
	0x75, 0x10,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x14,               /*                                      */
	0xA4,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x55, 0xFD,         /*                                      */
	0x65, 0x13,         /*                                      */
	0x34,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x46, 0x7C, 0x15,   /*                                      */
	0x26, 0xFF, 0x7F,   /*                                      */
	0x81, 0x02,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x46, 0xA0, 0x0F,   /*                                      */
	0x26, 0xFF, 0x7F,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xB4,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x26, 0xFF, 0x03,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xC0,               /*                                      */
	0xC0,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x08,         /*                                      */
	0x09, 0x01,         /*                                      */
	0xA0,               /*                                      */
	0x75, 0x01,         /*                                      */
	0x05, 0x09,         /*                                      */
	0x19, 0x01,         /*                                      */
	0x29, 0x03,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x05,         /*                                      */
	0x81, 0x01,         /*                                      */
	0x05, 0x01,         /*                                      */
	0x75, 0x08,         /*                                      */
	0x09, 0x30,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x15, 0x81,         /*                                      */
	0x25, 0x7F,         /*                                      */
	0x95, 0x02,         /*                                      */
	0x81, 0x06,         /*                                      */
	0x09, 0x38,         /*                                      */
	0x15, 0xFF,         /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x81, 0x06,         /*                                      */
	0x81, 0x01,         /*                                      */
	0xC0,               /*                                      */
	0xC0                /*                                      */
};

/*                                 */
static __u8 wp8060u_rdesc_fixed[] = {
	0x05, 0x0D,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x09,         /*                                      */
	0x09, 0x20,         /*                                      */
	0xA0,               /*                                      */
	0x75, 0x01,         /*                                      */
	0x09, 0x42,         /*                                      */
	0x09, 0x44,         /*                                      */
	0x09, 0x46,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x05,         /*                                      */
	0x81, 0x01,         /*                                      */
	0x75, 0x10,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x14,               /*                                      */
	0xA4,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x55, 0xFD,         /*                                      */
	0x65, 0x13,         /*                                      */
	0x34,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x46, 0x40, 0x1F,   /*                                      */
	0x26, 0xFF, 0x7F,   /*                                      */
	0x81, 0x02,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x46, 0x70, 0x17,   /*                                      */
	0x26, 0xFF, 0x7F,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xB4,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x26, 0xFF, 0x03,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xC0,               /*                                      */
	0xC0,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x08,         /*                                      */
	0x09, 0x01,         /*                                      */
	0xA0,               /*                                      */
	0x75, 0x01,         /*                                      */
	0x05, 0x09,         /*                                      */
	0x19, 0x01,         /*                                      */
	0x29, 0x03,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x05,         /*                                      */
	0x81, 0x01,         /*                                      */
	0x05, 0x01,         /*                                      */
	0x75, 0x08,         /*                                      */
	0x09, 0x30,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x15, 0x81,         /*                                      */
	0x25, 0x7F,         /*                                      */
	0x95, 0x02,         /*                                      */
	0x81, 0x06,         /*                                      */
	0x09, 0x38,         /*                                      */
	0x15, 0xFF,         /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x81, 0x06,         /*                                      */
	0x81, 0x01,         /*                                      */
	0xC0,               /*                                      */
	0xC0                /*                                      */
};

/*
                                                               
                                                                      
 */

/*                                                  */
#define WP1062_RDESC_ORIG_SIZE	254

/*                                */
static __u8 wp1062_rdesc_fixed[] = {
	0x05, 0x0D,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x09,         /*                                      */
	0x09, 0x20,         /*                                      */
	0xA0,               /*                                      */
	0x75, 0x01,         /*                                      */
	0x09, 0x42,         /*                                      */
	0x09, 0x44,         /*                                      */
	0x09, 0x46,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x04,         /*                                      */
	0x81, 0x01,         /*                                      */
	0x09, 0x32,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x75, 0x10,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x14,               /*                                      */
	0xA4,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x55, 0xFD,         /*                                      */
	0x65, 0x13,         /*                                      */
	0x34,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x46, 0x10, 0x27,   /*                                      */
	0x26, 0x20, 0x4E,   /*                                      */
	0x81, 0x02,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x46, 0xB7, 0x19,   /*                                      */
	0x26, 0x6E, 0x33,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xB4,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x26, 0xFF, 0x03,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xC0,               /*                                      */
	0xC0                /*                                      */
};

/*
                                                               
                                                                      
 */

/*                                                  */
#define PF1209_RDESC_ORIG_SIZE	234

/*                                */
static __u8 pf1209_rdesc_fixed[] = {
	0x05, 0x0D,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x09,         /*                                      */
	0x09, 0x20,         /*                                      */
	0xA0,               /*                                      */
	0x75, 0x01,         /*                                      */
	0x09, 0x42,         /*                                      */
	0x09, 0x44,         /*                                      */
	0x09, 0x46,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x05,         /*                                      */
	0x81, 0x01,         /*                                      */
	0x75, 0x10,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x14,               /*                                      */
	0xA4,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x55, 0xFD,         /*                                      */
	0x65, 0x13,         /*                                      */
	0x34,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x46, 0xE0, 0x2E,   /*                                      */
	0x26, 0xFF, 0x7F,   /*                                      */
	0x81, 0x02,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x46, 0x28, 0x23,   /*                                      */
	0x26, 0xFF, 0x7F,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xB4,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x26, 0xFF, 0x03,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xC0,               /*                                      */
	0xC0,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x08,         /*                                      */
	0x09, 0x01,         /*                                      */
	0xA0,               /*                                      */
	0x75, 0x01,         /*                                      */
	0x05, 0x09,         /*                                      */
	0x19, 0x01,         /*                                      */
	0x29, 0x03,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x05,         /*                                      */
	0x81, 0x01,         /*                                      */
	0x05, 0x01,         /*                                      */
	0x75, 0x08,         /*                                      */
	0x09, 0x30,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x15, 0x81,         /*                                      */
	0x25, 0x7F,         /*                                      */
	0x95, 0x02,         /*                                      */
	0x81, 0x06,         /*                                      */
	0x09, 0x38,         /*                                      */
	0x15, 0xFF,         /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x81, 0x06,         /*                                      */
	0x81, 0x01,         /*                                      */
	0xC0,               /*                                      */
	0xC0                /*                                      */
};

/*
                                                                
                                                                                
 */

/*                                                    */
#define TWHL850_RDESC_ORIG_SIZE0	182
#define TWHL850_RDESC_ORIG_SIZE1	161
#define TWHL850_RDESC_ORIG_SIZE2	92

/*                                                               */
static __u8 twhl850_rdesc_fixed0[] = {
	0x05, 0x0D,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x09,         /*                                      */
	0x09, 0x20,         /*                                      */
	0xA0,               /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x75, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x09, 0x42,         /*                                      */
	0x09, 0x44,         /*                                      */
	0x09, 0x46,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x81, 0x03,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x09, 0x32,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x81, 0x03,         /*                                      */
	0x75, 0x10,         /*                                      */
	0xA4,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x65, 0x13,         /*                                      */
	0x55, 0xFD,         /*                                      */
	0x34,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x46, 0x40, 0x1F,   /*                                      */
	0x26, 0x00, 0x7D,   /*                                      */
	0x81, 0x02,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x46, 0x88, 0x13,   /*                                      */
	0x26, 0x20, 0x4E,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xB4,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x26, 0xFF, 0x03,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xC0,               /*                                      */
	0xC0                /*                                      */
};

/*                                                              */
static __u8 twhl850_rdesc_fixed1[] = {
	0x05, 0x01,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x01,         /*                                      */
	0x09, 0x01,         /*                                      */
	0xA0,               /*                                      */
	0x05, 0x09,         /*                                      */
	0x75, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x19, 0x01,         /*                                      */
	0x29, 0x03,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x05,         /*                                      */
	0x81, 0x03,         /*                                      */
	0x05, 0x01,         /*                                      */
	0x09, 0x30,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x16, 0x00, 0x80,   /*                                      */
	0x26, 0xFF, 0x7F,   /*                                      */
	0x75, 0x10,         /*                                      */
	0x95, 0x02,         /*                                      */
	0x81, 0x06,         /*                                      */
	0x09, 0x38,         /*                                      */
	0x15, 0xFF,         /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x75, 0x08,         /*                                      */
	0x81, 0x06,         /*                                      */
	0x81, 0x03,         /*                                      */
	0xC0,               /*                                      */
	0xC0                /*                                      */
};

/*                                                                      */
static __u8 twhl850_rdesc_fixed2[] = {
	0x05, 0x01,         /*                                      */
	0x09, 0x06,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x03,         /*                                      */
	0x05, 0x07,         /*                                      */
	0x14,               /*                                      */
	0x19, 0xE0,         /*                                      */
	0x29, 0xE7,         /*                                      */
	0x25, 0x01,         /*                                      */
	0x75, 0x01,         /*                                      */
	0x95, 0x08,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x18,               /*                                      */
	0x29, 0xFF,         /*                                      */
	0x26, 0xFF, 0x00,   /*                                      */
	0x75, 0x08,         /*                                      */
	0x95, 0x06,         /*                                      */
	0x80,               /*                                      */
	0xC0                /*                                      */
};

/*
                                                               
                                                                      
 */

/*                                                   */
#define TWHA60_RDESC_ORIG_SIZE0 254
#define TWHA60_RDESC_ORIG_SIZE1 139

/*                                                      */
static __u8 twha60_rdesc_fixed0[] = {
	0x05, 0x0D,         /*                                      */
	0x09, 0x02,         /*                                      */
	0xA1, 0x01,         /*                                      */
	0x85, 0x09,         /*                                      */
	0x09, 0x20,         /*                                      */
	0xA0,               /*                                      */
	0x75, 0x01,         /*                                      */
	0x09, 0x42,         /*                                      */
	0x09, 0x44,         /*                                      */
	0x09, 0x46,         /*                                      */
	0x14,               /*                                      */
	0x25, 0x01,         /*                                      */
	0x95, 0x03,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x95, 0x04,         /*                                      */
	0x81, 0x01,         /*                                      */
	0x09, 0x32,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x81, 0x02,         /*                                      */
	0x75, 0x10,         /*                                      */
	0x95, 0x01,         /*                                      */
	0x14,               /*                                      */
	0xA4,               /*                                      */
	0x05, 0x01,         /*                                      */
	0x55, 0xFD,         /*                                      */
	0x65, 0x13,         /*                                      */
	0x34,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x46, 0x10, 0x27,   /*                                      */
	0x27, 0x3F, 0x9C,
		0x00, 0x00, /*                                      */
	0x81, 0x02,         /*                                      */
	0x09, 0x31,         /*                                      */
	0x46, 0x6A, 0x18,   /*                                      */
	0x26, 0xA7, 0x61,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xB4,               /*                                      */
	0x09, 0x30,         /*                                      */
	0x26, 0xFF, 0x03,   /*                                      */
	0x81, 0x02,         /*                                      */
	0xC0,               /*                                      */
	0xC0                /*                                      */
};

/*                                                             */
static __u8 twha60_rdesc_fixed1[] = {
	0x05, 0x01, /*                              */
	0x09, 0x06, /*                              */
	0xA1, 0x01, /*                              */
	0x85, 0x05, /*                              */
	0x05, 0x07, /*                              */
	0x14,       /*                              */
	0x25, 0x01, /*                              */
	0x75, 0x01, /*                              */
	0x95, 0x08, /*                              */
	0x81, 0x01, /*                              */
	0x95, 0x0C, /*                              */
	0x19, 0x3A, /*                              */
	0x29, 0x45, /*                              */
	0x81, 0x02, /*                              */
	0x95, 0x0C, /*                              */
	0x19, 0x68, /*                              */
	0x29, 0x73, /*                              */
	0x81, 0x02, /*                              */
	0x95, 0x08, /*                              */
	0x81, 0x01, /*                              */
	0xC0        /*                              */
};

static __u8 *uclogic_report_fixup(struct hid_device *hdev, __u8 *rdesc,
					unsigned int *rsize)
{
	struct usb_interface *iface = to_usb_interface(hdev->dev.parent);
	__u8 iface_num = iface->cur_altsetting->desc.bInterfaceNumber;

	switch (hdev->product) {
	case USB_DEVICE_ID_UCLOGIC_TABLET_PF1209:
		if (*rsize == PF1209_RDESC_ORIG_SIZE) {
			rdesc = pf1209_rdesc_fixed;
			*rsize = sizeof(pf1209_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_UCLOGIC_TABLET_WP4030U:
		if (*rsize == WPXXXXU_RDESC_ORIG_SIZE) {
			rdesc = wp4030u_rdesc_fixed;
			*rsize = sizeof(wp4030u_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_UCLOGIC_TABLET_WP5540U:
		if (*rsize == WPXXXXU_RDESC_ORIG_SIZE) {
			rdesc = wp5540u_rdesc_fixed;
			*rsize = sizeof(wp5540u_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_UCLOGIC_TABLET_WP8060U:
		if (*rsize == WPXXXXU_RDESC_ORIG_SIZE) {
			rdesc = wp8060u_rdesc_fixed;
			*rsize = sizeof(wp8060u_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_UCLOGIC_TABLET_WP1062:
		if (*rsize == WP1062_RDESC_ORIG_SIZE) {
			rdesc = wp1062_rdesc_fixed;
			*rsize = sizeof(wp1062_rdesc_fixed);
		}
		break;
	case USB_DEVICE_ID_UCLOGIC_WIRELESS_TABLET_TWHL850:
		switch (iface_num) {
		case 0:
			if (*rsize == TWHL850_RDESC_ORIG_SIZE0) {
				rdesc = twhl850_rdesc_fixed0;
				*rsize = sizeof(twhl850_rdesc_fixed0);
			}
			break;
		case 1:
			if (*rsize == TWHL850_RDESC_ORIG_SIZE1) {
				rdesc = twhl850_rdesc_fixed1;
				*rsize = sizeof(twhl850_rdesc_fixed1);
			}
			break;
		case 2:
			if (*rsize == TWHL850_RDESC_ORIG_SIZE2) {
				rdesc = twhl850_rdesc_fixed2;
				*rsize = sizeof(twhl850_rdesc_fixed2);
			}
			break;
		}
		break;
	case USB_DEVICE_ID_UCLOGIC_TABLET_TWHA60:
		switch (iface_num) {
		case 0:
			if (*rsize == TWHA60_RDESC_ORIG_SIZE0) {
				rdesc = twha60_rdesc_fixed0;
				*rsize = sizeof(twha60_rdesc_fixed0);
			}
			break;
		case 1:
			if (*rsize == TWHA60_RDESC_ORIG_SIZE1) {
				rdesc = twha60_rdesc_fixed1;
				*rsize = sizeof(twha60_rdesc_fixed1);
			}
			break;
		}
		break;
	}

	return rdesc;
}

static const struct hid_device_id uclogic_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_UCLOGIC,
				USB_DEVICE_ID_UCLOGIC_TABLET_PF1209) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_UCLOGIC,
				USB_DEVICE_ID_UCLOGIC_TABLET_WP4030U) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_UCLOGIC,
				USB_DEVICE_ID_UCLOGIC_TABLET_WP5540U) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_UCLOGIC,
				USB_DEVICE_ID_UCLOGIC_TABLET_WP8060U) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_UCLOGIC,
				USB_DEVICE_ID_UCLOGIC_TABLET_WP1062) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_UCLOGIC,
				USB_DEVICE_ID_UCLOGIC_WIRELESS_TABLET_TWHL850) },
	{ HID_USB_DEVICE(USB_VENDOR_ID_UCLOGIC,
				USB_DEVICE_ID_UCLOGIC_TABLET_TWHA60) },
	{ }
};
MODULE_DEVICE_TABLE(hid, uclogic_devices);

static struct hid_driver uclogic_driver = {
	.name = "uclogic",
	.id_table = uclogic_devices,
	.report_fixup = uclogic_report_fixup,
};
module_hid_driver(uclogic_driver);

MODULE_LICENSE("GPL");