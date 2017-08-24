/*
 *****************************************************************************
 *
 * FILE : unifi_clients.h
 *
 * PURPOSE : Private header file for unifi clients.
 *
 *           UDI = UniFi Debug Interface
 *
 * Copyright (C) 2005-2008 by Cambridge Silicon Radio Ltd.
 *
 * Refer to LICENSE.txt included with this source code for details on
 * the license terms.
 *
 *****************************************************************************
 */
#ifndef __LINUX_UNIFI_CLIENTS_H__
#define __LINUX_UNIFI_CLIENTS_H__ 1

#include <linux/kernel.h>

#define MAX_UDI_CLIENTS 8

/*                                                                */
#define UDI_SENDER_ID_BASE      0xC000
#define UDI_SENDER_ID_SHIFT     8


/*                                       */
typedef struct {

    /*                     */
    struct list_head q;

    /*                                                 */
    udi_msg_t msg;

    /*                             */

} udi_log_t;



typedef struct ul_client ul_client_t;

typedef void (*udi_event_t)(ul_client_t *client,
                            const u8 *sigdata, int signal_len,
                            const bulk_data_param_t *bulkdata,
                            int dir);

void logging_handler(void *ospriv,
                     u8 *sigdata, u32 signal_len,
                     const bulk_data_param_t *bulkdata,
                     enum udi_log_direction direction);


/*
                                         
                                                             
 */
typedef struct _bulk_data
{
    unsigned char ptr[2000];
    unsigned int length;
} bulk_data_t;


struct ul_client {
    /*                                               */
    int client_id;

    /*                                                         */
    int instance;

    /*                                                   */
    int udi_enabled;

    /*                                         */
    int sender_id;

    /*                                                  */
    unsigned int configuration;

    udi_event_t event_hook;

    /*                                                                  */
    struct list_head udi_log;

    /*                                       */
    struct semaphore udi_sem;

    /*
                                                         
                                                    
                                                  
     */
    wait_queue_head_t udi_wq;
    CSR_SIGNAL* reply_signal;
    bulk_data_t* reply_bulkdata[UNIFI_MAX_DATA_REFERENCES];

    u16 signal_filter[SIG_FILTER_SIZE];


    /*                                                                     */
    /*                                                              */

    /*                                                     */
    int wake_up_wq_id;

    /*
                                                              
                                                                       
                                                                    
                                                          
     */
    unsigned char seq_no;

    /*
                                                               
                                                     
                                                          
     */
    unsigned char wake_seq_no;

    unifiio_snap_filter_t snap_filter;
}; /*                  */


#endif /*                           */
