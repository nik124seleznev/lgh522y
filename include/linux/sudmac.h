/*
 * Header for the SUDMAC driver
 *
 * Copyright (C) 2013 Renesas Solutions Corp.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 */
#ifndef SUDMAC_H
#define SUDMAC_H

#include <linux/dmaengine.h>
#include <linux/shdma-base.h>
#include <linux/types.h>

/*                                                                        */
struct sudmac_slave {
	struct shdma_slave	shdma_slave;	/*                     */
};

/*
                                                                               
                       
 */
struct sudmac_slave_config {
	int		slave_id;
};

struct sudmac_channel {
	unsigned long	offset;
	unsigned long	config;
	unsigned long	wait;		/*                                 */
	unsigned long	dint_end_bit;
};

struct sudmac_pdata {
	const struct sudmac_slave_config *slave;
	int slave_num;
	const struct sudmac_channel *channel;
	int channel_num;
};

/*                                           */
#define SUDMAC_TX_BUFFER_MODE	BIT(0)
#define SUDMAC_RX_END_MODE	BIT(1)

/*                                                 */
#define SUDMAC_DMA_BIT_CH0	BIT(0)
#define SUDMAC_DMA_BIT_CH1	BIT(1)

#endif