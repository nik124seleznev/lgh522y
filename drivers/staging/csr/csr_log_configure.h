#ifndef CSR_LOG_CONFIGURE_H__
#define CSR_LOG_CONFIGURE_H__
/*****************************************************************************

  (c) Cambridge Silicon Radio Limited 2010
  All rights reserved and confidential information of CSR

  Refer to LICENSE.txt included with this source for details
  on the license terms.

 *****************************************************************************/

#include "csr_log.h"

/*                                            */
/*                                            */
/*                                            */
typedef u32 CsrLogLevelText;
#define CSR_LOG_LEVEL_TEXT_OFF       ((CsrLogLevelText) 0x0000)

#define CSR_LOG_LEVEL_TEXT_CRITICAL  ((CsrLogLevelText) 0x0001)
#define CSR_LOG_LEVEL_TEXT_ERROR     ((CsrLogLevelText) 0x0002)
#define CSR_LOG_LEVEL_TEXT_WARNING   ((CsrLogLevelText) 0x0004)
#define CSR_LOG_LEVEL_TEXT_INFO      ((CsrLogLevelText) 0x0008)
#define CSR_LOG_LEVEL_TEXT_DEBUG     ((CsrLogLevelText) 0x0010)

#define CSR_LOG_LEVEL_TEXT_ALL       ((CsrLogLevelText) 0xFFFF)

/*                                                                                                     
                                                                                                   
                                                                                                                
                                                                                                           
                                                                                                             
                                                                                                            
                                                                                                          
                                                                                     */
typedef u32 CsrLogTextTaskId;

#endif
