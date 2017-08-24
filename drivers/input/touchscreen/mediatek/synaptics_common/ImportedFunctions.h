//                    
//
//                                                              

#ifdef IMPORTEDFUNCTIONS_C
  #define EXTERN
#else
  #define EXTERN extern
#endif

#ifdef _cplusplus
extern "c" {
#endif
#ifndef IMPORTEDFUNCTIONS_H
#define IMPORTEDFUNCTIONS_H
//                                         
//                    
#include "cdci/CdciErrors.h"
#include "cdci/CrocodileTypes.h"
#include "cdci/CdciInterfaces.h"
#include "cdci/DllVersion.h"

EXTERN bool ControlBridgeRMI(bool bLoadCdciApi);
EXTERN void UnloadLibrary();

typedef void CCdciApi;

/* 
                                                                          
           
                                                                                  
                                                                                    
                                                            
                                          
                                                                                    
                                                                                        
*/
typedef void ( __cdecl   * LineCallback_T)(void         * context,
                                           const char   * sName,
                                           bool           bState,
                                           EError         error,
                                           const char   * pErrorMsg);

typedef void ( __stdcall * StdLineCallback_T)(void        * context,
                                              const char  * sName,
                                              bool          bState,
                                              EError        error,
                                              const char  * pErrorMsg);

typedef void ( __cdecl * PacketRxCallback_T)(void           * context,
                                             unsigned int     uCount, 
                                             unsigned char  * pData, 
                                             EError           error, 
                                             const char     * pErrorMsg);

//                 
typedef CCdciApi * (*InitializeFunc) ();
typedef EError (*UninitializeFunc) (CCdciApi *   ulHandle);
typedef CCdciStreamInterface * (*CdciStream_CreateByCdciTypeFunc)(int iConnectionType);
typedef EError (*CDCI_CreateByCdciTypeFunc)(int iConnectionType, CCdciApi ** ppCDCIApi);
typedef void                   (*CdciStream_DestroyFunc)(CCdciStreamInterface *pMpcUsb);
typedef CCdciApi *             (*CreateFromStreamFunc)(CCdciStreamInterface * pStream);
typedef void                   (*CDCI_DestroyFunc)(CCdciApi * pCDCIApi);

typedef EError (*ConnectFunc) (CCdciApi *   ulHandle);
typedef EError (*DisconnectFunc) (CCdciApi *   ulHandle);
typedef EError (*CreateBySerialNumberFunc) (const char * sSerialNumber, CCdciApi *&ulHandle);
typedef bool(*IsConnectedFunc)(CCdciApi * pCDCIApi);

typedef EError (*ConfigI2CRegExFunc) (CCdciApi *    ulHandle,  
                                 unsigned short   target,
                                 ERmiAddress      RmiAddressingMode,
                                 EPullups         pull_ups,  
                                 EI2CSpeed        i2cSpeed,                             
                                 EAttention       attn /*        */,
                                 unsigned long    dwTimeout /*           */);

typedef EError (*ConfigI2CRegFunc) (CCdciApi *    ulHandle,  
                                 unsigned short   target,
                                 bool             pull_ups,  
                                 EI2CSpeed        i2cSpeed,                             
                                 EAttention       attn /*        */,
                                 unsigned long    dwTimeout /*           */);

typedef EError (*ConfigSPIRegFunc) (CCdciApi *    ulHandle,
                                 unsigned short   target,  
                                 ERmiAddress      addressing_mode,
                                 EModuleMode      module_mode,
                                 EPullups         pull_ups, 
                                 ESlaveSelect     slave_select,
                                 unsigned int     byte_delay,
                                 unsigned int     bit_rate,
                                 unsigned int     spi_mode,
                                 EAttention       attn /*        */,
                                 unsigned long    dwTimeout /*           */);

typedef EError (*ConfigPS2Func) (CCdciApi *   ulHandle,
                              unsigned short  target, 
                              bool            resend_enable,
                              unsigned long   dwTimeout /*           */);

typedef EError (*PowerOnVoltageFunc) (CCdciApi *   ulHandle, 
                                   unsigned short  target, 
                                   unsigned int    vdd_millivolts,
                                   unsigned int    vdevpullup_millivolts,
                                   unsigned int    vled_millivolts,
                                   unsigned long   dwTimeout /*           */);

typedef EError (*PowerOffFunc) (CCdciApi *   ulHandle,
                             unsigned short  target,
                             unsigned long   dwTimeout/*          */);

//                         
typedef EError (*WriteRegister8DataFunc) (CCdciApi *    ulHandle,
                                       unsigned short   target, 
                                       unsigned short   uBusAddress,
                                       ERmiAddress      RmiAddressingMode,
                                       unsigned short   uRmiAddress, 
                                       unsigned char  * data,
                                       unsigned int     length,
                                       unsigned int     &lengthWritten,
                                       unsigned long    dwTimeout );

typedef EError (*ReadRegister8DataFunc) (CCdciApi *   ulHandle,
                                      unsigned short  target,
                                      unsigned short  uBusAddress, //           
                                      ERmiAddress     RmiAddressingMode,
                                      unsigned short  uRmiAddress, //                
                                      unsigned char * data,
                                      unsigned int    length, 
                                      unsigned int    &lengthRead,
                                      unsigned long   dwTimeout /*           */);

typedef EError (*PacketSetRxCallbackFunc) (CCdciApi *    ulHandle,
                                        PacketRxCallback_T,
                                        void           * context);

typedef EError (*PacketTxFunc) (CCdciApi *   ulHandle,
                             unsigned short  target,
                             unsigned char * pData,
                             unsigned int    uCount,
                             unsigned long   dwTimeout /*          */ );

typedef EError (*Line_SetCallbackFunc) (CCdciApi *         ulHandle, 
                                        StdLineCallback_T, 
                                        void              * context);

typedef void (*GetLastErrorMsgStringFunc)(CCdciApi * pCDCIApi, char *strValue);

typedef ICdciRmiTransaction * (*RmiTransactionNewFunc) (CCdciApi * pCDCIApi);
typedef void (*RmiTransactionDeleteFunc) (CCdciApi * pCDCIApi, ICdciRmiTransaction * pTrans);

typedef EError (*GetControllerInfoFunc)(CCdciApi * pCDCIApi, unsigned short &Id,              //               
    unsigned int &FWVersionMajor,     //                          
    unsigned int &FWVersionMinor,     //                          
    unsigned int &CDCIVersionMajor,  //                    
    unsigned int &CDCIVersionMinor, //                    
    char *strInfo,
    unsigned int &uBoardNumber,
    unsigned int &uSerialNumber);

//                                                          
typedef EError (*ConfigRmiOverSMBusFunc) (CCdciApi *    ulHandle,
                                          unsigned short  target,
                                          EPullups        Pullups,
                                          EI2CSpeed       i2cSpeed, 
                                          DWORD           dwTimeout /*          */);

typedef EError (*ConfigHidOverI2CFunc) (CCdciApi *      pCDCIApi,
                                        unsigned short  target,
                                        ERmiAddress     rmiAddressingMode,
                                        EPullups        pullups,
                                        EPullups        attnPullups,
                                        EI2CSpeed       i2cSpeed, 
                                        EAttention      attn/*            */,
                                        DWORD           dwTimeout/*           */);

typedef EError (*ConfigHidOverUsbFunc) (CCdciApi * pCDCIApi,
                                        EAttention attn,
                                        DWORD dwTimeout);

typedef EError (*ConfigRmiOverSMBusAddressFunc) ( CCdciApi *    ulHandle,
                                                  unsigned short  target,
                                                  EPullups        Pullups,
                                                  EI2CSpeed       i2cSpeed, 
                                                  unsigned short  i2cHostAddress,
                                                  DWORD           dwTimeout /*          */);

typedef EError (*ConfigRmiNativeFunc) ( CCdciApi *      ulHandle,
                                        unsigned short  target,
                                        EAttention      attn,
                                        DWORD           dwTimeout /*          */);

typedef EError (*CreateByCdciTypeFunc) (int iConnectionType, CCdciApi ** ppCDCIApi);

typedef EError (*SetStreamOptionFunc) (CCdciApi *       ulHandle, 
                                       int              iOptionID, 
                                       const char *     psValue);

typedef EError (*SetTargetOptionAsUInteger32Func) ( CCdciApi * ulHandle,
                                                    unsigned short target,
                                                    int iOption,
                                                    unsigned int uValue,
                                                    DWORD dwTimeout /*           */);

//                          
//                                                                                   
bool ControlBridgeRMI(bool bLoadCdciApi);
void Line_FlushAttention();
bool Line_AttentionIsAsserted();
EError Line_WaitForAttentionAsserted(DWORD dwMilliseconds);
EError Line_WaitForAttentionDeasserted(DWORD dwMilliseconds);
EError Line_WaitForAttention(DWORD dwMilliseconds);

void UnloadLibrary();
const int DefaultTimeout = 3000; //                
const unsigned short DefaultTarget = 0;
#endif

EXTERN HMODULE g_hDll;
EXTERN HANDLE  g_hMutex;                    //                                                                    
EXTERN HANDLE  g_hLoadHandleMutex;
EXTERN HANDLE  m_hAttenEvent;
#ifndef REFLASH_DLL
  EXTERN int g_DllCount;
#endif


EXTERN CdciStream_CreateByCdciTypeFunc  g_CdciStream_CreateByCdciType;
EXTERN CDCI_CreateByCdciTypeFunc        g_CdciCreateByCdciType;
EXTERN CdciStream_DestroyFunc           g_CdciStream_Destroy;
EXTERN CreateFromStreamFunc             CreateFromStream;
EXTERN CDCI_DestroyFunc                 Destroy;
EXTERN InitializeFunc                   InitializeDevice;
EXTERN UninitializeFunc                 UninitializeDevice;
EXTERN ConnectFunc                      ConnectDevice;
EXTERN DisconnectFunc                   DisconnectDevice;
EXTERN IsConnectedFunc                  IsConnected;
EXTERN CreateBySerialNumberFunc	        CreateBySerialNumber;
EXTERN ConfigI2CRegExFunc               ConfigI2CReg;
EXTERN ConfigSPIRegFunc                 ConfigSPIReg;
EXTERN ConfigPS2Func                    ConfigPS2;
EXTERN PowerOnVoltageFunc               PowerOnDevice;
EXTERN PowerOffFunc                     PowerOffDevice;
EXTERN WriteRegister8DataFunc           WriteRegister8;
EXTERN ReadRegister8DataFunc            ReadRegister8;
EXTERN PacketSetRxCallbackFunc          PacketSetRxCallback;
EXTERN PacketTxFunc                     PacketTx;
EXTERN Line_SetCallbackFunc             Line_SetCallback;
EXTERN RmiTransactionNewFunc            RmiTransactionNew;
EXTERN RmiTransactionDeleteFunc         RmiTransactionDelete;
EXTERN GetLastErrorMsgStringFunc        GetLastErrorMsgString;
EXTERN PFNCDCI_GetDllVersion            GetCDCIApiDllVersion;
EXTERN ConfigRmiOverSMBusFunc           ConfigRmiOverSMBus;
EXTERN GetControllerInfoFunc            GetControllerInfo;
EXTERN ConfigRmiNativeFunc              ConfigRmiNative;
EXTERN CreateByCdciTypeFunc             CreateByCdciType;
EXTERN SetStreamOptionFunc              SetStreamOption;
EXTERN SetTargetOptionAsUInteger32Func  SetTargetOption;
EXTERN ConfigHidOverI2CFunc             ConfigHidOverI2C;
EXTERN ConfigHidOverUsbFunc             ConfigHidOverUsb;

#ifdef _cplusplus
}
#endif
