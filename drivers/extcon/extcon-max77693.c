/*
 * extcon-max77693.c - MAX77693 extcon driver to support MAX77693 MUIC
 *
 * Copyright (C) 2012 Samsung Electrnoics
 * Chanwoo Choi <cw00.choi@samsung.com>
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
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/mfd/max77693.h>
#include <linux/mfd/max77693-private.h>
#include <linux/extcon.h>
#include <linux/regmap.h>
#include <linux/irqdomain.h>

#define	DEV_NAME			"max77693-muic"
#define	DELAY_MS_DEFAULT		20000		/*                   */

/*
                                                              
                                                                              
                                                                            
                           
 */
static struct max77693_reg_data default_init_data[] = {
	{
		/*                        */
		.addr = MAX77693_MUIC_REG_STATUS2,
		.data = STATUS2_CHGDETRUN_MASK,
	}, {
		/*                                     */
		.addr = MAX77693_MUIC_REG_INTMASK1,
		.data = INTMASK1_ADC1K_MASK
			| INTMASK1_ADC_MASK,
	}, {
		/*                              */
		.addr = MAX77693_MUIC_REG_INTMASK2,
		.data = INTMASK2_CHGTYP_MASK,
	}, {
		/*                                   */
		.addr = MAX77693_MUIC_REG_INTMASK3,
		.data = 0x0,
	}, {
		/*           */
		.addr = MAX77693_MUIC_REG_CDETCTRL2,
		.data = CDETCTRL2_VIDRMEN_MASK
			| CDETCTRL2_DXOVPEN_MASK,
	},
};

enum max77693_muic_adc_debounce_time {
	ADC_DEBOUNCE_TIME_5MS = 0,
	ADC_DEBOUNCE_TIME_10MS,
	ADC_DEBOUNCE_TIME_25MS,
	ADC_DEBOUNCE_TIME_38_62MS,
};

struct max77693_muic_info {
	struct device *dev;
	struct max77693_dev *max77693;
	struct extcon_dev *edev;
	int prev_cable_type;
	int prev_cable_type_gnd;
	int prev_chg_type;
	int prev_button_type;
	u8 status[2];

	int irq;
	struct work_struct irq_work;
	struct mutex mutex;

	/*
                                                        
                                                           
                                                                 
                                                    
  */
	struct delayed_work wq_detcable;

	/*                       */
	struct input_dev *dock;

	/*
                                                              
                                                 
  */
	int path_usb;
	int path_uart;
};

enum max77693_muic_cable_group {
	MAX77693_CABLE_GROUP_ADC = 0,
	MAX77693_CABLE_GROUP_ADC_GND,
	MAX77693_CABLE_GROUP_CHG,
	MAX77693_CABLE_GROUP_VBVOLT,
};

enum max77693_muic_charger_type {
	MAX77693_CHARGER_TYPE_NONE = 0,
	MAX77693_CHARGER_TYPE_USB,
	MAX77693_CHARGER_TYPE_DOWNSTREAM_PORT,
	MAX77693_CHARGER_TYPE_DEDICATED_CHG,
	MAX77693_CHARGER_TYPE_APPLE_500MA,
	MAX77693_CHARGER_TYPE_APPLE_1A_2A,
	MAX77693_CHARGER_TYPE_DEAD_BATTERY = 7,
};

/* 
                           
                                              
                          
                                           
 */
struct max77693_muic_irq {
	unsigned int irq;
	const char *name;
	unsigned int virq;
};

static struct max77693_muic_irq muic_irqs[] = {
	{ MAX77693_MUIC_IRQ_INT1_ADC,		"muic-ADC" },
	{ MAX77693_MUIC_IRQ_INT1_ADC_LOW,	"muic-ADCLOW" },
	{ MAX77693_MUIC_IRQ_INT1_ADC_ERR,	"muic-ADCError" },
	{ MAX77693_MUIC_IRQ_INT1_ADC1K,		"muic-ADC1K" },
	{ MAX77693_MUIC_IRQ_INT2_CHGTYP,	"muic-CHGTYP" },
	{ MAX77693_MUIC_IRQ_INT2_CHGDETREUN,	"muic-CHGDETREUN" },
	{ MAX77693_MUIC_IRQ_INT2_DCDTMR,	"muic-DCDTMR" },
	{ MAX77693_MUIC_IRQ_INT2_DXOVP,		"muic-DXOVP" },
	{ MAX77693_MUIC_IRQ_INT2_VBVOLT,	"muic-VBVOLT" },
	{ MAX77693_MUIC_IRQ_INT2_VIDRM,		"muic-VIDRM" },
	{ MAX77693_MUIC_IRQ_INT3_EOC,		"muic-EOC" },
	{ MAX77693_MUIC_IRQ_INT3_CGMBC,		"muic-CGMBC" },
	{ MAX77693_MUIC_IRQ_INT3_OVP,		"muic-OVP" },
	{ MAX77693_MUIC_IRQ_INT3_MBCCHG_ERR,	"muic-MBCCHG_ERR" },
	{ MAX77693_MUIC_IRQ_INT3_CHG_ENABLED,	"muic-CHG_ENABLED" },
	{ MAX77693_MUIC_IRQ_INT3_BAT_DET,	"muic-BAT_DET" },
};

/*                                 */
enum max77693_muic_acc_type {
	MAX77693_MUIC_ADC_GROUND = 0x0,
	MAX77693_MUIC_ADC_SEND_END_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S1_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S2_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S3_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S4_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S5_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S6_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S7_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S8_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S9_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S10_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S11_BUTTON,
	MAX77693_MUIC_ADC_REMOTE_S12_BUTTON,
	MAX77693_MUIC_ADC_RESERVED_ACC_1,
	MAX77693_MUIC_ADC_RESERVED_ACC_2,
	MAX77693_MUIC_ADC_RESERVED_ACC_3,
	MAX77693_MUIC_ADC_RESERVED_ACC_4,
	MAX77693_MUIC_ADC_RESERVED_ACC_5,
	MAX77693_MUIC_ADC_CEA936_AUDIO,
	MAX77693_MUIC_ADC_PHONE_POWERED_DEV,
	MAX77693_MUIC_ADC_TTY_CONVERTER,
	MAX77693_MUIC_ADC_UART_CABLE,
	MAX77693_MUIC_ADC_CEA936A_TYPE1_CHG,
	MAX77693_MUIC_ADC_FACTORY_MODE_USB_OFF,
	MAX77693_MUIC_ADC_FACTORY_MODE_USB_ON,
	MAX77693_MUIC_ADC_AV_CABLE_NOLOAD,
	MAX77693_MUIC_ADC_CEA936A_TYPE2_CHG,
	MAX77693_MUIC_ADC_FACTORY_MODE_UART_OFF,
	MAX77693_MUIC_ADC_FACTORY_MODE_UART_ON,
	MAX77693_MUIC_ADC_AUDIO_MODE_REMOTE,
	MAX77693_MUIC_ADC_OPEN,

	/*                                                        
                                                     */
	MAX77693_MUIC_GND_USB_OTG = 0x100,	/*                                       */
	MAX77693_MUIC_GND_USB_OTG_VB = 0x104,	/*                                       */
	MAX77693_MUIC_GND_AV_CABLE_LOAD = 0x102,/*                                       */
	MAX77693_MUIC_GND_MHL = 0x103,		/*                                       */
	MAX77693_MUIC_GND_MHL_VB = 0x107,	/*                                       */
};

/*                                                                            */
enum {
	EXTCON_CABLE_USB = 0,
	EXTCON_CABLE_USB_HOST,
	EXTCON_CABLE_TA,
	EXTCON_CABLE_FAST_CHARGER,
	EXTCON_CABLE_SLOW_CHARGER,
	EXTCON_CABLE_CHARGE_DOWNSTREAM,
	EXTCON_CABLE_MHL,
	EXTCON_CABLE_MHL_TA,
	EXTCON_CABLE_JIG_USB_ON,
	EXTCON_CABLE_JIG_USB_OFF,
	EXTCON_CABLE_JIG_UART_OFF,
	EXTCON_CABLE_JIG_UART_ON,
	EXTCON_CABLE_DOCK_SMART,
	EXTCON_CABLE_DOCK_DESK,
	EXTCON_CABLE_DOCK_AUDIO,

	_EXTCON_CABLE_NUM,
};

static const char *max77693_extcon_cable[] = {
	[EXTCON_CABLE_USB]			= "USB",
	[EXTCON_CABLE_USB_HOST]			= "USB-Host",
	[EXTCON_CABLE_TA]			= "TA",
	[EXTCON_CABLE_FAST_CHARGER]		= "Fast-charger",
	[EXTCON_CABLE_SLOW_CHARGER]		= "Slow-charger",
	[EXTCON_CABLE_CHARGE_DOWNSTREAM]	= "Charge-downstream",
	[EXTCON_CABLE_MHL]			= "MHL",
	[EXTCON_CABLE_MHL_TA]			= "MHL_TA",
	[EXTCON_CABLE_JIG_USB_ON]		= "JIG-USB-ON",
	[EXTCON_CABLE_JIG_USB_OFF]		= "JIG-USB-OFF",
	[EXTCON_CABLE_JIG_UART_OFF]		= "JIG-UART-OFF",
	[EXTCON_CABLE_JIG_UART_ON]		= "Dock-Car",
	[EXTCON_CABLE_DOCK_SMART]		= "Dock-Smart",
	[EXTCON_CABLE_DOCK_DESK]		= "Dock-Desk",
	[EXTCON_CABLE_DOCK_AUDIO]		= "Dock-Audio",

	NULL,
};

/*
                                                                 
                                                              
                                  
 */
static int max77693_muic_set_debounce_time(struct max77693_muic_info *info,
		enum max77693_muic_adc_debounce_time time)
{
	int ret;

	switch (time) {
	case ADC_DEBOUNCE_TIME_5MS:
	case ADC_DEBOUNCE_TIME_10MS:
	case ADC_DEBOUNCE_TIME_25MS:
	case ADC_DEBOUNCE_TIME_38_62MS:
		ret = max77693_update_reg(info->max77693->regmap_muic,
					  MAX77693_MUIC_REG_CTRL3,
					  time << CONTROL3_ADCDBSET_SHIFT,
					  CONTROL3_ADCDBSET_MASK);
		if (ret) {
			dev_err(info->dev, "failed to set ADC debounce time\n");
			return ret;
		}
		break;
	default:
		dev_err(info->dev, "invalid ADC debounce time\n");
		return -EINVAL;
	}

	return 0;
};

/*
                                                                         
                                                              
                                               
                                                                
  
                                                                           
                                                                           
                  
 */
static int max77693_muic_set_path(struct max77693_muic_info *info,
		u8 val, bool attached)
{
	int ret = 0;
	u8 ctrl1, ctrl2 = 0;

	if (attached)
		ctrl1 = val;
	else
		ctrl1 = CONTROL1_SW_OPEN;

	ret = max77693_update_reg(info->max77693->regmap_muic,
			MAX77693_MUIC_REG_CTRL1, ctrl1, COMP_SW_MASK);
	if (ret < 0) {
		dev_err(info->dev, "failed to update MUIC register\n");
		return ret;
	}

	if (attached)
		ctrl2 |= CONTROL2_CPEN_MASK;	/*                  */
	else
		ctrl2 |= CONTROL2_LOWPWR_MASK;	/*                  */

	ret = max77693_update_reg(info->max77693->regmap_muic,
			MAX77693_MUIC_REG_CTRL2, ctrl2,
			CONTROL2_LOWPWR_MASK | CONTROL2_CPEN_MASK);
	if (ret < 0) {
		dev_err(info->dev, "failed to update MUIC register\n");
		return ret;
	}

	dev_info(info->dev,
		"CONTROL1 : 0x%02x, CONTROL2 : 0x%02x, state : %s\n",
		ctrl1, ctrl2, attached ? "attached" : "detached");

	return 0;
}

/*
                                                                         
                                                              
                                               
                                          
  
                                                                   
                                                                  
                             
                                 
                             
                                
 */
static int max77693_muic_get_cable_type(struct max77693_muic_info *info,
		enum max77693_muic_cable_group group, bool *attached)
{
	int cable_type = 0;
	int adc;
	int adc1k;
	int adclow;
	int vbvolt;
	int chg_type;

	switch (group) {
	case MAX77693_CABLE_GROUP_ADC:
		/*
                                                              
                            
   */
		adc = info->status[0] & STATUS1_ADC_MASK;
		adc >>= STATUS1_ADC_SHIFT;

		/*
                                                              
                                                             
              
   */
		if (adc == MAX77693_MUIC_ADC_OPEN) {
			*attached = false;

			cable_type = info->prev_cable_type;
			info->prev_cable_type = MAX77693_MUIC_ADC_OPEN;
		} else {
			*attached = true;

			cable_type = info->prev_cable_type = adc;
		}
		break;
	case MAX77693_CABLE_GROUP_ADC_GND:
		/*
                                                              
                            
   */
		adc = info->status[0] & STATUS1_ADC_MASK;
		adc >>= STATUS1_ADC_SHIFT;

		/*
                                                              
                                                               
                 
   */
		if (adc == MAX77693_MUIC_ADC_OPEN) {
			*attached = false;

			cable_type = info->prev_cable_type_gnd;
			info->prev_cable_type_gnd = MAX77693_MUIC_ADC_OPEN;
		} else {
			*attached = true;

			adclow = info->status[0] & STATUS1_ADCLOW_MASK;
			adclow >>= STATUS1_ADCLOW_SHIFT;
			adc1k = info->status[0] & STATUS1_ADC1K_MASK;
			adc1k >>= STATUS1_ADC1K_SHIFT;

			vbvolt = info->status[1] & STATUS2_VBVOLT_MASK;
			vbvolt >>= STATUS2_VBVOLT_SHIFT;

			/* 
                                  
                                         
                                            
                                                                
                                                                   
                                                                
    */
			cable_type = ((0x1 << 8)
					| (vbvolt << 2)
					| (adclow << 1)
					| adc1k);

			info->prev_cable_type = adc;
			info->prev_cable_type_gnd = cable_type;
		}

		break;
	case MAX77693_CABLE_GROUP_CHG:
		/*
                                                                 
                                        
   */
		chg_type = info->status[1] & STATUS2_CHGTYP_MASK;
		chg_type >>= STATUS2_CHGTYP_SHIFT;

		if (chg_type == MAX77693_CHARGER_TYPE_NONE) {
			*attached = false;

			cable_type = info->prev_chg_type;
			info->prev_chg_type = MAX77693_CHARGER_TYPE_NONE;
		} else {
			*attached = true;

			/*
                                                          
                                                       
                                
    */
			cable_type = info->prev_chg_type = chg_type;
		}

		break;
	case MAX77693_CABLE_GROUP_VBVOLT:
		/*
                                                              
                            
   */
		adc = info->status[0] & STATUS1_ADC_MASK;
		adc >>= STATUS1_ADC_SHIFT;
		chg_type = info->status[1] & STATUS2_CHGTYP_MASK;
		chg_type >>= STATUS2_CHGTYP_SHIFT;

		if (adc == MAX77693_MUIC_ADC_OPEN
				&& chg_type == MAX77693_CHARGER_TYPE_NONE)
			*attached = false;
		else
			*attached = true;

		/*
                                       
                                     
   */
		vbvolt = info->status[1] & STATUS2_VBVOLT_MASK;
		vbvolt >>= STATUS2_VBVOLT_SHIFT;

		cable_type = vbvolt;
		break;
	default:
		dev_err(info->dev, "Unknown cable group (%d)\n", group);
		cable_type = -EINVAL;
		break;
	}

	return cable_type;
}

static int max77693_muic_dock_handler(struct max77693_muic_info *info,
		int cable_type, bool attached)
{
	int ret = 0;
	int vbvolt;
	bool cable_attached;
	char dock_name[CABLE_NAME_MAX];

	dev_info(info->dev,
		"external connector is %s (adc:0x%02x)\n",
		attached ? "attached" : "detached", cable_type);

	switch (cable_type) {
	case MAX77693_MUIC_ADC_RESERVED_ACC_3:		/*            */
		/*
                                                          
                                                             
                                                           
                                                
   */
		vbvolt = max77693_muic_get_cable_type(info,
				MAX77693_CABLE_GROUP_VBVOLT, &cable_attached);
		if (attached && !vbvolt) {
			dev_warn(info->dev,
				"Cannot detect external power supply\n");
			return 0;
		}

		/*
                                 
                                                          
                                                        
                                                             
                                                              
                                                             
                                                 
    
                                                      
                  
                                               
                                                           
                                                        
                                                                  
                                                                
                 
                                          
   */
		ret = max77693_muic_set_path(info, info->path_usb, attached);
		if (ret < 0)
			return ret;

		extcon_set_cable_state(info->edev, "Dock-Smart", attached);
		extcon_set_cable_state(info->edev, "MHL", attached);
		goto out;
	case MAX77693_MUIC_ADC_FACTORY_MODE_UART_ON:	/*          */
		strcpy(dock_name, "Dock-Car");
		break;
	case MAX77693_MUIC_ADC_AUDIO_MODE_REMOTE:	/*           */
		strcpy(dock_name, "Dock-Desk");
		break;
	case MAX77693_MUIC_ADC_AV_CABLE_NOLOAD:		/*            */
		strcpy(dock_name, "Dock-Audio");
		if (!attached)
			extcon_set_cable_state(info->edev, "USB", false);
		break;
	default:
		dev_err(info->dev, "failed to detect %s dock device\n",
			attached ? "attached" : "detached");
		return -EINVAL;
	}

	/*                                 */
	ret = max77693_muic_set_path(info, CONTROL1_SW_AUDIO, attached);
	if (ret < 0)
		return ret;
	extcon_set_cable_state(info->edev, dock_name, attached);

out:
	return 0;
}

static int max77693_muic_dock_button_handler(struct max77693_muic_info *info,
		int button_type, bool attached)
{
	struct input_dev *dock = info->dock;
	unsigned int code;

	switch (button_type) {
	case MAX77693_MUIC_ADC_REMOTE_S3_BUTTON-1
		... MAX77693_MUIC_ADC_REMOTE_S3_BUTTON+1:
		/*               */
		code = KEY_PREVIOUSSONG;
		break;
	case MAX77693_MUIC_ADC_REMOTE_S7_BUTTON-1
		... MAX77693_MUIC_ADC_REMOTE_S7_BUTTON+1:
		/*               */
		code = KEY_NEXTSONG;
		break;
	case MAX77693_MUIC_ADC_REMOTE_S9_BUTTON:
		/*               */
		code = KEY_VOLUMEDOWN;
		break;
	case MAX77693_MUIC_ADC_REMOTE_S10_BUTTON:
		/*             */
		code = KEY_VOLUMEUP;
		break;
	case MAX77693_MUIC_ADC_REMOTE_S12_BUTTON-1
		... MAX77693_MUIC_ADC_REMOTE_S12_BUTTON+1:
		/*                     */
		code = KEY_PLAYPAUSE;
		break;
	default:
		dev_err(info->dev,
			"failed to detect %s key (adc:0x%x)\n",
			attached ? "pressed" : "released", button_type);
		return -EINVAL;
	}

	input_event(dock, EV_KEY, code, attached);
	input_sync(dock);

	return 0;
}

static int max77693_muic_adc_ground_handler(struct max77693_muic_info *info)
{
	int cable_type_gnd;
	int ret = 0;
	bool attached;

	cable_type_gnd = max77693_muic_get_cable_type(info,
				MAX77693_CABLE_GROUP_ADC_GND, &attached);

	switch (cable_type_gnd) {
	case MAX77693_MUIC_GND_USB_OTG:
	case MAX77693_MUIC_GND_USB_OTG_VB:
		/*                       */
		ret = max77693_muic_set_path(info, CONTROL1_SW_USB, attached);
		if (ret < 0)
			return ret;
		extcon_set_cable_state(info->edev, "USB-Host", attached);
		break;
	case MAX77693_MUIC_GND_AV_CABLE_LOAD:
		/*                                         */
		ret = max77693_muic_set_path(info, CONTROL1_SW_AUDIO, attached);
		if (ret < 0)
			return ret;
		extcon_set_cable_state(info->edev,
				"Audio-video-load", attached);
		break;
	case MAX77693_MUIC_GND_MHL:
	case MAX77693_MUIC_GND_MHL_VB:
		/*                              */
		extcon_set_cable_state(info->edev, "MHL", attached);
		break;
	default:
		dev_err(info->dev, "failed to detect %s cable of gnd type\n",
			attached ? "attached" : "detached");
		return -EINVAL;
	}

	return 0;
}

static int max77693_muic_jig_handler(struct max77693_muic_info *info,
		int cable_type, bool attached)
{
	char cable_name[32];
	int ret = 0;
	u8 path = CONTROL1_SW_OPEN;

	dev_info(info->dev,
		"external connector is %s (adc:0x%02x)\n",
		attached ? "attached" : "detached", cable_type);

	switch (cable_type) {
	case MAX77693_MUIC_ADC_FACTORY_MODE_USB_OFF:	/*                 */
		/*             */
		strcpy(cable_name, "JIG-USB-OFF");
		path = CONTROL1_SW_USB;
		break;
	case MAX77693_MUIC_ADC_FACTORY_MODE_USB_ON:	/*                */
		/*             */
		strcpy(cable_name, "JIG-USB-ON");
		path = CONTROL1_SW_USB;
		break;
	case MAX77693_MUIC_ADC_FACTORY_MODE_UART_OFF:	/*                  */
		/*              */
		strcpy(cable_name, "JIG-UART-OFF");
		path = CONTROL1_SW_UART;
		break;
	default:
		dev_err(info->dev, "failed to detect %s jig cable\n",
			attached ? "attached" : "detached");
		return -EINVAL;
	}

	ret = max77693_muic_set_path(info, path, attached);
	if (ret < 0)
		return ret;

	extcon_set_cable_state(info->edev, cable_name, attached);

	return 0;
}

static int max77693_muic_adc_handler(struct max77693_muic_info *info)
{
	int cable_type;
	int button_type;
	bool attached;
	int ret = 0;

	/*                                                            */
	cable_type = max77693_muic_get_cable_type(info,
				MAX77693_CABLE_GROUP_ADC, &attached);

	dev_info(info->dev,
		"external connector is %s (adc:0x%02x, prev_adc:0x%x)\n",
		attached ? "attached" : "detached", cable_type,
		info->prev_cable_type);

	switch (cable_type) {
	case MAX77693_MUIC_ADC_GROUND:
		/*                   */
		max77693_muic_adc_ground_handler(info);
		break;
	case MAX77693_MUIC_ADC_FACTORY_MODE_USB_OFF:
	case MAX77693_MUIC_ADC_FACTORY_MODE_USB_ON:
	case MAX77693_MUIC_ADC_FACTORY_MODE_UART_OFF:
		/*     */
		ret = max77693_muic_jig_handler(info, cable_type, attached);
		if (ret < 0)
			return ret;
		break;
	case MAX77693_MUIC_ADC_RESERVED_ACC_3:		/*            */
	case MAX77693_MUIC_ADC_FACTORY_MODE_UART_ON:	/*          */
	case MAX77693_MUIC_ADC_AUDIO_MODE_REMOTE:	/*           */
	case MAX77693_MUIC_ADC_AV_CABLE_NOLOAD:		/*            */
		/*
                
    
                                                            
                                                          
                                                            
                                                                  
                                 
   */
		ret = max77693_muic_dock_handler(info, cable_type, attached);
		if (ret < 0)
			return ret;
		break;
	case MAX77693_MUIC_ADC_REMOTE_S3_BUTTON:	/*               */
	case MAX77693_MUIC_ADC_REMOTE_S7_BUTTON:	/*               */
	case MAX77693_MUIC_ADC_REMOTE_S9_BUTTON:	/*               */
	case MAX77693_MUIC_ADC_REMOTE_S10_BUTTON:	/*             */
	case MAX77693_MUIC_ADC_REMOTE_S12_BUTTON:	/*                     */
		/*
                          
                                                            
    
                                                            
                                                          
                                                            
                                                                  
                                 
   */
		if (attached)
			button_type = info->prev_button_type = cable_type;
		else
			button_type = info->prev_button_type;

		ret = max77693_muic_dock_button_handler(info, button_type,
							attached);
		if (ret < 0)
			return ret;
		break;
	case MAX77693_MUIC_ADC_SEND_END_BUTTON:
	case MAX77693_MUIC_ADC_REMOTE_S1_BUTTON:
	case MAX77693_MUIC_ADC_REMOTE_S2_BUTTON:
	case MAX77693_MUIC_ADC_REMOTE_S4_BUTTON:
	case MAX77693_MUIC_ADC_REMOTE_S5_BUTTON:
	case MAX77693_MUIC_ADC_REMOTE_S6_BUTTON:
	case MAX77693_MUIC_ADC_REMOTE_S8_BUTTON:
	case MAX77693_MUIC_ADC_REMOTE_S11_BUTTON:
	case MAX77693_MUIC_ADC_RESERVED_ACC_1:
	case MAX77693_MUIC_ADC_RESERVED_ACC_2:
	case MAX77693_MUIC_ADC_RESERVED_ACC_4:
	case MAX77693_MUIC_ADC_RESERVED_ACC_5:
	case MAX77693_MUIC_ADC_CEA936_AUDIO:
	case MAX77693_MUIC_ADC_PHONE_POWERED_DEV:
	case MAX77693_MUIC_ADC_TTY_CONVERTER:
	case MAX77693_MUIC_ADC_UART_CABLE:
	case MAX77693_MUIC_ADC_CEA936A_TYPE1_CHG:
	case MAX77693_MUIC_ADC_CEA936A_TYPE2_CHG:
		/*
                                                                 
                                                            
                                                               
   */
		dev_info(info->dev,
			"accessory is %s but it isn't used (adc:0x%x)\n",
			attached ? "attached" : "detached", cable_type);
		return -EAGAIN;
	default:
		dev_err(info->dev,
			"failed to detect %s accessory (adc:0x%x)\n",
			attached ? "attached" : "detached", cable_type);
		return -EINVAL;
	}

	return 0;
}

static int max77693_muic_chg_handler(struct max77693_muic_info *info)
{
	int chg_type;
	int cable_type_gnd;
	int cable_type;
	bool attached;
	bool cable_attached;
	int ret = 0;

	chg_type = max77693_muic_get_cable_type(info,
				MAX77693_CABLE_GROUP_CHG, &attached);

	dev_info(info->dev,
		"external connector is %s(chg_type:0x%x, prev_chg_type:0x%x)\n",
			attached ? "attached" : "detached",
			chg_type, info->prev_chg_type);

	switch (chg_type) {
	case MAX77693_CHARGER_TYPE_USB:
	case MAX77693_CHARGER_TYPE_DEDICATED_CHG:
	case MAX77693_CHARGER_TYPE_NONE:
		/*                                         */
		cable_type_gnd = max77693_muic_get_cable_type(info,
					MAX77693_CABLE_GROUP_ADC_GND,
					&cable_attached);
		switch (cable_type_gnd) {
		case MAX77693_MUIC_GND_MHL:
		case MAX77693_MUIC_GND_MHL_VB:
			/*
                                         
                                                                
                                                                
                                                                
                                                                
                           
     
                                                 
                   
                                                                       
    */
			extcon_set_cable_state(info->edev, "MHL_TA", attached);
			if (!cable_attached)
				extcon_set_cable_state(info->edev, "MHL", cable_attached);
			break;
		}

		/*                                     */
		cable_type = max77693_muic_get_cable_type(info,
					MAX77693_CABLE_GROUP_ADC,
					&cable_attached);
		switch (cable_type) {
		case MAX77693_MUIC_ADC_AV_CABLE_NOLOAD:		/*            */
			/*
                                         
                                                             
                                                              
                                                               
                                                                
                           
     
                                                       
                                                 
                                                            
                           
    */
			extcon_set_cable_state(info->edev, "USB", attached);

			if (!cable_attached)
				extcon_set_cable_state(info->edev, "Dock-Audio", cable_attached);
			break;
		case MAX77693_MUIC_ADC_RESERVED_ACC_3:		/*            */
			/*
                                         
                                                          
                                                         
                                                              
                                                               
                                                              
                                                  
     
                                                       
                   
                                                
                                                            
                                                         
                                                                   
                                                                 
                  
                                           
    */
			ret = max77693_muic_set_path(info, info->path_usb, attached);
			if (ret < 0)
				return ret;

			extcon_set_cable_state(info->edev, "Dock-Smart", attached);
			extcon_set_cable_state(info->edev, "MHL", attached);

			break;
		}

		/*                                     */
		switch (chg_type) {
		case MAX77693_CHARGER_TYPE_NONE:
			/*
                                                                 
                                                          
                                                                  
                                                                  
                                       
                                                                 
                                                   
     
                                                                  
                                                                
                                                                   
                                                                  
                                                               
                                                               
    */
			break;
		case MAX77693_CHARGER_TYPE_USB:
			/*                             */
			ret = max77693_muic_set_path(info, info->path_usb, attached);
			if (ret < 0)
				return ret;

			extcon_set_cable_state(info->edev, "USB", attached);
			break;
		case MAX77693_CHARGER_TYPE_DEDICATED_CHG:
			/*               */
			extcon_set_cable_state(info->edev, "TA", attached);
			break;
		}
		break;
	case MAX77693_CHARGER_TYPE_DOWNSTREAM_PORT:
		extcon_set_cable_state(info->edev,
				"Charge-downstream", attached);
		break;
	case MAX77693_CHARGER_TYPE_APPLE_500MA:
		extcon_set_cable_state(info->edev, "Slow-charger", attached);
		break;
	case MAX77693_CHARGER_TYPE_APPLE_1A_2A:
		extcon_set_cable_state(info->edev, "Fast-charger", attached);
		break;
	case MAX77693_CHARGER_TYPE_DEAD_BATTERY:
		break;
	default:
		dev_err(info->dev,
			"failed to detect %s accessory (chg_type:0x%x)\n",
			attached ? "attached" : "detached", chg_type);
		return -EINVAL;
	}

	return 0;
}

static void max77693_muic_irq_work(struct work_struct *work)
{
	struct max77693_muic_info *info = container_of(work,
			struct max77693_muic_info, irq_work);
	int irq_type = -1;
	int i, ret = 0;

	if (!info->edev)
		return;

	mutex_lock(&info->mutex);

	for (i = 0 ; i < ARRAY_SIZE(muic_irqs) ; i++)
		if (info->irq == muic_irqs[i].virq)
			irq_type = muic_irqs[i].irq;

	ret = max77693_bulk_read(info->max77693->regmap_muic,
			MAX77693_MUIC_REG_STATUS1, 2, info->status);
	if (ret) {
		dev_err(info->dev, "failed to read MUIC register\n");
		mutex_unlock(&info->mutex);
		return;
	}

	switch (irq_type) {
	case MAX77693_MUIC_IRQ_INT1_ADC:
	case MAX77693_MUIC_IRQ_INT1_ADC_LOW:
	case MAX77693_MUIC_IRQ_INT1_ADC_ERR:
	case MAX77693_MUIC_IRQ_INT1_ADC1K:
		/*                                   
                               */
		ret = max77693_muic_adc_handler(info);
		break;
	case MAX77693_MUIC_IRQ_INT2_CHGTYP:
	case MAX77693_MUIC_IRQ_INT2_CHGDETREUN:
	case MAX77693_MUIC_IRQ_INT2_DCDTMR:
	case MAX77693_MUIC_IRQ_INT2_DXOVP:
	case MAX77693_MUIC_IRQ_INT2_VBVOLT:
	case MAX77693_MUIC_IRQ_INT2_VIDRM:
		/*                          */
		ret = max77693_muic_chg_handler(info);
		break;
	case MAX77693_MUIC_IRQ_INT3_EOC:
	case MAX77693_MUIC_IRQ_INT3_CGMBC:
	case MAX77693_MUIC_IRQ_INT3_OVP:
	case MAX77693_MUIC_IRQ_INT3_MBCCHG_ERR:
	case MAX77693_MUIC_IRQ_INT3_CHG_ENABLED:
	case MAX77693_MUIC_IRQ_INT3_BAT_DET:
		break;
	default:
		dev_err(info->dev, "muic interrupt: irq %d occurred\n",
				irq_type);
		mutex_unlock(&info->mutex);
		return;
	}

	if (ret < 0)
		dev_err(info->dev, "failed to handle MUIC interrupt\n");

	mutex_unlock(&info->mutex);

	return;
}

static irqreturn_t max77693_muic_irq_handler(int irq, void *data)
{
	struct max77693_muic_info *info = data;

	info->irq = irq;
	schedule_work(&info->irq_work);

	return IRQ_HANDLED;
}

static struct regmap_config max77693_muic_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static int max77693_muic_detect_accessory(struct max77693_muic_info *info)
{
	int ret = 0;
	int adc;
	int chg_type;
	bool attached;

	mutex_lock(&info->mutex);

	/*                                           */
	ret = max77693_bulk_read(info->max77693->regmap_muic,
			MAX77693_MUIC_REG_STATUS1, 2, info->status);
	if (ret) {
		dev_err(info->dev, "failed to read MUIC register\n");
		mutex_unlock(&info->mutex);
		return ret;
	}

	adc = max77693_muic_get_cable_type(info, MAX77693_CABLE_GROUP_ADC,
					&attached);
	if (attached && adc != MAX77693_MUIC_ADC_OPEN) {
		ret = max77693_muic_adc_handler(info);
		if (ret < 0) {
			dev_err(info->dev, "Cannot detect accessory\n");
			mutex_unlock(&info->mutex);
			return ret;
		}
	}

	chg_type = max77693_muic_get_cable_type(info, MAX77693_CABLE_GROUP_CHG,
					&attached);
	if (attached && chg_type != MAX77693_CHARGER_TYPE_NONE) {
		ret = max77693_muic_chg_handler(info);
		if (ret < 0) {
			dev_err(info->dev, "Cannot detect charger accessory\n");
			mutex_unlock(&info->mutex);
			return ret;
		}
	}

	mutex_unlock(&info->mutex);

	return 0;
}

static void max77693_muic_detect_cable_wq(struct work_struct *work)
{
	struct max77693_muic_info *info = container_of(to_delayed_work(work),
				struct max77693_muic_info, wq_detcable);

	max77693_muic_detect_accessory(info);
}

static int max77693_muic_probe(struct platform_device *pdev)
{
	struct max77693_dev *max77693 = dev_get_drvdata(pdev->dev.parent);
	struct max77693_platform_data *pdata = dev_get_platdata(max77693->dev);
	struct max77693_muic_info *info;
	struct max77693_reg_data *init_data;
	int num_init_data;
	int delay_jiffies;
	int ret;
	int i;
	u8 id;

	info = devm_kzalloc(&pdev->dev, sizeof(struct max77693_muic_info),
				   GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "failed to allocate memory\n");
		return -ENOMEM;
	}
	info->dev = &pdev->dev;
	info->max77693 = max77693;
	if (info->max77693->regmap_muic) {
		dev_dbg(&pdev->dev, "allocate register map\n");
	} else {
		info->max77693->regmap_muic = devm_regmap_init_i2c(
						info->max77693->muic,
						&max77693_muic_regmap_config);
		if (IS_ERR(info->max77693->regmap_muic)) {
			ret = PTR_ERR(info->max77693->regmap_muic);
			dev_err(max77693->dev,
				"failed to allocate register map: %d\n", ret);
			return ret;
		}
	}

	/*                                                 */
	info->dock = devm_input_allocate_device(&pdev->dev);
	if (!info->dock) {
		dev_err(&pdev->dev, "%s: failed to allocate input\n", __func__);
		return -ENOMEM;
	}
	info->dock->name = "max77693-muic/dock";
	info->dock->phys = "max77693-muic/extcon";
	info->dock->dev.parent = &pdev->dev;

	__set_bit(EV_REP, info->dock->evbit);

	input_set_capability(info->dock, EV_KEY, KEY_VOLUMEUP);
	input_set_capability(info->dock, EV_KEY, KEY_VOLUMEDOWN);
	input_set_capability(info->dock, EV_KEY, KEY_PLAYPAUSE);
	input_set_capability(info->dock, EV_KEY, KEY_PREVIOUSSONG);
	input_set_capability(info->dock, EV_KEY, KEY_NEXTSONG);

	ret = input_register_device(info->dock);
	if (ret < 0) {
		dev_err(&pdev->dev, "Cannot register input device error(%d)\n",
				ret);
		return ret;
	}

	platform_set_drvdata(pdev, info);
	mutex_init(&info->mutex);

	INIT_WORK(&info->irq_work, max77693_muic_irq_work);

	/*                                             */
	for (i = 0; i < ARRAY_SIZE(muic_irqs); i++) {
		struct max77693_muic_irq *muic_irq = &muic_irqs[i];
		unsigned int virq = 0;

		virq = irq_create_mapping(max77693->irq_domain, muic_irq->irq);
		if (!virq) {
			ret = -EINVAL;
			goto err_irq;
		}
		muic_irq->virq = virq;

		ret = request_threaded_irq(virq, NULL,
				max77693_muic_irq_handler,
				IRQF_NO_SUSPEND,
				muic_irq->name, info);
		if (ret) {
			dev_err(&pdev->dev,
				"failed: irq request (IRQ: %d,"
				" error :%d)\n",
				muic_irq->irq, ret);
			goto err_irq;
		}
	}

	/*                          */
	info->edev = devm_kzalloc(&pdev->dev, sizeof(struct extcon_dev),
				  GFP_KERNEL);
	if (!info->edev) {
		dev_err(&pdev->dev, "failed to allocate memory for extcon\n");
		ret = -ENOMEM;
		goto err_irq;
	}
	info->edev->name = DEV_NAME;
	info->edev->supported_cable = max77693_extcon_cable;
	ret = extcon_dev_register(info->edev, NULL);
	if (ret) {
		dev_err(&pdev->dev, "failed to register extcon device\n");
		goto err_irq;
	}


	/*                                                                 */
	if (pdata && pdata->muic_data) {
		init_data = pdata->muic_data->init_data;
		num_init_data = pdata->muic_data->num_init_data;
	} else {
		init_data = default_init_data;
		num_init_data = ARRAY_SIZE(default_init_data);
	}

	for (i = 0 ; i < num_init_data ; i++) {
		enum max77693_irq_source irq_src
				= MAX77693_IRQ_GROUP_NR;

		max77693_write_reg(info->max77693->regmap_muic,
				init_data[i].addr,
				init_data[i].data);

		switch (init_data[i].addr) {
		case MAX77693_MUIC_REG_INTMASK1:
			irq_src = MUIC_INT1;
			break;
		case MAX77693_MUIC_REG_INTMASK2:
			irq_src = MUIC_INT2;
			break;
		case MAX77693_MUIC_REG_INTMASK3:
			irq_src = MUIC_INT3;
			break;
		}

		if (irq_src < MAX77693_IRQ_GROUP_NR)
			info->max77693->irq_masks_cur[irq_src]
				= init_data[i].data;
	}

	if (pdata && pdata->muic_data) {
		struct max77693_muic_platform_data *muic_pdata = pdata->muic_data;

		/*
                                                               
                                                  
   */
		if (muic_pdata->path_uart)
			info->path_uart = muic_pdata->path_uart;
		else
			info->path_uart = CONTROL1_SW_UART;

		if (muic_pdata->path_usb)
			info->path_usb = muic_pdata->path_usb;
		else
			info->path_usb = CONTROL1_SW_USB;

		/*
                                                 
                        
   */
		if (muic_pdata->detcable_delay_ms)
			delay_jiffies =
				msecs_to_jiffies(muic_pdata->detcable_delay_ms);
		else
			delay_jiffies = msecs_to_jiffies(DELAY_MS_DEFAULT);
	} else {
		info->path_usb = CONTROL1_SW_USB;
		info->path_uart = CONTROL1_SW_UART;
		delay_jiffies = msecs_to_jiffies(DELAY_MS_DEFAULT);
	}

	/*                           */
	 max77693_muic_set_path(info, info->path_uart, true);

	/*                                     */
	ret = max77693_read_reg(info->max77693->regmap_muic,
			MAX77693_MUIC_REG_ID, &id);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to read revision number\n");
		goto err_extcon;
	}
	dev_info(info->dev, "device ID : 0x%x\n", id);

	/*                       */
	max77693_muic_set_debounce_time(info, ADC_DEBOUNCE_TIME_25MS);

	/*
                                                                    
   
                                                          
                                                           
                                                                 
                                                    
  */
	INIT_DELAYED_WORK(&info->wq_detcable, max77693_muic_detect_cable_wq);
	schedule_delayed_work(&info->wq_detcable, delay_jiffies);

	return ret;

err_extcon:
	extcon_dev_unregister(info->edev);
err_irq:
	while (--i >= 0)
		free_irq(muic_irqs[i].virq, info);
	return ret;
}

static int max77693_muic_remove(struct platform_device *pdev)
{
	struct max77693_muic_info *info = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < ARRAY_SIZE(muic_irqs); i++)
		free_irq(muic_irqs[i].virq, info);
	cancel_work_sync(&info->irq_work);
	input_unregister_device(info->dock);
	extcon_dev_unregister(info->edev);

	return 0;
}

static struct platform_driver max77693_muic_driver = {
	.driver		= {
		.name	= DEV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= max77693_muic_probe,
	.remove		= max77693_muic_remove,
};

module_platform_driver(max77693_muic_driver);

MODULE_DESCRIPTION("Maxim MAX77693 Extcon driver");
MODULE_AUTHOR("Chanwoo Choi <cw00.choi@samsung.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:extcon-max77693");
