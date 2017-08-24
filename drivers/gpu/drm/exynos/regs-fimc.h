/* drivers/gpu/drm/exynos/regs-fimc.h
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * Register definition file for Samsung Camera Interface (FIMC) driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef EXYNOS_REGS_FIMC_H
#define EXYNOS_REGS_FIMC_H

/*
                
*/
/*                     */
#define EXYNOS_CISRCFMT		(0x00)
/*               */
#define EXYNOS_CIWDOFST		(0x04)
/*                */
#define EXYNOS_CIGCTRL		(0x08)
/*                 */
#define EXYNOS_CIWDOFST2	(0x14)
/*                                          */
#define EXYNOS_CIOYSA1		(0x18)
/*                                          */
#define EXYNOS_CIOYSA2		(0x1c)
/*                                          */
#define EXYNOS_CIOYSA3		(0x20)
/*                                          */
#define EXYNOS_CIOYSA4		(0x24)
/*                                           */
#define EXYNOS_CIOCBSA1		(0x28)
/*                                           */
#define EXYNOS_CIOCBSA2		(0x2c)
/*                                           */
#define EXYNOS_CIOCBSA3		(0x30)
/*                                           */
#define EXYNOS_CIOCBSA4		(0x34)
/*                                           */
#define EXYNOS_CIOCRSA1		(0x38)
/*                                           */
#define EXYNOS_CIOCRSA2		(0x3c)
/*                                           */
#define EXYNOS_CIOCRSA3		(0x40)
/*                                           */
#define EXYNOS_CIOCRSA4		(0x44)
/*                     */
#define EXYNOS_CITRGFMT		(0x48)
/*                    */
#define EXYNOS_CIOCTRL		(0x4c)
/*                      */
#define EXYNOS_CISCPRERATIO	(0x50)
/*                      */
#define EXYNOS_CISCPREDST		(0x54)
/*                     */
#define EXYNOS_CISCCTRL		(0x58)
/*             */
#define EXYNOS_CITAREA		(0x5c)
/*        */
#define EXYNOS_CISTATUS		(0x64)
/*         */
#define EXYNOS_CISTATUS2		(0x68)
/*                              */
#define EXYNOS_CIIMGCPT		(0xc0)
/*                  */
#define EXYNOS_CICPTSEQ		(0xc4)
/*               */
#define EXYNOS_CIIMGEFF		(0xd0)
/*                                     */
#define EXYNOS_CIIYSA0		(0xd4)
/*                                      */
#define EXYNOS_CIICBSA0		(0xd8)
/*                                      */
#define EXYNOS_CIICRSA0		(0xdc)
/*                       */
#define EXYNOS_CIILINESKIP_Y	(0xec)
/*                        */
#define EXYNOS_CIILINESKIP_CB	(0xf0)
/*                        */
#define EXYNOS_CIILINESKIP_CR	(0xf4)
/*                           */
#define EXYNOS_CIREAL_ISIZE	(0xf8)
/*                   */
#define EXYNOS_MSCTRL		(0xfc)
/*                                     */
#define EXYNOS_CIIYSA1		(0x144)
/*                                      */
#define EXYNOS_CIICBSA1		(0x148)
/*                                      */
#define EXYNOS_CIICRSA1		(0x14c)
/*                     */
#define EXYNOS_CIOYOFF		(0x168)
/*                      */
#define EXYNOS_CIOCBOFF		(0x16c)
/*                      */
#define EXYNOS_CIOCROFF		(0x170)
/*                    */
#define EXYNOS_CIIYOFF		(0x174)
/*                     */
#define EXYNOS_CIICBOFF		(0x178)
/*                     */
#define EXYNOS_CIICROFF		(0x17c)
/*                               */
#define EXYNOS_ORGISIZE		(0x180)
/*                                */
#define EXYNOS_ORGOSIZE		(0x184)
/*                            */
#define EXYNOS_CIEXTEN		(0x188)
/*               */
#define EXYNOS_CIDMAPARAM		(0x18c)
/*                       */
#define EXYNOS_CSIIMGFMT		(0x194)
/*                          */
#define EXYNOS_MISC_FIMC		(0x198)

/*                   */
/*                              */
#define EXYNOS_CIFCNTSEQ		(0x1fc)
/*                                          */
#define EXYNOS_CIOYSA5		(0x200)
/*                                          */
#define EXYNOS_CIOYSA6		(0x204)
/*                                          */
#define EXYNOS_CIOYSA7		(0x208)
/*                                          */
#define EXYNOS_CIOYSA8		(0x20c)
/*                                          */
#define EXYNOS_CIOYSA9		(0x210)
/*                                           */
#define EXYNOS_CIOYSA10		(0x214)
/*                                           */
#define EXYNOS_CIOYSA11		(0x218)
/*                                           */
#define EXYNOS_CIOYSA12		(0x21c)
/*                                           */
#define EXYNOS_CIOYSA13		(0x220)
/*                                           */
#define EXYNOS_CIOYSA14		(0x224)
/*                                           */
#define EXYNOS_CIOYSA15		(0x228)
/*                                           */
#define EXYNOS_CIOYSA16		(0x22c)
/*                                           */
#define EXYNOS_CIOYSA17		(0x230)
/*                                           */
#define EXYNOS_CIOYSA18		(0x234)
/*                                           */
#define EXYNOS_CIOYSA19		(0x238)
/*                                           */
#define EXYNOS_CIOYSA20		(0x23c)
/*                                           */
#define EXYNOS_CIOYSA21		(0x240)
/*                                           */
#define EXYNOS_CIOYSA22		(0x244)
/*                                           */
#define EXYNOS_CIOYSA23		(0x248)
/*                                           */
#define EXYNOS_CIOYSA24		(0x24c)
/*                                           */
#define EXYNOS_CIOYSA25		(0x250)
/*                                           */
#define EXYNOS_CIOYSA26		(0x254)
/*                                           */
#define EXYNOS_CIOYSA27		(0x258)
/*                                           */
#define EXYNOS_CIOYSA28		(0x25c)
/*                                           */
#define EXYNOS_CIOYSA29		(0x260)
/*                                           */
#define EXYNOS_CIOYSA30		(0x264)
/*                                           */
#define EXYNOS_CIOYSA31		(0x268)
/*                                           */
#define EXYNOS_CIOYSA32		(0x26c)

/*                                           */
#define EXYNOS_CIOCBSA5		(0x270)
/*                                           */
#define EXYNOS_CIOCBSA6		(0x274)
/*                                           */
#define EXYNOS_CIOCBSA7		(0x278)
/*                                           */
#define EXYNOS_CIOCBSA8		(0x27c)
/*                                           */
#define EXYNOS_CIOCBSA9		(0x280)
/*                                            */
#define EXYNOS_CIOCBSA10		(0x284)
/*                                            */
#define EXYNOS_CIOCBSA11		(0x288)
/*                                            */
#define EXYNOS_CIOCBSA12		(0x28c)
/*                                            */
#define EXYNOS_CIOCBSA13		(0x290)
/*                                            */
#define EXYNOS_CIOCBSA14		(0x294)
/*                                            */
#define EXYNOS_CIOCBSA15		(0x298)
/*                                            */
#define EXYNOS_CIOCBSA16		(0x29c)
/*                                            */
#define EXYNOS_CIOCBSA17		(0x2a0)
/*                                            */
#define EXYNOS_CIOCBSA18		(0x2a4)
/*                                            */
#define EXYNOS_CIOCBSA19		(0x2a8)
/*                                            */
#define EXYNOS_CIOCBSA20		(0x2ac)
/*                                            */
#define EXYNOS_CIOCBSA21		(0x2b0)
/*                                            */
#define EXYNOS_CIOCBSA22		(0x2b4)
/*                                            */
#define EXYNOS_CIOCBSA23		(0x2b8)
/*                                            */
#define EXYNOS_CIOCBSA24		(0x2bc)
/*                                            */
#define EXYNOS_CIOCBSA25		(0x2c0)
/*                                            */
#define EXYNOS_CIOCBSA26		(0x2c4)
/*                                            */
#define EXYNOS_CIOCBSA27		(0x2c8)
/*                                            */
#define EXYNOS_CIOCBSA28		(0x2cc)
/*                                            */
#define EXYNOS_CIOCBSA29		(0x2d0)
/*                                            */
#define EXYNOS_CIOCBSA30		(0x2d4)
/*                                            */
#define EXYNOS_CIOCBSA31		(0x2d8)
/*                                            */
#define EXYNOS_CIOCBSA32		(0x2dc)

/*                                           */
#define EXYNOS_CIOCRSA5		(0x2e0)
/*                                           */
#define EXYNOS_CIOCRSA6		(0x2e4)
/*                                           */
#define EXYNOS_CIOCRSA7		(0x2e8)
/*                                           */
#define EXYNOS_CIOCRSA8		(0x2ec)
/*                                           */
#define EXYNOS_CIOCRSA9		(0x2f0)
/*                                            */
#define EXYNOS_CIOCRSA10		(0x2f4)
/*                                            */
#define EXYNOS_CIOCRSA11		(0x2f8)
/*                                            */
#define EXYNOS_CIOCRSA12		(0x2fc)
/*                                            */
#define EXYNOS_CIOCRSA13		(0x300)
/*                                            */
#define EXYNOS_CIOCRSA14		(0x304)
/*                                            */
#define EXYNOS_CIOCRSA15		(0x308)
/*                                            */
#define EXYNOS_CIOCRSA16		(0x30c)
/*                                            */
#define EXYNOS_CIOCRSA17		(0x310)
/*                                            */
#define EXYNOS_CIOCRSA18		(0x314)
/*                                            */
#define EXYNOS_CIOCRSA19		(0x318)
/*                                            */
#define EXYNOS_CIOCRSA20		(0x31c)
/*                                            */
#define EXYNOS_CIOCRSA21		(0x320)
/*                                            */
#define EXYNOS_CIOCRSA22		(0x324)
/*                                            */
#define EXYNOS_CIOCRSA23		(0x328)
/*                                            */
#define EXYNOS_CIOCRSA24		(0x32c)
/*                                            */
#define EXYNOS_CIOCRSA25		(0x330)
/*                                            */
#define EXYNOS_CIOCRSA26		(0x334)
/*                                            */
#define EXYNOS_CIOCRSA27		(0x338)
/*                                            */
#define EXYNOS_CIOCRSA28		(0x33c)
/*                                            */
#define EXYNOS_CIOCRSA29		(0x340)
/*                                            */
#define EXYNOS_CIOCRSA30		(0x344)
/*                                            */
#define EXYNOS_CIOCRSA31		(0x348)
/*                                            */
#define EXYNOS_CIOCRSA32		(0x34c)

/*
             
*/
/*                                   */
/*                                   */
#define DEF_PP		4
#define EXYNOS_CIOYSA(__x)		\
	(((__x) < DEF_PP) ?	\
	 (EXYNOS_CIOYSA1  + (__x) * 4) : \
	(EXYNOS_CIOYSA5  + ((__x) - DEF_PP) * 4))
#define EXYNOS_CIOCBSA(__x)	\
	(((__x) < DEF_PP) ?	\
	 (EXYNOS_CIOCBSA1 + (__x) * 4) : \
	(EXYNOS_CIOCBSA5 + ((__x) - DEF_PP) * 4))
#define EXYNOS_CIOCRSA(__x)	\
	(((__x) < DEF_PP) ?	\
	 (EXYNOS_CIOCRSA1 + (__x) * 4) : \
	(EXYNOS_CIOCRSA5 + ((__x) - DEF_PP) * 4))
/*                                   */
#define DEF_IPP		1
#define EXYNOS_CIIYSA(__x)		\
	(((__x) < DEF_IPP) ?	\
	 (EXYNOS_CIIYSA0) : (EXYNOS_CIIYSA1))
#define EXYNOS_CIICBSA(__x)	\
	(((__x) < DEF_IPP) ?	\
	 (EXYNOS_CIICBSA0) : (EXYNOS_CIICBSA1))
#define EXYNOS_CIICRSA(__x)	\
	(((__x) < DEF_IPP) ?	\
	 (EXYNOS_CIICRSA0) : (EXYNOS_CIICRSA1))

#define EXYNOS_CISRCFMT_SOURCEHSIZE(x)		((x) << 16)
#define EXYNOS_CISRCFMT_SOURCEVSIZE(x)		((x) << 0)

#define EXYNOS_CIWDOFST_WINHOROFST(x)		((x) << 16)
#define EXYNOS_CIWDOFST_WINVEROFST(x)		((x) << 0)

#define EXYNOS_CIWDOFST2_WINHOROFST2(x)		((x) << 16)
#define EXYNOS_CIWDOFST2_WINVEROFST2(x)		((x) << 0)

#define EXYNOS_CITRGFMT_TARGETHSIZE(x)		(((x) & 0x1fff) << 16)
#define EXYNOS_CITRGFMT_TARGETVSIZE(x)		(((x) & 0x1fff) << 0)

#define EXYNOS_CISCPRERATIO_SHFACTOR(x)		((x) << 28)
#define EXYNOS_CISCPRERATIO_PREHORRATIO(x)		((x) << 16)
#define EXYNOS_CISCPRERATIO_PREVERRATIO(x)		((x) << 0)

#define EXYNOS_CISCPREDST_PREDSTWIDTH(x)		((x) << 16)
#define EXYNOS_CISCPREDST_PREDSTHEIGHT(x)		((x) << 0)

#define EXYNOS_CISCCTRL_MAINHORRATIO(x)		((x) << 16)
#define EXYNOS_CISCCTRL_MAINVERRATIO(x)		((x) << 0)

#define EXYNOS_CITAREA_TARGET_AREA(x)		((x) << 0)

#define EXYNOS_CISTATUS_GET_FRAME_COUNT(x)		(((x) >> 26) & 0x3)
#define EXYNOS_CISTATUS_GET_FRAME_END(x)		(((x) >> 17) & 0x1)
#define EXYNOS_CISTATUS_GET_LAST_CAPTURE_END(x)	(((x) >> 16) & 0x1)
#define EXYNOS_CISTATUS_GET_LCD_STATUS(x)		(((x) >> 9) & 0x1)
#define EXYNOS_CISTATUS_GET_ENVID_STATUS(x)	(((x) >> 8) & 0x1)

#define EXYNOS_CISTATUS2_GET_FRAMECOUNT_BEFORE(x)	(((x) >> 7) & 0x3f)
#define EXYNOS_CISTATUS2_GET_FRAMECOUNT_PRESENT(x)	((x) & 0x3f)

#define EXYNOS_CIIMGEFF_FIN(x)			((x & 0x7) << 26)
#define EXYNOS_CIIMGEFF_PAT_CB(x)			((x) << 13)
#define EXYNOS_CIIMGEFF_PAT_CR(x)			((x) << 0)

#define EXYNOS_CIILINESKIP(x)			(((x) & 0xf) << 24)

#define EXYNOS_CIREAL_ISIZE_HEIGHT(x)		((x) << 16)
#define EXYNOS_CIREAL_ISIZE_WIDTH(x)		((x) << 0)

#define EXYNOS_MSCTRL_SUCCESSIVE_COUNT(x)		((x) << 24)
#define EXYNOS_MSCTRL_GET_INDMA_STATUS(x)		((x) & 0x1)

#define EXYNOS_CIOYOFF_VERTICAL(x)			((x) << 16)
#define EXYNOS_CIOYOFF_HORIZONTAL(x)		((x) << 0)

#define EXYNOS_CIOCBOFF_VERTICAL(x)		((x) << 16)
#define EXYNOS_CIOCBOFF_HORIZONTAL(x)		((x) << 0)

#define EXYNOS_CIOCROFF_VERTICAL(x)		((x) << 16)
#define EXYNOS_CIOCROFF_HORIZONTAL(x)		((x) << 0)

#define EXYNOS_CIIYOFF_VERTICAL(x)			((x) << 16)
#define EXYNOS_CIIYOFF_HORIZONTAL(x)		((x) << 0)

#define EXYNOS_CIICBOFF_VERTICAL(x)		((x) << 16)
#define EXYNOS_CIICBOFF_HORIZONTAL(x)		((x) << 0)

#define EXYNOS_CIICROFF_VERTICAL(x)		((x) << 16)
#define EXYNOS_CIICROFF_HORIZONTAL(x)		((x) << 0)

#define EXYNOS_ORGISIZE_VERTICAL(x)		((x) << 16)
#define EXYNOS_ORGISIZE_HORIZONTAL(x)		((x) << 0)

#define EXYNOS_ORGOSIZE_VERTICAL(x)		((x) << 16)
#define EXYNOS_ORGOSIZE_HORIZONTAL(x)		((x) << 0)

#define EXYNOS_CIEXTEN_TARGETH_EXT(x)		((((x) & 0x2000) >> 13) << 26)
#define EXYNOS_CIEXTEN_TARGETV_EXT(x)		((((x) & 0x2000) >> 13) << 24)
#define EXYNOS_CIEXTEN_MAINHORRATIO_EXT(x)		(((x) & 0x3F) << 10)
#define EXYNOS_CIEXTEN_MAINVERRATIO_EXT(x)		((x) & 0x3F)

/*
                      
*/
/*                        */
#define EXYNOS_CISRCFMT_ITU601_8BIT		(1 << 31)
#define EXYNOS_CISRCFMT_ITU656_8BIT		(0 << 31)
#define EXYNOS_CISRCFMT_ITU601_16BIT		(1 << 29)
#define EXYNOS_CISRCFMT_ORDER422_YCBYCR		(0 << 14)
#define EXYNOS_CISRCFMT_ORDER422_YCRYCB		(1 << 14)
#define EXYNOS_CISRCFMT_ORDER422_CBYCRY		(2 << 14)
#define EXYNOS_CISRCFMT_ORDER422_CRYCBY		(3 << 14)
/*                   */
#define EXYNOS_CISRCFMT_ORDER422_Y4CBCRCBCR	(0 << 14)
/*                   */
#define EXYNOS_CISRCFMT_ORDER422_Y4CRCBCRCB	(1 << 14)

/*                        */
#define EXYNOS_CIWDOFST_WINOFSEN			(1 << 31)
#define EXYNOS_CIWDOFST_CLROVFIY			(1 << 30)
#define EXYNOS_CIWDOFST_CLROVRLB			(1 << 29)
#define EXYNOS_CIWDOFST_WINHOROFST_MASK		(0x7ff << 16)
#define EXYNOS_CIWDOFST_CLROVFICB			(1 << 15)
#define EXYNOS_CIWDOFST_CLROVFICR			(1 << 14)
#define EXYNOS_CIWDOFST_WINVEROFST_MASK		(0xfff << 0)

/*                         */
#define EXYNOS_CIGCTRL_SWRST			(1 << 31)
#define EXYNOS_CIGCTRL_CAMRST_A			(1 << 30)
#define EXYNOS_CIGCTRL_SELCAM_ITU_B		(0 << 29)
#define EXYNOS_CIGCTRL_SELCAM_ITU_A		(1 << 29)
#define EXYNOS_CIGCTRL_SELCAM_ITU_MASK		(1 << 29)
#define EXYNOS_CIGCTRL_TESTPATTERN_NORMAL		(0 << 27)
#define EXYNOS_CIGCTRL_TESTPATTERN_COLOR_BAR	(1 << 27)
#define EXYNOS_CIGCTRL_TESTPATTERN_HOR_INC		(2 << 27)
#define EXYNOS_CIGCTRL_TESTPATTERN_VER_INC		(3 << 27)
#define EXYNOS_CIGCTRL_TESTPATTERN_MASK		(3 << 27)
#define EXYNOS_CIGCTRL_TESTPATTERN_SHIFT		(27)
#define EXYNOS_CIGCTRL_INVPOLPCLK			(1 << 26)
#define EXYNOS_CIGCTRL_INVPOLVSYNC			(1 << 25)
#define EXYNOS_CIGCTRL_INVPOLHREF			(1 << 24)
#define EXYNOS_CIGCTRL_IRQ_OVFEN			(1 << 22)
#define EXYNOS_CIGCTRL_HREF_MASK			(1 << 21)
#define EXYNOS_CIGCTRL_IRQ_EDGE			(0 << 20)
#define EXYNOS_CIGCTRL_IRQ_LEVEL			(1 << 20)
#define EXYNOS_CIGCTRL_IRQ_CLR			(1 << 19)
#define EXYNOS_CIGCTRL_IRQ_END_DISABLE		(1 << 18)
#define EXYNOS_CIGCTRL_IRQ_DISABLE			(0 << 16)
#define EXYNOS_CIGCTRL_IRQ_ENABLE			(1 << 16)
#define EXYNOS_CIGCTRL_SHADOW_DISABLE		(1 << 12)
#define EXYNOS_CIGCTRL_CAM_JPEG			(1 << 8)
#define EXYNOS_CIGCTRL_SELCAM_MIPI_B		(0 << 7)
#define EXYNOS_CIGCTRL_SELCAM_MIPI_A		(1 << 7)
#define EXYNOS_CIGCTRL_SELCAM_MIPI_MASK		(1 << 7)
#define EXYNOS_CIGCTRL_SELWB_CAMIF_CAMERA	(0 << 6)
#define EXYNOS_CIGCTRL_SELWB_CAMIF_WRITEBACK	(1 << 6)
#define EXYNOS_CIGCTRL_SELWRITEBACK_MASK		(1 << 10)
#define EXYNOS_CIGCTRL_SELWRITEBACK_A		(1 << 10)
#define EXYNOS_CIGCTRL_SELWRITEBACK_B		(0 << 10)
#define EXYNOS_CIGCTRL_SELWB_CAMIF_MASK		(1 << 6)
#define EXYNOS_CIGCTRL_CSC_ITU601			(0 << 5)
#define EXYNOS_CIGCTRL_CSC_ITU709			(1 << 5)
#define EXYNOS_CIGCTRL_CSC_MASK			(1 << 5)
#define EXYNOS_CIGCTRL_INVPOLHSYNC			(1 << 4)
#define EXYNOS_CIGCTRL_SELCAM_FIMC_ITU		(0 << 3)
#define EXYNOS_CIGCTRL_SELCAM_FIMC_MIPI		(1 << 3)
#define EXYNOS_CIGCTRL_SELCAM_FIMC_MASK		(1 << 3)
#define EXYNOS_CIGCTRL_PROGRESSIVE			(0 << 0)
#define EXYNOS_CIGCTRL_INTERLACE			(1 << 0)

/*                         */
#define EXYNOS_CIWDOFST_WINHOROFST2_MASK		(0xfff << 16)
#define EXYNOS_CIWDOFST_WINVEROFST2_MASK		(0xfff << 16)

/*                        */
#define EXYNOS_CITRGFMT_INROT90_CLOCKWISE		(1 << 31)
#define EXYNOS_CITRGFMT_OUTFORMAT_YCBCR420		(0 << 29)
#define EXYNOS_CITRGFMT_OUTFORMAT_YCBCR422		(1 << 29)
#define EXYNOS_CITRGFMT_OUTFORMAT_YCBCR422_1PLANE	(2 << 29)
#define EXYNOS_CITRGFMT_OUTFORMAT_RGB		(3 << 29)
#define EXYNOS_CITRGFMT_OUTFORMAT_MASK		(3 << 29)
#define EXYNOS_CITRGFMT_FLIP_SHIFT			(14)
#define EXYNOS_CITRGFMT_FLIP_NORMAL		(0 << 14)
#define EXYNOS_CITRGFMT_FLIP_X_MIRROR		(1 << 14)
#define EXYNOS_CITRGFMT_FLIP_Y_MIRROR		(2 << 14)
#define EXYNOS_CITRGFMT_FLIP_180			(3 << 14)
#define EXYNOS_CITRGFMT_FLIP_MASK			(3 << 14)
#define EXYNOS_CITRGFMT_OUTROT90_CLOCKWISE		(1 << 13)
#define EXYNOS_CITRGFMT_TARGETV_MASK		(0x1fff << 0)
#define EXYNOS_CITRGFMT_TARGETH_MASK		(0x1fff << 16)

/*                             */
#define EXYNOS_CIOCTRL_WEAVE_OUT			(1 << 31)
#define EXYNOS_CIOCTRL_WEAVE_MASK			(1 << 31)
#define EXYNOS_CIOCTRL_LASTENDEN			(1 << 30)
#define EXYNOS_CIOCTRL_ORDER2P_LSB_CBCR		(0 << 24)
#define EXYNOS_CIOCTRL_ORDER2P_LSB_CRCB		(1 << 24)
#define EXYNOS_CIOCTRL_ORDER2P_MSB_CRCB		(2 << 24)
#define EXYNOS_CIOCTRL_ORDER2P_MSB_CBCR		(3 << 24)
#define EXYNOS_CIOCTRL_ORDER2P_SHIFT		(24)
#define EXYNOS_CIOCTRL_ORDER2P_MASK		(3 << 24)
#define EXYNOS_CIOCTRL_YCBCR_3PLANE		(0 << 3)
#define EXYNOS_CIOCTRL_YCBCR_2PLANE		(1 << 3)
#define EXYNOS_CIOCTRL_YCBCR_PLANE_MASK		(1 << 3)
#define EXYNOS_CIOCTRL_LASTIRQ_ENABLE		(1 << 2)
#define EXYNOS_CIOCTRL_ALPHA_OUT			(0xff << 4)
#define EXYNOS_CIOCTRL_ORDER422_YCBYCR		(0 << 0)
#define EXYNOS_CIOCTRL_ORDER422_YCRYCB		(1 << 0)
#define EXYNOS_CIOCTRL_ORDER422_CBYCRY		(2 << 0)
#define EXYNOS_CIOCTRL_ORDER422_CRYCBY		(3 << 0)
#define EXYNOS_CIOCTRL_ORDER422_MASK		(3 << 0)

/*                              */
#define EXYNOS_CISCCTRL_SCALERBYPASS		(1 << 31)
#define EXYNOS_CISCCTRL_SCALEUP_H			(1 << 30)
#define EXYNOS_CISCCTRL_SCALEUP_V			(1 << 29)
#define EXYNOS_CISCCTRL_CSCR2Y_NARROW		(0 << 28)
#define EXYNOS_CISCCTRL_CSCR2Y_WIDE		(1 << 28)
#define EXYNOS_CISCCTRL_CSCY2R_NARROW		(0 << 27)
#define EXYNOS_CISCCTRL_CSCY2R_WIDE		(1 << 27)
#define EXYNOS_CISCCTRL_LCDPATHEN_FIFO		(1 << 26)
#define EXYNOS_CISCCTRL_PROGRESSIVE		(0 << 25)
#define EXYNOS_CISCCTRL_INTERLACE			(1 << 25)
#define EXYNOS_CISCCTRL_SCAN_MASK			(1 << 25)
#define EXYNOS_CISCCTRL_SCALERSTART		(1 << 15)
#define EXYNOS_CISCCTRL_INRGB_FMT_RGB565		(0 << 13)
#define EXYNOS_CISCCTRL_INRGB_FMT_RGB666		(1 << 13)
#define EXYNOS_CISCCTRL_INRGB_FMT_RGB888		(2 << 13)
#define EXYNOS_CISCCTRL_INRGB_FMT_RGB_MASK		(3 << 13)
#define EXYNOS_CISCCTRL_OUTRGB_FMT_RGB565		(0 << 11)
#define EXYNOS_CISCCTRL_OUTRGB_FMT_RGB666		(1 << 11)
#define EXYNOS_CISCCTRL_OUTRGB_FMT_RGB888		(2 << 11)
#define EXYNOS_CISCCTRL_OUTRGB_FMT_RGB_MASK	(3 << 11)
#define EXYNOS_CISCCTRL_EXTRGB_NORMAL		(0 << 10)
#define EXYNOS_CISCCTRL_EXTRGB_EXTENSION		(1 << 10)
#define EXYNOS_CISCCTRL_ONE2ONE			(1 << 9)
#define EXYNOS_CISCCTRL_MAIN_V_RATIO_MASK		(0x1ff << 0)
#define EXYNOS_CISCCTRL_MAIN_H_RATIO_MASK		(0x1ff << 16)

/*                 */
#define EXYNOS_CISTATUS_OVFIY			(1 << 31)
#define EXYNOS_CISTATUS_OVFICB			(1 << 30)
#define EXYNOS_CISTATUS_OVFICR			(1 << 29)
#define EXYNOS_CISTATUS_VSYNC			(1 << 28)
#define EXYNOS_CISTATUS_SCALERSTART		(1 << 26)
#define EXYNOS_CISTATUS_WINOFSTEN			(1 << 25)
#define EXYNOS_CISTATUS_IMGCPTEN			(1 << 22)
#define EXYNOS_CISTATUS_IMGCPTENSC			(1 << 21)
#define EXYNOS_CISTATUS_VSYNC_A			(1 << 20)
#define EXYNOS_CISTATUS_VSYNC_B			(1 << 19)
#define EXYNOS_CISTATUS_OVRLB			(1 << 18)
#define EXYNOS_CISTATUS_FRAMEEND			(1 << 17)
#define EXYNOS_CISTATUS_LASTCAPTUREEND		(1 << 16)
#define EXYNOS_CISTATUS_VVALID_A			(1 << 15)
#define EXYNOS_CISTATUS_VVALID_B			(1 << 14)

/*                               */
#define EXYNOS_CIIMGCPT_IMGCPTEN			(1 << 31)
#define EXYNOS_CIIMGCPT_IMGCPTEN_SC		(1 << 30)
#define EXYNOS_CIIMGCPT_CPT_FREN_ENABLE		(1 << 25)
#define EXYNOS_CIIMGCPT_CPT_FRMOD_EN		(0 << 18)
#define EXYNOS_CIIMGCPT_CPT_FRMOD_CNT		(1 << 18)

/*                        */
#define EXYNOS_CIIMGEFF_IE_DISABLE			(0 << 30)
#define EXYNOS_CIIMGEFF_IE_ENABLE			(1 << 30)
#define EXYNOS_CIIMGEFF_IE_SC_BEFORE		(0 << 29)
#define EXYNOS_CIIMGEFF_IE_SC_AFTER		(1 << 29)
#define EXYNOS_CIIMGEFF_FIN_BYPASS			(0 << 26)
#define EXYNOS_CIIMGEFF_FIN_ARBITRARY		(1 << 26)
#define EXYNOS_CIIMGEFF_FIN_NEGATIVE		(2 << 26)
#define EXYNOS_CIIMGEFF_FIN_ARTFREEZE		(3 << 26)
#define EXYNOS_CIIMGEFF_FIN_EMBOSSING		(4 << 26)
#define EXYNOS_CIIMGEFF_FIN_SILHOUETTE		(5 << 26)
#define EXYNOS_CIIMGEFF_FIN_MASK			(7 << 26)
#define EXYNOS_CIIMGEFF_PAT_CBCR_MASK		((0xff < 13) | (0xff < 0))

/*                              */
#define EXYNOS_CIREAL_ISIZE_AUTOLOAD_ENABLE	(1 << 31)
#define EXYNOS_CIREAL_ISIZE_ADDR_CH_DISABLE	(1 << 30)
#define EXYNOS_CIREAL_ISIZE_HEIGHT_MASK		(0x3FFF << 16)
#define EXYNOS_CIREAL_ISIZE_WIDTH_MASK		(0x3FFF << 0)

/*                            */
#define EXYNOS_MSCTRL_FIELD_MASK			(1 << 31)
#define EXYNOS_MSCTRL_FIELD_WEAVE			(1 << 31)
#define EXYNOS_MSCTRL_FIELD_NORMAL			(0 << 31)
#define EXYNOS_MSCTRL_BURST_CNT			(24)
#define EXYNOS_MSCTRL_BURST_CNT_MASK		(0xf << 24)
#define EXYNOS_MSCTRL_ORDER2P_LSB_CBCR		(0 << 16)
#define EXYNOS_MSCTRL_ORDER2P_LSB_CRCB		(1 << 16)
#define EXYNOS_MSCTRL_ORDER2P_MSB_CRCB		(2 << 16)
#define EXYNOS_MSCTRL_ORDER2P_MSB_CBCR		(3 << 16)
#define EXYNOS_MSCTRL_ORDER2P_SHIFT		(16)
#define EXYNOS_MSCTRL_ORDER2P_SHIFT_MASK		(0x3 << 16)
#define EXYNOS_MSCTRL_C_INT_IN_3PLANE		(0 << 15)
#define EXYNOS_MSCTRL_C_INT_IN_2PLANE		(1 << 15)
#define EXYNOS_MSCTRL_FLIP_SHIFT			(13)
#define EXYNOS_MSCTRL_FLIP_NORMAL			(0 << 13)
#define EXYNOS_MSCTRL_FLIP_X_MIRROR		(1 << 13)
#define EXYNOS_MSCTRL_FLIP_Y_MIRROR		(2 << 13)
#define EXYNOS_MSCTRL_FLIP_180			(3 << 13)
#define EXYNOS_MSCTRL_FLIP_MASK			(3 << 13)
#define EXYNOS_MSCTRL_ORDER422_CRYCBY		(0 << 4)
#define EXYNOS_MSCTRL_ORDER422_YCRYCB		(1 << 4)
#define EXYNOS_MSCTRL_ORDER422_CBYCRY		(2 << 4)
#define EXYNOS_MSCTRL_ORDER422_YCBYCR		(3 << 4)
#define EXYNOS_MSCTRL_INPUT_EXTCAM			(0 << 3)
#define EXYNOS_MSCTRL_INPUT_MEMORY			(1 << 3)
#define EXYNOS_MSCTRL_INPUT_MASK			(1 << 3)
#define EXYNOS_MSCTRL_INFORMAT_YCBCR420		(0 << 1)
#define EXYNOS_MSCTRL_INFORMAT_YCBCR422		(1 << 1)
#define EXYNOS_MSCTRL_INFORMAT_YCBCR422_1PLANE	(2 << 1)
#define EXYNOS_MSCTRL_INFORMAT_RGB			(3 << 1)
#define EXYNOS_MSCTRL_ENVID			(1 << 0)

/*                        */
#define EXYNOS_CIDMAPARAM_R_MODE_LINEAR		(0 << 29)
#define EXYNOS_CIDMAPARAM_R_MODE_CONFTILE		(1 << 29)
#define EXYNOS_CIDMAPARAM_R_MODE_16X16		(2 << 29)
#define EXYNOS_CIDMAPARAM_R_MODE_64X32		(3 << 29)
#define EXYNOS_CIDMAPARAM_R_MODE_MASK		(3 << 29)
#define EXYNOS_CIDMAPARAM_R_TILE_HSIZE_64		(0 << 24)
#define EXYNOS_CIDMAPARAM_R_TILE_HSIZE_128		(1 << 24)
#define EXYNOS_CIDMAPARAM_R_TILE_HSIZE_256		(2 << 24)
#define EXYNOS_CIDMAPARAM_R_TILE_HSIZE_512		(3 << 24)
#define EXYNOS_CIDMAPARAM_R_TILE_HSIZE_1024	(4 << 24)
#define EXYNOS_CIDMAPARAM_R_TILE_HSIZE_2048	(5 << 24)
#define EXYNOS_CIDMAPARAM_R_TILE_HSIZE_4096	(6 << 24)
#define EXYNOS_CIDMAPARAM_R_TILE_VSIZE_1		(0 << 20)
#define EXYNOS_CIDMAPARAM_R_TILE_VSIZE_2		(1 << 20)
#define EXYNOS_CIDMAPARAM_R_TILE_VSIZE_4		(2 << 20)
#define EXYNOS_CIDMAPARAM_R_TILE_VSIZE_8		(3 << 20)
#define EXYNOS_CIDMAPARAM_R_TILE_VSIZE_16		(4 << 20)
#define EXYNOS_CIDMAPARAM_R_TILE_VSIZE_32		(5 << 20)
#define EXYNOS_CIDMAPARAM_W_MODE_LINEAR		(0 << 13)
#define EXYNOS_CIDMAPARAM_W_MODE_CONFTILE		(1 << 13)
#define EXYNOS_CIDMAPARAM_W_MODE_16X16		(2 << 13)
#define EXYNOS_CIDMAPARAM_W_MODE_64X32		(3 << 13)
#define EXYNOS_CIDMAPARAM_W_MODE_MASK		(3 << 13)
#define EXYNOS_CIDMAPARAM_W_TILE_HSIZE_64		(0 << 8)
#define EXYNOS_CIDMAPARAM_W_TILE_HSIZE_128		(1 << 8)
#define EXYNOS_CIDMAPARAM_W_TILE_HSIZE_256		(2 << 8)
#define EXYNOS_CIDMAPARAM_W_TILE_HSIZE_512		(3 << 8)
#define EXYNOS_CIDMAPARAM_W_TILE_HSIZE_1024	(4 << 8)
#define EXYNOS_CIDMAPARAM_W_TILE_HSIZE_2048	(5 << 8)
#define EXYNOS_CIDMAPARAM_W_TILE_HSIZE_4096	(6 << 8)
#define EXYNOS_CIDMAPARAM_W_TILE_VSIZE_1		(0 << 4)
#define EXYNOS_CIDMAPARAM_W_TILE_VSIZE_2		(1 << 4)
#define EXYNOS_CIDMAPARAM_W_TILE_VSIZE_4		(2 << 4)
#define EXYNOS_CIDMAPARAM_W_TILE_VSIZE_8		(3 << 4)
#define EXYNOS_CIDMAPARAM_W_TILE_VSIZE_16		(4 << 4)
#define EXYNOS_CIDMAPARAM_W_TILE_VSIZE_32		(5 << 4)

/*                              */
#define EXYNOS_CIEXTEN_TARGETH_EXT_MASK		(1 << 26)
#define EXYNOS_CIEXTEN_TARGETV_EXT_MASK		(1 << 24)
#define EXYNOS_CIEXTEN_MAINHORRATIO_EXT_MASK	(0x3F << 10)
#define EXYNOS_CIEXTEN_MAINVERRATIO_EXT_MASK	(0x3F)
#define EXYNOS_CIEXTEN_YUV444_OUT			(1 << 22)

/*                                   */
#define EXYNOS_CLKSRC_HCLK				(0 << 1)
#define EXYNOS_CLKSRC_HCLK_MASK			(1 << 1)
#define EXYNOS_CLKSRC_SCLK				(1 << 1)

/*                           */
#define SYSREG_CAMERA_BLK			(0x0218)
#define SYSREG_FIMD0WB_DEST_MASK		(0x3 << 23)
#define SYSREG_FIMD0WB_DEST_SHIFT		23

#endif /*                    */
