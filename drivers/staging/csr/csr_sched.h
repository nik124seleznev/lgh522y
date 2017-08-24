#ifndef CSR_SCHED_H__
#define CSR_SCHED_H__
/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2010
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/
#include <linux/types.h>
#include "csr_time.h"

/*                                        */
typedef u32 CsrSchedIdentifier;

/*                   */
typedef u16 CsrSchedTaskId;

/*                    */
typedef u16 CsrSchedQid;

/*                      */
typedef CsrSchedIdentifier CsrSchedMsgId;

/*                          */
typedef CsrSchedIdentifier CsrSchedTid;
#define CSR_SCHED_TID_INVALID     ((CsrSchedTid) 0)

/*                 */
#define CSR_SCHED_TIME_MAX                (0xFFFFFFFF)
#define CSR_SCHED_MILLISECOND             (1000)
#define CSR_SCHED_SECOND                  (1000 * CSR_SCHED_MILLISECOND)
#define CSR_SCHED_MINUTE                  (60 * CSR_SCHED_SECOND)

/*                                                     */
#define CSR_SCHED_TASK_ID        0xFFFF
#define CSR_SCHED_PRIM                   (CSR_SCHED_TASK_ID)
#define CSR_SCHED_EXCLUDED_MODULE_QUEUE      0xFFFF

/*
                                   
 */
typedef u16 CsrSchedBgint;
#define CSR_SCHED_BGINT_INVALID ((CsrSchedBgint) 0xFFFF)

/*                                                                             
        
                          
  
               
                                                                             
                                      
  
                                                                              
                                                                            
                
  
        
                                                                            
                                                                              
                                                                             
                                                                                     
                                                                               
                                                                    
  
           
             
  
                                                                              */
#if defined(CSR_LOG_ENABLE) && defined(CSR_LOG_INCLUDE_FILE_NAME_AND_LINE_NUMBER)
void CsrSchedMessagePutStringLog(CsrSchedQid q,
    u16 mi,
    void *mv,
    u32 line,
    const char *file);
#define CsrSchedMessagePut(q, mi, mv) CsrSchedMessagePutStringLog((q), (mi), (mv), __LINE__, __FILE__)
#else
void CsrSchedMessagePut(CsrSchedQid q,
    u16 mi,
    void *mv);
#endif

#endif
