/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_NME_AP_PRIM_H__
#define CSR_WIFI_NME_AP_PRIM_H__

#include <linux/types.h>
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_wifi_common.h"
#include "csr_result.h"
#include "csr_wifi_fsm_event.h"
#include "csr_wifi_sme_ap_prim.h"
#include "csr_wifi_nme_prim.h"

#ifndef CSR_WIFI_NME_ENABLE
#error CSR_WIFI_NME_ENABLE MUST be defined inorder to use csr_wifi_nme_ap_prim.h
#endif
#ifndef CSR_WIFI_AP_ENABLE
#error CSR_WIFI_AP_ENABLE MUST be defined inorder to use csr_wifi_nme_ap_prim.h
#endif

#define CSR_WIFI_NME_AP_PRIM                                            (0x0426)

typedef CsrPrim CsrWifiNmeApPrim;


/*                                                                              

      
                                  

             
                        

       
                                       
                                           
                                              
                                                               

                                                                              */
typedef u8 CsrWifiNmeApPersCredentialType;
#define CSR_WIFI_NME_AP_CREDENTIAL_TYPE_PSK          ((CsrWifiNmeApPersCredentialType) 0x00)
#define CSR_WIFI_NME_AP_CREDENTIAL_TYPE_PASSPHRASE   ((CsrWifiNmeApPersCredentialType) 0x01)


/*                                                                              

      
                      

             
                                     

         
                                                       
                                                                               
                                                                              
                                                
                                        
                                                   

                                                                              */
typedef struct
{
    u16 apGroupkeyTimeout;
    u8   apStrictGtkRekey;
    u16 apGmkTimeout;
    u16 apResponseTimeout;
    u8  apRetransLimit;
} CsrWifiNmeApConfig;

/*                                                                              

      
                        

             

         
                                                                              
                                                        
                                                           
                                                            
                                                                              
                                                        
                                                                                
                                                                  
                                     
                                     

                                                                              */
typedef struct
{
    CsrWifiSmeApAuthSupportMask      authSupport;
    CsrWifiSmeApRsnCapabilitiesMask  rsnCapabilities;
    CsrWifiSmeApWapiCapabilitiesMask wapiCapabilities;
    CsrWifiNmeApPersCredentialType   pskOrPassphrase;
    union {
        CsrWifiNmePsk        psk;
        CsrWifiNmePassphrase passphrase;
    } authPers_credentials;
} CsrWifiNmeApAuthPers;

/*                                                                              

      
                           

             
                                              

         
                                                      
                                                            
                                  
                                  
                                  

                                                                              */
typedef struct
{
    CsrWifiSmeApAuthType authType;
    union {
        CsrWifiSmeEmpty      openSystemEmpty;
        CsrWifiSmeWepAuth    authwep;
        CsrWifiNmeApAuthPers authTypePersonal;
    } nmeAuthType;
} CsrWifiNmeApCredentials;


/*            */
#define CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_LOWEST            (0x0000)

#define CSR_WIFI_NME_AP_CONFIG_SET_REQ                    ((CsrWifiNmeApPrim) (0x0000 + CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_WPS_REGISTER_REQ                  ((CsrWifiNmeApPrim) (0x0001 + CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_START_REQ                         ((CsrWifiNmeApPrim) (0x0002 + CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_STOP_REQ                          ((CsrWifiNmeApPrim) (0x0003 + CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_WMM_PARAM_UPDATE_REQ              ((CsrWifiNmeApPrim) (0x0004 + CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_STA_REMOVE_REQ                    ((CsrWifiNmeApPrim) (0x0005 + CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_LOWEST))


#define CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_HIGHEST           (0x0005 + CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_LOWEST)

/*          */
#define CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_WIFI_NME_AP_CONFIG_SET_CFM                    ((CsrWifiNmeApPrim)(0x0000 + CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_WPS_REGISTER_CFM                  ((CsrWifiNmeApPrim)(0x0001 + CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_START_CFM                         ((CsrWifiNmeApPrim)(0x0002 + CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_STOP_CFM                          ((CsrWifiNmeApPrim)(0x0003 + CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_STOP_IND                          ((CsrWifiNmeApPrim)(0x0004 + CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_WMM_PARAM_UPDATE_CFM              ((CsrWifiNmeApPrim)(0x0005 + CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_AP_STATION_IND                       ((CsrWifiNmeApPrim)(0x0006 + CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST))

#define CSR_WIFI_NME_AP_PRIM_UPSTREAM_HIGHEST             (0x0006 + CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST)

#define CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_COUNT             (CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_WIFI_NME_AP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_WIFI_NME_AP_PRIM_UPSTREAM_COUNT               (CSR_WIFI_NME_AP_PRIM_UPSTREAM_HIGHEST   + 1 - CSR_WIFI_NME_AP_PRIM_UPSTREAM_LOWEST)

/*                                                                              

      
                            

             
                                                                            
                                                                      
                                               

         
                                                                  
                                               
                                                       
                                                        

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    CsrWifiNmeApConfig    apConfig;
    CsrWifiSmeApMacConfig apMacConfig;
} CsrWifiNmeApConfigSetReq;

/*                                                                              

      
                              

             
                                                                        
                                                                     
                                    

         
                                                                               
                                                                            
                                        
                                                     
                                                                     
                                         
                                                                               
                                                                               
                                                                             
                                        

                                                                              */
typedef struct
{
    CsrWifiFsmEvent         common;
    u16               interfaceTag;
    CsrWifiSmeWpsDpid       selectedDevicePasswordId;
    CsrWifiSmeWpsConfigType selectedConfigMethod;
    u8                pin[8];
} CsrWifiNmeApWpsRegisterReq;

/*                                                                              

      
                        

             
                                                            

         
                                                                     
                                                                            
                                                                        
                                                                             
                                                    
                                           
                                    
                                                         
                                                       
                                                                   
                                                
                                                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent         common;
    u16               interfaceTag;
    CsrWifiSmeApType        apType;
    u8                 cloakSsid;
    CsrWifiSsid             ssid;
    CsrWifiSmeRadioIF       ifIndex;
    u8                channel;
    CsrWifiNmeApCredentials apCredentials;
    u8                maxConnections;
    CsrWifiSmeApP2pGoConfig p2pGoParam;
    u8                 wpsEnabled;
} CsrWifiNmeApStartReq;

/*                                                                              

      
                       

             
                                                         

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiNmeApStopReq;

/*                                                                              

      
                                 

             
                                                                

         
                                                                    
                                                                              
                                                
                                                                               
                                                                     
                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    CsrWifiSmeWmmAcParams wmmApParams[4];
    CsrWifiSmeWmmAcParams wmmApBcParams[4];
} CsrWifiNmeApWmmParamUpdateReq;

/*                                                                              

      
                            

             
                                                                             
                                                                      
                                                                        
                                            

         
                                                                    
                                                                           
                                                                            
                                                                                
                                                                               
                                                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    u16         interfaceTag;
    CsrWifiMacAddress staMacAddress;
    u8           keepBlocking;
} CsrWifiNmeApStaRemoveReq;

/*                                                                              

      
                            

             
                                                     

         
                                                             
                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiNmeApConfigSetCfm;

/*                                                                              

      
                              

             
                                                       

         
                                                                   
                                                                          
                                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiNmeApWpsRegisterCfm;

/*                                                                              

      
                        

             
                                                                       

         
                                                                   
                                                                          
                                         
                                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
    CsrWifiSsid     ssid;
} CsrWifiNmeApStartCfm;

/*                                                                              

      
                       

             
                                                                            
                                                                      
                        

         
                                                                   
                                                                          
                                         

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiNmeApStopCfm;

/*                                                                              

      
                       

             
                                                                         
                                                                            
                                                                           

         
                                                                   
                                                                          
                                                
                               

                                                                              */
typedef struct
{
    CsrWifiFsmEvent  common;
    u16        interfaceTag;
    CsrWifiSmeApType apType;
    CsrResult        status;
} CsrWifiNmeApStopInd;

/*                                                                              

      
                                 

             
                                               

         
                                                             
                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiNmeApWmmParamUpdateCfm;

/*                                                                              

      
                          

             
                                                                             
                                  

         
                                                                        
                                                                               
                                                                     
                                    
                                                  
                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    u16             interfaceTag;
    CsrWifiSmeMediaStatus mediaStatus;
    CsrWifiMacAddress     peerMacAddress;
    CsrWifiMacAddress     peerDeviceAddress;
} CsrWifiNmeApStationInd;

#endif /*                          */

