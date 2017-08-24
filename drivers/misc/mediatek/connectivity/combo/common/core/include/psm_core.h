/*       
                                            

                                                                                           
*/




#ifndef _PSM_CORE_H
#define _PSM_CORE_H

#include "stp_wmt.h"
#include "osal.h"

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define PFX_PSM                         "[STP-PSM] "
#define STP_PSM_LOG_LOUD                 4
#define STP_PSM_LOG_DBG                  3
#define STP_PSM_LOG_INFO                 2
#define STP_PSM_LOG_WARN                 1
#define STP_PSM_LOG_ERR                  0

#define ASSERT(expr)
#define STP_PSM_FIFO_SIZE  0x2000	/*         */
#define STP_PSM_TX_SIZE    0x800	/*         */

#define STP_PSM_OPERATION_FAIL    (-1)
#define STP_PSM_OPERATION_SUCCESS (0)

#define STP_PSM_PACKET_SIZE_MAX (2000)
#if (WMT_UART_RX_MODE_WORK || WMT_SDIO_MODE)
#define CFG_PSM_CORE_FIFO_SPIN_LOCK 0
#else
#define CFG_PSM_CORE_FIFO_SPIN_LOCK 1
#endif


#define PSM_HANDLING 127

#define STP_PSM_WMT_PS_TASK_HANDLING_TIME  30	/*                  */
#define STP_PSM_IDLE_TIME_SLEEP           5000	/*                              */
#define STP_PSM_SDIO_IDLE_TIME_SLEEP           100	/*                                   */
#define STP_PSM_WAIT_EVENT_TIMEOUT        6000

#if 0
#define STP_PSM_WMT_EVENT_SLEEP_EN                    (0x1UL << 0)
#define STP_PSM_WMT_EVENT_WAKEUP_EN                   (0x1UL << 1)
#define STP_PSM_BLOCK_DATA_EN                         (0x1UL << 2)
#define STP_PSM_WMT_EVENT_DISABLE_MONITOR             (0x1UL << 3)
#define STP_PSM_WMT_EVENT_ROLL_BACK_EN                (0x1UL << 4)
#define STP_PSM_RESET_EN                              (0x1UL << 5)
#define STP_PSM_WMT_EVENT_HOST_WAKEUP_EN                    (0x1UL << 6)
#define STP_PSM_WMT_EVENT_DISABLE_MONITOR_TX_HIGH_DENSITY   (0x1UL << 7)
#define STP_PSM_WMT_EVENT_DISABLE_MONITOR_RX_HIGH_DENSITY   (0x1UL << 8)
#endif

#define STP_PSM_WMT_EVENT_SLEEP_EN                    (0)
#define STP_PSM_WMT_EVENT_WAKEUP_EN                   (1)
#define STP_PSM_BLOCK_DATA_EN                         (2)
#define STP_PSM_WMT_EVENT_DISABLE_MONITOR             (3)
#define STP_PSM_WMT_EVENT_ROLL_BACK_EN                (4)
#define STP_PSM_RESET_EN                              (5)
#define STP_PSM_WMT_EVENT_HOST_WAKEUP_EN                    (6)
#define STP_PSM_WMT_EVENT_DISABLE_MONITOR_TX_HIGH_DENSITY   (7)
#define STP_PSM_WMT_EVENT_DISABLE_MONITOR_RX_HIGH_DENSITY   (8)

/*                                             */
#define STP_OP_BUF_SIZE (16)

#define PSM_THREAD_NAME "mtk_stp_psm"

/*                                                                              
                                                          
                                                                                
*/



/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/
typedef enum {
	ACT = 0,
	ACT_INACT,
	INACT,
	INACT_ACT,
	STP_PSM_MAX_STATE,
} MTKSTP_PSM_STATE_T;

typedef enum _ENUM_STP_OPID_T {
	STP_OPID_PSM_SLEEP = 0,
	STP_OPID_PSM_WAKEUP,
	STP_OPID_PSM_HOST_AWAKE,
	STP_OPID_PSM_EXIT,
	STP_OPID_PSM_NUM,
	STP_OPID_PSM_INALID = STP_OPID_PSM_NUM,
} ENUM_STP_OPID_T, *P_ENUM_STP_OPID_T;

typedef enum {
	MON = 0,
	UNMON,
} MTKSTP_PSM_MONSTATE_T;

typedef INT32(*wmt_notify_t) (MTKSTP_PSM_ACTION_T action);
typedef INT32(*stp_tx_cb_t) (PUINT8 buffer, UINT32 length, UINT8 type);

typedef OSAL_OP_DAT STP_OP;
typedef P_OSAL_OP_DAT P_STP_OP;
#if CFG_PSM_CORE_FIFO_SPIN_LOCK
typedef OSAL_UNSLEEPABLE_LOCK PSM_FIFO_LOCK, *PPSM_FIFO_LOCK;
#else
typedef OSAL_SLEEPABLE_LOCK PSM_FIFO_LOCK, *PPSM_FIFO_LOCK;
#endif

typedef struct mtk_stp_psm {
	OSAL_THREAD PSMd;	/*                           */
	OSAL_EVENT STPd_event;

	OSAL_OP_Q rFreeOpQ;	/*               */
	OSAL_OP_Q rActiveOpQ;	/*                 */
	OSAL_OP arQue[STP_OP_BUF_SIZE];	/*                   */

	/*                                            */
	/*                                            */
	UINT32 last_active_opId;
	MTKSTP_PSM_STATE_T work_state;	/*              */
	OSAL_BIT_OP_VAR flag;

	/*                                                                                                                       */
	INT32 sleep_en;

/*                                       */
	INT32 idle_time_to_sleep;
	OSAL_WAKE_LOCK wake_lock;
	OSAL_TIMER psm_timer;	/*                  */
	OSAL_EVENT wait_wmt_q;
	OSAL_FIFO hold_fifo;

	PSM_FIFO_LOCK hold_fifo_lock;

	OSAL_UNSLEEPABLE_LOCK wq_spinlock;
	OSAL_SLEEPABLE_LOCK user_lock;
	OSAL_SLEEPABLE_LOCK stp_psm_lock;
	INT32(*wmt_notify) (MTKSTP_PSM_ACTION_T action);
	INT32(*stp_tx_cb) (PUINT8 buffer, UINT32 length, UINT8 type);
	MTK_WCN_BOOL(*is_wmt_quick_ps_support)(VOID);
	UINT8 out_buf[STP_PSM_TX_SIZE];
} MTKSTP_PSM_T;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/
/*                         */
INT32 stp_psm_notify_stp(MTKSTP_PSM_T *stp_psm, const MTKSTP_PSM_ACTION_T action);
INT32 stp_psm_notify_wmt_wakeup(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_notify_wmt_sleep(MTKSTP_PSM_T *stp_psm);

INT32 stp_psm_start_monitor(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_is_to_block_traffic(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_is_disable(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_has_pending_data(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_release_data(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_hold_data(MTKSTP_PSM_T *stp_psm,
			const PUINT8 buffer, const UINT32 len, const UINT8 type);
INT32 stp_psm_do_wakeup(MTKSTP_PSM_T *stp_psm);
#define PSM_USE_COUNT_PACKAGE 0
#if PSM_USE_COUNT_PACKAGE
#define MTK_COMBO_PSM_RX_TH_DEFAULT (1600)
#define MTK_COMBO_PSM_TX_TH_DEFAULT (300)
INT32 stp_psm_disable_by_tx_rx_density(MTKSTP_PSM_T *stp_psm, INT32 dir);
#else
#define	SAMPLE_DURATION 1 /*        */
#define RTX_SPEED_THRESHOLD	50000	/*      */
INT32 stp_psm_disable_by_tx_rx_density(MTKSTP_PSM_T *stp_psm, INT32 dir,INT32 length);
#endif
INT32 stp_psm_reset(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_disable(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_enable(MTKSTP_PSM_T *stp_psm, INT32 idle_time_to_sleep);
struct mtk_stp_psm *stp_psm_init(void);
INT32 stp_psm_deinit(MTKSTP_PSM_T *stp_psm);
MTK_WCN_BOOL mtk_wcn_stp_psm_dbg_level(UINT32 dbglevel);
INT32 stp_psm_sleep_for_thermal(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_thread_lock_aquire(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_thread_lock_release(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_set_state(MTKSTP_PSM_T *stp_psm, MTKSTP_PSM_STATE_T state);
MTK_WCN_BOOL stp_psm_is_quick_ps_support(VOID);

INT32 stp_psm_set_sleep_enable(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_set_sleep_disable(MTKSTP_PSM_T *stp_psm);
INT32 stp_psm_check_sleep_enable(MTKSTP_PSM_T *stp_psm);

/*                                                                              
                                                
                                                                                
*/

#endif
