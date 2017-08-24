/*
 * comedi_pci.c
 * Comedi PCI driver specific functions.
 *
 * COMEDI - Linux Control and Measurement Device Interface
 * Copyright (C) 1997-2000 David A. Schleef <ds@schleef.org>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/pci.h>

#include "comedidev.h"

/* 
                                                                  
                             
 */
struct pci_dev *comedi_to_pci_dev(struct comedi_device *dev)
{
	return dev->hw_dev ? to_pci_dev(dev->hw_dev) : NULL;
}
EXPORT_SYMBOL_GPL(comedi_to_pci_dev);

/* 
                                                                       
                             
 */
int comedi_pci_enable(struct comedi_device *dev)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	int rc;

	if (!pcidev)
		return -ENODEV;

	rc = pci_enable_device(pcidev);
	if (rc < 0)
		return rc;

	rc = pci_request_regions(pcidev, dev->board_name);
	if (rc < 0)
		pci_disable_device(pcidev);
	else
		dev->ioenabled = true;

	return rc;
}
EXPORT_SYMBOL_GPL(comedi_pci_enable);

/* 
                                                                         
                             
 */
void comedi_pci_disable(struct comedi_device *dev)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);

	if (pcidev && dev->ioenabled) {
		pci_release_regions(pcidev);
		pci_disable_device(pcidev);
	}
	dev->ioenabled = false;
}
EXPORT_SYMBOL_GPL(comedi_pci_disable);

/* 
                                                                  
                          
                                
                                                                 
  
                                                          
 */
int comedi_pci_auto_config(struct pci_dev *pcidev,
			   struct comedi_driver *driver,
			   unsigned long context)
{
	return comedi_auto_config(&pcidev->dev, driver, context);
}
EXPORT_SYMBOL_GPL(comedi_pci_auto_config);

/* 
                                                                       
                          
  
                                                           
 */
void comedi_pci_auto_unconfig(struct pci_dev *pcidev)
{
	comedi_auto_unconfig(&pcidev->dev);
}
EXPORT_SYMBOL_GPL(comedi_pci_auto_unconfig);

/* 
                                                               
                                       
                                 
  
                                                                     
                                                                     
                 
 */
int comedi_pci_driver_register(struct comedi_driver *comedi_driver,
			       struct pci_driver *pci_driver)
{
	int ret;

	ret = comedi_driver_register(comedi_driver);
	if (ret < 0)
		return ret;

	ret = pci_register_driver(pci_driver);
	if (ret < 0) {
		comedi_driver_unregister(comedi_driver);
		return ret;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(comedi_pci_driver_register);

/* 
                                                                   
                                       
                                 
  
                                                                     
                                                                     
                 
 */
void comedi_pci_driver_unregister(struct comedi_driver *comedi_driver,
				  struct pci_driver *pci_driver)
{
	pci_unregister_driver(pci_driver);
	comedi_driver_unregister(comedi_driver);
}
EXPORT_SYMBOL_GPL(comedi_pci_driver_unregister);
