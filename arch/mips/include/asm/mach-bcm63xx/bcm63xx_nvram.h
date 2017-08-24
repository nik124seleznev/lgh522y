#ifndef BCM63XX_NVRAM_H
#define BCM63XX_NVRAM_H

#include <linux/types.h>

/* 
                                           
                                    
  
                                                                      
                
 */
void bcm63xx_nvram_init(void *nvram);

/* 
                                                                       
  
                                                                     
                                                  
 */
u8 *bcm63xx_nvram_get_name(void);

/* 
                                                                        
                                           
  
                                                                          
  
                        
 */
int bcm63xx_nvram_get_mac_address(u8 *mac);

#endif /*                 */
