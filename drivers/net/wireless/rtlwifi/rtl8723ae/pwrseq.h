/******************************************************************************
 *
 * Copyright(c) 2009-2012  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

#ifndef __RTL8723E_PWRSEQ_H__
#define __RTL8723E_PWRSEQ_H__

#include "pwrseqcmd.h"
/*
                                                                    
                             
                   
                   
                           
                    
                        
                

                                                       
                     
                     
                     
                     
                     
                 
                 

          
*/

#define	RTL8723A_TRANS_CARDEMU_TO_ACT_STPS	10
#define	RTL8723A_TRANS_ACT_TO_CARDEMU_STPS	10
#define	RTL8723A_TRANS_CARDEMU_TO_SUS_STPS	10
#define	RTL8723A_TRANS_SUS_TO_CARDEMU_STPS	10
#define	RTL8723A_TRANS_CARDEMU_TO_PDN_STPS	10
#define	RTL8723A_TRANS_PDN_TO_CARDEMU_STPS	10
#define	RTL8723A_TRANS_ACT_TO_LPS_STPS		15
#define	RTL8723A_TRANS_LPS_TO_ACT_STPS		15
#define	RTL8723A_TRANS_END_STPS			1


#define RTL8723A_TRANS_CARDEMU_TO_ACT					\
	/*        */							\
	/*                                                                    
                  */						\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(2), 0},		\
		/*                          */				\
	{0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(1), BIT(1)},	\
		/*                                      */		\
	{0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0), BIT(0)},	\
		/*                               */			\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), 0},		\
		/*                         */				\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, (BIT(4)|BIT(3)), 0},	\
	/*                   */						\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0), BIT(0)},	\
		/*                       */				\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(0), 0}

#define RTL8723A_TRANS_ACT_TO_CARDEMU					\
	/*        */							\
	/*                                                                    
                  */						\
	{0x001F, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0},		\
		/*                         */				\
	{0x004E, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), 0},		\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(1), BIT(1)},	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(1), 0}

#define RTL8723A_TRANS_CARDEMU_TO_SUS					\
	/*        */							\
	/*                                                                    
                  */						\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4)|BIT(3),		\
		(BIT(4)|BIT(3))},					\
		/*                                              */	\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK |	\
		PWR_INTF_SDIO_MSK,					\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3)|BIT(4), BIT(3)},\
		 /*                                     */		\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,	\
		PWR_BASEADDR_MAC,					\
		PWR_CMD_WRITE, BIT(3)|BIT(4), BIT(3)|BIT(4)},		\
		 /*                                              */	\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,	\
		PWR_BASEADDR_SDIO,					\
		PWR_CMD_WRITE, BIT(0), BIT(0)},				\
		/*                               */			\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,	\
		PWR_BASEADDR_SDIO,					\
		PWR_CMD_POLLING, BIT(1), 0}				\
		/*                           */

#define RTL8723A_TRANS_SUS_TO_CARDEMU					\
	/*        */							\
	/*                                                                   */\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,	\
		PWR_BASEADDR_SDIO, PWR_CMD_WRITE, BIT(0), 0},		\
		/*                               */			\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,	\
		PWR_BASEADDR_SDIO, PWR_CMD_POLLING, BIT(1), BIT(1)},	\
		/*                           */				\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3)|BIT(4), 0}	\
		/*                                    */

#define RTL8723A_TRANS_CARDEMU_TO_CARDDIS				\
	/*        */							\
	/*                                                                   */\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK,			\
	PWR_INTF_USB_MSK|PWR_INTF_SDIO_MSK,				\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3)|BIT(4), BIT(3)},\
		/*                                     */		\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(2), BIT(2)},	\
		/*                           */				\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,	\
		PWR_BASEADDR_SDIO, PWR_CMD_WRITE, BIT(0), BIT(0)},	\
		/*                               */			\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,	\
		PWR_BASEADDR_SDIO, PWR_CMD_POLLING, BIT(1), 0}		\
		/*                           */

#define RTL8723A_TRANS_CARDDIS_TO_CARDEMU				\
	/*        */							\
	/*                                                                   */\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,	\
		PWR_BASEADDR_SDIO, PWR_CMD_WRITE, BIT(0), 0},		\
		/*                               */			\
	{0x0086, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,	\
		PWR_BASEADDR_SDIO, PWR_CMD_POLLING, BIT(1), BIT(1)},	\
		/*                           */				\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(3)|BIT(4), 0},	\
		/*                                    */		\
	{0x0301, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0}		\
		/*              */

#define RTL8723A_TRANS_CARDEMU_TO_PDN					\
	/*        */							\
	/*                                                                   */\
	{0x0006, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0), 0},		\
		/*             */\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), BIT(7)}	\
		/*             */

#define RTL8723A_TRANS_PDN_TO_CARDEMU					\
	/*        */							\
	/*                                                                   */\
	{0x0005, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(7), 0}		\
		/*             */

#define RTL8723A_TRANS_ACT_TO_LPS					\
	/*        */							\
	/*                                                                   */\
	{0x0301, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0xFF},		\
		/*             */					\
	{0x0522, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0x7F},		\
		/*        */						\
	{0x05F8, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_POLLING, 0xFF, 0},		\
		/*                                           */		\
	{0x05F9, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_POLLING, 0xFF, 0},		\
		/*                                           */		\
	{0x05FA, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_POLLING, 0xFF, 0},		\
		/*                                           */		\
	{0x05FB, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_POLLING, 0xFF, 0},		\
		/*                                           */		\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(0), 0},		\
		/*                                             */	\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_DELAY, 0, PWRSEQ_DELAY_US},	\
		/*         */						\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(1), 0},		\
		/*                 */					\
	{0x0100, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0x3F},		\
		/*             */					\
	{0x0101, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(1), 0},		\
		/*                      */				\
	{0x0553, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(5), BIT(5)}	\
		/*                         */

#define RTL8723A_TRANS_LPS_TO_ACT					\
	/*        */							\
	/*                                                                   */\
	{0x0080, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK,	\
		 PWR_BASEADDR_SDIO, PWR_CMD_WRITE, 0xFF, 0x84},		\
		 /*         */						\
	{0xFE58, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0x84},		\
		/*        */						\
	{0x0361, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0x84},		\
		/*         */						\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		 PWR_BASEADDR_MAC, PWR_CMD_DELAY, 0, PWRSEQ_DELAY_MS},	\
		/*     */						\
	{0x0008, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(4), 0},		\
		/*                              */			\
	{0x0109, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_POLLING, BIT(7), 0},		\
		/*                              */			\
	{0x0029, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(6)|BIT(7), 0},	\
		/*                                    */	\
	{0x0101, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(1), BIT(1)},	\
		/*              */					\
	{0x0100, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0xFF},		\
		/*                                  */			\
	{0x0002, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, BIT(1)|BIT(0),		\
		BIT(1)|BIT(0)},						\
		/*                                   */			\
	{0x0522, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
		PWR_BASEADDR_MAC, PWR_CMD_WRITE, 0xFF, 0}		\
		/*           */

#define RTL8723A_TRANS_END						\
	/*        */							\
	/*                                                                   */\
	{0xFFFF, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_ALL_MSK,	\
	0, PWR_CMD_END, 0, 0}

extern struct
wlan_pwr_cfg rtl8723A_power_on_flow[RTL8723A_TRANS_CARDEMU_TO_ACT_STPS
				    + RTL8723A_TRANS_END_STPS];
extern struct
wlan_pwr_cfg rtl8723A_radio_off_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
				     + RTL8723A_TRANS_END_STPS];
extern struct
wlan_pwr_cfg rtl8723A_card_disable_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
					+ RTL8723A_TRANS_CARDEMU_TO_PDN_STPS
					+ RTL8723A_TRANS_END_STPS];
extern struct
wlan_pwr_cfg rtl8723A_card_enable_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
				       + RTL8723A_TRANS_CARDEMU_TO_PDN_STPS
				       + RTL8723A_TRANS_END_STPS];
extern struct
wlan_pwr_cfg rtl8723A_suspend_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
				   + RTL8723A_TRANS_CARDEMU_TO_SUS_STPS
				   + RTL8723A_TRANS_END_STPS];
extern struct
wlan_pwr_cfg rtl8723A_resume_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
				  + RTL8723A_TRANS_CARDEMU_TO_SUS_STPS
				  + RTL8723A_TRANS_END_STPS];
extern struct
wlan_pwr_cfg rtl8723A_hwpdn_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
				 + RTL8723A_TRANS_CARDEMU_TO_PDN_STPS
				 + RTL8723A_TRANS_END_STPS];
extern struct
wlan_pwr_cfg rtl8723A_enter_lps_flow[RTL8723A_TRANS_ACT_TO_LPS_STPS
				     + RTL8723A_TRANS_END_STPS];
extern struct
wlan_pwr_cfg rtl8723A_leave_lps_flow[RTL8723A_TRANS_LPS_TO_ACT_STPS
				     + RTL8723A_TRANS_END_STPS];

/*                                                     */
#define Rtl8723_NIC_PWR_ON_FLOW		rtl8723A_power_on_flow
#define Rtl8723_NIC_RF_OFF_FLOW		rtl8723A_radio_off_flow
#define Rtl8723_NIC_DISABLE_FLOW	rtl8723A_card_disable_flow
#define Rtl8723_NIC_ENABLE_FLOW		rtl8723A_card_enable_flow
#define Rtl8723_NIC_SUSPEND_FLOW	rtl8723A_suspend_flow
#define Rtl8723_NIC_RESUME_FLOW		rtl8723A_resume_flow
#define Rtl8723_NIC_PDN_FLOW		rtl8723A_hwpdn_flow
#define Rtl8723_NIC_LPS_ENTER_FLOW	rtl8723A_enter_lps_flow
#define Rtl8723_NIC_LPS_LEAVE_FLOW	rtl8723A_leave_lps_flow

#endif
