/*                                                                                         */


/*                                                                                         */

/*                  */
#ifndef __SENSOR_H
#define __SENSOR_H

//                                 
#define FULL_SIZE_30_FPS
//                                                                     

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

typedef enum {
    SENSOR_MODE_INIT = 0,
    SENSOR_MODE_PREVIEW,
    SENSOR_MODE_VIDEO,
    SENSOR_MODE_VIDEO_NIGHT,
    SENSOR_MODE_SMALL_SIZE_END=SENSOR_MODE_VIDEO_NIGHT,
    SENSOR_MODE_ZSD_PREVIEW,
    SENSOR_MODE_CAPTURE,
    SENSOR_MODE_FULL_SIZE_END=SENSOR_MODE_CAPTURE
} S5K3H7Y_SENSOR_MODE;

typedef struct
{
	kal_uint32 DummyPixels;
	kal_uint32 DummyLines;
	
	kal_uint32 pvShutter;
	kal_uint32 pvGain;
	
	kal_uint32 pvPclk;  //                  
	kal_uint32 capPclk; //    
	kal_uint32 m_vidPclk;
	
	kal_uint32 shutter;
	kal_uint32 maxExposureLines;
	kal_uint16 sensorGain; 
	kal_uint16 sensorBaseGain; 
	kal_int16 imgMirror;

	S5K3H7Y_SENSOR_MODE sensorMode;

	kal_bool S5K3H7YAutoFlickerMode;
	kal_bool S5K3H7YVideoMode;
	kal_uint32 FixedFrameLength;
	
}S5K3H7Y_PARA_STRUCT,*PS5K3H7Y_PARA_STRUCT;

	//                                                                   
	//       
	#ifdef USE_MIPI_2_LANES
	#define S5K3H7Y_PV_PERIOD_PIXEL_NUMS					(0x0E68*2) 	//            
	#else
	#define S5K3H7Y_PV_PERIOD_PIXEL_NUMS					(3688) //                        
	#endif
	#define S5K3H7Y_PV_PERIOD_LINE_NUMS 					(2530) //                                    
	
	//     
	#define S5K3H7Y_VIDEO_PERIOD_PIXEL_NUMS					(S5K3H7Y_PV_PERIOD_PIXEL_NUMS) 	//            
	#define S5K3H7Y_VIDEO_PERIOD_LINE_NUMS					(2530) //                         
	
	//       
	#define S5K3H7Y_FULL_PERIOD_PIXEL_NUMS					(S5K3H7Y_PV_PERIOD_PIXEL_NUMS)	//           
	#ifdef FULL_SIZE_30_FPS
	#define S5K3H7Y_FULL_PERIOD_LINE_NUMS					(2530) //                      
	#else		
	#define S5K3H7Y_FULL_PERIOD_LINE_NUMS					(0x0C22)	//                         
	#endif
	
	//   
	#define S5K3H7Y_ZSD_PERIOD_PIXEL_NUMS					S5K3H7Y_FULL_PERIOD_PIXEL_NUMS
	#define S5K3H7Y_ZSD_PERIOD_LINE_NUMS					S5K3H7Y_FULL_PERIOD_LINE_NUMS
	//                                                                   

	//                                                      
	#define S5K3H7Y_IMAGE_SENSOR_PV_WIDTH					(1600)
	#define S5K3H7Y_IMAGE_SENSOR_PV_HEIGHT					(1200)

	#define S5K3H7Y_IMAGE_SENSOR_VIDEO_WIDTH_SETTING 		(3264)
	#define S5K3H7Y_IMAGE_SENSOR_VIDEO_HEIGHT_SETTING		(1836)
	#define S5K3H7Y_IMAGE_SENSOR_VIDEO_WIDTH 				(S5K3H7Y_IMAGE_SENSOR_VIDEO_WIDTH_SETTING-64) //         
	#define S5K3H7Y_IMAGE_SENSOR_VIDEO_HEIGHT 				(S5K3H7Y_IMAGE_SENSOR_VIDEO_HEIGHT_SETTING-36) //          
	
	#ifdef CAPTURE_USE_VIDEO_SETTING
	#define S5K3H7Y_IMAGE_SENSOR_FULL_WIDTH 				(S5K3H7Y_IMAGE_SENSOR_VIDEO_WIDTH)	
	#define S5K3H7Y_IMAGE_SENSOR_FULL_HEIGHT				(S5K3H7Y_IMAGE_SENSOR_VIDEO_HEIGHT)
	#else
	#define S5K3H7Y_IMAGE_SENSOR_FULL_WIDTH 				(3264-64)	
	#define S5K3H7Y_IMAGE_SENSOR_FULL_HEIGHT				(2448-48)
	#endif

	#define S5K3H7Y_IMAGE_SENSOR_ZSD_WIDTH					S5K3H7Y_IMAGE_SENSOR_FULL_WIDTH	
	#define S5K3H7Y_IMAGE_SENSOR_ZSD_HEIGHT					S5K3H7Y_IMAGE_SENSOR_FULL_HEIGHT
	//                                                         


	/*                           */         	
	#define S5K3H7Y_FULL_X_START						    		(2)
	#define S5K3H7Y_FULL_Y_START						    		(2+1)
	#define S5K3H7Y_FULL_X_END						        	(3264) 
	#define S5K3H7Y_FULL_Y_END						        	(2448) 

	#define S5K3H7Y_ZSD_X_START								S5K3H7Y_FULL_X_START
	#define S5K3H7Y_ZSD_Y_START								S5K3H7Y_FULL_Y_START	
	#define S5K3H7Y_ZSD_X_END								S5K3H7Y_FULL_X_END	
	#define S5K3H7Y_ZSD_Y_END								S5K3H7Y_FULL_Y_END
	
	#define S5K3H7Y_PV_X_START								(2)
	#define S5K3H7Y_PV_Y_START								(2+1)	
	#define S5K3H7Y_PV_X_END								(1600) 
	#define S5K3H7Y_PV_Y_END								(1200) 

	#define S5K3H7Y_VIDEO_X_START								(2)
	#define S5K3H7Y_VIDEO_Y_START								(2+1)	
	#define S5K3H7Y_VIDEO_X_END								(1600) 
	#define S5K3H7Y_VIDEO_Y_END								(1200) 


	#define S5K3H7Y_MAX_ANALOG_GAIN					(16)
	#define S5K3H7Y_MIN_ANALOG_GAIN					(1)

	#define S5K3H7Y_MIN_LINE_LENGTH						(0x0E68)	//    
	#define S5K3H7Y_MIN_FRAME_LENGTH					(0x0A0F)	//   
	
	#define S5K3H7Y_MAX_LINE_LENGTH						0xCCCC
	#define S5K3H7Y_MAX_FRAME_LENGTH						0xFFFF

	/*                            */
	/*                                */
	#define S5K3H7Y_IMAGE_SENSOR_PV_INSERTED_PIXELS			2	//               
	#define S5K3H7Y_IMAGE_SENSOR_PV_INSERTED_LINES			2

	#define S5K3H7Y_IMAGE_SENSOR_FULL_INSERTED_PIXELS		4
	#define S5K3H7Y_IMAGE_SENSOR_FULL_INSERTED_LINES		4


#define S5K3H7YMIPI_WRITE_ID 	(0x20)
#define S5K3H7YMIPI_READ_ID	(0x21)

#define S5K3H7YMIPI_WRITE_ID2 	(0x5A)
#define S5K3H7YMIPI_READ_ID2	    (0x5B)

#define S5K3H7YMIPI_SENSOR_ID            S5K3H7Y_SENSOR_ID

//                
UINT32 S5K3H7YMIPIOpen(void);
UINT32 S5K3H7YMIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
UINT32 S5K3H7YMIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 S5K3H7YMIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 S5K3H7YMIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
UINT32 S5K3H7YMIPIClose(void);


#endif /*            */

