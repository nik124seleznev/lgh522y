/*
 * bfin_serial.h - Blackfin UART/Serial definitions
 *
 * Copyright 2006-2010 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __BFIN_ASM_SERIAL_H__
#define __BFIN_ASM_SERIAL_H__

#include <linux/serial_core.h>
#include <linux/spinlock.h>
#include <mach/anomaly.h>
#include <mach/bfin_serial.h>

#if defined(CONFIG_BFIN_UART0_CTSRTS) || \
    defined(CONFIG_BFIN_UART1_CTSRTS) || \
    defined(CONFIG_BFIN_UART2_CTSRTS) || \
    defined(CONFIG_BFIN_UART3_CTSRTS)
# if defined(BFIN_UART_BF54X_STYLE) || defined(BFIN_UART_BF60X_STYLE)
#  define CONFIG_SERIAL_BFIN_HARD_CTSRTS
# else
#  define CONFIG_SERIAL_BFIN_CTSRTS
# endif
#endif

struct circ_buf;
struct timer_list;
struct work_struct;

struct bfin_serial_port {
	struct uart_port port;
	unsigned int old_status;
	int tx_irq;
	int rx_irq;
	int status_irq;
#ifndef BFIN_UART_BF54X_STYLE
	unsigned int lsr;
#endif
#ifdef CONFIG_SERIAL_BFIN_DMA
	int tx_done;
	int tx_count;
	struct circ_buf rx_dma_buf;
	struct timer_list rx_dma_timer;
	int rx_dma_nrows;
	spinlock_t rx_lock;
	unsigned int tx_dma_channel;
	unsigned int rx_dma_channel;
	struct work_struct tx_dma_workqueue;
#elif ANOMALY_05000363
	unsigned int anomaly_threshold;
#endif
#if defined(CONFIG_SERIAL_BFIN_CTSRTS) || \
	defined(CONFIG_SERIAL_BFIN_HARD_CTSRTS)
	int cts_pin;
	int rts_pin;
#endif
};

#ifdef BFIN_UART_BF60X_STYLE

/*                */
#define UCEN                     0x1  /*                     */
#define LOOP_ENA                 0x2  /*                      */
#define UMOD_MDB                 0x10  /*                 */
#define UMOD_IRDA                0x20  /*                  */
#define UMOD_MASK                0x30  /*                */
#define WLS(x)                   (((x-5) & 0x03) << 8)  /*                    */
#define WLS_MASK                 0x300  /*                         */
#define WLS_OFFSET               8      /*                           */
#define STB                      0x1000  /*           */
#define STBH                     0x2000  /*                */
#define PEN                      0x4000  /*               */
#define EPS                      0x8000  /*                    */
#define STP                      0x10000  /*              */
#define FPE                      0x20000  /*                                */
#define FFE                      0x40000  /*                                 */
#define SB                       0x80000  /*           */
#define LCR_MASK		 (SB | STP | EPS | PEN | STB | WLS_MASK)
#define FCPOL                    0x400000  /*                           */
#define RPOLC                    0x800000  /*                         */
#define TPOLC                    0x1000000  /*                         */
#define MRTS                     0x2000000  /*                        */
#define XOFF                     0x4000000  /*                 */
#define ARTS                     0x8000000  /*                           */
#define ACTS                     0x10000000  /*                         */
#define RFIT                     0x20000000  /*                            */
#define RFRT                     0x40000000  /*                            */

/*                 */
#define DR                       0x01  /*            */
#define OE                       0x02  /*               */
#define PE                       0x04  /*              */
#define FE                       0x08  /*               */
#define BI                       0x10  /*                 */
#define THRE                     0x20  /*           */
#define TEMT                     0x80  /*                        */
#define TFI                      0x100  /*                                 */

#define ASTKY                    0x200  /*                */
#define ADDR                     0x400  /*                    */
#define RO			 0x800  /*                   */
#define SCTS                     0x1000  /*            */
#define CTS                      0x10000  /*               */
#define RFCS                     0x20000  /*                           */

/*                  */
#define EDBO                     0x80000000 /*                      */

#else /*                       */

/*                */
#define WLS(x)                   (((x)-5) & 0x03)  /*                    */
#define WLS_MASK                 0x03  /*                         */
#define WLS_OFFSET               0     /*                           */
#define STB                      0x04  /*           */
#define PEN                      0x08  /*               */
#define EPS                      0x10  /*                    */
#define STP                      0x20  /*              */
#define SB                       0x40  /*           */
#define DLAB                     0x80  /*                      */
#define LCR_MASK		 (SB | STP | EPS | PEN | STB | WLS_MASK)

/*                */
#define DR                       0x01  /*            */
#define OE                       0x02  /*               */
#define PE                       0x04  /*              */
#define FE                       0x08  /*               */
#define BI                       0x10  /*                 */
#define THRE                     0x20  /*           */
#define TEMT                     0x40  /*                        */
#define TFI                      0x80  /*                                 */

/*                */
#define XOFF                     0x01  /*                 */
#define MRTS                     0x02  /*                        */
#define RFIT                     0x04  /*                            */
#define RFRT                     0x08  /*                            */
#define LOOP_ENA                 0x10  /*                      */
#define FCPOL                    0x20  /*                           */
#define ARTS                     0x40  /*                           */
#define ACTS                     0x80  /*                         */

/*                */
#define SCTS                     0x01  /*            */
#define CTS                      0x10  /*               */
#define RFCS                     0x20  /*                           */

/*                 */
#define UCEN                     0x01  /*                     */
#define UMOD_IRDA                0x02  /*                  */
#define UMOD_MASK                0x02  /*                */
#define TPOLC                    0x04  /*                         */
#define RPOLC                    0x08  /*                         */
#define FPE                      0x10  /*                                */
#define FFE                      0x20  /*                                 */

#endif /*                       */

/*                */
#define ERBFI                    0x01  /*                                      */
#define ETBEI                    0x02  /*                                        */
#define ELSI                     0x04  /*                            */
#define EDSSI                    0x08  /*                               */
#define EDTPTI                   0x10  /*                                    */
#define ETFI                     0x20  /*                                        */
#define ERFCI                    0x40  /*                                     */

#if defined(BFIN_UART_BF60X_STYLE)
# define OFFSET_REDIV            0x00  /*                                 */
# define OFFSET_CTL              0x04  /*                                 */
# define OFFSET_STAT             0x08  /*                                 */
# define OFFSET_SCR              0x0C  /*                                 */
# define OFFSET_CLK              0x10  /*                                 */
# define OFFSET_IER              0x14  /*                                 */
# define OFFSET_IER_SET          0x18  /*                                 */
# define OFFSET_IER_CLEAR        0x1C  /*                                 */
# define OFFSET_RBR              0x20  /*                                 */
# define OFFSET_THR              0x24  /*                                 */
#elif defined(BFIN_UART_BF54X_STYLE)
# define OFFSET_DLL              0x00  /*                                 */
# define OFFSET_DLH              0x04  /*                                 */
# define OFFSET_GCTL             0x08  /*                                 */
# define OFFSET_LCR              0x0C  /*                                 */
# define OFFSET_MCR              0x10  /*                                 */
# define OFFSET_LSR              0x14  /*                                 */
# define OFFSET_MSR              0x18  /*                                 */
# define OFFSET_SCR              0x1C  /*                                 */
# define OFFSET_IER_SET          0x20  /*                                 */
# define OFFSET_IER_CLEAR        0x24  /*                                 */
# define OFFSET_THR              0x28  /*                                 */
# define OFFSET_RBR              0x2C  /*                                 */
#else /*             */
# define OFFSET_THR              0x00  /*                                   */
# define OFFSET_RBR              0x00  /*                                   */
# define OFFSET_DLL              0x00  /*                                   */
# define OFFSET_DLH              0x04  /*                                   */
# define OFFSET_IER              0x04  /*                                   */
# define OFFSET_IIR              0x08  /*                                   */
# define OFFSET_LCR              0x0C  /*                                   */
# define OFFSET_MCR              0x10  /*                                   */
# define OFFSET_LSR              0x14  /*                                   */
# define OFFSET_MSR              0x18  /*                                   */
# define OFFSET_SCR              0x1C  /*                                   */
# define OFFSET_GCTL             0x24  /*                                   */
/*                                                               */
# undef OFFSET_IIR
#endif

/*
                                                                     
                                                                    
 */
#define __BFP(m) u16 m; u16 __pad_##m
struct bfin_uart_regs {
#if defined(BFIN_UART_BF60X_STYLE)
	u32 revid;
	u32 ctl;
	u32 stat;
	u32 scr;
	u32 clk;
	u32 ier;
	u32 ier_set;
	u32 ier_clear;
	u32 rbr;
	u32 thr;
	u32 taip;
	u32 tsr;
	u32 rsr;
	u32 txdiv;
	u32 rxdiv;
#elif defined(BFIN_UART_BF54X_STYLE)
	__BFP(dll);
	__BFP(dlh);
	__BFP(gctl);
	__BFP(lcr);
	__BFP(mcr);
	__BFP(lsr);
	__BFP(msr);
	__BFP(scr);
	__BFP(ier_set);
	__BFP(ier_clear);
	__BFP(thr);
	__BFP(rbr);
#else
	union {
		u16 dll;
		u16 thr;
		const u16 rbr;
	};
	const u16 __pad0;
	union {
		u16 dlh;
		u16 ier;
	};
	const u16 __pad1;
	const __BFP(iir);
	__BFP(lcr);
	__BFP(mcr);
	__BFP(lsr);
	__BFP(msr);
	__BFP(scr);
	const u32 __pad2;
	__BFP(gctl);
#endif
};
#undef __BFP

#define port_membase(uart)     (((struct bfin_serial_port *)(uart))->port.membase)

/*
                    
                          
      
*/
#ifdef BFIN_UART_BF60X_STYLE

#define UART_GET_CHAR(p)      bfin_read32(port_membase(p) + OFFSET_RBR)
#define UART_GET_CLK(p)       bfin_read32(port_membase(p) + OFFSET_CLK)
#define UART_GET_CTL(p)       bfin_read32(port_membase(p) + OFFSET_CTL)
#define UART_GET_GCTL(p)      UART_GET_CTL(p)
#define UART_GET_LCR(p)       UART_GET_CTL(p)
#define UART_GET_MCR(p)       UART_GET_CTL(p)
#if ANOMALY_16000030
#define UART_GET_STAT(p) \
({ \
	u32 __ret; \
	unsigned long flags; \
	flags = hard_local_irq_save(); \
	__ret = bfin_read32(port_membase(p) + OFFSET_STAT); \
	hard_local_irq_restore(flags); \
	__ret; \
})
#else
#define UART_GET_STAT(p)      bfin_read32(port_membase(p) + OFFSET_STAT)
#endif
#define UART_GET_MSR(p)       UART_GET_STAT(p)

#define UART_PUT_CHAR(p, v)   bfin_write32(port_membase(p) + OFFSET_THR, v)
#define UART_PUT_CLK(p, v)    bfin_write32(port_membase(p) + OFFSET_CLK, v)
#define UART_PUT_CTL(p, v)    bfin_write32(port_membase(p) + OFFSET_CTL, v)
#define UART_PUT_GCTL(p, v)   UART_PUT_CTL(p, v)
#define UART_PUT_LCR(p, v)    UART_PUT_CTL(p, v)
#define UART_PUT_MCR(p, v)    UART_PUT_CTL(p, v)
#define UART_PUT_STAT(p, v)   bfin_write32(port_membase(p) + OFFSET_STAT, v)

#define UART_CLEAR_IER(p, v)  bfin_write32(port_membase(p) + OFFSET_IER_CLEAR, v)
#define UART_GET_IER(p)       bfin_read32(port_membase(p) + OFFSET_IER)
#define UART_SET_IER(p, v)    bfin_write32(port_membase(p) + OFFSET_IER_SET, v)

#define UART_CLEAR_DLAB(p)    /*                         */
#define UART_SET_DLAB(p)      /*                         */

#define UART_CLEAR_LSR(p)     UART_PUT_STAT(p, -1)
#define UART_GET_LSR(p)       UART_GET_STAT(p)
#define UART_PUT_LSR(p, v)    UART_PUT_STAT(p, v)

/*                           */
#define BFIN_UART_CTSRTS_HARD
#define UART_CLEAR_SCTS(p)      UART_PUT_STAT(p, SCTS)
#define UART_GET_CTS(x)         (UART_GET_MSR(x) & CTS)
#define UART_DISABLE_RTS(x)     UART_PUT_MCR(x, UART_GET_MCR(x) & ~(ARTS | MRTS))
#define UART_ENABLE_RTS(x)      UART_PUT_MCR(x, UART_GET_MCR(x) | MRTS | ARTS)
#define UART_ENABLE_INTS(x, v)  UART_SET_IER(x, v)
#define UART_DISABLE_INTS(x)    UART_CLEAR_IER(x, 0xF)

#else /*                       */

#define UART_GET_CHAR(p)      bfin_read16(port_membase(p) + OFFSET_RBR)
#define UART_GET_DLL(p)       bfin_read16(port_membase(p) + OFFSET_DLL)
#define UART_GET_DLH(p)       bfin_read16(port_membase(p) + OFFSET_DLH)
#define UART_GET_CLK(p)	      ((UART_GET_DLH(p) << 8) | UART_GET_DLL(p))
#define UART_GET_GCTL(p)      bfin_read16(port_membase(p) + OFFSET_GCTL)
#define UART_GET_LCR(p)       bfin_read16(port_membase(p) + OFFSET_LCR)
#define UART_GET_MCR(p)       bfin_read16(port_membase(p) + OFFSET_MCR)
#define UART_GET_MSR(p)       bfin_read16(port_membase(p) + OFFSET_MSR)

#define UART_PUT_CHAR(p, v)   bfin_write16(port_membase(p) + OFFSET_THR, v)
#define UART_PUT_DLL(p, v)    bfin_write16(port_membase(p) + OFFSET_DLL, v)
#define UART_PUT_DLH(p, v)    bfin_write16(port_membase(p) + OFFSET_DLH, v)
#define UART_PUT_CLK(p, v) do \
{\
UART_PUT_DLL(p, v & 0xFF); \
UART_PUT_DLH(p, (v >> 8) & 0xFF); } while (0);

#define UART_PUT_GCTL(p, v)   bfin_write16(port_membase(p) + OFFSET_GCTL, v)
#define UART_PUT_LCR(p, v)    bfin_write16(port_membase(p) + OFFSET_LCR, v)
#define UART_PUT_MCR(p, v)    bfin_write16(port_membase(p) + OFFSET_MCR, v)

#ifdef BFIN_UART_BF54X_STYLE

#define UART_CLEAR_IER(p, v)  bfin_write16(port_membase(p) + OFFSET_IER_CLEAR, v)
#define UART_GET_IER(p)       bfin_read16(port_membase(p) + OFFSET_IER_SET)
#define UART_SET_IER(p, v)    bfin_write16(port_membase(p) + OFFSET_IER_SET, v)

#define UART_CLEAR_DLAB(p)    /*                         */
#define UART_SET_DLAB(p)      /*                         */

#define UART_CLEAR_LSR(p)     bfin_write16(port_membase(p) + OFFSET_LSR, -1)
#define UART_GET_LSR(p)       bfin_read16(port_membase(p) + OFFSET_LSR)
#define UART_PUT_LSR(p, v)    bfin_write16(port_membase(p) + OFFSET_LSR, v)

/*                           */
#define BFIN_UART_CTSRTS_HARD
#define UART_CLEAR_SCTS(p)      bfin_write16((port_membase(p) + OFFSET_MSR), SCTS)
#define UART_GET_CTS(x)         (UART_GET_MSR(x) & CTS)
#define UART_DISABLE_RTS(x)     UART_PUT_MCR(x, UART_GET_MCR(x) & ~(ARTS | MRTS))
#define UART_ENABLE_RTS(x)      UART_PUT_MCR(x, UART_GET_MCR(x) | MRTS | ARTS)
#define UART_ENABLE_INTS(x, v)  UART_SET_IER(x, v)
#define UART_DISABLE_INTS(x)    UART_CLEAR_IER(x, 0xF)

#else /*             */

#define UART_CLEAR_IER(p, v)  UART_PUT_IER(p, UART_GET_IER(p) & ~(v))
#define UART_GET_IER(p)       bfin_read16(port_membase(p) + OFFSET_IER)
#define UART_PUT_IER(p, v)    bfin_write16(port_membase(p) + OFFSET_IER, v)
#define UART_SET_IER(p, v)    UART_PUT_IER(p, UART_GET_IER(p) | (v))

#define UART_CLEAR_DLAB(p)    do { UART_PUT_LCR(p, UART_GET_LCR(p) & ~DLAB); SSYNC(); } while (0)
#define UART_SET_DLAB(p)      do { UART_PUT_LCR(p, UART_GET_LCR(p) | DLAB); SSYNC(); } while (0)

#define get_lsr_cache(uart)    (((struct bfin_serial_port *)(uart))->lsr)
#define put_lsr_cache(uart, v) (((struct bfin_serial_port *)(uart))->lsr = (v))

/*
                     
                            
      
                     
                           
      
*/

/*                                                                
                                                               
                                                  
 */
static inline void UART_CLEAR_LSR(void *p)
{
	put_lsr_cache(p, 0);
	bfin_write16(port_membase(p) + OFFSET_LSR, -1);
}
static inline unsigned int UART_GET_LSR(void *p)
{
	unsigned int lsr = bfin_read16(port_membase(p) + OFFSET_LSR);
	put_lsr_cache(p, get_lsr_cache(p) | (lsr & (BI|FE|PE|OE)));
	return lsr | get_lsr_cache(p);
}
static inline void UART_PUT_LSR(void *p, uint16_t val)
{
	put_lsr_cache(p, get_lsr_cache(p) & ~val);
}

/*                           */
#define UART_GET_CTS(x)        gpio_get_value((x)->cts_pin)
#define UART_DISABLE_RTS(x)    gpio_set_value((x)->rts_pin, 1)
#define UART_ENABLE_RTS(x)     gpio_set_value((x)->rts_pin, 0)
#define UART_ENABLE_INTS(x, v) UART_PUT_IER(x, v)
#define UART_DISABLE_INTS(x)   UART_PUT_IER(x, 0)

#endif /*                       */

#endif /*                       */

#ifndef BFIN_UART_TX_FIFO_SIZE
# define BFIN_UART_TX_FIFO_SIZE 2
#endif

#endif /*                       */
