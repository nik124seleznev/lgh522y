/*****************************************************************************

  (c) Cambridge Silicon Radio Limited 2010
  Confidential information of CSR

  Refer to LICENSE.txt included with this source for details
  on the license terms.

 *****************************************************************************/
#include "csr_wifi_router_ctrl_sef.h"

const CsrWifiRouterCtrlStateHandlerType
	CsrWifiRouterCtrlDownstreamStateHandlers
	[CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_COUNT] = {
		/*        */ CsrWifiRouterCtrlConfigurePowerModeReqHandler,
		/*        */ CsrWifiRouterCtrlHipReqHandler,
		/*        */ CsrWifiRouterCtrlMediaStatusReqHandler,
		/*        */ CsrWifiRouterCtrlMulticastAddressResHandler,
		/*        */ CsrWifiRouterCtrlPortConfigureReqHandler,
		/*        */ CsrWifiRouterCtrlQosControlReqHandler,
		/*        */ CsrWifiRouterCtrlSuspendResHandler,
		/*        */ CsrWifiRouterCtrlTclasAddReqHandler,
		/*        */ CsrWifiRouterCtrlResumeResHandler,
		/*        */ CsrWifiRouterCtrlRawSdioDeinitialiseReqHandler,
		/*        */ CsrWifiRouterCtrlRawSdioInitialiseReqHandler,
		/*        */ CsrWifiRouterCtrlTclasDelReqHandler,
		/*        */ CsrWifiRouterCtrlTrafficClassificationReqHandler,
		/*        */ CsrWifiRouterCtrlTrafficConfigReqHandler,
		/*        */ CsrWifiRouterCtrlWifiOffReqHandler,
		/*        */ CsrWifiRouterCtrlWifiOffResHandler,
		/*        */ CsrWifiRouterCtrlWifiOnReqHandler,
		/*        */ CsrWifiRouterCtrlWifiOnResHandler,
		/*        */ CsrWifiRouterCtrlM4TransmitReqHandler,
		/*        */ CsrWifiRouterCtrlModeSetReqHandler,
		/*        */ CsrWifiRouterCtrlPeerAddReqHandler,
		/*        */ CsrWifiRouterCtrlPeerDelReqHandler,
		/*        */ CsrWifiRouterCtrlPeerUpdateReqHandler,
		/*        */ CsrWifiRouterCtrlCapabilitiesReqHandler,
		/*        */ CsrWifiRouterCtrlBlockAckEnableReqHandler,
		/*        */ CsrWifiRouterCtrlBlockAckDisableReqHandler,
		/*        */ CsrWifiRouterCtrlWapiRxPktReqHandler,
		/*        */ CsrWifiRouterCtrlWapiMulticastFilterReqHandler,
		/*        */ CsrWifiRouterCtrlWapiUnicastFilterReqHandler,
		/*        */ CsrWifiRouterCtrlWapiUnicastTxPktReqHandler,
		/*        */ CsrWifiRouterCtrlWapiFilterReqHandler,
};
