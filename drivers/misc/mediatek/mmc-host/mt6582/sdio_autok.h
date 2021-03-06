#ifndef MT6582_AUTOK_H
#define MT6582_AUTOK_H

#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>

#include "mt_sd.h"

#define AUTOK_READ 0
#define AUTOK_WRITE 1

#define PROC_BUF_SIZE 512

/*                                                                        
                                
                                                                        */
//                   
#define USE_KERNEL_THREAD
//                           
//                         
#define PMIC_MT6323

/*                                                              */
#define SDIO_IP_WTMDR       	(0x00B0)
#define SDIO_IP_WTMCR       	(0x00B4)
#define SDIO_IP_WTMDPCR0    	(0x00B8)
#define SDIO_IP_WTMDPCR1    	(0x00BC)
#define SDIO_IP_WPLRCR      	(0x00D4)
#define TEST_MODE_STATUS        (0x100)

struct sdio_autok_params
{
    u32 cmd_edge;
    u32 rdata_edge;
    u32 wdata_edge;
    u32 clk_drv;
    u32 cmd_drv;
    u32 dat_drv;
    u32 dat0_rd_dly;
    u32 dat1_rd_dly;
    u32 dat2_rd_dly;
    u32 dat3_rd_dly;
    u32 dat_wrd_dly;
    u32 cmd_resp_rd_dly;
    u32 cmd_rd_dly;
    u32 int_dat_latch_ck;
    u32 ckgen_msdc_dly_sel;
    u32 cmd_rsp_ta_cntr;
    u32 wrdat_crcs_ta_cntr;
    u32 pad_clk_txdly;
};

typedef struct
{
    unsigned int sel;
}S_AUTOK_DATA;

typedef union
{
    unsigned int version;
    unsigned int freq;
    S_AUTOK_DATA data;
}U_AUTOK_INTERFACE_DATA;

#ifdef USE_KERNEL_THREAD
struct sdio_autok_thread_data
{
    struct msdc_host *host;
    struct autok_predata *p_autok_predata;
    char stage;
    struct autok_progress *p_autok_progress;
    u8 *is_autok_done;
    struct completion *autok_completion;
    char *log;
};
#else   //                  
struct sdio_autok_workqueue_data
{
	struct delayed_work autok_delayed_work;
	struct msdc_host *host;
	struct autok_predata *p_autok_predata;
	char stage;
};
#endif  //                  

struct log_mmap_info {
  char *data; /*          */
  int reference;       /*                              */  
  int size;   
};

typedef enum
{
/*   */
    E_MSDC_PAD_TUNE_CMDRRDLY = 0,
    E_MSDC_CMD_RSP_TA_CNTR,
    E_MSDC_IOCON_RSPL,
    E_MSDC_CKGEN_MSDC_DLY_SEL,
    E_MSDC_PAD_TUNE_CMDRDLY,
/*    */
    E_MSDC_INT_DAT_LATCH_CK_SEL,
#if 1
    E_MSDC_IOCON_RDSPL,
    E_MSDC_PAD_TUNE_DATRRDLY,
#else
    E_IOCON_RD0SPL,
    E_IOCON_RD1SPL,
    E_IOCON_RD2SPL,
    E_IOCON_RD3SPL,
    E_DAT_RDDLY0_D0,
    E_DAT_RDDLY0_D1,
    E_DAT_RDDLY0_D2,
    E_DAT_RDDLY0_D3,
#endif
/*     */
    E_MSDC_WRDAT_CRCS_TA_CNTR,
    E_MSDC_IOCON_WDSPL,
    E_MSDC_PAD_TUNE_DATWRDLY,
    
    E_MSDC_PAD_DLY_PERIOD,
    E_MSDC_CMD_INT_MARGIN,

    E_MSDC_F_TINY_MARGIN,
    
    E_AUTOK_VERSION,
    E_AUTOK_FREQ,
    E_AUTOK_PARM_MAX
}E_AUTOK_PARAM;

#define MAX_AUTOK_DAT_NUM       (E_AUTOK_PARM_MAX)
#define E_AUTOK_DLY_PARAM_MAX   (E_MSDC_PAD_TUNE_DATWRDLY+1)

#define LTE_MODEM_FUNC (1)
#define CMD_52         (52)
#define CMD_53         (53)

#define REQ_CMD_EIO    (0x1 << 0)
#define REQ_CMD_TMO    (0x1 << 1)
#define REQ_DAT_ERR    (0x1 << 2)

#define MSDC_READ      (0)
#define MSDC_WRITE     (1)
#define LOG_SIZE (PAGE_SIZE*8)


enum AUTOK_PARAM {
    CMD_EDGE,                       //                                                                        
    RDATA_EDGE,                     //                                                                 
    WDATA_EDGE,                    //                                                                  
    CLK_DRV,                            //              
    CMD_DRV,                          //                
    DAT_DRV,                           //             
    DAT0_RD_DLY,                   //                                                              
    DAT1_RD_DLY,                   //                                                              
    DAT2_RD_DLY,                   //                                                              
    DAT3_RD_DLY,                   //                                                              
    DAT_WRD_DLY,                  //                                                                                                                                                     
    DAT_RD_DLY,                  //                                        
    CMD_RESP_RD_DLY,          //                                                                                                                                             
    CMD_RD_DLY,                    //                                                                                                                      
    DATA_DLYLINE_SEL,           //                                                                                                                                                                                                                                                   
    READ_DATA_SMPL_SEL,     //                                                                                                                                                                                                                                                    
    WRITE_DATA_SMPL_SEL,   //                                                                                                                                                                                                                                                     
    INT_DAT_LATCH_CK,          //                                                                                                        
    CKGEN_MSDC_DLY_SEL,    //                                                
    CMD_RSP_TA_CNTR,          //                                                                                                                                                        
    WRDAT_CRCS_TA_CNTR,   //                                                                                                                                                                         
    PAD_CLK_TXDLY,                //                                                                                           
    TOTAL_PARAM_COUNT
};

struct autok_progress{
    u32 host_id;
    u32 done;
};

struct autok_predata{
    u8 vol_count;
    u8 param_count;
    unsigned int *vol_list;
    U_AUTOK_INTERFACE_DATA **ai_data;
};


int msdc_autok_read(struct msdc_host *host, unsigned int u4Addr, unsigned int u4Func, void *pBuffer, unsigned int u4Len, unsigned int u4Cmd);
int msdc_autok_write(struct msdc_host *host, unsigned int u4Addr, unsigned int u4Func, void *pBuffer, unsigned int u4Len, unsigned int u4Cmd);
int msdc_autok_adjust_param(struct msdc_host *host, enum AUTOK_PARAM param, u32 *value, int rw);
int msdc_autok_stg1_cal(struct msdc_host *host, unsigned int offset_restore, struct autok_predata *p_single_autok);
//                                                       
int msdc_autok_stg2_cal(struct msdc_host *host, struct autok_predata *p_autok_data, unsigned int vcore_uv_off);
int msdc_autok_apply_param(struct msdc_host *host, unsigned int vcore_uv_off);
extern char* reset_autok_cursor(int voltage);
//                              
bool is_vcore_ss_corner(void);
#endif /*                       */
