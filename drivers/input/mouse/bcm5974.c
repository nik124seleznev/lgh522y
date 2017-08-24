/*
 * Apple USB BCM5974 (Macbook Air and Penryn Macbook Pro) multitouch driver
 *
 * Copyright (C) 2008	   Henrik Rydberg (rydberg@euromail.se)
 *
 * The USB initialization and package decoding was made by
 * Scott Shawcroft as part of the touchd user-space driver project:
 * Copyright (C) 2008	   Scott Shawcroft (scott.shawcroft@gmail.com)
 *
 * The BCM5974 driver is based on the appletouch driver:
 * Copyright (C) 2001-2004 Greg Kroah-Hartman (greg@kroah.com)
 * Copyright (C) 2005      Johannes Berg (johannes@sipsolutions.net)
 * Copyright (C) 2005	   Stelian Pop (stelian@popies.net)
 * Copyright (C) 2005	   Frank Arnold (frank@scirocco-5v-turbo.de)
 * Copyright (C) 2005	   Peter Osterlund (petero2@telia.com)
 * Copyright (C) 2005	   Michael Hanselmann (linux-kernel@hansmi.ch)
 * Copyright (C) 2006	   Nicolas Boichat (nicolas@boichat.ch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb/input.h>
#include <linux/hid.h>
#include <linux/mutex.h>
#include <linux/input/mt.h>

#define USB_VENDOR_ID_APPLE		0x05ac

/*                            */
#define USB_DEVICE_ID_APPLE_WELLSPRING_ANSI	0x0223
#define USB_DEVICE_ID_APPLE_WELLSPRING_ISO	0x0224
#define USB_DEVICE_ID_APPLE_WELLSPRING_JIS	0x0225
/*                                   */
#define USB_DEVICE_ID_APPLE_WELLSPRING2_ANSI	0x0230
#define USB_DEVICE_ID_APPLE_WELLSPRING2_ISO	0x0231
#define USB_DEVICE_ID_APPLE_WELLSPRING2_JIS	0x0232
/*                                       */
#define USB_DEVICE_ID_APPLE_WELLSPRING3_ANSI	0x0236
#define USB_DEVICE_ID_APPLE_WELLSPRING3_ISO	0x0237
#define USB_DEVICE_ID_APPLE_WELLSPRING3_JIS	0x0238
/*                                          */
#define USB_DEVICE_ID_APPLE_WELLSPRING4_ANSI	0x023f
#define USB_DEVICE_ID_APPLE_WELLSPRING4_ISO	0x0240
#define USB_DEVICE_ID_APPLE_WELLSPRING4_JIS	0x0241
/*                                          */
#define USB_DEVICE_ID_APPLE_WELLSPRING4A_ANSI	0x0242
#define USB_DEVICE_ID_APPLE_WELLSPRING4A_ISO	0x0243
#define USB_DEVICE_ID_APPLE_WELLSPRING4A_JIS	0x0244
/*                                */
#define USB_DEVICE_ID_APPLE_WELLSPRING5_ANSI	0x0245
#define USB_DEVICE_ID_APPLE_WELLSPRING5_ISO	0x0246
#define USB_DEVICE_ID_APPLE_WELLSPRING5_JIS	0x0247
/*                                    */
#define USB_DEVICE_ID_APPLE_WELLSPRING6A_ANSI	0x0249
#define USB_DEVICE_ID_APPLE_WELLSPRING6A_ISO	0x024a
#define USB_DEVICE_ID_APPLE_WELLSPRING6A_JIS	0x024b
/*                                    */
#define USB_DEVICE_ID_APPLE_WELLSPRING6_ANSI	0x024c
#define USB_DEVICE_ID_APPLE_WELLSPRING6_ISO	0x024d
#define USB_DEVICE_ID_APPLE_WELLSPRING6_JIS	0x024e
/*                      */
#define USB_DEVICE_ID_APPLE_WELLSPRING5A_ANSI	0x0252
#define USB_DEVICE_ID_APPLE_WELLSPRING5A_ISO	0x0253
#define USB_DEVICE_ID_APPLE_WELLSPRING5A_JIS	0x0254
/*                                     */
#define USB_DEVICE_ID_APPLE_WELLSPRING7_ANSI	0x0262
#define USB_DEVICE_ID_APPLE_WELLSPRING7_ISO	0x0263
#define USB_DEVICE_ID_APPLE_WELLSPRING7_JIS	0x0264
/*                                        */
#define USB_DEVICE_ID_APPLE_WELLSPRING7A_ANSI	0x0259
#define USB_DEVICE_ID_APPLE_WELLSPRING7A_ISO	0x025a
#define USB_DEVICE_ID_APPLE_WELLSPRING7A_JIS	0x025b
/*                                    */
#define USB_DEVICE_ID_APPLE_WELLSPRING8_ANSI	0x0290
#define USB_DEVICE_ID_APPLE_WELLSPRING8_ISO	0x0291
#define USB_DEVICE_ID_APPLE_WELLSPRING8_JIS	0x0292

#define BCM5974_DEVICE(prod) {					\
	.match_flags = (USB_DEVICE_ID_MATCH_DEVICE |		\
			USB_DEVICE_ID_MATCH_INT_CLASS |		\
			USB_DEVICE_ID_MATCH_INT_PROTOCOL),	\
	.idVendor = USB_VENDOR_ID_APPLE,			\
	.idProduct = (prod),					\
	.bInterfaceClass = USB_INTERFACE_CLASS_HID,		\
	.bInterfaceProtocol = USB_INTERFACE_PROTOCOL_MOUSE	\
}

/*                                             */
static const struct usb_device_id bcm5974_table[] = {
	/*               */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING_JIS),
	/*                  */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING2_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING2_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING2_JIS),
	/*            */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING3_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING3_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING3_JIS),
	/*               */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4_JIS),
	/*               */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4A_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4A_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING4A_JIS),
	/*             */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5_JIS),
	/*               */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6A_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6A_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6A_JIS),
	/*               */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING6_JIS),
	/*               */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5A_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5A_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING5A_JIS),
	/*                */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7_JIS),
	/*                */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7A_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7A_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING7A_JIS),
	/*               */
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING8_ANSI),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING8_ISO),
	BCM5974_DEVICE(USB_DEVICE_ID_APPLE_WELLSPRING8_JIS),
	/*                   */
	{}
};
MODULE_DEVICE_TABLE(usb, bcm5974_table);

MODULE_AUTHOR("Henrik Rydberg");
MODULE_DESCRIPTION("Apple USB BCM5974 multitouch driver");
MODULE_LICENSE("GPL");

#define dprintk(level, format, a...)\
	{ if (debug >= level) printk(KERN_DEBUG format, ##a); }

static int debug = 1;
module_param(debug, int, 0644);
MODULE_PARM_DESC(debug, "Activate debugging output");

/*                       */
struct bt_data {
	u8 unknown1;		/*          */
	u8 button;		/*             */
	u8 rel_x;		/*                       */
	u8 rel_y;		/*                       */
};

/*                       */
enum tp_type {
	TYPE1,			/*                */
	TYPE2,			/*                               */
	TYPE3			/*                                          */
};

/*                                            */
#define FINGER_TYPE1		(13 * sizeof(__le16))
#define FINGER_TYPE2		(15 * sizeof(__le16))
#define FINGER_TYPE3		(19 * sizeof(__le16))

/*                              */
#define BUTTON_TYPE2		15
#define BUTTON_TYPE3		23

/*                                */
#define HAS_INTEGRATED_BUTTON	1

/*                                         */
struct tp_finger {
	__le16 origin;		/*                                  */
	__le16 abs_x;		/*                      */
	__le16 abs_y;		/*                      */
	__le16 rel_x;		/*                      */
	__le16 rel_y;		/*                      */
	__le16 tool_major;	/*                       */
	__le16 tool_minor;	/*                       */
	__le16 orientation;	/*                                     */
	__le16 touch_major;	/*                        */
	__le16 touch_minor;	/*                        */
	__le16 unused[3];	/*       */
	__le16 multi;		/*                                            */
} __attribute__((packed,aligned(2)));

/*                                                             */
#define MAX_FINGERS		16
#define SIZEOF_FINGER		sizeof(struct tp_finger)
#define SIZEOF_ALL_FINGERS	(MAX_FINGERS * SIZEOF_FINGER)
#define MAX_FINGER_ORIENTATION	16384

/*                            */
struct bcm5974_param {
	int snratio;		/*                       */
	int min;		/*                        */
	int max;		/*                        */
};

/*                               */
struct bcm5974_config {
	int ansi, iso, jis;	/*                               */
	int caps;		/*                           */
	int bt_ep;		/*                                      */
	int bt_datalen;		/*                                     */
	int tp_ep;		/*                                        */
	enum tp_type tp_type;	/*                            */
	int tp_offset;		/*                                */
	int tp_datalen;		/*                                       */
	struct bcm5974_param p;	/*                        */
	struct bcm5974_param w;	/*                     */
	struct bcm5974_param x;	/*                   */
	struct bcm5974_param y;	/*                 */
	struct bcm5974_param o;	/*                    */
};

/*                          */
struct bcm5974 {
	char phys[64];
	struct usb_device *udev;	/*            */
	struct usb_interface *intf;	/*               */
	struct input_dev *input;	/*           */
	struct bcm5974_config cfg;	/*                      */
	struct mutex pm_mutex;		/*                                  */
	int opened;			/*                      */
	struct urb *bt_urb;		/*                          */
	struct bt_data *bt_data;	/*                         */
	struct urb *tp_urb;		/*                            */
	u8 *tp_data;			/*                           */
	const struct tp_finger *index[MAX_FINGERS];	/*                   */
	struct input_mt_pos pos[MAX_FINGERS];		/*                */
	int slots[MAX_FINGERS];				/*                  */
};

/*                        */
#define SN_PRESSURE	45		/*                                */
#define SN_WIDTH	25		/*                             */
#define SN_COORD	250		/*                                  */
#define SN_ORIENT	10		/*                                   */

/*                  */
static const struct bcm5974_config bcm5974_config_table[] = {
	{
		USB_DEVICE_ID_APPLE_WELLSPRING_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING_JIS,
		0,
		0x84, sizeof(struct bt_data),
		0x81, TYPE1, FINGER_TYPE1, FINGER_TYPE1 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 256 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4824, 5342 },
		{ SN_COORD, -172, 5820 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING2_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING2_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING2_JIS,
		0,
		0x84, sizeof(struct bt_data),
		0x81, TYPE1, FINGER_TYPE1, FINGER_TYPE1 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 256 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4824, 4824 },
		{ SN_COORD, -172, 4290 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING3_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING3_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING3_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, TYPE2, FINGER_TYPE2, FINGER_TYPE2 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4460, 5166 },
		{ SN_COORD, -75, 6700 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING4_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING4_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING4_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, TYPE2, FINGER_TYPE2, FINGER_TYPE2 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING4A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING4A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING4A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, TYPE2, FINGER_TYPE2, FINGER_TYPE2 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4616, 5112 },
		{ SN_COORD, -142, 5234 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING5_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING5_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING5_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, TYPE2, FINGER_TYPE2, FINGER_TYPE2 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4415, 5050 },
		{ SN_COORD, -55, 6680 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING6_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING6_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING6_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, TYPE2, FINGER_TYPE2, FINGER_TYPE2 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING5A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING5A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING5A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, TYPE2, FINGER_TYPE2, FINGER_TYPE2 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4750, 5280 },
		{ SN_COORD, -150, 6730 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING6A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING6A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING6A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, TYPE2, FINGER_TYPE2, FINGER_TYPE2 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING7_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING7_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING7_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, TYPE2, FINGER_TYPE2, FINGER_TYPE2 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4750, 5280 },
		{ SN_COORD, -150, 6730 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING7A_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING7A_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING7A_JIS,
		HAS_INTEGRATED_BUTTON,
		0x84, sizeof(struct bt_data),
		0x81, TYPE2, FINGER_TYPE2, FINGER_TYPE2 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4750, 5280 },
		{ SN_COORD, -150, 6730 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{
		USB_DEVICE_ID_APPLE_WELLSPRING8_ANSI,
		USB_DEVICE_ID_APPLE_WELLSPRING8_ISO,
		USB_DEVICE_ID_APPLE_WELLSPRING8_JIS,
		HAS_INTEGRATED_BUTTON,
		0, sizeof(struct bt_data),
		0x83, TYPE3, FINGER_TYPE3, FINGER_TYPE3 + SIZEOF_ALL_FINGERS,
		{ SN_PRESSURE, 0, 300 },
		{ SN_WIDTH, 0, 2048 },
		{ SN_COORD, -4620, 5140 },
		{ SN_COORD, -150, 6600 },
		{ SN_ORIENT, -MAX_FINGER_ORIENTATION, MAX_FINGER_ORIENTATION }
	},
	{}
};

/*                                                    */
static const struct bcm5974_config *bcm5974_get_config(struct usb_device *udev)
{
	u16 id = le16_to_cpu(udev->descriptor.idProduct);
	const struct bcm5974_config *cfg;

	for (cfg = bcm5974_config_table; cfg->ansi; ++cfg)
		if (cfg->ansi == id || cfg->iso == id || cfg->jis == id)
			return cfg;

	return bcm5974_config_table;
}

/*                                                */
static inline int raw2int(__le16 x)
{
	return (signed short)le16_to_cpu(x);
}

static void set_abs(struct input_dev *input, unsigned int code,
		    const struct bcm5974_param *p)
{
	int fuzz = p->snratio ? (p->max - p->min) / p->snratio : 0;
	input_set_abs_params(input, code, p->min, p->max, fuzz, 0);
}

/*                                      */
static void setup_events_to_report(struct input_dev *input_dev,
				   const struct bcm5974_config *cfg)
{
	__set_bit(EV_ABS, input_dev->evbit);

	/*                    */
	input_set_abs_params(input_dev, ABS_PRESSURE, 0, 256, 5, 0);
	input_set_abs_params(input_dev, ABS_TOOL_WIDTH, 0, 16, 0, 0);

	/*                   */
	set_abs(input_dev, ABS_MT_TOUCH_MAJOR, &cfg->w);
	set_abs(input_dev, ABS_MT_TOUCH_MINOR, &cfg->w);
	/*                      */
	set_abs(input_dev, ABS_MT_WIDTH_MAJOR, &cfg->w);
	set_abs(input_dev, ABS_MT_WIDTH_MINOR, &cfg->w);
	/*                    */
	set_abs(input_dev, ABS_MT_ORIENTATION, &cfg->o);
	/*                 */
	set_abs(input_dev, ABS_MT_POSITION_X, &cfg->x);
	set_abs(input_dev, ABS_MT_POSITION_Y, &cfg->y);

	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(BTN_LEFT, input_dev->keybit);

	if (cfg->caps & HAS_INTEGRATED_BUTTON)
		__set_bit(INPUT_PROP_BUTTONPAD, input_dev->propbit);

	input_mt_init_slots(input_dev, MAX_FINGERS,
		INPUT_MT_POINTER | INPUT_MT_DROP_UNUSED | INPUT_MT_TRACK);
}

/*                                            */
static int report_bt_state(struct bcm5974 *dev, int size)
{
	if (size != sizeof(struct bt_data))
		return -EIO;

	dprintk(7,
		"bcm5974: button data: %x %x %x %x\n",
		dev->bt_data->unknown1, dev->bt_data->button,
		dev->bt_data->rel_x, dev->bt_data->rel_y);

	input_report_key(dev->input, BTN_LEFT, dev->bt_data->button);
	input_sync(dev->input);

	return 0;
}

static void report_finger_data(struct input_dev *input, int slot,
			       const struct input_mt_pos *pos,
			       const struct tp_finger *f)
{
	input_mt_slot(input, slot);
	input_mt_report_slot_state(input, MT_TOOL_FINGER, true);

	input_report_abs(input, ABS_MT_TOUCH_MAJOR,
			 raw2int(f->touch_major) << 1);
	input_report_abs(input, ABS_MT_TOUCH_MINOR,
			 raw2int(f->touch_minor) << 1);
	input_report_abs(input, ABS_MT_WIDTH_MAJOR,
			 raw2int(f->tool_major) << 1);
	input_report_abs(input, ABS_MT_WIDTH_MINOR,
			 raw2int(f->tool_minor) << 1);
	input_report_abs(input, ABS_MT_ORIENTATION,
			 MAX_FINGER_ORIENTATION - raw2int(f->orientation));
	input_report_abs(input, ABS_MT_POSITION_X, pos->x);
	input_report_abs(input, ABS_MT_POSITION_Y, pos->y);
}

static void report_synaptics_data(struct input_dev *input,
				  const struct bcm5974_config *cfg,
				  const struct tp_finger *f, int raw_n)
{
	int abs_p = 0, abs_w = 0;

	if (raw_n) {
		int p = raw2int(f->touch_major);
		int w = raw2int(f->tool_major);
		if (p > 0 && raw2int(f->origin)) {
			abs_p = clamp_val(256 * p / cfg->p.max, 0, 255);
			abs_w = clamp_val(16 * w / cfg->w.max, 0, 15);
		}
	}

	input_report_abs(input, ABS_PRESSURE, abs_p);
	input_report_abs(input, ABS_TOOL_WIDTH, abs_w);
}

/*                                                */
static int report_tp_state(struct bcm5974 *dev, int size)
{
	const struct bcm5974_config *c = &dev->cfg;
	const struct tp_finger *f;
	struct input_dev *input = dev->input;
	int raw_n, i, n = 0;

	if (size < c->tp_offset || (size - c->tp_offset) % SIZEOF_FINGER != 0)
		return -EIO;

	/*                           */
	f = (const struct tp_finger *)(dev->tp_data + c->tp_offset);
	raw_n = (size - c->tp_offset) / SIZEOF_FINGER;

	for (i = 0; i < raw_n; i++) {
		if (raw2int(f[i].touch_major) == 0)
			continue;
		dev->pos[n].x = raw2int(f[i].abs_x);
		dev->pos[n].y = c->y.min + c->y.max - raw2int(f[i].abs_y);
		dev->index[n++] = &f[i];
	}

	input_mt_assign_slots(input, dev->slots, dev->pos, n);

	for (i = 0; i < n; i++)
		report_finger_data(input, dev->slots[i],
				   &dev->pos[i], dev->index[i]);

	input_mt_sync_frame(input);

	report_synaptics_data(input, c, f, raw_n);

	/*                                           */
	if (c->tp_type == TYPE2) {
		int ibt = raw2int(dev->tp_data[BUTTON_TYPE2]);
		input_report_key(input, BTN_LEFT, ibt);
	}

	if (c->tp_type == TYPE3)
		input_report_key(input, BTN_LEFT, dev->tp_data[BUTTON_TYPE3]);

	input_sync(input);

	return 0;
}

/*                                     */
#define BCM5974_WELLSPRING_MODE_READ_REQUEST_ID		1
#define BCM5974_WELLSPRING_MODE_WRITE_REQUEST_ID	9
#define BCM5974_WELLSPRING_MODE_REQUEST_VALUE		0x300
#define BCM5974_WELLSPRING_MODE_REQUEST_INDEX		0
#define BCM5974_WELLSPRING_MODE_VENDOR_VALUE		0x01
#define BCM5974_WELLSPRING_MODE_NORMAL_VALUE		0x08

static int bcm5974_wellspring_mode(struct bcm5974 *dev, bool on)
{
	int retval = 0, size;
	char *data;

	/*                                       */
	if (dev->cfg.tp_type == TYPE3)
		return 0;

	data = kmalloc(8, GFP_KERNEL);
	if (!data) {
		dev_err(&dev->intf->dev, "out of memory\n");
		retval = -ENOMEM;
		goto out;
	}

	/*                    */
	size = usb_control_msg(dev->udev, usb_rcvctrlpipe(dev->udev, 0),
			BCM5974_WELLSPRING_MODE_READ_REQUEST_ID,
			USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			BCM5974_WELLSPRING_MODE_REQUEST_VALUE,
			BCM5974_WELLSPRING_MODE_REQUEST_INDEX, data, 8, 5000);

	if (size != 8) {
		dev_err(&dev->intf->dev, "could not read from device\n");
		retval = -EIO;
		goto out;
	}

	/*                       */
	data[0] = on ?
		BCM5974_WELLSPRING_MODE_VENDOR_VALUE :
		BCM5974_WELLSPRING_MODE_NORMAL_VALUE;

	/*                     */
	size = usb_control_msg(dev->udev, usb_sndctrlpipe(dev->udev, 0),
			BCM5974_WELLSPRING_MODE_WRITE_REQUEST_ID,
			USB_DIR_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
			BCM5974_WELLSPRING_MODE_REQUEST_VALUE,
			BCM5974_WELLSPRING_MODE_REQUEST_INDEX, data, 8, 5000);

	if (size != 8) {
		dev_err(&dev->intf->dev, "could not write to device\n");
		retval = -EIO;
		goto out;
	}

	dprintk(2, "bcm5974: switched to %s mode.\n",
		on ? "wellspring" : "normal");

 out:
	kfree(data);
	return retval;
}

static void bcm5974_irq_button(struct urb *urb)
{
	struct bcm5974 *dev = urb->context;
	struct usb_interface *intf = dev->intf;
	int error;

	switch (urb->status) {
	case 0:
		break;
	case -EOVERFLOW:
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		dev_dbg(&intf->dev, "button urb shutting down: %d\n",
			urb->status);
		return;
	default:
		dev_dbg(&intf->dev, "button urb status: %d\n", urb->status);
		goto exit;
	}

	if (report_bt_state(dev, dev->bt_urb->actual_length))
		dprintk(1, "bcm5974: bad button package, length: %d\n",
			dev->bt_urb->actual_length);

exit:
	error = usb_submit_urb(dev->bt_urb, GFP_ATOMIC);
	if (error)
		dev_err(&intf->dev, "button urb failed: %d\n", error);
}

static void bcm5974_irq_trackpad(struct urb *urb)
{
	struct bcm5974 *dev = urb->context;
	struct usb_interface *intf = dev->intf;
	int error;

	switch (urb->status) {
	case 0:
		break;
	case -EOVERFLOW:
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		dev_dbg(&intf->dev, "trackpad urb shutting down: %d\n",
			urb->status);
		return;
	default:
		dev_dbg(&intf->dev, "trackpad urb status: %d\n", urb->status);
		goto exit;
	}

	/*                          */
	if (dev->tp_urb->actual_length == 2)
		goto exit;

	if (report_tp_state(dev, dev->tp_urb->actual_length))
		dprintk(1, "bcm5974: bad trackpad package, length: %d\n",
			dev->tp_urb->actual_length);

exit:
	error = usb_submit_urb(dev->tp_urb, GFP_ATOMIC);
	if (error)
		dev_err(&intf->dev, "trackpad urb failed: %d\n", error);
}

/*
                                                                   
                                                                
                                                                     
                                                            
                                                               
                                                                      
                                                                    
                                                           
                                                                 
                                                          
                                                                   
                                                                     
                                                                
                                                                   
                                                                   
                                  
 */
static int bcm5974_start_traffic(struct bcm5974 *dev)
{
	int error;

	error = bcm5974_wellspring_mode(dev, true);
	if (error) {
		dprintk(1, "bcm5974: mode switch failed\n");
		goto err_out;
	}

	if (dev->bt_urb) {
		error = usb_submit_urb(dev->bt_urb, GFP_KERNEL);
		if (error)
			goto err_reset_mode;
	}

	error = usb_submit_urb(dev->tp_urb, GFP_KERNEL);
	if (error)
		goto err_kill_bt;

	return 0;

err_kill_bt:
	usb_kill_urb(dev->bt_urb);
err_reset_mode:
	bcm5974_wellspring_mode(dev, false);
err_out:
	return error;
}

static void bcm5974_pause_traffic(struct bcm5974 *dev)
{
	usb_kill_urb(dev->tp_urb);
	usb_kill_urb(dev->bt_urb);
	bcm5974_wellspring_mode(dev, false);
}

/*
                                                                  
                                               
  
                                                                    
  
                                                         
 */
static int bcm5974_open(struct input_dev *input)
{
	struct bcm5974 *dev = input_get_drvdata(input);
	int error;

	error = usb_autopm_get_interface(dev->intf);
	if (error)
		return error;

	mutex_lock(&dev->pm_mutex);

	error = bcm5974_start_traffic(dev);
	if (!error)
		dev->opened = 1;

	mutex_unlock(&dev->pm_mutex);

	if (error)
		usb_autopm_put_interface(dev->intf);

	return error;
}

static void bcm5974_close(struct input_dev *input)
{
	struct bcm5974 *dev = input_get_drvdata(input);

	mutex_lock(&dev->pm_mutex);

	bcm5974_pause_traffic(dev);
	dev->opened = 0;

	mutex_unlock(&dev->pm_mutex);

	usb_autopm_put_interface(dev->intf);
}

static int bcm5974_suspend(struct usb_interface *iface, pm_message_t message)
{
	struct bcm5974 *dev = usb_get_intfdata(iface);

	mutex_lock(&dev->pm_mutex);

	if (dev->opened)
		bcm5974_pause_traffic(dev);

	mutex_unlock(&dev->pm_mutex);

	return 0;
}

static int bcm5974_resume(struct usb_interface *iface)
{
	struct bcm5974 *dev = usb_get_intfdata(iface);
	int error = 0;

	mutex_lock(&dev->pm_mutex);

	if (dev->opened)
		error = bcm5974_start_traffic(dev);

	mutex_unlock(&dev->pm_mutex);

	return error;
}

static int bcm5974_probe(struct usb_interface *iface,
			 const struct usb_device_id *id)
{
	struct usb_device *udev = interface_to_usbdev(iface);
	const struct bcm5974_config *cfg;
	struct bcm5974 *dev;
	struct input_dev *input_dev;
	int error = -ENOMEM;

	/*                        */
	cfg = bcm5974_get_config(udev);

	/*                                                        */
	dev = kzalloc(sizeof(struct bcm5974), GFP_KERNEL);
	input_dev = input_allocate_device();
	if (!dev || !input_dev) {
		dev_err(&iface->dev, "out of memory\n");
		goto err_free_devs;
	}

	dev->udev = udev;
	dev->intf = iface;
	dev->input = input_dev;
	dev->cfg = *cfg;
	mutex_init(&dev->pm_mutex);

	/*            */
	if (cfg->tp_type == TYPE1) {
		dev->bt_urb = usb_alloc_urb(0, GFP_KERNEL);
		if (!dev->bt_urb)
			goto err_free_devs;
	}

	dev->tp_urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!dev->tp_urb)
		goto err_free_bt_urb;

	if (dev->bt_urb) {
		dev->bt_data = usb_alloc_coherent(dev->udev,
					  dev->cfg.bt_datalen, GFP_KERNEL,
					  &dev->bt_urb->transfer_dma);
		if (!dev->bt_data)
			goto err_free_urb;
	}

	dev->tp_data = usb_alloc_coherent(dev->udev,
					  dev->cfg.tp_datalen, GFP_KERNEL,
					  &dev->tp_urb->transfer_dma);
	if (!dev->tp_data)
		goto err_free_bt_buffer;

	if (dev->bt_urb)
		usb_fill_int_urb(dev->bt_urb, udev,
				 usb_rcvintpipe(udev, cfg->bt_ep),
				 dev->bt_data, dev->cfg.bt_datalen,
				 bcm5974_irq_button, dev, 1);

	usb_fill_int_urb(dev->tp_urb, udev,
			 usb_rcvintpipe(udev, cfg->tp_ep),
			 dev->tp_data, dev->cfg.tp_datalen,
			 bcm5974_irq_trackpad, dev, 1);

	/*                       */
	usb_make_path(udev, dev->phys, sizeof(dev->phys));
	strlcat(dev->phys, "/input0", sizeof(dev->phys));

	input_dev->name = "bcm5974";
	input_dev->phys = dev->phys;
	usb_to_input_id(dev->udev, &input_dev->id);
	/*                                                  */
	input_dev->id.version = cfg->caps;
	input_dev->dev.parent = &iface->dev;

	input_set_drvdata(input_dev, dev);

	input_dev->open = bcm5974_open;
	input_dev->close = bcm5974_close;

	setup_events_to_report(input_dev, cfg);

	error = input_register_device(dev->input);
	if (error)
		goto err_free_buffer;

	/*                                                */
	usb_set_intfdata(iface, dev);

	return 0;

err_free_buffer:
	usb_free_coherent(dev->udev, dev->cfg.tp_datalen,
		dev->tp_data, dev->tp_urb->transfer_dma);
err_free_bt_buffer:
	if (dev->bt_urb)
		usb_free_coherent(dev->udev, dev->cfg.bt_datalen,
				  dev->bt_data, dev->bt_urb->transfer_dma);
err_free_urb:
	usb_free_urb(dev->tp_urb);
err_free_bt_urb:
	usb_free_urb(dev->bt_urb);
err_free_devs:
	usb_set_intfdata(iface, NULL);
	input_free_device(input_dev);
	kfree(dev);
	return error;
}

static void bcm5974_disconnect(struct usb_interface *iface)
{
	struct bcm5974 *dev = usb_get_intfdata(iface);

	usb_set_intfdata(iface, NULL);

	input_unregister_device(dev->input);
	usb_free_coherent(dev->udev, dev->cfg.tp_datalen,
			  dev->tp_data, dev->tp_urb->transfer_dma);
	if (dev->bt_urb)
		usb_free_coherent(dev->udev, dev->cfg.bt_datalen,
				  dev->bt_data, dev->bt_urb->transfer_dma);
	usb_free_urb(dev->tp_urb);
	usb_free_urb(dev->bt_urb);
	kfree(dev);
}

static struct usb_driver bcm5974_driver = {
	.name			= "bcm5974",
	.probe			= bcm5974_probe,
	.disconnect		= bcm5974_disconnect,
	.suspend		= bcm5974_suspend,
	.resume			= bcm5974_resume,
	.id_table		= bcm5974_table,
	.supports_autosuspend	= 1,
};

module_usb_driver(bcm5974_driver);
