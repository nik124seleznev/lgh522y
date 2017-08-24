/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_SME_LIB_H__
#define CSR_WIFI_SME_LIB_H__

#include "csr_sched.h"
#include "csr_macro.h"
#include "csr_msg_transport.h"

#include "csr_wifi_lib.h"

#include "csr_wifi_sme_prim.h"
#include "csr_wifi_sme_task.h"


#ifndef CSR_WIFI_SME_LIB_DESTINATION_QUEUE
# ifdef CSR_WIFI_NME_ENABLE
# include "csr_wifi_nme_task.h"
# define CSR_WIFI_SME_LIB_DESTINATION_QUEUE CSR_WIFI_NME_IFACEQUEUE
# else
# define CSR_WIFI_SME_LIB_DESTINATION_QUEUE CSR_WIFI_SME_IFACEQUEUE
# endif
#endif

/*                                                                             
                                         
  
               
                                                                              
                                                                            
  
              
                                                                    
                                                                              */
void CsrWifiSmeFreeUpstreamMessageContents(u16 eventClass, void *message);

/*                                                                             
                                           
  
               
                                                                                
                                                                              
  
              
                                                                      
                                                                              */
void CsrWifiSmeFreeDownstreamMessageContents(u16 eventClass, void *message);

/*                                                                             
                           
                                                                              */
const char* CsrWifiSme80211NetworkTypeToString(CsrWifiSme80211NetworkType value);
const char* CsrWifiSme80211PrivacyModeToString(CsrWifiSme80211PrivacyMode value);
const char* CsrWifiSme80211dTrustLevelToString(CsrWifiSme80211dTrustLevel value);
const char* CsrWifiSmeAmpStatusToString(CsrWifiSmeAmpStatus value);
const char* CsrWifiSmeAuthModeToString(CsrWifiSmeAuthMode value);
const char* CsrWifiSmeBasicUsabilityToString(CsrWifiSmeBasicUsability value);
const char* CsrWifiSmeBssTypeToString(CsrWifiSmeBssType value);
const char* CsrWifiSmeCoexSchemeToString(CsrWifiSmeCoexScheme value);
const char* CsrWifiSmeControlIndicationToString(CsrWifiSmeControlIndication value);
const char* CsrWifiSmeCtsProtectionTypeToString(CsrWifiSmeCtsProtectionType value);
const char* CsrWifiSmeD3AutoScanModeToString(CsrWifiSmeD3AutoScanMode value);
const char* CsrWifiSmeEncryptionToString(CsrWifiSmeEncryption value);
const char* CsrWifiSmeFirmwareDriverInterfaceToString(CsrWifiSmeFirmwareDriverInterface value);
const char* CsrWifiSmeHostPowerModeToString(CsrWifiSmeHostPowerMode value);
const char* CsrWifiSmeIEEE80211ReasonToString(CsrWifiSmeIEEE80211Reason value);
const char* CsrWifiSmeIEEE80211ResultToString(CsrWifiSmeIEEE80211Result value);
const char* CsrWifiSmeIndicationsToString(CsrWifiSmeIndications value);
const char* CsrWifiSmeKeyTypeToString(CsrWifiSmeKeyType value);
const char* CsrWifiSmeListActionToString(CsrWifiSmeListAction value);
const char* CsrWifiSmeMediaStatusToString(CsrWifiSmeMediaStatus value);
const char* CsrWifiSmeP2pCapabilityToString(CsrWifiSmeP2pCapability value);
const char* CsrWifiSmeP2pGroupCapabilityToString(CsrWifiSmeP2pGroupCapability value);
const char* CsrWifiSmeP2pNoaConfigMethodToString(CsrWifiSmeP2pNoaConfigMethod value);
const char* CsrWifiSmeP2pRoleToString(CsrWifiSmeP2pRole value);
const char* CsrWifiSmeP2pStatusToString(CsrWifiSmeP2pStatus value);
const char* CsrWifiSmePacketFilterModeToString(CsrWifiSmePacketFilterMode value);
const char* CsrWifiSmePowerSaveLevelToString(CsrWifiSmePowerSaveLevel value);
const char* CsrWifiSmePreambleTypeToString(CsrWifiSmePreambleType value);
const char* CsrWifiSmeRadioIFToString(CsrWifiSmeRadioIF value);
const char* CsrWifiSmeRegulatoryDomainToString(CsrWifiSmeRegulatoryDomain value);
const char* CsrWifiSmeRoamReasonToString(CsrWifiSmeRoamReason value);
const char* CsrWifiSmeScanTypeToString(CsrWifiSmeScanType value);
const char* CsrWifiSmeTrafficTypeToString(CsrWifiSmeTrafficType value);
const char* CsrWifiSmeTspecCtrlToString(CsrWifiSmeTspecCtrl value);
const char* CsrWifiSmeTspecResultCodeToString(CsrWifiSmeTspecResultCode value);
const char* CsrWifiSmeWepAuthModeToString(CsrWifiSmeWepAuthMode value);
const char* CsrWifiSmeWepCredentialTypeToString(CsrWifiSmeWepCredentialType value);
const char* CsrWifiSmeWmmModeToString(CsrWifiSmeWmmMode value);
const char* CsrWifiSmeWmmQosInfoToString(CsrWifiSmeWmmQosInfo value);
const char* CsrWifiSmeWpsConfigTypeToString(CsrWifiSmeWpsConfigType value);
const char* CsrWifiSmeWpsDeviceCategoryToString(CsrWifiSmeWpsDeviceCategory value);
const char* CsrWifiSmeWpsDeviceSubCategoryToString(CsrWifiSmeWpsDeviceSubCategory value);
const char* CsrWifiSmeWpsDpidToString(CsrWifiSmeWpsDpid value);
const char* CsrWifiSmeWpsRegistrationToString(CsrWifiSmeWpsRegistration value);


/*                                                                             
                                  
                                                            
                                                                              */
const char* CsrWifiSmePrimTypeToString(CsrPrim msgType);

/*                                                                             
                                          
                                                                              */
extern const char *CsrWifiSmeUpstreamPrimNames[CSR_WIFI_SME_PRIM_UPSTREAM_COUNT];
extern const char *CsrWifiSmeDownstreamPrimNames[CSR_WIFI_SME_PRIM_DOWNSTREAM_COUNT];

/*                                                                              

      
                             

             
                                                     
                                                                         

            
                                                                         

                                                                              */
#define CsrWifiSmeActivateReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeActivateReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ACTIVATE_REQ, dst__, src__);

#define CsrWifiSmeActivateReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeActivateReq *msg__; \
        CsrWifiSmeActivateReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeActivateReqSend(src__) \
    CsrWifiSmeActivateReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                             

             
                                                                 

            
                                   
                                              

                                                                              */
#define CsrWifiSmeActivateCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeActivateCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ACTIVATE_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeActivateCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeActivateCfm *msg__; \
        CsrWifiSmeActivateCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeActivateCfmSend(dst__, status__) \
    CsrWifiSmeActivateCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                   

             
                                                               

            
                                                                         

                                                                              */
#define CsrWifiSmeAdhocConfigGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeAdhocConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ADHOC_CONFIG_GET_REQ, dst__, src__);

#define CsrWifiSmeAdhocConfigGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeAdhocConfigGetReq *msg__; \
        CsrWifiSmeAdhocConfigGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeAdhocConfigGetReqSend(src__) \
    CsrWifiSmeAdhocConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                   

             
                                                     

            
                                        
                                                   
                                                                         
                             

                                                                              */
#define CsrWifiSmeAdhocConfigGetCfmCreate(msg__, dst__, src__, status__, adHocConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeAdhocConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ADHOC_CONFIG_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->adHocConfig = (adHocConfig__);

#define CsrWifiSmeAdhocConfigGetCfmSendTo(dst__, src__, status__, adHocConfig__) \
    { \
        CsrWifiSmeAdhocConfigGetCfm *msg__; \
        CsrWifiSmeAdhocConfigGetCfmCreate(msg__, dst__, src__, status__, adHocConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeAdhocConfigGetCfmSend(dst__, status__, adHocConfig__) \
    CsrWifiSmeAdhocConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, adHocConfig__)

/*                                                                              

      
                                   

             
                                                               

            
                                                                              
                                                                         

                                                                              */
#define CsrWifiSmeAdhocConfigSetReqCreate(msg__, dst__, src__, adHocConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeAdhocConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ADHOC_CONFIG_SET_REQ, dst__, src__); \
    msg__->adHocConfig = (adHocConfig__);

#define CsrWifiSmeAdhocConfigSetReqSendTo(dst__, src__, adHocConfig__) \
    { \
        CsrWifiSmeAdhocConfigSetReq *msg__; \
        CsrWifiSmeAdhocConfigSetReqCreate(msg__, dst__, src__, adHocConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeAdhocConfigSetReqSend(src__, adHocConfig__) \
    CsrWifiSmeAdhocConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, adHocConfig__)

/*                                                                              

      
                                   

             
                                                     

            
                                   
                                              

                                                                              */
#define CsrWifiSmeAdhocConfigSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeAdhocConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ADHOC_CONFIG_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeAdhocConfigSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeAdhocConfigSetCfm *msg__; \
        CsrWifiSmeAdhocConfigSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeAdhocConfigSetCfmSend(dst__, status__) \
    CsrWifiSmeAdhocConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                    

             
                                         

            
                                         
                                                               
                                                                     
                                 

                                                                              */
#define CsrWifiSmeAmpStatusChangeIndCreate(msg__, dst__, src__, interfaceTag__, ampStatus__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeAmpStatusChangeInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_AMP_STATUS_CHANGE_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->ampStatus = (ampStatus__);

#define CsrWifiSmeAmpStatusChangeIndSendTo(dst__, src__, interfaceTag__, ampStatus__) \
    { \
        CsrWifiSmeAmpStatusChangeInd *msg__; \
        CsrWifiSmeAmpStatusChangeIndCreate(msg__, dst__, src__, interfaceTag__, ampStatus__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeAmpStatusChangeIndSend(dst__, interfaceTag__, ampStatus__) \
    CsrWifiSmeAmpStatusChangeIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, ampStatus__)

/*                                                                              

      
                                        

             
                                                                             
                                                                           
                                                         
                                                                           
                                   

            
                                           
                                                                            
                                                                    
                                                                 
                                                 
                                                                                
                                                                     
                                                 
                                                                               
                                        

                                                                              */
#define CsrWifiSmeAssociationCompleteIndCreate(msg__, dst__, src__, interfaceTag__, status__, connectionInfo__, deauthReason__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeAssociationCompleteInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ASSOCIATION_COMPLETE_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->connectionInfo = (connectionInfo__); \
    msg__->deauthReason = (deauthReason__);

#define CsrWifiSmeAssociationCompleteIndSendTo(dst__, src__, interfaceTag__, status__, connectionInfo__, deauthReason__) \
    { \
        CsrWifiSmeAssociationCompleteInd *msg__; \
        CsrWifiSmeAssociationCompleteIndCreate(msg__, dst__, src__, interfaceTag__, status__, connectionInfo__, deauthReason__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeAssociationCompleteIndSend(dst__, interfaceTag__, status__, connectionInfo__, deauthReason__) \
    CsrWifiSmeAssociationCompleteIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, connectionInfo__, deauthReason__)

/*                                                                              

      
                                     

             
                                                                             
                                                                     

            
                                         
                                                                          
                                                   
                                                                    

                                                                              */
#define CsrWifiSmeAssociationStartIndCreate(msg__, dst__, src__, interfaceTag__, address__, ssid__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeAssociationStartInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ASSOCIATION_START_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->address = (address__); \
    msg__->ssid = (ssid__);

#define CsrWifiSmeAssociationStartIndSendTo(dst__, src__, interfaceTag__, address__, ssid__) \
    { \
        CsrWifiSmeAssociationStartInd *msg__; \
        CsrWifiSmeAssociationStartIndCreate(msg__, dst__, src__, interfaceTag__, address__, ssid__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeAssociationStartIndSend(dst__, interfaceTag__, address__, ssid__) \
    CsrWifiSmeAssociationStartIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, address__, ssid__)

/*                                                                              

      
                              

             
                                                                             
                                                                         
                                                                            
                                                          
                                                                            
                                                                            
                                                                       
               

            
                                                                                  
                                                                             
                                                                               
                                                                             
                               
                                                               
                                                                                
                                              

                                                                              */
#define CsrWifiSmeBlacklistReqCreate(msg__, dst__, src__, interfaceTag__, action__, setAddressCount__, setAddresses__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeBlacklistReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_BLACKLIST_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->action = (action__); \
    msg__->setAddressCount = (setAddressCount__); \
    msg__->setAddresses = (setAddresses__);

#define CsrWifiSmeBlacklistReqSendTo(dst__, src__, interfaceTag__, action__, setAddressCount__, setAddresses__) \
    { \
        CsrWifiSmeBlacklistReq *msg__; \
        CsrWifiSmeBlacklistReqCreate(msg__, dst__, src__, interfaceTag__, action__, setAddressCount__, setAddresses__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeBlacklistReqSend(src__, interfaceTag__, action__, setAddressCount__, setAddresses__) \
    CsrWifiSmeBlacklistReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, action__, setAddressCount__, setAddresses__)

/*                                                                              

      
                              

             
                                                                         
                                                                        
                            

            
                                            
                                                                             
                                                       
                                           
                                                                  
                                                   
                                                               
                                                                                
                                              

                                                                              */
#define CsrWifiSmeBlacklistCfmCreate(msg__, dst__, src__, interfaceTag__, status__, action__, getAddressCount__, getAddresses__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeBlacklistCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_BLACKLIST_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->action = (action__); \
    msg__->getAddressCount = (getAddressCount__); \
    msg__->getAddresses = (getAddresses__);

#define CsrWifiSmeBlacklistCfmSendTo(dst__, src__, interfaceTag__, status__, action__, getAddressCount__, getAddresses__) \
    { \
        CsrWifiSmeBlacklistCfm *msg__; \
        CsrWifiSmeBlacklistCfmCreate(msg__, dst__, src__, interfaceTag__, status__, action__, getAddressCount__, getAddresses__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeBlacklistCfmSend(dst__, interfaceTag__, status__, action__, getAddressCount__, getAddresses__) \
    CsrWifiSmeBlacklistCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, action__, getAddressCount__, getAddresses__)

/*                                                                              

      
                                       

             
                                                              

            
                                                                         

                                                                              */
#define CsrWifiSmeCalibrationDataGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCalibrationDataGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CALIBRATION_DATA_GET_REQ, dst__, src__);

#define CsrWifiSmeCalibrationDataGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeCalibrationDataGetReq *msg__; \
        CsrWifiSmeCalibrationDataGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCalibrationDataGetReqSend(src__) \
    CsrWifiSmeCalibrationDataGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                       

             
                                                     

            
                                                  
                                                             
                                                                    
                                           
                                                                               
                                                           

                                                                              */
#define CsrWifiSmeCalibrationDataGetCfmCreate(msg__, dst__, src__, status__, calibrationDataLength__, calibrationData__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCalibrationDataGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CALIBRATION_DATA_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->calibrationDataLength = (calibrationDataLength__); \
    msg__->calibrationData = (calibrationData__);

#define CsrWifiSmeCalibrationDataGetCfmSendTo(dst__, src__, status__, calibrationDataLength__, calibrationData__) \
    { \
        CsrWifiSmeCalibrationDataGetCfm *msg__; \
        CsrWifiSmeCalibrationDataGetCfmCreate(msg__, dst__, src__, status__, calibrationDataLength__, calibrationData__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCalibrationDataGetCfmSend(dst__, status__, calibrationDataLength__, calibrationData__) \
    CsrWifiSmeCalibrationDataGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, calibrationDataLength__, calibrationData__)

/*                                                                              

      
                                       

             
                                                         
                                                                      
                                                

            
                                                                                        
                                                                    
                                           
                                                                               
                                                           

                                                                              */
#define CsrWifiSmeCalibrationDataSetReqCreate(msg__, dst__, src__, calibrationDataLength__, calibrationData__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCalibrationDataSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CALIBRATION_DATA_SET_REQ, dst__, src__); \
    msg__->calibrationDataLength = (calibrationDataLength__); \
    msg__->calibrationData = (calibrationData__);

#define CsrWifiSmeCalibrationDataSetReqSendTo(dst__, src__, calibrationDataLength__, calibrationData__) \
    { \
        CsrWifiSmeCalibrationDataSetReq *msg__; \
        CsrWifiSmeCalibrationDataSetReqCreate(msg__, dst__, src__, calibrationDataLength__, calibrationData__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCalibrationDataSetReqSend(src__, calibrationDataLength__, calibrationData__) \
    CsrWifiSmeCalibrationDataSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, calibrationDataLength__, calibrationData__)

/*                                                                              

      
                                       

             
                                                     

            
                                   
                                              

                                                                              */
#define CsrWifiSmeCalibrationDataSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCalibrationDataSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CALIBRATION_DATA_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeCalibrationDataSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeCalibrationDataSetCfm *msg__; \
        CsrWifiSmeCalibrationDataSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCalibrationDataSetCfmSend(dst__, status__) \
    CsrWifiSmeCalibrationDataSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                 

             
                                                             
                            

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiSmeCcxConfigGetReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCcxConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CCX_CONFIG_GET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiSmeCcxConfigGetReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiSmeCcxConfigGetReq *msg__; \
        CsrWifiSmeCcxConfigGetReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCcxConfigGetReqSend(src__, interfaceTag__) \
    CsrWifiSmeCcxConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__)

/*                                                                              

      
                                 

             
                                                     

            
                                         
                                                                          
                                                    
                                          

                                                                              */
#define CsrWifiSmeCcxConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, ccxConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCcxConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CCX_CONFIG_GET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->ccxConfig = (ccxConfig__);

#define CsrWifiSmeCcxConfigGetCfmSendTo(dst__, src__, interfaceTag__, status__, ccxConfig__) \
    { \
        CsrWifiSmeCcxConfigGetCfm *msg__; \
        CsrWifiSmeCcxConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, ccxConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCcxConfigGetCfmSend(dst__, interfaceTag__, status__, ccxConfig__) \
    CsrWifiSmeCcxConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, ccxConfig__)

/*                                                                              

      
                                 

             
                                                             
                            

            
                                                                               
                                                                          
                                          

                                                                              */
#define CsrWifiSmeCcxConfigSetReqCreate(msg__, dst__, src__, interfaceTag__, ccxConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCcxConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CCX_CONFIG_SET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->ccxConfig = (ccxConfig__);

#define CsrWifiSmeCcxConfigSetReqSendTo(dst__, src__, interfaceTag__, ccxConfig__) \
    { \
        CsrWifiSmeCcxConfigSetReq *msg__; \
        CsrWifiSmeCcxConfigSetReqCreate(msg__, dst__, src__, interfaceTag__, ccxConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCcxConfigSetReqSend(src__, interfaceTag__, ccxConfig__) \
    CsrWifiSmeCcxConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, ccxConfig__)

/*                                                                              

      
                                 

             
                                                     

            
                                         
                                                                          
                                                    

                                                                              */
#define CsrWifiSmeCcxConfigSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCcxConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CCX_CONFIG_SET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiSmeCcxConfigSetCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiSmeCcxConfigSetCfm *msg__; \
        CsrWifiSmeCcxConfigSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCcxConfigSetCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiSmeCcxConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                                    

             
                                                                

            
                                                                         

                                                                              */
#define CsrWifiSmeCloakedSsidsGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCloakedSsidsGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CLOAKED_SSIDS_GET_REQ, dst__, src__);

#define CsrWifiSmeCloakedSsidsGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeCloakedSsidsGetReq *msg__; \
        CsrWifiSmeCloakedSsidsGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCloakedSsidsGetReqSend(src__) \
    CsrWifiSmeCloakedSsidsGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                    

             
                                                     

            
                                         
                                                    
                                                                                
                                

                                                                              */
#define CsrWifiSmeCloakedSsidsGetCfmCreate(msg__, dst__, src__, status__, cloakedSsids__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCloakedSsidsGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CLOAKED_SSIDS_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->cloakedSsids = (cloakedSsids__);

#define CsrWifiSmeCloakedSsidsGetCfmSendTo(dst__, src__, status__, cloakedSsids__) \
    { \
        CsrWifiSmeCloakedSsidsGetCfm *msg__; \
        CsrWifiSmeCloakedSsidsGetCfmCreate(msg__, dst__, src__, status__, cloakedSsids__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCloakedSsidsGetCfmSend(dst__, status__, cloakedSsids__) \
    CsrWifiSmeCloakedSsidsGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, cloakedSsids__)

/*                                                                              

      
                                    

             
                                                                             
             
                                                                             
                                                                           
                                                                            
                                                        

            
                                                                               
                                                 

                                                                              */
#define CsrWifiSmeCloakedSsidsSetReqCreate(msg__, dst__, src__, cloakedSsids__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCloakedSsidsSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CLOAKED_SSIDS_SET_REQ, dst__, src__); \
    msg__->cloakedSsids = (cloakedSsids__);

#define CsrWifiSmeCloakedSsidsSetReqSendTo(dst__, src__, cloakedSsids__) \
    { \
        CsrWifiSmeCloakedSsidsSetReq *msg__; \
        CsrWifiSmeCloakedSsidsSetReqCreate(msg__, dst__, src__, cloakedSsids__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCloakedSsidsSetReqSend(src__, cloakedSsids__) \
    CsrWifiSmeCloakedSsidsSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, cloakedSsids__)

/*                                                                              

      
                                    

             
                                                     

            
                                   
                                              

                                                                              */
#define CsrWifiSmeCloakedSsidsSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCloakedSsidsSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CLOAKED_SSIDS_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeCloakedSsidsSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeCloakedSsidsSetCfm *msg__; \
        CsrWifiSmeCloakedSsidsSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCloakedSsidsSetCfmSend(dst__, status__) \
    CsrWifiSmeCloakedSsidsSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                  

             
                                                              

            
                                                                         

                                                                              */
#define CsrWifiSmeCoexConfigGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCoexConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_COEX_CONFIG_GET_REQ, dst__, src__);

#define CsrWifiSmeCoexConfigGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeCoexConfigGetReq *msg__; \
        CsrWifiSmeCoexConfigGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCoexConfigGetReqSend(src__) \
    CsrWifiSmeCoexConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                  

             
                                                     

            
                                       
                                                  
                                                                         
                          

                                                                              */
#define CsrWifiSmeCoexConfigGetCfmCreate(msg__, dst__, src__, status__, coexConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCoexConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_COEX_CONFIG_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->coexConfig = (coexConfig__);

#define CsrWifiSmeCoexConfigGetCfmSendTo(dst__, src__, status__, coexConfig__) \
    { \
        CsrWifiSmeCoexConfigGetCfm *msg__; \
        CsrWifiSmeCoexConfigGetCfmCreate(msg__, dst__, src__, status__, coexConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCoexConfigGetCfmSend(dst__, status__, coexConfig__) \
    CsrWifiSmeCoexConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, coexConfig__)

/*                                                                              

      
                                  

             
                                                              

            
                                                                             
                                                     

                                                                              */
#define CsrWifiSmeCoexConfigSetReqCreate(msg__, dst__, src__, coexConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCoexConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_COEX_CONFIG_SET_REQ, dst__, src__); \
    msg__->coexConfig = (coexConfig__);

#define CsrWifiSmeCoexConfigSetReqSendTo(dst__, src__, coexConfig__) \
    { \
        CsrWifiSmeCoexConfigSetReq *msg__; \
        CsrWifiSmeCoexConfigSetReqCreate(msg__, dst__, src__, coexConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCoexConfigSetReqSend(src__, coexConfig__) \
    CsrWifiSmeCoexConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, coexConfig__)

/*                                                                              

      
                                  

             
                                                     

            
                                   
                                              

                                                                              */
#define CsrWifiSmeCoexConfigSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCoexConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_COEX_CONFIG_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeCoexConfigSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeCoexConfigSetCfm *msg__; \
        CsrWifiSmeCoexConfigSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCoexConfigSetCfmSend(dst__, status__) \
    CsrWifiSmeCoexConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                

             
                                                            

            
                                                                         

                                                                              */
#define CsrWifiSmeCoexInfoGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCoexInfoGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_COEX_INFO_GET_REQ, dst__, src__);

#define CsrWifiSmeCoexInfoGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeCoexInfoGetReq *msg__; \
        CsrWifiSmeCoexInfoGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCoexInfoGetReqSend(src__) \
    CsrWifiSmeCoexInfoGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                

             
                                                     

            
                                     
                                                
                                                                    

                                                                              */
#define CsrWifiSmeCoexInfoGetCfmCreate(msg__, dst__, src__, status__, coexInfo__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCoexInfoGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_COEX_INFO_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->coexInfo = (coexInfo__);

#define CsrWifiSmeCoexInfoGetCfmSendTo(dst__, src__, status__, coexInfo__) \
    { \
        CsrWifiSmeCoexInfoGetCfm *msg__; \
        CsrWifiSmeCoexInfoGetCfmCreate(msg__, dst__, src__, status__, coexInfo__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCoexInfoGetCfmSend(dst__, status__, coexInfo__) \
    CsrWifiSmeCoexInfoGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, coexInfo__)

/*                                                                              

      
                            

             
                                                                      
                                                                       
            
                                                                            
                                                                          
                                                                             
                            
                                                                           
                                                                           
                                                                  
                                                                              
                                                                                 
                                                              
                                                                        
                                                                              
                                                                             
                                                                             
                                                                                
                                                                              
                             
                                                                               
                                                                             
                                                                               
                                                                             
                              
                                                                          
                                                                          
                                                                          
                                                                          
              
                                                                         
                                                                         
                                   
                                                                             
                                                                          
            
                                                                         

            
                                                                                   
                                                                              
                                                                          

                                                                              */
#define CsrWifiSmeConnectReqCreate(msg__, dst__, src__, interfaceTag__, connectionConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeConnectReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CONNECT_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->connectionConfig = (connectionConfig__);

#define CsrWifiSmeConnectReqSendTo(dst__, src__, interfaceTag__, connectionConfig__) \
    { \
        CsrWifiSmeConnectReq *msg__; \
        CsrWifiSmeConnectReqCreate(msg__, dst__, src__, interfaceTag__, connectionConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeConnectReqSend(src__, interfaceTag__, connectionConfig__) \
    CsrWifiSmeConnectReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, connectionConfig__)

/*                                                                              

      
                            

             
                                                                            
                                 

            
                                         
                                                                          
                                                     
                                                                            
                                                 

                                                                              */
#define CsrWifiSmeConnectCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeConnectCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CONNECT_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiSmeConnectCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiSmeConnectCfm *msg__; \
        CsrWifiSmeConnectCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeConnectCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiSmeConnectCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                                        

             
                                                                    

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiSmeConnectionConfigGetReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeConnectionConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CONNECTION_CONFIG_GET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiSmeConnectionConfigGetReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiSmeConnectionConfigGetReq *msg__; \
        CsrWifiSmeConnectionConfigGetReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeConnectionConfigGetReqSend(src__, interfaceTag__) \
    CsrWifiSmeConnectionConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__)

/*                                                                              

      
                                        

             
                                                     

            
                                             
                                                                              
                                                        
                                                                         

                                                                              */
#define CsrWifiSmeConnectionConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectionConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeConnectionConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CONNECTION_CONFIG_GET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->connectionConfig = (connectionConfig__);

#define CsrWifiSmeConnectionConfigGetCfmSendTo(dst__, src__, interfaceTag__, status__, connectionConfig__) \
    { \
        CsrWifiSmeConnectionConfigGetCfm *msg__; \
        CsrWifiSmeConnectionConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectionConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeConnectionConfigGetCfmSend(dst__, interfaceTag__, status__, connectionConfig__) \
    CsrWifiSmeConnectionConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, connectionConfig__)

/*                                                                              

      
                                      

             
                                                                  

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiSmeConnectionInfoGetReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeConnectionInfoGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CONNECTION_INFO_GET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiSmeConnectionInfoGetReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiSmeConnectionInfoGetReq *msg__; \
        CsrWifiSmeConnectionInfoGetReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeConnectionInfoGetReqSend(src__, interfaceTag__) \
    CsrWifiSmeConnectionInfoGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__)

/*                                                                              

      
                                      

             
                                                     

            
                                           
                                                                            
                                                      
                                                             

                                                                              */
#define CsrWifiSmeConnectionInfoGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectionInfo__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeConnectionInfoGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CONNECTION_INFO_GET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->connectionInfo = (connectionInfo__);

#define CsrWifiSmeConnectionInfoGetCfmSendTo(dst__, src__, interfaceTag__, status__, connectionInfo__) \
    { \
        CsrWifiSmeConnectionInfoGetCfm *msg__; \
        CsrWifiSmeConnectionInfoGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectionInfo__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeConnectionInfoGetCfmSend(dst__, interfaceTag__, status__, connectionInfo__) \
    CsrWifiSmeConnectionInfoGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, connectionInfo__)

/*                                                                              

      
                                      

             
                                                                         
                                                                   
                                                                 
                                                                             
                                                             
                                    
                                                                            
            

            
                                         
                                                                          
                                                    

                                                                              */
#define CsrWifiSmeConnectionQualityIndCreate(msg__, dst__, src__, interfaceTag__, linkQuality__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeConnectionQualityInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CONNECTION_QUALITY_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->linkQuality = (linkQuality__);

#define CsrWifiSmeConnectionQualityIndSendTo(dst__, src__, interfaceTag__, linkQuality__) \
    { \
        CsrWifiSmeConnectionQualityInd *msg__; \
        CsrWifiSmeConnectionQualityIndCreate(msg__, dst__, src__, interfaceTag__, linkQuality__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeConnectionQualityIndSend(dst__, interfaceTag__, linkQuality__) \
    CsrWifiSmeConnectionQualityIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, linkQuality__)

/*                                                                              

      
                                       

             
                                                                   

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiSmeConnectionStatsGetReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeConnectionStatsGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CONNECTION_STATS_GET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiSmeConnectionStatsGetReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiSmeConnectionStatsGetReq *msg__; \
        CsrWifiSmeConnectionStatsGetReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeConnectionStatsGetReqSend(src__, interfaceTag__) \
    CsrWifiSmeConnectionStatsGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__)

/*                                                                              

      
                                       

             
                                                     

            
                                            
                                                                             
                                                       
                                                        

                                                                              */
#define CsrWifiSmeConnectionStatsGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectionStats__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeConnectionStatsGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CONNECTION_STATS_GET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->connectionStats = (connectionStats__);

#define CsrWifiSmeConnectionStatsGetCfmSendTo(dst__, src__, interfaceTag__, status__, connectionStats__) \
    { \
        CsrWifiSmeConnectionStatsGetCfm *msg__; \
        CsrWifiSmeConnectionStatsGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectionStats__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeConnectionStatsGetCfmSend(dst__, interfaceTag__, status__, connectionStats__) \
    CsrWifiSmeConnectionStatsGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, connectionStats__)

/*                                                                              

      
                             

             
                                                                             
                                      
                                                                     
               
                                                                             
                                       

            
                                       
                                                                   
                                                                            
                          

                                                                              */
#define CsrWifiSmeCoreDumpIndCreate(msg__, dst__, src__, dataLength__, data__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeCoreDumpInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_CORE_DUMP_IND, dst__, src__); \
    msg__->dataLength = (dataLength__); \
    msg__->data = (data__);

#define CsrWifiSmeCoreDumpIndSendTo(dst__, src__, dataLength__, data__) \
    { \
        CsrWifiSmeCoreDumpInd *msg__; \
        CsrWifiSmeCoreDumpIndCreate(msg__, dst__, src__, dataLength__, data__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeCoreDumpIndSend(dst__, dataLength__, data__) \
    CsrWifiSmeCoreDumpIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, dataLength__, data__)

/*                                                                              

      
                               

             
                                                       

            
                                                                         

                                                                              */
#define CsrWifiSmeDeactivateReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeDeactivateReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_DEACTIVATE_REQ, dst__, src__);

#define CsrWifiSmeDeactivateReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeDeactivateReq *msg__; \
        CsrWifiSmeDeactivateReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeDeactivateReqSend(src__) \
    CsrWifiSmeDeactivateReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                               

             
                                                                   
                                                                          
                                              

            
                                   
                                              

                                                                              */
#define CsrWifiSmeDeactivateCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeDeactivateCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_DEACTIVATE_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeDeactivateCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeDeactivateCfm *msg__; \
        CsrWifiSmeDeactivateCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeDeactivateCfmSend(dst__, status__) \
    CsrWifiSmeDeactivateCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                               

             
                                                                            
                             

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiSmeDisconnectReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeDisconnectReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_DISCONNECT_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiSmeDisconnectReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiSmeDisconnectReq *msg__; \
        CsrWifiSmeDisconnectReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeDisconnectReqSend(src__, interfaceTag__) \
    CsrWifiSmeDisconnectReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__)

/*                                                                              

      
                               

             
                                                                      
                                                                 
                                                                            
                              

            
                                         
                                                                          
                                                    

                                                                              */
#define CsrWifiSmeDisconnectCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeDisconnectCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_DISCONNECT_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiSmeDisconnectCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiSmeDisconnectCfm *msg__; \
        CsrWifiSmeDisconnectCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeDisconnectCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiSmeDisconnectCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                          

             
                                                                 

            
                                         
                                              

                                                                              */
#define CsrWifiSmeErrorIndCreate(msg__, dst__, src__, errorMessage__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeErrorInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ERROR_IND, dst__, src__); \
    msg__->errorMessage = (errorMessage__);

#define CsrWifiSmeErrorIndSendTo(dst__, src__, errorMessage__) \
    { \
        CsrWifiSmeErrorInd *msg__; \
        CsrWifiSmeErrorIndCreate(msg__, dst__, src__, errorMessage__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeErrorIndSend(dst__, errorMessage__) \
    CsrWifiSmeErrorIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, errorMessage__)

/*                                                                              

      
                                 

             
                                                                         
                                                                            
                                                                       
                                                                         

            
                                                                          
                                                             

                                                                              */
#define CsrWifiSmeEventMaskSetReqCreate(msg__, dst__, src__, indMask__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeEventMaskSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_EVENT_MASK_SET_REQ, dst__, src__); \
    msg__->indMask = (indMask__);

#define CsrWifiSmeEventMaskSetReqSendTo(dst__, src__, indMask__) \
    { \
        CsrWifiSmeEventMaskSetReq *msg__; \
        CsrWifiSmeEventMaskSetReqCreate(msg__, dst__, src__, indMask__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeEventMaskSetReqSend(src__, indMask__) \
    CsrWifiSmeEventMaskSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, indMask__)

/*                                                                              

      
                                 

             
                                                                   
              

            
                                   
                                              

                                                                              */
#define CsrWifiSmeEventMaskSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeEventMaskSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_EVENT_MASK_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeEventMaskSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeEventMaskSetCfm *msg__; \
        CsrWifiSmeEventMaskSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeEventMaskSetCfmSend(dst__, status__) \
    CsrWifiSmeEventMaskSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                  

             
                                                              

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiSmeHostConfigGetReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeHostConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_HOST_CONFIG_GET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiSmeHostConfigGetReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiSmeHostConfigGetReq *msg__; \
        CsrWifiSmeHostConfigGetReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeHostConfigGetReqSend(src__, interfaceTag__) \
    CsrWifiSmeHostConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__)

/*                                                                              

      
                                  

             
                                                     

            
                                         
                                                                          
                                                    
                                            

                                                                              */
#define CsrWifiSmeHostConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, hostConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeHostConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_HOST_CONFIG_GET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->hostConfig = (hostConfig__);

#define CsrWifiSmeHostConfigGetCfmSendTo(dst__, src__, interfaceTag__, status__, hostConfig__) \
    { \
        CsrWifiSmeHostConfigGetCfm *msg__; \
        CsrWifiSmeHostConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, hostConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeHostConfigGetCfmSend(dst__, interfaceTag__, status__, hostConfig__) \
    CsrWifiSmeHostConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, hostConfig__)

/*                                                                              

      
                                  

             
                                                              

            
                                                                               
                                                                          
                                                                             
                                                                               
                               

                                                                              */
#define CsrWifiSmeHostConfigSetReqCreate(msg__, dst__, src__, interfaceTag__, hostConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeHostConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_HOST_CONFIG_SET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->hostConfig = (hostConfig__);

#define CsrWifiSmeHostConfigSetReqSendTo(dst__, src__, interfaceTag__, hostConfig__) \
    { \
        CsrWifiSmeHostConfigSetReq *msg__; \
        CsrWifiSmeHostConfigSetReqCreate(msg__, dst__, src__, interfaceTag__, hostConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeHostConfigSetReqSend(src__, interfaceTag__, hostConfig__) \
    CsrWifiSmeHostConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, hostConfig__)

/*                                                                              

      
                                  

             
                                                     

            
                                         
                                                                          
                                                    

                                                                              */
#define CsrWifiSmeHostConfigSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeHostConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_HOST_CONFIG_SET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiSmeHostConfigSetCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiSmeHostConfigSetCfm *msg__; \
        CsrWifiSmeHostConfigSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeHostConfigSetCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiSmeHostConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                                

             
                                                                             
                            

            
                                        
                                                                    
                                                                       

                                                                              */
#define CsrWifiSmeIbssStationIndCreate(msg__, dst__, src__, address__, isconnected__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeIbssStationInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_IBSS_STATION_IND, dst__, src__); \
    msg__->address = (address__); \
    msg__->isconnected = (isconnected__);

#define CsrWifiSmeIbssStationIndSendTo(dst__, src__, address__, isconnected__) \
    { \
        CsrWifiSmeIbssStationInd *msg__; \
        CsrWifiSmeIbssStationIndCreate(msg__, dst__, src__, address__, isconnected__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeIbssStationIndSend(dst__, address__, isconnected__) \
    CsrWifiSmeIbssStationIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, address__, isconnected__)

/*                                                                              

      
                         

             
                                                                             
                                              

            
                                        
                                       

                                                                              */
#define CsrWifiSmeInfoIndCreate(msg__, dst__, src__, infoMessage__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeInfoInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_INFO_IND, dst__, src__); \
    msg__->infoMessage = (infoMessage__);

#define CsrWifiSmeInfoIndSendTo(dst__, src__, infoMessage__) \
    { \
        CsrWifiSmeInfoInd *msg__; \
        CsrWifiSmeInfoIndCreate(msg__, dst__, src__, infoMessage__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeInfoIndSend(dst__, infoMessage__) \
    CsrWifiSmeInfoIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, infoMessage__)

/*                                                                              

      
                                           

             
                                                                    
                                            

            
                                                                         

                                                                              */
#define CsrWifiSmeInterfaceCapabilityGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeInterfaceCapabilityGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_INTERFACE_CAPABILITY_GET_REQ, dst__, src__);

#define CsrWifiSmeInterfaceCapabilityGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeInterfaceCapabilityGetReq *msg__; \
        CsrWifiSmeInterfaceCapabilityGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeInterfaceCapabilityGetReqSend(src__) \
    CsrWifiSmeInterfaceCapabilityGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                           

             
                                                     

            
                                          
                                         
                                                      
                                                                                
                              
                                                                  
                                         
                              
                              
                             
                              

                                                                              */
#define CsrWifiSmeInterfaceCapabilityGetCfmCreate(msg__, dst__, src__, status__, numInterfaces__, capBitmap__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeInterfaceCapabilityGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_INTERFACE_CAPABILITY_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->numInterfaces = (numInterfaces__); \
    memcpy(msg__->capBitmap, (capBitmap__), sizeof(u8) * 2);

#define CsrWifiSmeInterfaceCapabilityGetCfmSendTo(dst__, src__, status__, numInterfaces__, capBitmap__) \
    { \
        CsrWifiSmeInterfaceCapabilityGetCfm *msg__; \
        CsrWifiSmeInterfaceCapabilityGetCfmCreate(msg__, dst__, src__, status__, numInterfaces__, capBitmap__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeInterfaceCapabilityGetCfmSend(dst__, status__, numInterfaces__, capBitmap__) \
    CsrWifiSmeInterfaceCapabilityGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, numInterfaces__, capBitmap__)

/*                                                                              

      
                        

             
                                                                          
                                                         
                                                                           
                                           

            
                                                                               
                                                                          
                                                                                
                                                                
                                                                      
                                             

                                                                              */
#define CsrWifiSmeKeyReqCreate(msg__, dst__, src__, interfaceTag__, action__, key__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeKeyReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_KEY_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->action = (action__); \
    msg__->key = (key__);

#define CsrWifiSmeKeyReqSendTo(dst__, src__, interfaceTag__, action__, key__) \
    { \
        CsrWifiSmeKeyReq *msg__; \
        CsrWifiSmeKeyReqCreate(msg__, dst__, src__, interfaceTag__, action__, key__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeKeyReqSend(src__, interfaceTag__, action__, key__) \
    CsrWifiSmeKeyReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, action__, key__)

/*                                                                              

      
                        

             
                                                                   
              

            
                                           
                                                                            
                                                      
                                          
                                                  
                                                              

                                                                              */
#define CsrWifiSmeKeyCfmCreate(msg__, dst__, src__, interfaceTag__, status__, action__, keyType__, peerMacAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeKeyCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_KEY_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->action = (action__); \
    msg__->keyType = (keyType__); \
    msg__->peerMacAddress = (peerMacAddress__);

#define CsrWifiSmeKeyCfmSendTo(dst__, src__, interfaceTag__, status__, action__, keyType__, peerMacAddress__) \
    { \
        CsrWifiSmeKeyCfm *msg__; \
        CsrWifiSmeKeyCfmCreate(msg__, dst__, src__, interfaceTag__, status__, action__, keyType__, peerMacAddress__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeKeyCfmSend(dst__, interfaceTag__, status__, action__, keyType__, peerMacAddress__) \
    CsrWifiSmeKeyCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, action__, keyType__, peerMacAddress__)

/*                                                                              

      
                                   

             
                                                               

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiSmeLinkQualityGetReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeLinkQualityGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_LINK_QUALITY_GET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiSmeLinkQualityGetReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiSmeLinkQualityGetReq *msg__; \
        CsrWifiSmeLinkQualityGetReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeLinkQualityGetReqSend(src__, interfaceTag__) \
    CsrWifiSmeLinkQualityGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__)

/*                                                                              

      
                                   

             
                                                     

            
                                         
                                                                          
                                                    
                                                    

                                                                              */
#define CsrWifiSmeLinkQualityGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, linkQuality__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeLinkQualityGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_LINK_QUALITY_GET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->linkQuality = (linkQuality__);

#define CsrWifiSmeLinkQualityGetCfmSendTo(dst__, src__, interfaceTag__, status__, linkQuality__) \
    { \
        CsrWifiSmeLinkQualityGetCfm *msg__; \
        CsrWifiSmeLinkQualityGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, linkQuality__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeLinkQualityGetCfmSend(dst__, interfaceTag__, status__, linkQuality__) \
    CsrWifiSmeLinkQualityGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, linkQuality__)

/*                                                                              

      
                                

             
                                                                         
                                                                             
               

            
                                           
                                                                            
                                               
                                                                          
                                                         
                                                                                
                                                                          
                                                            
                                                                                
                                   
                                                                          
                                                            
                                                                              
                                         

                                                                              */
#define CsrWifiSmeMediaStatusIndCreate(msg__, dst__, src__, interfaceTag__, mediaStatus__, connectionInfo__, disassocReason__, deauthReason__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMediaStatusInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MEDIA_STATUS_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->mediaStatus = (mediaStatus__); \
    msg__->connectionInfo = (connectionInfo__); \
    msg__->disassocReason = (disassocReason__); \
    msg__->deauthReason = (deauthReason__);

#define CsrWifiSmeMediaStatusIndSendTo(dst__, src__, interfaceTag__, mediaStatus__, connectionInfo__, disassocReason__, deauthReason__) \
    { \
        CsrWifiSmeMediaStatusInd *msg__; \
        CsrWifiSmeMediaStatusIndCreate(msg__, dst__, src__, interfaceTag__, mediaStatus__, connectionInfo__, disassocReason__, deauthReason__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMediaStatusIndSend(dst__, interfaceTag__, mediaStatus__, connectionInfo__, disassocReason__, deauthReason__) \
    CsrWifiSmeMediaStatusIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, mediaStatus__, connectionInfo__, disassocReason__, deauthReason__)

/*                                                                              

      
                                 

             
                                                             

            
                                                                         

                                                                              */
#define CsrWifiSmeMibConfigGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_CONFIG_GET_REQ, dst__, src__);

#define CsrWifiSmeMibConfigGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeMibConfigGetReq *msg__; \
        CsrWifiSmeMibConfigGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibConfigGetReqSend(src__) \
    CsrWifiSmeMibConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                 

             
                                                     

            
                                      
                                                 
                                                                              

                                                                              */
#define CsrWifiSmeMibConfigGetCfmCreate(msg__, dst__, src__, status__, mibConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_CONFIG_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->mibConfig = (mibConfig__);

#define CsrWifiSmeMibConfigGetCfmSendTo(dst__, src__, status__, mibConfig__) \
    { \
        CsrWifiSmeMibConfigGetCfm *msg__; \
        CsrWifiSmeMibConfigGetCfmCreate(msg__, dst__, src__, status__, mibConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibConfigGetCfmSend(dst__, status__, mibConfig__) \
    CsrWifiSmeMibConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, mibConfig__)

/*                                                                              

      
                                 

             
                                                             

            
                                                                            
                                                                              
                                    

                                                                              */
#define CsrWifiSmeMibConfigSetReqCreate(msg__, dst__, src__, mibConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_CONFIG_SET_REQ, dst__, src__); \
    msg__->mibConfig = (mibConfig__);

#define CsrWifiSmeMibConfigSetReqSendTo(dst__, src__, mibConfig__) \
    { \
        CsrWifiSmeMibConfigSetReq *msg__; \
        CsrWifiSmeMibConfigSetReqCreate(msg__, dst__, src__, mibConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibConfigSetReqSend(src__, mibConfig__) \
    CsrWifiSmeMibConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, mibConfig__)

/*                                                                              

      
                                 

             
                                                     

            
                                   
                                              

                                                                              */
#define CsrWifiSmeMibConfigSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_CONFIG_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeMibConfigSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeMibConfigSetCfm *msg__; \
        CsrWifiSmeMibConfigSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibConfigSetCfmSend(dst__, status__) \
    CsrWifiSmeMibConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                           

             
                                                                             

            
                                               
                                                          
                                               
                                                                            
                                                                        

                                                                              */
#define CsrWifiSmeMibGetCfmCreate(msg__, dst__, src__, status__, mibAttributeLength__, mibAttribute__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->mibAttributeLength = (mibAttributeLength__); \
    msg__->mibAttribute = (mibAttribute__);

#define CsrWifiSmeMibGetCfmSendTo(dst__, src__, status__, mibAttributeLength__, mibAttribute__) \
    { \
        CsrWifiSmeMibGetCfm *msg__; \
        CsrWifiSmeMibGetCfmCreate(msg__, dst__, src__, status__, mibAttributeLength__, mibAttribute__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibGetCfmSend(dst__, status__, mibAttributeLength__, mibAttribute__) \
    CsrWifiSmeMibGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, mibAttributeLength__, mibAttribute__)

/*                                                                              

      
                               

             
                                                                        
                                                       

            
                                                                                     
                                               
                                                                          
                                                                       

                                                                              */
#define CsrWifiSmeMibGetNextReqCreate(msg__, dst__, src__, mibAttributeLength__, mibAttribute__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibGetNextReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_GET_NEXT_REQ, dst__, src__); \
    msg__->mibAttributeLength = (mibAttributeLength__); \
    msg__->mibAttribute = (mibAttribute__);

#define CsrWifiSmeMibGetNextReqSendTo(dst__, src__, mibAttributeLength__, mibAttribute__) \
    { \
        CsrWifiSmeMibGetNextReq *msg__; \
        CsrWifiSmeMibGetNextReqCreate(msg__, dst__, src__, mibAttributeLength__, mibAttribute__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibGetNextReqSend(src__, mibAttributeLength__, mibAttribute__) \
    CsrWifiSmeMibGetNextReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, mibAttributeLength__, mibAttribute__)

/*                                                                              

      
                               

             
                                                                     
                                                                       
                                                                      

            
                                               
                                                          
                                               
                                                                          
                                                                         
                                                                   

                                                                              */
#define CsrWifiSmeMibGetNextCfmCreate(msg__, dst__, src__, status__, mibAttributeLength__, mibAttribute__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibGetNextCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_GET_NEXT_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->mibAttributeLength = (mibAttributeLength__); \
    msg__->mibAttribute = (mibAttribute__);

#define CsrWifiSmeMibGetNextCfmSendTo(dst__, src__, status__, mibAttributeLength__, mibAttribute__) \
    { \
        CsrWifiSmeMibGetNextCfm *msg__; \
        CsrWifiSmeMibGetNextCfmCreate(msg__, dst__, src__, status__, mibAttributeLength__, mibAttribute__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibGetNextCfmSend(dst__, status__, mibAttributeLength__, mibAttribute__) \
    CsrWifiSmeMibGetNextCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, mibAttributeLength__, mibAttribute__)

/*                                                                              

      
                           

             
                                                                            
                       

            
                                                                                     
                                               
                                                                            
                                                                   

                                                                              */
#define CsrWifiSmeMibGetReqCreate(msg__, dst__, src__, mibAttributeLength__, mibAttribute__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_GET_REQ, dst__, src__); \
    msg__->mibAttributeLength = (mibAttributeLength__); \
    msg__->mibAttribute = (mibAttribute__);

#define CsrWifiSmeMibGetReqSendTo(dst__, src__, mibAttributeLength__, mibAttribute__) \
    { \
        CsrWifiSmeMibGetReq *msg__; \
        CsrWifiSmeMibGetReqCreate(msg__, dst__, src__, mibAttributeLength__, mibAttribute__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibGetReqSend(src__, mibAttributeLength__, mibAttribute__) \
    CsrWifiSmeMibGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, mibAttributeLength__, mibAttribute__)

/*                                                                              

      
                           

             
                                                                            
                                                                             
                                     
                                                                   
                                                                    
                                     
                                                                           
                          
                                                                            
                 

            
                                                                                     
                                               
                                                                            
                                                                     

                                                                              */
#define CsrWifiSmeMibSetReqCreate(msg__, dst__, src__, mibAttributeLength__, mibAttribute__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_SET_REQ, dst__, src__); \
    msg__->mibAttributeLength = (mibAttributeLength__); \
    msg__->mibAttribute = (mibAttribute__);

#define CsrWifiSmeMibSetReqSendTo(dst__, src__, mibAttributeLength__, mibAttribute__) \
    { \
        CsrWifiSmeMibSetReq *msg__; \
        CsrWifiSmeMibSetReqCreate(msg__, dst__, src__, mibAttributeLength__, mibAttribute__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibSetReqSend(src__, mibAttributeLength__, mibAttribute__) \
    CsrWifiSmeMibSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, mibAttributeLength__, mibAttribute__)

/*                                                                              

      
                           

             
                                                                          

            
                                   
                                              

                                                                              */
#define CsrWifiSmeMibSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMibSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIB_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeMibSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeMibSetCfm *msg__; \
        CsrWifiSmeMibSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMibSetCfmSend(dst__, status__) \
    CsrWifiSmeMibSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                               

             
                                                                         
                                                                

            
                                          
                                                                           
                                                                         
                           
                                                                             
                               
                                                                              
                                                              

                                                                              */
#define CsrWifiSmeMicFailureIndCreate(msg__, dst__, src__, interfaceTag__, secondFailure__, count__, address__, keyType__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMicFailureInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MIC_FAILURE_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->secondFailure = (secondFailure__); \
    msg__->count = (count__); \
    msg__->address = (address__); \
    msg__->keyType = (keyType__);

#define CsrWifiSmeMicFailureIndSendTo(dst__, src__, interfaceTag__, secondFailure__, count__, address__, keyType__) \
    { \
        CsrWifiSmeMicFailureInd *msg__; \
        CsrWifiSmeMicFailureIndCreate(msg__, dst__, src__, interfaceTag__, secondFailure__, count__, address__, keyType__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMicFailureIndSend(dst__, interfaceTag__, secondFailure__, count__, address__, keyType__) \
    CsrWifiSmeMicFailureIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, secondFailure__, count__, address__, keyType__)

/*                                                                              

      
                                     

             
                                                                        
                                                                             
                                                                        
                                                                            
           

            
                                                                                    
                                                                               
                                                                       
                                                                             
                                      
                                                                       
                                                                          
                                                               

                                                                              */
#define CsrWifiSmeMulticastAddressReqCreate(msg__, dst__, src__, interfaceTag__, action__, setAddressesCount__, setAddresses__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMulticastAddressReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MULTICAST_ADDRESS_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->action = (action__); \
    msg__->setAddressesCount = (setAddressesCount__); \
    msg__->setAddresses = (setAddresses__);

#define CsrWifiSmeMulticastAddressReqSendTo(dst__, src__, interfaceTag__, action__, setAddressesCount__, setAddresses__) \
    { \
        CsrWifiSmeMulticastAddressReq *msg__; \
        CsrWifiSmeMulticastAddressReqCreate(msg__, dst__, src__, interfaceTag__, action__, setAddressesCount__, setAddresses__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMulticastAddressReqSend(src__, interfaceTag__, action__, setAddressesCount__, setAddresses__) \
    CsrWifiSmeMulticastAddressReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, action__, setAddressesCount__, setAddresses__)

/*                                                                              

      
                                     

             
                                                                          
                                                                         

            
                                              
                                                                               
                                                         
                                             
                                                                    
                                                     
                                                                       
                                                                          
                                                               

                                                                              */
#define CsrWifiSmeMulticastAddressCfmCreate(msg__, dst__, src__, interfaceTag__, status__, action__, getAddressesCount__, getAddresses__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeMulticastAddressCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_MULTICAST_ADDRESS_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->action = (action__); \
    msg__->getAddressesCount = (getAddressesCount__); \
    msg__->getAddresses = (getAddresses__);

#define CsrWifiSmeMulticastAddressCfmSendTo(dst__, src__, interfaceTag__, status__, action__, getAddressesCount__, getAddresses__) \
    { \
        CsrWifiSmeMulticastAddressCfm *msg__; \
        CsrWifiSmeMulticastAddressCfmCreate(msg__, dst__, src__, interfaceTag__, status__, action__, getAddressesCount__, getAddresses__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeMulticastAddressCfmSend(dst__, interfaceTag__, status__, action__, getAddressesCount__, getAddresses__) \
    CsrWifiSmeMulticastAddressCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, action__, getAddressesCount__, getAddresses__)

/*                                                                              

      
                                    

             
                                                                            
                                                                           
                                                                        
         
                                                                            
                                                       
                                                                             
                                                               
                     
                                                                         
                                              
                                                                 

            
                                                                                   
                                                                              
                                                     
                                                                        
                                                                               
                                                                      
                                                                 
                                                                  
                                                
                                                                               
                                          
                                                                             
                                                                                 
                                                                                 
                                
                                                                             
                                                                                  
                                                                                    
                                      

                                                                              */
#define CsrWifiSmePacketFilterSetReqCreate(msg__, dst__, src__, interfaceTag__, filterLength__, filter__, mode__, arpFilterAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePacketFilterSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_PACKET_FILTER_SET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->filterLength = (filterLength__); \
    msg__->filter = (filter__); \
    msg__->mode = (mode__); \
    msg__->arpFilterAddress = (arpFilterAddress__);

#define CsrWifiSmePacketFilterSetReqSendTo(dst__, src__, interfaceTag__, filterLength__, filter__, mode__, arpFilterAddress__) \
    { \
        CsrWifiSmePacketFilterSetReq *msg__; \
        CsrWifiSmePacketFilterSetReqCreate(msg__, dst__, src__, interfaceTag__, filterLength__, filter__, mode__, arpFilterAddress__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePacketFilterSetReqSend(src__, interfaceTag__, filterLength__, filter__, mode__, arpFilterAddress__) \
    CsrWifiSmePacketFilterSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, filterLength__, filter__, mode__, arpFilterAddress__)

/*                                                                              

      
                                    

             
                                                                          

            
                                         
                                                                          
                                                    

                                                                              */
#define CsrWifiSmePacketFilterSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePacketFilterSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_PACKET_FILTER_SET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiSmePacketFilterSetCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiSmePacketFilterSetCfm *msg__; \
        CsrWifiSmePacketFilterSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePacketFilterSetCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiSmePacketFilterSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                                           

             
                                                             

            
                                                                         

                                                                              */
#define CsrWifiSmePermanentMacAddressGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePermanentMacAddressGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_PERMANENT_MAC_ADDRESS_GET_REQ, dst__, src__);

#define CsrWifiSmePermanentMacAddressGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmePermanentMacAddressGetReq *msg__; \
        CsrWifiSmePermanentMacAddressGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePermanentMacAddressGetReqSend(src__) \
    CsrWifiSmePermanentMacAddressGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                           

             
                                                     

            
                                                
                                                           
                                                          

                                                                              */
#define CsrWifiSmePermanentMacAddressGetCfmCreate(msg__, dst__, src__, status__, permanentMacAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePermanentMacAddressGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_PERMANENT_MAC_ADDRESS_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->permanentMacAddress = (permanentMacAddress__);

#define CsrWifiSmePermanentMacAddressGetCfmSendTo(dst__, src__, status__, permanentMacAddress__) \
    { \
        CsrWifiSmePermanentMacAddressGetCfm *msg__; \
        CsrWifiSmePermanentMacAddressGetCfmCreate(msg__, dst__, src__, status__, permanentMacAddress__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePermanentMacAddressGetCfmSend(dst__, status__, permanentMacAddress__) \
    CsrWifiSmePermanentMacAddressGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, permanentMacAddress__)

/*                                                                              

      
                                       

             
                                                                             
                                                                         
                    

            
                                                 
                                                                        
                                    
                                                              
                                                              

                                                                              */
#define CsrWifiSmePmkidCandidateListIndCreate(msg__, dst__, src__, interfaceTag__, pmkidCandidatesCount__, pmkidCandidates__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePmkidCandidateListInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_PMKID_CANDIDATE_LIST_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->pmkidCandidatesCount = (pmkidCandidatesCount__); \
    msg__->pmkidCandidates = (pmkidCandidates__);

#define CsrWifiSmePmkidCandidateListIndSendTo(dst__, src__, interfaceTag__, pmkidCandidatesCount__, pmkidCandidates__) \
    { \
        CsrWifiSmePmkidCandidateListInd *msg__; \
        CsrWifiSmePmkidCandidateListIndCreate(msg__, dst__, src__, interfaceTag__, pmkidCandidatesCount__, pmkidCandidates__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePmkidCandidateListIndSend(dst__, interfaceTag__, pmkidCandidatesCount__, pmkidCandidates__) \
    CsrWifiSmePmkidCandidateListIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, pmkidCandidatesCount__, pmkidCandidates__)

/*                                                                              

      
                          

             
                                                                       
                                    
                                                           
                                                                         
                                                               
                                  
                                                                           
                              

            
                                                                                 
                                                                            
                                                                              
                                                                        
                                                             
                                                                               
                                             

                                                                              */
#define CsrWifiSmePmkidReqCreate(msg__, dst__, src__, interfaceTag__, action__, setPmkidsCount__, setPmkids__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePmkidReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_PMKID_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->action = (action__); \
    msg__->setPmkidsCount = (setPmkidsCount__); \
    msg__->setPmkids = (setPmkids__);

#define CsrWifiSmePmkidReqSendTo(dst__, src__, interfaceTag__, action__, setPmkidsCount__, setPmkids__) \
    { \
        CsrWifiSmePmkidReq *msg__; \
        CsrWifiSmePmkidReqCreate(msg__, dst__, src__, interfaceTag__, action__, setPmkidsCount__, setPmkids__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePmkidReqSend(src__, interfaceTag__, action__, setPmkidsCount__, setPmkids__) \
    CsrWifiSmePmkidReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, action__, setPmkidsCount__, setPmkids__)

/*                                                                              

      
                          

             
                                                                          
                                                                 

            
                                           
                                                                            
                                                      
                                          
                                                                 
                                                  
                                                             
                                                                               
                                             

                                                                              */
#define CsrWifiSmePmkidCfmCreate(msg__, dst__, src__, interfaceTag__, status__, action__, getPmkidsCount__, getPmkids__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePmkidCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_PMKID_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->action = (action__); \
    msg__->getPmkidsCount = (getPmkidsCount__); \
    msg__->getPmkids = (getPmkids__);

#define CsrWifiSmePmkidCfmSendTo(dst__, src__, interfaceTag__, status__, action__, getPmkidsCount__, getPmkids__) \
    { \
        CsrWifiSmePmkidCfm *msg__; \
        CsrWifiSmePmkidCfmCreate(msg__, dst__, src__, interfaceTag__, status__, action__, getPmkidsCount__, getPmkids__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePmkidCfmSend(dst__, interfaceTag__, status__, action__, getPmkidsCount__, getPmkids__) \
    CsrWifiSmePmkidCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, action__, getPmkidsCount__, getPmkids__)

/*                                                                              

      
                                   

             
                                                               

            
                                                                         

                                                                              */
#define CsrWifiSmePowerConfigGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePowerConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_POWER_CONFIG_GET_REQ, dst__, src__);

#define CsrWifiSmePowerConfigGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmePowerConfigGetReq *msg__; \
        CsrWifiSmePowerConfigGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePowerConfigGetReqSend(src__) \
    CsrWifiSmePowerConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                   

             
                                                     

            
                                        
                                                   
                                                                               
                              

                                                                              */
#define CsrWifiSmePowerConfigGetCfmCreate(msg__, dst__, src__, status__, powerConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePowerConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_POWER_CONFIG_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->powerConfig = (powerConfig__);

#define CsrWifiSmePowerConfigGetCfmSendTo(dst__, src__, status__, powerConfig__) \
    { \
        CsrWifiSmePowerConfigGetCfm *msg__; \
        CsrWifiSmePowerConfigGetCfmCreate(msg__, dst__, src__, status__, powerConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePowerConfigGetCfmSend(dst__, status__, powerConfig__) \
    CsrWifiSmePowerConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, powerConfig__)

/*                                                                              

      
                                   

             
                                                               

            
                                                                              
                                            

                                                                              */
#define CsrWifiSmePowerConfigSetReqCreate(msg__, dst__, src__, powerConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePowerConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_POWER_CONFIG_SET_REQ, dst__, src__); \
    msg__->powerConfig = (powerConfig__);

#define CsrWifiSmePowerConfigSetReqSendTo(dst__, src__, powerConfig__) \
    { \
        CsrWifiSmePowerConfigSetReq *msg__; \
        CsrWifiSmePowerConfigSetReqCreate(msg__, dst__, src__, powerConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePowerConfigSetReqSend(src__, powerConfig__) \
    CsrWifiSmePowerConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, powerConfig__)

/*                                                                              

      
                                   

             
                                                     

            
                                   
                                              

                                                                              */
#define CsrWifiSmePowerConfigSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmePowerConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_POWER_CONFIG_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmePowerConfigSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmePowerConfigSetCfm *msg__; \
        CsrWifiSmePowerConfigSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmePowerConfigSetCfmSend(dst__, status__) \
    CsrWifiSmePowerConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                            

             
                                                                        

            
                                                                         

                                                                              */
#define CsrWifiSmeRegulatoryDomainInfoGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeRegulatoryDomainInfoGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_REGULATORY_DOMAIN_INFO_GET_REQ, dst__, src__);

#define CsrWifiSmeRegulatoryDomainInfoGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeRegulatoryDomainInfoGetReq *msg__; \
        CsrWifiSmeRegulatoryDomainInfoGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeRegulatoryDomainInfoGetReqSend(src__) \
    CsrWifiSmeRegulatoryDomainInfoGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                            

             
                                                     

            
                                       
                                                  
                                                                           
                           

                                                                              */
#define CsrWifiSmeRegulatoryDomainInfoGetCfmCreate(msg__, dst__, src__, status__, regDomInfo__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeRegulatoryDomainInfoGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_REGULATORY_DOMAIN_INFO_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->regDomInfo = (regDomInfo__);

#define CsrWifiSmeRegulatoryDomainInfoGetCfmSendTo(dst__, src__, status__, regDomInfo__) \
    { \
        CsrWifiSmeRegulatoryDomainInfoGetCfm *msg__; \
        CsrWifiSmeRegulatoryDomainInfoGetCfmCreate(msg__, dst__, src__, status__, regDomInfo__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeRegulatoryDomainInfoGetCfmSend(dst__, status__, regDomInfo__) \
    CsrWifiSmeRegulatoryDomainInfoGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, regDomInfo__)

/*                                                                              

      
                                 

             
                                                                             
                                                                             
                                                                       
                                                            

            
                                         
                                                                          
                                                              

                                                                              */
#define CsrWifiSmeRoamCompleteIndCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeRoamCompleteInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ROAM_COMPLETE_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiSmeRoamCompleteIndSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiSmeRoamCompleteInd *msg__; \
        CsrWifiSmeRoamCompleteIndCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeRoamCompleteIndSend(dst__, interfaceTag__, status__) \
    CsrWifiSmeRoamCompleteIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                              

             
                                                                             
                                                              
                                                                          
                                                                            
                                                                      
                                                                            
                                                                          
                                             
                                                                      
                                                                        
           
                                                                             
                                                                           
            
                                                                      
                                                          
                                          
                                                                          
                                                                           

            
                                         
                                                                          
                                                                          
                                                                              

                                                                              */
#define CsrWifiSmeRoamStartIndCreate(msg__, dst__, src__, interfaceTag__, roamReason__, reason80211__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeRoamStartInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ROAM_START_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->roamReason = (roamReason__); \
    msg__->reason80211 = (reason80211__);

#define CsrWifiSmeRoamStartIndSendTo(dst__, src__, interfaceTag__, roamReason__, reason80211__) \
    { \
        CsrWifiSmeRoamStartInd *msg__; \
        CsrWifiSmeRoamStartIndCreate(msg__, dst__, src__, interfaceTag__, roamReason__, reason80211__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeRoamStartIndSend(dst__, interfaceTag__, roamReason__, reason80211__) \
    CsrWifiSmeRoamStartIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, roamReason__, reason80211__)

/*                                                                              

      
                                     

             
                                                                 

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiSmeRoamingConfigGetReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeRoamingConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ROAMING_CONFIG_GET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiSmeRoamingConfigGetReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiSmeRoamingConfigGetReq *msg__; \
        CsrWifiSmeRoamingConfigGetReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeRoamingConfigGetReqSend(src__, interfaceTag__) \
    CsrWifiSmeRoamingConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__)

/*                                                                              

      
                                     

             
                                                     

            
                                          
                                                                           
                                                     
                                                                            

                                                                              */
#define CsrWifiSmeRoamingConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, roamingConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeRoamingConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ROAMING_CONFIG_GET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->roamingConfig = (roamingConfig__);

#define CsrWifiSmeRoamingConfigGetCfmSendTo(dst__, src__, interfaceTag__, status__, roamingConfig__) \
    { \
        CsrWifiSmeRoamingConfigGetCfm *msg__; \
        CsrWifiSmeRoamingConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, roamingConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeRoamingConfigGetCfmSend(dst__, interfaceTag__, status__, roamingConfig__) \
    CsrWifiSmeRoamingConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, roamingConfig__)

/*                                                                              

      
                                     

             
                                                                 

            
                                                                                
                                                                           
                                                    

                                                                              */
#define CsrWifiSmeRoamingConfigSetReqCreate(msg__, dst__, src__, interfaceTag__, roamingConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeRoamingConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ROAMING_CONFIG_SET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->roamingConfig = (roamingConfig__);

#define CsrWifiSmeRoamingConfigSetReqSendTo(dst__, src__, interfaceTag__, roamingConfig__) \
    { \
        CsrWifiSmeRoamingConfigSetReq *msg__; \
        CsrWifiSmeRoamingConfigSetReqCreate(msg__, dst__, src__, interfaceTag__, roamingConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeRoamingConfigSetReqSend(src__, interfaceTag__, roamingConfig__) \
    CsrWifiSmeRoamingConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, roamingConfig__)

/*                                                                              

      
                                     

             
                                                                 

            
                                         
                                                                          
                                                    

                                                                              */
#define CsrWifiSmeRoamingConfigSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeRoamingConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_ROAMING_CONFIG_SET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiSmeRoamingConfigSetCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiSmeRoamingConfigSetCfm *msg__; \
        CsrWifiSmeRoamingConfigSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeRoamingConfigSetCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiSmeRoamingConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                                  

             
                                                              

            
                                                                         

                                                                              */
#define CsrWifiSmeScanConfigGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_CONFIG_GET_REQ, dst__, src__);

#define CsrWifiSmeScanConfigGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeScanConfigGetReq *msg__; \
        CsrWifiSmeScanConfigGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanConfigGetReqSend(src__) \
    CsrWifiSmeScanConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                  

             
                                                     

            
                                       
                                                  
                                                                           
                                          

                                                                              */
#define CsrWifiSmeScanConfigGetCfmCreate(msg__, dst__, src__, status__, scanConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_CONFIG_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->scanConfig = (scanConfig__);

#define CsrWifiSmeScanConfigGetCfmSendTo(dst__, src__, status__, scanConfig__) \
    { \
        CsrWifiSmeScanConfigGetCfm *msg__; \
        CsrWifiSmeScanConfigGetCfmCreate(msg__, dst__, src__, status__, scanConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanConfigGetCfmSend(dst__, status__, scanConfig__) \
    CsrWifiSmeScanConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, scanConfig__)

/*                                                                              

      
                                  

             
                                                              
                                                                           
                               
                                                                           
                                             
                                           
                                  
                                                                              
                                                                
                                                                            
                                                                

            
                                                                             
                                                                                
                                

                                                                              */
#define CsrWifiSmeScanConfigSetReqCreate(msg__, dst__, src__, scanConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_CONFIG_SET_REQ, dst__, src__); \
    msg__->scanConfig = (scanConfig__);

#define CsrWifiSmeScanConfigSetReqSendTo(dst__, src__, scanConfig__) \
    { \
        CsrWifiSmeScanConfigSetReq *msg__; \
        CsrWifiSmeScanConfigSetReqCreate(msg__, dst__, src__, scanConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanConfigSetReqSend(src__, scanConfig__) \
    CsrWifiSmeScanConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, scanConfig__)

/*                                                                              

      
                                  

             
                                                     

            
                                   
                                              

                                                                              */
#define CsrWifiSmeScanConfigSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_CONFIG_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeScanConfigSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeScanConfigSetCfm *msg__; \
        CsrWifiSmeScanConfigSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanConfigSetCfmSend(dst__, status__) \
    CsrWifiSmeScanConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                             

             
                                                                            
              
                                                                           
                             
                                                                             
                                                                            
              

            
                                                                                   
                                                
                                                                             
                                                                 
                                      
                                                                                
                                             
                                                                             
                                                                            
                                                                                
                                                                             
                                              
                                              
                                                   
                                                                          
                                                                           
                                         
                                                                                
                               
                                                                            
                                              
                                                                                
                                                   

                                                                              */
#define CsrWifiSmeScanFullReqCreate(msg__, dst__, src__, ssidCount__, ssid__, bssid__, forceScan__, bssType__, scanType__, channelListCount__, channelList__, probeIeLength__, probeIe__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanFullReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_FULL_REQ, dst__, src__); \
    msg__->ssidCount = (ssidCount__); \
    msg__->ssid = (ssid__); \
    msg__->bssid = (bssid__); \
    msg__->forceScan = (forceScan__); \
    msg__->bssType = (bssType__); \
    msg__->scanType = (scanType__); \
    msg__->channelListCount = (channelListCount__); \
    msg__->channelList = (channelList__); \
    msg__->probeIeLength = (probeIeLength__); \
    msg__->probeIe = (probeIe__);

#define CsrWifiSmeScanFullReqSendTo(dst__, src__, ssidCount__, ssid__, bssid__, forceScan__, bssType__, scanType__, channelListCount__, channelList__, probeIeLength__, probeIe__) \
    { \
        CsrWifiSmeScanFullReq *msg__; \
        CsrWifiSmeScanFullReqCreate(msg__, dst__, src__, ssidCount__, ssid__, bssid__, forceScan__, bssType__, scanType__, channelListCount__, channelList__, probeIeLength__, probeIe__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanFullReqSend(src__, ssidCount__, ssid__, bssid__, forceScan__, bssType__, scanType__, channelListCount__, channelList__, probeIeLength__, probeIe__) \
    CsrWifiSmeScanFullReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, ssidCount__, ssid__, bssid__, forceScan__, bssType__, scanType__, channelListCount__, channelList__, probeIeLength__, probeIe__)

/*                                                                              

      
                             

             
                                                                   
              

            
                                   
                                              

                                                                              */
#define CsrWifiSmeScanFullCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanFullCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_FULL_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeScanFullCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeScanFullCfm *msg__; \
        CsrWifiSmeScanFullCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanFullCfmSend(dst__, status__) \
    CsrWifiSmeScanFullCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                               

             
                                                                         
                                                                        

            
                                   
                                                         

                                                                              */
#define CsrWifiSmeScanResultIndCreate(msg__, dst__, src__, result__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanResultInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_RESULT_IND, dst__, src__); \
    msg__->result = (result__);

#define CsrWifiSmeScanResultIndSendTo(dst__, src__, result__) \
    { \
        CsrWifiSmeScanResultInd *msg__; \
        CsrWifiSmeScanResultIndCreate(msg__, dst__, src__, result__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanResultIndSend(dst__, result__) \
    CsrWifiSmeScanResultIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, result__)

/*                                                                              

      
                                     

             
                                                                          
                                                                            
                      
                                                                       
                             
                                                                          
                                
                                                                          
                                                                             
                                                                

            
                                                                         

                                                                              */
#define CsrWifiSmeScanResultsFlushReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanResultsFlushReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_RESULTS_FLUSH_REQ, dst__, src__);

#define CsrWifiSmeScanResultsFlushReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeScanResultsFlushReq *msg__; \
        CsrWifiSmeScanResultsFlushReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanResultsFlushReqSend(src__) \
    CsrWifiSmeScanResultsFlushReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                     

             
                                                                     

            
                                   
                                              

                                                                              */
#define CsrWifiSmeScanResultsFlushCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanResultsFlushCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_RESULTS_FLUSH_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeScanResultsFlushCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeScanResultsFlushCfm *msg__; \
        CsrWifiSmeScanResultsFlushCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanResultsFlushCfmSend(dst__, status__) \
    CsrWifiSmeScanResultsFlushCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                   

             
                                                                         
                                                                    
                                                                  

            
                                                                         

                                                                              */
#define CsrWifiSmeScanResultsGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanResultsGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_RESULTS_GET_REQ, dst__, src__);

#define CsrWifiSmeScanResultsGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeScanResultsGetReq *msg__; \
        CsrWifiSmeScanResultsGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanResultsGetReqSend(src__) \
    CsrWifiSmeScanResultsGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                   

             
                                                                            

            
                                             
                                                        
                                             
                                                                
                                                       

                                                                              */
#define CsrWifiSmeScanResultsGetCfmCreate(msg__, dst__, src__, status__, scanResultsCount__, scanResults__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeScanResultsGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SCAN_RESULTS_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->scanResultsCount = (scanResultsCount__); \
    msg__->scanResults = (scanResults__);

#define CsrWifiSmeScanResultsGetCfmSendTo(dst__, src__, status__, scanResultsCount__, scanResults__) \
    { \
        CsrWifiSmeScanResultsGetCfm *msg__; \
        CsrWifiSmeScanResultsGetCfmCreate(msg__, dst__, src__, status__, scanResultsCount__, scanResults__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeScanResultsGetCfmSend(dst__, status__, scanResultsCount__, scanResults__) \
    CsrWifiSmeScanResultsGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, scanResultsCount__, scanResults__)

/*                                                                              

      
                        

             
                                                                          
                                    
                                                                           
                

            
                                                                             
                                                                   
                                                                    

                                                                              */
#define CsrWifiSmeSetReqCreate(msg__, dst__, src__, dataLength__, data__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SET_REQ, dst__, src__); \
    msg__->dataLength = (dataLength__); \
    msg__->data = (data__);

#define CsrWifiSmeSetReqSendTo(dst__, src__, dataLength__, data__) \
    { \
        CsrWifiSmeSetReq *msg__; \
        CsrWifiSmeSetReqCreate(msg__, dst__, src__, dataLength__, data__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeSetReqSend(src__, dataLength__, data__) \
    CsrWifiSmeSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, dataLength__, data__)

/*                                                                              

      
                                       

             
                                                              

            
                                                                         

                                                                              */
#define CsrWifiSmeSmeCommonConfigGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeSmeCommonConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SME_COMMON_CONFIG_GET_REQ, dst__, src__);

#define CsrWifiSmeSmeCommonConfigGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeSmeCommonConfigGetReq *msg__; \
        CsrWifiSmeSmeCommonConfigGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeSmeCommonConfigGetReqSend(src__) \
    CsrWifiSmeSmeCommonConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                       

             
                                                     

            
                                         
                                                    
                                                   

                                                                              */
#define CsrWifiSmeSmeCommonConfigGetCfmCreate(msg__, dst__, src__, status__, deviceConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeSmeCommonConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SME_COMMON_CONFIG_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->deviceConfig = (deviceConfig__);

#define CsrWifiSmeSmeCommonConfigGetCfmSendTo(dst__, src__, status__, deviceConfig__) \
    { \
        CsrWifiSmeSmeCommonConfigGetCfm *msg__; \
        CsrWifiSmeSmeCommonConfigGetCfmCreate(msg__, dst__, src__, status__, deviceConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeSmeCommonConfigGetCfmSend(dst__, status__, deviceConfig__) \
    CsrWifiSmeSmeCommonConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, deviceConfig__)

/*                                                                              

      
                                       

             
                                                    

            
                                                                               
                                                   

                                                                              */
#define CsrWifiSmeSmeCommonConfigSetReqCreate(msg__, dst__, src__, deviceConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeSmeCommonConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SME_COMMON_CONFIG_SET_REQ, dst__, src__); \
    msg__->deviceConfig = (deviceConfig__);

#define CsrWifiSmeSmeCommonConfigSetReqSendTo(dst__, src__, deviceConfig__) \
    { \
        CsrWifiSmeSmeCommonConfigSetReq *msg__; \
        CsrWifiSmeSmeCommonConfigSetReqCreate(msg__, dst__, src__, deviceConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeSmeCommonConfigSetReqSend(src__, deviceConfig__) \
    CsrWifiSmeSmeCommonConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, deviceConfig__)

/*                                                                              

      
                                       

             
                                     

            
                                   
                                              

                                                                              */
#define CsrWifiSmeSmeCommonConfigSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeSmeCommonConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SME_COMMON_CONFIG_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeSmeCommonConfigSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeSmeCommonConfigSetCfm *msg__; \
        CsrWifiSmeSmeCommonConfigSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeSmeCommonConfigSetCfmSend(dst__, status__) \
    CsrWifiSmeSmeCommonConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                    

             
                                                                

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiSmeSmeStaConfigGetReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeSmeStaConfigGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SME_STA_CONFIG_GET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiSmeSmeStaConfigGetReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiSmeSmeStaConfigGetReq *msg__; \
        CsrWifiSmeSmeStaConfigGetReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeSmeStaConfigGetReqSend(src__, interfaceTag__) \
    CsrWifiSmeSmeStaConfigGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__)

/*                                                                              

      
                                    

             
                                                     

            
                                         
                                                                          
                                                    
                                                 

                                                                              */
#define CsrWifiSmeSmeStaConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, smeConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeSmeStaConfigGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SME_STA_CONFIG_GET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->smeConfig = (smeConfig__);

#define CsrWifiSmeSmeStaConfigGetCfmSendTo(dst__, src__, interfaceTag__, status__, smeConfig__) \
    { \
        CsrWifiSmeSmeStaConfigGetCfm *msg__; \
        CsrWifiSmeSmeStaConfigGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, smeConfig__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeSmeStaConfigGetCfmSend(dst__, interfaceTag__, status__, smeConfig__) \
    CsrWifiSmeSmeStaConfigGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, smeConfig__)

/*                                                                              

      
                                    

             
                                                             

            
                                                                               
                                                                          
                                                   

                                                                              */
#define CsrWifiSmeSmeStaConfigSetReqCreate(msg__, dst__, src__, interfaceTag__, smeConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeSmeStaConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SME_STA_CONFIG_SET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->smeConfig = (smeConfig__);

#define CsrWifiSmeSmeStaConfigSetReqSendTo(dst__, src__, interfaceTag__, smeConfig__) \
    { \
        CsrWifiSmeSmeStaConfigSetReq *msg__; \
        CsrWifiSmeSmeStaConfigSetReqCreate(msg__, dst__, src__, interfaceTag__, smeConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeSmeStaConfigSetReqSend(src__, interfaceTag__, smeConfig__) \
    CsrWifiSmeSmeStaConfigSetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, smeConfig__)

/*                                                                              

      
                                    

             
                                                     

            
                                         
                                                                          
                                                    

                                                                              */
#define CsrWifiSmeSmeStaConfigSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeSmeStaConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_SME_STA_CONFIG_SET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiSmeSmeStaConfigSetCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiSmeSmeStaConfigSetCfm *msg__; \
        CsrWifiSmeSmeStaConfigSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeSmeStaConfigSetCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiSmeSmeStaConfigSetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                                         

             
                                                                           
            

            
                                                                         

                                                                              */
#define CsrWifiSmeStationMacAddressGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeStationMacAddressGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_STATION_MAC_ADDRESS_GET_REQ, dst__, src__);

#define CsrWifiSmeStationMacAddressGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeStationMacAddressGetReq *msg__; \
        CsrWifiSmeStationMacAddressGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeStationMacAddressGetReqSend(src__) \
    CsrWifiSmeStationMacAddressGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                         

             
                                                     

            
                                              
                                                         
                                                           

                                                                              */
#define CsrWifiSmeStationMacAddressGetCfmCreate(msg__, dst__, src__, status__, stationMacAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeStationMacAddressGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_STATION_MAC_ADDRESS_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    memcpy(msg__->stationMacAddress, (stationMacAddress__), sizeof(CsrWifiMacAddress) * 2);

#define CsrWifiSmeStationMacAddressGetCfmSendTo(dst__, src__, status__, stationMacAddress__) \
    { \
        CsrWifiSmeStationMacAddressGetCfm *msg__; \
        CsrWifiSmeStationMacAddressGetCfmCreate(msg__, dst__, src__, status__, stationMacAddress__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeStationMacAddressGetCfmSend(dst__, status__, stationMacAddress__) \
    CsrWifiSmeStationMacAddressGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, stationMacAddress__)

/*                                                                              

      
                          

             
                                                                          
                  
                                                                     
                                           
                                                                         
                                                 
                                                                             
                                                                    
                                                                            
            
                                                                     
                                                                        
          
                                                                             
                                                                             
                                                                 

            
                                                                                
                                                                           
                                                                                 
                                                                          
                                                                           
                          
                                                                           
                                     
                                                           
                                                                  
                                           
                                        
                                                         
                                        
                                                                            
                                                                  

                                                                              */
#define CsrWifiSmeTspecReqCreate(msg__, dst__, src__, interfaceTag__, action__, transactionId__, strict__, ctrlMask__, tspecLength__, tspec__, tclasLength__, tclas__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeTspecReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_TSPEC_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->action = (action__); \
    msg__->transactionId = (transactionId__); \
    msg__->strict = (strict__); \
    msg__->ctrlMask = (ctrlMask__); \
    msg__->tspecLength = (tspecLength__); \
    msg__->tspec = (tspec__); \
    msg__->tclasLength = (tclasLength__); \
    msg__->tclas = (tclas__);

#define CsrWifiSmeTspecReqSendTo(dst__, src__, interfaceTag__, action__, transactionId__, strict__, ctrlMask__, tspecLength__, tspec__, tclasLength__, tclas__) \
    { \
        CsrWifiSmeTspecReq *msg__; \
        CsrWifiSmeTspecReqCreate(msg__, dst__, src__, interfaceTag__, action__, transactionId__, strict__, ctrlMask__, tspecLength__, tspec__, tclasLength__, tclas__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeTspecReqSend(src__, interfaceTag__, action__, transactionId__, strict__, ctrlMask__, tspecLength__, tspec__, tclasLength__, tclas__) \
    CsrWifiSmeTspecReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, interfaceTag__, action__, transactionId__, strict__, ctrlMask__, tspecLength__, tspec__, tclasLength__, tclas__)

/*                                                                              

      
                          

             
                                                                            
                                                        

            
                                            
                                                                             
                                                                             
                            
                                                                         
                             
                                          
                                                           

                                                                              */
#define CsrWifiSmeTspecIndCreate(msg__, dst__, src__, interfaceTag__, transactionId__, tspecResultCode__, tspecLength__, tspec__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeTspecInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_TSPEC_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->transactionId = (transactionId__); \
    msg__->tspecResultCode = (tspecResultCode__); \
    msg__->tspecLength = (tspecLength__); \
    msg__->tspec = (tspec__);

#define CsrWifiSmeTspecIndSendTo(dst__, src__, interfaceTag__, transactionId__, tspecResultCode__, tspecLength__, tspec__) \
    { \
        CsrWifiSmeTspecInd *msg__; \
        CsrWifiSmeTspecIndCreate(msg__, dst__, src__, interfaceTag__, transactionId__, tspecResultCode__, tspecLength__, tspec__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeTspecIndSend(dst__, interfaceTag__, transactionId__, tspecResultCode__, tspecLength__, tspec__) \
    CsrWifiSmeTspecIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, transactionId__, tspecResultCode__, tspecLength__, tspec__)

/*                                                                              

      
                          

             
                                                                           
            

            
                                            
                                                                             
                                                       
                                                                             
                            
                                                                            
                                          
                                                           

                                                                              */
#define CsrWifiSmeTspecCfmCreate(msg__, dst__, src__, interfaceTag__, status__, transactionId__, tspecResultCode__, tspecLength__, tspec__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeTspecCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_TSPEC_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->transactionId = (transactionId__); \
    msg__->tspecResultCode = (tspecResultCode__); \
    msg__->tspecLength = (tspecLength__); \
    msg__->tspec = (tspec__);

#define CsrWifiSmeTspecCfmSendTo(dst__, src__, interfaceTag__, status__, transactionId__, tspecResultCode__, tspecLength__, tspec__) \
    { \
        CsrWifiSmeTspecCfm *msg__; \
        CsrWifiSmeTspecCfmCreate(msg__, dst__, src__, interfaceTag__, status__, transactionId__, tspecResultCode__, tspecLength__, tspec__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeTspecCfmSend(dst__, interfaceTag__, status__, transactionId__, tspecResultCode__, tspecLength__, tspec__) \
    CsrWifiSmeTspecCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, interfaceTag__, status__, transactionId__, tspecResultCode__, tspecLength__, tspec__)

/*                                                                              

      
                                

             
                                                            

            
                                                                         

                                                                              */
#define CsrWifiSmeVersionsGetReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeVersionsGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_VERSIONS_GET_REQ, dst__, src__);

#define CsrWifiSmeVersionsGetReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeVersionsGetReq *msg__; \
        CsrWifiSmeVersionsGetReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeVersionsGetReqSend(src__) \
    CsrWifiSmeVersionsGetReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                                

             
                                                     

            
                                     
                                                
                                         

                                                                              */
#define CsrWifiSmeVersionsGetCfmCreate(msg__, dst__, src__, status__, versions__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeVersionsGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_VERSIONS_GET_CFM, dst__, src__); \
    msg__->status = (status__); \
    msg__->versions = (versions__);

#define CsrWifiSmeVersionsGetCfmSendTo(dst__, src__, status__, versions__) \
    { \
        CsrWifiSmeVersionsGetCfm *msg__; \
        CsrWifiSmeVersionsGetCfmCreate(msg__, dst__, src__, status__, versions__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeVersionsGetCfmSend(dst__, status__, versions__) \
    CsrWifiSmeVersionsGetCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__, versions__)

/*                                                                              

      
                                   

             
                                                                          
                                                                          
                              
                                                                 
                                                                            
                                                                            
                                                                     
                                                           
                                                                          
                                                                            
                                                                            
                                                                            
            
                                                                        
                                                                      
                                                                              
                                                                     
                                                   

            
                                                                                
                                                               
                                                                             
                                                                              
                                               
                                                                          
                                                                           
                                                                                
                                                                             
                                               
                                                                              
                           
                                                                    

                                                                              */
#define CsrWifiSmeWifiFlightmodeReqCreate(msg__, dst__, src__, address__, mibFilesCount__, mibFiles__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWifiFlightmodeReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WIFI_FLIGHTMODE_REQ, dst__, src__); \
    msg__->address = (address__); \
    msg__->mibFilesCount = (mibFilesCount__); \
    msg__->mibFiles = (mibFiles__);

#define CsrWifiSmeWifiFlightmodeReqSendTo(dst__, src__, address__, mibFilesCount__, mibFiles__) \
    { \
        CsrWifiSmeWifiFlightmodeReq *msg__; \
        CsrWifiSmeWifiFlightmodeReqCreate(msg__, dst__, src__, address__, mibFilesCount__, mibFiles__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWifiFlightmodeReqSend(src__, address__, mibFilesCount__, mibFiles__) \
    CsrWifiSmeWifiFlightmodeReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, address__, mibFilesCount__, mibFiles__)

/*                                                                              

      
                                   

             
                                                                           
                                                                         
                                                              
                             

            
                                   
                                              

                                                                              */
#define CsrWifiSmeWifiFlightmodeCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWifiFlightmodeCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WIFI_FLIGHTMODE_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeWifiFlightmodeCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeWifiFlightmodeCfm *msg__; \
        CsrWifiSmeWifiFlightmodeCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWifiFlightmodeCfmSend(dst__, status__) \
    CsrWifiSmeWifiFlightmodeCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                            

             
                                                                         
                                                     

            
                                                                         

                                                                              */
#define CsrWifiSmeWifiOffReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWifiOffReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WIFI_OFF_REQ, dst__, src__);

#define CsrWifiSmeWifiOffReqSendTo(dst__, src__) \
    { \
        CsrWifiSmeWifiOffReq *msg__; \
        CsrWifiSmeWifiOffReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWifiOffReqSend(src__) \
    CsrWifiSmeWifiOffReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__)

/*                                                                              

      
                            

             
                                                                         
                                                           

            
                                   
                                                                      

                                                                              */
#define CsrWifiSmeWifiOffIndCreate(msg__, dst__, src__, reason__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWifiOffInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WIFI_OFF_IND, dst__, src__); \
    msg__->reason = (reason__);

#define CsrWifiSmeWifiOffIndSendTo(dst__, src__, reason__) \
    { \
        CsrWifiSmeWifiOffInd *msg__; \
        CsrWifiSmeWifiOffIndCreate(msg__, dst__, src__, reason__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWifiOffIndSend(dst__, reason__) \
    CsrWifiSmeWifiOffIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, reason__)

/*                                                                              

      
                            

             
                                                                            
                                                                     
                                      
                                                              
                                                   

            
                                   
                                              

                                                                              */
#define CsrWifiSmeWifiOffCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWifiOffCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WIFI_OFF_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeWifiOffCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeWifiOffCfm *msg__; \
        CsrWifiSmeWifiOffCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWifiOffCfmSend(dst__, status__) \
    CsrWifiSmeWifiOffCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                           

             
                                                                        
               
                                                                        
                                                                            
                             
                                                                       
                                                   
                                                                         
                               
                                                                          
                                     

            
                                                                                
                                                               
                                                                             
                                                                              
                                              
                                                                          
                                                                           
                                                                                
                                                                             
                                               
                                                                              
                           
                                                                    

                                                                              */
#define CsrWifiSmeWifiOnReqCreate(msg__, dst__, src__, address__, mibFilesCount__, mibFiles__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWifiOnReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WIFI_ON_REQ, dst__, src__); \
    msg__->address = (address__); \
    msg__->mibFilesCount = (mibFilesCount__); \
    msg__->mibFiles = (mibFiles__);

#define CsrWifiSmeWifiOnReqSendTo(dst__, src__, address__, mibFilesCount__, mibFiles__) \
    { \
        CsrWifiSmeWifiOnReq *msg__; \
        CsrWifiSmeWifiOnReqCreate(msg__, dst__, src__, address__, mibFilesCount__, mibFiles__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWifiOnReqSend(src__, address__, mibFilesCount__, mibFiles__) \
    CsrWifiSmeWifiOnReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, address__, mibFilesCount__, mibFiles__)

/*                                                                              

      
                           

             
                                                                             
                                                        

            
                                    
                                 

                                                                              */
#define CsrWifiSmeWifiOnIndCreate(msg__, dst__, src__, address__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWifiOnInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WIFI_ON_IND, dst__, src__); \
    msg__->address = (address__);

#define CsrWifiSmeWifiOnIndSendTo(dst__, src__, address__) \
    { \
        CsrWifiSmeWifiOnInd *msg__; \
        CsrWifiSmeWifiOnIndCreate(msg__, dst__, src__, address__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWifiOnIndSend(dst__, address__) \
    CsrWifiSmeWifiOnIndSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, address__)

/*                                                                              

      
                           

             
                                                                           
                                                           

            
                                   
                                              

                                                                              */
#define CsrWifiSmeWifiOnCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWifiOnCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WIFI_ON_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeWifiOnCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeWifiOnCfm *msg__; \
        CsrWifiSmeWifiOnCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWifiOnCfmSend(dst__, status__) \
    CsrWifiSmeWifiOnCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

/*                                                                              

      
                                     

             
                                                                             
                                               

            
                                                                            
                           

                                                                              */
#define CsrWifiSmeWpsConfigurationReqCreate(msg__, dst__, src__, wpsConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWpsConfigurationReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WPS_CONFIGURATION_REQ, dst__, src__); \
    msg__->wpsConfig = (wpsConfig__);

#define CsrWifiSmeWpsConfigurationReqSendTo(dst__, src__, wpsConfig__) \
    { \
        CsrWifiSmeWpsConfigurationReq *msg__; \
        CsrWifiSmeWpsConfigurationReqCreate(msg__, dst__, src__, wpsConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWpsConfigurationReqSend(src__, wpsConfig__) \
    CsrWifiSmeWpsConfigurationReqSendTo(CSR_WIFI_SME_LIB_DESTINATION_QUEUE, src__, wpsConfig__)

/*                                                                              

      
                                     

             
            

            
                                   
                                   

                                                                              */
#define CsrWifiSmeWpsConfigurationCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiSmeWpsConfigurationCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_SME_PRIM, CSR_WIFI_SME_WPS_CONFIGURATION_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiSmeWpsConfigurationCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiSmeWpsConfigurationCfm *msg__; \
        CsrWifiSmeWpsConfigurationCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_SME_PRIM, msg__); \
    }

#define CsrWifiSmeWpsConfigurationCfmSend(dst__, status__) \
    CsrWifiSmeWpsConfigurationCfmSendTo(dst__, CSR_WIFI_SME_IFACEQUEUE, status__)

#endif /*                      */
