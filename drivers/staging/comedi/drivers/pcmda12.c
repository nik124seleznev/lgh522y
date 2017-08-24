/*
    comedi/drivers/pcmda12.c
    Driver for Winsystems PC-104 based PCM-D/A-12 8-channel AO board.

    COMEDI - Linux Control and Measurement Device Interface
    Copyright (C) 2006 Calin A. Culianu <calin@ajvar.org>

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

#include "../comedidev.h"

#define CHANS 8
#define IOSIZE 16
#define LSB(x) ((unsigned char)((x) & 0xff))
#define MSB(x) ((unsigned char)((((unsigned short)(x))>>8) & 0xff))
#define LSB_PORT(chan) (dev->iobase + (chan)*2)
#define MSB_PORT(chan) (LSB_PORT(chan)+1)
#define BITS 12

/*                                                              
                                                */
static const struct comedi_lrange pcmda12_ranges = {
	3,
	{
	 UNI_RANGE(5), UNI_RANGE(10), BIP_RANGE(5)
	 }
};

struct pcmda12_private {

	unsigned int ao_readback[CHANS];
	int simultaneous_xfer_mode;
};

static void zero_chans(struct comedi_device *dev)
{				/*           
                             */
	int i;
	for (i = 0; i < CHANS; ++i) {
/*                                           */
/*                               */
		outb(0, LSB_PORT(i));
		outb(0, MSB_PORT(i));
	}
	inb(LSB_PORT(0));	/*               */
}

static int ao_winsn(struct comedi_device *dev, struct comedi_subdevice *s,
		    struct comedi_insn *insn, unsigned int *data)
{
	struct pcmda12_private *devpriv = dev->private;
	int i;
	int chan = CR_CHAN(insn->chanspec);

	/*                                                          
                                                          */
	for (i = 0; i < insn->n; ++i) {

/*                                           */
/*                                     */

		/*                                                        */
		/*                           */
		outb(LSB(data[i]), LSB_PORT(chan));
		/*                            */
		outb(MSB(data[i]), MSB_PORT(chan));

		/*                      */
		devpriv->ao_readback[chan] = data[i];

		if (!devpriv->simultaneous_xfer_mode)
			inb(LSB_PORT(chan));
	}

	/*                                      */
	return i;
}

/*                                                               

                                                                     
                                                                       
                                            

                                                                    
                                                                      
                                                                     
                                                                  
                                                                 
*/
static int ao_rinsn(struct comedi_device *dev, struct comedi_subdevice *s,
		    struct comedi_insn *insn, unsigned int *data)
{
	struct pcmda12_private *devpriv = dev->private;
	int i;
	int chan = CR_CHAN(insn->chanspec);

	for (i = 0; i < insn->n; i++) {
		if (devpriv->simultaneous_xfer_mode)
			inb(LSB_PORT(chan));
		/*                           */
		data[i] = devpriv->ao_readback[chan];
	}

	return i;
}

static int pcmda12_attach(struct comedi_device *dev,
			  struct comedi_devconfig *it)
{
	struct pcmda12_private *devpriv;
	struct comedi_subdevice *s;
	int ret;

	ret = comedi_request_region(dev, it->options[0], IOSIZE);
	if (ret)
		return ret;

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	devpriv->simultaneous_xfer_mode = it->options[1];

	ret = comedi_alloc_subdevices(dev, 1);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	s->private = NULL;
	s->maxdata = (0x1 << BITS) - 1;
	s->range_table = &pcmda12_ranges;
	s->type = COMEDI_SUBD_AO;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE;
	s->n_chan = CHANS;
	s->insn_write = &ao_winsn;
	s->insn_read = &ao_rinsn;

	zero_chans(dev);	/*                                        */

	return 1;
}

static struct comedi_driver pcmda12_driver = {
	.driver_name	= "pcmda12",
	.module		= THIS_MODULE,
	.attach		= pcmda12_attach,
	.detach		= comedi_legacy_detach,
};
module_comedi_driver(pcmda12_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
