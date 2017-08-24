/*
                                                                                           
*/

/*                 
                                                                       
 */



/*
                  
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                                                                                                   
                                                                                                             
  
                   
  
                                                                               
  
                         
                                                
                                   
                                                                           
                              
                                                                           
                                                       
                                                                           
                    
  
*/

/*                                                                              
                                                      
                                                                                
*/
#include "gl_os.h"
#include "gl_kal.h"

/*                                                                              
                                            
                                                                                
*/
#define MAX_RECORD_NUM      128
#define MAX_STR_LEN         255
#define MAX_STR_BUF_SIZE    (MAX_STR_LEN + 1)

#define DEBUG_FILE_NAME     "\\temp\\mt_log.txt"

/*                                                                              
                                             
                                                                                
*/
typedef struct _DEBUG_MSG_FORMAT {
	UINT_32 id;
	UINT_32 sysTime;
	CHAR dbgStr[MAX_STR_BUF_SIZE];
} DEBUG_MSG_FORMAT, *PDEBUG_MSG_FORMAT;

typedef struct _DEBUG_MSG_POOL {
	UINT_32 currentIndex;
	DEBUG_MSG_FORMAT dbgMsg[1];
} DEBUG_MSG_POOL, *PDEBUG_MSG_POOL;

/*                                                                              
                                              
                                                                                
*/

/*                                                                              
                                               
                                                                                
*/
#if DBG
#if LOG_METHOD == MSG_DEBUG_MSG
static UINT_32 g_logId;
static HANDLE hMapFile;
static PDEBUG_MSG_POOL pBuf;

TCHAR szName[] = TEXT("MyFileMappingObject");
#endif
#endif
/*                                                                              
                                         
                                                                                
*/

/*                                                                              
                                                        
                                                                                
*/

/*                                                                              
                                            
                                                                                
*/
#if DBG
#if LOG_METHOD == MSG_DEBUG_MSG
/*                                                                            */
/* 
                                                     
 
             
 
                                                       
                                                  
                                       
 
*/
/*                                                                            */
NDIS_STATUS dbgFileCreate(VOID)
{
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,
				     NULL,
				     PAGE_READWRITE,
				     0,
				     MAX_RECORD_NUM * sizeof(DEBUG_MSG_FORMAT) +
				     sizeof(DEBUG_MSG_POOL), szName);

	if (hMapFile == NULL) {
		TCHAR buf[256];

		_stprintf(buf, TEXT("CreateFileMapping Error. Code:%d\n"), GetLastError());
		MessageBox(NULL, buf, TEXT("failed."), MB_OK);
		return 1;
	}

	pBuf = (PDEBUG_MSG_POOL) MapViewOfFile(hMapFile,	/*                      */
					       FILE_MAP_ALL_ACCESS,	/*                       */
					       0,
					       0,
					       MAX_RECORD_NUM * sizeof(DEBUG_MSG_FORMAT) +
					       sizeof(DEBUG_MSG_POOL));

	if (pBuf == NULL) {
		TCHAR buf[256];

		_stprintf(buf, TEXT("MapViewOfFile Error. Code:%d\n"), GetLastError());
		MessageBox(NULL, buf, TEXT("MapViewOfFile"), MB_OK);
		return 2;
	}

	pBuf->currentIndex = 0;
	return 0;
}				/*               */

/*                                                                            */
/* 
                            
 
             
 
              
*/
/*                                                                            */
VOID closeLog(void)
{
	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);
}				/*          */

/*                                                                            */
/* 
                                                               
 
                                                
                                        
 
                                                             
                                                        
*/
/*                                                                            */
NDIS_STATUS dbgLogWr(IN PINT_8 debugStr, IN ...
    )
{
	va_list paramList;
	PINT_8 buf_p = NULL;
	UINT_32 systemUpTime;

	if (pBuf == NULL) {
		return NDIS_STATUS_FAILURE;
	}

	/*                                          */
	buf_p = pBuf->dbgMsg[pBuf->currentIndex].dbgStr;

	va_start(paramList, debugStr);
	vsprintf(buf_p, debugStr, paramList);	/*                               */

	/*                 */
	NdisGetSystemUpTime(&systemUpTime);
	pBuf->dbgMsg[pBuf->currentIndex].sysTime = systemUpTime;
	pBuf->dbgMsg[pBuf->currentIndex].id = g_logId;

	g_logId++;
	pBuf->currentIndex++;
	if (pBuf->currentIndex >= MAX_RECORD_NUM) {
		pBuf->currentIndex = (pBuf->currentIndex) % MAX_RECORD_NUM;
	}

	return NDIS_STATUS_SUCCESS;

}				/*          */

/*                                                                            */
/* 
                                                               
                            
 
                                                
                                        
 
                                                             
                                                        
*/
/*                                                                            */
NDIS_STATUS dbgLogWr1(IN PINT_8 debugStr, IN ...
    )
{
	PINT_8 buf_p = NULL;
	UINT_32 systemUpTime;
	va_list paramList;

	if (pBuf == NULL) {
		return NDIS_STATUS_FAILURE;
	}

	/*                                          */
	buf_p = pBuf->dbgMsg[pBuf->currentIndex].dbgStr;

	va_start(paramList, debugStr);
	vsprintf(buf_p, debugStr, paramList);	/*                               */

	/*                 */
	NdisGetSystemUpTime(&systemUpTime);
	pBuf->dbgMsg[pBuf->currentIndex].sysTime = systemUpTime;
	pBuf->dbgMsg[pBuf->currentIndex].id = g_logId;

	g_logId++;
	pBuf->currentIndex++;
	if (pBuf->currentIndex >= MAX_RECORD_NUM) {
		pBuf->currentIndex = (pBuf->currentIndex) % MAX_RECORD_NUM;
	}

	return NDIS_STATUS_SUCCESS;
}				/*           */

#elif LOG_METHOD == FILE_DEBUG_MSG
/*                                                                            */
/* 
                                                     
 
             
 
                                                                         
                                                                    
*/
/*                                                                            */
NDIS_STATUS dbgFileCreate(VOID)
{
	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	NDIS_STATUS status = NDIS_STATUS_SUCCESS;

	/*                                            */
	FileHandle = CreateFile(TEXT(DEBUG_FILE_NAME), GENERIC_WRITE, 0,	/*            */
				NULL,	/*                            */
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	/*                                            */
	if (FileHandle == INVALID_HANDLE_VALUE) {
		FileHandle = CreateFile(TEXT(DEBUG_FILE_NAME), GENERIC_WRITE, 0,	/*            */
					NULL,	/*                            */
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (FileHandle == INVALID_HANDLE_VALUE) {
			status = NDIS_STATUS_FAILURE;
		}
	}

	if (FileHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(FileHandle);
	}

	return status;

}				/*               */

/*                                                                            */
/* 
                                                                              
 
                                                
                                        
 
                                                             
                                                        
*/
/*                                                                            */
NDIS_STATUS dbgLogWr(IN PINT_8 debugStr, IN ...
    )
{
#define TMP_BUF_LEN  256

	int strLen;
	DWORD BytesWritten;
	PINT_8 buf_p = NULL;
	HANDLE FileHandle = NULL;
	va_list paramList;
	UINT_32 systemUpTime;

	/*                   */
	FileHandle = CreateFile(TEXT(DEBUG_FILE_NAME), GENERIC_WRITE, 0,	/*            */
				NULL,	/*                            */
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (FileHandle == INVALID_HANDLE_VALUE) {
		return NDIS_STATUS_FAILURE;
	}

	/*                           */
	buf_p = (PUINT_8) kalMemAlloc(TMP_BUF_LEN, VIR_MEM_TYPE);
	if (buf_p == NULL) {
		CloseHandle(FileHandle);
		return NDIS_STATUS_FAILURE;
	}

	/*                 */
	NdisGetSystemUpTime(&systemUpTime);

	/*                                          */
	kalMemZero(buf_p, TMP_BUF_LEN);
	va_start(paramList, debugStr);
	sprintf(buf_p, "%10d ", systemUpTime);

	/*                               */
	_vsnprintf(buf_p + 11, TMP_BUF_LEN - 12, debugStr, paramList);

	strLen = strlen(buf_p);

	/*                               */
	SetFilePointer(FileHandle, 0, 0, FILE_END);
	WriteFile(FileHandle, buf_p, strLen, &BytesWritten, NULL);

	CloseHandle(FileHandle);
	kalMemFree(buf_p, VIR_MEM_TYPE, TMP_BUF_LEN);

	return NDIS_STATUS_SUCCESS;

}				/*          */

#if 0
/*                                                                            */
/* 
                                                               
                            
 
                                                
                                        
 
                                                             
                                                        
*/
/*                                                                            */
NDIS_STATUS dbgLogWr1(IN PINT_8 debugStr, IN ...
    )
{
#define TMP_BUF_LEN  256

	HANDLE FileHandle = NULL;
	DWORD BytesWritten;
	va_list paramList;
	PINT_8 buf_p = NULL;
	int strLen;

	/*                   */
	FileHandle = CreateFile(TEXT(DEBUG_FILE_NAME), GENERIC_WRITE, 0,	/*            */
				NULL,	/*                            */
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (FileHandle == INVALID_HANDLE_VALUE) {
		return NDIS_STATUS_FAILURE;
	}

	/*                           */
	buf_p = (PUINT_8) kalMemAlloc(TMP_BUF_LEN);
	if (buf_p == NULL) {
		CloseHandle(FileHandle);
		return NDIS_STATUS_FAILURE;
	}

	/*                                          */
	kalMemZero(buf_p, TMP_BUF_LEN);
	va_start(paramList, debugStr);
	_vsnprintf(buf_p, TMP_BUF_LEN - 1, debugStr, paramList);

	strLen = strlen(buf_p);

	/*                               */
	SetFilePointer(FileHandle, 0, 0, FILE_END);
	WriteFile(FileHandle, buf_p, strLen, &BytesWritten, NULL);

	CloseHandle(FileHandle);
	kalMemFree(buf_p, TMP_BUF_LEN);

	return NDIS_STATUS_SUCCESS;

}				/*           */
#endif

#elif UNICODE_MESSAGE
/*                                                                            */
/* 
                                                         
 
                                                
                                        
 
                                                             
                                                        
 
                                                                              */
NDIS_STATUS dbgLogWr2(IN PINT_8 debugStr, IN ...
    )
{
#define TMP_BUF_LEN   256
#define TMP_WBUF_LEN  (TMP_BUF_LEN * 2)

	va_list paramList;
	PINT_16 wbuf_p;
	PINT_8 buf_p;
	INT_32 strLen;

	/*                           */
	buf_p = (PINT_8) kalMemAlloc(TMP_BUF_LEN, VIR_MEM_TYPE);
	if (buf_p == NULL) {
		return NDIS_STATUS_FAILURE;
	}
	wbuf_p = (PINT_16) kalMemAlloc(TMP_WBUF_LEN, VIR_MEM_TYPE);
	if (wbuf_p == NULL) {
		kalMemFree(buf_p, VIR_MEM_TYPE, TMP_BUF_LEN);
		return NDIS_STATUS_FAILURE;
	}

	/*                */
	kalMemZero(buf_p, TMP_BUF_LEN);
	kalMemZero(wbuf_p, TMP_WBUF_LEN);
	va_start(paramList, debugStr);
	_vsnprintf(buf_p, TMP_BUF_LEN - 1, debugStr, paramList);
	va_end(paramList);
	strLen = strlen(buf_p);

	/*                                                                        
                        */
	mbstowcs(wbuf_p, buf_p, strLen);

	/*                       */
	NKDbgPrintfW(TEXT("%s"), wbuf_p);

	/*                     */
	kalMemFree(buf_p, VIR_MEM_TYPE, TMP_BUF_LEN);
	kalMemFree(wbuf_p, VIR_MEM_TYPE, TMP_WBUF_LEN);

	return NDIS_STATUS_SUCCESS;
}				/*           */
#endif				/*                 */

#endif				/*         */
