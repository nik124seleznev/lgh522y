/*
 * omap_hwmod_2xxx_3xxx_ipblock_data.c - common IP block data for OMAP2/3
 *
 * Copyright (C) 2011 Nokia Corporation
 * Copyright (C) 2012 Texas Instruments, Inc.
 * Paul Walmsley
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/dmaengine.h>
#include <linux/omap-dma.h>

#include "omap_hwmod.h"
#include "hdq1w.h"

#include "omap_hwmod_common_data.h"
#include "dma.h"

/*      */

static struct omap_hwmod_class_sysconfig omap2_uart_sysc = {
	.rev_offs	= 0x50,
	.sysc_offs	= 0x54,
	.syss_offs	= 0x58,
	.sysc_flags	= (SYSC_HAS_SIDLEMODE |
			   SYSC_HAS_ENAWAKEUP | SYSC_HAS_SOFTRESET |
			   SYSC_HAS_AUTOIDLE | SYSS_HAS_RESET_STATUS),
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2_uart_class = {
	.name	= "uart",
	.sysc	= &omap2_uart_sysc,
};

/*
              
                     
 */

static struct omap_hwmod_class_sysconfig omap2_dss_sysc = {
	.rev_offs	= 0x0000,
	.sysc_offs	= 0x0010,
	.syss_offs	= 0x0014,
	.sysc_flags	= (SYSC_HAS_SOFTRESET | SYSC_HAS_AUTOIDLE |
			   SYSS_HAS_RESET_STATUS),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2_dss_hwmod_class = {
	.name	= "dss",
	.sysc	= &omap2_dss_sysc,
	.reset	= omap_dss_reset,
};

/*
               
                                
 */

static struct omap_hwmod_class_sysconfig omap2_rfbi_sysc = {
	.rev_offs	= 0x0000,
	.sysc_offs	= 0x0010,
	.syss_offs	= 0x0014,
	.sysc_flags	= (SYSC_HAS_SIDLEMODE | SYSC_HAS_SOFTRESET |
			   SYSC_HAS_AUTOIDLE),
	.idlemodes	= (SIDLE_FORCE | SIDLE_NO | SIDLE_SMART),
	.sysc_fields	= &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2_rfbi_hwmod_class = {
	.name	= "rfbi",
	.sysc	= &omap2_rfbi_sysc,
};

/*
               
                
 */

struct omap_hwmod_class omap2_venc_hwmod_class = {
	.name = "venc",
};


/*                              */
struct omap_hwmod_dma_info omap2_uart1_sdma_reqs[] = {
	{ .name = "rx", .dma_req = OMAP24XX_DMA_UART1_RX, },
	{ .name = "tx", .dma_req = OMAP24XX_DMA_UART1_TX, },
	{ .dma_req = -1 }
};

struct omap_hwmod_dma_info omap2_uart2_sdma_reqs[] = {
	{ .name = "rx", .dma_req = OMAP24XX_DMA_UART2_RX, },
	{ .name = "tx", .dma_req = OMAP24XX_DMA_UART2_TX, },
	{ .dma_req = -1 }
};

struct omap_hwmod_dma_info omap2_uart3_sdma_reqs[] = {
	{ .name = "rx", .dma_req = OMAP24XX_DMA_UART3_RX, },
	{ .name = "tx", .dma_req = OMAP24XX_DMA_UART3_TX, },
	{ .dma_req = -1 }
};

struct omap_hwmod_dma_info omap2_i2c1_sdma_reqs[] = {
	{ .name = "tx", .dma_req = OMAP24XX_DMA_I2C1_TX },
	{ .name = "rx", .dma_req = OMAP24XX_DMA_I2C1_RX },
	{ .dma_req = -1 }
};

struct omap_hwmod_dma_info omap2_i2c2_sdma_reqs[] = {
	{ .name = "tx", .dma_req = OMAP24XX_DMA_I2C2_TX },
	{ .name = "rx", .dma_req = OMAP24XX_DMA_I2C2_RX },
	{ .dma_req = -1 }
};

struct omap_hwmod_dma_info omap2_mcspi1_sdma_reqs[] = {
	{ .name = "tx0", .dma_req = 35 }, /*              */
	{ .name = "rx0", .dma_req = 36 }, /*              */
	{ .name = "tx1", .dma_req = 37 }, /*              */
	{ .name = "rx1", .dma_req = 38 }, /*              */
	{ .name = "tx2", .dma_req = 39 }, /*              */
	{ .name = "rx2", .dma_req = 40 }, /*              */
	{ .name = "tx3", .dma_req = 41 }, /*              */
	{ .name = "rx3", .dma_req = 42 }, /*              */
	{ .dma_req = -1 }
};

struct omap_hwmod_dma_info omap2_mcspi2_sdma_reqs[] = {
	{ .name = "tx0", .dma_req = 43 }, /*              */
	{ .name = "rx0", .dma_req = 44 }, /*              */
	{ .name = "tx1", .dma_req = 45 }, /*              */
	{ .name = "rx1", .dma_req = 46 }, /*              */
	{ .dma_req = -1 }
};

struct omap_hwmod_dma_info omap2_mcbsp1_sdma_reqs[] = {
	{ .name = "rx", .dma_req = 32 },
	{ .name = "tx", .dma_req = 31 },
	{ .dma_req = -1 }
};

struct omap_hwmod_dma_info omap2_mcbsp2_sdma_reqs[] = {
	{ .name = "rx", .dma_req = 34 },
	{ .name = "tx", .dma_req = 33 },
	{ .dma_req = -1 }
};

struct omap_hwmod_dma_info omap2_mcbsp3_sdma_reqs[] = {
	{ .name = "rx", .dma_req = 18 },
	{ .name = "tx", .dma_req = 17 },
	{ .dma_req = -1 }
};

/*                     */


/*
                        
 */

struct omap_hwmod_class l3_hwmod_class = {
	.name = "l3"
};

struct omap_hwmod_class l4_hwmod_class = {
	.name = "l4"
};

struct omap_hwmod_class mpu_hwmod_class = {
	.name = "mpu"
};

struct omap_hwmod_class iva_hwmod_class = {
	.name = "iva"
};

/*                          */

struct omap_hwmod_irq_info omap2_timer1_mpu_irqs[] = {
	{ .irq = 37 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer2_mpu_irqs[] = {
	{ .irq = 38 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer3_mpu_irqs[] = {
	{ .irq = 39 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer4_mpu_irqs[] = {
	{ .irq = 40 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer5_mpu_irqs[] = {
	{ .irq = 41 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer6_mpu_irqs[] = {
	{ .irq = 42 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer7_mpu_irqs[] = {
	{ .irq = 43 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer8_mpu_irqs[] = {
	{ .irq = 44 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer9_mpu_irqs[] = {
	{ .irq = 45 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer10_mpu_irqs[] = {
	{ .irq = 46 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_timer11_mpu_irqs[] = {
	{ .irq = 47 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_uart1_mpu_irqs[] = {
	{ .irq = 72 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_uart2_mpu_irqs[] = {
	{ .irq = 73 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_uart3_mpu_irqs[] = {
	{ .irq = 74 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_dispc_irqs[] = {
	{ .irq = 25 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_i2c1_mpu_irqs[] = {
	{ .irq = 56 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_i2c2_mpu_irqs[] = {
	{ .irq = 57 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_gpio1_irqs[] = {
	{ .irq = 29 + OMAP_INTC_START, }, /*                     */
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_gpio2_irqs[] = {
	{ .irq = 30 + OMAP_INTC_START, }, /*                     */
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_gpio3_irqs[] = {
	{ .irq = 31 + OMAP_INTC_START, }, /*                     */
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_gpio4_irqs[] = {
	{ .irq = 32 + OMAP_INTC_START, }, /*                     */
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_dma_system_irqs[] = {
	{ .name = "0", .irq = 12 + OMAP_INTC_START, }, /*                    */
	{ .name = "1", .irq = 13 + OMAP_INTC_START, }, /*                    */
	{ .name = "2", .irq = 14 + OMAP_INTC_START, }, /*                    */
	{ .name = "3", .irq = 15 + OMAP_INTC_START, }, /*                    */
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_mcspi1_mpu_irqs[] = {
	{ .irq = 65 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_irq_info omap2_mcspi2_mpu_irqs[] = {
	{ .irq = 66 + OMAP_INTC_START, },
	{ .irq = -1 },
};

struct omap_hwmod_class_sysconfig omap2_hdq1w_sysc = {
	.rev_offs	= 0x0,
	.sysc_offs	= 0x14,
	.syss_offs	= 0x18,
	.sysc_flags	= (SYSC_HAS_SOFTRESET | SYSC_HAS_AUTOIDLE |
			   SYSS_HAS_RESET_STATUS),
	.sysc_fields    = &omap_hwmod_sysc_type1,
};

struct omap_hwmod_class omap2_hdq1w_class = {
	.name	= "hdq1w",
	.sysc	= &omap2_hdq1w_sysc,
	.reset	= &omap_hdq1w_reset,
};

struct omap_hwmod_irq_info omap2_hdq1w_mpu_irqs[] = {
	{ .irq = 58 + OMAP_INTC_START, },
	{ .irq = -1 },
};

