/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_ROUTER_CTRL_LIB_H__
#define CSR_WIFI_ROUTER_CTRL_LIB_H__

#include "csr_sched.h"
#include "csr_macro.h"
#include "csr_msg_transport.h"

#include "csr_wifi_lib.h"

#include "csr_wifi_router_ctrl_prim.h"
#include "csr_wifi_router_task.h"

/*                                                                             
                                                
  
               
                                                                                      
                                                                            
  
              
                                                                            
                                                                              */
void CsrWifiRouterCtrlFreeUpstreamMessageContents(u16 eventClass, void *message);

/*                                                                             
                                                  
  
               
                                                                                        
                                                                              
  
              
                                                                              
                                                                              */
void CsrWifiRouterCtrlFreeDownstreamMessageContents(u16 eventClass, void *message);

/*                                                                             
                           
                                                                              */
const char* CsrWifiRouterCtrlBlockAckRoleToString(CsrWifiRouterCtrlBlockAckRole value);
const char* CsrWifiRouterCtrlControlIndicationToString(CsrWifiRouterCtrlControlIndication value);
const char* CsrWifiRouterCtrlListActionToString(CsrWifiRouterCtrlListAction value);
const char* CsrWifiRouterCtrlLowPowerModeToString(CsrWifiRouterCtrlLowPowerMode value);
const char* CsrWifiRouterCtrlMediaStatusToString(CsrWifiRouterCtrlMediaStatus value);
const char* CsrWifiRouterCtrlModeToString(CsrWifiRouterCtrlMode value);
const char* CsrWifiRouterCtrlPeerStatusToString(CsrWifiRouterCtrlPeerStatus value);
const char* CsrWifiRouterCtrlPortActionToString(CsrWifiRouterCtrlPortAction value);
const char* CsrWifiRouterCtrlPowersaveTypeToString(CsrWifiRouterCtrlPowersaveType value);
const char* CsrWifiRouterCtrlProtocolDirectionToString(CsrWifiRouterCtrlProtocolDirection value);
const char* CsrWifiRouterCtrlQoSControlToString(CsrWifiRouterCtrlQoSControl value);
const char* CsrWifiRouterCtrlQueueConfigToString(CsrWifiRouterCtrlQueueConfig value);
const char* CsrWifiRouterCtrlTrafficConfigTypeToString(CsrWifiRouterCtrlTrafficConfigType value);
const char* CsrWifiRouterCtrlTrafficPacketTypeToString(CsrWifiRouterCtrlTrafficPacketType value);
const char* CsrWifiRouterCtrlTrafficTypeToString(CsrWifiRouterCtrlTrafficType value);


/*                                                                             
                                  
                                                            
                                                                              */
const char* CsrWifiRouterCtrlPrimTypeToString(CsrPrim msgType);

/*                                                                             
                                          
                                                                              */
extern const char *CsrWifiRouterCtrlUpstreamPrimNames[CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_COUNT];
extern const char *CsrWifiRouterCtrlDownstreamPrimNames[CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_COUNT];

/*                                                                              

      
                                           

             

            
                                                                                  
                     
                     
                     
                     
                     

                                                                              */
#define CsrWifiRouterCtrlBlockAckDisableReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlBlockAckDisableReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_DISABLE_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->macAddress = (macAddress__); \
    msg__->trafficStreamID = (trafficStreamID__); \
    msg__->role = (role__);

#define CsrWifiRouterCtrlBlockAckDisableReqSendTo(dst__, src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__) \
    { \
        CsrWifiRouterCtrlBlockAckDisableReq *msg__; \
        CsrWifiRouterCtrlBlockAckDisableReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlBlockAckDisableReqSend(src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__) \
    CsrWifiRouterCtrlBlockAckDisableReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__)

/*                                                                              

      
                                           

             

            
                                         
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlBlockAckDisableCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlBlockAckDisableCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_DISABLE_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlBlockAckDisableCfmSendTo(dst__, src__, clientData__, interfaceTag__, status__) \
    { \
        CsrWifiRouterCtrlBlockAckDisableCfm *msg__; \
        CsrWifiRouterCtrlBlockAckDisableCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlBlockAckDisableCfmSend(dst__, clientData__, interfaceTag__, status__) \
    CsrWifiRouterCtrlBlockAckDisableCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, status__)

/*                                                                              

      
                                          

             

            
                                                                                  
                     
                     
                     
                     
                     
                     
                     
                     

                                                                              */
#define CsrWifiRouterCtrlBlockAckEnableReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__, bufferSize__, timeout__, ssn__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlBlockAckEnableReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_ENABLE_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->macAddress = (macAddress__); \
    msg__->trafficStreamID = (trafficStreamID__); \
    msg__->role = (role__); \
    msg__->bufferSize = (bufferSize__); \
    msg__->timeout = (timeout__); \
    msg__->ssn = (ssn__);

#define CsrWifiRouterCtrlBlockAckEnableReqSendTo(dst__, src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__, bufferSize__, timeout__, ssn__) \
    { \
        CsrWifiRouterCtrlBlockAckEnableReq *msg__; \
        CsrWifiRouterCtrlBlockAckEnableReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__, bufferSize__, timeout__, ssn__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlBlockAckEnableReqSend(src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__, bufferSize__, timeout__, ssn__) \
    CsrWifiRouterCtrlBlockAckEnableReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, macAddress__, trafficStreamID__, role__, bufferSize__, timeout__, ssn__)

/*                                                                              

      
                                          

             

            
                                         
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlBlockAckEnableCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlBlockAckEnableCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_ENABLE_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlBlockAckEnableCfmSendTo(dst__, src__, clientData__, interfaceTag__, status__) \
    { \
        CsrWifiRouterCtrlBlockAckEnableCfm *msg__; \
        CsrWifiRouterCtrlBlockAckEnableCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlBlockAckEnableCfmSend(dst__, clientData__, interfaceTag__, status__) \
    CsrWifiRouterCtrlBlockAckEnableCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, status__)

/*                                                                              

      
                                         

             

            
                                            
                     
                     
                     
                     
                     

                                                                              */
#define CsrWifiRouterCtrlBlockAckErrorIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, trafficStreamID__, peerMacAddress__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlBlockAckErrorInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_ERROR_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->trafficStreamID = (trafficStreamID__); \
    msg__->peerMacAddress = (peerMacAddress__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlBlockAckErrorIndSendTo(dst__, src__, clientData__, interfaceTag__, trafficStreamID__, peerMacAddress__, status__) \
    { \
        CsrWifiRouterCtrlBlockAckErrorInd *msg__; \
        CsrWifiRouterCtrlBlockAckErrorIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, trafficStreamID__, peerMacAddress__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlBlockAckErrorIndSend(dst__, clientData__, interfaceTag__, trafficStreamID__, peerMacAddress__, status__) \
    CsrWifiRouterCtrlBlockAckErrorIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, trafficStreamID__, peerMacAddress__, status__)

/*                                                                              

      
                                        

             

            
                                                                             
                

                                                                              */
#define CsrWifiRouterCtrlCapabilitiesReqCreate(msg__, dst__, src__, clientData__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlCapabilitiesReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_CAPABILITIES_REQ, dst__, src__); \
    msg__->clientData = (clientData__);

#define CsrWifiRouterCtrlCapabilitiesReqSendTo(dst__, src__, clientData__) \
    { \
        CsrWifiRouterCtrlCapabilitiesReq *msg__; \
        CsrWifiRouterCtrlCapabilitiesReqCreate(msg__, dst__, src__, clientData__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlCapabilitiesReqSend(src__, clientData__) \
    CsrWifiRouterCtrlCapabilitiesReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__)

/*                                                                              

      
                                        

             
                                                                            
        

            
                                             
                      
                                            
                                                     

                                                                              */
#define CsrWifiRouterCtrlCapabilitiesCfmCreate(msg__, dst__, src__, clientData__, commandQueueSize__, trafficQueueSize__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlCapabilitiesCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_CAPABILITIES_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->commandQueueSize = (commandQueueSize__); \
    msg__->trafficQueueSize = (trafficQueueSize__);

#define CsrWifiRouterCtrlCapabilitiesCfmSendTo(dst__, src__, clientData__, commandQueueSize__, trafficQueueSize__) \
    { \
        CsrWifiRouterCtrlCapabilitiesCfm *msg__; \
        CsrWifiRouterCtrlCapabilitiesCfmCreate(msg__, dst__, src__, clientData__, commandQueueSize__, trafficQueueSize__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlCapabilitiesCfmSend(dst__, clientData__, commandQueueSize__, trafficQueueSize__) \
    CsrWifiRouterCtrlCapabilitiesCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, commandQueueSize__, trafficQueueSize__)

/*                                                                              

      
                                              

             

            
                                                                             
                
                
                

                                                                              */
#define CsrWifiRouterCtrlConfigurePowerModeReqCreate(msg__, dst__, src__, clientData__, mode__, wakeHost__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlConfigurePowerModeReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_CONFIGURE_POWER_MODE_REQ, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->mode = (mode__); \
    msg__->wakeHost = (wakeHost__);

#define CsrWifiRouterCtrlConfigurePowerModeReqSendTo(dst__, src__, clientData__, mode__, wakeHost__) \
    { \
        CsrWifiRouterCtrlConfigurePowerModeReq *msg__; \
        CsrWifiRouterCtrlConfigurePowerModeReqCreate(msg__, dst__, src__, clientData__, mode__, wakeHost__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlConfigurePowerModeReqSend(src__, clientData__, mode__, wakeHost__) \
    CsrWifiRouterCtrlConfigurePowerModeReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__, mode__, wakeHost__)

/*                                                                              

      
                                     

             

            
                                           
                    
                    
                    
                    

                                                                              */
#define CsrWifiRouterCtrlConnectedIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__, peerStatus__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlConnectedInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_CONNECTED_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->peerMacAddress = (peerMacAddress__); \
    msg__->peerStatus = (peerStatus__);

#define CsrWifiRouterCtrlConnectedIndSendTo(dst__, src__, clientData__, interfaceTag__, peerMacAddress__, peerStatus__) \
    { \
        CsrWifiRouterCtrlConnectedInd *msg__; \
        CsrWifiRouterCtrlConnectedIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__, peerStatus__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlConnectedIndSend(dst__, clientData__, interfaceTag__, peerMacAddress__, peerStatus__) \
    CsrWifiRouterCtrlConnectedIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, peerMacAddress__, peerStatus__)

/*                                                                              

      
                               

             
                                                                     

            
                                                                                    
                                                 
                                                  
                                                        
                                                  
                                                        
                                                  

                                                                              */
#define CsrWifiRouterCtrlHipReqCreate(msg__, dst__, src__, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlHipReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_HIP_REQ, dst__, src__); \
    msg__->mlmeCommandLength = (mlmeCommandLength__); \
    msg__->mlmeCommand = (mlmeCommand__); \
    msg__->dataRef1Length = (dataRef1Length__); \
    msg__->dataRef1 = (dataRef1__); \
    msg__->dataRef2Length = (dataRef2Length__); \
    msg__->dataRef2 = (dataRef2__);

#define CsrWifiRouterCtrlHipReqSendTo(dst__, src__, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__) \
    { \
        CsrWifiRouterCtrlHipReq *msg__; \
        CsrWifiRouterCtrlHipReqCreate(msg__, dst__, src__, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlHipReqSend(src__, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__) \
    CsrWifiRouterCtrlHipReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__)

/*                                                                              

      
                               

             
                                                                       

            
                                              
                                                 
                                                  
                                                        
                                                  
                                                        
                                                  

                                                                              */
#define CsrWifiRouterCtrlHipIndCreate(msg__, dst__, src__, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlHipInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_HIP_IND, dst__, src__); \
    msg__->mlmeCommandLength = (mlmeCommandLength__); \
    msg__->mlmeCommand = (mlmeCommand__); \
    msg__->dataRef1Length = (dataRef1Length__); \
    msg__->dataRef1 = (dataRef1__); \
    msg__->dataRef2Length = (dataRef2Length__); \
    msg__->dataRef2 = (dataRef2__);

#define CsrWifiRouterCtrlHipIndSendTo(dst__, src__, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__) \
    { \
        CsrWifiRouterCtrlHipInd *msg__; \
        CsrWifiRouterCtrlHipIndCreate(msg__, dst__, src__, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlHipIndSend(dst__, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__) \
    CsrWifiRouterCtrlHipIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, mlmeCommandLength__, mlmeCommand__, dataRef1Length__, dataRef1__, dataRef2Length__, dataRef2__)

/*                                                                              

      
                                         

             

            
                                           
                    
                    
                    

                                                                              */
#define CsrWifiRouterCtrlM4ReadyToSendIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlM4ReadyToSendInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_M4_READY_TO_SEND_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->peerMacAddress = (peerMacAddress__);

#define CsrWifiRouterCtrlM4ReadyToSendIndSendTo(dst__, src__, clientData__, interfaceTag__, peerMacAddress__) \
    { \
        CsrWifiRouterCtrlM4ReadyToSendInd *msg__; \
        CsrWifiRouterCtrlM4ReadyToSendIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlM4ReadyToSendIndSend(dst__, clientData__, interfaceTag__, peerMacAddress__) \
    CsrWifiRouterCtrlM4ReadyToSendIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, peerMacAddress__)

/*                                                                              

      
                                      

             

            
                                                                               
                  
                  

                                                                              */
#define CsrWifiRouterCtrlM4TransmitReqCreate(msg__, dst__, src__, interfaceTag__, clientData__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlM4TransmitReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_M4_TRANSMIT_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__);

#define CsrWifiRouterCtrlM4TransmitReqSendTo(dst__, src__, interfaceTag__, clientData__) \
    { \
        CsrWifiRouterCtrlM4TransmitReq *msg__; \
        CsrWifiRouterCtrlM4TransmitReqCreate(msg__, dst__, src__, interfaceTag__, clientData__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlM4TransmitReqSend(src__, interfaceTag__, clientData__) \
    CsrWifiRouterCtrlM4TransmitReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__)

/*                                                                              

      
                                         

             

            
                                           
                    
                    
                    
                    

                                                                              */
#define CsrWifiRouterCtrlM4TransmittedIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlM4TransmittedInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_M4_TRANSMITTED_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->peerMacAddress = (peerMacAddress__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlM4TransmittedIndSendTo(dst__, src__, clientData__, interfaceTag__, peerMacAddress__, status__) \
    { \
        CsrWifiRouterCtrlM4TransmittedInd *msg__; \
        CsrWifiRouterCtrlM4TransmittedIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlM4TransmittedIndSend(dst__, clientData__, interfaceTag__, peerMacAddress__, status__) \
    CsrWifiRouterCtrlM4TransmittedIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, peerMacAddress__, status__)

/*                                                                              

      
                                       

             

            
                                                                               
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlMediaStatusReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, mediaStatus__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlMediaStatusReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_MEDIA_STATUS_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->mediaStatus = (mediaStatus__);

#define CsrWifiRouterCtrlMediaStatusReqSendTo(dst__, src__, interfaceTag__, clientData__, mediaStatus__) \
    { \
        CsrWifiRouterCtrlMediaStatusReq *msg__; \
        CsrWifiRouterCtrlMediaStatusReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, mediaStatus__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlMediaStatusReqSend(src__, interfaceTag__, clientData__, mediaStatus__) \
    CsrWifiRouterCtrlMediaStatusReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, mediaStatus__)

/*                                                                              

      
                                      

             

            
                                           
                    
                    
                    
                    

                                                                              */
#define CsrWifiRouterCtrlMicFailureIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__, unicastPdu__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlMicFailureInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_MIC_FAILURE_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->peerMacAddress = (peerMacAddress__); \
    msg__->unicastPdu = (unicastPdu__);

#define CsrWifiRouterCtrlMicFailureIndSendTo(dst__, src__, clientData__, interfaceTag__, peerMacAddress__, unicastPdu__) \
    { \
        CsrWifiRouterCtrlMicFailureInd *msg__; \
        CsrWifiRouterCtrlMicFailureIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__, unicastPdu__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlMicFailureIndSend(dst__, clientData__, interfaceTag__, peerMacAddress__, unicastPdu__) \
    CsrWifiRouterCtrlMicFailureIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, peerMacAddress__, unicastPdu__)

/*                                                                              

      
                                   

             

            
                                                                                      
                         
                         
                         
                                                                               
                            
                                                                      
                                                     
                                                                        
                                                                              
                                                                                
                                                                              
                                                                        

                                                                              */
#define CsrWifiRouterCtrlModeSetReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, mode__, bssid__, protection__, intraBssDistEnabled__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlModeSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_MODE_SET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->mode = (mode__); \
    msg__->bssid = (bssid__); \
    msg__->protection = (protection__); \
    msg__->intraBssDistEnabled = (intraBssDistEnabled__);

#define CsrWifiRouterCtrlModeSetReqSendTo(dst__, src__, interfaceTag__, clientData__, mode__, bssid__, protection__, intraBssDistEnabled__) \
    { \
        CsrWifiRouterCtrlModeSetReq *msg__; \
        CsrWifiRouterCtrlModeSetReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, mode__, bssid__, protection__, intraBssDistEnabled__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlModeSetReqSend(src__, interfaceTag__, clientData__, mode__, bssid__, protection__, intraBssDistEnabled__) \
    CsrWifiRouterCtrlModeSetReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, mode__, bssid__, protection__, intraBssDistEnabled__)

/*                                                                              

      
                                   

             

            
                                         
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlModeSetCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, mode__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlModeSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_MODE_SET_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->mode = (mode__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlModeSetCfmSendTo(dst__, src__, clientData__, interfaceTag__, mode__, status__) \
    { \
        CsrWifiRouterCtrlModeSetCfm *msg__; \
        CsrWifiRouterCtrlModeSetCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, mode__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlModeSetCfmSend(dst__, clientData__, interfaceTag__, mode__, status__) \
    CsrWifiRouterCtrlModeSetCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, mode__, status__)

/*                                                                              

      
                                            

             

            
                                              
                       
                       
                       
                       
                       

                                                                              */
#define CsrWifiRouterCtrlMulticastAddressIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, action__, setAddressesCount__, setAddresses__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlMulticastAddressInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_MULTICAST_ADDRESS_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->action = (action__); \
    msg__->setAddressesCount = (setAddressesCount__); \
    msg__->setAddresses = (setAddresses__);

#define CsrWifiRouterCtrlMulticastAddressIndSendTo(dst__, src__, clientData__, interfaceTag__, action__, setAddressesCount__, setAddresses__) \
    { \
        CsrWifiRouterCtrlMulticastAddressInd *msg__; \
        CsrWifiRouterCtrlMulticastAddressIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, action__, setAddressesCount__, setAddresses__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlMulticastAddressIndSend(dst__, clientData__, interfaceTag__, action__, setAddressesCount__, setAddresses__) \
    CsrWifiRouterCtrlMulticastAddressIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, action__, setAddressesCount__, setAddresses__)

/*                                                                              

      
                                            

             

            
                       
                       
                       
                       
                       
                       

                                                                              */
#define CsrWifiRouterCtrlMulticastAddressResCreate(msg__, dst__, src__, interfaceTag__, clientData__, status__, action__, getAddressesCount__, getAddresses__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlMulticastAddressRes), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_MULTICAST_ADDRESS_RES, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->status = (status__); \
    msg__->action = (action__); \
    msg__->getAddressesCount = (getAddressesCount__); \
    msg__->getAddresses = (getAddresses__);

#define CsrWifiRouterCtrlMulticastAddressResSendTo(dst__, src__, interfaceTag__, clientData__, status__, action__, getAddressesCount__, getAddresses__) \
    { \
        CsrWifiRouterCtrlMulticastAddressRes *msg__; \
        CsrWifiRouterCtrlMulticastAddressResCreate(msg__, dst__, src__, interfaceTag__, clientData__, status__, action__, getAddressesCount__, getAddresses__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlMulticastAddressResSend(src__, interfaceTag__, clientData__, status__, action__, getAddressesCount__, getAddresses__) \
    CsrWifiRouterCtrlMulticastAddressResSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, status__, action__, getAddressesCount__, getAddresses__)

/*                                                                              

      
                                   

             

            
                                                                                 
                    
                    
                    
                    
                    

                                                                              */
#define CsrWifiRouterCtrlPeerAddReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, peerMacAddress__, associationId__, staInfo__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlPeerAddReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_PEER_ADD_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->peerMacAddress = (peerMacAddress__); \
    msg__->associationId = (associationId__); \
    msg__->staInfo = (staInfo__);

#define CsrWifiRouterCtrlPeerAddReqSendTo(dst__, src__, interfaceTag__, clientData__, peerMacAddress__, associationId__, staInfo__) \
    { \
        CsrWifiRouterCtrlPeerAddReq *msg__; \
        CsrWifiRouterCtrlPeerAddReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, peerMacAddress__, associationId__, staInfo__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlPeerAddReqSend(src__, interfaceTag__, clientData__, peerMacAddress__, associationId__, staInfo__) \
    CsrWifiRouterCtrlPeerAddReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, peerMacAddress__, associationId__, staInfo__)

/*                                                                              

      
                                   

             

            
                                             
                      
                      
                      
                      
                      

                                                                              */
#define CsrWifiRouterCtrlPeerAddCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__, peerRecordHandle__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlPeerAddCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_PEER_ADD_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->peerMacAddress = (peerMacAddress__); \
    msg__->peerRecordHandle = (peerRecordHandle__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlPeerAddCfmSendTo(dst__, src__, clientData__, interfaceTag__, peerMacAddress__, peerRecordHandle__, status__) \
    { \
        CsrWifiRouterCtrlPeerAddCfm *msg__; \
        CsrWifiRouterCtrlPeerAddCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__, peerRecordHandle__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlPeerAddCfmSend(dst__, clientData__, interfaceTag__, peerMacAddress__, peerRecordHandle__, status__) \
    CsrWifiRouterCtrlPeerAddCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, peerMacAddress__, peerRecordHandle__, status__)

/*                                                                              

      
                                   

             

            
                                                                                   
                      
                      
                      

                                                                              */
#define CsrWifiRouterCtrlPeerDelReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, peerRecordHandle__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlPeerDelReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_PEER_DEL_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->peerRecordHandle = (peerRecordHandle__);

#define CsrWifiRouterCtrlPeerDelReqSendTo(dst__, src__, interfaceTag__, clientData__, peerRecordHandle__) \
    { \
        CsrWifiRouterCtrlPeerDelReq *msg__; \
        CsrWifiRouterCtrlPeerDelReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, peerRecordHandle__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlPeerDelReqSend(src__, interfaceTag__, clientData__, peerRecordHandle__) \
    CsrWifiRouterCtrlPeerDelReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, peerRecordHandle__)

/*                                                                              

      
                                   

             

            
                                         
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlPeerDelCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlPeerDelCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_PEER_DEL_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlPeerDelCfmSendTo(dst__, src__, clientData__, interfaceTag__, status__) \
    { \
        CsrWifiRouterCtrlPeerDelCfm *msg__; \
        CsrWifiRouterCtrlPeerDelCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlPeerDelCfmSend(dst__, clientData__, interfaceTag__, status__) \
    CsrWifiRouterCtrlPeerDelCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, status__)

/*                                                                              

      
                                      

             

            
                                                                                   
                      
                      
                      
                      

                                                                              */
#define CsrWifiRouterCtrlPeerUpdateReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, peerRecordHandle__, powersaveMode__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlPeerUpdateReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_PEER_UPDATE_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->peerRecordHandle = (peerRecordHandle__); \
    msg__->powersaveMode = (powersaveMode__);

#define CsrWifiRouterCtrlPeerUpdateReqSendTo(dst__, src__, interfaceTag__, clientData__, peerRecordHandle__, powersaveMode__) \
    { \
        CsrWifiRouterCtrlPeerUpdateReq *msg__; \
        CsrWifiRouterCtrlPeerUpdateReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, peerRecordHandle__, powersaveMode__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlPeerUpdateReqSend(src__, interfaceTag__, clientData__, peerRecordHandle__, powersaveMode__) \
    CsrWifiRouterCtrlPeerUpdateReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, peerRecordHandle__, powersaveMode__)

/*                                                                              

      
                                      

             

            
                                         
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlPeerUpdateCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlPeerUpdateCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_PEER_UPDATE_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlPeerUpdateCfmSendTo(dst__, src__, clientData__, interfaceTag__, status__) \
    { \
        CsrWifiRouterCtrlPeerUpdateCfm *msg__; \
        CsrWifiRouterCtrlPeerUpdateCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlPeerUpdateCfmSend(dst__, clientData__, interfaceTag__, status__) \
    CsrWifiRouterCtrlPeerUpdateCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, status__)

/*                                                                              

      
                                         

             

            
                                                                                         
                            
                            
                            
                            
                            
                            

                                                                              */
#define CsrWifiRouterCtrlPortConfigureReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, uncontrolledPortAction__, controlledPortAction__, macAddress__, setProtection__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlPortConfigureReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_PORT_CONFIGURE_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->uncontrolledPortAction = (uncontrolledPortAction__); \
    msg__->controlledPortAction = (controlledPortAction__); \
    msg__->macAddress = (macAddress__); \
    msg__->setProtection = (setProtection__);

#define CsrWifiRouterCtrlPortConfigureReqSendTo(dst__, src__, interfaceTag__, clientData__, uncontrolledPortAction__, controlledPortAction__, macAddress__, setProtection__) \
    { \
        CsrWifiRouterCtrlPortConfigureReq *msg__; \
        CsrWifiRouterCtrlPortConfigureReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, uncontrolledPortAction__, controlledPortAction__, macAddress__, setProtection__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlPortConfigureReqSend(src__, interfaceTag__, clientData__, uncontrolledPortAction__, controlledPortAction__, macAddress__, setProtection__) \
    CsrWifiRouterCtrlPortConfigureReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, uncontrolledPortAction__, controlledPortAction__, macAddress__, setProtection__)

/*                                                                              

      
                                         

             

            
                                         
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlPortConfigureCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__, macAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlPortConfigureCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_PORT_CONFIGURE_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->macAddress = (macAddress__);

#define CsrWifiRouterCtrlPortConfigureCfmSendTo(dst__, src__, clientData__, interfaceTag__, status__, macAddress__) \
    { \
        CsrWifiRouterCtrlPortConfigureCfm *msg__; \
        CsrWifiRouterCtrlPortConfigureCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__, macAddress__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlPortConfigureCfmSend(dst__, clientData__, interfaceTag__, status__, macAddress__) \
    CsrWifiRouterCtrlPortConfigureCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, status__, macAddress__)

/*                                                                              

      
                                      

             

            
                                                                               
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlQosControlReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, control__, queueConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlQosControlReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_QOS_CONTROL_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->control = (control__); \
    msg__->queueConfig = (queueConfig__);

#define CsrWifiRouterCtrlQosControlReqSendTo(dst__, src__, interfaceTag__, clientData__, control__, queueConfig__) \
    { \
        CsrWifiRouterCtrlQosControlReq *msg__; \
        CsrWifiRouterCtrlQosControlReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, control__, queueConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlQosControlReqSend(src__, interfaceTag__, clientData__, control__, queueConfig__) \
    CsrWifiRouterCtrlQosControlReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, control__, queueConfig__)

/*                                                                              

      
                                               

             

            
                                                                             
                

                                                                              */
#define CsrWifiRouterCtrlRawSdioDeinitialiseReqCreate(msg__, dst__, src__, clientData__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlRawSdioDeinitialiseReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_RAW_SDIO_DEINITIALISE_REQ, dst__, src__); \
    msg__->clientData = (clientData__);

#define CsrWifiRouterCtrlRawSdioDeinitialiseReqSendTo(dst__, src__, clientData__) \
    { \
        CsrWifiRouterCtrlRawSdioDeinitialiseReq *msg__; \
        CsrWifiRouterCtrlRawSdioDeinitialiseReqCreate(msg__, dst__, src__, clientData__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlRawSdioDeinitialiseReqSend(src__, clientData__) \
    CsrWifiRouterCtrlRawSdioDeinitialiseReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__)

/*                                                                              

      
                                               

             

            
                                       
                
                

                                                                              */
#define CsrWifiRouterCtrlRawSdioDeinitialiseCfmCreate(msg__, dst__, src__, clientData__, result__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlRawSdioDeinitialiseCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_RAW_SDIO_DEINITIALISE_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->result = (result__);

#define CsrWifiRouterCtrlRawSdioDeinitialiseCfmSendTo(dst__, src__, clientData__, result__) \
    { \
        CsrWifiRouterCtrlRawSdioDeinitialiseCfm *msg__; \
        CsrWifiRouterCtrlRawSdioDeinitialiseCfmCreate(msg__, dst__, src__, clientData__, result__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlRawSdioDeinitialiseCfmSend(dst__, clientData__, result__) \
    CsrWifiRouterCtrlRawSdioDeinitialiseCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, result__)

/*                                                                              

      
                                             

             

            
                                                                             
                

                                                                              */
#define CsrWifiRouterCtrlRawSdioInitialiseReqCreate(msg__, dst__, src__, clientData__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlRawSdioInitialiseReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_RAW_SDIO_INITIALISE_REQ, dst__, src__); \
    msg__->clientData = (clientData__);

#define CsrWifiRouterCtrlRawSdioInitialiseReqSendTo(dst__, src__, clientData__) \
    { \
        CsrWifiRouterCtrlRawSdioInitialiseReq *msg__; \
        CsrWifiRouterCtrlRawSdioInitialiseReqCreate(msg__, dst__, src__, clientData__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlRawSdioInitialiseReqSend(src__, clientData__) \
    CsrWifiRouterCtrlRawSdioInitialiseReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__)

/*                                                                              

      
                                             

             

            
                                             
                      
                      
                      
                      
                      
                      
                      
                      
                      
                      

                                                                              */
#define CsrWifiRouterCtrlRawSdioInitialiseCfmCreate(msg__, dst__, src__, clientData__, result__, byteRead__, byteWrite__, firmwareDownload__, reset__, coreDumpPrepare__, byteBlockRead__, gpRead16__, gpWrite16__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlRawSdioInitialiseCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_RAW_SDIO_INITIALISE_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->result = (result__); \
    msg__->byteRead = (byteRead__); \
    msg__->byteWrite = (byteWrite__); \
    msg__->firmwareDownload = (firmwareDownload__); \
    msg__->reset = (reset__); \
    msg__->coreDumpPrepare = (coreDumpPrepare__); \
    msg__->byteBlockRead = (byteBlockRead__); \
    msg__->gpRead16 = (gpRead16__); \
    msg__->gpWrite16 = (gpWrite16__);

#define CsrWifiRouterCtrlRawSdioInitialiseCfmSendTo(dst__, src__, clientData__, result__, byteRead__, byteWrite__, firmwareDownload__, reset__, coreDumpPrepare__, byteBlockRead__, gpRead16__, gpWrite16__) \
    { \
        CsrWifiRouterCtrlRawSdioInitialiseCfm *msg__; \
        CsrWifiRouterCtrlRawSdioInitialiseCfmCreate(msg__, dst__, src__, clientData__, result__, byteRead__, byteWrite__, firmwareDownload__, reset__, coreDumpPrepare__, byteBlockRead__, gpRead16__, gpWrite16__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlRawSdioInitialiseCfmSend(dst__, clientData__, result__, byteRead__, byteWrite__, firmwareDownload__, reset__, coreDumpPrepare__, byteBlockRead__, gpRead16__, gpWrite16__) \
    CsrWifiRouterCtrlRawSdioInitialiseCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, result__, byteRead__, byteWrite__, firmwareDownload__, reset__, coreDumpPrepare__, byteBlockRead__, gpRead16__, gpWrite16__)

/*                                                                              

      
                                  

             

            
                                            
                     
                     

                                                                              */
#define CsrWifiRouterCtrlResumeIndCreate(msg__, dst__, src__, clientData__, powerMaintained__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlResumeInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_RESUME_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->powerMaintained = (powerMaintained__);

#define CsrWifiRouterCtrlResumeIndSendTo(dst__, src__, clientData__, powerMaintained__) \
    { \
        CsrWifiRouterCtrlResumeInd *msg__; \
        CsrWifiRouterCtrlResumeIndCreate(msg__, dst__, src__, clientData__, powerMaintained__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlResumeIndSend(dst__, clientData__, powerMaintained__) \
    CsrWifiRouterCtrlResumeIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, powerMaintained__)

/*                                                                              

      
                                  

             

            
                
                

                                                                              */
#define CsrWifiRouterCtrlResumeResCreate(msg__, dst__, src__, clientData__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlResumeRes), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_RESUME_RES, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlResumeResSendTo(dst__, src__, clientData__, status__) \
    { \
        CsrWifiRouterCtrlResumeRes *msg__; \
        CsrWifiRouterCtrlResumeResCreate(msg__, dst__, src__, clientData__, status__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlResumeResSend(src__, clientData__, status__) \
    CsrWifiRouterCtrlResumeResSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__, status__)

/*                                                                              

      
                                       

             

            
                                         
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlStaInactiveIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, staAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlStaInactiveInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_STA_INACTIVE_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->staAddress = (staAddress__);

#define CsrWifiRouterCtrlStaInactiveIndSendTo(dst__, src__, clientData__, interfaceTag__, staAddress__) \
    { \
        CsrWifiRouterCtrlStaInactiveInd *msg__; \
        CsrWifiRouterCtrlStaInactiveIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, staAddress__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlStaInactiveIndSend(dst__, clientData__, interfaceTag__, staAddress__) \
    CsrWifiRouterCtrlStaInactiveIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, staAddress__)

/*                                                                              

      
                                   

             

            
                                        
                 
                 
                 

                                                                              */
#define CsrWifiRouterCtrlSuspendIndCreate(msg__, dst__, src__, clientData__, hardSuspend__, d3Suspend__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlSuspendInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_SUSPEND_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->hardSuspend = (hardSuspend__); \
    msg__->d3Suspend = (d3Suspend__);

#define CsrWifiRouterCtrlSuspendIndSendTo(dst__, src__, clientData__, hardSuspend__, d3Suspend__) \
    { \
        CsrWifiRouterCtrlSuspendInd *msg__; \
        CsrWifiRouterCtrlSuspendIndCreate(msg__, dst__, src__, clientData__, hardSuspend__, d3Suspend__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlSuspendIndSend(dst__, clientData__, hardSuspend__, d3Suspend__) \
    CsrWifiRouterCtrlSuspendIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, hardSuspend__, d3Suspend__)

/*                                                                              

      
                                   

             

            
                
                

                                                                              */
#define CsrWifiRouterCtrlSuspendResCreate(msg__, dst__, src__, clientData__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlSuspendRes), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_SUSPEND_RES, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlSuspendResSendTo(dst__, src__, clientData__, status__) \
    { \
        CsrWifiRouterCtrlSuspendRes *msg__; \
        CsrWifiRouterCtrlSuspendResCreate(msg__, dst__, src__, clientData__, status__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlSuspendResSend(src__, clientData__, status__) \
    CsrWifiRouterCtrlSuspendResSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__, status__)

/*                                                                              

      
                                    

             

            
                                                                               
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlTclasAddReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, tclasLength__, tclas__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlTclasAddReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_TCLAS_ADD_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->tclasLength = (tclasLength__); \
    msg__->tclas = (tclas__);

#define CsrWifiRouterCtrlTclasAddReqSendTo(dst__, src__, interfaceTag__, clientData__, tclasLength__, tclas__) \
    { \
        CsrWifiRouterCtrlTclasAddReq *msg__; \
        CsrWifiRouterCtrlTclasAddReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, tclasLength__, tclas__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlTclasAddReqSend(src__, interfaceTag__, clientData__, tclasLength__, tclas__) \
    CsrWifiRouterCtrlTclasAddReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, tclasLength__, tclas__)

/*                                                                              

      
                                    

             

            
                                         
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlTclasAddCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlTclasAddCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_TCLAS_ADD_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlTclasAddCfmSendTo(dst__, src__, clientData__, interfaceTag__, status__) \
    { \
        CsrWifiRouterCtrlTclasAddCfm *msg__; \
        CsrWifiRouterCtrlTclasAddCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlTclasAddCfmSend(dst__, clientData__, interfaceTag__, status__) \
    CsrWifiRouterCtrlTclasAddCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, status__)

/*                                                                              

      
                                    

             

            
                                                                               
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlTclasDelReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, tclasLength__, tclas__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlTclasDelReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_TCLAS_DEL_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->tclasLength = (tclasLength__); \
    msg__->tclas = (tclas__);

#define CsrWifiRouterCtrlTclasDelReqSendTo(dst__, src__, interfaceTag__, clientData__, tclasLength__, tclas__) \
    { \
        CsrWifiRouterCtrlTclasDelReq *msg__; \
        CsrWifiRouterCtrlTclasDelReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, tclasLength__, tclas__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlTclasDelReqSend(src__, interfaceTag__, clientData__, tclasLength__, tclas__) \
    CsrWifiRouterCtrlTclasDelReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, tclasLength__, tclas__)

/*                                                                              

      
                                    

             

            
                                         
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlTclasDelCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlTclasDelCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_TCLAS_DEL_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlTclasDelCfmSendTo(dst__, src__, clientData__, interfaceTag__, status__) \
    { \
        CsrWifiRouterCtrlTclasDelCfm *msg__; \
        CsrWifiRouterCtrlTclasDelCfmCreate(msg__, dst__, src__, clientData__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlTclasDelCfmSend(dst__, clientData__, interfaceTag__, status__) \
    CsrWifiRouterCtrlTclasDelCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, status__)

/*                                                                              

      
                                                 

             

            
                                                                               
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlTrafficClassificationReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, trafficType__, period__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlTrafficClassificationReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_TRAFFIC_CLASSIFICATION_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->trafficType = (trafficType__); \
    msg__->period = (period__);

#define CsrWifiRouterCtrlTrafficClassificationReqSendTo(dst__, src__, interfaceTag__, clientData__, trafficType__, period__) \
    { \
        CsrWifiRouterCtrlTrafficClassificationReq *msg__; \
        CsrWifiRouterCtrlTrafficClassificationReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, trafficType__, period__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlTrafficClassificationReqSend(src__, interfaceTag__, clientData__, trafficType__, period__) \
    CsrWifiRouterCtrlTrafficClassificationReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, trafficType__, period__)

/*                                                                              

      
                                         

             

            
                                                                                    
                       
                       
                       
                       

                                                                              */
#define CsrWifiRouterCtrlTrafficConfigReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, trafficConfigType__, config__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlTrafficConfigReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_TRAFFIC_CONFIG_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->clientData = (clientData__); \
    msg__->trafficConfigType = (trafficConfigType__); \
    msg__->config = (config__);

#define CsrWifiRouterCtrlTrafficConfigReqSendTo(dst__, src__, interfaceTag__, clientData__, trafficConfigType__, config__) \
    { \
        CsrWifiRouterCtrlTrafficConfigReq *msg__; \
        CsrWifiRouterCtrlTrafficConfigReqCreate(msg__, dst__, src__, interfaceTag__, clientData__, trafficConfigType__, config__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlTrafficConfigReqSend(src__, interfaceTag__, clientData__, trafficConfigType__, config__) \
    CsrWifiRouterCtrlTrafficConfigReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, clientData__, trafficConfigType__, config__)

/*                                                                              

      
                                           

             

            
                                         
                  
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlTrafficProtocolIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, packetType__, direction__, srcAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlTrafficProtocolInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_TRAFFIC_PROTOCOL_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->packetType = (packetType__); \
    msg__->direction = (direction__); \
    msg__->srcAddress = (srcAddress__);

#define CsrWifiRouterCtrlTrafficProtocolIndSendTo(dst__, src__, clientData__, interfaceTag__, packetType__, direction__, srcAddress__) \
    { \
        CsrWifiRouterCtrlTrafficProtocolInd *msg__; \
        CsrWifiRouterCtrlTrafficProtocolIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, packetType__, direction__, srcAddress__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlTrafficProtocolIndSend(dst__, clientData__, interfaceTag__, packetType__, direction__, srcAddress__) \
    CsrWifiRouterCtrlTrafficProtocolIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, packetType__, direction__, srcAddress__)

/*                                                                              

      
                                         

             

            
                                         
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlTrafficSampleIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, stats__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlTrafficSampleInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_TRAFFIC_SAMPLE_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->stats = (stats__);

#define CsrWifiRouterCtrlTrafficSampleIndSendTo(dst__, src__, clientData__, interfaceTag__, stats__) \
    { \
        CsrWifiRouterCtrlTrafficSampleInd *msg__; \
        CsrWifiRouterCtrlTrafficSampleIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, stats__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlTrafficSampleIndSend(dst__, clientData__, interfaceTag__, stats__) \
    CsrWifiRouterCtrlTrafficSampleIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, stats__)

/*                                                                              

      
                                           

             

            
                                           
                    
                    
                    

                                                                              */
#define CsrWifiRouterCtrlUnexpectedFrameIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlUnexpectedFrameInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_UNEXPECTED_FRAME_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->peerMacAddress = (peerMacAddress__);

#define CsrWifiRouterCtrlUnexpectedFrameIndSendTo(dst__, src__, clientData__, interfaceTag__, peerMacAddress__) \
    { \
        CsrWifiRouterCtrlUnexpectedFrameInd *msg__; \
        CsrWifiRouterCtrlUnexpectedFrameIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, peerMacAddress__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlUnexpectedFrameIndSend(dst__, clientData__, interfaceTag__, peerMacAddress__) \
    CsrWifiRouterCtrlUnexpectedFrameIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, peerMacAddress__)

/*                                                                              

      
                                      

             

            
                                                                                  
                     
                     

                                                                              */
#define CsrWifiRouterCtrlWapiFilterReqCreate(msg__, dst__, src__, interfaceTag__, isWapiConnected__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWapiFilterReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WAPI_FILTER_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->isWapiConnected = (isWapiConnected__);

#define CsrWifiRouterCtrlWapiFilterReqSendTo(dst__, src__, interfaceTag__, isWapiConnected__) \
    { \
        CsrWifiRouterCtrlWapiFilterReq *msg__; \
        CsrWifiRouterCtrlWapiFilterReqCreate(msg__, dst__, src__, interfaceTag__, isWapiConnected__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWapiFilterReqSend(src__, interfaceTag__, isWapiConnected__) \
    CsrWifiRouterCtrlWapiFilterReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, isWapiConnected__)

/*                                                                              

      
                                               

             

            
                                                                               
                  
                  

                                                                              */
#define CsrWifiRouterCtrlWapiMulticastFilterReqCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWapiMulticastFilterReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WAPI_MULTICAST_FILTER_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlWapiMulticastFilterReqSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiRouterCtrlWapiMulticastFilterReq *msg__; \
        CsrWifiRouterCtrlWapiMulticastFilterReqCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWapiMulticastFilterReqSend(src__, interfaceTag__, status__) \
    CsrWifiRouterCtrlWapiMulticastFilterReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, status__)

/*                                                                              

      
                                          

             

            
                                         
                  
                  
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlWapiRxMicCheckIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, signalLength__, signal__, dataLength__, data__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWapiRxMicCheckInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WAPI_RX_MIC_CHECK_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->signalLength = (signalLength__); \
    msg__->signal = (signal__); \
    msg__->dataLength = (dataLength__); \
    msg__->data = (data__);

#define CsrWifiRouterCtrlWapiRxMicCheckIndSendTo(dst__, src__, clientData__, interfaceTag__, signalLength__, signal__, dataLength__, data__) \
    { \
        CsrWifiRouterCtrlWapiRxMicCheckInd *msg__; \
        CsrWifiRouterCtrlWapiRxMicCheckIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, signalLength__, signal__, dataLength__, data__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWapiRxMicCheckIndSend(dst__, clientData__, interfaceTag__, signalLength__, signal__, dataLength__, data__) \
    CsrWifiRouterCtrlWapiRxMicCheckIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, signalLength__, signal__, dataLength__, data__)

/*                                                                              

      
                                     

             

            
                                                                               
                  
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlWapiRxPktReqCreate(msg__, dst__, src__, interfaceTag__, signalLength__, signal__, dataLength__, data__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWapiRxPktReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WAPI_RX_PKT_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->signalLength = (signalLength__); \
    msg__->signal = (signal__); \
    msg__->dataLength = (dataLength__); \
    msg__->data = (data__);

#define CsrWifiRouterCtrlWapiRxPktReqSendTo(dst__, src__, interfaceTag__, signalLength__, signal__, dataLength__, data__) \
    { \
        CsrWifiRouterCtrlWapiRxPktReq *msg__; \
        CsrWifiRouterCtrlWapiRxPktReqCreate(msg__, dst__, src__, interfaceTag__, signalLength__, signal__, dataLength__, data__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWapiRxPktReqSend(src__, interfaceTag__, signalLength__, signal__, dataLength__, data__) \
    CsrWifiRouterCtrlWapiRxPktReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, signalLength__, signal__, dataLength__, data__)

/*                                                                              

      
                                             

             

            
                                                                               
                  
                  

                                                                              */
#define CsrWifiRouterCtrlWapiUnicastFilterReqCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWapiUnicastFilterReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WAPI_UNICAST_FILTER_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlWapiUnicastFilterReqSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiRouterCtrlWapiUnicastFilterReq *msg__; \
        CsrWifiRouterCtrlWapiUnicastFilterReqCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWapiUnicastFilterReqSend(src__, interfaceTag__, status__) \
    CsrWifiRouterCtrlWapiUnicastFilterReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, status__)

/*                                                                              

      
                                                

             

            
                                         
                  
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlWapiUnicastTxEncryptIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, dataLength__, data__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWapiUnicastTxEncryptInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WAPI_UNICAST_TX_ENCRYPT_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->dataLength = (dataLength__); \
    msg__->data = (data__);

#define CsrWifiRouterCtrlWapiUnicastTxEncryptIndSendTo(dst__, src__, clientData__, interfaceTag__, dataLength__, data__) \
    { \
        CsrWifiRouterCtrlWapiUnicastTxEncryptInd *msg__; \
        CsrWifiRouterCtrlWapiUnicastTxEncryptIndCreate(msg__, dst__, src__, clientData__, interfaceTag__, dataLength__, data__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWapiUnicastTxEncryptIndSend(dst__, clientData__, interfaceTag__, dataLength__, data__) \
    CsrWifiRouterCtrlWapiUnicastTxEncryptIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, interfaceTag__, dataLength__, data__)

/*                                                                              

      
                                            

             

            
                                                                               
                  
                  
                  

                                                                              */
#define CsrWifiRouterCtrlWapiUnicastTxPktReqCreate(msg__, dst__, src__, interfaceTag__, dataLength__, data__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWapiUnicastTxPktReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WAPI_UNICAST_TX_PKT_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->dataLength = (dataLength__); \
    msg__->data = (data__);

#define CsrWifiRouterCtrlWapiUnicastTxPktReqSendTo(dst__, src__, interfaceTag__, dataLength__, data__) \
    { \
        CsrWifiRouterCtrlWapiUnicastTxPktReq *msg__; \
        CsrWifiRouterCtrlWapiUnicastTxPktReqCreate(msg__, dst__, src__, interfaceTag__, dataLength__, data__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWapiUnicastTxPktReqSend(src__, interfaceTag__, dataLength__, data__) \
    CsrWifiRouterCtrlWapiUnicastTxPktReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, dataLength__, data__)

/*                                                                              

      
                                   

             

            
                                                                             
                

                                                                              */
#define CsrWifiRouterCtrlWifiOffReqCreate(msg__, dst__, src__, clientData__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWifiOffReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WIFI_OFF_REQ, dst__, src__); \
    msg__->clientData = (clientData__);

#define CsrWifiRouterCtrlWifiOffReqSendTo(dst__, src__, clientData__) \
    { \
        CsrWifiRouterCtrlWifiOffReq *msg__; \
        CsrWifiRouterCtrlWifiOffReqCreate(msg__, dst__, src__, clientData__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWifiOffReqSend(src__, clientData__) \
    CsrWifiRouterCtrlWifiOffReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__)

/*                                                                              

      
                                   

             

            
                                              
                       
                       

                                                                              */
#define CsrWifiRouterCtrlWifiOffIndCreate(msg__, dst__, src__, clientData__, controlIndication__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWifiOffInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WIFI_OFF_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->controlIndication = (controlIndication__);

#define CsrWifiRouterCtrlWifiOffIndSendTo(dst__, src__, clientData__, controlIndication__) \
    { \
        CsrWifiRouterCtrlWifiOffInd *msg__; \
        CsrWifiRouterCtrlWifiOffIndCreate(msg__, dst__, src__, clientData__, controlIndication__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWifiOffIndSend(dst__, clientData__, controlIndication__) \
    CsrWifiRouterCtrlWifiOffIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, controlIndication__)

/*                                                                              

      
                                   

             

            
                

                                                                              */
#define CsrWifiRouterCtrlWifiOffResCreate(msg__, dst__, src__, clientData__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWifiOffRes), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WIFI_OFF_RES, dst__, src__); \
    msg__->clientData = (clientData__);

#define CsrWifiRouterCtrlWifiOffResSendTo(dst__, src__, clientData__) \
    { \
        CsrWifiRouterCtrlWifiOffRes *msg__; \
        CsrWifiRouterCtrlWifiOffResCreate(msg__, dst__, src__, clientData__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWifiOffResSend(src__, clientData__) \
    CsrWifiRouterCtrlWifiOffResSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__)

/*                                                                              

      
                                   

             

            
                                       
                

                                                                              */
#define CsrWifiRouterCtrlWifiOffCfmCreate(msg__, dst__, src__, clientData__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWifiOffCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WIFI_OFF_CFM, dst__, src__); \
    msg__->clientData = (clientData__);

#define CsrWifiRouterCtrlWifiOffCfmSendTo(dst__, src__, clientData__) \
    { \
        CsrWifiRouterCtrlWifiOffCfm *msg__; \
        CsrWifiRouterCtrlWifiOffCfmCreate(msg__, dst__, src__, clientData__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWifiOffCfmSend(dst__, clientData__) \
    CsrWifiRouterCtrlWifiOffCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__)

/*                                                                              

      
                                  

             

            
                                                                             
                
                                                                   
                                                                    

                                                                              */
#define CsrWifiRouterCtrlWifiOnReqCreate(msg__, dst__, src__, clientData__, dataLength__, data__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWifiOnReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WIFI_ON_REQ, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->dataLength = (dataLength__); \
    msg__->data = (data__);

#define CsrWifiRouterCtrlWifiOnReqSendTo(dst__, src__, clientData__, dataLength__, data__) \
    { \
        CsrWifiRouterCtrlWifiOnReq *msg__; \
        CsrWifiRouterCtrlWifiOnReqCreate(msg__, dst__, src__, clientData__, dataLength__, data__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWifiOnReqSend(src__, clientData__, dataLength__, data__) \
    CsrWifiRouterCtrlWifiOnReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__, dataLength__, data__)

/*                                                                              

      
                                  

             

            
                                       
                
                
                

                                                                              */
#define CsrWifiRouterCtrlWifiOnIndCreate(msg__, dst__, src__, clientData__, status__, versions__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWifiOnInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WIFI_ON_IND, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->status = (status__); \
    msg__->versions = (versions__);

#define CsrWifiRouterCtrlWifiOnIndSendTo(dst__, src__, clientData__, status__, versions__) \
    { \
        CsrWifiRouterCtrlWifiOnInd *msg__; \
        CsrWifiRouterCtrlWifiOnIndCreate(msg__, dst__, src__, clientData__, status__, versions__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWifiOnIndSend(dst__, clientData__, status__, versions__) \
    CsrWifiRouterCtrlWifiOnIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, status__, versions__)

/*                                                                              

      
                                  

             

            
                         
                         
                         
                                                                         
                         
                         

                                                                              */
#define CsrWifiRouterCtrlWifiOnResCreate(msg__, dst__, src__, clientData__, status__, numInterfaceAddress__, stationMacAddress__, smeVersions__, scheduledInterrupt__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWifiOnRes), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WIFI_ON_RES, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->status = (status__); \
    msg__->numInterfaceAddress = (numInterfaceAddress__); \
    memcpy(msg__->stationMacAddress, (stationMacAddress__), sizeof(CsrWifiMacAddress) * 2); \
    msg__->smeVersions = (smeVersions__); \
    msg__->scheduledInterrupt = (scheduledInterrupt__);

#define CsrWifiRouterCtrlWifiOnResSendTo(dst__, src__, clientData__, status__, numInterfaceAddress__, stationMacAddress__, smeVersions__, scheduledInterrupt__) \
    { \
        CsrWifiRouterCtrlWifiOnRes *msg__; \
        CsrWifiRouterCtrlWifiOnResCreate(msg__, dst__, src__, clientData__, status__, numInterfaceAddress__, stationMacAddress__, smeVersions__, scheduledInterrupt__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWifiOnResSend(src__, clientData__, status__, numInterfaceAddress__, stationMacAddress__, smeVersions__, scheduledInterrupt__) \
    CsrWifiRouterCtrlWifiOnResSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, clientData__, status__, numInterfaceAddress__, stationMacAddress__, smeVersions__, scheduledInterrupt__)

/*                                                                              

      
                                  

             

            
                                       
                
                

                                                                              */
#define CsrWifiRouterCtrlWifiOnCfmCreate(msg__, dst__, src__, clientData__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterCtrlWifiOnCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_CTRL_PRIM, CSR_WIFI_ROUTER_CTRL_WIFI_ON_CFM, dst__, src__); \
    msg__->clientData = (clientData__); \
    msg__->status = (status__);

#define CsrWifiRouterCtrlWifiOnCfmSendTo(dst__, src__, clientData__, status__) \
    { \
        CsrWifiRouterCtrlWifiOnCfm *msg__; \
        CsrWifiRouterCtrlWifiOnCfmCreate(msg__, dst__, src__, clientData__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_CTRL_PRIM, msg__); \
    }

#define CsrWifiRouterCtrlWifiOnCfmSend(dst__, clientData__, status__) \
    CsrWifiRouterCtrlWifiOnCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, clientData__, status__)

#endif /*                              */
