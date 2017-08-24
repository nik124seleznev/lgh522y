/*                                                                            
  
            
            
             
  
           
           
         
  
               
               
                                 
  
  
          
          
            
  
                                                                              
                      
                                                                     
                                                                                
              
             
         
  
                     
                         
                 
                                                                                
                                                                             
                                                                              
                                                                            */
/*                  */
#ifndef __SENSOR_H
#define __SENSOR_H


typedef enum _GC2035_OP_TYPE_ {
        GC2035_MODE_NONE,
        GC2035_MODE_PREVIEW,
        GC2035_MODE_CAPTURE,
        GC2035_MODE_QCIF_VIDEO,
        GC2035_MODE_CIF_VIDEO,
        GC2035_MODE_QVGA_VIDEO
    } GC2035_OP_TYPE;

extern GC2035_OP_TYPE GC2035_g_iGC2035_Mode;

/*                         */
#define IMAGE_SENSOR_START_GRAB_X		        2 // 
#define IMAGE_SENSOR_START_GRAB_Y		        2 // 

/*                                         */
#define GC2035_FULL_PERIOD_PIXEL_NUMS  (1940)  //                                              
#define GC2035_FULL_PERIOD_LINE_NUMS   (1238)  //                                            
#define GC2035_PV_PERIOD_PIXEL_NUMS    (970)  //                                              
#define GC2035_PV_PERIOD_LINE_NUMS     (670)   //                                            

/*                                 */
#define GC2035_FULL_EXPOSURE_LIMITATION    (1236)
#define GC2035_PV_EXPOSURE_LIMITATION      (671)

/*                  */
#define GC2035_IMAGE_SENSOR_FULL_WIDTH	   (1600)
#define GC2035_IMAGE_SENSOR_FULL_HEIGHT	  (1200)

/*                */
#define GC2035_IMAGE_SENSOR_PV_WIDTH   (800) //   
#define GC2035_IMAGE_SENSOR_PV_HEIGHT  (600) //

#define GC2035_VIDEO_QCIF_WIDTH   (176)
#define GC2035_VIDEO_QCIF_HEIGHT  (144)

/*                      */
#define GC2035_WRITE_ID	0x78
#define GC2035_READ_ID		0x79


/*                     */
//                                     


//                 
//                 
//                 

//                
UINT32 GC2035Open(void);
UINT32 GC2035GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
UINT32 GC2035GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 GC2035Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 GC2035FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
UINT32 GC2035Close(void);


//                 
//                 
//                 


#endif /*            */
