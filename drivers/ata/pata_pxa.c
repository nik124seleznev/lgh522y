/*
 * Generic PXA PATA driver
 *
 * Copyright (C) 2010 Marek Vasut <marek.vasut@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/ata.h>
#include <linux/libata.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/completion.h>

#include <scsi/scsi_host.h>

#include <mach/pxa2xx-regs.h>
#include <linux/platform_data/ata-pxa.h>
#include <mach/dma.h>

#define DRV_NAME	"pata_pxa"
#define DRV_VERSION	"0.1"

struct pata_pxa_data {
	uint32_t		dma_channel;
	struct pxa_dma_desc	*dma_desc;
	dma_addr_t		dma_desc_addr;
	uint32_t		dma_desc_id;

	/*                         */
	uint32_t		dma_io_addr;
	/*                            */
	uint32_t		dma_dreq;
	/*                         */
	uint32_t		dma_dcsr;

	struct completion	dma_done;
};

/*
                                                                               
                                                                            
 */
static void pxa_load_dmac(struct scatterlist *sg, struct ata_queued_cmd *qc)
{
	struct pata_pxa_data *pd = qc->ap->private_data;

	uint32_t cpu_len, seg_len;
	dma_addr_t cpu_addr;

	cpu_addr = sg_dma_address(sg);
	cpu_len = sg_dma_len(sg);

	do {
		seg_len = (cpu_len > 0x1000) ? 0x1000 : cpu_len;

		pd->dma_desc[pd->dma_desc_id].ddadr = pd->dma_desc_addr +
			((pd->dma_desc_id + 1) * sizeof(struct pxa_dma_desc));

		pd->dma_desc[pd->dma_desc_id].dcmd = DCMD_BURST32 |
					DCMD_WIDTH2 | (DCMD_LENGTH & seg_len);

		if (qc->tf.flags & ATA_TFLAG_WRITE) {
			pd->dma_desc[pd->dma_desc_id].dsadr = cpu_addr;
			pd->dma_desc[pd->dma_desc_id].dtadr = pd->dma_io_addr;
			pd->dma_desc[pd->dma_desc_id].dcmd |= DCMD_INCSRCADDR |
						DCMD_FLOWTRG;
		} else {
			pd->dma_desc[pd->dma_desc_id].dsadr = pd->dma_io_addr;
			pd->dma_desc[pd->dma_desc_id].dtadr = cpu_addr;
			pd->dma_desc[pd->dma_desc_id].dcmd |= DCMD_INCTRGADDR |
						DCMD_FLOWSRC;
		}

		cpu_len -= seg_len;
		cpu_addr += seg_len;
		pd->dma_desc_id++;

	} while (cpu_len);

	/*                   */
	if (seg_len & 0x1f)
		DALGN |= (1 << pd->dma_dreq);
}

/*
                                   
 */
static void pxa_qc_prep(struct ata_queued_cmd *qc)
{
	struct pata_pxa_data *pd = qc->ap->private_data;
	int si = 0;
	struct scatterlist *sg;

	if (!(qc->flags & ATA_QCFLAG_DMAMAP))
		return;

	pd->dma_desc_id = 0;

	DCSR(pd->dma_channel) = 0;
	DALGN &= ~(1 << pd->dma_dreq);

	for_each_sg(qc->sg, sg, qc->n_elem, si)
		pxa_load_dmac(sg, qc);

	pd->dma_desc[pd->dma_desc_id - 1].ddadr = DDADR_STOP;

	/*                                        */
	pd->dma_desc[pd->dma_desc_id - 1].dcmd |= DCMD_ENDIRQEN;

	DDADR(pd->dma_channel) = pd->dma_desc_addr;
	DRCMR(pd->dma_dreq) = DRCMR_MAPVLD | pd->dma_channel;

}

/*
                                                                              
                                                  
 */
static void pxa_bmdma_setup(struct ata_queued_cmd *qc)
{
	qc->ap->ops->sff_exec_command(qc->ap, &qc->tf);
}

/*
                            
 */
static void pxa_bmdma_start(struct ata_queued_cmd *qc)
{
	struct pata_pxa_data *pd = qc->ap->private_data;
	init_completion(&pd->dma_done);
	DCSR(pd->dma_channel) = DCSR_RUN;
}

/*
                                                                       
 */
static void pxa_bmdma_stop(struct ata_queued_cmd *qc)
{
	struct pata_pxa_data *pd = qc->ap->private_data;

	if ((DCSR(pd->dma_channel) & DCSR_RUN) &&
		wait_for_completion_timeout(&pd->dma_done, HZ))
		dev_err(qc->ap->dev, "Timeout waiting for DMA completion!");

	DCSR(pd->dma_channel) = 0;
}

/*
                                                                             
                                                              
 */
static unsigned char pxa_bmdma_status(struct ata_port *ap)
{
	struct pata_pxa_data *pd = ap->private_data;
	unsigned char ret = ATA_DMA_INTR;

	if (pd->dma_dcsr & DCSR_BUSERR)
		ret |= ATA_DMA_ERR;

	return ret;
}

/*
                                            
 */
static void pxa_irq_clear(struct ata_port *ap)
{
}

/*
                                                                           
                                    
 */
static int pxa_check_atapi_dma(struct ata_queued_cmd *qc)
{
	return -EOPNOTSUPP;
}

static struct scsi_host_template pxa_ata_sht = {
	ATA_BMDMA_SHT(DRV_NAME),
};

static struct ata_port_operations pxa_ata_port_ops = {
	.inherits		= &ata_bmdma_port_ops,
	.cable_detect		= ata_cable_40wire,

	.bmdma_setup		= pxa_bmdma_setup,
	.bmdma_start		= pxa_bmdma_start,
	.bmdma_stop		= pxa_bmdma_stop,
	.bmdma_status		= pxa_bmdma_status,

	.check_atapi_dma	= pxa_check_atapi_dma,

	.sff_irq_clear		= pxa_irq_clear,

	.qc_prep		= pxa_qc_prep,
};

/*
                         
 */
static void pxa_ata_dma_irq(int dma, void *port)
{
	struct ata_port *ap = port;
	struct pata_pxa_data *pd = ap->private_data;

	pd->dma_dcsr = DCSR(dma);
	DCSR(dma) = pd->dma_dcsr;

	if (pd->dma_dcsr & DCSR_STOPSTATE)
		complete(&pd->dma_done);
}

static int pxa_ata_probe(struct platform_device *pdev)
{
	struct ata_host *host;
	struct ata_port *ap;
	struct pata_pxa_data *data;
	struct resource *cmd_res;
	struct resource *ctl_res;
	struct resource *dma_res;
	struct resource *irq_res;
	struct pata_pxa_pdata *pdata = pdev->dev.platform_data;
	int ret = 0;

	/*
                                                    
                            
                            
                            
              
  */
	if (pdev->num_resources != 4) {
		dev_err(&pdev->dev, "invalid number of resources\n");
		return -EINVAL;
	}

	/*
                         
  */
	cmd_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (unlikely(cmd_res == NULL))
		return -EINVAL;

	/*
                         
  */
	ctl_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (unlikely(ctl_res == NULL))
		return -EINVAL;

	/*
                         
  */
	dma_res = platform_get_resource(pdev, IORESOURCE_DMA, 0);
	if (unlikely(dma_res == NULL))
		return -EINVAL;

	/*
           
  */
	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (unlikely(irq_res == NULL))
		return -EINVAL;

	/*
                     
  */
	host = ata_host_alloc(&pdev->dev, 1);
	if (!host)
		return -ENOMEM;

	ap		= host->ports[0];
	ap->ops		= &pxa_ata_port_ops;
	ap->pio_mask	= ATA_PIO4;
	ap->mwdma_mask	= ATA_MWDMA2;

	ap->ioaddr.cmd_addr	= devm_ioremap(&pdev->dev, cmd_res->start,
						resource_size(cmd_res));
	ap->ioaddr.ctl_addr	= devm_ioremap(&pdev->dev, ctl_res->start,
						resource_size(ctl_res));
	ap->ioaddr.bmdma_addr	= devm_ioremap(&pdev->dev, dma_res->start,
						resource_size(dma_res));

	/*
                           
  */
	ap->ioaddr.altstatus_addr = ap->ioaddr.ctl_addr;
	ap->ioaddr.data_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_DATA << pdata->reg_shift);
	ap->ioaddr.error_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_ERR << pdata->reg_shift);
	ap->ioaddr.feature_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_FEATURE << pdata->reg_shift);
	ap->ioaddr.nsect_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_NSECT << pdata->reg_shift);
	ap->ioaddr.lbal_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_LBAL << pdata->reg_shift);
	ap->ioaddr.lbam_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_LBAM << pdata->reg_shift);
	ap->ioaddr.lbah_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_LBAH << pdata->reg_shift);
	ap->ioaddr.device_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_DEVICE << pdata->reg_shift);
	ap->ioaddr.status_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_STATUS << pdata->reg_shift);
	ap->ioaddr.command_addr	= ap->ioaddr.cmd_addr +
					(ATA_REG_CMD << pdata->reg_shift);

	/*
                                                      
  */
	data = devm_kzalloc(&pdev->dev, sizeof(struct pata_pxa_data),
								GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	ap->private_data = data;
	data->dma_dreq = pdata->dma_dreq;
	data->dma_io_addr = dma_res->start;

	/*
                                          
  */
	data->dma_desc = dmam_alloc_coherent(&pdev->dev, PAGE_SIZE,
					&data->dma_desc_addr, GFP_KERNEL);
	if (!data->dma_desc)
		return -EINVAL;

	/*
                           
  */
	data->dma_channel = pxa_request_dma(DRV_NAME, DMA_PRIO_LOW,
						pxa_ata_dma_irq, ap);
	if (data->dma_channel < 0)
		return -EBUSY;

	/*
                                  
  */
	DCSR(data->dma_channel) = 0;

	/*
                         
  */
	ret = ata_host_activate(host, irq_res->start, ata_sff_interrupt,
				pdata->irq_flags, &pxa_ata_sht);
	if (ret)
		pxa_free_dma(data->dma_channel);

	return ret;
}

static int pxa_ata_remove(struct platform_device *pdev)
{
	struct ata_host *host = dev_get_drvdata(&pdev->dev);
	struct pata_pxa_data *data = host->ports[0]->private_data;

	pxa_free_dma(data->dma_channel);

	ata_host_detach(host);

	return 0;
}

static struct platform_driver pxa_ata_driver = {
	.probe		= pxa_ata_probe,
	.remove		= pxa_ata_remove,
	.driver		= {
		.name		= DRV_NAME,
		.owner		= THIS_MODULE,
	},
};

module_platform_driver(pxa_ata_driver);

MODULE_AUTHOR("Marek Vasut <marek.vasut@gmail.com>");
MODULE_DESCRIPTION("DMA-capable driver for PATA on PXA CPU");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
MODULE_ALIAS("platform:" DRV_NAME);