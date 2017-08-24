/**
 * Copyright (C) 2007 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiTx_IW.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 07/08/07 16:00 $
 *
 * \brief         devlib driver component API for the TDA998x HDMI Transmitters
 *
 * \section refs  Reference Documents
 * TDA998x Driver - FRS.doc,
 * TDA998x Driver - tmdlHdmiTx - SCS.doc
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmdlHdmiTx_IW.h $
 *
 * *****************  Version 1  *****************
 * User: J. Lamotte Date: 07/08/07   Time: 16:00
 * Updated in $/Source/tmdlHdmiTx/inc
 * initial version
 *

   \endverbatim
 *
*/

#ifndef TMDLHDMITX_IW_H
#define TMDLHDMITX_IW_H

/*                                                                            */
/*                                                                            */
/*                                                                            */

#ifdef TMFL_OS_WINDOWS
#define _WIN32_WINNT 0x0500
#include "windows.h"
#endif

#include "tmNxTypes.h"
#include "tmdlHdmiTx_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */
	typedef void (*tmdlHdmiTxIWFuncPtr_t) (void);
	typedef UInt8 tmdlHdmiTxIWTaskHandle_t;
	typedef UInt8 tmdlHdmiTxIWQueueHandle_t;
#ifdef TMFL_OS_WINDOWS
#error
	typedef HANDLE tmdlHdmiTxIWSemHandle_t;
#else
#ifdef TMFL_LINUX_OS_KERNEL_DRIVER
	typedef unsigned long tmdlHdmiTxIWSemHandle_t;
#else
#error "platform error"
	typedef UInt8 tmdlHdmiTxIWSemHandle_t;
#endif
#endif

/* 
                                                                          
 */
	typedef enum {
		TMDL_HDMI_IW_RX_1,
		TMDL_HDMI_IW_RX_2,
		TMDL_HDMI_IW_TX_1,
		TMDL_HDMI_IW_TX_2,
		TMDL_HDMI_IW_CEC_1,
		TMDL_HDMI_IW_CEC_2
	} tmdlHdmiIWDeviceInterrupt_t;

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                             
                                                                        
                                                                        
                                                            

                                                                                          
                                                                                            
                                                                    
                                                     

                            
                                     
                                                                    
                                                                   
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWTaskCreate(tmdlHdmiTxIWFuncPtr_t pFunc, UInt8 priority,
					     UInt16 stackSize, tmdlHdmiTxIWTaskHandle_t *pHandle);

/*                                                                             
                                                                                   

                                                                                              

                            
                                     
                                                                    
                   
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWTaskDestroy(tmdlHdmiTxIWTaskHandle_t handle);

/*                                                                             
                                                 

                                                     

                            
                                     
                                                                         
                                                                 
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWTaskStart(tmdlHdmiTxIWTaskHandle_t handle);

/*                                                                             
                                                                                                        

                                                                      

                            
                                     
                                                                    

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWWait(UInt16 duration);

/*                                                                             
                                                  

                                                                        
                                                     

                            
                                     
                                                                   
                   
                                                                    

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWQueueCreate(UInt8 queueSize, tmdlHdmiTxIWQueueHandle_t *pHandle);

/*                                                                             
                                                             

                                                        

                            
                                     
                                                               
                                                                    
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWQueueDestroy(tmdlHdmiTxIWQueueHandle_t handle);

/*                                                                             
                                                                           

                                                                      
                                       

                            
                                     
                                                               
                                                                    
                   
                                                

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWQueueSend(tmdlHdmiTxIWQueueHandle_t handle, UInt8 message);

/*                                                                             
                                                                           

                                                                           
                                                      

                            
                                     
                                                               
                                                                    
                   
                                                                   
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWQueueReceive(tmdlHdmiTxIWQueueHandle_t handle, UInt8 *pMessage);

/*                                                                             
                                              

                                                 

                            
                                     
                                                                    
                                                                   
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWSemaphoreCreate(tmdlHdmiTxIWSemHandle_t *pHandle);

/*                                                                             
                                                         

                                                            

                            
                                     
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWSemaphoreDestroy(tmdlHdmiTxIWSemHandle_t handle);

/*                                                                             
                                                           

                                                           

                            
                                     
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWSemaphoreP(tmdlHdmiTxIWSemHandle_t handle);

/*                                                                             
                                                           

                                                           

                            
                                     
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxIWSemaphoreV(tmdlHdmiTxIWSemHandle_t handle);

/*                                                                             
                                                                        

          

                            
                                     

                                                                             */
	void tmdlHdmiTxIWDisableInterrupts(tmdlHdmiIWDeviceInterrupt_t device);

/*                                                                             
                                                                       

          

                            
                                     

                                                                             */
	void tmdlHdmiTxIWEnableInterrupts(tmdlHdmiIWDeviceInterrupt_t device);

#ifdef __cplusplus
}
#endif
#endif				/*                 */
/*                                                                            *//*                                                                            *//*                                                                            */
