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
#include <asm/system.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"

#include "ov5648mipi_Sensor.h"
#include "ov5648mipi_Camera_Sensor_para.h"
#include "ov5648mipi_CameraCustomized.h"

//                               
#define OV5648MIPI_DEBUG
#ifdef OV5648MIPI_DEBUG
#define SENSORDB(fmt, arg...) printk("%s: " fmt "\n", __FUNCTION__ ,##arg)
#else
#define SENSORDB(x,...)
#endif

typedef enum
{
    OV5648MIPI_SENSOR_MODE_INIT,
    OV5648MIPI_SENSOR_MODE_PREVIEW,  
    OV5648MIPI_SENSOR_MODE_CAPTURE,
    OV5648MIPI_SENSOR_MODE_VIDEO
} OV5648MIPI_SENSOR_MODE;

/*                       */
typedef struct OV5648MIPI_sensor_STRUCT
{
    MSDK_SENSOR_CONFIG_STRUCT cfg_data;
    sensor_data_struct eng; /*               */
    MSDK_SENSOR_ENG_INFO_STRUCT eng_info;
    kal_uint8 mirror;

    OV5648MIPI_SENSOR_MODE ov5648mipi_sensor_mode;
    
    kal_bool video_mode;
    kal_bool NightMode;
    kal_uint16 normal_fps; /*                           */
    kal_uint16 night_fps; /*                          */
    kal_uint16 FixedFps;
    kal_uint16 shutter;
    kal_uint16 gain;
    kal_uint32 pclk;

	kal_uint32 pvPclk;
	kal_uint32 videoPclk;
	kal_uint32 capPclk;
	
    kal_uint16 frame_length;
    kal_uint16 line_length;

    kal_uint16 dummy_pixel;
    kal_uint16 dummy_line;

    kal_uint32 maxExposureLines;
} OV5648MIPI_sensor_struct;

static OV5648MIPI_sensor_struct OV5648MIPI_sensor =
{
    .eng =
    {
        .reg = CAMERA_SENSOR_REG_DEFAULT_VALUE,
        .cct = CAMERA_SENSOR_CCT_DEFAULT_VALUE,
    },
    .eng_info =
    {
        .SensorId = 128,
        .SensorType = CMOS_SENSOR,
        .SensorOutputDataFormat = OV5648MIPI_COLOR_FORMAT,
    },
    .ov5648mipi_sensor_mode = OV5648MIPI_SENSOR_MODE_INIT,
    .shutter = 0x3D0,  
    .gain = 0x100,
    .pclk = OV5648MIPI_PREVIEW_CLK,
    .pvPclk = OV5648MIPI_PREVIEW_CLK,
    .capPclk = OV5648MIPI_CAPTURE_CLK,
    .videoPclk = OV5648MIPI_VIDEO_CLK,
    .frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS,
    .line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS,
    .dummy_pixel = 0,
    .dummy_line = 0,
};

static DEFINE_SPINLOCK(ov5648mipi_drv_lock);

static MSDK_SCENARIO_ID_ENUM mCurrentScenarioId = MSDK_SCENARIO_ID_CAMERA_PREVIEW;
static kal_bool OV5648MIPIAutoFlickerMode = KAL_FALSE;
kal_bool OV5648MIPIDuringTestPattern = KAL_FALSE;
#define OV5648MIPI_TEST_PATTERN_CHECKSUM (0x03d80c0a)

extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
//                                                   


kal_uint16 OV5648MIPI_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;

    char puSendCmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };
    iReadRegI2C(puSendCmd, 2, (u8*)&get_byte, 1, OV5648MIPI_WRITE_ID);

    return get_byte;
}

void OV5648MIPI_write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
    char puSendCmd[3] = {(char)(addr >> 8), (char)(addr & 0xFF), (char)(para & 0xFF)};
    iWriteRegI2C(puSendCmd, 3, OV5648MIPI_WRITE_ID);
}

static void OV5648MIPI_Write_Shutter(kal_uint16 iShutter)
{
	kal_uint32 min_framelength = 0, max_shutter=0;
	kal_uint32 extra_lines = 0;
    kal_uint32 frame_length = 0;
    kal_uint32 line_length = 0;
    unsigned long flags;


    #ifdef OV5648MIPI_DRIVER_TRACE
        SENSORDB("iShutter =  %d", iShutter);
    #endif

    if(OV5648MIPIAutoFlickerMode)
    {
		if (OV5648MIPI_SENSOR_MODE_PREVIEW  == OV5648MIPI_sensor.ov5648mipi_sensor_mode)
		{
			line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel;
			max_shutter = OV5648MIPI_PV_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line;
		}
        else if (OV5648MIPI_SENSOR_MODE_VIDEO  == OV5648MIPI_sensor.ov5648mipi_sensor_mode)
        {
            line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel;
            max_shutter = OV5648MIPI_PV_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line;
        }
		else
		{
			line_length = OV5648MIPI_FULL_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel;
			max_shutter = OV5648MIPI_FULL_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line;
		}

		switch(mCurrentScenarioId)
		{
        	case MSDK_SCENARIO_ID_CAMERA_ZSD:
			case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
				SENSORDB("AutoFlickerMode!!! MSDK_SCENARIO_ID_CAMERA_ZSD  0!!\n");
				min_framelength = max_shutter;//                                     
				break;
			case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
				if(OV5648MIPI_sensor.FixedFps == 30)
				{
					min_framelength = (OV5648MIPI_sensor.videoPclk) /(OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel)/285*10 ;
				}
				else if(OV5648MIPI_sensor.FixedFps == 15)
				{
					min_framelength = (OV5648MIPI_sensor.videoPclk) /(OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel)/146*10 ;
				}
				else
				{
					min_framelength = max_shutter;
				}
				break;
			default:
				min_framelength = (OV5648MIPI_sensor.pvPclk) /(OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel)/285*10 ;
    			break;
		}

        //                                                                                                                   
        //                                                                                                                
        
        if (iShutter < 3)
            iShutter = 3;
        
        if (iShutter > max_shutter-4)
            extra_lines = iShutter - max_shutter + 4;
        else
            extra_lines = 0;
        
        if (OV5648MIPI_SENSOR_MODE_PREVIEW  == OV5648MIPI_sensor.ov5648mipi_sensor_mode)
        {
            frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line + extra_lines ;
        }
        else if (OV5648MIPI_SENSOR_MODE_VIDEO  == OV5648MIPI_sensor.ov5648mipi_sensor_mode)
        {
            frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line + extra_lines ;
        }        
        else
        {
            frame_length = OV5648MIPI_FULL_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line + extra_lines ;
        }
        //                                              


        if (frame_length < min_framelength)
        {       
            switch(mCurrentScenarioId)
            {
            case MSDK_SCENARIO_ID_CAMERA_ZSD:
            case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
                extra_lines = min_framelength- (OV5648MIPI_FULL_PERIOD_LINE_NUMS+ OV5648MIPI_sensor.dummy_line);
                break;
            case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
                extra_lines = min_framelength- (OV5648MIPI_PV_PERIOD_LINE_NUMS+ OV5648MIPI_sensor.dummy_line);
                break;
            default:
                extra_lines = min_framelength- (OV5648MIPI_PV_PERIOD_LINE_NUMS+ OV5648MIPI_sensor.dummy_line);
                break;
            }
            frame_length = min_framelength;
        }
        
        //                                               

        
        //                      
        OV5648MIPI_write_cmos_sensor(0x380e, (frame_length >> 8) & 0xFF);
        OV5648MIPI_write_cmos_sensor(0x380f, frame_length & 0xFF);
        
        spin_lock_irqsave(&ov5648mipi_drv_lock,flags);
        OV5648MIPI_sensor.maxExposureLines = frame_length;
        OV5648MIPI_sensor.line_length = line_length;
        OV5648MIPI_sensor.frame_length = frame_length;
        spin_unlock_irqrestore(&ov5648mipi_drv_lock,flags);
        
        //                                                   
        OV5648MIPI_write_cmos_sensor(0x3500, (iShutter>>12) & 0x0F);
        OV5648MIPI_write_cmos_sensor(0x3501, (iShutter>>4) & 0xFF);
        OV5648MIPI_write_cmos_sensor(0x3502, (iShutter<<4) & 0xF0);  /*                              */
        
        //                                              
        SENSORDB("shutter=%d, extra_lines=%d, line_length=%d, frame_length=%d\n", iShutter, extra_lines, line_length, frame_length);
    }    
	else
	{
		if (OV5648MIPI_SENSOR_MODE_PREVIEW  == OV5648MIPI_sensor.ov5648mipi_sensor_mode)
		{
			line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel;
			max_shutter = OV5648MIPI_PV_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line;
		}
        else if (OV5648MIPI_SENSOR_MODE_VIDEO  == OV5648MIPI_sensor.ov5648mipi_sensor_mode)
        {
			line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel;
			max_shutter = OV5648MIPI_PV_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line;
        }         
		else
		{
			line_length = OV5648MIPI_FULL_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel;
			max_shutter = OV5648MIPI_FULL_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line;
		}


        if (iShutter < 3)
            iShutter = 3;
        
        if (iShutter > max_shutter-4)
            extra_lines = iShutter - max_shutter + 4;
        else
            extra_lines = 0;
        
        if (OV5648MIPI_SENSOR_MODE_PREVIEW  == OV5648MIPI_sensor.ov5648mipi_sensor_mode)
        {
            frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line + extra_lines ;
        }
        else if (OV5648MIPI_SENSOR_MODE_VIDEO  == OV5648MIPI_sensor.ov5648mipi_sensor_mode)
        {
            frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line + extra_lines ;
        }        
        else
        {
            frame_length = OV5648MIPI_FULL_PERIOD_LINE_NUMS + OV5648MIPI_sensor.dummy_line + extra_lines ;
        }
        //                                              

        //                      
        OV5648MIPI_write_cmos_sensor(0x380e, (frame_length >> 8) & 0xFF);
        OV5648MIPI_write_cmos_sensor(0x380f, frame_length & 0xFF);
        

        spin_lock_irqsave(&ov5648mipi_drv_lock,flags);
        OV5648MIPI_sensor.maxExposureLines = frame_length;
        OV5648MIPI_sensor.line_length = line_length;
        OV5648MIPI_sensor.frame_length = frame_length;
        spin_unlock_irqrestore(&ov5648mipi_drv_lock,flags);
        
        //                                                   
        OV5648MIPI_write_cmos_sensor(0x3500, (iShutter>>12) & 0x0F);
        OV5648MIPI_write_cmos_sensor(0x3501, (iShutter>>4) & 0xFF);
        OV5648MIPI_write_cmos_sensor(0x3502, (iShutter<<4) & 0xF0);  /*                              */
        
        //                                              
        SENSORDB("shutter=%d, extra_lines=%d, line_length=%d, frame_length=%d\n", iShutter, extra_lines, line_length, frame_length);
	}
}   /*                            */

static void OV5648MIPI_Set_Dummy(const kal_uint16 iDummyPixels, const kal_uint16 iDummyLines)
{
    kal_uint16 line_length, frame_length;

    #ifdef OV5648MIPI_DRIVER_TRACE
        SENSORDB("iDummyPixels = %d, iDummyLines = %d ", iDummyPixels, iDummyLines);
    #endif

    if (OV5648MIPI_SENSOR_MODE_PREVIEW == OV5648MIPI_sensor.ov5648mipi_sensor_mode){
        line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS + iDummyPixels;
        frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS + iDummyLines;
    }
    else if (OV5648MIPI_SENSOR_MODE_VIDEO  == OV5648MIPI_sensor.ov5648mipi_sensor_mode)
    {
        line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS + iDummyPixels;
        frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS + iDummyLines;
    }      
    else
    {
        line_length = OV5648MIPI_FULL_PERIOD_PIXEL_NUMS + iDummyPixels;
        frame_length = OV5648MIPI_FULL_PERIOD_LINE_NUMS + iDummyLines;
    }

    OV5648MIPI_sensor.dummy_pixel = iDummyPixels;
    OV5648MIPI_sensor.dummy_line = iDummyLines;
    OV5648MIPI_sensor.line_length = line_length;
    OV5648MIPI_sensor.frame_length = frame_length;
    
    OV5648MIPI_write_cmos_sensor(0x380c, line_length >> 8);
    OV5648MIPI_write_cmos_sensor(0x380d, line_length & 0xFF);
    OV5648MIPI_write_cmos_sensor(0x380e, frame_length >> 8);
    OV5648MIPI_write_cmos_sensor(0x380f, frame_length & 0xFF);
    
}   /*                        */

/*
                                                  
 
                         
                                                             
                        

                                  
                                                   
          

                                                                                              

                                                   
                                                  
                                                        

                                                         
                                                                     
        
                                                                   
        
                                       

                                                                     
 
*/

/*                                                                        
          
                         
 
             
                                                                      
 
            
                              
 
         
        
 
                  
 
                                                                        */
void set_OV5648MIPI_shutter(kal_uint16 iShutter)
{
    unsigned long flags;
    
    spin_lock_irqsave(&ov5648mipi_drv_lock, flags);
    OV5648MIPI_sensor.shutter = iShutter;
    spin_unlock_irqrestore(&ov5648mipi_drv_lock, flags);
    
    OV5648MIPI_Write_Shutter(iShutter);
}   /*                         */

#if 0
static kal_uint16 OV5648MIPI_Reg2Gain(const kal_uint8 iReg)
{
    kal_uint16 iGain ;
    /*                  */
    iGain = (iReg >> 4) * BASEGAIN + (iReg & 0xF) * BASEGAIN / 16; 
    return iGain ;
}
#endif

 kal_uint16 OV5648MIPI_Gain2Reg(const kal_uint16 iGain)
{
    kal_uint16 iReg = 0x0000;
    
    iReg = ((iGain / BASEGAIN) << 4) + ((iGain % BASEGAIN) * 16 / BASEGAIN);
    iReg = iReg & 0xFFFF;
    return (kal_uint16)iReg;
}


/*                                                                        
          
                      
 
             
                                                  
 
            
                                          
 
         
                                    
 
                  
 
                                                                        */
kal_uint16 OV5648MIPI_SetGain(kal_uint16 iGain)
{
    kal_uint16 iRegGain;

    OV5648MIPI_sensor.gain = iGain;

    /*                                        */
    /*                          */
    /*                           */
    /*                            */

    //
    if(iGain < BASEGAIN || iGain > 32 * BASEGAIN){
        SENSORDB("Error gain setting");
        if(iGain < BASEGAIN) iGain = BASEGAIN;
        if(iGain > 32 * BASEGAIN) iGain = 32 * BASEGAIN;        
    }
    
        iRegGain = OV5648MIPI_Gain2Reg(iGain);

        #ifdef OV5648MIPI_DRIVER_TRACE
            SENSORDB("iGain = %d , iRegGain = 0x%x ", iGain, iRegGain);
        #endif

    OV5648MIPI_write_cmos_sensor(0x350a, iRegGain >> 8);
    OV5648MIPI_write_cmos_sensor(0x350b, iRegGain & 0xFF);    
    
    return iGain;
}   /*                      */


void OV5648MIPI_Set_Mirror_Flip(kal_uint8 image_mirror)
{
    SENSORDB("image_mirror = %d", image_mirror);

    /*                                                       
        
                                      
                                         
        
                                          
                                             
        
                                                                          
        
                                                              */
    
	switch (image_mirror)
	{
		case IMAGE_NORMAL:
		    OV5648MIPI_write_cmos_sensor(0x3820,((OV5648MIPI_read_cmos_sensor(0x3820) & 0xF9) | 0x00));
		    OV5648MIPI_write_cmos_sensor(0x3821,((OV5648MIPI_read_cmos_sensor(0x3821) & 0xF9) | 0x06));
		    break;
		case IMAGE_H_MIRROR:
		    OV5648MIPI_write_cmos_sensor(0x3820,((OV5648MIPI_read_cmos_sensor(0x3820) & 0xF9) | 0x00));
		    OV5648MIPI_write_cmos_sensor(0x3821,((OV5648MIPI_read_cmos_sensor(0x3821) & 0xF9) | 0x00));
		    break;
		case IMAGE_V_MIRROR:
		    OV5648MIPI_write_cmos_sensor(0x3820,((OV5648MIPI_read_cmos_sensor(0x3820) & 0xF9) | 0x06));
		    OV5648MIPI_write_cmos_sensor(0x3821,((OV5648MIPI_read_cmos_sensor(0x3821) & 0xF9) | 0x06));		
		    break;
		case IMAGE_HV_MIRROR:
		    OV5648MIPI_write_cmos_sensor(0x3820,((OV5648MIPI_read_cmos_sensor(0x3820) & 0xF9) | 0x06));
		    OV5648MIPI_write_cmos_sensor(0x3821,((OV5648MIPI_read_cmos_sensor(0x3821) & 0xF9) | 0x00));
		    break;
		default:
		    SENSORDB("Error image_mirror setting");
	}

}


/*                                                                        
          
                        
 
             
                                           
 
            
                                                                         
 
         
        
 
                  
 
                                                                        */
void OV5648MIPI_night_mode(kal_bool enable)
{
/*                                  */ 
}   /*                         */


/*                                      */
static void OV5648MIPI_camera_para_to_sensor(void)
{
    kal_uint32 i;

    SENSORDB("OV5648MIPI_camera_para_to_sensor\n");

    for (i = 0; 0xFFFFFFFF != OV5648MIPI_sensor.eng.reg[i].Addr; i++)
    {
        OV5648MIPI_write_cmos_sensor(OV5648MIPI_sensor.eng.reg[i].Addr, OV5648MIPI_sensor.eng.reg[i].Para);
    }
    for (i = OV5648MIPI_FACTORY_START_ADDR; 0xFFFFFFFF != OV5648MIPI_sensor.eng.reg[i].Addr; i++)
    {
        OV5648MIPI_write_cmos_sensor(OV5648MIPI_sensor.eng.reg[i].Addr, OV5648MIPI_sensor.eng.reg[i].Para);
    }
    OV5648MIPI_SetGain(OV5648MIPI_sensor.gain); /*             */
}

/*                                         */
static void OV5648MIPI_sensor_to_camera_para(void)
{
    kal_uint32 i,temp_data;

    SENSORDB("OV5648MIPI_sensor_to_camera_para\n");

    for (i = 0; 0xFFFFFFFF != OV5648MIPI_sensor.eng.reg[i].Addr; i++)
    {
        temp_data =OV5648MIPI_read_cmos_sensor(OV5648MIPI_sensor.eng.reg[i].Addr);
     
        spin_lock(&ov5648mipi_drv_lock);
        OV5648MIPI_sensor.eng.reg[i].Para = temp_data;
        spin_unlock(&ov5648mipi_drv_lock);
    }
    for (i = OV5648MIPI_FACTORY_START_ADDR; 0xFFFFFFFF != OV5648MIPI_sensor.eng.reg[i].Addr; i++)
    {
        temp_data =OV5648MIPI_read_cmos_sensor(OV5648MIPI_sensor.eng.reg[i].Addr);
    
        spin_lock(&ov5648mipi_drv_lock);
        OV5648MIPI_sensor.eng.reg[i].Para = temp_data;
        spin_unlock(&ov5648mipi_drv_lock);
    }
}

/*                                                                 */
inline static void OV5648MIPI_get_sensor_group_count(kal_int32 *sensor_count_ptr)
{

    SENSORDB("OV5648MIPI_get_sensor_group_count\n");

    *sensor_count_ptr = OV5648MIPI_GROUP_TOTAL_NUMS;
}

inline static void OV5648MIPI_get_sensor_group_info(MSDK_SENSOR_GROUP_INFO_STRUCT *para)
{

    SENSORDB("OV5648MIPI_get_sensor_group_info\n");

    switch (para->GroupIdx)
    {
    case OV5648MIPI_PRE_GAIN:
        sprintf(para->GroupNamePtr, "CCT");
        para->ItemCount = 5;
        break;
    case OV5648MIPI_CMMCLK_CURRENT:
        sprintf(para->GroupNamePtr, "CMMCLK Current");
        para->ItemCount = 1;
        break;
    case OV5648MIPI_FRAME_RATE_LIMITATION:
        sprintf(para->GroupNamePtr, "Frame Rate Limitation");
        para->ItemCount = 2;
        break;
    case OV5648MIPI_REGISTER_EDITOR:
        sprintf(para->GroupNamePtr, "Register Editor");
        para->ItemCount = 2;
        break;
    default:
        ASSERT(0);
  }
}

inline static void OV5648MIPI_get_sensor_item_info(MSDK_SENSOR_ITEM_INFO_STRUCT *para)
{

    const static kal_char *cct_item_name[] = {"SENSOR_BASEGAIN", "Pregain-R", "Pregain-Gr", "Pregain-Gb", "Pregain-B"};
    const static kal_char *editer_item_name[] = {"REG addr", "REG value"};
  
    SENSORDB("OV5648MIPI_get_sensor_item_info");

    switch (para->GroupIdx)
    {
    case OV5648MIPI_PRE_GAIN:
        switch (para->ItemIdx)
        {
        case OV5648MIPI_SENSOR_BASEGAIN:
        case OV5648MIPI_PRE_GAIN_R_INDEX:
        case OV5648MIPI_PRE_GAIN_Gr_INDEX:
        case OV5648MIPI_PRE_GAIN_Gb_INDEX:
        case OV5648MIPI_PRE_GAIN_B_INDEX:
            break;
        default:
            ASSERT(0);
        }
        sprintf(para->ItemNamePtr, cct_item_name[para->ItemIdx - OV5648MIPI_SENSOR_BASEGAIN]);
        para->ItemValue = OV5648MIPI_sensor.eng.cct[para->ItemIdx].Para * 1000 / BASEGAIN;
        para->IsTrueFalse = para->IsReadOnly = para->IsNeedRestart = KAL_FALSE;
        para->Min = OV5648MIPI_MIN_ANALOG_GAIN * 1000;
        para->Max = OV5648MIPI_MAX_ANALOG_GAIN * 1000;
        break;
    case OV5648MIPI_CMMCLK_CURRENT:
        switch (para->ItemIdx)
        {
        case 0:
            sprintf(para->ItemNamePtr, "Drv Cur[2,4,6,8]mA");
            switch (OV5648MIPI_sensor.eng.reg[OV5648MIPI_CMMCLK_CURRENT_INDEX].Para)
            {
            case ISP_DRIVING_2MA:
                para->ItemValue = 2;
                break;
            case ISP_DRIVING_4MA:
                para->ItemValue = 4;
                break;
            case ISP_DRIVING_6MA:
                para->ItemValue = 6;
                break;
            case ISP_DRIVING_8MA:
                para->ItemValue = 8;
                break;
            default:
                ASSERT(0);
            }
            para->IsTrueFalse = para->IsReadOnly = KAL_FALSE;
            para->IsNeedRestart = KAL_TRUE;
            para->Min = 2;
            para->Max = 8;
            break;
        default:
            ASSERT(0);
        }
        break;
    case OV5648MIPI_FRAME_RATE_LIMITATION:
        switch (para->ItemIdx)
        {
        case 0:
            sprintf(para->ItemNamePtr, "Max Exposure Lines");
            para->ItemValue = 5998;
            break;
        case 1:
            sprintf(para->ItemNamePtr, "Min Frame Rate");
            para->ItemValue = 5;
            break;
        default:
            ASSERT(0);
        }
        para->IsTrueFalse = para->IsNeedRestart = KAL_FALSE;
        para->IsReadOnly = KAL_TRUE;
        para->Min = para->Max = 0;
        break;
    case OV5648MIPI_REGISTER_EDITOR:
        switch (para->ItemIdx)
        {
        case 0:
        case 1:
            sprintf(para->ItemNamePtr, editer_item_name[para->ItemIdx]);
            para->ItemValue = 0;
            para->IsTrueFalse = para->IsReadOnly = para->IsNeedRestart = KAL_FALSE;
            para->Min = 0;
            para->Max = (para->ItemIdx == 0 ? 0xFFFF : 0xFF);
            break;
        default:
            ASSERT(0);
        }
        break;
    default:
        ASSERT(0);
  }
}

inline static kal_bool OV5648MIPI_set_sensor_item_info(MSDK_SENSOR_ITEM_INFO_STRUCT *para)
{
    kal_uint16 temp_para;

    SENSORDB("OV5648MIPI_set_sensor_item_info\n");

    switch (para->GroupIdx)
    {
    case OV5648MIPI_PRE_GAIN:
        switch (para->ItemIdx)
        {
        case OV5648MIPI_SENSOR_BASEGAIN:
        case OV5648MIPI_PRE_GAIN_R_INDEX:
        case OV5648MIPI_PRE_GAIN_Gr_INDEX:
        case OV5648MIPI_PRE_GAIN_Gb_INDEX:
        case OV5648MIPI_PRE_GAIN_B_INDEX:
            spin_lock(&ov5648mipi_drv_lock);
            OV5648MIPI_sensor.eng.cct[para->ItemIdx].Para = para->ItemValue * BASEGAIN / 1000;
            spin_unlock(&ov5648mipi_drv_lock);
            OV5648MIPI_SetGain(OV5648MIPI_sensor.gain); /*             */
            break;
        default:
            ASSERT(0);
        }
        break;
    case OV5648MIPI_CMMCLK_CURRENT:
        switch (para->ItemIdx)
        {
        case 0:
            switch (para->ItemValue)
            {
            case 2:
                temp_para = ISP_DRIVING_2MA;
                break;
            case 3:
            case 4:
                temp_para = ISP_DRIVING_4MA;
                break;
            case 5:
            case 6:
                temp_para = ISP_DRIVING_6MA;
                break;
            default:
                temp_para = ISP_DRIVING_8MA;
                break;
            }
            spin_lock(&ov5648mipi_drv_lock);
            //                                              
            OV5648MIPI_sensor.eng.reg[OV5648MIPI_CMMCLK_CURRENT_INDEX].Para = temp_para;
            spin_unlock(&ov5648mipi_drv_lock);
            break;
        default:
            ASSERT(0);
        }
        break;
    case OV5648MIPI_FRAME_RATE_LIMITATION:
        ASSERT(0);
        break;
    case OV5648MIPI_REGISTER_EDITOR:
        switch (para->ItemIdx)
        {
        static kal_uint32 fac_sensor_reg;
        case 0:
            if (para->ItemValue < 0 || para->ItemValue > 0xFFFF) return KAL_FALSE;
            fac_sensor_reg = para->ItemValue;
            break;
        case 1:
            if (para->ItemValue < 0 || para->ItemValue > 0xFF) return KAL_FALSE;
            OV5648MIPI_write_cmos_sensor(fac_sensor_reg, para->ItemValue);
            break;
        default:
            ASSERT(0);
        }
        break;
    default:
        ASSERT(0);
    }

    return KAL_TRUE;
}

static void OV5648MIPI_Sensor_Init(void)
{
#ifdef OV5648MIPI_DRIVER_TRACE
   SENSORDB("Enter!");
#endif

   /*                                                                            
                              
                                                         
                                                          
        
                                   
                                                                                                                                     
    
                            
                                            
                                                
    
                            
                                             
                                           
    
                    
                                       
                           
                            
    
                                                                          
                                                                                   
                                
                                                                           
                                           
    
                                                                  
                                                                                                                                                                        
    
                   
    
                                                                
                                                                                                                                                          

                  
               
               
               
               
               

                          
                                    
                   
    */

    //                                                                                  
    OV5648MIPI_write_cmos_sensor(0x0100, 0x00); //                
    OV5648MIPI_write_cmos_sensor(0x0103, 0x01); //               

    mDELAY(5);

    OV5648MIPI_write_cmos_sensor(0x3001, 0x00); //                    
    OV5648MIPI_write_cmos_sensor(0x3002, 0x00); //                     
    OV5648MIPI_write_cmos_sensor(0x3011, 0x02); //                  
    
    OV5648MIPI_write_cmos_sensor(0x3018, 0x4c); //            
    OV5648MIPI_write_cmos_sensor(0x3022, 0x00);
    
    OV5648MIPI_write_cmos_sensor(0x3034, 0x1a); //            
    OV5648MIPI_write_cmos_sensor(0x3035, 0x21); //    
    OV5648MIPI_write_cmos_sensor(0x3036, 0x69); //    
    
    OV5648MIPI_write_cmos_sensor(0x3037, 0x03); //    
    
    OV5648MIPI_write_cmos_sensor(0x3038, 0x00); //    
    OV5648MIPI_write_cmos_sensor(0x3039, 0x00); //    
    OV5648MIPI_write_cmos_sensor(0x303a, 0x00); //     
    OV5648MIPI_write_cmos_sensor(0x303b, 0x19); //     
    OV5648MIPI_write_cmos_sensor(0x303c, 0x11); //     
    OV5648MIPI_write_cmos_sensor(0x303d, 0x30); //     
    OV5648MIPI_write_cmos_sensor(0x3105, 0x11);
    OV5648MIPI_write_cmos_sensor(0x3106, 0x05); //    
    OV5648MIPI_write_cmos_sensor(0x3304, 0x28);
    OV5648MIPI_write_cmos_sensor(0x3305, 0x41);
    OV5648MIPI_write_cmos_sensor(0x3306, 0x30);
    OV5648MIPI_write_cmos_sensor(0x3308, 0x00);
    OV5648MIPI_write_cmos_sensor(0x3309, 0xc8);
    OV5648MIPI_write_cmos_sensor(0x330a, 0x01);
    OV5648MIPI_write_cmos_sensor(0x330b, 0x90);
    OV5648MIPI_write_cmos_sensor(0x330c, 0x02);
    OV5648MIPI_write_cmos_sensor(0x330d, 0x58);
    OV5648MIPI_write_cmos_sensor(0x330e, 0x03);
    OV5648MIPI_write_cmos_sensor(0x330f, 0x20);
    OV5648MIPI_write_cmos_sensor(0x3300, 0x00);

    //              
    //                                                               
    //                                                              
    //                                                                                     

    OV5648MIPI_write_cmos_sensor(0x3503, 0x07); //                                  

    //     
    //                                                        
    //                                                                   
    
    OV5648MIPI_write_cmos_sensor(0x3601, 0x33); //               
    OV5648MIPI_write_cmos_sensor(0x3602, 0x00); //               
    OV5648MIPI_write_cmos_sensor(0x3611, 0x0e); //               
    OV5648MIPI_write_cmos_sensor(0x3612, 0x2b); //               
    OV5648MIPI_write_cmos_sensor(0x3614, 0x50); //               
    OV5648MIPI_write_cmos_sensor(0x3620, 0x33); //               
    OV5648MIPI_write_cmos_sensor(0x3622, 0x00); //               
    OV5648MIPI_write_cmos_sensor(0x3630, 0xad); //               
    OV5648MIPI_write_cmos_sensor(0x3631, 0x00); //               
    OV5648MIPI_write_cmos_sensor(0x3632, 0x94); //               
    OV5648MIPI_write_cmos_sensor(0x3633, 0x17); //               
    OV5648MIPI_write_cmos_sensor(0x3634, 0x14); //               

    //              
    OV5648MIPI_write_cmos_sensor(0x3704, 0xc0); //               
    
    OV5648MIPI_write_cmos_sensor(0x3705, 0x2a); //               
    OV5648MIPI_write_cmos_sensor(0x3708, 0x66); //               
    OV5648MIPI_write_cmos_sensor(0x3709, 0x52); //               
    OV5648MIPI_write_cmos_sensor(0x370b, 0x23); //               
    OV5648MIPI_write_cmos_sensor(0x370c, 0xc3); //               
    OV5648MIPI_write_cmos_sensor(0x370d, 0x00); //               
    OV5648MIPI_write_cmos_sensor(0x370e, 0x00); //               
    OV5648MIPI_write_cmos_sensor(0x371c, 0x07); //               
    OV5648MIPI_write_cmos_sensor(0x3739, 0xd2); //               
    OV5648MIPI_write_cmos_sensor(0x373c, 0x00);
    
    OV5648MIPI_write_cmos_sensor(0x3800, 0x00); //           
    OV5648MIPI_write_cmos_sensor(0x3801, 0x00); //       
    OV5648MIPI_write_cmos_sensor(0x3802, 0x00); //           
    OV5648MIPI_write_cmos_sensor(0x3803, 0x00); //       
    OV5648MIPI_write_cmos_sensor(0x3804, 0x0a); //            
    OV5648MIPI_write_cmos_sensor(0x3805, 0x3f); //     
    OV5648MIPI_write_cmos_sensor(0x3806, 0x07); //            
    OV5648MIPI_write_cmos_sensor(0x3807, 0xa3); //     
    OV5648MIPI_write_cmos_sensor(0x3808, 0x05); //                     
    OV5648MIPI_write_cmos_sensor(0x3809, 0x10); //              
    OV5648MIPI_write_cmos_sensor(0x380a, 0x03); //                    
    OV5648MIPI_write_cmos_sensor(0x380b, 0xcc); //              
    OV5648MIPI_write_cmos_sensor(0x380c, 0x0b); //           
    OV5648MIPI_write_cmos_sensor(0x380d, 0x00); //    
    OV5648MIPI_write_cmos_sensor(0x380e, 0x03); //          
    OV5648MIPI_write_cmos_sensor(0x380f, 0xe0); //    
    
    OV5648MIPI_write_cmos_sensor(0x3810, 0x00); //              
    OV5648MIPI_write_cmos_sensor(0x3811, 0x08); //          
    OV5648MIPI_write_cmos_sensor(0x3812, 0x00); //              
    OV5648MIPI_write_cmos_sensor(0x3813, 0x04); //          
    OV5648MIPI_write_cmos_sensor(0x3814, 0x31); //      
    OV5648MIPI_write_cmos_sensor(0x3815, 0x31); //      
    OV5648MIPI_write_cmos_sensor(0x3817, 0x00); //            

    //                   
    OV5648MIPI_write_cmos_sensor(0x3820, 0x0E); //                          
    OV5648MIPI_write_cmos_sensor(0x3821, 0x01); //                         
    
    OV5648MIPI_write_cmos_sensor(0x3826, 0x03);
    OV5648MIPI_write_cmos_sensor(0x3829, 0x00);
    OV5648MIPI_write_cmos_sensor(0x382b, 0x0b);
    OV5648MIPI_write_cmos_sensor(0x3830, 0x00);
    OV5648MIPI_write_cmos_sensor(0x3836, 0x00);
    OV5648MIPI_write_cmos_sensor(0x3837, 0x00);
    OV5648MIPI_write_cmos_sensor(0x3838, 0x00);
    OV5648MIPI_write_cmos_sensor(0x3839, 0x04);
    OV5648MIPI_write_cmos_sensor(0x383a, 0x00);
    OV5648MIPI_write_cmos_sensor(0x383b, 0x01);

    OV5648MIPI_write_cmos_sensor(0x3b00, 0x00); //           
    OV5648MIPI_write_cmos_sensor(0x3b02, 0x08); //              
    OV5648MIPI_write_cmos_sensor(0x3b03, 0x00); //              
    OV5648MIPI_write_cmos_sensor(0x3b04, 0x04); //         
    OV5648MIPI_write_cmos_sensor(0x3b05, 0x00); //         
    OV5648MIPI_write_cmos_sensor(0x3b06, 0x04);
    OV5648MIPI_write_cmos_sensor(0x3b07, 0x08); //         
    OV5648MIPI_write_cmos_sensor(0x3b08, 0x00); //             
    OV5648MIPI_write_cmos_sensor(0x3b09, 0x02); //                
    OV5648MIPI_write_cmos_sensor(0x3b0a, 0x04); //                
    OV5648MIPI_write_cmos_sensor(0x3b0b, 0x00); //                  
    OV5648MIPI_write_cmos_sensor(0x3b0c, 0x3d); //                  
    OV5648MIPI_write_cmos_sensor(0x3f01, 0x0d);
    OV5648MIPI_write_cmos_sensor(0x3f0f, 0xf5);
    
    OV5648MIPI_write_cmos_sensor(0x4000, 0x89); //           
    OV5648MIPI_write_cmos_sensor(0x4001, 0x02); //               
    OV5648MIPI_write_cmos_sensor(0x4002, 0x45); //                                 
    OV5648MIPI_write_cmos_sensor(0x4004, 0x02); //                  
    OV5648MIPI_write_cmos_sensor(0x4005, 0x18); //                  
    OV5648MIPI_write_cmos_sensor(0x4006, 0x08);
    OV5648MIPI_write_cmos_sensor(0x4007, 0x10);
    OV5648MIPI_write_cmos_sensor(0x4008, 0x00);
    OV5648MIPI_write_cmos_sensor(0x4300, 0xf8);
    OV5648MIPI_write_cmos_sensor(0x4303, 0xff);
    OV5648MIPI_write_cmos_sensor(0x4304, 0x00);
    OV5648MIPI_write_cmos_sensor(0x4307, 0xff);
    OV5648MIPI_write_cmos_sensor(0x4520, 0x00);
    OV5648MIPI_write_cmos_sensor(0x4521, 0x00);
    OV5648MIPI_write_cmos_sensor(0x4511, 0x22);

    //                   
    OV5648MIPI_write_cmos_sensor(0x5780, 0xfc);
    OV5648MIPI_write_cmos_sensor(0x5781, 0x1f);
    OV5648MIPI_write_cmos_sensor(0x5782, 0x03);
    OV5648MIPI_write_cmos_sensor(0x5786, 0x20);
    OV5648MIPI_write_cmos_sensor(0x5787, 0x40);
    OV5648MIPI_write_cmos_sensor(0x5788, 0x08);
    OV5648MIPI_write_cmos_sensor(0x5789, 0x08);
    OV5648MIPI_write_cmos_sensor(0x578a, 0x02);
    OV5648MIPI_write_cmos_sensor(0x578b, 0x01);
    OV5648MIPI_write_cmos_sensor(0x578c, 0x01);
    OV5648MIPI_write_cmos_sensor(0x578d, 0x0c);
    OV5648MIPI_write_cmos_sensor(0x578e, 0x02);
    OV5648MIPI_write_cmos_sensor(0x578f, 0x01);
    OV5648MIPI_write_cmos_sensor(0x5790, 0x01);

    OV5648MIPI_write_cmos_sensor(0x4800, 0x14); //                      
    
    OV5648MIPI_write_cmos_sensor(0x481f, 0x3c); //                     
    OV5648MIPI_write_cmos_sensor(0x4826, 0x00); //                    
    OV5648MIPI_write_cmos_sensor(0x4837, 0x18); //                   
    OV5648MIPI_write_cmos_sensor(0x4b00, 0x06);
    OV5648MIPI_write_cmos_sensor(0x4b01, 0x0a);
    OV5648MIPI_write_cmos_sensor(0x5000, 0xff); //               
    OV5648MIPI_write_cmos_sensor(0x5001, 0x00); //            
    OV5648MIPI_write_cmos_sensor(0x5002, 0x41); //                            
    OV5648MIPI_write_cmos_sensor(0x5003, 0x0a); //                    
    OV5648MIPI_write_cmos_sensor(0x5004, 0x00); //             
    OV5648MIPI_write_cmos_sensor(0x5043, 0x00);
    OV5648MIPI_write_cmos_sensor(0x5013, 0x00);
    OV5648MIPI_write_cmos_sensor(0x501f, 0x03); //                
    OV5648MIPI_write_cmos_sensor(0x503d, 0x00); //                 
    OV5648MIPI_write_cmos_sensor(0x5180, 0x08); //                  
    OV5648MIPI_write_cmos_sensor(0x5a00, 0x08);
    OV5648MIPI_write_cmos_sensor(0x5b00, 0x01);
    OV5648MIPI_write_cmos_sensor(0x5b01, 0x40);
    OV5648MIPI_write_cmos_sensor(0x5b02, 0x00);
    OV5648MIPI_write_cmos_sensor(0x5b03, 0xf0);
    
    //                                                        
    OV5648MIPI_write_cmos_sensor(0x4837, 0x17); //                   

    OV5648MIPI_write_cmos_sensor(0x0100, 0x01); //                            

#ifdef OV5648MIPI_DRIVER_TRACE
    SENSORDB("Exit!");
#endif
}   /*                          */


static void OV5648MIPI_Preview_Setting(void)
{
#ifdef OV5648MIPI_DRIVER_TRACE
    SENSORDB("Enter!");
#endif


    /*                                                       
        
                                                  
        
                                                              */

    OV5648MIPI_write_cmos_sensor(0x0100, 0x00); //           

    //                                                               
    //                                                       
    //                                                       
    OV5648MIPI_write_cmos_sensor(0x3708, 0x66);

    OV5648MIPI_write_cmos_sensor(0x3709, 0x52);
    OV5648MIPI_write_cmos_sensor(0x370c, 0xc3);
    OV5648MIPI_write_cmos_sensor(0x3800, 0x00); //            
    OV5648MIPI_write_cmos_sensor(0x3801, 0x00); //        
    OV5648MIPI_write_cmos_sensor(0x3802, 0x00); //            
    OV5648MIPI_write_cmos_sensor(0x3803, 0x00); //        
    OV5648MIPI_write_cmos_sensor(0x3804, 0x0a); //            
    OV5648MIPI_write_cmos_sensor(0x3805, 0x3f); //     
    OV5648MIPI_write_cmos_sensor(0x3806, 0x07); //            
    OV5648MIPI_write_cmos_sensor(0x3807, 0xa3); //     
    OV5648MIPI_write_cmos_sensor(0x3808, 0x05); //                     
    OV5648MIPI_write_cmos_sensor(0x3809, 0x10); //              
    OV5648MIPI_write_cmos_sensor(0x380a, 0x03); //                    
    OV5648MIPI_write_cmos_sensor(0x380b, 0xcc); //              


    //                                                         
    //                                                  
    //                                                        
    //                                                  
    OV5648MIPI_write_cmos_sensor(0x380c, ((OV5648MIPI_PV_PERIOD_PIXEL_NUMS >> 8) & 0xFF)); //           
    OV5648MIPI_write_cmos_sensor(0x380d, (OV5648MIPI_PV_PERIOD_PIXEL_NUMS & 0xFF));        //    
    OV5648MIPI_write_cmos_sensor(0x380e, ((OV5648MIPI_PV_PERIOD_LINE_NUMS >> 8) & 0xFF));  //          
    OV5648MIPI_write_cmos_sensor(0x380f, (OV5648MIPI_PV_PERIOD_LINE_NUMS & 0xFF));         //    
    
    OV5648MIPI_write_cmos_sensor(0x3810, 0x00); //              
    OV5648MIPI_write_cmos_sensor(0x3811, 0x08); //          
    OV5648MIPI_write_cmos_sensor(0x3812, 0x00); //              
    OV5648MIPI_write_cmos_sensor(0x3813, 0x04); //          
    OV5648MIPI_write_cmos_sensor(0x3814, 0x31); //      
    OV5648MIPI_write_cmos_sensor(0x3815, 0x31); //      
    OV5648MIPI_write_cmos_sensor(0x3817, 0x00); //            


    /*                                                       
        
                                      
                                         
        
                                          
                                             
        
                                                                          
        
                                                              */
    OV5648MIPI_write_cmos_sensor(0x3820, 0x0E); //                          
    OV5648MIPI_write_cmos_sensor(0x3821, 0x01); //                         

    
    OV5648MIPI_write_cmos_sensor(0x4004, 0x02); //                  
    //                                                                
    OV5648MIPI_write_cmos_sensor(0x4005, 0x18); //                  
    //                                                                
    OV5648MIPI_write_cmos_sensor(0x4050, 0x6e); //                  
    OV5648MIPI_write_cmos_sensor(0x4051, 0x8f); //                  

    //                                                        
    OV5648MIPI_write_cmos_sensor(0x4837, 0x17); //                   
    
    OV5648MIPI_write_cmos_sensor(0x0100, 0x01); //         

#ifdef OV5648MIPI_DRIVER_TRACE
    SENSORDB("Exit!");
#endif
}   /*                              */


static void OV5648MIPI_Capture_Setting(void)
{
#ifdef OV5648MIPI_DRIVER_TRACE
    SENSORDB("Enter!");
#endif

    /*                                                       
        
                                                   
        
                                                              */

    OV5648MIPI_write_cmos_sensor(0x0100, 0x00); //           

    //                                                               
    //                                                       
    //                                                       
    OV5648MIPI_write_cmos_sensor(0x3708, 0x63);
    
    OV5648MIPI_write_cmos_sensor(0x3709, 0x12);
    OV5648MIPI_write_cmos_sensor(0x370c, 0xc0);
    OV5648MIPI_write_cmos_sensor(0x3800, 0x00); //           
    OV5648MIPI_write_cmos_sensor(0x3801, 0x00); //       
    OV5648MIPI_write_cmos_sensor(0x3802, 0x00); //           
    OV5648MIPI_write_cmos_sensor(0x3803, 0x00); //       
    OV5648MIPI_write_cmos_sensor(0x3804, 0x0a); //            
    OV5648MIPI_write_cmos_sensor(0x3805, 0x3f); //     
    OV5648MIPI_write_cmos_sensor(0x3806, 0x07); //            
    OV5648MIPI_write_cmos_sensor(0x3807, 0xa3); //     
    OV5648MIPI_write_cmos_sensor(0x3808, 0x0a); //                     
    OV5648MIPI_write_cmos_sensor(0x3809, 0x20); //              
    OV5648MIPI_write_cmos_sensor(0x380a, 0x07); //                     
    OV5648MIPI_write_cmos_sensor(0x380b, 0x98); //              

    //                                                         
    //                                                  
    //                                                         
    //                                                  
    OV5648MIPI_write_cmos_sensor(0x380c, ((OV5648MIPI_FULL_PERIOD_PIXEL_NUMS >> 8) & 0xFF)); //           
    OV5648MIPI_write_cmos_sensor(0x380d, (OV5648MIPI_FULL_PERIOD_PIXEL_NUMS & 0xFF));        //    
    OV5648MIPI_write_cmos_sensor(0x380e, ((OV5648MIPI_FULL_PERIOD_LINE_NUMS >> 8) & 0xFF));  //           
    OV5648MIPI_write_cmos_sensor(0x380f, (OV5648MIPI_FULL_PERIOD_LINE_NUMS & 0xFF));         //    

    OV5648MIPI_write_cmos_sensor(0x3810, 0x00); //               
    OV5648MIPI_write_cmos_sensor(0x3811, 0x10); //          
    OV5648MIPI_write_cmos_sensor(0x3812, 0x00); //              
    OV5648MIPI_write_cmos_sensor(0x3813, 0x06); //          
    OV5648MIPI_write_cmos_sensor(0x3814, 0x11); //      
    OV5648MIPI_write_cmos_sensor(0x3815, 0x11); //      
    OV5648MIPI_write_cmos_sensor(0x3817, 0x00); //            


    /*                                                       
        
                                      
                                         
        
                                          
                                             
        
                                                                          
        
                                                              */
    OV5648MIPI_write_cmos_sensor(0x3820, 0x46); //                           
    OV5648MIPI_write_cmos_sensor(0x3821, 0x00); //                          

    
    OV5648MIPI_write_cmos_sensor(0x4004, 0x04); //                  
    //                                               
    OV5648MIPI_write_cmos_sensor(0x4005, 0x18); //                  
    
    //                                                        
    OV5648MIPI_write_cmos_sensor(0x4837, 0x17); //                   
   
    OV5648MIPI_write_cmos_sensor(0x0100, 0x01); //           

#ifdef OV5648MIPI_DRIVER_TRACE
    SENSORDB("Exit!");
#endif
}   /*                              */


/*                                                                        
          
                  
 
             
                                                         
 
            
        
 
         
        
 
                  
 
                                                                        */
UINT32 OV5648MIPIOpen(void)
{
    kal_uint16 sensor_id = 0; 

    //                           
    sensor_id=((OV5648MIPI_read_cmos_sensor(0x300A) << 8) | OV5648MIPI_read_cmos_sensor(0x300B));   
    SENSORDB("sensor_id = 0x%x ", sensor_id);
    
    if (sensor_id != OV5648MIPI_SENSOR_ID){
        return ERROR_SENSOR_CONNECT_FAIL;
    }
    
    /*                            */
    OV5648MIPI_Sensor_Init();

    spin_lock(&ov5648mipi_drv_lock);
    OV5648MIPIDuringTestPattern = KAL_FALSE;    
    OV5648MIPIAutoFlickerMode = KAL_FALSE;
    OV5648MIPI_sensor.ov5648mipi_sensor_mode = OV5648MIPI_SENSOR_MODE_INIT;
    OV5648MIPI_sensor.shutter = 0x3D0;
    OV5648MIPI_sensor.gain = 0x100;
    OV5648MIPI_sensor.pclk = OV5648MIPI_PREVIEW_CLK;
    OV5648MIPI_sensor.frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS;
    OV5648MIPI_sensor.line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS;
    OV5648MIPI_sensor.dummy_pixel = 0;
    OV5648MIPI_sensor.dummy_line = 0;
    spin_unlock(&ov5648mipi_drv_lock);

    return ERROR_NONE;
}   /*                  */


/*                                                                        
          
                     
 
             
                                    
 
            
                                     
 
         
        
 
                  
 
                                                                        */
UINT32 OV5648GetSensorID(UINT32 *sensorID) 
{
    //                           
    *sensorID=((OV5648MIPI_read_cmos_sensor(0x300A) << 8) | OV5648MIPI_read_cmos_sensor(0x300B));   

    SENSORDB("Sensor ID: 0x%x ", *sensorID);
        
    if (*sensorID != OV5648MIPI_SENSOR_ID) {
        //                                                               
        *sensorID = 0xFFFFFFFF;
        return ERROR_SENSOR_CONNECT_FAIL;
    }
 
    return ERROR_NONE;
}

/*                                                                        
          
                   
 
             
    
 
            
        
 
         
        
 
                  
 
                                                                        */
UINT32 OV5648MIPIClose(void)
{

    /*                                  */ 
    
    return ERROR_NONE;
}   /*                   */


/*                                                                        
          
                   
 
             
                                           
 
            
                                                                           
                                                                               
 
         
        
 
                  
 
                                                                        */
UINT32 OV5648MIPIPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    SENSORDB("Enter!");
    
    OV5648MIPI_Preview_Setting();

    spin_lock(&ov5648mipi_drv_lock);
    OV5648MIPI_sensor.ov5648mipi_sensor_mode = OV5648MIPI_SENSOR_MODE_PREVIEW;
    OV5648MIPI_sensor.pvPclk = OV5648MIPI_PREVIEW_CLK;
    OV5648MIPI_sensor.video_mode = KAL_FALSE;
    OV5648MIPI_sensor.line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS;
    OV5648MIPI_sensor.frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS; 
    OV5648MIPI_sensor.dummy_pixel = 0;
    OV5648MIPI_sensor.dummy_line = 0;
    spin_unlock(&ov5648mipi_drv_lock);

    SENSORDB("Exit!");
    
    return ERROR_NONE;
}   /*                      */


UINT32 OV5648MIPIVideo(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    SENSORDB("Enter!");
    
    OV5648MIPI_Preview_Setting();

    spin_lock(&ov5648mipi_drv_lock);
    OV5648MIPI_sensor.ov5648mipi_sensor_mode = OV5648MIPI_SENSOR_MODE_VIDEO;
    OV5648MIPI_sensor.videoPclk = OV5648MIPI_VIDEO_CLK;
    OV5648MIPI_sensor.video_mode = KAL_TRUE;
    OV5648MIPI_sensor.line_length = OV5648MIPI_PV_PERIOD_PIXEL_NUMS;
    OV5648MIPI_sensor.frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS;    
    OV5648MIPI_sensor.dummy_pixel = 0;
    OV5648MIPI_sensor.dummy_line = 0;    
    spin_unlock(&ov5648mipi_drv_lock);

    SENSORDB("Exit!");
    
    return ERROR_NONE;
}   /*                      */


UINT32 OV5648MIPIZSDPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    SENSORDB("Enter!");

    OV5648MIPI_Capture_Setting();

    spin_lock(&ov5648mipi_drv_lock);
    OV5648MIPI_sensor.ov5648mipi_sensor_mode = OV5648MIPI_SENSOR_MODE_CAPTURE;
    OV5648MIPI_sensor.capPclk = OV5648MIPI_CAPTURE_CLK;
    OV5648MIPI_sensor.video_mode = KAL_FALSE;
    OV5648MIPI_sensor.line_length = OV5648MIPI_FULL_PERIOD_PIXEL_NUMS;
    OV5648MIPI_sensor.frame_length = OV5648MIPI_FULL_PERIOD_LINE_NUMS;
    OV5648MIPI_sensor.dummy_pixel = 0;
    OV5648MIPI_sensor.dummy_line = 0;    
    spin_unlock(&ov5648mipi_drv_lock);

    SENSORDB("Exit!");

    return ERROR_NONE;
}   /*                         */

/*                                                                        
          
                     
 
             
                                                                 
 
            
 
         
        
 
                  
 
                                                                        */
UINT32 OV5648MIPICapture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                          MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    SENSORDB("Enter!");

    if(OV5648MIPI_SENSOR_MODE_CAPTURE != OV5648MIPI_sensor.ov5648mipi_sensor_mode)
    {
        OV5648MIPI_Capture_Setting();
    }
        
    spin_lock(&ov5648mipi_drv_lock);
    OV5648MIPI_sensor.ov5648mipi_sensor_mode = OV5648MIPI_SENSOR_MODE_CAPTURE;
    OV5648MIPI_sensor.capPclk = OV5648MIPI_CAPTURE_CLK;
    OV5648MIPI_sensor.video_mode = KAL_FALSE;    
    OV5648MIPIAutoFlickerMode = KAL_FALSE;
    OV5648MIPI_sensor.line_length = OV5648MIPI_FULL_PERIOD_PIXEL_NUMS;
    OV5648MIPI_sensor.frame_length = OV5648MIPI_FULL_PERIOD_LINE_NUMS;
    OV5648MIPI_sensor.dummy_pixel = 0;
    OV5648MIPI_sensor.dummy_line = 0;    
    spin_unlock(&ov5648mipi_drv_lock);

    SENSORDB("Exit!");
    
    return ERROR_NONE;
}   /*                      */

UINT32 OV5648MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
#ifdef OV5648MIPI_DRIVER_TRACE
    SENSORDB("Enter");
#endif

    pSensorResolution->SensorFullWidth = OV5648MIPI_IMAGE_SENSOR_FULL_WIDTH;
    pSensorResolution->SensorFullHeight = OV5648MIPI_IMAGE_SENSOR_FULL_HEIGHT;
    
    pSensorResolution->SensorPreviewWidth = OV5648MIPI_IMAGE_SENSOR_PV_WIDTH;
    pSensorResolution->SensorPreviewHeight = OV5648MIPI_IMAGE_SENSOR_PV_HEIGHT;

    pSensorResolution->SensorVideoWidth = OV5648MIPI_IMAGE_SENSOR_PV_WIDTH;
    pSensorResolution->SensorVideoHeight = OV5648MIPI_IMAGE_SENSOR_PV_HEIGHT;		
    
    return ERROR_NONE;
}   /*                           */

UINT32 OV5648MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
                      MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
                      MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
#ifdef OV5648MIPI_DRIVER_TRACE
    SENSORDB("ScenarioId = %d", ScenarioId);
#endif

    switch(ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
            pSensorInfo->SensorPreviewResolutionX = OV5648MIPI_IMAGE_SENSOR_FULL_WIDTH; /*         */
            pSensorInfo->SensorPreviewResolutionY = OV5648MIPI_IMAGE_SENSOR_FULL_HEIGHT; /*         */
            pSensorInfo->SensorCameraPreviewFrameRate = 15; /*         */
            break;

        default:
            pSensorInfo->SensorPreviewResolutionX = OV5648MIPI_IMAGE_SENSOR_PV_WIDTH; /*         */
            pSensorInfo->SensorPreviewResolutionY = OV5648MIPI_IMAGE_SENSOR_PV_HEIGHT; /*         */
            pSensorInfo->SensorCameraPreviewFrameRate = 30; /*         */
            break;
    }

    pSensorInfo->SensorFullResolutionX = OV5648MIPI_IMAGE_SENSOR_FULL_WIDTH; /*         */
    pSensorInfo->SensorFullResolutionY = OV5648MIPI_IMAGE_SENSOR_FULL_HEIGHT; /*         */

    pSensorInfo->SensorVideoFrameRate = 30; /*         */
	pSensorInfo->SensorStillCaptureFrameRate= 15; /*         */
	pSensorInfo->SensorWebCamCaptureFrameRate= 15; /*         */

    pSensorInfo->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW; /*         */
    pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW; //                       
    pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorInterruptDelayLines = 4; /*         */
    pSensorInfo->SensorResetActiveHigh = FALSE; /*         */
    pSensorInfo->SensorResetDelayCount = 5; /*         */

    pSensorInfo->SensroInterfaceType = SENSOR_INTERFACE_TYPE_MIPI;

    pSensorInfo->SensorOutputDataFormat = OV5648MIPI_COLOR_FORMAT;

    pSensorInfo->CaptureDelayFrame = 2; 
    pSensorInfo->PreviewDelayFrame = 2; 
    pSensorInfo->VideoDelayFrame = 2; 

    pSensorInfo->SensorMasterClockSwitch = 0; /*         */
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_6MA;
    
    pSensorInfo->AEShutDelayFrame = 0;          /*                                                   */
    pSensorInfo->AESensorGainDelayFrame = 0;    /*                                  */
    pSensorInfo->AEISPGainDelayFrame = 2; 

    switch (ScenarioId)
    {
	    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	    case MSDK_SCENARIO_ID_CAMERA_ZSD:
			pSensorInfo->SensorClockFreq = 24;
			pSensorInfo->SensorClockDividCount = 3; /*         */
			pSensorInfo->SensorClockRisingCount = 0;
			pSensorInfo->SensorClockFallingCount = 2; /*         */
			pSensorInfo->SensorPixelClockCount = 3; /*         */
			pSensorInfo->SensorDataLatchCount = 2; /*         */
	        pSensorInfo->SensorGrabStartX = OV5648MIPI_FULL_START_X; 
	        pSensorInfo->SensorGrabStartY = OV5648MIPI_FULL_START_Y;

            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE;         
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
            pSensorInfo->SensorWidthSampling = 0;  //                
            pSensorInfo->SensorHightSampling = 0;   //                 
            pSensorInfo->SensorPacketECCOrder = 1;

	        break;
	    default:
	        pSensorInfo->SensorClockFreq = 24;
			pSensorInfo->SensorClockDividCount = 3; /*         */
			pSensorInfo->SensorClockRisingCount = 0;
			pSensorInfo->SensorClockFallingCount = 2; /*         */
			pSensorInfo->SensorPixelClockCount = 3; /*         */
			pSensorInfo->SensorDataLatchCount = 2; /*         */
	        pSensorInfo->SensorGrabStartX = OV5648MIPI_PV_START_X; 
	        pSensorInfo->SensorGrabStartY = OV5648MIPI_PV_START_Y;

            pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE;         
            pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
            pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 14; 
            pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
            pSensorInfo->SensorWidthSampling = 0;  //                
            pSensorInfo->SensorHightSampling = 0;   //                 
            pSensorInfo->SensorPacketECCOrder = 1;

	        break;
    }
	
	return ERROR_NONE;
}   /*                     */


UINT32 OV5648MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
                      MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    SENSORDB("ScenarioId = %d", ScenarioId);

    mCurrentScenarioId =ScenarioId;
    
    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
            OV5648MIPIPreview(pImageWindow, pSensorConfigData);
            break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
            OV5648MIPIVideo(pImageWindow, pSensorConfigData);
            break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
            OV5648MIPICapture(pImageWindow, pSensorConfigData);
            break;
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
            OV5648MIPIZSDPreview(pImageWindow, pSensorConfigData);
            break;      
        default:
            SENSORDB("Error ScenarioId setting");
            return ERROR_INVALID_SCENARIO_ID;
    }

    return ERROR_NONE;
}   /*                     */



UINT32 OV5648MIPISetVideoMode(UINT16 u2FrameRate)
{
    kal_uint32 MIN_Frame_length = 0, frameRate = 0, extralines = 0;

    SENSORDB("u2FrameRate = %d ", u2FrameRate); 

    //                                           
    if(u2FrameRate < 5){
        //                   
        return ERROR_NONE;
    }

    if(OV5648MIPI_sensor.ov5648mipi_sensor_mode == OV5648MIPI_SENSOR_MODE_VIDEO)//                          
    {
    	if(OV5648MIPIAutoFlickerMode == KAL_TRUE)
    	{
    		if (u2FrameRate==30)
				frameRate= 285;
			else if(u2FrameRate==15)
				frameRate= 146;
			else
				frameRate=u2FrameRate*10;

			MIN_Frame_length = (OV5648MIPI_sensor.videoPclk)/(OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel)/frameRate*10;
    	}
		else
			MIN_Frame_length = (OV5648MIPI_sensor.videoPclk)/(OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel)/u2FrameRate;

		if((MIN_Frame_length <= OV5648MIPI_PV_PERIOD_LINE_NUMS))
		{
			MIN_Frame_length = OV5648MIPI_PV_PERIOD_LINE_NUMS;
			SENSORDB("[OV5648SetVideoMode]current fps = %d\n", (OV5648MIPI_sensor.videoPclk)  /(OV5648MIPI_PV_PERIOD_PIXEL_NUMS)/OV5648MIPI_PV_PERIOD_LINE_NUMS);
		}
		SENSORDB("[OV5648SetVideoMode]current fps (10 base)= %d\n", (OV5648MIPI_sensor.videoPclk)*10/(OV5648MIPI_PV_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel)/MIN_Frame_length);
		extralines = MIN_Frame_length - OV5648MIPI_PV_PERIOD_LINE_NUMS;
		
		spin_lock(&ov5648mipi_drv_lock);
		OV5648MIPI_sensor.dummy_pixel = 0;//                             
		OV5648MIPI_sensor.dummy_line = extralines ;
		spin_unlock(&ov5648mipi_drv_lock);
		
		OV5648MIPI_Set_Dummy(OV5648MIPI_sensor.dummy_pixel, OV5648MIPI_sensor.dummy_line);
    }
	else if(OV5648MIPI_sensor.ov5648mipi_sensor_mode == OV5648MIPI_SENSOR_MODE_CAPTURE)
	{
		SENSORDB("-------[OV5648SetVideoMode]ZSD???---------\n");
		if(OV5648MIPIAutoFlickerMode == KAL_TRUE)
    	{
    		if (u2FrameRate==15)
			    frameRate= 146;
			else
				frameRate=u2FrameRate*10;

			MIN_Frame_length = (OV5648MIPI_sensor.capPclk) /(OV5648MIPI_FULL_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel)/frameRate*10;
    	}
		else
			MIN_Frame_length = (OV5648MIPI_sensor.capPclk) /(OV5648MIPI_FULL_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel)/u2FrameRate;

		if((MIN_Frame_length <=OV5648MIPI_FULL_PERIOD_LINE_NUMS))
		{
			MIN_Frame_length = OV5648MIPI_FULL_PERIOD_LINE_NUMS;
			SENSORDB("[OV5648SetVideoMode]current fps = %d\n", (OV5648MIPI_sensor.capPclk) /(OV5648MIPI_FULL_PERIOD_PIXEL_NUMS)/OV5648MIPI_FULL_PERIOD_LINE_NUMS);

		}
		SENSORDB("[OV5648SetVideoMode]current fps (10 base)= %d\n", (OV5648MIPI_sensor.capPclk)*10/(OV5648MIPI_FULL_PERIOD_PIXEL_NUMS + OV5648MIPI_sensor.dummy_pixel)/MIN_Frame_length);

		extralines = MIN_Frame_length - OV5648MIPI_FULL_PERIOD_LINE_NUMS;

		spin_lock(&ov5648mipi_drv_lock);
		OV5648MIPI_sensor.dummy_pixel = 0;//                             
		OV5648MIPI_sensor.dummy_line= extralines ;
		spin_unlock(&ov5648mipi_drv_lock);

		OV5648MIPI_Set_Dummy(OV5648MIPI_sensor.dummy_pixel, OV5648MIPI_sensor.dummy_line);
	}
	SENSORDB("[OV5648SetVideoMode]MIN_Frame_length=%d, OV5648MIPI_sensor.dummy_line=%d\n", MIN_Frame_length, OV5648MIPI_sensor.dummy_line);

    return ERROR_NONE;
}

UINT32 OV5648MIPISetAutoFlickerMode(kal_bool bEnable, UINT16 u2FrameRate)
{
    SENSORDB("bEnable = %d, u2FrameRate = %d ", bEnable, u2FrameRate);

    if(bEnable){
        spin_lock(&ov5648mipi_drv_lock);
        OV5648MIPIAutoFlickerMode = KAL_TRUE;
        spin_unlock(&ov5648mipi_drv_lock);
        
    }else{ //                 
        spin_lock(&ov5648mipi_drv_lock);
        OV5648MIPIAutoFlickerMode = KAL_FALSE;
        spin_unlock(&ov5648mipi_drv_lock);
    }

    return ERROR_NONE;
}


UINT32 OV5648MIPISetMaxFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 frameRate) 
{
    kal_uint32 pclk;
	kal_int16 dummyLine;
	kal_uint16 lineLength,frameHeight;
  
	SENSORDB("scenarioId = %d, frame rate = %d\n", scenarioId, frameRate);

    switch (scenarioId) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			pclk = OV5648MIPI_PREVIEW_CLK;
			lineLength = OV5648MIPI_PV_PERIOD_PIXEL_NUMS;
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - OV5648MIPI_PV_PERIOD_LINE_NUMS;
			OV5648MIPI_Set_Dummy(0, dummyLine);			
			break;			
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			pclk = OV5648MIPI_VIDEO_CLK;
			lineLength = OV5648MIPI_VIDEO_PERIOD_PIXEL_NUMS;
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - OV5648MIPI_VIDEO_PERIOD_LINE_NUMS;
			OV5648MIPI_Set_Dummy(0, dummyLine);			
			break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:			
			pclk = OV5648MIPI_CAPTURE_CLK;
			lineLength = OV5648MIPI_FULL_PERIOD_PIXEL_NUMS;
			frameHeight = (10 * pclk)/frameRate/lineLength;
			dummyLine = frameHeight - OV5648MIPI_FULL_PERIOD_LINE_NUMS;
			OV5648MIPI_Set_Dummy(0, dummyLine);			
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


UINT32 OV5648MIPIGetDefaultFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 *pframeRate) 
{
	SENSORDB("scenarioId = %d \n", scenarioId);

	switch (scenarioId) {
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			 *pframeRate = 300;
			 break;
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
		case MSDK_SCENARIO_ID_CAMERA_ZSD:
			 *pframeRate = 150;
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

UINT32 OV5648MIPI_SetTestPatternMode(kal_bool bEnable)
{
    SENSORDB("bEnable: %d", bEnable);

	if(bEnable) 
	{
	    spin_lock(&ov5648mipi_drv_lock);
        OV5648MIPIDuringTestPattern = KAL_TRUE;
        spin_unlock(&ov5648mipi_drv_lock);

        //                                
        //                                                     
        OV5648MIPI_write_cmos_sensor(0x503D, 0x80);
	}
	else        
	{
	    spin_lock(&ov5648mipi_drv_lock);
        OV5648MIPIDuringTestPattern = KAL_FALSE;
        spin_unlock(&ov5648mipi_drv_lock);

        //                                
        //                                                     
        OV5648MIPI_write_cmos_sensor(0x503D, 0x00);
	}
    
    return ERROR_NONE;
}


UINT32 OV5648MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
                             UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
    UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
    UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
    UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
    UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
    //                                 
    //         
    //                                                                                      
    //                                                                                        
    MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;
    //                                                                                               
    //                                                                                            
    //                                                                                         

#ifdef OV5648MIPI_DRIVER_TRACE
    SENSORDB("FeatureId = %d", FeatureId);
#endif

    switch (FeatureId)
    {
        case SENSOR_FEATURE_GET_RESOLUTION:
            *pFeatureReturnPara16++=OV5648MIPI_IMAGE_SENSOR_FULL_WIDTH;
            *pFeatureReturnPara16=OV5648MIPI_IMAGE_SENSOR_FULL_HEIGHT;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_PERIOD:
            switch(mCurrentScenarioId)
            {
                case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
                case MSDK_SCENARIO_ID_CAMERA_ZSD:
                    *pFeatureReturnPara16++ = OV5648MIPI_sensor.line_length;
                    *pFeatureReturnPara16 = OV5648MIPI_sensor.frame_length;
                    *pFeatureParaLen=4;
                    break;
                default:
                    *pFeatureReturnPara16++ = OV5648MIPI_sensor.line_length;
                    *pFeatureReturnPara16 = OV5648MIPI_sensor.frame_length;
                    *pFeatureParaLen=4;
                    break;
            }
            break;
        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
            switch(mCurrentScenarioId)
            {
                case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
                case MSDK_SCENARIO_ID_CAMERA_ZSD: 
                    *pFeatureReturnPara32 = OV5648MIPI_CAPTURE_CLK;
                    *pFeatureParaLen=4;
                    break;
                default:
                    *pFeatureReturnPara32 = OV5648MIPI_PREVIEW_CLK;
                    *pFeatureParaLen=4;
                    break;
            }
            break;
        case SENSOR_FEATURE_SET_ESHUTTER:
            set_OV5648MIPI_shutter(*pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_NIGHTMODE:
            OV5648MIPI_night_mode((BOOL) *pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_GAIN:       
            OV5648MIPI_SetGain((UINT16) *pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_FLASHLIGHT:
            break;
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
            break;
        case SENSOR_FEATURE_SET_REGISTER:
            OV5648MIPI_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
            break;
        case SENSOR_FEATURE_GET_REGISTER:
            pSensorRegData->RegData = OV5648MIPI_read_cmos_sensor(pSensorRegData->RegAddr);
            break;
        case SENSOR_FEATURE_SET_CCT_REGISTER:
            memcpy(&OV5648MIPI_sensor.eng.cct, pFeaturePara, sizeof(OV5648MIPI_sensor.eng.cct));
            break;
            break;
        case SENSOR_FEATURE_GET_CCT_REGISTER:
            if (*pFeatureParaLen >= sizeof(OV5648MIPI_sensor.eng.cct) + sizeof(kal_uint32))
            {
              *((kal_uint32 *)pFeaturePara++) = sizeof(OV5648MIPI_sensor.eng.cct);
              memcpy(pFeaturePara, &OV5648MIPI_sensor.eng.cct, sizeof(OV5648MIPI_sensor.eng.cct));
            }
            break;
        case SENSOR_FEATURE_SET_ENG_REGISTER:
            memcpy(&OV5648MIPI_sensor.eng.reg, pFeaturePara, sizeof(OV5648MIPI_sensor.eng.reg));
            break;
        case SENSOR_FEATURE_GET_ENG_REGISTER:
            if (*pFeatureParaLen >= sizeof(OV5648MIPI_sensor.eng.reg) + sizeof(kal_uint32))
            {
              *((kal_uint32 *)pFeaturePara++) = sizeof(OV5648MIPI_sensor.eng.reg);
              memcpy(pFeaturePara, &OV5648MIPI_sensor.eng.reg, sizeof(OV5648MIPI_sensor.eng.reg));
            }
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
            ((PNVRAM_SENSOR_DATA_STRUCT)pFeaturePara)->Version = NVRAM_CAMERA_SENSOR_FILE_VERSION;
            ((PNVRAM_SENSOR_DATA_STRUCT)pFeaturePara)->SensorId = OV5648MIPI_SENSOR_ID;
            memcpy(((PNVRAM_SENSOR_DATA_STRUCT)pFeaturePara)->SensorEngReg, &OV5648MIPI_sensor.eng.reg, sizeof(OV5648MIPI_sensor.eng.reg));
            memcpy(((PNVRAM_SENSOR_DATA_STRUCT)pFeaturePara)->SensorCCTReg, &OV5648MIPI_sensor.eng.cct, sizeof(OV5648MIPI_sensor.eng.cct));
            *pFeatureParaLen = sizeof(NVRAM_SENSOR_DATA_STRUCT);
            break;
        case SENSOR_FEATURE_GET_CONFIG_PARA:
            memcpy(pFeaturePara, &OV5648MIPI_sensor.cfg_data, sizeof(OV5648MIPI_sensor.cfg_data));
            *pFeatureParaLen = sizeof(OV5648MIPI_sensor.cfg_data);
            break;
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
             OV5648MIPI_camera_para_to_sensor();
            break;
        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
            OV5648MIPI_sensor_to_camera_para();
            break;                          
        case SENSOR_FEATURE_GET_GROUP_COUNT:
            OV5648MIPI_get_sensor_group_count((kal_uint32 *)pFeaturePara);
            *pFeatureParaLen = 4;
            break;    
        case SENSOR_FEATURE_GET_GROUP_INFO:
            OV5648MIPI_get_sensor_group_info((MSDK_SENSOR_GROUP_INFO_STRUCT *)pFeaturePara);
            *pFeatureParaLen = sizeof(MSDK_SENSOR_GROUP_INFO_STRUCT);
            break;
        case SENSOR_FEATURE_GET_ITEM_INFO:
            OV5648MIPI_get_sensor_item_info((MSDK_SENSOR_ITEM_INFO_STRUCT *)pFeaturePara);
            *pFeatureParaLen = sizeof(MSDK_SENSOR_ITEM_INFO_STRUCT);
            break;
        case SENSOR_FEATURE_SET_ITEM_INFO:
            OV5648MIPI_set_sensor_item_info((MSDK_SENSOR_ITEM_INFO_STRUCT *)pFeaturePara);
            *pFeatureParaLen = sizeof(MSDK_SENSOR_ITEM_INFO_STRUCT);
            break;
        case SENSOR_FEATURE_GET_ENG_INFO:
            memcpy(pFeaturePara, &OV5648MIPI_sensor.eng_info, sizeof(OV5648MIPI_sensor.eng_info));
            *pFeatureParaLen = sizeof(OV5648MIPI_sensor.eng_info);
            break;
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
            //                                                                             
            //                                           
            *pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_SET_VIDEO_MODE:
            OV5648MIPISetVideoMode(*pFeatureData16);
            break; 
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            OV5648GetSensorID(pFeatureReturnPara32); 
            break; 
        case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
            OV5648MIPISetAutoFlickerMode((BOOL)*pFeatureData16,*(pFeatureData16+1));
            break;
		case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
			OV5648MIPISetMaxFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32, *(pFeatureData32+1));
			break;
		case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
			OV5648MIPIGetDefaultFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32, (MUINT32 *)(*(pFeatureData32+1)));
			break;
        case SENSOR_FEATURE_SET_TEST_PATTERN:
            OV5648MIPI_SetTestPatternMode((BOOL)*pFeatureData16);
            break;
        case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE://                                          
            *pFeatureReturnPara32= OV5648MIPI_TEST_PATTERN_CHECKSUM;
            *pFeatureParaLen=4;                             
             break;            
        default:
            break;
    }
  
    return ERROR_NONE;
}   /*                              */
SENSOR_FUNCTION_STRUCT  SensorFuncOV5648MIPI=
{
    OV5648MIPIOpen,
    OV5648MIPIGetInfo,
    OV5648MIPIGetResolution,
    OV5648MIPIFeatureControl,
    OV5648MIPIControl,
    OV5648MIPIClose
};

UINT32 OV5648MIPISensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
    /*                                  */
    if (pfFunc!=NULL)
        *pfFunc=&SensorFuncOV5648MIPI;

    return ERROR_NONE;
}   /*                        */
