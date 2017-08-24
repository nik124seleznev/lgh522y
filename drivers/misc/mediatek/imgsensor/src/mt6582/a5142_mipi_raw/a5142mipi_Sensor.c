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
#include <linux/slab.h>


#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"

#include "a5142mipi_Sensor.h"
#include "a5142mipi_Camera_Sensor_para.h"
#include "a5142mipi_CameraCustomized.h"

#define A5142MIPI_DEBUG
#ifdef A5142MIPI_DEBUG
#define SENSORDB(fmt, arg...) printk("%s: " fmt "\n", __FUNCTION__ ,##arg)
#else
#define SENSORDB(x,...)
#endif

static DEFINE_SPINLOCK(a5142mipiraw_drv_lock);

#define AUTO_FLICKER_NO 10
kal_uint16 A5142_Frame_Length_preview = 0;
kal_bool A5142DuringTestPattern = KAL_FALSE;
#define A5142MIPI_TEST_PATTERN_CHECKSUM (0xc9e24698)
#define OTP_SUPPORT


#if defined(OTP_SUPPORT)
#include "a5142_otp.h"
#endif

struct A5142MIPI_SENSOR_STRUCT A5142MIPI_sensor= 
{
    .i2c_write_id = 0x6c,
    .i2c_read_id  = 0x6d,
#ifdef MIPI_INTERFACE
    .preview_vt_clk = 1040,
    .capture_vt_clk = 1118,
#else
    .preview_vt_clk = 520,
    .capture_vt_clk = 520,
#endif
};

kal_uint16 A5142MIPI_dummy_pixels=0, A5142MIPI_dummy_lines=0;
kal_uint16 A5142MIPI_PV_dummy_pixels=0, A5142MIPI_PV_dummy_lines=0;

kal_uint16 A5142MIPI_exposure_lines = 0x100;
kal_uint16 A5142MIPI_sensor_global_gain = BASEGAIN, A5142MIPI_sensor_gain_base = BASEGAIN;
kal_uint16 A5142MIPI_sensor_gain_array[2][5] = {{0x0204,0x0208, 0x0206, 0x020C, 0x020A},{0x08,0x8, 0x8, 0x8, 0x8}};


MSDK_SENSOR_CONFIG_STRUCT A5142MIPISensorConfigData;
kal_uint32 A5142MIPI_FAC_SENSOR_REG;

/*                                         */
SENSOR_REG_STRUCT A5142MIPISensorCCT[FACTORY_END_ADDR]=CAMERA_SENSOR_CCT_DEFAULT_VALUE;
SENSOR_REG_STRUCT A5142MIPISensorReg[ENGINEER_END]=CAMERA_SENSOR_REG_DEFAULT_VALUE;
/*                                         */
MSDK_SCENARIO_ID_ENUM A5142_CurrentScenarioId = MSDK_SCENARIO_ID_CAMERA_PREVIEW;

typedef enum
{
    A5142MIPI_MODE_INIT,
    A5142MIPI_MODE_PREVIEW,
    A5142MIPI_MODE_CAPTURE
} A5142MIPI_MODE;

A5142MIPI_MODE g_iA5142MIPI_Mode = A5142MIPI_MODE_PREVIEW;


static void A5142MIPI_SetDummy(kal_bool mode,const kal_uint16 iDummyPixels, const kal_uint16 iDummyLines);


extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);

kal_uint16 A5142MIPI_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte=0;
    char puSendCmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
    iReadRegI2C(puSendCmd , 2, (u8*)&get_byte, 2, A5142MIPI_sensor.i2c_write_id);
    return ((get_byte<<8)&0xff00)|((get_byte>>8)&0x00ff);
}


void A5142MIPI_write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
    char puSendCmd[4] = {(char)(addr >> 8) , (char)(addr & 0xFF) ,(char)(para >> 8),(char)(para & 0xFF)};
    iWriteRegI2C(puSendCmd , 4,A5142MIPI_sensor.i2c_write_id);
}


kal_uint16 A5142MIPI_read_cmos_sensor_8(kal_uint32 addr)
{
    kal_uint16 get_byte=0;
    char puSendCmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
    iReadRegI2C(puSendCmd , 2, (u8*)&get_byte,1,A5142MIPI_sensor.i2c_write_id);
    return get_byte;
}

void A5142MIPI_write_cmos_sensor_8(kal_uint32 addr, kal_uint32 para)
{
    char puSendCmd[4] = {(char)(addr >> 8) , (char)(addr & 0xFF) ,(char)(para & 0xFF)};
    iWriteRegI2C(puSendCmd , 3,A5142MIPI_sensor.i2c_write_id);
}


/*                                                                              
  
                                                                               */
/*
                                                    
 
                    
                                                     
                                                            
                                                           

                                                                                   
                                                                          
                                   

                            
                               
                                   
                               
                                   
                               
                                   
                               
     

                         
                            
                                
                                 
                                
                            
            
                    
                                       
     

                                     
                                                                           
            
                                                                                     
     

                
 
*/

/*                                                                              
  
                                                                               */
/*
                                                
 
                        
                                                                 
                                                            

                                    
                                                         
                                                   
                                                     
                                     
                                                         
                                                   
                                                               
                                             
                                                         
                                                   
                                                           
                                     
                                                         
                                                   
                                                                   
                                     
                                                         
                                                   
                                                           
                                                
                                                         
                                                   
                                                           
                                     
                                                         
                                                   
                                                                   
                                      
                                                         
                                                   
                                                               
            
                    
                                       
     

                                                       

                    
 
*/
/*                                                                        
          
                        
 
             
                                                   
 
            
         
 
         
                                          
 
                  
 
                                                                        */
kal_uint16 read_A5142MIPI_gain(void)
{
    /*
                        
                                  
         
                                                  
                                                                 
    
                                                                                  

                
    */
    
    volatile signed char i;
    kal_uint16 temp_reg=0, sensor_gain=0,temp_reg_base=0;
    
    temp_reg_base=A5142MIPISensorCCT[SENSOR_BASEGAIN].Para;

    for(i=0;i<4;i++)
    {
        temp_reg=A5142MIPISensorCCT[PRE_GAIN_R_INDEX+i].Para;

        if(temp_reg>=0x08 && temp_reg<=0x78)  //                                               
            A5142MIPI_sensor_gain_array[1][PRE_GAIN_R_INDEX+i]=((((temp_reg*BASEGAIN)/8)*temp_reg_base)/8); //                      
        else if(temp_reg>0x78)
            SENSORDB("error gain setting");
    }

    sensor_gain=(temp_reg_base*BASEGAIN)/8;

    return sensor_gain;   //             
    
}

/*                                                                              
  
                                                                               */
void write_A5142MIPI_gain(kal_uint16 gain)
{
    kal_uint16 reg_gain;
  
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x01);        //              
    if(gain >= BASEGAIN && gain <= 32*BASEGAIN)
    {   
        reg_gain = 8 * gain/BASEGAIN;        //                                        
        A5142MIPI_write_cmos_sensor(0x0204,reg_gain);
        
        SENSORDB("reg_gain =%d, gain = %d", reg_gain, gain);
    }
    else
        SENSORDB("error gain setting");
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x00);        //              
    

    /*
                        
                             
    
                                                                        
    
                                               
     
                                                                                                 

                                                 
                                        
                                                 

                                                   
     
        
                                       

                                                                        
    */
}


/*                                                                        
          
                    
 
             
                                                
 
            
                                       
 
         
                                  
 
                  
 
                                                                        */
void A5142MIPI_Set_gain(kal_uint16 gain)
{
    write_A5142MIPI_gain(gain);
}


/*                                                                        
          
                        
 
             
                                                                     
 
            
                             
 
         
        
 
                  
 
                                                                        */
void A5142MIPI_SetShutter(kal_uint16 iShutter)
{
    unsigned long flags;

    SENSORDB("iShutter: %u", iShutter);

    spin_lock_irqsave(&a5142mipiraw_drv_lock, flags);
    //                                         
    //                                                          
    //           
    // 
    A5142MIPI_exposure_lines=iShutter;
    spin_unlock_irqrestore(&a5142mipiraw_drv_lock, flags);
    
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x01);    //                       
    A5142MIPI_write_cmos_sensor(0x0202, iShutter);  /*                         */
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x00);    //                       
}


/*                                                                        
          
                          
 
             
                                        
 
            
        
 
         
                             
 
                  
 
                                                                        */
UINT16 A5142MIPI_read_shutter(void)
{
    kal_uint16 ishutter;
    
    ishutter = A5142MIPI_read_cmos_sensor(0x0202); /*                         */
    
    return ishutter;
}



UINT32 A5142MIPISetMaxFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId,UINT32 frameRate)
{
    kal_uint32 pclk;
    kal_int16 dummyLine;
    kal_uint16 lineLength,frameHeight;

    printk("A5142SetMaxFramerate: scenarioID = %d, frame rate = %d\n",scenarioId,frameRate);
    switch(scenarioId) {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
            pclk = 104000000;  
            lineLength = A5142MIPI_PV_PERIOD_PIXEL_NUMS;  //    
            frameHeight = (10*pclk)/frameRate/lineLength;
            dummyLine = frameHeight - A5142MIPI_PV_PERIOD_PIXEL_NUMS;
            A5142MIPI_SetDummy(0, 1855,dummyLine);
            break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
            pclk = 104000000;  
            lineLength = A5142MIPI_PV_PERIOD_PIXEL_NUMS;  //     
            frameHeight = (10*pclk)/frameRate/lineLength;
            dummyLine = frameHeight - A5142MIPI_PV_PERIOD_PIXEL_NUMS;
            A5142MIPI_SetDummy(0, 1855,dummyLine);
            break;  
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
            pclk = 111800000; 
            lineLength = A5142MIPI_FULL_PERIOD_PIXEL_NUMS;  //    
            frameHeight = (10*pclk)/frameRate/lineLength;
            dummyLine = frameHeight - A5142MIPI_FULL_PERIOD_PIXEL_NUMS;
            A5142MIPI_SetDummy(0, 1102,dummyLine);
            break;  
        case MSDK_SCENARIO_ID_CAMERA_3D_PREVIEW:
            break;
        case MSDK_SCENARIO_ID_CAMERA_3D_VIDEO:
            break;
        case MSDK_SCENARIO_ID_CAMERA_3D_CAPTURE:
            break;  
            
        default:
            break;
            
        }
    return ERROR_NONE;
    
}

UINT32 A5142MIPIGetDefaultFramerateByScenario(MSDK_SCENARIO_ID_ENUM scenarioId,UINT32 *pframeRate)
{
    switch(scenarioId) {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
            *pframeRate = 300;
            break;  
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:   
            *pframeRate = 150;
            break;  
        case MSDK_SCENARIO_ID_CAMERA_3D_PREVIEW:
        case MSDK_SCENARIO_ID_CAMERA_3D_VIDEO:
        case MSDK_SCENARIO_ID_CAMERA_3D_CAPTURE:
            *pframeRate = 300;
            break;  
            
        default:
            break;
            
        }
    return ERROR_NONE;
}


/*                                                                              
  
                                                                               */
void A5142MIPI_camera_para_to_sensor(void)
{
    kal_uint32    i;

    
    for(i=0; 0xFFFFFFFF!=A5142MIPISensorReg[i].Addr; i++)
    {
        A5142MIPI_write_cmos_sensor(A5142MIPISensorReg[i].Addr, A5142MIPISensorReg[i].Para);
    }
    for(i=ENGINEER_START_ADDR; 0xFFFFFFFF!=A5142MIPISensorReg[i].Addr; i++)
    {
        A5142MIPI_write_cmos_sensor(A5142MIPISensorReg[i].Addr, A5142MIPISensorReg[i].Para);
    }
    for(i=FACTORY_START_ADDR; i<FACTORY_END_ADDR; i++)
    {
        A5142MIPI_write_cmos_sensor(A5142MIPISensorCCT[i].Addr, A5142MIPISensorCCT[i].Para);
    }
}


/*                                                                        
          
                                    
 
             
                                               
 
            
         
 
         
                                          
 
                  
 
                                                                        */
void A5142MIPI_sensor_to_camera_para(void)
{
    kal_uint32    i;
    
    for(i=0; 0xFFFFFFFF!=A5142MIPISensorReg[i].Addr; i++)
    {
        spin_lock(&a5142mipiraw_drv_lock);
        A5142MIPISensorReg[i].Para = A5142MIPI_read_cmos_sensor(A5142MIPISensorReg[i].Addr);
        spin_unlock(&a5142mipiraw_drv_lock);        
    }
    for(i=ENGINEER_START_ADDR; 0xFFFFFFFF!=A5142MIPISensorReg[i].Addr; i++)
    {   
        spin_lock(&a5142mipiraw_drv_lock);
        A5142MIPISensorReg[i].Para = A5142MIPI_read_cmos_sensor(A5142MIPISensorReg[i].Addr);
        spin_unlock(&a5142mipiraw_drv_lock);
    }
}


/*                                                                        
          
                                     
 
             
       
 
            
         
 
         
                                          
 
                  
 
                                                                        */
kal_int32  A5142MIPI_get_sensor_group_count(void)
{
    return GROUP_TOTAL_NUMS;
}


void A5142MIPI_get_sensor_group_info(kal_uint16 group_idx, kal_int8* group_name_ptr, kal_int32* item_count_ptr)
{
    switch (group_idx)
    {
        case PRE_GAIN:
            sprintf((char *)group_name_ptr, "CCT");
            *item_count_ptr = 5;
            break;
        case CMMCLK_CURRENT:
            sprintf((char *)group_name_ptr, "CMMCLK Current");
            *item_count_ptr = 1;
            break;
        case FRAME_RATE_LIMITATION:
            sprintf((char *)group_name_ptr, "Frame Rate Limitation");
            *item_count_ptr = 2;
            break;
        case REGISTER_EDITOR:
            sprintf((char *)group_name_ptr, "Register Editor");
            *item_count_ptr = 2;
            break;
        default:
            ASSERT(0);
    }
}


void A5142MIPI_get_sensor_item_info(kal_uint16 group_idx,kal_uint16 item_idx, MSDK_SENSOR_ITEM_INFO_STRUCT* info_ptr)
{
    kal_int16 temp_reg=0;
    kal_uint16 temp_gain=0, temp_addr=0, temp_para=0;
    
    switch (group_idx)
    {
        case PRE_GAIN:
           switch (item_idx)
          {
              case 0:
                sprintf((char *)info_ptr->ItemNamePtr,"Pregain-R");
                  temp_addr = PRE_GAIN_R_INDEX;
              break;
              case 1:
                sprintf((char *)info_ptr->ItemNamePtr,"Pregain-Gr");
                  temp_addr = PRE_GAIN_Gr_INDEX;
              break;
              case 2:
                sprintf((char *)info_ptr->ItemNamePtr,"Pregain-Gb");
                  temp_addr = PRE_GAIN_Gb_INDEX;
              break;
              case 3:
                sprintf((char *)info_ptr->ItemNamePtr,"Pregain-B");
                  temp_addr = PRE_GAIN_B_INDEX;
              break;
              case 4:
                 sprintf((char *)info_ptr->ItemNamePtr,"SENSOR_BASEGAIN");
                 temp_addr = SENSOR_BASEGAIN;
              break;
              default:
                 ASSERT(0);
          }

            temp_para=A5142MIPISensorCCT[temp_addr].Para;

           if(temp_para>=0x08 && temp_para<=0x78)
                temp_gain=(temp_para*BASEGAIN)/8;
            else
                ASSERT(0);

            temp_gain=(temp_gain*1000)/BASEGAIN;

            info_ptr->ItemValue=temp_gain;
            info_ptr->IsTrueFalse=KAL_FALSE;
            info_ptr->IsReadOnly=KAL_FALSE;
            info_ptr->IsNeedRestart=KAL_FALSE;
            info_ptr->Min=1000;
            info_ptr->Max=15000;
            break;
        case CMMCLK_CURRENT:
            switch (item_idx)
            {
                case 0:
                    sprintf((char *)info_ptr->ItemNamePtr,"Drv Cur[2,4,6,8]mA");
                
                    //                                                       
                    temp_reg = ISP_DRIVING_2MA;
                    if(temp_reg==ISP_DRIVING_2MA)
                    {
                        info_ptr->ItemValue=2;
                    }
                    else if(temp_reg==ISP_DRIVING_4MA)
                    {
                        info_ptr->ItemValue=4;
                    }
                    else if(temp_reg==ISP_DRIVING_6MA)
                    {
                        info_ptr->ItemValue=6;
                    }
                    else if(temp_reg==ISP_DRIVING_8MA)
                    {
                        info_ptr->ItemValue=8;
                    }
                
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_FALSE;
                    info_ptr->IsNeedRestart=KAL_TRUE;
                    info_ptr->Min=2;
                    info_ptr->Max=8;
                    break;
                default:
                    ASSERT(0);
            }
            break;
        case FRAME_RATE_LIMITATION:
            switch (item_idx)
            {
                case 0:
                    sprintf((char *)info_ptr->ItemNamePtr,"Max Exposure Lines");
                    info_ptr->ItemValue=    111;  //                             
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_TRUE;
                    info_ptr->IsNeedRestart=KAL_FALSE;
                    info_ptr->Min=0;
                    info_ptr->Max=0;
                    break;
                case 1:
                    sprintf((char *)info_ptr->ItemNamePtr,"Min Frame Rate");
                    info_ptr->ItemValue=12;
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_TRUE;
                    info_ptr->IsNeedRestart=KAL_FALSE;
                    info_ptr->Min=0;
                    info_ptr->Max=0;
                    break;
                default:
                    ASSERT(0);
            }
            break;
        case REGISTER_EDITOR:
            switch (item_idx)
            {
                case 0:
                    sprintf((char *)info_ptr->ItemNamePtr,"REG Addr.");
                    info_ptr->ItemValue=0;
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_FALSE;
                    info_ptr->IsNeedRestart=KAL_FALSE;
                    info_ptr->Min=0;
                    info_ptr->Max=0xFFFF;
                    break;
                case 1:
                    sprintf((char *)info_ptr->ItemNamePtr,"REG Value");
                    info_ptr->ItemValue=0;
                    info_ptr->IsTrueFalse=KAL_FALSE;
                    info_ptr->IsReadOnly=KAL_FALSE;
                    info_ptr->IsNeedRestart=KAL_FALSE;
                    info_ptr->Min=0;
                    info_ptr->Max=0xFFFF;
                    break;
                default:
                ASSERT(0);
            }
            break;
        default:
            ASSERT(0);
    }
}


kal_bool A5142MIPI_set_sensor_item_info(kal_uint16 group_idx, kal_uint16 item_idx, kal_int32 ItemValue)
{
//                      
   kal_uint16  temp_gain=0,temp_addr=0, temp_para=0;

   switch (group_idx)
    {
        case PRE_GAIN:
            switch (item_idx)
            {
              case 0:
                temp_addr = PRE_GAIN_R_INDEX;
              break;
              case 1:
                temp_addr = PRE_GAIN_Gr_INDEX;
              break;
              case 2:
                temp_addr = PRE_GAIN_Gb_INDEX;
              break;
              case 3:
                temp_addr = PRE_GAIN_B_INDEX;
              break;
              case 4:
                temp_addr = SENSOR_BASEGAIN;
              break;
              default:
                 ASSERT(0);
          }

            temp_gain=((ItemValue*BASEGAIN+500)/1000);          //                             

          if(temp_gain>=1*BASEGAIN && temp_gain<=15*BASEGAIN)
          {
             temp_para=(temp_gain*8+BASEGAIN/2)/BASEGAIN;
          }          
          else
              ASSERT(0);
            spin_lock(&a5142mipiraw_drv_lock);
            A5142MIPISensorCCT[temp_addr].Para = temp_para;
            spin_unlock(&a5142mipiraw_drv_lock);

            A5142MIPI_write_cmos_sensor(A5142MIPISensorCCT[temp_addr].Addr,temp_para);

            spin_lock(&a5142mipiraw_drv_lock);
            A5142MIPI_sensor_gain_base=read_A5142MIPI_gain();
            spin_unlock(&a5142mipiraw_drv_lock);

            break;
        case CMMCLK_CURRENT:
            switch (item_idx)
            {
                case 0:
                    //                                              
                    break;
                default:
                    ASSERT(0);
            }
            break;
        case FRAME_RATE_LIMITATION:
            ASSERT(0);
            break;
        case REGISTER_EDITOR:
            switch (item_idx)
            {
                case 0:
                    spin_lock(&a5142mipiraw_drv_lock);
                    A5142MIPI_FAC_SENSOR_REG=ItemValue;
                    spin_unlock(&a5142mipiraw_drv_lock);
                    break;
                case 1:
                    A5142MIPI_write_cmos_sensor(A5142MIPI_FAC_SENSOR_REG,ItemValue);
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


/*                                                                              
 
                                                                               */
static void A5142MIPI_Init_setting(void)
{
    //                      
    
    SENSORDB( "Enter!");
    
    A5142MIPI_write_cmos_sensor_8(0x0103, 0x01);    //                              
    mDELAY(5);      //                   
    
    //                                                         
    //              
    A5142MIPI_write_cmos_sensor_8(0x0100, 0x00);    //            
     
    #ifdef MIPI_INTERFACE
        #ifdef RAW10
            A5142MIPI_write_cmos_sensor(0x301A, 0x0218);    //                                                           
            A5142MIPI_write_cmos_sensor(0x3064, 0xB800);    //          
            A5142MIPI_write_cmos_sensor(0x31AE, 0x0202);    //         
            A5142MIPI_write_cmos_sensor(0x0112, 0x0A0A);    //                 
        #else
            A5142MIPI_write_cmos_sensor(0x301A, 0x0218);    //                                                           
            A5142MIPI_write_cmos_sensor(0x3064, 0x0805);    //          
            A5142MIPI_write_cmos_sensor(0x31AE, 0x0202);    //         
            A5142MIPI_write_cmos_sensor(0x0112, 0x0808);    //                
        #endif
    #else
        A5142MIPI_write_cmos_sensor(0x301A, 0x12C8);        //                                                    
        A5142MIPI_write_cmos_sensor(0x3064, 0x5840);        //          
        A5142MIPI_write_cmos_sensor(0x31AE, 0x0101);        //              
    #endif
    
    //                         
    A5142MIPI_write_cmos_sensor(0x316A, 0x8400);    //          
    A5142MIPI_write_cmos_sensor(0x316C, 0x8400);    //         
    A5142MIPI_write_cmos_sensor(0x316E, 0x8400);    //        
    A5142MIPI_write_cmos_sensor(0x3EFA, 0x1A1F);    //           
    A5142MIPI_write_cmos_sensor(0x3ED2, 0xD965);    //           
    A5142MIPI_write_cmos_sensor(0x3ED8, 0x7F1B);    //             
    A5142MIPI_write_cmos_sensor(0x3EDA, 0x2F11);    //             
    A5142MIPI_write_cmos_sensor(0x3EE2, 0x0060);    //             
    A5142MIPI_write_cmos_sensor(0x3EF2, 0xD965);    //           
    A5142MIPI_write_cmos_sensor(0x3EF8, 0x797F);    //            
    A5142MIPI_write_cmos_sensor(0x3EFC, 0x286F);    //             
    A5142MIPI_write_cmos_sensor(0x3EFE, 0x2C01);    //            

    //                 
    A5142MIPI_write_cmos_sensor(0x3E00, 0x042F);    //                  
    A5142MIPI_write_cmos_sensor(0x3E02, 0xFFFF);    //                  
    A5142MIPI_write_cmos_sensor(0x3E04, 0xFFFF);    //                  
    A5142MIPI_write_cmos_sensor(0x3E06, 0xFFFF);    //                  
    A5142MIPI_write_cmos_sensor(0x3E08, 0x8071);    //                  
    A5142MIPI_write_cmos_sensor(0x3E0A, 0x7281);    //                  
    A5142MIPI_write_cmos_sensor(0x3E0C, 0x4011);    //                  
    A5142MIPI_write_cmos_sensor(0x3E0E, 0x8010);    //                  
    A5142MIPI_write_cmos_sensor(0x3E10, 0x60A5);    //                  
    A5142MIPI_write_cmos_sensor(0x3E12, 0x4080);    //                  
    A5142MIPI_write_cmos_sensor(0x3E14, 0x4180);    //                  
    A5142MIPI_write_cmos_sensor(0x3E16, 0x0018);    //                  
    A5142MIPI_write_cmos_sensor(0x3E18, 0x46B7);    //                  
    A5142MIPI_write_cmos_sensor(0x3E1A, 0x4994);    //                  
    A5142MIPI_write_cmos_sensor(0x3E1C, 0x4997);    //                  
    A5142MIPI_write_cmos_sensor(0x3E1E, 0x4682);    //                  
    A5142MIPI_write_cmos_sensor(0x3E20, 0x0018);    //                  
    A5142MIPI_write_cmos_sensor(0x3E22, 0x4241);    //                  
    A5142MIPI_write_cmos_sensor(0x3E24, 0x8000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E26, 0x1880);    //                  
    A5142MIPI_write_cmos_sensor(0x3E28, 0x4785);    //                  
    A5142MIPI_write_cmos_sensor(0x3E2A, 0x4992);    //                  
    A5142MIPI_write_cmos_sensor(0x3E2C, 0x4997);    //                  
    A5142MIPI_write_cmos_sensor(0x3E2E, 0x4780);    //                  
    A5142MIPI_write_cmos_sensor(0x3E30, 0x4D80);    //                  
    A5142MIPI_write_cmos_sensor(0x3E32, 0x100C);    //                  
    A5142MIPI_write_cmos_sensor(0x3E34, 0x8000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E36, 0x184A);    //                  
    A5142MIPI_write_cmos_sensor(0x3E38, 0x8042);    //                  
    A5142MIPI_write_cmos_sensor(0x3E3A, 0x001A);    //                  
    A5142MIPI_write_cmos_sensor(0x3E3C, 0x9610);    //                  
    A5142MIPI_write_cmos_sensor(0x3E3E, 0x0C80);    //                  
    A5142MIPI_write_cmos_sensor(0x3E40, 0x4DC6);    //                  
    A5142MIPI_write_cmos_sensor(0x3E42, 0x4A80);    //                  
    A5142MIPI_write_cmos_sensor(0x3E44, 0x0018);    //                  
    A5142MIPI_write_cmos_sensor(0x3E46, 0x8042);    //                  
    A5142MIPI_write_cmos_sensor(0x3E48, 0x8041);    //                  
    A5142MIPI_write_cmos_sensor(0x3E4A, 0x0018);    //                  
    A5142MIPI_write_cmos_sensor(0x3E4C, 0x804B);    //                  
    A5142MIPI_write_cmos_sensor(0x3E4E, 0xB74B);    //                  
    A5142MIPI_write_cmos_sensor(0x3E50, 0x8010);    //                  
    A5142MIPI_write_cmos_sensor(0x3E52, 0x6056);    //                  
    A5142MIPI_write_cmos_sensor(0x3E54, 0x001C);    //                  
    A5142MIPI_write_cmos_sensor(0x3E56, 0x8211);    //                  
    A5142MIPI_write_cmos_sensor(0x3E58, 0x8056);    //                  
    A5142MIPI_write_cmos_sensor(0x3E5A, 0x827C);    //                  
    A5142MIPI_write_cmos_sensor(0x3E5C, 0x0970);    //                  
    A5142MIPI_write_cmos_sensor(0x3E5E, 0x8082);    //                  
    A5142MIPI_write_cmos_sensor(0x3E60, 0x7281);    //                  
    A5142MIPI_write_cmos_sensor(0x3E62, 0x4C40);    //                  
    A5142MIPI_write_cmos_sensor(0x3E64, 0x8E4D);    //                  
    A5142MIPI_write_cmos_sensor(0x3E66, 0x8110);    //                  
    A5142MIPI_write_cmos_sensor(0x3E68, 0x0CAF);    //                  
    A5142MIPI_write_cmos_sensor(0x3E6A, 0x4D80);    //                  
    A5142MIPI_write_cmos_sensor(0x3E6C, 0x100C);    //                  
    A5142MIPI_write_cmos_sensor(0x3E6E, 0x8440);    //                  
    A5142MIPI_write_cmos_sensor(0x3E70, 0x4C81);    //                  
    A5142MIPI_write_cmos_sensor(0x3E72, 0x7C5F);    //                  
    A5142MIPI_write_cmos_sensor(0x3E74, 0x7000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E76, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E78, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E7A, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E7C, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E7E, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E80, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E82, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E84, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E86, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E88, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E8A, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E8C, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E8E, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E90, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E92, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E94, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E96, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E98, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E9A, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E9C, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3E9E, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EA0, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EA2, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EA4, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EA6, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EA8, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EAA, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EAC, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EAE, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EB0, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EB2, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EB4, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EB6, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EB8, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EBA, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EBC, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EBE, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EC0, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EC2, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EC4, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EC6, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3EC8, 0x0000);    //                  
    A5142MIPI_write_cmos_sensor(0x3ECA, 0x0000);    //                  

    //                      
    A5142MIPI_write_cmos_sensor(0x3170, 0x2150);    //               
    A5142MIPI_write_cmos_sensor(0x317A, 0x0150);    //                
    A5142MIPI_write_cmos_sensor(0x3ECC, 0x2200);    //           
    A5142MIPI_write_cmos_sensor(0x3174, 0x0000);    //                
    A5142MIPI_write_cmos_sensor(0x3176, 0x0000);    //                

    A5142MIPI_write_cmos_sensor(0x30BC, 0x0384);
    A5142MIPI_write_cmos_sensor(0x30C0, 0x1220);
    A5142MIPI_write_cmos_sensor(0x30D4, 0x9200);
    A5142MIPI_write_cmos_sensor(0x30B2, 0xC000);
    
    A5142MIPI_write_cmos_sensor(0x31B0, 0x00C4);
    A5142MIPI_write_cmos_sensor(0x31B2, 0x0064);
    A5142MIPI_write_cmos_sensor(0x31B4, 0x0E77);
    A5142MIPI_write_cmos_sensor(0x31B6, 0x0D24);
    A5142MIPI_write_cmos_sensor(0x31B8, 0x020E);
    A5142MIPI_write_cmos_sensor(0x31BA, 0x0710);
    A5142MIPI_write_cmos_sensor(0x31BC, 0x2A0D);
    A5142MIPI_write_cmos_sensor(0x31BE, 0xC007);

    //                                                           
    //                                                                           
    //                                                        
    
    A5142MIPI_write_cmos_sensor(0x305E, 0x112E);    //            
    A5142MIPI_write_cmos_sensor(0x30F0, 0x0000);    //                                          

    //                                            
    #ifdef RAW10
        A5142MIPI_write_cmos_sensor(0x0300, 0x05);  //                  
        A5142MIPI_write_cmos_sensor(0x0302, 0x01);  //                  
        A5142MIPI_write_cmos_sensor(0x0304, 0x02);  //                   
        A5142MIPI_write_cmos_sensor(0x0306, 0x28);  //                       
        A5142MIPI_write_cmos_sensor(0x0308, 0x0A);  //                    
        A5142MIPI_write_cmos_sensor(0x030A, 0x01);  //                  
    #else
        #ifdef MIPI_INTERFACE
            A5142MIPI_write_cmos_sensor(0x0300, 0x04);  //                  
        #else
            A5142MIPI_write_cmos_sensor(0x0300, 0x08);  //                  
        #endif
        A5142MIPI_write_cmos_sensor(0x0302, 0x01);  //                  
        A5142MIPI_write_cmos_sensor(0x0304, 0x02);  //                   
        A5142MIPI_write_cmos_sensor(0x0306, 0x20);  //                       
        A5142MIPI_write_cmos_sensor(0x0308, 0x08);  //                   
        A5142MIPI_write_cmos_sensor(0x030A, 0x01);  //                  
    #endif

    //                                                                          
    //                                             
    //                                                                
    //                                                                         
    //                                                                       
    //                                                            
#if defined(OTP_SUPPORT)
   otp_lenc_update(0); //                    
#endif
    mDELAY(20);              //                  
}   /*                         */


/*                                                                        
          
                        
 
             
                                    
 
            
                                     
 
         
        
 
                  
 
                                                                        */
UINT32 A5142MIPIGetSensorID(UINT32 *sensorID) 
{
    const kal_uint8 i2c_addr[] = {A5142MIPI_WRITE_ID_1, A5142MIPI_WRITE_ID_2}; 
    kal_uint16 sensor_id = 0xFFFF;
    kal_uint16 i;
    //                            

    //                                     
    for(i = 0; i < sizeof(i2c_addr) / sizeof(i2c_addr[0]); i++)
    {
        spin_lock(&a5142mipiraw_drv_lock);
        A5142MIPI_sensor.i2c_write_id = A5142MIPI_WRITE_ID_1;//            
        spin_unlock(&a5142mipiraw_drv_lock);

        SENSORDB( "i2c address is %x ", A5142MIPI_sensor.i2c_write_id);
        
        sensor_id = A5142MIPI_read_cmos_sensor(0x0000);
        if(sensor_id == A5142MIPI_SENSOR_ID)
            break;
    }

    *sensorID  = sensor_id;
        
    SENSORDB("sensor_id is %x ", *sensorID );
 
    if (*sensorID != A5142MIPI_SENSOR_ID) {
        *sensorID = 0xFFFFFFFF; 
        return ERROR_SENSOR_CONNECT_FAIL;
    }
    
    return ERROR_NONE;
}


/*                                                                        
          
                 
 
             
                                                         
 
            
        
 
         
        
 
                  
 
                                                                        */
UINT32 A5142MIPIOpen(void)
{
    kal_uint16 sensor_id = 0;
//                
//                  

    A5142MIPIGetSensorID((UINT32 *)(&sensor_id));
    
    SENSORDB("sensor_id is %x ", sensor_id);
    
    if (sensor_id != A5142MIPI_SENSOR_ID){
        return ERROR_SENSOR_CONNECT_FAIL;
    }
printk("[LONG-CAM]A5142MIPIOpen\r\n");
    A5142MIPI_Init_setting();

    spin_lock(&a5142mipiraw_drv_lock);
    A5142DuringTestPattern = KAL_FALSE;
    A5142MIPI_sensor_gain_base = read_A5142MIPI_gain();
    g_iA5142MIPI_Mode = A5142MIPI_MODE_INIT;
    spin_unlock(&a5142mipiraw_drv_lock);
    
    return ERROR_NONE;
}


/*                                                                        
          
                        
 
             
                                          
 
            
        
 
         
        
 
                  
 
                                                                        */
void A5142MIPI_NightMode(kal_bool bEnable)
{
    //                                       
    
}


/*                                                                        
          
                  
 
             
                                                     
 
            
        
 
         
        
 
                  
 
                                                                        */
UINT32 A5142MIPIClose(void)
{
    return ERROR_NONE;
}   /*                  */


void A5142MIPI_Set_Mirror_Flip(kal_uint8 image_mirror)
{
    SENSORDB("image_mirror = %d", image_mirror);
    
    switch (image_mirror)
    {
        case IMAGE_NORMAL:
            A5142MIPI_write_cmos_sensor_8(0x0101,0x00);
        break;
        case IMAGE_H_MIRROR:
            A5142MIPI_write_cmos_sensor_8(0x0101,0x01);
        break;
        case IMAGE_V_MIRROR:
            A5142MIPI_write_cmos_sensor_8(0x0101,0x02);
        break;
        case IMAGE_HV_MIRROR:
            A5142MIPI_write_cmos_sensor_8(0x0101,0x03);
        break;
    }
}


static void A5142MIPI_preview_setting(void)
{
    kal_uint16 temp;

    //              
    A5142MIPI_write_cmos_sensor_8(0x0100, 0x0);     //            
    
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x01);    //                             
    
    //                                 
    #ifdef MIPI_INTERFACE
        #ifdef RAW10
            //                                                                      
            A5142MIPI_write_cmos_sensor(0x3064, 0xB800);    //          
            A5142MIPI_write_cmos_sensor(0x31AE, 0x0202);    //                     
            A5142MIPI_write_cmos_sensor(0x0112, 0x0A0A);    //                 
        #else
            //                                                                      
            A5142MIPI_write_cmos_sensor(0x3064, 0x0805);    //          
            A5142MIPI_write_cmos_sensor(0x31AE, 0x0202);    //         
            A5142MIPI_write_cmos_sensor(0x0112, 0x0808);    //                
        #endif
    #endif

    //                                          
    #ifdef RAW10
        A5142MIPI_write_cmos_sensor(0x0300, 0x05);  //                  
        A5142MIPI_write_cmos_sensor(0x0302, 0x01);  //                  
        A5142MIPI_write_cmos_sensor(0x0304, 0x02);  //                   
        A5142MIPI_write_cmos_sensor(0x0306, 0x28);  //                       
        A5142MIPI_write_cmos_sensor(0x0308, 0x0A);  //                    
        A5142MIPI_write_cmos_sensor(0x030A, 0x01);  //                  
    #else   
        #ifdef MIPI_INTERFACE
            A5142MIPI_write_cmos_sensor(0x0300, 0x04);  //                  
        #else
            A5142MIPI_write_cmos_sensor(0x0300, 0x08);  //                  
        #endif
        A5142MIPI_write_cmos_sensor(0x0302, 0x01);  //                  
        A5142MIPI_write_cmos_sensor(0x0304, 0x02);  //                   
        A5142MIPI_write_cmos_sensor(0x0306, 0x20);  //                       
        A5142MIPI_write_cmos_sensor(0x0308, 0x08);  //                   
        A5142MIPI_write_cmos_sensor(0x030A, 0x01);  //                  
    #endif

    mDELAY(10);
    
    A5142MIPI_write_cmos_sensor(0x0344, 0x0008);    //                    
    A5142MIPI_write_cmos_sensor(0x0346, 0x0008);    //                    
    A5142MIPI_write_cmos_sensor(0x0348, 0x0A25);    //                       
    A5142MIPI_write_cmos_sensor(0x034A, 0x079D);    //                         
    temp = A5142MIPI_read_cmos_sensor(0x3040);
    temp = temp & 0xF000;
    temp = temp | 0x04C3 ;
    A5142MIPI_write_cmos_sensor(0x3040, temp);      //                                                          
    A5142MIPI_write_cmos_sensor(0x034C, 0x0510);    //                        
    A5142MIPI_write_cmos_sensor(0x034E, 0x03CC);    //                        

    A5142MIPI_write_cmos_sensor(0x300C, 0x0C4F);    //                  
    A5142MIPI_write_cmos_sensor(0x300A, 0x044C);    //                  
    
    //                                                                          
    A5142MIPI_write_cmos_sensor(0x3014, 0x0908);    //                      
    A5142MIPI_write_cmos_sensor(0x3010, 0x0184);    //                

    A5142MIPI_write_cmos_sensor_8(0x0104, 0x00);    //                       
    
    //               
    A5142MIPI_write_cmos_sensor_8(0x0100, 0x01);    //            

    spin_lock(&a5142mipiraw_drv_lock);
    #ifdef MIPI_INTERFACE
        A5142MIPI_sensor.preview_vt_clk = 1040;
    #else
        A5142MIPI_sensor.preview_vt_clk = 520;
    #endif
    spin_unlock(&a5142mipiraw_drv_lock);

    mDELAY(100); 
}

static void A5142MIPI_capture_setting(void)
{
    kal_uint16 temp;

    //              
    A5142MIPI_write_cmos_sensor_8(0x0100, 0x00);  //            

    A5142MIPI_write_cmos_sensor_8(0x0104, 0x01); //                            

    #ifdef MIPI_INTERFACE
        #ifdef RAW10
            //                                                                            
            A5142MIPI_write_cmos_sensor(0x3064, 0xB800);    //          
            A5142MIPI_write_cmos_sensor(0x31AE, 0x0202);    //                     
            A5142MIPI_write_cmos_sensor(0x0112, 0x0A0A);    //                 
        #else
            //                                                                            
            A5142MIPI_write_cmos_sensor(0x3064, 0x0805);    //          
            A5142MIPI_write_cmos_sensor(0x31AE, 0x0202);    //         
            A5142MIPI_write_cmos_sensor(0x0112, 0x0808);    //                
        #endif
    #endif

    //                                              
    #ifdef RAW10
        A5142MIPI_write_cmos_sensor(0x0300, 0x05);  //                  
        A5142MIPI_write_cmos_sensor(0x0302, 0x01);  //                  
        A5142MIPI_write_cmos_sensor(0x0304, 0x04);  //                   
        A5142MIPI_write_cmos_sensor(0x0306, 0x56);  //                       
        A5142MIPI_write_cmos_sensor(0x0308, 0x0A);  //                    
        A5142MIPI_write_cmos_sensor(0x030A, 0x01);  //                  
    #else       
        #ifdef MIPI_INTERFACE
            A5142MIPI_write_cmos_sensor(0x0300, 0x04);  //                  
        #else
            A5142MIPI_write_cmos_sensor(0x0300, 0x08);  //                  
        #endif
        A5142MIPI_write_cmos_sensor(0x0302, 0x01);  //                  
        A5142MIPI_write_cmos_sensor(0x0304, 0x02);  //                   
        A5142MIPI_write_cmos_sensor(0x0306, 0x20);  //                       
        A5142MIPI_write_cmos_sensor(0x0308, 0x08);  //                   
        A5142MIPI_write_cmos_sensor(0x030A, 0x01);  //                  
    #endif

    mDELAY(10);

    A5142MIPI_write_cmos_sensor(0x0344, 0x0008);    //                  
    A5142MIPI_write_cmos_sensor(0x0346, 0x0008);    //                   
    A5142MIPI_write_cmos_sensor(0x0348, 0x0A27);    //                  
    A5142MIPI_write_cmos_sensor(0x034A, 0x079F);    //                 

    temp = A5142MIPI_read_cmos_sensor(0x3040);
    temp = temp & 0xF000 ;
    temp = temp | 0x0041;
    A5142MIPI_write_cmos_sensor(0x3040, temp);  //                                           

    A5142MIPI_write_cmos_sensor(0x034C, 0x0A20);    //                   
    A5142MIPI_write_cmos_sensor(0x034E, 0x0798);    //                    
    A5142MIPI_write_cmos_sensor(0x300A, 0x07E5);    //                         
    A5142MIPI_write_cmos_sensor(0x300C, 0x0E6E);    //                         
    A5142MIPI_write_cmos_sensor(0x3010, 0x00A0);    //                      
    //                                                                               
    A5142MIPI_write_cmos_sensor(0x3014, 0x0C8C);    //                             

    A5142MIPI_write_cmos_sensor_8(0x0104, 0x00);    //                      

    //               
    A5142MIPI_write_cmos_sensor_8(0x0100, 0x01);    //            

    spin_lock(&a5142mipiraw_drv_lock);
    #ifdef MIPI_INTERFACE
        A5142MIPI_sensor.capture_vt_clk = 1118;
    #else
        A5142MIPI_sensor.capture_vt_clk = 520;
    #endif
    spin_unlock(&a5142mipiraw_drv_lock);

    mDELAY(100);
}


/*                                                                        
          
                      
 
             
                                                         
 
            
                             
                                  
 
         
        
 
                  
 
                                                                        */
static void A5142MIPI_SetDummy(kal_bool mode,const kal_uint16 iDummyPixels, const kal_uint16 iDummyLines)
{
    kal_uint16 Line_length, Frame_length;
    
    if(mode == KAL_TRUE) //       
    {
        Line_length   = A5142MIPI_PV_PERIOD_PIXEL_NUMS + iDummyPixels;
        Frame_length = A5142MIPI_PV_PERIOD_LINE_NUMS  + iDummyLines;
    }
    else   //       
    {
        Line_length   = A5142MIPI_FULL_PERIOD_PIXEL_NUMS + iDummyPixels;
        Frame_length = A5142MIPI_FULL_PERIOD_LINE_NUMS  + iDummyLines;
    }
    
    spin_lock(&a5142mipiraw_drv_lock);
    A5142_Frame_Length_preview = Frame_length;
    spin_unlock(&a5142mipiraw_drv_lock);
    
    SENSORDB("Frame_length = %d, Line_length = %d", Frame_length, Line_length);

    A5142MIPI_write_cmos_sensor_8(0x0104, 0x01); //                       
    A5142MIPI_write_cmos_sensor(0x0340, Frame_length);
    A5142MIPI_write_cmos_sensor(0x0342, Line_length);
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x00); //                      
    
}   /*                     */


/*                                                                        
          
                    
 
             
                                           
 
            
                                                                           
                                                                               
 
         
        
 
                  
 
                                                                        */
UINT32 A5142MIPIPreview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                                                MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    //                

    SENSORDB("Enter!");

    A5142MIPI_preview_setting();
    
    //                                                                 

    spin_lock(&a5142mipiraw_drv_lock);
    g_iA5142MIPI_Mode = A5142MIPI_MODE_PREVIEW;
    //                                
    spin_unlock(&a5142mipiraw_drv_lock);

    //                                   
    spin_lock(&a5142mipiraw_drv_lock);
    A5142MIPI_PV_dummy_pixels = 0;
    A5142MIPI_PV_dummy_lines  = 0;
    A5142_Frame_Length_preview = A5142MIPI_PV_PERIOD_LINE_NUMS;
    spin_unlock(&a5142mipiraw_drv_lock);

    #if 0
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x01);    //                       
    A5142MIPI_write_cmos_sensor(0x0202, temp); /*                         */
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x00);    //                       

    memcpy(&A5142MIPISensorConfigData, sensor_config_data, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
    #endif
    
    return ERROR_NONE;
}   /*                    */


/*                                                                              
 
                                                                               */
UINT32 A5142MIPICapture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                                                MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    //                                              

    SENSORDB("Enter!");

    A5142MIPI_capture_setting();

    //                                                                 

    spin_lock(&a5142mipiraw_drv_lock);
    g_iA5142MIPI_Mode = A5142MIPI_MODE_CAPTURE; 
    spin_unlock(&a5142mipiraw_drv_lock);

    //                                   
    spin_lock(&a5142mipiraw_drv_lock);
    A5142MIPI_dummy_pixels = 0;
    A5142MIPI_dummy_lines  = 0;
    A5142_Frame_Length_preview = A5142MIPI_FULL_PERIOD_LINE_NUMS;
    spin_unlock(&a5142mipiraw_drv_lock);
    
    #if 0
    SENSORDB("preview shutter =%d ",shutter);

    shutter = shutter * (A5142MIPI_PV_PERIOD_PIXEL_NUMS + A5142MIPI_PV_dummy_pixels)/(A5142MIPI_FULL_PERIOD_PIXEL_NUMS +A5142MIPI_dummy_pixels);
    shutter = shutter * A5142MIPI_sensor.capture_vt_clk / A5142MIPI_sensor.preview_vt_clk;

    SENSORDB("capture  shutter =%d , gain = %d\n",shutter, read_A5142MIPI_gain());
    
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x01);    //                       
    A5142MIPI_write_cmos_sensor(0x0202, shutter);   /*                         */
    A5142MIPI_write_cmos_sensor_8(0x0104, 0x00);    //                       
    #endif
    
    return ERROR_NONE;
}   /*                    */


UINT32 A5142MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
    pSensorResolution->SensorFullWidth     =  A5142MIPI_IMAGE_SENSOR_FULL_WIDTH;
    pSensorResolution->SensorFullHeight    =  A5142MIPI_IMAGE_SENSOR_FULL_HEIGHT;
    
    pSensorResolution->SensorPreviewWidth  =  A5142MIPI_IMAGE_SENSOR_PV_WIDTH;
    pSensorResolution->SensorPreviewHeight =  A5142MIPI_IMAGE_SENSOR_PV_HEIGHT;
    
    pSensorResolution->SensorVideoWidth     =  A5142MIPI_IMAGE_SENSOR_PV_WIDTH;
    pSensorResolution->SensorVideoHeight    =  A5142MIPI_IMAGE_SENSOR_PV_HEIGHT;        

    return ERROR_NONE;
}   /*                          */


UINT32 A5142MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
                                                MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
                                                MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    switch(ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
            pSensorInfo->SensorPreviewResolutionX = A5142MIPI_IMAGE_SENSOR_FULL_WIDTH; /*         */
            pSensorInfo->SensorPreviewResolutionY = A5142MIPI_IMAGE_SENSOR_FULL_HEIGHT; /*         */
            pSensorInfo->SensorCameraPreviewFrameRate = 15; /*         */
        break;

        default:
            pSensorInfo->SensorPreviewResolutionX = A5142MIPI_IMAGE_SENSOR_PV_WIDTH; /*         */
            pSensorInfo->SensorPreviewResolutionY = A5142MIPI_IMAGE_SENSOR_PV_HEIGHT; /*         */
            pSensorInfo->SensorCameraPreviewFrameRate = 30; /*         */
        break;
    }
    pSensorInfo->SensorFullResolutionX = A5142MIPI_IMAGE_SENSOR_FULL_WIDTH; /*         */
    pSensorInfo->SensorFullResolutionY = A5142MIPI_IMAGE_SENSOR_FULL_HEIGHT; /*         */

    pSensorInfo->SensorVideoFrameRate = 30; /*         */
    pSensorInfo->SensorStillCaptureFrameRate= 15; /*         */
    pSensorInfo->SensorWebCamCaptureFrameRate= 15; /*         */

    pSensorInfo->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW; /*         */
    pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW; //                       
    pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
    pSensorInfo->SensorInterruptDelayLines = 1; /*         */
    pSensorInfo->SensorResetActiveHigh = FALSE; /*         */
    pSensorInfo->SensorResetDelayCount = 5; /*         */

    #ifdef MIPI_INTERFACE
        pSensorInfo->SensroInterfaceType        = SENSOR_INTERFACE_TYPE_MIPI;
    #else
        pSensorInfo->SensroInterfaceType        = SENSOR_INTERFACE_TYPE_PARALLEL;
    #endif
    pSensorInfo->SensorOutputDataFormat     = A5142MIPI_DATA_FORMAT;

    pSensorInfo->CaptureDelayFrame = 1; 
    pSensorInfo->PreviewDelayFrame = 2; 
    pSensorInfo->VideoDelayFrame = 5; 
    pSensorInfo->SensorMasterClockSwitch = 0; /*         */
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_6MA;      
    pSensorInfo->AEShutDelayFrame = 0;          /*                                                   */
    pSensorInfo->AESensorGainDelayFrame = 0;    /*                                  */
    pSensorInfo->AEISPGainDelayFrame = 2;
       
    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
            pSensorInfo->SensorClockFreq=26;
            pSensorInfo->SensorClockDividCount= 3; /*         */
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2; /*         */
            pSensorInfo->SensorPixelClockCount= 3; /*         */
            pSensorInfo->SensorDataLatchCount= 2; /*         */
            pSensorInfo->SensorGrabStartX = A5142MIPI_FULL_START_X; 
            pSensorInfo->SensorGrabStartY = A5142MIPI_FULL_START_Y;   
            
            #ifdef MIPI_INTERFACE
                pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE;         
                pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
                pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 0x20; 
                pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0; 
                pSensorInfo->SensorWidthSampling = 0;
                pSensorInfo->SensorHightSampling = 0;
                pSensorInfo->SensorPacketECCOrder = 1;
            #endif
            break;
        default:
            pSensorInfo->SensorClockFreq=26;
            pSensorInfo->SensorClockDividCount= 3; /*         */
            pSensorInfo->SensorClockRisingCount= 0;
            pSensorInfo->SensorClockFallingCount= 2; /*         */
            pSensorInfo->SensorPixelClockCount= 3; /*         */
            pSensorInfo->SensorDataLatchCount= 2; /*         */
            pSensorInfo->SensorGrabStartX = A5142MIPI_PV_START_X; 
            pSensorInfo->SensorGrabStartY = A5142MIPI_PV_START_Y;    
            
            #ifdef MIPI_INTERFACE
                pSensorInfo->SensorMIPILaneNumber = SENSOR_MIPI_2_LANE;         
                pSensorInfo->MIPIDataLowPwr2HighSpeedTermDelayCount = 0; 
                pSensorInfo->MIPIDataLowPwr2HighSpeedSettleDelayCount = 0x20; 
                pSensorInfo->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
                pSensorInfo->SensorWidthSampling = 0;
                pSensorInfo->SensorHightSampling = 0;
                pSensorInfo->SensorPacketECCOrder = 1;
            #endif
            break;
    }

    //                                                                                         

    return ERROR_NONE;
}   /*                    */


UINT32 A5142MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
                                                MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    spin_lock(&a5142mipiraw_drv_lock);
    A5142_CurrentScenarioId = ScenarioId;
    spin_unlock(&a5142mipiraw_drv_lock);
    
    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
            A5142MIPIPreview(pImageWindow, pSensorConfigData);
            break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
            A5142MIPICapture(pImageWindow, pSensorConfigData);
            break;
        default:
            return ERROR_INVALID_SCENARIO_ID;  
    }
    
    return ERROR_NONE;
} /*                    */


UINT32 A5142MIPISetVideoMode(UINT16 u2FrameRate)
{
    kal_uint16 MAX_Frame_length =0;

    SENSORDB("u2FrameRate =%d", u2FrameRate);

    if(u2FrameRate >30 || u2FrameRate <5)
        SENSORDB("Error frame rate seting");

    if (A5142MIPI_MODE_PREVIEW == g_iA5142MIPI_Mode)
    {
        MAX_Frame_length = A5142MIPI_sensor.preview_vt_clk*100000/(A5142MIPI_PV_PERIOD_PIXEL_NUMS+A5142MIPI_PV_dummy_pixels)/u2FrameRate;
        //                                                                                                                                        
        if(MAX_Frame_length < A5142MIPI_PV_PERIOD_LINE_NUMS )
            MAX_Frame_length = A5142MIPI_PV_PERIOD_LINE_NUMS;

        spin_lock(&a5142mipiraw_drv_lock);
        A5142MIPI_PV_dummy_lines = MAX_Frame_length - A5142MIPI_PV_PERIOD_LINE_NUMS  ;
        spin_unlock(&a5142mipiraw_drv_lock);

        A5142MIPI_SetDummy(KAL_TRUE, A5142MIPI_PV_dummy_pixels, A5142MIPI_PV_dummy_lines);
    }
    else if (A5142MIPI_MODE_CAPTURE == g_iA5142MIPI_Mode)
    {
        MAX_Frame_length = A5142MIPI_sensor.capture_vt_clk*100000/(A5142MIPI_FULL_PERIOD_PIXEL_NUMS+A5142MIPI_dummy_pixels)/u2FrameRate;
        if(MAX_Frame_length < A5142MIPI_FULL_PERIOD_LINE_NUMS )
            MAX_Frame_length = A5142MIPI_FULL_PERIOD_LINE_NUMS;

        spin_lock(&a5142mipiraw_drv_lock);
        A5142MIPI_dummy_lines = MAX_Frame_length - A5142MIPI_FULL_PERIOD_LINE_NUMS  ;
        spin_unlock(&a5142mipiraw_drv_lock);

        A5142MIPI_SetDummy(KAL_FALSE, A5142MIPI_dummy_pixels, A5142MIPI_dummy_lines);
    }

    return ERROR_NONE;
}

UINT32 A5142MIPISetAutoFlickerMode(kal_bool bEnable, UINT16 u2FrameRate)
{
    SENSORDB("frame rate(10base) = %d %d", bEnable, u2FrameRate);

    //                                                              
    //                                                                        
    if (KAL_TRUE == A5142DuringTestPattern) return ERROR_NONE;

    if(bEnable) 
    {   
        //                       
        A5142MIPI_write_cmos_sensor_8(0x0104, 1);       
        A5142MIPI_write_cmos_sensor(0x0340, A5142_Frame_Length_preview + AUTO_FLICKER_NO);
        A5142MIPI_write_cmos_sensor_8(0x0104, 0);           
    } else 
    {
        //                     
        A5142MIPI_write_cmos_sensor_8(0x0104, 1);        
        A5142MIPI_write_cmos_sensor(0x0340, A5142_Frame_Length_preview);
        A5142MIPI_write_cmos_sensor_8(0x0104, 0);               
    }
    return ERROR_NONE;
}

UINT32 A5142MIPI_SetTestPatternMode(kal_bool bEnable)
{
    kal_uint16 temp;

    SENSORDB("[A5142MIPI_SetTestPatternMode] Test pattern enable:%d\n", bEnable);

	if(bEnable) 
	{
	    spin_lock(&a5142mipiraw_drv_lock);
        A5142DuringTestPattern = KAL_TRUE;
        spin_unlock(&a5142mipiraw_drv_lock);
        
        //               
        //                                                                
        temp = A5142MIPI_read_cmos_sensor(0x3044);
        //                                
        temp = temp & 0xFDFF;
        A5142MIPI_write_cmos_sensor(0x3044, temp);        

        //              
        //                                                                      
        temp = A5142MIPI_read_cmos_sensor(0x30C0);
        //                                
        temp = temp & 0xFFFE;
        temp = temp | 0x0001;
        A5142MIPI_write_cmos_sensor(0x30C0, temp);

        //              
        //                                                                          
        temp = A5142MIPI_read_cmos_sensor(0x30D4);
        //                                
        temp = temp & 0x7FFF;
        A5142MIPI_write_cmos_sensor(0x30D4, temp);


        //              
        //                                                              
        temp = A5142MIPI_read_cmos_sensor(0x31E0);
        //                                
        temp = temp & 0xFFFE;
        A5142MIPI_write_cmos_sensor(0x31E0, temp);


        //               
        //                                                                               
        temp = A5142MIPI_read_cmos_sensor(0x3180);
        //                                
        temp = temp & 0x7FFF;
        A5142MIPI_write_cmos_sensor(0x3180, temp);

        //              
        //                                             
        temp = A5142MIPI_read_cmos_sensor(0x301A);
        //                                
        temp = temp & 0xFFF7;
        A5142MIPI_write_cmos_sensor(0x301A, temp);

        //                
        temp = A5142MIPI_read_cmos_sensor(0x301E);
        //                                
        A5142MIPI_write_cmos_sensor(0x301E, 0x0000);    //               

        //                            

        //                         
		A5142MIPI_write_cmos_sensor(0x3070, 0x0001);               
		A5142MIPI_write_cmos_sensor(0x3072, 0x0100);               
		A5142MIPI_write_cmos_sensor(0x3074, 0x0100);               
		A5142MIPI_write_cmos_sensor(0x3076, 0x0100);               
		A5142MIPI_write_cmos_sensor(0x3078, 0x0100);

        //                        
		//                                                           
	}
	else        
	{
	    spin_lock(&a5142mipiraw_drv_lock);
        A5142DuringTestPattern = KAL_FALSE;
        spin_unlock(&a5142mipiraw_drv_lock);
        
        //                                                               
        //                                                                     
        //                                                                         
        //                                                             
        //                                                                              
        //                                            
        //                                                                 

		A5142MIPI_write_cmos_sensor(0x3070,0x0000);
	}
	
	mDELAY(100);
    
    return ERROR_NONE;
}




UINT32 A5142MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
                                                                UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{    
    UINT16 *pFeatureReturnPara16=(UINT16 *) pFeaturePara;
    UINT16 *pFeatureData16=(UINT16 *) pFeaturePara;
    UINT32 *pFeatureReturnPara32=(UINT32 *) pFeaturePara;
    UINT32 *pFeatureData32=(UINT32 *) pFeaturePara;
    UINT32 SensorRegNumber;
    UINT32 i;
    PNVRAM_SENSOR_DATA_STRUCT pSensorDefaultData=(PNVRAM_SENSOR_DATA_STRUCT) pFeaturePara;
    MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData=(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
    MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData=(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;
    MSDK_SENSOR_GROUP_INFO_STRUCT *pSensorGroupInfo=(MSDK_SENSOR_GROUP_INFO_STRUCT *) pFeaturePara;
    MSDK_SENSOR_ITEM_INFO_STRUCT *pSensorItemInfo=(MSDK_SENSOR_ITEM_INFO_STRUCT *) pFeaturePara;
    MSDK_SENSOR_ENG_INFO_STRUCT *pSensorEngInfo=(MSDK_SENSOR_ENG_INFO_STRUCT *) pFeaturePara;

    switch (FeatureId)
    {
        case SENSOR_FEATURE_GET_RESOLUTION:
            *pFeatureReturnPara16++=A5142MIPI_IMAGE_SENSOR_FULL_WIDTH;
            *pFeatureReturnPara16=A5142MIPI_IMAGE_SENSOR_FULL_HEIGHT;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_PERIOD:
            switch(A5142_CurrentScenarioId)
            {
                case MSDK_SCENARIO_ID_CAMERA_ZSD:
                case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
                    *pFeatureReturnPara16++= A5142MIPI_FULL_PERIOD_PIXEL_NUMS + A5142MIPI_dummy_pixels;//                                                      
                    *pFeatureReturnPara16=A5142MIPI_FULL_PERIOD_LINE_NUMS + A5142MIPI_dummy_lines;
                    *pFeatureParaLen=4;
                     break;
                default:
                     *pFeatureReturnPara16++= A5142MIPI_PV_PERIOD_PIXEL_NUMS + A5142MIPI_PV_dummy_pixels;//                                                      
                    *pFeatureReturnPara16=A5142MIPI_PV_PERIOD_LINE_NUMS + A5142MIPI_PV_dummy_lines;
                    *pFeatureParaLen=4;
                     break;
            }
            break;
        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
            switch(A5142_CurrentScenarioId)
            {
                case MSDK_SCENARIO_ID_CAMERA_ZSD:
                case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
                    *pFeatureReturnPara32 = A5142MIPI_sensor.capture_vt_clk * 100000;
                    *pFeatureParaLen=4;
                    break;
                default:
                    *pFeatureReturnPara32 = A5142MIPI_sensor.preview_vt_clk * 100000;
                    *pFeatureParaLen=4;
                    break;
            }
            break;
        case SENSOR_FEATURE_SET_ESHUTTER:
            A5142MIPI_SetShutter(*pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_NIGHTMODE:
            A5142MIPI_NightMode((BOOL) *pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_GAIN:
            A5142MIPI_Set_gain((UINT16) *pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_FLASHLIGHT:
            break;
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
            //                                            
            break;
        case SENSOR_FEATURE_SET_REGISTER:
            A5142MIPI_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
            break;
        case SENSOR_FEATURE_GET_REGISTER:
            pSensorRegData->RegData = A5142MIPI_read_cmos_sensor(pSensorRegData->RegAddr);
            break;
        case SENSOR_FEATURE_SET_CCT_REGISTER:
            SensorRegNumber=FACTORY_END_ADDR;   
            for (i=0;i<SensorRegNumber;i++)
            {
                spin_lock(&a5142mipiraw_drv_lock);
                A5142MIPISensorCCT[i].Addr=*pFeatureData32++;
                A5142MIPISensorCCT[i].Para=*pFeatureData32++;
                spin_unlock(&a5142mipiraw_drv_lock);
            }
            break;
        case SENSOR_FEATURE_GET_CCT_REGISTER:
            SensorRegNumber=FACTORY_END_ADDR;
            if (*pFeatureParaLen<(SensorRegNumber*sizeof(SENSOR_REG_STRUCT)+4))
                return FALSE;
            *pFeatureData32++=SensorRegNumber;
            
            for (i=0;i<SensorRegNumber;i++)
            {
                spin_lock(&a5142mipiraw_drv_lock);
                *pFeatureData32++=A5142MIPISensorCCT[i].Addr;
                *pFeatureData32++=A5142MIPISensorCCT[i].Para;
                spin_unlock(&a5142mipiraw_drv_lock);
            }
            
            break;
        case SENSOR_FEATURE_SET_ENG_REGISTER:
            SensorRegNumber=ENGINEER_END;
            for (i=0;i<SensorRegNumber;i++)
            {
                spin_lock(&a5142mipiraw_drv_lock);
                A5142MIPISensorReg[i].Addr=*pFeatureData32++;
                A5142MIPISensorReg[i].Para=*pFeatureData32++;
                spin_unlock(&a5142mipiraw_drv_lock);
            }
            break;
        case SENSOR_FEATURE_GET_ENG_REGISTER:
            SensorRegNumber=ENGINEER_END;
            if (*pFeatureParaLen<(SensorRegNumber*sizeof(SENSOR_REG_STRUCT)+4))
                return FALSE;
            *pFeatureData32++=SensorRegNumber;
            for (i=0;i<SensorRegNumber;i++)
            {
                spin_lock(&a5142mipiraw_drv_lock);
                *pFeatureData32++=A5142MIPISensorReg[i].Addr;
                *pFeatureData32++=A5142MIPISensorReg[i].Para;
                spin_unlock(&a5142mipiraw_drv_lock);
            }
            break;
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
            if (*pFeatureParaLen>=sizeof(NVRAM_SENSOR_DATA_STRUCT))
            {
                pSensorDefaultData->Version=NVRAM_CAMERA_SENSOR_FILE_VERSION;
                pSensorDefaultData->SensorId=A5142MIPI_SENSOR_ID;
                memcpy(pSensorDefaultData->SensorEngReg, A5142MIPISensorReg, sizeof(SENSOR_REG_STRUCT)*ENGINEER_END);
                memcpy(pSensorDefaultData->SensorCCTReg, A5142MIPISensorCCT, sizeof(SENSOR_REG_STRUCT)*FACTORY_END_ADDR);
            }
            else
                return FALSE;
            *pFeatureParaLen=sizeof(NVRAM_SENSOR_DATA_STRUCT);
            break;
        case SENSOR_FEATURE_GET_CONFIG_PARA:
            memcpy(pSensorConfigData, &A5142MIPISensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
            *pFeatureParaLen=sizeof(MSDK_SENSOR_CONFIG_STRUCT);
            break;
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
            A5142MIPI_camera_para_to_sensor();
            break;
            
        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
            A5142MIPI_sensor_to_camera_para();
            break;
        case SENSOR_FEATURE_GET_GROUP_COUNT:
            *pFeatureReturnPara32++=A5142MIPI_get_sensor_group_count();
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_GET_GROUP_INFO:
            A5142MIPI_get_sensor_group_info(pSensorGroupInfo->GroupIdx, pSensorGroupInfo->GroupNamePtr, &pSensorGroupInfo->ItemCount);
            *pFeatureParaLen=sizeof(MSDK_SENSOR_GROUP_INFO_STRUCT);
            break;
        case SENSOR_FEATURE_GET_ITEM_INFO:
            A5142MIPI_get_sensor_item_info(pSensorItemInfo->GroupIdx,pSensorItemInfo->ItemIdx, pSensorItemInfo);
            *pFeatureParaLen=sizeof(MSDK_SENSOR_ITEM_INFO_STRUCT);
            break;

        case SENSOR_FEATURE_SET_ITEM_INFO:
            A5142MIPI_set_sensor_item_info(pSensorItemInfo->GroupIdx, pSensorItemInfo->ItemIdx, pSensorItemInfo->ItemValue);
            *pFeatureParaLen=sizeof(MSDK_SENSOR_ITEM_INFO_STRUCT);
            break;

        case SENSOR_FEATURE_GET_ENG_INFO:
            pSensorEngInfo->SensorId = 221;
            pSensorEngInfo->SensorType = CMOS_SENSOR;
            pSensorEngInfo->SensorOutputDataFormat = A5142MIPI_DATA_FORMAT;
            *pFeatureParaLen=sizeof(MSDK_SENSOR_ENG_INFO_STRUCT);
            break;
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
            //                                                                             
            //                                           
            *pFeatureReturnPara32=LENS_DRIVER_ID_DO_NOT_CARE;
            *pFeatureParaLen=4;
            break;
        case SENSOR_FEATURE_SET_VIDEO_MODE:
            A5142MIPISetVideoMode(*pFeatureData16);
            break;
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            A5142MIPIGetSensorID(pFeatureReturnPara32); 
            break; 
        case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
            A5142MIPISetAutoFlickerMode((BOOL) *pFeatureData16, *(pFeatureData16 + 1));
            break; 
        case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
            A5142MIPISetMaxFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32, *(pFeatureData32+1));
            break;
        case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
            A5142MIPIGetDefaultFramerateByScenario((MSDK_SCENARIO_ID_ENUM)*pFeatureData32, (MUINT32 *)(*(pFeatureData32+1)));
            break;      

        case SENSOR_FEATURE_SET_TEST_PATTERN:
            A5142MIPI_SetTestPatternMode((BOOL)*pFeatureData16);
             break;
        case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE://                                          
            *pFeatureReturnPara32= A5142MIPI_TEST_PATTERN_CHECKSUM;
            *pFeatureParaLen=4;                             
             break;  
        default:
            break;
    }
    
    return ERROR_NONE;
}   /*                           */


SENSOR_FUNCTION_STRUCT  SensorFuncA5142MIPI=
{
    A5142MIPIOpen,
    A5142MIPIGetInfo,
    A5142MIPIGetResolution,
    A5142MIPIFeatureControl,
    A5142MIPIControl,
    A5142MIPIClose
};

UINT32 A5142_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
    /*                                  */
    if (pfFunc!=NULL)
        *pfFunc=&SensorFuncA5142MIPI;

    return ERROR_NONE;
}/*              */

