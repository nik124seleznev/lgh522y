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
//                       
#include <linux/xlog.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h" 
#include "kd_imgsensor_errcode.h"

#include "ov8865mipiraw_Sensor.h"

extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);


#define PFX "OV8865_2lane_camera_sensor"
//                                                                                                         
//                                                                                                        
//                                                                                                          
#define LOG_INF(format, args...)	xlog_printk(ANDROID_LOG_INFO   , PFX, "[%s] " format, __FUNCTION__, ##args)

static DEFINE_SPINLOCK(imgsensor_drv_lock);


static imgsensor_info_struct imgsensor_info = { 
	.sensor_id = OV8865_SENSOR_ID,
	
	.checksum_value = 0x8d9f4a5f,//
	
	.pre = {
		.pclk = 74400000,									//                            
		.linelength = 1923,  /*                  */			//                                  
		.framelength = 1248,								//                                   
		.startx = 0,										//                                            
		.starty = 0,										//                                            
		.grabwindow_width = 1632,							//                                           
		.grabwindow_height = 1224,							//                                            
		/*                                                                               */
		.mipi_data_lp2hs_settle_dc = 23,
		/*                                                */
		.max_framerate = 300,	
	},
	.cap = {//
		.pclk = 74400000,//                                                 
		.linelength = 2008,  /*                  */	
		.framelength = 2470,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 3264,
		.grabwindow_height = 2448,
		.mipi_data_lp2hs_settle_dc = 23,
		.max_framerate = 150,
	},
	.cap1 = {//                   
		.pclk = 74400000,//                                                 
		.linelength = 2008,
		.framelength = 2470,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 3264,
		.grabwindow_height = 2448,
		.mipi_data_lp2hs_settle_dc = 23,
		.max_framerate = 150,	//                                                                                                               
	},
	.normal_video = {
		.pclk = 74400000,
		.linelength = 1923,       /*                  */	
		.framelength = 1248,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 1632,
		.grabwindow_height = 1224,
		.mipi_data_lp2hs_settle_dc = 23,
		.max_framerate = 300,
	},
	.hs_video = {//                                    
		.pclk = 74400000,
		.linelength = 1923,
		.framelength = 1248,
		.startx = 0,
		.starty = 0,
		.grabwindow_width = 1632,
		.grabwindow_height = 1224,
		.mipi_data_lp2hs_settle_dc = 23,
		.max_framerate = 300,
	},
	.slim_video = { //                     
		.pclk = 74400000,
		.linelength = 1923,  /*                  */			//                                  
		.framelength = 1248,								//                                   
		.startx = 0,										//                                            
		.starty = 0,										//                                            
		.grabwindow_width = 1632,							//                                           
		.grabwindow_height = 1224,							//                                            
		.mipi_data_lp2hs_settle_dc = 23,
		.max_framerate = 300,
	},
	
	.margin = 6,
	.min_shutter = 1,
	.max_frame_length = 0xffff,
	.ae_shut_delay_frame = 0,
	.ae_sensor_gain_delay_frame = 0,
	.ae_ispGain_delay_frame = 2,
	.ihdr_support = 0,	  //                         
	.ihdr_le_firstline = 0,  //                        
	.sensor_mode_num = 5,	  //                                                  
	
	.cap_delay_frame = 2, 
	.pre_delay_frame = 2, 
	.video_delay_frame = 2,
	.hs_video_delay_frame = 2,
	.slim_video_delay_frame = 2,
	
	.isp_driving_current = ISP_DRIVING_6MA,
	.sensor_interface_type = SENSOR_INTERFACE_TYPE_MIPI,
	//                                                                           
	//                                                                                                      
	.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_B,
	.mclk = 24,
	.mipi_lane_num = SENSOR_MIPI_2_LANE,
	.i2c_addr_table = {0x20,0xff},
};




static imgsensor_struct imgsensor = {
	.mirror = IMAGE_HV_MIRROR,				//                      
	.sensor_mode = IMGSENSOR_MODE_INIT, //                                                                                                                        
	.shutter = 0x3D0,					//               
	.gain = 0x100,						//            
	.dummy_pixel = 0,					//                  
	.dummy_line = 0,					//                 
	.current_fps = 0,  //                                                              
	.autoflicker_en = KAL_FALSE,  //                                                                                         
	.test_pattern = KAL_FALSE,		//                          
	.current_scenario_id = MSDK_SCENARIO_ID_CAMERA_PREVIEW,//                   
	.ihdr_en = 0, //                                           
	.i2c_write_id = 0x20,
};


/*                                  */
static SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[5] =	
{{ 3272, 2448, 0, 0, 3272, 2448,    1636,1224,0,0,  1636,1224,	  0,0,1632,1224}, //         
 { 3272, 2448, 0, 0, 3272, 2448, 	3272,2448,0,0,  3272,2448,	  0,0,3264,2448}, //        
 { 3272, 2448, 0, 0, 3272, 2448,    1636,1224,0,0,  1636,1224,	  0,0,1632,1224},//       
 { 3272, 2448, 0, 0, 3272, 2448,    1636,1224,0,0,  1636,1224,	  0,0,1632,1224},//                 
 { 3272, 2448, 0, 0, 3272, 2448,    1636,1224,0,0,  1636,1224,	  0,0,1632,1224}};//            


static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte=0;

	char pu_send_cmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };
	//                                                                       
    iReadReg((u16) addr, (u8*)&get_byte, imgsensor.i2c_write_id);
    
	return get_byte;
}

static void write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
	//                                                                                    
	//                                                     
	iWriteReg((u16) addr , (u32) para , 1, imgsensor.i2c_write_id);
}

static void set_dummy()
{
	LOG_INF("dummyline = %d, dummypixels = %d \n", imgsensor.dummy_line, imgsensor.dummy_pixel);

	write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
	write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);	  
	write_cmos_sensor(0x380c, imgsensor.line_length >> 8);
	write_cmos_sensor(0x380d, imgsensor.line_length & 0xFF);
	
}	/*            */


static void set_max_framerate(UINT16 framerate,kal_bool min_framelength_en)
{
	kal_int16 dummy_line;
	kal_uint32 frame_length = imgsensor.frame_length;

	LOG_INF("framerate = %d, min framelength should enable? \n", framerate,min_framelength_en);
   
   frame_length = imgsensor.pclk / framerate * 10 / imgsensor.line_length;
   
	spin_lock(&imgsensor_drv_lock);
	imgsensor.frame_length = (frame_length > imgsensor.min_frame_length) ? frame_length : imgsensor.min_frame_length; 
	imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
	//                                                       
	//                   
		//                         
	//    
		//                                  
	//                                                             
	
	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
	{
		imgsensor.frame_length = imgsensor_info.max_frame_length;
		imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
	}
	if (min_framelength_en)
		imgsensor.min_frame_length = imgsensor.frame_length;
	spin_unlock(&imgsensor_drv_lock);
	set_dummy();
}	/*                    */


static void write_shutter(kal_uint16 shutter)
{
	kal_uint16 realtime_fps = 0;
	
	//                                                                      
	spin_lock(&imgsensor_drv_lock);
	if (shutter > imgsensor.min_frame_length - imgsensor_info.margin)		
		imgsensor.frame_length = shutter + imgsensor_info.margin;
	else
		imgsensor.frame_length = imgsensor.min_frame_length;
	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		imgsensor.frame_length = imgsensor_info.max_frame_length;
	spin_unlock(&imgsensor_drv_lock);

	
	shutter = (shutter < imgsensor_info.min_shutter) ? imgsensor_info.min_shutter : shutter;
	shutter = (shutter > (imgsensor_info.max_frame_length - imgsensor_info.margin)) ? (imgsensor_info.max_frame_length - imgsensor_info.margin) : shutter;
	
	if (imgsensor.autoflicker_en){ 
		realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
		if(realtime_fps >= 297 && realtime_fps <= 305)
			set_max_framerate(296,0);
		else if(realtime_fps >= 147 && realtime_fps <= 150)
			set_max_framerate(146,0);
		else{
			write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
			write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);	  
		}
	} else {
		//                    
		write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
	}

	//               
	write_cmos_sensor(0x3500, (shutter>>12) & 0x0F);
	write_cmos_sensor(0x3501, (shutter>>4) & 0xFF);
	write_cmos_sensor(0x3502, (shutter<<4) & 0xF0);	
	
	LOG_INF("Exit! shutter =%d, framelength =%d\n", shutter,imgsensor.frame_length);

	//                                            
	
}	/*                */



/*                                                                        
          
             
 
             
                                                                
 
            
                            
 
         
      
 
                  
 
                                                                        */
static void set_shutter(kal_uint16 shutter)
{
	unsigned long flags;
	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);
	
	write_shutter(shutter);
}	/*             */



static kal_uint16 gain2reg(const kal_uint16 gain)
{

	kal_uint16 reg_gain = 0x0000;
	kal_uint16 OV8865_GAIN_BASE = 128;


	reg_gain = gain*OV8865_GAIN_BASE/BASEGAIN;
	return (kal_uint16)reg_gain;

	
}



/*                                                                        
          
          
 
             
                                                
 
            
                                        
 
         
                                  
 
                  
 
                                                                        */
static kal_uint16 set_gain(kal_uint16 gain)
{
	kal_uint16 reg_gain;


	if (gain < BASEGAIN || gain > 32 * BASEGAIN) {
		LOG_INF("Error gain setting");

		if (gain < BASEGAIN)
			gain = BASEGAIN;
		else if (gain > 32 * BASEGAIN)
			gain = 32 * BASEGAIN;		 
	}
 
	reg_gain = gain2reg(gain);
	spin_lock(&imgsensor_drv_lock);
	imgsensor.gain = reg_gain; 
	spin_unlock(&imgsensor_drv_lock);
	LOG_INF("gain = %d , reg_gain = 0x%x\n ", gain, reg_gain);

	write_cmos_sensor(0x3508,(gain>>8));
	write_cmos_sensor(0x3509,(gain&0xff));
	
	return gain;
}	/*           */


static void ihdr_write_shutter_gain(kal_uint16 le, kal_uint16 se, kal_uint16 gain)
{
  //                     

#if 0 //            
  LOG_INF("le:0x%x, se:0x%x, gain:0x%x\n",le,se,gain);
  if (imgsensor.ihdr_en) {
	  
	  spin_lock(&imgsensor_drv_lock);
	  if (le > imgsensor.min_frame_length - imgsensor_info.margin)		  
		  imgsensor.frame_length = le + imgsensor_info.margin;
	  else
		  imgsensor.frame_length = imgsensor.min_frame_length;
	  
	  if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		  imgsensor.frame_length = imgsensor_info.max_frame_length;
	  spin_unlock(&imgsensor_drv_lock);
	  
	  if (le < imgsensor_info.min_shutter) le = imgsensor_info.min_shutter;
	  if (se < imgsensor_info.min_shutter) se = imgsensor_info.min_shutter;
	  
	  //                          
	  write_cmos_sensor(xxxx, imgsensor.frame_length >> 8);
	  write_cmos_sensor(xxxx, imgsensor.frame_length & 0xFF);
  
	//             
	  write_cmos_sensor(xxxx, (le >> 8); 
	  write_cmos_sensor(xxxx, (le & 0xFF);
  
	  write_cmos_sensor(xxxx, (se >> 8); 
	  write_cmos_sensor(xxxx, (se & 0xFF);
  
	  set_gain(gain);
#endif
}


#if 1
static void set_mirror_flip(kal_uint8 image_mirror)
{

	kal_int16 mirror=0,flip=0;
	
	mirror= read_cmos_sensor(0x3820);
	flip  = read_cmos_sensor(0x3821);
    LOG_INF("image_mirror = %d, mirror:%#x, flip:%#x\n", image_mirror, mirror, flip);
    
    switch (image_mirror)
    {
        case IMAGE_H_MIRROR://             
            write_cmos_sensor(0x3820, (mirror & (0xF9)));//          
            write_cmos_sensor(0x3821, (flip & (0xF9)));	//          
            break;
        case IMAGE_NORMAL://               
            write_cmos_sensor(0x3820, (mirror & (0xF9)));//        
            write_cmos_sensor(0x3821, (flip | (0x06)));	//        
            break;
        case IMAGE_HV_MIRROR://               
            write_cmos_sensor(0x3820, (mirror |(0x06)));	//          
            write_cmos_sensor(0x3821, (flip & (0xF9)));	//          
            break;
        case IMAGE_V_MIRROR://                
            write_cmos_sensor(0x3820, (mirror |(0x06)));	//                 
            write_cmos_sensor(0x3821, (flip |(0x06)));	//                 
            break;
    }

}
#endif

/*                                                                        
          
            
 
             
                                     
 
            
                                                                       
 
         
      
 
                  
 
                                                                        */
static void night_mode(kal_bool enable)
{
/*                                  */ 
}	/*            */


static void sensor_init(void)
{
	LOG_INF("E\n");

	write_cmos_sensor(0x0103,0x01);//                 
	mdelay(10);
	write_cmos_sensor(0x0100,0x00);//                   
	write_cmos_sensor(0x0100,0x00);//
	write_cmos_sensor(0x0100,0x00);//
	write_cmos_sensor(0x0100,0x00);//
	write_cmos_sensor(0x3638,0xff);//                 

	write_cmos_sensor(0x0302,0x1f);//      
	
	write_cmos_sensor(0x0303,0x00);//      
	write_cmos_sensor(0x0304,0x03);//      
	write_cmos_sensor(0x030d,0x1f);//            
	write_cmos_sensor(0x030e,0x00);//      
	write_cmos_sensor(0x030f,0x09);//      
	write_cmos_sensor(0x0312,0x01);//      
	write_cmos_sensor(0x031e,0x0c);//      
	write_cmos_sensor(0x3015,0x01);//            
	
	//                                               
	write_cmos_sensor(0x3018,0x32);//              

	
	write_cmos_sensor(0x3020,0x93);//                       
	write_cmos_sensor(0x3022,0x01);//                               
	write_cmos_sensor(0x3031,0x0a);//         
	write_cmos_sensor(0x3106,0x01);//      
	write_cmos_sensor(0x3305,0xf1);//
	write_cmos_sensor(0x3308,0x00);//
	write_cmos_sensor(0x3309,0x28);//
	write_cmos_sensor(0x330a,0x00);//
	write_cmos_sensor(0x330b,0x20);//
	write_cmos_sensor(0x330c,0x00);//
	write_cmos_sensor(0x330d,0x00);//
	write_cmos_sensor(0x330e,0x00);//
	write_cmos_sensor(0x330f,0x40);//
	write_cmos_sensor(0x3307,0x04);//
	write_cmos_sensor(0x3604,0x04);//                 
	write_cmos_sensor(0x3602,0x30);//
	write_cmos_sensor(0x3605,0x00);//
	write_cmos_sensor(0x3607,0x20);//
	write_cmos_sensor(0x3608,0x11);//
	write_cmos_sensor(0x3609,0x68);//
	write_cmos_sensor(0x360a,0x40);//
	write_cmos_sensor(0x360c,0xdd);//
	write_cmos_sensor(0x360e,0x0c);//
	write_cmos_sensor(0x3610,0x07);//
	write_cmos_sensor(0x3612,0x86);//
	write_cmos_sensor(0x3613,0x58);//
	write_cmos_sensor(0x3614,0x28);//
	write_cmos_sensor(0x3617,0x40);//
	write_cmos_sensor(0x3618,0x5a);//
	write_cmos_sensor(0x3619,0x9b);//
	write_cmos_sensor(0x361c,0x00);//
	write_cmos_sensor(0x361d,0x60);//
	write_cmos_sensor(0x3631,0x60);//
	write_cmos_sensor(0x3633,0x10);//
	write_cmos_sensor(0x3634,0x10);//
	write_cmos_sensor(0x3635,0x10);//
	write_cmos_sensor(0x3636,0x10);//
	write_cmos_sensor(0x3641,0x55);//                
	write_cmos_sensor(0x3646,0x86);//                
	write_cmos_sensor(0x3647,0x27);//                
	write_cmos_sensor(0x364a,0x1b);//                
	
	write_cmos_sensor(0x3500,0x00);//               
	write_cmos_sensor(0x3501,0x4c);//             
	write_cmos_sensor(0x3502,0x00);//             
	
	write_cmos_sensor(0x3503,0x00);//                                   
	
	write_cmos_sensor(0x3508,0x02);//         
	write_cmos_sensor(0x3509,0x00);//         
	
	write_cmos_sensor(0x3700,0x24);//                       
	write_cmos_sensor(0x3701,0x0c);//
	write_cmos_sensor(0x3702,0x28);//
	write_cmos_sensor(0x3703,0x19);//
	write_cmos_sensor(0x3704,0x14);//
	write_cmos_sensor(0x3705,0x00);//
	write_cmos_sensor(0x3706,0x38);//    
	write_cmos_sensor(0x3707,0x04);//
	write_cmos_sensor(0x3708,0x24);//
	write_cmos_sensor(0x3709,0x40);//
	write_cmos_sensor(0x370a,0x00);//
	write_cmos_sensor(0x370b,0xb8);//    
	write_cmos_sensor(0x370c,0x04);//
	write_cmos_sensor(0x3718,0x12);//
	write_cmos_sensor(0x3719,0x31);//
	write_cmos_sensor(0x3712,0x42);//
	write_cmos_sensor(0x3714,0x12);//
	write_cmos_sensor(0x371e,0x19);//
	write_cmos_sensor(0x371f,0x40);//
	write_cmos_sensor(0x3720,0x05);//
	write_cmos_sensor(0x3721,0x05);//
	write_cmos_sensor(0x3724,0x02);//
	write_cmos_sensor(0x3725,0x02);//
	write_cmos_sensor(0x3726,0x06);//
	write_cmos_sensor(0x3728,0x05);//
	write_cmos_sensor(0x3729,0x02);//
	write_cmos_sensor(0x372a,0x03);//
	write_cmos_sensor(0x372b,0x53);//
	write_cmos_sensor(0x372c,0xa3);//
	write_cmos_sensor(0x372d,0x53);//
	write_cmos_sensor(0x372e,0x06);//
	write_cmos_sensor(0x372f,0x10);//
	write_cmos_sensor(0x3730,0x01);//
	write_cmos_sensor(0x3731,0x06);//
	write_cmos_sensor(0x3732,0x14);//
	write_cmos_sensor(0x3733,0x10);//
	write_cmos_sensor(0x3734,0x40);//
	write_cmos_sensor(0x3736,0x20);//
	write_cmos_sensor(0x373a,0x02);//
	write_cmos_sensor(0x373b,0x0c);//
	write_cmos_sensor(0x373c,0x0a);//
	write_cmos_sensor(0x373e,0x03);//
	write_cmos_sensor(0x3755,0x40);//
	write_cmos_sensor(0x3758,0x00);//
	write_cmos_sensor(0x3759,0x4c);//
	write_cmos_sensor(0x375a,0x06);//
	write_cmos_sensor(0x375b,0x13);//
	write_cmos_sensor(0x375c,0x20);//
	write_cmos_sensor(0x375d,0x02);//
	write_cmos_sensor(0x375e,0x00);//
	write_cmos_sensor(0x375f,0x14);//
	write_cmos_sensor(0x3767,0x04);//
	write_cmos_sensor(0x3768,0x04);//
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x376c,0x00);//
	write_cmos_sensor(0x376d,0x00);//
	write_cmos_sensor(0x376a,0x08);//
	write_cmos_sensor(0x3761,0x00);//
	write_cmos_sensor(0x3762,0x00);//
	write_cmos_sensor(0x3763,0x00);//
	write_cmos_sensor(0x3766,0xff);//
	write_cmos_sensor(0x376b,0x42);//
	write_cmos_sensor(0x3772,0x23);//
	write_cmos_sensor(0x3773,0x02);//
	write_cmos_sensor(0x3774,0x16);//
	write_cmos_sensor(0x3775,0x12);//
	write_cmos_sensor(0x3776,0x08);//
	write_cmos_sensor(0x37a0,0x44);//
	write_cmos_sensor(0x37a1,0x3d);//
	write_cmos_sensor(0x37a2,0x3d);//
	write_cmos_sensor(0x37a3,0x01);//
	write_cmos_sensor(0x37a4,0x00);//
	write_cmos_sensor(0x37a5,0x08);//
	write_cmos_sensor(0x37a6,0x00);//
	write_cmos_sensor(0x37a7,0x44);//
	write_cmos_sensor(0x37a8,0x58);//
	write_cmos_sensor(0x37a9,0x58);//
	write_cmos_sensor(0x3760,0x00);//
	write_cmos_sensor(0x376f,0x01);//
	write_cmos_sensor(0x37aa,0x44);//
	write_cmos_sensor(0x37ab,0x2e);//
	write_cmos_sensor(0x37ac,0x2e);//
	write_cmos_sensor(0x37ad,0x33);//
	write_cmos_sensor(0x37ae,0x0d);//
	write_cmos_sensor(0x37af,0x0d);//
	write_cmos_sensor(0x37b0,0x00);//
	write_cmos_sensor(0x37b1,0x00);//
	write_cmos_sensor(0x37b2,0x00);//
	write_cmos_sensor(0x37b3,0x42);//
	write_cmos_sensor(0x37b4,0x42);//
	write_cmos_sensor(0x37b5,0x33);//
	write_cmos_sensor(0x37b6,0x00);//
	write_cmos_sensor(0x37b7,0x00);//
	write_cmos_sensor(0x37b8,0x00);//
	write_cmos_sensor(0x37b9,0xff);//                 
	write_cmos_sensor(0x3800,0x00);//            
	write_cmos_sensor(0x3801,0x0c);//            
	write_cmos_sensor(0x3802,0x00);//            
	write_cmos_sensor(0x3803,0x0c);//            
	write_cmos_sensor(0x3804,0x0c);//          
	write_cmos_sensor(0x3805,0xd3);//          
	write_cmos_sensor(0x3806,0x09);//          
	write_cmos_sensor(0x3807,0xa3);//          
	write_cmos_sensor(0x3808,0x06);//                  
	write_cmos_sensor(0x3809,0x60);//                  
	write_cmos_sensor(0x380a,0x04);//                  
	write_cmos_sensor(0x380b,0xc8);//                  
	write_cmos_sensor(0x380c,0x07);//        
	write_cmos_sensor(0x380d,0x83);//        
	write_cmos_sensor(0x380e,0x04);//        
	write_cmos_sensor(0x380f,0xe0);//        
	write_cmos_sensor(0x3810,0x00);//              
	write_cmos_sensor(0x3811,0x04);//              
	write_cmos_sensor(0x3813,0x04);//              
	write_cmos_sensor(0x3814,0x03);//            
	write_cmos_sensor(0x3815,0x01);//             
	write_cmos_sensor(0x3820,0x00);//           
	write_cmos_sensor(0x3821,0x67);//                                  
	write_cmos_sensor(0x382a,0x03);//            
	write_cmos_sensor(0x382b,0x01);//             
	write_cmos_sensor(0x3830,0x08);//                    
	write_cmos_sensor(0x3836,0x02);//                     
	write_cmos_sensor(0x3837,0x18);//                                  
	write_cmos_sensor(0x3841,0xff);//            
	write_cmos_sensor(0x3846,0x88);//                                                

	//                                             
	write_cmos_sensor(0x3d85,0x06);//     
	write_cmos_sensor(0x3d8c,0x75);//     
	write_cmos_sensor(0x3d8d,0xef);//     


	
	write_cmos_sensor(0x3f08,0x0b);//
	
	write_cmos_sensor(0x4000,0xf1);//                                                                      
	write_cmos_sensor(0x4001,0x14);//                                                     
	write_cmos_sensor(0x4005,0x10);//             

	write_cmos_sensor(0x4006,0x04);//                                    
	write_cmos_sensor(0x4007,0x04);//   
	
	write_cmos_sensor(0x400b,0x0c);//                                                             
	write_cmos_sensor(0x400d,0x10);//                           
	write_cmos_sensor(0x401b,0x00);//
	write_cmos_sensor(0x401d,0x00);//
	write_cmos_sensor(0x4020,0x01);//                      
	write_cmos_sensor(0x4021,0x20);//                      
	write_cmos_sensor(0x4022,0x01);//                    
	write_cmos_sensor(0x4023,0x9f);//                    
	write_cmos_sensor(0x4024,0x03);//                       
	write_cmos_sensor(0x4025,0xe0);//                       
	write_cmos_sensor(0x4026,0x04);//                     
	write_cmos_sensor(0x4027,0x5f);//                     
	write_cmos_sensor(0x4028,0x00);//                      
	write_cmos_sensor(0x4029,0x02);//                       
	write_cmos_sensor(0x402a,0x04);//                       
	write_cmos_sensor(0x402b,0x04);//                        
	write_cmos_sensor(0x402c,0x02);//                         
	write_cmos_sensor(0x402d,0x02);//                          
	write_cmos_sensor(0x402e,0x08);//                          
	write_cmos_sensor(0x402f,0x02);//                           
	write_cmos_sensor(0x401f,0x00);//                     
	write_cmos_sensor(0x4034,0x3f);//                        
	write_cmos_sensor(0x4300,0xff);//             
	write_cmos_sensor(0x4301,0x00);//             
	write_cmos_sensor(0x4302,0x0f);//                        
	write_cmos_sensor(0x4500,0x40);//                   
	write_cmos_sensor(0x4503,0x10);//
	write_cmos_sensor(0x4601,0x74);//                 
	write_cmos_sensor(0x481f,0x32);//                  
	
	write_cmos_sensor(0x4837,0x16);//               
	
	
	write_cmos_sensor(0x4850,0x10);//              
	write_cmos_sensor(0x4851,0x32);//              
	write_cmos_sensor(0x4b00,0x2a);//                
	write_cmos_sensor(0x4b0d,0x00);//                
	write_cmos_sensor(0x4d00,0x04);//                     
	write_cmos_sensor(0x4d01,0x18);//                     
	write_cmos_sensor(0x4d02,0xc3);//                     
	write_cmos_sensor(0x4d03,0xff);//                     
	write_cmos_sensor(0x4d04,0xff);//                     
	write_cmos_sensor(0x4d05,0xff);//                     
	
	write_cmos_sensor(0x5000,0x96);//                                  
	write_cmos_sensor(0x5001,0x01);//         
	
	write_cmos_sensor(0x5002,0x08);//                  
	write_cmos_sensor(0x5901,0x00);//
	write_cmos_sensor(0x5e00,0x00);//                   
	write_cmos_sensor(0x5e01,0x41);//                    
	write_cmos_sensor(0x0100,0x01);//                     
	write_cmos_sensor(0x5800,0x1d);//                  
	write_cmos_sensor(0x5801,0x0e);//
	write_cmos_sensor(0x5802,0x0c);//
	write_cmos_sensor(0x5803,0x0c);//
	write_cmos_sensor(0x5804,0x0f);//
	write_cmos_sensor(0x5805,0x22);//
	write_cmos_sensor(0x5806,0x0a);//
	write_cmos_sensor(0x5807,0x06);//
	write_cmos_sensor(0x5808,0x05);//
	write_cmos_sensor(0x5809,0x05);//
	write_cmos_sensor(0x580a,0x07);//
	write_cmos_sensor(0x580b,0x0a);//
	write_cmos_sensor(0x580c,0x06);//
	write_cmos_sensor(0x580d,0x02);//
	write_cmos_sensor(0x580e,0x00);//
	write_cmos_sensor(0x580f,0x00);//
	write_cmos_sensor(0x5810,0x03);//
	write_cmos_sensor(0x5811,0x07);//
	write_cmos_sensor(0x5812,0x06);//
	write_cmos_sensor(0x5813,0x02);//
	write_cmos_sensor(0x5814,0x00);//
	write_cmos_sensor(0x5815,0x00);//
	write_cmos_sensor(0x5816,0x03);//
	write_cmos_sensor(0x5817,0x07);//
	write_cmos_sensor(0x5818,0x09);//
	write_cmos_sensor(0x5819,0x06);//
	write_cmos_sensor(0x581a,0x04);//
	write_cmos_sensor(0x581b,0x04);//
	write_cmos_sensor(0x581c,0x06);//
	write_cmos_sensor(0x581d,0x0a);//
	write_cmos_sensor(0x581e,0x19);//
	write_cmos_sensor(0x581f,0x0d);//
	write_cmos_sensor(0x5820,0x0b);//
	write_cmos_sensor(0x5821,0x0b);//
	write_cmos_sensor(0x5822,0x0e);//
	write_cmos_sensor(0x5823,0x22);//
	write_cmos_sensor(0x5824,0x23);//
	write_cmos_sensor(0x5825,0x28);//
	write_cmos_sensor(0x5826,0x29);//
	write_cmos_sensor(0x5827,0x27);//
	write_cmos_sensor(0x5828,0x13);//
	write_cmos_sensor(0x5829,0x26);//
	write_cmos_sensor(0x582a,0x33);//
	write_cmos_sensor(0x582b,0x32);//
	write_cmos_sensor(0x582c,0x33);//
	write_cmos_sensor(0x582d,0x16);//
	write_cmos_sensor(0x582e,0x14);//
	write_cmos_sensor(0x582f,0x30);//
	write_cmos_sensor(0x5830,0x31);//
	write_cmos_sensor(0x5831,0x30);//
	write_cmos_sensor(0x5832,0x15);//
	write_cmos_sensor(0x5833,0x26);//
	write_cmos_sensor(0x5834,0x23);//
	write_cmos_sensor(0x5835,0x21);//
	write_cmos_sensor(0x5836,0x23);//
	write_cmos_sensor(0x5837,0x05);//
	write_cmos_sensor(0x5838,0x36);//
	write_cmos_sensor(0x5839,0x27);//
	write_cmos_sensor(0x583a,0x28);//
	write_cmos_sensor(0x583b,0x26);//
	write_cmos_sensor(0x583c,0x24);//
	write_cmos_sensor(0x583d,0xdf);//                  


	//                                                     
	write_cmos_sensor(0x5b00,0x02);//
	write_cmos_sensor(0x5b01,0xd0);// 
	write_cmos_sensor(0x5b02,0x03);// 
	write_cmos_sensor(0x5b03,0xff);// 
	write_cmos_sensor(0x5b05,0x6c);// 
	//                                                            
}
/*              */

static void preview_setting(void)
{
	LOG_INF("Enter\n");

	write_cmos_sensor(0x0100,0x00);//                  
	write_cmos_sensor(0x030f,0x09);//     
	write_cmos_sensor(0x3018,0x32);//              
	write_cmos_sensor(0x3106,0x01);//
	
	//                                             
	//                                             
	
	write_cmos_sensor(0x3700,0x24);//                       
	write_cmos_sensor(0x3701,0x0c);//
	write_cmos_sensor(0x3702,0x28);//
	write_cmos_sensor(0x3703,0x19);//
	
	write_cmos_sensor(0x3704,0x14);//
	write_cmos_sensor(0x3706,0x38);//    
	write_cmos_sensor(0x3707,0x04);//
	write_cmos_sensor(0x3708,0x24);//
	write_cmos_sensor(0x3709,0x40);//
	write_cmos_sensor(0x370a,0x00);//
	write_cmos_sensor(0x370b,0xb8);//    
	write_cmos_sensor(0x370c,0x04);//
	
	write_cmos_sensor(0x3718,0x12);//
	write_cmos_sensor(0x3712,0x42);//
	write_cmos_sensor(0x371e,0x19);//
	write_cmos_sensor(0x371f,0x40);//
	write_cmos_sensor(0x3720,0x05);//
	write_cmos_sensor(0x3721,0x05);//
	write_cmos_sensor(0x3724,0x02);//
	write_cmos_sensor(0x3725,0x02);//
	write_cmos_sensor(0x3726,0x06);//
	write_cmos_sensor(0x3728,0x05);//
	write_cmos_sensor(0x3729,0x02);//
	write_cmos_sensor(0x372a,0x03);//
	write_cmos_sensor(0x372b,0x53);//
	write_cmos_sensor(0x372c,0xa3);//
	write_cmos_sensor(0x372d,0x53);//
	write_cmos_sensor(0x372e,0x06);//
	write_cmos_sensor(0x372f,0x10);//
	
	write_cmos_sensor(0x3730,0x01);//
	write_cmos_sensor(0x3731,0x06);//
	write_cmos_sensor(0x3732,0x14);//
	write_cmos_sensor(0x3736,0x20);//
	write_cmos_sensor(0x373a,0x02);//
	write_cmos_sensor(0x373b,0x0c);//
	write_cmos_sensor(0x373c,0x0a);//
	write_cmos_sensor(0x373e,0x03);//
	
	write_cmos_sensor(0x375a,0x06);//
	write_cmos_sensor(0x375b,0x13);//
	write_cmos_sensor(0x375c,0x40);//    
	write_cmos_sensor(0x375d,0x02);//
	write_cmos_sensor(0x375f,0x14);//
	write_cmos_sensor(0x3767,0x1c);//    
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x376c,0xc0);//    
	write_cmos_sensor(0x376d,0xc0);//    

	write_cmos_sensor(0x3772,0x23);//
	write_cmos_sensor(0x3773,0x02);//
	write_cmos_sensor(0x3774,0x16);//
	write_cmos_sensor(0x3775,0x12);//
	write_cmos_sensor(0x3776,0x08);//
	write_cmos_sensor(0x37a0,0x44);//
	write_cmos_sensor(0x37a1,0x3d);//
	write_cmos_sensor(0x37a2,0x3d);//
	write_cmos_sensor(0x37a3,0x01);//
	write_cmos_sensor(0x37a5,0x08);//
	write_cmos_sensor(0x37a7,0x44);//
	
	write_cmos_sensor(0x37a8,0x58);//
	write_cmos_sensor(0x37a9,0x58);//
	write_cmos_sensor(0x37aa,0x44);//
	write_cmos_sensor(0x37ab,0x2e);//
	write_cmos_sensor(0x37ac,0x2e);//
	write_cmos_sensor(0x37ad,0x33);//
	write_cmos_sensor(0x37ae,0x0d);//
	write_cmos_sensor(0x37af,0x0d);//
	write_cmos_sensor(0x37b3,0x42);//
	write_cmos_sensor(0x37b4,0x42);//
	write_cmos_sensor(0x37b5,0x33);//

	write_cmos_sensor(0x3808,0x06);//                           
	write_cmos_sensor(0x3809,0x60);//                                
	write_cmos_sensor(0x380a,0x04);//                           
	write_cmos_sensor(0x380b,0xc8);//                               

	write_cmos_sensor(0x380c,((imgsensor_info.pre.linelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380d,(imgsensor_info.pre.linelength & 0xFF));//       
	write_cmos_sensor(0x380e,((imgsensor_info.pre.framelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380f,(imgsensor_info.pre.framelength & 0xFF));//       
	
	write_cmos_sensor(0x3813,0x04);//             
	write_cmos_sensor(0x3814,0x03);//           
	write_cmos_sensor(0x3821,0x67);//                                 
	write_cmos_sensor(0x382a,0x03);//           
	write_cmos_sensor(0x382b,0x01);//            
	write_cmos_sensor(0x3830,0x08);//                   
	write_cmos_sensor(0x3836,0x02);//                    
	write_cmos_sensor(0x3846,0x88);//                                               
	write_cmos_sensor(0x3f08,0x0b);//
	
	write_cmos_sensor(0x4000,0xf1);//                                                                     
	
	write_cmos_sensor(0x4001,0x14);//                                                    
	write_cmos_sensor(0x4020,0x01);//                     
	write_cmos_sensor(0x4021,0x20);//                     
	write_cmos_sensor(0x4022,0x01);//                   
	write_cmos_sensor(0x4023,0x9f);//                   
	write_cmos_sensor(0x4024,0x03);//                      
	write_cmos_sensor(0x4025,0xe0);//                      
	write_cmos_sensor(0x4026,0x04);//                    
	write_cmos_sensor(0x4027,0x5f);//                    
	write_cmos_sensor(0x402a,0x04);//                      
	write_cmos_sensor(0x402b,0x04);//      
	write_cmos_sensor(0x402c,0x02);//                        
	write_cmos_sensor(0x402d,0x02);//                         
	write_cmos_sensor(0x402e,0x08);//                         
	write_cmos_sensor(0x4500,0x40);//                  
	write_cmos_sensor(0x4601,0x74);//                
	write_cmos_sensor(0x5002,0x08);//                 
	write_cmos_sensor(0x5901,0x00);//
	
	write_cmos_sensor(0x0100,0x01);//                    
}	/*                  */


static void capture_setting(kal_uint16 currefps)
{
	LOG_INF("Enter! currefps:%d\n",currefps);
    //                                
    //                    

	write_cmos_sensor(0x0100,0x00);

	write_cmos_sensor(0x030d,0x1f);//            
	write_cmos_sensor(0x030f,0x04);//      
	
	//                                               
	write_cmos_sensor(0x3018,0x32);//              

	write_cmos_sensor(0x3106,0x21);//          
	
	write_cmos_sensor(0x3700,0x24);//                       
	write_cmos_sensor(0x3701,0x0c);//
	write_cmos_sensor(0x3702,0x28);//
	write_cmos_sensor(0x3703,0x19);//
	write_cmos_sensor(0x3704,0x14);//
	write_cmos_sensor(0x3706,0x38);//    
	write_cmos_sensor(0x3707,0x04);//
	write_cmos_sensor(0x3708,0x24);//
	write_cmos_sensor(0x3709,0x40);//
	write_cmos_sensor(0x370a,0x00);//
	write_cmos_sensor(0x370b,0xb8);//    
	write_cmos_sensor(0x370c,0x04);//
	write_cmos_sensor(0x3718,0x12);//
	write_cmos_sensor(0x3712,0x42);//
	write_cmos_sensor(0x371e,0x19);//
	write_cmos_sensor(0x371f,0x40);//
	write_cmos_sensor(0x3720,0x05);//
	write_cmos_sensor(0x3721,0x05);//
	write_cmos_sensor(0x3724,0x02);//
	write_cmos_sensor(0x3725,0x02);//
	write_cmos_sensor(0x3726,0x06);//
	write_cmos_sensor(0x3728,0x05);//
	write_cmos_sensor(0x3729,0x02);//
	write_cmos_sensor(0x372a,0x03);//
	write_cmos_sensor(0x372b,0x53);//    
	write_cmos_sensor(0x372c,0xa3);//
	write_cmos_sensor(0x372d,0x53);//
	write_cmos_sensor(0x372e,0x06);//
	write_cmos_sensor(0x372f,0x10);//
	write_cmos_sensor(0x3730,0x01);//
	write_cmos_sensor(0x3731,0x06);//
	write_cmos_sensor(0x3732,0x14);//
	write_cmos_sensor(0x3736,0x20);//
	write_cmos_sensor(0x373a,0x02);//
	write_cmos_sensor(0x373b,0x0c);//
	write_cmos_sensor(0x373c,0x0a);//
	write_cmos_sensor(0x373e,0x03);//
	write_cmos_sensor(0x375a,0x06);//
	write_cmos_sensor(0x375b,0x13);//    
	write_cmos_sensor(0x375c,0x40);//    
	write_cmos_sensor(0x375d,0x02);//
	write_cmos_sensor(0x375f,0x14);//
	write_cmos_sensor(0x3767,0x1e);//    
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x376c,0xc0);//    
	write_cmos_sensor(0x376d,0xc0);//    
	write_cmos_sensor(0x3772,0x23);//
	write_cmos_sensor(0x3773,0x02);//
	write_cmos_sensor(0x3774,0x16);//
	write_cmos_sensor(0x3775,0x12);//
	write_cmos_sensor(0x3776,0x08);//
	write_cmos_sensor(0x37a0,0x44);//
	write_cmos_sensor(0x37a1,0x3d);//
	write_cmos_sensor(0x37a2,0x3d);//
	write_cmos_sensor(0x37a3,0x02);//
	write_cmos_sensor(0x37a5,0x09);//
	write_cmos_sensor(0x37a7,0x44);//
	write_cmos_sensor(0x37a8,0x58);//
	write_cmos_sensor(0x37a9,0x58);//
	write_cmos_sensor(0x37aa,0x44);//
	write_cmos_sensor(0x37ab,0x2e);//
	write_cmos_sensor(0x37ac,0x2e);//
	write_cmos_sensor(0x37ad,0x33);//
	write_cmos_sensor(0x37ae,0x0d);//
	write_cmos_sensor(0x37af,0x0d);//
	write_cmos_sensor(0x37b3,0x42);//
	write_cmos_sensor(0x37b4,0x42);//
	write_cmos_sensor(0x37b5,0x33);//

	write_cmos_sensor(0x3808,0x0c);//                  
	write_cmos_sensor(0x3809,0xc0);//                                
	write_cmos_sensor(0x380a,0x09);//                  
	write_cmos_sensor(0x380b,0x90);//                                
	
	write_cmos_sensor(0x380c,((imgsensor_info.cap.linelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380d,(imgsensor_info.cap.linelength & 0xFF));//       
	write_cmos_sensor(0x380e,((imgsensor_info.cap.framelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380f,(imgsensor_info.cap.framelength & 0xFF));//       

	write_cmos_sensor(0x3813,0x02);//              
	write_cmos_sensor(0x3814,0x01);//            
	write_cmos_sensor(0x3821,0x46);//                                  
	write_cmos_sensor(0x382a,0x01);//            
	write_cmos_sensor(0x382b,0x01);//             
	write_cmos_sensor(0x3830,0x04);//                    
	write_cmos_sensor(0x3836,0x01);//                     
	write_cmos_sensor(0x3846,0x48);//                                                
	write_cmos_sensor(0x3f08,0x0b);//
	
	write_cmos_sensor(0x4000,0xf1);//                                                                     
	
	write_cmos_sensor(0x4001,0x04);//                                                     
	write_cmos_sensor(0x4020,0x02);//                      
	write_cmos_sensor(0x4021,0x40);//                      
	write_cmos_sensor(0x4022,0x03);//                    
	write_cmos_sensor(0x4023,0x3f);//                    
	write_cmos_sensor(0x4024,0x07);//                       
	write_cmos_sensor(0x4025,0xc0);//                       
	write_cmos_sensor(0x4026,0x08);//                     
	write_cmos_sensor(0x4027,0xbf);//                     
	write_cmos_sensor(0x402a,0x04);//                       
	write_cmos_sensor(0x402b,0x04);//    
	write_cmos_sensor(0x402c,0x02);//                           
	write_cmos_sensor(0x402d,0x02);//                          
	write_cmos_sensor(0x402e,0x08);//                          
	write_cmos_sensor(0x4500,0x68);//                   
	write_cmos_sensor(0x4601,0x10);//                 
	write_cmos_sensor(0x5002,0x08);//                  
	write_cmos_sensor(0x5901,0x00);//
	
	write_cmos_sensor(0x0100,0x01);//                    
}	
	
		
static void normal_video_setting(kal_uint16 currefps)
{
	LOG_INF("Enter\n");

	write_cmos_sensor(0x0100,0x00);//                  
	write_cmos_sensor(0x030f,0x09);//     
	write_cmos_sensor(0x3018,0x32);//              
	write_cmos_sensor(0x3106,0x01);//
	
	//                                             
	//                                             
	
	write_cmos_sensor(0x3700,0x24);//                      
	write_cmos_sensor(0x3701,0x0c);//
	write_cmos_sensor(0x3702,0x28);//
	write_cmos_sensor(0x3703,0x19);//
	
	write_cmos_sensor(0x3704,0x14);//
	write_cmos_sensor(0x3706,0x38);//    
	write_cmos_sensor(0x3707,0x04);//
	write_cmos_sensor(0x3708,0x24);//
	write_cmos_sensor(0x3709,0x40);//
	write_cmos_sensor(0x370a,0x00);//
	write_cmos_sensor(0x370b,0xb8);//    
	write_cmos_sensor(0x370c,0x04);//
	
	write_cmos_sensor(0x3718,0x12);//
	write_cmos_sensor(0x3712,0x42);//
	write_cmos_sensor(0x371e,0x19);//
	write_cmos_sensor(0x371f,0x40);//
	write_cmos_sensor(0x3720,0x05);//
	write_cmos_sensor(0x3721,0x05);//
	write_cmos_sensor(0x3724,0x02);//
	write_cmos_sensor(0x3725,0x02);//
	write_cmos_sensor(0x3726,0x06);//
	write_cmos_sensor(0x3728,0x05);//
	write_cmos_sensor(0x3729,0x02);//
	write_cmos_sensor(0x372a,0x03);//
	write_cmos_sensor(0x372b,0x53);//
	write_cmos_sensor(0x372c,0xa3);//
	write_cmos_sensor(0x372d,0x53);//
	write_cmos_sensor(0x372e,0x06);//
	write_cmos_sensor(0x372f,0x10);//
	
	write_cmos_sensor(0x3730,0x01);//
	write_cmos_sensor(0x3731,0x06);//
	write_cmos_sensor(0x3732,0x14);//
	write_cmos_sensor(0x3736,0x20);//
	write_cmos_sensor(0x373a,0x02);//
	write_cmos_sensor(0x373b,0x0c);//
	write_cmos_sensor(0x373c,0x0a);//
	write_cmos_sensor(0x373e,0x03);//
	
	write_cmos_sensor(0x375a,0x06);//
	write_cmos_sensor(0x375b,0x13);//
	write_cmos_sensor(0x375c,0x40);//    
	write_cmos_sensor(0x375d,0x02);//
	write_cmos_sensor(0x375f,0x14);//
	write_cmos_sensor(0x3767,0x1c);//    
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x376c,0xc0);//    
	write_cmos_sensor(0x376d,0xc0);//    

	write_cmos_sensor(0x3772,0x23);//
	write_cmos_sensor(0x3773,0x02);//
	write_cmos_sensor(0x3774,0x16);//
	write_cmos_sensor(0x3775,0x12);//
	write_cmos_sensor(0x3776,0x08);//
	write_cmos_sensor(0x37a0,0x44);//
	write_cmos_sensor(0x37a1,0x3d);//
	write_cmos_sensor(0x37a2,0x3d);//
	write_cmos_sensor(0x37a3,0x01);//
	write_cmos_sensor(0x37a5,0x08);//
	write_cmos_sensor(0x37a7,0x44);//
	
	write_cmos_sensor(0x37a8,0x58);//
	write_cmos_sensor(0x37a9,0x58);//
	write_cmos_sensor(0x37aa,0x44);//
	write_cmos_sensor(0x37ab,0x2e);//
	write_cmos_sensor(0x37ac,0x2e);//
	write_cmos_sensor(0x37ad,0x33);//
	write_cmos_sensor(0x37ae,0x0d);//
	write_cmos_sensor(0x37af,0x0d);//
	write_cmos_sensor(0x37b3,0x42);//
	write_cmos_sensor(0x37b4,0x42);//
	write_cmos_sensor(0x37b5,0x33);//

	write_cmos_sensor(0x3808,0x06);//                          
	write_cmos_sensor(0x3809,0x60);//                               
	write_cmos_sensor(0x380a,0x04);//                          
	write_cmos_sensor(0x380b,0xc8);//                               

	write_cmos_sensor(0x380c,((imgsensor_info.normal_video.linelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380d,(imgsensor_info.normal_video.linelength & 0xFF));//       
	write_cmos_sensor(0x380e,((imgsensor_info.normal_video.framelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380f,(imgsensor_info.normal_video.framelength & 0xFF));//       
	
	write_cmos_sensor(0x3813,0x04);//             
	write_cmos_sensor(0x3814,0x03);//           
	write_cmos_sensor(0x3821,0x67);//                                 
	write_cmos_sensor(0x382a,0x03);//           
	write_cmos_sensor(0x382b,0x01);//            
	write_cmos_sensor(0x3830,0x08);//                   
	write_cmos_sensor(0x3836,0x02);//                    
	write_cmos_sensor(0x3846,0x88);//                                               
	write_cmos_sensor(0x3f08,0x0b);//
	
	write_cmos_sensor(0x4000,0xf1);//                                                                     
	
	write_cmos_sensor(0x4001,0x14);//                                                    
	write_cmos_sensor(0x4020,0x01);//                     
	write_cmos_sensor(0x4021,0x20);//                     
	write_cmos_sensor(0x4022,0x01);//                   
	write_cmos_sensor(0x4023,0x9f);//                   
	write_cmos_sensor(0x4024,0x03);//                      
	write_cmos_sensor(0x4025,0xe0);//                      
	write_cmos_sensor(0x4026,0x04);//                    
	write_cmos_sensor(0x4027,0x5f);//                    
	write_cmos_sensor(0x402a,0x04);//                      
	write_cmos_sensor(0x402b,0x04);//      
	write_cmos_sensor(0x402c,0x02);//                        
	write_cmos_sensor(0x402d,0x02);//                         
	write_cmos_sensor(0x402e,0x08);//                         
	write_cmos_sensor(0x4500,0x40);//                  
	write_cmos_sensor(0x4601,0x74);//                
	write_cmos_sensor(0x5002,0x08);//                 
	write_cmos_sensor(0x5901,0x00);//
	
	write_cmos_sensor(0x0100,0x01);//                    
}


static void hs_video_setting()
{
	LOG_INF("Enter\n");

	write_cmos_sensor(0x0100,0x00);//                  
	write_cmos_sensor(0x030f,0x09);//     
	write_cmos_sensor(0x3018,0x32);//              
	write_cmos_sensor(0x3106,0x01);//
	
	//                                             
	//                                             
	
	write_cmos_sensor(0x3700,0x24);//                      
	write_cmos_sensor(0x3701,0x0c);//
	write_cmos_sensor(0x3702,0x28);//
	write_cmos_sensor(0x3703,0x19);//
	
	write_cmos_sensor(0x3704,0x14);//
	write_cmos_sensor(0x3706,0x38);//    
	write_cmos_sensor(0x3707,0x04);//
	write_cmos_sensor(0x3708,0x24);//
	write_cmos_sensor(0x3709,0x40);//
	write_cmos_sensor(0x370a,0x00);//
	write_cmos_sensor(0x370b,0xb8);//    
	write_cmos_sensor(0x370c,0x04);//
	
	write_cmos_sensor(0x3718,0x12);//
	write_cmos_sensor(0x3712,0x42);//
	write_cmos_sensor(0x371e,0x19);//
	write_cmos_sensor(0x371f,0x40);//
	write_cmos_sensor(0x3720,0x05);//
	write_cmos_sensor(0x3721,0x05);//
	write_cmos_sensor(0x3724,0x02);//
	write_cmos_sensor(0x3725,0x02);//
	write_cmos_sensor(0x3726,0x06);//
	write_cmos_sensor(0x3728,0x05);//
	write_cmos_sensor(0x3729,0x02);//
	write_cmos_sensor(0x372a,0x03);//
	write_cmos_sensor(0x372b,0x53);//
	write_cmos_sensor(0x372c,0xa3);//
	write_cmos_sensor(0x372d,0x53);//
	write_cmos_sensor(0x372e,0x06);//
	write_cmos_sensor(0x372f,0x10);//
	
	write_cmos_sensor(0x3730,0x01);//
	write_cmos_sensor(0x3731,0x06);//
	write_cmos_sensor(0x3732,0x14);//
	write_cmos_sensor(0x3736,0x20);//
	write_cmos_sensor(0x373a,0x02);//
	write_cmos_sensor(0x373b,0x0c);//
	write_cmos_sensor(0x373c,0x0a);//
	write_cmos_sensor(0x373e,0x03);//
	
	write_cmos_sensor(0x375a,0x06);//
	write_cmos_sensor(0x375b,0x13);//
	write_cmos_sensor(0x375c,0x40);//    
	write_cmos_sensor(0x375d,0x02);//
	write_cmos_sensor(0x375f,0x14);//
	write_cmos_sensor(0x3767,0x1c);//    
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x376c,0xc0);//    
	write_cmos_sensor(0x376d,0xc0);//    

	write_cmos_sensor(0x3772,0x23);//
	write_cmos_sensor(0x3773,0x02);//
	write_cmos_sensor(0x3774,0x16);//
	write_cmos_sensor(0x3775,0x12);//
	write_cmos_sensor(0x3776,0x08);//
	write_cmos_sensor(0x37a0,0x44);//
	write_cmos_sensor(0x37a1,0x3d);//
	write_cmos_sensor(0x37a2,0x3d);//
	write_cmos_sensor(0x37a3,0x01);//
	write_cmos_sensor(0x37a5,0x08);//
	write_cmos_sensor(0x37a7,0x44);//
	
	write_cmos_sensor(0x37a8,0x58);//
	write_cmos_sensor(0x37a9,0x58);//
	write_cmos_sensor(0x37aa,0x44);//
	write_cmos_sensor(0x37ab,0x2e);//
	write_cmos_sensor(0x37ac,0x2e);//
	write_cmos_sensor(0x37ad,0x33);//
	write_cmos_sensor(0x37ae,0x0d);//
	write_cmos_sensor(0x37af,0x0d);//
	write_cmos_sensor(0x37b3,0x42);//
	write_cmos_sensor(0x37b4,0x42);//
	write_cmos_sensor(0x37b5,0x33);//

	write_cmos_sensor(0x3808,0x06);//                          
	write_cmos_sensor(0x3809,0x60);//                               
	write_cmos_sensor(0x380a,0x04);//                          
	write_cmos_sensor(0x380b,0xc8);//                               

	write_cmos_sensor(0x380c,((imgsensor_info.hs_video.linelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380d,(imgsensor_info.hs_video.linelength & 0xFF));//       
	write_cmos_sensor(0x380e,((imgsensor_info.hs_video.framelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380f,(imgsensor_info.hs_video.framelength & 0xFF));//       
	
	write_cmos_sensor(0x3813,0x04);//             
	write_cmos_sensor(0x3814,0x03);//           
	write_cmos_sensor(0x3821,0x67);//                                 
	write_cmos_sensor(0x382a,0x03);//           
	write_cmos_sensor(0x382b,0x01);//            
	write_cmos_sensor(0x3830,0x08);//                   
	write_cmos_sensor(0x3836,0x02);//                    
	write_cmos_sensor(0x3846,0x88);//                                               
	write_cmos_sensor(0x3f08,0x0b);//
	
	write_cmos_sensor(0x4000,0xf1);//                                                                     
	
	write_cmos_sensor(0x4001,0x14);//                                                    
	write_cmos_sensor(0x4020,0x01);//                     
	write_cmos_sensor(0x4021,0x20);//                     
	write_cmos_sensor(0x4022,0x01);//                   
	write_cmos_sensor(0x4023,0x9f);//                   
	write_cmos_sensor(0x4024,0x03);//                      
	write_cmos_sensor(0x4025,0xe0);//                      
	write_cmos_sensor(0x4026,0x04);//                    
	write_cmos_sensor(0x4027,0x5f);//                    
	write_cmos_sensor(0x402a,0x04);//                      
	write_cmos_sensor(0x402b,0x04);//      
	write_cmos_sensor(0x402c,0x02);//                        
	write_cmos_sensor(0x402d,0x02);//                         
	write_cmos_sensor(0x402e,0x08);//                         
	write_cmos_sensor(0x4500,0x40);//                  
	write_cmos_sensor(0x4601,0x74);//                
	write_cmos_sensor(0x5002,0x08);//                 
	write_cmos_sensor(0x5901,0x00);//
	
	write_cmos_sensor(0x0100,0x01);//                    
}



static void slim_video_setting()
{
	LOG_INF("Enter\n");

	write_cmos_sensor(0x0100,0x00);//                  
	write_cmos_sensor(0x030f,0x09);//     
	write_cmos_sensor(0x3018,0x32);//              
	write_cmos_sensor(0x3106,0x01);//
	
	//                                             
	//                                             
	
	write_cmos_sensor(0x3700,0x24);//                      
	write_cmos_sensor(0x3701,0x0c);//
	write_cmos_sensor(0x3702,0x28);//
	write_cmos_sensor(0x3703,0x19);//
	
	write_cmos_sensor(0x3704,0x14);//
	write_cmos_sensor(0x3706,0x38);//    
	write_cmos_sensor(0x3707,0x04);//
	write_cmos_sensor(0x3708,0x24);//
	write_cmos_sensor(0x3709,0x40);//
	write_cmos_sensor(0x370a,0x00);//
	write_cmos_sensor(0x370b,0xb8);//    
	write_cmos_sensor(0x370c,0x04);//
	
	write_cmos_sensor(0x3718,0x12);//
	write_cmos_sensor(0x3712,0x42);//
	write_cmos_sensor(0x371e,0x19);//
	write_cmos_sensor(0x371f,0x40);//
	write_cmos_sensor(0x3720,0x05);//
	write_cmos_sensor(0x3721,0x05);//
	write_cmos_sensor(0x3724,0x02);//
	write_cmos_sensor(0x3725,0x02);//
	write_cmos_sensor(0x3726,0x06);//
	write_cmos_sensor(0x3728,0x05);//
	write_cmos_sensor(0x3729,0x02);//
	write_cmos_sensor(0x372a,0x03);//
	write_cmos_sensor(0x372b,0x53);//
	write_cmos_sensor(0x372c,0xa3);//
	write_cmos_sensor(0x372d,0x53);//
	write_cmos_sensor(0x372e,0x06);//
	write_cmos_sensor(0x372f,0x10);//
	
	write_cmos_sensor(0x3730,0x01);//
	write_cmos_sensor(0x3731,0x06);//
	write_cmos_sensor(0x3732,0x14);//
	write_cmos_sensor(0x3736,0x20);//
	write_cmos_sensor(0x373a,0x02);//
	write_cmos_sensor(0x373b,0x0c);//
	write_cmos_sensor(0x373c,0x0a);//
	write_cmos_sensor(0x373e,0x03);//
	
	write_cmos_sensor(0x375a,0x06);//
	write_cmos_sensor(0x375b,0x13);//
	write_cmos_sensor(0x375c,0x40);//    
	write_cmos_sensor(0x375d,0x02);//
	write_cmos_sensor(0x375f,0x14);//
	write_cmos_sensor(0x3767,0x1c);//    
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x3769,0x20);//
	write_cmos_sensor(0x376c,0xc0);//    
	write_cmos_sensor(0x376d,0xc0);//    

	write_cmos_sensor(0x3772,0x23);//
	write_cmos_sensor(0x3773,0x02);//
	write_cmos_sensor(0x3774,0x16);//
	write_cmos_sensor(0x3775,0x12);//
	write_cmos_sensor(0x3776,0x08);//
	write_cmos_sensor(0x37a0,0x44);//
	write_cmos_sensor(0x37a1,0x3d);//
	write_cmos_sensor(0x37a2,0x3d);//
	write_cmos_sensor(0x37a3,0x01);//
	write_cmos_sensor(0x37a5,0x08);//
	write_cmos_sensor(0x37a7,0x44);//
	
	write_cmos_sensor(0x37a8,0x58);//
	write_cmos_sensor(0x37a9,0x58);//
	write_cmos_sensor(0x37aa,0x44);//
	write_cmos_sensor(0x37ab,0x2e);//
	write_cmos_sensor(0x37ac,0x2e);//
	write_cmos_sensor(0x37ad,0x33);//
	write_cmos_sensor(0x37ae,0x0d);//
	write_cmos_sensor(0x37af,0x0d);//
	write_cmos_sensor(0x37b3,0x42);//
	write_cmos_sensor(0x37b4,0x42);//
	write_cmos_sensor(0x37b5,0x33);//

	write_cmos_sensor(0x3808,0x06);//                          
	write_cmos_sensor(0x3809,0x60);//                               
	write_cmos_sensor(0x380a,0x04);//                          
	write_cmos_sensor(0x380b,0xc8);//                               

	write_cmos_sensor(0x380c,((imgsensor_info.slim_video.linelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380d,(imgsensor_info.slim_video.linelength & 0xFF));//       
	write_cmos_sensor(0x380e,((imgsensor_info.slim_video.framelength >> 8) & 0xFF));//       
	write_cmos_sensor(0x380f,(imgsensor_info.slim_video.framelength & 0xFF));//       
	
	write_cmos_sensor(0x3813,0x04);//             
	write_cmos_sensor(0x3814,0x03);//           
	write_cmos_sensor(0x3821,0x67);//                                 
	write_cmos_sensor(0x382a,0x03);//           
	write_cmos_sensor(0x382b,0x01);//            
	write_cmos_sensor(0x3830,0x08);//                   
	write_cmos_sensor(0x3836,0x02);//                    
	write_cmos_sensor(0x3846,0x88);//                                               
	write_cmos_sensor(0x3f08,0x0b);//
	
	write_cmos_sensor(0x4000,0xf1);//                                                                     
	
	write_cmos_sensor(0x4001,0x14);//                                                    
	write_cmos_sensor(0x4020,0x01);//                     
	write_cmos_sensor(0x4021,0x20);//                     
	write_cmos_sensor(0x4022,0x01);//                   
	write_cmos_sensor(0x4023,0x9f);//                   
	write_cmos_sensor(0x4024,0x03);//                      
	write_cmos_sensor(0x4025,0xe0);//                      
	write_cmos_sensor(0x4026,0x04);//                    
	write_cmos_sensor(0x4027,0x5f);//                    
	write_cmos_sensor(0x402a,0x04);//                      
	write_cmos_sensor(0x402b,0x04);//      
	write_cmos_sensor(0x402c,0x02);//                        
	write_cmos_sensor(0x402d,0x02);//                         
	write_cmos_sensor(0x402e,0x08);//                         
	write_cmos_sensor(0x4500,0x40);//                  
	write_cmos_sensor(0x4601,0x74);//                
	write_cmos_sensor(0x5002,0x08);//                 
	write_cmos_sensor(0x5901,0x00);//
	
	write_cmos_sensor(0x0100,0x01);//                    
}




/*                                                                        
          
                  
 
             
                                  
 
            
                                   
 
         
      
 
                  
 
                                                                        */
static kal_uint32 get_imgsensor_id(UINT32 *sensor_id) 
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;

	write_cmos_sensor(0x0103,0x01);//             
    mdelay(2);
	
	//                                   
	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
			*sensor_id = ((read_cmos_sensor(0x300B) << 8) | read_cmos_sensor(0x300C));
			if (*sensor_id == imgsensor_info.sensor_id) {				
				LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,*sensor_id);	  
				return ERROR_NONE;
			}	
			LOG_INF("Read sensor id fail, write id: 0x%x, sensor id:0x%x\n", imgsensor.i2c_write_id, *sensor_id);
			retry--;
		} while(retry > 0);
		
		i++;
		retry = 2;
	}
	if (*sensor_id != imgsensor_info.sensor_id) {
		//                                                                
		*sensor_id = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	return ERROR_NONE;
}


/*                                                                        
          
      
 
             
                                                       
 
            
      
 
         
      
 
                  
 
                                                                        */
static kal_uint32 open(void)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
	kal_uint16 sensor_id = 0; 
	LOG_INF("lanes:MIPI 2LANE\n");
	LOG_INF("preview/video:30fps,Capture:15 fps\n");
	LOG_INF("...\n");

	
	write_cmos_sensor(0x0103,0x01);//             
	mdelay(2);
	
	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
			sensor_id = ((read_cmos_sensor(0x300B) << 8) | read_cmos_sensor(0x300C));
			if (sensor_id == imgsensor_info.sensor_id) {				
				LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,sensor_id);	  
				break;
			}	
			LOG_INF("Read sensor id fail, id: 0x%x\n", imgsensor.i2c_write_id,sensor_id);
			retry--;
		} while(retry > 0);
		
		i++;
		if (sensor_id == imgsensor_info.sensor_id)
			break;
		retry = 2;
	}		 
	if (imgsensor_info.sensor_id != sensor_id)
		return ERROR_SENSOR_CONNECT_FAIL;
	
	/*                            */
	sensor_init();

	spin_lock(&imgsensor_drv_lock);
	imgsensor.autoflicker_en= KAL_FALSE;
	imgsensor.sensor_mode = IMGSENSOR_MODE_INIT;
	imgsensor.shutter = 0x3D0;
	imgsensor.gain = 0x100;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.dummy_pixel = 0;
	imgsensor.dummy_line = 0;
	imgsensor.ihdr_en = 0;
	imgsensor.current_fps = imgsensor_info.pre.max_framerate;
	imgsensor.test_pattern = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);

	return ERROR_NONE;
}	/*       */



/*                                                                        
          
       
 
             
  
 
            
      
 
         
      
 
                  
 
                                                                        */
static kal_uint32 close(void)
{
	LOG_INF("E\n");

	/*                                  */ 
	
	return ERROR_NONE;
}	/*        */


/*                                                                        
          
         
 
             
                                         
 
            
                                                                         
                                                                               
 
         
      
 
                  
 
                                                                        */
static kal_uint32 preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_PREVIEW;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	//                                 
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.frame_length = imgsensor_info.pre.framelength; 
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	preview_setting();
    set_mirror_flip(imgsensor.mirror);
	return ERROR_NONE;
}	/*           */

/*                                                                        
          
         
 
             
                                                               
 
            
 
         
      
 
                  
 
                                                                        */
static kal_uint32 capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
						  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CAPTURE;
	if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {
		imgsensor.pclk = imgsensor_info.cap1.pclk;
		imgsensor.line_length = imgsensor_info.cap1.linelength;
		imgsensor.frame_length = imgsensor_info.cap1.framelength;  
		imgsensor.min_frame_length = imgsensor_info.cap1.framelength;
		imgsensor.autoflicker_en = KAL_FALSE;
	} else {
		if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
			LOG_INF("Warning: current_fps %d fps is not support, so use cap1's setting: %d fps!\n",imgsensor_info.cap1.max_framerate/10);
		
		imgsensor.pclk = imgsensor_info.cap.pclk;
		imgsensor.line_length = imgsensor_info.cap.linelength;
		imgsensor.frame_length = imgsensor_info.cap.framelength;  
		imgsensor.min_frame_length = imgsensor_info.cap.framelength;
		imgsensor.autoflicker_en = KAL_FALSE;
	}
	spin_unlock(&imgsensor_drv_lock);

	capture_setting(imgsensor.current_fps); 
	
	if(imgsensor.test_pattern == KAL_TRUE)
	{
		write_cmos_sensor(0x5E00,0x80);
		
	}
    set_mirror_flip(imgsensor.mirror);
	return ERROR_NONE;
}	/*           */
static kal_uint32 normal_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");
	
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_VIDEO;
	imgsensor.pclk = imgsensor_info.normal_video.pclk;
	imgsensor.line_length = imgsensor_info.normal_video.linelength;
	imgsensor.frame_length = imgsensor_info.normal_video.framelength;  
	imgsensor.min_frame_length = imgsensor_info.normal_video.framelength;
	//                                                                  
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	normal_video_setting(imgsensor.current_fps);
	set_mirror_flip(imgsensor.mirror);
	return ERROR_NONE;
}	/*                */

static kal_uint32 hs_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");
	
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_HIGH_SPEED_VIDEO;
	imgsensor.pclk = imgsensor_info.hs_video.pclk;
	//                                
	imgsensor.line_length = imgsensor_info.hs_video.linelength;
	imgsensor.frame_length = imgsensor_info.hs_video.framelength; 
	imgsensor.min_frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	//                                                               
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	hs_video_setting();
	set_mirror_flip(imgsensor.mirror);
	return ERROR_NONE;
}	/*            */

static kal_uint32 slim_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");
	
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_SLIM_VIDEO;
	imgsensor.pclk = imgsensor_info.slim_video.pclk;
	imgsensor.line_length = imgsensor_info.slim_video.linelength;
	imgsensor.frame_length = imgsensor_info.slim_video.framelength; 
	imgsensor.min_frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	//                                                                 
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	slim_video_setting();
	set_mirror_flip(imgsensor.mirror);
	return ERROR_NONE;
}	/*             */



static kal_uint32 get_resolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *sensor_resolution)
{
	LOG_INF("E\n");
	sensor_resolution->SensorFullWidth = imgsensor_info.cap.grabwindow_width;
	sensor_resolution->SensorFullHeight = imgsensor_info.cap.grabwindow_height;
	
	sensor_resolution->SensorPreviewWidth = imgsensor_info.pre.grabwindow_width;
	sensor_resolution->SensorPreviewHeight = imgsensor_info.pre.grabwindow_height;

	sensor_resolution->SensorVideoWidth = imgsensor_info.normal_video.grabwindow_width;
	sensor_resolution->SensorVideoHeight = imgsensor_info.normal_video.grabwindow_height;		

	
	sensor_resolution->SensorHighSpeedVideoWidth	 = imgsensor_info.hs_video.grabwindow_width;
	sensor_resolution->SensorHighSpeedVideoHeight	 = imgsensor_info.hs_video.grabwindow_height;
	
	sensor_resolution->SensorSlimVideoWidth	 = imgsensor_info.slim_video.grabwindow_width;
	sensor_resolution->SensorSlimVideoHeight	 = imgsensor_info.slim_video.grabwindow_height;
	return ERROR_NONE;
}	/*                */

static kal_uint32 get_info(MSDK_SCENARIO_ID_ENUM scenario_id,
					  MSDK_SENSOR_INFO_STRUCT *sensor_info,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("scenario_id = %d\n", scenario_id);


	sensor_info->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW; /*         */
	sensor_info->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW; //                       
	sensor_info->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorInterruptDelayLines = 4; /*         */
	sensor_info->SensorResetActiveHigh = FALSE; /*         */
	sensor_info->SensorResetDelayCount = 5; /*         */

	sensor_info->SensroInterfaceType = imgsensor_info.sensor_interface_type;
	//                                                              
	//                                                                     
	sensor_info->SensorOutputDataFormat = imgsensor_info.sensor_output_dataformat;

	sensor_info->CaptureDelayFrame = imgsensor_info.cap_delay_frame; 
	sensor_info->PreviewDelayFrame = imgsensor_info.pre_delay_frame; 
	sensor_info->VideoDelayFrame = imgsensor_info.video_delay_frame;
	sensor_info->HighSpeedVideoDelayFrame = imgsensor_info.hs_video_delay_frame;
	sensor_info->SlimVideoDelayFrame = imgsensor_info.slim_video_delay_frame;

	sensor_info->SensorMasterClockSwitch = 0; /*         */
	sensor_info->SensorDrivingCurrent = imgsensor_info.isp_driving_current;
	
	sensor_info->AEShutDelayFrame = imgsensor_info.ae_shut_delay_frame; 		 /*                                                   */
	sensor_info->AESensorGainDelayFrame = imgsensor_info.ae_sensor_gain_delay_frame;	/*                                  */
	sensor_info->AEISPGainDelayFrame = imgsensor_info.ae_ispGain_delay_frame;	
	sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
	sensor_info->IHDR_LE_FirstLine = imgsensor_info.ihdr_le_firstline;
	sensor_info->SensorModeNum = imgsensor_info.sensor_mode_num;
	
	sensor_info->SensorMIPILaneNumber = imgsensor_info.mipi_lane_num; 
	sensor_info->SensorClockFreq = imgsensor_info.mclk;
	sensor_info->SensorClockDividCount = 3; /*         */
	sensor_info->SensorClockRisingCount = 0;
	sensor_info->SensorClockFallingCount = 2; /*         */
	sensor_info->SensorPixelClockCount = 3; /*         */
	sensor_info->SensorDataLatchCount = 2; /*         */
	
	sensor_info->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
	sensor_info->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->SensorWidthSampling = 0;  //                
	sensor_info->SensorHightSampling = 0;	//                 
	sensor_info->SensorPacketECCOrder = 1;

	switch (scenario_id) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			sensor_info->SensorGrabStartX = imgsensor_info.pre.startx; 
			sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;		
			
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
			
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			sensor_info->SensorGrabStartX = imgsensor_info.cap.startx; 
			sensor_info->SensorGrabStartY = imgsensor_info.cap.starty;
				  
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.cap.mipi_data_lp2hs_settle_dc; 

			break;	 
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			
			sensor_info->SensorGrabStartX = imgsensor_info.normal_video.startx; 
			sensor_info->SensorGrabStartY = imgsensor_info.normal_video.starty;
	   
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.normal_video.mipi_data_lp2hs_settle_dc; 

			break;	  
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:			
			sensor_info->SensorGrabStartX = imgsensor_info.hs_video.startx; 
			sensor_info->SensorGrabStartY = imgsensor_info.hs_video.starty;
				  
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.hs_video.mipi_data_lp2hs_settle_dc; 

			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			sensor_info->SensorGrabStartX = imgsensor_info.slim_video.startx; 
			sensor_info->SensorGrabStartY = imgsensor_info.slim_video.starty;
				  
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.slim_video.mipi_data_lp2hs_settle_dc; 

			break;
		default:			
			sensor_info->SensorGrabStartX = imgsensor_info.pre.startx; 
			sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;		
			
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
			break;
	}
	
	return ERROR_NONE;
}	/*           */


static kal_uint32 control(MSDK_SCENARIO_ID_ENUM scenario_id, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("scenario_id = %d\n", scenario_id);
	spin_lock(&imgsensor_drv_lock);
	imgsensor.current_scenario_id = scenario_id;
	spin_unlock(&imgsensor_drv_lock);
	switch (scenario_id) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			preview(image_window, sensor_config_data);
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			capture(image_window, sensor_config_data);
			break;	
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW://  
			normal_video(image_window, sensor_config_data);
			break;	  
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO://  
			hs_video(image_window, sensor_config_data);
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO: // 
			slim_video(image_window, sensor_config_data);
			break;	  
		default:
			LOG_INF("Error ScenarioId setting");
			preview(image_window, sensor_config_data);
			return ERROR_INVALID_SCENARIO_ID;
	}
	return ERROR_NONE;
}	/*           */



static kal_uint32 set_video_mode(UINT16 framerate)
{
	LOG_INF("framerate = %d\n ", framerate);
	//                                           
	if (framerate == 0)
		//                   
		return ERROR_NONE;
	
	spin_lock(&imgsensor_drv_lock);
	if ((framerate == 300) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 296;
	else if ((framerate == 150) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 146;
	else
		imgsensor.current_fps = framerate;
	spin_unlock(&imgsensor_drv_lock);
	set_max_framerate(imgsensor.current_fps,1);

	return ERROR_NONE;
}

static kal_uint32 set_auto_flicker_mode(kal_bool enable, UINT16 framerate)
{
	LOG_INF("enable = %d, framerate = %d \n", enable, framerate);
	spin_lock(&imgsensor_drv_lock);
	if (enable) 	  
		imgsensor.autoflicker_en = KAL_TRUE;
	else //                 
		imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}


static kal_uint32 set_max_framerate_by_scenario(MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 framerate) 
{
	kal_uint32 frame_length;
  
	LOG_INF("scenario_id = %d, framerate = %d\n", scenario_id, framerate);

	switch (scenario_id) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			frame_length = imgsensor_info.pre.pclk / framerate * 10 / imgsensor_info.pre.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.pre.framelength) ? (frame_length - imgsensor_info.pre.framelength) : 0;
			imgsensor.frame_length = imgsensor_info.pre.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			set_dummy();			
			break;			
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			if(framerate == 0)
				return ERROR_NONE;
			frame_length = imgsensor_info.normal_video.pclk / framerate * 10 / imgsensor_info.normal_video.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.normal_video.framelength) ? (frame_length - imgsensor_info.normal_video.framelength) : 0; 		
			imgsensor.frame_length = imgsensor_info.normal_video.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			set_dummy();			
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:		
			frame_length = imgsensor_info.cap.pclk / framerate * 10 / imgsensor_info.cap.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.cap.framelength) ? (frame_length - imgsensor_info.cap.framelength) : 0;
			imgsensor.frame_length = imgsensor_info.cap.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			set_dummy();			
			break;	
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			frame_length = imgsensor_info.hs_video.pclk / framerate * 10 / imgsensor_info.hs_video.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.hs_video.framelength) ? (frame_length - imgsensor_info.hs_video.framelength) : 0;
			imgsensor.frame_length = imgsensor_info.hs_video.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			set_dummy();			
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			frame_length = imgsensor_info.slim_video.pclk / framerate * 10 / imgsensor_info.slim_video.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.slim_video.framelength) ? (frame_length - imgsensor_info.slim_video.framelength): 0;	
			imgsensor.frame_length = imgsensor_info.slim_video.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			set_dummy();	
			break;		
		default:  //                                        
			frame_length = imgsensor_info.pre.pclk / framerate * 10 / imgsensor_info.pre.linelength;
			spin_lock(&imgsensor_drv_lock);
			imgsensor.dummy_line = (frame_length > imgsensor_info.pre.framelength) ? (frame_length - imgsensor_info.pre.framelength) : 0;
			imgsensor.frame_length = imgsensor_info.pre.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			set_dummy();	
			LOG_INF("error scenario_id = %d, we use preview scenario \n", scenario_id);
			break;
	}	
	return ERROR_NONE;
}



static kal_uint32 get_default_framerate_by_scenario(MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 *framerate) 
{
	LOG_INF("scenario_id = %d\n", scenario_id);

	switch (scenario_id) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			*framerate = imgsensor_info.pre.max_framerate;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*framerate = imgsensor_info.normal_video.max_framerate;
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*framerate = imgsensor_info.cap.max_framerate;
			break;		
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*framerate = imgsensor_info.hs_video.max_framerate;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO: 
			*framerate = imgsensor_info.slim_video.max_framerate;
			break;
		default:
			break;
	}

	return ERROR_NONE;
}

static kal_uint32 set_test_pattern_mode(kal_bool enable)
{
	LOG_INF("enable: %d\n", enable);
	
	spin_lock(&imgsensor_drv_lock);
	imgsensor.test_pattern = enable;
	spin_unlock(&imgsensor_drv_lock);
	
    if(enable)
    {
		write_cmos_sensor(0x5000,0x16);//                                   
		write_cmos_sensor(0x5E00,0x80);
		
    }
	else
	{
		write_cmos_sensor(0x5000,0x96);//                                  
		write_cmos_sensor(0x5E00,0x00);
	}

	return ERROR_NONE;
}

static kal_uint32 feature_control(MSDK_SENSOR_FEATURE_ENUM feature_id,
                             UINT8 *feature_para,UINT32 *feature_para_len)
{
    UINT16 *feature_return_para_16=(UINT16 *) feature_para;
    UINT16 *feature_data_16=(UINT16 *) feature_para;
    UINT32 *feature_return_para_32=(UINT32 *) feature_para;
    UINT32 *feature_data_32=(UINT32 *) feature_para;
    unsigned long long *feature_data=(unsigned long long *) feature_para;
    unsigned long long *feature_return_para=(unsigned long long *) feature_para;

    SENSOR_WINSIZE_INFO_STRUCT *wininfo;
    MSDK_SENSOR_REG_INFO_STRUCT *sensor_reg_data=(MSDK_SENSOR_REG_INFO_STRUCT *) feature_para;

    LOG_INF("feature_id = %d\n", feature_id);
    switch (feature_id) {
        case SENSOR_FEATURE_GET_PERIOD:
            *feature_return_para_16++ = imgsensor.line_length;
            *feature_return_para_16 = imgsensor.frame_length;
            *feature_para_len=4;
            break;
        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
            *feature_return_para_32 = imgsensor.pclk;
            *feature_para_len=4;
            break;
        case SENSOR_FEATURE_SET_ESHUTTER:
            set_shutter(*feature_data);
            break;
        case SENSOR_FEATURE_SET_NIGHTMODE:
            night_mode((BOOL) *feature_data);
            break;
        case SENSOR_FEATURE_SET_GAIN:
            set_gain((UINT16) *feature_data);
            break;
        case SENSOR_FEATURE_SET_FLASHLIGHT:
            break;
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
            break;
        case SENSOR_FEATURE_SET_REGISTER:
            write_cmos_sensor(sensor_reg_data->RegAddr, sensor_reg_data->RegData);
            break;
        case SENSOR_FEATURE_GET_REGISTER:
            sensor_reg_data->RegData = read_cmos_sensor(sensor_reg_data->RegAddr);
            break;
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
            //                                                                             
            //                                           
            *feature_return_para_32=LENS_DRIVER_ID_DO_NOT_CARE;
            *feature_para_len=4;
            break;
        case SENSOR_FEATURE_SET_VIDEO_MODE:
            set_video_mode(*feature_data);
            break;
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            get_imgsensor_id(feature_return_para_32);
            break;
        case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
            set_auto_flicker_mode((BOOL)*feature_data_16,*(feature_data_16+1));
            break;
        case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
            set_max_framerate_by_scenario((MSDK_SCENARIO_ID_ENUM)*feature_data, *(feature_data+1));
            break;
        case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
//                                                                                                             
            get_default_framerate_by_scenario((MSDK_SCENARIO_ID_ENUM)*(feature_data), (MUINT32 *)(uintptr_t)(*(feature_data+1)));
            break;
        case SENSOR_FEATURE_SET_TEST_PATTERN:
            set_test_pattern_mode((BOOL)*feature_data);
            break;
        case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE: //                             
            *feature_return_para_32 = imgsensor_info.checksum_value;
            *feature_para_len=4;
            break;
        case SENSOR_FEATURE_SET_FRAMERATE:
            LOG_INF("current fps :%d\n", *feature_data);
            spin_lock(&imgsensor_drv_lock);
            imgsensor.current_fps = *feature_data;
            spin_unlock(&imgsensor_drv_lock);
            break;
        case SENSOR_FEATURE_SET_HDR:
            LOG_INF("ihdr enable :%d\n", (BOOL)*feature_data);
            spin_lock(&imgsensor_drv_lock);
            imgsensor.ihdr_en = (BOOL)*feature_data;
            spin_unlock(&imgsensor_drv_lock);
            break;
        case SENSOR_FEATURE_GET_CROP_INFO:
            LOG_INF("SENSOR_FEATURE_GET_CROP_INFO scenarioId:%d\n", *feature_data);

            wininfo = (SENSOR_WINSIZE_INFO_STRUCT *)(uintptr_t)(*(feature_data+1));

            switch (*feature_data_32) {
                case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[1],sizeof(SENSOR_WINSIZE_INFO_STRUCT));
                    break;
                case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[2],sizeof(SENSOR_WINSIZE_INFO_STRUCT));
                    break;
                case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[3],sizeof(SENSOR_WINSIZE_INFO_STRUCT));
                    break;
                case MSDK_SCENARIO_ID_SLIM_VIDEO:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[4],sizeof(SENSOR_WINSIZE_INFO_STRUCT));
                    break;
                case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
                default:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[0],sizeof(SENSOR_WINSIZE_INFO_STRUCT));
                    break;
            }
        case SENSOR_FEATURE_SET_IHDR_SHUTTER_GAIN:        
            LOG_INF("SENSOR_SET_SENSOR_IHDR LE=%d, SE=%d, Gain=%d\n",(UINT16)*feature_data,(UINT16)*(feature_data+1),(UINT16)*(feature_data+2)); 
            ihdr_write_shutter_gain((UINT16)*feature_data,(UINT16)*(feature_data+1),(UINT16)*(feature_data+2));    
            break;
        default:
            break;
    }

    return ERROR_NONE;
}    /*                       */

static SENSOR_FUNCTION_STRUCT sensor_func = {
	open,
	get_info,
	get_resolution,
	feature_control,
	control,
	close
};

UINT32 OV8865_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/*                                  */
	if (pfFunc!=NULL)
		*pfFunc=&sensor_func;
	return ERROR_NONE;
}	/*                            */
