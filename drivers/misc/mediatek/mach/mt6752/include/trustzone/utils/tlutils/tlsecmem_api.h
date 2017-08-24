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

#ifndef TLSECMEM_H_
#define TLSECMEM_H_

#include "tci.h"

/*
                                                                 
 */
#define CMD_SEC_MEM_ALLOC         1
#define CMD_SEC_MEM_REF           2
#define CMD_SEC_MEM_UNREF         3
#define CMD_SEC_MEM_ALLOC_TBL     4
#define CMD_SEC_MEM_UNREF_TBL     5
#define CMD_SEC_MEM_USAGE_DUMP    6
#define CMD_SEC_MEM_SYS_TRACE     7




/*
                    
 */
#define EXIT_ERROR                  ((uint32_t)(-1))

/*
                   
  
                                            
                                                           
 */
typedef struct {
    tciCommandHeader_t  header;     /*                  */
    uint32_t            len;        /*                                       */
    uint32_t            respLen;    /*                             */
} tl_cmd_t;

/*
                                                     
 */
typedef struct {
    tciResponseHeader_t header;     /*                   */
    uint32_t            len;
} tl_rsp_t;

/*
                    
 */
typedef struct {
    union {
      tl_cmd_t     cmd_secmem;
      tl_rsp_t     rsp_secmem;
    };
    uint32_t    alignment;  /*    */        
    uint32_t    size;       /*    */        
    uint32_t    refcount;   /*       */        
    uint32_t    sec_handle; /*     */
    uint32_t    ResultData;
    uint32_t        systrace_event; /*    */
    unsigned long   systrace_bufpa; /*    */
    unsigned long   systrace_size;  /*    */
    uint32_t        systrace_head;  /*       */
} tciMessage_t;

/*
                 
 */
#define TL_SECMEM_UUID { 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

#endif //            
