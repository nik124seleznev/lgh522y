/* 
        
                         
  
                
          
  
                    
                                                    
  
               
                           
  
                       
                  
              
  
 */

#ifndef _VENC_DRV_IF_PUBLIC_H_
#define _VENC_DRV_IF_PUBLIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "val_types_public.h"


/* 
                   
                          
                   
                                           
 */
typedef enum __VENC_DRV_QUERY_TYPE_T
{
    VENC_DRV_QUERY_TYPE_NONE,                   //                           
    VENC_DRV_QUERY_TYPE_VIDEO_FORMAT,           //                              
    VENC_DRV_QUERY_TYPE_VIDEO_PROPERTY,         //                           
    VENC_DRV_QUERY_TYPE_VIDEO_PROPERTY_LIST,    //                                
    VENC_DRV_QUERY_TYPE_PROPERTY,               //                          
    VENC_DRV_QUERY_TYPE_MCI_SUPPORTED,          //                                 
    VENC_DRV_QUERY_TYPE_CHIP_NAME,              //                  
    VENC_DRV_QUERY_TYPE_INPUT_BUF_LIMIT,        //                                            
    VENC_DRV_QUERY_TYPE_NORMAL_PRIO,            //                                                                  
    VENC_DRV_QUERY_TYPE_VIDEO_CAMCORDER_CAP,    //                               
    VENC_DRV_QUERY_TYPE_MAX = 0xFFFFFFFF        //                                  
}
VENC_DRV_QUERY_TYPE_T;


/* 
                   
                          
                   
                                                      
 */
typedef enum __VENC_DRV_YUV_FORMAT_T
{
    VENC_DRV_YUV_FORMAT_NONE,                   //                           
    VENC_DRV_YUV_FORMAT_GRAY,                   //                  
    VENC_DRV_YUV_FORMAT_422,                    //                 
    VENC_DRV_YUV_FORMAT_420,                    //                 
    VENC_DRV_YUV_FORMAT_411,                    //                 
    VENC_DRV_YUV_FORMAT_YV12,                   //                                     
    VENC_DRV_YUV_FORMAT_NV12,                   //                  
    VENC_DRV_YUV_FORMAT_NV21,                   //                  
    VENC_DRV_YUV_FORMAT_BLK16X32,               //                         
    VENC_DRV_YUV_FORMAT_BLK64X32,               //                         
    VENC_DRV_YUV_FORMAT_YV12_1688,              //                  
    VENC_DRV_YUV_FORMAT_MAX = 0xFFFFFFFF        //                                  
} VENC_DRV_YUV_FORMAT_T;


/* 
                   
                            
                   
                                                  
 */
typedef enum __VENC_DRV_VIDEO_FORMAT_T
{
    VENC_DRV_VIDEO_FORMAT_NONE,                 //                           
    VENC_DRV_VIDEO_FORMAT_MPEG4,                //                     
    VENC_DRV_VIDEO_FORMAT_MPEG4_1080P,          //                               
    VENC_DRV_VIDEO_FORMAT_MPEG4_SHORT,          //                                                    
    VENC_DRV_VIDEO_FORMAT_H263,                 //                     
    VENC_DRV_VIDEO_FORMAT_H264,                 //                     
    VENC_DRV_VIDEO_FORMAT_H264_VGA,             //                             
    VENC_DRV_VIDEO_FORMAT_WMV9,                 //                    
    VENC_DRV_VIDEO_FORMAT_VC1,                  //                   
    VENC_DRV_VIDEO_FORMAT_VP8,                  //                   
    VENC_DRV_VIDEO_FORMAT_JPEG,                 //                      
    VENC_DRV_VIDEO_FORMAT_HEVC,                 //                    
    VENC_DRV_VIDEO_FORMAT_H264SEC,              //                
    VENC_DRV_VIDEO_FORMAT_MAX = 0xFFFFFFFF      //                                    
} VENC_DRV_VIDEO_FORMAT_T;


/* 
                   
                          
                   
                                                
 */
typedef enum __VENC_DRV_FRAME_RATE_T
{
    VENC_DRV_FRAME_RATE_NONE    = 0,            //                           
    VENC_DRV_FRAME_RATE_7_5     = 75,           //      
    VENC_DRV_FRAME_RATE_10      = 10,           //     
    VENC_DRV_FRAME_RATE_15      = 15,           //     
    VENC_DRV_FRAME_RATE_20      = 20,           //     
    VENC_DRV_FRAME_RATE_24      = 24,           //     
    VENC_DRV_FRAME_RATE_25      = 25,           //     
    VENC_DRV_FRAME_RATE_29_97   = 2997,         //        
    VENC_DRV_FRAME_RATE_30      = 30,           //     
    VENC_DRV_FRAME_RATE_60      = 60,           //     
    VENC_DRV_FRAME_RATE_120     = 120,          //      
    VENC_DRV_FRAME_RATE_180     = 180,          //      
    VENC_DRV_FRAME_RATE_240     = 240,          //      
    VENC_DRV_FRAME_RATE_480     = 480,          //      
    VENC_DRV_FRAME_RATE_MAX     = 0xFFFFFFFF    //                                  
} VENC_DRV_FRAME_RATE_T;


/* 
                   
                         
                   
                                                
 */
typedef enum __VENC_DRV_START_OPT_T
{
    VENC_DRV_START_OPT_NONE,                                //                           
    VENC_DRV_START_OPT_ENCODE_SEQUENCE_HEADER,              //                           
    VENC_DRV_START_OPT_ENCODE_SEQUENCE_HEADER_H264_SPS,     //                                    
    VENC_DRV_START_OPT_ENCODE_SEQUENCE_HEADER_H264_PPS,     //                                    
    VENC_DRV_START_OPT_ENCODE_FRAME,                        //                 
    VENC_DRV_START_OPT_ENCODE_KEY_FRAME,                    //                     
    VENC_DRV_START_OPT_ENCODE_FINAL,                        //                                                
    VENC_DRV_START_OPT_ENCODE_DUMMY_NAL,                    //                             
    VENC_DRV_START_OPT_MAX = 0xFFFFFFFF                     //                                 
} VENC_DRV_START_OPT_T;


/* 
                   
                       
                   
                                                  
 */
typedef enum __VENC_DRV_MESSAGE_T
{
    VENC_DRV_MESSAGE_NONE,                      //                           
    VENC_DRV_MESSAGE_OK,                        //            
    VENC_DRV_MESSAGE_ERR,                       //               
    VENC_DRV_MESSAGE_TIMEOUT,                   //                 
    VENC_DRV_MESSAGE_PARTIAL,                   //                                      
    VENC_DRV_MESSAGE_MAX = 0xFFFFFFFF           //                               
} VENC_DRV_MESSAGE_T;


/* 
                   
                                  
                   
                                                               
 */
typedef enum __VENC_DRV_H264_VIDEO_PROFILE_T
{
    VENC_DRV_H264_VIDEO_PROFILE_UNKNOWN              = 0,           //                           
    VENC_DRV_H264_VIDEO_PROFILE_BASELINE             = (1 << 0),    //           
    VENC_DRV_H264_VIDEO_PROFILE_CONSTRAINED_BASELINE = (1 << 1),    //                       
    VENC_DRV_H264_VIDEO_PROFILE_MAIN                 = (1 << 2),    //       
    VENC_DRV_H264_VIDEO_PROFILE_EXTENDED             = (1 << 3),    //           
    VENC_DRV_H264_VIDEO_PROFILE_HIGH                 = (1 << 4),    //       
    VENC_DRV_H264_VIDEO_PROFILE_HIGH_10              = (1 << 5),    //          
    VENC_DRV_H264_VIDEO_PROFILE_HIGH422              = (1 << 6),    //           
    VENC_DRV_H264_VIDEO_PROFILE_HIGH444              = (1 << 7),    //           
    VENC_DRV_H264_VIDEO_PROFILE_HIGH_10_INTRA        = (1 << 8),    //                              
    VENC_DRV_H264_VIDEO_PROFILE_HIGH422_INTRA        = (1 << 9),    //                               
    VENC_DRV_H264_VIDEO_PROFILE_HIGH444_INTRA        = (1 << 10),   //                               
    VENC_DRV_H264_VIDEO_PROFILE_CAVLC444_INTRA       = (1 << 11),   //                                
    VENC_DRV_H264_VIDEO_PROFILE_HIGH444_PREDICTIVE   = (1 << 12),   //                                    
    VENC_DRV_H264_VIDEO_PROFILE_SCALABLE_BASELINE    = (1 << 13),   //                                  
    VENC_DRV_H264_VIDEO_PROFILE_SCALABLE_HIGH        = (1 << 14),   //                              
    VENC_DRV_H264_VIDEO_PROFILE_SCALABLE_HIGH_INTRA  = (1 << 15),   //                                    
    VENC_DRV_H264_VIDEO_PROFILE_MULTIVIEW_HIGH       = (1 << 16),   //                                        
    VENC_DRV_H264_VIDEO_PROFILE_MAX                  = 0xFFFFFFFF   //                                          
} VENC_DRV_H264_VIDEO_PROFILE_T;


/* 
                   
                                  
                   
                                                              
 */
typedef enum __VENC_DRV_HEVC_VIDEO_PROFILE_T
{
    VENC_DRV_HEVC_VIDEO_PROFILE_UNKNOWN              = 0,           //                           
    VENC_DRV_HEVC_VIDEO_PROFILE_BASELINE             = (1 << 0),    //           
    VENC_DRV_HEVC_VIDEO_PROFILE_CONSTRAINED_BASELINE = (1 << 1),    //                       
    VENC_DRV_HEVC_VIDEO_PROFILE_MAIN                 = (1 << 2),    //       
    VENC_DRV_HEVC_VIDEO_PROFILE_EXTENDED             = (1 << 3),    //           
    VENC_DRV_HEVC_VIDEO_PROFILE_HIGH                 = (1 << 4),    //       
    VENC_DRV_HEVC_VIDEO_PROFILE_HIGH_10              = (1 << 5),    //          
    VENC_DRV_HEVC_VIDEO_PROFILE_HIGH422              = (1 << 6),    //           
    VENC_DRV_HEVC_VIDEO_PROFILE_HIGH444              = (1 << 7),    //           
    VENC_DRV_HEVC_VIDEO_PROFILE_HIGH_10_INTRA        = (1 << 8),    //                              
    VENC_DRV_HEVC_VIDEO_PROFILE_HIGH422_INTRA        = (1 << 9),    //                               
    VENC_DRV_HEVC_VIDEO_PROFILE_HIGH444_INTRA        = (1 << 10),   //                               
    VENC_DRV_HEVC_VIDEO_PROFILE_CAVLC444_INTRA       = (1 << 11),   //                                
    VENC_DRV_HEVC_VIDEO_PROFILE_HIGH444_PREDICTIVE   = (1 << 12),   //                                    
    VENC_DRV_HEVC_VIDEO_PROFILE_SCALABLE_BASELINE    = (1 << 13),   //                                  
    VENC_DRV_HEVC_VIDEO_PROFILE_SCALABLE_HIGH        = (1 << 14),   //                              
    VENC_DRV_HEVC_VIDEO_PROFILE_SCALABLE_HIGH_INTRA  = (1 << 15),   //                                    
    VENC_DRV_HEVC_VIDEO_PROFILE_MULTIVIEW_HIGH       = (1 << 16),   //                                        
    VENC_DRV_HEVC_VIDEO_PROFILE_MAX                  = 0xFFFFFFFF   //                                          
} VENC_DRV_HEVC_VIDEO_PROFILE_T;


/* 
                   
                                  
                   
                                                                             
 */
typedef enum __VENC_DRV_MPEG_VIDEO_PROFILE_T
{
    VENC_DRV_MPEG_VIDEO_PROFILE_UNKNOWN               = 0,          //                           
    VENC_DRV_MPEG_VIDEO_PROFILE_H263_0                = (1 << 0),   //          
    VENC_DRV_MPEG_VIDEO_PROFILE_H263_1                = (1 << 1),   //          
    VENC_DRV_MPEG_VIDEO_PROFILE_H263_2                = (1 << 2),   //          
    VENC_DRV_MPEG_VIDEO_PROFILE_H263_3                = (1 << 3),   //          
    VENC_DRV_MPEG_VIDEO_PROFILE_H263_4                = (1 << 4),   //          
    VENC_DRV_MPEG_VIDEO_PROFILE_H263_5                = (1 << 5),   //          
    VENC_DRV_MPEG_VIDEO_PROFILE_H263_6                = (1 << 6),   //          
    VENC_DRV_MPEG_VIDEO_PROFILE_H263_7                = (1 << 7),   //          
    VENC_DRV_MPEG_VIDEO_PROFILE_H263_8                = (1 << 8),   //          
    VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_SIMPLE          = (1 << 9),   //               
    VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_MAIN            = (1 << 10),  //             
    VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_SNR             = (1 << 11),  //            
    VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_SPATIAL         = (1 << 12),  //                
    VENC_DRV_MPEG_VIDEO_PROFILE_MPEG2_HIGH            = (1 << 13),  //             
    VENC_DRV_MPEG_VIDEO_PROFILE_MPEG4_SIMPLE          = (1 << 14),  //               
    VENC_DRV_MPEG_VIDEO_PROFILE_MPEG4_ADVANCED_SIMPLE = (1 << 15),  //                        
    VENC_DRV_MPEG_VIDEO_PROFILE_MAX                   = 0xFFFFFFFF  //                                          
} VENC_DRV_MPEG_VIDEO_PROFILE_T;


/* 
                   
                                
                   
                                                            
 */
typedef enum __VENC_DRV_MS_VIDEO_PROFILE_T
{
    VENC_DRV_MS_VIDEO_PROFILE_UNKNOWN      = 0,             //                           
    VENC_DRV_MS_VIDEO_PROFILE_VC1_SIMPLE   = (1 << 0),      //             
    VENC_DRV_MS_VIDEO_PROFILE_VC1_MAIN     = (1 << 1),      //           
    VENC_DRV_MS_VIDEO_PROFILE_VC1_ADVANCED = (1 << 2),      //               
    VENC_DRV_MS_VIDEO_PROFILE_WMV9_SIMPLE  = (1 << 3),      //              
    VENC_DRV_MS_VIDEO_PROFILE_WMV9_MAIN    = (1 << 4),      //            
    VENC_DRV_MS_VIDEO_PROFILE_WMV9_COMPLEX = (1 << 5),      //               
    VENC_DRV_MS_VIDEO_PROFILE_MAX          = 0xFFFFFFFF     //                                        
} VENC_DRV_MS_VIDEO_PROFILE_T;


/* 
                   
                           
                   
                                                       
 */
typedef enum __VENC_DRV_VIDEO_LEVEL_T
{
    VENC_DRV_VIDEO_LEVEL_UNKNOWN = 0,       //                           
    VENC_DRV_VIDEO_LEVEL_0,                 //      
    VENC_DRV_VIDEO_LEVEL_1,                 //                         
    VENC_DRV_VIDEO_LEVEL_1b,                //             
    VENC_DRV_VIDEO_LEVEL_1_1,               //             
    VENC_DRV_VIDEO_LEVEL_1_2,               //             
    VENC_DRV_VIDEO_LEVEL_1_3,               //             
    VENC_DRV_VIDEO_LEVEL_2,                 //                         
    VENC_DRV_VIDEO_LEVEL_2_1,               //             
    VENC_DRV_VIDEO_LEVEL_2_2,               //             
    VENC_DRV_VIDEO_LEVEL_3,                 //                         
    VENC_DRV_VIDEO_LEVEL_3_1,               //             
    VENC_DRV_VIDEO_LEVEL_3_2,               //             
    VENC_DRV_VIDEO_LEVEL_4,                 //                  
    VENC_DRV_VIDEO_LEVEL_4_1,               //             
    VENC_DRV_VIDEO_LEVEL_4_2,               //             
    VENC_DRV_VIDEO_LEVEL_5,                 //                   
    VENC_DRV_VIDEO_LEVEL_5_1,               //             
    VENC_DRV_VIDEO_LEVEL_LOW,               //             
    VENC_DRV_VIDEO_LEVEL_MEDIUM,            //             
    VENC_DRV_VIDEO_LEVEL_HIGH1440,          //        
    VENC_DRV_VIDEO_LEVEL_HIGH,              //             
    VENC_DRV_VIDEO_LEVEL_MAX = 0xFFFFFFFF   //                                   
} VENC_DRV_VIDEO_LEVEL_T;


/* 
                   
                          
                   
                                                            
 */
typedef enum __VENC_DRV_RESOLUTION_T
{
    VENC_DRV_RESOLUTION_UNKNOWN = 0,                //                           
    VENC_DRV_RESOLUTION_SUPPORT_QCIF,               //      
    VENC_DRV_RESOLUTION_SUPPORT_QVGA,               //       
    VENC_DRV_RESOLUTION_SUPPORT_CIF,                //       
    VENC_DRV_RESOLUTION_SUPPORT_HVGA,               //                
    VENC_DRV_RESOLUTION_SUPPORT_VGA,                //               
    VENC_DRV_RESOLUTION_SUPPORT_480I,               //       
    VENC_DRV_RESOLUTION_SUPPORT_480P,               //       
    VENC_DRV_RESOLUTION_SUPPORT_576I,               //       
    VENC_DRV_RESOLUTION_SUPPORT_576P,               //       
    VENC_DRV_RESOLUTION_SUPPORT_FWVGA,              //                 
    VENC_DRV_RESOLUTION_SUPPORT_720I,               //       
    VENC_DRV_RESOLUTION_SUPPORT_720P,               //       
    VENC_DRV_RESOLUTION_SUPPORT_1080I,              //        
    VENC_DRV_RESOLUTION_SUPPORT_1080P,              //        
    VENC_DRV_RESOLUTION_SUPPORT_2160P,              //        
    VENC_DRV_RESOLUTION_SUPPORT_MAX = 0xFFFFFFFF    //                                  
} VENC_DRV_RESOLUTION_T;


/* 
                   
                        
                   
                                                       
 */
typedef enum __VENC_DRV_SET_TYPE_T
{
    VENC_DRV_SET_TYPE_UNKONW = 0,           //                           
    VENC_DRV_SET_TYPE_RST,                  //            
    VENC_DRV_SET_TYPE_CB,                   //                        
    VENC_DRV_SET_TYPE_PARAM_RC,             //                             
    VENC_DRV_SET_TYPE_PARAM_ME,             //                                  
    VENC_DRV_SET_TYPE_PARAM_EIS,            //                    
    VENC_DRV_SET_TYPE_PARAM_ENC,            //                                                      
    VENC_DRV_SET_TYPE_STATISTIC_ON,         //                            
    VENC_DRV_SET_TYPE_STATISTIC_OFF,        //                             
    VENC_DRV_SET_TYPE_SET_OMX_TIDS,         //                     
    VENC_DRV_SET_TYPE_MPEG4_SHORT,          //                              
    VENC_DRV_SET_TYPE_FORCE_INTRA_ON,       //                           
    VENC_DRV_SET_TYPE_FORCE_INTRA_OFF,      //                            
    VENC_DRV_SET_TYPE_TIME_LAPSE,           //                 
    VENC_DRV_SET_TYPE_ALLOC_WORK_BUF,       //                              
    VENC_DRV_SET_TYPE_DUMP_WORK_BUF,        //                             
    VENC_DRV_SET_TYPE_FREE_WORK_BUF,        //                             
    VENC_DRV_SET_TYPE_ADJUST_BITRATE,       //                        
    VENC_DRV_SET_TYPE_I_FRAME_INTERVAL,     //                       
    VENC_DRV_SET_TYPE_WFD_MODE,             //                        
    VENC_DRV_SET_TYPE_RECORD_SIZE,          //                  
    VENC_DRV_SET_TYPE_USE_MCI_BUF,          //                        
    VENC_DRV_SET_TYPE_ADJUST_FRAMERATE,     //                 
    VENC_DRV_SET_TYPE_INIT_QP,              //              
    VENC_DRV_SET_TYPE_SKIP_FRAME,           //                     
    VENC_DRV_SET_TYPE_SCENARIO,             //                    
    VENC_DRV_SET_TYPE_PREPEND_HEADER,       //                                 
    VENC_DRV_SET_TYPE_SLOW_MOTION_ENCODE,       //                                                         
    VENC_DRV_SET_TYPE_SLOW_MOTION_POST_PROC,    //                                                                         
    VENC_DRV_SET_TYPE_SLOW_MOTION_LOCK_HW,      //                                                 
    VENC_DRV_SET_TYPE_SLOW_MOTION_UNLOCK_HW,    //                                                   
    VENC_DRV_SET_TYPE_NONREFP,              //                                           
    VENC_DRV_SET_TYPE_MAX = 0xFFFFFFFF      //                                
} VENC_DRV_SET_TYPE_T;


/* 
                   
                        
                   
                                                       
 */
typedef enum __VENC_DRV_GET_TYPE_T
{
    VENC_DRV_GET_TYPE_UNKONW = 0,               //                           
    VENC_DRV_GET_TYPE_PARAM_RC,                 //                             
    VENC_DRV_GET_TYPE_PARAM_ME,                 //                                  
    VENC_DRV_GET_TYPE_PARAM_EIS,                //                    
    VENC_DRV_GET_TYPE_PARAM_ENC,                //                                                      
    VENC_DRV_GET_TYPE_STATISTIC,                //                 
    VENC_DRV_GET_TYPE_GET_CPU_LOADING_INFO,     //                                                
    VENC_DRV_GET_TYPE_GET_YUV_FORMAT,           //                 
    VENC_DRV_GET_TYPE_GET_CODEC_TIDS,
    /*                            */
    VENC_DRV_GET_TYPE_ALLOC_META_HANDLE_LIST,           //                                           
    VENC_DRV_GET_TYPE_GET_BUF_INFO_FROM_META_HANDLE,    //                                                     
    VENC_DRV_GET_TYPE_FREE_META_HANDLE_LIST,            //                                                                        
    /*                            */
    VENC_DRV_GET_TYPE_MAX = 0xFFFFFFFF          //                                  
} VENC_DRV_GET_TYPE_T;


/* 
                   
                       
                   
                                               
 */
typedef enum __VENC_DRV_MRESULT_T
{
    VENC_DRV_MRESULT_OK = 0,                    //                 
    VENC_DRV_MRESULT_FAIL,                      //              
    VENC_DRV_MRESULT_MAX = 0x0FFFFFFF           //                               
} VENC_DRV_MRESULT_T;


/* 
                   
                        
                   
                                           
 */
typedef enum __VENC_DRV_SCENARIO_T
{
    VENC_DRV_SCENARIO_CAMERA_REC            = 1,        //                   
    VENC_DRV_SCENARIO_LIVEPHOTO_CAPTURE     = (1 << 1), //                      
    VENC_DRV_SCENARIO_LIVEPHOTO_EFFECT      = (1 << 2), //                               
    VENC_DRV_SCENARIO_CAMERA_REC_SLOW_MOTION = (1 << 3), //                                    
    VENC_DRV_SCENARIO_SCREEN_REC            = (1 << 4), //                   
} VENC_DRV_SCENARIO_T;


/* 
                 
                                  
                   
                                                            
 */
typedef struct __VENC_DRV_QUERY_VIDEO_FORMAT_T
{
    VENC_DRV_VIDEO_FORMAT_T eVideoFormat;       //                                
    VAL_UINT32_T            u4Profile;          //                                                                                                                             
    VENC_DRV_VIDEO_LEVEL_T  eLevel;             //                               
    VENC_DRV_RESOLUTION_T   eResolution;        //                                    
    VAL_UINT32_T            u4Width;            //                               
    VAL_UINT32_T            u4Height;           //                                
    VAL_UINT32_T            u4Bitrate;          //                                 
    VAL_UINT32_T            u4FrameRate;        //                                               
} VENC_DRV_QUERY_VIDEO_FORMAT_T;

/* 
                 
                                    
                   
                                                         
 */
typedef VENC_DRV_QUERY_VIDEO_FORMAT_T   *P_VENC_DRV_QUERY_VIDEO_FORMAT_T;


/* 
                 
                                   
                   
                                                            
 */
typedef struct __VENC_DRV_QUERY_INPUT_BUF_LIMIT
{
    VENC_DRV_VIDEO_FORMAT_T eVideoFormat;       //                     
    VAL_UINT32_T            u4Width;            //                    
    VAL_UINT32_T            u4Height;           //                     
    VAL_UINT32_T            u4Stride;           //                     
    VAL_UINT32_T            u4SliceHeight;      //                          
    VENC_DRV_SCENARIO_T     eScenario;          //                      
} VENC_DRV_QUERY_INPUT_BUF_LIMIT;


/* 
                 
                         
                   
                                                                                                                    
 */
typedef struct __VENC_DRV_PARAM_ENC_T
{
    VENC_DRV_YUV_FORMAT_T   eVEncFormat;        //                      
    VAL_UINT32_T            u4Profile;          //                   
    VAL_UINT32_T            u4Level;            //                 
    VAL_UINT32_T            u4Width;            //                       
    VAL_UINT32_T            u4Height;           //                        
    VAL_UINT32_T            u4BufWidth;         //                        
    VAL_UINT32_T            u4BufHeight;        //                        
    VAL_UINT32_T            u4NumPFrm;          //                                                      
    VAL_UINT32_T            u4NumBFrm;          //                                                              
    VENC_DRV_FRAME_RATE_T   eFrameRate;         //                      
    VAL_BOOL_T              fgInterlace;        //                             
    VAL_VOID_T              *pvExtraEnc;        //                                                
    VAL_MEMORY_T            rExtraEncMem;       //                                     
    VAL_BOOL_T              fgUseMCI;           //                   
    VAL_BOOL_T              fgMultiSlice;       //                                      
} VENC_DRV_PARAM_ENC_T;

/* 
                 
                           
                   
                                                
 */
typedef VENC_DRV_PARAM_ENC_T    *P_VENC_DRV_PARAM_ENC_T;


/* 
                 
                               
                   
                                                                                                                    
 */
typedef struct __VENC_DRV_PARAM_ENC_EXTRA_T
{
    VAL_UINT32_T            u4IntraFrameRate;   //                            
    VAL_UINT32_T            u4BitRate;          //                        
    VAL_UINT32_T            u4FrameRateQ16;     //                                    
    VAL_UINT32_T            u4UseMBAFF;         //                     
} VENC_DRV_PARAM_ENC_EXTRA_T;

/* 
                 
                                 
                   
                                                      
 */
typedef VENC_DRV_PARAM_ENC_EXTRA_T     *pVENC_DRV_PARAM_ENC_EXTRA_T;


#define VENC_DRV_VDO_PROP_LIST_MAX      (64)

/* 
                 
                              
                   
                                                                                      
 */
typedef struct __VENC_DRV_VIDEO_PROPERTY_T
{
    VENC_DRV_VIDEO_FORMAT_T     eVideoFormat;
    VAL_UINT32_T    u4Width;
    VAL_UINT32_T    u4Height;
    VAL_UINT32_T    u4FrameRate;
    VAL_UINT32_T    u4BitRate;    //                     
    VAL_BOOL_T      fgPropIsValid;
} VENC_DRV_VIDEO_PROPERTY_T;

/* 
                 
                                
                   
                                                     
 */
typedef VENC_DRV_VIDEO_PROPERTY_T     *P_VENC_DRV_VIDEO_PROPERTY_T;


/* 
                 
                         
                   
                                                                                                             
 */
typedef struct __VENC_DRV_TIMESTAMP_T
{
    VAL_UINT32_T    u4TimeStamp[2];     //                             
} VENC_DRV_TIMESTAMP_T;

/* 
                 
                           
                   
                                                
 */
typedef VENC_DRV_TIMESTAMP_T     *P_VENC_DRV_TIMESTAMP_T;


/* 
                 
                         
                   
                                                                           
 */
typedef struct __VENC_DRV_EIS_INPUT_T
{
    VAL_UINT32_T    u4X;    //                            
    VAL_UINT32_T    u4Y;    //                            
} VENC_DRV_EIS_INPUT_T;

/* 
                 
                           
                   
                                                
 */
typedef VENC_DRV_EIS_INPUT_T     *P_VENC_DRV_EIS_INPUT_T;


/* 
                 
                             
                   
                                                                                      
 */
typedef struct __VENC_DRV_PARAM_FRM_BUF_T
{
    VAL_MEM_ADDR_T          rFrmBufAddr;        //                            
    VAL_MEM_ADDR_T          rCoarseAddr;        //                      
    VENC_DRV_TIMESTAMP_T    rTimeStamp;         //                             
    VENC_DRV_EIS_INPUT_T    rEISInput;          //                       
    VAL_UINT32_T            rSecMemHandle;      //                                          
} VENC_DRV_PARAM_FRM_BUF_T;

/* 
                 
                               
                   
                                                    
 */
typedef VENC_DRV_PARAM_FRM_BUF_T     *P_VENC_DRV_PARAM_FRM_BUF_T;


/* 
                 
                            
                   
                                                                           
                       
 */
typedef struct __VENC_DRV_PARAM_BS_BUF_T
{
    VAL_MEM_ADDR_T          rBSAddr;        //                                
    VAL_ULONG_T             u4BSStartVA;    //                                    
    VAL_ULONG_T             u4BSSize;       //                                                  
    VENC_DRV_TIMESTAMP_T    rTimeStamp;     //                              
    VAL_UINT32_T            rSecMemHandle;  //                                          
} VENC_DRV_PARAM_BS_BUF_T;

/* 
                 
                              
                   
                                                   
 */
typedef VENC_DRV_PARAM_BS_BUF_T     *P_VENC_DRV_PARAM_BS_BUF_T;


/* 
                 
                           
                   
                                                                                              
 */
typedef struct __VENC_DRV_DONE_RESULT_T
{
    VENC_DRV_MESSAGE_T          eMessage;           //                                               
    P_VENC_DRV_PARAM_BS_BUF_T   prBSBuf;            //                              
    P_VENC_DRV_PARAM_FRM_BUF_T  prFrmBuf;           //                                                                                              
    VAL_BOOL_T                  fgIsKeyFrm;         //                                   
    VAL_UINT32_T                u4HWEncodeTime;     //                       
} VENC_DRV_DONE_RESULT_T;

/* 
                 
                             
                   
                                                  
 */
typedef VENC_DRV_DONE_RESULT_T     *P_VENC_DRV_DONE_RESULT_T;


/* 
                 
                        
                   
                                                                                            
 */
typedef struct __VENC_DRV_PROPERTY_T
{
    VAL_UINT32_T    u4BufAlign;             //                                     
    VAL_UINT32_T    u4BufUnitSize;          //                                                                         
    VAL_UINT32_T    u4ExtraBufSize;         //                                           
    VAL_BOOL_T      fgOutputRingBuf;        //                              
    VAL_BOOL_T      fgCoarseMESupport;      //                                 
    VAL_BOOL_T      fgEISSupport;           //                    
} VENC_DRV_PROPERTY_T;

/* 
                 
                          
                   
                                               
 */
typedef VENC_DRV_PROPERTY_T     *P_VENC_DRV_PROPERTY_T;


/* 
                
                            
                   
                                
         
                                                                        
         
                                                        
         
                                                         
               
                                                                                                    
 */
VENC_DRV_MRESULT_T  eVEncDrvQueryCapability(
    VENC_DRV_QUERY_TYPE_T a_eType,
    VAL_VOID_T *a_pvInParam,
    VAL_VOID_T *a_pvOutParam
);


/* 
                
                   
                   
                             
         
                                                   
         
                                                                       
               
                                                                                                 
 */
VENC_DRV_MRESULT_T  eVEncDrvCreate(
    VAL_HANDLE_T *a_phHandle,
    VENC_DRV_VIDEO_FORMAT_T a_eVideoFormat
);


/* 
                
                    
                   
                              
         
                                                   
         
                                                                       
               
                                                                                                 
 */
VENC_DRV_MRESULT_T  eVEncDrvRelease(
    VAL_HANDLE_T a_hHandle,
    VENC_DRV_VIDEO_FORMAT_T a_eVideoFormat
);


/* 
                
                 
                   
                                                           
         
                                                   
               
                                                                                                 
 */
VENC_DRV_MRESULT_T  eVEncDrvInit(
    VAL_HANDLE_T a_hHandle
);

/* 
                
                   
                   
                                                            
         
                                                   
               
                                                                                                 
 */
VENC_DRV_MRESULT_T  eVEncDrvDeInit(
    VAL_HANDLE_T a_hHandle
);


/* 
                
                     
                   
                            
         
                                                   
         
                                                                   
         
                                                     
         
                                                      
               
                                                                                                 
 */
VENC_DRV_MRESULT_T  eVEncDrvSetParam(
    VAL_HANDLE_T a_hHandle,
    VENC_DRV_SET_TYPE_T a_eType,
    VAL_VOID_T *a_pvInParam,
    VAL_VOID_T *a_pvOutParam
);


/* 
                
                     
                   
                              
         
                                                   
         
                                                                   
         
                                                     
         
                                                      
               
                                                                                                 
 */
VENC_DRV_MRESULT_T  eVEncDrvGetParam(
    VAL_HANDLE_T a_hHandle,
    VENC_DRV_GET_TYPE_T a_eType,
    VAL_VOID_T *a_pvInParam,
    VAL_VOID_T *a_pvOutParam
);


/* 
                
                   
                   
                 
         
                                                   
         
                                                                    
         
                                                                                                
         
                                                                                                   
         
                                                                                         
               
                                                                                                 
 */
VENC_DRV_MRESULT_T  eVEncDrvEncode(
    VAL_HANDLE_T a_hHandle,
    VENC_DRV_START_OPT_T a_eOpt,
    VENC_DRV_PARAM_FRM_BUF_T *a_prFrmBuf,
    VENC_DRV_PARAM_BS_BUF_T *a_prBSBuf,
    VENC_DRV_DONE_RESULT_T *a_prResult
);


#ifdef __cplusplus
}
#endif

#endif //                               
