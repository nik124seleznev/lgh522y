/* mt6620_fm.h
 *
 * (C) Copyright 2011
 * MediaTek <www.MediaTek.com>
 * Hongcheng <hongcheng.xia@MediaTek.com>
 *
 * MT6620 FM Radio Driver --  head file
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
#ifndef __MT6620_FM_H__
#define __MT6620_FM_H__

#include "fm_typedef.h"

/*                                      */
#define MT6620_RSSI_TH_LONG    0xFF01	/*                                                  */
#define MT6620_RSSI_TH_SHORT   0xFEE0	/*                                               */
#define MT6620_CQI_TH          0x00E9	/*                                                             */
#define MT6620_SEEK_SPACE      1	/*                                        */
#define MT6620_SCAN_CH_SIZE    40	/*                                */
#define MT6620_BAND            1	/*                                                                                     */
#define MT6620_BAND_FREQ_L     875	/*                                                 */
#define MT6620_BAND_FREQ_H     1080	/*                                                   */
#define MT6620_DEEMPHASIS_50us TRUE

#define MT6620_SLAVE_ADDR    0xE0	/*                    */
#define MT6620_MAX_COUNT     100
#define MT6620_SCANTBL_SIZE  16	/*              */

#define AFC_ON  0x01
#if AFC_ON
#define FM_MAIN_CTRL_INIT  0x480
#else
#define FM_MAIN_CTRL_INIT  0x080
#endif

#define ext_clk			/*                                                                          */
#define MT6620_DEV			"MT6620"

#endif				/*                                */
