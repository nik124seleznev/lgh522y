/**
 * Copyright (C) 2009 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmbslTDA9989_InOut.c
 *
 * \version       %version: 3 %
 *
 *
*/

/*                                                                            */
/*                                                                            */
/*                                                                            */
#ifndef TMFL_TDA19989
#define TMFL_TDA19989
#endif

#ifndef TMFL_NO_RTOS
#define TMFL_NO_RTOS
#endif

#ifndef TMFL_LINUX_OS_KERNEL_DRIVER
#define TMFL_LINUX_OS_KERNEL_DRIVER
#endif



#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
#include <linux/kernel.h>
#include <linux/module.h>
#endif

#include "tmbslHdmiTx_types.h"
#include "tmbslTDA9989_Functions.h"
#include "tmbslTDA9989_local.h"
#include "tmbslTDA9989_HDCP_l.h"
#include "tmbslTDA9989_State_l.h"
#include "tmbslTDA9989_Misc_l.h"
#include "tmbslTDA9989_InOut_l.h"

/*                                                                            */
/*                                                                            */
/*                                                                            */

#define SSD_UNUSED_VALUE 0xF0

#ifdef FORMAT_PC
#define DEPTH_COLOR_PC 1	/*                                 */
#endif				/*           */

#define REG_VAL_SEL_AIP_SPDIF   0
#define REG_VAL_SEL_AIP_I2S     1
#define REG_VAL_SEL_AIP_OBA     2
#define REG_VAL_SEL_AIP_DST     3
#define REG_VAL_SEL_AIP_HBR     5

struct vic2reg {
	unsigned char vic;
	unsigned char reg;
};

struct sync_desc {
	UInt16 Vs2;
	UInt8 pix_rep;
	UInt8 v_toggle;
	UInt8 h_toggle;
	UInt16 hfp;		/*                                    */
	UInt16 vfp;
	UInt16 href;		/*                                                */
	UInt16 vref;
};

/*                                                                            */
/*                                                                            */
/*                                                                            */

extern tmHdmiTxRegMaskVal_t kVoutHdcpOff[];
extern tmHdmiTxRegMaskVal_t kVoutHdcpOn[];

/* 
                                                                               
 */
tmbslTDA9989RegVip kRegVip[HDMITX_VIN_PORT_MAP_TABLE_LEN] = {
	{
	E_REG_P00_VIP_CNTRL_0_W, E_MASKREG_P00_VIP_CNTRL_0_swap_a, E_MASKREG_P00_VIP_CNTRL_0_mirr_a},	/*              */
	{
	E_REG_P00_VIP_CNTRL_0_W, E_MASKREG_P00_VIP_CNTRL_0_swap_b, E_MASKREG_P00_VIP_CNTRL_0_mirr_b},	/*              */
	{
	E_REG_P00_VIP_CNTRL_1_W, E_MASKREG_P00_VIP_CNTRL_1_swap_c, E_MASKREG_P00_VIP_CNTRL_1_mirr_c},	/*              */
	{
	E_REG_P00_VIP_CNTRL_1_W, E_MASKREG_P00_VIP_CNTRL_1_swap_d, E_MASKREG_P00_VIP_CNTRL_1_mirr_d},	/*              */
	{
	E_REG_P00_VIP_CNTRL_2_W, E_MASKREG_P00_VIP_CNTRL_2_swap_e, E_MASKREG_P00_VIP_CNTRL_2_mirr_e},	/*              */
	{
	E_REG_P00_VIP_CNTRL_2_W, E_MASKREG_P00_VIP_CNTRL_2_swap_f, E_MASKREG_P00_VIP_CNTRL_2_mirr_f}	/*              */
};

/* 
                                                                                   
 */
tmHdmiTxRegMaskVal_t kCommonPllCfg[] = {
	{E_REG_P02_PLL_SERIAL_1_RW, E_MASKREG_ALL, 0x00},
	{E_REG_P02_PLL_SERIAL_2_RW, E_MASKREG_ALL, 0x01},
	{E_REG_P02_PLL_SERIAL_3_RW, E_MASKREG_ALL, 0x00},
	{E_REG_P02_SERIALIZER_RW, E_MASKREG_ALL, 0x00},
	{E_REG_P02_BUFFER_OUT_RW, E_MASKREG_ALL, 0x00},
	{E_REG_P02_PLL_SCG1_RW, E_MASKREG_ALL, 0x00},
	{E_REG_P02_AUDIO_DIV_RW, E_MASKREG_ALL, 0x03},
	/*                                                   */
	{E_REG_P02_SEL_CLK_RW, E_MASKREG_ALL, 0x09},
	{0, 0, 0}
};

/* 
                                                                       
                                 
 */
tmHdmiTxRegMaskVal_t kDoublePrateVfmtOtherPllCfg[] = {
	{E_REG_P02_PLL_SCG2_RW, E_MASKREG_ALL, 0x00},
	{0, 0, 0}
};

/* 
                                                                           
                           
 */
tmHdmiTxRegMaskVal_t kSinglePrateVfmt480i576iPllCfg[] = {
	{E_REG_P02_PLL_SCG2_RW, E_MASKREG_ALL, 0x11},
	{0, 0, 0}
};

/* 
                                                                       
                                 
 */
tmHdmiTxRegMaskVal_t kSinglePrateVfmtOtherPllCfg[] = {
	{E_REG_P02_PLL_SCG2_RW, E_MASKREG_ALL, 0x10},
	{0, 0, 0}
};

/* 
                                                                                    
                           
 */
tmHdmiTxRegMaskVal_t kSrepeatedPrateVfmt480i576iPllCfg[] = {
	{E_REG_P02_PLL_SCG2_RW, E_MASKREG_ALL, 0x01},
	{0, 0, 0}
};

/* 
                                                                        
 */
tmHdmiTxRegMaskVal_t kVfmt480i576iPllCfg[] = {
	{E_REG_P02_PLL_SCGN1_RW, E_MASKREG_ALL, 0x14},
	{E_REG_P02_PLL_SCGN2_RW, E_MASKREG_ALL, 0x00},
	{E_REG_P02_PLL_SCGR1_RW, E_MASKREG_ALL, 0x0A},
	{E_REG_P02_PLL_SCGR2_RW, E_MASKREG_ALL, 0x00},
	{0, 0, 0}
};

/* 
                                                                                   
 */
tmHdmiTxRegMaskVal_t kVfmtOtherPllCfg[] = {
	{E_REG_P02_PLL_SCGN1_RW, E_MASKREG_ALL, 0xFA},
	{E_REG_P02_PLL_SCGN2_RW, E_MASKREG_ALL, 0x00},
	{E_REG_P02_PLL_SCGR1_RW, E_MASKREG_ALL, 0x5B},
	{E_REG_P02_PLL_SCGR2_RW, E_MASKREG_ALL, 0x00},
	{0, 0, 0}
};

/* 
                                                                             
                                                                       
                                                                       
                                                    
 */

UInt8 kVfmtToPixClk_TV[HDMITX_VFMT_TV_MAX][HDMITX_VFREQ_NUM] = {
	/*                                                                                                              */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_25175, E_PIXCLK_25200},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_27027},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_27027},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_74175, E_PIXCLK_74250},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_74175, E_PIXCLK_74250},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_27027},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_27027},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_27027},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_27027},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_54054},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_54054},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_54054},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_54054},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_54054},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_54054},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_148350, E_PIXCLK_148500},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_74250, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_74250, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_27000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_54000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_148500, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_74250, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_74250, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_74250, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */

	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_108000, E_PIXCLK_108108},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_108000, E_PIXCLK_108108},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_108000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_108000, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                                */

	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                  */

	{E_PIXCLK_59400, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                               */
	{E_PIXCLK_INVALID, E_PIXCLK_74250, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID},	/*                               */
	{E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_74250, E_PIXCLK_INVALID, E_PIXCLK_INVALID, E_PIXCLK_INVALID}	/*                               */
};

/*                                                                            */
/*                                                                            */
/*                                                                            */


/* 
                                                                           
               
                                                    
 */
#ifdef FORMAT_PC
UInt8 kVfmtToPixClk_PC[HDMITX_VFMT_PC_NUM] = {
	/*                                                                                                             */
	E_PIXCLK_25175,		/*                                */
	E_PIXCLK_40000,		/*                                */
	E_PIXCLK_INVALID,	/*                                */
	E_PIXCLK_65000,		/*                               */
	E_PIXCLK_79500,		/*                               */
	E_PIXCLK_108000,	/*                                */
	E_PIXCLK_85500,		/*                                */
	E_PIXCLK_121750,	/*                                */
	E_PIXCLK_162000,	/*                                */
	E_PIXCLK_75000,		/*                                */
	E_PIXCLK_31500,		/*                                */
	E_PIXCLK_50000,		/*                                */
	E_PIXCLK_31500,		/*                                */
	E_PIXCLK_78750,		/*                                */
	E_PIXCLK_49500,		/*                                */
	E_PIXCLK_INVALID,	/*                                */
	E_PIXCLK_INVALID,	/*                                */
	E_PIXCLK_INVALID,	/*                                */
	E_PIXCLK_INVALID,	/*                                */
	E_PIXCLK_INVALID,	/*                                */
	E_PIXCLK_36000,		/*                                */
	E_PIXCLK_56250,		/*                                */
	E_PIXCLK_INVALID,	/*                                */
	E_PIXCLK_INVALID,	/*                                */
	E_PIXCLK_INVALID,	/*                                */
	E_PIXCLK_INVALID,	/*                                *//*              */
	E_PIXCLK_INVALID	/*                                */
};
#endif

/* 
                                                                                
                                                                          
 */

#ifdef TMFL_RGB_DDR_12BITS
static struct vic2reg vic2reg_TV[] = {
	{HDMITX_VFMT_01_640x480p_60Hz, E_REGVFMT_640x480p_60Hz},
	{HDMITX_VFMT_02_720x480p_60Hz, E_REGVFMT_720x480p_60Hz},
	{HDMITX_VFMT_03_720x480p_60Hz, E_REGVFMT_720x480p_60Hz},
	{HDMITX_VFMT_04_1280x720p_60Hz, E_REGVFMT_1280x720p_60Hz},
	{HDMITX_VFMT_05_1920x1080i_60Hz, E_REGVFMT_1920x1080i_60Hz},
	{HDMITX_VFMT_06_720x480i_60Hz, E_REGVFMT_720x480i_60Hz},
	{HDMITX_VFMT_07_720x480i_60Hz, E_REGVFMT_720x480i_60Hz},
	{HDMITX_VFMT_08_720x240p_60Hz, E_REGVFMT_720x240p_60Hz},
	{HDMITX_VFMT_09_720x240p_60Hz, E_REGVFMT_720x240p_60Hz},
	{HDMITX_VFMT_10_720x480i_60Hz, E_REGVFMT_2880x480i_60Hz_PR4},
	{HDMITX_VFMT_11_720x480i_60Hz, E_REGVFMT_2880x480i_60Hz_PR4},
	{HDMITX_VFMT_14_1440x480p_60Hz, E_REGVFMT_1440x480p_60Hz},
	{HDMITX_VFMT_15_1440x480p_60Hz, E_REGVFMT_1440x480p_60Hz},
	{HDMITX_VFMT_16_1920x1080p_60Hz, E_REGVFMT_1920x1080p_60Hz},
	{HDMITX_VFMT_17_720x576p_50Hz, E_REGVFMT_720x576p_50Hz},
	{HDMITX_VFMT_18_720x576p_50Hz, E_REGVFMT_720x576p_50Hz},
	{HDMITX_VFMT_19_1280x720p_50Hz, E_REGVFMT_1280x720p_50Hz},
	{HDMITX_VFMT_20_1920x1080i_50Hz, E_REGVFMT_1920x1080i_50Hz},
	{HDMITX_VFMT_21_720x576i_50Hz, E_REGVFMT_720x576i_50Hz},
	{HDMITX_VFMT_22_720x576i_50Hz, E_REGVFMT_720x576i_50Hz},
	{HDMITX_VFMT_23_720x288p_50Hz, E_REGVFMT_720x288p_50Hz},
	{HDMITX_VFMT_24_720x288p_50Hz, E_REGVFMT_720x288p_50Hz},
	{HDMITX_VFMT_25_720x576i_50Hz, E_REGVFMT_2880x576i_50Hz},	/*            */
	{HDMITX_VFMT_26_720x576i_50Hz, E_REGVFMT_2880x576i_50Hz},	/*            */
	{HDMITX_VFMT_29_1440x576p_50Hz, E_REGVFMT_1440x576p_50Hz},
	{HDMITX_VFMT_30_1440x576p_50Hz, E_REGVFMT_1440x576p_50Hz},
	{HDMITX_VFMT_31_1920x1080p_50Hz, E_REGVFMT_1920x1080p_50Hz},
	{HDMITX_VFMT_32_1920x1080p_24Hz, E_REGVFMT_1920x1080p_24Hz},
	{HDMITX_VFMT_33_1920x1080p_25Hz, E_REGVFMT_1920x1080p_25Hz},
	{HDMITX_VFMT_34_1920x1080p_30Hz, E_REGVFMT_1920x1080p_30Hz},
	{HDMITX_VFMT_35_2880x480p_60Hz, E_REGVFMT_2880x480p_60Hz},
	{HDMITX_VFMT_36_2880x480p_60Hz, E_REGVFMT_2880x480p_60Hz},
	{HDMITX_VFMT_37_2880x576p_50Hz, E_REGVFMT_720x576p_50Hz},
	{HDMITX_VFMT_38_2880x576p_50Hz, E_REGVFMT_720x576p_50Hz},
	{HDMITX_VFMT_60_1280x720p_24Hz, E_REGVFMT_1280x720p_24Hz},
	{HDMITX_VFMT_61_1280x720p_25Hz, E_REGVFMT_1280x720p_25Hz},
	{HDMITX_VFMT_62_1280x720p_30Hz, E_REGVFMT_1280x720p_30Hz}
};

static struct vic2reg vic2reg_TV_FP[] = {
	{HDMITX_VFMT_01_640x480p_60Hz, E_REGVFMT_720x480p_60Hz_FP},
	{HDMITX_VFMT_02_720x480p_60Hz, E_REGVFMT_720x480p_60Hz_FP},
	{HDMITX_VFMT_03_720x480p_60Hz, E_REGVFMT_720x480p_60Hz_FP},
	{HDMITX_VFMT_04_1280x720p_60Hz, E_REGVFMT_1280x720p_60Hz_FP},
	{HDMITX_VFMT_05_1920x1080i_60Hz, E_REGVFMT_1920x1080i_60Hz_FP},
	{HDMITX_VFMT_17_720x576p_50Hz, E_REGVFMT_720x576p_50Hz_FP},
	{HDMITX_VFMT_18_720x576p_50Hz, E_REGVFMT_720x576p_50Hz_FP},
	{HDMITX_VFMT_19_1280x720p_50Hz, E_REGVFMT_1280x720p_50Hz_FP},
	{HDMITX_VFMT_20_1920x1080i_50Hz, E_REGVFMT_1920x1080i_50Hz_FP},
	{HDMITX_VFMT_32_1920x1080p_24Hz, E_REGVFMT_1920x1080p_24Hz_FP},
	{HDMITX_VFMT_33_1920x1080p_25Hz, E_REGVFMT_1920x1080p_25Hz_FP},
	{HDMITX_VFMT_34_1920x1080p_30Hz, E_REGVFMT_1920x1080p_30Hz_FP},
	{HDMITX_VFMT_60_1280x720p_24Hz, E_REGVFMT_1280x720p_24Hz_FP},
	{HDMITX_VFMT_61_1280x720p_25Hz, E_REGVFMT_1280x720p_25Hz_FP},
	{HDMITX_VFMT_62_1280x720p_30Hz, E_REGVFMT_1280x720p_30Hz_FP}
};
#else
static struct vic2reg vic2reg_TV[] = {
	{HDMITX_VFMT_01_640x480p_60Hz, E_REGVFMT_640x480p_60Hz},
	{HDMITX_VFMT_02_720x480p_60Hz, E_REGVFMT_720x480p_60Hz},
	{HDMITX_VFMT_03_720x480p_60Hz, E_REGVFMT_720x480p_60Hz},
	{HDMITX_VFMT_04_1280x720p_60Hz, E_REGVFMT_1280x720p_60Hz},
	{HDMITX_VFMT_05_1920x1080i_60Hz, E_REGVFMT_1920x1080i_60Hz},
	{HDMITX_VFMT_06_720x480i_60Hz, E_REGVFMT_720x480i_60Hz},
	{HDMITX_VFMT_07_720x480i_60Hz, E_REGVFMT_720x480i_60Hz},
	{HDMITX_VFMT_08_720x240p_60Hz, E_REGVFMT_720x240p_60Hz},
	{HDMITX_VFMT_09_720x240p_60Hz, E_REGVFMT_720x240p_60Hz},
	{HDMITX_VFMT_16_1920x1080p_60Hz, E_REGVFMT_1920x1080p_60Hz},
	{HDMITX_VFMT_17_720x576p_50Hz, E_REGVFMT_720x576p_50Hz},
	{HDMITX_VFMT_18_720x576p_50Hz, E_REGVFMT_720x576p_50Hz},
	{HDMITX_VFMT_19_1280x720p_50Hz, E_REGVFMT_1280x720p_50Hz},
	{HDMITX_VFMT_20_1920x1080i_50Hz, E_REGVFMT_1920x1080i_50Hz},
	{HDMITX_VFMT_21_720x576i_50Hz, E_REGVFMT_720x576i_50Hz},
	{HDMITX_VFMT_22_720x576i_50Hz, E_REGVFMT_720x576i_50Hz},
	{HDMITX_VFMT_23_720x288p_50Hz, E_REGVFMT_720x288p_50Hz},
	{HDMITX_VFMT_24_720x288p_50Hz, E_REGVFMT_720x288p_50Hz},
	{HDMITX_VFMT_31_1920x1080p_50Hz, E_REGVFMT_1920x1080p_50Hz},
	{HDMITX_VFMT_32_1920x1080p_24Hz, E_REGVFMT_1920x1080p_24Hz},
	{HDMITX_VFMT_33_1920x1080p_25Hz, E_REGVFMT_1920x1080p_25Hz},
	{HDMITX_VFMT_34_1920x1080p_30Hz, E_REGVFMT_1920x1080p_30Hz},
	{HDMITX_VFMT_35_2880x480p_60Hz, E_REGVFMT_720x480p_60Hz},
	{HDMITX_VFMT_36_2880x480p_60Hz, E_REGVFMT_720x480p_60Hz},
	{HDMITX_VFMT_37_2880x576p_50Hz, E_REGVFMT_720x576p_50Hz},
	{HDMITX_VFMT_38_2880x576p_50Hz, E_REGVFMT_720x576p_50Hz},
	{HDMITX_VFMT_60_1280x720p_24Hz, E_REGVFMT_1280x720p_24Hz},
	{HDMITX_VFMT_61_1280x720p_25Hz, E_REGVFMT_1280x720p_25Hz},
	{HDMITX_VFMT_62_1280x720p_30Hz, E_REGVFMT_1280x720p_30Hz}
};

static struct vic2reg vic2reg_TV_FP[] = {
	{HDMITX_VFMT_04_1280x720p_60Hz, E_REGVFMT_1280x720p_60Hz_FP},
	{HDMITX_VFMT_05_1920x1080i_60Hz, E_REGVFMT_1920x1080i_60Hz_FP},
	{HDMITX_VFMT_19_1280x720p_50Hz, E_REGVFMT_1280x720p_50Hz_FP},
	{HDMITX_VFMT_20_1920x1080i_50Hz, E_REGVFMT_1920x1080i_50Hz_FP},
	{HDMITX_VFMT_32_1920x1080p_24Hz, E_REGVFMT_1920x1080p_24Hz_FP},
	{HDMITX_VFMT_33_1920x1080p_25Hz, E_REGVFMT_1920x1080p_25Hz_FP},
	{HDMITX_VFMT_34_1920x1080p_30Hz, E_REGVFMT_1920x1080p_30Hz_FP},
	{HDMITX_VFMT_60_1280x720p_24Hz, E_REGVFMT_1280x720p_24Hz_FP},
	{HDMITX_VFMT_61_1280x720p_25Hz, E_REGVFMT_1280x720p_25Hz_FP},
	{HDMITX_VFMT_62_1280x720p_30Hz, E_REGVFMT_1280x720p_30Hz_FP}
};
#endif

#ifdef FORMAT_PC
static struct vic2reg vic2reg_PC[HDMITX_VFMT_PC_NUM] = {
	{HDMITX_VFMT_PC_640x480p_60Hz, E_REGVFMT_640x480p_60Hz},
	{HDMITX_VFMT_PC_800x600p_60Hz, E_REGVFMT_800x600p_60Hz},
	{HDMITX_VFMT_PC_1024x768p_60Hz, E_REGVFMT_1024x768p_60Hz},
	{HDMITX_VFMT_PC_1280x768p_60Hz, E_REGVFMT_1280x768p_60Hz},
	{HDMITX_VFMT_PC_1280x1024p_60Hz, E_REGVFMT_1280x1024p_60Hz},
	{HDMITX_VFMT_PC_1360x768p_60Hz, E_REGVFMT_1360x768p_60Hz},
	{HDMITX_VFMT_PC_1400x1050p_60Hz, E_REGVFMT_1400x1050p_60Hz},
	{HDMITX_VFMT_PC_1600x1200p_60Hz, E_REGVFMT_1600x1200p_60Hz},
	{HDMITX_VFMT_PC_1024x768p_70Hz, E_REGVFMT_1024x768p_70Hz},
	{HDMITX_VFMT_PC_640x480p_72Hz, E_REGVFMT_640x480p_72Hz},
	{HDMITX_VFMT_PC_800x600p_72Hz, E_REGVFMT_800x600p_72Hz},
	{HDMITX_VFMT_PC_640x480p_75Hz, E_REGVFMT_640x480p_75Hz},
	{HDMITX_VFMT_PC_1024x768p_75Hz, E_REGVFMT_1024x768p_75Hz},
	{HDMITX_VFMT_PC_800x600p_75Hz, E_REGVFMT_800x600p_75Hz},
	{HDMITX_VFMT_PC_640x480p_85Hz, E_REGVFMT_640x480p_85Hz},
	{HDMITX_VFMT_PC_800x600p_85Hz, E_REGVFMT_800x600p_85Hz},
	{HDMITX_VFMT_PC_1280x1024p_85Hz, E_REGVFMT_1280x1024p_85Hz}
};
#endif				/*           */


/* 
                                                              
                                                                
                                                               
                                              
  
                                                               
                                                  
 */

/* 
                                                              
                                                  
                                                        
                                    
                                          
 */

/* 
                                                              
                                                  
                                                              
                        
  
                                    
                                
                                                                  
                                                              
                                                       
                                                       
                                                       
                                                       
  
 */

static UInt8 pll[] = {
	/*          */
	2,			/*                           */
	2,			/*                           */
	1,			/*                           */
	1,			/*                           */
	3,			/*                           */
	0,			       /*                           *//*                            */
	0,			/*                           */
	2,			/*                           */
	1,			/*                           */
	1,			/*                           */
	3,			/*                           */
	0,			       /*                           *//*                            */
	0,			/*                           */
#ifdef TMFL_RGB_DDR_12BITS
	0,			/*                             */
	1,			/*                             */
	1,			/*                             */
	0,			/*                             */
	0,			/*                             */
	1,			/*                             */
	2,			/*                              */
	2,			/*                              */
	1,			/*                             */
	2,			/*                              */
	1,			/*                             */
	0,			/*                             */
	1,			/*                             */
	0,			/*                             */
	0,			/*                              */
	0,			/*                              */
	0,			/*                              */
	0,			/*                              */
	0,			/*                              */
#endif
	/*            */
#ifndef TMFL_RGB_DDR_12BITS
	1,			/*                           */
#endif
	1,			/*                           */
	1,			/*                           */
	1,			/*                           */
	1,			/*                           */
	1,			/*                           */
#ifndef TMFL_RGB_DDR_12BITS
	0,			/*                             */
	0,			/*                              */
	0,			/*                             */
	0,			/*                              */
	0,			/*                              */
	0,			/*                              */
	0,			/*                              */
#endif
	0,			/*                             */
	0,			/*                             */
	0,			/*                             */
#ifdef FORMAT_PC
	2,			/*                           */
	2,			/*                           */
	2,			/*                           */
	1,			/*                           */
	1,			/*                           */
	1,			/*                           */
	1,			/*                           */
	1,			/*                           */
	1,			/*                           */
	1,			/*                           */
	0,			/*                           */
	0,			/*                           */
	0,			/*                           */
	0,			/*                           */
	0,			/*                           */
	1			/*                           */
#endif				/*           */
};



/* 
                                                             
                                                              
                           
 */
/*               */
static struct sync_desc ref_sync[] = {
	/*
                                            
                                            
                  
                    
                   
                    

                                                                       
                                                                          
                 
                                                                             

  */
	/*                                      */
	{0, 0, 1, 1, 17, 2, 161, 36},	/*                           */
	{0, 0, 1, 1, 17, 8, 139, 43},	/*                           */
	{0, 0, 0, 0, 111, 2, 371, 26},	/*                           */
	{1100 + 88, 0, 0, 0, 89, 2, 281, 21},	/*                           */
	{429 + 19, 1, 1, 1, 20, 5, 139, 22},	/*                           */
	{0, 1, 1, 1, 20, 5, 139, 22},	/*                           */
	{0, 0, 0, 0, 89, 2, 281, 42},	/*                           */
	{0, 0, 1, 1, 13, 2, 145, 45},	/*                           */
	{0, 0, 0, 0, 441, 2, 701, 26},	/*                           */
	{1320 + 528, 0, 0, 0, 529, 2, 721, 21},	/*                           */
	{432 + 12, 1, 1, 1, 13, 2, 145, 23},	/*                           */
	{0, 1, 1, 1, 13, 2, 145, 23},	/*                           */
	{0, 0, 0, 0, 529, 2, 721, 42},	/*                           */
#ifdef TMFL_RGB_DDR_12BITS
	{0, 0, 0, 0, 639, 2, 831, 42},	/*                            */
	{0, 0, 1, 1, 25, 2, 289, 45},	/*                            */
	{0, 0, 1, 1, 33, 8, 277, 43},	/*                            */
	{0, 0, 1, 1, 65, 8, 553, 43},	/*                            */
	{0, 0, 1, 1, 49, 2, 577, 45},	/*                            */
	{1716 + 76, 0, 1, 1, 77, 5, 553, 22},	/*                            */
	{858 + 38, 1, 1, 1, 39, 5, 277, 22},	/*                              */
	{429 + 19, 2, 1, 1, 20, 5, 139, 22},	/*                              */
	{1728 + 48, 0, 1, 1, 49, 2, 577, 23},	/*                            */
	{864 + 24, 1, 1, 1, 25, 2, 289, 23}	/*                             */
#endif
};

/*            */
static struct sync_desc ref_sync_extra[] = {
	/*                                      */
#ifndef TMFL_RGB_DDR_12BITS
	{0, 0, 0, 0, 639, 2, 831, 42},	/*                           */
#endif
	{0, 0, 0, 0, 529, 2, 721, 42},	/*                           */
	{0, 0, 0, 0, 89, 2, 281, 42},	/*                           */
	{0, 0, 0, 0, 1761, 2, 2021, 26},	/*                           */
	{0, 0, 0, 0, 2421, 2, 2681, 26},	/*                           */
	{0, 0, 0, 0, 1761, 2, 2021, 26}	/*                           */
};

#ifdef FORMAT_PC
   /*         */
static struct sync_desc ref_sync_PC[] = {
	/*                                      */
	{0, 0, 1, 1, 25, 2, 195, 32},	/*                           */
	{0, 0, 1, 1, 17, 2, 203, 20},	/*                           */
	{0, 0, 1, 1, 57, 2, 195, 29},	/*                           */
	{0, 0, 0, 0, 41, 2, 259, 28},	/*                           */
	{0, 0, 0, 0, 57, 2, 243, 30},	/*                           */
	{0, 0, 0, 0, 17, 2, 259, 25},	/*                           */
	{0, 0, 0, 0, 33, 2, 251, 31},	/*                           */
	{0, 0, 1, 1, 25, 2, 323, 36},	/*                           */
	{0, 0, 1, 1, 25, 2, 307, 36},	/*                           */
	{0, 0, 0, 0, 17, 2, 291, 32},	/*                           */
	{0, 0, 0, 1, 65, 2, 387, 28},	/*                           */
	{0, 0, 0, 0, 49, 2, 411, 42},	/*                           */
	{0, 0, 0, 0, 65, 2, 435, 25},	/*                           */
	{0, 0, 0, 1, 89, 2, 467, 37},	/*                           */
	{0, 0, 0, 0, 65, 2, 563, 50},	/*                           */
	{0, 0, 0, 0, 65, 2, 451, 48}	/*                           */
};
#endif				/*           */

static tmHdmiTxVidReg_t format_param_extra[] = {
	/*                                                                                                                                      */
	/*                                                                                                                                     */
#ifndef TMFL_RGB_DDR_12BITS
	{2750, 1125, 1, 638, 6, 638, 638, 682, 41, 1121, 830, 2750, 0, 0},	/*                           */
#endif
	{2640, 1125, 1, 528, 6, 528, 528, 572, 41, 1121, 720, 2640, 0, 0},	/*                           */
	{2200, 1125, 1, 88, 6, 88, 88, 132, 41, 1121, 280, 2200, 0, 0},	/*                           */
	{3300, 750, 1, 1760, 6, 1760, 1760, 1800, 25, 745, 2020, 3300, 0, 0},	/*                           */
	{3960, 750, 1, 2420, 6, 2420, 2420, 2460, 25, 745, 2680, 3960, 0, 0},	/*                           */
	{3300, 750, 1, 1760, 6, 1760, 1760, 1800, 25, 745, 2020, 3300, 0, 0},	/*                           */
#ifndef TMFL_RGB_DDR_12BITS
	{1650, 1500, 1, 110, 6, 110, 110, 150, 25, 1495, 370, 1650, 746, 776},	/*                              */
	{2200, 2250, 1, 88, 6, 88, 88, 132, 20, 2248, 280, 2200, 0, 0},	/*                              */
	{1980, 1500, 1, 440, 6, 440, 440, 480, 25, 1495, 700, 1980, 746, 776},	/*                              */
	{2640, 2250, 1, 528, 6, 528, 528, 572, 20, 2248, 720, 2640, 0, 0},	/*                              */
	{2750, 2250, 1, 638, 6, 638, 638, 682, 41, 2246, 830, 2750, 1122, 1167},	/*                              */
	{2640, 2250, 1, 528, 6, 528, 528, 572, 41, 2246, 720, 2640, 1122, 1167},	/*                              */
	{2200, 2250, 1, 88, 6, 88, 88, 132, 41, 2246, 280, 2200, 1122, 1167},	/*                              */
#endif
	{3300, 1500, 1, 1760, 6, 1760, 1760, 1800, 25, 1495, 2020, 3300, 0, 0},	/*                              */
	{3960, 1500, 1, 2420, 6, 2420, 2420, 2460, 25, 1495, 2680, 3960, 0, 0},	/*                              */
	{3300, 1500, 1, 1760, 6, 1760, 1760, 1800, 25, 1495, 2020, 3300, 0, 0},	/*                              */
};

#ifdef FORMAT_PC
static tmHdmiTxVidReg_t format_param_PC[HDMITX_VFMT_PC_NUM] = {
	/*                                                                                                                                      */
	/*                                                                                                                                     */
	{832, 520, 1, 24, 4, 24, 24, 64, 31, 511, 192, 832, 0, 0},	/*                           */
	{840, 500, 1, 16, 4, 16, 16, 80, 19, 499, 200, 840, 0, 0},	/*                           */
	{832, 509, 1, 56, 4, 56, 56, 112, 28, 508, 192, 832, 0, 0},	/*                           */
	{1056, 628, 1, 40, 5, 40, 40, 168, 27, 627, 256, 1056, 0, 0},	/*                           */
	{1040, 666, 1, 56, 7, 56, 56, 176, 29, 619, 240, 1040, 0, 0},	/*                           */
	{1056, 625, 1, 16, 4, 16, 16, 96, 24, 624, 256, 1056, 0, 0},	/*                           */
	{1048, 631, 1, 32, 4, 32, 32, 96, 30, 630, 248, 1048, 0, 0},	/*                           */
	{1344, 806, 1, 24, 7, 24, 24, 160, 35, 803, 320, 1344, 0, 0},	/*                           */
	{1328, 806, 1, 24, 7, 24, 24, 160, 35, 803, 304, 1328, 0, 0},	/*                           */
	{1312, 800, 1, 16, 4, 16, 16, 112, 31, 799, 288, 1312, 0, 0},	/*                           */
	{1664, 798, 1, 64, 8, 64, 64, 192, 27, 795, 384, 1664, 0, 0},	/*                           */
	{1688, 1066, 1, 48, 4, 48, 48, 160, 41, 1065, 408, 1688, 0, 0},	/*                           */
	{1792, 795, 1, 64, 7, 64, 64, 176, 24, 792, 432, 1792, 0, 0},	/*                           */
	{1864, 1089, 1, 88, 5, 88, 88, 232, 36, 1086, 464, 1864, 0, 0},	/*                           */
	{2160, 1250, 1, 64, 4, 64, 64, 256, 49, 1249, 560, 2160, 0, 0},	/*                           */
	{1728, 1072, 1, 64, 4, 64, 64, 224, 47, 1071, 448, 1728, 0, 0}	/*                           */
};
#endif				/*           */

 /* 
                                                                  
                                                            
 */
static UInt32 kPixClkToAcrCts[E_PIXCLK_NUM][HDMITX_AFS_NUM] = {
	/*                                                          */
	/*                                                           */
	{28125, 31250, 28125, 31250, 28125, 31250, 28125},	/*                */
	{25200, 28000, 25200, 28000, 25200, 28000, 25200},	/*                */
	{27000, 30000, 27000, 30000, 27000, 30000, 27000},	/*                */
	{27027, 30030, 27027, 30030, 27027, 30030, 27027},	/*                */
	{54000, 60000, 54000, 60000, 54000, 60000, 54000},	/*                */
	{54054, 60060, 54054, 60060, 54054, 60060, 54054},	/*                */
	{59400, 65996, 59400, 65996, 59400, 65996, 59400},	/*                */
	{210937, 234375, 140625, 234375, 140625, 234375, 140625},	/*                */
	{74250, 82500, 74250, 82500, 74250, 82500, 74250},	/*                */
	{421875, 234375, 140625, 234375, 140625, 234375, 140625},	/*                 */
	{148500, 165000, 148500, 165000, 148500, 165000, 148500}	/*                 */
#ifdef FORMAT_PC
	, {31500, 35000, 31500, 35000, 31500, 35000, 31500},	/*                */
	{36000, 40000, 36000, 40000, 36000, 40000, 36000},	/*                */
	{40000, 44444, 40000, 44444, 40000, 44444, 40000},	/*                */
	{49500, 55000, 49500, 55000, 49500, 55000, 49500},	/*                */
	{50000, 55556, 50000, 55556, 50000, 55556, 50000},	/*                */
	{56250, 62500, 56250, 62500, 56250, 62500, 56250},	/*                */
	{65000, 72222, 65000, 72222, 65000, 72222, 65000},	/*                */
	{75000, 83333, 75000, 83333, 75000, 83333, 75000},	/*                */
	{78750, 87500, 78750, 87500, 78750, 87500, 78750},	/*                */
	{162000, 180000, 162000, 180000, 162000, 180000, 162000},	/*                 */
	{157500, 175000, 157500, 175000, 157500, 175000, 157500}	/*                 */
#endif				/*           */
};

/* 
                                                                             
                                                          
 */
static UInt32 kPixClkToAcrN[E_PIXCLK_NUM][HDMITX_AFS_NUM] = {
	/*                                                          */
	/*                                                           */
	{4576, 7007, 6864, 14014, 13728, 28028, 27456},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{11648, 17836, 11648, 35672, 23296, 71344, 46592},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{11648, 8918, 5824, 17836, 11648, 35672, 23296},	/*                 */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576}	/*                 */
#ifdef FORMAT_PC
	, {4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576},	/*                 */
	{4096, 6272, 6144, 12544, 12288, 25088, 24576}	/*                 */
#endif				/*           */
};

/* 
                                                                             
                                                   
 */
static UInt8 kPixClkToAdiv[E_PIXCLK_NUM][HDMITX_AFS_NUM] = {
	/*                                                          */
	/*                                                           */
	{2, 2, 2, 1, 1, 0, 0},	/*                */
	{2, 2, 2, 1, 1, 0, 0},	/*                */
	{2, 2, 2, 1, 1, 0, 0},	/*                */
	{2, 2, 2, 1, 1, 0, 0},	/*                */
	{3, 3, 3, 2, 2, 1, 1},	/*                */
	{3, 3, 3, 2, 2, 1, 1},	/*                */
	{3, 3, 3, 2, 2, 1, 1},	/*                */
	{4, 3, 3, 2, 2, 1, 1},	/*                */
	{4, 3, 3, 2, 2, 1, 1},	/*                */
	{5, 4, 4, 3, 3, 2, 2},	/*                 */
	{5, 4, 4, 3, 3, 2, 2}	/*                 */
#ifdef FORMAT_PC
	, {2, 2, 2, 1, 1, 0, 0},	/*                 */
	{3, 2, 2, 1, 1, 0, 0},	/*                 */
	{3, 2, 2, 1, 1, 0, 0},	/*                 */
	{3, 3, 3, 2, 2, 1, 1},	/*                 */
	{3, 3, 3, 2, 2, 1, 1},	/*                 */
	{3, 3, 3, 2, 2, 1, 1},	/*                 */
	{4, 3, 3, 2, 2, 1, 1},	/*                 */
	{4, 3, 3, 2, 2, 1, 1},	/*                 */
	{4, 3, 3, 2, 2, 1, 1},	/*                 */
	{5, 4, 4, 3, 3, 2, 2},	/*                 */
	{5, 4, 4, 3, 3, 2, 2}	/*                 */
#endif				/*           */

};

/* 
                                                                    
                                                             
 */
static UInt8 kAfsToCSbyte3[HDMITX_AFS_NUM + 1] = {
	3,			/*                */
	0,			/*                  */
	2,			/*                */
	8,			/*                  */
	10,			/*                */
	12,			/*                   */
	14,			/*                 */
	9,			/*                 */
	1,			/*                          */
};



/* 
                                                                
 */
static UInt8 kCtsXToMK[HDMITX_CTSX_NUM][2] = {
/*                                    */
/*                           */
	{3, 0},			/*      */
	{3, 1},			/*      */
	{3, 2},			/*      */
	{3, 3},			/*      */
	{0, 0}			/*      */
};

/* 
                                                            
 */
static tmHdmiTxRegMaskVal_t kResetCtsGenerator[] = {
	{E_REG_P11_AIP_CNTRL_0_RW, E_MASKREG_P11_AIP_CNTRL_0_rst_cts, 1},
	{E_REG_P11_AIP_CNTRL_0_RW, E_MASKREG_P11_AIP_CNTRL_0_rst_cts, 0},
	{0, 0, 0}
};

/* 
                                                                            
 */
static tmHdmiTxRegMaskVal_t kBypassColourProc[] = {
	/*                                     */
	{E_REG_P00_HVF_CNTRL_0_W, E_MASKREG_P00_HVF_CNTRL_0_intpol, 0},
	/*                                  */
	{E_REG_P00_MAT_CONTRL_W, E_MASKREG_P00_MAT_CONTRL_mat_bp, 1},
	/*                                       */
	{E_REG_P00_HVF_CNTRL_1_W, E_MASKREG_P00_HVF_CNTRL_1_for, 0},
	{0, 0, 0}
};

/* 
                                                           */
static tmHdmiTxRegMaskVal_t kVinModeCCIR656[] = {
	{E_REG_P00_VIP_CNTRL_4_W, E_MASKREG_P00_VIP_CNTRL_4_ccir656, 1},
	{E_REG_P00_HVF_CNTRL_1_W, E_MASKREG_P00_HVF_CNTRL_1_semi_planar, 1},
	/*                                                                        */
	{E_REG_P02_PLL_SERIAL_3_RW, E_MASKREG_P02_PLL_SERIAL_3_srl_ccir, 1},
	{E_REG_P02_SEL_CLK_RW, E_MASKREG_P02_SEL_CLK_sel_vrf_clk, 1},
	{0, 0, 0}
};

 /*                                                                                                        */
static tmHdmiTxRegMaskVal_t kVinModeCCIR656_DDR_above720p[] = {
	{E_REG_P00_VIP_CNTRL_4_W, E_MASKREG_P00_VIP_CNTRL_4_ccir656, 1},
	{E_REG_P00_HVF_CNTRL_1_W, E_MASKREG_P00_HVF_CNTRL_1_semi_planar, 1},	/*              */
	/*                                                                                     */
	{E_REG_P02_PLL_SERIAL_3_RW, E_MASKREG_P02_PLL_SERIAL_3_srl_ccir, 0},
	{E_REG_P02_SEL_CLK_RW, E_MASKREG_P02_SEL_CLK_sel_vrf_clk, 0},
	{0, 0, 0}
};

/* 
                                                                    
 */
static tmHdmiTxRegMaskVal_t kVinMode444[] = {
	{E_REG_P00_VIP_CNTRL_4_W, E_MASKREG_P00_VIP_CNTRL_4_ccir656, 0},
	{E_REG_P00_HVF_CNTRL_1_W, E_MASKREG_P00_HVF_CNTRL_1_semi_planar, 0},
	/*                                                                         */
	{E_REG_P02_PLL_SERIAL_3_RW, E_MASKREG_P02_PLL_SERIAL_3_srl_ccir, 0},
	{E_REG_P02_SEL_CLK_RW, E_MASKREG_P02_SEL_CLK_sel_vrf_clk, 0},
	{0, 0, 0}
};

/* 
                                                          
 */
static tmHdmiTxRegMaskVal_t kVinModeYUV422[] = {
	{E_REG_P00_VIP_CNTRL_4_W, E_MASKREG_P00_VIP_CNTRL_4_ccir656, 0},
	{E_REG_P00_HVF_CNTRL_1_W, E_MASKREG_P00_HVF_CNTRL_1_semi_planar, 1},
	/*                                                                        */
	{E_REG_P02_PLL_SERIAL_3_RW, E_MASKREG_P02_PLL_SERIAL_3_srl_ccir, 0},
	{E_REG_P02_SEL_CLK_RW, E_MASKREG_P02_SEL_CLK_sel_vrf_clk, 0},
	{0, 0, 0}
};


/* 
                                                                  
                                                                      
                  
 */
static UInt8 kMatrixPreset[MATRIX_PRESET_QTY][MATRIX_PRESET_SIZE] = {
	{0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3, 0x6F, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x3, 0x6F, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3,
	 0x6F, 0x0, 0x40, 0x0, 0x40, 0x0, 0x40},	/*                         */

	{0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x4, 0x1, 0x7, 0x0,
	 0x64, 0x6, 0x88, 0x1, 0xC2, 0x7, 0xB7, 0x6, 0xD6, 0x7, 0x68, 0x1,
	 0xC2, 0x0, 0x40, 0x2, 0x0, 0x2, 0x0},	/*                   */

	{0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x75, 0x0, 0xBB, 0x0,
	 0x3F, 0x6, 0x68, 0x1, 0xC2, 0x7, 0xD7, 0x6, 0xA6, 0x7, 0x99, 0x1,
	 0xC2, 0x0, 0x40, 0x2, 0x0, 0x2, 0x0},	/*                   */

	{0x1, 0x7, 0xC0, 0x7, 0xC0, 0x7, 0xC0, 0x2, 0x54, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x2, 0x54, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2,
	 0x54, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},	/*                         */

	{0x2, 0x7, 0xC0, 0x7, 0xC0, 0x7, 0xC0, 0x2, 0x59, 0x1, 0x32, 0x0,
	 0x75, 0x6, 0x4A, 0x2, 0x0C, 0x7, 0xAB, 0x6, 0xA5, 0x7, 0x4F, 0x2,
	 0x0C, 0x0, 0x40, 0x2, 0x0, 0x2, 0x0},	/*                      */

	{0x2, 0x7, 0xC0, 0x7, 0xC0, 0x7, 0xC0, 0x2, 0xDC, 0x0, 0xDA, 0x0,
	 0x4A, 0x6, 0x24, 0x2, 0x0C, 0x7, 0xD0, 0x6, 0x6C, 0x7, 0x88, 0x2,
	 0x0C, 0x0, 0x40, 0x2, 0x0, 0x2, 0x0},	/*                      */

	{0x0, 0x7, 0xC0, 0x6, 0x0, 0x6, 0x0, 0x1, 0x2A, 0x7, 0x30, 0x7,
	 0x9C, 0x1, 0x2A, 0x1, 0x99, 0x0, 0x0, 0x1, 0x2A, 0x0, 0x0, 0x2,
	 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},	/*                   */

	{0x1, 0x7, 0xC0, 0x6, 0x0, 0x6, 0x0, 0x2, 0x0, 0x6, 0x9A, 0x7,
	 0x54, 0x2, 0x0, 0x2, 0xBE, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x3,
	 0x77, 0x0, 0x40, 0x0, 0x40, 0x0, 0x40},	/*                      */

	{0x1, 0x7, 0xC0, 0x6, 0x0, 0x6, 0x0, 0x2, 0x0, 0x7, 0x96, 0x7,
	 0xC5, 0x0, 0x0, 0x2, 0x0D, 0x0, 0x26, 0x0, 0x0, 0x0, 0x3B, 0x2,
	 0x0A, 0x0, 0x40, 0x2, 0x0, 0x2, 0x0},	/*                */

	{0x0, 0x7, 0xC0, 0x6, 0x0, 0x6, 0x0, 0x1, 0x2A, 0x7, 0x77, 0x7,
	 0xC9, 0x1, 0x2A, 0x1, 0xCB, 0x0, 0x0, 0x1, 0x2A, 0x0, 0x0, 0x2,
	 0x1D, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},	/*                   */

	{0x1, 0x7, 0xC0, 0x6, 0x0, 0x6, 0x0, 0x2, 0x0, 0x7, 0x16, 0x7,
	 0xA2, 0x2, 0x0, 0x3, 0x14, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x3,
	 0xA1, 0x0, 0x40, 0x0, 0x40, 0x0, 0x40},	/*                      */

	{0x1, 0x7, 0xC0, 0x6, 0x0, 0x6, 0x0, 0x2, 0x0, 0x0, 0x62, 0x0,
	 0x33, 0x0, 0x0, 0x1, 0xF7, 0x7, 0xDB, 0x0, 0x0, 0x7, 0xC7, 0x1,
	 0xFB, 0x0, 0x40, 0x2, 0x0, 0x2, 0x0}	/*                */
};

/* 
                                                                     
                                         
                                                                         
                                                                       
 */
static Int kMatrixIndex[HDMITX_CS_NUM][HDMITX_CS_NUM] = {
	{-1, E_MATRIX_RGBF_2_RGBL, E_MATRIX_RGBF_2_BT601, E_MATRIX_RGBF_2_BT709},
	{E_MATRIX_RGBL_2_RGBF, -1, E_MATRIX_RGBL_2_BT601, E_MATRIX_RGBL_2_BT709},
	{E_MATRIX_BT601_2_RGBF, E_MATRIX_BT601_2_RGBL, -1, E_MATRIX_BT601_2_BT709},
	{E_MATRIX_BT709_2_RGBF, E_MATRIX_BT709_2_RGBL, E_MATRIX_BT709_2_BT601, -1}
};

/* 
                                                         
                                                                      
                  
 */
static UInt8 MatrixCoeffBlueScreen[][MATRIX_PRESET_SIZE] = {
	{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0},	/*                                        */

	{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x1, 0x0, 0x3, 0x0},	/*                                             */

	{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x1, 0x0, 0x3, 0x0},	/*                                             */
};

/* 
                                                          
                                                                      
                  
 */
static UInt8 MatrixCoeffBlackScreen[][MATRIX_PRESET_SIZE] = {
	{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},	/*                                         */

	{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x2, 0x0, 0x2, 0x0},	/*                                              */

	{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x0, 0x2, 0x0, 0x2, 0x0},	/*                                              */
};


/*                                                                            */
/*                                                                          */
/*                                                                            */
#define HDMITX_LAT_SCO_MAX_VAL 40
#define HDMITX_LAT_SCO_MIN_VAL 34

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
static UInt8 gMatContrl[HDMITX_UNITS_MAX];
static UInt8 gHvfCntrl0[HDMITX_UNITS_MAX];
static UInt8 gHvfCntrl1[HDMITX_UNITS_MAX];

/*                                                                            */
/*                                                                            */
/*                                                                            */

static tmErrorCode_t setDeVs(tmHdmiTxobject_t *pDis,
			     tmbslHdmiTxVidFmt_t voutFmt, tmbslHdmiTx3DStructure_t structure3D);
static tmErrorCode_t setPixelRepeat(tmHdmiTxobject_t *pDis,
				    tmbslHdmiTxVidFmt_t voutFmt,
				    UInt8 uPixelRepeat, tmbslHdmiTx3DStructure_t structure3D);
static tmErrorCode_t setSampling(tmHdmiTxobject_t *pDis);
static UInt8 calculateChecksum(UInt8 *pData, Int numBytes);
static UInt8 reg_vid_fmt(tmbslHdmiTxVidFmt_t fmt,
			 tmbslHdmiTx3DStructure_t structure3D,
			 UInt8 *idx, UInt8 *idx3d, struct sync_desc **sync);
UInt8 pix_clk(tmbslHdmiTxVidFmt_t fmt, tmbslHdmiTxVfreq_t freq, UInt8 *pclk);
static tmErrorCode_t InputConfig(tmHdmiTxobject_t *pDis,
				 tmbslHdmiTxVinMode_t vinMode,
				 tmbslHdmiTxPixEdge_t sampleEdge,
				 tmbslHdmiTxPixRate_t pixRate,
				 tmbslHdmiTxUpsampleMode_t upsampleMode,
				 UInt8 uPixelRepeat,
				 tmbslHdmiTxVidFmt_t voutFmt, tmbslHdmiTx3DStructure_t structure3D);
/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989AudioInResetCts(tmUnitSelect_t txUnit) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                     */
	    err = setHwRegisterFieldTable(pDis, &kResetCtsGenerator[0]);
	return err;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989AudioInSetConfig
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxaFmt_t aFmt,
     tmbslHdmiTxI2sFor_t i2sFormat,
     UInt8 chanI2s,
     UInt8 chanDsd,
     tmbslHdmiTxClkPolDsd_t clkPolDsd,
     tmbslHdmiTxSwapDsd_t swapDsd, UInt8 layout, UInt16 uLatency_rd, tmbslHdmiTxDstRate_t dstRate) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regVal;		/*                            */

	DUMMY_ACCESS(dstRate);

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                            */
#ifdef TMFL_HBR_SUPPORT
	    RETIF_BADPARAM((aFmt != HDMITX_AFMT_SPDIF) &&
			   (aFmt != HDMITX_AFMT_I2S) &&
			   (aFmt != HDMITX_AFMT_OBA) && (aFmt != HDMITX_AFMT_HBR))
#else				/*                  */
	    RETIF_BADPARAM((aFmt != HDMITX_AFMT_SPDIF) &&
			   (aFmt != HDMITX_AFMT_I2S) && (aFmt != HDMITX_AFMT_OBA))

#endif				/*                  */
	    RETIF_BADPARAM(chanI2s >= HDMITX_CHAN_INVALID)
	    RETIF_BADPARAM(chanDsd >= HDMITX_CHAN_INVALID)
	    RETIF_BADPARAM(clkPolDsd >= HDMITX_CLKPOLDSD_INVALID)
	    RETIF_BADPARAM(swapDsd >= HDMITX_SWAPDSD_INVALID)
	    RETIF_BADPARAM(layout >= HDMITX_LAYOUT_INVALID)
	    RETIF_BADPARAM(uLatency_rd >= HDMITX_LATENCY_INVALID)

	    if ((aFmt == HDMITX_AFMT_I2S)
#ifdef TMFL_HBR_SUPPORT
		|| (aFmt == HDMITX_AFMT_HBR)
#endif				/*                  */
	    ) {
		RETIF_BADPARAM((i2sFormat != HDMITX_I2SFOR_PHILIPS_L) &&
			       (i2sFormat != HDMITX_I2SFOR_OTH_L) &&
			       (i2sFormat != HDMITX_I2SFOR_OTH_R)
		    )
	}
#ifdef TMFL_HDCP_OPTIMIZED_POWER
	/*
                      
                             
  */
	err = setHwRegisterField(pDis, E_REG_FEAT_POWER_DOWN,
				 E_MASKREG_FEAT_POWER_DOWN_spdif, (aFmt != HDMITX_AFMT_SPDIF));
	RETIF_REG_FAIL(err);
#endif

	switch (aFmt) {
	case HDMITX_AFMT_SPDIF:
		regVal = (UInt8) REG_VAL_SEL_AIP_SPDIF;
		/*                  */
		err = setHwRegister(pDis, E_REG_P00_MUX_AP_RW, TDA19989_MUX_AP_SELECT_SPDIF);
		RETIF_REG_FAIL(err)
		    break;

	case HDMITX_AFMT_I2S:
		regVal = (UInt8) REG_VAL_SEL_AIP_I2S;
		/*                  */
		err = setHwRegister(pDis, E_REG_P00_MUX_AP_RW, TDA19989_MUX_AP_SELECT_I2S);
		RETIF_REG_FAIL(err)
		    break;

	case HDMITX_AFMT_OBA:
		regVal = (UInt8) REG_VAL_SEL_AIP_OBA;
		break;

	case HDMITX_AFMT_HBR:
		regVal = (UInt8) REG_VAL_SEL_AIP_HBR;
		break;

	default:
		return TMBSL_ERR_HDMI_BAD_PARAMETER;
	}


	/*                                               */
	err = setHwRegisterField(pDis,
				 E_REG_P00_AIP_CLKSEL_W, E_MASKREG_P00_AIP_CLKSEL_sel_aip, regVal);
	RETIF_REG_FAIL(err)

	    /*                              */
	    err = setHwRegisterField(pDis,
				     E_REG_P11_CA_I2S_RW, E_MASKREG_P11_CA_I2S_hbr_chstat_4, 0);
	RETIF_REG_FAIL(err)

	    /*                         */
	    if ((aFmt == HDMITX_AFMT_I2S)
#ifdef TMFL_HBR_SUPPORT
		|| (aFmt == HDMITX_AFMT_HBR)
#endif				/*                  */
	    ) {
		if (chanI2s != HDMITX_CHAN_NO_CHANGE) {
			err = setHwRegisterField(pDis,
						 E_REG_P11_CA_I2S_RW,
						 E_MASKREG_P11_CA_I2S_ca_i2s, (UInt8) chanI2s);
		}

		/*                       */
		err = setHwRegisterField(pDis,
					 E_REG_P00_I2S_FORMAT_RW,
					 E_MASKREG_P00_I2S_FORMAT_i2s_format, (UInt8) i2sFormat);
		RETIF_REG_FAIL(err)
/*                         */
		    /*                              */
		    /*   */
		    /*                              */
		    /*                                */
		    /*                      */
		    /*                                    */
		    /*     */
		    /*                     */
		    /*   */
/*                            */ */
	} else if (aFmt == HDMITX_AFMT_OBA) {
		if (chanDsd != HDMITX_CHAN_NO_CHANGE) {
			err = setHwRegister(pDis, E_REG_P11_CA_DSD_RW, chanDsd);
			RETIF_REG_FAIL(err)
		}
		if (clkPolDsd != HDMITX_CLKPOLDSD_NO_CHANGE) {
			err = setHwRegisterField(pDis,
						 E_REG_P00_AIP_CLKSEL_W,
						 E_MASKREG_P00_AIP_CLKSEL_sel_pol_clk,
						 (UInt8) clkPolDsd);
			RETIF_REG_FAIL(err)
		}
		if (swapDsd != HDMITX_SWAPDSD_NO_CHANGE) {
			err = setHwRegisterField(pDis,
						 E_REG_P11_AIP_CNTRL_0_RW,
						 E_MASKREG_P11_AIP_CNTRL_0_swap, (UInt8) swapDsd);
			RETIF_REG_FAIL(err)
		}
	}

	/*                        */
	if (layout != HDMITX_LAYOUT_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P11_AIP_CNTRL_0_RW,
					 E_MASKREG_P11_AIP_CNTRL_0_layout, layout);
		RETIF_REG_FAIL(err)
	}
	if (uLatency_rd != HDMITX_LATENCY_NO_CHANGE) {
		err = setHwRegister(pDis, E_REG_P11_LATENCY_RD_RW, (UInt8) uLatency_rd);
		RETIF_REG_FAIL(err)
	}
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989AudioInSetCts
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxctsRef_t ctsRef,
     tmbslHdmiTxafs_t afs,
     tmbslHdmiTxVidFmt_t voutFmt,
     tmbslHdmiTxVfreq_t voutFreq,
     UInt32 uCts,
     UInt16 uCtsX, tmbslHdmiTxctsK_t ctsK, tmbslHdmiTxctsM_t ctsM, tmbslHdmiTxDstRate_t dstRate) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regVal;		/*                */
	UInt8 pixClk;		/*                   */
	UInt32 acrN;		/*                        */

	DUMMY_ACCESS(dstRate);

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                            */
	    RETIF_BADPARAM(ctsRef >= HDMITX_CTSREF_INVALID)
	    RETIF_BADPARAM(afs >= HDMITX_AFS_INVALID)
	    RETIF_BADPARAM(!IS_VALID_FMT(voutFmt))
	    RETIF_BADPARAM(voutFreq >= HDMITX_VFREQ_INVALID)
	    RETIF_BADPARAM(uCtsX >= HDMITX_CTSX_INVALID)
	    RETIF_BADPARAM(ctsK >= HDMITX_CTSK_INVALID)
	    RETIF_BADPARAM(ctsM >= HDMITX_CTSMTS_INVALID)


	    if (IS_TV(voutFmt)) {
		if (voutFreq == HDMITX_VFREQ_50Hz) {
			RETIF(((voutFmt < HDMITX_VFMT_17_720x576p_50Hz)
			       || (voutFmt > HDMITX_VFMT_31_1920x1080p_50Hz)),
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS);
		} else if (voutFreq == HDMITX_VFREQ_24Hz) {
			RETIF((voutFmt != HDMITX_VFMT_32_1920x1080p_24Hz)
			      && (voutFmt != HDMITX_VFMT_60_1280x720p_24Hz),
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS);
		} else if (voutFreq == HDMITX_VFREQ_25Hz) {
			RETIF((voutFmt != HDMITX_VFMT_33_1920x1080p_25Hz)
			      && (voutFmt != HDMITX_VFMT_20_1920x1080i_50Hz)
			      && (voutFmt != HDMITX_VFMT_61_1280x720p_25Hz),
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS);
		} else if (voutFreq == HDMITX_VFREQ_30Hz) {
			RETIF((voutFmt != HDMITX_VFMT_34_1920x1080p_30Hz)
			      && (voutFmt != HDMITX_VFMT_05_1920x1080i_60Hz)
			      && (voutFmt != HDMITX_VFMT_62_1280x720p_30Hz),
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS);
		} else {
			RETIF(voutFmt >= HDMITX_VFMT_17_720x576p_50Hz,
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS);
		}
	}
#ifdef FORMAT_PC
	if (IS_PC(voutFmt)) {
		if (voutFreq == HDMITX_VFREQ_60Hz) {
			RETIF(voutFmt > HDMITX_VFMT_PC_1600x1200p_60Hz,
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)
		} else if (voutFreq == HDMITX_VFREQ_70Hz) {
			RETIF(voutFmt != HDMITX_VFMT_PC_1024x768p_70Hz,
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)
		} else if (voutFreq == HDMITX_VFREQ_72Hz) {
			RETIF(((voutFmt < HDMITX_VFMT_PC_640x480p_72Hz) ||
			       (voutFmt > HDMITX_VFMT_PC_800x600p_72Hz)),
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)
		} else if (voutFreq == HDMITX_VFREQ_75Hz) {
			RETIF(((voutFmt < HDMITX_VFMT_PC_640x480p_75Hz) ||
			       (voutFmt > HDMITX_VFMT_PC_1280x1024p_75Hz)),
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)
		} else if (voutFreq == HDMITX_VFREQ_85Hz) {
			RETIF(((voutFmt < HDMITX_VFMT_PC_640x350p_85Hz) ||
			       (voutFmt > HDMITX_VFMT_PC_1280x1024p_85Hz)),
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)
		} else {
			RETIF(voutFmt != HDMITX_VFMT_PC_1024x768i_87Hz,
			      TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)
		}
	}
#endif				/*           */

	/*                              */
	if (uCts == HDMITX_CTS_AUTO) {
		/*      */
		err = setHwRegisterField(pDis,
					 E_REG_P11_AIP_CNTRL_0_RW,
					 E_MASKREG_P11_AIP_CNTRL_0_acr_man, 0);
		RETIF_REG_FAIL(err)
	} else {
		/*        */
		err = setHwRegisterField(pDis,
					 E_REG_P11_AIP_CNTRL_0_RW,
					 E_MASKREG_P11_AIP_CNTRL_0_acr_man, 1);
		RETIF_REG_FAIL(err)
	}

	/*                        */
	if ((ctsM == HDMITX_CTSMTS_USE_CTSX) || (ctsK == HDMITX_CTSK_USE_CTSX)) {
		RETIF_BADPARAM(uCtsX == HDMITX_CTSX_UNUSED)
		    ctsM = (tmbslHdmiTxctsM_t) kCtsXToMK[uCtsX][0];
		ctsK = (tmbslHdmiTxctsK_t) kCtsXToMK[uCtsX][1];
	}

	/*                                                */
	regVal = (UInt8) ctsM;
	err = setHwRegisterField(pDis, E_REG_P11_CTS_N_RW, E_MASKREG_P11_CTS_N_m_sel, regVal);
	RETIF_REG_FAIL(err)

	    /*                           */
	    regVal = (UInt8) ctsK;
	err = setHwRegisterField(pDis, E_REG_P11_CTS_N_RW, E_MASKREG_P11_CTS_N_k_sel, regVal);
	RETIF_REG_FAIL(err);

	/*                                                             
                                   */
	pix_clk(voutFmt, voutFreq, &pixClk);

	if (pixClk != E_PIXCLK_INVALID) {
		/*                                                                    
                                            */
		acrN = kPixClkToAcrN[pixClk][afs];

		/*                                 */
		regVal = (UInt8) (acrN >> 16);
		err = setHwRegister(pDis, E_REG_P11_ACR_N_2_RW, regVal);
		RETIF_REG_FAIL(err)
		    /*                                */
		    regVal = (UInt8) (acrN >> 8);
		err = setHwRegister(pDis, E_REG_P11_ACR_N_1_RW, regVal);
		RETIF_REG_FAIL(err)
		    /*                               */
		    regVal = (UInt8) acrN;
		err = setHwRegister(pDis, E_REG_P11_ACR_N_0_RW, regVal);
		RETIF_REG_FAIL(err)

		    /*                                                                       
                                      */
		    regVal = kPixClkToAdiv[pixClk][afs];
		err = setHwRegister(pDis, E_REG_P02_AUDIO_DIV_RW, regVal);
		RETIF_REG_FAIL(err)

		    /*                                                     
                                                */
		    if (uCts == HDMITX_CTS_AUTO) {
			uCts = kPixClkToAcrCts[pixClk][afs];
		}
	}

	/*                               */
	if (uCts != HDMITX_CTS_AUTO) {
		/*                              */
		regVal = (UInt8) (uCts >> 16);
		err = setHwRegister(pDis, E_REG_P11_ACR_CTS_2_RW, regVal);
		RETIF_REG_FAIL(err)
		    /*                              */
		    regVal = (UInt8) (uCts >> 8);
		err = setHwRegister(pDis, E_REG_P11_ACR_CTS_1_RW, regVal);
		RETIF_REG_FAIL(err)
		    /*                             */
		    regVal = (UInt8) uCts;
		err = setHwRegister(pDis, E_REG_P11_ACR_CTS_0_RW, regVal);
		RETIF_REG_FAIL(err)
	}

	/*                                                          */
	regVal = (UInt8) ctsRef;
	err = setHwRegisterField(pDis,
				 E_REG_P00_AIP_CLKSEL_W, E_MASKREG_P00_AIP_CLKSEL_sel_fs, regVal);
	RETIF_REG_FAIL(err)

	    /*                                     */
	    err = setHwRegisterFieldTable(pDis, &kResetCtsGenerator[0]);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989AudioOutSetChanStatus
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxAudioData_t pcmIdentification,
     tmbslHdmiTxCSformatInfo_t formatInfo,
     tmbslHdmiTxCScopyright_t copyright,
     UInt8 categoryCode,
     tmbslHdmiTxafs_t sampleFreq,
     tmbslHdmiTxCSclkAcc_t clockAccuracy,
     tmbslHdmiTxCSmaxWordLength_t maxWordLength,
     tmbslHdmiTxCSwordLength_t wordLength, tmbslHdmiTxCSorigAfs_t origSampleFreq) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[4];		/*                                    */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                            */
	    RETIF_BADPARAM(formatInfo >= HDMITX_CSFI_INVALID)
	    RETIF_BADPARAM(copyright >= HDMITX_CSCOPYRIGHT_INVALID)
	    RETIF_BADPARAM(sampleFreq > HDMITX_AFS_NOT_INDICATED)
	    RETIF_BADPARAM(clockAccuracy >= HDMITX_CSCLK_INVALID)
	    RETIF_BADPARAM(maxWordLength >= HDMITX_CSMAX_INVALID)
	    RETIF_BADPARAM(wordLength >= HDMITX_CSWORD_INVALID)
	    RETIF_BADPARAM(wordLength == HDMITX_CSWORD_RESVD)
	    RETIF_BADPARAM(origSampleFreq >= HDMITX_CSAFS_INVALID)
	    RETIF_BADPARAM(pcmIdentification >= HDMITX_AUDIO_DATA_INVALID)

	    /*                */
	    buf[0] =
	    ((UInt8) formatInfo << 3) | ((UInt8) copyright << 2) | ((UInt8) pcmIdentification << 1);

	/*                */
	buf[1] = categoryCode;

	/*                                                                       */
	buf[2] = ((UInt8) clockAccuracy << 4) | kAfsToCSbyte3[sampleFreq];

	/*                */
	buf[3] = ((UInt8) origSampleFreq << 4) | ((UInt8) wordLength << 1) | (UInt8) maxWordLength;

	/*                              */
	err = setHwRegisters(pDis, E_REG_P11_CH_STAT_B_0_RW, &buf[0], 4);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t
    tmbslTDA9989AudioOutSetChanStatusMapping
    (tmUnitSelect_t txUnit,
     UInt8 sourceLeft[4], UInt8 channelLeft[4], UInt8 sourceRight[4], UInt8 channelRight[4]
    ) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[2];		/*                                    */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                            */
	    RETIF_BADPARAM(sourceLeft[0] > HDMITX_CS_SOURCES_MAX)
	    RETIF_BADPARAM(channelLeft[0] > HDMITX_CS_CHANNELS_MAX)
	    RETIF_BADPARAM(sourceRight[0] > HDMITX_CS_SOURCES_MAX)
	    RETIF_BADPARAM(channelRight[0] > HDMITX_CS_CHANNELS_MAX)

	    /*                   */
	    buf[0] = ((UInt8) channelLeft[0] << 4) | (UInt8) sourceLeft[0];

	/*                    */
	buf[1] = ((UInt8) channelRight[0] << 4) | (UInt8) sourceRight[0];

	/*                              */
	err = setHwRegisters(pDis, E_REG_P11_CH_STAT_B_2_ap0_l_RW, &buf[0], 2);
	return err;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989AudioOutSetMute(tmUnitSelect_t txUnit, tmbslHdmiTxaMute_t aMute) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                              */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                            */
	    RETIF_BADPARAM(aMute >= HDMITX_AMUTE_INVALID)

	    /*                                                         */
	    if (aMute == HDMITX_AMUTE_ON) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_SR_REG_W,
					 E_MASKREG_P00_SR_REG_sr_audio, (UInt8) aMute);
		RETIF(err != TM_OK, err)


		    err = setHwRegisterField(pDis,
					     E_REG_P00_SR_REG_W,
					     E_MASKREG_P00_SR_REG_sr_audio, (UInt8) !aMute);
		RETIF(err != TM_OK, err)

	}

	/*                                    */
	err = setHwRegisterField(pDis,
				 E_REG_P11_AIP_CNTRL_0_RW,
				 E_MASKREG_P11_AIP_CNTRL_0_rst_fifo, (UInt8) aMute);
	RETIF(err != TM_OK, err)


	    return TM_OK;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t tmbslTDA9989ScalerGet(tmUnitSelect_t txUnit, tmbslHdmiTxScalerDiag_t *pScalerDiag) {
	DUMMY_ACCESS(txUnit);	/*                     */
	DUMMY_ACCESS(pScalerDiag);	/*                     */
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989ScalerGetMode(tmUnitSelect_t txUnit, tmbslHdmiTxScaMode_t *pScalerMode) {
	DUMMY_ACCESS(txUnit);	/*                     */
	DUMMY_ACCESS(pScalerMode);	/*                               */
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989ScalerInDisable(tmUnitSelect_t txUnit, Bool bDisable) {
	DUMMY_ACCESS(txUnit);	/*                     */
	DUMMY_ACCESS(bDisable);
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t
    tmbslTDA9989ScalerSetCoeffs(tmUnitSelect_t txUnit, tmbslHdmiTxScaLut_t lutSel, UInt8 *pVsLut) {
	DUMMY_ACCESS(txUnit);	/*                     */
	DUMMY_ACCESS(lutSel);	/*                               */
	DUMMY_ACCESS(pVsLut);	/*                               */
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989ScalerSetFieldOrder
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxIntExt_t topExt,
     tmbslHdmiTxIntExt_t deExt, tmbslHdmiTxTopSel_t topSel, tmbslHdmiTxTopTgl_t topTgl) {
	DUMMY_ACCESS(txUnit);	/*                     */
	DUMMY_ACCESS(deExt);	/*                               */
	DUMMY_ACCESS(topExt);	/*                               */
	DUMMY_ACCESS(topSel);	/*                               */
	DUMMY_ACCESS(topTgl);	/*                               */
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989ScalerSetPhase(tmUnitSelect_t txUnit, tmbslHdmiTxHPhases_t horizontalPhases) {
	DUMMY_ACCESS(txUnit);	/*                     */
	DUMMY_ACCESS(horizontalPhases);	/*                               */
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                              */
/*                                                                            */
tmErrorCode_t tmbslTDA9989ScalerSetLatency(tmUnitSelect_t txUnit, UInt8 scaler_latency) {
	DUMMY_ACCESS(txUnit);	/*                     */
	DUMMY_ACCESS(scaler_latency);	/*                               */
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989ScalerSetFine(tmUnitSelect_t txUnit, UInt16 uRefPix, UInt16 uRefLine) {
	DUMMY_ACCESS(txUnit);	/*                     */
	DUMMY_ACCESS(uRefPix);	/*                               */
	DUMMY_ACCESS(uRefLine);	/*                               */
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989ScalerSetSync
    (tmUnitSelect_t txUnit, tmbslHdmiTxVsMeth_t method, tmbslHdmiTxVsOnce_t once) {
	DUMMY_ACCESS(txUnit);	/*                     */
	DUMMY_ACCESS(method);	/*                               */
	DUMMY_ACCESS(once);	/*                               */
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;

}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989TmdsSetOutputs(tmUnitSelect_t txUnit, tmbslHdmiTxTmdsOut_t tmdsOut) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(tmdsOut >= HDMITX_TMDSOUT_INVALID)

	    /*                          */
	    err = setHwRegisterField(pDis,
				     E_REG_P02_BUFFER_OUT_RW,
				     E_MASKREG_P02_BUFFER_OUT_srl_force, (UInt8) tmdsOut);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989TmdsSetSerializer(tmUnitSelect_t txUnit, UInt8 uPhase2, UInt8 uPhase3) {

	DUMMY_ACCESS(txUnit);
	DUMMY_ACCESS(uPhase2);
	DUMMY_ACCESS(uPhase3);
	return TMBSL_ERR_HDMI_NOT_SUPPORTED;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989TestSetPattern(tmUnitSelect_t txUnit, tmbslHdmiTxTestPattern_t pattern) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 serviceMode;	/*                */
	UInt8 bars8;		/*                */
	UInt8 buf[MATRIX_PRESET_SIZE];	/*             */
	UInt8 i;		/*            */
	UInt8 *MatrixCoeff = Null;

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                          */
	    switch (pattern) {
	case HDMITX_PATTERN_CBAR4:
		serviceMode = 1;
		bars8 = 0;
		break;
	case HDMITX_PATTERN_BLUE:
		MatrixCoeff = (UInt8 *) &MatrixCoeffBlueScreen[pDis->voutMode][0];
		serviceMode = 1;
		bars8 = 1;
		break;
	case HDMITX_PATTERN_BLACK:
		MatrixCoeff = (UInt8 *) &MatrixCoeffBlackScreen[pDis->voutMode][0];
	case HDMITX_PATTERN_CBAR8:
		serviceMode = 1;
		bars8 = 1;
		break;
	case HDMITX_PATTERN_OFF:
		serviceMode = 0;
		bars8 = 0;
		break;
	default:
		return TMBSL_ERR_HDMI_BAD_PARAMETER;
	}

	if (serviceMode) {
		if (!pDis->prevPattern) {	/*                                                 */
			/*                                                                            */
			/*                                                      */
			setHwRegisterFieldTable(pDis, &kBypassColourProc[0]);
		}
		if ((pattern == HDMITX_PATTERN_BLUE) || (pattern == HDMITX_PATTERN_BLACK)) {	/*                              */

			/*                                                                                                                          */
			/*                                                                 
                               */

			/*                                                                 
                               */
			err = setHwRegister(pDis, E_REG_P00_MAT_CONTRL_W, MatrixCoeff[0]);
			RETIF_REG_FAIL(err)

			    for (i = 0; i < MATRIX_PRESET_SIZE; i++) {
				buf[i] = MatrixCoeff[i];
			}

			/*                           */
			err = setHwRegisters(pDis,
					     E_REG_P00_MAT_OI1_MSB_W,
					     &buf[1], MATRIX_PRESET_SIZE - 1);
			RETIF_REG_FAIL(err)
			    pDis->prevFilterPattern = True;
		} else {	/*                      */

			/*                           */
			err = setHwRegisterField(pDis,
						 E_REG_P00_HVF_CNTRL_0_W,
						 E_MASKREG_P00_HVF_CNTRL_0_rwb, bars8);
			RETIF_REG_FAIL(err)

			    /*                                                      */
			    setHwRegisterFieldTable(pDis, &kBypassColourProc[0]);
		}
		pDis->prevPattern = True;
	} else {		/*                  */

		if (pDis->prevFilterPattern) {
			/*                                                   */
			err =
			    tmbslTDA9989MatrixSetConversion(txUnit, pDis->vinFmt, pDis->vinMode,
							    pDis->voutFmt, pDis->voutMode,
							    pDis->dviVqr);
			RETIF_REG_FAIL(err)

			    pDis->prevFilterPattern = False;
		}
		/*                                                           */
		setHwRegister(pDis, E_REG_P00_MAT_CONTRL_W, gMatContrl[txUnit]);
		setHwRegister(pDis, E_REG_P00_HVF_CNTRL_0_W, gHvfCntrl0[txUnit]);
		setHwRegister(pDis, E_REG_P00_HVF_CNTRL_1_W, gHvfCntrl1[txUnit]);
		pDis->prevPattern = False;
	}

	/*                            */
	err = setHwRegisterField(pDis,
				 E_REG_P00_HVF_CNTRL_0_W,
				 E_MASKREG_P00_HVF_CNTRL_0_sm, serviceMode);
#ifdef TMFL_HDCP_SUPPORT
	pDis->HDCPIgnoreEncrypt = True;	/*                          */
#endif				/*                   */

	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989VideoInSetBlanking
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxBlnkSrc_t blankitSource, tmbslHdmiTxBlnkCode_t blankingCodes) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(blankitSource >= HDMITX_BLNKSRC_INVALID)
	    RETIF_BADPARAM(blankingCodes >= HDMITX_BLNKCODE_INVALID)

	    /*                                                            */
	    if (blankitSource != HDMITX_BLNKSRC_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_4_W,
					 E_MASKREG_P00_VIP_CNTRL_4_blankit, (UInt8) blankitSource);
		RETIF_REG_FAIL(err)
	}
	if (blankingCodes != HDMITX_BLNKCODE_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_4_W,
					 E_MASKREG_P00_VIP_CNTRL_4_blc, (UInt8) blankingCodes);
		RETIF_REG_FAIL(err)
	}

	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989VideoInSetConfig
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxVinMode_t vinMode,
     tmbslHdmiTxVidFmt_t voutFmt,
     tmbslHdmiTx3DStructure_t structure3D,
     tmbslHdmiTxPixEdge_t sampleEdge,
     tmbslHdmiTxPixRate_t pixRate, tmbslHdmiTxUpsampleMode_t upsampleMode) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(vinMode >= HDMITX_VINMODE_INVALID)
	    RETIF_BADPARAM(sampleEdge >= HDMITX_PIXEDGE_INVALID)
	    RETIF_BADPARAM(pixRate >= HDMITX_PIXRATE_INVALID)
	    RETIF_BADPARAM(upsampleMode >= HDMITX_UPSAMPLE_INVALID)

	    err = InputConfig(pDis,
			      vinMode,
			      sampleEdge,
			      pixRate, upsampleMode, HDMITX_PIXREP_NO_CHANGE, voutFmt, structure3D);
	RETIF_REG_FAIL(err)

	    return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989VideoInSetFine
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxPixSubpkt_t subpacketCount, tmbslHdmiTxPixTogl_t toggleClk1) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(subpacketCount >= HDMITX_PIXSUBPKT_INVALID)
	    RETIF_BADPARAM(toggleClk1 >= HDMITX_PIXTOGL_INVALID)

	    /*                                                                      
                                                       
      */
	    if (subpacketCount <= HDMITX_PIXSUBPKT_FIX_3) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_5_W,
					 E_MASKREG_P00_VIP_CNTRL_5_sp_cnt, (UInt8) subpacketCount);
		RETIF_REG_FAIL(err)
		    err = setHwRegisterField(pDis,
					     E_REG_P00_VIP_CNTRL_3_W,
					     E_MASKREG_P00_VIP_CNTRL_3_sp_sync,
					     HDMITX_PIXSUBPKT_SYNC_FIXED);
		RETIF_REG_FAIL(err)
	}
	/*                                                                
                                                                          
                                        
  */
	else if (subpacketCount != HDMITX_PIXSUBPKT_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_5_W,
					 E_MASKREG_P00_VIP_CNTRL_5_sp_cnt, HDMITX_PIXSUBPKT_FIX_0);
		RETIF_REG_FAIL(err)

		    err = setHwRegisterField(pDis,
					     E_REG_P00_VIP_CNTRL_3_W,
					     E_MASKREG_P00_VIP_CNTRL_3_sp_sync,
					     (UInt8) (subpacketCount -
						      HDMITX_PIXSUBPKT_SYNC_FIRST));
		RETIF_REG_FAIL(err)
	}

	/*                                                         */
	if (toggleClk1 != HDMITX_PIXTOGL_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_5_W,
					 E_MASKREG_P00_VIP_CNTRL_5_ckcase, (UInt8) toggleClk1);
		RETIF_REG_FAIL(err)
	}
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989VideoInSetMapping
#ifdef TMFL_RGB_DDR_12BITS
    (tmUnitSelect_t txUnit, UInt8 *pSwapTable, UInt8 *pMirrorTable, UInt8 *pMux)
#else
    (tmUnitSelect_t txUnit, UInt8 *pSwapTable, UInt8 *pMirrorTable)
#endif
{
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	Int i;			/*              */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(pSwapTable == Null)
	    RETIF_BADPARAM(pMirrorTable == Null)
	    for (i = 0; i < HDMITX_VIN_PORT_MAP_TABLE_LEN; i++) {
		RETIF_BADPARAM(pSwapTable[i] >= HDMITX_VIN_PORT_SWAP_INVALID)
		    RETIF_BADPARAM(pMirrorTable[i] >= HDMITX_VIN_PORT_MIRROR_INVALID)
	}

	/*                                                                        
                 
  */
	for (i = 0; i < HDMITX_VIN_PORT_MAP_TABLE_LEN; i++) {
		if (pSwapTable[0] < HDMITX_VIN_PORT_SWAP_NO_CHANGE) {
			err = setHwRegisterField(pDis,
						 kRegVip[i].Register,
						 kRegVip[i].MaskSwap, pSwapTable[i]);
			RETIF_REG_FAIL(err)
		}
	}

	/*                                                                       
                        
  */
	for (i = 0; i < HDMITX_VIN_PORT_MAP_TABLE_LEN; i++) {
		if (pMirrorTable[0] < HDMITX_VIN_PORT_MIRROR_NO_CHANGE) {
			err = setHwRegisterField(pDis,
						 kRegVip[i].Register,
						 kRegVip[i].MaskMirror, pMirrorTable[i]);
			RETIF_REG_FAIL(err)
		}
	}

#ifdef TMFL_RGB_DDR_12BITS
	/*
                          
  */
	err = setHwRegister(pDis, E_REG_P00_MUX_VP_VIP_OUT_RW, *pMux);
	RETIF_REG_FAIL(err);
#endif

	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989SetVideoPortConfig
    (tmUnitSelect_t txUnit, UInt8 *pEnaVideoPortTable, UInt8 *pGndVideoPortTable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(pEnaVideoPortTable == Null)
	    RETIF_BADPARAM(pGndVideoPortTable == Null)

	    err = setHwRegister(pDis, E_REG_P00_ENA_VP_0_RW, pEnaVideoPortTable[0]);
	RETIF_REG_FAIL(err)

	    err = setHwRegister(pDis, E_REG_P00_ENA_VP_1_RW, pEnaVideoPortTable[1]);
	RETIF_REG_FAIL(err)

	    err = setHwRegister(pDis, E_REG_P00_ENA_VP_2_RW, pEnaVideoPortTable[2]);
	RETIF_REG_FAIL(err)

	    /*                            
                              
                               
                            */
	    /*                           
                              
                               
                            */
	    /*                          
                              
                               
                            */
	    return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989SetAudioPortConfig
    (tmUnitSelect_t txUnit, UInt8 *pEnaAudioPortTable, UInt8 *pGndAudioPortTable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(pEnaAudioPortTable == Null)
	    RETIF_BADPARAM(pGndAudioPortTable == Null)

	    err = setHwRegister(pDis, E_REG_P00_ENA_AP_RW, pEnaAudioPortTable[0]);
	RETIF_REG_FAIL(err)

	    /*                            
                            
                               
                            */
	    return TM_OK;
}

/*                                                                            */
/*                                                                                  */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989SetAudioClockPortConfig
    (tmUnitSelect_t txUnit, UInt8 *pEnaAudioClockPortTable, UInt8 *pGndAudioClockPortTable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(pEnaAudioClockPortTable == Null)
	    RETIF_BADPARAM(pGndAudioClockPortTable == Null)

	    err = setHwRegister(pDis, E_REG_P00_ENA_ACLK_RW, pEnaAudioClockPortTable[0]);
	RETIF_REG_FAIL(err)

	    /*                         
                              
                                    
                            */
	    return TM_OK;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t set_video(tmHdmiTxobject_t *pDis, tmbslHdmiTxVidFmt_t reg_idx,
			tmHdmiTxVidReg_t *format_param)
{
	tmErrorCode_t err;
	UInt8 regVal;

	regVal = 0x00;		/*              */
	err = setHwRegister(pDis, E_REG_P00_VIDFORMAT_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].nPix;
	err = setHwRegister(pDis, E_REG_P00_NPIX_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) (format_param[reg_idx].nPix >> 8);
	err = setHwRegister(pDis, E_REG_P00_NPIX_MSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].nLine;
	err = setHwRegister(pDis, E_REG_P00_NLINE_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) (format_param[reg_idx].nLine >> 8);
	err = setHwRegister(pDis, E_REG_P00_NLINE_MSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].VsLineStart;
	err = setHwRegister(pDis, E_REG_P00_VS_LINE_STRT_1_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].VsPixStart;
	err = setHwRegister(pDis, E_REG_P00_VS_PIX_STRT_1_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) (format_param[reg_idx].VsPixStart >> 8);
	err = setHwRegister(pDis, E_REG_P00_VS_PIX_STRT_1_MSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].VsLineEnd;
	err = setHwRegister(pDis, E_REG_P00_VS_LINE_END_1_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].VsPixEnd;
	err = setHwRegister(pDis, E_REG_P00_VS_PIX_END_1_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) (format_param[reg_idx].VsPixEnd >> 8);
	err = setHwRegister(pDis, E_REG_P00_VS_PIX_END_1_MSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].HsStart;
	err = setHwRegister(pDis, E_REG_P00_HS_PIX_START_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) (format_param[reg_idx].HsStart >> 8);
	err = setHwRegister(pDis, E_REG_P00_HS_PIX_START_MSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].HsEnd;
	err = setHwRegister(pDis, E_REG_P00_HS_PIX_STOP_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) (format_param[reg_idx].HsEnd >> 8);
	err = setHwRegister(pDis, E_REG_P00_HS_PIX_STOP_MSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].ActiveVideoStart;
	err = setHwRegister(pDis, E_REG_P00_VWIN_START_1_LSB_W, regVal);
	RETIF_REG_FAIL(err);
	err = setHwRegister(pDis, E_REG_P00_VWIN_START_1_MSB_W, 0);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].ActiveVideoEnd;
	err = setHwRegister(pDis, E_REG_P00_VWIN_END_1_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) (format_param[reg_idx].ActiveVideoEnd >> 8);
	err = setHwRegister(pDis, E_REG_P00_VWIN_END_1_MSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].DeStart;
	err = setHwRegister(pDis, E_REG_P00_DE_START_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) (format_param[reg_idx].DeStart >> 8);
	err = setHwRegister(pDis, E_REG_P00_DE_START_MSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) format_param[reg_idx].DeEnd;
	err = setHwRegister(pDis, E_REG_P00_DE_STOP_LSB_W, regVal);
	RETIF_REG_FAIL(err);

	regVal = (UInt8) (format_param[reg_idx].DeEnd >> 8);
	err = setHwRegister(pDis, E_REG_P00_DE_STOP_MSB_W, regVal);
	RETIF_REG_FAIL(err);

#ifdef TMFL_RGB_DDR_12BITS
	if (format_param[reg_idx].ActiveSpaceStart) {
		/*                     */
		err = setHwRegisterField(pDis, E_REG_P00_ENABLE_SPACE_W, 0x01, 0x01);
		RETIF_REG_FAIL(err);

		/*                           */
		err = setHwRegister(pDis, E_REG_P00_VSPACE_Y_DATA_W, 0x00);
		RETIF_REG_FAIL(err);

		err = setHwRegister(pDis, E_REG_P00_VSPACE_U_DATA_W, 0x80);
		RETIF_REG_FAIL(err);

		err = setHwRegister(pDis, E_REG_P00_VSPACE_V_DATA_W, 0x80);
		RETIF_REG_FAIL(err);

		/*                         */
		regVal = (UInt8) format_param[reg_idx].ActiveSpaceStart;
		err = setHwRegister(pDis, E_REG_P00_VSPACE_START_LSB_W, regVal);
		RETIF_REG_FAIL(err);

		regVal = (UInt8) ((format_param[reg_idx].ActiveSpaceStart >> 8) & 0x0F);
		err = setHwRegister(pDis, E_REG_P00_VSPACE_START_MSB_W, regVal);
		RETIF_REG_FAIL(err);

		regVal = (UInt8) format_param[reg_idx].ActiveSpaceEnd;
		err = setHwRegister(pDis, E_REG_P00_VSPACE_END_LSB_W, regVal);
		RETIF_REG_FAIL(err);

		regVal = (UInt8) ((format_param[reg_idx].ActiveSpaceEnd >> 8) & 0x0F);
		err = setHwRegister(pDis, E_REG_P00_VSPACE_END_MSB_W, regVal);
		RETIF_REG_FAIL(err);
	} else {
		/*                                                    */
		err = setHwRegisterField(pDis, E_REG_P00_ENABLE_SPACE_W, 0x01, 0x00);
		RETIF_REG_FAIL(err);
	}
#endif

	return TM_OK;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989VideoInSetSyncAuto
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxSyncSource_t syncSource,
     tmbslHdmiTxVidFmt_t vinFmt,
     tmbslHdmiTxVinMode_t vinMode, tmbslHdmiTx3DStructure_t structure3D) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 reg_idx, reg_idx3D;	/*                                         */
	UInt8 embedded;		/*                */
	UInt8 syncMethod;	/*             */
	UInt8 toggleV;		/*          */
	UInt8 toggleH;		/*          */
	UInt8 toggleX;		/*          */
	UInt16 uRefPix;		/*               */
	UInt16 uRefLine;	/*                */
#ifdef FORMAT_PC
	UInt8 regVal;		/*              */
#endif				/*           */
	struct sync_desc *sync;

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err);

	/*                                                 */
	RETIF_BADPARAM(syncSource >= HDMITX_SYNCSRC_NO_CHANGE);
	RETIF_BADPARAM(!IS_VALID_FMT(vinFmt));

	/*                                                                      */
	/*                                                              */
	RETIF_BADPARAM(reg_vid_fmt(vinFmt, structure3D, &reg_idx, &reg_idx3D, &sync));

	/*                                        */
	embedded = 0;
	switch (syncSource) {
	case HDMITX_SYNCSRC_EXT_VS:
		syncMethod = 0;
		toggleV = sync[BASE(reg_idx)].v_toggle;
		toggleH = sync[BASE(reg_idx)].h_toggle;
		toggleX = 0;
		uRefPix = sync[BASE(reg_idx)].hfp;
		uRefLine = sync[BASE(reg_idx)].vfp;
		break;
	case HDMITX_SYNCSRC_EMBEDDED:
		embedded++;
		/*           */
	case HDMITX_SYNCSRC_EXT_VREF:
	default:
		syncMethod = 1;
		toggleV = 1;
		toggleH = 1;
		toggleX = 1;
		uRefPix = sync[BASE(reg_idx)].href;
		uRefLine = sync[BASE(reg_idx)].vref;
		break;
	}
	/*                                                            
                                                                       */
	if (vinMode != HDMITX_VINMODE_CCIR656) {
		uRefPix = uRefPix + 2;
	}

	/*                                                            */
	/*                                                            */
	/*                                                            */
	if (vinMode == HDMITX_VINMODE_CCIR656) {
		if (syncSource == HDMITX_SYNCSRC_EXT_VS) {
			if (pDis->pixRate == HDMITX_PIXRATE_DOUBLE) {

				switch (reg_idx) {
				case E_REGVFMT_720x480p_60Hz:
				case E_REGVFMT_720x480i_60Hz:
				case E_REGVFMT_720x576p_50Hz:
				case E_REGVFMT_720x576i_50Hz:
					uRefPix = uRefPix + 1;
					break;
				default:
					/*                                        */
					break;
				}

			}
		}
	}


	/*                   */
	err = setHwRegisterField(pDis,
				 E_REG_P00_VIP_CNTRL_3_W, E_MASKREG_P00_VIP_CNTRL_3_emb, embedded);
	RETIF_REG_FAIL(err)

	    /*                 */
	    err = setHwRegisterField(pDis,
				     E_REG_P00_TBG_CNTRL_0_W,
				     E_MASKREG_P00_TBG_CNTRL_0_sync_mthd, syncMethod);
	RETIF_REG_FAIL(err)

/*                                                                           */
	    /*               */
	    err = setHwRegisterField(pDis,
				     E_REG_P00_VIP_CNTRL_3_W,
				     E_MASKREG_P00_VIP_CNTRL_3_v_tgl, toggleV);
	RETIF_REG_FAIL(err)
	    err = setHwRegisterField(pDis,
				     E_REG_P00_VIP_CNTRL_3_W,
				     E_MASKREG_P00_VIP_CNTRL_3_h_tgl, toggleH);
	RETIF_REG_FAIL(err)

	    /*              */
	    err = setHwRegisterField(pDis,
				     E_REG_P00_VIP_CNTRL_3_W,
				     E_MASKREG_P00_VIP_CNTRL_3_x_tgl, toggleX);
	RETIF_REG_FAIL(err);

#ifdef TMFL_RGB_DDR_12BITS
	if (syncSource == HDMITX_SYNCSRC_EXT_VREF) {
		if (structure3D == HDMITX_3D_FRAME_PACKING) {
			/*
                             
                             

                                                                                
                                                                                 
                                                                    

                  
                 
                         
                                                 
                         
                                                                              

                  
                 
                           
                                               
                         
                                                                                  


                                                                               
                                                                                 
                                                                                  
                                                                                
                                

    */

			err = setHwRegisterField(pDis,
						 E_REG_P00_VIDFORMAT_W,
						 E_MASKREG_P00_VIDFORMAT_3d_neg_vs, toggleV);
			RETIF_REG_FAIL(err);
		}
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIDFORMAT_W,
					 E_MASKREG_P00_VIDFORMAT_3d,
					 (structure3D == HDMITX_3D_FRAME_PACKING));
		RETIF_REG_FAIL(err);
	}
#endif


	if (EXTRA(reg_idx) && (structure3D != HDMITX_3D_FRAME_PACKING)) {
		/*                       */
		RETIF_REG_FAIL(set_video
			       (pDis, BASE(reg_idx), (tmHdmiTxVidReg_t *) format_param_extra));
	} else if (EXTRA(reg_idx3D) && (structure3D == HDMITX_3D_FRAME_PACKING)) {
		/*                        */
		RETIF_REG_FAIL(set_video
			       (pDis, BASE(reg_idx3D), (tmHdmiTxVidReg_t *) format_param_extra));
	} else {
		/*                                              */
	}


#ifdef FORMAT_PC

	if (IS_PC(vinFmt)) {
		RETIF_REG_FAIL(set_video(pDis, reg_idx, (tmHdmiTxVidReg_t *) format_param_PC));

		regVal = DEPTH_COLOR_PC;
		err = setHwRegisterField(pDis,
					 E_REG_P00_HVF_CNTRL_1_W,
					 E_MASKREG_P00_HVF_CNTRL_1_pad, regVal);
		RETIF_REG_FAIL(err);
	}
#endif				/*           */

	/*                     */
	err = setHwRegisterMsbLsb(pDis, E_REG_P00_REFPIX_MSB_W, uRefPix);
	RETIF_REG_FAIL(err)
	    err = setHwRegisterMsbLsb(pDis, E_REG_P00_REFLINE_MSB_W, uRefLine);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989VideoInSetSyncManual
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxSyncSource_t syncSource,
     tmbslHdmiTxVsMeth_t syncMethod,
     tmbslHdmiTxPixTogl_t toggleV,
     tmbslHdmiTxPixTogl_t toggleH, tmbslHdmiTxPixTogl_t toggleX, UInt16 uRefPix, UInt16 uRefLine) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 embedded;		/*                */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(syncSource >= HDMITX_SYNCSRC_INVALID)
	    RETIF_BADPARAM(syncMethod >= HDMITX_VSMETH_INVALID)
	    RETIF_BADPARAM(toggleV >= HDMITX_PIXTOGL_INVALID)
	    RETIF_BADPARAM(toggleH >= HDMITX_PIXTOGL_INVALID)
	    RETIF_BADPARAM(toggleX >= HDMITX_PIXTOGL_INVALID)
	    RETIF_BADPARAM(uRefPix >= HDMITX_VOUT_FINE_PIXEL_INVALID)
	    RETIF_BADPARAM(uRefLine >= HDMITX_VOUT_FINE_LINE_INVALID)

	    if (syncSource != HDMITX_SYNCSRC_NO_CHANGE) {
		if (syncSource == HDMITX_SYNCSRC_EMBEDDED) {
			embedded = 1;
		} else {
			embedded = 0;
		}
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_3_W,
					 E_MASKREG_P00_VIP_CNTRL_3_emb, embedded);
		RETIF_REG_FAIL(err)
	}
	if (syncMethod != HDMITX_VSMETH_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_TBG_CNTRL_0_W,
					 E_MASKREG_P00_TBG_CNTRL_0_sync_mthd, (UInt8) syncMethod);
		RETIF_REG_FAIL(err)
	}
/*                                                                             */
	if (toggleV != HDMITX_PIXTOGL_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_3_W,
					 E_MASKREG_P00_VIP_CNTRL_3_v_tgl, (UInt8) toggleV);
		RETIF_REG_FAIL(err)
	}
	if (toggleH != HDMITX_PIXTOGL_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_3_W,
					 E_MASKREG_P00_VIP_CNTRL_3_h_tgl, (UInt8) toggleH);
		RETIF_REG_FAIL(err)
	}
	if (toggleX != HDMITX_PIXTOGL_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_3_W,
					 E_MASKREG_P00_VIP_CNTRL_3_x_tgl, (UInt8) toggleX);
		RETIF_REG_FAIL(err)
	}
	if (uRefPix < HDMITX_VOUT_FINE_PIXEL_NO_CHANGE) {
		err = setHwRegisterMsbLsb(pDis, E_REG_P00_REFPIX_MSB_W, uRefPix);
		RETIF_REG_FAIL(err)
	}
	if (uRefLine < HDMITX_VOUT_FINE_LINE_NO_CHANGE) {
		err = setHwRegisterMsbLsb(pDis, E_REG_P00_REFLINE_MSB_W, uRefLine);
		RETIF_REG_FAIL(err)
	}

	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t tmbslTDA9989VideoOutDisable(tmUnitSelect_t txUnit, Bool bDisable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(bDisable > True)

	    /*                                                                 
                             
      */
	    err = setHwRegisterField(pDis,
				     E_REG_P00_TBG_CNTRL_0_W,
				     E_MASKREG_P00_TBG_CNTRL_0_frame_dis, (UInt8) bDisable);
	if (bDisable) {
		setState(pDis, EV_OUTDISABLE);
	}
	return err;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989VideoOutSetConfig
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxSinkType_t sinkType,
     tmbslHdmiTxVoutMode_t voutMode,
     tmbslHdmiTxVoutPrefil_t preFilter,
     tmbslHdmiTxVoutYuvBlnk_t yuvBlank, tmbslHdmiTxVoutQrange_t quantization) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 regVal;		/*                */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(sinkType >= HDMITX_SINK_INVALID)
	    RETIF_BADPARAM(voutMode >= HDMITX_VOUTMODE_INVALID)
	    RETIF_BADPARAM(preFilter >= HDMITX_VOUT_PREFIL_INVALID)
	    RETIF_BADPARAM(yuvBlank >= HDMITX_VOUT_YUV_BLNK_INVALID)
	    RETIF_BADPARAM(quantization >= HDMITX_VOUT_QRANGE_INVALID)

	    if (sinkType == HDMITX_SINK_EDID) {
		if (pDis->EdidStatus == HDMITX_EDID_NOT_READ) {
			/*                                                */
			pDis->sinkType = HDMITX_SINK_DVI;
		} else {
			/*                                                          */
			pDis->sinkType = pDis->EdidSinkType;
		}
	} else {
		/*                        */
		pDis->sinkType = sinkType;
	}

	/*                       */
	if (pDis->sinkType == HDMITX_SINK_DVI) {
		/*                     */
		err = setHwRegisterField(pDis,
					 E_REG_P11_AIP_CNTRL_0_RW,
					 E_MASKREG_P11_AIP_CNTRL_0_rst_fifo, 1);
		RETIF_REG_FAIL(err)

		    /*                             */
		    voutMode = HDMITX_VOUTMODE_RGB444;

		/*                                */
		err = setHwRegisterFieldTable(pDis, &kVoutHdcpOff[0]);
		RETIF_REG_FAIL(err);

		HDCP_F1;

		err = setHwRegisterField(pDis,
					 E_REG_P11_ENC_CNTRL_RW,
					 E_MASKREG_P11_ENC_CNTRL_ctl_code, regVal);
		RETIF_REG_FAIL(err)
	} else {
		/*                       */
		err = setHwRegisterField(pDis,
					 E_REG_P11_AIP_CNTRL_0_RW,
					 E_MASKREG_P11_AIP_CNTRL_0_rst_fifo, 0);
		RETIF_REG_FAIL(err)

		    /*                                */
		    /*                                            */
		    err = setHwRegisterFieldTable(pDis, &kVoutHdcpOn[0]);
		RETIF_REG_FAIL(err)
	}

	/*                                                            */
	if (voutMode != HDMITX_VOUTMODE_NO_CHANGE) {
		/*                                                                */
		pDis->voutMode = voutMode;
	}
	if (preFilter < HDMITX_VOUT_PREFIL_NO_CHANGE) {
#ifdef TMFL_HDCP_OPTIMIZED_POWER
		/*
                       
                              
   */
		err = setHwRegisterField(pDis, E_REG_FEAT_POWER_DOWN,
					 E_MASKREG_FEAT_POWER_DOWN_prefilt,
					 (preFilter == HDMITX_VOUT_PREFIL_OFF));
		RETIF_REG_FAIL(err);
#endif
		err = setHwRegisterField(pDis,
					 E_REG_P00_HVF_CNTRL_0_W,
					 E_MASKREG_P00_HVF_CNTRL_0_prefil, (UInt8) preFilter);
		RETIF_REG_FAIL(err)
	}
	if (yuvBlank < HDMITX_VOUT_YUV_BLNK_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_HVF_CNTRL_1_W,
					 E_MASKREG_P00_HVF_CNTRL_1_yuvblk, (UInt8) yuvBlank);
		RETIF_REG_FAIL(err)
	}
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989VideoOutSetSync
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxVsSrc_t srcH,
     tmbslHdmiTxVsSrc_t srcV,
     tmbslHdmiTxVsSrc_t srcX, tmbslHdmiTxVsTgl_t toggle, tmbslHdmiTxVsOnce_t once) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 reg_idx;
	struct sync_desc *sync;

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(srcH >= HDMITX_VSSRC_INVALID)
	    RETIF_BADPARAM(srcV >= HDMITX_VSSRC_INVALID)
	    RETIF_BADPARAM(srcX >= HDMITX_VSSRC_INVALID)
	    RETIF_BADPARAM(toggle >= HDMITX_VSTGL_INVALID)
	    RETIF_BADPARAM(once >= HDMITX_VSONCE_INVALID)

	    /*                                                            */
	    if (srcH != HDMITX_VSSRC_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_TBG_CNTRL_1_W,
					 E_MASKREG_P00_TBG_CNTRL_1_vhx_ext_hs, (UInt8) srcH);
		RETIF_REG_FAIL(err)
	}
	if (srcV != HDMITX_VSSRC_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_TBG_CNTRL_1_W,
					 E_MASKREG_P00_TBG_CNTRL_1_vhx_ext_vs, (UInt8) srcV);
		RETIF_REG_FAIL(err)
	}
	if (srcX != HDMITX_VSSRC_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_TBG_CNTRL_1_W,
					 E_MASKREG_P00_TBG_CNTRL_1_vhx_ext_de, (UInt8) srcX);
		RETIF_REG_FAIL(err)
	}
	{
		/*                    */
		/*                                                                            */
		RETIF_BADPARAM(reg_vid_fmt(pDis->vinFmt, 0, &reg_idx, 0, &sync));
		if (EXTRA(reg_idx)) {
			toggle = E_MASKREG_P00_TBG_CNTRL_1_vh_tgl &
			    (0x04 | sync[BASE(reg_idx)].v_toggle | sync[BASE(reg_idx)].h_toggle);
		}
	}


	if (toggle != HDMITX_VSTGL_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_TBG_CNTRL_1_W,
					 E_MASKREG_P00_TBG_CNTRL_1_vh_tgl, (UInt8) toggle);
/*                                                   */
		RETIF_REG_FAIL(err)
	}
	if (once != HDMITX_VSONCE_NO_CHANGE) {
		/*                           */
		err = setHwRegisterField(pDis,
					 E_REG_P00_TBG_CNTRL_0_W,
					 E_MASKREG_P00_TBG_CNTRL_0_sync_once, (UInt8) once);
		RETIF_REG_FAIL(err)
	}

	/*                                                    */
	err = setHwRegisterField(pDis,
				 E_REG_P02_BUFFER_OUT_RW,
				 E_MASKREG_P02_BUFFER_OUT_srl_force,
				 (UInt8) HDMITX_TMDSOUT_FORCED0);
	RETIF_REG_FAIL(err)
	    err = setHwRegisterField(pDis,
				     E_REG_P02_BUFFER_OUT_RW,
				     E_MASKREG_P02_BUFFER_OUT_srl_force,
				     (UInt8) HDMITX_TMDSOUT_NORMAL);
	RETIF_REG_FAIL(err)


	    if (once == HDMITX_VSONCE_ONCE) {
		/*                                                          */
		err = setHwRegisterField(pDis,
					 E_REG_P00_TBG_CNTRL_0_W,
					 E_MASKREG_P00_TBG_CNTRL_0_sync_once,
					 (UInt8) HDMITX_VSONCE_EACH_FRAME);
		RETIF_REG_FAIL(err)
		    err = setHwRegisterField(pDis,
					     E_REG_P00_TBG_CNTRL_0_W,
					     E_MASKREG_P00_TBG_CNTRL_0_sync_once,
					     (UInt8) HDMITX_VSONCE_ONCE);
		RETIF_REG_FAIL(err)
	}
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989VideoSetInOut
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxVidFmt_t vinFmt,
     tmbslHdmiTx3DStructure_t structure3D,
     tmbslHdmiTxScaMode_t scaModeRequest,
     tmbslHdmiTxVidFmt_t voutFmt,
     UInt8 uPixelRepeat,
     tmbslHdmiTxMatMode_t matMode, tmbslHdmiTxVoutDbits_t datapathBits, tmbslHdmiTxVQR_t dviVqr) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	tmbslHdmiTxScaMode_t scaMode;	/*             */
	UInt8 reg_idx, reg_idx3D;	/*                                          */
	UInt8 regVal;

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                  */
	    RETIF_BADPARAM(!IS_VALID_FMT(vinFmt))
	    RETIF_BADPARAM(!IS_VALID_FMT(voutFmt))

	    RETIF_BADPARAM(scaModeRequest >= HDMITX_SCAMODE_INVALID)
	    RETIF_BADPARAM(uPixelRepeat >= HDMITX_PIXREP_INVALID)
	    RETIF_BADPARAM(matMode >= HDMITX_MATMODE_INVALID)
	    RETIF_BADPARAM(datapathBits >= HDMITX_VOUT_DBITS_INVALID)

	    scaMode = HDMITX_SCAMODE_OFF;
	pDis->scaMode = HDMITX_SCAMODE_OFF;

	/*                                                */
	if (vinFmt == HDMITX_VFMT_NO_CHANGE) {
		RETIF(pDis->vinFmt == HDMITX_VFMT_NULL, TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)
		    vinFmt = pDis->vinFmt;
	} else {
		pDis->vinFmt = vinFmt;
		pDis->h3dFpOn = (structure3D == HDMITX_3D_FRAME_PACKING);

#ifdef TMFL_TDA9989_PIXEL_CLOCK_ON_DDC

		if (IS_TV(pDis->vinFmt)) {

			err = setHwRegister(pDis, E_REG_P00_TIMER_H_W, 0);
			RETIF(err != TM_OK, err);

			err = setHwRegister(pDis, E_REG_P00_NDIV_IM_W, kndiv_im[vinFmt]);
			RETIF(err != TM_OK, err);

			err = setHwRegister(pDis, E_REG_P12_TX3_RW, kclk_div[vinFmt]);
			RETIF(err != TM_OK, err);

		} else {

			err =
			    setHwRegister(pDis, E_REG_P00_TIMER_H_W,
					  E_MASKREG_P00_TIMER_H_im_clksel);
			RETIF(err != TM_OK, err);
			err = setHwRegister(pDis, E_REG_P12_TX3_RW, 17);
			RETIF(err != TM_OK, err);
		}
#endif				/*                                 */


	}

	/*                                                 */
	if (voutFmt == HDMITX_VFMT_NO_CHANGE) {
		RETIF(pDis->voutFmt == HDMITX_VFMT_NULL, TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)
		    voutFmt = pDis->voutFmt;
	} else {
		pDis->voutFmt = voutFmt;
	}
	if (pDis->voutMode == HDMITX_VOUTMODE_RGB444) {
		if ((pDis->voutFmt >= HDMITX_VFMT_02_720x480p_60Hz) && (IS_TV(pDis->voutFmt))) {
			err = setHwRegisterField(pDis,
						 E_REG_P00_HVF_CNTRL_1_W,
						 E_MASKREG_P00_HVF_CNTRL_1_vqr,
						 (UInt8) HDMITX_VOUT_QRANGE_RGB_YUV);
			RETIF_REG_FAIL(err)
		} else {	/*                 */

			err = setHwRegisterField(pDis,
						 E_REG_P00_HVF_CNTRL_1_W,
						 E_MASKREG_P00_HVF_CNTRL_1_vqr,
						 (UInt8) HDMITX_VOUT_QRANGE_FS);
			RETIF_REG_FAIL(err)
		}
	} else {
		err = setHwRegisterField(pDis,
					 E_REG_P00_HVF_CNTRL_1_W,
					 E_MASKREG_P00_HVF_CNTRL_1_vqr,
					 (UInt8) HDMITX_VOUT_QRANGE_YUV);
		RETIF_REG_FAIL(err);
	}

	/*                                                                       */
	err = setPixelRepeat(pDis, voutFmt, uPixelRepeat, structure3D);
	RETIF(err != TM_OK, err);

	/*                                                                       */
	if (scaMode != HDMITX_SCAMODE_NO_CHANGE) {
		/*                  */
		regVal = 0;
		if ((pDis->pixelRepeatCount > HDMITX_PIXREP_MIN) &&
		    (pDis->pixelRepeatCount <= HDMITX_PIXREP_MAX)) {
			regVal = 2;
		} else if (pDis->vinMode == HDMITX_VINMODE_CCIR656) {
			regVal = (UInt8) ((pDis->scaMode == HDMITX_SCAMODE_ON) ? 0 : 1);

			if (pDis->pixRate == HDMITX_PIXRATE_DOUBLE) {
				regVal = 0;
			}
		}

		err = setHwRegisterField(pDis,
					 E_REG_P02_SEL_CLK_RW,
					 E_MASKREG_P02_SEL_CLK_sel_vrf_clk, regVal);
		RETIF_REG_FAIL(err);

		/*                                                                      */
		RETIF_BADPARAM(reg_vid_fmt(vinFmt, structure3D, &reg_idx, &reg_idx3D, 0));

		/*                                                            */
		if (PREFETCH(reg_idx3D) && (structure3D == HDMITX_3D_FRAME_PACKING)) {
			/*                          */
			err = setHwRegister(pDis, E_REG_P00_VIDFORMAT_W, reg_idx3D);
		} else if (PREFETCH(reg_idx) && (structure3D != HDMITX_3D_FRAME_PACKING)) {
			/*                          */
/*                                                                        */
			err = setHwRegister(pDis, E_REG_P00_VIDFORMAT_W, reg_idx);
		} else {
			/*                                    */
		}
		RETIF_REG_FAIL(err);

	}

	/*                        */
	err = setDeVs(pDis, voutFmt, structure3D);
	RETIF(err != TM_OK, err);

	/*                                                                       */
	if (matMode != HDMITX_MATMODE_NO_CHANGE) {
		if (matMode == HDMITX_MATMODE_AUTO) {
			err = tmbslTDA9989MatrixSetConversion(txUnit, vinFmt,
							      pDis->vinMode, voutFmt,
							      pDis->voutMode, pDis->dviVqr);
		} else {
			err = tmbslTDA9989MatrixSetMode(txUnit, HDMITX_MCNTRL_OFF,
							HDMITX_MSCALE_NO_CHANGE);
		}
		RETIF(err != TM_OK, err)
	}

	/*                               */
	err = setSampling(pDis);
	RETIF(err != TM_OK, err)

	    /*                                */
	    if (datapathBits != HDMITX_VOUT_DBITS_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_HVF_CNTRL_1_W,
					 E_MASKREG_P00_HVF_CNTRL_1_pad, (UInt8) datapathBits);
		RETIF_REG_FAIL(err)
	}

	/*                                                         */
	getHwRegister(pDis, E_REG_P00_MAT_CONTRL_W, &gMatContrl[txUnit]);
	getHwRegister(pDis, E_REG_P00_HVF_CNTRL_0_W, &gHvfCntrl0[txUnit]);
	getHwRegister(pDis, E_REG_P00_HVF_CNTRL_1_W, &gHvfCntrl1[txUnit]);

	setState(pDis, EV_SETINOUT);
	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t tmbslTDA9989MatrixSetCoeffs(tmUnitSelect_t txUnit, tmbslHdmiTxMatCoeff_t *pMatCoeff) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	Int i;			/*            */
	UInt8 buf[HDMITX_MAT_COEFF_NUM * 2];	/*             */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pMatCoeff == (tmbslHdmiTxMatCoeff_t *) 0)
	    for (i = 0; i < HDMITX_MAT_COEFF_NUM; i++) {
		RETIF_BADPARAM((pMatCoeff->Coeff[i] < HDMITX_MAT_OFFSET_MIN) ||
			       (pMatCoeff->Coeff[i] > HDMITX_MAT_OFFSET_MAX))
	}

	/*                                                                  
                                                
  */
	for (i = 0; i < HDMITX_MAT_COEFF_NUM; i++) {
		/*                 */
		buf[i * 2] = (UInt8) (((UInt16) pMatCoeff->Coeff[i] & 0x0700) >> 8);
		/*          */
		buf[(i * 2) + 1] = (UInt8) ((UInt16) pMatCoeff->Coeff[i] & 0x00FF);
	}
	err = setHwRegisters(pDis, E_REG_P00_MAT_P11_MSB_W, &buf[0], HDMITX_MAT_COEFF_NUM * 2);
	return err;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989MatrixSetConversion
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxVidFmt_t vinFmt,
     tmbslHdmiTxVinMode_t vinMode,
     tmbslHdmiTxVidFmt_t voutFmt, tmbslHdmiTxVoutMode_t voutMode, tmbslHdmiTxVQR_t dviVqr) {
	tmHdmiTxobject_t *pDis;	/*                                  */
	tmErrorCode_t err;	/*            */
	tmbslTDA9989Colourspace_t cspace_in;	/*                    */
	tmbslTDA9989Colourspace_t cspace_out;	/*                     */
	Int matrixIndex;	/*                                */
	UInt8 buf[MATRIX_PRESET_SIZE];	/*             */
	UInt8 i;		/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(!IS_VALID_FMT(vinFmt))
	    RETIF_BADPARAM(!IS_VALID_FMT(voutFmt))
	    /*                                                                         */
	    RETIF_BADPARAM(vinMode >= HDMITX_VINMODE_NO_CHANGE)
	    /*                                                                         */
	    RETIF_BADPARAM(voutMode >= HDMITX_VOUTMODE_NO_CHANGE)

	    /*                                                                     
                                                      */
	    /*                              */
	    switch (vinFmt) {	/*                    */
	case HDMITX_VFMT_04_1280x720p_60Hz:
	case HDMITX_VFMT_05_1920x1080i_60Hz:
	case HDMITX_VFMT_16_1920x1080p_60Hz:
	case HDMITX_VFMT_19_1280x720p_50Hz:
	case HDMITX_VFMT_20_1920x1080i_50Hz:
	case HDMITX_VFMT_31_1920x1080p_50Hz:
	case HDMITX_VFMT_32_1920x1080p_24Hz:
	case HDMITX_VFMT_33_1920x1080p_25Hz:
	case HDMITX_VFMT_34_1920x1080p_30Hz:
	case HDMITX_VFMT_60_1280x720p_24Hz:
	case HDMITX_VFMT_61_1280x720p_25Hz:
	case HDMITX_VFMT_62_1280x720p_30Hz:

		if (vinMode == HDMITX_VINMODE_RGB444) {	/*     */
			cspace_in = HDMITX_CS_RGB_LIMITED;
		} else {	/*                        */

			cspace_in = HDMITX_CS_YUV_ITU_BT709;
		}
		break;
	default:		/*                      */
		if (vinMode == HDMITX_VINMODE_RGB444) {	/*           */
			cspace_in = HDMITX_CS_RGB_LIMITED;
		} else {	/*                        */

			cspace_in = HDMITX_CS_YUV_ITU_BT601;
		}
		break;
	}

/*       */

	/*                               */
#ifdef FORMAT_PC
	if (IS_PC(voutFmt)) {
		/*                      */
		cspace_in = HDMITX_CS_RGB_FULL;	/*              */
		cspace_out = HDMITX_CS_RGB_FULL;
	} else {
#endif
		switch (voutFmt) {	/*                    */
		case HDMITX_VFMT_04_1280x720p_60Hz:
		case HDMITX_VFMT_05_1920x1080i_60Hz:
		case HDMITX_VFMT_16_1920x1080p_60Hz:
		case HDMITX_VFMT_19_1280x720p_50Hz:
		case HDMITX_VFMT_20_1920x1080i_50Hz:
		case HDMITX_VFMT_31_1920x1080p_50Hz:
		case HDMITX_VFMT_32_1920x1080p_24Hz:
		case HDMITX_VFMT_33_1920x1080p_25Hz:
		case HDMITX_VFMT_34_1920x1080p_30Hz:
		case HDMITX_VFMT_60_1280x720p_24Hz:
		case HDMITX_VFMT_61_1280x720p_25Hz:
		case HDMITX_VFMT_62_1280x720p_30Hz:

			if (voutMode == HDMITX_VOUTMODE_RGB444) {	/*     */
				cspace_out = HDMITX_CS_RGB_LIMITED;
			} else {	/*                  */

				cspace_out = HDMITX_CS_YUV_ITU_BT709;
			}
			break;
		default:	/*                      */
			if (voutMode == HDMITX_VOUTMODE_RGB444) {	/*     */
				cspace_out = HDMITX_CS_RGB_LIMITED;
			} else {	/*                  */

				cspace_out = HDMITX_CS_YUV_ITU_BT601;
			}
			break;
		}
#ifdef FORMAT_PC
	}
#endif

#ifdef TMFL_HDCP_OPTIMIZED_POWER
	/*
                      
                                              
  */
	err = setHwRegisterField(pDis, E_REG_FEAT_POWER_DOWN,
				 E_MASKREG_FEAT_POWER_DOWN_csc, (cspace_in == cspace_out));
	RETIF_REG_FAIL(err);
#endif

	if (cspace_in == cspace_out) {
		/*                                                 */
		err = setHwRegisterField(pDis,
					 E_REG_P00_MAT_CONTRL_W,
					 E_MASKREG_P00_MAT_CONTRL_mat_bp, 1);
	} else {
		/*                                                                
                                                                         
   */
		matrixIndex = kMatrixIndex[cspace_in][cspace_out];

		/*                                                                 
                              */
		err = setHwRegister(pDis, E_REG_P00_MAT_CONTRL_W, kMatrixPreset[matrixIndex][0]);
		RETIF_REG_FAIL(err)

		    for (i = 0; i < MATRIX_PRESET_SIZE; i++) {
			buf[i] = kMatrixPreset[matrixIndex][i];
		}

		/*                           */
		err = setHwRegisters(pDis,
				     E_REG_P00_MAT_OI1_MSB_W, &buf[1], MATRIX_PRESET_SIZE - 1);
	}
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t
    tmbslTDA9989MatrixSetInputOffset(tmUnitSelect_t txUnit, tmbslHdmiTxMatOffset_t *pMatOffset) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	Int i;			/*            */
	UInt8 buf[HDMITX_MAT_OFFSET_NUM * 2];	/*             */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pMatOffset == (tmbslHdmiTxMatOffset_t *) 0)
	    for (i = 0; i < HDMITX_MAT_OFFSET_NUM; i++) {
		RETIF_BADPARAM((pMatOffset->Offset[i] < HDMITX_MAT_OFFSET_MIN) ||
			       (pMatOffset->Offset[i] > HDMITX_MAT_OFFSET_MAX))
	}

	/*                                                                   
                                                
  */
	for (i = 0; i < HDMITX_MAT_OFFSET_NUM; i++) {
		/*                 */
		buf[i * 2] = (UInt8) (((UInt16) pMatOffset->Offset[i] & 0x0700) >> 8);
		/*          */
		buf[(i * 2) + 1] = (UInt8) ((UInt16) pMatOffset->Offset[i] & 0x00FF);
	}
	err = setHwRegisters(pDis, E_REG_P00_MAT_OI1_MSB_W, &buf[0], HDMITX_MAT_OFFSET_NUM * 2);
	return err;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989MatrixSetMode
    (tmUnitSelect_t txUnit, tmbslHdmiTxmCntrl_t mControl, tmbslHdmiTxmScale_t mScale) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM((mControl > HDMITX_MCNTRL_MAX) || (mScale > HDMITX_MSCALE_MAX))

	    /*                                                                      */
	    if (mControl != HDMITX_MCNTRL_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_MAT_CONTRL_W,
					 E_MASKREG_P00_MAT_CONTRL_mat_bp, (UInt8) mControl);
		RETIF_REG_FAIL(err)
	}

	if (mScale != HDMITX_MSCALE_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_MAT_CONTRL_W,
					 E_MASKREG_P00_MAT_CONTRL_mat_sc, (UInt8) mScale);
		RETIF_REG_FAIL(err)
	}

	return TM_OK;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t
    tmbslTDA9989MatrixSetOutputOffset(tmUnitSelect_t txUnit, tmbslHdmiTxMatOffset_t *pMatOffset) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	Int i;			/*            */
	UInt8 buf[HDMITX_MAT_OFFSET_NUM * 2];	/*             */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                              */
	    RETIF_BADPARAM(pMatOffset == (tmbslHdmiTxMatOffset_t *) 0)
	    for (i = 0; i < HDMITX_MAT_OFFSET_NUM; i++) {
		RETIF_BADPARAM((pMatOffset->Offset[i] < HDMITX_MAT_OFFSET_MIN) ||
			       (pMatOffset->Offset[i] > HDMITX_MAT_OFFSET_MAX))
	}

	/*                                                                   
                                                
  */
	for (i = 0; i < HDMITX_MAT_OFFSET_NUM; i++) {
		/*                 */
		buf[i * 2] = (UInt8) (((UInt16) pMatOffset->Offset[i] & 0x0700) >> 8);
		/*          */
		buf[(i * 2) + 1] = (UInt8) ((UInt16) pMatOffset->Offset[i] & 0x00FF);
	}
	err = setHwRegisters(pDis, E_REG_P00_MAT_OO1_MSB_W, &buf[0], HDMITX_MAT_OFFSET_NUM * 2);
	return err;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t tmbslTDA9989PktSetAclkRecovery(tmUnitSelect_t txUnit, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                              */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    /*                                     */
	    err = setHwRegisterField(pDis,
				     E_REG_P11_DIP_FLAGS_RW,
				     E_MASKREG_P11_DIP_FLAGS_acr, (UInt8) bEnable);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989PktSetAcp
    (tmUnitSelect_t txUnit, tmbslHdmiTxPkt_t *pPkt, UInt byteCnt, UInt8 uAcpType, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[3];		/*                                  */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                       */
	    /*                                                     */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    if (pPkt != Null) {
		/*                                                   */
		RETIF_BADPARAM(byteCnt > HDMITX_PKT_DATA_BYTE_CNT)
		    RETIF(byteCnt == 0, TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)

		    /*                                                             */
		    err = setHwRegisterField(pDis,
					     E_REG_P11_DIP_FLAGS_RW,
					     E_MASKREG_P11_DIP_FLAGS_acp, 0x00);
		RETIF_REG_FAIL(err)

		    /*                    */
		    buf[0] = 0x04;	/*            */
		buf[1] = uAcpType;
		buf[2] = 0;	/*                     */


		/*                                   */
		err = setHwRegisters(pDis, E_REG_P11_ACP_HB0_RW, &buf[0], 3);
		RETIF_REG_FAIL(err)

		    /*                                            */
		    err = setHwRegisters(pDis,
					 E_REG_P11_ACP_PB0_RW,
					 &pPkt->dataByte[0], (UInt16) byteCnt);
		RETIF_REG_FAIL(err)
	}

	/*                                     */
	err = setHwRegisterField(pDis,
				 E_REG_P11_DIP_FLAGS_RW,
				 E_MASKREG_P11_DIP_FLAGS_acp, (UInt8) bEnable);
	return err;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989PktSetAudioInfoframe
    (tmUnitSelect_t txUnit, tmbslHdmiTxPktAif_t *pPkt, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[9];		/*                                         */
	UInt16 bufReg;		/*                                          */
	UInt16 flagReg;		/*                          */
	UInt8 flagMask;		/*                                     */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                                     */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))
	    if (pPkt != Null) {
		/*                                                   */
		RETIF_BADPARAM(pPkt->CodingType > 0x0F)
		    RETIF_BADPARAM(pPkt->ChannelCount > 0x07)
		    RETIF_BADPARAM(pPkt->SampleFreq > 0x07)
		    RETIF_BADPARAM(pPkt->SampleSize > 0x03)
		    /*                                                        */
		    RETIF_BADPARAM((pPkt->DownMixInhibit != True) &&
				   (pPkt->DownMixInhibit != False))
		    RETIF_BADPARAM(pPkt->LevelShift > 0x0F)
	}

	/*                                       */

	/*                                                             */
	bufReg = E_REG_P10_IF4_HB0_RW;
	flagReg = E_REG_P11_DIP_IF_FLAGS_RW;
	flagMask = E_MASKREG_P11_DIP_IF_FLAGS_if4;

	if (pPkt != Null) {
		/*                                                             */
		err = setHwRegisterField(pDis, flagReg, flagMask, 0x00);
		RETIF_REG_FAIL(err)

		    /*                    */
		    buf[0] = 0x84;	/*                 */
		buf[1] = 0x01;	/*                      */
		buf[2] = 0x0A;	/*                   */

		/*                                               */
		buf[0 + 3] = 0;	/*                                               */
		buf[1 + 3] = ((pPkt->CodingType & 0x0F) << 4) | (pPkt->ChannelCount & 0x07);	/*              */
		buf[2 + 3] = ((pPkt->SampleFreq & 0x07) << 2) | (pPkt->SampleSize & 0x03);	/*              */
		buf[3 + 3] = 0;	/*            */
		buf[4 + 3] = pPkt->ChannelAlloc;	/*       */
		buf[5 + 3] = ((pPkt->LevelShift & 0x0F) << 3);	/*       */
		if (pPkt->DownMixInhibit == True) {
			buf[5 + 3] += 0x80;	/*         */
		}

		/*                                                                 
                                                                     
                                                          
                                          
   */
		buf[0 + 3] = calculateChecksum(&buf[0], 0x0A + 1 + 3 - 5);

		/*                                                */
		err = setHwRegisters(pDis, bufReg, &buf[0], 9);
		RETIF_REG_FAIL(err)
	}

	/*                                 */
	err = setHwRegisterField(pDis, flagReg, flagMask, (UInt8) bEnable);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t
    tmbslTDA9989PktSetGeneralCntrl
    (tmUnitSelect_t txUnit, tmbslHdmiTxaMute_t *paMute, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                                     */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    if (paMute != Null) {
		RETIF_BADPARAM((*paMute != HDMITX_AMUTE_OFF) && (*paMute != HDMITX_AMUTE_ON))

		    if (*paMute == HDMITX_AMUTE_ON) {
			err = setHwRegister(pDis, E_REG_P11_GC_AVMUTE_RW, 0x02);
			RETIF_REG_FAIL(err)
		} else {
			err = setHwRegister(pDis, E_REG_P11_GC_AVMUTE_RW, 0x01);
			RETIF_REG_FAIL(err)
		}
	}

	/*                                       */
	err = setHwRegisterField(pDis,
				 E_REG_P11_DIP_FLAGS_RW,
				 E_MASKREG_P11_DIP_FLAGS_gc, (UInt8) bEnable);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989PktSetIsrc1
    (tmUnitSelect_t txUnit,
     tmbslHdmiTxPkt_t *pPkt,
     UInt byteCnt, Bool bIsrcCont, Bool bIsrcValid, UInt8 uIsrcStatus, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[3];		/*                                  */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                       */
	    /*                                                     */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    if (pPkt != Null) {
		/*                                                   */
		RETIF_BADPARAM((bIsrcCont != True) && (bIsrcCont != False))
		    RETIF_BADPARAM((bIsrcValid != True) && (bIsrcValid != False))
		    RETIF_BADPARAM(uIsrcStatus > 7)	/*        */
		    RETIF_BADPARAM(byteCnt > HDMITX_PKT_DATA_BYTE_CNT)
		    RETIF(byteCnt == 0, TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)

		    /*                                                               */
		    err = setHwRegisterField(pDis,
					     E_REG_P11_DIP_FLAGS_RW,
					     E_MASKREG_P11_DIP_FLAGS_isrc1, 0x00);
		RETIF_REG_FAIL(err)

		    /*                      */
		    buf[0] = 0x05;	/*              */
		buf[1] = (uIsrcStatus & 0x07);
		if (bIsrcValid == True) {
			buf[1] += 0x40;
		}
		if (bIsrcCont == True) {
			buf[1] += 0x80;
		}
		buf[2] = 0;	/*                     */

		/*                                   */
		err = setHwRegisters(pDis, E_REG_P11_ISRC1_HB0_RW, &buf[0], 3);
		RETIF_REG_FAIL(err)

		    /*                                            */
		    err = setHwRegisters(pDis,
					 E_REG_P11_ISRC1_PB0_RW,
					 &pPkt->dataByte[0], (UInt16) byteCnt);
		RETIF_REG_FAIL(err)
	}

	/*                                       */
	err = setHwRegisterField(pDis,
				 E_REG_P11_DIP_FLAGS_RW,
				 E_MASKREG_P11_DIP_FLAGS_isrc1, (UInt8) bEnable);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989PktSetIsrc2
    (tmUnitSelect_t txUnit, tmbslHdmiTxPkt_t *pPkt, UInt byteCnt, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[3];		/*                                  */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                       */
	    /*                                                     */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    if (pPkt != Null) {
		/*                                                   */
		RETIF_BADPARAM(byteCnt > HDMITX_PKT_DATA_BYTE_CNT)
		    RETIF(byteCnt == 0, TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)

		    /*                                                               */
		    err = setHwRegisterField(pDis,
					     E_REG_P11_DIP_FLAGS_RW,
					     E_MASKREG_P11_DIP_FLAGS_isrc2, 0x00);
		RETIF_REG_FAIL(err)

		    /*                      */
		    buf[0] = 0x06;	/*              */
		buf[1] = 0;	/*                     */
		buf[2] = 0;	/*                     */

		/*                                   */
		err = setHwRegisters(pDis, E_REG_P11_ISRC2_HB0_RW, &buf[0], 3);
		RETIF_REG_FAIL(err)

		    /*                                            */
		    err = setHwRegisters(pDis,
					 E_REG_P11_ISRC2_PB0_RW,
					 &pPkt->dataByte[0], (UInt16) byteCnt);
		RETIF_REG_FAIL(err)
	}

	/*                                       */
	err = setHwRegisterField(pDis,
				 E_REG_P11_DIP_FLAGS_RW,
				 E_MASKREG_P11_DIP_FLAGS_isrc2, (UInt8) bEnable);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989PktSetMpegInfoframe
    (tmUnitSelect_t txUnit, tmbslHdmiTxPktMpeg_t *pPkt, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[9];		/*                            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                       */
	    /*                              */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    if (pPkt != Null) {
		/*                                                   */
		RETIF_BADPARAM((pPkt->bFieldRepeat != True) && (pPkt->bFieldRepeat != False))
		    RETIF_BADPARAM(pPkt->frameType >= HDMITX_MPEG_FRAME_INVALID)

		    /*                                                              */
		    err = setHwRegisterField(pDis,
					     E_REG_P11_DIP_IF_FLAGS_RW,
					     E_MASKREG_P11_DIP_IF_FLAGS_if5, 0x00);
		RETIF_REG_FAIL(err)

		    /*                     */
		    buf[0] = 0x85;	/*                       */
		buf[1] = 0x01;	/*                      */
		buf[2] = 0x0A;	/*                   */

		/*                                                */
		buf[0 + 3] = 0;	/*                                               */
		buf[1 + 3] = (UInt8) (pPkt->bitRate & 0x000000FF);
		buf[2 + 3] = (UInt8) ((pPkt->bitRate & 0x0000FF00) >> 8);
		buf[3 + 3] = (UInt8) ((pPkt->bitRate & 0x00FF0000) >> 16);
		buf[4 + 3] = (UInt8) ((pPkt->bitRate & 0xFF000000) >> 24);
		buf[5 + 3] = pPkt->frameType;	/*       */
		if (pPkt->bFieldRepeat == True) {
			buf[5 + 3] += 0x10;	/*         */
		}

		/*                                                                 
                                                                     
                                                          
                                          
   */
		buf[0 + 3] = calculateChecksum(&buf[0], 0x0A + 1 + 3 - 5);

		/*                                                */
		err = setHwRegisters(pDis, E_REG_P10_IF5_HB0_RW, &buf[0], 9);
		RETIF_REG_FAIL(err)
	}

	/*                                      */
	err = setHwRegisterField(pDis,
				 E_REG_P11_DIP_IF_FLAGS_RW,
				 E_MASKREG_P11_DIP_IF_FLAGS_if5, (UInt8) bEnable);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989PktSetNullInsert(tmUnitSelect_t txUnit, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                              */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    /*                                               */
	    err = setHwRegisterField(pDis,
				     E_REG_P11_DIP_FLAGS_RW,
				     E_MASKREG_P11_DIP_FLAGS_force_null, (UInt8) bEnable);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t tmbslTDA9989PktSetNullSingle(tmUnitSelect_t txUnit) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                */
	    err = setHwRegisterField(pDis,
				     E_REG_P11_DIP_FLAGS_RW, E_MASKREG_P11_DIP_FLAGS_null, 0x01);
	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989PktSetSpdInfoframe
    (tmUnitSelect_t txUnit, tmbslHdmiTxPktSpd_t *pPkt, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[29];		/*                            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                       */
	    /*                              */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    if (pPkt != Null) {
		/*                                                   */
		RETIF_BADPARAM(pPkt->SourceDevInfo >= HDMITX_SPD_INFO_INVALID)

		    /*                                                             */
		    err = setHwRegisterField(pDis,
					     E_REG_P11_DIP_IF_FLAGS_RW,
					     E_MASKREG_P11_DIP_IF_FLAGS_if3, 0x00);
		RETIF_REG_FAIL(err)

		    /*                    */
		    buf[0] = 0x83;	/*                                      */
		buf[1] = 0x01;	/*                      */
		buf[2] = 0x19;	/*                   */

		/*                                               */
		buf[0 + 3] = 0;	/*                                               */
		lmemcpy(&buf[1 + 3], &pPkt->VendorName[0], HDMI_TX_SPD_VENDOR_SIZE);
		lmemcpy(&buf[1 + 3 + HDMI_TX_SPD_VENDOR_SIZE], &pPkt->ProdDescr[0],
			HDMI_TX_SPD_DESCR_SIZE);


		buf[HDMI_TX_SPD_LENGTH + 3] = pPkt->SourceDevInfo;

		/*                                                                 
                                                                     
                  
   */
		buf[0 + 3] = calculateChecksum(&buf[0], HDMI_TX_SPD_LENGTH + 1 + 3);

		/*                                                */
		err = setHwRegisters(pDis, E_REG_P10_IF3_HB0_RW, &buf[0], 29);
		RETIF_REG_FAIL(err)
	}

	/*                                     */
	err = setHwRegisterField(pDis,
				 E_REG_P11_DIP_IF_FLAGS_RW,
				 E_MASKREG_P11_DIP_IF_FLAGS_if3, (UInt8) bEnable);
	return err;
}


/*                                                                            */
/*                                                                             */
/*                                                                            */
tmErrorCode_t
    tmbslTDA9989PktSetVideoInfoframe
    (tmUnitSelect_t txUnit, tmbslHdmiTxPktVif_t *pPkt, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[17];		/*                                         */
	UInt16 bufReg;		/*                                          */
	UInt16 flagReg;		/*                          */
	UInt8 flagMask;		/*                                     */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                                     */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))
	    if (pPkt != Null) {
		/*                                                   */
		RETIF_BADPARAM(pPkt->Colour > 0x03)
		    RETIF_BADPARAM((pPkt->ActiveInfo != True) && (pPkt->ActiveInfo != False))
		    RETIF_BADPARAM(pPkt->BarInfo > 0x03)
		    RETIF_BADPARAM(pPkt->ScanInfo > 0x03)
		    RETIF_BADPARAM(pPkt->Colorimetry > 0x03)
		    RETIF_BADPARAM(pPkt->PictureAspectRatio > 0x03)
		    RETIF_BADPARAM(pPkt->ActiveFormatRatio > 0x0F)
		    RETIF_BADPARAM(pPkt->Scaling > 0x03)
		    RETIF_BADPARAM(pPkt->VidFormat > 0x7F)
		    RETIF_BADPARAM(pPkt->PixelRepeat > 0x0F)
	}

	/*                                       */

	/*                                                             */
	bufReg = E_REG_P10_IF2_HB0_RW;
	flagReg = E_REG_P11_DIP_IF_FLAGS_RW;
	flagMask = E_MASKREG_P11_DIP_IF_FLAGS_if2;

	if (pPkt != Null) {
		/*                                                             */
		err = setHwRegisterField(pDis, flagReg, flagMask, 0x00);
		RETIF_REG_FAIL(err)

		    /*                    */
		    buf[0] = 0x82;	/*                 */
		buf[1] = 0x02;	/*                      */
		buf[2] = 0x0D;	/*                   */

		/*                                               */
		buf[0 + 3] = 0;	/*                                               */
		buf[1 + 3] = ((pPkt->Colour & 0x03) << 5) |	/*                 */
		    ((pPkt->BarInfo & 0x03) << 2) | (pPkt->ScanInfo & 0x03);
		if (pPkt->ActiveInfo == True) {
			buf[1 + 3] += 0x10;	/*        */
		}
		buf[2 + 3] = ((pPkt->Colorimetry & 0x03) << 6) |	/*                  */
		    ((pPkt->PictureAspectRatio & 0x03) << 4) | (pPkt->ActiveFormatRatio & 0x0F);
		buf[3 + 3] = (pPkt->Scaling & 0x03);	/*       *//*            */
		buf[4 + 3] = (pPkt->VidFormat & 0x7F);	/*        */
		buf[5 + 3] = (pPkt->PixelRepeat & 0x0F);	/*       */
		buf[6 + 3] = (UInt8) (pPkt->EndTopBarLine & 0x00FF);
		buf[7 + 3] = (UInt8) ((pPkt->EndTopBarLine & 0xFF00) >> 8);
		buf[8 + 3] = (UInt8) (pPkt->StartBottomBarLine & 0x00FF);
		buf[9 + 3] = (UInt8) ((pPkt->StartBottomBarLine & 0xFF00) >> 8);
		buf[10 + 3] = (UInt8) (pPkt->EndLeftBarPixel & 0x00FF);
		buf[11 + 3] = (UInt8) ((pPkt->EndLeftBarPixel & 0xFF00) >> 8);
		buf[12 + 3] = (UInt8) (pPkt->StartRightBarPixel & 0x00FF);
		buf[13 + 3] = (UInt8) ((pPkt->StartRightBarPixel & 0xFF00) >> 8);

		/*                                                                 
                                                                     
                  
   */
		buf[0 + 3] = calculateChecksum(&buf[0], 0x0D + 1 + 3);

		/*                                                */
		err = setHwRegisters(pDis, bufReg, &buf[0], 17);
		RETIF_REG_FAIL(err)
	}

	/*                                 */
	err = setHwRegisterField(pDis, flagReg, flagMask, (UInt8) bEnable);
	return err;
}

/*                                                                            */
/*                                                                           */
/*                                                                            */
tmErrorCode_t tmbslTDA9989PktSetRawVideoInfoframe
    (tmUnitSelect_t txUnit, tmbslHdmiTxPktRawAvi_t *pPkt, Bool bEnable) {


	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                                     */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    /*                */
	    if (pPkt != Null) {
		/*                                                             */
		err = setHwRegisterField(pDis,
					 E_REG_P11_DIP_IF_FLAGS_RW,
					 E_MASKREG_P11_DIP_IF_FLAGS_if2, 0x00);
		RETIF_REG_FAIL(err)


		    /*                                */
		    err = setHwRegisters(pDis, E_REG_P10_IF2_HB0_RW, pPkt->HB, 3);
		RETIF_REG_FAIL(err)

		    /*                                  */
		    err = setHwRegisters(pDis, E_REG_P10_IF2_PB0_RW, pPkt->PB, 28);

		RETIF_REG_FAIL(err)

	}

	/*                                 */
	err = setHwRegisterField(pDis,
				 E_REG_P11_DIP_IF_FLAGS_RW,
				 E_MASKREG_P11_DIP_IF_FLAGS_if2, (UInt8) bEnable);

	return err;
}

/*                                                                            */
/*                                                                             */
/*                                                                            */

tmErrorCode_t
    tmbslTDA9989PktSetVsInfoframe
    (tmUnitSelect_t txUnit, tmbslHdmiTxPkt_t *pPkt, UInt byteCnt, UInt8 uVersion, Bool bEnable) {
	tmHdmiTxobject_t *pDis;	/*                                      */
	tmErrorCode_t err;	/*            */
	UInt8 buf[31];		/*                            */

	/*                                                  */
	err = checkUnitSetDis(txUnit, &pDis);
	RETIF(err != TM_OK, err)

	    /*                                                           
                            
      */
	    RETIF(pDis->sinkType != HDMITX_SINK_HDMI, TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED)

	    /*                                       */
	    /*                                                     */
	    RETIF_BADPARAM((bEnable != True) && (bEnable != False))

	    if (pPkt != Null) {
		/*                                                   */
		/*                                                                 */
		RETIF_BADPARAM(byteCnt > (HDMITX_PKT_DATA_BYTE_CNT - 1))
		    RETIF(byteCnt == 0, TMBSL_ERR_HDMI_INCONSISTENT_PARAMS)

		    /*                                                               */
		    err = setHwRegisterField(pDis,
					     E_REG_P11_DIP_IF_FLAGS_RW,
					     E_MASKREG_P11_DIP_IF_FLAGS_if1, 0x00);
		RETIF_REG_FAIL(err)

		    /*                      */
		    lmemset(&buf[0], 0, 31);	/*                                           */
		buf[0] = 0x81;	/*                           */
		buf[1] = uVersion;	/*                        */
		buf[2] = (UInt8) byteCnt;	/*                   */

		/*                                                 */
		buf[0 + 3] = 0;	/*                                               */
		lmemcpy(&buf[1 + 3], &pPkt->dataByte[0], byteCnt);

		/*                                                                 
                                                                     
                  
   */
		buf[0 + 3] = calculateChecksum(&buf[0], byteCnt + 1 + 3);

		/*                                                               
                                                                
                                         */
		err = setHwRegisters(pDis, E_REG_P10_IF1_HB0_RW, &buf[0], 31);
		RETIF_REG_FAIL(err)
	}

	/*                                       */
	err = setHwRegisterField(pDis,
				 E_REG_P11_DIP_IF_FLAGS_RW,
				 E_MASKREG_P11_DIP_IF_FLAGS_if1, (UInt8) bEnable);
	return err;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                               */
/*                                                                               */
/*                                                                               */
static UInt8 reg_vid_fmt(tmbslHdmiTxVidFmt_t fmt,
			 tmbslHdmiTx3DStructure_t structure3D,
			 UInt8 *idx, UInt8 *idx3d, struct sync_desc **sync)
{
	struct vic2reg *hash;
	int i;

	(*idx) = REGVFMT_INVALID;
	if (idx3d)
		(*idx3d) = REGVFMT_INVALID;
	if (IS_TV(fmt)) {
		VIC2REG_LOOP(vic2reg_TV, idx);
		if (idx3d) {
			if (structure3D == HDMITX_3D_FRAME_PACKING) {
				/*                      */
				VIC2REG_LOOP(vic2reg_TV_FP, idx3d);
			}
		}
	}
#ifdef FORMAT_PC
	else {
		VIC2REG_LOOP(vic2reg_PC, idx);
	}
#endif
	/*              */
	if (sync) {
		if PREFETCH
			(*idx) {
			*sync = (struct sync_desc *)ref_sync;
			}
#ifdef FORMAT_PC
		else if PCFORMAT
			(*idx) {
			*sync = (struct sync_desc *)ref_sync_PC;
			*idx = *idx - E_REGVFMT_MAX_EXTRA;
			}
#endif				/*           */
		else {
			*sync = (struct sync_desc *)ref_sync_extra;
		}
	}
	return ((*idx) == REGVFMT_INVALID);
}

/*                                                                               */
/*                                                                               */
/*                                                                               */
UInt8 pix_clk(tmbslHdmiTxVidFmt_t fmt, tmbslHdmiTxVfreq_t freq, UInt8 *pclk)
{

	(*pclk) = REGVFMT_INVALID;
#ifdef FORMAT_PC
	if (IS_PC(fmt)) {
		(*pclk) = kVfmtToPixClk_PC[fmt - HDMITX_VFMT_PC_MIN];
	}
#endif
	if (IS_TV(fmt)) {
		(*pclk) = kVfmtToPixClk_TV[fmt - HDMITX_VFMT_TV_MIN][freq];
	}
	return ((*pclk) == REGVFMT_INVALID);
}

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
EXPORT_SYMBOL(pix_clk);
#endif

/*                                                                               */
/*                                                                               */
/*                                                                               */
tmbslHdmiTxVidFmt_t calculateVidFmtIndex(tmbslHdmiTxVidFmt_t vidFmt)
{
	tmbslHdmiTxVidFmt_t vidFmtIndex = vidFmt;

	/*                                         */
	if ((vidFmt >= HDMITX_VFMT_60_1280x720p_24Hz) && (vidFmt <= HDMITX_VFMT_62_1280x720p_30Hz)) {
		vidFmtIndex =
		    (tmbslHdmiTxVidFmt_t) (HDMITX_VFMT_INDEX_60_1280x720p_24Hz +
					   (vidFmt - HDMITX_VFMT_60_1280x720p_24Hz));
	}
#ifdef FORMAT_PC
	else if (IS_PC(vidFmt)) {
		vidFmtIndex =
		    (tmbslHdmiTxVidFmt_t) (HDMITX_VFMT_TV_NUM + (vidFmt - HDMITX_VFMT_PC_MIN));
	}
#endif				/*           */
	return (vidFmtIndex);
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
static tmErrorCode_t
    setDeVs
    (tmHdmiTxobject_t *pDis, tmbslHdmiTxVidFmt_t voutFmt, tmbslHdmiTx3DStructure_t structure3D) {
	tmErrorCode_t err;	/*            */
	UInt16 vsPixStrt2;	/*                                            */
	UInt8 reg_idx;		/*                                      */
	struct sync_desc *sync;

	/*                                          */
	RETIF(voutFmt == HDMITX_VFMT_NO_CHANGE, TM_OK);

	/*                                                               */
	RETIF_BADPARAM(reg_vid_fmt(voutFmt, structure3D, &reg_idx, 0, &sync));

	/*                                                                        */

	/*                                                                     */
	vsPixStrt2 = sync[BASE(reg_idx)].Vs2;
	err = setHwRegisterMsbLsb(pDis, E_REG_P00_VS_PIX_STRT_2_MSB_W, vsPixStrt2);
	RETIF_REG_FAIL(err)
	    err = setHwRegisterMsbLsb(pDis, E_REG_P00_VS_PIX_END_2_MSB_W, vsPixStrt2);
/*                                                      */

	return err;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
static tmErrorCode_t
    setPixelRepeat
    (tmHdmiTxobject_t *pDis,
     tmbslHdmiTxVidFmt_t voutFmt, UInt8 uPixelRepeat, tmbslHdmiTx3DStructure_t structure3D) {
	tmErrorCode_t err = TM_OK;	/*            */

	RETIF(voutFmt == HDMITX_VFMT_NO_CHANGE, TM_OK)

	    err = InputConfig(pDis,
			      HDMITX_VINMODE_NO_CHANGE,
			      HDMITX_PIXEDGE_NO_CHANGE,
			      HDMITX_PIXRATE_NO_CHANGE,
			      HDMITX_UPSAMPLE_NO_CHANGE, uPixelRepeat, voutFmt, structure3D);

	return err;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
static tmErrorCode_t setSampling(tmHdmiTxobject_t *pDis) {
	tmErrorCode_t err;	/*            */
	UInt8 upSample;		/*                                */
	UInt8 downSample;	/*                                  */
	UInt8 matrixBypass;	/*                               */

	if ((pDis->vinMode == HDMITX_VINMODE_YUV422)
	    || (pDis->vinMode == HDMITX_VINMODE_CCIR656)) {
		if (pDis->voutMode == HDMITX_VOUTMODE_YUV422) {
			/*                           */
			err = getHwRegister(pDis, E_REG_P00_MAT_CONTRL_W, &matrixBypass);
			RETIF_REG_FAIL(err)
			    matrixBypass &= E_MASKREG_P00_MAT_CONTRL_mat_bp;
			/*                           */
			if (matrixBypass > 0) {
				upSample = 0;
				downSample = 0;
			} else {
				upSample = 1;
				downSample = 1;
			}
		} else {
			/*                               */
			upSample = 1;
			downSample = 0;
		}
	} else {
		if (pDis->voutMode == HDMITX_VOUTMODE_YUV422) {
			/*                               */
			upSample = 0;
			downSample = 1;
		} else {
			/*                                   */
			upSample = 0;
			downSample = 0;
		}
	}

	/*                                                           */
	if (pDis->upsampleMode != HDMITX_UPSAMPLE_AUTO) {
		/*                                         */
		upSample = pDis->upsampleMode;
	}

	/*               */
	err = setHwRegisterField(pDis,
				 E_REG_P00_HVF_CNTRL_0_W,
				 E_MASKREG_P00_HVF_CNTRL_0_intpol, upSample);
	RETIF_REG_FAIL(err)

	    /*                 */
	    err = setHwRegisterField(pDis,
				     E_REG_P00_HVF_CNTRL_1_W,
				     E_MASKREG_P00_HVF_CNTRL_1_for, downSample);
	return err;
}


/*                                                                            */
/*                                                                            */
/*                                                                            */
static UInt8 calculateChecksum(UInt8 *pData,	/*                          */
			       Int numBytes	/*                                         */
    ) {
	UInt8 checksum = 0;	/*                              */
	UInt8 result = 0;	/*                      */
	Int i;

	if ((pData != Null) && (numBytes > 0)) {
		for (i = 0; i < numBytes; i++) {
			checksum = checksum + (*(pData + i));
		}
		result = (255 - checksum) + 1;
	}
	return result;		/*                                              */
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
static tmErrorCode_t
    InputConfig
    (tmHdmiTxobject_t *pDis,
     tmbslHdmiTxVinMode_t vinMode,
     tmbslHdmiTxPixEdge_t sampleEdge,
     tmbslHdmiTxPixRate_t pixRate,
     tmbslHdmiTxUpsampleMode_t upsampleMode,
     UInt8 uPixelRepeat, tmbslHdmiTxVidFmt_t voutFmt, tmbslHdmiTx3DStructure_t structure3D) {
	tmErrorCode_t err = TM_OK;	/*            */
	UInt8 reg_idx, reg_idx3D;	/*                                      */
	UInt8 ssd = 0;		/*                        */
	struct sync_desc *sync;

    /*                                                  */
	/*                  */
	RETIF_BADPARAM(vinMode >= HDMITX_VINMODE_INVALID);
	RETIF_BADPARAM(sampleEdge >= HDMITX_PIXEDGE_INVALID);
	RETIF_BADPARAM(pixRate >= HDMITX_PIXRATE_INVALID);
	RETIF_BADPARAM(upsampleMode >= HDMITX_UPSAMPLE_INVALID);

	RETIF(voutFmt == HDMITX_VFMT_NO_CHANGE, TM_OK);
	RETIF_BADPARAM(!IS_VALID_FMT(voutFmt));

	/*                                                               */
	RETIF_BADPARAM(reg_vid_fmt(voutFmt, structure3D, &reg_idx, &reg_idx3D, &sync));

/*                                                      
                         
                             
                           
                         
*/
	if (vinMode != HDMITX_VINMODE_NO_CHANGE) {
		pDis->vinMode = vinMode;
	}
/*                                                        
                     */

	if (sampleEdge != HDMITX_PIXEDGE_NO_CHANGE) {
		err = setHwRegisterField(pDis,
					 E_REG_P00_VIP_CNTRL_3_W,
					 E_MASKREG_P00_VIP_CNTRL_3_edge, (UInt8) sampleEdge);
		RETIF_REG_FAIL(err)
	}

/*                                                        
                     
                             
                            
                         */

	if (pixRate != HDMITX_PIXRATE_NO_CHANGE) {
		pDis->pixRate = pixRate;
	}

	if ((pixRate != HDMITX_PIXRATE_NO_CHANGE) || (vinMode != HDMITX_VINMODE_NO_CHANGE)) {
		switch (pDis->vinMode) {
		case HDMITX_VINMODE_RGB444:
		case HDMITX_VINMODE_YUV444:

			if (pDis->pixRate == HDMITX_PIXRATE_SINGLE) {
				err = setHwRegisterFieldTable(pDis, &kVinMode444[0]);

				RETIF_REG_FAIL(err)



				    err = setHwRegisterField(pDis,
							     E_REG_P00_VIP_CNTRL_4_W,
							     E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);
				RETIF_REG_FAIL(err)

			} else if (pDis->pixRate == HDMITX_PIXRATE_SINGLE_REPEATED) {
				err = setHwRegisterFieldTable(pDis, &kVinMode444[0]);

				RETIF_REG_FAIL(err)



				    err = setHwRegisterField(pDis,
							     E_REG_P00_VIP_CNTRL_4_W,
							     E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);
				RETIF_REG_FAIL(err)

			} else {
				/*               */
			}
			break;
		case HDMITX_VINMODE_YUV422:
			if (pDis->pixRate == HDMITX_PIXRATE_SINGLE) {
				err = setHwRegisterFieldTable(pDis, &kVinModeYUV422[0]);

				RETIF_REG_FAIL(err)



				    err = setHwRegisterField(pDis,
							     E_REG_P00_VIP_CNTRL_4_W,
							     E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);
				RETIF_REG_FAIL(err)

			} else if (pDis->pixRate == HDMITX_PIXRATE_SINGLE_REPEATED) {
				err = setHwRegisterFieldTable(pDis, &kVinModeYUV422[0]);

				RETIF_REG_FAIL(err)



				    err = setHwRegisterField(pDis,
							     E_REG_P00_VIP_CNTRL_4_W,
							     E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);
				RETIF_REG_FAIL(err)

			} else {
				/*               */
				return TMBSL_ERR_HDMI_BAD_PARAMETER;
			}
			break;
		case HDMITX_VINMODE_CCIR656:
			if (pDis->pixRate == HDMITX_PIXRATE_SINGLE) {

				err = setHwRegisterFieldTable(pDis, &kVinModeCCIR656[0]);

				RETIF_REG_FAIL(err)



				    err = setHwRegisterField(pDis,
							     E_REG_P00_VIP_CNTRL_4_W,
							     E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);
				RETIF_REG_FAIL(err)

			} else if (pDis->pixRate == HDMITX_PIXRATE_SINGLE_REPEATED) {
				err = setHwRegisterFieldTable(pDis, &kVinModeCCIR656[0]);

				RETIF_REG_FAIL(err)



				    err = setHwRegisterField(pDis,
							     E_REG_P00_VIP_CNTRL_4_W,
							     E_MASKREG_P00_VIP_CNTRL_4_656_alt, 0);
				RETIF_REG_FAIL(err)

			} else if (pDis->pixRate == HDMITX_PIXRATE_DOUBLE) {
				err =
				    setHwRegisterFieldTable(pDis,
							    &kVinModeCCIR656_DDR_above720p[0]);

				RETIF_REG_FAIL(err)




				    err = setHwRegisterField(pDis,
							     E_REG_P00_VIP_CNTRL_4_W,
							     E_MASKREG_P00_VIP_CNTRL_4_656_alt, 1);
				RETIF_REG_FAIL(err)

			}
			break;
		default:
			err = setHwRegisterFieldTable(pDis, &kVinMode444[0]);

			RETIF_REG_FAIL(err)
			    break;
		}

	}
    /*                                                           */

	if (upsampleMode != HDMITX_UPSAMPLE_NO_CHANGE) {
		pDis->upsampleMode = upsampleMode;
	}

/*                                                                     
                           
                        */

	if ((structure3D == HDMITX_3D_FRAME_PACKING) && (reg_idx3D != REGVFMT_INVALID)) {
		/*                          */
		ssd = pll[reg_idx3D];
	} else {
		/*                          */
		ssd = pll[reg_idx];
	}

	if (ssd < SSD_UNUSED_VALUE) {
		err = setHwRegisterField(pDis, E_REG_P02_PLL_SERIAL_2_RW,
					 E_MASKREG_P02_PLL_SERIAL_2_srl_nosc, ssd);
/*                                        */
	}

/*                                                               
                         */

	/*                      */
	if (uPixelRepeat != HDMITX_PIXREP_NO_CHANGE) {
		if (uPixelRepeat == HDMITX_PIXREP_DEFAULT) {
			/*                                                           */
			uPixelRepeat = sync[BASE(reg_idx)].pix_rep;
		}

		/*                                       */
		pDis->pixelRepeatCount = uPixelRepeat;

		err = setHwRegisterField(pDis,
					 E_REG_P02_PLL_SERIAL_2_RW,
					 E_MASKREG_P02_PLL_SERIAL_2_srl_pr, uPixelRepeat);
		RETIF_REG_FAIL(err)
		    /*                                                 */
		    err = setHwRegister(pDis, E_REG_P00_RPT_CNTRL_W, uPixelRepeat);
	}

/*                                                          
                                     
                                     
                                     
                                     
                                     
                                     
*/

	err = setHwRegisterField(pDis,
				 E_REG_P02_PLL_SERIAL_1_RW,
				 E_MASKREG_P02_PLL_SERIAL_1_srl_man_iz, 0);


	RETIF_REG_FAIL(err)

	    err = setHwRegisterField(pDis,
				     E_REG_P02_PLL_SERIAL_3_RW,
				     E_MASKREG_P02_PLL_SERIAL_3_srl_de, 0);
	RETIF_REG_FAIL(err)

	    err = setHwRegister(pDis, E_REG_P02_SERIALIZER_RW, 0);
	RETIF_REG_FAIL(err)

	    return err;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
