/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_SME_AP_PRIM_H__
#define CSR_WIFI_SME_AP_PRIM_H__

#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_wifi_common.h"
#include "csr_result.h"
#include "csr_wifi_fsm_event.h"
#include "csr_wifi_sme_prim.h"

#ifndef CSR_WIFI_AP_ENABLE
#error CSR_WIFI_AP_ENABLE MUST be defined inorder to use csr_wifi_sme_ap_prim.h
#endif

#define CSR_WIFI_SME_AP_PRIM                                            (0x0407)

typedef CsrPrim CsrWifiSmeApPrim;


/*                                                                              

      
                          

             
                                           

       
                                        
                                                                              
                                                                            

                                                                              */
typedef u8 CsrWifiSmeApAccessType;
#define CSR_WIFI_AP_ACCESS_TYPE_NONE    ((CsrWifiSmeApAccessType) 0x00)
#define CSR_WIFI_AP_ACCESS_TYPE_ALLOW   ((CsrWifiSmeApAccessType) 0x01)
#define CSR_WIFI_AP_ACCESS_TYPE_DENY    ((CsrWifiSmeApAccessType) 0x02)

/*                                                                              

      
                           

             
                                             

       
                                                       
                                                        
                                                    

                                                                              */
typedef u8 CsrWifiSmeApAuthSupport;
#define CSR_WIFI_SME_RSN_AUTH_WPAPSK    ((CsrWifiSmeApAuthSupport) 0x01)
#define CSR_WIFI_SME_RSN_AUTH_WPA2PSK   ((CsrWifiSmeApAuthSupport) 0x02)
#define CSR_WIFI_SME_AUTH_WAPIPSK       ((CsrWifiSmeApAuthSupport) 0x04)

/*                                                                              

      
                        

             
                                                   

       
                                         
                                         
                                      
                                                                               
                         
                                 
                                                                              

                                                                              */
typedef u8 CsrWifiSmeApAuthType;
#define CSR_WIFI_SME_AP_AUTH_TYPE_OPEN_SYSTEM   ((CsrWifiSmeApAuthType) 0x00)
#define CSR_WIFI_SME_AP_AUTH_TYPE_PERSONAL      ((CsrWifiSmeApAuthType) 0x01)
#define CSR_WIFI_SME_AP_AUTH_TYPE_WEP           ((CsrWifiSmeApAuthType) 0x02)

/*                                                                              

      
                         

             
                           

       
                                                 
                                                 

                                                                              */
typedef u8 CsrWifiSmeApDirection;
#define CSR_WIFI_AP_DIRECTION_RECEIPIENT   ((CsrWifiSmeApDirection) 0x00)
#define CSR_WIFI_AP_DIRECTION_ORIGINATOR   ((CsrWifiSmeApDirection) 0x01)

/*                                                                              

      
                          

             
                                              

       
                                                                               
                                            
                                           
                                           
                                           

                                                                              */
typedef u8 CsrWifiSmeApPhySupport;
#define CSR_WIFI_SME_AP_PHY_SUPPORT_A   ((CsrWifiSmeApPhySupport) 0x01)
#define CSR_WIFI_SME_AP_PHY_SUPPORT_B   ((CsrWifiSmeApPhySupport) 0x02)
#define CSR_WIFI_SME_AP_PHY_SUPPORT_G   ((CsrWifiSmeApPhySupport) 0x04)
#define CSR_WIFI_SME_AP_PHY_SUPPORT_N   ((CsrWifiSmeApPhySupport) 0x08)

/*                                                                              

      
                    

             
                          

       
                                       
                                                 

                                                                              */
typedef u8 CsrWifiSmeApType;
#define CSR_WIFI_AP_TYPE_LEGACY   ((CsrWifiSmeApType) 0x00)
#define CSR_WIFI_AP_TYPE_P2P      ((CsrWifiSmeApType) 0x01)


/*                                                                              

      
                               

             
                                                   

                                                                              */
typedef u8 CsrWifiSmeApAuthSupportMask;
/*                                                                              

      
                              

             
                                                                       

                                                                              */
typedef u8 CsrWifiSmeApPhySupportMask;
/*                                                                              

      
                               

             
                                    

                                                                              */
typedef u16 CsrWifiSmeApRsnCapabilities;
/*                                                                              

      
                                   

             
                                                                            

                                                                              */
typedef u16 CsrWifiSmeApRsnCapabilitiesMask;
/*                                                                              

      
                                

             
                                                                          
                   

                                                                              */
typedef u16 CsrWifiSmeApWapiCapabilities;
/*                                                                              

      
                                    

             
                                                                             

                                                                              */
typedef u16 CsrWifiSmeApWapiCapabilitiesMask;


/*                                                                              

      
                        

             
                                   

         
                                                                             
                                                                              
                                                                                
                                  
                                                                           
                                                                               
                                                                                
                                    
                                                                                
                                                                              
                                               
                                                                            
                                                                               
                                                                           
                                 
                                                                                
                                               
                                                                                
                                    
                                                     

                                                                              */
typedef struct
{
    u8  greenfieldSupported;
    u8  shortGi20MHz;
    u8 rxStbc;
    u8  rifsModeAllowed;
    u8 htProtection;
    u8  dualCtsProtection;
} CsrWifiSmeApHtParams;

/*                                                                              

      
                                     

             

         
                                                   
                                                            
                                            

                                                                              */
typedef struct
{
    u8  operatingClass;
    u8  operatingChannelCount;
    u8 *operatingChannel;
} CsrWifiSmeApP2pOperatingChanEntry;

/*                                                                              

      
                                    

             
                                                               

         
                                   
                                             
                                           

                                                                              */
typedef struct
{
    u8                           country[3];
    u8                           channelEntryListCount;
    CsrWifiSmeApP2pOperatingChanEntry *channelEntryList;
} CsrWifiSmeApP2pOperatingChanList;

/*                                                                              

      
                        

             

         
                        
                        
                        
                        

                                                                              */
typedef struct
{
    CsrWifiSmeApAuthSupportMask      authSupport;
    CsrWifiSmeEncryptionMask         encryptionModeMask;
    CsrWifiSmeApRsnCapabilitiesMask  rsnCapabilities;
    CsrWifiSmeApWapiCapabilitiesMask wapiCapabilities;
} CsrWifiSmeApAuthPers;

/*                                                                              

      
                         

             

         
                                                              
                                                                                
                                             
                                                                              
                                                             

                                                                              */
typedef struct
{
    CsrWifiMacAddress     peerMacAddress;
    u8              tid;
    CsrWifiSmeApDirection direction;
} CsrWifiSmeApBaSession;

/*                                                                              

      
                         

             
                                           

         
                                                              
                                                          
                                                                             
                                                                              
                                    
                                                                      
                                                                           
                                                                            
                                                 
                                                                         
                                    
                                                                             
                                                                     
                                                    
                                                                            
                                                                           
                                    
                                                                             
                                                                             
                                                                          
                                                                       
                                                                            
                                                      
                                                  
                                                
                                                                      
                                                                          
                                                   

                                                                              */
typedef struct
{
    CsrWifiSmeApPhySupportMask  phySupportedBitmap;
    u16                   beaconInterval;
    u8                    dtimPeriod;
    u16                   maxListenInterval;
    u8                    supportedRatesCount;
    u8                    supportedRates[20];
    CsrWifiSmePreambleType      preamble;
    u8                     shortSlotTimeEnabled;
    CsrWifiSmeCtsProtectionType ctsProtectionType;
    u8                     wmmEnabled;
    CsrWifiSmeWmmAcParams       wmmApParams[4];
    CsrWifiSmeWmmAcParams       wmmApBcParams[4];
    CsrWifiSmeApAccessType      accessType;
    u8                    macAddressListCount;
    CsrWifiMacAddress          *macAddressList;
    CsrWifiSmeApHtParams        apHtParams;
} CsrWifiSmeApMacConfig;

/*                                                                              

      
                           

             

         
                                                                    
                                                                          
                                                                           
                                                                              
                                                                             
                                                                              
                                        
                                                                           
                                                             
                                                                                
                                                                  
                                                                           
                                                              
                                                                       
                                                                           
                                                              
                                                                             
                                                                             
                                                                 
                                                                           
                                                              

                                                                              */
typedef struct
{
    CsrWifiSmeP2pGroupCapabilityMask groupCapability;
    CsrWifiSmeApP2pOperatingChanList operatingChanList;
    u8                          opPsEnabled;
    u8                         ctWindow;
    CsrWifiSmeP2pNoaConfigMethod     noaConfigMethod;
    u8                          allowNoaWithNonP2pDevices;
} CsrWifiSmeApP2pGoConfig;

/*                                                                              

      
                           

             

         
                                 
                                 
                                 
                                 
                                 

                                                                              */
typedef struct
{
    CsrWifiSmeApAuthType authType;
    union {
        CsrWifiSmeEmpty      openSystemEmpty;
        CsrWifiSmeWepAuth    authwep;
        CsrWifiSmeApAuthPers authPers;
    } smeAuthType;
} CsrWifiSmeApCredentials;

/*                                                                              

      
                         

             

         
                   
                   

                                                                              */
typedef struct
{
    CsrWifiSmeApCredentials apCredentials;
    u8                 wpsEnabled;
} CsrWifiSmeApSecConfig;


/*            */
#define CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST            (0x0000)

#define CSR_WIFI_SME_AP_BEACONING_START_REQ               ((CsrWifiSmeApPrim) (0x0000 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_BEACONING_STOP_REQ                ((CsrWifiSmeApPrim) (0x0001 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_WPS_REGISTRATION_STARTED_REQ      ((CsrWifiSmeApPrim) (0x0002 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_WPS_REGISTRATION_FINISHED_REQ     ((CsrWifiSmeApPrim) (0x0003 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_WMM_PARAM_UPDATE_REQ              ((CsrWifiSmeApPrim) (0x0004 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_STA_DISCONNECT_REQ                ((CsrWifiSmeApPrim) (0x0005 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_WPS_CONFIGURATION_REQ             ((CsrWifiSmeApPrim) (0x0006 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_ACTIVE_BA_GET_REQ                 ((CsrWifiSmeApPrim) (0x0007 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_BA_DELETE_REQ                     ((CsrWifiSmeApPrim) (0x0008 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST))


#define CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_HIGHEST           (0x0008 + CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST)

/*          */
#define CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_WIFI_SME_AP_BEACONING_START_CFM               ((CsrWifiSmeApPrim)(0x0000 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_BEACONING_STOP_CFM                ((CsrWifiSmeApPrim)(0x0001 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_STA_NOTIFY_IND                    ((CsrWifiSmeApPrim)(0x0002 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_STA_CONNECT_START_IND             ((CsrWifiSmeApPrim)(0x0003 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_WPS_REGISTRATION_STARTED_CFM      ((CsrWifiSmeApPrim)(0x0004 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_WPS_REGISTRATION_FINISHED_CFM     ((CsrWifiSmeApPrim)(0x0005 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_WMM_PARAM_UPDATE_CFM              ((CsrWifiSmeApPrim)(0x0006 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_STA_DISCONNECT_CFM                ((CsrWifiSmeApPrim)(0x0007 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_WPS_CONFIGURATION_CFM             ((CsrWifiSmeApPrim)(0x0008 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_ERROR_IND                         ((CsrWifiSmeApPrim)(0x0009 + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_ACTIVE_BA_GET_CFM                 ((CsrWifiSmeApPrim)(0x000A + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_SME_AP_BA_DELETE_CFM                     ((CsrWifiSmeApPrim)(0x000B + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST))

#define CSR_WIFI_SME_AP_PRIM_UPSTREAM_HIGHEST             (0x000B + CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST)

#define CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_COUNT             (CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_WIFI_SME_AP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_WIFI_SME_AP_PRIM_UPSTREAM_COUNT               (CSR_WIFI_SME_AP_PRIM_UPSTREAM_HIGHEST   + 1 - CSR_WIFI_SME_AP_PRIM_UPSTREAM_LOWEST)

/*                                                                              

      
                                 

             
                                                                   

         
                                                                      
                     
                                                                             
                                                  
                                                 
                                     
                           
                                     
                              
                                                           
                                                                               
                              
                                      
                                                                               

                                                                              */
typedef struct
{
    CsrWifiFsmEvent         common;
    u16               interfaceTag;
    u8                initialPresence;
    CsrWifiSmeApType        apType;
    u8                 cloakSsid;
    CsrWifiSsid             ssid;
    CsrWifiSmeRadioIF       ifIndex;
    u8                channel;
    u8                maxConnections;
    CsrWifiSmeApSecConfig   apCredentials;
    CsrWifiSmeApMacConfig   smeApConfig;
    CsrWifiSmeApP2pGoConfig p2pGoParam;
} CsrWifiSmeApBeaconingStartReq;

/*                                                                              

      
                                

             
                                                                 

         
                                                                   
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeApBeaconingStopReq;

/*                                                                              

      
                                         

             
                                                                        

         
                                                                               
                              
                              
                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent         common;
    u16               interfaceTag;
    CsrWifiSmeWpsDpid       SelectedDevicePasswordId;
    CsrWifiSmeWpsConfigType SelectedconfigMethod;
} CsrWifiSmeApWpsRegistrationStartedReq;

/*                                                                              

      
                                          

             
                                                                         

         
                                                                   
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeApWpsRegistrationFinishedReq;

/*                                                                              

      
                                 

             
                                                                           

         
                                                                    
                   
                                                                      
                                 
                                                                            

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    u16             interfaceTag;
    CsrWifiSmeWmmAcParams wmmApParams[4];
    CsrWifiSmeWmmAcParams wmmApBcParams[4];
} CsrWifiSmeApWmmParamUpdateReq;

/*                                                                              

      
                                

             
                                                                           
              

         
                                                                     
                    
                    
                    
                    
                                                                              
                                                                           
                                                                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrWifiSmeIEEE80211Reason deauthReason;
    CsrWifiSmeIEEE80211Reason disassocReason;
    CsrWifiMacAddress         peerMacaddress;
    u8                   keepBlocking;
} CsrWifiSmeApStaDisconnectReq;

/*                                                                              

      
                                   

             
                                                                             
                                               

         
                                                                
                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    CsrWifiSmeWpsConfig wpsConfig;
} CsrWifiSmeApWpsConfigurationReq;

/*                                                                              

      
                              

             
                                                                           
                

         
                                                                   
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiSmeApActiveBaGetReq;

/*                                                                              

      
                           

             
                                                                

         
                                                                   
                  
                  
                                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrWifiSmeIEEE80211Reason reason;
    CsrWifiSmeApBaSession     baSession;
} CsrWifiSmeApBaDeleteReq;

/*                                                                              

      
                                 

             
                                                                        
          

         
                                                                   
                  
                  
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
    u16       secIeLength;
    u8       *secIe;
} CsrWifiSmeApBeaconingStartCfm;

/*                                                                              

      
                                

             
                                                               

         
                                                                   
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeApBeaconingStopCfm;

/*                                                                              

      
                            

             
                                                                             
                                  

         
                                                                        
                       
                       
                       
                       
                       
                       
                       
                       
                       
                       
                       

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrWifiSmeMediaStatus     mediaStatus;
    CsrWifiMacAddress         peerMacAddress;
    CsrWifiMacAddress         peerDeviceAddress;
    CsrWifiSmeIEEE80211Reason disassocReason;
    CsrWifiSmeIEEE80211Reason deauthReason;
    CsrWifiSmeWpsRegistration WpsRegistration;
    u8                  secIeLength;
    u8                 *secIe;
    u8                  groupKeyId;
    u16                 seqNumber[8];
} CsrWifiSmeApStaNotifyInd;

/*                                                                              

      
                                  

             
                                                                             
            

         
                                                                     
                    
                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    u16         interfaceTag;
    CsrWifiMacAddress peerMacAddress;
} CsrWifiSmeApStaConnectStartInd;

/*                                                                              

      
                                         

             
                                                               

         
                                                                   
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeApWpsRegistrationStartedCfm;

/*                                                                              

      
                                          

             
                                                                

         
                                                                   
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeApWpsRegistrationFinishedCfm;

/*                                                                              

      
                                 

             
                                                          

         
                                                                   
                  
                  

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiSmeApWmmParamUpdateCfm;

/*                                                                              

      
                                

             
                                                       

         
                                                                     
                    
                    
                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    u16         interfaceTag;
    CsrResult         status;
    CsrWifiMacAddress peerMacaddress;
} CsrWifiSmeApStaDisconnectCfm;

/*                                                                              

      
                                   

             
            

         
                                                             
                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiSmeApWpsConfigurationCfm;

/*                                                                              

      
                        

             
                                                                        
                                                                       
                                                                          
                                                   

         
                                                                   
                            
                  
                                            

                                                                              */
typedef struct
{
    CsrWifiFsmEvent  common;
    u16        interfaceTag;
    CsrWifiSmeApType apType;
    CsrResult        status;
} CsrWifiSmeApErrorInd;

/*                                                                              

      
                              

             
                                                                            

         
                                                                       
                      
                                                        
                                                         
                                                                
                                                        

                                                                              */
typedef struct
{
    CsrWifiFsmEvent        common;
    u16              interfaceTag;
    CsrResult              status;
    u16              activeBaCount;
    CsrWifiSmeApBaSession *activeBaSessions;
} CsrWifiSmeApActiveBaGetCfm;

/*                                                                              

      
                           

             
                                             

         
                                                                   
                  
                                                    
                                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    u16             interfaceTag;
    CsrResult             status;
    CsrWifiSmeApBaSession baSession;
} CsrWifiSmeApBaDeleteCfm;


#endif /*                          */

