/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiTx_local.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 02/08/07 08:32 $
 *
 * \brief         devlib driver component API for the TDA998x HDMI Transmitters
 *
 * \section refs  Reference Documents
 * HDMI Tx Driver - FRS.doc,
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmdlHdmiTx_local.h $
 *
  * *****************  Version 13  *****************
 * User: J. Lamotte Date: 02/08/07   Time: 08:32
 * Updated in $/Source/tmdlHdmiTx/inc
 * initial version
 *

   \endverbatim
 *
*/

#ifndef TMDLHDMITX_LOCAL_H
#define TMDLHDMITX_LOCAL_H

#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
#include <linux/kernel.h>
#endif

#include "tmdlHdmiTx_IW.h"
#include "tmNxTypes.h"
#include "tmdlHdmiTx_Types.h"
#include "tmdlHdmiTx_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                          */
#define VERSION_COMPATIBILITY 0
#define VERSION_MAJOR         5
#define VERSION_MINOR         3

/*                   */
#define HDCP_SEED_NULL      0

/*                                                                    
                                                                           
                                                   */
#define HDCP_SEED_DEFAULT   HDCP_SEED_NULL

/*                             */
#define HDCP_OPT_DEFAULT    (TMDL_HDMITX_HDCP_OPTION_FORCE_PJ_IGNORED \
				| TMDL_HDMITX_HDCP_OPTION_FORCE_VSLOW_DDC \
				| TMDL_HDMITX_HDCP_OPTION_FORCE_NO_1_1)

/* 
                                                           
 */
#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
#define RETIF(cond, rslt)       if ((cond)) { \
      pr_debug("%s %d\n", __func__, __LINE__); \
      return (rslt); }
#else
#define RETIF(cond, rslt)       if ((cond)) {return (rslt); }
#endif

/* 
                                                  
                                   
                                                                              
                             
 */
#ifdef NO_RETIF_BADPARAM
#define RETIF_BADPARAM(cond)
#else
#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
#define RETIF_BADPARAM(cond)  if ((cond)) { \
      pr_debug("%s %d\n", __func__, __LINE__);    \
      return TMDL_ERR_DLHDMITX_BAD_PARAMETER; }
#else
#define RETIF_BADPARAM(cond)  if ((cond)) {return TMDL_ERR_DLHDMITX_BAD_PARAMETER; }
#endif
#endif

/* 
                                                                                                         
 */
#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
#define RETIF_SEM(handle, cond, rslt)       if ((cond)) { \
      tmdlHdmiTxIWSemaphoreV(handle);                    \
      pr_debug("%s %d\n", __func__, __LINE__);     \
      return (rslt); }
#else
#define RETIF_SEM(handle, cond, rslt)       if ((cond)) {tmdlHdmiTxIWSemaphoreV(handle); return (rslt); }
#endif

	/*                      */
#ifndef FORMAT_PC
#define RESOLUTION_NB   41
#else
#define RESOLUTION_NB   68
#endif				/*           */

/*                 */
#define INSTANCE_0  0
#define INSTANCE_1  1

#ifdef HDMI_TX_REPEATER_ISR_MODE
/*                 */
#define EVENT_NB    10
#else				/*                           */
/*                 */
#define EVENT_NB    9
#endif				/*                           */

/*                             */
#define KSV_SIZE  5

/*                                  */
#define TV_INVALID      0
#define TV_VGA_60Hz     1
#define TV_240p_60Hz    2
#define TV_480p_60Hz    3
#define TV_480i_60Hz    4
#define TV_720p_60Hz    5
#define TV_1080p_60Hz   6
#define TV_1080i_60Hz   7
#define TV_288p_50Hz    8
#define TV_576p_50Hz    9
#define TV_576i_50Hz    10
#define TV_720p_50Hz    11
#define TV_1080p_50Hz   12
#define TV_1080i_50Hz   13

/*                                                 */
#define iINVALID TMDL_HDMITX_VINMODE_INVALID
#define iCCIR656 TMDL_HDMITX_VINMODE_CCIR656
#define iRGB444  TMDL_HDMITX_VINMODE_RGB444
#define iYUV444  TMDL_HDMITX_VINMODE_YUV444
#define iYUV422  TMDL_HDMITX_VINMODE_YUV422

/*                           */
#define EMB      1
#define EXT      0

/*                                                           */
#define SINGLE   TMDL_HDMITX_PIXRATE_SINGLE
#define DOUBLE   TMDL_HDMITX_PIXRATE_DOUBLE

/*                                                                  */
#define AIF_SF_REFER_TO_STREAM_HEADER   0
#define AIF_SF_32K                      1
#define AIF_SF_44K                      2
#define AIF_SF_48K                      3
#define AIF_SF_88K                      4
#define AIF_SF_96K                      5
#define AIF_SF_176K                     6
#define AIF_SF_192K                     7

/*                                     */
#define HDCP_CHECK_INTERVAL_MS  2500

/*                                                          */
#define HDCP_NUM_CHECKS         5

#define TMDL_HDMITX_CHANNELALLOC_LUT_SIZE 32


	static UInt8 kChanAllocChanNum[TMDL_HDMITX_CHANNELALLOC_LUT_SIZE] __maybe_unused =
	    { 2, 3, 3, 4, 3, 4, 4, 5, 4, 5, 5, 6, 5, 6, 6, 7, 6, 7, 7, 8, 4, 5, 5, 6, 5, 6, 6, 7, 6,
7, 7, 8 };


/* 
                                                          
                                        
                             
 */
#ifndef FORMAT_PC
	static UInt8 kVfmtToAspect_TV[TMDL_HDMITX_VFMT_TV_NUM] __maybe_unused =
#else				/*           */
	static UInt8 kVfmtToAspect_TV[TMDL_HDMITX_VFMT_TV_NUM + TMDL_HDMITX_VFMT_PC_NUM] =
#endif				/*           */
	{
		0,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		2,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		2,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		1,		/*                                 */
		2,		/*                                 */
		2,		/*                                 */
		2,		/*                                 */
		2,		/*                                 */
		2,		/*                                 */

		1,		/*                                    */
		2,		/*                                    */
		1,		/*                                    */
		2,		/*                                    */

		2,		/*                                    */
		2,		/*                                    */
		2		/*                                    */
#ifdef FORMAT_PC
		    , 1,	/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1,		/*                                 */
		1		/*                                 */
#endif				/*           */
	};

/* 
                                                          
                                                     
 */
	static UInt8 kVfmtToShortFmt_TV[TMDL_HDMITX_VFMT_TV_NUM] __maybe_unused = {
		TV_INVALID,	/*                                */
		TV_VGA_60Hz,	/*                                */
		TV_480p_60Hz,	/*                                */
		TV_480p_60Hz,	/*                                */
		TV_720p_60Hz,	/*                                */
		TV_1080i_60Hz,	/*                                */
		TV_480i_60Hz,	/*                                */
		TV_480i_60Hz,	/*                                */
		TV_240p_60Hz,	/*                                */
		TV_240p_60Hz,	/*                                */
		TV_480i_60Hz,	/*                                */
		TV_480i_60Hz,	/*                                */
		TV_240p_60Hz,	/*                                */
		TV_240p_60Hz,	/*                                */
		TV_480p_60Hz,	/*                                */
		TV_480p_60Hz,	/*                                */
		TV_1080p_60Hz,	/*                                */
		TV_576p_50Hz,	/*                                */
		TV_576p_50Hz,	/*                                */
		TV_720p_50Hz,	/*                                */
		TV_1080i_50Hz,	/*                                */
		TV_576i_50Hz,	/*                                */
		TV_576i_50Hz,	/*                                */
		TV_288p_50Hz,	/*                                */
		TV_288p_50Hz,	/*                                */
		TV_576i_50Hz,	/*                                */
		TV_576i_50Hz,	/*                                */
		TV_288p_50Hz,	/*                                */
		TV_288p_50Hz,	/*                                */
		TV_576p_50Hz,	/*                                */
		TV_576p_50Hz,	/*                                */
		TV_1080p_50Hz,	/*                                */
		TV_INVALID,	/*                                */
		TV_INVALID,	/*                                */
		TV_INVALID,	/*                                */
		TV_480p_60Hz,	/*                                */
		TV_480p_60Hz,	/*                                */
		TV_576p_50Hz,	/*                                */
		TV_576p_50Hz,	/*                                */
		TV_INVALID,	/*                                */
		TV_INVALID,	/*                                */
		TV_INVALID	/*                                */
	};

/* 
                                                                           
              
 */
#define PKBYTE(mode, rate, sync, verf) (((rate)<<7)|((sync)<<6)|((verf)<<5)|((mode)&0x1F))

/* 
                                                                              
              
 */
#define UNPKRATE(byte) (((byte)>>7)&1)
#define UNPKSYNC(byte) (((byte)>>6)&1)
#define UNPKVERF(byte) (((byte)>>5)&1)
#define UNPKMODE(byte) ((byte)&0x1F)

/* 
                                                                
                            
 */
	static struct {
		/*                 */
		UInt8 modeRateSyncVerf;	/*                                            */
		UInt8 shortVinFmt;
		UInt8 shortVoutFmt;
		/*                   */
		UInt16 refPix;	/*               */
		UInt16 refLine;
		UInt16 scRefPix;	/*               */
		UInt16 scRefLine;
	} kRefpixRefline[] __maybe_unused = {
  /*                                                           */
  /*                                                           */
  /*                                                           */
  /*                                                           */
		/*                                                                                                                  */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_480i_60Hz, TV_480p_60Hz, 0x08b, 0x024, 0x078, 0x017},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_480i_60Hz, TV_720p_60Hz, 0x08b, 0x012, 0x078, 0x017},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_480i_60Hz, TV_1080i_60Hz, 0x08b, 0x00e, 0x078, 0x017},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_480i_60Hz, TV_1080p_60Hz, 0x08b, 0x021, 0x078, 0x017},	/*  */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_480p_60Hz, TV_720p_60Hz, 0x08b, 0x017, 0x078, 0x02c},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_480p_60Hz, TV_1080i_60Hz, 0x08b, 0x013, 0x078, 0x02c},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_480p_60Hz, TV_1080p_60Hz, 0x08b, 0x027, 0x07A, 0x02c},	/*  */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_576i_50Hz, TV_576p_50Hz, 0x091, 0x026, 0x085, 0x018},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_576i_50Hz, TV_720p_50Hz, 0x091, 0x013, 0x085, 0x018},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_576i_50Hz, TV_1080i_50Hz, 0x091, 0x00f, 0x085, 0x018},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_576i_50Hz, TV_1080p_50Hz, 0x091, 0x022, 0x085, 0x018},	/*  */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_576p_50Hz, TV_720p_50Hz, 0x091, 0x019, 0x085, 0x02e},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_576p_50Hz, TV_1080i_50Hz, 0x091, 0x014, 0x085, 0x02e},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EMB, 1), TV_576p_50Hz, TV_1080p_50Hz, 0x091, 0x028, 0x087, 0x02e},	/*  */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_480i_60Hz, TV_480p_60Hz, 0x014, 0x20d, 0x359, 0x004},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_480i_60Hz, TV_720p_60Hz, 0x014, 0x2cb, 0x359, 0x004},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_480i_60Hz, TV_1080i_60Hz, 0x014, 0x44c, 0x359, 0x004},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_480i_60Hz, TV_1080p_60Hz, 0x014, 0x436, 0x359, 0x004},	/*  */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_480p_60Hz, TV_720p_60Hz, 0x011, 0x2d3, 0x358, 0x007},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_480p_60Hz, TV_1080i_60Hz, 0x011, 0x452, 0x358, 0x007},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_480p_60Hz, TV_1080p_60Hz, 0x011, 0x43e, 0x358, 0x007},	/*  */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_576i_50Hz, TV_576p_50Hz, 0x00d, 0x26b, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_576i_50Hz, TV_720p_50Hz, 0x00d, 0x2cb, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_576i_50Hz, TV_1080i_50Hz, 0x00d, 0x44b, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_576i_50Hz, TV_1080p_50Hz, 0x00d, 0x435, 0x35f, 0x001},	/*  */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_576p_50Hz, TV_720p_50Hz, 0x00d, 0x2d1, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_576p_50Hz, TV_1080i_50Hz, 0x00d, 0x451, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, SINGLE, EXT, 1), TV_576p_50Hz, TV_1080p_50Hz, 0x00d, 0x43d, 0x35f, 0x001},	/*  */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_480i_60Hz, TV_480p_60Hz, 0x08b, 0x024, 0x078, 0x017},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_480i_60Hz, TV_720p_60Hz, 0x08b, 0x012, 0x078, 0x017},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_480i_60Hz, TV_1080i_60Hz, 0x08b, 0x00e, 0x078, 0x017},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_480i_60Hz, TV_1080p_60Hz, 0x08b, 0x021, 0x078, 0x017},	/*  */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_480p_60Hz, TV_720p_60Hz, 0x08b, 0x017, 0x078, 0x02c},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_480p_60Hz, TV_1080i_60Hz, 0x08b, 0x013, 0x078, 0x02c},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_480p_60Hz, TV_1080p_60Hz, 0x08b, 0x027, 0x07A, 0x02c},	/*  */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_576i_50Hz, TV_576p_50Hz, 0x091, 0x026, 0x085, 0x018},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_576i_50Hz, TV_720p_50Hz, 0x091, 0x013, 0x085, 0x018},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_576i_50Hz, TV_1080i_50Hz, 0x091, 0x00f, 0x085, 0x018},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_576i_50Hz, TV_1080p_50Hz, 0x091, 0x022, 0x085, 0x018},	/*  */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_576p_50Hz, TV_720p_50Hz, 0x091, 0x019, 0x085, 0x02e},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_576p_50Hz, TV_1080i_50Hz, 0x091, 0x014, 0x085, 0x02e},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EMB, 1), TV_576p_50Hz, TV_1080p_50Hz, 0x091, 0x028, 0x087, 0x02e},	/*  */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_480i_60Hz, TV_480p_60Hz, 0x014, 0x20d, 0x359, 0x004},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_480i_60Hz, TV_720p_60Hz, 0x014, 0x2cb, 0x359, 0x004},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_480i_60Hz, TV_1080i_60Hz, 0x014, 0x44c, 0x359, 0x004},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_480i_60Hz, TV_1080p_60Hz, 0x014, 0x436, 0x359, 0x004},	/*  */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_480p_60Hz, TV_720p_60Hz, 0x011, 0x2d3, 0x358, 0x007},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_480p_60Hz, TV_1080i_60Hz, 0x011, 0x452, 0x358, 0x007},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_480p_60Hz, TV_1080p_60Hz, 0x011, 0x43e, 0x358, 0x007},	/*  */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_576i_50Hz, TV_576p_50Hz, 0x00d, 0x26b, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_576i_50Hz, TV_720p_50Hz, 0x00d, 0x2cb, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_576i_50Hz, TV_1080i_50Hz, 0x00d, 0x44b, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_576i_50Hz, TV_1080p_50Hz, 0x00d, 0x435, 0x35f, 0x001},	/*  */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_576p_50Hz, TV_720p_50Hz, 0x00d, 0x2d1, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_576p_50Hz, TV_1080i_50Hz, 0x00d, 0x451, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iCCIR656, DOUBLE, EXT, 1), TV_576p_50Hz, TV_1080p_50Hz, 0x00d, 0x43d, 0x35f, 0x001},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_480i_60Hz, TV_480p_60Hz, 0x08d, 0x028, 0x078, 0x017},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_480i_60Hz, TV_720p_60Hz, 0x08d, 0x014, 0x078, 0x017},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_480i_60Hz, TV_1080i_60Hz, 0x08d, 0x010, 0x078, 0x017},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_480i_60Hz, TV_1080p_60Hz, 0x08d, 0x021, 0x078, 0x017},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_480p_60Hz, TV_720p_60Hz, 0x08d, 0x017, 0x078, 0x02c},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_480p_60Hz, TV_1080i_60Hz, 0x08d, 0x014, 0x078, 0x02c},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_480p_60Hz, TV_1080p_60Hz, 0x08d, 0x027, 0x07C, 0x02c},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_576i_50Hz, TV_576p_50Hz, 0x093, 0x02a, 0x085, 0x018},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_576i_50Hz, TV_720p_50Hz, 0x093, 0x013, 0x085, 0x018},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_576i_50Hz, TV_1080i_50Hz, 0x093, 0x00e, 0x085, 0x018},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_576i_50Hz, TV_1080p_50Hz, 0x093, 0x022, 0x085, 0x018},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_576p_50Hz, TV_720p_50Hz, 0x093, 0x019, 0x085, 0x02e},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_576p_50Hz, TV_1080i_50Hz, 0x093, 0x014, 0x085, 0x02e},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_576p_50Hz, TV_1080p_50Hz, 0x093, 0x028, 0x089, 0x02e},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_720p_50Hz, TV_1080p_50Hz, 0x2bf, 0x024, 0x105, 0x019},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_720p_60Hz, TV_1080p_60Hz, 0x175, 0x024, 0x105, 0x019},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_1080i_50Hz, TV_1080p_50Hz, 0x2d3, 0x023, 0x0c3, 0x014},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EMB, 1), TV_1080i_60Hz, TV_1080p_60Hz, 0x11b, 0x023, 0x0c3, 0x014},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_480i_60Hz, TV_480p_60Hz, 0x016, 0x20d, 0x359, 0x004},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_480i_60Hz, TV_720p_60Hz, 0x016, 0x2cb, 0x359, 0x004},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_480i_60Hz, TV_1080i_60Hz, 0x016, 0x44c, 0x359, 0x004},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_480i_60Hz, TV_1080p_60Hz, 0x016, 0x436, 0x359, 0x004},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_480p_60Hz, TV_720p_60Hz, 0x013, 0x2d3, 0x358, 0x007},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_480p_60Hz, TV_1080i_60Hz, 0x013, 0x452, 0x358, 0x007},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_480p_60Hz, TV_1080p_60Hz, 0x013, 0x43e, 0x358, 0x007},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_576i_50Hz, TV_576p_50Hz, 0x00f, 0x26b, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_576i_50Hz, TV_720p_50Hz, 0x00f, 0x2cb, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_576i_50Hz, TV_1080i_50Hz, 0x00f, 0x44b, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_576i_50Hz, TV_1080p_50Hz, 0x00f, 0x435, 0x35f, 0x001},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_576p_50Hz, TV_720p_50Hz, 0x00f, 0x2d1, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_576p_50Hz, TV_1080i_50Hz, 0x00f, 0x451, 0x35f, 0x001},	/*          */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_576p_50Hz, TV_1080p_50Hz, 0x00f, 0x43d, 0x35f, 0x001},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_720p_50Hz, TV_1080p_50Hz, 0x1bb, 0x463, 0x7bb, 0x000},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_720p_60Hz, TV_1080p_60Hz, 0x071, 0x463, 0x671, 0x000},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_1080i_50Hz, TV_1080p_50Hz, 0x213, 0x460, 0xa4f, 0x000},	/*  */
		{
		PKBYTE(iYUV422, SINGLE, EXT, 1), TV_1080i_60Hz, TV_1080p_60Hz, 0x05b, 0x460, 0x897, 0x000},	/*  */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EMB, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV422, DOUBLE, EXT, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EMB, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, SINGLE, EXT, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EMB, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iYUV444, DOUBLE, EXT, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EMB, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_480p_60Hz, TV_VGA_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, SINGLE, EXT, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EMB, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_480i_60Hz, TV_480p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_480i_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_480i_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_480p_60Hz, TV_720p_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_480p_60Hz, TV_1080i_60Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_576i_50Hz, TV_576p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_576i_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_576i_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_576p_50Hz, TV_720p_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iRGB444, DOUBLE, EXT, 0), TV_576p_50Hz, TV_1080i_50Hz, 0x000, 0x000, 0x000, 0x000},	/*          */
		{
		PKBYTE(iINVALID, DOUBLE, EMB, 0), TV_INVALID, TV_INVALID, 0x000, 0x000, 0x000, 0x000}	/*          */
	};


/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                          */
	typedef enum {
		TMDL_HDMITX_COLORIMETRY_NO_DATA = 0,
		TMDL_HDMITX_COLORIMETRY_ITU601 = 1,
		TMDL_HDMITX_COLORIMETRY_ITU709 = 2,
		TMDL_HDMITX_COLORIMETRY_EXTENDED = 3
	} tmdlHdmiTxColorimetry_t;

/*                                      */
	typedef enum {
		STATE_NOT_INITIALIZED,
			    /*                             */
		STATE_INITIALIZED,
			    /*                         */
		STATE_UNPLUGGED,
			    /*                                 */
		STATE_PLUGGED,
			    /*                                         */
		STATE_EDID_AVAILABLE
			    /*                                   */
	} tmdlHdmiTxDriverState_t;

/*                           */
	typedef struct {
		UInt8 *pList;
		UInt32 length;
	} revocationList_t;


/*                              */
	typedef struct {
		Bool opened;						/*                          */
		Bool hdcpEnable;					/*                     */
		tmdlHdmiTxHdcpOptions_t hdcpOptions;		/*                */
		Bool repeaterEnable;				/*                         */
		Bool simplayHd;						/*                            */
		tmdlHdmiTxDeviceVersion_t deviceVersion;	/*                            */
		UInt8 *pEdidBuffer;				/*                            */
		UInt32 edidBufferSize;				/*                                    */
		tmdlHdmiTxIWTaskHandle_t commandTaskHandle;	/*                                                      */
		tmdlHdmiTxIWQueueHandle_t queueHandle;		/*                                                       */
		tmdlHdmiTxIWTaskHandle_t hdcpTaskHandle;		/*                                                         */
		tmdlHdmiTxDriverState_t state;				/*                               */
		ptmdlHdmiTxCallback_t pCallback;			/*                 */
		revocationList_t revocationList;	/*                   */
	} unitConfig_t;


/*                 */

/*                             */
	typedef struct _tmdlHdmiTxVideoInfo_t {
		Bool videoMuteState;	/*                          */
		tmdlHdmiTxVideoInConfig_t videoInConfig;	/*                           */
		tmdlHdmiTxVideoOutConfig_t videoOutConfig;	/*                            */
	} tmdlHdmiTxVideoInfo_t, *ptmdlHdmiTxVideoInfo_t;

/*                             */
	typedef struct _tmdlHdmiTxAudioInfo_t {
		Bool audioMuteState;	/*                          */
		tmdlHdmiTxAudioInConfig_t audioInCfg;	/*                           */
	} tmdlHdmiTxAudioInfo_t, *ptmdlHdmiTxAudioInfo_t;

/*                       */
	typedef struct _tmdlHdmiTxEventState_t {
		tmdlHdmiTxEvent_t event;	/*       */
		tmdlHdmiTxEventStatus_t status;	/*                                   */
	} tmdlHdmiTxEventState_t, *ptmdlHdmiTxEventState_t;

/*                            */
	typedef struct _tmdlHdmiTxColBarState_t {
		Bool disableColorBarOnR0;	/*                                      */
		Bool hdcpColbarChange;	/*                                */
		Bool hdcpEncryptOrT0;	/*                                   */
		Bool hdcpSecureOrT0;	/*                             */
		Bool inOutFirstSetDone;	/*                                                     */
		Bool colorBarOn;
		Bool changeColorBarNow;
	} tmdlHdmiTxColBarState_t, *ptmdlHdmiTxColBarState_t;

/*                       */
	typedef struct _tmdlHdmiTxGamutState_t {
		Bool gamutOn;	/*                                */
		UInt8 gamutBufNum;	/*                                                       */
		tmdlHdmiTxExtColorimetry_t wideGamutColorSpace;	/*                            */
		Bool extColOn;	/*                                                   */
		tmdlHdmiTxYCCQR_t yccQR;	/*                              */
	} tmdlHdmiTxGamutState_t, *ptmdlHdmiTxGamutState_t;


/*                           */
	typedef struct {
		ptmdlHdmiTxVideoInfo_t pVideoInfo;	/*                                               */
		ptmdlHdmiTxAudioInfo_t pAudioInfo;	/*                                               */
		ptmdlHdmiTxEventState_t pEventState;	/*                                  */
		ptmdlHdmiTxColBarState_t pColBarState;	/*                  */
		ptmdlHdmiTxGamutState_t pGamutState;	/*             */
	} instanceStatus_t;

/*                                                                            */
/*                                                                            */
/*                                                                            */


/*                                                                             
                                                                  
                             

                                                  

                            
                                     
                                                                     
                                               
                                                                   
                   

                                                                             */
	tmErrorCode_t dlHdmiTxGetConfig
	    (tmUnitSelect_t unit, tmdlHdmiTxDriverConfigTable_t *pConfig);


#ifdef __cplusplus
}
#endif

#endif				/*                    */

/*                                                                            */
/*                                                                            */
/*                                                                            */
