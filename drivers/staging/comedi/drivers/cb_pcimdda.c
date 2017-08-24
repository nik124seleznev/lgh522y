/*
    comedi/drivers/cb_pcimdda.c
    Computer Boards PCIM-DDA06-16 Comedi driver
    Author: Calin Culianu <calin@ajvar.org>

    COMEDI - Linux Control and Measurement Device Interface
    Copyright (C) 2000 David A. Schleef <ds@schleef.org>

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

/*
                                                                        
                                                                       
                                 

                                                                           
                                                                      
                                                                            

                                                                
                                                                       
         

                                                                              
                                                             

                                    
 */

#include <linux/pci.h>

#include "../comedidev.h"

#include "8255.h"

/*                                                                       */
#define PCI_ID_PCIM_DDA06_16		0x0053

/*
                                  
 */
#define PCIMDDA_DA_CHAN(x)		(0x00 + (x) * 2)
#define PCIMDDA_8255_BASE_REG		0x0c

#define MAX_AO_READBACK_CHANNELS	6

struct cb_pcimdda_private {
	unsigned int ao_readback[MAX_AO_READBACK_CHANNELS];
};

static int cb_pcimdda_ao_winsn(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn,
			       unsigned int *data)
{
	struct cb_pcimdda_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned long offset = dev->iobase + PCIMDDA_DA_CHAN(chan);
	unsigned int val = 0;
	int i;

	for (i = 0; i < insn->n; i++) {
		val = data[i];

		/*
                            
    
                                                     
                                                     
                                                    
                                                    
                
   */
		outb(val & 0x00ff, offset);
		outb((val >> 8) & 0x00ff, offset + 1);
	}

	/*                                   */
	devpriv->ao_readback[chan] = val;

	return insn->n;
}

static int cb_pcimdda_ao_rinsn(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn,
			       unsigned int *data)
{
	struct cb_pcimdda_private *devpriv = dev->private;
	int chan = CR_CHAN(insn->chanspec);
	unsigned long offset = dev->iobase + PCIMDDA_DA_CHAN(chan);
	int i;

	for (i = 0; i < insn->n; i++) {
		/*                                    */
		inw(offset);

		data[i] = devpriv->ao_readback[chan];
	}

	return insn->n;
}

static int cb_pcimdda_auto_attach(struct comedi_device *dev,
					    unsigned long context_unused)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	struct cb_pcimdda_private *devpriv;
	struct comedi_subdevice *s;
	int ret;

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;
	dev->iobase = pci_resource_start(pcidev, 3);

	ret = comedi_alloc_subdevices(dev, 2);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	/*                         */
	s->type		= COMEDI_SUBD_AO;
	s->subdev_flags	= SDF_WRITABLE | SDF_READABLE;
	s->n_chan	= 6;
	s->maxdata	= 0xffff;
	s->range_table	= &range_bipolar5;
	s->insn_write	= cb_pcimdda_ao_winsn;
	s->insn_read	= cb_pcimdda_ao_rinsn;

	s = &dev->subdevices[1];
	/*                       */
	ret = subdev_8255_init(dev, s, NULL,
			dev->iobase + PCIMDDA_8255_BASE_REG);
	if (ret)
		return ret;

	dev_info(dev->class_dev, "%s attached\n", dev->board_name);

	return 1;
}

static void cb_pcimdda_detach(struct comedi_device *dev)
{
	comedi_spriv_free(dev, 1);
	comedi_pci_disable(dev);
}

static struct comedi_driver cb_pcimdda_driver = {
	.driver_name	= "cb_pcimdda",
	.module		= THIS_MODULE,
	.auto_attach	= cb_pcimdda_auto_attach,
	.detach		= cb_pcimdda_detach,
};

static int cb_pcimdda_pci_probe(struct pci_dev *dev,
				const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &cb_pcimdda_driver,
				      id->driver_data);
}

static DEFINE_PCI_DEVICE_TABLE(cb_pcimdda_pci_table) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_CB, PCI_ID_PCIM_DDA06_16) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, cb_pcimdda_pci_table);

static struct pci_driver cb_pcimdda_driver_pci_driver = {
	.name		= "cb_pcimdda",
	.id_table	= cb_pcimdda_pci_table,
	.probe		= cb_pcimdda_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(cb_pcimdda_driver, cb_pcimdda_driver_pci_driver);

MODULE_AUTHOR("Calin A. Culianu <calin@rtlab.org>");
MODULE_DESCRIPTION("Comedi low-level driver for the Computerboards PCIM-DDA "
		   "series.  Currently only supports PCIM-DDA06-16 (which "
		   "also happens to be the only board in this series. :) ) ");
MODULE_LICENSE("GPL");
