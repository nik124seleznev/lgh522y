/*
 * comedi/drivers/cb_pcidda.c
 * Driver for the ComputerBoards / MeasurementComputing PCI-DDA series.
 *
 * Copyright (C) 2001 Ivan Martinez <ivanmr@altavista.com>
 * Copyright (C) 2001 Frank Mori Hess <fmhess@users.sourceforge.net>
 *
 * COMEDI - Linux Control and Measurement Device Interface
 * Copyright (C) 1997-8 David A. Schleef <ds@schleef.org>
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

/*
                    
                                                   
                                                               
                                                          
                                                          
                                                          
                                                          
                                                          
                                               
                                                    
                
  
                                                              
  
                                                  
 */

#include <linux/pci.h>

#include "../comedidev.h"

#include "comedi_fc.h"
#include "8255.h"

#define EEPROM_SIZE	128	/*                              */
/*                                                    */
#define MAX_AO_CHANNELS 8

/*                       */
#define CB_DDA_DIO0_8255_BASE		0x00
#define CB_DDA_DIO1_8255_BASE		0x04

/*               */
#define CB_DDA_DA_CTRL_REG		0x00	   /*                       */
#define CB_DDA_DA_CTRL_SU		(1 << 0)   /*                       */
#define CB_DDA_DA_CTRL_EN		(1 << 1)   /*                       */
#define CB_DDA_DA_CTRL_DAC(x)		((x) << 2) /*                       */
#define CB_DDA_DA_CTRL_RANGE2V5		(0 << 6)   /*                       */
#define CB_DDA_DA_CTRL_RANGE5V		(2 << 6)   /*                       */
#define CB_DDA_DA_CTRL_RANGE10V		(3 << 6)   /*                       */
#define CB_DDA_DA_CTRL_UNIP		(1 << 8)   /*                       */

#define DACALIBRATION1	4	/*                             */
/*            */
/*                                                      */
#define SERIAL_IN_BIT   0x1
#define	CAL_CHANNEL_MASK	(0x7 << 1)
#define	CAL_CHANNEL_BITS(channel)	(((channel) << 1) & CAL_CHANNEL_MASK)
/*           */
#define	CAL_COUNTER_MASK	0x1f
/*                                         */
#define CAL_COUNTER_OVERFLOW_BIT        0x20
/*                                                  */
#define AO_BELOW_REF_BIT        0x40
#define	SERIAL_OUT_BIT	0x80	/*                                           */

#define DACALIBRATION2	6	/*                             */
#define	SELECT_EEPROM_BIT	0x1	/*                                */
/*                                                */
#define DESELECT_REF_DAC_BIT    0x2
/*                                    */
#define DESELECT_CALDAC_BIT(n)  (0x4 << (n))
/*                                                 */
#define DUMMY_BIT       0x40

#define CB_DDA_DA_DATA_REG(x)		(0x08 + ((x) * 2))

/*                                 */
#define CB_DDA_CALDAC_FINE_GAIN		0
#define CB_DDA_CALDAC_COURSE_GAIN	1
#define CB_DDA_CALDAC_COURSE_OFFSET	2
#define CB_DDA_CALDAC_FINE_OFFSET	3

static const struct comedi_lrange cb_pcidda_ranges = {
	6, {
		BIP_RANGE(10),
		BIP_RANGE(5),
		BIP_RANGE(2.5),
		UNI_RANGE(10),
		UNI_RANGE(5),
		UNI_RANGE(2.5)
	}
};

enum cb_pcidda_boardid {
	BOARD_DDA02_12,
	BOARD_DDA04_12,
	BOARD_DDA08_12,
	BOARD_DDA02_16,
	BOARD_DDA04_16,
	BOARD_DDA08_16,
};

struct cb_pcidda_board {
	const char *name;
	int ao_chans;
	int ao_bits;
};

static const struct cb_pcidda_board cb_pcidda_boards[] = {
	[BOARD_DDA02_12] = {
		.name		= "pci-dda02/12",
		.ao_chans	= 2,
		.ao_bits	= 12,
	},
	[BOARD_DDA04_12] = {
		.name		= "pci-dda04/12",
		.ao_chans	= 4,
		.ao_bits	= 12,
	},
	[BOARD_DDA08_12] = {
		.name		= "pci-dda08/12",
		.ao_chans	= 8,
		.ao_bits	= 12,
	},
	[BOARD_DDA02_16] = {
		.name		= "pci-dda02/16",
		.ao_chans	= 2,
		.ao_bits	= 16,
	},
	[BOARD_DDA04_16] = {
		.name		= "pci-dda04/16",
		.ao_chans	= 4,
		.ao_bits	= 16,
	},
	[BOARD_DDA08_16] = {
		.name		= "pci-dda08/16",
		.ao_chans	= 8,
		.ao_bits	= 16,
	},
};

struct cb_pcidda_private {
	/*                                                */
	unsigned int dac_cal1_bits;
	/*                                            */
	unsigned int ao_range[MAX_AO_CHANNELS];
	u16 eeprom_data[EEPROM_SIZE];	/*                                  */
};

/*                           */
static unsigned int cb_pcidda_serial_in(struct comedi_device *dev)
{
	unsigned int value = 0;
	int i;
	const int value_width = 16;	/*                                 */

	for (i = 1; i <= value_width; i++) {
		/*                                       */
		if (inw_p(dev->iobase + DACALIBRATION1) & SERIAL_OUT_BIT)
			value |= 1 << (value_width - i);
	}

	return value;
}

/*                              */
static void cb_pcidda_serial_out(struct comedi_device *dev, unsigned int value,
				 unsigned int num_bits)
{
	struct cb_pcidda_private *devpriv = dev->private;
	int i;

	for (i = 1; i <= num_bits; i++) {
		/*                                       */
		if (value & (1 << (num_bits - i)))
			devpriv->dac_cal1_bits |= SERIAL_IN_BIT;
		else
			devpriv->dac_cal1_bits &= ~SERIAL_IN_BIT;
		outw_p(devpriv->dac_cal1_bits, dev->iobase + DACALIBRATION1);
	}
}

/*                                          */
static unsigned int cb_pcidda_read_eeprom(struct comedi_device *dev,
					  unsigned int address)
{
	unsigned int i;
	unsigned int cal2_bits;
	unsigned int value;
	/*                                       */
	const int max_num_caldacs = 4;
	/*                                             */
	const int read_instruction = 0x6;
	const int instruction_length = 3;
	const int address_length = 8;

	/*                                      */
	cal2_bits = SELECT_EEPROM_BIT | DESELECT_REF_DAC_BIT | DUMMY_BIT;
	/*                                                         */
	for (i = 0; i < max_num_caldacs; i++)
		cal2_bits |= DESELECT_CALDAC_BIT(i);
	outw_p(cal2_bits, dev->iobase + DACALIBRATION2);

	/*                              */
	cb_pcidda_serial_out(dev, read_instruction, instruction_length);
	/*                                    */
	cb_pcidda_serial_out(dev, address, address_length);

	value = cb_pcidda_serial_in(dev);

	/*                    */
	cal2_bits &= ~SELECT_EEPROM_BIT;
	outw_p(cal2_bits, dev->iobase + DACALIBRATION2);

	return value;
}

/*                                  */
static void cb_pcidda_write_caldac(struct comedi_device *dev,
				   unsigned int caldac, unsigned int channel,
				   unsigned int value)
{
	unsigned int cal2_bits;
	unsigned int i;
	/*                                         */
	const int num_channel_bits = 3;
	const int num_caldac_bits = 8;	/*                         */
	/*                                       */
	const int max_num_caldacs = 4;

	/*                     */
	cb_pcidda_serial_out(dev, channel, num_channel_bits);
	/*                           */
	cb_pcidda_serial_out(dev, value, num_caldac_bits);

/*
                                                             
*/
	cal2_bits = DESELECT_REF_DAC_BIT | DUMMY_BIT;
	/*                                                         */
	for (i = 0; i < max_num_caldacs; i++)
		cal2_bits |= DESELECT_CALDAC_BIT(i);
	/*                              */
	cal2_bits &= ~DESELECT_CALDAC_BIT(caldac);
	outw_p(cal2_bits, dev->iobase + DACALIBRATION2);
	/*                    */
	cal2_bits |= DESELECT_CALDAC_BIT(caldac);
	outw_p(cal2_bits, dev->iobase + DACALIBRATION2);
}

/*                                                          */
static void cb_pcidda_calibrate(struct comedi_device *dev, unsigned int channel,
				unsigned int range)
{
	struct cb_pcidda_private *devpriv = dev->private;
	unsigned int caldac = channel / 2;	/*                         */
	unsigned int chan = 4 * (channel % 2);	/*                     */
	unsigned int index = 2 * range + 12 * channel;
	unsigned int offset;
	unsigned int gain;

	/*                                                                */
	devpriv->ao_range[channel] = range;

	/*                             */
	offset = devpriv->eeprom_data[0x7 + index];
	gain = devpriv->eeprom_data[0x8 + index];

	/*             */
	cb_pcidda_write_caldac(dev, caldac, chan + CB_DDA_CALDAC_COURSE_OFFSET,
			       (offset >> 8) & 0xff);
	cb_pcidda_write_caldac(dev, caldac, chan + CB_DDA_CALDAC_FINE_OFFSET,
			       offset & 0xff);
	cb_pcidda_write_caldac(dev, caldac, chan + CB_DDA_CALDAC_COURSE_GAIN,
			       (gain >> 8) & 0xff);
	cb_pcidda_write_caldac(dev, caldac, chan + CB_DDA_CALDAC_FINE_GAIN,
			       gain & 0xff);
}

static int cb_pcidda_ao_insn_write(struct comedi_device *dev,
				   struct comedi_subdevice *s,
				   struct comedi_insn *insn,
				   unsigned int *data)
{
	struct cb_pcidda_private *devpriv = dev->private;
	unsigned int channel = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
	unsigned int ctrl;

	if (range != devpriv->ao_range[channel])
		cb_pcidda_calibrate(dev, channel, range);

	ctrl = CB_DDA_DA_CTRL_EN | CB_DDA_DA_CTRL_DAC(channel);

	switch (range) {
	case 0:
	case 3:
		ctrl |= CB_DDA_DA_CTRL_RANGE10V;
		break;
	case 1:
	case 4:
		ctrl |= CB_DDA_DA_CTRL_RANGE5V;
		break;
	case 2:
	case 5:
		ctrl |= CB_DDA_DA_CTRL_RANGE2V5;
		break;
	}

	if (range > 2)
		ctrl |= CB_DDA_DA_CTRL_UNIP;

	outw(ctrl, dev->iobase + CB_DDA_DA_CTRL_REG);

	outw(data[0], dev->iobase + CB_DDA_DA_DATA_REG(channel));

	return insn->n;
}

static int cb_pcidda_auto_attach(struct comedi_device *dev,
				 unsigned long context)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	const struct cb_pcidda_board *thisboard = NULL;
	struct cb_pcidda_private *devpriv;
	struct comedi_subdevice *s;
	unsigned long iobase_8255;
	int i;
	int ret;

	if (context < ARRAY_SIZE(cb_pcidda_boards))
		thisboard = &cb_pcidda_boards[context];
	if (!thisboard)
		return -ENODEV;
	dev->board_ptr = thisboard;
	dev->board_name = thisboard->name;

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;
	dev->iobase = pci_resource_start(pcidev, 3);
	iobase_8255 = pci_resource_start(pcidev, 2);

	ret = comedi_alloc_subdevices(dev, 3);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	/*                         */
	s->type = COMEDI_SUBD_AO;
	s->subdev_flags = SDF_WRITABLE;
	s->n_chan = thisboard->ao_chans;
	s->maxdata = (1 << thisboard->ao_bits) - 1;
	s->range_table = &cb_pcidda_ranges;
	s->insn_write = cb_pcidda_ao_insn_write;

	/*                                */
	for (i = 0; i < 2; i++) {
		s = &dev->subdevices[1 + i];
		ret = subdev_8255_init(dev, s, NULL, iobase_8255 + (i * 4));
		if (ret)
			return ret;
	}

	/*                             */
	for (i = 0; i < EEPROM_SIZE; i++)
		devpriv->eeprom_data[i] = cb_pcidda_read_eeprom(dev, i);

	/*                        */
	for (i = 0; i < thisboard->ao_chans; i++)
		cb_pcidda_calibrate(dev, i, devpriv->ao_range[i]);

	dev_info(dev->class_dev, "%s attached\n", dev->board_name);

	return 0;
}

static void cb_pcidda_detach(struct comedi_device *dev)
{
	comedi_spriv_free(dev, 1);
	comedi_spriv_free(dev, 2);
	comedi_pci_disable(dev);
}

static struct comedi_driver cb_pcidda_driver = {
	.driver_name	= "cb_pcidda",
	.module		= THIS_MODULE,
	.auto_attach	= cb_pcidda_auto_attach,
	.detach		= cb_pcidda_detach,
};

static int cb_pcidda_pci_probe(struct pci_dev *dev,
			       const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &cb_pcidda_driver,
				      id->driver_data);
}

static DEFINE_PCI_DEVICE_TABLE(cb_pcidda_pci_table) = {
	{ PCI_VDEVICE(CB, 0x0020), BOARD_DDA02_12 },
	{ PCI_VDEVICE(CB, 0x0021), BOARD_DDA04_12 },
	{ PCI_VDEVICE(CB, 0x0022), BOARD_DDA08_12 },
	{ PCI_VDEVICE(CB, 0x0023), BOARD_DDA02_16 },
	{ PCI_VDEVICE(CB, 0x0024), BOARD_DDA04_16 },
	{ PCI_VDEVICE(CB, 0x0025), BOARD_DDA08_16 },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, cb_pcidda_pci_table);

static struct pci_driver cb_pcidda_pci_driver = {
	.name		= "cb_pcidda",
	.id_table	= cb_pcidda_pci_table,
	.probe		= cb_pcidda_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(cb_pcidda_driver, cb_pcidda_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
