/*
 * EHCI HCD (Host Controller Driver) PCI Bus Glue.
 *
 * Copyright (c) 2000-2004 by David Brownell
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>

#include "ehci.h"
#include "pci-quirks.h"

#define DRIVER_DESC "EHCI PCI platform driver"

static const char hcd_name[] = "ehci-pci";

/*                                                                   */
#define PCI_DEVICE_ID_INTEL_CE4100_USB	0x2e70

/*                                                                         */
#define PCI_DEVICE_ID_INTEL_QUARK_X1000_SOC		0x0939
static inline bool is_intel_quark_x1000(struct pci_dev *pdev)
{
	return pdev->vendor == PCI_VENDOR_ID_INTEL &&
		pdev->device == PCI_DEVICE_ID_INTEL_QUARK_X1000_SOC;
}

/*
                                                   
                                                         
 */
#define	intel_quark_x1000_insnreg01	hostpc

/*                                                                   */
#define INTEL_QUARK_X1000_EHCI_MAX_THRESHOLD	0x007f007f

/*                                                      */
static int ehci_pci_reinit(struct ehci_hcd *ehci, struct pci_dev *pdev)
{
	int			retval;

	/*                                                                  
                                                                 
  */

	/*                                                                  */
	retval = pci_set_mwi(pdev);
	if (!retval)
		ehci_dbg(ehci, "MWI active\n");

	/*                           */
	if (is_intel_quark_x1000(pdev)) {
		/*
                                                              
                                                          
   */
		ehci_writel(ehci, INTEL_QUARK_X1000_EHCI_MAX_THRESHOLD,
			ehci->regs->intel_quark_x1000_insnreg01);
	}

	return 0;
}

/*                                                  */
static int ehci_pci_setup(struct usb_hcd *hcd)
{
	struct ehci_hcd		*ehci = hcd_to_ehci(hcd);
	struct pci_dev		*pdev = to_pci_dev(hcd->self.controller);
	struct pci_dev		*p_smbus;
	u8			rev;
	u32			temp;
	int			retval;

	ehci->caps = hcd->regs;

	/*
                                                     
                                                              
                                                           
                                         
   
                                                             
                                              
  */
	switch (pdev->vendor) {
	case PCI_VENDOR_ID_TOSHIBA_2:
		/*                         */
		if (pdev->device == 0x01b5) {
#ifdef CONFIG_USB_EHCI_BIG_ENDIAN_MMIO
			ehci->big_endian_mmio = 1;
#else
			ehci_warn(ehci,
				  "unsupported big endian Toshiba quirk\n");
#endif
		}
		break;
	case PCI_VENDOR_ID_NVIDIA:
		/*                                               
                                                    
                                                    
   */
		switch (pdev->device) {
		case 0x003c:	/*       */
		case 0x005b:	/*       */
		case 0x00d8:	/*     */
		case 0x00e8:	/*      */
			if (pci_set_consistent_dma_mask(pdev,
						DMA_BIT_MASK(31)) < 0)
				ehci_warn(ehci, "can't enable NVidia "
					"workaround for >2GB RAM\n");
			break;

		/*                                                         
                            
   */
		case 0x0068:
			if (pdev->revision < 0xa4)
				ehci->no_selective_suspend = 1;
			break;
		}
		break;
	case PCI_VENDOR_ID_INTEL:
		if (pdev->device == PCI_DEVICE_ID_INTEL_CE4100_USB)
			hcd->has_tt = 1;
		break;
	case PCI_VENDOR_ID_TDI:
		if (pdev->device == PCI_DEVICE_ID_TDI_EHCI)
			hcd->has_tt = 1;
		break;
	case PCI_VENDOR_ID_AMD:
		/*               */
		if (usb_amd_find_chipset_info())
			ehci->amd_pll_fix = 1;
		/*                                                    */
		if (pdev->device == 0x7463) {
			ehci_info(ehci, "ignoring AMD8111 (errata)\n");
			retval = -EIO;
			goto done;
		}

		/*
                                                                
                                                             
                                                                
                                                           
                                                            
   */
		if (pdev->device == 0x7808) {
			ehci->use_dummy_qh = 1;
			ehci_info(ehci, "applying AMD SB700/SB800/Hudson-2/3 EHCI dummy qh workaround\n");
		}
		break;
	case PCI_VENDOR_ID_VIA:
		if (pdev->device == 0x3104 && (pdev->revision & 0xf0) == 0x60) {
			u8 tmp;

			/*                                                      
                                                           
                                                   
    */
			pci_read_config_byte(pdev, 0x4b, &tmp);
			if (tmp & 0x20)
				break;
			pci_write_config_byte(pdev, 0x4b, tmp | 0x20);
		}
		break;
	case PCI_VENDOR_ID_ATI:
		/*               */
		if (usb_amd_find_chipset_info())
			ehci->amd_pll_fix = 1;

		/*
                                                                
                                                             
                                                                
                                                           
                                                            
   */
		if (pdev->device == 0x4396) {
			ehci->use_dummy_qh = 1;
			ehci_info(ehci, "applying AMD SB700/SB800/Hudson-2/3 EHCI dummy qh workaround\n");
		}
		/*                                                              
                                                          
   */
		if ((pdev->device == 0x4386) || (pdev->device == 0x4396)) {
			p_smbus = pci_get_device(PCI_VENDOR_ID_ATI,
						 PCI_DEVICE_ID_ATI_SBX00_SMBUS,
						 NULL);
			if (!p_smbus)
				break;
			rev = p_smbus->revision;
			if ((pdev->device == 0x4386) || (rev == 0x3a)
			    || (rev == 0x3b)) {
				u8 tmp;
				ehci_info(ehci, "applying AMD SB600/SB700 USB "
					"freeze workaround\n");
				pci_read_config_byte(pdev, 0x53, &tmp);
				pci_write_config_byte(pdev, 0x53, tmp | (1<<3));
			}
			pci_dev_put(p_smbus);
		}
		break;
	case PCI_VENDOR_ID_NETMOS:
		/*                                  */
		ehci_info(ehci, "applying MosChip frame-index workaround\n");
		ehci->frame_index_bug = 1;
		break;
	}

	/*                                                */
	temp = pci_find_capability(pdev, PCI_CAP_ID_DBG);
	if (temp) {
		pci_read_config_dword(pdev, temp, &temp);
		temp >>= 16;
		if (((temp >> 13) & 7) == 1) {
			u32 hcs_params = ehci_readl(ehci,
						    &ehci->caps->hcs_params);

			temp &= 0x1fff;
			ehci->debug = hcd->regs + temp;
			temp = ehci_readl(ehci, &ehci->debug->control);
			ehci_info(ehci, "debug port %d%s\n",
				  HCS_DEBUG_PORT(hcs_params),
				  (temp & DBGP_ENABLED) ? " IN USE" : "");
			if (!(temp & DBGP_ENABLED))
				ehci->debug = NULL;
		}
	}

	retval = ehci_setup(hcd);
	if (retval)
		return retval;

	/*                                                         */
	switch (pdev->vendor) {
	case PCI_VENDOR_ID_NEC:
		ehci->need_io_watchdog = 0;
		break;
	case PCI_VENDOR_ID_INTEL:
		ehci->need_io_watchdog = 0;
		break;
	case PCI_VENDOR_ID_NVIDIA:
		switch (pdev->device) {
		/*                                                  
                                                        
                                                     
                               
   */
		case 0x0d9d:
			ehci_info(ehci, "disable ppcd for nvidia mcp89\n");
			ehci->has_ppcd = 0;
			ehci->command &= ~CMD_PPCEE;
			break;
		}
		break;
	}

	/*                                              */
	temp = HCS_N_CC(ehci->hcs_params) * HCS_N_PCC(ehci->hcs_params);
	temp &= 0x0f;
	if (temp && HCS_N_PORTS(ehci->hcs_params) > temp) {
		ehci_dbg(ehci, "bogus port configuration: "
			"cc=%d x pcc=%d < ports=%d\n",
			HCS_N_CC(ehci->hcs_params),
			HCS_N_PCC(ehci->hcs_params),
			HCS_N_PORTS(ehci->hcs_params));

		switch (pdev->vendor) {
		case 0x17a0:		/*         */
			/*                           */
			temp |= (ehci->hcs_params & ~0xf);
			ehci->hcs_params = temp;
			break;
		case PCI_VENDOR_ID_NVIDIA:
			/*                       */
			break;
		}
	}

	/*                                                 */
	if (pdev->vendor == PCI_VENDOR_ID_STMICRO
	    && pdev->device == PCI_DEVICE_ID_STMICRO_USB_HOST)
		;	/*                              */
	else
		pci_read_config_byte(pdev, 0x60, &ehci->sbrn);

	/*                                                    
                                                         
                                                          
                           
  */
	if (!device_can_wakeup(&pdev->dev)) {
		u16	port_wake;

		pci_read_config_word(pdev, 0x62, &port_wake);
		if (port_wake & 0x0001) {
			dev_warn(&pdev->dev, "Enabling legacy PCI PM\n");
			device_set_wakeup_capable(&pdev->dev, 1);
		}
	}

#ifdef	CONFIG_PM_RUNTIME
	if (ehci->no_selective_suspend && device_can_wakeup(&pdev->dev))
		ehci_warn(ehci, "selective suspend/wakeup unavailable\n");
#endif

	retval = ehci_pci_reinit(ehci, pdev);
done:
	return retval;
}

/*                                                                         */

#ifdef	CONFIG_PM

/*                             */

/*                                        
                                                        
                                                                
                            
                                                        
 */

static bool usb_is_intel_switchable_ehci(struct pci_dev *pdev)
{
	return pdev->class == PCI_CLASS_SERIAL_USB_EHCI &&
		pdev->vendor == PCI_VENDOR_ID_INTEL &&
		(pdev->device == 0x1E26 ||
		 pdev->device == 0x8C2D ||
		 pdev->device == 0x8C26 ||
		 pdev->device == 0x9C26);
}

static void ehci_enable_xhci_companion(void)
{
	struct pci_dev		*companion = NULL;

	/*                                                            */
	for_each_pci_dev(companion) {
		if (!usb_is_intel_switchable_xhci(companion))
			continue;
		usb_enable_xhci_ports(companion);
		return;
	}
}

static int ehci_pci_resume(struct usb_hcd *hcd, bool hibernated)
{
	struct ehci_hcd		*ehci = hcd_to_ehci(hcd);
	struct pci_dev		*pdev = to_pci_dev(hcd->self.controller);

	/*                                                                    
                                                                        
                                                                 
                                                                      
   
                                                                  
                                                                     
                                                                     
            
   
                                                                        
                                                                     
                                                                         
                                                                       
                                   
  */
	if (usb_is_intel_switchable_ehci(pdev))
		ehci_enable_xhci_companion();

	if (ehci_resume(hcd, hibernated) != 0)
		(void) ehci_pci_reinit(ehci, pdev);
	return 0;
}

#else

#define ehci_suspend		NULL
#define ehci_pci_resume		NULL
#endif	/*           */

static struct hc_driver __read_mostly ehci_pci_hc_driver;

static const struct ehci_driver_overrides pci_overrides __initconst = {
	.reset =		ehci_pci_setup,
};

/*                                                                         */

/*                                                          */
static const struct pci_device_id pci_ids [] = { {
	/*                                    */
	PCI_DEVICE_CLASS(PCI_CLASS_SERIAL_USB_EHCI, ~0),
	.driver_data =	(unsigned long) &ehci_pci_hc_driver,
	}, {
	PCI_VDEVICE(STMICRO, PCI_DEVICE_ID_STMICRO_USB_HOST),
	.driver_data = (unsigned long) &ehci_pci_hc_driver,
	},
	{ /*                 */ }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

/*                                                          */
static struct pci_driver ehci_pci_driver = {
	.name =		(char *) hcd_name,
	.id_table =	pci_ids,

	.probe =	usb_hcd_pci_probe,
	.remove =	usb_hcd_pci_remove,
	.shutdown = 	usb_hcd_pci_shutdown,

#ifdef CONFIG_PM
	.driver =	{
		.pm =	&usb_hcd_pci_pm_ops
	},
#endif
};

static int __init ehci_pci_init(void)
{
	if (usb_disabled())
		return -ENODEV;

	pr_info("%s: " DRIVER_DESC "\n", hcd_name);

	ehci_init_driver(&ehci_pci_hc_driver, &pci_overrides);

	/*                                                          */
	ehci_pci_hc_driver.pci_suspend = ehci_suspend;
	ehci_pci_hc_driver.pci_resume = ehci_pci_resume;

	return pci_register_driver(&ehci_pci_driver);
}
module_init(ehci_pci_init);

static void __exit ehci_pci_cleanup(void)
{
	pci_unregister_driver(&ehci_pci_driver);
}
module_exit(ehci_pci_cleanup);

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_AUTHOR("David Brownell");
MODULE_AUTHOR("Alan Stern");
MODULE_LICENSE("GPL");
