/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*                                    */
struct usbmisc_ops {
	/*                                          */
	int (*init)(struct device *dev);
	/*                                            */
	int (*post)(struct device *dev);
};

struct usbmisc_usb_device {
	struct device *dev; /*                       */
	int index;

	unsigned int disable_oc:1; /*                              */
	unsigned int evdo:1; /*                                  */
};

int usbmisc_set_ops(const struct usbmisc_ops *ops);
void usbmisc_unset_ops(const struct usbmisc_ops *ops);
int
usbmisc_get_init_data(struct device *dev, struct usbmisc_usb_device *usbdev);
