/*
 * bfin_dma.h - Blackfin DMA defines/structures/etc...
 *
 * Copyright 2004-2010 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __ASM_BFIN_DMA_H__
#define __ASM_BFIN_DMA_H__

#include <linux/types.h>

/*                  */
#define DMAEN			0x0001	/*                    */
#define WNR				0x0002	/*                          */
#define WDSIZE_8		0x0000	/*                        */
#define PSIZE_8			0x00000000	/*                         */

#ifdef CONFIG_BF60x

#define PSIZE_16		0x00000010	/*                         */
#define PSIZE_32		0x00000020	/*                         */
#define PSIZE_64		0x00000030	/*                         */
#define WDSIZE_16		0x00000100	/*                         */
#define WDSIZE_32		0x00000200	/*                         */
#define WDSIZE_64		0x00000300	/*                         */
#define WDSIZE_128		0x00000400	/*                         */
#define WDSIZE_256		0x00000500	/*                         */
#define DMA2D			0x04000000	/*                   */
#define RESTART			0x00000004	/*                       */
#define DI_EN_X			0x00100000	/*                                  */
#define DI_EN_Y			0x00200000	/*                                  */
#define DI_EN_P			0x00300000	/*                                     */
#define DI_EN			DI_EN_X		/*                       */
#define NDSIZE_0		0x00000000	/*                          */
#define NDSIZE_1		0x00010000	/*                          */
#define NDSIZE_2		0x00020000	/*                          */
#define NDSIZE_3		0x00030000	/*                          */
#define NDSIZE_4		0x00040000	/*                          */
#define NDSIZE_5		0x00050000	/*                          */
#define NDSIZE_6		0x00060000	/*                          */
#define NDSIZE			0x00070000	/*                      */
#define NDSIZE_OFFSET		16		/*                             */
#define DMAFLOW_LIST		0x00004000	/*                      */
#define DMAFLOW_LARGE		DMAFLOW_LIST
#define DMAFLOW_ARRAY		0x00005000	/*                       */
#define DMAFLOW_LIST_DEMAND	0x00006000	/*                             */
#define DMAFLOW_ARRAY_DEMAND	0x00007000	/*                              */
#define DMA_RUN_DFETCH		0x00000100	/*                                        */
#define DMA_RUN			0x00000200	/*                               */
#define DMA_RUN_WAIT_TRIG	0x00000300	/*                                           */
#define DMA_RUN_WAIT_ACK	0x00000400	/*                                          */

#else

#define PSIZE_16		0x0000	/*                         */
#define PSIZE_32		0x0000	/*                         */
#define WDSIZE_16		0x0004	/*                         */
#define WDSIZE_32		0x0008	/*                         */
#define DMA2D			0x0010	/*                   */
#define RESTART			0x0020	/*                  */
#define DI_SEL			0x0040	/*                              */
#define DI_EN			0x0080	/*                       */
#define DI_EN_X			0x00C0	/*                                 */
#define DI_EN_Y			0x0080	/*                                 */
#define NDSIZE_0		0x0000	/*                                            */
#define NDSIZE_1		0x0100	/*                          */
#define NDSIZE_2		0x0200	/*                          */
#define NDSIZE_3		0x0300	/*                          */
#define NDSIZE_4		0x0400	/*                          */
#define NDSIZE_5		0x0500	/*                          */
#define NDSIZE_6		0x0600	/*                          */
#define NDSIZE_7		0x0700	/*                          */
#define NDSIZE_8		0x0800	/*                          */
#define NDSIZE_9		0x0900	/*                          */
#define NDSIZE			0x0f00	/*                      */
#define NDSIZE_OFFSET		8	/*                             */
#define DMAFLOW_ARRAY	0x4000	/*                       */
#define DMAFLOW_SMALL	0x6000	/*                                  */
#define DMAFLOW_LARGE	0x7000	/*                                  */
#define DFETCH			0x0004	/*                                */
#define DMA_RUN			0x0008	/*                               */

#endif
#define DMAFLOW			0x7000	/*              */
#define DMAFLOW_STOP	0x0000	/*           */
#define DMAFLOW_AUTO	0x1000	/*                 */

/*                      */
#define DMA_DONE		0x0001	/*                                 */
#define DMA_ERR			0x0002	/*                            */
#ifdef CONFIG_BF60x
#define DMA_PIRQ		0x0004	/*                                       */
#else
#define DMA_PIRQ		0
#endif

/*
                                                                     
                                                                    
 */
#define __BFP(m) u16 m; u16 __pad_##m

/*
                            
 */
struct bfin_dma_regs {
	u32 next_desc_ptr;
	u32 start_addr;
#ifdef CONFIG_BF60x
	u32 cfg;
	u32 x_count;
	u32 x_modify;
	u32 y_count;
	u32 y_modify;
	u32 pad1;
	u32 pad2;
	u32 curr_desc_ptr;
	u32 prev_desc_ptr;
	u32 curr_addr;
	u32 irq_status;
	u32 curr_x_count;
	u32 curr_y_count;
	u32 pad3;
	u32 bw_limit_count;
	u32 curr_bw_limit_count;
	u32 bw_monitor_count;
	u32 curr_bw_monitor_count;
#else
	__BFP(config);
	u32 __pad0;
	__BFP(x_count);
	__BFP(x_modify);
	__BFP(y_count);
	__BFP(y_modify);
	u32 curr_desc_ptr;
	u32 curr_addr;
	__BFP(irq_status);
	__BFP(peripheral_map);
	__BFP(curr_x_count);
	u32 __pad1;
	__BFP(curr_y_count);
	u32 __pad2;
#endif
};

#ifndef CONFIG_BF60x
/*
                                       
 */
struct bfin_hmdma_regs {
	__BFP(control);
	__BFP(ecinit);
	__BFP(bcinit);
	__BFP(ecurgent);
	__BFP(ecoverflow);
	__BFP(ecount);
	__BFP(bcount);
};
#endif

#undef __BFP

#endif
