/* drivers/i2c/chips/ms5607.c - MS5607 motion sensor driver
 *
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
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
//                 
 

#include <cust_baro.h>
#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>
#include "ms5607.h"
#include <linux/hwmsen_helper.h>

#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>

#define POWER_NONE_MACRO MT65XX_POWER_NONE

/*                                                                            */
#define I2C_DRIVERID_MS5607 345
/*                                                                            */
#define DEBUG 1
/*                                                                            */
    
/*                                                                            */

#define MS5607_DEV_NAME        "MS5607"
/*                                                                            */
static const struct i2c_device_id ms5607_i2c_id[] = {{MS5607_DEV_NAME,0},{}};
static struct i2c_board_info __initdata i2c_ms5607 = {I2C_BOARD_INFO("MS5607", 0xee)};
/*                                                 */
//                                                                                                     
//                                                                            
//                                                                                     

/*                                                                            */
static int ms5607_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id); 
static int ms5607_i2c_remove(struct i2c_client *client);
//                                                                                               

/*                                                                            */
//                                                                       


/*                                                                              */
typedef enum {
    BAR_TRC_READ  = 0x01,
    BAR_TRC_RAWDATA = 0x02,
    BAR_TRC_IOCTL   = 0x04,
    BAR_TRC_FILTER  =0x08,
} BAR_TRC;
/*                                                                            */

/*                                                                            */
#define MS5607_DATA_T 0
#define MS5607_DATA_P 1

#define MS5607_D1 0
#define MS5607_D2 1

#define C_MAX_FIR_LENGTH (32)
#define MS5607_DATA_NUM 2
#define CONFIG_MS5607_LOWPASS   /*                               */     

struct data_filter {
    u32 raw[C_MAX_FIR_LENGTH][MS5607_DATA_NUM];
    int sum[MS5607_DATA_NUM];
    int num;
    int idx;
};


struct ms5607_i2c_data {
    struct i2c_client *client;
    struct baro_hw *hw;
    struct hwmsen_convert   cvt;
    
    /*    */
    struct data_resolution *reso;
    atomic_t                trace;
    atomic_t                suspend;
    /*    */
    u32 D1; //                                     
    u32 D2; //                                        
    unsigned int C[8]; //                         
    int P; //                           
    int T; //                              
    int dT; //                                                   
    long long OFF; //                             
    long long SENS; //                                  
    unsigned char n_crc; //                      
    u32   data[2];
	atomic_t				filter;
#if defined(CONFIG_MS5607_LOWPASS)
    atomic_t                firlen;
    atomic_t                fir_en;
    struct data_filter      fir;
#endif 
	
    /*             */
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend    early_drv;
#endif     
};
/*                                                                            */
static struct i2c_driver ms5607_i2c_driver = {
    .driver = {
 //                                     
        .name           = MS5607_DEV_NAME,
    },
	.probe      		= ms5607_i2c_probe,
	.remove    			= ms5607_i2c_remove,
//                                
#if !defined(CONFIG_HAS_EARLYSUSPEND)    
    .suspend            = ms5607_suspend,
    .resume             = ms5607_resume,
#endif
	.id_table = ms5607_i2c_id,
//                                   
};

/*                                                                            */
static struct i2c_client *ms5607_i2c_client = NULL;
static struct platform_driver ms5607_barometer_driver;
static struct ms5607_i2c_data *obj_i2c_data = NULL;
static bool sensor_power = false;

/*                                                                            */
#define BAR_TAG                  "[Barometer] "
#define BAR_FUN(f)               printk(KERN_INFO BAR_TAG"%s\n", __FUNCTION__)
#define BAR_ERR(fmt, args...)    printk(KERN_ERR BAR_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define BAR_LOG(fmt, args...)    printk(KERN_INFO BAR_TAG fmt, ##args)
/*                                                                            */
/*
                                       
            
           
           
           
                      
  
*/
/*                                                                            */

/*                                                                             */


static void MS5607_power(struct baro_hw *hw, unsigned int on) 
{
	static unsigned int power_on = 0;

	if(hw->power_id != POWER_NONE_MACRO)		//                  
	{        
		BAR_LOG("power %s\n", on ? "on" : "off");
		if(power_on == on)	//                        
		{
			BAR_LOG("ignore power control: %d\n", on);
		}
		else if(on)	//         
		{
			if(!hwPowerOn(hw->power_id, hw->power_vol, "MS5607"))
			{
				BAR_ERR("power on fails!!\n");
			}
		}
		else	//          
		{
			if (!hwPowerDown(hw->power_id, "MS5607"))
			{
				BAR_ERR("power off fail!!\n");
			}			  
		}
	}
	power_on = on;    
}


static int ms5607_i2c_send(struct i2c_client *client,char cmd)
{
    u8 databuf[10];    
	int res = 0;
	memset(databuf, 0, sizeof(u8)*10);    
	databuf[0] = cmd;    
	res = i2c_master_send(client, databuf, 0x1);
	if(res <= 0)
	{
		BAR_ERR("ms5607_i2c_send fail!!\n");
		res = -1;
	}
	return res;
}

static int ms5607_i2c_recv(struct i2c_client *client,u8 *data,u8 len)
{
    int res = 0;
    if(NULL == data)
    {
      return -1;
	  BAR_ERR("ms5607_i2c_recv NULL pointer!!\n");
    }
	  
	res = i2c_master_recv(client, data, len);
	
	if(res <= 0)
	{
		BAR_ERR("ms5607_i2c_recv fail!!\n");
		res = -1;
	}
	return res;
}

//                                                        
//                            
//                   
//                                                        
static unsigned long ms5607_cmd_adc(struct i2c_client *client,char cmd)
{
    int ret;
    unsigned long temp=0;
	u8 databuf[10];
	memset(databuf, 0, sizeof(u8)*10); 
	
	ret = ms5607_i2c_send(client,CMD_ADC_CONV+cmd);
	if(ret <= 0)
	{
	   BAR_ERR("i2c error\n");
	   return -1;
	}
	
    switch (cmd & 0x0f) //                               
    {
     case CMD_ADC_256 : 
	 	udelay(900); break;
     case CMD_ADC_512 :
	 	mdelay(3); break;
     case CMD_ADC_1024: 
	 	mdelay(4); break;
     case CMD_ADC_2048: 
	 	mdelay(6); break;
     case CMD_ADC_4096: 
	 	mdelay(10); break;
    }
	ret = ms5607_i2c_send(client,CMD_ADC_READ);
	if(ret <= 0)
	{
	   BAR_ERR("i2c error\n");
	   return -1;
	}
 
    ret = ms5607_i2c_recv(client,databuf,3);
	if(ret <= 0)
	{
	   BAR_ERR("i2c error\n");
	   return -1;
	}
	temp = databuf[0]<<16 | databuf[1]<<8 |databuf[0];
	return temp;
}
//                                                        


//                                   
//                                                        
static unsigned int ms5607_cmd_prom(struct i2c_client *client ,u8 coef_num)
{
    int ret;
    unsigned int rC=0;
	u8 databuf[10];
	memset(databuf, 0, sizeof(u8)*10); 
	
	ret = ms5607_i2c_send(client,CMD_PROM_RD+coef_num*2);
	if(ret <= 0)
	{
	  BAR_ERR("ms5607_cmd_prom error!\n");
	  return -1;
	}
	
    ret = ms5607_i2c_recv(client,databuf,2);
	if(ret <= 0)
	{
	  BAR_ERR("ms5607_cmd_prom error!\n");
	  return -1;
	}

	rC = databuf[0]<<8 | databuf[1];
    return rC;
}


/*                                                                            */

//                                                        
//                            
//               
//                                                        
static unsigned char crc4(unsigned int n_prom[])
{
    int cnt; //               
    unsigned int n_rem; //             
    unsigned int crc_read; //                          
    unsigned char n_bit;
    n_rem = 0x00;
    crc_read=n_prom[7]; //             
    n_prom[7]=(0xFF00 & (n_prom[7])); //                         
    for (cnt = 0; cnt < 16; cnt++) //                                
    {//                  
      if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
      else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
      for (n_bit = 8; n_bit > 0; n_bit--)
      {
        if (n_rem & (0x8000))
        {
          n_rem = (n_rem << 1) ^ 0x3000;
        }
        else
        {
          n_rem = (n_rem << 1);
        }
      }
    }
    n_rem= (0x000F & (n_rem >> 12)); //                                 
    n_prom[7]=crc_read; //                                           
    return (n_rem ^ 0x0);
}


/*                                                                            */
static int MS5607_ReadData(struct i2c_client *client, u32 data[2])
{
	struct ms5607_i2c_data *priv = i2c_get_clientdata(client);
	int err =0;
	s32 T2=0;
	s64 OFF2=0;
	s64 SENS2=0;
	int idx, firlen;
	struct ms5607_i2c_data *obj = obj_i2c_data; 

	if(NULL == client)
	{
		err = -EINVAL;
	}

	obj->D2=ms5607_cmd_adc(client, CMD_ADC_D2+CMD_ADC_4096); //        
    obj->D1=ms5607_cmd_adc(client, CMD_ADC_D1+CMD_ADC_4096); //        

	#ifdef CONFIG_MS5607_LOWPASS
		if(atomic_read(&priv->filter))
		{
			if(atomic_read(&priv->fir_en) && !atomic_read(&priv->suspend))
			{
				firlen = atomic_read(&priv->firlen);   
				if(priv->fir.num < firlen)
				{                
					priv->fir.raw[priv->fir.num][MS5607_D1] = obj->D1;
					priv->fir.raw[priv->fir.num][MS5607_D2] = obj->D2;
					
					priv->fir.sum[MS5607_D1] += obj->D1;
					priv->fir.sum[MS5607_D2] += obj->D2;
					
					if(atomic_read(&priv->trace) & BAR_TRC_FILTER)
					{
						BAR_LOG("add [%2d] [%5d %5d ] => [%5d %5d ]\n", priv->fir.num,
							priv->fir.raw[priv->fir.num][MS5607_D1], priv->fir.raw[priv->fir.num][MS5607_D2], 
							priv->fir.sum[MS5607_D1], priv->fir.sum[MS5607_D2]);
					}
					priv->fir.num++;
					priv->fir.idx++;
				}
				else
				{
					idx = priv->fir.idx % firlen;
					priv->fir.sum[MS5607_D1] -= priv->fir.raw[idx][MS5607_D1];
					priv->fir.sum[MS5607_D2] -= priv->fir.raw[idx][MS5607_D2];
					
					priv->fir.raw[idx][MS5607_D1] = obj->D1;
					priv->fir.raw[idx][MS5607_D2] = obj->D2;
					
					priv->fir.sum[MS5607_D1] += obj->D1;
					priv->fir.sum[MS5607_D2] += obj->D2;
					
					priv->fir.idx++;
					obj->D1 = (priv->fir.sum[MS5607_D1])/firlen;
					obj->D2 = (priv->fir.sum[MS5607_D2])/firlen;
					
					if(atomic_read(&priv->trace) & BAR_TRC_FILTER)
					{
						BAR_LOG("add [%2d] [%5x %5x ] => [%5x %5x ] : [%5x %5x ]\n", idx,
						priv->fir.raw[idx][MS5607_D1], priv->fir.raw[idx][MS5607_D2], 
						priv->fir.sum[MS5607_D1], priv->fir.sum[MS5607_D2], 
						obj->D1, obj->D2);
					}
				}
			}
		}	
#endif         

	//                                                                          
	
    obj->dT=obj->D2 - obj->C[5]*(1<<8);
    obj->OFF=(s64)(obj->C[2])*(s64)(1<<17) + ((s64)(obj->dT)*(s64)(obj->C[4])) /(s64)(1<<6);
    obj->SENS=(s64)(obj->C[1])*(s64)(1<<16)+ ((s64)(obj->dT) * (s64)(obj->C[3]))/(s64)(1<<7);

    obj->T=(2000 + ((s64)obj->dT*(u64)obj->C[6])/(s64)(1<<23));
    //                                                                         
	
#if 1
	//                 
	T2=0;
	OFF2=0;
	SENS2=0;
	if(obj->T < 2000)
	{
	   //               
	   if(obj->T <1500)
	   {
	      //                    
	      //                                  
	      OFF2 = OFF2 + (s64)15*(s64)(obj->T+1500)*(s64)(obj->T+1500);
	      SENS2 =SENS2 + (s64)8*(s64)(obj->T+1500)*(s64)(obj->T+1500);
	   }
	   else
	   {
	     //                              
	      T2 = (s64)((obj->dT)*(obj->dT))/(s64)(1<<31);
		  //                                                                        
	      OFF2 =(s64)61*(s64)(obj->T-2000)*(s64)(obj->T-2000)/(s64)(1<<4);
	      SENS2 =(s64)2*(s64)(obj->T-2000)*(s64)(obj->T-2000); 
	   }   
	}
	else
	{
	   //                 
	   //                          
	}

	if(atomic_read(&obj->trace) & BAR_TRC_READ)//                                       
	{
	    
		BAR_LOG("D2:%d  D1:%d\n", obj->D2,obj->D1);
		BAR_LOG("dT:%x  \n", obj->dT);
		BAR_LOG("OFF:%llx  \n", obj->OFF);
		BAR_LOG("SENS:%llx  \n", obj->SENS);
		BAR_LOG("T:%d \n", obj->T);
		BAR_LOG("OFF2:%llx  \n", OFF2);
		BAR_LOG("SENS2:%llx  \n", SENS2);
		BAR_LOG("T2:%d \n", T2);
			
	}

	//                         
	obj->T = obj->T - T2;
	obj->OFF = obj->OFF - OFF2;
	obj->SENS = obj->SENS - SENS2;
	//                      
	obj->P=((((s64)(obj->D1)*obj->SENS)/(s64)(1<<21)-obj->OFF)/(s64)(1<<15));

#endif
	//                                         
    //
    if( obj->T < -4500 || obj->T >8000 )
    {
        BAR_ERR("error Temprature extend change scale!!\n" );
        BAR_LOG("D2:%d  D1:%d\n", obj->D2,obj->D1);
		BAR_LOG("dT:%x  \n", obj->dT);
		BAR_LOG("OFF:%llx  \n", obj->OFF);
		BAR_LOG("SENS:%llx  \n", obj->SENS);
		BAR_LOG("T:%d \n", obj->T);
		BAR_LOG("OFF2:%llx  \n", OFF2);
		BAR_LOG("SENS2:%llx  \n", SENS2);
		BAR_LOG("T2:%d \n", T2);
		return -1;
    }
    if( obj->P <30000 || obj->P >120000 )
    {
        BAR_ERR("error pressure extend change scale!!\n" );
        BAR_LOG("D2:%d  D1:%d\n", obj->D2,obj->D1);
		BAR_LOG("dT:%x  \n", obj->dT);
		BAR_LOG("OFF:%llx  \n", obj->OFF);
		BAR_LOG("SENS:%llx  \n", obj->SENS);
		BAR_LOG("T:%d \n", obj->T);
		BAR_LOG("OFF2:%llx  \n", OFF2);
		BAR_LOG("SENS2:%llx  \n", SENS2);
		BAR_LOG("T2:%d \n", T2);
		return -1;
    }
	
    data[MS5607_DATA_T] = obj->T;
	data[MS5607_DATA_P] = obj->P;
 
	if(atomic_read(&obj->trace) & BAR_TRC_RAWDATA)//                                          
	{
		BAR_LOG("[T:%d  P:%d\n", data[MS5607_DATA_T], data[MS5607_DATA_P]);
	}
	
    
	return err;
}
/*                                                                            */

static int MS5607_ReadPressureData(struct i2c_client *client, char *buf, int bufsize)
{
    struct ms5607_i2c_data *obj = (struct ms5607_i2c_data*)i2c_get_clientdata(client);
	
	//            
	int res = 0;

	if(NULL == buf)
	{
		return -1;
	}
	if(NULL == client)
	{
		*buf = 0;
		return -2;
	}

	if((res = MS5607_ReadData(client, obj->data)))
	{        
		BAR_ERR("read data eror %d", res);
		return -3;
	}
	else
	{
	    sprintf(buf, "%04x  ",obj->data[MS5607_DATA_P]);
		if(atomic_read(&obj->trace) & BAR_TRC_IOCTL)
		{
			BAR_LOG("BAR_sensor data: %s!\n", buf);
		}
	}
	return res;
}


/*                                                                            */

static int MS5607_ReadTempData(struct i2c_client *client, char *buf, int bufsize)
{
    struct ms5607_i2c_data *obj = (struct ms5607_i2c_data*)i2c_get_clientdata(client);
	//            
	int res = 0;
	
	if(NULL == buf)
	{
		return -1;
	}
	if(NULL == client)
	{
		*buf = 0;
		return -2;
	}

	if((res = MS5607_ReadData(client, obj->data)))
	{        
		BAR_ERR("I2C error: ret value=%d", res);
		return -3;
	}
	else
	{
	    sprintf(buf, "%04x  ",obj->data[MS5607_DATA_T]);
		if(atomic_read(&obj->trace) & BAR_TRC_IOCTL)
		{
			BAR_LOG("Temp_sensor data: %s!\n", buf);
		}
	}
	return res;
}

/*                                                                            */
static int ms5607_cmd_reset(struct i2c_client *client)
{
    int err;
	err  = ms5607_i2c_send(client,CMD_RESET); //                    
	if(err <= 0)
	{
	  BAR_ERR(" ms5607_cmd_reset err %d !\n",err);
	}
    mdelay(3); //                                   
	
	return err;    
}
/*                                                                            */

/*                                                                            */


static int MS5607_Init(struct i2c_client *client, int reset_cali)
{
	struct ms5607_i2c_data *obj = obj_i2c_data;
	//            
	int i=0;
//                                              
    BAR_LOG(" ms5607 addr %x!\n",client->addr);
	while( (ms5607_cmd_reset(client)<=0) && i< 3)//                            
	{
	  i++;
	  BAR_LOG(" retry  %d times\n",i);
	  if(3==i)
	  {
	    BAR_ERR(" retry failed");
	    return MS5607_ERR_I2C;
	  }
	}
	for (i=0;i<8;i++)
	{ 
	   obj->C[i]=ms5607_cmd_prom(client,i);
	   if(-1 ==  obj->C[i])
	   {
	      obj->C[i]=ms5607_cmd_prom(client,i);//             
	      BAR_LOG(" warning C[%d] %x!\n",i,obj->C[i]);
		  if(-1 ==  obj->C[i])
		  {
		    return -1;
			BAR_ERR(" read C[i] error ");
		  }
	   }
	   BAR_LOG(" ms5607 C[%d] %x!\n",i,obj->C[i]);
	} //                  
    obj->n_crc = crc4(obj->C); //                  
	
	return MS5607_SUCCESS;
}
/*                                                                            */

/*                                                                            */
/*
                                                                         
   
 
                                               
                             
                                    
                   
  
                                    
           
  
 
                                                        
 
 
*/
/*                                                                            */
static ssize_t show_sensordata_value(struct device_driver *ddri, char *buf)
{
	struct i2c_client *client = ms5607_i2c_client;
	
	char strbuf[MS5607_BUFSIZE];
	
	if(NULL == client)
	{
		BAR_ERR("i2c client is null!!\n");
		return 0;
	}
	
	return snprintf(buf, PAGE_SIZE, "%s\n", strbuf);            
}
/*                                                                            */

/*                                                                            */

/*                                                                            */

/*                                                                            */
static ssize_t show_trace_value(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	struct ms5607_i2c_data *obj = obj_i2c_data;
	BAR_LOG(" show_trace_value \n");
	if (obj == NULL)
	{
		BAR_ERR("i2c_data obj is null!!\n");
		return 0;
	}
	
	res = snprintf(buf, PAGE_SIZE, "0x%04X\n", atomic_read(&obj->trace));     
	return res;    
}
/*                                                                            */
static ssize_t store_trace_value(struct device_driver *ddri, const char *buf, size_t count)
{
    
	struct ms5607_i2c_data *obj = obj_i2c_data;
	int trace;
	BAR_LOG(" store_trace_value \n");
	if (obj == NULL)
	{
		BAR_ERR("i2c_data obj is null!!\n");
		return 0;
	}
	
	if(1 == sscanf(buf, "0x%x", &trace))
	{
		atomic_set(&obj->trace, trace);
	}	
	else
	{
		BAR_ERR("invalid content: '%s', length = %d\n", buf, count);
	}
	
	return count;    
}
/*                                                                            */
static ssize_t show_status_value(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;  
	BAR_LOG(" show_status_value \n");
	
	return len;    
}
/*                                                                            */
//                                                                                  
static DRIVER_ATTR(sensordata,           S_IRUGO, show_sensordata_value,    NULL);
static DRIVER_ATTR(trace,      S_IWUSR | S_IRUGO, show_trace_value,         store_trace_value);
static DRIVER_ATTR(status,               S_IRUGO, show_status_value,        NULL);
/*                                                                            */
static struct driver_attribute *ms5607_attr_list[] = {
	//                                               
	&driver_attr_sensordata,   /*                */
	&driver_attr_trace,        /*         */
	&driver_attr_status,       /*                 */     
};
/*                                                                            */
static int ms5607_create_attr(struct device_driver *driver) 
{
	int idx, err = 0;
	int num = (int)(sizeof(ms5607_attr_list)/sizeof(ms5607_attr_list[0]));
	if (driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		if((err = driver_create_file(driver, ms5607_attr_list[idx])))
		{            
			BAR_ERR("driver_create_file (%s) = %d\n", ms5607_attr_list[idx]->attr.name, err);
			break;
		}
	}    
	return err;
}
/*                                                                            */
static int ms5607_delete_attr(struct device_driver *driver)
{
	int idx ,err = 0;
	int num = (int)(sizeof(ms5607_attr_list)/sizeof(ms5607_attr_list[0]));

	if(driver == NULL)
	{
		return -EINVAL;
	}
	

	for(idx = 0; idx < num; idx++)
	{
		driver_remove_file(driver, ms5607_attr_list[idx]);
	}
	

	return err;
}

/*                                                                            */
int barometer_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;	
	struct ms5607_i2c_data *priv = (struct ms5607_i2c_data*)self;
	hwm_sensor_data* barometer_data;
	char buff[MS5607_BUFSIZE];
	
	switch (command)
	{
		case SENSOR_DELAY:
			//                    
			//                                     
			//                                                 
			break;

		case SENSOR_ENABLE:
			BAR_LOG("sensor enable barometer\n");
			//                                                           
			//                                                                                     
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				BAR_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;
				if(value == 0)
				{
					//                   
					memset(&(priv->fir), 0, sizeof(struct data_filter));
				}
				
			}
			break;

		case SENSOR_GET_DATA:
			//                                      
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				BAR_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				barometer_data = (hwm_sensor_data *)buff_out;
				err = MS5607_ReadPressureData(priv->client, buff, MS5607_BUFSIZE);
				if(err)
				{
				  BAR_ERR("get sensor data parameter error!\n");
				  return -1;
				}
				sscanf(buff, "%x %x %x", &barometer_data->values[0], 
					&barometer_data->values[1], &barometer_data->values[2]);				
				barometer_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;				
				barometer_data->value_divide = 100;
				//                                                                                                              
			}
			break;
		default:
			BAR_ERR("temperature operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}

/*                                                                            */
int temperature_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;	
	struct ms5607_i2c_data *priv = (struct ms5607_i2c_data*)self;
	hwm_sensor_data* temperature_data;
	char buff[MS5607_BUFSIZE];
	
	switch (command)
	{
		case SENSOR_DELAY:
			//                    
			//                                     
			//                                                 
			break;

		case SENSOR_ENABLE:
			BAR_LOG("sensor enable temperature\n");
			//                                                           
			//                                                                                     
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				BAR_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;
				if(value == 0)
				{
					//                   
					memset(&(priv->fir), 0, sizeof(struct data_filter));
				}
				
			}
			break;

		case SENSOR_GET_DATA:
			//                                      
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				BAR_ERR("get sensor data  error!\n");
				err = -EINVAL;
			}
			else
			{
				temperature_data = (hwm_sensor_data *)buff_out;
				MS5607_ReadTempData(priv->client, buff, MS5607_BUFSIZE);
				sscanf(buff, "%x %x %x", &temperature_data->values[0], 
					&temperature_data->values[1], &temperature_data->values[2]);				
				temperature_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;				
				temperature_data->value_divide = 100;
				//                                                                                                              
			}
			break;
		default:
			BAR_ERR("teperature operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}


/*                                                                              
                         
                                                                             */
static int ms5607_open(struct inode *inode, struct file *file)
{
	file->private_data = ms5607_i2c_client;

	if(file->private_data == NULL)
	{
		BAR_ERR("null pointer!!\n");
		return -EINVAL;
	}
	return nonseekable_open(inode, file);
}
/*                                                                            */
static int ms5607_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*                                                                            */
static int ms5607_unlocked_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
       unsigned long arg)
{
	struct i2c_client *client = (struct i2c_client*)file->private_data;
	//                                                                                   
	char strbuf[MS5607_BUFSIZE];
	s32 dat=0;
	void __user *data;
	//                        
	//            
	long err = 0;
	//            

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
		BAR_ERR("access error: %08X, (%2d, %2d)\n", cmd, _IOC_DIR(cmd), _IOC_SIZE(cmd));
		return -EFAULT;
	}

	switch(cmd)
	{
		case BAROMETER_IOCTL_INIT:
			BAR_LOG("BAROSENSOR_IOCTL_INIT\n");
			MS5607_Init(client, 0);			
			break;

		case BAROMETER_IOCTL_READ_CHIPINFO:
			 BAR_LOG("BAROSENSOR_IOCTL_READ_CHIPINFO\n");
			
			break;	  

		case BAROMETER_GET_PRESS_DATA:
			//                                              
			data = (void __user *) arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;	  
			}
			
			MS5607_ReadPressureData(client, strbuf, MS5607_BUFSIZE);
			sscanf(strbuf, "%x", &dat);
			BAR_LOG("dat = %d\n",dat);
			if(copy_to_user(data, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				break;	  
			}				 
			break;
		case BAROMETER_GET_TEMP_DATA:
			
			data = (void __user *) arg;
			if(data == NULL)
			{
				err = -EINVAL;
				break;	  
			}
			MS5607_ReadTempData(client, strbuf, MS5607_BUFSIZE);
			sscanf(strbuf, "%x", &dat);
			BAR_LOG("dat = %d\n",dat);
			if(copy_to_user(data, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				break;	  
			}
			
			break;

		default:
			BAR_ERR("unknown IOCTL: 0x%08x\n", cmd);
			err = -ENOIOCTLCMD;
			break;
			
	}

	return err;
}


/*                                                                            */
static struct file_operations ms5607_fops = {
	//                     
	.open = ms5607_open,
	.release = ms5607_release,
	//                      
	.unlocked_ioctl = ms5607_unlocked_ioctl,
};
/*                                                                            */
static struct miscdevice ms5607_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "barometer",
	.fops = &ms5607_fops,
};
/*                                                                            */
#ifndef CONFIG_HAS_EARLYSUSPEND
/*                                                                            */
static int ms5607_suspend(struct i2c_client *client, pm_message_t msg) 
{
	struct ms5607_i2c_data *obj = i2c_get_clientdata(client);    
	int err = 0;
	u8  dat=0;
	GSE_FUN();    

	if(msg.event == PM_EVENT_SUSPEND)
	{   
		if(obj == NULL)
		{
			BAR_ERR("null pointer!!\n");
			return -EINVAL;
		}
		//             
		if ((err = hwmsen_read_byte_sr(client, MS5607_REG_CTL_REG1, &dat))) 
		{
           BAR_ERR("read ctl_reg1  fail!!\n");
           return err;
        }
		dat = dat&0b11111110;//             
		atomic_set(&obj->suspend, 1);
		if(err = hwmsen_write_byte(client, MS5607_REG_CTL_REG1, dat))
		{
			BAR_ERR("write power control fail!!\n");
			return err;
		}        
		MS5607_power(obj->hw, 0);
	}
	return err;
}
/*                                                                            */
static int ms5607_resume(struct i2c_client *client)
{
	struct ms5607_i2c_data *obj = i2c_get_clientdata(client);        
	int err;
	GSE_FUN();

	if(obj == NULL)
	{
		BAR_ERR("null pointer!!\n");
		return -EINVAL;
	}

	MS5607_power(obj->hw, 1);
	if(err = MS5607_Init(client, 0))
	{
		BAR_ERR("initialize client fail!!\n");
		return err;        
	}
	atomic_set(&obj->suspend, 0);

	return 0;
}
/*                                                                            */
#else /*                                   */
/*                                                                            */
static void ms5607_early_suspend(struct early_suspend *h) 
{
	struct ms5607_i2c_data *obj = container_of(h, struct ms5607_i2c_data, early_drv);   
//         
	BAR_FUN();    

	if(obj == NULL)
	{
		BAR_ERR("null pointer!!\n");
		return;
	}
	atomic_set(&obj->suspend, 1); 
	/*
                                                                     
  
                                          
         
    
 */
	

	sensor_power = false;
	
	MS5607_power(obj->hw, 0);
}
/*                                                                            */
static void ms5607_late_resume(struct early_suspend *h)
{
	struct ms5607_i2c_data *obj = container_of(h, struct ms5607_i2c_data, early_drv);         
	int err;
	BAR_FUN();

	if(obj == NULL)
	{
		BAR_ERR("null pointer!!\n");
		return;
	}

	MS5607_power(obj->hw, 1);
	if((err = MS5607_Init(obj->client, 0)))
	{
		BAR_ERR("initialize client fail!!\n");
		return;        
	}
	atomic_set(&obj->suspend, 0);    
}
/*                                                                            */
#endif /*                       */
/*                                                                            */
/*
                                                                                               
     
                                     
          
 
*/
/*                                                                            */
static int ms5607_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct i2c_client *new_client;
	struct ms5607_i2c_data *obj;
	//                          
	struct hwmsen_object obj_press;
	int err = 0;
	BAR_FUN();

	if(!(obj = kzalloc(sizeof(*obj), GFP_KERNEL)))
	{
		err = -ENOMEM;
		goto exit;
	}
	
	memset(obj, 0, sizeof(struct ms5607_i2c_data));

	obj->hw = get_cust_baro_hw();
	
	if((err = hwmsen_get_convert(obj->hw->direction, &obj->cvt)))
	{
		BAR_ERR("invalid direction: %d\n", obj->hw->direction);
		goto exit;
	}

	obj_i2c_data = obj;
	obj->client = client;
	new_client = obj->client;
	i2c_set_clientdata(new_client,obj);
	
	atomic_set(&obj->trace, 0);
	atomic_set(&obj->suspend, 0);
	//
#ifdef CONFIG_MS5607_LOWPASS

    atomic_set(&obj->filter, 1);
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
	BAR_LOG("ms5607_device  filter len =%d \n",atomic_read(&obj->firlen));
#endif


	ms5607_i2c_client = new_client;	

	if((err = MS5607_Init(new_client, 1)))
	{
		goto exit_init_failed;
	}
	

	if((err = misc_register(&ms5607_device)))
	{
		BAR_ERR("ms5607_device register failed\n");
		goto exit_misc_device_register_failed;
	}

	if((err = ms5607_create_attr(&ms5607_barometer_driver.driver)))
	{
		BAR_ERR("create attribute err = %d\n", err);
		goto exit_create_attr_failed;
	}
    
	obj_press.self = obj;
    obj_press.polling = 1;
    obj_press.sensor_operate = barometer_operate;
	if((err = hwmsen_attach(ID_PRESSURE, &obj_press)))
	{
		BAR_ERR("attach fail = %d\n", err);
		goto exit_kfree;
	}

	//                       
	//                    
    //                     
    //                                              
	//                                                   
	// 
	//                                    
	//                 
	// 

#ifdef CONFIG_HAS_EARLYSUSPEND
	//                                                             
	obj->early_drv.level    = EARLY_SUSPEND_LEVEL_STOP_DRAWING - 2,
	obj->early_drv.suspend  = ms5607_early_suspend,
	obj->early_drv.resume   = ms5607_late_resume,    
	register_early_suspend(&obj->early_drv);
#endif 

	BAR_LOG("%s: OK\n", __func__); 
 
	return 0;

	exit_create_attr_failed:
	misc_deregister(&ms5607_device);
	exit_misc_device_register_failed:
	exit_init_failed:
	//                              
	exit_kfree:
	kfree(obj);
	exit:
	BAR_ERR("%s: err = %d\n", __func__, err);        
	return err;
}

/*                                                                            */
static int ms5607_i2c_remove(struct i2c_client *client)
{
	int err = 0;	
	
	if((err = ms5607_delete_attr(&ms5607_barometer_driver.driver)))
	{
		BAR_ERR("ms5607_delete_attr fail: %d\n", err);
	}
	
	if((err = misc_deregister(&ms5607_device)))
	{
		BAR_ERR("misc_deregister fail: %d\n", err);
	}

	if((err = hwmsen_detach(ID_PRESSURE)))
	    

	ms5607_i2c_client = NULL;
	i2c_unregister_device(client);
	kfree(i2c_get_clientdata(client));
	return 0;
}
/*                                                                            */
static int ms5607_probe(struct platform_device *pdev) 
{
	struct baro_hw *hw = get_cust_baro_hw();
	BAR_FUN();

	MS5607_power(hw, 1);
	//                              
	if(i2c_add_driver(&ms5607_i2c_driver))
	{
		BAR_ERR("add driver error\n");
		return -1;
	}
	return 0;
}
/*                                                                            */
static int ms5607_remove(struct platform_device *pdev)
{
    struct baro_hw *hw = get_cust_baro_hw();

    BAR_FUN();    
    MS5607_power(hw, 0);    
    i2c_del_driver(&ms5607_i2c_driver);
    return 0;
}
/*                                                                            */
static struct platform_driver ms5607_barometer_driver = {
	.probe      = ms5607_probe,
	.remove     = ms5607_remove,    
	.driver     = {
		.name  = "barometer",
		//                     
	}
};

/*                                                                            */
static int __init ms5607_init(void)
{
	BAR_FUN();
	struct baro_hw *hw = get_cust_baro_hw();
	BAR_LOG("%s : i2c_num = %d\n", __func__, hw->i2c_num); 
	i2c_register_board_info(hw->i2c_num, &i2c_ms5607, 1);
	if(platform_driver_register(&ms5607_barometer_driver))
	{
		BAR_ERR("failed to register driver");
		return -ENODEV;
	}
	return 0;    
}
/*                                                                            */
static void __exit ms5607_exit(void)
{
	BAR_FUN();
	platform_driver_unregister(&ms5607_barometer_driver);
}
/*                                                                            */
module_init(ms5607_init);
module_exit(ms5607_exit);
/*                                                                            */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MS5607 I2C driver");
MODULE_AUTHOR("Chunlei.Wang@mediatek.com");
