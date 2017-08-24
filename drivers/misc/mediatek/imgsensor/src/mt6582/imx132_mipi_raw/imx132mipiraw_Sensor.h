/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*                                                                            
  
            
            
             
  
           
           
         
  
               
               
                            
  
                                                                            */
#ifndef _IMX132MIPI_SENSOR_H
#define _IMX132MIPI_SENSOR_H

#define IMX132MIPI_DEBUG
#define IMX132MIPI_DRIVER_TRACE
//                              
#ifdef IMX132MIPI_DEBUG
//                       
#else
//                       
#endif

#define IMX132MIPI_FACTORY_START_ADDR 0
#define IMX132MIPI_ENGINEER_START_ADDR 10

//                      

 
typedef enum IMX132MIPI_group_enum
{
  IMX132MIPI_PRE_GAIN = 0,
  IMX132MIPI_CMMCLK_CURRENT,
  IMX132MIPI_FRAME_RATE_LIMITATION,
  IMX132MIPI_REGISTER_EDITOR,
  IMX132MIPI_GROUP_TOTAL_NUMS
} IMX132MIPI_FACTORY_GROUP_ENUM;

typedef enum IMX132MIPI_register_index
{
  IMX132MIPI_SENSOR_BASEGAIN = IMX132MIPI_FACTORY_START_ADDR,
  IMX132MIPI_PRE_GAIN_R_INDEX,
  IMX132MIPI_PRE_GAIN_Gr_INDEX,
  IMX132MIPI_PRE_GAIN_Gb_INDEX,
  IMX132MIPI_PRE_GAIN_B_INDEX,
  IMX132MIPI_FACTORY_END_ADDR
} IMX132MIPI_FACTORY_REGISTER_INDEX;

typedef enum IMX132MIPI_engineer_index
{
  IMX132MIPI_CMMCLK_CURRENT_INDEX = IMX132MIPI_ENGINEER_START_ADDR,
  IMX132MIPI_ENGINEER_END
} IMX132MIPI_FACTORY_ENGINEER_INDEX;

typedef struct _sensor_data_struct
{
  SENSOR_REG_STRUCT reg[IMX132MIPI_ENGINEER_END];
  SENSOR_REG_STRUCT cct[IMX132MIPI_FACTORY_END_ADDR];
} sensor_data_struct;
typedef enum {
    IMX132MIPI_SENSOR_MODE_INIT = 0,
    IMX132MIPI_SENSOR_MODE_PREVIEW,
    IMX132MIPI_SENSOR_MODE_VIDEO,
    IMX132MIPI_SENSOR_MODE_CAPTURE
} IMX132MIPI_SENSOR_MODE;


/*                                 */
//                                                                       
//                                                                       

#define IMX132MIPI_COLOR_FORMAT                    SENSOR_OUTPUT_FORMAT_RAW_R

#define IMX132MIPI_MIN_ANALOG_GAIN				1	/*    */
#define IMX132MIPI_MAX_ANALOG_GAIN				8	/*     */


/*                 */
#define IMX132MIPI_FPS(x)                          (10 * (x))

/*                                         */
//                                                                   

//                                
//                                                                                  
//                        

#if 1
#define IMX132MIPI_FULL_PERIOD_PIXEL_NUMS          2248  //                
#define IMX132MIPI_FULL_PERIOD_LINE_NUMS           1192//     
//                                                                
#define IMX132MIPI_PV_PERIOD_PIXEL_NUMS            2248 //                 
#define IMX132MIPI_PV_PERIOD_LINE_NUMS             1192//          
#define IMX132MIPI_VIDEO_PERIOD_PIXEL_NUMS         2248 //                 
#define IMX132MIPI_VIDEO_PERIOD_LINE_NUMS          1192//          
#else


#define IMX132MIPI_FULL_PERIOD_PIXEL_NUMS          2248  //                
#define IMX132MIPI_FULL_PERIOD_LINE_NUMS           1124//                          
#define IMX132MIPI_PV_PERIOD_PIXEL_NUMS            2248 //                 
#define IMX132MIPI_PV_PERIOD_LINE_NUMS             1124//          
#define IMX132MIPI_VIDEO_PERIOD_PIXEL_NUMS         2248 //                 
#define IMX132MIPI_VIDEO_PERIOD_LINE_NUMS          1124//          
#endif


/*                           */
#if 0
#define IMX132MIPI_FULL_X_START                    2
#define IMX132MIPI_FULL_Y_START                    2
#define IMX132MIPI_IMAGE_SENSOR_FULL_WIDTH         (1936 - 16) /*      */
#define IMX132MIPI_IMAGE_SENSOR_FULL_HEIGHT        (1090 - 10) /*      */
#define IMX132MIPI_PV_X_START                      2
#define IMX132MIPI_PV_Y_START                      2
#define IMX132MIPI_IMAGE_SENSOR_PV_WIDTH           (1936 - 16)  //                          
#define IMX132MIPI_IMAGE_SENSOR_PV_HEIGHT          (1090 - 10)  //                    

#define IMX132MIPI_VIDEO_X_START                   2
#define IMX132MIPI_VIDEO_Y_START                   2
#define IMX132MIPI_IMAGE_SENSOR_VIDEO_WIDTH        (1936 - 16)//                      
#define IMX132MIPI_IMAGE_SENSOR_VIDEO_HEIGHT       (1090 - 10)//                      
#else

#define IMX132MIPI_FULL_X_START                    2
#define IMX132MIPI_FULL_Y_START                    2
#define IMX132MIPI_IMAGE_SENSOR_FULL_WIDTH         (1948 - 28) /*      */
#define IMX132MIPI_IMAGE_SENSOR_FULL_HEIGHT        (1108 - 12) /*      */
#define IMX132MIPI_PV_X_START                      2
#define IMX132MIPI_PV_Y_START                      2
#define IMX132MIPI_IMAGE_SENSOR_PV_WIDTH           (1948 - 28)  //                          
#define IMX132MIPI_IMAGE_SENSOR_PV_HEIGHT          (1108 - 12)  //                    

#define IMX132MIPI_VIDEO_X_START                   2
#define IMX132MIPI_VIDEO_Y_START                   2
#define IMX132MIPI_IMAGE_SENSOR_VIDEO_WIDTH        (1948 - 28)//                      
#define IMX132MIPI_IMAGE_SENSOR_VIDEO_HEIGHT       (1108 - 12)//                      

#endif

/*                      */
#define IMX132MIPI_SLAVE_WRITE_ID_1   (0x6c)
#define IMX132MIPI_SLAVE_WRITE_ID_2   (0x20)

#define IMX132MIPI_WRITE_ID   (0x20)  //      
#define IMX132MIPI_READ_ID    (0x21)  //      

/*           */
//                                          


//                 
//                          

#if defined(IMX132MIPI_USE_OTP)

struct imx132mipi_otp_struct
{
    kal_uint16 customer_id;
	kal_uint16 module_integrator_id;
	kal_uint16 lens_id;
	kal_uint16 rg_ratio;
	kal_uint16 bg_ratio;
	kal_uint16 user_data[5];
	kal_uint16 lenc[63];

};

#define RG_TYPICAL 0x51
#define BG_TYPICAL 0x57


#endif




/*                       */
typedef struct IMX132MIPI_sensor_STRUCT
{
  MSDK_SENSOR_CONFIG_STRUCT cfg_data;
  sensor_data_struct eng; /*               */
  MSDK_SENSOR_ENG_INFO_STRUCT eng_info;
  kal_uint8 mirror;
  kal_bool pv_mode;
  kal_bool video_mode;
  kal_bool is_zsd;
  kal_bool is_zsd_cap;
  kal_bool is_autofliker;
  kal_uint16 normal_fps; /*                           */
  kal_uint16 night_fps; /*                          */
  kal_uint16 shutter;
  kal_uint16 gain;
  kal_uint32 pv_pclk;
  kal_uint32 cap_pclk;
  kal_uint32 video_pclk;
  kal_uint32 pclk;
  kal_uint16 frame_length;
  kal_uint16 line_length;  
  kal_uint16 write_id;
  kal_uint16 read_id;
  kal_uint16 dummy_pixels;
  kal_uint16 dummy_lines;
  kal_uint32 max_exposure_lines;
  
  IMX132MIPI_SENSOR_MODE sensorMode;
} IMX132MIPI_sensor_struct;

//                
UINT32 IMX132MIPIOpen(void);
UINT32 IMX132MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 IMX132MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
UINT32 IMX132MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 IMX132MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
UINT32 IMX132MIPIClose(void);

#define Sleep(ms) mdelay(ms)

#endif 
