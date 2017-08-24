/*******************************************************************************

  Intel PRO/1000 Linux driver
  Copyright(c) 1999 - 2013 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information:
  Linux NICS <linux.nics@intel.com>
  e1000-devel Mailing List <e1000-devel@lists.sourceforge.net>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

*******************************************************************************/

#ifndef _E1000E_80003ES2LAN_H_
#define _E1000E_80003ES2LAN_H_

#define E1000_KMRNCTRLSTA_OFFSET_FIFO_CTRL	0x00
#define E1000_KMRNCTRLSTA_OFFSET_INB_CTRL	0x02
#define E1000_KMRNCTRLSTA_OFFSET_HD_CTRL	0x10
#define E1000_KMRNCTRLSTA_OFFSET_MAC2PHY_OPMODE	0x1F

#define E1000_KMRNCTRLSTA_FIFO_CTRL_RX_BYPASS	0x0008
#define E1000_KMRNCTRLSTA_FIFO_CTRL_TX_BYPASS	0x0800
#define E1000_KMRNCTRLSTA_INB_CTRL_DIS_PADDING	0x0010

#define E1000_KMRNCTRLSTA_HD_CTRL_10_100_DEFAULT 0x0004
#define E1000_KMRNCTRLSTA_HD_CTRL_1000_DEFAULT	0x0000
#define E1000_KMRNCTRLSTA_OPMODE_E_IDLE		0x2000

#define E1000_KMRNCTRLSTA_OPMODE_MASK		0x000C
#define E1000_KMRNCTRLSTA_OPMODE_INBAND_MDIO	0x0004

#define E1000_TCTL_EXT_GCEX_MASK 0x000FFC00	/*                          */
#define DEFAULT_TCTL_EXT_GCEX_80003ES2LAN	0x00010000

#define DEFAULT_TIPG_IPGT_1000_80003ES2LAN	0x8
#define DEFAULT_TIPG_IPGT_10_100_80003ES2LAN	0x9

/*                                                           */
#define GG82563_PSCR_POLARITY_REVERSAL_DISABLE	0x0002	/*                */
#define GG82563_PSCR_CROSSOVER_MODE_MASK	0x0060
#define GG82563_PSCR_CROSSOVER_MODE_MDI		0x0000	/*               */
#define GG82563_PSCR_CROSSOVER_MODE_MDIX	0x0020	/*                */
#define GG82563_PSCR_CROSSOVER_MODE_AUTO	0x0060	/*                   */

/*                                                       */
#define GG82563_PSCR2_REVERSE_AUTO_NEG		0x2000	/*                    */

/*                                                     */
/*                                                                      */
#define GG82563_MSCR_TX_CLK_MASK		0x0007
#define GG82563_MSCR_TX_CLK_10MBPS_2_5		0x0004
#define GG82563_MSCR_TX_CLK_100MBPS_25		0x0005
#define GG82563_MSCR_TX_CLK_1000MBPS_25		0x0007

#define GG82563_MSCR_ASSERT_CRS_ON_TX		0x0010	/*          */

/*                                            
           
             
              
               
            
 */
#define GG82563_DSPD_CABLE_LENGTH		0x0007

/*                                                       */
#define GG82563_KMCR_PASS_FALSE_CARRIER		0x0800

/*                                                            */
#define GG82563_MAX_KMRN_RETRY			0x5

/*                                                           */
/*                                 */
#define GG82563_PMCR_ENABLE_ELECTRICAL_IDLE	0x0001

/*                                                  */
#define GG82563_ICR_DIS_PADDING			0x0010	/*                 */

#endif
