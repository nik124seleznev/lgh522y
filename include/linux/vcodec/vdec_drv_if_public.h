/* 
        
                         
  
                
          
  
                    
                                                    
  
               
                           
  
                       
                  
              
  
 */

#ifndef _VDEC_DRV_IF_PUBLIC_H_
#define _VDEC_DRV_IF_PUBLIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "val_types_public.h"


/* 
                   
                      
                   
                                             
 */
typedef enum _VDEC_DRV_FBSTSTUS
{
    VDEC_DRV_FBSTSTUS_NORMAL      = 0,          //              
    VDEC_DRV_FBSTSTUS_REPEAT_LAST = (1 << 0),   //                    
    VDEC_DRV_FBSTSTUS_NOT_DISPLAY = (1 << 1),   //                
    VDEC_DRV_FBSTSTUS_NOT_USED    = (1 << 2),   //           
}
VDEC_DRV_FBSTSTUS;


/* 
                   
                           
                   
                                   
 */
typedef enum _VDEC_DRV_VIDEO_FORMAT_T
{
    VDEC_DRV_VIDEO_FORMAT_UNKNOWN_VIDEO_FORMAT  = 0,            //                       
    VDEC_DRV_VIDEO_FORMAT_DIVX311               = (1 << 0),     //             
    VDEC_DRV_VIDEO_FORMAT_DIVX4                 = (1 << 1),     //          
    VDEC_DRV_VIDEO_FORMAT_DIVX5                 = (1 << 2),     //          
    VDEC_DRV_VIDEO_FORMAT_XVID                  = (1 << 3),     //       
    VDEC_DRV_VIDEO_FORMAT_MPEG1                 = (1 << 4),     //         
    VDEC_DRV_VIDEO_FORMAT_MPEG2                 = (1 << 5),     //         
    VDEC_DRV_VIDEO_FORMAT_MPEG4                 = (1 << 6),     //         
    VDEC_DRV_VIDEO_FORMAT_H263                  = (1 << 7),     //       
    VDEC_DRV_VIDEO_FORMAT_H264                  = (1 << 8),     //       
    VDEC_DRV_VIDEO_FORMAT_H265                  = (1 << 9),     //       
    VDEC_DRV_VIDEO_FORMAT_WMV7                  = (1 << 10),    //       
    VDEC_DRV_VIDEO_FORMAT_WMV8                  = (1 << 11),    //       
    VDEC_DRV_VIDEO_FORMAT_WMV9                  = (1 << 12),    //       
    VDEC_DRV_VIDEO_FORMAT_VC1                   = (1 << 13),    //      
    VDEC_DRV_VIDEO_FORMAT_REALVIDEO8            = (1 << 14),    //      
    VDEC_DRV_VIDEO_FORMAT_REALVIDEO9            = (1 << 15),    //      
    VDEC_DRV_VIDEO_FORMAT_VP6                   = (1 << 16),    //      
    VDEC_DRV_VIDEO_FORMAT_VP7                   = (1 << 17),    //      
    VDEC_DRV_VIDEO_FORMAT_VP8                   = (1 << 18),    //      
    VDEC_DRV_VIDEO_FORMAT_VP8_WEBP_PICTURE_MODE = (1 << 19),    //                        
    VDEC_DRV_VIDEO_FORMAT_VP8_WEBP_MB_ROW_MODE  = (1 << 20),    //                    
    VDEC_DRV_VIDEO_FORMAT_VP9                   = (1 << 21),    //      
    VDEC_DRV_VIDEO_FORMAT_VP9_WEBP_PICTURE_MODE = (1 << 22),    //                        
    VDEC_DRV_VIDEO_FORMAT_VP9_WEBP_MB_ROW_MODE  = (1 << 23),    //                    
    VDEC_DRV_VIDEO_FORMAT_AVS                   = (1 << 24),    //      
    VDEC_DRV_VIDEO_FORMAT_MJPEG                 = (1 << 25),    //              
    VDEC_DRV_VIDEO_FORMAT_S263                  = (1 << 26),    //                 
    VDEC_DRV_VIDEO_FORMAT_H264HP                = (1 << 27),
    VDEC_DRV_VIDEO_FORMAT_H264SEC               = (1 << 28)
} VDEC_DRV_VIDEO_FORMAT_T;


/* 
                   
                                 
                   
                           
 */
typedef enum _VDEC_DRV_H265_VIDEO_PROFILE_T
{
    VDEC_DRV_H265_VIDEO_PROFILE_UNKNOWN         = 0,            //                        
    VDEC_DRV_H265_VIDEO_PROFILE_H265_MAIN       = (1 << 0),      //                    
    VDEC_DRV_H265_VIDEO_PROFILE_H265_MAIN_10       = (1 << 1),      //                       
    VDEC_DRV_H265_VIDEO_PROFILE_H265_STILL_IMAGE       = (1 << 2)      //                           
} VDEC_DRV_H265_VIDEO_PROFILE_T;


/* 
                   
                                 
                   
                           
 */
typedef enum _VDEC_DRV_H264_VIDEO_PROFILE_T
{
    VDEC_DRV_H264_VIDEO_PROFILE_UNKNOWN                   = 0,          //                        
    VDEC_DRV_H264_VIDEO_PROFILE_H264_BASELINE             = (1 << 0),   //                        
    VDEC_DRV_H264_VIDEO_PROFILE_H264_CONSTRAINED_BASELINE = (1 << 1),   //                                    
    VDEC_DRV_H264_VIDEO_PROFILE_H264_MAIN                 = (1 << 2),   //                    
    VDEC_DRV_H264_VIDEO_PROFILE_H264_EXTENDED             = (1 << 3),   //                        
    VDEC_DRV_H264_VIDEO_PROFILE_H264_HIGH                 = (1 << 4),   //                    
    VDEC_DRV_H264_VIDEO_PROFILE_H264_HIGH_10              = (1 << 5),   //                       
    VDEC_DRV_H264_VIDEO_PROFILE_H264_HIGH422              = (1 << 6),   //                        
    VDEC_DRV_H264_VIDEO_PROFILE_H264_HIGH444              = (1 << 7),   //                        
    VDEC_DRV_H264_VIDEO_PROFILE_H264_HIGH_10_INTRA        = (1 << 8),   //                                            
    VDEC_DRV_H264_VIDEO_PROFILE_H264_HIGH422_INTRA        = (1 << 9),   //                                             
    VDEC_DRV_H264_VIDEO_PROFILE_H264_HIGH444_INTRA        = (1 << 10),  //                                             
    VDEC_DRV_H264_VIDEO_PROFILE_H264_CAVLC444_INTRA       = (1 << 11),  //                                              
    VDEC_DRV_H264_VIDEO_PROFILE_H264_HIGH444_PREDICTIVE   = (1 << 12),  //                                                  
    VDEC_DRV_H264_VIDEO_PROFILE_H264_SCALABLE_BASELINE    = (1 << 13),  //                                                
    VDEC_DRV_H264_VIDEO_PROFILE_H264_SCALABLE_HIGH        = (1 << 14),  //                                            
    VDEC_DRV_H264_VIDEO_PROFILE_H264_SCALABLE_HIGH_INTRA  = (1 << 15),  //                                                  
    VDEC_DRV_H264_VIDEO_PROFILE_H264_MULTIVIEW_HIGH       = (1 << 16)   //                       
} VDEC_DRV_H264_VIDEO_PROFILE_T;


/* 
                   
                                 
                   
                                        
 */
typedef enum _VDEC_DRV_MPEG_VIDEO_PROFILE_T
{
    VDEC_DRV_MPEG_VIDEO_PROFILE_H263_0                = (1 << 0),   //                 
    VDEC_DRV_MPEG_VIDEO_PROFILE_H263_1                = (1 << 1),   //                 
    VDEC_DRV_MPEG_VIDEO_PROFILE_H263_2                = (1 << 2),   //                 
    VDEC_DRV_MPEG_VIDEO_PROFILE_H263_3                = (1 << 3),   //                 
    VDEC_DRV_MPEG_VIDEO_PROFILE_H263_4                = (1 << 4),   //                 
    VDEC_DRV_MPEG_VIDEO_PROFILE_H263_5                = (1 << 5),   //                 
    VDEC_DRV_MPEG_VIDEO_PROFILE_H263_6                = (1 << 6),   //                 
    VDEC_DRV_MPEG_VIDEO_PROFILE_H263_7                = (1 << 7),   //                 
    VDEC_DRV_MPEG_VIDEO_PROFILE_H263_8                = (1 << 8),   //                 
    VDEC_DRV_MPEG_VIDEO_PROFILE_MPEG2_SIMPLE          = (1 << 9),   //                       
    VDEC_DRV_MPEG_VIDEO_PROFILE_MPEG2_MAIN            = (1 << 10),  //                     
    VDEC_DRV_MPEG_VIDEO_PROFILE_MPEG2_SNR             = (1 << 11),  //                    
    VDEC_DRV_MPEG_VIDEO_PROFILE_MPEG2_SPATIAL         = (1 << 12),  //                        
    VDEC_DRV_MPEG_VIDEO_PROFILE_MPEG2_HIGH            = (1 << 13),  //                     
    VDEC_DRV_MPEG_VIDEO_PROFILE_MPEG4_SIMPLE          = (1 << 14),  //                       
    VDEC_DRV_MPEG_VIDEO_PROFILE_MPEG4_ADVANCED_SIMPLE = (1 << 15)   //                                
} VDEC_DRV_MPEG_VIDEO_PROFILE_T;


/* 
                   
                               
                   
                               
 */
typedef enum _VDEC_DRV_MS_VIDEO_PROFILE_T
{
    VDEC_DRV_MS_VIDEO_PROFILE_VC1_SIMPLE   = (1 << 0),  //                      
    VDEC_DRV_MS_VIDEO_PROFILE_VC1_MAIN     = (1 << 1),  //                    
    VDEC_DRV_MS_VIDEO_PROFILE_VC1_ADVANCED = (1 << 2),  //                        
    VDEC_DRV_MS_VIDEO_PROFILE_WMV9_SIMPLE  = (1 << 3),  //                      
    VDEC_DRV_MS_VIDEO_PROFILE_WMV9_MAIN    = (1 << 4),  //                    
    VDEC_DRV_MS_VIDEO_PROFILE_WMV9_COMPLEX = (1 << 5)   //                       
} VDEC_DRV_MS_VIDEO_PROFILE_T;


/* 
                   
                          
                   
               
 */
typedef enum _VDEC_DRV_VIDEO_LEVEL_T
{
    VDEC_DRV_VIDEO_LEVEL_UNKNOWN = 0,           //                
    VDEC_DRV_VIDEO_LEVEL_0,                     //                   
    VDEC_DRV_VIDEO_LEVEL_1,                     //                                      
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_1,   //                    
    VDEC_DRV_VIDEO_LEVEL_1b,                    //                    
    VDEC_DRV_VIDEO_LEVEL_1_1,                   //                    
    VDEC_DRV_VIDEO_LEVEL_1_2,                   //                    
    VDEC_DRV_VIDEO_LEVEL_1_3,                   //                    
    VDEC_DRV_VIDEO_LEVEL_2,                     //                                      
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_2,   //                    
    VDEC_DRV_VIDEO_LEVEL_2_1,                   //                          
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_2_1,  //                    
    VDEC_DRV_VIDEO_LEVEL_2_2,                   //                    
    VDEC_DRV_VIDEO_LEVEL_3,                     //                                      
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_3,    //                    
    VDEC_DRV_VIDEO_LEVEL_3_1,                   //                          
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_3_1,   //                    
    VDEC_DRV_VIDEO_LEVEL_3_2,                   //                    
    VDEC_DRV_VIDEO_LEVEL_4,                     //                               
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_4,     //                    
    VDEC_DRV_VIDEO_LEVEL_4_1,                   //                          
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_4_1,   //                    
    VDEC_DRV_VIDEO_LEVEL_4_2,                   //                    
    VDEC_DRV_VIDEO_LEVEL_5,                     //                          
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_5,     //                    
    VDEC_DRV_VIDEO_LEVEL_5_1,                   //                          
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_5_1,   //                    
    VDEC_DRV_VIDEO_LEVEL_5_2,                    //                    
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_5_2,    //                    
    VDEC_DRV_VIDEO_LEVEL_6,                        //                    
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_6,     //                    
    VDEC_DRV_VIDEO_LEVEL_6_1,                    //                    
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_6_1,   //                    
    VDEC_DRV_VIDEO_LEVEL_6_2,                    //                    
    VDEC_DRV_VIDEO_HIGH_TIER_LEVEL_6_2,   //                    
    VDEC_DRV_VIDEO_LEVEL_LOW,                   //                          
    VDEC_DRV_VIDEO_LEVEL_MEDIUM,                //                          
    VDEC_DRV_VIDEO_LEVEL_HIGH1440,              //                     
    VDEC_DRV_VIDEO_LEVEL_HIGH                  //                          

} VDEC_DRV_VIDEO_LEVEL_T;


/* 
                   
                         
                   
                            
 */
typedef enum _VDEC_DRV_RESOLUTION_T
{
    VDEC_DRV_RESOLUTION_UNKNOWN = 0,    //                     
    VDEC_DRV_RESOLUTION_SUPPORT_QCIF,   //       
    VDEC_DRV_RESOLUTION_SUPPORT_QVGA,   //       
    VDEC_DRV_RESOLUTION_SUPPORT_CIF,    //      
    VDEC_DRV_RESOLUTION_SUPPORT_480I,   //                    
    VDEC_DRV_RESOLUTION_SUPPORT_480P,   //                      
    VDEC_DRV_RESOLUTION_SUPPORT_576I,   //                    
    VDEC_DRV_RESOLUTION_SUPPORT_576P,   //                      
    VDEC_DRV_RESOLUTION_SUPPORT_720P,   //                       
    VDEC_DRV_RESOLUTION_SUPPORT_1080I,  //                      
    VDEC_DRV_RESOLUTION_SUPPORT_1080P,   //                        
    VDEC_DRV_RESOLUTION_SUPPORT_2160P   //                        
} VDEC_DRV_RESOLUTION_T;


/* 
                   
                         
                   
                                             
 */
typedef enum _VDEC_DRV_QUERY_TYPE_T
{
    VDEC_DRV_QUERY_TYPE_FBTYPE,             //                                   
    VDEC_DRV_QUERY_TYPE_VIDEO_FORMAT,       //                                         
    VDEC_DRV_QUERY_TYPE_PROPERTY,           //                            
    VDEC_DRV_QUERY_TYPE_CHIP_NAME,          //                                      
    VDEC_DRV_QUERY_TYPE_BUFFER_CONTROL,     //                                           
    VDEC_DRV_QUERY_TYPE_FEATURE_SUPPORTED,   //                                              
    VDEC_DRV_QUERY_TYPE_CPUCORE_FREQUENCY       //                                              
} VDEC_DRV_QUERY_TYPE_T;


/* 
                   
                         
                   
                                                     
 */
typedef enum _VDEC_DRV_FEATURE_TYPE_T
{
    VDEC_DRV_FEATURE_TYPE_NONE                = 0,        //        
    VDEC_DRV_FEATURE_TYPE_QUEUE_INPUT_BUFFER  = (1 << 0), //                                           
} VDEC_DRV_FEATURE_TYPE_T;


/* 
                   
                       
                   
                                                 
 */
typedef enum _VDEC_DRV_GET_TYPE_T
{
    VDEC_DRV_GET_TYPE_QUERY_REF_POOL_SIZE,          //                                                             
    VDEC_DRV_GET_TYPE_QUERY_DISP_POOL_SIZE,         //                                                           
    VDEC_DRV_GET_TYPE_DISP_FRAME_BUFFER,            //                                                                                       
    VDEC_DRV_GET_TYPE_FREE_FRAME_BUFFER,            //                                                                                                                            
    VDEC_DRV_GET_TYPE_GET_PICTURE_INFO,             //                                                         
    VDEC_DRV_GET_TYPE_GET_STATISTIC_INFO,           //                                
    VDEC_DRV_GET_TYPE_GET_FRAME_MODE,               //                               
    VDEC_DRV_GET_TYPE_GET_FRAME_CROP_INFO,          //                                 
    VDEC_DRV_GET_TYPE_QUERY_REORDER_ABILITY,        //                                                                 
    VDEC_DRV_GET_TYPE_QUERY_DOWNSIZE_ABILITY,       //                                             
    VDEC_DRV_GET_TYPE_QUERY_RESIZE_ABILITY,         //                                           
    VDEC_DRV_GET_TYPE_QUERY_DEBLOCK_ABILITY,        //                                    
    VDEC_DRV_GET_TYPE_QUERY_DEINTERLACE_ABILITY,    //                                     
    VDEC_DRV_GET_TYPE_QUERY_DROPFRAME_ABILITY,      //                                 
    VDEC_DRV_GET_TYPE_GET_DECODE_STATUS_INFO,       //                                                                                          
    VDEC_DRV_GET_TYPE_GET_PIXEL_FORMAT,             //                                       
    VDEC_DRV_GET_TYPE_GET_CPU_LOADING_INFO,         //                                                
    VDEC_DRV_GET_TYPE_GET_HW_CRC,                   //                   
    VDEC_DRV_GET_TYPE_GET_CODEC_TIDS,               //                                          
    VDEC_DRV_GET_TYPE_GET_FRAME_INTERVAL,           //                                          
    VDEC_DRV_GET_TYPE_FREE_INPUT_BUFFER,            //                    
    VDEC_DRV_GET_TYPE_QUERY_VIDEO_INTERLACING,      //                                    
    VDEC_DRV_GET_TYPE_QUERY_VIDEO_DPB_SIZE          //                       
} VDEC_DRV_GET_TYPE_T;


/* 
                   
                           
                   
                
 */
typedef enum _VDEC_DRV_PIXEL_FORMAT_T
{
    VDEC_DRV_PIXEL_FORMAT_NONE =   0,               //       
    VDEC_DRV_PIXEL_FORMAT_YUV_420_PLANER,           //                 
    VDEC_DRV_PIXEL_FORMAT_YUV_420_PLANER_MTK,       //                          
    VDEC_DRV_PIXEL_FORMAT_YUV_420_PLANER_UFO,       //                              
    VDEC_DRV_PIXEL_FORMAT_YUV_YV12                  //           
} VDEC_DRV_PIXEL_FORMAT_T;


/* 
                   
                           
                   
                
 */
typedef enum _VDEC_DRV_DECODER_TYPE_T
{
    VDEC_DRV_DECODER_MTK_HARDWARE = 0,              //               
    VDEC_DRV_DECODER_MTK_SOFTWARE,                  //               
    VDEC_DRV_DECODER_GOOGLE_SOFTWARE                //                            
}   VDEC_DRV_DECODER_TYPE_T;


/* 
                   
                       
                   
                                           
 */
typedef enum _VDEC_DRV_SET_TYPE_T
{
    VDEC_DRV_SET_TYPE_USE_EXT_TIMESTAMP,            //                                                            
    VDEC_DRV_SET_TYPE_SET_BUFFER_MODE,              //                                       
    VDEC_DRV_SET_TYPE_SET_FRAME_BUFFER_TYPE,        //                                                                                                    
    VDEC_DRV_SET_TYPE_FREE_FRAME_BFFER,             //                                                         
    VDEC_DRV_SET_TYPE_SET_REF_EXT_POOL_ADDR,        //                                         
    VDEC_DRV_SET_TYPE_SET_DISP_EXT_POOL_ADDR,       //                                          
    VDEC_DRV_SET_TYPE_SET_DECODE_MODE,              //                    
    VDEC_DRV_SET_TYPE_SET_POST_PROC,                //                                                                   
    VDEC_DRV_SET_TYPE_SET_STATISTIC_ON,             //                             
    VDEC_DRV_SET_TYPE_SET_STATISTIC_OFF,            //                              
    VDEC_DRV_SET_TYPE_SET_FRAME_MODE,               //                 
    VDEC_DRV_SET_TYPE_SET_BUF_STATUS_FOR_SPEEDY,    //                                    
    VDEC_DRV_SET_TYPE_SET_LAST_DISPLAY_TIME,        //                            
    VDEC_DRV_SET_TYPE_SET_CURRENT_PLAY_TIME,        //                            
    VDEC_DRV_SET_TYPE_SET_CONCEAL_LEVEL,            //                                  
    VDEC_DRV_SET_TYPE_SET_OMX_TIDS,                 //                     
    VDEC_DRV_SET_TYPE_SET_SWITCH_TVOUT,             //                          
    VDEC_DRV_SET_TYPE_SET_CODEC_COLOR_FORAMT,       //                         
    VDEC_DRV_SET_TYPE_SET_CODEC_YUV_STRIDE,         //                       
    VDEC_DRV_SET_TYPE_SET_FRAMESIZE,                //                                               
    VDEC_DRV_SET_TYPE_SET_FIXEDMAXOUTPUTBUFFER,     //                                                                
    VDEC_DRV_SET_TYPE_SET_UFO_DECODE,
} VDEC_DRV_SET_TYPE_T;


/* 
                   
                          
                   
                            
 */
typedef enum _VDEC_DRV_DECODE_MODE_T
{
    VDEC_DRV_DECODE_MODE_UNKNOWN = 0,               //          
    VDEC_DRV_DECODE_MODE_NORMAL,                    //                              
    VDEC_DRV_DECODE_MODE_I_ONLY,                    //                     
    VDEC_DRV_DECODE_MODE_B_SKIP,                    //               
    VDEC_DRV_DECODE_MODE_DROPFRAME,                 //                                             
    VDEC_DRV_DECODE_MODE_NO_REORDER,                //                                      
    VDEC_DRV_DECODE_MODE_THUMBNAIL,                 //                 
    VDEC_DRV_DECODE_MODE_SKIP_REFERENCE_CHECK,      //                                                                        
    VDEC_DRV_DECODE_MODE_LOW_LATENCY_DECODE,        //                                                                             
} VDEC_DRV_DECODE_MODE_T;


/* 
                   
                      
                   
                      
 */
typedef enum __VDEC_DRV_MRESULT_T
{
    VDEC_DRV_MRESULT_OK = 0,                        //     
    VDEC_DRV_MRESULT_FAIL,                          //       
    VDEC_DRV_MRESULT_FATAL,                         //                       
    VDEC_DRV_MRESULT_RESOLUTION_CHANGED,            //                              
    VDEC_DRV_MRESULT_NEED_MORE_OUTPUT_BUFFER,       //                                    
    VDEC_DRV_MRESULT_MAX = 0x0FFFFFFF               //            
} VDEC_DRV_MRESULT_T;


/* 
                 
                      
                   
                         
                               
                                      
 */
typedef struct __VDEC_DRV_RINGBUF_T
{
    VAL_MEM_ADDR_T  rBase;                      //                                       
    VAL_ULONG_T     u4Read;                     //                                           
    VAL_ULONG_T     u4Write;                    //                                            
    VAL_UINT32_T    u4Timestamp;                //                           
    VAL_UINT32_T    rSecMemHandle;              //                                                                   
} VDEC_DRV_RINGBUF_T;

/* 
                 
                        
                   
                                 
 */
typedef VDEC_DRV_RINGBUF_T *P_VDEC_DRV_RINGBUF_T;


/* 
                 
                       
                   
                            
 */
typedef struct __VDEC_DRV_FRAMEBUF_T
{
    VAL_MEM_ADDR_T  rBaseAddr;                  //                        
    VAL_MEM_ADDR_T  rPostProcAddr;              //                                
    VAL_UINT32_T    u4BufWidth;                 //                        
    VAL_UINT32_T    u4BufHeight;                //                         
    VAL_UINT32_T    u4DispWidth;                //                         
    VAL_UINT32_T    u4DispHeight;               //                         
    VAL_UINT32_T    u4DispPitch;                //                         
    VAL_UINT32_T    u4Timestamp;                //                                             
    VAL_UINT32_T    u4AspectRatioW;             //                                                           
    VAL_UINT32_T    u4AspectRatioH;             //                                                         
    VAL_UINT32_T    u4FrameBufferType;          //                                  
    VAL_UINT32_T    u4PictureStructure;         //                                      
    VAL_UINT32_T    u4FrameBufferStatus;        //                                    
    VAL_UINT32_T    u4IsUFOEncoded;             //                             
    VAL_UINT32_T    u4Reserved1;                //                    
    VAL_UINT32_T    u4Reserved2;                //                    
    VAL_UINT32_T    u4Reserved3;                //                    
    VAL_UINT32_T    rSecMemHandle;              //                                                                   
    VAL_UINT32_T    u4ReeVA;                    //                                                   
    VAL_UINT32_T    rFrameBufVaShareHandle;     //                                                                                       
} VDEC_DRV_FRAMEBUF_T;

/* 
                 
                         
                   
                                  
 */
typedef VDEC_DRV_FRAMEBUF_T *P_VDEC_DRV_FRAMEBUF_T;


/* 
                 
                       
                   
                              
 */
typedef struct __VDEC_DRV_CROPINFO_T
{
    VAL_UINT32_T u4CropLeft;        //                            
    VAL_UINT32_T u4CropRight;      //                             
    VAL_UINT32_T u4CropTop;           //                           
    VAL_UINT32_T u4CropBottom;      //                              
} VDEC_DRV_CROPINFO_T;


/* 
                 
                         
                   
                                  
 */
typedef VDEC_DRV_CROPINFO_T *P_VDEC_DRV_CROPINFO_T;

/* 
                 
                      
                   
                       
 */
typedef struct __VDEC_DRV_PICINFO_T
{
    VAL_UINT32_T    u4Width;                    //                    
    VAL_UINT32_T    u4Height;                   //                     
    VAL_UINT32_T    u4RealWidth;                //                                                
    VAL_UINT32_T    u4RealHeight;               //                                                 
    VAL_UINT32_T    u4Timestamp;                //                                          
    VAL_UINT32_T    u4AspectRatioW;             //                                                       
    VAL_UINT32_T    u4AspectRatioH;             //                                                     
    VAL_UINT32_T    u4FrameRate;                //                                   
    VAL_UINT32_T    u4PictureStructure;         //                                   
    VAL_UINT32_T    u4IsProgressiveOnly;        //                                                      
} VDEC_DRV_PICINFO_T;

/* 
                 
                        
                   
                                 
 */
typedef VDEC_DRV_PICINFO_T *P_VDEC_DRV_PICINFO_T;


/* 
                 
                      
                   
                         
                            
 */
typedef struct __VDEC_DRV_SEQINFO_T
{
    VAL_UINT32_T    u4Width;                    //                              
    VAL_UINT32_T    u4Height;                   //                               
    VAL_UINT32_T    u4PicWidth;                 //                               
    VAL_UINT32_T    u4PicHeight;                //                                
    VAL_INT32_T     i4AspectRatioWidth;         //                                    
    VAL_INT32_T     i4AspectRatioHeight;        //                                     
    VAL_BOOL_T      bIsThumbnail;               //                        
} VDEC_DRV_SEQINFO_T;

/* 
                 
                        
                   
                                 
 */
typedef VDEC_DRV_SEQINFO_T *P_VDEC_DRV_SEQINFO_T;


/* 
                 
                         
                   
                           
 */
typedef struct __VDEC_DRV_YUV_STRIDE_T
{
    unsigned int    u4YStride;                  //                
    unsigned int    u4UVStride;                 //                 
} VDEC_DRV_YUV_STRIDE_T;

/* 
                 
                           
                   
                                    
 */
typedef VDEC_DRV_YUV_STRIDE_T *P_VDEC_DRV_YUV_STRIDE_T;


/* 
                 
                                 
                   
                                                    
 */
typedef struct __VDEC_DRV_QUERY_VIDEO_FORMAT_T
{
    VAL_UINT32_T            u4VideoFormat;          //                              
    VAL_UINT32_T            u4Profile;              //                               
    VAL_UINT32_T            u4Level;                //                             
    VAL_UINT32_T            u4Resolution;           //                            
    VAL_UINT32_T            u4Width;                //                    
    VAL_UINT32_T            u4Height;               //                     
    VAL_UINT32_T            u4StrideAlign;          //                               
    VAL_UINT32_T            u4SliceHeightAlign;     //                                     
    VDEC_DRV_PIXEL_FORMAT_T ePixelFormat;           //                     
    VDEC_DRV_DECODER_TYPE_T eDecodeType;            //                     
} VDEC_DRV_QUERY_VIDEO_FORMAT_T;

/* 
                 
                                   
                   
                                            
 */
typedef VDEC_DRV_QUERY_VIDEO_FORMAT_T *P_VDEC_DRV_QUERY_VIDEO_FORMAT_T;


/* 
                 
                              
                   
                           
 */
typedef struct __VDEC_DRV_SET_DECODE_MODE_T
{
    VDEC_DRV_DECODE_MODE_T  eDecodeMode;            //                                       
    VAL_UINT32_T            u4DisplayFrameNum;      //                                                          
    VAL_UINT32_T            u4DropFrameNum;         //                                                          
} VDEC_DRV_SET_DECODE_MODE_T;

/* 
                 
                                
                   
                                         
 */
typedef VDEC_DRV_SET_DECODE_MODE_T *P_VDEC_DRV_SET_DECODE_MODE_T;


/* 
                 
                       
                   
                                
 */
typedef struct __VDEC_DRV_PROPERTY_T
{
    VAL_UINT32_T        u4BufAlign;                 //                                      
    VAL_UINT32_T        u4BufUnitSize;              //                                                                           
    VAL_BOOL_T          fgPostprocessSupport;       //                              
    struct
    {
        VAL_UINT32_T    fgOverlay   : 1;            //                        
        VAL_UINT32_T    fgRotate    : 1;            //                       
        VAL_UINT32_T    fgResize    : 1;            //                       
        VAL_UINT32_T    fgCrop      : 1;            //                        
    } PostProcCapability;                           //                                 
} VDEC_DRV_PROPERTY_T;

/* 
                 
                         
                   
                                  
 */
typedef VDEC_DRV_PROPERTY_T *P_VDEC_DRV_PROPERTY_T;


/* 
                 
                            
                    
                                      
                                                                                                           
         
                                                                                   
         
                                                                     
         
                                                                                    
                
                                            
                                         
 */
VDEC_DRV_MRESULT_T  eVDecDrvQueryCapability(VDEC_DRV_QUERY_TYPE_T a_eType, VAL_VOID_T *a_pvInParam, VAL_VOID_T *a_pvOutParam);


/* 
                 
                   
                    
                     
                                           
                                                             
         
                                              
         
                                                                        
                
                                                                                 
                                                         
                                                      
 */
VDEC_DRV_MRESULT_T  eVDecDrvCreate(VAL_HANDLE_T *a_phHandle, VDEC_DRV_VIDEO_FORMAT_T a_eVideoFormat);


/* 
                 
                    
                    
                             
                                                              
                            
                            
                        
         
                                                          
                
                                                           
                                                        
 */
VDEC_DRV_MRESULT_T  eVDecDrvRelease(VAL_HANDLE_T a_hHandle);


/* 
                 
                 
                    
                                
                                         
         
                                           
         
                                                                       
         
                                                                  
                
                                                        
                                                     
 */
VDEC_DRV_MRESULT_T  eVDecDrvInit(VAL_HANDLE_T a_hHandle, VDEC_DRV_RINGBUF_T *a_prBitstream, VDEC_DRV_SEQINFO_T *a_prSeqinfo);


/* 
                 
                   
                    
                           
                                                     
         
                                          
                
                                                          
                                                       
 */
VDEC_DRV_MRESULT_T  eVDecDrvDeInit(VAL_HANDLE_T a_hHandle);


/* 
                 
                     
                    
                              
                                                                   
         
                                           
         
                                            
         
                                                                 
         
                                                    
                
                                                          
                                                       
                              
                                               
                                                   
 */
VDEC_DRV_MRESULT_T  eVDecDrvGetParam(VAL_HANDLE_T a_hHandle, VDEC_DRV_GET_TYPE_T a_eType, VAL_VOID_T *a_pvInParam, VAL_VOID_T *a_pvOutParam);


/* 
                 
                     
                    
                               
         
                                           
         
                                            
         
                                             
         
                                              
                
                                                          
                                                       
                              
                                               
                                 
 */
VDEC_DRV_MRESULT_T  eVDecDrvSetParam(VAL_HANDLE_T a_hHandle, VDEC_DRV_SET_TYPE_T a_eType, VAL_VOID_T *a_pvInParam, VAL_VOID_T *a_pvOutParam);


/* 
                 
                  
                    
                      
                                                             
                                          
                        
                           
                                                                                                                            
         
                                          
         
                                            
         
                                              
                
                                                   
                                                
 */
VDEC_DRV_MRESULT_T  eVDecDrvDecode(VAL_HANDLE_T a_hHandle, VDEC_DRV_RINGBUF_T *a_prBitstream, VDEC_DRV_FRAMEBUF_T *a_prFramebuf);


#ifdef __cplusplus
}
#endif

#endif //                               
