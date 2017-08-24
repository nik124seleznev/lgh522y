/*
    comedi/drivers/cb_pcidas.c

    Developed by Ivan Martinez and Frank Mori Hess, with valuable help from
    David Schleef and the rest of the Comedi developers comunity.

    Copyright (C) 2001-2003 Ivan Martinez <imr@oersted.dtu.dk>
    Copyright (C) 2001,2002 Frank Mori Hess <fmhess@users.sourceforge.net>

    COMEDI - Linux Control and Measurement Device Interface
    Copyright (C) 1997-8 David A. Schleef <ds@schleef.org>

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

************************************************************************
*/
/*
                 
                                                
                                    
                                           
                                                
                  
                                                            
                                                               
                                       

       
                                                                  
                                                                
                                                     

                                                             
          

                                                           

                                                      
                                                     
               

              
                                                                           
                                                            
                                                                   
                                                                  
                                                    
                                                     
                                                               
*/
/*

     

                                
*/

#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#include "../comedidev.h"

#include "8253.h"
#include "8255.h"
#include "amcc_s5933.h"
#include "comedi_fc.h"

#define TIMER_BASE		100	/*                    */
#define AI_BUFFER_SIZE		1024	/*                  */
#define AO_BUFFER_SIZE		1024	/*                  */
#define NUM_CHANNELS_8800	8
#define NUM_CHANNELS_7376	1
#define NUM_CHANNELS_8402	2
#define NUM_CHANNELS_DAC08	1

/*                          */
#define INT_ADCFIFO		0	/*                               */
#define   INT_EOS		0x1	/*                 */
#define   INT_FHF		0x2	/*                    */
#define   INT_FNE		0x3	/*                    */
#define   INT_MASK		0x3	/*                         */
#define   INTE			0x4	/*            */
#define   DAHFIE		0x8	/*                          */
#define   EOAIE			0x10	/*                        */
#define   DAHFI			0x20	/*                              */
#define   EOAI			0x40	/*                                */
#define   INT			0x80	/*                    */
#define   EOBI			0x200	/*                         */
#define   ADHFI			0x400	/*                      */
#define   ADNEI			0x800	/*                                   */
#define   ADNE			0x1000	/*                                  */
#define   DAEMIE		0x1000	/*                      */
#define   LADFUL		0x2000	/*                       */
#define   DAEMI			0x4000	/*                                   */

#define ADCMUX_CONT		2	/*                                 */
#define   BEGIN_SCAN(x)		((x) & 0xf)
#define   END_SCAN(x)		(((x) & 0xf) << 4)
#define   GAIN_BITS(x)		(((x) & 0x3) << 8)
#define   UNIP			0x800	/*                                */
#define   SE			0x400	/*                             */
#define   PACER_MASK		0x3000	/*                   */
#define   PACER_INT		0x1000	/*            */
#define   PACER_EXT_FALL	0x2000	/*                   */
#define   PACER_EXT_RISE	0x3000	/*                  */
#define   EOC			0x4000	/*              */

#define TRIG_CONTSTAT		 4	/*                                 */
#define   SW_TRIGGER		0x1	/*                        */
#define   EXT_TRIGGER		0x2	/*                    */
#define   ANALOG_TRIGGER	0x3	/*                     */
#define   TRIGGER_MASK		0x3	/*                    */
#define   TGPOL			0x04	/*                            */
#define   TGSEL			0x08	/*                                  */
#define   TGEN			0x10	/*                               */
#define   BURSTE		0x20	/*                   */
#define   XTRCL			0x80	/*                        */

#define CALIBRATION_REG		6	/*                      */
#define   SELECT_8800_BIT	0x100	/*                    */
#define   SELECT_TRIMPOT_BIT	0x200	/*                        */
#define   SELECT_DAC08_BIT	0x400	/*                     */
#define   CAL_SRC_BITS(x)	(((x) & 0x7) << 11)
#define   CAL_EN_BIT		0x4000	/*                           */
#define   SERIAL_DATA_IN_BIT	0x8000	/*                                 */

#define DAC_CSR			0x8	/*                                 */
#define   DACEN			0x02	/*            */
#define   DAC_MODE_UPDATE_BOTH	0x80	/*                  */

static inline unsigned int DAC_RANGE(unsigned int channel, unsigned int range)
{
	return (range & 0x3) << (8 + 2 * (channel & 0x1));
}

static inline unsigned int DAC_RANGE_MASK(unsigned int channel)
{
	return 0x3 << (8 + 2 * (channel & 0x1));
};

/*                           */
#define   DAC_EMPTY		0x1	/*                               */
#define   DAC_START		0x4	/*                           */
#define   DAC_PACER_MASK	0x18	/*                            */
#define   DAC_PACER_INT		0x8	/*             */
#define   DAC_PACER_EXT_FALL	0x10	/*                           */
#define   DAC_PACER_EXT_RISE	0x18	/*                          */

static inline unsigned int DAC_CHAN_EN(unsigned int channel)
{
	return 1 << (5 + (channel & 0x1));	/*                        */
};

/*                   */
#define ADCDATA			0	/*                   */
#define ADCFIFOCLR		2	/*                */

/*                               */
#define ADC8254			0
#define DIO_8255		4
#define DAC8254			8

/*                                               */
static inline unsigned int DAC_DATA_REG(unsigned int channel)
{
	return 2 * (channel & 0x1);
}

/*                                        */
#define DACDATA			0	/*                   */
#define DACFIFOCLR		2	/*                */

#define IS_UNIPOLAR		0x4	/*                     */

/*                                     */
static const struct comedi_lrange cb_pcidas_ranges = {
	8,
	{
	 BIP_RANGE(10),
	 BIP_RANGE(5),
	 BIP_RANGE(2.5),
	 BIP_RANGE(1.25),
	 UNI_RANGE(10),
	 UNI_RANGE(5),
	 UNI_RANGE(2.5),
	 UNI_RANGE(1.25)
	 }
};

/*                          */
static const struct comedi_lrange cb_pcidas_alt_ranges = {
	8,
	{
	 BIP_RANGE(10),
	 BIP_RANGE(1),
	 BIP_RANGE(0.1),
	 BIP_RANGE(0.01),
	 UNI_RANGE(10),
	 UNI_RANGE(1),
	 UNI_RANGE(0.1),
	 UNI_RANGE(0.01)
	 }
};

/*                      */
static const struct comedi_lrange cb_pcidas_ao_ranges = {
	4,
	{
	 BIP_RANGE(5),
	 BIP_RANGE(10),
	 UNI_RANGE(5),
	 UNI_RANGE(10),
	 }
};

enum trimpot_model {
	AD7376,
	AD8402,
};

enum cb_pcidas_boardid {
	BOARD_PCIDAS1602_16,
	BOARD_PCIDAS1200,
	BOARD_PCIDAS1602_12,
	BOARD_PCIDAS1200_JR,
	BOARD_PCIDAS1602_16_JR,
	BOARD_PCIDAS1000,
	BOARD_PCIDAS1001,
	BOARD_PCIDAS1002,
};

struct cb_pcidas_board {
	const char *name;
	int ai_nchan;		/*                              */
	int ai_bits;		/*                          */
	int ai_speed;		/*                                  */
	int ao_nchan;		/*                                */
	int has_ao_fifo;	/*                         */
	int ao_scan_speed;	/*                                           */
	int fifo_size;		/*                                  */
	const struct comedi_lrange *ranges;
	enum trimpot_model trimpot;
	unsigned has_dac08:1;
	unsigned is_1602:1;
};

static const struct cb_pcidas_board cb_pcidas_boards[] = {
	[BOARD_PCIDAS1602_16] = {
		.name		= "pci-das1602/16",
		.ai_nchan	= 16,
		.ai_bits	= 16,
		.ai_speed	= 5000,
		.ao_nchan	= 2,
		.has_ao_fifo	= 1,
		.ao_scan_speed	= 10000,
		.fifo_size	= 512,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD8402,
		.has_dac08	= 1,
		.is_1602	= 1,
	},
	[BOARD_PCIDAS1200] = {
		.name		= "pci-das1200",
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 3200,
		.ao_nchan	= 2,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
	},
	[BOARD_PCIDAS1602_12] = {
		.name		= "pci-das1602/12",
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 3200,
		.ao_nchan	= 2,
		.has_ao_fifo	= 1,
		.ao_scan_speed	= 4000,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
		.is_1602	= 1,
	},
	[BOARD_PCIDAS1200_JR] = {
		.name		= "pci-das1200/jr",
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 3200,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
	},
	[BOARD_PCIDAS1602_16_JR] = {
		.name		= "pci-das1602/16/jr",
		.ai_nchan	= 16,
		.ai_bits	= 16,
		.ai_speed	= 5000,
		.fifo_size	= 512,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD8402,
		.has_dac08	= 1,
		.is_1602	= 1,
	},
	[BOARD_PCIDAS1000] = {
		.name		= "pci-das1000",
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 4000,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
	},
	[BOARD_PCIDAS1001] = {
		.name		= "pci-das1001",
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 6800,
		.ao_nchan	= 2,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_alt_ranges,
		.trimpot	= AD7376,
	},
	[BOARD_PCIDAS1002] = {
		.name		= "pci-das1002",
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 6800,
		.ao_nchan	= 2,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
	},
};

struct cb_pcidas_private {
	/*                */
	unsigned long s5933_config;
	unsigned long control_status;
	unsigned long adc_fifo;
	unsigned long pacer_counter_dio;
	unsigned long ao_registers;
	/*                                                  */
	unsigned int divisor1;
	unsigned int divisor2;
	/*                                          */
	unsigned int count;
	/*                            */
	unsigned int adc_fifo_bits;
	unsigned int s5933_intcsr_bits;
	unsigned int ao_control_bits;
	/*              */
	short ai_buffer[AI_BUFFER_SIZE];
	short ao_buffer[AO_BUFFER_SIZE];
	/*                                                   */
	unsigned int ao_divisor1;
	unsigned int ao_divisor2;
	/*                                           */
	unsigned int ao_count;
	/*                            */
	int ao_value[2];
	unsigned int caldac_value[NUM_CHANNELS_8800];
	unsigned int trimpot_value[NUM_CHANNELS_8402];
	unsigned int dac08_value;
	unsigned int calibration_source;
};

static inline unsigned int cal_enable_bits(struct comedi_device *dev)
{
	struct cb_pcidas_private *devpriv = dev->private;

	return CAL_EN_BIT | CAL_SRC_BITS(devpriv->calibration_source);
}

static int cb_pcidas_ai_rinsn(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
	unsigned int aref = CR_AREF(insn->chanspec);
	unsigned int bits;
	int n, i;

	/*                                         */
	if (insn->chanspec & CR_ALT_SOURCE) {
		outw(cal_enable_bits(dev),
		     devpriv->control_status + CALIBRATION_REG);
		chan = 0;
	} else {
		outw(0, devpriv->control_status + CALIBRATION_REG);
	}

	/*                         */
	bits = BEGIN_SCAN(chan) | END_SCAN(chan) | GAIN_BITS(range);
	/*                      */
	if (range & IS_UNIPOLAR)
		bits |= UNIP;
	/*                               */
	if (aref != AREF_DIFF)
		bits |= SE;
	outw(bits, devpriv->control_status + ADCMUX_CONT);

	/*            */
	outw(0, devpriv->adc_fifo + ADCFIFOCLR);

	/*                   */
	for (n = 0; n < insn->n; n++) {
		/*                    */
		outw(0, devpriv->adc_fifo + ADCDATA);

		/*                            */
		/*                                         */
		for (i = 0; i < 10000; i++) {
			if (inw(devpriv->control_status + ADCMUX_CONT) & EOC)
				break;
		}
		if (i == 10000)
			return -ETIMEDOUT;

		/*           */
		data[n] = inw(devpriv->adc_fifo + ADCDATA);
	}

	/*                                           */
	return n;
}

static int ai_config_insn(struct comedi_device *dev, struct comedi_subdevice *s,
			  struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;
	int id = data[0];
	unsigned int source = data[1];

	switch (id) {
	case INSN_CONFIG_ALT_SOURCE:
		if (source >= 8) {
			dev_err(dev->class_dev,
				"invalid calibration source: %i\n",
				source);
			return -EINVAL;
		}
		devpriv->calibration_source = source;
		break;
	default:
		return -EINVAL;
		break;
	}
	return insn->n;
}

/*                                                    */
static int cb_pcidas_ao_nofifo_winsn(struct comedi_device *dev,
				     struct comedi_subdevice *s,
				     struct comedi_insn *insn,
				     unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
	unsigned long flags;

	/*                       */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->ao_control_bits &= (~DAC_MODE_UPDATE_BOTH &
				     ~DAC_RANGE_MASK(chan));
	devpriv->ao_control_bits |= (DACEN | DAC_RANGE(chan, range));
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*                             */
	devpriv->ao_value[chan] = data[0];

	/*           */
	outw(data[0], devpriv->ao_registers + DAC_DATA_REG(chan));

	return insn->n;
}

/*                                           */
static int cb_pcidas_ao_fifo_winsn(struct comedi_device *dev,
				   struct comedi_subdevice *s,
				   struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
	unsigned long flags;

	/*                */
	outw(0, devpriv->ao_registers + DACFIFOCLR);

	/*                       */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->ao_control_bits &= (~DAC_CHAN_EN(0) & ~DAC_CHAN_EN(1) &
				     ~DAC_RANGE_MASK(chan) & ~DAC_PACER_MASK);
	devpriv->ao_control_bits |= (DACEN | DAC_RANGE(chan, range) |
				     DAC_CHAN_EN(chan) | DAC_START);
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*                             */
	devpriv->ao_value[chan] = data[0];

	/*           */
	outw(data[0], devpriv->ao_registers + DACDATA);

	return insn->n;
}

static int cb_pcidas_ao_readback_insn(struct comedi_device *dev,
				      struct comedi_subdevice *s,
				      struct comedi_insn *insn,
				      unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;

	data[0] = devpriv->ao_value[CR_CHAN(insn->chanspec)];

	return 1;
}

static int wait_for_nvram_ready(unsigned long s5933_base_addr)
{
	static const int timeout = 1000;
	unsigned int i;

	for (i = 0; i < timeout; i++) {
		if ((inb(s5933_base_addr +
			 AMCC_OP_REG_MCSR_NVCMD) & MCSR_NV_BUSY)
		    == 0)
			return 0;
		udelay(1);
	}
	return -1;
}

static int nvram_read(struct comedi_device *dev, unsigned int address,
			uint8_t *data)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long iobase = devpriv->s5933_config;

	if (wait_for_nvram_ready(iobase) < 0)
		return -ETIMEDOUT;

	outb(MCSR_NV_ENABLE | MCSR_NV_LOAD_LOW_ADDR,
	     iobase + AMCC_OP_REG_MCSR_NVCMD);
	outb(address & 0xff, iobase + AMCC_OP_REG_MCSR_NVDATA);
	outb(MCSR_NV_ENABLE | MCSR_NV_LOAD_HIGH_ADDR,
	     iobase + AMCC_OP_REG_MCSR_NVCMD);
	outb((address >> 8) & 0xff, iobase + AMCC_OP_REG_MCSR_NVDATA);
	outb(MCSR_NV_ENABLE | MCSR_NV_READ, iobase + AMCC_OP_REG_MCSR_NVCMD);

	if (wait_for_nvram_ready(iobase) < 0)
		return -ETIMEDOUT;

	*data = inb(iobase + AMCC_OP_REG_MCSR_NVDATA);

	return 0;
}

static int eeprom_read_insn(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn, unsigned int *data)
{
	uint8_t nvram_data;
	int retval;

	retval = nvram_read(dev, CR_CHAN(insn->chanspec), &nvram_data);
	if (retval < 0)
		return retval;

	data[0] = nvram_data;

	return 1;
}

static void write_calibration_bitstream(struct comedi_device *dev,
					unsigned int register_bits,
					unsigned int bitstream,
					unsigned int bitstream_length)
{
	struct cb_pcidas_private *devpriv = dev->private;
	static const int write_delay = 1;
	unsigned int bit;

	for (bit = 1 << (bitstream_length - 1); bit; bit >>= 1) {
		if (bitstream & bit)
			register_bits |= SERIAL_DATA_IN_BIT;
		else
			register_bits &= ~SERIAL_DATA_IN_BIT;
		udelay(write_delay);
		outw(register_bits, devpriv->control_status + CALIBRATION_REG);
	}
}

static int caldac_8800_write(struct comedi_device *dev, unsigned int address,
			     uint8_t value)
{
	struct cb_pcidas_private *devpriv = dev->private;
	static const int num_caldac_channels = 8;
	static const int bitstream_length = 11;
	unsigned int bitstream = ((address & 0x7) << 8) | value;
	static const int caldac_8800_udelay = 1;

	if (address >= num_caldac_channels) {
		comedi_error(dev, "illegal caldac channel");
		return -1;
	}

	if (value == devpriv->caldac_value[address])
		return 1;

	devpriv->caldac_value[address] = value;

	write_calibration_bitstream(dev, cal_enable_bits(dev), bitstream,
				    bitstream_length);

	udelay(caldac_8800_udelay);
	outw(cal_enable_bits(dev) | SELECT_8800_BIT,
	     devpriv->control_status + CALIBRATION_REG);
	udelay(caldac_8800_udelay);
	outw(cal_enable_bits(dev), devpriv->control_status + CALIBRATION_REG);

	return 1;
}

static int caldac_write_insn(struct comedi_device *dev,
			     struct comedi_subdevice *s,
			     struct comedi_insn *insn, unsigned int *data)
{
	const unsigned int channel = CR_CHAN(insn->chanspec);

	return caldac_8800_write(dev, channel, data[0]);
}

static int caldac_read_insn(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;

	data[0] = devpriv->caldac_value[CR_CHAN(insn->chanspec)];

	return 1;
}

/*                        */
static void dac08_write(struct comedi_device *dev, unsigned int value)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long cal_reg;

	if (devpriv->dac08_value != value) {
		devpriv->dac08_value = value;

		cal_reg = devpriv->control_status + CALIBRATION_REG;

		value &= 0xff;
		value |= cal_enable_bits(dev);

		/*                                     */
		outw(value, cal_reg);
		udelay(1);
		outw(value | SELECT_DAC08_BIT, cal_reg);
		udelay(1);
		outw(value, cal_reg);
		udelay(1);
	}
}

static int dac08_write_insn(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn, unsigned int *data)
{
	int i;

	for (i = 0; i < insn->n; i++)
		dac08_write(dev, data[i]);

	return insn->n;
}

static int dac08_read_insn(struct comedi_device *dev,
			   struct comedi_subdevice *s, struct comedi_insn *insn,
			   unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;

	data[0] = devpriv->dac08_value;

	return 1;
}

static int trimpot_7376_write(struct comedi_device *dev, uint8_t value)
{
	struct cb_pcidas_private *devpriv = dev->private;
	static const int bitstream_length = 7;
	unsigned int bitstream = value & 0x7f;
	unsigned int register_bits;
	static const int ad7376_udelay = 1;

	register_bits = cal_enable_bits(dev) | SELECT_TRIMPOT_BIT;
	udelay(ad7376_udelay);
	outw(register_bits, devpriv->control_status + CALIBRATION_REG);

	write_calibration_bitstream(dev, register_bits, bitstream,
				    bitstream_length);

	udelay(ad7376_udelay);
	outw(cal_enable_bits(dev), devpriv->control_status + CALIBRATION_REG);

	return 0;
}

/*                 
                  
                              */
static int trimpot_8402_write(struct comedi_device *dev, unsigned int channel,
			      uint8_t value)
{
	struct cb_pcidas_private *devpriv = dev->private;
	static const int bitstream_length = 10;
	unsigned int bitstream = ((channel & 0x3) << 8) | (value & 0xff);
	unsigned int register_bits;
	static const int ad8402_udelay = 1;

	register_bits = cal_enable_bits(dev) | SELECT_TRIMPOT_BIT;
	udelay(ad8402_udelay);
	outw(register_bits, devpriv->control_status + CALIBRATION_REG);

	write_calibration_bitstream(dev, register_bits, bitstream,
				    bitstream_length);

	udelay(ad8402_udelay);
	outw(cal_enable_bits(dev), devpriv->control_status + CALIBRATION_REG);

	return 0;
}

static int cb_pcidas_trimpot_write(struct comedi_device *dev,
				   unsigned int channel, unsigned int value)
{
	const struct cb_pcidas_board *thisboard = comedi_board(dev);
	struct cb_pcidas_private *devpriv = dev->private;

	if (devpriv->trimpot_value[channel] == value)
		return 1;

	devpriv->trimpot_value[channel] = value;
	switch (thisboard->trimpot) {
	case AD7376:
		trimpot_7376_write(dev, value);
		break;
	case AD8402:
		trimpot_8402_write(dev, channel, value);
		break;
	default:
		comedi_error(dev, "driver bug?");
		return -1;
		break;
	}

	return 1;
}

static int trimpot_write_insn(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_insn *insn, unsigned int *data)
{
	unsigned int channel = CR_CHAN(insn->chanspec);

	return cb_pcidas_trimpot_write(dev, channel, data[0]);
}

static int trimpot_read_insn(struct comedi_device *dev,
			     struct comedi_subdevice *s,
			     struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int channel = CR_CHAN(insn->chanspec);

	data[0] = devpriv->trimpot_value[channel];

	return 1;
}

static int cb_pcidas_ai_cmdtest(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_cmd *cmd)
{
	const struct cb_pcidas_board *thisboard = comedi_board(dev);
	struct cb_pcidas_private *devpriv = dev->private;
	int err = 0;
	int tmp;
	int i, gain, start_chan;

	/*                                                */

	err |= cfc_check_trigger_src(&cmd->start_src, TRIG_NOW | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->scan_begin_src,
					TRIG_FOLLOW | TRIG_TIMER | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->convert_src,
					TRIG_TIMER | TRIG_NOW | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= cfc_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);

	if (err)
		return 1;

	/*                                                */

	err |= cfc_check_trigger_is_unique(cmd->start_src);
	err |= cfc_check_trigger_is_unique(cmd->scan_begin_src);
	err |= cfc_check_trigger_is_unique(cmd->convert_src);
	err |= cfc_check_trigger_is_unique(cmd->stop_src);

	/*                                   */

	if (cmd->scan_begin_src == TRIG_FOLLOW && cmd->convert_src == TRIG_NOW)
		err |= -EINVAL;
	if (cmd->scan_begin_src != TRIG_FOLLOW && cmd->convert_src != TRIG_NOW)
		err |= -EINVAL;
	if (cmd->start_src == TRIG_EXT &&
	    (cmd->convert_src == TRIG_EXT || cmd->scan_begin_src == TRIG_EXT))
		err |= -EINVAL;

	if (err)
		return 2;

	/*                                            */

	switch (cmd->start_src) {
	case TRIG_EXT:
		/*                                                            */
		if ((cmd->start_arg
		     & (CR_FLAGS_MASK & ~(CR_EDGE | CR_INVERT))) != 0) {
			cmd->start_arg &= ~(CR_FLAGS_MASK &
						~(CR_EDGE | CR_INVERT));
			err |= -EINVAL;
		}
		if (!thisboard->is_1602 && (cmd->start_arg & CR_INVERT)) {
			cmd->start_arg &= (CR_FLAGS_MASK & ~CR_INVERT);
			err |= -EINVAL;
		}
		break;
	default:
		err |= cfc_check_trigger_arg_is(&cmd->start_arg, 0);
		break;
	}

	if (cmd->scan_begin_src == TRIG_TIMER)
		err |= cfc_check_trigger_arg_min(&cmd->scan_begin_arg,
				thisboard->ai_speed * cmd->chanlist_len);

	if (cmd->convert_src == TRIG_TIMER)
		err |= cfc_check_trigger_arg_min(&cmd->convert_arg,
						 thisboard->ai_speed);

	err |= cfc_check_trigger_arg_is(&cmd->scan_end_arg, cmd->chanlist_len);

	if (cmd->stop_src == TRIG_NONE)
		err |= cfc_check_trigger_arg_is(&cmd->stop_arg, 0);

	if (err)
		return 3;

	/*                              */

	if (cmd->scan_begin_src == TRIG_TIMER) {
		tmp = cmd->scan_begin_arg;
		i8253_cascade_ns_to_timer_2div(TIMER_BASE,
					       &(devpriv->divisor1),
					       &(devpriv->divisor2),
					       &(cmd->scan_begin_arg),
					       cmd->flags & TRIG_ROUND_MASK);
		if (tmp != cmd->scan_begin_arg)
			err++;
	}
	if (cmd->convert_src == TRIG_TIMER) {
		tmp = cmd->convert_arg;
		i8253_cascade_ns_to_timer_2div(TIMER_BASE,
					       &(devpriv->divisor1),
					       &(devpriv->divisor2),
					       &(cmd->convert_arg),
					       cmd->flags & TRIG_ROUND_MASK);
		if (tmp != cmd->convert_arg)
			err++;
	}

	if (err)
		return 4;

	/*                                                     */
	if (cmd->chanlist) {
		gain = CR_RANGE(cmd->chanlist[0]);
		start_chan = CR_CHAN(cmd->chanlist[0]);
		for (i = 1; i < cmd->chanlist_len; i++) {
			if (CR_CHAN(cmd->chanlist[i]) !=
			    (start_chan + i) % s->n_chan) {
				comedi_error(dev,
					     "entries in chanlist must be consecutive channels, counting upwards\n");
				err++;
			}
			if (CR_RANGE(cmd->chanlist[i]) != gain) {
				comedi_error(dev,
					     "entries in chanlist must all have the same gain\n");
				err++;
			}
		}
	}

	if (err)
		return 5;

	return 0;
}

static void cb_pcidas_load_counters(struct comedi_device *dev, unsigned int *ns,
				    int rounding_flags)
{
	struct cb_pcidas_private *devpriv = dev->private;

	i8253_cascade_ns_to_timer_2div(TIMER_BASE, &(devpriv->divisor1),
				       &(devpriv->divisor2), ns,
				       rounding_flags & TRIG_ROUND_MASK);

	/*                                                         */
	i8254_load(devpriv->pacer_counter_dio + ADC8254, 0, 1,
		   devpriv->divisor1, 2);
	i8254_load(devpriv->pacer_counter_dio + ADC8254, 0, 2,
		   devpriv->divisor2, 2);
}

static int cb_pcidas_ai_cmd(struct comedi_device *dev,
			    struct comedi_subdevice *s)
{
	const struct cb_pcidas_board *thisboard = comedi_board(dev);
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
	unsigned int bits;
	unsigned long flags;

	/*                                   */
	outw(0, devpriv->control_status + CALIBRATION_REG);
	/*                                                           */
	outw(0, devpriv->control_status + TRIG_CONTSTAT);
	/*             */
	outw(0, devpriv->adc_fifo + ADCFIFOCLR);

	/*                                        */
	bits = BEGIN_SCAN(CR_CHAN(cmd->chanlist[0])) |
	    END_SCAN(CR_CHAN(cmd->chanlist[cmd->chanlist_len - 1])) |
	    GAIN_BITS(CR_RANGE(cmd->chanlist[0]));
	/*                       */
	if (CR_RANGE(cmd->chanlist[0]) & IS_UNIPOLAR)
		bits |= UNIP;
	/*                               */
	if (CR_AREF(cmd->chanlist[0]) != AREF_DIFF)
		bits |= SE;
	/*                   */
	if (cmd->convert_src == TRIG_EXT || cmd->scan_begin_src == TRIG_EXT)
		bits |= PACER_EXT_RISE;
	else
		bits |= PACER_INT;
	outw(bits, devpriv->control_status + ADCMUX_CONT);

	/*                */
	if (cmd->convert_src == TRIG_TIMER)
		cb_pcidas_load_counters(dev, &cmd->convert_arg,
					cmd->flags & TRIG_ROUND_MASK);
	else if (cmd->scan_begin_src == TRIG_TIMER)
		cb_pcidas_load_counters(dev, &cmd->scan_begin_arg,
					cmd->flags & TRIG_ROUND_MASK);

	/*                            */
	if (cmd->stop_src == TRIG_COUNT)
		devpriv->count = cmd->chanlist_len * cmd->stop_arg;
	/*                    */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->adc_fifo_bits |= INTE;
	devpriv->adc_fifo_bits &= ~INT_MASK;
	if (cmd->flags & TRIG_WAKE_EOS) {
		if (cmd->convert_src == TRIG_NOW && cmd->chanlist_len > 1) {
			/*                        */
			devpriv->adc_fifo_bits |= INT_EOS;
		} else {
			/*                          */
			devpriv->adc_fifo_bits |= INT_FNE;
		}
	} else {
		/*                          */
		devpriv->adc_fifo_bits |= INT_FHF;
	}

	/*                                */
	outw(devpriv->adc_fifo_bits | EOAI | INT | LADFUL,
	     devpriv->control_status + INT_ADCFIFO);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*                                   */
	bits = 0;
	if (cmd->start_src == TRIG_NOW)
		bits |= SW_TRIGGER;
	else if (cmd->start_src == TRIG_EXT) {
		bits |= EXT_TRIGGER | TGEN | XTRCL;
		if (thisboard->is_1602) {
			if (cmd->start_arg & CR_INVERT)
				bits |= TGPOL;
			if (cmd->start_arg & CR_EDGE)
				bits |= TGSEL;
		}
	} else {
		comedi_error(dev, "bug!");
		return -1;
	}
	if (cmd->convert_src == TRIG_NOW && cmd->chanlist_len > 1)
		bits |= BURSTE;
	outw(bits, devpriv->control_status + TRIG_CONTSTAT);

	return 0;
}

static int cb_pcidas_ao_cmdtest(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_cmd *cmd)
{
	const struct cb_pcidas_board *thisboard = comedi_board(dev);
	struct cb_pcidas_private *devpriv = dev->private;
	int err = 0;
	int tmp;

	/*                                                */

	err |= cfc_check_trigger_src(&cmd->start_src, TRIG_INT);
	err |= cfc_check_trigger_src(&cmd->scan_begin_src,
					TRIG_TIMER | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->convert_src, TRIG_NOW);
	err |= cfc_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= cfc_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);

	if (err)
		return 1;

	/*                                                */

	err |= cfc_check_trigger_is_unique(cmd->scan_begin_src);
	err |= cfc_check_trigger_is_unique(cmd->stop_src);

	/*                                   */

	if (err)
		return 2;

	/*                                                */

	err |= cfc_check_trigger_arg_is(&cmd->start_arg, 0);

	if (cmd->scan_begin_src == TRIG_TIMER)
		err |= cfc_check_trigger_arg_min(&cmd->scan_begin_arg,
						 thisboard->ao_scan_speed);

	err |= cfc_check_trigger_arg_is(&cmd->scan_end_arg, cmd->chanlist_len);

	if (cmd->stop_src == TRIG_NONE)
		err |= cfc_check_trigger_arg_is(&cmd->stop_arg, 0);

	if (err)
		return 3;

	/*                              */

	if (cmd->scan_begin_src == TRIG_TIMER) {
		tmp = cmd->scan_begin_arg;
		i8253_cascade_ns_to_timer_2div(TIMER_BASE,
					       &(devpriv->ao_divisor1),
					       &(devpriv->ao_divisor2),
					       &(cmd->scan_begin_arg),
					       cmd->flags & TRIG_ROUND_MASK);
		if (tmp != cmd->scan_begin_arg)
			err++;
	}

	if (err)
		return 4;

	/*                                                     */
	if (cmd->chanlist && cmd->chanlist_len > 1) {
		if (CR_CHAN(cmd->chanlist[0]) != 0 ||
		    CR_CHAN(cmd->chanlist[1]) != 1) {
			comedi_error(dev,
				     "channels must be ordered channel 0, channel 1 in chanlist\n");
			err++;
		}
	}

	if (err)
		return 5;

	return 0;
}

/*                             */
static int cb_pcidas_cancel(struct comedi_device *dev,
			    struct comedi_subdevice *s)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long flags;

	spin_lock_irqsave(&dev->spinlock, flags);
	/*                     */
	devpriv->adc_fifo_bits &= ~INTE & ~EOAIE;
	outw(devpriv->adc_fifo_bits, devpriv->control_status + INT_ADCFIFO);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*                                              */
	outw(0, devpriv->control_status + TRIG_CONTSTAT);
	/*                        */
	outw(0, devpriv->control_status + ADCMUX_CONT);

	return 0;
}

static int cb_pcidas_ao_inttrig(struct comedi_device *dev,
				struct comedi_subdevice *s,
				unsigned int trig_num)
{
	const struct cb_pcidas_board *thisboard = comedi_board(dev);
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int num_bytes, num_points = thisboard->fifo_size;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &s->async->cmd;
	unsigned long flags;

	if (trig_num != 0)
		return -EINVAL;

	/*               */
	if (cmd->stop_src == TRIG_COUNT && devpriv->ao_count < num_points)
		num_points = devpriv->ao_count;

	num_bytes = cfc_read_array_from_buffer(s, devpriv->ao_buffer,
					       num_points * sizeof(short));
	num_points = num_bytes / sizeof(short);

	if (cmd->stop_src == TRIG_COUNT)
		devpriv->ao_count -= num_points;
	/*                             */
	outsw(devpriv->ao_registers + DACDATA, devpriv->ao_buffer, num_bytes);

	/*                                            */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->adc_fifo_bits |= DAEMIE | DAHFIE;

	/*                              */
	outw(devpriv->adc_fifo_bits | DAEMI | DAHFI,
	     devpriv->control_status + INT_ADCFIFO);

	/*            */
	devpriv->ao_control_bits |= DAC_START | DACEN | DAC_EMPTY;
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);

	spin_unlock_irqrestore(&dev->spinlock, flags);

	async->inttrig = NULL;

	return 0;
}

static int cb_pcidas_ao_cmd(struct comedi_device *dev,
			    struct comedi_subdevice *s)
{
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
	unsigned int i;
	unsigned long flags;

	/*                           */
	spin_lock_irqsave(&dev->spinlock, flags);
	for (i = 0; i < cmd->chanlist_len; i++) {
		/*                 */
		devpriv->ao_control_bits |=
		    DAC_CHAN_EN(CR_CHAN(cmd->chanlist[i]));
		/*            */
		devpriv->ao_control_bits |= DAC_RANGE(CR_CHAN(cmd->chanlist[i]),
						      CR_RANGE(cmd->
							       chanlist[i]));
	}

	/*                                                                   */
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*             */
	outw(0, devpriv->ao_registers + DACFIFOCLR);

	/*                */
	if (cmd->scan_begin_src == TRIG_TIMER) {
		i8253_cascade_ns_to_timer_2div(TIMER_BASE,
					       &(devpriv->ao_divisor1),
					       &(devpriv->ao_divisor2),
					       &(cmd->scan_begin_arg),
					       cmd->flags);

		/*                                                         */
		i8254_load(devpriv->pacer_counter_dio + DAC8254, 0, 1,
			   devpriv->ao_divisor1, 2);
		i8254_load(devpriv->pacer_counter_dio + DAC8254, 0, 2,
			   devpriv->ao_divisor2, 2);
	}
	/*                            */
	if (cmd->stop_src == TRIG_COUNT)
		devpriv->ao_count = cmd->chanlist_len * cmd->stop_arg;
	/*                   */
	spin_lock_irqsave(&dev->spinlock, flags);
	switch (cmd->scan_begin_src) {
	case TRIG_TIMER:
		devpriv->ao_control_bits |= DAC_PACER_INT;
		break;
	case TRIG_EXT:
		devpriv->ao_control_bits |= DAC_PACER_EXT_RISE;
		break;
	default:
		spin_unlock_irqrestore(&dev->spinlock, flags);
		comedi_error(dev, "error setting dac pacer source");
		return -1;
		break;
	}
	spin_unlock_irqrestore(&dev->spinlock, flags);

	async->inttrig = cb_pcidas_ao_inttrig;

	return 0;
}

/*                              */
static int cb_pcidas_ao_cancel(struct comedi_device *dev,
			       struct comedi_subdevice *s)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long flags;

	spin_lock_irqsave(&dev->spinlock, flags);
	/*                     */
	devpriv->adc_fifo_bits &= ~DAHFIE & ~DAEMIE;
	outw(devpriv->adc_fifo_bits, devpriv->control_status + INT_ADCFIFO);

	/*                 */
	devpriv->ao_control_bits &= ~DACEN & ~DAC_PACER_MASK;
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	return 0;
}

static void handle_ao_interrupt(struct comedi_device *dev, unsigned int status)
{
	const struct cb_pcidas_board *thisboard = comedi_board(dev);
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_subdevice *s = dev->write_subdev;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
	unsigned int half_fifo = thisboard->fifo_size / 2;
	unsigned int num_points;
	unsigned long flags;

	async->events = 0;

	if (status & DAEMI) {
		/*                                  */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | DAEMI,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
		if (inw(devpriv->ao_registers + DAC_CSR) & DAC_EMPTY) {
			if (cmd->stop_src == TRIG_NONE ||
			    (cmd->stop_src == TRIG_COUNT
			     && devpriv->ao_count)) {
				comedi_error(dev, "dac fifo underflow");
				cb_pcidas_ao_cancel(dev, s);
				async->events |= COMEDI_CB_ERROR;
			}
			async->events |= COMEDI_CB_EOA;
		}
	} else if (status & DAHFI) {
		unsigned int num_bytes;

		/*                                                    */
		num_points = half_fifo;
		if (cmd->stop_src == TRIG_COUNT &&
		    devpriv->ao_count < num_points)
			num_points = devpriv->ao_count;
		num_bytes =
		    cfc_read_array_from_buffer(s, devpriv->ao_buffer,
					       num_points * sizeof(short));
		num_points = num_bytes / sizeof(short);

		if (async->cmd.stop_src == TRIG_COUNT)
			devpriv->ao_count -= num_points;
		/*                             */
		outsw(devpriv->ao_registers + DACDATA, devpriv->ao_buffer,
		      num_points);
		/*                                  */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | DAHFI,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
	}

	comedi_event(dev, s);
}

static irqreturn_t cb_pcidas_interrupt(int irq, void *d)
{
	struct comedi_device *dev = (struct comedi_device *)d;
	const struct cb_pcidas_board *thisboard = comedi_board(dev);
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_subdevice *s = dev->read_subdev;
	struct comedi_async *async;
	int status, s5933_status;
	int half_fifo = thisboard->fifo_size / 2;
	unsigned int num_samples, i;
	static const int timeout = 10000;
	unsigned long flags;

	if (!dev->attached)
		return IRQ_NONE;

	async = s->async;
	async->events = 0;

	s5933_status = inl(devpriv->s5933_config + AMCC_OP_REG_INTCSR);

	if ((INTCSR_INTR_ASSERTED & s5933_status) == 0)
		return IRQ_NONE;

	/*                               */
	inl_p(devpriv->s5933_config + AMCC_OP_REG_IMB4);
	/*                                */
	outl(devpriv->s5933_intcsr_bits | INTCSR_INBOX_INTR_STATUS,
	     devpriv->s5933_config + AMCC_OP_REG_INTCSR);

	status = inw(devpriv->control_status + INT_ADCFIFO);

	/*                                    */
	if (status & (DAHFI | DAEMI))
		handle_ao_interrupt(dev, status);
	/*                                    */
	/*                    */
	if (status & ADHFI) {
		/*            */
		num_samples = half_fifo;
		if (async->cmd.stop_src == TRIG_COUNT &&
		    num_samples > devpriv->count) {
			num_samples = devpriv->count;
		}
		insw(devpriv->adc_fifo + ADCDATA, devpriv->ai_buffer,
		     num_samples);
		cfc_write_array_to_buffer(s, devpriv->ai_buffer,
					  num_samples * sizeof(short));
		devpriv->count -= num_samples;
		if (async->cmd.stop_src == TRIG_COUNT && devpriv->count == 0) {
			async->events |= COMEDI_CB_EOA;
			cb_pcidas_cancel(dev, s);
		}
		/*                                  */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | INT,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
		/*                         */
	} else if (status & (ADNEI | EOBI)) {
		for (i = 0; i < timeout; i++) {
			/*                         */
			if ((ADNE & inw(devpriv->control_status +
					INT_ADCFIFO)) == 0)
				break;
			cfc_write_to_buffer(s, inw(devpriv->adc_fifo));
			if (async->cmd.stop_src == TRIG_COUNT &&
			    --devpriv->count == 0) {
				/*                    */
				cb_pcidas_cancel(dev, s);
				async->events |= COMEDI_CB_EOA;
				break;
			}
		}
		/*                                  */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | INT,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
	} else if (status & EOAI) {
		comedi_error(dev,
			     "bug! encountered end of acquisition interrupt?");
		/*                            */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | EOAI,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
	}
	/*                         */
	if (status & LADFUL) {
		comedi_error(dev, "fifo overflow");
		/*                                 */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | LADFUL,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
		cb_pcidas_cancel(dev, s);
		async->events |= COMEDI_CB_EOA | COMEDI_CB_ERROR;
	}

	comedi_event(dev, s);

	return IRQ_HANDLED;
}

static int cb_pcidas_auto_attach(struct comedi_device *dev,
				 unsigned long context)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	const struct cb_pcidas_board *thisboard = NULL;
	struct cb_pcidas_private *devpriv;
	struct comedi_subdevice *s;
	int i;
	int ret;

	if (context < ARRAY_SIZE(cb_pcidas_boards))
		thisboard = &cb_pcidas_boards[context];
	if (!thisboard)
		return -ENODEV;
	dev->board_ptr  = thisboard;
	dev->board_name = thisboard->name;

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;

	devpriv->s5933_config = pci_resource_start(pcidev, 0);
	devpriv->control_status = pci_resource_start(pcidev, 1);
	devpriv->adc_fifo = pci_resource_start(pcidev, 2);
	devpriv->pacer_counter_dio = pci_resource_start(pcidev, 3);
	if (thisboard->ao_nchan)
		devpriv->ao_registers = pci_resource_start(pcidev, 4);

	/*                                             */
	outl(INTCSR_INBOX_INTR_STATUS,
	     devpriv->s5933_config + AMCC_OP_REG_INTCSR);

	if (request_irq(pcidev->irq, cb_pcidas_interrupt,
			IRQF_SHARED, dev->driver->driver_name, dev)) {
		dev_dbg(dev->class_dev, "unable to allocate irq %d\n",
			pcidev->irq);
		return -EINVAL;
	}
	dev->irq = pcidev->irq;

	ret = comedi_alloc_subdevices(dev, 7);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	/*                        */
	dev->read_subdev = s;
	s->type = COMEDI_SUBD_AI;
	s->subdev_flags = SDF_READABLE | SDF_GROUND | SDF_DIFF | SDF_CMD_READ;
	/*                                                           */
	s->n_chan = thisboard->ai_nchan;
	s->len_chanlist = thisboard->ai_nchan;
	s->maxdata = (1 << thisboard->ai_bits) - 1;
	s->range_table = thisboard->ranges;
	s->insn_read = cb_pcidas_ai_rinsn;
	s->insn_config = ai_config_insn;
	s->do_cmd = cb_pcidas_ai_cmd;
	s->do_cmdtest = cb_pcidas_ai_cmdtest;
	s->cancel = cb_pcidas_cancel;

	/*                         */
	s = &dev->subdevices[1];
	if (thisboard->ao_nchan) {
		s->type = COMEDI_SUBD_AO;
		s->subdev_flags = SDF_READABLE | SDF_WRITABLE | SDF_GROUND;
		s->n_chan = thisboard->ao_nchan;
		/*
                                         
                                            
   */
		s->maxdata = (1 << thisboard->ai_bits) - 1;
		s->range_table = &cb_pcidas_ao_ranges;
		s->insn_read = cb_pcidas_ao_readback_insn;
		if (thisboard->has_ao_fifo) {
			dev->write_subdev = s;
			s->subdev_flags |= SDF_CMD_WRITE;
			s->insn_write = cb_pcidas_ao_fifo_winsn;
			s->do_cmdtest = cb_pcidas_ao_cmdtest;
			s->do_cmd = cb_pcidas_ao_cmd;
			s->cancel = cb_pcidas_ao_cancel;
		} else {
			s->insn_write = cb_pcidas_ao_nofifo_winsn;
		}
	} else {
		s->type = COMEDI_SUBD_UNUSED;
	}

	/*      */
	s = &dev->subdevices[2];
	ret = subdev_8255_init(dev, s, NULL,
			       devpriv->pacer_counter_dio + DIO_8255);
	if (ret)
		return ret;

	/*                 */
	s = &dev->subdevices[3];
	s->type = COMEDI_SUBD_MEMORY;
	s->subdev_flags = SDF_READABLE | SDF_INTERNAL;
	s->n_chan = 256;
	s->maxdata = 0xff;
	s->insn_read = eeprom_read_insn;

	/*              */
	s = &dev->subdevices[4];
	s->type = COMEDI_SUBD_CALIB;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE | SDF_INTERNAL;
	s->n_chan = NUM_CHANNELS_8800;
	s->maxdata = 0xff;
	s->insn_read = caldac_read_insn;
	s->insn_write = caldac_write_insn;
	for (i = 0; i < s->n_chan; i++)
		caldac_8800_write(dev, i, s->maxdata / 2);

	/*                     */
	s = &dev->subdevices[5];
	s->type = COMEDI_SUBD_CALIB;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE | SDF_INTERNAL;
	if (thisboard->trimpot == AD7376) {
		s->n_chan = NUM_CHANNELS_7376;
		s->maxdata = 0x7f;
	} else {
		s->n_chan = NUM_CHANNELS_8402;
		s->maxdata = 0xff;
	}
	s->insn_read = trimpot_read_insn;
	s->insn_write = trimpot_write_insn;
	for (i = 0; i < s->n_chan; i++)
		cb_pcidas_trimpot_write(dev, i, s->maxdata / 2);

	/*               */
	s = &dev->subdevices[6];
	if (thisboard->has_dac08) {
		s->type = COMEDI_SUBD_CALIB;
		s->subdev_flags = SDF_READABLE | SDF_WRITABLE | SDF_INTERNAL;
		s->n_chan = NUM_CHANNELS_DAC08;
		s->insn_read = dac08_read_insn;
		s->insn_write = dac08_write_insn;
		s->maxdata = 0xff;
		dac08_write(dev, s->maxdata / 2);
	} else
		s->type = COMEDI_SUBD_UNUSED;

	/*                               */
	inl(devpriv->s5933_config + AMCC_OP_REG_IMB4);
	/*                                                               */
	devpriv->s5933_intcsr_bits =
	    INTCSR_INBOX_BYTE(3) | INTCSR_INBOX_SELECT(3) |
	    INTCSR_INBOX_FULL_INT;
	/*                                           */
	outl(devpriv->s5933_intcsr_bits | INTCSR_INBOX_INTR_STATUS,
	     devpriv->s5933_config + AMCC_OP_REG_INTCSR);

	dev_info(dev->class_dev, "%s: %s attached\n",
		dev->driver->driver_name, dev->board_name);

	return 0;
}

static void cb_pcidas_detach(struct comedi_device *dev)
{
	struct cb_pcidas_private *devpriv = dev->private;

	if (devpriv) {
		if (devpriv->s5933_config) {
			outl(INTCSR_INBOX_INTR_STATUS,
			     devpriv->s5933_config + AMCC_OP_REG_INTCSR);
		}
	}
	if (dev->irq)
		free_irq(dev->irq, dev);
	comedi_spriv_free(dev, 2);
	comedi_pci_disable(dev);
}

static struct comedi_driver cb_pcidas_driver = {
	.driver_name	= "cb_pcidas",
	.module		= THIS_MODULE,
	.auto_attach	= cb_pcidas_auto_attach,
	.detach		= cb_pcidas_detach,
};

static int cb_pcidas_pci_probe(struct pci_dev *dev,
			       const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &cb_pcidas_driver,
				      id->driver_data);
}

static DEFINE_PCI_DEVICE_TABLE(cb_pcidas_pci_table) = {
	{ PCI_VDEVICE(CB, 0x0001), BOARD_PCIDAS1602_16 },
	{ PCI_VDEVICE(CB, 0x000f), BOARD_PCIDAS1200 },
	{ PCI_VDEVICE(CB, 0x0010), BOARD_PCIDAS1602_12 },
	{ PCI_VDEVICE(CB, 0x0019), BOARD_PCIDAS1200_JR },
	{ PCI_VDEVICE(CB, 0x001c), BOARD_PCIDAS1602_16_JR },
	{ PCI_VDEVICE(CB, 0x004c), BOARD_PCIDAS1000 },
	{ PCI_VDEVICE(CB, 0x001a), BOARD_PCIDAS1001 },
	{ PCI_VDEVICE(CB, 0x001b), BOARD_PCIDAS1002 },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, cb_pcidas_pci_table);

static struct pci_driver cb_pcidas_pci_driver = {
	.name		= "cb_pcidas",
	.id_table	= cb_pcidas_pci_table,
	.probe		= cb_pcidas_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(cb_pcidas_driver, cb_pcidas_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
