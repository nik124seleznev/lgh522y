/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * License Terms: GNU General Public License v2
 * Author: Arun R Murthy <arun.murthy@stericsson.com>
 * Author: Daniel Willerud <daniel.willerud@stericsson.com>
 * Author: Johan Palsson <johan.palsson@stericsson.com>
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/pm_runtime.h>
#include <linux/platform_device.h>
#include <linux/completion.h>
#include <linux/regulator/consumer.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mfd/abx500.h>
#include <linux/mfd/abx500/ab8500.h>
#include <linux/mfd/abx500/ab8500-gpadc.h>

/*
                         
              
 */
#define AB8500_GPADC_CTRL1_REG		0x00
#define AB8500_GPADC_CTRL2_REG		0x01
#define AB8500_GPADC_CTRL3_REG		0x02
#define AB8500_GPADC_AUTO_TIMER_REG	0x03
#define AB8500_GPADC_STAT_REG		0x04
#define AB8500_GPADC_MANDATAL_REG	0x05
#define AB8500_GPADC_MANDATAH_REG	0x06
#define AB8500_GPADC_AUTODATAL_REG	0x07
#define AB8500_GPADC_AUTODATAH_REG	0x08
#define AB8500_GPADC_MUX_CTRL_REG	0x09
#define AB8540_GPADC_MANDATA2L_REG	0x09
#define AB8540_GPADC_MANDATA2H_REG	0x0A
#define AB8540_GPADC_APEAAX_REG		0x10
#define AB8540_GPADC_APEAAT_REG		0x11
#define AB8540_GPADC_APEAAM_REG		0x12
#define AB8540_GPADC_APEAAH_REG		0x13
#define AB8540_GPADC_APEAAL_REG		0x14

/*
                       
              
 */
#define AB8500_GPADC_CAL_1		0x0F
#define AB8500_GPADC_CAL_2		0x10
#define AB8500_GPADC_CAL_3		0x11
#define AB8500_GPADC_CAL_4		0x12
#define AB8500_GPADC_CAL_5		0x13
#define AB8500_GPADC_CAL_6		0x14
#define AB8500_GPADC_CAL_7		0x15
/*                          */
#define AB8540_GPADC_OTP4_REG_7	0x38
#define AB8540_GPADC_OTP4_REG_6	0x39
#define AB8540_GPADC_OTP4_REG_5	0x3A

/*                 */
#define EN_VINTCORE12			0x04
#define EN_VTVOUT			0x02
#define EN_GPADC			0x01
#define DIS_GPADC			0x00
#define AVG_1				0x00
#define AVG_4				0x20
#define AVG_8				0x40
#define AVG_16				0x60
#define ADC_SW_CONV			0x04
#define EN_ICHAR			0x80
#define BTEMP_PULL_UP			0x08
#define EN_BUF				0x40
#define DIS_ZERO			0x00
#define GPADC_BUSY			0x01
#define EN_FALLING			0x10
#define EN_TRIG_EDGE			0x02
#define EN_VBIAS_XTAL_TEMP		0x02

/*                                          */
#define ADC_RESOLUTION			1024
#define ADC_CH_BTEMP_MIN		0
#define ADC_CH_BTEMP_MAX		1350
#define ADC_CH_DIETEMP_MIN		0
#define ADC_CH_DIETEMP_MAX		1350
#define ADC_CH_CHG_V_MIN		0
#define ADC_CH_CHG_V_MAX		20030
#define ADC_CH_ACCDET2_MIN		0
#define ADC_CH_ACCDET2_MAX		2500
#define ADC_CH_VBAT_MIN			2300
#define ADC_CH_VBAT_MAX			4800
#define ADC_CH_CHG_I_MIN		0
#define ADC_CH_CHG_I_MAX		1500
#define ADC_CH_BKBAT_MIN		0
#define ADC_CH_BKBAT_MAX		3200

/*                                  */
#define ADC_CH_IBAT_MIN			(-6000) /*                                  */
#define ADC_CH_IBAT_MAX			6000
#define ADC_CH_IBAT_MIN_V		(-60)	/*                                  */
#define ADC_CH_IBAT_MAX_V		60
#define IBAT_VDROP_L			(-56)  /*    */
#define IBAT_VDROP_H			56

/*                                                                         */
#define CALIB_SCALE		1000
/*
                                                  
                                  
 */
#define CALIB_SHIFT_IBAT	20

/*                                       */
#define GPADC_AUDOSUSPEND_DELAY		1

#define CONVERSION_TIME			500 /*    */

enum cal_channels {
	ADC_INPUT_VMAIN = 0,
	ADC_INPUT_BTEMP,
	ADC_INPUT_VBAT,
	ADC_INPUT_IBAT,
	NBR_CAL_INPUTS,
};

/* 
                                                                             
               
                                  
                                      
 */
struct adc_cal_data {
	s64 gain;
	s64 offset;
	u16 otp_calib_hi;
	u16 otp_calib_lo;
};

/* 
                                                        
                                       
                                                       
              
                                          
                                                                        
                                        
                                               
                                           
                                                           
                
                                                           
                
                                                    
 */
struct ab8500_gpadc {
	struct device *dev;
	struct list_head node;
	struct ab8500 *parent;
	struct completion ab8500_gpadc_complete;
	struct mutex ab8500_gpadc_lock;
	struct regulator *regu;
	int irq_sw;
	int irq_hw;
	struct adc_cal_data cal_data[NBR_CAL_INPUTS];
};

static LIST_HEAD(ab8500_gpadc_list);

/* 
                                                                       
                                              
 */
struct ab8500_gpadc *ab8500_gpadc_get(char *name)
{
	struct ab8500_gpadc *gpadc;

	list_for_each_entry(gpadc, &ab8500_gpadc_list, node) {
		if (!strcmp(name, dev_name(gpadc->dev)))
		    return gpadc;
	}

	return ERR_PTR(-ENOENT);
}
EXPORT_SYMBOL(ab8500_gpadc_get);

/* 
                                                                      
 */
int ab8500_gpadc_ad_to_voltage(struct ab8500_gpadc *gpadc, u8 channel,
	int ad_value)
{
	int res;

	switch (channel) {
	case MAIN_CHARGER_V:
		/*                                               */
		if (!gpadc->cal_data[ADC_INPUT_VMAIN].gain) {
			res = ADC_CH_CHG_V_MIN + (ADC_CH_CHG_V_MAX -
				ADC_CH_CHG_V_MIN) * ad_value /
				ADC_RESOLUTION;
			break;
		}
		/*                                     */
		res = (int) (ad_value * gpadc->cal_data[ADC_INPUT_VMAIN].gain +
			gpadc->cal_data[ADC_INPUT_VMAIN].offset) / CALIB_SCALE;
		break;

	case XTAL_TEMP:
	case BAT_CTRL:
	case BTEMP_BALL:
	case ACC_DETECT1:
	case ADC_AUX1:
	case ADC_AUX2:
		/*                                               */
		if (!gpadc->cal_data[ADC_INPUT_BTEMP].gain) {
			res = ADC_CH_BTEMP_MIN + (ADC_CH_BTEMP_MAX -
				ADC_CH_BTEMP_MIN) * ad_value /
				ADC_RESOLUTION;
			break;
		}
		/*                                     */
		res = (int) (ad_value * gpadc->cal_data[ADC_INPUT_BTEMP].gain +
			gpadc->cal_data[ADC_INPUT_BTEMP].offset) / CALIB_SCALE;
		break;

	case MAIN_BAT_V:
	case VBAT_TRUE_MEAS:
		/*                                               */
		if (!gpadc->cal_data[ADC_INPUT_VBAT].gain) {
			res = ADC_CH_VBAT_MIN + (ADC_CH_VBAT_MAX -
				ADC_CH_VBAT_MIN) * ad_value /
				ADC_RESOLUTION;
			break;
		}
		/*                                     */
		res = (int) (ad_value * gpadc->cal_data[ADC_INPUT_VBAT].gain +
			gpadc->cal_data[ADC_INPUT_VBAT].offset) / CALIB_SCALE;
		break;

	case DIE_TEMP:
		res = ADC_CH_DIETEMP_MIN +
			(ADC_CH_DIETEMP_MAX - ADC_CH_DIETEMP_MIN) * ad_value /
			ADC_RESOLUTION;
		break;

	case ACC_DETECT2:
		res = ADC_CH_ACCDET2_MIN +
			(ADC_CH_ACCDET2_MAX - ADC_CH_ACCDET2_MIN) * ad_value /
			ADC_RESOLUTION;
		break;

	case VBUS_V:
		res = ADC_CH_CHG_V_MIN +
			(ADC_CH_CHG_V_MAX - ADC_CH_CHG_V_MIN) * ad_value /
			ADC_RESOLUTION;
		break;

	case MAIN_CHARGER_C:
	case USB_CHARGER_C:
		res = ADC_CH_CHG_I_MIN +
			(ADC_CH_CHG_I_MAX - ADC_CH_CHG_I_MIN) * ad_value /
			ADC_RESOLUTION;
		break;

	case BK_BAT_V:
		res = ADC_CH_BKBAT_MIN +
			(ADC_CH_BKBAT_MAX - ADC_CH_BKBAT_MIN) * ad_value /
			ADC_RESOLUTION;
		break;

	case IBAT_VIRTUAL_CHANNEL:
		/*                                               */
		if (!gpadc->cal_data[ADC_INPUT_IBAT].gain) {
			res = ADC_CH_IBAT_MIN + (ADC_CH_IBAT_MAX -
				ADC_CH_IBAT_MIN) * ad_value /
				ADC_RESOLUTION;
			break;
		}
		/*                                     */
		res = (int) (ad_value * gpadc->cal_data[ADC_INPUT_IBAT].gain +
				gpadc->cal_data[ADC_INPUT_IBAT].offset)
				>> CALIB_SHIFT_IBAT;
		break;

	default:
		dev_err(gpadc->dev,
			"unknown channel, not possible to convert\n");
		res = -EINVAL;
		break;

	}
	return res;
}
EXPORT_SYMBOL(ab8500_gpadc_ad_to_voltage);

/* 
                                                  
                                                           
                                                
                                      
                                                
                                                             
  
                                                            
        
 */
int ab8500_gpadc_sw_hw_convert(struct ab8500_gpadc *gpadc, u8 channel,
		u8 avg_sample, u8 trig_edge, u8 trig_timer, u8 conv_type)
{
	int ad_value;
	int voltage;

	ad_value = ab8500_gpadc_read_raw(gpadc, channel, avg_sample,
			trig_edge, trig_timer, conv_type);
/*                                */
	if (ad_value < 0)
		ad_value = ab8500_gpadc_read_raw(gpadc, channel, avg_sample,
			trig_edge, trig_timer, conv_type);
if (ad_value < 0) {
		dev_err(gpadc->dev, "GPADC raw value failed ch: %d\n",
				channel);
		return ad_value;
	}

	voltage = ab8500_gpadc_ad_to_voltage(gpadc, channel, ad_value);
	if (voltage < 0)
		dev_err(gpadc->dev, "GPADC to voltage conversion failed ch:"
			" %d AD: 0x%x\n", channel, ad_value);

	return voltage;
}
EXPORT_SYMBOL(ab8500_gpadc_sw_hw_convert);

/* 
                                       
                                      
                                                
                                  
                                                
                                                             
  
                                                                      
                                                                          
 */
int ab8500_gpadc_read_raw(struct ab8500_gpadc *gpadc, u8 channel,
		u8 avg_sample, u8 trig_edge, u8 trig_timer, u8 conv_type)
{
	int raw_data;
	raw_data = ab8500_gpadc_double_read_raw(gpadc, channel,
			avg_sample, trig_edge, trig_timer, conv_type, NULL);
	return raw_data;
}

int ab8500_gpadc_double_read_raw(struct ab8500_gpadc *gpadc, u8 channel,
		u8 avg_sample, u8 trig_edge, u8 trig_timer, u8 conv_type,
		int *ibat)
{
	int ret;
	int looplimit = 0;
	unsigned long completion_timeout;
	u8 val, low_data, high_data, low_data2, high_data2;
	u8 val_reg1 = 0;
	unsigned int delay_min = 0;
	unsigned int delay_max = 0;
	u8 data_low_addr, data_high_addr;

	if (!gpadc)
		return -ENODEV;

	/*                                  */
	if ((gpadc->irq_sw < 0) && (conv_type == ADC_SW))
		return -ENOTSUPP;
	if ((gpadc->irq_hw < 0) && (conv_type == ADC_HW))
		return -ENOTSUPP;

	mutex_lock(&gpadc->ab8500_gpadc_lock);
	/*                                              */
	pm_runtime_get_sync(gpadc->dev);

	/*                                            */
	do {
		ret = abx500_get_register_interruptible(gpadc->dev,
			AB8500_GPADC, AB8500_GPADC_STAT_REG, &val);
		if (ret < 0)
			goto out;
		if (!(val & GPADC_BUSY))
			break;
		msleep(10);
	} while (++looplimit < 10);
	if (looplimit >= 10 && (val & GPADC_BUSY)) {
		dev_err(gpadc->dev, "gpadc_conversion: GPADC busy");
		ret = -EINVAL;
		goto out;
	}

	/*              */
	val_reg1 |= EN_GPADC;

	/*                                                   */
	switch (avg_sample) {
	case SAMPLE_1:
		val = channel | AVG_1;
		break;
	case SAMPLE_4:
		val = channel | AVG_4;
		break;
	case SAMPLE_8:
		val = channel | AVG_8;
		break;
	default:
		val = channel | AVG_16;
		break;
	}

	if (conv_type == ADC_HW) {
		ret = abx500_set_register_interruptible(gpadc->dev,
				AB8500_GPADC, AB8500_GPADC_CTRL3_REG, val);
		val_reg1 |= EN_TRIG_EDGE;
		if (trig_edge)
			val_reg1 |= EN_FALLING;
	}
	else
		ret = abx500_set_register_interruptible(gpadc->dev,
				AB8500_GPADC, AB8500_GPADC_CTRL2_REG, val);
	if (ret < 0) {
		dev_err(gpadc->dev,
			"gpadc_conversion: set avg samples failed\n");
		goto out;
	}

	/*
                                                                 
                                                                   
                  
  */
	switch (channel) {
	case MAIN_CHARGER_C:
	case USB_CHARGER_C:
		val_reg1 |= EN_BUF | EN_ICHAR;
		break;
	case BTEMP_BALL:
		if (!is_ab8500_2p0_or_earlier(gpadc->parent)) {
			val_reg1 |= EN_BUF | BTEMP_PULL_UP;
			/*
                                                       
                                           
   */
			delay_min = 1000; /*                        */
			delay_max = 10000; /*                                    */
			break;
		}
		/*                         */
	default:
		val_reg1 |= EN_BUF;
		break;
	}

	/*                                 */
	ret = abx500_set_register_interruptible(gpadc->dev,
		AB8500_GPADC, AB8500_GPADC_CTRL1_REG, val_reg1);
	if (ret < 0) {
		dev_err(gpadc->dev,
			"gpadc_conversion: set Control register failed\n");
		goto out;
	}

	if (delay_min != 0)
		usleep_range(delay_min, delay_max);

	if (conv_type == ADC_HW) {
		/*                         */
		ret = abx500_set_register_interruptible(gpadc->dev,
			AB8500_GPADC, AB8500_GPADC_AUTO_TIMER_REG, trig_timer);
		if (ret < 0) {
			dev_err(gpadc->dev,
				"gpadc_conversion: trig timer failed\n");
			goto out;
		}
		completion_timeout = 2 * HZ;
		data_low_addr = AB8500_GPADC_AUTODATAL_REG;
		data_high_addr = AB8500_GPADC_AUTODATAH_REG;
	} else {
		/*                     */
		ret = abx500_mask_and_set_register_interruptible(gpadc->dev,
			AB8500_GPADC, AB8500_GPADC_CTRL1_REG,
			ADC_SW_CONV, ADC_SW_CONV);
		if (ret < 0) {
			dev_err(gpadc->dev,
				"gpadc_conversion: start s/w conv failed\n");
			goto out;
		}
		completion_timeout = msecs_to_jiffies(CONVERSION_TIME);
		data_low_addr = AB8500_GPADC_MANDATAL_REG;
		data_high_addr = AB8500_GPADC_MANDATAH_REG;
	}

	/*                                   */
	if (!wait_for_completion_timeout(&gpadc->ab8500_gpadc_complete,
			completion_timeout)) {
		dev_err(gpadc->dev,
			"timeout didn't receive GPADC conv interrupt\n");
		ret = -EINVAL;
		goto out;
	}

	/*                             */
	ret = abx500_get_register_interruptible(gpadc->dev,
			AB8500_GPADC, data_low_addr, &low_data);
	if (ret < 0) {
		dev_err(gpadc->dev, "gpadc_conversion: read low data failed\n");
		goto out;
	}

	ret = abx500_get_register_interruptible(gpadc->dev,
		AB8500_GPADC, data_high_addr, &high_data);
	if (ret < 0) {
		dev_err(gpadc->dev, "gpadc_conversion: read high data failed\n");
		goto out;
	}

	/*                                        */
	if ((channel == BAT_CTRL_AND_IBAT) ||
			(channel == VBAT_MEAS_AND_IBAT) ||
			(channel == VBAT_TRUE_MEAS_AND_IBAT) ||
			(channel == BAT_TEMP_AND_IBAT)) {

		if (conv_type == ADC_HW) {
			/*               */
			ret = -ENOTSUPP;
			dev_err(gpadc->dev,
				"gpadc_conversion: only SW double conversion supported\n");
			goto out;
		} else {
			/*                               */
			ret = abx500_get_register_interruptible(gpadc->dev,
				AB8500_GPADC, AB8540_GPADC_MANDATA2L_REG,
				&low_data2);
			if (ret < 0) {
				dev_err(gpadc->dev,
					"gpadc_conversion: read sw low data 2 failed\n");
				goto out;
			}

			ret = abx500_get_register_interruptible(gpadc->dev,
				AB8500_GPADC, AB8540_GPADC_MANDATA2H_REG,
				&high_data2);
			if (ret < 0) {
				dev_err(gpadc->dev,
					"gpadc_conversion: read sw high data 2 failed\n");
				goto out;
			}
			if (ibat != NULL) {
				*ibat = (high_data2 << 8) | low_data2;
			} else {
				dev_warn(gpadc->dev,
					"gpadc_conversion: ibat not stored\n");
			}

		}
	}

	/*               */
	ret = abx500_set_register_interruptible(gpadc->dev, AB8500_GPADC,
		AB8500_GPADC_CTRL1_REG, DIS_GPADC);
	if (ret < 0) {
		dev_err(gpadc->dev, "gpadc_conversion: disable gpadc failed\n");
		goto out;
	}

	/*                                               */
	pm_runtime_mark_last_busy(gpadc->dev);
	pm_runtime_put_autosuspend(gpadc->dev);

	mutex_unlock(&gpadc->ab8500_gpadc_lock);

	return (high_data << 8) | low_data;

out:
	/*
                                                                       
                                                                        
                                                                      
                                                                     
  */
	(void) abx500_set_register_interruptible(gpadc->dev, AB8500_GPADC,
		AB8500_GPADC_CTRL1_REG, DIS_GPADC);
	pm_runtime_put(gpadc->dev);
	mutex_unlock(&gpadc->ab8500_gpadc_lock);
	dev_err(gpadc->dev,
		"gpadc_conversion: Failed to AD convert channel %d\n", channel);
	return ret;
}
EXPORT_SYMBOL(ab8500_gpadc_read_raw);

/* 
                                                                         
                   
                                                       
  
                                                                       
                                                                         
                                  
                                  
 */
static irqreturn_t ab8500_bm_gpadcconvend_handler(int irq, void *_gpadc)
{
	struct ab8500_gpadc *gpadc = _gpadc;

	complete(&gpadc->ab8500_gpadc_complete);

	return IRQ_HANDLED;
}

static int otp_cal_regs[] = {
	AB8500_GPADC_CAL_1,
	AB8500_GPADC_CAL_2,
	AB8500_GPADC_CAL_3,
	AB8500_GPADC_CAL_4,
	AB8500_GPADC_CAL_5,
	AB8500_GPADC_CAL_6,
	AB8500_GPADC_CAL_7,
};

static int otp4_cal_regs[] = {
	AB8540_GPADC_OTP4_REG_7,
	AB8540_GPADC_OTP4_REG_6,
	AB8540_GPADC_OTP4_REG_5,
};

static void ab8500_gpadc_read_calibration_data(struct ab8500_gpadc *gpadc)
{
	int i;
	int ret[ARRAY_SIZE(otp_cal_regs)];
	u8 gpadc_cal[ARRAY_SIZE(otp_cal_regs)];
	int ret_otp4[ARRAY_SIZE(otp4_cal_regs)];
	u8 gpadc_otp4[ARRAY_SIZE(otp4_cal_regs)];
	int vmain_high, vmain_low;
	int btemp_high, btemp_low;
	int vbat_high, vbat_low;
	int ibat_high, ibat_low;
	s64 V_gain, V_offset, V2A_gain, V2A_offset;
	struct ab8500 *ab8500;

	ab8500 = gpadc->parent;

	/*                                                          */
	for (i = 0; i < ARRAY_SIZE(otp_cal_regs); i++) {
		ret[i] = abx500_get_register_interruptible(gpadc->dev,
			AB8500_OTP_EMUL, otp_cal_regs[i],  &gpadc_cal[i]);
		if (ret[i] < 0)
			dev_err(gpadc->dev, "%s: read otp reg 0x%02x failed\n",
				__func__, otp_cal_regs[i]);
	}

	/*
                                                        
                                                                   
                                               
   
                           
                           
                          
   
                        
                                                        
                                                                    
                             
                                                                    
                                                         
                                                                    
                                                                   
                                                                    
                                                                   
                                                                    
                                                                   
                                                                    
                                                                   
                                                                    
                                                     
                                                                    
   
                   
        
                                                        
                                                                    
     
                                                                    
                                                                   
                                                                    
                                                                   
                                                                    
                                                                   
                                                                    
                                                                   
                                                                    
                                                                   
                                                                    
                                                     
                                                                    
   
                   
        
                                                        
                                                                    
                                    
                                                                    
                                                                   
                                                                    
                                             
   
   
                                                                   
                            
   
                                                    
                                                   
                                                    
                                                   
                                                    
                                                   
  */

	if (is_ab8540(ab8500)) {
		/*                                                           */
		if (!(ret[1] < 0 || ret[2] < 0)) {
			vmain_high = (((gpadc_cal[1] & 0xFF) << 2) |
				((gpadc_cal[2] & 0xC0) >> 6));
			vmain_low = ((gpadc_cal[2] & 0x3E) >> 1);

			gpadc->cal_data[ADC_INPUT_VMAIN].otp_calib_hi =
				(u16)vmain_high;
			gpadc->cal_data[ADC_INPUT_VMAIN].otp_calib_lo =
				(u16)vmain_low;

			gpadc->cal_data[ADC_INPUT_VMAIN].gain = CALIB_SCALE *
				(19500 - 315) / (vmain_high - vmain_low);
			gpadc->cal_data[ADC_INPUT_VMAIN].offset = CALIB_SCALE *
				19500 - (CALIB_SCALE * (19500 - 315) /
				(vmain_high - vmain_low)) * vmain_high;
		} else {
		gpadc->cal_data[ADC_INPUT_VMAIN].gain = 0;
		}

		/*                            */
		for (i = 0; i < ARRAY_SIZE(otp4_cal_regs); i++) {
			ret_otp4[i] = abx500_get_register_interruptible(
					gpadc->dev, AB8500_OTP_EMUL,
					otp4_cal_regs[i],  &gpadc_otp4[i]);
			if (ret_otp4[i] < 0)
				dev_err(gpadc->dev,
					"%s: read otp4 reg 0x%02x failed\n",
					__func__, otp4_cal_regs[i]);
		}

		/*                                                           */
		if (!(ret_otp4[0] < 0 || ret_otp4[1] < 0 || ret_otp4[2] < 0)) {
			ibat_high = (((gpadc_otp4[0] & 0x07) << 7) |
				((gpadc_otp4[1] & 0xFE) >> 1));
			ibat_low = (((gpadc_otp4[1] & 0x01) << 5) |
				((gpadc_otp4[2] & 0xF8) >> 3));

			gpadc->cal_data[ADC_INPUT_IBAT].otp_calib_hi =
				(u16)ibat_high;
			gpadc->cal_data[ADC_INPUT_IBAT].otp_calib_lo =
				(u16)ibat_low;

			V_gain = ((IBAT_VDROP_H - IBAT_VDROP_L)
				<< CALIB_SHIFT_IBAT) / (ibat_high - ibat_low);

			V_offset = (IBAT_VDROP_H << CALIB_SHIFT_IBAT) -
				(((IBAT_VDROP_H - IBAT_VDROP_L) <<
				CALIB_SHIFT_IBAT) / (ibat_high - ibat_low))
				* ibat_high;
			/*
                                                   
                                                    
    */
			V2A_gain = (ADC_CH_IBAT_MAX - ADC_CH_IBAT_MIN)/
				(ADC_CH_IBAT_MAX_V - ADC_CH_IBAT_MIN_V);
			V2A_offset = ((ADC_CH_IBAT_MAX_V * ADC_CH_IBAT_MIN -
				ADC_CH_IBAT_MAX * ADC_CH_IBAT_MIN_V)
				<< CALIB_SHIFT_IBAT)
				/ (ADC_CH_IBAT_MAX_V - ADC_CH_IBAT_MIN_V);

			gpadc->cal_data[ADC_INPUT_IBAT].gain = V_gain * V2A_gain;
			gpadc->cal_data[ADC_INPUT_IBAT].offset = V_offset *
				V2A_gain + V2A_offset;
		} else {
			gpadc->cal_data[ADC_INPUT_IBAT].gain = 0;
		}

		dev_dbg(gpadc->dev, "IBAT gain %llu offset %llu\n",
			gpadc->cal_data[ADC_INPUT_IBAT].gain,
			gpadc->cal_data[ADC_INPUT_IBAT].offset);
	} else {
		/*                                                            */
		if (!(ret[0] < 0 || ret[1] < 0 || ret[2] < 0)) {
			vmain_high = (((gpadc_cal[0] & 0x03) << 8) |
				((gpadc_cal[1] & 0x3F) << 2) |
				((gpadc_cal[2] & 0xC0) >> 6));
			vmain_low = ((gpadc_cal[2] & 0x3E) >> 1);

			gpadc->cal_data[ADC_INPUT_VMAIN].otp_calib_hi =
				(u16)vmain_high;
			gpadc->cal_data[ADC_INPUT_VMAIN].otp_calib_lo =
				(u16)vmain_low;

			gpadc->cal_data[ADC_INPUT_VMAIN].gain = CALIB_SCALE *
				(19500 - 315) / (vmain_high - vmain_low);

			gpadc->cal_data[ADC_INPUT_VMAIN].offset = CALIB_SCALE *
				19500 - (CALIB_SCALE * (19500 - 315) /
				(vmain_high - vmain_low)) * vmain_high;
		} else {
			gpadc->cal_data[ADC_INPUT_VMAIN].gain = 0;
		}
	}

	/*                                                            */
	if (!(ret[2] < 0 || ret[3] < 0 || ret[4] < 0)) {
		btemp_high = (((gpadc_cal[2] & 0x01) << 9) |
			(gpadc_cal[3] << 1) | ((gpadc_cal[4] & 0x80) >> 7));
		btemp_low = ((gpadc_cal[4] & 0x7C) >> 2);

		gpadc->cal_data[ADC_INPUT_BTEMP].otp_calib_hi = (u16)btemp_high;
		gpadc->cal_data[ADC_INPUT_BTEMP].otp_calib_lo = (u16)btemp_low;

		gpadc->cal_data[ADC_INPUT_BTEMP].gain =
			CALIB_SCALE * (1300 - 21) / (btemp_high - btemp_low);
		gpadc->cal_data[ADC_INPUT_BTEMP].offset = CALIB_SCALE * 1300 -
			(CALIB_SCALE * (1300 - 21) / (btemp_high - btemp_low))
			* btemp_high;
	} else {
		gpadc->cal_data[ADC_INPUT_BTEMP].gain = 0;
	}

	/*                                                           */
	if (!(ret[4] < 0 || ret[5] < 0 || ret[6] < 0)) {
		vbat_high = (((gpadc_cal[4] & 0x03) << 8) | gpadc_cal[5]);
		vbat_low = ((gpadc_cal[6] & 0xFC) >> 2);

		gpadc->cal_data[ADC_INPUT_VBAT].otp_calib_hi = (u16)vbat_high;
		gpadc->cal_data[ADC_INPUT_VBAT].otp_calib_lo = (u16)vbat_low;

		gpadc->cal_data[ADC_INPUT_VBAT].gain = CALIB_SCALE *
			(4700 - 2380) /	(vbat_high - vbat_low);
		gpadc->cal_data[ADC_INPUT_VBAT].offset = CALIB_SCALE * 4700 -
			(CALIB_SCALE * (4700 - 2380) /
			(vbat_high - vbat_low)) * vbat_high;
	} else {
		gpadc->cal_data[ADC_INPUT_VBAT].gain = 0;
	}

	dev_dbg(gpadc->dev, "VMAIN gain %llu offset %llu\n",
		gpadc->cal_data[ADC_INPUT_VMAIN].gain,
		gpadc->cal_data[ADC_INPUT_VMAIN].offset);

	dev_dbg(gpadc->dev, "BTEMP gain %llu offset %llu\n",
		gpadc->cal_data[ADC_INPUT_BTEMP].gain,
		gpadc->cal_data[ADC_INPUT_BTEMP].offset);

	dev_dbg(gpadc->dev, "VBAT gain %llu offset %llu\n",
		gpadc->cal_data[ADC_INPUT_VBAT].gain,
		gpadc->cal_data[ADC_INPUT_VBAT].offset);
}

static int ab8500_gpadc_runtime_suspend(struct device *dev)
{
	struct ab8500_gpadc *gpadc = dev_get_drvdata(dev);

	regulator_disable(gpadc->regu);
	return 0;
}

static int ab8500_gpadc_runtime_resume(struct device *dev)
{
	struct ab8500_gpadc *gpadc = dev_get_drvdata(dev);
	int ret;

	ret = regulator_enable(gpadc->regu);
	if (ret)
		dev_err(dev, "Failed to enable vtvout LDO: %d\n", ret);
	return ret;
}

static int ab8500_gpadc_runtime_idle(struct device *dev)
{
	pm_runtime_suspend(dev);
	return 0;
}

static int ab8500_gpadc_suspend(struct device *dev)
{
	struct ab8500_gpadc *gpadc = dev_get_drvdata(dev);

	mutex_lock(&gpadc->ab8500_gpadc_lock);

	pm_runtime_get_sync(dev);

	regulator_disable(gpadc->regu);
	return 0;
}

static int ab8500_gpadc_resume(struct device *dev)
{
	struct ab8500_gpadc *gpadc = dev_get_drvdata(dev);
	int ret;

	ret = regulator_enable(gpadc->regu);
	if (ret)
		dev_err(dev, "Failed to enable vtvout LDO: %d\n", ret);

	pm_runtime_mark_last_busy(gpadc->dev);
	pm_runtime_put_autosuspend(gpadc->dev);

	mutex_unlock(&gpadc->ab8500_gpadc_lock);
	return ret;
}

static int ab8500_gpadc_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct ab8500_gpadc *gpadc;

	gpadc = kzalloc(sizeof(struct ab8500_gpadc), GFP_KERNEL);
	if (!gpadc) {
		dev_err(&pdev->dev, "Error: No memory\n");
		return -ENOMEM;
	}

	gpadc->irq_sw = platform_get_irq_byname(pdev, "SW_CONV_END");
	if (gpadc->irq_sw < 0)
		dev_err(gpadc->dev, "failed to get platform sw_conv_end irq\n");

	gpadc->irq_hw = platform_get_irq_byname(pdev, "HW_CONV_END");
	if (gpadc->irq_hw < 0)
		dev_err(gpadc->dev, "failed to get platform hw_conv_end irq\n");

	gpadc->dev = &pdev->dev;
	gpadc->parent = dev_get_drvdata(pdev->dev.parent);
	mutex_init(&gpadc->ab8500_gpadc_lock);

	/*                                                               */
	init_completion(&gpadc->ab8500_gpadc_complete);

	/*                     */
	if (gpadc->irq_sw >= 0) {
		ret = request_threaded_irq(gpadc->irq_sw, NULL,
			ab8500_bm_gpadcconvend_handler,
			IRQF_NO_SUSPEND | IRQF_SHARED, "ab8500-gpadc-sw",
			gpadc);
		if (ret < 0) {
			dev_err(gpadc->dev,
				"Failed to register interrupt irq: %d\n",
				gpadc->irq_sw);
			goto fail;
		}
	}

	if (gpadc->irq_hw >= 0) {
		ret = request_threaded_irq(gpadc->irq_hw, NULL,
			ab8500_bm_gpadcconvend_handler,
			IRQF_NO_SUSPEND | IRQF_SHARED, "ab8500-gpadc-hw",
			gpadc);
		if (ret < 0) {
			dev_err(gpadc->dev,
				"Failed to register interrupt irq: %d\n",
				gpadc->irq_hw);
			goto fail_irq;
		}
	}

	/*                                          */
	gpadc->regu = devm_regulator_get(&pdev->dev, "vddadc");
	if (IS_ERR(gpadc->regu)) {
		ret = PTR_ERR(gpadc->regu);
		dev_err(gpadc->dev, "failed to get vtvout LDO\n");
		goto fail_irq;
	}

	platform_set_drvdata(pdev, gpadc);

	ret = regulator_enable(gpadc->regu);
	if (ret) {
		dev_err(gpadc->dev, "Failed to enable vtvout LDO: %d\n", ret);
		goto fail_enable;
	}

	pm_runtime_set_autosuspend_delay(gpadc->dev, GPADC_AUDOSUSPEND_DELAY);
	pm_runtime_use_autosuspend(gpadc->dev);
	pm_runtime_set_active(gpadc->dev);
	pm_runtime_enable(gpadc->dev);

	ab8500_gpadc_read_calibration_data(gpadc);
	list_add_tail(&gpadc->node, &ab8500_gpadc_list);
	dev_dbg(gpadc->dev, "probe success\n");

	return 0;

fail_enable:
fail_irq:
	free_irq(gpadc->irq_sw, gpadc);
	free_irq(gpadc->irq_hw, gpadc);
fail:
	kfree(gpadc);
	gpadc = NULL;
	return ret;
}

static int ab8500_gpadc_remove(struct platform_device *pdev)
{
	struct ab8500_gpadc *gpadc = platform_get_drvdata(pdev);

	/*                                       */
	list_del(&gpadc->node);
	/*                                                     */
	if (gpadc->irq_sw >= 0)
		free_irq(gpadc->irq_sw, gpadc);
	if (gpadc->irq_hw >= 0)
		free_irq(gpadc->irq_hw, gpadc);

	pm_runtime_get_sync(gpadc->dev);
	pm_runtime_disable(gpadc->dev);

	regulator_disable(gpadc->regu);

	pm_runtime_set_suspended(gpadc->dev);

	pm_runtime_put_noidle(gpadc->dev);

	kfree(gpadc);
	gpadc = NULL;
	return 0;
}

static const struct dev_pm_ops ab8500_gpadc_pm_ops = {
	SET_RUNTIME_PM_OPS(ab8500_gpadc_runtime_suspend,
			   ab8500_gpadc_runtime_resume,
			   ab8500_gpadc_runtime_idle)
	SET_SYSTEM_SLEEP_PM_OPS(ab8500_gpadc_suspend,
				ab8500_gpadc_resume)

};

static struct platform_driver ab8500_gpadc_driver = {
	.probe = ab8500_gpadc_probe,
	.remove = ab8500_gpadc_remove,
	.driver = {
		.name = "ab8500-gpadc",
		.owner = THIS_MODULE,
		.pm = &ab8500_gpadc_pm_ops,
	},
};

static int __init ab8500_gpadc_init(void)
{
	return platform_driver_register(&ab8500_gpadc_driver);
}

static void __exit ab8500_gpadc_exit(void)
{
	platform_driver_unregister(&ab8500_gpadc_driver);
}

/* 
                                              
  
 */
void ab8540_gpadc_get_otp(struct ab8500_gpadc *gpadc,
			u16 *vmain_l, u16 *vmain_h, u16 *btemp_l, u16 *btemp_h,
			u16 *vbat_l, u16 *vbat_h, u16 *ibat_l, u16 *ibat_h)
{
	*vmain_l = gpadc->cal_data[ADC_INPUT_VMAIN].otp_calib_lo;
	*vmain_h = gpadc->cal_data[ADC_INPUT_VMAIN].otp_calib_hi;
	*btemp_l = gpadc->cal_data[ADC_INPUT_BTEMP].otp_calib_lo;
	*btemp_h = gpadc->cal_data[ADC_INPUT_BTEMP].otp_calib_hi;
	*vbat_l = gpadc->cal_data[ADC_INPUT_VBAT].otp_calib_lo;
	*vbat_h = gpadc->cal_data[ADC_INPUT_VBAT].otp_calib_hi;
	*ibat_l = gpadc->cal_data[ADC_INPUT_IBAT].otp_calib_lo;
	*ibat_h = gpadc->cal_data[ADC_INPUT_IBAT].otp_calib_hi;
	return ;
}

subsys_initcall_sync(ab8500_gpadc_init);
module_exit(ab8500_gpadc_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Arun R Murthy, Daniel Willerud, Johan Palsson,"
		"M'boumba Cedric Madianga");
MODULE_ALIAS("platform:ab8500_gpadc");
MODULE_DESCRIPTION("AB8500 GPADC driver");
