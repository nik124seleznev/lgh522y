#ifndef __MTK_UART_H__
#define __MTK_UART_H__

#include <mach/hardware.h>
#ifndef CONFIG_OF
#include <mach/mt_reg_base.h>
#endif
#include <mach/sync_write.h>
#include "platform_uart.h"

/*                                                                           */
#if defined(ENABLE_VFIFO_DEBUG)
/*                                                                           */
#define DGBUF_INIT(v)  \
    do { \
	if (UART_DEBUG_EVT(DBG_EVT_BUF)) { \
	    v->dbgidx = (v->dbgidx+1)%(ARRAY_SIZE(v->dbg)); \
	    v->cur = &v->dbg[v->dbgidx];                    \
	    v->cur->idx = 0;                                \
	} \
    } while (0)
/*                                                                           */
#define DGBUF_PUSH_CH(v, c)    \
    do { \
	if (UART_DEBUG_EVT(DBG_EVT_BUF)) \
	    v->cur->dat[v->cur->idx++] = (char)(c); \
    } while (0)
/*                                                                           */
#define DGBUF_PUSH_STR(v, s, l) \
    do { \
	if (UART_DEBUG_EVT(DBG_EVT_BUF)) {\
	    memcpy(&v->cur->dat[v->cur->idx], (s), (l)); \
	    v->cur->idx += (l);                          \
	} \
    } while (0)
#else
/*                                                                           */
#define DGBUF_INIT(v)
#define DGBUF_PUSH_CH(v, c)
#define DGBUF_PUSH_STR(v, s, l)
#endif

/*                                                                             
                
                                                                             */
typedef enum {
	UART_NON_DMA,
	UART_TX_DMA,
	UART_TX_VFIFO_DMA,
	UART_RX_VFIFO_DMA,
} UART_DMA_TYPE;
/*                                                                           */
typedef enum {
	UART_TX_VFIFO,
	UART_RX_VFIFO,

	UART_VFIFO_NUM
} UART_VFF_TYPE;
/*                                                                           */
/*               */
enum {
	UART_DMA_MODE_0,
	UART_DMA_MODE_1,
};
/*                                                                           */
/*                   */
enum {
	UART_FC_NONE,		/*                */
	UART_FC_SW,		/*                                                     */
	UART_FC_HW,		/*                */
};
/*                                                                           */
struct mtk_uart_setting {
	u8 tx_mode;
	u8 rx_mode;
	u8 dma_mode;
	u8 sysrq;

	int tx_trig;
	int rx_trig;
	unsigned long uart_base;
#ifdef CONFIG_OF
	unsigned long uart_phys_base;

	unsigned long irq_flags;
#endif
	u8 irq_num;
	u8 irq_sen;
	u8 set_bit;		/*              */
	u8 clr_bit;		/*              */
	int pll_id;

	u8 hw_flow;		/*                                       */
	u8 vff;			/*                       */
	u16 _align;
};
/*                                                                           */
#define C_UART_DEBUG_BUF_NUM (5)
/*                                                                           */
struct mtk_uart_buf {
	unsigned char *dat;
	unsigned int idx;
	unsigned int len;
};
/*                                                                           */
struct mtk_uart_vfifo {
	struct mtk_uart_dma *dma;	/*                 */

	/*              */
	u16 ch;
	u16 size;
	u16 trig;
	u16 type;		/*                              */
	void *base;
	void *port;		/*        */
	void *addr;

	atomic_t reg_cb;
	atomic_t entry;		/*             */
	spinlock_t iolock;
	struct timer_list timer;	/*             */
	struct hrtimer flush;
	dma_addr_t dmahd;	/*            */

	struct tasklet_struct flush_tasklet;

	struct mtk_uart_buf dbg[C_UART_DEBUG_BUF_NUM];
	struct mtk_uart_buf *cur;
	int dbgidx;
	unsigned int irq_id;
};
/*                                                                           */
struct mtk_uart_dma {
	struct mtk_uart *uart;	/*          */
	atomic_t free;		/*                  */
	unsigned short mode;	/*          */
	unsigned short dir;	/*                        */
	struct tasklet_struct tasklet;	/*                      */
	struct completion done;	/*                   */
	struct mtk_uart_vfifo *vfifo;	/*           */
};
/*                                                                           */
struct mtk_uart_register {
	unsigned int dll;
	unsigned int dlh;
	unsigned int ier;
	unsigned int lcr;
	unsigned int mcr;
	unsigned int fcr;
	unsigned int lsr;
	unsigned int efr;
	unsigned int highspeed;
	unsigned int sample_count;
	unsigned int sample_point;
	unsigned int fracdiv_l;
	unsigned int fracdiv_m;
	unsigned int escape_en;
	unsigned int guard;
	unsigned int rx_sel;
};
/*                                                                           */
struct mtk_uart {
	struct uart_port port;
	unsigned long base;
	int nport;
	unsigned int old_status;
	unsigned int tx_stop;
	unsigned int rx_stop;
	unsigned int ms_enable;
	unsigned int auto_baud;
	unsigned int line_status;
	unsigned int ignore_rx;
	unsigned int flow_ctrl;
	unsigned long pending_tx_reqs;
	unsigned long tx_trig;	/*                       */
	unsigned long rx_trig;	/*                       */
	unsigned long sysclk;
	unsigned long evt_mask;
	unsigned long lsr_status;
	int baudrate;		/*                 */
	int custom_baud;	/*                                          */

	int dma_mode;
	int tx_mode;
	int rx_mode;
	int fctl_mode;		/*             */
	int poweron_count;
	int timeout_count;	/*                  */

	unsigned int fcr_back_up;	/*                                       */

	struct mtk_uart_register registers;
	struct mtk_uart_dma dma_tx;
	struct mtk_uart_dma dma_rx;
	struct mtk_uart_vfifo *tx_vfifo;
	struct mtk_uart_vfifo *rx_vfifo;

	struct mtk_uart_setting *setting;

	unsigned int (*write_allow) (struct mtk_uart *uart);
	unsigned int (*read_allow) (struct mtk_uart *uart);
	void (*write_byte) (struct mtk_uart *uart, unsigned int byte);
	unsigned int (*read_byte) (struct mtk_uart *uart);
	unsigned int (*read_status) (struct mtk_uart *uart);
};
/*                                                                           */
/*              */
/*                                                                           */
struct fiq_dbg_event {
	u32 iir;
	int data;
};
/*                                                                           */
/*                                                      */
/*                                                      */
/*                                                      */
#define UART_READ8(REG)             (*(volatile unsigned char*)(REG))
#define UART_READ16(REG)            (*(volatile unsigned short*)(REG))
#define UART_READ32(REG)            (*(volatile unsigned int*)(REG))
#define reg_sync_writeb(v, a)			mt_reg_sync_writeb(v, a)
#define reg_sync_writel(v, a)			mt_reg_sync_writel(v, a)
/*                                                                           */
#define UART_SET_BITS(BS, REG)       ((*(volatile u32*)(REG)) |= (u32)(BS))
#define UART_CLR_BITS(BS, REG)       ((*(volatile u32*)(REG)) &= ~((u32)(BS)))
/*                                                                           */
extern spinlock_t mtk_console_lock;
extern struct mtk_uart *console_port;
unsigned int mtk_uart_pdn_enable(char *port, int enable);

extern struct resource fiq_resource[];
#endif				/*            */
