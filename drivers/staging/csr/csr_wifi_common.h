/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#ifndef CSR_WIFI_COMMON_H__
#define CSR_WIFI_COMMON_H__

#include <linux/types.h>
#include "csr_result.h"

/*             */
typedef struct
{
    u8 a[6];
} CsrWifiMacAddress;

/*              */
typedef struct
{
    u8 a[4];
} CsrWifiIp4Address;

/*              */
typedef struct
{
    u8 a[16];
} CsrWifiIp6Address;

typedef struct
{
    u8 ssid[32];
    u8 length;
} CsrWifiSsid;

/*                                                                              

             
                                             

       
                      
                                                    
                      
                                                                              
                             
                                                                              
                           
                             
                                                                                
                             
                                                                        
                                                                          
                                     
                                                                            
                                      
                           
                                                                            
                                                
                               
                                                                              
                                         
                               
                                                                 
                            
                                                                           
                                  
                                  
                                                                                
                          
                                   
                                                                              
                                                           
                                         
                                                             
                                           
                                                                                            
                                        
                                                                              */
#define CSR_WIFI_RESULT_NOT_FOUND                 ((CsrResult) 0x0001)
#define CSR_WIFI_RESULT_TIMED_OUT                 ((CsrResult) 0x0002)
#define CSR_WIFI_RESULT_CANCELLED                 ((CsrResult) 0x0003)
#define CSR_WIFI_RESULT_INVALID_PARAMETER         ((CsrResult) 0x0004)
#define CSR_WIFI_RESULT_NO_ROOM                   ((CsrResult) 0x0005)
#define CSR_WIFI_RESULT_UNSUPPORTED               ((CsrResult) 0x0006)
#define CSR_WIFI_RESULT_UNAVAILABLE               ((CsrResult) 0x0007)
#define CSR_WIFI_RESULT_WIFI_OFF                  ((CsrResult) 0x0008)
#define CSR_WIFI_RESULT_SECURITY_ERROR            ((CsrResult) 0x0009)
#define CSR_WIFI_RESULT_MIB_SET_FAILURE           ((CsrResult) 0x000A)
#define CSR_WIFI_RESULT_INVALID_INTERFACE_TAG     ((CsrResult) 0x000B)
#define CSR_WIFI_RESULT_P2P_NOA_CONFIG_CONFLICT   ((CsrResult) 0x000C)

#define CSR_WIFI_VERSION	"5.1.0.0"

#endif

