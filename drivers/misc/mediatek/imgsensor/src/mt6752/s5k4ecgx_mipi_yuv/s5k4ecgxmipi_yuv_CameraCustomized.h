/*                                                                            
  
            
            
             
  
           
           
         
  
               
               
                                                 
  
  
          
          
                        
  
                                                                              
                      
                                                                     
                                                                                
              
             
         
  
    
                                      
  
  
                                                                                
                                                                     
                                                                              
                                                                            */

#ifndef __CAMERA_CUSTOMIZED_H
#define __CAMERA_CUSTOMIZED_H

//                                                                                         
#define MAIN_SENSOR_ORIENTATION_ANGLE	90
#define SUB_SENSOR_ORIENTATION_ANGLE	0		//                                             


//                                                  
//                                                
//                                       
//                                                                                       
#define MAIN_SENSOR_TO_PHONE_ANGLE	90
#define SUB_SENSOR_TO_PHONE_ANGLE	0


#define CAM_SIZE_QVGA_WIDTH 	320
#define CAM_SIZE_QVGA_HEIGHT 	240
#define CAM_SIZE_VGA_WIDTH 		640
#define CAM_SIZE_VGA_HEIGHT 	480
/*
                                
                                
                                
                                
                                
                                 
                                
                                 
                                
                                 
*/
//                
#define MAIN_NUM_OF_PREVIEW_RESOLUTION 3
#define MAIN_NUM_OF_VIDEO_RESOLUTION 3
#define MAIN_NUM_OF_STILL_RESOLUTION 2
#define MAIN_VIDEO_RESOLUTION_PROFILE		{{176,144},{320,240},{640,480}/*          */}
#define MAIN_PREVIEW_RESOLUTION_PROFILE	{{232,174},{320,240},{240,320}}
#define MAIN_STILL_RESOLUTION_PROFILE		{{CAM_SIZE_QVGA_WIDTH,CAM_SIZE_QVGA_HEIGHT}, \
									 {CAM_SIZE_VGA_WIDTH,CAM_SIZE_VGA_HEIGHT}, \
									/*                                            
                                                   
                                                   
                                                   
                                                */}

//                                                          
#define SUB_NUM_OF_PREVIEW_RESOLUTION 0
#define SUB_NUM_OF_VIDEO_RESOLUTION 0
#define SUB_NUM_OF_STILL_RESOLUTION 0
#define SUB_VIDEO_RESOLUTION_PROFILE		{{0,0}}
#define SUB_PREVIEW_RESOLUTION_PROFILE		{{0,0}}
#define SUB_STILL_RESOLUTION_PROFILE		{{0,0}}


#define NUM_OF_VIDEO_STREAM_BUFF	8	//             
#endif
