/*
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

#ifndef TLDAPC_H_
#define TLDAPC_H_

#include "tci.h"

/*
                                                                 
 */
#define CMD_DEVINFO_GET     1
#define CMD_DAPC_SET        2
#define CMD_HACC_REQUEST    3
#define CMD_TPLAY_REQUEST   4
#define CMD_TPLAY_DUMP_PHY  5



/*
                    
 */
#define EXIT_ERROR                  ((uint32_t)(-1))

/*
                   
  
                                            
                                                           
 */
typedef struct {
    tciCommandHeader_t  header;     /*                  */
    uint32_t            len;        /*                                       */
    uint32_t            respLen;    /*                             */
} dapc_cmd_t;

/*
                                                     
 */
typedef struct {
    tciResponseHeader_t header;     /*                   */
    uint32_t            len;
} dapc_rsp_t;

/*
                    
 */
typedef struct {
    union {
      dapc_cmd_t     cmd;
      dapc_rsp_t     rsp;
    };
    uint32_t    index;
    uint32_t    result;
    uint32_t    data_addr;
    uint32_t    data_len;
    uint32_t    seed_addr;
    uint32_t    seed_len;
    uint32_t    hacc_user;
    uint32_t    direction;
    uint32_t    tplay_handle_low_addr;    
    uint32_t    tplay_handle_high_addr;
} dapc_tciMessage_t;

/*
                 
 */
#define TL_DAPC_UUID { { 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }

#endif //         
