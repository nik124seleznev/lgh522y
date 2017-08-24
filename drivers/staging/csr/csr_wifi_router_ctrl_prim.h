/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_ROUTER_CTRL_PRIM_H__
#define CSR_WIFI_ROUTER_CTRL_PRIM_H__

#include <linux/types.h>
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_wifi_common.h"
#include "csr_result.h"
#include "csr_wifi_fsm_event.h"

#define CSR_WIFI_ROUTER_CTRL_PRIM                                       (0x0401)

typedef CsrPrim CsrWifiRouterCtrlPrim;

typedef CsrResult (*CsrWifiRouterCtrlRawSdioByteWrite)(u8 func, u32 address, u8 data);
typedef CsrResult (*CsrWifiRouterCtrlRawSdioByteRead)(u8 func, u32 address, u8 *pdata);
typedef CsrResult (*CsrWifiRouterCtrlRawSdioFirmwareDownload)(u32 length, const u8 *pdata);
typedef CsrResult (*CsrWifiRouterCtrlRawSdioReset)(void);
typedef CsrResult (*CsrWifiRouterCtrlRawSdioCoreDumpPrepare)(u8 suspendSme);
typedef CsrResult (*CsrWifiRouterCtrlRawSdioByteBlockRead)(u8 func, u32 address, u8 *pdata, u32 length);
typedef CsrResult (*CsrWifiRouterCtrlRawSdioGpRead16)(u8 func, u32 address, u16 *pdata);
typedef CsrResult (*CsrWifiRouterCtrlRawSdioGpWrite16)(u8 func, u32 address, u16 data);

/*                                                                              

      
                                 

             

       
                                             
                    
                                            
                    

                                                                              */
typedef u8 CsrWifiRouterCtrlBlockAckRole;
#define CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_ORIGINATOR   ((CsrWifiRouterCtrlBlockAckRole) 0x00)
#define CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_RECIPIENT    ((CsrWifiRouterCtrlBlockAckRole) 0x01)

/*                                                                              

      
                                      

             

       
                                                 
                    
                                                
                    
                                                          
                    

                                                                              */
typedef u8 CsrWifiRouterCtrlControlIndication;
#define CSR_WIFI_ROUTER_CTRL_CONTROL_INDICATION_ERROR            ((CsrWifiRouterCtrlControlIndication) 0x01)
#define CSR_WIFI_ROUTER_CTRL_CONTROL_INDICATION_EXIT             ((CsrWifiRouterCtrlControlIndication) 0x02)
#define CSR_WIFI_ROUTER_CTRL_CONTROL_INDICATION_USER_REQUESTED   ((CsrWifiRouterCtrlControlIndication) 0x03)

/*                                                                              

      
                               

             

       
                                        
                    
                                        
                    
                                           
                    
                                          
                    

                                                                              */
typedef u8 CsrWifiRouterCtrlListAction;
#define CSR_WIFI_ROUTER_CTRL_LIST_ACTION_GET      ((CsrWifiRouterCtrlListAction) 0x00)
#define CSR_WIFI_ROUTER_CTRL_LIST_ACTION_ADD      ((CsrWifiRouterCtrlListAction) 0x01)
#define CSR_WIFI_ROUTER_CTRL_LIST_ACTION_REMOVE   ((CsrWifiRouterCtrlListAction) 0x02)
#define CSR_WIFI_ROUTER_CTRL_LIST_ACTION_FLUSH    ((CsrWifiRouterCtrlListAction) 0x03)

/*                                                                              

      
                                 

             

       
                                                
                    
                                               
                    

                                                                              */
typedef u16 CsrWifiRouterCtrlLowPowerMode;
#define CSR_WIFI_ROUTER_CTRL_LOW_POWER_MODE_DISABLED   ((CsrWifiRouterCtrlLowPowerMode) 0x0000)
#define CSR_WIFI_ROUTER_CTRL_LOW_POWER_MODE_ENABLED    ((CsrWifiRouterCtrlLowPowerMode) 0x0001)

/*                                                                              

      
                                

             

       
                                               
                    
                                                  
                    

                                                                              */
typedef u8 CsrWifiRouterCtrlMediaStatus;
#define CSR_WIFI_ROUTER_CTRL_MEDIA_STATUS_CONNECTED      ((CsrWifiRouterCtrlMediaStatus) 0x00)
#define CSR_WIFI_ROUTER_CTRL_MEDIA_STATUS_DISCONNECTED   ((CsrWifiRouterCtrlMediaStatus) 0x01)

/*                                                                              

      
                         

             

       
                                       
                                       
                                       
                                       
                                       
                                       
                                       
                                       
                                       

                                                                              */
typedef u8 CsrWifiRouterCtrlMode;
#define CSR_WIFI_ROUTER_CTRL_MODE_NONE      ((CsrWifiRouterCtrlMode) 0x00)
#define CSR_WIFI_ROUTER_CTRL_MODE_IBSS      ((CsrWifiRouterCtrlMode) 0x01)
#define CSR_WIFI_ROUTER_CTRL_MODE_STA       ((CsrWifiRouterCtrlMode) 0x02)
#define CSR_WIFI_ROUTER_CTRL_MODE_AP        ((CsrWifiRouterCtrlMode) 0x03)
#define CSR_WIFI_ROUTER_CTRL_MODE_MONITOR   ((CsrWifiRouterCtrlMode) 0x04)
#define CSR_WIFI_ROUTER_CTRL_MODE_AMP       ((CsrWifiRouterCtrlMode) 0x05)
#define CSR_WIFI_ROUTER_CTRL_MODE_P2P       ((CsrWifiRouterCtrlMode) 0x06)
#define CSR_WIFI_ROUTER_CTRL_MODE_P2PGO     ((CsrWifiRouterCtrlMode) 0x07)
#define CSR_WIFI_ROUTER_CTRL_MODE_P2PCLI    ((CsrWifiRouterCtrlMode) 0x08)

/*                                                                              

      
                               

             

       
                                              
                    
                                                  
                    
                                          
                    

                                                                              */
typedef u8 CsrWifiRouterCtrlPeerStatus;
#define CSR_WIFI_ROUTER_CTRL_PEER_CONNECTED_ACTIVE       ((CsrWifiRouterCtrlPeerStatus) 0x00)
#define CSR_WIFI_ROUTER_CTRL_PEER_CONNECTED_POWER_SAVE   ((CsrWifiRouterCtrlPeerStatus) 0x01)
#define CSR_WIFI_ROUTER_CTRL_PEER_DISCONNECTED           ((CsrWifiRouterCtrlPeerStatus) 0x02)

/*                                                                              

      
                               

             

       
                                                    
                    
                                                              
                    
                                                            
                    

                                                                              */
typedef u16 CsrWifiRouterCtrlPortAction;
#define CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_OPEN             ((CsrWifiRouterCtrlPortAction) 0x0000)
#define CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_CLOSED_DISCARD   ((CsrWifiRouterCtrlPortAction) 0x0001)
#define CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_CLOSED_BLOCK     ((CsrWifiRouterCtrlPortAction) 0x0002)

/*                                                                              

      
                                  

             

       
                                              
                                                                  
                                              
                                                                  
                                              
                                                                  
                                              
                                                                    
                                              
                    
                                               
                    
                                              
                    
                                               
                    
                                              
                    
                                               
                    
                                              
                    
                                               
                    

                                                                              */
typedef u16 CsrWifiRouterCtrlPowersaveType;
#define CSR_WIFI_ROUTER_CTRL_AC_BK_PS_INFO_PRESENT    ((CsrWifiRouterCtrlPowersaveType) 0x0001)
#define CSR_WIFI_ROUTER_CTRL_AC_BE_PS_INFO_PRESENT    ((CsrWifiRouterCtrlPowersaveType) 0x0002)
#define CSR_WIFI_ROUTER_CTRL_AC_VI_PS_INFO_PRESENT    ((CsrWifiRouterCtrlPowersaveType) 0x0004)
#define CSR_WIFI_ROUTER_CTRL_AC_VO_PS_INFO_PRESENT    ((CsrWifiRouterCtrlPowersaveType) 0x0008)
#define CSR_WIFI_ROUTER_CTRL_AC_BK_TRIGGER_ENABLED    ((CsrWifiRouterCtrlPowersaveType) 0x0010)
#define CSR_WIFI_ROUTER_CTRL_AC_BK_DELIVERY_ENABLED   ((CsrWifiRouterCtrlPowersaveType) 0x0020)
#define CSR_WIFI_ROUTER_CTRL_AC_BE_TRIGGER_ENABLED    ((CsrWifiRouterCtrlPowersaveType) 0x0040)
#define CSR_WIFI_ROUTER_CTRL_AC_BE_DELIVERY_ENABLED   ((CsrWifiRouterCtrlPowersaveType) 0x0080)
#define CSR_WIFI_ROUTER_CTRL_AC_VI_TRIGGER_ENABLED    ((CsrWifiRouterCtrlPowersaveType) 0x0100)
#define CSR_WIFI_ROUTER_CTRL_AC_VI_DELIVERY_ENABLED   ((CsrWifiRouterCtrlPowersaveType) 0x0200)
#define CSR_WIFI_ROUTER_CTRL_AC_VO_TRIGGER_ENABLED    ((CsrWifiRouterCtrlPowersaveType) 0x0400)
#define CSR_WIFI_ROUTER_CTRL_AC_VO_DELIVERY_ENABLED   ((CsrWifiRouterCtrlPowersaveType) 0x0800)

/*                                                                              

      
                                      

             

       
                                              
                    
                                              
                    

                                                                              */
typedef u16 CsrWifiRouterCtrlProtocolDirection;
#define CSR_WIFI_ROUTER_CTRL_PROTOCOL_DIRECTION_RX   ((CsrWifiRouterCtrlProtocolDirection) 0x0000)
#define CSR_WIFI_ROUTER_CTRL_PROTOCOL_DIRECTION_TX   ((CsrWifiRouterCtrlProtocolDirection) 0x0001)

/*                                                                              

      
                               

             

       
                                        
                    
                                           
                    
                                             
                    

                                                                              */
typedef u16 CsrWifiRouterCtrlQoSControl;
#define CSR_WIFI_ROUTER_CTRL_QOS_CONTROL_OFF        ((CsrWifiRouterCtrlQoSControl) 0x0000)
#define CSR_WIFI_ROUTER_CTRL_QOS_CONTROL_WMM_ON     ((CsrWifiRouterCtrlQoSControl) 0x0001)
#define CSR_WIFI_ROUTER_CTRL_QOS_CONTROL_80211_ON   ((CsrWifiRouterCtrlQoSControl) 0x0002)

/*                                                                              

      
                                

             
                                             

       
                                        
                    
                                        
                    
                                        
                    
                                        
                    

                                                                              */
typedef u8 CsrWifiRouterCtrlQueueConfig;
#define CSR_WIFI_ROUTER_CTRL_QUEUE_BE_ENABLE   ((CsrWifiRouterCtrlQueueConfig) 0x01)
#define CSR_WIFI_ROUTER_CTRL_QUEUE_BK_ENABLE   ((CsrWifiRouterCtrlQueueConfig) 0x02)
#define CSR_WIFI_ROUTER_CTRL_QUEUE_VI_ENABLE   ((CsrWifiRouterCtrlQueueConfig) 0x04)
#define CSR_WIFI_ROUTER_CTRL_QUEUE_VO_ENABLE   ((CsrWifiRouterCtrlQueueConfig) 0x08)

/*                                                                              

      
                                      

             

       
                                                  
                    
                                                   
                    
                                                
                    

                                                                              */
typedef u16 CsrWifiRouterCtrlTrafficConfigType;
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_CONFIG_TYPE_RESET    ((CsrWifiRouterCtrlTrafficConfigType) 0x0000)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_CONFIG_TYPE_FILTER   ((CsrWifiRouterCtrlTrafficConfigType) 0x0001)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_CONFIG_TYPE_CLS      ((CsrWifiRouterCtrlTrafficConfigType) 0x0002)

/*                                                                              

      
                                      

             

       
                                                 
                    
                                                  
                    
                                                 
                    
                                                     
                    
                                                
                    
                                                    
                    
                                                   
                    
                                                
                    

                                                                              */
typedef u16 CsrWifiRouterCtrlTrafficPacketType;
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_PACKET_TYPE_NONE       ((CsrWifiRouterCtrlTrafficPacketType) 0x0000)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_PACKET_TYPE_EAPOL      ((CsrWifiRouterCtrlTrafficPacketType) 0x0001)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_PACKET_TYPE_DHCP       ((CsrWifiRouterCtrlTrafficPacketType) 0x0002)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_PACKET_TYPE_DHCP_ACK   ((CsrWifiRouterCtrlTrafficPacketType) 0x0004)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_PACKET_TYPE_ARP        ((CsrWifiRouterCtrlTrafficPacketType) 0x0008)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_PACKET_TYPE_AIRONET    ((CsrWifiRouterCtrlTrafficPacketType) 0x0010)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_PACKET_TYPE_CUSTOM     ((CsrWifiRouterCtrlTrafficPacketType) 0x0020)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_PACKET_TYPE_ALL        ((CsrWifiRouterCtrlTrafficPacketType) 0x00FF)

/*                                                                              

      
                                

             

       
                                                
                    
                                            
                    
                                              
                    
                                                
                    

                                                                              */
typedef u8 CsrWifiRouterCtrlTrafficType;
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_TYPE_OCCASIONAL   ((CsrWifiRouterCtrlTrafficType) 0x00)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_TYPE_BURSTY       ((CsrWifiRouterCtrlTrafficType) 0x01)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_TYPE_PERIODIC     ((CsrWifiRouterCtrlTrafficType) 0x02)
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_TYPE_CONTINUOUS   ((CsrWifiRouterCtrlTrafficType) 0x03)


/*                                                                              

      
                                     

             

                                                                              */
typedef u32 CsrWifiRouterCtrlPeerRecordHandle;
/*                                                                              

      
                                      

             
                                                
                                  

                                                                              */
typedef u16 CsrWifiRouterCtrlPowersaveTypeMask;
/*                                                                              

      
                                    

             
                                                                             

                                                                              */
typedef u8 CsrWifiRouterCtrlQueueConfigMask;
/*                                                                              

      
                                  

             

                                                                              */
typedef u16 CsrWifiRouterCtrlRequestorInfo;
/*                                                                              

      
                                    

             

                                                                              */
typedef u8 CsrWifiRouterCtrlTrafficStreamId;


/*                                                                              

      
                                

             

         
                   
                   
                   

                                                                              */
typedef struct
{
    u32      firmwarePatch;
    char *smeBuild;
    u32      smeHip;
} CsrWifiRouterCtrlSmeVersions;

/*                                                                              

      
                            

             

         
                         
                         
                         
                         

                                                                              */
typedef struct
{
    u8                            wmmOrQosEnabled;
    CsrWifiRouterCtrlPowersaveTypeMask powersaveMode;
    u8                           maxSpLength;
    u16                          listenIntervalInTus;
} CsrWifiRouterCtrlStaInfo;

/*                                                                              

      
                                  

             

         
                   
                   
                   
                   

                                                                              */
typedef struct
{
    u32 etherType;
    u8  ipType;
    u32 udpSourcePort;
    u32 udpDestPort;
} CsrWifiRouterCtrlTrafficFilter;

/*                                                                              

      
                                 

             

         
                                                      
                                                                    
                                                                    
                                                                         
                                                                         
                                                            

                                                                              */
typedef struct
{
    u32 rxMeanRate;
    u32 rxFramesNum;
    u32 txFramesNum;
    u32 rxBytesCount;
    u32 txBytesCount;
    u8  intervals[11];
} CsrWifiRouterCtrlTrafficStats;

/*                                                                              

      
                             

             

         
                   
                   
                   
                   
                   
                   

                                                                              */
typedef struct
{
    u32      chipId;
    u32      chipVersion;
    u32      firmwareBuild;
    u32      firmwareHip;
    char *routerBuild;
    u32      routerHip;
} CsrWifiRouterCtrlVersions;

/*                                                                              

      
                                  

             

         
                  
                  

                                                                              */
typedef struct
{
    u16                      packetFilter;
    CsrWifiRouterCtrlTrafficFilter customFilter;
} CsrWifiRouterCtrlTrafficConfig;


/*            */
#define CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST            (0x0000)

#define CSR_WIFI_ROUTER_CTRL_CONFIGURE_POWER_MODE_REQ     ((CsrWifiRouterCtrlPrim) (0x0000 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_HIP_REQ                      ((CsrWifiRouterCtrlPrim) (0x0001 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_MEDIA_STATUS_REQ             ((CsrWifiRouterCtrlPrim) (0x0002 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_MULTICAST_ADDRESS_RES        ((CsrWifiRouterCtrlPrim) (0x0003 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_PORT_CONFIGURE_REQ           ((CsrWifiRouterCtrlPrim) (0x0004 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_QOS_CONTROL_REQ              ((CsrWifiRouterCtrlPrim) (0x0005 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_SUSPEND_RES                  ((CsrWifiRouterCtrlPrim) (0x0006 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_TCLAS_ADD_REQ                ((CsrWifiRouterCtrlPrim) (0x0007 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_RESUME_RES                   ((CsrWifiRouterCtrlPrim) (0x0008 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_RAW_SDIO_DEINITIALISE_REQ    ((CsrWifiRouterCtrlPrim) (0x0009 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_RAW_SDIO_INITIALISE_REQ      ((CsrWifiRouterCtrlPrim) (0x000A + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_TCLAS_DEL_REQ                ((CsrWifiRouterCtrlPrim) (0x000B + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_CLASSIFICATION_REQ   ((CsrWifiRouterCtrlPrim) (0x000C + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_CONFIG_REQ           ((CsrWifiRouterCtrlPrim) (0x000D + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WIFI_OFF_REQ                 ((CsrWifiRouterCtrlPrim) (0x000E + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WIFI_OFF_RES                 ((CsrWifiRouterCtrlPrim) (0x000F + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WIFI_ON_REQ                  ((CsrWifiRouterCtrlPrim) (0x0010 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WIFI_ON_RES                  ((CsrWifiRouterCtrlPrim) (0x0011 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_M4_TRANSMIT_REQ              ((CsrWifiRouterCtrlPrim) (0x0012 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_MODE_SET_REQ                 ((CsrWifiRouterCtrlPrim) (0x0013 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_PEER_ADD_REQ                 ((CsrWifiRouterCtrlPrim) (0x0014 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_PEER_DEL_REQ                 ((CsrWifiRouterCtrlPrim) (0x0015 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_PEER_UPDATE_REQ              ((CsrWifiRouterCtrlPrim) (0x0016 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_CAPABILITIES_REQ             ((CsrWifiRouterCtrlPrim) (0x0017 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_ENABLE_REQ         ((CsrWifiRouterCtrlPrim) (0x0018 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_DISABLE_REQ        ((CsrWifiRouterCtrlPrim) (0x0019 + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WAPI_RX_PKT_REQ              ((CsrWifiRouterCtrlPrim) (0x001A + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WAPI_MULTICAST_FILTER_REQ    ((CsrWifiRouterCtrlPrim) (0x001B + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WAPI_UNICAST_FILTER_REQ      ((CsrWifiRouterCtrlPrim) (0x001C + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WAPI_UNICAST_TX_PKT_REQ      ((CsrWifiRouterCtrlPrim) (0x001D + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WAPI_FILTER_REQ              ((CsrWifiRouterCtrlPrim) (0x001E + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST))


#define CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_HIGHEST           (0x001E + CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST)

/*          */
#define CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_WIFI_ROUTER_CTRL_HIP_IND                      ((CsrWifiRouterCtrlPrim)(0x0000 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_MULTICAST_ADDRESS_IND        ((CsrWifiRouterCtrlPrim)(0x0001 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_PORT_CONFIGURE_CFM           ((CsrWifiRouterCtrlPrim)(0x0002 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_RESUME_IND                   ((CsrWifiRouterCtrlPrim)(0x0003 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_SUSPEND_IND                  ((CsrWifiRouterCtrlPrim)(0x0004 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_TCLAS_ADD_CFM                ((CsrWifiRouterCtrlPrim)(0x0005 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_RAW_SDIO_DEINITIALISE_CFM    ((CsrWifiRouterCtrlPrim)(0x0006 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_RAW_SDIO_INITIALISE_CFM      ((CsrWifiRouterCtrlPrim)(0x0007 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_TCLAS_DEL_CFM                ((CsrWifiRouterCtrlPrim)(0x0008 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_PROTOCOL_IND         ((CsrWifiRouterCtrlPrim)(0x0009 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_TRAFFIC_SAMPLE_IND           ((CsrWifiRouterCtrlPrim)(0x000A + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WIFI_OFF_IND                 ((CsrWifiRouterCtrlPrim)(0x000B + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WIFI_OFF_CFM                 ((CsrWifiRouterCtrlPrim)(0x000C + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WIFI_ON_IND                  ((CsrWifiRouterCtrlPrim)(0x000D + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WIFI_ON_CFM                  ((CsrWifiRouterCtrlPrim)(0x000E + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_M4_READY_TO_SEND_IND         ((CsrWifiRouterCtrlPrim)(0x000F + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_M4_TRANSMITTED_IND           ((CsrWifiRouterCtrlPrim)(0x0010 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_MIC_FAILURE_IND              ((CsrWifiRouterCtrlPrim)(0x0011 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_CONNECTED_IND                ((CsrWifiRouterCtrlPrim)(0x0012 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_PEER_ADD_CFM                 ((CsrWifiRouterCtrlPrim)(0x0013 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_PEER_DEL_CFM                 ((CsrWifiRouterCtrlPrim)(0x0014 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_UNEXPECTED_FRAME_IND         ((CsrWifiRouterCtrlPrim)(0x0015 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_PEER_UPDATE_CFM              ((CsrWifiRouterCtrlPrim)(0x0016 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_CAPABILITIES_CFM             ((CsrWifiRouterCtrlPrim)(0x0017 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_ENABLE_CFM         ((CsrWifiRouterCtrlPrim)(0x0018 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_DISABLE_CFM        ((CsrWifiRouterCtrlPrim)(0x0019 + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_BLOCK_ACK_ERROR_IND          ((CsrWifiRouterCtrlPrim)(0x001A + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_STA_INACTIVE_IND             ((CsrWifiRouterCtrlPrim)(0x001B + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WAPI_RX_MIC_CHECK_IND        ((CsrWifiRouterCtrlPrim)(0x001C + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_MODE_SET_CFM                 ((CsrWifiRouterCtrlPrim)(0x001D + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_ROUTER_CTRL_WAPI_UNICAST_TX_ENCRYPT_IND  ((CsrWifiRouterCtrlPrim)(0x001E + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST))

#define CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_HIGHEST             (0x001E + CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST)

#define CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_COUNT             (CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_WIFI_ROUTER_CTRL_PRIM_DOWNSTREAM_LOWEST)
#define CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_COUNT               (CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_HIGHEST   + 1 - CSR_WIFI_ROUTER_CTRL_PRIM_UPSTREAM_LOWEST)

/*                                                                              

      
                                          

             

         
                                                                 
                
                
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrWifiRouterCtrlLowPowerMode  mode;
    u8                        wakeHost;
} CsrWifiRouterCtrlConfigurePowerModeReq;

/*                                                                              

      
                           

             
                                                                     

         
                                                                        
                                                 
                                                  
                                                        
                                                  
                                                        
                                                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       mlmeCommandLength;
    u8       *mlmeCommand;
    u16       dataRef1Length;
    u8       *dataRef1;
    u16       dataRef2Length;
    u8       *dataRef2;
} CsrWifiRouterCtrlHipReq;

/*                                                                              

      
                                   

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrWifiRouterCtrlMediaStatus   mediaStatus;
} CsrWifiRouterCtrlMediaStatusReq;

/*                                                                              

      
                                        

             

         
                                                                        
                       
                       
                       
                       
                       
                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrResult                      status;
    CsrWifiRouterCtrlListAction    action;
    u8                       getAddressesCount;
    CsrWifiMacAddress             *getAddresses;
} CsrWifiRouterCtrlMulticastAddressRes;

/*                                                                              

      
                                     

             

         
                                                                             
                            
                            
                            
                            
                            
                            

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrWifiRouterCtrlPortAction    uncontrolledPortAction;
    CsrWifiRouterCtrlPortAction    controlledPortAction;
    CsrWifiMacAddress              macAddress;
    u8                        setProtection;
} CsrWifiRouterCtrlPortConfigureReq;

/*                                                                              

      
                                  

             

         
                                                                   
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                  common;
    u16                        interfaceTag;
    CsrWifiRouterCtrlRequestorInfo   clientData;
    CsrWifiRouterCtrlQoSControl      control;
    CsrWifiRouterCtrlQueueConfigMask queueConfig;
} CsrWifiRouterCtrlQosControlReq;

/*                                                                              

      
                               

             

         
                                                                 
                
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrResult                      status;
} CsrWifiRouterCtrlSuspendRes;

/*                                                                              

      
                                

             

         
                                                                   
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      tclasLength;
    u8                      *tclas;
} CsrWifiRouterCtrlTclasAddReq;

/*                                                                              

      
                              

             

         
                                                                 
                
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrResult                      status;
} CsrWifiRouterCtrlResumeRes;

/*                                                                              

      
                                           

             

         
                                                                 
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
} CsrWifiRouterCtrlRawSdioDeinitialiseReq;

/*                                                                              

      
                                         

             

         
                                                                 
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
} CsrWifiRouterCtrlRawSdioInitialiseReq;

/*                                                                              

      
                                

             

         
                                                                   
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      tclasLength;
    u8                      *tclas;
} CsrWifiRouterCtrlTclasDelReq;

/*                                                                              

      
                                             

             

         
                                                                   
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrWifiRouterCtrlTrafficType   trafficType;
    u16                      period;
} CsrWifiRouterCtrlTrafficClassificationReq;

/*                                                                              

      
                                     

             

         
                                                                        
                       
                       
                       
                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                    common;
    u16                          interfaceTag;
    CsrWifiRouterCtrlRequestorInfo     clientData;
    CsrWifiRouterCtrlTrafficConfigType trafficConfigType;
    CsrWifiRouterCtrlTrafficConfig     config;
} CsrWifiRouterCtrlTrafficConfigReq;

/*                                                                              

      
                               

             

         
                                                                 
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
} CsrWifiRouterCtrlWifiOffReq;

/*                                                                              

      
                               

             

         
                                                                 
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
} CsrWifiRouterCtrlWifiOffRes;

/*                                                                              

      
                              

             

         
                                                                 
                
                                                                   
                                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u32                      dataLength;
    u8                      *data;
} CsrWifiRouterCtrlWifiOnReq;

/*                                                                              

      
                              

             

         
                                                                          
                         
                         
                         
                                                                         
                         
                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrResult                      status;
    u16                      numInterfaceAddress;
    CsrWifiMacAddress              stationMacAddress[2];
    CsrWifiRouterCtrlSmeVersions   smeVersions;
    u8                        scheduledInterrupt;
} CsrWifiRouterCtrlWifiOnRes;

/*                                                                              

      
                                  

             

         
                                                                   
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    CsrWifiRouterCtrlRequestorInfo clientData;
} CsrWifiRouterCtrlM4TransmitReq;

/*                                                                              

      
                               

             

         
                                                                          
                         
                         
                         
                                                                               
                            
                                                                      
                                                     
                                                                        
                                                                              
                                                                                
                                                                              
                                                                        

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrWifiRouterCtrlMode          mode;
    CsrWifiMacAddress              bssid;
    u8                        protection;
    u8                        intraBssDistEnabled;
} CsrWifiRouterCtrlModeSetReq;

/*                                                                              

      
                               

             

         
                                                                     
                    
                    
                    
                    
                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    u16                      interfaceTag;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrWifiMacAddress              peerMacAddress;
    u16                      associationId;
    CsrWifiRouterCtrlStaInfo       staInfo;
} CsrWifiRouterCtrlPeerAddReq;

/*                                                                              

      
                               

             

         
                                                                       
                      
                      
                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                   common;
    u16                         interfaceTag;
    CsrWifiRouterCtrlRequestorInfo    clientData;
    CsrWifiRouterCtrlPeerRecordHandle peerRecordHandle;
} CsrWifiRouterCtrlPeerDelReq;

/*                                                                              

      
                                  

             

         
                                                                       
                      
                      
                      
                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                    common;
    u16                          interfaceTag;
    CsrWifiRouterCtrlRequestorInfo     clientData;
    CsrWifiRouterCtrlPeerRecordHandle  peerRecordHandle;
    CsrWifiRouterCtrlPowersaveTypeMask powersaveMode;
} CsrWifiRouterCtrlPeerUpdateReq;

/*                                                                              

      
                                    

             

         
                                                                 
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
} CsrWifiRouterCtrlCapabilitiesReq;

/*                                                                              

      
                                      

             

         
                                                                      
                     
                     
                     
                     
                     
                     
                     
                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                  common;
    u16                        interfaceTag;
    CsrWifiRouterCtrlRequestorInfo   clientData;
    CsrWifiMacAddress                macAddress;
    CsrWifiRouterCtrlTrafficStreamId trafficStreamID;
    CsrWifiRouterCtrlBlockAckRole    role;
    u16                        bufferSize;
    u16                        timeout;
    u16                        ssn;
} CsrWifiRouterCtrlBlockAckEnableReq;

/*                                                                              

      
                                       

             

         
                                                                      
                     
                     
                     
                     
                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                  common;
    u16                        interfaceTag;
    CsrWifiRouterCtrlRequestorInfo   clientData;
    CsrWifiMacAddress                macAddress;
    CsrWifiRouterCtrlTrafficStreamId trafficStreamID;
    CsrWifiRouterCtrlBlockAckRole    role;
} CsrWifiRouterCtrlBlockAckDisableReq;

/*                                                                              

      
                                 

             

         
                                                                   
                  
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    u16       signalLength;
    u8       *signal;
    u16       dataLength;
    u8       *data;
} CsrWifiRouterCtrlWapiRxPktReq;

/*                                                                              

      
                                           

             

         
                                                                   
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    u8        status;
} CsrWifiRouterCtrlWapiMulticastFilterReq;

/*                                                                              

      
                                         

             

         
                                                                   
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    u8        status;
} CsrWifiRouterCtrlWapiUnicastFilterReq;

/*                                                                              

      
                                        

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    u16       dataLength;
    u8       *data;
} CsrWifiRouterCtrlWapiUnicastTxPktReq;

/*                                                                              

      
                                  

             

         
                                                                      
                     
                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    u8         isWapiConnected;
} CsrWifiRouterCtrlWapiFilterReq;

/*                                                                              

      
                           

             
                                                                       

         
                                                                        
                                                 
                                                  
                                                        
                                                  
                                                        
                                                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       mlmeCommandLength;
    u8       *mlmeCommand;
    u16       dataRef1Length;
    u8       *dataRef1;
    u16       dataRef2Length;
    u8       *dataRef2;
} CsrWifiRouterCtrlHipInd;

/*                                                                              

      
                                        

             

         
                                                                        
                       
                       
                       
                       
                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrWifiRouterCtrlListAction    action;
    u8                       setAddressesCount;
    CsrWifiMacAddress             *setAddresses;
} CsrWifiRouterCtrlMulticastAddressInd;

/*                                                                              

      
                                     

             

         
                                                                   
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrResult                      status;
    CsrWifiMacAddress              macAddress;
} CsrWifiRouterCtrlPortConfigureCfm;

/*                                                                              

      
                              

             

         
                                                                      
                     
                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u8                        powerMaintained;
} CsrWifiRouterCtrlResumeInd;

/*                                                                              

      
                               

             

         
                                                                  
                 
                 
                 

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u8                        hardSuspend;
    u8                        d3Suspend;
} CsrWifiRouterCtrlSuspendInd;

/*                                                                              

      
                                

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrResult                      status;
} CsrWifiRouterCtrlTclasAddCfm;

/*                                                                              

      
                                           

             

         
                                                                 
                
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrResult                      result;
} CsrWifiRouterCtrlRawSdioDeinitialiseCfm;

/*                                                                              

      
                                         

             

         
                                                                       
                      
                      
                      
                      
                      
                      
                      
                      
                      
                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                          common;
    CsrWifiRouterCtrlRequestorInfo           clientData;
    CsrResult                                result;
    CsrWifiRouterCtrlRawSdioByteRead         byteRead;
    CsrWifiRouterCtrlRawSdioByteWrite        byteWrite;
    CsrWifiRouterCtrlRawSdioFirmwareDownload firmwareDownload;
    CsrWifiRouterCtrlRawSdioReset            reset;
    CsrWifiRouterCtrlRawSdioCoreDumpPrepare  coreDumpPrepare;
    CsrWifiRouterCtrlRawSdioByteBlockRead    byteBlockRead;
    CsrWifiRouterCtrlRawSdioGpRead16         gpRead16;
    CsrWifiRouterCtrlRawSdioGpWrite16        gpWrite16;
} CsrWifiRouterCtrlRawSdioInitialiseCfm;

/*                                                                              

      
                                

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrResult                      status;
} CsrWifiRouterCtrlTclasDelCfm;

/*                                                                              

      
                                       

             

         
                                                                   
                  
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                    common;
    CsrWifiRouterCtrlRequestorInfo     clientData;
    u16                          interfaceTag;
    CsrWifiRouterCtrlTrafficPacketType packetType;
    CsrWifiRouterCtrlProtocolDirection direction;
    CsrWifiMacAddress                  srcAddress;
} CsrWifiRouterCtrlTrafficProtocolInd;

/*                                                                              

      
                                     

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrWifiRouterCtrlTrafficStats  stats;
} CsrWifiRouterCtrlTrafficSampleInd;

/*                                                                              

      
                               

             

         
                                                                        
                       
                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                    common;
    CsrWifiRouterCtrlRequestorInfo     clientData;
    CsrWifiRouterCtrlControlIndication controlIndication;
} CsrWifiRouterCtrlWifiOffInd;

/*                                                                              

      
                               

             

         
                                                                 
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
} CsrWifiRouterCtrlWifiOffCfm;

/*                                                                              

      
                              

             

         
                                                                 
                
                
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrResult                      status;
    CsrWifiRouterCtrlVersions      versions;
} CsrWifiRouterCtrlWifiOnInd;

/*                                                                              

      
                              

             

         
                                                                 
                
                

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    CsrResult                      status;
} CsrWifiRouterCtrlWifiOnCfm;

/*                                                                              

      
                                     

             

         
                                                                     
                    
                    
                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrWifiMacAddress              peerMacAddress;
} CsrWifiRouterCtrlM4ReadyToSendInd;

/*                                                                              

      
                                     

             

         
                                                                     
                    
                    
                    
                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrWifiMacAddress              peerMacAddress;
    CsrResult                      status;
} CsrWifiRouterCtrlM4TransmittedInd;

/*                                                                              

      
                                  

             

         
                                                                     
                    
                    
                    
                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrWifiMacAddress              peerMacAddress;
    u8                        unicastPdu;
} CsrWifiRouterCtrlMicFailureInd;

/*                                                                              

      
                                 

             

         
                                                                     
                    
                    
                    
                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrWifiMacAddress              peerMacAddress;
    CsrWifiRouterCtrlPeerStatus    peerStatus;
} CsrWifiRouterCtrlConnectedInd;

/*                                                                              

      
                               

             

         
                                                                       
                      
                      
                      
                      
                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                   common;
    CsrWifiRouterCtrlRequestorInfo    clientData;
    u16                         interfaceTag;
    CsrWifiMacAddress                 peerMacAddress;
    CsrWifiRouterCtrlPeerRecordHandle peerRecordHandle;
    CsrResult                         status;
} CsrWifiRouterCtrlPeerAddCfm;

/*                                                                              

      
                               

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrResult                      status;
} CsrWifiRouterCtrlPeerDelCfm;

/*                                                                              

      
                                       

             

         
                                                                     
                    
                    
                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrWifiMacAddress              peerMacAddress;
} CsrWifiRouterCtrlUnexpectedFrameInd;

/*                                                                              

      
                                  

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrResult                      status;
} CsrWifiRouterCtrlPeerUpdateCfm;

/*                                                                              

      
                                    

             
                                                                            
        

         
                                                                       
                      
                                            
                                                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      commandQueueSize;
    u16                      trafficQueueSize;
} CsrWifiRouterCtrlCapabilitiesCfm;

/*                                                                              

      
                                      

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrResult                      status;
} CsrWifiRouterCtrlBlockAckEnableCfm;

/*                                                                              

      
                                       

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrResult                      status;
} CsrWifiRouterCtrlBlockAckDisableCfm;

/*                                                                              

      
                                     

             

         
                                                                      
                     
                     
                     
                     
                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                  common;
    CsrWifiRouterCtrlRequestorInfo   clientData;
    u16                        interfaceTag;
    CsrWifiRouterCtrlTrafficStreamId trafficStreamID;
    CsrWifiMacAddress                peerMacAddress;
    CsrResult                        status;
} CsrWifiRouterCtrlBlockAckErrorInd;

/*                                                                              

      
                                   

             

         
                                                                   
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrWifiMacAddress              staAddress;
} CsrWifiRouterCtrlStaInactiveInd;

/*                                                                              

      
                                      

             

         
                                                                   
                  
                  
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    u16                      signalLength;
    u8                      *signal;
    u16                      dataLength;
    u8                      *data;
} CsrWifiRouterCtrlWapiRxMicCheckInd;

/*                                                                              

      
                               

             

         
                                                                   
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    CsrWifiRouterCtrlMode          mode;
    CsrResult                      status;
} CsrWifiRouterCtrlModeSetCfm;

/*                                                                              

      
                                            

             

         
                                                                   
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent                common;
    CsrWifiRouterCtrlRequestorInfo clientData;
    u16                      interfaceTag;
    u16                      dataLength;
    u8                      *data;
} CsrWifiRouterCtrlWapiUnicastTxEncryptInd;

#endif /*                               */

