/*                                                                            
  
            
            
             
  
           
           
         
  
               
               
                                 
  
  
          
          
                        
  
                                                                              
                      
                                                                     
                                                                                
              
             
         
  
                      
                                           
    
  
  
                                                                                
                                                                     
                                                                              
                                                                            */

#if !defined(MTK_NATIVE_3D_SUPPORT) //  
  #define S5K4ECGX_MIPIYUV_MAIN_2_USE_HW_I2C
#else //                     
  #define S5K4ECGX_MIPIYUV_MAIN_2_USE_HW_I2C

  #ifdef S5K4ECGX_MIPI_MAIN_2_USE_HW_I2C
    #define S5K4ECGX_MIPI_SUPPORT_N3D
  #endif
#endif

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
//      
//                       
#include <linux/xlog.h>
#include <asm/atomic.h>
//                                         
//                             
//                            
#include <asm/io.h>
//                       

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"


#include "s5k4ecgxmipi_yuv_Sensor.h"
#include "s5k4ecgxmipi_yuv_Camera_Sensor_para.h"
#include "s5k4ecgxmipi_yuv_CameraCustomized.h"

#define PRE_CLK 80
#define S5K4ECGX_MIPI_DEBUG
#ifdef S5K4ECGX_MIPI_DEBUG
#define LOG_TAG "[4EC]"
#define SENSORDB(fmt, arg...)    xlog_printk(ANDROID_LOG_DEBUG , LOG_TAG, fmt, ##arg)
//                                                          
//                                                                               
#else
#define SENSORDB(x,...)
#endif
#define Sleep(ms) msleep(ms)



#define S5K4ECGX_MIPI_AF_Enable 1

#define S5K4ECGX_TEST_PATTERN_CHECKSUM (0x65fdd80e)

//                         
static DEFINE_SPINLOCK(s5k4ecgx_mipi_drv_lock);

static DEFINE_SPINLOCK(s5k4ecgx_mipi_rw_lock);


static kal_uint32  S5K4ECGX_MIPI_sensor_pclk = 390 * 1000000;
MSDK_SCENARIO_ID_ENUM S5K4ECGXCurrentScenarioId = MSDK_SCENARIO_ID_CAMERA_PREVIEW;
MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT S5K4ECGX_PreviewWin[5] = {0};

//       
static MSDK_SENSOR_CONFIG_STRUCT S5K4ECGXSensorConfigData;
//
kal_uint8 S5K4ECGX_MIPI_sensor_write_I2C_address = S5K4ECGX_WRITE_ID;
kal_uint8 S5K4ECGX_MIPI_sensor_read_I2C_address = S5K4ECGX_READ_ID;
kal_uint8 S5K4ECGX_MIPI_sensor_socket = DUAL_CAMERA_NONE_SENSOR;
struct S5K4ECGX_MIPI_sensor_struct S5K4ECGX_Driver;
unsigned int S5K4ECGX_Preview_enabled = 0;
unsigned int s5k4ec_cap_enable = 0;
UINT32 S5K4ECGX_MIPI_GetSensorID(UINT32 *sensorID);

/*
                                                   
 
                                                           
                                                           
                                                           
                                                          
  
*/

unsigned short S5K4ECGX_MIPI_DEFAULT_AE_TABLE[32] =
{
    0x101, 0x101, 0x101, 0x101, 0x101, 0x201, 0x102, 0x101,
    0x101, 0x202, 0x202, 0x101, 0x101, 0x502, 0x205, 0x101,
    0x201, 0x504, 0x405, 0x102, 0x402, 0x505, 0x505, 0x204,
    0x403, 0x505, 0x505, 0x304, 0x302, 0x303, 0x303, 0x203
};



extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
extern int iBurstWriteReg(u8 *pData, u32 bytes, u16 i2cId);
extern int iMultiWriteReg(u8 *pData, u16 lens, u16 i2cId);

static kal_uint16 S5K4ECGX_write_cmos_sensor_wID(kal_uint32 addr, kal_uint32 para, kal_uint32 id)
{
   char puSendCmd[4] = {(char)(addr >> 8) , (char)(addr & 0xFF) ,(char)(para >> 8),(char)(para & 0xFF)};
   iWriteRegI2C(puSendCmd , 4,id);
   //                                                                      
}

static kal_uint16 S5K4ECGX_read_cmos_sensor_wID(kal_uint32 addr, kal_uint32 id)
{
   kal_uint16 get_byte=0;
   char puSendCmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
   iReadRegI2C(puSendCmd , 2, (u8*)&get_byte,2,id);
   return ((get_byte<<8)&0xff00)|((get_byte>>8)&0x00ff);
}

static kal_uint16 S5K4ECGX_read_cmos_sensor(kal_uint32 addr)
{
   kal_uint16 get_byte=0;
   char puSendCmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
   iReadRegI2C(puSendCmd , 2, (u8*)&get_byte, 2, S5K4ECGX_MIPI_sensor_write_I2C_address);
   return ((get_byte<<8)&0xff00)|((get_byte>>8)&0x00ff);
}


static kal_uint16 S5K4ECGX_write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
   char puSendCmd[4] = {(char)(addr >> 8) , (char)(addr & 0xFF) ,(char)(para >> 8),(char)(para & 0xFF)};
   iWriteRegI2C(puSendCmd , 4,S5K4ECGX_MIPI_sensor_write_I2C_address);
   //                                                                                                          
}


//                           
#ifdef USE_I2C_BURST_WRITE
#define I2C_BUFFER_LEN 254 //                                                    
#define BLOCK_I2C_DATA_WRITE iBurstWriteReg
#else
#define I2C_BUFFER_LEN 8   //                                             
#define BLOCK_I2C_DATA_WRITE iWriteRegI2C
#endif

//                          
//                                     
//                                                          
static kal_uint16 S5K4ECGX_table_write_cmos_sensor(kal_uint16* para, kal_uint32 len)
{
   char puSendCmd[I2C_BUFFER_LEN]; //                                                                                                 
   kal_uint32 tosend, IDX;
   kal_uint16 addr, addr_last, data;

   tosend = 0;
   IDX = 0;
   while(IDX < len)
   {
       addr = para[IDX];

       if (tosend == 0) //                         
       {
           puSendCmd[tosend++] = (char)(addr >> 8);
           puSendCmd[tosend++] = (char)(addr & 0xFF);
           data = para[IDX+1];
           puSendCmd[tosend++] = (char)(data >> 8);
           puSendCmd[tosend++] = (char)(data & 0xFF);
           IDX += 2;
           addr_last = addr;
       }
       else if (addr == addr_last) //                                               
       {
           data = para[IDX+1];
           puSendCmd[tosend++] = (char)(data >> 8);
           puSendCmd[tosend++] = (char)(data & 0xFF);
           IDX += 2;
       }
       //                                                                                                    
       if (tosend == I2C_BUFFER_LEN || IDX == len || addr != addr_last)
       {
           BLOCK_I2C_DATA_WRITE(puSendCmd , tosend, S5K4ECGX_MIPI_sensor_write_I2C_address);
           tosend = 0;
       }
   }
   return 0;
}

int SEN_RUN_TEST_PATTERN = 0;

unsigned int
S5K4ECGX_MIPI_GetExposureTime(void)
{
    unsigned int interval = 0;

    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E, 0x2C28);
    interval  = S5K4ECGX_read_cmos_sensor(0x0F12);
    interval += S5K4ECGX_read_cmos_sensor(0x0F12) << 16 ;
    interval /= 400; //  
    //              
    if (interval < 30)
    {
       interval = 30;
    }


    if (interval > 500)
    {
       interval = 500;
    }

    SENSORDB("[4EC] FrameTime = %d ms\n", interval);
    return interval;
}



UINT32 S5K4ECGX_MIPI_SetTestPatternMode(kal_bool bEnable)
{
    //                                                                                                    
    //                
    S5K4ECGX_write_cmos_sensor(0x0028,0xd000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x3100);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0002);
    S5K4ECGX_write_cmos_sensor(0xFCFC,0xd000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    return ERROR_NONE;
}


/*                                                          
                      
                                                          */
void S5K4ECGX_MIPI_set_scene_mode(UINT16 para);
BOOL S5K4ECGX_MIPI_set_param_wb(UINT16 para);



static void
S5K4ECGX_MIPI_AE_Lock(void)
{
    //                           
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x2C5E);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);
    return;
}


static void
S5K4ECGX_MIPI_AE_UnLock(void)
{
    //                             
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x2C5E);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0001);
    return;
}

static void
S5K4ECGX_MIPI_AE_On(void)
{

    kal_uint16 Status_3A=0;

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x04E6);
    Status_3A = S5K4ECGX_read_cmos_sensor(0x0F12);


    //          
    Status_3A = Status_3A | 0x00000002;
    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002a, 0x04e6);//
    S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);
    return;
}

static void
S5K4ECGX_MIPI_AE_Off(void)
{
    kal_uint16 Status_3A=0;

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x04E6);
    Status_3A = S5K4ECGX_read_cmos_sensor(0x0F12);


    //           
    Status_3A = Status_3A & ~(0x00000002);
    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002a, 0x04e6);//
    S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);
    return;
}



static void
S5K4ECGX_MIPI_AWB_Lock(void)
{
    //                            
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x2C66);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);
    return;
}


static void
S5K4ECGX_MIPI_AWB_UnLock(void)
{
#if 0
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x2C66);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0001);
    if (!((SCENE_MODE_OFF == S5K4ECGX_Driver.sceneMode) || (SCENE_MODE_NORMAL ==
    S5K4ECGX_Driver.sceneMode) || (SCENE_MODE_HDR == S5K4ECGX_Driver.sceneMode)))
    {
       S5K4ECGX_MIPI_set_scene_mode(S5K4ECGX_Driver.sceneMode);
    }
    else
    {
        if (!((AWB_MODE_OFF == S5K4ECYX_MIPICurrentStatus.iWB) ||
            (AWB_MODE_AUTO == S5K4ECYX_MIPICurrentStatus.iWB)))
        {
            S5K4ECGX_MIPI_set_param_wb(S5K4ECYX_MIPICurrentStatus.iWB);
        }
    }
    if (!((AWB_MODE_OFF == S5K4ECYX_MIPICurrentStatus.iWB) ||
        (AWB_MODE_AUTO == S5K4ECYX_MIPICurrentStatus.iWB)))
    {
        S5K4ECGX_MIPI_set_param_wb(S5K4ECYX_MIPICurrentStatus.iWB);
    }
#endif
    if (((AWB_MODE_OFF == S5K4ECYX_MIPICurrentStatus.iWB) ||
        (AWB_MODE_AUTO == S5K4ECYX_MIPICurrentStatus.iWB)))
    {
        S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
        S5K4ECGX_write_cmos_sensor(0x002A,0x2C66);
        S5K4ECGX_write_cmos_sensor(0x0F12,0x0001);
    }

    return;
}

static void
S5K4ECGX_MIPI_AWB_On(void)
{

    kal_uint16 Status_3A=0;

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x04E6);
    Status_3A = S5K4ECGX_read_cmos_sensor(0x0F12);


    //           
    Status_3A = Status_3A | 0x00000008;
    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002a, 0x04e6);//
    S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);
    return;
}

static void
S5K4ECGX_MIPI_AWB_Off(void)
{
    kal_uint16 Status_3A=0;

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x04E6);
    Status_3A = S5K4ECGX_read_cmos_sensor(0x0F12);


    //            
    Status_3A = Status_3A & ~(0x00000008);
    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002a, 0x04e6);//
    S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);
    return;
}



static void
S5K4ECGX_MIPI_AE_ExpCurveGain_Config(unsigned int SceneMode)
{

    switch (SceneMode)
    {
       case SCENE_MODE_NIGHTSCENE:
           //        
           S5K4ECGX_write_cmos_sensor(0x002A, 0x0638);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x1478);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A0A);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x6810);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x6810);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0xD020);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0428);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A80);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0006); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A80);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0006); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A80);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0006); //                                            
           break;
       case SCENE_MODE_SPORTS:
           S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A, 0x0638);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A3C);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0D05);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                            
           break;
       default:
           S5K4ECGX_write_cmos_sensor(0x002A, 0x0638);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                                                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A3C);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0D05);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x6810);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x8214);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0xC350);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0xD4C0);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0xD4C0);   //    
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //    
           break;
    }
    return;
}




void S5K4ECGX_MIPI_AE_Rollback_Weight_Table(void)
{
    unsigned short *pae_table = 0;
    unsigned int i;

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    if (S5K4ECGX_Driver.userAskAeLock)
    {
        spin_unlock(&s5k4ecgx_mipi_drv_lock);
        return;
    }
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    //                                            

    pae_table = (unsigned short *) (&S5K4ECGX_MIPI_DEFAULT_AE_TABLE[0]);

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x1492);//      
    for (i = 0; i < 32; i++)
    {
       S5K4ECGX_write_cmos_sensor(0x0F12, pae_table[i]);
    }

    S5K4ECGX_write_cmos_sensor(0x0028 ,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A ,0x0268);
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0001);  //                           

    SENSORDB("[4EC]Rollback_Weight_Table\n");

    S5K4ECGX_write_cmos_sensor(0x0028 ,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A ,0x0268);
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0001);  //                           

    return;
}


static void
S5K4ECGX_MIPI_AE_Dump_WeightTbl(void)
{
    //                    
    unsigned int val[32];
    unsigned int offset = 0, i;
    unsigned short *pae_table = 0;

    pae_table = (unsigned short *) (&S5K4ECGX_Driver.ae_table[0]);
    for (offset = 0; offset < 32; offset+=8)
    {
        SENSORDB("[4EC] AETbl SW_Tbl[%d~%d]: %4x, %4x, %4x, %4x\n", offset, offset+3,
        pae_table[offset+0], pae_table[offset+1], pae_table[offset+2], pae_table[offset+3]);
        SENSORDB("[4EC] AETbl SW_Tbl[%d~%d]: %4x, %4x, %4x, %4x\n", offset+4, offset+7,
        pae_table[offset+4], pae_table[offset+5], pae_table[offset+6], pae_table[offset+7]);
    }

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x1492); //      
    for (i = 0; i < 32; i++)
    {
        val[i] = S5K4ECGX_read_cmos_sensor(0x0F12);
    }

    for (offset = 0; offset < 32; offset+=8)
    {
        SENSORDB("[4EC] AETbl HW_RdBack[%d~%d]: %x, %x, %x, %x\n", offset, offset+3,
        val[offset+0], val[offset+1], val[offset+2], val[offset+3]);
        SENSORDB("[4EC] AETbl HW_RdBack[%d~%d]: %x, %x, %x, %x\n", offset+4, offset+7,
        val[offset+4], val[offset+5], val[offset+6], val[offset+7]);
    }
    return;
}



unsigned short
S5K4ECGX_MIPI_AE_Get_WeightVal(unsigned int x, unsigned int y)
{
    unsigned short val = 0;
    //                    
    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E ,0x2E08 + (y * 8) + (x & 0xFFFE));
    val = S5K4ECGX_read_cmos_sensor(0x0F12);

    return val;
}


static void
S5K4ECGX_MIPI_AE_Set_Window2HW(void)
{
    //                            
    //                            
    //                           
    unsigned short *pae_table = 0;
    unsigned int i;
    //                          


    //                                                                                   

    pae_table = (unsigned short *) (&S5K4ECGX_Driver.ae_table[0]);
    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);

#if 0
    //                          
    S5K4ECGX_write_cmos_sensor(0x002A,0x0588);//      
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);
#endif

    //                         
    S5K4ECGX_write_cmos_sensor(0x002A,0x1492);//      
    for (i = 0; i < 32; i++)
    {
       S5K4ECGX_write_cmos_sensor(0x0F12, pae_table[i]);
    }

    //            
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x0268);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0001);  //                           

#if 0
    //                                                         
    exposureTime = S5K4ECGX_MIPI_GetExposureTime();
    Sleep(exposureTime);

    //                     
    i = 30;
    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x2E08); //      
    while (i--)
    {
        if (S5K4ECGX_read_cmos_sensor(0x0F12))
        {
            break;
        }
        else
        {
            Sleep(3);
        }
    }

    //                                       
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x0588);//      
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0002);
#endif

    //                                    
    //                                  
    return;
}



#define S5K4ECGX_MIPI_AE_MAX_WEIGHT_VAL 0xF
static S5K4ECGX_AAA_STATUS_ENUM
S5K4ECGX_MIPI_AE_Set_Window(
    uintptr_t zone_addr,
    unsigned int prevW,
    unsigned int prevH)
{
    unsigned int x0, y0, x1, y1, width, height, i, j, xx, yy;
    unsigned int* ptr = (unsigned int*)zone_addr;
    unsigned int srcW_maxW; //                         
    unsigned int srcW_maxH; //                          
    unsigned char ae_table[8][8] = {0};
    unsigned int  stepX, stepY;
    unsigned char aeStateOnOriginalSet;
    x0 = *ptr       ;
    y0 = *(ptr + 1) ;
    x1 = *(ptr + 2) ;
    y1 = *(ptr + 3) ;
    width = *(ptr + 4);
    height = *(ptr + 5);
    srcW_maxW = width;
    srcW_maxH = height;

    SENSORDB("[4EC] AE_Set_Window 3AWin: (%d,%d)~(%d,%d)\n",x0, y0, x1, y1);

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    aeStateOnOriginalSet = 0;
    if ((x0 == x1) && (y0 == y1))
    {
        aeStateOnOriginalSet = 1;
        memcpy((unsigned char*)&ae_table[0][0], (unsigned char*)&S5K4ECGX_MIPI_DEFAULT_AE_TABLE[0], 64);
        SENSORDB("[4EC] AE_Set_Window aeStateOnOriginalSet = 1\n");
    }
    else
    {
        //                                                           
    }
    S5K4ECGX_Driver.apAEWindows[0] = x0;
    S5K4ECGX_Driver.apAEWindows[1] = y0;
    S5K4ECGX_Driver.apAEWindows[2] = x1;
    S5K4ECGX_Driver.apAEWindows[3] = y1;
    S5K4ECGX_Driver.apAEWindows[5] = width;
    S5K4ECGX_Driver.apAEWindows[6] = height;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);


    if (width == 0)
    {
        width = 320;
    }

    if (height == 0)
    {
        height = 240;
    }

    if (width > prevW)
    {
        width = prevW;
    }

    if (height > prevH)
    {
        height = prevH;
    }


    if (x0 >= srcW_maxW)
    {
        x0 = srcW_maxW - 1;
    }

    if (x1 >= srcW_maxW)
    {
        x1 = srcW_maxW - 1;
    }

    if (y0 >= srcW_maxH)
    {
        y0 = srcW_maxH - 1;
    }

    if (y1 >= srcW_maxH)
    {
        y1 = srcW_maxH - 1;
    }


    srcW_maxW = width;
    srcW_maxH = height;


    x0 = x0 * (prevW / srcW_maxW);
    y0 = y0 * (prevH / srcW_maxH);
    x1 = x1 * (prevW / srcW_maxW);
    y1 = y1 * (prevH / srcW_maxH);


    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.aeWindows[0] = x0;
    S5K4ECGX_Driver.aeWindows[1] = y0;
    S5K4ECGX_Driver.aeWindows[2] = x1;
    S5K4ECGX_Driver.aeWindows[3] = y1;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    x0 = (x0 + x1) / 2;
    y0 = (y0 + y1) / 2;
    stepX = prevW / 8;
    stepY = prevH / 8;

    if (stepX == 0)
    {
       stepX = 1;
    }
    if (stepY == 0)
    {
       stepY = 1;
    }

    x1 = x0 / stepX;
    y1 = y0 / stepY;
    if (x1 >= 8) x1 = 7;
    if (y1 >= 8) y1 = 7;


    SENSORDB("[4EC] AE_Set_Window 3AMapPnt: (%d,%d)\n", x1, y1);
    if (0 == aeStateOnOriginalSet)
    {
        memset(ae_table, 0x0, sizeof(ae_table));
        if (y1 != 0)
        {
            if (x1 != 0)
            {
                 for (i = y1 - 1; i <= y1 + 1; i++)
                 {
                     yy = (i > 7) ? 7:i;
                     for (j = x1 - 1; j <= x1 + 1; j++)
                     {
                         xx = (j > 7) ? 7:j;
                         ae_table[yy][xx] = 2;
                     }
                 }
            }
            else
            {
                for (i = y1 - 1; i <= y1 + 1; i++)
                {
                    yy = (i > 7) ? 7:i;
                    for (j = x1; j <= x1 + 2; j++)
                    {
                        xx = (j > 7) ? 7:j;
                        ae_table[yy][xx] = 2;
                    }
                }
            }
        }
        else
        {
            if (x1 != 0)
            {
                 for (i = y1; i <= y1 + 2; i++)
                 {
                     yy = (i > 7) ? 7:i;
                     for (j = x1 - 1; j <= x1 + 1; j++)
                     {
                         xx = (j > 7) ? 7:j;
                         ae_table[yy][xx] = 2;
                     }
                 }
            }
            else
            {
                for (i = y1; i <= y1 + 2; i++)
                {
                    yy = (i > 7) ? 7:i;
                    for (j = x1; j <= x1 + 2; j++)
                    {
                        xx = (j > 7) ? 7:j;
                        ae_table[yy][xx] = 2;
                    }
                }
            }
        }

        ae_table[y1][x1] = S5K4ECGX_MIPI_AE_MAX_WEIGHT_VAL;
   }


    spin_lock(&s5k4ecgx_mipi_drv_lock);
    if ((x1 == S5K4ECGX_Driver.mapAEWindows[0]) &&
        (y1 == S5K4ECGX_Driver.mapAEWindows[1]))
    {
        if (!(memcmp(&S5K4ECGX_Driver.ae_table[0], &ae_table[0][0], 64)))
        {
           //                    
           spin_unlock(&s5k4ecgx_mipi_drv_lock);
           SENSORDB("[4EC] AE_Set_Window: NewSet is the same as PrevSet.....\n");
           S5K4ECGX_MIPI_AE_Dump_WeightTbl();
           return S5K4ECGX_AAA_AF_STATUS_OK;
        }
    }
    S5K4ECGX_Driver.mapAEWindows[0] = x1;
    S5K4ECGX_Driver.mapAEWindows[1] = y1;
    memcpy(&S5K4ECGX_Driver.ae_table[0], &(ae_table[0][0]), sizeof(ae_table));
    spin_unlock(&s5k4ecgx_mipi_drv_lock);


    S5K4ECGX_MIPI_AE_Set_Window2HW();
    S5K4ECGX_MIPI_AE_Dump_WeightTbl();
    //                                                                                       
    return S5K4ECGX_AAA_AF_STATUS_OK;
}




static void
S5K4ECGX_MIPI_AF_Get_Max_Num_Focus_Areas(unsigned int *pFeatureReturnPara32)
{
    *pFeatureReturnPara32 = 1;
    //                                                                          
}

static void
S5K4ECGX_MIPI_AE_Get_Max_Num_Metering_Areas(unsigned int *pFeatureReturnPara32)
{
    *pFeatureReturnPara32 = 1;
    //                                                                                                             
}

static void
S5K4ECGX_MIPI_AF_Get_Inf(unsigned int *pFeatureReturnPara32)
{
    *pFeatureReturnPara32 = 0;
}

static void
S5K4ECGX_MIPI_AF_Get_Macro(unsigned int *pFeatureReturnPara32)
{
    *pFeatureReturnPara32 = 0;//    
}



static void
S5K4ECGX_MIPI_AF_rollbackWinSet(void)
{
    S5K4ECGX_MIPI_AF_WIN_T  *AfWindows;

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    AfWindows = &S5K4ECGX_Driver.orignalAfWindows;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x029C);
    S5K4ECGX_write_cmos_sensor(0x0F12,AfWindows->inWx);
    S5K4ECGX_write_cmos_sensor(0x002A,0x029E);
    S5K4ECGX_write_cmos_sensor(0x0F12,AfWindows->inWy);

    S5K4ECGX_write_cmos_sensor(0x002A,0x0294);
    S5K4ECGX_write_cmos_sensor(0x0F12,AfWindows->outWx);
    S5K4ECGX_write_cmos_sensor(0x002A,0x0296);
    S5K4ECGX_write_cmos_sensor(0x0F12,AfWindows->outWy);

    //                
    S5K4ECGX_write_cmos_sensor(0x002A,0x02A4);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0001);
    return;
}



S5K4ECGX_AAA_STATUS_ENUM
S5K4ECGX_MIPI_AF_Init(void)
{
    unsigned int backupAFWindDone = 1;

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    //                           
    if (S5K4ECGX_AF_STATE_UNINIT == S5K4ECGX_Driver.afState)
    {
        S5K4ECGX_Driver.afState = S5K4ECGX_AF_STATE_IDLE;
        backupAFWindDone = 0;
    }
    S5K4ECGX_Driver.afMode = S5K4ECGX_AF_MODE_RSVD;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    if (!backupAFWindDone)
    {
        S5K4ECGX_MIPI_AF_WIN_T  *AfWindows;

        spin_lock(&s5k4ecgx_mipi_drv_lock);
        AfWindows = &S5K4ECGX_Driver.orignalAfWindows;
        spin_unlock(&s5k4ecgx_mipi_drv_lock);

        S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
        S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
        S5K4ECGX_write_cmos_sensor(0x002E,0x029C);
        AfWindows->inWx = S5K4ECGX_read_cmos_sensor(0x0F12);
        AfWindows->inWy = S5K4ECGX_read_cmos_sensor(0x0F12);

        S5K4ECGX_write_cmos_sensor(0x002E,0x0294);
        AfWindows->outWx = S5K4ECGX_read_cmos_sensor(0x0F12);
        AfWindows->outWy = S5K4ECGX_read_cmos_sensor(0x0F12);

    }

    return S5K4ECGX_AAA_AF_STATUS_OK;
}




S5K4ECGX_AAA_STATUS_ENUM
S5K4ECGX_MIPI_AF_Set_Window2HW(void)
{
    unsigned int inWx1, inWy1, inWw1, inWh1; //                    
    unsigned int outWx1, outWy1, outWw1, outWh1; //                    

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    if (S5K4ECGX_Driver.afStateOnOriginalSet)
    {
        spin_unlock(&s5k4ecgx_mipi_drv_lock);
        return S5K4ECGX_AAA_AF_STATUS_OK;
    }

    inWx1  = S5K4ECGX_Driver.afWindows.inWx;
    inWy1  = S5K4ECGX_Driver.afWindows.inWy;
    inWw1  = S5K4ECGX_Driver.afWindows.inWw;
    inWh1  = S5K4ECGX_Driver.afWindows.inWh;
    outWx1 = S5K4ECGX_Driver.afWindows.outWx;
    outWy1 = S5K4ECGX_Driver.afWindows.outWy;
    outWw1 = S5K4ECGX_Driver.afWindows.outWw;
    outWh1 = S5K4ECGX_Driver.afWindows.outWh;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);


    //                                                                                             
    //                                          
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x029C);
    S5K4ECGX_write_cmos_sensor(0x0F12,inWx1);
    S5K4ECGX_write_cmos_sensor(0x002A,0x029E);
    S5K4ECGX_write_cmos_sensor(0x0F12,inWy1);

    S5K4ECGX_write_cmos_sensor(0x002A,0x0294);
    S5K4ECGX_write_cmos_sensor(0x0F12,outWx1);
    S5K4ECGX_write_cmos_sensor(0x002A,0x0296);
    S5K4ECGX_write_cmos_sensor(0x0F12,outWy1);

    //                                               
    //                
    S5K4ECGX_write_cmos_sensor(0x002A,0x02A4);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0001);

    return S5K4ECGX_AAA_AF_STATUS_OK;
}





unsigned int orig_inWw = 0;
unsigned int orig_inWh = 0;
unsigned int orig_outWw = 0;
unsigned int orig_outWh = 0;
#define S5K4ECGX_FAF_TOLERANCE    100
#define S5K4ECGX_AF_MIX_ACCURACY  0x200000 //                         
static S5K4ECGX_AAA_STATUS_ENUM
S5K4ECGX_MIPI_AF_Set_Window(
    uintptr_t zone_addr,
    unsigned int prevW,
    unsigned int prevH)
{
    unsigned int x0, y0, x1, y1, FD_XS, FD_YS;
    unsigned int* ptr = (unsigned int*)zone_addr;
    unsigned int srcW_maxW = S5K4ECGX_MIPI_AF_CALLER_WINDOW_WIDTH;
    unsigned int srcW_maxH = S5K4ECGX_MIPI_AF_CALLER_WINDOW_HEIGHT;
    unsigned int af_win_idx = 1, frameTime;
    unsigned int af_resolution = 0;

    x0 = *ptr       ;
    y0 = *(ptr + 1) ;
    x1 = *(ptr + 2) ;
    y1 = *(ptr + 3) ;
    FD_XS = *(ptr + 4);
    FD_YS = *(ptr + 5);
    if (FD_XS == 0)
    {
        FD_XS = 320;
    }

    if (FD_YS == 0)
    {
        FD_YS = 240;
    }

    if (FD_XS > prevW)
    {
        FD_XS = prevW;
    }

    if (FD_YS > prevH)
    {
        FD_YS = prevH;
    }


    SENSORDB("[4EC] AF_Set_Window AP's setting: (%d,%d)~(%d,%d).size:(%d,%d)\n",x0, y0, x1, y1, FD_XS, FD_YS);

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.afStateOnOriginalSet = 0;
    if ((x0 == x1) && (y0 == y1))
    {
        S5K4ECGX_Driver.afStateOnOriginalSet = 1;
    }
    spin_unlock(&s5k4ecgx_mipi_drv_lock);


    srcW_maxW = FD_XS;
    srcW_maxH = FD_YS;

    if (x0 >= srcW_maxW)
    {
        x0 = srcW_maxW - 1;
    }

    if (x1 >= srcW_maxW)
    {
        x1 = srcW_maxW - 1;
    }

    if (y0 >= srcW_maxH)
    {
        y0 = srcW_maxH - 1;
    }

    if (y1 >= srcW_maxH)
    {
        y1 = srcW_maxH - 1;
    }

    x0 = (x0 + x1) / 2;
    y0 = (y0 + y1) / 2;

    //                                             
    x0 = x0 * (prevW / srcW_maxW);
    y0 = y0 * (prevH / srcW_maxH);


    {
        unsigned int inWw0, inWh0, outWw0, outWh0;
        unsigned int inWx1, inWy1, inWw1, inWh1;     //                    
        unsigned int outWx1, outWy1, outWw1, outWh1; //                    

        if ((orig_inWw == 0) || (orig_inWh == 0))
        {
            //                                   

            S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
            S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
            S5K4ECGX_write_cmos_sensor(0x002E,0x02A0);
            inWw0 = S5K4ECGX_read_cmos_sensor(0x0F12);
            orig_inWw = inWw0 * prevW / 1024;

            S5K4ECGX_write_cmos_sensor(0x002E,0x02A2);
            inWh0 = S5K4ECGX_read_cmos_sensor(0x0F12);
            orig_inWh = inWh0 * prevH / 1024;

            S5K4ECGX_write_cmos_sensor(0x002E,0x0298);
            outWw0 = S5K4ECGX_read_cmos_sensor(0x0F12);
            orig_outWw = outWw0 * prevW / 1024;

            S5K4ECGX_write_cmos_sensor(0x002E,0x029A);
            outWh0 = S5K4ECGX_read_cmos_sensor(0x0F12);
            orig_outWh = outWh0 * prevH / 1024;
        }

        inWw1 = orig_inWw;
        inWh1 = orig_inWh;
        outWw1 = orig_outWw;
        outWh1 = orig_outWh;


        //          
        if (x0 <= (inWw1/2))
        {
            inWx1 = 0;
            outWx1 = 0;
        }
        else if (x0 <= (outWw1 / 2))
        {
            inWx1 = x0 - (inWw1 / 2);
            outWx1 = 0;
        }
        else if (x0 >= ((prevW-1) - (inWw1 / 2)))
        {
            inWx1  = (prevW-1) - inWw1;
            outWx1 = (prevW-1) - outWw1;
        }
        else if (x0 >= ((prevW-1) - (outWw1 / 2)))
        {
            inWx1  = x0 - (inWw1/2);
            outWx1 = (prevW-1) - outWw1;
        }
        else
        {
            inWx1  = x0 - (inWw1/2);
            outWx1 = x0 - (outWw1/2);
        }


        //          
        if (y0 <= (inWh1/2))
        {
            inWy1 = 0;
            outWy1 = 0;
        }
        else if (y0 <= (outWh1/2))
        {
            inWy1 = y0 - (inWh1/2);
            outWy1 = 0;
        }
        else if (y0 >= ((prevH-1) - (inWh1/2)))
        {
            inWy1  = (prevH-1) - inWh1;
            outWy1 = (prevH-1) - outWh1;
        }
        else if (y0 >= ((prevH-1) - (outWh1/2)))
        {
            inWy1  = y0 - (inWh1/2);
            outWy1 = (prevH-1) - outWh1;
        }
        else
        {
            inWy1  = y0 - (inWh1/2);
            outWy1 = y0 - (outWh1/2);
        }

        inWx1  =  inWx1 * 1024 / (prevW);
        inWy1  =  inWy1 * 1024 / (prevH);
        outWx1 = outWx1 * 1024 / (prevW);
        outWy1 = outWy1 * 1024 / (prevH);


         //       
        spin_lock(&s5k4ecgx_mipi_drv_lock);
        //                                                                                    
        //                                                   
        if (!((inWx1 > (S5K4ECGX_Driver.afWindows.inWx + S5K4ECGX_FAF_TOLERANCE)) ||
            ((inWx1 + S5K4ECGX_FAF_TOLERANCE) < S5K4ECGX_Driver.afWindows.inWx) ||
            (inWy1 > (S5K4ECGX_Driver.afWindows.inWy + S5K4ECGX_FAF_TOLERANCE)) ||
            ((inWy1 + S5K4ECGX_FAF_TOLERANCE) < S5K4ECGX_Driver.afWindows.inWy) ||

            (outWx1 > (S5K4ECGX_Driver.afWindows.outWx + S5K4ECGX_FAF_TOLERANCE)) ||
            ((outWx1 + S5K4ECGX_FAF_TOLERANCE) < S5K4ECGX_Driver.afWindows.outWx) ||
            (outWy1 > (S5K4ECGX_Driver.afWindows.outWy + S5K4ECGX_FAF_TOLERANCE)) ||
            ((outWy1 + S5K4ECGX_FAF_TOLERANCE) < S5K4ECGX_Driver.afWindows.outWy)))
        {
             //                                                          
             //                
             spin_unlock(&s5k4ecgx_mipi_drv_lock);

             //                                          
             //                                          
             //                                                   
             //                                                            
             //                                                                
             //                                             
            {
                SENSORDB("[4EC] AF window is very near the previous'.......\n");
                return S5K4ECGX_AAA_AF_STATUS_OK;
            }
        }
        else
        {
            spin_unlock(&s5k4ecgx_mipi_drv_lock);
        }

        spin_lock(&s5k4ecgx_mipi_drv_lock);
        S5K4ECGX_Driver.afWindows.inWx = inWx1;
        S5K4ECGX_Driver.afWindows.inWy = inWy1;
        S5K4ECGX_Driver.afWindows.inWw = inWw1;
        S5K4ECGX_Driver.afWindows.inWh = inWh1;
        S5K4ECGX_Driver.afWindows.outWx = outWx1;
        S5K4ECGX_Driver.afWindows.outWy = outWy1;
        S5K4ECGX_Driver.afWindows.outWw = outWw1;
        S5K4ECGX_Driver.afWindows.outWh = outWh1;
        spin_unlock(&s5k4ecgx_mipi_drv_lock);

    }

    /*                                                                                       
                                                                   
                                                                   
                                                                     
                                                                      */


    //                                                           

    frameTime = S5K4ECGX_MIPI_GetExposureTime();
    //               

    if (S5K4ECGX_Driver.afStateOnOriginalSet)
    {
        //                                            
        SENSORDB("[4EC] AF window afStateOnOriginalSet = 1.......\n");
        S5K4ECGX_MIPI_AF_rollbackWinSet();
    }
    else
    {
        SENSORDB("[4EC] AF window Update New Window.......\n");
        S5K4ECGX_MIPI_AF_Set_Window2HW();
    }
    Sleep(frameTime); //              

    return S5K4ECGX_AAA_AF_STATUS_OK;
}




S5K4ECGX_AAA_STATUS_ENUM
S5K4ECGX_MIPI_AF_Stop(void)
{

#if 0
#if defined(S5K4ECGX_MIPI_AF_Enable)

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.afState = S5K4ECGX_AF_STATE_IDLE;

    //         
    if ((!S5K4ECGX_Driver.userAskAeLock) &&
        (S5K4ECGX_AF_MODE_SINGLE == S5K4ECGX_Driver.afMode))
    {
        spin_unlock(&s5k4ecgx_mipi_drv_lock);
        S5K4ECGX_MIPI_AE_UnLock();
    }
    else
    {
        spin_unlock(&s5k4ecgx_mipi_drv_lock);
    }
    SENSORDB("S5K4ECGX ~~~~S5K4ECGX_MIPI_AF_Stop---\n");
#endif
#endif

    return S5K4ECGX_AAA_AF_STATUS_OK;
}


/*
           
                                       
 
                                                                 
                              

                                              
                                              
                                              
                                                     

                                              
                                                     

                                              
                                                      

                                              
                                                           

                                                                                                                                                                      

           
 
*/


static S5K4ECGX_AAA_STATUS_ENUM
S5K4ECGX_MIPI_AF_CancelFocus(void)
{
    signed int loop_iter = 35;
    unsigned int af_status;
    unsigned int frameTime;

    SENSORDB("[4EC] CancelFocus+\n");

    if (S5K4ECGX_AF_MODE_RSVD == S5K4ECGX_Driver.afMode)
    {
        //                 
        return S5K4ECGX_AAA_AF_STATUS_OK;
    }

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);

    af_status = 1;
    while(loop_iter--)
    {
        S5K4ECGX_write_cmos_sensor(0x002E,0x2EEE);
        af_status = S5K4ECGX_read_cmos_sensor(0x0F12);
        if ((af_status == 1) || (af_status == 6) || (af_status == 7))
        {
            //                                          
            Sleep(2);
        }
    }


    S5K4ECGX_write_cmos_sensor(0x002E,0x2EEE);
    af_status = S5K4ECGX_read_cmos_sensor(0x028C);

    frameTime = S5K4ECGX_MIPI_GetExposureTime();
    //                                                           

    if (1 != af_status) //         
    {
        S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
        S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
        S5K4ECGX_write_cmos_sensor(0x002A,0x028C);
        S5K4ECGX_write_cmos_sensor(0x0F12,0x0001); //     
        Sleep(frameTime);
    }

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.afPrevMode = S5K4ECGX_Driver.afMode;
    S5K4ECGX_Driver.afMode = S5K4ECGX_AF_MODE_RSVD;
    S5K4ECGX_Driver.afState = S5K4ECGX_AF_STATE_DONE;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);
    //                                 

    return S5K4ECGX_AAA_AF_STATUS_OK;
}




S5K4ECGX_AAA_STATUS_ENUM
S5K4ECGX_MIPI_AF_Start(S5K4ECGX_AF_MODE_ENUM mode)
{
    unsigned int af_status;
    unsigned int frameTime;
    //                           

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    if (S5K4ECGX_Driver.userAskAeLock)
    {
        //                                                                     
        spin_unlock(&s5k4ecgx_mipi_drv_lock);
        SENSORDB("[4EC] AF_Start+: Return due2 AE Been Loked\n");
        return S5K4ECGX_AAA_AF_STATUS_OK;
    }


    if (mode == S5K4ECGX_AF_MODE_SINGLE)
    {
        SENSORDB("[4EC] SAF_Start+\n\n");
        S5K4ECGX_Driver.afMode = S5K4ECGX_AF_MODE_SINGLE;
        S5K4ECGX_Driver.afState = S5K4ECGX_AF_STATE_ENTERING;
    }
    else
    {
        SENSORDB("[4EC] CAF_Start+\n\n");
        if (S5K4ECGX_AF_MODE_CONTINUOUS == S5K4ECGX_Driver.afMode)
        {
            spin_unlock(&s5k4ecgx_mipi_drv_lock);
            SENSORDB("[4EC] CAF_Start: Been at this Mode...\n");
            return;
        }
        S5K4ECGX_Driver.afMode = S5K4ECGX_AF_MODE_CONTINUOUS;
        S5K4ECGX_Driver.afState = S5K4ECGX_AF_STATE_ENTERING;
    }
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    frameTime = S5K4ECGX_MIPI_GetExposureTime();
    //                                                          


    if (mode == S5K4ECGX_AF_MODE_SINGLE)
    {
        S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
        S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
        S5K4ECGX_write_cmos_sensor(0x002A, 0x163E);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0); //                   
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080); //                      
        S5K4ECGX_write_cmos_sensor(0x002A, 0x15E8);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010); //                     
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0018); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0028); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0030); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0038); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0048); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0050); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0058); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0068); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0078); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0088); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0090); //              
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0098); //              

    }
    else
    {
            S5K4ECGX_write_cmos_sensor(0x002A, 0x163E);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x00a0);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070); //                                                        
            S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
            S5K4ECGX_write_cmos_sensor(0x002A, 0x15E8);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A); //                                                    
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0018); //                                    
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0030); //                  
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0048); //                  
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0058); //                  
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0068); //                  
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070); //                  
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0078); //                  
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080); //                  
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0088); //                  
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0090); //                  
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0098); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0090); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0098); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0054); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0058); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x005C); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0064); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0068); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x006C); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0074); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0078); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x007C); //                   
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080); //                   
    }


    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x028C);
    if (mode == S5K4ECGX_AF_MODE_SINGLE)
    {
        S5K4ECGX_write_cmos_sensor(0x0F12,0x0005);
    }
    else
    {
        S5K4ECGX_write_cmos_sensor(0x0F12,0x0006); //              
    }
    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.afState = S5K4ECGX_AF_STATE_ENTERED;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    Sleep(frameTime * 2); //              
    return;
}



static S5K4ECGX_AAA_STATUS_ENUM
S5K4ECGX_MIPI_AF_Move_to(unsigned int a_u2MovePosition)//                            
{
    return S5K4ECGX_AAA_AF_STATUS_OK;
}




static S5K4ECGX_AAA_STATUS_ENUM
S5K4ECGX_MIPI_AF_Get_Status(unsigned int *pFeatureReturnPara32)
{
    S5K4ECGX_AF_STATE_ENUM af_state;
    S5K4ECGX_AF_MODE_ENUM af_Mode;
    unsigned int af_1stSearch_status;
    unsigned int frameTime;

    //                         


    if (S5K4ECGX_AF_STATE_ENTERING == S5K4ECGX_Driver.afState)
    {
        SENSORDB("\n[4EC]AFGet_Status: ENTERING State\n");
        *pFeatureReturnPara32 = SENSOR_AF_IDLE;
        return S5K4ECGX_AAA_AF_STATUS_OK;
    }


    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x2EEE);
    af_1stSearch_status = S5K4ECGX_read_cmos_sensor(0x0F12);

    {
        switch (af_1stSearch_status)
        {
            case 0:
                *pFeatureReturnPara32 = SENSOR_AF_IDLE;// 
                //                                 
                break;
            case 1:
                *pFeatureReturnPara32 = SENSOR_AF_FOCUSING;
                //                                        
                break;

            case 2:
                *pFeatureReturnPara32 = SENSOR_AF_FOCUSED;
                //                                    
                break;

            case 3: //                                 
                *pFeatureReturnPara32 = SENSOR_AF_ERROR;//                
                //                                           
                break;

            case 4: //        
                *pFeatureReturnPara32 = SENSOR_AF_IDLE;//               
                spin_lock(&s5k4ecgx_mipi_drv_lock);
                S5K4ECGX_Driver.afState = S5K4ECGX_AF_STATE_DONE;
                spin_unlock(&s5k4ecgx_mipi_drv_lock);
                //                                      
                break;

            case 6: //          
                *pFeatureReturnPara32 = SENSOR_AF_SCENE_DETECTING;//                          
                //                                       
                break;

            case 7: //             
                *pFeatureReturnPara32 = SENSOR_AF_SCENE_DETECTING;//                          
                //                                          
                break;

            default:
                *pFeatureReturnPara32 = SENSOR_AF_SCENE_DETECTING;
                SENSORDB("\n[4EC] AF--- default. Status:%x\n", af_1stSearch_status);
                {
                    //                                                   
                    UINT32 sensorID = 0;
                    S5K4ECGX_MIPI_GetSensorID(&sensorID);
                }
                break;
        }

        return S5K4ECGX_AAA_AF_STATUS_OK;
    }

    *pFeatureReturnPara32 = SENSOR_AF_FOCUSED;
     return S5K4ECGX_AAA_AF_STATUS_OK;
}



static void S5K4ECGX_MIPI_set_AF_infinite(kal_bool is_AF_OFF)
{
#if 0
    if(is_AF_OFF){
      S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
      S5K4ECGX_write_cmos_sensor(0x002a, 0x028E);
      S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
      Sleep(100);
      S5K4ECGX_write_cmos_sensor(0x002a, 0x028C);
      S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);
    } else {
      S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
      S5K4ECGX_write_cmos_sensor(0x002a, 0x028C);
      S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);
    }
#endif

}
/*                                                          
                                                            
                                                          */





/*                                                          
                         
                                                          */

/*                                                                        
          
                        
 
             
                                           
 
            
                              
 
         
         
 
                
 
                                                                        */
static kal_uint32 S5K4ECGX_MIPI_ReadShutter(void)
{
    unsigned int interval = 0;

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x2C28);
    interval  = S5K4ECGX_read_cmos_sensor(0x0F12);
    interval |= (S5K4ECGX_read_cmos_sensor(0x0F12) << 16);

    //                               
    interval = interval * 5 / 2; //  

    SENSORDB("[4EC] Shutter = %d us\n", interval);
    return interval;
}


void S5K4ECGX_MIPI_SetShutter(kal_uint32 iShutter)
{
    //                             
    iShutter *= 32;
    unsigned int exposureTime = iShutter >> 3; //                

    SENSORDB("[4EC] SetdShutter+, iShutter=%d us, 0x%08x\n", iShutter, exposureTime);
    //                   
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x04E6);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0779); //                
    S5K4ECGX_Driver.manualAEStart = 1;

    S5K4ECGX_write_cmos_sensor(0x002A,0x04AC);
    S5K4ECGX_write_cmos_sensor(0x0F12,exposureTime&0xFFFF); //             
    S5K4ECGX_write_cmos_sensor(0x002A,0x04AE);
    S5K4ECGX_write_cmos_sensor(0x0F12,exposureTime>>16); //             
    S5K4ECGX_write_cmos_sensor(0x002A,0x04B0);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0001); //                    

    //                                          
    //                                                            
    //                                          
        return;
}


/*                                                                        
          
                           
             
                                       
            
         
         
                       
                
                                                                        */
static kal_uint32 S5K4ECGX_MIPI_ReadGain(void)
{

    //                                

    unsigned int A_Gain, D_Gain, ISOValue;
    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E, 0x2BC4);
    A_Gain = S5K4ECGX_read_cmos_sensor(0x0F12);
    D_Gain = S5K4ECGX_read_cmos_sensor(0x0F12);

    ISOValue = ((A_Gain * D_Gain) >> 8);

    SENSORDB("[4EC] ReadGain+, isoSpeed=%d\n", ISOValue);
/*
                                     
     
                       
                     

                       
                  

                       
               
                  
 
*/
    return ISOValue;
}



/*                                                                        
          
                          
             
                                     
            
         
         
                       
                
                                                                        */
static void S5K4ECGX_MIPI_SetGain(kal_uint32 iGain)
{
    //                                       
    //                                         
    //                     
    //                           
    //                           
    //                          


    unsigned int totalGain = iGain;
    /*
                  
     
                
                                                 
              

                
                          
              

                
               
                          
              
     
    */
    SENSORDB("[4EC] SetGain+, isoGain=%d\n", totalGain);

    //                   
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A,0x04E6);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0779); //                

    //                                          
    S5K4ECGX_write_cmos_sensor(0x002A,0x04B2);
    S5K4ECGX_write_cmos_sensor(0x0F12,totalGain);   //          
    S5K4ECGX_write_cmos_sensor(0x002A,0x04B4);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0001);

#if 0
    switch (iGain)
    {
        case 200://           
             //       
             S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
             S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
             S5K4ECGX_write_cmos_sensor(0x002A, 0x0938);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0014);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x03A2);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0484);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x07D0);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x1388);  //                     
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D6);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                        
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                               

             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D0);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                   
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0380);  //                  
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                      
             S5K4ECGX_write_cmos_sensor(0x002A, 0x06C2);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);  //                  
             break;
        case 400://           
             //       
             S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
             S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
             S5K4ECGX_write_cmos_sensor(0x002A, 0x0938);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0014);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x08D2);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C84);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x10D0);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x1388);  //                     
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D6);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                        
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                               
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D0);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                   
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0700);  //                  
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                      
             S5K4ECGX_write_cmos_sensor(0x002A, 0x06C2);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);  //                  
             break;
        default:
            case 100://           
             //       
             S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
             S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
             S5K4ECGX_write_cmos_sensor(0x002A, 0x0938);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0014);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D2);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x07D0);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x1388);  //                     
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D6);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                        
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                               
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D0);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                   
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C0);  //                  
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                      
             S5K4ECGX_write_cmos_sensor(0x002A, 0x06C2);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);  //                  
             break;
    }
#endif

    return;
}


void S5K4ECGX_MIPI_get_exposure_gain()
{
    kal_uint32 again = 0, dgain = 0, evTime = 0;

    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E, 0x2C28);
    evTime  = S5K4ECGX_read_cmos_sensor(0x0F12);
    evTime += S5K4ECGX_read_cmos_sensor(0x0F12) << 16 ;
    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.currentExposureTime = evTime >> 2;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x2BC4);
    again = S5K4ECGX_read_cmos_sensor(0x0F12); //      
    dgain = S5K4ECGX_read_cmos_sensor(0x0F12); //      

    spin_lock(&s5k4ecgx_mipi_drv_lock);

    S5K4ECGX_Driver.currentAxDGain = (dgain * again) >> 8;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

}


void S5K4ECGX_MIPI_GetAEFlashlightInfo(uintptr_t infoAddr)
{
    SENSOR_FLASHLIGHT_AE_INFO_STRUCT* pAeInfo = (SENSOR_FLASHLIGHT_AE_INFO_STRUCT*) infoAddr;

    pAeInfo->Exposuretime = S5K4ECGX_MIPI_ReadShutter();
    pAeInfo->Gain = S5K4ECGX_MIPI_ReadGain();
    pAeInfo->u4Fno = 28;
    pAeInfo->GAIN_BASE = 0x100;

    return;
}



#define FLASH_BV_THRESHOLD 0x25
static void S5K4ECGX_MIPI_FlashTriggerCheck(unsigned int *pFeatureReturnPara32)
{
    unsigned int NormBr;

    //                                          
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x29A4);
    NormBr = S5K4ECGX_read_cmos_sensor(0x0F12);

    if (NormBr > FLASH_BV_THRESHOLD)
    {
       *pFeatureReturnPara32 = FALSE;
        return;
    }

    *pFeatureReturnPara32 = TRUE;
    return;
}


/*                                                          
                                                            
                                                          */



/*                                                          
                                                            
                                                          */
static
void S5K4ECGX_MIPI_Config_JPEG_Capture(ACDK_SENSOR_JPEG_OUTPUT_PARA *jpeg_para)
{
#if defined(__CAPTURE_JPEG_OUTPUT__)
    /*
                                               
                                                                                      
                                               
                                                                          
                                                                             
                                               
                                                                            
    */

    S5K4ECGX_write_cmos_sensor(0x002A, 0x0398);
    S5K4ECGX_write_cmos_sensor(0x0F12, jpeg_para->tgtWidth);
    S5K4ECGX_write_cmos_sensor(0x0F12, jpeg_para->tgtHeight);

    S5K4ECGX_write_cmos_sensor(0x002A, 0x0478);
    S5K4ECGX_write_cmos_sensor(0x0F12, jpeg_para->quality); //                        

#endif

    return;
}


#define JPEG_MARKER_SKIP_CODE    0x00  //                       
#define JPEG_MARKER_START_CODE   0XFF

#define JPEG_MARKER_SOF(I)       (0xC0 + I)

/*                                                          */
#define JPEG_MARKER_SOF0         0XC0
#define JPEG_MARKER_SOF1         0XC1
#define JPEG_MARKER_SOF2         0XC2
#define JPEG_MARKER_SOF3         0XC3

/*                                                      */
#define JPEG_MARKER_SOF5         0XC5
#define JPEG_MARKER_SOF6         0XC6
#define JPEG_MARKER_SOF7         0XC7

/*                                                             */
#define JPEG_MARKER_JPG0         0XC8
#define JPEG_MARKER_SOF9         0XC9
#define JPEG_MARKER_SOF10        0XCA
#define JPEG_MARKER_SOF11        0XCB

/*                                                         */
#define JPEG_MARKER_SOF13        0xCD
#define JPEG_MARKER_SOF14        0xCE
#define JPEG_MARKER_SOF15        0xCF

/*                             */
#define JPEG_MARKER_DHT          0xC4  /*                         */

/*                                              */
#define JPEG_MARKER_DAC          0xCC  /*                                          */

/*                              */
#define JPEG_MARKER_RST(I)       (0xD0 + I)
#define JPEG_MARKER_RST0         0xD0
#define JPEG_MARKER_RST1         0xD1
#define JPEG_MARKER_RST2         0xD2
#define JPEG_MARKER_RST3         0xD3
#define JPEG_MARKER_RST4         0xD4
#define JPEG_MARKER_RST5         0xD5
#define JPEG_MARKER_RST6         0xD6
#define JPEG_MARKER_RST7         0xD7

#define JPEG_MARKER_SOI          0xD8
#define JPEG_MARKER_EOI          0xD9
#define JPEG_MARKER_SOS          0xDA
#define JPEG_MARKER_DQT          0xDB
#define JPEG_MARKER_DNL          0xDC
#define JPEG_MARKER_DRI          0xDD
#define JPEG_MARKER_DHP          0xDE
#define JPEG_MARKER_EXP          0xDF

#define JPEG_MARKER_APP(I)       (0xE0 + I)

#define JPEG_MARKER_JPG(I)       (0xF0 + I)

#define JPEG_MARKER_TEM          0x01

#define JPEG_MARKER_COM          0xFE

//         
typedef enum {
   JPEG_PARSE_STATE_STOP = 0,
   JPEG_PARSE_STATE_WAITING_FOR_SOI,

   JPEG_PARSE_STATE_WAITING_FOR_MARKER,
   JPEG_PARSE_STATE_WAITING_FOR_LENGTH,
   JPEG_PARSE_STATE_WAITING_FOR_DATA,

   JPEG_PARSE_STATE_ERROR,
   JPEG_PARSE_STATE_COMPLETE
} JPEG_PARSE_STATE_ENUM;


unsigned int
jpegParserParseImage(unsigned char* srcBuf, unsigned int bufSize, unsigned int *eoiOffset)
{
   unsigned char marker[2] = {0};
   unsigned char *rdPtr = srcBuf;
   unsigned char *endPtr = srcBuf + bufSize;
   unsigned int   parseState = JPEG_PARSE_STATE_WAITING_FOR_SOI;
   unsigned short curSegmentLength;
   unsigned int   offsetOfEncounter100Zeros = 0;
   unsigned char  array100Zeros[100] ={0};

    if (JPEG_MARKER_START_CODE != rdPtr[0] || JPEG_MARKER_SOI != rdPtr[1])
    {
        //                                 
        SENSORDB("[4EC] jpegParserParseImage: invalid file ###############\n");
        *eoiOffset = 2560 * 1920;
        return FALSE;
    }



    rdPtr += 636;
    SENSORDB("[4EC] jpegParserParseImage:A (rd,end)=(0x%x,0x%x)\n", rdPtr, endPtr);


    while (rdPtr < endPtr)
    {
        if (JPEG_MARKER_START_CODE == rdPtr[0])
        {
            if (JPEG_MARKER_EOI == rdPtr[1])
            {
                rdPtr += 2;
                SENSORDB("[4EC] jpegParserParseImage B:Encounter FFD9: rdPtr=0x%x\n", rdPtr);
                break;
            }
            rdPtr += 1;
        }
        else
        {
            if ((offsetOfEncounter100Zeros == 0) &&
                ((rdPtr[0] == 0) && (rdPtr[1] == 0)))
            {
                if (!(memcmp(array100Zeros, rdPtr, 100)))
                {
                    SENSORDB("[4EC] jpegParserParseImage:C offsetOfEncounter100Zeros = 1: rdPtr=0x%x\n", rdPtr);
                    offsetOfEncounter100Zeros = rdPtr;
                }
            }
            rdPtr += 1;
        }
    }

    if (rdPtr >= endPtr)
    {
       //                             
       rdPtr = offsetOfEncounter100Zeros;
       rdPtr[0] = 0xFF;
       rdPtr[1] = 0xD9;
       rdPtr += 2;
       SENSORDB("[4EC] jpegParserParseImage:D offsetOfEncounter100Zeros = 1: rdPtr=0x%x\n", rdPtr);
    }

    SENSORDB("[4EC] jpegParserParseImage:E Marker:(%x,%x); (rd,end)=(0x%x,0x%x)\n",rdPtr[-2], rdPtr[-1], rdPtr, endPtr);


   *eoiOffset = (rdPtr - srcBuf);
   SENSORDB("[4EC] jpegParserParseImage:eoiOffset =%x\n",*eoiOffset);

   return TRUE;
}



void S5K4ECGX_MIPI_JPEG_Capture_Parser(
     uintptr_t jpegFileAddr, UINT32 jpegMaxBufSize, ACDK_SENSOR_JPEG_INFO *jpeg_info)
{
    unsigned int eoiOffset = 0;
#if defined(__CAPTURE_JPEG_OUTPUT__)

    unsigned int* ptr;
    ptr = (unsigned int*)jpegFileAddr;
    //                                                                          
    //                                                                                                                                                          

    //                                     
    jpegParserParseImage((unsigned char*)jpegFileAddr, jpegMaxBufSize, &eoiOffset);

    jpeg_info->u4FileSize = eoiOffset;
    jpeg_info->u4SrcW = S5K4ECGX_Driver.jpegSensorPara.tgtWidth;
    jpeg_info->u4SrcH = S5K4ECGX_Driver.jpegSensorPara.tgtHeight;

    SENSORDB("[4EC] JPEG_Capture_Parser jpegMaxBufSize=0x%x, W=%d, H=%d\n", jpegMaxBufSize, jpeg_info->u4SrcW, jpeg_info->u4SrcH);
#else
    jpeg_info->u4FileSize = 2560 * 1920 * 1; //                  
    jpeg_info->u4SrcW = S5K4ECGX_Driver.jpegSensorPara.tgtWidth;
    jpeg_info->u4SrcH = S5K4ECGX_Driver.jpegSensorPara.tgtHeight;
#endif

    return;
}


/*                                                          
                                                            
                                                          */


#define S5K4EC_PREVIEW_MODE             0
#define S5K4EC_VIDEO_MODE               1
#define S5K4EC_PREVIEW_FULLSIZE_MODE    2

#define MIPI_CLK0_SYS_OP_RATE   0x4074
#define MIPI_CLK0_MIN           0x59D8
#define MIPI_CLK0_MAX           0x59D8

#define MIPI_CLK1_SYS_OP_RATE   0x4074 //                           
#define MIPI_CLK1_MIN           0x59D8
#define MIPI_CLK1_MAX           0x59D8
#define MIPI_CAP_CLK_IDX        1

static void S5K4ECGX_MIPI_Init_Setting(void)
{
    SENSORDB("[4EC] Sensor Init...\n");
    //               
    //               
    //                                                                                                                   


#if defined(__CAPTURE_JPEG_OUTPUT__)
    #define OUTPUT_FMT  9 //    
#else
    #define OUTPUT_FMT  5 //   
#endif
    //                                                   
    S5K4ECGX_Driver.userAskAeLock = 0;//     
    S5K4ECGX_Driver.afMode = S5K4ECGX_AF_MODE_RSVD;
    S5K4ECGX_Driver.afState = S5K4ECGX_AF_STATE_UNINIT;
    S5K4ECGX_Driver.afStateOnOriginalSet = 1;//    
    S5K4ECGX_Driver.Night_Mode = 0;
    S5K4ECGX_Driver.videoFrameRate = 30;
    S5K4ECGX_Driver.sceneMode = SCENE_MODE_OFF;
    S5K4ECGX_Driver.isoSpeed = AE_ISO_100;
    S5K4ECGX_Driver.awbMode = AWB_MODE_AUTO;
    S5K4ECGX_Driver.capExposureTime = 60;
    S5K4ECGX_Driver.aeWindows[0] = 0;
    S5K4ECGX_Driver.aeWindows[1] = 0;
    S5K4ECGX_Driver.aeWindows[2] = 0;
    S5K4ECGX_Driver.aeWindows[3] = 0;
    S5K4ECGX_Driver.jpegSensorPara.tgtWidth = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV;
    S5K4ECGX_Driver.jpegSensorPara.tgtHeight = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV;
    S5K4ECGX_Driver.jpegSensorPara.quality = 100;


    S5K4ECGX_MIPI_sensor_pclk = MIPI_CLK0_MAX * 4000 * 2; //                  
    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x0010, 0x0001);  //         
    S5K4ECGX_write_cmos_sensor(0x1030, 0x0000);  //                
    S5K4ECGX_write_cmos_sensor(0x0014, 0x0001);  //                                                      
    Sleep(50);

    //                                                                                  
    //              
    //                                                                                  
    S5K4ECGX_write_cmos_sensor(0x0028, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1082);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                                              
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1088);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                                                                


    //                                                                                  
    //                                
    //                                                                                  
    {
        static const kal_uint16 addr_data_pair[] =
        {
        //                                  
        //                                           
        //                                                                  
            0x002A ,0x007A,
            0x0F12 ,0x0000,
            0x002A ,0xE406,
            0x0F12 ,0x0092,
            0x002A ,0xE410,
            0x0F12 ,0x3804, //                                               
            0x002A ,0xE41A,
            0x0F12 ,0x0010,
            0x002A ,0xE420,
            0x0F12 ,0x0003, //                         
            0x0F12 ,0x0060, //                                
            0x002A ,0xE42E,
            0x0F12 ,0x0004, //                     
            0x002A ,0xF400,
            0x0F12 ,0x5A3C, //                                
            0x0F12 ,0x0023, //                                                                                                                                                                        
            0x0F12 ,0x8080, //          
            0x0F12 ,0x03AF, //                         
            0x0F12 ,0x000A, //        
            0x0F12 ,0xAA54, //                                          
            0x0F12 ,0x0040, //                         
            0x0F12 ,0x464E, //                                         
            0x0F12 ,0x0240, //                
            0x0F12 ,0x0240, //                
            0x0F12 ,0x0040, //                                   
            0x0F12 ,0x1000, //                                                                           
            0x0F12 ,0x55FF, //                                                     
            0x0F12 ,0xD000, //                                             
            0x0F12 ,0x0010, //          
            0x0F12 ,0x0202, //                                      
            0x0F12 ,0x0401, //                                    
            0x0F12 ,0x0022, //                                                       
            0x0F12 ,0x0088, //                                                                                                          
            0x0F12 ,0x009F, //                                                           
            0x0F12 ,0x0000, //                   
            0x0F12 ,0x1800, //                                                                 
            0x0F12 ,0x0088, //                  
            0x0F12 ,0x0000, //                                  
            0x0F12 ,0x2428, //                                             
            0x0F12 ,0x0000, //             
            0x0F12 ,0x03EE, //            
            0x0F12 ,0x0000, //               
            0x0F12 ,0x0000, //                 
            0x0F12 ,0x0000, //               
            0x002A ,0xF552,
            0x0F12 ,0x0708, //                            
            0x0F12 ,0x080C, //                              
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
    }

    //                                                                                 
    //                                                                
    //                                                                                 
    {
        static const kal_uint16 addr_data_pair[] =
        {
        //                    
             0x0028, 0x7000,
             0x002A, 0x3AF8,
             0x0F12 ,0xB5F8,     //          
             0x0F12 ,0x4B41,     //          
             0x0F12 ,0x4941,     //          
             0x0F12 ,0x4842,     //          
             0x0F12 ,0x2200,     //          
             0x0F12 ,0xC008,     //          
             0x0F12 ,0x6001,     //          
             0x0F12 ,0x4941,     //          
             0x0F12 ,0x4841,     //          
             0x0F12 ,0x2401,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFC5E,     //          
             0x0F12 ,0x4940,     //          
             0x0F12 ,0x4841,     //          
             0x0F12 ,0x2702,     //          
             0x0F12 ,0x0022,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFC58,     //          
             0x0F12 ,0x0260,     //          
             0x0F12 ,0x4C3F,     //          
             0x0F12 ,0x8020,     //          
             0x0F12 ,0x2600,     //          
             0x0F12 ,0x8066,     //          
             0x0F12 ,0x493E,     //          
             0x0F12 ,0x483E,     //          
             0x0F12 ,0x6041,     //          
             0x0F12 ,0x493E,     //          
             0x0F12 ,0x483F,     //          
             0x0F12 ,0x003A,     //          
             0x0F12 ,0x2503,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFC4A,     //          
             0x0F12 ,0x483A,     //          
             0x0F12 ,0x493D,     //          
             0x0F12 ,0x30C0,     //          
             0x0F12 ,0x63C1,     //          
             0x0F12 ,0x4F38,     //          
             0x0F12 ,0x483C,     //          
             0x0F12 ,0x3F80,     //          
             0x0F12 ,0x6438,     //          
             0x0F12 ,0x483B,     //          
             0x0F12 ,0x493C,     //          
             0x0F12 ,0x6388,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x493B,     //          
             0x0F12 ,0x483C,     //          
             0x0F12 ,0x2504,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFC39,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x493A,     //          
             0x0F12 ,0x483B,     //          
             0x0F12 ,0x2505,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF89D,     //          
             0x0F12 ,0x4839,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x4939,     //          
             0x0F12 ,0x2506,     //          
             0x0F12 ,0x1D80,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF896,     //          
             0x0F12 ,0x4835,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x4936,     //          
             0x0F12 ,0x2507,     //          
             0x0F12 ,0x300C,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF88F,     //          
             0x0F12 ,0x4832,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x4934,     //          
             0x0F12 ,0x2508,     //          
             0x0F12 ,0x3010,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF888,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x4932,     //          
             0x0F12 ,0x4832,     //          
             0x0F12 ,0x2509,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFC18,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x4931,     //          
             0x0F12 ,0x4831,     //          
             0x0F12 ,0x250A,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFC12,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x4930,     //          
             0x0F12 ,0x4830,     //          
             0x0F12 ,0x250B,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFC0C,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x492F,     //          
             0x0F12 ,0x482F,     //          
             0x0F12 ,0x250C,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFC06,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x492E,     //          
             0x0F12 ,0x482E,     //          
             0x0F12 ,0x250D,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFC00,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x492D,     //          
             0x0F12 ,0x482D,     //          
             0x0F12 ,0x250E,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFBFA,     //          
             0x0F12 ,0x8626,     //          
             0x0F12 ,0x20FF,     //          
             0x0F12 ,0x1C40,     //          
             0x0F12 ,0x8660,     //          
             0x0F12 ,0x482A,     //          
             0x0F12 ,0x64F8,     //          
             0x0F12 ,0x492A,     //          
             0x0F12 ,0x482B,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x240F,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFBEE,     //          
             0x0F12 ,0x4929,     //          
             0x0F12 ,0x482A,     //          
             0x0F12 ,0x0022,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFBE9,     //          
             0x0F12 ,0xBCF8,     //          
             0x0F12 ,0xBC08,     //          
             0x0F12 ,0x4718,     //          
             0x0F12 ,0x017B,     //          
             0x0F12 ,0x4EC2,     //          
             0x0F12 ,0x237F,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x1F90,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x3CB9,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0xE38B,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x3CF1,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0xC3B1,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4780,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x3D4F,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x0080,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x3D8B,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0xB49D,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x3E37,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x3DEB,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0xFFFF,     //          
             0x0F12 ,0x00FF,     //          
             0x0F12 ,0x17E0,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x3FB3,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x053D,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x0A89,     //          
             0x0F12 ,0x6CD2,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x02C9,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x0A9A,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x02D2,     //          
             0x0F12 ,0x4001,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x9E65,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4075,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x7C49,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x40E9,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x7C63,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4105,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x8F01,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x41A7,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x7F3F,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4235,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x98C5,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x42FB,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x4351,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0xA70B,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4373,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x400D,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0xB570,     //          
             0x0F12 ,0x000C,     //          
             0x0F12 ,0x0015,     //          
             0x0F12 ,0x0029,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFB98,     //          
             0x0F12 ,0x49F8,     //          
             0x0F12 ,0x00A8,     //          
             0x0F12 ,0x500C,     //          
             0x0F12 ,0xBC70,     //          
             0x0F12 ,0xBC08,     //          
             0x0F12 ,0x4718,     //          
             0x0F12 ,0x6808,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x6849,     //          
             0x0F12 ,0x0409,     //          
             0x0F12 ,0x0C09,     //          
             0x0F12 ,0x4AF3,     //          
             0x0F12 ,0x8992,     //          
             0x0F12 ,0x2A00,     //          
             0x0F12 ,0xD00D,     //          
             0x0F12 ,0x2300,     //          
             0x0F12 ,0x1A89,     //          
             0x0F12 ,0xD400,     //          
             0x0F12 ,0x000B,     //          
             0x0F12 ,0x0419,     //          
             0x0F12 ,0x0C09,     //          
             0x0F12 ,0x23FF,     //          
             0x0F12 ,0x33C1,     //          
             0x0F12 ,0x1810,     //          
             0x0F12 ,0x4298,     //          
             0x0F12 ,0xD800,     //          
             0x0F12 ,0x0003,     //          
             0x0F12 ,0x0418,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x4AEB,     //          
             0x0F12 ,0x8150,     //          
             0x0F12 ,0x8191,     //          
             0x0F12 ,0x4770,     //          
             0x0F12 ,0xB5F3,     //          
             0x0F12 ,0x0004,     //          
             0x0F12 ,0xB081,     //          
             0x0F12 ,0x9802,     //          
             0x0F12 ,0x6800,     //          
             0x0F12 ,0x0600,     //          
             0x0F12 ,0x0E00,     //          
             0x0F12 ,0x2201,     //          
             0x0F12 ,0x0015,     //          
             0x0F12 ,0x0021,     //          
             0x0F12 ,0x3910,     //          
             0x0F12 ,0x408A,     //          
             0x0F12 ,0x40A5,     //          
             0x0F12 ,0x4FE4,     //          
             0x0F12 ,0x0016,     //          
             0x0F12 ,0x2C10,     //          
             0x0F12 ,0xDA03,     //          
             0x0F12 ,0x8839,     //          
             0x0F12 ,0x43A9,     //          
             0x0F12 ,0x8039,     //          
             0x0F12 ,0xE002,     //          
             0x0F12 ,0x8879,     //          
             0x0F12 ,0x43B1,     //          
             0x0F12 ,0x8079,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFB64,     //          
             0x0F12 ,0x2C10,     //          
             0x0F12 ,0xDA03,     //          
             0x0F12 ,0x8839,     //          
             0x0F12 ,0x4329,     //          
             0x0F12 ,0x8039,     //          
             0x0F12 ,0xE002,     //          
             0x0F12 ,0x8879,     //          
             0x0F12 ,0x4331,     //          
             0x0F12 ,0x8079,     //          
             0x0F12 ,0x49DA,     //          
             0x0F12 ,0x8809,     //          
             0x0F12 ,0x2900,     //          
             0x0F12 ,0xD102,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFB5D,     //          
             0x0F12 ,0x2000,     //          
             0x0F12 ,0x9902,     //          
             0x0F12 ,0x6008,     //          
             0x0F12 ,0xBCFE,     //          
             0x0F12 ,0xBC08,     //          
             0x0F12 ,0x4718,     //          
             0x0F12 ,0xB538,     //          
             0x0F12 ,0x9C04,     //          
             0x0F12 ,0x0015,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x9400,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFB58,     //          
             0x0F12 ,0x4AD1,     //          
             0x0F12 ,0x8811,     //          
             0x0F12 ,0x2900,     //          
             0x0F12 ,0xD00F,     //          
             0x0F12 ,0x8820,     //          
             0x0F12 ,0x4281,     //          
             0x0F12 ,0xD20C,     //          
             0x0F12 ,0x8861,     //          
             0x0F12 ,0x8853,     //          
             0x0F12 ,0x4299,     //          
             0x0F12 ,0xD200,     //          
             0x0F12 ,0x1E40,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x8020,     //          
             0x0F12 ,0x8851,     //          
             0x0F12 ,0x8061,     //          
             0x0F12 ,0x4368,     //          
             0x0F12 ,0x1840,     //          
             0x0F12 ,0x6060,     //          
             0x0F12 ,0xBC38,     //          
             0x0F12 ,0xBC08,     //          
             0x0F12 ,0x4718,     //          
             0x0F12 ,0xB5F8,     //          
             0x0F12 ,0x0004,     //          
             0x0F12 ,0x6808,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x2201,     //          
             0x0F12 ,0x0015,     //          
             0x0F12 ,0x0021,     //          
             0x0F12 ,0x3910,     //          
             0x0F12 ,0x408A,     //          
             0x0F12 ,0x40A5,     //          
             0x0F12 ,0x4FBE,     //          
             0x0F12 ,0x0016,     //          
             0x0F12 ,0x2C10,     //          
             0x0F12 ,0xDA03,     //          
             0x0F12 ,0x8839,     //          
             0x0F12 ,0x43A9,     //          
             0x0F12 ,0x8039,     //          
             0x0F12 ,0xE002,     //          
             0x0F12 ,0x8879,     //          
             0x0F12 ,0x43B1,     //          
             0x0F12 ,0x8079,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFB31,     //          
             0x0F12 ,0x2C10,     //          
             0x0F12 ,0xDA03,     //          
             0x0F12 ,0x8838,     //          
             0x0F12 ,0x4328,     //          
             0x0F12 ,0x8038,     //          
             0x0F12 ,0xE002,     //          
             0x0F12 ,0x8878,     //          
             0x0F12 ,0x4330,     //          
             0x0F12 ,0x8078,     //          
             0x0F12 ,0x48B6,     //          
             0x0F12 ,0x8800,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0xD507,     //          
             0x0F12 ,0x4BB5,     //          
             0x0F12 ,0x7819,     //          
             0x0F12 ,0x4AB5,     //          
             0x0F12 ,0x7810,     //          
             0x0F12 ,0x7018,     //          
             0x0F12 ,0x7011,     //          
             0x0F12 ,0x49B4,     //          
             0x0F12 ,0x8188,     //          
             0x0F12 ,0xBCF8,     //          
             0x0F12 ,0xBC08,     //          
             0x0F12 ,0x4718,     //          
             0x0F12 ,0xB538,     //          
             0x0F12 ,0x48B2,     //          
             0x0F12 ,0x4669,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFB1C,     //          
             0x0F12 ,0x48B1,     //          
             0x0F12 ,0x49B0,     //          
             0x0F12 ,0x69C2,     //          
             0x0F12 ,0x2400,     //          
             0x0F12 ,0x31A8,     //          
             0x0F12 ,0x2A00,     //          
             0x0F12 ,0xD008,     //          
             0x0F12 ,0x61C4,     //          
             0x0F12 ,0x684A,     //          
             0x0F12 ,0x6242,     //          
             0x0F12 ,0x6282,     //          
             0x0F12 ,0x466B,     //          
             0x0F12 ,0x881A,     //          
             0x0F12 ,0x6302,     //          
             0x0F12 ,0x885A,     //          
             0x0F12 ,0x6342,     //          
             0x0F12 ,0x6A02,     //          
             0x0F12 ,0x2A00,     //          
             0x0F12 ,0xD00A,     //          
             0x0F12 ,0x6204,     //          
             0x0F12 ,0x6849,     //          
             0x0F12 ,0x6281,     //          
             0x0F12 ,0x466B,     //          
             0x0F12 ,0x8819,     //          
             0x0F12 ,0x6301,     //          
             0x0F12 ,0x8859,     //          
             0x0F12 ,0x6341,     //          
             0x0F12 ,0x49A5,     //          
             0x0F12 ,0x88C9,     //          
             0x0F12 ,0x63C1,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFB04,     //          
             0x0F12 ,0xE7A6,     //          
             0x0F12 ,0xB5F0,     //          
             0x0F12 ,0xB08B,     //          
             0x0F12 ,0x20FF,     //          
             0x0F12 ,0x1C40,     //          
             0x0F12 ,0x49A1,     //          
             0x0F12 ,0x89CC,     //          
             0x0F12 ,0x4E9E,     //          
             0x0F12 ,0x6AB1,     //          
             0x0F12 ,0x4284,     //          
             0x0F12 ,0xD101,     //          
             0x0F12 ,0x489F,     //          
             0x0F12 ,0x6081,     //          
             0x0F12 ,0x6A70,     //          
             0x0F12 ,0x0200,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFAFB,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x4A96,     //          
             0x0F12 ,0x8A11,     //          
             0x0F12 ,0x9109,     //          
             0x0F12 ,0x2101,     //          
             0x0F12 ,0x0349,     //          
             0x0F12 ,0x4288,     //          
             0x0F12 ,0xD200,     //          
             0x0F12 ,0x0001,     //          
             0x0F12 ,0x4A92,     //          
             0x0F12 ,0x8211,     //          
             0x0F12 ,0x4D97,     //          
             0x0F12 ,0x8829,     //          
             0x0F12 ,0x9108,     //          
             0x0F12 ,0x4A8B,     //          
             0x0F12 ,0x2303,     //          
             0x0F12 ,0x3222,     //          
             0x0F12 ,0x1F91,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFAEC,     //          
             0x0F12 ,0x8028,     //          
             0x0F12 ,0x488E,     //          
             0x0F12 ,0x4987,     //          
             0x0F12 ,0x6BC2,     //          
             0x0F12 ,0x6AC0,     //          
             0x0F12 ,0x4282,     //          
             0x0F12 ,0xD201,     //          
             0x0F12 ,0x8CC8,     //          
             0x0F12 ,0x8028,     //          
             0x0F12 ,0x88E8,     //          
             0x0F12 ,0x9007,     //          
             0x0F12 ,0x2240,     //          
             0x0F12 ,0x4310,     //          
             0x0F12 ,0x80E8,     //          
             0x0F12 ,0x2000,     //          
             0x0F12 ,0x0041,     //          
             0x0F12 ,0x194B,     //          
             0x0F12 ,0x001E,     //          
             0x0F12 ,0x3680,     //          
             0x0F12 ,0x8BB2,     //          
             0x0F12 ,0xAF04,     //          
             0x0F12 ,0x527A,     //          
             0x0F12 ,0x4A7D,     //          
             0x0F12 ,0x188A,     //          
             0x0F12 ,0x8897,     //          
             0x0F12 ,0x83B7,     //          
             0x0F12 ,0x33A0,     //          
             0x0F12 ,0x891F,     //          
             0x0F12 ,0xAE01,     //          
             0x0F12 ,0x5277,     //          
             0x0F12 ,0x8A11,     //          
             0x0F12 ,0x8119,     //          
             0x0F12 ,0x1C40,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x2806,     //          
             0x0F12 ,0xD3E9,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFACD,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFAD3,     //          
             0x0F12 ,0x4F79,     //          
             0x0F12 ,0x37A8,     //          
             0x0F12 ,0x2800,     //          
             0x0F12 ,0xD10A,     //          
             0x0F12 ,0x1FE0,     //          
             0x0F12 ,0x38FD,     //          
             0x0F12 ,0xD001,     //          
             0x0F12 ,0x1CC0,     //          
             0x0F12 ,0xD105,     //          
             0x0F12 ,0x4874,     //          
             0x0F12 ,0x8829,     //          
             0x0F12 ,0x3818,     //          
             0x0F12 ,0x6840,     //          
             0x0F12 ,0x4348,     //          
             0x0F12 ,0x6078,     //          
             0x0F12 ,0x4972,     //          
             0x0F12 ,0x6878,     //          
             0x0F12 ,0x6B89,     //          
             0x0F12 ,0x4288,     //          
             0x0F12 ,0xD300,     //          
             0x0F12 ,0x0008,     //          
             0x0F12 ,0x6078,     //          
             0x0F12 ,0x2000,     //          
             0x0F12 ,0x0041,     //          
             0x0F12 ,0xAA04,     //          
             0x0F12 ,0x5A53,     //          
             0x0F12 ,0x194A,     //          
             0x0F12 ,0x269C,     //          
             0x0F12 ,0x52B3,     //          
             0x0F12 ,0xAB01,     //          
             0x0F12 ,0x5A59,     //          
             0x0F12 ,0x32A0,     //          
             0x0F12 ,0x8111,     //          
             0x0F12 ,0x1C40,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x2806,     //          
             0x0F12 ,0xD3F0,     //          
             0x0F12 ,0x4965,     //          
             0x0F12 ,0x9809,     //          
             0x0F12 ,0x8208,     //          
             0x0F12 ,0x9808,     //          
             0x0F12 ,0x8028,     //          
             0x0F12 ,0x9807,     //          
             0x0F12 ,0x80E8,     //          
             0x0F12 ,0x1FE0,     //          
             0x0F12 ,0x38FD,     //          
             0x0F12 ,0xD13B,     //          
             0x0F12 ,0x4D64,     //          
             0x0F12 ,0x89E8,     //          
             0x0F12 ,0x1FC1,     //          
             0x0F12 ,0x39FF,     //          
             0x0F12 ,0xD136,     //          
             0x0F12 ,0x4C5F,     //          
             0x0F12 ,0x8AE0,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFAA2,     //          
             0x0F12 ,0x0006,     //          
             0x0F12 ,0x8B20,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFAA6,     //          
             0x0F12 ,0x9000,     //          
             0x0F12 ,0x6AA1,     //          
             0x0F12 ,0x6878,     //          
             0x0F12 ,0x1809,     //          
             0x0F12 ,0x0200,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFA79,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x0022,     //          
             0x0F12 ,0x3246,     //          
             0x0F12 ,0x0011,     //          
             0x0F12 ,0x310A,     //          
             0x0F12 ,0x2305,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFA76,     //          
             0x0F12 ,0x66E8,     //          
             0x0F12 ,0x6B23,     //          
             0x0F12 ,0x0002,     //          
             0x0F12 ,0x0031,     //          
             0x0F12 ,0x0018,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFA97,     //          
             0x0F12 ,0x466B,     //          
             0x0F12 ,0x8518,     //          
             0x0F12 ,0x6EEA,     //          
             0x0F12 ,0x6B60,     //          
             0x0F12 ,0x9900,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFA90,     //          
             0x0F12 ,0x466B,     //          
             0x0F12 ,0x8558,     //          
             0x0F12 ,0x0029,     //          
             0x0F12 ,0x980A,     //          
             0x0F12 ,0x3170,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFA91,     //          
             0x0F12 ,0x0028,     //          
             0x0F12 ,0x3060,     //          
             0x0F12 ,0x8A02,     //          
             0x0F12 ,0x4946,     //          
             0x0F12 ,0x3128,     //          
             0x0F12 ,0x808A,     //          
             0x0F12 ,0x8A42,     //          
             0x0F12 ,0x80CA,     //          
             0x0F12 ,0x8A80,     //          
             0x0F12 ,0x8108,     //          
             0x0F12 ,0xB00B,     //          
             0x0F12 ,0xBCF0,     //          
             0x0F12 ,0xBC08,     //          
             0x0F12 ,0x4718,     //          
             0x0F12 ,0xB570,     //          
             0x0F12 ,0x2400,     //          
             0x0F12 ,0x4D46,     //          
             0x0F12 ,0x4846,     //          
             0x0F12 ,0x8881,     //          
             0x0F12 ,0x4846,     //          
             0x0F12 ,0x8041,     //          
             0x0F12 ,0x2101,     //          
             0x0F12 ,0x8001,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFA80,     //          
             0x0F12 ,0x4842,     //          
             0x0F12 ,0x3820,     //          
             0x0F12 ,0x8BC0,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFA83,     //          
             0x0F12 ,0x4B42,     //          
             0x0F12 ,0x220D,     //          
             0x0F12 ,0x0712,     //          
             0x0F12 ,0x18A8,     //          
             0x0F12 ,0x8806,     //          
             0x0F12 ,0x00E1,     //          
             0x0F12 ,0x18C9,     //          
             0x0F12 ,0x81CE,     //          
             0x0F12 ,0x8846,     //          
             0x0F12 ,0x818E,     //          
             0x0F12 ,0x8886,     //          
             0x0F12 ,0x824E,     //          
             0x0F12 ,0x88C0,     //          
             0x0F12 ,0x8208,     //          
             0x0F12 ,0x3508,     //          
             0x0F12 ,0x042D,     //          
             0x0F12 ,0x0C2D,     //          
             0x0F12 ,0x1C64,     //          
             0x0F12 ,0x0424,     //          
             0x0F12 ,0x0C24,     //          
             0x0F12 ,0x2C07,     //          
             0x0F12 ,0xD3EC,     //          
             0x0F12 ,0xE658,     //          
             0x0F12 ,0xB510,     //          
             0x0F12 ,0x4834,     //          
             0x0F12 ,0x4C34,     //          
             0x0F12 ,0x88C0,     //          
             0x0F12 ,0x8060,     //          
             0x0F12 ,0x2001,     //          
             0x0F12 ,0x8020,     //          
             0x0F12 ,0x4831,     //          
             0x0F12 ,0x3820,     //          
             0x0F12 ,0x8BC0,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFA60,     //          
             0x0F12 ,0x88E0,     //          
             0x0F12 ,0x4A31,     //          
             0x0F12 ,0x2800,     //          
             0x0F12 ,0xD003,     //          
             0x0F12 ,0x4930,     //          
             0x0F12 ,0x8849,     //          
             0x0F12 ,0x2900,     //          
             0x0F12 ,0xD009,     //          
             0x0F12 ,0x2001,     //          
             0x0F12 ,0x03C0,     //          
             0x0F12 ,0x8050,     //          
             0x0F12 ,0x80D0,     //          
             0x0F12 ,0x2000,     //          
             0x0F12 ,0x8090,     //          
             0x0F12 ,0x8110,     //          
             0x0F12 ,0xBC10,     //          
             0x0F12 ,0xBC08,     //          
             0x0F12 ,0x4718,     //          
             0x0F12 ,0x8050,     //          
             0x0F12 ,0x8920,     //          
             0x0F12 ,0x80D0,     //          
             0x0F12 ,0x8960,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x1400,     //          
             0x0F12 ,0x8090,     //          
             0x0F12 ,0x89A1,     //          
             0x0F12 ,0x0409,     //          
             0x0F12 ,0x1409,     //          
             0x0F12 ,0x8111,     //          
             0x0F12 ,0x89E3,     //          
             0x0F12 ,0x8A24,     //          
             0x0F12 ,0x2B00,     //          
             0x0F12 ,0xD104,     //          
             0x0F12 ,0x17C3,     //          
             0x0F12 ,0x0F5B,     //          
             0x0F12 ,0x1818,     //          
             0x0F12 ,0x10C0,     //          
             0x0F12 ,0x8090,     //          
             0x0F12 ,0x2C00,     //          
             0x0F12 ,0xD1E6,     //          
             0x0F12 ,0x17C8,     //          
             0x0F12 ,0x0F40,     //          
             0x0F12 ,0x1840,     //          
             0x0F12 ,0x10C0,     //          
             0x0F12 ,0x8110,     //          
             0x0F12 ,0xE7E0,     //          
             0x0F12 ,0xB510,     //          
             0x0F12 ,0x000C,     //          
             0x0F12 ,0x4919,     //          
             0x0F12 ,0x2204,     //          
             0x0F12 ,0x6820,     //          
             0x0F12 ,0x5E8A,     //          
             0x0F12 ,0x0140,     //          
             0x0F12 ,0x1A80,     //          
             0x0F12 ,0x0280,     //          
             0x0F12 ,0x8849,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xFA2E,     //          
             0x0F12 ,0x6020,     //          
             0x0F12 ,0xE7D2,     //          
             0x0F12 ,0x38D4,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x17D0,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x5000,     //          
             0x0F12 ,0xD000,     //          
             0x0F12 ,0x1100,     //          
             0x0F12 ,0xD000,     //          
             0x0F12 ,0x171A,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x4780,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2FCA,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2FC5,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2FC6,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2ED8,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2BD0,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x17E0,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2DE8,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x37E0,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x210C,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x1484,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0xA006,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x0724,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0xA000,     //          
             0x0F12 ,0xD000,     //          
             0x0F12 ,0x2270,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2558,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x146C,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0xB510,     //          
             0x0F12 ,0x000C,     //          
             0x0F12 ,0x49AC,     //          
             0x0F12 ,0x2208,     //          
             0x0F12 ,0x6820,     //          
             0x0F12 ,0x5E8A,     //          
             0x0F12 ,0x0140,     //          
             0x0F12 ,0x1A80,     //          
             0x0F12 ,0x0280,     //          
             0x0F12 ,0x88C9,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF9F4,     //          
             0x0F12 ,0x6020,     //          
             0x0F12 ,0xE798,     //          
             0x0F12 ,0xB5FE,     //          
             0x0F12 ,0x000C,     //          
             0x0F12 ,0x6825,     //          
             0x0F12 ,0x6866,     //          
             0x0F12 ,0x68A0,     //          
             0x0F12 ,0x9001,     //          
             0x0F12 ,0x68E7,     //          
             0x0F12 ,0x1BA8,     //          
             0x0F12 ,0x42B5,     //          
             0x0F12 ,0xDA00,     //          
             0x0F12 ,0x1B70,     //          
             0x0F12 ,0x9000,     //          
             0x0F12 ,0x49A0,     //          
             0x0F12 ,0x48A1,     //          
             0x0F12 ,0x884A,     //          
             0x0F12 ,0x8843,     //          
             0x0F12 ,0x435A,     //          
             0x0F12 ,0x2304,     //          
             0x0F12 ,0x5ECB,     //          
             0x0F12 ,0x0A92,     //          
             0x0F12 ,0x18D2,     //          
             0x0F12 ,0x02D2,     //          
             0x0F12 ,0x0C12,     //          
             0x0F12 ,0x88CB,     //          
             0x0F12 ,0x8880,     //          
             0x0F12 ,0x4343,     //          
             0x0F12 ,0x0A98,     //          
             0x0F12 ,0x2308,     //          
             0x0F12 ,0x5ECB,     //          
             0x0F12 ,0x18C0,     //          
             0x0F12 ,0x02C0,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x0411,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x1409,     //          
             0x0F12 ,0x1400,     //          
             0x0F12 ,0x1A08,     //          
             0x0F12 ,0x4995,     //          
             0x0F12 ,0x39E0,     //          
             0x0F12 ,0x6148,     //          
             0x0F12 ,0x9801,     //          
             0x0F12 ,0x3040,     //          
             0x0F12 ,0x7880,     //          
             0x0F12 ,0x2800,     //          
             0x0F12 ,0xD103,     //          
             0x0F12 ,0x9801,     //          
             0x0F12 ,0x0029,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF9C7,     //          
             0x0F12 ,0x8839,     //          
             0x0F12 ,0x9800,     //          
             0x0F12 ,0x4281,     //          
             0x0F12 ,0xD814,     //          
             0x0F12 ,0x8879,     //          
             0x0F12 ,0x9800,     //          
             0x0F12 ,0x4281,     //          
             0x0F12 ,0xD20C,     //          
             0x0F12 ,0x9801,     //          
             0x0F12 ,0x0029,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF9C3,     //          
             0x0F12 ,0x9801,     //          
             0x0F12 ,0x0029,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF9BF,     //          
             0x0F12 ,0x9801,     //          
             0x0F12 ,0x0029,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF9BB,     //          
             0x0F12 ,0xE003,     //          
             0x0F12 ,0x9801,     //          
             0x0F12 ,0x0029,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF9B6,     //          
             0x0F12 ,0x9801,     //          
             0x0F12 ,0x0032,     //          
             0x0F12 ,0x0039,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF9B9,     //          
             0x0F12 ,0x6020,     //          
             0x0F12 ,0xE5D0,     //          
             0x0F12 ,0xB57C,     //          
             0x0F12 ,0x487F,     //          
             0x0F12 ,0xA901,     //          
             0x0F12 ,0x0004,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF93D,     //          
             0x0F12 ,0x466B,     //          
             0x0F12 ,0x88D9,     //          
             0x0F12 ,0x8898,     //          
             0x0F12 ,0x4B7A,     //          
             0x0F12 ,0x3346,     //          
             0x0F12 ,0x1E9A,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF9B1,     //          
             0x0F12 ,0x4879,     //          
             0x0F12 ,0x4977,     //          
             0x0F12 ,0x3812,     //          
             0x0F12 ,0x3140,     //          
             0x0F12 ,0x8A42,     //          
             0x0F12 ,0x888B,     //          
             0x0F12 ,0x18D2,     //          
             0x0F12 ,0x8242,     //          
             0x0F12 ,0x8AC2,     //          
             0x0F12 ,0x88C9,     //          
             0x0F12 ,0x1851,     //          
             0x0F12 ,0x82C1,     //          
             0x0F12 ,0x0020,     //          
             0x0F12 ,0x4669,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF925,     //          
             0x0F12 ,0x4872,     //          
             0x0F12 ,0x214D,     //          
             0x0F12 ,0x8301,     //          
             0x0F12 ,0x2196,     //          
             0x0F12 ,0x8381,     //          
             0x0F12 ,0x211D,     //          
             0x0F12 ,0x3020,     //          
             0x0F12 ,0x8001,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF99F,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF9A5,     //          
             0x0F12 ,0x486D,     //          
             0x0F12 ,0x4C6D,     //          
             0x0F12 ,0x6E00,     //          
             0x0F12 ,0x60E0,     //          
             0x0F12 ,0x466B,     //          
             0x0F12 ,0x8818,     //          
             0x0F12 ,0x8859,     //          
             0x0F12 ,0x0025,     //          
             0x0F12 ,0x1A40,     //          
             0x0F12 ,0x3540,     //          
             0x0F12 ,0x61A8,     //          
             0x0F12 ,0x4864,     //          
             0x0F12 ,0x9900,     //          
             0x0F12 ,0x3060,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF99D,     //          
             0x0F12 ,0x466B,     //          
             0x0F12 ,0x8819,     //          
             0x0F12 ,0x1DE0,     //          
             0x0F12 ,0x30F9,     //          
             0x0F12 ,0x8741,     //          
             0x0F12 ,0x8859,     //          
             0x0F12 ,0x8781,     //          
             0x0F12 ,0x2000,     //          
             0x0F12 ,0x71A0,     //          
             0x0F12 ,0x74A8,     //          
             0x0F12 ,0xBC7C,     //          
             0x0F12 ,0xBC08,     //          
             0x0F12 ,0x4718,     //          
             0x0F12 ,0xB5F8,     //          
             0x0F12 ,0x0005,     //          
             0x0F12 ,0x6808,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x684A,     //          
             0x0F12 ,0x0412,     //          
             0x0F12 ,0x0C12,     //          
             0x0F12 ,0x688E,     //          
             0x0F12 ,0x68CC,     //          
             0x0F12 ,0x4955,     //          
             0x0F12 ,0x884B,     //          
             0x0F12 ,0x4343,     //          
             0x0F12 ,0x0A98,     //          
             0x0F12 ,0x2304,     //          
             0x0F12 ,0x5ECB,     //          
             0x0F12 ,0x18C0,     //          
             0x0F12 ,0x02C0,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x88CB,     //          
             0x0F12 ,0x4353,     //          
             0x0F12 ,0x0A9A,     //          
             0x0F12 ,0x2308,     //          
             0x0F12 ,0x5ECB,     //          
             0x0F12 ,0x18D1,     //          
             0x0F12 ,0x02C9,     //          
             0x0F12 ,0x0C09,     //          
             0x0F12 ,0x2701,     //          
             0x0F12 ,0x003A,     //          
             0x0F12 ,0x40AA,     //          
             0x0F12 ,0x9200,     //          
             0x0F12 ,0x002A,     //          
             0x0F12 ,0x3A10,     //          
             0x0F12 ,0x4097,     //          
             0x0F12 ,0x2D10,     //          
             0x0F12 ,0xDA06,     //          
             0x0F12 ,0x4A4E,     //          
             0x0F12 ,0x9B00,     //          
             0x0F12 ,0x8812,     //          
             0x0F12 ,0x439A,     //          
             0x0F12 ,0x4B4C,     //          
             0x0F12 ,0x801A,     //          
             0x0F12 ,0xE003,     //          
             0x0F12 ,0x4B4B,     //          
             0x0F12 ,0x885A,     //          
             0x0F12 ,0x43BA,     //          
             0x0F12 ,0x805A,     //          
             0x0F12 ,0x0023,     //          
             0x0F12 ,0x0032,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF945,     //          
             0x0F12 ,0x2D10,     //          
             0x0F12 ,0xDA05,     //          
             0x0F12 ,0x4946,     //          
             0x0F12 ,0x9A00,     //          
             0x0F12 ,0x8808,     //          
             0x0F12 ,0x4310,     //          
             0x0F12 ,0x8008,     //          
             0x0F12 ,0xE003,     //          
             0x0F12 ,0x4843,     //          
             0x0F12 ,0x8841,     //          
             0x0F12 ,0x4339,     //          
             0x0F12 ,0x8041,     //          
             0x0F12 ,0x4D40,     //          
             0x0F12 ,0x2000,     //          
             0x0F12 ,0x3580,     //          
             0x0F12 ,0x88AA,     //          
             0x0F12 ,0x5E30,     //          
             0x0F12 ,0x2100,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF951,     //          
             0x0F12 ,0x8030,     //          
             0x0F12 ,0x2000,     //          
             0x0F12 ,0x88AA,     //          
             0x0F12 ,0x5E20,     //          
             0x0F12 ,0x2100,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF94A,     //          
             0x0F12 ,0x8020,     //          
             0x0F12 ,0xE587,     //          
             0x0F12 ,0xB510,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF94D,     //          
             0x0F12 ,0x4A38,     //          
             0x0F12 ,0x8D50,     //          
             0x0F12 ,0x2800,     //          
             0x0F12 ,0xD007,     //          
             0x0F12 ,0x4933,     //          
             0x0F12 ,0x31C0,     //          
             0x0F12 ,0x684B,     //          
             0x0F12 ,0x4935,     //          
             0x0F12 ,0x4283,     //          
             0x0F12 ,0xD202,     //          
             0x0F12 ,0x8D90,     //          
             0x0F12 ,0x81C8,     //          
             0x0F12 ,0xE6A0,     //          
             0x0F12 ,0x8DD0,     //          
             0x0F12 ,0x81C8,     //          
             0x0F12 ,0xE69D,     //          
             0x0F12 ,0xB5F8,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF942,     //          
             0x0F12 ,0x4D2E,     //          
             0x0F12 ,0x8E28,     //          
             0x0F12 ,0x2800,     //          
             0x0F12 ,0xD01F,     //          
             0x0F12 ,0x4E2E,     //          
             0x0F12 ,0x4829,     //          
             0x0F12 ,0x68B4,     //          
             0x0F12 ,0x6800,     //          
             0x0F12 ,0x4284,     //          
             0x0F12 ,0xD903,     //          
             0x0F12 ,0x1A21,     //          
             0x0F12 ,0x0849,     //          
             0x0F12 ,0x1847,     //          
             0x0F12 ,0xE006,     //          
             0x0F12 ,0x4284,     //          
             0x0F12 ,0xD203,     //          
             0x0F12 ,0x1B01,     //          
             0x0F12 ,0x0849,     //          
             0x0F12 ,0x1A47,     //          
             0x0F12 ,0xE000,     //          
             0x0F12 ,0x0027,     //          
             0x0F12 ,0x0020,     //          
             0x0F12 ,0x4920,     //          
             0x0F12 ,0x3120,     //          
             0x0F12 ,0x7A0C,     //          
             0x0F12 ,0x2C00,     //          
             0x0F12 ,0xD004,     //          
             0x0F12 ,0x0200,     //          
             0x0F12 ,0x0039,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF887,     //          
             0x0F12 ,0x8668,     //          
             0x0F12 ,0x2C00,     //          
             0x0F12 ,0xD000,     //          
             0x0F12 ,0x60B7,     //          
             0x0F12 ,0xE54D,     //          
             0x0F12 ,0x20FF,     //          
             0x0F12 ,0x1C40,     //          
             0x0F12 ,0x8668,     //          
             0x0F12 ,0xE549,     //          
             0x0F12 ,0xB510,     //          
             0x0F12 ,0x000C,     //          
             0x0F12 ,0x6820,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0x4918,     //          
             0x0F12 ,0x8E0A,     //          
             0x0F12 ,0x2A00,     //          
             0x0F12 ,0xD003,     //          
             0x0F12 ,0x8E49,     //          
             0x0F12 ,0x0200,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF871,     //          
             0x0F12 ,0x6020,     //          
             0x0F12 ,0x0400,     //          
             0x0F12 ,0x0C00,     //          
             0x0F12 ,0xE661,     //          
             0x0F12 ,0xB570,     //          
             0x0F12 ,0x680C,     //          
             0x0F12 ,0x4D14,     //          
             0x0F12 ,0x0020,     //          
             0x0F12 ,0x6F29,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF90A,     //          
             0x0F12 ,0x6F69,     //          
             0x0F12 ,0x1D20,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF906,     //          
             0x0F12 ,0x480C,     //          
             0x0F12 ,0x8E00,     //          
             0x0F12 ,0x2800,     //          
             0x0F12 ,0xD006,     //          
             0x0F12 ,0x4907,     //          
             0x0F12 ,0x2214,     //          
             0x0F12 ,0x3168,     //          
             0x0F12 ,0x0008,     //          
             0x0F12 ,0x383C,     //          
             0x0F12 ,0xF000,     //          
             0x0F12 ,0xF903,     //          
             0x0F12 ,0xE488,     //          
             0x0F12 ,0x2558,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2AB8,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x145E,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2698,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2BB8,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x2998,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x1100,     //          
             0x0F12 ,0xD000,     //          
             0x0F12 ,0x4780,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0xE200,     //          
             0x0F12 ,0xD000,     //          
             0x0F12 ,0x210C,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x7000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x1789,     //          
             0x0F12 ,0x0001,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x16F1,     //          
             0x0F12 ,0x0001,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xC3B1,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xC36D,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xF6D7,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xB49D,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x7EDF,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x448D,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xF004,     //          
             0x0F12 ,0xE51F,     //          
             0x0F12 ,0x29EC,     //          
             0x0F12 ,0x0001,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x2EF1,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xEE03,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xA58B,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x7C49,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x7C63,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x2DB7,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xEB3D,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xF061,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xF0EF,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xF004,     //          
             0x0F12 ,0xE51F,     //          
             0x0F12 ,0x2824,     //          
             0x0F12 ,0x0001,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x8EDD,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x8DCB,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x8E17,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x98C5,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x7C7D,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x7E31,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x7EAB,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x7501,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0xF63F,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x3D0B,     //          
             0x0F12 ,0x0000,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xC000,     //          
             0x0F12 ,0xE59F,     //          
             0x0F12 ,0xFF1C,     //          
             0x0F12 ,0xE12F,     //          
             0x0F12 ,0x29BF,     //          
             0x0F12 ,0x0001,     //          
             0x0F12 ,0x4778,     //          
             0x0F12 ,0x46C0,     //          
             0x0F12 ,0xF004,     //          
             0x0F12 ,0xE51F,     //          
             0x0F12 ,0x26D8,     //          
             0x0F12 ,0x0001     //          
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
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
    //                   
    //                   
    //                   
    //                        
    //             
    //              
    //              
    //                                 


    //                                                                                  
    //               
    //                                                                                  
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0722);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100); //                                                                           
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0726);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                           
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08D6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //               
    S5K4ECGX_write_cmos_sensor(0x002A, 0x146E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08DC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                   
    //      
    //            
    //                                                                                          
    //            
    //                                                          
    //            
    //                              
    //            
    //                               
    //            
    //                                  

    S5K4ECGX_write_cmos_sensor(0x0028, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);

    //                                                                                  
    //                    
    //                                                                                  
    {
        static const kal_uint16 addr_data_pair[] =
        {
        //                      
             0x0028, 0x7000,
             0x002A, 0x08B4,
             0x0F12, 0x0001, //                  
             0x002A, 0x08BC,
             0x0F12, 0x00C0, //                            
             0x0F12, 0x00DF, //                            
             0x0F12, 0x0100, //                            
             0x0F12, 0x0125, //                            
             0x0F12, 0x015F, //                            
             0x0F12, 0x017C, //                            
             0x0F12, 0x0194, //                            
             0x002A, 0x08F6,
             0x0F12, 0x5000, //                                   
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                         
             0x0F12, 0x5000, //                                                     
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                         
             0x0F12, 0x4000, //                                   
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                         
             0x0F12, 0x4000, //                                   
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                         
             0x0F12, 0x4000, //                                   
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                         
             0x0F12, 0x4300, //                                   
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                         
             0x0F12, 0x5000, //                                   
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                          
             0x0F12, 0x4000, //                         
        //                 
             0x0F12, 0x4500,
             0x0F12, 0x4000,
             0x0F12, 0x4000,
             0x0F12, 0x4000,
             0x002A, 0x08F4,
             0x0F12, 0x0001, //                


        //                                                                  
        //                        
             0x002A, 0x0D26,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x0F0F,
             0x0F12, 0x0F0F,
             0x0F12, 0x0F00,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x000F,
             0x0F12, 0x0F00,
             0x0F12, 0x0000,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x0F0F,
             0x0F12, 0x0000,
             0x0F12, 0x000F,
             0x0F12, 0x0000,
             0x0F12, 0x000F,
             0x0F12, 0x0F00,
             0x0F12, 0x0F00,
             0x0F12, 0x0000,
             0x0F12, 0x0F0F,
             0x0F12, 0x0F0F,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x0000,
             0x0F12, 0x000F,
             0x0F12, 0x0F00,
             0x0F12, 0x0000,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x0F0F,
             0x0F12, 0x000F,
             0x0F12, 0x000F,
             0x0F12, 0x0000,
             0x0F12, 0x000F,
             0x0F12, 0x0F0F,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x000F,
             0x0F12, 0x000F,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x0F00,
             0x0F12, 0x0000,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x0F0F,
             0x0F12, 0x000F,
             0x0F12, 0x000F,
             0x0F12, 0x0000,
             0x0F12, 0x000F,
             0x0F12, 0x0F0F,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x0F0F,
             0x0F12, 0x0F0F,
             0x0F12, 0x0F00,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x000F,
             0x0F12, 0x0F00,
             0x0F12, 0x0000,
             0x0F12, 0x0F00,
             0x0F12, 0x000F,
             0x0F12, 0x0F0F,
             0x0F12, 0x000F,
             0x0F12, 0x000F,
             0x0F12, 0x0000,
             0x0F12, 0x000F,
             0x0F12, 0x0F0F,

       //                  
             0x002A, 0x0DB6,
             0x0F12, 0x88A2,
             0x0F12, 0xEF5B,
             0x0F12, 0xF576,
             0x0F12, 0x2242,
             0x0F12, 0xEC90,
             0x0F12, 0xFCB2,
             0x0F12, 0xD726,
             0x0F12, 0xF77C,
             0x0F12, 0x1CCB,
             0x0F12, 0xDB4D,
             0x0F12, 0x0948,
             0x0F12, 0x13C2,
             0x0F12, 0x0A14,
             0x0F12, 0x017A,
             0x0F12, 0xE9B4,
             0x0F12, 0x190D,
             0x0F12, 0x16E5,
             0x0F12, 0xCAB2,
             0x0F12, 0x18CD,
             0x0F12, 0x0A84,
             0x0F12, 0x097E,
             0x0F12, 0xF076,
             0x0F12, 0xE849,
             0x0F12, 0x2CFC,
             0x0F12, 0xE460,
             0x0F12, 0xEE89,
             0x0F12, 0x0693,
             0x0F12, 0x06B4,
             0x0F12, 0xF16E,
             0x0F12, 0x12B6,
             0x0F12, 0x0F99,
             0x0F12, 0x0F3B,
             0x0F12, 0xE728,
             0x0F12, 0x19BB,
             0x0F12, 0x058E,
             0x0F12, 0xDA99,
             0x0F12, 0x952B,
             0x0F12, 0xE6F0,
             0x0F12, 0x0163,
             0x0F12, 0x1376,
             0x0F12, 0xFC0E,
             0x0F12, 0xF3A2,
             0x0F12, 0xCE5D,
             0x0F12, 0xFA86,
             0x0F12, 0x11D3,
             0x0F12, 0xEB02,
             0x0F12, 0xFE43,
             0x0F12, 0x17ED,
             0x0F12, 0x1320,
             0x0F12, 0x0156,
             0x0F12, 0xF4FF,
             0x0F12, 0x0ACA,
             0x0F12, 0x162B,
             0x0F12, 0xD2D8,
             0x0F12, 0x0F4F,
             0x0F12, 0x0178,
             0x0F12, 0x0AD1,
             0x0F12, 0xEDE5,
             0x0F12, 0xFBA5,
             0x0F12, 0x1A69,
             0x0F12, 0xF30F,
             0x0F12, 0xFC58,
             0x0F12, 0xF92D,
             0x0F12, 0x131C,
             0x0F12, 0xE607,
             0x0F12, 0x1564,
             0x0F12, 0x02A8,
             0x0F12, 0x08B5,
             0x0F12, 0xF04C,
             0x0F12, 0x15D0,
             0x0F12, 0xFAD0,
             0x0F12, 0xEB70,
             0x0F12, 0x8564,
             0x0F12, 0xE967,
             0x0F12, 0xFFFF,
             0x0F12, 0x16A8,
             0x0F12, 0xEFD6,
             0x0F12, 0x01AF,
             0x0F12, 0xD7AD,
             0x0F12, 0x01A2,
             0x0F12, 0x0A4E,
             0x0F12, 0xF1CE,
             0x0F12, 0xFA95,
             0x0F12, 0x143F,
             0x0F12, 0x1046,
             0x0F12, 0xF6A1,
             0x0F12, 0xF7BB,
             0x0F12, 0x0E8D,
             0x0F12, 0x11A3,
             0x0F12, 0xDB43,
             0x0F12, 0x1459,
             0x0F12, 0x0FFA,
             0x0F12, 0x0731,
             0x0F12, 0xEC67,
             0x0F12, 0xF7CA,
             0x0F12, 0x1682,
             0x0F12, 0xDF77,
             0x0F12, 0xEEA5,
             0x0F12, 0xFF71,
             0x0F12, 0x08FF,
             0x0F12, 0xF8FA,
             0x0F12, 0x138E,
             0x0F12, 0x16FE,
             0x0F12, 0x0BA0,
             0x0F12, 0xF297,
             0x0F12, 0x1717,
             0x0F12, 0xF5BB,
             0x0F12, 0xE6B7,
             0x0F12, 0x87A3,
             0x0F12, 0xECB4,
             0x0F12, 0xF8A1,
             0x0F12, 0x1D23,
             0x0F12, 0xF35F,
             0x0F12, 0xF7C7,
             0x0F12, 0xD9ED,
             0x0F12, 0xF792,
             0x0F12, 0x1E98,
             0x0F12, 0xD734,
             0x0F12, 0x0BA1,
             0x0F12, 0x14E3,
             0x0F12, 0x0BB9,
             0x0F12, 0x0279,
             0x0F12, 0xDEC5,
             0x0F12, 0x2EDC,
             0x0F12, 0x010A,
             0x0F12, 0xD36F,
             0x0F12, 0x1A6A,
             0x0F12, 0x03F6,
             0x0F12, 0x1AE5,
             0x0F12, 0xD3FB,
             0x0F12, 0xFFFA,
             0x0F12, 0x26A0,
             0x0F12, 0xDF98,
             0x0F12, 0xF8DC,
             0x0F12, 0xF675,
             0x0F12, 0x168E,
             0x0F12, 0xEFC9,
             0x0F12, 0x0A42,
             0x0F12, 0x11D3,
             0x0F12, 0x08BE,
             0x0F12, 0xEF30,
             0x0F12, 0x1785,
             0x0F12, 0xFBF7,
             0x0F12, 0xE573,
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
    }
    //                                                                                  
    //                     
    //                                                                                  
    {
        static const kal_uint16 addr_data_pair[] =
        {
        //                                  
        //                                           
        //                                                                  
        //                    
             0x002A, 0x18BC,
             0x0F12, 0x0004,
             0x0F12, 0x05B6,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0001,
             0x0F12, 0x05BA,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0007,
             0x0F12, 0x05BA,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x01F4,
             0x0F12, 0x024E,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x01F4,
             0x0F12, 0x05B6,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x01F4,
             0x0F12, 0x05BA,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x01F4,
             0x0F12, 0x024F,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0075,
             0x0F12, 0x00CF,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0075,
             0x0F12, 0x00D6,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0004,
             0x0F12, 0x01F4,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x00F0,
             0x0F12, 0x01F4,
             0x0F12, 0x029E,
             0x0F12, 0x05B2,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x01F8,
             0x0F12, 0x0228,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0208,
             0x0F12, 0x0238,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0218,
             0x0F12, 0x0238,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0001,
             0x0F12, 0x0009,
             0x0F12, 0x00DE,
             0x0F12, 0x05C0,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x00DF,
             0x0F12, 0x00E4,
             0x0F12, 0x01F8,
             0x0F12, 0x01FD,
             0x0F12, 0x05B6,
             0x0F12, 0x05BB,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x01F8,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0077,
             0x0F12, 0x007E,
             0x0F12, 0x024F,
             0x0F12, 0x025E,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
        //            
             0x0F12, 0x0004,
             0x0F12, 0x09D1,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0001,
             0x0F12, 0x09D5,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0008,
             0x0F12, 0x09D5,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x02AA,
             0x0F12, 0x0326,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x02AA,
             0x0F12, 0x09D1,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x02AA,
             0x0F12, 0x09D5,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x02AA,
             0x0F12, 0x0327,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0008,
             0x0F12, 0x0084,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0008,
             0x0F12, 0x008D,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0008,
             0x0F12, 0x02AA,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x00AA,
             0x0F12, 0x02AA,
             0x0F12, 0x03AD,
             0x0F12, 0x09CD,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x02AE,
             0x0F12, 0x02DE,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x02BE,
             0x0F12, 0x02EE,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x02CE,
             0x0F12, 0x02EE,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0001,
             0x0F12, 0x0009,
             0x0F12, 0x0095,
             0x0F12, 0x09DB,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0096,
             0x0F12, 0x009B,
             0x0F12, 0x02AE,
             0x0F12, 0x02B3,
             0x0F12, 0x09D1,
             0x0F12, 0x09D6,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x02AE,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0009,
             0x0F12, 0x0010,
             0x0F12, 0x0327,
             0x0F12, 0x0336,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x002A, 0x1AF8,
             0x0F12, 0x5A3C, //                                                              
             0x002A, 0x1896,
             0x0F12, 0x0002, //                                          
             0x0F12, 0x0000, //                                        
             0x0F12, 0x0001, //                                                  
             0x002A, 0x1B00,
             0x0F12, 0xF428,
             0x0F12, 0xFFFF,
             0x0F12, 0x0000,
             0x002A, 0x189E,
             0x0F12, 0x0FB0, //                   
             0x002A, 0x18AC,
             0x0F12, 0x0060,   //                  
             0x0F12, 0x0060, //                    
             0x0F12, 0x07DC, //                  
             0x0F12, 0x05C0, //                    
             0x002A, 0x1AEA,
             0x0F12, 0x8080, //                  
             0x0F12, 0x0080, //                   
             0x002A, 0x1AE0,
             0x0F12, 0x0000, //              
             0x002A, 0x1A72,
             0x0F12, 0x0000, //                   
             0x002A, 0x18A2,
             0x0F12, 0x0004, //                                                    
             0x002A, 0x1A6A,
             0x0F12, 0x009A, //                                                           
             0x002A, 0x385E,
             0x0F12, 0x024C, //                     
             0x002A, 0x0EE6,
             0x0F12, 0x0000, //                     
             0x002A, 0x1B2A,
             0x0F12, 0x0300, //                                          
             0x0F12, 0x00D6, //                                         
             0x0F12, 0x008D, //                                         
             0x0F12, 0x00CF, //                                         
             0x0F12, 0x0084, //                                         
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
    }
    //                                                                                  
    //              
    //                                                                                  

    {
        static const kal_uint16 addr_data_pair[] =
        {
        //                    
             0x002A, 0x01FC,
             0x0F12, 0x0001, //                                      
        //         
        //                                              
             0x0F12, 0x0003, //                                       
             0x0F12, 0x0000, //                              
             0x002A, 0x0204,
             0x0F12, 0x0061, //                                                 
             0x002A, 0x020C,
             0x0F12, 0x2F0C, //                           
             0x0F12, 0x0190, //                                          

        //                  
             0x002A, 0x0294,
             0x0F12, 0x0100, //                      
             0x0F12, 0x00E3, //                      
             0x0F12, 0x0200, //                     
             0x0F12, 0x0238, //                     
             0x0F12, 0x018C, //                       
             0x0F12, 0x0166, //                       
             0x0F12, 0x00E6, //                      
             0x0F12, 0x0132, //                      
             0x0F12, 0x0001, //                         
        //                  
             0x002A, 0x070E,
             0x0F12, 0x00C0, //                        
             0x002A, 0x071E,
             0x0F12, 0x0000, //                 
             0x002A, 0x163C,
             0x0F12, 0x0000, //                    
             0x002A, 0x1648,
             0x0F12, 0x9002, //                         
             0x002A, 0x1652,
             0x0F12, 0x0002, //                         
             0x0F12, 0x0000, //                        
             0x002A, 0x15E0,
             0x0F12, 0x0403, //                        
             0x002A, 0x1656,
             0x0F12, 0x0000, //                         
        //              
             0x002A, 0x164C,
             0x0F12, 0x0003, //                          
             0x002A, 0x163E,
             0x0F12, 0x00C0, //                   
             0x0F12, 0x0080, //                      
             0x002A, 0x47A8,
             0x0F12, 0x0080, //                             
        //        
             0x002A, 0x15D4,
             0x0F12, 0x0000, //                
             0x0F12, 0xD000, //                   
        //             
             0x002A, 0x169A,
             0x0F12, 0xFF95, //                             
             0x002A, 0x166A,
             0x0F12, 0x0280, //                      
             0x002A, 0x1676,
             0x0F12, 0x03A0, //                       
             0x0F12, 0x0320, //                       
             0x002A, 0x16BC,
             0x0F12, 0x0030, //                    
             0x002A, 0x16E0,
             0x0F12, 0x0060, //                            
             0x002A, 0x16D4,
             0x0F12, 0x0010, //                   


        //                               
             0x002A, 0x15E8,
             0x0F12, 0x0010, //                     
             0x0F12, 0x0018, //              
             0x0F12, 0x0020, //              
             0x0F12, 0x0028, //              
             0x0F12, 0x0030, //              
             0x0F12, 0x0038, //              
             0x0F12, 0x0040, //              
             0x0F12, 0x0048, //              
             0x0F12, 0x0050, //              
             0x0F12, 0x0058, //              
             0x0F12, 0x0060, //              
             0x0F12, 0x0068, //              
             0x0F12, 0x0070, //              
             0x0F12, 0x0078, //              
             0x0F12, 0x0080, //              
             0x0F12, 0x0088, //              
             0x0F12, 0x0090, //              
             0x0F12, 0x0098, //              


        //                          
             0x002A, 0x1722,
             0x0F12, 0x8000, //                                     
             0x0F12, 0x0006, //                                   
             0x0F12, 0x3FF0, //                            
             0x0F12, 0x03E8, //                         
             0x0F12, 0x0000, //                          
             0x0F12, 0x0020, //                                                               
             0x0F12, 0x0010, //                                   
             0x0F12, 0x0008, //                             
             0x0F12, 0x0040, //                                   
             0x0F12, 0x0080, //                                   
             0x0F12, 0x00C0, //                                    
             0x0F12, 0x00E0, //                                    
             0x002A, 0x028C,
             0x0F12, 0x0003, //               
    //                                                                                  
    //                     
    //                                                                                  

        //                     
             0x002A, 0x145E,
             0x0F12, 0x0580, //                 
             0x0F12, 0x0428, //                 
             0x0F12, 0x07B0, //                 
        //                      
             0x0F12, 0x0008, //                   
             0x0F12, 0x0190, //                   
             0x0F12, 0x00A0, //                 
             0x0F12, 0x0004, //                 
             0x0F12, 0x0002, //               
             0x002A, 0x144E,
             0x0F12, 0x0000, //             
             0x0F12, 0x0000, //             
             0x0F12, 0x0000, //             
             0x0F12, 0x00C2, //                    
             0x0F12, 0x0002, //                       
             0x0F12, 0x0001, //                 
             0x0F12, 0x0074, //                
             0x0F12, 0x0001, //                         
        //            
             0x002A, 0x11F0,
             0x0F12, 0x012C, //          
             0x0F12, 0x0121, //          
             0x0F12, 0x02DF, //            
             0x0F12, 0x0314, //            
             0x002A, 0x120E,
             0x0F12, 0x0000, //                  
             0x0F12, 0x05FD, //                   
             0x0F12, 0x036B, //                    
             0x0F12, 0x0020, //                   
             0x0F12, 0x001A, //                    
             0x002A, 0x1278,
             0x0F12, 0xFEF7, //                                
             0x0F12, 0x0021, //                               
             0x0F12, 0x07D0, //                               
             0x0F12, 0x07D0, //                              
             0x0F12, 0x01C8, //                                  
             0x0F12, 0x0096, //                                   
             0x0F12, 0x0004, //                                      
             0x002A, 0x1224,
             0x0F12, 0x0032, //          
             0x0F12, 0x001E, //                 
             0x0F12, 0x00E2, //                
             0x0F12, 0x0010, //                    
             0x0F12, 0x0002, //                   
             0x002A, 0x2BA4,
             0x0F12, 0x0004, //                  
             0x002A, 0x11FC,
             0x0F12, 0x000C, //                         
             0x002A, 0x1208,
             0x0F12, 0x0020, //                                 
        //            
             0x002A, 0x101C,
             0x0F12, 0x0360, //                                    
             0x0F12, 0x036C, //                                     
             0x0F12, 0x0320, //                                    
             0x0F12, 0x038A, //                                     
             0x0F12, 0x02E8, //                                    
             0x0F12, 0x0380, //                                     
             0x0F12, 0x02BE, //                                    
             0x0F12, 0x035A, //                                     
             0x0F12, 0x0298, //                                    
             0x0F12, 0x0334, //                                     
             0x0F12, 0x0272, //                                    
             0x0F12, 0x030E, //                                     
             0x0F12, 0x024C, //                                    
             0x0F12, 0x02EA, //                                     
             0x0F12, 0x0230, //                                    
             0x0F12, 0x02CC, //                                     
             0x0F12, 0x0214, //                                    
             0x0F12, 0x02B0, //                                     
             0x0F12, 0x01F8, //                                    
             0x0F12, 0x0294, //                                     
             0x0F12, 0x01DC, //                                     
             0x0F12, 0x0278, //                                      
             0x0F12, 0x01C0, //                                     
             0x0F12, 0x0264, //                                      
             0x0F12, 0x01AA, //                                     
             0x0F12, 0x0250, //                                      
             0x0F12, 0x0196, //                                     
             0x0F12, 0x023C, //                                      
             0x0F12, 0x0180, //                                     
             0x0F12, 0x0228, //                                      
             0x0F12, 0x016C, //                                     
             0x0F12, 0x0214, //                                      
             0x0F12, 0x0168, //                                     
             0x0F12, 0x0200, //                                      
             0x0F12, 0x0172, //                                     
             0x0F12, 0x01EC, //                                      
             0x0F12, 0x019A, //                                     
             0x0F12, 0x01D8, //                                      
             0x0F12, 0x0000, //                                     
             0x0F12, 0x0000, //                                      
             0x0F12, 0x0005, //                             
             0x002A, 0x1070,
             0x0F12, 0x0013, //                                 
             0x002A, 0x1074,
             0x0F12, 0x00EC, //                          
        //             
             0x002A, 0x1078,
             0x0F12, 0x0232, //                                     
             0x0F12, 0x025A, //                                      
             0x0F12, 0x021E, //                                     
             0x0F12, 0x0274, //                                      
             0x0F12, 0x020E, //                                     
             0x0F12, 0x028E, //                                      
             0x0F12, 0x0200, //                                     
             0x0F12, 0x0290, //                                      
             0x0F12, 0x01F4, //                                     
             0x0F12, 0x0286, //                                      
             0x0F12, 0x01E8, //                                     
             0x0F12, 0x027E, //                                      
             0x0F12, 0x01DE, //                                     
             0x0F12, 0x0274, //                                      
             0x0F12, 0x01D2, //                                     
             0x0F12, 0x0268, //                                      
             0x0F12, 0x01D0, //                                     
             0x0F12, 0x025E, //                                      
             0x0F12, 0x01D6, //                                     
             0x0F12, 0x0252, //                                      
             0x0F12, 0x01E2, //                                      
             0x0F12, 0x0248, //                                       
             0x0F12, 0x01F4, //                                      
             0x0F12, 0x021A, //                                       
             0x0F12, 0x0004, //                              
             0x002A, 0x10AC,
             0x0F12, 0x000C, //                                  
             0x002A, 0x10B0,
             0x0F12, 0x01DA, //                           
        //                    
             0x002A, 0x10B4,
             0x0F12, 0x0348, //                                   
             0x0F12, 0x03B6, //                                    
             0x0F12, 0x02B8, //                                   
             0x0F12, 0x03B6, //                                    
             0x0F12, 0x0258, //                                   
             0x0F12, 0x038E, //                                    
             0x0F12, 0x0212, //                                   
             0x0F12, 0x0348, //                                    
             0x0F12, 0x01CC, //                                   
             0x0F12, 0x030C, //                                    
             0x0F12, 0x01A2, //                                   
             0x0F12, 0x02D2, //                                    
             0x0F12, 0x0170, //                                   
             0x0F12, 0x02A6, //                                    
             0x0F12, 0x014C, //                                   
             0x0F12, 0x0280, //                                    
             0x0F12, 0x0128, //                                   
             0x0F12, 0x025C, //                                    
             0x0F12, 0x0146, //                                   
             0x0F12, 0x0236, //                                    
             0x0F12, 0x0164, //                                    
             0x0F12, 0x0212, //                                     
             0x0F12, 0x0000, //                                    
             0x0F12, 0x0000, //                                     
             0x0F12, 0x0006, //                            
             0x002A, 0x10E8,
             0x0F12, 0x000B, //                                
             0x002A, 0x10EC,
             0x0F12, 0x00D2, //                         

        //               
             0x002A, 0x10F0,
             0x0F12, 0x039A,
             0x0F12, 0x0000, //                 
             0x0F12, 0x00FE,
             0x0F12, 0x0000, //                 
             0x0F12, 0x2284,
             0x0F12, 0x0000, //                   

        //                    
             0x002A, 0x1434,
             0x0F12, 0x02C1, //                   
             0x0F12, 0x033A, //                   
             0x0F12, 0x038A, //                   
             0x0F12, 0x101A, //                   
             0x0F12, 0x1075, //                   
             0x0F12, 0x113D, //                   
             0x0F12, 0x113F, //                   
             0x0F12, 0x11AF, //                   
             0x0F12, 0x11F0, //                   
             0x0F12, 0x00B2, //                  
             0x0F12, 0x00B8, //                  
             0x0F12, 0x00CA, //                  
             0x0F12, 0x009D, //                  

        //                   
             0x002A, 0x13A4,
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     
             0x0F12, 0x0000, //                     

        //                    
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
#if 0
             0x002A, 0x13A4,
             0x0F12, 0xFFE0, //                     
             0x0F12, 0xFFE0, //                     
             0x0F12, 0xFFE0, //                     
             0x0F12, 0xFFA0, //                     
             0x0F12, 0xFFEE, //                     
             0x0F12, 0x0096, //                     
             0x0F12, 0xFFE0, //                     
             0x0F12, 0xFFE0, //                     
             0x0F12, 0xFFE0, //                     
             0x0F12, 0xFFA0, //                     
             0x0F12, 0xFFEE, //                     
             0x0F12, 0x0096, //                     
             0x0F12, 0xFFE0, //                     
             0x0F12, 0xFFE0, //                     
             0x0F12, 0xFFE0, //                     
             0x0F12, 0xFFA0, //                     
             0x0F12, 0xFFEE, //                     
             0x0F12, 0x0096, //                     
             0x0F12, 0xFFC0, //                     
             0x0F12, 0xFFC0, //                     
             0x0F12, 0xFFC0, //                     
             0x0F12, 0xFF38, //                     
             0x0F12, 0xFEF2, //                     
             0x0F12, 0xFE5C, //                     
             0x0F12, 0xFFC0, //                     
             0x0F12, 0xFFC0, //                     
             0x0F12, 0xFFC0, //                     
             0x0F12, 0xFF38, //                     
             0x0F12, 0xFEF2, //                     
             0x0F12, 0xFE5C, //                     
             0x0F12, 0xFFC0, //                     
             0x0F12, 0xFFC0, //                     
             0x0F12, 0xFFC0, //                     
             0x0F12, 0xFF38, //                     
             0x0F12, 0xFEF2, //                     
             0x0F12, 0xFE5C, //                     

        //                    
             0x0F12, 0xFFC0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0xFFC0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0xFFC0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0010, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0010, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0010, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0xFFD0, //                         
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                         
#endif
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
    }

    S5K4ECGX_MIPI_AF_Init();

    //                                                                                  
    //                          
    //                                                                                  
    {
        static const kal_uint16 addr_data_pair[] =
        {
             0x002A, 0x0F30,
             0x0F12, 0x0001, //                      
        //                          
             0x002A, 0x0F2A,
             0x0F12, 0x0000,  //                                    
             0x002A, 0x04E6,
             0x0F12, 0x077F,  //          

    //                                                                                  
    //              
    //                                                                                  
        //         
             0x002A, 0x1484,
             0x0F12, 0x0030,   //             
        //                                                                                                
             0x002A, 0x148A,
             0x0F12, 0x000F,   //           
             0x002A, 0x058C,
             0x0F12, 0x3520,
             0x0F12, 0x0000,   //           
             0x0F12, 0xD4C0,
             0x0F12, 0x0001,   //           
             0x0F12, 0x3520,
             0x0F12, 0x0000,   //              
             0x0F12, 0xD4C0,
             0x0F12, 0x0001,   //              
             0x002A, 0x059C,
             0x0F12, 0x0470,   //              
             0x0F12, 0x0C00,   //              
             0x0F12, 0x0100,   //              
             0x0F12, 0x1000,   //              
             0x002A, 0x0544,
             0x0F12, 0x0111,   //             
             0x0F12, 0x00EF,   //            
             0x002A, 0x0608,
             0x0F12, 0x0001,   //                           
             0x0F12, 0x0001,   //                          
             0x0F12, 0x0800,   //                             
             0x0F12, 0x0100,   //                                                     
             0x0F12, 0x0001,   //    
             0x0F12, 0x0000,   //                                                    
             0x0F12, 0x0A3C,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0x0D05,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0x4008,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0x7000,   //                   
             0x0F12, 0x0000,   //    
             0x0F12, 0x9C00,   //                                  
             0x0F12, 0x0000,   //    
             0x0F12, 0xAD00,   //    
             0x0F12, 0x0001,   //    
             0x0F12, 0xF1D4,   //    
             0x0F12, 0x0002,   //    
             0x0F12, 0xDC00,   //    
             0x0F12, 0x0005,   //    
             0x0F12, 0xDC00,   //    
             0x0F12, 0x0005,   //               
             0x002A, 0x0638,   //    
             0x0F12, 0x0001,   //    
             0x0F12, 0x0000,   //                                                     
             0x0F12, 0x0A3C,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0x0D05,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0x3408,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0x3408,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0x6810,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0x8214,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0xC350,   //    
             0x0F12, 0x0000,   //    
             0x0F12, 0xD4C0,   //    
             0x0F12, 0x0001,   //    
             0x0F12, 0xD4C0,   //    
             0x0F12, 0x0001,   //    
             0x002A, 0x0660,
             0x0F12, 0x0650,   //                                
             0x0F12, 0x0100,   //                                            
             0x002A, 0x06B8,
             0x0F12, 0x452C,
             0x0F12, 0x000A,   //                   
             0x002A, 0x05D0,
             0x0F12, 0x0000,   //                  
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
    }

    //                                                                                  
    //                      
    //                                                                                  
    S5K4ECGX_MIPI_AE_Rollback_Weight_Table();

    {
        static const kal_uint16 addr_data_pair[] =
        {
             //                                                                                  
             //                 
             //                                                                                  
             0x002A, 0x0484,
             0x0F12, 0x0002,     //                
             0x002A, 0x183A,
             0x0F12, 0x0001,     //            
             0x002A, 0x17F6,
             0x0F12, 0x023C,     //           
             0x0F12, 0x0248,     //           
             0x002A, 0x1840,
             0x0F12, 0x0001,     //                  
             0x0F12, 0x0100,     //                     
             0x0F12, 0x0120,
             0x0F12, 0x0180,
             0x0F12, 0x0200,
             0x0F12, 0x0400,
             0x0F12, 0x0800,
             0x0F12, 0x0A00,
             0x0F12, 0x1000,
             0x0F12, 0x0100,     //                       
             0x0F12, 0x00A0,
             0x0F12, 0x0090,
             0x0F12, 0x0080,
             0x0F12, 0x0070,
             0x0F12, 0x0045,
             0x0F12, 0x0030,
             0x0F12, 0x0010,
             0x002A, 0x1884,
             0x0F12, 0x0100,     //                                   
             0x0F12, 0x0100,
             0x0F12, 0x0100,
             0x0F12, 0x0100,
             0x0F12, 0x0100,
             0x0F12, 0x0100,
             0x0F12, 0x0100,
             0x0F12, 0x0100,
             0x002A, 0x1826,
             0x0F12, 0x0100,     //                                         
             0x0F12, 0x00C0,
             0x0F12, 0x0080,
             0x0F12, 0x000A,
             0x0F12, 0x0000,
             0x0F12, 0x0030,     //                                         
             0x0F12, 0x0040,
             0x0F12, 0x0048,
             0x0F12, 0x0050,
             0x0F12, 0x0060,
             0x002A, 0x4784,
             0x0F12, 0x00A0,     //                                            
             0x0F12, 0x00C0,
             0x0F12, 0x00D0,
             0x0F12, 0x0100,
             0x0F12, 0x0200,
             0x0F12, 0x0300,
             0x0F12, 0x0088,     //                                              
             0x0F12, 0x00B0,
             0x0F12, 0x00C0,
             0x0F12, 0x0100,
             0x0F12, 0x0200,
             0x0F12, 0x0300,
             0x0F12, 0x0120,     //         
             0x0F12, 0x0150,
             0x0F12, 0x0200,
             0x0F12, 0x003C,     //           
             0x0F12, 0x003B,
             0x0F12, 0x0026,     //          



    //                                                                                  
    //               
    //                                                                                  
             0x002A, 0x08A6,
             0x0F12, 0x00C0, //                  
             0x0F12, 0x0100, //                  
             0x0F12, 0x0125, //                  
             0x0F12, 0x015F, //                  
             0x0F12, 0x017C, //                  
             0x0F12, 0x0194, //                  
             0x002A, 0x0898,
             0x0F12, 0x4800, //                  
             0x0F12, 0x7000,
             0x002A, 0x08A0,
             0x0F12, 0x48D8, //                    
             0x0F12, 0x7000,
        //       
             0x002A, 0x4800,
             0x0F12, 0x0208, //                     
             0x0F12, 0xFFB5, //                     
             0x0F12, 0xFFE8, //                     
             0x0F12, 0xFF20, //                     
             0x0F12, 0x01BF, //                     
             0x0F12, 0xFF53, //                     
             0x0F12, 0x0022, //                     
             0x0F12, 0xFFEA, //                     
             0x0F12, 0x01C2, //                     
             0x0F12, 0x00C6, //                     
             0x0F12, 0x0095, //                      
             0x0F12, 0xFEFD, //                      
             0x0F12, 0x0206, //                      
             0x0F12, 0xFF7F, //                      
             0x0F12, 0x0191, //                      
             0x0F12, 0xFF06, //                      
             0x0F12, 0x01BA, //                      
             0x0F12, 0x0108, //                      

        //       
             0x0F12, 0x0208, //                      
             0x0F12, 0xFFB5, //                      
             0x0F12, 0xFFE8, //                      
             0x0F12, 0xFF20, //                      
             0x0F12, 0x01BF, //                      
             0x0F12, 0xFF53, //                      
             0x0F12, 0x0022, //                      
             0x0F12, 0xFFEA, //                      
             0x0F12, 0x01C2, //                      
             0x0F12, 0x00C6, //                      
             0x0F12, 0x0095, //                      
             0x0F12, 0xFEFD, //                      
             0x0F12, 0x0206, //                      
             0x0F12, 0xFF7F, //                      
             0x0F12, 0x0191, //                      
             0x0F12, 0xFF06, //                      
             0x0F12, 0x01BA, //                      
             0x0F12, 0x0108, //                      
        //          
             0x0F12, 0x0208, //                      
             0x0F12, 0xFFB5, //                      
             0x0F12, 0xFFE8, //                      
             0x0F12, 0xFF20, //                      
             0x0F12, 0x01BF, //                      
             0x0F12, 0xFF53, //                      
             0x0F12, 0x0022, //                      
             0x0F12, 0xFFEA, //                      
             0x0F12, 0x01C2, //                      
             0x0F12, 0x00C6, //                      
             0x0F12, 0x0095, //                      
             0x0F12, 0xFEFD, //                      
             0x0F12, 0x0206, //                      
             0x0F12, 0xFF7F, //                      
             0x0F12, 0x0191, //                      
             0x0F12, 0xFF06, //                      
             0x0F12, 0x01BA, //                      
             0x0F12, 0x0108, //                      
        //          
             0x0F12, 0x0204, //                      
             0x0F12, 0xFFB2, //                      
             0x0F12, 0xFFF5, //                      
             0x0F12, 0xFEF1, //                      
             0x0F12, 0x014E, //                      
             0x0F12, 0xFF18, //                      
             0x0F12, 0xFFE6, //                      
             0x0F12, 0xFFDD, //                      
             0x0F12, 0x01B2, //                      
             0x0F12, 0x00F2, //                      
             0x0F12, 0x00CA, //                      
             0x0F12, 0xFF48, //                      
             0x0F12, 0x0151, //                      
             0x0F12, 0xFF50, //                      
             0x0F12, 0x0147, //                      
             0x0F12, 0xFF75, //                      
             0x0F12, 0x0187, //                      
             0x0F12, 0x01BF, //                      
        //   
             0x0F12, 0x0204, //                      
             0x0F12, 0xFFB2, //                      
             0x0F12, 0xFFF5, //                      
             0x0F12, 0xFEF1, //                      
             0x0F12, 0x014E, //                      
             0x0F12, 0xFF18, //                      
             0x0F12, 0xFFE6, //                      
             0x0F12, 0xFFDD, //                      
             0x0F12, 0x01B2, //                      
             0x0F12, 0x00F2, //                      
             0x0F12, 0x00CA, //                      
             0x0F12, 0xFF48, //                      
             0x0F12, 0x0151, //                      
             0x0F12, 0xFF50, //                      
             0x0F12, 0x0147, //                      
             0x0F12, 0xFF75, //                      
             0x0F12, 0x0187, //                      
             0x0F12, 0x01BF, //                      
        //   
             0x0F12, 0x0204, //                      
             0x0F12, 0xFFB2, //                      
             0x0F12, 0xFFF5, //                      
             0x0F12, 0xFEF1, //                      
             0x0F12, 0x014E, //                      
             0x0F12, 0xFF18, //                      
             0x0F12, 0xFFE6, //                      
             0x0F12, 0xFFDD, //                      
             0x0F12, 0x01B2, //                      
             0x0F12, 0x00F2, //                      
             0x0F12, 0x00CA, //                       
             0x0F12, 0xFF48, //                       
             0x0F12, 0x0151, //                       
             0x0F12, 0xFF50, //                       
             0x0F12, 0x0147, //                       
             0x0F12, 0xFF75, //                       
             0x0F12, 0x0187, //                       
             0x0F12, 0x01BF, //                       
        //       
             0x0F12, 0x01E5, //                       
             0x0F12, 0xFFA4, //                       
             0x0F12, 0xFFDC, //                       
             0x0F12, 0xFE90, //                       
             0x0F12, 0x013F, //                       
             0x0F12, 0xFF1B, //                       
             0x0F12, 0xFFD2, //                       
             0x0F12, 0xFFDF, //                       
             0x0F12, 0x0236, //                       
             0x0F12, 0x00EC, //                       
             0x0F12, 0x00F8, //                        
             0x0F12, 0xFF34, //                        
             0x0F12, 0x01CE, //                        
             0x0F12, 0xFF83, //                        
             0x0F12, 0x0195, //                        
             0x0F12, 0xFEF3, //                        
             0x0F12, 0x0126, //                        
             0x0F12, 0x0162, //                        

    //                                                                                  
    //         
    //                                                                                  
             0x002A,  0x0734,
             0x0F12,  0x0000,   //                                          
             0x0F12,  0x0007,   //                                          
             0x0F12,  0x0012,   //                                          
             0x0F12,  0x0028,   //                                          
             0x0F12,  0x0050,   //                                          
             0x0F12,  0x00D5,   //                                          
             0x0F12,  0x0138,   //                                          
             0x0F12,  0x0163,   //                                          
             0x0F12,  0x0189,   //                                          
             0x0F12,  0x01C6,   //                                          
             0x0F12,  0x01F8,   //                                           
             0x0F12,  0x0222,   //                                           
             0x0F12,  0x0247,   //                                           
             0x0F12,  0x0282,   //                                           
             0x0F12,  0x02B5,   //                                           
             0x0F12,  0x0305,   //                                           
             0x0F12,  0x034C,   //                                           
             0x0F12,  0x0388,   //                                           
             0x0F12,  0x03B8,   //                                           
             0x0F12,  0x03E8,   //                                           
             0x0F12,  0x0000,   //                                          
             0x0F12,  0x0007,   //                                          
             0x0F12,  0x0012,   //                                          
             0x0F12,  0x0028,   //                                          
             0x0F12,  0x0050,   //                                          
             0x0F12,  0x00D5,   //                                          
             0x0F12,  0x0138,   //                                          
             0x0F12,  0x0163,   //                                          
             0x0F12,  0x0189,   //                                          
             0x0F12,  0x01C6,   //                                          
             0x0F12,  0x01F8,   //                                           
             0x0F12,  0x0222,   //                                           
             0x0F12,  0x0247,   //                                           
             0x0F12,  0x0282,   //                                           
             0x0F12,  0x02B5,   //                                           
             0x0F12,  0x0305,   //                                           
             0x0F12,  0x034C,   //                                           
             0x0F12,  0x0388,   //                                           
             0x0F12,  0x03B8,   //                                           
             0x0F12,  0x03E8,   //                                           
             0x0F12,  0x0000,   //                                          
             0x0F12,  0x0007,   //                                          
             0x0F12,  0x0012,   //                                          
             0x0F12,  0x0028,   //                                          
             0x0F12,  0x0050,   //                                          
             0x0F12,  0x00D5,   //                                          
             0x0F12,  0x0138,   //                                          
             0x0F12,  0x0163,   //                                          
             0x0F12,  0x0189,   //                                          
             0x0F12,  0x01C6,   //                                          
             0x0F12,  0x01F8,   //                                           
             0x0F12,  0x0222,   //                                           
             0x0F12,  0x0247,   //                                           
             0x0F12,  0x0282,   //                                           
             0x0F12,  0x02B5,   //                                           
             0x0F12,  0x0305,   //                                           
             0x0F12,  0x034C,   //                                           
             0x0F12,  0x0388,   //                                           
             0x0F12,  0x03B8,   //                                           
             0x0F12,  0x03E8,   //                                           
             0x0F12,  0x0000,  //                                   
             0x0F12,  0x000B,  //                                   
             0x0F12,  0x0019,  //                                   
             0x0F12,  0x0036,  //                                   
             0x0F12,  0x006F,  //                                   
             0x0F12,  0x00D8,  //                                   
             0x0F12,  0x0135,  //                                   
             0x0F12,  0x015F,  //                                   
             0x0F12,  0x0185,  //                                   
             0x0F12,  0x01C1,  //                                   
             0x0F12,  0x01F3,  //                                    
             0x0F12,  0x0220,  //                                    
             0x0F12,  0x024A,  //                                    
             0x0F12,  0x0291,  //                                    
             0x0F12,  0x02D0,  //                                    
             0x0F12,  0x032A,  //                                    
             0x0F12,  0x036A,  //                                    
             0x0F12,  0x039F,  //                                    
             0x0F12,  0x03CC,  //                                    
             0x0F12,  0x03F9,  //                                    
             0x0F12,  0x0000,  //                                   
             0x0F12,  0x000B,  //                                   
             0x0F12,  0x0019,  //                                   
             0x0F12,  0x0036,  //                                   
             0x0F12,  0x006F,  //                                   
             0x0F12,  0x00D8,  //                                   
             0x0F12,  0x0135,  //                                   
             0x0F12,  0x015F,  //                                   
             0x0F12,  0x0185,  //                                   
             0x0F12,  0x01C1,  //                                   
             0x0F12,  0x01F3,  //                                    
             0x0F12,  0x0220,  //                                    
             0x0F12,  0x024A,  //                                    
             0x0F12,  0x0291,  //                                    
             0x0F12,  0x02D0,  //                                    
             0x0F12,  0x032A,  //                                    
             0x0F12,  0x036A,  //                                    
             0x0F12,  0x039F,  //                                    
             0x0F12,  0x03CC,  //                                    
             0x0F12,  0x03F9,  //                                    
             0x0F12,  0x0000,  //                                   
             0x0F12,  0x000B,  //                                   
             0x0F12,  0x0019,  //                                   
             0x0F12,  0x0036,  //                                   
             0x0F12,  0x006F,  //                                   
             0x0F12,  0x00D8,  //                                   
             0x0F12,  0x0135,  //                                   
             0x0F12,  0x015F,  //                                   
             0x0F12,  0x0185,  //                                   
             0x0F12,  0x01C1,  //                                   
             0x0F12,  0x01F3,  //                                    
             0x0F12,  0x0220,  //                                    
             0x0F12,  0x024A,  //                                    
             0x0F12,  0x0291,  //                                    
             0x0F12,  0x02D0,  //                                    
             0x0F12,  0x032A,  //                                    
             0x0F12,  0x036A,  //                                    
             0x0F12,  0x039F,  //                                    
             0x0F12,  0x03CC,  //                                    
             0x0F12,  0x03F9,  //                                    



    //                                                                                  
    //        
    //                                                                                  
             0x002A, 0x0944,
             0x0F12, 0x0050, //                    
             0x0F12, 0x00B0, //                    
             0x0F12, 0x0196, //                    
             0x0F12, 0x0245, //                    
             0x0F12, 0x0300, //                    
             0x002A, 0x0938,
             0x0F12, 0x0000, //                         
             0x0F12, 0x0014, //                  
             0x0F12, 0x00D2, //                  
             0x0F12, 0x0384, //                  
             0x0F12, 0x07D0, //                  
             0x0F12, 0x1388, //                  
             0x002A, 0x0976,
             0x0F12, 0x0070, //              
             0x0F12, 0x0005, //                     
             0x0F12, 0x0000, //               
             0x0F12, 0x01CC, //                
             0x0F12, 0x01CC, //                
             0x0F12, 0x01CC, //                
             0x0F12, 0x01CC, //                
             0x0F12, 0x01CC, //                
             0x0F12, 0x0180, //                        
             0x0F12, 0x0196, //                 
             0x002A, 0x098C,
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                        
             0x0F12, 0x00C0, //                              
             0x0F12, 0x0064, //                                    
             0x0F12, 0x0384, //                                     
             0x0F12, 0x005F, //                                 
             0x0F12, 0x01F4, //                                  
             0x0F12, 0x0070, //                               
             0x0F12, 0x0040, //                                     
             0x0F12, 0x00A0, //                                     
             0x0F12, 0x0100, //                                     
             0x0F12, 0x0010, //                                     
             0x0F12, 0x0040, //                                     
             0x0F12, 0x00A0, //                                     
             0x0F12, 0x1430, //                                                         
             0x0F12, 0x0201, //                                                              
             0x0F12, 0x0204, //                                                                   
             0x0F12, 0x3604, //                                                               
             0x0F12, 0x032A, //                                                                
             0x0F12, 0x0403, //                                                                   
             0x0F12, 0x1B06, //                                                                 
             0x0F12, 0x6015, //                                                                      
             0x0F12, 0x00C0, //                                                                   
             0x0F12, 0x6080, //                                                                     
             0x0F12, 0x4080, //                                                                             
             0x0F12, 0x0640, //                                                                            
             0x0F12, 0x0306, //                                                                        
             0x0F12, 0x2003, //                                                                 
             0x0F12, 0xFF01, //                                                                        
             0x0F12, 0x0000, //                                                                   
             0x0F12, 0x0400, //                                                              
             0x0F12, 0x365A, //                                                           
             0x0F12, 0x102A, //                                                            
             0x0F12, 0x000B, //                                                         
             0x0F12, 0x0600, //                                                       
             0x0F12, 0x5A0F, //                                                            
             0x0F12, 0x0505, //                                                                 
             0x0F12, 0x1802, //                                                         
             0x0F12, 0x0000, //                                                             
             0x0F12, 0x2006, //                                                             
             0x0F12, 0x3028, //                                                              
             0x0F12, 0x0418, //                                                      
             0x0F12, 0x0101, //                                                              
             0x0F12, 0x0800, //                                                               
             0x0F12, 0x1804, //                                                                
             0x0F12, 0x4008, //                                                                 
             0x0F12, 0x0540, //                                                               
             0x0F12, 0x8006, //                                                               
             0x0F12, 0x0020, //                                                                
             0x0F12, 0x0000, //                                                                       
             0x0F12, 0x1800, //                                                                   
             0x0F12, 0x0000, //                                                                      
             0x0F12, 0x1E10, //                                                             
             0x0F12, 0x000B, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0005, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0405, //                                                                    
             0x0F12, 0x0205, //                                                                   
             0x0F12, 0x0304, //                                                                     
             0x0F12, 0x0409, //                                                                     
             0x0F12, 0x0306, //                                                                          
             0x0F12, 0x0407, //                                                                             
             0x0F12, 0x1C04, //                                                                              
             0x0F12, 0x0214, //                                                                           
             0x0F12, 0x1002, //                                                                            
             0x0F12, 0x0610, //                                                                            
             0x0F12, 0x1A02, //                                                                            
             0x0F12, 0x4A18, //                                                                            
             0x0F12, 0x0080, //                                                                      
             0x0F12, 0x0048, //                                                                    
             0x0F12, 0x0180, //                                                              
             0x0F12, 0x0A0A, //                                                             
             0x0F12, 0x0101, //                                                                   
             0x0F12, 0x2A36, //                                                                  
             0x0F12, 0x6024, //                                                                  
             0x0F12, 0x2A36, //                                                              
             0x0F12, 0xFFFF, //                                                                  
             0x0F12, 0x0808, //                                                                       
             0x0F12, 0x0A01, //                                                                        
             0x0F12, 0x010A, //                                                                       
             0x0F12, 0x3601, //                                                                          
             0x0F12, 0x242A, //                                                                          
             0x0F12, 0x3660, //                                                                         
             0x0F12, 0xFF2A, //                                                                         
             0x0F12, 0x08FF, //                                                                              
             0x0F12, 0x0008, //                                             
             0x0F12, 0x0001, //                                        
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                        
             0x0F12, 0x00C0, //                              
             0x0F12, 0x0064, //                                    
             0x0F12, 0x0384, //                                     
             0x0F12, 0x0051, //                                 
             0x0F12, 0x01F4, //                                  
             0x0F12, 0x0070, //                               
             0x0F12, 0x0040, //                                     
             0x0F12, 0x00A0, //                                     
             0x0F12, 0x0100, //                                     
             0x0F12, 0x0010, //                                     
             0x0F12, 0x0060, //                                     
             0x0F12, 0x0100, //                                     
             0x0F12, 0x1430, //                                                         
             0x0F12, 0x0201, //                                                              
             0x0F12, 0x0204, //                                                                   
             0x0F12, 0x2404, //                                                               
             0x0F12, 0x031B, //                                                                
             0x0F12, 0x0103, //                                                                   
             0x0F12, 0x1205, //                                                                 
             0x0F12, 0x400D, //                                                                      
             0x0F12, 0x0080, //                                                                   
             0x0F12, 0x2080, //                                                                     
             0x0F12, 0x3040, //                                                                             
             0x0F12, 0x0630, //                                                                            
             0x0F12, 0x0306, //                                                                        
             0x0F12, 0x2003, //                                                                 
             0x0F12, 0xFF01, //                                                                        
             0x0F12, 0x0404, //                                                                   
             0x0F12, 0x0300, //                                                              
             0x0F12, 0x245A, //                                                           
             0x0F12, 0x1018, //                                                            
             0x0F12, 0x000B, //                                                         
             0x0F12, 0x0B00, //                                                       
             0x0F12, 0x5A0F, //                                                            
             0x0F12, 0x0505, //                                                                 
             0x0F12, 0x1802, //                                                         
             0x0F12, 0x0000, //                                                             
             0x0F12, 0x2006, //                                                             
             0x0F12, 0x3428, //                                                              
             0x0F12, 0x041C, //                                                      
             0x0F12, 0x0101, //                                                              
             0x0F12, 0x0800, //                                                               
             0x0F12, 0x1004, //                                                                
             0x0F12, 0x4008, //                                                                 
             0x0F12, 0x0540, //                                                               
             0x0F12, 0x8006, //                                                               
             0x0F12, 0x0020, //                                                                
             0x0F12, 0x0000, //                                                                       
             0x0F12, 0x1800, //                                                                   
             0x0F12, 0x0000, //                                                                      
             0x0F12, 0x1E10, //                                                             
             0x0F12, 0x000B, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0005, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0405, //                                                                    
             0x0F12, 0x0205, //                                                                   
             0x0F12, 0x0304, //                                                                     
             0x0F12, 0x0409, //                                                                     
             0x0F12, 0x0306, //                                                                          
             0x0F12, 0x0407, //                                                                             
             0x0F12, 0x1F04, //                                                                              
             0x0F12, 0x0218, //                                                                           
             0x0F12, 0x1102, //                                                                            
             0x0F12, 0x0611, //                                                                            
             0x0F12, 0x1A02, //                                                                            
             0x0F12, 0x8018, //                                                                            
             0x0F12, 0x0080, //                                                                      
             0x0F12, 0x0080, //                                                                    
             0x0F12, 0x0180, //                                                              
             0x0F12, 0x0A0A, //                                                             
             0x0F12, 0x0101, //                                                                   
             0x0F12, 0x1B24, //                                                                  
             0x0F12, 0x6024, //                                                                  
             0x0F12, 0x1D22, //                                                              
             0x0F12, 0xFFFF, //                                                                  
             0x0F12, 0x0808, //                                                                       
             0x0F12, 0x0A01, //                                                                        
             0x0F12, 0x010A, //                                                                       
             0x0F12, 0x2401, //                                                                          
             0x0F12, 0x241B, //                                                                          
             0x0F12, 0x1E60, //                                                                         
             0x0F12, 0xFF18, //                                                                         
             0x0F12, 0x08FF, //                                                                              
             0x0F12, 0x0008, //                                             
             0x0F12, 0x0001, //                                        
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                        
             0x0F12, 0x00C0, //                              
             0x0F12, 0x0064, //                                    
             0x0F12, 0x0384, //                                     
             0x0F12, 0x0043, //                                 
             0x0F12, 0x01F4, //                                  
             0x0F12, 0x0070, //                               
             0x0F12, 0x0040, //                                     
             0x0F12, 0x00A0, //                                     
             0x0F12, 0x0100, //                                     
             0x0F12, 0x0010, //                                     
             0x0F12, 0x0060, //                                     
             0x0F12, 0x0100, //                                     
             0x0F12, 0x1430, //                                                         
             0x0F12, 0x0201, //                                                              
             0x0F12, 0x0204, //                                                                   
             0x0F12, 0x1B04, //                                                               
             0x0F12, 0x0312, //                                                                
             0x0F12, 0x0003, //                                                                   
             0x0F12, 0x0C03, //                                                                 
             0x0F12, 0x2806, //                                                                      
             0x0F12, 0x0060, //                                                                   
             0x0F12, 0x1580, //                                                                     
             0x0F12, 0x2020, //                                                                             
             0x0F12, 0x0620, //                                                                            
             0x0F12, 0x0306, //                                                                        
             0x0F12, 0x2003, //                                                                 
             0x0F12, 0xFF01, //                                                                        
             0x0F12, 0x0404, //                                                                   
             0x0F12, 0x0300, //                                                              
             0x0F12, 0x145A, //                                                           
             0x0F12, 0x1010, //                                                            
             0x0F12, 0x000B, //                                                         
             0x0F12, 0x0E00, //                                                       
             0x0F12, 0x5A0F, //                                                            
             0x0F12, 0x0504, //                                                                 
             0x0F12, 0x1802, //                                                         
             0x0F12, 0x0000, //                                                             
             0x0F12, 0x2006, //                                                             
             0x0F12, 0x3828, //                                                              
             0x0F12, 0x0428, //                                                      
             0x0F12, 0x0101, //                                                              
             0x0F12, 0x8000, //                                                               
             0x0F12, 0x0A04, //                                                                
             0x0F12, 0x4008, //                                                                 
             0x0F12, 0x0540, //                                                               
             0x0F12, 0x8006, //                                                               
             0x0F12, 0x0020, //                                                                
             0x0F12, 0x0000, //                                                                       
             0x0F12, 0x1800, //                                                                   
             0x0F12, 0x0000, //                                                                      
             0x0F12, 0x1E10, //                                                             
             0x0F12, 0x000B, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0005, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0405, //                                                                    
             0x0F12, 0x0207, //                                                                   
             0x0F12, 0x0304, //                                                                     
             0x0F12, 0x0409, //                                                                     
             0x0F12, 0x0306, //                                                                          
             0x0F12, 0x0407, //                                                                             
             0x0F12, 0x2404, //                                                                              
             0x0F12, 0x0221, //                                                                           
             0x0F12, 0x1202, //                                                                            
             0x0F12, 0x0613, //                                                                            
             0x0F12, 0x1A02, //                                                                            
             0x0F12, 0x8018, //                                                                            
             0x0F12, 0x0080, //                                                                      
             0x0F12, 0x0080, //                                                                    
             0x0F12, 0x0180, //                                                              
             0x0F12, 0x0A0A, //                                                             
             0x0F12, 0x0101, //                                                                   
             0x0F12, 0x141D, //                                                                  
             0x0F12, 0x8030, //                                                                  
             0x0F12, 0x0C0C, //                                                              
             0x0F12, 0xFFFF, //                                                                  
             0x0F12, 0x0808, //                                                                       
             0x0F12, 0x0A01, //                                                                        
             0x0F12, 0x010A, //                                                                       
             0x0F12, 0x1B01, //                                                                          
             0x0F12, 0x3012, //                                                                          
             0x0F12, 0x0C80, //                                                                         
             0x0F12, 0xFF0C, //                                                                         
             0x0F12, 0x08FF, //                                                                              
             0x0F12, 0x0008, //                                             
             0x0F12, 0x0001, //                                        
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                        
             0x0F12, 0x00C0, //                              
             0x0F12, 0x0064, //                                    
             0x0F12, 0x0384, //                                     
             0x0F12, 0x0032, //                                 
             0x0F12, 0x01F4, //                                  
             0x0F12, 0x0070, //                               
             0x0F12, 0x0040, //                                     
             0x0F12, 0x00A0, //                                     
             0x0F12, 0x0100, //                                     
             0x0F12, 0x0010, //                                     
             0x0F12, 0x0060, //                                     
             0x0F12, 0x0100, //                                     
             0x0F12, 0x1430, //                                                         
             0x0F12, 0x0201, //                                                              
             0x0F12, 0x0204, //                                                                   
             0x0F12, 0x1504, //                                                               
             0x0F12, 0x030F, //                                                                
             0x0F12, 0x0003, //                                                                   
             0x0F12, 0x0902, //                                                                 
             0x0F12, 0x2004, //                                                                      
             0x0F12, 0x0050, //                                                                   
             0x0F12, 0x1140, //                                                                     
             0x0F12, 0x201C, //                                                                             
             0x0F12, 0x0620, //                                                                            
             0x0F12, 0x0306, //                                                                        
             0x0F12, 0x2003, //                                                                 
             0x0F12, 0xFF01, //                                                                        
             0x0F12, 0x0404, //                                                                   
             0x0F12, 0x0300, //                                                              
             0x0F12, 0x145A, //                                                           
             0x0F12, 0x1010, //                                                            
             0x0F12, 0x000B, //                                                         
             0x0F12, 0x1000, //                                                       
             0x0F12, 0x5A0F, //                                                            
             0x0F12, 0x0503, //                                                                 
             0x0F12, 0x1802, //                                                         
             0x0F12, 0x0000, //                                                             
             0x0F12, 0x2006, //                                                             
             0x0F12, 0x3C28, //                                                              
             0x0F12, 0x042C, //                                                      
             0x0F12, 0x0101, //                                                              
             0x0F12, 0xFF00, //                                                               
             0x0F12, 0x0904, //                                                                
             0x0F12, 0x4008, //                                                                 
             0x0F12, 0x0540, //                                                               
             0x0F12, 0x8006, //                                                               
             0x0F12, 0x0020, //                                                                
             0x0F12, 0x0000, //                                                                       
             0x0F12, 0x1800, //                                                                   
             0x0F12, 0x0000, //                                                                      
             0x0F12, 0x1E10, //                                                             
             0x0F12, 0x000B, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0005, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0405, //                                                                    
             0x0F12, 0x0206, //                                                                   
             0x0F12, 0x0304, //                                                                     
             0x0F12, 0x0409, //                                                                     
             0x0F12, 0x0305, //                                                                          
             0x0F12, 0x0406, //                                                                             
             0x0F12, 0x2804, //                                                                              
             0x0F12, 0x0228, //                                                                           
             0x0F12, 0x1402, //                                                                            
             0x0F12, 0x0618, //                                                                            
             0x0F12, 0x1A02, //                                                                            
             0x0F12, 0x8018, //                                                                            
             0x0F12, 0x0080, //                                                                      
             0x0F12, 0x0080, //                                                                    
             0x0F12, 0x0180, //                                                              
             0x0F12, 0x0A0A, //                                                             
             0x0F12, 0x0101, //                                                                   
             0x0F12, 0x1117, //                                                                  
             0x0F12, 0x8030, //                                                                  
             0x0F12, 0x0A0A, //                                                              
             0x0F12, 0xFFFF, //                                                                  
             0x0F12, 0x0808, //                                                                       
             0x0F12, 0x0A01, //                                                                        
             0x0F12, 0x010A, //                                                                       
             0x0F12, 0x1501, //                                                                          
             0x0F12, 0x3012, //                                                                          
             0x0F12, 0x0C80, //                                                                         
             0x0F12, 0xFF0A, //                                                                         
             0x0F12, 0x08FF, //                                                                              
             0x0F12, 0x0008, //                                             
             0x0F12, 0x0001, //                                        
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                         
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                           
             0x0F12, 0x0000, //                        
             0x0F12, 0x00C0, //                              
             0x0F12, 0x0064, //                                    
             0x0F12, 0x0384, //                                     
             0x0F12, 0x0032, //                                 
             0x0F12, 0x01F4, //                                  
             0x0F12, 0x0070, //                               
             0x0F12, 0x0040, //                                     
             0x0F12, 0x00A0, //                                     
             0x0F12, 0x0100, //                                     
             0x0F12, 0x0010, //                                     
             0x0F12, 0x0060, //                                     
             0x0F12, 0x0100, //                                     
             0x0F12, 0x1430, //                                                         
             0x0F12, 0x0201, //                                                              
             0x0F12, 0x0204, //                                                                   
             0x0F12, 0x0F04, //                                                               
             0x0F12, 0x030C, //                                                                
             0x0F12, 0x0003, //                                                                   
             0x0F12, 0x0602, //                                                                 
             0x0F12, 0x1803, //                                                                      
             0x0F12, 0x0040, //                                                                   
             0x0F12, 0x0E20, //                                                                     
             0x0F12, 0x2018, //                                                                             
             0x0F12, 0x0620, //                                                                            
             0x0F12, 0x0306, //                                                                        
             0x0F12, 0x2003, //                                                                 
             0x0F12, 0xFF01, //                                                                        
             0x0F12, 0x0404, //                                                                   
             0x0F12, 0x0200, //                                                              
             0x0F12, 0x145A, //                                                           
             0x0F12, 0x1010, //                                                            
             0x0F12, 0x000B, //                                                         
             0x0F12, 0x1200, //                                                       
             0x0F12, 0x5A0F, //                                                            
             0x0F12, 0x0502, //                                                                 
             0x0F12, 0x1802, //                                                         
             0x0F12, 0x0000, //                                                             
             0x0F12, 0x2006, //                                                             
             0x0F12, 0x4028, //                                                              
             0x0F12, 0x0430, //                                                      
             0x0F12, 0x0101, //                                                              
             0x0F12, 0xFF00, //                                                               
             0x0F12, 0x0804, //                                                                
             0x0F12, 0x4008, //                                                                 
             0x0F12, 0x0540, //                                                               
             0x0F12, 0x8006, //                                                               
             0x0F12, 0x0020, //                                                                
             0x0F12, 0x0000, //                                                                       
             0x0F12, 0x1800, //                                                                   
             0x0F12, 0x0000, //                                                                      
             0x0F12, 0x1E10, //                                                             
             0x0F12, 0x000B, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0005, //                                                                     
             0x0F12, 0x0607, //                                                                     
             0x0F12, 0x0405, //                                                                    
             0x0F12, 0x0205, //                                                                   
             0x0F12, 0x0304, //                                                                     
             0x0F12, 0x0409, //                                                                     
             0x0F12, 0x0306, //                                                                          
             0x0F12, 0x0407, //                                                                             
             0x0F12, 0x2C04, //                                                                              
             0x0F12, 0x022C, //                                                                           
             0x0F12, 0x1402, //                                                                            
             0x0F12, 0x0618, //                                                                            
             0x0F12, 0x1A02, //                                                                            
             0x0F12, 0x8018, //                                                                            
             0x0F12, 0x0080, //                                                                      
             0x0F12, 0x0080, //                                                                    
             0x0F12, 0x0180, //                                                              
             0x0F12, 0x0A0A, //                                                             
             0x0F12, 0x0101, //                                                                   
             0x0F12, 0x0C0F, //                                                                  
             0x0F12, 0x8030, //                                                                  
             0x0F12, 0x0808, //                                                              
             0x0F12, 0xFFFF, //                                                                  
             0x0F12, 0x0808, //                                                                       
             0x0F12, 0x0A01, //                                                                        
             0x0F12, 0x010A, //                                                                       
             0x0F12, 0x0F01, //                                                                          
             0x0F12, 0x3012, //                                                                          
             0x0F12, 0x0C80, //                                                                         
             0x0F12, 0xFF08, //                                                                         
             0x0F12, 0x08FF, //                                                                              
             0x0F12, 0x0008, //                                             
             0x0F12, 0x0001,   //                                                      
             0x0F12, 0x23CE, //                           
             0x0F12, 0xFDC8, //                           
             0x0F12, 0x112E, //                           
             0x0F12, 0x93A5, //                           
             0x0F12, 0xFE67, //                           
             0x0F12, 0x0000, //                           

             0x002A ,0x0A1E,
             0x0F12 ,0x0350, //                               
    //                                                                                  
    //                         
    //                                                                                  
#if defined(__CAPTURE_JPEG_OUTPUT__)
             //                          
             //                    
             //              
             0x002A, 0x0476,
             0x0F12, 0x0000, //                         
             0x002A, 0x0478,
             0x0F12, 0x005F, //                        
             0x0F12, 0x005F, //                           
             0x002A, 0x047c,
             0x0F12, 0x0000, //                          
#endif

             //         
             //                                    
             //                                       
             //                                      
             //                                     
             //                                      
             //                                     
             //         
             //                           
             //         
             //                                
             //         
             //                                      
             //         
             //                                           
             //                                                
             //         
             //         
             //         
             //         

             0x002A, 0x0EE2,      //              
             0x0F12, 0x0010,      //      

    //                                                                                  
    //                 
    //                                                                                  
    //                         
             0x002A, 0x01F8,      //              
             0x0F12, (S5K4ECGX_MCLK*1000), //      
             0x002A, 0x0212,
             0x0F12, 0x0000,   //                               
             0x0F12, 0x0002,   //                                
             0x0F12, 0x0002,   //                                   

             0x002A, 0x021A,
             0x0F12, MIPI_CLK0_SYS_OP_RATE,  //                        
             0x0F12, MIPI_CLK0_MAX, //                                             
             0x0F12, MIPI_CLK0_MIN, //                                             

             0x0F12, MIPI_CLK1_SYS_OP_RATE,  //               
             0x0F12, MIPI_CLK1_MIN, //                                             
             0x0F12, MIPI_CLK1_MAX, //                                             

    //                                                                                  
    //                      
    //                                                                                  
    //          
             0x002A, 0x0250,
             0x0F12, 0x0A00, //                           
             0x0F12, 0x0780, //                            
             0x0F12, 0x0010, //                           
             0x0F12, 0x000C, //                            
             0x0F12, 0x0A00, //                          
             0x0F12, 0x0780, //                           
             0x0F12, 0x0010, //                          
             0x0F12, 0x000C, //                           
             0x002A, 0x0494,
             0x0F12, 0x0A00, //                           
             0x0F12, 0x0780, //                            
             0x0F12, 0x0000, //                              
             0x0F12, 0x0000, //                               
             0x0F12, 0x0A00, //                           
             0x0F12, 0x0780, //                            
             0x0F12, 0x0000, //                              
             0x0F12, 0x0000, //                               
             0x002A, 0x0262,
             0x0F12, 0x0001, //                           
             0x0F12, 0x0001, //                           


    //                              
    //                                       
             0x002A, 0x02A6,   //                                                        
             0x0F12, S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV,   //                          
             0x0F12, S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV,   //                          
             0x0F12, 0x0005,   //                                     
             0x0F12, MIPI_CLK0_MAX,  //                                              
             0x0F12, MIPI_CLK0_MIN,  //                                              
             0x0F12, 0x0100,   //                                        
             0x0F12, 0x0300,   //                   
             0x0F12, 0x0002,   //                    
             0x0F12, 0x0000,   //                    
             0x0F12, 0x01E0,   //                             
             0x0F12, 0x0000,   //                               
             0x0F12, 0x0000,   //                      
             0x0F12, 0x0000,   //                         
             0x0F12, 0x0001,   //                              
             0x0F12, 0x0535, //                                  
             0x0F12, 0x014D, //                                  
             0x002A, 0x02D0,
             0x0F12, 0x0000,   //                        
             0x0F12, 0x0000,   //                           
             0x0F12, 0x0000,   //                      
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
    }

    S5K4ECGX_PreviewWin[0].GrabStartX = 0;
    S5K4ECGX_PreviewWin[0].GrabStartY = 0;
    S5K4ECGX_PreviewWin[0].ExposureWindowWidth = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV;
    S5K4ECGX_PreviewWin[0].ExposureWindowHeight = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV;

    {
        static const kal_uint16 addr_data_pair[] =
        {
             //                             
             //                          
             0x002A, 0x02D6, //                               
             0x0F12, 1280,   //                    
             0x0F12, 960,    //                     
             0x0F12, 0x0005,   //                                     
             0x0F12, MIPI_CLK0_MAX,  //                                              
             0x0F12, MIPI_CLK0_MIN,  //                                              
             0x0F12, 0x0100,   //                           
             0x0F12, 0x0300,   //                   
             0x0F12, 0x0002,   //                    
             0x0F12, 0x0000,   //                    
             0x0F12, 0x01E0,   //                             
             0x0F12, 0x0000,   //                               
             0x0F12, 0x0000,   //                      
             0x0F12, 0x0002,   //                         
             0x0F12, 0x0001,   //                              
             0x0F12, 0x014d,   //                                  
             0x0F12, 0x014d,   //                                  
             0x002A, 0x0300,
             0x0F12, 0x0000,   //                        
             0x0F12, 0x0000,   //                           
             0x0F12, 0x0000,   //                      
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
    }
    S5K4ECGX_PreviewWin[1].GrabStartX = 0;
    S5K4ECGX_PreviewWin[1].GrabStartY = 0;
    S5K4ECGX_PreviewWin[1].ExposureWindowWidth = 0x0500;
    S5K4ECGX_PreviewWin[1].ExposureWindowHeight = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV;


    {
        static const kal_uint16 addr_data_pair[] =
        {
             //                  
             //                                 
             0x002A, 0x0306,   //                               
             0x0F12, 2560,   //                    
             0x0F12, 1920,   //                     
             0x0F12, 0x0005,   //                                     
             0x0F12, MIPI_CLK0_MAX,  //                                              
             0x0F12, MIPI_CLK0_MIN,  //                                              
             0x0F12, 0x0100,   //                           
             0x0F12, 0x0300,   //                   
             0x0F12, 0x0002,   //                    
             0x0F12, 0x0000,   //                    
             0x0F12, 0x01E0,   //                             
             0x0F12, 0x0000,   //                               
             0x0F12, 0x0000,   //                      
             0x0F12, 0x0000,   //                         
             0x0F12, 0x0002,   //                              
             0x0F12, 0x0535,   //                                  
             0x0F12, 0x0341,   //                                  
             0x002A, 0x0330,
             0x0F12, 0x0000,   //                        
             0x0F12, 0x0000,   //                           
             0x0F12, 0x0000,   //                      
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
    }
    S5K4ECGX_PreviewWin[2].GrabStartX = 0;
    S5K4ECGX_PreviewWin[2].GrabStartY = 0;
    S5K4ECGX_PreviewWin[2].ExposureWindowWidth = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV;
    S5K4ECGX_PreviewWin[2].ExposureWindowHeight = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV;



    {
        static const kal_uint16 addr_data_pair[]=
        {
             //                              
             0x002A, 0x0396,  //                           
             0x0F12, 0x0001,   //                                       
             0x002A, 0x0398,
             0x0F12, S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV,   //                         
             0x0F12, S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV,   //                     
             0x0F12, OUTPUT_FMT,   //                                     
             0x0F12, MIPI_CLK1_MAX,  //                                              
             0x0F12, MIPI_CLK1_MIN,  //                                              
             0x0F12, 0x0100,   //                           
             0x0F12, 0x0300,   //                   
             0x0F12, 0x0002,   //                                   
             0x0F12, 0x0070,   //                    
             0x0F12, 0x0810,   //                             
             0x0F12, 0x0000,   //                               
             0x0F12, MIPI_CAP_CLK_IDX,   //                      
             0x0F12, 0x0000,   //                         
             0x0F12, 0x0002,   //                              
             0x0F12, 0x0535,   //                                         
             0x0F12, 0x0341,   //                                  


             0x002A, 0x0250,
             0x0F12, 0x0A00,   //                           
             0x0F12, 0x0780,   //                            
             0x0F12, 0x0010,   //                           
             0x0F12, 0x000C,   //                            
             0x0F12, 0x0A00,   //                          
             0x0F12, 0x0780,   //                           
             0x0F12, 0x0010,   //                          
             0x0F12, 0x000C,   //                           
             0x002A, 0x0494,
             0x0F12, 0x0A00,
             0x0F12, 0x0780,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x0F12, 0x0A00,
             0x0F12, 0x0780,
             0x0F12, 0x0000,
             0x0F12, 0x0000,
             0x002A, 0x0262,
             0x0F12, 0x0001,
             0x0F12, 0x0001,
             0x002A, 0x1CC2,   //                                    
             0x0F12, 0x0100,
             0x0F12, 0x0100,
             0x0F12, 0x0100,
             0x0F12, 0x0100,
             0x002A, 0x022C,
             0x0F12, 0x0001,   //                             

    //                                                                                  
    //                              
    //                                                                                  
    //       
    //                
             0x002A, 0x0266,
             0x0F12, 0x0000, //                          
             0x002A, 0x026A,
             0x0F12, 0x0001, //                             
             0x002A, 0x0268,
             0x0F12, 0x0001, //                           
             0x002A, 0x026E,
             0x0F12, 0x0000, //                         
             0x002A, 0x026A,
             0x0F12, 0x0001, //                            
             0x002A, 0x0270,
             0x0F12, 0x0001, //                          
             0x002A, 0x024E,
             0x0F12, 0x0001, //                       
             0x002A, 0x023E,
             0x0F12, 0x0001, //                       
             0x0F12, 0x0001, //                              
        };
        S5K4ECGX_table_write_cmos_sensor(addr_data_pair, sizeof(addr_data_pair)/sizeof(kal_uint16));
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
   //                                           
   //                                                                       
   //                                           
   //                                           
   //                                                                       

    SENSORDB("Sensor Init Done\n");
   return;
}



static void S5K4ECGX_MIPI_enb_preview(void)
{
   SENSORDB("[4EC] Enable preview...\n");
   S5K4ECGX_write_cmos_sensor(0x002A, 0x023E);
   S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                     
   S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                           

   //                                           
}

static void S5K4ECGX_Init_Setting(void)
{
    printk("[4EC Parallel] Sensor Init...\n");
                //               
                //               

    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x0010, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x1030, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0014, 0x0001);
    mdelay(50);
    S5K4ECGX_write_cmos_sensor(0x0028, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1082);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0155);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0155);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0055);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05D5);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x100E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x007A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0xE406);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0092);
    S5K4ECGX_write_cmos_sensor(0x002A, 0xE410);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3804);
    S5K4ECGX_write_cmos_sensor(0x002A, 0xE41A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x002A, 0xE420);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x002A, 0xE42E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);
    S5K4ECGX_write_cmos_sensor(0x002A, 0xF400);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5A3C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0023);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03AF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xAA54);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x464E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0240);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0240);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x55FF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0202);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0401);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0088);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x009F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0088);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2428);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03EE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0xF552);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0708);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x080C);
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x18BC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05B6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05BA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0007);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05BA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x024E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05B6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05BA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x024F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0075);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00CF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0075);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00F0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x029E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05B2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0228);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0208);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0238);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0218);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0238);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0009);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00DE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00DF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00E4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01FD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05B6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05BB);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0077);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x007E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x024F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x025E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09D1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09D5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09D5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0326);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09D1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09D5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0327);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0084);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x008D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00AA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03AD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09CD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02DE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02BE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02EE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02CE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02EE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0009);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0095);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09DB);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0096);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x009B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02B3);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09D1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09D6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0009);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0327);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0336);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1AF8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5A3C);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1896);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x189E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0FB0);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x18AC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05C0);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1AEA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1AE0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1A72);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x18A2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1A6A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x009A);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x385E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x024C);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0EE6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1B2A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x008D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00CF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0084);

    //                                                                                  
    //                            
    //                                                                                  
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0722);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100); //                                                                          
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0726);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                           
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08D6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //               
    S5K4ECGX_write_cmos_sensor(0x002A, 0x146E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08DC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                   

    //                   
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0D26);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F00);

    //                  
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0DB6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x94C8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE78D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF985);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2237);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEA86);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x013A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xADE0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0B8F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1276);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD850);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0B55);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x158F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5E3B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF62B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE9A1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2952);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x073E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC818);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xA6F0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFD77);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0DB0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE9D3);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF39F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3BCD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3CA4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x133B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A3C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF18D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF760);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF6B3);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF5AE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE8D4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF00F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1834);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x186A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE0AE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xA83C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDFE0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFED4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x20B2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE8B2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0541);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xA78D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0B62);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1364);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD247);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x17C7);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0B8A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5817);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF488);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEC21);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2E7E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF8AF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD353);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBBCB);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05B2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE7F9);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFE21);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x360B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x247E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0E82);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x14DB);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEF9E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF488);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF513);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x040F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEAA4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE8A7);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1ECC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1084);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE896);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x994F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDFC8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFF6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2643);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD8D2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x11C8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xAB93);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x149D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x09A7);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD77A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x170D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0B56);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5E42);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEB77);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF5A5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2301);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x07D8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC746);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB08D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x06FE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0335);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xED10);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xED93);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x48AB);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2982);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0B93);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1018);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF758);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF8DA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE6DA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03F5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xED1E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xECBF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1528);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x15E8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEBFE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x99D6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE4DE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFD52);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1CBC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF06E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFD9B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xAC4A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0D08);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1285);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD2A1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1426);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1132);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5DDB);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF3CE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE728);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3A5E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xED32);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD64C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB144);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF27);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0B10);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD985);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1209);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2B0C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2A96);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x12A1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x16F9);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEEDF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEDD3);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFC25);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x021C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE89F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE2E5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2854);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0BE7);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE768);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08B4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                  
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08BC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00DF); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0125); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x015F); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x017C); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0194); //                            
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08F6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3B00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);

    //                 
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4500);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                



    S5K4ECGX_write_cmos_sensor(0x002A, 0x189E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0FB0);       //                    
    S5K4ECGX_write_cmos_sensor(0x002A, 0x18AC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);       //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);       //                     
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05C0);       //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05C0);       //                     
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1AEA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8080);       //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);       //                    
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1AE0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);       //               
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1A72);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);       //                       
    S5K4ECGX_write_cmos_sensor(0x002A, 0x18A2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);       //                                                           
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1A6A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x009A);       //                                                                 
    S5K4ECGX_write_cmos_sensor(0x002A, 0x385E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x024C);       //                      
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0EE6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);       //                      
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1B2A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);  //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D6);  //                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x008D);  //                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00CF);  //                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0084);  //                              

       //                                                                         

       //            

       //                                                                         

    S5K4ECGX_write_cmos_sensor(0x002A, 0x0722);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);       //                                                                                         
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0726);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                                                            
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08D6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);       //                                                                                                             
    S5K4ECGX_write_cmos_sensor(0x002A, 0x146E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);       //                                                                                    
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08DC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);       //                                                                                                  

       //                                                                         

       //            

       //                                                                         

       //                    

    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x3AF8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB570);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4B3D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4A3D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x483E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC008);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6002);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x493C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x483D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2401);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFC3B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x493C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x483C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2502);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFC35);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x483B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0261);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8041);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4939);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x483A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6041);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x493A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x483A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2403);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFC27);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4939);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4839);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4939);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3840);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x63C1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4933);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4838);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3980);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6408);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4838);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4938);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6388);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4938);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4839);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2504);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFC14);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4937);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4838);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2405);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF890);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4835);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4936);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2506);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1D80);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF889);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4832);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4933);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2407);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x300C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF882);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4931);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2508);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3010);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF87B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x492F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2409);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFBF3);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x492E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x250A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFBED);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x492D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x240B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFBE7);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x492C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x250C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFBE1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x492B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x240D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFBDB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x492A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x250E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFBD5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4929);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4829);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFBD0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC70);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC08);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4718);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0184);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4EC2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03FF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1F90);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3CA5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE38B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3CDD);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC3B1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4780);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3D3B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3D77);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB49D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3F9F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0180);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3E23);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3DD7);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFFF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00FF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x17E0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3FBD);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x053D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A89);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6CD2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02C9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A9A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02D2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x400B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9E65);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x40DD);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7C49);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x40F9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7C63);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4115);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8F01);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x41B7);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7F3F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4245);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x98C5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x42FB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xCD59);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB570);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0015);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0029);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFB80);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x49F9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x500C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC70);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC08);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4718);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6808);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6849);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0409);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C09);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4AF4);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8992);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2A00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD00D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2300);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A89);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0419);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C09);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x23FF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x33C1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1810);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4298);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD800);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0418);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4AEC);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8150);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8191);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4770);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB5F3);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB081);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9802);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6800);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0600);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0E00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2201);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0015);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0021);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3910);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x408A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x40A5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4FE5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0016);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2C10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDA03);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8839);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x43A9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8039);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE002);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8879);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x43B1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8079);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFB4C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2C10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDA03);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8839);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4329);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8039);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE002);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8879);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4331);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8079);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x49DB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8809);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2900);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD102);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFB45);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9902);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6008);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBCFE);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC08);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4718);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB538);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9C04);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0015);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFB40);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4AD2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8811);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2900);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD00F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8820);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4281);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD20C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8861);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8853);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4299);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD200);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1E40);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8851);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8061);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4368);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1840);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6060);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC38);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC08);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4718);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB5F8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6808);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2201);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0015);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0021);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3910);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x408A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x40A5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4FBF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0016);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2C10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDA03);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8839);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x43A9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8039);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE002);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8879);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x43B1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8079);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFB19);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2C10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDA03);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8838);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4328);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8038);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE002);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8878);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4330);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8078);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x48B7);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8800);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD507);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4BB6);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7819);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4AB6);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7810);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7018);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7011);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x49B5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8188);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBCF8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC08);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4718);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB538);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x48B3);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4669);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFB04);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x48B2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x49B1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x69C2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x31A8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2A00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD008);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x61C4);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x684A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6242);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6282);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x466B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x881A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6302);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x885A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6342);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6A02);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2A00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD00A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6204);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6849);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6281);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x466B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8819);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6301);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8859);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6341);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x49A6);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88C9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x63C1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFAEC);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE7A6);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB5F0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB08B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x20FF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1C40);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x49A2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x89CC);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4E9F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6AB1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4284);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD101);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x48A0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6081);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6A70);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0200);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFAE3);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4A97);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8A11);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9109);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2101);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0349);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4288);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD200);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4A93);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8211);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4D98);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8829);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9108);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4A8C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2303);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3222);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1F91);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFAD4);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8028);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x488F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4988);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6BC2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6AC0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4282);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD201);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8CC8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8028);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88E8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9007);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2240);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4310);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x80E8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0041);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x194B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x001E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3680);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8BB2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xAF04);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x527A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4A7E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x188A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8897);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x83B7);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x33A0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x891F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xAE01);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5277);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8A11);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8119);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1C40);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2806);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD3E9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFAB5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFABB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4F7A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x37A8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2800);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD10A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1FE0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x38FD);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1CC0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD105);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4875);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8829);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3818);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6840);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4348);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6078);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4973);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6878);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6B89);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4288);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD300);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6078);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0041);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xAA04);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5A53);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x194A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x269C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x52B3);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xAB01);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5A59);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x32A0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8111);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1C40);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2806);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD3F0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4966);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9809);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8208);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9808);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8028);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9807);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x80E8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1FE0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x38FD);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD13B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4D65);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x89E8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1FC1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x39FF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD136);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4C60);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8AE0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA8A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0006);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8B20);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA8E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6AA1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6878);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1809);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0200);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA61);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3246);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0011);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x310A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2305);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA5E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x66E8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6B23);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0031);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0018);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA7F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x466B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8518);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6EEA);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6B60);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9900);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA78);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x466B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8558);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0029);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x980A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3170);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA79);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0028);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3060);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8A02);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4947);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3128);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x808A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8A42);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x80CA);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8A80);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8108);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB00B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBCF0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC08);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4718);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4845);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3060);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8881);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2900);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD007);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8081);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4944);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x20FF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1C40);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8048);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4770);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4770);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB570);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4D40);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4841);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8881);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4841);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8041);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2101);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA59);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x483D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3820);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8BC0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA5C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4B3C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x220D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0712);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x18A8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8806);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00E1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x18C9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x81CE);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8846);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x818E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8886);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x824E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8208);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3508);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x042D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C2D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1C64);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0424);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C24);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2C07);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD3EC);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE649);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB510);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4C2F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8060);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3820);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8BC0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFA39);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88E0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4A2B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2800);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD003);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x492B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8849);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2900);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD009);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8050);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x80D0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8090);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8110);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC08);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4718);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8050);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8920);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x80D0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8960);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8090);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x89A1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0409);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1409);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8111);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x89E3);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8A24);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2B00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD104);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x17C3);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F5B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1818);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x10C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8090);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD1E6);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x17C8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F40);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1840);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x10C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8110);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE7E0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x38D4);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x17D0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x171A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4780);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2FCA);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2FC5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2FC6);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2ED8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2BD0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x17E0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2DE8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x37E0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x210C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1484);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xA006);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0724);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xA000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2270);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2558);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x146C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB510);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x499D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2204);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6820);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5E8A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0140);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A80);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0280);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8849);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF9D8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE7A3);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB510);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4996);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2208);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6820);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5E8A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0140);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A80);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0280);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88C9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF9CA);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE795);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB5FE);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6825);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6866);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x68A0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x68E7);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1BA8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x42B5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDA00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1B70);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x498A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x488B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x884A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8843);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x435A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2304);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5ECB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A92);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x18D2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02D2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C12);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88CB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8880);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4343);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A98);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2308);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5ECB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x18C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0411);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1409);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A08);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x497F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x39E0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6148);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9801);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3040);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7880);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2800);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD103);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9801);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0029);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF99D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8839);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9800);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4281);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD814);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8879);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9800);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4281);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD20C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9801);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0029);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF999);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9801);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0029);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF995);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9801);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0029);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF991);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE003);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9801);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0029);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF98C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9801);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0032);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0039);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF98F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE5BE);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB57C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4869);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xA901);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF913);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x466B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88D9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8898);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4B64);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3346);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1E9A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF987);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4863);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4961);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3812);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3140);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8A42);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x888B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x18D2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8242);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8AC2);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88C9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1851);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x82C1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4669);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF8FB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x485C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x214D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8301);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2196);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8381);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x211D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF975);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF97B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4857);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4C57);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6E00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x60E0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x466B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8818);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8859);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0025);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A40);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3540);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x61A8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x484E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9900);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3060);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF973);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x466B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8819);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1DE0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x30F9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8741);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8859);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8781);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x71A0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x74A8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC7C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xBC08);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4718);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB5F8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6808);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x684A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0412);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C12);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x688E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x68CC);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x493F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x884B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4343);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A98);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2304);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5ECB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x18C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88CB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4353);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A9A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2308);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5ECB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x18D1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02C9);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C09);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2701);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x003A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x40AA);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9200);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3A10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4097);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2D10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDA06);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4A38);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9B00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8812);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x439A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4B36);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x801A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE003);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4B35);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x885A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x43BA);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x805A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0023);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0032);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF91B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2D10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDA05);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4930);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9A00);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8808);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4310);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8008);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE003);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x482D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8841);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4339);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8041);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4D2A);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3580);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88AA);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5E30);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF927);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8030);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x88AA);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5E20);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF920);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE575);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4823);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2103);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x81C1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4A23);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8011);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2101);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8181);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2102);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x81C1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4770);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB5F3);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB081);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2101);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3810);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4081);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x40A5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4F18);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2C10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDA03);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8838);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x43A8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8038);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE002);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8878);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x43B0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8078);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF905);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4B15);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7018);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2C10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDA03);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8838);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4328);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8038);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE002);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8878);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4330);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8078);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4C10);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7820);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2800);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD005);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF7FF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFCE);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7020);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x490D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7008);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7818);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9902);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7008);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE4ED);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2558);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2AB8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x145E);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2698);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2BB8);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2998);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1040);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9100);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3044);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3898);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3076);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1789);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x16F1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC3B1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC36D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF6D7);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xB49D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7EDF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x448D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF004);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE51F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x29EC);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2EF1);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEE03);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xA58B);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7C49);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7C63);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2DB7);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xEB3D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF061);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF0EF);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF004);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE51F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2824);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8EDD);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8DCB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8E17);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x98C5);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7C7D);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7E31);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7EAB);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7501);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4778);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x46C0);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC000);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE59F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1C);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xE12F);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xCD59);    //         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);    //         
           //                                    
           //                         
           //                                

           //                     
           //                
           //                      
           //                           
           //             
           //                 
           //                  
           //                  
           //                  
           //                       
           //                      


    S5K4ECGX_write_cmos_sensor(0x0028, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x01FC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x01FE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0204);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0061);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x020C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2F0C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0190);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0294);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00E3);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0200);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0238);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0166);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0074);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0132);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x070E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00FF);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x071E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x163C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1648);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9002);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1652);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x15E0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0801);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x164C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x163E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00E5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00CC);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x15D4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x169A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF95);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x166A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0280);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1676);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0320);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x16BC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0030);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x16E0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x16D4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1656);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x15E6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x003C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0015);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0032);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0038);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x003E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0044);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x004A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0050);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0056);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x005C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0062);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0068);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x006E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0074);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x007A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0086);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x008C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0092);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0098);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x009E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00AA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00B0);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1722);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3FF0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03E8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0009);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00E0);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x028C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08B4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08BC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00DF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0125);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x015F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x017C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0194);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08F6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3B00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4500);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1492);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0201);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0102);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0202);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0202);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0201);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0302);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0203);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0102);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0201);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0302);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0203);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0102);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0201);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0202);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0202);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0102);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0202);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0202);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1484);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x003C);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x148A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000F);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x058C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3520);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD4c0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3520);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD4c0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x059C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0470);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0544);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0111);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00EF);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0F2A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x04E6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x077F);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0F30);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0608);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A3C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0D05);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9C00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xAD00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xF1D4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDC00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xDC00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0638);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A3C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0D05);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3408);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6810);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8214);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xC350);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD4C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xD4c0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0660);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0650);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x06B8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x452C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x05D0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x145E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0580);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0428);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x07B0);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x11F0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0120);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0121);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x101C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x037C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x038E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x033C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02FE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x036C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02BA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0352);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x028E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x026A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02C8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0254);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02A8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0242);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x021A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0298);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01D4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0290);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01CC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0276);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01D2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0260);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x023A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1070);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000E);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1074);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0126);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1078);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0272);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x025A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02BC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x024A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x023C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02BE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x022E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02BC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0224);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02B6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0218);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02AA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0210);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x020C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0296);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x020A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x028C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0212);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x027E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0234);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0256);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x10AC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x10B0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01D8);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x10B4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0350);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0422);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02C4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0452);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0278);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x041C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0230);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03EE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0392);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0340);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0194);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0302);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x016E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02C2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0148);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0286);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x018A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0242);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0006);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x10E8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x10EC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0106);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x10F0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0380);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0168);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2D90);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1464);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0190);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A0);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1228);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x122C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x122A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x120A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x05D5);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x120E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0771);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03A4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0036);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x002A);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1278);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFEF7);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0021);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0AF0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0AF0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x018F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0096);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000E);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1224);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0032);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x001E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x2BA4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0006);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x146C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1434);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02CE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0347);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03C2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x10A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x10A1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1185);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1186);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x11E5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x11E6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00AB);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00BF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0093);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x13A4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFEC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF66);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFEC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF66);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFEC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF66);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFC0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1208);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x144E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFE0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0734);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0016);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0030);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0066);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0138);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0163);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0189);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0222);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0247);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0282);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02B5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x030F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x035F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03A2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03D8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03FF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0016);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0030);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0066);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0138);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0163);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0189);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0222);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0247);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0282);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02B5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x030F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x035F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03A2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03D8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03FF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0016);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0030);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0066);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0138);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0163);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0189);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0222);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0247);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0282);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02B5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x030F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x035F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03A2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03D8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03FF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0019);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0036);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x006F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0135);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x015F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0185);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F3);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0220);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x024A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0291);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02D0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x032A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x036A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x039F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03CC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03F9);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0019);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0036);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x006F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0135);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x015F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0185);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F3);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0220);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x024A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0291);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02D0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x032A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x036A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x039F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03CC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03F9);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0019);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0036);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x006F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0135);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x015F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0185);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F3);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0220);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x024A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0291);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02D0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x032A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x036A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x039F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03CC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03F9);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08A6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0125);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x015F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x017C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0194);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0898);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x08A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x48D8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x4800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0208);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFB5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFE8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF20);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01BF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF53);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFEA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0095);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFEFD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0206);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF7F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0191);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF06);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01BA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0108);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0208);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFB5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFE8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF20);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01BF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF53);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFEA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0095);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFEFD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0206);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF7F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0191);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF06);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01BA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0108);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0208);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFB5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFE8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF20);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01BF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF53);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0022);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFEA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0095);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFEFD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0206);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF7F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0191);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF06);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01BA);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0108);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0203);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFBD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFEF1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x014E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF18);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFE6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFDD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01B2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00F0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF49);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0151);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF50);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0147);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF75);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0187);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01BF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0203);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFBD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFEF1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x014E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF18);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFE6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFDD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01B2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00F0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF49);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0151);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF50);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0147);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF75);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0187);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01BF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0203);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFBD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFEF1);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x014E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF18);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFE6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFDD);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01B2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00F0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF49);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0151);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF50);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0147);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF75);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0187);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01BF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01E5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFA4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFDC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFE90);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x013F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF1B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFD2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFDF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0236);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00EC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00F8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF34);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01CE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF83);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0195);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFEF3);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0126);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0162);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0944);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0050);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00B0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0196);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0245);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x097A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01CC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01CC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01CC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01CC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01CC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0180);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0196);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0976);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0938);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0014);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x07D0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1388);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x098C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0064);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x005F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1430);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0201);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0204);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3604);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x032A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0403);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1B06);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6015);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0640);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0306);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x365A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x102A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0600);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5A0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0505);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1802);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3028);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0418);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1804);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0540);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1E10);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0405);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0205);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0304);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0409);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0306);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0407);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1C04);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0214);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1002);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0610);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A02);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4A18);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0348);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0180);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A0A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2A36);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2A36);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFFF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0808);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x010A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3601);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x242A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3660);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF2A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x08FF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0064);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0051);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1430);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0201);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0204);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2404);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x031B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0103);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1205);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x400D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0630);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0306);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0404);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x245A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1018);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0B00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5A0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0505);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1802);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3428);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x041C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1004);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0540);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1E10);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0405);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0205);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0304);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0409);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0306);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0407);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1F04);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0218);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1102);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0611);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A02);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8018);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0380);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0180);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A0A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1B24);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1D22);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFFF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0808);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x010A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2401);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x241B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1E60);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF18);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x08FF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0064);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0043);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1430);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0201);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0204);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1B04);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0312);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C03);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2806);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1580);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2020);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0620);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0306);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0404);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x145A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0E00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5A0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0504);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1802);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3828);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0428);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A04);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0540);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1E10);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0405);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0207);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0304);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0409);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0306);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0407);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2404);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0221);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1202);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0613);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A02);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8018);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0180);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A0A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x141D);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C0C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFFF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0808);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x010A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1B01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2412);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C60);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF0C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x08FF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0064);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0032);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1430);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0201);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0204);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1504);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x030F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0902);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2004);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0050);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1140);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x201C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0620);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0306);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0404);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x145A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5A0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0503);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1802);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3C28);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x042C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0904);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0540);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1E10);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0405);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0206);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0304);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0409);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0305);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0406);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2804);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0228);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1402);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0618);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A02);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8018);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0180);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A0A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1117);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A0A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFFF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0808);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x010A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1501);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x240F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A60);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF0A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x08FF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0064);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0032);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01F4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1430);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0201);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0204);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F04);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x030C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0602);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1803);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0E20);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2018);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0620);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0306);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2003);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0404);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0200);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x145A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1200);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x5A0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0502);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1802);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4028);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0430);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0804);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x4008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0540);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8006);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0020);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1E10);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0607);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0405);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0205);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0304);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0409);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0306);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0407);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x2C04);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x022C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1402);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0618);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x1A02);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x8018);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0180);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A0A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0101);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C0F);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0808);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFFF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0808);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x010A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0F01);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x240C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0860);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFF08);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x08FF);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0008);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x23CE);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFDC8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x112E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x93A5);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0xFE67);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);

       //                 
    //                            
    #if 0
    S5K4ECGX_write_cmos_sensor(0x002A, 0x01F8);   //              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x9C40);   //                                   
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0212);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                             
    S5K4ECGX_write_cmos_sensor(0x002A, 0x021A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3A98);   //                                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x53fc);   //                                                
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x53fc);   //                                                
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x278D);   //                                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x53fc);   //                                                
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x53fc);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x53fc);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x53fc);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x53fc);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x53fc);   //                                             
    #endif
    //                            
    #if 1
    S5K4ECGX_write_cmos_sensor(0x002A, 0x01F8);   //              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x6590);   //                                   
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0212);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                             
    S5K4ECGX_write_cmos_sensor(0x002A, 0x021A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3A98);   //                                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                                                
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                                                
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x278D);   //                                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                                                
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                                             
    #endif
    //                            
    #if 0
    S5K4ECGX_write_cmos_sensor(0x002A, 0x01F8);   //              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x7530);   //                                   
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0212);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                             
    S5K4ECGX_write_cmos_sensor(0x002A, 0x021A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x3A98);   //                                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x57E4);   //                                                
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x57E4);   //                                                
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x278D);   //                                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x57E4);   //                                                
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x57E4);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x57E4);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x57E4);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x57E4);   //                                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x57E4);   //                                             
    #endif


    S5K4ECGX_write_cmos_sensor(0x002A, 0x022C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //                             
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0478);   //           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x005F);   //                        
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x005F);   //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0280);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01E0);   //                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);   //                         
    S5K4ECGX_write_cmos_sensor(0x002A, 0x17DC);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0054);   //               
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1AE4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x001C);   //                    
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0284);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //                          
    S5K4ECGX_write_cmos_sensor(0x002A, 0x028A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                                                                     
       //                 
       //                                 
    S5K4ECGX_write_cmos_sensor(0x002A, 0x02A6);   //                                                        
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0500);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03c0);   //                     
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);   //                                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);   //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);   //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0042);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01E0);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                      
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //                              
    //                                                                                  
    //                                                                                  
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01A0);   //                                  
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01A0);   //                                  
    S5K4ECGX_write_cmos_sensor(0x002A, 0x02D0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                        
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                      
       //                 
       //                                
    S5K4ECGX_write_cmos_sensor(0x002A, 0x02D6);   //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0500);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03c0);   //                     
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);   //                                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);   //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);   //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0042);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01E0);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                      
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x07D0);   //                                  
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03e8);   //                                  
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                        
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                      
       //                 
       //                          
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0306);   //                                                        
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0500);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03c0);   //                     
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);   //                                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);   //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);   //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0042);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01E0);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                      
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x014D);   //                                  
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x014D);   //                                  
    S5K4ECGX_write_cmos_sensor(0x002A, 0x02D0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                        
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                      

       //                 
       //                            
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0336);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0500); //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02D0); //                     
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005); //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE); //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE); //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100); //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300); //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0042); //                                 
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01E0); //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                      
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x014D); //                                  
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x014D); //                                  
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0330);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                       
       //                 
       //                                              
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0366);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0500); //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x02D0); //                     
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005); //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE); //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE); //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100); //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300); //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0042); //                                 
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01E0); //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                      
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03E8); //                                  
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03E8); //                                  
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0360);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                       
       //                 
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0396);  //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);   //                     
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);   //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);   //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);   //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0042);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03C0);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0E80);   //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                      
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0570);   //                                  
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0500);   //                                  
       //                 
    S5K4ECGX_write_cmos_sensor(0x002A, 0x03C2);   //                            
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);   //                     
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);   //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x58DE);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);   //                           
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);   //                   
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0042);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x03c0);   //                             
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0E80);   //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);   //                      
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);   //                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);   //                              
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0570);   //                                  
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0500);   //                                  
    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xd000);
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);                     //      
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0250);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0494);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0262);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);


      //       
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1CC2);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x01A8);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A0A);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x147C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0170);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1482);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01E0);

    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0484);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0002);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x183A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x17F6);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x023C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0248);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1840);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0120);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0180);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0200);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0400);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0800);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0090);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0070);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0045);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0030);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1884);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x1826);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0080);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0030);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0040);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0048);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0050);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0060);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x4784);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00A0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0200);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0088);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00B0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x00C0);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0200);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0300);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x479C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0120);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0150);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0200);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x003C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x003B);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0026);
       //                
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0266);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x026A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x024E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0268);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0270);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
}

static void S5K4ECGX_enb_preview(){

        printk("[4EC Parallel] Enable preview...\n");
    S5K4ECGX_write_cmos_sensor(0x002A, 0x023E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
        printk("[4EC Parallel] Enable preview done...\n");
}


/*                                                                        
          
                        
 
             
                                                                      
 
            
        
 
         
         
 
                
 
                                                                        */
static void S5K4ECGX_MIPI_GetEvAwbRef(PSENSOR_AE_AWB_REF_STRUCT Ref)  //   
{
    Ref->SensorAERef.AeRefLV05Shutter = 3816; //     
    Ref->SensorAERef.AeRefLV05Gain = 896; /*                */
    Ref->SensorAERef.AeRefLV13Shutter = 99;   //    
    Ref->SensorAERef.AeRefLV13Gain = 1 * 128; /*              */
    Ref->SensorAwbGainRef.AwbRefD65Rgain = 210; //                         
    Ref->SensorAwbGainRef.AwbRefD65Bgain = 149; //                         
    Ref->SensorAwbGainRef.AwbRefCWFRgain = 179; //                              
    Ref->SensorAwbGainRef.AwbRefCWFBgain = 267; //                              
}
/*                                                                        
          
                            
 
             
                                                              
 
            
         
 
         
         
 
                
 
                                                                        */
static void S5K4ECGX_MIPI_GetCurAeAwbInfo(PSENSOR_AE_AWB_CUR_STRUCT Info)
{
    //                                                             
    //                                                                      
    //                                           

    //                                                                                                      
    //                                                                                        
}



UINT32 S5K4ECGX_MIPI_GetSensorID(UINT32 *sensorID)
{
   int  retry = 3;

   //                                    
   //                           
   do {
        S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
        S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
        S5K4ECGX_write_cmos_sensor(0x002E,0x01A4);//           
        *sensorID = S5K4ECGX_read_cmos_sensor(0x0F12);
        SENSORDB("[4EC]Read Sensor ID = 0x%04x, Currect Sensor ID = 0x%04x\n", *sensorID, S5K4ECGX_SENSOR_ID);

        if (*sensorID == S5K4ECGX_SENSOR_ID)
        {
            SENSORDB("[4EC]ID = 0x%04x\n", *sensorID);
            break;
        }
        SENSORDB("[4EC]Read Sensor ID Fail = 0x%04x, Currect Sensor ID = 0x%04x\n", *sensorID, S5K4ECGX_SENSOR_ID);
        retry--;
   } while (retry > 0);

   if (*sensorID != S5K4ECGX_SENSOR_ID) {
        *sensorID = 0xFFFFFFFF;
        SENSORDB("[4EC] Read Sensor ID Fail = 0x%04x, Currect Sensor ID = 0x%04x\n", *sensorID, S5K4ECGX_SENSOR_ID);
        return ERROR_SENSOR_CONNECT_FAIL;
   }

   //                                    

   return ERROR_NONE;
} /*                       */




/*                                                                        
          
            
 
             
                                                       
 
            
      
 
         
      
 
                  
 
                                                                        */

UINT32 S5K4ECGX_MIPI_Open(void)
{
    kal_uint32 sensor_id=0;

    SENSORDB("[Enter]:S5K4ECGX_MIPI_Open:\r\n");
    S5K4ECGX_MIPI_GetSensorID(&sensor_id);
    if (0xFFFFFFFF == sensor_id)
    {
        SENSORDB("[Camera & Sensor] S5K4EC Read Sensor ID Fail\n");
        return ERROR_SENSOR_CONNECT_FAIL;
    }

#ifdef MIPI_INTERFACE
    S5K4ECGX_MIPI_Init_Setting();
    S5K4ECGX_MIPI_enb_preview();
#else //         
    S5K4ECGX_Init_Setting();
    S5K4ECGX_enb_preview();
#endif
    S5K4ECGX_Driver.shutter = 0x4EA;
    S5K4ECGX_Driver.sensorGain = 0x1f;
    S5K4ECGX_Driver.Dummy_Pixels = 0;
    S5K4ECGX_Driver.Dummy_Lines = 0;


    return ERROR_NONE;
} /*                */




/*                                                                        
          
               
 
             
                                                   
 
            
      
 
         
      
 
                  
 
                                                                        */
UINT32 S5K4ECGX_MIPI_Close(void)
{

    return ERROR_NONE;
} /*                 */



void S5K4ECGX_MIPI_GetAutoISOValue(void)
{
    //                                       
    //                                         
    //                     
    //                           
    //                           
    //                          
    if (AE_ISO_AUTO != S5K4ECGX_Driver.isoSpeed)
    {
       return;
    }
    unsigned int A_Gain, D_Gain, ISOValue;
    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E, 0x2BC4);
    A_Gain = S5K4ECGX_read_cmos_sensor(0x0F12);
    D_Gain = S5K4ECGX_read_cmos_sensor(0x0F12);

    ISOValue = ((A_Gain * D_Gain) >> 9);
    spin_lock(&s5k4ecgx_mipi_drv_lock);
#if 0
    if (ISOValue == 256)
    {
       S5K4ECGX_Driver.isoSpeed = AE_ISO_50;
    }
    else if ((ISOValue >= 257) && (ISOValue <= 511 ))
    {
       S5K4ECGX_Driver.isoSpeed = AE_ISO_100;
    }
#endif
#if 0
    if ((ISOValue >= 200) && (ISOValue < 896 ))
    {
       S5K4ECGX_Driver.isoSpeed = AE_ISO_200;
    }
    else if (ISOValue >= 896)
    {
       S5K4ECGX_Driver.isoSpeed = AE_ISO_400;
    }
    else
    {
       S5K4ECGX_Driver.isoSpeed = AE_ISO_100;
    }
#endif
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    SENSORDB("[4EC] Auto ISO Value = %d \n", ISOValue);
}


void S5K4ECGX_MIPI_GetActiveConfigNum(unsigned int *pActiveConfigNum)
{
    spin_lock(&s5k4ecgx_mipi_drv_lock);
    MSDK_SCENARIO_ID_ENUM scen = S5K4ECGXCurrentScenarioId;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);


    if (MSDK_SCENARIO_ID_VIDEO_PREVIEW == scen)
    {
        *pActiveConfigNum = S5K4EC_VIDEO_MODE;
    }
    else if (MSDK_SCENARIO_ID_CAMERA_PREVIEW == scen)
    {
        *pActiveConfigNum = S5K4EC_PREVIEW_MODE;
    }
    else
    {
        *pActiveConfigNum = S5K4EC_PREVIEW_FULLSIZE_MODE;
    }

    return;
}


void S5K4ECGX_MIPI_SetFrameRate(MSDK_SCENARIO_ID_ENUM scen, UINT16 u2FrameRate)
{

    //                                   
    //                                                       
    //                                     
    UINT32 u4frameTime = (1000 * 10) / u2FrameRate;

    if (15 >= u2FrameRate)
    {
       switch (scen)
       {
           default:
           case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
            S5K4ECGX_write_cmos_sensor(0x002A, 0x02C2);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0594); //                                               
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x029A); //                                                
            break;
           case MSDK_SCENARIO_ID_VIDEO_PREVIEW: //     
            S5K4ECGX_write_cmos_sensor(0x002A ,0x02F2);
            S5K4ECGX_write_cmos_sensor(0x0F12, u4frameTime); //                                               
            S5K4ECGX_write_cmos_sensor(0x0F12, u4frameTime); //                                                
            break;
           case MSDK_SCENARIO_ID_CAMERA_ZSD:  //                             
            S5K4ECGX_write_cmos_sensor(0x002A ,0x0322);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0594); //                                               
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0341); //                                                
            break;
        }
    }
    else
    {
      switch (scen)
      {
         default:
         case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
            S5K4ECGX_write_cmos_sensor(0x002A, 0x02C2);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0535); //                                               
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x014D); //                                                
            break;
         case MSDK_SCENARIO_ID_VIDEO_PREVIEW: //     
            S5K4ECGX_write_cmos_sensor(0x002A ,0x02F2);
            S5K4ECGX_write_cmos_sensor(0x0F12, u4frameTime); //                                               
            S5K4ECGX_write_cmos_sensor(0x0F12, u4frameTime); //                                                
            break;
         case MSDK_SCENARIO_ID_CAMERA_ZSD:
            S5K4ECGX_write_cmos_sensor(0x002A ,0x0322);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0535); //                                               
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x0341); //                                                
            break;
      }
    }


    S5K4ECGX_write_cmos_sensor(0x002A  ,0x026A);
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                             
    S5K4ECGX_write_cmos_sensor(0x002A  ,0x0268);
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                           
    S5K4ECGX_write_cmos_sensor(0x002A  ,0x024E);
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                       
    //             
    //                                                                     
    S5K4ECGX_write_cmos_sensor(0x0028  ,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A  ,0x023E);
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                       
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                              
    return;
}



static void S5K4ECGX_MIPI_Preview_Mode_Setting(kal_uint8 preview_mode)
{
    unsigned int cap_en = 0;
    unsigned int frameTime;

    if(SEN_RUN_TEST_PATTERN)
    {
        S5K4ECGX_MIPI_SetTestPatternMode(1);
    }
    SENSORDB("\n[4EC] Preview_Mode_Setting+: PVmode=%d\r\n", preview_mode);
    if (preview_mode > S5K4EC_PREVIEW_FULLSIZE_MODE)
    {
        preview_mode = S5K4EC_PREVIEW_MODE;
    }

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    cap_en = s5k4ec_cap_enable;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    if (cap_en)
    {
        //                          

        //             
        //                       
#if 0
        S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
        S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
        S5K4ECGX_write_cmos_sensor(0x002E,0x2C28);
        frameTime = (S5K4ECGX_read_cmos_sensor(0x0F12) & 0xFFFF) ;
        frameTime += (S5K4ECGX_read_cmos_sensor(0x0F12) & 0xFFFF)<<16 ;
        frameTime /= 400; //  
        //               
#endif
        frameTime = S5K4ECGX_MIPI_GetExposureTime();
        SENSORDB("[4EC] Preview_Mode_Setting: frameTime=%d ms\n", frameTime);

        S5K4ECGX_write_cmos_sensor(0x0028 ,0x7000);
        S5K4ECGX_write_cmos_sensor(0x002A ,0x023E);
        S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0000); //                       
        S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0001); //                              
        S5K4ECGX_write_cmos_sensor(0x002A ,0x0242);
        S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0000); //                       
        S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0001); //                              


        spin_lock(&s5k4ecgx_mipi_drv_lock);
        s5k4ec_cap_enable = 0;
        spin_unlock(&s5k4ecgx_mipi_drv_lock);
        //                                                                   
        Sleep(frameTime);//      
    }

    //        
    //                                                           
    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028 ,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A ,0x18AC);
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0060); //                  
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0060); //                    
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x07DC); //                  
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x05C0); //                    

    S5K4ECGX_write_cmos_sensor(0x002A ,0x0250);
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0A00); //                          
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0780); //                           
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0010); //                          
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x000C); //                           

    S5K4ECGX_write_cmos_sensor(0x002A ,0x0494);
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0A00); //                           
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0780); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0000); //                              
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0000); //                               
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0A00); //                           
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0780); //                            
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0000); //                              
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0000); //                               

    S5K4ECGX_write_cmos_sensor(0x002A ,0x0262);
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0001); //                             
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0001); //                           

    S5K4ECGX_write_cmos_sensor(0x002A ,0x0AD4);
    S5K4ECGX_write_cmos_sensor(0x0F12 ,0x003C); //                                 

    //              
    S5K4ECGX_write_cmos_sensor(0x002A  ,0x0266);
    S5K4ECGX_write_cmos_sensor(0x0F12  ,preview_mode);  //                          
    S5K4ECGX_write_cmos_sensor(0x002A  ,0x026A);
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                             

    S5K4ECGX_write_cmos_sensor(0x002A  ,0x0268);
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                           

    S5K4ECGX_write_cmos_sensor(0x002A  ,0x024E);
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                       


    //             
    //                                                                     
    S5K4ECGX_write_cmos_sensor(0x0028  ,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A  ,0x023E);
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                       
    S5K4ECGX_write_cmos_sensor(0x0F12  ,0x0001);  //                              
    return;
}

static void S5K4ECGX_Preview_Mode_Setting(kal_uint8 preview_mode )
{

        SENSORDB("[Enter]:Preview mode: mode=%d\r\n", preview_mode);

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xd000);
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);

    S5K4ECGX_write_cmos_sensor(0x002A, 0x0250);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0494);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0262);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);

    S5K4ECGX_write_cmos_sensor(0x002A, 0x0266);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000+preview_mode);      //                           
    S5K4ECGX_write_cmos_sensor(0x002A, 0x026A);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);       //                              
    S5K4ECGX_write_cmos_sensor(0x002A, 0x024E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);       //                        
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0268);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);       //                            
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0270);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);       //                           
    S5K4ECGX_write_cmos_sensor(0x002A, 0x023E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);       //                        
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);

        SENSORDB("[Exit]:Preview mode:\r\n");

}


UINT32 S5K4ECGX_MIPI_StopPreview(void)
{
  unsigned int status = 1;
  unsigned int prev_en = 1;

    {
      unsigned int frameTime;
      //                                              
#if 0
      S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
      S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
      S5K4ECGX_write_cmos_sensor(0x002E,0x2C28);
      frameTime = (S5K4ECGX_read_cmos_sensor(0x0F12) & 0xFFFF) ;
      frameTime += (S5K4ECGX_read_cmos_sensor(0x0F12) & 0xFFFF)<<16 ;
      frameTime /= 400; //  
      //               
#endif
      frameTime = S5K4ECGX_MIPI_GetExposureTime();
      SENSORDB("[4EC] StopPreview: frameTime=%d ms\n", frameTime);

      S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
      S5K4ECGX_write_cmos_sensor(0x002A, 0x023E);
      S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                       
      S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                              
      S5K4ECGX_write_cmos_sensor(0x002A, 0x0242);
      S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                       
      S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                              

      Sleep(frameTime);//      
      //                            
      SENSORDB("[4EC]StopPreview- \n");
    }
    return 1;
}



static void S5K4ECGX_MIPI_Capture_Mode_Setting(kal_uint8 capture_mode)
{
    unsigned int status = 0;
    unsigned int frameTime;

    SENSORDB("[4EC] Capture_Mode_Setting+\n");

    S5K4ECGX_MIPI_AF_CancelFocus(); //               

    S5K4ECGX_MIPI_StopPreview();

    //                

    //       
    //                                          
    //                                                               


    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0258);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00); //                                 
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780); //                                  
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0010); //                                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x000C); //                                           
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0264);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                           

    S5K4ECGX_write_cmos_sensor(0x002A, 0x03B4);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0535);   //                                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0341);   //                                  

    S5K4ECGX_write_cmos_sensor(0x002A, 0x049C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0A00); //                                        
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0780); //                                         
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                    
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000); //                                     

    //                                       
    S5K4ECGX_write_cmos_sensor(0x002A, 0x047C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                          
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0280); //                               
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x01E0); //                                

    S5K4ECGX_MIPI_Config_JPEG_Capture(&S5K4ECGX_Driver.jpegSensorPara);

    //           
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002a, 0x026e);
    S5K4ECGX_write_cmos_sensor(0x0f12, 0x0000); //                         

    //                                                                    
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0242);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                       
    S5K4ECGX_write_cmos_sensor(0x002A, 0x024E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                       
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0244);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                              
    Sleep(5);

    frameTime = S5K4ECGX_MIPI_GetExposureTime();
    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.capExposureTime = frameTime;
    s5k4ec_cap_enable = 1;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    S5K4ECGX_MIPI_GetAutoISOValue();

    SENSORDB("[4EC] Capture_Mode_Setting-\n");
}




static void S5K4ECGX_MIPI_HVMirror(kal_uint8 image_mirror)
{
    /*                                                       
                                                         
                                                         
                                                            */

    //                              

    //                                                      
    //           

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xd000);
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);


    switch (image_mirror)
    {
        case IMAGE_NORMAL:
        default:
            S5K4ECGX_write_cmos_sensor(0x002A,  0x02D0);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0300);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0330);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0360);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0390);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0000);  //                            
            break;

        case IMAGE_H_MIRROR:
            S5K4ECGX_write_cmos_sensor(0x002A,  0x02D0);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0300);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0330);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0360);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0390);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0001);  //                            
            break;

        case IMAGE_V_MIRROR:
            S5K4ECGX_write_cmos_sensor(0x002A,  0x02D0);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0300);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0330);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0360);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0390);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0002);  //                            
            break;

        case IMAGE_HV_MIRROR:
            S5K4ECGX_write_cmos_sensor(0x002A,  0x02D0);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0300);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0330);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0360);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                            

            S5K4ECGX_write_cmos_sensor(0x002A,  0x0390);
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                         
            S5K4ECGX_write_cmos_sensor(0x0F12,  0x0003);  //                            
            break;
    }



    //                                   
    //                                                  
    //                                     
}




void S5K4ECGX_MIPI_SceneOffMode()
{
    MSDK_SCENARIO_ID_ENUM scen;
    spin_lock(&s5k4ecgx_mipi_drv_lock);
    scen = S5K4ECGXCurrentScenarioId;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

   //                                   

   //                                           
   //                                           
   //                                           
   //                                                            

   //            
   S5K4ECGX_write_cmos_sensor(0x002A, 0x02C2);
   S5K4ECGX_write_cmos_sensor(0x0F12, 0x0535);  //                                                         
   S5K4ECGX_write_cmos_sensor(0x0F12, 0x014d);  //                                                        

   S5K4ECGX_write_cmos_sensor(0x002A, 0x0322);
   S5K4ECGX_write_cmos_sensor(0x0F12, 0x0535);  //                                                 
   S5K4ECGX_write_cmos_sensor(0x0F12, 0x0341);  //                                      

   S5K4ECGX_write_cmos_sensor(0x002A, 0x03B4);
   S5K4ECGX_write_cmos_sensor(0x0F12, 0x0535);  //                                                 
   S5K4ECGX_write_cmos_sensor(0x0F12, 0x0341);  //                                                          

   if (((AWB_MODE_OFF == S5K4ECYX_MIPICurrentStatus.iWB) ||
        (AWB_MODE_AUTO == S5K4ECYX_MIPICurrentStatus.iWB)))
   {
       S5K4ECGX_write_cmos_sensor(0x002A, 0x04E6);
       S5K4ECGX_write_cmos_sensor(0x0F12, 0x077F);  //                        
   }

    return;
}




/*                                                                        
          
                 
 
             
                                         
 
            
                                                                          
                                                                               
 
         
      
 
                  
 
                                                                        */
static UINT32 S5K4ECGX_MIPI_Preview(
    MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
    MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    unsigned int scene = 0;
    unsigned int width, height;
    SENSORDB("[4EC]Preview+ PV_Mode=%d\n", sensor_config_data->SensorOperationMode);

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    width = S5K4ECGX_Driver.Preview_Width;
    height = S5K4ECGX_Driver.Preview_Height;

    scene = S5K4ECGXCurrentScenarioId;
    S5K4ECGX_Driver.Period_PixelNum = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV + S5K4ECGX_Driver.Dummy_Pixels;
    S5K4ECGX_Driver.Period_LineNum = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV + S5K4ECGX_Driver.Dummy_Lines;
    S5K4ECGX_Driver.Preview_Width = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV;
    S5K4ECGX_Driver.Preview_Height = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV;
    S5K4ECGX_Driver.Camco_mode = S5K4ECGX_CAM_PREVIEW;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    if (MSDK_SCENARIO_ID_CAMERA_PREVIEW == scene)
    {
       S5K4ECGX_MIPI_Preview_Mode_Setting(S5K4EC_PREVIEW_MODE);
       if ((width == S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV) ||
           (height == S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV))
       {
           //                                          
           #if 0
           SENSORDB("[4EC] Reset AE table");
           S5K4ECGX_MIPI_AE_Set_Window(S5K4ECGX_Driver.apAEWindows,
                                       S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV,
                                       S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV);
           #endif
       }
    }
    else
    {
       S5K4ECGX_MIPI_Preview_Mode_Setting(S5K4EC_VIDEO_MODE);
    }

    image_window->GrabStartX = S5K4ECGX_PV_X_START;
    image_window->GrabStartY = S5K4ECGX_PV_Y_START;
    image_window->ExposureWindowWidth = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH - 2 * S5K4ECGX_PV_X_START;
    image_window->ExposureWindowHeight = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT - 2 * S5K4ECGX_PV_Y_START;

    memcpy(&S5K4ECGXSensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

    SENSORDB("[4EC]Preview-\n");
    return ERROR_NONE;
} /*                 */

/*                                                                        
          
                   
 
             
                                           
 
            
                                                                           
                                                                               
 
         
        
 
                  
 
                                                                        */
static UINT32 S5K4ECGXPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{

    SENSORDB("[Enter]:S5K4ECGX preview func:");

    S5K4ECGX_MIPI_HVMirror(sensor_config_data->SensorImageMirror);
    S5K4ECGX_Preview_Mode_Setting(0); //                

    image_window->GrabStartX = S5K4ECGX_PV_X_START;
    image_window->GrabStartY = S5K4ECGX_PV_Y_START;
    image_window->ExposureWindowWidth = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH-S5K4ECGX_PV_X_START;
    image_window->ExposureWindowHeight = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT-S5K4ECGX_PV_Y_START;

    memcpy(&S5K4ECGXSensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

    SENSORDB("[Exit]:S5K4ECGX preview func\n");
    return ERROR_NONE;
}

static UINT32 S5K4ECGX_MIPI_Preview_ZSD(
    MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
    MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    unsigned int width, height;


    SENSORDB("[4EC]ZSD+");
    spin_lock(&s5k4ecgx_mipi_drv_lock);
    //                                                                                                
    width = S5K4ECGX_Driver.Preview_Width;
    height = S5K4ECGX_Driver.Preview_Height;
    S5K4ECGX_Driver.Period_PixelNum = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV + S5K4ECGX_Driver.Dummy_Pixels;
    S5K4ECGX_Driver.Period_LineNum = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV + S5K4ECGX_Driver.Dummy_Lines;
    S5K4ECGX_Driver.Preview_Width = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV;
    S5K4ECGX_Driver.Preview_Height = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV;
    S5K4ECGX_Driver.Camco_mode = S5K4ECGX_CAM_PREVIEW;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    if ((width == S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV) ||
        (height == S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV))
    {
        //                                          
        S5K4ECGX_MIPI_AE_Set_Window(S5K4ECGX_Driver.apAEWindows,
                                    S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV,
                                    S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV);
    }

    S5K4ECGX_MIPI_Preview_Mode_Setting(S5K4EC_PREVIEW_FULLSIZE_MODE); //                

    image_window->GrabStartX = S5K4ECGX_FULL_X_START;
    image_window->GrabStartY = S5K4ECGX_FULL_Y_START;
    image_window->ExposureWindowWidth = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV - 2 * S5K4ECGX_PV_X_START;
    image_window->ExposureWindowHeight = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV - 2 * S5K4ECGX_PV_Y_START;

    memcpy(&S5K4ECGXSensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));

    SENSORDB("[4EC]ZSD-");
    return ERROR_NONE;
} /*                 */




UINT32 S5K4ECGX_MIPI_Capture(
    MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
    MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    kal_uint32 shutter = S5K4ECGX_Driver.shutter;
    kal_uint32 gain = 0;

    if (SCENE_MODE_HDR == S5K4ECGX_Driver.sceneMode)
    {
        S5K4ECGX_MIPI_get_exposure_gain();
    }
    //                                                                     
    // 
    //                                            
    // 

    //                             
    //                                      
    //                                

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.shutter = shutter;
    S5K4ECGX_Driver.sensorGain = gain;
    S5K4ECGX_Driver.Camco_mode = S5K4ECGX_CAM_CAPTURE;
    S5K4ECGX_Driver.StartX = S5K4ECGX_FULL_X_START;//                   
    S5K4ECGX_Driver.StartY = S5K4ECGX_FULL_Y_START;//                   
    spin_unlock(&s5k4ecgx_mipi_drv_lock);;

    //                                                
    SENSORDB("[4EC]Capture+\r\n");

    S5K4ECGX_MIPI_Capture_Mode_Setting(0);

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.iGrabWidth = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV -2 * S5K4ECGX_FULL_X_START;
    S5K4ECGX_Driver.iGrabheight = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV -2 * S5K4ECGX_FULL_Y_START;
    image_window->GrabStartX = S5K4ECGX_FULL_X_START;
    image_window->GrabStartY = S5K4ECGX_FULL_Y_START;
    image_window->ExposureWindowWidth = S5K4ECGX_Driver.iGrabWidth;
    image_window->ExposureWindowHeight = S5K4ECGX_Driver.iGrabheight;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);;

#if defined(__CAPTURE_JPEG_OUTPUT__)

    if (S5K4ECGX_Driver.manualAEStart)
    {
        //                               
        //                                                           
        //                                         
    }

#endif

}


UINT32 S5K4ECGX_MIPI_GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
    //                                                   

    pSensorResolution->SensorFullWidth=S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV - 2*S5K4ECGX_FULL_X_START;
    pSensorResolution->SensorFullHeight=S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV - 2*S5K4ECGX_FULL_Y_START;
    switch(S5K4ECGXCurrentScenarioId)
    {
       case MSDK_SCENARIO_ID_CAMERA_ZSD:
            pSensorResolution->SensorPreviewWidth   = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV - 2*S5K4ECGX_FULL_X_START;
            pSensorResolution->SensorPreviewHeight  = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV - 2*S5K4ECGX_FULL_Y_START;
            break;
       default:
            pSensorResolution->SensorPreviewWidth   = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV - 2*S5K4ECGX_PV_X_START;
            pSensorResolution->SensorPreviewHeight  = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV - 2*S5K4ECGX_PV_Y_START;
            pSensorResolution->SensorVideoWidth  = pSensorResolution->SensorPreviewWidth;
            pSensorResolution->SensorVideoHeight = pSensorResolution->SensorPreviewHeight;
            break;
    }

    pSensorResolution->Sensor3DFullWidth = 0;
    pSensorResolution->Sensor3DFullHeight= 0;
    pSensorResolution->Sensor3DPreviewWidth = 0;
    pSensorResolution->Sensor3DPreviewHeight = 0;
    //                                                  
    return ERROR_NONE;
} /*                                 */


UINT32 S5K4ECGX_MIPI_GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
    MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
    MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    //                                                                        

    switch(ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
          pSensorInfo->SensorPreviewResolutionX = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV;
          pSensorInfo->SensorPreviewResolutionY = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV;
          pSensorInfo->SensorCameraPreviewFrameRate=15;
          break;
        default:
          pSensorInfo->SensorPreviewResolutionX = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV;
          pSensorInfo->SensorPreviewResolutionY = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV;
          pSensorInfo->SensorCameraPreviewFrameRate=30;
          break;
    }

    pSensorInfo->SensorFullResolutionX = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV;
    pSensorInfo->SensorFullResolutionY = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV;
    //                                                   
    pSensorInfo->SensorVideoFrameRate = 30;
    pSensorInfo->SensorStillCaptureFrameRate = 5;
    pSensorInfo->SensorWebCamCaptureFrameRate = 15;
    pSensorInfo->SensorResetActiveHigh=FALSE;//               
    pSensorInfo->SensorResetDelayCount=4;  //   

    pSensorInfo->SensorOutputDataFormat=SENSOR_OUTPUT_FORMAT_YUYV;
    pSensorInfo->SensorCaptureOutputJPEG = FALSE;
#if defined(__CAPTURE_JPEG_OUTPUT__)
    pSensorInfo->SensorCaptureOutputJPEG = TRUE;
#endif
    pSensorInfo->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW;

#ifdef MIPI_INTERFACE
    pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
#else
    pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
    pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
#endif
    pSensorInfo->SensorInterruptDelayLines = 1;

#ifdef MIPI_INTERFACE
      pSensorInfo->SensorInterruptDelayLines = 2;
      pSensorInfo->SensroInterfaceType = SENSOR_INTERFACE_TYPE_MIPI;
#else
      pSensorInfo->SensroInterfaceType = SENSOR_INTERFACE_TYPE_PARALLEL;
#endif

    pSensorInfo->SensorMasterClockSwitch = 0;
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_4MA;
    spin_lock(&s5k4ecgx_mipi_drv_lock);
    pSensorInfo->CaptureDelayFrame = 3; //                                             
    spin_unlock(&s5k4ecgx_mipi_drv_lock);
    pSensorInfo->PreviewDelayFrame = 0;
    pSensorInfo->VideoDelayFrame = 0;

    //                 
    pSensorInfo->YUVAwbDelayFrame = 5;
    pSensorInfo->YUVEffectDelayFrame = 3;

    //                                
    pSensorInfo->AEShutDelayFrame = 0;

    //                                             
    //                                                    
    //                                                                                     

//           
#ifdef MIPI_INTERFACE  //             
    pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE;
    pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
    pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 4;
    pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
    pSensorInfo->SensorWidthSampling = 0;   //                
    pSensorInfo->SensorHightSampling = 0;   //                
    pSensorInfo->SensorPacketECCOrder = 1;
#endif

    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        //                                          
             pSensorInfo->SensorClockFreq = S5K4ECGX_MCLK;
             pSensorInfo->SensorClockDividCount = 5;
             pSensorInfo->SensorClockRisingCount = 0;
             pSensorInfo->SensorClockFallingCount = 2;
             pSensorInfo->SensorPixelClockCount = 3;
             pSensorInfo->SensorDataLatchCount = 2;
             pSensorInfo->SensorGrabStartX = S5K4ECGX_PV_X_START;
             pSensorInfo->SensorGrabStartY = S5K4ECGX_PV_Y_START;
             break;

        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        //                                         
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
             pSensorInfo->SensorClockFreq = S5K4ECGX_MCLK;
             pSensorInfo->SensorClockDividCount = 5;
             pSensorInfo->SensorClockRisingCount = 0;
             pSensorInfo->SensorClockFallingCount = 2;
             pSensorInfo->SensorPixelClockCount = 3;
             pSensorInfo->SensorDataLatchCount = 2;
             pSensorInfo->SensorGrabStartX = S5K4ECGX_FULL_X_START;
             pSensorInfo->SensorGrabStartY = S5K4ECGX_FULL_Y_START;
             break;

        default:
             pSensorInfo->SensorClockFreq = S5K4ECGX_MCLK;
             pSensorInfo->SensorClockDividCount = 5;
             pSensorInfo->SensorClockRisingCount = 0;
             pSensorInfo->SensorClockFallingCount = 2;
             pSensorInfo->SensorPixelClockCount =3;
             pSensorInfo->SensorDataLatchCount =2;
             pSensorInfo->SensorGrabStartX = S5K4ECGX_PV_X_START;
             pSensorInfo->SensorGrabStartY = S5K4ECGX_PV_Y_START;
             break;
    }

   //                                            
    return ERROR_NONE;
}  /*                           */


/*                                                                        
          
                           
 
             
                 
 
            
      
 
         
      
 
                  
 
                                                                        */
BOOL S5K4ECGX_MIPI_set_param_effect(UINT16 para)
{

   SENSORDB("[4EC] set_param_effect func:para = %d\n",para);
   switch (para)
   {
      case MEFFECT_NEGATIVE:
          S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
          S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
          S5K4ECGX_write_cmos_sensor(0x002A,0x023c);
          S5K4ECGX_write_cmos_sensor(0x0F12,0x0003);  //                        
          break;
      case MEFFECT_SEPIA:
          S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
          S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
          S5K4ECGX_write_cmos_sensor(0x002A,0x023c);
          S5K4ECGX_write_cmos_sensor(0x0F12,0x0004);  //                        
          break;
      case MEFFECT_SEPIABLUE:
          S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
          S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
          S5K4ECGX_write_cmos_sensor(0x002A,0x023c);
          S5K4ECGX_write_cmos_sensor(0x0F12,0x0007);  //                        
          break;
      case MEFFECT_SEPIAGREEN:
          S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
          S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
          S5K4ECGX_write_cmos_sensor(0x002A,0x023c);
          S5K4ECGX_write_cmos_sensor(0x0F12,0x0008);  //                        
          break;
      case MEFFECT_MONO:
          S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
          S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
          S5K4ECGX_write_cmos_sensor(0x002A,0x023c);
          S5K4ECGX_write_cmos_sensor(0x0F12,0x0001);  //                        
          break;
      default:
    case MEFFECT_OFF:
      S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
      S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
      S5K4ECGX_write_cmos_sensor(0x002A,0x023c);
      S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);  //                        
      break;
   }
   return KAL_TRUE;
} /*                           */





UINT32 S5K4ECGX_MIPI_Control(
    MSDK_SCENARIO_ID_ENUM ScenarioId,
    MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
    MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
   SENSORDB("[4EC]Control Scenario = 0x%04x\n",ScenarioId);

   spin_lock(&s5k4ecgx_mipi_drv_lock);
   S5K4ECGXCurrentScenarioId = ScenarioId;
   spin_unlock(&s5k4ecgx_mipi_drv_lock);

   switch (ScenarioId)
   {
      case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
      case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
#ifdef MIPI_INTERFACE
         S5K4ECGX_MIPI_Preview(pImageWindow, pSensorConfigData);
#else
         S5K4ECGXPreview(pImageWindow, pSensorConfigData);
#endif
         break;
      case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
         S5K4ECGX_MIPI_Capture(pImageWindow, pSensorConfigData);
         break;
      case MSDK_SCENARIO_ID_CAMERA_ZSD:
     S5K4ECGX_MIPI_Preview_ZSD(pImageWindow, pSensorConfigData);
         break;
      default:
         break;
   }

   //                                                 
   return ERROR_NONE;
} /*                   */

/* 
                                 
  
        
                               
 */
UINT32 S5K4ECGX_MIPI_AF_Infinity()
{
    SENSORDB("[Enter] S5K4ECGX Set_infinity_af()\n");

    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002a, 0x028E);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);
    Sleep(100);
    S5K4ECGX_write_cmos_sensor(0x002a, 0x028C);
    S5K4ECGX_write_cmos_sensor(0x0F12, 0x0004);
}

/* 
                         
  
        
                               
                                               
 */
static void S5K4ECGX_MIPI_AE_Get_Status(UINT32 *pFeatureReturnPara32)
{
    SENSORDB("[Enter] S5K4ECGX GET_AE_STATUS\n");

    //                                                
    unsigned int aStatus; //                                                                       
    unsigned int aeStable; //                       
    kal_bool aeEnable;

    /*                               
                                
                              
                                 
                                               
                                                
     */

    //                                                                            

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x04E6);
    aStatus = S5K4ECGX_read_cmos_sensor(0x0F12); //                                                                       
    aeEnable = (aStatus & (1 << 1)) == 0 ? false : true;

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x2C74);
    aeStable = S5K4ECGX_read_cmos_sensor(0x0F12); //                      

    //           
    if(false == aeEnable)
    {
        *pFeatureReturnPara32 = SENSOR_AE_IDLE;
    }
    else if(true == S5K4ECGX_Driver.userAskAeLock)
    {
        *pFeatureReturnPara32 = SENSOR_AE_LOCKED;
    }
    else if(1 == aeStable)
    {
        *pFeatureReturnPara32 = SENSOR_AE_EXPOSED;
    }
    else
    {
        *pFeatureReturnPara32 = SENSOR_AE_EXPOSING;
    }

}

/* 
                          
  
        
                               
                                               
 */
static void S5K4ECGX_MIPI_AWB_Get_Status(UINT32 *pFeatureReturnPara32)
{
    SENSORDB("[Enter] S5K4ECGX GET_AWB_STATUS\n");

    //                                                
    unsigned int aStatus; //                                                                       
    unsigned int awbStable; //                       
    kal_bool awbEnable;

    /*                                
                                 
                               
                                  
                                                
                                                 
     */

    //                                                                            

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x04E6);
    aStatus = S5K4ECGX_read_cmos_sensor(0x0F12); //                                                                       
    awbEnable = (aStatus & (1 << 3)) == 0 ? false : true;

    S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
    S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002E,0x2C78);
    awbStable = S5K4ECGX_read_cmos_sensor(0x0F12); //                      

    //            
    if(false == awbEnable)
    {
        *pFeatureReturnPara32 = SENSOR_AWB_IDLE;
    }
    else if(true == S5K4ECGX_Driver.userAskAwbLock)
    {
        *pFeatureReturnPara32 = SENSOR_AWB_LOCKED;
    }
    else if(1 == awbEnable)
    {
        *pFeatureReturnPara32 = SENSOR_AWB_BALANCED;
    }
    else
    {
        *pFeatureReturnPara32 = SENSOR_AWB_BALANCING;
    }

}



/*                                                                        
          
                       
 
             
             
 
            
      
 
         
      
 
                  
 
                                                                        */

BOOL S5K4ECGX_MIPI_set_param_wb(UINT16 para)
{

    //                                          
    //                                                                                    
    //                                                               
    kal_uint16 Status_3A=0;
    kal_int16 iter = 30;

    while ((Status_3A==0) && (iter-- > 0))
    {

        S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
        S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
        S5K4ECGX_write_cmos_sensor(0x002E,0x04E6);
        Status_3A=S5K4ECGX_read_cmos_sensor(0x0F12); //                                                               

        Sleep(1);
    }
    SENSORDB("[Enter]S5K4ECGX AWB_MODE\n");

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.awbMode = para;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    //                                                              
    /*
                             
     
                                           
                                               
                                             
                                   
     
    */


    switch (para)
    {
      case AWB_MODE_AUTO:
      {
           Status_3A = (Status_3A | 0x8); //           
           S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
           S5K4ECGX_write_cmos_sensor(0x002a, 0x04e6);//
           S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);//
           //                                             
           //                                            
      }
      break;

      case AWB_MODE_OFF:
      /*
       
                                             
                                                
                                               
                                   
       
      */
      break;

      case AWB_MODE_CLOUDY_DAYLIGHT:
      {
           Status_3A = (Status_3A & 0xFFF7); //            
           S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04E6);
           S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);
           //                                           
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04BA);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x01D0); //                 
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0140); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           //                                                       
      }
      break;
      case AWB_MODE_DAYLIGHT:
      {
           Status_3A = (Status_3A & 0xFFF7); //            
           S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04E6);
           S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);
           //                                           
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04BA);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0190); //                 
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0150); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           //                                                

      }
      break;
      case AWB_MODE_INCANDESCENT:
      {
           Status_3A = (Status_3A & 0xFFF7); //            
           S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04E6);
           S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);
           //                                           
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04BA);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0118); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x026F); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           //                                                    
      }
      break;
      case AWB_MODE_FLUORESCENT:
      {
           Status_3A = (Status_3A & 0xFFF7); //            
           S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04E6);
           S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);
           //                                           
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04BA);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0180); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0240); //                 
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           //                                                   
      }
      break;
      case AWB_MODE_TUNGSTEN:
      {
           Status_3A = (Status_3A & 0xFFF7); //            
           S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04E6);
           S5K4ECGX_write_cmos_sensor(0x0F12, Status_3A);
           //                                           
           S5K4ECGX_write_cmos_sensor(0x002A, 0x04BA);
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0110); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0120); //                     
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0320); //                 
           S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001); //                        
           //                                                
      }
      break;
      default:
        //                                                 
        break;//             
    }


    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECYX_MIPICurrentStatus.iWB = para;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    //                                                  
    return TRUE;
} /*                       */


/*                                                                        
          
                            
 
             
                  
 
            
      
 
         
      
 
                  
 
                                                                        */
BOOL S5K4ECGX_MIPI_set_param_banding(UINT16 para)
{
   //                                                                    
   kal_uint16 Status_3A=0;
   kal_int16 iter = 30;

   if(S5K4ECGX_Driver.Banding == para)
       return TRUE;

   spin_lock(&s5k4ecgx_mipi_drv_lock);
   S5K4ECGX_Driver.Banding = para;
   spin_unlock(&s5k4ecgx_mipi_drv_lock);;


   while ((Status_3A==0) && (iter-- > 0))
   {

      S5K4ECGX_write_cmos_sensor(0xFCFC,0xd000);
      S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
      S5K4ECGX_write_cmos_sensor(0x002E,0x04E6); //                        
      Status_3A=S5K4ECGX_read_cmos_sensor(0x0F12);

      Sleep(1);
   }



   switch (para)
   {
       case AE_FLICKER_MODE_50HZ:
       {
          Status_3A = (Status_3A & 0xFFDF); //                           
          S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
          S5K4ECGX_write_cmos_sensor(0x002a, 0x04e6);
          S5K4ECGX_write_cmos_sensor(0x0f12, Status_3A);
          //                                           
          S5K4ECGX_write_cmos_sensor(0x002a, 0x04d6);
          S5K4ECGX_write_cmos_sensor(0x0f12, 0x0001); //            
          S5K4ECGX_write_cmos_sensor(0x0f12, 0x0001); //                         
          //                                                   
       }
       break;

       case AE_FLICKER_MODE_60HZ:
       {
          Status_3A = (Status_3A & 0xFFDF); //                     
          S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
          S5K4ECGX_write_cmos_sensor(0x002a, 0x04e6);
          S5K4ECGX_write_cmos_sensor(0x0f12, Status_3A);
          //                                           
          S5K4ECGX_write_cmos_sensor(0x002a, 0x04d6);
          S5K4ECGX_write_cmos_sensor(0x0f12, 0x0002); //            
          S5K4ECGX_write_cmos_sensor(0x0f12, 0x0001); //                         
          //                                                   

       }
       break;

       case AE_FLICKER_MODE_OFF:
       {
           Status_3A = (Status_3A & 0xFFDF); //                           
           S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
           S5K4ECGX_write_cmos_sensor(0x002a, 0x04e6);
           S5K4ECGX_write_cmos_sensor(0x0f12, Status_3A);
           //                                           
           S5K4ECGX_write_cmos_sensor(0x002a, 0x04d6);
           S5K4ECGX_write_cmos_sensor(0x0f12, 0x0000); //           
           S5K4ECGX_write_cmos_sensor(0x0f12, 0x0001); //                         
           //                                                  
       }
       break;

       case AE_FLICKER_MODE_AUTO:
       default:
       {
          //            
          Status_3A = (Status_3A | 0x5); //                    
          S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
          S5K4ECGX_write_cmos_sensor(0x002a, 0x04e6);
          S5K4ECGX_write_cmos_sensor(0x0f12, Status_3A);
          //                                           
          //                                                   
          break;
       }
   }


   SENSORDB("S5K4ECGXparam_banding Status_3A = 0x%x\n",Status_3A);
   return KAL_TRUE;
} /*                            */



//                                                                        
/*                                                                        
          
                                     
 
             
                   
 
            
      
 
         
      
 
                  
 
                                                                        */
#define S5K4ECGX_MIPI_MAX_AXD_GAIN (12 << 8) //             
#define S5K4ECGX_MIPI_MAX_EXPOSURE_TIME ( 133 * 100) //                          

//                                                                        
BOOL S5K4ECGX_MIPI_set_param_exposure_for_HDR(UINT16 para)
{
    kal_uint32 totalGain = 0, exposureTime = 0;
    spin_lock(&s5k4ecgx_mipi_drv_lock);

    SENSORDB("[4EC] S5K4ECGX_MIPI_set_param_exposure_for_HDR\n");
    if (0 == S5K4ECGX_Driver.manualAEStart)
    {
        S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
        S5K4ECGX_write_cmos_sensor(0x002A,0x04E6);
        S5K4ECGX_write_cmos_sensor(0x0F12,0x0779); //                
        S5K4ECGX_Driver.manualAEStart = 1;
    }

  switch (para)
  {
     case AE_EV_COMP_20:  //     
     case AE_EV_COMP_10:  //      
           totalGain = S5K4ECGX_Driver.currentAxDGain << 1;
           exposureTime = S5K4ECGX_Driver.currentExposureTime << 1;
           SENSORDB("[4EC] HDR AE_EV_COMP_20\n");
           break;
     case AE_EV_COMP_00:  //      
           totalGain = S5K4ECGX_Driver.currentAxDGain;
           exposureTime = S5K4ECGX_Driver.currentExposureTime;
           SENSORDB("[4EC] HDR AE_EV_COMP_00\n");
           break;
     case AE_EV_COMP_n10:  //      
     case AE_EV_COMP_n20:  //      
           totalGain = S5K4ECGX_Driver.currentAxDGain >> 1;
           exposureTime = S5K4ECGX_Driver.currentExposureTime >> 1;
           SENSORDB("[4EC] HDR AE_EV_COMP_n20\n");
           break;
     default:
           break;//             
  }
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    totalGain = (totalGain > S5K4ECGX_MIPI_MAX_AXD_GAIN) ? S5K4ECGX_MIPI_MAX_AXD_GAIN : totalGain;
    exposureTime = (exposureTime > S5K4ECGX_MIPI_MAX_EXPOSURE_TIME) ? S5K4ECGX_MIPI_MAX_EXPOSURE_TIME : exposureTime;

    SENSORDB("[4EC] HDR set totalGain=0x%x gain(x%d)\n", totalGain, totalGain>>8);
    SENSORDB("[4EC] HDR set exposureTime=0x%x exposureTime=%dms \n", exposureTime, exposureTime/100);

    S5K4ECGX_write_cmos_sensor(0x002A,0x04AC);
    S5K4ECGX_write_cmos_sensor(0x0F12,exposureTime); //             
    S5K4ECGX_write_cmos_sensor(0x002A,0x04B0);
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0001); //                    
    S5K4ECGX_write_cmos_sensor(0x002A,0x04B2);
    S5K4ECGX_write_cmos_sensor(0x0F12,totalGain);   //          
    S5K4ECGX_write_cmos_sensor(0x0F12,0x0001);

    return TRUE;
}

/*                                                                        
          
                             
 
             
                   
 
            
      
 
         
      
 
                  
 
                                                                        */
BOOL S5K4ECGX_MIPI_set_param_exposure(UINT16 para)
{
   kal_uint16 base_target = 0;

   SENSORDB("[Enter]S5K4ECGX set_param_exposure func:para = %d\n",para);

   spin_lock(&s5k4ecgx_mipi_drv_lock);
   if (SCENE_MODE_HDR == S5K4ECGX_Driver.sceneMode &&
       S5K4ECGX_CAM_CAPTURE == S5K4ECGX_Driver.Camco_mode)
   {
       spin_unlock(&s5k4ecgx_mipi_drv_lock);
       S5K4ECGX_MIPI_set_param_exposure_for_HDR(para);
       return TRUE;
   }
   spin_unlock(&s5k4ecgx_mipi_drv_lock);

   switch (para)
   {
      case AE_EV_COMP_30:  //     
           S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A,0x023A);  //                 
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0280);  //             
           break;
      case AE_EV_COMP_20:  //     
           S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A,0x023A);  //                 
           S5K4ECGX_write_cmos_sensor(0x0F12,0x01E0);  //             
           S5K4ECGX_write_cmos_sensor(0x002A,0x098C);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000F);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0A42);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000F);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0AF8);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000F);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0BAE);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000F);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0C64);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000F);  //

           break;
      case AE_EV_COMP_10:  //      
           S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A,0x023A);  //                 
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0160);  //             

           S5K4ECGX_write_cmos_sensor(0x002A,0x098C);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000A);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0A42);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000A);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0AF8);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000A);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0BAE);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000A);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0C64);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x000A);  //

           break;
      case AE_EV_COMP_00:  //      
           S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A,0x023A);  //                 
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0100);  //             

           S5K4ECGX_write_cmos_sensor(0x002A,0x098C);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0A42);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0AF8);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0BAE);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0C64);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);  //

           break;
      case AE_EV_COMP_n10:  //      
           S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A,0x023A);  //                 
           S5K4ECGX_write_cmos_sensor(0x0F12,0x00B0);  //             

           S5K4ECGX_write_cmos_sensor(0x002A,0x098C);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFFB);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0A42);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFFB);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0AF8);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFFB);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0BAE);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFFB);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0C64);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFFB);  //
           break;
      case AE_EV_COMP_n20:  //      
           S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A,0x023A);  //                 
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0080);  //             

           S5K4ECGX_write_cmos_sensor(0x002A,0x098C);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFF6);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0A42);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFF6);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0AF8);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFF6);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0BAE);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFF6);  //
           S5K4ECGX_write_cmos_sensor(0x002A,0x0C64);  //
           S5K4ECGX_write_cmos_sensor(0x0F12,0xFFF6);  //
           break;
      case AE_EV_COMP_n30:   //     
           S5K4ECGX_write_cmos_sensor(0xFCFC,0xD000);
           S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
           S5K4ECGX_write_cmos_sensor(0x002A,0x023A);  //                 
           S5K4ECGX_write_cmos_sensor(0x0F12,0x0050);  //             
           break;
      default:
           break;//             
   }


   return TRUE;

} /*                             */


void S5K4ECGX_MIPI_get_AEAWB_lock(UINT32 *pAElockRet32,UINT32 *pAWBlockRet32)
{
    *pAElockRet32 = 1;
    *pAWBlockRet32 = 1;
    SENSORDB("[4EC]GetAEAWBLock,AE=%d ,AWB=%d\n,",*pAElockRet32,*pAWBlockRet32);
}



void S5K4ECGX_MIPI_set_brightness(UINT16 para)
{
    SENSORDB("[4EC]Set_Brightness\n");

    S5K4ECGX_write_cmos_sensor(0xFCFC ,0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A ,0x0230);

    switch (para)
    {
        case ISP_BRIGHT_LOW:
             S5K4ECGX_write_cmos_sensor(0x0F12 ,0xFF81);
             break;
        case ISP_BRIGHT_HIGH:
             S5K4ECGX_write_cmos_sensor(0x0F12 ,0x007F);
             break;
        case ISP_BRIGHT_MIDDLE:
        default:
             S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0000);
             break;
    }


    return;
}



void S5K4ECGX_MIPI_set_contrast(UINT16 para)
{
    SENSORDB("[4EC]Set_Contrast\n");

    S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000),
    S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A, 0x0232);  //                 

    switch (para)
    {
        case ISP_CONTRAST_LOW:
             S5K4ECGX_write_cmos_sensor(0x0F12,0xFFC0);
             break;
        case ISP_CONTRAST_HIGH:
             S5K4ECGX_write_cmos_sensor(0x0F12,0x0040);
             break;
        case ISP_CONTRAST_MIDDLE:
        default:
             S5K4ECGX_write_cmos_sensor(0x0F12,0x0000);
             break;
    }

    return;
}



void S5K4ECGX_MIPI_set_iso(UINT16 para)
{

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGX_Driver.isoSpeed = para;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);
    SENSORDB("[4EC]Set_Iso\n");

    switch (para)
    {
        case AE_ISO_100:
             //       
             S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
             S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
             S5K4ECGX_write_cmos_sensor(0x002A, 0x0938);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0014);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D2);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x07D0);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x1388);  //                     
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D6);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                        
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                               
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D0);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                   
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x01C0);  //                  
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                      
             S5K4ECGX_write_cmos_sensor(0x002A, 0x06C2);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);  //                  
             break;
        case AE_ISO_200:
             //       
             S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
             S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
             S5K4ECGX_write_cmos_sensor(0x002A, 0x0938);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0014);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x03A2);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0484);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x07D0);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x1388);  //                     
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D6);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                        
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                               

             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D0);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                   
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0380);  //                  
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                      
             S5K4ECGX_write_cmos_sensor(0x002A, 0x06C2);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);  //                  
             break;
        case AE_ISO_400:
             //       
             S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
             S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
             S5K4ECGX_write_cmos_sensor(0x002A, 0x0938);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0014);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x08D2);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0C84);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x10D0);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x1388);  //                     
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D6);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                        
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                               
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D0);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                   
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0700);  //                  
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                      
             S5K4ECGX_write_cmos_sensor(0x002A, 0x06C2);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0100);  //                  
             break;
        default:
        case AE_ISO_AUTO:
             //        
             S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
             S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
             S5K4ECGX_write_cmos_sensor(0x002A, 0x0938);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0014);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x00D2);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0384);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x07D0);  //                     
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x1388);  //                     
             S5K4ECGX_write_cmos_sensor(0x002A, 0x0F2A);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                                   
             S5K4ECGX_write_cmos_sensor(0x002A, 0x04D0);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                   
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0000);  //                  
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0001);  //                      
             S5K4ECGX_write_cmos_sensor(0x002A, 0x06C2);
             S5K4ECGX_write_cmos_sensor(0x0F12, 0x0200);  //                  
             break;
    }
    return;
}


void S5K4ECGX_MIPI_set_saturation(UINT16 para)
{

    SENSORDB("[4EC]Set_saturation\n");


    S5K4ECGX_write_cmos_sensor(0xFCFC ,0xD000);
    S5K4ECGX_write_cmos_sensor(0x0028 ,0x7000);
    S5K4ECGX_write_cmos_sensor(0x002A ,0x0234);
    switch (para)
    {
        case ISP_SAT_HIGH:
             S5K4ECGX_write_cmos_sensor(0x0F12 ,0x007F);  //                     
             break;
        case ISP_SAT_LOW:
             S5K4ECGX_write_cmos_sensor(0x0F12 ,0xFF81);  //                     
             break;
        case ISP_SAT_MIDDLE:
        default:
             S5K4ECGX_write_cmos_sensor(0x0F12 ,0x0000);  //                     
             break;
    }
     return;
}



void S5K4ECGX_MIPI_set_scene_mode(UINT16 para)
{
    unsigned int activeConfigNum = 0;
    unsigned int prevSceneMode;
    unsigned int oscar_iSaturation = 0;

    S5K4ECGX_MIPI_GetActiveConfigNum(&activeConfigNum);
    spin_lock(&s5k4ecgx_mipi_drv_lock);
    prevSceneMode = S5K4ECGX_Driver.sceneMode;
    S5K4ECGX_Driver.sceneMode = para;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);


    //                                                             
    //                                                         
    if (((SCENE_MODE_PORTRAIT == prevSceneMode) &&
         (SCENE_MODE_PORTRAIT != para))   ||
        ((SCENE_MODE_LANDSCAPE == prevSceneMode) &&
         (SCENE_MODE_LANDSCAPE != para)))
    {
        //                                  
        S5K4ECGX_write_cmos_sensor(0x002A, 0x0A28);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);  //                                                   
        S5K4ECGX_write_cmos_sensor(0x002A, 0x0ADE);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);  //                                                   
        S5K4ECGX_write_cmos_sensor(0x002A, 0x0B94);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);  //                                                   
        S5K4ECGX_write_cmos_sensor(0x002A, 0x0C4A);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);  //                                                   
        S5K4ECGX_write_cmos_sensor(0x002A, 0x0D00);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x6024);  //                                                   
    }


    //                                           
    if (((SCENE_MODE_NIGHTSCENE == prevSceneMode) &&
         (SCENE_MODE_NIGHTSCENE != para)))
    {
        S5K4ECGX_write_cmos_sensor(0x002A, 0x06B8);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x452C);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0005);  //          
        S5K4ECGX_write_cmos_sensor(0x002A, 0x0A1E);
        S5K4ECGX_write_cmos_sensor(0x0F12, 0x0350);  //                                                     
    }

    //                                                        
    //                                                           
    //                     
    if (((SCENE_MODE_NIGHTSCENE == prevSceneMode) &&
         (SCENE_MODE_NIGHTSCENE != para)) ||
        ((SCENE_MODE_SPORTS == prevSceneMode) &&
         (SCENE_MODE_SPORTS != para)))
    {
        S5K4ECGX_MIPI_AE_ExpCurveGain_Config(para);
    }
    else if ((SCENE_MODE_NIGHTSCENE == para) ||
             (SCENE_MODE_SPORTS == para))
    {
        S5K4ECGX_MIPI_AE_ExpCurveGain_Config(para);
    }


    if (SCENE_MODE_NIGHTSCENE != para)
    {
        S5K4ECGX_MIPI_SetFrameRate(S5K4ECGXCurrentScenarioId, 30);
    }
    else
    {
        //                                               
        //                                                
        //                                                                     
        //                                                          
    }




    switch (para)
    {
        case SCENE_MODE_PORTRAIT:
            SENSORDB("[4EC]SCENE_MODE_PORTRAIT\n");
            //                                                           
            //                                                           
            //                                                           
            //                 
            S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
            S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0A28);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x2020); //                                                   
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0ADE);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x2020); //                                                   
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0B94);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x2020); //                                                   
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0C4A);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x2020); //                                                   
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0D00);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x2020); //                                                   
            break;
        case SCENE_MODE_LANDSCAPE:
            SENSORDB("[4EC]SCENE_MODE_LANDSCAPE\n");
            //                                                           
            //                                                             
            //                                                           
            //                                         
            S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0A28);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0xE082); //                                                   
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0ADE);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0xE082); //                                                   
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0B94);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0xE082); //                                                   
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0C4A);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0xE082); //                                                   
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0D00);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0xE082); //                                                   
            break;
        //                      
        case SCENE_MODE_NIGHTSCENE:
            SENSORDB("[4EC]SCENE_MODE_NIGHTSCENE\n");
            //                                                           
            //                                                        
            //                                                           
            {
                S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
                S5K4ECGX_write_cmos_sensor(0x002C,0x7000);
                S5K4ECGX_write_cmos_sensor(0x002E,0x0A1E);
                oscar_iSaturation = S5K4ECGX_read_cmos_sensor(0x0F12);
                SENSORDB("[4EC]SCENE_MODE_NIGHTSCENE: oscar_iSaturation:%x\n", oscar_iSaturation);
            }

            S5K4ECGX_write_cmos_sensor(0xFCFC, 0xD000);
            S5K4ECGX_write_cmos_sensor(0x0028, 0x7000);
            S5K4ECGX_write_cmos_sensor(0x002A, 0x06B8);
            S5K4ECGX_write_cmos_sensor(0x0F12, 0xFFFF); //          
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x00FF); //           
            S5K4ECGX_write_cmos_sensor(0x002A, 0x0A1E);
            //                                                                    
            //                                                                                                           
            S5K4ECGX_write_cmos_sensor(0x0F12, 0x1550); //                                                             
            break;

        case SCENE_MODE_SUNSET:
            SENSORDB("[4EC]SCENE_MODE_SUNSET\n");
            //                                                           
            //                                                            
            //                                                           
            break;
        case SCENE_MODE_SPORTS:
            SENSORDB("[4EC]SCENE_MODE_SPORTS\n");
            //                                                           
            //                                                         
            //                                                           
            //        
            break;

        case SCENE_MODE_HDR:
            SENSORDB("[4EC]SCENE_MODE_HDR\n");
            spin_lock(&s5k4ecgx_mipi_drv_lock);
            if (1 == S5K4ECGX_Driver.manualAEStart)
            {
                S5K4ECGX_write_cmos_sensor(0x0028,0x7000);
                S5K4ECGX_write_cmos_sensor(0x002A,0x04E6);
                S5K4ECGX_write_cmos_sensor(0x0F12,0x077F); //                 
                S5K4ECGX_Driver.manualAEStart = 0;
                S5K4ECGX_Driver.currentExposureTime = 0;
                S5K4ECGX_Driver.currentAxDGain = 0;
            }
            spin_unlock(&s5k4ecgx_mipi_drv_lock);
            break;

        case SCENE_MODE_OFF:
            SENSORDB("[4EC]SCENE_MODE_OFF\n");
        default:
            SENSORDB("[4EC]SCENE_MODE default: %d\n", para);
            S5K4ECGX_MIPI_SceneOffMode(); //                
            break;
    }
    return;
}




UINT32 S5K4ECGX_MIPI_SensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{
    //                                  
    //            

    switch (iCmd)
    {
        case FID_SCENE_MODE:     //                       
            SENSORDB("[4EC] FID_SCENE_MODE\n");
            S5K4ECGX_MIPI_set_scene_mode(iPara);
            break;
        case FID_AWB_MODE:
            SENSORDB("[4EC]FID_AWB_MODE: para=%d\n", iPara);
            S5K4ECGX_MIPI_set_param_wb(iPara);
            break;
        case FID_COLOR_EFFECT:
            SENSORDB("[4EC]FID_COLOR_EFFECT para=%d\n", iPara);
            S5K4ECGX_MIPI_set_param_effect(iPara);
            break;
        case FID_AE_EV:
            SENSORDB("[4EC]FID_AE_EV para=%d\n", iPara);
            S5K4ECGX_MIPI_set_param_exposure(iPara);
            break;
        case FID_AE_FLICKER:
            SENSORDB("[4EC]FID_AE_FLICKER para=%d\n", iPara);
            S5K4ECGX_MIPI_set_param_banding(iPara);
            break;
        case FID_AE_SCENE_MODE:
            SENSORDB("[4EC]FID_AE_SCENE_MODE para=%d\n", iPara);
           /*                          
             
                                                   
                                                     
                                                     
                                       
             
                  
                                                   
                                                      
                                                     
                                         
             */
            break;
        case FID_ZOOM_FACTOR:
            SENSORDB("[4EC]FID_ZOOM_FACTOR para=%d\n", iPara);
            break;
        case FID_ISP_CONTRAST:
            SENSORDB("[4EC]FID_ISP_CONTRAST:%d\n",iPara);
            S5K4ECGX_MIPI_set_contrast(iPara);
            break;
        case FID_ISP_BRIGHT:
            SENSORDB("[4EC]FID_ISP_BRIGHT:%d\n",iPara);
            S5K4ECGX_MIPI_set_brightness(iPara);
            break;
        case FID_ISP_SAT:
            SENSORDB("[4EC]FID_ISP_SAT:%d\n",iPara);
            S5K4ECGX_MIPI_set_saturation(iPara);
            break;
        case FID_AE_ISO:
            SENSORDB("[4EC]FID_AE_ISO:%d\n",iPara);
            S5K4ECGX_MIPI_set_iso(iPara);
            break;
        default:
            SENSORDB("[4EC]SensorSetting Default, FID=%d\n", iCmd);
            break;
    }
    return TRUE;
}   /*                            */



void S5K4ECGX_MIPI_SetVideoMode(UINT16 u2FrameRate)
{
    SENSORDB("[4EC] SetVideoMode+ :FrameRate= %d\n",u2FrameRate);

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGXCurrentScenarioId = MSDK_SCENARIO_ID_VIDEO_PREVIEW;
    S5K4ECGX_Driver.videoFrameRate = u2FrameRate;
    S5K4ECGX_Driver.Period_PixelNum = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV + S5K4ECGX_Driver.Dummy_Pixels;
    S5K4ECGX_Driver.Period_LineNum = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV + S5K4ECGX_Driver.Dummy_Lines;
    S5K4ECGX_Driver.Preview_Width = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV;
    S5K4ECGX_Driver.Preview_Height = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    S5K4ECGX_MIPI_SetFrameRate(MSDK_SCENARIO_ID_VIDEO_PREVIEW, u2FrameRate);
    return;
}

void S5K4ECGX_MIPI_SetMaxMinFps(UINT32 u2MinFrameRate, UINT32 u2MaxFrameRate)
{
    SENSORDB("[4EC] S5K4ECGX_MIPI_SetMaxMinFps+ :FrameRate= %d %d\n",u2MinFrameRate,u2MaxFrameRate);

    spin_lock(&s5k4ecgx_mipi_drv_lock);
    S5K4ECGXCurrentScenarioId = MSDK_SCENARIO_ID_VIDEO_PREVIEW;
    S5K4ECGX_Driver.videoFrameRate = u2MaxFrameRate;
    S5K4ECGX_Driver.Period_PixelNum = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV + S5K4ECGX_Driver.Dummy_Pixels;
    S5K4ECGX_Driver.Period_LineNum = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV + S5K4ECGX_Driver.Dummy_Lines;
    S5K4ECGX_Driver.Preview_Width = S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV;
    S5K4ECGX_Driver.Preview_Height = S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV;
    spin_unlock(&s5k4ecgx_mipi_drv_lock);

    S5K4ECGX_MIPI_SetFrameRate(MSDK_SCENARIO_ID_VIDEO_PREVIEW, u2MaxFrameRate);
    return;
}

void S5K4ECGX_MIPI_GetExifInfo(uintptr_t exifAddr)
{
    SENSOR_EXIF_INFO_STRUCT* pExifInfo = (SENSOR_EXIF_INFO_STRUCT*)exifAddr;
    pExifInfo->FNumber = 28;
    pExifInfo->AEISOSpeed = S5K4ECGX_Driver.isoSpeed;
    pExifInfo->AWBMode = S5K4ECGX_Driver.awbMode;
    pExifInfo->CapExposureTime = S5K4ECGX_Driver.capExposureTime;
    pExifInfo->FlashLightTimeus = 0;
    pExifInfo->RealISOValue = (S5K4ECGX_MIPI_ReadGain()*57) >> 8;
        //                         
}




void S5K4ECGX_MIPI_GetDelayInfo(uintptr_t delayAddr)
{
    SENSOR_DELAY_INFO_STRUCT* pDelayInfo = (SENSOR_DELAY_INFO_STRUCT*)delayAddr;
    pDelayInfo->InitDelay = 3;
    pDelayInfo->EffectDelay = 0;
    pDelayInfo->AwbDelay = 3;
    pDelayInfo->AFSwitchDelayFrame = 50;
}



UINT32 S5K4ECGX_MIPI_SetMaxFramerateByScenario(
  MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 frameRate)
{
    kal_uint32 pclk;
    kal_int16 dummyLine;
    kal_uint16 lineLength,frameHeight;
#if 0
    SENSORDB("S5K4ECGX_MIPI_SetMaxFramerateByScenario: scenarioId = %d, frame rate = %d\n",scenarioId,frameRate);
    switch (scenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
             pclk = S5K4ECGX_MIPI_sensor_pclk;
             lineLength = S5K3H7Y_PV_PERIOD_PIXEL_NUMS;
             frameHeight = (10 * pclk)/frameRate/lineLength;
             dummyLine = frameHeight - S5K3H7Y_PV_PERIOD_LINE_NUMS;
             s5k3h7y.sensorMode = SENSOR_MODE_PREVIEW;
             S5K3H7Y_SetDummy(0, dummyLine);
             break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
             pclk = S5K4ECGX_MIPI_sensor_pclk;
             lineLength = S5K3H7Y_PV_PERIOD_PIXEL_NUMS;
             frameHeight = (10 * pclk)/frameRate/lineLength;
             dummyLine = frameHeight - S5K3H7Y_PV_PERIOD_LINE_NUMS;
             s5k3h7y.sensorMode = SENSOR_MODE_PREVIEW;
             S5K3H7Y_SetDummy(0, dummyLine);
             break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
             pclk = S5K4ECGX_MIPI_sensor_pclk;
             lineLength = S5K3H7Y_FULL_PERIOD_PIXEL_NUMS;
             frameHeight = (10 * pclk)/frameRate/lineLength;
             dummyLine = frameHeight - S5K3H7Y_FULL_PERIOD_LINE_NUMS;
             s5k3h7y.sensorMode = SENSOR_MODE_CAPTURE;
             S5K3H7Y_SetDummy(0, dummyLine);
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
#endif
  return ERROR_NONE;
}


UINT32 S5K4ECGX_MIPI_GetDefaultFramerateByScenario(
  MSDK_SCENARIO_ID_ENUM scenarioId, MUINT32 *pframeRate)
{
    switch (scenarioId)
    {
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



void S5K4ECGX_MIPI_3ACtrl(ACDK_SENSOR_3A_LOCK_ENUM action)
{
   switch (action)
   {
      case SENSOR_3A_AE_LOCK:
          SENSORDB("[4EC] SENSOR_3A_AE_LOCK\n");
          spin_lock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_Driver.userAskAeLock = TRUE;
          spin_unlock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_MIPI_AE_Lock();
      break;

      case SENSOR_3A_AE_UNLOCK:
          SENSORDB("[4EC] SENSOR_3A_AE_UNLOCK\n");
          spin_lock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_Driver.userAskAeLock = FALSE;
          spin_unlock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_MIPI_AE_UnLock();
      break;

      case SENSOR_3A_AE_ON:
          SENSORDB("[4EC] SENSOR_3A_AE_ON\n");
          spin_lock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_Driver.aeEnable= TRUE;
          spin_unlock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_MIPI_AE_On();
      break;

      case SENSOR_3A_AE_OFF:
          SENSORDB("[4EC] SENSOR_3A_AE_OFF\n");
          spin_lock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_Driver.aeEnable= FALSE;
          spin_unlock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_MIPI_AE_Off();
      break;

      case SENSOR_3A_AWB_LOCK:
          SENSORDB("[4EC] SENSOR_3A_AWB_LOCK\n");
          spin_lock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_Driver.userAskAwbLock = TRUE;
          spin_unlock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_MIPI_AWB_Lock();
      break;

      case SENSOR_3A_AWB_UNLOCK:
          SENSORDB("[4EC] SENSOR_3A_AWB_UNLOCK\n");
          spin_lock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_Driver.userAskAwbLock = FALSE;
          spin_unlock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_MIPI_AWB_UnLock();
      break;

      case SENSOR_3A_AWB_ON:
          SENSORDB("[4EC] SENSOR_3A_AWB_ON\n");
          spin_lock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_Driver.awbEnable= TRUE;
          spin_unlock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_MIPI_AWB_On();
      break;

      case SENSOR_3A_AWB_OFF:
          SENSORDB("[4EC] SENSOR_3A_AWB_OFF\n");
          spin_lock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_Driver.awbEnable= FALSE;
          spin_unlock(&s5k4ecgx_mipi_drv_lock);
          S5K4ECGX_MIPI_AWB_Off();
      break;

      default:
      break;
   }
   return;
}



UINT32 S5K4ECGX_MIPI_FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
               UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
    UINT16 u2Temp = 0;
    UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
    UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
    UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
    UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
    unsigned long long *pFeatureData=(unsigned long long *) pFeaturePara;
    unsigned long long *pFeatureReturnPara=(unsigned long long *) pFeaturePara;
    PNVRAM_SENSOR_DATA_STRUCT pSensorDefaultData=(PNVRAM_SENSOR_DATA_STRUCT) pFeaturePara;
    MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
    MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;
    MSDK_SENSOR_GROUP_INFO_STRUCT *pSensorGroupInfo=(MSDK_SENSOR_GROUP_INFO_STRUCT *) pFeaturePara;
    MSDK_SENSOR_ITEM_INFO_STRUCT *pSensorItemInfo=(MSDK_SENSOR_ITEM_INFO_STRUCT *) pFeaturePara;
    MSDK_SENSOR_ENG_INFO_STRUCT *pSensorEngInfo=(MSDK_SENSOR_ENG_INFO_STRUCT *) pFeaturePara;
    //                                                               

    switch (FeatureId)
    {
        case SENSOR_FEATURE_GET_RESOLUTION:
             SENSORDB("[4EC] F_GET_RESOLUTION\n");
             *pFeatureReturnPara16++ = S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV;
             *pFeatureReturnPara16 = S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV;
             *pFeatureParaLen=4;
             break;

        case SENSOR_FEATURE_GET_PERIOD:
             SENSORDB("[4EC] F_GET_PERIOD\n");
             *pFeatureReturnPara16++ = S5K4ECGX_Driver.Period_PixelNum;
             *pFeatureReturnPara16 = S5K4ECGX_Driver.Period_LineNum;
             *pFeatureParaLen=4;
             break;

        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
             SENSORDB("[4EC] F_PIXEL_CLOCK_FREQ\n");
             *pFeatureReturnPara32 = S5K4ECGX_MIPI_sensor_pclk;
             *pFeatureParaLen=4;
             break;

        case SENSOR_FEATURE_SET_NIGHTMODE:
             SENSORDB("[4EC] F_SET_NIGHTMODE\n");
             //                                              
             //                
             break;



        /*                                                                     */
        case SENSOR_FEATURE_SET_ESHUTTER:
             SENSORDB("[4EC] F_SET_ESHUTTER: Not Support\n");
             S5K4ECGX_MIPI_SetShutter(*pFeatureData);
             break;

        case SENSOR_FEATURE_SET_GAIN:
             SENSORDB("[4EC] F_SET_GAIN: Not Support\n");
             S5K4ECGX_MIPI_SetGain((UINT16)*pFeatureData);
             break;

        case SENSOR_FEATURE_SET_GAIN_AND_ESHUTTER:
            SENSORDB("[4EC] F_SET_GAIN_AND_ESHUTTER\n");
            S5K4ECGX_MIPI_SetGain(*pFeatureData);
            S5K4ECGX_MIPI_SetShutter(*pFeatureData++);
            break;

        case SENSOR_FEATURE_GET_AE_FLASHLIGHT_INFO:
             SENSORDB("[4EC] F_GET_AE_FLASHLIGHT_INFO: Not Support\n");
             S5K4ECGX_MIPI_GetAEFlashlightInfo((uintptr_t)*pFeatureData);
             break;

        case SENSOR_FEATURE_GET_TRIGGER_FLASHLIGHT_INFO:
             S5K4ECGX_MIPI_FlashTriggerCheck(pFeatureReturnPara32);
             SENSORDB("[4EC] F_GET_TRIGGER_FLASHLIGHT_INFO: %d\n", *pFeatureReturnPara32);
             break;

        case SENSOR_FEATURE_SET_FLASHLIGHT:
             SENSORDB("S5K4ECGX SENSOR_FEATURE_SET_FLASHLIGHT\n");
             break;
        /*                                                                   */


        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
             SENSORDB("[4EC] F_SET_ISP_MASTER_CLOCK_FREQ\n");
             //                                   
             //                                                 
             //                                     
             break;

        case SENSOR_FEATURE_SET_REGISTER:
             SENSORDB("[4EC] F_SET_REGISTER\n");
             S5K4ECGX_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
             break;

        case SENSOR_FEATURE_GET_REGISTER:
             SENSORDB("[4EC] F_GET_REGISTER\n");
             pSensorRegData->RegData = S5K4ECGX_read_cmos_sensor(pSensorRegData->RegAddr);
             break;

        case SENSOR_FEATURE_GET_CONFIG_PARA:
             SENSORDB("[4EC] F_ET_CONFIG_PARA\n");
             memcpy(pSensorConfigData, &S5K4ECGXSensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
             *pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
             break;

        case SENSOR_FEATURE_SET_CCT_REGISTER: //          
        case SENSOR_FEATURE_GET_CCT_REGISTER: //          
        case SENSOR_FEATURE_SET_ENG_REGISTER: //          
        case SENSOR_FEATURE_GET_ENG_REGISTER: //          
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT: //          
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:  //          
        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA: //          
        case SENSOR_FEATURE_GET_GROUP_INFO:
        case SENSOR_FEATURE_GET_ITEM_INFO:
        case SENSOR_FEATURE_SET_ITEM_INFO:
        case SENSOR_FEATURE_GET_ENG_INFO:
             //                                   
             break;

        case SENSOR_FEATURE_GET_GROUP_COUNT:
             *pFeatureReturnPara32++=0;
             *pFeatureParaLen=4;
             //                                      
             break;

        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
             //                                         
             //                                                                             
             //                                           
             *pFeatureReturnPara32= LENS_DRIVER_ID_DO_NOT_CARE;
             *pFeatureParaLen=4;
             break;

        case SENSOR_FEATURE_SET_YUV_CMD:
             //                                                         
             S5K4ECGX_MIPI_SensorSetting((FEATURE_ID)*pFeatureData, *(pFeatureData+1));
             break;

        case SENSOR_FEATURE_SET_YUV_3A_CMD:
             SENSORDB("[4EC] SENSOR_FEATURE_SET_YUV_3A_CMD ID:%d\n", *pFeatureData);
             S5K4ECGX_MIPI_3ACtrl((ACDK_SENSOR_3A_LOCK_ENUM)*pFeatureData);
             break;

        case SENSOR_FEATURE_SET_VIDEO_MODE:
             SENSORDB("[4EC] F_SET_VIDEO_MODE\n");
             S5K4ECGX_MIPI_SetVideoMode(*pFeatureData);
             break;

        case SENSOR_FEATURE_CHECK_SENSOR_ID:
             SENSORDB("[4EC] F_CHECK_SENSOR_ID\n");
             S5K4ECGX_MIPI_GetSensorID(pFeatureReturnPara32);
             break;

        case SENSOR_FEATURE_GET_EV_AWB_REF:
             SENSORDB("[4EC] F_GET_EV_AWB_REF\n");
             S5K4ECGX_MIPI_GetEvAwbRef((uintptr_t)*pFeatureData);
             break;

        case SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN:
             SENSORDB("[4EC] F_GET_SHUTTER_GAIN_AWB_GAIN\n");
             S5K4ECGX_MIPI_GetCurAeAwbInfo((uintptr_t)*pFeatureData);
             break;

  #ifdef MIPI_INTERFACE
        case SENSOR_FEATURE_GET_EXIF_INFO:
             //                                    
             S5K4ECGX_MIPI_GetExifInfo((uintptr_t)*pFeatureData);
             break;
  #endif

        case SENSOR_FEATURE_GET_DELAY_INFO:
             SENSORDB("[4EC] F_GET_DELAY_INFO\n");
             S5K4ECGX_MIPI_GetDelayInfo((uintptr_t)*pFeatureData);
             break;

        case SENSOR_FEATURE_SET_SLAVE_I2C_ID:
             //                                                            
             S5K4ECGX_MIPI_sensor_socket = *pFeatureData;
             break;

        case SENSOR_FEATURE_SET_TEST_PATTERN:
             SENSORDB("[4EC] F_SET_TEST_PATTERN\n");
             S5K4ECGX_MIPI_SetTestPatternMode((BOOL)*pFeatureData);
             break;

        case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
             SENSORDB("[4EC] F_SET_MAX_FRAME_RATE_BY_SCENARIO: FAIL: NOT Support\n");
             S5K4ECGX_MIPI_SetMaxFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData, *(pFeatureData+1));
             break;

        case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
             SENSORDB("[4EC] F_GET_DEFAULT_FRAME_RATE_BY_SCENARIO\n");
             S5K4ECGX_MIPI_GetDefaultFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData, (MUINT32 *)(uintptr_t)(*(pFeatureData+1)));
             break;

        //                   
        case SENSOR_FEATURE_INITIALIZE_AF:
             SENSORDB("[4EC] F_INITIALIZE_AF\n");
             S5K4ECGX_MIPI_AF_Init();
             break;

        case SENSOR_FEATURE_MOVE_FOCUS_LENS:
             SENSORDB("[4EC] F_MOVE_FOCUS_LENS\n");
             S5K4ECGX_MIPI_AF_Move_to(*pFeatureData16); //                   
             break;

        case SENSOR_FEATURE_GET_AF_STATUS:
            S5K4ECGX_MIPI_AF_Get_Status(pFeatureReturnPara32);
            //                                                              
            *pFeatureParaLen = 4;
             break;

        case SENSOR_FEATURE_SINGLE_FOCUS_MODE:
             SENSORDB("[4EC] F_SINGLE_FOCUS_MODE\n");
             S5K4ECGX_MIPI_AF_Start(S5K4ECGX_AF_MODE_SINGLE);
             break;

        case SENSOR_FEATURE_CONSTANT_AF:
             SENSORDB("[4EC] F_CONSTANT_AF\n");
             S5K4ECGX_MIPI_AF_Start(S5K4ECGX_AF_MODE_CONTINUOUS);
             break;

        case SENSOR_FEATURE_CANCEL_AF:
             SENSORDB("[4EC] F_CANCEL_AF\n");
             S5K4ECGX_MIPI_AF_CancelFocus();
             break;

        case SENSOR_FEATURE_INFINITY_AF:
            SENSORDB("[4EC] F_INFINITY_AF\n");
            S5K4ECGX_MIPI_AF_Infinity();
            break;

        case SENSOR_FEATURE_GET_AE_STATUS:
            SENSORDB("[4EC] F_GET_AE_STATUS\n");
            S5K4ECGX_MIPI_AE_Get_Status(pFeatureReturnPara32);
            *pFeatureParaLen=4;
            break;

        case SENSOR_FEATURE_GET_AWB_STATUS:
            SENSORDB("[4EC] F_GET_AWB_STATUS\n");
            S5K4ECGX_MIPI_AWB_Get_Status(pFeatureReturnPara32);
            *pFeatureParaLen=4;
            break;

        case SENSOR_FEATURE_GET_AF_INF:
             SENSORDB("[4EC] F_GET_AF_INF\n");
             S5K4ECGX_MIPI_AF_Get_Inf(pFeatureReturnPara32);
             *pFeatureParaLen=4;
             break;

        case SENSOR_FEATURE_GET_AF_MACRO:
             S5K4ECGX_MIPI_AF_Get_Macro(pFeatureReturnPara32);
             SENSORDB("[4EC] F_GET_AF_MACRO=%d\n", *pFeatureReturnPara32);
             *pFeatureParaLen=4;
             break;

        case SENSOR_FEATURE_GET_AF_MAX_NUM_FOCUS_AREAS:
             S5K4ECGX_MIPI_AF_Get_Max_Num_Focus_Areas(pFeatureReturnPara32);
             SENSORDB("[4EC] F_GET_AF_MAX_NUM_FOCUS_AREAS=%d\n", *pFeatureReturnPara32);
             *pFeatureParaLen=4;
             break;

        case SENSOR_FEATURE_GET_AE_MAX_NUM_METERING_AREAS:
             S5K4ECGX_MIPI_AE_Get_Max_Num_Metering_Areas(pFeatureReturnPara32);
             SENSORDB("[4EC] F_GET_AE_MAX_NUM_METERING_AREAS=%d\n", *pFeatureReturnPara32);
             *pFeatureParaLen=4;
             break;
        case SENSOR_FEATURE_SET_AF_WINDOW:
             SENSORDB("[4EC] F_SET_AF_WINDOW\n");
             S5K4ECGX_MIPI_AF_Set_Window((uintptr_t)*pFeatureData, S5K4ECGX_Driver.Preview_Width, S5K4ECGX_Driver.Preview_Height);
             break;

        case SENSOR_FEATURE_SET_AE_WINDOW:
             SENSORDB("[4EC] F_SET_AE_WINDOW\n");
             S5K4ECGX_MIPI_AE_Set_Window((uintptr_t)*pFeatureData, S5K4ECGX_Driver.Preview_Width, S5K4ECGX_Driver.Preview_Height);
             break;

        case SENSOR_FEATURE_GET_AE_AWB_LOCK_INFO:
             SENSORDB("[4EC] F_GET_AE_AWB_LOCK_INFO\n");
             S5K4ECGX_MIPI_get_AEAWB_lock((uintptr_t)(*pFeatureData),(uintptr_t)(*(pFeatureData+1)));
             break;
         case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE://                             
             *pFeatureReturnPara32= S5K4ECGX_TEST_PATTERN_CHECKSUM;
             *pFeatureParaLen=4;
             break;

#if defined(__CAPTURE_JPEG_OUTPUT__)
        /*                                                
                                          
                                
                    
        */
        case SENSOR_FEATURE_GET_YUV_JPEG_INFO:
            SENSORDB("[4EC]GET_YUV_JPEG_INFO");
            {
               UINT32*                para =  (UINT32*)pFeaturePara;
               UINT8                 *jpegFileAddr = para[0];
               //                                             
               ACDK_SENSOR_JPEG_INFO *jpegInfo = (ACDK_SENSOR_JPEG_INFO*) para[1];

               UINT32                 maxBufSize =  2560 * 1920;

               SENSORDB("[4EC]GET_YUV_JPEG_INFO: jpegFileAddr=0x%p, maxBufSize=%d, infoAddr=0x%p\n", jpegFileAddr, maxBufSize, jpegInfo);
               S5K4ECGX_MIPI_JPEG_Capture_Parser(jpegFileAddr, maxBufSize, &S5K4ECGX_Driver.jpegSensorInfo);
               memcpy(jpegInfo, &S5K4ECGX_Driver.jpegSensorInfo, sizeof(ACDK_SENSOR_JPEG_INFO));
               *pFeatureParaLen = sizeof(ACDK_SENSOR_JPEG_INFO);
            }
             break;

        case SENSOR_FEATURE_SET_YUV_JPEG_PARA:
            SENSORDB("[4EC]SET_JPEG_PARA \n");
            {
                UINT32* para =  (UINT32*)pFeaturePara;
                ACDK_SENSOR_JPEG_OUTPUT_PARA *jpegPara = (ACDK_SENSOR_JPEG_OUTPUT_PARA*) para[0];
                SENSORDB("[4EC]width=%d, hegith=%d, Quality=%d\n", jpegPara->tgtWidth, jpegPara->tgtHeight,jpegPara->quality);
                if ((jpegPara->tgtHeight == 0) || (jpegPara->tgtWidth == 0) || (jpegPara->quality == 0) || (jpegPara->quality > 100))
                {
                    SENSORDB("[4EC]SET_JPEG_PARA: Invalid Para!\n");
                    return ERROR_INVALID_PARA;
                }
                memcpy(&S5K4ECGX_Driver.jpegSensorPara, jpegPara, sizeof(ACDK_SENSOR_JPEG_OUTPUT_PARA));
                SENSORDB("[4EC]SET_JPEG_PARA S5K4ECGX_Driver.jpegSensorPara width=%d, hegith=%d, Quality=%d\n",
                       S5K4ECGX_Driver.jpegSensorPara.tgtWidth, S5K4ECGX_Driver.jpegSensorPara.tgtHeight,S5K4ECGX_Driver.jpegSensorPara.quality);
            }
             break;
#endif
        case SENSOR_FEATURE_SET_MIN_MAX_FPS:
             SENSORDB("SENSOR_FEATURE_SET_MIN_MAX_FPS:[%d,%d]\n",*pFeatureData,*(pFeatureData+1));
             S5K4ECGX_MIPI_SetMaxMinFps((UINT32)*pFeatureData, (UINT32)*(pFeatureData+1));

             break;

        default:
             SENSORDB("[4EC]FeatureControl default\n");
             break;
    }

    //                                              
    return ERROR_NONE;
} /*                          */


SENSOR_FUNCTION_STRUCT  SensorFuncS5K4ECGX_MIPI=
{
    S5K4ECGX_MIPI_Open,             //                                             
    S5K4ECGX_MIPI_GetInfo,          //                      
    S5K4ECGX_MIPI_GetResolution,    //                                     
    S5K4ECGX_MIPI_FeatureControl,   //                                    
    S5K4ECGX_MIPI_Control,          //                                     
    S5K4ECGX_MIPI_Close             //                            
};

UINT32 S5K4ECGX_MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
  /*                                  */
  if (pfFunc!=NULL)
     *pfFunc=&SensorFuncS5K4ECGX_MIPI;

  return ERROR_NONE;
} /*              */

