/*
 * PCI Error Recovery Driver for RPA-compliant PPC64 platform.
 * Copyright IBM Corp. 2004 2005
 * Copyright Linas Vepstas <linas@linas.org> 2004, 2005
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 * NON INFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Send comments and feedback to Linas Vepstas <linas@austin.ibm.com>
 */
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <asm/eeh.h>
#include <asm/eeh_event.h>
#include <asm/ppc-pci.h>
#include <asm/pci-bridge.h>
#include <asm/prom.h>
#include <asm/rtas.h>

/* 
                                                     
                    
  
                                                                 
                   
 */
static inline const char *eeh_pcid_name(struct pci_dev *pdev)
{
	if (pdev && pdev->dev.driver)
		return pdev->dev.driver->name;
	return "";
}

/* 
                                           
                    
  
                                                             
                                                                    
                                                             
                                                  
 */
static inline struct pci_driver *eeh_pcid_get(struct pci_dev *pdev)
{
	if (!pdev || !pdev->driver)
		return NULL;

	if (!try_module_get(pdev->driver->driver.owner))
		return NULL;

	return pdev->driver;
}

/* 
                                                      
                    
  
                                                             
                                      
 */
static inline void eeh_pcid_put(struct pci_dev *pdev)
{
	if (!pdev || !pdev->driver)
		return;

	module_put(pdev->driver->driver.owner);
}

#if 0
static void print_device_node_tree(struct pci_dn *pdn, int dent)
{
	int i;
	struct device_node *pc;

	if (!pdn)
		return;
	for (i = 0; i < dent; i++)
		printk(" ");
	printk("dn=%s mode=%x \tcfg_addr=%x pe_addr=%x \tfull=%s\n",
		pdn->node->name, pdn->eeh_mode, pdn->eeh_config_addr,
		pdn->eeh_pe_config_addr, pdn->node->full_name);
	dent += 3;
	pc = pdn->node->child;
	while (pc) {
		print_device_node_tree(PCI_DN(pc), dent);
		pc = pc->sibling;
	}
}
#endif

/* 
                                                                
                   
  
                                                                    
                                                                  
                                                                       
                                                                     
                                                                
 */
static void eeh_disable_irq(struct pci_dev *dev)
{
	struct eeh_dev *edev = pci_dev_to_eeh_dev(dev);

	/*                                                 
                                                 
                             
  */
	if (dev->msi_enabled || dev->msix_enabled)
		return;

	if (!irq_has_action(dev->irq))
		return;

	edev->mode |= EEH_DEV_IRQ_DISABLED;
	disable_irq_nosync(dev->irq);
}

/* 
                                                              
                   
  
                                                               
                           
 */
static void eeh_enable_irq(struct pci_dev *dev)
{
	struct eeh_dev *edev = pci_dev_to_eeh_dev(dev);

	if ((edev->mode) & EEH_DEV_IRQ_DISABLED) {
		edev->mode &= ~EEH_DEV_IRQ_DISABLED;
		enable_irq(dev->irq);
	}
}

/* 
                                                            
                    
                          
   
                                                             
                                                          
                             
 */
static void *eeh_report_error(void *data, void *userdata)
{
	struct eeh_dev *edev = (struct eeh_dev *)data;
	struct pci_dev *dev = eeh_dev_to_pci_dev(edev);
	enum pci_ers_result rc, *res = userdata;
	struct pci_driver *driver;

	/*                                             
                                         
  */
	if (!dev) return NULL;
	dev->error_state = pci_channel_io_frozen;

	driver = eeh_pcid_get(dev);
	if (!driver) return NULL;

	eeh_disable_irq(dev);

	if (!driver->err_handler ||
	    !driver->err_handler->error_detected) {
		eeh_pcid_put(dev);
		return NULL;
	}

	rc = driver->err_handler->error_detected(dev, pci_channel_io_frozen);

	/*                                               */
	if (rc == PCI_ERS_RESULT_NEED_RESET) *res = rc;
	if (*res == PCI_ERS_RESULT_NONE) *res = rc;

	eeh_pcid_put(dev);
	return NULL;
}

/* 
                                                                    
                    
                          
  
                                                                    
                                                                       
                                                 
 */
static void *eeh_report_mmio_enabled(void *data, void *userdata)
{
	struct eeh_dev *edev = (struct eeh_dev *)data;
	struct pci_dev *dev = eeh_dev_to_pci_dev(edev);
	enum pci_ers_result rc, *res = userdata;
	struct pci_driver *driver;

	driver = eeh_pcid_get(dev);
	if (!driver) return NULL;

	if (!driver->err_handler ||
	    !driver->err_handler->mmio_enabled) {
		eeh_pcid_put(dev);
		return NULL;
	}

	rc = driver->err_handler->mmio_enabled(dev);

	/*                                               */
	if (rc == PCI_ERS_RESULT_NEED_RESET) *res = rc;
	if (*res == PCI_ERS_RESULT_NONE) *res = rc;

	eeh_pcid_put(dev);
	return NULL;
}

/* 
                                                          
                    
                          
  
                                                                  
                                                                 
                                                                  
                                                       
 */
static void *eeh_report_reset(void *data, void *userdata)
{
	struct eeh_dev *edev = (struct eeh_dev *)data;
	struct pci_dev *dev = eeh_dev_to_pci_dev(edev);
	enum pci_ers_result rc, *res = userdata;
	struct pci_driver *driver;

	if (!dev) return NULL;
	dev->error_state = pci_channel_io_normal;

	driver = eeh_pcid_get(dev);
	if (!driver) return NULL;

	eeh_enable_irq(dev);

	if (!driver->err_handler ||
	    !driver->err_handler->slot_reset) {
		eeh_pcid_put(dev);
		return NULL;
	}

	rc = driver->err_handler->slot_reset(dev);
	if ((*res == PCI_ERS_RESULT_NONE) ||
	    (*res == PCI_ERS_RESULT_RECOVERED)) *res = rc;
	if (*res == PCI_ERS_RESULT_DISCONNECT &&
	     rc == PCI_ERS_RESULT_NEED_RESET) *res = rc;

	eeh_pcid_put(dev);
	return NULL;
}

/* 
                                                              
                    
                          
  
                                                                  
                                                                    
                                           
 */
static void *eeh_report_resume(void *data, void *userdata)
{
	struct eeh_dev *edev = (struct eeh_dev *)data;
	struct pci_dev *dev = eeh_dev_to_pci_dev(edev);
	struct pci_driver *driver;

	if (!dev) return NULL;
	dev->error_state = pci_channel_io_normal;

	driver = eeh_pcid_get(dev);
	if (!driver) return NULL;

	eeh_enable_irq(dev);

	if (!driver->err_handler ||
	    !driver->err_handler->resume) {
		eeh_pcid_put(dev);
		return NULL;
	}

	driver->err_handler->resume(dev);

	eeh_pcid_put(dev);
	return NULL;
}

/* 
                                                               
                    
                          
  
                                                                
                                                                  
 */
static void *eeh_report_failure(void *data, void *userdata)
{
	struct eeh_dev *edev = (struct eeh_dev *)data;
	struct pci_dev *dev = eeh_dev_to_pci_dev(edev);
	struct pci_driver *driver;

	if (!dev) return NULL;
	dev->error_state = pci_channel_io_perm_failure;

	driver = eeh_pcid_get(dev);
	if (!driver) return NULL;

	eeh_disable_irq(dev);

	if (!driver->err_handler ||
	    !driver->err_handler->error_detected) {
		eeh_pcid_put(dev);
		return NULL;
	}

	driver->err_handler->error_detected(dev, pci_channel_io_perm_failure);

	eeh_pcid_put(dev);
	return NULL;
}

/* 
                                                        
              
                                                    
  
                                                               
                                                                 
                                              
 */
static int eeh_reset_device(struct eeh_pe *pe, struct pci_bus *bus)
{
	int cnt, rc;

	/*                                                */
	cnt = pe->freeze_count;

	/*
                                                          
                                                        
                                                         
                                   
  */
	if (bus)
		__pcibios_remove_pci_devices(bus, 0);

	/*                                                               
                                                                  
                                           
  */
	rc = eeh_reset_pe(pe);
	if (rc)
		return rc;

	/*            */
	eeh_ops->configure_bridge(pe);
	eeh_pe_restore_bars(pe);

	/*                                                           
                                                              
                                                              
                                                         
                                    
  */
	if (bus) {
		ssleep(5);
		pcibios_add_pci_devices(bus);
	}
	pe->freeze_count = cnt;

	return 0;
}

/*                                                    
                                    
 */
#define MAX_WAIT_FOR_RECOVERY 150

/* 
                                                           
              
  
                                                                    
                                                                   
                                                                
                                                                  
                                                                      
  
                                                                   
                                                                     
                                                                      
                                                                    
                                                                   
              
 */
void eeh_handle_event(struct eeh_pe *pe)
{
	struct pci_bus *frozen_bus;
	int rc = 0;
	enum pci_ers_result result = PCI_ERS_RESULT_NONE;

	frozen_bus = eeh_pe_bus_get(pe);
	if (!frozen_bus) {
		pr_err("%s: Cannot find PCI bus for PHB#%d-PE#%x\n",
			__func__, pe->phb->global_number, pe->addr);
		return;
	}

	pe->freeze_count++;
	if (pe->freeze_count > EEH_MAX_ALLOWED_FREEZES)
		goto excess_failures;
	pr_warning("EEH: This PCI device has failed %d times in the last hour\n",
		pe->freeze_count);

	/*                                                              
                                                                    
                                                             
                                                                   
                                    
  */
	eeh_pe_dev_traverse(pe, eeh_report_error, &result);

	/*                                                           
                                                 
  */
	rc = eeh_ops->wait_state(pe, MAX_WAIT_FOR_RECOVERY*1000);
	if (rc < 0 || rc == EEH_STATE_NOT_SUPPORT) {
		printk(KERN_WARNING "EEH: Permanent failure\n");
		goto hard_fail;
	}

	/*                                                       
                                                        
                       
  */
	eeh_slot_error_detail(pe, EEH_LOG_TEMP);

	/*                                                  
                                                 
                                                   
  */
	if (result == PCI_ERS_RESULT_NONE) {
		rc = eeh_reset_device(pe, frozen_bus);
		if (rc) {
			printk(KERN_WARNING "EEH: Unable to reset, rc=%d\n", rc);
			goto hard_fail;
		}
	}

	/*                                                               */
	if (result == PCI_ERS_RESULT_CAN_RECOVER) {
		rc = eeh_pci_enable(pe, EEH_OPT_THAW_MMIO);

		if (rc < 0)
			goto hard_fail;
		if (rc) {
			result = PCI_ERS_RESULT_NEED_RESET;
		} else {
			result = PCI_ERS_RESULT_NONE;
			eeh_pe_dev_traverse(pe, eeh_report_mmio_enabled, &result);
		}
	}

	/*                                                              */
	if (result == PCI_ERS_RESULT_CAN_RECOVER) {
		rc = eeh_pci_enable(pe, EEH_OPT_THAW_DMA);

		if (rc < 0)
			goto hard_fail;
		if (rc)
			result = PCI_ERS_RESULT_NEED_RESET;
		else
			result = PCI_ERS_RESULT_RECOVERED;
	}

	/*                                                             */
	if (result == PCI_ERS_RESULT_DISCONNECT) {
		printk(KERN_WARNING "EEH: Device driver gave up\n");
		goto hard_fail;
	}

	/*                                                           */
	if (result == PCI_ERS_RESULT_NEED_RESET) {
		rc = eeh_reset_device(pe, NULL);
		if (rc) {
			printk(KERN_WARNING "EEH: Cannot reset, rc=%d\n", rc);
			goto hard_fail;
		}
		result = PCI_ERS_RESULT_NONE;
		eeh_pe_dev_traverse(pe, eeh_report_reset, &result);
	}

	/*                                                      */
	if ((result != PCI_ERS_RESULT_RECOVERED) &&
	    (result != PCI_ERS_RESULT_NONE)) {
		printk(KERN_WARNING "EEH: Not recovered\n");
		goto hard_fail;
	}

	/*                                                         */
	eeh_pe_dev_traverse(pe, eeh_report_resume, NULL);

	return;
	
excess_failures:
	/*
                                                        
                                                          
                                
  */
	pr_err("EEH: PHB#%d-PE#%x has failed %d times in the\n"
	       "last hour and has been permanently disabled.\n"
	       "Please try reseating or replacing it.\n",
		pe->phb->global_number, pe->addr,
		pe->freeze_count);
	goto perm_error;

hard_fail:
	pr_err("EEH: Unable to recover from failure from PHB#%d-PE#%x.\n"
	       "Please try reseating or replacing it\n",
		pe->phb->global_number, pe->addr);

perm_error:
	eeh_slot_error_detail(pe, EEH_LOG_PERM);

	/*                                                   */
	eeh_pe_dev_traverse(pe, eeh_report_failure, NULL);

	/*                                        */
	if (frozen_bus)
		pcibios_remove_pci_devices(frozen_bus);
}

