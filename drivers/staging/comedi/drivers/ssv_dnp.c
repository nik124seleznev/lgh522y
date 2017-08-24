/*
    comedi/drivers/ssv_dnp.c
    generic comedi driver for SSV Embedded Systems' DIL/Net-PCs
    Copyright (C) 2001 Robert Schwebel <robert@schwebel.de>

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

/*                                                                           */

#include "../comedidev.h"

/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */

#define CSCIR 0x22		/*                                           */
#define CSCDR 0x23		/*                                           */
#define PAMR  0xa5		/*                                           */
#define PADR  0xa9		/*                                           */
#define PBMR  0xa4		/*                                           */
#define PBDR  0xa8		/*                                           */
#define PCMR  0xa3		/*                                           */
#define PCDR  0xa7		/*                                           */

/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */

static int dnp_dio_insn_bits(struct comedi_device *dev,
			     struct comedi_subdevice *s,
			     struct comedi_insn *insn, unsigned int *data)
{
	/*                                                                   */
	/*                                                                   */

	/*                                                                   */
	/*                                                                   */
	/*                                                                   */

	if (data[0]) {

		outb(PADR, CSCIR);
		outb((inb(CSCDR)
		      & ~(u8) (data[0] & 0x0000FF))
		     | (u8) (data[1] & 0x0000FF), CSCDR);

		outb(PBDR, CSCIR);
		outb((inb(CSCDR)
		      & ~(u8) ((data[0] & 0x00FF00) >> 8))
		     | (u8) ((data[1] & 0x00FF00) >> 8), CSCDR);

		outb(PCDR, CSCIR);
		outb((inb(CSCDR)
		      & ~(u8) ((data[0] & 0x0F0000) >> 12))
		     | (u8) ((data[1] & 0x0F0000) >> 12), CSCDR);
	}

	/*                                                                   */
	outb(PADR, CSCIR);
	data[1] = inb(CSCDR);
	outb(PBDR, CSCIR);
	data[1] += inb(CSCDR) << 8;
	outb(PCDR, CSCIR);
	data[1] += ((inb(CSCDR) & 0xF0) << 12);

	return insn->n;

}

/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */

static int dnp_dio_insn_config(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_insn *insn, unsigned int *data)
{

	u8 register_buffer;

	/*                                   */
	int chan = CR_CHAN(insn->chanspec);

	switch (data[0]) {
	case INSN_CONFIG_DIO_OUTPUT:
	case INSN_CONFIG_DIO_INPUT:
		break;
	case INSN_CONFIG_DIO_QUERY:
		data[1] =
		    (inb(CSCDR) & (1 << chan)) ? COMEDI_OUTPUT : COMEDI_INPUT;
		return insn->n;
		break;
	default:
		return -EINVAL;
		break;
	}
	/*                                                                    */

	/*                                                                    */
	/*                                                                    */
	/*                                                                    */

	if ((chan >= 0) && (chan <= 7)) {
		/*                */
		outb(PAMR, CSCIR);
	} else if ((chan >= 8) && (chan <= 15)) {
		/*                */
		chan -= 8;
		outb(PBMR, CSCIR);
	} else if ((chan >= 16) && (chan <= 19)) {
		/*                                                            */
		/*                                                            */
		chan -= 16;
		chan *= 2;
		outb(PCMR, CSCIR);
	} else {
		return -EINVAL;
	}

	/*                                                                    */
	register_buffer = inb(CSCDR);
	if (data[0] == COMEDI_OUTPUT)
		register_buffer |= (1 << chan);
	else
		register_buffer &= ~(1 << chan);

	outb(register_buffer, CSCDR);

	return 1;

}

static int dnp_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	struct comedi_subdevice *s;
	int ret;

	ret = comedi_alloc_subdevices(dev, 1);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	/*                                                                   */
	s->type = COMEDI_SUBD_DIO;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE;
	s->n_chan = 20;
	s->maxdata = 1;
	s->range_table = &range_digital;
	s->insn_bits = dnp_dio_insn_bits;
	s->insn_config = dnp_dio_insn_config;

	/*                                                               
                                                                     
               */

	/*                                                                   */
	outb(PAMR, CSCIR);
	outb(0x00, CSCDR);
	outb(PBMR, CSCIR);
	outb(0x00, CSCDR);
	outb(PCMR, CSCIR);
	outb((inb(CSCDR) & 0xAA), CSCDR);

	dev_info(dev->class_dev, "%s: attached\n", dev->board_name);
	return 1;
}

static void dnp_detach(struct comedi_device *dev)
{
	outb(PAMR, CSCIR);
	outb(0x00, CSCDR);
	outb(PBMR, CSCIR);
	outb(0x00, CSCDR);
	outb(PCMR, CSCIR);
	outb((inb(CSCDR) & 0xAA), CSCDR);
}

static struct comedi_driver dnp_driver = {
	.driver_name	= "dnp-1486",
	.module		= THIS_MODULE,
	.attach		= dnp_attach,
	.detach		= dnp_detach,
};
module_comedi_driver(dnp_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
