/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*
                                                                              
  
                                      
  
            
                                              
                                                                              
 */
#ifndef __CARD_SDIO_H__
#define __CARD_SDIO_H__

#include "csr_wifi_hip_unifi.h"
#include "csr_wifi_hip_unifi_udi.h"
#include "csr_wifi_hip_unifihw.h"
#include "csr_wifi_hip_unifiversion.h"
#ifndef CSR_WIFI_HIP_TA_DISABLE
#include "csr_wifi_hip_ta_sampling.h"
#endif
#include "csr_wifi_hip_xbv.h"
#include "csr_wifi_hip_chiphelper.h"


/*
  
                       
                                                              
  
 */

/*
                                                                          
                                               
                                                                           
                                                                         
                                  
 */
#define RESUME_XMIT_THRESHOLD           4


/*
                                                                          
                                                                      
                                
                                                                         
                                                                          
                                                                       
  
                                                                          
                                                                   
                                     
 */
#define TO_HOST_FLUSH_THRESHOLD (500 * 5)


/*                                    */
#define SDIO_CCCR_SDIO_REVISION     (0x00)
#define SDIO_SD_SPEC_REVISION       (0x01)
#define SDIO_IO_ENABLE              (0x02)
#define SDIO_IO_READY               (0x03)
#define SDIO_INT_ENABLE             (0x04)
#define SDIO_INT_PENDING            (0x05)
#define SDIO_IO_ABORT               (0x06)
#define SDIO_BUS_IFACE_CONTROL      (0x07)
#define SDIO_CARD_CAPABILOTY        (0x08)
#define SDIO_COMMON_CIS_POINTER     (0x09)
#define SDIO_BUS_SUSPEND            (0x0C)
#define SDIO_FUNCTION_SELECT        (0x0D)
#define SDIO_EXEC_FLAGS             (0x0E)
#define SDIO_READY_FLAGS            (0x0F)
#define SDIO_FN0_BLOCK_SIZE         (0x10)
#define SDIO_POWER_CONTROL          (0x12)
#define SDIO_VENDOR_START           (0xF0)

#define SDIO_CSR_HOST_WAKEUP        (0xf0)
#define SDIO_CSR_HOST_INT_CLEAR     (0xf1)
#define SDIO_CSR_FROM_HOST_SCRATCH0 (0xf2)
#define SDIO_CSR_FROM_HOST_SCRATCH1 (0xf3)
#define SDIO_CSR_TO_HOST_SCRATCH0   (0xf4)
#define SDIO_CSR_TO_HOST_SCRATCH1   (0xf5)
#define SDIO_CSR_FUNC_EN            (0xf6)
#define SDIO_CSR_CSPI_MODE          (0xf7)
#define SDIO_CSR_CSPI_STATUS        (0xf8)
#define SDIO_CSR_CSPI_PADDING       (0xf9)


#define UNIFI_SD_INT_ENABLE_IENM 0x0001    /*                   */

#ifdef CSR_PRE_ALLOC_NET_DATA
#define BULK_DATA_PRE_ALLOC_NUM 16
#endif

/*
                                                               
 */
typedef struct
{
    /*
                                                                
                                                                            
                                                                              
                                                                      
     */
    u16 version;

    /*
                                                                   
                         
     */
    u16 sdio_ctrl_offset;

    /*                                             */
    u16 fromhost_sigbuf_handle;

    /*                                           */
    u16 tohost_sigbuf_handle;

    /*
                                                                                    
                                         
     */
    u16 num_fromhost_sig_frags;

    /*
                                                                                     
                                                                             
                                                                            
                                                                                
                                                                              
                                                                         
                                                                          
                                    
     */
    u16 num_tohost_sig_frags;

    /*
                                                                           
                                            
     */
    u16 num_fromhost_data_slots;

    /*
                                                                            
                                            
     */
    u16 num_tohost_data_slots;

    /*
                                             
                                                                      
                                                                     
     */
    u16 data_slot_size;

    /*
                                                                        
                                                                      
     */
    u16 initialised;

    /*                                  */
    u32 overlay_size;

    /*                                  */
    u16 data_slot_round;
    u16 sig_frag_size;

    /*                                  */
    u16 tohost_signal_padding;
} sdio_config_data_t;

/*
                                                                        
 */
/*
                                                                       
                                
 */
#define SDIO_CONFIG_DATA_SIZE 30

/*                                                   */
#define SDIO_INIT_FLAG_OFFSET 0x12
#define SDIO_TO_HOST_SIG_PADDING_OFFSET 0x1C


/*                                            */
typedef struct
{
    u16 cmd_and_len;   /*                               */
    u16 data_slot;     /*                                                 */
    u16 offset;
    u16 buffer_handle;
} bulk_data_cmd_t;


/*                                 */
#define SDIO_CMD_SIGNAL                 0x00
#define SDIO_CMD_TO_HOST_TRANSFER       0x01
#define SDIO_CMD_TO_HOST_TRANSFER_ACK   0x02 /*          */
#define SDIO_CMD_FROM_HOST_TRANSFER     0x03
#define SDIO_CMD_FROM_HOST_TRANSFER_ACK 0x04 /*          */
#define SDIO_CMD_CLEAR_SLOT             0x05
#define SDIO_CMD_OVERLAY_TRANSFER       0x06
#define SDIO_CMD_OVERLAY_TRANSFER_ACK   0x07 /*          */
#define SDIO_CMD_FROM_HOST_AND_CLEAR    0x08
#define SDIO_CMD_PADDING                0x0f

#define SLOT_DIR_TO_HOST 0x8000


/*                         
           
                                        
 */
#define UNIFI_INIT_BULK_DATA(bulk_data_slot)        \
    {                                               \
        (bulk_data_slot)->os_data_ptr = NULL;       \
        (bulk_data_slot)->data_length = 0;          \
        (bulk_data_slot)->os_net_buf_ptr = NULL;    \
        (bulk_data_slot)->net_buf_length = 0;       \
    }

/*
                                          
                                                                      
              
                  
                                                                            
                                      
                                                                   
                                                            
 */
typedef struct card_signal
{
    u8 sigbuf[UNIFI_PACKED_SIGBUF_SIZE];

    /*                                    */
    u16 signal_length;

    bulk_data_desc_t bulkdata[UNIFI_MAX_DATA_REFERENCES];
} card_signal_t;


/*
                                               
 */
#define UNIFI_QUEUE_NAME_MAX_LENGTH     16
typedef struct
{
    card_signal_t *q_body;

    /*                                                         */
    u16 q_length;

    u16 q_wr_ptr;
    u16 q_rd_ptr;

    char name[UNIFI_QUEUE_NAME_MAX_LENGTH];
} q_t;


#define UNIFI_RESERVED_COMMAND_SLOTS   2

/*                                                      */
#define UNIFI_PACKETS_INTERVAL         1000

/*
                                   
 */
typedef struct
{
    u16 from_host_used_slots[UNIFI_NO_OF_TX_QS];
    u16 from_host_max_slots[UNIFI_NO_OF_TX_QS];
    u16 from_host_reserved_slots[UNIFI_NO_OF_TX_QS];

    /*                                               
                                                              
                                                           
                                                               
                     */
    /*                                   */
    u16 packets_txed[UNIFI_NO_OF_TX_QS];
    u16 total_packets_txed;

    /*                                                         */
    u16 packets_interval;

    /*                                                       */
    u8 queue_stable[UNIFI_NO_OF_TX_QS];
} card_dynamic_slot_t;


/*                                                            
              */

/*                              
           
                    
            
           
 */
#define CSR_WIFI_HIP_Q_SLOTS_USED(q)     \
    (((q)->q_wr_ptr - (q)->q_rd_ptr < 0)? \
     ((q)->q_wr_ptr - (q)->q_rd_ptr + (q)->q_length) : ((q)->q_wr_ptr - (q)->q_rd_ptr))

/*                              
           
                    
            
           
 */
#define CSR_WIFI_HIP_Q_SLOTS_FREE(q)     \
    ((q)->q_length - CSR_WIFI_HIP_Q_SLOTS_USED((q)) - 1)

/*                                
           
                    
                
            
                       
 */
#define CSR_WIFI_HIP_Q_SLOT_DATA(q, slot)    \
    ((q)->q_body + slot)

/*                            
           
                    
            
                       
 */
#define CSR_WIFI_HIP_Q_NEXT_R_SLOT(q)    \
    ((q)->q_rd_ptr)

/*                             
           
                    
            
                       
 */
#define CSR_WIFI_HIP_Q_NEXT_W_SLOT(q)    \
    ((q)->q_wr_ptr)

/*                                                       
           
                    
                                                
            
                                 
 */
#define CSR_WIFI_HIP_Q_WRAP(q, x)    \
    ((((x) >= (q)->q_length)?((x) % (q)->q_length) : (x)))

/*                           
           
                    
 */
#define CSR_WIFI_HIP_Q_INC_R(q)  \
    ((q)->q_rd_ptr = CSR_WIFI_HIP_Q_WRAP((q), (q)->q_rd_ptr + 1))

/*                            
           
                    
 */
#define CSR_WIFI_HIP_Q_INC_W(q)  \
    ((q)->q_wr_ptr = CSR_WIFI_HIP_Q_WRAP((q), (q)->q_wr_ptr + 1))

enum unifi_host_state
{
    UNIFI_HOST_STATE_AWAKE   = 0,
    UNIFI_HOST_STATE_DROWSY  = 1,
    UNIFI_HOST_STATE_TORPID  = 2
};

typedef struct
{
    bulk_data_desc_t   bd;
    unifi_TrafficQueue queue; /*                                   */
} slot_desc_t;

/*
                                          
 */
struct card
{
    /*
                                                                   
                                                                         
     */
    void *ospriv;

    /*
                                                         
                                                             
                                                       
     */
    u32 *fh_slot_host_tag_record;


    /*                                          */
    u32     build_id;
    char build_id_string[128];

    /*                            */
    u16 chip_id;

    /*                             */
    u16 chip_version;

    /*                                   */
    u8 function;

    /*                                                         */
    ChipDescript *helper;

    /*
                                                                 
                                                                    
                                 
     */
    s32 loader_led_mask;

    /*
                                                                    
                                                                         
                                                                  
                                                      
     */
#define card_is_tx_q_paused(card, q)   (card->tx_q_paused_flag[q])
#define card_tx_q_unpause(card, q)   (card->tx_q_paused_flag[q] = 0)
#define card_tx_q_pause(card, q)   (card->tx_q_paused_flag[q] = 1)

    u16 tx_q_paused_flag[UNIFI_TRAFFIC_Q_MAX + 1 + UNIFI_NO_OF_TX_QS]; /*                                */

    /*                                             */
    udi_func_t udi_hook;

    u8 bh_reason_host;
    u8 bh_reason_unifi;

    /*                                        */
    u8 request_max_clock;

    /*                               */
    u32 sdio_clock_speed;

    /*
                                                                
                              
     */
    enum unifi_host_state host_state;

    enum unifi_low_power_mode     low_power_mode;
    enum unifi_periodic_wake_mode periodic_wake_mode;

    /*
                                                                     
                
                                                                    
                                                              
                                                                           
                                                                     
                                                                           
                                                                 
     */
    card_signal_t fh_command_q_body[UNIFI_SOFT_COMMAND_Q_LENGTH];
    q_t           fh_command_queue;

    card_signal_t fh_traffic_q_body[UNIFI_NO_OF_TX_QS][UNIFI_SOFT_TRAFFIC_Q_LENGTH];
    q_t           fh_traffic_queue[UNIFI_NO_OF_TX_QS];

    /*
                                                            
                                                                        
                             
      
                                                                              
                                                 
     */
    s32 from_host_signals_w;
    s32 from_host_signals_r;
    s32 to_host_signals_r;
    s32 to_host_signals_w;


    /*                                                   */
    /*
                                             
                         
                         
     */
#define UNIFI_FH_BUF_SIZE 1024
    struct sigbuf
    {
        u8 *buf;     /*             */
        u8 *ptr;     /*             */
        u16 count;   /*              */
        u16 bufsize;
    } fh_buffer;
    struct sigbuf th_buffer;


    /*
                                                                    
                                    
                                                                         
     */
    u32 unifi_interrupt_seq;
    u8  generate_interrupt;


    /*                                 */
    slot_desc_t      *from_host_data;
    bulk_data_desc_t *to_host_data;


    /*
                                                    
                                                                           
                          
     */
    s16 from_host_data_head;

    /*                                    */
    card_dynamic_slot_t dynamic_slot_data;

    /*
                           
     */

    /*                                        */
    CsrSdioFunction *sdio_if;

    /*                                          */
    sdio_config_data_t config_data;

    /*                                                              */
    u32 init_flag_addr;
    u32 sdio_ctrl_addr;

    /*                                                                */
    u32 proc_select;
    u32 dmem_page;
    u32 pmem_page;

    /*                                                                    */
    u32 sdio_bytes_read;
    u32 sdio_bytes_written;

    u8 memory_resources_allocated;

    /*                            */
    u16 sdio_io_block_size;

    /*                                             */
    u8 sdio_io_block_pad;

    /*                   */
    struct FWOV fwov;

#ifndef CSR_WIFI_HIP_TA_DISABLE
    /*             */
    ta_data_t ta_sampling;
#endif

    /*               */
    s16             request_coredump_on_reset; /*                                */
    struct coredump_buf *dump_buf;                  /*           */
    struct coredump_buf *dump_next_write;           /*                           */
    struct coredump_buf *dump_cur_read;             /*                             */

#ifdef CSR_WIFI_HIP_DATA_PLANE_PROFILE
    struct cmd_profile
    {
        u32 cmd52_count;
        u32 cmd53_count;
        u32 tx_count;
        u32 tx_cfm_count;
        u32 rx_count;
        u32 bh_count;
        u32 process_count;
        u32 protocol_count;

        u32 cmd52_f0_r_count;
        u32 cmd52_f0_w_count;
        u32 cmd52_r8or16_count;
        u32 cmd52_w8or16_count;
        u32 cmd52_r16_count;
        u32 cmd52_w16_count;
        u32 cmd52_r32_count;

        u32 sdio_cmd_signal;
        u32 sdio_cmd_clear_slot;
        u32 sdio_cmd_to_host;
        u32 sdio_cmd_from_host;
        u32 sdio_cmd_from_host_and_clear;
    } hip_prof;
    struct cmd_profile cmd_prof;
#endif

    /*                                 */
    u32 intmode;

#ifdef UNIFI_DEBUG
    u8 lsb;
#endif

    /*                               */
    u32 panic_data_phy_addr;
    u32 panic_data_mac_addr;
    u16 last_phy_panic_code;
    u16 last_phy_panic_arg;
    u16 last_mac_panic_code;
    u16 last_mac_panic_arg;
#ifdef CSR_PRE_ALLOC_NET_DATA
    bulk_data_desc_t bulk_data_desc_list[BULK_DATA_PRE_ALLOC_NUM];
    u16        prealloc_netdata_r;
    u16        prealloc_netdata_w;
#endif
}; /*             */


/*             */
enum unifi_reset_type
{
    UNIFI_COLD_RESET = 1,
    UNIFI_WARM_RESET = 2
};

/*
                                                                     
                                                                             
                                                              
                                                                        
                                                                    
                                                               
                                                         
 */
CsrResult unifi_set_host_state(card_t *card, enum unifi_host_state state);


CsrResult unifi_set_proc_select(card_t *card, enum unifi_dbg_processors_select select);
s32 card_read_signal_counts(card_t *card);
bulk_data_desc_t* card_find_data_slot(card_t *card, s16 slot);


CsrResult unifi_read32(card_t *card, u32 unifi_addr, u32 *pdata);
CsrResult unifi_readnz(card_t *card, u32 unifi_addr,
                       void *pdata, u16 len);
s32 unifi_read_shared_count(card_t *card, u32 addr);

CsrResult unifi_writen(card_t *card, u32 unifi_addr, void *pdata, u16 len);

CsrResult unifi_bulk_rw(card_t *card, u32 handle,
                        void *pdata, u32 len, s16 direction);
CsrResult unifi_bulk_rw_noretry(card_t *card, u32 handle,
                                void *pdata, u32 len, s16 direction);
#define UNIFI_SDIO_READ       0
#define UNIFI_SDIO_WRITE      1

CsrResult unifi_read_8_or_16(card_t *card, u32 unifi_addr, u8 *pdata);
CsrResult unifi_write_8_or_16(card_t *card, u32 unifi_addr, u8 data);
CsrResult unifi_read_direct_8_or_16(card_t *card, u32 addr, u8 *pdata);
CsrResult unifi_write_direct_8_or_16(card_t *card, u32 addr, u8 data);

CsrResult unifi_read_direct16(card_t *card, u32 addr, u16 *pdata);
CsrResult unifi_read_direct32(card_t *card, u32 addr, u32 *pdata);
CsrResult unifi_read_directn(card_t *card, u32 addr, void *pdata, u16 len);

CsrResult unifi_write_direct16(card_t *card, u32 addr, u16 data);
CsrResult unifi_write_directn(card_t *card, u32 addr, void *pdata, u16 len);

CsrResult sdio_read_f0(card_t *card, u32 addr, u8 *pdata);
CsrResult sdio_write_f0(card_t *card, u32 addr, u8 data);

void unifi_read_panic(card_t *card);
#ifdef CSR_PRE_ALLOC_NET_DATA
void prealloc_netdata_free(card_t *card);
CsrResult prealloc_netdata_alloc(card_t *card);
#endif
/*                    */
void dump(void *mem, u16 len);
void dump16(void *mem, u16 len);

#endif /*                 */
