#ifndef __EEMCS_EXPT_H__
#define __EEMCS_EXPT_H__
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <asm/atomic.h>

#include "eemcs_ccci.h"
#include "eemcs_char.h"
#include "lte_df_main.h"

/*                               */
#define MD_EX_MAGIC  		0x5745444D  //      

/*                            */ 
#define EEMCS_EX_MODE_MASK 	(0xF0000000)

/*                             */
#define EE_BUF_LEN		   	(256)

/*                    */
#define CCCI_AED_DUMP_EX_MEM		(1<<0)
#define CCCI_AED_DUMP_MD_IMG_MEM	(1<<1)

/*                                */
#define MD_TSID_MAGIC 		0x43525442 //      


enum { 
	MD_INIT_START_BOOT = 0x00000000, 
	MD_INIT_CHK_ID = 0x5555FFFF,
	MD_EX = 0x00000004, 
	MD_EX_CHK_ID = 0x45584350,
	MD_EX_REC_OK = 0x00000006, 
	MD_EX_REC_OK_CHK_ID = 0x45524543, 
	MD_EX_RESUME_CHK_ID = 0x7,
	CCCI_DRV_VER_ERROR = 0x5,
	//                                                    
	MD_WDT_MONITOR = 0x1000,
	MD_WAKEN_UP = 0x10000,
};

enum {
	MD_EE_CASE_NORMAL = 0,
	MD_EE_CASE_ONLY_EX,
	MD_EE_CASE_ONLY_EX_OK,
	MD_EE_CASE_TX_TRG,
	MD_EE_CASE_ISR_TRG,
	MD_EE_CASE_NO_RESPONSE,
	MD_EE_CASE_AP_MASK_I_BIT_TOO_LONG,
};


enum { 
	MD_EX_TYPE_INVALID = 0, 
	MD_EX_TYPE_UNDEF = 1, 
	MD_EX_TYPE_SWI = 2,
	MD_EX_TYPE_PREF_ABT = 3, 
	MD_EX_TYPE_DATA_ABT = 4, 
	MD_EX_TYPE_ASSERT = 5,
	MD_EX_TYPE_FATALERR_TASK = 6, 
	MD_EX_TYPE_FATALERR_BUF = 7,
	MD_EX_TYPE_LOCKUP = 8, 
	MD_EX_TYPE_ASSERT_DUMP = 9,
	MD_EX_TYPE_ASSERT_FAIL = 10,
	DSP_EX_TYPE_ASSERT = 11,
	DSP_EX_TYPE_EXCEPTION = 12,
	DSP_EX_FATAL_ERROR = 13,
	NUM_EXCEPTION
};

#define MD_EX_TYPE_EMI_CHECK 99

/*                                      */
typedef struct _exception_record_header_t
{
	unsigned char  ex_type;
	unsigned char  ex_nvram;
	unsigned short ex_serial_num;
}EX_HEADER_T;

/*                                                */
typedef struct _ex_environment_info_t
{
	unsigned char  boot_mode;
	unsigned char reserved1[8];
	unsigned char execution_unit[8];
	unsigned char reserved2[147];
}EX_ENVINFO_T;

/*                                             */
typedef struct _ex_fatalerror_code_t
{
	unsigned int code1;
	unsigned int code2;
}EX_FATALERR_CODE_T;

/*                                   */
typedef struct _ex_fatalerror_t
{
	EX_FATALERR_CODE_T error_code;
	unsigned char reserved1[288];
}EX_FATALERR_T;

/*                                   */
typedef struct _ex_assert_fail_t
{
	unsigned char filename[24];
	unsigned int  linenumber;
	unsigned int  parameters[3];
	unsigned char reserved1[256];
}EX_ASSERTFAIL_T;

/*                                                         */
typedef union
{
	EX_FATALERR_T fatalerr;
	EX_ASSERTFAIL_T assert;
}EX_CONTENT_T;

/*                                                     */
typedef struct _ex_exception_log_t
{
	EX_HEADER_T	header;
	unsigned char	reserved1[12];
	EX_ENVINFO_T	envinfo;
	unsigned char	reserved2[36];
	EX_CONTENT_T	content;
}EX_LOG_T;

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
		CCCI_BUFF_T data;
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


typedef enum EEMCS_EXCEPTION_STATE_e {
    EEMCS_EX_INVALID    = -1,
    EEMCS_EX_NONE       = 0,
    EEMCS_EX_INIT       = 1,
    EEMCS_EX_DHL_DL_RDY = 2,
    EEMCS_EX_INIT_DONE  = 3,
    EEMCS_EX_MSG_OK		= 4,
    EEMCS_EX_REC_MSG_OK = 5,
} EEMCS_EXCEPTION_STATE;


typedef struct
{
    KAL_UINT32  hif_type;
    KAL_UINT32 	expt_txq_id;
    KAL_UINT32 	expt_rxq_id;
}ccci_expt_port_cfg;

typedef struct EEMCS_EXCEPTION_RECORD_st {
    KAL_UINT32          id;         //                            
    struct sk_buff_head skb_list;
    atomic_t            pkt_cnt;
} EEMCS_EXCEPTION_RECORD;

typedef void (*EEMCS_CCCI_EXCEPTION_IND_CALLBACK)(KAL_UINT32 msg_id);

//                                                             
#define CCCI_CDEV_NUM (CCCI_PORT_NUM_MAX - START_OF_CCCI_CDEV)
#define CCCI_PORT_NUM CCCI_PORT_NUM_MAX
typedef struct EEMCS_EXCEPTION_SET_st {
    int md_ex_type;
    spinlock_t expt_cb_lock;
    struct timer_list md_ex_monitor;
    EEMCS_CCCI_EXCEPTION_IND_CALLBACK expt_cb[CCCI_PORT_NUM_MAX]; 
    KAL_UINT8 *expt_info_mem;

    EEMCS_EXCEPTION_RECORD txq[SDIO_TX_Q_NUM];
    EEMCS_EXCEPTION_RECORD rxq[SDIO_RX_Q_NUM];
    EEMCS_EXCEPTION_RECORD port[CCCI_PORT_NUM];     //                        
    atomic_t ptx_blk_cnt[CCCI_PORT_NUM];            //                                     
    atomic_t prx_blk_cnt[CCCI_PORT_NUM];            //                                     
    atomic_t ptx_drp_cnt[CCCI_PORT_NUM];            //                                     
    atomic_t prx_drp_cnt[CCCI_PORT_NUM];            //                                     
    atomic_t ccci_tx_drp_cnt[CCCI_PORT_NUM];        //                                   
    atomic_t ccci_rx_drp_cnt[CCCI_PORT_NUM];        //                                   
} EEMCS_EXCEPTION_SET;

typedef struct{
	KAL_UINT32 TsId;
	KAL_UINT32 TimeStp;
} MD_BOOTUP_TRACE;

/*                                                                              
                                    
                                                                               */
KAL_INT32 	eemcs_expt_handshake(struct sk_buff *skb);
KAL_INT32 	eemcs_bootup_trace(struct sk_buff *skb);
void 		eemcs_aed(unsigned int dump_flag, char *aed_str);
KAL_INT32 	get_md_expt_type(void);

KAL_INT32 	eemcs_expt_mod_init(void);
void 		eemcs_expt_exit(void);
KAL_UINT32 	eemcs_register_expt_callback(EEMCS_CCCI_EXCEPTION_IND_CALLBACK func_ptr);
KAL_UINT32 	eemcs_unregister_expt_callback(KAL_UINT32 id);
KAL_INT32 	eemcs_expt_log_port(struct sk_buff *skb, KAL_UINT32 port_id);

void 		eemcs_expt_dev_tx_block(KAL_UINT32 port_id);
void 		eemcs_expt_dev_tx_drop(KAL_UINT32 port_id);
void 		eemcs_expt_dev_rx_block(KAL_UINT32 port_id);
void 		eemcs_expt_dev_rx_drop(KAL_UINT32 port_id);
void 		eemcs_expt_ccci_tx_drop(KAL_UINT32 port_id);
void 		eemcs_expt_ccci_rx_drop(KAL_UINT32 port_id);
KAL_INT32 	eemcs_expt_flush(void);
void 		eemcs_expt_reset(void);

kal_bool 	set_exception_mode(EEMCS_EXCEPTION_STATE mode);
kal_bool 	is_exception_mode(EEMCS_EXCEPTION_STATE *mode);
kal_bool 	is_valid_exception_port(KAL_UINT32 port_id, kal_bool is_rx);
kal_bool 	is_valid_exception_tx_channel(CCCI_CHANNEL_T chn);
ccci_expt_port_cfg* get_expt_port_info(KAL_UINT32 port_id);

ssize_t 	eemcs_expt_show_statistics(char *buf);
void 		eemcs_expt_reset_statistics(void);


#define hif_expt_dl_pkt_in_swq             mtlte_df_DL_pkt_in_swq
#define hif_expt_dl_read_swq               mtlte_df_DL_read_skb_from_swq
#define hif_expt_dl_pkt_handle_complete    mtlte_df_DL_pkt_handle_complete
#define hif_expt_ul_pkt_in_swq             mtlte_df_UL_pkt_in_swq
//                                                                        



#endif //                 
