/*
 * Copyright (c) 2008-2011 Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/nl80211.h>
#include <linux/pci.h>
#include <linux/pci-aspm.h>
#include <linux/ath9k_platform.h>
#include <linux/module.h>
#include "ath9k.h"

static DEFINE_PCI_DEVICE_TABLE(ath_pci_id_table) = {
	{ PCI_VDEVICE(ATHEROS, 0x0023) }, /*       */
	{ PCI_VDEVICE(ATHEROS, 0x0024) }, /*       */
	{ PCI_VDEVICE(ATHEROS, 0x0027) }, /*       */
	{ PCI_VDEVICE(ATHEROS, 0x0029) }, /*       */
	{ PCI_VDEVICE(ATHEROS, 0x002A) }, /*       */
	{ PCI_VDEVICE(ATHEROS, 0x002B) }, /*       */
	{ PCI_VDEVICE(ATHEROS, 0x002C) }, /*                          */
	{ PCI_VDEVICE(ATHEROS, 0x002D) }, /*       */
	{ PCI_VDEVICE(ATHEROS, 0x002E) }, /*       */
	{ PCI_VDEVICE(ATHEROS, 0x0030) }, /*               */
	{ PCI_VDEVICE(ATHEROS, 0x0032) }, /*               */
	{ PCI_VDEVICE(ATHEROS, 0x0033) }, /*               */
	{ PCI_VDEVICE(ATHEROS, 0x0034) }, /*               */
	{ PCI_VDEVICE(ATHEROS, 0x0037) }, /*                      */
	{ PCI_VDEVICE(ATHEROS, 0x0036) }, /*               */
	{ 0 }
};


/*                                       */
static void ath_pci_read_cachesize(struct ath_common *common, int *csz)
{
	struct ath_softc *sc = (struct ath_softc *) common->priv;
	u8 u8tmp;

	pci_read_config_byte(to_pci_dev(sc->dev), PCI_CACHE_LINE_SIZE, &u8tmp);
	*csz = (int)u8tmp;

	/*
                                                               
                                                          
                                                     
  */

	if (*csz == 0)
		*csz = DEFAULT_CACHELINE >> 2;   /*                      */
}

static bool ath_pci_eeprom_read(struct ath_common *common, u32 off, u16 *data)
{
	struct ath_softc *sc = (struct ath_softc *) common->priv;
	struct ath9k_platform_data *pdata = sc->dev->platform_data;

	if (pdata) {
		if (off >= (ARRAY_SIZE(pdata->eeprom_data))) {
			ath_err(common,
				"%s: eeprom read failed, offset %08x is out of range\n",
				__func__, off);
		}

		*data = pdata->eeprom_data[off];
	} else {
		struct ath_hw *ah = (struct ath_hw *) common->ah;

		common->ops->read(ah, AR5416_EEPROM_OFFSET +
				      (off << AR5416_EEPROM_S));

		if (!ath9k_hw_wait(ah,
				   AR_EEPROM_STATUS_DATA,
				   AR_EEPROM_STATUS_DATA_BUSY |
				   AR_EEPROM_STATUS_DATA_PROT_ACCESS, 0,
				   AH_WAIT_TIMEOUT)) {
			return false;
		}

		*data = MS(common->ops->read(ah, AR_EEPROM_STATUS_DATA),
			   AR_EEPROM_STATUS_DATA_VAL);
	}

	return true;
}

/*                                                         */
static void ath_pci_aspm_init(struct ath_common *common)
{
	struct ath_softc *sc = (struct ath_softc *) common->priv;
	struct ath_hw *ah = sc->sc_ah;
	struct pci_dev *pdev = to_pci_dev(sc->dev);
	struct pci_dev *parent;
	u16 aspm;

	if (!ah->is_pciexpress)
		return;

	parent = pdev->bus->self;
	if (!parent)
		return;

	if ((ath9k_hw_get_btcoex_scheme(ah) != ATH_BTCOEX_CFG_NONE) &&
	    (AR_SREV_9285(ah))) {
		/*                                                */
		pcie_capability_clear_word(pdev, PCI_EXP_LNKCTL,
			PCI_EXP_LNKCTL_ASPM_L0S | PCI_EXP_LNKCTL_ASPM_L1);

		/*
                                                        
                                 
   */
		pcie_capability_clear_word(parent, PCI_EXP_LNKCTL,
			PCI_EXP_LNKCTL_ASPM_L0S | PCI_EXP_LNKCTL_ASPM_L1);

		ath_info(common, "Disabling ASPM since BTCOEX is enabled\n");
		return;
	}

	pcie_capability_read_word(parent, PCI_EXP_LNKCTL, &aspm);
	if (aspm & (PCI_EXP_LNKCTL_ASPM_L0S | PCI_EXP_LNKCTL_ASPM_L1)) {
		ah->aspm_enabled = true;
		/*                                          */
		ath9k_hw_configpcipowersave(ah, false);
		ath_info(common, "ASPM enabled: 0x%x\n", aspm);
	}
}

static const struct ath_bus_ops ath_pci_bus_ops = {
	.ath_bus_type = ATH_PCI,
	.read_cachesize = ath_pci_read_cachesize,
	.eeprom_read = ath_pci_eeprom_read,
	.aspm_init = ath_pci_aspm_init,
};

static int ath_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	struct ath_softc *sc;
	struct ieee80211_hw *hw;
	u8 csz;
	u32 val;
	int ret = 0;
	char hw_name[64];

	if (pcim_enable_device(pdev))
		return -EIO;

	ret =  pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
	if (ret) {
		pr_err("32-bit DMA not available\n");
		return ret;
	}

	ret = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(32));
	if (ret) {
		pr_err("32-bit DMA consistent DMA enable failed\n");
		return ret;
	}

	/*
                                                     
                                                     
  */
	pci_read_config_byte(pdev, PCI_CACHE_LINE_SIZE, &csz);
	if (csz == 0) {
		/*
                                                       
                                                       
                                                   
                                                       
                   
   */
		csz = L1_CACHE_BYTES / sizeof(u32);
		pci_write_config_byte(pdev, PCI_CACHE_LINE_SIZE, csz);
	}
	/*
                                                             
                                                              
                               
  */
	pci_write_config_byte(pdev, PCI_LATENCY_TIMER, 0xa8);

	pci_set_master(pdev);

	/*
                                                     
                                                      
  */
	pci_read_config_dword(pdev, 0x40, &val);
	if ((val & 0x0000ff00) != 0)
		pci_write_config_dword(pdev, 0x40, val & 0xffff00ff);

	ret = pcim_iomap_regions(pdev, BIT(0), "ath9k");
	if (ret) {
		dev_err(&pdev->dev, "PCI memory region reserve error\n");
		return -ENODEV;
	}

	hw = ieee80211_alloc_hw(sizeof(struct ath_softc), &ath9k_ops);
	if (!hw) {
		dev_err(&pdev->dev, "No memory for ieee80211_hw\n");
		return -ENOMEM;
	}

	SET_IEEE80211_DEV(hw, &pdev->dev);
	pci_set_drvdata(pdev, hw);

	sc = hw->priv;
	sc->hw = hw;
	sc->dev = &pdev->dev;
	sc->mem = pcim_iomap_table(pdev)[0];

	/*                                  */
	set_bit(SC_OP_INVALID, &sc->sc_flags);

	ret = request_irq(pdev->irq, ath_isr, IRQF_SHARED, "ath9k", sc);
	if (ret) {
		dev_err(&pdev->dev, "request_irq failed\n");
		goto err_irq;
	}

	sc->irq = pdev->irq;

	ret = ath9k_init_device(id->device, sc, &ath_pci_bus_ops);
	if (ret) {
		dev_err(&pdev->dev, "Failed to initialize device\n");
		goto err_init;
	}

	ath9k_hw_name(sc->sc_ah, hw_name, sizeof(hw_name));
	wiphy_info(hw->wiphy, "%s mem=0x%lx, irq=%d\n",
		   hw_name, (unsigned long)sc->mem, pdev->irq);

	return 0;

err_init:
	free_irq(sc->irq, sc);
err_irq:
	ieee80211_free_hw(hw);
	return ret;
}

static void ath_pci_remove(struct pci_dev *pdev)
{
	struct ieee80211_hw *hw = pci_get_drvdata(pdev);
	struct ath_softc *sc = hw->priv;

	if (!is_ath9k_unloaded)
		sc->sc_ah->ah_flags |= AH_UNPLUGGED;
	ath9k_deinit_device(sc);
	free_irq(sc->irq, sc);
	ieee80211_free_hw(sc->hw);
}

#ifdef CONFIG_PM_SLEEP

static int ath_pci_suspend(struct device *device)
{
	struct pci_dev *pdev = to_pci_dev(device);
	struct ieee80211_hw *hw = pci_get_drvdata(pdev);
	struct ath_softc *sc = hw->priv;

	if (sc->wow_enabled)
		return 0;

	/*                                                  
                                                 
                            
  */
	ath9k_stop_btcoex(sc);
	ath9k_hw_disable(sc->sc_ah);
	ath9k_hw_setpower(sc->sc_ah, ATH9K_PM_FULL_SLEEP);

	return 0;
}

static int ath_pci_resume(struct device *device)
{
	struct pci_dev *pdev = to_pci_dev(device);
	struct ieee80211_hw *hw = pci_get_drvdata(pdev);
	struct ath_softc *sc = hw->priv;
	struct ath_hw *ah = sc->sc_ah;
	struct ath_common *common = ath9k_hw_common(ah);
	u32 val;

	/*
                                                                    
                                                        
                                                     
  */
	pci_read_config_dword(pdev, 0x40, &val);
	if ((val & 0x0000ff00) != 0)
		pci_write_config_dword(pdev, 0x40, val & 0xffff00ff);

	ath_pci_aspm_init(common);
	ah->reset_power_on = false;

	return 0;
}

static SIMPLE_DEV_PM_OPS(ath9k_pm_ops, ath_pci_suspend, ath_pci_resume);

#define ATH9K_PM_OPS	(&ath9k_pm_ops)

#else /*                  */

#define ATH9K_PM_OPS	NULL

#endif /*                  */


MODULE_DEVICE_TABLE(pci, ath_pci_id_table);

static struct pci_driver ath_pci_driver = {
	.name       = "ath9k",
	.id_table   = ath_pci_id_table,
	.probe      = ath_pci_probe,
	.remove     = ath_pci_remove,
	.driver.pm  = ATH9K_PM_OPS,
};

int ath_pci_init(void)
{
	return pci_register_driver(&ath_pci_driver);
}

void ath_pci_exit(void)
{
	pci_unregister_driver(&ath_pci_driver);
}
