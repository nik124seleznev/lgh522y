/******************************************************************************
 * mt6593_isp.c - MT6593 Linux ISP Device Driver
 *
 * Copyright 2008-2009 MediaTek Co.,Ltd.
 *
 * DESCRIPTION:
 *     This file provid the other drivers ISP relative functions
 *
 ******************************************************************************/

#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
//                   
//                    
#include <linux/proc_fs.h>  //             
//
#include <linux/slab.h>
#include <linux/spinlock.h>
//                     
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <asm/atomic.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/xlog.h> //                   
//
#include <mach/hardware.h>
//                            
#include <mach/camera_isp.h>
#include <mach/irqs.h>
#include <mach/mt_reg_base.h>
#include <mach/mt_clkmgr.h>     //                                                    
#include <mach/sync_write.h>    //                              
#include <mach/mt_spm_idle.h>    //                                          

#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

#include <mach/m4u.h>
#include <cmdq_core.h>

#ifdef CONFIG_COMPAT
//       
#include <linux/fs.h>
#include <linux/compat.h>
#endif

//
#include "smi_common.h"

#define CAMSV_DBG
#ifdef CAMSV_DBG
    #define CAM_TAG "CAM:"
    #define CAMSV_TAG "SV1:"
    #define CAMSV2_TAG "SV2:"
#else
    #define CAMSV_TAG ""
    #define CAMSV2_TAG ""
    #define CAM_TAG ""
#endif
typedef unsigned char           MUINT8;
typedef unsigned int            MUINT32;
//
typedef signed char             MINT8;
typedef signed int              MINT32;
//
typedef bool                    MBOOL;
//
#ifndef MTRUE
    #define MTRUE               1
#endif
#ifndef MFALSE
    #define MFALSE              0
#endif
//                                                                            
//                                                                                    
//                                                                                     
//                                                                                           
//                                                                                                         


#define LOG_VRB(format, args...)    xlog_printk(ANDROID_LOG_VERBOSE, "ISP", "[%s] " format, __FUNCTION__, ##args)
#define LOG_DBG(format, args...)    xlog_printk(ANDROID_LOG_DEBUG  , "ISP", "[%s] " format, __FUNCTION__, ##args)
//                                                                                
//                                                                                                                          
#define LOG_INF(format, args...)    xlog_printk(ANDROID_LOG_INFO  , "ISP", "[%s] " format, __FUNCTION__, ##args)
#define LOG_WRN(format, args...)    xlog_printk(ANDROID_LOG_WARN   , "ISP", "[%s] WARNING: " format, __FUNCTION__, ##args)
#define LOG_ERR(format, args...)    xlog_printk(ANDROID_LOG_ERROR  , "ISP", "[%s, line%04d] ERROR: " format, __FUNCTION__, __LINE__, ##args)
#define LOG_AST(format, args...)    xlog_printk(ANDROID_LOG_ASSERT , "ISP", "[%s, line%04d] ASSERT: " format, __FUNCTION__, __LINE__, ##args)

/*                                                                              
 
                                                                               */
//                                                                            
#define ISP_WR32(addr, data)    mt_reg_sync_writel(data, addr)    //                                         
#define ISP_RD32(addr)          ioread32(addr)
#define ISP_SET_BIT(reg, bit)   ((*(volatile MUINT32*)(reg)) |= (MUINT32)(1 << (bit)))
#define ISP_CLR_BIT(reg, bit)   ((*(volatile MUINT32*)(reg)) &= ~((MUINT32)(1 << (bit))))
/*                                                                              
 
                                                                               */
#define ISP_DEV_NAME                "camera-isp"

//                                                                 
//                                      
//          
#define ISP_RTBUF_REG_RANGE  0x10000
#define IMGSYS_BASE_ADDR     0x15000000
#define ISP_REG_RANGE        (0x10000)   //                                                             
//             
#define SENINF_BASE_ADDR     0x15008000 //                                                         
#define SENINF_REG_RANGE    (0x4000)   //                                                               
#define PLL_BASE_ADDR        0x10000000 //                                                         
#define PLL_RANGE            (0x1000)   //                                                                
#define MIPIRX_CONFIG_ADDR   0x1500C000 //                                                         
#define MIPIRX_CONFIG_RANGE (0x1000)//                                                                
#define MIPIRX_ANALOG_ADDR   0x10217000 //                                                         
#define MIPIRX_ANALOG_RANGE (0x3000)
#define GPIO_BASE_ADDR       0x10002000 //                                                         
#define GPIO_RANGE          (0x1000)
//                
#define ISP_RANGE         (0x10000)
//                                                                 

/*                                                                              
 
                                                                               */
#define ISP_DBG_INT                 (0x00000001)
#define ISP_DBG_HOLD_REG            (0x00000002)
#define ISP_DBG_READ_REG            (0x00000004)
#define ISP_DBG_WRITE_REG           (0x00000008)
#define ISP_DBG_CLK                 (0x00000010)
#define ISP_DBG_TASKLET             (0x00000020)
#define ISP_DBG_SCHEDULE_WORK       (0x00000040)
#define ISP_DBG_BUF_WRITE           (0x00000080)
#define ISP_DBG_BUF_CTRL            (0x00000100)
#define ISP_DBG_REF_CNT_CTRL        (0x00000200)
#define ISP_DBG_INT_2               (0x00000400)
#define ISP_DBG_INT_3               (0x00000800)

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"
#include "kd_imgsensor_errcode.h"

#define CONSST_FBC_CNT_INIT 1

extern BOOL g_bEnableDriver[KDIMGSENSOR_MAX_INVOKE_DRIVERS];
extern SENSOR_FUNCTION_STRUCT *g_pInvokeSensorFunc[KDIMGSENSOR_MAX_INVOKE_DRIVERS];
extern char g_invokeSensorNameStr[KDIMGSENSOR_MAX_INVOKE_DRIVERS][32];
extern CAMERA_DUAL_CAMERA_SENSOR_ENUM g_invokeSocketIdx[KDIMGSENSOR_MAX_INVOKE_DRIVERS];
extern int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName,BOOL On, char* mode_name);

#ifdef AP_DONT_STOP_PREVIEW
//                                        
static volatile MINT32 bResumeSignal = 0;
#endif
typedef struct _isp_backup_reg
{
    UINT32               CAM_CTL_START;                  //    
    UINT32               CAM_CTL_EN_P1;                  //    
    UINT32           CAM_CTL_EN_P1_DMA;                  //    
    UINT32                    rsv_400C;                  //    
    UINT32             CAM_CTL_EN_P1_D;                  //    
    UINT32         CAM_CTL_EN_P1_DMA_D;                  //    
    UINT32               CAM_CTL_EN_P2;                  //    
    UINT32           CAM_CTL_EN_P2_DMA;                  //    
    UINT32               CAM_CTL_CQ_EN;                  //    
    UINT32            CAM_CTL_SCENARIO;                  //    
    UINT32          CAM_CTL_FMT_SEL_P1;                  //    
    UINT32        CAM_CTL_FMT_SEL_P1_D;                  //    
    UINT32          CAM_CTL_FMT_SEL_P2;                  //    
    UINT32              CAM_CTL_SEL_P1;                  //    
    UINT32            CAM_CTL_SEL_P1_D;                  //    
    UINT32              CAM_CTL_SEL_P2;                  //    
    UINT32          CAM_CTL_SEL_GLOBAL;                  //    
    UINT32                    rsv_4044;                  //    
    UINT32           CAM_CTL_INT_P1_EN;                  //    
    UINT32       CAM_CTL_INT_P1_STATUS;                  //    
    UINT32          CAM_CTL_INT_P1_EN2;                  //    
    UINT32      CAM_CTL_INT_P1_STATUS2;                  //    
    UINT32         CAM_CTL_INT_P1_EN_D;                  //    
    UINT32     CAM_CTL_INT_P1_STATUS_D;                  //    
    UINT32        CAM_CTL_INT_P1_EN2_D;                  //    
    UINT32    CAM_CTL_INT_P1_STATUS2_D;                  //    
    UINT32           CAM_CTL_INT_P2_EN;                  //    
    UINT32       CAM_CTL_INT_P2_STATUS;                  //    
    UINT32         CAM_CTL_INT_STATUSX;                  //    
    UINT32        CAM_CTL_INT_STATUS2X;                  //    
    UINT32        CAM_CTL_INT_STATUS3X;                  //    
    UINT32                CAM_CTL_TILE;                  //    
    UINT32       CAM_CTL_TDR_EN_STATUS;                  //    
    UINT32              CAM_CTL_TCM_EN;                  //    
    UINT32      CAM_CTL_TDR_DBG_STATUS;                  //    
    UINT32              CAM_CTL_SW_CTL;                  //    
    UINT32              CAM_CTL_SPARE0;                  //    
    UINT32               CAM_RRZ_OUT_W;                 //    
    UINT32                    rsv_4098;                 //    
    UINT32              CAM_RRZ_OUT_W_D;                //    
    UINT32        CAM_CTL_CQ1_BASEADDR;                 //    
    UINT32        CAM_CTL_CQ2_BASEADDR;                 //    
    UINT32        CAM_CTL_CQ3_BASEADDR;                 //    
    UINT32        CAM_CTL_CQ0_BASEADDR;                 //    
    UINT32       CAM_CTL_CQ0B_BASEADDR;                 //    
    UINT32       CAM_CTL_CQ0C_BASEADDR;                 //    
    UINT32    CAM_CTL_CUR_CQ0_BASEADDR;                 //    
    UINT32   CAM_CTL_CUR_CQ0B_BASEADDR;                 //    
    UINT32   CAM_CTL_CUR_CQ0C_BASEADDR;                 //    
    UINT32      CAM_CTL_CQ0_D_BASEADDR;                 //    
    UINT32     CAM_CTL_CQ0B_D_BASEADDR;                 //    
    UINT32     CAM_CTL_CQ0C_D_BASEADDR;                 //    
    UINT32  CAM_CTL_CUR_CQ0_D_BASEADDR;                 //    
    UINT32 CAM_CTL_CUR_CQ0B_D_BASEADDR;                 //    
    UINT32 CAM_CTL_CUR_CQ0C_D_BASEADDR;                 //    
    UINT32           CAM_CTL_DB_LOAD_D;                 //    
    UINT32             CAM_CTL_DB_LOAD;                 //    
    UINT32         CAM_CTL_P1_DONE_BYP;                 //    
    UINT32       CAM_CTL_P1_DONE_BYP_D;                 //    
    UINT32         CAM_CTL_P2_DONE_BYP;                 //    
    UINT32            CAM_CTL_IMGO_FBC;                 //    
    UINT32            CAM_CTL_RRZO_FBC;                 //    
    UINT32          CAM_CTL_IMGO_D_FBC;                 //    
    UINT32          CAM_CTL_RRZO_D_FBC;                 //    



    UINT32                CAM_CTL_IHDR;                 //    
    UINT32              CAM_CTL_IHDR_D;                 //    
    UINT32            CAM_CTL_CQ_EN_P2;                 //    

    UINT32              CAM_CTL_CLK_EN;                 //    
    UINT32             CAM_TG_SEN_MODE;                 //    
    UINT32               CAM_TG_VF_CON;                 //    
    UINT32         CAM_TG_SEN_GRAB_PXL;                 //    
    UINT32         CAM_TG_SEN_GRAB_LIN;                 //    
    UINT32             CAM_TG_PATH_CFG;                 //    
    UINT32            CAM_TG_MEMIN_CTL;                 //    


    UINT32              CAM_OBC_OFFST0;                 //    
    UINT32              CAM_OBC_OFFST1;                 //    
    UINT32              CAM_OBC_OFFST2;                 //    
    UINT32              CAM_OBC_OFFST3;                 //    
    UINT32               CAM_OBC_GAIN0;                 //    
    UINT32               CAM_OBC_GAIN1;                 //    
    UINT32               CAM_OBC_GAIN2;                 //    
    UINT32               CAM_OBC_GAIN3;                 //    
    UINT32                 rsv_4520[4];                 //           
    UINT32                CAM_LSC_CTL1;                 //    
    UINT32                CAM_LSC_CTL2;                 //    
    UINT32                CAM_LSC_CTL3;                 //    
    UINT32              CAM_LSC_LBLOCK;                 //    
    UINT32               CAM_LSC_RATIO;                 //    
    UINT32          CAM_LSC_TPIPE_OFST;                 //    
    UINT32          CAM_LSC_TPIPE_SIZE;                 //    
    UINT32             CAM_LSC_GAIN_TH;                 //    
    UINT32              CAM_RPG_SATU_1;                 //    
    UINT32              CAM_RPG_SATU_2;                 //    
    UINT32              CAM_RPG_GAIN_1;                 //    
    UINT32              CAM_RPG_GAIN_2;                 //    
    UINT32              CAM_RPG_OFST_1;                 //    
    UINT32              CAM_RPG_OFST_2;                 //    
    UINT32                rsv_4568[6];                  //            
    UINT32                CAM_SGG2_PGN;                 //    
    UINT32             CAM_SGG2_GMRC_1;                 //    
    UINT32             CAM_SGG2_GMRC_2;                 //    
    UINT32                 rsv_458C[9];                 //           
    UINT32             CAM_AWB_WIN_ORG;                 //    
    UINT32            CAM_AWB_WIN_SIZE;                 //    
    UINT32             CAM_AWB_WIN_PIT;                 //    
    UINT32             CAM_AWB_WIN_NUM;                 //    
    UINT32             CAM_AWB_GAIN1_0;                 //    
    UINT32             CAM_AWB_GAIN1_1;                 //    
    UINT32              CAM_AWB_LMT1_0;                 //    
    UINT32              CAM_AWB_LMT1_1;                 //    
    UINT32             CAM_AWB_LOW_THR;                 //    
    UINT32              CAM_AWB_HI_THR;                 //    
    UINT32          CAM_AWB_PIXEL_CNT0;                 //    
    UINT32          CAM_AWB_PIXEL_CNT1;                 //    
    UINT32          CAM_AWB_PIXEL_CNT2;                 //    
    UINT32             CAM_AWB_ERR_THR;                 //    
    UINT32                 CAM_AWB_ROT;                 //    
    UINT32                CAM_AWB_L0_X;                 //    
    UINT32                CAM_AWB_L0_Y;                 //    
    UINT32                CAM_AWB_L1_X;                 //    
    UINT32                CAM_AWB_L1_Y;                 //    
    UINT32                CAM_AWB_L2_X;                 //    
    UINT32                CAM_AWB_L2_Y;                 //    
    UINT32                CAM_AWB_L3_X;                 //    
    UINT32                CAM_AWB_L3_Y;                 //    
    UINT32                CAM_AWB_L4_X;                 //    
    UINT32                CAM_AWB_L4_Y;                 //    
    UINT32                CAM_AWB_L5_X;                 //    
    UINT32                CAM_AWB_L5_Y;                 //    
    UINT32                CAM_AWB_L6_X;                 //    
    UINT32                CAM_AWB_L6_Y;                 //    
    UINT32                CAM_AWB_L7_X;                 //    
    UINT32                CAM_AWB_L7_Y;                 //    
    UINT32                CAM_AWB_L8_X;                 //    
    UINT32                CAM_AWB_L8_Y;                 //    
    UINT32                CAM_AWB_L9_X;                 //    
    UINT32                CAM_AWB_L9_Y;                 //    
    UINT32               CAM_AWB_SPARE;                 //    
    UINT32                 rsv_4640[4];                 //           
    UINT32              CAM_AE_HST_CTL;                 //    
    UINT32              CAM_AE_GAIN2_0;                 //    
    UINT32              CAM_AE_GAIN2_1;                 //    
    UINT32               CAM_AE_LMT2_0;                 //    
    UINT32               CAM_AE_LMT2_1;                 //    
    UINT32             CAM_AE_RC_CNV_0;                 //    
    UINT32             CAM_AE_RC_CNV_1;                 //    
    UINT32             CAM_AE_RC_CNV_2;                 //    
    UINT32             CAM_AE_RC_CNV_3;                 //    
    UINT32             CAM_AE_RC_CNV_4;                 //    
    UINT32             CAM_AE_YGAMMA_0;                 //    
    UINT32             CAM_AE_YGAMMA_1;                 //    
    UINT32              CAM_AE_HST_SET;                 //    
    UINT32             CAM_AE_HST0_RNG;                 //    
    UINT32             CAM_AE_HST1_RNG;                 //    
    UINT32             CAM_AE_HST2_RNG;                 //    
    UINT32             CAM_AE_HST3_RNG;                 //    
    UINT32                CAM_AE_SPARE;                 //    
    UINT32                 rsv_4698[2];                 //           
    UINT32                CAM_SGG1_PGN;                 //    
    UINT32             CAM_SGG1_GMRC_1;                 //    
    UINT32             CAM_SGG1_GMRC_2;                 //    
    UINT32                    rsv_46AC;                 //    
    UINT32                  CAM_AF_CON;                 //    
    UINT32               CAM_AF_WINX_1;                 //    
    UINT32               CAM_AF_WINX_2;                 //    
    UINT32               CAM_AF_WINX_3;                 //    
    UINT32               CAM_AF_WINY_1;                 //    
    UINT32               CAM_AF_WINY_2;                 //    
    UINT32               CAM_AF_WINY_3;                 //    
    UINT32                 CAM_AF_SIZE;                 //    
    UINT32                    rsv_46D0;                 //    
    UINT32                CAM_AF_FLT_1;                 //    
    UINT32                CAM_AF_FLT_2;                 //    
    UINT32                CAM_AF_FLT_3;                 //    
    UINT32                   CAM_AF_TH;                 //    
    UINT32            CAM_AF_FLO_WIN_1;                 //    
    UINT32           CAM_AF_FLO_SIZE_1;                 //    
    UINT32            CAM_AF_FLO_WIN_2;                 //    
    UINT32           CAM_AF_FLO_SIZE_2;                 //    
    UINT32            CAM_AF_FLO_WIN_3;                 //    
    UINT32           CAM_AF_FLO_SIZE_3;                 //    
    UINT32               CAM_AF_FLO_TH;                 //    
    UINT32           CAM_AF_IMAGE_SIZE;                 //    
    UINT32                CAM_AF_FLT_4;                 //    
    UINT32                CAM_AF_FLT_5;                 //    
    UINT32               CAM_AF_STAT_L;                 //    
    UINT32               CAM_AF_STAT_M;                 //    
    UINT32          CAM_AF_FLO_STAT_1L;                 //    
    UINT32          CAM_AF_FLO_STAT_1M;                 //    
    UINT32          CAM_AF_FLO_STAT_1V;                 //    
    UINT32          CAM_AF_FLO_STAT_2L;                 //    
    UINT32          CAM_AF_FLO_STAT_2M;                 //    
    UINT32          CAM_AF_FLO_STAT_2V;                 //    
    UINT32          CAM_AF_FLO_STAT_3L;                 //    
    UINT32          CAM_AF_FLO_STAT_3M;                 //    
    UINT32          CAM_AF_FLO_STAT_3V;                 //    
    UINT32                rsv_4738[10];                 //                                                            
    UINT32                CAM_WBN_SIZE;                 //    
    UINT32                CAM_WBN_MODE;                 //    
    UINT32                 rsv_4768[2];                 //           
    UINT32                 CAM_FLK_CON;                 //    
    UINT32                CAM_FLK_OFST;                 //    
    UINT32                CAM_FLK_SIZE;                 //    
    UINT32                 CAM_FLK_NUM;                 //    
    UINT32                 CAM_LCS_CON;                 //    
    UINT32                  CAM_LCS_ST;                 //    
    UINT32                 CAM_LCS_AWS;                 //    
    UINT32                 CAM_LCS_FLR;                 //    
    UINT32              CAM_LCS_LRZR_1;                 //    
    UINT32              CAM_LCS_LRZR_2;                 //    
    UINT32                 rsv_4798[2];                 //           
    UINT32                 CAM_RRZ_CTL;                 //    
    UINT32              CAM_RRZ_IN_IMG;                 //    
    UINT32             CAM_RRZ_OUT_IMG;                 //    
    UINT32           CAM_RRZ_HORI_STEP;                 //    
    UINT32           CAM_RRZ_VERT_STEP;                 //    
    UINT32       CAM_RRZ_HORI_INT_OFST;                 //    
    UINT32       CAM_RRZ_HORI_SUB_OFST;                 //    
    UINT32       CAM_RRZ_VERT_INT_OFST;                 //    
    UINT32       CAM_RRZ_VERT_SUB_OFST;                 //    
    UINT32             CAM_RRZ_MODE_TH;                 //    
    UINT32            CAM_RRZ_MODE_CTL;                 //    
    UINT32                rsv_47CC[13];                 //           
    UINT32                 CAM_BPC_CON;                 //    
    UINT32                 CAM_BPC_TH1;                 //    
    UINT32                 CAM_BPC_TH2;                 //    
    UINT32                 CAM_BPC_TH3;                 //    
    UINT32                 CAM_BPC_TH4;                 //    
    UINT32                 CAM_BPC_DTC;                 //    
    UINT32                 CAM_BPC_COR;                 //    
    UINT32               CAM_BPC_TBLI1;                 //    
    UINT32               CAM_BPC_TBLI2;                 //    
    UINT32               CAM_BPC_TH1_C;                 //    
    UINT32               CAM_BPC_TH2_C;                 //    
    UINT32               CAM_BPC_TH3_C;                 //    
    UINT32                CAM_BPC_RMM1;                 //    
    UINT32                CAM_BPC_RMM2;                 //    
    UINT32          CAM_BPC_RMM_REVG_1;                 //    
    UINT32          CAM_BPC_RMM_REVG_2;                 //    
    UINT32            CAM_BPC_RMM_LEOS;                 //    
    UINT32            CAM_BPC_RMM_GCNT;                 //    
    UINT32                 rsv_4848[2];                 //           
    UINT32                 CAM_NR1_CON;                 //    
    UINT32              CAM_NR1_CT_CON;                 //    
    UINT32                CAM_BNR_RSV1;                 //    
    UINT32                CAM_BNR_RSV2;                 //    
    UINT32                 rsv_4860[8];                 //           
    UINT32              CAM_PGN_SATU_1;                 //    
    UINT32              CAM_PGN_SATU_2;                 //    
    UINT32              CAM_PGN_GAIN_1;                 //    
    UINT32              CAM_PGN_GAIN_2;                 //    
    UINT32              CAM_PGN_OFST_1;                 //    
    UINT32              CAM_PGN_OFST_2;                 //    
    UINT32                 rsv_4898[2];                 //           
    UINT32                CAM_DM_O_BYP;                 //    
    UINT32            CAM_DM_O_ED_FLAT;                 //    
    UINT32             CAM_DM_O_ED_NYQ;                 //    
    UINT32            CAM_DM_O_ED_STEP;                 //    
    UINT32             CAM_DM_O_RGB_HF;                 //    
    UINT32                CAM_DM_O_DOT;                 //    
    UINT32             CAM_DM_O_F1_ACT;                 //    
    UINT32             CAM_DM_O_F2_ACT;                 //    
    UINT32             CAM_DM_O_F3_ACT;                 //    
    UINT32             CAM_DM_O_F4_ACT;                 //    
    UINT32               CAM_DM_O_F1_L;                 //    
    UINT32               CAM_DM_O_F2_L;                 //    
    UINT32               CAM_DM_O_F3_L;                 //    
    UINT32               CAM_DM_O_F4_L;                 //    
    UINT32              CAM_DM_O_HF_RB;                 //    
    UINT32            CAM_DM_O_HF_GAIN;                 //    
    UINT32            CAM_DM_O_HF_COMP;                 //    
    UINT32        CAM_DM_O_HF_CORIN_TH;                 //    
    UINT32            CAM_DM_O_ACT_LUT;                 //    
    UINT32                    rsv_48EC;                 //    
    UINT32              CAM_DM_O_SPARE;                 //    
    UINT32                 CAM_DM_O_BB;                 //    
    UINT32                 rsv_48F8[6];                 //           
    UINT32                 CAM_CCL_GTC;                 //    
    UINT32                 CAM_CCL_ADC;                 //    
    UINT32                 CAM_CCL_BAC;                 //    
    UINT32                    rsv_491C;                 //    
    UINT32               CAM_G2G_CNV_1;                 //    
    UINT32               CAM_G2G_CNV_2;                 //    
    UINT32               CAM_G2G_CNV_3;                 //    
    UINT32               CAM_G2G_CNV_4;                 //    
    UINT32               CAM_G2G_CNV_5;                 //    
    UINT32               CAM_G2G_CNV_6;                 //    
    UINT32                CAM_G2G_CTRL;                 //    
    UINT32                 rsv_493C[3];                 //           
    UINT32                CAM_UNP_OFST;                 //    
    UINT32                    rsv_494C;                 //    
    UINT32                 CAM_C02_CON;                 //    
    UINT32           CAM_C02_CROP_CON1;                 //    
    UINT32           CAM_C02_CROP_CON2;                 //    
    UINT32                   rsv_495C;                  //    
    UINT32                 CAM_MFB_CON;                 //    
    UINT32             CAM_MFB_LL_CON1;                 //    
    UINT32             CAM_MFB_LL_CON2;                 //    
    UINT32             CAM_MFB_LL_CON3;                 //    
    UINT32             CAM_MFB_LL_CON4;                 //    
    UINT32             CAM_MFB_LL_CON5;                 //    
    UINT32             CAM_MFB_LL_CON6;                 //    
    UINT32                rsv_497C[17];                 //           
    UINT32                 CAM_LCE_CON;                 //    
    UINT32                  CAM_LCE_ZR;                 //    
    UINT32                 CAM_LCE_QUA;                 //    
    UINT32               CAM_LCE_DGC_1;                 //    
    UINT32               CAM_LCE_DGC_2;                 //    
    UINT32                  CAM_LCE_GM;                 //    
    UINT32                 rsv_49D8[2];                 //           
    UINT32              CAM_LCE_SLM_LB;                 //    
    UINT32            CAM_LCE_SLM_SIZE;                 //    
    UINT32                CAM_LCE_OFST;                 //    
    UINT32                CAM_LCE_BIAS;                 //    
    UINT32          CAM_LCE_IMAGE_SIZE;                 //    
    UINT32                 rsv_49F4[3];                 //           
    UINT32              CAM_CPG_SATU_1;                 //                                                            
    UINT32              CAM_CPG_SATU_2;                 //                                                            
    UINT32              CAM_CPG_GAIN_1;                 //                                                            
    UINT32              CAM_CPG_GAIN_2;                 //                                                            
    UINT32              CAM_CPG_OFST_1;                 //                                                            
    UINT32                rsv_4A18[1];                  //                                                     
    UINT32                 CAM_C42_CON;                 //    
    UINT32                CAM_ANR_CON1;                 //    
    UINT32                CAM_ANR_CON2;                 //    
    UINT32                CAM_ANR_CON3;                 //    
    UINT32                CAM_ANR_YAD1;                 //    
    UINT32                CAM_ANR_YAD2;                 //    
    UINT32               CAM_ANR_4LUT1;                 //    
    UINT32               CAM_ANR_4LUT2;                 //    
    UINT32               CAM_ANR_4LUT3;                 //    
    UINT32                 CAM_ANR_PTY;                 //    
    UINT32                 CAM_ANR_CAD;                 //    
    UINT32                 CAM_ANR_PTC;                 //    
    UINT32                CAM_ANR_LCE1;                 //    
    UINT32                CAM_ANR_LCE2;                 //    
    UINT32                 CAM_ANR_HP1;                 //    
    UINT32                 CAM_ANR_HP2;                 //    
    UINT32                 CAM_ANR_HP3;                 //    
    UINT32                CAM_ANR_ACTY;                 //    
    UINT32                CAM_ANR_ACTC;                 //    
    UINT32                CAM_ANR_RSV1;                 //    
    UINT32                CAM_ANR_RSV2;                 //    
    UINT32                 rsv_4A70[4];                 //           
    UINT32                 CAM_CCR_CON;                 //    
    UINT32                CAM_CCR_YLUT;                 //    
    UINT32               CAM_CCR_UVLUT;                 //    
    UINT32               CAM_CCR_YLUT2;                 //    
    UINT32            CAM_CCR_SAT_CTRL;                 //    
    UINT32            CAM_CCR_UVLUT_SP;                 //    
    UINT32                 rsv_4A98[2];                 //           
    UINT32           CAM_SEEE_SRK_CTRL;                 //    
    UINT32          CAM_SEEE_CLIP_CTRL;                 //    
    UINT32         CAM_SEEE_FLT_CTRL_1;                 //    
    UINT32         CAM_SEEE_FLT_CTRL_2;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_01;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_02;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_03;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_04;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_05;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_06;                 //    
    UINT32          CAM_SEEE_EDTR_CTRL;                 //    
    UINT32      CAM_SEEE_OUT_EDGE_CTRL;                 //    
    UINT32          CAM_SEEE_SE_Y_CTRL;                 //    
    UINT32     CAM_SEEE_SE_EDGE_CTRL_1;                 //    
    UINT32     CAM_SEEE_SE_EDGE_CTRL_2;                 //    
    UINT32     CAM_SEEE_SE_EDGE_CTRL_3;                 //    
    UINT32      CAM_SEEE_SE_SPECL_CTRL;                 //    
    UINT32     CAM_SEEE_SE_CORE_CTRL_1;                 //    
    UINT32     CAM_SEEE_SE_CORE_CTRL_2;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_07;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_08;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_09;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_10;                 //    
    UINT32       CAM_SEEE_GLUT_CTRL_11;                 //    
    UINT32             CAM_CRZ_CONTROL;                 //    
    UINT32              CAM_CRZ_IN_IMG;                 //    
    UINT32             CAM_CRZ_OUT_IMG;                 //    
    UINT32           CAM_CRZ_HORI_STEP;                 //    
    UINT32           CAM_CRZ_VERT_STEP;                 //    
    UINT32  CAM_CRZ_LUMA_HORI_INT_OFST;                 //    
    UINT32  CAM_CRZ_LUMA_HORI_SUB_OFST;                 //    
    UINT32  CAM_CRZ_LUMA_VERT_INT_OFST;                 //    
    UINT32  CAM_CRZ_LUMA_VERT_SUB_OFST;                 //    
    UINT32  CAM_CRZ_CHRO_HORI_INT_OFST;                 //    
    UINT32  CAM_CRZ_CHRO_HORI_SUB_OFST;                 //    
    UINT32  CAM_CRZ_CHRO_VERT_INT_OFST;                 //    
    UINT32  CAM_CRZ_CHRO_VERT_SUB_OFST;                 //    
    UINT32               CAM_CRZ_DER_1;                 //    
    UINT32               CAM_CRZ_DER_2;                 //    
    UINT32                rsv_4B3C[25];                 //           
    UINT32             CAM_G2C_CONV_0A;                 //    
    UINT32             CAM_G2C_CONV_0B;                 //    
    UINT32             CAM_G2C_CONV_1A;                 //    
    UINT32             CAM_G2C_CONV_1B;                 //    
    UINT32             CAM_G2C_CONV_2A;                 //    
    UINT32             CAM_G2C_CONV_2B;                 //    
    UINT32         CAM_G2C_SHADE_CON_1;                 //    
    UINT32         CAM_G2C_SHADE_CON_2;                 //    
    UINT32         CAM_G2C_SHADE_CON_3;                 //    
    UINT32           CAM_G2C_SHADE_TAR;                 //    
    UINT32            CAM_G2C_SHADE_SP;                 //    
    UINT32                rsv_4BCC[25];                 //                                                            
    UINT32            CAM_SRZ1_CONTROL;                 //    
    UINT32             CAM_SRZ1_IN_IMG;                 //    
    UINT32            CAM_SRZ1_OUT_IMG;                 //    
    UINT32          CAM_SRZ1_HORI_STEP;                 //    
    UINT32          CAM_SRZ1_VERT_STEP;                 //    
    UINT32      CAM_SRZ1_HORI_INT_OFST;                 //    
    UINT32      CAM_SRZ1_HORI_SUB_OFST;                 //    
    UINT32      CAM_SRZ1_VERT_INT_OFST;                 //    
    UINT32      CAM_SRZ1_VERT_SUB_OFST;                 //    
    UINT32                 rsv_4C54[3];                 //           
    UINT32            CAM_SRZ2_CONTROL;                 //    
    UINT32             CAM_SRZ2_IN_IMG;                 //    
    UINT32            CAM_SRZ2_OUT_IMG;                 //    
    UINT32          CAM_SRZ2_HORI_STEP;                 //    
    UINT32          CAM_SRZ2_VERT_STEP;                 //    
    UINT32      CAM_SRZ2_HORI_INT_OFST;                 //    
    UINT32      CAM_SRZ2_HORI_SUB_OFST;                 //    
    UINT32      CAM_SRZ2_VERT_INT_OFST;                 //    
    UINT32      CAM_SRZ2_VERT_SUB_OFST;                 //    
    UINT32                 rsv_4C84[3];                 //           
    UINT32             CAM_MIX1_CTRL_0;                 //    
    UINT32             CAM_MIX1_CTRL_1;                 //    
    UINT32              CAM_MIX1_SPARE;                 //    
    UINT32                    rsv_4C9C;                 //    
    UINT32             CAM_MIX2_CTRL_0;                 //    
    UINT32             CAM_MIX2_CTRL_1;                 //    
    UINT32              CAM_MIX2_SPARE;                 //    
    UINT32                    rsv_4CAC;                 //    
    UINT32             CAM_MIX3_CTRL_0;                 //    
    UINT32             CAM_MIX3_CTRL_1;                 //    
    UINT32              CAM_MIX3_SPARE;                 //    
    UINT32                    rsv_4CBC;                 //    
    UINT32              CAM_NR3D_BLEND;                 //    
    UINT32          CAM_NR3D_FBCNT_OFF;                 //    
    UINT32          CAM_NR3D_FBCNT_SIZ;                 //    
    UINT32           CAM_NR3D_FB_COUNT;                 //    
    UINT32            CAM_NR3D_LMT_CPX;                 //    
    UINT32         CAM_NR3D_LMT_Y_CON1;                 //    
    UINT32         CAM_NR3D_LMT_Y_CON2;                 //    
    UINT32         CAM_NR3D_LMT_Y_CON3;                 //    
    UINT32         CAM_NR3D_LMT_U_CON1;                 //    
    UINT32         CAM_NR3D_LMT_U_CON2;                 //    
    UINT32         CAM_NR3D_LMT_U_CON3;                 //    
    UINT32         CAM_NR3D_LMT_V_CON1;                 //    
    UINT32         CAM_NR3D_LMT_V_CON2;                 //    
    UINT32         CAM_NR3D_LMT_V_CON3;                 //    
    UINT32               CAM_NR3D_CTRL;                 //    
    UINT32             CAM_NR3D_ON_OFF;                 //    
    UINT32             CAM_NR3D_ON_SIZ;                 //    
    UINT32             CAM_NR3D_SPARE0;                 //    



    UINT32       CAM_EIS_PREP_ME_CTRL1;                 //    
    UINT32       CAM_EIS_PREP_ME_CTRL2;                 //    
    UINT32              CAM_EIS_LMV_TH;                 //    
    UINT32           CAM_EIS_FL_OFFSET;                 //    
    UINT32           CAM_EIS_MB_OFFSET;                 //    
    UINT32         CAM_EIS_MB_INTERVAL;                 //    
    UINT32                 CAM_EIS_GMV;                 //    
    UINT32            CAM_EIS_ERR_CTRL;                 //    
    UINT32          CAM_EIS_IMAGE_CTRL;                 //    
    UINT32                 rsv_4DE4[7];                 //           
    UINT32                 CAM_DMX_CTL;                 //    
    UINT32                CAM_DMX_CROP;                 //    
    UINT32               CAM_DMX_VSIZE;                 //    
    UINT32                    rsv_4E0C;                 //    
    UINT32                 CAM_BMX_CTL;                 //    
    UINT32                CAM_BMX_CROP;                 //    
    UINT32               CAM_BMX_VSIZE;                 //    
    UINT32                    rsv_4E1C;                 //    
    UINT32                 CAM_RMX_CTL;                 //    
    UINT32                CAM_RMX_CROP;                 //    
    UINT32               CAM_RMX_VSIZE;                 //    
    UINT32                 rsv_4E2C[9];                 //           
    UINT32                 CAM_UFE_CON;                 //    


    UINT32                 CAM_SL2_CEN;                 //    
    UINT32             CAM_SL2_MAX0_RR;                 //    
    UINT32             CAM_SL2_MAX1_RR;                 //    
    UINT32             CAM_SL2_MAX2_RR;                 //    
    UINT32                 CAM_SL2_HRZ;                 //    
    UINT32                CAM_SL2_XOFF;                 //    
    UINT32                CAM_SL2_YOFF;                 //    
    UINT32                    rsv_4F5C;                 //    
    UINT32                CAM_SL2B_CEN;                 //    
    UINT32            CAM_SL2B_MAX0_RR;                 //    
    UINT32            CAM_SL2B_MAX1_RR;                 //    
    UINT32            CAM_SL2B_MAX2_RR;                 //    
    UINT32                CAM_SL2B_HRZ;                 //    
    UINT32               CAM_SL2B_XOFF;                 //    
    UINT32               CAM_SL2B_YOFF;                 //    
    UINT32                 rsv_4F7C[9];                 //           
    UINT32               CAM_CRSP_CTRL;                 //    
    UINT32                   rsv_4FA4;                  //    
    UINT32            CAM_CRSP_OUT_IMG;                 //    
    UINT32          CAM_CRSP_STEP_OFST;                 //    
    UINT32             CAM_CRSP_CROP_X;                 //    
    UINT32             CAM_CRSP_CROP_Y;                 //    
    UINT32                 rsv_4FB8[2];                 //           
    UINT32                CAM_SL2C_CEN;                 //    
    UINT32            CAM_SL2C_MAX0_RR;                 //    
    UINT32            CAM_SL2C_MAX1_RR;                 //    
    UINT32            CAM_SL2C_MAX2_RR;                 //    
    UINT32                CAM_SL2C_HRZ;                 //    
    UINT32               CAM_SL2C_XOFF;                 //    
    UINT32               CAM_SL2C_YOFF;                 //    

    UINT32                CAM_GGM_CTRL;                 //    

    UINT32                CAM_PCA_CON1;                 //    
    UINT32                CAM_PCA_CON2;                 //    

    UINT32           CAM_TILE_RING_CON1;                //    
    UINT32           CAM_CTL_IMGI_SIZE;                 //    



    UINT32            CAM_TG2_SEN_MODE;                 //    
    UINT32              CAM_TG2_VF_CON;                 //    
    UINT32        CAM_TG2_SEN_GRAB_PXL;                 //    
    UINT32        CAM_TG2_SEN_GRAB_LIN;                 //    
    UINT32            CAM_TG2_PATH_CFG;                 //    
    UINT32           CAM_TG2_MEMIN_CTL;                 //    


    UINT32            CAM_OBC_D_OFFST0;                 //    
    UINT32            CAM_OBC_D_OFFST1;                 //    
    UINT32            CAM_OBC_D_OFFST2;                 //    
    UINT32            CAM_OBC_D_OFFST3;                 //    
    UINT32             CAM_OBC_D_GAIN0;                 //    
    UINT32             CAM_OBC_D_GAIN1;                 //    
    UINT32             CAM_OBC_D_GAIN2;                 //    
    UINT32             CAM_OBC_D_GAIN3;                 //    
    UINT32                 rsv_6520[4];                 //           
    UINT32              CAM_LSC_D_CTL1;                 //    
    UINT32              CAM_LSC_D_CTL2;                 //    
    UINT32              CAM_LSC_D_CTL3;                 //    
    UINT32            CAM_LSC_D_LBLOCK;                 //    
    UINT32             CAM_LSC_D_RATIO;                 //    
    UINT32        CAM_LSC_D_TPIPE_OFST;                 //    
    UINT32        CAM_LSC_D_TPIPE_SIZE;                 //    
    UINT32           CAM_LSC_D_GAIN_TH;                 //    
    UINT32            CAM_RPG_D_SATU_1;                 //    
    UINT32            CAM_RPG_D_SATU_2;                 //    
    UINT32            CAM_RPG_D_GAIN_1;                 //    
    UINT32            CAM_RPG_D_GAIN_2;                 //    
    UINT32            CAM_RPG_D_OFST_1;                 //    
    UINT32            CAM_RPG_D_OFST_2;                 //    
    UINT32                rsv_6568[18];                 //           
    UINT32           CAM_AWB_D_WIN_ORG;                 //    
    UINT32          CAM_AWB_D_WIN_SIZE;                 //    
    UINT32           CAM_AWB_D_WIN_PIT;                 //    
    UINT32           CAM_AWB_D_WIN_NUM;                 //    
    UINT32           CAM_AWB_D_GAIN1_0;                 //    
    UINT32           CAM_AWB_D_GAIN1_1;                 //    
    UINT32            CAM_AWB_D_LMT1_0;                 //    
    UINT32            CAM_AWB_D_LMT1_1;                 //    
    UINT32           CAM_AWB_D_LOW_THR;                 //    
    UINT32            CAM_AWB_D_HI_THR;                 //    
    UINT32        CAM_AWB_D_PIXEL_CNT0;                 //    
    UINT32        CAM_AWB_D_PIXEL_CNT1;                 //    
    UINT32        CAM_AWB_D_PIXEL_CNT2;                 //    
    UINT32           CAM_AWB_D_ERR_THR;                 //    
    UINT32               CAM_AWB_D_ROT;                 //    
    UINT32              CAM_AWB_D_L0_X;                 //    
    UINT32              CAM_AWB_D_L0_Y;                 //    
    UINT32              CAM_AWB_D_L1_X;                 //    
    UINT32              CAM_AWB_D_L1_Y;                 //    
    UINT32              CAM_AWB_D_L2_X;                 //    
    UINT32              CAM_AWB_D_L2_Y;                 //    
    UINT32              CAM_AWB_D_L3_X;                 //    
    UINT32              CAM_AWB_D_L3_Y;                 //    
    UINT32              CAM_AWB_D_L4_X;                 //    
    UINT32              CAM_AWB_D_L4_Y;                 //    
    UINT32              CAM_AWB_D_L5_X;                 //    
    UINT32              CAM_AWB_D_L5_Y;                 //    
    UINT32              CAM_AWB_D_L6_X;                 //    
    UINT32              CAM_AWB_D_L6_Y;                 //    
    UINT32              CAM_AWB_D_L7_X;                 //    
    UINT32              CAM_AWB_D_L7_Y;                 //    
    UINT32              CAM_AWB_D_L8_X;                 //    
    UINT32              CAM_AWB_D_L8_Y;                 //    
    UINT32              CAM_AWB_D_L9_X;                 //    
    UINT32              CAM_AWB_D_L9_Y;                 //    
    UINT32             CAM_AWB_D_SPARE;                 //    
    UINT32                 rsv_6640[4];                 //           
    UINT32            CAM_AE_D_HST_CTL;                 //    
    UINT32            CAM_AE_D_GAIN2_0;                 //    
    UINT32            CAM_AE_D_GAIN2_1;                 //    
    UINT32             CAM_AE_D_LMT2_0;                 //    
    UINT32             CAM_AE_D_LMT2_1;                 //    
    UINT32           CAM_AE_D_RC_CNV_0;                 //    
    UINT32           CAM_AE_D_RC_CNV_1;                 //    
    UINT32           CAM_AE_D_RC_CNV_2;                 //    
    UINT32           CAM_AE_D_RC_CNV_3;                 //    
    UINT32           CAM_AE_D_RC_CNV_4;                 //    
    UINT32           CAM_AE_D_YGAMMA_0;                 //    
    UINT32           CAM_AE_D_YGAMMA_1;                 //    
    UINT32            CAM_AE_D_HST_SET;                 //    
    UINT32           CAM_AE_D_HST0_RNG;                 //    
    UINT32           CAM_AE_D_HST1_RNG;                 //    
    UINT32           CAM_AE_D_HST2_RNG;                 //    
    UINT32           CAM_AE_D_HST3_RNG;                 //    
    UINT32              CAM_AE_D_SPARE;                 //    
    UINT32                 rsv_6698[2];                 //           
    UINT32              CAM_SGG1_D_PGN;                 //    
    UINT32           CAM_SGG1_D_GMRC_1;                 //    
    UINT32           CAM_SGG1_D_GMRC_2;                 //    
    UINT32                    rsv_66AC;                 //    
    UINT32                CAM_AF_D_CON;                 //    
    UINT32             CAM_AF_D_WINX_1;                 //    
    UINT32             CAM_AF_D_WINX_2;                 //    
    UINT32             CAM_AF_D_WINX_3;                 //    
    UINT32             CAM_AF_D_WINY_1;                 //    
    UINT32             CAM_AF_D_WINY_2;                 //    
    UINT32             CAM_AF_D_WINY_3;                 //    
    UINT32               CAM_AF_D_SIZE;                 //    
    UINT32                    rsv_66D0;                 //    
    UINT32              CAM_AF_D_FLT_1;                 //    
    UINT32              CAM_AF_D_FLT_2;                 //    
    UINT32              CAM_AF_D_FLT_3;                 //    
    UINT32                 CAM_AF_D_TH;                 //    
    UINT32          CAM_AF_D_FLO_WIN_1;                 //    
    UINT32         CAM_AF_D_FLO_SIZE_1;                 //    
    UINT32          CAM_AF_D_FLO_WIN_2;                 //    
    UINT32         CAM_AF_D_FLO_SIZE_2;                 //    
    UINT32          CAM_AF_D_FLO_WIN_3;                 //    
    UINT32         CAM_AF_D_FLO_SIZE_3;                 //    
    UINT32             CAM_AF_D_FLO_TH;                 //    
    UINT32         CAM_AF_D_IMAGE_SIZE;                 //    
    UINT32              CAM_AF_D_FLT_4;                 //    
    UINT32              CAM_AF_D_FLT_5;                 //    
    UINT32             CAM_AF_D_STAT_L;                 //    
    UINT32             CAM_AF_D_STAT_M;                 //    
    UINT32        CAM_AF_D_FLO_STAT_1L;                 //    
    UINT32        CAM_AF_D_FLO_STAT_1M;                 //    
    UINT32        CAM_AF_D_FLO_STAT_1V;                 //    
    UINT32        CAM_AF_D_FLO_STAT_2L;                 //    
    UINT32        CAM_AF_D_FLO_STAT_2M;                 //    
    UINT32        CAM_AF_D_FLO_STAT_2V;                 //    
    UINT32        CAM_AF_D_FLO_STAT_3L;                 //    
    UINT32        CAM_AF_D_FLO_STAT_3M;                 //    
    UINT32        CAM_AF_D_FLO_STAT_3V;                 //    
    UINT32                 rsv_6738[2];                 //           
    UINT32               CAM_W2G_D_BLD;                 //    
    UINT32              CAM_W2G_D_TH_1;                 //    
    UINT32              CAM_W2G_D_TH_2;                 //    
    UINT32           CAM_W2G_D_CTL_OFT;                 //    
    UINT32                 rsv_6750[4];                 //           
    UINT32              CAM_WBN_D_SIZE;                 //    
    UINT32              CAM_WBN_D_MODE;                 //    
    UINT32                 rsv_6768[6];                 //           
    UINT32               CAM_LCS_D_CON;                 //    
    UINT32                CAM_LCS_D_ST;                 //    
    UINT32               CAM_LCS_D_AWS;                 //    
    UINT32               CAM_LCS_D_FLR;                 //    
    UINT32            CAM_LCS_D_LRZR_1;                 //    
    UINT32            CAM_LCS_D_LRZR_2;                 //    
    UINT32                 rsv_6798[2];                 //           
    UINT32               CAM_RRZ_D_CTL;                 //    
    UINT32            CAM_RRZ_D_IN_IMG;                 //    
    UINT32           CAM_RRZ_D_OUT_IMG;                 //    
    UINT32         CAM_RRZ_D_HORI_STEP;                 //    
    UINT32         CAM_RRZ_D_VERT_STEP;                 //    
    UINT32     CAM_RRZ_D_HORI_INT_OFST;                 //    
    UINT32     CAM_RRZ_D_HORI_SUB_OFST;                 //    
    UINT32     CAM_RRZ_D_VERT_INT_OFST;                 //    
    UINT32     CAM_RRZ_D_VERT_SUB_OFST;                 //    
    UINT32           CAM_RRZ_D_MODE_TH;                 //    
    UINT32          CAM_RRZ_D_MODE_CTL;                 //    
    UINT32                rsv_67CC[13];                 //           
    UINT32               CAM_BPC_D_CON;                 //    
    UINT32               CAM_BPC_D_TH1;                 //    
    UINT32               CAM_BPC_D_TH2;                 //    
    UINT32               CAM_BPC_D_TH3;                 //    
    UINT32               CAM_BPC_D_TH4;                 //    
    UINT32               CAM_BPC_D_DTC;                 //    
    UINT32               CAM_BPC_D_COR;                 //    
    UINT32             CAM_BPC_D_TBLI1;                 //    
    UINT32             CAM_BPC_D_TBLI2;                 //    
    UINT32             CAM_BPC_D_TH1_C;                 //    
    UINT32             CAM_BPC_D_TH2_C;                 //    
    UINT32             CAM_BPC_D_TH3_C;                 //    
    UINT32              CAM_BPC_D_RMM1;                 //    
    UINT32              CAM_BPC_D_RMM2;                 //    
    UINT32        CAM_BPC_D_RMM_REVG_1;                 //    
    UINT32        CAM_BPC_D_RMM_REVG_2;                 //    
    UINT32          CAM_BPC_D_RMM_LEOS;                 //    
    UINT32          CAM_BPC_D_RMM_GCNT;                 //    
    UINT32                 rsv_6848[2];                 //           
    UINT32               CAM_NR1_D_CON;                 //    
    UINT32            CAM_NR1_D_CT_CON;                 //    

    UINT32               CAM_DMX_D_CTL;                  //    
    UINT32              CAM_DMX_D_CROP;                  //    
    UINT32             CAM_DMX_D_VSIZE;                  //    
    UINT32                    rsv_6E0C;                  //    
    UINT32               CAM_BMX_D_CTL;                  //    
    UINT32              CAM_BMX_D_CROP;                  //    
    UINT32             CAM_BMX_D_VSIZE;                  //    
    UINT32                    rsv_6E1C;                  //    
    UINT32               CAM_RMX_D_CTL;                  //    
    UINT32              CAM_RMX_D_CROP;                  //    
    UINT32             CAM_RMX_D_VSIZE;                  //        

    UINT32          CAM_TDRI_BASE_ADDR;                 //    
    UINT32          CAM_TDRI_OFST_ADDR;                 //    
    UINT32              CAM_TDRI_XSIZE;                 //    
    UINT32          CAM_CQ0I_BASE_ADDR;                 //    
    UINT32              CAM_CQ0I_XSIZE;                 //    
    UINT32        CAM_CQ0I_D_BASE_ADDR;                 //    
    UINT32            CAM_CQ0I_D_XSIZE;                 //    
    UINT32        CAM_VERTICAL_FLIP_EN;                 //    
    UINT32          CAM_DMA_SOFT_RESET;                 //    
    UINT32           CAM_LAST_ULTRA_EN;                 //    
    UINT32          CAM_IMGI_SLOW_DOWN;                 //    
    UINT32          CAM_IMGI_BASE_ADDR;                 //    
    UINT32          CAM_IMGI_OFST_ADDR;                 //    
    UINT32              CAM_IMGI_XSIZE;                 //    
    UINT32              CAM_IMGI_YSIZE;                 //    
    UINT32             CAM_IMGI_STRIDE;                 //    
    UINT32                    rsv_7244;                 //    
    UINT32                CAM_IMGI_CON;                 //    
    UINT32               CAM_IMGI_CON2;                 //    

    UINT32          CAM_BPCI_BASE_ADDR;                  //    
    UINT32          CAM_BPCI_OFST_ADDR;                  //    
    UINT32              CAM_BPCI_XSIZE;                  //    
    UINT32              CAM_BPCI_YSIZE;                  //    
    UINT32             CAM_BPCI_STRIDE;                  //    
    UINT32                CAM_BPCI_CON;                  //    
    UINT32               CAM_BPCI_CON2;                  //    

    UINT32          CAM_LSCI_BASE_ADDR;                  //    
    UINT32          CAM_LSCI_OFST_ADDR;                  //    
    UINT32              CAM_LSCI_XSIZE;                  //    
    UINT32              CAM_LSCI_YSIZE;                  //    
    UINT32             CAM_LSCI_STRIDE;                  //    
    UINT32                CAM_LSCI_CON;                  //    
    UINT32               CAM_LSCI_CON2;                  //    
    UINT32          CAM_UFDI_BASE_ADDR;                  //    
    UINT32          CAM_UFDI_OFST_ADDR;                  //    
    UINT32              CAM_UFDI_XSIZE;                  //    
    UINT32              CAM_UFDI_YSIZE;                  //    
    UINT32             CAM_UFDI_STRIDE;                  //    
    UINT32                CAM_UFDI_CON;                  //    
    UINT32               CAM_UFDI_CON2;                  //    
    UINT32          CAM_LCEI_BASE_ADDR;                  //    
    UINT32          CAM_LCEI_OFST_ADDR;                  //    
    UINT32              CAM_LCEI_XSIZE;                  //    
    UINT32              CAM_LCEI_YSIZE;                  //    
    UINT32             CAM_LCEI_STRIDE;                  //    
    UINT32                CAM_LCEI_CON;                  //    
    UINT32               CAM_LCEI_CON2;                  //    
    UINT32          CAM_VIPI_BASE_ADDR;                  //    
    UINT32          CAM_VIPI_OFST_ADDR;                  //    
    UINT32              CAM_VIPI_XSIZE;                  //    
    UINT32              CAM_VIPI_YSIZE;                  //    
    UINT32             CAM_VIPI_STRIDE;                  //    
    UINT32                    rsv_72D4;                  //    
    UINT32                CAM_VIPI_CON;                  //    
    UINT32               CAM_VIPI_CON2;                  //    
    UINT32         CAM_VIP2I_BASE_ADDR;                  //    
    UINT32         CAM_VIP2I_OFST_ADDR;                  //    
    UINT32             CAM_VIP2I_XSIZE;                  //    
    UINT32             CAM_VIP2I_YSIZE;                  //    
    UINT32            CAM_VIP2I_STRIDE;                  //    
    UINT32                    rsv_72F4;                  //    
    UINT32               CAM_VIP2I_CON;                  //    
    UINT32              CAM_VIP2I_CON2;                  //    
    UINT32          CAM_IMGO_BASE_ADDR;                  //    
    UINT32          CAM_IMGO_OFST_ADDR;                  //    
    UINT32              CAM_IMGO_XSIZE;                  //    
    UINT32              CAM_IMGO_YSIZE;                  //    
    UINT32             CAM_IMGO_STRIDE;                  //    
    UINT32                CAM_IMGO_CON;                  //    
    UINT32               CAM_IMGO_CON2;                  //    
    UINT32               CAM_IMGO_CROP;                  //    

    UINT32          CAM_RRZO_BASE_ADDR;                  //    
    UINT32          CAM_RRZO_OFST_ADDR;                  //    
    UINT32              CAM_RRZO_XSIZE;                  //    
    UINT32              CAM_RRZO_YSIZE;                  //    
    UINT32             CAM_RRZO_STRIDE;                  //    
    UINT32                CAM_RRZO_CON;                  //    
    UINT32               CAM_RRZO_CON2;                  //    
    UINT32               CAM_RRZO_CROP;                  //    
    UINT32          CAM_LCSO_BASE_ADDR;                  //    
    UINT32          CAM_LCSO_OFST_ADDR;                  //    
    UINT32              CAM_LCSO_XSIZE;                  //    
    UINT32              CAM_LCSO_YSIZE;                  //    
    UINT32             CAM_LCSO_STRIDE;                  //    
    UINT32                CAM_LCSO_CON;                  //    
    UINT32               CAM_LCSO_CON2;                  //    
    UINT32          CAM_EISO_BASE_ADDR;                  //    
    UINT32              CAM_EISO_XSIZE;                  //    
    UINT32           CAM_AFO_BASE_ADDR;                  //    
    UINT32               CAM_AFO_XSIZE;                  //    
    UINT32         CAM_ESFKO_BASE_ADDR;                  //    
    UINT32             CAM_ESFKO_XSIZE;                  //    


    UINT32             CAM_ESFKO_YSIZE;                  //    
    UINT32            CAM_ESFKO_STRIDE;                  //    
    UINT32               CAM_ESFKO_CON;                  //    
    UINT32              CAM_ESFKO_CON2;                  //    

    UINT32           CAM_AAO_BASE_ADDR;                  //    
    UINT32           CAM_AAO_OFST_ADDR;                  //    
    UINT32               CAM_AAO_XSIZE;                  //    
    UINT32               CAM_AAO_YSIZE;                  //    
    UINT32              CAM_AAO_STRIDE;                  //    
    UINT32                 CAM_AAO_CON;                  //    
    UINT32                CAM_AAO_CON2;                  //    

    UINT32         CAM_VIP3I_BASE_ADDR;                  //    
    UINT32         CAM_VIP3I_OFST_ADDR;                  //    
    UINT32             CAM_VIP3I_XSIZE;                  //    
    UINT32             CAM_VIP3I_YSIZE;                  //    
    UINT32            CAM_VIP3I_STRIDE;                  //    
    UINT32                    rsv_73B8;                  //    
    UINT32               CAM_VIP3I_CON;                  //    
    UINT32              CAM_VIP3I_CON2;                  //    
    UINT32          CAM_UFEO_BASE_ADDR;                  //    
    UINT32          CAM_UFEO_OFST_ADDR;                  //    
    UINT32              CAM_UFEO_XSIZE;                  //    
    UINT32              CAM_UFEO_YSIZE;                  //    
    UINT32             CAM_UFEO_STRIDE;                  //    
    UINT32                CAM_UFEO_CON;                  //    
    UINT32               CAM_UFEO_CON2;                  //    
    UINT32          CAM_MFBO_BASE_ADDR;                  //    
    UINT32          CAM_MFBO_OFST_ADDR;                  //    
    UINT32              CAM_MFBO_XSIZE;                  //    
    UINT32              CAM_MFBO_YSIZE;                  //    
    UINT32             CAM_MFBO_STRIDE;                  //    
    UINT32                CAM_MFBO_CON;                  //    
    UINT32               CAM_MFBO_CON2;                  //    
    UINT32               CAM_MFBO_CROP;                  //    
    UINT32        CAM_IMG3BO_BASE_ADDR;                  //    
    UINT32        CAM_IMG3BO_OFST_ADDR;                  //    
    UINT32            CAM_IMG3BO_XSIZE;                  //    
    UINT32            CAM_IMG3BO_YSIZE;                  //    
    UINT32           CAM_IMG3BO_STRIDE;                  //    
    UINT32              CAM_IMG3BO_CON;                  //    
    UINT32             CAM_IMG3BO_CON2;                  //    
    UINT32             CAM_IMG3BO_CROP;                  //    
    UINT32        CAM_IMG3CO_BASE_ADDR;                  //    
    UINT32        CAM_IMG3CO_OFST_ADDR;                  //    
    UINT32            CAM_IMG3CO_XSIZE;                  //    
    UINT32            CAM_IMG3CO_YSIZE;                  //    
    UINT32           CAM_IMG3CO_STRIDE;                  //    
    UINT32              CAM_IMG3CO_CON;                  //    
    UINT32             CAM_IMG3CO_CON2;                  //    
    UINT32             CAM_IMG3CO_CROP;                  //    
    UINT32         CAM_IMG2O_BASE_ADDR;                  //    
    UINT32         CAM_IMG2O_OFST_ADDR;                  //    
    UINT32             CAM_IMG2O_XSIZE;                  //    
    UINT32             CAM_IMG2O_YSIZE;                  //    
    UINT32            CAM_IMG2O_STRIDE;                  //    
    UINT32               CAM_IMG2O_CON;                  //    
    UINT32              CAM_IMG2O_CON2;                  //    
    UINT32              CAM_IMG2O_CROP;                  //    
    UINT32         CAM_IMG3O_BASE_ADDR;                  //    
    UINT32         CAM_IMG3O_OFST_ADDR;                  //    
    UINT32             CAM_IMG3O_XSIZE;                  //    
    UINT32             CAM_IMG3O_YSIZE;                  //    
    UINT32            CAM_IMG3O_STRIDE;                  //    
    UINT32               CAM_IMG3O_CON;                  //    
    UINT32              CAM_IMG3O_CON2;                  //    
    UINT32              CAM_IMG3O_CROP;                  //    
    UINT32           CAM_FEO_BASE_ADDR;                  //    
    UINT32           CAM_FEO_OFST_ADDR;                  //    
    UINT32               CAM_FEO_XSIZE;                  //    
    UINT32               CAM_FEO_YSIZE;                  //    
    UINT32              CAM_FEO_STRIDE;                  //    
    UINT32                 CAM_FEO_CON;                  //    
    UINT32                CAM_FEO_CON2;                  //    



    UINT32        CAM_BPCI_D_BASE_ADDR;                  //    
    UINT32        CAM_BPCI_D_OFST_ADDR;                  //    
    UINT32            CAM_BPCI_D_XSIZE;                  //    
    UINT32            CAM_BPCI_D_YSIZE;                  //    
    UINT32           CAM_BPCI_D_STRIDE;                  //    
    UINT32              CAM_BPCI_D_CON;                  //    
    UINT32             CAM_BPCI_D_CON2;                  //    

    UINT32        CAM_LSCI_D_BASE_ADDR;                  //    
    UINT32        CAM_LSCI_D_OFST_ADDR;                  //    
    UINT32            CAM_LSCI_D_XSIZE;                  //    
    UINT32            CAM_LSCI_D_YSIZE;                  //    
    UINT32           CAM_LSCI_D_STRIDE;                  //    
    UINT32              CAM_LSCI_D_CON;                  //    
    UINT32             CAM_LSCI_D_CON2;                  //    

    UINT32        CAM_IMGO_D_BASE_ADDR;                  //    
    UINT32        CAM_IMGO_D_OFST_ADDR;                  //    
    UINT32            CAM_IMGO_D_XSIZE;                  //    
    UINT32            CAM_IMGO_D_YSIZE;                  //    
    UINT32           CAM_IMGO_D_STRIDE;                  //    
    UINT32              CAM_IMGO_D_CON;                  //    
    UINT32             CAM_IMGO_D_CON2;                  //    
    UINT32             CAM_IMGO_D_CROP;                  //    

    UINT32        CAM_RRZO_D_BASE_ADDR;                  //    
    UINT32        CAM_RRZO_D_OFST_ADDR;                  //    
    UINT32            CAM_RRZO_D_XSIZE;                  //    
    UINT32            CAM_RRZO_D_YSIZE;                  //    
    UINT32           CAM_RRZO_D_STRIDE;                  //    
    UINT32              CAM_RRZO_D_CON;                  //    
    UINT32             CAM_RRZO_D_CON2;                  //    
    UINT32             CAM_RRZO_D_CROP;                  //    

    UINT32        CAM_LCSO_D_BASE_ADDR;                  //    
    UINT32        CAM_LCSO_D_OFST_ADDR;                  //    
    UINT32            CAM_LCSO_D_XSIZE;                  //    
    UINT32            CAM_LCSO_D_YSIZE;                  //    
    UINT32           CAM_LCSO_D_STRIDE;                  //    
    UINT32              CAM_LCSO_D_CON;                  //    
    UINT32             CAM_LCSO_D_CON2;                  //    

    UINT32         CAM_AFO_D_BASE_ADDR;                  //    
    UINT32             CAM_AFO_D_XSIZE;                  //    

    UINT32             CAM_AFO_D_YSIZE;                  //    
    UINT32            CAM_AFO_D_STRIDE;                  //    
    UINT32               CAM_AFO_D_CON;                  //    
    UINT32              CAM_AFO_D_CON2;                  //    

    UINT32         CAM_AAO_D_BASE_ADDR;                  //    
    UINT32         CAM_AAO_D_OFST_ADDR;                  //    
    UINT32             CAM_AAO_D_XSIZE;                  //    
    UINT32             CAM_AAO_D_YSIZE;                  //    
    UINT32            CAM_AAO_D_STRIDE;                  //    
    UINT32               CAM_AAO_D_CON;                  //    
    UINT32              CAM_AAO_D_CON2;                  //    

}_isp_backup_reg_t;



typedef struct _seninf_backup_reg
{
    UINT32             SENINF_TOP_CTRL;                //    
    UINT32       SENINF_TOP_CMODEL_PAR;                //    
    UINT32         SENINF_TOP_MUX_CTRL;                //    
    UINT32                rsv_800C[45];                //           
    UINT32                     N3D_CTL;                //    
    UINT32                     N3D_POS;                //    
    UINT32                    N3D_TRIG;                //    
    UINT32                     N3D_INT;                        //    
    UINT32                    N3D_CNT0;                       //    
    UINT32                    N3D_CNT1;                       //    
    UINT32                     N3D_DBG;                        //    
    UINT32                N3D_DIFF_THR;                   //    
    UINT32                N3D_DIFF_CNT;                   //    
    UINT32                          rsv_80E4[7];                    //           
    UINT32                SENINF1_CTRL;                   //    
    UINT32                          rsv_8104[7];                    //           
    UINT32            SENINF1_MUX_CTRL;               //    
    UINT32           SENINF1_MUX_INTEN;              //    
    UINT32          SENINF1_MUX_INTSTA;             //    
    UINT32            SENINF1_MUX_SIZE;               //    
    UINT32         SENINF1_MUX_DEBUG_1;            //    
    UINT32         SENINF1_MUX_DEBUG_2;            //    
    UINT32         SENINF1_MUX_DEBUG_3;            //    
    UINT32         SENINF1_MUX_DEBUG_4;            //    
    UINT32         SENINF1_MUX_DEBUG_5;            //    
    UINT32         SENINF1_MUX_DEBUG_6;            //    
    UINT32         SENINF1_MUX_DEBUG_7;            //    
    UINT32           SENINF1_MUX_SPARE;              //    
    UINT32            SENINF1_MUX_DATA;               //    
    UINT32        SENINF1_MUX_DATA_CNT;           //    
    UINT32            SENINF1_MUX_CROP;               //    
    UINT32                          rsv_815C[41];                   //           
    UINT32           SENINF_TG1_PH_CNT;              //    
    UINT32           SENINF_TG1_SEN_CK;              //    
    UINT32           SENINF_TG1_TM_CTL;              //    
    UINT32          SENINF_TG1_TM_SIZE;             //    
    UINT32           SENINF_TG1_TM_CLK;              //    
    UINT32                          rsv_8214[59];                   //           
    UINT32          MIPI_RX_CON00_CSI0;             //    
    UINT32          MIPI_RX_CON04_CSI0;             //    
    UINT32          MIPI_RX_CON08_CSI0;             //    
    UINT32          MIPI_RX_CON0C_CSI0;             //    
    UINT32          MIPI_RX_CON10_CSI0;             //    
    UINT32                          rsv_8314[4];                    //           
    UINT32          MIPI_RX_CON24_CSI0;             //    
    UINT32          MIPI_RX_CON28_CSI0;             //    
    UINT32                          rsv_832C[2];                    //           
    UINT32          MIPI_RX_CON34_CSI0;             //    
    UINT32          MIPI_RX_CON38_CSI0;             //    
    UINT32          MIPI_RX_CON3C_CSI0;             //    
    UINT32          MIPI_RX_CON40_CSI0;             //    
    UINT32          MIPI_RX_CON44_CSI0;             //    
    UINT32          MIPI_RX_CON48_CSI0;             //    
    UINT32                          rsv_834C;                       //    
    UINT32          MIPI_RX_CON50_CSI0;             //    
    UINT32                          rsv_8354[3];                    //           
    UINT32           SENINF1_CSI2_CTRL;              //    
    UINT32          SENINF1_CSI2_DELAY;             //    
    UINT32          SENINF1_CSI2_INTEN;             //    
    UINT32         SENINF1_CSI2_INTSTA;            //    
    UINT32         SENINF1_CSI2_ECCDBG;            //    
    UINT32         SENINF1_CSI2_CRCDBG;            //    
    UINT32            SENINF1_CSI2_DBG;               //    
    UINT32            SENINF1_CSI2_VER;               //    
    UINT32     SENINF1_CSI2_SHORT_INFO;        //    
    UINT32          SENINF1_CSI2_LNFSM;             //    
    UINT32          SENINF1_CSI2_LNMUX;             //    
    UINT32      SENINF1_CSI2_HSYNC_CNT;         //    
    UINT32            SENINF1_CSI2_CAL;               //    
    UINT32             SENINF1_CSI2_DS;                //    
    UINT32             SENINF1_CSI2_VS;                //    
    UINT32           SENINF1_CSI2_BIST;              //    
    UINT32           SENINF1_NCSI2_CTL;              //    
    UINT32   SENINF1_NCSI2_LNRC_TIMING;      //    
    UINT32   SENINF1_NCSI2_LNRD_TIMING;      //    
    UINT32          SENINF1_NCSI2_DPCM;             //    
    UINT32        SENINF1_NCSI2_INT_EN;           //    
    UINT32    SENINF1_NCSI2_INT_STATUS;       //    
    UINT32       SENINF1_NCSI2_DGB_SEL;          //    
    UINT32      SENINF1_NCSI2_DBG_PORT;         //    
    UINT32        SENINF1_NCSI2_SPARE0;           //    
    UINT32        SENINF1_NCSI2_SPARE1;           //    
    UINT32      SENINF1_NCSI2_LNRC_FSM;         //    
    UINT32      SENINF1_NCSI2_LNRD_FSM;         //    
    UINT32 SENINF1_NCSI2_FRAME_LINE_NUM;   //    
    UINT32 SENINF1_NCSI2_GENERIC_SHORT;    //    
    UINT32      SENINF1_NCSI2_HSRX_DBG;         //    
    UINT32            SENINF1_NCSI2_DI;               //    
    UINT32      SENINF1_NCSI2_HS_TRAIL;         //    
    UINT32       SENINF1_NCSI2_DI_CTRL;          //    
    UINT32                          rsv_83E8[70];                   //           
    UINT32                SENINF2_CTRL;                   //    
    UINT32                          rsv_8504[7];                    //           
    UINT32            SENINF2_MUX_CTRL;               //    
    UINT32           SENINF2_MUX_INTEN;              //    
    UINT32          SENINF2_MUX_INTSTA;             //    
    UINT32            SENINF2_MUX_SIZE;               //    
    UINT32         SENINF2_MUX_DEBUG_1;            //    
    UINT32         SENINF2_MUX_DEBUG_2;            //    
    UINT32         SENINF2_MUX_DEBUG_3;            //    
    UINT32         SENINF2_MUX_DEBUG_4;            //    
    UINT32         SENINF2_MUX_DEBUG_5;            //    
    UINT32         SENINF2_MUX_DEBUG_6;            //    
    UINT32         SENINF2_MUX_DEBUG_7;            //    
    UINT32           SENINF2_MUX_SPARE;              //    
    UINT32            SENINF2_MUX_DATA;               //    
    UINT32        SENINF2_MUX_DATA_CNT;           //    
    UINT32            SENINF2_MUX_CROP;               //    
    UINT32                          rsv_855C[41];                   //           
    UINT32           SENINF_TG2_PH_CNT;              //    
    UINT32           SENINF_TG2_SEN_CK;              //    
    UINT32           SENINF_TG2_TM_CTL;              //    
    UINT32          SENINF_TG2_TM_SIZE;             //    
    UINT32           SENINF_TG2_TM_CLK;              //    
    UINT32                          rsv_8614[59];                   //           
    UINT32          MIPI_RX_CON00_CSI1;             //    
    UINT32          MIPI_RX_CON04_CSI1;             //    
    UINT32          MIPI_RX_CON08_CSI1;             //    
    UINT32          MIPI_RX_CON0C_CSI1;             //    
    UINT32          MIPI_RX_CON10_CSI1;             //    
    UINT32                          rsv_8714[4];                    //           
    UINT32          MIPI_RX_CON24_CSI1;             //    
    UINT32          MIPI_RX_CON28_CSI1;             //    
    UINT32                          rsv_872C[2];                    //           
    UINT32          MIPI_RX_CON34_CSI1;             //    
    UINT32          MIPI_RX_CON38_CSI1;             //    
    UINT32          MIPI_RX_CON3C_CSI1;             //    
    UINT32          MIPI_RX_CON40_CSI1;             //    
    UINT32          MIPI_RX_CON44_CSI1;             //    
    UINT32          MIPI_RX_CON48_CSI1;             //    
    UINT32                          rsv_874C;                       //    
    UINT32          MIPI_RX_CON50_CSI1;             //    
    UINT32                          rsv_8754[3];                    //           
    UINT32           SENINF2_CSI2_CTRL;              //    
    UINT32          SENINF2_CSI2_DELAY;             //    
    UINT32          SENINF2_CSI2_INTEN;             //    
    UINT32         SENINF2_CSI2_INTSTA;            //    
    UINT32         SENINF2_CSI2_ECCDBG;            //    
    UINT32         SENINF2_CSI2_CRCDBG;            //    
    UINT32            SENINF2_CSI2_DBG;               //    
    UINT32            SENINF2_CSI2_VER;               //    
    UINT32     SENINF2_CSI2_SHORT_INFO;        //    
    UINT32          SENINF2_CSI2_LNFSM;             //    
    UINT32          SENINF2_CSI2_LNMUX;             //    
    UINT32      SENINF2_CSI2_HSYNC_CNT;         //    
    UINT32            SENINF2_CSI2_CAL;               //    
    UINT32             SENINF2_CSI2_DS;                //    
    UINT32             SENINF2_CSI2_VS;                //    
    UINT32           SENINF2_CSI2_BIST;              //    
    UINT32           SENINF2_NCSI2_CTL;              //    
    UINT32   SENINF2_NCSI2_LNRC_TIMING;      //    
    UINT32   SENINF2_NCSI2_LNRD_TIMING;      //    
    UINT32          SENINF2_NCSI2_DPCM;             //    
    UINT32        SENINF2_NCSI2_INT_EN;           //    
    UINT32    SENINF2_NCSI2_INT_STATUS;       //    
    UINT32       SENINF2_NCSI2_DGB_SEL;          //    
    UINT32      SENINF2_NCSI2_DBG_PORT;         //    
    UINT32        SENINF2_NCSI2_SPARE0;           //    
    UINT32        SENINF2_NCSI2_SPARE1;           //    
    UINT32      SENINF2_NCSI2_LNRC_FSM;         //    
    UINT32      SENINF2_NCSI2_LNRD_FSM;         //    
    UINT32 SENINF2_NCSI2_FRAME_LINE_NUM;   //    
    UINT32 SENINF2_NCSI2_GENERIC_SHORT;    //    
    UINT32      SENINF2_NCSI2_HSRX_DBG;         //    
    UINT32            SENINF2_NCSI2_DI;               //    
    UINT32      SENINF2_NCSI2_HS_TRAIL;         //    
    UINT32       SENINF2_NCSI2_DI_CTRL;          //    

    UINT32      MIPIRX_ANALOG_BASE_000;
    UINT32      MIPIRX_ANALOG_BASE_004;
    UINT32      MIPIRX_ANALOG_BASE_008;
    UINT32      MIPIRX_ANALOG_BASE_00C;
    UINT32      MIPIRX_ANALOG_BASE_010;
    UINT32      MIPIRX_ANALOG_BASE_014;
    UINT32      MIPIRX_ANALOG_BASE_018;
    UINT32      MIPIRX_ANALOG_BASE_01C;
    UINT32      MIPIRX_ANALOG_BASE_020;
    UINT32      MIPIRX_ANALOG_BASE_024;
    UINT32      MIPIRX_ANALOG_BASE_028;
    UINT32      MIPIRX_ANALOG_BASE_02C;
    UINT32      MIPIRX_ANALOG_BASE_030;
    UINT32      MIPIRX_ANALOG_BASE_034;
    UINT32      MIPIRX_ANALOG_BASE_038;
    UINT32      MIPIRX_ANALOG_BASE_03C;
    UINT32      MIPIRX_ANALOG_BASE_040;
    UINT32      MIPIRX_ANALOG_BASE_044;
    UINT32      MIPIRX_ANALOG_BASE_048;
    UINT32      MIPIRX_ANALOG_BASE_04C;
    UINT32      MIPIRX_ANALOG_BASE_050;
    UINT32      MIPIRX_ANALOG_BASE_054;
    UINT32      MIPIRX_ANALOG_BASE_058;
    UINT32      MIPIRX_ANALOG_BASE_05C;


}_seninf_backup_reg_t;

static volatile _isp_backup_reg_t g_backupReg;

static volatile _seninf_backup_reg_t g_SeninfBackupReg;


/*                                                                              
 
                                                                               */

#ifdef CONFIG_OF

typedef enum
{
    ISP_CAM0_IRQ_IDX = 0,
    ISP_CAM1_IRQ_IDX,
    ISP_CAM2_IRQ_IDX,
    ISP_CAMSV0_IRQ_IDX,
    ISP_CAMSV1_IRQ_IDX,
    ISP_CAM_IRQ_IDX_NUM
}ISP_CAM_IRQ_ENUM;

typedef enum
{
    ISP_BASE_ADDR = 0,
    ISP_INNER_BASE_ADDR,
    ISP_IMGSYS_CONFIG_BASE_ADDR,
    ISP_MIPI_ANA_BASE_ADDR,
    ISP_GPIO_BASE_ADDR,
    ISP_CAM_BASEADDR_NUM
}ISP_CAM_BASEADDR_ENUM;


static unsigned long gISPSYS_Irq[ISP_CAM_IRQ_IDX_NUM];
static unsigned long gISPSYS_Reg[ISP_CAM_BASEADDR_NUM];


static void __iomem *g_isp_base_dase;
static void __iomem *g_isp_inner_base_dase;
static void __iomem *g_imgsys_config_base_dase;


#define ISP_ADDR                        (gISPSYS_Reg[ISP_BASE_ADDR])
#define ISP_IMGSYS_BASE                 (gISPSYS_Reg[ISP_IMGSYS_CONFIG_BASE_ADDR])
#define ISP_ADDR_CAMINF                 (gISPSYS_Reg[ISP_IMGSYS_CONFIG_BASE_ADDR])

#define ISP_MIPI_ANA_ADDR               (gISPSYS_Reg[ISP_MIPI_ANA_BASE_ADDR])
#define ISP_GPIO_ADDR					(gISPSYS_Reg[ISP_GPIO_BASE_ADDR])

#define ISP_IMGSYS_BASE_PHY				0x15000000

#else
#define ISP_ADDR                        (IMGSYS_BASE + 0x4000)
#define ISP_IMGSYS_BASE                 IMGSYS_BASE
#define ISP_ADDR_CAMINF                 IMGSYS_BASE
#define ISP_MIPI_ANA_ADDR				0x10217000
#define ISP_GPIO_ADDR					GPIO_BASE

#endif


#define ISP_REG_ADDR_EN1                (ISP_ADDR + 0x4)
#define ISP_REG_ADDR_INT_P1_ST          (ISP_ADDR + 0x4C)
#define ISP_REG_ADDR_INT_P1_ST2         (ISP_ADDR + 0x54)
#define ISP_REG_ADDR_INT_P1_ST_D        (ISP_ADDR + 0x5C)
#define ISP_REG_ADDR_INT_P1_ST2_D       (ISP_ADDR + 0x64)
#define ISP_REG_ADDR_INT_P2_ST          (ISP_ADDR + 0x6C)
#define ISP_REG_ADDR_INT_STATUSX        (ISP_ADDR + 0x70)
#define ISP_REG_ADDR_INT_STATUS2X       (ISP_ADDR + 0x74)
#define ISP_REG_ADDR_INT_STATUS3X       (ISP_ADDR + 0x78)
#define ISP_REG_ADDR_CAM_SW_CTL         (ISP_ADDR + 0x8C)
#define ISP_REG_ADDR_IMGO_FBC           (ISP_ADDR + 0xF0)
#define ISP_REG_ADDR_RRZO_FBC           (ISP_ADDR + 0xF4)
#define ISP_REG_ADDR_IMGO_D_FBC         (ISP_ADDR + 0xF8)
#define ISP_REG_ADDR_RRZO_D_FBC         (ISP_ADDR + 0xFC)
#define ISP_REG_ADDR_TG_VF_CON          (ISP_ADDR + 0x414)
#define ISP_REG_ADDR_TG_INTER_ST        (ISP_ADDR + 0x44C)
#define ISP_REG_ADDR_TG2_VF_CON         (ISP_ADDR + 0x2414)
#define ISP_REG_ADDR_TG2_INTER_ST       (ISP_ADDR + 0x244C)
#define ISP_REG_ADDR_IMGO_BASE_ADDR     (ISP_ADDR + 0x3300)
#define ISP_REG_ADDR_RRZO_BASE_ADDR     (ISP_ADDR + 0x3320)
#define ISP_REG_ADDR_IMGO_D_BASE_ADDR   (ISP_ADDR + 0x34D4)
#define ISP_REG_ADDR_RRZO_D_BASE_ADDR   (ISP_ADDR + 0x34F4)
#define ISP_REG_ADDR_SENINF1_INT        (ISP_ADDR + 0x4128)
#define ISP_REG_ADDR_SENINF2_INT        (ISP_ADDR + 0x4528)
#define ISP_REG_ADDR_SENINF3_INT        (ISP_ADDR + 0x4928)
#define ISP_REG_ADDR_SENINF4_INT        (ISP_ADDR + 0x4D28)
#define ISP_REG_ADDR_CAMSV_FMT_SEL      (ISP_ADDR + 0x5004)
#define ISP_REG_ADDR_CAMSV_INT          (ISP_ADDR + 0x500C)
#define ISP_REG_ADDR_CAMSV_SW_CTL       (ISP_ADDR + 0x5010)
#define ISP_REG_ADDR_CAMSV_TG_INTER_ST  (ISP_ADDR + 0x544C)
#define ISP_REG_ADDR_CAMSV2_FMT_SEL     (ISP_ADDR + 0x5804)
#define ISP_REG_ADDR_CAMSV2_INT         (ISP_ADDR + 0x580C)
#define ISP_REG_ADDR_CAMSV2_SW_CTL      (ISP_ADDR + 0x5810)
#define ISP_REG_ADDR_CAMSV_TG2_INTER_ST (ISP_ADDR + 0x5C4C)
#define ISP_REG_ADDR_CAMSV_IMGO_FBC     (ISP_ADDR + 0x501C)
#define ISP_REG_ADDR_CAMSV2_IMGO_FBC    (ISP_ADDR + 0x581C)
#define ISP_REG_ADDR_IMGO_SV_BASE_ADDR  (ISP_ADDR + 0x5208)
#define ISP_REG_ADDR_IMGO_SV_XSIZE      (ISP_ADDR + 0x5210)
#define ISP_REG_ADDR_IMGO_SV_YSIZE      (ISP_ADDR + 0x5214)
#define ISP_REG_ADDR_IMGO_SV_STRIDE     (ISP_ADDR + 0x5218)
#define ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR    (ISP_ADDR + 0x5228)
#define ISP_REG_ADDR_IMGO_SV_D_XSIZE    (ISP_ADDR + 0x5230)
#define ISP_REG_ADDR_IMGO_SV_D_YSIZE    (ISP_ADDR + 0x5234)
#define ISP_REG_ADDR_IMGO_SV_D_STRIDE   (ISP_ADDR + 0x5238)
#define TG_REG_ADDR_GRAB_W              (ISP_ADDR + 0x418)
#define TG2_REG_ADDR_GRAB_W             (ISP_ADDR + 0x2418)
#define TG_REG_ADDR_GRAB_H              (ISP_ADDR + 0x41C)
#define TG2_REG_ADDR_GRAB_H             (ISP_ADDR + 0x241C)

#define ISP_REG_ADDR_CAMSV_TG_VF_CON    (ISP_ADDR + 0x5414)
#define ISP_REG_ADDR_CAMSV_TG2_VF_CON   (ISP_ADDR + 0x5C14)
//              
//                                                         
//                                                         
//                      
#define ISP_REG_ADDR_TG_MAGIC_0         (ISP_IMGSYS_BASE + 0x75DC) //    

#define ISP_REG_ADDR_TG2_MAGIC_0        (ISP_IMGSYS_BASE + 0x75E4) //    

//                       
#define ISP_REG_ADDR_TG_RRZ_CROP_IN     (ISP_IMGSYS_BASE + 0x75E0) 
#define ISP_REG_ADDR_TG_RRZ_CROP_IN_D   (ISP_IMGSYS_BASE + 0x75E8) 

//                                                                                    
#define ISP_REG_ADDR_RRZ_W         (ISP_ADDR_CAMINF + 0x4094)//   
#define ISP_REG_ADDR_RRZ_W_D       (ISP_ADDR_CAMINF + 0x409C)//   
/*
                                                                      
                                                                      
                                                                      
                                                                       
                                                                     
                                                                      
                                                                      
                                                                      
                                                                     
                                                                    
                                                                      
                                                                  
                                                                  
                                                                 
                                                                 
*/

//              
//                       
//                       
//                       
#define ISP_INNER_REG_ADDR_FMT_SEL_P1       (ISP_ADDR + 0x0028)
#define ISP_INNER_REG_ADDR_FMT_SEL_P1_D     (ISP_ADDR + 0x002C)
//                                                                      
//                                                                      
#define ISP_INNER_REG_ADDR_IMGO_XSIZE       (ISP_ADDR_CAMINF + 0xF308)
#define ISP_INNER_REG_ADDR_IMGO_YSIZE       (ISP_ADDR_CAMINF + 0xF30C)
#define ISP_INNER_REG_ADDR_IMGO_STRIDE      (ISP_ADDR_CAMINF + 0xF310)
#define ISP_INNER_REG_ADDR_IMGO_CROP        (ISP_ADDR_CAMINF + 0xF31C)
#define ISP_INNER_REG_ADDR_RRZO_XSIZE       (ISP_ADDR_CAMINF + 0xF328)
#define ISP_INNER_REG_ADDR_RRZO_YSIZE       (ISP_ADDR_CAMINF + 0xF32C)
#define ISP_INNER_REG_ADDR_RRZO_STRIDE      (ISP_ADDR_CAMINF + 0xF330)
#define ISP_INNER_REG_ADDR_RRZO_CROP        (ISP_ADDR_CAMINF + 0xF33C)
#define ISP_INNER_REG_ADDR_IMGO_D_XSIZE     (ISP_ADDR_CAMINF + 0xF4DC)
#define ISP_INNER_REG_ADDR_IMGO_D_YSIZE     (ISP_ADDR_CAMINF + 0xF4E0)
#define ISP_INNER_REG_ADDR_IMGO_D_STRIDE    (ISP_ADDR_CAMINF + 0xF4E4)
#define ISP_INNER_REG_ADDR_IMGO_D_CROP      (ISP_ADDR_CAMINF + 0xF4F0)
#define ISP_INNER_REG_ADDR_RRZO_D_XSIZE     (ISP_ADDR_CAMINF + 0xF4FC)
#define ISP_INNER_REG_ADDR_RRZO_D_YSIZE     (ISP_ADDR_CAMINF + 0xF500)
#define ISP_INNER_REG_ADDR_RRZO_D_STRIDE    (ISP_ADDR_CAMINF + 0xF504)
#define ISP_INNER_REG_ADDR_RRZO_D_CROP      (ISP_ADDR_CAMINF + 0xF510)

#define ISP_INNER_REG_ADDR_RRZ_HORI_INT_OFST (ISP_ADDR_CAMINF + 0xD7B4)
#define ISP_INNER_REG_ADDR_RRZ_VERT_INT_OFST (ISP_ADDR_CAMINF + 0xD7BC)
#define ISP_INNER_REG_ADDR_RRZ_IN_IMG        (ISP_ADDR_CAMINF + 0xD7A4)
#define ISP_INNER_REG_ADDR_RRZ_OUT_IMG       (ISP_ADDR_CAMINF + 0xD7A8)

#define ISP_INNER_REG_ADDR_RRZ_D_HORI_INT_OFST (ISP_ADDR_CAMINF + 0xE7B4)
#define ISP_INNER_REG_ADDR_RRZ_D_VERT_INT_OFST (ISP_ADDR_CAMINF + 0xE7BC)
#define ISP_INNER_REG_ADDR_RRZ_D_IN_IMG        (ISP_ADDR_CAMINF + 0xE7A4)
#define ISP_INNER_REG_ADDR_RRZ_D_OUT_IMG       (ISP_ADDR_CAMINF + 0xE7A8)


//                                 
//                                               
//                                             
//                                             
//                                                 
//                                                 
//                                                 
//                                             
//                                             

#define ISP_TPIPE_ADDR                  (0x15004000)

//              
#define ISP_REG_SW_CTL_SW_RST_P1_MASK   (0x00000007)
#define ISP_REG_SW_CTL_SW_RST_TRIG      (0x00000001)
#define ISP_REG_SW_CTL_SW_RST_STATUS    (0x00000002)
#define ISP_REG_SW_CTL_HW_RST           (0x00000004)
#define ISP_REG_SW_CTL_SW_RST_P2_MASK   (0x00000070)
#define ISP_REG_SW_CTL_SW_RST_P2_TRIG   (0x00000010)
#define ISP_REG_SW_CTL_SW_RST_P2_STATUS (0x00000020)
#define ISP_REG_SW_CTL_HW_RST_P2        (0x00000040)
#define ISP_REG_SW_CTL_RST_CAM_P1       (1)
#define ISP_REG_SW_CTL_RST_CAM_P2       (2)
#define ISP_REG_SW_CTL_RST_CAMSV        (3)
#define ISP_REG_SW_CTL_RST_CAMSV2       (4)

//                     
//         
#define ISP_REG_MASK_INT_P1_ST          (   ISP_IRQ_P1_STATUS_VS1_INT_ST |\
                                            ISP_IRQ_P1_STATUS_TG1_INT1_ST |\
                                            ISP_IRQ_P1_STATUS_TG1_INT2_ST |\
                                            ISP_IRQ_P1_STATUS_EXPDON1_ST |\
                                            ISP_IRQ_P1_STATUS_PASS1_DON_ST |\
                                            ISP_IRQ_P1_STATUS_SOF1_INT_ST |\
                                            ISP_IRQ_P1_STATUS_AF_DON_ST |\
                                            ISP_IRQ_P1_STATUS_FLK_DON_ST |\
                                            ISP_IRQ_P1_STATUS_FBC_RRZO_DON_ST |\
                                            ISP_IRQ_P1_STATUS_FBC_IMGO_DON_ST )
//              
#define ISP_REG_MASK_INT_P1_ST_ERR     (    ISP_IRQ_P1_STATUS_TG1_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_TG1_GBERR |\
                                            ISP_IRQ_P1_STATUS_CQ0_ERR |\
                                            ISP_IRQ_P1_STATUS_CQ0_VS_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_IMGO_DROP_FRAME_ST |\
                                            ISP_IRQ_P1_STATUS_RRZO_DROP_FRAME_ST |\
                                            ISP_IRQ_P1_STATUS_IMGO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_AAO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_LCSO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_RRZO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_ESFKO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_FLK_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_LSC_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_DMA_ERR_ST )
//                               
#define ISP_REG_MASK_INT_P1_ST_WAITQ    (    ISP_IRQ_P1_STATUS_VS1_INT_ST |\
                                            ISP_IRQ_P1_STATUS_PASS1_DON_ST |\
                                            ISP_IRQ_P1_STATUS_SOF1_INT_ST|\
                                            ISP_IRQ_P1_STATUS_AF_DON_ST)

//                      
//         
#define ISP_REG_MASK_INT_P1_ST2         (   ISP_IRQ_P1_STATUS2_IMGO_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_UFEO_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_RRZO_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_ESFKO_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_LCSO_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_AAO_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_BPCI_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_LSCI_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_AF_TAR_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_AF_FLO1_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_AF_FLO2_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_AF_FLO3_DONE_ST)
//              
#define ISP_REG_MASK_INT_P1_ST2_ERR     (   0x0 )
//                               
#define ISP_REG_MASK_INT_P1_ST2_WAITQ   (   0x0 )


//                       
//         
#define ISP_REG_MASK_INT_P1_ST_D        (   ISP_IRQ_P1_STATUS_D_VS1_INT_ST |\
                                            ISP_IRQ_P1_STATUS_D_TG1_INT1_ST |\
                                            ISP_IRQ_P1_STATUS_D_TG1_INT2_ST |\
                                            ISP_IRQ_P1_STATUS_D_EXPDON1_ST |\
                                            ISP_IRQ_P1_STATUS_D_PASS1_DON_ST |\
                                            ISP_IRQ_P1_STATUS_D_SOF1_INT_ST |\
                                            ISP_IRQ_P1_STATUS_D_AF_DON_ST |\
                                            ISP_IRQ_P1_STATUS_D_FBC_RRZO_DON_ST |\
                                            ISP_IRQ_P1_STATUS_D_FBC_IMGO_DON_ST )
//              
#define ISP_REG_MASK_INT_P1_ST_D_ERR     (    ISP_IRQ_P1_STATUS_D_TG1_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_D_TG1_GBERR |\
                                            ISP_IRQ_P1_STATUS_D_CQ0_ERR |\
                                            ISP_IRQ_P1_STATUS_D_CQ0_VS_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_D_IMGO_DROP_FRAME_ST |\
                                            ISP_IRQ_P1_STATUS_D_RRZO_DROP_FRAME_ST |\
                                            ISP_IRQ_P1_STATUS_D_IMGO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_D_AAO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_D_LCSO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_D_RRZO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_D_AFO_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_D_LSC_ERR_ST |\
                                            ISP_IRQ_P1_STATUS_D_DMA_ERR_ST )
//                               
#define ISP_REG_MASK_INT_P1_ST_D_WAITQ    (  ISP_IRQ_P1_STATUS_D_VS1_INT_ST |\
                                            ISP_IRQ_P1_STATUS_D_PASS1_DON_ST |\
                                            ISP_IRQ_P1_STATUS_D_SOF1_INT_ST|\
                                            ISP_IRQ_P1_STATUS_D_AF_DON_ST)

//                        
//         
#define ISP_REG_MASK_INT_P1_ST2_D       (   ISP_IRQ_P1_STATUS2_D_IMGO_D_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_RRZO_D_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_AFO_D_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_LCSO_D_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_AAO_D_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_BPCI_D_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_LSCI_D_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_AF_TAR_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_AF_FLO1_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_AF_FLO2_DONE_ST |\
                                            ISP_IRQ_P1_STATUS2_D_AF_FLO3_DONE_ST)
//              
#define ISP_REG_MASK_INT_P1_ST2_D_ERR   (   0x0 )
//                               
#define ISP_REG_MASK_INT_P1_ST2_D_WAITQ  (   0x0 )

//                     
//         
#define ISP_REG_MASK_INT_P2_ST          (   ISP_IRQ_P2_STATUS_PASS2_DON_ST |\
                                            ISP_IRQ_P2_STATUS_TILE_DON_ST |\
                                            ISP_IRQ_P2_STATUS_CQ_DON_ST |\
                                            ISP_IRQ_P2_STATUS_PASS2A_DON_ST |\
                                            ISP_IRQ_P2_STATUS_PASS2B_DON_ST |\
                                            ISP_IRQ_P2_STATUS_PASS2C_DON_ST |\
                                            ISP_IRQ_P2_STATUS_CQ1_DONE_ST   |\
                                            ISP_IRQ_P2_STATUS_CQ2_DONE_ST   |\
                                            ISP_IRQ_P2_STATUS_CQ3_DONE_ST   |\
                                            ISP_IRQ_P2_STATUS_IMGI_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_UFDI_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_VIPI_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_VIP2I_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_VIP3I_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_LCEI_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_MFBO_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_IMG2O_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_IMG3O_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_IMG3BO_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_IMG3CO_DONE_ST |\
                                            ISP_IRQ_P2_STATUS_FEO_DONE_ST)
//              
#define ISP_REG_MASK_INT_P2_ST_ERR      (   ISP_IRQ_P2_STATUS_CQ_ERR_ST |\
                                            ISP_IRQ_P2_STATUS_TDR_ERR_ST |\
                                            ISP_IRQ_P2_STATUS_PASS2A_ERR_TRIG_ST |\
                                            ISP_IRQ_P2_STATUS_PASS2B_ERR_TRIG_ST |\
                                            ISP_IRQ_P2_STATUS_PASS2C_ERR_TRIG_ST )
//                               
#define ISP_REG_MASK_INT_P2_ST_WAITQ    (   ISP_IRQ_P2_STATUS_PASS2_DON_ST |\
                                            ISP_IRQ_P2_STATUS_PASS2A_DON_ST |\
                                            ISP_IRQ_P2_STATUS_PASS2B_DON_ST |\
                                            ISP_IRQ_P2_STATUS_PASS2C_DON_ST)
//                   
//         
#define ISP_REG_MASK_INT_STATUSX        (   ISP_IRQ_STATUSX_VS1_INT_ST |\
                                            ISP_IRQ_STATUSX_TG1_INT1_ST |\
                                            ISP_IRQ_STATUSX_TG1_INT2_ST |\
                                            ISP_IRQ_STATUSX_EXPDON1_ST |\
                                            ISP_IRQ_STATUSX_PASS1_DON_ST |\
                                            ISP_IRQ_STATUSX_SOF1_INT_ST |\
                                            ISP_IRQ_STATUSX_PASS2_DON_ST |\
                                            ISP_IRQ_STATUSX_TILE_DON_ST |\
                                            ISP_IRQ_STATUSX_AF_DON_ST |\
                                            ISP_IRQ_STATUSX_FLK_DON_ST |\
                                            ISP_IRQ_STATUSX_CQ_DON_ST |\
                                            ISP_IRQ_STATUSX_FBC_RRZO_DON_ST |\
                                            ISP_IRQ_STATUSX_FBC_IMGO_DON_ST)
//              
#define ISP_REG_MASK_INT_STATUSX_ERR    (   ISP_IRQ_STATUSX_TG1_ERR_ST |\
                                            ISP_IRQ_STATUSX_TG1_GBERR |\
                                            ISP_IRQ_STATUSX_CQ0_ERR |\
                                            ISP_IRQ_STATUSX_CQ0_VS_ERR_ST |\
                                            ISP_IRQ_STATUSX_IMGO_DROP_FRAME_ST |\
                                            ISP_IRQ_STATUSX_RRZO_DROP_FRAME_ST |\
                                            ISP_IRQ_STATUSX_CQ_ERR_ST |\
                                            ISP_IRQ_STATUSX_IMGO_ERR_ST |\
                                            ISP_IRQ_STATUSX_AAO_ERR_ST |\
                                            ISP_IRQ_STATUSX_LCSO_ERR_ST |\
                                            ISP_IRQ_STATUSX_RRZO_ERR_ST |\
                                            ISP_IRQ_STATUSX_ESFKO_ERR_ST |\
                                            ISP_IRQ_STATUSX_FLK_ERR_ST |\
                                            ISP_IRQ_STATUSX_LSC_ERR_ST |\
                                            ISP_IRQ_STATUSX_DMA_ERR_ST)
//                               
#define ISP_REG_MASK_INT_STATUSX_WAITQ  (   0x0 )

//                    
//         
#define ISP_REG_MASK_INT_STATUS2X       (   ISP_IRQ_STATUS2X_VS1_INT_ST |\
                                            ISP_IRQ_STATUS2X_TG1_INT1_ST |\
                                            ISP_IRQ_STATUS2X_TG1_INT2_ST |\
                                            ISP_IRQ_STATUS2X_EXPDON1_ST |\
                                            ISP_IRQ_STATUS2X_PASS1_DON_ST |\
                                            ISP_IRQ_STATUS2X_SOF1_INT_ST |\
                                            ISP_IRQ_STATUS2X_AF_DON_ST |\
                                            ISP_IRQ_STATUS2X_FBC_RRZO_DON_ST |\
                                            ISP_IRQ_STATUS2X_FBC_IMGO_DON_ST)
//              
#define ISP_REG_MASK_INT_STATUS2X_ERR   (   ISP_IRQ_STATUS2X_TG1_ERR_ST |\
                                            ISP_IRQ_STATUS2X_TG1_GBERR |\
                                            ISP_IRQ_STATUS2X_CQ0_ERR |\
                                            ISP_IRQ_STATUS2X_CQ0_VS_ERR_ST |\
                                            ISP_IRQ_STATUS2X_IMGO_DROP_FRAME_ST |\
                                            ISP_IRQ_STATUS2X_RRZO_DROP_FRAME_ST |\
                                            ISP_IRQ_STATUS2X_IMGO_ERR_ST |\
                                            ISP_IRQ_STATUS2X_AAO_ERR_ST |\
                                            ISP_IRQ_STATUS2X_LCSO_ERR_ST |\
                                            ISP_IRQ_STATUS2X_RRZO_ERR_ST |\
                                            ISP_IRQ_STATUS2X_AFO_ERR_ST |\
                                            ISP_IRQ_STATUS2X_LSC_ERR_ST |\
                                            ISP_IRQ_STATUS2X_DMA_ERR_ST)
//                               
#define ISP_REG_MASK_INT_STATUS2X_WAITQ  (   0x0 )

//                    
//         
#define ISP_REG_MASK_INT_STATUS3X       (   ISP_IRQ_STATUS3X_IMGO_DONE_ST |\
                                            ISP_IRQ_STATUS3X_UFEO_DONE_ST |\
                                            ISP_IRQ_STATUS3X_RRZO_DONE_ST |\
                                            ISP_IRQ_STATUS3X_ESFKO_DONE_ST |\
                                            ISP_IRQ_STATUS3X_LCSO_DONE_ST |\
                                            ISP_IRQ_STATUS3X_AAO_DONE_ST |\
                                            ISP_IRQ_STATUS3X_BPCI_DONE_ST |\
                                            ISP_IRQ_STATUS3X_LSCI_DONE_ST |\
                                            ISP_IRQ_STATUS3X_IMGO_D_DONE_ST |\
                                            ISP_IRQ_STATUS3X_RRZO_D_DONE_ST |\
                                            ISP_IRQ_STATUS3X_AFO_D_DONE_ST |\
                                            ISP_IRQ_STATUS3X_LCSO_D_DONE_ST |\
                                            ISP_IRQ_STATUS3X_AAO_D_DONE_ST |\
                                            ISP_IRQ_STATUS3X_BPCI_D_DONE_ST |\
                                            ISP_IRQ_STATUS3X_LCSI_D_DONE_ST |\
                                            ISP_IRQ_STATUS3X_IMGI_DONE_ST |\
                                            ISP_IRQ_STATUS3X_UFDI_DONE_ST |\
                                            ISP_IRQ_STATUS3X_VIPI_DONE_ST |\
                                            ISP_IRQ_STATUS3X_VIP2I_DONE_ST |\
                                            ISP_IRQ_STATUS3X_VIP3I_DONE_ST |\
                                            ISP_IRQ_STATUS3X_LCEI_DONE_ST |\
                                            ISP_IRQ_STATUS3X_MFBO_DONE_ST |\
                                            ISP_IRQ_STATUS3X_IMG2O_DONE_ST |\
                                            ISP_IRQ_STATUS3X_IMG3O_DONE_ST |\
                                            ISP_IRQ_STATUS3X_IMG3BO_DONE_ST |\
                                            ISP_IRQ_STATUS3X_IMG3CO_DONE_ST |\
                                            ISP_IRQ_STATUS3X_FEO_DONE_ST)
//              
#define ISP_REG_MASK_INT_STATUS3X_ERR   (   0X0)
//                               
#define ISP_REG_MASK_INT_STATUS3X_WAITQ  (   0x0 )


//                  
#define ISP_REG_MASK_INT_SENINF1       (   0X0)
#define ISP_REG_MASK_INT_SENINF1_ERR   (   SENINF1_IRQ_OVERRUN_IRQ_STA |\
                                           SENINF1_IRQ_CRCERR_IRQ_STA |\
                                           SENINF1_IRQ_FSMERR_IRQ_STA |\
                                           SENINF1_IRQ_VSIZEERR_IRQ_STA |\
                                           SENINF1_IRQ_HSIZEERR_IRQ_STA |\
                                           SENINF1_IRQ_SENSOR_VSIZEERR_IRQ_STA |\
                                           SENINF1_IRQ_SENSOR_HSIZEERR_IRQ_STA)
//                               
#define ISP_REG_MASK_INT_SENINF1_WAITQ  (   0x0 )

//                  
#define ISP_REG_MASK_INT_SENINF2       (   0X0)
#define ISP_REG_MASK_INT_SENINF2_ERR   (   SENINF2_IRQ_OVERRUN_IRQ_STA |\
                                           SENINF1_IRQ_CRCERR_IRQ_STA |\
                                           SENINF2_IRQ_FSMERR_IRQ_STA |\
                                           SENINF2_IRQ_VSIZEERR_IRQ_STA |\
                                           SENINF2_IRQ_HSIZEERR_IRQ_STA |\
                                           SENINF2_IRQ_SENSOR_VSIZEERR_IRQ_STA |\
                                           SENINF2_IRQ_SENSOR_HSIZEERR_IRQ_STA)
//                               
#define ISP_REG_MASK_INT_SENINF2_WAITQ  (   0x0 )

//                  
#define ISP_REG_MASK_INT_SENINF3       (   0X0)
#define ISP_REG_MASK_INT_SENINF3_ERR   (   SENINF3_IRQ_OVERRUN_IRQ_STA |\
                                           SENINF3_IRQ_CRCERR_IRQ_STA |\
                                           SENINF3_IRQ_FSMERR_IRQ_STA |\
                                           SENINF3_IRQ_VSIZEERR_IRQ_STA |\
                                           SENINF3_IRQ_HSIZEERR_IRQ_STA |\
                                           SENINF3_IRQ_SENSOR_VSIZEERR_IRQ_STA |\
                                           SENINF3_IRQ_SENSOR_HSIZEERR_IRQ_STA)
//                               
#define ISP_REG_MASK_INT_SENINF3_WAITQ  (   0x0 )

//                  
#define ISP_REG_MASK_INT_SENINF4       (   0X0)
#define ISP_REG_MASK_INT_SENINF4_ERR   (   SENINF4_IRQ_OVERRUN_IRQ_STA |\
                                           SENINF4_IRQ_CRCERR_IRQ_STA |\
                                           SENINF4_IRQ_FSMERR_IRQ_STA |\
                                           SENINF4_IRQ_VSIZEERR_IRQ_STA |\
                                           SENINF4_IRQ_HSIZEERR_IRQ_STA |\
                                           SENINF4_IRQ_SENSOR_VSIZEERR_IRQ_STA |\
                                           SENINF4_IRQ_SENSOR_HSIZEERR_IRQ_STA)
//                               
#define ISP_REG_MASK_INT_SENINF4_WAITQ  (   0x0 )

#define ISP_REG_MASK_CAMSV_ST          (    ISP_IRQ_CAMSV_STATUS_VS1_ST |\
                                            ISP_IRQ_CAMSV_STATUS_TG_ST1 |\
                                            ISP_IRQ_CAMSV_STATUS_TG_ST2 |\
                                            ISP_IRQ_CAMSV_STATUS_EXPDON1_ST |\
                                            ISP_IRQ_CAMSV_STATUS_TG_SOF1_ST |\
                                            ISP_IRQ_CAMSV_STATUS_PASS1_DON_ST)
//              
#define ISP_REG_MASK_CAMSV_ST_ERR     (     ISP_IRQ_CAMSV_STATUS_TG_ERR_ST |\
                                            ISP_IRQ_CAMSV_STATUS_TG_GBERR_ST |\
                                            ISP_IRQ_CAMSV_STATUS_TG_DROP_ST |\
                                            ISP_IRQ_CAMSV_STATUS_IMGO_ERR_ST |\
                                            ISP_IRQ_CAMSV_STATUS_IMGO_OVERR_ST |\
                                            ISP_IRQ_CAMSV_STATUS_IMGO_DROP_ST)
//                               
#define ISP_REG_MASK_CAMSV_ST_WAITQ    (    ISP_IRQ_CAMSV_STATUS_VS1_ST |\
                                            ISP_IRQ_CAMSV_STATUS_TG_SOF1_ST |\
                                            ISP_IRQ_CAMSV_STATUS_PASS1_DON_ST)

#define ISP_REG_MASK_CAMSV2_ST          (   ISP_IRQ_CAMSV2_STATUS_VS1_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_TG_ST1 |\
                                            ISP_IRQ_CAMSV2_STATUS_TG_ST2 |\
                                            ISP_IRQ_CAMSV2_STATUS_EXPDON1_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_TG_SOF1_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_PASS1_DON_ST)
//              
#define ISP_REG_MASK_CAMSV2_ST_ERR     (    ISP_IRQ_CAMSV2_STATUS_TG_ERR_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_TG_GBERR_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_TG_DROP_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_IMGO_ERR_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_IMGO_OVERR_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_IMGO_DROP_ST)
//                               
#define ISP_REG_MASK_CAMSV2_ST_WAITQ    (   ISP_IRQ_CAMSV2_STATUS_VS1_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_TG_SOF1_ST |\
                                            ISP_IRQ_CAMSV2_STATUS_PASS1_DON_ST)

static volatile MINT32 EDBufQueRemainNodeCnt = 0;               //                                                                             

static volatile wait_queue_head_t WaitQueueHead_EDBuf_WaitDeque;
static volatile wait_queue_head_t WaitQueueHead_EDBuf_WaitFrame;
static spinlock_t      SpinLockEDBufQueList;
#define _MAX_SUPPORT_P2_FRAME_NUM_ 512
#define _MAX_SUPPORT_P2_BURSTQ_NUM_ 4
static volatile MINT32 P2_Support_BurstQNum = 1;
#define _MAX_SUPPORT_P2_PACKAGE_NUM_ (_MAX_SUPPORT_P2_FRAME_NUM_/_MAX_SUPPORT_P2_BURSTQ_NUM_)
#define P2_EDBUF_MLIST_TAG 1
#define P2_EDBUF_RLIST_TAG 2
typedef struct
{
    volatile MUINT32                processID;  //                 
    volatile MUINT32                callerID;   //                
    volatile MINT32                 p2dupCQIdx; //                                                            
    volatile ISP_ED_BUF_STATE_ENUM  bufSts;     //             
}ISP_EDBUF_STRUCT;
static volatile MINT32 P2_EDBUF_RList_FirstBufIdx=0;
static volatile MINT32 P2_EDBUF_RList_CurBufIdx=0;
static volatile MINT32 P2_EDBUF_RList_LastBufIdx=0;
static volatile ISP_EDBUF_STRUCT P2_EDBUF_RingList[_MAX_SUPPORT_P2_FRAME_NUM_];

typedef struct
{
    volatile MUINT32                processID;  //                 
    volatile MUINT32                callerID;   //                
    volatile MINT32                 p2dupCQIdx; //                                                            
    volatile MINT32                 dequedNum;  //                                                       
}ISP_EDBUF_MGR_STRUCT;
static volatile MINT32 P2_EDBUF_MList_FirstBufIdx=0;
//                                                  
static volatile MINT32 P2_EDBUF_MList_LastBufIdx=0;
static volatile ISP_EDBUF_MGR_STRUCT P2_EDBUF_MgrList[_MAX_SUPPORT_P2_PACKAGE_NUM_];

static volatile MUINT32         g_regScen=0xa5a5a5a5;
static volatile spinlock_t      SpinLockRegScen;

//                                                            
//                                                             
#define IRQ_USER_NUM_MAX 32
static volatile spinlock_t      SpinLock_UserKey;

m4u_callback_ret_t ISP_M4U_TranslationFault_callback(int port, unsigned int mva, void* data);

/*                                                                              
 
                                                                               */
//              
//                                                             
static int* pTbl_RTBuf = NULL;
//                                                           
static void* pBuf_kmalloc = NULL;
//
static volatile ISP_RT_BUF_STRUCT* pstRTBuf = NULL;

//                                                                                            

unsigned long g_Flash_SpinLock;


static volatile unsigned int G_u4EnableClockCount = 0;


/*                                                                              
 
                                                                               */
typedef struct
{
    pid_t   Pid;
    pid_t   Tid;
}ISP_USER_INFO_STRUCT;

/*                                                                              
 
                                                                               */
#define ISP_BUF_SIZE            (4096)
#define ISP_BUF_SIZE_WRITE      1024
#define ISP_BUF_WRITE_AMOUNT    6

typedef enum
{
    ISP_BUF_STATUS_EMPTY,
    ISP_BUF_STATUS_HOLD,
    ISP_BUF_STATUS_READY
}ISP_BUF_STATUS_ENUM;

typedef struct
{
    volatile ISP_BUF_STATUS_ENUM    Status;
    volatile MUINT32                Size;
    MUINT8*                         pData;
}ISP_BUF_STRUCT;

typedef struct
{
    ISP_BUF_STRUCT      Read;
    ISP_BUF_STRUCT      Write[ISP_BUF_WRITE_AMOUNT];
}ISP_BUF_INFO_STRUCT;


/*                                                                              
 
                                                                               */
typedef struct
{
    atomic_t            HoldEnable;
    atomic_t            WriteEnable;
    ISP_HOLD_TIME_ENUM  Time;
}ISP_HOLD_INFO_STRUCT;

static volatile MINT32 IrqLockedUserKey[IRQ_USER_NUM_MAX] = {0};        /*                                                   */

typedef struct
{
    volatile MUINT32    Status[IRQ_USER_NUM_MAX][ISP_IRQ_TYPE_AMOUNT];
    MUINT32             Mask[ISP_IRQ_TYPE_AMOUNT];
    MUINT32             ErrMask[ISP_IRQ_TYPE_AMOUNT];
    volatile MUINT32   MarkedFlag[IRQ_USER_NUM_MAX][ISP_IRQ_TYPE_AMOUNT];            /*                                                              */
    volatile MUINT32   MarkedTime_sec[IRQ_USER_NUM_MAX][ISP_IRQ_TYPE_AMOUNT][32];   /*                                       */
    volatile MUINT32   MarkedTime_usec[IRQ_USER_NUM_MAX][ISP_IRQ_TYPE_AMOUNT][32];   /*                                       */
    volatile MINT32     PassedBySigCnt[IRQ_USER_NUM_MAX][ISP_IRQ_TYPE_AMOUNT][32];   /*                                            */
    volatile MUINT32   LastestSigTime_sec[ISP_IRQ_TYPE_AMOUNT][32];                                    /*                                         */
    volatile MUINT32   LastestSigTime_usec[ISP_IRQ_TYPE_AMOUNT][32];                                    /*                                         */
}ISP_IRQ_INFO_STRUCT;

typedef struct
{
    MUINT32     Vd;
    MUINT32     Expdone;
    MUINT32     WorkQueueVd;
    MUINT32     WorkQueueExpdone;
    MUINT32     TaskletVd;
    MUINT32     TaskletExpdone;
}ISP_TIME_LOG_STRUCT;

typedef enum _eChannel
{
    _PASS1      = 0,
    _PASS1_D    = 1,
    _CAMSV      = 2,
    _CAMSV_D    = 3,
    _PASS2      = 4,
    _ChannelMax = 5,
}eChannel;

/*                                                                    */
#define my_get_pow_idx(value)      \
({                                                          \
    int i=0,cnt=0;                                  \
    for(i=0;i<32;i++)                            \
    {                                                       \
        if((value>>i) & (0x00000001))    \
        {break;}                                            \
        else                                            \
        {cnt++;}                                      \
    }                                                    \
    cnt;                                                \
})


#define DMA_TRANS(dma,Out) \
do{ \
    if (dma == _imgo_|| dma == _rrzo_ ) {\
        Out = _PASS1;\
    }\
    else if(dma == _imgo_d_ || dma == _rrzo_d_) { \
        Out = _PASS1_D;\
    }\
    else if(dma == _camsv_imgo_) {\
        Out = _CAMSV;\
    }\
    else if(dma == _camsv2_imgo_) {\
        Out = _CAMSV_D;\
    }\
    else {}\
}while(0)

//                                                          
//                                        
typedef enum _eISPIrq
{
    _IRQ            = 0,
    _IRQ_D          = 1,
    _CAMSV_IRQ      = 2,
    _CAMSV_D_IRQ    = 3,
    _IRQ_MAX        = 4,
}eISPIrq;

typedef enum _eLOG_TYPE{
    _LOG_DBG = 0,   //                                                                 
    _LOG_INF = 1,
    _LOG_ERR = 2,
    _LOG_MAX = 3,
}eLOG_TYPE;

typedef enum _eLOG_OP {
    _LOG_INIT = 0,
    _LOG_RST = 1,
    _LOG_ADD = 2,
    _LOG_PRT = 3,
    _LOG_GETCNT = 4,
    _LOG_OP_MAX = 5
}eLOG_OP;

#define NORMAL_STR_LEN (512)
#define ERR_PAGE 2
#define DBG_PAGE 2
#define INF_PAGE 4
//                                     

#define LOG_PPNUM 2
volatile static MUINT32 m_CurrentPPB = 0;
typedef struct _SV_LOG_STR {
    MUINT32 _cnt[LOG_PPNUM][_LOG_MAX];
    //                                      
    char*  _str[LOG_PPNUM][_LOG_MAX];
}SV_LOG_STR,*PSV_LOG_STR;

static void* pLog_kmalloc = NULL;
static SV_LOG_STR gSvLog[_IRQ_MAX];
//                                   
//                                        
//                                          
static volatile MBOOL g_bDmaERR_p1 = MFALSE;
static volatile MBOOL g_bDmaERR_p1_d = MFALSE;
static volatile MBOOL g_bDmaERR_p2 = MFALSE;
static volatile MBOOL g_bDmaERR_deepDump = MFALSE;
static volatile UINT32 g_ISPIntErr[_IRQ_MAX] = {0};
#define nDMA_ERR_P1     (11)
#define nDMA_ERR_P1_D   (7)
#define nDMA_ERR    (nDMA_ERR_P1 + nDMA_ERR_P1_D)
static MUINT32 g_DmaErr_p1[nDMA_ERR] = {0};

/* 
                                                               
            
                                            
                                                                  
*/
#define IRQ_LOG_KEEPER_T(sec,usec) {\
        ktime_t time;           \
        time = ktime_get();     \
        sec = time.tv64;        \
        do_div( sec, 1000 );    \
        usec = do_div( sec, 1000000);\
}
#if 1
#define IRQ_LOG_KEEPER(irq,ppb,logT,fmt,...) do{\
    char* ptr; \
    char* pDes;\
    MUINT32* ptr2 = &gSvLog[irq]._cnt[ppb][logT];\
    unsigned int str_leng;\
    if(_LOG_ERR == logT) {\
        str_leng = NORMAL_STR_LEN*ERR_PAGE; \
    }else if(_LOG_DBG == logT){\
        str_leng = NORMAL_STR_LEN*DBG_PAGE; \
    }else if(_LOG_INF == logT) {\
        str_leng = NORMAL_STR_LEN*INF_PAGE;\
    }else {\
        str_leng = 0;\
    }\
    ptr = pDes = (char*)&(gSvLog[irq]._str[ppb][logT][gSvLog[irq]._cnt[ppb][logT]]);    \
    sprintf((char*)(pDes),fmt,##__VA_ARGS__);   \
    if('\0'!= gSvLog[irq]._str[ppb][logT][str_leng -1 ]) {\
        LOG_ERR("log str over flow(%d)",irq);\
    }\
    while(*ptr++ != '\0') {        \
        (*ptr2)++;\
    }     \
}while(0);
#else
#define IRQ_LOG_KEEPER(irq,ppb,logT,fmt,...)  xlog_printk(ANDROID_LOG_DEBUG  , "KEEPER", "[%s] " fmt, __FUNCTION__, ##__VA_ARGS__)
#endif

#if 1
#define IRQ_LOG_PRINTER(irq,ppb_in,logT_in) do{\
    SV_LOG_STR* pSrc = &gSvLog[irq];\
    char* ptr;\
    MUINT32 i;\
    MINT32 ppb=0;\
    MINT32 logT=0;\
    if(ppb_in>1){\
        ppb = 1;\
    }else{\
        ppb = ppb_in;\
    }\
    if(logT_in > _LOG_ERR){\
        logT = _LOG_ERR;\
    }else{\
        logT = logT_in;\
    }\
    ptr = pSrc->_str[ppb][logT];\
    if(0!= pSrc->_cnt[ppb][logT]){\
        if(_LOG_DBG == logT){\
            for(i=0;i<DBG_PAGE;i++){\
                if(ptr[NORMAL_STR_LEN*(i+1) - 1] != '\0') {\
                    ptr[NORMAL_STR_LEN*(i+1) - 1] = '\0';\
                    LOG_DBG("%s",&ptr[NORMAL_STR_LEN*i]);\
                }else{\
                    LOG_DBG("%s",&ptr[NORMAL_STR_LEN*i]);\
                    break;\
                }\
            }\
        }\
        else if(_LOG_INF == logT) {\
            for(i=0;i<INF_PAGE;i++){\
                if(ptr[NORMAL_STR_LEN*(i+1) - 1] != '\0') {\
                    ptr[NORMAL_STR_LEN*(i+1) - 1] = '\0';\
                    LOG_INF("%s",&ptr[NORMAL_STR_LEN*i]);\
                }else{\
                    LOG_INF("%s",&ptr[NORMAL_STR_LEN*i]);\
                    break;\
                }\
            }\
        }\
        else if(_LOG_ERR == logT) {\
            for(i=0;i<ERR_PAGE;i++){\
                if(ptr[NORMAL_STR_LEN*(i+1) - 1] != '\0') {\
                    ptr[NORMAL_STR_LEN*(i+1) - 1] = '\0';\
                    LOG_ERR("%s",&ptr[NORMAL_STR_LEN*i]);\
                }else{\
                    LOG_ERR("%s",&ptr[NORMAL_STR_LEN*i]);\
                    break;\
                }\
            }\
        }\
        else {\
            LOG_ERR("N.S.%d",logT);\
        }\
        ptr[0] = '\0';\
        pSrc->_cnt[ppb][logT] = 0;\
    }\
}while(0);


#else
#define IRQ_LOG_PRINTER(irq,ppb,logT)
#endif

#define SUPPORT_MAX_IRQ 32
typedef struct
{
    spinlock_t                      SpinLockIspRef;
    spinlock_t                      SpinLockIsp;
    spinlock_t                      SpinLockIrq[_IRQ_MAX];//                                                                        
    spinlock_t                      SpinLockHold;
    spinlock_t                      SpinLockRTBC;
    spinlock_t                      SpinLockClock;
    wait_queue_head_t               WaitQueueHead;
    //                                              
    volatile wait_queue_head_t      WaitQHeadList[SUPPORT_MAX_IRQ];
    struct work_struct              ScheduleWorkVD;
    struct work_struct              ScheduleWorkEXPDONE;
    MUINT32                         UserCount;
    MUINT32                         DebugMask;
    MINT32                          IrqNum;
    ISP_IRQ_INFO_STRUCT             IrqInfo;
    ISP_HOLD_INFO_STRUCT            HoldInfo;
    ISP_BUF_INFO_STRUCT             BufInfo;
    ISP_TIME_LOG_STRUCT             TimeLog;
    ISP_CALLBACK_STRUCT             Callback[ISP_CALLBACK_AMOUNT];
}ISP_INFO_STRUCT;

static struct tasklet_struct isp_tasklet;

static volatile MBOOL bSlowMotion = MFALSE;
static volatile MBOOL bRawEn = MFALSE;
static volatile MBOOL bRawDEn = MFALSE;


static ISP_INFO_STRUCT IspInfo;

volatile MUINT32 PrvAddr[_ChannelMax] = {0};

/*                                             
                                               */
#ifdef  T_STAMP_2_0
    #define SlowMotion  100
    typedef struct{
        volatile unsigned long long T_ns;//                                                 
        unsigned long interval_us; //           
        unsigned long compensation_us;
        MUINT32 fps;
        MUINT32 fcnt;
    }T_STAMP;
    
    static T_STAMP m_T_STAMP = {0};
#endif

/*                                                                              
 
                                                                               */
//         
#define ISP_KERNEL_MOTIFY_SINGAL_TEST
#ifdef ISP_KERNEL_MOTIFY_SINGAL_TEST
/*                       */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/siginfo.h>    //       
#include <linux/rcupdate.h> //             
#include <linux/sched.h>    //                     
#include <linux/debugfs.h>
#include <linux/uaccess.h>

//       
#define __tcmfunc


#define SIG_TEST 44 //                                                                                   

struct siginfo info;
struct task_struct *t;


int getTaskInfo( pid_t pid )
{
    /*                 */
    memset(&info, 0, sizeof(struct siginfo));
    info.si_signo = SIG_TEST;
    info.si_code = SI_QUEUE;    //                                                                                  
                                //                                                                                   
                                //                                                            
    info.si_int = 1234;         //                                            

    rcu_read_lock();

    t = find_task_by_vpid(pid);
    //                                                                                               
    if(t == NULL){
        LOG_DBG("no such pid");
        rcu_read_unlock();
        return -ENODEV;
    }
    rcu_read_unlock();

    return 0;
}

int sendSignal( void )
{
int ret = 0;
    ret = send_sig_info(SIG_TEST, &info, t);    //               
    if (ret < 0) {
        LOG_DBG("error sending signal");
        return ret;
    }

    return ret;
}

/*                       */

#endif  //                              

/*                                                                              
 
                                                                               */
static __inline MUINT32 ISP_MsToJiffies(MUINT32 Ms)
{
    return ((Ms * HZ + 512) >> 10);
}

/*                                                                              
 
                                                                               */
static __inline MUINT32 ISP_UsToJiffies(MUINT32 Us)
{
    return (((Us/1000) * HZ + 512) >> 10);
}

/*                                                                              
 
                                                                               */
static __inline MUINT32 ISP_GetIRQState(eISPIrq eIrq, MUINT32 type,MUINT32 userNumber, MUINT32 stus)
{
    MUINT32 ret;
    MUINT32 flags;
    //
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
    ret = (IspInfo.IrqInfo.Status[userNumber][type] & stus);
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
    //
    return ret;
}

/*                                                                              
 
                                                                               */
static __inline MUINT32 ISP_GetEDBufQueWaitDequeState(MINT32 idx)
{
    MUINT32 ret=MFALSE;
    //
    spin_lock(&(SpinLockEDBufQueList));
    if(P2_EDBUF_RingList[idx].bufSts==ISP_ED_BUF_STATE_RUNNING)
    {
        ret=MTRUE;
    }
    spin_unlock(&(SpinLockEDBufQueList));
    //
    return ret;
}
static __inline MUINT32 ISP_GetEDBufQueWaitFrameState(MINT32 idx)
{
    MUINT32 ret=MFALSE;
    //
    spin_lock(&(SpinLockEDBufQueList));
    if(P2_EDBUF_MgrList[idx].dequedNum==P2_Support_BurstQNum)
    {
        ret=MTRUE;
    }
    spin_unlock(&(SpinLockEDBufQueList));
    //
    return ret;
}
/*                                                                              
 
                                                                               */
static __inline MUINT32 ISP_JiffiesToMs(MUINT32 Jiffies)
{
    return ((Jiffies*1000)/HZ);
}


/*                                                                              
 
                                                                               */
static MUINT32 ISP_DumpDmaDeepDbg(void){
    if(g_bDmaERR_p1){
        g_DmaErr_p1[0] = (MUINT32)ISP_RD32(ISP_ADDR + 0x356c);
        g_DmaErr_p1[1] = (MUINT32)ISP_RD32(ISP_ADDR + 0x3570);
        g_DmaErr_p1[2] = (MUINT32)ISP_RD32(ISP_ADDR + 0x3574);
        g_DmaErr_p1[3] = (MUINT32)ISP_RD32(ISP_ADDR + 0x3578);
        g_DmaErr_p1[4] = (MUINT32)ISP_RD32(ISP_ADDR + 0x357C);
        g_DmaErr_p1[5] = (MUINT32)ISP_RD32(ISP_ADDR + 0x358c);
        g_DmaErr_p1[6] = (MUINT32)ISP_RD32(ISP_ADDR + 0x3590);
        g_DmaErr_p1[7] = (MUINT32)ISP_RD32(ISP_ADDR + 0x3594);
        g_DmaErr_p1[8] = (MUINT32)ISP_RD32(ISP_ADDR + 0x3598);
        g_DmaErr_p1[9] = (MUINT32)ISP_RD32(ISP_ADDR + 0x359c);
        g_DmaErr_p1[10] = (MUINT32)ISP_RD32(ISP_ADDR + 0x35a0);
        LOG_ERR("IMGI:0x%x,BPCI:0x%x,LSCI=0x%x,UFDI=0x%x,LCEI=0x%x,imgo=0x%x,rrzo:0x%x,lcso:0x%x,esfko:0x%x,aao:0x%x,ufeo:0x%x",\
        g_DmaErr_p1[0],\
        g_DmaErr_p1[1],\
        g_DmaErr_p1[2],\
        g_DmaErr_p1[3],\
        g_DmaErr_p1[4],\
        g_DmaErr_p1[5],\
        g_DmaErr_p1[6],\
        g_DmaErr_p1[7],\
        g_DmaErr_p1[8],\
        g_DmaErr_p1[9],\
        g_DmaErr_p1[10]);
        g_bDmaERR_p1 = MFALSE;
    }
    if(g_bDmaERR_p1_d){
        g_DmaErr_p1[11] = (MUINT32)ISP_RD32(ISP_ADDR + 0x35bc);
        g_DmaErr_p1[12] = (MUINT32)ISP_RD32(ISP_ADDR + 0x35c0);
        g_DmaErr_p1[13] = (MUINT32)ISP_RD32(ISP_ADDR + 0x35c4);
        g_DmaErr_p1[14] = (MUINT32)ISP_RD32(ISP_ADDR + 0x35c8);
        g_DmaErr_p1[15] = (MUINT32)ISP_RD32(ISP_ADDR + 0x35cc);
        g_DmaErr_p1[16] = (MUINT32)ISP_RD32(ISP_ADDR + 0x35d0);
        g_DmaErr_p1[17] = (MUINT32)ISP_RD32(ISP_ADDR + 0x35d4);
        LOG_ERR("BPCI_D:0x%x,LSCI_D:0x%x,IMGO_D=0x%x,RRZO_D=0x%x,LSCO_D=0x%x,AFO_D=0x%x,AAO_D:0x%x",\
        g_DmaErr_p1[11],\
        g_DmaErr_p1[12],\
        g_DmaErr_p1[13],\
        g_DmaErr_p1[14],\
        g_DmaErr_p1[15],\
        g_DmaErr_p1[16],\
        g_DmaErr_p1[17]);
        g_bDmaERR_p1_d = MFALSE;
    }
#if 0
    if(g_bDmaERR_p2){
        LOG_ERR("vipi:0x%x,VIPI:0x%x,VIP2I=0x%x,VIP3I=0x%x,MFBO=0x%x,IMG3BO=0x%x,IMG3CO:0x%x,IMG2O:0x%x,IMG3O:0x%x,FEO:0x%x",\
        ISP_RD32(ISP_ADDR + 0x3574),\
        ISP_RD32(ISP_ADDR + 0x3580),\
        ISP_RD32(ISP_ADDR + 0x3584),\
        ISP_RD32(ISP_ADDR + 0x3588),\
        ISP_RD32(ISP_ADDR + 0x35a4),\
        ISP_RD32(ISP_ADDR + 0x35a8),\
        ISP_RD32(ISP_ADDR + 0x35ac),\
        ISP_RD32(ISP_ADDR + 0x35b0),\
        ISP_RD32(ISP_ADDR + 0x35b4),\
        ISP_RD32(ISP_ADDR + 0x35b8));
        g_bDmaERR_p2 = MFALSE;
    }

    if(g_bDmaERR_deepDump){
        ISP_WR32((ISP_ADDR + 0x160),0x0);
        ISP_WR32((ISP_ADDR + 0x35f4),0x1E);
        LOG_ERR("imgi_debug_0 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x11E);
        LOG_ERR("imgi_debug_1 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x21E);
        LOG_ERR("imgi_debug_2 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x31E);
        LOG_ERR("imgi_debug_3 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        //    
        ISP_WR32((ISP_ADDR + 0x35f4),0x41E);
        LOG_ERR("vipi_debug_0 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x51E);
        LOG_ERR("vipi_debug_1 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x61E);
        LOG_ERR("vipi_debug_2 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x71E);
        LOG_ERR("vipi_debug_3 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        //    
        ISP_WR32((ISP_ADDR + 0x35f4),0x81E);
        LOG_ERR("imgo_debug_0 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x91E);
        LOG_ERR("imgo_debug_1 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0xa1E);
        LOG_ERR("imgo_debug_2 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0xb1E);
        LOG_ERR("imgo_debug_3 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        //      
        ISP_WR32((ISP_ADDR + 0x35f4),0xc1E);
        LOG_ERR("imgo_d_debug_0 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0xd1E);
        LOG_ERR("imgo_d_debug_1 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0xe1E);
        LOG_ERR("imgo_d_debug_2 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0xf1E);
        LOG_ERR("imgo_d_debug_3 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        //    
        ISP_WR32((ISP_ADDR + 0x35f4),0x101E);
        LOG_ERR("rrzo_debug_0 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x111E);
        LOG_ERR("rrzo_debug_1 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x121E);
        LOG_ERR("rrzo_debug_2 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x131E);
        LOG_ERR("rrzo_debug_3 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        //      
        ISP_WR32((ISP_ADDR + 0x35f4),0x151E);
        LOG_ERR("rrzo_d_debug_0 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x161E);
        LOG_ERR("rrzo_d_debug_1 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x171E);
        LOG_ERR("rrzo_d_debug_2 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x181E);
        LOG_ERR("rrzo_d_debug_3 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        //     
        ISP_WR32((ISP_ADDR + 0x35f4),0x181E);
        LOG_ERR("img3o_debug_0 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x191E);
        LOG_ERR("img3o_debug_1 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x1a1E);
        LOG_ERR("img3o_debug_2 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x1b1E);
        LOG_ERR("img3o_debug_3 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        //     
        ISP_WR32((ISP_ADDR + 0x35f4),0x1c1E);
        LOG_ERR("img3o_debug_0 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x1d1E);
        LOG_ERR("img3o_debug_1 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x1e1E);
        LOG_ERR("img3o_debug_2 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        ISP_WR32((ISP_ADDR + 0x35f4),0x1f1E);
        LOG_ERR("img3o_debug_3 = 0x%x\n",ISP_RD32(ISP_ADDR + 0x164));
        g_bDmaERR_deepDump= MFALSE;
    }
#endif

    return 0;
}

#define RegDump(start,end) {\
    MUINT32 i;\
    for(i=start;i<=end;i+=0x10){\
        LOG_DBG("[0x%08X %08X],[0x%08X %08X],[0x%08X %08X],[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i),\
                    (unsigned int)(ISP_TPIPE_ADDR + i+0x4), (unsigned int)ISP_RD32(ISP_ADDR + i+0x4),\
                    (unsigned int)(ISP_TPIPE_ADDR + i+0x8), (unsigned int)ISP_RD32(ISP_ADDR + i+0x8),\
                    (unsigned int)(ISP_TPIPE_ADDR + i+0xc), (unsigned int)ISP_RD32(ISP_ADDR + i+0xc));\
    }\
}


static MINT32 ISP_DumpReg(void)
{
    MINT32 Ret = 0;
    //
    LOG_DBG("- E.");
    //
    //                                              

    //                     
    //                                                  
    //                                                                
    //
    //           
    ISP_WR32((ISP_ADDR + 0x40c0),0x746); //                       
    LOG_DBG("[0x%08X %08X] [0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x40c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x40c0),\
        (unsigned int)(ISP_TPIPE_ADDR + 0x40d8), (unsigned int)ISP_RD32(ISP_ADDR + 0x40d8));
    ISP_WR32((ISP_ADDR + 0x40c0),0x946); //                       
    LOG_DBG("[0x%08X %08X] [0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x40c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x40c0),\
        (unsigned int)(ISP_TPIPE_ADDR + 0x40d8), (unsigned int)ISP_RD32(ISP_ADDR + 0x40d8));

    //       
    RegDump(0x0,0x200);
    //               
    RegDump(0x3200,0x3570);
    //                    
    RegDump(0x3300,0x3400);
    //                        
    RegDump(0x3560,0x35e0);
#if 0
    g_bDmaERR_p1 = g_bDmaERR_p1_d = g_bDmaERR_p2 = g_bDmaERR_deepDump = MTRUE;
    ISP_DumpDmaDeepDbg();
#endif
    //   
    RegDump(0x410,0x4a0);
    //   
    RegDump(0x2410,0x2450);
    //    
    LOG_ERR("[0x%08X %08X],[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x4f0), (unsigned int)ISP_RD32(ISP_ADDR + 0x534),(unsigned int)(ISP_TPIPE_ADDR + 0x4f4), (unsigned int)ISP_RD32(ISP_ADDR + 0x538));
    //   
    RegDump(0x530,0x550);
    //       
    RegDump(0x5b0,0x5d0);
    //      
    RegDump(0x650,0x690);
    //      
    RegDump(0x6b0,0x700);
    //   
    RegDump(0x770,0x780);
    //   
    RegDump(0x7a0,0x7d0);
    //   
    RegDump(0xdc0,0xdf0);
    //           
    RegDump(0xe00,0xe30);
    //           
    LOG_DBG("[0x%08X %08X],[0x%08X %08X]", (unsigned int)(0x10217000), (unsigned int)ISP_RD32(ISP_MIPI_ANA_ADDR),(unsigned int)(0x10217004), (unsigned int)ISP_RD32(ISP_MIPI_ANA_ADDR+0x4));
    LOG_DBG("[0x%08X %08X],[0x%08X %08X]", (unsigned int)(0x10217008), (unsigned int)ISP_RD32(ISP_MIPI_ANA_ADDR+0x8),(unsigned int)(0x1021700c), (unsigned int)ISP_RD32(ISP_MIPI_ANA_ADDR+0xc));
    LOG_DBG("[0x%08X %08X],[0x%08X %08X]", (unsigned int)(0x10217030), (unsigned int)ISP_RD32(ISP_MIPI_ANA_ADDR+0x30),(unsigned int)(0x10219030), (unsigned int)ISP_RD32(ISP_MIPI_ANA_ADDR+0x2030));

    //             
    ISP_WR32((ISP_ADDR + 0x43B8), 0x02);
    LOG_DBG("[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x43B8), (unsigned int)ISP_RD32(ISP_ADDR + 0x43B8));
    LOG_DBG("[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x43BC), (unsigned int)ISP_RD32(ISP_ADDR + 0x43BC));
    ISP_WR32((ISP_ADDR + 0x43B8), 0x12);
    LOG_DBG("[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x43B8), (unsigned int)ISP_RD32(ISP_ADDR + 0x43B8));
    LOG_DBG("[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x43BC), (unsigned int)ISP_RD32(ISP_ADDR + 0x43BC));
    //             
    ISP_WR32((ISP_ADDR + 0x4BB8), 0x02);
    LOG_DBG("[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x4BB8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BB8));
    LOG_DBG("[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x4BBC), (unsigned int)ISP_RD32(ISP_ADDR + 0x43BC));
    ISP_WR32((ISP_ADDR + 0x4BB8), 0x12);
    LOG_DBG("[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x43B8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BB8));
    LOG_DBG("[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x4BBC), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BBC));

    //       
    LOG_ERR("[0x%08X %08X],[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x4008), (unsigned int)ISP_RD32(ISP_ADDR + 0x4008),(unsigned int)(ISP_TPIPE_ADDR + 0x4100), (unsigned int)ISP_RD32(ISP_ADDR + 0x4100));
    RegDump(0x4120,0x4160);
    RegDump(0x4360,0x43f0)
    //       
    LOG_ERR("[0x%08X %08X],[0x%08X %08X]", (unsigned int)(ISP_TPIPE_ADDR + 0x4008), (unsigned int)ISP_RD32(ISP_ADDR + 0x4008),(unsigned int)(ISP_TPIPE_ADDR + 0x4100), (unsigned int)ISP_RD32(ISP_ADDR + 0x4100));
    RegDump(0x4520,0x4560);
    RegDump(0x4600,0x4610);
    RegDump(0x4760,0x47f0);
    //     
    RegDump(0x2530,0x2550);
    //     
    RegDump(0x25b0,0x25d0);
    //    
    RegDump(0x2650,0x2690);
    //    
    RegDump(0x26b0,0x2700);
    //     
    RegDump(0x27a0,0x27d0);
    //                 
    RegDump(0x2e00,0x2e30);

    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x800), (unsigned int)ISP_RD32(ISP_ADDR + 0x800));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x880), (unsigned int)ISP_RD32(ISP_ADDR + 0x880));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x884), (unsigned int)ISP_RD32(ISP_ADDR + 0x884));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x888), (unsigned int)ISP_RD32(ISP_ADDR + 0x888));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x8A0), (unsigned int)ISP_RD32(ISP_ADDR + 0x8A0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x920), (unsigned int)ISP_RD32(ISP_ADDR + 0x920));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x924), (unsigned int)ISP_RD32(ISP_ADDR + 0x924));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x928), (unsigned int)ISP_RD32(ISP_ADDR + 0x928));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x92C), (unsigned int)ISP_RD32(ISP_ADDR + 0x92C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x930), (unsigned int)ISP_RD32(ISP_ADDR + 0x930));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x934), (unsigned int)ISP_RD32(ISP_ADDR + 0x934));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x938), (unsigned int)ISP_RD32(ISP_ADDR + 0x938));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x93C), (unsigned int)ISP_RD32(ISP_ADDR + 0x93C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x960), (unsigned int)ISP_RD32(ISP_ADDR + 0x960));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x9C4), (unsigned int)ISP_RD32(ISP_ADDR + 0x9C4));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x9E4), (unsigned int)ISP_RD32(ISP_ADDR + 0x9E4));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x9E8), (unsigned int)ISP_RD32(ISP_ADDR + 0x9E8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x9EC), (unsigned int)ISP_RD32(ISP_ADDR + 0x9EC));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xA00), (unsigned int)ISP_RD32(ISP_ADDR + 0xA00));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xA04), (unsigned int)ISP_RD32(ISP_ADDR + 0xA04));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xA08), (unsigned int)ISP_RD32(ISP_ADDR + 0xA08));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xA0C), (unsigned int)ISP_RD32(ISP_ADDR + 0xA0C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xA10), (unsigned int)ISP_RD32(ISP_ADDR + 0xA10));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xA14), (unsigned int)ISP_RD32(ISP_ADDR + 0xA14));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xA20), (unsigned int)ISP_RD32(ISP_ADDR + 0xA20));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xAA0), (unsigned int)ISP_RD32(ISP_ADDR + 0xAA0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xACC), (unsigned int)ISP_RD32(ISP_ADDR + 0xACC));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB00), (unsigned int)ISP_RD32(ISP_ADDR + 0xB00));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB04), (unsigned int)ISP_RD32(ISP_ADDR + 0xB04));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB08), (unsigned int)ISP_RD32(ISP_ADDR + 0xB08));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB0C), (unsigned int)ISP_RD32(ISP_ADDR + 0xB0C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB10), (unsigned int)ISP_RD32(ISP_ADDR + 0xB10));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB14), (unsigned int)ISP_RD32(ISP_ADDR + 0xB14));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB18), (unsigned int)ISP_RD32(ISP_ADDR + 0xB18));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB1C), (unsigned int)ISP_RD32(ISP_ADDR + 0xB1C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB20), (unsigned int)ISP_RD32(ISP_ADDR + 0xB20));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB44), (unsigned int)ISP_RD32(ISP_ADDR + 0xB44));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB48), (unsigned int)ISP_RD32(ISP_ADDR + 0xB48));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB4C), (unsigned int)ISP_RD32(ISP_ADDR + 0xB4C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB50), (unsigned int)ISP_RD32(ISP_ADDR + 0xB50));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB54), (unsigned int)ISP_RD32(ISP_ADDR + 0xB54));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB58), (unsigned int)ISP_RD32(ISP_ADDR + 0xB58));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB5C), (unsigned int)ISP_RD32(ISP_ADDR + 0xB5C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xB60), (unsigned int)ISP_RD32(ISP_ADDR + 0xB60));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xBA0), (unsigned int)ISP_RD32(ISP_ADDR + 0xBA0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xBA4), (unsigned int)ISP_RD32(ISP_ADDR + 0xBA4));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xBA8), (unsigned int)ISP_RD32(ISP_ADDR + 0xBA8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xBAC), (unsigned int)ISP_RD32(ISP_ADDR + 0xBAC));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xBB0), (unsigned int)ISP_RD32(ISP_ADDR + 0xBB0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xBB4), (unsigned int)ISP_RD32(ISP_ADDR + 0xBB4));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xBB8), (unsigned int)ISP_RD32(ISP_ADDR + 0xBB8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xBBC), (unsigned int)ISP_RD32(ISP_ADDR + 0xBBC));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xBC0), (unsigned int)ISP_RD32(ISP_ADDR + 0xBC0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xC20), (unsigned int)ISP_RD32(ISP_ADDR + 0xC20));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xCC0), (unsigned int)ISP_RD32(ISP_ADDR + 0xCC0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xCE4), (unsigned int)ISP_RD32(ISP_ADDR + 0xCE4));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xCE8), (unsigned int)ISP_RD32(ISP_ADDR + 0xCE8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xCEC), (unsigned int)ISP_RD32(ISP_ADDR + 0xCEC));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xCF0), (unsigned int)ISP_RD32(ISP_ADDR + 0xCF0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xCF4), (unsigned int)ISP_RD32(ISP_ADDR + 0xCF4));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xCF8), (unsigned int)ISP_RD32(ISP_ADDR + 0xCF8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xCFC), (unsigned int)ISP_RD32(ISP_ADDR + 0xCFC));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD24), (unsigned int)ISP_RD32(ISP_ADDR + 0xD24));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD28), (unsigned int)ISP_RD32(ISP_ADDR + 0xD28));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD2C), (unsigned int)ISP_RD32(ISP_ADDR + 0xD2c));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD40), (unsigned int)ISP_RD32(ISP_ADDR + 0xD40));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD64), (unsigned int)ISP_RD32(ISP_ADDR + 0xD64));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD68), (unsigned int)ISP_RD32(ISP_ADDR + 0xD68));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD6C), (unsigned int)ISP_RD32(ISP_ADDR + 0xD6c));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD70), (unsigned int)ISP_RD32(ISP_ADDR + 0xD70));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD74), (unsigned int)ISP_RD32(ISP_ADDR + 0xD74));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD78), (unsigned int)ISP_RD32(ISP_ADDR + 0xD78));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xD7C), (unsigned int)ISP_RD32(ISP_ADDR + 0xD7C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xDA4), (unsigned int)ISP_RD32(ISP_ADDR + 0xDA4));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xDA8), (unsigned int)ISP_RD32(ISP_ADDR + 0xDA8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0xDAC), (unsigned int)ISP_RD32(ISP_ADDR + 0xDAC));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x2410), (unsigned int)ISP_RD32(ISP_ADDR + 0x2410));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x2414), (unsigned int)ISP_RD32(ISP_ADDR + 0x2414));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x2418), (unsigned int)ISP_RD32(ISP_ADDR + 0x2418));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x241C), (unsigned int)ISP_RD32(ISP_ADDR + 0x241C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x2420), (unsigned int)ISP_RD32(ISP_ADDR + 0x2420));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x243C), (unsigned int)ISP_RD32(ISP_ADDR + 0x243C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x2440), (unsigned int)ISP_RD32(ISP_ADDR + 0x2440));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x2444), (unsigned int)ISP_RD32(ISP_ADDR + 0x2444));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x2448), (unsigned int)ISP_RD32(ISP_ADDR + 0x2448));

    //       
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4900), (unsigned int)ISP_RD32(ISP_ADDR + 0x4900));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4920), (unsigned int)ISP_RD32(ISP_ADDR + 0x4920));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4924), (unsigned int)ISP_RD32(ISP_ADDR + 0x4924));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4928), (unsigned int)ISP_RD32(ISP_ADDR + 0x4928));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x492C), (unsigned int)ISP_RD32(ISP_ADDR + 0x492C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4930), (unsigned int)ISP_RD32(ISP_ADDR + 0x4930));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4934), (unsigned int)ISP_RD32(ISP_ADDR + 0x4934));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4938), (unsigned int)ISP_RD32(ISP_ADDR + 0x4938));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BA0), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BA0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BA4), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BA4));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BA8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BA8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BAC), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BAC));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BB0), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BB0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BB4), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BB4));
    ISP_WR32((ISP_ADDR + 0x4BB8), 0x10);
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BB8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BB8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BBC), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BBC));
    ISP_WR32((ISP_ADDR + 0x4BB8), 0x11);
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BB8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BB8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BBC), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BBC));
    ISP_WR32((ISP_ADDR + 0x4BB8), 0x12);
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BB8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BB8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4BBC), (unsigned int)ISP_RD32(ISP_ADDR + 0x4BBC));
    //       
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4D00), (unsigned int)ISP_RD32(ISP_ADDR + 0x4D00));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4D20), (unsigned int)ISP_RD32(ISP_ADDR + 0x4D20));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4D24), (unsigned int)ISP_RD32(ISP_ADDR + 0x4D24));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4D28), (unsigned int)ISP_RD32(ISP_ADDR + 0x4D28));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4D2C), (unsigned int)ISP_RD32(ISP_ADDR + 0x4D2C));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4D30), (unsigned int)ISP_RD32(ISP_ADDR + 0x4D30));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4D34), (unsigned int)ISP_RD32(ISP_ADDR + 0x4D34));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4D38), (unsigned int)ISP_RD32(ISP_ADDR + 0x4D38));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FA0), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FA0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FA4), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FA4));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FA8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FA8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FAC), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FAC));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FB0), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FB0));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FB4), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FB4));
    ISP_WR32((ISP_ADDR + 0x4FB8), 0x10);
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FB8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FB8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FBC), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FBC));
    ISP_WR32((ISP_ADDR + 0x4FB8), 0x11);
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FB8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FB8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FBC), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FBC));
    ISP_WR32((ISP_ADDR + 0x4FB8), 0x12);
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FB8), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FB8));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x4FBC), (unsigned int)ISP_RD32(ISP_ADDR + 0x4FBC));

    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x35FC), (unsigned int)ISP_RD32(ISP_ADDR + 0x35FC));
    LOG_DBG("end MT6593");

    //
    LOG_DBG("0x%08X %08X ", (unsigned int)ISP_ADDR_CAMINF, (unsigned int)ISP_RD32(ISP_ADDR_CAMINF));
    LOG_DBG("0x%08X %08X ", (unsigned int)(ISP_TPIPE_ADDR + 0x150), (unsigned int)ISP_RD32(ISP_ADDR + 0x150));
    //
    //                         


    //        
    LOG_DBG("MDPCROP Related");
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_ADDR + 0xd10), (unsigned int)ISP_RD32(ISP_ADDR + 0xd10));
    LOG_DBG("0x%08X %08X", (unsigned int)(ISP_ADDR + 0xd20), (unsigned int)ISP_RD32(ISP_ADDR + 0xd20));
    //  
    LOG_DBG("CQ Related");
    ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x6000 );
    LOG_DBG("0x%08X %08X (0x15004160=6000)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x7000 );
    LOG_DBG("0x%08X %08X (0x15004160=7000)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x8000 );
    LOG_DBG("0x%08X %08X (0x15004160=8000)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    //          
    LOG_DBG("IMGI_DEBUG Related");
    ISP_WR32( ISP_IMGSYS_BASE + 0x75f4, 0x001e );
    LOG_DBG("0x%08X %08X (0x150075f4=001e)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    ISP_WR32( ISP_IMGSYS_BASE + 0x75f4, 0x011e );
    LOG_DBG("0x%08X %08X (0x150075f4=011e)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    ISP_WR32( ISP_IMGSYS_BASE + 0x75f4, 0x021e );
    LOG_DBG("0x%08X %08X (0x150075f4=021e)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    ISP_WR32( ISP_IMGSYS_BASE + 0x75f4, 0x031e );
    LOG_DBG("0x%08X %08X (0x150075f4=031e)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    //   
    LOG_DBG("yuv-mdp crop Related");
    ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x3014 );
    LOG_DBG("0x%08X %08X (0x15004160=3014)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    LOG_DBG("yuv-c24b out Related");
    ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x301e );
    LOG_DBG("0x%08X %08X (0x15004160=301e)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x301f );
    LOG_DBG("0x%08X %08X (0x15004160=301f)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x3020 );
    LOG_DBG("0x%08X %08X (0x15004160=3020)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));
    ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x3021 );
    LOG_DBG("0x%08X %08X (0x15004160=3021)", (unsigned int)(ISP_IMGSYS_BASE + 0x4164), (unsigned int)ISP_RD32(ISP_IMGSYS_BASE + 0x4164));


#if 0 //                    
{
    int tpipePA = ISP_RD32(ISP_ADDR + 0x204);
    int ctlStart = ISP_RD32(ISP_ADDR + 0x000);
    int ctlTcm = ISP_RD32(ISP_ADDR + 0x054);
    int map_va=0, map_size;
    int i;
    int *pMapVa;
#define TPIPE_DUMP_SIZE    200

    if((ctlStart&0x01)&&(tpipePA)&&(ctlTcm&0x80000000)){ //          
        map_va = 0;
        m4u_mva_map_kernel( tpipePA, TPIPE_DUMP_SIZE, 0, &map_va, &map_size);
        pMapVa = map_va;
        LOG_DBG("pMapVa(0x%x),map_size(0x%x)",pMapVa,map_size);
        LOG_DBG("ctlStart(0x%x),tpipePA(0x%x),ctlTcm(0x%x)",ctlStart,tpipePA,ctlTcm);
        if(pMapVa){
            for(i=0;i<TPIPE_DUMP_SIZE;i+=10) {
                LOG_DBG("[idx(%d)]%08X-%08X-%08X-%08X-%08X-%08X-%08X-%08X-%08X-%08X",i,pMapVa[i],pMapVa[i+1],pMapVa[i+2],pMapVa[i+3],
                    pMapVa[i+4],pMapVa[i+5],pMapVa[i+6],pMapVa[i+7],pMapVa[i+8],pMapVa[i+9]);
            }
        }
        m4u_mva_unmap_kernel(tpipePA, map_size, map_va);
    }
}
#endif

    //                                                   
    //
    LOG_DBG("- X.");
    //
    return Ret;
}


/*                                                                              
 
                                                                               */
static void ISP_EnableClock(MBOOL En)
{

    if(G_u4EnableClockCount == 1)
    {
        //                                                                            
    }
    if (En) //              
    {
        //                                                                                                                                        
        //                                                          
        spin_lock(&(IspInfo.SpinLockClock));
        //                                                                                
        switch(G_u4EnableClockCount)
        {
            case 0:
                enable_clock(MT_CG_DISP0_SMI_COMMON, "CAMERA");
                enable_clock(MT_CG_IMAGE_CAM_SMI, "CAMERA");
                enable_clock(MT_CG_IMAGE_CAM_CAM, "CAMERA");
                enable_clock(MT_CG_IMAGE_SEN_TG,  "CAMERA");
                enable_clock(MT_CG_IMAGE_SEN_CAM, "CAMERA");
                enable_clock(MT_CG_IMAGE_CAM_SV,  "CAMERA");
                //                                       
                enable_clock(MT_CG_IMAGE_LARB2_SMI, "CAMERA");
                break;
            default:
                break;
        }
        G_u4EnableClockCount++;
        spin_unlock(&(IspInfo.SpinLockClock));
    }
    else    //               
    {
        spin_lock(&(IspInfo.SpinLockClock));
        //                                                                                  
        G_u4EnableClockCount--;
        switch(G_u4EnableClockCount)
        {
            case 0:
                //                
                disable_clock(MT_CG_IMAGE_CAM_SMI, "CAMERA");
                disable_clock(MT_CG_IMAGE_CAM_CAM, "CAMERA");
                disable_clock(MT_CG_IMAGE_SEN_TG,  "CAMERA");
                disable_clock(MT_CG_IMAGE_SEN_CAM, "CAMERA");
                disable_clock(MT_CG_IMAGE_CAM_SV,  "CAMERA");
                //                                        
                disable_clock(MT_CG_IMAGE_LARB2_SMI, "CAMERA");
                disable_clock(MT_CG_DISP0_SMI_COMMON, "CAMERA");
                break;
            default:
                break;
        }
        spin_unlock(&(IspInfo.SpinLockClock));
    }
}

/*                                                                              
 
                                                                               */
static inline void ISP_Reset(MINT32 rst_path)
{
    //                                                  
    //                                 
    MUINT32 Reg;
    MUINT32 setReg;
    //                 
    //
    LOG_DBG("- E.");

    LOG_DBG("isp gate clk(0x%x),rst_path(%d)",ISP_RD32(ISP_ADDR_CAMINF),rst_path);


    if(rst_path==ISP_REG_SW_CTL_RST_CAM_P1)
    {
        //                         
        #if 1
        Reg = ISP_RD32(ISP_REG_ADDR_CAM_SW_CTL);
        setReg = (Reg&(~ISP_REG_SW_CTL_SW_RST_P1_MASK))|(ISP_REG_SW_CTL_SW_RST_TRIG&ISP_REG_SW_CTL_SW_RST_P1_MASK);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, setReg);
        //                                     
        do {
            Reg = ISP_RD32(ISP_REG_ADDR_CAM_SW_CTL);
        } while ((!Reg) & ISP_REG_SW_CTL_SW_RST_STATUS);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, ISP_REG_SW_CTL_HW_RST);

        setReg = (Reg&(~ISP_REG_SW_CTL_SW_RST_P1_MASK))|(0x00&ISP_REG_SW_CTL_SW_RST_P1_MASK);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, setReg);
        #else
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, ISP_REG_SW_CTL_SW_RST_TRIG);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, 0);
        do {
            Reg = ISP_RD32(ISP_REG_ADDR_CAM_SW_CTL);
        } while ((!Reg) & ISP_REG_SW_CTL_SW_RST_STATUS);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, ISP_REG_SW_CTL_HW_RST);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, 0);
        #endif
    }
    else if(rst_path==ISP_REG_SW_CTL_RST_CAM_P2)
    {
        #if 1
        Reg = ISP_RD32(ISP_REG_ADDR_CAM_SW_CTL);
        setReg = (Reg&(~ISP_REG_SW_CTL_SW_RST_P2_MASK))|(ISP_REG_SW_CTL_SW_RST_P2_TRIG&ISP_REG_SW_CTL_SW_RST_P2_MASK);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, setReg);
        //                                     
        do {
            Reg = ISP_RD32(ISP_REG_ADDR_CAM_SW_CTL);
        } while ((!Reg) & ISP_REG_SW_CTL_SW_RST_P2_STATUS);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, ISP_REG_SW_CTL_HW_RST_P2);

        setReg = (Reg&(~ISP_REG_SW_CTL_SW_RST_P2_MASK))|(0x00&ISP_REG_SW_CTL_SW_RST_P2_MASK);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, setReg);

        #else
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, ISP_REG_SW_CTL_SW_RST_P2_TRIG);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, 0);
        do {
            Reg = ISP_RD32(ISP_REG_ADDR_CAM_SW_CTL);
        } while ((!Reg) & ISP_REG_SW_CTL_SW_RST_P2_STATUS);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, ISP_REG_SW_CTL_HW_RST_P2);
        ISP_WR32(ISP_REG_ADDR_CAM_SW_CTL, 0);
        #endif
    }
    else if(rst_path==ISP_REG_SW_CTL_RST_CAMSV)
    {
        ISP_WR32(ISP_REG_ADDR_CAMSV_SW_CTL, ISP_REG_SW_CTL_SW_RST_TRIG);
        ISP_WR32(ISP_REG_ADDR_CAMSV_SW_CTL, 0);
        do {
            Reg = ISP_RD32(ISP_REG_ADDR_CAMSV_SW_CTL);
        } while ((!Reg) & ISP_REG_SW_CTL_SW_RST_STATUS);
        ISP_WR32(ISP_REG_ADDR_CAMSV_SW_CTL, ISP_REG_SW_CTL_HW_RST);
        ISP_WR32(ISP_REG_ADDR_CAMSV_SW_CTL, 0);
    }
    else if(rst_path==ISP_REG_SW_CTL_RST_CAMSV2)
    {
        ISP_WR32(ISP_REG_ADDR_CAMSV2_SW_CTL, ISP_REG_SW_CTL_SW_RST_TRIG);
        ISP_WR32(ISP_REG_ADDR_CAMSV2_SW_CTL, 0);
        do {
            Reg = ISP_RD32(ISP_REG_ADDR_CAMSV2_SW_CTL);
        } while ((!Reg) & ISP_REG_SW_CTL_SW_RST_STATUS);
        ISP_WR32(ISP_REG_ADDR_CAMSV2_SW_CTL, ISP_REG_SW_CTL_HW_RST);
        ISP_WR32(ISP_REG_ADDR_CAMSV2_SW_CTL, 0);
    }
#if 0
    //                
    for(i=0;i< _IRQ_MAX; i++) {
        spin_lock_irqsave(&(IspInfo.SpinLockIrq[i]), flags);
    }
#endif
#if 0
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[_IRQ]), flags);
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[_IRQ_D]), flags);
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[_CAMSV_IRQ]), flags);
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[_CAMSV_D_IRQ]), flags);
    for(i=0; i<ISP_IRQ_TYPE_AMOUNT; i++) {
        IspInfo.IrqInfo.Status[i] = 0;
    }
    for(i=0;i<_ChannelMax;i++) {
        PrvAddr[i] = 0;
    }
#if 0
    for(i=0;i< _IRQ_MAX ; i++) {
        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[i]), flags);
    }
#endif
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[_CAMSV_D_IRQ]), flags);
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[_CAMSV_IRQ]), flags);
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[_IRQ_D]), flags);
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[_IRQ]), flags);
//
#endif


    //
    LOG_DBG("- X.");
}

/*                                                                              
 
                                                                               */
static MINT32 ISP_ReadReg(ISP_REG_IO_STRUCT* pRegIo)
{
    MUINT32 i;
    MINT32 Ret = 0;
    //
    ISP_REG_STRUCT reg;
    //                                        
    ISP_REG_STRUCT* pData = (ISP_REG_STRUCT*)pRegIo->pData;
    for (i = 0; i < pRegIo->Count; i++)
    {
        if  ( 0 != get_user(reg.Addr, (MUINT32*)pData) )
        {
            LOG_ERR("get_user failed");
            Ret = -EFAULT;
            goto EXIT;
        }
        //        
        //
        if((ISP_ADDR_CAMINF + reg.Addr >= ISP_ADDR) && (ISP_ADDR_CAMINF + reg.Addr < (ISP_ADDR_CAMINF+ISP_RANGE)))
        {
            reg.Val = ISP_RD32(ISP_ADDR_CAMINF + reg.Addr);
        }
        else
        {
            LOG_ERR("Wrong address(0x%x)",(unsigned int)(ISP_ADDR_CAMINF + reg.Addr));
            reg.Val = 0;
        }
        //
        //                                                                                   

        if  ( 0 != put_user(reg.Val, (MUINT32*)&(pData->Val)) )
        {
            LOG_ERR("put_user failed");
            Ret = -EFAULT;
            goto EXIT;
        }
        pData++;
        //
    }
    //
    EXIT:
    return Ret;
}


/*                                                                              
 
                                                                               */
static MINT32 ISP_WriteRegToHw(
    ISP_REG_STRUCT* pReg,
    MUINT32         Count)
{
    MINT32 Ret = 0;
    MUINT32 i;
    //
    if(IspInfo.DebugMask & ISP_DBG_WRITE_REG)
    {
        LOG_DBG("- E.");
    }
    //
    spin_lock(&(IspInfo.SpinLockIsp));
    for(i = 0; i < Count; i++)
    {
        if(IspInfo.DebugMask & ISP_DBG_WRITE_REG)
        {
            LOG_DBG("Addr(0x%08X), Val(0x%08X)", (MUINT32)(ISP_ADDR_CAMINF + pReg[i].Addr), (MUINT32)(pReg[i].Val));
        }
        if(((ISP_ADDR_CAMINF + pReg[i].Addr) >= ISP_ADDR) && ((ISP_ADDR_CAMINF + pReg[i].Addr) < (ISP_ADDR_CAMINF+ISP_RANGE)))
        {
            ISP_WR32(ISP_ADDR_CAMINF + pReg[i].Addr, pReg[i].Val);
        }
        else
        {
            LOG_ERR("wrong address(0x%x)",(unsigned int)(ISP_ADDR_CAMINF + pReg[i].Addr));
        }
    }
    spin_unlock(&(IspInfo.SpinLockIsp));
    //
    return Ret;
}

/*                                                                              
 
                                                                                
                                                                                                                                                    
 
              
      
                                             
     
                        
     
      
                                         
     
                                                               
                                          
                                              
     
 

                                                                               
 
                                                                               */
static void ISP_BufWrite_Dump(void)
{
    MUINT32 i;
    //
    LOG_DBG("- E.");
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        LOG_DBG("i=%d, Status=%d, Size=%d",i,IspInfo.BufInfo.Write[i].Status,IspInfo.BufInfo.Write[i].Size);
        IspInfo.BufInfo.Write[i].Status = ISP_BUF_STATUS_EMPTY;
        IspInfo.BufInfo.Write[i].Size = 0;
        IspInfo.BufInfo.Write[i].pData = NULL;
    }
}


/*                                                                              
 
                                                                               */
static void ISP_BufWrite_Free(void)
{
    MUINT32 i;
    //
    if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
    {
        LOG_DBG("- E.");
    }
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        IspInfo.BufInfo.Write[i].Status = ISP_BUF_STATUS_EMPTY;
        IspInfo.BufInfo.Write[i].Size = 0;
        if(IspInfo.BufInfo.Write[i].pData != NULL)
        {
            kfree(IspInfo.BufInfo.Write[i].pData);
            IspInfo.BufInfo.Write[i].pData = NULL;
        }
    }
}

/*                                                                              
 
                                                                               */
static MBOOL ISP_BufWrite_Alloc(void)
{
    MUINT32 i;
    //
    if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
    {
        LOG_DBG("- E.");
    }
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        IspInfo.BufInfo.Write[i].Status = ISP_BUF_STATUS_EMPTY;
        IspInfo.BufInfo.Write[i].Size = 0;
        IspInfo.BufInfo.Write[i].pData = (MUINT8*)kmalloc(ISP_BUF_SIZE_WRITE, GFP_ATOMIC);
        if(IspInfo.BufInfo.Write[i].pData == NULL)
        {
            LOG_DBG("ERROR: i = %d, pData is NULL",i);
            ISP_BufWrite_Free();
            return false;
        }
    }
    return true;
}

/*                                                                              
 
                                                                               */
static void ISP_BufWrite_Reset(void)
{
    MUINT32 i;
    //
    if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
    {
        LOG_DBG("- E.");
    }
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        IspInfo.BufInfo.Write[i].Status = ISP_BUF_STATUS_EMPTY;
        IspInfo.BufInfo.Write[i].Size = 0;
    }
}

/*                                                                              
 
                                                                               */
static __inline MUINT32 ISP_BufWrite_GetAmount(void)
{
    MUINT32 i, Count = 0;
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        if(IspInfo.BufInfo.Write[i].Status == ISP_BUF_STATUS_READY)
        {
            Count++;
        }
    }
    //
    if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
    {
        LOG_DBG("Count = %d",Count);
    }
    return Count;
}

/*                                                                              
 
                                                                               */
static MBOOL ISP_BufWrite_Add(
    MUINT32     Size,
    //                  
    ISP_REG_STRUCT* pData)
{
    MUINT32 i;
    //
    //                
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        if(IspInfo.BufInfo.Write[i].Status == ISP_BUF_STATUS_HOLD)
        {
            if((IspInfo.BufInfo.Write[i].Size+Size) > ISP_BUF_SIZE_WRITE)
            {
                LOG_ERR("i = %d, BufWriteSize(%d)+Size(%d) > %d",i,IspInfo.BufInfo.Write[i].Size,Size,ISP_BUF_SIZE_WRITE);
                return false;
            }
            //
            if(copy_from_user((MUINT8*)(IspInfo.BufInfo.Write[i].pData+IspInfo.BufInfo.Write[i].Size), (MUINT8*)pData, Size) != 0)
            {
                LOG_ERR("copy_from_user failed");
                return false;
            }
            //
            if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
            {
                LOG_DBG("i = %d, BufSize = %d, Size = %d",i,IspInfo.BufInfo.Write[i].Size,Size);
            }
            //
            IspInfo.BufInfo.Write[i].Size += Size;
            return true;
        }
    }
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        if(IspInfo.BufInfo.Write[i].Status == ISP_BUF_STATUS_EMPTY)
        {
            if(Size > ISP_BUF_SIZE_WRITE)
            {
                LOG_ERR("i = %d, Size(%d) > %d",i,Size,ISP_BUF_SIZE_WRITE);
                return false;
            }
            //
            if(copy_from_user((MUINT8*)(IspInfo.BufInfo.Write[i].pData), (MUINT8*)pData, Size) != 0)
            {
                LOG_ERR("copy_from_user failed");
                return false;
            }
            //
            if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
            {
                LOG_DBG("i = %d, Size = %d",i,Size);
            }
            //
            IspInfo.BufInfo.Write[i].Size = Size;
            //
            IspInfo.BufInfo.Write[i].Status = ISP_BUF_STATUS_HOLD;
            return true;
        }
    }

    //
    LOG_ERR("All write buffer are full of data!");
    return false;

}
/*                                                                              
 
                                                                               */
static void ISP_BufWrite_SetReady(void)
{
    MUINT32 i;
    //
    //                
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        if(IspInfo.BufInfo.Write[i].Status == ISP_BUF_STATUS_HOLD)
        {
            if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
            {
                LOG_DBG("i = %d, Size = %d",i,IspInfo.BufInfo.Write[i].Size);
            }
            IspInfo.BufInfo.Write[i].Status = ISP_BUF_STATUS_READY;
        }
    }
}

/*                                                                              
 
                                                                               */
static MBOOL ISP_BufWrite_Get(
    MUINT32*    pIndex,
    MUINT32*    pSize,
    MUINT8**    ppData)
{
    MUINT32 i;
    //
    //                
    //
    for(i=0; i<ISP_BUF_WRITE_AMOUNT; i++)
    {
        if(IspInfo.BufInfo.Write[i].Status == ISP_BUF_STATUS_READY)
        {
            if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
            {
                LOG_DBG("i = %d, Size = %d",i,IspInfo.BufInfo.Write[i].Size);
            }
            *pIndex = i;
            *pSize = IspInfo.BufInfo.Write[i].Size;
            *ppData = IspInfo.BufInfo.Write[i].pData;
            return true;
        }
    }
    //
    if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
    {
        LOG_DBG("No buf is ready!");
    }
    return false;
}

/*                                                                              
 
                                                                               */
static MBOOL ISP_BufWrite_Clear(MUINT32  Index)
{
    //
    //                
    //
    if(IspInfo.BufInfo.Write[Index].Status == ISP_BUF_STATUS_READY)
    {
        if(IspInfo.DebugMask & ISP_DBG_BUF_WRITE)
        {
            LOG_DBG("Index = %d, Size = %d",Index,IspInfo.BufInfo.Write[Index].Size);
        }
        IspInfo.BufInfo.Write[Index].Size = 0;
        IspInfo.BufInfo.Write[Index].Status = ISP_BUF_STATUS_EMPTY;
        return true;
    }
    else
    {
        LOG_DBG("WARNING: Index(%d) is not ready! Status = %d",Index,IspInfo.BufInfo.Write[Index].Status);
        return false;
    }
}

/*                                                                              
 
                                                                               */
static void ISP_BufWrite_WriteToHw(void)
{
    MUINT8* pBuf;
    MUINT32 Index, BufSize;
    //
    spin_lock(&(IspInfo.SpinLockHold));
    //
    LOG_DBG("- E.");
    //
    while(ISP_BufWrite_Get(&Index,&BufSize,&pBuf))
    {
        if(IspInfo.DebugMask & ISP_DBG_TASKLET)
        {
            LOG_DBG("Index = %d, BufSize = %d ", Index, BufSize);
        }
        ISP_WriteRegToHw((ISP_REG_STRUCT*)pBuf, BufSize/sizeof(ISP_REG_STRUCT));
        ISP_BufWrite_Clear(Index);
    }
    //                        
    atomic_set(&(IspInfo.HoldInfo.WriteEnable), 0);
    wake_up_interruptible(&(IspInfo.WaitQueueHead));
    //
    spin_unlock(&(IspInfo.SpinLockHold));
}


/*                                                                              
 
                                                                               */
void ISP_ScheduleWork_VD(struct work_struct *data)
{
    if(IspInfo.DebugMask & ISP_DBG_SCHEDULE_WORK)
    {
        LOG_DBG("- E.");
    }
    //
    IspInfo.TimeLog.WorkQueueVd = ISP_JiffiesToMs(jiffies);
    //
    if(IspInfo.Callback[ISP_CALLBACK_WORKQUEUE_VD].Func != NULL)
    {
        IspInfo.Callback[ISP_CALLBACK_WORKQUEUE_VD].Func();
    }
}

/*                                                                              
 
                                                                               */
void ISP_ScheduleWork_EXPDONE(struct work_struct *data)
{
    if(IspInfo.DebugMask & ISP_DBG_SCHEDULE_WORK)
    {
        LOG_DBG("- E.");
    }
    //
    IspInfo.TimeLog.WorkQueueExpdone = ISP_JiffiesToMs(jiffies);
    //
    if(IspInfo.Callback[ISP_CALLBACK_WORKQUEUE_EXPDONE].Func != NULL)
    {
        IspInfo.Callback[ISP_CALLBACK_WORKQUEUE_EXPDONE].Func();
    }
}

/*                                                                              
 
                                                                               */
void ISP_Tasklet_VD(unsigned long Param)
{
    if(IspInfo.DebugMask & ISP_DBG_TASKLET)
    {
        LOG_DBG("- E.");
    }
    //
    IspInfo.TimeLog.TaskletVd = ISP_JiffiesToMs(jiffies);
    //
    if(IspInfo.Callback[ISP_CALLBACK_TASKLET_VD].Func != NULL)
    {
        IspInfo.Callback[ISP_CALLBACK_TASKLET_VD].Func();
    }
    //
    if(IspInfo.HoldInfo.Time == ISP_HOLD_TIME_VD)
    {
        ISP_BufWrite_WriteToHw();
    }
}
DECLARE_TASKLET(IspTaskletVD, ISP_Tasklet_VD, 0);

/*                                                                              
 
                                                                               */
void ISP_Tasklet_EXPDONE(unsigned long Param)
{
    if(IspInfo.DebugMask & ISP_DBG_TASKLET)
    {
        LOG_DBG("- E.");
    }
    //
    IspInfo.TimeLog.TaskletExpdone = ISP_JiffiesToMs(jiffies);
    //
    if(IspInfo.Callback[ISP_CALLBACK_TASKLET_EXPDONE].Func != NULL)
    {
        IspInfo.Callback[ISP_CALLBACK_TASKLET_EXPDONE].Func();
    }
    //
    if(IspInfo.HoldInfo.Time == ISP_HOLD_TIME_EXPDONE)
    {
        ISP_BufWrite_WriteToHw();
    }
}
DECLARE_TASKLET(IspTaskletEXPDONE, ISP_Tasklet_EXPDONE, 0);


/*                                                                              
 
                                                                               */
static MINT32 ISP_WriteReg(ISP_REG_IO_STRUCT*   pRegIo)
{
    MINT32 Ret = 0;
    MINT32 TimeVd = 0;
    MINT32 TimeExpdone = 0;
    MINT32 TimeTasklet = 0;
    //                     
    ISP_REG_STRUCT* pData = NULL;
    //
    if(IspInfo.DebugMask & ISP_DBG_WRITE_REG)
    {
        //                                                                                       
        LOG_DBG("Data(0x%p), Count(%d)", (pRegIo->pData), (pRegIo->Count));
    }
    //
    if(atomic_read(&(IspInfo.HoldInfo.HoldEnable)))
    {
        //                                                                                     
        if(ISP_BufWrite_Add((pRegIo->Count)*sizeof(ISP_REG_STRUCT), pRegIo->pData))
        {
            //                               
        }
        else
        {
            LOG_ERR("Add write buffer fail");
            TimeVd = ISP_JiffiesToMs(jiffies)-IspInfo.TimeLog.Vd;
            TimeExpdone = ISP_JiffiesToMs(jiffies)-IspInfo.TimeLog.Expdone;
            TimeTasklet = ISP_JiffiesToMs(jiffies)-IspInfo.TimeLog.TaskletExpdone;
            LOG_ERR("HoldTime(%d), VD(%d ms), Expdone(%d ms), Tasklet(%d ms)",IspInfo.HoldInfo.Time,TimeVd,TimeExpdone,TimeTasklet);
            ISP_BufWrite_Dump();
            ISP_DumpReg();
            Ret = -EFAULT;
            goto EXIT;
        }
    }
    else
    {
        //                                                                             
        pData = (ISP_REG_STRUCT*)kmalloc((pRegIo->Count)*sizeof(ISP_REG_STRUCT), GFP_ATOMIC);
        if(pData == NULL)
        {
            LOG_DBG("ERROR: kmalloc failed, (process, pid, tgid)=(%s, %d, %d)", current->comm, current->pid, current->tgid);
            Ret = -ENOMEM;
        }
        //
        if(copy_from_user(pData, (void __user*)(pRegIo->pData), pRegIo->Count*sizeof(ISP_REG_STRUCT)) != 0)
        {
            LOG_ERR("copy_from_user failed");
            Ret = -EFAULT;
            goto EXIT;
        }
        //
        Ret = ISP_WriteRegToHw(
                //                       
                pData,
                pRegIo->Count);
    }
    //
    EXIT:
    if(pData != NULL)
    {
        kfree(pData);
        pData = NULL;
    }
    return Ret;
}

/*                                                                              
 
                                                                               */
static MINT32 ISP_SetHoldTime(ISP_HOLD_TIME_ENUM HoldTime)
{
    LOG_DBG("HoldTime(%d)", HoldTime);
    IspInfo.HoldInfo.Time = HoldTime;
    //
    return 0;
}

/*                                                                              
 
                                                                               */
static MINT32 ISP_ResetBuf(void)
{
    LOG_DBG("- E. hold_reg(%d), BufAmount(%d)", atomic_read(&(IspInfo.HoldInfo.HoldEnable)), ISP_BufWrite_GetAmount());
    //
    ISP_BufWrite_Reset();
    atomic_set(&(IspInfo.HoldInfo.HoldEnable), 0);
    atomic_set(&(IspInfo.HoldInfo.WriteEnable), 0);
    LOG_DBG("- X.");
    return 0;
}


/*                                                                              
 
                                                                               */
static MINT32 ISP_EnableHoldReg(MBOOL En)
{
    MINT32 Ret = 0;
    MUINT32 BufAmount = 0;
    //
    if(IspInfo.DebugMask & ISP_DBG_HOLD_REG)
    {
        LOG_DBG("En(%d), HoldEnable(%d)", En, atomic_read(&(IspInfo.HoldInfo.HoldEnable)));
    }
    //
    if(!spin_trylock_bh(&(IspInfo.SpinLockHold)))
    {
        //                                 
        MINT32 Timeout;
        MINT32 IsLock = 0;
        //
        if(IspInfo.DebugMask & ISP_DBG_TASKLET)
        {
            LOG_DBG("Start wait ... ");
        }
        //
        Timeout = wait_event_interruptible_timeout(
                    IspInfo.WaitQueueHead,
                    (IsLock = spin_trylock_bh(&(IspInfo.SpinLockHold))),
                    ISP_MsToJiffies(500));
        //
        if(IspInfo.DebugMask & ISP_DBG_TASKLET)
        {
            LOG_DBG("End wait ");
        }
        //
        if(IsLock == 0)
        {
            LOG_ERR("Should not happen, Timeout & IsLock is 0");
            Ret = -EFAULT;
            goto EXIT;
        }
    }
    //                       
    if(En == MFALSE)
    {
        ISP_BufWrite_SetReady();
        BufAmount = ISP_BufWrite_GetAmount();
        //
        if(BufAmount)
        {
            atomic_set(&(IspInfo.HoldInfo.WriteEnable), 1);
        }
    }
    //
    if(IspInfo.DebugMask & ISP_DBG_HOLD_REG)
    {
        LOG_DBG("En(%d), HoldEnable(%d), BufAmount(%d)", En, atomic_read(&(IspInfo.HoldInfo.HoldEnable)),BufAmount);
    }
    //
    atomic_set(&(IspInfo.HoldInfo.HoldEnable), En);
    //
    spin_unlock_bh(&(IspInfo.SpinLockHold));
    //
    EXIT:
    return Ret;
}
/*                                                                              
 
                                                                               */
static atomic_t g_imem_ref_cnt[ISP_REF_CNT_ID_MAX];
//
//                                                
static long ISP_REF_CNT_CTRL_FUNC(unsigned long Param)
{
    MINT32 Ret = 0;
    ISP_REF_CNT_CTRL_STRUCT ref_cnt_ctrl;
    MINT32 imem_ref_cnt = 0;


    //                                  
//                                     
    //                  
    //
    if(IspInfo.DebugMask & ISP_DBG_REF_CNT_CTRL) {
        LOG_DBG("[rc]+");
    }
    //
    if (NULL == (void __user*)Param)  {
        LOG_ERR("[rc]NULL Param");
        //                                    
//                                                         
        //                  
        return -EFAULT;
    }
    //
    if(copy_from_user(&ref_cnt_ctrl, (void __user*)Param, sizeof(ISP_REF_CNT_CTRL_STRUCT)) == 0)
    {


        if(IspInfo.DebugMask & ISP_DBG_REF_CNT_CTRL) {
            LOG_DBG("[rc]ctrl(%d),id(%d)",ref_cnt_ctrl.ctrl,ref_cnt_ctrl.id);
        }
        //
        if ( ISP_REF_CNT_ID_MAX > ref_cnt_ctrl.id ) {
            //                                  
            spin_lock(&(IspInfo.SpinLockIspRef));
            //                  
            //
            switch(ref_cnt_ctrl.ctrl) {
                case ISP_REF_CNT_GET:
                    break;
                case ISP_REF_CNT_INC:
                    atomic_inc(&g_imem_ref_cnt[ref_cnt_ctrl.id]);
                    //                 
                    break;
                case ISP_REF_CNT_DEC:
                case ISP_REF_CNT_DEC_AND_RESET_P1_P2_IF_LAST_ONE:
                case ISP_REF_CNT_DEC_AND_RESET_P1_IF_LAST_ONE:
                case ISP_REF_CNT_DEC_AND_RESET_P2_IF_LAST_ONE:
                    atomic_dec(&g_imem_ref_cnt[ref_cnt_ctrl.id]);
                    //                 
                    break;
                default:
                case ISP_REF_CNT_MAX:   //                                  
                    //            
                    break;
            }
            //
            imem_ref_cnt = (MINT32)atomic_read(&g_imem_ref_cnt[ref_cnt_ctrl.id]);

            if (imem_ref_cnt == 0) {
                //                                                           
                if(ref_cnt_ctrl.ctrl==ISP_REF_CNT_DEC_AND_RESET_P1_P2_IF_LAST_ONE || ref_cnt_ctrl.ctrl==ISP_REF_CNT_DEC_AND_RESET_P1_IF_LAST_ONE) {
                    ISP_Reset(ISP_REG_SW_CTL_RST_CAM_P1);
                    LOG_DBG("Reset P1\n");
                }

                if(ref_cnt_ctrl.ctrl==ISP_REF_CNT_DEC_AND_RESET_P1_P2_IF_LAST_ONE || ref_cnt_ctrl.ctrl==ISP_REF_CNT_DEC_AND_RESET_P2_IF_LAST_ONE) {
                    ISP_Reset(ISP_REG_SW_CTL_RST_CAM_P2);
                }
            }
            //                                    
            spin_unlock(&(IspInfo.SpinLockIspRef));
            //                  

            //
            if(IspInfo.DebugMask & ISP_DBG_REF_CNT_CTRL) {
                LOG_DBG("[rc]ref_cnt(%d)",imem_ref_cnt);
            }
            //
            if(copy_to_user((void*)ref_cnt_ctrl.data_ptr, &imem_ref_cnt, sizeof(MINT32)) != 0)
            {
                LOG_ERR("[rc][GET]:copy_to_user failed");
                Ret = -EFAULT;
            }
        }
        else {
            LOG_ERR("[rc]:id(%d) exceed",ref_cnt_ctrl.id);
            Ret = -EFAULT;
        }


    }
    else
    {
        LOG_ERR("[rc]copy_from_user failed");
        Ret = -EFAULT;
    }

    //
    if(IspInfo.DebugMask & ISP_DBG_REF_CNT_CTRL) {
        LOG_DBG("[rc]-");
    }
    //
    //                                    
//                                                     
    //                  
    return Ret;
}
/*                                                                              
 
                                                                               */
//       
//
#ifndef _rtbc_use_cq0c_
static MUINT32  bEnqBuf = 0;
static MUINT32  bDeqBuf = 0;
static MINT32 rtbc_enq_dma = _rt_dma_max_;
static MINT32 rtbc_deq_dma = _rt_dma_max_;
#endif

static MUINT32 prv_tstamp_s[_rt_dma_max_] = {0};
static MUINT32 prv_tstamp_us[_rt_dma_max_] = {0};

static volatile MUINT32 sof_count[_ChannelMax] = {0,0,0,0};
static MUINT32 start_time[_ChannelMax] = {0,0,0,0};
static MUINT32 avg_frame_time[_ChannelMax] = {0,0,0,0};


static volatile int sof_pass1done[2] = {0,0};                    //                                                                                         
static volatile MUINT32 gSof_camsvdone[2] = {0,0};               //                                                                                         

static volatile MBOOL g1stSof[4] = {MTRUE,MTRUE};
#ifdef _rtbc_buf_que_2_0_
typedef struct _FW_RCNT_CTRL {
    MUINT32 INC[_IRQ_MAX][ISP_RT_BUF_SIZE];//       
    MUINT32 DMA_IDX[_rt_dma_max_];//         
    MUINT32 rdIdx[_IRQ_MAX];     //              
    MUINT32 curIdx[_IRQ_MAX];    //                      
    MUINT32 bLoadBaseAddr[_IRQ_MAX];
}FW_RCNT_CTRL;
static volatile FW_RCNT_CTRL mFwRcnt = {{{0}},{0},{0},{0},{0}};
static MUINT8 dma_en_recorder[_rt_dma_max_][ISP_RT_BUF_SIZE] = {{0}};
#endif
//
static MINT32 ISP_RTBC_ENQUE(MINT32 dma,ISP_RT_BUF_INFO_STRUCT* prt_buf_info)
{
    MINT32 Ret = 0;
    MINT32 rt_dma = dma;
    MUINT32 buffer_exist = 0;
    MUINT32 i = 0;
    MUINT32 index = 0;

    //         
    if ( ISP_RT_BUF_SIZE == pstRTBuf->ring_buf[rt_dma].total_count ) {
        LOG_ERR("[rtbc][ENQUE]:real time buffer number FULL:rt_dma(%d)",rt_dma);
        Ret = -EFAULT;
        //      
    }

    //
    //                                                            
    //                     
    for (i=0;i<ISP_RT_BUF_SIZE;i++) {
        if ( pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr == prt_buf_info->base_pAddr ) {
            buffer_exist = 1;
            break;
        }
        //
        if ( pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr == 0 ) {
            break;
        }
    }
    //
    if (buffer_exist) {
        //
        if ( ISP_RTBC_BUF_EMPTY != pstRTBuf->ring_buf[rt_dma].data[i].bFilled ) {
            pstRTBuf->ring_buf[rt_dma].data[i].bFilled    = ISP_RTBC_BUF_EMPTY;
            pstRTBuf->ring_buf[rt_dma].empty_count++;
            index = i;
        }
        //
        //                                          
            LOG_DBG("[rtbc][ENQUE]::buffer_exist(%d)/i(%d)/PA(0x%x)/bFilled(%d)/empty(%d)",
                buffer_exist,
                i,
                prt_buf_info->base_pAddr,
                pstRTBuf->ring_buf[rt_dma].data[i].bFilled, \
                pstRTBuf->ring_buf[rt_dma].empty_count);
        // 

    }
    else {
        //                                          
        if ( pstRTBuf->ring_buf[rt_dma].total_count == ISP_RT_BUF_SIZE) {
            LOG_ERR("[ENQUE]:[rtbc]:buffer full(%d)",pstRTBuf->ring_buf[rt_dma].total_count);
        }
        else {
            //              
            index = pstRTBuf->ring_buf[rt_dma].total_count % ISP_RT_BUF_SIZE;
            //
            pstRTBuf->ring_buf[rt_dma].data[index].memID      = prt_buf_info->memID;
            pstRTBuf->ring_buf[rt_dma].data[index].size       = prt_buf_info->size;
            pstRTBuf->ring_buf[rt_dma].data[index].base_vAddr = prt_buf_info->base_vAddr;
            pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr = prt_buf_info->base_pAddr;
            pstRTBuf->ring_buf[rt_dma].data[index].bFilled    = ISP_RTBC_BUF_EMPTY;
            pstRTBuf->ring_buf[rt_dma].data[index].bufIdx     = prt_buf_info->bufIdx;
            //
            pstRTBuf->ring_buf[rt_dma].total_count++;
            pstRTBuf->ring_buf[rt_dma].empty_count++;
            //
            //                                          
                LOG_DBG("[rtbc][ENQUE]:dma(%d),index(%d),bufIdx(0x%x),PA(0x%x)/empty(%d)/total(%d)",
                    rt_dma, \
                    index, \
                    prt_buf_info->bufIdx,\
                    pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr, \
                    pstRTBuf->ring_buf[rt_dma].empty_count, \
                    pstRTBuf->ring_buf[rt_dma].total_count);
            // 
        }
    }
    //

    //                                                    
    if ( 1 == pstRTBuf->ring_buf[rt_dma].empty_count) {
        if ( _imgo_ == rt_dma ) {
            //                                       
            ISP_WR32(ISP_REG_ADDR_IMGO_BASE_ADDR,pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr);
        }
        else if ( _rrzo_ == rt_dma ) {
            //                                       
            ISP_WR32(ISP_REG_ADDR_RRZO_BASE_ADDR,pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr);
        }
        else if ( _imgo_d_ == rt_dma ) {
            //                                       
            ISP_WR32(ISP_REG_ADDR_IMGO_D_BASE_ADDR,pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr);
        }
        else if ( _rrzo_d_ == rt_dma ) {
            //                                       
            ISP_WR32(ISP_REG_ADDR_RRZO_D_BASE_ADDR,pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr);
        }
        else if (_camsv_imgo_ == rt_dma) {
            ISP_WR32(ISP_REG_ADDR_IMGO_SV_BASE_ADDR,pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr);
        }
        else if (_camsv2_imgo_ == rt_dma) {
            ISP_WR32(ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR,pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr);
        }

        //                                          
            LOG_DBG("[rtbc][ENQUE]:dma(%d),base_pAddr(0x%x)/imgo(0x%x)/rrzo(0x%x)/imgo_d(0x%x)/rrzo_d(0x%x)/empty_count(%d) ",
                rt_dma, \
                pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr,\
                ISP_RD32(ISP_REG_ADDR_IMGO_BASE_ADDR), \
                ISP_RD32(ISP_REG_ADDR_RRZO_BASE_ADDR), \
                ISP_RD32(ISP_REG_ADDR_IMGO_D_BASE_ADDR), \
                ISP_RD32(ISP_REG_ADDR_RRZO_D_BASE_ADDR), \
                pstRTBuf->ring_buf[rt_dma].empty_count,\
                ISP_RD32(ISP_REG_ADDR_IMGO_SV_BASE_ADDR), \
                ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR));

        // 

#if defined(_rtbc_use_cq0c_)
    //          
#else
        MUINT32 reg_val = 0;

        //                                     
        if (_imgo_ == rt_dma) {
            reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_FBC);
            reg_val &= ~0x4000;
            ISP_WR32(ISP_REG_ADDR_IMGO_FBC,reg_val);
        }
        else {
            reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_D_FBC);
            reg_val &= ~0x4000;
            ISP_WR32(ISP_REG_ADDR_IMGO_D_FBC,reg_val);
        }
        if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
            LOG_DBG("[rtbc][ENQUE]:dma(%d),disable fbc:IMGO(0x%x),IMG2O(0x%x)",rt_dma,ISP_RD32(ISP_REG_ADDR_IMGO_FBC),ISP_RD32(ISP_REG_ADDR_IMGO_D_FBC));
        }
#endif
        pstRTBuf->ring_buf[rt_dma].pre_empty_count = pstRTBuf->ring_buf[rt_dma].empty_count;

    }

    //
    //                                                                 
    //
    //                                          
        LOG_DBG("[rtbc][ENQUE]:dma:(%d),start(%d),index(%d),empty_count(%d),base_pAddr(0x%x)", \
            rt_dma,
            pstRTBuf->ring_buf[rt_dma].start,
            index,
            pstRTBuf->ring_buf[rt_dma].empty_count,
            pstRTBuf->ring_buf[rt_dma].data[index].base_pAddr);
    // 
    //
    return Ret;
}
static void ISP_FBC_DUMP(MUINT32 dma_id,MUINT32 VF_1,MUINT32 VF_2,MUINT32 VF_3,MUINT32 VF_4){
    MUINT32 z;
    char str[128];
    char str2[_rt_dma_max_];
    MUINT32 dma;
    LOG_INF("================================\n");
    LOG_INF("pass1 timeout log(timeout port:%d)",dma_id);
    LOG_INF("================================\n");
    str[0] = '\0';
    LOG_INF("current activated dmaport");
    for(z=0;z<_rt_dma_max_;z++){
        sprintf(str2,"%d_",pstRTBuf->ring_buf[z].active);
        strcat(str,str2);
    }
    LOG_INF("%s",str);
    LOG_INF("================================\n");
    if(VF_1){
        LOG_INF("imgo:");
        dma = _imgo_;
        {
            str[0] = '\0';
            LOG_INF("current fillled buffer(buf cnt):\n",pstRTBuf->ring_buf[dma].total_count);
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",pstRTBuf->ring_buf[dma].data[z].bFilled);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("cur_start_idx:%d",pstRTBuf->ring_buf[dma].start);
            LOG_INF("cur_read_idx=%d",pstRTBuf->ring_buf[dma].read_idx);
            LOG_INF("cur_empty_cnt:%d",pstRTBuf->ring_buf[dma].empty_count);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:cur dma_en_recorder\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",dma_en_recorder[dma][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:inc record\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",mFwRcnt.INC[_IRQ][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("RCNT_RECORD: dma idx = %d\n",mFwRcnt.DMA_IDX[dma]);
            LOG_INF("RCNT_RECORD: read idx = %d\n",mFwRcnt.rdIdx[_IRQ]);
            LOG_INF("RCNT_RECORD: cur idx = %d\n",mFwRcnt.curIdx[_IRQ]);
            LOG_INF("RCNT_RECORD: bLoad = %d\n",mFwRcnt.bLoadBaseAddr[_IRQ]);
            LOG_INF("================================\n");
        }
        LOG_INF("rrzo:");
        dma = _rrzo_;
        {
            str[0] = '\0';
            LOG_INF("current fillled buffer(buf cnt):\n",pstRTBuf->ring_buf[dma].total_count);
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",pstRTBuf->ring_buf[dma].data[z].bFilled);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("cur_start_idx:%d",pstRTBuf->ring_buf[dma].start);
            LOG_INF("cur_read_idx=%d",pstRTBuf->ring_buf[dma].read_idx);
            LOG_INF("cur_empty_cnt:%d",pstRTBuf->ring_buf[dma].empty_count);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:cur dma_en_recorder\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",dma_en_recorder[dma][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:inc record\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",mFwRcnt.INC[_IRQ][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("RCNT_RECORD: dma idx = %d\n",mFwRcnt.DMA_IDX[dma]);
            LOG_INF("RCNT_RECORD: read idx = %d\n",mFwRcnt.rdIdx[_IRQ]);
            LOG_INF("RCNT_RECORD: cur idx = %d\n",mFwRcnt.curIdx[_IRQ]);
            LOG_INF("RCNT_RECORD: bLoad = %d\n",mFwRcnt.bLoadBaseAddr[_IRQ]);
            LOG_INF("================================\n");
        }
    }
    if(VF_2){
        LOG_INF("imgo_d:");
        dma = _imgo_d_;
        {
            str[0] = '\0';
            LOG_INF("current fillled buffer(buf cnt):\n",pstRTBuf->ring_buf[dma].total_count);
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",pstRTBuf->ring_buf[dma].data[z].bFilled);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("cur_start_idx:%d",pstRTBuf->ring_buf[dma].start);
            LOG_INF("cur_read_idx=%d",pstRTBuf->ring_buf[dma].read_idx);
            LOG_INF("cur_empty_cnt:%d",pstRTBuf->ring_buf[dma].empty_count);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:cur dma_en_recorder\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",dma_en_recorder[dma][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:inc record\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",mFwRcnt.INC[_IRQ_D][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("RCNT_RECORD: dma idx = %d\n",mFwRcnt.DMA_IDX[dma]);
            LOG_INF("RCNT_RECORD: read idx = %d\n",mFwRcnt.rdIdx[_IRQ_D]);
            LOG_INF("RCNT_RECORD: cur idx = %d\n",mFwRcnt.curIdx[_IRQ_D]);
            LOG_INF("RCNT_RECORD: bLoad = %d\n",mFwRcnt.bLoadBaseAddr[_IRQ_D]);
            LOG_INF("================================\n");
        }
        LOG_INF("rrzo_d:");
        dma = _rrzo_d_;
        {
            str[0] = '\0';
            LOG_INF("current fillled buffer(buf cnt):\n",pstRTBuf->ring_buf[dma].total_count);
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",pstRTBuf->ring_buf[dma].data[z].bFilled);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("cur_start_idx:%d",pstRTBuf->ring_buf[dma].start);
            LOG_INF("cur_read_idx=%d",pstRTBuf->ring_buf[dma].read_idx);
            LOG_INF("cur_empty_cnt:%d",pstRTBuf->ring_buf[dma].empty_count);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:cur dma_en_recorder\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",dma_en_recorder[dma][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:inc record\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",mFwRcnt.INC[_IRQ_D][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("RCNT_RECORD: dma idx = %d\n",mFwRcnt.DMA_IDX[dma]);
            LOG_INF("RCNT_RECORD: read idx = %d\n",mFwRcnt.rdIdx[_IRQ_D]);
            LOG_INF("RCNT_RECORD: cur idx = %d\n",mFwRcnt.curIdx[_IRQ_D]);
            LOG_INF("RCNT_RECORD: bLoad = %d\n",mFwRcnt.bLoadBaseAddr[_IRQ_D]);
            LOG_INF("================================\n");
        }
    }
    if(VF_3){
        LOG_INF("camsv_imgo:");
        dma = _camsv_imgo_;
        {
            str[0] = '\0';
            LOG_INF("current fillled buffer(buf cnt):\n",pstRTBuf->ring_buf[dma].total_count);
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",pstRTBuf->ring_buf[dma].data[z].bFilled);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("cur_start_idx:%d",pstRTBuf->ring_buf[dma].start);
            LOG_INF("cur_read_idx=%d",pstRTBuf->ring_buf[dma].read_idx);
            LOG_INF("cur_empty_cnt:%d",pstRTBuf->ring_buf[dma].empty_count);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:cur dma_en_recorder\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",dma_en_recorder[dma][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:inc record\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",mFwRcnt.INC[_CAMSV_IRQ][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("RCNT_RECORD: dma idx = %d\n",mFwRcnt.DMA_IDX[dma]);
            LOG_INF("RCNT_RECORD: read idx = %d\n",mFwRcnt.rdIdx[_CAMSV_IRQ]);
            LOG_INF("RCNT_RECORD: cur idx = %d\n",mFwRcnt.curIdx[_CAMSV_IRQ]);
            LOG_INF("RCNT_RECORD: bLoad = %d\n",mFwRcnt.bLoadBaseAddr[_CAMSV_IRQ]);
            LOG_INF("================================\n");
        }
    }
    if(VF_4){
        LOG_INF("camsv2_imgo:");
        dma = _camsv2_imgo_;
        {
            str[0] = '\0';
            LOG_INF("current fillled buffer(buf cnt):\n",pstRTBuf->ring_buf[dma].total_count);
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",pstRTBuf->ring_buf[dma].data[z].bFilled);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("cur_start_idx:%d",pstRTBuf->ring_buf[dma].start);
            LOG_INF("cur_read_idx=%d",pstRTBuf->ring_buf[dma].read_idx);
            LOG_INF("cur_empty_cnt:%d",pstRTBuf->ring_buf[dma].empty_count);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:cur dma_en_recorder\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",dma_en_recorder[dma][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("================================\n");
            LOG_INF("RCNT_RECORD:inc record\n");
            str[0] = '\0';
            for(z=0;z<ISP_RT_BUF_SIZE;z++){
                sprintf(str2,"%d_",mFwRcnt.INC[_CAMSV_D_IRQ][z]);
                strcat(str,str2);
            }
            LOG_INF("%s",str);
            LOG_INF("RCNT_RECORD: dma idx = %d\n",mFwRcnt.DMA_IDX[dma]);
            LOG_INF("RCNT_RECORD: read idx = %d\n",mFwRcnt.rdIdx[_CAMSV_D_IRQ]);
            LOG_INF("RCNT_RECORD: cur idx = %d\n",mFwRcnt.curIdx[_CAMSV_D_IRQ]);
            LOG_INF("RCNT_RECORD: bLoad = %d\n",mFwRcnt.bLoadBaseAddr[_CAMSV_D_IRQ]);
            LOG_INF("================================\n");
        }
    }
}
static MINT32 ISP_RTBC_DEQUE(MINT32 dma,ISP_DEQUE_BUF_INFO_STRUCT*    pdeque_buf)
{
    MINT32 Ret = 0;
    MINT32 rt_dma = dma;
    MUINT32 i=0;
    MUINT32 index = 0;

    //                                                            
    //
    if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
        LOG_DBG("[rtbc][DEQUE]+");
    }
    //
    pdeque_buf->count = 0;
    //                                    
    for ( i=0 ; i < pstRTBuf->ring_buf[rt_dma].total_count ; i++ ) {
        //
        index = ( pstRTBuf->ring_buf[rt_dma].start + i ) % pstRTBuf->ring_buf[rt_dma].total_count;
        //
        if ( ISP_RTBC_BUF_FILLED == pstRTBuf->ring_buf[rt_dma].data[index].bFilled ) {
            pstRTBuf->ring_buf[rt_dma].data[index].bFilled = ISP_RTBC_BUF_LOCKED;
            pdeque_buf->count= P1_DEQUE_CNT;
            break;
        }
    }
    //
    if (0==pdeque_buf->count){
        //                   
        LOG_DBG("[rtbc][DEQUE]:dma(%d),start(%d),total(%d),empty(%d), pdeque_buf->count(%d) ", \
            rt_dma, \
            pstRTBuf->ring_buf[rt_dma].start, \
            pstRTBuf->ring_buf[rt_dma].total_count, \
            pstRTBuf->ring_buf[rt_dma].empty_count, \
            pdeque_buf->count);
        //
        for ( i=0;i<=pstRTBuf->ring_buf[rt_dma].total_count-1;i++ ) {
            LOG_DBG("[rtbc][DEQUE]Buf List:%d/%d/0x%llx/0x%x/0x%x/%d/  ", \
                i, \
                pstRTBuf->ring_buf[rt_dma].data[i].memID, \
                pstRTBuf->ring_buf[rt_dma].data[i].size, \
                pstRTBuf->ring_buf[rt_dma].data[i].base_vAddr, \
                pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr, \
                pstRTBuf->ring_buf[rt_dma].data[i].bFilled);
        }
    }
    //
    if ( pdeque_buf->count ) {
        //                
        //                                
        if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
            LOG_DBG("[rtbc][DEQUE]:rt_dma(%d)/index(%d)/empty(%d)/total(%d)", \
                rt_dma, \
                index, \
                pstRTBuf->ring_buf[rt_dma].empty_count, \
                pstRTBuf->ring_buf[rt_dma].total_count);
        }
        //
        for (i=0;i<pdeque_buf->count;i++) {
            pdeque_buf->data[i].memID         = pstRTBuf->ring_buf[rt_dma].data[index+i].memID;
            pdeque_buf->data[i].size          = pstRTBuf->ring_buf[rt_dma].data[index+i].size;
            pdeque_buf->data[i].base_vAddr    = pstRTBuf->ring_buf[rt_dma].data[index+i].base_vAddr;
            pdeque_buf->data[i].base_pAddr    = pstRTBuf->ring_buf[rt_dma].data[index+i].base_pAddr;
            pdeque_buf->data[i].timeStampS    = pstRTBuf->ring_buf[rt_dma].data[index+i].timeStampS;
            pdeque_buf->data[i].timeStampUs   = pstRTBuf->ring_buf[rt_dma].data[index+i].timeStampUs;
            //
            if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
                LOG_DBG("[rtbc][DEQUE]:index(%d)/PA(0x%x)/memID(%d)/size(0x%x)/VA(0x%llx)", \
                    index+i, \
                    pdeque_buf->data[i].base_pAddr, \
                    pdeque_buf->data[i].memID, \
                    pdeque_buf->data[i].size, \
                    pdeque_buf->data[i].base_vAddr);
            }

        }
        //
        if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
            LOG_DBG("[rtbc][DEQUE]-");
        }
        //
        //                                                                 
        //
    }
    else {
        //
        //                                                                 
        LOG_ERR("[rtbc][DEQUE]:no filled buffer");
        Ret = -EFAULT;
    }

    return Ret;
}

#ifdef _MAGIC_NUM_ERR_HANDLING_
#define _INVALID_FRM_CNT_ 0xFFFF
#define _MAX_FRM_CNT_ 0xFF

#define _UNCERTAIN_MAGIC_NUM_FLAG_ 0x40000000
#define _DUMMY_MAGIC_              0x20000000
static MUINT32 m_LastMNum[_rt_dma_max_] = {0}; //         

#endif
//                                            
static long ISP_Buf_CTRL_FUNC(unsigned long Param)
{
    MINT32 Ret = 0;
    MINT32 rt_dma;
    MUINT32 reg_val = 0;
    MUINT32 reg_val2 = 0;
    MUINT32 camsv_reg_cal[2] = {0,0};
    MUINT32 i = 0;
    MUINT32 x = 0;
    MUINT32 iBuf = 0;
    MUINT32 size = 0;
    MUINT32 bWaitBufRdy = 0;
    ISP_BUFFER_CTRL_STRUCT         rt_buf_ctrl;
    MBOOL _bFlag = MTRUE;
    //                         
    CQ_RTBC_FBC p1_fbc[_rt_dma_max_];
    //                                 
    unsigned long  p1_fbc_reg[_rt_dma_max_];
    //                                      
    unsigned long p1_dma_addr_reg[_rt_dma_max_];
    MUINT32 flags;
    ISP_RT_BUF_INFO_STRUCT       rt_buf_info;
    ISP_DEQUE_BUF_INFO_STRUCT    deque_buf;
    eISPIrq irqT=_IRQ_MAX;
    eISPIrq irqT_Lock=_IRQ_MAX;
    MBOOL CurVF_En = MFALSE;
    //
    if (NULL == pstRTBuf)  {
        LOG_ERR("[rtbc]NULL pstRTBuf");
        return -EFAULT;
    }
    //
    if(copy_from_user(&rt_buf_ctrl, (void __user*)Param, sizeof(ISP_BUFFER_CTRL_STRUCT)) == 0)
    {
        rt_dma = rt_buf_ctrl.buf_id;
        //
        //                                          
        //                                                                                 
        //                           
        //                             
        //                               
        //                                 
        // 
        //
        if ( _imgo_ == rt_dma || \
             _rrzo_ == rt_dma || \
             _imgo_d_ == rt_dma || \
             _rrzo_d_ == rt_dma || \
             _camsv_imgo_  == rt_dma || \
             _camsv2_imgo_ == rt_dma) {

#if defined(_rtbc_use_cq0c_)
           //           
#else //         
            if ( ( _camsv_imgo_  == rt_dma) || (_camsv2_imgo_ == rt_dma) )
                _bFlag = MTRUE;
            else
                _bFlag = MFALSE;
#endif
            //
            if(MTRUE == _bFlag) {
                if ( (ISP_RT_BUF_CTRL_ENQUE==rt_buf_ctrl.ctrl) || \
                     (ISP_RT_BUF_CTRL_DEQUE==rt_buf_ctrl.ctrl) || \
                     (ISP_RT_BUF_CTRL_IS_RDY==rt_buf_ctrl.ctrl)|| \
                     (ISP_RT_BUF_CTRL_ENQUE_IMD==rt_buf_ctrl.ctrl)) {
                    //
                    p1_fbc[_imgo_].Reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_FBC);
                    p1_fbc[_rrzo_].Reg_val = ISP_RD32(ISP_REG_ADDR_RRZO_FBC);
                    p1_fbc[_imgo_d_].Reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_D_FBC);
                    p1_fbc[_rrzo_d_].Reg_val = ISP_RD32(ISP_REG_ADDR_RRZO_D_FBC);

                    p1_fbc_reg[_imgo_] = ISP_REG_ADDR_IMGO_FBC;
                    p1_fbc_reg[_rrzo_] = ISP_REG_ADDR_RRZO_FBC;
                    p1_fbc_reg[_imgo_d_] = ISP_REG_ADDR_IMGO_D_FBC;
                    p1_fbc_reg[_rrzo_d_] = ISP_REG_ADDR_RRZO_D_FBC;

                    p1_dma_addr_reg[_imgo_] = ISP_REG_ADDR_IMGO_BASE_ADDR;
                    p1_dma_addr_reg[_rrzo_] = ISP_REG_ADDR_RRZO_BASE_ADDR;
                    p1_dma_addr_reg[_imgo_d_] = ISP_REG_ADDR_IMGO_D_BASE_ADDR;
                    p1_dma_addr_reg[_rrzo_d_] = ISP_REG_ADDR_RRZO_D_BASE_ADDR;


                    p1_fbc[_camsv_imgo_].Reg_val = ISP_RD32(ISP_REG_ADDR_CAMSV_IMGO_FBC);
                    p1_fbc[_camsv2_imgo_].Reg_val = ISP_RD32(ISP_REG_ADDR_CAMSV2_IMGO_FBC);

                    p1_fbc_reg[_camsv_imgo_] = ISP_REG_ADDR_CAMSV_IMGO_FBC;
                    p1_fbc_reg[_camsv2_imgo_] = ISP_REG_ADDR_CAMSV2_IMGO_FBC;

                    p1_dma_addr_reg[_camsv_imgo_] = ISP_REG_ADDR_IMGO_SV_BASE_ADDR;
                    p1_dma_addr_reg[_camsv_imgo_] = ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR;
                    //
#if 0
                    if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
                        LOG_DBG("[rtbc]:ctrl(%d),o(0x%x),zo(0x%x),camsv(0x%x/0x%x)", \
                            rt_buf_ctrl.ctrl, \
                            p1_fbc[_imgo_].Reg_val, \
                            p1_fbc[_rrzo_].Reg_val, \
                            p1_fbc[_camsv_imgo_].Reg_val,\
                            p1_fbc[_camsv2_imgo_].Reg_val);
                    }
#endif
                }
            }
        }
        else {
#ifdef _rtbc_buf_que_2_0_
            if(rt_buf_ctrl.ctrl != ISP_RT_BUF_CTRL_DMA_EN)
#endif
            {
                LOG_ERR("[rtbc]invalid dma channel(%d)",rt_dma);
                return -EFAULT;
            }
        }
        //
        switch(rt_buf_ctrl.ctrl) {
            //                                                
            case ISP_RT_BUF_CTRL_ENQUE:
            case ISP_RT_BUF_CTRL_ENQUE_IMD:
//                                                
                //
                if(copy_from_user(&rt_buf_info, (void __user*)rt_buf_ctrl.data_ptr, sizeof(ISP_RT_BUF_INFO_STRUCT)) == 0) {
                    reg_val  = ISP_RD32(ISP_REG_ADDR_TG_VF_CON);
                    reg_val2 = ISP_RD32(ISP_REG_ADDR_TG2_VF_CON);
                    camsv_reg_cal[0] = ISP_RD32(ISP_REG_ADDR_CAMSV_TG_VF_CON);
                    camsv_reg_cal[1] = ISP_RD32(ISP_REG_ADDR_CAMSV_TG2_VF_CON);
                    //                
                    //                        
                    if((_imgo_ == rt_dma)||(_rrzo_ == rt_dma)) {
                        if(reg_val & 0x1)
                            CurVF_En = MTRUE;
                        else
                            CurVF_En = MFALSE;
                    }else if((_imgo_d_ == rt_dma)||(_rrzo_d_ == rt_dma)) {
                        if(reg_val2 & 0x1)
                            CurVF_En = MTRUE;
                        else
                            CurVF_En = MFALSE;
                    }else if(_camsv_imgo_ == rt_dma) {
                        if(camsv_reg_cal[0] & 0x1)
                            CurVF_En = MTRUE;
                        else
                            CurVF_En = MFALSE;
                    }else if(_camsv2_imgo_ == rt_dma) {
                        if(camsv_reg_cal[1] & 0x1)
                            CurVF_En = MTRUE;
                        else
                            CurVF_En = MFALSE;
                    }

                    if ( CurVF_En ) {
                        if(_bFlag == MTRUE) {
                            MUINT32 ch_imgo=0,ch_rrzo=0;
                            //
                            switch(rt_dma) {
                                case _imgo_:
                                case _rrzo_:
                                    irqT = _IRQ;
                                    ch_imgo = _imgo_;
                                    ch_rrzo = _rrzo_;
                                    irqT_Lock = _IRQ;
                                    break;
                                case _imgo_d_:
                                case _rrzo_d_:
                                    irqT = _IRQ_D;
                                    ch_imgo = _imgo_d_;
                                    ch_rrzo = _rrzo_d_;
                                    irqT_Lock = _IRQ;
                                    break;
                                case _camsv_imgo_:
                                    irqT_Lock = _CAMSV_IRQ;
                                    irqT = _CAMSV_IRQ;
                                    break;
                                case _camsv2_imgo_:
                                    irqT_Lock = _CAMSV_D_IRQ;
                                    irqT = _CAMSV_D_IRQ;
                                    break;
                                default:
                                    irqT_Lock = _IRQ;
                                    irqT = _IRQ;
                                    LOG_ERR("[rtbc]N.S.(%d)\n",rt_dma);
                                    break;
                            }
#if 0
                            static MUINT32 RTBC_DBG_test=0;
                            if(RTBC_DBG_test++ > 3){
                                RTBC_DBG_test -=3;
                                ISP_FBC_DUMP(rt_dma,1,0,0,0);
                            }
#endif
                            spin_lock_irqsave(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);
                            if ( 0 != rt_buf_ctrl.ex_data_ptr ) {
                                //                                                                                   
                                if(copy_from_user(&deque_buf.data[0], (void __user*)rt_buf_ctrl.ex_data_ptr, sizeof(ISP_RT_BUF_INFO_STRUCT)) == 0) {
                                    //
                                    i = 0;
                                    if (deque_buf.data[0].bufIdx != 0xFFFF){
                                        //                                                
                                        //                                         
                                        for (i=0;i<ISP_RT_BUF_SIZE;i++) {
                                           if ( pstRTBuf->ring_buf[rt_dma].data[i].bufIdx == deque_buf.data[0].bufIdx ) {
                                               break;
                                           }
                                        }
                                    } else {
                                        //
                                        //                                           
                                        for (i=0;i<ISP_RT_BUF_SIZE;i++) {
                                            if ( pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr == rt_buf_info.base_pAddr ) {
                                              //                                                   
                                              break;
                                            }
                                        }
                                    }

                                    if (i == ISP_RT_BUF_SIZE) {
                                        //                                 
                                        LOG_ERR("[rtbc][replace2]error Can't get the idx-(0x%x)/Addr(0x%x) buf\n",deque_buf.data[0].bufIdx, rt_buf_info.base_pAddr);
                                        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);
                                        IRQ_LOG_PRINTER(irqT,0,_LOG_DBG);

                                        for (i=0;i<ISP_RT_BUF_SIZE;i+=4) {
                                            LOG_ERR("[rtbc][replace2]error idx-(0x%x/0x%x/0x%x/0x%x)\n",\
                                              pstRTBuf->ring_buf[rt_dma].data[i+0].bufIdx,\
                                              pstRTBuf->ring_buf[rt_dma].data[i+1].bufIdx,\
                                              pstRTBuf->ring_buf[rt_dma].data[i+2].bufIdx,\
                                              pstRTBuf->ring_buf[rt_dma].data[i+3].bufIdx);
                                        }
                                        return -EFAULT;
                                    }
                                    {
                                        //
                                        {
                                            //                                                                                                                                                        
                                            IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"[rtbc][replace2]dma(%d),idx(%d) PA(0x%x_0x%x)\n",rt_dma,i,pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr,deque_buf.data[0].base_pAddr);
                                            //                                                       
                                            pstRTBuf->ring_buf[rt_dma].data[i].memID= deque_buf.data[0].memID;
                                            pstRTBuf->ring_buf[rt_dma].data[i].size= deque_buf.data[0].size;
                                            pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr = deque_buf.data[0].base_pAddr;
                                            pstRTBuf->ring_buf[rt_dma].data[i].base_vAddr = deque_buf.data[0].base_vAddr;
                                            pstRTBuf->ring_buf[rt_dma].data[i].bFilled = ISP_RTBC_BUF_EMPTY;
                                            pstRTBuf->ring_buf[rt_dma].data[i].image.frm_cnt = _INVALID_FRM_CNT_;

#ifdef _rtbc_buf_que_2_0_
                                            if(pstRTBuf->ring_buf[rt_dma].empty_count<pstRTBuf->ring_buf[rt_dma].total_count)
                                                pstRTBuf->ring_buf[rt_dma].empty_count++;
                                            else{
                                                spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);
                                                IRQ_LOG_PRINTER(irqT,0,_LOG_DBG);
                                                LOG_ERR("[rtbc]dma(%d),PA(0x%x),over enque_1",rt_dma,pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr);
                                                return -EFAULT;
                                            }
                                            //                                                                                                                                                                                 
#else
                                            pstRTBuf->ring_buf[rt_dma].empty_count++;
#endif
                                            //                                                            
                                        }
                                    }
                                }
                                else
                                {
                                    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);
                                    LOG_ERR("[rtbc][ENQUE_ext]:copy_from_user fail, dst_buf(0x%x), user_buf(0x%x)",&deque_buf.data[0],rt_buf_ctrl.ex_data_ptr);
                                    return -EAGAIN;
                                }
                            }
                            else {//                                   
                                for (i=0;i<ISP_RT_BUF_SIZE;i++) {
                                    //
                                    if ( pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr == rt_buf_info.base_pAddr ) {
                                        //                                                                                                                                                        
                                        //                                                       
                                        pstRTBuf->ring_buf[rt_dma].data[i].bFilled = ISP_RTBC_BUF_EMPTY;
                                        pstRTBuf->ring_buf[rt_dma].data[i].image.frm_cnt = _INVALID_FRM_CNT_;
#ifdef _rtbc_buf_que_2_0_
                                        if(pstRTBuf->ring_buf[rt_dma].empty_count<pstRTBuf->ring_buf[rt_dma].total_count)
                                            pstRTBuf->ring_buf[rt_dma].empty_count++;
                                        else{
                                            spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);
                                            IRQ_LOG_PRINTER(irqT,0,_LOG_DBG);
                                            LOG_ERR("[rtbc]error:dma(%d),PA(0x%x),over enque_2",rt_dma,pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr);
                                            return -EFAULT;
                                        }

                                        //            
                                        if (1) {
                                            if ( rt_buf_info.bufIdx != pstRTBuf->ring_buf[rt_dma].data[i].bufIdx) {
                                                LOG_ERR("[rtbc][replace2]error: BufIdx MisMatch. 0x%x/0x%x", rt_buf_info.bufIdx, pstRTBuf->ring_buf[rt_dma].data[i].bufIdx);
                                            }
                                        }

#else
                                        pstRTBuf->ring_buf[rt_dma].empty_count++;
#endif
                                        //                                                            
                                        //                                                                                                                                                        
                                        break;
                                    }
                                }
                                if (i == ISP_RT_BUF_SIZE) {

                                    for (x=0;x<ISP_RT_BUF_SIZE;x++) {
                                        LOG_DBG("[rtbc]dma(%d),idx(%d) PA(0x%x) VA(0x%llx)",rt_dma,x,pstRTBuf->ring_buf[rt_dma].data[x].base_pAddr,pstRTBuf->ring_buf[rt_dma].data[x].base_vAddr);
                                    }
                                    LOG_ERR("[rtbc][replace3]can't find thespecified Addr(0x%x)\n", rt_buf_info.base_pAddr);
                                }
                            }
                            //                
                            //      
                            //         


                            //          
                            {
#ifdef _rtbc_buf_que_2_0_
                                MUINT32 z;
                                if(rt_buf_ctrl.ctrl == ISP_RT_BUF_CTRL_ENQUE){
                                    //                                                    
                                    if((_IRQ == irqT)|| (_IRQ_D == irqT) ) {
                                        if( (MTRUE == pstRTBuf->ring_buf[ch_imgo].active) && (MTRUE == pstRTBuf->ring_buf[ch_rrzo].active) ){
                                            if ( 0 != rt_buf_ctrl.ex_data_ptr ){
                                                if ( (p1_fbc[rt_dma].Bits.FB_NUM == p1_fbc[rt_dma].Bits.FBC_CNT) ||
                                                     ((p1_fbc[rt_dma].Bits.FB_NUM-1) == p1_fbc[rt_dma].Bits.FBC_CNT)) {
                                                    mFwRcnt.bLoadBaseAddr[irqT] = MTRUE;
                                                }
                                            }
                                            dma_en_recorder[rt_dma][mFwRcnt.DMA_IDX[rt_dma]] = MTRUE;
                                            mFwRcnt.DMA_IDX[rt_dma] = (++mFwRcnt.DMA_IDX[rt_dma] >= ISP_RT_BUF_SIZE)?(mFwRcnt.DMA_IDX[rt_dma] - ISP_RT_BUF_SIZE):(mFwRcnt.DMA_IDX[rt_dma]);
                                            //                                                                                                                
                                            for(z=0;z<ISP_RT_BUF_SIZE;z++) {
                                                if(dma_en_recorder[ch_imgo][mFwRcnt.rdIdx[irqT]] && dma_en_recorder[ch_rrzo][mFwRcnt.rdIdx[irqT]]) {
                                                    mFwRcnt.INC[irqT][mFwRcnt.curIdx[irqT]++] = 1;
                                                    dma_en_recorder[ch_imgo][mFwRcnt.rdIdx[irqT]] = dma_en_recorder[ch_rrzo][mFwRcnt.rdIdx[irqT]] = MFALSE;
                                                    mFwRcnt.rdIdx[irqT] = (++mFwRcnt.rdIdx[irqT] >= ISP_RT_BUF_SIZE)?(mFwRcnt.rdIdx[irqT] - ISP_RT_BUF_SIZE):(mFwRcnt.rdIdx[irqT]);
                                                    //                                                                      
                                                } else {
                                                    break;
                                                }
                                            }
                                        }
                                        else {
                                            //                                                                                                          
                                            //                             
                                            if(MTRUE == pstRTBuf->ring_buf[rt_dma].active) {
                                                if ( 0 != rt_buf_ctrl.ex_data_ptr ){
                                                    if ( (p1_fbc[rt_dma].Bits.FB_NUM == p1_fbc[rt_dma].Bits.FBC_CNT) ||
                                                         ((p1_fbc[rt_dma].Bits.FB_NUM-1) == p1_fbc[rt_dma].Bits.FBC_CNT)) {
                                                        mFwRcnt.bLoadBaseAddr[irqT] = MTRUE;
                                                    }
                                                }
                                                dma_en_recorder[rt_dma][mFwRcnt.DMA_IDX[rt_dma]] = MTRUE;
                                                mFwRcnt.DMA_IDX[rt_dma] = (++mFwRcnt.DMA_IDX[rt_dma] >= ISP_RT_BUF_SIZE)?(mFwRcnt.DMA_IDX[rt_dma] - ISP_RT_BUF_SIZE):(mFwRcnt.DMA_IDX[rt_dma]);

                                                for(z=0;z<ISP_RT_BUF_SIZE;z++) {
                                                    if(dma_en_recorder[rt_dma][mFwRcnt.rdIdx[irqT]]) {
                                                        mFwRcnt.INC[irqT][mFwRcnt.curIdx[irqT]++] = 1;
                                                        dma_en_recorder[rt_dma][mFwRcnt.rdIdx[irqT]] = MFALSE;
                                                        mFwRcnt.rdIdx[irqT] = (++mFwRcnt.rdIdx[irqT] >= ISP_RT_BUF_SIZE)?(mFwRcnt.rdIdx[irqT] - ISP_RT_BUF_SIZE):(mFwRcnt.rdIdx[irqT]);
                                                    } else {
                                                        break;
                                                    }
                                                }
                                            }
                                            else {
                                                LOG_ERR("[rtbc]error:dma(%d) r not being activated(%d)",rt_dma,pstRTBuf->ring_buf[rt_dma].active);
                                                return -EFAULT;
                                            }
                                        }
                                    }
                                    else  {//          
                                        if(MTRUE == pstRTBuf->ring_buf[rt_dma].active) {
                                            if ( 0 != rt_buf_ctrl.ex_data_ptr ){
                                                if ( (p1_fbc[rt_dma].Bits.FB_NUM == p1_fbc[rt_dma].Bits.FBC_CNT) ||
                                                     ((p1_fbc[rt_dma].Bits.FB_NUM-1) == p1_fbc[rt_dma].Bits.FBC_CNT)) {
                                                    mFwRcnt.bLoadBaseAddr[irqT] = MTRUE;
                                                }
                                            }
                                            dma_en_recorder[rt_dma][mFwRcnt.DMA_IDX[rt_dma]] = MTRUE;
                                            mFwRcnt.DMA_IDX[rt_dma] = (++mFwRcnt.DMA_IDX[rt_dma] >= ISP_RT_BUF_SIZE)?(mFwRcnt.DMA_IDX[rt_dma] - ISP_RT_BUF_SIZE):(mFwRcnt.DMA_IDX[rt_dma]);

                                            for(z=0;z<ISP_RT_BUF_SIZE;z++) {
                                                if(dma_en_recorder[rt_dma][mFwRcnt.rdIdx[irqT]]) {
                                                    mFwRcnt.INC[irqT][mFwRcnt.curIdx[irqT]++] = 1;
                                                    dma_en_recorder[rt_dma][mFwRcnt.rdIdx[irqT]] = MFALSE;
                                                    mFwRcnt.rdIdx[irqT] = (++mFwRcnt.rdIdx[irqT] >= ISP_RT_BUF_SIZE)?(mFwRcnt.rdIdx[irqT] - ISP_RT_BUF_SIZE):(mFwRcnt.rdIdx[irqT]);
                                                } else {
                                                    break;
                                                }
                                            }
                                        }
                                        else {
                                            LOG_ERR("[rtbc]error:dma(%d) r not being activated(%d)",rt_dma,pstRTBuf->ring_buf[rt_dma].active);
                                            return -EFAULT;
                                        }
                                    }
                                }
                                else {//                    
                                    MUINT32 _openedDma = 1;
                                    MBOOL   _bypass = MFALSE;
                                    if( (MTRUE == pstRTBuf->ring_buf[ch_imgo].active) && (MTRUE == pstRTBuf->ring_buf[ch_rrzo].active) ){
                                        //                                                 
                                        //                                                                   
                                        //                                                    
                                        dma_en_recorder[rt_dma][0] = MTRUE; //                                                                           
                                        _openedDma = 2;
                                        if( (dma_en_recorder[ch_imgo][0] == MTRUE) && (dma_en_recorder[ch_rrzo][0] == MTRUE))
                                        {
                                            dma_en_recorder[ch_imgo][0] = dma_en_recorder[ch_rrzo][0] = MFALSE;
                                        }
                                        else
                                            _bypass = MTRUE;
                                    }
                                    if(_bypass == MFALSE){
                                        if ( (p1_fbc[rt_dma].Bits.FB_NUM == p1_fbc[rt_dma].Bits.FBC_CNT) ||
                                             ((p1_fbc[rt_dma].Bits.FB_NUM-1) == p1_fbc[rt_dma].Bits.FBC_CNT)) {
                                            //                     
                                            //                                                                                                                    
                                            IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"[rtbc_%d][ENQUE] write2Phy directly(%d,%d) ",rt_dma,p1_fbc[rt_dma].Bits.FB_NUM,p1_fbc[rt_dma].Bits.FBC_CNT);
                                            ISP_WR32(p1_dma_addr_reg[rt_dma],pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr);
                                        }
                                        if( (_camsv_imgo_ == rt_dma) ||(_camsv2_imgo_ == rt_dma) ){
                                            p1_fbc[rt_dma].Bits.RCNT_INC = 1;
                                            ISP_WR32(p1_fbc_reg[rt_dma],p1_fbc[rt_dma].Reg_val);
                                            p1_fbc[rt_dma].Bits.RCNT_INC = 0;
                                            ISP_WR32(p1_fbc_reg[rt_dma],p1_fbc[rt_dma].Reg_val);
                                        } else{
                                            if(_openedDma == 1){
                                                p1_fbc[rt_dma].Bits.RCNT_INC = 1;
                                                ISP_WR32(p1_fbc_reg[rt_dma],p1_fbc[rt_dma].Reg_val);
                                                IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"  RCNT_INC(dma:0x%x)\n",rt_dma);
                                            }
                                            else{
                                                p1_fbc[ch_imgo].Bits.RCNT_INC = 1;
                                                ISP_WR32(p1_fbc_reg[ch_imgo],p1_fbc[ch_imgo].Reg_val);
                                                p1_fbc[ch_rrzo].Bits.RCNT_INC = 1;
                                                ISP_WR32(p1_fbc_reg[ch_rrzo],p1_fbc[ch_rrzo].Reg_val);
                                                IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"  RCNT_INC(dma:0x%x)\n",ch_imgo);
                                                IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"  RCNT_INC(dma:0x%x)\n",ch_rrzo);
                                            }
                                        }
                                    }
                                }
                                
#else                           //                  
                                //                                              
                                if ( (p1_fbc[rt_dma].Bits.FB_NUM == p1_fbc[rt_dma].Bits.FBC_CNT) ||
                                     ((p1_fbc[rt_dma].Bits.FB_NUM-1) == p1_fbc[rt_dma].Bits.FBC_CNT)) {
                                    //                     
                                    //                                                                                                                    
                                    IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"[rtbc_%d][ENQUE] write2Phy directly(%d,%d)\n",rt_dma,p1_fbc[rt_dma].Bits.FB_NUM,p1_fbc[rt_dma].Bits.FBC_CNT);
                                    ISP_WR32(p1_dma_addr_reg[rt_dma],pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr);
                                }

                                //                  
                                if( (_camsv_imgo_ == rt_dma) ||(_camsv2_imgo_ == rt_dma) ){
                                    p1_fbc[rt_dma].Bits.RCNT_INC = 1;
                                    ISP_WR32(p1_fbc_reg[rt_dma],p1_fbc[rt_dma].Reg_val);
                                    p1_fbc[rt_dma].Bits.RCNT_INC = 0;
                                    ISP_WR32(p1_fbc_reg[rt_dma],p1_fbc[rt_dma].Reg_val);
                                } else{
                                    p1_fbc[rt_dma].Bits.RCNT_INC = 1;
                                    ISP_WR32(p1_fbc_reg[rt_dma],p1_fbc[rt_dma].Reg_val);
                                }
#endif
                            }
                            spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);
                            IRQ_LOG_PRINTER(irqT,0,_LOG_DBG);
                            //
                            if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
                                //                                                                                                                  
                                LOG_DBG("[rtbc][ENQUE]:dma(%d),PA(0x%x),O(0x%x),ZO(0x%x),O_D(0x%x),ZO_D(0x%x),camsv(0x%x/0x%x)fps(%d/%d/%d/%d)us,rtctrl_%d\n",
                                    rt_dma, \
                                    rt_buf_info.base_pAddr, \
                                    ISP_RD32(ISP_REG_ADDR_IMGO_BASE_ADDR), \
                                    ISP_RD32(ISP_REG_ADDR_RRZO_BASE_ADDR), \
                                    ISP_RD32(ISP_REG_ADDR_IMGO_D_BASE_ADDR), \
                                    ISP_RD32(ISP_REG_ADDR_RRZO_D_BASE_ADDR), \
                                    ISP_RD32(ISP_REG_ADDR_IMGO_SV_BASE_ADDR), \
                                    ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR), \
                                    avg_frame_time[_PASS1], \
                                    avg_frame_time[_PASS1_D],\
                                    avg_frame_time[_CAMSV],\
                                    avg_frame_time[_CAMSV_D],\
                                    rt_buf_ctrl.ctrl);
                            }
                        }
                    }
                    else {
                        ISP_RTBC_ENQUE(rt_dma,&rt_buf_info);
                    }

                }
                else {
                    LOG_ERR("[rtbc][ENQUE]:copy_from_user fail");
                    return -EFAULT;
                }
                break;

            case ISP_RT_BUF_CTRL_DEQUE:
                switch(rt_dma) {
                    case _camsv_imgo_:
                        irqT_Lock = _CAMSV_IRQ;
                        irqT = _CAMSV_IRQ;
                        break;
                    case _camsv2_imgo_:
                        irqT_Lock = _CAMSV_D_IRQ;
                        irqT = _CAMSV_D_IRQ;
                        break;
                    default:
                        irqT_Lock =_IRQ;
                        irqT = _IRQ;
                        break;
                }
                //
                reg_val  = ISP_RD32(ISP_REG_ADDR_TG_VF_CON);
                reg_val2 = ISP_RD32(ISP_REG_ADDR_TG2_VF_CON);
                camsv_reg_cal[0] = ISP_RD32(ISP_REG_ADDR_CAMSV_TG_VF_CON);
                camsv_reg_cal[1] = ISP_RD32(ISP_REG_ADDR_CAMSV_TG2_VF_CON);
                //                
                //                                                            
                if ( (reg_val & 0x01) || (reg_val2 & 0x01) || (camsv_reg_cal[0] & 0x01) || (camsv_reg_cal[1] & 0x01)) {
                    if(MTRUE == _bFlag) {
                        MUINT32 out;
                        deque_buf.count = P1_DEQUE_CNT;
                        spin_lock_irqsave(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);
#ifdef _rtbc_buf_que_2_0_
                        iBuf = pstRTBuf->ring_buf[rt_dma].read_idx;//                                                  
                        pstRTBuf->ring_buf[rt_dma].read_idx = (pstRTBuf->ring_buf[rt_dma].read_idx +1 )%pstRTBuf->ring_buf[rt_dma].total_count;
                        if(deque_buf.count != P1_DEQUE_CNT){
                            LOG_ERR("support only deque 1 buf at 1 time\n");
                            deque_buf.count = P1_DEQUE_CNT;
                        }
#else
                        iBuf = p1_fbc[rt_dma].Bits.RCNT - 1;    //                  
#endif
                        i = 0;
                        if (ISP_RTBC_BUF_LOCKED == pstRTBuf->ring_buf[rt_dma].data[iBuf+i].bFilled)
                        {
                            LOG_ERR("the same buffer deque twice \n");
                        }
                        //                                 
                            //            

                            deque_buf.data[i].memID         = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].memID;
                            deque_buf.data[i].size          = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].size;
                            deque_buf.data[i].base_vAddr    = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].base_vAddr;
                            deque_buf.data[i].base_pAddr    = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].base_pAddr;
                            deque_buf.data[i].timeStampS    = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].timeStampS;
                            deque_buf.data[i].timeStampUs   = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].timeStampUs;
                            deque_buf.data[i].image.w       = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.w;
                            deque_buf.data[i].image.h       = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.h;
                            deque_buf.data[i].image.xsize   = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.xsize;
                            deque_buf.data[i].image.stride  = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.stride;
                            deque_buf.data[i].image.bus_size= pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.bus_size;
                            deque_buf.data[i].image.fmt     = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.fmt;
                            deque_buf.data[i].image.pxl_id  = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.pxl_id;
                            deque_buf.data[i].image.wbn     = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.wbn;
                            deque_buf.data[i].image.ob      = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.ob;
                            deque_buf.data[i].image.lsc     = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.lsc;
                            deque_buf.data[i].image.rpg     = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.rpg;
                            deque_buf.data[i].image.m_num_0 = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.m_num_0;
                            deque_buf.data[i].image.frm_cnt = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].image.frm_cnt;
                            deque_buf.data[i].rrzInfo.srcX = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].rrzInfo.srcX;
                            deque_buf.data[i].rrzInfo.srcY = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].rrzInfo.srcY;
                            deque_buf.data[i].rrzInfo.srcW = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].rrzInfo.srcW;
                            deque_buf.data[i].rrzInfo.srcH = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].rrzInfo.srcH;
                            deque_buf.data[i].rrzInfo.dstW = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].rrzInfo.dstW;
                            deque_buf.data[i].rrzInfo.dstH = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].rrzInfo.dstH;
                            deque_buf.data[i].dmaoCrop.x    = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].dmaoCrop.x;
                            deque_buf.data[i].dmaoCrop.y    = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].dmaoCrop.y;
                            deque_buf.data[i].dmaoCrop.w    = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].dmaoCrop.w;
                            deque_buf.data[i].dmaoCrop.h    = pstRTBuf->ring_buf[rt_dma].data[iBuf+i].dmaoCrop.h;

#ifdef _MAGIC_NUM_ERR_HANDLING_

                            /*                                                                             
                                                         
                                                                                  
                                                                      
                                                                                   
                                                                    
                                            */

                            MUINT32 _magic = deque_buf.data[i].image.m_num_0;
                            if(_DUMMY_MAGIC_ & deque_buf.data[i].image.m_num_0)
                                _magic = (deque_buf.data[i].image.m_num_0 & (~_DUMMY_MAGIC_));
                                                        

                            if ( (_INVALID_FRM_CNT_ == deque_buf.data[i].image.frm_cnt) || \
                                 (m_LastMNum[rt_dma] > _magic) ) {
                                 //
                                 if((_DUMMY_MAGIC_ & deque_buf.data[i].image.m_num_0) == 0)
                                    deque_buf.data[i].image.m_num_0 |= _UNCERTAIN_MAGIC_NUM_FLAG_;
                                //
                                IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"m# uncertain:dma(%d),m0(0x%x),fcnt(0x%x),Lm#(0x%x)", \
                                        rt_dma,
                                        deque_buf.data[i].image.m_num_0,\
                                        deque_buf.data[i].image.frm_cnt,\
                                        m_LastMNum[rt_dma]);
#ifdef T_STAMP_2_0
                                if(m_T_STAMP.fps > SlowMotion){//                                                                                                                                                                               
                                    m_T_STAMP.T_ns += ((unsigned long long)m_T_STAMP.interval_us*1000);
                                    if(++m_T_STAMP.fcnt  == m_T_STAMP.fps){
                                        m_T_STAMP.fcnt = 0;
                                        m_T_STAMP.T_ns += ((unsigned long long)m_T_STAMP.compensation_us*1000);
                                    }
                                }
#endif
                            }
                            else {
                                m_LastMNum[rt_dma] = _magic;
                            }

#endif

                            DMA_TRANS(rt_dma,out);
                            pstRTBuf->ring_buf[rt_dma].data[iBuf+i].bFilled = ISP_RTBC_BUF_LOCKED;
                            deque_buf.sof_cnt = sof_count[out];
                            deque_buf.img_cnt = pstRTBuf->ring_buf[rt_dma].img_cnt;
                            spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);
                            IRQ_LOG_PRINTER(irqT,0,_LOG_DBG);
                            //                                                                                                                                                                                 
                            if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
                                LOG_DBG("[rtbc][DEQUE](%d):d(%d)/id(0x%x)/bs(0x%x)/va(0x%llx)/pa(0x%x)/t(%d.%d)/img(%d,%d,%d,%d,%d,%d,%d,%d,%d)/m(0x%x)/fc(%d)/rrz(%d,%d,%d,%d,%d,%d),dmao(%d,%d,%d,%d),lm#(0x%x)", \
                                    iBuf+i,\
                                    rt_dma, \
                                    deque_buf.data[i].memID, \
                                    deque_buf.data[i].size, \
                                    deque_buf.data[i].base_vAddr, \
                                    deque_buf.data[i].base_pAddr, \
                                    deque_buf.data[i].timeStampS, \
                                    deque_buf.data[i].timeStampUs, \
                                    deque_buf.data[i].image.w, \
                                    deque_buf.data[i].image.h, \
                                    deque_buf.data[i].image.stride, \
                                    deque_buf.data[i].image.bus_size, \
                                    deque_buf.data[i].image.fmt, \
                                    deque_buf.data[i].image.wbn, \
                                    deque_buf.data[i].image.ob, \
                                    deque_buf.data[i].image.lsc, \
                                    deque_buf.data[i].image.rpg, \
                                    deque_buf.data[i].image.m_num_0, \
                                    deque_buf.data[i].image.frm_cnt, \
                                    deque_buf.data[i].rrzInfo.srcX, \
                                    deque_buf.data[i].rrzInfo.srcY, \
                                    deque_buf.data[i].rrzInfo.srcW, \
                                    deque_buf.data[i].rrzInfo.srcH, \
                                    deque_buf.data[i].rrzInfo.dstW,\
                                    deque_buf.data[i].rrzInfo.dstH,\
                                    deque_buf.data[i].dmaoCrop.x,\
                                    deque_buf.data[i].dmaoCrop.y,\
                                    deque_buf.data[i].dmaoCrop.w,\
                                    deque_buf.data[i].dmaoCrop.h,\
                                    m_LastMNum[rt_dma]);
                            }
                            //
                            //                                                                            
                            //                                                    
                            if ( 0 != prv_tstamp_s[rt_dma] ) {
                                if ( ( prv_tstamp_s[rt_dma] > deque_buf.data[i].timeStampS) ||
                                     ( (prv_tstamp_s[rt_dma] == deque_buf.data[i].timeStampS)&&(prv_tstamp_us[rt_dma] >= deque_buf.data[i].timeStampUs) ) ) {
                                    LOG_ERR("[rtbc]TS rollback,D(%d),prv\"%d.%06d\",cur\"%d.%06d\"",rt_dma,prv_tstamp_s[rt_dma],prv_tstamp_us[rt_dma],deque_buf.data[i].timeStampS,deque_buf.data[i].timeStampUs);
                                }
                            }
                            prv_tstamp_s[rt_dma] = deque_buf.data[i].timeStampS;
                            prv_tstamp_us[rt_dma] = deque_buf.data[i].timeStampUs;
                        //                                               
                    }
                }
                else {
                    ISP_RTBC_DEQUE(rt_dma,&deque_buf);
                }

                if ( deque_buf.count ) {
                    //
                    //                                                                                                        
                    if(copy_to_user((void __user*)rt_buf_ctrl.pExtend, &deque_buf, sizeof(ISP_DEQUE_BUF_INFO_STRUCT)) != 0)
                    {
                        LOG_ERR("[rtbc][DEQUE]:copy_to_user failed");
                        Ret = -EFAULT;
                    }

                }
                else {
                    //
                    //                                                                 
                    LOG_ERR("[rtbc][DEQUE]:no filled buffer");
                    Ret = -EFAULT;
                }

                break;
            case ISP_RT_BUF_CTRL_CUR_STATUS:
                reg_val  = ISP_RD32(ISP_REG_ADDR_TG_VF_CON) & 0x1;
                reg_val2 = ISP_RD32(ISP_REG_ADDR_TG2_VF_CON) & 0x1;
                camsv_reg_cal[0] = ISP_RD32(ISP_REG_ADDR_CAMSV_TG_VF_CON) & 0x1;
                camsv_reg_cal[1] = ISP_RD32(ISP_REG_ADDR_CAMSV_TG2_VF_CON) & 0x1;
                ISP_FBC_DUMP(rt_buf_ctrl.buf_id,reg_val,reg_val2,camsv_reg_cal[0],camsv_reg_cal[1]);
                break;
            case ISP_RT_BUF_CTRL_IS_RDY:
                //
                //                                                            
                //
                bWaitBufRdy = 1;
#ifdef _rtbc_buf_que_2_0_
                switch(rt_dma){
                    case _imgo_:
                    case _rrzo_:
                        irqT_Lock = _IRQ;
                        irqT = _IRQ;
                        break;
                    case _imgo_d_:
                    case _rrzo_d_:
                        irqT = _IRQ_D;
                        irqT_Lock = _IRQ;
                        break;
                    case _camsv_imgo_:
                        irqT_Lock = _CAMSV_IRQ;
                        irqT = _CAMSV_IRQ;
                        break;
                    case _camsv2_imgo_:
                        irqT_Lock = _CAMSV_D_IRQ;
                        irqT = _CAMSV_D_IRQ;
                        break;
                    default:
                        LOG_ERR("[rtbc]N.S.(%d)\n",rt_dma);
                        irqT_Lock = _IRQ;
                        irqT = _IRQ;
                        break;
                }

                spin_lock_irqsave(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);

                if ( ISP_RTBC_BUF_FILLED == pstRTBuf->ring_buf[rt_dma].data[pstRTBuf->ring_buf[rt_dma].read_idx].bFilled ) {
                    bWaitBufRdy = 0;
                }
                if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL){
                    MUINT32 z;
                    IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"cur dma:%d,read idx = %d,total cnt = %d,bWaitBufRdy= %d  ,",rt_dma,pstRTBuf->ring_buf[rt_dma].read_idx,pstRTBuf->ring_buf[rt_dma].total_count,bWaitBufRdy);
                    for(z=0;z<pstRTBuf->ring_buf[rt_dma].total_count;z++){
                        IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"%d_",pstRTBuf->ring_buf[rt_dma].data[z].bFilled);
                    }
                    IRQ_LOG_KEEPER(irqT,0,_LOG_DBG,"\n");
                }
                spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[irqT_Lock]), flags);
                IRQ_LOG_PRINTER(irqT,0,_LOG_DBG);
#else
    #if defined(_rtbc_use_cq0c_)
                bWaitBufRdy = p1_fbc[rt_dma].Bits.FBC_CNT?0:1;
    #else
                bWaitBufRdy = MTRUE;
    #endif
#endif

                //
                //                                                                 
                //
                //                                                                                        
                if(copy_to_user((void __user*)rt_buf_ctrl.pExtend, &bWaitBufRdy, sizeof(MUINT32)) != 0)
                {
                    LOG_ERR("[rtbc][IS_RDY]:copy_to_user failed");
                    Ret = -EFAULT;
                }
                //
                //                                                       
                //
                break;
            case ISP_RT_BUF_CTRL_GET_SIZE:
                //
                size = pstRTBuf->ring_buf[rt_dma].total_count;
                //
                //                                          
                //                                                                
                // 
                //                                                                                 
                if(copy_to_user((void __user*)rt_buf_ctrl.pExtend, &size, sizeof(MUINT32)) != 0)
                {
                    LOG_ERR("[rtbc][GET_SIZE]:copy_to_user failed");
                    Ret = -EFAULT;
                }
                break;
            case ISP_RT_BUF_CTRL_CLEAR:
                //
                if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
                    LOG_INF("[rtbc][CLEAR]:rt_dma(%d)",rt_dma);
                }
                //
                switch(rt_dma){
                    case _imgo_:
                    case _rrzo_:
                        memset(&g_DmaErr_p1[0],0,sizeof(MUINT32)*nDMA_ERR_P1);        
                        break;
                    case _imgo_d_:
                    case _rrzo_d_:
                        memset(&g_DmaErr_p1[nDMA_ERR_P1],0,sizeof(MUINT32)*(nDMA_ERR-nDMA_ERR_P1));
                        break;
                    case _camsv_imgo_:
                        break;
                    case _camsv2_imgo_:
                        break;
                    default:
                        LOG_ERR("[rtbc][CLEAR]N.S.(%d)\n",rt_dma);
                        return -EFAULT;
                }
                //                                                                               
                //                                                            

#if 0
                pstRTBuf->ring_buf[rt_dma].total_count= 0;
                pstRTBuf->ring_buf[rt_dma].start    = 0;
                pstRTBuf->ring_buf[rt_dma].empty_count= 0;
                pstRTBuf->ring_buf[rt_dma].active   = 0;

                for (i=0;i<ISP_RT_BUF_SIZE;i++) {
                    if ( pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr == rt_buf_info.base_pAddr ) {
                        buffer_exist = 1;
                        break;
                    }
                    //
                    if ( pstRTBuf->ring_buf[rt_dma].data[i].base_pAddr == 0 ) {
                        break;
                    }
                }
#else
                //                                                                                       
                //      
                pstRTBuf->ring_buf[rt_dma].active = MFALSE;
                memset((char*)&pstRTBuf->ring_buf[rt_dma],0x00,sizeof(ISP_RT_RING_BUF_INFO_STRUCT));
                //                         
                for (i=0 ;i<ISP_RT_BUF_SIZE;i++) {
                    pstRTBuf->ring_buf[rt_dma].data[i].image.frm_cnt = _INVALID_FRM_CNT_;
                }

                memset((char*)&prv_tstamp_s[rt_dma],0x0,sizeof(MUINT32));
                memset((char*)&prv_tstamp_us[rt_dma],0x0,sizeof(MUINT32));
#ifdef _rtbc_buf_que_2_0_
                memset((void*)dma_en_recorder[rt_dma],0,sizeof(MUINT8)*ISP_RT_BUF_SIZE);
                mFwRcnt.DMA_IDX[rt_dma] = 0;
#endif

                {
                    unsigned int ii = 0;
                    MUINT32 out[4] = {_IRQ_MAX,_IRQ_MAX,_IRQ_MAX,_IRQ_MAX};
                    if((pstRTBuf->ring_buf[_imgo_].active== MFALSE) && (pstRTBuf->ring_buf[_rrzo_].active== MFALSE) )
                            out[0] = _IRQ;
                    if((pstRTBuf->ring_buf[_imgo_d_].active== MFALSE) && (pstRTBuf->ring_buf[_rrzo_d_].active== MFALSE) )
                            out[1] = _IRQ_D;
                    if(pstRTBuf->ring_buf[_camsv_imgo_].active== MFALSE)
                            out[2] = _CAMSV_IRQ;
                    if(pstRTBuf->ring_buf[_camsv2_imgo_].active== MFALSE)
                            out[3] = _CAMSV_D_IRQ;

                    for (ii = 0; ii < 4; ii++) {
                        if(out[ii] != _IRQ_MAX){
                            sof_count[out[ii]] = 0;
                            start_time[out[ii]] = 0;
                            avg_frame_time[out[ii]] = 0;
                            g1stSof[out[ii]] = MTRUE;
                            PrvAddr[out[ii]] = 0;
                            g_ISPIntErr[out[ii]] = 0;
#ifdef _rtbc_buf_que_2_0_
                            mFwRcnt.bLoadBaseAddr[out[ii]] = 0;
                            mFwRcnt.curIdx[out[ii]] = 0;
                            memset((void*)mFwRcnt.INC[out[ii]],0,sizeof(MUINT32)*ISP_RT_BUF_SIZE);
                            mFwRcnt.rdIdx[out[ii]] = 0;
#endif
#ifdef T_STAMP_2_0
                            if(out[ii] == _IRQ){
                                memset((char*)&m_T_STAMP,0x0,sizeof(T_STAMP));
                                bSlowMotion = MFALSE;
                            }
#endif
                        }
                    }
                    for (ii = 0; ii < _rt_dma_max_; ii++){
                        if (pstRTBuf->ring_buf[ii].active) {
                            break;
                        }
                    }

                    if (ii == _rt_dma_max_) {
                            pstRTBuf->dropCnt = 0;
                            pstRTBuf->state = 0;
                    }
                }

#ifdef _MAGIC_NUM_ERR_HANDLING_
                m_LastMNum[rt_dma] = 0;
#endif

#endif
                //                                                                 

                break;
#ifdef _rtbc_buf_que_2_0_
            case ISP_RT_BUF_CTRL_DMA_EN:
                {
                    MUINT8 array[_rt_dma_max_];
                    //                                                                                                
                    if(copy_from_user(array, (void __user*)rt_buf_ctrl.pExtend, sizeof(UINT8)*_rt_dma_max_) == 0) {
                        MUINT32 z;
                        bRawEn = MFALSE;
                        bRawDEn = MFALSE;
                        for(z=0;z<_rt_dma_max_;z++) {
                            pstRTBuf->ring_buf[z].active = array[z];
                            if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
                                LOG_INF("[rtbc][DMA_EN]:dma_%d:%d",z,array[z]);
                            }
                        }
                        if ((MTRUE == pstRTBuf->ring_buf[_imgo_].active) || 
                            (MTRUE == pstRTBuf->ring_buf[_rrzo_].active))
                        {
                            bRawEn= MTRUE;
                        }
                        if ((MTRUE == pstRTBuf->ring_buf[_imgo_d_].active) || 
                            (MTRUE == pstRTBuf->ring_buf[_rrzo_d_].active))
                        {
                            bRawDEn= MTRUE;
                        }
                    }else {
                        LOG_ERR("[rtbc][DMA_EN]:copy_from_user failed");
                        Ret = -EFAULT;
                    }
                }
                break;
#endif
            case ISP_RT_BUF_CTRL_MAX:   //                                  
                //            
                break;

        }
        //
    }
    else
    {
        LOG_ERR("[rtbc]copy_from_user failed");
        Ret = -EFAULT;
    }

    return Ret;
}



#if 0
/* 
                                                                                                       
                                                                                                                             
*/
static MINT32 ISP_LostP1Done_ErrHandle(MUINT32 dma)
{
    switch(dma){
        case _imgo_:
        case _rrzo_:
        case _imgo_d_:
        case _rrzo_d_:
            pstRTBuf->ring_buf[dma].start++;
            pstRTBuf->ring_buf[dma].start = pstRTBuf->ring_buf[dma].start%pstRTBuf->ring_buf[dma].total_count;
            break;
        default: break;
    }
}
#endif
//                                                                                         
//                                                               
//                                
static MINT32 ISP_SOF_Buf_Get(eISPIrq irqT, CQ_RTBC_FBC* pFbc,MUINT32* pCurr_pa, unsigned long long sec,unsigned long usec,MBOOL bDrop)
{
#if defined(_rtbc_use_cq0c_)

    CQ_RTBC_FBC imgo_fbc;
    CQ_RTBC_FBC rrzo_fbc;
    MUINT32 imgo_idx = 0;//                                                                               
    MUINT32 rrzo_idx = 0;//                                                                                  
    MUINT32 curr_pa = 0;
    MUINT32 ch_imgo,ch_rrzo;
    MUINT32 i=0;
    MUINT32 _dma_cur_fw_idx = 0;
    MUINT32 _dma_cur_hw_idx = 0;
    MUINT32 _working_dma = 0;
    MUINT32 out=0;

    if(_IRQ == irqT) {
        imgo_fbc.Reg_val = pFbc[0].Reg_val;
        rrzo_fbc.Reg_val = pFbc[1].Reg_val;
        ch_imgo = _imgo_;
        ch_rrzo = _rrzo_;
        if(pstRTBuf->ring_buf[ch_imgo].active)
            curr_pa = pCurr_pa[0];
        else
            curr_pa = pCurr_pa[1];
        i = _PASS1;
    }
    else {  //      
        imgo_fbc.Reg_val = pFbc[2].Reg_val;
        rrzo_fbc.Reg_val = pFbc[3].Reg_val;
        ch_imgo = _imgo_d_;
        ch_rrzo = _rrzo_d_;
        if(pstRTBuf->ring_buf[ch_imgo].active)
            curr_pa = pCurr_pa[2];
        else
            curr_pa = pCurr_pa[3];
        i = _PASS1_D;
    }

    if(MTRUE == g1stSof[irqT]){ //                       
        pstRTBuf->ring_buf[ch_imgo].start = imgo_fbc.Bits.WCNT - 1;
        pstRTBuf->ring_buf[ch_rrzo].start = rrzo_fbc.Bits.WCNT - 1;
        //                                               
        g1stSof[irqT] = MFALSE;
    }

    //
#if 0   //                                                           
    //       
    if ( imgo_fbc.Bits.FB_NUM != imgo_fbc.Bits.FBC_CNT ) {
        pstRTBuf->dropCnt = 0;
    }
    //       
    else {
        pstRTBuf->dropCnt = 1;
    }
#else
    pstRTBuf->dropCnt = bDrop;
#endif
    //
    //                                       
    //                                                                                       
    // 
    //       
    if ( 0 == pstRTBuf->dropCnt ) {

        //                   

        //                         
        // 
        //                                                                                                      
            //              
        // 
        PrvAddr[i] = curr_pa;
#ifdef _rtbc_buf_que_2_0_
        imgo_idx = pstRTBuf->ring_buf[ch_imgo].start;
        rrzo_idx = pstRTBuf->ring_buf[ch_rrzo].start;
        //                     
        if(pstRTBuf->ring_buf[ch_imgo].active){
            _dma_cur_fw_idx = imgo_idx;
            _dma_cur_hw_idx = imgo_fbc.Bits.WCNT - 1;
            _working_dma = ch_imgo;
        }
        else if(pstRTBuf->ring_buf[ch_rrzo].active){
            _dma_cur_fw_idx = rrzo_idx;
            _dma_cur_hw_idx = rrzo_fbc.Bits.WCNT - 1;
            _working_dma = ch_rrzo;
        }
        if(_dma_cur_fw_idx != _dma_cur_hw_idx) {
            IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"dma sof after done %d_%d\n",_dma_cur_fw_idx,_dma_cur_hw_idx);
        }
#else
        //                        
        rrzo_idx = rrzo_fbc.Bits.WCNT - 1; //           
        //                                                   
        imgo_idx = rrzo_idx;
#endif
        //                                                             
        if ( ISP_RT_CQ0C_BUF_SIZE < pstRTBuf->ring_buf[_working_dma].total_count ) {
            IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_ERR,"buf cnt(%d)\n",pstRTBuf->ring_buf[_working_dma].total_count);
            pstRTBuf->ring_buf[_working_dma].total_count = ISP_RT_CQ0C_BUF_SIZE;
        }
        //
        if ( curr_pa != pstRTBuf->ring_buf[_working_dma].data[_dma_cur_fw_idx].base_pAddr ) {
            //
            //                                                                                               
            for( i = 0; i< pstRTBuf->ring_buf[_working_dma].total_count; i++ ) {
                //
                if ( curr_pa == pstRTBuf->ring_buf[_working_dma].data[i].base_pAddr ) {
                    //
                    if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                        IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"[rtbc]curr:old/new(%d/%d)\n",_dma_cur_fw_idx,i);
                    }
                    //    
                    //                                                                               
                    //                                                                    
                    //              
                    //             
                    //                                         
                    IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"img header err: PA(%x):0x%x_0x%x, idx:0x%x_0x%x\n",_working_dma,curr_pa,\
                        pstRTBuf->ring_buf[_working_dma].data[_dma_cur_fw_idx].base_pAddr,\
                        _dma_cur_fw_idx,i);
                    break;
                }
            }
        }
    //                                               
    //                                                                                                                                                                      
    //                                                                                                                                                                      
        //
        pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampS = sec;
        pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampUs = usec;
        pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].timeStampS = sec;
        pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].timeStampUs = usec;
        if(IspInfo.DebugMask & ISP_DBG_INT_3){
            static MUINT32 m_sec = 0,m_usec = 0;
            MUINT32 _tmp = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampS*1000000 + pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampUs;
            if(g1stSof[irqT]){
                m_sec = 0;
                m_usec = 0;
            }
            else
                IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF," timestamp:%d\n",(_tmp - (1000000*m_sec+m_usec)));
            m_sec = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampS;
            m_usec = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampUs;
        }
        if(_IRQ == irqT) {
            MUINT32 _tmp = ISP_RD32(TG_REG_ADDR_GRAB_W);
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.xsize = ISP_RD32(ISP_INNER_REG_ADDR_IMGO_XSIZE)&0x3FFF;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.w = ((_tmp>>16)&0x7FFF) - _tmp&0x7FFF;
            _tmp = ISP_RD32(TG_REG_ADDR_GRAB_H);
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.h = ((_tmp>>16)&0x1FFF) - _tmp&0x1FFF;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.stride = ISP_RD32(ISP_INNER_REG_ADDR_IMGO_STRIDE)&0x3FFF;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.bus_size = (ISP_RD32(ISP_INNER_REG_ADDR_IMGO_STRIDE)>>16)&0x03;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.fmt = (ISP_RD32(ISP_INNER_REG_ADDR_FMT_SEL_P1)&0xF000)>>12;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.pxl_id = (ISP_RD32(ISP_INNER_REG_ADDR_FMT_SEL_P1)&0x03);
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.m_num_0 = ISP_RD32(ISP_REG_ADDR_TG_MAGIC_0);
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.frm_cnt = (ISP_RD32(ISP_REG_ADDR_TG_INTER_ST)&0x00FF0000)>>16;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].dmaoCrop.x = ISP_RD32(ISP_INNER_REG_ADDR_IMGO_CROP) & 0x3fff;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].dmaoCrop.y = (ISP_RD32(ISP_INNER_REG_ADDR_IMGO_CROP)>>16) & 0x1fff;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].dmaoCrop.w = (ISP_RD32(ISP_INNER_REG_ADDR_IMGO_XSIZE)&0x3FFF) + 1;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].dmaoCrop.h = (ISP_RD32(ISP_INNER_REG_ADDR_IMGO_YSIZE)&0x1FFF) + 1;
            //                                                    
            //                                                   
            //                                                    
            //                                                    
            //
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.xsize = ISP_RD32(ISP_INNER_REG_ADDR_RRZO_XSIZE)&0x3FFF;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.w = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.w;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.h = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.h;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.stride = ISP_RD32(ISP_INNER_REG_ADDR_RRZO_STRIDE)&0x3FFF;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.bus_size = (ISP_RD32(ISP_INNER_REG_ADDR_RRZO_STRIDE)>>16)&0x03;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.fmt = (ISP_RD32(ISP_INNER_REG_ADDR_FMT_SEL_P1)&0x30)>>4;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.pxl_id = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.pxl_id;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.m_num_0 = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.m_num_0;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.frm_cnt = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.frm_cnt;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcX = ISP_RD32(ISP_INNER_REG_ADDR_RRZ_HORI_INT_OFST)&0x1FFF;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcY = ISP_RD32(ISP_INNER_REG_ADDR_RRZ_VERT_INT_OFST)&0x1FFF;
            _tmp = ISP_RD32(ISP_INNER_REG_ADDR_RRZ_IN_IMG);
            //                                                                                                                                            
            //                                                                                                                                                 
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcW = (ISP_RD32(ISP_REG_ADDR_TG_RRZ_CROP_IN)&0XFFFF);
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcH = (ISP_RD32(ISP_REG_ADDR_TG_RRZ_CROP_IN)>>16);                        
           
            _tmp = ISP_RD32(ISP_INNER_REG_ADDR_RRZ_OUT_IMG);
//                                                                                   
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.dstW = ISP_RD32(ISP_REG_ADDR_RRZ_W);
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.dstH = (_tmp>>16)&0x1FFF;            
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].dmaoCrop.x = ISP_RD32(ISP_INNER_REG_ADDR_RRZO_CROP) & 0x3fff;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].dmaoCrop.y = (ISP_RD32(ISP_INNER_REG_ADDR_RRZO_CROP)>>16) & 0x1fff;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].dmaoCrop.w = (ISP_RD32(ISP_INNER_REG_ADDR_RRZO_XSIZE)&0x3FFF) + 1;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].dmaoCrop.h = (ISP_RD32(ISP_INNER_REG_ADDR_RRZO_YSIZE)&0x1FFF) + 1;
            //

            //



#if 0
#ifdef _MAGIC_NUM_ERR_HANDLING_
                        LOG_ERR("[rtbc][sof0][m_num]:fc(0x%x),m0(0x%x),rrz_src(%d,%d)", \
                            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.frm_cnt, \
                            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.m_num_0, \
                            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcW, \
                            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcH);
#endif

            if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"[rtbc]TStamp(%d.%06d),curr(%d),pa(0x%x/0x%x),cq0c(0x%x)\n", \
                    pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampS,  \
                    pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampUs, \
                    imgo_idx,
                    pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].base_pAddr,  \
                    pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].base_pAddr,  \
                    ISP_RD32(ISP_ADDR+0xB4) );
            }
#endif
        }else {
            MUINT32 _tmp = ISP_RD32(TG2_REG_ADDR_GRAB_W);
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.xsize = ISP_RD32(ISP_INNER_REG_ADDR_IMGO_D_XSIZE)&0x3FFF;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.w = ((_tmp>>16)&0x7FFF) - _tmp&0x7FFF;
            _tmp = ISP_RD32(TG2_REG_ADDR_GRAB_H);
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.h = ((_tmp>>16)&0x1FFF) - _tmp&0x1FFF;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.stride  = ISP_RD32(ISP_INNER_REG_ADDR_IMGO_D_STRIDE)&0x3FFF;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.bus_size  = (ISP_RD32(ISP_INNER_REG_ADDR_IMGO_D_STRIDE)>>16)&0x03;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.fmt     = (ISP_RD32(ISP_INNER_REG_ADDR_FMT_SEL_P1_D)&0xF000)>>12;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.pxl_id  = (ISP_RD32(ISP_INNER_REG_ADDR_FMT_SEL_P1_D)&0x03);
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.m_num_0 = ISP_RD32(ISP_REG_ADDR_TG2_MAGIC_0);
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.frm_cnt = (ISP_RD32(ISP_REG_ADDR_TG2_INTER_ST)&0x00FF0000)>>16;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].dmaoCrop.x = ISP_RD32(ISP_INNER_REG_ADDR_IMGO_D_CROP) & 0x3fff;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].dmaoCrop.y = (ISP_RD32(ISP_INNER_REG_ADDR_IMGO_D_CROP)>>16) & 0x1fff;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].dmaoCrop.w = (ISP_RD32(ISP_INNER_REG_ADDR_IMGO_D_XSIZE)&0x3FFF) + 1;
            pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].dmaoCrop.h = (ISP_RD32(ISP_INNER_REG_ADDR_IMGO_D_YSIZE)&0x1FFF) + 1;

            //
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.xsize = ISP_RD32(ISP_INNER_REG_ADDR_RRZO_D_XSIZE)&0x3FFF;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.w = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.w;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.h = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.h;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.stride  = ISP_RD32(ISP_INNER_REG_ADDR_RRZO_D_STRIDE)&0x3FFF;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.bus_size  = (ISP_RD32(ISP_INNER_REG_ADDR_RRZO_D_STRIDE)>>16)&0x03;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.fmt     = (ISP_RD32(ISP_INNER_REG_ADDR_FMT_SEL_P1_D)&0x30)>>4;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.pxl_id  = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.pxl_id;;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.m_num_0 = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.m_num_0;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.frm_cnt = pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].image.frm_cnt;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcX = ISP_RD32(ISP_INNER_REG_ADDR_RRZ_D_HORI_INT_OFST)&0x1FFF;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcY = ISP_RD32(ISP_INNER_REG_ADDR_RRZ_D_VERT_INT_OFST)&0x1FFF;
            _tmp = ISP_RD32(ISP_INNER_REG_ADDR_RRZ_D_IN_IMG);
            //                                                                                                                                             
            //                                                                                                                                                   
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcW = (ISP_RD32(ISP_REG_ADDR_TG_RRZ_CROP_IN_D)&0XFFFF);
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcH = (ISP_RD32(ISP_REG_ADDR_TG_RRZ_CROP_IN_D)>>16);            
           
            _tmp = ISP_RD32(ISP_INNER_REG_ADDR_RRZ_D_OUT_IMG);
//                                                                                   
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.dstW = ISP_RD32(ISP_REG_ADDR_RRZ_W_D);
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.dstH  = (_tmp>>16)&0x1FFF;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].dmaoCrop.x = ISP_RD32(ISP_INNER_REG_ADDR_RRZO_D_CROP) & 0x3fff;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].dmaoCrop.y = (ISP_RD32(ISP_INNER_REG_ADDR_RRZO_D_CROP)>>16) & 0x1fff;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].dmaoCrop.w = (ISP_RD32(ISP_INNER_REG_ADDR_RRZO_D_XSIZE)&0x3FFF) + 1;
            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].dmaoCrop.h = (ISP_RD32(ISP_INNER_REG_ADDR_RRZO_D_YSIZE)&0x1FFF) + 1;
            //


#if 0
#ifdef _MAGIC_NUM_ERR_HANDLING_
                        LOG_ERR("[rtbc][sof0][m_num]:fc(0x%x),m0(0x%x),rrz_src(%d,%d)", \
                            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.frm_cnt, \
                            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].image.m_num_0, \
                            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcW, \
                            pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].rrzInfo.srcH);
#endif

            if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"[rtbc]TStamp(%d.%06d),curr(%d),pa(0x%x/0x%x),cq0c(0x%x)\n", \
                    pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampS,  \
                    pstRTBuf->ring_buf[ch_imgo].data[imgo_idx].timeStampUs, \
                    imgo_idx,
                    pstRTBuf->ring_buf[ch_imgo].data[rrzo_idx].base_pAddr,  \
                    pstRTBuf->ring_buf[ch_rrzo].data[rrzo_idx].base_pAddr,  \
                    ISP_RD32(ISP_ADDR+0xB4) );
            }
#endif
        }
        //
    }

    //                  
    DMA_TRANS(ch_imgo,out);
    if ( 0 == start_time[out] ) {
        start_time[out] = sec*1000000+ usec; //  
    }
    else {  //                    
        if(avg_frame_time[out]){
            avg_frame_time[out] += (sec*1000000+ usec)- avg_frame_time[out];
            avg_frame_time[out]  = avg_frame_time[out]>>1;
        }
        else{
            avg_frame_time[out] = (sec*1000000+ usec)- start_time[out];
        }
    }

    sof_count[out]++;
    if(sof_count[out] > 255 ){//                   
        sof_count[out] -= 256;
    }
    pstRTBuf->state = ISP_RTBC_STATE_SOF;
#else
    #ifdef _rtbc_buf_que_2_0_
        #error "isp kernel define condition is conflicted"
    #endif
#endif

    return 0;
}//

//                                                                                         
//                                                               
//                                
static MINT32 ISP_CAMSV_SOF_Buf_Get(unsigned int dma,CQ_RTBC_FBC camsv_fbc,MUINT32 curr_pa,unsigned long long sec,unsigned long usec,MBOOL bDrop)
{
    MUINT32 camsv_imgo_idx = 0;
    eISPIrq irqT;
    MUINT32 out;
    DMA_TRANS(dma,out);

    if(_camsv_imgo_ == dma ){
        irqT = _CAMSV_IRQ;
    }
    else {
        irqT = _CAMSV_D_IRQ;
    }

    if(MTRUE == g1stSof[irqT]){ //                       
        pstRTBuf->ring_buf[dma].start = camsv_fbc.Bits.WCNT - 1;
        g1stSof[irqT] = MFALSE;
    }

#if 0    //                                                           
    if(camsv_fbc.Bits.FB_NUM != camsv_fbc.Bits.FBC_CNT ){
        pstRTBuf->dropCnt = 0;
    }
    else {
        pstRTBuf->dropCnt = 1;
    }
#else
    pstRTBuf->dropCnt = bDrop;
#endif
    if(IspInfo.DebugMask & ISP_DBG_INT_2) {
        IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"sv%d dropCnt(%ld)\n",dma,pstRTBuf->dropCnt);
    }


    //       
    if ( 0 == pstRTBuf->dropCnt ) {
        if(PrvAddr[out] == curr_pa) {
            IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_ERR,"sv%d overlap prv(0x%x) = Cur(0x%x)\n",dma,PrvAddr[out],curr_pa);
            //              
        }
        PrvAddr[out] = curr_pa;

        //                        
        camsv_imgo_idx = (camsv_fbc.Bits.WCNT % camsv_fbc.Bits.FB_NUM) ; //          


        if(_camsv_imgo_ == dma) {
            ISP_WR32(ISP_REG_ADDR_IMGO_SV_BASE_ADDR,pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].base_pAddr);
        }
        else {
            ISP_WR32(ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR,pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].base_pAddr);
        }

        //
        camsv_imgo_idx = (camsv_imgo_idx>0)? (camsv_imgo_idx-1): (camsv_fbc.Bits.FB_NUM-1);//           
        if(camsv_imgo_idx  != pstRTBuf->ring_buf[dma].start) {//                                                                 
            IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_ERR,"sv%d WCNT%d != start%d\n",dma,camsv_fbc.Bits.WCNT,pstRTBuf->ring_buf[dma].start);
        }
        pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].timeStampS = sec;
        pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].timeStampUs = usec;
        //                                                                           
        //                                                  
        if(dma == _camsv_imgo_){
            pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].image.w = (ISP_RD32(ISP_REG_ADDR_IMGO_SV_XSIZE) & 0x3FFF);
            pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].image.h = (ISP_RD32(ISP_REG_ADDR_IMGO_SV_YSIZE) & 0x1FFF);
            pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].image.stride = (ISP_RD32(ISP_REG_ADDR_IMGO_SV_STRIDE) & 0x3FFF);
            pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].image.fmt = (ISP_RD32(ISP_REG_ADDR_CAMSV_FMT_SEL) & 0x30000);
        }
        else {
            pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].image.w = (ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_XSIZE) & 0x3FFF);
            pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].image.h = (ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_YSIZE) & 0x1FFF);
            pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].image.stride = (ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_STRIDE) & 0x3FFF);
            pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].image.fmt = (ISP_RD32(ISP_REG_ADDR_CAMSV2_FMT_SEL) & 0x30000);
        }

        //
        if(IspInfo.DebugMask & ISP_DBG_INT_2) {
           IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"sv%d T(%d.%06d),cur(%d),addr(0x%x),prv(0x%x),fbc(0x%08x)\n", \
                dma,\
                pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].timeStampS,  \
                pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].timeStampUs, \
                camsv_imgo_idx, \
                pstRTBuf->ring_buf[dma].data[camsv_imgo_idx].base_pAddr,  \
                PrvAddr[out], \
                camsv_fbc.Reg_val);
        }
        //
    }

    if ( 0 == start_time[out] ) {
        start_time[out] = sec*1000000+ usec; //  
    }
    else {  //                    
        if(avg_frame_time[out]){
            avg_frame_time[out] += (sec*1000000+ usec)- avg_frame_time[out];
            avg_frame_time[out]  = avg_frame_time[out]>>1;
        }
        else{
            avg_frame_time[out] = (sec*1000000+ usec)- start_time[out];
        }
    }

    sof_count[out]++;

    pstRTBuf->state = ISP_RTBC_STATE_SOF;
    return 0;
}

//                                                                                         
//                                                               
//                                
static MINT32 ISP_CAMSV_DONE_Buf_Time(unsigned int dma,CQ_RTBC_FBC fbc, unsigned long long sec,unsigned long usec)
{
    unsigned int curr;
    eISPIrq irqT;
    //                      
    MUINT32 _tmp;
    MUINT32 out=0;

    //
    if(_camsv_imgo_==dma) {
        irqT = _CAMSV_IRQ;
    }
    else {
        irqT = _CAMSV_D_IRQ;
    }

    //
    if ( 0 == pstRTBuf->ring_buf[dma].empty_count) {
        //
        if(IspInfo.DebugMask & ISP_DBG_INT_2) {
            IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"sv%d RTB empty,start(%d)\n",dma,pstRTBuf->ring_buf[dma].start);
        }
        //                
        return -1;
    }

    curr = pstRTBuf->ring_buf[dma].start;

    {//                                            
        _tmp = fbc.Bits.WCNT-1;
        _tmp = (_tmp > 0)?(_tmp - 1):(fbc.Bits.FB_NUM -1);
    }
    if(curr != _tmp ) {
        IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_ERR,"sv%d:RTBC_%d != FBC cnt_%d\n",dma,curr,_tmp);
    }

    DMA_TRANS(dma,out);
    while(1)//                                                 
    {
        if(IspInfo.DebugMask & ISP_DBG_INT_2) {
            IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"sv%d,cur(%d),bFilled(%d)\n",\
                    dma,\
                    curr,\
                    pstRTBuf->ring_buf[dma].data[curr].bFilled);
        }
        //                                                                                  
        if(pstRTBuf->ring_buf[dma].data[curr].bFilled == ISP_RTBC_BUF_EMPTY)
        {
            pstRTBuf->ring_buf[dma].data[curr].bFilled = ISP_RTBC_BUF_FILLED;
            //         
            pstRTBuf->ring_buf[dma].start = (curr+1)%pstRTBuf->ring_buf[dma].total_count;
            pstRTBuf->ring_buf[dma].empty_count--;
            pstRTBuf->ring_buf[dma].img_cnt = sof_count[out];
            if(g1stSof[irqT] == MTRUE){
                LOG_ERR("Done&&Sof recieve at the same time in 1st f\n");
            }
            break;
        }
        else
        {
            if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_ERR,"sv%d:curr(%d),bFilled(%d) != EMPTY\n",\
                        dma,\
                        curr,\
                        pstRTBuf->ring_buf[dma].data[curr].bFilled);
            }
            //         
            //                                                    
            break;
        }
#if 0
        if(++loopCount > pstRTBuf->ring_buf[dma].total_count)
        {
            IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_ERR,"sv%d:find no empty buf in total_count(%d)\n",\
                    dma,\
                    pstRTBuf->ring_buf[dma].total_count);
            break;
        }
        else{
            IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"sv%d:buf is not empty for current p1_done\n",dma);
        }
#endif
    }

    //
    if(IspInfo.DebugMask & ISP_DBG_INT_2) {
        IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"sv%d:start(%d),empty(%d)\n", \
            dma,\
            pstRTBuf->ring_buf[dma].start,\
            pstRTBuf->ring_buf[dma].empty_count);
    }

    //
    //                                       
    //                                                                                                           
    // 


    //
    pstRTBuf->state = ISP_RTBC_STATE_DONE;

    return 0;
}

//                                                                                         
//                                                               
static MINT32 ISP_DONE_Buf_Time(eISPIrq irqT, CQ_RTBC_FBC* pFbc ,unsigned long long sec,unsigned long usec)
{
    int i,k,m;
    int i_dma;
    unsigned int curr;
    //                     
    //                    
    MUINT32 ch_imgo,ch_rrzo;//                                                                   
    CQ_RTBC_FBC imgo_fbc;
    CQ_RTBC_FBC rrzo_fbc;
    CQ_RTBC_FBC _dma_cur_fbc;
    MUINT32 _working_dma = 0;
#ifdef _rtbc_buf_que_2_0_
    //                                  
    MUINT32 shiftT = 0;
    MUINT32 out;
#endif
    if(_IRQ == irqT) {
        ch_imgo = _imgo_;
        ch_rrzo = _rrzo_;
        imgo_fbc.Reg_val = pFbc[0].Reg_val;
        rrzo_fbc.Reg_val = pFbc[1].Reg_val;
    } else {
        ch_imgo = _imgo_d_;
        ch_rrzo = _rrzo_d_;
        imgo_fbc.Reg_val = pFbc[2].Reg_val;
        rrzo_fbc.Reg_val = pFbc[3].Reg_val;
    }

#ifdef _rtbc_buf_que_2_0_

    //                     
    if(pstRTBuf->ring_buf[ch_imgo].active){
        _dma_cur_fbc = imgo_fbc;
        _working_dma = ch_imgo;
    }
    else if(pstRTBuf->ring_buf[ch_rrzo].active){
        _dma_cur_fbc = rrzo_fbc;
        _working_dma = ch_rrzo;
    }
    else
    {
        LOG_ERR("non-supported dma port(%d/%d)\n",pstRTBuf->ring_buf[ch_imgo].active,pstRTBuf->ring_buf[ch_rrzo].active);
        return 0;
    }
    //                              
    if(_dma_cur_fbc.Bits.WCNT > 0){
        if( _dma_cur_fbc.Bits.WCNT  > (pstRTBuf->ring_buf[_working_dma].start + 2) )  {
            shiftT = _dma_cur_fbc.Bits.WCNT - pstRTBuf->ring_buf[_working_dma].start - 2;
            if(shiftT > 0)
                IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"[rtbc%d]:alert(%d,%d)\n",irqT,pstRTBuf->ring_buf[_working_dma].start,_dma_cur_fbc.Bits.WCNT);
        }
        else if( _dma_cur_fbc.Bits.WCNT  < (pstRTBuf->ring_buf[_working_dma].start + 2) )  {
            shiftT = _dma_cur_fbc.Bits.WCNT + _dma_cur_fbc.Bits.FB_NUM - (pstRTBuf->ring_buf[_working_dma].start + 2);
            if(shiftT >= _dma_cur_fbc.Bits.FB_NUM)
            {
                LOG_ERR("err shiftT = (%d,%d ,%d)\n",_dma_cur_fbc.Bits.WCNT,_dma_cur_fbc.Bits.FB_NUM,pstRTBuf->ring_buf[_working_dma].start);
                shiftT = (_dma_cur_fbc.Bits.FB_NUM?(_dma_cur_fbc.Bits.FB_NUM - 1):(_dma_cur_fbc.Bits.FB_NUM));
            }
        }
        else{}//                                                                      
    }
#endif


#ifdef _rtbc_buf_que_2_0_
    for(k=0;k<shiftT+1;k++) //                     
#endif
    {
        for ( i=0;i<=1;i++ ) {
            //
            if(0==i) {
                i_dma = ch_imgo;
                //                      
            } else {
                i_dma = ch_rrzo;
                //                      
            }
            //
            if ( 0 == pstRTBuf->ring_buf[i_dma].empty_count) {
                //
                if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                    IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"[rtbc][DONE]:dma(%d)buf num empty,start(%d)\n",i_dma,pstRTBuf->ring_buf[i_dma].start);
                }
                //
                continue;
            }
#if 0
            //                                                       
            if ( MFALSE == pstRTBuf->ring_buf[i_dma].active ) {
                //
                if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                    LOG_DBG("[rtbc][DONE] ERROR: missing SOF ");
                }
                //
                continue;
            }
#endif
            curr = pstRTBuf->ring_buf[i_dma].start;
            //                      
            while(1)
            {
                if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                    IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"i_dma(%d),curr(%d),bFilled(%d)\n",
                            i_dma,
                            curr,
                            pstRTBuf->ring_buf[i_dma].data[curr].bFilled);
                }
                //
                if(pstRTBuf->ring_buf[i_dma].data[curr].bFilled == ISP_RTBC_BUF_EMPTY)
                {
                    if(IspInfo.DebugMask & ISP_DBG_INT_2)
                        IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"[rtbc][DONE]:dma_%d,fill buffer,cur_%d\n",i_dma,curr);
                    pstRTBuf->ring_buf[i_dma].data[curr].bFilled = ISP_RTBC_BUF_FILLED;
                    //         
                    pstRTBuf->ring_buf[i_dma].start = (curr+1)%pstRTBuf->ring_buf[i_dma].total_count;
                    pstRTBuf->ring_buf[i_dma].empty_count--;
                    //
                    if(g1stSof[irqT] == MTRUE){
                        LOG_ERR("Done&&Sof recieve at the same time in 1st f(%d)\n",i_dma);
                    }
                    break;
                }
                else
                {
                    if(1){//                                     
                        for (m=0;m<ISP_RT_BUF_SIZE;)
                        {
                            LOG_ERR("dma_%d,cur_%d,bFilled_%d != EMPTY(%d %d %d %d)\n",\
                                i_dma,\
                                curr,\
                                pstRTBuf->ring_buf[i_dma].data[curr].bFilled,\
                                pstRTBuf->ring_buf[i_dma].data[m].bFilled,pstRTBuf->ring_buf[i_dma].data[m+1].bFilled,pstRTBuf->ring_buf[i_dma].data[m+2].bFilled,pstRTBuf->ring_buf[i_dma].data[m+3].bFilled);
                            m=m+4;                          
                        }
                    }
                    //         
                    //                                                                                 
                    break;
                }
#if 0
                loopCount++;
                if(loopCount > pstRTBuf->ring_buf[i_dma].total_count)
                {
                    LOG_ERR("Can't find empty dma(%d) buf in total_count(%d)",
                            i_dma,
                            pstRTBuf->ring_buf[i_dma].total_count);
                    break;
                }
#endif
            }
#if 0
            //                       
            if ( 0 == pstRTBuf->ring_buf[i_dma].empty_count) {
                    if (_imgo_ == i_dma) {
                        reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_FBC);
                        reg_val |= 0x4000;
                        //                                        
                    }
                    else {
                        reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_D_FBC);
                        reg_val |= 0x4000;
                        //                                          
                    }
                    //
                    if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                    IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"[rtbc][DONE]:dma(%d),en fbc(0x%x) stalled DMA out",i_dma,ISP_RD32(reg_fbc));
                }
            }
#endif
            //
            if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"[rtbc][DONE]:dma(%d),start(%d),empty(%d)\n", \
                    i_dma,
                    pstRTBuf->ring_buf[i_dma].start,
                    pstRTBuf->ring_buf[i_dma].empty_count);
            }
#if 0 //                      
            //
            pstRTBuf->ring_buf[i_dma].data[curr].timeStampS = sec;
            pstRTBuf->ring_buf[i_dma].data[curr].timeStampUs = usec;
            //
            if(IspInfo.DebugMask & ISP_DBG_INT_2) {
                LOG_DBG("[rtbc][DONE]:dma(%d),curr(%d),sec(%lld),usec(%ld) ", i_dma, curr, sec, usec);
            }
#endif
            //
            DMA_TRANS(i_dma,out);
            pstRTBuf->ring_buf[i_dma].img_cnt = sof_count[out];
        }
    }

    if(pstRTBuf->ring_buf[ch_imgo].active && pstRTBuf->ring_buf[ch_rrzo].active){
        if(pstRTBuf->ring_buf[ch_imgo].start!= pstRTBuf->ring_buf[ch_rrzo].start){
            LOG_ERR("start idx mismatch %d_%d(%d %d %d,%d %d %d)",pstRTBuf->ring_buf[ch_imgo].start,pstRTBuf->ring_buf[ch_rrzo].start,\
            pstRTBuf->ring_buf[ch_imgo].data[0].bFilled,pstRTBuf->ring_buf[ch_imgo].data[1].bFilled,pstRTBuf->ring_buf[ch_imgo].data[2].bFilled,\
            pstRTBuf->ring_buf[ch_rrzo].data[0].bFilled,pstRTBuf->ring_buf[ch_rrzo].data[1].bFilled,pstRTBuf->ring_buf[ch_rrzo].data[2].bFilled);
        }
    }
    //                                                                                                                                                                                 
    //                                                                                                                                                                                 
#if 0
    if(IspInfo.DebugMask & ISP_DBG_INT_2) {
        IRQ_LOG_KEEPER(irqT,m_CurrentPPB,_LOG_INF,"-:[rtbc]");
    }
#endif
    //
    pstRTBuf->state = ISP_RTBC_STATE_DONE;
    //                                                                 

    return 0;
}

/*                                                                              
 
                                                                               */
static MINT32 ISP_ED_BufQue_Update_GPtr(int listTag)
{
    MINT32 ret=0;
    MINT32 tmpIdx=0;
    MINT32 cnt=0;
    bool stop=false;
    int i=0;
    ISP_ED_BUF_STATE_ENUM gPtrSts=ISP_ED_BUF_STATE_NONE;
    switch(listTag)
    {
        case P2_EDBUF_RLIST_TAG:
            //                                    
            gPtrSts=P2_EDBUF_RingList[P2_EDBUF_RList_CurBufIdx].bufSts;

            //                                                                      
            //                                                            
            //                                         
            //                                               
            //                                         
            //                                             
            //            
            //
            //                                                                                                                                           
            //                                                                      
            //                                  
            if(P2_EDBUF_RList_FirstBufIdx<=P2_EDBUF_RList_LastBufIdx)
            {
                cnt=P2_EDBUF_RList_LastBufIdx-P2_EDBUF_RList_FirstBufIdx;
            }
            else
            {
                cnt=_MAX_SUPPORT_P2_FRAME_NUM_-P2_EDBUF_RList_FirstBufIdx;
                cnt+=P2_EDBUF_RList_LastBufIdx;
            }

            //          
            tmpIdx=P2_EDBUF_RList_CurBufIdx;
            switch(gPtrSts)
            {
                case ISP_ED_BUF_STATE_ENQUE:
                    P2_EDBUF_RingList[P2_EDBUF_RList_CurBufIdx].bufSts=ISP_ED_BUF_STATE_RUNNING;
                    break;
                case ISP_ED_BUF_STATE_WAIT_DEQUE_FAIL:
                case ISP_ED_BUF_STATE_DEQUE_SUCCESS:
                case ISP_ED_BUF_STATE_DEQUE_FAIL:
                    do  //                            
                    {
                        tmpIdx=(tmpIdx+1)%_MAX_SUPPORT_P2_FRAME_NUM_;
                        switch(P2_EDBUF_RingList[tmpIdx].bufSts)
                        {
                            case ISP_ED_BUF_STATE_ENQUE:
                            case ISP_ED_BUF_STATE_RUNNING:
                                P2_EDBUF_RingList[tmpIdx].bufSts=ISP_ED_BUF_STATE_RUNNING;
                                P2_EDBUF_RList_CurBufIdx=tmpIdx;
                                stop=true;
                                break;
                            case ISP_ED_BUF_STATE_WAIT_DEQUE_FAIL:
                            case ISP_ED_BUF_STATE_DEQUE_SUCCESS:
                            case ISP_ED_BUF_STATE_DEQUE_FAIL:
                            case ISP_ED_BUF_STATE_NONE:
                            default:
                                break;
                        }
                        i++;
                    }while((i<cnt) && (!stop));
                    //                                                                      
                    //                                                            
                    //                                         
                    //                                               
                    //                                         
                    //                                             
                    //            
                    //
                    //                                                                                                         
                    //                                                                                   
                    //                                                                      
                    if((!stop) && (i==(cnt)))
                    {
                        P2_EDBUF_RList_CurBufIdx=P2_EDBUF_RList_LastBufIdx;
                    }
                    break;
                case ISP_ED_BUF_STATE_NONE:
                case ISP_ED_BUF_STATE_RUNNING:
                default:
                    break;
            }
            break;
        case P2_EDBUF_MLIST_TAG:
        default:
            LOG_ERR("Wrong List tag(%d)\n",listTag);
            break;
    }
    return ret;
}
/*                                                                              
 
                                                                               */
#if 0 //                                  
static MINT32 ISP_ED_BufQue_Set_FailNode(ISP_ED_BUF_STATE_ENUM failType,MINT32 idx)
{
    MINT32 ret=0;
    spin_lock(&(SpinLockEDBufQueList));
    //                
    P2_EDBUF_RingList[idx].bufSts=failType;

    //                        
    ISP_ED_BufQue_Update_GPtr(P2_EDBUF_RLIST_TAG);
    spin_unlock(&(SpinLockEDBufQueList));
    return ret;
}
#endif

/*                                                                              
 
                                                                               */
static MINT32 ISP_ED_BufQue_Erase(MINT32 idx,int listTag)
{
    MINT32 ret=-1;
    bool stop=false;
    int i=0;
    MINT32 cnt=0;
    int tmpIdx=0;

    switch(listTag)
    {
        case P2_EDBUF_MLIST_TAG:
            tmpIdx=P2_EDBUF_MList_FirstBufIdx;
            //                       
            P2_EDBUF_MgrList[idx].processID=0x0;
            P2_EDBUF_MgrList[idx].callerID=0x0;
            P2_EDBUF_MgrList[idx].p2dupCQIdx=-1;
            P2_EDBUF_MgrList[idx].dequedNum=0;
            //                      
            if(P2_EDBUF_MgrList[tmpIdx].p2dupCQIdx == -1)
            {
                //                                                                                                       
                if(P2_EDBUF_MList_FirstBufIdx<=P2_EDBUF_MList_LastBufIdx)
                {
                    cnt=P2_EDBUF_MList_LastBufIdx-P2_EDBUF_MList_FirstBufIdx;
                }
                else
                {
                    cnt=_MAX_SUPPORT_P2_PACKAGE_NUM_-P2_EDBUF_MList_FirstBufIdx;
                    cnt+=P2_EDBUF_MList_LastBufIdx;
                }
                do  //                               
                {
                    tmpIdx=(tmpIdx+1)%_MAX_SUPPORT_P2_PACKAGE_NUM_;
                    switch(P2_EDBUF_MgrList[tmpIdx].p2dupCQIdx)
                    {
                        case (-1):
                            break;
                        default:
                            stop=true;
                            P2_EDBUF_MList_FirstBufIdx=tmpIdx;
                            break;
                    }
                    i++;
                }while((i<cnt) && (!stop));
                //                                                                     
                //                                                                                                        
                if((!stop) && (i==cnt))
                {
                    P2_EDBUF_MList_FirstBufIdx=P2_EDBUF_MList_LastBufIdx;
                }
            }
            break;
        case P2_EDBUF_RLIST_TAG:
            tmpIdx=P2_EDBUF_RList_FirstBufIdx;
            //                       
            P2_EDBUF_RingList[idx].processID=0x0;
            P2_EDBUF_RingList[idx].callerID=0x0;
            P2_EDBUF_RingList[idx].p2dupCQIdx=-1;
            P2_EDBUF_RingList[idx].bufSts=ISP_ED_BUF_STATE_NONE;
            EDBufQueRemainNodeCnt--;
            //                     
            if(P2_EDBUF_RingList[tmpIdx].bufSts == ISP_ED_BUF_STATE_NONE)
            {
                //                                                                                  
                if(P2_EDBUF_RList_FirstBufIdx<=P2_EDBUF_RList_LastBufIdx)
                {
                    cnt=P2_EDBUF_RList_LastBufIdx-P2_EDBUF_RList_FirstBufIdx;
                }
                else
                {
                    cnt=_MAX_SUPPORT_P2_FRAME_NUM_-P2_EDBUF_RList_FirstBufIdx;
                    cnt+=P2_EDBUF_RList_LastBufIdx;
                }
                //                               
                do
                {
                    tmpIdx=(tmpIdx+1)%_MAX_SUPPORT_P2_FRAME_NUM_;
                    switch(P2_EDBUF_RingList[tmpIdx].bufSts)
                    {
                        case ISP_ED_BUF_STATE_ENQUE:
                        case ISP_ED_BUF_STATE_RUNNING:
                        case ISP_ED_BUF_STATE_WAIT_DEQUE_FAIL:
                        case ISP_ED_BUF_STATE_DEQUE_SUCCESS:
                        case ISP_ED_BUF_STATE_DEQUE_FAIL:
                            stop=true;
                            P2_EDBUF_RList_FirstBufIdx=tmpIdx;
                            break;
                        case ISP_ED_BUF_STATE_NONE:
                        default:
                            break;
                    }
                    i++;
                }while((i<cnt) && (!stop));
                //                                                                     
                //                                                                                                        
                if((!stop) && (i==(cnt)))
                {
                    P2_EDBUF_RList_FirstBufIdx=P2_EDBUF_RList_LastBufIdx;
                }
            }
            break;
        default:
            break;
    }
    return ret;
}

/*                                                                              
                          
                                                                               */
static MINT32 ISP_ED_BufQue_Get_FirstMatBuf(ISP_ED_BUFQUE_STRUCT param,int ListTag, int type)
{
    MINT32 idx=-1;
    MINT32 i=0;
    switch(ListTag)
    {
        case P2_EDBUF_MLIST_TAG:
            if(type==0)
            {   //                                                                                
                if(P2_EDBUF_MList_FirstBufIdx<=P2_EDBUF_MList_LastBufIdx)
                {
                    for(i=P2_EDBUF_MList_FirstBufIdx;i<=P2_EDBUF_MList_LastBufIdx;i++)
                    {
                        if((P2_EDBUF_MgrList[i].processID==param.processID) && (P2_EDBUF_MgrList[i].callerID==param.callerID))
                        {
                            idx=i;
                            break;
                        }
                    }
                }
                else
                {
                    for(i=P2_EDBUF_MList_FirstBufIdx;i<_MAX_SUPPORT_P2_PACKAGE_NUM_;i++)
                    {
                        if((P2_EDBUF_MgrList[i].processID==param.processID) && (P2_EDBUF_MgrList[i].callerID==param.callerID))
                        {
                            idx=i;
                            break;
                        }
                    }
                    if(idx!=-1)
                    {/*                         */}
                    else
                    {
                        for(i=0;i<=P2_EDBUF_MList_LastBufIdx;i++)
                        {
                            if((P2_EDBUF_MgrList[i].processID==param.processID) && (P2_EDBUF_MgrList[i].callerID==param.callerID))
                            {
                                idx=i;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {   //                                 
                if(P2_EDBUF_MList_FirstBufIdx<=P2_EDBUF_MList_LastBufIdx)
                {
                    for(i=P2_EDBUF_MList_FirstBufIdx;i<=P2_EDBUF_MList_LastBufIdx;i++)
                    {
                        if((P2_EDBUF_MgrList[i].processID==param.processID) && (P2_EDBUF_MgrList[i].callerID==param.callerID) &&
                            (P2_EDBUF_MgrList[i].p2dupCQIdx==param.p2dupCQIdx) && (P2_EDBUF_MgrList[i].dequedNum<P2_Support_BurstQNum))
                        {   //                                                                                                                
                            idx=i;
                            break;
                        }
                    }
                }
                else
                {
                    for(i=P2_EDBUF_MList_FirstBufIdx;i<_MAX_SUPPORT_P2_PACKAGE_NUM_;i++)
                    {
                        if((P2_EDBUF_MgrList[i].processID==param.processID) && (P2_EDBUF_MgrList[i].callerID==param.callerID) &&
                            (P2_EDBUF_MgrList[i].p2dupCQIdx==param.p2dupCQIdx) && (P2_EDBUF_MgrList[i].dequedNum<P2_Support_BurstQNum))
                        {
                            idx=i;
                            break;
                        }
                    }
                    if(idx!=-1)
                    {/*                         */}
                    else
                    {
                        for(i=0;i<=P2_EDBUF_MList_LastBufIdx;i++)
                        {
                            if((P2_EDBUF_MgrList[i].processID==param.processID) && (P2_EDBUF_MgrList[i].callerID==param.callerID) &&
                                (P2_EDBUF_MgrList[i].p2dupCQIdx==param.p2dupCQIdx) && (P2_EDBUF_MgrList[i].dequedNum<P2_Support_BurstQNum))
                            {
                                idx=i;
                                break;
                            }
                        }
                    }
                }
            }
            break;
        case P2_EDBUF_RLIST_TAG:
            if(P2_EDBUF_RList_FirstBufIdx<=P2_EDBUF_RList_LastBufIdx)
            {
                for(i=P2_EDBUF_RList_FirstBufIdx;i<=P2_EDBUF_RList_LastBufIdx;i++)
                {
                    if((P2_EDBUF_RingList[i].processID==param.processID) && (P2_EDBUF_RingList[i].callerID==param.callerID))
                    {
                        idx=i;
                        break;
                    }
                }
            }
            else
            {
                for(i=P2_EDBUF_RList_FirstBufIdx;i<_MAX_SUPPORT_P2_FRAME_NUM_;i++)
                {
                    if((P2_EDBUF_RingList[i].processID==param.processID) && (P2_EDBUF_RingList[i].callerID==param.callerID))
                    {
                        idx=i;
                        break;
                    }
                }
                if(idx!=-1)
                {/*                         */}
                else
                {
                    for(i=0;i<=P2_EDBUF_RList_LastBufIdx;i++)
                    {
                        if((P2_EDBUF_RingList[i].processID==param.processID) && (P2_EDBUF_RingList[i].callerID==param.callerID))
                        {
                            idx=i;
                            break;
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    if(idx==-1)
    {
        LOG_ERR("Could not find match buffer tag(%d) pid/cid/p2dupCQidx(%d/0x%x/%d)",ListTag,param.processID,param.callerID,param.p2dupCQIdx);
    }
    return idx;
}
/*                                                                              
 
                                                                               */
static MINT32 ISP_ED_BufQue_CTRL_FUNC(ISP_ED_BUFQUE_STRUCT param)
{
    MINT32 ret=0;
    int i=0;
    int idx=-1,idx2=-1;
    MINT32 restTime=0;
    switch(param.ctrl)
    {
        case ISP_ED_BUFQUE_CTRL_ENQUE_FRAME:    //                                         
            //                                             
            spin_lock(&(SpinLockEDBufQueList));
            if(((P2_EDBUF_MList_LastBufIdx+1)%_MAX_SUPPORT_P2_PACKAGE_NUM_) == P2_EDBUF_MList_FirstBufIdx && (P2_EDBUF_MList_LastBufIdx!=-1))
            {
                LOG_ERR("F/L(%d,%d),(%d,%d), RF/C/L(%d,%d,%d),(%d,%d,%d)",P2_EDBUF_MList_FirstBufIdx,P2_EDBUF_MList_LastBufIdx,\
                    P2_EDBUF_MgrList[P2_EDBUF_MList_FirstBufIdx].p2dupCQIdx,P2_EDBUF_MgrList[P2_EDBUF_MList_LastBufIdx].p2dupCQIdx,\
                    P2_EDBUF_RList_FirstBufIdx,P2_EDBUF_RList_CurBufIdx,P2_EDBUF_RList_LastBufIdx,P2_EDBUF_RingList[P2_EDBUF_RList_FirstBufIdx].bufSts,\
                    P2_EDBUF_RingList[P2_EDBUF_RList_CurBufIdx].bufSts,P2_EDBUF_RingList[P2_EDBUF_RList_LastBufIdx].bufSts);
                spin_unlock(&(SpinLockEDBufQueList));
                LOG_ERR("p2 ring buffer list is full, enque Fail.");
                ret=-EFAULT;
                return ret;
            }
            else
            {
                IRQ_LOG_KEEPER(_CAMSV_D_IRQ,0,_LOG_DBG,"pD(%d_0x%x) MF/L(%d,%d),(%d,%d), RF/C/L(%d,%d,%d),(%d,%d,%d),dCq(%d)/Bq(%d)\n",\
                    param.processID,param.callerID,P2_EDBUF_MList_FirstBufIdx,P2_EDBUF_MList_LastBufIdx,\
                    P2_EDBUF_MgrList[P2_EDBUF_MList_FirstBufIdx].p2dupCQIdx,P2_EDBUF_MgrList[P2_EDBUF_MList_LastBufIdx].p2dupCQIdx,\
                    P2_EDBUF_RList_FirstBufIdx,P2_EDBUF_RList_CurBufIdx,P2_EDBUF_RList_LastBufIdx,P2_EDBUF_RingList[P2_EDBUF_RList_FirstBufIdx].bufSts,\
                    P2_EDBUF_RingList[P2_EDBUF_RList_CurBufIdx].bufSts,P2_EDBUF_RingList[P2_EDBUF_RList_LastBufIdx].bufSts,\
                    param.p2dupCQIdx,param.p2burstQIdx);
                //                                           
                if(P2_EDBUF_RList_FirstBufIdx==P2_EDBUF_RList_LastBufIdx && P2_EDBUF_RingList[P2_EDBUF_RList_FirstBufIdx].bufSts==ISP_ED_BUF_STATE_NONE)
                {   //                        
                    P2_EDBUF_RList_LastBufIdx=(P2_EDBUF_RList_LastBufIdx+1)%_MAX_SUPPORT_P2_FRAME_NUM_;
                    P2_EDBUF_RList_FirstBufIdx=P2_EDBUF_RList_LastBufIdx;
                    P2_EDBUF_RList_CurBufIdx=P2_EDBUF_RList_LastBufIdx;
                }
                else if(P2_EDBUF_RList_CurBufIdx==P2_EDBUF_RList_LastBufIdx && P2_EDBUF_RingList[P2_EDBUF_RList_CurBufIdx].bufSts==ISP_ED_BUF_STATE_NONE)
                {   //                                                  
                    P2_EDBUF_RList_LastBufIdx=(P2_EDBUF_RList_LastBufIdx+1)%_MAX_SUPPORT_P2_FRAME_NUM_;
                    P2_EDBUF_RList_CurBufIdx=P2_EDBUF_RList_LastBufIdx;
                }
                else
                {
                    P2_EDBUF_RList_LastBufIdx=(P2_EDBUF_RList_LastBufIdx+1)%_MAX_SUPPORT_P2_FRAME_NUM_;
                }
                P2_EDBUF_RingList[P2_EDBUF_RList_LastBufIdx].processID=param.processID;
                P2_EDBUF_RingList[P2_EDBUF_RList_LastBufIdx].callerID=param.callerID;
                P2_EDBUF_RingList[P2_EDBUF_RList_LastBufIdx].p2dupCQIdx=param.p2dupCQIdx;
                P2_EDBUF_RingList[P2_EDBUF_RList_LastBufIdx].bufSts=ISP_ED_BUF_STATE_ENQUE;
                EDBufQueRemainNodeCnt++;

                //                                          
                if(param.p2burstQIdx==0)
                {
                    if(P2_EDBUF_MList_FirstBufIdx==P2_EDBUF_MList_LastBufIdx && P2_EDBUF_MgrList[P2_EDBUF_MList_FirstBufIdx].p2dupCQIdx==-1)
                    {   //                                
                        P2_EDBUF_MList_LastBufIdx=(P2_EDBUF_MList_LastBufIdx+1)%_MAX_SUPPORT_P2_PACKAGE_NUM_;
                        P2_EDBUF_MList_FirstBufIdx=P2_EDBUF_MList_LastBufIdx;
                    }
                    else
                    {
                        P2_EDBUF_MList_LastBufIdx=(P2_EDBUF_MList_LastBufIdx+1)%_MAX_SUPPORT_P2_PACKAGE_NUM_;
                    }
                    P2_EDBUF_MgrList[P2_EDBUF_MList_LastBufIdx].processID=param.processID;
                    P2_EDBUF_MgrList[P2_EDBUF_MList_LastBufIdx].callerID=param.callerID;
                    P2_EDBUF_MgrList[P2_EDBUF_MList_LastBufIdx].p2dupCQIdx=param.p2dupCQIdx;
                    P2_EDBUF_MgrList[P2_EDBUF_MList_LastBufIdx].dequedNum=0;
                }
            }
            //                      
            ISP_ED_BufQue_Update_GPtr(P2_EDBUF_RLIST_TAG);
            spin_unlock(&(SpinLockEDBufQueList));
            IRQ_LOG_PRINTER(_CAMSV_D_IRQ,0,_LOG_DBG);
            //                                      
            wake_up_interruptible(&WaitQueueHead_EDBuf_WaitDeque);
            break;
        case ISP_ED_BUFQUE_CTRL_WAIT_DEQUE:    //                                         
            //                                                                             
            spin_lock(&(SpinLockEDBufQueList));
            if(P2_EDBUF_RList_FirstBufIdx<=P2_EDBUF_RList_LastBufIdx)
            {
                for(i=P2_EDBUF_RList_FirstBufIdx;i<=P2_EDBUF_RList_LastBufIdx;i++)
                {
                    if((P2_EDBUF_RingList[i].processID==param.processID) && ((P2_EDBUF_RingList[i].bufSts==ISP_ED_BUF_STATE_ENQUE)||(P2_EDBUF_RingList[i].bufSts==ISP_ED_BUF_STATE_RUNNING)))
                    {
                        idx=i;
                        break;
                    }
                }
            }
            else
            {
                for(i=P2_EDBUF_RList_FirstBufIdx;i<_MAX_SUPPORT_P2_FRAME_NUM_;i++)
                {
                    if((P2_EDBUF_RingList[i].processID==param.processID) && ((P2_EDBUF_RingList[i].bufSts==ISP_ED_BUF_STATE_ENQUE)||(P2_EDBUF_RingList[i].bufSts==ISP_ED_BUF_STATE_RUNNING)))
                    {
                        idx=i;
                        break;
                    }
                }
                if(idx!=-1)
                {/*                         */}
                else
                {
                    for(i=0;i<=P2_EDBUF_RList_LastBufIdx;i++)
                    {
                        if((P2_EDBUF_RingList[i].processID==param.processID) && ((P2_EDBUF_RingList[i].bufSts==ISP_ED_BUF_STATE_ENQUE)||(P2_EDBUF_RingList[i].bufSts==ISP_ED_BUF_STATE_RUNNING)))
                        {
                            idx=i;
                            break;
                        }
                    }
                }
            }
            spin_unlock(&(SpinLockEDBufQueList));
            if(idx==-1)
            {
                LOG_ERR("Do not find match buffer (pid/cid %d/0x%x) to deque!",param.processID,param.callerID);
                ret=-EFAULT;
                return ret;
            }
            else
            {
                restTime=wait_event_interruptible_timeout(
                        WaitQueueHead_EDBuf_WaitDeque,
                        ISP_GetEDBufQueWaitDequeState(idx),
                        ISP_UsToJiffies(5000000)); //  
                if(restTime==0)
                {
                    LOG_ERR("Wait Deque fail, idx(%d) pID(%d),cID(0x%x)",idx,param.processID,param.callerID);
                    ret=-EFAULT;
                }
                else
                {
                    //                                                                             
                }
            }
            break;
        case ISP_ED_BUFQUE_CTRL_DEQUE_SUCCESS:             //                                          
        case ISP_ED_BUFQUE_CTRL_DEQUE_FAIL:                //                                       
            if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL)
            {
                LOG_DBG("dq cm(%d),pID(%d),cID(0x%x)\n",param.ctrl,param.processID,param.callerID);
            }
            spin_lock(&(SpinLockEDBufQueList));
            //                                              
            //                                                                      
            //                                                            
            //                   
            //                       
            //                   
            //
            //                                                                                                                                                                 
            //                                                                                   
            //                                                                                                                                                            
            //                                                                      
            idx2=ISP_ED_BufQue_Get_FirstMatBuf(param,P2_EDBUF_RLIST_TAG,1);
            if(param.ctrl==ISP_ED_BUFQUE_CTRL_DEQUE_SUCCESS)
            {
                P2_EDBUF_RingList[idx2].bufSts=ISP_ED_BUF_STATE_DEQUE_SUCCESS;
            }
            else
            {
                P2_EDBUF_RingList[idx2].bufSts=ISP_ED_BUF_STATE_DEQUE_FAIL;
            }
            //                                             
            idx=ISP_ED_BufQue_Get_FirstMatBuf(param,P2_EDBUF_MLIST_TAG,1);
            if(idx==-1)
            {
                spin_unlock(&(SpinLockEDBufQueList));
                LOG_ERR("ERRRRRRRRRRR findmatch index fail");
                ret=-EFAULT;
                return ret;
            }
            P2_EDBUF_MgrList[idx].dequedNum++;
            //                        
            ISP_ED_BufQue_Update_GPtr(P2_EDBUF_RLIST_TAG);
            //                                 
            if(idx2==-1)
            {
                spin_unlock(&(SpinLockEDBufQueList));
                LOG_ERR("ERRRRRRRRRRR findmatch index fail");
                ret=-EFAULT;
                return ret;
            }
            ISP_ED_BufQue_Erase(idx2,P2_EDBUF_RLIST_TAG);
            spin_unlock(&(SpinLockEDBufQueList));
            //                                                                                         
            wake_up_interruptible(&WaitQueueHead_EDBuf_WaitFrame);
            wake_up_interruptible(&WaitQueueHead_EDBuf_WaitDeque);
            break;
        case ISP_ED_BUFQUE_CTRL_WAIT_FRAME:             //                           
            spin_lock(&(SpinLockEDBufQueList));
            //                          
            idx=ISP_ED_BufQue_Get_FirstMatBuf(param,P2_EDBUF_MLIST_TAG,0);
            if(idx==-1)
            {
                spin_unlock(&(SpinLockEDBufQueList));
                LOG_ERR("could not find match buffer pID/cID (%d/0x%x)",param.processID,param.callerID);
                ret=-EFAULT;
                return ret;
            }
            //                                      
            if(P2_EDBUF_MgrList[idx].dequedNum==P2_Support_BurstQNum)
            {
                ISP_ED_BufQue_Erase(idx,P2_EDBUF_MLIST_TAG); //                                                        
                spin_unlock(&(SpinLockEDBufQueList));
                ret=0;
                LOG_DBG("Frame is alreay dequeued, return user, pd(%d/0x%x),idx(%d)",param.processID,param.callerID,idx);
                return ret;
            }
            else
            {
                spin_unlock(&(SpinLockEDBufQueList));
                if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL)
                {
                    LOG_DBG("=pd(%d/0x%x_%d)wait(%d us)=\n",param.processID,param.callerID,idx,param.timeoutUs);
                }
                //                                                         
                restTime=wait_event_interruptible_timeout(
                            WaitQueueHead_EDBuf_WaitFrame,
                            ISP_GetEDBufQueWaitFrameState(idx),
                            ISP_UsToJiffies(param.timeoutUs));
                if(restTime==0)
                {
                    LOG_ERR("Dequeue Buffer fail, rT(%d),idx(%d) pID(%d),cID(0x%x),p2SupportBNum(%d)\n",restTime,idx,param.processID,\
                        param.callerID,P2_Support_BurstQNum);
                    ret=-EFAULT;
                    break;
                }
                else
                {
                    spin_lock(&(SpinLockEDBufQueList));
                    ISP_ED_BufQue_Erase(idx,P2_EDBUF_MLIST_TAG); //                                     
                    spin_unlock(&(SpinLockEDBufQueList));
                }
            }
            break;
        case ISP_ED_BUFQUE_CTRL_WAKE_WAITFRAME:   //                                                         
            wake_up_interruptible(&WaitQueueHead_EDBuf_WaitFrame);
            break;
        case ISP_ED_BUFQUE_CTRL_CLAER_ALL:           //                                 
            spin_lock(&(SpinLockEDBufQueList));
            for(i=0;i<_MAX_SUPPORT_P2_FRAME_NUM_;i++)
            {
                P2_EDBUF_RingList[i].processID=0x0;
                P2_EDBUF_RingList[i].callerID=0x0;
                P2_EDBUF_RingList[i].bufSts=ISP_ED_BUF_STATE_NONE;
            }
            P2_EDBUF_RList_FirstBufIdx=0;
            P2_EDBUF_RList_CurBufIdx=0;
            P2_EDBUF_RList_LastBufIdx=-1;
            //
            for(i=0;i<_MAX_SUPPORT_P2_PACKAGE_NUM_;i++)
            {
                P2_EDBUF_MgrList[i].processID=0x0;
                P2_EDBUF_MgrList[i].callerID=0x0;
                P2_EDBUF_MgrList[i].p2dupCQIdx=-1;
                P2_EDBUF_MgrList[i].dequedNum=0;
            }
            P2_EDBUF_MList_FirstBufIdx=0;
            P2_EDBUF_MList_LastBufIdx=-1;
            spin_unlock(&(SpinLockEDBufQueList));
            break;
        default:
            LOG_ERR("do not support this ctrl cmd(%d)",param.ctrl);
            break;
    }
    return ret;
}

/*                                                                              
 
                                                                               */
static MINT32 ISP_REGISTER_IRQ_USERKEY(void)
{
    int key=-1; //                                           
    int i=0;

    spin_lock(&SpinLock_UserKey);
    for(i=1;i<IRQ_USER_NUM_MAX;i++) //                                                           
{
        if(!IrqLockedUserKey[i])
    {
            key=i;
            IrqLockedUserKey[i]=1;
            break;
        }
    }
    spin_unlock(&SpinLock_UserKey);
    return key;
    }

/*                                                                              
 
                                                                               */
static MINT32 ISP_MARK_IRQ(ISP_WAIT_IRQ_STRUCT irqinfo)
    {
    eISPIrq eIrq = _IRQ;
    switch (irqinfo.UserInfo.Type){
        case ISP_IRQ_TYPE_INT_CAMSV:    eIrq = _CAMSV_IRQ;      break;
        case ISP_IRQ_TYPE_INT_CAMSV2:   eIrq = _CAMSV_D_IRQ;    break;
        case ISP_IRQ_TYPE_INT_P1_ST_D:
        case ISP_IRQ_TYPE_INT_P1_ST2_D:
        default:                        eIrq = _IRQ;
            break;
    }

    MUINT32 flags;

    //                     
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
    IspInfo.IrqInfo.MarkedFlag[irqinfo.UserInfo.UserKey][irqinfo.UserInfo.Type] |= irqinfo.UserInfo.Status;
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);

    //                   
    int idx=my_get_pow_idx(irqinfo.UserInfo.Status);

    unsigned long long  sec = 0;
    unsigned long       usec = 0;
    sec = cpu_clock(0);     //  
    do_div( sec, 1000 );    //    
    usec = do_div( sec, 1000000);    //            

    spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
    IspInfo.IrqInfo.MarkedTime_usec[irqinfo.UserInfo.UserKey][irqinfo.UserInfo.Type][idx] =(unsigned int)usec;
    IspInfo.IrqInfo.MarkedTime_sec[irqinfo.UserInfo.UserKey][irqinfo.UserInfo.Type][idx] = (unsigned int)sec;
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);

    //                               
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
    IspInfo.IrqInfo.PassedBySigCnt[irqinfo.UserInfo.UserKey][irqinfo.UserInfo.Type][idx] = 0;
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);

    LOG_VRB("[MARK]  key/type/sts/idx (%d/%d/0x%x/%d), t(%d/%d)\n",irqinfo.UserInfo.UserKey,irqinfo.UserInfo.Type,irqinfo.UserInfo.Status,idx,(int)sec,(int)usec);

    return 0;
    }


/*                                                                              
 
                                                                               */
static MINT32 ISP_GET_MARKtoQEURY_TIME(ISP_WAIT_IRQ_STRUCT* irqinfo)
{
    MINT32 Ret = 0;
    MUINT32 flags;
    struct timeval time_getrequest;
    struct timeval time_ready2return;
    
    unsigned long long  sec = 0;
    unsigned long       usec = 0;

        
    eISPIrq eIrq = _IRQ;
    
//                                        
    sec = cpu_clock(0);     //  
    do_div( sec, 1000 );    //    
    usec = do_div( sec, 1000000);    //            
    time_ready2return.tv_usec = usec;
    time_ready2return.tv_sec = sec;
    
    int idx=my_get_pow_idx(irqinfo->UserInfo.Status);

    switch (irqinfo->UserInfo.Type){
        case ISP_IRQ_TYPE_INT_CAMSV:    eIrq = _CAMSV_IRQ;      break;
        case ISP_IRQ_TYPE_INT_CAMSV2:   eIrq = _CAMSV_D_IRQ;    break;
        case ISP_IRQ_TYPE_INT_P1_ST_D:
        case ISP_IRQ_TYPE_INT_P1_ST2_D:
        default:                        eIrq = _IRQ;
            break;
    }

    spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
    if(irqinfo->UserInfo.Status & IspInfo.IrqInfo.MarkedFlag[irqinfo->UserInfo.UserKey][irqinfo->UserInfo.Type])
    {
        //
        irqinfo->TimeInfo.passedbySigcnt = IspInfo.IrqInfo.PassedBySigCnt[irqinfo->UserInfo.UserKey][irqinfo->UserInfo.Type][idx];
        //
        irqinfo->TimeInfo.tMark2WaitSig_usec = (time_ready2return.tv_usec - IspInfo.IrqInfo.MarkedTime_usec[irqinfo->UserInfo.UserKey][irqinfo->UserInfo.Type][idx]);
        irqinfo->TimeInfo.tMark2WaitSig_sec = (time_ready2return.tv_sec - IspInfo.IrqInfo.MarkedTime_sec[irqinfo->UserInfo.UserKey][irqinfo->UserInfo.Type][idx]);
        if((int)(irqinfo->TimeInfo.tMark2WaitSig_usec)<0)
        {
            irqinfo->TimeInfo.tMark2WaitSig_sec=irqinfo->TimeInfo.tMark2WaitSig_sec-1;
            if((int)(irqinfo->TimeInfo.tMark2WaitSig_sec) <0)
    {
                irqinfo->TimeInfo.tMark2WaitSig_sec=0;
    }
            irqinfo->TimeInfo.tMark2WaitSig_usec = 1*1000000+irqinfo->TimeInfo.tMark2WaitSig_usec;
    }
        //
        if(irqinfo->TimeInfo.passedbySigcnt>0)
    {
            irqinfo->TimeInfo.tLastSig2GetSig_usec = (time_ready2return.tv_usec - IspInfo.IrqInfo.LastestSigTime_usec[irqinfo->UserInfo.Type][idx]);
            irqinfo->TimeInfo.tLastSig2GetSig_sec = (time_ready2return.tv_sec - IspInfo.IrqInfo.LastestSigTime_sec[irqinfo->UserInfo.Type][idx]);
            if((int)(irqinfo->TimeInfo.tLastSig2GetSig_usec)<0)
            {
                irqinfo->TimeInfo.tLastSig2GetSig_sec=irqinfo->TimeInfo.tLastSig2GetSig_sec-1;
                if((int)(irqinfo->TimeInfo.tLastSig2GetSig_sec) <0)
            {
                    irqinfo->TimeInfo.tLastSig2GetSig_sec=0;
                }
                irqinfo->TimeInfo.tLastSig2GetSig_usec = 1*1000000+irqinfo->TimeInfo.tLastSig2GetSig_usec;
            }
            }
            else
            {
            irqinfo->TimeInfo.tLastSig2GetSig_usec=0;
            irqinfo->TimeInfo.tLastSig2GetSig_sec=0;
            }
    }
    else
    {
        LOG_WRN("plz mark irq first, userKey/Type/Status (%d/%d/0x%x)",irqinfo->UserInfo.UserKey,irqinfo->UserInfo.Type, irqinfo->UserInfo.Status);
        Ret = -EFAULT;
    }
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
    LOG_VRB(" [ISP_GET_MARKtoQEURY_TIME] user/type/idx(%d/%d/%d),mark sec/usec (%d/%d), irq sec/usec (%d/%d),query sec/usec(%d/%d),sig(%d)\n",irqinfo->UserInfo.UserKey,irqinfo->UserInfo.Type,idx,\
        IspInfo.IrqInfo.MarkedTime_sec[irqinfo->UserInfo.UserKey][irqinfo->UserInfo.Type][idx],\
         IspInfo.IrqInfo.MarkedTime_usec[irqinfo->UserInfo.UserKey][irqinfo->UserInfo.Type][idx],\
        IspInfo.IrqInfo.LastestSigTime_sec[irqinfo->UserInfo.Type][idx],\
        IspInfo.IrqInfo.LastestSigTime_usec[irqinfo->UserInfo.Type][idx],(int)time_ready2return.tv_sec,(int)time_ready2return.tv_usec,\
        IspInfo.IrqInfo.PassedBySigCnt[irqinfo->UserInfo.UserKey][irqinfo->UserInfo.Type][idx]);
    return Ret;
    }

/*                                                                              
 
                                                                               */
static MINT32 ISP_FLUSH_IRQ(ISP_WAIT_IRQ_STRUCT irqinfo)
{
    eISPIrq eIrq = _IRQ;
    switch (irqinfo.UserInfo.Type){
        case ISP_IRQ_TYPE_INT_CAMSV:    eIrq = _CAMSV_IRQ;      break;
        case ISP_IRQ_TYPE_INT_CAMSV2:   eIrq = _CAMSV_D_IRQ;    break;
        case ISP_IRQ_TYPE_INT_P1_ST_D:
        case ISP_IRQ_TYPE_INT_P1_ST2_D:
        default:                        eIrq = _IRQ;
            break;
    }
    MUINT32 flags;

    //                
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
    IspInfo.IrqInfo.Status[irqinfo.UserInfo.UserKey][irqinfo.UserInfo.Type] |= irqinfo.UserInfo.Status;
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);

    //                                                             
    wake_up_interruptible(&IspInfo.WaitQueueHead);

    return 0;
}

/*                                                                              
 
                                                                               */
static MINT32 ISP_WaitIrq(ISP_WAIT_IRQ_STRUCT* WaitIrq)
{
    MINT32 Ret = 0, Timeout = WaitIrq->Timeout;
    MUINT32 i;
    MUINT32 flags;
    eISPIrq eIrq = _IRQ;
    int cnt=0;
    //
    if(IspInfo.DebugMask & ISP_DBG_INT)
    {
        if(WaitIrq->Status& (ISP_IRQ_P1_STATUS_SOF1_INT_ST|ISP_IRQ_P1_STATUS_PASS1_DON_ST|ISP_IRQ_P1_STATUS_D_SOF1_INT_ST|ISP_IRQ_P1_STATUS_D_PASS1_DON_ST)){
            LOG_DBG("+WaitIrq Clear(%d), Type(%d), Status(0x%08X), Timeout(%d),user(%d)\n",
                        WaitIrq->Clear,
                        WaitIrq->Type,
                        WaitIrq->Status,
                        WaitIrq->Timeout,
                        WaitIrq->UserNumber);
        }
    }
    //

    switch (WaitIrq->Type){
        case ISP_IRQ_TYPE_INT_CAMSV:    eIrq = _CAMSV_IRQ;      break;
        case ISP_IRQ_TYPE_INT_CAMSV2:   eIrq = _CAMSV_D_IRQ;    break;
        case ISP_IRQ_TYPE_INT_P1_ST_D:
        case ISP_IRQ_TYPE_INT_P1_ST2_D:
        default:                        eIrq = _IRQ;
            break;
    }
    if(WaitIrq->Clear == ISP_IRQ_CLEAR_WAIT)
    {
        spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
        if(IspInfo.IrqInfo.Status[WaitIrq->UserNumber][WaitIrq->Type] & WaitIrq->Status)
        {
            //                                                                                                                                                                 
            IspInfo.IrqInfo.Status[WaitIrq->UserNumber][WaitIrq->Type] &= (~WaitIrq->Status);
        }
        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
    }
    else if(WaitIrq->Clear == ISP_IRQ_CLEAR_ALL)
    {
        spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
        //                                                                                                                                               
        IspInfo.IrqInfo.Status[WaitIrq->UserNumber][WaitIrq->Type] = 0;
        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
    }
    else if(WaitIrq->Clear == ISP_IRQ_CLEAR_STATUS){
        spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
        //                                                                                                                                               
        IspInfo.IrqInfo.Status[WaitIrq->UserNumber][WaitIrq->Type] &= (~WaitIrq->Status);
        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
        return Ret;
    }
    //
    Timeout = wait_event_interruptible_timeout(
                        IspInfo.WaitQueueHead,
                        ISP_GetIRQState(eIrq,WaitIrq->Type,WaitIrq->UserNumber, WaitIrq->Status),
                        ISP_MsToJiffies(WaitIrq->Timeout));
    //                                             
    if((Timeout !=0) && (!ISP_GetIRQState(eIrq,WaitIrq->Type,WaitIrq->UserNumber, WaitIrq->Status)))
    {
        LOG_DBG("interrupted by system signal,return value(%d),irq Type/User/Sts(0x%x/%d/0x%x)",Timeout,WaitIrq->Type,WaitIrq->UserNumber, WaitIrq->Status);
        Ret = -ERESTARTSYS;  //                                  
        goto EXIT;
    }
    
    //       
    if(Timeout == 0)
    {
        spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
        LOG_ERR("v1 ERR WaitIrq Timeout Clear(%d), Type(%d), IrqStatus(0x%08X), WaitStatus(0x%08X), Timeout(%d),user(%d)",
                                    WaitIrq->Clear,
                                    WaitIrq->Type,
                                    IspInfo.IrqInfo.Status[WaitIrq->UserNumber][WaitIrq->Type],
                                    WaitIrq->Status,
                                    WaitIrq->Timeout,
                                    WaitIrq->UserNumber);
        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
        if(WaitIrq->bDumpReg){
            ISP_DumpReg();
        }
        Ret = -EFAULT;
        goto EXIT;
    }


    //
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
    //
    if(IspInfo.DebugMask & ISP_DBG_INT)
    {
        for(i=0; i<ISP_IRQ_TYPE_AMOUNT; i++)
        {
            //                                                                   
        }
    }
    //
    IspInfo.IrqInfo.Status[WaitIrq->UserNumber][WaitIrq->Type] &= (~WaitIrq->Status);    //                                       
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
    //
    //                                                
    if( WaitIrq->Type == ISP_IRQ_TYPE_INT_P2_ST )
    {
        MUINT32 CQ_status;
        ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x6000 );
        CQ_status = ISP_RD32( ISP_IMGSYS_BASE + 0x4164 );
        switch( WaitIrq->Status )
        {
            case ISP_IRQ_P2_STATUS_PASS2A_DON_ST:
                if( (CQ_status&0x0000000F) != 0x001 )
                {
                    LOG_ERR("CQ1 not idle dbg(0x%08x 0x%08x)", \
                        ISP_RD32( ISP_IMGSYS_BASE + 0x4160 ), CQ_status );
                }
                break;
            case ISP_IRQ_P2_STATUS_PASS2B_DON_ST:
                if( (CQ_status&0x000000F0) != 0x010 )
                {
                    LOG_ERR("CQ2 not idle dbg(0x%08x 0x%08x)", \
                        ISP_RD32( ISP_IMGSYS_BASE + 0x4160 ), CQ_status );
                }
                break;
            case ISP_IRQ_P2_STATUS_PASS2C_DON_ST:
                if( (CQ_status&0x00000F00) != 0x100 )
                {
                    LOG_ERR("CQ3 not idle dbg(0x%08x 0x%08x)", \
                        ISP_RD32( ISP_IMGSYS_BASE + 0x4160 ), CQ_status );
                }
                break;
            default:
                break;
        }
    }

    EXIT:
    return Ret;
}

/*                                                                              
 
                                                                               */
static MINT32 ISP_WaitIrq_v3(ISP_WAIT_IRQ_STRUCT* WaitIrq)
{
    MINT32 Ret = 0, Timeout = WaitIrq->Timeout;
    MUINT32 i;
    MUINT32 flags;
    eISPIrq eIrq = _IRQ;
    int cnt=0;
    int idx=my_get_pow_idx(WaitIrq->UserInfo.Status);
    struct timeval time_getrequest;
    struct timeval time_ready2return;
    bool freeze_passbysigcnt = false;
    unsigned long long  sec = 0;
    unsigned long       usec = 0;


    //                                  
    sec = cpu_clock(0);     //  
    do_div( sec, 1000 );    //    
    usec = do_div( sec, 1000000);    //                
    time_getrequest.tv_usec = usec;
    time_getrequest.tv_sec = sec;    
    
    //
    if(IspInfo.DebugMask & ISP_DBG_INT)
    {
        if(WaitIrq->UserInfo.Status& (ISP_IRQ_P1_STATUS_SOF1_INT_ST|ISP_IRQ_P1_STATUS_PASS1_DON_ST|ISP_IRQ_P1_STATUS_D_SOF1_INT_ST|ISP_IRQ_P1_STATUS_D_PASS1_DON_ST)){
            if(WaitIrq->UserInfo.UserKey>0)
            {
                    LOG_DBG("+WaitIrq Clear(%d), Type(%d), Status(0x%08X), Timeout(%d/%d),user(%d)\n",
                                WaitIrq->Clear,
                                WaitIrq->UserInfo.Type,
                                WaitIrq->UserInfo.Status,
                                Timeout,WaitIrq->Timeout,
                                WaitIrq->UserInfo.UserKey);
                }
        }
    }
    //

    switch (WaitIrq->UserInfo.Type){
        case ISP_IRQ_TYPE_INT_CAMSV:    eIrq = _CAMSV_IRQ;      break;
        case ISP_IRQ_TYPE_INT_CAMSV2:   eIrq = _CAMSV_D_IRQ;    break;
        case ISP_IRQ_TYPE_INT_P1_ST_D:
        case ISP_IRQ_TYPE_INT_P1_ST2_D:
        default:                        eIrq = _IRQ;
            break;
    }
    //                   
    if(WaitIrq->Clear == ISP_IRQ_CLEAR_STATUS){
        spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
        //                                                                                                                                               
        IspInfo.IrqInfo.Status[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type] &= (~WaitIrq->UserInfo.Status);
        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
        return Ret;
    }
    else
    {
        spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
        if(WaitIrq->UserInfo.Status & IspInfo.IrqInfo.MarkedFlag[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type])
        {
            spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
            //                                                                              
            //                                                                                                                

            //
            //                                
            //                                
            //                   
            //
            //                                                 
            //
            //                                                     
            //                                                   
            //                                      
            //                                                     
            //                                                     
            //
            //                                                  
            //
            //                                                     
            //                                                     
            //                                                    
            //                                                     
            //                                                     
            //

            freeze_passbysigcnt = !(ISP_GetIRQState(eIrq,WaitIrq->UserInfo.Type,WaitIrq->UserInfo.UserKey, WaitIrq->UserInfo.Status));
        }
        else
        {
            spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
            
            if(WaitIrq->Clear == ISP_IRQ_CLEAR_WAIT)
            {
                spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
                if(IspInfo.IrqInfo.Status[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type] & WaitIrq->UserInfo.Status)
                {
                    //                                                                                                                                                                      
                    IspInfo.IrqInfo.Status[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type] &= (~WaitIrq->UserInfo.Status);
                }
                spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
            }
            else if(WaitIrq->Clear == ISP_IRQ_CLEAR_ALL)
            {
                spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
                //                                                                                                                                                    
                IspInfo.IrqInfo.Status[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type] = 0;
                spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
            }
        }
    }
    //                       
    Timeout = wait_event_interruptible_timeout(
                        IspInfo.WaitQueueHead,
                        ISP_GetIRQState(eIrq,WaitIrq->UserInfo.Type,WaitIrq->UserInfo.UserKey, WaitIrq->UserInfo.Status),
                        ISP_MsToJiffies(WaitIrq->Timeout));
    //                                             
    if((Timeout !=0) && (!ISP_GetIRQState(eIrq,WaitIrq->UserInfo.Type,WaitIrq->UserInfo.UserKey, WaitIrq->UserInfo.Status)))
    {
        LOG_DBG("interrupted by system signal,return value(%d),irq Type/User/Sts(0x%x/%d/0x%x)",Timeout,WaitIrq->UserInfo.Type,WaitIrq->UserInfo.UserKey, WaitIrq->UserInfo.Status);
        Ret = -ERESTARTSYS;  //                                  
        goto EXIT;
    }
    //       
    if(Timeout == 0)
    {
        spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
        LOG_ERR("v3 ERRRR WaitIrq Timeout(%d) Clear(%d), Type(%d), IrqStatus(0x%08X), WaitStatus(0x%08X), Timeout(%d),userKey(%d)\n",
                                    WaitIrq->Timeout,
                                    WaitIrq->Clear,
                                    WaitIrq->UserInfo.Type,
                                    IspInfo.IrqInfo.Status[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type],
                                    WaitIrq->UserInfo.Status,
                                    WaitIrq->Timeout,
                                    WaitIrq->UserInfo.UserKey);
        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
        if(WaitIrq->bDumpReg){
            ISP_DumpReg();
        }
        Ret = -EFAULT;
        goto EXIT;
    }

    //                                                                                 
//                                        
    sec = cpu_clock(0);     //  
    do_div( sec, 1000 );    //    
    usec = do_div( sec, 1000000);    //                
    time_ready2return.tv_usec = usec;
    time_ready2return.tv_sec = sec;    
        
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
    //                         
    WaitIrq->TimeInfo.tLastSig_sec=IspInfo.IrqInfo.LastestSigTime_sec[WaitIrq->UserInfo.Type][idx];
    WaitIrq->TimeInfo.tLastSig_usec=IspInfo.IrqInfo.LastestSigTime_usec[WaitIrq->UserInfo.Type][idx];
    //                  
    if(WaitIrq->UserInfo.Status & IspInfo.IrqInfo.MarkedFlag[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type])
    {
        WaitIrq->TimeInfo.tMark2WaitSig_usec = (time_getrequest.tv_usec - IspInfo.IrqInfo.MarkedTime_usec[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx]);
        WaitIrq->TimeInfo.tMark2WaitSig_sec = (time_getrequest.tv_sec - IspInfo.IrqInfo.MarkedTime_sec[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx]);
        if((int)(WaitIrq->TimeInfo.tMark2WaitSig_usec)<0)
        {
            WaitIrq->TimeInfo.tMark2WaitSig_sec=WaitIrq->TimeInfo.tMark2WaitSig_sec-1;
            if((int)(WaitIrq->TimeInfo.tMark2WaitSig_sec) <0)
            {
                WaitIrq->TimeInfo.tMark2WaitSig_sec=0;
            }
            WaitIrq->TimeInfo.tMark2WaitSig_usec = 1*1000000+WaitIrq->TimeInfo.tMark2WaitSig_usec;
        }
        //
        WaitIrq->TimeInfo.tLastSig2GetSig_usec = (time_ready2return.tv_usec - IspInfo.IrqInfo.LastestSigTime_usec[WaitIrq->UserInfo.Type][idx]);
        WaitIrq->TimeInfo.tLastSig2GetSig_sec = (time_ready2return.tv_sec - IspInfo.IrqInfo.LastestSigTime_sec[WaitIrq->UserInfo.Type][idx]);
        if((int)(WaitIrq->TimeInfo.tLastSig2GetSig_usec)<0)
        {
            WaitIrq->TimeInfo.tLastSig2GetSig_sec=WaitIrq->TimeInfo.tLastSig2GetSig_sec-1;
            if((int)(WaitIrq->TimeInfo.tLastSig2GetSig_sec) <0)
            {
                WaitIrq->TimeInfo.tLastSig2GetSig_sec=0;
            }
            WaitIrq->TimeInfo.tLastSig2GetSig_usec = 1*1000000+WaitIrq->TimeInfo.tLastSig2GetSig_usec;
        }
        //
        if(freeze_passbysigcnt)
        {
             WaitIrq->TimeInfo.passedbySigcnt = IspInfo.IrqInfo.PassedBySigCnt[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx]- 1;
        }
        else
        {
            WaitIrq->TimeInfo.passedbySigcnt = IspInfo.IrqInfo.PassedBySigCnt[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx];
        }
    }
    IspInfo.IrqInfo.Status[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type] &= (~WaitIrq->UserInfo.Status);    //                                       
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
    if(WaitIrq->UserInfo.UserKey>0)
    {
        LOG_VRB(" [WAITIRQv3]user(%d) mark sec/usec (%d/%d), last irq sec/usec (%d/%d),enterwait(%d/%d),getIRQ(%d/%d)\n",WaitIrq->UserInfo.UserKey,IspInfo.IrqInfo.MarkedTime_sec[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx],\
            IspInfo.IrqInfo.MarkedTime_usec[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx],\
           IspInfo.IrqInfo.LastestSigTime_sec[WaitIrq->UserInfo.Type][idx],\
           IspInfo.IrqInfo.LastestSigTime_usec[WaitIrq->UserInfo.Type][idx],\
           (int)(time_getrequest.tv_sec),(int)(time_getrequest.tv_usec),(int)(time_ready2return.tv_sec),(int)(time_ready2return.tv_usec));
        LOG_VRB(" [WAITIRQv3]user(%d)  sigNum(%d/%d), mark sec/usec (%d/%d), irq sec/usec (%d/%d),user(0x%x)\n",WaitIrq->UserInfo.UserKey,\
             IspInfo.IrqInfo.PassedBySigCnt[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx],WaitIrq->TimeInfo.passedbySigcnt,WaitIrq->TimeInfo.tMark2WaitSig_sec,WaitIrq->TimeInfo.tMark2WaitSig_usec,\
            WaitIrq->TimeInfo.tLastSig2GetSig_sec,WaitIrq->TimeInfo.tLastSig2GetSig_usec,WaitIrq->UserInfo.UserKey);
    }
    //
    //                                                
    if( WaitIrq->UserInfo.Type == ISP_IRQ_TYPE_INT_P2_ST )
    {
        MUINT32 CQ_status;
        ISP_WR32( ISP_IMGSYS_BASE + 0x4160, 0x6000 );
        CQ_status = ISP_RD32( ISP_IMGSYS_BASE + 0x4164 );
        switch( WaitIrq->UserInfo.Status )
        {
            case ISP_IRQ_P2_STATUS_PASS2A_DON_ST:
                if( (CQ_status&0x0000000F) != 0x001 )
                {
                    LOG_ERR("CQ1 not idle dbg(0x%08x 0x%08x)", \
                        ISP_RD32( ISP_IMGSYS_BASE + 0x4160 ), CQ_status );
                }
                break;
            case ISP_IRQ_P2_STATUS_PASS2B_DON_ST:
                if( (CQ_status&0x000000F0) != 0x010 )
                {
                    LOG_ERR("CQ2 not idle dbg(0x%08x 0x%08x)", \
                        ISP_RD32( ISP_IMGSYS_BASE + 0x4160 ), CQ_status );
                }
                break;
            case ISP_IRQ_P2_STATUS_PASS2C_DON_ST:
                if( (CQ_status&0x00000F00) != 0x100 )
                {
                    LOG_ERR("CQ3 not idle dbg(0x%08x 0x%08x)", \
                        ISP_RD32( ISP_IMGSYS_BASE + 0x4160 ), CQ_status );
                }
                break;
            default:
                break;
        }
    }

    EXIT:
    //                                                                                           
    spin_lock_irqsave(&(IspInfo.SpinLockIrq[eIrq]), flags);
    if(WaitIrq->UserInfo.Status & IspInfo.IrqInfo.MarkedFlag[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type])
    {
        IspInfo.IrqInfo.MarkedFlag[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type] &= (~WaitIrq->UserInfo.Status);
        IspInfo.IrqInfo.MarkedTime_usec[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx]= 0;
        IspInfo.IrqInfo.MarkedTime_sec[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx]= 0;
        IspInfo.IrqInfo.PassedBySigCnt[WaitIrq->UserInfo.UserKey][WaitIrq->UserInfo.Type][idx]=0;
    }
    spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[eIrq]), flags);
    
    return Ret;
}


//                       
#if defined(_debug_dma_err_)
#define bit(x) (0x1<<(x))

MUINT32 DMA_ERR[3*12] = {
    bit(1) , 0xF50043A8, 0x00000011, //    
    bit(2) , 0xF50043AC, 0x00000021, //     
    bit(4) , 0xF50043B0, 0x00000031, //    
    bit(5) , 0xF50043B4, 0x00000051, //    
    bit(6) , 0xF50043B8, 0x00000061, //    
    bit(7) , 0xF50043BC, 0x00000071, //    
    bit(8) , 0xF50043C0, 0x00000081, //     
    bit(9) , 0xF50043C4, 0x00000194, //    
    bit(10), 0xF50043C8, 0x000001a4, //     
    bit(11), 0xF50043CC, 0x000001b4, //    
    bit(12), 0xF50043D0, 0x000001c4, //     
    bit(13), 0xF50043D4, 0x000001d4, //   
};

static MINT32 DMAErrHandler()
{
    MUINT32 err_ctrl = ISP_RD32(0xF50043A4);
    LOG_DBG("err_ctrl(0x%08x)", err_ctrl);

    MUINT32 i = 0;

    MUINT32 *pErr = DMA_ERR;
    for( i = 0; i < 12 ; i++ )
    {
        MUINT32 addr = 0;
#if 1
        if( err_ctrl & (*pErr) )
        {
            ISP_WR32(0xF5004160, pErr[2]);
            addr = pErr[1];

            LOG_DBG("(0x%08x, 0x%08x), dbg(0x%08x, 0x%08x)",
                addr, ISP_RD32(addr),
                ISP_RD32(0xF5004160), ISP_RD32(0xF5004164));
        }
#else
        addr = pErr[1];
        MUINT32 status = ISP_RD32( addr );

        if( status & 0x0000FFFF )
        {
            ISP_WR32(0xF5004160, pErr[2]);
            addr = pErr[1];

            LOG_DBG("(0x%08x, 0x%08x), dbg(0x%08x, 0x%08x)",
                addr, status,
                ISP_RD32(0xF5004160), ISP_RD32(0xF5004164));
        }
#endif
        pErr = pErr + 3;
    }

}
#endif

//                                                                             
//          
static __tcmfunc irqreturn_t ISP_Irq_CAMSV(
    MINT32  Irq,
    void*   DeviceId)
{
    //                   
    MUINT32 i=0;
    //              
    MUINT32 IrqStatus_CAMSV;
    volatile CQ_RTBC_FBC fbc;
    volatile MUINT32 curr_pa;
    struct timeval time_frmb;
    MUINT32 idx=0,k=0;
    unsigned long long  sec = 0;
    unsigned long       usec = 0;  
    
//                                
    sec = cpu_clock(0);     //  
    do_div( sec, 1000 );    //    
    usec = do_div( sec, 1000000);    //                
    time_frmb.tv_usec = usec;
    time_frmb.tv_sec = sec;      
    
    fbc.Reg_val = ISP_RD32(ISP_REG_ADDR_CAMSV_IMGO_FBC);
    curr_pa = ISP_RD32(ISP_REG_ADDR_IMGO_SV_BASE_ADDR);
    spin_lock(&(IspInfo.SpinLockIrq[_CAMSV_IRQ]));
    IrqStatus_CAMSV=(ISP_RD32(ISP_REG_ADDR_CAMSV_INT) & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_CAMSV]));
   
    for(i=0;i<IRQ_USER_NUM_MAX;i++)
    {
        //                                       
        IspInfo.IrqInfo.Status[i][ISP_IRQ_TYPE_INT_CAMSV] |=(IrqStatus_CAMSV & IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV]);

        //                                                         
        if(IspInfo.IrqInfo.MarkedFlag[i][ISP_IRQ_TYPE_INT_CAMSV] & IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV])
        {
            for(k=0;k<32;k++)
    {
                if((IrqStatus_CAMSV & IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV]) & (1<<k))
                {
                    idx=my_get_pow_idx(1<<k);
                    IspInfo.IrqInfo.LastestSigTime_usec[ISP_IRQ_TYPE_INT_CAMSV][idx]=(unsigned int)time_frmb.tv_usec;
                    IspInfo.IrqInfo.LastestSigTime_sec[ISP_IRQ_TYPE_INT_CAMSV][idx]=(unsigned int) time_frmb.tv_sec;
                    IspInfo.IrqInfo.PassedBySigCnt[i][ISP_IRQ_TYPE_INT_CAMSV][k]++;
                }
            }
        }
        else
        {/*                                                                  */ }
    }
    if(IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_CAMSV] & IrqStatus_CAMSV) {
        IRQ_LOG_KEEPER(_CAMSV_IRQ,m_CurrentPPB,_LOG_ERR,CAMSV_TAG"Err IRQ, Type(%d), Status(0x%x)\n",ISP_IRQ_TYPE_INT_CAMSV,IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_CAMSV] & IrqStatus_CAMSV);
    }
    if(IspInfo.DebugMask & ISP_DBG_INT) {
        IRQ_LOG_KEEPER(_CAMSV_IRQ,m_CurrentPPB,_LOG_INF,CAMSV_TAG"Type(%d), IrqStatus(0x%x | 0x%x)\n",ISP_IRQ_TYPE_INT_CAMSV,IspInfo.IrqInfo.Status[ISP_IRQ_USER_ISPDRV][ISP_IRQ_TYPE_INT_CAMSV], IrqStatus_CAMSV);
    }


    if(IrqStatus_CAMSV & ISP_IRQ_CAMSV_STATUS_PASS1_DON_ST){
        if(IspInfo.DebugMask & ISP_DBG_BUF_CTRL) {
            IRQ_LOG_KEEPER(_CAMSV_IRQ,m_CurrentPPB,_LOG_INF,CAMSV_TAG"DONE_%d_%d(0x%x,0x%x,0x%x,0x%x)\n", \
                        (sof_count[_CAMSV])?(sof_count[_CAMSV]-1):(sof_count[_CAMSV]),\
                        ((ISP_RD32(ISP_REG_ADDR_CAMSV_TG_INTER_ST)&0x00FF0000)>>16),\
                        (unsigned int)(fbc.Reg_val), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_BASE_ADDR), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_XSIZE), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_YSIZE));
        }
#if 0   //                      
        unsigned long long  sec = 0;
        unsigned long       usec = 0;
        if(IspInfo.DebugMask & ISP_DBG_INT){
            sec = cpu_clock(0);     //  
            do_div( sec, 1000 );    //    
            usec = do_div( sec, 1000000);    //            
        }
        ISP_CAMSV_DONE_Buf_Time(_camsv_imgo_,sec,usec);
#else
        ISP_CAMSV_DONE_Buf_Time(_camsv_imgo_,fbc,0,0);
#endif
    }
    if( IrqStatus_CAMSV & ISP_IRQ_CAMSV_STATUS_TG_SOF1_ST) {
        //                              
        if(fbc.Bits.FB_NUM == fbc.Bits.FBC_CNT){
            gSof_camsvdone[0] = 1;
            IRQ_LOG_KEEPER(_CAMSV_IRQ,m_CurrentPPB,_LOG_INF,CAMSV_TAG"Lost done_%d\n",sof_count[_CAMSV]);

        }else {
            gSof_camsvdone[0] = 0;
        }
#ifdef _rtbc_buf_que_2_0_
        {
            MUINT32 z;
            if (1 == mFwRcnt.bLoadBaseAddr[_CAMSV_IRQ]) {
                if(pstRTBuf->ring_buf[_camsv_imgo_].active) {
                    IRQ_LOG_KEEPER(_CAMSV_IRQ,m_CurrentPPB,_LOG_INF,CAMSV_TAG"wr2Phy,");
                    ISP_WR32(ISP_REG_ADDR_IMGO_SV_BASE_ADDR,pstRTBuf->ring_buf[_camsv_imgo_].data[pstRTBuf->ring_buf[_camsv_imgo_].start].base_pAddr);
                }
                mFwRcnt.bLoadBaseAddr[_CAMSV_IRQ] = 0;
            }
            //                              
            for(z=0;z<=mFwRcnt.curIdx[_CAMSV_IRQ];z++) {
                if(1 == mFwRcnt.INC[_CAMSV_IRQ][z]) {
                    mFwRcnt.INC[_CAMSV_IRQ][z] = 0;
                    //            
                    fbc.Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_CAMSV_IMGO_FBC,fbc.Reg_val);
                    fbc.Bits.RCNT_INC = 0;
                    ISP_WR32(ISP_REG_ADDR_CAMSV_IMGO_FBC,fbc.Reg_val);
                    IRQ_LOG_KEEPER(_CAMSV_IRQ,m_CurrentPPB,_LOG_INF,CAMSV_TAG"RCNT_INC\n");
                }
                else {
                    mFwRcnt.curIdx[_CAMSV_IRQ] = 0;
                    break;
                }
            }
        }
#endif
        if(IspInfo.DebugMask & ISP_DBG_INT) {
                CQ_RTBC_FBC _fbc_chk;
                _fbc_chk.Reg_val = ISP_RD32(ISP_REG_ADDR_CAMSV_IMGO_FBC);//                                    
                IRQ_LOG_KEEPER(_CAMSV_IRQ,m_CurrentPPB,_LOG_INF,CAMSV_TAG"SOF_%d_%d(0x%x,0x%x,0x%x,0x%x)\n", \
                        sof_count[_CAMSV],\
                        ((ISP_RD32(ISP_REG_ADDR_CAMSV_TG_INTER_ST)&0x00FF0000)>>16),\
                        _fbc_chk.Reg_val, \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_BASE_ADDR), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_XSIZE), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_YSIZE));
                
                if(_fbc_chk.Bits.WCNT != fbc.Bits.WCNT)
                        IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"sv1:SW ISR right on next hw p1_done(0x%x_0x%x)\n",_fbc_chk.Reg_val,fbc.Reg_val); 
        }
        {
            unsigned long long  sec;
            unsigned long       usec;
            ktime_t             time;

            time = ktime_get();     //  
            sec = time.tv64;
            do_div( sec, 1000 );    //    
            usec = do_div( sec, 1000000);    //            

            ISP_CAMSV_SOF_Buf_Get(_camsv_imgo_,fbc,curr_pa, sec,usec,gSof_camsvdone[0]);
        }
    }
    spin_unlock(&(IspInfo.SpinLockIrq[_CAMSV_IRQ]));
#ifdef ISR_LOG_ON
    IRQ_LOG_PRINTER(_CAMSV_IRQ,m_CurrentPPB,_LOG_INF);
    IRQ_LOG_PRINTER(_CAMSV_IRQ,m_CurrentPPB,_LOG_ERR);
#endif
    wake_up_interruptible(&IspInfo.WaitQueueHead);

    return IRQ_HANDLED;
}
static __tcmfunc irqreturn_t ISP_Irq_CAMSV2(
    MINT32  Irq,
    void*   DeviceId)
{
    //                   
    MUINT32 i=0;
    //              
    MUINT32 IrqStatus_CAMSV2;
    volatile CQ_RTBC_FBC fbc;
    volatile MUINT32 curr_pa;
    struct timeval time_frmb;
    MUINT32 idx=0,k=0;
    unsigned long long  sec = 0;
    unsigned long       usec = 0;
            
//                                
    sec = cpu_clock(0);     //  
    do_div( sec, 1000 );    //    
    usec = do_div( sec, 1000000);    //                
    time_frmb.tv_usec = usec;
    time_frmb.tv_sec = sec;  
        
    fbc.Reg_val = ISP_RD32(ISP_REG_ADDR_CAMSV2_IMGO_FBC);
    curr_pa = ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR);
    spin_lock(&(IspInfo.SpinLockIrq[_CAMSV_D_IRQ]));
    IrqStatus_CAMSV2=(ISP_RD32(ISP_REG_ADDR_CAMSV2_INT) & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV2]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_CAMSV2]));

    for(i=0;i<IRQ_USER_NUM_MAX;i++)
    {
        //                                       
        IspInfo.IrqInfo.Status[i][ISP_IRQ_TYPE_INT_CAMSV2] |=(IrqStatus_CAMSV2 & IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV2]);

        //                                                         
        if(IspInfo.IrqInfo.MarkedFlag[i][ISP_IRQ_TYPE_INT_CAMSV2] & IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV2])
    {
            for(k=0;k<32;k++)
    {
                if((IrqStatus_CAMSV2 & IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV2]) & (1<<k))
    {
                    idx=my_get_pow_idx(1<<k);
                    IspInfo.IrqInfo.LastestSigTime_usec[ISP_IRQ_TYPE_INT_CAMSV2][idx]=(unsigned int)time_frmb.tv_usec;
                    IspInfo.IrqInfo.LastestSigTime_sec[ISP_IRQ_TYPE_INT_CAMSV2][idx]=(unsigned int) time_frmb.tv_sec;
                    IspInfo.IrqInfo.PassedBySigCnt[i][ISP_IRQ_TYPE_INT_CAMSV2][k]++;
                }
            }
        }
        else
        {/*                                                                  */ }
    }
    if(IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_CAMSV2] & IrqStatus_CAMSV2)
    {
        IRQ_LOG_KEEPER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_ERR,CAMSV2_TAG"Error IRQ, Type(%d), Status(0x%08x)\n",ISP_IRQ_TYPE_INT_CAMSV2,IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_CAMSV2] & IrqStatus_CAMSV2);
    }
    if(IspInfo.DebugMask & ISP_DBG_INT)
    {
        IRQ_LOG_KEEPER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_INF,CAMSV2_TAG"Type(%d), IrqStatus(0x%x | 0x%08x)\n",ISP_IRQ_TYPE_INT_CAMSV2,IspInfo.IrqInfo.Status[ISP_IRQ_USER_ISPDRV][ISP_IRQ_TYPE_INT_CAMSV2], (unsigned int)(IrqStatus_CAMSV2));
    }
    if(IrqStatus_CAMSV2 & ISP_IRQ_CAMSV2_STATUS_PASS1_DON_ST){
        if(IspInfo.DebugMask & ISP_DBG_INT) {
            IRQ_LOG_KEEPER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_INF,CAMSV2_TAG"fbc(0x%x)",(unsigned int)(fbc.Reg_val));

            IRQ_LOG_KEEPER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_INF,CAMSV2_TAG"DONE_%d_%d(0x%x,0x%x,0x%x,0x%x,camsv support no inner addr)\n", \
                        (sof_count[_CAMSV_D])?(sof_count[_CAMSV_D]-1):(sof_count[_CAMSV_D]),\
                        ((ISP_RD32(ISP_REG_ADDR_CAMSV_TG2_INTER_ST)&0x00FF0000)>>16),\
                        (unsigned int)(fbc.Reg_val), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_XSIZE), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_YSIZE));
        }
#if 0 //                      
        unsigned long long  sec;
        unsigned long       usec;
        sec = cpu_clock(0);     //  
        do_div( sec, 1000 );    //    
        usec = do_div( sec, 1000000);    //            

        ISP_CAMSV_DONE_Buf_Time(_camsv2_imgo_,sec,usec);
#else
        ISP_CAMSV_DONE_Buf_Time(_camsv2_imgo_,fbc, 0,0);
#endif
    }
    if( IrqStatus_CAMSV2 & ISP_IRQ_CAMSV2_STATUS_TG_SOF1_ST) {
        //                              
        if(fbc.Bits.FB_NUM == fbc.Bits.FBC_CNT){
            gSof_camsvdone[1] = 1;
            IRQ_LOG_KEEPER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_INF,CAMSV2_TAG"Lost done %d",sof_count[_CAMSV_D]);

        }else {
            gSof_camsvdone[1] = 0;
        }
#ifdef _rtbc_buf_que_2_0_
        {
            MUINT32 z;
            if (1 == mFwRcnt.bLoadBaseAddr[_CAMSV_D_IRQ]) {
                if(pstRTBuf->ring_buf[_camsv2_imgo_].active) {
                    IRQ_LOG_KEEPER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_INF,CAMSV2_TAG"wr2Phy,");
                    ISP_WR32(ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR,pstRTBuf->ring_buf[_camsv2_imgo_].data[pstRTBuf->ring_buf[_camsv2_imgo_].start].base_pAddr);
                }
                mFwRcnt.bLoadBaseAddr[_CAMSV_D_IRQ] = 0;
            }
            //                              
            for(z=0;z<=mFwRcnt.curIdx[_CAMSV_D_IRQ];z++) {
                if(1 == mFwRcnt.INC[_CAMSV_D_IRQ][z]) {
                    mFwRcnt.INC[_CAMSV_D_IRQ][z] = 0;
                    //           
                    fbc.Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_CAMSV2_IMGO_FBC,fbc.Reg_val);
                    fbc.Bits.RCNT_INC = 0;
                    ISP_WR32(ISP_REG_ADDR_CAMSV2_IMGO_FBC,fbc.Reg_val);
                    IRQ_LOG_KEEPER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_INF,CAMSV2_TAG"RCNT_INC\n");
                }
                else {
                    mFwRcnt.curIdx[_CAMSV_D_IRQ] = 0;
                    break;
                }
            }
        }
#endif

        if(IspInfo.DebugMask & ISP_DBG_INT) {
            CQ_RTBC_FBC _fbc_chk;
                _fbc_chk.Reg_val = ISP_RD32(ISP_REG_ADDR_CAMSV2_IMGO_FBC);//                                    
                
            IRQ_LOG_KEEPER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_INF,CAMSV2_TAG"SOF_%d_%d(0x%x,0x%x,0x%x,0x%x,camsv support no inner addr)\n", \
                        sof_count[_CAMSV_D],\
                        ((ISP_RD32(ISP_REG_ADDR_CAMSV_TG2_INTER_ST)&0x00FF0000)>>16),\
                        _fbc_chk.Reg_val, \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_BASE_ADDR), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_XSIZE), \
                        ISP_RD32(ISP_REG_ADDR_IMGO_SV_D_YSIZE));
            
                if(_fbc_chk.Bits.WCNT != fbc.Bits.WCNT)
                        IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"sv2:SW ISR right on next hw p1_done(0x%x_0x%x)\n",_fbc_chk.Reg_val,fbc.Reg_val);
        }
        {
            unsigned long long  sec;
            unsigned long       usec;
            ktime_t             time;

            time = ktime_get();     //  
            sec = time.tv64;
            do_div( sec, 1000 );    //    
            usec = do_div( sec, 1000000);    //            

            ISP_CAMSV_SOF_Buf_Get(_camsv2_imgo_,fbc,curr_pa, sec,usec,gSof_camsvdone[1]);
        }
    }
    spin_unlock(&(IspInfo.SpinLockIrq[_CAMSV_D_IRQ]));
    //                         
#ifdef ISR_LOG_ON
    IRQ_LOG_PRINTER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_INF);
    IRQ_LOG_PRINTER(_CAMSV_D_IRQ,m_CurrentPPB,_LOG_ERR);
#endif
    wake_up_interruptible(&IspInfo.WaitQueueHead);
    return IRQ_HANDLED;
}

//                                                                             
#ifdef AP_DONT_STOP_PREVIEW
void ISP_ResumeHWFBC(MUINT32* irqstat,MUINT32 irqlen, CQ_RTBC_FBC* pFbc, MUINT32 fbclen)
{
    int regWCNT;
    int regRCNT;
    int regFBCCNT;
    int i;
    int backup_WCNT;
    int backup_RCNT;
    int backup_FBCCNT;
    int diffRCnt;
   
    if((irqstat[ISP_IRQ_TYPE_INT_P1_ST] & (ISP_IRQ_P1_STATUS_PASS1_DON_ST)))
    {
        if(pstRTBuf->ring_buf[_rrzo_].active)
        {
            regWCNT = (pFbc[1].Reg_val & 0x0f000000) >> 24;
            regRCNT = (pFbc[1].Reg_val & 0x00f00000) >> 20;
            regFBCCNT = (pFbc[1].Reg_val & 0x0000000f);
            backup_WCNT = (g_backupReg.CAM_CTL_RRZO_FBC & 0x0f000000)  >> 24;
            backup_RCNT = (g_backupReg.CAM_CTL_RRZO_FBC & 0x00f00000)  >> 20;
            backup_FBCCNT = (g_backupReg.CAM_CTL_RRZO_FBC & 0x0000000F);
        }
        else if(pstRTBuf->ring_buf[_imgo_].active)
        {
            regWCNT = (pFbc[0].Reg_val & 0x0f000000) >> 24;
            regRCNT = (pFbc[0].Reg_val & 0x00f00000) >> 20;
            regFBCCNT = (pFbc[0].Reg_val & 0x0000000f);
            backup_WCNT = (g_backupReg.CAM_CTL_IMGO_FBC & 0x0f000000)  >> 24;
            backup_RCNT = (g_backupReg.CAM_CTL_IMGO_FBC & 0x00f00000)  >> 20;
            backup_FBCCNT = (g_backupReg.CAM_CTL_IMGO_FBC & 0x0000000f);
        }
        LOG_DBG("backup WCNT(%d), RCNT(%d), FBC_CNT(%d)\n)", backup_WCNT, backup_RCNT, backup_FBCCNT);
        LOG_DBG("Register WCNT(%d), RCNT(%d), FBC_CNT(%d)\n", regWCNT, regRCNT, regFBCCNT);
        //                                                    
        if ((backup_WCNT == backup_RCNT) && (backup_FBCCNT > 0))
        {
            if (backup_WCNT != CONSST_FBC_CNT_INIT)
            {
                //                           
                if (backup_RCNT>regRCNT)
                {
                    pFbc[0].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_IMGO_FBC,pFbc[0].Reg_val);
                    pFbc[1].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_RRZO_FBC,pFbc[1].Reg_val);
                }
            }

            if (regFBCCNT == backup_FBCCNT)
            {
                //                         
                //                                  
                {
                    for(i=0;i<IRQ_USER_NUM_MAX;i++)
                    {
                        //                                       
                        //                                                    
                        IspInfo.IrqInfo.Status[i][ISP_IRQ_TYPE_INT_P1_ST] |= ISP_IRQ_P1_STATUS_PESUDO_P1_DON_ST;
                    }
                }
                //                                                                        
                ISP_WR32(ISP_ADDR + 0x20, (g_backupReg.CAM_CTL_CQ_EN & 0xffff7bff));
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7300), g_backupReg.CAM_IMGO_BASE_ADDR);
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7320), g_backupReg.CAM_RRZO_BASE_ADDR);
                bResumeSignal = 0;
                wake_up_interruptible(&IspInfo.WaitQueueHead);
            }   

        }
        else
        {
            //                                                                                 
            //                                    
            if (regWCNT == backup_WCNT)
            {   
                if (regRCNT>=backup_RCNT)
                {
                    diffRCnt = (regRCNT - backup_RCNT);
                }
                else
                {
                    diffRCnt = (backup_RCNT - regRCNT);
                }
                for (i=0;i< diffRCnt; i++)
                {
                    pFbc[0].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_IMGO_FBC,pFbc[0].Reg_val);
                    pFbc[1].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_RRZO_FBC,pFbc[1].Reg_val);
                }
            
                //                         
                //                                  
                {
                    for(i=0;i<IRQ_USER_NUM_MAX;i++)
                    {
                        //                                       
                        //                                                    
                        IspInfo.IrqInfo.Status[i][ISP_IRQ_TYPE_INT_P1_ST] |= ISP_IRQ_P1_STATUS_PESUDO_P1_DON_ST;
                    }
                }
                //                                                                        
                ISP_WR32(ISP_ADDR + 0x20, (g_backupReg.CAM_CTL_CQ_EN & 0xffff7bff));
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7300), g_backupReg.CAM_IMGO_BASE_ADDR);
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7320), g_backupReg.CAM_RRZO_BASE_ADDR);
                bResumeSignal = 0;
                wake_up_interruptible(&IspInfo.WaitQueueHead);
            }

        }
    }

    if(irqstat[ISP_IRQ_TYPE_INT_P1_ST_D] & (ISP_IRQ_P1_STATUS_D_PASS1_DON_ST))
    {
        if(pstRTBuf->ring_buf[_rrzo_d_].active)
        {
            regWCNT = (pFbc[3].Reg_val & 0x0f000000) >> 24;
            regRCNT = (pFbc[3].Reg_val & 0x00f00000) >> 20;
            regFBCCNT = (pFbc[3].Reg_val & 0x0000000f);
            backup_WCNT = (g_backupReg.CAM_CTL_RRZO_D_FBC & 0x0f000000)  >> 24;
            backup_RCNT = (g_backupReg.CAM_CTL_RRZO_D_FBC & 0x00f00000)  >> 20;
            backup_FBCCNT = (g_backupReg.CAM_CTL_RRZO_D_FBC & 0x0000000f);
        }
        else if(pstRTBuf->ring_buf[_imgo_d_].active)
        {
            regWCNT = (pFbc[2].Reg_val & 0x0f000000) >> 24;
            regRCNT = (pFbc[2].Reg_val & 0x00f00000) >> 20;
            regFBCCNT = (pFbc[2].Reg_val & 0x0000000f);
            backup_WCNT = (g_backupReg.CAM_CTL_IMGO_D_FBC & 0x0f000000)  >> 24;
            backup_RCNT = (g_backupReg.CAM_CTL_IMGO_D_FBC & 0x00f00000)  >> 20;
            backup_FBCCNT = (g_backupReg.CAM_CTL_IMGO_D_FBC & 0x0000000f);
        }
        LOG_DBG("backup WCNT(%d), RCNT(%d), FBC_CNT(%d)\n)", backup_WCNT, backup_RCNT, backup_FBCCNT);
        LOG_DBG("Register WCNT(%d), RCNT(%d), FBC_CNT(%d)\n", regWCNT, regRCNT, regFBCCNT);
        if ((backup_WCNT == backup_RCNT) && (backup_FBCCNT > 0))
        {
            if (backup_WCNT != CONSST_FBC_CNT_INIT)
            {
                //                           
                if (backup_RCNT>regRCNT)
                {
                    pFbc[2].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_IMGO_D_FBC,pFbc[2].Reg_val);
                    pFbc[3].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_RRZO_D_FBC,pFbc[3].Reg_val);
                }
            }

            if (regFBCCNT == backup_FBCCNT)
            {
                //                         
                //                                  
                {
                    for(i=0;i<IRQ_USER_NUM_MAX;i++)
                    {
                        //                                       
                        //                                                    
                        IspInfo.IrqInfo.Status[i][ISP_IRQ_TYPE_INT_P1_ST_D] |= ISP_IRQ_P1_STATUS_PESUDO_P1_DON_ST;
                    }
                }
                //                                                                                        
                ISP_WR32(ISP_ADDR + 0x20, (g_backupReg.CAM_CTL_CQ_EN & 0xffff7bff));
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74D4), g_backupReg.CAM_IMGO_D_BASE_ADDR);
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74F4), g_backupReg.CAM_RRZO_D_BASE_ADDR);
                bResumeSignal = 0;
                wake_up_interruptible(&IspInfo.WaitQueueHead);
            }   

        }
        else
        {
            //                                                                                 
            //                                    

            if (regWCNT == backup_WCNT)
            {   
                if (regRCNT>=backup_RCNT)
                {
                    diffRCnt = (regRCNT - backup_RCNT);
                }
                else
                {
                    diffRCnt = (backup_RCNT - regRCNT);
                }
                for (i=0;i< diffRCnt; i++)
                {
                    pFbc[2].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_IMGO_D_FBC,pFbc[2].Reg_val);
                    pFbc[3].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_RRZO_D_FBC,pFbc[3].Reg_val);
                }

                //                         
                //                                  
                {
                    for(i=0;i<IRQ_USER_NUM_MAX;i++)
                    {
                        //                                       
                        //                                                    
                        IspInfo.IrqInfo.Status[i][ISP_IRQ_TYPE_INT_P1_ST_D] |= ISP_IRQ_P1_STATUS_D_PESUDO_P1_DON_ST;
                    }
                }
                //                                                                                        
                ISP_WR32(ISP_ADDR + 0x20, g_backupReg.CAM_CTL_CQ_EN);
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74D4), g_backupReg.CAM_IMGO_D_BASE_ADDR);
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74F4), g_backupReg.CAM_RRZO_D_BASE_ADDR);
                bResumeSignal = 0;
                wake_up_interruptible(&IspInfo.WaitQueueHead);
            }
        }
    }
}
#endif
/*                                                                              
 
                                                                               */
static __tcmfunc irqreturn_t ISP_Irq_CAM(
    MINT32  Irq,
    void*   DeviceId)
{
    //                                  

//                    
    MUINT32 i;
    MUINT32 IrqStatus[ISP_IRQ_TYPE_AMOUNT];
    //                          
    volatile CQ_RTBC_FBC p1_fbc[4];
    volatile MUINT32 curr_pa[4];//                 
    MUINT32 cur_v_cnt = 0;
    MUINT32 d_cur_v_cnt = 0;
    MUINT32 j=0,idx=0,k=0;
    struct timeval time_frmb;
    unsigned long long  sec = 0;
    unsigned long       usec = 0;    
#if 0
    if( (ISP_RD32(ISP_REG_ADDR_TG_VF_CON) & 0x1) == 0x0) {
        LOG_INF("before vf:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",ISP_RD32(ISP_REG_ADDR_INT_P1_ST),\
        ISP_RD32(ISP_REG_ADDR_INT_P1_ST2),\
        ISP_RD32(ISP_REG_ADDR_INT_P1_ST_D),\
        ISP_RD32(ISP_REG_ADDR_INT_P1_ST2_D),\
        ISP_RD32(ISP_REG_ADDR_INT_P2_ST),\
        ISP_RD32(ISP_REG_ADDR_INT_STATUSX),\
        ISP_RD32(ISP_REG_ADDR_INT_STATUS2X),\
        ISP_RD32(ISP_REG_ADDR_INT_STATUS3X));
    }
#endif
    //
//                                
    sec = cpu_clock(0);     //  
    do_div( sec, 1000 );    //    
    usec = do_div( sec, 1000000);    //                
    time_frmb.tv_usec = usec;
    time_frmb.tv_sec = sec; 
    
    //                
    //                                        
    IrqStatus[ISP_IRQ_TYPE_INT_P1_ST]     = (ISP_RD32(ISP_REG_ADDR_INT_P1_ST)    & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_P1_ST]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_P1_ST]));
    IrqStatus[ISP_IRQ_TYPE_INT_P1_ST2]    = (ISP_RD32(ISP_REG_ADDR_INT_P1_ST2));//                                                                                                       
    IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D]   = (ISP_RD32(ISP_REG_ADDR_INT_P1_ST_D)  & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_P1_ST_D]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_P1_ST_D]));
    IrqStatus[ISP_IRQ_TYPE_INT_P1_ST2_D]  = (ISP_RD32(ISP_REG_ADDR_INT_P1_ST2_D) & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_P1_ST2_D]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_P1_ST2_D]));
#ifdef __debugused__ 
    IrqStatus[ISP_IRQ_TYPE_INT_P2_ST]     = (ISP_RD32(ISP_REG_ADDR_INT_P2_ST)    & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_P2_ST]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_P2_ST]));
#endif
    //                                
    IrqStatus[ISP_IRQ_TYPE_INT_STATUSX]    = (ISP_RD32(ISP_REG_ADDR_INT_STATUSX)   & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_STATUSX]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_STATUSX]));
    IrqStatus[ISP_IRQ_TYPE_INT_STATUS2X]   = (ISP_RD32(ISP_REG_ADDR_INT_STATUS2X)  & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_STATUS2X]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_STATUS2X]));
    IrqStatus[ISP_IRQ_TYPE_INT_STATUS3X]   = (ISP_RD32(ISP_REG_ADDR_INT_STATUS3X)  & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_STATUS3X]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_STATUS3X]));
    //                                          
    //                                              

    p1_fbc[0].Reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_FBC);
    p1_fbc[1].Reg_val = ISP_RD32(ISP_REG_ADDR_RRZO_FBC);    
    p1_fbc[2].Reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_D_FBC);
    p1_fbc[3].Reg_val = ISP_RD32(ISP_REG_ADDR_RRZO_D_FBC);
    curr_pa[0] = ISP_RD32(ISP_REG_ADDR_IMGO_BASE_ADDR); 
    curr_pa[1] = ISP_RD32(ISP_REG_ADDR_RRZO_BASE_ADDR); 
    curr_pa[2] = ISP_RD32(ISP_REG_ADDR_IMGO_D_BASE_ADDR);
    curr_pa[3] = ISP_RD32(ISP_REG_ADDR_RRZO_D_BASE_ADDR);
#ifdef __debugused__ 
    LOG_INF("irq status:0x%x,0x%x,0x%x,0x%x,0x%x\n",\
            IrqStatus[ISP_IRQ_TYPE_INT_P1_ST],\
            IrqStatus[ISP_IRQ_TYPE_INT_P1_ST2],\
            IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D],\
            IrqStatus[ISP_IRQ_TYPE_INT_P1_ST2_D],\
            IrqStatus[ISP_IRQ_TYPE_INT_P2_ST]);
#endif

#ifdef AP_DONT_STOP_PREVIEW
    if (1 == bResumeSignal)
    {
        ISP_ResumeHWFBC(IrqStatus,ISP_IRQ_TYPE_AMOUNT , p1_fbc, 4);
        return IRQ_HANDLED;
    }
#endif

#if 1   //                    
        #define STATUSX_WARNING (ISP_IRQ_STATUSX_ESFKO_ERR_ST|ISP_IRQ_STATUSX_RRZO_ERR_ST|ISP_IRQ_STATUSX_LCSO_ERR_ST|ISP_IRQ_STATUSX_AAO_ERR_ST|ISP_IRQ_STATUSX_IMGO_ERR_ST|ISP_IRQ_STATUSX_RRZO_ERR_ST)
        IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_STATUSX] = IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_STATUSX] & (~(ISP_IRQ_STATUSX_IMGO_DROP_FRAME_ST|ISP_IRQ_STATUSX_RRZO_DROP_FRAME_ST));
        //                                                    
        if(IrqStatus[ISP_IRQ_TYPE_INT_STATUSX] & IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_STATUSX]){
            if((IrqStatus[ISP_IRQ_TYPE_INT_STATUSX] & ISP_IRQ_STATUSX_DMA_ERR_ST) || (IrqStatus[ISP_IRQ_TYPE_INT_STATUS2X] & ISP_IRQ_STATUSX_DMA_ERR_ST)){
                g_bDmaERR_p1_d = MTRUE;
                g_bDmaERR_p1 = MTRUE;
                g_bDmaERR_deepDump= MFALSE;
                ISP_DumpDmaDeepDbg();
            }

            //                                                                     
            //                  
            // 
            //                                                         
            //                                                          
            //                                         
            // 
            if(IrqStatus[ISP_IRQ_TYPE_INT_STATUSX] & (~STATUSX_WARNING)){
                LOG_ERR("ISP INT ERR_P1 0x%x\n",IrqStatus[ISP_IRQ_TYPE_INT_STATUSX]);
                g_ISPIntErr[_IRQ] |= IrqStatus[ISP_IRQ_TYPE_INT_STATUSX];
            }
            if(IrqStatus[ISP_IRQ_TYPE_INT_STATUS2X] & (~STATUSX_WARNING)){
                LOG_ERR("ISP INT ERR_P1_D 0x%x\n",IrqStatus[ISP_IRQ_TYPE_INT_STATUS2X]);
                g_ISPIntErr[_IRQ_D] |= IrqStatus[ISP_IRQ_TYPE_INT_STATUS2X];
            }
        }
        //                                                                                                          
        //                                                                                                                                                                 
        //                                                                                                                                                                 
        if( (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_PASS1_DON_ST) || (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_SOF1_INT_ST) )
            cur_v_cnt = ((ISP_RD32(ISP_REG_ADDR_TG_INTER_ST)&0x00FF0000)>>16);
        if( (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_D_PASS1_DON_ST) || (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_D_SOF1_INT_ST) )
            d_cur_v_cnt = ((ISP_RD32(ISP_REG_ADDR_TG2_INTER_ST)&0x00FF0000)>>16);
        if( (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_PASS1_DON_ST) && (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_SOF1_INT_ST) ){
            if( cur_v_cnt != sof_count[_PASS1])
                LOG_ERR("isp sof_don block, %d_%d\n",cur_v_cnt,sof_count[_PASS1]);
        }
#endif

     #if 0
    /sensor interface would use another isr id
    //                            
    IrqStatus[ISP_IRQ_TYPE_INT_SENINF1]    = (ISP_RD32(ISP_REG_ADDR_SENINF1_INT)   & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_SENINF1]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_SENINF1]));
    IrqStatus[ISP_IRQ_TYPE_INT_SENINF2]    = (ISP_RD32(ISP_REG_ADDR_SENINF2_INT)   & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_SENINF2]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_SENINF2]));
    IrqStatus[ISP_IRQ_TYPE_INT_SENINF3]    = (ISP_RD32(ISP_REG_ADDR_SENINF3_INT)   & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_SENINF3]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_SENINF3]));
    IrqStatus[ISP_IRQ_TYPE_INT_SENINF4]    = (ISP_RD32(ISP_REG_ADDR_SENINF4_INT)   & (IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_SENINF4]|IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_SENINF4]));
    #endif


    //                                                                    
    //              
    //                                
    //                                                                                                  
    spin_lock(&(IspInfo.SpinLockIrq[_IRQ]));
    if(IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_PASS1_DON_ST)
    {
#ifdef _rtbc_buf_que_2_0_
        unsigned long long  sec;
        unsigned long       usec;
        sec = cpu_clock(0);     //  
        do_div( sec, 1000 );    //    
        usec = do_div( sec, 1000000);    //            
        //                                                                                         
        IspInfo.IrqInfo.LastestSigTime_usec[ISP_IRQ_TYPE_INT_P1_ST][10]=(unsigned int)(usec);
        IspInfo.IrqInfo.LastestSigTime_sec[ISP_IRQ_TYPE_INT_P1_ST][10]=(unsigned int) (sec);
        ISP_DONE_Buf_Time(_IRQ,p1_fbc, 0,0);//                      
        if(IspInfo.DebugMask & ISP_DBG_INT) {
            IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"P1_DON_%d(0x%x,0x%x)\n",(sof_count[_PASS1])?(sof_count[_PASS1]-1):(sof_count[_PASS1]),(unsigned int)(p1_fbc[0].Reg_val), \
                                                 (unsigned int)(p1_fbc[1].Reg_val));
        }
#else
    #if defined(_rtbc_use_cq0c_)
        if(IspInfo.DebugMask & ISP_DBG_INT) {
            IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"P1_DON_%d(0x%x,0x%x)\n",(sof_count[_PASS1])?(sof_count[_PASS1]-1):(sof_count[_PASS1]),(unsigned int)(p1_fbc[0].Reg_val), \
                                                 (unsigned int)(p1_fbc[1].Reg_val));
        }
    #else
        //                                                                   
        unsigned long long  sec;
        unsigned long       usec;
        sec = cpu_clock(0);     //  
        do_div( sec, 1000 );    //    
        usec = do_div( sec, 1000000);    //            

        ISP_DONE_Buf_Time(p1_fbc, sec,usec);
        /*                      */
        //            
        //
    #endif
#endif
    }

    //                        
    //                        
    if(IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_SOF1_INT_ST)
    {
        MUINT32 _dmaport = 0;
        if(pstRTBuf->ring_buf[_imgo_].active)
            _dmaport = 0;
        else if(pstRTBuf->ring_buf[_rrzo_].active)
            _dmaport = 1;
        else
            LOG_ERR("no main dma port opened at SOF\n");
        //                                                    
        if(p1_fbc[_dmaport].Bits.FB_NUM == p1_fbc[_dmaport].Bits.FBC_CNT){
            sof_pass1done[0] = 1;
            #ifdef _rtbc_buf_que_2_0_
                //                                 
                //                                 
                //                                                                     
            #endif

            IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"Lost p1 done_%d (0x%x): ",sof_count[_PASS1],cur_v_cnt);
        }else {
            sof_pass1done[0] = 0;            
            if(p1_fbc[_dmaport].Bits.FB_NUM == (p1_fbc[_dmaport].Bits.FBC_CNT+1)){
                sof_pass1done[0] = 2;
            }
        } 
#ifdef _rtbc_buf_que_2_0_
        {
            MUINT32 z;
            if (1 == mFwRcnt.bLoadBaseAddr[_IRQ]) {
                if(pstRTBuf->ring_buf[_imgo_].active) {
                    IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF," p1_%d:wr2Phy_0x%x: ",_imgo_,pstRTBuf->ring_buf[_imgo_].data[pstRTBuf->ring_buf[_imgo_].start].base_pAddr);
                    ISP_WR32(ISP_REG_ADDR_IMGO_BASE_ADDR,pstRTBuf->ring_buf[_imgo_].data[pstRTBuf->ring_buf[_imgo_].start].base_pAddr);
                }
                if(pstRTBuf->ring_buf[_rrzo_].active) {
                    IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF," p1_%d:wr2Phy_0x%x: ",_rrzo_,pstRTBuf->ring_buf[_rrzo_].data[pstRTBuf->ring_buf[_rrzo_].start].base_pAddr);
                    ISP_WR32(ISP_REG_ADDR_RRZO_BASE_ADDR,pstRTBuf->ring_buf[_rrzo_].data[pstRTBuf->ring_buf[_rrzo_].start].base_pAddr);
                }
                mFwRcnt.bLoadBaseAddr[_IRQ] = 0;
            }
            //                              
            for(z=0;z<=mFwRcnt.curIdx[_IRQ];z++) {
                //                                            
                if(1 == mFwRcnt.INC[_IRQ][z]) {
                    mFwRcnt.INC[_IRQ][z] = 0;
                    p1_fbc[0].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_IMGO_FBC,p1_fbc[0].Reg_val);
                    p1_fbc[1].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_RRZO_FBC,p1_fbc[1].Reg_val);
                    if(IspInfo.DebugMask & ISP_DBG_INT)
                        IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF," p1:RCNT_INC: ");
                }
                else {
                    //                                                                                                                                                           
                    //                                                                                                          
                    mFwRcnt.curIdx[_IRQ] = 0;
                    break;
                }
            }
        }
#endif
        if(IspInfo.DebugMask & ISP_DBG_INT) {
            CQ_RTBC_FBC _fbc_chk[2];//                                                                               
            _fbc_chk[0].Reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_FBC);//                                    
            _fbc_chk[1].Reg_val = ISP_RD32(ISP_REG_ADDR_RRZO_FBC);
            IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"P1_SOF_%d_%d(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x)\n",sof_count[_PASS1],cur_v_cnt,(unsigned int)(_fbc_chk[0].Reg_val), \
                                                 (unsigned int)(_fbc_chk[1].Reg_val), \
                                                 ISP_RD32(ISP_REG_ADDR_IMGO_BASE_ADDR), \
                                                 ISP_RD32(ISP_REG_ADDR_RRZO_BASE_ADDR), \
                                                 ISP_RD32(ISP_INNER_REG_ADDR_IMGO_YSIZE), \
                                                 ISP_RD32(ISP_INNER_REG_ADDR_RRZO_YSIZE),\
                                                 ISP_RD32(ISP_REG_ADDR_TG_MAGIC_0));
            //                
            if(pstRTBuf->ring_buf[_imgo_].active){
                if(_fbc_chk[0].Bits.WCNT != p1_fbc[0].Bits.WCNT)
                        IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"imgo:SW ISR right on next hw p1_done(0x%x_0x%x)\n",_fbc_chk[0].Reg_val,p1_fbc[0].Reg_val);                
            }
            else if(pstRTBuf->ring_buf[_rrzo_].active){
                if(_fbc_chk[1].Bits.WCNT != p1_fbc[1].Bits.WCNT)
                        IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"rrzo:SW ISR right on next hw p1_done(0x%x_0x%x)\n",_fbc_chk[1].Reg_val,p1_fbc[1].Reg_val);                
            }
        }
        {
            unsigned long long  sec;
            unsigned long       usec;
            ktime_t             time;

            time = ktime_get();     //  
            sec = time.tv64;
#ifdef T_STAMP_2_0
            if(g1stSof[_IRQ] == MTRUE){
                m_T_STAMP.T_ns = sec;
            }
            if(m_T_STAMP.fps > SlowMotion){
                m_T_STAMP.fcnt++;
                if(g1stSof[_IRQ] == MFALSE){
                    m_T_STAMP.T_ns += ((unsigned long long)m_T_STAMP.interval_us*1000);
                    if(m_T_STAMP.fcnt  == m_T_STAMP.fps){
                        m_T_STAMP.fcnt = 0;
                        m_T_STAMP.T_ns += ((unsigned long long)m_T_STAMP.compensation_us*1000);
                    }
                }
                sec = m_T_STAMP.T_ns;
            }
#endif
            do_div( sec, 1000 );    //    
            usec = do_div( sec, 1000000);    //            
            //                                                                                  
            IspInfo.IrqInfo.LastestSigTime_usec[ISP_IRQ_TYPE_INT_P1_ST][12]=(unsigned int)(sec);
            IspInfo.IrqInfo.LastestSigTime_sec[ISP_IRQ_TYPE_INT_P1_ST][12]=(unsigned int) (usec);
            if(sof_pass1done[0] == 1)
	            ISP_SOF_Buf_Get(_IRQ, p1_fbc, curr_pa, sec,usec,MTRUE);    
            else
	            ISP_SOF_Buf_Get(_IRQ, p1_fbc, curr_pa, sec,usec,MFALSE);
        }
    }
    spin_unlock(&(IspInfo.SpinLockIrq[_IRQ]));
#if 0 //                                   
    if (MFALSE == bSlowMotion)
    {
        if(IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_PASS1_DON_ST)
        {
            LOG_INF("1D_%d\n",(sof_count[_PASS1])?(sof_count[_PASS1]-1):(sof_count[_PASS1]));
        }  
        if(IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & ISP_IRQ_P1_STATUS_SOF1_INT_ST)
        {
            LOG_INF("1S_%d\n",(sof_count[_PASS1])?(sof_count[_PASS1]-1):(sof_count[_PASS1]));
        }
    }
#endif
    spin_lock(&(IspInfo.SpinLockIrq[_IRQ]));
    //         
    if(IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D] & ISP_IRQ_P1_STATUS_D_PASS1_DON_ST)
    {
        if(IspInfo.DebugMask & ISP_DBG_INT) {
            IRQ_LOG_KEEPER(_IRQ_D,m_CurrentPPB,_LOG_INF,"P1_D_DON_%d_%d(0x%x,0x%x)\n",(sof_count[_PASS1_D])?(sof_count[_PASS1_D]-1):(sof_count[_PASS1_D]),d_cur_v_cnt,(unsigned int)(p1_fbc[2].Reg_val), \
                                                 (unsigned int)(p1_fbc[3].Reg_val));
        }
#ifdef _rtbc_buf_que_2_0_
        unsigned long long  sec;
        unsigned long       usec;
        sec = cpu_clock(0);     //  
        do_div( sec, 1000 );    //    
        usec = do_div( sec, 1000000);    //            
        //                                                                                         
        IspInfo.IrqInfo.LastestSigTime_usec[ISP_IRQ_TYPE_INT_P1_ST_D][10]=(unsigned int)(usec);
        IspInfo.IrqInfo.LastestSigTime_sec[ISP_IRQ_TYPE_INT_P1_ST_D][10]=(unsigned int) (sec);
        ISP_DONE_Buf_Time(_IRQ_D,p1_fbc, 0,0);//                      
#endif
    }

    //        
    if(IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D] & ISP_IRQ_P1_STATUS_D_SOF1_INT_ST)
    {
        MUINT32 _dmaport = 0;
        if(pstRTBuf->ring_buf[_imgo_d_].active)
            _dmaport = 2;
        else if(pstRTBuf->ring_buf[_rrzo_d_].active)
            _dmaport = 3;
        else
            LOG_ERR("no main dma port opened at SOF_D\n");
        //                                                   
        if(p1_fbc[_dmaport].Bits.FB_NUM == p1_fbc[_dmaport].Bits.FBC_CNT){
            sof_pass1done[1] = 1;
            #ifdef _rtbc_buf_que_2_0_
                //                                   
                //                                   
                //                                                                         
            #endif

            IRQ_LOG_KEEPER(_IRQ_D,m_CurrentPPB,_LOG_INF,"Lost p1_d done_%d (0x%x): ",sof_count[_PASS1_D],d_cur_v_cnt);
        }else {
            sof_pass1done[1] = 0;
            if(p1_fbc[_dmaport].Bits.FB_NUM == (p1_fbc[_dmaport].Bits.FBC_CNT-1))
                sof_pass1done[1] = 2;
        }
#ifdef _rtbc_buf_que_2_0_
        {
            MUINT32 z;
            if (1 == mFwRcnt.bLoadBaseAddr[_IRQ_D]) {
                if(pstRTBuf->ring_buf[_imgo_d_].active) {
                    IRQ_LOG_KEEPER(_IRQ_D,m_CurrentPPB,_LOG_INF,"p1_d_%d:wr2Phy: ",_imgo_d_);
                    ISP_WR32(ISP_REG_ADDR_IMGO_D_BASE_ADDR,pstRTBuf->ring_buf[_imgo_d_].data[pstRTBuf->ring_buf[_imgo_d_].start].base_pAddr);
                }
                if(pstRTBuf->ring_buf[_rrzo_d_].active) {
                    IRQ_LOG_KEEPER(_IRQ_D,m_CurrentPPB,_LOG_INF,"p1_d_%d:wr2Phy: ",_rrzo_d_);
                    ISP_WR32(ISP_REG_ADDR_RRZO_D_BASE_ADDR,pstRTBuf->ring_buf[_rrzo_d_].data[pstRTBuf->ring_buf[_rrzo_d_].start].base_pAddr);
                }
                mFwRcnt.bLoadBaseAddr[_IRQ_D] = 0;
            }
            //                              
            for(z=0;z<=mFwRcnt.curIdx[_IRQ_D];z++) {
                if(1 == mFwRcnt.INC[_IRQ_D][z]) {
                    mFwRcnt.INC[_IRQ_D][z] = 0;
                    p1_fbc[2].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_IMGO_D_FBC,p1_fbc[2].Reg_val);
                    p1_fbc[3].Bits.RCNT_INC = 1;
                    ISP_WR32(ISP_REG_ADDR_RRZO_D_FBC,p1_fbc[3].Reg_val);
                    if(IspInfo.DebugMask & ISP_DBG_INT)
                        IRQ_LOG_KEEPER(_IRQ_D,m_CurrentPPB,_LOG_INF,"p1_d:RCNT_INC: ");
                }
                else {
                    mFwRcnt.curIdx[_IRQ_D] = 0;
                    break;
                }
            }
        }
#endif

        if(IspInfo.DebugMask & ISP_DBG_INT) {
            CQ_RTBC_FBC _fbc_chk[2];//                                                                               
            _fbc_chk[0].Reg_val = ISP_RD32(ISP_REG_ADDR_IMGO_D_FBC);
            _fbc_chk[1].Reg_val = ISP_RD32(ISP_REG_ADDR_RRZO_D_FBC);
            IRQ_LOG_KEEPER(_IRQ_D,m_CurrentPPB,_LOG_INF,"P1_D_SOF_%d_%d(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x)\n",sof_count[_PASS1_D],d_cur_v_cnt,(unsigned int)(_fbc_chk[0].Reg_val), \
                                                 (unsigned int)(_fbc_chk[1].Reg_val), \
                                                 ISP_RD32(ISP_REG_ADDR_IMGO_D_BASE_ADDR), \
                                                 ISP_RD32(ISP_REG_ADDR_RRZO_D_BASE_ADDR), \
                                                 ISP_RD32(ISP_INNER_REG_ADDR_IMGO_D_YSIZE), \
                                                 ISP_RD32(ISP_INNER_REG_ADDR_RRZO_D_YSIZE),\
                                                 ISP_RD32(ISP_REG_ADDR_TG2_MAGIC_0));
            //                
            if(pstRTBuf->ring_buf[_imgo_d_].active){
                if(_fbc_chk[0].Bits.WCNT != p1_fbc[2].Bits.WCNT)
                        IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"imgo_d:SW ISR right on next hw p1_done(0x%x_0x%x)\n",_fbc_chk[0].Reg_val,p1_fbc[0].Reg_val);                
            }
            else if(pstRTBuf->ring_buf[_rrzo_d_].active){
                if(_fbc_chk[1].Bits.WCNT != p1_fbc[3].Bits.WCNT)
                        IRQ_LOG_KEEPER(_IRQ,m_CurrentPPB,_LOG_INF,"rrzo_d:SW ISR right on next hw p1_done(0x%x_0x%x)\n",_fbc_chk[1].Reg_val,p1_fbc[1].Reg_val);                
            }
        }
        {
            unsigned long long  sec;
            unsigned long       usec;
            ktime_t             time;
            //
            time = ktime_get();     //  
            sec = time.tv64;
            do_div( sec, 1000 );    //    
            usec = do_div( sec, 1000000);    //            
            //                                                                                  
            IspInfo.IrqInfo.LastestSigTime_usec[ISP_IRQ_TYPE_INT_P1_ST_D][12]=(unsigned int)(sec);
            IspInfo.IrqInfo.LastestSigTime_sec[ISP_IRQ_TYPE_INT_P1_ST_D][12]=(unsigned int) (usec);
            //
            if(sof_pass1done[1] == 1)
	            ISP_SOF_Buf_Get(_IRQ_D,p1_fbc,curr_pa,sec,usec,MTRUE);
            else
	            ISP_SOF_Buf_Get(_IRQ_D,p1_fbc,curr_pa,sec,usec,MFALSE);
        }
    }
    //
    //                                                          
    
    for(j=0;j<ISP_IRQ_TYPE_AMOUNT;j++)
    {
        for(i=0;i<IRQ_USER_NUM_MAX;i++)
        {
            //                                       
            IspInfo.IrqInfo.Status[i][j] |= (IrqStatus[j] & IspInfo.IrqInfo.Mask[j]);

            //                                                         
            if(IspInfo.IrqInfo.MarkedFlag[i][j] & IspInfo.IrqInfo.Mask[j])
    {
                for(k=0;k<32;k++)
        {
                    if((IrqStatus[j] & IspInfo.IrqInfo.Mask[j]) & (1<<k))
                    {
                        idx=my_get_pow_idx(1<<k);
                        IspInfo.IrqInfo.LastestSigTime_usec[j][idx]=(unsigned int)time_frmb.tv_usec;
                        IspInfo.IrqInfo.LastestSigTime_sec[j][idx]=(unsigned int) time_frmb.tv_sec;
                        IspInfo.IrqInfo.PassedBySigCnt[i][j][k]++;
                    }
                }
            }
            else
            {/*                                                                  */ }
        }
    }
    spin_unlock(&(IspInfo.SpinLockIrq[_IRQ]));
#if 0 //                                   
    if (MFALSE == bSlowMotion)
    {
        if(IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D] & ISP_IRQ_P1_STATUS_D_PASS1_DON_ST)
        {
            LOG_INF("2D_%d\n",(sof_count[_PASS1])?(sof_count[_PASS1]-1):(sof_count[_PASS1]));
        }  
        if(IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D] & ISP_IRQ_P1_STATUS_D_SOF1_INT_ST)
        {
            LOG_INF("2S_%d\n",(sof_count[_PASS1])?(sof_count[_PASS1]-1):(sof_count[_PASS1]));
        }
    }
#endif
    //                         
#ifdef ISR_LOG_ON
    //                                            
    //                                            

    //                                              
    //                                              
#endif
    //
    wake_up_interruptible(&IspInfo.WaitQueueHead);

    if((IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & (ISP_IRQ_P1_STATUS_PASS1_DON_ST)) ||
        (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & (ISP_IRQ_P1_STATUS_SOF1_INT_ST)) ||
        (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D] & (ISP_IRQ_P1_STATUS_D_PASS1_DON_ST)) ||
        (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D] & (ISP_IRQ_P1_STATUS_D_SOF1_INT_ST)))
    {
        tasklet_schedule(&isp_tasklet);
    }
    //                                                                                
    if((IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & (ISP_IRQ_P1_STATUS_VS1_INT_ST)) & (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D] & (ISP_IRQ_P1_STATUS_D_VS1_INT_ST)))
    {
        IspInfo.TimeLog.Vd = ISP_JiffiesToMs(jiffies);
        schedule_work(&IspInfo.ScheduleWorkVD);
        tasklet_schedule(&IspTaskletVD);
    }
    //                                                                              
    if((IrqStatus[ISP_IRQ_TYPE_INT_P1_ST] & (ISP_IRQ_P1_STATUS_EXPDON1_ST)) & (IrqStatus[ISP_IRQ_TYPE_INT_P1_ST_D] & (ISP_IRQ_P1_STATUS_D_EXPDON1_ST)))
    {
        IspInfo.TimeLog.Expdone = ISP_JiffiesToMs(jiffies);
        schedule_work(&IspInfo.ScheduleWorkEXPDONE);
        tasklet_schedule(&IspTaskletEXPDONE);
    }

//                    

    //
    //                                  
    return IRQ_HANDLED;
}


static void ISP_TaskletFunc(unsigned long data)
{
    if (MFALSE == bSlowMotion)
    {
        if (MTRUE == bRawEn)
        {
            LOG_INF("tks_%d",(sof_count[_PASS1])?(sof_count[_PASS1]-1):(sof_count[_PASS1]));
            IRQ_LOG_PRINTER(_IRQ,m_CurrentPPB,_LOG_INF);
            LOG_INF("tke_%d",(sof_count[_PASS1])?(sof_count[_PASS1]-1):(sof_count[_PASS1]));
        }
        if (MTRUE == bRawDEn)
        {
            LOG_INF("dtks_%d",(sof_count[_PASS1_D])?(sof_count[_PASS1_D]-1):(sof_count[_PASS1_D]));
            IRQ_LOG_PRINTER(_IRQ_D,m_CurrentPPB,_LOG_INF);
            LOG_INF("dtke_%d",(sof_count[_PASS1_D])?(sof_count[_PASS1_D]-1):(sof_count[_PASS1_D]));
        }
    }
    else
    {
        IRQ_LOG_PRINTER(_IRQ,m_CurrentPPB,_LOG_INF);
        IRQ_LOG_PRINTER(_IRQ_D,m_CurrentPPB,_LOG_INF);
    }
}

/*                                                                              
 
                                                                               */
static long ISP_ioctl(
    struct file*    pFile,
    unsigned int    Cmd,
    unsigned long   Param)
{
    MINT32 Ret = 0;
    //
    MBOOL   HoldEnable = MFALSE;
    MUINT32 DebugFlag[2] = {0},pid = 0;
    ISP_REG_IO_STRUCT       RegIo;
    ISP_HOLD_TIME_ENUM      HoldTime;
    ISP_WAIT_IRQ_STRUCT     IrqInfo;
    ISP_READ_IRQ_STRUCT     ReadIrq;
    ISP_CLEAR_IRQ_STRUCT    ClearIrq;
    ISP_USER_INFO_STRUCT*   pUserInfo;
    ISP_ED_BUFQUE_STRUCT    edQueBuf;
    MUINT32                 regScenInfo_value=0xa5a5a5a5;
    MINT32                  burstQNum;
    MUINT32 flags;
    int userKey=-1;
    //
    if(pFile->private_data == NULL)
    {
        LOG_WRN("private_data is NULL,(process, pid, tgid)=(%s, %d, %d)", current->comm , current->pid, current->tgid);
        return -EFAULT;
    }
    //
    pUserInfo = (ISP_USER_INFO_STRUCT*)(pFile->private_data);
    //
    switch(Cmd)
    {
        case ISP_GET_DROP_FRAME:
            if(copy_from_user(&DebugFlag[0], (void*)Param, sizeof(MUINT32)) != 0){
                LOG_ERR("get irq from user fail");
                Ret = -EFAULT;
            }
            else{
                switch(DebugFlag[0]){
                    case _IRQ:
                        spin_lock_irqsave(&(IspInfo.SpinLockIrq[_IRQ]), flags);
                        DebugFlag[1] = sof_pass1done[0];
                        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[_IRQ]), flags);
                        break;
                    case _IRQ_D:
                        spin_lock_irqsave(&(IspInfo.SpinLockIrq[_IRQ]), flags);
                        DebugFlag[1] = sof_pass1done[1];
                        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[_IRQ]), flags);
                        break;
                    case _CAMSV_IRQ:
                        spin_lock_irqsave(&(IspInfo.SpinLockIrq[_CAMSV_IRQ]), flags);
                        DebugFlag[1] = gSof_camsvdone[0];
                        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[_CAMSV_IRQ]), flags);
                        break;
                    case _CAMSV_D_IRQ:
                        spin_lock_irqsave(&(IspInfo.SpinLockIrq[_CAMSV_D_IRQ]), flags);
                        DebugFlag[1] = gSof_camsvdone[1];
                        spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[_CAMSV_D_IRQ]), flags);
                        break;
                    default:
                        LOG_ERR("err TG(0x%x)\n",DebugFlag[0]);
                        Ret = -EFAULT;
                        break;
                }
                if(copy_to_user((void*)Param, &DebugFlag[1], sizeof(MUINT32)) != 0){
                    LOG_ERR("copy to user fail");
                    Ret = -EFAULT;
                }
            }
            break;
        case ISP_GET_INT_ERR:
            if(copy_to_user((void*)Param, g_ISPIntErr, sizeof(MUINT32)*_IRQ_MAX) != 0){
                LOG_ERR("get int err fail\n");
            }
            break;
        case ISP_GET_DMA_ERR:
            if(copy_to_user((void*)Param, &g_DmaErr_p1[0], sizeof(MUINT32)*nDMA_ERR) != 0){
                LOG_ERR("get dma_err fail\n");
            }
            break;
        case ISP_GET_CUR_SOF:
            if(copy_from_user(&DebugFlag[0], (void*)Param, sizeof(MUINT32)) != 0){
                LOG_ERR("get cur sof from user fail");
                Ret = -EFAULT;
            }
            else{
                switch(DebugFlag[0]){
                    case _IRQ:
                        DebugFlag[1] = ((ISP_RD32(ISP_REG_ADDR_TG_INTER_ST)&0x00FF0000)>>16);
                        break;
                    case _IRQ_D:
                        DebugFlag[1] = ((ISP_RD32(ISP_REG_ADDR_TG2_INTER_ST)&0x00FF0000)>>16);
                        break;
                    case _CAMSV_IRQ:
                        DebugFlag[1] = ((ISP_RD32(ISP_REG_ADDR_CAMSV_TG_INTER_ST)&0x00FF0000)>>16);
                        break;
                    case _CAMSV_D_IRQ:
                        DebugFlag[1] = ((ISP_RD32(ISP_REG_ADDR_CAMSV_TG2_INTER_ST)&0x00FF0000)>>16);
                        break;
                    default:
                        LOG_ERR("err TG(0x%x)\n",DebugFlag[0]);
                        Ret = -EFAULT;
                        break;
                }
            }                
            if(copy_to_user((void*)Param, &DebugFlag[1], sizeof(MUINT32)) != 0){
                LOG_ERR("copy to user fail");
                Ret = -EFAULT;
            }            
            break;
        case ISP_RESET_CAM_P1:
        {
            spin_lock(&(IspInfo.SpinLockIsp));
            ISP_Reset(ISP_REG_SW_CTL_RST_CAM_P1);
            spin_unlock(&(IspInfo.SpinLockIsp));
            break;
        }
        case ISP_RESET_CAM_P2:
        {
            spin_lock(&(IspInfo.SpinLockIsp));
            ISP_Reset(ISP_REG_SW_CTL_RST_CAM_P2);
            spin_unlock(&(IspInfo.SpinLockIsp));
            break;
        }
        case ISP_RESET_CAMSV:
        {
            spin_lock(&(IspInfo.SpinLockIsp));
            ISP_Reset(ISP_REG_SW_CTL_RST_CAMSV);
            spin_unlock(&(IspInfo.SpinLockIsp));
            break;
        }
        case ISP_RESET_CAMSV2:
        {
            spin_lock(&(IspInfo.SpinLockIsp));
            ISP_Reset(ISP_REG_SW_CTL_RST_CAMSV2);
            spin_unlock(&(IspInfo.SpinLockIsp));
            break;
        }
        case ISP_RESET_BUF:
        {
            spin_lock_bh(&(IspInfo.SpinLockHold));
            ISP_ResetBuf();
            spin_unlock_bh(&(IspInfo.SpinLockHold));
            break;
        }
        case ISP_READ_REGISTER:
        {
            if(copy_from_user(&RegIo, (void*)Param, sizeof(ISP_REG_IO_STRUCT)) == 0)
            {
                Ret = ISP_ReadReg(&RegIo);
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        }
        case ISP_WRITE_REGISTER:
        {
            if(copy_from_user(&RegIo, (void*)Param, sizeof(ISP_REG_IO_STRUCT)) == 0)
            {
                Ret = ISP_WriteReg(&RegIo);
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        }
        case ISP_HOLD_REG_TIME:
        {
            if(copy_from_user(&HoldTime, (void*)Param, sizeof(ISP_HOLD_TIME_ENUM)) == 0)
            {
                spin_lock(&(IspInfo.SpinLockIsp));
                Ret = ISP_SetHoldTime(HoldTime);
                spin_unlock(&(IspInfo.SpinLockIsp));
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        }
        case ISP_HOLD_REG:
        {
            if(copy_from_user(&HoldEnable, (void*)Param, sizeof(MBOOL)) == 0)
            {
                Ret = ISP_EnableHoldReg(HoldEnable);
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        }
        case ISP_WAIT_IRQ:
        {
            if(copy_from_user(&IrqInfo, (void*)Param, sizeof(ISP_WAIT_IRQ_STRUCT)) == 0)
            {
                //
                if((IrqInfo.Type >= ISP_IRQ_TYPE_AMOUNT) ||(IrqInfo.Type<0))
                {
                    Ret = -EFAULT;
                    LOG_ERR("invalid type(%d)",IrqInfo.Type);
                    goto EXIT;
                }
                if((IrqInfo.UserNumber>=ISP_IRQ_USER_MAX)||(IrqInfo.UserNumber<0))
                {
                    LOG_ERR("errUserEnum(%d)",IrqInfo.UserNumber);
                    Ret = -EFAULT;
                    goto EXIT;
                }
                
                //           
                if(IrqInfo.UserNumber > 0)
                {   //       
                    Ret = ISP_WaitIrq(&IrqInfo);
                }
#if 1 
                else
                {   //                                                                                                   
                    //  
                    if((IrqInfo.UserInfo.Type >= ISP_IRQ_TYPE_AMOUNT) ||(IrqInfo.UserInfo.Type<0))
                    {
                        Ret = -EFAULT;
                        LOG_ERR("invalid type(%d)",IrqInfo.UserInfo.Type);
                        goto EXIT;
                    }
                    if((IrqInfo.UserInfo.UserKey>=IRQ_USER_NUM_MAX) || (IrqInfo.UserInfo.UserKey<=0) || (IrqInfo.UserNumber==0))
                    {
                        //                                                                                       
                        userKey=0;    //                                                                          
                    }
                    if((IrqInfo.UserInfo.UserKey>0) && (IrqInfo.UserInfo.UserKey<IRQ_USER_NUM_MAX))
                    {   //                                                                                      
                        userKey=IrqInfo.UserInfo.UserKey;
                    }
                    IrqInfo.UserInfo.UserKey=userKey;
                    
                    Ret = ISP_WaitIrq_v3(&IrqInfo);
                }               
#endif
                if(copy_to_user((void*)Param, &IrqInfo, sizeof(ISP_WAIT_IRQ_STRUCT)) != 0)
                {
                    LOG_ERR("copy_to_user failed\n");
                    Ret = -EFAULT;
                }
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        }
        case ISP_READ_IRQ:
        {
            if(copy_from_user(&ReadIrq, (void*)Param, sizeof(ISP_READ_IRQ_STRUCT)) == 0)
            {
                LOG_DBG("ISP_READ_IRQ Type(%d)",ReadIrq.Type);
                if((ReadIrq.Type >= ISP_IRQ_TYPE_AMOUNT) ||(ReadIrq.Type<0))
                {
                    Ret = -EFAULT;
                    LOG_ERR("invalid type(%d)",ReadIrq.Type);
                    goto EXIT;
                }
                eISPIrq irqT=_IRQ;
                //
                if((ReadIrq.UserNumber>=ISP_IRQ_USER_MAX)||(ReadIrq.UserNumber<0))
                {
                    LOG_ERR("errUserEnum(%d)",ReadIrq.UserNumber);
                    Ret = -EFAULT;
                    goto EXIT;
                }
                //
                switch(ReadIrq.Type) {
                    case ISP_IRQ_TYPE_INT_CAMSV:    irqT = _CAMSV_IRQ;   break;
                    case ISP_IRQ_TYPE_INT_CAMSV2:   irqT = _CAMSV_D_IRQ; break;
                    default:                        irqT = _IRQ;         break;
                }
                spin_lock_irqsave(&(IspInfo.SpinLockIrq[irqT]), flags);
                ReadIrq.Status = IspInfo.IrqInfo.Status[ReadIrq.UserNumber][ReadIrq.Type];
                spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[irqT]), flags);
                //
                if(copy_to_user((void*)Param, &ReadIrq, sizeof(ISP_READ_IRQ_STRUCT)) != 0)
                {
                    LOG_ERR("copy_to_user failed");
                    Ret = -EFAULT;
                }
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        }
        case ISP_CLEAR_IRQ:
        {
            if(copy_from_user(&ClearIrq, (void*)Param, sizeof(ISP_CLEAR_IRQ_STRUCT)) == 0)
            {
                LOG_DBG("ISP_CLEAR_IRQ Type(%d)",ClearIrq.Type);
                
                if((ClearIrq.Type >= ISP_IRQ_TYPE_AMOUNT) ||(ClearIrq.Type<0))
                {
                    Ret = -EFAULT;
                    LOG_ERR("invalid type(%d)",ClearIrq.Type);
                    goto EXIT;
                }
                eISPIrq irqT;
                //
                if((ClearIrq.UserNumber>=ISP_IRQ_USER_MAX) ||(ClearIrq.UserNumber<0))
                {
                    LOG_ERR("errUserEnum(%d)",ClearIrq.UserNumber);
                    Ret = -EFAULT;
                    goto EXIT;
                }
                //
                switch(ClearIrq.Type) {
                    case ISP_IRQ_TYPE_INT_CAMSV:    irqT = _CAMSV_IRQ;   break;
                    case ISP_IRQ_TYPE_INT_CAMSV2:   irqT = _CAMSV_D_IRQ; break;
                    default:                        irqT = _IRQ;         break;
                }
                LOG_DBG("ISP_CLEAR_IRQ:Type(%d),Status(0x%08X),IrqStatus(0x%08X)",ClearIrq.Type,ClearIrq.Status,IspInfo.IrqInfo.Status[ClearIrq.UserNumber][ClearIrq.Type]);
                spin_lock_irqsave(&(IspInfo.SpinLockIrq[irqT]), flags);
                IspInfo.IrqInfo.Status[ClearIrq.UserNumber][ClearIrq.Type] &= (~ClearIrq.Status);
                spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[irqT]), flags);
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        }
        //
        case ISP_REGISTER_IRQ_USER_KEY:
            userKey=ISP_REGISTER_IRQ_USERKEY();
             if(copy_to_user((void*)Param, &userKey, sizeof(int)) != 0){
                 LOG_ERR("query irq user key\n");
             }
             if(userKey<0)
             {
                Ret = -1 ;
             }
            break;
        //
        case ISP_MARK_IRQ_REQUEST:
            if(copy_from_user(&IrqInfo, (void*)Param, sizeof(ISP_WAIT_IRQ_STRUCT)) == 0)
            {
                if((IrqInfo.UserInfo.UserKey>=IRQ_USER_NUM_MAX) || (IrqInfo.UserInfo.UserKey<1))
            {
                    LOG_ERR("invalid userKey(%d), max(%d)",IrqInfo.UserInfo.UserKey,IRQ_USER_NUM_MAX);
                    Ret = -EFAULT;
                    break;
                }
                if((IrqInfo.UserInfo.Type>=ISP_IRQ_TYPE_AMOUNT) || (IrqInfo.UserInfo.Type<0))
            {
                    LOG_ERR("invalid type(%d), max(%d)",IrqInfo.UserInfo.Type,ISP_IRQ_TYPE_AMOUNT);
                    Ret = -EFAULT;
                    break;
                }
                Ret = ISP_MARK_IRQ(IrqInfo);
            }
            else
            {
                LOG_ERR("ISP_MARK_IRQ, copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        //
        case ISP_GET_MARK2QUERY_TIME:
            if(copy_from_user(&IrqInfo, (void*)Param, sizeof(ISP_WAIT_IRQ_STRUCT)) == 0)
            {
                if((IrqInfo.UserInfo.UserKey>=IRQ_USER_NUM_MAX) || (IrqInfo.UserInfo.UserKey<1))
                {
                    LOG_ERR("invalid userKey(%d), max(%d)",IrqInfo.UserInfo.UserKey,IRQ_USER_NUM_MAX);
                Ret = -EFAULT;
                    break;
            }
                if((IrqInfo.UserInfo.Type>=ISP_IRQ_TYPE_AMOUNT) || (IrqInfo.UserInfo.Type<0))
                {
                    LOG_ERR("invalid type(%d), max(%d)",IrqInfo.UserInfo.Type,ISP_IRQ_TYPE_AMOUNT);
                    Ret = -EFAULT;
            break;
                }
                Ret = ISP_GET_MARKtoQEURY_TIME(&IrqInfo);
                //
                if(copy_to_user((void*)Param, &IrqInfo, sizeof(ISP_WAIT_IRQ_STRUCT)) != 0)
            {
                    LOG_ERR("copy_to_user failed");
                    Ret = -EFAULT;
                }
            }
            else
            {
                LOG_ERR("ISP_GET_MARK2QUERY_TIME, copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        //
        case ISP_FLUSH_IRQ_REQUEST:
            if(copy_from_user(&IrqInfo, (void*)Param, sizeof(ISP_WAIT_IRQ_STRUCT)) == 0)
            {
                if((IrqInfo.UserInfo.UserKey>=IRQ_USER_NUM_MAX) || (IrqInfo.UserInfo.UserKey<1))
                {
                    LOG_ERR("invalid userKey(%d), max(%d)",IrqInfo.UserInfo.UserKey,IRQ_USER_NUM_MAX);
                    Ret = -EFAULT;
                    break;
                }
                if((IrqInfo.UserInfo.Type>=ISP_IRQ_TYPE_AMOUNT) || (IrqInfo.UserInfo.Type<0))
                {
                    LOG_ERR("invalid type(%d), max(%d)",IrqInfo.UserInfo.Type,ISP_IRQ_TYPE_AMOUNT);
                Ret = -EFAULT;
                    break;
                }
                Ret = ISP_FLUSH_IRQ(IrqInfo);
            }
            break;
        //
        case ISP_ED_QUEBUF_CTRL:
            if(copy_from_user(&edQueBuf, (void*)Param, sizeof(ISP_ED_BUFQUE_STRUCT)) == 0)
            {
                edQueBuf.processID=pUserInfo->Pid;
                Ret = ISP_ED_BufQue_CTRL_FUNC(edQueBuf);
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        //
        case ISP_UPDATE_REGSCEN:
            if(copy_from_user(&regScenInfo_value, (void*)Param, sizeof(MUINT32)) == 0)
            {
                spin_lock(&SpinLockRegScen);
                g_regScen=regScenInfo_value;
                spin_unlock(&SpinLockRegScen);
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        case ISP_QUERY_REGSCEN:
            spin_lock(&SpinLockRegScen);
            regScenInfo_value=g_regScen;
            spin_unlock(&SpinLockRegScen);
            //
            if(copy_to_user((void*)Param, &regScenInfo_value, sizeof(MUINT32)) != 0)
            {
                LOG_ERR("copy_to_user failed");
                Ret = -EFAULT;
            }
            break;
        //
        case ISP_UPDATE_BURSTQNUM:
            if(copy_from_user(&burstQNum, (void*)Param, sizeof(MINT32)) == 0)
            {
                spin_lock(&SpinLockRegScen);
                P2_Support_BurstQNum=burstQNum;
                spin_unlock(&SpinLockRegScen);
                LOG_DBG("new BurstQNum(%d)",P2_Support_BurstQNum);
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        case ISP_QUERY_BURSTQNUM:
            spin_lock(&SpinLockRegScen);
            burstQNum=P2_Support_BurstQNum;
            spin_unlock(&SpinLockRegScen);
            //
            if(copy_to_user((void*)Param, &burstQNum, sizeof(MUINT32)) != 0)
            {
                LOG_ERR("copy_to_user failed");
                Ret = -EFAULT;
            }
            break;
        //
        case ISP_DUMP_REG:
        {
            Ret = ISP_DumpReg();
            break;
        }
        case ISP_DEBUG_FLAG:
        {
            if(copy_from_user(DebugFlag, (void*)Param, sizeof(MUINT32)*2) == 0)
            {
                MUINT32 lock_key = _IRQ_MAX;
                if(DebugFlag[1] == _IRQ_D)
                    lock_key = _IRQ;
                else
                    lock_key = DebugFlag[1];
                spin_lock_irqsave(&(IspInfo.SpinLockIrq[lock_key]), flags);
                IspInfo.DebugMask = DebugFlag[0];
                spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[lock_key]), flags);
                //                                                           
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
        }
#ifdef ISP_KERNEL_MOTIFY_SINGAL_TEST
        case ISP_SET_USER_PID:
        {
            if(copy_from_user(&pid, (void*)Param, sizeof(MUINT32)) == 0)
            {
                spin_lock(&(IspInfo.SpinLockIsp));
                getTaskInfo( (pid_t) pid );

                sendSignal();

                LOG_DBG("[ISP_KERNEL_MOTIFY_SINGAL_TEST]:0x08%x ",pid);
                spin_unlock(&(IspInfo.SpinLockIsp));
            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }

            break;
        }
#endif
        case ISP_BUFFER_CTRL:
            Ret = ISP_Buf_CTRL_FUNC(Param);
            break;
        case ISP_REF_CNT_CTRL:
            Ret = ISP_REF_CNT_CTRL_FUNC(Param);
            break;
        case ISP_DUMP_ISR_LOG:
            if(copy_from_user(DebugFlag, (void*)Param, sizeof(MUINT32)) == 0) {
                MUINT32 currentPPB=m_CurrentPPB;
                MUINT32 lock_key = _IRQ_MAX;
                if(DebugFlag[0] == _IRQ_D)
                    lock_key = _IRQ;
                else
                    lock_key = DebugFlag[0];
                spin_lock_irqsave(&(IspInfo.SpinLockIrq[lock_key]), flags);
                m_CurrentPPB = (m_CurrentPPB+1)%LOG_PPNUM;
                spin_unlock_irqrestore(&(IspInfo.SpinLockIrq[lock_key]), flags);

                IRQ_LOG_PRINTER(DebugFlag[0],currentPPB,_LOG_INF);
                IRQ_LOG_PRINTER(DebugFlag[0],currentPPB,_LOG_ERR);

            }
            else
            {
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
#ifdef T_STAMP_2_0
        case ISP_SET_FPS:
            if(copy_from_user(DebugFlag, (void*)Param, sizeof(MUINT32)) == 0) {
                if(m_T_STAMP.fps == 0){
                    m_T_STAMP.fps = DebugFlag[0];
                    m_T_STAMP.interval_us = 1000000/m_T_STAMP.fps;
                    m_T_STAMP.compensation_us = 1000000 - (m_T_STAMP.interval_us*m_T_STAMP.fps);
                    if(m_T_STAMP.fps > 90){
                        bSlowMotion = MTRUE;
                        LOG_INF("slow motion enable:%d",m_T_STAMP.fps);
                    }
                }
            }
            else{
                LOG_ERR("copy_from_user failed");
                Ret = -EFAULT;
            }
            break;
#endif

        default:
        {
            LOG_ERR("Unknown Cmd(%d)",Cmd);
            Ret = -EPERM;
            break;
        }
    }
    //
    EXIT:
    if(Ret != 0)
    {
        LOG_ERR("Fail, Cmd(%d), Pid(%d), (process, pid, tgid)=(%s, %d, %d)",Cmd, pUserInfo->Pid, current->comm , current->pid, current->tgid);
    }
    //
    return Ret;
}

#ifdef CONFIG_COMPAT

/*                                                                              
 
                                                                               */
static int compat_get_isp_read_register_data(
            compat_ISP_REG_IO_STRUCT __user *data32,
            ISP_REG_IO_STRUCT __user *data)
{
    compat_uint_t count;
    compat_uptr_t uptr;
    int err;

    err = get_user(uptr, &data32->pData);
    err |= put_user(compat_ptr(uptr), &data->pData);
    err |= get_user(count, &data32->Count);
    err |= put_user(count, &data->Count);
    return err;
}

static int compat_put_isp_read_register_data(
            compat_ISP_REG_IO_STRUCT __user *data32,
            ISP_REG_IO_STRUCT __user *data)
{
    compat_uint_t count;
    compat_uptr_t uptr;
    int err;
    //                                 
    //                                               
    //                                      
    err |= get_user(count, &data->Count);
    err |= put_user(count, &data32->Count);
    return err;
}

static int compat_get_isp_waitirq_data(
            compat_ISP_WAIT_IRQ_STRUCT __user *data32,
            ISP_WAIT_IRQ_STRUCT __user *data)
{
    compat_uint_t tmp;
    compat_uptr_t uptr;
    ISP_IRQ_USER_STRUCT isp_irq_user_tmp;
    ISP_IRQ_TIME_STRUCT isp_irq_time_tmp;
    int err;

    err = get_user(tmp, &data32->Clear);
    err |= put_user(tmp, &data->Clear);
    err |= get_user(tmp, &data32->Type);
    err |= put_user(tmp, &data->Type);
    err |= get_user(tmp, &data32->Status);
    err |= put_user(tmp, &data->Status);
    err |= get_user(tmp, &data32->UserNumber);
    err |= put_user(tmp, &data->UserNumber);
    err |= get_user(tmp, &data32->Timeout);
    err |= put_user(tmp, &data->Timeout);
    err |= get_user(uptr, &data32->UserName);
    err |= put_user(compat_ptr(uptr), &data->UserName);
    err |= get_user(tmp, &data32->irq_TStamp);
    err |= put_user(tmp, &data->irq_TStamp);
    err |= get_user(tmp, &data32->bDumpReg);
    err |= put_user(tmp, &data->bDumpReg);
    //              
    err |= copy_from_user(&isp_irq_user_tmp, &data32->UserInfo, sizeof(ISP_IRQ_USER_STRUCT));
    err |= copy_to_user((void*)&data->UserInfo, &isp_irq_user_tmp, sizeof(ISP_IRQ_USER_STRUCT));
    err |= copy_from_user(&isp_irq_time_tmp, &data32->TimeInfo, sizeof(ISP_IRQ_TIME_STRUCT));
    err |= copy_to_user((void*)&data->TimeInfo, &isp_irq_time_tmp , sizeof(ISP_IRQ_TIME_STRUCT));

    return err;
}

static int compat_put_isp_waitirq_data(
            compat_ISP_WAIT_IRQ_STRUCT __user *data32,
            ISP_WAIT_IRQ_STRUCT __user *data)
{
    compat_uint_t tmp;
    compat_uptr_t uptr;
    ISP_IRQ_USER_STRUCT isp_irq_user_tmp;
    ISP_IRQ_TIME_STRUCT isp_irq_time_tmp;
    int err;

    err = get_user(tmp, &data->Clear);
    err |= put_user(tmp, &data32->Clear);
    err |= get_user(tmp, &data->Type);
    err |= put_user(tmp, &data32->Type);
    err |= get_user(tmp, &data->Status);
    err |= put_user(tmp, &data32->Status);
    err |= get_user(tmp, &data->UserNumber);
    err |= put_user(tmp, &data32->UserNumber);
    err |= get_user(tmp, &data->Timeout);
    err |= put_user(tmp, &data32->Timeout);
    //                                 
    //                                       
    //                                         
    err |= get_user(tmp, &data->irq_TStamp);
    err |= put_user(tmp, &data32->irq_TStamp);
    err |= get_user(tmp, &data->bDumpReg);
    err |= put_user(tmp, &data32->bDumpReg);

    //              
    err |= copy_from_user(&isp_irq_user_tmp, &data->UserInfo, sizeof(ISP_IRQ_USER_STRUCT));
    err |= copy_to_user((void*)&data32->UserInfo, &isp_irq_user_tmp, sizeof(ISP_IRQ_USER_STRUCT));
    err |= copy_from_user(&isp_irq_time_tmp, &data->TimeInfo, sizeof(ISP_IRQ_TIME_STRUCT));
    err |= copy_to_user((void*)&data32->TimeInfo, &isp_irq_time_tmp , sizeof(ISP_IRQ_TIME_STRUCT));

    return err;
}

static int compat_get_isp_readirq_data(
            compat_ISP_READ_IRQ_STRUCT __user *data32,
            ISP_READ_IRQ_STRUCT __user *data)
{
    compat_uint_t tmp;
    int err;

    err = get_user(tmp, &data32->Type);
    err |= put_user(tmp, &data->Type);
    err |= get_user(tmp, &data32->UserNumber);
    err |= put_user(tmp, &data->UserNumber);
    err |= get_user(tmp, &data32->Status);
    err |= put_user(tmp, &data->Status);
    return err;
}


static int compat_put_isp_readirq_data(
            compat_ISP_READ_IRQ_STRUCT __user *data32,
            ISP_READ_IRQ_STRUCT __user *data)
{
    compat_uint_t tmp;
    int err;

    err = get_user(tmp, &data->Type);
    err |= put_user(tmp, &data32->Type);
    err |= get_user(tmp, &data->UserNumber);
    err |= put_user(tmp, &data32->UserNumber);
    err |= get_user(tmp, &data->Status);
    err |= put_user(tmp, &data32->Status);
    return err;
}

static int compat_get_isp_buf_ctrl_struct_data(
            compat_ISP_BUFFER_CTRL_STRUCT __user *data32,
            ISP_BUFFER_CTRL_STRUCT __user *data)
{
    compat_uint_t tmp;
    compat_uptr_t uptr;
    int err;

    err = get_user(tmp, &data32->ctrl);
    err |= put_user(tmp, &data->ctrl);
    err |= get_user(tmp, &data32->buf_id);
    err |= put_user(tmp, &data->buf_id);
    err |= get_user(uptr, &data32->data_ptr);
    err |= put_user(compat_ptr(uptr), &data->data_ptr);
    err |= get_user(uptr, &data32->ex_data_ptr);
    err |= put_user(compat_ptr(uptr), &data->ex_data_ptr);
    err |= get_user(uptr, &data32->pExtend);
    err |= put_user(compat_ptr(uptr), &data->pExtend);

    return err;
}

static int compat_put_isp_buf_ctrl_struct_data(
            compat_ISP_BUFFER_CTRL_STRUCT __user *data32,
            ISP_BUFFER_CTRL_STRUCT __user *data)
{
    compat_uint_t tmp;
    compat_uptr_t uptr;
    int err;

    err = get_user(tmp, &data->ctrl);
    err |= put_user(tmp, &data32->ctrl);
    err |= get_user(tmp, &data->buf_id);
    err |= put_user(tmp, &data32->buf_id);
    //                                 
    //                                                   
    //                                         
    //                                                      
    //                                            
    //                                                  
    //                                        

    return err;
}

static int compat_get_isp_ref_cnt_ctrl_struct_data(
            compat_ISP_REF_CNT_CTRL_STRUCT __user *data32,
            ISP_REF_CNT_CTRL_STRUCT __user *data)
{
    compat_uint_t tmp;
    compat_uptr_t uptr;
    int err;

    err = get_user(tmp, &data32->ctrl);
    err |= put_user(tmp, &data->ctrl);
    err |= get_user(tmp, &data32->id);
    err |= put_user(tmp, &data->id);
    err |= get_user(uptr, &data32->data_ptr);
    err |= put_user(compat_ptr(uptr), &data->data_ptr);

    return err;
}

static int compat_put_isp_ref_cnt_ctrl_struct_data(
            compat_ISP_REF_CNT_CTRL_STRUCT __user *data32,
            ISP_REF_CNT_CTRL_STRUCT __user *data)
{
    compat_uint_t tmp;
    compat_uptr_t uptr;
    int err;

    err = get_user(tmp, &data->ctrl);
    err |= put_user(tmp, &data32->ctrl);
    err |= get_user(tmp, &data->id);
    err |= put_user(tmp, &data32->id);
    //                                 
    //                                                   
    //                                         

    return err;
}

static long ISP_ioctl_compat(struct file *filp, unsigned int cmd, unsigned long arg)
{
    long ret;

    if (!filp->f_op || !filp->f_op->unlocked_ioctl)
        return -ENOTTY;

    switch (cmd) {
    case COMPAT_ISP_READ_REGISTER:
    {
        compat_ISP_REG_IO_STRUCT __user *data32;
        ISP_REG_IO_STRUCT __user *data;
        int err;

        data32 = compat_ptr(arg);
        data = compat_alloc_user_space(sizeof(*data));
        if (data == NULL)
            return -EFAULT;

        err = compat_get_isp_read_register_data(data32, data);
        if (err)
        {
            LOG_INF("compat_get_isp_read_register_data error!!!\n");
            return err;
        }
        ret = filp->f_op->unlocked_ioctl(filp, ISP_READ_REGISTER,(unsigned long)data);
        err = compat_put_isp_read_register_data(data32, data);
        if (err)
        {
            LOG_INF("compat_put_isp_read_register_data error!!!\n");
            return err;
        }
        return ret;
    }
    case COMPAT_ISP_WRITE_REGISTER:
    {
        compat_ISP_REG_IO_STRUCT __user *data32;
        ISP_REG_IO_STRUCT __user *data;
        int err;

        data32 = compat_ptr(arg);
        data = compat_alloc_user_space(sizeof(*data));
        if (data == NULL)
            return -EFAULT;

        err = compat_get_isp_read_register_data(data32, data);
        if (err)
        {
            LOG_INF("COMPAT_ISP_WRITE_REGISTER error!!!\n");
            return err;
        }
        ret = filp->f_op->unlocked_ioctl(filp, ISP_WRITE_REGISTER,(unsigned long)data);
        return ret;
    }
    case COMPAT_ISP_WAIT_IRQ:
    {
        compat_ISP_WAIT_IRQ_STRUCT __user *data32;
        ISP_WAIT_IRQ_STRUCT __user *data;
        int err;

        data32 = compat_ptr(arg);
        data = compat_alloc_user_space(sizeof(*data));
        if (data == NULL)
            return -EFAULT;

        err = compat_get_isp_waitirq_data(data32, data);
        if (err)
        {
            LOG_INF("compat_get_isp_waitirq_data error!!!\n");
            return err;
        }
        ret = filp->f_op->unlocked_ioctl(filp, ISP_WAIT_IRQ,(unsigned long)data);
        err = compat_put_isp_waitirq_data(data32, data);
        if (err)
        {
            LOG_INF("compat_put_isp_waitirq_data error!!!\n");
            return err;
        }
        return ret;
    }
	case COMPAT_ISP_MARK_IRQ_REQUEST:
	{
		compat_ISP_WAIT_IRQ_STRUCT __user *data32;
		ISP_WAIT_IRQ_STRUCT __user *data;
		int err;
	
		data32 = compat_ptr(arg);
		data = compat_alloc_user_space(sizeof(*data));
		if (data == NULL)
			return -EFAULT;
	
		err = compat_get_isp_waitirq_data(data32, data);
		if (err)
		{
			LOG_INF("compat_get_ISP_MARK_IRQ_REQUEST error!!!\n");
			return err;
		}
		ret = filp->f_op->unlocked_ioctl(filp, ISP_MARK_IRQ_REQUEST,(unsigned long)data);
		err = compat_put_isp_waitirq_data(data32, data);
		if (err)
		{
			LOG_INF("compat_put_ISP_MARK_IRQ_REQUEST error!!!\n");
			return err;
		}
		return ret;
	}
	case COMPAT_ISP_GET_MARK2QUERY_TIME:
	{
		compat_ISP_WAIT_IRQ_STRUCT __user *data32;
		ISP_WAIT_IRQ_STRUCT __user *data;
		int err;
	
		data32 = compat_ptr(arg);
		data = compat_alloc_user_space(sizeof(*data));
		if (data == NULL)
			return -EFAULT;
	
		err = compat_get_isp_waitirq_data(data32, data);
		if (err)
		{
			LOG_INF("compat_get_ISP_GET_MARK2QUERY_TIME error!!!\n");
			return err;
		}
		ret = filp->f_op->unlocked_ioctl(filp, ISP_GET_MARK2QUERY_TIME,(unsigned long)data);
		err = compat_put_isp_waitirq_data(data32, data);
		if (err)
		{
			LOG_INF("compat_put_ISP_GET_MARK2QUERY_TIME error!!!\n");
			return err;
		}
		return ret;
	}
	case COMPAT_ISP_FLUSH_IRQ_REQUEST:
	{
		compat_ISP_WAIT_IRQ_STRUCT __user *data32;
		ISP_WAIT_IRQ_STRUCT __user *data;
		int err;
	
		data32 = compat_ptr(arg);
		data = compat_alloc_user_space(sizeof(*data));
		if (data == NULL)
			return -EFAULT;
	
		err = compat_get_isp_waitirq_data(data32, data);
		if (err)
		{
			LOG_INF("compat_get_ISP_FLUSH_IRQ_REQUEST error!!!\n");
			return err;
		}
		ret = filp->f_op->unlocked_ioctl(filp, ISP_FLUSH_IRQ_REQUEST,(unsigned long)data);
		err = compat_put_isp_waitirq_data(data32, data);
		if (err)
		{
			LOG_INF("compat_put_ISP_FLUSH_IRQ_REQUEST error!!!\n");
			return err;
		}
		return ret;
	}
    case COMPAT_ISP_READ_IRQ:
    {
        compat_ISP_READ_IRQ_STRUCT __user *data32;
        ISP_READ_IRQ_STRUCT __user *data;
        int err;

        data32 = compat_ptr(arg);
        data = compat_alloc_user_space(sizeof(*data));
        if (data == NULL)
            return -EFAULT;

        err = compat_get_isp_readirq_data(data32, data);
        if (err)
        {
            LOG_INF("compat_get_isp_readirq_data error!!!\n");
            return err;
        }
        ret = filp->f_op->unlocked_ioctl(filp, ISP_READ_IRQ,(unsigned long)data);
        err = compat_put_isp_readirq_data(data32, data);
        if (err)
        {
            LOG_INF("compat_put_isp_readirq_data error!!!\n");
            return err;
        }

        return ret;
    }
    case COMPAT_ISP_BUFFER_CTRL:
    {
        compat_ISP_BUFFER_CTRL_STRUCT __user *data32;
        ISP_BUFFER_CTRL_STRUCT __user *data;
        int err;

        data32 = compat_ptr(arg);
        data = compat_alloc_user_space(sizeof(*data));
        if (data == NULL)
            return -EFAULT;

        err = compat_get_isp_buf_ctrl_struct_data(data32, data);
        if (err)
            return err;
        if (err)
        {
            LOG_INF("compat_get_isp_buf_ctrl_struct_data error!!!\n");
            return err;
        }
        ret = filp->f_op->unlocked_ioctl(filp, ISP_BUFFER_CTRL,(unsigned long)data);
        err = compat_put_isp_buf_ctrl_struct_data(data32, data);

        if (err)
        {
            LOG_INF("compat_put_isp_buf_ctrl_struct_data error!!!\n");
            return err;
        }
        return ret;

    }
    case COMPAT_ISP_REF_CNT_CTRL:
    {
        compat_ISP_REF_CNT_CTRL_STRUCT __user *data32;
        ISP_REF_CNT_CTRL_STRUCT __user *data;
        int err;

        data32 = compat_ptr(arg);
        data = compat_alloc_user_space(sizeof(*data));
        if (data == NULL)
            return -EFAULT;

        err = compat_get_isp_ref_cnt_ctrl_struct_data(data32, data);
        if (err)
        {
            LOG_INF("compat_get_isp_ref_cnt_ctrl_struct_data error!!!\n");
            return err;
        }
        ret = filp->f_op->unlocked_ioctl(filp, ISP_REF_CNT_CTRL,(unsigned long)data);
        err = compat_put_isp_ref_cnt_ctrl_struct_data(data32, data);
        if (err)
        {
            LOG_INF("compat_put_isp_ref_cnt_ctrl_struct_data error!!!\n");
            return err;
        }
        return ret;

    }
    case COMPAT_ISP_DEBUG_FLAG:
	{
		//                                     
		ret = filp->f_op->unlocked_ioctl(filp, ISP_DEBUG_FLAG,(unsigned long)compat_ptr(arg));
		return ret;
	}
    case COMPAT_ISP_GET_INT_ERR:
	{
		//                                     
		ret = filp->f_op->unlocked_ioctl(filp, ISP_GET_INT_ERR,(unsigned long)compat_ptr(arg));
		return ret;
	}
    case COMPAT_ISP_GET_DMA_ERR:
    {
        //                                     
        ret = filp->f_op->unlocked_ioctl(filp, ISP_GET_DMA_ERR,(unsigned long)compat_ptr(arg));
        return ret;
    }
    case ISP_GET_CUR_SOF:
    case ISP_GET_DROP_FRAME:
    case ISP_RESET_CAM_P1:
    case ISP_RESET_CAM_P2:
    case ISP_RESET_CAMSV:
    case ISP_RESET_CAMSV2:
    case ISP_RESET_BUF:
    case ISP_HOLD_REG_TIME: //                
    case ISP_HOLD_REG:  //                       
    case ISP_CLEAR_IRQ: //                      
    case ISP_REGISTER_IRQ:
    case ISP_UNREGISTER_IRQ:
    case ISP_ED_QUEBUF_CTRL://                      
    case ISP_UPDATE_REGSCEN:
    case ISP_QUERY_REGSCEN:
    case ISP_UPDATE_BURSTQNUM:
    case ISP_QUERY_BURSTQNUM:
    case ISP_DUMP_REG:
    case ISP_SET_USER_PID: //                                                          
    case ISP_SET_FPS:
    case ISP_DUMP_ISR_LOG:
	case ISP_REGISTER_IRQ_USER_KEY:
        return filp->f_op->unlocked_ioctl(filp, cmd, arg);
    default:
        return -ENOIOCTLCMD;    
    //                                  
    }
}

#endif

/*                                                                              
 
                                                                               */
static MINT32 ISP_open(
    struct inode*   pInode,
    struct file*    pFile)
{
    MINT32 Ret = 0;
    MUINT32 i;
    int q=0,p=0;
    ISP_USER_INFO_STRUCT* pUserInfo;

    LOG_DBG("- E. UserCount: %d.", IspInfo.UserCount);
    //
    spin_lock(&(IspInfo.SpinLockIspRef));
    //
    //                                           
    //
    pFile->private_data = NULL;
    pFile->private_data = kmalloc(sizeof(ISP_USER_INFO_STRUCT) , GFP_ATOMIC);
    if(pFile->private_data == NULL)
    {
        LOG_DBG("ERROR: kmalloc failed, (process, pid, tgid)=(%s, %d, %d)", current->comm, current->pid, current->tgid);
        Ret = -ENOMEM;
    }
    else
    {
        pUserInfo = (ISP_USER_INFO_STRUCT*)pFile->private_data;
        pUserInfo->Pid = current->pid;
        pUserInfo->Tid = current->tgid;
    }
    //
    if(IspInfo.UserCount > 0)
    {
        IspInfo.UserCount++;
        LOG_DBG("Curr UserCount(%d), (process, pid, tgid)=(%s, %d, %d), users exist",IspInfo.UserCount,current->comm, current->pid, current->tgid);
        goto EXIT;
    }
    //                                               
    EDBufQueRemainNodeCnt = 0;
    P2_Support_BurstQNum=1;
    //
    for(i=0;i<IRQ_USER_NUM_MAX;i++)
    {
        IrqLockedUserKey[i]=0;
    }
    //
    for(i=0;i<_MAX_SUPPORT_P2_FRAME_NUM_;i++)
    {
        P2_EDBUF_RingList[i].processID=0x0;
        P2_EDBUF_RingList[i].callerID=0x0;
        P2_EDBUF_RingList[i].p2dupCQIdx=-1;
        P2_EDBUF_RingList[i].bufSts=ISP_ED_BUF_STATE_NONE;
    }
    P2_EDBUF_RList_FirstBufIdx=0;
    P2_EDBUF_RList_CurBufIdx=0;
    P2_EDBUF_RList_LastBufIdx=-1;
    //
    for(i=0;i<_MAX_SUPPORT_P2_PACKAGE_NUM_;i++)
    {
        P2_EDBUF_MgrList[i].processID=0x0;
        P2_EDBUF_MgrList[i].callerID=0x0;
        P2_EDBUF_MgrList[i].p2dupCQIdx=-1;
        P2_EDBUF_MgrList[i].dequedNum=0;
    }
    P2_EDBUF_MList_FirstBufIdx=0;
    P2_EDBUF_MList_LastBufIdx=-1;
    //
    g_regScen=0xa5a5a5a5;
    //
    IspInfo.BufInfo.Read.pData = (MUINT8 *) kmalloc(ISP_BUF_SIZE, GFP_ATOMIC);
    IspInfo.BufInfo.Read.Size = ISP_BUF_SIZE;
    IspInfo.BufInfo.Read.Status = ISP_BUF_STATUS_EMPTY;
    if(IspInfo.BufInfo.Read.pData == NULL)
    {
        LOG_DBG("ERROR: BufRead kmalloc failed");
        Ret = -ENOMEM;
        goto EXIT;
    }
    //
    if(!ISP_BufWrite_Alloc())
    {
        LOG_DBG("ERROR: BufWrite kmalloc failed");
        Ret = -ENOMEM;
        goto EXIT;
    }
    //
    atomic_set(&(IspInfo.HoldInfo.HoldEnable), 0);
    atomic_set(&(IspInfo.HoldInfo.WriteEnable), 0);
    for (i=0;i<ISP_REF_CNT_ID_MAX;i++) {
        atomic_set(&g_imem_ref_cnt[i],0);
    }
    //             
    ISP_EnableClock(MTRUE);
    LOG_DBG("isp open G_u4EnableClockCount: %d", G_u4EnableClockCount);
    //

   //
    for(q=0;q<IRQ_USER_NUM_MAX;q++)
    {
        for(i=0; i<ISP_IRQ_TYPE_AMOUNT; i++)
        {
            IspInfo.IrqInfo.Status[q][i] = 0;
            IspInfo.IrqInfo.MarkedFlag[q][i]=0;
            for(p=0;p<32;p++)
            {
                IspInfo.IrqInfo.MarkedTime_usec[q][i][p]=0;
                IspInfo.IrqInfo.PassedBySigCnt[q][i][p]=0;
                IspInfo.IrqInfo.LastestSigTime_usec[i][p]=0;
            }
        }
    }
    for(i=0; i<ISP_CALLBACK_AMOUNT; i++)
    {
        IspInfo.Callback[i].Func = NULL;
    }
    //
    IspInfo.UserCount++;
    LOG_DBG("Curr UserCount(%d), (process, pid, tgid)=(%s, %d, %d), first user",IspInfo.UserCount,current->comm, current->pid, current->tgid);
    //


#ifdef KERNEL_LOG
    IspInfo.DebugMask = (ISP_DBG_INT|ISP_DBG_BUF_CTRL);
#endif
    //
    EXIT:
    if(Ret < 0)
    {
        if(IspInfo.BufInfo.Read.pData != NULL)
        {
            kfree(IspInfo.BufInfo.Read.pData);
            IspInfo.BufInfo.Read.pData = NULL;
        }
        //
        ISP_BufWrite_Free();
    }
    //
    spin_unlock(&(IspInfo.SpinLockIspRef));
    //

    //                                      
    //                                     


#if 0 //                    
    spm_disable_sodi();
#endif

    LOG_DBG("- X. Ret: %d. UserCount: %d.", Ret, IspInfo.UserCount);
    return Ret;

}

/*                                                                              
 
                                                                               */
static MINT32 ISP_release(
    struct inode*   pInode,
    struct file*    pFile)
{
    ISP_USER_INFO_STRUCT* pUserInfo;
    MUINT32 Reg;
    LOG_DBG("- E. UserCount: %d.", IspInfo.UserCount);
    //

    //
    //                                           
    //
    if(pFile->private_data != NULL)
    {
        pUserInfo = (ISP_USER_INFO_STRUCT*)pFile->private_data;
        kfree(pFile->private_data);
        pFile->private_data = NULL;
    }
    //
    spin_lock(&(IspInfo.SpinLockIspRef));
    IspInfo.UserCount--;
    if(IspInfo.UserCount > 0)
    {
        spin_unlock(&(IspInfo.SpinLockIspRef));
        LOG_DBG("Curr UserCount(%d), (process, pid, tgid)=(%s, %d, %d), users exist",IspInfo.UserCount,current->comm, current->pid, current->tgid);
        goto EXIT;
    }
    else
        spin_unlock(&(IspInfo.SpinLockIspRef));
    //
    LOG_DBG("Curr UserCount(%d), (process, pid, tgid)=(%s, %d, %d), last user",IspInfo.UserCount,current->comm, current->pid, current->tgid);

    //                                                                                        
    Reg = ISP_RD32(ISP_REG_ADDR_TG_VF_CON);
    Reg &= 0xfffffffE;//             
    ISP_WR32(ISP_REG_ADDR_TG_VF_CON,Reg);

    Reg = ISP_RD32(ISP_REG_ADDR_TG2_VF_CON);
    Reg &= 0xfffffffE;//             
    ISP_WR32(ISP_REG_ADDR_TG2_VF_CON,Reg);

    //               
    ISP_EnableClock(MFALSE);
    LOG_DBG("isp release G_u4EnableClockCount: %d", G_u4EnableClockCount);

    if(IspInfo.BufInfo.Read.pData != NULL)
    {
        kfree(IspInfo.BufInfo.Read.pData);
        IspInfo.BufInfo.Read.pData = NULL;
        IspInfo.BufInfo.Read.Size = 0;
        IspInfo.BufInfo.Read.Status = ISP_BUF_STATUS_EMPTY;
    }
    //
    ISP_BufWrite_Free();
    //
    EXIT:

    //
    //                                     
    //                                    

#if 0 //                    
    spm_enable_sodi();
#endif

    LOG_DBG("- X. UserCount: %d.", IspInfo.UserCount);
    return 0;
}

/*                                                                              
 
                                                                               */
//                                                                          
static MINT32 mmap_kmem(struct file *filp, struct vm_area_struct *vma)
{
        int ret;
        long length = 0;
        length=vma->vm_end - vma->vm_start;

        /*                                                               
                           */
        if (length > RT_BUF_TBL_NPAGES * PAGE_SIZE)
                return -EIO;

        /*                                                       */
        LOG_DBG("Vma->vm_pgoff(0x%x),Vma->vm_start(0x%x),Vma->vm_end(0x%x),length(0x%x)",\
            vma->vm_pgoff,vma->vm_start,vma->vm_end,length);
        if(length>ISP_RTBUF_REG_RANGE)
        {
            LOG_ERR("mmap range error! : length(0x%x),ISP_RTBUF_REG_RANGE(0x%x)!",length,ISP_RTBUF_REG_RANGE);
            return -EAGAIN;
        }
        if ((ret = remap_pfn_range(vma,
                                   vma->vm_start,
                                   virt_to_phys((void *)pTbl_RTBuf) >> PAGE_SHIFT,
                                   length,
                                   vma->vm_page_prot)) < 0) {
            return ret;
        }

        return 0;
}
/*                                                                              
 
                                                                               */
static MINT32 ISP_mmap(
    struct file*            pFile,
    struct vm_area_struct*  pVma)
{
    long length = 0;
    MUINT32 pfn = 0x0;
    LOG_DBG("- E.");
    length=pVma->vm_end - pVma->vm_start;
    /*                                                       */
    if (pVma->vm_pgoff == RT_BUF_TBL_NPAGES) {
            return mmap_kmem(pFile, pVma);
    }
    else
    {
        //
        pVma->vm_page_prot = pgprot_noncached(pVma->vm_page_prot);
        LOG_DBG("pVma->vm_pgoff(0x%x),phy(0x%x),pVmapVma->vm_start(0x%x),pVma->vm_end(0x%x),length(0x%x)",\
            pVma->vm_pgoff,pVma->vm_pgoff<<PAGE_SHIFT,pVma->vm_start,pVma->vm_end,length);
        pfn=pVma->vm_pgoff<<PAGE_SHIFT;//                                                   
        switch(pfn)
        {
            case IMGSYS_BASE_ADDR:  //      
                if(length>ISP_REG_RANGE)
                {
                    LOG_ERR("mmap range error : length(0x%x),ISP_REG_RANGE(0x%x)!",length,ISP_REG_RANGE);
                    return -EAGAIN;
                }
                break;
            case SENINF_BASE_ADDR:
                if(length>SENINF_REG_RANGE)
                {
                    LOG_ERR("mmap range error : length(0x%x),SENINF_REG_RANGE(0x%x)!",length,SENINF_REG_RANGE);
                    return -EAGAIN;
                }
                break;
            case PLL_BASE_ADDR:
                if(length>PLL_RANGE)
                {
                    LOG_ERR("mmap range error : length(0x%x),PLL_RANGE(0x%x)!",length,PLL_RANGE);
                    return -EAGAIN;
                }
                break;
            case MIPIRX_CONFIG_ADDR:
                if(length>MIPIRX_CONFIG_RANGE)
                {
                    LOG_ERR("mmap range error : length(0x%x),MIPIRX_CONFIG_RANGE(0x%x)!",length,MIPIRX_CONFIG_RANGE);
                    return -EAGAIN;
                }
                break;
            case MIPIRX_ANALOG_ADDR:
                if(length>MIPIRX_ANALOG_RANGE)
                {
                    LOG_ERR("mmap range error : length(0x%x),MIPIRX_ANALOG_RANGE(0x%x)!",length,MIPIRX_ANALOG_RANGE);
                    return -EAGAIN;
                }
                break;
            case GPIO_BASE_ADDR:
                if(length>GPIO_RANGE)
                {
                    LOG_ERR("mmap range error : length(0x%x),GPIO_RANGE(0x%x)!",length,GPIO_RANGE);
                    return -EAGAIN;
                }
                break;
            default:
                LOG_ERR("Illegal starting HW addr for mmap!");
                return -EAGAIN;
                break;
        }
        if(remap_pfn_range(pVma, pVma->vm_start, pVma->vm_pgoff,pVma->vm_end - pVma->vm_start, pVma->vm_page_prot))
        {
            return -EAGAIN;
        }
    }
    //
    return 0;
}

/*                                                                              
 
                                                                               */
#ifdef CONFIG_OF
struct cam_isp_device{
    void __iomem *regs[ISP_CAM_BASEADDR_NUM];
    struct device *dev;
    int irq[ISP_CAM_IRQ_IDX_NUM];
};

static struct cam_isp_device *cam_isp_devs;
static int nr_camisp_devs = 0;
#endif
static dev_t IspDevNo;
static struct cdev *pIspCharDrv = NULL;
static struct class *pIspClass = NULL;

static const struct file_operations IspFileOper =
{
    .owner   = THIS_MODULE,
    .open    = ISP_open,
    .release = ISP_release,
    //                        
    .mmap    = ISP_mmap,
    .unlocked_ioctl   = ISP_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = ISP_ioctl_compat,
#endif
};

/*                                                                              
 
                                                                               */
inline static void ISP_UnregCharDev(void)
{
    LOG_DBG("- E.");
    //
    //                   
    if(pIspCharDrv != NULL)
    {
        cdev_del(pIspCharDrv);
        pIspCharDrv = NULL;
    }
    //
    unregister_chrdev_region(IspDevNo, 1);
}

/*                                                                              
 
                                                                               */
inline static MINT32 ISP_RegCharDev(void)
{
    MINT32 Ret = 0;
    //
    LOG_DBG("- E.");
    //
    if((Ret = alloc_chrdev_region(&IspDevNo, 0, 1, ISP_DEV_NAME)) < 0)
    {
        LOG_ERR("alloc_chrdev_region failed, %d", Ret);
        return Ret;
    }
    //               
    pIspCharDrv = cdev_alloc();
    if(pIspCharDrv == NULL)
    {
        LOG_ERR("cdev_alloc failed");
        Ret = -ENOMEM;
        goto EXIT;
    }
    //                       
    cdev_init(pIspCharDrv, &IspFileOper);
    //
    pIspCharDrv->owner = THIS_MODULE;
    //             
    if((Ret = cdev_add(pIspCharDrv, IspDevNo, 1)) < 0)
    {
        LOG_ERR("Attatch file operation failed, %d", Ret);
        goto EXIT;
    }
    //
    EXIT:
    if(Ret < 0)
    {
        ISP_UnregCharDev();
    }
    //

    LOG_DBG("- X.");
    return Ret;
}

/*                                                                              
 
                                                                               */
static MINT32 ISP_probe(struct platform_device* pDev)
{
    MINT32 Ret = 0;
    struct resource *pRes = NULL;
    MINT32 i=0;
    MUINT8 n;
    int new_count;
#ifdef CONFIG_OF
    struct cam_isp_device *cam_isp_dev;
#endif
    //           
    //
    printk("kk:+ ISP_probe\n");

    LOG_DBG("- E.");
    LOG_INF("ISP driver proble.");
    //                                 
#ifdef CONFIG_OF

    if(pDev == NULL)
    {
        dev_err(&pDev->dev, "pDev is NULL");
        return -ENXIO;
    }

    new_count = nr_camisp_devs + 1;
    cam_isp_devs = krealloc(cam_isp_devs, 
        sizeof(struct cam_isp_device) * new_count, GFP_KERNEL);
    if (!cam_isp_devs) {
        dev_err(&pDev->dev, "Unable to allocate cam_isp_devs\n");
        return -ENOMEM;
    }

    cam_isp_dev = &(cam_isp_devs[nr_camisp_devs]);
    cam_isp_dev->dev = &pDev->dev;

    /*                        */
    for(i=0;i<ISP_CAM_BASEADDR_NUM;i++)
    {
        cam_isp_dev->regs[i] = of_iomap(pDev->dev.of_node, i);
        if (!cam_isp_dev->regs[i]) {
            dev_err(&pDev->dev, "Unable to ioremap registers, of_iomap fail, i=%d \n", i);
            return -ENOMEM;
        }

        gISPSYS_Reg[i] = cam_isp_dev->regs[i];
        LOG_INF("DT, i=%d, map_addr=0x%lx\n", i, cam_isp_dev->regs[i]);
     }

      /*                            */
     for(i=0;i<ISP_CAM_IRQ_IDX_NUM;i++)
     {
          cam_isp_dev->irq[i] = irq_of_parse_and_map(pDev->dev.of_node, i);
          gISPSYS_Irq[i] = cam_isp_dev->irq[i];
          if (ISP_CAM0_IRQ_IDX == i)
          {
             Ret = request_irq(cam_isp_dev->irq[i], (irq_handler_t)ISP_Irq_CAM, 
                               IRQF_TRIGGER_NONE, "ISP",  NULL);  //                                                                               
          }
          else if (ISP_CAMSV0_IRQ_IDX == i)
          {
              Ret = request_irq(cam_isp_dev->irq[i], (irq_handler_t)ISP_Irq_CAMSV, 
                                IRQF_TRIGGER_NONE, "ISP",  NULL);  //                                                                               
          }
          else if (ISP_CAMSV1_IRQ_IDX == i)
          {
              Ret = request_irq(cam_isp_dev->irq[i], (irq_handler_t)ISP_Irq_CAMSV2, 
                                IRQF_TRIGGER_NONE, "ISP",  NULL);  //                                                                               
          }

          if (Ret) {
              dev_err(&pDev->dev, "Unable to request IRQ, request_irq fail, i=%d, irq=%d \n", i, cam_isp_dev->irq[i]);
              return Ret;
           }
          LOG_INF("DT, i=%d, map_irq=%d\n", i, cam_isp_dev->irq[i]);

      }
      nr_camisp_devs = new_count;


    if(pDev == NULL)
    {
        dev_err(&pDev->dev, "pDev is NULL");
        return -ENXIO;
    }

    //                     
    if((Ret = ISP_RegCharDev()))
    {
        dev_err(&pDev->dev, "register char failed");
        return Ret;
    }
    //                      
#if 0
    for(i = 0; i < 1; i++)  //                                                                                                     
    {
        LOG_DBG("Mapping CAM_REGISTERS. i: %d.", i);
        pRes = platform_get_resource(pDev, IORESOURCE_MEM, i);
        if(pRes == NULL)
        {
            dev_err(&pDev->dev, "platform_get_resource failed");
            Ret = -ENOMEM;
            goto EXIT;
        }
        pRes = request_mem_region(pRes->start, pRes->end - pRes->start + 1, pDev->name);
        if(pRes == NULL)
        {
            dev_err(&pDev->dev, "request_mem_region failed");
            Ret = -ENOMEM;
            goto EXIT;
        }
    }
#endif

    //                      
    pIspClass = class_create(THIS_MODULE, "ispdrv");
    if(IS_ERR(pIspClass))
    {
        Ret = PTR_ERR(pIspClass);
        LOG_ERR("Unable to create class, err = %d", Ret);
        return Ret;
    }
    //                      
    device_create(pIspClass, NULL, IspDevNo, NULL, ISP_DEV_NAME);

#endif
    //
    init_waitqueue_head(&IspInfo.WaitQueueHead);
    tasklet_init(&isp_tasklet,ISP_TaskletFunc,0);

    //
    INIT_WORK(&IspInfo.ScheduleWorkVD,       ISP_ScheduleWork_VD);
    INIT_WORK(&IspInfo.ScheduleWorkEXPDONE,  ISP_ScheduleWork_EXPDONE);
    //
    spin_lock_init(&(IspInfo.SpinLockIspRef));
    spin_lock_init(&(IspInfo.SpinLockIsp));
    for(n=0;n<_IRQ_MAX;n++)
    {
        spin_lock_init(&(IspInfo.SpinLockIrq[n]));
    }
    spin_lock_init(&(IspInfo.SpinLockHold));
    spin_lock_init(&(IspInfo.SpinLockRTBC));
    spin_lock_init(&(IspInfo.SpinLockClock));
    //
    IspInfo.UserCount = 0;
    IspInfo.HoldInfo.Time = ISP_HOLD_TIME_EXPDONE;
    //
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_P1_ST]    = ISP_REG_MASK_INT_P1_ST;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_P1_ST2]   = ISP_REG_MASK_INT_P1_ST2;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_P1_ST_D]  = ISP_REG_MASK_INT_P1_ST_D;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_P1_ST2_D] = ISP_REG_MASK_INT_P1_ST2_D;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_P2_ST]    = ISP_REG_MASK_INT_P2_ST;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_STATUSX]  = ISP_REG_MASK_INT_STATUSX;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_STATUS2X] = ISP_REG_MASK_INT_STATUS2X;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_STATUS3X] = ISP_REG_MASK_INT_STATUS3X;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_SENINF1]  = ISP_REG_MASK_INT_SENINF1;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_SENINF2]  = ISP_REG_MASK_INT_SENINF2;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_SENINF3]  = ISP_REG_MASK_INT_SENINF3;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_SENINF4]  = ISP_REG_MASK_INT_SENINF4;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV]    = ISP_REG_MASK_CAMSV_ST;
    IspInfo.IrqInfo.Mask[ISP_IRQ_TYPE_INT_CAMSV2]   = ISP_REG_MASK_CAMSV2_ST;
    //
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_P1_ST]    = ISP_REG_MASK_INT_P1_ST_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_P1_ST2]   = ISP_REG_MASK_INT_P1_ST2_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_P1_ST_D]  = ISP_REG_MASK_INT_P1_ST_D_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_P1_ST2_D] = ISP_REG_MASK_INT_P1_ST2_D_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_P2_ST]    = ISP_REG_MASK_INT_P2_ST_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_STATUSX]  = ISP_REG_MASK_INT_STATUSX_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_STATUS2X] = ISP_REG_MASK_INT_STATUS2X_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_STATUS3X] = ISP_REG_MASK_INT_STATUS3X_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_SENINF1] = ISP_REG_MASK_INT_SENINF1_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_SENINF2] = ISP_REG_MASK_INT_SENINF2_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_SENINF3] = ISP_REG_MASK_INT_SENINF3_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_SENINF4] = ISP_REG_MASK_INT_SENINF4_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_CAMSV]    = ISP_REG_MASK_CAMSV_ST_ERR;
    IspInfo.IrqInfo.ErrMask[ISP_IRQ_TYPE_INT_CAMSV2]   = ISP_REG_MASK_CAMSV2_ST_ERR;

    //                                           
    init_waitqueue_head(&WaitQueueHead_EDBuf_WaitDeque);
    init_waitqueue_head(&WaitQueueHead_EDBuf_WaitFrame);
    spin_lock_init(&(SpinLockEDBufQueList));
    spin_lock_init(&(SpinLockRegScen));
    spin_lock_init(&(SpinLock_UserKey));

    //                    
#ifndef CONFIG_OF
    //      
    if (request_irq(CAM0_IRQ_BIT_ID, (irq_handler_t)ISP_Irq_CAM, IRQF_TRIGGER_LOW , "ISP", NULL))
//                                                                                                
    {
        LOG_ERR("MT6593_CAM_IRQ_LINE IRQ LINE NOT AVAILABLE!!");
        goto EXIT;
    }
    //                                  
    //                   
    if (request_irq(CAM_SV0_IRQ_BIT_ID, (irq_handler_t)ISP_Irq_CAMSV, IRQF_TRIGGER_LOW , "ISP", NULL))
    {
        LOG_ERR("MT6593_CAMSV1_IRQ_LINE IRQ LINE NOT AVAILABLE!!");
        goto EXIT;
    }
    //                    
    if (request_irq(CAM_SV1_IRQ_BIT_ID, (irq_handler_t)ISP_Irq_CAMSV2, IRQF_TRIGGER_LOW , "ISP", NULL))
    {
        LOG_ERR("MT6593_CAMSV2_IRQ_LINE IRQ LINE NOT AVAILABLE!!");
        goto EXIT;
    }
    #endif

    EXIT:
    if(Ret < 0)
    {
        ISP_UnregCharDev();
    }
    //
    LOG_DBG("- X.");
    //
    printk("kk:- ISP_probe\n");
    //
    return Ret;
}

/*                                                                              
                                                          
                                                                               */
static MINT32 ISP_remove(struct platform_device *pDev)
{
    struct resource *pRes;
    MINT32 i;
    MINT32 IrqNum;
    //
    LOG_DBG("- E.");
    //                        
    ISP_UnregCharDev();
    //                                    
#if 0
    for(i = 0; i < 2; i++)
    {
        pRes = platform_get_resource(pDev, IORESOURCE_MEM, 0);
        release_mem_region(pRes->start, (pRes->end - pRes->start + 1));
    }
#endif
    //            
    disable_irq(IspInfo.IrqNum);
    IrqNum = platform_get_irq(pDev, 0);
    free_irq(IrqNum , NULL);

    //            
    tasklet_kill(&isp_tasklet);
    #if 0
    //                                    
    ISP_UnRegister_AllregIrq();
    //                                  
    struct my_list_head* head;
    struct my_list_head* father;
    head=((struct my_list_head*)(&SupIrqUserListHead.list));
    while(1)
    {
        father=head;
        if(father->nextirq!=father)
        {
            father=father->nextirq;
            REG_IRQ_NODE *accessNode;
            typeof( ((REG_IRQ_NODE *)0)->list ) *__mptr = (father);
            accessNode=((REG_IRQ_NODE *)( (char *)__mptr - offsetof(REG_IRQ_NODE,list)) );
            LOG_INF("free father,reg_T(%d)\n",accessNode->reg_T);
            if(father->nextirq!=father)
            {
                head->nextirq=father->nextirq;
                father->nextirq=father;
            }
            else
            {   //                
                head->nextirq=head;
                LOG_INF("break\n");
                break;
            }
            kfree(accessNode);
        }
    }
    #endif
    //
    device_destroy(pIspClass, IspDevNo);
    //
    class_destroy(pIspClass);
    pIspClass = NULL;
    //
    return 0;
}

/*                                                                              
 
                                                                               */
static MINT32 bPass1_On_In_Resume_TG1 = 0;
static MINT32 bPass1_On_In_Resume_TG2 = 0;

#define CAMERA_HW_DRVNAME1  "kd_camera_hw"

/*                                                                                                                                                              */
static void backRegister()
{
    //            
    MUINT32 i;
    MUINT32* pReg;

    pReg = &g_backupReg.CAM_CTL_START; 
    for (i= 0; i <= 0x3c; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+i))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_CTL_SEL_GLOBAL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x4040));

    g_backupReg.CAM_CTL_INT_P1_EN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x4048));
    g_backupReg.CAM_CTL_INT_P1_EN2 = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x4050));
    g_backupReg.CAM_CTL_INT_P1_EN_D = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x4058));
    g_backupReg.CAM_CTL_INT_P1_EN2_D = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x4060));
    g_backupReg.CAM_CTL_INT_P2_EN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x4068));
    g_backupReg.CAM_CTL_TILE = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x407C));
    g_backupReg.CAM_CTL_TCM_EN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x4084));

    pReg = &g_backupReg.CAM_CTL_SW_CTL; 
    for (i= 0x8C; i <= 0xFC; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x8C)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_CTL_CLK_EN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x4170));

    pReg = &g_backupReg.CAM_TG_SEN_MODE; 
    for (i= 0x410; i <= 0x424; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x410)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    pReg = &g_backupReg.CAM_OBC_OFFST0; 
    for (i= 0x500; i <= 0xD04; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x500)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }


    pReg = &g_backupReg.CAM_EIS_PREP_ME_CTRL1; 
    for (i= 0xDC0; i <= 0xE50; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0xDC0)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    pReg = &g_backupReg.CAM_SL2_CEN; 
    for (i= 0xF40; i <= 0xFD8; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0xF40)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }
    g_backupReg.CAM_GGM_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x5480));
    g_backupReg.CAM_PCA_CON1 = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x5E00));
    g_backupReg.CAM_PCA_CON2 = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x5E04));
    g_backupReg.CAM_TILE_RING_CON1 = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x5FF0));
    g_backupReg.CAM_CTL_IMGI_SIZE = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x5FF4));


    pReg = &g_backupReg.CAM_TG2_SEN_MODE; 
    for (i= 0x2410; i <= 0x2424; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x2410)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    pReg = &g_backupReg.CAM_OBC_D_OFFST0; 
    for (i= 0x2500; i <= 0x2854; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x2500)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    pReg = &g_backupReg.CAM_DMX_D_CTL; 
    for (i= 0x2E00; i <= 0x2E28; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x2E00)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_TDRI_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7204));
    pReg = &g_backupReg.CAM_TDRI_XSIZE; 
    for (i= 0x720C; i <= 0x7220; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x720C)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_LAST_ULTRA_EN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7228));
    g_backupReg.CAM_IMGI_SLOW_DOWN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x722C));
    g_backupReg.CAM_IMGI_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7230));

    pReg = &g_backupReg.CAM_IMGI_XSIZE; 
    for (i= 0x3238; i <= 0x324C; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3238)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }


    g_backupReg.CAM_BPCI_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7250));
    pReg = &g_backupReg.CAM_BPCI_XSIZE; 
    for (i= 0x3258; i <= 0x3268; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3258)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_LSCI_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x726C));
    pReg = &g_backupReg.CAM_LSCI_XSIZE; 
    for (i= 0x3274; i <= 0x3284; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3274)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_UFDI_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7288));
    pReg = &g_backupReg.CAM_UFDI_XSIZE; 
    for (i= 0x3290; i <= 0x32A4; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3290)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    pReg = &g_backupReg.CAM_LCEI_XSIZE; 
    for (i= 0x32AC; i <= 0x32C0; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x32AC)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    pReg = &g_backupReg.CAM_VIPI_XSIZE; 
    for (i= 0x32C8; i <= 0x32E0; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x32C8)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    pReg = &g_backupReg.CAM_VIP2I_XSIZE; 
    for (i= 0x32E8; i <= 0x32FC; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x32E8)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }


    g_backupReg.CAM_IMGO_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7300));
    pReg = &g_backupReg.CAM_IMGO_XSIZE; 
    for (i= 0x3308; i <= 0x331C; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3308)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_RRZO_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7320));
    pReg = &g_backupReg.CAM_RRZO_XSIZE; 
    for (i= 0x3328; i <= 0x333C; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3328)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_LCSO_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7340));
    pReg = &g_backupReg.CAM_LCSO_XSIZE; 
    for (i= 0x3348; i <= 0x3358; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3348)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    pReg = &g_backupReg.CAM_EISO_BASE_ADDR; 
    for (i= 0x335C; i <= 0x3370; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x335C)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    pReg = &g_backupReg.CAM_ESFKO_YSIZE; 
    for (i= 0x3378; i <= 0x3384; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3378)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_AAO_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7388));
    pReg = &g_backupReg.CAM_AAO_XSIZE; 
    for (i= 0x3390; i <= 0x33A0; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3390)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_VIP3I_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x73A4));
    pReg = &g_backupReg.CAM_VIP3I_XSIZE; 
    for (i= 0x33AC; i <= 0x33C0; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x33AC)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_UFEO_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x73C4));
    pReg = &g_backupReg.CAM_UFEO_XSIZE; 
    for (i= 0x33CC; i <= 0x33DC; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x33CC)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_MFBO_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x73E0));
    pReg = &g_backupReg.CAM_MFBO_XSIZE; 
    for (i= 0x33E8; i <= 0x33FC; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x33E8)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_IMG3BO_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7400));
    pReg = &g_backupReg.CAM_IMG3BO_XSIZE; 
    for (i= 0x3408; i <= 0x341C; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3408)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_IMG3CO_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7420));
    pReg = &g_backupReg.CAM_IMG3CO_XSIZE; 
    for (i= 0x3428; i <= 0x343C; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3428)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_IMG2O_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7440));
    pReg = &g_backupReg.CAM_IMG2O_XSIZE; 
    for (i= 0x3448; i <= 0x345C; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3448)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_IMG3O_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7460));
    pReg = &g_backupReg.CAM_IMG3O_XSIZE; 
    for (i= 0x3468; i <= 0x347C; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3468)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_FEO_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7480));
    pReg = &g_backupReg.CAM_FEO_XSIZE; 
    for (i= 0x3488; i <= 0x3498; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3488)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }


    g_backupReg.CAM_BPCI_D_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x749C));
    pReg = &g_backupReg.CAM_BPCI_D_XSIZE; 
    for (i= 0x34A4; i <= 0x34B4; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x34A4)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_LSCI_D_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x74B8));
    pReg = &g_backupReg.CAM_LSCI_D_XSIZE; 
    for (i= 0x34C0; i <= 0x34D0; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x34C0)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_IMGO_D_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x74D4));
    pReg = &g_backupReg.CAM_IMGO_D_XSIZE; 
    for (i= 0x34DC; i <= 0x34F0; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x34DC)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_RRZO_D_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x74F4));
    pReg = &g_backupReg.CAM_RRZO_D_XSIZE; 
    for (i= 0x34FC; i <= 0x3510; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x34FC)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_LCSO_D_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7514));
    pReg = &g_backupReg.CAM_LCSO_D_XSIZE; 
    for (i= 0x351C; i <= 0x352C; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x351C)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_AFO_D_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7530));
    g_backupReg.CAM_AFO_D_XSIZE = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x7534));
    pReg = &g_backupReg.CAM_AFO_D_YSIZE; 
    for (i= 0x353C; i <= 0x3548; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x353C)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }

    g_backupReg.CAM_AAO_D_BASE_ADDR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x754C));
    pReg = &g_backupReg.CAM_AAO_D_XSIZE; 
    for (i= 0x3554; i <= 0x3564; i += 4)    
    {
        (*((MUINT32*)((unsigned long)pReg+(i-0x3554)))) = (MUINT32)ISP_RD32(ISP_ADDR + i);  
    }


    g_SeninfBackupReg.SENINF_TOP_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8000));
    g_SeninfBackupReg.SENINF_TOP_CMODEL_PAR = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8004));
    g_SeninfBackupReg.SENINF_TOP_MUX_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8008));

    g_SeninfBackupReg.N3D_CTL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x80C0));
    g_SeninfBackupReg.N3D_POS = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x80C4));
    g_SeninfBackupReg.N3D_TRIG = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x80C8));
    g_SeninfBackupReg.N3D_INT = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x80CC));

//        
    g_SeninfBackupReg.SENINF1_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8100));
    g_SeninfBackupReg.SENINF1_MUX_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8120));
    g_SeninfBackupReg.SENINF1_MUX_INTEN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8124));
    g_SeninfBackupReg.SENINF1_MUX_SIZE = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x812C));

    g_SeninfBackupReg.SENINF_TG1_PH_CNT = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8200));
    g_SeninfBackupReg.SENINF_TG1_SEN_CK = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8204));

    g_SeninfBackupReg.SENINF1_CSI2_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8360));
    g_SeninfBackupReg.SENINF1_CSI2_DELAY = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8364));
    g_SeninfBackupReg.SENINF1_CSI2_INTEN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8368));

    g_SeninfBackupReg.SENINF1_NCSI2_CTL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x83A0));
    g_SeninfBackupReg.SENINF1_NCSI2_LNRC_TIMING = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x83A4));
    g_SeninfBackupReg.SENINF1_NCSI2_LNRD_TIMING = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x83A8));
    g_SeninfBackupReg.SENINF1_NCSI2_INT_EN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x83B0));
    g_SeninfBackupReg.SENINF1_NCSI2_DI_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x83E4));
//        
    g_SeninfBackupReg.SENINF2_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8500));
    g_SeninfBackupReg.SENINF2_MUX_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8520));
    g_SeninfBackupReg.SENINF2_MUX_INTEN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8524));
    g_SeninfBackupReg.SENINF2_MUX_SIZE = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x852C));

    g_SeninfBackupReg.SENINF_TG2_PH_CNT = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8600));
    g_SeninfBackupReg.SENINF_TG2_SEN_CK = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8604));

    g_SeninfBackupReg.SENINF2_CSI2_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8760));
    g_SeninfBackupReg.SENINF2_CSI2_DELAY = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8764));
    g_SeninfBackupReg.SENINF2_CSI2_INTEN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x8768));

    g_SeninfBackupReg.SENINF2_NCSI2_CTL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x87A0));
    g_SeninfBackupReg.SENINF2_NCSI2_LNRC_TIMING = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x87A4));
    g_SeninfBackupReg.SENINF2_NCSI2_LNRD_TIMING = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x87A8));
    g_SeninfBackupReg.SENINF2_NCSI2_INT_EN = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x87B0));
    g_SeninfBackupReg.SENINF2_NCSI2_DI_CTRL = ISP_RD32((void *)(ISP_IMGSYS_BASE + 0x87E4));

    //                  

    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_000 = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x0));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_004 = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x4));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_008 = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x8));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_00C = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0xC));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_010 = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x10));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_014 = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x14));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_018 = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x18));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_01C = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x1C));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_020 = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x20));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_04C = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x4C));
    g_SeninfBackupReg.MIPIRX_ANALOG_BASE_050 = ISP_RD32((void *)(ISP_MIPI_ANA_ADDR + 0x50));
}
void ConfigM4uPort(void)
{
    //                                                                                                         
    M4U_PORT_STRUCT port;

    //                             

    port.Virtuality = 1;
    port.Security = 0;
    port.domain = 0;
    port.Distance = 1;
    port.Direction = 0; //                  
    
    port.ePortID = M4U_PORT_MDP_WDMA;
    m4u_config_port(&port);
    
    port.ePortID = M4U_PORT_MDP_WROT;
    m4u_config_port(&port);
    
    port.ePortID = M4U_PORT_IMGO;
    m4u_config_port(&port);
    
    port.ePortID = M4U_PORT_RRZO;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_AAO;
    m4u_config_port(&port);
    
    port.ePortID = M4U_PORT_ESFKO;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_IMGO_S;
    m4u_config_port(&port);
    
    port.ePortID = M4U_PORT_LSCI;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_LSCI_D;
    m4u_config_port(&port);
    
    port.ePortID = M4U_PORT_BPCI;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_BPCI_D;
    m4u_config_port(&port);
    
    port.ePortID = M4U_PORT_UFDI;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_IMGI;
    m4u_config_port(&port);
    
    port.ePortID = M4U_PORT_IMG2O;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_IMG3O;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_VIPI;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_VIP2I;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_VIP3I;
    m4u_config_port(&port);

    port.ePortID = M4U_PORT_LCEI;
    m4u_config_port(&port);


}
static void restoreRegister(MUINT32 openSensorIdx)
{
    MUINT32 i;
    MUINT32* pReg;
    unsigned int temp = 0;

    ConfigM4uPort();
    
    pReg = &g_backupReg.CAM_CTL_START;   
    for (i= 0x0; i <= 0x3c; i += 4)    
    {
        if (0x20 == i)
        {
            //                                     
            temp = ((MUINT32)(*(pReg))) & 0xEBFF7BFF;
            ISP_WR32((ISP_ADDR + i), temp);       
            pReg = (MUINT32*)((unsigned long)pReg+4);  
        }
        else
        {
            ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
            pReg = (MUINT32*)((unsigned long)pReg+4);  
        }
    }   

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4040), g_backupReg.CAM_CTL_SEL_GLOBAL);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4048), g_backupReg.CAM_CTL_INT_P1_EN);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4050), g_backupReg.CAM_CTL_INT_P1_EN2);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4058), g_backupReg.CAM_CTL_INT_P1_EN_D);

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4060), g_backupReg.CAM_CTL_INT_P1_EN2_D);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4068), g_backupReg.CAM_CTL_INT_P2_EN);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x407C), g_backupReg.CAM_CTL_TILE);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4084), g_backupReg.CAM_CTL_TCM_EN);


    pReg = &g_backupReg.CAM_CTL_SW_CTL;   
    for (i= 0x8C; i <= 0xFC; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }   

#if 0
    //                     
    temp = g_backupReg.CAM_CTL_CQ_EN;
    temp = temp & 0Xff77ff77;

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4020), temp);
    //                     
    switch (openSensorIdx)
    {
        case 1:
            ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4000), (0x00000020));
#if 0
            if ((g_backupReg.CAM_CTL_EN_P1 & 0x00000A02) == 0x00000A02)
            {
                //                
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4000), (0x00001020));
            }
            else
            {
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4000), (0x00000020));
            }
#endif
            break;
        case 2:
            ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4000), (0x00001000));
            break;
        case 3:
            ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4000), (0x00001020));
            break;
        default:
            break;
    }
    //              
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4020), g_backupReg.CAM_CTL_CQ_EN);
    
#endif

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x4170), g_backupReg.CAM_CTL_CLK_EN);

    pReg = &g_backupReg.CAM_TG_SEN_MODE; 
    for (i= 0x410; i <= 0x424; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }   

    pReg = &g_backupReg.CAM_OBC_OFFST0; 
    for (i= 0x500; i <= 0xD04; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }   

    pReg = &g_backupReg.CAM_EIS_PREP_ME_CTRL1; 
    for (i= 0xDC0; i <= 0xE50; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }   

    pReg = &g_backupReg.CAM_TG2_SEN_MODE; 
    for (i= 0x2410; i <= 0x2424; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }   

    pReg = &g_backupReg.CAM_OBC_D_OFFST0; 
    for (i= 0x2500; i <= 0x2854; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }   

    pReg = &g_backupReg.CAM_DMX_D_CTL; 
    for (i= 0x2E00; i <= 0x2E28; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }   

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7204), g_backupReg.CAM_TDRI_BASE_ADDR);
    pReg = &g_backupReg.CAM_TDRI_XSIZE; 
    for (i= 0x720C; i <= 0x7220; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7228), g_backupReg.CAM_LAST_ULTRA_EN);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x722C), g_backupReg.CAM_IMGI_SLOW_DOWN);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7230), g_backupReg.CAM_IMGI_BASE_ADDR);

    pReg = &g_backupReg.CAM_IMGI_XSIZE; 
    for (i= 0x3238; i <= 0x324C; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }


    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7250), g_backupReg.CAM_BPCI_BASE_ADDR);
    pReg = &g_backupReg.CAM_BPCI_XSIZE; 
    for (i= 0x3258; i <= 0x3268; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }   

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x726C), g_backupReg.CAM_LSCI_BASE_ADDR);
    pReg = &g_backupReg.CAM_LSCI_XSIZE; 
    for (i= 0x3274; i <= 0x3284; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }   

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7288), g_backupReg.CAM_UFDI_BASE_ADDR);
    pReg = &g_backupReg.CAM_UFDI_XSIZE; 
    for (i= 0x3290; i <= 0x32A4; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }

    pReg = &g_backupReg.CAM_LCEI_XSIZE; 
    for (i= 0x32AC; i <= 0x32C0; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4); 
    }

    pReg = &g_backupReg.CAM_VIPI_XSIZE; 
    for (i= 0x32C8; i <= 0x32E0; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);
    }

    pReg = &g_backupReg.CAM_VIP2I_XSIZE; 
    for (i= 0x32E8; i <= 0x32FC; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);
    }


    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7300), g_backupReg.CAM_IMGO_BASE_ADDR);
    pReg = &g_backupReg.CAM_IMGO_XSIZE; 
    for (i= 0x3308; i <= 0x331C; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    } 

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7320), g_backupReg.CAM_RRZO_BASE_ADDR);
    pReg = &g_backupReg.CAM_RRZO_XSIZE; 
    for (i= 0x3328; i <= 0x333C; i += 4)
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    } 

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7340), g_backupReg.CAM_LCSO_BASE_ADDR);
    pReg = &g_backupReg.CAM_LCSO_XSIZE; 
    for (i= 0x3348; i <= 0x3358; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4); 
    }

    pReg = &g_backupReg.CAM_EISO_BASE_ADDR; 
    for (i= 0x335C; i <= 0x3370; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    } 

    pReg = &g_backupReg.CAM_ESFKO_YSIZE; 
    for (i= 0x3378; i <= 0x3384; i += 4)   
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    } 

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7388), g_backupReg.CAM_AAO_BASE_ADDR);
    pReg = &g_backupReg.CAM_AAO_XSIZE; 
    for (i= 0x3390; i <= 0x33A0; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    } 

     ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x73A4), g_backupReg.CAM_VIP3I_BASE_ADDR);
     pReg = &g_backupReg.CAM_VIP3I_XSIZE; 
     for (i= 0x33AC; i <= 0x33C0; i += 4)    
     {
         ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
         pReg = (MUINT32*)((unsigned long)pReg+4);  
     }
    
     ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x73C4), g_backupReg.CAM_UFEO_BASE_ADDR);
     pReg = &g_backupReg.CAM_UFEO_XSIZE; 
     for (i= 0x33CC; i <= 0x33DC; i += 4)    
     {
         ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
         pReg = (MUINT32*)((unsigned long)pReg+4);  
     }

     ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x73E0), g_backupReg.CAM_MFBO_BASE_ADDR);
     pReg = &g_backupReg.CAM_MFBO_XSIZE; 
     for (i= 0x33E8; i <= 0x33FC; i += 4)    
     {
         ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
         pReg = (MUINT32*)((unsigned long)pReg+4);  
     }
    
     ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7400), g_backupReg.CAM_IMG3BO_BASE_ADDR);
     pReg = &g_backupReg.CAM_IMG3BO_XSIZE; 
     for (i= 0x3408; i <= 0x341C; i += 4)    
     {
         ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
         pReg = (MUINT32*)((unsigned long)pReg+4);  
     }
    
     ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7420), g_backupReg.CAM_IMG3CO_BASE_ADDR);
     pReg = &g_backupReg.CAM_IMG3CO_XSIZE; 
     for (i= 0x3428; i <= 0x343C; i += 4)    
     {
         ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
         pReg = (MUINT32*)((unsigned long)pReg+4);  
     }
    
     ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7440), g_backupReg.CAM_IMG2O_BASE_ADDR);
     pReg = &g_backupReg.CAM_IMG2O_XSIZE; 
     for (i= 0x3448; i <= 0x345C; i += 4)    
     {
         ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
         pReg = (MUINT32*)((unsigned long)pReg+4);  
     }
    
     ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7460), g_backupReg.CAM_IMG3O_BASE_ADDR);
     pReg = &g_backupReg.CAM_IMG3O_XSIZE; 
     for (i= 0x3468; i <= 0x347C; i += 4)    
     {
         ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
         pReg = (MUINT32*)((unsigned long)pReg+4);  
     }
    
     ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7480), g_backupReg.CAM_FEO_BASE_ADDR);
     pReg = &g_backupReg.CAM_FEO_XSIZE; 
     for (i= 0x3488; i <= 0x3498; i += 4)    
     {
         ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
         pReg = (MUINT32*)((unsigned long)pReg+4);  
     }


    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x749C), g_backupReg.CAM_BPCI_D_BASE_ADDR);
    pReg = &g_backupReg.CAM_BPCI_D_XSIZE; 
    for (i= 0x34A4; i <= 0x34B4; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }
    
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74B8), g_backupReg.CAM_LSCI_D_BASE_ADDR);
    pReg = &g_backupReg.CAM_LSCI_D_XSIZE; 
    for (i= 0x34C0; i <= 0x34D0; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74D4), g_backupReg.CAM_IMGO_D_BASE_ADDR);
    pReg = &g_backupReg.CAM_IMGO_D_XSIZE; 
    for (i= 0x34DC; i <= 0x34F0; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74F4), g_backupReg.CAM_RRZO_D_BASE_ADDR);
    pReg = &g_backupReg.CAM_RRZO_D_XSIZE; 
    for (i= 0x34FC; i <= 0x3510; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7514), g_backupReg.CAM_LCSO_D_BASE_ADDR);
    pReg = &g_backupReg.CAM_LCSO_D_XSIZE; 
    for (i= 0x351C; i <= 0x352C; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4); 
    }

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7530), g_backupReg.CAM_AFO_D_BASE_ADDR);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7534), g_backupReg.CAM_AFO_D_XSIZE);
    pReg = &g_backupReg.CAM_AFO_D_YSIZE; 
    for (i= 0x353C; i <= 0x3548; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x754C), g_backupReg.CAM_AAO_D_BASE_ADDR);
    pReg = &g_backupReg.CAM_AAO_D_XSIZE; 
    for (i= 0x3554; i <= 0x3564; i += 4)    
    {
        ISP_WR32((ISP_ADDR + i), (MUINT32)(*(pReg)));       
        pReg = (MUINT32*)((unsigned long)pReg+4);  
    }

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8000), g_SeninfBackupReg.SENINF_TOP_CTRL);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8004), g_SeninfBackupReg.SENINF_TOP_CMODEL_PAR);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8008), g_SeninfBackupReg.SENINF_TOP_MUX_CTRL);


    //                                                                        
    if (0x01 == openSensorIdx)
    {
        ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x80C0), 0x746);
    }
    else
    {
        ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x80C0), 0x946);
    }

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x80C4), g_SeninfBackupReg.N3D_POS);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x80C8), g_SeninfBackupReg.N3D_TRIG);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x80CC), g_SeninfBackupReg.N3D_INT);

//        
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8100), g_SeninfBackupReg.SENINF1_CTRL);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8120), g_SeninfBackupReg.SENINF1_MUX_CTRL);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8124), g_SeninfBackupReg.SENINF1_MUX_INTEN);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x812C), g_SeninfBackupReg.SENINF1_MUX_SIZE);

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8200), g_SeninfBackupReg.SENINF_TG1_PH_CNT);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8204), g_SeninfBackupReg.SENINF_TG1_SEN_CK);

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8360), g_SeninfBackupReg.SENINF1_CSI2_CTRL);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8364), g_SeninfBackupReg.SENINF1_CSI2_DELAY);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8368), g_SeninfBackupReg.SENINF1_CSI2_INTEN);

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x83A4), g_SeninfBackupReg.SENINF1_NCSI2_LNRC_TIMING);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x83A8), g_SeninfBackupReg.SENINF1_NCSI2_LNRD_TIMING);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x83B0), g_SeninfBackupReg.SENINF1_NCSI2_INT_EN);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x83E4), g_SeninfBackupReg.SENINF1_NCSI2_DI_CTRL);

    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x4C), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_04C & 0xFEFBEFBE));//                            
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x50), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_050 & 0xFEFBEFBE));//                             
    //                                            
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x0), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_000 | 0x00000008));//                            
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x4), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_004 | 0x00000008));//                             
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x8), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_008 | 0x00000008));//                             
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0xC), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_00C | 0x00000008));//                             
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x10), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_010 | 0x00000008));//                             

    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x24), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_024 | 0x00000001));//                 
    mDELAY(1);
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x20), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_020 | 0x00000001));//                   
    mDELAY(2);
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x0), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_000 | 0x00000009));//                       
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x4), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_004 | 0x00000009));//                        
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x8), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_008 | 0x00000009));//                        
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0xC), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_00C | 0x00000009));//                        
    ISP_WR32((void *)(ISP_MIPI_ANA_ADDR + 0x10), (g_SeninfBackupReg.MIPIRX_ANALOG_BASE_010 | 0x00000009));//                        


    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x83A0), g_SeninfBackupReg.SENINF1_NCSI2_CTL);

    temp = g_SeninfBackupReg.SENINF1_MUX_CTRL;
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8120), temp|0x3); //     
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8120), temp&0xFFFFFFFC); //           

//        
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8500), g_SeninfBackupReg.SENINF2_CTRL);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8520), g_SeninfBackupReg.SENINF2_MUX_CTRL);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8524), g_SeninfBackupReg.SENINF2_MUX_INTEN);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x852C), g_SeninfBackupReg.SENINF2_MUX_SIZE);

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8600), g_SeninfBackupReg.SENINF_TG2_PH_CNT);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8604), g_SeninfBackupReg.SENINF_TG2_SEN_CK);

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8760), g_SeninfBackupReg.SENINF2_CSI2_CTRL);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8764), g_SeninfBackupReg.SENINF2_CSI2_DELAY);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8768), g_SeninfBackupReg.SENINF2_CSI2_INTEN);

    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x87A4), g_SeninfBackupReg.SENINF2_NCSI2_LNRC_TIMING);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x87A8), g_SeninfBackupReg.SENINF2_NCSI2_LNRD_TIMING);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x87B0), g_SeninfBackupReg.SENINF2_NCSI2_INT_EN);
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x87E4), g_SeninfBackupReg.SENINF2_NCSI2_DI_CTRL);


    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x87A0), g_SeninfBackupReg.SENINF2_NCSI2_CTL);

    temp = g_SeninfBackupReg.SENINF2_MUX_CTRL;
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8520), temp|0x3); //     
    ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x8520), temp&0xFFFFFFFC); //           

}

static void sensorPowerOn()
{
    MUINT32 ret = ERROR_NONE;
    MINT32 i = 0;
    MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT imageWindow;
    MSDK_SENSOR_CONFIG_STRUCT sensorConfigData;
    memset(&imageWindow, 0, sizeof(ACDK_SENSOR_EXPOSURE_WINDOW_STRUCT));
    memset(&sensorConfigData, 0, sizeof(ACDK_SENSOR_CONFIG_STRUCT));

    for ( i = (KDIMGSENSOR_MAX_INVOKE_DRIVERS-1) ; i >= KDIMGSENSOR_INVOKE_DRIVER_0 ; i-- ) {
        if ( g_bEnableDriver[i] && g_pInvokeSensorFunc[i] ) {
            //              
            ret = kdCISModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM)g_invokeSocketIdx[i],(char*)g_invokeSensorNameStr[i],true,CAMERA_HW_DRVNAME1);
            if ( ERROR_NONE != ret ) {
                LOG_ERR("[%s]",__FUNCTION__);
                return ret;
            }
            //                     
            mDELAY(10);
            LOG_DBG("kdModulePowerOn");
    
            ret = g_pInvokeSensorFunc[i]->SensorOpen();
            if ( ERROR_NONE != ret ) {
                kdCISModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM)g_invokeSocketIdx[i],(char*)g_invokeSensorNameStr[i],false,CAMERA_HW_DRVNAME1);
                LOG_ERR("SensorOpen");
                return ret;
            }

            memcpy(&imageWindow, &g_pInvokeSensorFunc[i]->imageWindow, sizeof(ACDK_SENSOR_EXPOSURE_WINDOW_STRUCT));
            memcpy(&sensorConfigData, &g_pInvokeSensorFunc[i]->sensorConfigData, sizeof(ACDK_SENSOR_CONFIG_STRUCT));

            ret = g_pInvokeSensorFunc[i]->SensorControl(g_pInvokeSensorFunc[i]->ScenarioId,&imageWindow,&sensorConfigData);
            if ( ERROR_NONE != ret ) {
                LOG_ERR("ERR:SensorControl(), i =%d\n",i);
                return ret;
            }

            //                
            //                                    
            //                                                                       
        }
    }

}


static void sensorPowerOff()    
{
    MUINT32 ret = ERROR_NONE;
    MINT32 i = 0;

    for ( i = (KDIMGSENSOR_MAX_INVOKE_DRIVERS-1) ; i >= KDIMGSENSOR_INVOKE_DRIVER_0 ; i-- ) {
        if ( g_bEnableDriver[i] && g_pInvokeSensorFunc[i] ) {
            //               
            ret = kdCISModulePowerOn((CAMERA_DUAL_CAMERA_SENSOR_ENUM)g_invokeSocketIdx[i],(char*)g_invokeSensorNameStr[i],false,CAMERA_HW_DRVNAME1);
            if ( ERROR_NONE != ret ) {
                LOG_ERR("[%s]",__FUNCTION__);
                return ret;
            }
        }
    }

}

static MINT32 ISP_suspend(
    struct platform_device* pDev,
    pm_message_t            Mesg
)
{
    MUINT32 regTG1Val = ISP_RD32(ISP_ADDR + 0x414);
    MUINT32 regTG2Val = ISP_RD32(ISP_ADDR + 0x2414);
#ifdef AP_DONT_STOP_PREVIEW
    ISP_WAIT_IRQ_STRUCT waitirq;
    MINT32 ret = 0;  
#endif
    LOG_DBG("bPass1_On_In_Resume_TG1(%d). bPass1_On_In_Resume_TG2(%d). regTG1Val(0x%08x). regTG2Val(0x%08x) \n", bPass1_On_In_Resume_TG1, bPass1_On_In_Resume_TG2, regTG1Val, regTG2Val);

    bPass1_On_In_Resume_TG1 = 0;
    if ( regTG1Val & 0x01 )    //                     
    {
        bPass1_On_In_Resume_TG1 = 1;
        ISP_WR32(ISP_ADDR + 0x414, (regTG1Val&(~0x01)) );
#ifdef AP_DONT_STOP_PREVIEW
        waitirq.Clear=ISP_IRQ_CLEAR_WAIT;   
        waitirq.Type=ISP_IRQ_TYPE_INT_P1_ST;    
        waitirq.Status=ISP_IRQ_P1_STATUS_PASS1_DON_ST;      
        waitirq.Timeout=100;  
        waitirq.UserNumber=0;  
        waitirq.UserInfo.Type=ISP_IRQ_TYPE_INT_P1_ST;     
        waitirq.UserInfo.Status=ISP_IRQ_P1_STATUS_PASS1_DON_ST;    
        waitirq.UserInfo.UserKey=0;    
        ret=ISP_WaitIrq_v3(&waitirq);
#endif
    }

    bPass1_On_In_Resume_TG2 = 0;
    if ( regTG2Val & 0x01 )    //                    
{
        bPass1_On_In_Resume_TG2 = 1;
        ISP_WR32(ISP_ADDR + 0x2414, (regTG2Val&(~0x01)) );
#ifdef AP_DONT_STOP_PREVIEW
        waitirq.Clear=ISP_IRQ_CLEAR_WAIT;   
        waitirq.Type=ISP_IRQ_TYPE_INT_P1_ST_D;    
        waitirq.Status=ISP_IRQ_P1_STATUS_D_PASS1_DON_ST;      
        waitirq.Timeout=100;  
        waitirq.UserNumber=0;  
        waitirq.UserInfo.Type=ISP_IRQ_TYPE_INT_P1_ST_D;     
        waitirq.UserInfo.Status=ISP_IRQ_P1_STATUS_D_PASS1_DON_ST;    
        waitirq.UserInfo.UserKey=0;    
        ret=ISP_WaitIrq_v3(&waitirq);
#endif
    }
#ifdef AP_DONT_STOP_PREVIEW
    if ((regTG1Val & 0x01 )||(regTG2Val & 0x01 ))
    {
        sensorPowerOff();
        backRegister();
        //              
        ISP_EnableClock(MFALSE);
    }
#endif
    return 0;
}

/*                                                                              
 
                                                                               */
static MINT32 ISP_resume(struct platform_device* pDev)
{
    MUINT32 regTG1Val;
    MUINT32 regTG2Val;
#ifdef AP_DONT_STOP_PREVIEW
    MUINT32 openSensorIdx = 0;  //                                                 
    ISP_WAIT_IRQ_STRUCT waitirq;
    MINT32 ret = 0;  
    int j;
    unsigned int previousBufpos;
    int backup_TotalCnt;
    int backup_WCNT;
    int backup_RCNT;
    int backup_FBCCNT;
    int backup_TotalCnt_D;
    int backup_WCNT_D;
    int backup_RCNT_D;
    int backup_FBCCNT_D;
    UINT32 tempIMGOBaseAddr;
    UINT32 tempRRZOBaseAddr;
    UINT32 tempIMGOBaseAddr_D;
    UINT32 tempRRZOBaseAddr_D;    
    //                                                                                  
    bResumeSignal = 1;
#endif

    LOG_DBG("bPass1_On_In_Resume_TG1(%d). bPass1_On_In_Resume_TG2(%d). regTG1Val(0x%x) regTG2Val(0x%x) \n", bPass1_On_In_Resume_TG1, bPass1_On_In_Resume_TG2, regTG1Val, regTG2Val);
    if (( bPass1_On_In_Resume_TG1 )||(bPass1_On_In_Resume_TG2))
    {

#ifdef AP_DONT_STOP_PREVIEW
        //                                                                                           
        //           
        if(pstRTBuf->ring_buf[_rrzo_].active)
        {
            backup_WCNT = (g_backupReg.CAM_CTL_RRZO_FBC & 0x0f000000)  >> 24;
            backup_RCNT = (g_backupReg.CAM_CTL_RRZO_FBC & 0x00f00000)  >> 20;
            backup_TotalCnt = pstRTBuf->ring_buf[_rrzo_].total_count;
            backup_FBCCNT = (g_backupReg.CAM_CTL_RRZO_FBC & 0x0000000f);
            for (j=0 ; j<ISP_RT_BUF_SIZE; j++)
            {
                LOG_DBG("rrzo addr[%d]:0x%x\n",j, pstRTBuf->ring_buf[_rrzo_].data[j].base_pAddr);
                if (pstRTBuf->ring_buf[_rrzo_].data[j].base_pAddr == g_backupReg.CAM_RRZO_BASE_ADDR)
                {
                    LOG_DBG("rrzo find the same addr\n");
                }
            }
        
            }
        else if (pstRTBuf->ring_buf[_imgo_].active)
        {
            backup_WCNT = (g_backupReg.CAM_CTL_IMGO_FBC & 0x0f000000)  >> 24;
            backup_RCNT = (g_backupReg.CAM_CTL_IMGO_FBC & 0x00f00000)  >> 20;
            backup_TotalCnt = pstRTBuf->ring_buf[_imgo_].total_count;
            backup_FBCCNT = (g_backupReg.CAM_CTL_IMGO_FBC & 0x0000000f);
            for (j=0 ; j<ISP_RT_BUF_SIZE; j++)
            {
                LOG_DBG("imgo addr[%d]:0x%x\n",j, pstRTBuf->ring_buf[_imgo_].data[j].base_pAddr);
                if (pstRTBuf->ring_buf[_imgo_].data[j].base_pAddr == g_backupReg.CAM_IMGO_BASE_ADDR)
                {
                    LOG_DBG("imgo find the same addr\n");
                }
            }
        }
        if((pstRTBuf->ring_buf[_imgo_].active) || (pstRTBuf->ring_buf[_rrzo_].active))
        {
           LOG_DBG("imgo start(%d) rrzo start(%d)\n", pstRTBuf->ring_buf[_imgo_].start, pstRTBuf->ring_buf[_rrzo_].start);
           LOG_DBG("main WCNT(%d), RCNT(%d)\n", backup_WCNT, backup_RCNT);
        }
        //          
        if(pstRTBuf->ring_buf[_rrzo_d_].active)
        {
            
            backup_WCNT_D = (g_backupReg.CAM_CTL_RRZO_D_FBC & 0x0f000000)  >> 24;
            backup_RCNT_D = (g_backupReg.CAM_CTL_RRZO_D_FBC & 0x00f00000)  >> 20;
            backup_TotalCnt_D = pstRTBuf->ring_buf[_rrzo_d_].total_count;
            backup_FBCCNT_D = (g_backupReg.CAM_CTL_RRZO_D_FBC & 0x0000000f);
            for (j=0 ; j<ISP_RT_BUF_SIZE; j++)
            {
                LOG_DBG("rrzo_d addr[%d]:0x%x\n",j, pstRTBuf->ring_buf[_rrzo_d_].data[j].base_pAddr);
                if (pstRTBuf->ring_buf[_rrzo_d_].data[j].base_pAddr == g_backupReg.CAM_RRZO_D_BASE_ADDR)
                {
                    LOG_DBG("rrzo_d find the same addr\n");
                }
            }
        
        }
        else if(pstRTBuf->ring_buf[_imgo_d_].active)
        {
            backup_WCNT_D = (g_backupReg.CAM_CTL_IMGO_D_FBC & 0x0f000000)  >> 24;
            backup_RCNT_D = (g_backupReg.CAM_CTL_IMGO_D_FBC & 0x00f00000)  >> 20;
            backup_TotalCnt_D = pstRTBuf->ring_buf[_imgo_d_].total_count;
            backup_FBCCNT_D = (g_backupReg.CAM_CTL_IMGO_D_FBC & 0x0000000f);
            for (j=0 ; j<ISP_RT_BUF_SIZE; j++)
            {
                LOG_DBG("imgo_d addr[%d]:0x%x\n",j, pstRTBuf->ring_buf[_imgo_d_].data[j].base_pAddr);
                if (pstRTBuf->ring_buf[_imgo_d_].data[j].base_pAddr == g_backupReg.CAM_IMGO_D_BASE_ADDR)
                {
                    LOG_DBG("imgo_d find the same addr\n");
                }
            }
        }
        if((pstRTBuf->ring_buf[_imgo_d_].active) || (pstRTBuf->ring_buf[_rrzo_d_].active))
        {
            LOG_DBG("imgo_d start(%d) rrzo_d start(%d)\n", pstRTBuf->ring_buf[_imgo_d_].start, pstRTBuf->ring_buf[_rrzo_d_].start);
            LOG_DBG("sub WCNT(%d), RCNT(%d)\n", backup_WCNT, backup_RCNT);
        }

        ISP_EnableClock(MTRUE);
        if ( bPass1_On_In_Resume_TG2 ) {
            openSensorIdx = 0x1;
        }
        restoreRegister(openSensorIdx);
        //           
        if (backup_WCNT == backup_RCNT)
        {
            if (backup_FBCCNT > 0)
            {
                //                              
                LOG_DBG("ori imgo addr(0x%x) rrzo start(0x%x)\n", g_backupReg.CAM_IMGO_BASE_ADDR, g_backupReg.CAM_RRZO_BASE_ADDR);
                previousBufpos = ((backup_WCNT+2)-1)%backup_TotalCnt;
                tempIMGOBaseAddr = pstRTBuf->ring_buf[_imgo_].data[previousBufpos].base_pAddr;
                tempRRZOBaseAddr = pstRTBuf->ring_buf[_rrzo_].data[previousBufpos].base_pAddr;
                LOG_DBG("temp imgo addr(0x%x) rrzo addr(0x%x)\n", tempIMGOBaseAddr, tempRRZOBaseAddr);
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7300), tempIMGOBaseAddr);
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7320), tempRRZOBaseAddr);
            }
        }
        else
        {
            //                                        
        }

        //          
        if (backup_WCNT_D == backup_RCNT_D)
        {
            if (backup_FBCCNT_D > 0)
            {
                //                              
                LOG_DBG("ori imgo_d addr(0x%x) rrzo_d start(0x%x)\n", g_backupReg.CAM_IMGO_D_BASE_ADDR, g_backupReg.CAM_RRZO_D_BASE_ADDR);
                previousBufpos = ((backup_WCNT_D+2)-1)%backup_TotalCnt_D;
                tempIMGOBaseAddr_D = pstRTBuf->ring_buf[_imgo_d_].data[previousBufpos].base_pAddr;
                tempRRZOBaseAddr_D = pstRTBuf->ring_buf[_rrzo_d_].data[previousBufpos].base_pAddr;
                LOG_DBG("temp imgo_d addr(0x%x) rrz_do addr(0x%x)\n", tempIMGOBaseAddr_D, tempRRZOBaseAddr_D);
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74D4), tempIMGOBaseAddr_D);
                ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74F4), tempRRZOBaseAddr_D);
            }
        }
        else
        {
            //                                        
        }
        sensorPowerOn();
        //              
#endif

        regTG1Val = ISP_RD32(ISP_ADDR + 0x414);
        regTG2Val = ISP_RD32(ISP_ADDR + 0x2414);

        if ( bPass1_On_In_Resume_TG1 ) {
            bPass1_On_In_Resume_TG1 = 0;
            //                      
            ISP_WR32(ISP_ADDR + 0x414, (regTG1Val|0x01) );    //                     

#ifdef AP_DONT_STOP_PREVIEW
            //                                                
            waitirq.Clear=ISP_IRQ_CLEAR_WAIT;   
            waitirq.Type=ISP_IRQ_TYPE_INT_P1_ST;    
            waitirq.Status=ISP_IRQ_P1_STATUS_PESUDO_P1_DON_ST;      
            waitirq.Timeout=500;  //                           
            waitirq.UserNumber=0;  
            waitirq.UserInfo.Type=ISP_IRQ_TYPE_INT_P1_ST;     
            waitirq.UserInfo.Status=ISP_IRQ_P1_STATUS_PESUDO_P1_DON_ST;    
            waitirq.UserInfo.UserKey=0;    
            ret=ISP_WaitIrq_v3(&waitirq);
            //                   
            ISP_WR32(ISP_ADDR + 0x20, (g_backupReg.CAM_CTL_CQ_EN & 0xffff7bff));
            ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7300), g_backupReg.CAM_IMGO_BASE_ADDR);
            ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x7320), g_backupReg.CAM_RRZO_BASE_ADDR);
#endif

        }
        
        if ( bPass1_On_In_Resume_TG2 ) {
            bPass1_On_In_Resume_TG2 = 0;
            ISP_WR32(ISP_ADDR + 0x2414, (regTG2Val|0x01) );    //                    

#ifdef AP_DONT_STOP_PREVIEW
            waitirq.Clear=ISP_IRQ_CLEAR_WAIT;   
            waitirq.Type=ISP_IRQ_TYPE_INT_P1_ST_D;    
            waitirq.Status=ISP_IRQ_P1_STATUS_D_PESUDO_P1_DON_ST;      
            waitirq.Timeout=500;  //                             
            waitirq.UserNumber=0;  
            waitirq.UserInfo.Type=ISP_IRQ_TYPE_INT_P1_ST_D;     
            waitirq.UserInfo.Status=ISP_IRQ_P1_STATUS_D_PESUDO_P1_DON_ST;    
            waitirq.UserInfo.UserKey=0;    
            ret=ISP_WaitIrq_v3(&waitirq);
            //                               
            ISP_WR32(ISP_ADDR + 0x20, g_backupReg.CAM_CTL_CQ_EN);
            ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74D4), g_backupReg.CAM_IMGO_D_BASE_ADDR);
            ISP_WR32((void *)(ISP_IMGSYS_BASE + 0x74F4), g_backupReg.CAM_RRZO_D_BASE_ADDR);
#endif
        }
#ifdef AP_DONT_STOP_PREVIEW
        //                                                               
        //                                                                     
        //                                  
        ISP_WR32(ISP_ADDR + 0x20, g_backupReg.CAM_CTL_CQ_EN);
#endif
    }

#ifdef AP_DONT_STOP_PREVIEW
    //                                                                        
    bResumeSignal = 0;
#endif

    return 0;
}

/*                                                                           */
#ifdef CONFIG_PM
/*                                                                           */
int ISP_pm_suspend(struct device *device)
{
    struct platform_device *pdev = to_platform_device(device);
    BUG_ON(pdev == NULL);

    pr_debug("calling %s()\n", __func__);

    return ISP_suspend(pdev, PMSG_SUSPEND);
}

int ISP_pm_resume(struct device *device)
{
    struct platform_device *pdev = to_platform_device(device);
    BUG_ON(pdev == NULL);

    pr_debug("calling %s()\n", __func__);

    return ISP_resume(pdev);
}

extern void mt_irq_set_sens(unsigned int irq, unsigned int sens);
extern void mt_irq_set_polarity(unsigned int irq, unsigned int polarity);
int ISP_pm_restore_noirq(struct device *device)
{
    pr_debug("calling %s()\n", __func__);

    mt_irq_set_sens(CAM0_IRQ_BIT_ID, MT_LEVEL_SENSITIVE);
    mt_irq_set_polarity(CAM0_IRQ_BIT_ID, MT_POLARITY_LOW);

    return 0;

}
/*                                                                           */
#else /*         */
/*                                                                           */
#define ISP_pm_suspend NULL
#define ISP_pm_resume  NULL
#define ISP_pm_restore_noirq NULL
/*                                                                           */
#endif /*         */
/*                                                                           */

#ifdef CONFIG_OF
static const struct of_device_id isp_of_ids[] = {
    { .compatible = "mediatek,ISPSYS", },
    {}
};
#endif

struct dev_pm_ops ISP_pm_ops = {
    .suspend = ISP_pm_suspend,
    .resume = ISP_pm_resume,
    .freeze = ISP_pm_suspend,
    .thaw = ISP_pm_resume,
    .poweroff = ISP_pm_suspend,
    .restore = ISP_pm_resume,
    .restore_noirq = ISP_pm_restore_noirq,
};


/*                                                                              
 
                                                                               */
static struct platform_driver IspDriver =
{
    .probe   = ISP_probe,
    .remove  = ISP_remove,
    .suspend = ISP_suspend,
    .resume  = ISP_resume,
    .driver  = {
        .name  = ISP_DEV_NAME,
        .owner = THIS_MODULE,
#ifdef CONFIG_OF
        .of_match_table = isp_of_ids,
#endif
#ifdef CONFIG_PM
        .pm     = &ISP_pm_ops,
#endif
    }
};

/*                                                                              
 
                                                                               */
static MINT32 ISP_DumpRegToProc(
    char*   pPage,
    char**  ppStart,
    off_t   off,
    MINT32  Count,
    MINT32* pEof,
    void*   pData)
{
    char *p = pPage;
    MINT32 Length = 0;
    MUINT32 i = 0;
    MINT32 ret = 0;
    //
    LOG_DBG("- E. pPage: %p. off: %d. Count: %d.", pPage, (unsigned int)off, Count);
    //
    p += sprintf(p, " MT6593 ISP Register\n");
    p += sprintf(p,"====== top ====\n");
    for(i = 0x0; i <= 0x1AC; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    p += sprintf(p,"====== dma ====\n");
    for(i = 0x200; i <= 0x3D8; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n\r", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    p += sprintf(p,"====== tg ====\n");
    for(i = 0x400; i <= 0x4EC; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    p += sprintf(p,"====== cdp (including EIS) ====\n");
    for(i = 0xB00; i <= 0xDE0; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    p += sprintf(p,"====== seninf ====\n");
    for(i = 0x4000; i <= 0x40C0; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    for(i = 0x4100; i <= 0x41BC; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    for(i = 0x4200; i <= 0x4208; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    for(i = 0x4300; i <= 0x4310; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    for(i = 0x43A0; i <= 0x43B0; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    for(i = 0x4400; i <= 0x4424; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    for(i = 0x4500; i <= 0x4520; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    for(i = 0x4600; i <= 0x4608; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    for(i = 0x4A00; i <= 0x4A08; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
     p += sprintf(p,"====== 3DNR ====\n");
    for(i = 0x4F00; i <= 0x4F38; i += 4)
    {
        p += sprintf(p,"+0x%08x 0x%08x\n", (unsigned int)(ISP_ADDR + i), (unsigned int)ISP_RD32(ISP_ADDR + i));
    }
    //
    *ppStart = pPage + off;
    //
    Length = p - pPage;
    if(Length > off)
    {
        Length -= off;
    }
    else
    {
        Length = 0;
    }
    //

    ret = Length < Count ? Length : Count;

    LOG_DBG("- X. ret: %d.", ret);

    return ret;
}

/*                                                                              
 
                                                                               */
static MINT32  ISP_RegDebug(
    struct file*    pFile,
    const char*     pBuffer,
    unsigned long   Count,
    void*           pData)
{
    char RegBuf[64];
    MUINT32 CopyBufSize = (Count < (sizeof(RegBuf) - 1)) ? (Count) : (sizeof(RegBuf) - 1);
    MUINT32 Addr = 0;
    MUINT32 Data = 0;

    LOG_DBG("- E. pFile: %p. pBuffer: %p. Count: %d.", pFile, pBuffer, (int)Count);
    //
    if(copy_from_user(RegBuf, pBuffer, CopyBufSize))
    {
        LOG_ERR("copy_from_user() fail.");
        return -EFAULT;
    }

    //
    if (sscanf(RegBuf, "%x %x",  &Addr, &Data) == 2)
    {
        ISP_WR32(ISP_ADDR_CAMINF + Addr,Data);
        LOG_DBG("Write => Addr: 0x%08X, Write Data: 0x%08X. Read Data: 0x%08X.", (int)(ISP_ADDR_CAMINF + Addr), (int)Data, (int)ioread32(ISP_ADDR_CAMINF + Addr));
    }
    else if (sscanf(RegBuf, "%x", &Addr) == 1)
    {
        LOG_DBG("Read => Addr: 0x%08X, Read Data: 0x%08X.", (int)(ISP_ADDR_CAMINF + Addr), (int)ioread32(ISP_ADDR_CAMINF + Addr));
    }
    //
    LOG_DBG("- X. Count: %d.", (int)Count);
    return Count;
}

static MUINT32 proc_regOfst = 0;
static MINT32 CAMIO_DumpRegToProc(
    char*   pPage,
    char**  ppStart,
    off_t   off,
    MINT32  Count,
    MINT32* pEof,
    void*   pData)
{
    char *p = pPage;
    MINT32 Length = 0;
    MINT32 ret = 0;
    //
    LOG_DBG("- E. pPage: %p. off: %d. Count: %d.", pPage, (int)off, Count);
    //
    p += sprintf(p,"reg_0x%lx = 0x%08x \n", ISP_ADDR_CAMINF+proc_regOfst , ioread32(ISP_ADDR_CAMINF + proc_regOfst));

    *ppStart = pPage + off;
    //
    Length = p - pPage;
    if(Length > off)
    {
        Length -= off;
    }
    else
    {
        Length = 0;
    }
    //

    ret = Length < Count ? Length : Count;

    LOG_DBG("- X. ret: %d.", ret);

    return ret;
}

/*                                                                              
 
                                                                               */
static MINT32  CAMIO_RegDebug(
    struct file*    pFile,
    const char*     pBuffer,
    unsigned long   Count,
    void*           pData)
{
    char RegBuf[64];
    MUINT32 CopyBufSize = (Count < (sizeof(RegBuf) - 1)) ? (Count) : (sizeof(RegBuf) - 1);
    MUINT32 Addr = 0;
    MUINT32 Data = 0;
    LOG_DBG("- E. pFile: %p. pBuffer: %p. Count: %d.", pFile, pBuffer, (int)Count);

    //
    if(copy_from_user(RegBuf, pBuffer, CopyBufSize))
    {
        LOG_ERR("copy_from_user() fail.");
        return -EFAULT;
    }

    //
    if (sscanf(RegBuf, "%x %x",  &Addr, &Data) == 2)
    {
        proc_regOfst = Addr;
        ISP_WR32(ISP_GPIO_ADDR + Addr,Data);
        LOG_DBG("Write => Addr: 0x%08X, Write Data: 0x%08X. Read Data: 0x%08X.", (int)(ISP_GPIO_ADDR + Addr), (int)Data, (int)ioread32(ISP_GPIO_ADDR + Addr));
    }
    else if (sscanf(RegBuf, "%x", &Addr) == 1)
    {
        proc_regOfst = Addr;
        LOG_DBG("Read => Addr: 0x%08X, Read Data: 0x%08X.", (int)(ISP_GPIO_ADDR + Addr), (int)ioread32(ISP_GPIO_ADDR + Addr));
    }
    //
    LOG_DBG("- X. Count: %d.", (int)Count);
    return Count;
}
/*                                                                              
 
                                                                               */
static const struct file_operations fcameraisp_proc_fops = {
    .read = ISP_DumpRegToProc,
    .write = ISP_RegDebug,
};
static const struct file_operations fcameraio_proc_fops = {
    .read = CAMIO_DumpRegToProc,
    .write = CAMIO_RegDebug,
};
/*                                                                              
 
                                                                               */

static MINT32 __init ISP_Init(void)
{
    MINT32 Ret = 0,j;
    void* tmp;
#if 0
    struct proc_dir_entry*  pEntry;
#endif
    int i;
    //
    LOG_DBG("- E.");
    //
    if((Ret = platform_driver_register(&IspDriver)) < 0)
    {
        LOG_ERR("platform_driver_register fail");
        return Ret;
    }
    //
//                                     
#if 1
    proc_create("driver/isp_reg",0,NULL,&fcameraisp_proc_fops);
    proc_create("driver/camio_reg",0,NULL,&fcameraio_proc_fops);
#else
    pEntry = create_proc_entry("driver/isp_reg", 0, NULL);
    if(pEntry)
    {
        pEntry->read_proc = ISP_DumpRegToProc;
        pEntry->write_proc = ISP_RegDebug;
    }
    else
    {
        LOG_ERR("add /proc/driver/isp_reg entry fail");
    }

    pEntry = create_proc_entry("driver/camio_reg", 0, NULL);
    if(pEntry)
    {
        pEntry->read_proc = CAMIO_DumpRegToProc;
        pEntry->write_proc = CAMIO_RegDebug;
    }
    else
    {
        LOG_ERR("add /proc/driver/camio_reg entry fail");
    }
#endif
    //
    /*                                                                            */
    //                                         

    if(sizeof(ISP_RT_BUF_STRUCT) > ((RT_BUF_TBL_NPAGES ) * PAGE_SIZE)){
        i=0;
        while(i<sizeof(ISP_RT_BUF_STRUCT)){
            i += PAGE_SIZE;

        }
        if ((pBuf_kmalloc = kmalloc(i+2*PAGE_SIZE, GFP_KERNEL)) == NULL) {
                LOG_ERR("mem not enough\n");
                return -ENOMEM;
        }
        memset(pBuf_kmalloc,0x00,i);
    }
    else
    {
        if ((pBuf_kmalloc = kmalloc((RT_BUF_TBL_NPAGES + 2) * PAGE_SIZE, GFP_KERNEL)) == NULL) {
                LOG_ERR("mem not enough\n");
                return -ENOMEM;
        }
        memset(pBuf_kmalloc,0x00,RT_BUF_TBL_NPAGES*PAGE_SIZE);
    }    
    /*                                 */
    pTbl_RTBuf = (int *)((((unsigned long)pBuf_kmalloc) + PAGE_SIZE - 1) & PAGE_MASK);
    pstRTBuf = (ISP_RT_BUF_STRUCT*)pTbl_RTBuf;
    pstRTBuf->state = ISP_RTBC_STATE_INIT;
    //       
    if(PAGE_SIZE < (( _IRQ_MAX* NORMAL_STR_LEN*((DBG_PAGE+INF_PAGE+ERR_PAGE)+1))*LOG_PPNUM)) {
        i=0;
        while(i<(( _IRQ_MAX* NORMAL_STR_LEN*((DBG_PAGE+INF_PAGE+ERR_PAGE)+1))*LOG_PPNUM)) {
            i += PAGE_SIZE;
        }
    }
    else{
        i = PAGE_SIZE;
    }
    if ((pLog_kmalloc = kmalloc(i, GFP_KERNEL)) == NULL) {
            LOG_ERR("mem not enough\n");
            return -ENOMEM;
    }
    memset(pLog_kmalloc,0x00,i);
    tmp = pLog_kmalloc;
    for(i=0;i<LOG_PPNUM;i++){
        for(j=0;j<_IRQ_MAX;j++) {
            gSvLog[j]._str[i][_LOG_DBG] = (char*)tmp;
            //                                                              
            tmp = (void*) ((char*)tmp + (NORMAL_STR_LEN*DBG_PAGE));
            gSvLog[j]._str[i][_LOG_INF] = (char*)tmp;
            //                                                              
            tmp = (void*) ((char*)tmp + (NORMAL_STR_LEN*INF_PAGE));
            gSvLog[j]._str[i][_LOG_ERR] = (char*)tmp;
            //                                                              
            tmp = (void*) ((char*)tmp + (NORMAL_STR_LEN*ERR_PAGE));
        }
        //                                                                                     
        tmp = (void*) ((char*)tmp + NORMAL_STR_LEN); //                               
    }
    /*                                   */
    for (i = 0; i < RT_BUF_TBL_NPAGES * PAGE_SIZE; i+= PAGE_SIZE) {
        SetPageReserved(virt_to_page(((unsigned long)pTbl_RTBuf) + i));
    }
    //
    //                      
    LOG_DBG("register isp callback for MDP");
    cmdqCoreRegisterCB(CMDQ_GROUP_ISP,
                       ISP_MDPClockOnCallback,
                       ISP_MDPDumpCallback,
                       ISP_MDPResetCallback,
                       ISP_MDPClockOffCallback);
    //                                               
    LOG_DBG("register isp callback for GCE");
    cmdqCoreRegisterDebugRegDumpCB(ISP_BeginGCECallback, ISP_EndGCECallback);

    //                           
    LOG_DBG("register M4U callback dump");
    m4u_register_fault_callback(M4U_PORT_IMGI, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_IMGO, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_RRZO, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_AAO, ISP_M4U_TranslationFault_callback, NULL);
    //                                                                                    
    m4u_register_fault_callback(M4U_PORT_ESFKO, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_IMGO_S, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_LSCI, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_LSCI_D, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_BPCI, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_BPCI_D, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_UFDI, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_IMG2O, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_IMG3O, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_VIPI, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_VIP2I, ISP_M4U_TranslationFault_callback, NULL);
    m4u_register_fault_callback(M4U_PORT_VIP3I, ISP_M4U_TranslationFault_callback, NULL);


#ifdef _MAGIC_NUM_ERR_HANDLING_
    LOG_DBG("init m_LastMNum");
    for ( i=0; i<_rt_dma_max_;i++ ) {
        m_LastMNum[i] = 0;
    }
#endif


    LOG_DBG("- X. Ret: %d.", Ret);
    return Ret;
}

/*                                                                              
 
                                                                               */
static void __exit ISP_Exit(void)
{
    int i;
    LOG_DBG("- E.");
    //
    platform_driver_unregister(&IspDriver);
    //
    //                        
    cmdqCoreRegisterCB(CMDQ_GROUP_ISP,
                       NULL,
                       NULL,
                       NULL,
                       NULL);
    //                         
    LOG_DBG("Un-register isp callback for GCE");
    cmdqCoreRegisterDebugRegDumpCB(NULL, NULL);
    //
    //                              
    LOG_DBG("Un-Register M4U callback dump");
    m4u_unregister_fault_callback(M4U_PORT_IMGI);


    /*                     */
    for (i = 0; i < RT_BUF_TBL_NPAGES * PAGE_SIZE; i+= PAGE_SIZE) {
            SetPageReserved(virt_to_page(((unsigned long)pTbl_RTBuf) + i));
    }
    /*                       */
    kfree(pBuf_kmalloc);
    kfree(pLog_kmalloc);

    //
}

/*                                                                              
 
                                                                               */
MBOOL ISP_RegCallback(ISP_CALLBACK_STRUCT* pCallback)
{
    //
    if(pCallback == NULL)
    {
        LOG_ERR("pCallback is null");
        return MFALSE;
    }
    //
    if(pCallback->Func == NULL)
    {
        LOG_ERR("Func is null");
        return MFALSE;
    }
    //
    LOG_DBG("Type(%d)",pCallback->Type);
    IspInfo.Callback[pCallback->Type].Func = pCallback->Func;
    //
    return MTRUE;
}

/*                                                                              
 
                                                                               */
MBOOL ISP_UnregCallback(ISP_CALLBACK_ENUM   Type)
{
    if(Type > ISP_CALLBACK_AMOUNT)
    {
        LOG_ERR("Type(%d) must smaller than %d",Type,ISP_CALLBACK_AMOUNT);
        return MFALSE;
    }
    //
    LOG_DBG("Type(%d)",Type);
    IspInfo.Callback[Type].Func = NULL;
    //
    return MTRUE;
}

void ISP_MCLK1_EN(BOOL En)
{
    static MUINT32 mMclk1User = 0;
    if(1 == En)
        mMclk1User++;
    else
        mMclk1User--;
    
    MUINT32 temp=0;
    temp = ISP_RD32(ISP_ADDR + 0x4200);
    if(En)
    {
        if(mMclk1User > 0)
        {
            temp |= 0x20000000;
            ISP_WR32(ISP_ADDR + 0x4200,temp);
        }
    }
    else
    {
        if(mMclk1User == 0)
        {
            temp &= 0xDFFFFFFF;
            ISP_WR32(ISP_ADDR + 0x4200,temp);
        }
    }
    temp = ISP_RD32(ISP_ADDR + 0x4200);
    LOG_INF("ISP_MCLK1_EN(0x%lx), mMclk1User(%d)",temp,mMclk1User);

}

void ISP_MCLK2_EN(BOOL En)
{
    static MUINT32 mMclk2User = 0;
    if(1 == En)
        mMclk2User++;
    else
        mMclk2User--;
    
    MUINT32 temp=0;
    temp = ISP_RD32(ISP_ADDR + 0x4600);
    if(En)
    {
        if(mMclk2User > 0)
        {
            temp |= 0x20000000;
            ISP_WR32(ISP_ADDR + 0x4600,temp);
        }
    }
    else
    {
        if(mMclk2User == 0)
        {
            temp &= 0xDFFFFFFF;
            ISP_WR32(ISP_ADDR + 0x4600,temp);
        }
    }
    LOG_INF("ISP_MCLK2_EN(%lx), mMclk2User(%d)",temp,mMclk2User);
}

void ISP_MCLK3_EN(BOOL En)
{
    static MUINT32 mMclk3User = 0;
    if(1 == En)
        mMclk3User++;
    else
        mMclk3User--;
    
    MUINT32 temp=0;
    temp = ISP_RD32(ISP_ADDR + 0x4A00);
    if(En)
    {
        if(mMclk3User > 0)
        {
            temp |= 0x20000000;
            ISP_WR32(ISP_ADDR + 0x4A00,temp);
        }
    }
    else
    {
        if(mMclk3User == 0)
        {
            temp &= 0xDFFFFFFF;
            ISP_WR32(ISP_ADDR + 0x4A00,temp);
        }
    }
    LOG_INF("ISP_MCLK3_EN(%lx), mMclk3User(%d)",temp,mMclk3User);

}

int32_t ISP_MDPClockOnCallback(uint64_t engineFlag)
{
    //                                  
    LOG_DBG("+MDPEn:%d", G_u4EnableClockCount);
    ISP_EnableClock(MTRUE);

    return 0;
}

int32_t ISP_MDPDumpCallback(uint64_t engineFlag,
                            int level)
{
    LOG_DBG("ISP_MDPDumpCallback");

    ISP_DumpReg();

    return 0;
}
int32_t ISP_MDPResetCallback(uint64_t engineFlag)
{
    LOG_DBG("ISP_MDPResetCallback");

    ISP_Reset(ISP_REG_SW_CTL_RST_CAM_P2);

    return 0;
}

int32_t ISP_MDPClockOffCallback(uint64_t engineFlag)
{
    //                                   
    ISP_EnableClock(MFALSE);
    LOG_DBG("-MDPEn:%d", G_u4EnableClockCount);
    return 0;
}


static uint32_t *addressToDump[] = {
#if CONFIG_OF
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4018),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x401C),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4024),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4030),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x403C),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4040),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4080),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4084),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4088),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x40A0),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x40A4),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x40A8),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48A0),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48A4),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48A8),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48AC),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48B0),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48B4),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48B8),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48BC),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48C0),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48C4),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48C8),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48CC),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48D0),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48D4),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48D8),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48DC),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48E0),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48E4),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x48E8),
	//
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4948),
	//
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B00),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B04),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B08),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B0C),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B10),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B14),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B18),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B1C),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B20),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B24),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B28),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B2C),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B30),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B34),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x4B38),
	//
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x7204),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x7208),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x720C),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x7230),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x7240),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x7288),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x72a4),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x7300),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x7320),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x7440),
	(uint32_t*)(ISP_IMGSYS_BASE_PHY + 0x7460)

#else
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4018),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x401C),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4024),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4030),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x403C),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4040),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4080),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4084),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4088),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x40A0),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x40A4),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x40A8),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48A0),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48A4),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48A8),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48AC),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48B0),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48B4),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48B8),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48BC),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48C0),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48C4),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48C8),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48CC),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48D0),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48D4),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48D8),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48DC),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48E0),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48E4),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x48E8),
    //
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4948),
    //
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B00),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B04),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B08),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B0C),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B10),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B14),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B18),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B1C),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B20),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B24),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B28),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B2C),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B30),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B34),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x4B38),
    //
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x7204),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x7208),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x720C),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x7230),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x7240),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x7288),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x72a4),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x7300),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x7320),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x7440),
    (uint32_t*)IO_VIRT_TO_PHYS(ISP_IMGSYS_BASE + 0x7460)
#endif
    };

int32_t ISP_BeginGCECallback(uint32_t taskID, uint32_t *regCount, uint32_t **regAddress)
{
    LOG_DBG("+,taskID(%d)",taskID);

    *regCount = sizeof(addressToDump)/sizeof(uint32_t);
    *regAddress = (uint32_t *)addressToDump;

    LOG_DBG("-,*regCount(%d)",*regCount);

    return 0;
}

int32_t ISP_EndGCECallback(uint32_t taskID, uint32_t regCount, uint32_t *regValues)
{
#define PER_LINE_LOG_SIZE   10
    int32_t i, j, pos;
    //                                
    uint32_t* add[PER_LINE_LOG_SIZE];
    uint32_t val[PER_LINE_LOG_SIZE];

    LOG_DBG("End taskID(%d),regCount(%d)",taskID,regCount);

    for(i=0;i< regCount; i+=PER_LINE_LOG_SIZE){
        for(j=0; j<PER_LINE_LOG_SIZE; j++){
            pos = i + j;
            if(pos < regCount){
                //                                              
                add[j] = addressToDump[pos];
                val[j] = regValues[pos];
            }
        }

        //                                                                                                                                                                     
        //                                                                                                                                                 
        LOG_DBG("[0x%p,0x%08x][0x%p,0x%08x][0x%p,0x%08x][0x%p,0x%08x][0x%p,0x%08x][0x%p,0x%08x][0x%p,0x%08x][0x%p,0x%08x][0x%p,0x%08x][0x%p,0x%08x]\n", \
            add[0],val[0],add[1],val[1],add[2],val[2],add[3],val[3],add[4],val[4],add[5],val[5],add[6],val[6],add[7],val[7],add[8],val[8],add[9],val[9]);
    }

    return 0;
}

m4u_callback_ret_t ISP_M4U_TranslationFault_callback(int port, unsigned int mva, void* data)
{
    LOG_DBG("[ISP_M4U]fault call port=%d, mva=0x%x", port, mva);

    switch(port){
        case M4U_PORT_IMGO:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0004), (unsigned int)ISP_RD32(ISP_ADDR + 0x0004));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0008), (unsigned int)ISP_RD32(ISP_ADDR + 0x0008));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0010), (unsigned int)ISP_RD32(ISP_ADDR + 0x0010));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0014), (unsigned int)ISP_RD32(ISP_ADDR + 0x0014));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0020), (unsigned int)ISP_RD32(ISP_ADDR + 0x0020));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00ac), (unsigned int)ISP_RD32(ISP_ADDR + 0x00ac));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x00bc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00c0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3204), (unsigned int)ISP_RD32(ISP_ADDR + 0x3204));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3300), (unsigned int)ISP_RD32(ISP_ADDR + 0x3300));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3304), (unsigned int)ISP_RD32(ISP_ADDR + 0x3304));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3308), (unsigned int)ISP_RD32(ISP_ADDR + 0x3308));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x330c), (unsigned int)ISP_RD32(ISP_ADDR + 0x330c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3310), (unsigned int)ISP_RD32(ISP_ADDR + 0x3310));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3314), (unsigned int)ISP_RD32(ISP_ADDR + 0x3314));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3318), (unsigned int)ISP_RD32(ISP_ADDR + 0x3318));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x331c), (unsigned int)ISP_RD32(ISP_ADDR + 0x331c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3320), (unsigned int)ISP_RD32(ISP_ADDR + 0x3320));
            break;
        case M4U_PORT_RRZO:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0004), (unsigned int)ISP_RD32(ISP_ADDR + 0x0004));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0008), (unsigned int)ISP_RD32(ISP_ADDR + 0x0008));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0010), (unsigned int)ISP_RD32(ISP_ADDR + 0x0010));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0014), (unsigned int)ISP_RD32(ISP_ADDR + 0x0014));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0020), (unsigned int)ISP_RD32(ISP_ADDR + 0x0020));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00ac), (unsigned int)ISP_RD32(ISP_ADDR + 0x00ac));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x00bc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00c0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3204), (unsigned int)ISP_RD32(ISP_ADDR + 0x3204));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3320), (unsigned int)ISP_RD32(ISP_ADDR + 0x3320));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3324), (unsigned int)ISP_RD32(ISP_ADDR + 0x3324));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3328), (unsigned int)ISP_RD32(ISP_ADDR + 0x3328));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x332c), (unsigned int)ISP_RD32(ISP_ADDR + 0x332c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3330), (unsigned int)ISP_RD32(ISP_ADDR + 0x3330));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3334), (unsigned int)ISP_RD32(ISP_ADDR + 0x3334));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3338), (unsigned int)ISP_RD32(ISP_ADDR + 0x3338));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x333c), (unsigned int)ISP_RD32(ISP_ADDR + 0x333c));
            break;
        case M4U_PORT_AAO:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0004), (unsigned int)ISP_RD32(ISP_ADDR + 0x0004));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0008), (unsigned int)ISP_RD32(ISP_ADDR + 0x0008));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0010), (unsigned int)ISP_RD32(ISP_ADDR + 0x0010));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0014), (unsigned int)ISP_RD32(ISP_ADDR + 0x0014));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0020), (unsigned int)ISP_RD32(ISP_ADDR + 0x0020));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00ac), (unsigned int)ISP_RD32(ISP_ADDR + 0x00ac));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x00bc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00c0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3364), (unsigned int)ISP_RD32(ISP_ADDR + 0x3364));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3368), (unsigned int)ISP_RD32(ISP_ADDR + 0x3368));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3388), (unsigned int)ISP_RD32(ISP_ADDR + 0x3388));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x338c), (unsigned int)ISP_RD32(ISP_ADDR + 0x338c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3390), (unsigned int)ISP_RD32(ISP_ADDR + 0x3390));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3394), (unsigned int)ISP_RD32(ISP_ADDR + 0x3394));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3398), (unsigned int)ISP_RD32(ISP_ADDR + 0x3398));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x339c), (unsigned int)ISP_RD32(ISP_ADDR + 0x339c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x33a0), (unsigned int)ISP_RD32(ISP_ADDR + 0x33a0));
            break;
#if 0
        case M4U_PORT_LCSO:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0004), (unsigned int)ISP_RD32(ISP_ADDR + 0x0004));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0008), (unsigned int)ISP_RD32(ISP_ADDR + 0x0008));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0010), (unsigned int)ISP_RD32(ISP_ADDR + 0x0010));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0014), (unsigned int)ISP_RD32(ISP_ADDR + 0x0014));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0020), (unsigned int)ISP_RD32(ISP_ADDR + 0x0020));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00ac), (unsigned int)ISP_RD32(ISP_ADDR + 0x00ac));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x00bc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00c0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3340), (unsigned int)ISP_RD32(ISP_ADDR + 0x3340));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3344), (unsigned int)ISP_RD32(ISP_ADDR + 0x3344));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3348), (unsigned int)ISP_RD32(ISP_ADDR + 0x3348));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x334c), (unsigned int)ISP_RD32(ISP_ADDR + 0x334c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3350), (unsigned int)ISP_RD32(ISP_ADDR + 0x3350));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3354), (unsigned int)ISP_RD32(ISP_ADDR + 0x3354));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3358), (unsigned int)ISP_RD32(ISP_ADDR + 0x3358));
            break;
#endif
        case M4U_PORT_ESFKO:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0004), (unsigned int)ISP_RD32(ISP_ADDR + 0x0004));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0008), (unsigned int)ISP_RD32(ISP_ADDR + 0x0008));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0010), (unsigned int)ISP_RD32(ISP_ADDR + 0x0010));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0014), (unsigned int)ISP_RD32(ISP_ADDR + 0x0014));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0020), (unsigned int)ISP_RD32(ISP_ADDR + 0x0020));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00ac), (unsigned int)ISP_RD32(ISP_ADDR + 0x00ac));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x00bc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00c0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x335c), (unsigned int)ISP_RD32(ISP_ADDR + 0x335c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3360), (unsigned int)ISP_RD32(ISP_ADDR + 0x3360));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x336c), (unsigned int)ISP_RD32(ISP_ADDR + 0x336c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3370), (unsigned int)ISP_RD32(ISP_ADDR + 0x3370));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3374), (unsigned int)ISP_RD32(ISP_ADDR + 0x3374));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3378), (unsigned int)ISP_RD32(ISP_ADDR + 0x3378));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x337c), (unsigned int)ISP_RD32(ISP_ADDR + 0x337c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3380), (unsigned int)ISP_RD32(ISP_ADDR + 0x3380));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3384), (unsigned int)ISP_RD32(ISP_ADDR + 0x3384));
            break;
        case M4U_PORT_IMGO_S:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0004), (unsigned int)ISP_RD32(ISP_ADDR + 0x0004));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0008), (unsigned int)ISP_RD32(ISP_ADDR + 0x0008));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0010), (unsigned int)ISP_RD32(ISP_ADDR + 0x0010));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0014), (unsigned int)ISP_RD32(ISP_ADDR + 0x0014));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0020), (unsigned int)ISP_RD32(ISP_ADDR + 0x0020));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00c4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00c4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00c8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00c8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00cc), (unsigned int)ISP_RD32(ISP_ADDR + 0x00cc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00d0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00d0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00d4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00d4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00d8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00d8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34d4), (unsigned int)ISP_RD32(ISP_ADDR + 0x34d4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34d8), (unsigned int)ISP_RD32(ISP_ADDR + 0x34d8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34dc), (unsigned int)ISP_RD32(ISP_ADDR + 0x34dc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34e0), (unsigned int)ISP_RD32(ISP_ADDR + 0x34e0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34e4), (unsigned int)ISP_RD32(ISP_ADDR + 0x34e4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34e8), (unsigned int)ISP_RD32(ISP_ADDR + 0x34e8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34ec), (unsigned int)ISP_RD32(ISP_ADDR + 0x34ec));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34f0), (unsigned int)ISP_RD32(ISP_ADDR + 0x34f0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34f4), (unsigned int)ISP_RD32(ISP_ADDR + 0x34f4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34f8), (unsigned int)ISP_RD32(ISP_ADDR + 0x34f8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34fc), (unsigned int)ISP_RD32(ISP_ADDR + 0x34fc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3500), (unsigned int)ISP_RD32(ISP_ADDR + 0x3500));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3504), (unsigned int)ISP_RD32(ISP_ADDR + 0x3504));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3508), (unsigned int)ISP_RD32(ISP_ADDR + 0x3508));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x350c), (unsigned int)ISP_RD32(ISP_ADDR + 0x350c));
            break;
        case M4U_PORT_LSCI:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0004), (unsigned int)ISP_RD32(ISP_ADDR + 0x0004));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0008), (unsigned int)ISP_RD32(ISP_ADDR + 0x0008));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0010), (unsigned int)ISP_RD32(ISP_ADDR + 0x0010));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0014), (unsigned int)ISP_RD32(ISP_ADDR + 0x0014));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0020), (unsigned int)ISP_RD32(ISP_ADDR + 0x0020));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00ac), (unsigned int)ISP_RD32(ISP_ADDR + 0x00ac));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x00bc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00c0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x326c), (unsigned int)ISP_RD32(ISP_ADDR + 0x326c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3270), (unsigned int)ISP_RD32(ISP_ADDR + 0x3270));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3274), (unsigned int)ISP_RD32(ISP_ADDR + 0x3274));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3278), (unsigned int)ISP_RD32(ISP_ADDR + 0x3278));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x327c), (unsigned int)ISP_RD32(ISP_ADDR + 0x327c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3280), (unsigned int)ISP_RD32(ISP_ADDR + 0x3280));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3284), (unsigned int)ISP_RD32(ISP_ADDR + 0x3284));
            break;
        case M4U_PORT_LSCI_D:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0004), (unsigned int)ISP_RD32(ISP_ADDR + 0x0004));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0008), (unsigned int)ISP_RD32(ISP_ADDR + 0x0008));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0010), (unsigned int)ISP_RD32(ISP_ADDR + 0x0010));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0014), (unsigned int)ISP_RD32(ISP_ADDR + 0x0014));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0020), (unsigned int)ISP_RD32(ISP_ADDR + 0x0020));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00ac), (unsigned int)ISP_RD32(ISP_ADDR + 0x00ac));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x00bc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00c0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x34b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x34bc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x34c0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34c4), (unsigned int)ISP_RD32(ISP_ADDR + 0x34c4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34c8), (unsigned int)ISP_RD32(ISP_ADDR + 0x34c8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34cc), (unsigned int)ISP_RD32(ISP_ADDR + 0x34cc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x34d0), (unsigned int)ISP_RD32(ISP_ADDR + 0x34d0));
            break;
        case M4U_PORT_BPCI:
        case M4U_PORT_BPCI_D:
            break;
        case M4U_PORT_IMGI:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0018), (unsigned int)ISP_RD32(ISP_ADDR + 0x0018));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x001c), (unsigned int)ISP_RD32(ISP_ADDR + 0x001c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3204), (unsigned int)ISP_RD32(ISP_ADDR + 0x3204));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3230), (unsigned int)ISP_RD32(ISP_ADDR + 0x3230));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3230), (unsigned int)ISP_RD32(ISP_ADDR + 0x3230));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3234), (unsigned int)ISP_RD32(ISP_ADDR + 0x3234));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3238), (unsigned int)ISP_RD32(ISP_ADDR + 0x3238));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x323c), (unsigned int)ISP_RD32(ISP_ADDR + 0x323c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3240), (unsigned int)ISP_RD32(ISP_ADDR + 0x3240));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3248), (unsigned int)ISP_RD32(ISP_ADDR + 0x3248));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x324c), (unsigned int)ISP_RD32(ISP_ADDR + 0x324c));
            break;
        case M4U_PORT_LCEI:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0018), (unsigned int)ISP_RD32(ISP_ADDR + 0x0018));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x001c), (unsigned int)ISP_RD32(ISP_ADDR + 0x001c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3204), (unsigned int)ISP_RD32(ISP_ADDR + 0x3204));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32a4), (unsigned int)ISP_RD32(ISP_ADDR + 0x32a4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32a8), (unsigned int)ISP_RD32(ISP_ADDR + 0x32a8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32ac), (unsigned int)ISP_RD32(ISP_ADDR + 0x32ac));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32b0), (unsigned int)ISP_RD32(ISP_ADDR + 0x32b0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32b4), (unsigned int)ISP_RD32(ISP_ADDR + 0x32b4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x32b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x32bc));
            break;
        case M4U_PORT_UFDI:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0018), (unsigned int)ISP_RD32(ISP_ADDR + 0x0018));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x001c), (unsigned int)ISP_RD32(ISP_ADDR + 0x001c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3204), (unsigned int)ISP_RD32(ISP_ADDR + 0x3204));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3288), (unsigned int)ISP_RD32(ISP_ADDR + 0x3288));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x328c), (unsigned int)ISP_RD32(ISP_ADDR + 0x328c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3290), (unsigned int)ISP_RD32(ISP_ADDR + 0x3290));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3294), (unsigned int)ISP_RD32(ISP_ADDR + 0x3294));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3298), (unsigned int)ISP_RD32(ISP_ADDR + 0x3298));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x329c), (unsigned int)ISP_RD32(ISP_ADDR + 0x329c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32a0), (unsigned int)ISP_RD32(ISP_ADDR + 0x32a0));
            break;
        case M4U_PORT_IMG2O:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0018), (unsigned int)ISP_RD32(ISP_ADDR + 0x0018));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x001c), (unsigned int)ISP_RD32(ISP_ADDR + 0x001c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3440), (unsigned int)ISP_RD32(ISP_ADDR + 0x3440));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3444), (unsigned int)ISP_RD32(ISP_ADDR + 0x3444));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3448), (unsigned int)ISP_RD32(ISP_ADDR + 0x3448));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x344c), (unsigned int)ISP_RD32(ISP_ADDR + 0x344c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3450), (unsigned int)ISP_RD32(ISP_ADDR + 0x3450));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3454), (unsigned int)ISP_RD32(ISP_ADDR + 0x3454));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3458), (unsigned int)ISP_RD32(ISP_ADDR + 0x3458));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x345c), (unsigned int)ISP_RD32(ISP_ADDR + 0x345c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3480), (unsigned int)ISP_RD32(ISP_ADDR + 0x3480));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3484), (unsigned int)ISP_RD32(ISP_ADDR + 0x3484));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3488), (unsigned int)ISP_RD32(ISP_ADDR + 0x3488));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x348c), (unsigned int)ISP_RD32(ISP_ADDR + 0x348c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3490), (unsigned int)ISP_RD32(ISP_ADDR + 0x3490));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3494), (unsigned int)ISP_RD32(ISP_ADDR + 0x3494));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3498), (unsigned int)ISP_RD32(ISP_ADDR + 0x3498));
            break;
        case M4U_PORT_IMG3O:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0018), (unsigned int)ISP_RD32(ISP_ADDR + 0x0018));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x001c), (unsigned int)ISP_RD32(ISP_ADDR + 0x001c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3460), (unsigned int)ISP_RD32(ISP_ADDR + 0x3460));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3464), (unsigned int)ISP_RD32(ISP_ADDR + 0x3464));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3468), (unsigned int)ISP_RD32(ISP_ADDR + 0x3468));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x346c), (unsigned int)ISP_RD32(ISP_ADDR + 0x346c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3470), (unsigned int)ISP_RD32(ISP_ADDR + 0x3470));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3474), (unsigned int)ISP_RD32(ISP_ADDR + 0x3474));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3478), (unsigned int)ISP_RD32(ISP_ADDR + 0x3478));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x347c), (unsigned int)ISP_RD32(ISP_ADDR + 0x347c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3400), (unsigned int)ISP_RD32(ISP_ADDR + 0x3400));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3404), (unsigned int)ISP_RD32(ISP_ADDR + 0x3404));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3408), (unsigned int)ISP_RD32(ISP_ADDR + 0x3408));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x340c), (unsigned int)ISP_RD32(ISP_ADDR + 0x340c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3410), (unsigned int)ISP_RD32(ISP_ADDR + 0x3410));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3414), (unsigned int)ISP_RD32(ISP_ADDR + 0x3414));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3418), (unsigned int)ISP_RD32(ISP_ADDR + 0x3418));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x341c), (unsigned int)ISP_RD32(ISP_ADDR + 0x341c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3420), (unsigned int)ISP_RD32(ISP_ADDR + 0x3420));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3424), (unsigned int)ISP_RD32(ISP_ADDR + 0x3424));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3428), (unsigned int)ISP_RD32(ISP_ADDR + 0x3428));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x342c), (unsigned int)ISP_RD32(ISP_ADDR + 0x342c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3430), (unsigned int)ISP_RD32(ISP_ADDR + 0x3430));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3434), (unsigned int)ISP_RD32(ISP_ADDR + 0x3434));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3438), (unsigned int)ISP_RD32(ISP_ADDR + 0x3438));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x343c), (unsigned int)ISP_RD32(ISP_ADDR + 0x343c));
            break;
        case M4U_PORT_VIPI:
        case M4U_PORT_VIP2I:
        case M4U_PORT_VIP3I:
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x0018), (unsigned int)ISP_RD32(ISP_ADDR + 0x0018));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x001c), (unsigned int)ISP_RD32(ISP_ADDR + 0x001c));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x00a8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3204), (unsigned int)ISP_RD32(ISP_ADDR + 0x3204));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x3230), (unsigned int)ISP_RD32(ISP_ADDR + 0x3230));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x32c0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32c4), (unsigned int)ISP_RD32(ISP_ADDR + 0x32c4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32c8), (unsigned int)ISP_RD32(ISP_ADDR + 0x32c8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32cc), (unsigned int)ISP_RD32(ISP_ADDR + 0x32cc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32d0), (unsigned int)ISP_RD32(ISP_ADDR + 0x32d0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32d4), (unsigned int)ISP_RD32(ISP_ADDR + 0x32d4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32d8), (unsigned int)ISP_RD32(ISP_ADDR + 0x32d8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32dc), (unsigned int)ISP_RD32(ISP_ADDR + 0x32dc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32e0), (unsigned int)ISP_RD32(ISP_ADDR + 0x32e0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32e4), (unsigned int)ISP_RD32(ISP_ADDR + 0x32e4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32e8), (unsigned int)ISP_RD32(ISP_ADDR + 0x32e8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32ec), (unsigned int)ISP_RD32(ISP_ADDR + 0x32ec));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32f0), (unsigned int)ISP_RD32(ISP_ADDR + 0x32f0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32f4), (unsigned int)ISP_RD32(ISP_ADDR + 0x32f4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32f8), (unsigned int)ISP_RD32(ISP_ADDR + 0x32f8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x32fc), (unsigned int)ISP_RD32(ISP_ADDR + 0x32fc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x33a4), (unsigned int)ISP_RD32(ISP_ADDR + 0x33a4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x33a8), (unsigned int)ISP_RD32(ISP_ADDR + 0x33a8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x33ac), (unsigned int)ISP_RD32(ISP_ADDR + 0x33ac));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x33b0), (unsigned int)ISP_RD32(ISP_ADDR + 0x33b0));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x33b4), (unsigned int)ISP_RD32(ISP_ADDR + 0x33b4));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x33b8), (unsigned int)ISP_RD32(ISP_ADDR + 0x33b8));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x33bc), (unsigned int)ISP_RD32(ISP_ADDR + 0x33bc));
            LOG_DBG("[TF_%d]0x%08X %08X",port, (unsigned int)(ISP_TPIPE_ADDR + 0x33c0), (unsigned int)ISP_RD32(ISP_ADDR + 0x33c0));
            break;
        default:
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x0000), (unsigned int)ISP_RD32(ISP_ADDR + 0x0000));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x0004), (unsigned int)ISP_RD32(ISP_ADDR + 0x0004));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x0008), (unsigned int)ISP_RD32(ISP_ADDR + 0x0008));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x0010), (unsigned int)ISP_RD32(ISP_ADDR + 0x0010));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x0014), (unsigned int)ISP_RD32(ISP_ADDR + 0x0014));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x0018), (unsigned int)ISP_RD32(ISP_ADDR + 0x0018));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x001c), (unsigned int)ISP_RD32(ISP_ADDR + 0x001c));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x0020), (unsigned int)ISP_RD32(ISP_ADDR + 0x0020));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x00a0), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a0));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x00a4), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a4));
            LOG_DBG("0x%08X %08X", (unsigned int)(ISP_TPIPE_ADDR + 0x00a8), (unsigned int)ISP_RD32(ISP_ADDR + 0x00a8));
            break;
    }

    return M4U_CALLBACK_HANDLED;
}



/*                                                                              
 
                                                                               */
module_init(ISP_Init);
module_exit(ISP_Exit);
MODULE_DESCRIPTION("Camera ISP driver");
MODULE_AUTHOR("ME3");
MODULE_LICENSE("GPL");
EXPORT_SYMBOL(ISP_RegCallback);
EXPORT_SYMBOL(ISP_UnregCallback);
EXPORT_SYMBOL(ISP_MCLK1_EN);
EXPORT_SYMBOL(ISP_MCLK2_EN);
EXPORT_SYMBOL(ISP_MCLK3_EN);




