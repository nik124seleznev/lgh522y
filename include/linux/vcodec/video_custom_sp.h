/* 
        
                      
  
                
          
  
                    
                                                  
  
               
                           
  
                       
                  
              
  
 */

#ifndef VIDEO_CUSTOM_SP_H
#define VIDEO_CUSTOM_SP_H

//                              
#include "vcodec_if_v2.h"

#define ASSERT(expr)                                            \
    do {                                                        \
        if (!(expr))                                            \
            AssertionFailed(__FUNCTION__,__FILE__, __LINE__);   \
    } while (0)     //                    

/*                                                                             
  
  
         
  
  
                                                                             */


/* 
                   
                    
                   
                                              
 */
typedef enum _VIDEO_DECODER_T
{
    CODEC_DEC_NONE      = 0,            //       
    CODEC_DEC_H263      = (0x1 << 0),   //       
    CODEC_DEC_MPEG4     = (0x1 << 1),   //        
    CODEC_DEC_H264      = (0x1 << 2),   //       
    CODEC_DEC_RV        = (0x1 << 3),   //     
    CODEC_DEC_VC1       = (0x1 << 4),   //      
    CODEC_DEC_VP8       = (0x1 << 5),   //      
    CODEC_DEC_VP9       = (0x1 << 6),   //      
    CODEC_DEC_HEVC      = (0x1 << 7),   //       
    CODEC_DEC_MPEG2     = (0x1 << 8),   //        
    CODEC_DEC_MAX       = (0x1 << 9)    //      
} VIDEO_DECODER_T;

#if 1   //                                         


/* 
                
                  
                   
                                                           
         
                                         
         
                                    
         
                                              
         
                                                  
               
                                       
 */
VIDEO_DEC_API_T  *GetDecoderAPI(VIDEO_DECODER_T eDecType, HANDLE hWrapper, void **ppDrvModule, unsigned int bUseMultiCoreCodec);
#else
VIDEO_DEC_API_T  *GetDecoderAPI(VIDEO_DECODER_T, HANDLE); //                          
#endif
//                                           
//                                          
//                                        
//                                         
//                                         


/*                                                                             
 
 
        
 
 
                                                                             */


/* 
                   
                    
                   
                                              
 */
typedef enum _VIDEO_ENCODER_T
{
    CODEC_ENC_NONE      = 0,            //       
    CODEC_ENC_H263      = (0x1 << 0),   //       
    CODEC_ENC_MPEG4     = (0x1 << 1),   //        
    CODEC_ENC_H264      = (0x1 << 2),   //       
    CODEC_ENC_HEVC      = (0x1 << 3),   //       
    CODEC_ENC_VP8       = (0x1 << 5),   //      
    CODEC_ENC_VP9       = (0x1 << 6),   //      
    CODEC_ENC_MAX       = (0x1 << 7)    //      
} VIDEO_ENCODER_T;


/* 
                
                  
                   
                                                           
         
                                         
         
                                    
         
                                              
         
                                                  
               
                                       
 */
VIDEO_ENC_API_T *GetEncoderAPI(VIDEO_ENCODER_T eEncType, HANDLE hWrapper, void **ppDrvModule, unsigned int bUseMultiCoreCodec);
//                                           
//                                          
//                                            


#endif /*                   */
