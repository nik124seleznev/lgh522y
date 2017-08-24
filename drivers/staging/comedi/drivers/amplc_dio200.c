/*
    comedi/drivers/amplc_dio200.c

    Driver for Amplicon PC212E, PC214E, PC215E, PC218E, PC272E.

    Copyright (C) 2005-2013 MEV Ltd. <http://www.mev.co.uk/>

    COMEDI - Linux Control and Measurement Device Interface
    Copyright (C) 1998,2000 David A. Schleef <ds@schleef.org>

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

#include <linux/slab.h>

#include "../comedidev.h"

#include "amplc_dio200.h"

/*
                      
 */
static const struct dio200_board dio200_isa_boards[] = {
	{
		.name = "pc212e",
		.bustype = isa_bustype,
		.mainsize = DIO200_IO_SIZE,
		.layout = {
			.n_subdevs = 6,
			.sdtype = {sd_8255, sd_8254, sd_8254, sd_8254, sd_8254,
				   sd_intr},
			.sdinfo = {0x00, 0x08, 0x0C, 0x10, 0x14, 0x3F},
			.has_int_sce = true,
			.has_clk_gat_sce = true,
		},
	},
	{
		.name = "pc214e",
		.bustype = isa_bustype,
		.mainsize = DIO200_IO_SIZE,
		.layout = {
			.n_subdevs = 4,
			.sdtype = {sd_8255, sd_8255, sd_8254, sd_intr},
			.sdinfo = {0x00, 0x08, 0x10, 0x01},
		},
	},
	{
		.name = "pc215e",
		.bustype = isa_bustype,
		.mainsize = DIO200_IO_SIZE,
		.layout = {
			.n_subdevs = 5,
			.sdtype = {sd_8255, sd_8255, sd_8254, sd_8254, sd_intr},
			.sdinfo = {0x00, 0x08, 0x10, 0x14, 0x3F},
			.has_int_sce = true,
			.has_clk_gat_sce = true,
		},
	},
	{
		.name = "pc218e",
		.bustype = isa_bustype,
		.mainsize = DIO200_IO_SIZE,
		.layout = {
			.n_subdevs = 7,
			.sdtype = {sd_8254, sd_8254, sd_8255, sd_8254, sd_8254,
				   sd_intr},
			.sdinfo = {0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x3F},
			.has_int_sce = true,
			.has_clk_gat_sce = true,
		},
	},
	{
		.name = "pc272e",
		.bustype = isa_bustype,
		.mainsize = DIO200_IO_SIZE,
		.layout = {
			.n_subdevs = 4,
			.sdtype = {sd_8255, sd_8255, sd_8255, sd_intr},
			.sdinfo = {0x00, 0x08, 0x10, 0x3F},
			.has_int_sce = true,
		},
	},
};

static int dio200_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	const struct dio200_board *thisboard = comedi_board(dev);
	struct dio200_private *devpriv;
	unsigned int irq;
	int ret;

	irq = it->options[1];

	devpriv = kzalloc(sizeof(*devpriv), GFP_KERNEL);
	if (!devpriv)
		return -ENOMEM;
	dev->private = devpriv;

	ret = comedi_request_region(dev, it->options[0], thisboard->mainsize);
	if (ret)
		return ret;
	devpriv->io.u.iobase = dev->iobase;
	devpriv->io.regtype = io_regtype;
	return amplc_dio200_common_attach(dev, irq, 0);
}

static void dio200_detach(struct comedi_device *dev)
{
	amplc_dio200_common_detach(dev);
	comedi_legacy_detach(dev);
}

static struct comedi_driver amplc_dio200_driver = {
	.driver_name = "amplc_dio200",
	.module = THIS_MODULE,
	.attach = dio200_attach,
	.detach = dio200_detach,
	.board_name = &dio200_isa_boards[0].name,
	.offset = sizeof(struct dio200_board),
	.num_names = ARRAY_SIZE(dio200_isa_boards),
};
module_comedi_driver(amplc_dio200_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi driver for Amplicon 200 Series ISA DIO boards");
MODULE_LICENSE("GPL");
