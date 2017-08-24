#ifndef VENC_DRV_IF_DEP_H
#define VENC_DRV_IF_DEP_H

/*                                                                             
                                             
                                                                             */

#include "val_types_private.h"
#include "vcodec_if_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

/*                                                                             
                                               
                                                                             */

/* 
                 
                 
                   
                                                                        
 */
typedef struct mhalVdoDrv_s
{
    VAL_VOID_T                      *prCodecHandle;
    VAL_UINT32_T                    u4EncodedFrameCount;
    VCODEC_ENC_CALLBACK_T           rCodecCb;
    VIDEO_ENC_API_T                 *prCodecAPI;
    VENC_BS_T                       pBSBUF;

    VCODEC_ENC_BUFFER_INFO_T        EncoderInputParamNC;
    VENC_DRV_PARAM_BS_BUF_T         BSout;
    VENC_HYBRID_ENCSETTING          rVencSetting;
    VAL_UINT8_T                     *ptr;
} mhalVdoDrv_t;

typedef struct __VENC_HANDLE_T
{
    VAL_HANDLE_T            hHalHandle;    //            
    VAL_HANDLE_T            vdriver_Handle;       //                         
    VAL_MEMORY_T            rHandleMem;    //                                                        
    VAL_BOOL_T              bFirstDecoded; //                                          
    VAL_BOOL_T              bHeaderPassed; //                                      
    VAL_BOOL_T              bFlushAll;
    VAL_MEMORY_T            HeaderBuf;
    VAL_HANDLE_T            hCodec;
    //                                 
    VAL_UINT32_T            CustomSetting;
    VCODEC_MEMORY_TYPE_T    rVideoDecMemType;
    VAL_UINT32_T            nYUVBufferIndex;
    VCODEC_OPEN_SETTING_T   codecOpenSetting;

    mhalVdoDrv_t            rMhalVdoDrv;
    VAL_MEMORY_T            bs_driver_workingmem;

    //                       
    VENC_DRV_VIDEO_FORMAT_T CodecFormat;
    VAL_VOID_T              *prExtraData;  //                               
    VAL_MEMORY_T             rExtraDataMem; //                                                            
    //   
    VAL_UINT32_T  nOmxTids;
#if 1   //                                         
    //              
    VAL_VOID_T              *pDrvModule;    //                              
    //  
#endif
    VIDEO_ENC_WRAP_HANDLE_T hWrapper;
} VENC_HANDLE_T;


#ifdef __cplusplus
}
#endif

#endif /*                   */