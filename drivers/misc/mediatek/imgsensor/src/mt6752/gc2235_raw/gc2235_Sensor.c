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

#include "gc2235_Sensor.h"

#define DEBUG 1
/*                                                                                        */
#define PFX "GC2235_camera_sensor"
#if DEBUG
    #define LOG_INF(format, args...)    xlog_printk(ANDROID_LOG_INFO   , PFX, "[%s] " format, __FUNCTION__, ##args)

    #define LOG_1 LOG_INF("GC2235 RAW,MIPI 2LANE\n")
    #define LOG_2 LOG_INF("preview 1280*960@30fps,864Mbps/lane; video 5M@30fps,864Mbps/lane; capture 5M@30fps,864Mbps/lane; HS HD0@120fps,864Mbps/lane; slim HD@30fps,864Mbps/lane\n")
#else
    #define LOG_INF(format, args...)
    #define LOG_1
    #define LOG_2
#endif
/*                                                                                      */


static DEFINE_SPINLOCK(imgsensor_drv_lock);


static imgsensor_info_struct imgsensor_info = {
    .sensor_id = GC2235_SENSOR_ID,        //                                          

    .checksum_value = 0x9f254a0a,        //                     

    .pre = {
        .pclk = 24000000,                //                            
        .linelength = 1030,                //                                  
        .framelength = 1258,            //                                   
        .startx = 8,                    //                                            
        .starty = 6,                    //                                            
        .grabwindow_width = 1584,        //                                           
        .grabwindow_height = 1188,        //                                            
        /*                                                                                     */
        .mipi_data_lp2hs_settle_dc = 85,//         
        /*                                                      */
        .max_framerate = 185,
    },
    .cap = {
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,
    },
    .cap1 = {                            //                                                                                                                                       
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,    //                                                                                                             
    },
    .normal_video = {
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,
    },
    .hs_video = {
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,
    },
    .slim_video = {
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,
    },
    .custom1 = {
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,
    },
    .custom2 = {
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,
    },
    .custom3 = {
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,
    },
    .custom4 = {
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,
    },
    .custom5 = {
        .pclk = 24000000,
        .linelength = 1030,
        .framelength = 1258,
        .startx = 8,
        .starty = 6,
        .grabwindow_width = 1584,
        .grabwindow_height = 1188,
        .mipi_data_lp2hs_settle_dc = 85,//         
        .max_framerate = 185,
    },
    .margin = 0,            //                                   
    .min_shutter = 1,        //           
    .max_frame_length = 8191,//                                               
    .ae_shut_delay_frame = 0,    //                                                                              
    .ae_sensor_gain_delay_frame = 1,//                                                                                        
    .ae_isp_gain_delay_frame = 3,//                                 
    .ihdr_support = 0,      //                         
    .ihdr_le_firstline = 0,  //                        
    .sensor_mode_num = 5,      //                       

    .cap_delay_frame = 4,        //                             
    .pre_delay_frame = 3,         //                             
    .video_delay_frame = 3,        //                           
    .hs_video_delay_frame = 3,    //                                       
    .slim_video_delay_frame = 3,//                                
    .custom1_delay_frame = 2,
    .custom2_delay_frame = 2, 
    .custom3_delay_frame = 2, 
    .custom4_delay_frame = 2, 
    .custom5_delay_frame = 2,

    .isp_driving_current = ISP_DRIVING_8MA, //                    
    .sensor_interface_type = SENSOR_INTERFACE_TYPE_PARALLEL,//                     
    //                                                                           
    //                                                                                                     
    .sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_B,//                               
    .mclk = 24,//                                               
    //                                                   
    .i2c_addr_table = {0x78, 0xff},//                                                                        
    //              
    //              
    .i2c_speed = 300, //                     
};


static imgsensor_struct imgsensor = {
    .mirror = IMAGE_V_MIRROR,                //                      
    .sensor_mode = IMGSENSOR_MODE_INIT, //                                                                                                                        
    .shutter = 0x3D0,                    //               
    .gain = 0x100,                        //            
    .dummy_pixel = 0,                    //                  
    .dummy_line = 0,                    //                 
    .current_fps = 185,  //                                                              
    .autoflicker_en = KAL_FALSE,  //                                                                                         
    .test_pattern = KAL_FALSE,        //                                                                                        
    .current_scenario_id = MSDK_SCENARIO_ID_CAMERA_PREVIEW,//                   
    .ihdr_en = 0, //                                           
    .i2c_write_id = 0x78,//                                    
};


/*                                  */
static SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[] =
{{ 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188}, //        
 { 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188}, //        
 { 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188}, //      
 { 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188}, //                 
 { 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188},//           
 { 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188}, //       
 { 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188}, //        
 { 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188}, //       
 { 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188},//        
 { 1600, 1200,   0,   0, 1600, 1200, 1600, 1200, 0, 0, 1600, 1200,   8,   6, 1584, 1188}};//       



static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;
    char pu_send_cmd[1] = {(char)(addr & 0xFF) };
    kdSetI2CSpeed(imgsensor_info.i2c_speed); //                                              
    
    iReadRegI2C(pu_send_cmd, 1, (u8*)&get_byte, 1, imgsensor.i2c_write_id);

    return get_byte;
}

static void write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
    char pu_send_cmd[2] = {(char)(addr & 0xFF), (char)(para & 0xFF)};
    kdSetI2CSpeed(imgsensor_info.i2c_speed); //                                              
    iWriteRegI2C(pu_send_cmd, 2, imgsensor.i2c_write_id);
}

static void set_dummy(void)
{
    LOG_INF("dummyline = %d, dummypixels = %d \n", imgsensor.dummy_line, imgsensor.dummy_pixel);
    /*                                                                                                                                               */
    //                                                       
    //                                                         
    //                                                      
    //                                                        
	//      
	write_cmos_sensor(0x05, ((imgsensor.line_length - 822)>> 8)& 0xFF);
	write_cmos_sensor(0x06, (imgsensor.line_length - 822) & 0xFF);

	//      
	write_cmos_sensor(0x07, ((imgsensor.frame_length - 1232)>> 8) & 0xFF);
	write_cmos_sensor(0x08, (imgsensor.frame_length - 1232) & 0xFF);

}    /*               */

static kal_uint32 return_sensor_id(void)
{
    return ((read_cmos_sensor(0xf0) << 8) | read_cmos_sensor(0xf1));
}
static void set_max_framerate(UINT16 framerate,kal_bool min_framelength_en)
{
    //                     
    kal_uint32 frame_length = imgsensor.frame_length;
    //                    

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
}    /*                       */



/*                                                                        
          
                
 
             
                                                                   
 
            
                               
 
         
         
 
                  
 
                                                                        */
static void set_shutter(kal_uint16 shutter)
{
    unsigned long flags;
    //                            
    //                            
    spin_lock_irqsave(&imgsensor_drv_lock, flags);
    imgsensor.shutter = shutter;
    spin_unlock_irqrestore(&imgsensor_drv_lock, flags);

    //                       
    /*                                                                                        */
    /*                                                                                                */

    //                      
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
	#if 0
    if (imgsensor.autoflicker_en) {
        realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
        if(realtime_fps >= 297 && realtime_fps <= 305)
            set_max_framerate(296,0);
        else if(realtime_fps >= 147 && realtime_fps <= 150)
            set_max_framerate(146,0);
        else {
        //                    
        write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
        write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
        }
    } else {
        //                    
        write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
        write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
    }
	#endif
    //               
	write_cmos_sensor(0x03, (shutter>>8) & 0x1F);
	write_cmos_sensor(0x04, shutter & 0xFF);

    LOG_INF("Exit! shutter =%d, framelength =%d\n", shutter,imgsensor.frame_length);

}    /*                */


/*
                                                 
 
                                 

                                                                              
                                 
                                
 
*/

/*                                                                        
          
             
 
             
                                                   
 
            
                                           
 
         
                                     
 
                  
 
                                                                        */
static kal_uint16 set_gain(kal_uint16 gain)
{
    kal_uint16 reg_gain = 0,temp;

    /*                                        */
    /*                            */
    /*                             */
    /*                            */

    //
    if (gain < BASEGAIN || gain > 6 * BASEGAIN) {
        LOG_INF("Error gain setting");

        if (gain < BASEGAIN)
            gain = BASEGAIN;
        else if (gain > 6 * BASEGAIN)
            gain = 6 * BASEGAIN;
    }

    //                          
	if(256 > gain) {
		//             
		temp = gain;
		write_cmos_sensor(0xb0, 0x40); //            
		write_cmos_sensor(0xb1, temp);//                       
	} else {
		//             
		temp = 64 * gain / 256; 
		//                       
		if(temp > 0xc0) 
			temp = 0xc0;
		write_cmos_sensor(0xb0, temp); //            
		write_cmos_sensor(0xb1, 0xff);//                       
	}	
    spin_lock(&imgsensor_drv_lock);
    imgsensor.gain = temp;
    spin_unlock(&imgsensor_drv_lock);
    LOG_INF("gain = %d , reg_gain = 0x%x\n ", gain, reg_gain);

    return gain;
}    /*              */

static void ihdr_write_shutter_gain(kal_uint16 le, kal_uint16 se, kal_uint16 gain)
{
#if 0
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
        write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
        write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);

        write_cmos_sensor(0x3502, (le << 4) & 0xFF);
        write_cmos_sensor(0x3501, (le >> 4) & 0xFF);
        write_cmos_sensor(0x3500, (le >> 12) & 0x0F);

        write_cmos_sensor(0x3508, (se << 4) & 0xFF);
        write_cmos_sensor(0x3507, (se >> 4) & 0xFF);
        write_cmos_sensor(0x3506, (se >> 12) & 0x0F);

        set_gain(gain);
    }
#endif    

}



static void set_mirror_flip(kal_uint8 image_mirror)
{
    LOG_INF("image_mirror = %d\n", image_mirror);

    switch (image_mirror) {
        case IMAGE_NORMAL:
            write_cmos_sensor(0x17,0x14);
		    write_cmos_sensor(0x92,0x03);
		    write_cmos_sensor(0x94,0x07);
            break;
        case IMAGE_H_MIRROR:
            write_cmos_sensor(0x17,0x15);
		    write_cmos_sensor(0x92,0x03);
		    write_cmos_sensor(0x94,0x06);
            break;
        case IMAGE_V_MIRROR:
            write_cmos_sensor(0x17,0x16);
		    write_cmos_sensor(0x92,0x02);
		    write_cmos_sensor(0x94,0x07);
            break;
        case IMAGE_HV_MIRROR:
            write_cmos_sensor(0x17,0x17);
		    write_cmos_sensor(0x92,0x02);
		    write_cmos_sensor(0x94,0x06);
            break;
        default:
            LOG_INF("Error image_mirror setting\n");
    }

}

/*                                                                        
          
               
 
             
                                        
 
            
                                                                          
 
         
         
 
                  
 
                                                                        */
static void night_mode(kal_bool enable)
{
/*                                  */
}    /*                  */

static void sensor_init(void)
{
	//                                                   
	//                                                  
	//                                                   
	write_cmos_sensor(0xfe, 0x80);
	write_cmos_sensor(0xfe, 0x80);
	write_cmos_sensor(0xfe, 0x80);
	write_cmos_sensor(0xf2, 0x00);
	write_cmos_sensor(0xf6, 0x00);
	write_cmos_sensor(0xfc, 0x06);
	write_cmos_sensor(0xf7, 0x15); //          
	write_cmos_sensor(0xf8, 0x83); //          
	write_cmos_sensor(0xf9, 0xfe); //              
	write_cmos_sensor(0xfa, 0x00); //   
	write_cmos_sensor(0xfe, 0x00);
	
	//                                                   
	//                                                  
	//                                                   
	write_cmos_sensor(0x03, 0x0a);
	write_cmos_sensor(0x04, 0xec);
	write_cmos_sensor(0x05, 0x00);
	write_cmos_sensor(0x06, 0xd0);
	write_cmos_sensor(0x07, 0x00);
	write_cmos_sensor(0x08, 0x1a);
	write_cmos_sensor(0x0a, 0x02);
	write_cmos_sensor(0x0c, 0x00);
	write_cmos_sensor(0x0d, 0x04);
	write_cmos_sensor(0x0e, 0xd0);
	write_cmos_sensor(0x0f, 0x06); 
	write_cmos_sensor(0x10, 0x50);
	
	write_cmos_sensor(0x17, 0x15);//                      
	write_cmos_sensor(0x18, 0x12); //     
	write_cmos_sensor(0x19, 0x06);
	write_cmos_sensor(0x1a, 0x01);
	write_cmos_sensor(0x1b, 0x48);
	write_cmos_sensor(0x1e, 0x88); 
	write_cmos_sensor(0x1f, 0x48); 
	write_cmos_sensor(0x20, 0x03);
	write_cmos_sensor(0x21, 0x6f);
	write_cmos_sensor(0x22, 0x80); 
	write_cmos_sensor(0x23, 0xc1);
	write_cmos_sensor(0x24, 0x2f);
	write_cmos_sensor(0x26, 0x01);
	write_cmos_sensor(0x27, 0x30);
	write_cmos_sensor(0x3f, 0x00);
	
	//                                                   
	//                                                  
	//                                                   
	write_cmos_sensor(0x8b, 0xa0);
	write_cmos_sensor(0x8c, 0x02);
	write_cmos_sensor(0x90, 0x01);
	write_cmos_sensor(0x92, 0x02);
	write_cmos_sensor(0x94, 0x06);
	write_cmos_sensor(0x95, 0x04);
	write_cmos_sensor(0x96, 0xb0);
	write_cmos_sensor(0x97, 0x06);
	write_cmos_sensor(0x98, 0x40);
	
	//                                                   
	//                                                  
	//                                                   
	write_cmos_sensor(0x40, 0x72); //             
	write_cmos_sensor(0x41, 0x04);
	write_cmos_sensor(0x5e, 0x00);
	write_cmos_sensor(0x5f, 0x00);
	write_cmos_sensor(0x60, 0x00);
	write_cmos_sensor(0x61, 0x00); 
	write_cmos_sensor(0x62, 0x00);
	write_cmos_sensor(0x63, 0x00); 
	write_cmos_sensor(0x64, 0x00);
	write_cmos_sensor(0x65, 0x00);
	write_cmos_sensor(0x66, 0x20);
	write_cmos_sensor(0x67, 0x20); 
	write_cmos_sensor(0x68, 0x20);
	write_cmos_sensor(0x69, 0x20);
	mDELAY(100);

	
	//                                                   
	//                                                
	//                                                   
	write_cmos_sensor(0xb2, 0x00);
	write_cmos_sensor(0xb3, 0x40);
	write_cmos_sensor(0xb4, 0x40);
	write_cmos_sensor(0xb5, 0x40);
	
	//                                                   
	//                                                   
	//                                                   
	write_cmos_sensor(0xb8, 0x0f);
	write_cmos_sensor(0xb9, 0x23);
	write_cmos_sensor(0xba, 0xff);
	write_cmos_sensor(0xbc, 0x00);
	write_cmos_sensor(0xbd, 0x00);
	write_cmos_sensor(0xbe, 0xff);
	write_cmos_sensor(0xbf, 0x09);

	//                                                   
	//                                                  
	//                                                   
	write_cmos_sensor(0xfe, 0x03);
	write_cmos_sensor(0x01, 0x00);
	write_cmos_sensor(0x02, 0x00);
	write_cmos_sensor(0x03, 0x00);
	write_cmos_sensor(0x06, 0x00);
	write_cmos_sensor(0x10, 0x00);
	write_cmos_sensor(0x15, 0x00);
	write_cmos_sensor(0xfe, 0x00);
	write_cmos_sensor(0xf2, 0x0f);
}    /*                 */


static void preview_setting(void)
{

}    /*                     */

static void capture_setting(kal_uint16 currefps)
{


}

static void normal_video_setting(kal_uint16 currefps)
{
   

}
static void hs_video_setting(void)
{
    
}

static void slim_video_setting(void)
{
    
}

static kal_uint32 set_test_pattern_mode(kal_bool enable)
{
    LOG_INF("enable: %d\n", enable);

    if (enable) {
        //                                
        //                                                               
        //                                                                                     
        write_cmos_sensor(0xfe, 0x00);
        write_cmos_sensor(0x03, 0x05);
        write_cmos_sensor(0x04, 0x4b); 
        write_cmos_sensor(0x18, 0x16);
        write_cmos_sensor(0x40, 0x20);
        write_cmos_sensor(0x44, 0x00);
        write_cmos_sensor(0x45, 0x00);
        write_cmos_sensor(0x46, 0x00);
        write_cmos_sensor(0x47, 0x00);
        write_cmos_sensor(0x48, 0x00);
        write_cmos_sensor(0x49, 0x00);
        write_cmos_sensor(0x4a, 0x00);
        write_cmos_sensor(0x4b, 0x00);
        write_cmos_sensor(0x54, 0x00);
        write_cmos_sensor(0x55, 0x00);
        write_cmos_sensor(0x56, 0x00);
        write_cmos_sensor(0x57, 0x00);
        write_cmos_sensor(0x58, 0x00);
        write_cmos_sensor(0x59, 0x00);
        write_cmos_sensor(0x5a, 0x00);
        write_cmos_sensor(0x5b, 0x00);
        write_cmos_sensor(0xc4, 0x00);
        write_cmos_sensor(0xc5, 0x00);
        write_cmos_sensor(0xc6, 0x00);
        write_cmos_sensor(0xc7, 0x00);
        write_cmos_sensor(0xc8, 0x00);
        write_cmos_sensor(0xc9, 0x00);
        write_cmos_sensor(0xca, 0x00);
        write_cmos_sensor(0xcb, 0x00);
        write_cmos_sensor(0xd4, 0x00);
        write_cmos_sensor(0xd5, 0x00);
        write_cmos_sensor(0xd6, 0x00);
        write_cmos_sensor(0xd7, 0x00);
        write_cmos_sensor(0xd8, 0x00);
        write_cmos_sensor(0xd9, 0x00);
        write_cmos_sensor(0xda, 0x00);
        write_cmos_sensor(0xdb, 0x00);
        write_cmos_sensor(0xb0, 0x40);
        write_cmos_sensor(0xb3, 0x40);
        write_cmos_sensor(0xb4, 0x40);
        write_cmos_sensor(0xb5, 0x40);
        write_cmos_sensor(0x8c, 0x12);
        mDELAY(50);
    } else {
        //                                
        //                                                               
        write_cmos_sensor(0x8b,0xa0); //                                                     
    }
    spin_lock(&imgsensor_drv_lock);
    imgsensor.test_pattern = enable;
    spin_unlock(&imgsensor_drv_lock);
    return ERROR_NONE;
}

/*                                                                        
          
                     
 
             
                                    
 
            
                                     
 
         
         
 
                  
 
                                                                        */
static kal_uint32 get_imgsensor_id(UINT32 *sensor_id)
{
    kal_uint8 i = 0;
    kal_uint8 retry = 2;
    //                                                                                               
    while (imgsensor_info.i2c_addr_table[i] != 0xff) {
        spin_lock(&imgsensor_drv_lock);
        imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
        spin_unlock(&imgsensor_drv_lock);
        do {
            *sensor_id = return_sensor_id();
            if (*sensor_id == imgsensor_info.sensor_id) {
                LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,*sensor_id);
                return ERROR_NONE;
            }
            LOG_INF("Read sensor id fail, write id: 0x%x, id: 0x%x\n", imgsensor.i2c_write_id,*sensor_id);
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
    kal_uint32 sensor_id = 0;
    LOG_1;
    LOG_2;
    //                                                                                               
    while (imgsensor_info.i2c_addr_table[i] != 0xff) {
        spin_lock(&imgsensor_drv_lock);
        imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
        spin_unlock(&imgsensor_drv_lock);
        do {
            sensor_id = return_sensor_id();
            if (sensor_id == imgsensor_info.sensor_id) {
                LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,sensor_id);
                break;
            }
            LOG_INF("Read sensor id fail, write id: 0x%x, id: 0x%x\n", imgsensor.i2c_write_id,sensor_id);
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
    imgsensor.pclk = imgsensor_info.pre.pclk;
    imgsensor.frame_length = imgsensor_info.pre.framelength;
    imgsensor.line_length = imgsensor_info.pre.linelength;
    imgsensor.min_frame_length = imgsensor_info.pre.framelength;
    imgsensor.dummy_pixel = 0;
    imgsensor.dummy_line = 0;
	imgsensor.ihdr_en = KAL_FALSE;
    imgsensor.test_pattern = KAL_FALSE;
    imgsensor.current_fps = imgsensor_info.pre.max_framerate;
    spin_unlock(&imgsensor_drv_lock);

    return ERROR_NONE;
}    /*          */



/*                                                                        
          
          
 
             
 
 
            
         
 
         
         
 
                  
 
                                                                        */
static kal_uint32 close(void)
{
    LOG_INF("E\n");

    /*                                  */

    return ERROR_NONE;
}    /*           */


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
}    /*              */

/*                                                                        
          
            
 
             
                                                                  
 
            
 
         
         
 
                  
 
                                                                        */
static kal_uint32 capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                          MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");
	
    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_CAPTURE;
    if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {//                                                                 
        imgsensor.pclk = imgsensor_info.cap1.pclk;
        imgsensor.line_length = imgsensor_info.cap1.linelength;
        imgsensor.frame_length = imgsensor_info.cap1.framelength;
        imgsensor.min_frame_length = imgsensor_info.cap1.framelength;
        imgsensor.autoflicker_en = KAL_FALSE;
    } else {
        if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
            LOG_INF("Warning: current_fps %d fps is not support, so use cap's setting: %d fps!\n",imgsensor.current_fps,imgsensor_info.cap.max_framerate/10);
        imgsensor.pclk = imgsensor_info.cap.pclk;
        imgsensor.line_length = imgsensor_info.cap.linelength;
        imgsensor.frame_length = imgsensor_info.cap.framelength;
        imgsensor.min_frame_length = imgsensor_info.cap.framelength;
        imgsensor.autoflicker_en = KAL_FALSE;
    }
    spin_unlock(&imgsensor_drv_lock);
    capture_setting(imgsensor.current_fps);
	set_mirror_flip(imgsensor.mirror);
    return ERROR_NONE;
}    /*           */
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
}    /*                   */

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
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    hs_video_setting();	
	set_mirror_flip(imgsensor.mirror);
    return ERROR_NONE;
}    /*               */

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
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    slim_video_setting();
    set_mirror_flip(imgsensor.mirror);
    return ERROR_NONE;
}   /*               */
/*                                                                        
          
         
 
             
                                           
 
            
                                                                           
                                                                               
 
         
        
 
                  
 
                                                                        */
static kal_uint32 custom1(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM1;
    imgsensor.pclk = imgsensor_info.custom1.pclk;
    //                                 
    imgsensor.line_length = imgsensor_info.custom1.linelength;
    imgsensor.frame_length = imgsensor_info.custom1.framelength; 
    imgsensor.min_frame_length = imgsensor_info.custom1.framelength;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    preview_setting();
    return ERROR_NONE;
}   /*            */

static kal_uint32 custom2(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM2;
    imgsensor.pclk = imgsensor_info.custom2.pclk;
    //                                 
    imgsensor.line_length = imgsensor_info.custom2.linelength;
    imgsensor.frame_length = imgsensor_info.custom2.framelength; 
    imgsensor.min_frame_length = imgsensor_info.custom2.framelength;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    preview_setting();
    return ERROR_NONE;
}   /*            */

static kal_uint32 custom3(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM3;
    imgsensor.pclk = imgsensor_info.custom3.pclk;
    //                                 
    imgsensor.line_length = imgsensor_info.custom3.linelength;
    imgsensor.frame_length = imgsensor_info.custom3.framelength; 
    imgsensor.min_frame_length = imgsensor_info.custom3.framelength;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    preview_setting();
    return ERROR_NONE;
}   /*            */

static kal_uint32 custom4(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM4;
    imgsensor.pclk = imgsensor_info.custom4.pclk;
    //                                 
    imgsensor.line_length = imgsensor_info.custom4.linelength;
    imgsensor.frame_length = imgsensor_info.custom4.framelength; 
    imgsensor.min_frame_length = imgsensor_info.custom4.framelength;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    preview_setting();
    return ERROR_NONE;
}   /*            */
static kal_uint32 custom5(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM5;
    imgsensor.pclk = imgsensor_info.custom5.pclk;
    //                                 
    imgsensor.line_length = imgsensor_info.custom5.linelength;
    imgsensor.frame_length = imgsensor_info.custom5.framelength; 
    imgsensor.min_frame_length = imgsensor_info.custom5.framelength;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    preview_setting();
    return ERROR_NONE;
}   /*            */
static kal_uint32 get_resolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *sensor_resolution)
{
    LOG_INF("E\n");
    
    sensor_resolution->SensorFullWidth = imgsensor_info.cap.grabwindow_width;
    sensor_resolution->SensorFullHeight = imgsensor_info.cap.grabwindow_height;

    sensor_resolution->SensorPreviewWidth = imgsensor_info.pre.grabwindow_width;
    sensor_resolution->SensorPreviewHeight = imgsensor_info.pre.grabwindow_height;

    sensor_resolution->SensorVideoWidth = imgsensor_info.normal_video.grabwindow_width;
    sensor_resolution->SensorVideoHeight = imgsensor_info.normal_video.grabwindow_height;


    sensor_resolution->SensorHighSpeedVideoWidth     = imgsensor_info.hs_video.grabwindow_width;
    sensor_resolution->SensorHighSpeedVideoHeight     = imgsensor_info.hs_video.grabwindow_height;

    sensor_resolution->SensorSlimVideoWidth     = imgsensor_info.slim_video.grabwindow_width;
    sensor_resolution->SensorSlimVideoHeight     = imgsensor_info.slim_video.grabwindow_height;
    sensor_resolution->SensorCustom1Width  = imgsensor_info.custom1.grabwindow_width;
    sensor_resolution->SensorCustom1Height     = imgsensor_info.custom1.grabwindow_height;

    sensor_resolution->SensorCustom2Width  = imgsensor_info.custom2.grabwindow_width;
    sensor_resolution->SensorCustom2Height     = imgsensor_info.custom2.grabwindow_height;

    sensor_resolution->SensorCustom3Width  = imgsensor_info.custom3.grabwindow_width;
    sensor_resolution->SensorCustom3Height     = imgsensor_info.custom3.grabwindow_height;

    sensor_resolution->SensorCustom4Width  = imgsensor_info.custom4.grabwindow_width;
    sensor_resolution->SensorCustom4Height     = imgsensor_info.custom4.grabwindow_height;

    sensor_resolution->SensorCustom5Width  = imgsensor_info.custom5.grabwindow_width;
    sensor_resolution->SensorCustom5Height     = imgsensor_info.custom5.grabwindow_height;
    return ERROR_NONE;
}    /*                      */

static kal_uint32 get_info(MSDK_SCENARIO_ID_ENUM scenario_id,
                      MSDK_SENSOR_INFO_STRUCT *sensor_info,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("scenario_id = %d\n", scenario_id);


    //                                                                                               
    //                                                                                            
    //                                                                                           

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
    sensor_info->Custom1DelayFrame = imgsensor_info.custom1_delay_frame; 
    sensor_info->Custom2DelayFrame = imgsensor_info.custom2_delay_frame; 
    sensor_info->Custom3DelayFrame = imgsensor_info.custom3_delay_frame; 
    sensor_info->Custom4DelayFrame = imgsensor_info.custom4_delay_frame; 
    sensor_info->Custom5DelayFrame = imgsensor_info.custom5_delay_frame; 

    sensor_info->SensorMasterClockSwitch = 0; /*         */
    sensor_info->SensorDrivingCurrent = imgsensor_info.isp_driving_current;

    sensor_info->AEShutDelayFrame = imgsensor_info.ae_shut_delay_frame;          /*                                                   */
    sensor_info->AESensorGainDelayFrame = imgsensor_info.ae_sensor_gain_delay_frame;    /*                                  */
    sensor_info->AEISPGainDelayFrame = imgsensor_info.ae_isp_gain_delay_frame;
    sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
    sensor_info->IHDR_LE_FirstLine = imgsensor_info.ihdr_le_firstline;
    sensor_info->SensorModeNum = imgsensor_info.sensor_mode_num;
    //                                                                                                                                                           
    //                                                                 
    sensor_info->SensorClockFreq = imgsensor_info.mclk;
    sensor_info->SensorClockDividCount = 3; /*         */
    sensor_info->SensorClockRisingCount = 0;
    sensor_info->SensorClockFallingCount = 2; /*         */
    sensor_info->SensorPixelClockCount = 3; /*         */
    sensor_info->SensorDataLatchCount = 2; /*         */

    sensor_info->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
    sensor_info->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
    sensor_info->SensorWidthSampling = 0;  //                
    sensor_info->SensorHightSampling = 0;    //                
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
        case MSDK_SCENARIO_ID_CUSTOM1:
            sensor_info->SensorGrabStartX = imgsensor_info.custom1.startx; 
            sensor_info->SensorGrabStartY = imgsensor_info.custom1.starty;   
            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom1.mipi_data_lp2hs_settle_dc; 

            break;
        case MSDK_SCENARIO_ID_CUSTOM2:
            sensor_info->SensorGrabStartX = imgsensor_info.custom2.startx; 
            sensor_info->SensorGrabStartY = imgsensor_info.custom2.starty;   
            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom2.mipi_data_lp2hs_settle_dc; 

            break;
        case MSDK_SCENARIO_ID_CUSTOM3:
            sensor_info->SensorGrabStartX = imgsensor_info.custom3.startx; 
            sensor_info->SensorGrabStartY = imgsensor_info.custom3.starty;   
            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom3.mipi_data_lp2hs_settle_dc; 

            break;
        case MSDK_SCENARIO_ID_CUSTOM4:
            sensor_info->SensorGrabStartX = imgsensor_info.custom4.startx; 
            sensor_info->SensorGrabStartY = imgsensor_info.custom4.starty;   
            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom4.mipi_data_lp2hs_settle_dc; 

            break;
        case MSDK_SCENARIO_ID_CUSTOM5:
            sensor_info->SensorGrabStartX = imgsensor_info.custom5.startx; 
            sensor_info->SensorGrabStartY = imgsensor_info.custom5.starty;   
            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom5.mipi_data_lp2hs_settle_dc; 

            break;
        default:
            sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
            sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
            break;
    }

    return ERROR_NONE;
}    /*              */


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
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
            normal_video(image_window, sensor_config_data);
            break;
        case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
            hs_video(image_window, sensor_config_data);
            break;
        case MSDK_SCENARIO_ID_SLIM_VIDEO:
            slim_video(image_window, sensor_config_data);
            break;
        case MSDK_SCENARIO_ID_CUSTOM1:
            custom1(image_window, sensor_config_data); //        
            break;
        case MSDK_SCENARIO_ID_CUSTOM2:
            custom2(image_window, sensor_config_data); //        
            break;
        case MSDK_SCENARIO_ID_CUSTOM3:
            custom3(image_window, sensor_config_data); //        
            break;
        case MSDK_SCENARIO_ID_CUSTOM4:
            custom4(image_window, sensor_config_data); //        
            break;
        case MSDK_SCENARIO_ID_CUSTOM5:
            custom5(image_window, sensor_config_data); //        
            break;
        default:
            LOG_INF("Error ScenarioId setting");
            preview(image_window, sensor_config_data);
            return ERROR_INVALID_SCENARIO_ID;
    }
    return ERROR_NONE;
}    /*           */



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
    if (enable) //                   
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
        	  if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {
                frame_length = imgsensor_info.cap1.pclk / framerate * 10 / imgsensor_info.cap1.linelength;
                spin_lock(&imgsensor_drv_lock);
		            imgsensor.dummy_line = (frame_length > imgsensor_info.cap1.framelength) ? (frame_length - imgsensor_info.cap1.framelength) : 0;
		            imgsensor.frame_length = imgsensor_info.cap1.framelength + imgsensor.dummy_line;
		            imgsensor.min_frame_length = imgsensor.frame_length;
		            spin_unlock(&imgsensor_drv_lock);
            } else {
        		    if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
                    LOG_INF("Warning: current_fps %d fps is not support, so use cap's setting: %d fps!\n",framerate,imgsensor_info.cap.max_framerate/10);
                frame_length = imgsensor_info.cap.pclk / framerate * 10 / imgsensor_info.cap.linelength;
                spin_lock(&imgsensor_drv_lock);
		            imgsensor.dummy_line = (frame_length > imgsensor_info.cap.framelength) ? (frame_length - imgsensor_info.cap.framelength) : 0;
		            imgsensor.frame_length = imgsensor_info.cap.framelength + imgsensor.dummy_line;
		            imgsensor.min_frame_length = imgsensor.frame_length;
		            spin_unlock(&imgsensor_drv_lock);
            }
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
        case MSDK_SCENARIO_ID_CUSTOM1:
            frame_length = imgsensor_info.custom1.pclk / framerate * 10 / imgsensor_info.custom1.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.custom1.framelength) ? (frame_length - imgsensor_info.custom1.framelength) : 0;
            if (imgsensor.dummy_line < 0)
                imgsensor.dummy_line = 0;
            imgsensor.frame_length = imgsensor_info.custom1.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
            set_dummy();            
            break;
        case MSDK_SCENARIO_ID_CUSTOM2:
            frame_length = imgsensor_info.custom2.pclk / framerate * 10 / imgsensor_info.custom2.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.custom2.framelength) ? (frame_length - imgsensor_info.custom2.framelength) : 0;
            if (imgsensor.dummy_line < 0)
                imgsensor.dummy_line = 0;
            imgsensor.frame_length = imgsensor_info.custom2.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
            set_dummy();            
            break; 
        case MSDK_SCENARIO_ID_CUSTOM3:
            frame_length = imgsensor_info.custom3.pclk / framerate * 10 / imgsensor_info.custom3.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.custom3.framelength) ? (frame_length - imgsensor_info.custom3.framelength) : 0;
            if (imgsensor.dummy_line < 0)
                imgsensor.dummy_line = 0;
            imgsensor.frame_length = imgsensor_info.custom3.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
            set_dummy();            
            break; 
        case MSDK_SCENARIO_ID_CUSTOM4:
            frame_length = imgsensor_info.custom4.pclk / framerate * 10 / imgsensor_info.custom4.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.custom4.framelength) ? (frame_length - imgsensor_info.custom4.framelength) : 0;
            if (imgsensor.dummy_line < 0)
                imgsensor.dummy_line = 0;
            imgsensor.frame_length = imgsensor_info.custom4.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
            set_dummy();            
            break; 
        case MSDK_SCENARIO_ID_CUSTOM5:
            frame_length = imgsensor_info.custom5.pclk / framerate * 10 / imgsensor_info.custom5.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.custom5.framelength) ? (frame_length - imgsensor_info.custom5.framelength) : 0;
            if (imgsensor.dummy_line < 0)
                imgsensor.dummy_line = 0;
            imgsensor.frame_length = imgsensor_info.custom5.framelength + imgsensor.dummy_line;
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
        case MSDK_SCENARIO_ID_CUSTOM1:
            *framerate = imgsensor_info.custom1.max_framerate;
            break;
        case MSDK_SCENARIO_ID_CUSTOM2:
            *framerate = imgsensor_info.custom2.max_framerate;
            break;
        case MSDK_SCENARIO_ID_CUSTOM3:
            *framerate = imgsensor_info.custom3.max_framerate;
            break;
        case MSDK_SCENARIO_ID_CUSTOM4:
            *framerate = imgsensor_info.custom4.max_framerate;
            break;
        case MSDK_SCENARIO_ID_CUSTOM5:
            *framerate = imgsensor_info.custom5.max_framerate;
            break;
        default:
            break;
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

UINT32 GC2235_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
    /*                                  */
    if (pfFunc!=NULL)
        *pfFunc=&sensor_func;
    return ERROR_NONE;
}    /*                                  */
