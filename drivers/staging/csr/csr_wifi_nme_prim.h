/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*                                       */

#ifndef CSR_WIFI_NME_PRIM_H__
#define CSR_WIFI_NME_PRIM_H__

#include <linux/types.h>
#include "csr_prim_defs.h"
#include "csr_sched.h"
#include "csr_wifi_common.h"
#include "csr_result.h"
#include "csr_wifi_fsm_event.h"
#include "csr_wifi_sme_prim.h"

#ifndef CSR_WIFI_NME_ENABLE
#error CSR_WIFI_NME_ENABLE MUST be defined inorder to use csr_wifi_nme_prim.h
#endif

#define CSR_WIFI_NME_PRIM                                               (0x0424)

typedef CsrPrim CsrWifiNmePrim;

typedef void (*CsrWifiNmeFrameFreeFunction)(void *frame);

/*                                                                              

      
                      

             
                            

       
                                     
                                                                                
                                                                
                                       
                                                      
                                    
                                                                  
                                       
                                                                           
                                     
                                                                   
                                        
                                                                            
                                     
                                                        
                                   
                                                                   
                                      
                                                                            
                                        
                                    

                                                                              */
typedef u16 CsrWifiNmeAuthMode;
#define CSR_WIFI_NME_AUTH_MODE_80211_OPEN      ((CsrWifiNmeAuthMode) 0x0001)
#define CSR_WIFI_NME_AUTH_MODE_80211_SHARED    ((CsrWifiNmeAuthMode) 0x0002)
#define CSR_WIFI_NME_AUTH_MODE_8021X_WPA       ((CsrWifiNmeAuthMode) 0x0004)
#define CSR_WIFI_NME_AUTH_MODE_8021X_WPAPSK    ((CsrWifiNmeAuthMode) 0x0008)
#define CSR_WIFI_NME_AUTH_MODE_8021X_WPA2      ((CsrWifiNmeAuthMode) 0x0010)
#define CSR_WIFI_NME_AUTH_MODE_8021X_WPA2PSK   ((CsrWifiNmeAuthMode) 0x0020)
#define CSR_WIFI_NME_AUTH_MODE_8021X_CCKM      ((CsrWifiNmeAuthMode) 0x0040)
#define CSR_WIFI_NME_AUTH_MODE_WAPI_WAI        ((CsrWifiNmeAuthMode) 0x0080)
#define CSR_WIFI_NME_AUTH_MODE_WAPI_WAIPSK     ((CsrWifiNmeAuthMode) 0x0100)
#define CSR_WIFI_NME_AUTH_MODE_8021X_OTHER1X   ((CsrWifiNmeAuthMode) 0x0200)

/*                                                                              

      
                     

             
               

       
                                        
                                                                               
                                                  
                               
                                                                              
                                                                                
                                             
                                  
                                                             
                             
                                           

                                                                              */
typedef u8 CsrWifiNmeBssType;
#define CSR_WIFI_NME_BSS_TYPE_INFRASTRUCTURE   ((CsrWifiNmeBssType) 0x00)
#define CSR_WIFI_NME_BSS_TYPE_ADHOC            ((CsrWifiNmeBssType) 0x01)
#define CSR_WIFI_NME_BSS_TYPE_RESERVED         ((CsrWifiNmeBssType) 0x02)
#define CSR_WIFI_NME_BSS_TYPE_P2P              ((CsrWifiNmeBssType) 0x03)

/*                                                                              

      
                            

             
                                                                           

       
                                                        
                                                          

                                                                              */
typedef u8 CsrWifiNmeCcxOptionsMask;
#define CSR_WIFI_NME_CCX_OPTION_NONE   ((CsrWifiNmeCcxOptionsMask) 0x00)
#define CSR_WIFI_NME_CCX_OPTION_CCKM   ((CsrWifiNmeCcxOptionsMask) 0x01)

/*                                                                              

      
                          

             

       
                                    
                                    
                                    

                                                                              */
typedef u8 CsrWifiNmeConfigAction;
#define CSR_WIFI_PIN_ENTRY_PUSH_BUTTON   ((CsrWifiNmeConfigAction) 0x00)
#define CSR_WIFI_PIN_ENTRY_DISPLAY_PIN   ((CsrWifiNmeConfigAction) 0x01)
#define CSR_WIFI_PIN_ENTRY_ENTER_PIN     ((CsrWifiNmeConfigAction) 0x02)

/*                                                                              

      
                              

             
                                                                            

       
                                                                 
                                         
                                                               
                                                         
                                                                   
                                                                                
                                                              
                                      
                                                                  
                                                            

                                                                              */
typedef u8 CsrWifiNmeConnectionStatus;
#define CSR_WIFI_NME_CONNECTION_STATUS_CONNECTION_STATUS_DISCONNECTED     ((CsrWifiNmeConnectionStatus) 0x00)
#define CSR_WIFI_NME_CONNECTION_STATUS_CONNECTION_STATUS_CONNECTING       ((CsrWifiNmeConnectionStatus) 0x01)
#define CSR_WIFI_NME_CONNECTION_STATUS_CONNECTION_STATUS_AUTHENTICATING   ((CsrWifiNmeConnectionStatus) 0x02)
#define CSR_WIFI_NME_CONNECTION_STATUS_CONNECTION_STATUS_CONNECTED        ((CsrWifiNmeConnectionStatus) 0x03)
#define CSR_WIFI_NME_CONNECTION_STATUS_CONNECTION_STATUS_DISCONNECTING    ((CsrWifiNmeConnectionStatus) 0x04)

/*                                                                              

      
                            

             
                        

       
                                            
                                                 
                                      
                                           
                                       
                                            
                                        
                                                       
                                               
                                                    
                                         
                                                         
                                                
                                                     
                                         
                                                         
                                                
                                                     
                                     
                                                      
                                      
                                                                         
                                                 

                                                                              */
typedef u16 CsrWifiNmeCredentialType;
#define CSR_WIFI_NME_CREDENTIAL_TYPE_OPEN_SYSTEM       ((CsrWifiNmeCredentialType) 0x0000)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_WEP64             ((CsrWifiNmeCredentialType) 0x0001)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_WEP128            ((CsrWifiNmeCredentialType) 0x0002)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_WPA_PSK           ((CsrWifiNmeCredentialType) 0x0003)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_WPA_PASSPHRASE    ((CsrWifiNmeCredentialType) 0x0004)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_WPA2_PSK          ((CsrWifiNmeCredentialType) 0x0005)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_WPA2_PASSPHRASE   ((CsrWifiNmeCredentialType) 0x0006)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_WAPI_PSK          ((CsrWifiNmeCredentialType) 0x0007)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_WAPI_PASSPHRASE   ((CsrWifiNmeCredentialType) 0x0008)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_WAPI              ((CsrWifiNmeCredentialType) 0x0009)
#define CSR_WIFI_NME_CREDENTIAL_TYPE_8021X             ((CsrWifiNmeCredentialType) 0x000A)

/*                                                                              

      
                       

             
                                                

       
                               
                                    
                                         
                                                   
                                    
                                              
                                         
                                                   
                               
                                    
                               
                                    
                                    
                                              
                                         
                                                   
                                
                                     

                                                                              */
typedef u16 CsrWifiNmeEapMethod;
#define CSR_WIFI_NME_EAP_METHOD_TLS             ((CsrWifiNmeEapMethod) 0x0001)
#define CSR_WIFI_NME_EAP_METHOD_TTLS_MSCHAPV2   ((CsrWifiNmeEapMethod) 0x0002)
#define CSR_WIFI_NME_EAP_METHOD_PEAP_GTC        ((CsrWifiNmeEapMethod) 0x0004)
#define CSR_WIFI_NME_EAP_METHOD_PEAP_MSCHAPV2   ((CsrWifiNmeEapMethod) 0x0008)
#define CSR_WIFI_NME_EAP_METHOD_SIM             ((CsrWifiNmeEapMethod) 0x0010)
#define CSR_WIFI_NME_EAP_METHOD_AKA             ((CsrWifiNmeEapMethod) 0x0020)
#define CSR_WIFI_NME_EAP_METHOD_FAST_GTC        ((CsrWifiNmeEapMethod) 0x0040)
#define CSR_WIFI_NME_EAP_METHOD_FAST_MSCHAPV2   ((CsrWifiNmeEapMethod) 0x0080)
#define CSR_WIFI_NME_EAP_METHOD_LEAP            ((CsrWifiNmeEapMethod) 0x0100)

/*                                                                              

      
                        

             
                          

       
                                       
                                       
                                                 
                                                                     
                                                  
                                                                      
                                                
                                                             
                                                
                                                             
                                                
                                                             
                                              
                                                             
                                               
                                                              
                                             
                                                     
                                             
                                                    
                                             
                                                     

                                                                              */
typedef u16 CsrWifiNmeEncryption;
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_NONE              ((CsrWifiNmeEncryption) 0x0000)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_PAIRWISE_WEP40    ((CsrWifiNmeEncryption) 0x0001)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_PAIRWISE_WEP104   ((CsrWifiNmeEncryption) 0x0002)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_PAIRWISE_TKIP     ((CsrWifiNmeEncryption) 0x0004)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_PAIRWISE_CCMP     ((CsrWifiNmeEncryption) 0x0008)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_PAIRWISE_SMS4     ((CsrWifiNmeEncryption) 0x0010)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_GROUP_WEP40       ((CsrWifiNmeEncryption) 0x0020)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_GROUP_WEP104      ((CsrWifiNmeEncryption) 0x0040)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_GROUP_TKIP        ((CsrWifiNmeEncryption) 0x0080)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_GROUP_CCMP        ((CsrWifiNmeEncryption) 0x0100)
#define CSR_WIFI_NME_ENCRYPTION_CIPHER_GROUP_SMS4        ((CsrWifiNmeEncryption) 0x0200)

/*                                                                              

      
                         

             
                   

       
                                           
                                               
                                        
                                            
                                              
                                                        
                                                
                                                    
                                                 
                                                     
                                               
                                                   
                                                   
                                                       
                                               
                                                   
                                             
                                             
                                             
                                                       
                                
                                                                   

                                                                              */
typedef u32 CsrWifiNmeIndications;
#define CSR_WIFI_NME_INDICATIONS_IND_AP_STATION           ((CsrWifiNmeIndications) 0x00100000)
#define CSR_WIFI_NME_INDICATIONS_IND_AP_STOP              ((CsrWifiNmeIndications) 0x00200000)
#define CSR_WIFI_NME_INDICATIONS_IND_SIM_UMTS_AUTH        ((CsrWifiNmeIndications) 0x01000000)
#define CSR_WIFI_NME_INDICATIONS_IND_P2P_GROUP_START      ((CsrWifiNmeIndications) 0x02000000)
#define CSR_WIFI_NME_INDICATIONS_IND_P2P_GROUP_STATUS     ((CsrWifiNmeIndications) 0x04000000)
#define CSR_WIFI_NME_INDICATIONS_IND_P2P_GROUP_ROLE       ((CsrWifiNmeIndications) 0x08000000)
#define CSR_WIFI_NME_INDICATIONS_IND_PROFILE_DISCONNECT   ((CsrWifiNmeIndications) 0x10000000)
#define CSR_WIFI_NME_INDICATIONS_IND_PROFILE_UPDATE       ((CsrWifiNmeIndications) 0x20000000)
#define CSR_WIFI_NME_INDICATIONS_IND_SIM_IMSI_GET         ((CsrWifiNmeIndications) 0x40000000)
#define CSR_WIFI_NME_INDICATIONS_IND_SIM_GSM_AUTH         ((CsrWifiNmeIndications) 0x80000000)
#define CSR_WIFI_NME_INDICATIONS_ALL                      ((CsrWifiNmeIndications) 0xFFFFFFFF)

/*                                                                              

      
                      

             
                       
                                                      

       
                                            
                                            

                                                                              */
typedef u8 CsrWifiNmeSecError;
#define CSR_WIFI_NME_SEC_ERROR_SEC_ERROR_UNKNOWN   ((CsrWifiNmeSecError) 0x00)

/*                                                                              

      
                         

             
                               

       
                                                                            
                                                          
                                                                               
                                                                             
                                                                       
                                                                              
                                                                   

                                                                              */
typedef u8 CsrWifiNmeSimCardType;
#define CSR_WIFI_NME_SIM_CARD_TYPE_2G     ((CsrWifiNmeSimCardType) 0x01)
#define CSR_WIFI_NME_SIM_CARD_TYPE_3G     ((CsrWifiNmeSimCardType) 0x02)
#define CSR_WIFI_NME_SIM_CARD_TYPE_2G3G   ((CsrWifiNmeSimCardType) 0x03)

/*                                                                              

      
                            

             
                                                                       
                                                        

       
                                         
                                                                              
                                                            
                                           
                                                                               
                                                                               
                                                                                
                                                   
                                        
                                                                                
                                                                        
                             

                                                                              */
typedef u8 CsrWifiNmeUmtsAuthResult;
#define CSR_WIFI_NME_UMTS_AUTH_RESULT_SUCCESS     ((CsrWifiNmeUmtsAuthResult) 0x00)
#define CSR_WIFI_NME_UMTS_AUTH_RESULT_SYNC_FAIL   ((CsrWifiNmeUmtsAuthResult) 0x01)
#define CSR_WIFI_NME_UMTS_AUTH_RESULT_REJECT      ((CsrWifiNmeUmtsAuthResult) 0x02)

/*                                                                              

      
                        

             
                                                                             
                                                                             
                                                                           
                    

       
                                           
                                                            
                                   
                                                                        
                                   
                                                                        
                                   
                                                                             
                                   
                                                                              
                                           
                                                                          
                                                      
                                            
                                                                               
                                           
                                                                               

                                                                              */
typedef u8 CsrWifiNmeWmmQosInfo;
#define CSR_WIFI_NME_WMM_QOS_INFO_AC_MAX_SP_ALL    ((CsrWifiNmeWmmQosInfo) 0x00)
#define CSR_WIFI_NME_WMM_QOS_INFO_AC_VO            ((CsrWifiNmeWmmQosInfo) 0x01)
#define CSR_WIFI_NME_WMM_QOS_INFO_AC_VI            ((CsrWifiNmeWmmQosInfo) 0x02)
#define CSR_WIFI_NME_WMM_QOS_INFO_AC_BK            ((CsrWifiNmeWmmQosInfo) 0x04)
#define CSR_WIFI_NME_WMM_QOS_INFO_AC_BE            ((CsrWifiNmeWmmQosInfo) 0x08)
#define CSR_WIFI_NME_WMM_QOS_INFO_AC_MAX_SP_TWO    ((CsrWifiNmeWmmQosInfo) 0x20)
#define CSR_WIFI_NME_WMM_QOS_INFO_AC_MAX_SP_FOUR   ((CsrWifiNmeWmmQosInfo) 0x40)
#define CSR_WIFI_NME_WMM_QOS_INFO_AC_MAX_SP_SIX    ((CsrWifiNmeWmmQosInfo) 0x60)


/*                                                                              

      
                           

             
                                                                     

                                                                              */
typedef u16 CsrWifiNmeEapMethodMask;
/*                                                                              

      
                            

             
                                                                     

                                                                              */
typedef u16 CsrWifiNmeEncryptionMask;
/*                                                                              

      
                             

             
                                                                      

                                                                              */
typedef u32 CsrWifiNmeIndicationsMask;
/*                                                                              

      
                                

             
                                                                          
                                                                           
                        

                                                                              */
typedef u32 CsrWifiNmeNmeIndicationsMask;
/*                                                                              

      
                            

             
                                                                     

                                                                              */
typedef u8 CsrWifiNmeWmmQosInfoMask;


/*                                                                              

      
                   

             
                                                                  

         
                                                                   

                                                                              */
typedef struct
{
    u8 empty;
} CsrWifiNmeEmpty;

/*                                                                              

      
                        

             
                                                 

         
                                                                        
                                             

                                                                              */
typedef struct
{
    u16      encryptionMode;
    char *passphrase;
} CsrWifiNmePassphrase;

/*                                                                              

      
                 

             
                                              

         
                                                                        
                                          

                                                                              */
typedef struct
{
    u16 encryptionMode;
    u8  psk[32];
} CsrWifiNmePsk;

/*                                                                              

      
                             

             
                                            

         
                                                                              
                                                                          
                                                   
                                                                       
                                                                 
                                                                                
                                      
                                                                               
                                                                         
                                                                               
                                                                               
                                           

                                                                              */
typedef struct
{
    u32 certificateLength;
    u8 *certificate;
    u16 privateKeyLength;
    u8 *privateKey;
    u32 caCertificateLength;
    u8 *caCertificate;
} CsrWifiNmeWapiCredentials;

/*                                                                              

      
                            

             
                                              

         
                                                                              
                       
                                                                               
                                       
                                                                              
                               

                                                                              */
typedef struct
{
    CsrWifiMacAddress  bssid;
    CsrResult          status;
    CsrWifiNmeSecError securityError;
} CsrWifiNmeConnectAttempt;

/*                                                                              

      
                            

             
                                                                        
                                                                        

         
                 
                                                      
                                                                    
                                                      
            
                                                                               
                                                                                
                                                                               
                                                              
                  
                                                                           
                                                                              
                                                                                
                                                                                
            
                                                                                
                
                                                                             
                                
                          
                                                                              
                                                           
                           
                                                                            
                                                     
                     
                                                                          
                                                                
                                         
                                                                           
                                                                            
                                   
                                                                          
                                                                       
                                                                             
                                                                               
                                                                   
                    
                                                                               
                                           
              
                                                                                
                                                              
                      
                                                                  
                 
                                                                             
                                                            
           
                                                                             
                                                                      
                        
                                                                         
                                                      
                                            
             
                                                                               
                                                                              
                                       
       
                                                                               
                                                                      
               
                                                                            
                          

                                                                              */
typedef struct
{
    CsrWifiNmeEapMethodMask  eapMethodMask;
    CsrWifiSmeAuthModeMask   authMode;
    CsrWifiNmeEncryptionMask encryptionMode;
    char           *userName;
    char           *userPassword;
    char           *authServerUserIdentity;
    u32                clientCertificateLength;
    u8                *clientCertificate;
    u32                certificateAuthorityCertificateLength;
    u8                *certificateAuthorityCertificate;
    u16                privateKeyLength;
    u8                *privateKey;
    char           *privateKeyPassword;
    u32                sessionLength;
    u8                *session;
    u8                  allowPacProvisioning;
    u32                pacLength;
    u8                *pac;
    char           *pacPassword;
} CsrWifiNmeEapCredentials;

/*                                                                              

      
                        

             
                                       

         
                         
                         
                         

                                                                              */
typedef struct
{
    CsrWifiMacAddress                p2pDeviceId;
    CsrWifiSmeP2pGroupCapabilityMask groupCapabilityMask;
    u8                         groupOwnerIntent;
} CsrWifiNmePeerConfig;

/*                                                                              

      
                             

             
                                                                            
             

         
                                                                               
                                                 
                            

                                                                              */
typedef struct
{
    CsrWifiMacAddress bssid;
    CsrWifiSsid       ssid;
} CsrWifiNmeProfileIdentity;

/*                                                                              

      
                        

             
                                                                           
                      

         
                                                                                
                                                                       
                                            
                                            
                                            
                                            
                                            

                                                                              */
typedef struct
{
    CsrWifiSmeAuthModeMask wepAuthType;
    u8               selectedWepKey;
    u8               key1[13];
    u8               key2[13];
    u8               key3[13];
    u8               key4[13];
} CsrWifiNmeWep128Keys;

/*                                                                              

      
                       

             
                                                                          
                          

         
                                                                                
                                                                       
                                            
                                            
                                            
                                            
                                            

                                                                              */
typedef struct
{
    CsrWifiSmeAuthModeMask wepAuthType;
    u8               selectedWepKey;
    u8               key1[5];
    u8               key2[5];
    u8               key3[5];
    u8               key4[5];
} CsrWifiNmeWep64Keys;

/*                                                                              

      
                         

             
                                              

         
                                                                        
                                                  
                                                                             
                                                         
                               
                               
                               
                               
                               
                               
                               
                               
                               
                               
                               

                                                                              */
typedef struct
{
    CsrWifiNmeCredentialType credentialType;
    union {
        CsrWifiNmeEapCredentials  eap;
        CsrWifiNmePassphrase      wapiPassphrase;
        CsrWifiNmePassphrase      wpa2Passphrase;
        CsrWifiNmePsk             wpa2Psk;
        CsrWifiNmePsk             wapiPsk;
        CsrWifiNmePassphrase      wpaPassphrase;
        CsrWifiNmeWapiCredentials wapi;
        CsrWifiNmeWep128Keys      wep128Key;
        CsrWifiNmePsk             wpaPsk;
        CsrWifiNmeEmpty           openSystem;
        CsrWifiNmeWep64Keys       wep64Key;
    } credential;
} CsrWifiNmeCredentials;

/*                                                                              

      
                     

             
                                          

         
                                       
                                                   
                                                            
                                     
                                                                   
                                                                     
                                          
                                       

                                                                              */
typedef struct
{
    CsrWifiNmeProfileIdentity profileIdentity;
    CsrWifiNmeWmmQosInfoMask  wmmQosInfoMask;
    CsrWifiNmeBssType         bssType;
    u8                  channelNo;
    u8                  ccxOptionsMask;
    u8                   cloakedSsid;
    CsrWifiNmeCredentials     credentials;
} CsrWifiNmeProfile;


/*            */
#define CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST            (0x0000)

#define CSR_WIFI_NME_PROFILE_SET_REQ                      ((CsrWifiNmePrim) (0x0000 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_DELETE_REQ                   ((CsrWifiNmePrim) (0x0001 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_DELETE_ALL_REQ               ((CsrWifiNmePrim) (0x0002 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_ORDER_SET_REQ                ((CsrWifiNmePrim) (0x0003 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_CONNECT_REQ                  ((CsrWifiNmePrim) (0x0004 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_WPS_REQ                              ((CsrWifiNmePrim) (0x0005 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_WPS_CANCEL_REQ                       ((CsrWifiNmePrim) (0x0006 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_CONNECTION_STATUS_GET_REQ            ((CsrWifiNmePrim) (0x0007 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_SIM_IMSI_GET_RES                     ((CsrWifiNmePrim) (0x0008 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_SIM_GSM_AUTH_RES                     ((CsrWifiNmePrim) (0x0009 + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_SIM_UMTS_AUTH_RES                    ((CsrWifiNmePrim) (0x000A + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_WPS_CONFIG_SET_REQ                   ((CsrWifiNmePrim) (0x000B + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))
#define CSR_WIFI_NME_EVENT_MASK_SET_REQ                   ((CsrWifiNmePrim) (0x000C + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST))


#define CSR_WIFI_NME_PRIM_DOWNSTREAM_HIGHEST           (0x000C + CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST)

/*          */
#define CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST              (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_WIFI_NME_PROFILE_SET_CFM                      ((CsrWifiNmePrim)(0x0000 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_DELETE_CFM                   ((CsrWifiNmePrim)(0x0001 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_DELETE_ALL_CFM               ((CsrWifiNmePrim)(0x0002 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_ORDER_SET_CFM                ((CsrWifiNmePrim)(0x0003 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_CONNECT_CFM                  ((CsrWifiNmePrim)(0x0004 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_WPS_CFM                              ((CsrWifiNmePrim)(0x0005 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_WPS_CANCEL_CFM                       ((CsrWifiNmePrim)(0x0006 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_CONNECTION_STATUS_GET_CFM            ((CsrWifiNmePrim)(0x0007 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_UPDATE_IND                   ((CsrWifiNmePrim)(0x0008 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_PROFILE_DISCONNECT_IND               ((CsrWifiNmePrim)(0x0009 + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_SIM_IMSI_GET_IND                     ((CsrWifiNmePrim)(0x000A + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_SIM_GSM_AUTH_IND                     ((CsrWifiNmePrim)(0x000B + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_SIM_UMTS_AUTH_IND                    ((CsrWifiNmePrim)(0x000C + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_WPS_CONFIG_SET_CFM                   ((CsrWifiNmePrim)(0x000D + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))
#define CSR_WIFI_NME_EVENT_MASK_SET_CFM                   ((CsrWifiNmePrim)(0x000E + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST))

#define CSR_WIFI_NME_PRIM_UPSTREAM_HIGHEST             (0x000E + CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST)

#define CSR_WIFI_NME_PRIM_DOWNSTREAM_COUNT             (CSR_WIFI_NME_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_WIFI_NME_PRIM_DOWNSTREAM_LOWEST)
#define CSR_WIFI_NME_PRIM_UPSTREAM_COUNT               (CSR_WIFI_NME_PRIM_UPSTREAM_HIGHEST   + 1 - CSR_WIFI_NME_PRIM_UPSTREAM_LOWEST)

/*                                                                              

      
                           

             
                                                                             
                                                                             
                                                                         
                                                                            
                                                                             
                                                                

         
                                                              
                                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    CsrWifiNmeProfile profile;
} CsrWifiNmeProfileSetReq;

/*                                                                              

      
                              

             
                                                                             
                           

         
                                                                      
                                                                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    CsrWifiNmeProfileIdentity profileIdentity;
} CsrWifiNmeProfileDeleteReq;

/*                                                                              

      
                                 

             
                                                                    
                           

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiNmeProfileDeleteAllReq;

/*                                                                              

      
                                

             
                                                                          
                                               
                                                                            
                                                                           
              
                                                                            
                                                  
                                                                           
                                                                         
                                                                   

         
                                                                            
                                                                         
                                     
                                                                          
                                                                     

                                                                              */
typedef struct
{
    CsrWifiFsmEvent            common;
    u16                  interfaceTag;
    u8                   profileIdentitysCount;
    CsrWifiNmeProfileIdentity *profileIdentitys;
} CsrWifiNmeProfileOrderSetReq;

/*                                                                              

      
                               

             
                                                                    
                                             

         
                                                                      
                                                                             
                                                                           
                                                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrWifiNmeProfileIdentity profileIdentity;
} CsrWifiNmeProfileConnectReq;

/*                                                                              

      
                    

             
                                                                           
                                                                           
                                                                            
                                                                             
                                                                          

         
                                                                   
                                                                          
                             
                                         
                                                                                
                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    u16         interfaceTag;
    u8          pin[8];
    CsrWifiSsid       ssid;
    CsrWifiMacAddress bssid;
} CsrWifiNmeWpsReq;

/*                                                                              

      
                          

             
                                                                     
                                                                         
                                    

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiNmeWpsCancelReq;

/*                                                                              

      
                                    

             
                                                      

         
                                                                   
                                                                          

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
} CsrWifiNmeConnectionStatusGetReq;

/*                                                                              

      
                           

             
                                                                         

         
                                                               
                                                                               
                               
                                                            
                                                                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent       common;
    CsrResult             status;
    char        *imsi;
    CsrWifiNmeSimCardType cardType;
} CsrWifiNmeSimImsiGetRes;

/*                                                                              

      
                           

             
                                                                             
                                                                       
                                                                            
                                                                           
                                                                      
                                                                             
                                                                          
               

         
                                                                 
                                                                  
                                               
                                                                          
                                                    
                                                                           
                                                        

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
    u8        kcsLength;
    u8       *kcs;
    u8        sresLength;
    u8       *sres;
} CsrWifiNmeSimGsmAuthRes;

/*                                                                              

      
                            

             
                                                                          
                                                                         
                                                                           
                                             
                                      

         
                                                                         
                                                                          
                                                        
                                                                              
                                                                               
                                                                              
                                                    
                                                                              
                              
                                                                             
                                  
                                                                               
                               
                                                                            
                              
                                                                             
                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent          common;
    CsrResult                status;
    CsrWifiNmeUmtsAuthResult result;
    u8                 umtsCipherKey[16];
    u8                 umtsIntegrityKey[16];
    u8                 resParameterLength;
    u8                *resParameter;
    u8                 auts[14];
} CsrWifiNmeSimUmtsAuthRes;

/*                                                                              

      
                             

             
                                                                             
                                               

         
                                                                
                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent     common;
    CsrWifiSmeWpsConfig wpsConfig;
} CsrWifiNmeWpsConfigSetReq;

/*                                                                              

      
                             

             
                                                                         
                                                                            
                                                                       
                                                                         

         
                                                              
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    CsrWifiNmeIndicationsMask indMask;
} CsrWifiNmeEventMaskSetReq;

/*                                                                              

      
                           

             
                                                                             
                                                                           
                                                                       
                                                                          
                                                                           
                              

         
                                                             
                                                                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiNmeProfileSetCfm;

/*                                                                              

      
                              

             
                                                              
                                                                          

         
                                                             
                                                                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiNmeProfileDeleteCfm;

/*                                                                              

      
                                 

             
                                                                  
                                               

         
                                                             
                                                                               
                                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiNmeProfileDeleteAllCfm;

/*                                                                              

      
                                

             
                                                        

         
                                                                   
                                                                          
                                                                      
                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiNmeProfileOrderSetCfm;

/*                                                                              

      
                               

             
                                                                          
                                                                     
                                                                        

         
                                                                           
                                                                        
                                    
                                                                              
                                     
                                                             
                                                               
                                                                              
                                         
                                                             
                                                               
                                                                            
                                                       
                                                                              
                                                                             
                                                                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    CsrResult                 status;
    u8                  connectAttemptsCount;
    CsrWifiNmeConnectAttempt *connectAttempts;
} CsrWifiNmeProfileConnectCfm;

/*                                                                              

      
                    

             
                                          
                                                                      
                                       

         
                                                                   
                                                                          
                                                                      
                             
                                                     
                                                       
                                                               

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    u16         interfaceTag;
    CsrResult         status;
    CsrWifiNmeProfile profile;
} CsrWifiNmeWpsCfm;

/*                                                                              

      
                          

             
                                                                            

         
                                                                   
                                                                          
                                                           

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u16       interfaceTag;
    CsrResult       status;
} CsrWifiNmeWpsCancelCfm;

/*                                                                              

      
                                    

             
                                             

         
                                                                       
                                                                              
                                                                          
                                 
                                                    

                                                                              */
typedef struct
{
    CsrWifiFsmEvent            common;
    u16                  interfaceTag;
    CsrResult                  status;
    CsrWifiNmeConnectionStatus connectionStatus;
} CsrWifiNmeConnectionStatusGetCfm;

/*                                                                              

      
                              

             
                                                                      
                                                                            
            
                                                                             
                                   
                                                                         
        

         
                                                                   
                                                                          
                                                               

                                                                              */
typedef struct
{
    CsrWifiFsmEvent   common;
    u16         interfaceTag;
    CsrWifiNmeProfile profile;
} CsrWifiNmeProfileUpdateInd;

/*                                                                              

      
                                  

             
                                                                      
                                                                      
                                                                        
                                                                          
                           

         
                                                                           
                                                                        
                                    
                                                                              
                                         
                                                                            
                                                       
                                                                              
                                                                             
                                                                 

                                                                              */
typedef struct
{
    CsrWifiFsmEvent           common;
    u16                 interfaceTag;
    u8                  connectAttemptsCount;
    CsrWifiNmeConnectAttempt *connectAttempts;
} CsrWifiNmeProfileDisconnectInd;

/*                                                                              

      
                           

             
                                                                      
                                                                           
                                                                           
                                                                       
                                                                             
                                                                            
         

         
                                                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
} CsrWifiNmeSimImsiGetInd;

/*                                                                              

      
                           

             
                                                                      
                                                               
                                                                          
                                                                            
                                                                       
                                                                           
                                                                           
                                                                         
                             

         
                                                                  
                                                                               
                                  
                                      

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u8        randsLength;
    u8       *rands;
} CsrWifiNmeSimGsmAuthInd;

/*                                                                              

      
                            

             
                                                                      
                                                                
                                                                          
                                                                            
                                                                       
                                                                           
                                                                   
                                                                            
             

         
                                                             
                             
                             

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    u8        rand[16];
    u8        autn[16];
} CsrWifiNmeSimUmtsAuthInd;

/*                                                                              

      
                             

             
            

         
                                                             
                                   

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiNmeWpsConfigSetCfm;

/*                                                                              

      
                             

             
                                                                   
              

         
                                                             
                                              

                                                                              */
typedef struct
{
    CsrWifiFsmEvent common;
    CsrResult       status;
} CsrWifiNmeEventMaskSetCfm;

#endif /*                       */

