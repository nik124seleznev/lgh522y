#ifndef CSR_SDIO_H__
#define CSR_SDIO_H__
/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2010
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#include "csr_result.h"

/*              */
#define CSR_SDIO_RESULT_INVALID_VALUE   ((CsrResult) 1) /*                        */
#define CSR_SDIO_RESULT_NO_DEVICE       ((CsrResult) 2) /*                                           */
#define CSR_SDIO_RESULT_CRC_ERROR       ((CsrResult) 3) /*                                                                         */
#define CSR_SDIO_RESULT_TIMEOUT         ((CsrResult) 4) /*                                                                                                                    */
#define CSR_SDIO_RESULT_NOT_RESET       ((CsrResult) 5) /*                          */

/*                                                          */
#define CSR_SDIO_FEATURE_BYTE_MODE                   0x00000001 /*                                                           */
#define CSR_SDIO_FEATURE_DMA_CAPABLE_MEM_REQUIRED    0x00000002 /*                                             */

/*                                                                    */
#define CSR_SDIO_ANY_MANF_ID        0xFFFF
#define CSR_SDIO_ANY_CARD_ID        0xFFFF
#define CSR_SDIO_ANY_SDIO_FUNCTION  0xFF
#define CSR_SDIO_ANY_SDIO_INTERFACE 0xFF

/*                                                                             
        
                         
  
               
                                                                            
                                                                                
                                                                           
  
           
                                                     
                                                     
                                                                            
                                                                                     
  
                                                                              */
typedef struct
{
    u16 manfId;       /*                                            */
    u16 cardId;       /*                                            */
    u8  sdioFunction; /*                                                             */
    u8  sdioInterface; /*                                                                      */
} CsrSdioFunctionId;

/*                                                                             
        
                       
  
               
                                                                
  
           
                                                                             
                                                                                
                                                                      
                                               
                                                                       
                                                               
                                                                                
                                                                        
                                                                              
                                                        
                                                                                
                                          
  
  
                                                                              */
typedef struct
{
    CsrSdioFunctionId sdioId;
    u16         blockSize; /*                                                    */
    u32         features; /*                                             */
    void             *device; /*                                          */
    void             *driverData; /*                                */
    void             *priv; /*                            */
} CsrSdioFunction;

/*                                                                             
        
                                                       
  
               
                                                                              
                                                                
                                                                               
                                                                                
                                              
  
                                                                             
                                                                              
  
              
                                                               
  
                                                                              */
typedef void (*CsrSdioInsertedCallback)(CsrSdioFunction *function);
typedef void (*CsrSdioRemovedCallback)(CsrSdioFunction *function);

/*                                                                             
        
                                                             
  
               
                                                                          
                                                          
  
                                                                             
                                                                              
  
              
                                                               
  
                                           
                                                            
  
                                                                              */
typedef void (*CsrSdioInterruptDsrCallback)(CsrSdioFunction *function);
typedef CsrSdioInterruptDsrCallback (*CsrSdioInterruptCallback)(CsrSdioFunction *function);

/*                                                                             
        
                                                     
  
               
                                                                           
                                                                               
                                          
  
                                                                             
                                                                              
  
              
                                                               
  
                                                                              */
typedef void (*CsrSdioSuspendCallback)(CsrSdioFunction *function);
typedef void (*CsrSdioResumeCallback)(CsrSdioFunction *function);

/*                                                                             
        
                                                     
  
               
                                                                                
  
                                                                             
                                                                             
                            
  
              
                                                               
                                                                                
                                                                       
  
                                       
                                                        
  
                                                                              */
typedef void (*CsrSdioAsyncDsrCallback)(CsrSdioFunction *function, CsrResult result);
typedef CsrSdioAsyncDsrCallback (*CsrSdioAsyncCallback)(CsrSdioFunction *function, CsrResult result);

/*                                                                             
        
                             
  
               
                                                 
  
           
                                                                        
                                                                      
                                                                     
                                                                      
                                                                    
                                                                              
                                                   
                                           
                                                                             
                                                                                
                                 
  
                                                                              */
typedef struct
{
    CsrSdioInsertedCallback  inserted;
    CsrSdioRemovedCallback   removed;
    CsrSdioInterruptCallback intr;
    CsrSdioSuspendCallback   suspend;
    CsrSdioResumeCallback    resume;
    CsrSdioFunctionId       *ids;
    u8                 idsCount;
    void                    *priv;          /*                            */
} CsrSdioFunctionDriver;

/*                                                                             
        
                                     
  
               
                                   
  
              
                                                                          
  
           
                                                                 
                                        
                                                                    
                                                                              
                                                                          
                                                                             
                                                                          
                                                                                
                                                                
  
                                                                              */
CsrResult CsrSdioFunctionDriverRegister(CsrSdioFunctionDriver *functionDriver);

/*                                                                             
        
                                       
  
               
                                                           
  
              
                                                                          
  
                                                                              */
void CsrSdioFunctionDriverUnregister(CsrSdioFunctionDriver *functionDriver);

/*                                                                             
        
                                                     
  
               
                                                                     
                                                                            
                                                                               
                                                                   
  
              
                                                               
  
           
                                                                         
                                                       
                                                                      
                                                                          
                                                                             
                                              
                                                                             
                                                                       
                                                                        
  
                                                                     
                                                           
                                                                               
                                                                   
                                                                           
                                                  
  
                                                               
                                                               
                                                             
  
                                                                              */
CsrResult CsrSdioFunctionEnable(CsrSdioFunction *function);
CsrResult CsrSdioFunctionDisable(CsrSdioFunction *function);

/*                                                                             
        
                                                       
  
               
                                                                  
                                                                            
                   
  
              
                                                               
  
           
                                                                         
                                                       
                                                                      
                                                                          
                                                                             
                                              
                                                                        
                                                                           
                                                                               
                                                                              
                                                              
  
                                                                     
                                                           
                                                                               
                                                                   
                                                                           
                                                  
  
                                                               
                                                               
                                                             
  
                                                                              */
CsrResult CsrSdioInterruptEnable(CsrSdioFunction *function);
CsrResult CsrSdioInterruptDisable(CsrSdioFunction *function);

/*                                                                             
        
                                   
  
               
                                                                           
                                                                            
                               
  
              
                                                                           
                                       
  
                                                                              */
void CsrSdioInterruptAcknowledge(CsrSdioFunction *function);

/*                                                                             
        
                                                             
  
               
                                                                             
                                                                               
                                   
  
              
                                                                           
                                          
                                                
                                                                     
                                                                               
                                                                   
                                                                          
                                                   
                                                                   
                                                                              
                                                                             
                                                                        
                                               
  
                                                                              */
void CsrSdioInsertedAcknowledge(CsrSdioFunction *function, CsrResult result);
void CsrSdioRemovedAcknowledge(CsrSdioFunction *function);

/*                                                                             
        
                                                           
  
               
                                                                               
                                                                        
                               
  
              
                                                                           
                                       
              
                                                                
                                                           
  
                                                                              */
void CsrSdioSuspendAcknowledge(CsrSdioFunction *function, CsrResult result);
void CsrSdioResumeAcknowledge(CsrSdioFunction *function, CsrResult result);

/*                                                                             
        
                           
  
               
                                                                               
                                     
                                                            
                                                                   
                                                                  
  
                                                                       
                                                                 
  
              
                                                               
                                                                           
                         
  
           
                                                                
                                              
                                                       
                                                                           
                                                                      
                                                                              
                                                      
                                                              
  
                                                                           
                                                                           
                                                                
                                                                           
                                                  
  
                                                               
                                                               
                                                             
  
                                                                            
                                                                             
                                                                           
                                                                          
                                                                 
  
                                                                              */
CsrResult CsrSdioBlockSizeSet(CsrSdioFunction *function, u16 blockSize);

/*                                                                             
        
                                      
  
               
                                                                             
                                                                             
                                       
                                                               
                                                                        
                                                                             
                      
  
                                                                             
                                                                               
                                                                            
  
                                                                             
                    
  
              
                                                               
                                                                             
  
           
                                                                         
                                                       
                                                       
                                                                           
                                                                      
  
                                                                           
                                                                           
                                                                
                                                                           
                                                  
  
                                                               
                                                               
                                                             
  
  
                                                                              */
CsrResult CsrSdioMaxBusClockFrequencySet(CsrSdioFunction *function, u32 maxFrequency);

/*                                                                             
        
                                                                          
  
               
                                                                        
  
              
                                                               
                                                       
                                      
                                                                
  
           
                                                                    
                                                       
                                                                           
                                                                      
                                                                               
                                                              
  
                                                                     
                                                           
                                                                               
                                                                   
                                                                           
                                                  
  
                                                               
                                                               
                                                             
  
                                                                           
                                                                                
                                                                                
                                    
  
                                                                              */
CsrResult CsrSdioRead8(CsrSdioFunction *function, u32 address, u8 *data);
CsrResult CsrSdioWrite8(CsrSdioFunction *function, u32 address, u8 data);
void CsrSdioRead8Async(CsrSdioFunction *function, u32 address, u8 *data, CsrSdioAsyncCallback callback);
void CsrSdioWrite8Async(CsrSdioFunction *function, u32 address, u8 data, CsrSdioAsyncCallback callback);

/*                                                                             
        
                                                                              
  
               
                                                                        
  
              
                                                               
                                                       
                                      
                                                                
  
           
                                                                    
                                                       
                                                                           
                                                                      
                                                                            
                                                           
                                                              
  
                                                                     
                                                           
                                                                               
                                                                   
                                                                           
                                                  
  
                                                               
                                                               
                                                             
  
                                                                             
                                                                                
                                                                                
                                    
  
                                                                              */
CsrResult CsrSdioRead16(CsrSdioFunction *function, u32 address, u16 *data);
CsrResult CsrSdioWrite16(CsrSdioFunction *function, u32 address, u16 data);
void CsrSdioRead16Async(CsrSdioFunction *function, u32 address, u16 *data, CsrSdioAsyncCallback callback);
void CsrSdioWrite16Async(CsrSdioFunction *function, u32 address, u16 data, CsrSdioAsyncCallback callback);

/*                                                                             
        
                                                             
                            
  
               
                                                                          
                   
  
              
                                                               
                                                       
                                      
                                                                
  
           
                                                                    
                                                       
                                                                           
                                                                      
                                                                               
                                                              
  
                                                                     
                                                           
                                                                               
                                                                   
                                                                           
                                                  
  
                                                               
                                                               
                                                             
  
                                                                               
                                                                                
                                                                                
                                    
  
                                                                              */
CsrResult CsrSdioF0Read8(CsrSdioFunction *function, u32 address, u8 *data);
CsrResult CsrSdioF0Write8(CsrSdioFunction *function, u32 address, u8 data);
void CsrSdioF0Read8Async(CsrSdioFunction *function, u32 address, u8 *data, CsrSdioAsyncCallback callback);
void CsrSdioF0Write8Async(CsrSdioFunction *function, u32 address, u8 data, CsrSdioAsyncCallback callback);

/*                                                                             
        
                                                                      
  
               
                                                                             
                
  
              
                                                               
                                                       
                                      
                                              
                                                                
  
           
                                                                    
                                                       
                                                                           
                                                                      
                                                                            
                                                           
                                                              
  
                                                                     
                                                           
                                                                               
                                                                   
                                                                           
                                                  
  
                                                               
                                                               
                                                             
  
                                                                               
                                                                                
                                                                                
                                    
  
                                                                              */
CsrResult CsrSdioRead(CsrSdioFunction *function, u32 address, void *data, u32 length);
CsrResult CsrSdioWrite(CsrSdioFunction *function, u32 address, const void *data, u32 length);
void CsrSdioReadAsync(CsrSdioFunction *function, u32 address, void *data, u32 length, CsrSdioAsyncCallback callback);
void CsrSdioWriteAsync(CsrSdioFunction *function, u32 address, const void *data, u32 length, CsrSdioAsyncCallback callback);

/*                                                                             
        
                                       
  
               
                                
  
              
                                                                              
                                              
  
                                 
                                                                            
                                                         
                                                       
                                                                      
                                                                                 
                                                                    
                                                   
                                                                    
                                                                          
                                                        
  
                                                                              */
CsrResult CsrSdioPowerOn(CsrSdioFunction *function);
void CsrSdioPowerOff(CsrSdioFunction *function);

/*                                                                             
        
                        
  
               
                                               
  
              
                                                                              
                                            
  
           
                                                                            
                                                         
                                                       
                                                                      
                                                                                 
                                                                    
                                                   
                                                                               
                                                                         
                                              
  
                                                                              */
CsrResult CsrSdioHardReset(CsrSdioFunction *function);

/*                                                                             
        
                                                  
  
               
  
              
                                                               
  
                                                                              */
void CsrSdioFunctionActive(CsrSdioFunction *function);
void CsrSdioFunctionIdle(CsrSdioFunction *function);

#endif
