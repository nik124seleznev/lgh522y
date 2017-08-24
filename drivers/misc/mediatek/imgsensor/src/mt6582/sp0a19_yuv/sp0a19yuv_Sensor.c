/*                                                                            
  
            
            
                           
  
           
           
         
  
               


               
                                 
           
  
          
          
        
  
                                                               
                      
                                                                        
                                                                                
        
                        
     
  
                                                                                
                                                                        
                                                               
                                                                              */
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <asm/system.h>


#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"

#include "sp0a19yuv_Sensor.h"
#include "sp0a19yuv_Camera_Sensor_para.h"
#include "sp0a19yuv_CameraCustomized.h"


#define SP0A19YUV_DEBUG
#ifdef SP0A19YUV_DEBUG
#define SENSORDB printk
#else
#define SENSORDB(x,...)
#endif

static DEFINE_SPINLOCK(sp0a19yuv_drv_lock);

kal_bool   SP0A19_MPEG4_encode_mode = KAL_FALSE;
kal_uint16 SP0A19_dummy_pixels = 0, SP0A19_dummy_lines = 0;

kal_uint32 SP0A19_isp_master_clock= 0;
static kal_uint32 SP0A19_PCLK = 26000000;

kal_uint8 sp0a19_isBanding = 0; //                
static kal_uint32 zoom_factor = 0; 

extern kal_bool searchMainSensor;

MSDK_SENSOR_CONFIG_STRUCT SP0A19SensorConfigData;
MSDK_SCENARIO_ID_ENUM SP0A19CurrentScenarioId = MSDK_SCENARIO_ID_CAMERA_PREVIEW;

#define SP0A19_SET_PAGE0 	SP0A19_write_cmos_sensor(0xfd, 0x00)
#define SP0A19_SET_PAGE1 	SP0A19_write_cmos_sensor(0xfd, 0x01)


extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);

void SP0A19_write_cmos_sensor(kal_uint8 addr, kal_uint8 para)
{
    char puSendCmd[2] = {(char)(addr & 0xFF) , (char)(para & 0xFF)};
	
	iWriteRegI2C(puSendCmd , 2, SP0A19_WRITE_ID);
	

}
kal_uint16 SP0A19_read_cmos_sensor(kal_uint8 addr)
{
	kal_uint16 get_byte=0;
    char puSendCmd = { (char)(addr & 0xFF) };
	iReadRegI2C(&puSendCmd , 1, (u8*)&get_byte, 1, SP0A19_WRITE_ID);
	
    return get_byte;
}


static void SP0A19_InitialPara(void)
{
  spin_lock(&sp0a19yuv_drv_lock);
  SP0A19_CurrentStatus.iNightMode = KAL_FALSE;
  SP0A19_CurrentStatus.iWB = AWB_MODE_AUTO;
  SP0A19_CurrentStatus.iEffect = MEFFECT_OFF;
  SP0A19_CurrentStatus.iBanding = AE_FLICKER_MODE_50HZ;
  SP0A19_CurrentStatus.iEV = AE_EV_COMP_00;
  SP0A19_CurrentStatus.iMirror = IMAGE_NORMAL;
  SP0A19_CurrentStatus.iFrameRate = 0;
  SP0A19_CurrentStatus.iBrightness =ISP_BRIGHT_MIDDLE;
  SP0A19_CurrentStatus.iIso =AE_ISO_AUTO;

  sp0a19_isBanding = 0;
  spin_unlock(&sp0a19yuv_drv_lock);
  
}



/*                                                                        
           
                    
  
              
                                                                 
  
             
                               
  
          
       
  
                   
  
                                                                         */
void SP0A19_Set_Shutter(kal_uint16 iShutter)
{
    /*         
                                      
                                              
                                                
    */
} 


/*                                                                        
           
                      
  
              
                                           
  
             
        
  
          
       
  
                   
  
                                                                         */
kal_uint16 SP0A19_Read_Shutter(void)
{
    kal_uint8 temp_reg1, temp_reg2;
	kal_uint16 shutter;

	SP0A19_write_cmos_sensor(0xfd, 0x00);
	temp_reg1 = SP0A19_read_cmos_sensor(0x04);
	temp_reg2 = SP0A19_read_cmos_sensor(0x03);

	shutter = (temp_reg1 & 0xFF) | (temp_reg2 << 8);

	//                                                      

	return shutter;
} /*                     */


kal_uint16 SP0A19_Read_Shutter_for_exif(void)
{
    kal_uint8 temp_reg1, temp_reg2;
	kal_uint16 shutter;
	kal_uint16 ExposureT;

	SP0A19_write_cmos_sensor(0xfd, 0x00);
	temp_reg1 = SP0A19_read_cmos_sensor(0x04);
	temp_reg2 = SP0A19_read_cmos_sensor(0x03);

	shutter = (temp_reg1 & 0xFF) | (temp_reg2 << 8);
	SENSORDB("SP0A19_Read_Shutter shutter=%d \n",shutter);

	ExposureT = shutter/10 ;//  
	SENSORDB("SP0A19_Read_Shutter ExposureT=%d \n",ExposureT);

	return ExposureT;
} /*                     */


kal_uint16 SP0A19_Read_ISO_for_exif(void)
{
    kal_uint16 temp=0;
	//                   

	SP0A19_write_cmos_sensor(0xfd, 0x00);
	temp = SP0A19_read_cmos_sensor(0x23);//     
	SENSORDB("SP0A19ReadGain reg value=%d \n",temp);
	
    if(temp < 0x20)
    {
		return AE_ISO_100;
	}
	else if((temp >=0x20)&&(temp < 0x30))
	{
		return AE_ISO_200;
	}
	else if((temp >=0x30)&&(temp < 0x60))
	{
		return AE_ISO_400;
	}
	else
	{
		return AE_ISO_800;
	}

}


/*                                                                        
           
                   
  
              
                                            
  
             
                                      
                                                                
  
          
       
  
                   
  
                                                                         */
void SP0A19_write_reg(kal_uint32 addr, kal_uint32 para)
{
	SP0A19_write_cmos_sensor(addr, para);
}


/*                                                                        
           
                  
  
              
                                                                  
  
             
                                      
  
          
                                 
  
                   
  
                                                                         */
kal_uint32 SP0A19_read_reg(kal_uint32 addr)
{
	return SP0A19_read_cmos_sensor(addr);
}



/*                                                                        
          
                   
 
             
                                                               
 
            
                                                               
 
         
                                           
 
                                                                        */
#if 0
static void SP0A19_awb_enable(kal_bool enalbe)
{	 
	kal_uint16 temp_AWB_reg = 0;

	SP0A19_write_cmos_sensor(0xfd, 0x00);
	temp_AWB_reg = SP0A19_read_cmos_sensor(0x32);
	
	if (enalbe)
	{
		SP0A19_write_cmos_sensor(0x32, (temp_AWB_reg |0x08));
	}
	else
	{
		SP0A19_write_cmos_sensor(0x32, (temp_AWB_reg & (~0x08)));
	}

}
#endif



/*                                                                        
           
                   
  
              
                                      
  
             
                                                                        
  
          
       
  
                   
  
                                                                         */
void SP0A19NightMode(kal_bool bEnable)
{
   kal_uint8 Reg32Val=0;
   spin_lock(&sp0a19yuv_drv_lock);
   SP0A19_CurrentStatus.iNightMode = bEnable;
	spin_unlock(&sp0a19yuv_drv_lock);
	SP0A19_write_cmos_sensor(0xfd,0x00);
	Reg32Val = SP0A19_read_cmos_sensor(0x32);
	SP0A19_write_cmos_sensor(0x32,Reg32Val & 0xf0);

	if(bEnable)//          
	{ 
		SP0A19_write_cmos_sensor(0xfd,0x00);
		SP0A19_write_cmos_sensor(0xb2,0x25);
		SP0A19_write_cmos_sensor(0xb3,0x1f);
					
	   if(SP0A19_MPEG4_encode_mode == KAL_TRUE)
		{
				if(sp0a19_isBanding== 0)
				{
					//          
					//          
				    SENSORDB(" video 50Hz night\r\n");
					SP0A19_write_cmos_sensor(0xfd,0x00);
					SP0A19_write_cmos_sensor(0x03,0x01);
					SP0A19_write_cmos_sensor(0x04,0xce);
					
					SP0A19_write_cmos_sensor(0x06,0x00);
					SP0A19_write_cmos_sensor(0x09,0x03);
					SP0A19_write_cmos_sensor(0x0a,0x88);
					SP0A19_write_cmos_sensor(0xf0,0x4d);
					SP0A19_write_cmos_sensor(0xf1,0x00);
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0x90,0x06);
					SP0A19_write_cmos_sensor(0x92,0x01);
					SP0A19_write_cmos_sensor(0x98,0x4d);
					SP0A19_write_cmos_sensor(0x99,0x00);
					SP0A19_write_cmos_sensor(0x9a,0x01);
					SP0A19_write_cmos_sensor(0x9b,0x00);
					//         
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0xce,0xce);
					SP0A19_write_cmos_sensor(0xcf,0x01);
					SP0A19_write_cmos_sensor(0xd0,0xce);
					SP0A19_write_cmos_sensor(0xd1,0x01);
					SP0A19_write_cmos_sensor(0xfd,0x00);
				}
				else if(sp0a19_isBanding == 1)
				{
					//          
					//          
					SENSORDB(" video 60Hz night\r\n");
					SP0A19_write_cmos_sensor(0xfd,0x00);
					SP0A19_write_cmos_sensor(0x03,0x02);
					SP0A19_write_cmos_sensor(0x04,0x00);
					
					SP0A19_write_cmos_sensor(0x06,0x00);
					SP0A19_write_cmos_sensor(0x09,0x03);
					SP0A19_write_cmos_sensor(0x0a,0x8d);
					SP0A19_write_cmos_sensor(0xf0,0x40);
					SP0A19_write_cmos_sensor(0xf1,0x00);
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0x90,0x08);
					SP0A19_write_cmos_sensor(0x92,0x01);
					SP0A19_write_cmos_sensor(0x98,0x40);
					SP0A19_write_cmos_sensor(0x99,0x00);
					SP0A19_write_cmos_sensor(0x9a,0x01);
					SP0A19_write_cmos_sensor(0x9b,0x00);
					//       
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0xce,0x00);
					SP0A19_write_cmos_sensor(0xcf,0x02);
					SP0A19_write_cmos_sensor(0xd0,0x00);
					SP0A19_write_cmos_sensor(0xd1,0x02);
					SP0A19_write_cmos_sensor(0xfd,0x00);
				}
   		  	}	
	    else 
	    {  
			       if(sp0a19_isBanding== 0)
					{
					   //            
					   //          
					   SENSORDB(" priview 50Hz night\r\n");    
					   SP0A19_write_cmos_sensor(0xfd,0x00);
					 SP0A19_write_cmos_sensor(0x03,0x0b);
					 SP0A19_write_cmos_sensor(0x04,0xf4);
					   SP0A19_write_cmos_sensor(0x06,0x00);
					   SP0A19_write_cmos_sensor(0x09,0x00);
					   SP0A19_write_cmos_sensor(0x0a,0x42);
					   SP0A19_write_cmos_sensor(0xf0,0x99);
					   SP0A19_write_cmos_sensor(0xf1,0x00);
					   SP0A19_write_cmos_sensor(0xfd,0x01);
					   SP0A19_write_cmos_sensor(0x90,0x14);
					   SP0A19_write_cmos_sensor(0x92,0x01);
					   SP0A19_write_cmos_sensor(0x98,0x99);
					   SP0A19_write_cmos_sensor(0x99,0x00);
					   SP0A19_write_cmos_sensor(0x9a,0x01);
					   SP0A19_write_cmos_sensor(0x9b,0x00);
					   //         
					   SP0A19_write_cmos_sensor(0xfd,0x01);
					   SP0A19_write_cmos_sensor(0xce,0xf4);
					   SP0A19_write_cmos_sensor(0xcf,0x0b);
					   SP0A19_write_cmos_sensor(0xd0,0xf4);
					   SP0A19_write_cmos_sensor(0xd1,0x0b);
					   SP0A19_write_cmos_sensor(0xfd,0x00);
				}  
				else if(sp0a19_isBanding== 1)
				{
					//            
					//          
				    SENSORDB(" priview 60Hz night\r\n");	
					SP0A19_write_cmos_sensor(0xfd,0x00);
					SP0A19_write_cmos_sensor(0x03,0x0c);
					SP0A19_write_cmos_sensor(0x04,0x00);
					SP0A19_write_cmos_sensor(0x06,0x00);
					SP0A19_write_cmos_sensor(0x09,0x00);
					SP0A19_write_cmos_sensor(0x0a,0x3e);
					SP0A19_write_cmos_sensor(0xf0,0x80);
					SP0A19_write_cmos_sensor(0xf1,0x00);
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0x90,0x18);
					SP0A19_write_cmos_sensor(0x92,0x01);
					SP0A19_write_cmos_sensor(0x98,0x80);
					SP0A19_write_cmos_sensor(0x99,0x00);
					SP0A19_write_cmos_sensor(0x9a,0x01);
					SP0A19_write_cmos_sensor(0x9b,0x00);
					//         
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0xce,0x00);
					SP0A19_write_cmos_sensor(0xcf,0x0c);
					SP0A19_write_cmos_sensor(0xd0,0x00);
					SP0A19_write_cmos_sensor(0xd1,0x0c);
					SP0A19_write_cmos_sensor(0xfd,0x00);
				}
			       } 		
	}
	else    //              
	{
		
				SP0A19_write_cmos_sensor(0xfd,0x00);
				SP0A19_write_cmos_sensor(0xb2,0x08);
				SP0A19_write_cmos_sensor(0xb3,0x1f);
					
	         if(SP0A19_MPEG4_encode_mode == KAL_TRUE)
	          {
				if(sp0a19_isBanding== 0)
				{
					//              
					//          
				    SENSORDB(" video 50Hz normal\r\n");				
					SP0A19_write_cmos_sensor(0xfd,0x00);
					SP0A19_write_cmos_sensor(0x03,0x01);
					SP0A19_write_cmos_sensor(0x04,0xdd);
					SP0A19_write_cmos_sensor(0x06,0x00);
					SP0A19_write_cmos_sensor(0x09,0x00);
					SP0A19_write_cmos_sensor(0x0a,0x22);
					SP0A19_write_cmos_sensor(0xf0,0x9f);
					SP0A19_write_cmos_sensor(0xf1,0x00);
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0x90,0x03);
					SP0A19_write_cmos_sensor(0x92,0x01);
					SP0A19_write_cmos_sensor(0x98,0x9f);
					SP0A19_write_cmos_sensor(0x99,0x00);
					SP0A19_write_cmos_sensor(0x9a,0x01);
					SP0A19_write_cmos_sensor(0x9b,0x00);
					//         
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0xce,0xdd);
					SP0A19_write_cmos_sensor(0xcf,0x01);
					SP0A19_write_cmos_sensor(0xd0,0xdd);
					SP0A19_write_cmos_sensor(0xd1,0x01);
					SP0A19_write_cmos_sensor(0xfd,0x00);
			
				}
				else if(sp0a19_isBanding == 1)
				{
					//              
					//          
				    SENSORDB(" video 60Hz normal \n");	
					SP0A19_write_cmos_sensor(0xfd,0x00);
					SP0A19_write_cmos_sensor(0x03,0x01);
					SP0A19_write_cmos_sensor(0x04,0x8c);
					SP0A19_write_cmos_sensor(0x06,0x00);
					SP0A19_write_cmos_sensor(0x09,0x00);
					SP0A19_write_cmos_sensor(0x0a,0x25);
					SP0A19_write_cmos_sensor(0xf0,0x84);
					SP0A19_write_cmos_sensor(0xf1,0x00);
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0x90,0x03);
					SP0A19_write_cmos_sensor(0x92,0x01);
					SP0A19_write_cmos_sensor(0x98,0x84);
					SP0A19_write_cmos_sensor(0x99,0x00);
					SP0A19_write_cmos_sensor(0x9a,0x01);
					SP0A19_write_cmos_sensor(0x9b,0x00);
					//        
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0xce,0x8c);
					SP0A19_write_cmos_sensor(0xcf,0x01);
					SP0A19_write_cmos_sensor(0xd0,0x8c);
					SP0A19_write_cmos_sensor(0xd1,0x01);
					SP0A19_write_cmos_sensor(0xfd,0x00);
				}
			   }
			else 
			{
				if(sp0a19_isBanding== 0)
				{
					//              
					//          
				    SENSORDB(" priview 50Hz normal\r\n");
					SP0A19_write_cmos_sensor(0xfd,0x00);
					SP0A19_write_cmos_sensor(0x03,0x05);
					SP0A19_write_cmos_sensor(0x04,0xfa);
					SP0A19_write_cmos_sensor(0x06,0x00);
					SP0A19_write_cmos_sensor(0x09,0x00);
					SP0A19_write_cmos_sensor(0x0a,0x42);
					SP0A19_write_cmos_sensor(0xf0,0x99);
					SP0A19_write_cmos_sensor(0xf1,0x00);
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0x90,0x0a);
					SP0A19_write_cmos_sensor(0x92,0x01);
					SP0A19_write_cmos_sensor(0x98,0x99);
					SP0A19_write_cmos_sensor(0x99,0x00);
					SP0A19_write_cmos_sensor(0x9a,0x01);
					SP0A19_write_cmos_sensor(0x9b,0x00);
					//       
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0xce,0xfa);
					SP0A19_write_cmos_sensor(0xcf,0x05);
					SP0A19_write_cmos_sensor(0xd0,0xfa);
					SP0A19_write_cmos_sensor(0xd1,0x05);
					SP0A19_write_cmos_sensor(0xfd,0x00);
				}
				else if(sp0a19_isBanding== 1)
				{
					//             
					//          
				    SENSORDB(" priview 60Hz normal\r\n");
					SP0A19_write_cmos_sensor(0xfd,0x00);
					SP0A19_write_cmos_sensor(0x03,0x06);
					SP0A19_write_cmos_sensor(0x04,0x00);
					SP0A19_write_cmos_sensor(0x06,0x00);
					SP0A19_write_cmos_sensor(0x09,0x00);
					SP0A19_write_cmos_sensor(0x0a,0x3e);
					SP0A19_write_cmos_sensor(0xf0,0x80);
					SP0A19_write_cmos_sensor(0xf1,0x00);
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0x90,0x0c);
					SP0A19_write_cmos_sensor(0x92,0x01);
					SP0A19_write_cmos_sensor(0x98,0x80);
					SP0A19_write_cmos_sensor(0x99,0x00);
					SP0A19_write_cmos_sensor(0x9a,0x01);
					SP0A19_write_cmos_sensor(0x9b,0x00);
					//         
					SP0A19_write_cmos_sensor(0xfd,0x01);
					SP0A19_write_cmos_sensor(0xce,0x00);
					SP0A19_write_cmos_sensor(0xcf,0x06);
					SP0A19_write_cmos_sensor(0xd0,0x00);
					SP0A19_write_cmos_sensor(0xd1,0x06);
					SP0A19_write_cmos_sensor(0xfd,0x00);
				}
			 }
	}  
    mdelay(50);
	SP0A19_write_cmos_sensor(0xfd,0x00);
	SP0A19_write_cmos_sensor(0x32,Reg32Val |0x05);//         
	spin_lock(&sp0a19yuv_drv_lock);
	SP0A19_CurrentStatus.iNightMode = bEnable;
	spin_unlock(&sp0a19yuv_drv_lock);
}

/*                                                                        
          
                    
 
             
                                                           
 
            
      
 
         
      
 
                                                                        */
void SP0A19_Sensor_Init(void)
{  
	  SENSORDB("SP0A19_Sensor_Init enter \n");
	  SP0A19_write_cmos_sensor(0xfd,0x00);
	  SP0A19_write_cmos_sensor(0x1C,0x28);
	  SP0A19_write_cmos_sensor(0x32,0x00);
	  SP0A19_write_cmos_sensor(0x0f,0x2f);
	  SP0A19_write_cmos_sensor(0x10,0x2e);
	  SP0A19_write_cmos_sensor(0x11,0x00);
	  SP0A19_write_cmos_sensor(0x12,0x10);//    
	  SP0A19_write_cmos_sensor(0x13,0x2f);
	  SP0A19_write_cmos_sensor(0x14,0x00);
	  SP0A19_write_cmos_sensor(0x15,0x12);//    
	  SP0A19_write_cmos_sensor(0x16,0x00);
	  SP0A19_write_cmos_sensor(0x17,0x12);//    
	  SP0A19_write_cmos_sensor(0x0b,0x42);//   
	  SP0A19_write_cmos_sensor(0x25,0x12);//    
	  SP0A19_write_cmos_sensor(0x1a,0x0b);
	  SP0A19_write_cmos_sensor(0x1b,0xc );
	  SP0A19_write_cmos_sensor(0x1e,0xb );
	  SP0A19_write_cmos_sensor(0x20,0x3f); //    
	  SP0A19_write_cmos_sensor(0x21,0x13); //        
	  SP0A19_write_cmos_sensor(0x22,0x19);
	  SP0A19_write_cmos_sensor(0x26,0x1a);
	  SP0A19_write_cmos_sensor(0x27,0xab);
	  SP0A19_write_cmos_sensor(0x28,0xfd);
	  SP0A19_write_cmos_sensor(0x30,0x00);
	  
	  //                                        
	  SP0A19_write_cmos_sensor(0x31,0x60);//    
	  
	  SP0A19_write_cmos_sensor(0xfb,0x30); //     
	  SP0A19_write_cmos_sensor(0x1f,0x10);
	  
	//          
	  SP0A19_write_cmos_sensor(0xfd,0x00);
	  SP0A19_write_cmos_sensor(0x65,0x06);//              
	  SP0A19_write_cmos_sensor(0x66,0x06);//             
	  SP0A19_write_cmos_sensor(0x67,0x06);//            
	  SP0A19_write_cmos_sensor(0x68,0x06);//            
	  SP0A19_write_cmos_sensor(0x45,0x00);
	  SP0A19_write_cmos_sensor(0x46,0x0f);
	//                 
	//          
		SP0A19_write_cmos_sensor(0xfd,0x00);
		SP0A19_write_cmos_sensor(0x03,0x01);
		SP0A19_write_cmos_sensor(0x04,0xcb);
		SP0A19_write_cmos_sensor(0x06,0x00);
		SP0A19_write_cmos_sensor(0x09,0x00);
		SP0A19_write_cmos_sensor(0x0a,0x42);
		SP0A19_write_cmos_sensor(0xf0,0x99);
		SP0A19_write_cmos_sensor(0xf1,0x00);
		SP0A19_write_cmos_sensor(0xfd,0x01);
		SP0A19_write_cmos_sensor(0x90,0x0a);
		SP0A19_write_cmos_sensor(0x92,0x01);
		SP0A19_write_cmos_sensor(0x98,0x99);
		SP0A19_write_cmos_sensor(0x99,0x00);
		SP0A19_write_cmos_sensor(0x9a,0x01);
		SP0A19_write_cmos_sensor(0x9b,0x00);
		//       
		SP0A19_write_cmos_sensor(0xfd,0x01);
		SP0A19_write_cmos_sensor(0xce,0xfa);
		SP0A19_write_cmos_sensor(0xcf,0x05);
		SP0A19_write_cmos_sensor(0xd0,0xfa);
		SP0A19_write_cmos_sensor(0xd1,0x05);
		SP0A19_write_cmos_sensor(0xfd,0x00);

	  SP0A19_write_cmos_sensor(0xfd,0x01);
	  SP0A19_write_cmos_sensor(0xc4,0x56);//  
	  SP0A19_write_cmos_sensor(0xc5,0x8f);//  
	  SP0A19_write_cmos_sensor(0xca,0x30);
	  SP0A19_write_cmos_sensor(0xcb,0x45);
	  SP0A19_write_cmos_sensor(0xcc,0x70);//           
	  SP0A19_write_cmos_sensor(0xcd,0x48);//             
	  SP0A19_write_cmos_sensor(0xfd,0x00);

	  //            
	  SP0A19_write_cmos_sensor(0xfd,0x01);
	  SP0A19_write_cmos_sensor(0x35,0x15);
	  SP0A19_write_cmos_sensor(0x36,0x15); //  
	  SP0A19_write_cmos_sensor(0x37,0x15);
	  SP0A19_write_cmos_sensor(0x38,0x15);
	  SP0A19_write_cmos_sensor(0x39,0x15);
	  SP0A19_write_cmos_sensor(0x3a,0x15); //  
	  SP0A19_write_cmos_sensor(0x3b,0x13);
	  SP0A19_write_cmos_sensor(0x3c,0x15);
	  SP0A19_write_cmos_sensor(0x3d,0x15);
	  SP0A19_write_cmos_sensor(0x3e,0x15); //  
	  SP0A19_write_cmos_sensor(0x3f,0x15);
	  SP0A19_write_cmos_sensor(0x40,0x18);
	  SP0A19_write_cmos_sensor(0x41,0x00);
	  SP0A19_write_cmos_sensor(0x42,0x04);
	  SP0A19_write_cmos_sensor(0x43,0x04);
	  SP0A19_write_cmos_sensor(0x44,0x00);
	  SP0A19_write_cmos_sensor(0x45,0x00);
	  SP0A19_write_cmos_sensor(0x46,0x00);
	  SP0A19_write_cmos_sensor(0x47,0x00);
	  SP0A19_write_cmos_sensor(0x48,0x00);
	  SP0A19_write_cmos_sensor(0x49,0xfd);
	  SP0A19_write_cmos_sensor(0x4a,0x00);
	  SP0A19_write_cmos_sensor(0x4b,0x00);
	  SP0A19_write_cmos_sensor(0x4c,0xfd);
	  SP0A19_write_cmos_sensor(0xfd,0x00);

	//     
	  SP0A19_write_cmos_sensor(0xfd,0x01);
	  SP0A19_write_cmos_sensor(0x28,0xA0);
	  SP0A19_write_cmos_sensor(0x29,0xA0);
	//                                    
	//                                     
	//                                    
	  SP0A19_write_cmos_sensor(0x2e,0x02);	
	  SP0A19_write_cmos_sensor(0x2f,0x16);
	  SP0A19_write_cmos_sensor(0x17,0x17);
	  SP0A19_write_cmos_sensor(0x18,0x19);	//          
	  SP0A19_write_cmos_sensor(0x19,0x45);	

	//                                            
	//                                           
	//                                    
	  SP0A19_write_cmos_sensor(0x2a,0xef);
	  SP0A19_write_cmos_sensor(0x2b,0x15);

	  //    
	  SP0A19_write_cmos_sensor(0xfd,0x01);
	  SP0A19_write_cmos_sensor(0x73,0x80);
	  SP0A19_write_cmos_sensor(0x1a,0x80);
	  SP0A19_write_cmos_sensor(0x1b,0x80); 
	//   
	  SP0A19_write_cmos_sensor(0x65,0xd5); //  
	  SP0A19_write_cmos_sensor(0x66,0xfa); //  
	  SP0A19_write_cmos_sensor(0x67,0x72); //  
	  SP0A19_write_cmos_sensor(0x68,0x8a); //  
	//      
	  SP0A19_write_cmos_sensor(0x69,0xc6); //  
	  SP0A19_write_cmos_sensor(0x6a,0xee); //  
	  SP0A19_write_cmos_sensor(0x6b,0x94); //  
	  SP0A19_write_cmos_sensor(0x6c,0xab); //  
	//  
	  SP0A19_write_cmos_sensor(0x61,0x7a); //  
	  SP0A19_write_cmos_sensor(0x62,0x98); //  
	  SP0A19_write_cmos_sensor(0x63,0xc5); //  
	  SP0A19_write_cmos_sensor(0x64,0xe6); //  
	  //   
	  SP0A19_write_cmos_sensor(0x6d,0xb9); //  
	  SP0A19_write_cmos_sensor(0x6e,0xde); //  
	  SP0A19_write_cmos_sensor(0x6f,0xb2); //  
	  SP0A19_write_cmos_sensor(0x70,0xd5); //  
	 
	//           
	 SP0A19_write_cmos_sensor(0xfd,0x01);
	 SP0A19_write_cmos_sensor(0x08,0x15);
	 SP0A19_write_cmos_sensor(0x09,0x04);
	 SP0A19_write_cmos_sensor(0x0a,0x20);
	 SP0A19_write_cmos_sensor(0x0b,0x12);
	 SP0A19_write_cmos_sensor(0x0c,0x27);
	 SP0A19_write_cmos_sensor(0x0d,0x06);
	 SP0A19_write_cmos_sensor(0x0f,0x63);//         

	   //        
	  SP0A19_write_cmos_sensor(0xfd,0x00);
	  SP0A19_write_cmos_sensor(0x79,0xf0);
	  SP0A19_write_cmos_sensor(0x7a,0x80);	//  
	  SP0A19_write_cmos_sensor(0x7b,0x80);	//  
	  SP0A19_write_cmos_sensor(0x7c,0x20); //  
	  
	//      
	  SP0A19_write_cmos_sensor(0xfd,0x00);

	  //
	  SP0A19_write_cmos_sensor(0x57,0x06); //                   
	  SP0A19_write_cmos_sensor(0x58,0x0d); //                  
	  SP0A19_write_cmos_sensor(0x56,0x10); //                 
	  SP0A19_write_cmos_sensor(0x59,0x10); //                    
		//    
	  SP0A19_write_cmos_sensor(0x89,0x06); //                     
	  SP0A19_write_cmos_sensor(0x8a,0x0d); //                    
	  SP0A19_write_cmos_sensor(0x9c,0x10); //                   
	  SP0A19_write_cmos_sensor(0x9d,0x10); //                     
		//     
	  SP0A19_write_cmos_sensor(0x81,0xe0); //                    
	  SP0A19_write_cmos_sensor(0x82,0xe0); //                   
	  SP0A19_write_cmos_sensor(0x83,0x80); //                  
	  SP0A19_write_cmos_sensor(0x84,0x40); //                     
		//    
	  SP0A19_write_cmos_sensor(0x85,0xe0); //                    
	  SP0A19_write_cmos_sensor(0x86,0xc0); //                  
	  SP0A19_write_cmos_sensor(0x87,0x80); //                 
	  SP0A19_write_cmos_sensor(0x88,0x40); //                   
		//
	  SP0A19_write_cmos_sensor(0x5a,0xff);	//                  
	  SP0A19_write_cmos_sensor(0x5b,0xe0);	//                 
	  SP0A19_write_cmos_sensor(0x5c,0x80);	//                
	  SP0A19_write_cmos_sensor(0x5d,0x00);	//                   
	  
	//        
	  SP0A19_write_cmos_sensor(0xfd,0x01); 
	  SP0A19_write_cmos_sensor(0xe2,0x30); //              
	  SP0A19_write_cmos_sensor(0xe4,0xa0); //             

	  SP0A19_write_cmos_sensor(0xe5,0x04); //                          
	  SP0A19_write_cmos_sensor(0xd3,0x04); //                         
	  SP0A19_write_cmos_sensor(0xd7,0x04); //                         
	 
	  SP0A19_write_cmos_sensor(0xe6,0x04); //                          
	  SP0A19_write_cmos_sensor(0xd4,0x04); //                          
	  SP0A19_write_cmos_sensor(0xd8,0x04); //                          
	  
	  SP0A19_write_cmos_sensor(0xe7,0x08); //                        
	  SP0A19_write_cmos_sensor(0xd5,0x08); //                        
	  SP0A19_write_cmos_sensor(0xd9,0x08); //                         
		
	  SP0A19_write_cmos_sensor(0xd2,0x10); //                   
	  SP0A19_write_cmos_sensor(0xd6,0x10); //                   
	  SP0A19_write_cmos_sensor(0xda,0x10); //                   
	 
	  SP0A19_write_cmos_sensor(0xe8,0x20);	//                              
	  SP0A19_write_cmos_sensor(0xec,0x35);	//                     
	  SP0A19_write_cmos_sensor(0xe9,0x20);	//                          
	  SP0A19_write_cmos_sensor(0xed,0x35);	//                
	  SP0A19_write_cmos_sensor(0xea,0x20);	//                           
	  SP0A19_write_cmos_sensor(0xef,0x30);	//                           
	  SP0A19_write_cmos_sensor(0xeb,0x10);	//                 
	  SP0A19_write_cmos_sensor(0xf0,0x20);	//                  
	  
	//   
	  SP0A19_write_cmos_sensor(0xfd,0x01);
	  SP0A19_write_cmos_sensor(0xa0,0x85);	//                     
	  SP0A19_write_cmos_sensor(0xa1,0x00);
	  SP0A19_write_cmos_sensor(0xa2,0x00);
	  SP0A19_write_cmos_sensor(0xa3,0xf3);	//     
	  SP0A19_write_cmos_sensor(0xa4,0x8e);	//     
	  SP0A19_write_cmos_sensor(0xa5,0x00);	//     
	  SP0A19_write_cmos_sensor(0xa6,0x00);	//     
	  SP0A19_write_cmos_sensor(0xa7,0xe6);	 //     
	  SP0A19_write_cmos_sensor(0xa8,0x9a);//                  
	  SP0A19_write_cmos_sensor(0xa9,0x00);
	  SP0A19_write_cmos_sensor(0xaa,0x03);	//     
	  SP0A19_write_cmos_sensor(0xab,0x0c);
	  SP0A19_write_cmos_sensor(0xfd,0x00);

			//       
	  SP0A19_write_cmos_sensor(0xfd,0x00);
	  SP0A19_write_cmos_sensor(0x8b,0x0 );	//         
	  SP0A19_write_cmos_sensor(0x8c,0xC );	//         
	  SP0A19_write_cmos_sensor(0x8d,0x19);	//         
	  SP0A19_write_cmos_sensor(0x8e,0x2C);	//         
	  SP0A19_write_cmos_sensor(0x8f,0x49);	//         
	  SP0A19_write_cmos_sensor(0x90,0x61);	//         
	  SP0A19_write_cmos_sensor(0x91,0x77);	//         
	  SP0A19_write_cmos_sensor(0x92,0x8A);	//         
	  SP0A19_write_cmos_sensor(0x93,0x9B);	//         
	  SP0A19_write_cmos_sensor(0x94,0xA9);	//         
	  SP0A19_write_cmos_sensor(0x95,0xB5);	//         
	  SP0A19_write_cmos_sensor(0x96,0xC0);	//         
	  SP0A19_write_cmos_sensor(0x97,0xCA);	//         
	  SP0A19_write_cmos_sensor(0x98,0xD4);	//         
	  SP0A19_write_cmos_sensor(0x99,0xDD);	//         
	  SP0A19_write_cmos_sensor(0x9a,0xE6);	//         
	  SP0A19_write_cmos_sensor(0x9b,0xEF);	//         
	  SP0A19_write_cmos_sensor(0xfd,0x01);	//         
	  SP0A19_write_cmos_sensor(0x8d,0xF7);	//         
	  SP0A19_write_cmos_sensor(0x8e,0xFF);	//            
	  SP0A19_write_cmos_sensor(0xfd,0x00);	//

	   //   
	  SP0A19_write_cmos_sensor(0xfd,0x00); 
	  SP0A19_write_cmos_sensor(0xe0,0x4c); //                                    
	  SP0A19_write_cmos_sensor(0xe1,0x3c); //                                    
	  SP0A19_write_cmos_sensor(0xe2,0x34); //                                    
	  SP0A19_write_cmos_sensor(0xe3,0x2e); //                                 
	  SP0A19_write_cmos_sensor(0xe4,0x2e); //                                 
	  SP0A19_write_cmos_sensor(0xe5,0x2c); //                                 
	  SP0A19_write_cmos_sensor(0xe6,0x2c); //                                 
	  SP0A19_write_cmos_sensor(0xe8,0x2a); //                                    
	  SP0A19_write_cmos_sensor(0xe9,0x2a); //                                     
	  SP0A19_write_cmos_sensor(0xea,0x2a); //                                     
	  SP0A19_write_cmos_sensor(0xeb,0x28); //                                     
	  SP0A19_write_cmos_sensor(0xf5,0x28); //                                     
	  SP0A19_write_cmos_sensor(0xf6,0x28); //                                      

	//             
	  SP0A19_write_cmos_sensor(0xfd,0x01);
	  SP0A19_write_cmos_sensor(0x94,0xa0);	//            
	  SP0A19_write_cmos_sensor(0x95,0x28);	 //                 
	  SP0A19_write_cmos_sensor(0x9c,0xa0);	//             
	  SP0A19_write_cmos_sensor(0x9d,0x28);	//               
	//         
	  SP0A19_write_cmos_sensor(0xfd,0x00); 
	  SP0A19_write_cmos_sensor(0xed,0x8c); //   
	  SP0A19_write_cmos_sensor(0xf7,0x88); //   
	  SP0A19_write_cmos_sensor(0xf8,0x80); //   
	  SP0A19_write_cmos_sensor(0xec,0x7c); //    
	  
	  SP0A19_write_cmos_sensor(0xef,0x74); //  
	  SP0A19_write_cmos_sensor(0xf9,0x70); //  
	  SP0A19_write_cmos_sensor(0xfa,0x68); //  
	  SP0A19_write_cmos_sensor(0xee,0x64); //  

		
	//           
	  SP0A19_write_cmos_sensor(0xfd,0x01);
	  SP0A19_write_cmos_sensor(0x30,0x40);
	  //         
	  SP0A19_write_cmos_sensor(0x31,0x70);
	  
	  SP0A19_write_cmos_sensor(0x32,0x40);
	  SP0A19_write_cmos_sensor(0x33,0xef);
	  SP0A19_write_cmos_sensor(0x34,0x05);
	  SP0A19_write_cmos_sensor(0x4d,0x2f);
	  SP0A19_write_cmos_sensor(0x4e,0x20);
	  SP0A19_write_cmos_sensor(0x4f,0x16);

	//            
	  SP0A19_write_cmos_sensor(0xfd,0x00); //
	  SP0A19_write_cmos_sensor(0xb2,0x20); //                 
	  SP0A19_write_cmos_sensor(0xb3,0x1f); //       
	  SP0A19_write_cmos_sensor(0xb4,0x30); //                 
	  SP0A19_write_cmos_sensor(0xb5,0x45); //        

	//          
	  SP0A19_write_cmos_sensor(0xfd,0x00); 
	  SP0A19_write_cmos_sensor(0xbe,0xff); 
	  SP0A19_write_cmos_sensor(0xbf,0x01); 
	  SP0A19_write_cmos_sensor(0xc0,0xff); 
	  SP0A19_write_cmos_sensor(0xc1,0xd8); 
	  SP0A19_write_cmos_sensor(0xd3,0x88); //    
	  SP0A19_write_cmos_sensor(0xd4,0x88); //    
	  SP0A19_write_cmos_sensor(0xd6,0x70); //                           
	  SP0A19_write_cmos_sensor(0xd7,0x60);

	//   
	  SP0A19_write_cmos_sensor(0xfd,0x00); 
	  SP0A19_write_cmos_sensor(0xdc,0x00); 
	  SP0A19_write_cmos_sensor(0xdd,0x80); //                 
	  SP0A19_write_cmos_sensor(0xde,0xa8); //              
	  SP0A19_write_cmos_sensor(0xdf,0x80); 
	   
	//           
	  SP0A19_write_cmos_sensor(0xfd,0x00);	
	  SP0A19_write_cmos_sensor(0x32,0x15);	//    
	  SP0A19_write_cmos_sensor(0x34,0x76);	//  
	  SP0A19_write_cmos_sensor(0x35,0x00);	
	  SP0A19_write_cmos_sensor(0x33,0xef);	
	  SP0A19_write_cmos_sensor(0x5f,0x51); 
}



/*                                                                        
          
                     
 
             
                                                                        
 
            
      
 
         
      
 
                  
 
                                                                        */
UINT32 SP0A19GetSensorID(UINT32 *sensorID)
{
    kal_uint16 sensor_id=0;

	SENSORDB("SP0A19GetSensorID \n");	

#if 1 //                                                          
	if(searchMainSensor == KAL_TRUE)
	{
		*sensorID=0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
#endif


	SP0A19_write_cmos_sensor(0xfd,0x00);
	sensor_id=SP0A19_read_cmos_sensor(0x02);
	SENSORDB("Sensor Read SP0A19GetSensorID: %x \r\n",sensor_id);

	*sensorID=sensor_id;
	if (sensor_id != SP0A19_YUV_SENSOR_ID)
	{
		*sensorID=0xFFFFFFFF;
		SENSORDB("Sensor Read ByeBye \r\n");
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	return ERROR_NONE;	  
}


/*                                                                        
           
             
  
              
                                                        
  
             
       
  
          
       
  
                   
  
                                                                         */
UINT32 SP0A19Open(void)
{
    kal_uint16 sensor_id=0;

     SENSORDB("SP0A19Open \n");

		
	SP0A19_write_cmos_sensor(0xfd,0x00);
    sensor_id=SP0A19_read_cmos_sensor(0x02);
    SENSORDB("SP0A19Open Sensor id = %x\n", sensor_id);
	
	if (sensor_id != SP0A19_YUV_SENSOR_ID) 
	   {
		  SENSORDB("Sensor Read ByeBye \r\n");
	      return ERROR_SENSOR_CONNECT_FAIL;
	   }

    mdelay(10);

	SP0A19_InitialPara();
    SP0A19_Sensor_Init();		
    SENSORDB("SP0A19Open end \n");
    return ERROR_NONE;
}


/*                                                                        
           
              
  
              
                                                    
  
             
       
  
          
       
  
                   
  
                                                                         */
UINT32 SP0A19Close(void)
{
   SENSORDB("SP0A19Close\n");
    return ERROR_NONE;
} 


#if 0
//                                                                                                            
static void SP0A19_set_dummy( const kal_uint32 iPixels, const kal_uint32 iLines )
{

}
#endif

static void SP0A19_set_dummy_line(const kal_uint32 iLines )
{

   kal_uint32 dummyLine= 0x00;
   
    if(dummyLine >256)
        dummyLine=256;
	
	SP0A19_write_cmos_sensor(0xfd, 0x00);
	SP0A19_write_cmos_sensor(0x06, dummyLine);
}


/*                                                                        
           
                
  
              
                                          
  
             
                                                                          
                                                                                
  
          
       
  
                   
  
                                                                         */
UINT32 SP0A19Preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
        MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)

{
	SENSORDB("SP0A19Preview enter \n");

    image_window->GrabStartX= SP0A19_IMAGE_SENSOR_START_PIXELS;
    image_window->GrabStartY= SP0A19_IMAGE_SENSOR_START_LINES;
    image_window->ExposureWindowWidth = SP0A19_IMAGE_SENSOR_PV_WIDTH;
    image_window->ExposureWindowHeight = SP0A19_IMAGE_SENSOR_PV_HEIGHT;

	SP0A19NightMode(SP0A19_CurrentStatus.iNightMode);

    //                        
    memcpy(&SP0A19SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

	SENSORDB("SP0A19Preview out \n");
    return ERROR_NONE;
} 


UINT32 SP0A19Video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
        MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)

{
	SENSORDB("SP0A19Video enter \n");

    image_window->GrabStartX= SP0A19_IMAGE_SENSOR_START_PIXELS;
    image_window->GrabStartY= SP0A19_IMAGE_SENSOR_START_LINES;
    image_window->ExposureWindowWidth = SP0A19_IMAGE_SENSOR_VIDEO_WIDTH;
    image_window->ExposureWindowHeight = SP0A19_IMAGE_SENSOR_VIDEO_HEIGHT;

    SP0A19NightMode(SP0A19_CurrentStatus.iNightMode);

    //                        
    memcpy(&SP0A19SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

	SENSORDB("SP0A19Preview out \n");
    return ERROR_NONE;
} 

/*                                                                        
           
                
  
              
                                                                
  
             
  
          
       
  
                   
  
                                                                         */
UINT32 SP0A19Capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
        MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)

{
	SENSORDB("SP0A19Capture enter \n");

    image_window->GrabStartX = SP0A19_IMAGE_SENSOR_START_PIXELS;
    image_window->GrabStartY = SP0A19_IMAGE_SENSOR_START_LINES;
    image_window->ExposureWindowWidth=  SP0A19_IMAGE_SENSOR_FULL_WIDTH;
    image_window->ExposureWindowHeight = SP0A19_IMAGE_SENSOR_FULL_HEIGHT;

    //                        
    memcpy(&SP0A19SensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	SENSORDB("SP0A19Capture out \n");
	
    return ERROR_NONE;
} /*                  */



UINT32 SP0A19GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
    pSensorResolution->SensorFullWidth=		SP0A19_IMAGE_SENSOR_FULL_WIDTH;
    pSensorResolution->SensorFullHeight=	SP0A19_IMAGE_SENSOR_FULL_HEIGHT;
    pSensorResolution->SensorPreviewWidth=	SP0A19_IMAGE_SENSOR_PV_WIDTH;
    pSensorResolution->SensorPreviewHeight=	SP0A19_IMAGE_SENSOR_PV_HEIGHT;
    pSensorResolution->SensorVideoWidth=	SP0A19_IMAGE_SENSOR_VIDEO_WIDTH;
    pSensorResolution->SensorVideoHeight=	SP0A19_IMAGE_SENSOR_VIDEO_HEIGHT;
	
    return ERROR_NONE;
}


UINT32 SP0A19GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
        MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
        MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{

    pSensorInfo->SensorCameraPreviewFrameRate=30;
    pSensorInfo->SensorVideoFrameRate=30;
    pSensorInfo->SensorStillCaptureFrameRate=10;
    pSensorInfo->SensorWebCamCaptureFrameRate=15;
    pSensorInfo->SensorResetActiveHigh=FALSE;
    pSensorInfo->SensorResetDelayCount=1;
	
    //                                                              
	//                                                              
	pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_VYUY;
	
    pSensorInfo->SensorClockPolarity=SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorClockFallingPolarity=SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
    pSensorInfo->SensorInterruptDelayLines = 1;
	
    pSensorInfo->SensroInterfaceType=SENSOR_INTERFACE_TYPE_PARALLEL;

    pSensorInfo->CaptureDelayFrame = 1;
    pSensorInfo->PreviewDelayFrame = 1;
    pSensorInfo->VideoDelayFrame = 4;
    pSensorInfo->SensorMasterClockSwitch = 0;
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;

	pSensorInfo->YUVAwbDelayFrame = 2;
	pSensorInfo->YUVEffectDelayFrame= 2;

    switch (ScenarioId)
    {
    case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
    case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        pSensorInfo->SensorClockFreq=26;
        pSensorInfo->SensorClockDividCount=	3;
        pSensorInfo->SensorClockRisingCount= 0;
        pSensorInfo->SensorClockFallingCount= 2;
        pSensorInfo->SensorPixelClockCount= 3;
        pSensorInfo->SensorDataLatchCount= 2;
        pSensorInfo->SensorGrabStartX = SP0A19_IMAGE_SENSOR_START_PIXELS;
        pSensorInfo->SensorGrabStartY = SP0A19_IMAGE_SENSOR_START_LINES;

        break;
    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        pSensorInfo->SensorClockFreq=26;
        pSensorInfo->SensorClockDividCount= 3;
        pSensorInfo->SensorClockRisingCount=0;
        pSensorInfo->SensorClockFallingCount=2;
        pSensorInfo->SensorPixelClockCount=3;
        pSensorInfo->SensorDataLatchCount=2;
        pSensorInfo->SensorGrabStartX = SP0A19_IMAGE_SENSOR_START_PIXELS;
        pSensorInfo->SensorGrabStartY = SP0A19_IMAGE_SENSOR_START_LINES;
        break;
    default:
        pSensorInfo->SensorClockFreq=26;
        pSensorInfo->SensorClockDividCount= 3;
        pSensorInfo->SensorClockRisingCount=0;
        pSensorInfo->SensorClockFallingCount=2;
        pSensorInfo->SensorPixelClockCount=3;
        pSensorInfo->SensorDataLatchCount=2;
        pSensorInfo->SensorGrabStartX = SP0A19_IMAGE_SENSOR_START_PIXELS;
        pSensorInfo->SensorGrabStartY = SP0A19_IMAGE_SENSOR_START_LINES;
        break;
    }
    memcpy(pSensorConfigData, &SP0A19SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
    return ERROR_NONE;
}


UINT32 SP0A19Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
        MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{

    SP0A19CurrentScenarioId = ScenarioId;

	SENSORDB("SP0A19Control:ScenarioId =%d \n",SP0A19CurrentScenarioId);
   
    switch (ScenarioId)
    {
	    case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_MPEG4_encode_mode = KAL_FALSE;
			spin_unlock(&sp0a19yuv_drv_lock);
	        SP0A19Preview(pImageWindow, pSensorConfigData);
	        break;
			
	    case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_MPEG4_encode_mode = KAL_TRUE;
			spin_unlock(&sp0a19yuv_drv_lock);
	        SP0A19Video(pImageWindow, pSensorConfigData);
	        break;
	    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_MPEG4_encode_mode = KAL_FALSE;
			spin_unlock(&sp0a19yuv_drv_lock);
	        SP0A19Capture(pImageWindow, pSensorConfigData);
	        break;
	    default:
	        return ERROR_INVALID_SCENARIO_ID;
    }
    return ERROR_NONE;
}	/*                 */


BOOL SP0A19_set_param_wb(UINT16 para)
{

	if(SP0A19_CurrentStatus.iWB == para)
		return TRUE;

	switch (para)
	{            
		case AWB_MODE_AUTO:
			SP0A19_write_cmos_sensor(0xfd,0x00);                                                          
			SP0A19_write_cmos_sensor(0x32,0x05);                                                          
			SP0A19_write_cmos_sensor(0xe7,0x03);                                                          
			SP0A19_write_cmos_sensor(0xfd,0x01);  
  			SP0A19_write_cmos_sensor(0x28,0xA0);                                                          
			SP0A19_write_cmos_sensor(0x29,0xA0);
			
			SP0A19_write_cmos_sensor(0xfd,0x00);   //                                                                           
			SP0A19_write_cmos_sensor(0xe7,0x00);                                                          
			SP0A19_write_cmos_sensor(0x32,0x15);                                                          
            break;
		case AWB_MODE_OFF:
	         {
			 SP0A19_write_cmos_sensor(0xfd,0x01);														   
			 SP0A19_write_cmos_sensor(0x28,0xc4);																
			 SP0A19_write_cmos_sensor(0x29,0x9e);
			 SP0A19_write_cmos_sensor(0xfd,0x00);  //                      
			 SP0A19_write_cmos_sensor(0x32,0x05); //         
		     }
			 break;

		case AWB_MODE_CLOUDY_DAYLIGHT:
		    //                          
			SP0A19_write_cmos_sensor(0xfd,0x00);   //                                          
			SP0A19_write_cmos_sensor(0x32,0x05);                                                          
			SP0A19_write_cmos_sensor(0xfd,0x01);                                                          
			SP0A19_write_cmos_sensor(0x28,0xbf);		                                                       
			SP0A19_write_cmos_sensor(0x29,0x89);		                                                       
			SP0A19_write_cmos_sensor(0xfd,0x00);   
		    break;
		case AWB_MODE_DAYLIGHT:
	        //                          
			SP0A19_write_cmos_sensor(0xfd,0x00);  //                                          
			SP0A19_write_cmos_sensor(0x32,0x05);                                                          
			SP0A19_write_cmos_sensor(0xfd,0x01);                                                          
			SP0A19_write_cmos_sensor(0x28,0xbc);		                                                       
			SP0A19_write_cmos_sensor(0x29,0x5d);		                                                       
			SP0A19_write_cmos_sensor(0xfd,0x00); 
		    break;
		case AWB_MODE_INCANDESCENT:	
		    //                           
			SP0A19_write_cmos_sensor(0xfd,0x00);  //                                                
			SP0A19_write_cmos_sensor(0x32,0x05);                                                          
			SP0A19_write_cmos_sensor(0xfd,0x01);                                                          
			SP0A19_write_cmos_sensor(0x28,0x89);		                                                       
			SP0A19_write_cmos_sensor(0x29,0xb8);		                                                       
			SP0A19_write_cmos_sensor(0xfd,0x00); 
		    break;  
		case AWB_MODE_FLUORESCENT:
	        //                           
			SP0A19_write_cmos_sensor(0xfd,0x00);  //                                                
			SP0A19_write_cmos_sensor(0x32,0x05);                                                          
			SP0A19_write_cmos_sensor(0xfd,0x01);                                                          
			SP0A19_write_cmos_sensor(0x28,0xb5);		                                                       
			SP0A19_write_cmos_sensor(0x29,0xa5);		                                                       
			SP0A19_write_cmos_sensor(0xfd,0x00); 
		    break;  
		case AWB_MODE_TUNGSTEN:
	        //                          
			SP0A19_write_cmos_sensor(0xfd,0x00);  //                                        
			SP0A19_write_cmos_sensor(0x32,0x05);                                                          
			SP0A19_write_cmos_sensor(0xfd,0x01);                                                          
			SP0A19_write_cmos_sensor(0x28,0xaf);		                                                       
			SP0A19_write_cmos_sensor(0x29,0x99);		                                                       
			SP0A19_write_cmos_sensor(0xfd,0x00);  
		    break;
		default:
			return KAL_FALSE;
	}
	spin_lock(&sp0a19yuv_drv_lock);
	SP0A19_CurrentStatus.iWB = para;
	spin_unlock(&sp0a19yuv_drv_lock);
	
	return KAL_TRUE;
}


BOOL SP0A19_set_param_effect(UINT16 para)
{

	
	if(SP0A19_CurrentStatus.iEffect == para)
		return TRUE;


	SP0A19_write_cmos_sensor(0xfd, 0x00);
	SP0A19_write_cmos_sensor(0x36, 0x20);
	SP0A19_write_cmos_sensor(0x06, 0xff);
	mdelay(200);
    //                             

	switch (para)
	{
		case MEFFECT_OFF:
            SP0A19_write_cmos_sensor(0xfd, 0x00);
			SP0A19_write_cmos_sensor(0x62, 0x00);
			SP0A19_write_cmos_sensor(0x63, 0x80);
			SP0A19_write_cmos_sensor(0x64, 0x80);
	        break;
		case MEFFECT_SEPIA:
            SP0A19_write_cmos_sensor(0xfd, 0x00);
			SP0A19_write_cmos_sensor(0x62, 0x10);
			SP0A19_write_cmos_sensor(0x63, 0xc0);
			SP0A19_write_cmos_sensor(0x64, 0x20);
			break;  
		case MEFFECT_NEGATIVE:
            SP0A19_write_cmos_sensor(0xfd, 0x00);
			SP0A19_write_cmos_sensor(0x62, 0x04);
			SP0A19_write_cmos_sensor(0x63, 0x80);
			SP0A19_write_cmos_sensor(0x64, 0x80);
			break; 
		case MEFFECT_SEPIAGREEN:		
	        SP0A19_write_cmos_sensor(0xfd, 0x00);
			SP0A19_write_cmos_sensor(0x62, 0x10);
			SP0A19_write_cmos_sensor(0x63, 0x20);
			SP0A19_write_cmos_sensor(0x64, 0x20);
			break;
		case MEFFECT_SEPIABLUE:
		  	SP0A19_write_cmos_sensor(0xfd, 0x00);
			SP0A19_write_cmos_sensor(0x62, 0x10);
			SP0A19_write_cmos_sensor(0x63, 0x20);
			SP0A19_write_cmos_sensor(0x64, 0xf0);
			break;        
		case MEFFECT_MONO:			
			SP0A19_write_cmos_sensor(0xfd, 0x00);
			SP0A19_write_cmos_sensor(0x62, 0x20);
			SP0A19_write_cmos_sensor(0x63, 0x80);
			SP0A19_write_cmos_sensor(0x64, 0x80);
			break;
		default:
			return KAL_FALSE;
	}

	SP0A19_write_cmos_sensor(0x36, 0x00);
	SP0A19_write_cmos_sensor(0x06, 0x00);
	spin_lock(&sp0a19yuv_drv_lock);
	SP0A19_CurrentStatus.iEffect = para;
	spin_unlock(&sp0a19yuv_drv_lock);

	return KAL_TRUE;
}


BOOL SP0A19_set_param_banding(UINT16 para)
{
	if(SP0A19_CurrentStatus.iBanding == para)
      return TRUE;

	switch (para)
	{
		case AE_FLICKER_MODE_50HZ:
			spin_lock(&sp0a19yuv_drv_lock);
			sp0a19_isBanding = 0;
			spin_unlock(&sp0a19yuv_drv_lock);
			break;

		case AE_FLICKER_MODE_60HZ:
			spin_lock(&sp0a19yuv_drv_lock);
			sp0a19_isBanding = 1;
			spin_unlock(&sp0a19yuv_drv_lock);
		break;
		default:
			return FALSE;
	}
	 SP0A19NightMode(SP0A19_CurrentStatus.iNightMode);
	spin_lock(&sp0a19yuv_drv_lock);
	SP0A19_CurrentStatus.iBanding =para;
	spin_unlock(&sp0a19yuv_drv_lock);

	return TRUE;
} /*                          */


BOOL SP0A19_set_param_exposure(UINT16 para)
{
	if(SP0A19_CurrentStatus.iEV == para)
      return TRUE;

	switch (para)
	{
		case AE_EV_COMP_13:  
		case AE_EV_COMP_10:  
		case AE_EV_COMP_07:  
		case AE_EV_COMP_03: 
			//         
			  SP0A19_write_cmos_sensor(0xfd,0x00); 
			  SP0A19_write_cmos_sensor(0xed,0x8c+0x18); //                     
			  SP0A19_write_cmos_sensor(0xf7,0x88+0x18); //                
			  SP0A19_write_cmos_sensor(0xf8,0x80+0x18);
			  SP0A19_write_cmos_sensor(0xec,0x7c+0x18); //                   
			  
			  SP0A19_write_cmos_sensor(0xef,0x74+0x18); //        
			  SP0A19_write_cmos_sensor(0xf9,0x70+0x18); 
			  SP0A19_write_cmos_sensor(0xfa,0x68+0x18); 
			  SP0A19_write_cmos_sensor(0xee,0x64+0x18);
			break;
		case AE_EV_COMP_00:  //      
			//         
			  SP0A19_write_cmos_sensor(0xfd,0x00); 
			  SP0A19_write_cmos_sensor(0xed,0x8c); 
			  SP0A19_write_cmos_sensor(0xf7,0x88); 
			  SP0A19_write_cmos_sensor(0xf8,0x80); 
			  SP0A19_write_cmos_sensor(0xec,0x7c); 
			  
			  SP0A19_write_cmos_sensor(0xef,0x74); 
			  SP0A19_write_cmos_sensor(0xf9,0x70); 
			  SP0A19_write_cmos_sensor(0xfa,0x68); 
			  SP0A19_write_cmos_sensor(0xee,0x64); 
			break;
		case AE_EV_COMP_n03:  
		case AE_EV_COMP_n07:	
		case AE_EV_COMP_n10:  
		case AE_EV_COMP_n13:
			//         
			  SP0A19_write_cmos_sensor(0xfd,0x00); 
			  SP0A19_write_cmos_sensor(0xed,0x8c-0x18); 
			  SP0A19_write_cmos_sensor(0xf7,0x88-0x18); 
			  SP0A19_write_cmos_sensor(0xf8,0x80-0x18); 
			  SP0A19_write_cmos_sensor(0xec,0x7c-0x18); 
			  
			  SP0A19_write_cmos_sensor(0xef,0x74-0x18); 
			  SP0A19_write_cmos_sensor(0xf9,0x70-0x18); 
			  SP0A19_write_cmos_sensor(0xfa,0x68-0x18); 
			  SP0A19_write_cmos_sensor(0xee,0x64-0x18); 
			break;
		default:
			return KAL_FALSE;
	}
	spin_lock(&sp0a19yuv_drv_lock);
	SP0A19_CurrentStatus.iEV = para;
	spin_unlock(&sp0a19yuv_drv_lock);
	return KAL_TRUE;
} 




void SP0A19_set_saturation(UINT16 para)
{
    switch (para)
    {

		SP0A19_write_cmos_sensor(0xfd,0x00); 
		
        case ISP_SAT_HIGH:
			SP0A19_write_cmos_sensor(0xd3,0x88+0x20); //        
			SP0A19_write_cmos_sensor(0xd4,0x88+0x20); //       
			SP0A19_write_cmos_sensor(0xd6,0x70+0x20); //     
			SP0A19_write_cmos_sensor(0xd7,0x60+0x20); //         
             break;
			 
        case ISP_SAT_LOW:
			SP0A19_write_cmos_sensor(0xd3,0x88-0x20); 
			SP0A19_write_cmos_sensor(0xd4,0x88-0x20); 
			SP0A19_write_cmos_sensor(0xd6,0x70-0x20); 
			SP0A19_write_cmos_sensor(0xd7,0x60-0x20);
             break;
			 
        case ISP_SAT_MIDDLE:
        default:
			SP0A19_write_cmos_sensor(0xd3,0x88);
			SP0A19_write_cmos_sensor(0xd4,0x88); 
			SP0A19_write_cmos_sensor(0xd6,0x70); 
			SP0A19_write_cmos_sensor(0xd7,0x60);
             break;
    }
     return;
}

void SP0A19_set_contrast(UINT16 para)
{
	//     
	switch (para)
	{
		SP0A19_write_cmos_sensor(0xfd,0x00);

		case ISP_CONTRAST_LOW:
			SP0A19_write_cmos_sensor(0xdd,0x80-0x10); 
			SP0A19_write_cmos_sensor(0xde,0xa8-0x20); 
			 break;
		case ISP_CONTRAST_HIGH:
			SP0A19_write_cmos_sensor(0xd4,0x80+0x10); 
			SP0A19_write_cmos_sensor(0xd6,0xa8+0x20); 
			 break;
		case ISP_CONTRAST_MIDDLE:
		default:
			SP0A19_write_cmos_sensor(0xdd,0x80); 
			SP0A19_write_cmos_sensor(0xde,0xa8); 
			 break;
	}
	
	return;
}


void SP0A19_set_brightness(UINT16 para)
{
    //         
    switch (para)
    {
	    SP0A19_write_cmos_sensor(0xfd,0x00); 
		
        case ISP_BRIGHT_LOW:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_CurrentStatus.iBrightness = ISP_BRIGHT_LOW;
			spin_unlock(&sp0a19yuv_drv_lock);
			 SP0A19_write_cmos_sensor(0xdc,0xe0); 
             break;
        case ISP_BRIGHT_HIGH:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_CurrentStatus.iBrightness = ISP_BRIGHT_HIGH;
			spin_unlock(&sp0a19yuv_drv_lock);
			 SP0A19_write_cmos_sensor(0xfd,0x00+0x10); 
             break;
        case ISP_BRIGHT_MIDDLE:
        default:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_CurrentStatus.iBrightness = ISP_BRIGHT_MIDDLE;
			spin_unlock(&sp0a19yuv_drv_lock);
			SP0A19_write_cmos_sensor(0xfd,0x00); 
             break;
    }
	//                            
    return;
}


void SP0A19_set_iso(UINT16 para)
{
    //         
    switch (para)
    {
        case AE_ISO_100:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_CurrentStatus.iIso =AE_ISO_100;
			spin_unlock(&sp0a19yuv_drv_lock);
             break;
        case AE_ISO_200:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_CurrentStatus.iIso =AE_ISO_200;
			spin_unlock(&sp0a19yuv_drv_lock);
             break;
        case AE_ISO_400:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_CurrentStatus.iIso =AE_ISO_400;
			spin_unlock(&sp0a19yuv_drv_lock);
             break;
        default:
        case AE_ISO_AUTO:
			spin_lock(&sp0a19yuv_drv_lock);
			SP0A19_CurrentStatus.iIso =AE_ISO_AUTO;
			spin_unlock(&sp0a19yuv_drv_lock);
             break;
    }
	//                            
    return;
}


void SP0A19_AE_Lock_Enable(UINT16 iPara)
{
	kal_uint16 temp_AE_reg = 0;

	SP0A19_write_cmos_sensor(0xfd, 0x00);
	temp_AE_reg = SP0A19_read_cmos_sensor(0x32);

	switch(iPara)
	{

		case AE_MODE_OFF:
			SP0A19_write_cmos_sensor(0x32, (temp_AE_reg & (~0x01)));
			break;

		//                
		default:
			SP0A19_write_cmos_sensor(0x32, (temp_AE_reg |0x01));
			break;

	}		
}


UINT32 SP0A19YUVSensorSetting(FEATURE_ID iCmd, UINT16 iPara)
{
    switch (iCmd) {
		
	case FID_COLOR_EFFECT:
		SP0A19_set_param_effect(iPara);
		break;

    case FID_AE_EV:
        SP0A19_set_param_exposure(iPara);
        break;

	case FID_SCENE_MODE:
		SP0A19NightMode(iPara);
        break;
		
    case FID_AWB_MODE:
        SP0A19_set_param_wb(iPara);
        break;

    case FID_AE_FLICKER:
        SP0A19_set_param_banding(iPara);
		break;

	case FID_ISP_SAT:
		SP0A19_set_saturation(iPara);
		break;
		
	case FID_ISP_CONTRAST:
		SP0A19_set_contrast(iPara);
		break;

	case FID_ISP_BRIGHT:
		SP0A19_set_brightness(iPara);
		break;
		
	case FID_AE_ISO:
		SP0A19_set_iso(iPara);
		break;
	
	case FID_AE_SCENE_MODE:
		SP0A19_AE_Lock_Enable(iPara);
		break;

	case FID_ZOOM_FACTOR:
		spin_lock(&sp0a19yuv_drv_lock);
		zoom_factor = iPara; 
		spin_unlock(&sp0a19yuv_drv_lock);
		break; 
    default:
        break;
    }
    return TRUE;
} /*                        */


UINT32 SP0A19YUVSetVideoMode(UINT16 u2FrameRate)
{
    spin_lock(&sp0a19yuv_drv_lock);
	SP0A19_MPEG4_encode_mode = KAL_TRUE;
	spin_unlock(&sp0a19yuv_drv_lock);

	//        
	SP0A19NightMode(SP0A19_CurrentStatus.iNightMode);
	
    return ERROR_NONE;
}


/*                                                                        
          
                      
 
             
         
         
 
                
 
                                                                        */
static void SP0A19GetEvAwbRef(UINT32 pSensorAEAWBRefStruct)//              
{
    PSENSOR_AE_AWB_REF_STRUCT Ref = (PSENSOR_AE_AWB_REF_STRUCT)pSensorAEAWBRefStruct;
    SENSORDB("AP0A19GetEvAwbRef  \n" );
    	
	Ref->SensorAERef.AeRefLV05Shutter = 1530;
    Ref->SensorAERef.AeRefLV05Gain = 1280; /*          */ 
    Ref->SensorAERef.AeRefLV13Shutter = 61;
    Ref->SensorAERef.AeRefLV13Gain = 320; /*           */
    Ref->SensorAwbGainRef.AwbRefD65Rgain = 197; /*          */
    Ref->SensorAwbGainRef.AwbRefD65Bgain = 134; /*          */
    Ref->SensorAwbGainRef.AwbRefCWFRgain = 177; /*          */
    Ref->SensorAwbGainRef.AwbRefCWFBgain = 183; /*          */
}

kal_uint16 SP0A19ReadGain(void)
{
    kal_uint32 temp=0,Sensor_GainBase=16,Feature_GainBase=128;

	SP0A19_write_cmos_sensor(0xfd, 0x00);
	temp = SP0A19_read_cmos_sensor(0x23);//     
	//                                                
	
	temp = temp*Feature_GainBase/Sensor_GainBase;
	//                                           

	return temp;
}


kal_uint16 SP0A19ReadAwbRGain(void)
{
    kal_uint32 temp=0,Sensor_GainBase=128,Feature_GainBase=128;
	
	SP0A19_write_cmos_sensor(0xfd, 0x01);
	temp = SP0A19_read_cmos_sensor(0x28);//     
	
	temp = temp*Feature_GainBase/Sensor_GainBase;
	//                                                 

	return temp;
}

kal_uint16 SP0A19ReadAwbBGain(void)
{
    kal_uint16 temp=0x0000,Sensor_GainBase=128,Feature_GainBase=128;
	
	SP0A19_write_cmos_sensor(0xfd, 0x01);
	temp = SP0A19_read_cmos_sensor(0x29);//     
	
	temp = temp*Feature_GainBase/Sensor_GainBase;
	//                                                 

	return temp;
}


/*                                                                        
          
                          
 
             
         
         
 
                
 
                                                                        */
static void SP0A19GetCurAeAwbInfo(UINT32 pSensorAEAWBCurStruct)
{
    PSENSOR_AE_AWB_CUR_STRUCT Info = (PSENSOR_AE_AWB_CUR_STRUCT)pSensorAEAWBCurStruct;
    SENSORDB("AP0A19GetCurAeAwbInfo  \n" );

    Info->SensorAECur.AeCurShutter = SP0A19_Read_Shutter();
    Info->SensorAECur.AeCurGain = SP0A19ReadGain(); /*          */
    
    Info->SensorAwbGainCur.AwbCurRgain = SP0A19ReadAwbRGain(); /*          */
    
    Info->SensorAwbGainCur.AwbCurBgain = SP0A19ReadAwbBGain(); /*          */
}


void SP0A19GetAFMaxNumFocusAreas(UINT32 *pFeatureReturnPara32)
{	
    *pFeatureReturnPara32 = 0;    
    SENSORDB("SP0A19GetAFMaxNumFocusAreas, *pFeatureReturnPara32 = %d\n",*pFeatureReturnPara32);

}


void SP0A19GetAFMaxNumMeteringAreas(UINT32 *pFeatureReturnPara32)
{	
    *pFeatureReturnPara32 = 0;    
    SENSORDB("SP0A19GetAFMaxNumMeteringAreas,*pFeatureReturnPara32 = %d\n",*pFeatureReturnPara32);

}


void SP0A19GetExifInfo(UINT32 exifAddr)
{
    SENSOR_EXIF_INFO_STRUCT* pExifInfo = (SENSOR_EXIF_INFO_STRUCT*)exifAddr;
    pExifInfo->FNumber = 28;
    pExifInfo->AEISOSpeed = AE_ISO_100;
    pExifInfo->AWBMode = SP0A19_CurrentStatus.iWB;
    pExifInfo->CapExposureTime = SP0A19_Read_Shutter_for_exif();
    pExifInfo->FlashLightTimeus = 0;
    pExifInfo->RealISOValue = SP0A19_Read_ISO_for_exif();
}


void SP0A19GetDelayInfo(UINT32 delayAddr)
{
    SENSOR_DELAY_INFO_STRUCT* pDelayInfo = (SENSOR_DELAY_INFO_STRUCT*)delayAddr;
    pDelayInfo->InitDelay = 3;
    pDelayInfo->EffectDelay = 2;
    pDelayInfo->AwbDelay = 2;
   //                                     
}


void SP0A19GetAEAWBLock(UINT32 *pAElockRet32,UINT32 *pAWBlockRet32)
{
    *pAElockRet32 = 1;
	*pAWBlockRet32 = 1;
    SENSORDB("SP0A19GetAEAWBLock,AE=%d ,AWB=%d\n,",*pAElockRet32,*pAWBlockRet32);
}


UINT32 SP0A19_SetTestPatternMode(kal_bool bEnable)
{
    SENSORDB("[SP0A19_SetTestPatternMode] Test pattern enable:%d\n", bEnable);

	SP0A19_write_cmos_sensor(0xfd, 0x00);

	if(bEnable) //                          
	{
		SP0A19_write_cmos_sensor(0x0d, 0x10);//            
		//                                                  

	}
	else //                               
	{
		SP0A19_write_cmos_sensor(0x0d, 0x00);//            
	}
    return ERROR_NONE;
}



kal_uint32 SP0A19GetLineLength(void)
{

	kal_uint32 Current_LineLength = 0;

	SP0A19_write_cmos_sensor(0xfd, 0x00);

	//                                                                             
    Current_LineLength = ((SP0A19_read_cmos_sensor(0x09)&0x0f)<<8)|(SP0A19_read_cmos_sensor(0x08));
	Current_LineLength =Current_LineLength + SP0A19_VGA_PERIOD_PIXEL_NUMS;

	SENSORDB("SP0A19GetLineLength: Current_LineLength = %d\n",Current_LineLength);
    return Current_LineLength;
}


UINT32 SP0A19SetMaxFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 frameRate) {
	kal_uint32 pclk;
	kal_int16 dummyLine;
	kal_uint16 lineLength,frameHeight;
		
	SENSORDB("SP0A19SetMaxFramerateByScenario: scenarioId = %d, frame rate = %d\n",scenarioId,frameRate);

	switch (scenarioId) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			pclk = SP0A19_PCLK;
			
			//                                          
			lineLength = SP0A19GetLineLength();
			
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - SP0A19_VGA_PERIOD_LINE_NUMS;
			if(dummyLine<0)
				dummyLine = 0;
			SENSORDB("SP0A19SetMaxFramerateByScenario MSDK_SCENARIO_ID_CAMERA_PREVIEW: lineLength = %d, dummy=%d\n",lineLength,dummyLine);

			//                                
			SP0A19_set_dummy_line(dummyLine);
			
			break;			
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			pclk = SP0A19_PCLK;
			
			//                                          
			lineLength = SP0A19GetLineLength();
			
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - SP0A19_VGA_PERIOD_LINE_NUMS;
			if(dummyLine<0)
				dummyLine = 0;
			SENSORDB("SP0A19SetMaxFramerateByScenario MSDK_SCENARIO_ID_VIDEO_PREVIEW: lineLength = %d, dummy=%d\n",lineLength,dummyLine);			
			//                                
			SP0A19_set_dummy_line(dummyLine);
			break;			
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:			
			pclk = SP0A19_PCLK;
			
			//                                          
			lineLength = SP0A19GetLineLength();
			
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - SP0A19_VGA_PERIOD_LINE_NUMS;
			if(dummyLine<0)
				dummyLine = 0;
			SENSORDB("SP0A19SetMaxFramerateByScenario MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG: lineLength = %d, dummy=%d\n",lineLength,dummyLine);			
			//                               
			SP0A19_set_dummy_line(dummyLine);
			
			break;		
        case MSDK_SCENARIO_ID_CAMERA_3D_PREVIEW: //     
            break;
        case MSDK_SCENARIO_ID_CAMERA_3D_VIDEO:
			break;
        case MSDK_SCENARIO_ID_CAMERA_3D_CAPTURE: //        
			break;		
		default:
			break;
	}	

	return ERROR_NONE;
}



UINT32 SP0A19GetDefaultFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 *pframeRate) 
{

	switch (scenarioId) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			 *pframeRate = 300;
			 break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
			 *pframeRate = 300;
			break;		
        case MSDK_SCENARIO_ID_CAMERA_3D_PREVIEW: //     
        case MSDK_SCENARIO_ID_CAMERA_3D_VIDEO:
        case MSDK_SCENARIO_ID_CAMERA_3D_CAPTURE: //        
			 *pframeRate = 300;
			break;		
		default:
			break;
	}

	return ERROR_NONE;
}


UINT32 SP0A19FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
        UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
    UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
    UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
    UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
    UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
    MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
    MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;

    switch (FeatureId)
    {
    case SENSOR_FEATURE_GET_RESOLUTION:
        *pFeatureReturnPara16++=SP0A19_IMAGE_SENSOR_FULL_WIDTH;
        *pFeatureReturnPara16=SP0A19_IMAGE_SENSOR_FULL_HEIGHT;
        *pFeatureParaLen=4;
        break;
    case SENSOR_FEATURE_GET_PERIOD:
        //                                                                         
        //                                                                     
        //                   
        break;
    case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
        *pFeatureReturnPara32 = SP0A19_PCLK;
        *pFeatureParaLen=4;
        break;
    case SENSOR_FEATURE_SET_ESHUTTER:
        break;
    case SENSOR_FEATURE_SET_NIGHTMODE:
        SP0A19NightMode((BOOL) *pFeatureData16);
        break;

	case SENSOR_FEATURE_SET_VIDEO_MODE:
		SP0A19YUVSetVideoMode(*pFeatureData16);
		break; 
		
    case SENSOR_FEATURE_SET_GAIN:
    case SENSOR_FEATURE_SET_FLASHLIGHT:
        break;
    case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
		spin_lock(&sp0a19yuv_drv_lock);
        SP0A19_isp_master_clock=*pFeatureData32;
		spin_unlock(&sp0a19yuv_drv_lock);
        break;
    case SENSOR_FEATURE_SET_REGISTER:
        SP0A19_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
        break;
    case SENSOR_FEATURE_GET_REGISTER:
        pSensorRegData->RegData = SP0A19_read_cmos_sensor(pSensorRegData->RegAddr);
        break;
    case SENSOR_FEATURE_GET_CONFIG_PARA:
        memcpy(pSensorConfigData, &SP0A19SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
        *pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
        break;
    case SENSOR_FEATURE_SET_CCT_REGISTER:
    case SENSOR_FEATURE_GET_CCT_REGISTER:
    case SENSOR_FEATURE_SET_ENG_REGISTER:
    case SENSOR_FEATURE_GET_ENG_REGISTER:
    case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
    case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
    case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
    case SENSOR_FEATURE_GET_GROUP_INFO:
    case SENSOR_FEATURE_GET_ITEM_INFO:
    case SENSOR_FEATURE_SET_ITEM_INFO:
    case SENSOR_FEATURE_GET_ENG_INFO:
        break;
	case SENSOR_FEATURE_GET_GROUP_COUNT:
        *pFeatureReturnPara32++=0;
        *pFeatureParaLen=4;	   
		break;
		
    case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
        //                                                                             
        //                                           
        *pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
        *pFeatureParaLen=4;
        break;
    case SENSOR_FEATURE_SET_YUV_CMD:
        SP0A19YUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32+1));
        break;
    case SENSOR_FEATURE_CHECK_SENSOR_ID:
		SP0A19GetSensorID(pFeatureData32);
		break;

	case SENSOR_FEATURE_GET_EV_AWB_REF:
		 SP0A19GetEvAwbRef(*pFeatureData32);
		break;

	case SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN:
		 SP0A19GetCurAeAwbInfo(*pFeatureData32);	
		 break;

	case SENSOR_FEATURE_GET_AF_MAX_NUM_FOCUS_AREAS:
		 SP0A19GetAFMaxNumFocusAreas(pFeatureReturnPara32);		   
		 *pFeatureParaLen=4;
		 break;	   

	case SENSOR_FEATURE_GET_AE_MAX_NUM_METERING_AREAS:
		 SP0A19GetAFMaxNumMeteringAreas(pFeatureReturnPara32);			  
		 *pFeatureParaLen=4;
		 break;

	case SENSOR_FEATURE_GET_EXIF_INFO:
		SENSORDB("SENSOR_FEATURE_GET_EXIF_INFO\n");
		SENSORDB("EXIF addr = 0x%x\n",*pFeatureData32); 		 
		SP0A19GetExifInfo(*pFeatureData32);
		break;

	case SENSOR_FEATURE_GET_DELAY_INFO:
		SENSORDB("SENSOR_FEATURE_GET_DELAY_INFO\n");
		SP0A19GetDelayInfo(*pFeatureData32);
		break;

	case SENSOR_FEATURE_GET_AE_AWB_LOCK_INFO:
		SP0A19GetAEAWBLock((UINT32*)(*pFeatureData32),(UINT32*)*(pFeatureData32+1));
		break;

	case SENSOR_FEATURE_SET_TEST_PATTERN:
		 SP0A19_SetTestPatternMode((BOOL)*pFeatureData16);
		 break;

	case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:	   
		 *pFeatureReturnPara32= SP0A19_TEST_PATTERN_CHECKSUM; 		  
		 *pFeatureParaLen=4;							 
		 break;
	
	case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
		SP0A19SetMaxFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32, *(pFeatureData32+1));
		break;
	case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
		SP0A19GetDefaultFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32, (MUINT32 *)(*(pFeatureData32+1)));
		break;
    default:
        break;
	}
return ERROR_NONE;
}	/*                        */


SENSOR_FUNCTION_STRUCT	SensorFuncSP0A19YUV=
{
	SP0A19Open,
	SP0A19GetInfo,
	SP0A19GetResolution,
	SP0A19FeatureControl,
	SP0A19Control,
	SP0A19Close
};


UINT32 SP0A19_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/*                                  */
	if (pfFunc!=NULL)
		*pfFunc=&SensorFuncSP0A19YUV;
	return ERROR_NONE;
} /*              */
