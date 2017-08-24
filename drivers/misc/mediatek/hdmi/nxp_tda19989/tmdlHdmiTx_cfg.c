/**
 * Copyright (C) 2006 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiTx_cfg.c
 *
 * \version       Revision: 1
 *
 * \date          Date: 25/03/11 11:00
 *
 * \brief         devlib driver component API for the TDA998x HDMI Transmitters
 *
 * \section refs  Reference Documents
 * HDMI Tx Driver - FRS.doc,
 *
 * \section info  Change Information
 *
 * \verbatim

   History:       tmdlHdmiTx_cfg.c
 *
 * *****************  Version 2  *****************
 * User: V. Vrignaud Date: March 25th, 2011
 *
 * *****************  Version 1  *****************
 * User: J. Lamotte Date: 08/08/07  Time: 11:00
 * initial version
 *

   \endverbatim
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


#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/mt_gpio.h>
#include <linux/slab.h>
#include "tmNxCompId.h"
#include "tmdlHdmiTx_Types.h"
#include "tmdlHdmiTx_Functions.h"



#include "tmNxTypes.h"
#include "tmbslHdmiTx_types.h"
#include "tmdlHdmiTx_cfg.h"
#include "tmdlHdmiTx_IW.h"
/*                                */
#define RETIF(cond, rslt) if ((cond)) {return (rslt); }
#define I2C_M_WR 0

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t TxI2cReadFunction(tmbslHdmiTxSysArgs_t *pSysArgs);
tmErrorCode_t TxI2cWriteFunction(tmbslHdmiTxSysArgs_t *pSysArgs);

/*                                                                             
                                                                               
                                                                               
                                                                               
                                                                             */
struct i2c_client *GetThisI2cClient(void);
unsigned char my_i2c_data[255];

/*                                                               */
/*                                                                            */
/*                                                                            */
#	include <linux/kernel.h>
#	include <linux/errno.h>
#	include <linux/string.h>
#	include <linux/types.h>
#	include <linux/i2c.h>
#	include <linux/delay.h>

/*                                                                            */
/*                                                                            */
#define UNIT_I2C_ADDRESS_0 0x70

/*                                                                            */
/*                                                                            */
#define I2C_BUS_NUMBER_0 0	/*           */

/*                                                                            */
#define SUB_ADDR_BYTE_COUNT_0 1

/*                                                                            */
/*                                                                            */
/*                                                                            */
#define COMMAND_TASK_PRIORITY_0  250
#define COMMAND_TASK_PRIORITY_1  250

#include "tmdlHdmiTx_IW.h"
#include "tmdlHdmiTx.h"
#include "tmdlHdmiTx_cfg.h"
#include "I2C.h"

#ifdef TMFL_CEC_AVAILABLE
#include "tmdlHdmiCEC_functions.h"
#define CEC_UNIT_I2C_ADDRESS_0 0x34
#define CEC_UNIT_I2C_ADDRESS_1 0x34
#endif

/*                                    */
#if defined(TMFL_CFG_ZOOMII)	/*              */
#	include "tmdlHdmiTx_Linux_cfg.c"
#elif defined(TMFL_CFG_INTELCE4100)	/*               */
#	include "tmdlHdmiTx_IntelCE4100_cfg.c"
#elif defined(TMFL_OS_WINDOWS)	/*                          */
#	include "tmdlHdmiTx_Win_cfg.c"
#else				/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t TxI2cReadFunction(tmbslHdmiTxSysArgs_t *pSysArgs);
tmErrorCode_t TxI2cWriteFunction(tmbslHdmiTxSysArgs_t *pSysArgs);

/*                                                                             
                                                                               
                                                                               
                                                                               
                                                                             */
/*                                                                        */
/*                                                                            */
/*                                                                            */
/*                  */
/*                      */

/*                                                                            */
/*                                                                            */
#define UNIT_I2C_ADDRESS_0 0x70

/*                                                                            */
/*                                                                            */
/*                                                                            */
#define COMMAND_TASK_PRIORITY_0  250
#define COMMAND_TASK_PRIORITY_1  250

/*                                  */
/*                                                                            */
/*                                                                            */
#define HDCP_CHECK_TASK_PRIORITY_0  250

/*                                 */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
#define COMMAND_TASK_STACKSIZE_0 128
#define COMMAND_TASK_STACKSIZE_1 128

/*                                    */
/*                                                                            */
/*                                                                            */
#define HDCP_CHECK_TASK_STACKSIZE_0 128

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
#define COMMAND_TASK_QUEUESIZE_0 128
#define COMMAND_TASK_QUEUESIZE_1 128

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
#define KEY_SEED 0x1234

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
const tmdlHdmiTxCfgVideoSignal444 videoPortMapping_YUV444[MAX_UNITS][6] = {
	{
	 TMDL_HDMITX_VID444_GY_0_TO_3,	/*                                */
	 TMDL_HDMITX_VID444_GY_4_TO_7,	/*                                */
	 TMDL_HDMITX_VID444_BU_0_TO_3,	/*                                */
	 TMDL_HDMITX_VID444_BU_4_TO_7,	/*                                */
	 TMDL_HDMITX_VID444_VR_0_TO_3,	/*                                */
	 TMDL_HDMITX_VID444_VR_4_TO_7	/*                                */
	 }
};

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
const tmdlHdmiTxCfgVideoSignal444 videoPortMapping_RGB444[MAX_UNITS][6] = {
	{
	 TMDL_HDMITX_VID444_VR_0_TO_3,	/*                                */
	 TMDL_HDMITX_VID444_VR_4_TO_7,	/*                                */

	 TMDL_HDMITX_VID444_BU_0_TO_3,	/*                                */
	 TMDL_HDMITX_VID444_BU_4_TO_7,	/*                                */
	 TMDL_HDMITX_VID444_GY_0_TO_3,	/*                                */
	 TMDL_HDMITX_VID444_GY_4_TO_7,	/*                                */


	 }
};

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
const tmdlHdmiTxCfgVideoSignal422 videoPortMapping_YUV422[MAX_UNITS][6] = {
	{
	 TMDL_HDMITX_VID422_NOT_CONNECTED,	/*                                */
	 TMDL_HDMITX_VID422_NOT_CONNECTED,	/*                                */
	 TMDL_HDMITX_VID422_UV_4_TO_7,	/*                                */
	 TMDL_HDMITX_VID422_UV_8_TO_11,	/*                                */
	 TMDL_HDMITX_VID422_Y_4_TO_7,	/*                                */
	 TMDL_HDMITX_VID422_Y_8_TO_11	/*                                */
	 }
};

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
const tmdlHdmiTxCfgVideoSignalCCIR656 videoPortMapping_CCIR656[MAX_UNITS][6] = {
	{
	 TMDL_HDMITX_VIDCCIR_NOT_CONNECTED,	/*                                */
	 TMDL_HDMITX_VIDCCIR_NOT_CONNECTED,	/*                                */
	 TMDL_HDMITX_VIDCCIR_NOT_CONNECTED,	/*                                */
	 TMDL_HDMITX_VIDCCIR_NOT_CONNECTED,	/*                                */
	 TMDL_HDMITX_VIDCCIR_4_TO_7,	/*                                */
	 TMDL_HDMITX_VIDCCIR_8_TO_11	/*                                */
	 }
};

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t TxI2cReadFunction(tmbslHdmiTxSysArgs_t *pSysArgs)
{
	tmErrorCode_t errCode = TM_OK;

	if (pSysArgs->slaveAddr == 0x70) {
		errCode = i2cRead(reg_TDA998X, (tmbslHdmiSysArgs_t *) pSysArgs);
	} else if (pSysArgs->slaveAddr == 0x34) {
		errCode = i2cRead(reg_TDA9989_CEC, (tmbslHdmiSysArgs_t *) pSysArgs);
	} else {
		errCode = ~TM_OK;
	}

	return errCode;
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
tmErrorCode_t TxI2cWriteFunction(tmbslHdmiTxSysArgs_t *pSysArgs)
{
	tmErrorCode_t errCode = TM_OK;

	if (pSysArgs->slaveAddr == 0x70) {
		errCode = i2cWrite(reg_TDA998X, (tmbslHdmiSysArgs_t *) pSysArgs);

	} else if (pSysArgs->slaveAddr == 0x34) {
		errCode = i2cWrite(reg_TDA9989_CEC, (tmbslHdmiSysArgs_t *) pSysArgs);
	} else {
		errCode = ~TM_OK;
	}

	return errCode;
}

#endif

#ifdef TMFL_RGB_DDR_12BITS

/*                                                                            */
/*                                                                            */
/*
                                                                                        
                                                                                                      
                                                                    
                                                                                     
                                                         

                              

                                                                         
                                           

                                

                                                              
                                                     

                                                                        
                                                                 
                                    
                                    
                                    
                                       
                                   
           

                                                  

                                                                     
                                                                     
                                                                     
                                                                     
                                                                     
                                          
                                          
                                          
                                                              

                                                                    
                                                              

                                                                   
                                          
                                          
                                          
                                                                        
                                                                     
                                                                     
                                                                     
                                                                        

                                                                   
                                          
                                          
                                          
                                                                     
                                                                     
                                                                     
                                                                     
                                                                     






                                                                     

                                                           
                                       
                         
                                      

                                                                     
                                                                     
                                                                     
                                                                     
                                                                     
                                                                 

                              

                                                                                                                                      
                                                                                                                                      
                                                                                                                                      
                                                                                                                                      
                                                                                                                                      
                                                                                                                                              

                                                                                                                                      
                                                                                                                                      
                                                                                                                                      
                                                                                                                                      
                                                                                                                                      
                                                                                                                                              

                                                                                                                                      
                                                                                                                                      
                                                                                                                                      
                                                                                                                                      
                                                                                                                                      
                                                                                                                                              

                                      
                        
                        
                        

   

*/

const tmdlHdmiTxCfgVideoSignal_RGB_DDR_12bits VideoPortMapping_RGB_DDR_12bits[MAX_UNITS][6] = {
	{
	 TMDL_HDMITX_VID_B_0_3_G_4_7,	/*                                */
	 TMDL_HDMITX_VID_DDR_NOT_CONNECTED,	/*                                */
	 TMDL_HDMITX_VID_B_4_7_R_0_3,	/*                                */
	 TMDL_HDMITX_VID_G_0_3_R_4_7,	/*                                */
	 TMDL_HDMITX_VID_DDR_NOT_CONNECTED,	/*                                */
	 TMDL_HDMITX_VID_DDR_NOT_CONNECTED	/*                                */
	 }
};

/*

                                            
                                                      
                                                  

                                                                     
                                                                     
                                                                     
                                                                     
                                                                     
                                                                 
                                                   
                                         
                                       
                              
                            
                          
                               
                             
                                 
                                     
                                                

                                                                 

*/

const UInt8 VideoPortMux_RGB_DDR_12bits[MAX_UNITS] = {
	VIP_MUX_R_R | VIP_MUX_G_G | VIP_MUX_B_B
};

const UInt8 VideoPortNoMux[MAX_UNITS] = {
	VIP_MUX_G_B | VIP_MUX_B_R | VIP_MUX_R_G
};

#endif				/*                     */

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
UInt8 enableAudioPortSPDIF[MAX_UNITS] = { 0x02 };
UInt8 enableAudioClockPortSPDIF[MAX_UNITS] = { DISABLE_AUDIO_CLOCK_PORT };
UInt8 groundAudioPortSPDIF[MAX_UNITS] = { 0xFD };
UInt8 groundAudioClockPortSPDIF[MAX_UNITS] = { ENABLE_AUDIO_CLOCK_PORT_PULLDOWN };

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
UInt8 enableAudioPortI2S[MAX_UNITS] = { 0x03 };
UInt8 enableAudioPortI2S8C[MAX_UNITS] = { 0x1f };
UInt8 enableAudioClockPortI2S[MAX_UNITS] = { ENABLE_AUDIO_CLOCK_PORT };
UInt8 groundAudioPortI2S[MAX_UNITS] = { 0xfc };
UInt8 groundAudioPortI2S8C[MAX_UNITS] = { 0xe0 };
UInt8 groundAudioClockPortI2S[MAX_UNITS] = { DISABLE_AUDIO_CLOCK_PORT_PULLDOWN };

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
UInt8 enableAudioPortOBA[MAX_UNITS] = { 0xFF };
UInt8 enableAudioClockPortOBA[MAX_UNITS] = { ENABLE_AUDIO_CLOCK_PORT };
UInt8 groundAudioPortOBA[MAX_UNITS] = { 0x00 };
UInt8 groundAudioClockPortOBA[MAX_UNITS] = { DISABLE_AUDIO_CLOCK_PORT_PULLDOWN };

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
UInt8 enableAudioPortDST[MAX_UNITS] = { 0xFF };
UInt8 enableAudioClockPortDST[MAX_UNITS] = { ENABLE_AUDIO_CLOCK_PORT };
UInt8 groundAudioPortDST[MAX_UNITS] = { 0x00 };
UInt8 groundAudioClockPortDST[MAX_UNITS] = { DISABLE_AUDIO_CLOCK_PORT_PULLDOWN };

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
UInt8 enableAudioPortHBR[MAX_UNITS] = { 0x1f };
UInt8 enableAudioClockPortHBR[MAX_UNITS] = { ENABLE_AUDIO_CLOCK_PORT };
UInt8 groundAudioPortHBR[MAX_UNITS] = { 0xe0 };
UInt8 groundAudioClockPortHBR[MAX_UNITS] = { DISABLE_AUDIO_CLOCK_PORT_PULLDOWN };

/*                                                                            
                                                                              
                                                                              
                                                                              
                                                                            */

/*                                                                            */
/*                                                                            */
UInt8 mirrorTableCCIR656[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 swapTableCCIR656[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 enableVideoPortCCIR656[MAX_UNITS][3] = { {0x00, 0x00, 0x00} };
UInt8 groundVideoPortCCIR656[MAX_UNITS][3] = { {0xFF, 0xFF, 0xFF} };
UInt8 mirrorTableYUV422[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 swapTableYUV422[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 enableVideoPortYUV422[MAX_UNITS][3] = { {0x00, 0x00, 0x00} };
UInt8 groundVideoPortYUV422[MAX_UNITS][3] = { {0xFF, 0xFF, 0xFF} };
UInt8 mirrorTableYUV444[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 swapTableYUV444[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 enableVideoPortYUV444[MAX_UNITS][3] = { {0x00, 0x00, 0x00} };
UInt8 groundVideoPortYUV444[MAX_UNITS][3] = { {0xFF, 0xFF, 0xFF} };
UInt8 mirrorTableRGB444[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 swapTableRGB444[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 enableVideoPortRGB444[MAX_UNITS][3] = { {0x00, 0x00, 0x00} };
UInt8 groundVideoPortRGB444[MAX_UNITS][3] = { {0xFF, 0xFF, 0xFF} };

#ifdef TMFL_RGB_DDR_12BITS
UInt8 mirrorTableRGB_DDR_12bits[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 swapTableRGB_DDR_12bits[MAX_UNITS][6] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };
UInt8 enableVideoPortRGB_DDR_12bits[MAX_UNITS][3] = { {0x00, 0x00, 0x00} };
UInt8 groundVideoPortRGB_DDR_12bits[MAX_UNITS][3] = { {0xFF, 0xFF, 0xFF} };
UInt8 NoMux[MAX_UNITS] = { 0x00 };
UInt8 Mux_RGB_DDR_12bits[MAX_UNITS] = { 0x00 };
#endif

/*                                                                            */
/*                                                                            */
tmdlHdmiTxDriverConfigTable_t driverConfigTableTx[MAX_UNITS] = {
	{
	 COMMAND_TASK_PRIORITY_0,
	 COMMAND_TASK_STACKSIZE_0,
	 COMMAND_TASK_QUEUESIZE_0,
	 HDCP_CHECK_TASK_PRIORITY_0,
	 HDCP_CHECK_TASK_STACKSIZE_0,
	 UNIT_I2C_ADDRESS_0,
	 TxI2cReadFunction,
	 TxI2cWriteFunction,
	 Null,			/*                                   */
	 &mirrorTableCCIR656[0][0],	/*                                   */
	 &swapTableCCIR656[0][0],	/*                                   */
	 &enableVideoPortCCIR656[0][0],	/*                                   */
	 &groundVideoPortCCIR656[0][0],	/*                                   */
	 &mirrorTableYUV422[0][0],	/*                                   */
	 &swapTableYUV422[0][0],	/*                                   */
	 &enableVideoPortYUV422[0][0],	/*                                   */
	 &groundVideoPortYUV422[0][0],	/*                                   */
	 &mirrorTableYUV444[0][0],	/*                                   */
	 &swapTableYUV444[0][0],	/*                                   */
	 &enableVideoPortYUV444[0][0],	/*                                   */
	 &groundVideoPortYUV444[0][0],	/*                                   */
	 &mirrorTableRGB444[0][0],	/*                                   */
	 &swapTableRGB444[0][0],	/*                                   */
	 &enableVideoPortRGB444[0][0],	/*                                   */
	 &groundVideoPortRGB444[0][0],	/*                                   */
#ifdef TMFL_RGB_DDR_12BITS
	 &mirrorTableRGB_DDR_12bits[0][0],
	 &swapTableRGB_DDR_12bits[0][0],
	 &NoMux[0],
	 &Mux_RGB_DDR_12bits[0],
	 &enableVideoPortRGB_DDR_12bits[0][0],
	 &groundVideoPortRGB_DDR_12bits[0][0],
#endif
	 &enableAudioPortSPDIF[0],
	 &groundAudioPortSPDIF[0],
	 &enableAudioClockPortSPDIF[0],
	 &groundAudioClockPortSPDIF[0],
	 &enableAudioPortI2S[0],
	 &groundAudioPortI2S[0],
	 &enableAudioPortI2S8C[0],
	 &groundAudioPortI2S8C[0],
	 &enableAudioClockPortI2S[0],
	 &groundAudioClockPortI2S[0],
	 &enableAudioPortOBA[0],
	 &groundAudioPortOBA[0],
	 &enableAudioClockPortOBA[0],
	 &groundAudioClockPortOBA[0],
	 &enableAudioPortDST[0],
	 &groundAudioPortDST[0],
	 &enableAudioClockPortDST[0],
	 &groundAudioClockPortDST[0],
	 &enableAudioPortHBR[0],
	 &groundAudioPortHBR[0],
	 &enableAudioClockPortHBR[0],
	 &groundAudioClockPortHBR[0],
	 KEY_SEED,
	 TMDL_HDMITX_PATTERN_BLUE,
	 1			/*                        */
	 }
};

#ifdef TMFL_CEC_AVAILABLE

tmdlHdmiCecCapabilities_t CeccapabilitiesList = { TMDL_HDMICEC_DEVICE_UNKNOWN, CEC_VERSION_1_3a };

/* 
                                                                          
                                         
 */

tmdlHdmiCecDriverConfigTable_t CecdriverConfigTable[MAX_UNITS] = {
	{
	 COMMAND_TASK_PRIORITY_0,
	 COMMAND_TASK_STACKSIZE_0,
	 COMMAND_TASK_QUEUESIZE_0,
	 CEC_UNIT_I2C_ADDRESS_0,
	 TxI2cReadFunction,
	 TxI2cWriteFunction,
	 &CeccapabilitiesList}
};


/*                                                                             
                                                                              
                                                                              
                                                                              
                                                                            */

/* 
                                                                  
                             

                                                  

                            
                                     
                                                                      
                                               
                                                                    
                   

                                                                             */
tmErrorCode_t tmdlHdmiCecCfgGetConfig(tmUnitSelect_t unit, tmdlHdmiCecDriverConfigTable_t *pConfig) {
	/*                                  */
	if ((unit < 0) || (unit >= MAX_UNITS))
		return TMDL_ERR_DLHDMICEC_BAD_UNIT_NUMBER;

	/*                          */
	if (pConfig == Null)
		return TMDL_ERR_DLHDMICEC_INCONSISTENT_PARAMS;

	*pConfig = CecdriverConfigTable[unit];

	return TM_OK;
}


/*                                                                            */
/*                                                                            */
/*                                                                            */

#endif
/*                                                                             
                                                                                
                            

                                                                      

                            
                                     
                                                                    

                                                                             */
tmErrorCode_t tmdlHdmiTxIWWait(UInt16 duration) {
	msleep((unsigned long)duration);

	return (TM_OK);
}

/*                                                                             
                                              

                                                 

                            
                                     
                                                                    
                                                                   
                   

                                                                             */
/*
                                         
 
                                    
 
 
                             

                                           
                                                                 

                                                                             
                 
                                                 
                                             
     
                                                                 

                      
                                              

                                                          

                  
 
*/

DEFINE_SEMAPHORE(mutex0);
DEFINE_SEMAPHORE(mutex1);
DEFINE_SEMAPHORE(mutex2);

tmErrorCode_t tmdlHdmiTxIWSemaphoreCreate(tmdlHdmiTxIWSemHandle_t *pHandle)
{


	static int i;
	struct semaphore *mutex[3];
	/*                                      */
	RETIF(pHandle == Null, TMDL_ERR_DLHDMITX_INCONSISTENT_PARAMS)

	    mutex[0] = &mutex0;
	mutex[1] = &mutex1;
	mutex[2] = &mutex2;

	if (i > 2 || i < 0) {
		pr_debug("%s,create semphore error\n", __func__);
		return -1;
	}
	*pHandle = (tmdlHdmiTxIWSemHandle_t) mutex[i];
	i++;

	RETIF(pHandle == NULL, TMDL_ERR_DLHDMITX_NO_RESOURCES)

	    return (TM_OK);
}


/*                                                                             
                                                         

                                                            

                            
                                     
                                                               

                                                                             */
tmErrorCode_t tmdlHdmiTxIWSemaphoreDestroy(tmdlHdmiTxIWSemHandle_t handle) {
	RETIF(handle == False, TMDL_ERR_DLHDMITX_BAD_HANDLE);
	/*
                                                                           
                                          
     

                           
  */
	return (TM_OK);
}

/*                                                                             
                                                           

                                                           

                            
                                     
                                                               

                                                                             */
tmErrorCode_t tmdlHdmiTxIWSemaphoreP(tmdlHdmiTxIWSemHandle_t handle) {
	/*                                                                  */
	down((struct semaphore *)handle);

	return (TM_OK);
}

/*                                                                             
                                                           

                                                           

                            
                                     
                                                               

                                                                             */
tmErrorCode_t tmdlHdmiTxIWSemaphoreV(tmdlHdmiTxIWSemHandle_t handle) {
	/*                                                                  */
	up((struct semaphore *)handle);

	return (TM_OK);
}

/*                                                                            */
/*                                                                            */
/*                                                                            */
