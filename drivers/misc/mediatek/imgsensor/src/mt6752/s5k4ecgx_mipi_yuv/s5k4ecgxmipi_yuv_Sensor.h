/*                                                                            
  
            
            
             
  
           
           
         
  
               
               
                                 
  
  
          
          
                        
  
                                                                              
                      
                                                                     
                                                                                
              
             
         
  
  
  
                                                                                
                                                                     
                                                                              
                                                                            */
/*                  */
#ifndef _4EC_SENSOR_H
#define _4EC_SENSOR_H

//                         
#define MIPI_INTERFACE
//                 
//                               



typedef enum S5K4ECGX_CAMCO_MODE
{
    S5K4ECGX_CAM_PREVIEW=0,//              
    S5K4ECGX_CAM_CAPTURE,//              
    S5K4ECGX_VIDEO_MPEG4,//          
    S5K4ECGX_VIDEO_MJPEG,
    S5K4ECGX_WEBCAM_CAPTURE,//      
    S5K4ECGX_VIDEO_MAX
} S5K4ECGX_Camco_MODE;

#define S5K4ECGX_WRITE_ID                      0xAC//                             
#define S5K4ECGX_READ_ID                       0xAD//                      
#define S5K4ECGX2_WRITE_ID                     0x78
#define S5K4ECGX2_READ_ID                      0x79
#define S5K4ECGX_SENSOR_ID                     0x4EC0

/*                     */
#define S5K4ECGX_IMAGE_SENSOR_FULL_WIDTH_DRV   1280//    
#define S5K4ECGX_IMAGE_SENSOR_FULL_HEIGHT_DRV  960//   
#define S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV     1280
#define S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV    960
#define S5K4ECGX_IMAGE_SENSOR_PV_WIDTH         (S5K4ECGX_IMAGE_SENSOR_PV_WIDTH_DRV)
#define S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT        (S5K4ECGX_IMAGE_SENSOR_PV_HEIGHT_DRV) /*                         */

#define S5K4ECGX_IMAGE_SENSOR_VDO_WIDTH_DRV     1280
#define S5K4ECGX_IMAGE_SENSOR_VDO_HEIGHT_DRV    960

/*                           */
#if !defined(__CAPTURE_JPEG_OUTPUT__)
#define S5K4ECGX_PV_X_START                     0    //  
#define S5K4ECGX_PV_Y_START                     1    //  
#define S5K4ECGX_FULL_X_START                   0
#define S5K4ECGX_FULL_Y_START                   1
#else
#define S5K4ECGX_PV_X_START                     0    
#define S5K4ECGX_PV_Y_START                     0    
#define S5K4ECGX_FULL_X_START                   0
#define S5K4ECGX_FULL_Y_START                   0
#endif

#define S5K4ECGX_MCLK                           24


//         
#define CAM_SIZE_QVGA_WIDTH   320
#define CAM_SIZE_QVGA_HEIGHT  240
#define CAM_SIZE_VGA_WIDTH    640
#define CAM_SIZE_VGA_HEIGHT   480
#define CAM_SIZE_05M_WIDTH    800
#define CAM_SIZE_05M_HEIGHT   600
#define CAM_SIZE_1M_WIDTH     1280
#define CAM_SIZE_1M_HEIGHT    960
#define CAM_SIZE_2M_WIDTH     1600
#define CAM_SIZE_2M_HEIGHT    1200
#define CAM_SIZE_3M_WIDTH     2048
#define CAM_SIZE_3M_HEIGHT    1536
#define CAM_SIZE_5M_WIDTH     2560
#define CAM_SIZE_5M_HEIGHT    1920

//                                                                 
//          
typedef enum
{
    S5K4ECGX_AF_MODE_SINGLE,
    S5K4ECGX_AF_MODE_CONTINUOUS,
    S5K4ECGX_AF_MODE_RSVD,
}S5K4ECGX_AF_MODE_ENUM;

typedef enum
{
    S5K4ECGX_AAA_AF_STATUS_OK,
    S5K4ECGX_AAA_AF_STATUS_1ST_SEARCH_FAIL,
    S5K4ECGX_AAA_AF_STATUS_2ND_SEARCH_FAIL,
    S5K4ECGX_AAA_AF_STATUS_BAD_PARA,
    S5K4ECGX_AAA_AF_STATUS_BAD_RSVD,
}S5K4ECGX_AAA_STATUS_ENUM;


typedef enum
{
    S5K4ECGX_AF_STATE_UNINIT, // 
    //                            
    S5K4ECGX_AF_STATE_IDLE,  // 

    S5K4ECGX_AF_STATE_1ST_SEARCHING, //           
    //                                             

    S5K4ECGX_AF_STATE_2ND_SEARCHING, //          
    //                                            

    S5K4ECGX_AF_STATE_ERROR, // 
    S5K4ECGX_AF_STATE_CANCEL, // 

    S5K4ECGX_AF_STATE_DONE, //          


    S5K4ECGX_AF_STATE_ENTERING, //
    S5K4ECGX_AF_STATE_ENTERED, //

    S5K4ECGX_AF_STATE_BAD_RSVD,
} S5K4ECGX_AF_STATE_ENUM;


typedef enum
{
    S5K4ECGX_AE_STATE_LOCK,
    S5K4ECGX_AE_STATE_UNLOCK,
    S5K4ECGX_AE_STATE_BAD_RSVD,
} S5K4ECGX_AE_STATE_ENUM;



typedef struct
{
   unsigned int inWx; //                   
   unsigned int inWy; //                    
   unsigned int inWw; //                   
   unsigned int inWh; //                    

   unsigned int outWx; //                   
   unsigned int outWy; //                    
   unsigned int outWw; //                   
   unsigned int outWh; //                    
}S5K4ECGX_MIPI_AF_WIN_T;


#define S5K4ECGX_MIPI_AF_CALLER_WINDOW_WIDTH  320
#define S5K4ECGX_MIPI_AF_CALLER_WINDOW_HEIGHT 240

//                                                        




typedef enum {
    SENSOR_MODE_INIT = 0,
    SENSOR_MODE_PREVIEW,
    SENSOR_MODE_CAPTURE,
    SENSOR_MODE_VIDEO
} S5K4ECGX_MIPI_SENSOR_MODE;



struct S5K4ECGX_MIPI_sensor_struct
{
    kal_uint16 sensor_id;

    kal_uint32 Preview_PClk;

    //                                  
    //                                  
    //                                  
    //                                  
    kal_uint32 Preview_Width;
    kal_uint32 Preview_Height;

    kal_uint16 Preview_Shutter;
    kal_uint16 Capture_Shutter;

    kal_uint16 StartX;
    kal_uint16 StartY;
    kal_uint16 iGrabWidth;
    kal_uint16 iGrabheight;

    kal_uint16 Capture_Size_Width;
    kal_uint16 Capture_Size_Height;
    kal_uint32 Digital_Zoom_Factor;

    kal_uint16 Max_Zoom_Factor;

    kal_uint32 Min_Frame_Rate;
    kal_uint32 Max_Frame_Rate;
    kal_uint32 Fixed_Frame_Rate;
    S5K4ECGX_Camco_MODE Camco_mode;
    AE_FLICKER_MODE_T Banding;

    kal_bool Night_Mode;

    S5K4ECGX_MIPI_SENSOR_MODE sensorMode;
    kal_uint32 Period_PixelNum;
    kal_uint32 Period_LineNum;

    kal_uint16 Dummy_Pixels;
    kal_uint16 Dummy_Lines;

    kal_uint32 shutter;
    kal_uint32 sensorGain;

    kal_bool   aeEnable;
    kal_bool   awbEnable;


    //          
    S5K4ECGX_MIPI_AF_WIN_T  afWindows;
    S5K4ECGX_MIPI_AF_WIN_T  orignalAfWindows;
    unsigned int            prevAfWinWidth;
    unsigned int            prevAfWinHeight;

    kal_bool                afStateOnOriginalSet; //                               
    kal_bool                aeStateOnOriginalSet; //                               

    S5K4ECGX_AF_MODE_ENUM   afMode;
    S5K4ECGX_AF_STATE_ENUM  afState;
    S5K4ECGX_AF_MODE_ENUM   afPrevMode;


    S5K4ECGX_AE_STATE_ENUM  aeState;
    unsigned int            aeWindows[4];
    unsigned int            apAEWindows[6];

    kal_uint32              videoFrameRate;
    unsigned char           ae_table[64]; //               
    unsigned int            mapAEWindows[4]; //      

    //                   
    kal_bool                userAskAeLock;
    kal_bool                userAskAwbLock;

    kal_uint32              sceneMode;

    //                                        
    kal_uint32              currentExposureTime;
    kal_uint32              currentShutter;
    kal_uint32              currentAxDGain;
    kal_bool                manualAEStart;

    //             
    unsigned char           isoSpeed;
    unsigned char           awbMode;
    unsigned int            capExposureTime;

    ACDK_SENSOR_JPEG_OUTPUT_PARA jpegSensorPara;
    ACDK_SENSOR_JPEG_INFO        jpegSensorInfo;
    unsigned int                 jpegSensorHDRDelayFrmCnt;


};


typedef struct
{
    UINT16  iSensorVersion;
    UINT16  iNightMode;
    UINT16  iWB;
    UINT16  iEffect;
    UINT16  iEV;
    UINT16  iBanding;
    UINT16  iMirror;
    UINT16  iFrameRate;
} S5K4ECYX_MIPIStatus;
S5K4ECYX_MIPIStatus S5K4ECYX_MIPICurrentStatus;


#endif /*            */

