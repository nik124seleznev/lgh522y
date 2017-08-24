/* 
        
                          
  
                
          
  
                    
                                                    
  
               
                           
  
                       
                  
              
  
 */

#ifndef _VENC_DRV_IF_PRIVATE_H_
#define _VENC_DRV_IF_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "val_types_private.h"
#include "venc_drv_if_public.h"


typedef enum __VENC_DRV_COLOR_FORMAT_T
{
    VENC_DRV_COLOR_FORMAT_YUV420,
    VENC_DRV_COLOR_FORMAT_YV12,
}
VENC_DRV_COLOR_FORMAT_T;


typedef struct __VENC_DRV_YUV_STRIDE_T
{
    unsigned int    u4YStride;
    unsigned int    u4UVStride;
} VENC_DRV_YUV_STRIDE_T;


/* 
                 
                         
                   
                                                                            
                                               
 */
typedef struct __VENC_DRV_PARAM_EIS_T
{
    VAL_BOOL_T      fgEISEnable;            //                       
    VAL_UINT32_T    u4EISFrameWidth;        //                  
    VAL_UINT32_T    u4EISFrameHeight;       //                   
    VAL_UINT32_T    u4GMV_X;                //                                                            
    VAL_UINT32_T    u4GMV_Y;                //                                                            
} VENC_DRV_PARAM_EIS_T;

/* 
                 
                           
                   
                                                
 */
typedef VENC_DRV_PARAM_EIS_T     *P_VENC_DRV_PARAM_EIS_T;


/* 
                 
                         
                   
                                                                     
                         
 */
typedef struct __VENC_DRV_STATISTIC_T
{
    VAL_UINT32_T    u4EncTimeMax;   //                              
    VAL_UINT32_T    u4EncTimeMin;   //                              
    VAL_UINT32_T    u4EncTimeAvg;   //                                  
    VAL_UINT32_T    u4EncTimeSum;   //                              
} VENC_DRV_STATISTIC_T;

/* 
                 
                           
                   
                                                
 */
typedef VENC_DRV_STATISTIC_T     *P_VENC_DRV_STATISTIC_T;


typedef struct __VENC_HYB_ENCSETTING
{

    //                     
    VAL_UINT32_T    u4Width;
    VAL_UINT32_T    u4Height;
    VAL_UINT32_T    u4IntraVOPRate;     //          
    VAL_UINT32_T    eFrameRate;
    VAL_UINT32_T    u4VEncBitrate;
    VAL_UINT32_T    u4QualityLevel;
    VAL_UINT32_T    u4ShortHeaderMode;
    VAL_UINT32_T    u4CodecType;        //                     
    VAL_UINT32_T    u4RotateAngle;

    //                       
    VENC_DRV_COLOR_FORMAT_T     eVEncFormat;    //                   
    VENC_DRV_YUV_STRIDE_T       rVCodecYUVStride;
    VAL_UINT32_T    u4Profile;
    VAL_UINT32_T    u4Level;
    VAL_UINT32_T    u4BufWidth;
    VAL_UINT32_T    u4BufHeight;
    VAL_UINT32_T    u4NumBFrm;
    VAL_UINT32_T    fgInterlace;

    //              
    VAL_UINT32_T    u4InitQ;
    VAL_UINT32_T    u4MinQ;
    VAL_UINT32_T    u4MaxQ;
    VAL_UINT32_T    u4Algorithm;
    VAL_UINT32_T    u4_Rate_Hard_Limit;
    VAL_UINT32_T    u4RateBalance;
    VAL_UINT32_T    u4ForceIntraEnable;
    VAL_UINT32_T    u4VEncMinBitrate;   //            

    //                                     
    VAL_BOOL_T      fgUseMCI;
} VENC_HYBRID_ENCSETTING;


typedef struct VENC_BS_s
{
    VAL_UINT8_T                     *u4BS_addr;
    VAL_UINT8_T                     *u4BS_addr_PA;
    VAL_UINT32_T                    u4BSSize;
    VAL_UINT32_T                    u4BS_frmSize;
    VAL_UINT32_T                    u4BS_frmCount;
    VAL_UINT32_T                    u4BS_index;
    VAL_UINT32_T                    u4BS_preindex;
    VAL_UINT32_T                    u4Fillcnt;
    VAL_UINT32_T                    Handle;
} VENC_BS_T;


#ifdef __cplusplus
}
#endif

#endif //                                
