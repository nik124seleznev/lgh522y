#ifndef _ASM_X86_BOOTPARAM_UTILS_H
#define _ASM_X86_BOOTPARAM_UTILS_H

#include <asm/bootparam.h>

/*
                                                            
                                                  
 */

/*
                                                                   
                                                                    
                                                                    
                                                                          
  
                                                                      
                                                        
 */
static void sanitize_boot_params(struct boot_params *boot_params)
{
	/* 
                                                             
                                                          
                                                
                                                         
                                                             
                                                             
                        
   
                                                              
                                                              
                                                          
                   
  */
	if (boot_params->sentinel) {
		/*                                                          */
		memset(&boot_params->ext_ramdisk_image, 0,
		       (char *)&boot_params->efi_info -
			(char *)&boot_params->ext_ramdisk_image);
		memset(&boot_params->kbd_status, 0,
		       (char *)&boot_params->hdr -
		       (char *)&boot_params->kbd_status);
		memset(&boot_params->_pad7[0], 0,
		       (char *)&boot_params->edd_mbr_sig_buffer[0] -
			(char *)&boot_params->_pad7[0]);
		memset(&boot_params->_pad8[0], 0,
		       (char *)&boot_params->eddbuf[0] -
			(char *)&boot_params->_pad8[0]);
		memset(&boot_params->_pad9[0], 0, sizeof(boot_params->_pad9));
	}
}

#endif /*                            */
