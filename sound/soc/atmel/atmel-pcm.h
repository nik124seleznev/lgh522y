/*
 * at91-pcm.h - ALSA PCM interface for the Atmel AT91 SoC.
 *
 *  Copyright (C) 2005 SAN People
 *  Copyright (C) 2008 Atmel
 *
 * Authors: Sedji Gaouaou <sedji.gaouaou@atmel.com>
 *
 * Based on at91-pcm. by:
 * Frank Mandarino <fmandarino@endrelia.com>
 * Copyright 2006 Endrelia Technologies Inc.
 *
 * Based on pxa2xx-pcm.c by:
 *
 * Author:	Nicolas Pitre
 * Created:	Nov 30, 2004
 * Copyright:	(C) 2004 MontaVista Software, Inc.
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

#ifndef _ATMEL_PCM_H
#define _ATMEL_PCM_H

#include <linux/atmel-ssc.h>

#define ATMEL_SSC_DMABUF_SIZE	(64 * 1024)

/*
                                                                 
 */
struct atmel_pdc_regs {
	unsigned int	xpr;		/*                        */
	unsigned int	xcr;		/*                        */
	unsigned int	xnpr;		/*                             */
	unsigned int	xncr;		/*                             */
	unsigned int	ptcr;		/*                      */
};

struct atmel_ssc_mask {
	u32	ssc_enable;		/*                       */
	u32	ssc_disable;		/*                        */
	u32	ssc_error;		/*                      */
	u32	ssc_endx;		/*                    */
	u32	ssc_endbuf;		/*                      */
	u32	pdc_enable;		/*                       */
	u32	pdc_disable;		/*                        */
};

/*
                                                                   
                                                                     
                                                                           
                                                                      
                                                                    
            
 */
struct atmel_pcm_dma_params {
	char *name;			/*                   */
	int pdc_xfer_size;		/*                                */
	struct ssc_device *ssc;		/*                       */
	struct atmel_pdc_regs *pdc;	/*                                   */
	struct atmel_ssc_mask *mask;	/*                       */
	struct snd_pcm_substream *substream;
	void (*dma_intr_handler)(u32, struct snd_pcm_substream *);
};

/*
                                                                              
 */
#define ssc_readx(base, reg)            (__raw_readl((base) + (reg)))
#define ssc_writex(base, reg, value)    __raw_writel((value), (base) + (reg))

int atmel_pcm_new(struct snd_soc_pcm_runtime *rtd);
void atmel_pcm_free(struct snd_pcm *pcm);
int atmel_pcm_mmap(struct snd_pcm_substream *substream,
		struct vm_area_struct *vma);

#if defined(CONFIG_SND_ATMEL_SOC_PDC) || \
	defined(CONFIG_SND_ATMEL_SOC_PDC_MODULE)
int atmel_pcm_pdc_platform_register(struct device *dev);
void atmel_pcm_pdc_platform_unregister(struct device *dev);
#else
static inline int atmel_pcm_pdc_platform_register(struct device *dev)
{
	return 0;
}
static inline void atmel_pcm_pdc_platform_unregister(struct device *dev)
{
}
#endif

#if defined(CONFIG_SND_ATMEL_SOC_DMA) || \
	defined(CONFIG_SND_ATMEL_SOC_DMA_MODULE)
int atmel_pcm_dma_platform_register(struct device *dev);
void atmel_pcm_dma_platform_unregister(struct device *dev);
#else
static inline int atmel_pcm_dma_platform_register(struct device *dev)
{
	return 0;
}
static inline void atmel_pcm_dma_platform_unregister(struct device *dev)
{
}
#endif

#endif /*              */