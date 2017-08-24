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
#include "gl_os.h"
LINT_EXT_HEADER_BEGIN 
#include <ceddk.h>
    LINT_EXT_HEADER_END
#include "hif.h"
#include "mt6516.h"
/*                                                                             
                                                
                                                                               
*/
/*                                                                             
                                                 
                                                                               
*/
/*                                                                             
                                                  
                                                                               
*/
    HANDLE hSdio = INVALID_HANDLE_VALUE;

/*                                                                             
                                                   
                                                                               
*/

/*                                                                             
                                             
                                                                               
*/

/*                                                                             
                                                             
                                                                               
*/

/*                                                                             
                                                
                                                                               
*/

BOOLEAN platformBusInit(IN P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);

	if (INVALID_HANDLE_VALUE == hSdio) {
		hSdio = CreateFile(SDIO_DEVICE_NAME, (GENERIC_READ | GENERIC_WRITE), 0,	/*                                           */
				   NULL,	/*                   */
				   OPEN_EXISTING,	/*                                */
				   FILE_ATTRIBUTE_NORMAL,	/*        */
				   NULL	/*                                    */
		    );
	}

	if (INVALID_HANDLE_VALUE == hSdio) {
		ERRORLOG(("Fail to open SDIO device:%s\r\n", SDIO_DEVICE_NAME));
		return FALSE;
	}

	return TRUE;
}

VOID platformBusDeinit(IN P_GLUE_INFO_T prGlueInfo)
{
	ASSERT(prGlueInfo);

	if (INVALID_HANDLE_VALUE != hSdio) {
		CloseHandle(hSdio);
		hSdio = INVALID_HANDLE_VALUE;
	}
}

VOID platformSetPowerState(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 ePowerMode)
{
	CEDEVICE_POWER_STATE cPowerState;

	ASSERT(prGlueInfo);

	if (ParamDeviceStateD0 == ePowerMode) {
		cPowerState = D0;
	} else if (ParamDeviceStateD3 == ePowerMode) {
		cPowerState = D4;
	} else {
		ERRORLOG(("platformSetPowerState fail, unsupported ePowerMode:%x\n", ePowerMode));
		return;
	}

	DeviceIoControl(hSdio,
			IOCTL_POWER_SET, NULL, 0, &cPowerState, sizeof(cPowerState), NULL, NULL);
	return;
}
