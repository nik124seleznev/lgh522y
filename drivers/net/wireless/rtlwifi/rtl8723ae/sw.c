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

#include "../wifi.h"
#include <linux/vmalloc.h>
#include <linux/module.h>

#include "../core.h"
#include "../pci.h"
#include "reg.h"
#include "def.h"
#include "phy.h"
#include "dm.h"
#include "hw.h"
#include "sw.h"
#include "trx.h"
#include "led.h"
#include "table.h"
#include "hal_btc.h"

static void rtl8723ae_init_aspm_vars(struct ieee80211_hw *hw)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));

	/*                             */
	rtlpci->const_amdpci_aspm = 0;

	/*              
                     
                                      
                                   
                                          
                                             
                                        
  */
	rtlpci->const_pci_aspm = 3;

	/*                         */
	rtlpci->const_devicepci_aspm_setting = 0x03;

	/*                         */
	rtlpci->const_hostpci_aspm_setting = 0x02;

	/*                              
                
                                              
                                           
              
                                          
  */
	rtlpci->const_hwsw_rfoff_d3 = 0;

	/*                                         
                                               
                         
                     
                             
  */
	rtlpci->const_support_pciaspm = 1;
}

int rtl8723ae_init_sw_vars(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	int err;

	rtl8723ae_bt_reg_init(hw);
	rtlpriv->dm.dm_initialgain_enable = 1;
	rtlpriv->dm.dm_flag = 0;
	rtlpriv->dm.disable_framebursting = 0;
	rtlpriv->dm.thermalvalue = 0;
	rtlpci->transmit_config = CFENDFORM | BIT(12) | BIT(13);

	/*                                               */
	rtlpriv->rtlhal.current_bandtype = BAND_ON_2_4G;
	rtlpriv->rtlhal.bandset = BAND_ON_2_4G;
	rtlpriv->rtlhal.macphymode = SINGLEMAC_SINGLEPHY;

	rtlpci->receive_config = (RCR_APPFCS |
				  RCR_APP_MIC |
				  RCR_APP_ICV |
				  RCR_APP_PHYST_RXFF |
				  RCR_HTC_LOC_CTRL |
				  RCR_AMF |
				  RCR_ACF |
				  RCR_ADF |
				  RCR_AICV |
				  RCR_AB |
				  RCR_AM |
				  RCR_APM |
				  0);

	rtlpci->irq_mask[0] =
	    (u32) (PHIMR_ROK |
		   PHIMR_RDU |
		   PHIMR_VODOK |
		   PHIMR_VIDOK |
		   PHIMR_BEDOK |
		   PHIMR_BKDOK |
		   PHIMR_MGNTDOK |
		   PHIMR_HIGHDOK |
		   PHIMR_C2HCMD |
		   PHIMR_HISRE_IND |
		   PHIMR_TSF_BIT32_TOGGLE |
		   PHIMR_TXBCNOK |
		   PHIMR_PSTIMEOUT |
		   0);

	rtlpci->irq_mask[1] = (u32)(PHIMR_RXFOVW | 0);

	/*                 */
	rtlpriv->dbg.global_debuglevel = rtlpriv->cfg->mod_params->debug;
	/*               */
	rtlpriv->psc.inactiveps = rtlpriv->cfg->mod_params->inactiveps;
	rtlpriv->psc.swctrl_lps = rtlpriv->cfg->mod_params->swctrl_lps;
	rtlpriv->psc.fwctrl_lps = rtlpriv->cfg->mod_params->fwctrl_lps;
	rtlpriv->psc.reg_fwctrl_lps = 3;
	rtlpriv->psc.reg_max_lps_awakeintvl = 5;
	/*                                     
                                 
  */
	rtl8723ae_init_aspm_vars(hw);

	if (rtlpriv->psc.reg_fwctrl_lps == 1)
		rtlpriv->psc.fwctrl_psmode = FW_PS_MIN_MODE;
	else if (rtlpriv->psc.reg_fwctrl_lps == 2)
		rtlpriv->psc.fwctrl_psmode = FW_PS_MAX_MODE;
	else if (rtlpriv->psc.reg_fwctrl_lps == 3)
		rtlpriv->psc.fwctrl_psmode = FW_PS_DTIM_MODE;

	/*                  */
	rtlpriv->rtlhal.pfirmware = vmalloc(0x6000);
	if (!rtlpriv->rtlhal.pfirmware) {
		RT_TRACE(rtlpriv, COMP_ERR, DBG_EMERG,
			 "Can't alloc buffer for fw.\n");
		return 1;
	}

	if (IS_VENDOR_8723_A_CUT(rtlhal->version))
		rtlpriv->cfg->fw_name = "rtlwifi/rtl8723fw.bin";
	else if (IS_81xxC_VENDOR_UMC_B_CUT(rtlhal->version))
		rtlpriv->cfg->fw_name = "rtlwifi/rtl8723fw_B.bin";

	rtlpriv->max_fw_size = 0x6000;
	pr_info("Using firmware %s\n", rtlpriv->cfg->fw_name);
	err = request_firmware_nowait(THIS_MODULE, 1, rtlpriv->cfg->fw_name,
				      rtlpriv->io.dev, GFP_KERNEL, hw,
				      rtl_fw_cb);
	if (err) {
		RT_TRACE(rtlpriv, COMP_ERR, DBG_EMERG,
			 "Failed to request firmware!\n");
		return 1;
	}
	return 0;
}

void rtl8723ae_deinit_sw_vars(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	if (rtlpriv->rtlhal.pfirmware) {
		vfree(rtlpriv->rtlhal.pfirmware);
		rtlpriv->rtlhal.pfirmware = NULL;
	}
}

static struct rtl_hal_ops rtl8723ae_hal_ops = {
	.init_sw_vars = rtl8723ae_init_sw_vars,
	.deinit_sw_vars = rtl8723ae_deinit_sw_vars,
	.read_eeprom_info = rtl8723ae_read_eeprom_info,
	.interrupt_recognized = rtl8723ae_interrupt_recognized,
	.hw_init = rtl8723ae_hw_init,
	.hw_disable = rtl8723ae_card_disable,
	.hw_suspend = rtl8723ae_suspend,
	.hw_resume = rtl8723ae_resume,
	.enable_interrupt = rtl8723ae_enable_interrupt,
	.disable_interrupt = rtl8723ae_disable_interrupt,
	.set_network_type = rtl8723ae_set_network_type,
	.set_chk_bssid = rtl8723ae_set_check_bssid,
	.set_qos = rtl8723ae_set_qos,
	.set_bcn_reg = rtl8723ae_set_beacon_related_registers,
	.set_bcn_intv = rtl8723ae_set_beacon_interval,
	.update_interrupt_mask = rtl8723ae_update_interrupt_mask,
	.get_hw_reg = rtl8723ae_get_hw_reg,
	.set_hw_reg = rtl8723ae_set_hw_reg,
	.update_rate_tbl = rtl8723ae_update_hal_rate_tbl,
	.fill_tx_desc = rtl8723ae_tx_fill_desc,
	.fill_tx_cmddesc = rtl8723ae_tx_fill_cmddesc,
	.query_rx_desc = rtl8723ae_rx_query_desc,
	.set_channel_access = rtl8723ae_update_channel_access_setting,
	.radio_onoff_checking = rtl8723ae_gpio_radio_on_off_checking,
	.set_bw_mode = rtl8723ae_phy_set_bw_mode,
	.switch_channel = rtl8723ae_phy_sw_chnl,
	.dm_watchdog = rtl8723ae_dm_watchdog,
	.scan_operation_backup = rtl8723ae_phy_scan_operation_backup,
	.set_rf_power_state = rtl8723ae_phy_set_rf_power_state,
	.led_control = rtl8723ae_led_control,
	.set_desc = rtl8723ae_set_desc,
	.get_desc = rtl8723ae_get_desc,
	.tx_polling = rtl8723ae_tx_polling,
	.enable_hw_sec = rtl8723ae_enable_hw_security_config,
	.set_key = rtl8723ae_set_key,
	.init_sw_leds = rtl8723ae_init_sw_leds,
	.allow_all_destaddr = rtl8723ae_allow_all_destaddr,
	.get_bbreg = rtl8723ae_phy_query_bb_reg,
	.set_bbreg = rtl8723ae_phy_set_bb_reg,
	.get_rfreg = rtl8723ae_phy_query_rf_reg,
	.set_rfreg = rtl8723ae_phy_set_rf_reg,
	.c2h_command_handle = rtl_8723e_c2h_command_handle,
	.bt_wifi_media_status_notify = rtl_8723e_bt_wifi_media_status_notify,
	.bt_coex_off_before_lps = rtl8723ae_bt_coex_off_before_lps,
};

static struct rtl_mod_params rtl8723ae_mod_params = {
	.sw_crypto = false,
	.inactiveps = true,
	.swctrl_lps = false,
	.fwctrl_lps = true,
	.debug = DBG_EMERG,
};

static struct rtl_hal_cfg rtl8723ae_hal_cfg = {
	.bar_id = 2,
	.write_readback = true,
	.name = "rtl8723ae_pci",
	.fw_name = "rtlwifi/rtl8723fw.bin",
	.ops = &rtl8723ae_hal_ops,
	.mod_params = &rtl8723ae_mod_params,
	.maps[SYS_ISO_CTRL] = REG_SYS_ISO_CTRL,
	.maps[SYS_FUNC_EN] = REG_SYS_FUNC_EN,
	.maps[SYS_CLK] = REG_SYS_CLKR,
	.maps[MAC_RCR_AM] = AM,
	.maps[MAC_RCR_AB] = AB,
	.maps[MAC_RCR_ACRC32] = ACRC32,
	.maps[MAC_RCR_ACF] = ACF,
	.maps[MAC_RCR_AAP] = AAP,
	.maps[EFUSE_TEST] = REG_EFUSE_TEST,
	.maps[EFUSE_CTRL] = REG_EFUSE_CTRL,
	.maps[EFUSE_CLK] = 0,
	.maps[EFUSE_CLK_CTRL] = REG_EFUSE_CTRL,
	.maps[EFUSE_PWC_EV12V] = PWC_EV12V,
	.maps[EFUSE_FEN_ELDR] = FEN_ELDR,
	.maps[EFUSE_LOADER_CLK_EN] = LOADER_CLK_EN,
	.maps[EFUSE_ANA8M] = ANA8M,
	.maps[EFUSE_HWSET_MAX_SIZE] = HWSET_MAX_SIZE,
	.maps[EFUSE_MAX_SECTION_MAP] = EFUSE_MAX_SECTION,
	.maps[EFUSE_REAL_CONTENT_SIZE] = EFUSE_REAL_CONTENT_LEN,
	.maps[EFUSE_OOB_PROTECT_BYTES_LEN] = EFUSE_OOB_PROTECT_BYTES,

	.maps[RWCAM] = REG_CAMCMD,
	.maps[WCAMI] = REG_CAMWRITE,
	.maps[RCAMO] = REG_CAMREAD,
	.maps[CAMDBG] = REG_CAMDBG,
	.maps[SECR] = REG_SECCFG,
	.maps[SEC_CAM_NONE] = CAM_NONE,
	.maps[SEC_CAM_WEP40] = CAM_WEP40,
	.maps[SEC_CAM_TKIP] = CAM_TKIP,
	.maps[SEC_CAM_AES] = CAM_AES,
	.maps[SEC_CAM_WEP104] = CAM_WEP104,

	.maps[RTL_IMR_BCNDMAINT6] = IMR_BCNDMAINT6,
	.maps[RTL_IMR_BCNDMAINT5] = IMR_BCNDMAINT5,
	.maps[RTL_IMR_BCNDMAINT4] = IMR_BCNDMAINT4,
	.maps[RTL_IMR_BCNDMAINT3] = IMR_BCNDMAINT3,
	.maps[RTL_IMR_BCNDMAINT2] = IMR_BCNDMAINT2,
	.maps[RTL_IMR_BCNDMAINT1] = IMR_BCNDMAINT1,
	.maps[RTL_IMR_BCNDOK8] = IMR_BCNDOK8,
	.maps[RTL_IMR_BCNDOK7] = IMR_BCNDOK7,
	.maps[RTL_IMR_BCNDOK6] = IMR_BCNDOK6,
	.maps[RTL_IMR_BCNDOK5] = IMR_BCNDOK5,
	.maps[RTL_IMR_BCNDOK4] = IMR_BCNDOK4,
	.maps[RTL_IMR_BCNDOK3] = IMR_BCNDOK3,
	.maps[RTL_IMR_BCNDOK2] = IMR_BCNDOK2,
	.maps[RTL_IMR_BCNDOK1] = IMR_BCNDOK1,
	.maps[RTL_IMR_TIMEOUT2] = IMR_TIMEOUT2,
	.maps[RTL_IMR_TIMEOUT1] = IMR_TIMEOUT1,

	.maps[RTL_IMR_TXFOVW] = PHIMR_TXFOVW,
	.maps[RTL_IMR_PSTIMEOUT] = PHIMR_PSTIMEOUT,
	.maps[RTL_IMR_BCNINT] = PHIMR_BCNDMAINT0,
	.maps[RTL_IMR_RXFOVW] = PHIMR_RXFOVW,
	.maps[RTL_IMR_RDU] = PHIMR_RDU,
	.maps[RTL_IMR_ATIMEND] = PHIMR_ATIMEND_E,
	.maps[RTL_IMR_BDOK] = PHIMR_BCNDOK0,
	.maps[RTL_IMR_MGNTDOK] = PHIMR_MGNTDOK,
	.maps[RTL_IMR_TBDER] = PHIMR_TXBCNERR,
	.maps[RTL_IMR_HIGHDOK] = PHIMR_HIGHDOK,
	.maps[RTL_IMR_TBDOK] = PHIMR_TXBCNOK,
	.maps[RTL_IMR_BKDOK] = PHIMR_BKDOK,
	.maps[RTL_IMR_BEDOK] = PHIMR_BEDOK,
	.maps[RTL_IMR_VIDOK] = PHIMR_VIDOK,
	.maps[RTL_IMR_VODOK] = PHIMR_VODOK,
	.maps[RTL_IMR_ROK] = PHIMR_ROK,
	.maps[RTL_IBSS_INT_MASKS] = (PHIMR_BCNDMAINT0 |
				     PHIMR_TXBCNOK | PHIMR_TXBCNERR),
	.maps[RTL_IMR_C2HCMD] = PHIMR_C2HCMD,


	.maps[RTL_RC_CCK_RATE1M] = DESC92_RATE1M,
	.maps[RTL_RC_CCK_RATE2M] = DESC92_RATE2M,
	.maps[RTL_RC_CCK_RATE5_5M] = DESC92_RATE5_5M,
	.maps[RTL_RC_CCK_RATE11M] = DESC92_RATE11M,
	.maps[RTL_RC_OFDM_RATE6M] = DESC92_RATE6M,
	.maps[RTL_RC_OFDM_RATE9M] = DESC92_RATE9M,
	.maps[RTL_RC_OFDM_RATE12M] = DESC92_RATE12M,
	.maps[RTL_RC_OFDM_RATE18M] = DESC92_RATE18M,
	.maps[RTL_RC_OFDM_RATE24M] = DESC92_RATE24M,
	.maps[RTL_RC_OFDM_RATE36M] = DESC92_RATE36M,
	.maps[RTL_RC_OFDM_RATE48M] = DESC92_RATE48M,
	.maps[RTL_RC_OFDM_RATE54M] = DESC92_RATE54M,

	.maps[RTL_RC_HT_RATEMCS7] = DESC92_RATEMCS7,
	.maps[RTL_RC_HT_RATEMCS15] = DESC92_RATEMCS15,
};

static struct pci_device_id rtl8723ae_pci_ids[] = {
	{RTL_PCI_DEVICE(PCI_VENDOR_ID_REALTEK, 0x8723, rtl8723ae_hal_cfg)},
	{},
};

MODULE_DEVICE_TABLE(pci, rtl8723ae_pci_ids);

MODULE_AUTHOR("lizhaoming	<chaoming_li@realsil.com.cn>");
MODULE_AUTHOR("Realtek WlanFAE	<wlanfae@realtek.com>");
MODULE_AUTHOR("Larry Finger	<Larry.Finger@lwfinger.net>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Realtek 8723E 802.11n PCI wireless");
MODULE_FIRMWARE("rtlwifi/rtl8723fw.bin");
MODULE_FIRMWARE("rtlwifi/rtl8723fw_B.bin");

module_param_named(swenc, rtl8723ae_mod_params.sw_crypto, bool, 0444);
module_param_named(debug, rtl8723ae_mod_params.debug, int, 0444);
module_param_named(ips, rtl8723ae_mod_params.inactiveps, bool, 0444);
module_param_named(swlps, rtl8723ae_mod_params.swctrl_lps, bool, 0444);
module_param_named(fwlps, rtl8723ae_mod_params.fwctrl_lps, bool, 0444);
MODULE_PARM_DESC(swenc, "Set to 1 for software crypto (default 0)\n");
MODULE_PARM_DESC(ips, "Set to 0 to not use link power save (default 1)\n");
MODULE_PARM_DESC(swlps, "Set to 1 to use SW control power save (default 0)\n");
MODULE_PARM_DESC(fwlps, "Set to 1 to use FW control power save (default 1)\n");
MODULE_PARM_DESC(debug, "Set debug level (0-5) (default 0)");

static SIMPLE_DEV_PM_OPS(rtlwifi_pm_ops, rtl_pci_suspend, rtl_pci_resume);

static struct pci_driver rtl8723ae_driver = {
	.name = KBUILD_MODNAME,
	.id_table = rtl8723ae_pci_ids,
	.probe = rtl_pci_probe,
	.remove = rtl_pci_disconnect,
	.driver.pm = &rtlwifi_pm_ops,
};

module_pci_driver(rtl8723ae_driver);