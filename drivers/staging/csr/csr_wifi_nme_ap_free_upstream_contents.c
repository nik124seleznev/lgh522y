/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#include "csr_wifi_nme_ap_prim.h"
#include "csr_wifi_nme_ap_lib.h"

/*                                                                             
        
                                               
  
               
  
  
              
                                                                       
                                       
                                                                              */
void CsrWifiNmeApFreeUpstreamMessageContents(u16 eventClass, void *message)
{
    if (eventClass != CSR_WIFI_NME_AP_PRIM)
    {
        return;
    }
    if (NULL == message)
    {
        return;
    }
}


