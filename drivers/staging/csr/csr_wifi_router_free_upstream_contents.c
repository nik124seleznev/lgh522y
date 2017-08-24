/*****************************************************************************

	(c) Cambridge Silicon Radio Limited 2011
	All rights reserved and confidential information of CSR

	Refer to LICENSE.txt included with this source for details
	on the license terms.

*****************************************************************************/

/*                                       */
#include <linux/slab.h>
#include "csr_wifi_router_prim.h"
#include "csr_wifi_router_lib.h"

/*                                                                             
        
                                                
  
               
  
  
              
                                                                       
                                       
                                                                              */
void CsrWifiRouterFreeUpstreamMessageContents(u16 eventClass, void *message)
{
	if (eventClass != CSR_WIFI_ROUTER_PRIM)
		return;
	if (NULL == message)
		return;
	switch (*((CsrWifiRouterPrim *) message)) {
	case CSR_WIFI_ROUTER_MA_PACKET_IND:
	{
		CsrWifiRouterMaPacketInd *p =
			(CsrWifiRouterMaPacketInd *) message;
		kfree(p->frame);
		p->frame = NULL;
		break;
	}
	default:
		break;
	}
}


