/*
 * OHCI HCD (Host Controller Driver) for USB.
 *
 * (C) Copyright 1999 Roman Weissgaerber <weissg@vienna.at>
 * (C) Copyright 2000-2002 David Brownell <dbrownell@users.sourceforge.net>
 * (C) Copyright 2002 Hewlett-Packard Company
 *
 * Bus Glue for pxa27x
 *
 * Written by Christopher Hoover <ch@hpl.hp.com>
 * Based on fragments of previous driver by Russell King et al.
 *
 * Modified for LH7A404 from ohci-sa1111.c
 *  by Durgesh Pattamatta <pattamattad@sharpsec.com>
 *
 * Modified for pxa27x from ohci-lh7a404.c
 *  by Nick Bane <nick@cecomputing.co.uk> 26-8-2004
 *
 * This file is licenced under the GPL.
 */

#include <linux/device.h>
#include <linux/signal.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <mach/hardware.h>
#include <linux/platform_data/usb-ohci-pxa27x.h>
#include <linux/platform_data/usb-pxa3xx-ulpi.h>

/*
                                                            
 */
#define UHCREV		(0x0000) /*                       */
#define UHCHCON		(0x0004) /*                           */
#define UHCCOMS		(0x0008) /*                             */
#define UHCINTS		(0x000C) /*                               */
#define UHCINTE		(0x0010) /*                      */
#define UHCINTD		(0x0014) /*                       */
#define UHCHCCA		(0x0018) /*                                */
#define UHCPCED		(0x001C) /*                                */
#define UHCCHED		(0x0020) /*                              */
#define UHCCCED		(0x0024) /*                                 */
#define UHCBHED		(0x0028) /*                           */
#define UHCBCED		(0x002C) /*                              */
#define UHCDHEAD	(0x0030) /*               */
#define UHCFMI		(0x0034) /*                    */
#define UHCFMR		(0x0038) /*                     */
#define UHCFMN		(0x003C) /*                  */
#define UHCPERS		(0x0040) /*                    */
#define UHCLS		(0x0044) /*                         */

#define UHCRHDA		(0x0048) /*                           */
#define UHCRHDA_NOCP	(1 << 12)	/*                            */
#define UHCRHDA_OCPM	(1 << 11)	/*                              */
#define UHCRHDA_POTPGT(x) \
			(((x) & 0xff) << 24) /*                             */

#define UHCRHDB		(0x004C) /*                           */
#define UHCRHS		(0x0050) /*                     */
#define UHCRHPS1	(0x0054) /*                            */
#define UHCRHPS2	(0x0058) /*                            */
#define UHCRHPS3	(0x005C) /*                            */

#define UHCSTAT		(0x0060) /*                     */
#define UHCSTAT_UPS3	(1 << 16)	/*                       */
#define UHCSTAT_SBMAI	(1 << 15)	/*                                  */
#define UHCSTAT_SBTAI	(1 << 14)	/*                                  */
#define UHCSTAT_UPRI	(1 << 13)	/*                           */
#define UHCSTAT_UPS2	(1 << 12)	/*                        */
#define UHCSTAT_UPS1	(1 << 11)	/*                        */
#define UHCSTAT_HTA	(1 << 10)	/*                  */
#define UHCSTAT_HBA	(1 << 8)	/*                   */
#define UHCSTAT_RWUE	(1 << 7)	/*                          */

#define UHCHR           (0x0064) /*                    */
#define UHCHR_SSEP3	(1 << 11)	/*                                */
#define UHCHR_SSEP2	(1 << 10)	/*                                */
#define UHCHR_SSEP1	(1 << 9)	/*                                */
#define UHCHR_PCPL	(1 << 7)	/*                            */
#define UHCHR_PSPL	(1 << 6)	/*                          */
#define UHCHR_SSE	(1 << 5)	/*                      */
#define UHCHR_UIT	(1 << 4)	/*                    */
#define UHCHR_SSDC	(1 << 3)	/*                             */
#define UHCHR_CGR	(1 << 2)	/*                        */
#define UHCHR_FHR	(1 << 1)	/*                             */
#define UHCHR_FSBIR	(1 << 0)	/*                              */

#define UHCHIE          (0x0068) /*                              */
#define UHCHIE_UPS3IE	(1 << 14)	/*                         */
#define UHCHIE_UPRIE	(1 << 13)	/*                   */
#define UHCHIE_UPS2IE	(1 << 12)	/*                         */
#define UHCHIE_UPS1IE	(1 << 11)	/*                         */
#define UHCHIE_TAIE	(1 << 10)	/*                             
                        */
#define UHCHIE_HBAIE	(1 << 8)	/*                         */
#define UHCHIE_RWIE	(1 << 7)	/*                      */

#define UHCHIT          (0x006C) /*                             */

#define PXA_UHC_MAX_PORTNUM    3

struct pxa27x_ohci {
	/*                                                   */
	struct ohci_hcd ohci;

	struct device	*dev;
	struct clk	*clk;
	void __iomem	*mmio_base;
};

#define to_pxa27x_ohci(hcd)	(struct pxa27x_ohci *)hcd_to_ohci(hcd)

/*
                                              
                                     

                                              
                                             

                                                 
                                     
 */
static int pxa27x_ohci_select_pmm(struct pxa27x_ohci *ohci, int mode)
{
	uint32_t uhcrhda = __raw_readl(ohci->mmio_base + UHCRHDA);
	uint32_t uhcrhdb = __raw_readl(ohci->mmio_base + UHCRHDB);

	switch (mode) {
	case PMM_NPS_MODE:
		uhcrhda |= RH_A_NPS;
		break;
	case PMM_GLOBAL_MODE:
		uhcrhda &= ~(RH_A_NPS & RH_A_PSM);
		break;
	case PMM_PERPORT_MODE:
		uhcrhda &= ~(RH_A_NPS);
		uhcrhda |= RH_A_PSM;

		/*                                                 */
		uhcrhdb |= (0x7<<17);
		break;
	default:
		printk( KERN_ERR
			"Invalid mode %d, set to non-power switch mode.\n",
			mode );

		uhcrhda |= RH_A_NPS;
	}

	__raw_writel(uhcrhda, ohci->mmio_base + UHCRHDA);
	__raw_writel(uhcrhdb, ohci->mmio_base + UHCRHDB);
	return 0;
}

extern int usb_disabled(void);

/*                                                                         */

static inline void pxa27x_setup_hc(struct pxa27x_ohci *ohci,
				   struct pxaohci_platform_data *inf)
{
	uint32_t uhchr = __raw_readl(ohci->mmio_base + UHCHR);
	uint32_t uhcrhda = __raw_readl(ohci->mmio_base + UHCRHDA);

	if (inf->flags & ENABLE_PORT1)
		uhchr &= ~UHCHR_SSEP1;

	if (inf->flags & ENABLE_PORT2)
		uhchr &= ~UHCHR_SSEP2;

	if (inf->flags & ENABLE_PORT3)
		uhchr &= ~UHCHR_SSEP3;

	if (inf->flags & POWER_CONTROL_LOW)
		uhchr |= UHCHR_PCPL;

	if (inf->flags & POWER_SENSE_LOW)
		uhchr |= UHCHR_PSPL;

	if (inf->flags & NO_OC_PROTECTION)
		uhcrhda |= UHCRHDA_NOCP;
	else
		uhcrhda &= ~UHCRHDA_NOCP;

	if (inf->flags & OC_MODE_PERPORT)
		uhcrhda |= UHCRHDA_OCPM;
	else
		uhcrhda &= ~UHCRHDA_OCPM;

	if (inf->power_on_delay) {
		uhcrhda &= ~UHCRHDA_POTPGT(0xff);
		uhcrhda |= UHCRHDA_POTPGT(inf->power_on_delay / 2);
	}

	__raw_writel(uhchr, ohci->mmio_base + UHCHR);
	__raw_writel(uhcrhda, ohci->mmio_base + UHCRHDA);
}

static inline void pxa27x_reset_hc(struct pxa27x_ohci *ohci)
{
	uint32_t uhchr = __raw_readl(ohci->mmio_base + UHCHR);

	__raw_writel(uhchr | UHCHR_FHR, ohci->mmio_base + UHCHR);
	udelay(11);
	__raw_writel(uhchr & ~UHCHR_FHR, ohci->mmio_base + UHCHR);
}

#ifdef CONFIG_PXA27x
extern void pxa27x_clear_otgph(void);
#else
#define pxa27x_clear_otgph()	do {} while (0)
#endif

static int pxa27x_start_hc(struct pxa27x_ohci *ohci, struct device *dev)
{
	int retval = 0;
	struct pxaohci_platform_data *inf;
	uint32_t uhchr;

	inf = dev->platform_data;

	clk_prepare_enable(ohci->clk);

	pxa27x_reset_hc(ohci);

	uhchr = __raw_readl(ohci->mmio_base + UHCHR) | UHCHR_FSBIR;
	__raw_writel(uhchr, ohci->mmio_base + UHCHR);

	while (__raw_readl(ohci->mmio_base + UHCHR) & UHCHR_FSBIR)
		cpu_relax();

	pxa27x_setup_hc(ohci, inf);

	if (inf->init)
		retval = inf->init(dev);

	if (retval < 0)
		return retval;

	if (cpu_is_pxa3xx())
		pxa3xx_u2d_start_hc(&ohci_to_hcd(&ohci->ohci)->self);

	uhchr = __raw_readl(ohci->mmio_base + UHCHR) & ~UHCHR_SSE;
	__raw_writel(uhchr, ohci->mmio_base + UHCHR);
	__raw_writel(UHCHIE_UPRIE | UHCHIE_RWIE, ohci->mmio_base + UHCHIE);

	/*                        */
	pxa27x_clear_otgph();
	return 0;
}

static void pxa27x_stop_hc(struct pxa27x_ohci *ohci, struct device *dev)
{
	struct pxaohci_platform_data *inf;
	uint32_t uhccoms;

	inf = dev->platform_data;

	if (cpu_is_pxa3xx())
		pxa3xx_u2d_stop_hc(&ohci_to_hcd(&ohci->ohci)->self);

	if (inf->exit)
		inf->exit(dev);

	pxa27x_reset_hc(ohci);

	/*                       */
	uhccoms = __raw_readl(ohci->mmio_base + UHCCOMS) | 0x01;
	__raw_writel(uhccoms, ohci->mmio_base + UHCCOMS);
	udelay(10);

	clk_disable_unprepare(ohci->clk);
}

#ifdef CONFIG_OF
static const struct of_device_id pxa_ohci_dt_ids[] = {
	{ .compatible = "marvell,pxa-ohci" },
	{ }
};

MODULE_DEVICE_TABLE(of, pxa_ohci_dt_ids);

static int ohci_pxa_of_init(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct pxaohci_platform_data *pdata;
	u32 tmp;

	if (!np)
		return 0;

	/*                                                             
                                                            
                                                          
  */
	if (!pdev->dev.dma_mask)
		pdev->dev.dma_mask = &pdev->dev.coherent_dma_mask;
	if (!pdev->dev.coherent_dma_mask)
		pdev->dev.coherent_dma_mask = DMA_BIT_MASK(32);

	pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	if (of_get_property(np, "marvell,enable-port1", NULL))
		pdata->flags |= ENABLE_PORT1;
	if (of_get_property(np, "marvell,enable-port2", NULL))
		pdata->flags |= ENABLE_PORT2;
	if (of_get_property(np, "marvell,enable-port3", NULL))
		pdata->flags |= ENABLE_PORT3;
	if (of_get_property(np, "marvell,port-sense-low", NULL))
		pdata->flags |= POWER_SENSE_LOW;
	if (of_get_property(np, "marvell,power-control-low", NULL))
		pdata->flags |= POWER_CONTROL_LOW;
	if (of_get_property(np, "marvell,no-oc-protection", NULL))
		pdata->flags |= NO_OC_PROTECTION;
	if (of_get_property(np, "marvell,oc-mode-perport", NULL))
		pdata->flags |= OC_MODE_PERPORT;
	if (!of_property_read_u32(np, "marvell,power-on-delay", &tmp))
		pdata->power_on_delay = tmp;
	if (!of_property_read_u32(np, "marvell,port-mode", &tmp))
		pdata->port_mode = tmp;
	if (!of_property_read_u32(np, "marvell,power-budget", &tmp))
		pdata->power_budget = tmp;

	pdev->dev.platform_data = pdata;

	return 0;
}
#else
static int ohci_pxa_of_init(struct platform_device *pdev)
{
	return 0;
}
#endif

/*                                                                         */

/*                                                      */
/*                                                    */


/* 
                                                      
                           
  
                                                              
                                                                 
                                           
  
 */
int usb_hcd_pxa27x_probe (const struct hc_driver *driver, struct platform_device *pdev)
{
	int retval, irq;
	struct usb_hcd *hcd;
	struct pxaohci_platform_data *inf;
	struct pxa27x_ohci *ohci;
	struct resource *r;
	struct clk *usb_clk;

	retval = ohci_pxa_of_init(pdev);
	if (retval)
		return retval;

	inf = pdev->dev.platform_data;

	if (!inf)
		return -ENODEV;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		pr_err("no resource of IORESOURCE_IRQ");
		return -ENXIO;
	}

	usb_clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(usb_clk))
		return PTR_ERR(usb_clk);

	hcd = usb_create_hcd (driver, &pdev->dev, "pxa27x");
	if (!hcd) {
		retval = -ENOMEM;
		goto err0;
	}

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r) {
		pr_err("no resource of IORESOURCE_MEM");
		retval = -ENXIO;
		goto err1;
	}

	hcd->rsrc_start = r->start;
	hcd->rsrc_len = resource_size(r);

	if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
		pr_debug("request_mem_region failed");
		retval = -EBUSY;
		goto err1;
	}

	hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
	if (!hcd->regs) {
		pr_debug("ioremap failed");
		retval = -ENOMEM;
		goto err2;
	}

	/*                                 */
	ohci = (struct pxa27x_ohci *)hcd_to_ohci(hcd);
	ohci->dev = &pdev->dev;
	ohci->clk = usb_clk;
	ohci->mmio_base = (void __iomem *)hcd->regs;

	if ((retval = pxa27x_start_hc(ohci, &pdev->dev)) < 0) {
		pr_debug("pxa27x_start_hc failed");
		goto err3;
	}

	/*                              */
	pxa27x_ohci_select_pmm(ohci, inf->port_mode);

	if (inf->power_budget)
		hcd->power_budget = inf->power_budget;

	ohci_hcd_init(hcd_to_ohci(hcd));

	retval = usb_add_hcd(hcd, irq, 0);
	if (retval == 0)
		return retval;

	pxa27x_stop_hc(ohci, &pdev->dev);
 err3:
	iounmap(hcd->regs);
 err2:
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
 err1:
	usb_put_hcd(hcd);
 err0:
	clk_put(usb_clk);
	return retval;
}


/*                                                       */
/*                                                        */

/* 
                                                                    
                                          
                           
  
                                                                
                                                              
                                                           
  
 */
void usb_hcd_pxa27x_remove (struct usb_hcd *hcd, struct platform_device *pdev)
{
	struct pxa27x_ohci *ohci = to_pxa27x_ohci(hcd);

	usb_remove_hcd(hcd);
	pxa27x_stop_hc(ohci, &pdev->dev);
	iounmap(hcd->regs);
	release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
	usb_put_hcd(hcd);
	clk_put(ohci->clk);
}

/*                                                                         */

static int
ohci_pxa27x_start (struct usb_hcd *hcd)
{
	struct ohci_hcd	*ohci = hcd_to_ohci (hcd);
	int		ret;

	ohci_dbg (ohci, "ohci_pxa27x_start, ohci:%p", ohci);

	/*                                                             */
	ohci->num_ports = 3;

	if ((ret = ohci_init(ohci)) < 0)
		return ret;

	if ((ret = ohci_run (ohci)) < 0) {
		dev_err(hcd->self.controller, "can't start %s",
			hcd->self.bus_name);
		ohci_stop (hcd);
		return ret;
	}

	return 0;
}

/*                                                                         */

static const struct hc_driver ohci_pxa27x_hc_driver = {
	.description =		hcd_name,
	.product_desc =		"PXA27x OHCI",
	.hcd_priv_size =	sizeof(struct pxa27x_ohci),

	/*
                            
  */
	.irq =			ohci_irq,
	.flags =		HCD_USB11 | HCD_MEMORY,

	/*
                              
  */
	.start =		ohci_pxa27x_start,
	.stop =			ohci_stop,
	.shutdown =		ohci_shutdown,

	/*
                                                         
  */
	.urb_enqueue =		ohci_urb_enqueue,
	.urb_dequeue =		ohci_urb_dequeue,
	.endpoint_disable =	ohci_endpoint_disable,

	/*
                      
  */
	.get_frame_number =	ohci_get_frame,

	/*
                    
  */
	.hub_status_data =	ohci_hub_status_data,
	.hub_control =		ohci_hub_control,
#ifdef  CONFIG_PM
	.bus_suspend =		ohci_bus_suspend,
	.bus_resume =		ohci_bus_resume,
#endif
	.start_port_reset =	ohci_start_port_reset,
};

/*                                                                         */

static int ohci_hcd_pxa27x_drv_probe(struct platform_device *pdev)
{
	pr_debug ("In ohci_hcd_pxa27x_drv_probe");

	if (usb_disabled())
		return -ENODEV;

	return usb_hcd_pxa27x_probe(&ohci_pxa27x_hc_driver, pdev);
}

static int ohci_hcd_pxa27x_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_hcd_pxa27x_remove(hcd, pdev);
	platform_set_drvdata(pdev, NULL);
	return 0;
}

#ifdef CONFIG_PM
static int ohci_hcd_pxa27x_drv_suspend(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct pxa27x_ohci *ohci = to_pxa27x_ohci(hcd);

	if (time_before(jiffies, ohci->ohci.next_statechange))
		msleep(5);
	ohci->ohci.next_statechange = jiffies;

	pxa27x_stop_hc(ohci, dev);
	return 0;
}

static int ohci_hcd_pxa27x_drv_resume(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct pxa27x_ohci *ohci = to_pxa27x_ohci(hcd);
	struct pxaohci_platform_data *inf = dev->platform_data;
	int status;

	if (time_before(jiffies, ohci->ohci.next_statechange))
		msleep(5);
	ohci->ohci.next_statechange = jiffies;

	if ((status = pxa27x_start_hc(ohci, dev)) < 0)
		return status;

	/*                              */
	pxa27x_ohci_select_pmm(ohci, inf->port_mode);

	ohci_resume(hcd, false);
	return 0;
}

static const struct dev_pm_ops ohci_hcd_pxa27x_pm_ops = {
	.suspend	= ohci_hcd_pxa27x_drv_suspend,
	.resume		= ohci_hcd_pxa27x_drv_resume,
};
#endif

/*                                */
MODULE_ALIAS("platform:pxa27x-ohci");

static struct platform_driver ohci_hcd_pxa27x_driver = {
	.probe		= ohci_hcd_pxa27x_drv_probe,
	.remove		= ohci_hcd_pxa27x_drv_remove,
	.shutdown	= usb_hcd_platform_shutdown,
	.driver		= {
		.name	= "pxa27x-ohci",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(pxa_ohci_dt_ids),
#ifdef CONFIG_PM
		.pm	= &ohci_hcd_pxa27x_pm_ops,
#endif
	},
};

