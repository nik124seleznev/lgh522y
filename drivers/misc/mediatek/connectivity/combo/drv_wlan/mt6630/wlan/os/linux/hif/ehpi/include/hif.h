/*
                                                                                                 
*/

/*                 
                                                                      

                                                              
*/



/*
                
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                                
                                                                                               
                                                                                                       
                                                                               
  
                   
                                                                                
                              
  
*/

#ifndef _HIF_H
#define _HIF_H

/*                                                                              
                                                     
                                                                                
*/


/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/

/*                                                                      
                        
 */
typedef struct _GL_HIF_INFO_T {
	void __iomem *mcr_addr_base;
	void __iomem *mcr_data_base;
	BOOLEAN fgIntReadClear;
	BOOLEAN fgMbxReadClear;
} GL_HIF_INFO_T, *P_GL_HIF_INFO_T;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

WLAN_STATUS glRegisterBus(probe_card pfProbe, remove_card pfRemove);

VOID glUnregisterBus(remove_card pfRemove);

VOID glSetHifInfo(P_GLUE_INFO_T prGlueInfo, ULONG ulCookie);

VOID glClearHifInfo(P_GLUE_INFO_T prGlueInfo);

BOOL glBusInit(PVOID pvData);

VOID glBusRelease(PVOID pData);

INT_32 glBusSetIrq(PVOID pvData, PVOID pfnIsr, PVOID pvCookie);

VOID glBusFreeIrq(PVOID pvData, PVOID pvCookie);

VOID glSetPowerState(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 ePowerMode);

/*                                                                              
                                                
                                                                                
*/
#endif				/*        */
