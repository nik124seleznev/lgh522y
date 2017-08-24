/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_ROUTER_CONVERTER_INIT_H__
#define CSR_WIFI_ROUTER_CONVERTER_INIT_H__

#ifndef EXCLUDE_CSR_WIFI_ROUTER_MODULE

#include "csr_msgconv.h"

#ifdef CSR_LOG_ENABLE
#include "csr_log.h"

extern const CsrLogPrimitiveInformation* CsrWifiRouterTechInfoGet(void);
#endif /*                */

extern void CsrWifiRouterConverterInit(void);

#else /*                                */

#define CsrWifiRouterConverterInit()

#endif /*                                */

#endif /*                                    */
