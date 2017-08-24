/*
 * twl4030_usb - TWL4030 USB transceiver, talking to OMAP OTG controller
 *
 * Copyright (C) 2004-2007 Texas Instruments
 * Copyright (C) 2008 Nokia Corporation
 * Contact: Felipe Balbi <felipe.balbi@nokia.com>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Current status:
 *	- HS USB ULPI mode works.
 *	- 3-pin mode support may be added in future.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/usb/otg.h>
#include <linux/usb/musb-omap.h>
#include <linux/usb/ulpi.h>
#include <linux/i2c/twl.h>
#include <linux/regulator/consumer.h>
#include <linux/err.h>
#include <linux/slab.h>

/*                  */

#define MCPC_CTRL			0x30
#define MCPC_CTRL_RTSOL			(1 << 7)
#define MCPC_CTRL_EXTSWR		(1 << 6)
#define MCPC_CTRL_EXTSWC		(1 << 5)
#define MCPC_CTRL_VOICESW		(1 << 4)
#define MCPC_CTRL_OUT64K		(1 << 3)
#define MCPC_CTRL_RTSCTSSW		(1 << 2)
#define MCPC_CTRL_HS_UART		(1 << 0)

#define MCPC_IO_CTRL			0x33
#define MCPC_IO_CTRL_MICBIASEN		(1 << 5)
#define MCPC_IO_CTRL_CTS_NPU		(1 << 4)
#define MCPC_IO_CTRL_RXD_PU		(1 << 3)
#define MCPC_IO_CTRL_TXDTYP		(1 << 2)
#define MCPC_IO_CTRL_CTSTYP		(1 << 1)
#define MCPC_IO_CTRL_RTSTYP		(1 << 0)

#define MCPC_CTRL2			0x36
#define MCPC_CTRL2_MCPC_CK_EN		(1 << 0)

#define OTHER_FUNC_CTRL			0x80
#define OTHER_FUNC_CTRL_BDIS_ACON_EN	(1 << 4)
#define OTHER_FUNC_CTRL_FIVEWIRE_MODE	(1 << 2)

#define OTHER_IFC_CTRL			0x83
#define OTHER_IFC_CTRL_OE_INT_EN	(1 << 6)
#define OTHER_IFC_CTRL_CEA2011_MODE	(1 << 5)
#define OTHER_IFC_CTRL_FSLSSERIALMODE_4PIN	(1 << 4)
#define OTHER_IFC_CTRL_HIZ_ULPI_60MHZ_OUT	(1 << 3)
#define OTHER_IFC_CTRL_HIZ_ULPI		(1 << 2)
#define OTHER_IFC_CTRL_ALT_INT_REROUTE	(1 << 0)

#define OTHER_INT_EN_RISE		0x86
#define OTHER_INT_EN_FALL		0x89
#define OTHER_INT_STS			0x8C
#define OTHER_INT_LATCH			0x8D
#define OTHER_INT_VB_SESS_VLD		(1 << 7)
#define OTHER_INT_DM_HI			(1 << 6) /*                           */
#define OTHER_INT_DP_HI			(1 << 5) /*                           */
#define OTHER_INT_BDIS_ACON		(1 << 3) /*                           */
#define OTHER_INT_MANU			(1 << 1)
#define OTHER_INT_ABNORMAL_STRESS	(1 << 0)

#define ID_STATUS			0x96
#define ID_RES_FLOAT			(1 << 4)
#define ID_RES_440K			(1 << 3)
#define ID_RES_200K			(1 << 2)
#define ID_RES_102K			(1 << 1)
#define ID_RES_GND			(1 << 0)

#define POWER_CTRL			0xAC
#define POWER_CTRL_OTG_ENAB		(1 << 5)

#define OTHER_IFC_CTRL2			0xAF
#define OTHER_IFC_CTRL2_ULPI_STP_LOW	(1 << 4)
#define OTHER_IFC_CTRL2_ULPI_TXEN_POL	(1 << 3)
#define OTHER_IFC_CTRL2_ULPI_4PIN_2430	(1 << 2)
#define OTHER_IFC_CTRL2_USB_INT_OUTSEL_MASK	(3 << 0) /*              */
#define OTHER_IFC_CTRL2_USB_INT_OUTSEL_INT1N	(0 << 0)
#define OTHER_IFC_CTRL2_USB_INT_OUTSEL_INT2N	(1 << 0)

#define REG_CTRL_EN			0xB2
#define REG_CTRL_ERROR			0xB5
#define ULPI_I2C_CONFLICT_INTEN		(1 << 0)

#define OTHER_FUNC_CTRL2		0xB8
#define OTHER_FUNC_CTRL2_VBAT_TIMER_EN	(1 << 0)

/*                                                                  */
#define VBUS_DEBOUNCE			0xC0
#define ID_DEBOUNCE			0xC1
#define VBAT_TIMER			0xD3
#define PHY_PWR_CTRL			0xFD
#define PHY_PWR_PHYPWD			(1 << 0)
#define PHY_CLK_CTRL			0xFE
#define PHY_CLK_CTRL_CLOCKGATING_EN	(1 << 2)
#define PHY_CLK_CTRL_CLK32K_EN		(1 << 1)
#define REQ_PHY_DPLL_CLK		(1 << 0)
#define PHY_CLK_CTRL_STS		0xFF
#define PHY_DPLL_CLK			(1 << 0)

/*                                */
#define STS_HW_CONDITIONS		0x0F

/*                                  */
#define VUSB_DEDICATED1			0x7D
#define VUSB_DEDICATED2			0x7E
#define VUSB1V5_DEV_GRP			0x71
#define VUSB1V5_TYPE			0x72
#define VUSB1V5_REMAP			0x73
#define VUSB1V8_DEV_GRP			0x74
#define VUSB1V8_TYPE			0x75
#define VUSB1V8_REMAP			0x76
#define VUSB3V1_DEV_GRP			0x77
#define VUSB3V1_TYPE			0x78
#define VUSB3V1_REMAP			0x79

/*                                */
#define PMBR1				0x0D
#define GPIO_USB_4PIN_ULPI_2430C	(3 << 0)

struct twl4030_usb {
	struct usb_phy		phy;
	struct device		*dev;

	/*                                         */
	struct regulator	*usb1v5;
	struct regulator	*usb1v8;
	struct regulator	*usb3v1;

	/*                                       */
	spinlock_t		lock;

	/*                   */
	enum twl4030_usb_mode	usb_mode;

	int			irq;
	enum omap_musb_vbus_id_status linkstat;
	bool			vbus_supplied;
	u8			asleep;
	bool			irq_enabled;

	struct delayed_work	id_workaround_work;
};

/*                                        */
#define phy_to_twl(x)		container_of((x), struct twl4030_usb, phy)

/*                                                                         */

static int twl4030_i2c_write_u8_verify(struct twl4030_usb *twl,
		u8 module, u8 data, u8 address)
{
	u8 check;

	if ((twl_i2c_write_u8(module, data, address) >= 0) &&
	    (twl_i2c_read_u8(module, &check, address) >= 0) &&
						(check == data))
		return 0;
	dev_dbg(twl->dev, "Write%d[%d,0x%x] wrote %02x but read %02x\n",
			1, module, address, check, data);

	/*                        */
	if ((twl_i2c_write_u8(module, data, address) >= 0) &&
	    (twl_i2c_read_u8(module, &check, address) >= 0) &&
						(check == data))
		return 0;
	dev_dbg(twl->dev, "Write%d[%d,0x%x] wrote %02x but read %02x\n",
			2, module, address, check, data);

	/*                            */
	return -EBUSY;
}

#define twl4030_usb_write_verify(twl, address, data)	\
	twl4030_i2c_write_u8_verify(twl, TWL_MODULE_USB, (data), (address))

static inline int twl4030_usb_write(struct twl4030_usb *twl,
		u8 address, u8 data)
{
	int ret = 0;

	ret = twl_i2c_write_u8(TWL_MODULE_USB, data, address);
	if (ret < 0)
		dev_dbg(twl->dev,
			"TWL4030:USB:Write[0x%x] Error %d\n", address, ret);
	return ret;
}

static inline int twl4030_readb(struct twl4030_usb *twl, u8 module, u8 address)
{
	u8 data;
	int ret = 0;

	ret = twl_i2c_read_u8(module, &data, address);
	if (ret >= 0)
		ret = data;
	else
		dev_dbg(twl->dev,
			"TWL4030:readb[0x%x,0x%x] Error %d\n",
					module, address, ret);

	return ret;
}

static inline int twl4030_usb_read(struct twl4030_usb *twl, u8 address)
{
	return twl4030_readb(twl, TWL_MODULE_USB, address);
}

/*                                                                         */

static inline int
twl4030_usb_set_bits(struct twl4030_usb *twl, u8 reg, u8 bits)
{
	return twl4030_usb_write(twl, ULPI_SET(reg), bits);
}

static inline int
twl4030_usb_clear_bits(struct twl4030_usb *twl, u8 reg, u8 bits)
{
	return twl4030_usb_write(twl, ULPI_CLR(reg), bits);
}

/*                                                                         */

static bool twl4030_is_driving_vbus(struct twl4030_usb *twl)
{
	int ret;

	ret = twl4030_usb_read(twl, PHY_CLK_CTRL_STS);
	if (ret < 0 || !(ret & PHY_DPLL_CLK))
		/*
                                                  
                                                       
   */
		return false;

	ret = twl4030_usb_read(twl, ULPI_OTG_CTRL);
	if (ret < 0)
		return false;

	return (ret & (ULPI_OTG_DRVVBUS | ULPI_OTG_CHRGVBUS)) ? true : false;
}

static enum omap_musb_vbus_id_status
	twl4030_usb_linkstat(struct twl4030_usb *twl)
{
	int	status;
	enum omap_musb_vbus_id_status linkstat = OMAP_MUSB_UNKNOWN;

	twl->vbus_supplied = false;

	/*
                                                      
                                                    
                                                      
                                                     
                                                     
                                                     
                                                      
                                                      
  */
	status = twl4030_readb(twl, TWL_MODULE_PM_MASTER, STS_HW_CONDITIONS);
	if (status < 0)
		dev_err(twl->dev, "USB link status err %d\n", status);
	else if (status & (BIT(7) | BIT(2))) {
		if (status & BIT(7)) {
			if (twl4030_is_driving_vbus(twl))
				status &= ~BIT(7);
			else
				twl->vbus_supplied = true;
		}

		if (status & BIT(2))
			linkstat = OMAP_MUSB_ID_GROUND;
		else if (status & BIT(7))
			linkstat = OMAP_MUSB_VBUS_VALID;
		else
			linkstat = OMAP_MUSB_VBUS_OFF;
	} else {
		if (twl->linkstat != OMAP_MUSB_UNKNOWN)
			linkstat = OMAP_MUSB_VBUS_OFF;
	}

	dev_dbg(twl->dev, "HW_CONDITIONS 0x%02x/%d; link %d\n",
			status, status, linkstat);

	/*                                                     
                                               
  */

	return linkstat;
}

static void twl4030_usb_set_mode(struct twl4030_usb *twl, int mode)
{
	twl->usb_mode = mode;

	switch (mode) {
	case T2_USB_MODE_ULPI:
		twl4030_usb_clear_bits(twl, ULPI_IFC_CTRL,
					ULPI_IFC_CTRL_CARKITMODE);
		twl4030_usb_set_bits(twl, POWER_CTRL, POWER_CTRL_OTG_ENAB);
		twl4030_usb_clear_bits(twl, ULPI_FUNC_CTRL,
					ULPI_FUNC_CTRL_XCVRSEL_MASK |
					ULPI_FUNC_CTRL_OPMODE_MASK);
		break;
	case -1:
		/*                          */
		break;
	default:
		dev_err(twl->dev, "unsupported T2 transceiver mode %d\n",
				mode);
		break;
	};
}

static void twl4030_i2c_access(struct twl4030_usb *twl, int on)
{
	unsigned long timeout;
	int val = twl4030_usb_read(twl, PHY_CLK_CTRL);

	if (val >= 0) {
		if (on) {
			/*                                              */
			val |= REQ_PHY_DPLL_CLK;
			WARN_ON(twl4030_usb_write_verify(twl, PHY_CLK_CTRL,
						(u8)val) < 0);

			timeout = jiffies + HZ;
			while (!(twl4030_usb_read(twl, PHY_CLK_CTRL_STS) &
							PHY_DPLL_CLK)
				&& time_before(jiffies, timeout))
					udelay(10);
			if (!(twl4030_usb_read(twl, PHY_CLK_CTRL_STS) &
							PHY_DPLL_CLK))
				dev_err(twl->dev, "Timeout setting T2 HSUSB "
						"PHY DPLL clock\n");
		} else {
			/*                                 */
			val &= ~REQ_PHY_DPLL_CLK;
			WARN_ON(twl4030_usb_write_verify(twl, PHY_CLK_CTRL,
						(u8)val) < 0);
		}
	}
}

static void __twl4030_phy_power(struct twl4030_usb *twl, int on)
{
	u8 pwr = twl4030_usb_read(twl, PHY_PWR_CTRL);

	if (on)
		pwr &= ~PHY_PWR_PHYPWD;
	else
		pwr |= PHY_PWR_PHYPWD;

	WARN_ON(twl4030_usb_write_verify(twl, PHY_PWR_CTRL, pwr) < 0);
}

static void twl4030_phy_power(struct twl4030_usb *twl, int on)
{
	int ret;

	if (on) {
		ret = regulator_enable(twl->usb3v1);
		if (ret)
			dev_err(twl->dev, "Failed to enable usb3v1\n");

		ret = regulator_enable(twl->usb1v8);
		if (ret)
			dev_err(twl->dev, "Failed to enable usb1v8\n");

		/*
                                                               
                                                                
                                                                 
                                                                
                                                                 
   */
		twl_i2c_write_u8(TWL_MODULE_PM_RECEIVER, 0, VUSB_DEDICATED2);

		ret = regulator_enable(twl->usb1v5);
		if (ret)
			dev_err(twl->dev, "Failed to enable usb1v5\n");

		__twl4030_phy_power(twl, 1);
		twl4030_usb_write(twl, PHY_CLK_CTRL,
				  twl4030_usb_read(twl, PHY_CLK_CTRL) |
					(PHY_CLK_CTRL_CLOCKGATING_EN |
						PHY_CLK_CTRL_CLK32K_EN));
	} else {
		__twl4030_phy_power(twl, 0);
		regulator_disable(twl->usb1v5);
		regulator_disable(twl->usb1v8);
		regulator_disable(twl->usb3v1);
	}
}

static void twl4030_phy_suspend(struct twl4030_usb *twl, int controller_off)
{
	if (twl->asleep)
		return;

	twl4030_phy_power(twl, 0);
	twl->asleep = 1;
	dev_dbg(twl->dev, "%s\n", __func__);
}

static void __twl4030_phy_resume(struct twl4030_usb *twl)
{
	twl4030_phy_power(twl, 1);
	twl4030_i2c_access(twl, 1);
	twl4030_usb_set_mode(twl, twl->usb_mode);
	if (twl->usb_mode == T2_USB_MODE_ULPI)
		twl4030_i2c_access(twl, 0);
}

static void twl4030_phy_resume(struct twl4030_usb *twl)
{
	if (!twl->asleep)
		return;
	__twl4030_phy_resume(twl);
	twl->asleep = 0;
	dev_dbg(twl->dev, "%s\n", __func__);

	/*
                                                        
                                                     
                                                            
  */
	if (twl->linkstat == OMAP_MUSB_ID_GROUND) {
		cancel_delayed_work(&twl->id_workaround_work);
		schedule_delayed_work(&twl->id_workaround_work, HZ);
	}
}

static int twl4030_usb_ldo_init(struct twl4030_usb *twl)
{
	/*                                                 */
	twl_i2c_write_u8(TWL_MODULE_PM_MASTER, TWL4030_PM_MASTER_KEY_CFG1,
			 TWL4030_PM_MASTER_PROTECT_KEY);

	twl_i2c_write_u8(TWL_MODULE_PM_MASTER, TWL4030_PM_MASTER_KEY_CFG2,
			 TWL4030_PM_MASTER_PROTECT_KEY);

	/*                                                              */
	/*                                                             */

	/*                                             */
	twl_i2c_write_u8(TWL_MODULE_PM_RECEIVER, 0x14, VUSB_DEDICATED1);

	/*                           */
	twl_i2c_write_u8(TWL_MODULE_PM_RECEIVER, 0, VUSB3V1_DEV_GRP);

	twl->usb3v1 = devm_regulator_get(twl->dev, "usb3v1");
	if (IS_ERR(twl->usb3v1))
		return -ENODEV;

	twl_i2c_write_u8(TWL_MODULE_PM_RECEIVER, 0, VUSB3V1_TYPE);

	/*                           */
	twl_i2c_write_u8(TWL_MODULE_PM_RECEIVER, 0, VUSB1V5_DEV_GRP);

	twl->usb1v5 = devm_regulator_get(twl->dev, "usb1v5");
	if (IS_ERR(twl->usb1v5))
		return -ENODEV;

	twl_i2c_write_u8(TWL_MODULE_PM_RECEIVER, 0, VUSB1V5_TYPE);

	/*                           */
	twl_i2c_write_u8(TWL_MODULE_PM_RECEIVER, 0, VUSB1V8_DEV_GRP);

	twl->usb1v8 = devm_regulator_get(twl->dev, "usb1v8");
	if (IS_ERR(twl->usb1v8))
		return -ENODEV;

	twl_i2c_write_u8(TWL_MODULE_PM_RECEIVER, 0, VUSB1V8_TYPE);

	/*                                                 */
	twl_i2c_write_u8(TWL_MODULE_PM_MASTER, 0,
			 TWL4030_PM_MASTER_PROTECT_KEY);

	return 0;
}

static ssize_t twl4030_usb_vbus_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct twl4030_usb *twl = dev_get_drvdata(dev);
	unsigned long flags;
	int ret = -EINVAL;

	spin_lock_irqsave(&twl->lock, flags);
	ret = sprintf(buf, "%s\n",
			twl->vbus_supplied ? "on" : "off");
	spin_unlock_irqrestore(&twl->lock, flags);

	return ret;
}
static DEVICE_ATTR(vbus, 0444, twl4030_usb_vbus_show, NULL);

static irqreturn_t twl4030_usb_irq(int irq, void *_twl)
{
	struct twl4030_usb *twl = _twl;
	enum omap_musb_vbus_id_status status;
	bool status_changed = false;

	status = twl4030_usb_linkstat(twl);

	spin_lock_irq(&twl->lock);
	if (status >= 0 && status != twl->linkstat) {
		twl->linkstat = status;
		status_changed = true;
	}
	spin_unlock_irq(&twl->lock);

	if (status_changed) {
		/*                                                     
                                                          
                                                           
                                                          
                 
    
                                                          
                                                        
                                                        
                                        
   */
		omap_musb_mailbox(status);
	}
	sysfs_notify(&twl->dev->kobj, NULL, "vbus");

	return IRQ_HANDLED;
}

static void twl4030_id_workaround_work(struct work_struct *work)
{
	struct twl4030_usb *twl = container_of(work, struct twl4030_usb,
		id_workaround_work.work);
	enum omap_musb_vbus_id_status status;
	bool status_changed = false;

	status = twl4030_usb_linkstat(twl);

	spin_lock_irq(&twl->lock);
	if (status >= 0 && status != twl->linkstat) {
		twl->linkstat = status;
		status_changed = true;
	}
	spin_unlock_irq(&twl->lock);

	if (status_changed) {
		dev_dbg(twl->dev, "handle missing status change to %d\n",
				status);
		omap_musb_mailbox(status);
	}

	/*                                                    */
	if (status == OMAP_MUSB_ID_GROUND && !twl->asleep) {
		cancel_delayed_work(&twl->id_workaround_work);
		schedule_delayed_work(&twl->id_workaround_work, HZ);
	}
}

static int twl4030_usb_phy_init(struct usb_phy *phy)
{
	struct twl4030_usb *twl = phy_to_twl(phy);
	enum omap_musb_vbus_id_status status;

	/*
                                                                
                                                                 
  */
	__twl4030_phy_power(twl, 0);
	twl->asleep = 1;

	status = twl4030_usb_linkstat(twl);
	twl->linkstat = status;

	if (status == OMAP_MUSB_ID_GROUND || status == OMAP_MUSB_VBUS_VALID)
		omap_musb_mailbox(twl->linkstat);

	sysfs_notify(&twl->dev->kobj, NULL, "vbus");
	return 0;
}

static int twl4030_set_suspend(struct usb_phy *x, int suspend)
{
	struct twl4030_usb *twl = phy_to_twl(x);

	if (suspend)
		twl4030_phy_suspend(twl, 1);
	else
		twl4030_phy_resume(twl);

	return 0;
}

static int twl4030_set_peripheral(struct usb_otg *otg,
					struct usb_gadget *gadget)
{
	if (!otg)
		return -ENODEV;

	otg->gadget = gadget;
	if (!gadget)
		otg->phy->state = OTG_STATE_UNDEFINED;

	return 0;
}

static int twl4030_set_host(struct usb_otg *otg, struct usb_bus *host)
{
	if (!otg)
		return -ENODEV;

	otg->host = host;
	if (!host)
		otg->phy->state = OTG_STATE_UNDEFINED;

	return 0;
}

static int twl4030_usb_probe(struct platform_device *pdev)
{
	struct twl4030_usb_data *pdata = pdev->dev.platform_data;
	struct twl4030_usb	*twl;
	int			status, err;
	struct usb_otg		*otg;
	struct device_node	*np = pdev->dev.of_node;

	twl = devm_kzalloc(&pdev->dev, sizeof *twl, GFP_KERNEL);
	if (!twl)
		return -ENOMEM;

	if (np)
		of_property_read_u32(np, "usb_mode",
				(enum twl4030_usb_mode *)&twl->usb_mode);
	else if (pdata)
		twl->usb_mode = pdata->usb_mode;
	else {
		dev_err(&pdev->dev, "twl4030 initialized without pdata\n");
		return -EINVAL;
	}

	otg = devm_kzalloc(&pdev->dev, sizeof *otg, GFP_KERNEL);
	if (!otg)
		return -ENOMEM;

	twl->dev		= &pdev->dev;
	twl->irq		= platform_get_irq(pdev, 0);
	twl->vbus_supplied	= false;
	twl->asleep		= 1;
	twl->linkstat		= OMAP_MUSB_UNKNOWN;

	twl->phy.dev		= twl->dev;
	twl->phy.label		= "twl4030";
	twl->phy.otg		= otg;
	twl->phy.type		= USB_PHY_TYPE_USB2;
	twl->phy.set_suspend	= twl4030_set_suspend;
	twl->phy.init		= twl4030_usb_phy_init;

	otg->phy		= &twl->phy;
	otg->set_host		= twl4030_set_host;
	otg->set_peripheral	= twl4030_set_peripheral;

	/*                             */
	spin_lock_init(&twl->lock);

	INIT_DELAYED_WORK(&twl->id_workaround_work, twl4030_id_workaround_work);

	err = twl4030_usb_ldo_init(twl);
	if (err) {
		dev_err(&pdev->dev, "ldo init failed\n");
		return err;
	}
	usb_add_phy_dev(&twl->phy);

	platform_set_drvdata(pdev, twl);
	if (device_create_file(&pdev->dev, &dev_attr_vbus))
		dev_warn(&pdev->dev, "could not create sysfs file\n");

	/*                                                        
                                                              
   
                                                           
                                                              
                                                             
                                                   
  */
	twl->irq_enabled = true;
	status = devm_request_threaded_irq(twl->dev, twl->irq, NULL,
			twl4030_usb_irq, IRQF_TRIGGER_FALLING |
			IRQF_TRIGGER_RISING | IRQF_ONESHOT, "twl4030_usb", twl);
	if (status < 0) {
		dev_dbg(&pdev->dev, "can't get IRQ %d, err %d\n",
			twl->irq, status);
		return status;
	}

	dev_info(&pdev->dev, "Initialized TWL4030 USB module\n");
	return 0;
}

static int twl4030_usb_remove(struct platform_device *pdev)
{
	struct twl4030_usb *twl = platform_get_drvdata(pdev);
	int val;

	cancel_delayed_work(&twl->id_workaround_work);
	device_remove_file(twl->dev, &dev_attr_vbus);

	/*                                           */
	twl4030_usb_set_mode(twl, -1);

	/*                           
                                            
                 
  */
	val = twl4030_usb_read(twl, PHY_CLK_CTRL);
	if (val >= 0) {
		val |= PHY_CLK_CTRL_CLOCKGATING_EN;
		val &= ~(PHY_CLK_CTRL_CLK32K_EN | REQ_PHY_DPLL_CLK);
		twl4030_usb_write(twl, PHY_CLK_CTRL, (u8)val);
	}

	/*                            */
	twl4030_usb_clear_bits(twl, POWER_CTRL, POWER_CTRL_OTG_ENAB);

	if (!twl->asleep)
		twl4030_phy_power(twl, 0);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id twl4030_usb_id_table[] = {
	{ .compatible = "ti,twl4030-usb" },
	{}
};
MODULE_DEVICE_TABLE(of, twl4030_usb_id_table);
#endif

static struct platform_driver twl4030_usb_driver = {
	.probe		= twl4030_usb_probe,
	.remove		= twl4030_usb_remove,
	.driver		= {
		.name	= "twl4030_usb",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(twl4030_usb_id_table),
	},
};

static int __init twl4030_usb_init(void)
{
	return platform_driver_register(&twl4030_usb_driver);
}
subsys_initcall(twl4030_usb_init);

static void __exit twl4030_usb_exit(void)
{
	platform_driver_unregister(&twl4030_usb_driver);
}
module_exit(twl4030_usb_exit);

MODULE_ALIAS("platform:twl4030_usb");
MODULE_AUTHOR("Texas Instruments, Inc, Nokia Corporation");
MODULE_DESCRIPTION("TWL4030 USB transceiver driver");
MODULE_LICENSE("GPL");
