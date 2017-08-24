/* comedi/drivers/amplc_dio200_pci.c

    Driver for Amplicon PCI215, PCI272, PCIe215, PCIe236, PCIe296.

    Copyright (C) 2005-2013 MEV Ltd. <http://www.mev.co.uk/>

    COMEDI - Linux Control and Measurement Device Interface
    Copyright (C) 1998,2000 David A. Schleef <ds@schleef.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
/*
                           
                                                   
                                         
                                                                   
                    
                                           
                
  
                         
         
  
                                                                             
                 
  
             
  
                                                            
                                                               
                                                         
                                                           
                                                            
                                                            
                                                            
                                                            
                                                            
                                                           
                                                             
  
  
                                             
                                                
                                          
                                             
                                             
                                             
                                             
                                             
                                             
                                            
                                              
  
                                                                       
                                                        
  
                                
                                
                                
                                
  
                                              
  
                                                                     
                                                                         
                                                                      
                                                                    
                                                                          
                              
  
                                                                       
                                               
  
                                                                           
                                    
  
                                                                           
                                                                     
                                                                          
                         
  
                                                                          
                                                                         
                                                  
                                   
                                  
                                    
                                   
                                  
                                                                           
                                                                         
                                                                      
                                
  
                                                                          
                                                              
  
                              
                             
                                      
                                   
  
                                                                      
                                                                           
                           
  
                                                                         
                                                                     
                                                                           
                       
  
                                                                    
                                                                    
                                                                          
                                                                         
                                                  
                                                                           
                    
                      
                      
                      
                      
  
                                                                          
                                                                           
                   
  
                                                                       
                                                        
                                                                    
                                              
                                                                  
                                                                         
                  
                                                                         
                                       
                                                                        
                                              
                                                                        
                                                             
                                                                         
                                                                       
                      
  
                                                                          
                         
  
                                        
  
                                                                           
                                                                       
                                                       
  
                                                                        
                                                                           
                                                              
  
                                                                       
                                           
  
                                                                  
  
                                                                           
                                                                         
                                                                         
                                                                      
         
  
                    
  
                                                            
                                                               
                                                         
                                                            
                                                            
                                                           
                                                           
                                                              
                                                              
  
                                             
                                                
                                          
                                              
                                              
                                              
                                              
                                               
                                               
  
                                                                     
                                                                         
                                             
  
                                                                          
                                                                       
                                                                       
                                                                           
                                                                         
                                                                   
                                      
  
           
  
                                                                  
                                                                         
                                                                        
                                                                        
                                                                      
                                                                           
                                                                      
                                         
 */

#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#include "../comedidev.h"

#include "amplc_dio200.h"

/*         */
#define PCI_DEVICE_ID_AMPLICON_PCI272 0x000a
#define PCI_DEVICE_ID_AMPLICON_PCI215 0x000b
#define PCI_DEVICE_ID_AMPLICON_PCIE236 0x0011
#define PCI_DEVICE_ID_AMPLICON_PCIE215 0x0012
#define PCI_DEVICE_ID_AMPLICON_PCIE296 0x0014

/*
                      
 */

enum dio200_pci_model {
	pci215_model,
	pci272_model,
	pcie215_model,
	pcie236_model,
	pcie296_model
};

static const struct dio200_board dio200_pci_boards[] = {
	[pci215_model] = {
		.name = "pci215",
		.bustype = pci_bustype,
		.mainbar = 2,
		.mainsize = DIO200_IO_SIZE,
		.layout = {
			.n_subdevs = 5,
			.sdtype = {sd_8255, sd_8255, sd_8254, sd_8254, sd_intr},
			.sdinfo = {0x00, 0x08, 0x10, 0x14, 0x3F},
			.has_int_sce = true,
			.has_clk_gat_sce = true,
		},
	},
	[pci272_model] = {
		.name = "pci272",
		.bustype = pci_bustype,
		.mainbar = 2,
		.mainsize = DIO200_IO_SIZE,
		.layout = {
			.n_subdevs = 4,
			.sdtype = {sd_8255, sd_8255, sd_8255, sd_intr},
			.sdinfo = {0x00, 0x08, 0x10, 0x3F},
			.has_int_sce = true,
		},
	},
	[pcie215_model] = {
		.name = "pcie215",
		.bustype = pci_bustype,
		.mainbar = 1,
		.mainshift = 3,
		.mainsize = DIO200_PCIE_IO_SIZE,
		.layout = {
			.n_subdevs = 8,
			.sdtype = {sd_8255, sd_none, sd_8255, sd_none,
				   sd_8254, sd_8254, sd_timer, sd_intr},
			.sdinfo = {0x00, 0x00, 0x08, 0x00,
				   0x10, 0x14, 0x00, 0x3F},
			.has_int_sce = true,
			.has_clk_gat_sce = true,
			.has_enhancements = true,
		},
	},
	[pcie236_model] = {
		.name = "pcie236",
		.bustype = pci_bustype,
		.mainbar = 1,
		.mainshift = 3,
		.mainsize = DIO200_PCIE_IO_SIZE,
		.layout = {
			.n_subdevs = 8,
			.sdtype = {sd_8255, sd_none, sd_none, sd_none,
				   sd_8254, sd_8254, sd_timer, sd_intr},
			.sdinfo = {0x00, 0x00, 0x00, 0x00,
				   0x10, 0x14, 0x00, 0x3F},
			.has_int_sce = true,
			.has_clk_gat_sce = true,
			.has_enhancements = true,
		},
	},
	[pcie296_model] = {
		.name = "pcie296",
		.bustype = pci_bustype,
		.mainbar = 1,
		.mainshift = 3,
		.mainsize = DIO200_PCIE_IO_SIZE,
		.layout = {
			.n_subdevs = 8,
			.sdtype = {sd_8255, sd_8255, sd_8255, sd_8255,
				   sd_8254, sd_8254, sd_timer, sd_intr},
			.sdinfo = {0x00, 0x04, 0x08, 0x0C,
				   0x10, 0x14, 0x00, 0x3F},
			.has_int_sce = true,
			.has_clk_gat_sce = true,
			.has_enhancements = true,
		},
	},
};

/*
                                                             
                             
 */
static int dio200_pcie_board_setup(struct comedi_device *dev)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	void __iomem *brbase;

	/*
                                                              
                                                               
                                                               
                              
   
                                                                    
                                                               
                                                                    
  */
	if (pci_resource_len(pcidev, 0) < 0x4000) {
		dev_err(dev->class_dev, "error! bad PCI region!\n");
		return -EINVAL;
	}
	brbase = pci_ioremap_bar(pcidev, 0);
	if (!brbase) {
		dev_err(dev->class_dev, "error! failed to map registers!\n");
		return -ENOMEM;
	}
	writel(0x80, brbase + 0x50);
	iounmap(brbase);
	/*                                      */
	amplc_dio200_set_enhance(dev, 1);
	return 0;
}

static int dio200_pci_auto_attach(struct comedi_device *dev,
				  unsigned long context_model)
{
	struct pci_dev *pci_dev = comedi_to_pci_dev(dev);
	const struct dio200_board *thisboard = NULL;
	struct dio200_private *devpriv;
	unsigned int bar;
	int ret;

	if (context_model < ARRAY_SIZE(dio200_pci_boards))
		thisboard = &dio200_pci_boards[context_model];
	if (!thisboard)
		return -EINVAL;
	dev->board_ptr = thisboard;
	dev->board_name = thisboard->name;

	dev_info(dev->class_dev, "%s: attach pci %s (%s)\n",
		 dev->driver->driver_name, pci_name(pci_dev), dev->board_name);

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;

	bar = thisboard->mainbar;
	if (pci_resource_len(pci_dev, bar) < thisboard->mainsize) {
		dev_err(dev->class_dev, "error! PCI region size too small!\n");
		return -EINVAL;
	}
	if (pci_resource_flags(pci_dev, bar) & IORESOURCE_MEM) {
		devpriv->io.u.membase = pci_ioremap_bar(pci_dev, bar);
		if (!devpriv->io.u.membase) {
			dev_err(dev->class_dev,
				"error! cannot remap registers\n");
			return -ENOMEM;
		}
		devpriv->io.regtype = mmio_regtype;
	} else {
		devpriv->io.u.iobase = pci_resource_start(pci_dev, bar);
		devpriv->io.regtype = io_regtype;
	}
	switch (context_model) {
	case pcie215_model:
	case pcie236_model:
	case pcie296_model:
		ret = dio200_pcie_board_setup(dev);
		if (ret < 0)
			return ret;
		break;
	default:
		break;
	}
	return amplc_dio200_common_attach(dev, pci_dev->irq, IRQF_SHARED);
}

static void dio200_pci_detach(struct comedi_device *dev)
{
	const struct dio200_board *thisboard = comedi_board(dev);
	struct dio200_private *devpriv = dev->private;

	if (!thisboard || !devpriv)
		return;
	amplc_dio200_common_detach(dev);
	if (devpriv->io.regtype == mmio_regtype)
		iounmap(devpriv->io.u.membase);
	comedi_pci_disable(dev);
}

static struct comedi_driver dio200_pci_comedi_driver = {
	.driver_name = "amplc_dio200_pci",
	.module = THIS_MODULE,
	.auto_attach = dio200_pci_auto_attach,
	.detach = dio200_pci_detach,
};

static DEFINE_PCI_DEVICE_TABLE(dio200_pci_table) = {
	{
		PCI_VDEVICE(AMPLICON, PCI_DEVICE_ID_AMPLICON_PCI215),
		pci215_model
	}, {
		PCI_VDEVICE(AMPLICON, PCI_DEVICE_ID_AMPLICON_PCI272),
		pci272_model
	}, {
		PCI_VDEVICE(AMPLICON, PCI_DEVICE_ID_AMPLICON_PCIE236),
		pcie236_model
	}, {
		PCI_VDEVICE(AMPLICON, PCI_DEVICE_ID_AMPLICON_PCIE215),
		pcie215_model
	}, {
		PCI_VDEVICE(AMPLICON, PCI_DEVICE_ID_AMPLICON_PCIE296),
		pcie296_model
	},
	{0}
};

MODULE_DEVICE_TABLE(pci, dio200_pci_table);

static int dio200_pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &dio200_pci_comedi_driver,
				      id->driver_data);
}

static struct pci_driver dio200_pci_pci_driver = {
	.name = "amplc_dio200_pci",
	.id_table = dio200_pci_table,
	.probe = dio200_pci_probe,
	.remove	= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(dio200_pci_comedi_driver, dio200_pci_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi driver for Amplicon 200 Series PCI(e) DIO boards");
MODULE_LICENSE("GPL");
