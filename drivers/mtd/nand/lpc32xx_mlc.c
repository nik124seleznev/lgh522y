/*
 * Driver for NAND MLC Controller in LPC32xx
 *
 * Author: Roland Stigge <stigge@antcom.de>
 *
 * Copyright © 2011 WORK Microwave GmbH
 * Copyright © 2011, 2012 Roland Stigge
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
 *
 * NAND Flash Controller Operation:
 * - Read: Auto Decode
 * - Write: Auto Encode
 * - Tested Page Sizes: 2048, 4096
 */

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/of.h>
#include <linux/of_mtd.h>
#include <linux/of_gpio.h>
#include <linux/mtd/lpc32xx_mlc.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/mtd/nand_ecc.h>

#define DRV_NAME "lpc32xx_mlc"

/*                                                                     
                                      
                                                                     */

#define MLC_BUFF(x)			(x + 0x00000)
#define MLC_DATA(x)			(x + 0x08000)
#define MLC_CMD(x)			(x + 0x10000)
#define MLC_ADDR(x)			(x + 0x10004)
#define MLC_ECC_ENC_REG(x)		(x + 0x10008)
#define MLC_ECC_DEC_REG(x)		(x + 0x1000C)
#define MLC_ECC_AUTO_ENC_REG(x)		(x + 0x10010)
#define MLC_ECC_AUTO_DEC_REG(x)		(x + 0x10014)
#define MLC_RPR(x)			(x + 0x10018)
#define MLC_WPR(x)			(x + 0x1001C)
#define MLC_RUBP(x)			(x + 0x10020)
#define MLC_ROBP(x)			(x + 0x10024)
#define MLC_SW_WP_ADD_LOW(x)		(x + 0x10028)
#define MLC_SW_WP_ADD_HIG(x)		(x + 0x1002C)
#define MLC_ICR(x)			(x + 0x10030)
#define MLC_TIME_REG(x)			(x + 0x10034)
#define MLC_IRQ_MR(x)			(x + 0x10038)
#define MLC_IRQ_SR(x)			(x + 0x1003C)
#define MLC_LOCK_PR(x)			(x + 0x10044)
#define MLC_ISR(x)			(x + 0x10048)
#define MLC_CEH(x)			(x + 0x1004C)

/*                                                                     
                         
                                                                     */
#define MLCCMD_RESET			0xFF

/*                                                                     
                         
                                                                     */
#define MLCICR_WPROT			(1 << 3)
#define MLCICR_LARGEBLOCK		(1 << 2)
#define MLCICR_LONGADDR			(1 << 1)
#define MLCICR_16BIT			(1 << 0)  /*                         */

/*                                                                     
                              
                                                                     */
#define MLCTIMEREG_TCEA_DELAY(n)	(((n) & 0x03) << 24)
#define MLCTIMEREG_BUSY_DELAY(n)	(((n) & 0x1F) << 19)
#define MLCTIMEREG_NAND_TA(n)		(((n) & 0x07) << 16)
#define MLCTIMEREG_RD_HIGH(n)		(((n) & 0x0F) << 12)
#define MLCTIMEREG_RD_LOW(n)		(((n) & 0x0F) << 8)
#define MLCTIMEREG_WR_HIGH(n)		(((n) & 0x0F) << 4)
#define MLCTIMEREG_WR_LOW(n)		(((n) & 0x0F) << 0)

/*                                                                     
                                           
                                                                     */
#define MLCIRQ_NAND_READY		(1 << 5)
#define MLCIRQ_CONTROLLER_READY		(1 << 4)
#define MLCIRQ_DECODE_FAILURE		(1 << 3)
#define MLCIRQ_DECODE_ERROR		(1 << 2)
#define MLCIRQ_ECC_READY		(1 << 1)
#define MLCIRQ_WRPROT_FAULT		(1 << 0)

/*                                                                     
                             
                                                                     */
#define MLCLOCKPR_MAGIC			0xA25E

/*                                                                     
                         
                                                                     */
#define MLCISR_DECODER_FAILURE		(1 << 6)
#define MLCISR_ERRORS			((1 << 4) | (1 << 5))
#define MLCISR_ERRORS_DETECTED		(1 << 3)
#define MLCISR_ECC_READY		(1 << 2)
#define MLCISR_CONTROLLER_READY		(1 << 1)
#define MLCISR_NAND_READY		(1 << 0)

/*                                                                     
                         
                                                                     */
#define MLCCEH_NORMAL			(1 << 0)

struct lpc32xx_nand_cfg_mlc {
	uint32_t tcea_delay;
	uint32_t busy_delay;
	uint32_t nand_ta;
	uint32_t rd_high;
	uint32_t rd_low;
	uint32_t wr_high;
	uint32_t wr_low;
	int wp_gpio;
	struct mtd_partition *parts;
	unsigned num_parts;
};

static struct nand_ecclayout lpc32xx_nand_oob = {
	.eccbytes = 40,
	.eccpos = { 6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
		   22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
		   38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
		   54, 55, 56, 57, 58, 59, 60, 61, 62, 63 },
	.oobfree = {
		{ .offset = 0,
		  .length = 6, },
		{ .offset = 16,
		  .length = 6, },
		{ .offset = 32,
		  .length = 6, },
		{ .offset = 48,
		  .length = 6, },
		},
};

static struct nand_bbt_descr lpc32xx_nand_bbt = {
	.options = NAND_BBT_ABSPAGE | NAND_BBT_2BIT | NAND_BBT_NO_OOB |
		   NAND_BBT_WRITE,
	.pages = { 524224, 0, 0, 0, 0, 0, 0, 0 },
};

static struct nand_bbt_descr lpc32xx_nand_bbt_mirror = {
	.options = NAND_BBT_ABSPAGE | NAND_BBT_2BIT | NAND_BBT_NO_OOB |
		   NAND_BBT_WRITE,
	.pages = { 524160, 0, 0, 0, 0, 0, 0, 0 },
};

struct lpc32xx_nand_host {
	struct nand_chip	nand_chip;
	struct lpc32xx_mlc_platform_data *pdata;
	struct clk		*clk;
	struct mtd_info		mtd;
	void __iomem		*io_base;
	int			irq;
	struct lpc32xx_nand_cfg_mlc	*ncfg;
	struct completion       comp_nand;
	struct completion       comp_controller;
	uint32_t llptr;
	/*
                                                                       
  */
	dma_addr_t		oob_buf_phy;
	/*
                                                                      
  */
	uint8_t			*oob_buf;
	/*                                      */
	dma_addr_t		io_base_phy;

	struct completion	comp_dma;
	struct dma_chan		*dma_chan;
	struct dma_slave_config	dma_slave_config;
	struct scatterlist	sgl;
	uint8_t			*dma_buf;
	uint8_t			*dummy_buf;
	int			mlcsubpages; /*                             */
};

/*
                                     
  
                                                                        
                                                                             
                                                                        
  
                                              
                                                                
                                                      
  
                                                                 
                                                    
  
                                                                      
                                                                             
                                                                        
                                                                            
         
  
                                                                  
  
 */
static int use_dma;

static void lpc32xx_nand_setup(struct lpc32xx_nand_host *host)
{
	uint32_t clkrate, tmp;

	/*                      */
	writel(MLCCMD_RESET, MLC_CMD(host->io_base));
	udelay(1000);

	/*                              */
	clkrate = clk_get_rate(host->clk);
	if (clkrate == 0)
		clkrate = 104000000;

	/*               
                                                       */
	writew(MLCLOCKPR_MAGIC, MLC_LOCK_PR(host->io_base));

	/*                                                       */
	tmp = MLCICR_LARGEBLOCK | MLCICR_LONGADDR;
	writel(tmp, MLC_ICR(host->io_base));

	/*                    
                                                       */
	writew(MLCLOCKPR_MAGIC, MLC_LOCK_PR(host->io_base));

	/*                                                     */
	tmp = 0;
	tmp |= MLCTIMEREG_TCEA_DELAY(clkrate / host->ncfg->tcea_delay + 1);
	tmp |= MLCTIMEREG_BUSY_DELAY(clkrate / host->ncfg->busy_delay + 1);
	tmp |= MLCTIMEREG_NAND_TA(clkrate / host->ncfg->nand_ta + 1);
	tmp |= MLCTIMEREG_RD_HIGH(clkrate / host->ncfg->rd_high + 1);
	tmp |= MLCTIMEREG_RD_LOW(clkrate / host->ncfg->rd_low);
	tmp |= MLCTIMEREG_WR_HIGH(clkrate / host->ncfg->wr_high + 1);
	tmp |= MLCTIMEREG_WR_LOW(clkrate / host->ncfg->wr_low);
	writel(tmp, MLC_TIME_REG(host->io_base));

	/*                                                */
	writeb(MLCIRQ_CONTROLLER_READY | MLCIRQ_NAND_READY,
			MLC_IRQ_MR(host->io_base));

	/*                                                    */
	writel(MLCCEH_NORMAL, MLC_CEH(host->io_base));
}

/*
                                            
 */
static void lpc32xx_nand_cmd_ctrl(struct mtd_info *mtd, int cmd,
				  unsigned int ctrl)
{
	struct nand_chip *nand_chip = mtd->priv;
	struct lpc32xx_nand_host *host = nand_chip->priv;

	if (cmd != NAND_CMD_NONE) {
		if (ctrl & NAND_CLE)
			writel(cmd, MLC_CMD(host->io_base));
		else
			writel(cmd, MLC_ADDR(host->io_base));
	}
}

/*
                                                         
 */
static int lpc32xx_nand_device_ready(struct mtd_info *mtd)
{
	struct nand_chip *nand_chip = mtd->priv;
	struct lpc32xx_nand_host *host = nand_chip->priv;

	if ((readb(MLC_ISR(host->io_base)) &
	     (MLCISR_CONTROLLER_READY | MLCISR_NAND_READY)) ==
	    (MLCISR_CONTROLLER_READY | MLCISR_NAND_READY))
		return  1;

	return 0;
}

static irqreturn_t lpc3xxx_nand_irq(int irq, struct lpc32xx_nand_host *host)
{
	uint8_t sr;

	/*                                        */
	sr = readb(MLC_IRQ_SR(host->io_base));
	if (sr & MLCIRQ_NAND_READY)
		complete(&host->comp_nand);
	if (sr & MLCIRQ_CONTROLLER_READY)
		complete(&host->comp_controller);

	return IRQ_HANDLED;
}

static int lpc32xx_waitfunc_nand(struct mtd_info *mtd, struct nand_chip *chip)
{
	struct lpc32xx_nand_host *host = chip->priv;

	if (readb(MLC_ISR(host->io_base)) & MLCISR_NAND_READY)
		goto exit;

	wait_for_completion(&host->comp_nand);

	while (!(readb(MLC_ISR(host->io_base)) & MLCISR_NAND_READY)) {
		/*                                             */
		dev_dbg(&mtd->dev, "Warning: NAND not ready.\n");
		cpu_relax();
	}

exit:
	return NAND_STATUS_READY;
}

static int lpc32xx_waitfunc_controller(struct mtd_info *mtd,
				       struct nand_chip *chip)
{
	struct lpc32xx_nand_host *host = chip->priv;

	if (readb(MLC_ISR(host->io_base)) & MLCISR_CONTROLLER_READY)
		goto exit;

	wait_for_completion(&host->comp_controller);

	while (!(readb(MLC_ISR(host->io_base)) &
		 MLCISR_CONTROLLER_READY)) {
		dev_dbg(&mtd->dev, "Warning: Controller not ready.\n");
		cpu_relax();
	}

exit:
	return NAND_STATUS_READY;
}

static int lpc32xx_waitfunc(struct mtd_info *mtd, struct nand_chip *chip)
{
	lpc32xx_waitfunc_nand(mtd, chip);
	lpc32xx_waitfunc_controller(mtd, chip);

	return NAND_STATUS_READY;
}

/*
                            
 */
static void lpc32xx_wp_enable(struct lpc32xx_nand_host *host)
{
	if (gpio_is_valid(host->ncfg->wp_gpio))
		gpio_set_value(host->ncfg->wp_gpio, 0);
}

/*
                             
 */
static void lpc32xx_wp_disable(struct lpc32xx_nand_host *host)
{
	if (gpio_is_valid(host->ncfg->wp_gpio))
		gpio_set_value(host->ncfg->wp_gpio, 1);
}

static void lpc32xx_dma_complete_func(void *completion)
{
	complete(completion);
}

static int lpc32xx_xmit_dma(struct mtd_info *mtd, void *mem, int len,
			    enum dma_transfer_direction dir)
{
	struct nand_chip *chip = mtd->priv;
	struct lpc32xx_nand_host *host = chip->priv;
	struct dma_async_tx_descriptor *desc;
	int flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;
	int res;

	sg_init_one(&host->sgl, mem, len);

	res = dma_map_sg(host->dma_chan->device->dev, &host->sgl, 1,
			 DMA_BIDIRECTIONAL);
	if (res != 1) {
		dev_err(mtd->dev.parent, "Failed to map sg list\n");
		return -ENXIO;
	}
	desc = dmaengine_prep_slave_sg(host->dma_chan, &host->sgl, 1, dir,
				       flags);
	if (!desc) {
		dev_err(mtd->dev.parent, "Failed to prepare slave sg\n");
		goto out1;
	}

	init_completion(&host->comp_dma);
	desc->callback = lpc32xx_dma_complete_func;
	desc->callback_param = &host->comp_dma;

	dmaengine_submit(desc);
	dma_async_issue_pending(host->dma_chan);

	wait_for_completion_timeout(&host->comp_dma, msecs_to_jiffies(1000));

	dma_unmap_sg(host->dma_chan->device->dev, &host->sgl, 1,
		     DMA_BIDIRECTIONAL);
	return 0;
out1:
	dma_unmap_sg(host->dma_chan->device->dev, &host->sgl, 1,
		     DMA_BIDIRECTIONAL);
	return -ENXIO;
}

static int lpc32xx_read_page(struct mtd_info *mtd, struct nand_chip *chip,
			     uint8_t *buf, int oob_required, int page)
{
	struct lpc32xx_nand_host *host = chip->priv;
	int i, j;
	uint8_t *oobbuf = chip->oob_poi;
	uint32_t mlc_isr;
	int res;
	uint8_t *dma_buf;
	bool dma_mapped;

	if ((void *)buf <= high_memory) {
		dma_buf = buf;
		dma_mapped = true;
	} else {
		dma_buf = host->dma_buf;
		dma_mapped = false;
	}

	/*                             */
	chip->cmdfunc(mtd, NAND_CMD_READ0, 0, page);

	/*                   */
	for (i = 0; i < host->mlcsubpages; i++) {
		/*                           */
		writeb(0x00, MLC_ECC_AUTO_DEC_REG(host->io_base));

		/*                           */
		lpc32xx_waitfunc_controller(mtd, chip);

		/*                        */
		mlc_isr = readl(MLC_ISR(host->io_base));
		if (mlc_isr & MLCISR_DECODER_FAILURE) {
			mtd->ecc_stats.failed++;
			dev_warn(&mtd->dev, "%s: DECODER_FAILURE\n", __func__);
		} else if (mlc_isr & MLCISR_ERRORS_DETECTED) {
			mtd->ecc_stats.corrected += ((mlc_isr >> 4) & 0x3) + 1;
		}

		/*                     */
		if (use_dma) {
			res = lpc32xx_xmit_dma(mtd, dma_buf + i * 512, 512,
					       DMA_DEV_TO_MEM);
			if (res)
				return res;
		} else {
			for (j = 0; j < (512 >> 2); j++) {
				*((uint32_t *)(buf)) =
					readl(MLC_BUFF(host->io_base));
				buf += 4;
			}
		}
		for (j = 0; j < (16 >> 2); j++) {
			*((uint32_t *)(oobbuf)) =
				readl(MLC_BUFF(host->io_base));
			oobbuf += 4;
		}
	}

	if (use_dma && !dma_mapped)
		memcpy(buf, dma_buf, mtd->writesize);

	return 0;
}

static int lpc32xx_write_page_lowlevel(struct mtd_info *mtd,
				       struct nand_chip *chip,
				       const uint8_t *buf, int oob_required)
{
	struct lpc32xx_nand_host *host = chip->priv;
	const uint8_t *oobbuf = chip->oob_poi;
	uint8_t *dma_buf = (uint8_t *)buf;
	int res;
	int i, j;

	if (use_dma && (void *)buf >= high_memory) {
		dma_buf = host->dma_buf;
		memcpy(dma_buf, buf, mtd->writesize);
	}

	for (i = 0; i < host->mlcsubpages; i++) {
		/*              */
		writeb(0x00, MLC_ECC_ENC_REG(host->io_base));

		/*                               */
		if (use_dma) {
			res = lpc32xx_xmit_dma(mtd, dma_buf + i * 512, 512,
					       DMA_MEM_TO_DEV);
			if (res)
				return res;
		} else {
			for (j = 0; j < (512 >> 2); j++) {
				writel(*((uint32_t *)(buf)),
				       MLC_BUFF(host->io_base));
				buf += 4;
			}
		}
		writel(*((uint32_t *)(oobbuf)), MLC_BUFF(host->io_base));
		oobbuf += 4;
		writew(*((uint16_t *)(oobbuf)), MLC_BUFF(host->io_base));
		oobbuf += 12;

		/*                                                          */
		writeb(0x00, MLC_ECC_AUTO_ENC_REG(host->io_base));

		/*                           */
		lpc32xx_waitfunc_controller(mtd, chip);
	}
	return 0;
}

static int lpc32xx_write_page(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offset, int data_len, const uint8_t *buf,
			int oob_required, int page, int cached, int raw)
{
	int res;

	chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, page);
	res = lpc32xx_write_page_lowlevel(mtd, chip, buf, oob_required);
	chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);
	lpc32xx_waitfunc(mtd, chip);

	return res;
}

static int lpc32xx_read_oob(struct mtd_info *mtd, struct nand_chip *chip,
			    int page)
{
	struct lpc32xx_nand_host *host = chip->priv;

	/*                                                  */
	lpc32xx_read_page(mtd, chip, host->dummy_buf, 1, page);

	return 0;
}

static int lpc32xx_write_oob(struct mtd_info *mtd, struct nand_chip *chip,
			      int page)
{
	/*                                                                   */
	return 0;
}

/*                                                                        */
static void lpc32xx_ecc_enable(struct mtd_info *mtd, int mode)
{
	/*                 */
}

static int lpc32xx_dma_setup(struct lpc32xx_nand_host *host)
{
	struct mtd_info *mtd = &host->mtd;
	dma_cap_mask_t mask;

	if (!host->pdata || !host->pdata->dma_filter) {
		dev_err(mtd->dev.parent, "no DMA platform data\n");
		return -ENOENT;
	}

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);
	host->dma_chan = dma_request_channel(mask, host->pdata->dma_filter,
					     "nand-mlc");
	if (!host->dma_chan) {
		dev_err(mtd->dev.parent, "Failed to request DMA channel\n");
		return -EBUSY;
	}

	/*
                                                                  
                                                                       
                                                       
  */
	host->dma_slave_config.direction = DMA_DEV_TO_MEM;
	host->dma_slave_config.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	host->dma_slave_config.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	host->dma_slave_config.src_maxburst = 128;
	host->dma_slave_config.dst_maxburst = 128;
	/*                                   */
	host->dma_slave_config.device_fc = false;
	host->dma_slave_config.src_addr = MLC_BUFF(host->io_base_phy);
	host->dma_slave_config.dst_addr = MLC_BUFF(host->io_base_phy);
	if (dmaengine_slave_config(host->dma_chan, &host->dma_slave_config)) {
		dev_err(mtd->dev.parent, "Failed to setup DMA slave\n");
		goto out1;
	}

	return 0;
out1:
	dma_release_channel(host->dma_chan);
	return -ENXIO;
}

static struct lpc32xx_nand_cfg_mlc *lpc32xx_parse_dt(struct device *dev)
{
	struct lpc32xx_nand_cfg_mlc *ncfg;
	struct device_node *np = dev->of_node;

	ncfg = devm_kzalloc(dev, sizeof(*ncfg), GFP_KERNEL);
	if (!ncfg) {
		dev_err(dev, "could not allocate memory for platform data\n");
		return NULL;
	}

	of_property_read_u32(np, "nxp,tcea-delay", &ncfg->tcea_delay);
	of_property_read_u32(np, "nxp,busy-delay", &ncfg->busy_delay);
	of_property_read_u32(np, "nxp,nand-ta", &ncfg->nand_ta);
	of_property_read_u32(np, "nxp,rd-high", &ncfg->rd_high);
	of_property_read_u32(np, "nxp,rd-low", &ncfg->rd_low);
	of_property_read_u32(np, "nxp,wr-high", &ncfg->wr_high);
	of_property_read_u32(np, "nxp,wr-low", &ncfg->wr_low);

	if (!ncfg->tcea_delay || !ncfg->busy_delay || !ncfg->nand_ta ||
	    !ncfg->rd_high || !ncfg->rd_low || !ncfg->wr_high ||
	    !ncfg->wr_low) {
		dev_err(dev, "chip parameters not specified correctly\n");
		return NULL;
	}

	ncfg->wp_gpio = of_get_named_gpio(np, "gpios", 0);

	return ncfg;
}

/*
                            
 */
static int lpc32xx_nand_probe(struct platform_device *pdev)
{
	struct lpc32xx_nand_host *host;
	struct mtd_info *mtd;
	struct nand_chip *nand_chip;
	struct resource *rc;
	int res;
	struct mtd_part_parser_data ppdata = {};

	/*                                                        */
	host = devm_kzalloc(&pdev->dev, sizeof(*host), GFP_KERNEL);
	if (!host) {
		dev_err(&pdev->dev, "failed to allocate device structure.\n");
		return -ENOMEM;
	}

	rc = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	host->io_base = devm_ioremap_resource(&pdev->dev, rc);
	if (IS_ERR(host->io_base))
		return PTR_ERR(host->io_base);
	
	host->io_base_phy = rc->start;

	mtd = &host->mtd;
	nand_chip = &host->nand_chip;
	if (pdev->dev.of_node)
		host->ncfg = lpc32xx_parse_dt(&pdev->dev);
	if (!host->ncfg) {
		dev_err(&pdev->dev,
			"Missing or bad NAND config from device tree\n");
		return -ENOENT;
	}
	if (host->ncfg->wp_gpio == -EPROBE_DEFER)
		return -EPROBE_DEFER;
	if (gpio_is_valid(host->ncfg->wp_gpio) &&
			gpio_request(host->ncfg->wp_gpio, "NAND WP")) {
		dev_err(&pdev->dev, "GPIO not available\n");
		return -EBUSY;
	}
	lpc32xx_wp_disable(host);

	host->pdata = pdev->dev.platform_data;

	nand_chip->priv = host;		/*                                  */
	mtd->priv = nand_chip;
	mtd->owner = THIS_MODULE;
	mtd->dev.parent = &pdev->dev;

	/*                */
	host->clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(host->clk)) {
		dev_err(&pdev->dev, "Clock initialization failure\n");
		res = -ENOENT;
		goto err_exit1;
	}
	clk_enable(host->clk);

	nand_chip->cmd_ctrl = lpc32xx_nand_cmd_ctrl;
	nand_chip->dev_ready = lpc32xx_nand_device_ready;
	nand_chip->chip_delay = 25; /*    */
	nand_chip->IO_ADDR_R = MLC_DATA(host->io_base);
	nand_chip->IO_ADDR_W = MLC_DATA(host->io_base);

	/*                      */
	lpc32xx_nand_setup(host);

	platform_set_drvdata(pdev, host);

	/*                              */
	nand_chip->ecc.hwctl = lpc32xx_ecc_enable;
	nand_chip->ecc.read_page_raw = lpc32xx_read_page;
	nand_chip->ecc.read_page = lpc32xx_read_page;
	nand_chip->ecc.write_page_raw = lpc32xx_write_page_lowlevel;
	nand_chip->ecc.write_page = lpc32xx_write_page_lowlevel;
	nand_chip->ecc.write_oob = lpc32xx_write_oob;
	nand_chip->ecc.read_oob = lpc32xx_read_oob;
	nand_chip->ecc.strength = 4;
	nand_chip->write_page = lpc32xx_write_page;
	nand_chip->waitfunc = lpc32xx_waitfunc;

	nand_chip->bbt_options = NAND_BBT_USE_FLASH | NAND_BBT_NO_OOB;
	nand_chip->bbt_td = &lpc32xx_nand_bbt;
	nand_chip->bbt_md = &lpc32xx_nand_bbt_mirror;

	/*                                                               
                                                                      
                                                         
                                            */
	mtd->bitflip_threshold = nand_chip->ecc.strength;

	if (use_dma) {
		res = lpc32xx_dma_setup(host);
		if (res) {
			res = -EIO;
			goto err_exit2;
		}
	}

	/*
                                            
                                                          
  */
	if (nand_scan_ident(mtd, 1, NULL)) {
		res = -ENXIO;
		goto err_exit3;
	}

	host->dma_buf = devm_kzalloc(&pdev->dev, mtd->writesize, GFP_KERNEL);
	if (!host->dma_buf) {
		dev_err(&pdev->dev, "Error allocating dma_buf memory\n");
		res = -ENOMEM;
		goto err_exit3;
	}

	host->dummy_buf = devm_kzalloc(&pdev->dev, mtd->writesize, GFP_KERNEL);
	if (!host->dummy_buf) {
		dev_err(&pdev->dev, "Error allocating dummy_buf memory\n");
		res = -ENOMEM;
		goto err_exit3;
	}

	nand_chip->ecc.mode = NAND_ECC_HW;
	nand_chip->ecc.size = mtd->writesize;
	nand_chip->ecc.layout = &lpc32xx_nand_oob;
	host->mlcsubpages = mtd->writesize / 512;

	/*                                  */
	readb(MLC_IRQ_SR(host->io_base));

	init_completion(&host->comp_nand);
	init_completion(&host->comp_controller);

	host->irq = platform_get_irq(pdev, 0);
	if ((host->irq < 0) || (host->irq >= NR_IRQS)) {
		dev_err(&pdev->dev, "failed to get platform irq\n");
		res = -EINVAL;
		goto err_exit3;
	}

	if (request_irq(host->irq, (irq_handler_t)&lpc3xxx_nand_irq,
			IRQF_TRIGGER_HIGH, DRV_NAME, host)) {
		dev_err(&pdev->dev, "Error requesting NAND IRQ\n");
		res = -ENXIO;
		goto err_exit3;
	}

	/*
                                                                       
                                                   
  */
	if (nand_scan_tail(mtd)) {
		res = -ENXIO;
		goto err_exit4;
	}

	mtd->name = DRV_NAME;

	ppdata.of_node = pdev->dev.of_node;
	res = mtd_device_parse_register(mtd, NULL, &ppdata, host->ncfg->parts,
					host->ncfg->num_parts);
	if (!res)
		return res;

	nand_release(mtd);

err_exit4:
	free_irq(host->irq, host);
err_exit3:
	if (use_dma)
		dma_release_channel(host->dma_chan);
err_exit2:
	clk_disable(host->clk);
	clk_put(host->clk);
	platform_set_drvdata(pdev, NULL);
err_exit1:
	lpc32xx_wp_enable(host);
	gpio_free(host->ncfg->wp_gpio);

	return res;
}

/*
                     
 */
static int lpc32xx_nand_remove(struct platform_device *pdev)
{
	struct lpc32xx_nand_host *host = platform_get_drvdata(pdev);
	struct mtd_info *mtd = &host->mtd;

	nand_release(mtd);
	free_irq(host->irq, host);
	if (use_dma)
		dma_release_channel(host->dma_chan);

	clk_disable(host->clk);
	clk_put(host->clk);
	platform_set_drvdata(pdev, NULL);

	lpc32xx_wp_enable(host);
	gpio_free(host->ncfg->wp_gpio);

	return 0;
}

#ifdef CONFIG_PM
static int lpc32xx_nand_resume(struct platform_device *pdev)
{
	struct lpc32xx_nand_host *host = platform_get_drvdata(pdev);

	/*                      */
	clk_enable(host->clk);

	/*                               */
	lpc32xx_nand_setup(host);

	/*                       */
	lpc32xx_wp_disable(host);

	return 0;
}

static int lpc32xx_nand_suspend(struct platform_device *pdev, pm_message_t pm)
{
	struct lpc32xx_nand_host *host = platform_get_drvdata(pdev);

	/*                                 */
	lpc32xx_wp_enable(host);

	/*               */
	clk_disable(host->clk);
	return 0;
}

#else
#define lpc32xx_nand_resume NULL
#define lpc32xx_nand_suspend NULL
#endif

static const struct of_device_id lpc32xx_nand_match[] = {
	{ .compatible = "nxp,lpc3220-mlc" },
	{ /*          */ },
};
MODULE_DEVICE_TABLE(of, lpc32xx_nand_match);

static struct platform_driver lpc32xx_nand_driver = {
	.probe		= lpc32xx_nand_probe,
	.remove		= lpc32xx_nand_remove,
	.resume		= lpc32xx_nand_resume,
	.suspend	= lpc32xx_nand_suspend,
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(lpc32xx_nand_match),
	},
};

module_platform_driver(lpc32xx_nand_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roland Stigge <stigge@antcom.de>");
MODULE_DESCRIPTION("NAND driver for the NXP LPC32XX MLC controller");
