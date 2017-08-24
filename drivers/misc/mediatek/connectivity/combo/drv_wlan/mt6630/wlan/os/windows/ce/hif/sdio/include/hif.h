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
#include "SDCardDDK.h"
#if CFG_SDIO_PATHRU_MODE
#include <sdhcd.h>		/*                                  */
#endif

#if _PF_COLIBRI
#include "colibri.h"		/*                                 */
#endif
#if _PF_MT6516
#include "mt6516.h"		/*                                 */
#endif

#ifdef MT6620
#define SDNDIS_REG_PATH TEXT("\\Comm\\MT6620")
#endif

#include "bldver.h"

#define BLOCK_TRANSFER_LEN (512)

#ifdef __cplusplus
extern "C" {
	/*                 */
#include <ndis.h>
}
#endif
#if (CE_MAJOR_VER >= 4)
#define NDIS_SUCCESS(Status) ((NDIS_STATUS)(Status) == NDIS_STATUS_SUCCESS)
#endif
/*                                                                             
                                                
                                                                               
*/
#define MAX_ACTIVE_REG_PATH         256
#define NIC_INTERFACE_TYPE          NdisInterfaceInternal
#define NIC_ATTRIBUTE               NDIS_ATTRIBUTE_DESERIALIZE
#define NIC_DMA_MAPPED              0
#define NIC_MINIPORT_INT_REG        0
#ifdef X86_CPU
/* Please make sure the MCR you wrote will not take any effect.
 * MCR_MIBSDR (0x00C4) has confirm with DE.
 */* / TODO: yarco */
#define SDIO_X86_WORKAROUND_WRITE_MCR  0x0000
#endif
#if CFG_SDIO_PATHRU_MODE
#define SDIO_PATHRU_SHC_NAME    TEXT("SHC1:")
#define FILE_DEVICE_SDHC    (0x8CE7)	/*                                 */
#define _SDHC_CTL_CODE(_Function, _Method, _Access)  \
    CTL_CODE(FILE_DEVICE_SDHC, _Function, _Method, _Access)
#define IOCTL_SDHC_PATHRU _SDHC_CTL_CODE(0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif
/*                                                                             
                                                 
                                                                               
*/
#if CFG_SDIO_PATHRU_MODE
/*                                                                           
                                  
 */ typedef struct _GL_PATHRU_INFO_IN_T {
	DWORD dwEnable;		/*                                  */
	DWORD dwSlotNumber;	/*                                     */
	 VOID(*pIndicateSlotStateChange) (PSDCARD_HC_CONTEXT pHCContext,	/*                                       */
					  DWORD SlotNumber, SD_SLOT_EVENT Event, PVOID pvClient);
	PVOID pvClientContext;	/*                                             */
} GL_PATHRU_INFO_IN_T, *P_GL_PATHRU_INFO_IN_T;

/*                                                                            
                                  
 */
typedef struct _GL_PATHRU_INFO_OUT_T {
	PSDCARD_HC_CONTEXT pHcd;	/*                                                           */
	 VOID(*pIndicateSlotStateChange) (PSDCARD_HC_CONTEXT pHCContext,	/*                                     */
					  DWORD SlotNumber, SD_SLOT_EVENT Event);
} GL_PATHRU_INFO_OUT_T, *P_GL_PATHRU_INFO_OUT_T;

/*                                                                            
 */
typedef struct _GL_PATHRU_INFO_T {
	HANDLE hSHCDev;		/*               */
	TCHAR szSHCDevName[16];	/*          */
	BOOLEAN fgInitialized;	/*                                      */
	BOOLEAN fgEnabled;	/*                                  */
	DWORD dwSlotNumber;	/*                                 */
	PSDCARD_HC_CONTEXT pSHCContext;	/*                                                        */
	GL_PATHRU_INFO_IN_T rInfoIn;	/*                                    */
	GL_PATHRU_INFO_OUT_T rInfoOut;	/*                              */
	CRITICAL_SECTION rLock;	/*                                                                       */
} GL_PATHRU_INFO_T, *P_GL_PATHRU_INFO_T;
#endif				/*                      */

/*                                                                      
                        
 */
typedef struct _GL_HIF_INFO_T {
	SD_DEVICE_HANDLE hDevice;	/*                */
	P_GLUE_INFO_T prGlueInfo;	/*                     */
	UCHAR Function;		/*                     */
	PWSTR pRegPath;		/*                     */
	ULONG Errors;		/*             */
	SD_CARD_RCA RCA;	/*                       */
	SD_HOST_BLOCK_CAPABILITY sdHostBlockCap;
	ULONG WBlkBitSize;
	WCHAR ActivePath[MAX_ACTIVE_REG_PATH];

#if CFG_SDIO_PATHRU_MODE
	BOOLEAN fgSDIOFastPathEnable;	/*                                                         */
	GL_PATHRU_INFO_T rPathruInfo;	/*                                     */
#endif

} GL_HIF_INFO_T, *P_GL_HIF_INFO_T;


/*                                                                             
                                                  
                                                                               
*/

/*                                                                             
                                                   
                                                                               
*/

/*                                                                             
                                             
                                                                               
*/
#ifdef CFG_HAVE_PLATFORM_INIT
#define sdioBusDeinit(prGlueInfo) \
    platformBusDeinit(prGlueInfo)
#define sdioSetPowerState(prGlueInfo, ePowerMode) \
    platformSetPowerState(prGlueInfo, ePowerMode)
#else
/*                                                                           
                    
*/
#define platformBusInit(prGlueInfo) ((BOOLEAN)TRUE)
#define sdioBusDeinit(prGlueInfo)
#define sdioSetPowerState(prGlueInfo, ePowerMode)
#endif

/*                                                                             
                                                             
                                                                               
*/

#if CFG_SDIO_PATHRU_MODE
VOID sdioInitPathruMode(IN P_GLUE_INFO_T prGlueInfo);

VOID sdioDeinitPathruMode(IN P_GLUE_INFO_T prGlueInfo);

BOOLEAN sdioEnablePathruMode(IN P_GLUE_INFO_T prGlueInfo, IN BOOLEAN fgEnable);

#endif

#endif				/*        */
