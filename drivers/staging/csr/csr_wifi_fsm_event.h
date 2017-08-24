/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#ifndef CSR_WIFI_FSM_EVENT_H
#define CSR_WIFI_FSM_EVENT_H

#include "csr_prim_defs.h"
#include "csr_sched.h"

/* 
         
                      
  
                   
                                                          
                                                        
 */
typedef struct CsrWifiFsmEvent
{
    CsrPrim     type;
    u16   primtype;
    CsrSchedQid destination;
    CsrSchedQid source;

    /*                                                */
    /*                           
                                                          
                                                                  
                                                          
     */
    struct CsrWifiFsmEvent *next;
} CsrWifiFsmEvent;

#endif /*                      */

