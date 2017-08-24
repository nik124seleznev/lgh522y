/*                                                                            
  
            
            
                
  
   
          
          
                           
  
                                                                            */

#ifndef __EEMCS_IPC_H
#define __EEMCS_IPC_H
#include <asm/ioctl.h>
#include <asm/atomic.h>
#include <linux/skbuff.h>
#include <linux/wait.h>
#include "eemcs_ccci.h"
#include "eemcs_kal.h"

#define EEMCS_IPC_MAJOR 183 
#define EEMCS_IPC_MINOR_BASE 100 
#define EEMCS_IPC_PORT CCCI_PORT_IPC
#define EEMCS_IPC_NAME "eemcs_ipc"
#define EEMCS_IPCD_MAX_NUM (10) 
#define EEMCS_IPC_NAME_LEN (30)
#define CCCI_IPC_MAGIC 'P'
#define CCCI_IPC_RESET_RECV	        _IO(CCCI_IPC_MAGIC,0)
#define CCCI_IPC_RESET_SEND	        _IO(CCCI_IPC_MAGIC,1)
#define CCCI_IPC_WAIT_MD_READY	    _IO(CCCI_IPC_MAGIC,2)
#define CCCI_IPC_KERN_WRITE_TEST    _IO(CCCI_IPC_MAGIC,3)

/*                               */
typedef enum
{
    IPC_L4C_MSG_ID_BEGIN = 0x80000000,
    IPC_L4C_MSG_ID_RANGE = 0x80,
    IPC_EL1_MSG_ID_BEGIN = IPC_L4C_MSG_ID_BEGIN + IPC_L4C_MSG_ID_RANGE,
    IPC_EL1_MSG_ID_RANGE = 0x20,
    
}EEMCS_IPC_MSG_ID_RANGE;

typedef enum
{
    IPC_L4C_MSG_ID_INVALID = IPC_L4C_MSG_ID_BEGIN,
    //                                              
    //                         
    //                                             
    IPC_L4C_MSG_ID_END,
    IPC_EL1_MSG_ID_INVALID = IPC_EL1_MSG_ID_BEGIN,
    #include "eemcs_ipc_el1_msg_id.h"
    IPC_EL1_MSG_ID_END,    
}EEMCS_IPC_MSG_ID_CODE;



/*                       */
typedef unsigned int uint32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef struct {
	uint8  ref_count;
	uint8  lp_reserved;
	uint16 msg_len;
	uint8 data[0];
} local_para_struct ;

typedef struct {
   uint16	pdu_len; 
   uint8	ref_count; 
   uint8   	pb_resvered; 
   uint16	free_header_space; 
   uint16	free_tail_space;
   uint8	data[0];
}peer_buff_struct ;

typedef struct ipc_ilm_struct 
{
    uint32           src_mod_id;
    uint32           dest_mod_id;
    uint32           sap_id;
    uint32           msg_id;
    local_para_struct    *local_para_ptr;
    peer_buff_struct     *peer_buff_ptr;
}ipc_ilm_t;

/*                        */
typedef enum _EEMCS_IPC_STATE{
    IPCD_CLOSE   = 0,
    IPCD_OPEN    = 1,
    IPCD_KERNEL  = 2
}EEMCS_IPC_STATE;

/*                           */
typedef struct _eemcs_ipc_node_t
{
    KAL_CHAR            dev_name[32];
    KAL_UINT8           ipc_node_id;
    atomic_t            dev_state;  /*               */
    /*                */
    /*                                                 
                                                
                                                   
                                                                      
                                                
                                                      
     */
    wait_queue_head_t   rx_waitq;
    struct sk_buff_head rx_skb_list;
    atomic_t            rx_pkt_cnt;
    atomic_t            rx_pkt_drop_cnt; /*                                       */
    struct fasync_struct *fasync;
    /*                */    
    wait_queue_head_t   tx_waitq;
    atomic_t            tx_pkt_cnt;
}eemcs_ipc_node_t;


/*                               */
typedef struct _eemcs_ipc_inst_t
{
    struct class        *dev_class;        /*                                                         */
    struct cdev         *eemcs_ipcdev;     /*                                        */
    KAL_UINT8           eemcs_port_id;
    ccci_ch_set         ccci_ch;
    eemcs_ipc_node_t    ipc_node[EEMCS_IPCD_MAX_NUM];
    wait_queue_head_t   state_waitq;
    int                 md_is_ready;
}eemcs_ipc_inst_t;

/*                        */
typedef struct IPC_MSGSVC_TASKMAP_STRUCT
{
    uint32  extq_id;            /*                                      */
    uint32  task_id;            /*                                */
        
}IPC_MSGSVC_TASKMAP_T;

#ifdef _EEMCS_IPCD_LB_UT_
KAL_UINT32 ipcdut_register_callback(CCCI_CHANNEL_T chn, EEMCS_CCCI_CALLBACK func_ptr , KAL_UINT32 private_data) ;
KAL_UINT32 ipcdut_unregister_callback(CCCI_CHANNEL_T chn);
inline KAL_INT32 ipcdut_UL_write_skb_to_swq(CCCI_CHANNEL_T chn, struct sk_buff *skb);
inline KAL_UINT32 ipcdut_UL_write_room_check(CCCI_CHANNEL_T chn);

#define ccci_ch_register(ch_num,cb,para)           ipcdut_register_callback(ch_num,cb,para) 
#define ccci_ch_unregister(ch_num)			     ipcdut_unregister_callback(ch_num) 
#define ccci_ch_write_desc_to_q(ch_num,desc_p)     ipcdut_UL_write_skb_to_swq(ch_num, desc_p)
#define ccci_ch_write_space_alloc(ch_num)          ipcdut_UL_write_room_alloc(ch_num)
//                                                                                  
#else
#define ccci_ch_register(ch_num,cb,para)           eemcs_ccci_register_callback(ch_num,cb,para) 
#define ccci_ch_unregister(ch_num)			     eemcs_ccci_unregister_callback(ch_num) 
#define ccci_ch_write_desc_to_q(ch_num,desc_p)     eemcs_ccci_UL_write_skb_to_swq(ch_num, desc_p)
#define ccci_ch_write_space_alloc(ch_num)          eemcs_ccci_UL_write_room_alloc(ch_num)
#endif

#ifdef CCCI_SDIO_HEAD
#define CCCI_IPC_HEADER_ROOM                   (sizeof(SDIO_H)+sizeof(CCCI_BUFF_T))
#else
#define CCCI_IPC_HEADER_ROOM                   (sizeof(CCCI_BUFF_T))
#endif

//                                                                
#define ccci_ipc_mem_alloc(sz, flag)            __dev_alloc_skb(sz, flag)

ssize_t eemcs_ipc_kern_write(ipc_ilm_t *in_ilm);
KAL_INT32 eemcs_ipc_mod_init(void);
void eemcs_ipc_exit(void);

#endif
