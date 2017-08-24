/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_NME_AP_LIB_H__
#define CSR_WIFI_NME_AP_LIB_H__

#include "csr_sched.h"
#include "csr_macro.h"
#include "csr_msg_transport.h"

#include "csr_wifi_lib.h"

#include "csr_wifi_nme_ap_prim.h"
#include "csr_wifi_nme_task.h"

#ifndef CSR_WIFI_NME_ENABLE
#error CSR_WIFI_NME_ENABLE MUST be defined inorder to use csr_wifi_nme_ap_lib.h
#endif
#ifndef CSR_WIFI_AP_ENABLE
#error CSR_WIFI_AP_ENABLE MUST be defined inorder to use csr_wifi_nme_ap_lib.h
#endif

/*                                                                             
                                           
  
               
                                                                                 
                                                                            
  
              
                                                                       
                                                                              */
void CsrWifiNmeApFreeUpstreamMessageContents(u16 eventClass, void *message);

/*                                                                             
                                             
  
               
                                                                                   
                                                                              
  
              
                                                                         
                                                                              */
void CsrWifiNmeApFreeDownstreamMessageContents(u16 eventClass, void *message);

/*                                                                              

      
                                

             
                                                                            
                                                                      
                                               

            
                                                                              
                                               
                                                       
                                                        

                                                                              */
#define CsrWifiNmeApConfigSetReqCreate(msg__, dst__, src__, apConfig__, apMacConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_CONFIG_SET_REQ, dst__, src__); \
    msg__->apConfig = (apConfig__); \
    msg__->apMacConfig = (apMacConfig__);

#define CsrWifiNmeApConfigSetReqSendTo(dst__, src__, apConfig__, apMacConfig__) \
    { \
        CsrWifiNmeApConfigSetReq *msg__; \
        CsrWifiNmeApConfigSetReqCreate(msg__, dst__, src__, apConfig__, apMacConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApConfigSetReqSend(src__, apConfig__, apMacConfig__) \
    CsrWifiNmeApConfigSetReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, apConfig__, apMacConfig__)

/*                                                                              

      
                                

             
                                                     

            
                                   
                                   

                                                                              */
#define CsrWifiNmeApConfigSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_CONFIG_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiNmeApConfigSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiNmeApConfigSetCfm *msg__; \
        CsrWifiNmeApConfigSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApConfigSetCfmSend(dst__, status__) \
    CsrWifiNmeApConfigSetCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, status__)

/*                                                                              

      
                                

             
                                                                             
                                                                      
                                                                        
                                            

            
                                                                                
                                                                           
                                                                            
                                                                                
                                                                               
                                                         

                                                                              */
#define CsrWifiNmeApStaRemoveReqCreate(msg__, dst__, src__, interfaceTag__, staMacAddress__, keepBlocking__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApStaRemoveReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_STA_REMOVE_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->staMacAddress = (staMacAddress__); \
    msg__->keepBlocking = (keepBlocking__);

#define CsrWifiNmeApStaRemoveReqSendTo(dst__, src__, interfaceTag__, staMacAddress__, keepBlocking__) \
    { \
        CsrWifiNmeApStaRemoveReq *msg__; \
        CsrWifiNmeApStaRemoveReqCreate(msg__, dst__, src__, interfaceTag__, staMacAddress__, keepBlocking__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApStaRemoveReqSend(src__, interfaceTag__, staMacAddress__, keepBlocking__) \
    CsrWifiNmeApStaRemoveReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, interfaceTag__, staMacAddress__, keepBlocking__)

/*                                                                              

      
                            

             
                                                            

            
                                                                                 
                                                                            
                                                                        
                                                                             
                                                    
                                           
                                    
                                                         
                                                       
                                                                   
                                                
                                                                   

                                                                              */
#define CsrWifiNmeApStartReqCreate(msg__, dst__, src__, interfaceTag__, apType__, cloakSsid__, ssid__, ifIndex__, channel__, apCredentials__, maxConnections__, p2pGoParam__, wpsEnabled__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApStartReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_START_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->apType = (apType__); \
    msg__->cloakSsid = (cloakSsid__); \
    msg__->ssid = (ssid__); \
    msg__->ifIndex = (ifIndex__); \
    msg__->channel = (channel__); \
    msg__->apCredentials = (apCredentials__); \
    msg__->maxConnections = (maxConnections__); \
    msg__->p2pGoParam = (p2pGoParam__); \
    msg__->wpsEnabled = (wpsEnabled__);

#define CsrWifiNmeApStartReqSendTo(dst__, src__, interfaceTag__, apType__, cloakSsid__, ssid__, ifIndex__, channel__, apCredentials__, maxConnections__, p2pGoParam__, wpsEnabled__) \
    { \
        CsrWifiNmeApStartReq *msg__; \
        CsrWifiNmeApStartReqCreate(msg__, dst__, src__, interfaceTag__, apType__, cloakSsid__, ssid__, ifIndex__, channel__, apCredentials__, maxConnections__, p2pGoParam__, wpsEnabled__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApStartReqSend(src__, interfaceTag__, apType__, cloakSsid__, ssid__, ifIndex__, channel__, apCredentials__, maxConnections__, p2pGoParam__, wpsEnabled__) \
    CsrWifiNmeApStartReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, interfaceTag__, apType__, cloakSsid__, ssid__, ifIndex__, channel__, apCredentials__, maxConnections__, p2pGoParam__, wpsEnabled__)

/*                                                                              

      
                            

             
                                                                       

            
                                         
                                                                          
                                         
                                         

                                                                              */
#define CsrWifiNmeApStartCfmCreate(msg__, dst__, src__, interfaceTag__, status__, ssid__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApStartCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_START_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->ssid = (ssid__);

#define CsrWifiNmeApStartCfmSendTo(dst__, src__, interfaceTag__, status__, ssid__) \
    { \
        CsrWifiNmeApStartCfm *msg__; \
        CsrWifiNmeApStartCfmCreate(msg__, dst__, src__, interfaceTag__, status__, ssid__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApStartCfmSend(dst__, interfaceTag__, status__, ssid__) \
    CsrWifiNmeApStartCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, status__, ssid__)

/*                                                                              

      
                              

             
                                                                             
                                  

            
                                              
                                                                               
                                                                     
                                    
                                                  
                                          

                                                                              */
#define CsrWifiNmeApStationIndCreate(msg__, dst__, src__, interfaceTag__, mediaStatus__, peerMacAddress__, peerDeviceAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApStationInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_STATION_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->mediaStatus = (mediaStatus__); \
    msg__->peerMacAddress = (peerMacAddress__); \
    msg__->peerDeviceAddress = (peerDeviceAddress__);

#define CsrWifiNmeApStationIndSendTo(dst__, src__, interfaceTag__, mediaStatus__, peerMacAddress__, peerDeviceAddress__) \
    { \
        CsrWifiNmeApStationInd *msg__; \
        CsrWifiNmeApStationIndCreate(msg__, dst__, src__, interfaceTag__, mediaStatus__, peerMacAddress__, peerDeviceAddress__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApStationIndSend(dst__, interfaceTag__, mediaStatus__, peerMacAddress__, peerDeviceAddress__) \
    CsrWifiNmeApStationIndSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, mediaStatus__, peerMacAddress__, peerDeviceAddress__)

/*                                                                              

      
                           

             
                                                         

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiNmeApStopReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApStopReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_STOP_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiNmeApStopReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiNmeApStopReq *msg__; \
        CsrWifiNmeApStopReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApStopReqSend(src__, interfaceTag__) \
    CsrWifiNmeApStopReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, interfaceTag__)

/*                                                                              

      
                           

             
                                                                         
                                                                            
                                                                           

            
                                         
                                                                          
                                                
                               

                                                                              */
#define CsrWifiNmeApStopIndCreate(msg__, dst__, src__, interfaceTag__, apType__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApStopInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_STOP_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->apType = (apType__); \
    msg__->status = (status__);

#define CsrWifiNmeApStopIndSendTo(dst__, src__, interfaceTag__, apType__, status__) \
    { \
        CsrWifiNmeApStopInd *msg__; \
        CsrWifiNmeApStopIndCreate(msg__, dst__, src__, interfaceTag__, apType__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApStopIndSend(dst__, interfaceTag__, apType__, status__) \
    CsrWifiNmeApStopIndSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, apType__, status__)

/*                                                                              

      
                           

             
                                                                            
                                                                      
                        

            
                                         
                                                                          
                                         

                                                                              */
#define CsrWifiNmeApStopCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApStopCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_STOP_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiNmeApStopCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiNmeApStopCfm *msg__; \
        CsrWifiNmeApStopCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApStopCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiNmeApStopCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                                     

             
                                                                

            
                                                                                
                                                                              
                                                
                                                                               
                                                                     
                                          

                                                                              */
#define CsrWifiNmeApWmmParamUpdateReqCreate(msg__, dst__, src__, wmmApParams__, wmmApBcParams__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApWmmParamUpdateReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_WMM_PARAM_UPDATE_REQ, dst__, src__); \
    memcpy(msg__->wmmApParams, (wmmApParams__), sizeof(CsrWifiSmeWmmAcParams) * 4); \
    memcpy(msg__->wmmApBcParams, (wmmApBcParams__), sizeof(CsrWifiSmeWmmAcParams) * 4);

#define CsrWifiNmeApWmmParamUpdateReqSendTo(dst__, src__, wmmApParams__, wmmApBcParams__) \
    { \
        CsrWifiNmeApWmmParamUpdateReq *msg__; \
        CsrWifiNmeApWmmParamUpdateReqCreate(msg__, dst__, src__, wmmApParams__, wmmApBcParams__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApWmmParamUpdateReqSend(src__, wmmApParams__, wmmApBcParams__) \
    CsrWifiNmeApWmmParamUpdateReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, wmmApParams__, wmmApBcParams__)

/*                                                                              

      
                                     

             
                                               

            
                                   
                                   

                                                                              */
#define CsrWifiNmeApWmmParamUpdateCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApWmmParamUpdateCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_WMM_PARAM_UPDATE_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiNmeApWmmParamUpdateCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiNmeApWmmParamUpdateCfm *msg__; \
        CsrWifiNmeApWmmParamUpdateCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApWmmParamUpdateCfmSend(dst__, status__) \
    CsrWifiNmeApWmmParamUpdateCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, status__)

/*                                                                              

      
                                  

             
                                                                        
                                                                     
                                    

            
                                                                                           
                                                                            
                                        
                                                     
                                                                     
                                         
                                                                               
                                                                               
                                                                             
                                        

                                                                              */
#define CsrWifiNmeApWpsRegisterReqCreate(msg__, dst__, src__, interfaceTag__, selectedDevicePasswordId__, selectedConfigMethod__, pin__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApWpsRegisterReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_WPS_REGISTER_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->selectedDevicePasswordId = (selectedDevicePasswordId__); \
    msg__->selectedConfigMethod = (selectedConfigMethod__); \
    memcpy(msg__->pin, (pin__), sizeof(u8) * 8);

#define CsrWifiNmeApWpsRegisterReqSendTo(dst__, src__, interfaceTag__, selectedDevicePasswordId__, selectedConfigMethod__, pin__) \
    { \
        CsrWifiNmeApWpsRegisterReq *msg__; \
        CsrWifiNmeApWpsRegisterReqCreate(msg__, dst__, src__, interfaceTag__, selectedDevicePasswordId__, selectedConfigMethod__, pin__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApWpsRegisterReqSend(src__, interfaceTag__, selectedDevicePasswordId__, selectedConfigMethod__, pin__) \
    CsrWifiNmeApWpsRegisterReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, interfaceTag__, selectedDevicePasswordId__, selectedConfigMethod__, pin__)

/*                                                                              

      
                                  

             
                                                       

            
                                         
                                                                          
                                         

                                                                              */
#define CsrWifiNmeApWpsRegisterCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeApWpsRegisterCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_AP_PRIM, CSR_WIFI_NME_AP_WPS_REGISTER_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiNmeApWpsRegisterCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiNmeApWpsRegisterCfm *msg__; \
        CsrWifiNmeApWpsRegisterCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_AP_PRIM, msg__); \
    }

#define CsrWifiNmeApWpsRegisterCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiNmeApWpsRegisterCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, status__)

#endif /*                         */
