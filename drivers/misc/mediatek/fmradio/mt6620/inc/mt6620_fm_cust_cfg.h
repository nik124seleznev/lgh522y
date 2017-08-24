/*
 *
 * (C) Copyright 2009
 * MediaTek <www.MediaTek.com>
 * William Chung <William.Chung@MediaTek.com>
 *
 * MT6516 AR10x0 FM Radio Driver
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

#ifndef __FM_CUST_CFG_H__
#define __FM_CUST_CFG_H__

/*                     */
enum {
	FM_SCAN_SORT_NON = 0,
	FM_SCAN_SORT_UP,
	FM_SCAN_SORT_DOWN,
	FM_SCAN_SORT_MAX
};
/*                                                                                               */
/*    */
#define FMR_RSSI_TH_LONG_MT6620    0x0301	/*                                                  */
#define FMR_RSSI_TH_SHORT_MT6620   0x02E0	/*                                               */
#define FMR_CQI_TH_MT6620          0x00E9	/*                                                             */
/*                                                                                               */
/*    */
#define FMR_MR_TH_MT6620           0x01BD	/*                       */
#if 0
/*                                                                                           */
/*                                                                                               */
/*                                                                                           */
#define FMR_SEEK_SPACE      1	/*                                        */
#define FMR_SCAN_CH_SIZE    40	/*                                */
#define FMR_BAND            1	/*                                                                                     */
#define FM_SCAN_SORT_SELECT FM_SCAN_SORT_NON
#define FM_FAKE_CH_NUM      1
#define FM_FAKE_CH_RSSI     40
#define FM_FAKE_CH_1        1075
#define FM_FAKE_CH_2        0
#define FM_FAKE_CH_3        0
#define FM_FAKE_CH_4        0
#define FM_FAKE_CH_5        0

/*    */
#define FMTX_PWR_LEVEL_MAX  120

/*                                                                                           */
#define ADDR_SCAN_TH        0xE0	/*                        */
#define ADDR_CQI_TH         0xE1	/*                   */

/*    */
#define FMTX_SCAN_HOLE_LOW  923	/*                                         */
#define FMTX_SCAN_HOLE_HIGH 954	/*                                         */
/*                                                                                           */

/*       */
#define FM_SUCCESS      0
#define FM_FAILED       1
#define FM_EPARM        2
#define FM_BADSTATUS    3
#define FM_TUNE_FAILED  4
#define FM_SEEK_FAILED  5
#define FM_BUSY         6
#define FM_SCAN_FAILED  7

/*                  */
#define FM_MAX_CHL_SIZE FMR_SCAN_CH_SIZE
/*           */
#define FM_AUTO_HILO_OFF    0
#define FM_AUTO_HILO_ON     1

#define FM_CHIP_AR1000 0x1000
#define FM_CHIP_MT5192 0x91
#define FM_CHIP_MT5193 0x92
#define FM_CHIP_MT6616 0x6616
#define FM_CHIP_MT6626 0x6626
#define FM_CHIP_MT6628 0x6628
#define FM_CHIP_MT6620 0x6620
#define FM_CHIP_UNSUPPORTED 0xffff

/*                */
#define FM_SEEKTH_LEVEL_DEFAULT 4
#endif
/*    */
#define FM_RX_RSSI_TH_LONG_MT6620    -296	/*                                              */
#define FM_RX_RSSI_TH_SHORT_MT6620   -296	/*                                               */
#define FM_RX_DESENSE_RSSI_MT6620   -245
#define FM_RX_PAMD_TH_MT6620          -12
#define FM_RX_MR_TH_MT6620           -67
#define FM_RX_ATDC_TH_MT6620           219
#define FM_RX_PRX_TH_MT6620           64
#define FM_RX_SMG_TH_MT6620          6	/*                             */
#define FM_RX_DEEMPHASIS_MT6620       0	/*                                             */
#define FM_RX_OSC_FREQ_MT6620         0	/*                                                        */
#if 0
#define FM_RX_RSSI_TH_LONG_MT6620    0xF2D8	/*                                              */
#define FM_RX_RSSI_TH_SHORT_MT6620   0xF2D8	/*                                               */
#define FM_RX_CQI_TH_MT6620          0x00E9	/*                                                             */
#define FM_RX_MR_TH_MT6620           0x01BD	/*                       */
#define FM_RX_SMG_TH_MT6620          0x4025	/*                             */
#define FM_RX_SEEK_SPACE_MT6620      1	/*                                        */
#define FM_RX_SCAN_CH_SIZE_MT6620    40	/*                                */
#define FM_RX_BAND_MT6620            1	/*                                                                                     */

#define FM_RX_SCAN_SORT_SELECT_MT6620 FM_SCAN_SORT_NON
#define FM_RX_FAKE_CH_NUM_MT6620      1
#define FM_RX_FAKE_CH_RSSI_MT6620     40
#define FM_RX_FAKE_CH_1_MT6620        1075
#define FM_RX_FAKE_CH_2_MT6620        0
#define FM_RX_FAKE_CH_3_MT6620        0
#define FM_RX_FAKE_CH_4_MT6620        0
#define FM_RX_FAKE_CH_5_MT6620        0
#endif
/*    */
#define FM_TX_PWR_LEVEL_MAX_MT6620  120
#define FM_TX_SCAN_HOLE_LOW_MT6620  923	/*                                         */
#define FM_TX_SCAN_HOLE_HIGH_MT6620 954	/*                                         */

#endif				/*                   */
