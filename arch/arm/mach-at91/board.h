/*
 * arch/arm/mach-at91/include/mach/board.h
 *
 *  Copyright (C) 2005 HP Labs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
                                                                        
                                                                      
                                           
  
                                                                    
                                                                    
                                                                       
 */

#ifndef __ASM_ARCH_BOARD_H
#define __ASM_ARCH_BOARD_H

#include <linux/platform_data/atmel.h>

 /*            */
extern void __init at91_add_device_udc(struct at91_udc_data *data);

 /*                       */
extern void __init at91_add_device_usba(struct usba_platform_data *data);

 /*               */
extern void __init at91_add_device_cf(struct at91_cf_data *data);

 /*          */
  /*                           */
extern void __init at91_add_device_mci(short mmc_id, struct mci_platform_data *data);

extern void __init at91_add_device_eth(struct macb_platform_data *data);

 /*          */
extern void __init at91_add_device_usbh(struct at91_usbh_data *data);
extern void __init at91_add_device_usbh_ohci(struct at91_usbh_data *data);
extern void __init at91_add_device_usbh_ehci(struct at91_usbh_data *data);

extern void __init at91_add_device_nand(struct atmel_nand_data *data);

 /*    */
#if defined(CONFIG_ARCH_AT91SAM9G45)
extern void __init at91_add_device_i2c(short i2c_id, struct i2c_board_info *devices, int nr_devices);
#else
extern void __init at91_add_device_i2c(struct i2c_board_info *devices, int nr_devices);
#endif

 /*     */
extern void __init at91_add_device_spi(struct spi_board_info *devices, int nr_devices);

 /*        */
#define ATMEL_UART_CTS	0x01
#define ATMEL_UART_RTS	0x02
#define ATMEL_UART_DSR	0x04
#define ATMEL_UART_DTR	0x08
#define ATMEL_UART_DCD	0x10
#define ATMEL_UART_RI	0x20

extern void __init at91_register_uart(unsigned id, unsigned portnr, unsigned pins);

extern struct platform_device *atmel_default_console_device;

extern void __init at91_add_device_serial(void);

/*
      
 */
#define AT91_PWM0	0
#define AT91_PWM1	1
#define AT91_PWM2	2
#define AT91_PWM3	3

extern void __init at91_add_device_pwm(u32 mask);

/*
                                                                      
                                                                       
                                                                  
 */
#define ATMEL_SSC_TK	0x01
#define ATMEL_SSC_TF	0x02
#define ATMEL_SSC_TD	0x04
#define ATMEL_SSC_TX	(ATMEL_SSC_TK | ATMEL_SSC_TF | ATMEL_SSC_TD)

#define ATMEL_SSC_RK	0x10
#define ATMEL_SSC_RF	0x20
#define ATMEL_SSC_RD	0x40
#define ATMEL_SSC_RX	(ATMEL_SSC_RK | ATMEL_SSC_RF | ATMEL_SSC_RD)

extern void __init at91_add_device_ssc(unsigned id, unsigned pins);

 /*                */
struct atmel_lcdfb_info;
extern void __init at91_add_device_lcdc(struct atmel_lcdfb_info *data);

 /*      */
extern void __init at91_add_device_ac97(struct ac97c_platform_data *data);

 /*     */
struct isi_platform_data;
extern void __init at91_add_device_isi(struct isi_platform_data *data,
		bool use_pck_as_mck);

 /*                        */
extern void __init at91_add_device_tsadcc(struct at91_tsadcc_data *data);

/*     */
extern void __init at91_add_device_can(struct at91_can_data *data);

 /*      */
extern void __init at91_gpio_leds(struct gpio_led *leds, int nr);
extern void __init at91_pwm_leds(struct gpio_led *leds, int nr);

#endif
