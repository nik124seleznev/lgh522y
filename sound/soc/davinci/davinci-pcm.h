/*
 * ALSA PCM interface for the TI DAVINCI processor
 *
 * Author:      Vladimir Barinov, <vbarinov@embeddedalley.com>
 * Copyright:   (C) 2007 MontaVista Software, Inc., <source@mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _DAVINCI_PCM_H
#define _DAVINCI_PCM_H

#include <linux/genalloc.h>
#include <linux/platform_data/davinci_asp.h>
#include <mach/edma.h>

struct davinci_pcm_dma_params {
	int channel;			/*                     */
	unsigned short acnt;
	dma_addr_t dma_addr;		/*                                 */
	unsigned sram_size;
	struct gen_pool *sram_pool;	/*                             */
	enum dma_event_q asp_chan_q;	/*                                    */
	enum dma_event_q ram_chan_q;	/*                                    */
	unsigned char data_type;	/*                */
	unsigned char convert_mono_stereo;
	unsigned int fifo_level;
};

int davinci_soc_platform_register(struct device *dev);
void davinci_soc_platform_unregister(struct device *dev);

#endif
