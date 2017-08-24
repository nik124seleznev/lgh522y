/*
 *	watchdog_core.c
 *
 *	(c) Copyright 2008-2011 Alan Cox <alan@lxorguk.ukuu.org.uk>,
 *						All Rights Reserved.
 *
 *	(c) Copyright 2008-2011 Wim Van Sebroeck <wim@iguana.be>.
 *
 *	This source code is part of the generic code that can be used
 *	by all the watchdog timer drivers.
 *
 *	Based on source code of the following authors:
 *	  Matt Domsch <Matt_Domsch@dell.com>,
 *	  Rob Radez <rob@osinvestor.com>,
 *	  Rusty Lynch <rusty@linux.co.intel.com>
 *	  Satyam Sharma <satyam@infradead.org>
 *	  Randy Dunlap <randy.dunlap@oracle.com>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 *	Neither Alan Cox, CymruNet Ltd., Wim Van Sebroeck nor Iguana vzw.
 *	admit liability nor provide warranty for any of this software.
 *	This material is provided "AS-IS" and at no charge.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>	/*                                    */
#include <linux/types.h>	/*                    */
#include <linux/errno.h>	/*                            */
#include <linux/kernel.h>	/*                      */
#include <linux/watchdog.h>	/*                             */
#include <linux/init.h>		/*                       */
#include <linux/idr.h>		/*                  */
#include <linux/err.h>		/*                   */
#include <linux/of.h>		/*                        */

#include "watchdog_core.h"	/*                               */

static DEFINE_IDA(watchdog_ida);
static struct class *watchdog_class;

static void watchdog_check_min_max_timeout(struct watchdog_device *wdd)
{
	/*
                                                           
                                                   
  */
	if (wdd->min_timeout > wdd->max_timeout) {
		pr_info("Invalid min and max timeout values, resetting to 0!\n");
		wdd->min_timeout = 0;
		wdd->max_timeout = 0;
	}
}

/* 
                                                         
                                          
                                                    
  
                                                                             
                                                                              
                                                                       
                                                                              
                                
  
                                                         
 */
int watchdog_init_timeout(struct watchdog_device *wdd,
				unsigned int timeout_parm, struct device *dev)
{
	unsigned int t = 0;
	int ret = 0;

	watchdog_check_min_max_timeout(wdd);

	/*                                               */
	if (!watchdog_timeout_invalid(wdd, timeout_parm)) {
		wdd->timeout = timeout_parm;
		return ret;
	}
	if (timeout_parm)
		ret = -EINVAL;

	/*                                     */
	if (dev == NULL || dev->of_node == NULL)
		return ret;
	of_property_read_u32(dev->of_node, "timeout-sec", &t);
	if (!watchdog_timeout_invalid(wdd, t))
		wdd->timeout = t;
	else
		ret = -EINVAL;

	return ret;
}
EXPORT_SYMBOL_GPL(watchdog_init_timeout);

/* 
                                                          
                        
  
                                                         
                                                 
  
                                                              
           
 */
int watchdog_register_device(struct watchdog_device *wdd)
{
	int ret, id, devno;

	if (wdd == NULL || wdd->info == NULL || wdd->ops == NULL)
		return -EINVAL;

	/*                                           */
	if (wdd->ops->start == NULL || wdd->ops->stop == NULL)
		return -EINVAL;

	watchdog_check_min_max_timeout(wdd);

	/*
                                                                 
                                                                
                                                             
  */

	mutex_init(&wdd->lock);
	id = ida_simple_get(&watchdog_ida, 0, MAX_DOGS, GFP_KERNEL);
	if (id < 0)
		return id;
	wdd->id = id;

	ret = watchdog_dev_register(wdd);
	if (ret) {
		ida_simple_remove(&watchdog_ida, id);
		if (!(id == 0 && ret == -EBUSY))
			return ret;

		/*                                               */
		id = ida_simple_get(&watchdog_ida, 1, MAX_DOGS, GFP_KERNEL);
		if (id < 0)
			return id;
		wdd->id = id;

		ret = watchdog_dev_register(wdd);
		if (ret) {
			ida_simple_remove(&watchdog_ida, id);
			return ret;
		}
	}

	devno = wdd->cdev.dev;
	wdd->dev = device_create(watchdog_class, wdd->parent, devno,
					NULL, "watchdog%d", wdd->id);
	if (IS_ERR(wdd->dev)) {
		watchdog_dev_unregister(wdd);
		ida_simple_remove(&watchdog_ida, id);
		ret = PTR_ERR(wdd->dev);
		return ret;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(watchdog_register_device);

/* 
                                                              
                                      
  
                                                                
                                              
 */
void watchdog_unregister_device(struct watchdog_device *wdd)
{
	int ret;
	int devno;

	if (wdd == NULL)
		return;

	devno = wdd->cdev.dev;
	ret = watchdog_dev_unregister(wdd);
	if (ret)
		pr_err("error unregistering /dev/watchdog (err=%d)\n", ret);
	device_destroy(watchdog_class, devno);
	ida_simple_remove(&watchdog_ida, wdd->id);
	wdd->dev = NULL;
}
EXPORT_SYMBOL_GPL(watchdog_unregister_device);

static int __init watchdog_init(void)
{
	int err;

	watchdog_class = class_create(THIS_MODULE, "watchdog");
	if (IS_ERR(watchdog_class)) {
		pr_err("couldn't create class\n");
		return PTR_ERR(watchdog_class);
	}

	err = watchdog_dev_init();
	if (err < 0) {
		class_destroy(watchdog_class);
		return err;
	}

	return 0;
}

static void __exit watchdog_exit(void)
{
	watchdog_dev_exit();
	class_destroy(watchdog_class);
	ida_destroy(&watchdog_ida);
}

subsys_initcall(watchdog_init);
module_exit(watchdog_exit);

MODULE_AUTHOR("Alan Cox <alan@lxorguk.ukuu.org.uk>");
MODULE_AUTHOR("Wim Van Sebroeck <wim@iguana.be>");
MODULE_DESCRIPTION("WatchDog Timer Driver Core");
MODULE_LICENSE("GPL");
