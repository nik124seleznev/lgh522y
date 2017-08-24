/*
                                                                                                   
*/

/*                    
                                                          

*/



/*
                     
  
                      
                                                             
                                                  
                                                                      
  
                   
  
                                                                               
  
                         
                                                
                                   
                                                                           
                    
  
*/

/*                                                                              
                                                      
                                                                                
*/
#include "gl_os.h"

/*                                                                              
                                            
                                                                                
*/
#define MAX_NUMBER_OF_ADAPTERS                  8


/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                              
                                                                                
*/

/*                                                                              
                                               
                                                                                
*/

#define LOADER_INSTANCE_KEY     TEXT("Instance")


#if 0
#define MAX_MINIPORT_NAME_PATH                  256

    /*                                */
typedef struct _MINIPORT_INSTANCE_INFO {
	WCHAR MiniportName[MAX_MINIPORT_NAME_PATH];
	WCHAR MiniportInstance[MAX_MINIPORT_NAME_PATH];
	WCHAR RegPath[MAX_MINIPORT_NAME_PATH];
	WCHAR ActiveKeyPath[MAX_MINIPORT_NAME_PATH];
	ULONG InstanceNumber;
} MINIPORT_INSTANCE_INFO, *PMINIPORT_INSTANCE_INFO;
#endif

BOOL AllocatedInstance[MAX_NUMBER_OF_ADAPTERS];

CRITICAL_SECTION LoaderCriticalSection;

BOOL devicePowerDown;

/*                                                                              
                                         
                                                                                
*/
#define SdDbgPrint(_Module, _Fmt)       DbgPrintZo(_Module, _Fmt)

/*                                                                              
                                                        
                                                                                
*/
BOOL LoaderEntry(HINSTANCE hInstance, ULONG Reason, LPVOID pReserved);

BOOL LoadMiniport(PMINIPORT_INSTANCE_INFO pInstance);

VOID UnloadMiniport(PMINIPORT_INSTANCE_INFO pInstance);

/*                                                                              
                                            
                                                                                
*/

/*                                                                            */
/* 
                   
 
                                                
                                                      
                       
 
                       
 
                                                                              */
BOOL WINAPI DllEntry(HINSTANCE hInstance, INT Reason, LPVOID Reserved)
{
	if (Reason == DLL_PROCESS_ATTACH) {
		/*                                  */
		CREATE_LOG_FILE();

		/*                                       */
		SD_DEBUG_ZONE_REGISTER(hInstance, SDNDIS_REG_PATH);
#ifdef MT6620
		SDInitializeMemoryTagging(2, TEXT("MT6620SD"));
#endif
		DisableThreadLibraryCalls((HMODULE) hInstance);
	}

	if (Reason == DLL_PROCESS_DETACH) {
#if DEBUG
		SDCheckMemoryTags(NULL);
#endif

		SDDeleteMemoryTagging();

		SdDbgPrint(SDCARD_ZONE_INIT,
			   (TEXT("SDNdis: DllEntry - Reason == DLL_PROCESS_DETACH\n")));
	}

	return LoaderEntry(hInstance, Reason, Reserved);
}				/*          */

/*                                                                            */
/* 
                    
 
                                                      
                                                
                                
 
                       
 
                                                     
 
                                                                              */
BOOL LoaderEntry(HINSTANCE hInstance, ULONG Reason, LPVOID pReserved)
{
	if (Reason == DLL_PROCESS_ATTACH) {
		DEBUGREGISTER(hInstance);
		InitializeCriticalSection(&LoaderCriticalSection);
		memset(&AllocatedInstance, 0, sizeof(AllocatedInstance));
	}

	if (Reason == DLL_PROCESS_DETACH) {
		DeleteCriticalSection(&LoaderCriticalSection);
	}

	return (TRUE);
}				/*             */

/*                                                                            */
/* 
                                            
 
                                                     
 
                       
 
                                                                              */
BOOL LoadMiniport(PMINIPORT_INSTANCE_INFO pInstance)
{
#define STRING_BUF_SZ       128
#define INSTANCE_NAME_SZ    32
#define INSTANCE_NUMBER_SZ  10

	HKEY hKey;		/*              */
	DWORD win32Status;	/*        */
	DWORD dataSize;		/*                      */
	WCHAR stringBuff[STRING_BUF_SZ];	/*                */
	WCHAR instanceKey[INSTANCE_NAME_SZ];	/*                */
	WCHAR instanceNumber[INSTANCE_NUMBER_SZ];	/*                  */
	WCHAR *token;		/*            */
	NDIS_STATUS NdisStatus;	/*              */

	/*                                          */
	if ((win32Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					pInstance->RegPath, 0, 0, &hKey)) != ERROR_SUCCESS) {
		SdDbgPrint(SDCARD_ZONE_ERROR, (TEXT("SDNDISLDR:Failed to open path %s; %d\n"),
					       pInstance->RegPath, win32Status));
		return FALSE;
	}

	dataSize = sizeof(stringBuff);

	/*                           */
	wcscpy(instanceKey, LOADER_INSTANCE_KEY);
	_ultow(pInstance->InstanceNumber, instanceNumber, INSTANCE_NUMBER_SZ);
	wcscat(instanceKey, instanceNumber);

	/*                                                     */
	if (RegQueryValueEx(hKey,
			    instanceKey,
			    0, NULL, (PUCHAR) stringBuff, &dataSize) != ERROR_SUCCESS) {

		SdDbgPrint(SDCARD_ZONE_ERROR,
			   (TEXT("SDNDISLDR: Failed to get the instance key : %d\n"),
			    instanceKey));
		RegCloseKey(hKey);
		return FALSE;
	}

	RegCloseKey(hKey);

	SdDbgPrint(SDCARD_ZONE_INIT,
		   (TEXT("SDNDISLDR: Tokenizing instance information: %s\n"), stringBuff));

	/*                                                            
                                         
  */
	token = wcstok(stringBuff, TEXT(":"));

	if (token != NULL) {

		wcscpy(pInstance->MiniportName, token);

		/*                         */
		token = wcstok(NULL, TEXT(":"));

		if (token != NULL) {
			wcscpy(pInstance->MiniportInstance, token);
		} else {
			SdDbgPrint(SDCARD_ZONE_ERROR,
				   (TEXT("SDNDISLDR: Failed to get miniport instance\n")));
			return FALSE;
		}
	} else {
		SdDbgPrint(SDCARD_ZONE_ERROR, (TEXT("SDNDISLDR: Failed to get miniport name\n")));
		return FALSE;
	}

	/*                                                             
                     */
	wcscpy(stringBuff, TEXT("\\Comm\\"));
	wcscat(stringBuff, pInstance->MiniportInstance);
	wcscat(stringBuff, TEXT("\\Parms"));

	SdDbgPrint(SDCARD_ZONE_INIT, (TEXT("SDNDISLDR: Miniport instance path %s\n"), stringBuff));

	if ((win32Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					stringBuff, 0, 0, &hKey)) != ERROR_SUCCESS) {
		SdDbgPrint(SDCARD_ZONE_ERROR, (TEXT("SDNDISLDR:Failed to open path %s; %d\n"),
					       stringBuff, win32Status));
		return FALSE;
	}

	/*                                    */
	RegDeleteValue(hKey, TEXT("ActivePath"));

	SdDbgPrint(SDCARD_ZONE_INIT,
		   (TEXT("SDNDISLDR: Storing ActiveKey Path %s\n"), pInstance->ActiveKeyPath));

	/*                                                                  
                                               */
	if (RegSetValueEx(hKey,
			  TEXT("ActivePath"),
			  0,
			  REG_SZ,
			  (PUCHAR) pInstance->ActiveKeyPath,
			  ((sizeof(WCHAR)) * (wcslen(pInstance->ActiveKeyPath) + 1))) !=
	    ERROR_SUCCESS) {
		SdDbgPrint(SDCARD_ZONE_ERROR, (TEXT("SDNDISLDR: Failed to set ActiveKey path\n")));
		RegCloseKey(hKey);
		return FALSE;
	}

	/*               */
	RegCloseKey(hKey);

	/*                                                                 */
	wcscpy(stringBuff, TEXT("\\Comm\\"));
	wcscat(stringBuff, pInstance->MiniportName);
	SdDbgPrint(SDCARD_ZONE_INIT, (TEXT("SDNDISLDR: Miniport name path %s\n"), stringBuff));

	if ((win32Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					stringBuff, 0, 0, &hKey)) != ERROR_SUCCESS) {
		SdDbgPrint(SDCARD_ZONE_ERROR, (TEXT("SDNDISLDR:Failed to open path %s; %d\n"),
					       stringBuff, win32Status));
		return FALSE;
	}

	/*                                                       */
	wcscpy(stringBuff, TEXT("NDIS"));
	if (RegSetValueEx(hKey,
			  TEXT("Group"),
			  0,
			  REG_SZ,
			  (PUCHAR) stringBuff,
			  ((sizeof(WCHAR)) * (wcslen(stringBuff) + 1))) != ERROR_SUCCESS) {
		SdDbgPrint(SDCARD_ZONE_ERROR, (TEXT("SDNDISLDR: Failed to set Group entry\n")));
		RegCloseKey(hKey);
		return FALSE;
	}

	/*               */
	RegCloseKey(hKey);

	/*                      */
	NdisRegisterAdapter(&NdisStatus, pInstance->MiniportName, pInstance->MiniportInstance);

	if (!NDIS_SUCCESS(NdisStatus)) {
		SdDbgPrint(SDCARD_ZONE_ERROR,
			   (TEXT("SDNDISLDR: Failed to register the adapter\n")));
		return FALSE;
	}

	/*                                                                               */
	wcscpy(stringBuff, TEXT("\\Comm\\"));
	wcscat(stringBuff, pInstance->MiniportInstance);
	wcscat(stringBuff, TEXT("\\Parms"));


	if ((win32Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					stringBuff, 0, 0, &hKey)) != ERROR_SUCCESS) {
		return FALSE;
	}
	RegDeleteValue(hKey, TEXT("ActivePath"));
	RegCloseKey(hKey);

	/*                                                                  */
	wcscpy(stringBuff, TEXT("\\Comm\\"));
	wcscat(stringBuff, pInstance->MiniportName);
	if ((win32Status = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
					stringBuff, 0, 0, &hKey)) != ERROR_SUCCESS) {
		return FALSE;
	}
	RegDeleteValue(hKey, TEXT("Group"));
	RegCloseKey(hKey);

	return TRUE;

}				/*              */

/*                                                                            */
/* 
                            
 
                                              
 
              
 
                                                                              */
VOID UnloadMiniport(PMINIPORT_INSTANCE_INFO pInstance)
{
	NDIS_STATUS NdisStatus;

	SdDbgPrint(SDCARD_ZONE_INIT,
		   (TEXT("SDNDISLDR: Unloading Miniport Instance %s\n"),
		    pInstance->MiniportInstance));

	NdisDeregisterAdapter(&NdisStatus, pInstance->MiniportInstance);

	SdDbgPrint(SDCARD_ZONE_INIT, (TEXT("SDNDISLDR: Miniport Unloaded 0x%08X\n"), NdisStatus));
}				/*                */

/*                                                                            */
/* 
                                               
 
                                                                  
 
                       
 
                                                                              */
BOOL NDL_Deinit(DWORD hDeviceContext)
{
	PMINIPORT_INSTANCE_INFO pInstance;	/*                       */

	SdDbgPrint(SDCARD_ZONE_INIT, (TEXT("SDNDISLDR: +NDL_Deinit\n")));

	pInstance = (PMINIPORT_INSTANCE_INFO) hDeviceContext;

	/*                     */
	UnloadMiniport(pInstance);

	EnterCriticalSection(&LoaderCriticalSection);

	/*                          */
	AllocatedInstance[pInstance->InstanceNumber] = FALSE;

	LeaveCriticalSection(&LoaderCriticalSection);

	LocalFree(pInstance);

	SdDbgPrint(SDCARD_ZONE_INIT, (TEXT("SDNDISLDR: -NDL_Deinit\n")));

	return TRUE;
}				/*            */

/*                                                                            */
/* 
                             
 
                                                 
 
                                    
 
                                                                              */
DWORD NDL_Init(DWORD dwContext)
{
	PMINIPORT_INSTANCE_INFO pInstance;	/*                             */
	ULONG ii;


	SdDbgPrint(SDCARD_ZONE_INIT, (TEXT("SDNDISLDR: +NDL_Init\n")));

	pInstance = (PMINIPORT_INSTANCE_INFO)
	    LocalAlloc(LPTR, sizeof(MINIPORT_INSTANCE_INFO));

	if (pInstance == NULL) {
		SdDbgPrint(SDCARD_ZONE_ERROR,
			   (TEXT("SDNDISLDR: Failed to allocate device info\n")));
		SdDbgPrint(SDCARD_ZONE_INIT, (TEXT("SDNDISLDR: -NDL_Init\n")));
		return 0;
	}

	/*                                                                            
                                   
  */
	wcscpy(pInstance->ActiveKeyPath, (PWCHAR) dwContext);

	if (SDGetRegPathFromInitContext((PWCHAR) dwContext,
					pInstance->RegPath,
					sizeof(pInstance->RegPath)) != ERROR_SUCCESS) {
		SdDbgPrint(SDCARD_ZONE_ERROR, (TEXT("SDNDISLDR: Failed to get reg path\n")));
		LocalFree(pInstance);
		return 0;
	}

	EnterCriticalSection(&LoaderCriticalSection);

	/*                                                          */
	for (ii = 0; ii < MAX_NUMBER_OF_ADAPTERS; ii++) {
		if (AllocatedInstance[ii] == FALSE) {
			/*                                  */
			AllocatedInstance[ii] = TRUE;
			/*                     */
			pInstance->InstanceNumber = ii;
			break;
		}
	}

	LeaveCriticalSection(&LoaderCriticalSection);

	if (ii >= MAX_NUMBER_OF_ADAPTERS) {
		SdDbgPrint(SDCARD_ZONE_ERROR, (TEXT("SDNDISLDR: Max instances exceeded\n")));
		LocalFree(pInstance);
		return 0;
	}


	if (!LoadMiniport(pInstance)) {
		LocalFree(pInstance);
		SdDbgPrint(SDCARD_ZONE_ERROR, (TEXT("SDNDISLDR: LoadMiniport fail\n")));
		return 0;
	}

	SdDbgPrint(SDCARD_ZONE_INIT, (TEXT("SDNDISLDR: -NDL_Init\n")));

	return (DWORD) pInstance;
}				/*          */

/*                                                                            */
/* 
                                    
 
                                                          
                                      
                                                      
                                                      
                                                       
                                                       
                                                     
 
                                 
 
                
 
                                                                              */
BOOL
NDL_IOControl(DWORD Handle,
	      DWORD IoctlCode,
	      PBYTE pInBuf, DWORD InBufSize, PBYTE pOutBuf, DWORD OutBufSize, PDWORD pBytesReturned)
{
	return FALSE;

}				/*               */

/*                                                                            */
/* 
                             
 
                                                            
                                              
                                                  
 
                           
 
                
 
                                                                              */
DWORD NDL_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	/*                          */
	return hDeviceContext;
}				/*          */

/*                                                                            */
/* 
                                                      
 
                                                            
 
              
 
                           
 
                                                                              */
VOID NDL_PowerDown(DWORD hDeviceContext)
{
	devicePowerDown = TRUE;
}				/*               */

/*                                                                            */
/* 
                                                                
 
                                                            
 
              
 
                           
 
                                                                              */
VOID NDL_PowerUp(DWORD hDeviceContext)
{
	return;
}				/*             */

/*                                                                            */
/* 
                             
 
                                                     
                                             
                                                    
 
                                
 
                
 
                                                                              */
DWORD NDL_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	return 0;
}				/*          */

/*                                                                            */
/* 
                             
 
                                                     
                                              
                                            
 
                                
 
                
 
                                                                              */
DWORD NDL_Seek(DWORD hOpenContext, LONG Amount, DWORD Type)
{
	return 0;
}				/*          */

/*                                                                            */
/* 
                              
 
                                                     
                                             
                                                    
 
                                
 
                
 
                                                                              */
DWORD NDL_Write(DWORD hOpenContext, LPCVOID pBuffer, DWORD Count)
{
	return 0;
}				/*           */

/*                                                                            */
/* 
                              
 
                                                              
 
              
 
                
 
                                                                              */
BOOL NDL_Close(DWORD hOpenContext)
{
	return TRUE;
}				/*           */
