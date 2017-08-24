/*
    comedi/drivers/skel.c
    Skeleton code for a Comedi driver

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

/*
                                                               
                                                      
  
                                  
                                                                          
                                                                      
                                                                       
                                                                        
                                                                            
                                                                         
                    
               
                                                                           
                              
                                                                    
                                                                  
                           
                                                  
                                                                    
                                             
                                                                     
                                                                     
                                  
                                                                     
                                  
  
                                                                     
                                                                   
                                                                      
                                                                        
                   
  
                                                                     
                                            
 */

#include <linux/pci.h>

#include "../comedidev.h"

#include "comedi_fc.h"

/*                                             */

#define SKEL_SIZE 0

#define SKEL_START_AI_CONV	0
#define SKEL_AI_READ		0

/*
                                                               
                                                                   
                                                      
 */
enum skel_boardid {
	BOARD_SKEL100,
	BOARD_SKEL200,
};

struct skel_board {
	const char *name;
	int ai_chans;
	int ai_bits;
	int have_dio;
};

static const struct skel_board skel_boards[] = {
	[BOARD_SKEL100] = {
		.name		= "skel-100",
		.ai_chans	= 16,
		.ai_bits	= 12,
		.have_dio	= 1,
	},
	[BOARD_SKEL200] = {
		.name		= "skel-200",
		.ai_chans	= 8,
		.ai_bits	= 16,
	},
};

/*                                                               
                                                                       
                                                                               
 */
struct skel_private {

	int data;

	/*                      */
	unsigned int ao_readback[2];
};

/*                                                              
                                                             
                                                                     
                                                                   
                                             */
static int skel_ns_to_timer(unsigned int *ns, int round)
{
	/*               */
	/*                                                        
                                                         
                                                                  
                                                                      
  */

	return *ns;
}

/*
                                                                       
        
 */
static int skel_ai_rinsn(struct comedi_device *dev, struct comedi_subdevice *s,
			 struct comedi_insn *insn, unsigned int *data)
{
	const struct skel_board *thisboard = comedi_board(dev);
	int n, i;
	unsigned int d;
	unsigned int status;

	/*                                */

	/*                              */
	/*                                    */

	/*                              */

	/*                   */
	for (n = 0; n < insn->n; n++) {
		/*                    */
		/*                                     */

#define TIMEOUT 100
		/*                            */
		for (i = 0; i < TIMEOUT; i++) {
			status = 1;
			/*                                          */
			if (status)
				break;
		}
		if (i == TIMEOUT) {
			dev_warn(dev->class_dev, "ai timeout\n");
			return -ETIMEDOUT;
		}

		/*           */
		/*                                      */
		d = 0;

		/*                              */
		d ^= 1 << (thisboard->ai_bits - 1);

		data[n] = d;
	}

	/*                                           */
	return n;
}

/*
                                                            
                                                         
                                              
  
                                                         
                      
 */
static int skel_ai_cmdtest(struct comedi_device *dev,
			   struct comedi_subdevice *s,
			   struct comedi_cmd *cmd)
{
	int err = 0;
	int tmp;

	/*                                                */

	err |= cfc_check_trigger_src(&cmd->start_src, TRIG_NOW);
	err |= cfc_check_trigger_src(&cmd->scan_begin_src,
					TRIG_TIMER | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->convert_src, TRIG_TIMER | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= cfc_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);

	if (err)
		return 1;

	/*                                                */

	err |= cfc_check_trigger_is_unique(cmd->scan_begin_src);
	err |= cfc_check_trigger_is_unique(cmd->convert_src);
	err |= cfc_check_trigger_is_unique(cmd->stop_src);

	/*                                   */

	if (err)
		return 2;

	/*                                                */

	err |= cfc_check_trigger_arg_is(&cmd->start_arg, 0);

#define MAX_SPEED	10000	/*                */
#define MIN_SPEED	1000000000	/*                */

	if (cmd->scan_begin_src == TRIG_TIMER) {
		err |= cfc_check_trigger_arg_min(&cmd->scan_begin_arg,
						 MAX_SPEED);
		err |= cfc_check_trigger_arg_max(&cmd->scan_begin_arg,
						 MIN_SPEED);
	} else {
		/*                  */
		/*                                                */
		/*                                           */
		err |= cfc_check_trigger_arg_max(&cmd->scan_begin_arg, 9);
	}

	if (cmd->convert_src == TRIG_TIMER) {
		err |= cfc_check_trigger_arg_min(&cmd->convert_arg, MAX_SPEED);
		err |= cfc_check_trigger_arg_max(&cmd->convert_arg, MIN_SPEED);
	} else {
		/*                  */
		/*           */
		err |= cfc_check_trigger_arg_max(&cmd->scan_begin_arg, 9);
	}

	err |= cfc_check_trigger_arg_is(&cmd->scan_end_arg, cmd->chanlist_len);

	if (cmd->stop_src == TRIG_COUNT)
		err |= cfc_check_trigger_arg_max(&cmd->stop_arg, 0x00ffffff);
	else	/*           */
		err |= cfc_check_trigger_arg_is(&cmd->stop_arg, 0);

	if (err)
		return 3;

	/*                              */

	if (cmd->scan_begin_src == TRIG_TIMER) {
		tmp = cmd->scan_begin_arg;
		skel_ns_to_timer(&cmd->scan_begin_arg,
				 cmd->flags & TRIG_ROUND_MASK);
		if (tmp != cmd->scan_begin_arg)
			err++;
	}
	if (cmd->convert_src == TRIG_TIMER) {
		tmp = cmd->convert_arg;
		skel_ns_to_timer(&cmd->convert_arg,
				 cmd->flags & TRIG_ROUND_MASK);
		if (tmp != cmd->convert_arg)
			err++;
		if (cmd->scan_begin_src == TRIG_TIMER &&
		    cmd->scan_begin_arg <
		    cmd->convert_arg * cmd->scan_end_arg) {
			cmd->scan_begin_arg =
			    cmd->convert_arg * cmd->scan_end_arg;
			err++;
		}
	}

	if (err)
		return 4;

	return 0;
}

static int skel_ao_winsn(struct comedi_device *dev, struct comedi_subdevice *s,
			 struct comedi_insn *insn, unsigned int *data)
{
	struct skel_private *devpriv = dev->private;
	int i;
	int chan = CR_CHAN(insn->chanspec);

	/*                                                          
                                                          */
	for (i = 0; i < insn->n; i++) {
		/*                                */
		/*                                              */
		devpriv->ao_readback[chan] = data[i];
	}

	/*                                           */
	return i;
}

/*                                                               
                                                         */
static int skel_ao_rinsn(struct comedi_device *dev, struct comedi_subdevice *s,
			 struct comedi_insn *insn, unsigned int *data)
{
	struct skel_private *devpriv = dev->private;
	int i;
	int chan = CR_CHAN(insn->chanspec);

	for (i = 0; i < insn->n; i++)
		data[i] = devpriv->ao_readback[chan];

	return i;
}

/*                                                              
                                                                
                                                                   
                                                               
                                                                 */
static int skel_dio_insn_bits(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_insn *insn, unsigned int *data)
{
	/*                                                    
                                                     */
	if (data[0]) {
		s->state &= ~data[0];
		s->state |= data[0] & data[1];
		/*                                        */
		/*                                        */
	}

	/*                                                     
                            */
	/*                                      */
	/*                                                                  
                                             */
	/*                   */

	return insn->n;
}

static int skel_dio_insn_config(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn, unsigned int *data)
{
	int chan = CR_CHAN(insn->chanspec);

	/*                                                          
                                                              
                                                                
                                         */
	switch (data[0]) {
	case INSN_CONFIG_DIO_OUTPUT:
		s->io_bits |= 1 << chan;
		break;
	case INSN_CONFIG_DIO_INPUT:
		s->io_bits &= ~(1 << chan);
		break;
	case INSN_CONFIG_DIO_QUERY:
		data[1] =
		    (s->io_bits & (1 << chan)) ? COMEDI_OUTPUT : COMEDI_INPUT;
		return insn->n;
		break;
	default:
		return -EINVAL;
		break;
	}
	/*                                                 */

	return insn->n;
}

/*
                                                              
 */
static int skel_common_attach(struct comedi_device *dev)
{
	const struct skel_board *thisboard = comedi_board(dev);
	struct comedi_subdevice *s;
	int ret;

	ret = comedi_alloc_subdevices(dev, 3);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	/*                     */
	/*                        */
	s->type = COMEDI_SUBD_AI;
	/*                                                   */
	s->subdev_flags = SDF_READABLE | SDF_GROUND | SDF_DIFF;
	s->n_chan = thisboard->ai_chans;
	s->maxdata = (1 << thisboard->ai_bits) - 1;
	s->range_table = &range_bipolar10;
	s->len_chanlist = 16;	/*                                         
                            */
	s->insn_read = skel_ai_rinsn;
/*
                                        
                                
*/
	s->do_cmdtest = skel_ai_cmdtest;

	s = &dev->subdevices[1];
	/*                         */
	s->type = COMEDI_SUBD_AO;
	s->subdev_flags = SDF_WRITABLE;
	s->n_chan = 1;
	s->maxdata = 0xffff;
	s->range_table = &range_bipolar5;
	s->insn_write = skel_ao_winsn;
	s->insn_read = skel_ao_rinsn;

	s = &dev->subdevices[2];
	/*                       */
	if (thisboard->have_dio) {
		s->type = COMEDI_SUBD_DIO;
		s->subdev_flags = SDF_READABLE | SDF_WRITABLE;
		s->n_chan = 16;
		s->maxdata = 1;
		s->range_table = &range_digital;
		s->insn_bits = skel_dio_insn_bits;
		s->insn_config = skel_dio_insn_config;
	} else {
		s->type = COMEDI_SUBD_UNUSED;
	}

	dev_info(dev->class_dev, "skel: attached\n");

	return 0;
}

/*
                                                               
                                                                       
                                                                        
                                                                 
  
                                                                     
                                                                        
                                                                   
                                                                         
                       
 */
static int skel_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	const struct skel_board *thisboard;
	struct skel_private *devpriv;

/*
                                                                   
                                                                
                                 
 */
	/*                                       */

	thisboard = comedi_board(dev);

	/*
                                                                
                                                               
                                                   
  */
	/*                                    */

	/*                           */
	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

/*
                                                            
                                                                     
                                                               
                                                                
                             
 */
/*
                                           
                           
                                                           
                      
    
 */

/*
                                                               
                                                                   
                                                              
 */

	/*
                                                                   
                                                                  
                                                       
  */
	return skel_common_attach(dev);
}

/*
                                                          
                                                                         
                                                                       
  
                                             
 */
static int skel_auto_attach(struct comedi_device *dev,
			    unsigned long context)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	const struct skel_board *thisboard = NULL;
	struct skel_private *devpriv;
	int ret;

	/*                                                */
	if (!IS_ENABLED(CONFIG_COMEDI_PCI_DRIVERS))
		return -EINVAL;

	/*
                                                          
   
                                                                
                                                               
                                                             
                                                                  
  */
	if (context < ARRAY_SIZE(skel_boards))
		thisboard = &skel_boards[context];
	if (!thisboard)
		return -ENODEV;

	/*
                                                             
                           
  */
	dev->board_ptr = thisboard;
	dev->board_name = thisboard->name;

	/*                           */
	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	/*                        */
	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;

	/*
                                                                 
                                 
   
                                                                
                                                                  
                                                                 
            
   
                                                                
                                                               
                                                      
  */
	dev->iobase = pci_resource_start(pcidev, 0);

	/*
                                                                   
                                                                  
                                                       
  */
	return skel_common_attach(dev);
}

/*
                                                                   
             
                                                                     
                                                             
                                                                
                                         
 */
static void skel_detach(struct comedi_device *dev)
{
	const struct skel_board *thisboard = comedi_board(dev);
	struct skel_private *devpriv = dev->private;

	if (!thisboard || !devpriv)
		return;

/*
                                                                 
                                                                      
                                                                        
            
 */

	if (IS_ENABLED(CONFIG_COMEDI_PCI_DRIVERS) /*   
                                       */) {
		/*
              
    
                                                            
                     
   */
		comedi_pci_disable(dev);
	} else {
		/*
              
    
                                                      
                                                        
                                                        
                                            
   */
		comedi_legacy_detach(dev);
	}
}

/*
                                                                  
                                                                   
                                                            
                   
 */
static struct comedi_driver skel_driver = {
	.driver_name = "dummy",
	.module = THIS_MODULE,
	.attach = skel_attach,
	.auto_attach = skel_auto_attach,
	.detach = skel_detach,
/*                                                                  
                                              */
	/*                                                      
                                                            
                                                         
                                                          
                                                             
                                              
                                                          
                                                         
                                                          
                                                       
                                                       
   
                                                          
                                                            
                            
  */
	.board_name = &skel_boards[0].name,
	.offset = sizeof(struct skel_board),
	.num_names = ARRAY_SIZE(skel_boards),
};

#ifdef CONFIG_COMEDI_PCI_DRIVERS

static int skel_pci_probe(struct pci_dev *dev,
			  const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &skel_driver, id->driver_data);
}

/*
                                                            
                         
 */
#define PCI_VENDOR_ID_SKEL	0xdafe

/*
                                                            
                                                  
 */
static DEFINE_PCI_DEVICE_TABLE(skel_pci_table) = {
	{ PCI_VDEVICE(SKEL, 0x0100), BOARD_SKEL100 },
	{ PCI_VDEVICE(SKEL, 0x0200), BOARD_SKEL200 },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, skel_pci_table);

static struct pci_driver skel_pci_driver = {
	.name		= "dummy",
	.id_table	= skel_pci_table,
	.probe		= skel_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(skel_driver, skel_pci_driver);
#else
module_comedi_driver(skel_driver);
#endif

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
