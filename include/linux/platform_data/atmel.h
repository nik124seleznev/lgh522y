/*
 * atmel platform data
 *
 * GPL v2 Only
 */

#ifndef __ATMEL_H__
#define __ATMEL_H__

#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/leds.h>
#include <linux/spi/spi.h>
#include <linux/usb/atmel_usba_udc.h>
#include <linux/atmel-mci.h>
#include <sound/atmel-ac97c.h>
#include <linux/serial.h>
#include <linux/platform_data/macb.h>

/*
                                             
           
 */
#define ATMEL_MAX_UART	7

 /*            */
struct at91_udc_data {
	int	vbus_pin;		/*                          */
	u8	vbus_active_low;	/*               */
	u8	vbus_polled;		/*                            */
	int	pullup_pin;		/*                        */
	u8	pullup_active_low;	/*                                  */
};

 /*               */
struct at91_cf_data {
	int	irq_pin;		/*         */
	int	det_pin;		/*             */
	int	vcc_pin;		/*                 */
	int	rst_pin;		/*            */
	u8	chipselect;		/*                        */
	u8	flags;
#define AT91_CF_TRUE_IDE	0x01
#define AT91_IDE_SWAP_A0_A2	0x02
};

 /*          */
#define AT91_MAX_USBH_PORTS	3
struct at91_usbh_data {
	int		vbus_pin[AT91_MAX_USBH_PORTS];	/*                        */
	int             overcurrent_pin[AT91_MAX_USBH_PORTS];
	u8		ports;				/*                             */
	u8              overcurrent_supported;
	u8              vbus_pin_active_low[AT91_MAX_USBH_PORTS];
	u8              overcurrent_status[AT91_MAX_USBH_PORTS];
	u8              overcurrent_changed[AT91_MAX_USBH_PORTS];
};

 /*                   */
struct atmel_nand_data {
	int		enable_pin;		/*             */
	int		det_pin;		/*             */
	int		rdy_pin;		/*            */
	u8		rdy_pin_active_low;	/*                           */
	u8		ale;			/*                                      */
	u8		cle;			/*                                      */
	u8		bus_width_16;		/*                    */
	u8		ecc_mode;		/*          */
	u8		on_flash_bbt;		/*              */
	struct mtd_partition *parts;
	unsigned int	num_parts;
};

 /*        */
struct atmel_uart_data {
	int			num;		/*          */
	short			use_dma_tx;	/*                   */
	short			use_dma_rx;	/*                  */
	void __iomem		*regs;		/*                            */
	struct serial_rs485	rs485;		/*                */
};

 /*                        */
struct at91_tsadcc_data {
	unsigned int    adc_clock;
	u8		pendet_debounce;
	u8		ts_sample_hold_time;
};

/*     */
struct at91_can_data {
	void (*transceiver_switch)(int on);
};

/*                                                                    */
extern int at91_suspend_entering_slow_clock(void);

#endif /*             */
