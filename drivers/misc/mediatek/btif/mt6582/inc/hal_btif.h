#ifndef __HAL_BTIF_H_
#define __HAL_BTIF_H_

#define MTK_BTIF_REG_BASE BTIF_BASE
#define MTK_BTIF_CG_BIT MT_CG_PERI_BTIF

#define BTIF_RBR(base)         (base + 0x0)	/*                              */
#define BTIF_THR(base)         (base + 0x0) /*                                */
#define BTIF_IER(base)         (base + 0x4) /*                                      */
#define BTIF_IIR(base)         (base + 0x8)	/*                                             */
#define BTIF_FIFOCTRL(base)    (base + 0x8)	/*                                  */
#define BTIF_FAKELCR(base)     (base + 0xC)	/*                              */
#define BTIF_LSR(base)         (base + 0x14)	/*                                */
#define BTIF_SLEEP_EN(base)    (base + 0x48)	/*                                  */
#define BTIF_DMA_EN(base)      (base + 0x4C)	/*                                */
#define BTIF_RTOCNT(base)      (base + 0x54)	/*                                      */
#define BTIF_TRI_LVL(base)     (base + 0x60)	/*                                                 */
#define BTIF_WAK(base)         (base + 0x64)	/*                                        */
#define BTIF_WAT_TIME(base)    (base + 0x68)	/*                                                  */
#define BTIF_HANDSHAKE(base)   (base + 0x6C)	/*                                                */

/*             */
#define BTIF_IER_TXEEN (0x1 << 1)	/*                                */
#define BTIF_IER_RXFEN (0x1 << 0)	/*                           */

/*             */
#define BTIF_IIR_NINT        (0x1 << 0)	/*               */
#define BTIF_IIR_TX_EMPTY    (0x1 << 1)	/*                          */
#define BTIF_IIR_RX          (0x1 << 2)	/*                 */
#define BTIF_IIR_RX_TIMEOUT  (0x11 << 2)	/*                 */

/*             */
#define BTIF_LSR_DR_BIT (0x1 << 0)
#define BTIF_LSR_THRE_BIT (0x1 << 5)
#define BTIF_LSR_TEMT_BIT (0x1 << 6)

/*                  */
#define BTIF_FIFOCTRL_CLR_TX (0x1 << 2)	/*              */
#define BTIF_FIFOCTRL_CLR_RX (0x1 << 1)	/*              */

/*                 */
#define BTIF_FAKELCR_NORMAL_MODE 0x0

/*                  */
#define BTIF_SLEEP_EN_BIT (0x1 << 0)	/*                  */
#define BTIF_SLEEP_DIS_BIT (0x0)	/*                   */

/*                */
#define BTIF_DMA_EN_RX  (0x1 << 0)	/*              */
#define BTIF_DMA_EN_TX  (0x1 << 1)	/*              */
#define BTIF_DMA_EN_AUTORST_EN  (0x1 << 2)	/*                                      */
#define BTIF_DMA_EN_AUTORST_DIS  (0x0 << 2)	/*                                                                               */

/*                 */
#define BTIF_TRI_LVL_TX_MASK ((0xf) << 0)
#define BTIF_TRI_LVL_RX_MASK ((0x7) << 4)

#define BTIF_TRI_LVL_TX(x) ((x & 0xf) << 0)
#define BTIF_TRI_LVL_RX(x) ((x & 0x7) << 4)

#define BTIF_TRI_LOOP_EN (0x1 << 7)
#define BTIF_TRI_LOOP_DIS (0x0 << 7)

/*             */
#define BTIF_WAK_BIT (0x1 << 0)

/*                   */
#define BTIF_HANDSHAKE_EN_HANDSHAKE 1
#define BTIF_HANDSHAKE_DIS_HANDSHAKE 0

#define BTIF_TX_FIFO_SIZE 16
#define BTIF_RX_FIFO_SIZE 8

#define BTIF_TX_FIFO_THRE (BTIF_TX_FIFO_SIZE / 2)
#define BTIF_RX_FIFO_THRE 0x1	/*     */

#endif /*             */
