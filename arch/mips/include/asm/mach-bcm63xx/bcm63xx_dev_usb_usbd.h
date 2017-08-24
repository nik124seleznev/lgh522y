#ifndef BCM63XX_DEV_USB_USBD_H_
#define BCM63XX_DEV_USB_USBD_H_

/*
                           
 */
struct bcm63xx_usbd_platform_data {
	/*                                             */
	int use_fullspeed;

	/*                                               */
	int port_no;
};

int bcm63xx_usbd_register(const struct bcm63xx_usbd_platform_data *pd);

#endif /*                         */
