
#ifndef VCODEC_IF_V2_H
#define VCODEC_IF_V2_H

/*                                                  
 
                                           
 
                                                  */


#ifndef TRUE
#define TRUE 1
#elif TRUE != 1
#error TRUE is not equal to 1
#endif

#ifndef FALSE
#define FALSE 0
#elif FALSE != 0
#error FALSE is not equal to 0
#endif
#define IN
#define OUT
#define INOUT
typedef void *HANDLE;
#ifdef WIN32
typedef unsigned __int64    UINT64;
#else
typedef unsigned long long  UINT64;
#endif

typedef enum
{
    VCODEC_DECODER,
    VCODEC_ENCODER,
    NUM_OF_CODEC_TYPE
} VCODEC_CODEC_TYPE_T;

typedef struct
{
    unsigned int u4YStride;
    unsigned int u4UVStride;
} VCODEC_YUV_STRIDE_T;

typedef enum
{
    VCODEC_COLOR_FORMAT_YUV420,
    VCODEC_COLOR_FORMAT_YV12,
} VCODEC_COLOR_FORMAT_T;

typedef struct
{
    unsigned int                MaxSupportWidth;
    unsigned int                MaxSupportHeight;
    unsigned int                eFlag;                   //                       
    unsigned int                ExternalMEMSize;
    int                         OutBufferNum;            //         
    VCODEC_YUV_STRIDE_T         stride;
    VCODEC_COLOR_FORMAT_T       eColorFormat;
    void *PrivateData[4];
} VCODEC_OPEN_SETTING_T;

typedef struct
{
    unsigned int MaxSupportWidthForYUV420_BP;
    unsigned int MaxSupportHeightForYUV420_BP;
    unsigned int MaxSupportWidthForYUV420_MPHP;
    unsigned int MaxSupportHeightForYUV420_MPHP;
    unsigned int ExternalMEMSize;
    unsigned int DPBSize;
} H264_DEC_CUSTOM_SETTING_T;

typedef struct
{
    unsigned int MaxSupportWidthForYUV420_MPHP;
    unsigned int MaxSupportHeightForYUV420_MPHP;
    unsigned int NormalMaxWidthForYUV420_MPHP;
    unsigned int NormalMaxHeightForYUV420_MPHP;
    unsigned int u4dpbSizes;
    void *reserved[4];
} H264_DECODER_PRIVATE_PARAM_T;
/*
              
 
                                             
                                              
                                              
                                               
                                 
                             

              
 
                                        
                                         
                                                                 
                               
                       
                           
*/

typedef enum
{
    VCODEC_FRAMETYPE_I,
    VCODEC_FRAMETYPE_NS_I, //                          
    VCODEC_FRAMETYPE_P,
    VCODEC_FRAMETYPE_B,
    VCODEC_HEADER,
    VCODEC_FRM_DROPPED,
    VCODEC_UNKNOWN_TYPE,
    NUM_OF_FRAME_TYPE
} VCODEC_FRAME_TYPE_T;



typedef enum
{
    VA_NON_CACHED   = 0x0,
    VA_CACHED       = 0x1,
} VCODEC_BUFFER_ATTRIBUTE_T;

typedef enum
{
    VCODEC_BUFFER_CACHEABLE           = 0,
    VCODEC_BUFFER_NON_CACHEABLE     = 1,
    VCODEC_BUFFER_MCI_SHARABLE      = 2
} VCODEC_MEMORY_TYPE_T;
typedef struct
{
    unsigned int u4InternalSize;
    unsigned int u4ExternalSize;
} VCODEC_MEMORY_SIZE_T;

typedef struct
{
    unsigned char           *pu1Buffer_VA;
    unsigned char           *pu1Buffer_PA;
    unsigned int             eBufferStatus;//                         
} VCODEC_BUFFER_T;

typedef enum
{
    VCODEC_DEC_ERROR_NONE,
    VCODEC_DEC_ERROR_DECODE_ERROR,
    VCODEC_DEC_ERROR_ASSERT_FAIL,
    VCODEC_DEC_ERROR_FATAL_ERROR,
    VCODEC_DEC_ERROR_NOT_SUPPORT,
    VCODEC_DEC_ERROR_NOT_ENOUGH_MEM,
    VCODEC_DEC_ERROR_PAYLOAD_DATA_ERROR,
    VCODEC_DEC_ERROR_OAL_CHECK_VERSION_FAIL,
    VCODEC_DEC_ERROR_DIMENSION_CHANGE,
    NUM_OF_DEC_ERROR_TYPE
} VCODEC_DEC_ERROR_T;


typedef enum
{
    CUSTOM_SETTING, //              
    BEST_QUALITY,    //             
    FAVOR_QUALITY,   //                                            
    FAVOR_FLUENCY,   //                                            
    BEST_FLUENCY,    //         
    NUM_OF_DECODE_MODE,
} VCODEC_DEC_DECODE_MODE_T;



typedef enum
{
    VCODEC_DEC_PARAM_EOF,
    VCODEC_DEC_PARAM_QUERY_RESOLUTION_AHEAD,
    VCODEC_DEC_PARAM_QUERY_RESOLUTION,
    VCODEC_DEC_PARAM_QUERY_PREDICATION_TIME,
    VCODEC_DEC_PARAM_MEMORY_REQUIREMENT,
    VCODEC_DEC_PARAM_CAPABILITY,
    VCODEC_DEC_PARAM_NOT_BUFFERING,
    VCODEC_DEC_PARAM_BUFFERING,
    VCODEC_DEC_PARAM_BITRATE,
    VCODEC_DEC_PARAM_FRAMERATE,
    VCODEC_DEC_PARAM_EXCLUDE_BUF_NUM,
    VCODEC_DEC_PARAM_NO_OUTPUT_REORDERING,
    VCODEC_DEC_PARAM_FLUSH_BUFFER,
    VCODEC_DEC_PARAM_SET_DECRYPTION_MODE,
    VCODEC_DEC_PARAM_SET_DECODE_MODE,
    VCODEC_DEC_PARAM_GET_DECODE_MODE,
    VCODEC_DEC_PARAM_CTRL_VOS,
    VCODEC_DEC_PARAM_GET_SBSFLAG,
    VCODEC_DEC_PARAM_CONCEAL_LEVEL,
    VCODEC_DEC_PARAM_NUM_OF_HW_CTRL_THID,
    VCODEC_DEC_PARAM_GET_REG_HW_CTRL_THID,                 /*                                                                                  */
    VCODEC_DEC_PARAM_SET_COLOR_FORMAT,
    //                                      
    VCODEC_DEC_PARAM_SET_AVAILABLE_CPU_NUM,
    VCODEC_DEC_PARAM_SET_MCI,                             //                               
    NUM_OF_DEC_PARAM_TYPE,
} VCODEC_DEC_PARAM_TYPE_T;

typedef enum
{
    VCODEC_DEC_DISPLAY_CONCEALED_FRAME_DURING_PLAYBACK  = 0x01,
    VCODEC_DEC_DISPLAY_CONCEALED_FRAME_BEFORE_FIRST_I   = 0X02,
    VCODEC_DEC_DISPLAY_CONCEALED_FRAME_AFTER_FIRST_I   = 0X04,
    NUM_OF_DEC_CONCEAL_LEVEL_TYPE,
} VCODEC_DEC_CONCEAL_LEVEL_TYPE_T;
typedef enum
{
    //                                                                                    
    //                                                                                    
    //                                                                                                         
    //                                                                                                                
    VCODEC_DEC_QUERY_INFO_OAL_FUNCTION,                 //                          
    VCODEC_DEC_QUERY_INFO_CURRENT_TIME,                 //                 
    VCODEC_DEC_QUERY_INFO_LAST_VIDEO_TIME,                 //                          
    //                                                                                  
    NUM_OF_QUERY_INFO_TYPE
} VCODEC_DEC_QUERY_INFO_TYPE_T;

typedef struct
{
    VCODEC_COLOR_FORMAT_T eColorFormat;
    unsigned int          u4MaxWidth;
    unsigned int          u4MaxHeight;
    unsigned int          MaxVideoCodingResolution;
} VCODEC_ENC_GENERAL_SETTING_T;

typedef struct
{
    VCODEC_COLOR_FORMAT_T eColorFormat;
    unsigned int         u4MaxWidth;
    unsigned int         u4MaxHeight;
    unsigned int         MaxVideoCodingResolution;
    unsigned int         complexityIndex;
} VCODEC_ENC_MPEG4_SETTING_T;

typedef union
{
    VCODEC_ENC_MPEG4_SETTING_T     rMPEG4;
    VCODEC_ENC_GENERAL_SETTING_T   rVT;
    VCODEC_ENC_GENERAL_SETTING_T   rH264;
    VCODEC_ENC_GENERAL_SETTING_T   rHEVC;
    VCODEC_ENC_GENERAL_SETTING_T   rVP9;
    VCODEC_ENC_GENERAL_SETTING_T   rVP8;
} VCODEC_ENC_SETTING_T;

typedef struct
{
    unsigned char *pu1ParamStream;
    unsigned int  u4ParamLength;
    unsigned int  u4Width;
    unsigned int  u4Height;
} VCODEC_DEC_QUERY_FRAME_SIZE_TYPE_T;

typedef enum
{
    DISPLAY_CURRENT,  //               
    REPEAT_LAST,      //                                
    NOT_DISPLAY,      //                        
    LAST_FRAME,       //    
    NO_PIC,           //          
    NOT_USED,         //                  
    DISPLAY_CURRENT_INTERLACE,    //                 
    NUM_OF_DISPLAY_FRAME_STATUS
} VCODEC_DEC_DISPLAY_FRAME_STATUS;

typedef struct
{
    int                      i4AspectRatioWidth;  //                  
    int                      i4AspectRatioHeight; //                   
    //                                                 
    //                                                  
    //                                                  
    //                                                
    void                     *prExtra;
} VCODEC_DEC_PRIVATE_OUTPUT_EXTRA_T;

typedef struct
{
    UINT64                             u8TimeStamp;
    int                                fgUpdateTime;   //                 
    unsigned short                     u2FrameWidth;   //               
    unsigned short                     u2FrameHeight;  //               
    unsigned short                     u2ClipTop;
    unsigned short                     u2ClipBottom;
    unsigned short                     u2ClipLeft;
    unsigned short                     u2ClipRight;
    VCODEC_FRAME_TYPE_T                eFrameType;
    VCODEC_BUFFER_T                    rYStartAddr;   /*                                                     */
    VCODEC_BUFFER_T                    rUStartAddr;
    VCODEC_BUFFER_T                    rVStartAddr;
    VCODEC_DEC_DISPLAY_FRAME_STATUS    eDisplayFrameStatus;
    void                               *prExtra;
} VCODEC_DEC_PRIVATE_OUTPUT_T;



typedef void VCODEC_DEC_INPUT_DATA_T;

typedef enum
{
    INPUT_FLAG_STREAM_DATA_TYPE     =  0x01,  /*                                                                */
    INPUT_FLAG_STARTTIME            =  0x02,  /*                                */
    INPUT_FLAG_DECODEONLY           =  0x04,  /*      */
    INPUT_FLAG_PARAMETERSET         =  0x08,  /*                                                                      */
    INPUT_FLAG_CUSTOM_SETTING       =  0x10,  /*                                 */
    INPUT_FLAG_DECODE_INTRA_ONLY    =  0x20,  /*                         */
    INPUT_FLAG_OPENAPI              =  0x40,  /*         */
    INPUT_FLAG_DECODE_MODE          =  0x80,  /*             */
    INPUT_FLAG_LEGACY_MODE          =  0x100, /*             */
    INPUT_FLAG_MAX_DEC
} VCODEC_DEC_INPUT_FLAG_T;

typedef struct
{
    UINT64                      u8TimeStamp;
    unsigned int                eFlags;           //                        
    VCODEC_DEC_INPUT_DATA_T     *prInputData;     //                                                                                                            
    VCODEC_BUFFER_T             *prBuffer;        //                   
    unsigned int                 u4BuffSize;      //                        
    void                        *prExtra;
} VCODEC_DEC_INPUT_T;






typedef struct
{
    unsigned int    u4Width;                //                       
    unsigned int    u4Height;               //                       
    unsigned short  u2ClipTop;
    unsigned short  u2ClipBottom;
    unsigned short  u2ClipLeft;
    unsigned short  u2ClipRight;
    unsigned int    u4Offset;               //                                  
    unsigned int    u4ReduceLength;         //                                    
    unsigned char   u1Alignment;            //                            
    VCODEC_MEMORY_TYPE_T rYUVBUfferMemType; //                      
    unsigned int    u4MaxBufferNum;
    unsigned int    u4ExtraBufferNum;
    void            *prExtra;
} VCODEC_DEC_OUTPUT_BUFFER_PARAM_T;

typedef struct
{
    VCODEC_MEMORY_TYPE_T rBitStreamBufferMemType; //                            
    unsigned int    u4MaxBufferNum;
    void *PrivateData[4];
} VCODEC_DEC_INPUT_BUFFER_PARAM_T;

typedef struct
{
    VCODEC_BUFFER_T rYBuffer;
    VCODEC_BUFFER_T rUBuffer;
    VCODEC_BUFFER_T rVBuffer;
} VCODEC_DEC_INPUT_YUV_INFO_T;

//             
#define MAX_BITSTREAM_BUFFER_INFO_NUM 10
#define MAX_REF_FREE_YUV_BUFFER_NUM 18
typedef struct
{
    VCODEC_BUFFER_T       *prRetBitsBuf;    //                   
    unsigned int          u4ReturnInputCnt;
    VCODEC_BUFFER_T       rReturnInput[MAX_BITSTREAM_BUFFER_INFO_NUM];
    unsigned int          u4RefFreeYUVBufCnt;
    VCODEC_DEC_INPUT_YUV_INFO_T       parRefFreeYUVBuf[MAX_REF_FREE_YUV_BUFFER_NUM];
} VCODEC_DEC_OUTPUT_PARAM_T;
//              

typedef struct
{
    unsigned int u4SupportWidth;
    unsigned int u4SupportHeight;
    unsigned int u4SupportResolution;
    unsigned int u4SupportProfile;
    unsigned int u4SupportLevel;
} VCODEC_DEC_CAPABILITY_T;

typedef struct
{
    void (*pfnMalloc)(IN HANDLE                             /*      */,
                      IN unsigned int                       /*        */,
                      IN unsigned int                       /*           */,
                      IN VCODEC_MEMORY_TYPE_T                     /*             */,
                      OUT VCODEC_BUFFER_T *                  /*       */
                     );  /*                                      */

    void (*pfnIntMalloc)(IN HANDLE                             /*      */,
                         IN unsigned int                       /*        */,
                         IN unsigned int                       /*           */,
                         OUT VCODEC_BUFFER_T *                 /*             */
                        );

    void (*pfnFree)(IN HANDLE                             /*      */,
                    IN VCODEC_BUFFER_T *                   /*       */
                   );  /*                              */

    void (*pfnIntFree)(IN HANDLE                             /*      */,
                       IN VCODEC_BUFFER_T *                 /*             */
                      );

    VCODEC_DEC_ERROR_T(*pfnSetYUVBuffer)(IN HANDLE                              /*      */,
                                         IN VCODEC_DEC_OUTPUT_BUFFER_PARAM_T *  /*            */
                                        );

    VCODEC_DEC_ERROR_T(*pfnGetYUVBuffer)(IN  HANDLE                             /*      */,
                                         OUT VCODEC_DEC_INPUT_YUV_INFO_T *      /*          */
                                        );

    void (*pfnRefFreeYUVBuffer)(IN HANDLE                             /*      */,
                                IN VCODEC_DEC_INPUT_YUV_INFO_T *      /*          */
                               );

    VCODEC_DEC_ERROR_T(*pfnQueryInfo)(IN HANDLE                              /*      */,
                                      IN VCODEC_DEC_QUERY_INFO_TYPE_T        /*         */,
                                      OUT void *                           /*            */ /* */
                                     );

    void (*pfnReturnBitstream)(IN HANDLE                             /*      */,
                               IN VCODEC_BUFFER_T  *                  /*          */  /*                      */,
                               IN unsigned int                          /*            */ /*                         */
                              );

} VCODEC_DEC_CALLBACK_T;


//             
typedef struct
{
    VCODEC_DEC_INPUT_T              *prInputData;
    VCODEC_DEC_INPUT_YUV_INFO_T     *prYUVBufAddr;   //         
} VIDEO_DECODER_INPUT_NC_T;



typedef struct
{
    VCODEC_DEC_ERROR_T(*pfnGetCodecRequired)(IN    VCODEC_DEC_INPUT_T *                 /*         */ ,
                                             OUT   VCODEC_MEMORY_SIZE_T *               /*           */,
                                             OUT   VCODEC_DEC_OUTPUT_BUFFER_PARAM_T *   /*                  */,
                                             OUT   VCODEC_DEC_INPUT_BUFFER_PARAM_T *    /*                      */,
                                             INOUT void *                               /*         */
                                            );


    /*                                                       
                              
                                                           */
    VCODEC_DEC_ERROR_T(*pfnOpen)(IN HANDLE,                                             /*      */
                                 IN VCODEC_DEC_CALLBACK_T *,        /*             */
                                 IN void *,                            /*              */
                                 OUT HANDLE * ,                         /*        */
                                 INOUT void *                                  /*         */
                                );

    VCODEC_DEC_ERROR_T(*pfnClose)(IN HANDLE   ,                                          /*        */
                                  INOUT void *                           /*         */
                                 );

    VCODEC_DEC_ERROR_T(*pfnInit)(IN HANDLE   ,                                           /*        */
                                 INOUT void *                          /*         */
                                );

    /*                                                       
                              
                                                           */
    VCODEC_DEC_ERROR_T(*pfnDeInit)(IN HANDLE,                                           /*        */
                                   IN HANDLE,                            /*       */
                                   OUT VCODEC_DEC_OUTPUT_PARAM_T ** ,     /*                 */
                                   INOUT void *                                  /*         */
                                  );

    VCODEC_DEC_ERROR_T(*pfnGetParameter)(IN HANDLE,                                     /*        */
                                         IN VCODEC_DEC_PARAM_TYPE_T,
                                         INOUT void *                          /*             */
                                        );
    /*                                                       
                              
                                                           */
    VCODEC_DEC_ERROR_T(*pfnSetParameter)(IN HANDLE,                                     /*        */
                                         IN HANDLE,                            /*       */
                                         IN VCODEC_DEC_PARAM_TYPE_T,           /*            */
                                         IN void *  ,                           /*             */
                                         INOUT void *                          /*         */
                                        );

    /*                                                       
                              
                                                           */
    VCODEC_DEC_ERROR_T(*pfnDecodeOneUnit)(IN HANDLE,                                    /*        */
                                          IN HANDLE,                            /*       */
                                          IN VIDEO_DECODER_INPUT_NC_T *,        /*         */
                                          OUT VCODEC_DEC_OUTPUT_PARAM_T **  ,    /*                 */
                                          INOUT void *                          /*         */
                                         );

    /*                                                       
                              
                                                           */
    VCODEC_DEC_ERROR_T(*pfnGetNextDisplay)(IN HANDLE,                                   /*        */
                                           IN HANDLE,                            /*       */
                                           OUT VCODEC_DEC_PRIVATE_OUTPUT_T * ,    /*              */
                                           INOUT void *                          /*         */
                                          );
} VIDEO_DEC_API_T;
//              







typedef struct
{

    VCODEC_DEC_ERROR_T(*pfnGetCodecRequired)(IN    VCODEC_DEC_INPUT_T *                 /*         */ ,
                                             OUT   VCODEC_MEMORY_SIZE_T *               /*           */,
                                             OUT   VCODEC_DEC_OUTPUT_BUFFER_PARAM_T *      /*                  */,
                                             OUT   VCODEC_DEC_INPUT_BUFFER_PARAM_T *       /*                     */,
                                             INOUT void *                              /*         */
                                            );


    VCODEC_DEC_ERROR_T(*pfnOpen)(IN HANDLE                             /*      */,
                                 IN VCODEC_DEC_CALLBACK_T *            /*             */,
                                 IN void *                             /*               */,
                                 OUT HANDLE *                          /*        */  ,
                                 INOUT void *                          /*         */
                                );

    VCODEC_DEC_ERROR_T(*pfnClose)(IN HANDLE                             /*        */,
                                  INOUT void *                          /*         */
                                 );

    VCODEC_DEC_ERROR_T(*pfnInit)(IN HANDLE                             /*        */,
                                 INOUT void *                          /*         */
                                );

    VCODEC_DEC_ERROR_T(*pfnDeInit)(IN HANDLE                             /*        */,
                                   INOUT void *                          /*         */
                                  );

    VCODEC_DEC_ERROR_T(*pfnGetParameter)(IN HANDLE                             /*        */,
                                         IN VCODEC_DEC_PARAM_TYPE_T            /*            */,
                                         INOUT void *                          /*             */,
                                         INOUT void *                          /*         */
                                        );

    VCODEC_DEC_ERROR_T(*pfnSetParameter)(IN HANDLE                             /*        */,
                                         IN VCODEC_DEC_PARAM_TYPE_T            /*            */,
                                         IN void *                             /*             */,
                                         INOUT void *                          /*         */
                                        );

    VCODEC_DEC_ERROR_T(*pfnDecodeOneUnit)(IN    HANDLE                         /*        */,
                                          IN    VCODEC_DEC_INPUT_T *           /*         */,
                                          INOUT void *                         /*         */
                                         );

    VCODEC_DEC_ERROR_T(*pfnGetNextDisplay)(IN    HANDLE                         /*        */,
                                           OUT   VCODEC_DEC_PRIVATE_OUTPUT_T *  /*              */,
                                           INOUT void *                         /*         */
                                          );




} VCODEC_DEC_API_T;

typedef struct
{
    VIDEO_DEC_API_T  rVideoDecAPI;
    VCODEC_DEC_API_T *pfnVcodecDecAPI;
    VCODEC_DEC_INPUT_YUV_INFO_T rGetYUVBuf;
    VCODEC_BUFFER_T rRetBitsBuf;
    VCODEC_DEC_OUTPUT_PARAM_T rDecoderOutputParam;
    unsigned int fgTookYUVBuff;
    HANDLE hDriver;
} VIDEO_WRAP_HANDLE_T;


#define VCODEC_ENC_MAX_PKTS_IN_SET        99
#define VCODEC_ENC_MAX_NALS_IN_SET        10

typedef struct
{
    void        *u4Addr;
    unsigned int u4Size;
} VCODEC_ENC_CODEC_PACKET_INFO_T;


//                                                                   
//                     
//                              
typedef struct
{
    unsigned int                                    u4NumOfPkts;
    VCODEC_ENC_CODEC_PACKET_INFO_T  arPktInfo[VCODEC_ENC_MAX_PKTS_IN_SET];
} VCODEC_ENC_PACKET_SET_T;


typedef struct
{
    int          u4NalUnitType;
    void        *u4Addr;        //          
    unsigned int u4Size;        //          
} VCODEC_ENC_CODEC_NAL_INFO_T;

typedef struct
{
    unsigned int                    u4NumOfNals;
    VCODEC_ENC_CODEC_NAL_INFO_T     arNalInfo[VCODEC_ENC_MAX_NALS_IN_SET];
} VCODEC_ENC_NAL_SET_T;

typedef enum
{
    MPEG4_RECODER,
    MPEG4_VT,
    H264_RECODER,
    H264_VT,
    NUM_OF_ENC_CODEC_TYPE
} VCODEC_ENC_CODEC_TYPE_T;

typedef struct
{
    VCODEC_FRAME_TYPE_T      eFrameType;
    VCODEC_BUFFER_T          rBitstreamAddr;          //                                                               
    unsigned int             u4BitstreamLength;
    int                      fgEndOfFrame;
    void                     *prChassis;
    VCODEC_ENC_CODEC_TYPE_T   eCodecType;
    UINT64                    u8TimeStamp;
    void                      *reserved[4];
} VCODEC_ENC_GENERAL_OUTPUT_T;

typedef struct
{
    VCODEC_BUFFER_T rStartAddr;
    VCODEC_BUFFER_T rEndAddr;
    VCODEC_BUFFER_T rWriteAddr;
    VCODEC_BUFFER_T rReadAddr;
    unsigned int    u4BufferLength;
} VCODEC_ENC_BUFFER_INFO_T;


typedef enum
{
    INPUT_FLAG_YUVBUFFER          = 0x01,
    INPUT_FLAG_NO_INPUT           = 0x02,
    INPUT_FLAG_NO_MORE_INPUT    = 0x03,
    INPUT_FLAG_MAX_ENC
} VCODEC_ENC_INPUT_FLAG_T;

typedef struct
{
    VCODEC_BUFFER_T rYUVBuffer;
    unsigned int u4Length;
} VCODEC_ENC_INPUT_INFO_T;

typedef struct
{
    VCODEC_BUFFER_T rYBuffer;
    VCODEC_BUFFER_T rUBuffer;
    VCODEC_BUFFER_T rVBuffer;
    unsigned int u4Length;
} VCODEC_ENC_INPUT_YUV_INFO_T;



typedef struct
{
    UINT64                 u8TimeStamp;
    VCODEC_ENC_INPUT_FLAG_T  eFlags;
    VCODEC_ENC_INPUT_INFO_T  rInput;
} VCODEC_ENC_INPUT_PARAM_T;

typedef struct
{
    UINT64                       u8TimeStamp;
    VCODEC_ENC_INPUT_FLAG_T      eFlags;
    VCODEC_ENC_INPUT_YUV_INFO_T rInput;
    void *prExtra;
} VCODEC_ENC_INPUT_YUV_PARAM_T;

typedef struct
{
    VCODEC_BUFFER_T  rWp;
    int            fgSliceContained;
} VCODEC_ENC_UPDATE_WP_INTO_T;

typedef enum
{
    VCODEC_ENC_ERROR_NONE,
    VCODEC_ENC_ERROR,
    VCODEC_ENC_ASSERT_FAIL,
    VCODEC_ENC_BS_BUFFER_NOT_ENOUGH,
    VCODEC_ENC_INPUT_REJECT,
    VCODEC_ENC_PARAM_NOT_SUPPORT,
    NUM_OF_ENC_ERROR_TYPE
} VCODEC_ENC_ERROR_T;


typedef enum
{
    VCODEC_ENC_PARAM_MEMORY_REQUIREMENT,      //                                               
    VCODEC_ENC_PARAM_BITSTREAM_IN_CACHE,      //                                                                                                 
    VCODEC_ENC_PARAM_FRM_BUFFER_ALIGNMENT,    //                                                    
    VCODEC_ENC_PARAM_HOLD_RES_TILL_RELEASE_FRM,
    VCODEC_ENC_PARAM_IS_BLOCKBASED_YUV,
    VCODEC_ENC_PARAM_DECODER_CONFIGURATION_RECORD,
    VCODEC_ENC_PARAM_IF_ADAPTOR_MODIFY_TIMESTAMP,
    VCODEC_ENC_PARAM_WIDTH,
    VCODEC_ENC_PARAM_HEIGHT,
    VCODEC_ENC_PARAM_BITRATE,
    VCODEC_ENC_PARAM_FRAME_RATE,
    VCODEC_ENC_PARAM_FRAME_RATE_NUM,
    VCODEC_ENC_PARAM_FRAME_RATE_DEN,
    VCDOEC_ENC_PARAM_AUD,
    VCODEC_ENC_PARAM_REPEAD_HEADERS,
    VCODEC_ENC_PARAM_ANNEXB,
    VCODEC_ENC_PARAM_GEN_HEADER_FRM_RATE,
    VCODEC_ENC_PARAM_SHORT_HEADER,
    VCODEC_ENC_PARAM_SYNC_INTERVAL,
    VCODEC_ENC_PARAM_MAX_PKG_SIZE,
    VCODEC_ENC_PARAM_FORCE_ENCODE_I,
    VCODEC_ENC_PARAM_QUALITY,
    VCODEC_ENC_PARAM_SCENARIO,
    VCODEC_ENC_PARAM_CODEC_TYPE,
    VCODEC_ENC_PARAM_VT_RESTART,
    VCODEC_ENC_PARAM_ROTATE,
    VCODEC_ENC_PARAM_SET_CALLBACK,
    VCODEC_ENC_PARAM_SET_NO_MORE_INPUT,
    VCODEC_ENC_PARAM_NUM_OF_HW_CTRL_THID,
    VCODEC_ENC_PARAM_GET_REG_HW_CTRL_THID, //                                                                                  
    VCODEC_ENC_PARAM_SET_COLOR_FORMAT,
    VCODEC_ENC_PARAM_SET_YUV_STRIDE_ALIGNMENT,
    VCODEC_ENC_PARAM_SET_AVAILABLE_CPU_NUM,
    VCODEC_ENC_PARAM_SET_MCI,            //                               
    VCODEC_ENC_PARAM_WPP,
    VCODEC_ENC_PARAM_CONSTQ,
    VCODEC_ENC_PARAM_RC_VERSION,
    VCODEC_ENC_PARAM_INIT_QP,
    VCODEC_ENC_PARAM_MAX_QP,
    VCODEC_ENC_PARAM_MIN_QP,
    VCODEC_ENC_PARAM_NUM_OF_SLICE,
    VCODEC_ENC_PARAM_PROFILE,
    VCODEC_ENC_PARAM_LEVEL,
    VCODEC_ENC_PARAM_THREADS,
    VCODEC_ENC_PARAM_VP8_TOKEN_PARTITIONS,
    VCODEC_ENC_PARAM_VP9_ENABLE_TILES,
    VCODEC_ENC_PARAM_VPX_ERR_RESILIENT,
    VCODEC_ENC_PARAM_VPX_NUMBER_OF_LAYERS,
    VCODEC_ENC_PARAM_VPX_MODE,
    VCODEC_ENC_PARAM_VPX_CPU_USED,
    VCODEC_ENC_PARAM_YUV_STRIDE,
    NUM_OF_ENC_PARAM_TYPE
} VCODEC_ENC_PARAM_TYPE_T;

typedef enum
{
    VCODEC_ENC_QUERY_INFO_TOTAL_FRAME_BUFFER,      //                        
    VCODEC_ENC_QUERY_INFO_FRAMES_QUEUED,           //                                   
    VCODEC_ENC_QUERY_INFO_VTBUFFER_FULLNESS_DENOM, //                                  
    VCODEC_ENC_QUERY_INFO_VTBUFFER_FULLNESS_NUM,   //                                
    VCODEC_ENC_QUERY_INFO_INIT_Q,                  //              
    VCODEC_ENC_QUERY_INFO_MIN_QP,                  //              
    VCODEC_ENC_QUERY_INFO_MAX_QP,                  //              
    VCODEC_ENC_QUERY_INFO_INTRA_VOP_RATE,          //                  
    VCODEC_ENC_QUERY_INFO_ALGORITHM,               //              
    VCODEC_ENC_QUERY_INFO_BIT_RATE,                //                  
    VCODEC_ENC_QUERY_INFO_RATE_HARD_LIMIT,         //              
    VCODEC_ENC_QUERY_INFO_RATE_BALANCE,            //              
    VCODEC_ENC_QUERY_INFO_DYNAMIC_RANGE_REDUCTION,
    VCODEC_ENC_QUERY_INFO_IF_CUSTOMER_SET_TABLE,
    VCODEC_ENC_QUERY_INFO_DYNAMIC_RANGE_TABLE,
    VCODEC_ENC_QUERY_INFO_OAL_FUNCTION,
    VCODEC_ENC_QUERY_INFO_ENCODER_FRAME_RATE,      //                      
    VCODEC_ENC_QUERY_INFO_TARGET_COMPLEXITY,       //                      
    VCODEC_ENC_QUERY_INFO_THRESHOLD_AVG_LOW,       //                      
    VCODEC_ENC_QUERY_INFO_THRESHOLD_AVG_HIGH,      //                      
    VCODEC_ENC_QUERY_INFO_THRESHOLD_CUR_LOW,       //                      
    VCODEC_ENC_QUERY_INFO_THRESHOLD_CUR_HIGH,       //                      
    //                                       
    VCODEC_ENC_QUERY_INFO_VPX_CPU_USED,
    VCODEC_ENC_QUERY_INFO_VPX_MODE,
    VCODEC_ENC_QUERY_INFO_FIXED_QP,
    VCODEC_ENC_QUERY_INFO_SCENARIO,
    NUM_OF_ENC_QUERY_INFO_TYPE
} VCODEC_ENC_QUERY_INFO_TYPE_T;

/*                                                                     


                                           

                                                                      */

//           
typedef enum
{
    VCODEC_ENC_ROTATE_0     =   0,
    VCODEC_ENC_ROTATE_90    =   1,
    VCODEC_ENC_ROTATE_180   =   2,
    VCODEC_ENC_ROTATE_270   =   3
} VCODEC_ENC_ROTATE_ANGLE_T;

typedef enum
{
    VCODEC_ENC_QUALITY_NONE,
    VCODEC_ENC_QUALITY_LOW,
    VCODEC_ENC_QUALITY_NORMAL,
    VCODEC_ENC_QUALITY_GOOD,
    VCODEC_ENC_QUALITY_FINE,
    VCODEC_ENC_QUALITY_CUSTOM
} VCODEC_ENC_QUALITY_T;

typedef enum
{
    VCODEC_ENC_CODEC_TYPE_NONE,
    VCODEC_ENC_CODEC_TYPE_MPEG4,
    VCODEC_ENC_CODEC_TYPE_H263,
    VCODEC_ENC_CODEC_TYPE_H264
} VCODEC_ENC_CODEC_T;

typedef struct
{

    void (*pfnMalloc)(IN HANDLE                             /*      */,
                      IN unsigned int                       /*        */,
                      IN unsigned int                       /*           */,
                      IN VCODEC_MEMORY_TYPE_T                     /*             */,
                      OUT VCODEC_BUFFER_T *                  /*       */
                     );   /*                                     */

    void (*pfnIntMalloc)(IN HANDLE                             /*      */,
                         IN unsigned int                       /*        */,
                         IN unsigned int                       /*           */,
                         OUT VCODEC_BUFFER_T *                      /*              */
                        );


    void (*pfnFree)(IN HANDLE                             /*      */,
                    IN VCODEC_BUFFER_T *                   /*       */
                   );  /*                              */


    void (*pfnIntFree)(IN HANDLE                             /*      */,
                       IN VCODEC_BUFFER_T *                   /*              */
                      );

    void (*pfnReleaseYUV)(IN HANDLE                             /*      */,
                          IN VCODEC_BUFFER_T *                   /*          */
                         );

    void (*pfnPaused)(IN HANDLE                              /*      */,
                      IN VCODEC_BUFFER_T *                    /*                */
                     );

    void (*pfnAllocateBitstreamBuffer)(IN HANDLE                              /*      */,
                                       OUT VCODEC_ENC_BUFFER_INFO_T *      /*                */
                                      );

    void (*pfnUpdateBitstreamWP)(IN HANDLE                               /*      */,
                                 IN VCODEC_ENC_UPDATE_WP_INTO_T *     /*                      */
                                );
    VCODEC_ENC_ERROR_T(*pfnQueryInfo)(IN HANDLE                              /*      */,
                                      IN VCODEC_ENC_QUERY_INFO_TYPE_T        /*         */,
                                      OUT void *                           /*            */
                                     );


} VCODEC_ENC_CALLBACK_T;

//             
typedef struct
{
    UINT64                       u8TimeStamp;
    VCODEC_ENC_INPUT_FLAG_T      eFlags;
    VCODEC_ENC_INPUT_INFO_T      rInput;
    VCODEC_ENC_BUFFER_INFO_T     pBuffInfo;
    VCODEC_COLOR_FORMAT_T       eVEncFormat;
    unsigned int                 u4Width;
    unsigned int                 u4Height;
    unsigned int                 u4YStride;
    unsigned int                 u4UVStride;
    unsigned int                 u4SliceHeight;
    void                         *reserved[4];
} VIDEO_ENCODER_INPUT_PARAM_NC_T;
//             
typedef struct
{
    UINT64                       u8TimeStamp;
    VCODEC_ENC_INPUT_FLAG_T      eFlags;
    VCODEC_ENC_INPUT_YUV_INFO_T  rInput;
    VCODEC_ENC_BUFFER_INFO_T     pBuffInfo;
    VCODEC_COLOR_FORMAT_T        eVEncFormat;
    unsigned int                 u4Width;
    unsigned int                 u4Height;
    unsigned int                 u4YStride;
    unsigned int                 u4UVStride;
    unsigned int                 u4SliceHeight;
    void                         *reserved[4];
} VIDEO_ENCODER_INPUT_YUV_PARAM_NC_T;

typedef struct
{
    VCODEC_ENC_ERROR_T(*pfnGetMemoryRequired)(IN VCODEC_ENC_SETTING_T *rCodecFormat,
                                              OUT VCODEC_MEMORY_SIZE_T *prExternalSize
                                             );

    /*                                                       
                              
                                                           */
    VCODEC_ENC_ERROR_T(*pfnOpen)(IN HANDLE hDrv,
                                 IN HANDLE hWrapper,
                                 IN VCODEC_ENC_CALLBACK_T * ,
                                 OUT HANDLE *hCodec
                                );

    VCODEC_ENC_ERROR_T(*pfnInit)(IN HANDLE hCodec
                                );

    VCODEC_ENC_ERROR_T(*pfnGetParameter)(IN HANDLE hCodec,
                                         OUT VCODEC_ENC_PARAM_TYPE_T, /*                   */
                                         void *
                                        );

    VCODEC_ENC_ERROR_T(*pfnSetParameter)(IN HANDLE hCodec,
                                         IN VCODEC_ENC_PARAM_TYPE_T, /*                                                                         */
                                         void *
                                        );

    /*                                                       
                              
                                                           */
    VCODEC_ENC_ERROR_T(*pfnGenerateHeader)(IN HANDLE hCodec,
                                           IN HANDLE hWrapper,
                                           IN VCODEC_ENC_BUFFER_INFO_T *prBufferInfo //                
                                          );

    /*                                                       
                              
                                                           */
    VCODEC_ENC_ERROR_T(*pfnEncodeOneUnit)(IN HANDLE hCodec,
                                          IN HANDLE hWrapper,
                                          IN VIDEO_ENCODER_INPUT_YUV_PARAM_NC_T *prEncoderInputParamNC //                
                                         );

    VCODEC_ENC_ERROR_T(*pfnDeInit)(IN HANDLE hCodec
                                  );

    VCODEC_ENC_ERROR_T(*pfnClose)(IN HANDLE hCodec
                                 );

    VCODEC_ENC_ERROR_T(*pfnGetNextBitstream)(IN HANDLE hCodec,
                                             OUT VCODEC_ENC_GENERAL_OUTPUT_T *
                                            );
} VIDEO_ENC_API_T;


//              

typedef struct
{
    VCODEC_ENC_ERROR_T(*pfnGetMemoryRequired)(IN VCODEC_ENC_SETTING_T *,          /*       */
                                              OUT VCODEC_MEMORY_SIZE_T *          /*              */
                                             );

    VCODEC_ENC_ERROR_T(*pfnOpen)(IN HANDLE                            /*        */,
                                 IN VCODEC_ENC_CALLBACK_T *        /*             */,
                                 OUT HANDLE *                          /*        */
                                );

    VCODEC_ENC_ERROR_T(*pfnInit)(IN HANDLE                                          /*        */
                                );

    VCODEC_ENC_ERROR_T(*pfnGetParameter)(IN HANDLE                            /*        */,
                                         IN VCODEC_ENC_PARAM_TYPE_T       /*                   */,
                                         OUT void *                           /*             */
                                        );

    VCODEC_ENC_ERROR_T(*pfnSetParameter)(IN HANDLE                            /*        */,
                                         IN VCODEC_ENC_PARAM_TYPE_T        /*             */, /*                                                                   */
                                         IN void *                            /*             */
                                        );

    VCODEC_ENC_ERROR_T(*pfnGenerateHeader)(IN HANDLE                                 /*        */
                                          );

    VCODEC_ENC_ERROR_T(*pfnEncodeOneUnit)(IN HANDLE                             /*        */,
                                          IN void *      /*       */  /*                                                     */
                                         );
    VCODEC_ENC_ERROR_T(*pfnDeInit)(IN HANDLE                                         /*        */
                                  );

    VCODEC_ENC_ERROR_T(*pfnClose)(IN HANDLE                                          /*        */
                                 );

    VCODEC_ENC_ERROR_T(*pfnGetNextBitstream)(IN HANDLE                             /*        */,
                                             OUT VCODEC_ENC_GENERAL_OUTPUT_T *  /*             */
                                            );
} VCODEC_ENC_API_T;

typedef struct
{
    VIDEO_ENC_API_T             rVideoEncAPI;
    VCODEC_ENC_API_T            *pfnVcodecEncAPI;
    HANDLE                      hDriver;
    VCODEC_BUFFER_T             rReleaseYUV;
    VCODEC_ENC_BUFFER_INFO_T    rEncoderBuffInfoNC;
} VIDEO_ENC_WRAP_HANDLE_T;

typedef struct
{
    unsigned int u4TimeIncrResolution;
    unsigned int      u4BufferSize;
    VCODEC_BUFFER_T    *prBuffer;
} MPEG4_VT_ENCODER_GEN_VT_HEADER_INPUT_T;

VCODEC_ENC_ERROR_T MPEG4EncoderGenerateVTHeader(
    IN    MPEG4_VT_ENCODER_GEN_VT_HEADER_INPUT_T *prInput,
    OUT   unsigned int                        *pu4EncodedSize
);






#endif /*             */

