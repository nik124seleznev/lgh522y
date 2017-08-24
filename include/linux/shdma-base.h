/*
 * Dmaengine driver base library for DMA controllers, found on SH-based SoCs
 *
 * extracted from shdma.c and headers
 *
 * Copyright (C) 2011-2012 Guennadi Liakhovetski <g.liakhovetski@gmx.de>
 * Copyright (C) 2009 Nobuhiro Iwamatsu <iwamatsu.nobuhiro@renesas.com>
 * Copyright (C) 2009 Renesas Solutions, Inc. All rights reserved.
 * Copyright (C) 2007 Freescale Semiconductor, Inc. All rights reserved.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 */

#ifndef SHDMA_BASE_H
#define SHDMA_BASE_H

#include <linux/dmaengine.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/types.h>

/* 
                                        
                                                            
                                                                   
                               
                                      
 */
enum shdma_pm_state {
	SHDMA_PM_ESTABLISHED,
	SHDMA_PM_BUSY,
	SHDMA_PM_PENDING,
};

struct device;

/*
                                                                      
                                                               
                                                                     
 */

struct shdma_slave {
	int slave_id;
};

struct shdma_desc {
	struct list_head node;
	struct dma_async_tx_descriptor async_tx;
	enum dma_transfer_direction direction;
	size_t partial;
	dma_cookie_t cookie;
	int chunks;
	int mark;
};

struct shdma_chan {
	spinlock_t chan_lock;		/*                        */
	struct list_head ld_queue;	/*                        */
	struct list_head ld_free;	/*                       */
	struct dma_chan dma_chan;	/*             */
	struct device *dev;		/*                */
	void *desc;			/*                             */
	int desc_num;			/*            */
	size_t max_xfer_len;		/*                     */
	int id;				/*                        */
	int irq;			/*             */
	int slave_id;			/*                         */
	enum shdma_pm_state pm_state;
};

/* 
                                                  
                                                                        
                       
                                                    
                                                             
                                        
                                                                        
                                      
                                                                 
                                               
                                                               
                     
                                                                
                       
 */
struct shdma_ops {
	bool (*desc_completed)(struct shdma_chan *, struct shdma_desc *);
	void (*halt_channel)(struct shdma_chan *);
	bool (*channel_busy)(struct shdma_chan *);
	dma_addr_t (*slave_addr)(struct shdma_chan *);
	int (*desc_setup)(struct shdma_chan *, struct shdma_desc *,
			  dma_addr_t, dma_addr_t, size_t *);
	int (*set_slave)(struct shdma_chan *, int, bool);
	void (*setup_xfer)(struct shdma_chan *, int);
	void (*start_xfer)(struct shdma_chan *, struct shdma_desc *);
	struct shdma_desc *(*embedded_desc)(void *, int);
	bool (*chan_irq)(struct shdma_chan *, int);
	size_t (*get_partial)(struct shdma_chan *, struct shdma_desc *);
};

struct shdma_dev {
	struct dma_device dma_dev;
	struct shdma_chan **schan;
	const struct shdma_ops *ops;
	size_t desc_size;
};

#define shdma_for_each_chan(c, d, i) for (i = 0, c = (d)->schan[0]; \
				i < (d)->dma_dev.chancnt; c = (d)->schan[++i])

int shdma_request_irq(struct shdma_chan *, int,
			   unsigned long, const char *);
void shdma_free_irq(struct shdma_chan *);
bool shdma_reset(struct shdma_dev *sdev);
void shdma_chan_probe(struct shdma_dev *sdev,
			   struct shdma_chan *schan, int id);
void shdma_chan_remove(struct shdma_chan *schan);
int shdma_init(struct device *dev, struct shdma_dev *sdev,
		    int chan_num);
void shdma_cleanup(struct shdma_dev *sdev);

#endif
