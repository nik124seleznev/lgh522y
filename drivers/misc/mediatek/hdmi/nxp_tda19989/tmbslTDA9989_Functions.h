/**
 * Copyright (C) 2009 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmbslTDA9989_functions.h
 *
 * \version       $Revision: 2 $
 *
 *
*/

#ifndef TMBSLTDA9989_FUNCTIONS_H
#define TMBSLTDA9989_FUNCTIONS_H

/*                                                                            */
/*                                                                            */
/*                                                                            */

#include "tmNxCompId.h"
#include "tmbslHdmiTx_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                            */
/* 
                                                                      

                                                   

                                
                                  
                                     
                                                                 
                                                                 
                                                               
                                                         
 */
	tmErrorCode_t tmbslTDA9989AudioInResetCts(tmUnitSelect_t txUnit);

/* 
                                                               

                                                   
                                              
                                           
                                                  
                                                  
                                              
                                         
                                         
                                                   
                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
                                                         
 */
	tmErrorCode_t
	    tmbslTDA9989AudioInSetConfig
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxaFmt_t aFmt,
	     tmbslHdmiTxI2sFor_t i2sFormat,
	     UInt8 chanI2s,
	     UInt8 chanDsd,
	     tmbslHdmiTxClkPolDsd_t clkPolDsd,
	     tmbslHdmiTxSwapDsd_t swapDsd,
	     UInt8 layout, UInt16 latency_rd, tmbslHdmiTxDstRate_t dstRate);


/* 
                                                                    

                                                   
                                                             
                                                        
                                               
                                                     
                                                   
                                                     
                                                         
                                                          
                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
                                                         
 */
	tmErrorCode_t
	    tmbslTDA9989AudioInSetCts
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxctsRef_t ctsRef,
	     tmbslHdmiTxafs_t afs,
	     tmbslHdmiTxVidFmt_t voutFmt,
	     tmbslHdmiTxVfreq_t voutFreq,
	     UInt32 uCts,
	     UInt16 uCtsX,
	     tmbslHdmiTxctsK_t ctsK, tmbslHdmiTxctsM_t ctsM, tmbslHdmiTxDstRate_t dstRate);

/**
    \brief      Set the Channel Status Bytes 0,1,3 & 4

    \param[in]  txUnit              Transmitter unit number
    \param[in]  copyright           Byte 0 Copyright bit (bit2)
    \param[in]  formatInfo          Byte 0 Audio sample format (bit1) and additional info (bit345)
    \param[in]  categoryCode        Byte 1 Category code (bits8-15)
    \param[in]  sampleFreq          Byte 3 Sample Frequency (bits24-27)
    \param[in]  clockAccuracy       Byte 3 Clock Accuracy (bits38-31)
    \param[in]  maxWordLength       Byte 4 Maximum word length (bit32)
    \param[in]  wordLength          Byte 4 Word length (bits33-35)
    \param[in]  origSampleFreq      Byte 4 Original Sample Frequency (bits36-39)

    \return     The call result:
		- TM_OK: the call was successful
		- Else a problem has been detected:
		  - TMBSL_ERR_HDMI_BAD_UNIT_NUMBER: bad transmitter unit number
		  - TMBSL_ERR_HDMI_BAD_PARAMETER: a parameter was out of range
		  - TMBSL_ERR_HDMI_NOT_INITIALIZED: transmitter not initialized
		  - TMBSL_ERR_HDMI_I2C_WRITE: failed when writing to the I2C bus
		  - TMBSL_ERR_HDMI_OPERATION_NOT_PERMITTED: in DVI mode

    \note       The consumer use bit (bit0) and Mode bits (bits6-7) are forced to zero.
		Use tmbslTDA9989AudioOutSetChanStatusMapping to set CS Byte 2.

 */
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
	     tmbslHdmiTxCSwordLength_t wordLength, tmbslHdmiTxCSorigAfs_t origSampleFreq);

/* 
                                                             

                                                           
                                                                                    
                                                                                     
                                                                                    
                                                                                     
                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
                                                         

                                                                               
                                                       

 */
	tmErrorCode_t
	    tmbslTDA9989AudioOutSetChanStatusMapping
	    (tmUnitSelect_t txUnit,
	     UInt8 sourceLeft[4], UInt8 channelLeft[4], UInt8 sourceRight[4], UInt8 channelRight[4]
	    );

/* 
                                                                               
                   

                                                   
                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
                                                         

                                                                                
                                      

 */
	tmErrorCode_t tmbslTDA9989AudioOutSetMute(tmUnitSelect_t txUnit, tmbslHdmiTxaMute_t aMute);


/*                                                                            */
/* 
                                                                         
          

                                              

                                
                                  
                                     
                                                                 
                                                                 
                                                                  
 */
	tmErrorCode_t tmbslTDA9989Deinit(tmUnitSelect_t txUnit);


/* 
                                                                       

                                                   
                                                                             
                                        
                                                                          
                                
                                                                           
                                                                               
            

                                
                                  
                                     
                                                                
                                                                 
                                                              
                                                           
                                                                 

                   
                                          
                                                
                                                      
                                             
     
                                                
                                                    
                                             
                                                       
                
 */
	tmErrorCode_t
	    tmbslTDA9989EdidGetAudioCapabilities
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxEdidSad_t *pEdidAFmts,
	     UInt aFmtLength, UInt *pAFmtsAvail, UInt8 *pAudioFlags);

/*                                                                            */
/* 
                                        

                                                           
                                                                               
                   

                                
                                  
                                     
                                                                
                                                                 
                                                              
                                                           
                                                                 
 */
	tmErrorCode_t
	    tmbslTDA9989EdidGetBlockCount(tmUnitSelect_t txUnit, UInt8 *puEdidBlockCount);

/*                                                                            */
/* 
                                   

                                                           
                                                                               
                   

                                
                                  
                                     
                                                                
                                                                 
                                                                 
 */
	tmErrorCode_t tmbslTDA9989EdidGetStatus(tmUnitSelect_t txUnit, UInt8 *puEdidStatus);

/* 
                                                                        
                                                 
                        


                                                   
                                                                                
                         
                                                    
                                                                  
                                                                               
                                                

                                
                                  
                                     
                                                                
                                                                 
                                                              
                                                                 
                                                          

                  

 */

	tmErrorCode_t tmbslTDA9989EdidRequestBlockData(tmUnitSelect_t txUnit, UInt8 *pRawEdid, Int numBlocks,	/*                                 */
						       Int lenRawEdid	/*                                 */
	    );


/* 
                                                         

                                                   
                                                                      

                                
                                  
                                     
                                                                
                                                                 
                                                              
                                                           
                                                                 
                                                         

                                            
 */
	tmErrorCode_t
	    tmbslTDA9989EdidGetSinkType(tmUnitSelect_t txUnit, tmbslHdmiTxSinkType_t *pSinkType);

/*                                                                            */
/* 
                                                                       

                                                       
                                                                                   

                                
                                  
                                     
                                                                
                                                                 
                                                              
                                                           
                                                                 

                                            
 */
	tmErrorCode_t
	    tmbslTDA9989EdidGetSourceAddress(tmUnitSelect_t txUnit, UInt16 *pSourceAddress);

/* 
                                                                       

                                                   
                                                                               
                        
                                                                          
                                
                                                                           
                                                                             
                              
                                
                                

                                
                                  
                                     
                                                                
                                                                 
                                                              
                                                           
                                                                 
                                                         

                   
                                          
                                                                 
                                                       
     
                                                         
                                                       
                
                                            
 */
	tmErrorCode_t
	    tmbslTDA9989EdidGetVideoCapabilities
	    (tmUnitSelect_t txUnit,
	     UInt8 *pEdidVFmts, UInt vFmtLength, UInt *pVFmtsAvail, UInt8 *pVidFlags);
/* 
                                                                        

                                                   
                                                                                          

                                                                      

                                                                        

                                
                                  
                                     
                                                                
                                                                 
                                                              
                                                           
                                                                 
*/
	tmErrorCode_t
	    tmbslTDA9989EdidGetDetailedTimingDescriptors
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxEdidDtd_t *pEdidDTD, UInt8 nb_size, UInt8 *pDTDAvail);

/* 
                                                                         

                                                       
                                                                       
                                                                        
                                                                       
                                        
                                        

                                
                                  
                                     
                                                                
                                                                 
                                                              
                                                           
                                                                 
*/
	tmErrorCode_t
	    tmbslTDA9989EdidGetMonitorDescriptors
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxEdidFirstMD_t *pEdidFirstMD,
	     tmbslHdmiTxEdidSecondMD_t *pEdidSecondMD,
	     tmbslHdmiTxEdidOtherMD_t *pEdidOtherMD, UInt8 sizeOtherMD, UInt8 *pOtherMDAvail);

/* 
                                                                      

                                                       
                                                                       

                                
                                  
                                     
                                                                
                                                                 
                                                           
                                                                 
*/
	tmErrorCode_t
	    tmbslTDA9989EdidGetBasicDisplayParam
	    (tmUnitSelect_t txUnit, tmbslHdmiTxEdidBDParam_t *pEdidBDParam);


/* 
                                                                    

                                                   
                                                                            
                                                          
             

                                
                                  
                                     
                                                                
                                                                 
                                                              
                                                           
                                                                 
                                                         

                   
                                                         
                                                  
                                                  
                                                    
                                               
                                                 
                                                         
                                                              
                                                      
                                                           
                                                    
                                                  
                                                    
                                                  
                                             
                
                                            
 */
	tmErrorCode_t
	    tmbslTDA9989EdidGetVideoPreferred
	    (tmUnitSelect_t txUnit, tmbslHdmiTxEdidDtd_t *pEdidDTD);

/* 
                                                                         
                                                                   
                                                 
                                            

                                                               
                                                                             
                                                                

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                              
                                                               
                                                                
 */
	tmErrorCode_t
	    tmbslTDA9989HdcpCheck
	    (tmUnitSelect_t txUnit, UInt16 uTimeSinceLastCallMs, tmbslHdmiTxHdcpCheck_t *pResult);

/* 
                                                 
                                                 
                                            

                                                       
                                                     
                                                              
                                                                          
                                                                    
                                                                             
                                              
                                         

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                              
                                                               
                                                                

                                                         
 */
	tmErrorCode_t
	    tmbslTDA9989HdcpConfigure
	    (tmUnitSelect_t txUnit,
	     UInt8 slaveAddress,
	     tmbslHdmiTxHdcpTxMode_t txMode,
	     tmbslHdmiTxHdcpOptions_t options, UInt16 uCheckIntervalMs, UInt8 uChecksToDo);

/* 
                                                                         
                                                 
                                            


                                                       
                                          
                                                                                  

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                              
                                                               
                                                                
 */
	tmErrorCode_t
	    tmbslTDA9989HdcpDownloadKeys
	    (tmUnitSelect_t txUnit, UInt16 seed, tmbslHdmiTxDecrypt_t keyDecryption);

/*                                                                            */
/* 
                                                                              
                     
                                                 
                                            

                                               
                                                     

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                               
                                                                
*/
	tmErrorCode_t tmbslTDA9989HdcpEncryptionOn(tmUnitSelect_t txUnit, Bool bOn);

/*                                                                            */
/* 
                                      
                                                 
                                            

                                                   
                                                  
                                                                            
                         
                         
                         

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                               
                                                                
*/
	tmErrorCode_t
	    tmbslTDA9989HdcpGetOtp(tmUnitSelect_t txUnit, UInt8 otpAddress, UInt8 *pOtpData);

/*                                                                            */
/* 
                                                                          
                                                 
                                            

                                                   
                                                                

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                
*/
	tmErrorCode_t tmbslTDA9989HdcpGetT0FailState(tmUnitSelect_t txUnit, UInt8 *pFailState);

/*                                                                            */
/* 
                                      
                                                 
                                            


                                               

                                
                                  
                                     
                                                                 
                                                                 
                                                              
                                                               

                                                                 
                                                 
                            
*/
	tmErrorCode_t tmbslTDA9989HdcpHandleBCAPS(tmUnitSelect_t txUnit);

/*                                                                            */
/* 
                                   
                                                 
                                            

                                                       
                                                                               
                     
                                                                               
                                                   
                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                              
                                                               

                                                                 
                                                
                             
*/
	tmErrorCode_t
	    tmbslTDA9989HdcpHandleBKSV
	    (tmUnitSelect_t txUnit, UInt8 *pBksv, Bool *pbCheckRequired);

/*                                                                            */
/* 
                                                              
                                                 
                                            


                                                       
                                                                        
                    
                                                 
                                                 

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                               

                                                                 
                                                
                            
*/
	tmErrorCode_t tmbslTDA9989HdcpHandleBKSVResult(tmUnitSelect_t txUnit, Bool bSecure);

/* 
                                        
                                                 
                                            


                                                   
                                                                              

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                              
                                                               

                                                                          
                  
*/
	tmErrorCode_t tmbslTDA9989HdcpHandleBSTATUS(tmUnitSelect_t txUnit, UInt16 *pBstatus);

/*                                                                            */
/* 
                                        
                                                 
                                            

                                               

                                
                                  
                                     
                                                                 
                                                                 
                                                              
                                                               

                                                                          
                  
*/
	tmErrorCode_t tmbslTDA9989HdcpHandleENCRYPT(tmUnitSelect_t txUnit);

/*                                                                            */
/* 
                                   
                                                 
                                            


                                               

                                
                                  
                                     
                                                                 
                                                                 
                                                              
                                                               

                                                                     
                  
*/
	tmErrorCode_t tmbslTDA9989HdcpHandlePJ(tmUnitSelect_t txUnit);

/* 
                                      
                                                 
                                            


                                                       
                                                                              
                                  
                                                  
          
                                                                             
                                                  
                     
                                     
                                                                          
                       
                                                  
             
                                     
                                                     

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                 
                                                              
                                                               

                                                                        
                  
*/
	tmErrorCode_t
	    tmbslTDA9989HdcpHandleSHA_1
	    (tmUnitSelect_t txUnit,
	     UInt8 maxKsvDevices, UInt8 *pKsvList, UInt8 *pnKsvDevices, UInt8 *pDepth);

/*                                                                            */
/* 
                                                                  
                                                 
                                            


                                                       
                                                                            
                    
                                                  
                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                               

                                                                 
                                                
                             
*/
	tmErrorCode_t tmbslTDA9989HdcpHandleSHA_1Result(tmUnitSelect_t txUnit, Bool bSecure);

/*                                                                            */
/* 
                                   
                                                 
                                            

                                                   

                                
                                  
                                     
                                                                 
                                                                 
                                                               

                                                                     
                  
*/
	tmErrorCode_t tmbslTDA9989HdcpHandleT0(tmUnitSelect_t txUnit);

/*                                                                            */
/* 
                                          
                                                 
                                            

                                                   
                                               
                                                     

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                               
                                                                

                                                         
*/
	tmErrorCode_t
	    tmbslTDA9989HdcpInit
	    (tmUnitSelect_t txUnit, tmbslHdmiTxVidFmt_t voutFmt, tmbslHdmiTxVfreq_t voutFreq);

/*                                                                            */
/* 
                                    
                                                 
                                            

                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                               
                                                                

                                                         
*/
	tmErrorCode_t tmbslTDA9989HdcpRun(tmUnitSelect_t txUnit);

/*                                                                            */
/* 
                                                                    
                                                 
                                            

                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                               
                                                                

                                                      
*/
	tmErrorCode_t tmbslTDA9989HdcpStop(tmUnitSelect_t txUnit);
/* 
                                                                           
                                  

                                                       
                                                                          

                                
                                  
                                     
                                                                 
                                                                
                                                                 
 */
	tmErrorCode_t tmbslTDA9989HotPlugGetStatus(tmUnitSelect_t txUnit, tmbslHdmiTxHotPlug_t *pHotPlugStatus, Bool client	/*                                                                  */
	    );

/* 
                                                                           
                                  

                                                       
                                                                          

                                
                                  
                                     
                                                                 
                                                                
                                                                 
 */
	tmErrorCode_t tmbslTDA9989RxSenseGetStatus(tmUnitSelect_t txUnit, tmbslHdmiTxRxSense_t *pRxSenseStatus, Bool client	/*                                                                  */
	    );

/*                                                                            */
/* 
                                                            

                                                   
                                                                               
                                                                               
                                                                          
                                                                            

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                              
                                                               
 */
	tmErrorCode_t
	    tmbslTDA9989HwGetRegisters
	    (tmUnitSelect_t txUnit, Int regPage, Int regAddr, UInt8 *pRegBuf, Int nRegs);

/*                                                                            */
/* 
                                                                         

                                                       
                                                                    

                                
                                  
                                     
                                                                 
                                                                
                                                                 
 */
	tmErrorCode_t tmbslTDA9989HwGetVersion(tmUnitSelect_t txUnit, UInt8 *puDeviceVersion);


/* 
                                                                         

                                                         
                                                           
                                                                    

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                              
 */
	tmErrorCode_t
	    tmbslTDA9989HwGetCapabilities
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxHwFeature_t deviceCapability, Bool *pFeatureSupported);


/*                                                                            */
/* 
                                                                      

                                                   

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                              
                                                                               
                           
 */
	tmErrorCode_t tmbslTDA9989HwHandleInterrupt(tmUnitSelect_t txUnit);


/*                                                                            */
/* 
                                                      

                                                   
                                                                               
                                                                               
                                                                               
                                                                              
                                                         
                                                        
                                                  

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	tmErrorCode_t
	    tmbslTDA9989HwSetRegisters
	    (tmUnitSelect_t txUnit, Int regPage, Int regAddr, UInt8 *pRegBuf, Int nRegs);


/*                                                                            */
/* 
                                                                         
 */
	void
	 tmbslTDA9989HwStartup(void
	    );

/* 
                                                                         
                                                          
                

                                                        
                                                         
                                                             
                                                            
                                                                            
                                                               
                                                                            
                                               
                                               
                                                                          
                                           
                                 
                                     
                                                                                   
                                                                                  

                                
                                  
                                     
                                                                 
                                                     
                                                                 
              
                                                              
                 
                                                                 
                                           
                                                                  
                                                              
 */
	 tmErrorCode_t
	    tmbslTDA9989Init
	    (tmUnitSelect_t txUnit,
	     UInt8 uHwAddress,
	     ptmbslHdmiTxSysFunc_t sysFuncWrite,
	     ptmbslHdmiTxSysFunc_t sysFuncRead,
	     ptmbslHdmiTxSysFuncEdid_t sysFuncEdidRead,
	     ptmbslHdmiTxSysFuncTimer_t sysFuncTimer,
	     tmbslHdmiTxCallbackList_t *funcIntCallbacks,
	     Bool bEdidAltAddr, tmbslHdmiTxVidFmt_t vinFmt, tmbslHdmiTxPixRate_t pixRate);

/* 
                                                              

                                                   
                                                                   

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  

                                                       
                                                               
 */
	 tmErrorCode_t
	    tmbslTDA9989MatrixSetCoeffs(tmUnitSelect_t txUnit, tmbslHdmiTxMatCoeff_t *pMatCoeff);

/* 
                                                               

                                                       
                                                  
                                                
                                                   
                                                 

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 
 */
	 tmErrorCode_t
	    tmbslTDA9989MatrixSetConversion
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxVidFmt_t vinFmt,
	     tmbslHdmiTxVinMode_t vinMode,
	     tmbslHdmiTxVidFmt_t voutFmt, tmbslHdmiTxVoutMode_t voutMode, tmbslHdmiTxVQR_t dviVqr);

/* 
                                                                 

                                                   
                                                              

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 

                                                            
                                                       
 */
	 tmErrorCode_t
	    tmbslTDA9989MatrixSetInputOffset
	    (tmUnitSelect_t txUnit, tmbslHdmiTxMatOffset_t *pMatOffset);

/* 
                                                      

                                                   
                                                              
                                                                                

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 

                  

                  
 */
	 tmErrorCode_t
	    tmbslTDA9989MatrixSetMode
	    (tmUnitSelect_t txUnit, tmbslHdmiTxmCntrl_t mControl, tmbslHdmiTxmScale_t mScale);

/*                                                                            */
/* 
                                                                  

                                                   
                                                              

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 

                                                  
                                                      
 */
	 tmErrorCode_t
	    tmbslTDA9989MatrixSetOutputOffset
	    (tmUnitSelect_t txUnit, tmbslHdmiTxMatOffset_t *pMatOffset);

/*                                                                            */
/* 
                                                            

                                                   
                                                              

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 
                                                              

                                                               
 */
	 tmErrorCode_t tmbslTDA9989PktSetAclkRecovery(tmUnitSelect_t txUnit, Bool bEnable);

/* 
                                                                           
           

                                                   
                                                                   
                                                    
                                                   
                                                              

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                                 
                                                                 
                                                                 
                                                                     

                                                       
                                     

                  
 */
	 tmErrorCode_t
	    tmbslTDA9989PktSetAcp
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxPkt_t *pPkt, UInt byteCnt, UInt8 uAcpType, Bool bEnable);

/* 
                                                                             

                                               
                                                            
                                                          

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 
                                                                     

                                          
                  
                    
                  
                  
                    
                     
                  
 */
	 tmErrorCode_t
	    tmbslTDA9989PktSetAudioInfoframe
	    (tmUnitSelect_t txUnit, tmbslHdmiTxPktAif_t *pPkt, Bool bEnable);


/*                                                                            */
/* 
                                                                       
                  

                                                   
                                                                               
                       
                                                              

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 
                                                                     

                                                                                 
 */
	 tmErrorCode_t
	    tmbslTDA9989PktSetGeneralCntrl
	    (tmUnitSelect_t txUnit, tmbslHdmiTxaMute_t *paMute, Bool bEnable);

/*                                                                            */
/* 
                                                                  

                                                   
                                                                   
                                                    
                                                  
                                           
                                       
                                                              

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                                 
                                                                 
                                                                 
                                                                     

                                                       
                                 

                  
 */
	 tmErrorCode_t
	    tmbslTDA9989PktSetIsrc1
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxPkt_t *pPkt,
	     UInt byteCnt, Bool bIsrcCont, Bool bIsrcValid, UInt8 uIsrcStatus, Bool bEnable);

/*                                                                            */
/* 
                                                                  

                                                   
                                                                   
                                                    
                                                              

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                                 
                                                                 
                                                                 
                                                                     

                                                       
                                     

                  
 */
	 tmErrorCode_t
	    tmbslTDA9989PktSetIsrc2
	    (tmUnitSelect_t txUnit, tmbslHdmiTxPkt_t *pPkt, UInt byteCnt, Bool bEnable);

/* 
                                                                           

                                                       
                                                                   
                                                                  

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 
                                                                 
                                                                     

                                         
                                 
                                   
                                      

                  
 */
	 tmErrorCode_t
	    tmbslTDA9989PktSetMpegInfoframe
	    (tmUnitSelect_t txUnit, tmbslHdmiTxPktMpeg_t *pPkt, Bool bEnable);

/*                                                                            */
/* 
                                            

                                                   
                                                              

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 
                                                                     
 */
	 tmErrorCode_t tmbslTDA9989PktSetNullInsert(tmUnitSelect_t txUnit, Bool bEnable);

/*                                                                            */
/* 
                                                                        
               

                                                   

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 
                                                                     

                                                          
 */
	 tmErrorCode_t tmbslTDA9989PktSetNullSingle(tmUnitSelect_t txUnit);

/* 
                                                                             

                                               
                                                            
                                                          

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 
                                                                 
                                                                     

                                          
                                       
                                       
                                       
 */
	 tmErrorCode_t
	    tmbslTDA9989PktSetSpdInfoframe
	    (tmUnitSelect_t txUnit, tmbslHdmiTxPktSpd_t *pPkt, Bool bEnable);

/* 
                                                                            

                                               
                                                            
                                                          

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                 
                                                                     

                                          
              
                 
               
                
                   
                          
                         
               
                 
                   
                      
                           
                        
                                                              
 */
	 tmErrorCode_t
	    tmbslTDA9989PktSetVideoInfoframe
	    (tmUnitSelect_t txUnit, tmbslHdmiTxPktVif_t *pPkt, Bool bEnable);

/*                                                                            */
/* 
                                                                                      

                                                   
                                                                   
                                                    
                                                            
                                                              

                                
                                  
                                     
                                                                
                                                                 
                                                                  
                                                                                 
                                                                 
                                                                 
                                                                     

                                                       
                                                             

                  
 */
	 tmErrorCode_t
	    tmbslTDA9989PktSetVsInfoframe
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxPkt_t *pPkt, UInt byteCnt, UInt8 uVersion, Bool bEnable);

/*                                                                            */
/* 
                                                                                

                                                   
                                                           
                                                              

                                
                                  
                                     
                                                                 
                                                                  
                                                                 
                                                                     

                                                       
                                     

                  
 */
	tmErrorCode_t tmbslTDA9989PktSetRawVideoInfoframe
	    (tmUnitSelect_t txUnit, tmbslHdmiTxPktRawAvi_t *pPkt, Bool bEnable);


/*                                                                            */
/* 
                                                      

                                                    
                                                                         

                                
                                  
                                     
                                                                 
                                                                
                                                                 

                             
             
                  
 */
	 tmErrorCode_t
	    tmbslTDA9989PowerGetState(tmUnitSelect_t txUnit, tmPowerState_t *pePowerState);

/*                                                                            */
/* 
                                                      

                                                   
                                              

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  

                                                                               
             
                  
                  
              
 */
	 tmErrorCode_t tmbslTDA9989PowerSetState(tmUnitSelect_t txUnit, tmPowerState_t ePowerState);


/*                                                                            */
/* 
                                          

                                                   

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  

                  

                                
 */
	 tmErrorCode_t tmbslTDA9989Reset(tmUnitSelect_t txUnit);

/* 
                                                       
                                                 
                                            

                                                   
                                                                             
                

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                              

                                                      
                                                      
                                                           
                                                          
                                                              
                                                              
                                                              
                                                              
                                                               
                                                                
 */
	 tmErrorCode_t
	    tmbslTDA9989ScalerGet(tmUnitSelect_t txUnit, tmbslHdmiTxScalerDiag_t *pScalerDiag);



/* 
                                           
                                                 
                                            


                                                   
                                                                      

                                
                                  
                                     
                                                                 
                                                                
                                                                 
*/
	 tmErrorCode_t
	    tmbslTDA9989ScalerGetMode(tmUnitSelect_t txUnit, tmbslHdmiTxScaMode_t *pScalerMode);


/*                                                                            */
/* 
                                                    
                                                 
                                            


                                                   
                                                          

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t tmbslTDA9989ScalerInDisable(tmUnitSelect_t txUnit, Bool bDisable);

/* 
                                                                               
                                                 
                                            

                                                   
                                                              
                                                                              
                                                          

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989ScalerSetCoeffs
	    (tmUnitSelect_t txUnit, tmbslHdmiTxScaLut_t lutSel, UInt8 *pVsLut);

/* 
                                          
                                                 
                                            

                                                   
                                                         
                                                         
                                                    
                                                        

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989ScalerSetFieldOrder
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxIntExt_t topExt,
	     tmbslHdmiTxIntExt_t deExt, tmbslHdmiTxTopSel_t topSel, tmbslHdmiTxTopTgl_t topTgl);

/* 
                                                  
                                                 
                                            

                                                    
                                                                             
                                                                          

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989ScalerSetFine(tmUnitSelect_t txUnit, UInt16 uRefPix, UInt16 uRefLine);

/* 
                                                  
                                                 
                                            

                                                               
                                                                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */

	 tmErrorCode_t
	    tmbslTDA9989ScalerSetPhase
	    (tmUnitSelect_t txUnit, tmbslHdmiTxHPhases_t horizontalPhases);

/* 
                                                              
                                                 
                                            

                                                   
                                         

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t tmbslTDA9989ScalerSetLatency(tmUnitSelect_t txUnit, UInt8 scaler_latency);

/* 
                                                  
                                                 
                                            

                                                    
                                                     
                                                                        

                                
                                  
                                     
                                                                 
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989ScalerSetSync
	    (tmUnitSelect_t txUnit, tmbslHdmiTxVsMeth_t method, tmbslHdmiTxVsOnce_t once);


/*                                                                            */
/* 
                                                                         

                                                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
 */
	 tmErrorCode_t tmbslTDA9989SwGetVersion(ptmSWVersion_t pSWVersion);


/*                                                                            */
/* 
                                                                         

                                                    
                                                           

                                
                                  
                                     
                                                                 
                                                                 
 */
	 tmErrorCode_t tmbslTDA9989SysTimerWait(tmUnitSelect_t txUnit, UInt16 waitMs);

/* 
                                                                              
       

                                                   
                                            

                                
                                  
                                     
                                                                 
                                                                
                                                                 
 */
	 tmErrorCode_t
	    tmbslTDA9989TmdsSetOutputs(tmUnitSelect_t txUnit, tmbslHdmiTxTmdsOut_t tmdsOut);

/* 
                                             

                                                   
                                              
                                              

                                
                                  
                                     
                                                                 
                                                                
                                                                 
 */
	 tmErrorCode_t
	    tmbslTDA9989TmdsSetSerializer(tmUnitSelect_t txUnit, UInt8 uPhase2, UInt8 uPhase3);

/* 
                                             

                                                  
                                       

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989TestSetPattern(tmUnitSelect_t txUnit, tmbslHdmiTxTestPattern_t pattern);

/* 
                                                                

                                                   
                                                                          
                                  
                                              

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989TestSetMode
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxTestMode_t testMode, tmbslHdmiTxTestState_t testState);

/* 
                                                   

                                                       
                                                                  
                                           
                                                                         
                     

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  

                  

                  
 */
	 tmErrorCode_t
	    tmbslTDA9989VideoInSetBlanking
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxBlnkSrc_t blankitSource, tmbslHdmiTxBlnkCode_t blankingCodes);

/* 
                                                                       

                                                       
                                                
                                                                                   
                                            
                              
                                        
                                                               
                                             

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989VideoInSetConfig
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxVinMode_t vinMode,
	     tmbslHdmiTxVidFmt_t voutFmt,
	     tmbslHdmiTx3DStructure_t structure3D,
	     tmbslHdmiTxPixEdge_t sampleEdge,
	     tmbslHdmiTxPixRate_t pixRate, tmbslHdmiTxUpsampleMode_t upsampleMode);

/* 
                                       

                                                       
                                                                             
                                                                   

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  

                  

                  
 */
	 tmErrorCode_t
	    tmbslTDA9989VideoInSetFine
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxPixSubpkt_t subpacketCount, tmbslHdmiTxPixTogl_t toggleClk1);

/* 
                                                           

                                                       
                                                                 
                                                                   

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  

                                   

                                                               
                                                             
                                                                
                                                                   
                                                                   

                       

                                                               
                                                              
                                                             
                                                     
                    
 */
	 tmErrorCode_t tmbslTDA9989VideoInSetMapping
#ifdef TMFL_RGB_DDR_12BITS
	 (tmUnitSelect_t txUnit, UInt8 *pSwapTable, UInt8 *pMirrorTable, UInt8 *pMux);
#else
	 (tmUnitSelect_t txUnit, UInt8 *pSwapTable, UInt8 *pMirrorTable);
#endif
/* 
                                                     

                                                             
                                                                               
                                                                               

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  

                                           

                                                               
                              
                                
                                   
                                                                         
                                                          

                               

                                                               
                              
                                  
                                        
                                                                            
                                                          
 */
	 tmErrorCode_t
	    tmbslTDA9989SetVideoPortConfig
	    (tmUnitSelect_t txUnit, UInt8 *pEnaVideoPortTable, UInt8 *pGndVideoPortTable);

/*                                                                            */
/* 
                                                     

                                                             
                                                                                       
                                                                                       

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  

                                           
                                
                                   
                                                                             

                               
                                  
                                        
                                                                           
*/
	 tmErrorCode_t
	    tmbslTDA9989SetAudioPortConfig
	    (tmUnitSelect_t txUnit, UInt8 *pEnaAudioPortTable, UInt8 *pGndAudioPortTable);

/*                                                                            */
/* 
                                                           

                                                             
                                                                                                  
                                                                                                  

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  

                                                
                                
                                   
                                                                                 

                                    
                                  
                                        
                                                                                 
*/
	 tmErrorCode_t
	    tmbslTDA9989SetAudioClockPortConfig
	    (tmUnitSelect_t txUnit,
	     UInt8 *pEnaAudioClockPortTable, UInt8 *pGndAudioClockPortTable);

/* 
                                                        

                                                    
                                         
                                            
                 
                                                                               
                                             

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989VideoInSetSyncAuto
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxSyncSource_t syncSource,
	     tmbslHdmiTxVidFmt_t vinFmt,
	     tmbslHdmiTxVinMode_t vinMode, tmbslHdmiTx3DStructure_t structure3D);

/* 
                                                                 

                                                    
                                         
                                            
                 
                                                                        
                                       
                                            
                                       
                                            
                                            
                                            
                                                                             
                                                                          

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989VideoInSetSyncManual
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxSyncSource_t syncSource,
	     tmbslHdmiTxVsMeth_t syncMethod,
	     tmbslHdmiTxPixTogl_t toggleV,
	     tmbslHdmiTxPixTogl_t toggleH,
	     tmbslHdmiTxPixTogl_t toggleX, UInt16 uRefPix, UInt16 uRefLine);


/*                                                                            */
/* 
                                                    

                                                   
                                                          

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t tmbslTDA9989VideoOutDisable(tmUnitSelect_t txUnit, Bool bDisable);

/* 
                                                                      
                                                             
                            

                                                        
                                                                               
                                                 
                                                                            
                                                              
                                                         
                                          


                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989VideoOutSetConfig
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxSinkType_t sinkType,
	     tmbslHdmiTxVoutMode_t voutMode,
	     tmbslHdmiTxVoutPrefil_t preFilter,
	     tmbslHdmiTxVoutYuvBlnk_t yuvBlank, tmbslHdmiTxVoutQrange_t quantization);

/* 
                                         

                                                   
                                                                                
                                                                              
                                                                       
                                                               
                                                                       

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989VideoOutSetSync
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxVsSrc_t srcH,
	     tmbslHdmiTxVsSrc_t srcV,
	     tmbslHdmiTxVsSrc_t srcX, tmbslHdmiTxVsTgl_t toggle, tmbslHdmiTxVsOnce_t once);

/* 
                                                          

                                                    
                                                                               
                                                                  
                                                    
                                                                                
                                                                           
                      
                                                           
                                                                             
                                       

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                 
                                                                  
 */
	 tmErrorCode_t
	    tmbslTDA9989VideoSetInOut
	    (tmUnitSelect_t txUnit,
	     tmbslHdmiTxVidFmt_t vinFmt,
	     tmbslHdmiTx3DStructure_t structure3D,
	     tmbslHdmiTxScaMode_t scaMode,
	     tmbslHdmiTxVidFmt_t voutFmt,
	     UInt8 uPixelRepeat,
	     tmbslHdmiTxMatMode_t matMode,
	     tmbslHdmiTxVoutDbits_t datapathBits, tmbslHdmiTxVQR_t dviVqr);

/* 
                                                                       

                                                    
                                                                     

                                
                                  
                                     
                                                                 
                                                                 
                                                                  
 */
	 tmErrorCode_t tmbslTDA9989FlagSwInt(tmUnitSelect_t txUnit, UInt32 uSwInt);


/* 
                                          

                                                 
                                                              

                                
                                  
                                     
                                                                 
                                                                 
                                                                  
                                                                               
 */
	 tmErrorCode_t tmbslTDA9989Set5vpower(tmUnitSelect_t txUnit, Bool pwrEnable);

/* 
                                                   

                                                      
                                              
                                                                          

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
                                                                        
 */
	 tmErrorCode_t
	    tmbslTDA9989EnableCallback
	    (tmUnitSelect_t txUnit, tmbslHdmiTxCallbackInt_t callbackSource, Bool enable);

/* 
                                             

                                                  
                                                                   
                                                         

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
                                                      
 */
	 tmErrorCode_t
	    tmbslTDA9989SetColorDepth
	    (tmUnitSelect_t txUnit, tmbslHdmiTxColorDepth colorDepth, Bool termEnable);

/* 
                                                                          

                                                   
                                                                     
                                                    
                                                                    

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
                                                                               
 */
	 tmErrorCode_t
	    tmbslTDA9989SetDefaultPhase
	    (tmUnitSelect_t txUnit,
	     Bool bEnable, tmbslHdmiTxColorDepth colorDepth, UInt8 videoFormat);



/* 
                                                     

                                                    
                                                        

                                
                                  
                                     
                                                                 
                                                                
                                                                 
                                                                  
 */

	 tmErrorCode_t tmbslTDA9989CtlVsInterrupt(tmUnitSelect_t txUnit, Bool uIntFlag);

/*                                                                            */
/* 
                                                                           
                                                                    
                                        

                                         
                                                       
                                                    

                            
                                     
                                                                  
                                               
                                                                  
               
                                                               
          

                                                                              */
	tmErrorCode_t tmbslTDA9989PktFillGamut
	    (tmUnitSelect_t txUnit, tmbslHdmiTxPktGamut_t *pPkt, UInt8 bufSel);

/*                                                                            */
/* 
                                                                              
                                                                     
                                                                      
                

                                         
                                                       
                                                          

                            
                                     
                                                                  
                                               
                                                                  
               
                                                               
          

                                                                              */
	tmErrorCode_t tmbslTDA9989PktSendGamut(tmUnitSelect_t txUnit, UInt8 bufSel, Bool bEnable);


/* 
                                                      

                                           
                                        

                            
                                     
                                                                  
                                               
                                                                  
                                                             

*/
	tmErrorCode_t tmbslTDA9989HdcpGetSinkCategory
	    (tmUnitSelect_t txUnit, tmbslHdmiTxSinkCategory_t *category);


/* 
                                                     

                                                 
                                                          

                            
                                     
                                                                  
                                               
                                                                  
                                                             

*/
	tmErrorCode_t tmbslTDA9989EdidGetLatencyInfo
	    (tmUnitSelect_t txUnit, tmbslHdmiTxEdidLatency_t *pEdidLatency);


/* 
                                                                  

                                                 
                                                     

                            
                                     
                                                                  
                                               
                                                                  
                                                             

*/
	tmErrorCode_t tmbslTDA9989EdidGetExtraVsdbData
	    (tmUnitSelect_t txUnit, tmbslHdmiTxEdidExtraVsdbData_t **pExtraVsdbData);


#ifdef TMFL_HDCP_OPTIMIZED_POWER
/* 
                                                                    

                                                 
                                            

                            
                                     
                                                                  
                                               
                                                                  

*/
	 tmErrorCode_t tmbslTDA9989HdcpPowerDown(tmUnitSelect_t txUnit, Bool requested);
#endif

#ifdef __cplusplus
}
#endif
#endif				/*                          */
/*                                                                            *//*                                                                            *//*                                                                            */
