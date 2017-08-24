/*
   comedi/drivers/c6xdigio.c

   Hardware driver for Mechatronic Systems Inc. C6x_DIGIO DSP daughter card.
   (http://robot0.ge.uiuc.edu/~spong/mecha/)

   COMEDI - Linux Control and Measurement Device Interface
   Copyright (C) 1999 Dan Block

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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/timex.h>
#include <linux/timer.h>
#include <linux/io.h>
#include <linux/pnp.h>

#include "../comedidev.h"

static u8 ReadByteFromHwPort(unsigned long addr)
{
	u8 result = inb(addr);
	return result;
}

static void WriteByteToHwPort(unsigned long addr, u8 val)
{
	outb_p(val, addr);
}

#define C6XDIGIO_SIZE 3

/*
               
 */
#define C6XDIGIO_PARALLEL_DATA 0
#define C6XDIGIO_PARALLEL_STATUS 1
#define C6XDIGIO_PARALLEL_CONTROL 2
struct pwmbitstype {
	unsigned sb0:2;
	unsigned sb1:2;
	unsigned sb2:2;
	unsigned sb3:2;
	unsigned sb4:2;
};
union pwmcmdtype {
	unsigned cmd;		/*                                  */
	struct pwmbitstype bits;
};
struct encbitstype {
	unsigned sb0:3;
	unsigned sb1:3;
	unsigned sb2:3;
	unsigned sb3:3;
	unsigned sb4:3;
	unsigned sb5:3;
	unsigned sb6:3;
	unsigned sb7:3;
};
union encvaluetype {
	unsigned value;
	struct encbitstype bits;
};

#define C6XDIGIO_TIME_OUT 20

static void C6X_pwmInit(unsigned long baseAddr)
{
	int timeout = 0;

/*                                 */

	WriteByteToHwPort(baseAddr, 0x70);
	while (((ReadByteFromHwPort(baseAddr + 1) & 0x80) == 0)
	       && (timeout < C6XDIGIO_TIME_OUT)) {
		timeout++;
	}

	WriteByteToHwPort(baseAddr, 0x74);
	timeout = 0;
	while (((ReadByteFromHwPort(baseAddr + 1) & 0x80) == 0x80)
	       && (timeout < C6XDIGIO_TIME_OUT)) {
		timeout++;
	}

	WriteByteToHwPort(baseAddr, 0x70);
	timeout = 0;
	while (((ReadByteFromHwPort(baseAddr + 1) & 0x80) == 0x0)
	       && (timeout < C6XDIGIO_TIME_OUT)) {
		timeout++;
	}

	WriteByteToHwPort(baseAddr, 0x0);
	timeout = 0;
	while (((ReadByteFromHwPort(baseAddr + 1) & 0x80) == 0x80)
	       && (timeout < C6XDIGIO_TIME_OUT)) {
		timeout++;
	}

}

static void C6X_pwmOutput(unsigned long baseAddr, unsigned channel, int value)
{
	unsigned ppcmd;
	union pwmcmdtype pwm;
	int timeout = 0;
	unsigned tmp;

	/*                                   */

	pwm.cmd = value;
	if (pwm.cmd > 498)
		pwm.cmd = 498;
	if (pwm.cmd < 2)
		pwm.cmd = 2;

	if (channel == 0) {
		ppcmd = 0x28;
	} else {		/*                  */
		ppcmd = 0x30;
	}			/*       */

	WriteByteToHwPort(baseAddr, ppcmd + pwm.bits.sb0);
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}

	WriteByteToHwPort(baseAddr, ppcmd + pwm.bits.sb1 + 0x4);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x80) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}

	WriteByteToHwPort(baseAddr, ppcmd + pwm.bits.sb2);
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}

	WriteByteToHwPort(baseAddr, ppcmd + pwm.bits.sb3 + 0x4);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x80) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}

	WriteByteToHwPort(baseAddr, ppcmd + pwm.bits.sb4);
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}

	WriteByteToHwPort(baseAddr, 0x0);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x80) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}

}

static int C6X_encInput(unsigned long baseAddr, unsigned channel)
{
	unsigned ppcmd;
	union encvaluetype enc;
	int timeout = 0;
	int tmp;

	/*                                  */

	enc.value = 0;
	if (channel == 0)
		ppcmd = 0x48;
	else
		ppcmd = 0x50;

	WriteByteToHwPort(baseAddr, ppcmd);
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}

	enc.bits.sb0 = ((ReadByteFromHwPort(baseAddr + 1) >> 3) & 0x7);
	WriteByteToHwPort(baseAddr, ppcmd + 0x4);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x80) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}
	enc.bits.sb1 = ((ReadByteFromHwPort(baseAddr + 1) >> 3) & 0x7);
	WriteByteToHwPort(baseAddr, ppcmd);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}
	enc.bits.sb2 = ((ReadByteFromHwPort(baseAddr + 1) >> 3) & 0x7);
	WriteByteToHwPort(baseAddr, ppcmd + 0x4);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x80) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}
	enc.bits.sb3 = ((ReadByteFromHwPort(baseAddr + 1) >> 3) & 0x7);
	WriteByteToHwPort(baseAddr, ppcmd);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}
	enc.bits.sb4 = ((ReadByteFromHwPort(baseAddr + 1) >> 3) & 0x7);
	WriteByteToHwPort(baseAddr, ppcmd + 0x4);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x80) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}
	enc.bits.sb5 = ((ReadByteFromHwPort(baseAddr + 1) >> 3) & 0x7);
	WriteByteToHwPort(baseAddr, ppcmd);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x0) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}
	enc.bits.sb6 = ((ReadByteFromHwPort(baseAddr + 1) >> 3) & 0x7);
	WriteByteToHwPort(baseAddr, ppcmd + 0x4);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x80) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}
	enc.bits.sb7 = ((ReadByteFromHwPort(baseAddr + 1) >> 3) & 0x7);
	WriteByteToHwPort(baseAddr, ppcmd);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x0) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}

	WriteByteToHwPort(baseAddr, 0x0);
	timeout = 0;
	tmp = ReadByteFromHwPort(baseAddr + 1);
	while (((tmp & 0x80) == 0x80) && (timeout < C6XDIGIO_TIME_OUT)) {
		tmp = ReadByteFromHwPort(baseAddr + 1);
		timeout++;
	}

	return enc.value ^ 0x800000;
}

static void C6X_encResetAll(unsigned long baseAddr)
{
	unsigned timeout = 0;

/*                                     */

	WriteByteToHwPort(baseAddr, 0x68);
	while (((ReadByteFromHwPort(baseAddr + 1) & 0x80) == 0)
	       && (timeout < C6XDIGIO_TIME_OUT)) {
		timeout++;
	}
	WriteByteToHwPort(baseAddr, 0x6C);
	timeout = 0;
	while (((ReadByteFromHwPort(baseAddr + 1) & 0x80) == 0x80)
	       && (timeout < C6XDIGIO_TIME_OUT)) {
		timeout++;
	}
	WriteByteToHwPort(baseAddr, 0x68);
	timeout = 0;
	while (((ReadByteFromHwPort(baseAddr + 1) & 0x80) == 0x0)
	       && (timeout < C6XDIGIO_TIME_OUT)) {
		timeout++;
	}
	WriteByteToHwPort(baseAddr, 0x0);
	timeout = 0;
	while (((ReadByteFromHwPort(baseAddr + 1) & 0x80) == 0x80)
	       && (timeout < C6XDIGIO_TIME_OUT)) {
		timeout++;
	}
}

static int c6xdigio_pwmo_insn_read(struct comedi_device *dev,
				   struct comedi_subdevice *s,
				   struct comedi_insn *insn, unsigned int *data)
{
	printk(KERN_DEBUG "c6xdigio_pwmo_insn_read %x\n", insn->n);
	return insn->n;
}

static int c6xdigio_pwmo_insn_write(struct comedi_device *dev,
				    struct comedi_subdevice *s,
				    struct comedi_insn *insn,
				    unsigned int *data)
{
	int i;
	int chan = CR_CHAN(insn->chanspec);

	/*                                                     */
	for (i = 0; i < insn->n; i++) {
		C6X_pwmOutput(dev->iobase, chan, data[i]);
		/*                                          */
	}
	return i;
}

/*                                                                  */
/*                             */
/*                           */
/*                     */
/*   */
/*                                                     */
/*                 */
/*   */

/*                                                                   */
/*                             */
/*                           */
/*                     */
/*   */
/*        */
/*                                     */
      /*  *//*                                 */
      /*  *//*                 */
/*   */

static int c6xdigio_ei_insn_read(struct comedi_device *dev,
				 struct comedi_subdevice *s,
				 struct comedi_insn *insn, unsigned int *data)
{
	/*                                                   */
	int n;
	int chan = CR_CHAN(insn->chanspec);

	for (n = 0; n < insn->n; n++)
		data[n] = (C6X_encInput(dev->iobase, chan) & 0xffffff);

	return n;
}

static void board_init(struct comedi_device *dev)
{

	/*                                */

	C6X_pwmInit(dev->iobase);
	C6X_encResetAll(dev->iobase);

}

static const struct pnp_device_id c6xdigio_pnp_tbl[] = {
	/*                           */
	{.id = "PNP0400", .driver_data = 0},
	/*                  */
	{.id = "PNP0401", .driver_data = 0},
	{}
};

static struct pnp_driver c6xdigio_pnp_driver = {
	.name = "c6xdigio",
	.id_table = c6xdigio_pnp_tbl,
};

static int c6xdigio_attach(struct comedi_device *dev,
			   struct comedi_devconfig *it)
{
	struct comedi_subdevice *s;
	int ret;

	ret = comedi_request_region(dev, it->options[0], C6XDIGIO_SIZE);
	if (ret)
		return ret;

	ret = comedi_alloc_subdevices(dev, 2);
	if (ret)
		return ret;

	/*                                         */
	pnp_register_driver(&c6xdigio_pnp_driver);

	s = &dev->subdevices[0];
	/*                      */
	s->type = COMEDI_SUBD_AO;	/*                            */
	s->subdev_flags = SDF_WRITEABLE;
	s->n_chan = 2;
	/*                                  */
	s->insn_read = c6xdigio_pwmo_insn_read;
	s->insn_write = c6xdigio_pwmo_insn_write;
	s->maxdata = 500;
	s->range_table = &range_bipolar10;	/*                 */

	s = &dev->subdevices[1];
	/*                             */
	s->type = COMEDI_SUBD_COUNTER;
	s->subdev_flags = SDF_READABLE | SDF_LSAMPL;
	s->n_chan = 2;
	/*                           */
	s->insn_read = c6xdigio_ei_insn_read;
	s->maxdata = 0xffffff;
	s->range_table = &range_unknown;

	/*                          */
	/*                      */
	/*                                                              */
	/*                                  */
	/*                */
	/*                                */
	/*                                            */
	/*                                              */
	/*                                                   */
	/*                                                               */

	/*                                                                  */
	/*                                                      */
	board_init(dev);

	return 0;
}

static void c6xdigio_detach(struct comedi_device *dev)
{
	comedi_legacy_detach(dev);
	pnp_unregister_driver(&c6xdigio_pnp_driver);
}

static struct comedi_driver c6xdigio_driver = {
	.driver_name	= "c6xdigio",
	.module		= THIS_MODULE,
	.attach		= c6xdigio_attach,
	.detach		= c6xdigio_detach,
};
module_comedi_driver(c6xdigio_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");