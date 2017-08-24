/*
 * bfin_ppi.h - interface to Blackfin PPIs
 *
 * Copyright 2005-2010 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __ASM_BFIN_PPI_H__
#define __ASM_BFIN_PPI_H__

#include <linux/types.h>
#include <asm/blackfin.h>

/*
                                                                     
                                                                    
 */
#define __BFP(m) u16 m; u16 __pad_##m

/*
                            
 */
struct bfin_ppi_regs {
	__BFP(control);
	__BFP(status);
	__BFP(count);
	__BFP(delay);
	__BFP(frame);
};

/*
                             
 */
struct bfin_eppi_regs {
	__BFP(status);
	__BFP(hcount);
	__BFP(hdelay);
	__BFP(vcount);
	__BFP(vdelay);
	__BFP(frame);
	__BFP(line);
	__BFP(clkdiv);
	u32 control;
	u32 fs1w_hbl;
	u32 fs1p_avpl;
	u32 fs2w_lvb;
	u32 fs2p_lavf;
	u32 clip;
};

/*
                              
 */
struct bfin_eppi3_regs {
	u32 stat;
	u32 hcnt;
	u32 hdly;
	u32 vcnt;
	u32 vdly;
	u32 frame;
	u32 line;
	u32 clkdiv;
	u32 ctl;
	u32 fs1_wlhb;
	u32 fs1_paspl;
	u32 fs2_wlvb;
	u32 fs2_palpf;
	u32 imsk;
	u32 oddclip;
	u32 evenclip;
	u32 fs1_dly;
	u32 fs2_dly;
	u32 ctl2;
};

#undef __BFP

#ifdef EPPI0_CTL2
#define EPPI_STAT_CFIFOERR              0x00000001    /*                   */
#define EPPI_STAT_YFIFOERR              0x00000002    /*                 */
#define EPPI_STAT_LTERROVR              0x00000004    /*                     */
#define EPPI_STAT_LTERRUNDR             0x00000008    /*                      */
#define EPPI_STAT_FTERROVR              0x00000010    /*                      */
#define EPPI_STAT_FTERRUNDR             0x00000020    /*                       */
#define EPPI_STAT_ERRNCOR               0x00000040    /*                              */
#define EPPI_STAT_PXPERR                0x00000080    /*                 */
#define EPPI_STAT_ERRDET                0x00004000    /*                         */
#define EPPI_STAT_FLD                   0x00008000    /*                                */

#define EPPI_HCNT_VALUE                 0x0000FFFF    /*                                                                                                                                                  */

#define EPPI_HDLY_VALUE                 0x0000FFFF    /*                                                                                                      */

#define EPPI_VCNT_VALUE                 0x0000FFFF    /*                                                                                                            */

#define EPPI_VDLY_VALUE                 0x0000FFFF    /*                                                                                              */

#define EPPI_FRAME_VALUE                0x0000FFFF    /*                                                      */

#define EPPI_LINE_VALUE                 0x0000FFFF    /*                                               */

#define EPPI_CLKDIV_VALUE               0x0000FFFF    /*                        */

#define EPPI_CTL_EN                     0x00000001    /*            */
#define EPPI_CTL_DIR                    0x00000002    /*               */
#define EPPI_CTL_XFRTYPE                0x0000000C    /*                    */
#define EPPI_CTL_ACTIVE656              0x00000000    /*                                        */
#define EPPI_CTL_ENTIRE656              0x00000004    /*                                   */
#define EPPI_CTL_VERT656                0x00000008    /*                                             */
#define EPPI_CTL_NON656                 0x0000000C    /*                                    */
#define EPPI_CTL_FSCFG                  0x00000030    /*                          */
#define EPPI_CTL_SYNC0                  0x00000000    /*                    */
#define EPPI_CTL_SYNC1                  0x00000010    /*                    */
#define EPPI_CTL_SYNC2                  0x00000020    /*                    */
#define EPPI_CTL_SYNC3                  0x00000030    /*                    */
#define EPPI_CTL_FLDSEL                 0x00000040    /*                      */
#define EPPI_CTL_ITUTYPE                0x00000080    /*                              */
#define EPPI_CTL_BLANKGEN               0x00000100    /*                                                   */
#define EPPI_CTL_ICLKGEN                0x00000200    /*                           */
#define EPPI_CTL_IFSGEN                 0x00000400    /*                                */
#define EPPI_CTL_SIGNEXT                0x00000800    /*                */
#define EPPI_CTL_POLC                   0x00003000    /*                                                */
#define EPPI_CTL_POLC0                  0x00000000    /*                                  */
#define EPPI_CTL_POLC1                  0x00001000    /*                                  */
#define EPPI_CTL_POLC2                  0x00002000    /*                                  */
#define EPPI_CTL_POLC3                  0x00003000    /*                                  */
#define EPPI_CTL_POLS                   0x0000C000    /*                     */
#define EPPI_CTL_FS1HI_FS2HI            0x00000000    /*                                   */
#define EPPI_CTL_FS1LO_FS2HI            0x00004000    /*                                             */
#define EPPI_CTL_FS1HI_FS2LO            0x00008000    /*                                             */
#define EPPI_CTL_FS1LO_FS2LO            0x0000C000    /*                                  */
#define EPPI_CTL_DLEN                   0x00070000    /*             */
#define EPPI_CTL_DLEN08                 0x00000000    /*              */
#define EPPI_CTL_DLEN10                 0x00010000    /*               */
#define EPPI_CTL_DLEN12                 0x00020000    /*               */
#define EPPI_CTL_DLEN14                 0x00030000    /*               */
#define EPPI_CTL_DLEN16                 0x00040000    /*               */
#define EPPI_CTL_DLEN18                 0x00050000    /*               */
#define EPPI_CTL_DLEN20                 0x00060000    /*               */
#define EPPI_CTL_DLEN24                 0x00070000    /*               */
#define EPPI_CTL_DMIRR                  0x00080000    /*                */
#define EPPI_CTL_SKIPEN                 0x00100000    /*             */
#define EPPI_CTL_SKIPEO                 0x00200000    /*                  */
#define EPPI_CTL_PACKEN                 0x00400000    /*                    */
#define EPPI_CTL_SWAPEN                 0x00800000    /*             */
#define EPPI_CTL_SPLTEO                 0x01000000    /*                                 */
#define EPPI_CTL_SUBSPLTODD             0x02000000    /*                       */
#define EPPI_CTL_SPLTWRD                0x04000000    /*            */
#define EPPI_CTL_RGBFMTEN               0x08000000    /*                       */
#define EPPI_CTL_DMACFG                 0x10000000    /*                              */
#define EPPI_CTL_DMAFINEN               0x20000000    /*                   */
#define EPPI_CTL_MUXSEL                 0x40000000    /*            */
#define EPPI_CTL_CLKGATEN               0x80000000    /*                     */

#define EPPI_FS2_WLVB_F2VBAD            0xFF000000    /*                                                                                                    */
#define EPPI_FS2_WLVB_F2VBBD            0x00FF0000    /*                                                                                                     */
#define EPPI_FS2_WLVB_F1VBAD            0x0000FF00    /*                                                                                                     */
#define EPPI_FS2_WLVB_F1VBBD            0x000000FF    /*                                                                                                                                                                          */

#define EPPI_FS2_PALPF_F2ACT            0xFFFF0000    /*                                           */
#define EPPI_FS2_PALPF_F1ACT            0x0000FFFF    /*                                           */

#define EPPI_IMSK_CFIFOERR              0x00000001    /*                                                  */
#define EPPI_IMSK_YFIFOERR              0x00000002    /*                                                  */
#define EPPI_IMSK_LTERROVR              0x00000004    /*                                          */
#define EPPI_IMSK_LTERRUNDR             0x00000008    /*                                           */
#define EPPI_IMSK_FTERROVR              0x00000010    /*                                           */
#define EPPI_IMSK_FTERRUNDR             0x00000020    /*                                            */
#define EPPI_IMSK_ERRNCOR               0x00000040    /*                                                 */
#define EPPI_IMSK_PXPERR                0x00000080    /*                                */

#define EPPI_ODDCLIP_HIGHODD            0xFFFF0000
#define EPPI_ODDCLIP_LOWODD             0x0000FFFF

#define EPPI_EVENCLIP_HIGHEVEN          0xFFFF0000
#define EPPI_EVENCLIP_LOWEVEN           0x0000FFFF

#define EPPI_CTL2_FS1FINEN              0x00000002    /*                     */
#endif
#endif
