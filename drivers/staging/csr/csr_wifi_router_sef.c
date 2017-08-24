/*****************************************************************************

  (c) Cambridge Silicon Radio Limited 2010
  Confidential information of CSR

  Refer to LICENSE.txt included with this source for details
  on the license terms.

 *****************************************************************************/
#include "csr_wifi_router_sef.h"

const CsrWifiRouterStateHandlerType CsrWifiRouterDownstreamStateHandlers[CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_COUNT] =
{
    /*        */ CsrWifiRouterMaPacketSubscribeReqHandler,
    /*        */ CsrWifiRouterMaPacketUnsubscribeReqHandler,
    /*        */ CsrWifiRouterMaPacketReqHandler,
    /*        */ CsrWifiRouterMaPacketResHandler,
    /*        */ CsrWifiRouterMaPacketCancelReqHandler,
};
