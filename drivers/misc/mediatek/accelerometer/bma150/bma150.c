/* BMA255 motion sensor driver
 *
 *
 * This software program is licensed subject to the GNU General Public License
 * (GPL).Version 2,June 1991, available at http://www.fsf.org/copyleft/gpl.html

 * (C) Copyright 2011 Bosch Sensortec GmbH
 * All Rights Reserved
 */

#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>
#include <linux/module.h>

#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>

#define POWER_NONE_MACRO MT65XX_POWER_NONE

#include <cust_acc.h>
#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>
#include "bma150.h"
#include <linux/hwmsen_helper.h>


#define CONFIG_OF_DT


#ifdef CONFIG_OF_DT
static const struct of_device_id gsensor_of_match[] = {
	{ .compatible = "mediatek,gsensor", },
	{},
};
#endif


/*                                                                            */
#define I2C_DRIVERID_BMA255 255
/*                                                                            */
#define DEBUG 1
/*                                                                            */
//                                                                   
#ifdef CONFIG_MACH_LGE
//                                                       
#else	//                  
#define SW_CALIBRATION
#endif

/*                                                                            */
#define BMA255_AXIS_X          0
#define BMA255_AXIS_Y          1
#define BMA255_AXIS_Z          2
#define BMA255_AXES_NUM        3
#define BMA255_DATA_LEN        6
#define BMA255_DEV_NAME        "BMA255"

#define BMA255_MODE_NORMAL      0
#define BMA255_MODE_LOWPOWER    1
#define BMA255_MODE_SUSPEND     2

#define BMA255_ACC_X_LSB__POS           4
#define BMA255_ACC_X_LSB__LEN           4
#define BMA255_ACC_X_LSB__MSK           0xF0
#define BMA255_ACC_X_LSB__REG           BMA255_X_AXIS_LSB_REG

#define BMA255_ACC_X_MSB__POS           0
#define BMA255_ACC_X_MSB__LEN           8
#define BMA255_ACC_X_MSB__MSK           0xFF
#define BMA255_ACC_X_MSB__REG           BMA255_X_AXIS_MSB_REG

#define BMA255_ACC_Y_LSB__POS           4
#define BMA255_ACC_Y_LSB__LEN           4
#define BMA255_ACC_Y_LSB__MSK           0xF0
#define BMA255_ACC_Y_LSB__REG           BMA255_Y_AXIS_LSB_REG

#define BMA255_ACC_Y_MSB__POS           0
#define BMA255_ACC_Y_MSB__LEN           8
#define BMA255_ACC_Y_MSB__MSK           0xFF
#define BMA255_ACC_Y_MSB__REG           BMA255_Y_AXIS_MSB_REG

#define BMA255_ACC_Z_LSB__POS           4
#define BMA255_ACC_Z_LSB__LEN           4
#define BMA255_ACC_Z_LSB__MSK           0xF0
#define BMA255_ACC_Z_LSB__REG           BMA255_Z_AXIS_LSB_REG

#define BMA255_ACC_Z_MSB__POS           0
#define BMA255_ACC_Z_MSB__LEN           8
#define BMA255_ACC_Z_MSB__MSK           0xFF
#define BMA255_ACC_Z_MSB__REG           BMA255_Z_AXIS_MSB_REG

#define BMA255_EN_LOW_POWER__POS          6
#define BMA255_EN_LOW_POWER__LEN          1
#define BMA255_EN_LOW_POWER__MSK          0x40
#define BMA255_EN_LOW_POWER__REG          BMA255_REG_POWER_CTL

#define BMA255_EN_SUSPEND__POS            7
#define BMA255_EN_SUSPEND__LEN            1
#define BMA255_EN_SUSPEND__MSK            0x80
#define BMA255_EN_SUSPEND__REG            BMA255_REG_POWER_CTL

#define BMA255_RANGE_SEL__POS             0
#define BMA255_RANGE_SEL__LEN             4
#define BMA255_RANGE_SEL__MSK             0x0F
#define BMA255_RANGE_SEL__REG             BMA255_REG_DATA_FORMAT

#define BMA255_BANDWIDTH__POS             0
#define BMA255_BANDWIDTH__LEN             5
#define BMA255_BANDWIDTH__MSK             0x1F
#define BMA255_BANDWIDTH__REG             BMA255_REG_BW_RATE

#ifdef CONFIG_MACH_LGE
#define BMA255_ACCEL_CALIBRATION

#ifdef BMA255_ACCEL_CALIBRATION
#define BMA255_SHAKING_DETECT_THRESHOLD	(300)	 //                     
#endif

struct bma255acc{
	s16	x,
		y,
		z;
} ;
#endif

#define BMA255_GET_BITSLICE(regvar, bitname)\
	((regvar & bitname##__MSK) >> bitname##__POS)

#define BMA255_SET_BITSLICE(regvar, bitname, val)\
	((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

/*                                                                            */

/*                                                                            */
static const struct i2c_device_id bma255_i2c_id[] = {{BMA255_DEV_NAME,0},{}};
static struct i2c_board_info __initdata bma255_i2c_info ={ I2C_BOARD_INFO(BMA255_DEV_NAME, BMA255_I2C_ADDR)};

/*                                                                            */
static int bma255_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int bma255_i2c_remove(struct i2c_client *client);

/*                                                                            */
typedef enum {
    BMA_TRC_FILTER  = 0x01,
    BMA_TRC_RAWDATA = 0x02,
    BMA_TRC_IOCTL   = 0x04,
    BMA_TRC_CALI	= 0X08,
    BMA_TRC_INFO	= 0X10,
} BMA_TRC;
/*                                                                            */
struct scale_factor{
    u8  whole;
    u8  fraction;
};
/*                                                                            */
struct data_resolution {
    struct scale_factor scalefactor;
    int                 sensitivity;
};
/*                                                                            */
#define C_MAX_FIR_LENGTH (32)
/*                                                                            */
struct data_filter {
    s16 raw[C_MAX_FIR_LENGTH][BMA255_AXES_NUM];
    int sum[BMA255_AXES_NUM];
    int num;
    int idx;
};
/*                                                                            */
struct bma255_i2c_data {
    struct i2c_client *client;
    struct acc_hw *hw;
    struct hwmsen_convert   cvt;

    /*    */
    struct data_resolution *reso;
    atomic_t                trace;
    atomic_t                suspend;
    atomic_t                selftest;
	atomic_t				filter;
    s16                     cali_sw[BMA255_AXES_NUM+1];

    /*    */
    s8                      offset[BMA255_AXES_NUM+1];  /*                        */
    s16                     data[BMA255_AXES_NUM+1];

#if defined(CONFIG_BMA255_LOWPASS)
    atomic_t                firlen;
    atomic_t                fir_en;
    struct data_filter      fir;
#endif
    /*             */
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend    early_drv;
#endif

#ifdef CONFIG_MACH_LGE
	atomic_t fast_calib_x_rslt;
	atomic_t fast_calib_y_rslt;
	atomic_t fast_calib_z_rslt;
	atomic_t fast_calib_rslt;
#endif
};
/*                                                                            */
static struct i2c_driver bma255_i2c_driver = {
    .driver = {
        .name           = BMA255_DEV_NAME,
    },
	.probe      		= bma255_i2c_probe,
	.remove    			= bma255_i2c_remove,
#if !defined(CONFIG_HAS_EARLYSUSPEND)
    .suspend            = bma255_suspend,
    .resume             = bma255_resume,
#endif
	.id_table = bma255_i2c_id,
};

/*                                                                            */
static struct i2c_client *bma255_i2c_client = NULL;
static struct platform_driver bma255_gsensor_driver;
static struct bma255_i2c_data *obj_i2c_data = NULL;
static bool sensor_power = true;
static bool test_status = 0;
static GSENSOR_VECTOR3D gsensor_gain;
static char selftestRes[8]= {0};

/*                                                                            */
#define GSE_TAG                  "[Gsensor] "
#define GSE_FUN(f)  	   printk(KERN_ERR GSE_TAG"%s()\n", __FUNCTION__)
#define GSE_ERR(fmt, args...)    printk(KERN_ERR GSE_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define GSE_LOG(fmt, args...)    printk(KERN_INFO GSE_TAG fmt, ##args)
/*                                                                            */
static struct data_resolution bma255_data_resolution[1] = {
 /*                                */
    {{ 1, 0}, 1024},   //                                                                                                
};
/*                                                                            */
static struct data_resolution bma255_offset_resolution = {{1, 0}, 1024};

/*                                                                                   */
static void BMA255_power(struct acc_hw *hw, unsigned int on)
{
	static unsigned int power_on = 0;

	if(hw->power_id != POWER_NONE_MACRO)		//                  
	{
		GSE_LOG("power %s\n", on ? "on" : "off");
		if(power_on == on)	//                        
		{
			GSE_LOG("ignore power control: %d\n", on);
		}
		else if(on)	//         
		{
			if(!hwPowerOn(hw->power_id, hw->power_vol, "BMA255"))
			{
				GSE_ERR("power on fails!!\n");
			}
		}
		else	//          
		{
			if (!hwPowerDown(hw->power_id, "BMA255"))
			{
				GSE_ERR("power off fail!!\n");
			}
		}
	}
	power_on = on;
}
/*                                                                            */
#ifdef CONFIG_MACH_LGE
static int bma255_smbus_read_byte(struct i2c_client *client, unsigned char reg_addr, unsigned char *data)
{
	s32 dummy;
	dummy = i2c_smbus_read_byte_data(client, reg_addr);
	if (dummy < 0)
		return -1;
	*data = dummy & 0x000000ff;

	return 0;
}

static int bma255_smbus_write_byte(struct i2c_client *client, unsigned char reg_addr, unsigned char *data)
{
	s32 dummy;
	dummy = i2c_smbus_write_byte_data(client, reg_addr, *data);
	if (dummy < 0)
		return -1;
	return 0;
}

static int bma255_smbus_read_byte_block(struct i2c_client *client, unsigned char reg_addr, unsigned char *data, unsigned char len)
{
	s32 dummy;
	dummy = i2c_smbus_read_i2c_block_data(client, reg_addr, len, data);
	if (dummy < 0)
		return -1;
	return 0;
}
#endif
/*                                                                            */
static int BMA255_SetDataResolution(struct bma255_i2c_data *obj)
{

/*                                   */

/*                                                                                   */

/*                           */

/*                                                                                       
                                                                                                             */

/*                                                                                                                   */

    obj->reso = &bma255_data_resolution[0];
	return 0;

/*                                                                                                   
                                                           */

}
/*                                                                            */
static int BMA255_ReadData(struct i2c_client *client, s16 data[BMA255_AXES_NUM])
{
	struct bma255_i2c_data *priv = i2c_get_clientdata(client);
	u8 addr = BMA255_REG_DATAXLOW;
	u8 buf[BMA255_DATA_LEN] = {0};
	int err = 0;

	if(NULL == client)
	{
		err = -EINVAL;
	}
	else if(err = hwmsen_read_block(client, addr, buf, BMA255_DATA_LEN))
	{
		GSE_ERR("error: %d\n", err);
	}
	else
	{
		/*                                           */
		data[BMA255_AXIS_X] = BMA255_GET_BITSLICE(buf[0], BMA255_ACC_X_LSB)
                                |(BMA255_GET_BITSLICE(buf[1], BMA255_ACC_X_MSB)<<BMA255_ACC_X_LSB__LEN);
		data[BMA255_AXIS_X] = data[BMA255_AXIS_X] << (sizeof(short)*8-(BMA255_ACC_X_LSB__LEN
                                + BMA255_ACC_X_MSB__LEN));
		data[BMA255_AXIS_X] = data[BMA255_AXIS_X] >> (sizeof(short)*8-(BMA255_ACC_X_LSB__LEN
                                + BMA255_ACC_X_MSB__LEN));
		data[BMA255_AXIS_Y] = BMA255_GET_BITSLICE(buf[2], BMA255_ACC_Y_LSB)
                                | (BMA255_GET_BITSLICE(buf[3], BMA255_ACC_Y_MSB)<<BMA255_ACC_Y_LSB__LEN);
		data[BMA255_AXIS_Y] = data[BMA255_AXIS_Y] << (sizeof(short)*8-(BMA255_ACC_Y_LSB__LEN
                                + BMA255_ACC_Y_MSB__LEN));
		data[BMA255_AXIS_Y] = data[BMA255_AXIS_Y] >> (sizeof(short)*8-(BMA255_ACC_Y_LSB__LEN
                                + BMA255_ACC_Y_MSB__LEN));
		data[BMA255_AXIS_Z] = BMA255_GET_BITSLICE(buf[4], BMA255_ACC_Z_LSB)
                                | (BMA255_GET_BITSLICE(buf[5], BMA255_ACC_Z_MSB)<<BMA255_ACC_Z_LSB__LEN);
		data[BMA255_AXIS_Z] = data[BMA255_AXIS_Z] << (sizeof(short)*8-(BMA255_ACC_Z_LSB__LEN
                                + BMA255_ACC_Z_MSB__LEN));
		data[BMA255_AXIS_Z] = data[BMA255_AXIS_Z] >> (sizeof(short)*8-(BMA255_ACC_Z_LSB__LEN
                                + BMA255_ACC_Z_MSB__LEN));

#ifdef CONFIG_BMA255_LOWPASS
		if(atomic_read(&priv->filter))
		{
			if(atomic_read(&priv->fir_en) && !atomic_read(&priv->suspend))
			{
				int idx, firlen = atomic_read(&priv->firlen);
				if(priv->fir.num < firlen)
				{
					priv->fir.raw[priv->fir.num][BMA255_AXIS_X] = data[BMA255_AXIS_X];
					priv->fir.raw[priv->fir.num][BMA255_AXIS_Y] = data[BMA255_AXIS_Y];
					priv->fir.raw[priv->fir.num][BMA255_AXIS_Z] = data[BMA255_AXIS_Z];
					priv->fir.sum[BMA255_AXIS_X] += data[BMA255_AXIS_X];
					priv->fir.sum[BMA255_AXIS_Y] += data[BMA255_AXIS_Y];
					priv->fir.sum[BMA255_AXIS_Z] += data[BMA255_AXIS_Z];
					if(atomic_read(&priv->trace) & BMA_TRC_FILTER)
					{
						GSE_LOG("add [%2d] [%5d %5d %5d] => [%5d %5d %5d]\n", priv->fir.num,
							priv->fir.raw[priv->fir.num][BMA255_AXIS_X], priv->fir.raw[priv->fir.num][BMA255_AXIS_Y], priv->fir.raw[priv->fir.num][BMA255_AXIS_Z],
							priv->fir.sum[BMA255_AXIS_X], priv->fir.sum[BMA255_AXIS_Y], priv->fir.sum[BMA255_AXIS_Z]);
					}
					priv->fir.num++;
					priv->fir.idx++;
				}
				else
				{
					idx = priv->fir.idx % firlen;
					priv->fir.sum[BMA255_AXIS_X] -= priv->fir.raw[idx][BMA255_AXIS_X];
					priv->fir.sum[BMA255_AXIS_Y] -= priv->fir.raw[idx][BMA255_AXIS_Y];
					priv->fir.sum[BMA255_AXIS_Z] -= priv->fir.raw[idx][BMA255_AXIS_Z];
					priv->fir.raw[idx][BMA255_AXIS_X] = data[BMA255_AXIS_X];
					priv->fir.raw[idx][BMA255_AXIS_Y] = data[BMA255_AXIS_Y];
					priv->fir.raw[idx][BMA255_AXIS_Z] = data[BMA255_AXIS_Z];
					priv->fir.sum[BMA255_AXIS_X] += data[BMA255_AXIS_X];
					priv->fir.sum[BMA255_AXIS_Y] += data[BMA255_AXIS_Y];
					priv->fir.sum[BMA255_AXIS_Z] += data[BMA255_AXIS_Z];
					priv->fir.idx++;
					data[BMA255_AXIS_X] = priv->fir.sum[BMA255_AXIS_X]/firlen;
					data[BMA255_AXIS_Y] = priv->fir.sum[BMA255_AXIS_Y]/firlen;
					data[BMA255_AXIS_Z] = priv->fir.sum[BMA255_AXIS_Z]/firlen;
					if(atomic_read(&priv->trace) & BMA_TRC_FILTER)
					{
						GSE_LOG("add [%2d] [%5d %5d %5d] => [%5d %5d %5d] : [%5d %5d %5d]\n", idx,
						priv->fir.raw[idx][BMA255_AXIS_X], priv->fir.raw[idx][BMA255_AXIS_Y], priv->fir.raw[idx][BMA255_AXIS_Z],
						priv->fir.sum[BMA255_AXIS_X], priv->fir.sum[BMA255_AXIS_Y], priv->fir.sum[BMA255_AXIS_Z],
						data[BMA255_AXIS_X], data[BMA255_AXIS_Y], data[BMA255_AXIS_Z]);
					}
				}
			}
		}
#endif
	}
	return err;
}
/*                                                                            */
static int BMA255_ReadOffset(struct i2c_client *client, s8 ofs[BMA255_AXES_NUM])
{
	int err;

#ifdef SW_CALIBRATION
	ofs[0]=ofs[1]=ofs[2]=0x0;
#else
	if(err = hwmsen_read_block(client, BMA255_REG_OFSX, ofs, BMA255_AXES_NUM))
	{
		GSE_ERR("error: %d\n", err);
	}
#endif

	return err;
}
/*                                                                            */
static int BMA255_ResetCalibration(struct i2c_client *client)
{
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);
	u8 ofs[4]={0,0,0,0};
	int err;

	#ifdef SW_CALIBRATION

	#else
		if(err = hwmsen_write_block(client, BMA255_REG_OFSX, ofs, 4))
		{
			GSE_ERR("error: %d\n", err);
		}
	#endif

	memset(obj->cali_sw, 0x00, sizeof(obj->cali_sw));
	memset(obj->offset, 0x00, sizeof(obj->offset));
	return err;
}
/*                                                                            */
static int BMA255_ReadCalibration(struct i2c_client *client, int dat[BMA255_AXES_NUM])
{
    struct bma255_i2c_data *obj = i2c_get_clientdata(client);
    int err = 0;
    int mul;

#ifdef SW_CALIBRATION
    mul = 0;//                                           
#else
    if ((err = BMA255_ReadOffset(client, obj->offset)))
    {
        GSE_ERR("read offset fail, %d\n", err);
        return err;
    }
    mul = obj->reso->sensitivity/bma255_offset_resolution.sensitivity;
#endif

    dat[obj->cvt.map[BMA255_AXIS_X]] = obj->cvt.sign[BMA255_AXIS_X]*(obj->offset[BMA255_AXIS_X]*mul + obj->cali_sw[BMA255_AXIS_X]);
    dat[obj->cvt.map[BMA255_AXIS_Y]] = obj->cvt.sign[BMA255_AXIS_Y]*(obj->offset[BMA255_AXIS_Y]*mul + obj->cali_sw[BMA255_AXIS_Y]);
    dat[obj->cvt.map[BMA255_AXIS_Z]] = obj->cvt.sign[BMA255_AXIS_Z]*(obj->offset[BMA255_AXIS_Z]*mul + obj->cali_sw[BMA255_AXIS_Z]);

    return err;
}
/*                                                                            */
static int BMA255_ReadCalibrationEx(struct i2c_client *client, int act[BMA255_AXES_NUM], int raw[BMA255_AXES_NUM])
{
	/*                                                               */
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);
	int err;
	int mul;

	#ifdef SW_CALIBRATION
		mul = 0;//                                           
	#else
		if(err = BMA255_ReadOffset(client, obj->offset))
		{
			GSE_ERR("read offset fail, %d\n", err);
			return err;
		}
		mul = obj->reso->sensitivity/bma255_offset_resolution.sensitivity;
	#endif

	raw[BMA255_AXIS_X] = obj->offset[BMA255_AXIS_X]*mul + obj->cali_sw[BMA255_AXIS_X];
	raw[BMA255_AXIS_Y] = obj->offset[BMA255_AXIS_Y]*mul + obj->cali_sw[BMA255_AXIS_Y];
	raw[BMA255_AXIS_Z] = obj->offset[BMA255_AXIS_Z]*mul + obj->cali_sw[BMA255_AXIS_Z];

	act[obj->cvt.map[BMA255_AXIS_X]] = obj->cvt.sign[BMA255_AXIS_X]*raw[BMA255_AXIS_X];
	act[obj->cvt.map[BMA255_AXIS_Y]] = obj->cvt.sign[BMA255_AXIS_Y]*raw[BMA255_AXIS_Y];
	act[obj->cvt.map[BMA255_AXIS_Z]] = obj->cvt.sign[BMA255_AXIS_Z]*raw[BMA255_AXIS_Z];

	return 0;
}
/*                                                                            */
static int BMA255_WriteCalibration(struct i2c_client *client, int dat[BMA255_AXES_NUM])
{
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);
	int err = 0;
	int cali[BMA255_AXES_NUM], raw[BMA255_AXES_NUM];
	int lsb = bma255_offset_resolution.sensitivity;
	int divisor = obj->reso->sensitivity/lsb;

	if(err = BMA255_ReadCalibrationEx(client, cali, raw))	/*                                     */
	{
		GSE_ERR("read offset fail, %d\n", err);
		return err;
	}

	GSE_LOG("OLDOFF: (%+3d %+3d %+3d): (%+3d %+3d %+3d) / (%+3d %+3d %+3d)\n",
		raw[BMA255_AXIS_X], raw[BMA255_AXIS_Y], raw[BMA255_AXIS_Z],
		obj->offset[BMA255_AXIS_X], obj->offset[BMA255_AXIS_Y], obj->offset[BMA255_AXIS_Z],
		obj->cali_sw[BMA255_AXIS_X], obj->cali_sw[BMA255_AXIS_Y], obj->cali_sw[BMA255_AXIS_Z]);

	/*                                            */
	cali[BMA255_AXIS_X] += dat[BMA255_AXIS_X];
	cali[BMA255_AXIS_Y] += dat[BMA255_AXIS_Y];
	cali[BMA255_AXIS_Z] += dat[BMA255_AXIS_Z];

	GSE_LOG("UPDATE: (%+3d %+3d %+3d)\n",
		dat[BMA255_AXIS_X], dat[BMA255_AXIS_Y], dat[BMA255_AXIS_Z]);

#ifdef SW_CALIBRATION
	obj->cali_sw[BMA255_AXIS_X] = obj->cvt.sign[BMA255_AXIS_X]*(cali[obj->cvt.map[BMA255_AXIS_X]]);
	obj->cali_sw[BMA255_AXIS_Y] = obj->cvt.sign[BMA255_AXIS_Y]*(cali[obj->cvt.map[BMA255_AXIS_Y]]);
	obj->cali_sw[BMA255_AXIS_Z] = obj->cvt.sign[BMA255_AXIS_Z]*(cali[obj->cvt.map[BMA255_AXIS_Z]]);
#else
	obj->offset[BMA255_AXIS_X] = (s8)(obj->cvt.sign[BMA255_AXIS_X]*(cali[obj->cvt.map[BMA255_AXIS_X]])/(divisor));
	obj->offset[BMA255_AXIS_Y] = (s8)(obj->cvt.sign[BMA255_AXIS_Y]*(cali[obj->cvt.map[BMA255_AXIS_Y]])/(divisor));
	obj->offset[BMA255_AXIS_Z] = (s8)(obj->cvt.sign[BMA255_AXIS_Z]*(cali[obj->cvt.map[BMA255_AXIS_Z]])/(divisor));

	/*                                                       */
	obj->cali_sw[BMA255_AXIS_X] = obj->cvt.sign[BMA255_AXIS_X]*(cali[obj->cvt.map[BMA255_AXIS_X]])%(divisor);
	obj->cali_sw[BMA255_AXIS_Y] = obj->cvt.sign[BMA255_AXIS_Y]*(cali[obj->cvt.map[BMA255_AXIS_Y]])%(divisor);
	obj->cali_sw[BMA255_AXIS_Z] = obj->cvt.sign[BMA255_AXIS_Z]*(cali[obj->cvt.map[BMA255_AXIS_Z]])%(divisor);

	GSE_LOG("NEWOFF: (%+3d %+3d %+3d): (%+3d %+3d %+3d) / (%+3d %+3d %+3d)\n",
		obj->offset[BMA255_AXIS_X]*divisor + obj->cali_sw[BMA255_AXIS_X],
		obj->offset[BMA255_AXIS_Y]*divisor + obj->cali_sw[BMA255_AXIS_Y],
		obj->offset[BMA255_AXIS_Z]*divisor + obj->cali_sw[BMA255_AXIS_Z],
		obj->offset[BMA255_AXIS_X], obj->offset[BMA255_AXIS_Y], obj->offset[BMA255_AXIS_Z],
		obj->cali_sw[BMA255_AXIS_X], obj->cali_sw[BMA255_AXIS_Y], obj->cali_sw[BMA255_AXIS_Z]);

	if(err = hwmsen_write_block(obj->client, BMA255_REG_OFSX, obj->offset, BMA255_AXES_NUM))
	{
		GSE_ERR("write offset fail: %d\n", err);
		return err;
	}
#endif

	return err;
}
/*                                                                            */
static int BMA255_CheckDeviceID(struct i2c_client *client)
{
	u8 databuf[2];
	int res = 0;

	memset(databuf, 0, sizeof(u8)*2);
	databuf[0] = BMA255_REG_DEVID;

	res = i2c_master_send(client, databuf, 0x1);
	if(res <= 0)
	{
		res = i2c_master_send(client, databuf, 0x1);
		if(res <= 0)
		{
			goto exit_BMA255_CheckDeviceID;
		}
	}

	udelay(500);

	databuf[0] = 0x0;
	res = i2c_master_recv(client, databuf, 0x01);
	if(res <= 0)
	{
		goto exit_BMA255_CheckDeviceID;
	}

	if(databuf[0]!=BMA255_FIXED_DEVID)
	{
		printk("BMA255_CheckDeviceID %d failt!\n ", databuf[0]);
		return BMA255_ERR_IDENTIFICATION;
	}
	else
	{
		printk("BMA255_CheckDeviceID %d pass!\n ", databuf[0]);
	}

	exit_BMA255_CheckDeviceID:
	if (res <= 0)
	{
		return BMA255_ERR_I2C;
	}

	return BMA255_SUCCESS;
}
/*                                                                            */
static int BMA255_SetPowerMode(struct i2c_client *client, bool enable)
{
	u8 databuf[2] = {0};
	int res = 0;
	u8 addr = BMA255_REG_POWER_CTL;
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);

	if(enable == sensor_power )
	{
		GSE_LOG("Sensor power status is newest!\n");
		return BMA255_SUCCESS;
	}

	if(hwmsen_read_block(client, addr, databuf, 0x01))
	{
		GSE_ERR("read power ctl register err!\n");
		return BMA255_ERR_I2C;
	}

	if(enable == TRUE)
	{
		databuf[0] &= ~BMA255_MEASURE_MODE;
	}
	else
	{
		databuf[0] |= BMA255_MEASURE_MODE;
	}
	databuf[1] = databuf[0];
	databuf[0] = BMA255_REG_POWER_CTL;

	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		GSE_LOG("set power mode failed!\n");
		return BMA255_ERR_I2C;
	}
	else if(atomic_read(&obj->trace) & BMA_TRC_INFO)
	{
		GSE_LOG("set power mode ok %d!\n", databuf[1]);
	}

	/*                     */
	databuf[0] = 0xff;
	hwmsen_read_block(client, BMA255_REG_POWER_CTL, databuf, 0x01);
	if (databuf[0] != databuf[1])
	{
		GSE_ERR("fail to set power mode!\n");
		return BMA255_ERR_SETUP_FAILURE;
	}

	//                                     

	sensor_power = enable;
	test_status = sensor_power;

	mdelay(20);

	return BMA255_SUCCESS;
}
/*                                                                            */
static int BMA255_SetDataFormat(struct i2c_client *client, u8 dataformat)
{
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);
	u8 databuf[2] = {0};
	int res = 0;

	if(hwmsen_read_block(client, BMA255_REG_DATA_FORMAT, databuf, 0x01))
	{
		printk("bma255 read Dataformat failt \n");
		return BMA255_ERR_I2C;
	}

	databuf[0] &= ~BMA255_RANGE_MASK;
	databuf[0] |= dataformat;
	databuf[1] = databuf[0];
	databuf[0] = BMA255_REG_DATA_FORMAT;

	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		return BMA255_ERR_I2C;
	}

	/*                     */
	databuf[0] = 0xff;
	hwmsen_read_block(client, BMA255_REG_DATA_FORMAT, databuf, 0x01);
	if (databuf[0] != databuf[1])
	{
		GSE_ERR("fail to set data format!\n");
		return BMA255_ERR_SETUP_FAILURE;
	}
	//                                      

	return BMA255_SetDataResolution(obj);
}
/*                                                                            */
static int BMA255_SetBWRate(struct i2c_client *client, u8 bwrate)
{
	u8 databuf[2] = {0};
	int res = 0;

	if(hwmsen_read_block(client, BMA255_REG_BW_RATE, databuf, 0x01))
	{
		printk("bma255 read rate failt \n");
		return BMA255_ERR_I2C;
	}

	databuf[0] &= ~BMA255_BW_MASK;
	databuf[0] |= bwrate;
	databuf[1] = databuf[0];
	databuf[0] = BMA255_REG_BW_RATE;

	res = i2c_master_send(client, databuf, 0x2);
	if(res <= 0)
	{
		return BMA255_ERR_I2C;
	}

	/*                     */
	databuf[0] = 0xff;
	hwmsen_read_block(client, BMA255_REG_BW_RATE, databuf, 0x01);
	if (databuf[0] != databuf[1])
	{
		GSE_ERR("fail to set bandwidth!\n");
		return BMA255_ERR_SETUP_FAILURE;
	}

	return BMA255_SUCCESS;
}
/*                                                                            */
static int BMA255_SetIntEnable(struct i2c_client *client, u8 intenable)
{
	int res = 0;

	res = hwmsen_write_byte(client, BMA255_INT_REG_1, 0x00);
	if(res != BMA255_SUCCESS)
	{
		return res;
	}
	res = hwmsen_write_byte(client, BMA255_INT_REG_2, 0x00);
	if(res != BMA255_SUCCESS)
	{
		return res;
	}
	printk("BMA255 disable interrupt ...\n");

	/*                              */

	return BMA255_SUCCESS;
}

/*                                                                            */
static int bma255_init_client(struct i2c_client *client, int reset_cali)
{
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);
	int res = 0;
	printk("bma255_init_client \n");

	res = BMA255_CheckDeviceID(client);
	if(res != BMA255_SUCCESS)
	{
		return res;
	}
	printk("BMA255_CheckDeviceID ok \n");

#ifdef CONFIG_MACH_LGE	/*                                                                 */
	res = BMA255_SetBWRate(client, BMA255_BW_125HZ);
#else	//                       
	res = BMA255_SetBWRate(client, BMA255_BW_100HZ);
#endif
	if(res != BMA255_SUCCESS )
	{
		return res;
	}
	printk("BMA255_SetBWRate OK!\n");

	res = BMA255_SetDataFormat(client, BMA255_RANGE_2G);
	if(res != BMA255_SUCCESS)
	{
		return res;
	}
	printk("BMA255_SetDataFormat OK!\n");

	gsensor_gain.x = gsensor_gain.y = gsensor_gain.z = obj->reso->sensitivity;

	res = BMA255_SetIntEnable(client, 0x00);
	if(res != BMA255_SUCCESS)
	{
		return res;
	}
	printk("BMA255 disable interrupt function!\n");

	res = BMA255_SetPowerMode(client, false);
	if(res != BMA255_SUCCESS)
	{
		return res;
	}
	printk("BMA255_SetPowerMode OK!\n");

	if(0 != reset_cali)
	{
		/*                                  */
		res = BMA255_ResetCalibration(client);
		if(res != BMA255_SUCCESS)
		{
			return res;
		}
	}
	printk("bma255_init_client OK!\n");
#ifdef CONFIG_BMA255_LOWPASS
	memset(&obj->fir, 0x00, sizeof(obj->fir));
#endif

	mdelay(20);

	return BMA255_SUCCESS;
}
/*                                                                            */
static int BMA255_ReadChipInfo(struct i2c_client *client, char *buf, int bufsize)
{
	u8 databuf[10];

	memset(databuf, 0, sizeof(u8)*10);

	if((NULL == buf)||(bufsize<=30))
	{
		return -1;
	}

	if(NULL == client)
	{
		*buf = 0;
		return -2;
	}

	sprintf(buf, "BMA255 Chip");
	return 0;
}
/*                                                                            */
static int BMA255_CompassReadData(struct i2c_client *client, char *buf, int bufsize)
{
	struct bma255_i2c_data *obj = (struct bma255_i2c_data*)i2c_get_clientdata(client);
	u8 databuf[20];
	int acc[BMA255_AXES_NUM];
	int res = 0;
	memset(databuf, 0, sizeof(u8)*10);

	if(NULL == buf)
	{
		return -1;
	}
	if(NULL == client)
	{
		*buf = 0;
		return -2;
	}

	if(sensor_power == FALSE)
	{
		res = BMA255_SetPowerMode(client, true);
		if(res)
		{
			GSE_ERR("Power on bma255 error %d!\n", res);
		}
	}

	if(res = BMA255_ReadData(client, obj->data))
	{
		GSE_ERR("I2C error: ret value=%d", res);
		return -3;
	}
	else
	{
		/*                */
		acc[obj->cvt.map[BMA255_AXIS_X]] = obj->cvt.sign[BMA255_AXIS_X]*obj->data[BMA255_AXIS_X];
		acc[obj->cvt.map[BMA255_AXIS_Y]] = obj->cvt.sign[BMA255_AXIS_Y]*obj->data[BMA255_AXIS_Y];
		acc[obj->cvt.map[BMA255_AXIS_Z]] = obj->cvt.sign[BMA255_AXIS_Z]*obj->data[BMA255_AXIS_Z];

		sprintf(buf, "%d %d %d", (s16)acc[BMA255_AXIS_X], (s16)acc[BMA255_AXIS_Y], (s16)acc[BMA255_AXIS_Z]);
		if(atomic_read(&obj->trace) & BMA_TRC_IOCTL)
		{
			GSE_LOG("gsensor data for compass: %s!\n", buf);
		}
	}

	return 0;
}
/*                                                                            */
static int BMA255_ReadSensorData(struct i2c_client *client, char *buf, int bufsize)
{
	struct bma255_i2c_data *obj = (struct bma255_i2c_data*)i2c_get_clientdata(client);
	u8 databuf[20];
	int acc[BMA255_AXES_NUM];
	int res = 0;
	memset(databuf, 0, sizeof(u8)*10);

	if(NULL == buf)
	{
		return -1;
	}
	if(NULL == client)
	{
		*buf = 0;
		return -2;
	}

	if(sensor_power == FALSE)
	{
		res = BMA255_SetPowerMode(client, true);
		if(res)
		{
			GSE_ERR("Power on bma255 error %d!\n", res);
		}
	}

	if(res = BMA255_ReadData(client, obj->data))
	{
		GSE_ERR("I2C error: ret value=%d", res);
		return -3;
	}
	else
	{
		obj->data[BMA255_AXIS_X] += obj->cali_sw[BMA255_AXIS_X];
		obj->data[BMA255_AXIS_Y] += obj->cali_sw[BMA255_AXIS_Y];
		obj->data[BMA255_AXIS_Z] += obj->cali_sw[BMA255_AXIS_Z];

		/*                */
		acc[obj->cvt.map[BMA255_AXIS_X]] = obj->cvt.sign[BMA255_AXIS_X]*obj->data[BMA255_AXIS_X];
		acc[obj->cvt.map[BMA255_AXIS_Y]] = obj->cvt.sign[BMA255_AXIS_Y]*obj->data[BMA255_AXIS_Y];
		acc[obj->cvt.map[BMA255_AXIS_Z]] = obj->cvt.sign[BMA255_AXIS_Z]*obj->data[BMA255_AXIS_Z];

		//              
		acc[BMA255_AXIS_X] = acc[BMA255_AXIS_X] * GRAVITY_EARTH_1000 / obj->reso->sensitivity;
		acc[BMA255_AXIS_Y] = acc[BMA255_AXIS_Y] * GRAVITY_EARTH_1000 / obj->reso->sensitivity;
		acc[BMA255_AXIS_Z] = acc[BMA255_AXIS_Z] * GRAVITY_EARTH_1000 / obj->reso->sensitivity;

		sprintf(buf, "%04x %04x %04x", acc[BMA255_AXIS_X], acc[BMA255_AXIS_Y], acc[BMA255_AXIS_Z]);
		if(atomic_read(&obj->trace) & BMA_TRC_IOCTL)
		{
			GSE_LOG("gsensor data: %s!\n", buf);
		}
	}

	return 0;
}
/*                                                                            */
static int BMA255_ReadRawData(struct i2c_client *client, char *buf)
{
	struct bma255_i2c_data *obj = (struct bma255_i2c_data*)i2c_get_clientdata(client);
	int res = 0;

	if (!buf || !client)
	{
		return EINVAL;
	}

	if(res = BMA255_ReadData(client, obj->data))
	{
		GSE_ERR("I2C error: ret value=%d", res);
		return EIO;
	}
	else
	{
		sprintf(buf, "BMA255_ReadRawData %04x %04x %04x", obj->data[BMA255_AXIS_X],
			obj->data[BMA255_AXIS_Y], obj->data[BMA255_AXIS_Z]);
	}

	return 0;
}
/*                                                                            */
static int bma255_set_mode(struct i2c_client *client, unsigned char mode)
{
	int comres = 0;
	unsigned char data[2] = {BMA255_EN_LOW_POWER__REG};

	if ((client == NULL) || (mode >= 3))
	{
		return -1;
	}

	comres = hwmsen_read_block(client, BMA255_EN_LOW_POWER__REG, data+1, 1);
	switch (mode) {
	case BMA255_MODE_NORMAL:
		data[1]  = BMA255_SET_BITSLICE(data[1], BMA255_EN_LOW_POWER, 0);
		data[1]  = BMA255_SET_BITSLICE(data[1],	BMA255_EN_SUSPEND, 0);
		break;
	case BMA255_MODE_LOWPOWER:
		data[1]  = BMA255_SET_BITSLICE(data[1],	BMA255_EN_LOW_POWER, 1);
		data[1]  = BMA255_SET_BITSLICE(data[1],	BMA255_EN_SUSPEND, 0);
		break;
	case BMA255_MODE_SUSPEND:
		data[1]  = BMA255_SET_BITSLICE(data[1],	BMA255_EN_LOW_POWER, 0);
		data[1]  = BMA255_SET_BITSLICE(data[1],	BMA255_EN_SUSPEND, 1);
		break;
	default:
		break;
	}

	comres = i2c_master_send(client, data, 2);

	if(comres <= 0)
	{
		return BMA255_ERR_I2C;
	}
	else
	{
		return comres;
	}
}
/*                                                                            */
static int bma255_get_mode(struct i2c_client *client, unsigned char *mode)
{
	int comres = 0;

	if (client == NULL)
	{
		return -1;
	}
	comres = hwmsen_read_block(client,
			BMA255_EN_LOW_POWER__REG, mode, 1);
	*mode  = (*mode) >> 6;

	return comres;
}

/*                                                                            */
static int bma255_set_range(struct i2c_client *client, unsigned char range)
{
	int comres = 0;
	unsigned char data[2] = {BMA255_RANGE_SEL__REG};

	if (client == NULL)
	{
		return -1;
	}

	comres = hwmsen_read_block(client,
			BMA255_RANGE_SEL__REG, data+1, 1);

	data[1]  = BMA255_SET_BITSLICE(data[1],
			BMA255_RANGE_SEL, range);

	comres = i2c_master_send(client, data, 2);

	if(comres <= 0)
	{
		return BMA255_ERR_I2C;
	}
	else
	{
		return comres;
	}
}
/*                                                                            */
static int bma255_get_range(struct i2c_client *client, unsigned char *range)
{
	int comres = 0;
	unsigned char data;

	if (client == NULL)
	{
		return -1;
	}

	comres = hwmsen_read_block(client, BMA255_RANGE_SEL__REG,	&data, 1);
	*range = BMA255_GET_BITSLICE(data, BMA255_RANGE_SEL);

	return comres;
}
/*                                                                            */
static int bma255_set_bandwidth(struct i2c_client *client, unsigned char bandwidth)
{
	int comres = 0;
	unsigned char data[2] = {BMA255_BANDWIDTH__REG};

	if (client == NULL)
	{
		return -1;
	}

	comres = hwmsen_read_block(client,
			BMA255_BANDWIDTH__REG, data+1, 1);

	data[1]  = BMA255_SET_BITSLICE(data[1],
			BMA255_BANDWIDTH, bandwidth);

	comres = i2c_master_send(client, data, 2);

	if(comres <= 0)
	{
		return BMA255_ERR_I2C;
	}
	else
	{
		return comres;
	}
}
/*                                                                            */
static int bma255_get_bandwidth(struct i2c_client *client, unsigned char *bandwidth)
{
	int comres = 0;
	unsigned char data;

	if (client == NULL)
	{
		return -1;
	}

	comres = hwmsen_read_block(client, BMA255_BANDWIDTH__REG, &data, 1);
	data = BMA255_GET_BITSLICE(data, BMA255_BANDWIDTH);

	if (data < 0x08)        //      
	{
		*bandwidth = 0x08;
	}
	else if (data > 0x0f)   //       
	{
		*bandwidth = 0x0f;
	}
	else
	{
		*bandwidth = data;
	}
	return comres;
}
/*                                                                            */
#ifdef BMA255_ACCEL_CALIBRATION
static int bma255_set_offset_target(struct i2c_client *client, unsigned char channel, unsigned char offset)
{
	unsigned char data;
	int comres = 0;

	switch (channel)
	{
		case BMA255_CUT_OFF:
			comres = bma255_smbus_read_byte(client, BMA255_COMP_CUTOFF__REG, &data);
			data = BMA255_SET_BITSLICE(data, BMA255_COMP_CUTOFF, offset);
			comres = bma255_smbus_write_byte(client, BMA255_COMP_CUTOFF__REG, &data);
			break;

		case BMA255_OFFSET_TRIGGER_X:
			comres = bma255_smbus_read_byte(client, BMA255_COMP_TARGET_OFFSET_X__REG, &data);
			data = BMA255_SET_BITSLICE(data, BMA255_COMP_TARGET_OFFSET_X, offset);
			comres = bma255_smbus_write_byte(client, BMA255_COMP_TARGET_OFFSET_X__REG, &data);
			break;

		case BMA255_OFFSET_TRIGGER_Y:
			comres = bma255_smbus_read_byte(client, BMA255_COMP_TARGET_OFFSET_Y__REG, &data);
			data = BMA255_SET_BITSLICE(data, BMA255_COMP_TARGET_OFFSET_Y, offset);
			comres = bma255_smbus_write_byte(client, BMA255_COMP_TARGET_OFFSET_Y__REG, &data);
			break;

		case BMA255_OFFSET_TRIGGER_Z:
			comres = bma255_smbus_read_byte(client, BMA255_COMP_TARGET_OFFSET_Z__REG, &data);
			data = BMA255_SET_BITSLICE(data, BMA255_COMP_TARGET_OFFSET_Z, offset);
			comres = bma255_smbus_write_byte(client, BMA255_COMP_TARGET_OFFSET_Z__REG, &data);
			break;

		default:
			comres = -1;
			break;
	}

	return comres;
}

static int bma255_get_cal_ready(struct i2c_client *client, unsigned char *calrdy)
{
	int comres = 0 ;
	unsigned char data;

	comres = bma255_smbus_read_byte(client, BMA255_FAST_CAL_RDY_S__REG, &data);
	data = BMA255_GET_BITSLICE(data, BMA255_FAST_CAL_RDY_S);
	*calrdy = data;

	return comres;
}

static int bma255_set_cal_trigger(struct i2c_client *client, unsigned char caltrigger)
{
	int comres = 0;
	unsigned char data;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);

	if(atomic_read(&bma255->fast_calib_rslt) != 0)
	{
		atomic_set(&bma255->fast_calib_rslt, 0);
		GSE_LOG(KERN_INFO "[set] bma2X2->fast_calib_rslt:%d\n",atomic_read(&bma255->fast_calib_rslt));
	}

	comres = bma255_smbus_read_byte(client, BMA255_CAL_TRIGGER__REG, &data);
	data = BMA255_SET_BITSLICE(data, BMA255_CAL_TRIGGER, caltrigger);
	comres = bma255_smbus_write_byte(client, BMA255_CAL_TRIGGER__REG, &data);

	return comres;
}

static int bma255_set_offset_x(struct i2c_client *client, unsigned char offsetfilt)
{
	int comres = 0;
	unsigned char data;

	data =  offsetfilt;
	comres = bma255_smbus_write_byte(client, BMA255_OFFSET_X_AXIS_REG, &data);

	return comres;
}

static int bma255_get_offset_x(struct i2c_client *client, unsigned char *offsetfilt)
{
	int comres = 0;
	unsigned char data;

	comres = bma255_smbus_read_byte(client, BMA255_OFFSET_X_AXIS_REG, &data);
	*offsetfilt = data;

	return comres;
}

static int bma255_set_offset_y(struct i2c_client *client, unsigned char offsetfilt)
{
	int comres = 0;
	unsigned char data;

	data =  offsetfilt;
	comres = bma255_smbus_write_byte(client, BMA255_OFFSET_Y_AXIS_REG, &data);

	return comres;
}

static int bma255_get_offset_y(struct i2c_client *client, unsigned char *offsetfilt)
{
	int comres = 0;
	unsigned char data;

	comres = bma255_smbus_read_byte(client, BMA255_OFFSET_Y_AXIS_REG, &data);
	*offsetfilt = data;

	return comres;
}

static int bma255_set_offset_z(struct i2c_client *client, unsigned char offsetfilt)
{
	int comres = 0;
	unsigned char data;

	data =  offsetfilt;
	comres = bma255_smbus_write_byte(client, BMA255_OFFSET_Z_AXIS_REG, &data);

	return comres;
}

static int bma255_get_offset_z(struct i2c_client *client, unsigned char *offsetfilt)
{
	int comres = 0;
	unsigned char data;

	comres = bma255_smbus_read_byte(client, BMA255_OFFSET_Z_AXIS_REG, &data);
	*offsetfilt = data;

	return comres;
}

static int bma255_read_accel_xyz(struct i2c_client *client, struct bma255acc *acc)
{
	int comres = 0;
	unsigned char data[6];

	comres = bma255_smbus_read_byte_block(client, BMA255_ACC_X_LSB__REG, data, 6);

	acc->x = BMA255_GET_BITSLICE(data[0], BMA255_ACC_X_LSB) | (BMA255_GET_BITSLICE(data[1], BMA255_ACC_X_MSB)<<(BMA255_ACC_X_LSB__LEN));
	acc->x = acc->x << (sizeof(short)*8-(BMA255_ACC_X_LSB__LEN + BMA255_ACC_X_MSB__LEN));
	acc->x = acc->x >> (sizeof(short)*8-(BMA255_ACC_X_LSB__LEN + BMA255_ACC_X_MSB__LEN));

	acc->y = BMA255_GET_BITSLICE(data[2], BMA255_ACC_Y_LSB) | (BMA255_GET_BITSLICE(data[3], BMA255_ACC_Y_MSB)<<(BMA255_ACC_Y_LSB__LEN));
	acc->y = acc->y << (sizeof(short)*8-(BMA255_ACC_Y_LSB__LEN + BMA255_ACC_Y_MSB__LEN));
	acc->y = acc->y >> (sizeof(short)*8-(BMA255_ACC_Y_LSB__LEN + BMA255_ACC_Y_MSB__LEN));

	acc->z = BMA255_GET_BITSLICE(data[4], BMA255_ACC_Z_LSB) | (BMA255_GET_BITSLICE(data[5], BMA255_ACC_Z_MSB)<<(BMA255_ACC_Z_LSB__LEN));
	acc->z = acc->z << (sizeof(short)*8-(BMA255_ACC_Z_LSB__LEN + BMA255_ACC_Z_MSB__LEN));
	acc->z = acc->z >> (sizeof(short)*8-(BMA255_ACC_Z_LSB__LEN + BMA255_ACC_Z_MSB__LEN));

	return comres;
}


static ssize_t bma255_fast_calibration_x_show(struct device_driver *ddri, char *buf)
{
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&bma255->fast_calib_x_rslt));
}

static ssize_t bma255_fast_calibration_x_store(struct device_driver *ddri, char *buf, size_t count)
{
	unsigned long data;
	signed char tmp;
	unsigned char timeout = 0;
	unsigned int timeout_shaking = 0;
	int error;
	struct i2c_client *client =bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);
	struct bma255acc acc_cal;
	struct bma255acc acc_cal_pre;

	test_status = 4;	//                   

	if (error = strict_strtoul(buf, 10, &data))
		return error;

	atomic_set(&bma255->fast_calib_x_rslt, 0);
	bma255_read_accel_xyz(bma255->client, &acc_cal_pre);

	do{
		mdelay(20);
		bma255_read_accel_xyz(bma255->client, &acc_cal);

		GSE_LOG(KERN_INFO "===============moved x=============== timeout = %d\n",timeout_shaking);
		GSE_LOG(KERN_INFO "(%d, %d, %d) (%d, %d, %d)\n", acc_cal_pre.x, acc_cal_pre.y, acc_cal_pre.z, acc_cal.x,acc_cal.y,acc_cal.z);

		if((abs(acc_cal.x - acc_cal_pre.x) > BMA255_SHAKING_DETECT_THRESHOLD)
			|| (abs((acc_cal.y - acc_cal_pre.y)) > BMA255_SHAKING_DETECT_THRESHOLD)
			|| (abs((acc_cal.z - acc_cal_pre.z)) > BMA255_SHAKING_DETECT_THRESHOLD))
		{
			atomic_set(&bma255->fast_calib_rslt, 0);
			GSE_LOG(KERN_INFO "===============shaking x===============\n");
			return -EINVAL;
		}
		else
		{
			acc_cal_pre.x = acc_cal.x;
			acc_cal_pre.y = acc_cal.y;
			acc_cal_pre.z = acc_cal.z;
		}
		timeout_shaking++;
		GSE_LOG(KERN_INFO "===============timeout_shaking: %d=============== \n",timeout_shaking);
	} while(timeout_shaking < 10);

	GSE_LOG(KERN_INFO "===============complete shaking x check===============\n");

	if (bma255_set_offset_target(bma255->client, 1, (unsigned char)data) < 0)
		return -EINVAL;

	if (bma255_set_cal_trigger(bma255->client, 1) < 0)
		return -EINVAL;

	do{
		mdelay(2);
		bma255_get_cal_ready(bma255->client, &tmp);

		GSE_LOG(KERN_INFO "x wait 2ms cal ready flag is %d\n", tmp);

		timeout++;
		if (timeout == 50)
		{
			GSE_LOG(KERN_INFO "get fast calibration ready error\n");
			return -EINVAL;
		}
	} while (tmp == 0);

	atomic_set(&bma255->fast_calib_x_rslt, 1);
	GSE_LOG(KERN_INFO "x axis fast calibration finished\n");

	return count;
}

static ssize_t bma255_fast_calibration_y_show(struct device_driver *ddri, char *buf)
{

	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&bma255->fast_calib_y_rslt));
}

static ssize_t bma255_fast_calibration_y_store(struct device_driver *ddri, char *buf, size_t count)
{
	unsigned long data;
	signed char tmp;
	unsigned char timeout = 0;
	unsigned int timeout_shaking = 0;
	int error;
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);
	struct bma255acc acc_cal;
	struct bma255acc acc_cal_pre;

	if (error = strict_strtoul(buf, 10, &data))
		return error;

	atomic_set(&bma255->fast_calib_y_rslt, 0);
	bma255_read_accel_xyz(bma255->client, &acc_cal_pre);

	do{
		mdelay(20);
		bma255_read_accel_xyz(bma255->client, &acc_cal);

		GSE_LOG(KERN_INFO "===============moved y=============== timeout = %d\n",timeout_shaking);
		GSE_LOG(KERN_INFO "(%d, %d, %d) (%d, %d, %d)\n", acc_cal_pre.x, acc_cal_pre.y, acc_cal_pre.z, acc_cal.x,acc_cal.y,acc_cal.z);

		if((abs(acc_cal.x - acc_cal_pre.x) > BMA255_SHAKING_DETECT_THRESHOLD)
			|| (abs((acc_cal.y - acc_cal_pre.y)) > BMA255_SHAKING_DETECT_THRESHOLD)
			|| (abs((acc_cal.z - acc_cal_pre.z)) > BMA255_SHAKING_DETECT_THRESHOLD))
		{
			atomic_set(&bma255->fast_calib_rslt, 0);
			GSE_LOG(KERN_INFO "===============shaking y===============\n");
			return -EINVAL;
		}
		else
		{
			acc_cal_pre.x = acc_cal.x;
			acc_cal_pre.y = acc_cal.y;
			acc_cal_pre.z = acc_cal.z;
		}
		timeout_shaking++;
	} while(timeout_shaking < 10);

	GSE_LOG(KERN_INFO "===============complete shaking y check===============\n");

	if (bma255_set_offset_target(bma255->client, 2, (unsigned char)data) < 0)
		return -EINVAL;

	if (bma255_set_cal_trigger(bma255->client, 2) < 0)
		return -EINVAL;

	do {
		mdelay(2);
		bma255_get_cal_ready(bma255->client, &tmp);

		GSE_LOG(KERN_INFO "y wait 2ms cal ready flag is %d\n", tmp);

		timeout++;
		if (timeout == 50)
		{
			GSE_LOG(KERN_INFO "get fast calibration ready error\n");
			return -EINVAL;
		}
	} while (tmp == 0);

	atomic_set(&bma255->fast_calib_y_rslt, 1);
	GSE_LOG(KERN_INFO "y axis fast calibration finished\n");

	return count;
}

static ssize_t bma255_fast_calibration_z_show(struct device_driver *ddri, char *buf)
{
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&bma255->fast_calib_z_rslt));
}

static ssize_t bma255_fast_calibration_z_store(struct device_driver *ddri, char *buf, size_t count)
{
	unsigned long data;
	signed char tmp;
	unsigned char timeout = 0;
	unsigned int timeout_shaking = 0;
	int error;
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);
	struct bma255acc acc_cal;
	struct bma255acc acc_cal_pre;

	if (error = strict_strtoul(buf, 10, &data))
		return error;

	atomic_set(&bma255->fast_calib_z_rslt, 0);
	bma255_read_accel_xyz(bma255->client, &acc_cal_pre);

	do{
		mdelay(20);
		bma255_read_accel_xyz(bma255->client, &acc_cal);

		GSE_LOG(KERN_INFO "===============moved z=============== timeout = %d\n",timeout_shaking);
		GSE_LOG(KERN_INFO "(%d, %d, %d) (%d, %d, %d)\n", acc_cal_pre.x, acc_cal_pre.y, acc_cal_pre.z, acc_cal.x,acc_cal.y,acc_cal.z );

		if((abs(acc_cal.x - acc_cal_pre.x) > BMA255_SHAKING_DETECT_THRESHOLD)
			|| (abs((acc_cal.y - acc_cal_pre.y)) > BMA255_SHAKING_DETECT_THRESHOLD)
			|| (abs((acc_cal.z - acc_cal_pre.z)) > BMA255_SHAKING_DETECT_THRESHOLD))
		{
			atomic_set(&bma255->fast_calib_rslt, 0);
			GSE_LOG(KERN_INFO "===============shaking z===============\n");
			return -EINVAL;
		}
		else
		{
			acc_cal_pre.x = acc_cal.x;
			acc_cal_pre.y = acc_cal.y;
			acc_cal_pre.z = acc_cal.z;
		}
		timeout_shaking++;
	} while(timeout_shaking < 10);

	GSE_LOG(KERN_INFO "===============complete shaking z check===============\n");

	if (bma255_set_offset_target(bma255->client, 3, (unsigned char)data) < 0)
		return -EINVAL;

	if (bma255_set_cal_trigger(bma255->client, 3) < 0)
		return -EINVAL;

	do {
		mdelay(2);
		bma255_get_cal_ready(bma255->client, &tmp);

		GSE_LOG(KERN_INFO " z wait 2ms cal ready flag is %d\n", tmp);

		timeout++;
		if (timeout == 50)
		{
			GSE_LOG(KERN_INFO "get fast calibration ready error\n");
			return -EINVAL;
		}
	} while (tmp == 0);

	atomic_set(&bma255->fast_calib_z_rslt, 1);
	GSE_LOG(KERN_INFO "z axis fast calibration finished\n");

	test_status = sensor_power;

	return count;
}

/*                                                                */
static int bma255_runCalibration(void)
{
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);
	unsigned char backup_offset_x, backup_offset_y, backup_offset_z;
	int res = 0;
	bool flatDetection = 1;
	int cali[3];

	if(bma255_get_offset_x(bma255->client, &backup_offset_x) < 0)
		return FALSE;
	if(bma255_get_offset_y(bma255->client, &backup_offset_y) < 0)
		return FALSE;
	if(bma255_get_offset_z(bma255->client, &backup_offset_z) < 0)
		return FALSE;

	GSE_LOG("backup_offset_x: %d, backup_offset_y: %d, backup_offset_z: %d\n",backup_offset_x,backup_offset_y,backup_offset_z);

	if(sensor_power == FALSE)
	{
		res = BMA255_SetPowerMode(client, true);
		if(res)
		{
			GSE_ERR("Power on bma255 error %d!\n", res);
			return FALSE;
		}
	}

	flatDetection = bma255_fast_calibration_x_store(NULL, "0", 1);
	GSE_LOG("Calibration X-Axis, flatDetection : %d", flatDetection);

	if(flatDetection)
	{
		mdelay(400);
		flatDetection = bma255_fast_calibration_y_store(NULL, "0", 1);
		GSE_LOG("Calibration Y-Axis, flatDetection : %d", flatDetection);

		if(flatDetection)
		{
			mdelay(400);
			flatDetection = bma255_fast_calibration_z_store(NULL, "1", 1);
			GSE_LOG("Calibration Z-Axis, flatDetection : %d", flatDetection);

			if(flatDetection)
			{
				mdelay(300);
				bma255_get_offset_x(bma255->client, &cali[0]);
				bma255_get_offset_y(bma255->client, &cali[1]);
				bma255_get_offset_z(bma255->client, &cali[2]);
				GSE_LOG("new_offset_x: %d, new_offset_y: %d, new_offset_z: %d\n", cali[0], cali[1], cali[2]);
#if defined(CONFIG_MACH_LGE)
				atomic_set(&bma255->fast_calib_rslt, 1);
#else	//                                                                         
				LGE_FacWriteAccelerometerCalibration(((unsigned int *)cali) == TRUE) {
					atomic_set(&bma255->fast_calib_rslt, 1);
				}
#endif

			}
		}
	}

	if(!flatDetection)
	{
		if(bma255_set_offset_x(bma255->client, (unsigned char)backup_offset_x) < 0)
			return -EINVAL;
		if(bma255_set_offset_y(bma255->client, (unsigned char)backup_offset_y) < 0)
			return -EINVAL;
		if(bma255_set_offset_z(bma255->client, (unsigned char)backup_offset_z) < 0)
			return -EINVAL;

		return BMA255_ERR_STATUS;
		GSE_LOG("Recovery backup cal value: x=%d, y=%d, z=%d\n", backup_offset_x, backup_offset_y, backup_offset_z);
	}

	return BMA255_SUCCESS;
}
/*                                                                */
#endif

static ssize_t show_chipinfo_value(struct device_driver *ddri, char *buf)
{
	struct i2c_client *client = bma255_i2c_client;
	char strbuf[BMA255_BUFSIZE];
	if(NULL == client)
	{
		GSE_ERR("i2c client is null!!\n");
		return 0;
	}

	BMA255_ReadChipInfo(client, strbuf, BMA255_BUFSIZE);
	return snprintf(buf, PAGE_SIZE, "%s\n", strbuf);
}

static ssize_t gsensor_init(struct device_driver *ddri, char *buf, size_t count)
{
	struct i2c_client *client = bma255_i2c_client;
	char strbuf[BMA255_BUFSIZE];

	if(NULL == client)
	{
		GSE_ERR("i2c client is null!!\n");
		return 0;
	}
	bma255_init_client(client, 1);
	return snprintf(buf, PAGE_SIZE, "%s\n", strbuf);
}

/*                                                                            */
/*                                              
 */
static ssize_t show_cpsopmode_value(struct device_driver *ddri, char *buf)
{
	unsigned char data;

	if (bma255_get_mode(bma255_i2c_client, &data) < 0)
	{
		return sprintf(buf, "Read error\n");
	}
	else
	{
		return sprintf(buf, "%d\n", data);
	}
}

/*                                                                            */
/*                                              
 */
static ssize_t store_cpsopmode_value(struct device_driver *ddri, char *buf, size_t count)
{
	unsigned long data;
	int error;

	if (error = strict_strtoul(buf, 10, &data))
	{
		return error;
	}
	if (data == BMA255_MODE_NORMAL)
	{
		BMA255_SetPowerMode(bma255_i2c_client, true);
	}
	else if (data == BMA255_MODE_SUSPEND)
	{
		BMA255_SetPowerMode(bma255_i2c_client, false);
	}
	else if (bma255_set_mode(bma255_i2c_client, (unsigned char) data) < 0)
	{
		GSE_ERR("invalid content: '%s', length = %d\n", buf, count);
	}

	return count;
}

/*                                                                            */
/*                                             
 */
static ssize_t show_cpsrange_value(struct device_driver *ddri, char *buf)
{
	unsigned char data;

	if (bma255_get_range(bma255_i2c_client, &data) < 0)
	{
		return sprintf(buf, "Read error\n");
	}
	else
	{
		return sprintf(buf, "%d\n", data);
	}
}

/*                                                                            */
/*                                             
 */
static ssize_t store_cpsrange_value(struct device_driver *ddri, char *buf, size_t count)
{
	unsigned long data;
	int error;

	if (error = strict_strtoul(buf, 10, &data))
	{
		return error;
	}
	if (bma255_set_range(bma255_i2c_client, (unsigned char) data) < 0)
	{
		GSE_ERR("invalid content: '%s', length = %d\n", buf, count);
	}

	return count;
}
/*                                                                            */
/*                                                 
 */
static ssize_t show_cpsbandwidth_value(struct device_driver *ddri, char *buf)
{
	unsigned char data;

	if (bma255_get_bandwidth(bma255_i2c_client, &data) < 0)
	{
		return sprintf(buf, "Read error\n");
	}
	else
	{
		return sprintf(buf, "%d\n", data);
	}
}

/*                                                                            */
/*                                                 
 */
static ssize_t store_cpsbandwidth_value(struct device_driver *ddri, char *buf, size_t count)
{
	unsigned long data;
	int error;

	if (error = strict_strtoul(buf, 10, &data))
	{
		return error;
	}
	if (bma255_set_bandwidth(bma255_i2c_client, (unsigned char) data) < 0)
	{
		GSE_ERR("invalid content: '%s', length = %d\n", buf, count);
	}

	return count;
}

/*                                                                            */
/*                                            
 */
static ssize_t show_cpsdata_value(struct device_driver *ddri, char *buf)
{
	struct i2c_client *client = bma255_i2c_client;
	char strbuf[BMA255_BUFSIZE];

	if(NULL == client)
	{
		GSE_ERR("i2c client is null!!\n");
		return 0;
	}
	BMA255_CompassReadData(client, strbuf, BMA255_BUFSIZE);
	return snprintf(buf, PAGE_SIZE, "%s\n", strbuf);
}

/*                                                                            */
static ssize_t show_sensordata_value(struct device_driver *ddri, char *buf)
{
	struct i2c_client *client = bma255_i2c_client;
	char strbuf[BMA255_BUFSIZE];

	if(NULL == client)
	{
		GSE_ERR("i2c client is null!!\n");
		return 0;
	}
	BMA255_ReadSensorData(client, strbuf, BMA255_BUFSIZE);
	return snprintf(buf, PAGE_SIZE, "%s\n", strbuf);
}

static ssize_t show_sensorrawdata_value(struct device_driver *ddri, char *buf, size_t count)
{
	struct i2c_client *client = bma255_i2c_client;
	char strbuf[BMA255_BUFSIZE];

	if(NULL == client)
	{
		GSE_ERR("i2c client is null!!\n");
		return 0;
	}
	BMA255_ReadRawData(client, strbuf);
	return snprintf(buf, PAGE_SIZE, "%s\n", strbuf);
}

/*                                                                            */
#ifdef CONFIG_MACH_LGE
static ssize_t show_cali_value(struct device_driver *ddri, char *buf)
{
	GSE_FUN();
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);
	unsigned char offset_x,offset_y,offset_z;
	unsigned int offsets[3];

	if(bma255_get_offset_x(bma255->client, &offset_x) < 0)
		return -EINVAL;
	if(bma255_get_offset_y(bma255->client, &offset_y) < 0)
		return -EINVAL;
	if(bma255_get_offset_z(bma255->client, &offset_z) < 0)
		return -EINVAL;

	GSE_LOG("offset_x: %c, offset_y: %c, offset_z: %c\n",offset_x,offset_y,offset_z);

	return snprintf(buf, PAGE_SIZE, "%d %d %d \n", offset_x, offset_y, offset_z);
}

static ssize_t store_cali_value(struct device_driver *ddri, char *buf, size_t count)
{
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);
	int err;
	unsigned int offset_x,offset_y,offset_z;
	int dat[BMA255_AXES_NUM];

	if(!strncmp(buf, "rst", 3))
	{
		GSE_FUN();
		if(err = BMA255_ResetCalibration(client))
		{
			GSE_ERR("reset offset err = %d\n", err);
		}
	}
	else if(3 == sscanf(buf, "%d %d %d", &offset_x, &offset_y, &offset_z))
	{
		GSE_LOG("store_cali_value: x=%d, y=%d, z=%d\n", offset_x, offset_y, offset_z);

		if(bma255_set_offset_x(bma255->client, (unsigned char)offset_x) < 0)
			return -EINVAL;
		if(bma255_set_offset_y(bma255->client, (unsigned char)offset_y) < 0)
			return -EINVAL;
		if(bma255_set_offset_z(bma255->client, (unsigned char)offset_z) < 0)
			return -EINVAL;
	}
	else
	{
		GSE_ERR("invalid format\n");
	}

	return count;
}
#else	//                  
static ssize_t show_cali_value(struct device_driver *ddri, char *buf)
{
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *obj;
	int err, len = 0, mul;
	int tmp[BMA255_AXES_NUM];

	if(NULL == client)
	{
		GSE_ERR("i2c client is null!!\n");
		return 0;
	}

	obj = i2c_get_clientdata(client);

	if(err = BMA255_ReadOffset(client, obj->offset))
	{
		return -EINVAL;
	}
	else if(err = BMA255_ReadCalibration(client, tmp))
	{
		return -EINVAL;
	}
	else
	{
		mul = obj->reso->sensitivity/bma255_offset_resolution.sensitivity;
		len += snprintf(buf+len, PAGE_SIZE-len, "[HW ][%d] (%+3d, %+3d, %+3d) : (0x%02X, 0x%02X, 0x%02X)\n", mul,
			obj->offset[BMA255_AXIS_X], obj->offset[BMA255_AXIS_Y], obj->offset[BMA255_AXIS_Z],
			obj->offset[BMA255_AXIS_X], obj->offset[BMA255_AXIS_Y], obj->offset[BMA255_AXIS_Z]);
		len += snprintf(buf+len, PAGE_SIZE-len, "[SW ][%d] (%+3d, %+3d, %+3d)\n", 1,
			obj->cali_sw[BMA255_AXIS_X], obj->cali_sw[BMA255_AXIS_Y], obj->cali_sw[BMA255_AXIS_Z]);

		len += snprintf(buf+len, PAGE_SIZE-len, "[ALL]    (%+3d, %+3d, %+3d) : (%+3d, %+3d, %+3d)\n",
			obj->offset[BMA255_AXIS_X]*mul + obj->cali_sw[BMA255_AXIS_X],
			obj->offset[BMA255_AXIS_Y]*mul + obj->cali_sw[BMA255_AXIS_Y],
			obj->offset[BMA255_AXIS_Z]*mul + obj->cali_sw[BMA255_AXIS_Z],
			tmp[BMA255_AXIS_X], tmp[BMA255_AXIS_Y], tmp[BMA255_AXIS_Z]);

		return len;
    }
}
/*                                                                            */
static ssize_t store_cali_value(struct device_driver *ddri, char *buf, size_t count)
{
	struct i2c_client *client = bma255_i2c_client;
	int err, x, y, z;
	int dat[BMA255_AXES_NUM];

	if(!strncmp(buf, "rst", 3))
	{
		if(err = BMA255_ResetCalibration(client))
		{
			GSE_ERR("reset offset err = %d\n", err);
		}
	}
	else if(3 == sscanf(buf, "0x%02X 0x%02X 0x%02X", &x, &y, &z))
	{
		dat[BMA255_AXIS_X] = x;
		dat[BMA255_AXIS_Y] = y;
		dat[BMA255_AXIS_Z] = z;
		if(err = BMA255_WriteCalibration(client, dat))
		{
			GSE_ERR("write calibration err = %d\n", err);
		}
	}
	else
	{
		GSE_ERR("invalid format\n");
	}

	return count;
}
#endif

/*                                                                            */
static ssize_t show_firlen_value(struct device_driver *ddri, char *buf)
{
#ifdef CONFIG_BMA255_LOWPASS
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);
	if(atomic_read(&obj->firlen))
	{
		int idx, len = atomic_read(&obj->firlen);
		GSE_LOG("len = %2d, idx = %2d\n", obj->fir.num, obj->fir.idx);

		for(idx = 0; idx < len; idx++)
		{
			GSE_LOG("[%5d %5d %5d]\n", obj->fir.raw[idx][BMA255_AXIS_X], obj->fir.raw[idx][BMA255_AXIS_Y], obj->fir.raw[idx][BMA255_AXIS_Z]);
		}

		GSE_LOG("sum = [%5d %5d %5d]\n", obj->fir.sum[BMA255_AXIS_X], obj->fir.sum[BMA255_AXIS_Y], obj->fir.sum[BMA255_AXIS_Z]);
		GSE_LOG("avg = [%5d %5d %5d]\n", obj->fir.sum[BMA255_AXIS_X]/len, obj->fir.sum[BMA255_AXIS_Y]/len, obj->fir.sum[BMA255_AXIS_Z]/len);
	}
	return snprintf(buf, PAGE_SIZE, "%d\n", atomic_read(&obj->firlen));
#else
	return snprintf(buf, PAGE_SIZE, "not support\n");
#endif
}
/*                                                                            */
static ssize_t store_firlen_value(struct device_driver *ddri, char *buf, size_t count)
{
#ifdef CONFIG_BMA255_LOWPASS
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);
	int firlen;

	if(1 != sscanf(buf, "%d", &firlen))
	{
		GSE_ERR("invallid format\n");
	}
	else if(firlen > C_MAX_FIR_LENGTH)
	{
		GSE_ERR("exceeds maximum filter length\n");
	}
	else
	{
		atomic_set(&obj->firlen, firlen);
		if(NULL == firlen)
		{
			atomic_set(&obj->fir_en, 0);
		}
		else
		{
			memset(&obj->fir, 0x00, sizeof(obj->fir));
			atomic_set(&obj->fir_en, 1);
		}
	}
#endif
	return count;
}
/*                                                                            */
static ssize_t show_trace_value(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	struct bma255_i2c_data *obj = obj_i2c_data;
	if (obj == NULL)
	{
		GSE_ERR("i2c_data obj is null!!\n");
		return 0;
	}

	res = snprintf(buf, PAGE_SIZE, "0x%04X\n", atomic_read(&obj->trace));
	return res;
}
/*                                                                            */
static ssize_t store_trace_value(struct device_driver *ddri, char *buf, size_t count)
{
	struct bma255_i2c_data *obj = obj_i2c_data;
	int trace;
	if (obj == NULL)
	{
		GSE_ERR("i2c_data obj is null!!\n");
		return 0;
	}

	if(1 == sscanf(buf, "0x%x", &trace))
	{
		atomic_set(&obj->trace, trace);
	}
	else
	{
		GSE_ERR("invalid content: '%s', length = %d\n", buf, count);
	}

	return count;
}
/*                                                                            */
static ssize_t show_status_value(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	struct bma255_i2c_data *obj = obj_i2c_data;
	if (obj == NULL)
	{
		GSE_ERR("i2c_data obj is null!!\n");
		return 0;
	}

	if(obj->hw)
	{
		len += snprintf(buf+len, PAGE_SIZE-len, "CUST: %d %d (%d %d)\n",
	            obj->hw->i2c_num, obj->hw->direction, obj->hw->power_id, obj->hw->power_vol);
	}
	else
	{
		len += snprintf(buf+len, PAGE_SIZE-len, "CUST: NULL\n");
	}
	return len;
}
/*                                                                            */
static ssize_t show_power_status_value(struct device_driver *ddri, char *buf)
{
	if(sensor_power)
		printk("G sensor is in work mode, sensor_power = %d\n", sensor_power);
	else
		printk("G sensor is in standby mode, sensor_power = %d\n", sensor_power);

	return 0;
}
/*                                                                            */
static ssize_t show_teststatus_value(struct device_driver *ddri, char *buf)
{
	struct i2c_client *client = bma255_i2c_client;

	if(client == NULL)
	{
		printk("i2c client is null!!\n");
		return 0;
	}

	return snprintf(buf, PAGE_SIZE, "%d\n", test_status);
}
/*                                                                            */
static ssize_t bma255_bandwidth_show(struct device_driver *ddri, char *buf)
{
	unsigned char data;
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);

	if (bma255_get_bandwidth(bma255->client, &data) < 0)
		return sprintf(buf, "Read error\n");

	return sprintf(buf, "%d\n", data);
}
/*                                                                            */
static ssize_t bma255_bandwidth_store(struct device_driver *ddri, char *buf, size_t count)
{
	unsigned long data;
	int error;
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);

	if (error = strict_strtoul(buf, 10, &data))
		return error;

	if (bma255_set_bandwidth(bma255->client, (unsigned char) data) < 0)
		return -EINVAL;

	return count;
}
/*                                                                            */
#ifdef CONFIG_MACH_LGE
static ssize_t bma255_eeprom_writing_show(struct device_driver *ddri, char *buf)
{
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);

	return sprintf(buf, "%d\n", atomic_read(&bma255->fast_calib_rslt));
}

static ssize_t bma255_eeprom_writing_store(struct device_driver *ddri, char *buf, size_t count)
{
	unsigned char offset_x, offset_y, offset_z;
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);

	if(bma255_get_offset_x(bma255->client, &offset_x) < 0)
		return -EINVAL;
	if(bma255_get_offset_y(bma255->client, &offset_y) < 0)
		return -EINVAL;
	if(bma255_get_offset_z(bma255->client, &offset_z) < 0)
		return -EINVAL;

	atomic_set(&bma255->fast_calib_rslt, 1);

	return count;
}

static int bma255_soft_reset(struct i2c_client *client)
{
	int comres = 0;
	unsigned char data = BMA255_EN_SOFT_RESET_VALUE;

	comres = bma255_smbus_write_byte(client, BMA255_EN_SOFT_RESET__REG, &data);

	return comres;
}
static ssize_t bma255_softreset_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = bma255_i2c_client;
	struct bma255_i2c_data *bma255 = i2c_get_clientdata(client);

	if (bma255_soft_reset(bma255->client) < 0)
		return -EINVAL;

	return count;
}
/*                                                                            */
/*                                                                */
static int selfCalibration = 1;
static ssize_t bma255_runCalibration_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	if(bma255_runCalibration() == BMA255_SUCCESS)
	{
		selfCalibration = BMA255_SUCCESS;
	}
	else
	{
		selfCalibration = 1;  //         
	}

	return count;
}

static ssize_t bma255_runCalibration_show(struct device_driver *ddri, char *buf)
{
	return sprintf(buf, "%d\n", selfCalibration);
}
/*                                                                */
#endif
/*                                                                            */
static DRIVER_ATTR(chipinfo,   S_IWUSR | S_IRUGO, show_chipinfo_value,   NULL);
static DRIVER_ATTR(cpsdata,    S_IWUSR | S_IRUGO, show_cpsdata_value,    NULL);
static DRIVER_ATTR(cpsopmode,  S_IWUSR | S_IRUGO, show_cpsopmode_value,    store_cpsopmode_value);
static DRIVER_ATTR(cpsrange, 	 S_IWUSR | S_IRUGO, show_cpsrange_value,     store_cpsrange_value);
static DRIVER_ATTR(cpsbandwidth, S_IWUSR | S_IRUGO, show_cpsbandwidth_value, store_cpsbandwidth_value);
static DRIVER_ATTR(sensordata, S_IWUSR | S_IRUGO, show_sensordata_value,    NULL);
static DRIVER_ATTR(cali,       S_IWUSR | S_IRUGO, show_cali_value,       store_cali_value);
static DRIVER_ATTR(firlen,     S_IWUSR | S_IRUGO, show_firlen_value,     store_firlen_value);
static DRIVER_ATTR(trace,      S_IWUSR | S_IRUGO, show_trace_value,      store_trace_value);
static DRIVER_ATTR(status,     S_IRUGO, show_status_value,        NULL);
static DRIVER_ATTR(powerstatus, S_IRUGO, show_power_status_value, NULL);
#ifdef CONFIG_MACH_LGE
static DRIVER_ATTR(softreset, S_IWUSR|S_IWGRP, NULL, bma255_softreset_store);
static DRIVER_ATTR(teststatus, S_IWUSR|S_IRUGO, show_teststatus_value, NULL);
static DRIVER_ATTR(fast_calibration_x, S_IRUGO|S_IWUSR|S_IWGRP, bma255_fast_calibration_x_show, bma255_fast_calibration_x_store);
static DRIVER_ATTR(fast_calibration_y, S_IRUGO|S_IWUSR|S_IWGRP, bma255_fast_calibration_y_show, bma255_fast_calibration_y_store);
static DRIVER_ATTR(fast_calibration_z, S_IRUGO|S_IWUSR|S_IWGRP, bma255_fast_calibration_z_show, bma255_fast_calibration_z_store);
static DRIVER_ATTR(run_fast_calibration, S_IRUGO|S_IWUSR|S_IWGRP, bma255_runCalibration_show, bma255_runCalibration_store);
static DRIVER_ATTR(eeprom_writing, S_IRUGO|S_IWUSR|S_IWGRP, bma255_eeprom_writing_show, bma255_eeprom_writing_store);
#endif
static DRIVER_ATTR(bandwidth, S_IRUGO|S_IWUSR|S_IWGRP, bma255_bandwidth_show, bma255_bandwidth_store);
/*                                                                            */
static struct driver_attribute *bma255_attr_list[] = {
	&driver_attr_chipinfo,     /*                */
	&driver_attr_sensordata,   /*                */
	&driver_attr_cali,         /*                     */
	&driver_attr_firlen,       /*                                         */
	&driver_attr_trace,        /*         */
	&driver_attr_status,
	&driver_attr_powerstatus,
	&driver_attr_cpsdata,	/*                                           */
	&driver_attr_cpsopmode,	/*                                             */
	&driver_attr_cpsrange,	/*                                            */
	&driver_attr_cpsbandwidth,	/*                                                */
	&driver_attr_bandwidth,
#ifdef CONFIG_MACH_LGE
	&driver_attr_softreset,
	&driver_attr_teststatus,
	&driver_attr_fast_calibration_x,
	&driver_attr_fast_calibration_y,
	&driver_attr_fast_calibration_z,
	&driver_attr_run_fast_calibration,
	&driver_attr_eeprom_writing,
#endif
};
/*                                                                            */
static int bma255_create_attr(struct device_driver *driver)
{
	int idx, err = 0;
	int num = (int)(sizeof(bma255_attr_list)/sizeof(bma255_attr_list[0]));
	if (driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		if(err = driver_create_file(driver, bma255_attr_list[idx]))
		{
			GSE_ERR("driver_create_file (%s) = %d\n", bma255_attr_list[idx]->attr.name, err);
			break;
		}
	}
	return err;
}
/*                                                                            */
static int bma255_delete_attr(struct device_driver *driver)
{
	int idx ,err = 0;
	int num = (int)(sizeof(bma255_attr_list)/sizeof(bma255_attr_list[0]));

	if(driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		driver_remove_file(driver, bma255_attr_list[idx]);
	}

	return err;
}

/*                                                                            */
int gsensor_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value, sample_delay;
	struct bma255_i2c_data *priv = (struct bma255_i2c_data*)self;
	hwm_sensor_data* gsensor_data;
	char buff[BMA255_BUFSIZE];

	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				GSE_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;

#ifdef CONFIG_MACH_LGE	/*                                                                 */
				if(value >= 70) {
					sample_delay = BMA255_BW_7_81HZ;	//      
				}
				else if(value >= 40) {
					sample_delay = BMA255_BW_15_63HZ;	//      
				}
				else if(value >= 20) {
					sample_delay = BMA255_BW_31_25HZ;	//      
				}
				else if(value >= 10) {
					sample_delay = BMA255_BW_62_5HZ;	//     
				}
				else {
					sample_delay = BMA255_BW_125HZ;		//     
				}
#else	//                       
				if(value <= 5)
				{
					sample_delay = BMA255_BW_200HZ;
				}
				else if(value <= 10)
				{
					sample_delay = BMA255_BW_100HZ;
				}
				else
				{
					sample_delay = BMA255_BW_50HZ;
				}
#endif	/*                                                                 */

				err = BMA255_SetBWRate(priv->client, sample_delay);
				if(err != BMA255_SUCCESS ) //              
				{
					GSE_ERR("Set delay parameter error!\n");
				}

				if(value >= 50)
				{
					atomic_set(&priv->filter, 0);
				}
				else
				{
				#if defined(CONFIG_BMA255_LOWPASS)
					priv->fir.num = 0;
					priv->fir.idx = 0;
					priv->fir.sum[BMA255_AXIS_X] = 0;
					priv->fir.sum[BMA255_AXIS_Y] = 0;
					priv->fir.sum[BMA255_AXIS_Z] = 0;
					atomic_set(&priv->filter, 1);
				#endif
				}
			}
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				GSE_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;
				if(((value == 0) && (sensor_power == false)) ||((value == 1) && (sensor_power == true)))
				{
					GSE_LOG("Gsensor device have updated!\n");
				}
				else
				{
					err = BMA255_SetPowerMode( priv->client, !sensor_power);
				}
			}
			break;

		case SENSOR_GET_DATA:
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				GSE_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				gsensor_data = (hwm_sensor_data *)buff_out;
				BMA255_ReadSensorData(priv->client, buff, BMA255_BUFSIZE);
				sscanf(buff, "%x %x %x", &gsensor_data->values[0],
					&gsensor_data->values[1], &gsensor_data->values[2]);
				gsensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
				gsensor_data->value_divide = 1000;
			}
			break;
		default:
			GSE_ERR("gsensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}

	return err;
}

/*                                                                             
                         
                                                                             */
static int bma255_open(struct inode *inode, struct file *file)
{
	file->private_data = bma255_i2c_client;

	if(file->private_data == NULL)
	{
		GSE_ERR("null pointer!!\n");
		return -EINVAL;
	}
	return nonseekable_open(inode, file);
}
/*                                                                            */
static int bma255_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*                                                                            */
static long bma255_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct i2c_client *client = (struct i2c_client*)file->private_data;
	struct bma255_i2c_data *obj = (struct bma255_i2c_data*)i2c_get_clientdata(client);
	char strbuf[BMA255_BUFSIZE];
	void __user *data;
	SENSOR_DATA sensor_data;
	long err = 0;
	int cali[3];
	uint32_t enable = 0;

	//           
	if(_IOC_DIR(cmd) & _IOC_READ)
	{
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	}
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
	{
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	}

	if(err)
	{
		GSE_ERR("access error: %08X, (%2d, %2d)\n", cmd, _IOC_DIR(cmd), _IOC_SIZE(cmd));
		return -EFAULT;
	}

	switch(cmd)
	{
		#if 0
		case GSENSOR_IOCTL_SET_ENABLE:
			data = (void __user *) arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;
			}

			if(copy_from_user(&enable, data, sizeof(enable)))
			{
				return -EFAULT;
			}
			else
			{
				if(enable == 1)
				{
					BMA255_SetPowerMode(obj->client, 1);
				}
				else if(enable == 0)
				{
					BMA255_SetPowerMode(obj->client, 0);
				}
			}
			break;
		#endif
		case GSENSOR_IOCTL_INIT:
			bma255_init_client(client, 0);
			break;

		case GSENSOR_IOCTL_READ_CHIPINFO:
			data = (void __user *) arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;
			}

			BMA255_ReadChipInfo(client, strbuf, BMA255_BUFSIZE);
			if(copy_to_user(data, strbuf, strlen(strbuf)+1))
			{
				err = -EFAULT;
				break;
			}
			break;

		case GSENSOR_IOCTL_READ_SENSORDATA:
			data = (void __user *) arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;
			}

			BMA255_ReadSensorData(client, strbuf, BMA255_BUFSIZE);
			if(copy_to_user(data, strbuf, strlen(strbuf)+1))
			{
				err = -EFAULT;
				break;
			}
			break;

		case GSENSOR_IOCTL_READ_GAIN:
			data = (void __user *) arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;
			}

			if(copy_to_user(data, &gsensor_gain, sizeof(GSENSOR_VECTOR3D)))
			{
				err = -EFAULT;
				break;
			}
			break;

		case GSENSOR_IOCTL_READ_RAW_DATA:
			data = (void __user *) arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;
			}
			BMA255_ReadRawData(client, strbuf);
			if(copy_to_user(data, &strbuf, strlen(strbuf)+1))
			{
				err = -EFAULT;
				break;
			}
			break;

		case GSENSOR_IOCTL_SET_CALI:
			data = (void __user*)arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;
			}
			if(copy_from_user(&sensor_data, data, sizeof(sensor_data)))
			{
				err = -EFAULT;
				break;
			}
			if(atomic_read(&obj->suspend))
			{
				GSE_ERR("Perform calibration in suspend state!!\n");
				err = -EINVAL;
			}
			else
			{
#ifdef CONFIG_LGE_MACH
				if(bma255_set_offset_x(obj->client, (unsigned char)sensor_data.x) < 0)
					return -EINVAL;
				if(bma255_set_offset_y(obj->client, (unsigned char)sensor_data.y) < 0)
					return -EINVAL;
				if(bma255_set_offset_z(obj->client, (unsigned char)sensor_data.z) < 0)
					return -EINVAL;
#else	//                  
				cali[BMA255_AXIS_X] = sensor_data.x * obj->reso->sensitivity / GRAVITY_EARTH_1000;
				cali[BMA255_AXIS_Y] = sensor_data.y * obj->reso->sensitivity / GRAVITY_EARTH_1000;
				cali[BMA255_AXIS_Z] = sensor_data.z * obj->reso->sensitivity / GRAVITY_EARTH_1000;
				err = BMA255_WriteCalibration(client, cali);
#endif
			}
			break;

		case GSENSOR_IOCTL_CLR_CALI:
			err = BMA255_ResetCalibration(client);
			break;

		case GSENSOR_IOCTL_GET_CALI:
			data = (void __user*)arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;
			}
#ifdef CONFIG_LGE_MACH
			if(bma255_get_offset_x(obj->client, (unsigned char)sensor_data.x) < 0)
				return -EINVAL;
			if(bma255_get_offset_y(obj->client, (unsigned char)sensor_data.y) < 0)
				return -EINVAL;
			if(bma255_get_offset_z(obj->client, (unsigned char)sensor_data.z) < 0)
				return -EINVAL;
#else	//                  
			if(err = BMA255_ReadCalibration(client, cali))
			{
				break;
			}

			sensor_data.x = cali[BMA255_AXIS_X] * GRAVITY_EARTH_1000 / obj->reso->sensitivity;
			sensor_data.y = cali[BMA255_AXIS_Y] * GRAVITY_EARTH_1000 / obj->reso->sensitivity;
			sensor_data.z = cali[BMA255_AXIS_Z] * GRAVITY_EARTH_1000 / obj->reso->sensitivity;
#endif
			if(copy_to_user(data, &sensor_data, sizeof(sensor_data)))
			{
				err = -EFAULT;
				break;
			}
			break;

		default:
			GSE_ERR("unknown IOCTL: 0x%08x\n", cmd);
			err = -ENOIOCTLCMD;
			break;

	}

	return err;
}


/*                                                                            */
static struct file_operations bma255_fops = {
	//                     
	.open = bma255_open,
	.release = bma255_release,
	.unlocked_ioctl = bma255_unlocked_ioctl,
};
/*                                                                            */
static struct miscdevice bma255_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "gsensor",
	.fops = &bma255_fops,
};
/*                                                                            */
#ifndef CONFIG_HAS_EARLYSUSPEND
/*                                                                            */
static int bma255_suspend(struct i2c_client *client, pm_message_t msg)
{
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);
	int err = 0;

	GSE_FUN();

	if(msg.event == PM_EVENT_SUSPEND)
	{
		if(obj == NULL)
		{
			GSE_ERR("null pointer!!\n");
			return -EINVAL;
		}
		atomic_set(&obj->suspend, 1);
		if(err = BMA255_SetPowerMode(obj->client, false))
		{
			GSE_ERR("write power control fail!!\n");
			return;
		}
		BMA255_power(obj->hw, 0);
	}
	return err;
}
/*                                                                            */
static int bma255_resume(struct i2c_client *client)
{
	struct bma255_i2c_data *obj = i2c_get_clientdata(client);
	int err;

    GSE_FUN();

	if(obj == NULL)
	{
		GSE_ERR("null pointer!!\n");
		return -EINVAL;
	}

	BMA255_power(obj->hw, 1);
	if(err = bma255_init_client(client, 0))
	{
		GSE_ERR("initialize client fail!!\n");
		return err;
	}
	atomic_set(&obj->suspend, 0);

	return 0;
}
/*                                                                            */
#else /*                                   */
/*                                                                            */
static void bma255_early_suspend(struct early_suspend *h)
{
	struct bma255_i2c_data *obj = container_of(h, struct bma255_i2c_data, early_drv);
	int err;

	GSE_FUN();

	if(obj == NULL)
	{
		GSE_ERR("null pointer!!\n");
		return;
	}
	atomic_set(&obj->suspend, 1);
	if(err = BMA255_SetPowerMode(obj->client, false))
	{
		GSE_ERR("write power control fail!!\n");
		return;
	}

	sensor_power = false;

	BMA255_power(obj->hw, 0);
}
/*                                                                            */
static void bma255_late_resume(struct early_suspend *h)
{
	struct bma255_i2c_data *obj = container_of(h, struct bma255_i2c_data, early_drv);
	int err;

	GSE_FUN();

	if(obj == NULL)
	{
		GSE_ERR("null pointer!!\n");
		return;
	}

	BMA255_power(obj->hw, 1);
	if(err = bma255_init_client(obj->client, 0))
	{
		GSE_ERR("initialize client fail!!\n");
		return;
	}
	atomic_set(&obj->suspend, 0);
}
/*                                                                            */
#endif /*                       */
/*                                                                            */
static int bma255_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct i2c_client *new_client;
	struct bma255_i2c_data *obj;
	struct hwmsen_object sobj;
	int err = 0;

	GSE_FUN();

	if(!(obj = kzalloc(sizeof(*obj), GFP_KERNEL)))
	{
		err = -ENOMEM;
		goto exit;
	}

	memset(obj, 0, sizeof(struct bma255_i2c_data));

	obj->hw = get_cust_acc_hw();

	if(err = hwmsen_get_convert(obj->hw->direction, &obj->cvt))
	{
		GSE_ERR("invalid direction: %d\n", obj->hw->direction);
		goto exit;
	}

	obj_i2c_data = obj;
	obj->client = client;
	new_client = obj->client;
	i2c_set_clientdata(new_client,obj);

	atomic_set(&obj->trace, 0);
	atomic_set(&obj->suspend, 0);

#ifdef CONFIG_BMA255_LOWPASS
	if(obj->hw->firlen > C_MAX_FIR_LENGTH)
	{
		atomic_set(&obj->firlen, C_MAX_FIR_LENGTH);
	}
	else
	{
		atomic_set(&obj->firlen, obj->hw->firlen);
	}

	if(atomic_read(&obj->firlen) > 0)
	{
		atomic_set(&obj->fir_en, 1);
	}

#endif

	bma255_i2c_client = new_client;

	if(err = bma255_init_client(new_client, 1))
	{
		goto exit_init_failed;
	}


	if(err = misc_register(&bma255_device))
	{
		GSE_ERR("bma255_device register failed\n");
		goto exit_misc_device_register_failed;
	}

	if(err = bma255_create_attr(&bma255_gsensor_driver.driver))
	{
		GSE_ERR("create attribute err = %d\n", err);
		goto exit_create_attr_failed;
	}

	sobj.self = obj;
    sobj.polling = 1;
    sobj.sensor_operate = gsensor_operate;
	if(err = hwmsen_attach(ID_ACCELEROMETER, &sobj))
	{
		GSE_ERR("attach fail = %d\n", err);
		goto exit_kfree;
	}
	printk("HWSEN> hwmsen_attach good \n");
#ifdef CONFIG_HAS_EARLYSUSPEND
	obj->early_drv.level    = EARLY_SUSPEND_LEVEL_DISABLE_FB - 1,
	obj->early_drv.suspend  = bma255_early_suspend,
	obj->early_drv.resume   = bma255_late_resume,
	register_early_suspend(&obj->early_drv);
#endif

	GSE_LOG("%s: OK\n", __func__);
	return 0;

exit_create_attr_failed:
	misc_deregister(&bma255_device);
exit_misc_device_register_failed:
exit_init_failed:
	//                              
exit_kfree:
	kfree(obj);
exit:
	GSE_ERR("%s: err = %d\n", __func__, err);
	return err;
}

/*                                                                            */
static int bma255_i2c_remove(struct i2c_client *client)
{
	int err = 0;

	if(err = bma255_delete_attr(&bma255_gsensor_driver.driver))
	{
		GSE_ERR("bma150_delete_attr fail: %d\n", err);
	}

	if(err = misc_deregister(&bma255_device))
	{
		GSE_ERR("misc_deregister fail: %d\n", err);
	}

	if(err = hwmsen_detach(ID_ACCELEROMETER))
    {
		GSE_ERR("hwmsen_detach fail: %d\n", err);
    }

    bma255_i2c_client = NULL;
	i2c_unregister_device(client);
	kfree(i2c_get_clientdata(client));
	return 0;
}
/*                                                                            */
static int bma255_probe(struct platform_device *pdev)
{
	struct acc_hw *hw = get_cust_acc_hw();

	GSE_FUN();

	BMA255_power(hw, 1);
	if(i2c_add_driver(&bma255_i2c_driver))
	{
		GSE_ERR("add driver error\n");
		return -1;
	}
	return 0;
}
/*                                                                            */
static int bma255_remove(struct platform_device *pdev)
{
    struct acc_hw *hw = get_cust_acc_hw();

    GSE_FUN();
    BMA255_power(hw, 0);
    i2c_del_driver(&bma255_i2c_driver);
    return 0;
}
/*                                                                            */
static struct platform_driver bma255_gsensor_driver = {
	.probe      = bma255_probe,
	.remove     = bma255_remove,
	.driver     = {
		.name  = "gsensor",
#ifdef CONFIG_OF_DT
						.of_match_table = gsensor_of_match,
#endif
	}
};

/*                                                                            */
static int __init bma255_init(void)
{
	struct acc_hw *hw = get_cust_acc_hw();

	GSE_FUN();
	i2c_register_board_info(hw->i2c_num, &bma255_i2c_info, 1);
	if(platform_driver_register(&bma255_gsensor_driver))
	{
		GSE_ERR("failed to register driver");
		return -ENODEV;
	}
	return 0;
}
/*                                                                            */
static void __exit bma255_exit(void)
{
	GSE_FUN();
	platform_driver_unregister(&bma255_gsensor_driver);
}
/*                                                                            */
module_init(bma255_init);
module_exit(bma255_exit);
/*                                                                            */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("BMA255 I2C driver");
MODULE_AUTHOR("hongji.zhou@bosch-sensortec.com");
