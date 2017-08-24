#ifndef __HAL_BTIF_DMA_H_
#define __HAL_BTIF_DMA_H_

#include "hal_btif_dma_pub.h"

#define MTK_BTIF_APDMA_CLK_CG MT_CG_PERI_AP_DMA

#define TX_DMA_VFF_SIZE (1024 * 8)	/*                                       */
#define RX_DMA_VFF_SIZE (1024 * 8)	/*                                       */

#define DMA_TX_THRE(n) (n - 7)	/*                 */
#define DMA_RX_THRE(n) ((n) * 3 / 4)	/*                 */

/*                                                                                     */
/*                                                   */
#define BTIF_TX_DMA_OFFSET 0x780
#define BTIF_RX_DMA_OFFSET 0x800

/*                       */
#define DMA_INT_FLAG_OFFSET  0x00
#define DMA_INT_EN_OFFSET  0x04
#define DMA_EN_OFFSET  0x08
#define DMA_RST_OFFSET  0x0C
#define DMA_STOP_OFFSET  0x10
#define DMA_FLUSH_OFFSET  0x14

#define DMA_BASE_OFFSET  0x1C
#define DMA_LEN_OFFSET  0x24

#define DMA_THRE_OFFSET  0x28
#define DMA_WPT_OFFSET  0x2C
#define DMA_RPT_OFFSET  0x30
#define DMA_VALID_OFFSET  0x3C
#define DMA_LEFT_OFFSET  0x40

#define TX_DMA_INT_FLAG(base)           (base + 0x0)	/*                                             */
#define TX_DMA_INT_EN(base)             (base + 0x4)	/*                                               */
#define TX_DMA_EN(base)                 (base + DMA_EN_OFFSET)	/*                                     */
#define TX_DMA_RST(base)                (base + DMA_RST_OFFSET)	/*                                     */
#define TX_DMA_STOP(base)               (base + DMA_STOP_OFFSET)	/*                                    */
#define TX_DMA_FLUSH(base)              (base + DMA_FLUSH_OFFSET)	/*                                    */
#define TX_DMA_VFF_ADDR(base)           (base + 0x1C)	/*                                           */
#define TX_DMA_VFF_LEN(base)            (base + 0x24)	/*                                     */
#define TX_DMA_VFF_THRE(base)           (base + 0x28)	/*                                        */
#define TX_DMA_VFF_WPT(base)            (base + 0x2C)	/*                                            */
#define TX_DMA_VFF_RPT(base)            (base + 0x30)	/*                                            */
#define TX_DMA_W_INT_BUF_SIZE(base)     (base + 0x34)	/*                                                            */
#define TX_DMA_INT_BUF_SIZE(base)       (base + 0x38)	/*                                                      */
#define TX_DMA_VFF_VALID_SIZE(base)     (base + 0x3C)	/*                                         */
#define TX_DMA_VFF_LEFT_SIZE(base)      (base + 0x40)	/*                                        */
#define TX_DMA_DEBUG_STATUS(base)       (base + 0x50)	/*                                           */

/*                           */
#define RX_DMA_INT_FLAG(base)           (base + 0x0)	/*                                             */
#define RX_DMA_INT_EN(base)             (base + 0x4)	/*                                               */
#define RX_DMA_EN(base)                 (base + DMA_EN_OFFSET)	/*                                     */
#define RX_DMA_RST(base)                (base + DMA_RST_OFFSET)	/*                                    */
#define RX_DMA_STOP(base)               (base + DMA_STOP_OFFSET)	/*                                   */
#define RX_DMA_FLUSH(base)              (base + DMA_FLUSH_OFFSET)	/*                                    */
#define RX_DMA_VFF_ADDR(base)           (base + 0x1C)	/*                                           */
#define RX_DMA_VFF_LEN(base)            (base + 0x24)	/*                                     */
#define RX_DMA_VFF_THRE(base)           (base + 0x28)	/*                                        */
#define RX_DMA_VFF_WPT(base)            (base + 0x2C)	/*                                            */
#define RX_DMA_VFF_RPT(base)            (base + 0x30)	/*                                           */
#define RX_DMA_FLOW_CTRL_THRE(base)     (base + 0x34)	/*                                            */
#define RX_DMA_INT_BUF_SIZE(base)       (base + 0x38)	/*                                              */
#define RX_DMA_VFF_VALID_SIZE(base)     (base + 0x3C)	/*                                         */
#define RX_DMA_VFF_LEFT_SIZE(base)      (base + 0x40)	/*                                         */
#define RX_DMA_DEBUG_STATUS(base)       (base + 0x50)	/*                                           */

#define DMA_EN_BIT (0x1)
#define DMA_STOP_BIT (0x1)
#define DMA_RST_BIT (0x1)
#define DMA_FLUSH_BIT (0x1)

#define DMA_WARM_RST (0x1 << 0)
#define DMA_HARD_RST (0x1 << 1)

#define DMA_WPT_MASK (0x0000FFFF)
#define DMA_WPT_WRAP (0x00010000)

#define DMA_RPT_MASK (0x0000FFFF)
#define DMA_RPT_WRAP (0x00010000)

/*                          */
#define TX_DMA_INT_FLAG_MASK (0x1)

#define TX_DMA_INTEN_BIT (0x1)

#define TX_DMA_ADDR_MASK (0xFFFFFFF8)
#define TX_DMA_LEN_MASK (0x0000FFF8)

#define TX_DMA_THRE_MASK (0x0000FFFF)

#define TX_DMA_W_INT_BUF_MASK (0x000000FF)

#define TX_DMA_VFF_VALID_MASK (0x0000FFFF)
#define TX_DMA_VFF_LEFT_MASK (0x0000FFFF)

/*                          */
#define RX_DMA_INT_THRE (0x1 << 0)
#define RX_DMA_INT_DONE (0x1 << 1)

#define RX_DMA_INT_THRE_EN (0x1 << 0)
#define RX_DMA_INT_DONE_EN (0x1 << 1)

#define RX_DMA_ADDR_MASK (0xFFFFFFF8)
#define RX_DMA_LEN_MASK (0x0000FFF8)

#define RX_DMA_THRE_MASK (0x0000FFFF)

#define RX_DMA_FLOW_CTRL_THRE_MASK (0x000000FF)

#define RX_DMA_INT_BUF_SIZE_MASK (0x0000001F)

#define RX_DMA_VFF_VALID_MASK (0x0000001F)

#define RX_DMA_VFF_LEFT_MASK (0x0000FFFF)

typedef struct _MTK_BTIF_DMA_VFIFO_ {
	DMA_VFIFO vfifo;
	unsigned int wpt;	/*                                                                            */
	unsigned int last_wpt_wrap;	/*                      */
	unsigned int rpt;	/*                                                                           */
	unsigned int last_rpt_wrap;	/*                      */
} MTK_BTIF_DMA_VFIFO, *P_MTK_BTIF_DMA_VFIFO;

#endif /*                 */
