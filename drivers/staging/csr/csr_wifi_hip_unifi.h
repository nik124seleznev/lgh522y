/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*
                                                                              
  
                              
  
                                                       
  
                                                                              
 */
#ifndef __CSR_WIFI_HIP_UNIFI_H__
#define __CSR_WIFI_HIP_UNIFI_H__ 1

#ifndef CSR_WIFI_HIP_TA_DISABLE
#include "csr_wifi_router_ctrl_prim.h"
#include "csr_wifi_router_prim.h"
#else
#include "csr_time.h"
#endif

/*                      */

/*                 */
#define SDIO_MANF_ID_CSR              0x032a

/*           */
#define SDIO_CARD_ID_UNIFI_1          0x0001
#define SDIO_CARD_ID_UNIFI_2          0x0002
#define SDIO_CARD_ID_UNIFI_3          0x0007
#define SDIO_CARD_ID_UNIFI_4          0x0008

/*                          */
#define SDIO_WLAN_FUNC_ID_UNIFI_1          0x0001
#define SDIO_WLAN_FUNC_ID_UNIFI_2          0x0001
#define SDIO_WLAN_FUNC_ID_UNIFI_3          0x0001
#define SDIO_WLAN_FUNC_ID_UNIFI_4          0x0002

/*                                   */
#define UNIFI_SDIO_CLOCK_MAX_HZ    50000000  /*    */

/*
                                 
  
                                                                            
                                                                           
                                                            
 */
#define UNIFI_SDIO_CLOCK_INIT_HZ    12500000 /*    */

/*
                       
  
                                                                  
                                                       
 */
#define UNIFI_SDIO_CLOCK_SAFE_HZ    1000000  /*    */

/*                                          */
#define UNIFI_IO_BLOCK_SIZE     64

#define UNIFI_WOL_OFF   0
#define UNIFI_WOL_SDIO  1
#define UNIFI_WOL_PIO   2

/*                                 */
#define UNIFI_NO_OF_TX_QS              4

#define CSR_WIFI_HIP_RESERVED_HOST_TAG 0xFFFFFFFF

/*
                                               
  
                                                                           
                                               
                                            
  
                                                                           
                                                          
  
                                                                 
 */
#define UNIFI_SOFT_TRAFFIC_Q_LENGTH (20 + 1)
#define UNIFI_SOFT_COMMAND_Q_LENGTH (16 + 1)

#include "csr_framework_ext.h"  /*                                 */
#include "csr_sdio.h"           /*                                 */
#include "csr_macro.h"          /*                                 */
#include "csr_wifi_result.h"

/*                                                                         */
#define UNIFI_MAC_ADDRESS_COPY(dst, src) \
    do { (dst)[0] = (src)[0]; (dst)[1] = (src)[1]; \
         (dst)[2] = (src)[2]; (dst)[3] = (src)[3]; \
         (dst)[4] = (src)[4]; (dst)[5] = (src)[5]; \
    } while (0)

#define UNIFI_MAC_ADDRESS_CMP(addr1, addr2) \
    (((addr1)[0] == (addr2)[0]) && ((addr1)[1] == (addr2)[1]) && \
     ((addr1)[2] == (addr2)[2]) && ((addr1)[3] == (addr2)[3]) && \
     ((addr1)[4] == (addr2)[4]) && ((addr1)[5] == (addr2)[5]))

/*                                            
                                                   
 */
typedef enum
{
    UNIFI_TRAFFIC_Q_BK = 0,
    UNIFI_TRAFFIC_Q_BE,
    UNIFI_TRAFFIC_Q_VI,
    UNIFI_TRAFFIC_Q_VO,
    UNIFI_TRAFFIC_Q_EAPOL,    /*                     */
    UNIFI_TRAFFIC_Q_MAX,      /*              */
    UNIFI_TRAFFIC_Q_MLME      /*              */
} unifi_TrafficQueue;

/*
                                         
                                                                   
                                                                         
  
                                                                  
                                                    
 */
typedef struct _bulk_data_desc
{
    const u8 *os_data_ptr;
    u32       data_length;
    const void     *os_net_buf_ptr;
    u32       net_buf_length;
} bulk_data_desc_t;

/*                                                           */
typedef struct _symbol
{
    u16 id;
    u32 obj;
} symbol_t;

/*
                                                               
                                                           
                                                                   
  
                                                          
 */

#include "unifi_os.h"

/*
                                                                              
                                                                         
                                                                
 */
#include "unifi_config.h"

#include "csr_wifi_hip_signals.h" /*               */

/*
                                                                       
                                   
 */
typedef struct card card_t;


/*
                                                                
                            
 */
typedef struct _bulk_data_param
{
    bulk_data_desc_t d[UNIFI_MAX_DATA_REFERENCES];
} bulk_data_param_t;


/*
                                                     
                                            
 */
typedef struct _card_info
{
    u16 chip_id;
    u16 chip_version;
    u32 fw_build;
    u16 fw_hip_version;
    u32 sdio_block_size;
} card_info_t;


/*
                            
 */
/*                                                                  
                                                                   
                                                                           
 */
typedef enum unifi_coredump_space
{
    UNIFI_COREDUMP_MAC_REG,
    UNIFI_COREDUMP_PHY_REG,
    UNIFI_COREDUMP_SH_DMEM,
    UNIFI_COREDUMP_MAC_DMEM,
    UNIFI_COREDUMP_PHY_DMEM,
    UNIFI_COREDUMP_TRIGGER_MAGIC = 0xFEED
} unifi_coredump_space_t;

/*                                                                        */
typedef struct unifi_coredump_req
{
    /*           */
    s32               index;       /*                     */
    unifi_coredump_space_t space;       /*              */
    u32              offset;      /*                          */
    /*             */
    u32 drv_build;                /*                 */
    u32 chip_ver;                 /*              */
    u32 fw_ver;                   /*                  */
    s32  requestor;                /*                                  */
    u32 timestamp;                /*                           */
    u32 serial;                   /*                       */
    s32  value;                    /*                */
} unifi_coredump_req_t;                 /*                                   */


/* 
                                     
  
                                                               
 */

/* 
  
                               
                                                                        
                                                
  
                                             
  
                                      
  
                                                  
  
                     
 */
card_t* unifi_alloc_card(CsrSdioFunction *sdiopriv, void *ospriv);


/* 
  
                             
  
                                            
  
                                                  
  
                                        
  
                                                                        
  
                                                           
  
                     
 */
CsrResult unifi_init_card(card_t *card, s32 led_mask);

/* 
  
                                  
  
                                            
  
                     
 */
void unifi_free_card(card_t *card);

/* 
  
                                                      
                                                                             
  
                                            
  
                     
 */
void unifi_cancel_pending_signals(card_t *card);

/* 
  
                          
                                                                  
                 
  
                                                                          
                                                                 
  
                                            
  
                                       
  
                                    
  
                                                                       
  
                               
  
                                                                    
  
                                                           
  
                     
 */
CsrResult unifi_send_signal(card_t *card, const u8 *sigptr,
                            u32 siglen,
                            const bulk_data_param_t *bulkdata);

/* 
  
                                                            
                                               
  
                                            
  
                                       
  
                                                      
  
                                                           
  
                     
 */
CsrResult unifi_send_resources_available(card_t *card, const u8 *sigptr);

/* 
  
                                                        
  
                                            
  
                                                    
  
                     
 */
void unifi_card_info(card_t *card, card_info_t *card_info);

/* 
  
                                            
                                               
  
                                            

                                           
  
                                           
  
                                                                      
  
                                                           
  
                     
 */
CsrResult unifi_check_io_status(card_t *card, s32 *status);


/* 
  
                                    
                                                            
                                            
  
                                            
  
                                                                            
                                                                          
                                                                   
                                              
  
                                     
  
                                                           
  
                                                                         
  
                     
 */
CsrResult unifi_bh(card_t *card, u32 *remaining);


/* 
                                              
  
                                                                      
                                                              
                            
 */
enum unifi_low_power_mode
{
    UNIFI_LOW_POWER_DISABLED,
    UNIFI_LOW_POWER_ENABLED
};

/* 
                          
  
                                                                
                                                            
                                             
 */
enum unifi_periodic_wake_mode
{
    UNIFI_PERIODIC_WAKE_HOST_DISABLED,
    UNIFI_PERIODIC_WAKE_HOST_ENABLED
};

/* 
  
                                    
                                                            
                                            
  
                                                                      
                                                                 
                                                        
  
                                                         
                                                               
  
                                            
  
                                            
  
                                                    
  
                                     
  
                                            
  
                     
 */
CsrResult unifi_configure_low_power_mode(card_t                       *card,
                                         enum unifi_low_power_mode     low_power_mode,
                                         enum unifi_periodic_wake_mode periodic_wake_mode);

/* 
  
                                                     
                                                                      
  
                                                                     
  
                                            
  
                                     
  
                                                           
  
                                            
  
                     
 */
CsrResult unifi_force_low_power_mode(card_t *card);

#ifndef CSR_WIFI_HIP_TA_DISABLE
/* 
                                          
  
                                          
                                                 
  
                                   
                                           
                                                         
  
                                               
  
                                                             
  
                     
 */
CsrResult unifi_ta_configure(card_t                               *card,
                             CsrWifiRouterCtrlTrafficConfigType    config_type,
                             const CsrWifiRouterCtrlTrafficConfig *config);

/* 
                                              
  
                                   
                                                          
                                                               
                                                
                                                       
                                             
  
                     
 */
void unifi_ta_sample(card_t                            *card,
                     CsrWifiRouterCtrlProtocolDirection direction,
                     const bulk_data_desc_t            *data,
                     const u8                    *saddr,
                     const u8                    *sta_macaddr,
                     u32                          timestamp,
                     u16                          rate);

/* 
                                                                 
                                                                      
                                                                   
                                                 
  
                                    
                                                 
                                                    
  
                     
 */
void unifi_ta_classification(card_t                      *card,
                             CsrWifiRouterCtrlTrafficType traffic_type,
                             u16                    period);

#endif
/* 
                                       
                                                                     
                                                                        
  
                                    
  
                     
 */
CsrResult unifi_card_hard_reset(card_t *card);


CsrResult unifi_card_readn(card_t *card, u32 unifi_addr, void *pdata, u16 len);
CsrResult unifi_card_read16(card_t *card, u32 unifi_addr, u16 *pdata);
CsrResult unifi_card_write16(card_t *card, u32 unifi_addr, u16 data);


enum unifi_dbg_processors_select
{
    UNIFI_PROC_MAC,
    UNIFI_PROC_PHY,
    UNIFI_PROC_BT,
    UNIFI_PROC_BOTH,
    UNIFI_PROC_INVALID
};

CsrResult unifi_card_stop_processor(card_t *card, enum unifi_dbg_processors_select which);

/* 
                                                   
                                                                              
 */

/* 
                                                  
                                                               
                     
  
                                         
  
                                                                      
  
                     
 */
unifi_TrafficQueue
unifi_frame_priority_to_queue(CSR_PRIORITY priority);

/* 
                                                                             
                                           
                                                                        
                     
  
               
  
                                                          
  
                     
 */
CSR_PRIORITY unifi_get_default_downgrade_priority(unifi_TrafficQueue queue);

/* 
  
                          
                                                                       
                                                               
                                                          
  
                                      
  
                     
 */
void unifi_pause_xmit(void *ospriv, unifi_TrafficQueue queue);
void unifi_restart_xmit(void *ospriv, unifi_TrafficQueue queue);

/* 
  
                                  
                                                                  
                                                                  
                                  
                                                                  
                                                            
  
                                      
  
                     
 */
CsrResult unifi_run_bh(void *ospriv);

/* 
  
                                                         
                                                                   
                                                     
  
                                                                 
                                        
  
                                      
  
                                       
  
                                    
  
                                                                       
  
                     
 */
void unifi_receive_event(void *ospriv,
                         u8 *sigdata, u32 siglen,
                         const bulk_data_param_t *bulkdata);

#ifdef CSR_WIFI_REQUEUE_PACKET_TO_HAL
/* 
  
                                                                
  
                                      
  
                                                      
  
                                                
  
                     
 */
CsrResult unifi_reque_ma_packet_request(void *ospriv, u32 host_tag,
                                        u16 status,
                                        bulk_data_desc_t *bulkDataDesc);

#endif
typedef struct
{
    u16 free_fh_sig_queue_slots[UNIFI_NO_OF_TX_QS];
    u16 free_fh_bulkdata_slots;
    u16 free_fh_fw_slots;
} unifi_HipQosInfo;

void unifi_get_hip_qos_info(card_t *card, unifi_HipQosInfo *hipqosinfo);


/* 
                                                    
  
                                                                        
                                                                            
                                                                             
 */
#define UNIFI_FW_STA    1   /*                            */

/* 
  
                                                         
  
                                      
  
                                                                         
                     
  
                                                                       
                                                                  
  
                                                                            
                                                                            
                                                                               
  
                     
 */
void* unifi_fw_read_start(void *ospriv, s8 is_fw, const card_info_t *info);

/* 
  
                                                        
  
                                      
  
                                                               
  
                                                                  
  
                                                    
  
                                                      
  
                     
 */
s32 unifi_fw_read(void *ospriv, void *arg, u32 offset, void *buf, u32 len);

/* 
  
                                                      
  
                                      
  
                                                                  
  
                     
 */
void unifi_fw_read_stop(void *ospriv, void *dlpriv);

/* 
  
                                                                       
                                                                            
  
                                      
  
                                                       
  
                                                
  
                     
 */
void* unifi_fw_open_buffer(void *ospriv, void *fwbuf, u32 len);

/* 
  
                                                              
                                                                            
  
                                      
  
                                                       
  
                     
 */
void unifi_fw_close_buffer(void *ospriv, void *fwbuf);

#ifndef CSR_WIFI_HIP_TA_DISABLE
/*
                             
  
                                         
                                                                             
                                                       
 */
/* 
  
                                                   
                                                         
                                                           
                                    
  
                                      
  
                                               
  
                                                         
  
                                                    
  
                     
 */
void unifi_ta_indicate_protocol(void                              *ospriv,
                                CsrWifiRouterCtrlTrafficPacketType packet_type,
                                CsrWifiRouterCtrlProtocolDirection direction,
                                const CsrWifiMacAddress           *src_addr);

/* 
  
                                                          
                                                         
                                                           
                                  
  
                                      
  
                                                                          
  
                     
 */
void unifi_ta_indicate_sampling(void *ospriv, CsrWifiRouterCtrlTrafficStats *stats);
void unifi_ta_indicate_l4stats(void     *ospriv,
                               u32 rxTcpThroughput,
                               u32 txTcpThroughput,
                               u32 rxUdpThroughput,
                               u32 txUdpThroughput);
#endif

void unifi_rx_queue_flush(void *ospriv);

/* 
                                     
  
                                                                         
                        
  
                                    
  
                      
 */
void unifi_sdio_interrupt_handler(card_t *card);


/*                  */

/*
                                                                                        
                                    
 */
CsrResult unifi_init(card_t *card);
CsrResult unifi_download(card_t *card, s32 led_mask);

/*
                                                                       
 */
CsrResult unifi_start_processors(card_t *card);

CsrResult unifi_capture_panic(card_t *card);

/*
                                          
 */
#define CSR_WIFI_INTMODE_DEFAULT        0
#define CSR_WIFI_INTMODE_RUN_BH_ONCE    1       /*                           */

void unifi_set_interrupt_mode(card_t *card, u32 mode);

/*
                                                                             
                             
 */
void unifi_request_max_sdio_clock(card_t *card);


/*                                              */
const char* lookup_bulkcmd_name(u16 id);

/*                                           */
#ifdef CSR_WIFI_HIP_DEBUG_OFFLINE
void unifi_debug_buf_dump(void);
void unifi_debug_log_to_buf(const char *fmt, ...);
void unifi_debug_hex_to_buf(const char *buff, u16 length);
#endif

/*                                 */
CsrResult unifi_coredump_get_value(card_t *card, struct unifi_coredump_req *req);
CsrResult unifi_coredump_capture(card_t *card, struct unifi_coredump_req *req);
CsrResult unifi_coredump_request_at_next_reset(card_t *card, s8 enable);
CsrResult unifi_coredump_init(card_t *card, u16 num_dump_buffers);
void unifi_coredump_free(card_t *card);

#endif /*                          */
