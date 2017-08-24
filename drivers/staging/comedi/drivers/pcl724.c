/*
                           

                                  

                                        
                                      
                                     
                     
                                          
                                         

                                                         
                  

                                     
                  
                                                                
                        
                                    
                                    
*/
/*
              
                                                                           
           
                                      
                                                                          
                                                                       
                

                                                                        
                                                                      
                                                                   
                                         
*/
/*
                          
                       
 */

#include "../comedidev.h"

#include <linux/ioport.h>
#include <linux/delay.h>

#include "8255.h"

#define PCL722_SIZE    32
#define PCL722_96_SIZE 16
#define PCL724_SIZE     4
#define PCL731_SIZE     8
#define PET48_SIZE      2

#define SIZE_8255	4

/*                                            */

struct pcl724_board {

	const char *name;	/*             */
	int dio;		/*             */
	int numofports;		/*                         */
	unsigned int IRQbits;	/*                     */
	unsigned int io_range;	/*                  */
	char can_have96;
	char is_pet48;
};

static int subdev_8255_cb(int dir, int port, int data, unsigned long arg)
{
	unsigned long iobase = arg;

	if (dir) {
		outb(data, iobase + port);
		return 0;
	} else {
		return inb(iobase + port);
	}
}

static int subdev_8255mapped_cb(int dir, int port, int data,
				unsigned long iobase)
{
	int movport = SIZE_8255 * (iobase >> 12);

	iobase &= 0x0fff;

	if (dir) {
		outb(port + movport, iobase);
		outb(data, iobase + 1);
		return 0;
	} else {
		outb(port + movport, iobase);
		return inb(iobase + 1);
	}
}

static int pcl724_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
	const struct pcl724_board *board = comedi_board(dev);
	struct comedi_subdevice *s;
	unsigned int iorange;
	int ret, i, n_subdevices;
#ifdef PCL724_IRQ
	unsigned int irq;
#endif

	iorange = board->io_range;
	if ((board->can_have96) &&
	    ((it->options[1] == 1) || (it->options[1] == 96)))
		iorange = PCL722_96_SIZE; /*                                 */
	ret = comedi_request_region(dev, it->options[0], iorange);
	if (ret)
		return ret;

#ifdef PCL724_IRQ
	irq = 0;
	if (board->IRQbits != 0) {	/*                   */
		irq = it->options[1];
		if (irq) {	/*                    */
			if (((1 << irq) & board->IRQbits) == 0) {
				printk(KERN_WARNING
				       ", IRQ %u is out of allowed range, "
				       "DISABLING IT", irq);
				irq = 0;	/*         */
			} else {
				if (request_irq(irq, interrupt_pcl724, 0,
					        dev->board_name, dev)) {
					printk(KERN_WARNING
					       ", unable to allocate IRQ %u, "
					       "DISABLING IT", irq);
					irq = 0;	/*               */
				} else {
					printk(", irq=%u", irq);
				}
			}
		}
	}

	dev->irq = irq;
#endif

	printk("\n");

	n_subdevices = board->numofports;
	if ((board->can_have96) && ((it->options[1] == 1)
					 || (it->options[1] == 96)))
		n_subdevices = 4;	/*                                  */

	ret = comedi_alloc_subdevices(dev, n_subdevices);
	if (ret)
		return ret;

	for (i = 0; i < dev->n_subdevices; i++) {
		s = &dev->subdevices[i];
		if (board->is_pet48) {
			subdev_8255_init(dev, s, subdev_8255mapped_cb,
					 (unsigned long)(dev->iobase +
							 i * 0x1000));
		} else
			subdev_8255_init(dev, s, subdev_8255_cb,
					 (unsigned long)(dev->iobase +
							 SIZE_8255 * i));
	}

	return 0;
}

static void pcl724_detach(struct comedi_device *dev)
{
	int i;

	for (i = 0; i < dev->n_subdevices; i++)
		comedi_spriv_free(dev, i);
	comedi_legacy_detach(dev);
}

static const struct pcl724_board boardtypes[] = {
	{ "pcl724", 24, 1, 0x00fc, PCL724_SIZE, 0, 0, },
	{ "pcl722", 144, 6, 0x00fc, PCL722_SIZE, 1, 0, },
	{ "pcl731", 48, 2, 0x9cfc, PCL731_SIZE, 0, 0, },
	{ "acl7122", 144, 6, 0x9ee8, PCL722_SIZE, 1, 0, },
	{ "acl7124", 24, 1, 0x00fc, PCL724_SIZE, 0, 0, },
	{ "pet48dio", 48, 2, 0x9eb8, PET48_SIZE, 0, 1, },
};

static struct comedi_driver pcl724_driver = {
	.driver_name	= "pcl724",
	.module		= THIS_MODULE,
	.attach		= pcl724_attach,
	.detach		= pcl724_detach,
	.board_name	= &boardtypes[0].name,
	.num_names	= ARRAY_SIZE(boardtypes),
	.offset		= sizeof(struct pcl724_board),
};
module_comedi_driver(pcl724_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
