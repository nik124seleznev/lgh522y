/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_ROUTER_LIB_H__
#define CSR_WIFI_ROUTER_LIB_H__

#include "csr_sched.h"
#include "csr_macro.h"
#include "csr_msg_transport.h"

#include "csr_wifi_lib.h"

#include "csr_wifi_router_prim.h"
#include "csr_wifi_router_task.h"

/*                                                                             
                                            
  
               
                                                                                 
                                                                            
  
              
                                                                       
                                                                              */
void CsrWifiRouterFreeUpstreamMessageContents(u16 eventClass, void *message);

/*                                                                             
                                              
  
               
                                                                                   
                                                                              
  
              
                                                                         
                                                                              */
void CsrWifiRouterFreeDownstreamMessageContents(u16 eventClass, void *message);

/*                                                                             
                           
                                                                              */
const char* CsrWifiRouterAppTypeToString(CsrWifiRouterAppType value);
const char* CsrWifiRouterEncapsulationToString(CsrWifiRouterEncapsulation value);
const char* CsrWifiRouterOuiToString(CsrWifiRouterOui value);
const char* CsrWifiRouterPriorityToString(CsrWifiRouterPriority value);


/*                                                                             
                                  
                                                            
                                                                              */
const char* CsrWifiRouterPrimTypeToString(CsrPrim msgType);

/*                                                                             
                                          
                                                                              */
extern const char *CsrWifiRouterUpstreamPrimNames[CSR_WIFI_ROUTER_PRIM_UPSTREAM_COUNT];
extern const char *CsrWifiRouterDownstreamPrimNames[CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_COUNT];

/*                                                                              

      
                                      

             
                                                                       
                             
                                                                         
                                                                             
                             
                                                                 
                             
                                                          
                             

            
                                                                                 
                                                                            
                                                                          
                                                                     
                                                                          
                              

                                                                              */
#define CsrWifiRouterMaPacketCancelReqCreate(msg__, dst__, src__, interfaceTag__, hostTag__, priority__, peerMacAddress__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterMaPacketCancelReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_PRIM, CSR_WIFI_ROUTER_MA_PACKET_CANCEL_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->hostTag = (hostTag__); \
    msg__->priority = (priority__); \
    msg__->peerMacAddress = (peerMacAddress__);

#define CsrWifiRouterMaPacketCancelReqSendTo(dst__, src__, interfaceTag__, hostTag__, priority__, peerMacAddress__) \
    { \
        CsrWifiRouterMaPacketCancelReq *msg__; \
        CsrWifiRouterMaPacketCancelReqCreate(msg__, dst__, src__, interfaceTag__, hostTag__, priority__, peerMacAddress__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_PRIM, msg__); \
    }

#define CsrWifiRouterMaPacketCancelReqSend(src__, interfaceTag__, hostTag__, priority__, peerMacAddress__) \
    CsrWifiRouterMaPacketCancelReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, hostTag__, priority__, peerMacAddress__)

/*                                                                              

      
                                

             
                                                    

            
                                                                                     
                                                                                
                                                       
                                                                
                                                        
                                                                         
                                                                    
                                                                                
                                                                               
                                                                               
                                                                       
                                                                                
                                                                              
                                                                              
                                                                       
                                                                               
                                                           
                                                                              
                                                                               
                                                                              
                                    

                                                                              */
#define CsrWifiRouterMaPacketReqCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__, frameLength__, frame__, freeFunction__, priority__, hostTag__, cfmRequested__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterMaPacketReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_PRIM, CSR_WIFI_ROUTER_MA_PACKET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->subscriptionHandle = (subscriptionHandle__); \
    msg__->frameLength = (frameLength__); \
    msg__->frame = (frame__); \
    msg__->freeFunction = (freeFunction__); \
    msg__->priority = (priority__); \
    msg__->hostTag = (hostTag__); \
    msg__->cfmRequested = (cfmRequested__);

#define CsrWifiRouterMaPacketReqSendTo(dst__, src__, interfaceTag__, subscriptionHandle__, frameLength__, frame__, freeFunction__, priority__, hostTag__, cfmRequested__) \
    { \
        CsrWifiRouterMaPacketReq *msg__; \
        CsrWifiRouterMaPacketReqCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__, frameLength__, frame__, freeFunction__, priority__, hostTag__, cfmRequested__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_PRIM, msg__); \
    }

#define CsrWifiRouterMaPacketReqSend(src__, interfaceTag__, subscriptionHandle__, frameLength__, frame__, freeFunction__, priority__, hostTag__, cfmRequested__) \
    CsrWifiRouterMaPacketReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, subscriptionHandle__, frameLength__, frame__, freeFunction__, priority__, hostTag__, cfmRequested__)

/*                                                                              

      
                                

             
                                                                          
                                    

            
                                               
                                                                                
                                                       
                                                
                                                              
                                                      
                                                                         
                                                                   
                                              
                                                    

                                                                              */
#define CsrWifiRouterMaPacketIndCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__, result__, frameLength__, frame__, freeFunction__, rssi__, snr__, rate__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterMaPacketInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_PRIM, CSR_WIFI_ROUTER_MA_PACKET_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->subscriptionHandle = (subscriptionHandle__); \
    msg__->result = (result__); \
    msg__->frameLength = (frameLength__); \
    msg__->frame = (frame__); \
    msg__->freeFunction = (freeFunction__); \
    msg__->rssi = (rssi__); \
    msg__->snr = (snr__); \
    msg__->rate = (rate__);

#define CsrWifiRouterMaPacketIndSendTo(dst__, src__, interfaceTag__, subscriptionHandle__, result__, frameLength__, frame__, freeFunction__, rssi__, snr__, rate__) \
    { \
        CsrWifiRouterMaPacketInd *msg__; \
        CsrWifiRouterMaPacketIndCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__, result__, frameLength__, frame__, freeFunction__, rssi__, snr__, rate__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_PRIM, msg__); \
    }

#define CsrWifiRouterMaPacketIndSend(dst__, interfaceTag__, subscriptionHandle__, result__, frameLength__, frame__, freeFunction__, rssi__, snr__, rate__) \
    CsrWifiRouterMaPacketIndSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, interfaceTag__, subscriptionHandle__, result__, frameLength__, frame__, freeFunction__, rssi__, snr__, rate__)

/*                                                                              

      
                                

             
                                                                       
          

            
                                                                                
                                                       
                                                

                                                                              */
#define CsrWifiRouterMaPacketResCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__, result__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterMaPacketRes), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_PRIM, CSR_WIFI_ROUTER_MA_PACKET_RES, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->subscriptionHandle = (subscriptionHandle__); \
    msg__->result = (result__);

#define CsrWifiRouterMaPacketResSendTo(dst__, src__, interfaceTag__, subscriptionHandle__, result__) \
    { \
        CsrWifiRouterMaPacketRes *msg__; \
        CsrWifiRouterMaPacketResCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__, result__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_PRIM, msg__); \
    }

#define CsrWifiRouterMaPacketResSend(src__, interfaceTag__, subscriptionHandle__, result__) \
    CsrWifiRouterMaPacketResSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, subscriptionHandle__, result__)

/*                                                                              

      
                                

             
                                                                            
                                                                             

            
                                         
                                                                          
                                          
                                                                           
                                              

                                                                              */
#define CsrWifiRouterMaPacketCfmCreate(msg__, dst__, src__, interfaceTag__, result__, hostTag__, rate__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterMaPacketCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_PRIM, CSR_WIFI_ROUTER_MA_PACKET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->result = (result__); \
    msg__->hostTag = (hostTag__); \
    msg__->rate = (rate__);

#define CsrWifiRouterMaPacketCfmSendTo(dst__, src__, interfaceTag__, result__, hostTag__, rate__) \
    { \
        CsrWifiRouterMaPacketCfm *msg__; \
        CsrWifiRouterMaPacketCfmCreate(msg__, dst__, src__, interfaceTag__, result__, hostTag__, rate__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_PRIM, msg__); \
    }

#define CsrWifiRouterMaPacketCfmSend(dst__, interfaceTag__, result__, hostTag__, rate__) \
    CsrWifiRouterMaPacketCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, interfaceTag__, result__, hostTag__, rate__)

/*                                                                              

      
                                         

             
                                                                            
                                                              
                                                                            
                                                                    
                  

            
                                                                                
                                                                           
                                                                                
                                
                                                                               
                                         
                                                                             
                                

                                                                              */
#define CsrWifiRouterMaPacketSubscribeReqCreate(msg__, dst__, src__, interfaceTag__, encapsulation__, protocol__, oui__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterMaPacketSubscribeReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_PRIM, CSR_WIFI_ROUTER_MA_PACKET_SUBSCRIBE_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->encapsulation = (encapsulation__); \
    msg__->protocol = (protocol__); \
    msg__->oui = (oui__);

#define CsrWifiRouterMaPacketSubscribeReqSendTo(dst__, src__, interfaceTag__, encapsulation__, protocol__, oui__) \
    { \
        CsrWifiRouterMaPacketSubscribeReq *msg__; \
        CsrWifiRouterMaPacketSubscribeReqCreate(msg__, dst__, src__, interfaceTag__, encapsulation__, protocol__, oui__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_PRIM, msg__); \
    }

#define CsrWifiRouterMaPacketSubscribeReqSend(src__, interfaceTag__, encapsulation__, protocol__, oui__) \
    CsrWifiRouterMaPacketSubscribeReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, encapsulation__, protocol__, oui__)

/*                                                                              

      
                                         

             
                                                                
                 

            
                                               
                                                                                
                                                   
                                                                            
                                                
                                                                              

                                                                              */
#define CsrWifiRouterMaPacketSubscribeCfmCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__, status__, allocOffset__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterMaPacketSubscribeCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_PRIM, CSR_WIFI_ROUTER_MA_PACKET_SUBSCRIBE_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->subscriptionHandle = (subscriptionHandle__); \
    msg__->status = (status__); \
    msg__->allocOffset = (allocOffset__);

#define CsrWifiRouterMaPacketSubscribeCfmSendTo(dst__, src__, interfaceTag__, subscriptionHandle__, status__, allocOffset__) \
    { \
        CsrWifiRouterMaPacketSubscribeCfm *msg__; \
        CsrWifiRouterMaPacketSubscribeCfmCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__, status__, allocOffset__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_PRIM, msg__); \
    }

#define CsrWifiRouterMaPacketSubscribeCfmSend(dst__, interfaceTag__, subscriptionHandle__, status__, allocOffset__) \
    CsrWifiRouterMaPacketSubscribeCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, interfaceTag__, subscriptionHandle__, status__, allocOffset__)

/*                                                                              

      
                                           

             
                                                             

            
                                                                                     
                                                                                
                                                       

                                                                              */
#define CsrWifiRouterMaPacketUnsubscribeReqCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterMaPacketUnsubscribeReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_PRIM, CSR_WIFI_ROUTER_MA_PACKET_UNSUBSCRIBE_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->subscriptionHandle = (subscriptionHandle__);

#define CsrWifiRouterMaPacketUnsubscribeReqSendTo(dst__, src__, interfaceTag__, subscriptionHandle__) \
    { \
        CsrWifiRouterMaPacketUnsubscribeReq *msg__; \
        CsrWifiRouterMaPacketUnsubscribeReqCreate(msg__, dst__, src__, interfaceTag__, subscriptionHandle__); \
        CsrMsgTransport(dst__, CSR_WIFI_ROUTER_PRIM, msg__); \
    }

#define CsrWifiRouterMaPacketUnsubscribeReqSend(src__, interfaceTag__, subscriptionHandle__) \
    CsrWifiRouterMaPacketUnsubscribeReqSendTo(CSR_WIFI_ROUTER_IFACEQUEUE, src__, interfaceTag__, subscriptionHandle__)

/*                                                                              

      
                                           

             
                                                                
                   

            
                                         
                                                                          
                                          

                                                                              */
#define CsrWifiRouterMaPacketUnsubscribeCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiRouterMaPacketUnsubscribeCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_ROUTER_PRIM, CSR_WIFI_ROUTER_MA_PACKET_UNSUBSCRIBE_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiRouterMaPacketUnsubscribeCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiRouterMaPacketUnsubscribeCfm *msg__; \
        CsrWifiRouterMaPacketUnsubscribeCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_ROUTER_PRIM, msg__); \
    }

#define CsrWifiRouterMaPacketUnsubscribeCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiRouterMaPacketUnsubscribeCfmSendTo(dst__, CSR_WIFI_ROUTER_IFACEQUEUE, interfaceTag__, status__)

#endif /*                         */
