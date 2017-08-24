/**
 * Copyright (C) 2007 NXP N.V., All Rights Reserved.
 * This source code and any compilation or derivative thereof is the proprietary
 * information of NXP N.V. and is confidential in nature. Under no circumstances
 * is this software to be  exposed to or placed under an Open Source License of
 * any type without the expressed written permission of NXP N.V.
 *
 * \file          tmdlHdmiTx_Functions.h
 *
 * \version       $Revision: 1 $
 *
 * \date          $Date: 02/08/07 8:32 $
 *
 * \brief         devlib driver component API for the TDA998x HDMI Transmitters
 *
 * \section refs  Reference Documents
 * HDMI Tx Driver - FRS.doc,
 * HDMI Tx Driver - tmdlHdmiTx - SCS.doc
 *
 * \section info  Change Information
 *
 * \verbatim

   $History: tmdlHdmiTx_Functions.h $
 *
 * *****************  Version 1  *****************
 * User: Demoment     Date: 02/08/07   Time: 8:32
 * Updated in $/Source/tmdlHdmiTx/inc
 * initial version
 *

   \endverbatim
 *
*/

#ifndef TMDLHDMITX_FUNCTIONS_H
#define TMDLHDMITX_FUNCTIONS_H

/*                                                                            */
/*                                                                            */
/*                                                                            */

#include "tmNxTypes.h"
#include "tmdlHdmiTx_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*                                                                            */
/*                                                                            */
/*                                                                            */

/*                                                                           */
/* 
                                                  

                                                       

                            
                                     
                                                                   
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetSWVersion(tmSWVersion_t *pSWVersion);

/*                                                                           */
/* 
                                                                               
                                                 

                                                               

                            
                                     
                                                                   
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetNumberOfUnits(UInt32 *pUnitCount);

/*                                                                           */
/* 
                                                                         
                                                                 

                                                               

                            
                                     
                                                                     
                                               
                                                                   
                   
                                                                     
               
                                                                  
                                                                 
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetCapabilities(tmdlHdmiTxCapabilities_t *pCapabilities);

/*                                                                           */
/* 
                                                                           
                                                                
           

                                           
                                                               

                            
                                     
                                                                     
                                               
                                                                   
                   
                                                                     
               
                                                                  
                                                                 
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetCapabilitiesM
	    (tmUnitSelect_t unit, tmdlHdmiTxCapabilities_t *pCapabilities);

/*                                                                           */
/* 
                                                                           
                                                                    
                                                                       

                                                                           
                  

                            
                                     
                                                                     
                                                  
                                                               
                                                                   
                   
                                                                       
                                                                
                   
                                                                  
                                                              
                                                                    
                                                                  
                                                                  
                                                                  
               
                                                               
                  
                                                                  
                                            
                                                                   
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxOpen(tmInstance_t *pInstance);

/*                                                                           */
/* 
                                                                          
                                                                     
                                                                        
                                                            
                        

                                                                            
                  
                                              

                            
                                     
                                                                     
                                                  
                                                               
                                                                   
                   
                                                                       
                                                                
                   
                                                                  
                                                              
                                                                    
                                                                  
                                                                  
                                                                  
               
                                                               
                  
                                                                  
                                            
                                                                   
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxOpenM(tmInstance_t *pInstance, tmUnitSelect_t unit);

/*                                                                           */
/* 
                                              

                                        

                            
                                     
                                                                      
                   
                                                                    
                   
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxClose(tmInstance_t instance);

/*                                                                           */
/* 
                                                                      

                                          
                                         

                            
                                     
                                                                      
                   
                                                               
                                                                  
                                                                 
                                                                  
                                                                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetPowerState(tmInstance_t instance, tmPowerState_t powerState);

/*                                                                           */
/* 
                                                                      

                                           
                                                  

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                  
                                                                 
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetPowerState(tmInstance_t instance, tmPowerState_t *pPowerState);

/*                                                                           */
/* 
                                                                         
                                                                        
          

                                           

                            
                                     
                                                                      
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxInstanceConfig(tmInstance_t instance);

/*                                                                           */
/* 
                                                                              
                                                                    
                                                        

                                          
                                                                               

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                    
                   
                                                                
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxInstanceSetup
	    (tmInstance_t instance, tmdlHdmiTxInstanceSetupInfo_t *pSetupInfo);

/*                                                                           */
/* 
                                         

                                          
                                                                      
                   

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                    
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetInstanceSetup
	    (tmInstance_t instance, tmdlHdmiTxInstanceSetupInfo_t *pSetupInfo);

/*                                                                           */
/* 
                                                                             
                                                                    
                      

                                          

                            
                                     
                                                                      
                   
                                                               
                                                                    
                   
                                                

                                                                              */
	tmErrorCode_t tmdlHdmiTxHandleInterrupt(tmInstance_t instance);

/*                                                                           */
/* 
                                                                            
                                                                
                                                                   
                                     
                                 
                                  

                                          
                                                                              
                        

                            
                                     
                                                                      
                   
                                                               
                                                                    
                   
                                                                
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxRegisterCallbacks
	    (tmInstance_t instance, ptmdlHdmiTxCallback_t pCallback);

/*                                                                           */
/* 
                                                                          
                                           

                                        
                                    

                            
                                     
                                                                      
                   
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxEnableEvent(tmInstance_t instance, tmdlHdmiTxEvent_t event);

/*                                                                           */
/* 
                                                                           
                                           

                                        
                                     

                            
                                     
                                                                      
                   
                                                               

                                                                             */
	tmErrorCode_t tmdlHdmiTxDisableEvent(tmInstance_t instance, tmdlHdmiTxEvent_t event);

/*                                                                           */
/* 
                                                                          
                                                               
                                                           
                                 
                                  

                                                
                                                                        
                                                                     

                            
                                     
                                                                      
                   
                                                                   
                   
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetVideoFormatSpecs
	    (tmInstance_t instance,
	     tmdlHdmiTxVidFmt_t resolutionID, tmdlHdmiTxVidFmtSpecs_t *pResolutionSpecs);

/*                                                                           */
/* 
                                                                             
                                                                    
                                                                  
           
                                 
                                      

                                                 
                                                              
                                                               
                                                              
                                                                            

                            
                                     
                                                                      
                   
                                                               
                                                                  
                                                                 
                                                                  
                                                                   
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetInputOutput
	    (tmInstance_t instance,
	     tmdlHdmiTxVideoInConfig_t videoInputConfig,
	     tmdlHdmiTxVideoOutConfig_t videoOutputConfig,
	     tmdlHdmiTxAudioInConfig_t audioInputConfig, tmdlHdmiTxSinkType_t sinkType);

/*                                                                           */
/* 
                                                                 
                                                                    
                              
                                 
                                      

                                                 
                                                              
                                                                            

                            
                                     
                                                                      
                   
                                                               
                                                                  
                                                                 
                                                                  
                                                                   
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetAudioInput
	    (tmInstance_t instance,
	     tmdlHdmiTxAudioInConfig_t audioInputConfig, tmdlHdmiTxSinkType_t sinkType);

/*                                                                           */
/* 
                                                                        
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetVideoInfoframe
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxAviIfData_t *pAviIfData);

/*                                                                           */
/* 
                                                                        
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetAudioInfoframe
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxAudIfData_t *pAudIfData);

/*                                                                           */
/* 
                                                                           
                      
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetACPPacket
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxAcpPktData_t *pAcpPktData);

/*                                                                           */
/* 
                                                                             
           
                                 
                                      

                                            
                                                           
                                                                                  

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetGeneralControlPacket
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxGcpPktData_t *pGcpPktData);

/*                                                                           */
/* 
                                                                       
                                 
                                      

                                                
                                                               
                                                                                      

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetISRC1Packet
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxIsrc1PktData_t *pIsrc1PktData);

/*                                                                           */
/* 
                                                                       
                                 
                                      

                                                
                                                               
                                                                                      

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetISRC2Packet
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxIsrc2PktData_t *pIsrc2PktData);

/*                                                                           */
/* 
                                                                        
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetMPSInfoframe
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxMpsIfData_t *pMpsIfData);

/*                                                                           */
/* 
                                                                        
                                 
                                      

                                            
                                                           
                                                                         
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetSpdInfoframe
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxSpdIfData_t *pSpdIfData);

/*                                                                           */
/* 
                                                                       
                                 
                                      

                                            
                                                           
                                                                        
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetVsInfoframe
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxVsPktData_t *pVsIfData);

/*                                                                           */
/* 
                                                                              
                                 
                                      

                                         
                                                     

                            
                                     
                                                                      
                   
                                                               
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxDebugSetNullPacket(tmInstance_t instance, Bool enable);

/*                                                                           */
/* 
                                                                     
                                 
                                      

                                         

                            
                                     
                                                                      
                   
                                                               
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxDebugSetSingleNullPacket(tmInstance_t instance);

/*                                                                           */
/* 
                                                                              
                                                                       
                                                          
                                 
                                      

                                          
                                               

                            
                                     
                                                                      
                   
                                                               
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetAudioMute(tmInstance_t instance, Bool audioMute);

/*                                                                           */
/* 
                           
                                 
                                      

                                          

                            
                                     
                                                                      
                   
                                                               
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      


                                                                             */
	tmErrorCode_t tmdlHdmiTxResetAudioCts(tmInstance_t instance);

/*                                                                           */
/* 
                                            
                                 
                                  

                                                
                                                                                   
                                                                                  
                       

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                 
                                                                  
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidStatus
	    (tmInstance_t instance, tmdlHdmiTxEdidStatus_t *pEdidStatus, UInt8 *pEdidBlkCount);

/*                                                                           */
/* 
                                                                          
                                                          
                                 
                                      


                                                    
                                                                            
                
                                                  
                                                                                   
                                  
                                                                                        
                                
                                                              
                                                                               
                                                                         
                                              
                                                                          
                                                            
              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidAudioCaps
	    (tmInstance_t instance,
	     tmdlHdmiTxEdidAudioDesc_t *pAudioDescs,
	     UInt maxAudioDescs, UInt *pWrittenAudioDescs, UInt8 *pAudioFlags);

/*                                                                           */
/* 
                                                                     
                                                                      
                      
                                 
                                      

                                                    
                                                                                
                      
                                                  
                                                                                   
                                  
                                                                                      
                           
                             
                             

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidVideoCaps
	    (tmInstance_t instance,
	     tmdlHdmiTxShortVidDesc_t *pVideoDesc,
	     UInt maxVideoFormats, UInt *pWrittenVideoFormats, UInt8 *pVideoFlags);

/*                                                                           */
/* 
                                                                     
                                                                      
                      
                                 
                                      

                                                        
                                                                             
                          

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidVideoPreferred
	    (tmInstance_t instance, tmdlHdmiTxEdidVideoTimings_t *pNativeVideoFormat);

/*                                                                           */
/* 
                                                                           
                                                                   
                                 
                                      

                                         
                                                                      

                            
                                     
                                                                      
                   
                                                               
                                                                     
               
                                                                    
                                                                     
                                                                            
                                                                
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidSinkType
	    (tmInstance_t instance, tmdlHdmiTxSinkType_t *pSinkType);

/*                                                                           */
/* 
                                                               
                                                                   
                                 
                                      

                                                
                                                                                    
               

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidSourceAddress
	    (tmInstance_t instance, UInt16 *pSourceAddress);



/*                                                                           */
/* 
                                                    
                                 
                                      

                                            
                                                                            
                                                                       
                                                                    
                                              

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                    

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetKsvList
	    (tmInstance_t instance,
	     UInt8 *pKsv,
	     UInt8 maxKsv,
	     UInt8 *pWrittenKsv, UInt8 *pDepth, Bool *pMaxCascExd, Bool *pMaxDevsExd);
#ifdef HDMI_TX_REPEATER_ISR_MODE
/*                                                                             
                                                      
                                 
                                      

                                            
                                                                     

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                    

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetDepth(tmInstance_t instance, UInt8 *pDepth);

/*                                                                             
                                                   
                                 
                                      

                                            


                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                    

                                                                             */
	tmErrorCode_t tmdlHdmiTxGeneSHA_1_IT(tmInstance_t instance);
#endif				/*                           */
/*                                                                           */
/* 
                                          
                                 
                                      

                                          
                                                           

                            
                                     
                                                                      
                   
                                                               
                                                                
                   
                                                                      
                                                                    
                                                                  
                                                                 
                                                                  
                                                                   
                                                               
                                                                 
                                                                 

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetHdcp(tmInstance_t instance, Bool hdcpEnable);

/*                                                                           */
/* 
                                     
                                 
                                      

                                                 
                                                                                           

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                    

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetHdcpState
	    (tmInstance_t instance, tmdlHdmiTxHdcpCheck_t *pHdcpCheckState);

/*                                                                           */
/* 
                                                                    
                                                                       
                                                                     
                                                     
                                 
                                      

                                                    
                                                                            
                                

                            
                                     
                                                                      
                   
                                                               
                                                                    
                                                                  
                                                                 
                                                                  
                                                               
                                                                
                                                                 

                                                                             */
	tmErrorCode_t tmdlHdmiTxHdcpCheck(tmInstance_t instance, UInt16 timeSinceLastCall);

/*                                                                           */
/* 
                                                                           
                                                                     
             
                                 
                                      

                                            
                                                                       
                                                                          
              

                            
                                     
                                                                      
                   
                                                               
                                                                
                   
                                                                   
                   
                                                                 
                                                                  
                                                                   
                                                                  
                                                                  
                                                                      

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetGamutPacket
	    (tmInstance_t instance, Bool enable, tmdlHdmiTxGamutData_t *pGamutData);

/*                                                                           */
/* 
                                                                               
                                                                    
                                                                    
                                                                             
                                                                        
                                                                           
                                                                   

                                 
                                      

                                                     
                                                                     
                                                                                      
                                                       
                                                                                   
                

                            
                                    
                                                                  
                                                                   
                                                                            
                                                                
                                                                 
                                                                  
                                                                 
                                                                 
                                                                     

                                                                             */
	tmErrorCode_t tmdlHdmiTxSetExtendedColorimetry
	    (tmInstance_t instance,
	     Bool enable,
	     tmdlHdmiTxExtColorimetry_t extendedColorimetry,
	     tmdlHdmiTxYCCQR_t yccQR, tmdlHdmiTxGamutData_t *pGamutData);

/*                                                                           */
/* 
                                                               
                                                                      
                      
                                 
                                      

                                                
                                                                            
                          
                                              
                                                                               
                                     

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidDetailledTimingDescriptors
	    (tmInstance_t instance,
	     tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors,
	     UInt8 maxDTDesc, UInt8 *pWrittenDTDesc);

/*                                                                           */
/* 
                                                                       
                                                     
                      
                                 
                                      

                                                
                                                                                    
                   
                                                                                     
                   
                                                                                    
                   
                                              
                                                                               
                                     

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidMonitorDescriptors
	    (tmInstance_t instance,
	     tmdlHdmiTxEdidFirstMD_t *pEdidFirstMD,
	     tmdlHdmiTxEdidSecondMD_t *pEdidSecondMD,
	     tmdlHdmiTxEdidOtherMD_t *pEdidOtherMD, UInt8 maxOtherMD, UInt8 *pWrittenOtherMD);

/*                                                                           */
/* 
                                                           
                                                     
                      
                                 
                                      

                                                    
                                                                                          
          

                            
                                     
                                                                      
                   
                                                               
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidTVPictureRatio
	    (tmInstance_t instance, tmdlHdmiTxPictAspectRatio_t *pEdidTvPictureRatio);

/*                                                                             
                                                             
                                 
                                      

                                            
                                                                          
                                                  

                            
                                                           
                                                     
                                      
                                                                             
                                                                                     
                                                         
                                                                      
                   
                                                               
                                                                   
                   


                                                                             */

	tmErrorCode_t tmdlHdmiTxSetHDCPRevocationList(tmInstance_t instance,
						      void *listPtr, UInt32 length);


/*                                                                           */
/* 
                                                                              
                                                              
                

                                           
                                                                 
                                                                           

                            
                                     
                                                                      
                   
                                                               
                                                                     
               
                                                                            

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetHdcpFailStatus
	    (tmInstance_t instance, tmdlHdmiTxHdcpStatus_t *pHdcpStatus, UInt8 *pRawStatus);


/*                                                                           */
/* 
                                                              
                                 
                                      


                                                    
                                                                                  
                

                            
                                     
                                                                      
                   
                                                                   
                   
                                                                
                   
                                                            
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidLatencyInfo
	    (tmInstance_t instance, tmdlHdmiTxEdidLatency_t *pLatency);


/*                                                                             
                                                                             
                                                          
                                 
                                      

                                                    
                                                                                

                            
                                     
                                                                      
                   
                                                                   
                   
                                                                
                   
                                                                 
                                                                  
                                                            
                                                                  

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetEdidExtraVsdbData
	    (tmInstance_t instance, tmdlHdmiTxEdidExtraVsdbData_t **pExtraVsdbData);


/*                                                                             
                                            
                                 
                                      

                                            

                            
                                                           
                                                     
                                      
                                                                      
                   


                                                                             */

	tmErrorCode_t tmdlHdmiTxSetBScreen(tmInstance_t instance, tmdlHdmiTxTestPattern_t pattern);

/*                                                                             
                                                    
                                 
                                      

                                            

                            
                                                           
                                                     
                                      
                                                                      
                   


                                                                             */

	tmErrorCode_t tmdlHdmiTxRemoveBScreen(tmInstance_t instance);

/*                                                                             
                                           
                                 
                                      

                                                
                                              
                            
               


                                                                             */
	tmdlHdmiTxVidFmt_t tmdlHdmiTxConvertDTDtoCEA
	    (tmInstance_t instance, tmdlHdmiTxEdidVideoTimings_t *pDTDescriptors);


/*                                                                           */
/* 
                                           
                                 
                                  

                                                
                                                                                     

                            
                                     
                                                                      
                   
                                                                   
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetHPDStatus
	    (tmInstance_t instance, tmdlHdmiTxHotPlug_t *pHPDStatus);


/*                                                                           */
/* 
                                               
                                 
                                  

                                                
                                                                                         

                            
                                     
                                                                      
                   
                                                                   
                   

                                                                             */
	tmErrorCode_t tmdlHdmiTxGetRXSenseStatus
	    (tmInstance_t instance, tmdlHdmiTxRxSense_t *pRXSenseStatus);


/*                                                                             
                                           

                                                
                                                      

               

                                                                             */
	tmErrorCode_t tmdlHdmiTxTmdsSetOutputsMute(tmInstance_t instance, Bool muteTmdsOut);

#ifdef __cplusplus
}
#endif
#endif				/*                        */
/*                                                                            *//*                                                                            *//*                                                                            */
