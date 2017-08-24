/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*
                                                                              
                                        
  
            
                                                                     
                                      
  
                                                                              
 */
#ifndef __TA_SAMPLING_H__
#define __TA_SAMPLING_H__

#include "csr_wifi_hip_unifi.h"

typedef struct ta_l4stats
{
    u32 rxTcpBytesCount;
    u32 txTcpBytesCount;
    u32 rxUdpBytesCount;
    u32 txUdpBytesCount;
} ta_l4stats_t;

/*
                                                     
 */

typedef struct ta_data
{
    /*                                     */
    u16 packet_filter;

    /*                                            */
    CsrWifiRouterCtrlTrafficFilter custom_filter;

    /*                                                */
    u32 tx_last_ts;

    /*                                             */
    u32 last_indication_time;

    /*            */
    CsrWifiRouterCtrlTrafficStats stats;

    /*                                */
    CsrWifiRouterCtrlTrafficType traffic_type;

    /*                                                                 */
    u32    rx_sum_rate;
    ta_l4stats_t ta_l4stats;
} ta_data_t;


void unifi_ta_sampling_init(card_t *card);

#endif /*                   */
