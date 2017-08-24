/*
                                                                                                         
*/

/*                     
                                                      

*/



/*
                   
  
                      
                                                             
                                                  
                                                                      
  
                   
  
                                                                               
  
                         
                                                
                                   
                                                                           
                    
*/

#ifndef _MT6516_H
#define _MT6516_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "gl_os.h"

/*                                                                              
                                                
                                                                                
*/
#define SDIO_DEVICE_NAME (L"SHC1:")

/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define CFG_HAVE_PLATFORM_INIT

/*                                                                              
                                                             
                                                                                
*/

BOOLEAN platformBusInit(IN P_GLUE_INFO_T prGlueInfo);

VOID platformBusDeinit(IN P_GLUE_INFO_T prGlueInfo);

VOID platformSetPowerState(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 ePowerMode);


/*                                                                              
                                                
                                                                                
*/

#endif				/*           */
