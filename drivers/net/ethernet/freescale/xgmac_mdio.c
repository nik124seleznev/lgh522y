/*
 * QorIQ 10G MDIO Controller
 *
 * Copyright 2012 Freescale Semiconductor, Inc.
 *
 * Authors: Andy Fleming <afleming@freescale.com>
 *          Timur Tabi <timur@freescale.com>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/mdio.h>
#include <linux/of_platform.h>
#include <linux/of_mdio.h>

/*                                                          */
#define TIMEOUT	1000

struct tgec_mdio_controller {
	__be32	reserved[12];
	__be32	mdio_stat;	/*                               */
	__be32	mdio_ctl;	/*              */
	__be32	mdio_data;	/*           */
	__be32	mdio_addr;	/*              */
} __packed;

#define MDIO_STAT_CLKDIV(x)	(((x>>1) & 0xff) << 8)
#define MDIO_STAT_BSY		(1 << 0)
#define MDIO_STAT_RD_ER		(1 << 1)
#define MDIO_CTL_DEV_ADDR(x) 	(x & 0x1f)
#define MDIO_CTL_PORT_ADDR(x)	((x & 0x1f) << 5)
#define MDIO_CTL_PRE_DIS	(1 << 10)
#define MDIO_CTL_SCAN_EN	(1 << 11)
#define MDIO_CTL_POST_INC	(1 << 14)
#define MDIO_CTL_READ		(1 << 15)

#define MDIO_DATA(x)		(x & 0xffff)
#define MDIO_DATA_BSY		(1 << 31)

/*
                                   
 */
static int xgmac_wait_until_free(struct device *dev,
				 struct tgec_mdio_controller __iomem *regs)
{
	uint32_t status;

	/*                           */
	status = spin_event_timeout(
		!((in_be32(&regs->mdio_stat)) & MDIO_STAT_BSY), TIMEOUT, 0);
	if (!status) {
		dev_err(dev, "timeout waiting for bus to be free\n");
		return -ETIMEDOUT;
	}

	return 0;
}

/*
                                                         
 */
static int xgmac_wait_until_done(struct device *dev,
				 struct tgec_mdio_controller __iomem *regs)
{
	uint32_t status;

	/*                                      */
	status = spin_event_timeout(
		!((in_be32(&regs->mdio_data)) & MDIO_DATA_BSY), TIMEOUT, 0);
	if (!status) {
		dev_err(dev, "timeout waiting for operation to complete\n");
		return -ETIMEDOUT;
	}

	return 0;
}

/*
                                                                           
                                                                              
                                    
 */
static int xgmac_mdio_write(struct mii_bus *bus, int phy_id, int regnum, u16 value)
{
	struct tgec_mdio_controller __iomem *regs = bus->priv;
	uint16_t dev_addr = regnum >> 16;
	int ret;

	/*                                */
	out_be32(&regs->mdio_stat, MDIO_STAT_CLKDIV(100));

	ret = xgmac_wait_until_free(&bus->dev, regs);
	if (ret)
		return ret;

	/*                           */
	out_be32(&regs->mdio_ctl,
		 MDIO_CTL_PORT_ADDR(phy_id) | MDIO_CTL_DEV_ADDR(dev_addr));

	/*                          */
	out_be32(&regs->mdio_addr, regnum & 0xffff);

	ret = xgmac_wait_until_free(&bus->dev, regs);
	if (ret)
		return ret;

	/*                                 */
	out_be32(&regs->mdio_data, MDIO_DATA(value));

	ret = xgmac_wait_until_done(&bus->dev, regs);
	if (ret)
		return ret;

	return 0;
}

/*
                                                                             
                                                                          
                   
 */
static int xgmac_mdio_read(struct mii_bus *bus, int phy_id, int regnum)
{
	struct tgec_mdio_controller __iomem *regs = bus->priv;
	uint16_t dev_addr = regnum >> 16;
	uint32_t mdio_ctl;
	uint16_t value;
	int ret;

	/*                                */
	out_be32(&regs->mdio_stat, MDIO_STAT_CLKDIV(100));

	ret = xgmac_wait_until_free(&bus->dev, regs);
	if (ret)
		return ret;

	/*                               */
	mdio_ctl = MDIO_CTL_PORT_ADDR(phy_id) | MDIO_CTL_DEV_ADDR(dev_addr);
	out_be32(&regs->mdio_ctl, mdio_ctl);

	/*                          */
	out_be32(&regs->mdio_addr, regnum & 0xffff);

	ret = xgmac_wait_until_free(&bus->dev, regs);
	if (ret)
		return ret;

	/*                   */
	out_be32(&regs->mdio_ctl, mdio_ctl | MDIO_CTL_READ);

	ret = xgmac_wait_until_done(&bus->dev, regs);
	if (ret)
		return ret;

	/*                                    */
	if (in_be32(&regs->mdio_stat) & MDIO_STAT_RD_ER) {
		dev_err(&bus->dev, "MDIO read error\n");
		return 0xffff;
	}

	value = in_be32(&regs->mdio_data) & 0xffff;
	dev_dbg(&bus->dev, "read %04x\n", value);

	return value;
}

/*                                                        */
static int xgmac_mdio_reset(struct mii_bus *bus)
{
	struct tgec_mdio_controller __iomem *regs = bus->priv;
	int ret;

	mutex_lock(&bus->mdio_lock);

	/*                                */
	out_be32(&regs->mdio_stat, MDIO_STAT_CLKDIV(100));

	ret = xgmac_wait_until_free(&bus->dev, regs);

	mutex_unlock(&bus->mdio_lock);

	return ret;
}

static int xgmac_mdio_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct mii_bus *bus;
	struct resource res;
	int ret;

	ret = of_address_to_resource(np, 0, &res);
	if (ret) {
		dev_err(&pdev->dev, "could not obtain address\n");
		return ret;
	}

	bus = mdiobus_alloc_size(PHY_MAX_ADDR * sizeof(int));
	if (!bus)
		return -ENOMEM;

	bus->name = "Freescale XGMAC MDIO Bus";
	bus->read = xgmac_mdio_read;
	bus->write = xgmac_mdio_write;
	bus->reset = xgmac_mdio_reset;
	bus->irq = bus->priv;
	bus->parent = &pdev->dev;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%llx", (unsigned long long)res.start);

	/*                          */
	bus->priv = of_iomap(np, 0);
	if (!bus->priv) {
		ret = -ENOMEM;
		goto err_ioremap;
	}

	ret = of_mdiobus_register(bus, np);
	if (ret) {
		dev_err(&pdev->dev, "cannot register MDIO bus\n");
		goto err_registration;
	}

	dev_set_drvdata(&pdev->dev, bus);

	return 0;

err_registration:
	iounmap(bus->priv);

err_ioremap:
	mdiobus_free(bus);

	return ret;
}

static int xgmac_mdio_remove(struct platform_device *pdev)
{
	struct mii_bus *bus = dev_get_drvdata(&pdev->dev);

	mdiobus_unregister(bus);
	iounmap(bus->priv);
	mdiobus_free(bus);

	return 0;
}

static struct of_device_id xgmac_mdio_match[] = {
	{
		.compatible = "fsl,fman-xmdio",
	},
	{},
};
MODULE_DEVICE_TABLE(of, xgmac_mdio_match);

static struct platform_driver xgmac_mdio_driver = {
	.driver = {
		.name = "fsl-fman_xmdio",
		.of_match_table = xgmac_mdio_match,
	},
	.probe = xgmac_mdio_probe,
	.remove = xgmac_mdio_remove,
};

module_platform_driver(xgmac_mdio_driver);

MODULE_DESCRIPTION("Freescale QorIQ 10G MDIO Controller");
MODULE_LICENSE("GPL v2");
