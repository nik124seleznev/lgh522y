/*
 * Copyright (C) 2012,2013 Infineon Technologies
 *
 * Authors:
 * Peter Huewe <peter.huewe@infineon.com>
 *
 * Device driver for TCG/TCPA TPM (trusted platform module).
 * Specifications at www.trustedcomputinggroup.org
 *
 * This device driver implements the TPM interface as defined in
 * the TCG TPM Interface Spec version 1.2, revision 1.0 and the
 * Infineon I2C Protocol Stack Specification v0.20.
 *
 * It is based on the original tpm_tis device driver from Leendert van
 * Dorn and Kyleen Hall.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 *
 */
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/wait.h>
#include "tpm.h"

/*                                       */
#define TPM_BUFSIZE 1260

/*                                         */
#define MAX_COUNT 3

#define SLEEP_DURATION_LOW 55
#define SLEEP_DURATION_HI 65

/*                                                            
                                                                             
                                                                           
                         
 */
#define MAX_COUNT_LONG 50

#define SLEEP_DURATION_LONG_LOW 200
#define SLEEP_DURATION_LONG_HI 220

/*                                                                         */
#define SLEEP_DURATION_RESET_LOW 2400
#define SLEEP_DURATION_RESET_HI 2600

/*                                                                       */
#define TPM_TIMEOUT_US_LOW (TPM_TIMEOUT * 1000)
#define TPM_TIMEOUT_US_HI  (TPM_TIMEOUT_US_LOW + 2000)

/*                                    */
#define TPM_TIS_I2C_DID_VID_9635 0xd1150b00L
#define TPM_TIS_I2C_DID_VID_9645 0x001a15d1L

enum i2c_chip_type {
	SLB9635,
	SLB9645,
	UNKNOWN,
};

/*                                           */
struct tpm_inf_dev {
	struct i2c_client *client;
	u8 buf[TPM_BUFSIZE + sizeof(u8)]; /*                         */
	struct tpm_chip *chip;
	enum i2c_chip_type chip_type;
};

static struct tpm_inf_dev tpm_dev;
static struct i2c_driver tpm_tis_i2c_driver;

/*
                                          
                                       
                              
                                
  
                                                     
                                                                        
  
                                                            
                             
  
                                                                     
                                                                     
                                                                 
                                                      
  
                                      
 */
static int iic_tpm_read(u8 addr, u8 *buffer, size_t len)
{

	struct i2c_msg msg1 = {
		.addr = tpm_dev.client->addr,
		.len = 1,
		.buf = &addr
	};
	struct i2c_msg msg2 = {
		.addr = tpm_dev.client->addr,
		.flags = I2C_M_RD,
		.len = len,
		.buf = buffer
	};
	struct i2c_msg msgs[] = {msg1, msg2};

	int rc = 0;
	int count;

	/*                                                          */
	if (!tpm_dev.client->adapter->algo->master_xfer)
		return -EOPNOTSUPP;
	i2c_lock_adapter(tpm_dev.client->adapter);

	if (tpm_dev.chip_type == SLB9645) {
		/*                                    
                                                              
                                    
                                                               
                         
   */
		for (count = 0; count < MAX_COUNT; count++) {
			rc = __i2c_transfer(tpm_dev.client->adapter, msgs, 2);
			if (rc > 0)
				break;	/*                          */
			usleep_range(SLEEP_DURATION_LOW, SLEEP_DURATION_HI);
		}
	} else {
		/*                                           */
		for (count = 0; count < MAX_COUNT; count++) {
			rc = __i2c_transfer(tpm_dev.client->adapter, &msg1, 1);
			if (rc > 0)
				break;	/*                          */

			usleep_range(SLEEP_DURATION_LOW, SLEEP_DURATION_HI);
		}

		if (rc <= 0)
			goto out;

		/*                                                             
                                                               
                        
   */
		for (count = 0; count < MAX_COUNT; count++) {
			usleep_range(SLEEP_DURATION_LOW, SLEEP_DURATION_HI);
			rc = __i2c_transfer(tpm_dev.client->adapter, &msg2, 1);
			if (rc > 0)
				break;
		}
	}

out:
	i2c_unlock_adapter(tpm_dev.client->adapter);
	/*                           */
	usleep_range(SLEEP_DURATION_LOW, SLEEP_DURATION_HI);

	/*                                                              
             
                                                                   
  */
	if (rc <= 0)
		return -EIO;

	return 0;
}

static int iic_tpm_write_generic(u8 addr, u8 *buffer, size_t len,
				 unsigned int sleep_low,
				 unsigned int sleep_hi, u8 max_count)
{
	int rc = -EIO;
	int count;

	struct i2c_msg msg1 = {
		.addr = tpm_dev.client->addr,
		.len = len + 1,
		.buf = tpm_dev.buf
	};

	if (len > TPM_BUFSIZE)
		return -EINVAL;

	if (!tpm_dev.client->adapter->algo->master_xfer)
		return -EOPNOTSUPP;
	i2c_lock_adapter(tpm_dev.client->adapter);

	/*                                              */
	tpm_dev.buf[0] = addr;
	memcpy(&(tpm_dev.buf[1]), buffer, len);

	/*
                                                                        
                                                         
                                                    
                                                   
  */
	for (count = 0; count < max_count; count++) {
		rc = __i2c_transfer(tpm_dev.client->adapter, &msg1, 1);
		if (rc > 0)
			break;
		usleep_range(sleep_low, sleep_hi);
	}

	i2c_unlock_adapter(tpm_dev.client->adapter);
	/*                           */
	usleep_range(SLEEP_DURATION_LOW, SLEEP_DURATION_HI);

	/*                                                              
             
                                                                   
  */
	if (rc <= 0)
		return -EIO;

	return 0;
}

/*
                                          
                                      
                                         
                                 
  
                                                               
                                                           
  
                                                            
                             
  
                                                                         
  
                                     
 */
static int iic_tpm_write(u8 addr, u8 *buffer, size_t len)
{
	return iic_tpm_write_generic(addr, buffer, len, SLEEP_DURATION_LOW,
				     SLEEP_DURATION_HI, MAX_COUNT);
}

/*
                                                                     
                    
   */
static int iic_tpm_write_long(u8 addr, u8 *buffer, size_t len)
{
	return iic_tpm_write_generic(addr, buffer, len, SLEEP_DURATION_LONG_LOW,
				     SLEEP_DURATION_LONG_HI, MAX_COUNT_LONG);
}

enum tis_access {
	TPM_ACCESS_VALID = 0x80,
	TPM_ACCESS_ACTIVE_LOCALITY = 0x20,
	TPM_ACCESS_REQUEST_PENDING = 0x04,
	TPM_ACCESS_REQUEST_USE = 0x02,
};

enum tis_status {
	TPM_STS_VALID = 0x80,
	TPM_STS_COMMAND_READY = 0x40,
	TPM_STS_GO = 0x20,
	TPM_STS_DATA_AVAIL = 0x10,
	TPM_STS_DATA_EXPECT = 0x08,
};

enum tis_defaults {
	TIS_SHORT_TIMEOUT = 750,	/*    */
	TIS_LONG_TIMEOUT = 2000,	/*       */
};

#define	TPM_ACCESS(l)			(0x0000 | ((l) << 4))
#define	TPM_STS(l)			(0x0001 | ((l) << 4))
#define	TPM_DATA_FIFO(l)		(0x0005 | ((l) << 4))
#define	TPM_DID_VID(l)			(0x0006 | ((l) << 4))

static int check_locality(struct tpm_chip *chip, int loc)
{
	u8 buf;
	int rc;

	rc = iic_tpm_read(TPM_ACCESS(loc), &buf, 1);
	if (rc < 0)
		return rc;

	if ((buf & (TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID)) ==
	    (TPM_ACCESS_ACTIVE_LOCALITY | TPM_ACCESS_VALID)) {
		chip->vendor.locality = loc;
		return loc;
	}

	return -EIO;
}

/*                                   */
static void release_locality(struct tpm_chip *chip, int loc, int force)
{
	u8 buf;
	if (iic_tpm_read(TPM_ACCESS(loc), &buf, 1) < 0)
		return;

	if (force || (buf & (TPM_ACCESS_REQUEST_PENDING | TPM_ACCESS_VALID)) ==
	    (TPM_ACCESS_REQUEST_PENDING | TPM_ACCESS_VALID)) {
		buf = TPM_ACCESS_ACTIVE_LOCALITY;
		iic_tpm_write(TPM_ACCESS(loc), &buf, 1);
	}
}

static int request_locality(struct tpm_chip *chip, int loc)
{
	unsigned long stop;
	u8 buf = TPM_ACCESS_REQUEST_USE;

	if (check_locality(chip, loc) >= 0)
		return loc;

	iic_tpm_write(TPM_ACCESS(loc), &buf, 1);

	/*                     */
	stop = jiffies + chip->vendor.timeout_a;
	do {
		if (check_locality(chip, loc) >= 0)
			return loc;
		usleep_range(TPM_TIMEOUT_US_LOW, TPM_TIMEOUT_US_HI);
	} while (time_before(jiffies, stop));

	return -ETIME;
}

static u8 tpm_tis_i2c_status(struct tpm_chip *chip)
{
	/*                                                                   */
	u8 buf = 0xFF;
	u8 i = 0;

	do {
		if (iic_tpm_read(TPM_STS(chip->vendor.locality), &buf, 1) < 0)
			return 0;

		i++;
	/*                                            */
	} while ((buf == 0xFF) && i < 10);

	return buf;
}

static void tpm_tis_i2c_ready(struct tpm_chip *chip)
{
	/*                                               */
	u8 buf = TPM_STS_COMMAND_READY;
	iic_tpm_write_long(TPM_STS(chip->vendor.locality), &buf, 1);
}

static ssize_t get_burstcount(struct tpm_chip *chip)
{
	unsigned long stop;
	ssize_t burstcnt;
	u8 buf[3];

	/*                     */
	/*                                                 */
	stop = jiffies + chip->vendor.timeout_d;
	do {
		/*                            */
		if (iic_tpm_read(TPM_STS(chip->vendor.locality)+1, buf, 3) < 0)
			burstcnt = 0;
		else
			burstcnt = (buf[2] << 16) + (buf[1] << 8) + buf[0];

		if (burstcnt)
			return burstcnt;

		usleep_range(TPM_TIMEOUT_US_LOW, TPM_TIMEOUT_US_HI);
	} while (time_before(jiffies, stop));
	return -EBUSY;
}

static int wait_for_stat(struct tpm_chip *chip, u8 mask, unsigned long timeout,
			 int *status)
{
	unsigned long stop;

	/*                      */
	*status = tpm_tis_i2c_status(chip);
	if ((*status != 0xFF) && (*status & mask) == mask)
		return 0;

	stop = jiffies + timeout;
	do {
		/*                                                          */
		usleep_range(TPM_TIMEOUT_US_LOW, TPM_TIMEOUT_US_HI);
		*status = tpm_tis_i2c_status(chip);
		if ((*status & mask) == mask)
			return 0;

	} while (time_before(jiffies, stop));

	return -ETIME;
}

static int recv_data(struct tpm_chip *chip, u8 *buf, size_t count)
{
	size_t size = 0;
	ssize_t burstcnt;
	u8 retries = 0;
	int rc;

	while (size < count) {
		burstcnt = get_burstcount(chip);

		/*                            */
		if (burstcnt < 0)
			return burstcnt;

		/*                                  */
		if (burstcnt > (count - size))
			burstcnt = count - size;

		rc = iic_tpm_read(TPM_DATA_FIFO(chip->vendor.locality),
				  &(buf[size]), burstcnt);
		if (rc == 0)
			size += burstcnt;
		else if (rc < 0)
			retries++;

		/*                                         */
		if (retries > MAX_COUNT_LONG)
			return -EIO;
	}
	return size;
}

static int tpm_tis_i2c_recv(struct tpm_chip *chip, u8 *buf, size_t count)
{
	int size = 0;
	int expected, status;

	if (count < TPM_HEADER_SIZE) {
		size = -EIO;
		goto out;
	}

	/*                                                           */
	size = recv_data(chip, buf, TPM_HEADER_SIZE);
	if (size < TPM_HEADER_SIZE) {
		dev_err(chip->dev, "Unable to read header\n");
		goto out;
	}

	expected = be32_to_cpu(*(__be32 *)(buf + 2));
	if ((size_t) expected > count) {
		size = -EIO;
		goto out;
	}

	size += recv_data(chip, &buf[TPM_HEADER_SIZE],
			  expected - TPM_HEADER_SIZE);
	if (size < expected) {
		dev_err(chip->dev, "Unable to read remainder of result\n");
		size = -ETIME;
		goto out;
	}

	wait_for_stat(chip, TPM_STS_VALID, chip->vendor.timeout_c, &status);
	if (status & TPM_STS_DATA_AVAIL) {	/*        */
		dev_err(chip->dev, "Error left over data\n");
		size = -EIO;
		goto out;
	}

out:
	tpm_tis_i2c_ready(chip);
	/*                                          
                                                
  */
	usleep_range(SLEEP_DURATION_RESET_LOW, SLEEP_DURATION_RESET_HI);
	release_locality(chip, chip->vendor.locality, 0);
	return size;
}

static int tpm_tis_i2c_send(struct tpm_chip *chip, u8 *buf, size_t len)
{
	int rc, status;
	ssize_t burstcnt;
	size_t count = 0;
	u8 retries = 0;
	u8 sts = TPM_STS_GO;

	if (len > TPM_BUFSIZE)
		return -E2BIG;	/*                                        */

	if (request_locality(chip, 0) < 0)
		return -EBUSY;

	status = tpm_tis_i2c_status(chip);
	if ((status & TPM_STS_COMMAND_READY) == 0) {
		tpm_tis_i2c_ready(chip);
		if (wait_for_stat
		    (chip, TPM_STS_COMMAND_READY,
		     chip->vendor.timeout_b, &status) < 0) {
			rc = -ETIME;
			goto out_err;
		}
	}

	while (count < len - 1) {
		burstcnt = get_burstcount(chip);

		/*                            */
		if (burstcnt < 0)
			return burstcnt;

		if (burstcnt > (len - 1 - count))
			burstcnt = len - 1 - count;

		rc = iic_tpm_write(TPM_DATA_FIFO(chip->vendor.locality),
				   &(buf[count]), burstcnt);
		if (rc == 0)
			count += burstcnt;
		else if (rc < 0)
			retries++;

		/*                                         */
		if (retries > MAX_COUNT_LONG) {
			rc = -EIO;
			goto out_err;
		}

		wait_for_stat(chip, TPM_STS_VALID,
			      chip->vendor.timeout_c, &status);

		if ((status & TPM_STS_DATA_EXPECT) == 0) {
			rc = -EIO;
			goto out_err;
		}
	}

	/*                 */
	iic_tpm_write(TPM_DATA_FIFO(chip->vendor.locality), &(buf[count]), 1);
	wait_for_stat(chip, TPM_STS_VALID, chip->vendor.timeout_c, &status);
	if ((status & TPM_STS_DATA_EXPECT) != 0) {
		rc = -EIO;
		goto out_err;
	}

	/*              */
	iic_tpm_write(TPM_STS(chip->vendor.locality), &sts, 1);

	return len;
out_err:
	tpm_tis_i2c_ready(chip);
	/*                                          
                                                
  */
	usleep_range(SLEEP_DURATION_RESET_LOW, SLEEP_DURATION_RESET_HI);
	release_locality(chip, chip->vendor.locality, 0);
	return rc;
}

static bool tpm_tis_i2c_req_canceled(struct tpm_chip *chip, u8 status)
{
	return (status == TPM_STS_COMMAND_READY);
}

static const struct file_operations tis_ops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.open = tpm_open,
	.read = tpm_read,
	.write = tpm_write,
	.release = tpm_release,
};

static DEVICE_ATTR(pubek, S_IRUGO, tpm_show_pubek, NULL);
static DEVICE_ATTR(pcrs, S_IRUGO, tpm_show_pcrs, NULL);
static DEVICE_ATTR(enabled, S_IRUGO, tpm_show_enabled, NULL);
static DEVICE_ATTR(active, S_IRUGO, tpm_show_active, NULL);
static DEVICE_ATTR(owned, S_IRUGO, tpm_show_owned, NULL);
static DEVICE_ATTR(temp_deactivated, S_IRUGO, tpm_show_temp_deactivated, NULL);
static DEVICE_ATTR(caps, S_IRUGO, tpm_show_caps_1_2, NULL);
static DEVICE_ATTR(cancel, S_IWUSR | S_IWGRP, NULL, tpm_store_cancel);
static DEVICE_ATTR(durations, S_IRUGO, tpm_show_durations, NULL);
static DEVICE_ATTR(timeouts, S_IRUGO, tpm_show_timeouts, NULL);

static struct attribute *tis_attrs[] = {
	&dev_attr_pubek.attr,
	&dev_attr_pcrs.attr,
	&dev_attr_enabled.attr,
	&dev_attr_active.attr,
	&dev_attr_owned.attr,
	&dev_attr_temp_deactivated.attr,
	&dev_attr_caps.attr,
	&dev_attr_cancel.attr,
	&dev_attr_durations.attr,
	&dev_attr_timeouts.attr,
	NULL,
};

static struct attribute_group tis_attr_grp = {
	.attrs = tis_attrs
};

static struct tpm_vendor_specific tpm_tis_i2c = {
	.status = tpm_tis_i2c_status,
	.recv = tpm_tis_i2c_recv,
	.send = tpm_tis_i2c_send,
	.cancel = tpm_tis_i2c_ready,
	.req_complete_mask = TPM_STS_DATA_AVAIL | TPM_STS_VALID,
	.req_complete_val = TPM_STS_DATA_AVAIL | TPM_STS_VALID,
	.req_canceled = tpm_tis_i2c_req_canceled,
	.attr_group = &tis_attr_grp,
	.miscdev.fops = &tis_ops,
};

static int tpm_tis_i2c_init(struct device *dev)
{
	u32 vendor;
	int rc = 0;
	struct tpm_chip *chip;

	chip = tpm_register_hardware(dev, &tpm_tis_i2c);
	if (!chip) {
		dev_err(dev, "could not register hardware\n");
		rc = -ENODEV;
		goto out_err;
	}

	/*                    */
	chip->vendor.irq = 0;

	/*                  */
	chip->vendor.timeout_a = msecs_to_jiffies(TIS_SHORT_TIMEOUT);
	chip->vendor.timeout_b = msecs_to_jiffies(TIS_LONG_TIMEOUT);
	chip->vendor.timeout_c = msecs_to_jiffies(TIS_SHORT_TIMEOUT);
	chip->vendor.timeout_d = msecs_to_jiffies(TIS_SHORT_TIMEOUT);

	if (request_locality(chip, 0) != 0) {
		dev_err(dev, "could not request locality\n");
		rc = -ENODEV;
		goto out_vendor;
	}

	/*                                       */
	if (iic_tpm_read(TPM_DID_VID(0), (u8 *)&vendor, 4) < 0) {
		dev_err(dev, "could not read vendor id\n");
		rc = -EIO;
		goto out_release;
	}

	if (vendor == TPM_TIS_I2C_DID_VID_9645) {
		tpm_dev.chip_type = SLB9645;
	} else if (vendor == TPM_TIS_I2C_DID_VID_9635) {
		tpm_dev.chip_type = SLB9635;
	} else {
		dev_err(dev, "vendor id did not match! ID was %08x\n", vendor);
		rc = -ENODEV;
		goto out_release;
	}

	dev_info(dev, "1.2 TPM (device-id 0x%X)\n", vendor >> 16);

	INIT_LIST_HEAD(&chip->vendor.list);
	tpm_dev.chip = chip;

	tpm_get_timeouts(chip);
	tpm_do_selftest(chip);

	return 0;

out_release:
	release_locality(chip, chip->vendor.locality, 1);

out_vendor:
	/*                    */
	tpm_dev_vendor_release(chip);

	/*                 */
	tpm_remove_hardware(chip->dev);

	/*                                         */
	chip->dev->release = NULL;
	chip->release = NULL;
	tpm_dev.client = NULL;
	dev_set_drvdata(chip->dev, chip);
out_err:
	return rc;
}

static const struct i2c_device_id tpm_tis_i2c_table[] = {
	{"tpm_i2c_infineon", 0},
	{"slb9635tt", 0},
	{"slb9645tt", 1},
	{},
};

MODULE_DEVICE_TABLE(i2c, tpm_tis_i2c_table);

#ifdef CONFIG_OF
static const struct of_device_id tpm_tis_i2c_of_match[] = {
	{
		.name = "tpm_i2c_infineon",
		.type = "tpm",
		.compatible = "infineon,tpm_i2c_infineon",
		.data = (void *)0
	},
	{
		.name = "slb9635tt",
		.type = "tpm",
		.compatible = "infineon,slb9635tt",
		.data = (void *)0
	},
	{
		.name = "slb9645tt",
		.type = "tpm",
		.compatible = "infineon,slb9645tt",
		.data = (void *)1
	},
	{},
};
MODULE_DEVICE_TABLE(of, tpm_tis_i2c_of_match);
#endif

static SIMPLE_DEV_PM_OPS(tpm_tis_i2c_ops, tpm_pm_suspend, tpm_pm_resume);

static int tpm_tis_i2c_probe(struct i2c_client *client,
			     const struct i2c_device_id *id)
{
	int rc;
	struct device *dev = &(client->dev);

	if (tpm_dev.client != NULL) {
		dev_err(dev, "This driver only supports one client at a time\n");
		return -EBUSY;	/*                            */
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(dev, "no algorithms associated to the i2c bus\n");
		return -ENODEV;
	}

	client->driver = &tpm_tis_i2c_driver;
	tpm_dev.client = client;
	rc = tpm_tis_i2c_init(&client->dev);
	if (rc != 0) {
		client->driver = NULL;
		tpm_dev.client = NULL;
		rc = -ENODEV;
	}
	return rc;
}

static int tpm_tis_i2c_remove(struct i2c_client *client)
{
	struct tpm_chip *chip = tpm_dev.chip;
	release_locality(chip, chip->vendor.locality, 1);

	/*                    */
	tpm_dev_vendor_release(chip);

	/*                 */
	tpm_remove_hardware(chip->dev);

	/*                                         */
	chip->dev->release = NULL;
	chip->release = NULL;
	tpm_dev.client = NULL;
	dev_set_drvdata(chip->dev, chip);

	return 0;
}

static struct i2c_driver tpm_tis_i2c_driver = {
	.id_table = tpm_tis_i2c_table,
	.probe = tpm_tis_i2c_probe,
	.remove = tpm_tis_i2c_remove,
	.driver = {
		   .name = "tpm_i2c_infineon",
		   .owner = THIS_MODULE,
		   .pm = &tpm_tis_i2c_ops,
		   .of_match_table = of_match_ptr(tpm_tis_i2c_of_match),
		   },
};

module_i2c_driver(tpm_tis_i2c_driver);
MODULE_AUTHOR("Peter Huewe <peter.huewe@infineon.com>");
MODULE_DESCRIPTION("TPM TIS I2C Infineon Driver");
MODULE_VERSION("2.2.0");
MODULE_LICENSE("GPL");
