/*
 *     comedi/drivers/ni_daq_700.c
 *     Driver for DAQCard-700 DIO/AI
 *     copied from 8255
 *
 *     COMEDI - Linux Control and Measurement Device Interface
 *     Copyright (C) 1998 David A. Schleef <ds@schleef.org>
 *
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/*
                  
                                                             
                                            
                                                                 
                                                                
             
                                        

                                                                      
                                                                           

                                                               
                                                            

                                                                             
                                                

                                                           
                             

                                    
                                                                   

                                                                 
                                                        
*/

#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#include "../comedidev.h"

#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>

/*                      */
#define DIO_W		0x04	/*         */
#define DIO_R		0x05	/*         */
#define CMD_R1		0x00	/*         */
#define CMD_R2		0x07	/*         */
#define CMD_R3		0x05	/*         */
#define STA_R1		0x00	/*         */
#define STA_R2		0x01	/*         */
#define ADFIFO_R	0x02	/*          */
#define ADCLEAR_R	0x01	/*         */
#define CDA_R0		0x08	/*         */
#define CDA_R1		0x09	/*         */
#define CDA_R2		0x0A	/*         */
#define CMO_R		0x0B	/*         */
#define TIC_R		0x06	/*         */

static int daq700_dio_insn_bits(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn, unsigned int *data)
{
	if (data[0]) {
		s->state &= ~data[0];
		s->state |= (data[0] & data[1]);

		if (data[0] & 0xff)
			outb(s->state & 0xff, dev->iobase + DIO_W);
	}

	data[1] = s->state & 0xff;
	data[1] |= inb(dev->iobase + DIO_R) << 8;

	return insn->n;
}

static int daq700_dio_insn_config(struct comedi_device *dev,
				  struct comedi_subdevice *s,
				  struct comedi_insn *insn, unsigned int *data)
{
	unsigned int chan = 1 << CR_CHAN(insn->chanspec);

	switch (data[0]) {
	case INSN_CONFIG_DIO_INPUT:
		break;
	case INSN_CONFIG_DIO_OUTPUT:
		break;
	case INSN_CONFIG_DIO_QUERY:
		data[1] = (s->io_bits & chan) ? COMEDI_OUTPUT : COMEDI_INPUT;
		break;
	default:
		return -EINVAL;
	}

	return insn->n;
}

static int daq700_ai_rinsn(struct comedi_device *dev,
			   struct comedi_subdevice *s,
			   struct comedi_insn *insn, unsigned int *data)
{
	int n, i, chan;
	int d;
	unsigned int status;
	enum { TIMEOUT = 100 };

	chan = CR_CHAN(insn->chanspec);
	/*                              */
	/*                                            */
	outb(chan | 0x80, dev->iobase + CMD_R1);
	/*                                               */
	udelay(2);

	/*                   */
	for (n = 0; n < insn->n; n++) {
		/*                                     */
		outb(0x00, dev->iobase + CMD_R2); /*                        */
		outb(0x30, dev->iobase + CMO_R); /*                       */
		/*                                         */
		outb(0x32, dev->iobase + CMO_R);
		/*                            */
		for (i = 0; i < TIMEOUT; i++) {
			status = inb(dev->iobase + STA_R2);
			if ((status & 0x03) != 0) {
				dev_info(dev->class_dev,
					 "Overflow/run Error\n");
				return -EOVERFLOW;
			}
			status = inb(dev->iobase + STA_R1);
			if ((status & 0x02) != 0) {
				dev_info(dev->class_dev, "Data Error\n");
				return -ENODATA;
			}
			if ((status & 0x11) == 0x01) {
				/*                         */
				break;
			}
			udelay(1);
		}
		if (i == TIMEOUT) {
			dev_info(dev->class_dev,
				 "timeout during ADC conversion\n");
			return -ETIMEDOUT;
		}
		/*           */
		d = inw(dev->iobase + ADFIFO_R);
		/*                              */
		/*                                                 */
		d &= 0x0fff;
		d ^= 0x0800;
		data[n] = d;
	}
	return n;
}

/*
                               
                                
                                                      
                                         
                               
                                          
                                         
                                                          
                           
 */
static void daq700_ai_config(struct comedi_device *dev,
			     struct comedi_subdevice *s)
{			
	unsigned long iobase = dev->iobase;

	outb(0x80, iobase + CMD_R1);	/*                                 */
	outb(0x00, iobase + CMD_R2);	/*                */
	outb(0x00, iobase + CMD_R3);	/*                */
	outb(0x32, iobase + CMO_R);	/*                                 */
	outb(0x00, iobase + TIC_R);	/*                         */
	outb(0x00, iobase + ADCLEAR_R);	/*                    */
	inw(iobase + ADFIFO_R);		/*                                    */
}

static int daq700_auto_attach(struct comedi_device *dev,
			      unsigned long context)
{
	struct pcmcia_device *link = comedi_to_pcmcia_dev(dev);
	struct comedi_subdevice *s;
	int ret;

	link->config_flags |= CONF_AUTO_SET_IO;
	ret = comedi_pcmcia_enable(dev, NULL);
	if (ret)
		return ret;
	dev->iobase = link->resource[0]->start;

	ret = comedi_alloc_subdevices(dev, 2);
	if (ret)
		return ret;

	/*                 */
	s = &dev->subdevices[0];
	s->type		= COMEDI_SUBD_DIO;
	s->subdev_flags	= SDF_READABLE | SDF_WRITABLE;
	s->n_chan	= 16;
	s->range_table	= &range_digital;
	s->maxdata	= 1;
	s->insn_bits	= daq700_dio_insn_bits;
	s->insn_config	= daq700_dio_insn_config;
	s->state	= 0;
	s->io_bits	= 0x00ff;

	/*                */
	s = &dev->subdevices[1];
	s->type = COMEDI_SUBD_AI;
	/*                                   */
	s->subdev_flags = SDF_READABLE | SDF_GROUND;
	s->n_chan = 16;
	s->maxdata = (1 << 12) - 1;
	s->range_table = &range_bipolar10;
	s->insn_read = daq700_ai_rinsn;
	daq700_ai_config(dev, s);

	dev_info(dev->class_dev, "%s: %s, io 0x%lx\n",
		dev->driver->driver_name,
		dev->board_name,
		dev->iobase);

	return 0;
}

static struct comedi_driver daq700_driver = {
	.driver_name	= "ni_daq_700",
	.module		= THIS_MODULE,
	.auto_attach	= daq700_auto_attach,
	.detach		= comedi_pcmcia_disable,
};

static int daq700_cs_attach(struct pcmcia_device *link)
{
	return comedi_pcmcia_auto_config(link, &daq700_driver);
}

static const struct pcmcia_device_id daq700_cs_ids[] = {
	PCMCIA_DEVICE_MANF_CARD(0x010b, 0x4743),
	PCMCIA_DEVICE_NULL
};
MODULE_DEVICE_TABLE(pcmcia, daq700_cs_ids);

static struct pcmcia_driver daq700_cs_driver = {
	.name		= "ni_daq_700",
	.owner		= THIS_MODULE,
	.id_table	= daq700_cs_ids,
	.probe		= daq700_cs_attach,
	.remove		= comedi_pcmcia_auto_unconfig,
};
module_comedi_pcmcia_driver(daq700_driver, daq700_cs_driver);

MODULE_AUTHOR("Fred Brooks <nsaspook@nsaspook.com>");
MODULE_DESCRIPTION(
	"Comedi driver for National Instruments PCMCIA DAQCard-700 DIO/AI");
MODULE_VERSION("0.2.00");
MODULE_LICENSE("GPL");
