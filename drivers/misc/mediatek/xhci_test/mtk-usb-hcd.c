//                        
//                      
//                          
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <asm/unaligned.h>
#include "xhci.h"
#include "mtk-test.h"

#if 0
/*                    */
#define PCI_VENDOR_ID_FRESCO_LOGIC	0x1b73
#define PCI_DEVICE_ID_FRESCO_LOGIC_PDK	0x1000
#endif

/*                                               */
static const size_t	pool_max [HCD_BUFFER_POOLS] = {
	/*                                                    
                                
  */
	32,
	128,
	512,
	PAGE_SIZE / 2
	/*                           */
};

#if 0
/* 
                                              
                                                         
                           
  
                                                                        
                                                                            
                                                                             
                        
  
                                                                 
 */
int hcd_buffer_create(struct usb_hcd *hcd)
{
	char		name[16];
	int 		i, size;

	if (!hcd->self.controller->dma_mask &&
	    !(hcd->driver->flags & HCD_LOCAL_MEM))
		return 0;

	for (i = 0; i < HCD_BUFFER_POOLS; i++) {
		size = pool_max[i];
		if (!size)
			continue;
		snprintf(name, sizeof name, "buffer-%d", size);
		hcd->pool[i] = dma_pool_create(name, hcd->self.controller,
				size, size, 0);
		if (!hcd->pool [i]) {
			hcd_buffer_destroy(hcd);
			return -ENOMEM;
		}
	}
	return 0;
}
#endif
#if 0
/* 
                                               
                                                       
                           
  
                                                              
 */
void hcd_buffer_destroy(struct usb_hcd *hcd)
{
	int i;

	for (i = 0; i < HCD_BUFFER_POOLS; i++) {
		struct dma_pool *pool = hcd->pool[i];
		if (pool) {
			dma_pool_destroy(pool);
			hcd->pool[i] = NULL;
		}
	}
}
#endif
#if 0
/* 
                                                      
                             
                                                         
  
                                                                  
                                             
 */
irqreturn_t usb_hcd_irq (int irq, void *__hcd)
{
	struct usb_hcd		*hcd = __hcd;
	unsigned long		flags;
	irqreturn_t		rc;

	/*                                                      
                                                         
                           
  */
	local_irq_save(flags);
	if (unlikely(hcd->state == HC_STATE_HALT ||
		     !test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags))) {
		rc = IRQ_NONE;
	} else if (hcd->driver->irq(hcd) == IRQ_NONE) {
		rc = IRQ_NONE;
	} else {
		set_bit(HCD_FLAG_SAW_IRQ, &hcd->flags);

		if (unlikely(hcd->state == HC_STATE_HALT))
			usb_hc_died(hcd);
		rc = IRQ_HANDLED;
	}

	local_irq_restore(flags);
	return rc;
}
#endif
#if 0
/*                                            */
static unsigned usb_bus_is_wusb(struct usb_bus *bus)
{
	struct usb_hcd *hcd = container_of(bus, struct usb_hcd, self);
	return hcd->wireless;
}
#endif

struct usb_device *mtk_usb_alloc_dev(struct usb_device *parent, struct usb_bus *bus, unsigned port1){
	struct usb_device *dev;
	struct usb_hcd *usb_hcd = container_of(bus, struct usb_hcd, self);
	unsigned root_hub = 0;
	
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return NULL;

	if (!usb_get_hcd(bus_to_hcd(bus))) {
		kfree(dev);
		return NULL;
	}
	device_initialize(&dev->dev);
	dev->children = kzalloc(31 * sizeof(struct usb_device *), GFP_KERNEL);
	
	/*                                                             
                                                               
                                                                   
                                                                
                                                              
                                                               
                                          
  */
	if (unlikely(!parent)) {
		dev->devpath[0] = '0';
		dev->route = 0;

		dev->dev.parent = bus->controller;
		dev_set_name(&dev->dev, "usb%d", bus->busnum);
		root_hub = 1;
	} else {
		/*                                                */
		if (parent->devpath[0] == '0') {
			snprintf(dev->devpath, sizeof dev->devpath,	"%d", port1);
			/*                                            */
			dev->route = 0;
			printk(KERN_DEBUG "device attached on roothub\n");
		} else {
			snprintf(dev->devpath, sizeof dev->devpath,	"%s.%d", parent->devpath, port1);
			/*                                                   */
			if (port1 < 15)
				dev->route = parent->route +
					(port1 << ((parent->level - 1)*4));
			else
				dev->route = parent->route +
					(15 << ((parent->level - 1)*4));
			printk(KERN_DEBUG "device route string %d\n", dev->route);
			printk(KERN_DEBUG "parent level %d\n", parent->level);
			printk(KERN_DEBUG "parent route string %d\n", parent->route);
		}

		dev->dev.parent = &parent->dev;
		dev_set_name(&dev->dev, "%d-%s", bus->busnum, dev->devpath);

		/*                               */
	}
	dev->portnum = port1;
	dev->bus = bus;
	dev->parent = parent;
	if (root_hub)	/*                                       */
		dev->authorized = 1;
	else {
		dev->authorized = usb_hcd->authorized_default;
//                                          
	}

	dev->ep0.desc.bLength = USB_DT_ENDPOINT_SIZE;
	dev->ep0.desc.bDescriptorType = USB_DT_ENDPOINT;
	dev->ep0.enabled = 1;
	dev->ep_in[0] = &dev->ep0;
	dev->ep_out[0] = &dev->ep0;
#if 0
	/*                                                   */
	usb_enable_endpoint(dev, &dev->ep0, false);
#endif
	dev->can_submit = 1;
	
	return dev;
}
#if 0
void *hcd_buffer_alloc(
	struct usb_bus 	*bus,
	size_t			size,
	gfp_t			mem_flags,
	dma_addr_t		*dma
)
{
	struct usb_hcd		*hcd = bus_to_hcd(bus);
	int 			i;

	/*                             */
	if (!bus->controller->dma_mask &&
	    !(hcd->driver->flags & HCD_LOCAL_MEM)) {
		*dma = ~(dma_addr_t) 0;
		return kmalloc(size, mem_flags);
	}

	for (i = 0; i < HCD_BUFFER_POOLS; i++) {
		if (size <= pool_max [i])
			return dma_pool_alloc(hcd->pool [i], mem_flags, dma);
	}
	return dma_alloc_coherent(hcd->self.controller, size, dma, mem_flags);
}

void hcd_buffer_free(
	struct usb_bus 	*bus,
	size_t			size,
	void 			*addr,
	dma_addr_t		dma
)
{
	struct usb_hcd		*hcd = bus_to_hcd(bus);
	int 			i;

	if (!addr)
		return;

	if (!bus->controller->dma_mask &&
	    !(hcd->driver->flags & HCD_LOCAL_MEM)) {
		kfree(addr);
		return;
	}

	for (i = 0; i < HCD_BUFFER_POOLS; i++) {
		if (size <= pool_max [i]) {
			dma_pool_free(hcd->pool [i], addr, dma);
			return;
		}
	}
	dma_free_coherent(hcd->self.controller, size, addr, dma);
}
#endif

/*
                                                                          
                                                                             
                                                                      
  
                                                                          
                                                                            
                                                                        
                                                                        
                                           
  
                                                                          
                                                                         
                                                       
  
                      
  
                                                    
                                                    
                                                 
                                               
                                    
  
                                                   
                                                   
  
                                                        
                                                     
                                     
  
 */

static int hcd_alloc_coherent(struct usb_bus *bus,
			      gfp_t mem_flags, dma_addr_t *dma_handle,
			      void **vaddr_handle, size_t size,
			      enum dma_data_direction dir)
{
	unsigned char *vaddr;

	vaddr = hcd_buffer_alloc(bus, size + sizeof(vaddr),
				 mem_flags, dma_handle);
	if (!vaddr)
		return -ENOMEM;

	/*
                                                      
                                                       
                                                    
                                                       
                                                       
                                               
  */
	put_unaligned((unsigned long)*vaddr_handle,
		      (unsigned long *)(vaddr + size));

	if (dir == DMA_TO_DEVICE)
		memcpy(vaddr, *vaddr_handle, size);

	*vaddr_handle = vaddr;
	return 0;
}

void hcd_free_coherent(struct usb_bus *bus, dma_addr_t *dma_handle,
			      void **vaddr_handle, size_t size,
			      enum dma_data_direction dir)
{
	unsigned char *vaddr = *vaddr_handle;

	vaddr = (void *)get_unaligned((unsigned long *)(vaddr + size));

	if (dir == DMA_FROM_DEVICE)
		memcpy(vaddr, *vaddr_handle, size);

	hcd_buffer_free(bus, size + sizeof(vaddr), *vaddr_handle, *dma_handle);

	*vaddr_handle = vaddr;
	*dma_handle = 0;
}

#if 0
void unmap_urb_for_dma(struct usb_hcd *hcd, struct urb *urb)
{
	enum dma_data_direction dir;

	dir = usb_urb_dir_in(urb) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
	hcd_free_coherent(urb->dev->bus,
			&urb->transfer_dma,
			&urb->transfer_buffer,
			urb->transfer_buffer_length,
			dir);

}


int map_urb_for_dma(struct usb_hcd *hcd, struct urb *urb,
			   gfp_t mem_flags)
{
	enum dma_data_direction dir;
	int ret = 0;
	dir = usb_urb_dir_in(urb) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
	/*                                      
                                                      
                                                     
                                                      
  */
	ret = hcd_alloc_coherent(
					urb->dev->bus, mem_flags,
					&urb->transfer_dma,
					&urb->transfer_buffer,
					urb->transfer_buffer_length,
					dir);

	if (ret == 0)
		urb->transfer_flags |= URB_MAP_LOCAL;
	return ret;
}

void rh_port_clear_change(struct xhci_hcd *xhci, int port_id){
	u32 temp,status;
	u32 __iomem *addr;
	port_id--;
	status = 0;
	
	addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id & 0xff);
	temp = xhci_readl(xhci, addr);
	xhci_dbg(xhci, "to clear port change, actual port %d status  = 0x%x\n", port_id, temp);
	temp = mtktest_xhci_port_state_to_clear_change(temp);
	xhci_writel(xhci, temp, addr);
	temp = xhci_readl(xhci, addr);
	xhci_dbg(xhci, "clear port change, actual port %d status  = 0x%x\n", port_id, temp);
}

int rh_get_port_status(struct xhci_hcd *xhci, int port_id){
	u32 temp,status;
	u32 __iomem *addr;
	
	port_id--;
	status = 0;
	
	addr = &xhci->op_regs->port_status_base + NUM_PORT_REGS*(port_id & 0xff);
	temp = xhci_readl(xhci, addr);
	xhci_dbg(xhci, "get port status, actual port %d status  = 0x%x\n", port_id, temp);

	/*                  */
	if (temp & PORT_CSC)
		status |= USB_PORT_STAT_C_CONNECTION << 16;
	if (temp & PORT_PEC)
		status |= USB_PORT_STAT_C_ENABLE << 16;
	if ((temp & PORT_OCC))
		status |= USB_PORT_STAT_C_OVERCURRENT << 16;
	if ((temp & PORT_RC))
		status |= USB_PORT_STAT_C_RESET << 16;
	if ((temp & PORT_PLC))
		status |= USB_PORT_STAT_C_SUSPEND << 16;
	/*
                                                           
           
  */
	if (temp & PORT_CONNECT) {
		status |= USB_PORT_STAT_CONNECTION;
		status |= mtktest_xhci_port_speed(temp);
	}
	if (temp & PORT_PE)
		status |= USB_PORT_STAT_ENABLE;
	if (temp & PORT_OC)
		status |= USB_PORT_STAT_OVERCURRENT;
	if (temp & PORT_RESET)
		status |= USB_PORT_STAT_RESET;
	if (temp & PORT_POWER)
		status |= USB_PORT_STAT_POWER;
	xhci_dbg(xhci, "Get port status returned 0x%x\n", status);
	temp = mtktest_xhci_port_state_to_neutral(temp);
	xhci_writel(xhci, temp, addr);
	temp = xhci_readl(xhci, addr);
	xhci_dbg(xhci, "clear port change, actual port %d status  = 0x%x\n", port_id, temp);
#if 0
	put_unaligned(cpu_to_le32(status), (__le32 *) buf);
#endif
	return status;
}
#endif

int mtk_map_urb_for_dma(struct usb_hcd *hcd, struct urb *urb, gfp_t mem_flags){
	enum dma_data_direction dir;
	int ret = 0;
	dir = usb_urb_dir_in(urb) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
	/*                                      
                                                      
                                                     
                                                      
  */
	ret = hcd_alloc_coherent(
					urb->dev->bus, mem_flags,
					&urb->transfer_dma,
					&urb->transfer_buffer,
					urb->transfer_buffer_length,
					dir);

	if (ret == 0)
		urb->transfer_flags |= URB_MAP_LOCAL;
	return ret;
}
void mtk_unmap_urb_for_dma(struct usb_hcd *hcd, struct urb *urb){
	enum dma_data_direction dir;

	dir = usb_urb_dir_in(urb) ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
	hcd_free_coherent(urb->dev->bus,
			&urb->transfer_dma,
			&urb->transfer_buffer,
			urb->transfer_buffer_length,
			dir);
}

/* 
                                                            
                                                                         
                                      
                                                         
                           
  
                                                                
                                      
  
                                                       
 */
struct usb_device *mtk_usb_alloc_rhdev(struct usb_device *parent,
				 struct usb_bus *bus, unsigned port1)
{
	struct usb_device *dev;
	struct usb_hcd *usb_hcd = container_of(bus, struct usb_hcd, self);
	unsigned root_hub = 0;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return NULL;

	if (!usb_get_hcd(bus_to_hcd(bus))) {
		kfree(dev);
		return NULL;
	}
	device_initialize(&dev->dev);
	dev->children = kzalloc(31 * sizeof(struct usb_device *), GFP_KERNEL);
	dev->dev.dma_mask = bus->controller->dma_mask;
	atomic_set(&dev->urbnum, 0);
	dev->can_submit = 1;

	/*                                                             
                                                               
                                                                   
                                                                
                                                              
                                                               
                                          
  */
	if (unlikely(!parent)) {
		dev->devpath[0] = '0';
		dev->route = 0;
		dev->dev.parent = bus->controller;
		dev_set_name(&dev->dev, "usb%d", bus->busnum);
		root_hub = 1;
	} 

	dev->portnum = port1;
	dev->bus = bus;
	dev->parent = parent;

	dev->authorized = 1;
	return dev;
}


/* 
                                                                         
                                            
                                      
                                  
  
                                                                         
                                                                        
                                                      
 */
int mtk_usb_add_hcd(struct usb_hcd *hcd,
		unsigned int irqnum, unsigned long irqflags)
{
	int retval;
	struct usb_device *rhdev;
	dev_info(hcd->self.controller, "%s\n", hcd->product_desc);

	hcd->authorized_default = hcd->wireless? 0 : 1;
	set_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags);

	/*                                                                 
                                                                   
                                                                  
  */
	if ((retval = hcd_buffer_create(hcd)) != 0) {
		dev_dbg(hcd->self.controller, "pool alloc failed\n");
		return retval;
	}
	
	if ((rhdev = mtk_usb_alloc_rhdev(NULL, &hcd->self, 0)) == NULL) {
		dev_err(hcd->self.controller, "unable to allocate root hub\n");
		retval = -ENOMEM;
		goto err_allocate_root_hub;
	}
	hcd->self.root_hub = rhdev;
#if 0
	if ((retval = usb_register_bus(&hcd->self)) < 0)
		goto err_register_bus;

	if ((rhdev = usb_alloc_dev(NULL, &hcd->self, 0)) == NULL) {
		dev_err(hcd->self.controller, "unable to allocate root hub\n");
		retval = -ENOMEM;
		goto err_allocate_root_hub;
	}

	switch (hcd->driver->flags & HCD_MASK) {
	case HCD_USB11:
		rhdev->speed = USB_SPEED_FULL;
		break;
	case HCD_USB2:
		rhdev->speed = USB_SPEED_HIGH;
		break;
	case HCD_USB3:
		rhdev->speed = USB_SPEED_SUPER;
		break;
	default:
		goto err_allocate_root_hub;
	}
	hcd->self.root_hub = rhdev;

	/*                                                               
                                                                
                                                                
  */
	device_init_wakeup(&rhdev->dev, 1);
#endif

	/*                                                                   
                                                                      
  */
	printk(KERN_DEBUG "call xhci_mtk_setup\n");
	if (hcd->driver->reset && (retval = hcd->driver->reset(hcd)) < 0) {
		dev_err(hcd->self.controller, "can't setup\n");
		goto err_hcd_driver_setup;
	}
#if 0
	/*                                                                */
	if (device_can_wakeup(hcd->self.controller)
			&& device_can_wakeup(&hcd->self.root_hub->dev))
		dev_dbg(hcd->self.controller, "supports USB remote wakeup\n");
#endif
	/*                                                 */
	if (hcd->driver->irq) {

		/*                                                            
                                                           
                                      
   */
		if (irqflags & IRQF_SHARED)
			irqflags &= ~IRQF_DISABLED;

		snprintf(hcd->irq_descr, sizeof(hcd->irq_descr), "%s:usb%d",
				hcd->driver->description, hcd->self.busnum);
		if ((retval = request_irq(irqnum, &usb_hcd_irq, irqflags,
				hcd->irq_descr, hcd)) != 0) {
			dev_err(hcd->self.controller,
					"request interrupt %d failed\n", irqnum);
			goto err_request_irq;
		}
		hcd->irq = irqnum;
		dev_info(hcd->self.controller, "irq %d, %s 0x%08llx\n", irqnum,
				(hcd->driver->flags & HCD_MEMORY) ?
					"io mem" : "io base",
					(unsigned long long)hcd->rsrc_start);
	} else {
		hcd->irq = -1;
		if (hcd->rsrc_start)
			dev_info(hcd->self.controller, "%s 0x%08llx\n",
					(hcd->driver->flags & HCD_MEMORY) ?
					"io mem" : "io base",
					(unsigned long long)hcd->rsrc_start);
	}

	if ((retval = hcd->driver->start(hcd)) < 0) {
		dev_err(hcd->self.controller, "startup error %d\n", retval);
		goto err_hcd_driver_start;
	}
#if 0
	/*                                                            */
	rhdev->bus_mA = min(500u, hcd->power_budget);
	if ((retval = register_root_hub(hcd)) != 0)
		goto err_register_root_hub;

	retval = sysfs_create_group(&rhdev->dev.kobj, &usb_bus_attr_group);
	if (retval < 0) {
		printk(KERN_ERR "Cannot register USB bus sysfs attributes: %d\n",
		       retval);
		goto error_create_attr_group;
	}
	if (hcd->uses_new_polling && hcd->poll_rh)
		usb_hcd_poll_rh_status(hcd);
#endif
	return retval;

error_create_attr_group:
	mutex_lock(&usb_bus_list_lock);
	usb_disconnect(&hcd->self.root_hub);
	mutex_unlock(&usb_bus_list_lock);
err_register_root_hub:
	hcd->driver->stop(hcd);
err_hcd_driver_start:
	if (hcd->irq >= 0)
		free_irq(irqnum, hcd);
err_request_irq:
err_hcd_driver_setup:
	hcd->self.root_hub = NULL;
	usb_put_dev(rhdev);
#if 1
err_allocate_root_hub:
	hcd->driver->stop(hcd);
#endif
err_register_bus:
	hcd_buffer_destroy(hcd);
	return retval;
} 


/* 
                                                        
                                        
                           
  
                                                                        
                                    
 */
void mtk_usb_remove_hcd(struct usb_hcd *hcd)
{
	dev_info(hcd->self.controller, "remove, state %x\n", hcd->state);

	if (HC_IS_RUNNING (hcd->state))
		hcd->state = HC_STATE_QUIESCING;

	dev_dbg(hcd->self.controller, "roothub graceful disconnect\n");
#if 0
	spin_lock_irq (&hcd_root_hub_lock);
	hcd->rh_registered = 0;
	spin_unlock_irq (&hcd_root_hub_lock);
#endif
#if 0 //                         
	cancel_work_sync(&hcd->wakeup_work);
#endif
#if 0
	sysfs_remove_group(&hcd->self.root_hub->dev.kobj, &usb_bus_attr_group);
	mutex_lock(&usb_bus_list_lock);
	usb_disconnect(&hcd->self.root_hub);
	mutex_unlock(&usb_bus_list_lock);
#endif
	hcd->driver->stop(hcd);
	hcd->state = HC_STATE_HALT;
#if 0
	hcd->poll_rh = 0;
	del_timer_sync(&hcd->rh_timer);
#endif
	if (hcd->irq >= 0)
		free_irq(hcd->irq, hcd);
#if 0
	usb_deregister_bus(&hcd->self);
#endif
	hcd_buffer_destroy(hcd);
}
/* 
                                            
                                            
  
                                                                           
                      
 */
static void mtk_usb_bus_init (struct usb_bus *bus)
{
	memset (&bus->devmap, 0, sizeof(struct usb_devmap));

	bus->devnum_next = 1;

	bus->root_hub = NULL;
	bus->busnum = -1;
	bus->bandwidth_allocated = 0;
	bus->bandwidth_int_reqs  = 0;
	bus->bandwidth_isoc_reqs = 0;

	INIT_LIST_HEAD (&bus->bus_list);
}

/*                                                                         */

/* 
                                                          
                                            
                                                           
                                                  
                           
  
                                                                 
                                                                   
                 
  
                                          
 */
struct usb_hcd *mtk_usb_create_hcd (const struct hc_driver *driver,
		struct device *dev, const char *bus_name)
{
	struct usb_hcd *hcd;

	hcd = kzalloc(sizeof(*hcd) + driver->hcd_priv_size, GFP_KERNEL);
	if (!hcd) {
		dev_dbg (dev, "hcd alloc failed\n");
		return NULL;
	}

    hcd->bandwidth_mutex = kmalloc(sizeof(*hcd->bandwidth_mutex), GFP_KERNEL);
	if (!hcd->bandwidth_mutex) {
			kfree(hcd);
			dev_dbg(dev, "hcd bandwidth mutex alloc failed\n");
			return NULL;
	}
    mutex_init(hcd->bandwidth_mutex);
    
    printk(KERN_ERR "====%s(%d)==== hcd->pool[0] = 0x%p!\n", __func__, __LINE__, hcd->pool[0]);
	dev_set_drvdata(dev, hcd);
	kref_init(&hcd->kref);
    
    printk(KERN_ERR "====%s(%d)==== hcd->pool[0] = 0x%p!\n", __func__, __LINE__, hcd->pool[0]);
    
	mtk_usb_bus_init(&hcd->self);
	hcd->self.controller = dev;
	hcd->self.bus_name = bus_name;
	hcd->self.uses_dma = (dev->dma_mask != NULL);

    printk(KERN_ERR "====%s(%d)==== hcd->pool[0] = 0x%p!\n", __func__, __LINE__, hcd->pool[0]);
    
//                            
//                                        
//                                          
#if 0 //                         
	INIT_WORK(&hcd->wakeup_work, hcd_resume_work);
#endif
    printk(KERN_ERR "====%s(%d)==== hcd->pool[0] = 0x%p!\n", __func__, __LINE__, hcd->pool[0]);
	hcd->driver = driver;
	hcd->product_desc = (driver->product_desc) ? driver->product_desc :
			"USB Host Controller";
	return hcd;
}


/*                                                                           
                                                                                
              
 */
unsigned int mtktest_xhci_get_endpoint_flag(struct usb_endpoint_descriptor *desc)
{
	return 1 << (mtktest_xhci_get_endpoint_index(desc) + 1);
}

