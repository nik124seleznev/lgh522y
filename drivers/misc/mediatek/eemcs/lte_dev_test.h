#ifndef __LTE_DEV_H__
#define __LTE_DEV_H__

#include <linux/version.h>
#include <linux/tty.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/mmc/sdio_func.h>

#include "lte_df_main.h"
#include "eemcs_kal.h"


 struct mtlte_ttydev {
	int minor ;
	unsigned int open_count ;
	struct tty_port port ;
	struct tty_struct *tty_ref;
	volatile int throttle ;
};


/*                                                            
 
                        
 
                                                            */
typedef enum {
	HDRV_SDBUS_BUSWIDTH_1BIT = 0x00,
	HDRV_SDBUS_BUSWIDTH_4BIT = 0x02,	
	HDRV_SDBUS_BUSWIDTH_BITMASK = 0x03,
} HDRV_SDBUS_BUSWIDTH, *PHDRV_SDBUS_BUSWIDTH ;

typedef enum {
	HDRV_SDBUS_FUNCTION_NUMBER=0,
	HDRV_SDBUS_FUNCTION_TYPE,
	HDRV_SDBUS_BUS_DRIVER_VERSION,
	HDRV_SDBUS_BUS_WIDTH,
	HDRV_SDBUS_BUS_CLOCK,
	HDRV_SDBUS_BUS_INTERFACE_CONTROL,
	HDRV_SDBUS_HOST_BLOCK_LENGTH,
	HDRV_SDBUS_FUNCTION_BLOCK_LENGTH,
	HDRV_SDBUS_FN0_BLOCK_LENGTH,
	HDRV_SDBUS_FUNCTION_INT_ENABLE
} HDRV_SDBUS_PROPERTY, *PHDRV_SDBUS_PROPERTY;

typedef enum _SDIO_THREAD_STATE {
	SDIO_THREAD_IDLE = 0x0 , 
	SDIO_THREAD_RUNNING = 0x1 , 

}SDIO_THREAD_STATE ;
/*  
                                            
 */
#define SDIO_FN0_CCCR_CSRR       0x0000
#define SDIO_FN0_CCCR_SDSRR      0x0001
#define SDIO_FN0_CCCR_IOER       0x0002
#define SDIO_FN0_CCCR_IORR       0x0003
#define SDIO_FN0_CCCR_IER        0x0004
#define SDIO_FN0_CCCR_IPR        0x0005
#define SDIO_FN0_CCCR_IOAR       0x0006
#define SDIO_FN0_CCCR_BICR       0x0007
#define SDIO_FN0_CCCR_CCR        0x0008
#define SDIO_FN0_CCCR_CCPR       0x0009  //                
#define SDIO_FN0_CCCR_BSR        0x000C
#define SDIO_FN0_CCCR_FSR        0x000D
#define SDIO_FN0_CCCR_EFR        0x000E
#define SDIO_FN0_CCCR_RFR        0x000F
#define SDIO_FN0_CCCR_F0BSR      0x0010	//                
#define SDIO_FN0_CCCR_PCR        0x0012
#define SDIO_FN0_CCCR_HSR        0x0013
#define SDIO_FN0_CCCR_UHSR       0x0014
#define SDIO_FN0_CCCR_DSR        0x0015
#define SDIO_FN0_CCCR_IEXTR      0x0016
#define SDIO_FN0_CCCR_IOBSF1R    0x0110
#define SDIO_FN0_CCCR_CIS0       0x1000
#define SDIO_FN0_CCCR_CIS1       0x2000

#define SDIO_CIS0_FW_ADDR        0xBF27C000
#define SDIO_CIS1_FW_ADDR        0xBF27C400
#define CARD_CAPABILITY_ADDR     0xBF27CC00


/*  
                                            
 */
#define SDIO_FN1_FBR_CSAR        0x0100
#define SDIO_FN1_FBR_EXTSCR      0x0101
#define SDIO_FN1_FBR_PSR         0x0102
#define SDIO_FN1_FBR_CIS         0x0109 //              
#define SDIO_FN1_FBR_F1BSR       0x0110 //              

/* 
                    
 */
int sdio_func0_wr(unsigned int u4Register,unsigned char *pValue, unsigned Length) ;

int sdio_func0_rd(unsigned int u4Register,unsigned char *pValue, unsigned int Length) ;

int sdio_func1_wr(unsigned int u4Register,void *pBuffer, unsigned int Length) ;

int sdio_func1_rd(unsigned int u4Register,void *pBuffer, unsigned int Length) ;

int sdio_property_set(HDRV_SDBUS_PROPERTY PropFunc, unsigned char * pData, unsigned int size) ;

int sdio_property_get(HDRV_SDBUS_PROPERTY PropFunc, unsigned char * pData, unsigned int size) ;

int sdio_open_device(struct sdio_func *sdiofunc) ;

int sdio_close_device(struct sdio_func *sdiofunc) ;

#ifdef USER_BUILD_KERNEL
typedef int (*MTLTE_HAL_TO_HIF_CALLBACK)(int data);
int mtlte_hal_register_MSDC_ERR_callback(MTLTE_HAL_TO_HIF_CALLBACK func_ptr); 
#endif

/*                                                            
 
                       
 
                                                            */
struct mtlte_dev {
	/*                               */	
	//                                               
	//              

/*                            */		
	//                                                
	//                                              
			
#ifdef CONFIG_REQUEST_FW
	const struct firmware *fw;
#endif
	const char *fw_name;
	
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22))
	struct net_device_stats net_stats;
#endif

/*                                 */	
	struct sdio_func *sdio_func;	/*                              */
	unsigned card_exist ;

	struct task_struct *sdio_thread;	
	wait_queue_head_t sdio_thread_wq;		
	volatile int sdio_thread_kick;
    volatile int sdio_thread_kick_isr;
    volatile int sdio_thread_kick_own_timer;
	volatile SDIO_THREAD_STATE sdio_thread_state ;

	struct kref kref;
	KAL_MUTEX   thread_kick_lock ;
};

/*                                                            
 
                        
 
                                                            */
#if TEST_DRV
#define LTE_TEST_DEVICE_MINOR	0

#define H2D_INT_H2DMB_init_req   (0x1<<31)
#define H2D_INT_H2DMB_data_sent  (0x1<<30)
#define H2D_INT_D2HMB_init_ack   (0x1<<29)
#define H2D_INT_D2HMB_data_ack   (0x1<<28)

#define D2H_INT_H2DMB_init_ack  (0x1<<31)
#define D2H_INT_H2DMB_data_ack  (0x1<<30)
#define D2H_INT_D2HMB_init_req  (0x1<<29)
#define D2H_INT_D2HMB_data_sent (0x1<<28)


typedef struct _at_msg_status {
    bool D2H_INT_H2DMB_init_ack_st;
    bool D2H_INT_H2DMB_data_ack_st;
    bool D2H_NEW_MSD_receiving_st;
    bool D2H_NEW_MSG_arrived;
} at_msg_status;


int mtlte_dev_test_probe(int index, struct device *dev);

void mtlte_dev_test_detach(int index);

int mtlte_dev_test_drvinit(void);

int mtlte_dev_test_drvdeinit(void) ;



typedef enum _athif_cmd_code{
    /*
                                                            
     */
	ATHIF_CMD_SET_RGPD_RL_MD = 0,
    /* 
                                                    
     */
	ATHIF_CMD_SET_FWD_MD,
    /* 
                                                                                        
     */
	ATHIF_CMD_SET_LB_TYPE,
    /* 
                                                                              
     */
	ATHIF_CMD_GET_PKT_CNT,	
    /* 
                                                                                  
     */
	ATHIF_CMD_DL_PERF,	
    /* 
                                                                                                  
     */
	ATHIF_CMD_CTRL_CH_TST,	
    /* 
                                                                                         
     */
	ATHIF_CMD_DL_SEND_N,	
    /* 
                                                                                     
     */
	ATHIF_CMD_DL_SEND_RAND_N,
    /* 
                                                                                  
     */
	ATHIF_CMD_DL_SEND_RAND_STRESS,
    /* 
                                                                
     */
	ATHIF_CMD_HIF_RECONNECT,
    /* 
                                                          
     */
	ATHIF_CMD_PAUSE_RGPD_RL,
    /* 
                                                                          
     */
	ATHIF_CMD_PREPARE_RGPD,
    /* 
                                                                 
     */
	ATHIF_CMD_SET_RGPD_FROMAT,
    /* 
                                                     
     */
	ATHIF_CMD_SET_TGPD_FROMAT,
    /* 
                                    
     */
	ATHIF_CMD_WRITE_MEM,
    /* 
                                   
     */
	ATHIF_CMD_READ_MEM,	
    /* 
                                                             
     */
	ATHIF_CMD_GET_QMU_INTR_INFO,
    /* 
                                                               
     */
	ATHIF_CMD_CLR_QMU_INTR_INFO,
    /* 
                                                                
     */
	ATHIF_CMD_PREPARE_IOC_RGPD,
    /* 
                                                               
     */
	ATHIF_CMD_PREPARE_IOC_TGPD,
    /* 
                                            
     */
	ATHIF_CMD_SET_CS_LEN,
    /* 
                                                                               
     */
	ATHIF_CMD_PAUSE_RESUME_DATAFLOW,
    /* 
                                                              
     */
	ATHIF_CMD_PREPARE_CS_TST_RGPD,
    /* 
                                                                                
     */
	ATHIF_CMD_GET_LOCAL_RGPD_RSLT,
    /* 
                                                              
     */
	ATHIF_CMD_PREPARE_CS_TST_TGPD,
    /* 
                                             
     */
	ATHIF_CMD_GET_LOCAL_TGPD_RSLT,
    /* 
                                    
     */
	ATHIF_CMD_SET_BUF_OFFSET,
    /* 
                                                    
     */
	ATHIF_CMD_USB_SET_EP0_DMA,
    /* 
                                       
     */
	ATHIF_CMD_SET_STOPQ_TIME,
    /* 
                                                               
     */
	ATHIF_CMD_CFG_SPECIFIC_RGPD_RL,
    /* 
                                                                             
     */
	ATHIF_CMD_PREPARE_MSD_RGPD,
    /* 
                                                                
     */
	ATHIF_CMD_SET_LB_QUE_MAP_TBL,
    /* 
                                                                               
     */
	ATHIF_CMD_GET_USB_IP_INFO,
    /* 
                                                
     */
    ATHIF_CMD_SET_USB_SPEED,
	/*
                                                     
 */
	ATHIF_CMD_RECONFIG_USB_EP,
	/*
                                                                            
 */
	ATHIF_CMD_EXCEPTION_DRV_TST,
	/*
                                                           
 */
    ATHIF_CMD_EXCEPTION_SWITCH_USB_CH_TST,
	/*
                                                               
 */
    ATHIF_CMD_GET_USB_EVT_CNT,
	/*
                                                                                  
                               
 */
    ATHIF_CMD_USB_REMOTE_WK_TST,
    
	/*
                                                    
 */
    ATHIF_CMD_U3_DEV_NOTIFY,

	/*
                                                         
 */
    ATHIF_CMD_U3_DEV_REQ_UX_TST,

	/*
                                                                  
 */
    ATHIF_CMD_U3_LB_DEV_UX_EXIT_TYPE,

	/*
                                                                
 */
    ATHIF_CMD_U2_LPM_REMOTE_WK_TST,

	/*
                                                                
 */
    ATHIF_CMD_U2_LPM_REMOTE_WK_RAND_TST,

	/*
                                                                                               
 */
    ATHIF_CMD_SEL_SUSPEND_TST,
	/*
                                                
 */
    ATHIF_CMD_AP_MD_OWN_TST,

	/*
                                  
 */
    ATHIF_CMD_SET_WDT,

	/*
                        
 */
    ATHIF_CMD_SET_USB_SWRST,

	/*
                                                 
 */
    ATHIF_CMD_USB_ENTER_SM,

	/*
                                                                       
 */
    ATHIF_CMD_GET_CLR_SM_INFO,

	/*
                                                 
 */
    ATHIF_CMD_EN_AUTO_SLEEP_MD,

	/*
                                                      
 */
    ATHIF_CMD_EN_SM_LB_TX_DELAY,



/*                              */
/*                                          */
/*                              */
  /* 
                                                           
   */
    SDIO_AT_CMD_LOOPBACK = 0x100,

  /* 
                                                     
   */
    SDIO_AT_MB_LOOPBACK,

  /* 
                                                           
   */
    SDIO_AT_SW_INT,
    
 /* 
                                                                            
   */
    SDIO_AT_D2H_NORMAL_OP,
    
 /* 
                                                                            
   */
    SDIO_AT_SELF_NORMAL_INT,

 /* 
                                                                            
   */
    SDIO_AT_NORMAL_DLQ_OP,

 /* 
                                          
   */
    SDIO_AT_DL_SEND_SP,

  /* 
                                             
   */
    SDIO_AT_BIG_SIZE,

  /* 
                                                
   */
    SDIO_AT_UL_BIG_SIZE,

 /* 
                                                 
   */
    SDIO_AT_DL_BIG_SIZE, 
    
 /* 
                                                                  
   */
   SDIO_AT_DL_INT_TEST_SWITCH,
 /* 
                                                              
  */
   SDIO_AT_RESET_DL_QUEUE,

 /* 
                                                              
  */
    SDIO_AT_RESET_UL_QUEUE,

  /* 
                                            
  */
   SDIO_AT_READ_INT_STATUS,

  /* 
                                               
  */
    SDIO_AT_SET_INT_MASK,

 /* 
                                                
  */
    SDIO_AT_READ_INT_MASK,

 /* 
                                                  
  */
    SDIO_AT_READ_TX_COMP_RET,

 /* 
                                                  
  */
    SDIO_AT_WD_RESET,

   /* 
                                                 
  */
    SDIO_AT_SELF_SLEEP,
  
    /* 
                                                                    
  */
    SDIO_AT_SET_WAKEUP_EINT,
  
     /* 
                                           
  */
    SDIO_AT_WAKEUP_EVENT_TEST,
  
  /* 
                                                      
  */
    SDIO_AT_CLEAN_18V_BIT = 0x1F0,
    
    /* 
                                                        
  */
    SDIO_AT_SET_BLOCK_INT = 0x1F1,

    /* 
                                        
  */
    SDIO_AT_PRESLT_END = 0x1F2,
    
    /* 
                               
  */
    SDIO_AT_EXCEPTION_TEST = 0x1F3,


    
}athif_cmd_code_e;


typedef struct _athif_init_cmd {
    /* 
                        
  */
    char signature[2];
    
    /* 
                                                                
  */
    short length;
    
}athif_init_cmd_t;


typedef struct _athif_cmd {
	/* 
                        
  */
	kal_uint8 	signature[2];
	/* 
                                                                
  */
	kal_uint16 	len;
	/* 
                           
  */
	kal_uint16 	cmd;
	/* 
                                  
  */
	kal_uint8	reverve[2];
	/* 
                                
  */
	kal_uint8 	buf[512];
}athif_cmd_t;

typedef struct _athif_status {
	/* 
                        
  */
	kal_uint8 	signature[2];
	/* 
                                                           
  */
	kal_uint16 	len;
	/* 
                          
  */
	kal_uint16 	status;
	/* 
                                  
  */
	kal_uint8	reverve[2];
	/* 
                                      
  */
	kal_uint8 	buf[512];
}athif_status_t;



#endif

#endif
