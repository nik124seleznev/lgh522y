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

#include "pwrseqcmd.h"
#include "pwrseq.h"

/*                                                                    */

/*                 */
struct wlan_pwr_cfg rtl8723A_power_on_flow[RTL8723A_TRANS_CARDEMU_TO_ACT_STPS
					+ RTL8723A_TRANS_END_STPS] = {
	RTL8723A_TRANS_CARDEMU_TO_ACT,
	RTL8723A_TRANS_END
};

/*                      */
struct wlan_pwr_cfg rtl8723A_radio_off_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
					+ RTL8723A_TRANS_END_STPS] = {
	RTL8723A_TRANS_ACT_TO_CARDEMU,
	RTL8723A_TRANS_END
};

/*                   */
struct wlan_pwr_cfg
rtl8723A_card_disable_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
			  + RTL8723A_TRANS_CARDEMU_TO_PDN_STPS
			  + RTL8723A_TRANS_END_STPS] = {
	RTL8723A_TRANS_ACT_TO_CARDEMU,
	RTL8723A_TRANS_CARDEMU_TO_CARDDIS,
	RTL8723A_TRANS_END
};

/*                   */
struct wlan_pwr_cfg rtl8723A_card_enable_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
					+ RTL8723A_TRANS_CARDEMU_TO_PDN_STPS
					+ RTL8723A_TRANS_END_STPS] = {
	RTL8723A_TRANS_CARDDIS_TO_CARDEMU,
	RTL8723A_TRANS_CARDEMU_TO_ACT,
	RTL8723A_TRANS_END
};

/*              */
struct wlan_pwr_cfg rtl8723A_suspend_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
					+ RTL8723A_TRANS_CARDEMU_TO_SUS_STPS
					+ RTL8723A_TRANS_END_STPS] = {
	RTL8723A_TRANS_ACT_TO_CARDEMU,
	RTL8723A_TRANS_CARDEMU_TO_SUS,
	RTL8723A_TRANS_END
};

/*              */
struct wlan_pwr_cfg rtl8723A_resume_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
					+ RTL8723A_TRANS_CARDEMU_TO_SUS_STPS
					+ RTL8723A_TRANS_END_STPS] = {
	RTL8723A_TRANS_SUS_TO_CARDEMU,
	RTL8723A_TRANS_CARDEMU_TO_ACT,
	RTL8723A_TRANS_END
};

/*            */
struct wlan_pwr_cfg rtl8723A_hwpdn_flow[RTL8723A_TRANS_ACT_TO_CARDEMU_STPS
				+ RTL8723A_TRANS_CARDEMU_TO_PDN_STPS
				+ RTL8723A_TRANS_END_STPS] = {
	RTL8723A_TRANS_ACT_TO_CARDEMU,
	RTL8723A_TRANS_CARDEMU_TO_PDN,
	RTL8723A_TRANS_END
};

/*            */
struct wlan_pwr_cfg rtl8723A_enter_lps_flow[RTL8723A_TRANS_ACT_TO_LPS_STPS
					+ RTL8723A_TRANS_END_STPS] = {
	/*           */
	RTL8723A_TRANS_ACT_TO_LPS,
	RTL8723A_TRANS_END
};

/*            */
struct wlan_pwr_cfg rtl8723A_leave_lps_flow[RTL8723A_TRANS_LPS_TO_ACT_STPS
					+ RTL8723A_TRANS_END_STPS] = {
	/*           */
	RTL8723A_TRANS_LPS_TO_ACT,
	RTL8723A_TRANS_END
};