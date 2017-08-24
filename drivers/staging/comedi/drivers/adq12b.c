/*
    comedi/drivers/adq12b.c
    driver for MicroAxial ADQ12-B data acquisition and control card

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

#include "../comedidev.h"

/*                                          */
#define ADQ12B_SIZE     16

#define ADQ12B_CTREG    0x00
#define ADQ12B_STINR    0x00
#define ADQ12B_OUTBR    0x04
#define ADQ12B_ADLOW    0x08
#define ADQ12B_ADHIG    0x09
#define ADQ12B_CONT0    0x0c
#define ADQ12B_CONT1    0x0d
#define ADQ12B_CONT2    0x0e
#define ADQ12B_COWORD   0x0f

/*                                                     */
#define ADQ12B_EOC     0x20

#define TIMEOUT        20

/*                                        */
static const struct comedi_lrange range_adq12b_ai_bipolar = { 4, {
								  BIP_RANGE(5),
								  BIP_RANGE(2),
								  BIP_RANGE(1),
								  BIP_RANGE(0.5)
								  }
};

static const struct comedi_lrange range_adq12b_ai_unipolar = { 4, {
								   UNI_RANGE(5),
								   UNI_RANGE(2),
								   UNI_RANGE(1),
								   UNI_RANGE
								   (0.5)
								   }
};

struct adq12b_private {
	int unipolar;		/*                                         */
	int differential;	/*                           */
	int last_channel;
	int last_range;
	unsigned int digital_state;
};

/*
                                                                       
        
 */

static int adq12b_ai_rinsn(struct comedi_device *dev,
			   struct comedi_subdevice *s, struct comedi_insn *insn,
			   unsigned int *data)
{
	struct adq12b_private *devpriv = dev->private;
	int n, i;
	int range, channel;
	unsigned char hi, lo, status;

	/*                                                                    */
	range = CR_RANGE(insn->chanspec);
	channel = CR_CHAN(insn->chanspec);
	if (channel != devpriv->last_channel || range != devpriv->last_range) {
		outb((range << 4) | channel, dev->iobase + ADQ12B_CTREG);
		udelay(50);	/*                            */
	}

	/*                    */
	status = inb(dev->iobase + ADQ12B_ADLOW);

	/*                   */
	for (n = 0; n < insn->n; n++) {

		/*                            */
		i = 0;
		do {
			/*            */
			status = inb(dev->iobase + ADQ12B_STINR);
			status = status & ADQ12B_EOC;
		} while (status == 0 && ++i < TIMEOUT);
		/*                     */

		/*           */
		hi = inb(dev->iobase + ADQ12B_ADHIG);
		lo = inb(dev->iobase + ADQ12B_ADLOW);

		/*                                                          
                                           */
		data[n] = (hi << 8) | lo;

	}

	/*                                           */
	return n;
}

static int adq12b_di_insn_bits(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{

	/*                                                     */
	data[1] = (inb(dev->iobase + ADQ12B_STINR) & (0x1f));

	return insn->n;
}

static int adq12b_do_insn_bits(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{
	struct adq12b_private *devpriv = dev->private;
	int channel;

	for (channel = 0; channel < 8; channel++)
		if (((data[0] >> channel) & 0x01) != 0)
			outb((((data[1] >> channel) & 0x01) << 3) | channel,
			     dev->iobase + ADQ12B_OUTBR);

	/*                                                      */
	if (data[0]) {
		devpriv->digital_state &= ~data[0];
		devpriv->digital_state |= (data[0] & data[1]);
	}

	data[1] = devpriv->digital_state;

	return insn->n;
}

static int adq12b_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	struct adq12b_private *devpriv;
	struct comedi_subdevice *s;
	int ret;

	ret = comedi_request_region(dev, it->options[0], ADQ12B_SIZE);
	if (ret)
		return ret;

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	devpriv->unipolar = it->options[1];
	devpriv->differential = it->options[2];
	devpriv->digital_state = 0;
	/*
                                                         
                                                              
  */
	devpriv->last_channel = -1;
	devpriv->last_range = -1;

	ret = comedi_alloc_subdevices(dev, 3);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	/*                        */
	s->type = COMEDI_SUBD_AI;
	if (devpriv->differential) {
		s->subdev_flags = SDF_READABLE | SDF_GROUND | SDF_DIFF;
		s->n_chan = 8;
	} else {
		s->subdev_flags = SDF_READABLE | SDF_GROUND;
		s->n_chan = 16;
	}

	if (devpriv->unipolar)
		s->range_table = &range_adq12b_ai_unipolar;
	else
		s->range_table = &range_adq12b_ai_bipolar;

	s->maxdata = 0xfff;

	s->len_chanlist = 4;	/*                                         
                            */
	s->insn_read = adq12b_ai_rinsn;

	s = &dev->subdevices[1];
	/*                         */
	s->type = COMEDI_SUBD_DI;
	s->subdev_flags = SDF_READABLE;
	s->n_chan = 5;
	s->maxdata = 1;
	s->range_table = &range_digital;
	s->insn_bits = adq12b_di_insn_bits;

	s = &dev->subdevices[2];
	/*                          */
	s->type = COMEDI_SUBD_DO;
	s->subdev_flags = SDF_WRITABLE;
	s->n_chan = 8;
	s->maxdata = 1;
	s->range_table = &range_digital;
	s->insn_bits = adq12b_do_insn_bits;

	return 0;
}

static struct comedi_driver adq12b_driver = {
	.driver_name	= "adq12b",
	.module		= THIS_MODULE,
	.attach		= adq12b_attach,
	.detach		= comedi_legacy_detach,
};
module_comedi_driver(adq12b_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
