/**
 * Copyright (C) 2007 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmbslHdmiTx_types.h
 *
 * \version       $Revision: 18 $
 *
 * \date          $Date: 17/03/08 $
 *
 * \brief         HDMI Transmitter common types
 *
 * \section refs  Reference Documents
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmbslHdmiTx_types.h $
 *
 *
 * **************** Version 18  ******************
 * User: G.Burnouf     Date: 01/04/08
 * Updated in $/Source/tmbslHdmiTx/inc
 * PR1468 : add new function tmbslTDA9984GetSinkCategory
 *
 *
 * **************** Version 17  ******************
 * User: G.Burnouf     Date: 17/03/08
 * Updated in $/Source/tmbslHdmiTx/inc
 * PR1430 : Increase size of table for
 *          Short Audio Descriptor
 *
 * **************** Version 16  ******************
 * User: G.Burnouf     Date: 06/03/08
 * Updated in $/Source/tmbslHdmiTx/inc
 * PR1406 : new reset audio fifo sequence
 *
 * **************** Version 15  ******************
 * User: G.Burnouf     Date: 05/02/08
 * Updated in $/Source/tmbslHdmiTx/inc
 * PR1251 : add new type for function
 *          tmbslTDA9984EdidGetBasicDisplayParam
 *
 ******************  Version 14  ******************
 * User: G.Burnouf     Date: 14/01/08
 * Updated in $/Source/tmbslHdmiTx/inc
 * PR580 - Change BSL error base address
 *
 ******************  Version 13  ******************
 * User: G.Burnouf     Date: 10/01/08
 * Updated in $/Source/tmbslHdmiTx/inc
 * PR606 - Apply audio port config in function
 *         of audio format
 *
 * **************** Version 12  ******************
 * User: G.Burnouf     Date: 10/12/07   Time: 08:30
 * Updated in $/Source/tmbslHdmiTx/inc
 * PR1145 : return DTD and monitor description
 *
 * *****************  Version 11  *****************
 * User: G.Burnouf     Date: 04/12/07
 * Updated in $/Source/tmbslHdmiTx/inc
 * PR948 : add new formats, 1080p24/25/30
 *
 * *****************  Version 10 *****************
 * User: C. Diehl	  Date: 27/11/07
 * Updated in $/Source/tmbslHdmiTx/inc
 * PR1030 : - Align with the common interface
 *            reworked for the LIPP4200
 *
 * *****************  Version 9  *****************
 * User: J.Lamotte	  Date: 23/11/07   Time: 09:35
 * Updated in $/Source/tmbslHdmiTx/src
 * PR1078 : - update HDMI_TX_SVD_MAX_CNT from 30
 *            to 113
 *
 * *****************  Version 8  *****************
 * User: G.Burnouf	  Date: 13/11/07   Time: 09:29
 * Updated in $/Source/tmbslHdmiTx/src
 * PR1008 : - update type tmbslHdmiTxHwFeature_t
 *
 * *****************  Version 7  *****************
 * User: G.Burnouf	  Date: 16/10/07   Time: 14:32
 * Updated in $/Source/tmbslHdmiTx/src
 * PR882 : - add type tmbslHdmiTxPowerState_t
 *         - add type tmbslHdmiTxPktGmt_t for gamut
 *         - add new interrupt callback for VS
 *
 * *****************  Version 6  *****************
 * User: G.Burnouf	  Date: 05/10/07   Time: 14:32
 * Updated in $/Source/tmbslHdmiTx/src
 * PR824 : add type for enum _tmbslHdmiTxCallbackInt
 *
 * *****************  Version 5  *****************
 * User: J.Turpin	  Date: 13/09/07   Time: 14:32
 * Updated in $/Source/tmbslHdmiTx/src
 * PR693 : add black pattern functionality
 *		 - add HDMITX_PATTERN_BLACK in
 *			enum tmbslHdmiTxTestPattern_t
 *
 * *****************  Version 4  *****************
 * User: G.Burnouf   Date: 06/09/07   Time: 17:22
 * Updated in $/Source/tmbslTDA9984/Inc
 * PR656 : - add HBR format
 *         - add format I2s Philips left and right justified
 *
 * *****************  Version 3  *****************
 * User: G. Burnouf      Date: 07/08/07   Time: 10:30
 * Updated in $/Source/tmbslTDA9984/Inc
 * PR572 - change type name of tmbslTDA9984_ to tmbslHdmiTx_
 *
 * *****************  Version 2  *****************
 * User: B.Vereecke      Date: 07/08/07   Time: 10:30
 * Updated in $/Source/tmbslTDA9984/Inc
 * PR551 - Add a new Pattern type in tmbslHdmiTxTestPattern_t
 *			it is used for set the bluescreen
 *
 * *****************  Version 1  *****************
 * User: G. Burnouf    Date: 05/07/07   Time: 17:00
 * Updated in $/Source/tmbslTDA9984/Inc
 * PR 414 : Add new edid management
 *
   \endverbatim
 *
*/

#ifndef TMBSLHDMITX_TYPES_H
#define TMBSLHDMITX_TYPES_H

/*                                                                            */
/*                                                                            */
/*                                                                            */

#include "tmNxCompId.h"



#ifdef __cplusplus
extern "C" {
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */

/* 
                                                         
 */
#define HDMITX_UNITS_MAX       2

/*              
                                                                       
 */
/*  */
/*                                      
                                                                               
                                                           
 */
#define TMBSL_ERR_HDMI_BASE	CID_BSL_HDMITX

/*                                            */
#ifndef TM_OK
#define TM_OK   0
#endif

/*                                   */
#define TMBSL_ERR_HDMI_COMPATIBILITY            (TMBSL_ERR_HDMI_BASE + 0x001U)

/*                         */
#define TMBSL_ERR_HDMI_MAJOR_VERSION            (TMBSL_ERR_HDMI_BASE + 0x002U)

/*                             */
#define TMBSL_ERR_HDMI_COMP_VERSION             (TMBSL_ERR_HDMI_BASE + 0x003U)

/*                             */
#define TMBSL_ERR_HDMI_BAD_UNIT_NUMBER          (TMBSL_ERR_HDMI_BASE + 0x005U)

/*                                                 */
#define TMBSL_ERR_HDMI_BAD_PARAMETER            (TMBSL_ERR_HDMI_BASE + 0x009U)

/*                         */
#define TMBSL_ERR_HDMI_RESOURCE_NOT_AVAILABLE   (TMBSL_ERR_HDMI_BASE + 0x00CU)

/*                                */
#define TMBSL_ERR_HDMI_INCONSISTENT_PARAMS      (TMBSL_ERR_HDMI_BASE + 0x010U)

/*                               */
#define TMBSL_ERR_HDMI_NOT_INITIALIZED          (TMBSL_ERR_HDMI_BASE + 0x011U)

/*                                           */
#define TMBSL_ERR_HDMI_NOT_SUPPORTED            (TMBSL_ERR_HDMI_BASE + 0x013U)

/*                        */
#define TMBSL_ERR_HDMI_INIT_FAILED              (TMBSL_ERR_HDMI_BASE + 0x014U)

/*                                                  */
#define TMBSL_ERR_HDMI_BUSY                     (TMBSL_ERR_HDMI_BASE + 0x015U)

/*                 */
#define TMBSL_ERR_HDMI_I2C_READ                 (TMBSL_ERR_HDMI_BASE + 0x017U)

/*                  */
#define TMBSL_ERR_HDMI_I2C_WRITE                (TMBSL_ERR_HDMI_BASE + 0x018U)

/*                    */
#define TMBSL_ERR_HDMI_ASSERTION                (TMBSL_ERR_HDMI_BASE + 0x049U)

/*                          */
#define TMBSL_ERR_HDMI_INVALID_STATE            (TMBSL_ERR_HDMI_BASE + 0x066U)
#define TMBSL_ERR_HDMI_INVALID_CHECKSUM         TMBSL_ERR_HDMI_INVALID_STATE

/*                           */
#define TMBSL_ERR_HDMI_NULL_CONNECTION          (TMBSL_ERR_HDMI_BASE + 0x067U)

/*                          */
#define TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED  (TMBSL_ERR_HDMI_BASE + 0x068U)

/*                             */
#define TMBSL_ERR_HDMI_MAX              TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED

/*                                                                            */

#define HDMITX_ENABLE_VP_TABLE_LEN	3
#define HDMITX_GROUND_VP_TABLE_LEN	3

/*                  */
#define EDID_BLOCK_SIZE		    128

/*                                              */
#define EDID_MONITOR_DESCRIPTOR_SIZE   13

/*  */


/*                                                                            */
/*                                                                            */
/*                                                                            */

/* 
                          
*/
	typedef enum {
		BSLHDMITX_UNKNOWN = 0x00,
			       /*                           */
		BSLHDMITX_TDA9984,
			       /*                           */
		BSLHDMITX_TDA9989,
			       /*                                                */
		BSLHDMITX_TDA9981,
			       /*                           */
		BSLHDMITX_TDA9983,
			       /*                           */
		BSLHDMITX_TDA19989,
			       /*                           */
		BSLHDMITX_TDA19988,
			       /*                                  */
	} tmbslHdmiTxVersion_t;


/* 
                                                                             
                                             
                                                                          
                                                                               
                                                                              
                                        
                                                        
                                                                       
                                                                       
 */
	typedef struct _tmbslHdmiTxSysArgs_t {
		UInt8 slaveAddr;
		UInt8 firstRegister;
		UInt8 lenData;
		UInt8 *pData;
	} tmbslHdmiTxSysArgs_t;
	typedef tmErrorCode_t(*ptmbslHdmiTxSysFunc_t) (tmbslHdmiTxSysArgs_t *pSysArgs);

/* 
                                                                           
                                                                 
                                                         
                                                               
                                                              
                                                                          
                                  
                                                                       
                                        
                                                        
                                                                       
                                                                       
 */
	typedef struct _tmbslHdmiTxSysArgsEdid_t {
		UInt8 segPtrAddr;
		UInt8 segPtr;
		UInt8 dataRegAddr;
		UInt8 wordOffset;
		UInt8 lenData;
		UInt8 *pData;
	} tmbslHdmiTxSysArgsEdid_t;


/* 
                                                                            
                                                                                         
 */
	typedef tmErrorCode_t(*ptmbslHdmiTxSysFuncEdid_t)
	 (tmbslHdmiTxSysArgsEdid_t *pSysArgs);

/* 
                                                                   
                                  
 */
	typedef Void(*ptmbslHdmiTxSysFuncTimer_t) (UInt16 ms);

/*                                                                            */
/* 
                                                                          
                                                                 
 */
	typedef Void(*ptmbslHdmiTxCallback_t) (tmUnitSelect_t txUnit);

/*                                                                            */
/* 
                                 
 */
	typedef enum {
		HDMITX_VFMT_NULL = 0,	    /*                                */
		HDMITX_VFMT_NO_CHANGE = 0,  /*                                */
		HDMITX_VFMT_TV_MIN = 1,	    /*                                */
		HDMITX_VFMT_01_640x480p_60Hz = 1,
					    /*                                */
		HDMITX_VFMT_02_720x480p_60Hz = 2,
					    /*                                */
		HDMITX_VFMT_03_720x480p_60Hz = 3,
					    /*                                */
		HDMITX_VFMT_04_1280x720p_60Hz = 4,
					    /*                                */
		HDMITX_VFMT_05_1920x1080i_60Hz = 5,
					    /*                                */
		HDMITX_VFMT_06_720x480i_60Hz = 6,
					    /*                                */
		HDMITX_VFMT_07_720x480i_60Hz = 7,
					    /*                                */
		HDMITX_VFMT_08_720x240p_60Hz = 8,
					    /*                                */
		HDMITX_VFMT_09_720x240p_60Hz = 9,
					    /*                                */
		HDMITX_VFMT_10_720x480i_60Hz = 10,
					    /*                                */
		HDMITX_VFMT_11_720x480i_60Hz = 11,
					    /*                                */
		HDMITX_VFMT_12_720x240p_60Hz = 12,
					    /*                                */
		HDMITX_VFMT_13_720x240p_60Hz = 13,
					    /*                                */
		HDMITX_VFMT_14_1440x480p_60Hz = 14,
					    /*                                */
		HDMITX_VFMT_15_1440x480p_60Hz = 15,
					    /*                                */
		HDMITX_VFMT_16_1920x1080p_60Hz = 16,
					    /*                                */
		HDMITX_VFMT_17_720x576p_50Hz = 17,
					    /*                                */
		HDMITX_VFMT_18_720x576p_50Hz = 18,
					    /*                                */
		HDMITX_VFMT_19_1280x720p_50Hz = 19,
					    /*                                */
		HDMITX_VFMT_20_1920x1080i_50Hz = 20,
					    /*                                */
		HDMITX_VFMT_21_720x576i_50Hz = 21,
					    /*                                */
		HDMITX_VFMT_22_720x576i_50Hz = 22,
					    /*                                */
		HDMITX_VFMT_23_720x288p_50Hz = 23,
					    /*                                */
		HDMITX_VFMT_24_720x288p_50Hz = 24,
					    /*                                */
		HDMITX_VFMT_25_720x576i_50Hz = 25,
					    /*                                */
		HDMITX_VFMT_26_720x576i_50Hz = 26,
					    /*                                */
		HDMITX_VFMT_27_720x288p_50Hz = 27,
					    /*                                */
		HDMITX_VFMT_28_720x288p_50Hz = 28,
					    /*                                */
		HDMITX_VFMT_29_1440x576p_50Hz = 29,
					    /*                                */
		HDMITX_VFMT_30_1440x576p_50Hz = 30,
					    /*                                */
		HDMITX_VFMT_31_1920x1080p_50Hz = 31,
					    /*                                */
		HDMITX_VFMT_32_1920x1080p_24Hz = 32,
					    /*                                */

		HDMITX_VFMT_TV_NO_REG_MIN = 32,
					    /*                                             */

		HDMITX_VFMT_33_1920x1080p_25Hz = 33,
					    /*                                */
		HDMITX_VFMT_34_1920x1080p_30Hz = 34,
					    /*                                */
		HDMITX_VFMT_35_2880x480p_60Hz = 35,
					   /*                                    */
		HDMITX_VFMT_36_2880x480p_60Hz = 36,
					   /*                                    */
		HDMITX_VFMT_37_2880x576p_50Hz = 37,
					   /*                                    */
		HDMITX_VFMT_38_2880x576p_50Hz = 38,
					   /*                                    */

		HDMITX_VFMT_INDEX_60_1280x720p_24Hz = 39,
					     /*                                          */
		HDMITX_VFMT_60_1280x720p_24Hz = 60,
					   /*                                          */
		HDMITX_VFMT_61_1280x720p_25Hz = 61,
					   /*                                    */
		HDMITX_VFMT_62_1280x720p_30Hz = 62,
					   /*                                          */

		HDMITX_VFMT_TV_MAX = 62,    /*                                      */
		HDMITX_VFMT_TV_NUM = 42,    /*                                                               */

		HDMITX_VFMT_PC_MIN = 128,   /*                                */
		HDMITX_VFMT_PC_640x480p_60Hz = 128,
					    /*                                */
		HDMITX_VFMT_PC_800x600p_60Hz = 129,
					    /*                                */
		HDMITX_VFMT_PC_1152x960p_60Hz = 130,
					    /*                                */
		HDMITX_VFMT_PC_1024x768p_60Hz = 131,
					    /*                                */
		HDMITX_VFMT_PC_1280x768p_60Hz = 132,
					    /*                                */
		HDMITX_VFMT_PC_1280x1024p_60Hz = 133,
					    /*                                */
		HDMITX_VFMT_PC_1360x768p_60Hz = 134,
					    /*                                */
		HDMITX_VFMT_PC_1400x1050p_60Hz = 135,
					    /*                                */
		HDMITX_VFMT_PC_1600x1200p_60Hz = 136,
					    /*                                */
		HDMITX_VFMT_PC_1024x768p_70Hz = 137,
					    /*                                */
		HDMITX_VFMT_PC_640x480p_72Hz = 138,
					    /*                                */
		HDMITX_VFMT_PC_800x600p_72Hz = 139,
					    /*                                */
		HDMITX_VFMT_PC_640x480p_75Hz = 140,
					    /*                                */
		HDMITX_VFMT_PC_1024x768p_75Hz = 141,
					    /*                                */
		HDMITX_VFMT_PC_800x600p_75Hz = 142,
					    /*                                */
		HDMITX_VFMT_PC_1024x864p_75Hz = 143,
					    /*                                */
		HDMITX_VFMT_PC_1280x1024p_75Hz = 144,
					    /*                                */
		HDMITX_VFMT_PC_640x350p_85Hz = 145,
					    /*                                */
		HDMITX_VFMT_PC_640x400p_85Hz = 146,
					    /*                                */
		HDMITX_VFMT_PC_720x400p_85Hz = 147,
					    /*                                */
		HDMITX_VFMT_PC_640x480p_85Hz = 148,
					    /*                                */
		HDMITX_VFMT_PC_800x600p_85Hz = 149,
					    /*                                */
		HDMITX_VFMT_PC_1024x768p_85Hz = 150,
					    /*                                */
		HDMITX_VFMT_PC_1152x864p_85Hz = 151,
					    /*                                */
		HDMITX_VFMT_PC_1280x960p_85Hz = 152,
					    /*                                */
		HDMITX_VFMT_PC_1280x1024p_85Hz = 153,
					    /*                                */
		HDMITX_VFMT_PC_1024x768i_87Hz = 154,
					    /*                                */
		HDMITX_VFMT_PC_MAX = 154,   /*                                */
		HDMITX_VFMT_PC_NUM = (1 + 154 - 128)
						/*                                */
	} tmbslHdmiTxVidFmt_t;

/*                                                                            */
/* 
                                                 
 */
/*                      */
	typedef enum {
		HDMITX_AFMT_SPDIF = 0,	/*                       */
		HDMITX_AFMT_I2S = 1,	/*                       */
		HDMITX_AFMT_OBA = 2,	/*                       */
		HDMITX_AFMT_DST = 3,	/*                       */
		HDMITX_AFMT_HBR = 4,	/*                       */
		HDMITX_AFMT_NUM = 5,	/*                                */
		HDMITX_AFMT_INVALID = 5	/*                       */
	} tmbslHdmiTxaFmt_t;

/*              */
	typedef enum {
		HDMITX_I2SFOR_PHILIPS_L = 0,
					/*                       */
		HDMITX_I2SFOR_OTH_L = 2,/*                                    */
		HDMITX_I2SFOR_OTH_R = 3,/*                                     */
		HDMITX_I2SFOR_INVALID = 4
					/*                 */
	} tmbslHdmiTxI2sFor_t;

/*                       */
	typedef enum {
		HDMITX_CLKPOLDSD_ACLK = 0,
					/*                */
		HDMITX_CLKPOLDSD_NACLK = 1,
					/*                           */
		HDMITX_CLKPOLDSD_NO_CHANGE = 2,
					/*                */
		HDMITX_CLKPOLDSD_INVALID = 3
					/*                */
	} tmbslHdmiTxClkPolDsd_t;

/*                       */
	typedef enum {
		HDMITX_SWAPDSD_OFF = 0,	/*             */
		HDMITX_SWAPDSD_ON = 1,	/*             */
		HDMITX_SWAPDSD_NO_CHANGE = 2,
					/*             */
		HDMITX_SWAPDSD_INVALID = 3
					/*             */
	} tmbslHdmiTxSwapDsd_t;

/*                          */
	typedef enum {
		HDMITX_DSTRATE_SINGLE = 0,
					/*                        */
		HDMITX_DSTRATE_DOUBLE = 1,
					/*                    */
		HDMITX_DSTRATE_NO_CHANGE = 2,
					/*             */
		HDMITX_DSTRATE_INVALID = 3
					/*             */
	} tmbslHdmiTxDstRate_t;

/*                                               */
	enum _tmbslHdmiTxChan {
		HDMITX_CHAN_MIN = 0,
		HDMITX_CHAN_MAX = 31,
		HDMITX_CHAN_NO_CHANGE = 32,
		HDMITX_CHAN_INVALID = 33
	};

/*                      */
	enum _tmbslHdmiTxLayout {
		HDMITX_LAYOUT_MIN = 0,
		HDMITX_LAYOUT_MAX = 1,
		HDMITX_LAYOUT_NO_CHANGE = 2,
		HDMITX_LAYOUT_INVALID = 3
	};

/*                                 */
	enum _tmbslHdmiTxlatency_rd {
		HDMITX_LATENCY_MIN = 0x000,
		HDMITX_LATENCY_CURRENT = 0x080,
		HDMITX_LATENCY_MAX = 0x0FF,
		HDMITX_LATENCY_NO_CHANGE = 0x100,
		HDMITX_LATENCY_INVALID = 0x101
	};

/*                                                                            */
/* 
                                              
 */
/*                                    */
	typedef enum {
		HDMITX_CTSREF_ACLK = 0,
				    /*                                 */
		HDMITX_CTSREF_MCLK = 1,
				    /*                                 */
		HDMITX_CTSREF_FS64SPDIF = 2,
				    /*                                 */
		HDMITX_CTSREF_INVALID = 3
				    /*                                 */
	} tmbslHdmiTxctsRef_t;

/*                                */
	typedef enum {
		HDMITX_AFS_32k = 0,   /*            */
		HDMITX_AFS_44_1k = 1, /*            */
		HDMITX_AFS_48K = 2,   /*            */
		HDMITX_AFS_88_2K = 3, /*            */
		HDMITX_AFS_96K = 4,   /*            */
		HDMITX_AFS_176_4K = 5,/*            */
		HDMITX_AFS_192K = 6,  /*            */
		HDMITX_AFS_768K = 7,  /*            */
		HDMITX_AFS_NOT_INDICATED = 8,
				      /*                                  */
		HDMITX_AFS_INVALID = 8,
				      /*            */
		HDMITX_AFS_NUM = 8    /*            */
	} tmbslHdmiTxafs_t;

/*                              */
	typedef enum {
		HDMITX_VFREQ_24Hz = 0,
				    /*                 */
		HDMITX_VFREQ_25Hz = 1,	/*                 */
		HDMITX_VFREQ_30Hz = 2,	/*                 */
		HDMITX_VFREQ_50Hz = 3,
				    /*                 */
		HDMITX_VFREQ_59Hz = 4,
				    /*                 */
		HDMITX_VFREQ_60Hz = 5,
				    /*                 */
#ifndef FORMAT_PC
		HDMITX_VFREQ_INVALID = 6,
				    /*                 */
		HDMITX_VFREQ_NUM = 6/*                 */
#else				/*           */
		HDMITX_VFREQ_70Hz = 6,
				    /*                 */
		HDMITX_VFREQ_72Hz = 7,
				    /*                 */
		HDMITX_VFREQ_75Hz = 8,
				    /*                 */
		HDMITX_VFREQ_85Hz = 9,
				    /*                 */
		HDMITX_VFREQ_87Hz = 10,
				    /*                 */
		HDMITX_VFREQ_INVALID = 11,
				    /*                 */
		HDMITX_VFREQ_NUM = 11
				    /*                 */
#endif				/*           */
	} tmbslHdmiTxVfreq_t;

/*                                         */
	typedef enum {
		HDMITX_CTSK1 = 0,   /*       */
		HDMITX_CTSK2 = 1,   /*       */
		HDMITX_CTSK3 = 2,   /*       */
		HDMITX_CTSK4 = 3,   /*       */
		HDMITX_CTSK8 = 4,   /*       */
		HDMITX_CTSK_USE_CTSX = 5,
				    /*                              */
		HDMITX_CTSK_INVALID = 6
				    /*           */
	} tmbslHdmiTxctsK_t;

/*                                                   */
	typedef enum {
		HDMITX_CTSMTS = 0,  /*          */
		HDMITX_CTSMTS2 = 1, /*          */
		HDMITX_CTSMTS4 = 2, /*          */
		HDMITX_CTSMTS8 = 3, /*          */
		HDMITX_CTSMTS_USE_CTSX = 4,
				    /*                              */
		HDMITX_CTSMTS_INVALID = 5
				    /*           */
	} tmbslHdmiTxctsM_t;

/*                          */
	enum _tmbslHdmiTxCts {
		HDMITX_CTS_AUTO = 0,
		HDMITX_CTS_MIN = 0x000001
	};

/*                             */
	enum _tmbslHdmiTxCtsX {
		HDMITX_CTSX_16 = 0,
		HDMITX_CTSX_32 = 1,
		HDMITX_CTSX_48 = 2,
		HDMITX_CTSX_64 = 3,
		HDMITX_CTSX_128 = 4,
		HDMITX_CTSX_NUM = 5,
		HDMITX_CTSX_UNUSED = 5,
				    /*                                        */
		HDMITX_CTSX_INVALID = 6
	};


/*                                                                            */
/* 
                                                      
 */

	typedef enum {
		HDMITX_AUDIO_DATA_PCM = 0,
				  /*                                                     */
		HDMITX_AUDIO_DATA_OTHER = 1,
				  /*                                                     */
		HDMITX_AUDIO_DATA_INVALID = 2
				  /*                 */
	} tmbslHdmiTxAudioData_t;

/*                                            */
	typedef enum {
		HDMITX_CSFI_PCM_2CHAN_NO_PRE = 0,
					  /*                                                 */
		HDMITX_CSFI_PCM_2CHAN_PRE = 1,
					  /*                                                 */
		HDMITX_CSFI_PCM_2CHAN_PRE_RSVD1 = 2,
					  /*                                                 */
		HDMITX_CSFI_PCM_2CHAN_PRE_RSVD2 = 3,
					  /*                                                 */
		HDMITX_CSFI_NOTPCM_DEFAULT = 4,
					  /*                                                 */
		HDMITX_CSFI_INVALID = 5	  /*                                                 */
	} tmbslHdmiTxCSformatInfo_t;

/** BYTE 0: Channel Status Copyright assertion */
	typedef enum {
		HDMITX_CSCOPYRIGHT_PROTECTED = 0,
					 /**< Copyright protected     */
		HDMITX_CSCOPYRIGHT_UNPROTECTED = 1,
					 /**< Not copyright protected */
		HDMITX_CSCOPYRIGHT_INVALID = 2
					 /*                           */
	} tmbslHdmiTxCScopyright_t;

/*                                        */
	typedef enum {
		HDMITX_CSCLK_LEVEL_II = 0,
				      /*                                */
		HDMITX_CSCLK_LEVEL_I = 1,
				      /*                                */
		HDMITX_CSCLK_LEVEL_III = 2,
				      /*                                */
		HDMITX_CSCLK_NOT_MATCHED = 3,
				      /*                                */
		HDMITX_CSCLK_INVALID = 4
				      /*                                */
	} tmbslHdmiTxCSclkAcc_t;

/*                                                    */
	typedef enum {
		HDMITX_CSMAX_LENGTH_20 = 0,
				   /*                                */
		HDMITX_CSMAX_LENGTH_24 = 1,
				   /*                                */
		HDMITX_CSMAX_INVALID = 2
				   /*                                */
	} tmbslHdmiTxCSmaxWordLength_t;


/*                                            */
	typedef enum {
		HDMITX_CSWORD_DEFAULT = 0,  /*                                                   */
		HDMITX_CSWORD_20_OF_24 = 1, /*                                                   */
		HDMITX_CSWORD_16_OF_20 = 1, /*                                                   */
		HDMITX_CSWORD_22_OF_24 = 2, /*                                                   */
		HDMITX_CSWORD_18_OF_20 = 2, /*                                                   */
		HDMITX_CSWORD_RESVD = 3,    /*                                */
		HDMITX_CSWORD_23_OF_24 = 4, /*                                                   */
		HDMITX_CSWORD_19_OF_20 = 4, /*                                                   */
		HDMITX_CSWORD_24_OF_24 = 5, /*                                                   */
		HDMITX_CSWORD_20_OF_20 = 5, /*                                                   */
		HDMITX_CSWORD_21_OF_24 = 6, /*                                                   */
		HDMITX_CSWORD_17_OF_20 = 6, /*                                                   */
		HDMITX_CSWORD_INVALID = 7   /*           */
	} tmbslHdmiTxCSwordLength_t;

/*                                                   */
	typedef enum {
		HDMITX_CSOFREQ_NOT_INDICATED = 0,
					/*                 */
		HDMITX_CSOFREQ_192k = 1,/*                 */
		HDMITX_CSOFREQ_12k = 2,	/*                 */
		HDMITX_CSOFREQ_176_4k = 3,
					/*                 */
		HDMITX_CSOFREQ_RSVD1 = 4,
					/*                 */
		HDMITX_CSOFREQ_96k = 5,	/*                 */
		HDMITX_CSOFREQ_8k = 6,	/*                 */
		HDMITX_CSOFREQ_88_2k = 7,
					/*                 */
		HDMITX_CSOFREQ_16k = 8,	/*                 */
		HDMITX_CSOFREQ_24k = 9,	/*                 */
		HDMITX_CSOFREQ_11_025k = 10,
					/*                 */
		HDMITX_CSOFREQ_22_05k = 11,
					/*                 */
		HDMITX_CSOFREQ_32k = 12,/*                 */
		HDMITX_CSOFREQ_48k = 13,/*                 */
		HDMITX_CSOFREQ_RSVD2 = 14,
					/*                 */
		HDMITX_CSOFREQ_44_1k = 15,
					/*                 */
		HDMITX_CSAFS_INVALID = 16
					/*                 */
	} tmbslHdmiTxCSorigAfs_t;

/*                                                                            */
/* 
                                                             
 */
/*                                              */
	enum _tmbslHdmiTxChanStatusChanLimits {
		HDMITX_CS_CHANNELS_MAX = 0x0F,
		HDMITX_CS_SOURCES_MAX = 0x0F
	};

/*                                                                            */
/* 
                                               
 */
/*                   */
	typedef enum {
		HDMITX_AMUTE_OFF = 0,
				   /*            */
		HDMITX_AMUTE_ON = 1,
				   /*            */
		HDMITX_AMUTE_INVALID = 2
				   /*            */
	} tmbslHdmiTxaMute_t;

/*                                                                */
#define HDMI_TX_SAD_MAX_CNT     30

/*                                                                            */
/* 
                                                 
 */
/*                                               */
	enum _tmbslHdmiTxEdidSta_t {
		HDMITX_EDID_READ = 0,	/*                    */
		HDMITX_EDID_READ_INCOMPLETE = 1,	/*                                  
                                      */
		HDMITX_EDID_ERROR_CHK_BLOCK_0 = 2,	/*                        */

		HDMITX_EDID_ERROR_CHK = 3,	/*                                  
                              */
		HDMITX_EDID_NOT_READ = 4,	/*               */

		HDMITX_EDID_STATUS_INVALID = 5
					    /*             */
	};

/*                                                                            */
/* 
                                                
 */
/*                   */
	typedef enum {
		HDMITX_SINK_DVI = 0,	   /*        */
		HDMITX_SINK_HDMI = 1,	   /*        */
		HDMITX_SINK_EDID = 2,	   /*                                */
		HDMITX_SINK_INVALID = 3	   /*             */
	} tmbslHdmiTxSinkType_t;

/*                                                                            */
/* 
                                                                
                                          
 */
	typedef struct _tmbslHdmiTxEdidDtd_t {
		UInt16 uPixelClock;
				/*                              */
		UInt16 uHActivePixels;
				/*                              */
		UInt16 uHBlankPixels;
				/*                              */
		UInt16 uVActiveLines;
				/*                              */
		UInt16 uVBlankLines;
				/*                              */
		UInt16 uHSyncOffset;
				/*                              */
		UInt16 uHSyncWidth;
				/*                              */
		UInt16 uVSyncOffset;
				/*                              */
		UInt16 uVSyncWidth;
				/*                              */
		UInt16 uHImageSize;
				/*                              */
		UInt16 uVImageSize;
				/*                              */
		UInt16 uHBorderPixels;
				/*                              */
		UInt16 uVBorderPixels;
				/*                              */
		UInt8 Flags;	/*                              */
	} tmbslHdmiTxEdidDtd_t;


/*                                                                            */
/* 
                                     
 */
	typedef struct _tmbslHdmiTxEdidFirstMD_t {
		Bool bDescRecord;			    /*                                                  */
		UInt8 uMonitorName[EDID_MONITOR_DESCRIPTOR_SIZE];
							    /*                                                  */
	} tmbslHdmiTxEdidFirstMD_t;

/*                                                                            */
/* 
                                      
 */
	typedef struct _tmbslHdmiTxEdidSecondMD_t {
		Bool bDescRecord;			    /*                                                  */
		UInt8 uMinVerticalRate;			    /*                                                  */
		UInt8 uMaxVerticalRate;			    /*                                                  */
		UInt8 uMinHorizontalRate;		    /*                                                  */
		UInt8 uMaxHorizontalRate;		    /*                                                  */
		UInt8 uMaxSupportedPixelClk;		    /*                                                  */
	} tmbslHdmiTxEdidSecondMD_t;

/*                                                                            */
/* 
                                     
 */
	typedef struct _tmbslHdmiTxEdidOtherMD_t {
		Bool bDescRecord;			    /*                                                  */
		UInt8 uOtherDescriptor[EDID_MONITOR_DESCRIPTOR_SIZE];
							    /*                                                  */
	} tmbslHdmiTxEdidOtherMD_t;

/*                                                                            */
/* 
                                     
 */
	typedef struct _tmbslHdmiTxEdidBDParam_t {
		UInt8 uVideoInputDef;
				 /*                                                */
		UInt8 uMaxHorizontalSize;
				 /*                                                */
		UInt8 uMaxVerticalSize;
				 /*                                                */
		UInt8 uGamma;	 /*                                                */
		UInt8 uFeatureSupport;
				 /*                                                */
	} tmbslHdmiTxEdidBDParam_t;

/*                                                                            */
/* 
                                                                   
 */
	typedef struct _tmbslHdmiTxEdidSad_t {
		UInt8 ModeChans;	/*                                                 */
		UInt8 Freqs;	/*                                   */
		UInt8 Byte3;	/*                                               */
	} tmbslHdmiTxEdidSad_t;

/*                                                                            */
/* 
                                                                                         
 */
	typedef struct _tmbslHdmiTxEdidToApp_t {
		UInt8 *pRawEdid;	/*                                                         */
		Int numBlocks;	/*                                               */
	} tmbslHdmiTxEdidToApp_t;

/*                                                                            */
/* 
                                                          
 */
/*                                                                */
#define HDMI_TX_SVD_MAX_CNT     113

/*                                                     */
#define NUMBER_DTD_STORED       10



/*                                                               */
#define HDMI_TX_SVD_NATIVE_MASK 0x80
#define HDMI_TX_SVD_NATIVE_NOT  0x7F

/*                         */
	enum _tmbslHdmiTxVidCap_t {
		HDMITX_VIDCAP_UNDERSCAN = 0x80,
					/*                       */
		HDMITX_VIDCAP_YUV444 = 0x40,
					/*                         */
		HDMITX_VIDCAP_YUV422 = 0x20,
					/*                         */
		HDMITX_VIDCAP_UNUSED = 0x1F
					/*                */
	};

/*                                                                            */
/* 
                                         
 */
/*                    */
	typedef enum {
		HDMITX_HDCP_CHECK_NOT_STARTED = 0,
						/*                     */
		HDMITX_HDCP_CHECK_IN_PROGRESS = 1,
						/*                           */
		HDMITX_HDCP_CHECK_PASS = 2,	/*                          */
		HDMITX_HDCP_CHECK_FAIL_FIRST = 3,
						/*                            */
		HDMITX_HDCP_CHECK_FAIL_DRIVER_STATE = 3,
						/*                            */
		HDMITX_HDCP_CHECK_FAIL_DEVICE_T0 = 4,
						/*                           */
		HDMITX_HDCP_CHECK_FAIL_DEVICE_RI = 5,
						/*                     */
		HDMITX_HDCP_CHECK_FAIL_DEVICE_FSM = 6,
						/*                      */
		HDMITX_HDCP_CHECK_NUM = 7	/*                           */
	} tmbslHdmiTxHdcpCheck_t;

/*                                                                            */
/* 
                                             
 */
/*                           */
	enum _tmbslHdmiTxHdcpSlaveAddress {
		HDMITX_HDCP_SLAVE_PRIMARY = 0x74,
		HDMITX_HDCP_SLAVE_SECONDARY = 0x76
	};

/*                         */
	typedef enum {
		HDMITX_HDCP_TXMODE_NOT_SET = 0,
		HDMITX_HDCP_TXMODE_REPEATER = 1,
		HDMITX_HDCP_TXMODE_TOP_LEVEL = 2,
		HDMITX_HDCP_TXMODE_MAX = 2
	} tmbslHdmiTxHdcpTxMode_t;

/*                    */
	typedef enum {
		HDMITX_HDCP_OPTION_FORCE_PJ_IGNORED = 0x01,	/*                             */
		HDMITX_HDCP_OPTION_FORCE_SLOW_DDC = 0x02,	/*                             */
		HDMITX_HDCP_OPTION_FORCE_NO_1_1 = 0x04,	/*                             */
		HDMITX_HDCP_OPTION_FORCE_REPEATER = 0x08,	/*                             */
		HDMITX_HDCP_OPTION_FORCE_NO_REPEATER = 0x10,	/*                             */
		HDMITX_HDCP_OPTION_FORCE_V_EQU_VBAR = 0x20,	/*                             */
		HDMITX_HDCP_OPTION_FORCE_VSLOW_DDC = 0x40,	/*                */
		HDMITX_HDCP_OPTION_DEFAULT = 0x00,	/*                            */
		HDMITX_HDCP_OPTION_MASK = 0x7F,	/*                             */
		HDMITX_HDCP_OPTION_MASK_BAD = 0x80	/*                             */
	} tmbslHdmiTxHdcpOptions_t;

/*                                                                            */
/* 
                                                
 */
/*                       */
	typedef enum {
		HDMITX_HDCP_DECRYPT_DISABLE = 0,
		HDMITX_HDCP_DECRYPT_ENABLE = 1,
		HDMITX_HDCP_DECRYPT_MAX = 1
	} tmbslHdmiTxDecrypt_t;

/*                                                                            */
/* 
                                                 
 */
/*                     */
	enum _tmbslHdmiTxHdcpHandleBSTATUS {
		HDMITX_HDCP_BSTATUS_HDMI_MODE = 0x1000,
		HDMITX_HDCP_BSTATUS_MAX_CASCADE_EXCEEDED = 0x0800,
		HDMITX_HDCP_BSTATUS_CASCADE_DEPTH = 0x0700,
		HDMITX_HDCP_BSTATUS_MAX_DEVS_EXCEEDED = 0x0080,
		HDMITX_HDCP_BSTATUS_DEVICE_COUNT = 0x007F
	};

/*                                                                            */
/* 
                                                
 */
/*                 */
	enum _tmbslHdmiTxHdcpHandleSHA_1 {
		HDMITX_KSV_LIST_MAX_DEVICES = 128,
		HDMITX_KSV_BYTES_PER_DEVICE = 5
	};

/*                                                                            */
/* 
                                                
 */
/*                         */
	typedef enum {
		HDMITX_HOTPLUG_INACTIVE = 0,
				       /*                    */
		HDMITX_HOTPLUG_ACTIVE = 1,
				       /*                    */
		HDMITX_HOTPLUG_INVALID = 2
				       /*                    */
	} tmbslHdmiTxHotPlug_t;

/*                                                                            */
/* 
                                                
 */
/*                          */
	typedef enum {
		HDMITX_RX_SENSE_INACTIVE = 0,
					/*                    */
		HDMITX_RX_SENSE_ACTIVE = 1,
					/*                    */
		HDMITX_RX_SENSE_INVALID = 2
					/*                    */
	} tmbslHdmiTxRxSense_t;

/*                                                                            */
/* 
                                                 
 */
/*                                                  */
	typedef enum {
		HDMITX_FEATURE_HW_HDCP = 0,  /*                  */
		HDMITX_FEATURE_HW_SCALER = 1,/*                  */
		HDMITX_FEATURE_HW_AUDIO_OBA = 2,
					     /*                         */
		HDMITX_FEATURE_HW_AUDIO_DST = 3,
					     /*                     */
		HDMITX_FEATURE_HW_AUDIO_HBR = 4,
					     /*                     */
		HDMITX_FEATURE_HW_HDMI_1_1 = 5,
					     /*                    */
		HDMITX_FEATURE_HW_HDMI_1_2A = 6,
					     /*                     */
		HDMITX_FEATURE_HW_HDMI_1_3A = 7,
					     /*                     */
		HDMITX_FEATURE_HW_DEEP_COLOR_30 = 8,
					     /*                              */
		HDMITX_FEATURE_HW_DEEP_COLOR_36 = 9,
					     /*                              */
		HDMITX_FEATURE_HW_DEEP_COLOR_48 = 11,
					     /*                              */
		HDMITX_FEATURE_HW_UPSAMPLER = 12,
					     /*                      */
		HDMITX_FEATURE_HW_DOWNSAMPLER = 13,
					     /*                        */
		HDMITX_FEATURE_HW_COLOR_CONVERSION = 14
					     /*                           */
	} tmbslHdmiTxHwFeature_t;

/*                                                                            */
/* 
                                     
 */
/*                                         */
	enum _tmbslHdmiTxSlaveAddress {
		HDMITX_SLAVE_ADDRESS_MIN = 1,
		HDMITX_SLAVE_ADDRESS_MAX = 127
	};

/* 
                                                                                
 */
	typedef enum _tmbslHdmiTxCallbackInt {
		HDMITX_CALLBACK_INT_SECURITY = 0,
					      /*                                    */
		HDMITX_CALLBACK_INT_ENCRYPT = 0,
					      /*                                    */
		HDMITX_CALLBACK_INT_HPD = 1,  /*                                    */
		HDMITX_CALLBACK_INT_T0 = 2,   /*                                    */
		HDMITX_CALLBACK_INT_BCAPS = 3,/*                                    */
		HDMITX_CALLBACK_INT_BSTATUS = 4,
					      /*                                    */
		HDMITX_CALLBACK_INT_SHA_1 = 5,/*                                    */
		HDMITX_CALLBACK_INT_PJ = 6,   /*                                    */
		HDMITX_CALLBACK_INT_R0 = 7,   /*                                    */
		HDMITX_CALLBACK_INT_SW_INT = 8,
					      /*                                    */
		HDMITX_CALLBACK_INT_RX_SENSE = 9,
					      /*                                    */
		HDMITX_CALLBACK_INT_EDID_BLK_READ = 10,
					      /*                                    */
		HDMITX_CALLBACK_INT_PLL_LOCK = 11,
					      /*                                    */
		HDMITX_CALLBACK_INT_VS_RPT = 12,
					      /*                                    */
		HDMITX_CALLBACK_INT_NUM = 13  /*                                    */
	} tmbslHdmiTxCallbackInt_t;

/*             */
	typedef enum {
		HDMITX_PIXRATE_DOUBLE = 0,	    /*                     */
		HDMITX_PIXRATE_SINGLE = 1,	    /*                     */
		HDMITX_PIXRATE_SINGLE_REPEATED = 2, /*                         */
		HDMITX_PIXRATE_NO_CHANGE = 3,	    /*             */
		HDMITX_PIXRATE_INVALID = 4	    /*             */
	} tmbslHdmiTxPixRate_t;

/* 
                                                    
 */
	typedef struct _tmbslHdmiTxCallbackList_t {
    /*                                                                     */
		ptmbslHdmiTxCallback_t funcCallback[HDMITX_CALLBACK_INT_NUM];

	} tmbslHdmiTxCallbackList_t;

/*                                                                            */
/* 
                                               
 */
/*                                 */
	enum _tmbslHdmiTxMatCoeff {
		HDMITX_MAT_COEFF_NUM = 9
	};


/*                                                               */
/*                                                */
	typedef struct _tmbslHdmiTxMatCoeff_t {
    /*                                                */
		Int16 Coeff[HDMITX_MAT_COEFF_NUM];
	} tmbslHdmiTxMatCoeff_t;

/*                                                                            */
/* 
                                                   
 */
/*                   */
	typedef enum {
		HDMITX_VINMODE_CCIR656 = 0,
				       /*            */
		HDMITX_VINMODE_RGB444 = 1,
				       /*             */
		HDMITX_VINMODE_YUV444 = 2,
				       /*             */
		HDMITX_VINMODE_YUV422 = 3,
				       /*             */
		HDMITX_VINMODE_NO_CHANGE = 4,
				       /*             */
		HDMITX_VINMODE_INVALID = 5
				       /*             */
	} tmbslHdmiTxVinMode_t;

/*                    */
	typedef enum {
		HDMITX_VOUTMODE_RGB444 = 0,
					/*             */
		HDMITX_VOUTMODE_YUV422 = 1,
					/*             */
		HDMITX_VOUTMODE_YUV444 = 2,
					/*             */
		HDMITX_VOUTMODE_NO_CHANGE = 3,
					/*             */
		HDMITX_VOUTMODE_INVALID = 4
					/*             */
	} tmbslHdmiTxVoutMode_t;

/*                                                                            */
/* 
                                              
 */
/*                         */
	typedef enum {
		HDMITX_MCNTRL_ON = 0,
				   /*              */
		HDMITX_MCNTRL_OFF = 1,
				   /*              */
		HDMITX_MCNTRL_NO_CHANGE = 2,
				   /*                    */
		HDMITX_MCNTRL_MAX = 2,
				   /*              */
		HDMITX_MCNTRL_INVALID = 3
				   /*              */
	} tmbslHdmiTxmCntrl_t;

/*                       */
	typedef enum {
		HDMITX_MSCALE_256 = 0,
				   /*                 */
		HDMITX_MSCALE_512 = 1,
				   /*                 */
		HDMITX_MSCALE_1024 = 2,
				   /*                 */
		HDMITX_MSCALE_NO_CHANGE = 3,
				   /*                    */
		HDMITX_MSCALE_MAX = 3,
				   /*                 */
		HDMITX_MSCALE_INVALID = 4
				   /*                 */
	} tmbslHdmiTxmScale_t;

/*                                                                            */
/* 
                               
 */
/*                                  */
	enum _tmbslHdmiTxPkt {
		HDMITX_PKT_DATA_BYTE_CNT = 28
	};

/*                                                             */
	typedef struct _tmbslHdmiTxPkt_t {
		UInt8 dataByte[HDMITX_PKT_DATA_BYTE_CNT];
						    /*                 */
	} tmbslHdmiTxPkt_t;

/*                                                                            */
/* 
                                                 
 */
	typedef struct _tmbslHdmiTxPktAif_t {
		UInt8 CodingType;
			    /*                        */
		UInt8 ChannelCount;
			    /*                          */
		UInt8 SampleFreq;
			    /*                             */
		UInt8 SampleSize;
			    /*                        */
		UInt8 ChannelAlloc;
			    /*                               */
		Bool DownMixInhibit;
			    /*                            */
		UInt8 LevelShift;
			    /*                        */
	} tmbslHdmiTxPktAif_t;

/*                                                                            */
/* 
                                                    
 */
/*                   */
	typedef enum {
		HDMITX_MPEG_FRAME_UNKNOWN = 0,
					/*            */
		HDMITX_MPEG_FRAME_I = 1,/*             */
		HDMITX_MPEG_FRAME_B = 2,/*           */
		HDMITX_MPEG_FRAME_P = 3,/*           */
		HDMITX_MPEG_FRAME_INVALID = 4
					/*             */
	} tmbslHdmiTxMpegFrame_t;

/*                                                */
	typedef struct _tmbslHdmiTxPktMpeg_t {
		UInt32 bitRate;		    /*                       */
		tmbslHdmiTxMpegFrame_t frameType;
					    /*                   */
		Bool bFieldRepeat;	    /*                                  */
	} tmbslHdmiTxPktMpeg_t;

/*                                                                            */
/* 
                                                        
  */
/*                  */
	typedef enum {
		HDMITX_SPD_INFO_UNKNOWN = 0,
		HDMITX_SPD_INFO_DIGITAL_STB = 1,
		HDMITX_SPD_INFO_DVD = 2,
		HDMITX_SPD_INFO_DVHS = 3,
		HDMITX_SPD_INFO_HDD_VIDEO = 4,
		HDMITX_SPD_INFO_DVC = 5,
		HDMITX_SPD_INFO_DSC = 6,
		HDMITX_SPD_INFO_VIDEO_CD = 7,
		HDMITX_SPD_INFO_GAME = 8,
		HDMITX_SPD_INFO_PC = 9,
		HDMITX_SPD_INFO_INVALID = 10
	} tmbslHdmiTxSourceDev_t;

#define HDMI_TX_SPD_VENDOR_SIZE 8
#define HDMI_TX_SPD_DESCR_SIZE  16
#define HDMI_TX_SPD_LENGTH      25

/*                                                                      */
	typedef struct _tmbslHdmiTxPktSpd_t {
		UInt8 VendorName[HDMI_TX_SPD_VENDOR_SIZE];	 /*               */
		UInt8 ProdDescr[HDMI_TX_SPD_DESCR_SIZE];       /*                       */
		tmbslHdmiTxSourceDev_t SourceDevInfo;	       /*                      */
	} tmbslHdmiTxPktSpd_t;

/*                                                                            */
/* 
                                                  
  */
	typedef struct _tmbslHdmiTxPktVif_t {
		UInt8 Colour;	 /*            */
		Bool ActiveInfo; /*       */
		UInt8 BarInfo;	 /*            */
		UInt8 ScanInfo;	 /*            */
		UInt8 Colorimetry;
				 /*            */
		UInt8 PictureAspectRatio;
				 /*            */
		UInt8 ActiveFormatRatio;
				 /*            */
		UInt8 Scaling;	 /*            */
		UInt8 VidFormat; /*            */
		UInt8 PixelRepeat;
				 /*            */
		UInt16 EndTopBarLine;
		UInt16 StartBottomBarLine;
		UInt16 EndLeftBarPixel;
		UInt16 StartRightBarPixel;
	} tmbslHdmiTxPktVif_t;

/*                                                                            */
/* 
                                              
 */
/*               */
	typedef enum {
		HDMITX_SCAMODE_OFF = 0,
				     /*        */
		HDMITX_SCAMODE_ON = 1,
				     /*        */
		HDMITX_SCAMODE_AUTO = 2,
				     /*        */
		HDMITX_SCAMODE_NO_CHANGE = 3,
				     /*             */
		HDMITX_SCAMODE_INVALID = 4
				     /*             */
	} tmbslHdmiTxScaMode_t;

/*                                                                            */
/* 
                                                    
 */
	typedef struct _tmbslHdmiTxScalerDiag_t {
		UInt16 maxBuffill_p;
			     /*                                          */
		UInt16 maxBuffill_d;
			     /*                                          */
		UInt8 maxFifofill_pi;
			     /*                                          */
		UInt8 minFifofill_po1;
			     /*                                          */
		UInt8 minFifofill_po2;
			     /*                                          */
		UInt8 minFifofill_po3;
			     /*                                          */
		UInt8 minFifofill_po4;
			     /*                                          */
		UInt8 maxFifofill_di;
			     /*                                          */
		UInt8 maxFifofill_do;
			     /*                                          */
	} tmbslHdmiTxScalerDiag_t;

/*                                                                            */
/* 
                                                
 */
/*                                */
	typedef enum {
		HDMITX_SCALUT_DEFAULT_TAB1 = 0,
					   /*                       */
		HDMITX_SCALUT_DEFAULT_TAB2 = 1,
					   /*                       */
		HDMITX_SCALUT_USE_VSLUT = 2,
					   /*                       */
		HDMITX_SCALUT_INVALID = 3  /*                       */
	} tmbslHdmiTxScaLut_t;

/*                                                */
	enum _tmbslHdmiTxvsLut {
		HDMITX_VSLUT_COEFF_NUM = 45
	};
/*                                                                            */
/* 
                                                    
 */
/*                */
	typedef enum {
		HDMITX_INTEXT_INTERNAL = 0,/*             */
		HDMITX_INTEXT_EXTERNAL = 1,/*             */
		HDMITX_INTEXT_NO_CHANGE = 2,
					   /*             */
		HDMITX_INTEXT_INVALID = 3  /*             */
	} tmbslHdmiTxIntExt_t;

/*                */
	typedef enum {
		HDMITX_TOPSEL_INTERNAL = 0,/*             */
		HDMITX_TOPSEL_VRF = 1,	   /*             */
		HDMITX_TOPSEL_NO_CHANGE = 2,
					   /*             */
		HDMITX_TOPSEL_INVALID = 3  /*             */
	} tmbslHdmiTxTopSel_t;

/*                */
	typedef enum {
		HDMITX_TOPTGL_NO_ACTION = 0,
					   /*             */
		HDMITX_TOPTGL_TOGGLE = 1,  /*             */
		HDMITX_TOPTGL_NO_CHANGE = 2,
					   /*             */
		HDMITX_TOPTGL_INVALID = 3  /*             */
	} tmbslHdmiTxTopTgl_t;

/*                                                                            */
/* 
                                               
 */
/*                  */
	typedef enum {
		HDMITX_H_PHASES_16 = 0,
				      /*                        */
		HDMITX_H_PHASES_15 = 1,
				      /*                        */
		HDMITX_H_PHASES_INVALID = 2
				      /*             */
	} tmbslHdmiTxHPhases_t;

/*                                                                            */
/* 
                                              
 */
/*                         */
	enum _tmbslHdmiTxScalerFinePixelLimits {
		HDMITX_SCALER_FINE_PIXEL_MIN = 0x0000,
		HDMITX_SCALER_FINE_PIXEL_MAX = 0x1FFF,
		HDMITX_SCALER_FINE_PIXEL_NO_CHANGE = 0x2000,
		HDMITX_SCALER_FINE_PIXEL_INVALID = 0x2001
	};

/*                        */
	enum _tmbslHdmiTxScalerFineLineLimits {
		HDMITX_SCALER_FINE_LINE_MIN = 0x0000,
		HDMITX_SCALER_FINE_LINE_MAX = 0x07FF,
		HDMITX_SCALER_FINE_LINE_NO_CHANGE = 0x0800,
		HDMITX_SCALER_FINE_LINE_INVALID = 0x0801
	};
/*                                                                            */
/* 
                                              
 */
/*                    */
	typedef enum {
		HDMITX_VSMETH_V_H = 0, /*              */
		HDMITX_VSMETH_V_XDE = 1,
				       /*              */
		HDMITX_VSMETH_NO_CHANGE = 2,
				       /*              */
		HDMITX_VSMETH_INVALID = 3
				       /*              */
	} tmbslHdmiTxVsMeth_t;

/*                           */
	typedef enum {
		HDMITX_VSONCE_EACH_FRAME = 0,
				       /*                      */
		HDMITX_VSONCE_ONCE = 1,/*                      */
		HDMITX_VSONCE_NO_CHANGE = 2,
				       /*              */
		HDMITX_VSONCE_INVALID = 3
				       /*              */
	} tmbslHdmiTxVsOnce_t;

/*                                                                            */
/* 
                                               
 */
/*                   */
	typedef enum {
		HDMITX_TMDSOUT_NORMAL = 0,
				       /*                    */
		HDMITX_TMDSOUT_NORMAL1 = 1,
				       /*                              */
		HDMITX_TMDSOUT_FORCED0 = 2,
				       /*                    */
		HDMITX_TMDSOUT_FORCED1 = 3,
				       /*                    */
		HDMITX_TMDSOUT_INVALID = 4
				       /*                    */
	} tmbslHdmiTxTmdsOut_t;

/*                                                                            */
/* 
                                                  
 */
/*                          */
	enum _tmbslHdmiTxTmdsPhase {
		HDMITX_TMDSPHASE_MIN = 0,
		HDMITX_TMDSPHASE_MAX = 15,
		HDMITX_TMDSPHASE_INVALID = 16
	};

/*                                                                            */
/* 
                                               
 */
/*                     */
	typedef enum {
		HDMITX_PATTERN_OFF = 0,
				/*                           */
		HDMITX_PATTERN_CBAR4 = 1,
				/*                           */
		HDMITX_PATTERN_CBAR8 = 2,
				/*                           */
		HDMITX_PATTERN_BLUE = 3,/*                        */
		HDMITX_PATTERN_BLACK = 4,
				    /*                        */
		HDMITX_PATTERN_INVALID = 5
				/*                     */
	} tmbslHdmiTxTestPattern_t;

/*                                                                            */
/* 
                                            
 */
/*             */
	typedef enum {
		HDMITX_TESTMODE_PAT = 0,
				/*                                          */
		HDMITX_TESTMODE_656 = 1,
				/*                                          */
		HDMITX_TESTMODE_SERPHOE = 2,
				/*                                          */
		HDMITX_TESTMODE_NOSC = 3,
				/*                                          */
		HDMITX_TESTMODE_HVP = 4,
				/*                                          */
		HDMITX_TESTMODE_PWD = 5,
				/*                                          */
		HDMITX_TESTMODE_DIVOE = 6,
				/*                                          */
		HDMITX_TESTMODE_INVALID = 7
				/*                */
	} tmbslHdmiTxTestMode_t;

/*              */
	typedef enum {
		HDMITX_TESTSTATE_OFF = 0,
				   /*                             */
		HDMITX_TESTSTATE_ON = 1,
				   /*                             */
		HDMITX_TESTSTATE_INVALID = 2
				   /*                 */
	} tmbslHdmiTxTestState_t;

/*                                                                            */
/* 
                                                   
 */
/*                 */
	typedef enum {
		HDMITX_BLNKSRC_NOT_DE = 0, /*                        */
		HDMITX_BLNKSRC_VS_HS = 1,  /*                        */
		HDMITX_BLNKSRC_VS_NOT_HS = 2,
					   /*                        */
		HDMITX_BLNKSRC_VS_HEMB_VEMB = 3,
					   /*                        */
		HDMITX_BLNKSRC_NO_CHANGE = 4,
					   /*             */
		HDMITX_BLNKSRC_INVALID = 5 /*             */
	} tmbslHdmiTxBlnkSrc_t;

/*                 */
	typedef enum {
		HDMITX_BLNKCODE_ALL_0 = 0, /*                 */
		HDMITX_BLNKCODE_RGB444 = 1,/*                 */
		HDMITX_BLNKCODE_YUV444 = 2,/*                 */
		HDMITX_BLNKCODE_YUV422 = 3,/*                 */
		HDMITX_BLNKCODE_NO_CHANGE = 4,
					   /*             */
		HDMITX_BLNKCODE_INVALID = 5/*             */
	} tmbslHdmiTxBlnkCode_t;

/*                                                                            */
/* 
                                                 
 */
/*              */
	typedef enum {
		HDMITX_PIXEDGE_CLK_POS = 0, /*                             */
		HDMITX_PIXEDGE_CLK_NEG = 1, /*                             */
		HDMITX_PIXEDGE_NO_CHANGE = 2,
					    /*             */
		HDMITX_PIXEDGE_INVALID = 3  /*             */
	} tmbslHdmiTxPixEdge_t;

/*                 */
	typedef enum {
		HDMITX_UPSAMPLE_BYPASS = 0, /*          */
		HDMITX_UPSAMPLE_COPY = 1,   /*        */
		HDMITX_UPSAMPLE_INTERPOLATE = 2,
					    /*               */
		HDMITX_UPSAMPLE_AUTO = 3,   /*                                   */
		HDMITX_UPSAMPLE_NO_CHANGE = 4,
					    /*             */
		HDMITX_UPSAMPLE_INVALID = 5 /*             */
	} tmbslHdmiTxUpsampleMode_t;

/*                                                                            */
/* 
                                               
 */
/*                  */
	typedef enum {
		HDMITX_PIXSUBPKT_FIX_0 = 0, /*            */
		HDMITX_PIXSUBPKT_FIX_1 = 1, /*            */
		HDMITX_PIXSUBPKT_FIX_2 = 2, /*            */
		HDMITX_PIXSUBPKT_FIX_3 = 3, /*            */
		HDMITX_PIXSUBPKT_SYNC_FIRST = 4,
					    /*                    */
		HDMITX_PIXSUBPKT_SYNC_HEMB = 4,
					    /*                */
		HDMITX_PIXSUBPKT_SYNC_DE = 5,
					    /*                          */
		HDMITX_PIXSUBPKT_SYNC_HS = 6,
					    /*                          */
		HDMITX_PIXSUBPKT_NO_CHANGE = 7,
					    /*             */
		HDMITX_PIXSUBPKT_INVALID = 8,
					    /*             */
		HDMITX_PIXSUBPKT_SYNC_FIXED = 3
					    /*                                 
                                     
                                           */
	} tmbslHdmiTxPixSubpkt_t;

/*           */
	typedef enum {
		HDMITX_PIXTOGL_NO_ACTION = 0,
					    /*              */
		HDMITX_PIXTOGL_ENABLE = 1,  /*              */
		HDMITX_PIXTOGL_NO_CHANGE = 2,
					    /*              */
		HDMITX_PIXTOGL_INVALID = 3  /*              */
	} tmbslHdmiTxPixTogl_t;

/*                                                                            */
/* 
                                                  
 */
/*                                                             */
	enum _tmbslHdmiTxVinPortMap {
		HDMITX_VIN_PORT_MAP_TABLE_LEN = 6,

		HDMITX_VIN_PORT_SWAP_NO_CHANGE = 6,
		HDMITX_VIN_PORT_SWAP_INVALID = 7,

		HDMITX_VIN_PORT_MIRROR_NO_CHANGE = 2,
		HDMITX_VIN_PORT_MIRROR_INVALID = 3
	};


/*                                                                            */
/* 
                                                   
 */
/*                                                     */
	typedef enum {
		HDMITX_SYNCSRC_EMBEDDED = 0,
					 /*                 */
		HDMITX_SYNCSRC_EXT_VREF = 1,
					 /*                                  */
		HDMITX_SYNCSRC_EXT_VS = 2,
					 /*                        */
		HDMITX_SYNCSRC_NO_CHANGE = 3,
					 /*                 */
		HDMITX_SYNCSRC_INVALID = 4
					 /*                 */
	} tmbslHdmiTxSyncSource_t;

/*                                                                            */
/* 
                                                     
 */
/*                                  */
	enum _tmbslHdmiTxVoutFinePixelLimits {
		HDMITX_VOUT_FINE_PIXEL_MIN = 0x0000,
		HDMITX_VOUT_FINE_PIXEL_MAX = 0x1FFF,
		HDMITX_VOUT_FINE_PIXEL_NO_CHANGE = 0x2000,
		HDMITX_VOUT_FINE_PIXEL_INVALID = 0x2001
	};

/*                                 */
	enum _tmbslHdmiTxVoutFineLineLimits {
		HDMITX_VOUT_FINE_LINE_MIN = 0x0000,
		HDMITX_VOUT_FINE_LINE_MAX = 0x07FF,
		HDMITX_VOUT_FINE_LINE_NO_CHANGE = 0x0800,
		HDMITX_VOUT_FINE_LINE_INVALID = 0x0801
	};

/*                                                                            */
/* 
                                                  
 */
/*            */
	typedef enum {
		HDMITX_VOUT_PREFIL_OFF = 0,/*       */
		HDMITX_VOUT_PREFIL_121 = 1,/*       */
		HDMITX_VOUT_PREFIL_109 = 2,/*       */
		HDMITX_VOUT_PREFIL_CCIR601 = 3,
					   /*             */
		HDMITX_VOUT_PREFIL_NO_CHANGE = 4,
					   /*             */
		HDMITX_VOUT_PREFIL_INVALID = 5
					   /*             */
	} tmbslHdmiTxVoutPrefil_t;

/*               */
	typedef enum {
		HDMITX_VOUT_YUV_BLNK_16 = 0,
					   /*      */
		HDMITX_VOUT_YUV_BLNK_0 = 1,/*      */
		HDMITX_VOUT_YUV_BLNK_NO_CHANGE = 2,
					   /*             */
		HDMITX_VOUT_YUV_BLNK_INVALID = 3
					   /*             */
	} tmbslHdmiTxVoutYuvBlnk_t;

/*                           */
	typedef enum {
		HDMITX_VOUT_QRANGE_FS = 0, /*              */
		HDMITX_VOUT_QRANGE_RGB_YUV = 1,
					   /*              */
		HDMITX_VOUT_QRANGE_YUV = 2,/*              */
		HDMITX_VOUT_QRANGE_NO_CHANGE = 3,
					   /*              */
		HDMITX_VOUT_QRANGE_INVALID = 4
					   /*              */
	} tmbslHdmiTxVoutQrange_t;

/*                                                                            */
/* 
                                                
 */
/*                    */
	typedef enum {
		HDMITX_VSSRC_INTERNAL = 0,
				       /*             */
		HDMITX_VSSRC_EXTERNAL = 1,
				       /*             */
		HDMITX_VSSRC_NO_CHANGE = 2,
				       /*             */
		HDMITX_VSSRC_INVALID = 3
				       /*             */
	} tmbslHdmiTxVsSrc_t;

/*                    */
	typedef enum {
		HDMITX_VSTGL_TABLE = 0,/*                             */
		HDMITX_VSTGL_UNUSED_1 = 1,
				       /*                   */
		HDMITX_VSTGL_UNUSED_2 = 2,
				       /*                   */
		HDMITX_VSTGL_UNUSED_3 = 3,
				       /*                   */
		HDMITX_VSTGL_NO_ACTION = 4,
				       /*                   */
		HDMITX_VSTGL_HS = 5,   /*                   */
		HDMITX_VSTGL_VS = 6,   /*                   */
		HDMITX_VSTGL_HS_VS = 7,/*                   */
		HDMITX_VSTGL_NO_CHANGE = 8,
				       /*                   */
		HDMITX_VSTGL_INVALID = 9
				       /*                   */
	} tmbslHdmiTxVsTgl_t;

/*                                                                            */
/* 
                                              
 */
/*                          */
	enum _tmbslHdmiTxPixRepeat {
		HDMITX_PIXREP_NONE = 0,
				     /*                  */
		HDMITX_PIXREP_MIN = 0,
				     /*                  */

		HDMITX_PIXREP_0 = 0,
		HDMITX_PIXREP_1 = 1,
		HDMITX_PIXREP_2 = 2,
		HDMITX_PIXREP_3 = 3,
		HDMITX_PIXREP_4 = 4,
		HDMITX_PIXREP_5 = 5,
		HDMITX_PIXREP_6 = 6,
		HDMITX_PIXREP_7 = 7,
		HDMITX_PIXREP_8 = 8,
		HDMITX_PIXREP_9 = 9,

		HDMITX_PIXREP_MAX = 9,
				     /*                  */
		HDMITX_PIXREP_DEFAULT = 10,
				     /*                                         */
		HDMITX_PIXREP_NO_CHANGE = 11,
				     /*             */
		HDMITX_PIXREP_INVALID = 12
				     /*             */
	};

/*               */
	typedef enum {
		HDMITX_MATMODE_OFF = 0,
				     /*        */
		HDMITX_MATMODE_AUTO = 1,
				     /*        */
		HDMITX_MATMODE_NO_CHANGE = 2,
				     /*             */
		HDMITX_MATMODE_INVALID = 3
				     /*             */
	} tmbslHdmiTxMatMode_t;

/*                    */
	typedef enum {
		HDMITX_VOUT_DBITS_12 = 0,  /*           */
		HDMITX_VOUT_DBITS_8 = 1,   /*           */
		HDMITX_VOUT_DBITS_10 = 2,  /*           */
		HDMITX_VOUT_DBITS_NO_CHANGE = 3,
					   /*             */
		HDMITX_VOUT_DBITS_INVALID = 4
					   /*             */
	} tmbslHdmiTxVoutDbits_t;

/*              */
	typedef enum {
		HDMITX_COLORDEPTH_24 = 0,  /*                     */
		HDMITX_COLORDEPTH_30 = 1,  /*                     */
		HDMITX_COLORDEPTH_36 = 2,  /*                     */
		HDMITX_COLORDEPTH_48 = 3,  /*                     */
		HDMITX_COLORDEPTH_NO_CHANGE = 4,
					   /*             */
		HDMITX_COLORDEPTH_INVALID = 5
					   /*             */
	} tmbslHdmiTxColorDepth;

/*                                                      */
	typedef enum {
		HDMITX_3D_NONE = 0,	   /*                             */
		HDMITX_3D_FRAME_PACKING = 1,
					   /*                                         */
		HDMITX_3D_TOP_AND_BOTTOM = 2,
					   /*                                          */
		HDMITX_3D_SIDE_BY_SIDE_HALF = 3,
					   /*                                             */
		HDMITX_3D_INVALID = 4	   /*           */
	} tmbslHdmiTx3DStructure_t;

/*                                                                            */
/* 
                                                    
 */
/*                                 */
	enum _tmbslHdmiTxMatOffset {
		HDMITX_MAT_OFFSET_NUM = 3
	};

/*                                                                    */
	typedef struct _tmbslHdmiTxMatOffset_t {
    /*                                        */
		Int16 Offset[HDMITX_MAT_OFFSET_NUM];
	} tmbslHdmiTxMatOffset_t;

/*                        */
	enum _tmbslHdmiTxMatLimits {
		HDMITX_MAT_OFFSET_MIN = -1024,
		HDMITX_MAT_OFFSET_MAX = 1023
	};

/*                                                                            */
/* 
                                                                              
 */
	typedef enum {
		HDMITX_POWER_STATE_STAND_BY = 0,   /*                        */
		HDMITX_POWER_STATE_SLEEP_MODE = 1, /*                        */
		HDMITX_POWER_STATE_ON = 2,	   /*                */
		HDMITX_POWER_STATE_INVALID = 3	   /*                        */
	} tmbslHdmiTxPowerState_t;

/* 
                                                                        
 */
	typedef struct {
		UInt8 HB[3];
		   /*                                 */
		UInt8 PB[28];
		   /*                       */
	} tmbslHdmiTxPktGamut_t;


/* 
                                                
 */
	typedef struct {
		UInt8 HB[3];
		   /*                                 */
		UInt8 PB[28];
		   /*                       */
	} tmbslHdmiTxPktRawAvi_t;


/*                */
	typedef enum {
		HDMITX_SINK_CAT_NOT_REPEATER = 0,
					 /*                 */
		HDMITX_SINK_CAT_REPEATER = 1,
					 /*                 */
		HDMITX_SINK_CAT_INVALID = 3
					 /*                 */
	} tmbslHdmiTxSinkCategory_t;


	typedef struct {
		Bool latency_available;
		Bool Ilatency_available;
		UInt8 Edidvideo_latency;
		UInt8 Edidaudio_latency;
		UInt8 EdidIvideo_latency;
		UInt8 EdidIaudio_latency;

	} tmbslHdmiTxEdidLatency_t;

/* 
                                                 
 */
	typedef struct {
		UInt8 maxTmdsClock;	/*                              */
		UInt8 cnc0;	/*                              */
		UInt8 cnc1;	/*                    */
		UInt8 cnc2;	/*                     */
		UInt8 cnc3;	/*                   */
		UInt8 hdmiVideoPresent;	/*                         */
		UInt8 h3DPresent;	/*                             */
		UInt8 h3DMultiPresent;	/*                            */
		UInt8 imageSize;	/*                                                       */
		UInt8 hdmi3DLen;	/*                                  */
		UInt8 hdmiVicLen;	/*                                        */
		UInt8 ext3DData[21];	/*                          */
	} tmbslHdmiTxEdidExtraVsdbData_t;

/* 
                                                   
 */
	typedef enum {
		HDMITX_VQR_DEFAULT = 0,	/*                   */
		HDMITX_RGB_FULL = 1,	/*                           */
		HDMITX_RGB_LIMITED = 2	/*                              */
	} tmbslHdmiTxVQR_t;




#ifdef __cplusplus
}
#endif
#endif				/*                     */
/*                                                                            *//*                                                                            *//*                                                                            */
