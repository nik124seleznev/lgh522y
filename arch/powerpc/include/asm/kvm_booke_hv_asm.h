/*
 * Copyright 2010-2011 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 */

#ifndef ASM_KVM_BOOKE_HV_ASM_H
#define ASM_KVM_BOOKE_HV_ASM_H

#ifdef __ASSEMBLY__

/*
                                                      
                                                                  
                                                                
                           
  
              
                                       
                         
                        
                                                               
                   
                                            
                                            
  
                                                  
                                   
                                     
                                 
                                                               
                            
                                                      
                              
  
              
                                                       
                  
                      
                                               
                                               
                                          
  
                                          
                  
                      
                                               
                                               
                                               
  
                                        
                  
                      
                                                   
                                                   
                                     
  
                                                                           
 */
.macro DO_KVM intno srr1
#ifdef CONFIG_KVM_BOOKE_HV
BEGIN_FTR_SECTION
	mtocrf	0x80, r11	/*                                      */
	bf	3, 1975f
	b	kvmppc_handler_\intno\()_\srr1
1975:
END_FTR_SECTION_IFSET(CPU_FTR_EMB_HV)
#endif
.endm

#endif /*             */
#endif /*                        */
