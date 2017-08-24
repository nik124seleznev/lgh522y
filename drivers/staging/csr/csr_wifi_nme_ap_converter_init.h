/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_NME_AP_CONVERTER_INIT_H__
#define CSR_WIFI_NME_AP_CONVERTER_INIT_H__

#ifndef CSR_WIFI_NME_ENABLE
#error CSR_WIFI_NME_ENABLE MUST be defined inorder to use csr_wifi_nme_ap_converter_init.h
#endif
#ifndef CSR_WIFI_AP_ENABLE
#error CSR_WIFI_AP_ENABLE MUST be defined inorder to use csr_wifi_nme_ap_converter_init.h
#endif

#ifndef EXCLUDE_CSR_WIFI_NME_AP_MODULE

#include "csr_msgconv.h"

#ifdef CSR_LOG_ENABLE
#include "csr_log.h"

extern const CsrLogPrimitiveInformation* CsrWifiNmeApTechInfoGet(void);
#endif /*                */

extern void CsrWifiNmeApConverterInit(void);

#else /*                                */

#define CsrWifiNmeApConverterInit()

#endif /*                                */

#endif /*                                    */
