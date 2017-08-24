/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_NME_LIB_H__
#define CSR_WIFI_NME_LIB_H__

#include "csr_sched.h"
#include "csr_macro.h"
#include "csr_msg_transport.h"

#include "csr_wifi_lib.h"

#include "csr_wifi_nme_prim.h"
#include "csr_wifi_nme_task.h"


#ifndef CSR_WIFI_NME_ENABLE
#error CSR_WIFI_NME_ENABLE MUST be defined inorder to use csr_wifi_nme_lib.h
#endif

/*                                                                              

      
                                        

             
                                                      

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiNmeConnectionStatusGetReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeConnectionStatusGetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_CONNECTION_STATUS_GET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiNmeConnectionStatusGetReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiNmeConnectionStatusGetReq *msg__; \
        CsrWifiNmeConnectionStatusGetReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeConnectionStatusGetReqSend(src__, interfaceTag__) \
    CsrWifiNmeConnectionStatusGetReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, interfaceTag__)

/*                                                                              

      
                                        

             
                                             

            
                                             
                                                                              
                                                                          
                                 
                                                    

                                                                              */
#define CsrWifiNmeConnectionStatusGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectionStatus__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeConnectionStatusGetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_CONNECTION_STATUS_GET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->connectionStatus = (connectionStatus__);

#define CsrWifiNmeConnectionStatusGetCfmSendTo(dst__, src__, interfaceTag__, status__, connectionStatus__) \
    { \
        CsrWifiNmeConnectionStatusGetCfm *msg__; \
        CsrWifiNmeConnectionStatusGetCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectionStatus__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeConnectionStatusGetCfmSend(dst__, interfaceTag__, status__, connectionStatus__) \
    CsrWifiNmeConnectionStatusGetCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, status__, connectionStatus__)

/*                                                                              

      
                                 

             
                                                                         
                                                                            
                                                                       
                                                                         

            
                                                                          
                                                             

                                                                              */
#define CsrWifiNmeEventMaskSetReqCreate(msg__, dst__, src__, indMask__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeEventMaskSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_EVENT_MASK_SET_REQ, dst__, src__); \
    msg__->indMask = (indMask__);

#define CsrWifiNmeEventMaskSetReqSendTo(dst__, src__, indMask__) \
    { \
        CsrWifiNmeEventMaskSetReq *msg__; \
        CsrWifiNmeEventMaskSetReqCreate(msg__, dst__, src__, indMask__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeEventMaskSetReqSend(src__, indMask__) \
    CsrWifiNmeEventMaskSetReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, indMask__)

/*                                                                              

      
                                 

             
                                                                   
              

            
                                   
                                              

                                                                              */
#define CsrWifiNmeEventMaskSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeEventMaskSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_EVENT_MASK_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiNmeEventMaskSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiNmeEventMaskSetCfm *msg__; \
        CsrWifiNmeEventMaskSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeEventMaskSetCfmSend(dst__, status__) \
    CsrWifiNmeEventMaskSetCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, status__)

/*                                                                              

      
                                   

             
                                                                    
                                             

            
                                                                                  
                                                                             
                                                                           
                                                                   

                                                                              */
#define CsrWifiNmeProfileConnectReqCreate(msg__, dst__, src__, interfaceTag__, profileIdentity__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileConnectReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_CONNECT_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->profileIdentity = (profileIdentity__);

#define CsrWifiNmeProfileConnectReqSendTo(dst__, src__, interfaceTag__, profileIdentity__) \
    { \
        CsrWifiNmeProfileConnectReq *msg__; \
        CsrWifiNmeProfileConnectReqCreate(msg__, dst__, src__, interfaceTag__, profileIdentity__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileConnectReqSend(src__, interfaceTag__, profileIdentity__) \
    CsrWifiNmeProfileConnectReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, interfaceTag__, profileIdentity__)

/*                                                                              

      
                                   

             
                                                                          
                                                                     
                                                                        

            
                                                 
                                                                        
                                    
                                                                              
                                     
                                                             
                                                               
                                                                              
                                         
                                                             
                                                               
                                                                            
                                                       
                                                                              
                                                                             
                                                                      

                                                                              */
#define CsrWifiNmeProfileConnectCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectAttemptsCount__, connectAttempts__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileConnectCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_CONNECT_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->connectAttemptsCount = (connectAttemptsCount__); \
    msg__->connectAttempts = (connectAttempts__);

#define CsrWifiNmeProfileConnectCfmSendTo(dst__, src__, interfaceTag__, status__, connectAttemptsCount__, connectAttempts__) \
    { \
        CsrWifiNmeProfileConnectCfm *msg__; \
        CsrWifiNmeProfileConnectCfmCreate(msg__, dst__, src__, interfaceTag__, status__, connectAttemptsCount__, connectAttempts__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileConnectCfmSend(dst__, interfaceTag__, status__, connectAttemptsCount__, connectAttempts__) \
    CsrWifiNmeProfileConnectCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, status__, connectAttemptsCount__, connectAttempts__)

/*                                                                              

      
                                     

             
                                                                    
                           

            
                                                                         

                                                                              */
#define CsrWifiNmeProfileDeleteAllReqCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileDeleteAllReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_DELETE_ALL_REQ, dst__, src__);

#define CsrWifiNmeProfileDeleteAllReqSendTo(dst__, src__) \
    { \
        CsrWifiNmeProfileDeleteAllReq *msg__; \
        CsrWifiNmeProfileDeleteAllReqCreate(msg__, dst__, src__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileDeleteAllReqSend(src__) \
    CsrWifiNmeProfileDeleteAllReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__)

/*                                                                              

      
                                     

             
                                                                  
                                               

            
                                   
                                                                               
                                                   

                                                                              */
#define CsrWifiNmeProfileDeleteAllCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileDeleteAllCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_DELETE_ALL_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiNmeProfileDeleteAllCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiNmeProfileDeleteAllCfm *msg__; \
        CsrWifiNmeProfileDeleteAllCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileDeleteAllCfmSend(dst__, status__) \
    CsrWifiNmeProfileDeleteAllCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, status__)

/*                                                                              

      
                                  

             
                                                                             
                           

            
                                                                                  
                                                                      

                                                                              */
#define CsrWifiNmeProfileDeleteReqCreate(msg__, dst__, src__, profileIdentity__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileDeleteReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_DELETE_REQ, dst__, src__); \
    msg__->profileIdentity = (profileIdentity__);

#define CsrWifiNmeProfileDeleteReqSendTo(dst__, src__, profileIdentity__) \
    { \
        CsrWifiNmeProfileDeleteReq *msg__; \
        CsrWifiNmeProfileDeleteReqCreate(msg__, dst__, src__, profileIdentity__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileDeleteReqSend(src__, profileIdentity__) \
    CsrWifiNmeProfileDeleteReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, profileIdentity__)

/*                                                                              

      
                                  

             
                                                              
                                                                          

            
                                   
                                                                           

                                                                              */
#define CsrWifiNmeProfileDeleteCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileDeleteCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_DELETE_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiNmeProfileDeleteCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiNmeProfileDeleteCfm *msg__; \
        CsrWifiNmeProfileDeleteCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileDeleteCfmSend(dst__, status__) \
    CsrWifiNmeProfileDeleteCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, status__)

/*                                                                              

      
                                      

             
                                                                      
                                                                      
                                                                        
                                                                          
                           

            
                                                 
                                                                        
                                    
                                                                              
                                         
                                                                            
                                                       
                                                                              
                                                                             
                                                                 

                                                                              */
#define CsrWifiNmeProfileDisconnectIndCreate(msg__, dst__, src__, interfaceTag__, connectAttemptsCount__, connectAttempts__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileDisconnectInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_DISCONNECT_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->connectAttemptsCount = (connectAttemptsCount__); \
    msg__->connectAttempts = (connectAttempts__);

#define CsrWifiNmeProfileDisconnectIndSendTo(dst__, src__, interfaceTag__, connectAttemptsCount__, connectAttempts__) \
    { \
        CsrWifiNmeProfileDisconnectInd *msg__; \
        CsrWifiNmeProfileDisconnectIndCreate(msg__, dst__, src__, interfaceTag__, connectAttemptsCount__, connectAttempts__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileDisconnectIndSend(dst__, interfaceTag__, connectAttemptsCount__, connectAttempts__) \
    CsrWifiNmeProfileDisconnectIndSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, connectAttemptsCount__, connectAttempts__)

/*                                                                              

      
                                    

             
                                                                          
                                               
                                                                            
                                                                           
              
                                                                            
                                                  
                                                                           
                                                                         
                                                                   

            
                                                                                        
                                                                         
                                     
                                                                          
                                                                     

                                                                              */
#define CsrWifiNmeProfileOrderSetReqCreate(msg__, dst__, src__, interfaceTag__, profileIdentitysCount__, profileIdentitys__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileOrderSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_ORDER_SET_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->profileIdentitysCount = (profileIdentitysCount__); \
    msg__->profileIdentitys = (profileIdentitys__);

#define CsrWifiNmeProfileOrderSetReqSendTo(dst__, src__, interfaceTag__, profileIdentitysCount__, profileIdentitys__) \
    { \
        CsrWifiNmeProfileOrderSetReq *msg__; \
        CsrWifiNmeProfileOrderSetReqCreate(msg__, dst__, src__, interfaceTag__, profileIdentitysCount__, profileIdentitys__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileOrderSetReqSend(src__, interfaceTag__, profileIdentitysCount__, profileIdentitys__) \
    CsrWifiNmeProfileOrderSetReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, interfaceTag__, profileIdentitysCount__, profileIdentitys__)

/*                                                                              

      
                                    

             
                                                        

            
                                         
                                                                          
                                                                      
                             

                                                                              */
#define CsrWifiNmeProfileOrderSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileOrderSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_ORDER_SET_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiNmeProfileOrderSetCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiNmeProfileOrderSetCfm *msg__; \
        CsrWifiNmeProfileOrderSetCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileOrderSetCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiNmeProfileOrderSetCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                               

             
                                                                             
                                                                             
                                                                         
                                                                            
                                                                             
                                                                

            
                                                                          
                                                                    

                                                                              */
#define CsrWifiNmeProfileSetReqCreate(msg__, dst__, src__, profile__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_SET_REQ, dst__, src__); \
    msg__->profile = (profile__);

#define CsrWifiNmeProfileSetReqSendTo(dst__, src__, profile__) \
    { \
        CsrWifiNmeProfileSetReq *msg__; \
        CsrWifiNmeProfileSetReqCreate(msg__, dst__, src__, profile__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileSetReqSend(src__, profile__) \
    CsrWifiNmeProfileSetReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, profile__)

/*                                                                              

      
                               

             
                                                                             
                                                                           
                                                                       
                                                                          
                                                                           
                              

            
                                   
                                                                           

                                                                              */
#define CsrWifiNmeProfileSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiNmeProfileSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiNmeProfileSetCfm *msg__; \
        CsrWifiNmeProfileSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileSetCfmSend(dst__, status__) \
    CsrWifiNmeProfileSetCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, status__)

/*                                                                              

      
                                  

             
                                                                      
                                                                            
            
                                                                             
                                   
                                                                         
        

            
                                         
                                                                          
                                                               

                                                                              */
#define CsrWifiNmeProfileUpdateIndCreate(msg__, dst__, src__, interfaceTag__, profile__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeProfileUpdateInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_PROFILE_UPDATE_IND, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->profile = (profile__);

#define CsrWifiNmeProfileUpdateIndSendTo(dst__, src__, interfaceTag__, profile__) \
    { \
        CsrWifiNmeProfileUpdateInd *msg__; \
        CsrWifiNmeProfileUpdateIndCreate(msg__, dst__, src__, interfaceTag__, profile__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeProfileUpdateIndSend(dst__, interfaceTag__, profile__) \
    CsrWifiNmeProfileUpdateIndSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, profile__)

/*                                                                              

      
                               

             
                                                                      
                                                               
                                                                          
                                                                            
                                                                       
                                                                           
                                                                           
                                                                         
                             

            
                                        
                                                                               
                                  
                                      

                                                                              */
#define CsrWifiNmeSimGsmAuthIndCreate(msg__, dst__, src__, randsLength__, rands__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeSimGsmAuthInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_SIM_GSM_AUTH_IND, dst__, src__); \
    msg__->randsLength = (randsLength__); \
    msg__->rands = (rands__);

#define CsrWifiNmeSimGsmAuthIndSendTo(dst__, src__, randsLength__, rands__) \
    { \
        CsrWifiNmeSimGsmAuthInd *msg__; \
        CsrWifiNmeSimGsmAuthIndCreate(msg__, dst__, src__, randsLength__, rands__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeSimGsmAuthIndSend(dst__, randsLength__, rands__) \
    CsrWifiNmeSimGsmAuthIndSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, randsLength__, rands__)

/*                                                                              

      
                               

             
                                                                             
                                                                       
                                                                            
                                                                           
                                                                      
                                                                             
                                                                          
               

            
                                                                  
                                               
                                                                          
                                                    
                                                                           
                                                        

                                                                              */
#define CsrWifiNmeSimGsmAuthResCreate(msg__, dst__, src__, status__, kcsLength__, kcs__, sresLength__, sres__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeSimGsmAuthRes), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_SIM_GSM_AUTH_RES, dst__, src__); \
    msg__->status = (status__); \
    msg__->kcsLength = (kcsLength__); \
    msg__->kcs = (kcs__); \
    msg__->sresLength = (sresLength__); \
    msg__->sres = (sres__);

#define CsrWifiNmeSimGsmAuthResSendTo(dst__, src__, status__, kcsLength__, kcs__, sresLength__, sres__) \
    { \
        CsrWifiNmeSimGsmAuthRes *msg__; \
        CsrWifiNmeSimGsmAuthResCreate(msg__, dst__, src__, status__, kcsLength__, kcs__, sresLength__, sres__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeSimGsmAuthResSend(src__, status__, kcsLength__, kcs__, sresLength__, sres__) \
    CsrWifiNmeSimGsmAuthResSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, status__, kcsLength__, kcs__, sresLength__, sres__)

/*                                                                              

      
                               

             
                                                                      
                                                                           
                                                                           
                                                                       
                                                                             
                                                                            
         

            
                                   

                                                                              */
#define CsrWifiNmeSimImsiGetIndCreate(msg__, dst__, src__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeSimImsiGetInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_SIM_IMSI_GET_IND, dst__, src__);

#define CsrWifiNmeSimImsiGetIndSendTo(dst__, src__) \
    { \
        CsrWifiNmeSimImsiGetInd *msg__; \
        CsrWifiNmeSimImsiGetIndCreate(msg__, dst__, src__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeSimImsiGetIndSend(dst__) \
    CsrWifiNmeSimImsiGetIndSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE)

/*                                                                              

      
                               

             
                                                                         

            
                                                                               
                               
                                                            
                                                                      

                                                                              */
#define CsrWifiNmeSimImsiGetResCreate(msg__, dst__, src__, status__, imsi__, cardType__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeSimImsiGetRes), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_SIM_IMSI_GET_RES, dst__, src__); \
    msg__->status = (status__); \
    msg__->imsi = (imsi__); \
    msg__->cardType = (cardType__);

#define CsrWifiNmeSimImsiGetResSendTo(dst__, src__, status__, imsi__, cardType__) \
    { \
        CsrWifiNmeSimImsiGetRes *msg__; \
        CsrWifiNmeSimImsiGetResCreate(msg__, dst__, src__, status__, imsi__, cardType__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeSimImsiGetResSend(src__, status__, imsi__, cardType__) \
    CsrWifiNmeSimImsiGetResSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, status__, imsi__, cardType__)

/*                                                                              

      
                                

             
                                                                      
                                                                
                                                                          
                                                                            
                                                                       
                                                                           
                                                                   
                                                                            
             

            
                                   
                             
                             

                                                                              */
#define CsrWifiNmeSimUmtsAuthIndCreate(msg__, dst__, src__, rand__, autn__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeSimUmtsAuthInd), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_SIM_UMTS_AUTH_IND, dst__, src__); \
    memcpy(msg__->rand, (rand__), sizeof(u8) * 16); \
    memcpy(msg__->autn, (autn__), sizeof(u8) * 16);

#define CsrWifiNmeSimUmtsAuthIndSendTo(dst__, src__, rand__, autn__) \
    { \
        CsrWifiNmeSimUmtsAuthInd *msg__; \
        CsrWifiNmeSimUmtsAuthIndCreate(msg__, dst__, src__, rand__, autn__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeSimUmtsAuthIndSend(dst__, rand__, autn__) \
    CsrWifiNmeSimUmtsAuthIndSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, rand__, autn__)

/*                                                                              

      
                                

             
                                                                          
                                                                         
                                                                           
                                             
                                      

            
                                                                          
                                                        
                                                                              
                                                                               
                                                                              
                                                    
                                                                              
                              
                                                                             
                                  
                                                                               
                               
                                                                            
                              
                                                                             
                              

                                                                              */
#define CsrWifiNmeSimUmtsAuthResCreate(msg__, dst__, src__, status__, result__, umtsCipherKey__, umtsIntegrityKey__, resParameterLength__, resParameter__, auts__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeSimUmtsAuthRes), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_SIM_UMTS_AUTH_RES, dst__, src__); \
    msg__->status = (status__); \
    msg__->result = (result__); \
    memcpy(msg__->umtsCipherKey, (umtsCipherKey__), sizeof(u8) * 16); \
    memcpy(msg__->umtsIntegrityKey, (umtsIntegrityKey__), sizeof(u8) * 16); \
    msg__->resParameterLength = (resParameterLength__); \
    msg__->resParameter = (resParameter__); \
    memcpy(msg__->auts, (auts__), sizeof(u8) * 14);

#define CsrWifiNmeSimUmtsAuthResSendTo(dst__, src__, status__, result__, umtsCipherKey__, umtsIntegrityKey__, resParameterLength__, resParameter__, auts__) \
    { \
        CsrWifiNmeSimUmtsAuthRes *msg__; \
        CsrWifiNmeSimUmtsAuthResCreate(msg__, dst__, src__, status__, result__, umtsCipherKey__, umtsIntegrityKey__, resParameterLength__, resParameter__, auts__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeSimUmtsAuthResSend(src__, status__, result__, umtsCipherKey__, umtsIntegrityKey__, resParameterLength__, resParameter__, auts__) \
    CsrWifiNmeSimUmtsAuthResSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, status__, result__, umtsCipherKey__, umtsIntegrityKey__, resParameterLength__, resParameter__, auts__)

/*                                                                              

      
                              

             
                                                                     
                                                                         
                                    

            
                                                                               
                                                                          

                                                                              */
#define CsrWifiNmeWpsCancelReqCreate(msg__, dst__, src__, interfaceTag__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeWpsCancelReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_WPS_CANCEL_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__);

#define CsrWifiNmeWpsCancelReqSendTo(dst__, src__, interfaceTag__) \
    { \
        CsrWifiNmeWpsCancelReq *msg__; \
        CsrWifiNmeWpsCancelReqCreate(msg__, dst__, src__, interfaceTag__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeWpsCancelReqSend(src__, interfaceTag__) \
    CsrWifiNmeWpsCancelReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, interfaceTag__)

/*                                                                              

      
                              

             
                                                                            

            
                                         
                                                                          
                                                           

                                                                              */
#define CsrWifiNmeWpsCancelCfmCreate(msg__, dst__, src__, interfaceTag__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeWpsCancelCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_WPS_CANCEL_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__);

#define CsrWifiNmeWpsCancelCfmSendTo(dst__, src__, interfaceTag__, status__) \
    { \
        CsrWifiNmeWpsCancelCfm *msg__; \
        CsrWifiNmeWpsCancelCfmCreate(msg__, dst__, src__, interfaceTag__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeWpsCancelCfmSend(dst__, interfaceTag__, status__) \
    CsrWifiNmeWpsCancelCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, status__)

/*                                                                              

      
                        

             
                                          
                                                                      
                                       

            
                                         
                                                                          
                                                                      
                             
                                                     
                                                       
                                                               

                                                                              */
#define CsrWifiNmeWpsCfmCreate(msg__, dst__, src__, interfaceTag__, status__, profile__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeWpsCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_WPS_CFM, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    msg__->status = (status__); \
    msg__->profile = (profile__);

#define CsrWifiNmeWpsCfmSendTo(dst__, src__, interfaceTag__, status__, profile__) \
    { \
        CsrWifiNmeWpsCfm *msg__; \
        CsrWifiNmeWpsCfmCreate(msg__, dst__, src__, interfaceTag__, status__, profile__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeWpsCfmSend(dst__, interfaceTag__, status__, profile__) \
    CsrWifiNmeWpsCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, interfaceTag__, status__, profile__)

/*                                                                              

      
                                 

             
                                                                             
                                               

            
                                                                            
                           

                                                                              */
#define CsrWifiNmeWpsConfigSetReqCreate(msg__, dst__, src__, wpsConfig__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeWpsConfigSetReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_WPS_CONFIG_SET_REQ, dst__, src__); \
    msg__->wpsConfig = (wpsConfig__);

#define CsrWifiNmeWpsConfigSetReqSendTo(dst__, src__, wpsConfig__) \
    { \
        CsrWifiNmeWpsConfigSetReq *msg__; \
        CsrWifiNmeWpsConfigSetReqCreate(msg__, dst__, src__, wpsConfig__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeWpsConfigSetReqSend(src__, wpsConfig__) \
    CsrWifiNmeWpsConfigSetReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, wpsConfig__)

/*                                                                              

      
                                 

             
            

            
                                   
                                   

                                                                              */
#define CsrWifiNmeWpsConfigSetCfmCreate(msg__, dst__, src__, status__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeWpsConfigSetCfm), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_WPS_CONFIG_SET_CFM, dst__, src__); \
    msg__->status = (status__);

#define CsrWifiNmeWpsConfigSetCfmSendTo(dst__, src__, status__) \
    { \
        CsrWifiNmeWpsConfigSetCfm *msg__; \
        CsrWifiNmeWpsConfigSetCfmCreate(msg__, dst__, src__, status__); \
        CsrSchedMessagePut(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeWpsConfigSetCfmSend(dst__, status__) \
    CsrWifiNmeWpsConfigSetCfmSendTo(dst__, CSR_WIFI_NME_IFACEQUEUE, status__)

/*                                                                              

      
                        

             
                                                                           
                                                                           
                                                                            
                                                                             
                                                                          

            
                                                                               
                                                                          
                             
                                         
                                                                                
                              

                                                                              */
#define CsrWifiNmeWpsReqCreate(msg__, dst__, src__, interfaceTag__, pin__, ssid__, bssid__) \
    msg__ = kmalloc(sizeof(CsrWifiNmeWpsReq), GFP_KERNEL); \
    CsrWifiFsmEventInit(&msg__->common, CSR_WIFI_NME_PRIM, CSR_WIFI_NME_WPS_REQ, dst__, src__); \
    msg__->interfaceTag = (interfaceTag__); \
    memcpy(msg__->pin, (pin__), sizeof(u8) * 8); \
    msg__->ssid = (ssid__); \
    msg__->bssid = (bssid__);

#define CsrWifiNmeWpsReqSendTo(dst__, src__, interfaceTag__, pin__, ssid__, bssid__) \
    { \
        CsrWifiNmeWpsReq *msg__; \
        CsrWifiNmeWpsReqCreate(msg__, dst__, src__, interfaceTag__, pin__, ssid__, bssid__); \
        CsrMsgTransport(dst__, CSR_WIFI_NME_PRIM, msg__); \
    }

#define CsrWifiNmeWpsReqSend(src__, interfaceTag__, pin__, ssid__, bssid__) \
    CsrWifiNmeWpsReqSendTo(CSR_WIFI_NME_IFACEQUEUE, src__, interfaceTag__, pin__, ssid__, bssid__)

#endif /*                      */
