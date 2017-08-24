/* mt6628_fm.h
 *
 * (C) Copyright 2009
 * MediaTek <www.MediaTek.com>
 * Hongcheng <hongcheng.xia@MediaTek.com>
 *
 * MT6626 FM Radio Driver --  head file
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __MT6626_FM_H__
#define __MT6626_FM_H__

#include "fm_typedef.h"

/*                                      */
#define MT6626_RSSI_TH_LONG    0xFF01	/*                                                  */
#define MT6626_RSSI_TH_SHORT   0xFEE0	/*                                               */
#define MT6626_CQI_TH          0x00E9	/*                                                             */
#define MT6626_SEEK_SPACE      1	/*                                        */
#define MT6626_SCAN_CH_SIZE    40	/*                                */
#define MT6626_BAND            1	/*                                                                                     */
#define MT6626_BAND_FREQ_L     875	/*                                                 */
#define MT6626_BAND_FREQ_H     1080	/*                                                   */
#define MT6626_DEEMPHASIS_50us TRUE

/*                                      */
#if 0				/*            */
#define MT6626_I2C_PORT   2
#else
#define MT6626_I2C_PORT   0
#endif

#define MT6626_SLAVE_ADDR    0xE0	/*                    */
#define MT6626_MAX_COUNT     100
#define MT6626_SCANTBL_SIZE  16	/*              */

#define AFC_ON  0x01
#if AFC_ON
#define FM_MAIN_CTRL_INIT  0x480
#else
#define FM_MAIN_CTRL_INIT  0x080
#endif

/*                     */
#define FM_EXT_STC_DONE_MASK 0x01
#define FM_EXT_RDS_MASK      0x20

#define MT6626_FM_STC_DONE_TIMEOUT 12	/*        */

/*                     */
#define FM_MAIN_CHANDET_MASK   0x3FF0	/*                       */
#define FM_MAIN_CHANDET_SHIFT  0x04
#define FM_HOST_CHAN	0x3FF0

/*                                          */
#define MT6626_FM_SEEK_UP       0x0
#define MT6626_FM_SEEK_DOWN     0x01
#define MT6626_FM_SCAN_UP       0x0
#define MT6626_FM_SCAN_DOWN     0x01
#define MT6626_FM_SPACE_INVALID 0x0
#define MT6626_FM_SPACE_50K     0x01
#define MT6626_FM_SPACE_100K    0x02
#define MT6626_FM_SPACE_200K    0x04

#define ext_clk			/*                                                                          */
#define MT6626_DEV			"MT6626"

#endif				/*                                */
