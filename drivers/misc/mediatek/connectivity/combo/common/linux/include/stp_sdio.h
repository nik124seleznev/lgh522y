/*
         
*/

/*                      
          


*/

/*
          
*/

#ifndef _STP_SDIO_H
#define _STP_SDIO_H
/*                                                                              
                                                     
                                                                                
*/


#define KMALLOC_UPDATE 1

#if 0				/*                                                                               */
#define STP_SDIO_HOST_COUNT (1)
#define STP_SDIO_ONLY_ONE_HOST (0)
#endif
#define STP_SDIO_POLL_OWNBACK_INTR (1)

#define STP_SDIO_NEW_TXRING (0)
/*                                               
                                   
 */

#define STP_SDIO_OWN_THREAD (1)

/*                                                                              
                                                          
                                                                                
*/

#include "osal.h"
#include "hif_sdio.h"

/*                                                                              
                                                
                                                                                
*/
/*                             */
#define CCIR        (0x0000)
#define CHLPCR        (0x0004)
#define CSDIOCSR    (0x0008)
#define CHCR        (0x000c)
#define CHISR        (0x0010)
#define CHIER        (0x0014)
#define CTDR        (0x0018)
#define CRDR        (0x001c)
#define CTFSR        (0x0020)
#define CRPLR        (0x0024)
#define CSR        (0x00D8) //                          

/*                                       */
/*        */
#define C_FW_OWN_REQ_CLR    (0x00000200)
#define C_FW_OWN_REQ_SET    (0x00000100)
#define C_FW_INT_EN_CLR     (0x00000002)
#define C_FW_INT_EN_SET     (0x00000001)
#define C_FW_COM_DRV_OWN    (0x00000100)

/*       */
#define CHISR_EN_15_7       (0x0000ff80)
#define CHISR_EN_3_0        (0x0000000f)
/*       */
#define RX_PKT_LEN          (0xffff0000)
#define FIRMWARE_INT        (0x0000fe00)
#define TX_FIFO_OVERFLOW    (0x00000100)
#define FW_INT_IND_INDICATOR (0x00000080)
#define TX_COMPLETE_COUNT   (0x00000070)
#define TX_UNDER_THOLD      (0x00000008)
#define TX_EMPTY            (0x00000004)
#define RX_DONE             (0x00000002)
#define FW_OWN_BACK_INT     (0x00000001)

/*                   */
#define STP_SDIO_TX_FIFO_SIZE (2080UL)
#define STP_SDIO_RX_FIFO_SIZE (2304UL)	/*       */
#define STP_SDIO_TX_PKT_MAX_CNT (7)	/*                                                               */
#define STP_SDIO_HDR_SIZE (4)	/*                                                                    */

/*                   */
#define STP_SDIO_BLK_SIZE (512UL)

/*                          */
#define STP_SDIO_TX_BUF_CNT (16UL)	/*    */
#define STP_SDIO_TX_BUF_CNT_MASK (STP_SDIO_TX_BUF_CNT - 1)
#define STP_SDIO_TX_PKT_LIST_SIZE (STP_SDIO_TX_BUF_CNT)	/*                    */
#define STP_SDIO_TX_PKT_LIST_SIZE_MASK (STP_SDIO_TX_PKT_LIST_SIZE - 1)

/*                                   */
/*                                                    */
#if 1
/*                               */
#define STP_SDIO_TX_ENTRY_SIZE ((STP_SDIO_TX_FIFO_SIZE + (STP_SDIO_BLK_SIZE - 1)) & ~(STP_SDIO_BLK_SIZE - 1))
#else
/*                                 */
#define STP_SDIO_TX_MAX_BLK_CNT (STP_SDIO_TX_FIFO_SIZE / STP_SDIO_BLK_SIZE)
#define STP_SDIO_TX_ENTRY_SIZE (STP_SDIO_TX_MAX_BLK_CNT * STP_SDIO_BLK_SIZE)
#endif

/*                        */
/*                                                    */
/*                                                    */
#if 1
/*                               */
#define STP_SDIO_RX_BUF_SIZE ((STP_SDIO_RX_FIFO_SIZE + (STP_SDIO_BLK_SIZE - 1)) & ~(STP_SDIO_BLK_SIZE - 1))
#else
/*                                 */
#define STP_SDIO_RX_MAX_BLK_CNT (STP_SDIO_RX_FIFO_SIZE / STP_SDIO_BLK_SIZE)
#define STP_SDIO_RX_BUF_SIZE (STP_SDIO_RX_MAX_BLK_CNT * STP_SDIO_BLK_SIZE)
#endif

#define COHEC_00006052 (1)
/*                            */


/*                                                                              
                                                 
                                                                                
*/
/*                                               */
typedef struct _MTK_WCN_STP_SDIO_PKT_BUF {
	/*                                                                 */
#if KMALLOC_UPDATE
	PUINT8 tx_buf;
#else
	UINT8 tx_buf[STP_SDIO_TX_BUF_CNT][STP_SDIO_TX_ENTRY_SIZE];
#endif

	/*                                                        */
	UINT32 tx_buf_sz[STP_SDIO_TX_BUF_CNT];
	/*                                                                 */
	UINT32 tx_buf_ts[STP_SDIO_TX_BUF_CNT];
#if KMALLOC_UPDATE
	PUINT8 rx_buf;
#else
	UINT8 rx_buf[STP_SDIO_RX_BUF_SIZE];	/*                      */
#endif
#if STP_SDIO_NEW_TXRING
	UINT32 wr_cnt;		/*                                  */
	UINT32 rd_cnt;		/*                                 */
	spinlock_t rd_cnt_lock;	/*                                           */
#else
	UINT8 wr_idx;		/*                            *//*                 */
	UINT8 rd_idx;		/*                           *//*                 */
	spinlock_t rd_idx_lock;	/*                                         */
#endif
	MTK_WCN_BOOL full_flag;	/*                                                              */
	/*                                                            */
	UINT32 rd_irq_flag;
	/*                                                 */
	wait_queue_head_t fullwait_q;
} MTK_WCN_STP_SDIO_PKT_BUF;

/*                            */
typedef struct _MTK_WCN_STP_SDIO_Tx_Pkt_LIST {
	UINT32 pkt_rd_cnt;
	UINT32 pkt_wr_cnt;
	UINT16 pkt_size_list[STP_SDIO_TX_PKT_LIST_SIZE];	/*                        */
	UINT32 out_ts[STP_SDIO_TX_PKT_LIST_SIZE];
	UINT32 in_ts[STP_SDIO_TX_PKT_LIST_SIZE];
} MTK_WCN_STP_SDIO_Tx_Pkt_LIST;

/*                              */
typedef struct _MTK_WCN_STP_SDIO_FIRMWARE_INFO {
	UINT32 tx_fifo_size;	/*                           */
	UINT32 tx_packet_num;	/*                                     */
	atomic_t tx_comp_num;	/*                                                             */
} MTK_WCN_STP_SDIO_FIRMWARE_INFO;

/*                              */
typedef struct _MTK_WCN_STP_SDIO_PRIVATE_INFO {
	UINT8 stp_sdio_host_idx;
} MTK_WCN_STP_SDIO_PRIVATE_INFO;

/*                           */
typedef struct _MTK_WCN_STP_SDIO_HIF_INFO {
	MTK_WCN_HIF_SDIO_CLTCTX sdio_cltctx;
	MTK_WCN_STP_SDIO_PKT_BUF pkt_buf;
	MTK_WCN_STP_SDIO_Tx_Pkt_LIST tx_pkt_list;
	UINT32 rx_pkt_len;	/*                                                                   */
	MTK_WCN_STP_SDIO_FIRMWARE_INFO firmware_info;
	MTK_WCN_STP_SDIO_PRIVATE_INFO private_info;
#if STP_SDIO_OWN_THREAD
	/*                                  */
	OSAL_THREAD tx_rx_thread;
	INT32 irq_pending;
	INT32 sleep_flag;
	INT32 wakeup_flag;
	INT32 awake_flag;
	OSAL_EVENT tx_rx_event;
	OSAL_SIGNAL isr_check_complete;
	INT32 dump_flag;
#endif
	INT32 tx_dbg_dump_flag;
	struct work_struct tx_work;
	struct work_struct rx_work;
} MTK_WCN_STP_SDIO_HIF_INFO;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
/*                                       */
#define STP_SDIO_GET_PKT_AR_IDX(idx) ((idx) & STP_SDIO_TX_PKT_LIST_SIZE_MASK)

/*                                                                              
                                                             
                                                                                
*/

/* 
                                                   
  
                                                        
  
                                                     
  
                                     
                          
 */
extern INT32 mtk_wcn_hif_sdio_client_reg(const MTK_WCN_HIF_SDIO_CLTINFO *pinfo);

extern INT32 mtk_wcn_stp_sdio_do_own_clr(VOID);

/*              */
/*                                     */

/*                                                                              
                                                
                                                                                
*/
#endif				/*             */
