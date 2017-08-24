/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
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
/*                  */
#ifndef __SENSOR_H
#define __SENSOR_H

typedef enum group_enum {
    PRE_GAIN=0,
    CMMCLK_CURRENT,
    FRAME_RATE_LIMITATION,
    REGISTER_EDITOR,
    GROUP_TOTAL_NUMS
} FACTORY_GROUP_ENUM;


#define ENGINEER_START_ADDR 10
#define FACTORY_START_ADDR 0

typedef enum engineer_index
{
    CMMCLK_CURRENT_INDEX=ENGINEER_START_ADDR,
    ENGINEER_END
} FACTORY_ENGINEER_INDEX;
typedef enum register_index
{
	SENSOR_BASEGAIN=FACTORY_START_ADDR,
	PRE_GAIN_R_INDEX,
	PRE_GAIN_Gr_INDEX,
	PRE_GAIN_Gb_INDEX,
	PRE_GAIN_B_INDEX,
	FACTORY_END_ADDR
} FACTORY_REGISTER_INDEX;



typedef struct
{
    SENSOR_REG_STRUCT	Reg[ENGINEER_END];
    SENSOR_REG_STRUCT	CCT[FACTORY_END_ADDR];
} SENSOR_DATA_STRUCT, *PSENSOR_DATA_STRUCT;


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
#define IMAGE_SENSOR_H_DECIMATION				2	//                                                                            
#define IMAGE_SENSOR_V_DECIMATION				2	//                                                                          

/*                                                                                      */



#define IMX135MIPI_REAL_PV_WIDTH				(2104-8)//                                               
#define IMX135MIPI_REAL_PV_HEIGHT				(1560-8)//                                              
/*                                                                                       */
#define IMX135MIPI_REAL_CAP_WIDTH				(4208-16)//               
#define IMX135MIPI_REAL_CAP_HEIGHT				(3120-16)//               
#define IMX135MIPI_REAL_VIDEO_WIDTH				(2104-8)//                                                          
#define IMX135MIPI_REAL_VIDEO_HEIGHT			(1560-8)//                                                         





/*                    */
#define IMX135MIPI_IMAGE_SENSOR_PV_STARTX       3
#define IMX135MIPI_IMAGE_SENSOR_PV_STARTY       3	//                                       
#define IMX135MIPI_IMAGE_SENSOR_CAP_STARTX		(IMX135MIPI_IMAGE_SENSOR_PV_STARTX * IMAGE_SENSOR_H_DECIMATION+1)
#define IMX135MIPI_IMAGE_SENSOR_CAP_STARTY		(IMX135MIPI_IMAGE_SENSOR_PV_STARTY * IMAGE_SENSOR_V_DECIMATION+1)		//                                       
#define IMX135MIPI_IMAGE_SENSOR_VIDEO_STARTX       3
#define IMX135MIPI_IMAGE_SENSOR_VIDEO_STARTY       3	//                                       

/*                */
#define IMX135MIPI_IMAGE_SENSOR_PV_WIDTH		(IMX135MIPI_REAL_PV_WIDTH  + 2 * IMX135MIPI_IMAGE_SENSOR_PV_STARTX)	//                                                                                                                        
#define IMX135MIPI_IMAGE_SENSOR_PV_HEIGHT		(IMX135MIPI_REAL_PV_HEIGHT + 2 * IMX135MIPI_IMAGE_SENSOR_PV_STARTY)	//                                                                                                                      
/*                */
#define IMX135MIPI_IMAGE_SENSOR_FULL_WIDTH		(IMX135MIPI_REAL_CAP_WIDTH  + 2 * IMX135MIPI_IMAGE_SENSOR_CAP_STARTX)	//                                                                                             
#define IMX135MIPI_IMAGE_SENSOR_FULL_HEIGHT		(IMX135MIPI_REAL_CAP_HEIGHT + 2 * IMX135MIPI_IMAGE_SENSOR_CAP_STARTY)	//                                                                                              


#define IMX135MIPI_IMAGE_SENSOR_VIDEO_WIDTH		(IMX135MIPI_REAL_VIDEO_WIDTH  + 2 * IMX135MIPI_IMAGE_SENSOR_VIDEO_STARTX)	//                                                                                             
#define IMX135MIPI_IMAGE_SENSOR_VIDEO_HEIGHT	(IMX135MIPI_REAL_VIDEO_HEIGHT + 2 * IMX135MIPI_IMAGE_SENSOR_VIDEO_STARTY)	//                                                                                              

/*                                         */
#define IMX135MIPI_PV_LINE_LENGTH_PIXELS 						(4572)
#define IMX135MIPI_PV_FRAME_LENGTH_LINES						(1640)//                               
#define IMX135MIPI_FULL_LINE_LENGTH_PIXELS 						(5456)//              
#define IMX135MIPI_FULL_FRAME_LENGTH_LINES			            (3146)//              
#define IMX135MIPI_VIDEO_LINE_LENGTH_PIXELS 					(4572)
#define IMX135MIPI_VIDEO_FRAME_LENGTH_LINES						(1640)	


#define IMX135MIPI_WRITE_ID (0x20)
#define IMX135MIPI_READ_ID	(0x21)


#define IMAGE_SENSOR_PV_WIDTH		IMX135MIPI_IMAGE_SENSOR_PV_WIDTH
#define IMAGE_SENSOR_PV_HEIGHT		IMX135MIPI_IMAGE_SENSOR_PV_HEIGHT

#define IMAGE_SENSOR_FULL_WIDTH		IMX135MIPI_IMAGE_SENSOR_FULL_WIDTH
#define IMAGE_SENSOR_FULL_HEIGHT	IMX135MIPI_IMAGE_SENSOR_FULL_HEIGHT

#define IMAGE_SENSOR_VIDEO_WIDTH		IMX135MIPI_IMAGE_SENSOR_VIDEO_WIDTH
#define IMAGE_SENSOR_VIDEO_HEIGHT	IMX135MIPI_IMAGE_SENSOR_VIDEO_HEIGHT



/*                       */
struct IMX135_SENSOR_STRUCT
{
	kal_uint8 i2c_write_id;
	kal_uint8 i2c_read_id;

};



struct IMX135MIPI_sensor_STRUCT
	{	 
		  kal_uint16 i2c_write_id;
		  kal_uint16 i2c_read_id;
		  kal_bool first_init;
		  kal_bool fix_video_fps;
		  kal_bool pv_mode; 
		  kal_bool video_mode; 				
		  kal_bool capture_mode; 				//                                       
		  kal_bool is_zsd;
  	      kal_bool is_zsd_cap;
		  kal_bool night_mode;				//                                  
		  kal_uint8 mirror_flip;
		  kal_uint32 pv_pclk;				//            
		  kal_uint32 video_pclk;				//          
		  kal_uint32 cp_pclk;				//            
		  kal_uint32 pv_shutter;		   
		  kal_uint32 video_shutter;		   
		  kal_uint32 cp_shutter;
		  kal_uint32 pv_gain;
		  kal_uint32 video_gain;
		  kal_uint32 cp_gain;
		  kal_uint32 pv_line_length;
		  kal_uint32 pv_frame_length;
		  kal_uint32 video_line_length;
		  kal_uint32 video_frame_length;
		  kal_uint32 cp_line_length;
		  kal_uint32 cp_frame_length;
		  kal_uint16 pv_dummy_pixels;		   //                        
		  kal_uint16 pv_dummy_lines;		   //           
		  kal_uint16 video_dummy_pixels;		   //                        
		  kal_uint16 video_dummy_lines;		   //           
		  kal_uint16 cp_dummy_pixels;		   //                        
		  kal_uint16 cp_dummy_lines;		   //              
		  kal_uint16 video_current_frame_rate;
	};

//                    

#define IMX135MIPI_SENSOR_ID            IMX135_SENSOR_ID

#define IMX135MIPI_PAGE_SETTING_REG    (0xFF)

//                 
//                 
//                 

//                
UINT32 IMX135MIPIOpen(void);
UINT32 IMX135MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
UINT32 IMX135MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 IMX135MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 IMX135MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
UINT32 IMX135MIPIClose(void);

//                            
//                        
//            

//                 
//                 
//                 

#endif /*            */

