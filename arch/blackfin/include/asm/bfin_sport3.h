/*
 * bfin_sport - Analog Devices BF6XX SPORT registers
 *
 * Copyright (c) 2012 Analog Devices Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _BFIN_SPORT3_H_
#define _BFIN_SPORT3_H_

#include <linux/types.h>

#define SPORT_CTL_SPENPRI             0x00000001    /*                        */
#define SPORT_CTL_DTYPE               0x00000006    /*                  */
#define SPORT_CTL_RJUSTIFY_ZFILL      0x00000000    /*                                                       */
#define SPORT_CTL_RJUSTIFY_SFILL      0x00000002    /*                                                         */
#define SPORT_CTL_USE_U_LAW           0x00000004    /*                                      */
#define SPORT_CTL_USE_A_LAW           0x00000006    /*                                      */
#define SPORT_CTL_LSBF                0x00000008    /*                          */
#define SPORT_CTL_SLEN                0x000001F0    /*                           */
#define SPORT_CTL_PACK                0x00000200    /*                                 */
#define SPORT_CTL_ICLK                0x00000400    /*                       */
#define SPORT_CTL_OPMODE              0x00000800    /*                */
#define SPORT_CTL_CKRE                0x00001000    /*                          */
#define SPORT_CTL_FSR                 0x00002000    /*                     */
#define SPORT_CTL_IFS                 0x00004000    /*                            */
#define SPORT_CTL_DIFS                0x00008000    /*                                    */
#define SPORT_CTL_LFS                 0x00010000    /*                              */
#define SPORT_CTL_LAFS                0x00020000    /*                            */
#define SPORT_CTL_RJUST               0x00040000    /*                             */
#define SPORT_CTL_FSED                0x00080000    /*                                 */
#define SPORT_CTL_TFIEN               0x00100000    /*                                         */
#define SPORT_CTL_GCLKEN              0x00200000    /*                         */
#define SPORT_CTL_SPENSEC             0x01000000    /*                          */
#define SPORT_CTL_SPTRAN              0x02000000    /*                        */
#define SPORT_CTL_DERRSEC             0x04000000    /*                                */
#define SPORT_CTL_DXSSEC              0x18000000    /*                                      */
#define SPORT_CTL_SEC_EMPTY           0x00000000    /*               */
#define SPORT_CTL_SEC_PART_FULL       0x10000000    /*                        */
#define SPORT_CTL_SEC_FULL            0x18000000    /*              */
#define SPORT_CTL_DERRPRI             0x20000000    /*                              */
#define SPORT_CTL_DXSPRI              0xC0000000    /*                                    */
#define SPORT_CTL_PRM_EMPTY           0x00000000    /*               */
#define SPORT_CTL_PRM_PART_FULL       0x80000000    /*                        */
#define SPORT_CTL_PRM_FULL            0xC0000000    /*              */

#define SPORT_DIV_CLKDIV              0x0000FFFF    /*               */
#define SPORT_DIV_FSDIV               0xFFFF0000    /*                    */

#define SPORT_MCTL_MCE                0x00000001    /*                     */
#define SPORT_MCTL_MCPDE              0x00000004    /*                                  */
#define SPORT_MCTL_MFD                0x000000F0    /*                          */
#define SPORT_MCTL_WSIZE              0x00007F00    /*                              */
#define SPORT_MCTL_WOFFSET            0x03FF0000    /*                    */

#define SPORT_CNT_CLKCNT              0x0000FFFF    /*                                  */
#define SPORT_CNT_FSDIVCNT            0xFFFF0000    /*                                    */

#define SPORT_ERR_DERRPMSK            0x00000001    /*                                             */
#define SPORT_ERR_DERRSMSK            0x00000002    /*                                               */
#define SPORT_ERR_FSERRMSK            0x00000004    /*                                   */
#define SPORT_ERR_DERRPSTAT           0x00000010    /*                                   */
#define SPORT_ERR_DERRSSTAT           0x00000020    /*                                     */
#define SPORT_ERR_FSERRSTAT           0x00000040    /*                         */

#define SPORT_MSTAT_CURCHAN           0x000003FF    /*                                                               */

#define SPORT_CTL2_FSMUXSEL           0x00000001    /*                       */
#define SPORT_CTL2_CKMUXSEL           0x00000002    /*                  */
#define SPORT_CTL2_LBSEL              0x00000004    /*                 */

struct sport_register {
	u32 spctl;
	u32 div;
	u32 spmctl;
	u32 spcs0;
	u32 spcs1;
	u32 spcs2;
	u32 spcs3;
	u32 spcnt;
	u32 sperrctl;
	u32 spmstat;
	u32 spctl2;
	u32 txa;
	u32 rxa;
	u32 txb;
	u32 rxb;
	u32 revid;
};

struct bfin_snd_platform_data {
	const unsigned short *pin_req;
};

#endif
