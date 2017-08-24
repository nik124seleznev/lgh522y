/*
 * Copyright 2012 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */
#ifndef _HV_IORPC_H_
#define _HV_IORPC_H_

/* 
  
                                                             
  
                                                                  
                                                                      
                                                                    
                                                                    
               
  
                                                                   
                                                                      
                                                                  
                      
  
                                                                  
                                                             
                                                                      
                                                                     
                                                                
                                                             
            
  
                                                                  
                                                                 
                                                                   
                                                                   
                                                                 
                                                                     
                                                                    
                                                                      
                                                              
  
                                         
  
                                                                    
                                                               
                                                                 
                                                               
                                                                  
                                                                      
                                                                   
                                                                 
                                                                 
                          
  
                                                                      
                                                              
                                                                   
                                                                   
                                                                      
              
  
                                                                   
                                                                    
                                                                      
                                                                     
                                                                    
  
                                                     
  
                                                                
                                                                  
                                                                  
                                                                 
             
  
                                                                
                                                               
                                                                    
                                                                     
                                                                      
                                                                     
                                                                   
                                                                    
                                                                     
             
  
                                                                    
                                                           
                                                                  
                                                                   
                                                         
  
                                         
  
                                                                    
                                                                    
                                                               
                                                                     
                                                                    
                                                               
           
  
                                          
  
                                                                   
                                                                     
                                                               
                                                                   
                                                                    
                                                                    
                                                                      
                                                                      
                                                                      
                                                      
  
                                                           
                                                                    
                                                                 
                                                                     
                                                              
                             
  
                                                                    
                                                                
                                                                      
                                                                    
                                                               
  
                                                               
                                                                  
                                                                     
                                                               
          
  
                                                                    
                                                                  
                                                                  
                                                                  
                                                                   
                                                                     
                                                                      
                        
  
                                        
  
                                                                 
                                                                      
                                                               
                                                                    
                                                                    
  
                                              
  
                                                                      
                                                                    
                                                                       
                                                                         
                                                                       
                                                                    
                                                                        
                                                                          
        
  
                                  
  
                                                                     
                                                                       
                                                                           
                                                                        
                                                                  
                                                                     
                                                                 
  
                              
  
                                                                 
                                                                   
                                                                
                                                                     
                                                                     
                                                                   
                                                                    
                               
  
                                                          
  
                                                                      
                                                                           
                                                                           
                                                                           
                                                                           
                                                                       
                                                                          
                                                                        
                                                                           
                                                                           
                                                                           
                                                                  
                                                                         
                                                                  
  
                                                                      
                                                                 
                                                                     
                                                                    
                                                                     
                                                                      
                                                                   
                          
  
                                            
  
                                                                  
                                                                
                                                                
                                                                      
                                                                      
                                                               
                                                                
                                  
  
                                                           
  
                                                                   
                                                                    
                                                                     
                                                               
                                                                    
                                                                     
                                                                     
                                                            
 */

#ifdef __KERNEL__
#include <linux/stddef.h>
#else
#include <stddef.h>
#endif

#if defined(__HV__)
#include <hv/hypervisor.h>
#elif defined(__KERNEL__)
#include <hv/hypervisor.h>
#include <linux/types.h>
#else
#include <stdint.h>
#endif


/*                                                                    
                                                                     
                                                               
 */
enum iorpc_format_e
{
  /*                                             */
  IORPC_FORMAT_NONE,

  /*                                                              
                                 */
  IORPC_FORMAT_NONE_NOUSER,

  /*                                           */
  IORPC_FORMAT_USER_MEM,

  /*                                                     */
  IORPC_FORMAT_KERNEL_MEM,

  /*                                    */
  IORPC_FORMAT_KERNEL_INTERRUPT,

  /*                                               */
  IORPC_FORMAT_USER_INTERRUPT,

  /*                                                               */
  IORPC_FORMAT_KERNEL_POLLFD_SETUP,

  /*                                                                    */
  IORPC_FORMAT_USER_POLLFD_SETUP,

  /*                                                    */
  IORPC_FORMAT_KERNEL_POLLFD,

  /*                                                              */
  IORPC_FORMAT_USER_POLLFD,
};


/*                                            */
#define IORPC_OPCODE(FORMAT, CODE) (((FORMAT) << 16) | (CODE))

/*                                                               
                                                              */
union iorpc_offset
{
#ifndef __BIG_ENDIAN__
  uint64_t offset;              /*             */

  struct
  {
    uint16_t code;              /*             */
    uint16_t format;            /*                  */
    uint32_t sub_offset;        /*                            */
  };

  uint32_t opcode;              /*                                  */
#else
  uint64_t offset;              /*             */

  struct
  {
    uint32_t sub_offset;        /*                            */
    uint16_t format;            /*                  */
    uint16_t code;              /*             */
  };

  struct
  {
    uint32_t padding;
    uint32_t opcode;              /*                                  */
  };
#endif
};


/*                                                                */
struct iorpc_mem_attr
{
  unsigned int lotar_x:4;       /*                                   */
  unsigned int lotar_y:4;       /*                                     */
  unsigned int hfh:1;           /*                       */
  unsigned int nt_hint:1;       /*                      */
  unsigned int io_pin:1;        /*                              */
};

/*                       */
#define IORPC_MEM_BUFFER_FLAG_NT_HINT (1 << 0)

/*                      */
#define IORPC_MEM_BUFFER_FLAG_IO_PIN (1 << 1)


/*                                                              
                                                                
                                                                   
                                                           
                                                                   
                                                                */
union iorpc_mem_buffer
{
  struct
  {
    uint64_t va;                /*                         */
    uint64_t size;              /*                */
    unsigned int flags;         /*                    */
  }
  user;                         /*                                     */

  struct
  {
    unsigned long long cpa;     /*                            */
#if defined(__KERNEL__) || defined(__HV__)
    size_t size;                /*                */
    HV_PTE pte;                 /*                                 */
#else
    uint64_t size;
    uint64_t pte;
#endif
    unsigned int flags;         /*                    */
  }
  kernel;                       /*                                  */

  struct
  {
    unsigned long long pa;      /*                     */
    size_t size;                /*                */
    struct iorpc_mem_attr attr;      /*                                  */
  }
  hv;                           /*                                    */
};


/*                                                                       
                                                                          
                                               */
union iorpc_interrupt
{
  struct
  {
    int cpu;   /*        */
    int event; /*           */
  }
  user;        /*                                                */

  struct
  {
    int x;     /*            */
    int y;     /*            */
    int ipi;   /*           */
    int event; /*           */
  }
  kernel;      /*                                         */

};


/*                                                                       
                                                                         
                                                                          
                           */
union iorpc_pollfd_setup
{
  struct
  {
    int fd;    /*                             */
  }
  user;        /*                                                   */

  struct
  {
    int x;     /*            */
    int y;     /*            */
    int ipi;   /*           */
    int event; /*           */
  }
  kernel;      /*                                            */

};


/*                                                                     
                                                                       
                                                                            
                                               */
union iorpc_pollfd
{
  struct
  {
    int fd;    /*                             */
  }
  user;        /*                                             */

  struct
  {
    int cookie; /*                                                     */
  }
  kernel;      /*                                      */

};


/*                                                                
  
                                                                   
                                                                 
                                                    
 */
enum gxio_err_e {

  /*                              */
  GXIO_ERR_MAX = -1101,


  /*                                                      */
  /*                                                      */
  /*                                                      */

  /*                                                     */
  GXIO_ERR_OPCODE = -1101,

  /*                     */
  GXIO_ERR_INVAL = -1102,

  /*                                                     */
  GXIO_ERR_ALIGNMENT = -1103,

  /*                                                 */
  GXIO_ERR_COHERENCE = -1104,

  /*                                */
  GXIO_ERR_ALREADY_INIT = -1105,

  /*                                */
  GXIO_ERR_NO_SVC_DOM = -1106,

  /*                                 */
  GXIO_ERR_INVAL_SVC_DOM = -1107,

  /*                        */
  GXIO_ERR_MMIO_ADDRESS = -1108,

  /*                             */
  GXIO_ERR_INTERRUPT = -1109,

  /*                              */
  GXIO_ERR_CLIENT_MEMORY = -1110,

  /*                         */
  GXIO_ERR_IOTLB_ENTRY = -1111,

  /*                       */
  GXIO_ERR_INVAL_MEMORY_SIZE = -1112,

  /*                         */
  GXIO_ERR_UNSUPPORTED_OP = -1113,

  /*                            */
  GXIO_ERR_DMA_CREDITS = -1114,

  /*                       */
  GXIO_ERR_TIMEOUT = -1115,

  /*                            */
  GXIO_ERR_NO_DEVICE = -1116,

  /*                           */
  GXIO_ERR_BUSY = -1117,

  /*             */
  GXIO_ERR_IO = -1118,

  /*                     */
  GXIO_ERR_PERM = -1119,



  /*                                                      */
  /*                                                      */
  /*                                                      */

  /*                           */
  GXIO_TEST_ERR_REG_NUMBER = -1120,

  /*                       */
  GXIO_TEST_ERR_BUFFER_SLOT = -1121,


  /*                                                      */
  /*                                                      */
  /*                                                      */


  /*                       */
  GXIO_MPIPE_ERR_INVAL_BUFFER_SIZE = -1131,

  /*                                */
  GXIO_MPIPE_ERR_NO_BUFFER_STACK = -1140,

  /*                               */
  GXIO_MPIPE_ERR_BAD_BUFFER_STACK = -1141,

  /*                             */
  GXIO_MPIPE_ERR_NO_NOTIF_RING = -1142,

  /*                            */
  GXIO_MPIPE_ERR_BAD_NOTIF_RING = -1143,

  /*                              */
  GXIO_MPIPE_ERR_NO_NOTIF_GROUP = -1144,

  /*                             */
  GXIO_MPIPE_ERR_BAD_NOTIF_GROUP = -1145,

  /*                          */
  GXIO_MPIPE_ERR_NO_BUCKET = -1146,

  /*                         */
  GXIO_MPIPE_ERR_BAD_BUCKET = -1147,

  /*                             */
  GXIO_MPIPE_ERR_NO_EDMA_RING = -1148,

  /*                            */
  GXIO_MPIPE_ERR_BAD_EDMA_RING = -1149,

  /*                          */
  GXIO_MPIPE_ERR_BAD_CHANNEL = -1150,

  /*                     */
  GXIO_MPIPE_ERR_BAD_CONFIG = -1151,

  /*                */
  GXIO_MPIPE_ERR_IQUEUE_EMPTY = -1152,

  /*               */
  GXIO_MPIPE_ERR_RULES_EMPTY = -1160,

  /*              */
  GXIO_MPIPE_ERR_RULES_FULL = -1161,

  /*                 */
  GXIO_MPIPE_ERR_RULES_CORRUPT = -1162,

  /*                 */
  GXIO_MPIPE_ERR_RULES_INVALID = -1163,

  /*                         */
  GXIO_MPIPE_ERR_CLASSIFIER_TOO_BIG = -1170,

  /*                             */
  GXIO_MPIPE_ERR_CLASSIFIER_TOO_COMPLEX = -1171,

  /*                             */
  GXIO_MPIPE_ERR_CLASSIFIER_BAD_HEADER = -1172,

  /*                               */
  GXIO_MPIPE_ERR_CLASSIFIER_BAD_CONTENTS = -1173,

  /*                                         */
  GXIO_MPIPE_ERR_CLASSIFIER_INVAL_SYMBOL = -1174,

  /*                                         */
  GXIO_MPIPE_ERR_CLASSIFIER_INVAL_BOUNDS = -1175,

  /*                                             */
  GXIO_MPIPE_ERR_CLASSIFIER_INVAL_RELOCATION = -1176,

  /*                                           */
  GXIO_MPIPE_ERR_CLASSIFIER_UNDEF_SYMBOL = -1177,


  /*                                                      */
  /*                                                      */
  /*                                                      */

  /*                                     */
  GXIO_TRIO_ERR_NO_MEMORY_MAP = -1180,

  /*                                    */
  GXIO_TRIO_ERR_BAD_MEMORY_MAP = -1181,

  /*                                 */
  GXIO_TRIO_ERR_NO_SCATTER_QUEUE = -1182,

  /*                                */
  GXIO_TRIO_ERR_BAD_SCATTER_QUEUE = -1183,

  /*                                 */
  GXIO_TRIO_ERR_NO_PUSH_DMA_RING = -1184,

  /*                               */
  GXIO_TRIO_ERR_BAD_PUSH_DMA_RING = -1185,

  /*                                 */
  GXIO_TRIO_ERR_NO_PULL_DMA_RING = -1186,

  /*                               */
  GXIO_TRIO_ERR_BAD_PULL_DMA_RING = -1187,

  /*                              */
  GXIO_TRIO_ERR_NO_PIO = -1188,

  /*                            */
  GXIO_TRIO_ERR_BAD_PIO = -1189,

  /*                        */
  GXIO_TRIO_ERR_NO_ASID = -1190,

  /*                */
  GXIO_TRIO_ERR_BAD_ASID = -1191,


  /*                                                      */
  /*                                                      */
  /*                                                      */

  /*                            */
  GXIO_MICA_ERR_BAD_ACCEL_TYPE = -1220,

  /*                           */
  GXIO_MICA_ERR_NO_CONTEXT = -1221,

  /*                                                        */
  GXIO_MICA_ERR_PKA_CMD_QUEUE_FULL = -1222,

  /*                                                                */
  GXIO_MICA_ERR_PKA_RESULT_QUEUE_EMPTY = -1223,

  /*                                                      */
  /*                                                      */
  /*                                                      */

  /*                                                                
                                                         */
  GXIO_GPIO_ERR_PIN_UNAVAILABLE = -1240,

  /*                                                                   
                                                            */
  GXIO_GPIO_ERR_PIN_BUSY = -1241,

  /*                                                              
                                                                 
               */
  GXIO_GPIO_ERR_PIN_UNATTACHED = -1242,

  /*                                                                
                                                                
                                    */
  GXIO_GPIO_ERR_PIN_INVALID_MODE = -1243,

  /*                               */
  GXIO_ERR_MIN = -1299
};


#endif /*               */
