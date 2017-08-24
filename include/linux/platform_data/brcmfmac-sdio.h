/*
 * Copyright (c) 2013 Broadcom Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _LINUX_BRCMFMAC_PLATFORM_H
#define _LINUX_BRCMFMAC_PLATFORM_H

/*
                                                                             
                                                                       
                                                                 
  
                                                        
  
  
                              


                                   
 
 

                                    
 
 

                                
 
 

                                                                 
                                
                                  
                          
  

                                                 
                                    
                             
                                          
  

                                     
 
                                              
                                                                   
                                                     
                                  
                                            
 
  
  
                                                                            
                                                                     
                                                                         
                                                                            
                                                                            
                                                                             
                                         
  
                                                                               
                                                                               
                                    
  
                                           
  
                                                                          
                                                                         
                                                                       
                              
  
                                                                          
                                                                         
                                                                       
                                                                             
                          
  
                                                                           
                                                           
  
                                                                         
                                                                              
                                                                          
                                                               
  
                                                                          
                                                                                
                                                                                
                                                                                
                                                                           
                            
  
                                                                              
                                                                                
                                                                           
                                                                           
                                                                      
 */

#define BRCMFMAC_SDIO_PDATA_NAME	"brcmfmac_sdio"

struct brcmfmac_sdio_platform_data {
	unsigned int drive_strength;
	bool oob_irq_supported;
	unsigned int oob_irq_nr;
	unsigned long oob_irq_flags;
	void (*power_on)(void);
	void (*power_off)(void);
	void (*reset)(void);
};

#endif /*                            */
