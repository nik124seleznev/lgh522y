/*
 * drivers/net/phy/mdio_bus.c
 *
 * MDIO Bus interface
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

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/of_mdio.h>
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

/* 
                                                    
                                                                 
                                                        
  
                                                             
                        
 */
struct mii_bus *mdiobus_alloc_size(size_t size)
{
	struct mii_bus *bus;
	size_t aligned_size = ALIGN(sizeof(*bus), NETDEV_ALIGN);
	size_t alloc_size;

	/*                                               */
	if (size)
		alloc_size = aligned_size + size;
	else
		alloc_size = sizeof(*bus);

	bus = kzalloc(alloc_size, GFP_KERNEL);
	if (bus) {
		bus->state = MDIOBUS_ALLOCATED;
		if (size)
			bus->priv = (void *)bus + aligned_size;
	}

	return bus;
}
EXPORT_SYMBOL(mdiobus_alloc_size);

/* 
                                                    
                                                         
  
                                                               
                                          
 */
static void mdiobus_release(struct device *d)
{
	struct mii_bus *bus = to_mii_bus(d);
	BUG_ON(bus->state != MDIOBUS_RELEASED &&
	       /*                                                  */
	       bus->state != MDIOBUS_ALLOCATED);
	kfree(bus);
}

static struct class mdio_bus_class = {
	.name		= "mdio_bus",
	.dev_release	= mdiobus_release,
};

#if IS_ENABLED(CONFIG_OF_MDIO)
/*                                      */
static int of_mdio_bus_match(struct device *dev, const void *mdio_bus_np)
{
	return dev->of_node == mdio_bus_np;
}
/* 
                                                              
                                        
  
                                                           
  
                                                                   
                                                                  
                                         
  
 */
struct mii_bus *of_mdio_find_bus(struct device_node *mdio_bus_np)
{
	struct device *d;

	if (!mdio_bus_np)
		return NULL;

	d = class_find_device(&mdio_bus_class, NULL,  mdio_bus_np,
			      of_mdio_bus_match);

	return d ? to_mii_bus(d) : NULL;
}
EXPORT_SYMBOL(of_mdio_find_bus);
#endif

/* 
                                                                                 
                       
  
                                                               
                                                
  
                                        
 */
int mdiobus_register(struct mii_bus *bus)
{
	int i, err;

	if (NULL == bus || NULL == bus->name ||
			NULL == bus->read ||
			NULL == bus->write)
		return -EINVAL;

	BUG_ON(bus->state != MDIOBUS_ALLOCATED &&
	       bus->state != MDIOBUS_UNREGISTERED);

	bus->dev.parent = bus->parent;
	bus->dev.class = &mdio_bus_class;
	bus->dev.groups = NULL;
	dev_set_name(&bus->dev, "%s", bus->id);

	err = device_register(&bus->dev);
	if (err) {
		pr_err("mii_bus %s failed to register\n", bus->id);
		return -EINVAL;
	}

	mutex_init(&bus->mdio_lock);

	if (bus->reset)
		bus->reset(bus);

	for (i = 0; i < PHY_MAX_ADDR; i++) {
		if ((bus->phy_mask & (1 << i)) == 0) {
			struct phy_device *phydev;

			phydev = mdiobus_scan(bus, i);
			if (IS_ERR(phydev)) {
				err = PTR_ERR(phydev);
				goto error;
			}
		}
	}

	bus->state = MDIOBUS_REGISTERED;
	pr_info("%s: probed\n", bus->name);
	return 0;

error:
	while (--i >= 0) {
		if (bus->phy_map[i])
			device_unregister(&bus->phy_map[i]->dev);
	}
	device_del(&bus->dev);
	return err;
}
EXPORT_SYMBOL(mdiobus_register);

void mdiobus_unregister(struct mii_bus *bus)
{
	int i;

	BUG_ON(bus->state != MDIOBUS_REGISTERED);
	bus->state = MDIOBUS_UNREGISTERED;

	device_del(&bus->dev);
	for (i = 0; i < PHY_MAX_ADDR; i++) {
		if (bus->phy_map[i])
			device_unregister(&bus->phy_map[i]->dev);
		bus->phy_map[i] = NULL;
	}
}
EXPORT_SYMBOL(mdiobus_unregister);

/* 
                                       
                        
  
                                                                
                                                                     
                 
 */
void mdiobus_free(struct mii_bus *bus)
{
	/*
                                                     
  */
	if (bus->state == MDIOBUS_ALLOCATED) {
		kfree(bus);
		return;
	}

	BUG_ON(bus->state != MDIOBUS_UNREGISTERED);
	bus->state = MDIOBUS_RELEASED;

	put_device(&bus->dev);
}
EXPORT_SYMBOL(mdiobus_free);

struct phy_device *mdiobus_scan(struct mii_bus *bus, int addr)
{
	struct phy_device *phydev;
	int err;

	phydev = get_phy_device(bus, addr, false);
	if (IS_ERR(phydev) || phydev == NULL)
		return phydev;

	err = phy_device_register(phydev);
	if (err) {
		phy_device_free(phydev);
		return NULL;
	}

	return phydev;
}
EXPORT_SYMBOL(mdiobus_scan);

/* 
                                                                            
                           
                         
                                   
  
                                                   
                                                                 
                             
 */
int mdiobus_read(struct mii_bus *bus, int addr, u32 regnum)
{
	int retval;

	BUG_ON(in_interrupt());

	mutex_lock(&bus->mdio_lock);
	retval = bus->read(bus, addr, regnum);
	mutex_unlock(&bus->mdio_lock);

	return retval;
}
EXPORT_SYMBOL(mdiobus_read);

/* 
                                                                             
                           
                         
                                    
                                  
  
                                                   
                                                                 
                             
 */
int mdiobus_write(struct mii_bus *bus, int addr, u32 regnum, u16 val)
{
	int err;

	BUG_ON(in_interrupt());

	mutex_lock(&bus->mdio_lock);
	err = bus->write(bus, addr, regnum, val);
	mutex_unlock(&bus->mdio_lock);

	return err;
}
EXPORT_SYMBOL(mdiobus_write);

/* 
                                                                               
                          
                         
  
                                                                 
                                                          
 */
static int mdio_bus_match(struct device *dev, struct device_driver *drv)
{
	struct phy_device *phydev = to_phy_device(dev);
	struct phy_driver *phydrv = to_phy_driver(drv);

	if (of_driver_match_device(dev, drv))
		return 1;

	if (phydrv->match_phy_device)
		return phydrv->match_phy_device(phydev);

	return ((phydrv->phy_id & phydrv->phy_id_mask) ==
		(phydev->phy_id & phydrv->phy_id_mask));
}

#ifdef CONFIG_PM

static bool mdio_bus_phy_may_suspend(struct phy_device *phydev)
{
	struct device_driver *drv = phydev->dev.driver;
	struct phy_driver *phydrv = to_phy_driver(drv);
	struct net_device *netdev = phydev->attached_dev;

	if (!drv || !phydrv->suspend)
		return false;

	/*                                */
	if (!netdev)
		return true;

	/*
                                                              
                                                           
  */
	if (netdev->dev.parent && device_may_wakeup(netdev->dev.parent))
		return false;

	/*
                                                                
                                                                 
                     
  */
	if (device_may_wakeup(&netdev->dev))
		return false;

	return true;
}

static int mdio_bus_suspend(struct device *dev)
{
	struct phy_driver *phydrv = to_phy_driver(dev->driver);
	struct phy_device *phydev = to_phy_device(dev);

	/*
                                                                      
                                                                     
                                                                      
                       
  */
	if (phydev->attached_dev && phydev->adjust_link)
		phy_stop_machine(phydev);

	if (!mdio_bus_phy_may_suspend(phydev))
		return 0;

	return phydrv->suspend(phydev);
}

static int mdio_bus_resume(struct device *dev)
{
	struct phy_driver *phydrv = to_phy_driver(dev->driver);
	struct phy_device *phydev = to_phy_device(dev);
	int ret;

	if (!mdio_bus_phy_may_suspend(phydev))
		goto no_resume;

	ret = phydrv->resume(phydev);
	if (ret < 0)
		return ret;

no_resume:
	if (phydev->attached_dev && phydev->adjust_link)
		phy_start_machine(phydev, NULL);

	return 0;
}

static int mdio_bus_restore(struct device *dev)
{
	struct phy_device *phydev = to_phy_device(dev);
	struct net_device *netdev = phydev->attached_dev;
	int ret;

	if (!netdev)
		return 0;

	ret = phy_init_hw(phydev);
	if (ret < 0)
		return ret;

	/*                               */
	phydev->link = 0;
	phydev->state = PHY_UP;

	phy_start_machine(phydev, NULL);

	return 0;
}

static struct dev_pm_ops mdio_bus_pm_ops = {
	.suspend = mdio_bus_suspend,
	.resume = mdio_bus_resume,
	.freeze = mdio_bus_suspend,
	.thaw = mdio_bus_resume,
	.restore = mdio_bus_restore,
};

#define MDIO_BUS_PM_OPS (&mdio_bus_pm_ops)

#else

#define MDIO_BUS_PM_OPS NULL

#endif /*           */

static ssize_t
phy_id_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct phy_device *phydev = to_phy_device(dev);

	return sprintf(buf, "0x%.8lx\n", (unsigned long)phydev->phy_id);
}

static struct device_attribute mdio_dev_attrs[] = {
	__ATTR_RO(phy_id),
	__ATTR_NULL
};

struct bus_type mdio_bus_type = {
	.name		= "mdio_bus",
	.match		= mdio_bus_match,
	.pm		= MDIO_BUS_PM_OPS,
	.dev_attrs	= mdio_dev_attrs,
};
EXPORT_SYMBOL(mdio_bus_type);

int __init mdio_bus_init(void)
{
	int ret;

	ret = class_register(&mdio_bus_class);
	if (!ret) {
		ret = bus_register(&mdio_bus_type);
		if (ret)
			class_unregister(&mdio_bus_class);
	}

	return ret;
}

void mdio_bus_exit(void)
{
	class_unregister(&mdio_bus_class);
	bus_unregister(&mdio_bus_type);
}
