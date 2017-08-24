/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#ifndef CSR_WIFI_FSM_TYPES_H
#define CSR_WIFI_FSM_TYPES_H

#include <linux/types.h>
#include "csr_macro.h"
#include "csr_sched.h"

#ifdef CSR_WIFI_FSM_MUTEX_ENABLE
#include "csr_framework_ext.h"
#endif

#include "csr_wifi_fsm.h"

#define CSR_WIFI_FSM_MAX_TRANSITION_HISTORY 10

/* 
         
                           
  
                   
                                  
 */
typedef struct CsrWifiFsmEventList
{
    CsrWifiFsmEvent *first;
    CsrWifiFsmEvent *last;
} CsrWifiFsmEventList;


/* 
         
                  
  
                   
                                                              
                                                                      
 */
typedef struct CsrWifiFsmTimerId
{
    CsrPrim     type;
    u16   primtype;
    CsrSchedQid destination;
    u16   uniqueid;
} CsrWifiFsmTimerId;

/* 
         
                      
  
                   
                                                         
                                                              
                                                                        
 */
typedef struct CsrWifiFsmTimer
{
    CsrPrim     type;
    u16   primtype;
    CsrSchedQid destination;
    CsrSchedQid source;

    /*                                                */
    struct CsrWifiFsmTimer *next;

    CsrWifiFsmTimerId timerid;
    u32         timeoutTimeMs;
} CsrWifiFsmTimer;


/* 
         
                    
  
                   
                                                                        
                                            
 */
typedef struct
{
    CsrWifiFsmEvent event;
    void           *alienEvent;
} CsrWifiFsmAlienEvent;


/* 
         
                           
  
                   
                                  
 */
typedef struct CsrWifiFsmTimerList
{
    CsrWifiFsmTimer *first;
    CsrWifiFsmTimer *last;
    u16        nexttimerid;
} CsrWifiFsmTimerList;

/* 
         
                                   
  
                   
                                                
                                      
  
                                      
  
          
         
 */
typedef void (*CsrWifiFsmProcEntryFnPtr)(CsrWifiFsmContext *context);

/* 
         
                                        
  
                   
                                                   
                                                          
  
                                                 
                                               
                                                           
  
          
         
 */
typedef void (*CsrWifiFsmTransitionFnPtr)(CsrWifiFsmContext *context, void *fsmData, const CsrWifiFsmEvent *event);

/* 
         
                                            
  
                   
                                                         
                                        
  
                                           
  
          
         
 */
typedef void (*CsrWifiFsmProcResetFnPtr)(CsrWifiFsmContext *context);

/* 
         
                                                     
  
                   
                                                         
                                                             
         
  
                                           
  
          
                                                
 */
typedef u16 (*CsrWifiFsmDestLookupCallbackPtr)(void *context, const CsrWifiFsmEvent *event);


#ifdef CSR_WIFI_FSM_DUMP_ENABLE
/* 
         
                                
  
                   
                                         
  
                                      
                                 
  
          
         
 */
typedef void (*CsrWifiFsmDumpFnPtr)(CsrWifiFsmContext *context, void *fsmData);
#endif

/* 
         
                                          
  
                   
                                                   
 */
typedef struct
{
    u32                 eventid;
    CsrWifiFsmTransitionFnPtr transition;
#ifdef CSR_LOG_ENABLE
    const char *transitionName;
#endif
} CsrWifiFsmEventEntry;

/* 
         
                                    
  
                   
                                              
                                 
 */
typedef struct
{
    const u8              numEntries;
    const u8               saveAll;
    const CsrWifiFsmEventEntry *eventEntryArray; /*                                                 */
#ifdef CSR_LOG_ENABLE
    u16            stateNumber;
    const char *stateName;
#endif
} CsrWifiFsmTableEntry;

/* 
         
                                  
  
                   
                                                          
 */
typedef struct
{
    u16                   numStates;         /*                     */
    const CsrWifiFsmTableEntry *aStateEventMatrix; /*                     */
} CsrWifiFsmTransitionFunctionTable;

/* 
         
                             
  
                   
                                    
                                                  
               
 */
typedef struct
{
    const char                    *processName;
    const u32                         processId;
    const CsrWifiFsmTransitionFunctionTable transitionTable;
    const CsrWifiFsmTableEntry              unhandledTransitions;
    const CsrWifiFsmTableEntry              ignoreFunctions;
    const CsrWifiFsmProcEntryFnPtr          entryFn;
    const CsrWifiFsmProcResetFnPtr          resetFn;
#ifdef CSR_WIFI_FSM_DUMP_ENABLE
    const CsrWifiFsmDumpFnPtr dumpFn;               /*                                               */
#endif
} CsrWifiFsmProcessStateMachine;

#ifdef CSR_WIFI_FSM_DUMP_ENABLE
/* 
         
                                      
 */
typedef struct
{
    u16                 transitionNumber;
    CsrWifiFsmEvent           event;
    u16                 fromState;
    u16                 toState;
    CsrWifiFsmTransitionFnPtr transitionFn;
    u16                 transitionCount; /*                                                      */
#ifdef CSR_LOG_ENABLE
    const char *transitionName;
#endif
} CsrWifiFsmTransitionRecord;

/* 
         
                                             
 */
typedef struct
{
    u16                  numTransitions;
    CsrWifiFsmTransitionRecord records[CSR_WIFI_FSM_MAX_TRANSITION_HISTORY];
} CsrWifiFsmTransitionRecords;
#endif

/* 
         
                         
  
                   
                                                           
                                          
 */
typedef struct
{
    const CsrWifiFsmProcessStateMachine *fsmInfo;         /*                                                           */
    u16                            instanceId;      /*                    */
    u16                            state;           /*               */
    void                                *params;          /*                    */
    CsrWifiFsmEventList                  savedEventQueue; /*                       */
    struct CsrWifiFsmInstanceEntry      *subFsm;          /*                       */
    struct CsrWifiFsmInstanceEntry      *subFsmCaller;    /*                                                                          */
#ifdef CSR_WIFI_FSM_DUMP_ENABLE
    CsrWifiFsmTransitionRecords transitionRecords;        /*                               */
#endif
} CsrWifiFsmInstanceEntry;

/* 
         
                                       
  
                   
                                   
  
                                              
                                        
  
          
         
 */
typedef void (*CsrWifiFsmOnCreateFnPtr)(void *extContext, const CsrWifiFsmInstanceEntry *instance);

/* 
         
                                           
  
                   
                                               
  
                                              
                                             
                              
  
          
         
 */
typedef void (*CsrWifiFsmOnTransitionFnPtr)(void *extContext, const CsrWifiFsmEventEntry *eventEntryArray, const CsrWifiFsmEvent *event);

/* 
         
                                            
  
                   
                                              
  
                                              
  
          
         
 */
typedef void (*CsrWifiFsmOnStateChangeFnPtr)(void *extContext, u16 nextstate);

/* 
         
                                                            
  
                   
                                               
  
                                              
                              
  
          
         
 */
typedef void (*CsrWifiFsmOnEventFnPtr)(void *extContext, const CsrWifiFsmEvent *event);

/* 
         
                              
  
                   
                                                    
 */
struct CsrWifiFsmContext
{
    CsrWifiFsmEventList eventQueue;                           /*                                              */
    CsrWifiFsmEventList externalEventQueue;                   /*                                              */
#ifdef CSR_WIFI_FSM_MUTEX_ENABLE
    CsrMutexHandle externalEventQueueLock;                    /*                                              */
#endif
    u32                          timeOffset;            /*                                              */
    CsrWifiFsmTimerList                timerQueue;            /*                                              */
    u8                            useTempSaveList;       /*                                              */
    CsrWifiFsmEventList                tempSaveList;          /*                                              */
    CsrWifiFsmEvent                   *eventForwardedOrSaved; /*                                              */
    u16                          maxProcesses;          /*                                              */
    u16                          numProcesses;          /*                                              */
    CsrWifiFsmInstanceEntry           *instanceArray;         /*                                              */
    CsrWifiFsmInstanceEntry           *ownerInstance;         /*                                                        */
    CsrWifiFsmInstanceEntry           *currentInstance;       /*                                              */
    CsrWifiFsmExternalWakupCallbackPtr externalEventFn;       /*                                              */
    CsrWifiFsmOnEventFnPtr             appIgnoreCallback;     /*                                              */
    CsrWifiFsmDestLookupCallbackPtr    appEvtDstCallback;     /*                                              */

    void            *applicationContext;                      /*                                              */
    void            *externalContext;                         /*                                              */
    CsrLogTextTaskId loggingTaskId;                           /*                                              */

#ifndef CSR_WIFI_FSM_SCHEDULER_DISABLED
    CsrSchedTid schedTimerId;                                 /*                                              */
    u32   schedTimerNexttimeoutMs;                      /*                                              */
#endif

#ifdef CSR_WIFI_FSM_MUTEX_ENABLE
#ifdef CSR_WIFI_FSM_TRANSITION_LOCK
    CsrMutexHandle transitionLock;                     /*                                               */
#endif
#endif

#ifdef CSR_LOG_ENABLE
    CsrWifiFsmOnCreateFnPtr      onCreate;             /*                                              */
    CsrWifiFsmOnTransitionFnPtr  onTransition;         /*                                              */
    CsrWifiFsmOnTransitionFnPtr  onUnhandedCallback;   /*                                              */
    CsrWifiFsmOnStateChangeFnPtr onStateChange;        /*                                              */
    CsrWifiFsmOnEventFnPtr       onIgnoreCallback;     /*                                              */
    CsrWifiFsmOnEventFnPtr       onSaveCallback;       /*                                              */
    CsrWifiFsmOnEventFnPtr       onErrorCallback;      /*                                              */
    CsrWifiFsmOnEventFnPtr       onInvalidCallback;    /*                                              */
#endif
#ifdef CSR_WIFI_FSM_DUMP_ENABLE
    u16 masterTransitionNumber;                  /*                                             */
#endif
};

#endif /*                      */
