/*
 * comedi_pcmcia.c
 * Comedi PCMCIA driver specific functions.
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

#include <linux/kernel.h>

#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>

#include "comedidev.h"

/* 
                                                                           
                             
 */
struct pcmcia_device *comedi_to_pcmcia_dev(struct comedi_device *dev)
{
	return dev->hw_dev ? to_pcmcia_dev(dev->hw_dev) : NULL;
}
EXPORT_SYMBOL_GPL(comedi_to_pcmcia_dev);

static int comedi_pcmcia_conf_check(struct pcmcia_device *link,
				    void *priv_data)
{
	if (link->config_index == 0)
		return -EINVAL;

	return pcmcia_request_io(link);
}

/* 
                                                                             
                             
                                                                          
  
                                                                   
                                                                     
                                                                      
 */
int comedi_pcmcia_enable(struct comedi_device *dev,
			 int (*conf_check)(struct pcmcia_device *, void *))
{
	struct pcmcia_device *link = comedi_to_pcmcia_dev(dev);
	int ret;

	if (!link)
		return -ENODEV;

	if (!conf_check)
		conf_check = comedi_pcmcia_conf_check;

	ret = pcmcia_loop_config(link, conf_check, NULL);
	if (ret)
		return ret;

	return pcmcia_enable_device(link);
}
EXPORT_SYMBOL_GPL(comedi_pcmcia_enable);

/* 
                                                                               
                             
 */
void comedi_pcmcia_disable(struct comedi_device *dev)
{
	struct pcmcia_device *link = comedi_to_pcmcia_dev(dev);

	if (link)
		pcmcia_disable_device(link);
}
EXPORT_SYMBOL_GPL(comedi_pcmcia_disable);

/* 
                                                                        
                              
                                
  
                                                             
 */
int comedi_pcmcia_auto_config(struct pcmcia_device *link,
			      struct comedi_driver *driver)
{
	return comedi_auto_config(&link->dev, driver, 0);
}
EXPORT_SYMBOL_GPL(comedi_pcmcia_auto_config);

/* 
                                                                             
                              
  
                                                              
 */
void comedi_pcmcia_auto_unconfig(struct pcmcia_device *link)
{
	comedi_auto_unconfig(&link->dev);
}
EXPORT_SYMBOL_GPL(comedi_pcmcia_auto_unconfig);

/* 
                                                                     
                                       
                                       
  
                                                                     
                                                                        
                 
 */
int comedi_pcmcia_driver_register(struct comedi_driver *comedi_driver,
				  struct pcmcia_driver *pcmcia_driver)
{
	int ret;

	ret = comedi_driver_register(comedi_driver);
	if (ret < 0)
		return ret;

	ret = pcmcia_register_driver(pcmcia_driver);
	if (ret < 0) {
		comedi_driver_unregister(comedi_driver);
		return ret;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(comedi_pcmcia_driver_register);

/* 
                                                                         
                                       
                                       
  
                                                                        
                                                                        
                 
 */
void comedi_pcmcia_driver_unregister(struct comedi_driver *comedi_driver,
				     struct pcmcia_driver *pcmcia_driver)
{
	pcmcia_unregister_driver(pcmcia_driver);
	comedi_driver_unregister(comedi_driver);
}
EXPORT_SYMBOL_GPL(comedi_pcmcia_driver_unregister);
