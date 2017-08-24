/*
 * Driver for OMAP-UART controller.
 * Based on drivers/serial/8250.c
 *
 * Copyright (C) 2010 Texas Instruments.
 *
 * Authors:
 *	Govindraj R	<govindraj.raja@ti.com>
 *	Thara Gopinath	<thara@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __OMAP_SERIAL_H__
#define __OMAP_SERIAL_H__

#include <linux/serial_core.h>
#include <linux/device.h>
#include <linux/pm_qos.h>

#define DRIVER_NAME	"omap_uart"

/*
                                           
                                                   
                           
 */
#define OMAP_SERIAL_NAME	"ttyO"

struct omap_uart_port_info {
	bool			dma_enabled;	/*                     */
	unsigned int		uartclk;	/*                 */
	upf_t			flags;		/*             */
	unsigned int		dma_rx_buf_size;
	unsigned int		dma_rx_timeout;
	unsigned int		autosuspend_timeout;
	unsigned int		dma_rx_poll_rate;
	int			DTR_gpio;
	int			DTR_inverted;
	int			DTR_present;

	int (*get_context_loss_count)(struct device *);
	void (*enable_wakeup)(struct device *, bool);
};

#endif /*                   */
