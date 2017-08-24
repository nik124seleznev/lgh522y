/*                                   
 
                      
 
                                 
                                 
                                  
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                    
 
                           
 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                  
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                  
                                 
                                 
                                 
                                 
                                     
 
                         
 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                 
                                   
 
                                         
                                                                                       
                                                                                             
 
                                
                                
                                
                                
                                
                                
                                
                                
                                
                                  */

/*                                                    
                                                                                  
                                                                                               
                                                                                  
 
                                                             
                                                                                              
                                                             
                                                                              
                                                             
                                               */

#ifndef __CCCI_CCMNI_H__
#define __CCCI_CCMNI_H__
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/bitops.h>
#include <linux/wakelock.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/dma-mapping.h>
#include <linux/timer.h>
#include <linux/if_ether.h>
#include <asm/bitops.h>
#include <asm/dma-mapping.h>
#include <mach/mt_typedefs.h>

#define  CCCI_NETWORK           0
#define  CCMNI_MTU              1500

#define  CCMNI_CHANNEL_OFFSET   2
//                                              
//                                                                                  


#define  IPV4_VERSION 0x40
#define  IPV6_VERSION 0x60

//                        
#define CCMNI_RECV_ACK_PENDING        (0)
#define CCMNI_SEND_PENDING            (1)
#define CCMNI_RECV_PENDING            (2)

#define  CCMNI_TX_QUEUE         8
//                                                
//                                                                                
//                                                  
#define CCMNI_CTRL_Q_RX_SIZE    (256)
#define CCMNI_CTRL_Q_TX_SIZE    (64)

#define CCMNI_RX_ACK_BOUND         (CCMNI_CTRL_Q_RX_SIZE_DEFAULT/4)

#define CCMNI_CTRL_Q_RX_SIZE_DEFAULT    100

#define CCMNI_SINGLE_BUFF_SIZE        (CCMNI_MTU + 28)
#define CCMNI_BUFF_DATA_FIELD_SIZE    (CCMNI_SINGLE_BUFF_SIZE -24)    //        

#define CCCI_CCMNI_SMEM_UL_SIZE        (300 * 1024)
#define CCCI_CCMNI_SMEM_DL_SIZE        (300 * CCMNI_SINGLE_BUFF_SIZE)

//                    
#define RX_BUF_RESOURCE_LOWER_BOUND (20)
#define RX_BUF_RESOURCE_UPPER_BOUND (80)    //                                                
#define RX_BUF_RESOURCE_OCCUPY_CNT  (20)    //                                                                


typedef struct
{
    unsigned port;
    unsigned avai_in_no;
    unsigned avai_out_no;
    unsigned read_out_no;
} dbg_info_ccmni_t;

#define CCMNI_BUFF_DBG_INFO_SIZE    (sizeof(dbg_info_ccmni_t))
#define CCMNI_BUFF_HEADER_SIZE        (4)
#define CCMNI_BUFF_FOOTER_SIZE        (4)

#define CCMNI_BUFF_HEADER            (0xE1E1E1E1)
#define CCMNI_BUFF_FOOTER            (0xE2E2E2E2)
#define CCMNI_DATA_END                (0xE3)

typedef struct
{
    unsigned char *ptr;
    unsigned len;
} q_ringbuf_ccmni_t;

typedef struct
{
    unsigned avai_in;
    unsigned avai_out;
    unsigned read_out;
    unsigned q_length;        //                  
} buffer_control_ccmni_t;

typedef struct
{
    buffer_control_ccmni_t    rx_control;                                //          
    q_ringbuf_ccmni_t        q_rx_ringbuff[CCMNI_CTRL_Q_RX_SIZE];    //                      
    buffer_control_ccmni_t    tx_control;                                //       
    q_ringbuf_ccmni_t        q_tx_ringbuff[CCMNI_CTRL_Q_TX_SIZE];    //                   
} shared_mem_ccmni_t;
    
#define CCCI_CCMNI_SMEM_SIZE     (sizeof(shared_mem_ccmni_t))
#define CCMNI_DL_CTRL_MEM_SIZE    ((sizeof(buffer_control_ccmni_t)) + (CCMNI_CTRL_Q_RX_SIZE * (sizeof(q_ringbuf_ccmni_t))))
#define CCMNI_UL_CTRL_MEM_SIZE    ((sizeof(buffer_control_ccmni_t)) + (CCMNI_CTRL_Q_TX_SIZE * (sizeof(q_ringbuf_ccmni_t))))

#endif //                  