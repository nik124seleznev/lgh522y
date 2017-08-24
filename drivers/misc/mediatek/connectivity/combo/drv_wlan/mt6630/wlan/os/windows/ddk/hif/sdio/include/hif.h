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
#include <initguid.h>
#include <ntddsd.h>
#include <ntddk.h>
#include <wdm.h>

#ifdef MT5921
#define SDNDIS_REG_PATH TEXT("\\Comm\\MT5921")
#endif
#ifdef MT5922
#define SDNDIS_REG_PATH TEXT("\\Comm\\MT5922")
#endif

#ifdef __cplusplus
extern "C" {
	/*                 */
#include <ndis.h>
}
#endif
/*                                                                              
                                                
                                                                                
*/
#define MAX_ACTIVE_REG_PATH         256
#define NIC_INTERFACE_TYPE          NdisInterfaceInternal
#define NIC_ATTRIBUTE               (NDIS_ATTRIBUTE_DESERIALIZE | NDIS_ATTRIBUTE_SURPRISE_REMOVE_OK)
#define NIC_DMA_MAPPED              0
#define NIC_MINIPORT_INT_REG        0
#define REQ_FLAG_HALT               (0x01)
#define REQ_FLAG_INT                (0x02)
#define REQ_FLAG_OID                (0x04)
#define REQ_FLAG_TIMER              (0x08)
#define REQ_FLAG_RESET              (0x10)
#define BLOCK_TRANSFER_LEN          (512)
/* Please make sure the MCR you wrote will not take any effect.
 * MCR_MIBSDR (0x00C4) has confirm with DE.
 */
#define SDIO_X86_WORKAROUND_WRITE_MCR   0x00C4
/*                                                                              
                                                 
                                                                                
*/ typedef struct _DEVICE_EXTENSION {
	/*                       */
	PDEVICE_OBJECT PhysicalDeviceObject;

	/*                          */
	PDEVICE_OBJECT FunctionalDeviceObject;

	/*                                                 */
	PDEVICE_OBJECT NextLowerDriverObject;

	/*              */
	SDBUS_INTERFACE_STANDARD BusInterface;

	/*                      */
	UCHAR FunctionNumber;
	BOOLEAN busInited;

	BOOLEAN fgIsSdioBlockModeEnabled;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

/*                                                      
                                  
 */
typedef struct _GL_HIF_INFO_T {
	/*                                          */
	DEVICE_EXTENSION dx;

	HANDLE rOidThreadHandle;
	PKTHREAD prOidThread;
	KEVENT rOidReqEvent;

	UINT_32 u4ReqFlag;	/*              */

	UINT_32 u4BusClock;
	UINT_32 u4BusWidth;
	UINT_32 u4BlockLength;

	/*            */
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

/*                                                                              
                                                
                                                                                
*/

#endif				/*        */
