/*
 * drivers/net/phy/lxt.c
 *
 * Driver for Intel LXT PHYs
 *
 * Author: Andy Fleming
 *
 * Copyright (c) 2004 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

/*                                                     */

#define MII_LXT970_IER       17  /*                           */

#define MII_LXT970_IER_IEN	0x0002

#define MII_LXT970_ISR       18  /*                           */

#define MII_LXT970_CONFIG    19  /*                           */

/*                                                                           */
/*                                                                           */

/*                                  */
#define MII_LXT971_IER		18  /*                           */
#define MII_LXT971_IER_IEN	0x00f2

#define MII_LXT971_ISR		19  /*                           */

/*                                  */
#define MII_LXT973_PCR 16 /*                             */
#define PCR_FIBER_SELECT 1

MODULE_DESCRIPTION("Intel LXT PHY driver");
MODULE_AUTHOR("Andy Fleming");
MODULE_LICENSE("GPL");

static int lxt970_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = phy_read(phydev, MII_BMSR);

	if (err < 0)
		return err;

	err = phy_read(phydev, MII_LXT970_ISR);

	if (err < 0)
		return err;

	return 0;
}

static int lxt970_config_intr(struct phy_device *phydev)
{
	int err;

	if(phydev->interrupts == PHY_INTERRUPT_ENABLED)
		err = phy_write(phydev, MII_LXT970_IER, MII_LXT970_IER_IEN);
	else
		err = phy_write(phydev, MII_LXT970_IER, 0);

	return err;
}

static int lxt970_config_init(struct phy_device *phydev)
{
	int err;

	err = phy_write(phydev, MII_LXT970_CONFIG, 0);

	return err;
}


static int lxt971_ack_interrupt(struct phy_device *phydev)
{
	int err = phy_read(phydev, MII_LXT971_ISR);

	if (err < 0)
		return err;

	return 0;
}

static int lxt971_config_intr(struct phy_device *phydev)
{
	int err;

	if(phydev->interrupts == PHY_INTERRUPT_ENABLED)
		err = phy_write(phydev, MII_LXT971_IER, MII_LXT971_IER_IEN);
	else
		err = phy_write(phydev, MII_LXT971_IER, 0);

	return err;
}

/*
                                                                           
                                                                       
 */

static int lxt973a2_update_link(struct phy_device *phydev)
{
	int status;
	int control;
	int retry = 8; /*                */

	/*                */
	status = phy_read(phydev, MII_BMSR);

	if (status < 0)
		return status;

	control = phy_read(phydev, MII_BMCR);
	if (control < 0)
		return control;

	do {
		/*                                      */
		status = phy_read(phydev, MII_BMSR);
	} while (status >= 0 && retry-- && status == control);

	if (status < 0)
		return status;

	if ((status & BMSR_LSTATUS) == 0)
		phydev->link = 0;
	else
		phydev->link = 1;

	return 0;
}

static int lxt973a2_read_status(struct phy_device *phydev)
{
	int adv;
	int err;
	int lpa;
	int lpagb = 0;

	/*                                                   */
	err = lxt973a2_update_link(phydev);
	if (err)
		return err;

	if (AUTONEG_ENABLE == phydev->autoneg) {
		int retry = 1;

		adv = phy_read(phydev, MII_ADVERTISE);

		if (adv < 0)
			return adv;

		do {
			lpa = phy_read(phydev, MII_LPA);

			if (lpa < 0)
				return lpa;

			/*                                                   
                                
   */
		} while (lpa == adv && retry--);

		lpa &= adv;

		phydev->speed = SPEED_10;
		phydev->duplex = DUPLEX_HALF;
		phydev->pause = phydev->asym_pause = 0;

		if (lpagb & (LPA_1000FULL | LPA_1000HALF)) {
			phydev->speed = SPEED_1000;

			if (lpagb & LPA_1000FULL)
				phydev->duplex = DUPLEX_FULL;
		} else if (lpa & (LPA_100FULL | LPA_100HALF)) {
			phydev->speed = SPEED_100;

			if (lpa & LPA_100FULL)
				phydev->duplex = DUPLEX_FULL;
		} else {
			if (lpa & LPA_10FULL)
				phydev->duplex = DUPLEX_FULL;
		}

		if (phydev->duplex == DUPLEX_FULL) {
			phydev->pause = lpa & LPA_PAUSE_CAP ? 1 : 0;
			phydev->asym_pause = lpa & LPA_PAUSE_ASYM ? 1 : 0;
		}
	} else {
		int bmcr = phy_read(phydev, MII_BMCR);

		if (bmcr < 0)
			return bmcr;

		if (bmcr & BMCR_FULLDPLX)
			phydev->duplex = DUPLEX_FULL;
		else
			phydev->duplex = DUPLEX_HALF;

		if (bmcr & BMCR_SPEED1000)
			phydev->speed = SPEED_1000;
		else if (bmcr & BMCR_SPEED100)
			phydev->speed = SPEED_100;
		else
			phydev->speed = SPEED_10;

		phydev->pause = phydev->asym_pause = 0;
	}

	return 0;
}

static int lxt973_probe(struct phy_device *phydev)
{
	int val = phy_read(phydev, MII_LXT973_PCR);

	if (val & PCR_FIBER_SELECT) {
		/*
                                                        
                                                       
   */
		val = phy_read(phydev, MII_BMCR);
		val |= (BMCR_SPEED100 | BMCR_FULLDPLX);
		val &= ~BMCR_ANENABLE;
		phy_write(phydev, MII_BMCR, val);
		/*                                          */
		phydev->priv = lxt973_probe;
	} else {
		phydev->priv = NULL;
	}
	return 0;
}

static int lxt973_config_aneg(struct phy_device *phydev)
{
	/*                                      */
	return phydev->priv ? 0 : genphy_config_aneg(phydev);
}

static struct phy_driver lxt97x_driver[] = {
{
	.phy_id		= 0x78100000,
	.name		= "LXT970",
	.phy_id_mask	= 0xfffffff0,
	.features	= PHY_BASIC_FEATURES,
	.flags		= PHY_HAS_INTERRUPT,
	.config_init	= lxt970_config_init,
	.config_aneg	= genphy_config_aneg,
	.read_status	= genphy_read_status,
	.ack_interrupt	= lxt970_ack_interrupt,
	.config_intr	= lxt970_config_intr,
	.driver		= { .owner = THIS_MODULE,},
}, {
	.phy_id		= 0x001378e0,
	.name		= "LXT971",
	.phy_id_mask	= 0xfffffff0,
	.features	= PHY_BASIC_FEATURES,
	.flags		= PHY_HAS_INTERRUPT,
	.config_aneg	= genphy_config_aneg,
	.read_status	= genphy_read_status,
	.ack_interrupt	= lxt971_ack_interrupt,
	.config_intr	= lxt971_config_intr,
	.driver		= { .owner = THIS_MODULE,},
}, {
	.phy_id		= 0x00137a10,
	.name		= "LXT973-A2",
	.phy_id_mask	= 0xffffffff,
	.features	= PHY_BASIC_FEATURES,
	.flags		= 0,
	.probe		= lxt973_probe,
	.config_aneg	= lxt973_config_aneg,
	.read_status	= lxt973a2_read_status,
	.driver		= { .owner = THIS_MODULE,},
}, {
	.phy_id		= 0x00137a10,
	.name		= "LXT973",
	.phy_id_mask	= 0xfffffff0,
	.features	= PHY_BASIC_FEATURES,
	.flags		= 0,
	.probe		= lxt973_probe,
	.config_aneg	= lxt973_config_aneg,
	.read_status	= genphy_read_status,
	.driver		= { .owner = THIS_MODULE,},
} };

static int __init lxt_init(void)
{
	return phy_drivers_register(lxt97x_driver,
		ARRAY_SIZE(lxt97x_driver));
}

static void __exit lxt_exit(void)
{
	phy_drivers_unregister(lxt97x_driver,
		ARRAY_SIZE(lxt97x_driver));
}

module_init(lxt_init);
module_exit(lxt_exit);

static struct mdio_device_id __maybe_unused lxt_tbl[] = {
	{ 0x78100000, 0xfffffff0 },
	{ 0x001378e0, 0xfffffff0 },
	{ 0x00137a10, 0xfffffff0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, lxt_tbl);
