#include "venc_drv_if_private.h"
#include "vcodec_if_v2.h"

#include <sys/time.h>

#ifndef _VENC_DRV_BASE_
#define _VENC_DRV_BASE_

#define DO_VCODEC_RESET(cmd, index)                                                             \
    {                                                                                               \
    }

typedef enum __VDDRV_MRESULT_T
{
    VDDRV_MRESULT_SUCCESS = VAL_TRUE,  //                    
    VDDRV_MRESULT_FAIL = VAL_FALSE     //                    
} VDDRV_MRESULT_T;

typedef struct __VENC_DRV_BASE_T
{
    VAL_UINT32_T(*Init)(VAL_HANDLE_T *handle, VAL_HANDLE_T halhandle, VAL_HANDLE_T valhandle);                       //                                       
    VAL_UINT32_T(*Encode)(VAL_HANDLE_T handle, VENC_DRV_START_OPT_T eOpt, P_VENC_DRV_PARAM_FRM_BUF_T pFrameBuf, P_VENC_DRV_PARAM_BS_BUF_T pBitstreamBuf, VENC_DRV_DONE_RESULT_T *pResult);
    VAL_UINT32_T(*GetParam)(VAL_HANDLE_T handle, VENC_DRV_GET_TYPE_T a_eType, VAL_VOID_T *a_pvInParam, VAL_VOID_T *a_pvOutParam);      //                                    
    VAL_UINT32_T(*SetParam)(VAL_HANDLE_T handle, VENC_DRV_SET_TYPE_T a_eType, VAL_VOID_T *a_pvInParam, VAL_VOID_T *a_pvOutParam);      //                                    
    VAL_UINT32_T(*DeInit)(VAL_HANDLE_T handle);                                                                                     //                                          
} VENC_DRV_BASE_T;

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


typedef struct __VENC_HYBRID_HANDLE_T
{
    mhalVdoDrv_t                    rMhalVdoDrv;
    VAL_MEMORY_T                    rBSDrvWorkingMem;
    VAL_UINT32_T                    nOmxTids;
    VAL_VCODEC_THREAD_ID_T          rThreadID;
    VIDEO_ENC_WRAP_HANDLE_T         hWrapper;
    VAL_VOID_T                      *pDrvModule;    //                              
} VENC_HYBRID_HANDLE_T;


typedef struct __VENC_HANDLE_T
{
    VENC_DRV_VIDEO_FORMAT_T CodecFormat;
    VENC_DRV_BASE_T         rFuncPtr;      //                                          
    VAL_HANDLE_T            hDrvHandle;    //                               
    VAL_HANDLE_T            hHalHandle;    //             
    VAL_HANDLE_T            hValHandle;    //             
    VAL_MEMORY_T            rHandleMem;    //                         
    VAL_VOID_T              *prExtraData;  //                               
    VAL_MEMORY_T            rExtraDataMem; //                                                            
    VENC_HYBRID_HANDLE_T    rHybridHandle; //                
    FILE                    *pfDump;       //            
    VAL_UINT32_T            u4ShowInfo;    //                                
    VAL_UINT32_T            u4FPS;         //      
    VAL_UINT32_T            u4Bitrate;     //          
    struct timeval          tStart;        //                                      
    VENC_DRV_SCENARIO_T     eScenario;     //               
} VENC_HANDLE_T;

VENC_DRV_MRESULT_T ParseConfig(const char *cfgFileName, const char *ParameterItem, VAL_UINT32_T *val);


#endif
