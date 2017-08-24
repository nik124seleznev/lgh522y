/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_SME_CONVERTER_INIT_H__
#define CSR_WIFI_SME_CONVERTER_INIT_H__

#ifndef EXCLUDE_CSR_WIFI_SME_MODULE

#include "csr_msgconv.h"

#ifdef CSR_LOG_ENABLE
#include "csr_log.h"

extern const CsrLogPrimitiveInformation* CsrWifiSmeTechInfoGet(void);
#endif /*                */

extern void CsrWifiSmeConverterInit(void);

#else /*                             */

#define CsrWifiSmeConverterInit()

#endif /*                             */

#endif /*                                 */
