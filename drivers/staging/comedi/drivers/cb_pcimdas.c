/*
    comedi/drivers/cb_pcimdas.c
    Comedi driver for Computer Boards PCIM-DAS1602/16

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

#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#include "../comedidev.h"

#include "plx9052.h"
#include "8255.h"

/*                         */
#undef CBPCIMDAS_DEBUG

/*                                   */

/*                             */
#define BADR3_SIZE 16

/*             */
#define ADC_TRIG 0
#define DAC0_OFFSET 2
#define DAC1_OFFSET 4

/*                          */
#define MUX_LIMITS 0
#define MAIN_CONN_DIO 1
#define ADC_STAT 2
#define ADC_CONV_STAT 3
#define ADC_INT 4
#define ADC_PACER 5
#define BURST_MODE 6
#define PROG_GAIN 7
#define CLK8254_1_DATA 8
#define CLK8254_2_DATA 9
#define CLK8254_3_DATA 10
#define CLK8254_CONTROL 11
#define USER_COUNTER 12
#define RESID_COUNT_H 13
#define RESID_COUNT_L 14

/*
                                                                 
                                                                       
                                                                       
          
 */
struct cb_pcimdas_private {
	/*                */
	unsigned long BADR3;

	/*                      */
	unsigned int ao_readback[2];
};

/*
                                                                       
        
 */
static int cb_pcimdas_ai_rinsn(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcimdas_private *devpriv = dev->private;
	int n, i;
	unsigned int d;
	unsigned int busy;
	int chan = CR_CHAN(insn->chanspec);
	unsigned short chanlims;
	int maxchans;

	/*                                                        */

	/*                      */
	if ((inb(devpriv->BADR3 + 2) & 0x20) == 0)	/*                   */
		maxchans = s->n_chan / 2;
	else
		maxchans = s->n_chan;

	if (chan > (maxchans - 1))
		return -ETIMEDOUT;	/*                              */

	/*                                 */
	d = inb(devpriv->BADR3 + 5);
	if ((d & 0x03) > 0) {	/*                       */
		d = d & 0xfd;
		outb(d, devpriv->BADR3 + 5);
	}
	outb(0x01, devpriv->BADR3 + 6);	/*                                  */
	outb(0x00, devpriv->BADR3 + 7);	/*                                                                */

	/*
                                                               
                                     
  */
	chanlims = chan | (chan << 4);
	outb(chanlims, devpriv->BADR3 + 0);

	/*                   */
	for (n = 0; n < insn->n; n++) {
		/*                    */
		outw(0, dev->iobase + 0);

#define TIMEOUT 1000		/*                                                             */
		/*                                                                             */

		/*                            */
		for (i = 0; i < TIMEOUT; i++) {
			busy = inb(devpriv->BADR3 + 2) & 0x80;
			if (!busy)
				break;
		}
		if (i == TIMEOUT) {
			printk("timeout\n");
			return -ETIMEDOUT;
		}
		/*           */
		data[n] = inw(dev->iobase + 0);
	}

	/*                                           */
	return n;
}

static int cb_pcimdas_ao_winsn(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcimdas_private *devpriv = dev->private;
	int i;
	int chan = CR_CHAN(insn->chanspec);

	/*                                                          
                                                          */
	for (i = 0; i < insn->n; i++) {
		switch (chan) {
		case 0:
			outw(data[i] & 0x0FFF, dev->iobase + DAC0_OFFSET);
			break;
		case 1:
			outw(data[i] & 0x0FFF, dev->iobase + DAC1_OFFSET);
			break;
		default:
			return -1;
		}
		devpriv->ao_readback[chan] = data[i];
	}

	/*                                           */
	return i;
}

/*                                                               
                                                         */
static int cb_pcimdas_ao_rinsn(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcimdas_private *devpriv = dev->private;
	int i;
	int chan = CR_CHAN(insn->chanspec);

	for (i = 0; i < insn->n; i++)
		data[i] = devpriv->ao_readback[chan];

	return i;
}

static int cb_pcimdas_auto_attach(struct comedi_device *dev,
					    unsigned long context_unused)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	struct cb_pcimdas_private *devpriv;
	struct comedi_subdevice *s;
	unsigned long iobase_8255;
	int ret;

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;

	dev->iobase = pci_resource_start(pcidev, 2);
	devpriv->BADR3 = pci_resource_start(pcidev, 3);
	iobase_8255 = pci_resource_start(pcidev, 4);

/*                      */
/*          */
/*                                                                                     */
/*   */
/*                                                      */
/*                 */
/*   */
/*                         */

	ret = comedi_alloc_subdevices(dev, 3);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	/*                     */
	/*                         */
	s->type = COMEDI_SUBD_AI;
	s->subdev_flags = SDF_READABLE | SDF_GROUND;
	s->n_chan = 16;
	s->maxdata = 0xffff;
	s->range_table = &range_unknown;
	s->len_chanlist = 1;	/*                                           */
	/*                       */
	s->insn_read = cb_pcimdas_ai_rinsn;

	s = &dev->subdevices[1];
	/*                          */
	s->type = COMEDI_SUBD_AO;
	s->subdev_flags = SDF_WRITABLE;
	s->n_chan = 2;
	s->maxdata = 0xfff;
	/*                                                          */
	s->range_table = &range_unknown;
	s->insn_write = &cb_pcimdas_ao_winsn;
	s->insn_read = &cb_pcimdas_ao_rinsn;

	s = &dev->subdevices[2];
	/*                       */
	subdev_8255_init(dev, s, NULL, iobase_8255);

	dev_info(dev->class_dev, "%s attached\n", dev->board_name);

	return 0;
}

static void cb_pcimdas_detach(struct comedi_device *dev)
{
	if (dev->irq)
		free_irq(dev->irq, dev);
	comedi_pci_disable(dev);
}

static struct comedi_driver cb_pcimdas_driver = {
	.driver_name	= "cb_pcimdas",
	.module		= THIS_MODULE,
	.auto_attach	= cb_pcimdas_auto_attach,
	.detach		= cb_pcimdas_detach,
};

static int cb_pcimdas_pci_probe(struct pci_dev *dev,
				const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &cb_pcimdas_driver,
				      id->driver_data);
}

static DEFINE_PCI_DEVICE_TABLE(cb_pcimdas_pci_table) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_CB, 0x0056) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, cb_pcimdas_pci_table);

static struct pci_driver cb_pcimdas_pci_driver = {
	.name		= "cb_pcimdas",
	.id_table	= cb_pcimdas_pci_table,
	.probe		= cb_pcimdas_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(cb_pcimdas_driver, cb_pcimdas_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
