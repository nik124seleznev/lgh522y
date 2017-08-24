/*
    i2c-isch.c - Linux kernel driver for Intel SCH chipset SMBus
    - Based on i2c-piix4.c
    Copyright (c) 1998 - 2002 Frodo Looijaard <frodol@dds.nl> and
    Philip Edelbrock <phil@netroedge.com>
    - Intel SCH support
    Copyright (c) 2007 - 2008 Jacob Jun Pan <jacob.jun.pan@intel.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
            
                                                     
                                                                          
*/

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/stddef.h>
#include <linux/ioport.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/acpi.h>

/*                           */
#define SMBHSTCNT	(0 + sch_smba)
#define SMBHSTSTS	(1 + sch_smba)
#define SMBHSTCLK	(2 + sch_smba)
#define SMBHSTADD	(4 + sch_smba) /*     */
#define SMBHSTCMD	(5 + sch_smba)
#define SMBHSTDAT0	(6 + sch_smba)
#define SMBHSTDAT1	(7 + sch_smba)
#define SMBBLKDAT	(0x20 + sch_smba)

/*                */
#define MAX_RETRIES	5000

/*               */
#define SCH_QUICK		0x00
#define SCH_BYTE		0x01
#define SCH_BYTE_DATA		0x02
#define SCH_WORD_DATA		0x03
#define SCH_BLOCK_DATA		0x05

static unsigned short sch_smba;
static struct i2c_adapter sch_adapter;
static int backbone_speed = 33000; /*                       */
module_param(backbone_speed, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(backbone_speed, "Backbone speed in kHz, (default = 33000)");

/*
                                                                           
                                     
                                               
 */
static int sch_transaction(void)
{
	int temp;
	int result = 0;
	int retries = 0;

	dev_dbg(&sch_adapter.dev, "Transaction (pre): CNT=%02x, CMD=%02x, "
		"ADD=%02x, DAT0=%02x, DAT1=%02x\n", inb(SMBHSTCNT),
		inb(SMBHSTCMD), inb(SMBHSTADD), inb(SMBHSTDAT0),
		inb(SMBHSTDAT1));

	/*                                                         */
	temp = inb(SMBHSTSTS) & 0x0f;
	if (temp) {
		/*                                                   */
		if (temp & 0x01) {
			dev_dbg(&sch_adapter.dev, "Completion (%02x). "
				"Clear...\n", temp);
		}
		if (temp & 0x06) {
			dev_dbg(&sch_adapter.dev, "SMBus error (%02x). "
				"Resetting...\n", temp);
		}
		outb(temp, SMBHSTSTS);
		temp = inb(SMBHSTSTS) & 0x0f;
		if (temp) {
			dev_err(&sch_adapter.dev,
				"SMBus is not ready: (%02x)\n", temp);
			return -EAGAIN;
		}
	}

	/*                                        */
	outb(inb(SMBHSTCNT) | 0x10, SMBHSTCNT);

	do {
		usleep_range(100, 200);
		temp = inb(SMBHSTSTS) & 0x0f;
	} while ((temp & 0x08) && (retries++ < MAX_RETRIES));

	/*                                        */
	if (retries > MAX_RETRIES) {
		dev_err(&sch_adapter.dev, "SMBus Timeout!\n");
		result = -ETIMEDOUT;
	}
	if (temp & 0x04) {
		result = -EIO;
		dev_dbg(&sch_adapter.dev, "Bus collision! SMBus may be "
			"locked until next hard reset. (sorry!)\n");
		/*                                                    */
	} else if (temp & 0x02) {
		result = -EIO;
		dev_err(&sch_adapter.dev, "Error: no response!\n");
	} else if (temp & 0x01) {
		dev_dbg(&sch_adapter.dev, "Post complete!\n");
		outb(temp, SMBHSTSTS);
		temp = inb(SMBHSTSTS) & 0x07;
		if (temp & 0x06) {
			/*                         */
			dev_dbg(&sch_adapter.dev, "Failed reset at end of "
				"transaction (%02x), Bus error!\n", temp);
		}
	} else {
		result = -ENXIO;
		dev_dbg(&sch_adapter.dev, "No such address.\n");
	}
	dev_dbg(&sch_adapter.dev, "Transaction (post): CNT=%02x, CMD=%02x, "
		"ADD=%02x, DAT0=%02x, DAT1=%02x\n", inb(SMBHSTCNT),
		inb(SMBHSTCMD), inb(SMBHSTADD), inb(SMBHSTDAT0),
		inb(SMBHSTDAT1));
	return result;
}

/*
                                                   
                                                                              
                                                                             
                                                                         
                                               
 */
static s32 sch_access(struct i2c_adapter *adap, u16 addr,
		 unsigned short flags, char read_write,
		 u8 command, int size, union i2c_smbus_data *data)
{
	int i, len, temp, rc;

	/*                                      */
	temp = inb(SMBHSTSTS) & 0x0f;
	if (temp & 0x08) {
		dev_dbg(&sch_adapter.dev, "SMBus busy (%02x)\n", temp);
		return -EAGAIN;
	}
	temp = inw(SMBHSTCLK);
	if (!temp) {
		/*
                                                         
                                                         
                                                          
                                                 
   */
		dev_notice(&sch_adapter.dev,
			"Clock divider unitialized. Setting defaults\n");
		outw(backbone_speed / (4 * 100), SMBHSTCLK);
	}

	dev_dbg(&sch_adapter.dev, "access size: %d %s\n", size,
		(read_write)?"READ":"WRITE");
	switch (size) {
	case I2C_SMBUS_QUICK:
		outb((addr << 1) | read_write, SMBHSTADD);
		size = SCH_QUICK;
		break;
	case I2C_SMBUS_BYTE:
		outb((addr << 1) | read_write, SMBHSTADD);
		if (read_write == I2C_SMBUS_WRITE)
			outb(command, SMBHSTCMD);
		size = SCH_BYTE;
		break;
	case I2C_SMBUS_BYTE_DATA:
		outb((addr << 1) | read_write, SMBHSTADD);
		outb(command, SMBHSTCMD);
		if (read_write == I2C_SMBUS_WRITE)
			outb(data->byte, SMBHSTDAT0);
		size = SCH_BYTE_DATA;
		break;
	case I2C_SMBUS_WORD_DATA:
		outb((addr << 1) | read_write, SMBHSTADD);
		outb(command, SMBHSTCMD);
		if (read_write == I2C_SMBUS_WRITE) {
			outb(data->word & 0xff, SMBHSTDAT0);
			outb((data->word & 0xff00) >> 8, SMBHSTDAT1);
		}
		size = SCH_WORD_DATA;
		break;
	case I2C_SMBUS_BLOCK_DATA:
		outb((addr << 1) | read_write, SMBHSTADD);
		outb(command, SMBHSTCMD);
		if (read_write == I2C_SMBUS_WRITE) {
			len = data->block[0];
			if (len == 0 || len > I2C_SMBUS_BLOCK_MAX)
				return -EINVAL;
			outb(len, SMBHSTDAT0);
			for (i = 1; i <= len; i++)
				outb(data->block[i], SMBBLKDAT+i-1);
		}
		size = SCH_BLOCK_DATA;
		break;
	default:
		dev_warn(&adap->dev, "Unsupported transaction %d\n", size);
		return -EOPNOTSUPP;
	}
	dev_dbg(&sch_adapter.dev, "write size %d to 0x%04x\n", size, SMBHSTCNT);
	outb((inb(SMBHSTCNT) & 0xb0) | (size & 0x7), SMBHSTCNT);

	rc = sch_transaction();
	if (rc)	/*                      */
		return rc;

	if ((read_write == I2C_SMBUS_WRITE) || (size == SCH_QUICK))
		return 0;

	switch (size) {
	case SCH_BYTE:
	case SCH_BYTE_DATA:
		data->byte = inb(SMBHSTDAT0);
		break;
	case SCH_WORD_DATA:
		data->word = inb(SMBHSTDAT0) + (inb(SMBHSTDAT1) << 8);
		break;
	case SCH_BLOCK_DATA:
		data->block[0] = inb(SMBHSTDAT0);
		if (data->block[0] == 0 || data->block[0] > I2C_SMBUS_BLOCK_MAX)
			return -EPROTO;
		for (i = 1; i <= data->block[0]; i++)
			data->block[i] = inb(SMBBLKDAT+i-1);
		break;
	}
	return 0;
}

static u32 sch_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_SMBUS_QUICK | I2C_FUNC_SMBUS_BYTE |
	    I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA |
	    I2C_FUNC_SMBUS_BLOCK_DATA;
}

static const struct i2c_algorithm smbus_algorithm = {
	.smbus_xfer	= sch_access,
	.functionality	= sch_func,
};

static struct i2c_adapter sch_adapter = {
	.owner		= THIS_MODULE,
	.class		= I2C_CLASS_HWMON | I2C_CLASS_SPD,
	.algo		= &smbus_algorithm,
};

static int smbus_sch_probe(struct platform_device *dev)
{
	struct resource *res;
	int retval;

	res = platform_get_resource(dev, IORESOURCE_IO, 0);
	if (!res)
		return -EBUSY;

	if (!request_region(res->start, resource_size(res), dev->name)) {
		dev_err(&dev->dev, "SMBus region 0x%x already in use!\n",
			sch_smba);
		return -EBUSY;
	}

	sch_smba = res->start;

	dev_dbg(&dev->dev, "SMBA = 0x%X\n", sch_smba);

	/*                                               */
	sch_adapter.dev.parent = &dev->dev;

	snprintf(sch_adapter.name, sizeof(sch_adapter.name),
		"SMBus SCH adapter at %04x", sch_smba);

	retval = i2c_add_adapter(&sch_adapter);
	if (retval) {
		dev_err(&dev->dev, "Couldn't register adapter!\n");
		release_region(res->start, resource_size(res));
		sch_smba = 0;
	}

	return retval;
}

static int smbus_sch_remove(struct platform_device *pdev)
{
	struct resource *res;
	if (sch_smba) {
		i2c_del_adapter(&sch_adapter);
		res = platform_get_resource(pdev, IORESOURCE_IO, 0);
		release_region(res->start, resource_size(res));
		sch_smba = 0;
	}

	return 0;
}

static struct platform_driver smbus_sch_driver = {
	.driver = {
		.name = "isch_smbus",
		.owner = THIS_MODULE,
	},
	.probe		= smbus_sch_probe,
	.remove		= smbus_sch_remove,
};

module_platform_driver(smbus_sch_driver);

MODULE_AUTHOR("Jacob Pan <jacob.jun.pan@intel.com>");
MODULE_DESCRIPTION("Intel SCH SMBus driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:isch_smbus");