/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2010
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/freezer.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/bitops.h>

#include "csr_framework_ext.h"

/*                                                                             
        
                      
  
               
                                 
  
           
            
  
                                                                              */
void CsrThreadSleep(u16 sleepTimeInMs)
{
    unsigned long t;

    /*                                         */
    t = ((sleepTimeInMs * HZ) + 999) / 1000;
    schedule_timeout_uninterruptible(t);
}
EXPORT_SYMBOL_GPL(CsrThreadSleep);
