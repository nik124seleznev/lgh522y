/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#ifndef CSR_WIFI_FSM_H
#define CSR_WIFI_FSM_H

#include "csr_prim_defs.h"
#include "csr_log_text.h"
#include "csr_wifi_fsm_event.h"

/*                                             */
#include "csr_wifi_common.h"

#define CSR_WIFI_FSM_ENV       (0xFFFF)

/* 
         
                              
  
                   
                                                    
 */
typedef struct CsrWifiFsmContext CsrWifiFsmContext;

/* 
         
                                                 
  
                   
                                                     
                                                               
  
                                           
  
          
         
 */
typedef void (*CsrWifiFsmExternalWakupCallbackPtr)(void *context);

/* 
         
                                        
  
                   
                                                                  
                                                 
  
                                                 
                                                                      
                                                      
                                                                        
  
          
                                   
 */
extern CsrWifiFsmContext* CsrWifiFsmInit(void *applicationContext, void *externalContext, u16 maxProcesses, CsrLogTextTaskId loggingTaskId);

/* 
         
                                              
  
                   
                                                                          
                                       
                                                                          
                                                                     
                                     
                                                                           
                                                                                  
  
                                         
  
          
         
 */
extern void CsrWifiFsmReset(CsrWifiFsmContext *context);

/* 
         
                                                
  
                   
                                                                          
                                                                        
                 
                                                                           
                                                                     
                                     
                                                                                     
  
                                            
  
          
         
 */
extern void CsrWifiFsmShutdown(CsrWifiFsmContext *context);

/* 
         
                             
  
                   
                                                                                     
                                                                                       
                                                         
                                                                                
                                                  
  
                                       
  
          
                                                                        
 */
extern u32 CsrWifiFsmExecute(CsrWifiFsmContext *context);

/* 
         
                                                                           
  
                   
                                                 
                                                                             
  
                                           
                                                               
                                                                                                            
                                                                                                            
                                        
  
          
         
 */
extern void CsrWifiFsmSendEventExternal(CsrWifiFsmContext *context, CsrWifiFsmEvent *event, u16 source, u16 destination, CsrPrim primtype, u16 id);

/* 
         
                                                                                 
  
                   
                                                 
                                                                             
  
                                           
                                                               
                                                                                                            
                                                                                                            
                                        
 */
#define CsrWifiFsmSendAlienEventExternal(_context, _alienEvent, _source, _destination, _primtype, _id) \
    { \
        CsrWifiFsmAlienEvent *_evt = kmalloc(sizeof(CsrWifiFsmAlienEvent), GFP_KERNEL); \
        _evt->alienEvent = _alienEvent; \
        CsrWifiFsmSendEventExternal(_context, (CsrWifiFsmEvent *)_evt, _source, _destination, _primtype, _id); \
    }


/* 
         
                              
  
                                        
  
          
                       
 */
extern u32 CsrWifiFsmGetTimeOfDayMs(CsrWifiFsmContext *context);

/* 
         
                                                  
  
                   
                                                             
  
                                         
  
          
                                                                        
 */
extern u32 CsrWifiFsmGetNextTimeout(CsrWifiFsmContext *context);

/* 
         
                                                    
  
                                    
                                                        
  
          
         
 */
extern void CsrWifiFsmFastForward(CsrWifiFsmContext *context, u16 ms);

/* 
         
                                               
  
                   
                                                      
  
                                       
                                                
  
          
         
 */
extern void CsrWifiFsmTestAdvanceTime(CsrWifiFsmContext *context, u32 ms);

/* 
         
                                            
  
                                         
  
          
                                                               
 */
extern u8 CsrWifiFsmHasEvents(CsrWifiFsmContext *context);

/* 
         
                                                        
  
                                         
                                                       
  
          
         
 */
extern void CsrWifiFsmInstallWakeupCallback(CsrWifiFsmContext *context, CsrWifiFsmExternalWakupCallbackPtr callback);

#endif /*                */

