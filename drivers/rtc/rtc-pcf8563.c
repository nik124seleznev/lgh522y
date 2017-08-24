/*
 * An I2C driver for the Philips PCF8563 RTC
 * Copyright 2005-06 Tower Technologies
 *
 * Author: Alessandro Zummo <a.zummo@towertech.it>
 * Maintainers: http://www.nslu2-linux.org/
 *
 * based on the other drivers in this same directory.
 *
 * http://www.semiconductors.philips.com/acrobat/datasheets/PCF8563-04.pdf
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/i2c.h>
#include <linux/bcd.h>
#include <linux/rtc.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/of.h>

#define DRV_VERSION "0.4.3"

#define PCF8563_REG_ST1		0x00 /*        */
#define PCF8563_REG_ST2		0x01

#define PCF8563_REG_SC		0x02 /*          */
#define PCF8563_REG_MN		0x03
#define PCF8563_REG_HR		0x04
#define PCF8563_REG_DM		0x05
#define PCF8563_REG_DW		0x06
#define PCF8563_REG_MO		0x07
#define PCF8563_REG_YR		0x08

#define PCF8563_REG_AMN		0x09 /*       */
#define PCF8563_REG_AHR		0x0A
#define PCF8563_REG_ADM		0x0B
#define PCF8563_REG_ADW		0x0C

#define PCF8563_REG_CLKO	0x0D /*           */
#define PCF8563_REG_TMRC	0x0E /*               */
#define PCF8563_REG_TMR		0x0F /*       */

#define PCF8563_SC_LV		0x80 /*             */
#define PCF8563_MO_C		0x80 /*         */

static struct i2c_driver pcf8563_driver;

struct pcf8563 {
	struct rtc_device *rtc;
	/*
                                                    
                                                              
                                    
                                     
                                     
                                                        
                                                              
                                                             
                                                  
                                                               
                                                               
                
  */
	int c_polarity;	/*                                                   */
	int voltage_low; /*                                         */
};

/*
                                                                       
                                                               
 */
static int pcf8563_get_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	struct pcf8563 *pcf8563 = i2c_get_clientdata(client);
	unsigned char buf[13] = { PCF8563_REG_ST1 };

	struct i2c_msg msgs[] = {
		{/*                */
			.addr = client->addr,
			.len = 1,
			.buf = buf
		},
		{/*                    */
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = 13,
			.buf = buf
		},
	};

	/*                */
	if ((i2c_transfer(client->adapter, msgs, 2)) != 2) {
		dev_err(&client->dev, "%s: read error\n", __func__);
		return -EIO;
	}

	if (buf[PCF8563_REG_SC] & PCF8563_SC_LV) {
		pcf8563->voltage_low = 1;
		dev_info(&client->dev,
			"low voltage detected, date/time is not reliable.\n");
	}

	dev_dbg(&client->dev,
		"%s: raw data is st1=%02x, st2=%02x, sec=%02x, min=%02x, hr=%02x, "
		"mday=%02x, wday=%02x, mon=%02x, year=%02x\n",
		__func__,
		buf[0], buf[1], buf[2], buf[3],
		buf[4], buf[5], buf[6], buf[7],
		buf[8]);


	tm->tm_sec = bcd2bin(buf[PCF8563_REG_SC] & 0x7F);
	tm->tm_min = bcd2bin(buf[PCF8563_REG_MN] & 0x7F);
	tm->tm_hour = bcd2bin(buf[PCF8563_REG_HR] & 0x3F); /*             */
	tm->tm_mday = bcd2bin(buf[PCF8563_REG_DM] & 0x3F);
	tm->tm_wday = buf[PCF8563_REG_DW] & 0x07;
	tm->tm_mon = bcd2bin(buf[PCF8563_REG_MO] & 0x1F) - 1; /*             */
	tm->tm_year = bcd2bin(buf[PCF8563_REG_YR]);
	if (tm->tm_year < 70)
		tm->tm_year += 100;	/*                              */
	/*                                                    */
	pcf8563->c_polarity = (buf[PCF8563_REG_MO] & PCF8563_MO_C) ?
		(tm->tm_year >= 100) : (tm->tm_year < 100);

	dev_dbg(&client->dev, "%s: tm is secs=%d, mins=%d, hours=%d, "
		"mday=%d, mon=%d, year=%d, wday=%d\n",
		__func__,
		tm->tm_sec, tm->tm_min, tm->tm_hour,
		tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_wday);

	/*                                                              
                                                                    
  */
	if (rtc_valid_tm(tm) < 0)
		dev_err(&client->dev, "retrieved date/time is not valid.\n");

	return 0;
}

static int pcf8563_set_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	struct pcf8563 *pcf8563 = i2c_get_clientdata(client);
	int i, err;
	unsigned char buf[9];

	dev_dbg(&client->dev, "%s: secs=%d, mins=%d, hours=%d, "
		"mday=%d, mon=%d, year=%d, wday=%d\n",
		__func__,
		tm->tm_sec, tm->tm_min, tm->tm_hour,
		tm->tm_mday, tm->tm_mon, tm->tm_year, tm->tm_wday);

	/*                            */
	buf[PCF8563_REG_SC] = bin2bcd(tm->tm_sec);
	buf[PCF8563_REG_MN] = bin2bcd(tm->tm_min);
	buf[PCF8563_REG_HR] = bin2bcd(tm->tm_hour);

	buf[PCF8563_REG_DM] = bin2bcd(tm->tm_mday);

	/*               */
	buf[PCF8563_REG_MO] = bin2bcd(tm->tm_mon + 1);

	/*                  */
	buf[PCF8563_REG_YR] = bin2bcd(tm->tm_year % 100);
	if (pcf8563->c_polarity ? (tm->tm_year >= 100) : (tm->tm_year < 100))
		buf[PCF8563_REG_MO] |= PCF8563_MO_C;

	buf[PCF8563_REG_DW] = tm->tm_wday & 0x07;

	/*                       */
	for (i = 0; i < 7; i++) {
		unsigned char data[2] = { PCF8563_REG_SC + i,
						buf[PCF8563_REG_SC + i] };

		err = i2c_master_send(client, data, sizeof(data));
		if (err != sizeof(data)) {
			dev_err(&client->dev,
				"%s: err=%d addr=%02x, data=%02x\n",
				__func__, err, data[0], data[1]);
			return -EIO;
		}
	}

	return 0;
}

#ifdef CONFIG_RTC_INTF_DEV
static int pcf8563_rtc_ioctl(struct device *dev, unsigned int cmd, unsigned long arg)
{
	struct pcf8563 *pcf8563 = i2c_get_clientdata(to_i2c_client(dev));
	struct rtc_time tm;

	switch (cmd) {
	case RTC_VL_READ:
		if (pcf8563->voltage_low)
			dev_info(dev, "low voltage detected, date/time is not reliable.\n");

		if (copy_to_user((void __user *)arg, &pcf8563->voltage_low,
					sizeof(int)))
			return -EFAULT;
		return 0;
	case RTC_VL_CLR:
		/*
                                                     
                                                   
                                                  
                                                      
                            
   */
		if (pcf8563_get_datetime(to_i2c_client(dev), &tm))
			pcf8563_set_datetime(to_i2c_client(dev), &tm);

		/*                         */
		pcf8563->voltage_low = 0;

		return 0;
	default:
		return -ENOIOCTLCMD;
	}
}
#else
#define pcf8563_rtc_ioctl NULL
#endif

static int pcf8563_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	return pcf8563_get_datetime(to_i2c_client(dev), tm);
}

static int pcf8563_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	return pcf8563_set_datetime(to_i2c_client(dev), tm);
}

static const struct rtc_class_ops pcf8563_rtc_ops = {
	.ioctl		= pcf8563_rtc_ioctl,
	.read_time	= pcf8563_rtc_read_time,
	.set_time	= pcf8563_rtc_set_time,
};

static int pcf8563_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	struct pcf8563 *pcf8563;

	dev_dbg(&client->dev, "%s\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

	pcf8563 = devm_kzalloc(&client->dev, sizeof(struct pcf8563),
				GFP_KERNEL);
	if (!pcf8563)
		return -ENOMEM;

	dev_info(&client->dev, "chip found, driver version " DRV_VERSION "\n");

	i2c_set_clientdata(client, pcf8563);

	pcf8563->rtc = devm_rtc_device_register(&client->dev,
				pcf8563_driver.driver.name,
				&pcf8563_rtc_ops, THIS_MODULE);

	if (IS_ERR(pcf8563->rtc))
		return PTR_ERR(pcf8563->rtc);

	return 0;
}

static int pcf8563_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id pcf8563_id[] = {
	{ "pcf8563", 0 },
	{ "rtc8564", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, pcf8563_id);

#ifdef CONFIG_OF
static const struct of_device_id pcf8563_of_match[] = {
	{ .compatible = "nxp,pcf8563" },
	{}
};
MODULE_DEVICE_TABLE(of, pcf8563_of_match);
#endif

static struct i2c_driver pcf8563_driver = {
	.driver		= {
		.name	= "rtc-pcf8563",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(pcf8563_of_match),
	},
	.probe		= pcf8563_probe,
	.remove		= pcf8563_remove,
	.id_table	= pcf8563_id,
};

module_i2c_driver(pcf8563_driver);

MODULE_AUTHOR("Alessandro Zummo <a.zummo@towertech.it>");
MODULE_DESCRIPTION("Philips PCF8563/Epson RTC8564 RTC driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
