/*                                                                            
  
            
            
                 
  
           
           
         
  
               
               
                            
  
                                                                            */

#ifndef __CCCI_LAYER_H__
#define __CCCI_LAYER_H__

#include <linux/kfifo.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/kfifo.h>
#include <linux/io.h>
#include <asm/atomic.h>
#include <linux/rwlock.h>
#include <asm/bitops.h>
#include <linux/sched.h>
#include <linux/wakelock.h>
#include <ccci_ch.h>
#include <ccif.h>


#define CCCI_DEV_NAME "ccci"
//                                             
#define CCCI_SYSFS_INFO "info"
#define CCCI_FIFO_MAX_LEN 8 /*                             */


//                                                                   
#define L_CH_ATTR_TX            (1<<0)        //                                   
#define L_CH_ATTR_PRVLG0        (1<<1)        //                                                               
#define L_CH_ATTR_PRVLG1        (1<<2)        //                                                              
#define L_CH_ATTR_PRVLG2        (1<<3)        //                                                             
#define L_CH_ATTR_DUMMY_WRITE    (1<<4)        //                                                            
#define L_CH_ATTR_OPEN_CLEAR    (1<<5)        //                              
#define L_CH_DROP_TOLERATED        (1<<6)        //                                           
#define L_CH_MUST_RDY_FOR_BOOT    (1<<7)        //                                                

//                                                                               
//                                                                                                     
//                                                                   


//                                                              
/*                                  */
#define CCCI_INIT_MAILBOX(buff, mailbox_id) \
        do {    \
            ((ccci_msg_t*)buff)->magic = 0xFFFFFFFF; \
            ((ccci_msg_t*)buff)->id = (mailbox_id);  \
            ((ccci_msg_t*)buff)->channel = CCCI_INVALID_CH_ID;  \
            ((ccci_msg_t*)buff)->reserved = 0;    \
        } while (0)

/*                                 */
#define CCCI_INIT_STREAM(buff, stream_addr, stream_len) \
        do {    \
            ((ccci_msg_t*)buff)->addr = (stream_addr); \
            ((ccci_msg_t*)buff)->len = (stream_len);  \
            ((ccci_msg_t*)buff)->channel = CCCI_INVALID_CH_ID;  \
            ((ccci_msg_t*)buff)->reserved = 0;    \
        } while (0)


//                                                                 
/*                       */
typedef enum
{
    CCCI_SUCCESS = 0,
    CCCI_FAIL = -EIO,
    CCCI_IN_USE = -EEXIST,
    CCCI_NOT_OWNER = -EPERM,
    CCCI_INVALID_PARAM = -EINVAL,
    CCCI_NO_PHY_CHANNEL = -ENXIO,
    CCCI_IN_INTERRUPT = -EACCES,
    CCCI_IN_IRQ = -EINTR,
    CCCI_MD_NOT_READY = -EBUSY,
    CCCI_MD_IN_RESET = -ESRCH,
    CCCI_RESET_NOT_READY = -ENODEV
}CCCI_RETURNVAL_T;

/*            
 
                
                    
               
                         
                                              
                 
*/

/*                                */
typedef struct
{
    unsigned int magic;   /*            */
    unsigned int id;
} CCCI_MAILBOX_T;

/*                               */
typedef struct
{
    unsigned int addr;
    unsigned int len;
} CCCI_STREAM_T;

/*                               */
typedef struct
{
    unsigned int data[2];
    unsigned int channel;
    unsigned int reserved;
} CCCI_BUFF_T;

/*                                  */
typedef void (*CCCI_CALLBACK)(CCCI_BUFF_T *buff, void *private_data);

/*             */
/*
            
 
                     
                                                                                       

                             
                          
               
*/
/*                        */

typedef enum {
    CCCI_ENABLED=0x0,
    CCCI_RUNNING=0x1,

} CCCI_STATE_T;
typedef struct _CCCI_LOG_T
{
    unsigned long sec;
    unsigned long nanosec;
    CCCI_BUFF_T buff;
    short rx;
} CCCI_LOG_T;

//                               
typedef struct _ccci_mail_box{
    unsigned int magic;   //           
    unsigned int id;
}ccci_mail_box_t;

//                              
typedef struct _ccci_stream_msg{
    unsigned int addr;
    unsigned int len;
}ccci_stream_msg_t;

//                              
typedef struct _ccci_common_msg{
    unsigned int data[2];
}ccci_common_msg_t;

typedef struct _ccci_msg{
    union{
        unsigned int magic;    //                          
        unsigned int addr;    //                      
        unsigned int data0;    //                        
    };
    union{
        unsigned int id;    //                        
        unsigned int len;    //               
        unsigned int data1;    //                        
    };
    unsigned int channel;
    unsigned int reserved;
}ccci_msg_t;

typedef struct dump_debug_info
{
    unsigned int type;
    char *name;
    unsigned int more_info;
    union {
        struct {
            
            char file_name[30];
            int line_num;
            unsigned int parameters[3];
        } assert;    
        struct {
            int err_code1;
                int err_code2;
        
        }fatal_error;
        ccci_msg_t data;
        struct {
            unsigned char execution_unit[9]; //    
            char file_name[30];
            int line_num;
            unsigned int parameters[3];
        }dsp_assert;
        struct {
            unsigned char execution_unit[9];
            unsigned int  code1;
        }dsp_exception;
        struct {
            unsigned char execution_unit[9];
            unsigned int  err_code[2];
        }dsp_fatal_err;
    };
    int *ext_mem;
    size_t ext_size;
    int *md_image;
    size_t md_size;
    void *platform_data;
    void (*platform_call)(void *data);
}DEBUG_INFO_T ;

typedef struct _logic_channel_info{
    unsigned int    m_ch_id;
    unsigned int    m_attrs;
    struct kfifo    m_kfifo;
    unsigned int    m_kfifo_ready;
    char*            m_ch_name;
    char*            m_owner_name;
    void*            m_owner;
    void            (*m_call_back)(void*);
    spinlock_t        m_lock;
    unsigned int    m_register;
    int                m_md_id;
}logic_channel_info_t;

typedef struct _logic_channel_static_info{
    unsigned int    m_ch_id;
    unsigned int    m_kfifo_size;
    char*            m_ch_name;
    unsigned int    m_attrs;
}logic_channel_static_info_t;

typedef struct _logic_dispatch_ctl_block{
    logic_channel_info_t    m_logic_ch_table[CCCI_MAX_CH_NUM];
    ccif_t                    *m_ccif;
    struct tasklet_struct    m_dispatch_tasklet;
    volatile unsigned char    m_has_pending_data;
    volatile unsigned char    m_freezed;
    volatile unsigned char    m_running;
    unsigned int            m_md_id;
    struct wake_lock        m_wakeup_wake_lock;
    char                    m_wakelock_name[16];
    void                    (*m_send_notify_cb)(int, unsigned int);    
    unsigned long            m_last_send_ref_jiffies;    
    unsigned long            m_status_flag;    
    spinlock_t                m_lock;
}logic_dispatch_ctl_block_t;
//                                      


//                                                           
int  register_to_logic_ch(int md_id, int ch, void (*func)(void*), void* owner);
int  un_register_to_logic_ch(int md_id, int ch);
int  logic_layer_reset(int);
int  cal_ring_buffer_free_space(int read_idx, int write_idx, int ring_buf_len);
int  cal_ring_buffer_valid_data_size(int read_idx, int write_idx, int ring_buf_len);
int  ccci_logic_ctlb_init(int);
void ccci_logic_ctlb_deinit(int);
int  get_logic_ch_data(logic_channel_info_t *ch_info, ccci_msg_t *msg);
int  get_logic_ch_data_len(logic_channel_info_t *ch_info);
logic_channel_info_t* get_logic_ch_info(int md_id, int ch_id);
void freeze_logic_layer_tx(int);
void freeze_all_logic_layer(int);
//                                             
void * get_dispatch_ctl_block(int md_sys);
int  ccci_message_send(int md_id, ccci_msg_t *msg, int retry_en);
void ccci_system_message(int md_id, unsigned int msg, unsigned int resv);
void ccci_disable_md_intr(int md_id);
void ccci_enable_md_intr(int md_id);
void ccci_hal_reset(int md_id);
void ccci_hal_irq_register(int md_id);
void set_md_sys_max_num(unsigned int max_num);
void update_active_md_sys_state(int md_id, int active);
void set_md_enable(int md_id, int en);
int  ccci_logic_layer_init(int);
void ccci_logic_layer_exit(int);
int  ccci_write_runtime_data(int md_id, unsigned char buf[], int len);



//                                                                
//                                     
//                                   
//                                              
//                                        
//                                    
//                                                  
//                                                  
//                                                                            
//                                                                  
extern int __init ccif_module_init(void);
extern void __exit ccif_module_exit(void);


#endif  //                 

