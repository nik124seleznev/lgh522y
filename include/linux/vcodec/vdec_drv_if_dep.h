#ifndef VDEC_DRV_IF_DEP_H //              
#define VDEC_DRV_IF_DEP_H

/*                                                                             
                                             
                                                                             */

#include "val_types_private.h"
#include "vcodec_if_v2.h"
//                           
#include "wmc_type.h"
//                     
#include "vcodec_dec_demuxer_if_v2.h"
//                             
#define DumpInput__
#ifdef DumpInput__
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*                                                                             
                                          
                                                                             */
#define MAX_BUFFER_SIZE 21
/*              
 
                      
                                  
                                    
                                      
                   
*/
/*
                               
 
                                            
                                             
                                            
                                                         
                                                                         
                                    
*/

typedef struct __H264_DRV_DATA_T
{
    H264_DECODER_PAYLOAD_INFO_T     prH264Payload;
    H264_DECODER_INPUT_PARAM_T      prInputData;
    H264_DECODER_PRIVATE_PARAM_T    rPrivateData;
} H264_DRV_DATA_T, *P_H264_DRV_DATA_T;

typedef struct __MPEG4_DRV_DATA_T
{
    MPEG4_DECODER_PAYLOAD_INFO_T     prMPEG4Payload;
    MPEG4_DECODER_INPUT_PARAM_T      prInputData;
} MPEG4_DRV_DATA_T, *P_MPEG4_DRV_DATA_T;

typedef struct __VP8_DRV_DATA_T
{
    //                                               
} VP8_DRV_DATA_T, *P_VP8_DRV_DATA_T;

typedef struct __VP9_DRV_DATA_T
{
    //                                               
} VP9_DRV_DATA_T, *P_VP9_DRV_DATA_T;

typedef struct __VC1_DRV_DATA_T
{
    VAL_BOOL_T                      bVC1FirstDecode;
    VC1_DECODER_PAYLOAD_INFO_T      prVC1Payload;
    VC1_DECODER_INPUT_PARAM_T       prInputData;
    TEMP_INTERFACE                  VC1TempInterface;
} VC1_DRV_DATA_T, *P_VC1_DRV_DATA_T;

typedef struct __VDEC_DRV_BUF_STATUS_T
{
    VAL_BOOL_T          bDisplay;
    VAL_BOOL_T          bFree;
    VDEC_DRV_FRAMEBUF_T *pFrameBuf;
} VDEC_DRV_BUF_STATUS_T, *P_VDEC_DRV_BUF_STATUS_T;

typedef enum
{
    VDEC_DRV_STATUS_OPEN_DONE   = 0x00000001,
    VDEC_DRV_STATUS_INIT_DONE   = 0x00000002,
    VDEC_DRV_STATUS_DECODE_EVER = 0x00000004
} VDEC_DRV_STATUS;


typedef struct __VDEC_HANDLE_T
{
    VAL_HANDLE_T            hHalHandle;    //            
    VAL_HANDLE_T            vHandle;       //                         
    VAL_MEMORY_T            rHandleMem;    //                                                        
    VAL_BOOL_T              bFirstDecoded; //                                          
    VAL_BOOL_T              bHeaderPassed; //                                      
    VAL_BOOL_T              bFlushAll;
    VAL_BOOL_T              bNewMemory;    //                               
    VAL_MEMORY_T            HeaderBuf;
    VAL_MEMORY_T            HeaderBufwithFrame;
    VAL_HANDLE_T            hCodec;
    DRIVER_HANDLER_T        hDrv;
    VIDEO_WRAP_HANDLE_T     hWrapper;
    VAL_UINT32_T            CustomSetting;
    VCODEC_BUFFER_T         rVideoBitBuf;
    VCODEC_DEC_INPUT_YUV_INFO_T rVideoFrameBuf;
    VCODEC_MEMORY_TYPE_T    rVideoDecMemType;
    VAL_UINT32_T            YUVBuffer[MAX_BUFFER_SIZE];
    VAL_UINT32_T            nYUVBufferIndex;
    VAL_UINT32_T            nDrvStatus;
    VDEC_DRV_BUF_STATUS_T   pFrameBufArray[MAX_BUFFER_SIZE];
    VDEC_DRV_FRAMEBUF_T     *DispFrameBuf, *FreeFrameBuf;
    VCODEC_OPEN_SETTING_T           codecOpenSetting;
    VCODEC_DEC_INPUT_T              rInputUnit;
    VIDEO_DECODER_INPUT_NC_T        rVideoDecInputNC;
    VCODEC_DEC_OUTPUT_PARAM_T       *rVideoDecOutputParam;
    VCODEC_DEC_PRIVATE_OUTPUT_T     rVideoDecOutput;
    VCODEC_DEC_OUTPUT_BUFFER_PARAM_T   rVideoDecYUVBufferParameter;
    VCODEC_DEC_INPUT_BUFFER_PARAM_T    rBitStreamParam;
    //                                         
    VAL_BOOL_T                      bFirstDecodeForThumbnail;
    VAL_BOOL_T                      bThumbnailModeOK;
    VDEC_DRV_SET_DECODE_MODE_T      rSetDecodeMode;
    //                             
    VAL_UINT16_T            nDefWidth;
    VAL_UINT16_T            nDefHeight;

    VDEC_DRV_VIDEO_FORMAT_T CodecFormat;
    VAL_VOID_T              *prExtraData;  //                               
    VAL_MEMORY_T             rExtraDataMem; //                                                            
    VCODEC_DEC_PRIVATE_OUTPUT_EXTRA_T prExtraDecOutput;
#ifdef DumpInput__
    FILE *pf_out;
#endif

    //                       
    VAL_UINT32_T  nOmxTids;
    //  
#if 1   //                                         
    //              
    VAL_VOID_T              *pDrvModule;    //                              
    //  
#endif
    VAL_BOOL_T              fgValInitFlag; //                                 
} VDEC_HANDLE_T;

/*                                                                             
                                                    
                                                                              */

#ifdef __cplusplus
}
#endif

#endif /*                   */

