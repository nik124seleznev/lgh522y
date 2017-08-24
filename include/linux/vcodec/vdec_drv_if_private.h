/* 
        
                          
  
                
          
  
                    
                                                    
  
               
                           
  
                       
                  
              
  
 */

#ifndef _VDEC_DRV_IF_PRIVATE_H_
#define _VDEC_DRV_IF_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "val_types_private.h"
#include "vdec_drv_if_public.h"


typedef enum
{
    UNKNOWN_FBTYPE = 0,                         //               
    VDEC_DRV_FBTYPE_YUV420_BK_16x1 = (1 << 0),  //                                  
    VDEC_DRV_FBTYPE_YUV420_BK_8x2  = (1 << 1),  //                                  
    VDEC_DRV_FBTYPE_YUV420_BK_4x4  = (1 << 2),  //                                  
    VDEC_DRV_FBTYPE_YUV420_RS      = (1 << 3),  //                                        
    VDEC_DRV_FBTYPE_RGB565_RS      = (1 << 4)   //                       
}
VDEC_DRV_FBTYPE_T;


/* 
                 
                             
                   
                          
                                  
                          
                        
                                                           
                                                             
 */
typedef enum
{
    VDEC_DRV_BUFFER_CONTROL_UNKNOWN                    = 0,         //               
    VDEC_DRV_BUFFER_CONTROL_REF_IS_DISP_EXT            = (1 << 0),  //                                                                   
    VDEC_DRV_BUFFER_CONTROL_REF_IS_DISP_INT            = (1 << 1),  //                                                                   
    VDEC_DRV_BUFFER_CONTROL_REF_IS_DISP_EXT_POOL       = (1 << 2),  //                                                                        
    VDEC_DRV_BUFFER_CONTROL_REF_IS_DISP_INT_POOL       = (1 << 3),  //                                                                        
    VDEC_DRV_BUFFER_CONTROL_REF_EXT_DISP_EXT           = (1 << 4),  //                                                                                     
    VDEC_DRV_BUFFER_CONTROL_REF_EXT_DISP_INT           = (1 << 5),  //                                                                              
    VDEC_DRV_BUFFER_CONTROL_REF_EXT_DISP_EXT_POOL      = (1 << 6),  //                                                                                   
    VDEC_DRV_BUFFER_CONTROL_REF_EXT_DISP_INT_POOL      = (1 << 7),  //                                                                                   
    VDEC_DRV_BUFFER_CONTROL_REF_EXT_POOL_DISP_EXT      = (1 << 8),  //                                                                                  
    VDEC_DRV_BUFFER_CONTROL_REF_EXT_POOL_DISP_INT      = (1 << 9),  //                                                                                   
    VDEC_DRV_BUFFER_CONTROL_REF_EXT_POOL_DISP_EXT_POOL = (1 << 10), //                                                                                        
    VDEC_DRV_BUFFER_CONTROL_REF_EXT_POOL_DISP_INT_POOL = (1 << 11), //                                                                                        
    VDEC_DRV_BUFFER_CONTROL_REF_INT_DISP_EXT           = (1 << 12), //                                                                             
    VDEC_DRV_BUFFER_CONTROL_REF_INT_DISP_INT           = (1 << 13), //                                                                              
    VDEC_DRV_BUFFER_CONTROL_REF_INT_DISP_EXT_POOL      = (1 << 14), //                                                                                   
    VDEC_DRV_BUFFER_CONTROL_REF_INT_DISP_INT_POOL      = (1 << 15), //                                                                                   
    VDEC_DRV_BUFFER_CONTROL_REF_INT_POOL_DISP_EXT      = (1 << 16), //                                                                                  
    VDEC_DRV_BUFFER_CONTROL_REF_INT_POOL_DISP_INT      = (1 << 17), //                                                                                   
    VDEC_DRV_BUFFER_CONTROL_REF_INT_POOL_DISP_EXT_POOL = (1 << 18), //                                                                                        
    VDEC_DRV_BUFFER_CONTROL_REF_INT_POOL_DISP_INT_POOL = (1 << 19)  //                                                                                                
} VDEC_DRV_BUFFER_CONTROL_T;


/* 
                 
                             
                   
                  
                                                         
 */
typedef enum
{
    VDEC_DRV_DOWNSIZE_RATIO_UNKNOWN = 0,        //                
    VDEC_DRV_DOWNSIZE_RATIO_1_1     = (1 << 0), //                 
    VDEC_DRV_DOWNSIZE_RATIO_1_2     = (1 << 1), //              
    VDEC_DRV_DOWNSIZE_RATIO_1_3     = (1 << 2), //              
    VDEC_DRV_DOWNSIZE_RATIO_1_4     = (1 << 3), //              
    VDEC_DRV_DOWNSIZE_RATIO_1_5     = (1 << 4), //              
    VDEC_DRV_DOWNSIZE_RATIO_1_6     = (1 << 5), //              
    VDEC_DRV_DOWNSIZE_RATIO_1_7     = (1 << 6), //              
    VDEC_DRV_DOWNSIZE_RATIO_1_8     = (1 << 7)  //              
} VDEC_DRV_DOWNSIZE_RATIO_T;


/* 
                 
                         
                   
                          
                                
                                 
 */
typedef enum
{
    VDEC_DRV_PIC_STRUCT_UNKNOWN = 0,            //          
    VDEC_DRV_PIC_STRUCT_CONSECUTIVE_FRAME,      //                    
    VDEC_DRV_PIC_STRUCT_CONSECUTIVE_TOP_FIELD,  //                        
    VDEC_DRV_PIC_STRUCT_CONSECUTIVE_BOT_FIELD,  //                           
    VDEC_DRV_PIC_STRUCT_SEPARATED_TOP_FIELD,    //                       
    VDEC_DRV_PIC_STRUCT_SEPARATED_BOT_FIELD,     //                          
    VDEC_DRV_PIC_STRUCT_FIELD,
} VDEC_DRV_PIC_STRUCT_T;


/* 
                 
                         
                   
                    
 */
typedef enum
{
    VDEC_DRV_FRAME_RATE_UNKNOWN = 0,            //              
    VDEC_DRV_FRAME_RATE_23_976,                 //                               
    VDEC_DRV_FRAME_RATE_24,                     //           
    VDEC_DRV_FRAME_RATE_25,                     //           
    VDEC_DRV_FRAME_RATE_29_97,                  //                              
    VDEC_DRV_FRAME_RATE_30,                     //           
    VDEC_DRV_FRAME_RATE_50,                     //           
    VDEC_DRV_FRAME_RATE_59_94,                  //                              
    VDEC_DRV_FRAME_RATE_60,                     //           
    VDEC_DRV_FRAME_RATE_120,                    //            
    VDEC_DRV_FRAME_RATE_1,                      //          
    VDEC_DRV_FRAME_RATE_5,                      //          
    VDEC_DRV_FRAME_RATE_8,                      //          
    VDEC_DRV_FRAME_RATE_10,                     //           
    VDEC_DRV_FRAME_RATE_12,                     //           
    VDEC_DRV_FRAME_RATE_15,                     //           
    VDEC_DRV_FRAME_RATE_16,                     //           
    VDEC_DRV_FRAME_RATE_17,                     //           
    VDEC_DRV_FRAME_RATE_18,                     //           
    VDEC_DRV_FRAME_RATE_20,                     //           
    VDEC_DRV_FRAME_RATE_2,                      //          
    VDEC_DRV_FRAME_RATE_6,                      //          
    VDEC_DRV_FRAME_RATE_48,                     //           
    VDEC_DRV_FRAME_RATE_70,                     //           
    VDEC_DRV_FRAME_RATE_VARIABLE                //            
} VDEC_DRV_FRAME_RATE_T;


/* 
                 
                        
                   
                                                                  
 */
typedef enum
{
    VDEC_DRV_POST_PROC_UNKNOWN = 0,             //          
    VDEC_DRV_POST_PROC_DISABLE,                 //                            
    VDEC_DRV_POST_PROC_DOWNSIZE,                //              
    VDEC_DRV_POST_PROC_RESIZE,                  //            
    VDEC_DRV_POST_PROC_DEBLOCK,                 //                
    VDEC_DRV_POST_PROC_DEINTERLACE              //                 
} VDEC_DRV_POST_PROC_T;


/* 
                 
                   
                   
                    
                                    
                                 
 */

typedef struct
{
    VAL_UINT32_T    u4AddrOfNALu;               //                           
    VAL_UINT32_T    u4LengthOfNALu;             //                        
    void            *pReseved;                  //           
} VDEC_DRV_NALU_T;


/* 
                 
                        
                   
                                 
 */
typedef struct __VDEC_DRV_STATISTIC_T
{
    VAL_UINT32_T   u4DecTimeMax;                //                                      
    VAL_UINT32_T   u4DecTimeMin;                //                                      
    VAL_UINT32_T   u4DecTimeAvg;                //                                          
} VDEC_DRV_STATISTIC_T;

/* 
                 
                          
                   
                                   
 */
typedef VDEC_DRV_STATISTIC_T *P_VDEC_DRV_STATISTIC_T;


/* 
                 
                     
                   
                                               
 */
typedef struct
{
    //                
    VAL_UINT32_T    nBufferStatus;
    VAL_INT64_T     llLastVideoTime;
    VAL_INT64_T     llCurrentPlayTime;
} DRIVER_HANDLER_T;


/* 
                 
                           
                   
                                              
 */
typedef struct __VDEC_DRV_VIDEO_FBTYPE_T
{
    VAL_UINT32_T u4FBType;  //                  
} VDEC_DRV_VIDEO_FBTYPE_T;

/* 
                 
                             
                   
                                      
 */
typedef VDEC_DRV_VIDEO_FBTYPE_T *P_VDEC_DRV_VIDEO_FBTYPE_T;


/* 
                 
                                
                   
                                                      
 */
typedef struct __VDEC_DRV_QUERY_BUFFER_MODE_T
{
    VAL_UINT32_T u4BufCtrl;        //                          
} VDEC_DRV_QUERY_BUFFER_MODE_T;

/* 
                 
                                  
                   
                                           
 */
typedef VDEC_DRV_QUERY_BUFFER_MODE_T *P_VDEC_DRV_QUERY_BUFFER_MODE_T;


/* 
                 
                              
                   
                                                                                
 */
typedef struct __VDEC_DRV_QUERY_POOL_SIZE_T
{
    VAL_UINT32_T    u4Size;     //                                 
} VDEC_DRV_QUERY_POOL_SIZE_T;

/* 
                 
                                
                   
                                         
 */
typedef VDEC_DRV_QUERY_POOL_SIZE_T *P_VDEC_DRV_QUERY_POOL_SIZE_T;

//                                                                                                
//                                                                        
//                                                                                


/* 
                 
                                  
                   
                                                         
 */
typedef struct __VDEC_DRV_QUERY_POOL_DOWNSIZE_T
{
    VAL_UINT32_T    u4Ratio;    //                          
} VDEC_DRV_QUERY_POOL_DOWNSIZE_T;

/* 
                 
                                    
                   
                                             
 */
typedef VDEC_DRV_QUERY_POOL_DOWNSIZE_T *P_VDEC_DRV_QUERY_POOL_DOWNSIZE_T;


/* 
                 
                                
                   
                                                                         
 */
typedef struct __VDEC_DRV_QUERY_POOL_RESIZE_T
{
    VAL_UINT32_T    u4OutWidth;     //                 
    VAL_UINT32_T    u4OutHeight;    //                  
} VDEC_DRV_QUERY_POOL_RESIZE_T;

/* 
                 
                                  
                   
                                           
 */
typedef VDEC_DRV_QUERY_POOL_RESIZE_T *P_VDEC_DRV_QUERY_POOL_RESIZE_T;

//                                                                                
//                                                                               
//                                                                                    
//                                                                                  


/* 
                 
                              
                   
                                                                               
 */
typedef struct __VDEC_DRV_SET_BUFFER_MODE_T
{
    VAL_UINT32_T    u4BufferMode;       //                          
} VDEC_DRV_SET_BUFFER_MODE_T;

//                                                                                                  

/* 
                 
                                
                   
                                         
 */
typedef VDEC_DRV_SET_BUFFER_MODE_T *P_VDEC_DRV_SET_BUFFER_MODE_T;

//                                                                                                  


/* 
                 
                              
                   
                                                                                      
 */
typedef struct __VDEC_DRV_SET_BUFFER_ADDR_T
{
    VAL_MEM_ADDR_T rBufferAddr;         //                             
} VDEC_DRV_SET_BUFFER_ADDR_T;

/* 
                 
                                
                   
                                         
 */
typedef VDEC_DRV_SET_BUFFER_ADDR_T *P_VDEC_DRV_SET_BUFFER_ADDR_T;

//                                                                                                                                


/* 
                 
                                 
                   
                                       
 */
typedef struct __VDEC_DRV_SET_POST_PROC_MODE_T
{
    VAL_UINT32_T    u4PostProcMode;     //                            
    VAL_UINT32_T    u4DownsizeRatio;    //                                
    VAL_UINT32_T    u4ResizeWidth;      //                              
    VAL_UINT32_T    u4ResizeHeight;     //                              
} VDEC_DRV_SET_POST_PROC_MODE_T;

/* 
                 
                                   
                   
                                            
 */
typedef VDEC_DRV_SET_POST_PROC_MODE_T *P_VDEC_DRV_SET_POST_PROC_MODE_T;



typedef struct _VDEC_DRV_HW_REG_T
{
    VAL_UINT32_T    u4VdecHWBase;
    VAL_UINT32_T    u4VdecHWSYS;
    VAL_UINT32_T    u4VdecMISC;
    VAL_UINT32_T    u4VdecVLD;
    VAL_UINT32_T    u4VdecVLDTOP;
    VAL_UINT32_T    u4VdecMC;
    VAL_UINT32_T    u4VdecAVCVLD;
    VAL_UINT32_T    u4VdecAVCMV;
    VAL_UINT32_T    u4VdecPP;
    VAL_UINT32_T    u4VdecSQT;
    VAL_UINT32_T    u4VdecVP8VLD;
    VAL_UINT32_T    u4VdecVP6VLD;
    VAL_UINT32_T    u4VdecVP8VLD2;
} VDEC_DRV_HW_REG_T;

typedef VDEC_DRV_HW_REG_T *P_VDEC_DRV_HW_REG_T;


#ifdef __cplusplus
}
#endif

#endif //                                
