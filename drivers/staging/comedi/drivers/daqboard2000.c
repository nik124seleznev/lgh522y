/*
   comedi/drivers/daqboard2000.c
   hardware driver for IOtech DAQboard/2000

   COMEDI - Linux Control and Measurement Device Interface
   Copyright (C) 1999 Anders Blomdell <anders.blomdell@control.lth.se>

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
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/firmware.h>

#include "../comedidev.h"

#include "8255.h"

#define DAQBOARD2000_FIRMWARE		"daqboard2000_firmware.bin"

#define DAQBOARD2000_SUBSYSTEM_IDS2 	0x0002	/*                        */
#define DAQBOARD2000_SUBSYSTEM_IDS4 	0x0004	/*                        */

/*                                                            */
#define DAQBOARD2000_SECRProgPinHi      0x8001767e
#define DAQBOARD2000_SECRProgPinLo      0x8000767e
#define DAQBOARD2000_SECRLocalBusHi     0xc000767e
#define DAQBOARD2000_SECRLocalBusLo     0x8000767e
#define DAQBOARD2000_SECRReloadHi       0xa000767e
#define DAQBOARD2000_SECRReloadLo       0x8000767e

/*                  */
#define DAQBOARD2000_EEPROM_PRESENT     0x10000000

/*                  */
#define DAQBOARD2000_CPLD_INIT 		0x0002
#define DAQBOARD2000_CPLD_DONE 		0x0004

static const struct comedi_lrange range_daqboard2000_ai = {
	13, {
		BIP_RANGE(10),
		BIP_RANGE(5),
		BIP_RANGE(2.5),
		BIP_RANGE(1.25),
		BIP_RANGE(0.625),
		BIP_RANGE(0.3125),
		BIP_RANGE(0.156),
		UNI_RANGE(10),
		UNI_RANGE(5),
		UNI_RANGE(2.5),
		UNI_RANGE(1.25),
		UNI_RANGE(0.625),
		UNI_RANGE(0.3125)
	}
};

/*
                      
 */
#define acqControl			0x00		/*     */
#define acqScanListFIFO			0x02		/*     */
#define acqPacerClockDivLow		0x04		/*     */
#define acqScanCounter			0x08		/*     */
#define acqPacerClockDivHigh		0x0a		/*     */
#define acqTriggerCount			0x0c		/*     */
#define acqResultsFIFO			0x10		/*     */
#define acqResultsShadow		0x14		/*     */
#define acqAdcResult			0x18		/*     */
#define dacScanCounter			0x1c		/*     */
#define dacControl			0x20		/*     */
#define dacFIFO				0x24		/*     */
#define dacPacerClockDiv		0x2a		/*     */
#define refDacs				0x2c		/*     */
#define dioControl			0x30		/*     */
#define dioP3hsioData			0x32		/*     */
#define dioP3Control			0x34		/*     */
#define calEepromControl		0x36		/*     */
#define dacSetting(x)			(0x38 + (x)*2)	/*     */
#define dioP2ExpansionIO8Bit		0x40		/*     */
#define ctrTmrControl			0x80		/*     */
#define ctrInput(x)			(0x88 + (x)*2)	/*     */
#define timerDivisor(x)			(0xa0 + (x)*2)	/*     */
#define dmaControl			0xb0		/*     */
#define trigControl			0xb2		/*     */
#define calEeprom			0xb8		/*     */
#define acqDigitalMark			0xba		/*     */
#define trigDacs			0xbc		/*     */
#define dioP2ExpansionIO16Bit(x)	(0xc0 + (x)*2)	/*     */

/*                            */
#define DAQBOARD2000_SeqStartScanList            0x0011
#define DAQBOARD2000_SeqStopScanList             0x0010

/*                         */
#define DAQBOARD2000_AcqResetScanListFifo        0x0004
#define DAQBOARD2000_AcqResetResultsFifo         0x0002
#define DAQBOARD2000_AcqResetConfigPipe          0x0001

/*                        */
#define DAQBOARD2000_AcqResultsFIFOMore1Sample   0x0001
#define DAQBOARD2000_AcqResultsFIFOHasValidData  0x0002
#define DAQBOARD2000_AcqResultsFIFOOverrun       0x0004
#define DAQBOARD2000_AcqLogicScanning            0x0008
#define DAQBOARD2000_AcqConfigPipeFull           0x0010
#define DAQBOARD2000_AcqScanListFIFOEmpty        0x0020
#define DAQBOARD2000_AcqAdcNotReady              0x0040
#define DAQBOARD2000_ArbitrationFailure          0x0080
#define DAQBOARD2000_AcqPacerOverrun             0x0100
#define DAQBOARD2000_DacPacerOverrun             0x0200
#define DAQBOARD2000_AcqHardwareError            0x01c0

/*                            */
#define DAQBOARD2000_SeqStartScanList            0x0011
#define DAQBOARD2000_SeqStopScanList             0x0010

/*                     */
#define DAQBOARD2000_AdcPacerInternal            0x0030
#define DAQBOARD2000_AdcPacerExternal            0x0032
#define DAQBOARD2000_AdcPacerEnable              0x0031
#define DAQBOARD2000_AdcPacerEnableDacPacer      0x0034
#define DAQBOARD2000_AdcPacerDisable             0x0030
#define DAQBOARD2000_AdcPacerNormalMode          0x0060
#define DAQBOARD2000_AdcPacerCompatibilityMode   0x0061
#define DAQBOARD2000_AdcPacerInternalOutEnable   0x0008
#define DAQBOARD2000_AdcPacerExternalRising      0x0100

/*            */
#define DAQBOARD2000_DacFull                     0x0001
#define DAQBOARD2000_RefBusy                     0x0002
#define DAQBOARD2000_TrgBusy                     0x0004
#define DAQBOARD2000_CalBusy                     0x0008
#define DAQBOARD2000_Dac0Busy                    0x0010
#define DAQBOARD2000_Dac1Busy                    0x0020
#define DAQBOARD2000_Dac2Busy                    0x0040
#define DAQBOARD2000_Dac3Busy                    0x0080

/*             */
#define DAQBOARD2000_Dac0Enable                  0x0021
#define DAQBOARD2000_Dac1Enable                  0x0031
#define DAQBOARD2000_Dac2Enable                  0x0041
#define DAQBOARD2000_Dac3Enable                  0x0051
#define DAQBOARD2000_DacEnableBit                0x0001
#define DAQBOARD2000_Dac0Disable                 0x0020
#define DAQBOARD2000_Dac1Disable                 0x0030
#define DAQBOARD2000_Dac2Disable                 0x0040
#define DAQBOARD2000_Dac3Disable                 0x0050
#define DAQBOARD2000_DacResetFifo                0x0004
#define DAQBOARD2000_DacPatternDisable           0x0060
#define DAQBOARD2000_DacPatternEnable            0x0061
#define DAQBOARD2000_DacSelectSignedData         0x0002
#define DAQBOARD2000_DacSelectUnsignedData       0x0000

/*                 */
#define DAQBOARD2000_TrigAnalog                  0x0000
#define DAQBOARD2000_TrigTTL                     0x0010
#define DAQBOARD2000_TrigTransHiLo               0x0004
#define DAQBOARD2000_TrigTransLoHi               0x0000
#define DAQBOARD2000_TrigAbove                   0x0000
#define DAQBOARD2000_TrigBelow                   0x0004
#define DAQBOARD2000_TrigLevelSense              0x0002
#define DAQBOARD2000_TrigEdgeSense               0x0000
#define DAQBOARD2000_TrigEnable                  0x0001
#define DAQBOARD2000_TrigDisable                 0x0000

/*                         */
#define DAQBOARD2000_PosRefDacSelect             0x0100
#define DAQBOARD2000_NegRefDacSelect             0x0000

struct daq200_boardtype {
	const char *name;
	int id;
};
static const struct daq200_boardtype boardtypes[] = {
	{"ids2", DAQBOARD2000_SUBSYSTEM_IDS2},
	{"ids4", DAQBOARD2000_SUBSYSTEM_IDS4},
};

struct daqboard2000_private {
	enum {
		card_daqboard_2000
	} card;
	void __iomem *daq;
	void __iomem *plx;
	unsigned int ao_readback[2];
};

static void writeAcqScanListEntry(struct comedi_device *dev, u16 entry)
{
	struct daqboard2000_private *devpriv = dev->private;

	/*            */
	writew(entry & 0x00ff, devpriv->daq + acqScanListFIFO);
	/*            */
	writew((entry >> 8) & 0x00ff, devpriv->daq + acqScanListFIFO);
}

static void setup_sampling(struct comedi_device *dev, int chan, int gain)
{
	u16 word0, word1, word2, word3;

	/*                                             */
	word0 = 0;
	word1 = 0x0004;		/*           */
	word2 = (chan << 6) & 0x00c0;
	switch (chan / 4) {
	case 0:
		word3 = 0x0001;
		break;
	case 1:
		word3 = 0x0002;
		break;
	case 2:
		word3 = 0x0005;
		break;
	case 3:
		word3 = 0x0006;
		break;
	case 4:
		word3 = 0x0041;
		break;
	case 5:
		word3 = 0x0042;
		break;
	default:
		word3 = 0;
		break;
	}
/*
                                                      
                                                    
*/
	/*                                  */
	word2 |= 0x0800;
	word3 |= 0xc000;
	writeAcqScanListEntry(dev, word0);
	writeAcqScanListEntry(dev, word1);
	writeAcqScanListEntry(dev, word2);
	writeAcqScanListEntry(dev, word3);
}

static int daqboard2000_ai_insn_read(struct comedi_device *dev,
				     struct comedi_subdevice *s,
				     struct comedi_insn *insn,
				     unsigned int *data)
{
	struct daqboard2000_private *devpriv = dev->private;
	unsigned int val;
	int gain, chan, timeout;
	int i;

	writew(DAQBOARD2000_AcqResetScanListFifo |
	       DAQBOARD2000_AcqResetResultsFifo |
	       DAQBOARD2000_AcqResetConfigPipe, devpriv->daq + acqControl);

	/*
                                                              
                                                         
  */
	/*                                 */
	writel(1000000, devpriv->daq + acqPacerClockDivLow);
	writew(0, devpriv->daq + acqPacerClockDivHigh);

	gain = CR_RANGE(insn->chanspec);
	chan = CR_CHAN(insn->chanspec);

	/*                                                                 
                                                                 
                                                                     
                            */
	for (i = 0; i < insn->n; i++) {
		setup_sampling(dev, chan, gain);
		/*                                       */
		writew(DAQBOARD2000_SeqStartScanList,
		       devpriv->daq + acqControl);
		for (timeout = 0; timeout < 20; timeout++) {
			val = readw(devpriv->daq + acqControl);
			if (val & DAQBOARD2000_AcqConfigPipeFull)
				break;
			/*            */
		}
		writew(DAQBOARD2000_AdcPacerEnable, devpriv->daq + acqControl);
		for (timeout = 0; timeout < 20; timeout++) {
			val = readw(devpriv->daq + acqControl);
			if (val & DAQBOARD2000_AcqLogicScanning)
				break;
			/*            */
		}
		for (timeout = 0; timeout < 20; timeout++) {
			val = readw(devpriv->daq + acqControl);
			if (val & DAQBOARD2000_AcqResultsFIFOHasValidData)
				break;
			/*            */
		}
		data[i] = readw(devpriv->daq + acqResultsFIFO);
		writew(DAQBOARD2000_AdcPacerDisable, devpriv->daq + acqControl);
		writew(DAQBOARD2000_SeqStopScanList, devpriv->daq + acqControl);
	}

	return i;
}

static int daqboard2000_ao_insn_read(struct comedi_device *dev,
				     struct comedi_subdevice *s,
				     struct comedi_insn *insn,
				     unsigned int *data)
{
	struct daqboard2000_private *devpriv = dev->private;
	int chan = CR_CHAN(insn->chanspec);
	int i;

	for (i = 0; i < insn->n; i++)
		data[i] = devpriv->ao_readback[chan];

	return i;
}

static int daqboard2000_ao_insn_write(struct comedi_device *dev,
				      struct comedi_subdevice *s,
				      struct comedi_insn *insn,
				      unsigned int *data)
{
	struct daqboard2000_private *devpriv = dev->private;
	int chan = CR_CHAN(insn->chanspec);
	unsigned int val;
	int timeout;
	int i;

	for (i = 0; i < insn->n; i++) {
#if 0
		/*
                                                      
                                           
   */
		writew((chan + 2) * 0x0010 | 0x0001,
		       devpriv->daq + dacControl);
		udelay(1000);
#endif
		writew(data[i], devpriv->daq + dacSetting(chan));
		for (timeout = 0; timeout < 20; timeout++) {
			val = readw(devpriv->daq + dacControl);
			if ((val & ((chan + 1) * 0x0010)) == 0)
				break;
			/*            */
		}
		devpriv->ao_readback[chan] = data[i];
#if 0
		/*
                                                    
                     
   */
		writew((chan + 2) * 0x0010 | 0x0000,
		       devpriv->daq + dacControl);
		udelay(1000);
#endif
	}

	return i;
}

static void daqboard2000_resetLocalBus(struct comedi_device *dev)
{
	struct daqboard2000_private *devpriv = dev->private;

	writel(DAQBOARD2000_SECRLocalBusHi, devpriv->plx + 0x6c);
	mdelay(10);
	writel(DAQBOARD2000_SECRLocalBusLo, devpriv->plx + 0x6c);
	mdelay(10);
}

static void daqboard2000_reloadPLX(struct comedi_device *dev)
{
	struct daqboard2000_private *devpriv = dev->private;

	writel(DAQBOARD2000_SECRReloadLo, devpriv->plx + 0x6c);
	mdelay(10);
	writel(DAQBOARD2000_SECRReloadHi, devpriv->plx + 0x6c);
	mdelay(10);
	writel(DAQBOARD2000_SECRReloadLo, devpriv->plx + 0x6c);
	mdelay(10);
}

static void daqboard2000_pulseProgPin(struct comedi_device *dev)
{
	struct daqboard2000_private *devpriv = dev->private;

	writel(DAQBOARD2000_SECRProgPinHi, devpriv->plx + 0x6c);
	mdelay(10);
	writel(DAQBOARD2000_SECRProgPinLo, devpriv->plx + 0x6c);
	mdelay(10);	/*                                                  */
}

static int daqboard2000_pollCPLD(struct comedi_device *dev, int mask)
{
	struct daqboard2000_private *devpriv = dev->private;
	int result = 0;
	int i;
	int cpld;

	/*                               */
	for (i = 0; i < 50; i++) {
		cpld = readw(devpriv->daq + 0x1000);
		if ((cpld & mask) == mask) {
			result = 1;
			break;
		}
		udelay(100);
	}
	udelay(5);
	return result;
}

static int daqboard2000_writeCPLD(struct comedi_device *dev, int data)
{
	struct daqboard2000_private *devpriv = dev->private;
	int result = 0;

	udelay(10);
	writew(data, devpriv->daq + 0x1000);
	if ((readw(devpriv->daq + 0x1000) & DAQBOARD2000_CPLD_INIT) ==
	    DAQBOARD2000_CPLD_INIT) {
		result = 1;
	}
	return result;
}

static int initialize_daqboard2000(struct comedi_device *dev,
				   const u8 *cpld_array, size_t len)
{
	struct daqboard2000_private *devpriv = dev->private;
	int result = -EIO;
	/*                                         */
	int secr;
	int retry;
	size_t i;

	/*                                                              */
	secr = readl(devpriv->plx + 0x6c);
	if (!(secr & DAQBOARD2000_EEPROM_PRESENT))
		return -EIO;

	for (retry = 0; retry < 3; retry++) {
		daqboard2000_resetLocalBus(dev);
		daqboard2000_reloadPLX(dev);
		daqboard2000_pulseProgPin(dev);
		if (daqboard2000_pollCPLD(dev, DAQBOARD2000_CPLD_INIT)) {
			for (i = 0; i < len; i++) {
				if (cpld_array[i] == 0xff &&
				    cpld_array[i + 1] == 0x20)
					break;
			}
			for (; i < len; i += 2) {
				int data =
				    (cpld_array[i] << 8) + cpld_array[i + 1];
				if (!daqboard2000_writeCPLD(dev, data))
					break;
			}
			if (i >= len) {
				daqboard2000_resetLocalBus(dev);
				daqboard2000_reloadPLX(dev);
				result = 0;
				break;
			}
		}
	}
	return result;
}

static int daqboard2000_upload_firmware(struct comedi_device *dev)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	const struct firmware *fw;
	int ret;

	ret = request_firmware(&fw, DAQBOARD2000_FIRMWARE, &pcidev->dev);
	if (ret)
		return ret;

	ret = initialize_daqboard2000(dev, fw->data, fw->size);
	release_firmware(fw);

	return ret;
}

static void daqboard2000_adcStopDmaTransfer(struct comedi_device *dev)
{
}

static void daqboard2000_adcDisarm(struct comedi_device *dev)
{
	struct daqboard2000_private *devpriv = dev->private;

	/*                           */
	udelay(2);
	writew(DAQBOARD2000_TrigAnalog | DAQBOARD2000_TrigDisable,
	       devpriv->daq + trigControl);
	udelay(2);
	writew(DAQBOARD2000_TrigTTL | DAQBOARD2000_TrigDisable,
	       devpriv->daq + trigControl);

	/*                                                             */
	udelay(2);
	writew(DAQBOARD2000_SeqStopScanList, devpriv->daq + acqControl);

	/*                      */
	udelay(2);
	writew(DAQBOARD2000_AdcPacerDisable, devpriv->daq + acqControl);

	/*                                      */
	daqboard2000_adcStopDmaTransfer(dev);
}

static void daqboard2000_activateReferenceDacs(struct comedi_device *dev)
{
	struct daqboard2000_private *devpriv = dev->private;
	unsigned int val;
	int timeout;

	/*                                            */
	writew(0x80 | DAQBOARD2000_PosRefDacSelect, devpriv->daq + refDacs);
	for (timeout = 0; timeout < 20; timeout++) {
		val = readw(devpriv->daq + dacControl);
		if ((val & DAQBOARD2000_RefBusy) == 0)
			break;
		udelay(2);
	}

	/*                                            */
	writew(0x80 | DAQBOARD2000_NegRefDacSelect, devpriv->daq + refDacs);
	for (timeout = 0; timeout < 20; timeout++) {
		val = readw(devpriv->daq + dacControl);
		if ((val & DAQBOARD2000_RefBusy) == 0)
			break;
		udelay(2);
	}
}

static void daqboard2000_initializeCtrs(struct comedi_device *dev)
{
}

static void daqboard2000_initializeTmrs(struct comedi_device *dev)
{
}

static void daqboard2000_dacDisarm(struct comedi_device *dev)
{
}

static void daqboard2000_initializeAdc(struct comedi_device *dev)
{
	daqboard2000_adcDisarm(dev);
	daqboard2000_activateReferenceDacs(dev);
	daqboard2000_initializeCtrs(dev);
	daqboard2000_initializeTmrs(dev);
}

static void daqboard2000_initializeDac(struct comedi_device *dev)
{
	daqboard2000_dacDisarm(dev);
}

static int daqboard2000_8255_cb(int dir, int port, int data,
				unsigned long ioaddr)
{
	void __iomem *mmio_base = (void __iomem *)ioaddr;

	if (dir) {
		writew(data, mmio_base + port * 2);
		return 0;
	} else {
		return readw(mmio_base + port * 2);
	}
}

static const void *daqboard2000_find_boardinfo(struct comedi_device *dev,
					       struct pci_dev *pcidev)
{
	const struct daq200_boardtype *board;
	int i;

	if (pcidev->subsystem_device != PCI_VENDOR_ID_IOTECH)
		return NULL;

	for (i = 0; i < ARRAY_SIZE(boardtypes); i++) {
		board = &boardtypes[i];
		if (pcidev->subsystem_device == board->id)
			return board;
	}
	return NULL;
}

static int daqboard2000_auto_attach(struct comedi_device *dev,
					      unsigned long context_unused)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	const struct daq200_boardtype *board;
	struct daqboard2000_private *devpriv;
	struct comedi_subdevice *s;
	int result;

	board = daqboard2000_find_boardinfo(dev, pcidev);
	if (!board)
		return -ENODEV;
	dev->board_ptr = board;
	dev->board_name = board->name;

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	result = comedi_pci_enable(dev);
	if (result)
		return result;

	devpriv->plx = pci_ioremap_bar(pcidev, 0);
	devpriv->daq = pci_ioremap_bar(pcidev, 2);
	if (!devpriv->plx || !devpriv->daq)
		return -ENOMEM;

	result = comedi_alloc_subdevices(dev, 3);
	if (result)
		return result;

	readl(devpriv->plx + 0x6c);

	result = daqboard2000_upload_firmware(dev);
	if (result < 0)
		return result;

	daqboard2000_initializeAdc(dev);
	daqboard2000_initializeDac(dev);

	s = &dev->subdevices[0];
	/*              */
	s->type = COMEDI_SUBD_AI;
	s->subdev_flags = SDF_READABLE | SDF_GROUND;
	s->n_chan = 24;
	s->maxdata = 0xffff;
	s->insn_read = daqboard2000_ai_insn_read;
	s->range_table = &range_daqboard2000_ai;

	s = &dev->subdevices[1];
	/*              */
	s->type = COMEDI_SUBD_AO;
	s->subdev_flags = SDF_WRITABLE;
	s->n_chan = 2;
	s->maxdata = 0xffff;
	s->insn_read = daqboard2000_ao_insn_read;
	s->insn_write = daqboard2000_ao_insn_write;
	s->range_table = &range_bipolar10;

	s = &dev->subdevices[2];
	result = subdev_8255_init(dev, s, daqboard2000_8255_cb,
			(unsigned long)(devpriv->daq + dioP2ExpansionIO8Bit));
	if (result)
		return result;

	dev_info(dev->class_dev, "%s: %s attached\n",
		dev->driver->driver_name, dev->board_name);

	return 0;
}

static void daqboard2000_detach(struct comedi_device *dev)
{
	struct daqboard2000_private *devpriv = dev->private;

	comedi_spriv_free(dev, 2);
	if (dev->irq)
		free_irq(dev->irq, dev);
	if (devpriv) {
		if (devpriv->daq)
			iounmap(devpriv->daq);
		if (devpriv->plx)
			iounmap(devpriv->plx);
	}
	comedi_pci_disable(dev);
}

static struct comedi_driver daqboard2000_driver = {
	.driver_name	= "daqboard2000",
	.module		= THIS_MODULE,
	.auto_attach	= daqboard2000_auto_attach,
	.detach		= daqboard2000_detach,
};

static int daqboard2000_pci_probe(struct pci_dev *dev,
				  const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &daqboard2000_driver,
				      id->driver_data);
}

static DEFINE_PCI_DEVICE_TABLE(daqboard2000_pci_table) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_IOTECH, 0x0409) },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, daqboard2000_pci_table);

static struct pci_driver daqboard2000_pci_driver = {
	.name		= "daqboard2000",
	.id_table	= daqboard2000_pci_table,
	.probe		= daqboard2000_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(daqboard2000_driver, daqboard2000_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE(DAQBOARD2000_FIRMWARE);
