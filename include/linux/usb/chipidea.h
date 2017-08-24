/*
                                                          
 */

#ifndef __LINUX_USB_CHIPIDEA_H
#define __LINUX_USB_CHIPIDEA_H

#include <linux/usb/otg.h>

struct ci13xxx;
struct ci13xxx_platform_data {
	const char	*name;
	/*                                    */
	uintptr_t	 capoffset;
	unsigned	 power_budget;
	struct usb_phy	*phy;
	unsigned long	 flags;
#define CI13XXX_REGS_SHARED		BIT(0)
#define CI13XXX_REQUIRE_TRANSCEIVER	BIT(1)
#define CI13XXX_PULLUP_ON_VBUS		BIT(2)
#define CI13XXX_DISABLE_STREAMING	BIT(3)

#define CI13XXX_CONTROLLER_RESET_EVENT		0
#define CI13XXX_CONTROLLER_STOPPED_EVENT	1
	void	(*notify_event) (struct ci13xxx *ci, unsigned event);
};

/*                                        */
#define DEF_CAPOFFSET		0x100

/*                    */
struct platform_device *ci13xxx_add_device(struct device *dev,
			struct resource *res, int nres,
			struct ci13xxx_platform_data *platdata);
/*                       */
void ci13xxx_remove_device(struct platform_device *pdev);

#endif
