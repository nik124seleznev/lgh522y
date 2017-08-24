/*                             */
/*
  
                                                                      
                                                                    
                                                  
 */

#ifndef __LINUX_USB_OTG_H
#define __LINUX_USB_OTG_H

#include <linux/usb/phy.h>

struct usb_otg {
	u8			default_a;

	struct usb_phy		*phy;
	struct usb_bus		*host;
	struct usb_gadget	*gadget;

	/*                                 */
	int	(*set_host)(struct usb_otg *otg, struct usb_bus *host);

	/*                                       */
	int	(*set_peripheral)(struct usb_otg *otg,
					struct usb_gadget *gadget);

	/*                                                   */
	int	(*set_vbus)(struct usb_otg *otg, bool enabled);

	/*                                                */
	int	(*start_srp)(struct usb_otg *otg);

	/*                                   */
	int	(*start_hnp)(struct usb_otg *otg);

};

extern const char *usb_otg_state_string(enum usb_otg_state state);

/*                    */
static inline int
otg_start_hnp(struct usb_otg *otg)
{
	if (otg && otg->start_hnp)
		return otg->start_hnp(otg);

	return -ENOTSUPP;
}

/*                    */
static inline int
otg_set_vbus(struct usb_otg *otg, bool enabled)
{
	if (otg && otg->set_vbus)
		return otg->set_vbus(otg, enabled);

	return -ENOTSUPP;
}

/*          */
static inline int
otg_set_host(struct usb_otg *otg, struct usb_bus *host)
{
	if (otg && otg->set_host)
		return otg->set_host(otg, host);

	return -ENOTSUPP;
}

/*                                       */

/*                    */
static inline int
otg_set_peripheral(struct usb_otg *otg, struct usb_gadget *periph)
{
	if (otg && otg->set_peripheral)
		return otg->set_peripheral(otg, periph);

	return -ENOTSUPP;
}

static inline int
otg_start_srp(struct usb_otg *otg)
{
	if (otg && otg->start_srp)
		return otg->start_srp(otg);

	return -ENOTSUPP;
}

/*                                                    */
extern int usb_bus_start_enum(struct usb_bus *bus, unsigned port_num);

#endif /*                   */
