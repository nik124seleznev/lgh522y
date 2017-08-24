/*
                         
  
                                    
 */

#ifndef _ASM_X86_VGA_H
#define _ASM_X86_VGA_H

/*
                                                        
                                                        
 */

#define VGA_MAP_MEM(x, s) (unsigned long)phys_to_virt(x)

#define vga_readb(x) (*(x))
#define vga_writeb(x, y) (*(y) = (x))

#ifdef CONFIG_FB_EFI
#define __ARCH_HAS_VGA_DEFAULT_DEVICE
extern struct pci_dev *vga_default_device(void);
extern void vga_set_default_device(struct pci_dev *pdev);
#endif

#endif /*                */
