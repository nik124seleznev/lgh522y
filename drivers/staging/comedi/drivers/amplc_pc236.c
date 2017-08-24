/*
    comedi/drivers/amplc_pc236.c
    Driver for Amplicon PC36AT and PCI236 DIO boards.

    Copyright (C) 2002 MEV Ltd. <http://www.mev.co.uk/>

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
#include <linux/interrupt.h>

#include "../comedidev.h"

#include "comedi_fc.h"
#include "8255.h"
#include "plx9052.h"

#define PC236_DRIVER_NAME	"amplc_pc236"

#define DO_ISA	IS_ENABLED(CONFIG_COMEDI_AMPLC_PC236_ISA)
#define DO_PCI	IS_ENABLED(CONFIG_COMEDI_AMPLC_PC236_PCI)

/*                                               */
#define PCI_DEVICE_ID_AMPLICON_PCI236 0x0009
#define PCI_DEVICE_ID_INVALID 0xffff

/*                           */

#define PC236_IO_SIZE		4
#define PC236_LCR_IO_SIZE	128

/*                                */
#define PCI236_INTR_DISABLE	(PLX9052_INTCSR_LI1POL |	\
				 PLX9052_INTCSR_LI2POL |	\
				 PLX9052_INTCSR_LI1SEL |	\
				 PLX9052_INTCSR_LI1CLRINT)

/*                               */
#define PCI236_INTR_ENABLE	(PLX9052_INTCSR_LI1ENAB |	\
				 PLX9052_INTCSR_LI1POL |	\
				 PLX9052_INTCSR_LI2POL |	\
				 PLX9052_INTCSR_PCIENAB |	\
				 PLX9052_INTCSR_LI1SEL |	\
				 PLX9052_INTCSR_LI1CLRINT)

/*
                                                     
 */

enum pc236_bustype { isa_bustype, pci_bustype };
enum pc236_model { pc36at_model, pci236_model, anypci_model };

struct pc236_board {
	const char *name;
	unsigned short devid;
	enum pc236_bustype bustype;
	enum pc236_model model;
};
static const struct pc236_board pc236_boards[] = {
#if DO_ISA
	{
		.name = "pc36at",
		.bustype = isa_bustype,
		.model = pc36at_model,
	},
#endif
#if DO_PCI
	{
		.name = "pci236",
		.devid = PCI_DEVICE_ID_AMPLICON_PCI236,
		.bustype = pci_bustype,
		.model = pci236_model,
	},
	{
		.name = PC236_DRIVER_NAME,
		.devid = PCI_DEVICE_ID_INVALID,
		.bustype = pci_bustype,
		.model = anypci_model,	/*          */
	},
#endif
};

/*                                                               
                                                                       
                                                                               
 */
struct pc236_private {
	unsigned long lcr_iobase; /*                                         */
	int enable_irq;
};

/*                                                */
static inline bool is_isa_board(const struct pc236_board *board)
{
	return DO_ISA && board->bustype == isa_bustype;
}

/*                                               */
static inline bool is_pci_board(const struct pc236_board *board)
{
	return DO_PCI && board->bustype == pci_bustype;
}

/*
                                                                    
 */
static const struct pc236_board *pc236_find_pci_board(struct pci_dev *pci_dev)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(pc236_boards); i++)
		if (is_pci_board(&pc236_boards[i]) &&
		    pci_dev->device == pc236_boards[i].devid)
			return &pc236_boards[i];
	return NULL;
}

/*
                                                                          
                
 */
static struct pci_dev *pc236_find_pci_dev(struct comedi_device *dev,
					  struct comedi_devconfig *it)
{
	const struct pc236_board *thisboard = comedi_board(dev);
	struct pci_dev *pci_dev = NULL;
	int bus = it->options[0];
	int slot = it->options[1];

	for_each_pci_dev(pci_dev) {
		if (bus || slot) {
			if (bus != pci_dev->bus->number ||
			    slot != PCI_SLOT(pci_dev->devfn))
				continue;
		}
		if (pci_dev->vendor != PCI_VENDOR_ID_AMPLICON)
			continue;

		if (thisboard->model == anypci_model) {
			/*                                                 */
			const struct pc236_board *foundboard;

			foundboard = pc236_find_pci_board(pci_dev);
			if (foundboard == NULL)
				continue;
			/*                             */
			dev->board_ptr = foundboard;
		} else {
			/*                            */
			if (pci_dev->device != thisboard->devid)
				continue;
		}
		return pci_dev;
	}
	dev_err(dev->class_dev,
		"No supported board found! (req. bus %d, slot %d)\n",
		bus, slot);
	return NULL;
}

/*
                                                                        
                                                                     
                                                                    
 */
static void pc236_intr_disable(struct comedi_device *dev)
{
	const struct pc236_board *thisboard = comedi_board(dev);
	struct pc236_private *devpriv = dev->private;
	unsigned long flags;

	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->enable_irq = 0;
	if (is_pci_board(thisboard))
		outl(PCI236_INTR_DISABLE, devpriv->lcr_iobase + PLX9052_INTCSR);
	spin_unlock_irqrestore(&dev->spinlock, flags);
}

/*
                                                                      
                                                                    
                                                                          
 */
static void pc236_intr_enable(struct comedi_device *dev)
{
	const struct pc236_board *thisboard = comedi_board(dev);
	struct pc236_private *devpriv = dev->private;
	unsigned long flags;

	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->enable_irq = 1;
	if (is_pci_board(thisboard))
		outl(PCI236_INTR_ENABLE, devpriv->lcr_iobase + PLX9052_INTCSR);
	spin_unlock_irqrestore(&dev->spinlock, flags);
}

/*
                                                                    
                                                                    
                                                                 
             
                                                
 */
static int pc236_intr_check(struct comedi_device *dev)
{
	const struct pc236_board *thisboard = comedi_board(dev);
	struct pc236_private *devpriv = dev->private;
	int retval = 0;
	unsigned long flags;
	unsigned int intcsr;

	spin_lock_irqsave(&dev->spinlock, flags);
	if (devpriv->enable_irq) {
		retval = 1;
		if (is_pci_board(thisboard)) {
			intcsr = inl(devpriv->lcr_iobase + PLX9052_INTCSR);
			if (!(intcsr & PLX9052_INTCSR_LI1STAT)) {
				retval = 0;
			} else {
				/*                                      */
				outl(PCI236_INTR_ENABLE,
				     devpriv->lcr_iobase + PLX9052_INTCSR);
			}
		}
	}
	spin_unlock_irqrestore(&dev->spinlock, flags);

	return retval;
}

/*
                          
                                         
 */
static int pc236_intr_insn(struct comedi_device *dev,
			   struct comedi_subdevice *s, struct comedi_insn *insn,
			   unsigned int *data)
{
	data[1] = 0;
	return insn->n;
}

/*
                            
                                         
 */
static int pc236_intr_cmdtest(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_cmd *cmd)
{
	int err = 0;

	/*                                                */

	err |= cfc_check_trigger_src(&cmd->start_src, TRIG_NOW);
	err |= cfc_check_trigger_src(&cmd->scan_begin_src, TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->convert_src, TRIG_FOLLOW);
	err |= cfc_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= cfc_check_trigger_src(&cmd->stop_src, TRIG_NONE);

	if (err)
		return 1;

	/*                                                */
	/*                                   */

	if (err)
		return 2;

	/*                                                */

	err |= cfc_check_trigger_arg_is(&cmd->start_arg, 0);
	err |= cfc_check_trigger_arg_is(&cmd->scan_begin_arg, 0);
	err |= cfc_check_trigger_arg_is(&cmd->convert_arg, 0);
	err |= cfc_check_trigger_arg_is(&cmd->scan_end_arg, 1);
	err |= cfc_check_trigger_arg_is(&cmd->stop_arg, 0);

	if (err)
		return 3;

	/*                 */

	if (err)
		return 4;

	return 0;
}

/*
                       
 */
static int pc236_intr_cmd(struct comedi_device *dev, struct comedi_subdevice *s)
{
	pc236_intr_enable(dev);

	return 0;
}

/*
                              
 */
static int pc236_intr_cancel(struct comedi_device *dev,
			     struct comedi_subdevice *s)
{
	pc236_intr_disable(dev);

	return 0;
}

/*
                             
                                      
 */
static irqreturn_t pc236_interrupt(int irq, void *d)
{
	struct comedi_device *dev = d;
	struct comedi_subdevice *s = &dev->subdevices[1];
	int handled;

	handled = pc236_intr_check(dev);
	if (dev->attached && handled) {
		comedi_buf_put(s->async, 0);
		s->async->events |= COMEDI_CB_BLOCK | COMEDI_CB_EOS;
		comedi_event(dev, s);
	}
	return IRQ_RETVAL(handled);
}

static void pc236_report_attach(struct comedi_device *dev, unsigned int irq)
{
	const struct pc236_board *thisboard = comedi_board(dev);
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	char tmpbuf[60];
	int tmplen;

	if (is_isa_board(thisboard))
		tmplen = scnprintf(tmpbuf, sizeof(tmpbuf),
				   "(base %#lx) ", dev->iobase);
	else if (is_pci_board(thisboard))
		tmplen = scnprintf(tmpbuf, sizeof(tmpbuf),
				   "(pci %s) ", pci_name(pcidev));
	else
		tmplen = 0;
	if (irq)
		tmplen += scnprintf(&tmpbuf[tmplen], sizeof(tmpbuf) - tmplen,
				    "(irq %u%s) ", irq,
				    (dev->irq ? "" : " UNAVAILABLE"));
	else
		tmplen += scnprintf(&tmpbuf[tmplen], sizeof(tmpbuf) - tmplen,
				    "(no irq) ");
	dev_info(dev->class_dev, "%s %sattached\n",
		 dev->board_name, tmpbuf);
}

static int pc236_common_attach(struct comedi_device *dev, unsigned long iobase,
			       unsigned int irq, unsigned long req_irq_flags)
{
	const struct pc236_board *thisboard = comedi_board(dev);
	struct comedi_subdevice *s;
	int ret;

	dev->board_name = thisboard->name;
	dev->iobase = iobase;

	ret = comedi_alloc_subdevices(dev, 2);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	/*                              */
	ret = subdev_8255_init(dev, s, NULL, iobase);
	if (ret < 0) {
		dev_err(dev->class_dev, "error! out of memory!\n");
		return ret;
	}
	s = &dev->subdevices[1];
	dev->read_subdev = s;
	s->type = COMEDI_SUBD_UNUSED;
	pc236_intr_disable(dev);
	if (irq) {
		if (request_irq(irq, pc236_interrupt, req_irq_flags,
				PC236_DRIVER_NAME, dev) >= 0) {
			dev->irq = irq;
			s->type = COMEDI_SUBD_DI;
			s->subdev_flags = SDF_READABLE | SDF_CMD_READ;
			s->n_chan = 1;
			s->maxdata = 1;
			s->range_table = &range_digital;
			s->insn_bits = pc236_intr_insn;
			s->do_cmdtest = pc236_intr_cmdtest;
			s->do_cmd = pc236_intr_cmd;
			s->cancel = pc236_intr_cancel;
		}
	}
	pc236_report_attach(dev, irq);
	return 1;
}

static int pc236_pci_common_attach(struct comedi_device *dev,
				   struct pci_dev *pci_dev)
{
	struct pc236_private *devpriv = dev->private;
	unsigned long iobase;
	int ret;

	comedi_set_hw_dev(dev, &pci_dev->dev);

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;

	devpriv->lcr_iobase = pci_resource_start(pci_dev, 1);
	iobase = pci_resource_start(pci_dev, 2);
	return pc236_common_attach(dev, iobase, pci_dev->irq, IRQF_SHARED);
}

/*
                                                              
                                                               
                                                        
           
 */
static int pc236_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	const struct pc236_board *thisboard = comedi_board(dev);
	struct pc236_private *devpriv;
	int ret;

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	/*                                        */
	if (is_isa_board(thisboard)) {
		ret = comedi_request_region(dev, it->options[0], PC236_IO_SIZE);
		if (ret)
			return ret;

		return pc236_common_attach(dev, dev->iobase, it->options[1], 0);
	} else if (is_pci_board(thisboard)) {
		struct pci_dev *pci_dev;

		pci_dev = pc236_find_pci_dev(dev, it);
		if (!pci_dev)
			return -EIO;
		return pc236_pci_common_attach(dev, pci_dev);
	} else {
		dev_err(dev->class_dev, PC236_DRIVER_NAME
			": BUG! cannot determine board type!\n");
		return -EINVAL;
	}
}

/*
                                                       
                                                              
                                                                  
 */
static int pc236_auto_attach(struct comedi_device *dev,
				       unsigned long context_unused)
{
	struct pci_dev *pci_dev = comedi_to_pci_dev(dev);
	struct pc236_private *devpriv;

	if (!DO_PCI)
		return -EINVAL;

	dev_info(dev->class_dev, PC236_DRIVER_NAME ": attach pci %s\n",
		 pci_name(pci_dev));

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	dev->board_ptr = pc236_find_pci_board(pci_dev);
	if (dev->board_ptr == NULL) {
		dev_err(dev->class_dev, "BUG! cannot determine board type!\n");
		return -EINVAL;
	}
	/*
                                                                      
                                                                  
                                                                   
                     
  */
	pci_dev_get(pci_dev);
	return pc236_pci_common_attach(dev, pci_dev);
}

static void pc236_detach(struct comedi_device *dev)
{
	const struct pc236_board *thisboard = comedi_board(dev);

	if (!thisboard)
		return;
	if (dev->iobase)
		pc236_intr_disable(dev);
	comedi_spriv_free(dev, 0);
	if (is_isa_board(thisboard)) {
		comedi_legacy_detach(dev);
	} else if (is_pci_board(thisboard)) {
		struct pci_dev *pcidev = comedi_to_pci_dev(dev);
		if (dev->irq)
			free_irq(dev->irq, dev);
		comedi_pci_disable(dev);
		if (pcidev)
			pci_dev_put(pcidev);
	}
}

/*
                                                                  
                                                                   
                                                            
                   
 */
static struct comedi_driver amplc_pc236_driver = {
	.driver_name = PC236_DRIVER_NAME,
	.module = THIS_MODULE,
	.attach = pc236_attach,
	.auto_attach = pc236_auto_attach,
	.detach = pc236_detach,
	.board_name = &pc236_boards[0].name,
	.offset = sizeof(struct pc236_board),
	.num_names = ARRAY_SIZE(pc236_boards),
};

#if DO_PCI
static DEFINE_PCI_DEVICE_TABLE(pc236_pci_table) = {
	{ PCI_DEVICE(PCI_VENDOR_ID_AMPLICON, PCI_DEVICE_ID_AMPLICON_PCI236) },
	{0}
};

MODULE_DEVICE_TABLE(pci, pc236_pci_table);

static int amplc_pc236_pci_probe(struct pci_dev *dev,
				 const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &amplc_pc236_driver,
				      id->driver_data);
}

static struct pci_driver amplc_pc236_pci_driver = {
	.name = PC236_DRIVER_NAME,
	.id_table = pc236_pci_table,
	.probe = &amplc_pc236_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};

module_comedi_pci_driver(amplc_pc236_driver, amplc_pc236_pci_driver);
#else
module_comedi_driver(amplc_pc236_driver);
#endif

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
