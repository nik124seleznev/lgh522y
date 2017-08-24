/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_ROUTER_PRIM_H__
#define CSR_WIFI_ROUTER_PRIM_H__

#include <linux/types.h>
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_wifi_common.h"
#include "csr_result.h"
#include "csr_wifi_fsm_event.h"

#define CSR_WIFI_ROUTER_PRIM                                            (0x0400)

typedef CsrPrim CsrWifiRouterPrim;

typedef void (*CsrWifiRouterFrameFreeFunction)(void *frame);

/*                                                                              

      
                        

             

       
                                    
                                    
                                    
                                    

                                                                              */
typedef u8 CsrWifiRouterAppType;
#define CSR_WIFI_ROUTER_APP_TYPE_SME     ((CsrWifiRouterAppType) 0x0)
#define CSR_WIFI_ROUTER_APP_TYPE_PAL     ((CsrWifiRouterAppType) 0x1)
#define CSR_WIFI_ROUTER_APP_TYPE_NME     ((CsrWifiRouterAppType) 0x2)
#define CSR_WIFI_ROUTER_APP_TYPE_OTHER   ((CsrWifiRouterAppType) 0x3)

/*                                                                              

      
                              

             
                                                                 

       
                                          
                                           
                                          
                                           

                                                                              */
typedef u8 CsrWifiRouterEncapsulation;
#define CSR_WIFI_ROUTER_ENCAPSULATION_ETHERNET   ((CsrWifiRouterEncapsulation) 0x00)
#define CSR_WIFI_ROUTER_ENCAPSULATION_LLC_SNAP   ((CsrWifiRouterEncapsulation) 0x01)

/*                                                                              

      
                    

             

       
                                  
                                  

                                                                              */
typedef u32 CsrWifiRouterOui;
#define CSR_WIFI_ROUTER_OUI_RFC_1042   ((CsrWifiRouterOui) 0x000000)
#define CSR_WIFI_ROUTER_OUI_BT         ((CsrWifiRouterOui) 0x001958)

/*                                                                              

      
                         

             
                                           

       
                                    
                                             
                                    
                                             
                                    
                                             
                                    
                                             
                                    
                                             
                                    
                                             
                                    
                                             
                                    
                                             
                                       
                                             
                                       
                                             

                                                                              */
typedef u16 CsrWifiRouterPriority;
#define CSR_WIFI_ROUTER_PRIORITY_QOS_UP0      ((CsrWifiRouterPriority) 0x0000)
#define CSR_WIFI_ROUTER_PRIORITY_QOS_UP1      ((CsrWifiRouterPriority) 0x0001)
#define CSR_WIFI_ROUTER_PRIORITY_QOS_UP2      ((CsrWifiRouterPriority) 0x0002)
#define CSR_WIFI_ROUTER_PRIORITY_QOS_UP3      ((CsrWifiRouterPriority) 0x0003)
#define CSR_WIFI_ROUTER_PRIORITY_QOS_UP4      ((CsrWifiRouterPriority) 0x0004)
#define CSR_WIFI_ROUTER_PRIORITY_QOS_UP5      ((CsrWifiRouterPriority) 0x0005)
#define CSR_WIFI_ROUTER_PRIORITY_QOS_UP6      ((CsrWifiRouterPriority) 0x0006)
#define CSR_WIFI_ROUTER_PRIORITY_QOS_UP7      ((CsrWifiRouterPriority) 0x0007)
#define CSR_WIFI_ROUTER_PRIORITY_CONTENTION   ((CsrWifiRouterPriority) 0x8000)
#define CSR_WIFI_ROUTER_PRIORITY_MANAGEMENT   ((CsrWifiRouterPriority) 0x8010)


/*            */
#define CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_LOWEST            (0x0000)

#define CSR_WIFI_ROUTER_MA_PACKET_SUBSCRIBE_REQ           ((CsrWifiRouterPrim) (0x0000 + CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_MA_PACKET_UNSUBSCRIBE_REQ         ((CsrWifiRouterPrim) (0x0001 + CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_MA_PACKET_REQ                     ((CsrWifiRouterPrim) (0x0002 + CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_MA_PACKET_RES                     ((CsrWifiRouterPrim) (0x0003 + CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_MA_PACKET_CANCEL_REQ              ((CsrWifiRouterPrim) (0x0004 + CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_LOWEST))


#define CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_HIGHEST           (0x0004 + CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_LOWEST)

/*          */
#define CSR_WIFI_ROUTER_PRIM_UPSTREAM_LOWEST              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_WIFI_ROUTER_MA_PACKET_SUBSCRIBE_CFM           ((CsrWifiRouterPrim)(0x0000 + CSR_WIFI_ROUTER_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_MA_PACKET_UNSUBSCRIBE_CFM         ((CsrWifiRouterPrim)(0x0001 + CSR_WIFI_ROUTER_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_MA_PACKET_CFM                     ((CsrWifiRouterPrim)(0x0002 + CSR_WIFI_ROUTER_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_MA_PACKET_IND                     ((CsrWifiRouterPrim)(0x0003 + CSR_WIFI_ROUTER_PRIM_UPSTREAM_LOWEST))

#define CSR_WIFI_ROUTER_PRIM_UPSTREAM_HIGHEST             (0x0003 + CSR_WIFI_ROUTER_PRIM_UPSTREAM_LOWEST)

#define CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_COUNT             (CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_WIFI_ROUTER_PRIM_DOWNSTREAM_LOWEST)
#define CSR_WIFI_ROUTER_PRIM_UPSTREAM_COUNT               (CSR_WIFI_ROUTER_PRIM_UPSTREAM_HIGHEST   + 1 - CSR_WIFI_ROUTER_PRIM_UPSTREAM_LOWEST)

/*                                                                              

      
                                     

             
                                                                            
                                                              
                                                                            
                                                                    
                  

         
                                                                    
                                                                           
                                                                                
                                
                                                                               
                                         
                                                                             
                                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent            common;
    u16                  interfaceTag;
    CsrWifiRouterEncapsulation encapsulation;
    u16                  protocol;
    u32                  oui;
} CsrWifiRouterMaPacketSubscribeReq;

/*                                                                              

      
                                       

             
                                                             

         
                                                                         
                                                                                
                                                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    u8        subscriptionHandle;
} CsrWifiRouterMaPacketUnsubscribeReq;

/*                                                                              

      
                            

             
                                                    

         
                                                                         
                                                                                
                                                       
                                                                
                                                        
                                                                         
                                                                    
                                                                                
                                                                               
                                                                               
                                                                       
                                                                                
                                                                              
                                                                              
                                                                       
                                                                               
                                                           
                                                                              
                                                                               
                                                                              
                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    u8                       subscriptionHandle;
    u16                      frameLength;
    u8                      *frame;
    CsrWifiRouterFrameFreeFunction freeFunction;
    CsrWifiRouterPriority          priority;
    u32                      hostTag;
    u8                        cfmRequested;
} CsrWifiRouterMaPacketReq;

/*                                                                              

      
                            

             
                                                                       
          

         
                                                                         
                                                                                
                                                       
                                                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    u8        subscriptionHandle;
    CsrResult       result;
} CsrWifiRouterMaPacketRes;

/*                                                                              

      
                                  

             
                                                                       
                             
                                                                         
                                                                             
                             
                                                                 
                             
                                                          
                             

         
                                                                     
                                                                            
                                                                          
                                                                     
                                                                          
                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    u16             interfaceTag;
    u32             hostTag;
    CsrWifiRouterPriority priority;
    CsrWifiMacAddress     peerMacAddress;
} CsrWifiRouterMaPacketCancelReq;

/*                                                                              

      
                                     

             
                                                                
                 

         
                                                                         
                                                                                
                                                   
                                                                            
                                                
                                                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    u8        subscriptionHandle;
    CsrResult       status;
    u16       allocOffset;
} CsrWifiRouterMaPacketSubscribeCfm;

/*                                                                              

      
                                       

             
                                                                
                   

         
                                                                   
                                                                          
                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiRouterMaPacketUnsubscribeCfm;

/*                                                                              

      
                            

             
                                                                            
                                                                             

         
                                                                   
                                                                          
                                          
                                                                           
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       result;
    u32       hostTag;
    u16       rate;
} CsrWifiRouterMaPacketCfm;

/*                                                                              

      
                            

             
                                                                          
                                    

         
                                                                         
                                                                                
                                                       
                                                
                                                              
                                                      
                                                                         
                                                                   
                                              
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    u8                       subscriptionHandle;
    CsrResult                      result;
    u16                      frameLength;
    u8                      *frame;
    CsrWifiRouterFrameFreeFunction freeFunction;
    s16                       rssi;
    s16                       snr;
    u16                      rate;
} CsrWifiRouterMaPacketInd;

#endif /*                          */

